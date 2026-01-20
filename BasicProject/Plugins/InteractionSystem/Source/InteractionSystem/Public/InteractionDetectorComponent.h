// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "InteractionDetectorComponent.generated.h"

class UInteractableComponent;
class UUserWidget;
class UWidgetComponent;

/**
 * Component that detects nearby interactable objects and manages UI feedback
 * Attach this to the player character/pawn to enable interaction detection
 *
 * 주변 상호작용 대상을 탐지하고 UI 피드백을 관리하는 컴포넌트
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

  /**
   * Attempt to interact with the current target
   * Call this from your input binding (e.g., when E key is pressed)
   */
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
  /** Radius for detecting interactable objects (in cm) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Interaction|Detection", meta = (ClampMin = "10.0"))
  float DetectionRadius = 50.0f;

  /** How often to check for nearby interactables (0 = every tick) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Interaction|Detection", meta = (ClampMin = "0.0"))
  float DetectionInterval = 0.1f;

  /** Widget class to display when an interactable is detected */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI")
  TSubclassOf<UUserWidget> WidgetClass;

  /** Duration of fade in/out animation in seconds */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI",
            meta = (ClampMin = "0.0"))
  float FadeDuration = 0.3f;

  /** Elasticity of the animation (higher = more bounce) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI",
            meta = (ClampMin = "0.0", ClampMax = "10.0"))
  float AnimationElasticity = 2.0f;

  /** Called when the interaction target changes */
  UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
  FOnInteractionTargetChanged OnTargetChanged;

  /** Get the current widget opacity (for fade animation) */
  UFUNCTION(BlueprintPure, Category = "Interaction|UI")
  float GetCurrentOpacity() const { return CurrentOpacity; }

  /** Get the current widget scale (for elastic animation) */
  UFUNCTION(BlueprintPure, Category = "Interaction|UI")
  float GetCurrentScale() const { return CurrentScale; }

  /** Get the current interaction display text */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  FText GetCurrentDisplayText() const;

protected:
  /** Find the nearest interactable within detection radius */
  void UpdateNearestInteractable();

  /** Update the widget visibility and content */
  void UpdateWidget();

  /** Show the interaction widget */
  void ShowWidget();

  /** Hide the interaction widget */
  void HideWidget();

  /** Set the current target and fire events */
  void SetCurrentTarget(UInteractableComponent *NewTarget);

private:
  /** Currently detected interactable component */
  UPROPERTY()
  TObjectPtr<UInteractableComponent> CurrentTarget;

  /** The widget instance for displaying interaction prompt */
  UPROPERTY()
  TObjectPtr<UUserWidget> WidgetInstance;

  /** Timer handle for detection updates */
  float TimeSinceLastDetection = 0.0f;

  /** Current opacity for fade animation (0.0 - 1.0) */
  float CurrentOpacity = 0.0f;

  /** Target opacity for fade animation */
  float TargetOpacity = 0.0f;

  /** Current scale for elastic animation (0.0 - 1.0+) */
  float CurrentScale = 0.0f;

  /** Target scale for animation */
  float TargetScale = 0.0f;

  /** Velocity for elastic animation */
  float ScaleVelocity = 0.0f;

  /** Whether animation is currently playing */
  bool bIsAnimating = false;

  /** Update elastic animation (opacity + scale) */
  void UpdateElasticAnimation(float DeltaTime);
};
