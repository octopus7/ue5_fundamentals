// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionSettings.h"

UInteractionSettings::UInteractionSettings() {
  // Default values are set in header via UPROPERTY defaults
}

UInteractionSettings *UInteractionSettings::Get() {
  return GetMutableDefault<UInteractionSettings>();
}
