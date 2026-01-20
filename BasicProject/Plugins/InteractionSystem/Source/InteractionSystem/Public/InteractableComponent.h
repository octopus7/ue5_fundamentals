// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "InteractableComponent.generated.h"


class UWidgetComponent;
class UInteractionWidget;

/**
 * Component that makes any actor interactable
 * Attach this to any actor to enable interaction functionality
 * Creates a WidgetComponent for world-space marker display
 *
 * 아무 액터에나 부착하여 상호작용 가능하게 만드는 컴포넌트
 * 월드 스페이스 마커 표시를 위한 WidgetComponent 생성
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UInteractableComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractableComponent();

  //~ Begin UActorComponent Interface
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;
  //~ End UActorComponent Interface

  /**
   * Attempt to interact with this component
   * Called by InteractionDetectorComponent when player triggers interaction
   */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void TryInteract(AActor *InteractionInstigator);

  /** Get the display text for UI */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  FText GetDisplayText() const { return DisplayText; }

  /** Get the interaction type */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  FName GetInteractionType() const { return InteractionType; }

  /** Check if interaction is currently possible */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool CanInteract() const { return bCanInteract; }

  /** Enable or disable interaction */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetCanInteract(bool bNewCanInteract);

  //~ Visibility state management (called by InteractionDetectorComponent)

  /** Set visibility state (within visibility radius) */
  void SetVisible(bool bNewVisible);

  /** Set interactable state (within interaction radius + is current target) */
  void SetInteractable(bool bNewInteractable);

  /** Get current marker opacity (for animation) */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  float GetMarkerOpacity() const { return CurrentMarkerOpacity; }

  /** Check if marker is visible */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool IsMarkerVisible() const { return bIsVisible; }

  /** Check if this is the current interaction target */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool IsCurrentTarget() const { return bIsInteractable; }

public:
  /** Identifier for the type of interaction */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FName InteractionType = TEXT("Default");

  /** Text displayed in the interaction widget */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FText DisplayText = NSLOCTEXT("Interaction", "DefaultText", "Interact");

  /** If true, processes interaction locally via OnInteractLocal delegate */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  bool bHandleLocally = true;

  /** Whether this component can currently be interacted with */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  bool bCanInteract = true;

  /** Offset for the marker widget position (relative to actor) */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI")
  FVector MarkerOffset = FVector(0.0f, 0.0f, 100.0f);

  /** Called when interaction occurs and bHandleLocally is true */
  UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
  FOnInteractLocal OnInteractLocal;

  /** Global delegate for interaction events */
  static FOnInteractionEvent OnInteractionEvent;

protected:
  /** The widget component for displaying marker */
  UPROPERTY()
  TObjectPtr<UWidgetComponent> MarkerWidgetComponent;

  /** Current visibility state (within visibility radius) */
  bool bIsVisible = false;

  /** Current interactable state (current target) */
  bool bIsInteractable = false;

  /** Animation state */
  float CurrentMarkerOpacity = 0.0f;
  float TargetMarkerOpacity = 0.0f;
  float MarkerScaleVelocity = 0.0f;
  float CurrentMarkerScale = 0.0f;
  float TargetMarkerScale = 0.0f;
  bool bIsAnimating = false;

  /** Update marker animation */
  void UpdateMarkerAnimation(float DeltaTime);

  /** Internal function to build event data */
  FInteractionEventData BuildEventData(AActor *InteractionInstigator) const;
};
