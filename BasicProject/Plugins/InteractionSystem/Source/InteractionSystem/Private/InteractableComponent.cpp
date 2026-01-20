// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractableComponent.h"
#include "Engine/Engine.h"

// Initialize static delegate
FOnInteractionEvent UInteractableComponent::OnInteractionEvent;

UInteractableComponent::UInteractableComponent() {
  PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
}

void UInteractableComponent::TryInteract(AActor *InteractionInstigator) {
  if (!bCanInteract) {
    return;
  }

  FInteractionEventData EventData = BuildEventData(InteractionInstigator);

  // Step 1: Try local handling if enabled
  if (bHandleLocally) {
    OnInteractLocal.Broadcast(EventData);

    // Show debug message for local handling
    if (GEngine) {
      GEngine->AddOnScreenDebugMessage(
          -1, 3.0f, FColor::Green,
          FString::Printf(TEXT("[Local] %s: %s"), *InteractionType.ToString(),
                          *DisplayText.ToString()));
    }
  }

  // Step 2: Broadcast to global listeners if not handled locally
  if (!EventData.bHandled) {
    // Broadcast to main project (non-const to allow setting bHandled)
    OnInteractionEvent.Broadcast(EventData);

    // Step 3: Show unhandled message if no one processed it
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
