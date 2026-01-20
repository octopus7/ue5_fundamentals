// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CharacterSystemSettings.generated.h"


class UCharacterSystemConfig;

/**
 * Developer Settings for Character System.
 * Accessible via Project Settings -> Plugins -> Character System.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Character System"))
class CHARACTERSYSTEM_API UCharacterSystemSettings : public UDeveloperSettings {
  GENERATED_BODY()

public:
  UCharacterSystemSettings();

  // ============================================
  // Settings
  // ============================================

  /** Character System Configuration Data Asset */
  UPROPERTY(
      Config, EditAnywhere, BlueprintReadOnly, Category = "Configuration",
      meta = (AllowedClasses = "/Script/CharacterSystem.CharacterSystemConfig"))
  FSoftObjectPath CharacterSystemConfig;

public:
  // ============================================
  // Accessors
  // ============================================

  /** Get the singleton settings instance */
  static UCharacterSystemSettings *Get();

  /** Get the Character System Config Data Asset (loads if needed) */
  UFUNCTION(BlueprintCallable, Category = "Character System")
  UCharacterSystemConfig *GetConfig() const;

  // ============================================
  // UDeveloperSettings Interface
  // ============================================

  virtual FName GetCategoryName() const override { return FName("Plugins"); }
  virtual FName GetSectionName() const override {
    return FName("Character System");
  }

#if WITH_EDITOR
  virtual FText GetSectionText() const override {
    return NSLOCTEXT("CharacterSystem", "SettingsSection", "Character System");
  }
  virtual FText GetSectionDescription() const override {
    return NSLOCTEXT("CharacterSystem", "SettingsDesc",
                     "Configure the Character System plugin");
  }
#endif
};
