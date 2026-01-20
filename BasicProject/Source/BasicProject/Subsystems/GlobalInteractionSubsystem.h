// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalInteractionSubsystem.generated.h"


/**
 * Game Instance Subsystem for global interaction tracking
 * Persists across level loads, used for achievements, statistics, etc.
 *
 * 글로벌 상호작용 추적 서브시스템
 * 레벨 전환에도 유지됨 - 업적, 통계 등에 사용
 */
UCLASS()
class BASICPROJECT_API UGlobalInteractionSubsystem
    : public UGameInstanceSubsystem {
  GENERATED_BODY()

public:
  //~ Begin USubsystem Interface
  virtual bool ShouldCreateSubsystem(UObject *Outer) const override {
    return true;
  }
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;
  //~ End USubsystem Interface

  /**
   * Track an interaction event globally
   * Called by InteractionEventSubsystem after world-level routing
   */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void TrackInteraction(UPARAM(ref) FInteractionEventData &EventData);

  /** Delegate for global interaction events (achievements, analytics, etc.) */
  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnInteractionEvent OnGlobalInteraction;

  /** Get total interaction count across all sessions */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  int32 GetTotalInteractionCount() const { return TotalInteractionCount; }

  /** Get interaction count by type */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  int32 GetInteractionCountByType(FName InteractionType) const;

private:
  /** Total interactions since game start */
  int32 TotalInteractionCount = 0;

  /** Interaction count by type */
  TMap<FName, int32> InteractionCountByType;
};
