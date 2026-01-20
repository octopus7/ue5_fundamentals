// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "InteractableComponent.generated.h"


/**
 * Component that makes any actor interactable
 * Attach this to any actor to enable interaction functionality
 *
 * 아무 액터에나 부착하여 상호작용 가능하게 만드는 컴포넌트
 */
UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UInteractableComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractableComponent();

  //~ Begin UActorComponent Interface
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  //~ End UActorComponent Interface

  /**
   * Attempt to interact with this component
   * Called by InteractionDetectorComponent when player triggers interaction
   *
   * @param InteractionInstigator The actor initiating the interaction
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

public:
  /** Identifier for the type of interaction (e.g., "Door", "Item", "NPC") */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FName InteractionType = TEXT("Default");

  /** Text displayed in the interaction widget */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  FText DisplayText = NSLOCTEXT("Interaction", "DefaultText", "Interact");

  /** If true, processes interaction locally via OnInteractLocal delegate
   *  If false, only broadcasts to global OnInteractionEvent */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  bool bHandleLocally = true;

  /** Whether this component can currently be interacted with */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  bool bCanInteract = true;

  /** Called when interaction occurs and bHandleLocally is true
   *  Bind to this in Blueprint or C++ to handle interaction locally */
  UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
  FOnInteractLocal OnInteractLocal;

  /** Global delegate for interaction events - bind from main project to receive
   * all interactions Use
   * UInteractableComponent::OnInteractionEvent.AddDynamic() to bind */
  static FOnInteractionEvent OnInteractionEvent;

private:
  /** Internal function to build event data */
  FInteractionEventData BuildEventData(AActor *InteractionInstigator) const;
};
