// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.h"
#include "UObject/Interface.h"
#include "InteractionRouter.generated.h"


/**
 * Interface for routing interaction events to external systems
 * MainProject implements this to receive events without circular dependency
 *
 * 상호작용 이벤트를 외부 시스템으로 라우팅하기 위한 인터페이스
 * 순환 의존성 없이 메인 프로젝트에서 이벤트를 받을 수 있음
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionRouter : public UInterface {
  GENERATED_BODY()
};

class INTERACTIONSYSTEM_API IInteractionRouter {
  GENERATED_BODY()

public:
  /**
   * Called when an interaction event should be routed to external systems
   * @param EventData Interaction event data (can be modified by handlers)
   */
  virtual void RouteInteraction(FInteractionEventData &EventData) = 0;
};

/**
 * Static registry for the interaction router
 * MainProject sets the router on startup
 *
 * 상호작용 라우터를 위한 정적 레지스트리
 * 메인 프로젝트가 시작 시 라우터를 설정함
 */
class INTERACTIONSYSTEM_API FInteractionRouterRegistry {
public:
  /** Set the global router (called by MainProject on init) */
  static void SetRouter(TScriptInterface<IInteractionRouter> InRouter);

  /** Get the global router */
  static TScriptInterface<IInteractionRouter> GetRouter();

  /** Route an event through the registered router */
  static void Route(FInteractionEventData &EventData);

private:
  static TScriptInterface<IInteractionRouter> GlobalRouter;
};
