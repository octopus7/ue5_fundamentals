// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionRouter.h"

TScriptInterface<IInteractionRouter> FInteractionRouterRegistry::GlobalRouter;

void FInteractionRouterRegistry::SetRouter(
    TScriptInterface<IInteractionRouter> InRouter) {
  GlobalRouter = InRouter;
}

TScriptInterface<IInteractionRouter> FInteractionRouterRegistry::GetRouter() {
  return GlobalRouter;
}

void FInteractionRouterRegistry::Route(FInteractionEventData &EventData) {
  if (GlobalRouter.GetInterface()) {
    GlobalRouter->RouteInteraction(EventData);
  }
}
