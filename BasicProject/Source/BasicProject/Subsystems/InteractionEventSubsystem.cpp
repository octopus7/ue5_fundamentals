// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionEventSubsystem.h"
#include "Engine/GameInstance.h"
#include "GlobalInteractionSubsystem.h"


void UInteractionEventSubsystem::Initialize(
    FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
  WorldInteractionCount = 0;

  // Register this subsystem as the global interaction router
  FInteractionRouterRegistry::SetRouter(
      TScriptInterface<IInteractionRouter>(this));
}

void UInteractionEventSubsystem::Deinitialize() {
  // Unregister router
  FInteractionRouterRegistry::SetRouter(nullptr);
  OnInteractionReceived.Clear();
  Super::Deinitialize();
}

void UInteractionEventSubsystem::RouteInteraction(
    FInteractionEventData &EventData) {
  WorldInteractionCount++;

  // Broadcast to world-level listeners
  OnInteractionReceived.Broadcast(EventData);

  // Forward to global subsystem for game-wide tracking
  if (UGameInstance *GI = GetWorld()->GetGameInstance()) {
    if (UGlobalInteractionSubsystem *GlobalSub =
            GI->GetSubsystem<UGlobalInteractionSubsystem>()) {
      GlobalSub->TrackInteraction(EventData);
    }
  }
}
