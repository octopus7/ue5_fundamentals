// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractableComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "InteractionRouter.h"
#include "InteractionSettings.h"
#include "InteractionWidget.h"

// Initialize static delegate
FOnInteractionEvent UInteractableComponent::OnInteractionEvent;

UInteractableComponent::UInteractableComponent() {
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickInterval = 0.0f;
}

void UInteractableComponent::BeginPlay() {
  Super::BeginPlay();

  // Get widget class from settings
  TSubclassOf<UUserWidget> WidgetClass = nullptr;
  if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
    WidgetClass = Settings->DefaultWidgetClass.LoadSynchronous();
  }

  if (WidgetClass && GetOwner()) {
    // Create widget component
    MarkerWidgetComponent =
        NewObject<UWidgetComponent>(GetOwner(), TEXT("InteractionMarker"));
    if (MarkerWidgetComponent) {
      MarkerWidgetComponent->SetupAttachment(GetOwner()->GetRootComponent());
      MarkerWidgetComponent->SetRelativeLocation(MarkerOffset);
      MarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
      MarkerWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f));
      MarkerWidgetComponent->SetWidgetClass(WidgetClass);
      MarkerWidgetComponent->SetVisibility(false);
      MarkerWidgetComponent->RegisterComponent();

      // Initialize widget
      if (UUserWidget *Widget = MarkerWidgetComponent->GetWidget()) {
        if (UInteractionWidget *InteractionWidget =
                Cast<UInteractionWidget>(Widget)) {
          InteractionWidget->SetOwnerInteractable(this);
        }
      }
    }
  }
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  if (MarkerWidgetComponent) {
    MarkerWidgetComponent->DestroyComponent();
    MarkerWidgetComponent = nullptr;
  }
  Super::EndPlay(EndPlayReason);
}

void UInteractableComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bIsAnimating) {
    UpdateMarkerAnimation(DeltaTime);
  }
}

void UInteractableComponent::TryInteract(AActor *InteractionInstigator) {
  if (!bCanInteract) {
    return;
  }

  FInteractionEventData EventData = BuildEventData(InteractionInstigator);

  // Step 1: Try local handling if enabled
  if (bHandleLocally) {
    OnInteractLocal.Broadcast(EventData);

    if (GEngine) {
      GEngine->AddOnScreenDebugMessage(
          -1, 3.0f, FColor::Green,
          FString::Printf(TEXT("[Local] %s: %s"), *InteractionType.ToString(),
                          *DisplayText.ToString()));
    }
  }

  // Step 2: Route to global systems via Router if not handled locally
  if (!EventData.bHandled) {
    // Route through the global router (MainProject's subsystem)
    FInteractionRouterRegistry::Route(EventData);

    // Also broadcast to static delegate for backwards compatibility
    OnInteractionEvent.Broadcast(EventData);

    if (!EventData.bHandled && GEngine) {
      GEngine->AddOnScreenDebugMessage(
          -1, 3.0f, FColor::Yellow,
          FString::Printf(TEXT("[Unhandled] %s: %s"),
                          *InteractionType.ToString(),
                          *DisplayText.ToString()));
    }
  }
}

void UInteractableComponent::SetCanInteract(bool bNewCanInteract) {
  bCanInteract = bNewCanInteract;
}

void UInteractableComponent::SetVisible(bool bNewVisible) {
  if (bIsVisible == bNewVisible) {
    return;
  }

  bIsVisible = bNewVisible;

  if (bNewVisible) {
    // Start fade in animation
    TargetMarkerOpacity = 1.0f;
    TargetMarkerScale = 1.0f;
    bIsAnimating = true;

    if (MarkerWidgetComponent) {
      MarkerWidgetComponent->SetVisibility(true);
    }
  } else {
    // Start fade out animation
    TargetMarkerOpacity = 0.0f;
    TargetMarkerScale = 0.0f;
    bIsAnimating = true;

    // Also hide text panel immediately when leaving visibility
    bIsInteractable = false;
  }
}

