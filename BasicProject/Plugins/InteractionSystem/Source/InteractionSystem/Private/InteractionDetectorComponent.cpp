// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionDetectorComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "InteractableComponent.h"
#include "InteractionSettings.h"

UInteractionDetectorComponent::UInteractionDetectorComponent() {
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickInterval = 0.0f;

  // Load defaults from settings
  if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
    VisibilityRadius = Settings->DefaultVisibilityRadius;
    InteractionRadius = Settings->DefaultInteractionRadius;
  }
}

void UInteractionDetectorComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionDetectorComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // Update detection based on interval
  TimeSinceLastDetection += DeltaTime;
  if (TimeSinceLastDetection >= DetectionInterval) {
    TimeSinceLastDetection = 0.0f;
    UpdateInteractables();
  }

  // Draw debug circles
  if (bDrawDebug && GetOwner()) {
    const FVector Location = GetOwner()->GetActorLocation();
    // Visibility radius (green)
    DrawDebugCircle(GetWorld(), Location, VisibilityRadius, 64, FColor::Green,
                    false, -1.0f, 0, 2.0f, FVector(1, 0, 0), FVector(0, 1, 0),
                    false);
    // Interaction radius (yellow)
    DrawDebugCircle(GetWorld(), Location, InteractionRadius, 32, FColor::Yellow,
                    false, -1.0f, 0, 2.0f, FVector(1, 0, 0), FVector(0, 1, 0),
                    false);
  }
}

void UInteractionDetectorComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason) {
  // Clear all visible interactables
  for (UInteractableComponent *Interactable : VisibleInteractables) {
    if (Interactable) {
      Interactable->SetVisible(false);
      Interactable->SetInteractable(false);
    }
  }
  VisibleInteractables.Empty();
  CurrentTarget = nullptr;

  Super::EndPlay(EndPlayReason);
}

void UInteractionDetectorComponent::TryInteract() {
  if (CurrentTarget && CurrentTarget->CanInteract()) {
    CurrentTarget->TryInteract(GetOwner());
  }
}

void UInteractionDetectorComponent::ForceUpdateDetection() {
  UpdateInteractables();
}

void UInteractionDetectorComponent::UpdateInteractables() {
  AActor *Owner = GetOwner();
  if (!Owner) {
    SetCurrentTarget(nullptr);
    return;
  }

  const FVector OwnerLocation = Owner->GetActorLocation();
  const float VisibilityRadiusSq = VisibilityRadius * VisibilityRadius;
  const float InteractionRadiusSq = InteractionRadius * InteractionRadius;

  // Track which interactables are still visible
  TSet<UInteractableComponent *> StillVisibleSet;

  // Find nearest for interaction target
  UInteractableComponent *NearestInteractable = nullptr;
  float NearestDistanceSq = InteractionRadiusSq;

  // Scan all actors
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

    const float DistanceSq =
        FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());

    // Check visibility radius
    if (DistanceSq < VisibilityRadiusSq) {
      StillVisibleSet.Add(InteractableComp);

      // Check if newly visible
      if (!VisibleInteractables.Contains(InteractableComp)) {
        InteractableComp->SetVisible(true);
      }

      // Check interaction radius (for current target)
      if (DistanceSq < NearestDistanceSq) {
        NearestDistanceSq = DistanceSq;
        NearestInteractable = InteractableComp;
      }
    }
  }

  // Hide interactables that are no longer visible
  for (int32 i = VisibleInteractables.Num() - 1; i >= 0; --i) {
    UInteractableComponent *Interactable = VisibleInteractables[i];
    if (Interactable && !StillVisibleSet.Contains(Interactable)) {
      Interactable->SetVisible(false);
      Interactable->SetInteractable(false);
      VisibleInteractables.RemoveAt(i);
    }
  }

  // Add newly visible interactables to the list
  for (UInteractableComponent *Interactable : StillVisibleSet) {
    if (!VisibleInteractables.Contains(Interactable)) {
      VisibleInteractables.Add(Interactable);
    }
  }

  // Update interaction target
  SetCurrentTarget(NearestInteractable);
}

void UInteractionDetectorComponent::SetCurrentTarget(
    UInteractableComponent *NewTarget) {
  if (CurrentTarget != NewTarget) {
    // Deactivate old target - text panel hides immediately
    if (CurrentTarget) {
      CurrentTarget->SetInteractable(false);
    }

    // Activate new target
    CurrentTarget = NewTarget;
    if (CurrentTarget) {
      CurrentTarget->SetInteractable(true);
    }

    OnTargetChanged.Broadcast(NewTarget);
  }
}
