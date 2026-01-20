// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "InteractionDetectorComponent.generated.h"

class UInteractableComponent;

/**
 * Component that detects nearby interactable objects and manages
 * visibility/interaction states Attach this to the player character/pawn
 *
 * 주변 상호작용 대상을 탐지하고 가시성/상호작용 상태를 관리
 * 플레이어 캐릭터/폰에 부착하여 사용
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UInteractionDetectorComponent
    : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractionDetectorComponent();

  //~ Begin UActorComponent Interface
  virtual void BeginPlay() override;
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  //~ End UActorComponent Interface

  /** Attempt to interact with the current target */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void TryInteract();

  /** Get the currently targeted interactable component */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  UInteractableComponent *GetCurrentTarget() const { return CurrentTarget; }

  /** Check if there is a valid interaction target */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool HasValidTarget() const { return CurrentTarget != nullptr; }

  /** Manually trigger a detection update */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void ForceUpdateDetection();

public:
  /** Radius for visibility (marker appears) in cm */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Interaction|Detection", meta = (ClampMin = "10.0"))
  float VisibilityRadius = 300.0f;

  /** Radius for interaction (text panel appears, can interact) in cm */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Interaction|Detection", meta = (ClampMin = "10.0"))
  float InteractionRadius = 150.0f;

  /** How often to check for nearby interactables (0 = every tick) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Interaction|Detection", meta = (ClampMin = "0.0"))
  float DetectionInterval = 0.1f;

  /** Draw debug circles for visibility/interaction radius */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
  bool bDrawDebug = false;

  /** Called when the interaction target changes */
  UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
  FOnInteractionTargetChanged OnTargetChanged;

protected:
  /** Update visibility and interaction states for all interactables */
  void UpdateInteractables();

  /** Set the current target and fire events */
  void SetCurrentTarget(UInteractableComponent *NewTarget);

private:
  /** Currently targeted interactable (within InteractionRadius) */
  UPROPERTY()
  TObjectPtr<UInteractableComponent> CurrentTarget;

  /** All currently visible interactables (within VisibilityRadius) */
  UPROPERTY()
  TArray<TObjectPtr<UInteractableComponent>> VisibleInteractables;

  /** Timer handle for detection updates */
  float TimeSinceLastDetection = 0.0f;
};
