// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TopDownPlayerController.generated.h"

/**
 * Player controller for top-down gameplay.
 * Handles mouse cursor visibility and input mode setup.
 */
UCLASS()
class CHARACTERSYSTEM_API ATopDownPlayerController : public APlayerController {
  GENERATED_BODY()

public:
  ATopDownPlayerController();

protected:
  virtual void BeginPlay() override;

protected:
  // ============================================
  // Cursor Settings
  // ============================================

  /** Mouse cursor type to use */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
  TEnumAsByte<EMouseCursor::Type> DefaultCursorType = EMouseCursor::Crosshairs;
};
