// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionTypes.h"
#include "TestEventReceiver.generated.h"

class UTextRenderComponent;

/**
 * Test actor that receives interaction events and displays them as text
 * Used for debugging and testing the interaction event routing system
 *
 * 상호작용 이벤트를 받아서 텍스트로 표시하는 테스트 액터
 * 이벤트 라우팅 시스템 디버깅 및 테스트용
 */
UCLASS()
class BASICPROJECT_API ATestEventReceiver : public AActor {
  GENERATED_BODY()

public:
  ATestEventReceiver();

protected:
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  /** Called when an interaction event is received */
  UFUNCTION()
  void OnInteractionReceived(FInteractionEventData &EventData);

  /** Display text above the actor */
  void DisplayEventText(const FString &Text);

  /** Clear the displayed text after a delay */
  void ClearEventText();

protected:
  /** Text component to display events */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<UTextRenderComponent> TextDisplay;

  /** How long to display the event text */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
  float DisplayDuration = 3.0f;

  /** Timer handle for clearing text */
  FTimerHandle ClearTextTimerHandle;
};
