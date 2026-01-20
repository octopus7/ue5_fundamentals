// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlobalInteractionSubsystem.h"

void UGlobalInteractionSubsystem::Initialize(
    FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
  TotalInteractionCount = 0;
  InteractionCountByType.Empty();
}

void UGlobalInteractionSubsystem::Deinitialize() {
  OnGlobalInteraction.Clear();
  Super::Deinitialize();
}

void UGlobalInteractionSubsystem::TrackInteraction(
    FInteractionEventData &EventData) {
  TotalInteractionCount++;

  // Track by type
  int32 &Count = InteractionCountByType.FindOrAdd(EventData.InteractionType);
  Count++;

  // Broadcast to global listeners (achievements, analytics, etc.)
  OnGlobalInteraction.Broadcast(EventData);
}

int32 UGlobalInteractionSubsystem::GetInteractionCountByType(
    FName InteractionType) const {
  const int32 *Count = InteractionCountByType.Find(InteractionType);
  return Count ? *Count : 0;
}
