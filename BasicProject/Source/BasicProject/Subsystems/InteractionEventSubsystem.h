// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionRouter.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractionEventSubsystem.generated.h"


/**
 * World Subsystem for routing interaction events within a level
 * Implements IInteractionRouter to receive events from InteractionSystem
 *
 * 월드별 상호작용 이벤트 라우팅을 담당하는 서브시스템
 * IInteractionRouter 구현하여 InteractionSystem으로부터 이벤트 수신
 */
UCLASS()
class BASICPROJECT_API UInteractionEventSubsystem : public UWorldSubsystem,
                                                    public IInteractionRouter {
  GENERATED_BODY()

public:
  //~ Begin USubsystem Interface
  virtual bool ShouldCreateSubsystem(UObject *Outer) const override {
    return true;
  }
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;
  //~ End USubsystem Interface

  //~ Begin IInteractionRouter Interface
  virtual void RouteInteraction(FInteractionEventData &EventData) override;
  //~ End IInteractionRouter Interface

  /** Delegate for systems to bind to receive interaction events */
  UPROPERTY(BlueprintAssignable, Category = "Interaction")
  FOnInteractionEvent OnInteractionReceived;

  /** Get world interaction count for this session */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  int32 GetWorldInteractionCount() const { return WorldInteractionCount; }

private:
  /** Count of interactions in this world */
  int32 WorldInteractionCount = 0;
};
