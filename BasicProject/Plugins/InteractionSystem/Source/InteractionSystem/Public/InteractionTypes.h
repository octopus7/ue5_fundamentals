// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.generated.h"

class UInteractableComponent;

/**
 * Interaction event data structure
 * Contains all information about an interaction event
 * 상호작용 이벤트 데이터 구조체
 */
USTRUCT(BlueprintType)
struct INTERACTIONSYSTEM_API FInteractionEventData {
  GENERATED_BODY()

  /** The actor that initiated the interaction (usually the player) */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  AActor *Instigator = nullptr;

  /** The actor that owns the interactable component */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  AActor *TargetActor = nullptr;

  /** The interactable component being interacted with */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  UInteractableComponent *TargetComponent = nullptr;

  /** Identifier for the type of interaction (e.g., "Door", "Item", "NPC") */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  FName InteractionType;

  /** Text to display in the UI widget */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  FText DisplayText;

  /** Whether this event has been handled (stops further propagation if true) */
  UPROPERTY(BlueprintReadWrite, Category = "Interaction")
  bool bHandled = false;
};

/**
 * Delegate for local interaction handling (within the component or its owner)
 * 로컬 상호작용 처리용 델리게이트
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractLocal,
                                            const FInteractionEventData &,
                                            EventData);

/**
 * Delegate for global interaction events (broadcasted to main project)
 * 글로벌 상호작용 이벤트용 델리게이트 (메인 프로젝트로 전파)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEvent,
                                            FInteractionEventData &, EventData);

/**
 * Delegate for when the current interaction target changes
 * 현재 상호작용 대상이 변경될 때 발생하는 델리게이트
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChanged,
                                            UInteractableComponent *,
                                            NewTarget);