void UInteractableComponent::SetInteractable(bool bNewInteractable) {
  if (bIsInteractable == bNewInteractable) {
    return;
  }

  bIsInteractable = bNewInteractable;

  // Update widget text panel visibility
  if (MarkerWidgetComponent) {
    if (UUserWidget *Widget = MarkerWidgetComponent->GetWidget()) {
      if (UInteractionWidget *InteractionWidget =
              Cast<UInteractionWidget>(Widget)) {
        if (bNewInteractable) {
          InteractionWidget->ShowTextPanel();
        } else {
          InteractionWidget->HideTextPanelImmediate();
        }
      }
    }
  }
}

void UInteractableComponent::UpdateMarkerAnimation(float DeltaTime) {
  if (!MarkerWidgetComponent) {
    bIsAnimating = false;
    return;
  }

  // Get animation settings
  float FadeDuration = 0.3f;
  float Elasticity = 2.0f;
  if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
    FadeDuration = Settings->DefaultFadeDuration;
    Elasticity = Settings->DefaultAnimationElasticity;
  }

  // Spring parameters
  const float SpringStiffness = 300.0f;
  const float Damping = 15.0f + (10.0f - Elasticity) * 2.0f;

  // Scale animation (elastic)
  const float ScaleDisplacement = TargetMarkerScale - CurrentMarkerScale;
  const float SpringForce = ScaleDisplacement * SpringStiffness;
  const float DampingForce = -MarkerScaleVelocity * Damping;
  const float Acceleration = SpringForce + DampingForce;

  MarkerScaleVelocity += Acceleration * DeltaTime;
  CurrentMarkerScale += MarkerScaleVelocity * DeltaTime;
  CurrentMarkerScale = FMath::Max(0.0f, CurrentMarkerScale);

  // Opacity animation (linear)
  const float FadeSpeed = FadeDuration > 0.0f ? 1.0f / FadeDuration : 100.0f;
  if (CurrentMarkerOpacity < TargetMarkerOpacity) {
    CurrentMarkerOpacity = FMath::Min(
        CurrentMarkerOpacity + FadeSpeed * DeltaTime, TargetMarkerOpacity);
  } else if (CurrentMarkerOpacity > TargetMarkerOpacity) {
    CurrentMarkerOpacity = FMath::Max(
        CurrentMarkerOpacity - FadeSpeed * DeltaTime, TargetMarkerOpacity);
  }

  // Apply to marker elements via InteractionWidget
  if (UUserWidget *Widget = MarkerWidgetComponent->GetWidget()) {
    if (UInteractionWidget *InteractionWidget =
            Cast<UInteractionWidget>(Widget)) {
      InteractionWidget->SetMarkerOpacity(CurrentMarkerOpacity);
      InteractionWidget->SetMarkerScale(CurrentMarkerScale);
    }
  }

  // Check if animation complete
  const bool bScaleSettled =
      FMath::IsNearlyEqual(CurrentMarkerScale, TargetMarkerScale, 0.01f) &&
      FMath::Abs(MarkerScaleVelocity) < 0.1f;
  const bool bOpacitySettled =
      FMath::IsNearlyEqual(CurrentMarkerOpacity, TargetMarkerOpacity, 0.01f);

  if (bScaleSettled && bOpacitySettled) {
    CurrentMarkerScale = TargetMarkerScale;
    CurrentMarkerOpacity = TargetMarkerOpacity;
    MarkerScaleVelocity = 0.0f;
    bIsAnimating = false;

    // Hide completely when faded out
    if (TargetMarkerScale == 0.0f && TargetMarkerOpacity == 0.0f) {
      MarkerWidgetComponent->SetVisibility(false);
    }
  }
}

FInteractionEventData
UInteractableComponent::BuildEventData(AActor *InteractionInstigator) const {
  FInteractionEventData EventData;
  EventData.Instigator = InteractionInstigator;
  EventData.TargetActor = GetOwner();
  EventData.TargetComponent = const_cast<UInteractableComponent *>(this);
  EventData.InteractionType = InteractionType;
  EventData.DisplayText = DisplayText;
  EventData.bHandled = false;

  return EventData;
}
