// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterSystemSettings.h"
#include "CharacterSystemConfig.h"

UCharacterSystemSettings::UCharacterSystemSettings() {}

UCharacterSystemSettings *UCharacterSystemSettings::Get() {
  return GetMutableDefault<UCharacterSystemSettings>();
}

UCharacterSystemConfig *UCharacterSystemSettings::GetConfig() const {
  if (CharacterSystemConfig.IsValid()) {
    return Cast<UCharacterSystemConfig>(CharacterSystemConfig.TryLoad());
  }
  return nullptr;
}
