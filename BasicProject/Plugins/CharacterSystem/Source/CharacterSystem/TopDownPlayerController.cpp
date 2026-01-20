// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownPlayerController.h"

ATopDownPlayerController::ATopDownPlayerController() {
  // Show mouse cursor by default
  bShowMouseCursor = true;
}

void ATopDownPlayerController::BeginPlay() {
  Super::BeginPlay();

  // Set mouse cursor type
  DefaultMouseCursor = DefaultCursorType;

  // Set input mode to allow both game and UI input
  // This allows mouse to be visible while still controlling the game
  FInputModeGameAndUI InputMode;
  InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
  InputMode.SetHideCursorDuringCapture(false);
  SetInputMode(InputMode);
}
