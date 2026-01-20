// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TopDownCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterSystemConfig;

/**
 * Top-down character with fixed camera and mouse cursor rotation.
 * Designed for top-down games with WASD movement and mouse aiming.
 * Configuration is loaded from CharacterSystemConfig Data Asset via Project
 * Settings.
 */
UCLASS()
class CHARACTERSYSTEM_API ATopDownCharacter : public ACharacter {
  GENERATED_BODY()

public:
  ATopDownCharacter();

protected:
  virtual void BeginPlay() override;

public:
  virtual void Tick(float DeltaTime) override;
  virtual void SetupPlayerInputComponent(
      class UInputComponent *PlayerInputComponent) override;

protected:
  // ============================================
  // Camera Components
  // ============================================

  /** Camera boom positioning the camera behind the character */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
            meta = (AllowPrivateAccess = "true"))
  TObjectPtr<USpringArmComponent> CameraBoom;

  /** Follow camera */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
            meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UCameraComponent> TopDownCamera;

  // ============================================
  // Configuration (from Data Asset or override)
  // ============================================

  /** Override: Use this config instead of Project Settings config */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
  TObjectPtr<UCharacterSystemConfig> ConfigOverride;

  /** Cached config reference */
  UPROPERTY(Transient)
  TObjectPtr<UCharacterSystemConfig> CachedConfig;

  // ============================================
  // Runtime Settings (can be modified at runtime)
  // ============================================

  /** Camera arm length (initialized from config) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Settings")
  float CameraArmLength = 2000.f;

  /** Camera pitch angle in degrees (initialized from config) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Settings",
            meta = (ClampMin = "-89.0", ClampMax = "0.0"))
  float CameraPitch = -45.f;

  /** How fast the character rotates toward the mouse cursor (initialized from
   * config) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
  float RotationInterpSpeed = 15.f;

  // ============================================
  // Internal Methods
  // ============================================

  /** Load and apply configuration from Data Asset */
  void LoadConfiguration();

  /** Apply settings to components */
  void ApplyCameraSettings();

  // ============================================
  // Input Handlers
  // ============================================

  /** Called for movement input */
  void Move(const FInputActionValue &Value);

  // ============================================
  // Rotation
  // ============================================

  /** Update character rotation to face mouse cursor */
  void UpdateCharacterRotation();

  /** Get the world location under the mouse cursor */
  bool GetMouseWorldLocation(FVector &OutLocation) const;

public:
  // ============================================
  // Accessors
  // ============================================

  /** Get the active configuration */
  UFUNCTION(BlueprintCallable, Category = "Character System")
  UCharacterSystemConfig *GetConfig() const;

  /** Returns CameraBoom subobject */
  FORCEINLINE USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

  /** Returns TopDownCamera subobject */
  FORCEINLINE UCameraComponent *GetTopDownCamera() const {
    return TopDownCamera;
  }
};
