// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCharacter.h"
#include "Camera/CameraComponent.h"
#include "CharacterSystemConfig.h"
#include "CharacterSystemSettings.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"


ATopDownCharacter::ATopDownCharacter() {
  PrimaryActorTick.bCanEverTick = true;

  // Don't rotate when the controller rotates (we handle rotation ourselves)
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // Configure character movement for top-down
  GetCharacterMovement()->bOrientRotationToMovement = false;
  GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
  GetCharacterMovement()->bConstrainToPlane = true;
  GetCharacterMovement()->bSnapToPlaneAtStart = true;

  // Create camera boom (spring arm)
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->SetUsingAbsoluteRotation(true);
  CameraBoom->TargetArmLength = CameraArmLength;
  CameraBoom->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
  CameraBoom->bDoCollisionTest = false; // No collision for fixed camera

  // Create follow camera
  TopDownCamera =
      CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
  TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  TopDownCamera->bUsePawnControlRotation =
      false; // Camera does not rotate relative to arm
}

void ATopDownCharacter::BeginPlay() {
  Super::BeginPlay();

  // Load configuration from Data Asset
  LoadConfiguration();

  // Apply camera settings
  ApplyCameraSettings();

  // Add Input Mapping Context from config
  if (APlayerController *PlayerController =
          Cast<APlayerController>(Controller)) {
    if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer())) {
      if (UCharacterSystemConfig *Config = GetConfig()) {
        if (UInputMappingContext *IMC =
                Config->DefaultMappingContext.LoadSynchronous()) {
          Subsystem->AddMappingContext(IMC, 0);
        }
      }
    }
  }
}

void ATopDownCharacter::LoadConfiguration() {
  // Priority: ConfigOverride > Project Settings
  if (ConfigOverride) {
    CachedConfig = ConfigOverride;
  } else {
    CachedConfig = UCharacterSystemSettings::Get()->GetConfig();
  }

  // Apply settings from config
  if (CachedConfig) {
    CameraArmLength = CachedConfig->CameraArmLength;
    CameraPitch = CachedConfig->CameraPitch;
    RotationInterpSpeed = CachedConfig->RotationInterpSpeed;
  }
}

void ATopDownCharacter::ApplyCameraSettings() {
  if (CameraBoom) {
    CameraBoom->TargetArmLength = CameraArmLength;
    CameraBoom->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
  }
}

UCharacterSystemConfig *ATopDownCharacter::GetConfig() const {
  return CachedConfig;
}

void ATopDownCharacter::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  UpdateCharacterRotation();
}

void ATopDownCharacter::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent) {
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  // Set up action bindings with Enhanced Input
  if (UEnhancedInputComponent *EnhancedInputComponent =
          Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
    // Get move action from config
    if (UCharacterSystemConfig *Config = GetConfig()) {
      if (UInputAction *MoveAction = Config->MoveAction.LoadSynchronous()) {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
                                           this, &ATopDownCharacter::Move);
      }
    }
  }
}

void ATopDownCharacter::Move(const FInputActionValue &Value) {
  // Input is a Vector2D
  FVector2D MovementVector = Value.Get<FVector2D>();

  if (Controller != nullptr) {
    // World-space movement (top-down: X is forward/back, Y is left/right)
    // WASD: W/S affect Y (forward/back), A/D affect X (left/right)
    const FVector MoveDirection =
        FVector(MovementVector.Y, MovementVector.X, 0.f);

    // Add movement input in world space
    AddMovementInput(MoveDirection, 1.0f, false);
  }
}

void ATopDownCharacter::UpdateCharacterRotation() {
  FVector MouseLocation;
  if (GetMouseWorldLocation(MouseLocation)) {
    // Calculate direction from character to mouse
    FVector Direction = MouseLocation - GetActorLocation();
    Direction.Z = 0.f;

    if (!Direction.IsNearlyZero()) {
      // Calculate target rotation
      FRotator TargetRotation = Direction.Rotation();

      // Smoothly interpolate to target rotation
      FRotator NewRotation =
          FMath::RInterpTo(GetActorRotation(), TargetRotation,
                           GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);

      SetActorRotation(NewRotation);
    }
  }
}

bool ATopDownCharacter::GetMouseWorldLocation(FVector &OutLocation) const {
  APlayerController *PC = Cast<APlayerController>(GetController());
  if (!PC) {
    return false;
  }

  FHitResult HitResult;
  if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult)) {
    OutLocation = HitResult.Location;
    return true;
  }

  return false;
}
