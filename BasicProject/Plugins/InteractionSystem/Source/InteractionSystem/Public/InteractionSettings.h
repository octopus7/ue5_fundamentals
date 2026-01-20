// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InteractionSettings.generated.h"

class UUserWidget;

/**
 * Interaction System Settings
 * Configurable via Project Settings → Plugins → Interaction System
 *
 * 상호작용 시스템 설정
 * 프로젝트 설정 → 플러그인 → Interaction System 에서 설정 가능
 */
UCLASS(Config = Game, DefaultConfig,
       meta = (DisplayName = "Interaction System"))
class INTERACTIONSYSTEM_API UInteractionSettings : public UDeveloperSettings {
  GENERATED_BODY()

public:
  UInteractionSettings();

  /** Get the singleton settings instance */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  static UInteractionSettings *Get();

  //~ Begin UDeveloperSettings Interface
  virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
  virtual FName GetSectionName() const override {
    return TEXT("Interaction System");
  }
#if WITH_EDITOR
  virtual FText GetSectionText() const override {
    return NSLOCTEXT("InteractionSettings", "SectionText",
                     "Interaction System");
  }
  virtual FText GetSectionDescription() const override {
    return NSLOCTEXT("InteractionSettings", "SectionDesc",
                     "Configure the Interaction System plugin settings");
  }
#endif
  //~ End UDeveloperSettings Interface

public:
  /** Default widget class to use for interaction UI
   *  Can be overridden per-component */
  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI",
            meta = (DisplayName = "Default Widget Class"))
  TSoftClassPtr<UUserWidget> DefaultWidgetClass;

  /** Default detection radius in cm */
  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Detection",
            meta = (DisplayName = "Default Detection Radius",
                    ClampMin = "10.0"))
  float DefaultDetectionRadius = 50.0f;

  /** Default fade animation duration in seconds */
  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Animation",
            meta = (DisplayName = "Default Fade Duration", ClampMin = "0.0"))
  float DefaultFadeDuration = 0.3f;

  /** Default animation elasticity (higher = more bounce) */
  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Animation",
            meta = (DisplayName = "Default Animation Elasticity",
                    ClampMin = "0.0", ClampMax = "10.0"))
  float DefaultAnimationElasticity = 2.0f;
};
