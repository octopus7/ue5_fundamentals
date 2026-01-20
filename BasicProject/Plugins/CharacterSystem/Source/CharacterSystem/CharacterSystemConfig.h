// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSystemConfig.generated.h"


class UInputMappingContext;
class UInputAction;

/**
 * Configuration Data Asset for Character System.
 * Holds references to Input Actions and Mapping Contexts.
 */
UCLASS(BlueprintType)
class CHARACTERSYSTEM_API UCharacterSystemConfig : public UPrimaryDataAsset {
  GENERATED_BODY()

public:
  // ============================================
  // Input Configuration
  // ============================================

  /** Default Input Mapping Context for top-down movement */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  TSoftObjectPtr<UInputMappingContext> DefaultMappingContext;

  /** Move Input Action */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
  TSoftObjectPtr<UInputAction> MoveAction;

  // ============================================
  // Camera Settings
  // ============================================

  /** Camera arm length (distance from character) */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
  float CameraArmLength = 2000.f;

  /** Camera pitch angle in degrees (negative = looking down) */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera",
            meta = (ClampMin = "-89.0", ClampMax = "0.0"))
  float CameraPitch = -45.f;

  // ============================================
  // Movement Settings
  // ============================================

  /** How fast the character rotates toward the mouse cursor */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
  float RotationInterpSpeed = 15.f;

public:
  // ============================================
  // Asset Info
  // ============================================

  virtual FPrimaryAssetId GetPrimaryAssetId() const override {
    return FPrimaryAssetId("CharacterSystemConfig", GetFName());
  }
};
