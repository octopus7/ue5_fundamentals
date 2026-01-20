// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionDetectorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScaleBox.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "InteractableComponent.h"
#include "InteractionSettings.h"
#include "Kismet/GameplayStatics.h"

UInteractionDetectorComponent::UInteractionDetectorComponent() {
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickInterval = 0.0f;

  // Load defaults from settings
  if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
    DetectionRadius = Settings->DefaultDetectionRadius;
    FadeDuration = Settings->DefaultFadeDuration;
    AnimationElasticity = Settings->DefaultAnimationElasticity;
  }
}

void UInteractionDetectorComponent::BeginPlay() {
  Super::BeginPlay();

  // If no widget class set, try to load from settings
  TSubclassOf<UUserWidget> WidgetToUse = WidgetClass;
  if (!WidgetToUse) {
    if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
      WidgetToUse = Settings->DefaultWidgetClass.LoadSynchronous();
    }
  }

  // Create widget instance if widget class is available
  if (WidgetToUse) {
    APlayerController *PC =
        Cast<APlayerController>(GetOwner()->GetInstigatorController());
    if (!PC) {
      PC = UGameplayStatics::GetPlayerController(this, 0);
    }

    if (PC) {
      WidgetInstance = CreateWidget<UUserWidget>(PC, WidgetToUse);
      if (WidgetInstance) {
        WidgetInstance->AddToViewport();
        WidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
        WidgetInstance->SetRenderOpacity(0.0f);
        WidgetInstance->SetRenderScale(FVector2D(0.0f, 0.0f));
      }
    }
  }
}

void UInteractionDetectorComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // Update detection based on interval
  TimeSinceLastDetection += DeltaTime;
  if (TimeSinceLastDetection >= DetectionInterval) {
    TimeSinceLastDetection = 0.0f;
    UpdateNearestInteractable();
  }

  // Update elastic animation
  if (bIsAnimating) {
    UpdateElasticAnimation(DeltaTime);
  }
}

void UInteractionDetectorComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason) {
  // Clean up widget
  if (WidgetInstance) {
    WidgetInstance->RemoveFromParent();
    WidgetInstance = nullptr;
  }

  CurrentTarget = nullptr;

  Super::EndPlay(EndPlayReason);
}

void UInteractionDetectorComponent::TryInteract() {
  if (CurrentTarget && CurrentTarget->CanInteract()) {
    CurrentTarget->TryInteract(GetOwner());
  }
}

void UInteractionDetectorComponent::ForceUpdateDetection() {
  UpdateNearestInteractable();
}

FText UInteractionDetectorComponent::GetCurrentDisplayText() const {
  if (CurrentTarget) {
    return CurrentTarget->GetDisplayText();
  }
  return FText::GetEmpty();
}

void UInteractionDetectorComponent::UpdateNearestInteractable() {
  AActor *Owner = GetOwner();
  if (!Owner) {
    SetCurrentTarget(nullptr);
    return;
  }

  const FVector OwnerLocation = Owner->GetActorLocation();
  UInteractableComponent *NearestInteractable = nullptr;
  float NearestDistanceSquared = DetectionRadius * DetectionRadius;

  // Use simple distance check for all actors with interactable components
  for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
    AActor *Actor = *It;
    if (Actor == Owner) {
      continue;
    }

    UInteractableComponent *InteractableComp =
        Actor->FindComponentByClass<UInteractableComponent>();
    if (!InteractableComp || !InteractableComp->CanInteract()) {
      continue;
    }

    const float DistanceSquared =
        FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());
    if (DistanceSquared < NearestDistanceSquared) {
      NearestDistanceSquared = DistanceSquared;
      NearestInteractable = InteractableComp;
    }
  }

  SetCurrentTarget(NearestInteractable);
}

void UInteractionDetectorComponent::UpdateWidget() {
  if (!WidgetInstance) {
    return;
  }

  if (CurrentTarget) {
    ShowWidget();
  } else {
    HideWidget();
  }
}

void UInteractionDetectorComponent::ShowWidget() {
  if (WidgetInstance) {
    WidgetInstance->SetVisibility(ESlateVisibility::Visible);
    TargetOpacity = 1.0f;
    TargetScale = 1.0f;
    bIsAnimating = true;
  }
}

void UInteractionDetectorComponent::HideWidget() {
  if (WidgetInstance) {
    TargetOpacity = 0.0f;
    TargetScale = 0.0f;
    bIsAnimating = true;
    // Note: Visibility will be set to Collapsed when animation completes
  }
}

void UInteractionDetectorComponent::UpdateElasticAnimation(float DeltaTime) {
  if (!WidgetInstance) {
    bIsAnimating = false;
    return;
  }

  // Spring damping parameters
  const float SpringStiffness = 300.0f;
  const float Damping = 15.0f + (10.0f - AnimationElasticity) * 2.0f;

  // ===== Scale Animation (Elastic/Spring) =====
  const float ScaleDisplacement = TargetScale - CurrentScale;
  const float SpringForce = ScaleDisplacement * SpringStiffness;
  const float DampingForce = -ScaleVelocity * Damping;
  const float Acceleration = SpringForce + DampingForce;

  ScaleVelocity += Acceleration * DeltaTime;
  CurrentScale += ScaleVelocity * DeltaTime;
  CurrentScale = FMath::Max(0.0f, CurrentScale);

  // ===== Opacity Animation (Linear) =====
  const float FadeSpeed = FadeDuration > 0.0f ? 1.0f / FadeDuration : 100.0f;
  if (CurrentOpacity < TargetOpacity) {
    CurrentOpacity =
        FMath::Min(CurrentOpacity + FadeSpeed * DeltaTime, TargetOpacity);
  } else if (CurrentOpacity > TargetOpacity) {
    CurrentOpacity =
        FMath::Max(CurrentOpacity - FadeSpeed * DeltaTime, TargetOpacity);
  }

  // Apply to widget
  WidgetInstance->SetRenderOpacity(CurrentOpacity);
  WidgetInstance->SetRenderScale(FVector2D(CurrentScale, CurrentScale));

  // Check if animation is complete
  const bool bScaleSettled =
      FMath::IsNearlyEqual(CurrentScale, TargetScale, 0.01f) &&
      FMath::Abs(ScaleVelocity) < 0.1f;
  const bool bOpacitySettled =
      FMath::IsNearlyEqual(CurrentOpacity, TargetOpacity, 0.01f);

  if (bScaleSettled && bOpacitySettled) {
    CurrentScale = TargetScale;
    CurrentOpacity = TargetOpacity;
    ScaleVelocity = 0.0f;
    bIsAnimating = false;

    // Hide widget completely when animation completes at zero
    if (TargetScale == 0.0f && TargetOpacity == 0.0f) {
      WidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
    }
  }
}

void UInteractionDetectorComponent::SetCurrentTarget(
    UInteractableComponent *NewTarget) {
  if (CurrentTarget != NewTarget) {
    CurrentTarget = NewTarget;
    OnTargetChanged.Broadcast(NewTarget);
    UpdateWidget();
  }
}
