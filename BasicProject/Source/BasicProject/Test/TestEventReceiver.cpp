// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestEventReceiver.h"
#include "../Subsystems/InteractionEventSubsystem.h"
#include "Components/TextRenderComponent.h"
#include "TimerManager.h"


ATestEventReceiver::ATestEventReceiver() {
  PrimaryActorTick.bCanEverTick = false;

  // Create root component
  USceneComponent *Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);

  // Create text display component
  TextDisplay =
      CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextDisplay"));
  TextDisplay->SetupAttachment(Root);
  TextDisplay->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
  TextDisplay->SetHorizontalAlignment(EHTA_Center);
  TextDisplay->SetVerticalAlignment(EVRTA_TextCenter);
  TextDisplay->SetWorldSize(24.0f);
  TextDisplay->SetTextRenderColor(FColor::Yellow);
  TextDisplay->SetText(FText::GetEmpty());
}

void ATestEventReceiver::BeginPlay() {
  Super::BeginPlay();

  // Bind to the world subsystem's interaction event
  if (UWorld *World = GetWorld()) {
    if (UInteractionEventSubsystem *Subsystem =
            World->GetSubsystem<UInteractionEventSubsystem>()) {
      Subsystem->OnInteractionReceived.AddDynamic(
          this, &ATestEventReceiver::OnInteractionReceived);
    }
  }

  // Show ready message
  DisplayEventText(TEXT("Ready to receive events..."));
}

void ATestEventReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  // Unbind from subsystem
  if (UWorld *World = GetWorld()) {
    if (UInteractionEventSubsystem *Subsystem =
            World->GetSubsystem<UInteractionEventSubsystem>()) {
      Subsystem->OnInteractionReceived.RemoveDynamic(
          this, &ATestEventReceiver::OnInteractionReceived);
    }
  }

  // Clear timer
  GetWorldTimerManager().ClearTimer(ClearTextTimerHandle);

  Super::EndPlay(EndPlayReason);
}

void ATestEventReceiver::OnInteractionReceived(
    FInteractionEventData &EventData) {
  // Build display string
  FString DisplayString = FString::Printf(
      TEXT("[%s] %s\nFrom: %s"), *EventData.InteractionType.ToString(),
      *EventData.DisplayText.ToString(),
      EventData.TargetActor ? *EventData.TargetActor->GetName()
                            : TEXT("Unknown"));

  DisplayEventText(DisplayString);

  // Mark as handled (optional - for testing)
  // EventData.bHandled = true;
}

void ATestEventReceiver::DisplayEventText(const FString &Text) {
  if (TextDisplay) {
    TextDisplay->SetText(FText::FromString(Text));
  }

  // Set timer to clear text
  GetWorldTimerManager().ClearTimer(ClearTextTimerHandle);
  GetWorldTimerManager().SetTimer(ClearTextTimerHandle, this,
                                  &ATestEventReceiver::ClearEventText,
                                  DisplayDuration, false);
}

void ATestEventReceiver::ClearEventText() {
  if (TextDisplay) {
    TextDisplay->SetText(FText::GetEmpty());
  }
}
