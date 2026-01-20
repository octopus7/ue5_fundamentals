// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InteractionDetectorComponent.h"

void UInteractionWidget::NativeConstruct() {
  Super::NativeConstruct();

  // Try to find detector component from owning player
  if (APlayerController *PC = GetOwningPlayer()) {
    if (APawn *Pawn = PC->GetPawn()) {
      DetectorComponent =
          Pawn->FindComponentByClass<UInteractionDetectorComponent>();
    }
  }
}

void UInteractionWidget::NativeTick(const FGeometry &MyGeometry,
                                    float InDeltaTime) {
  Super::NativeTick(MyGeometry, InDeltaTime);

  UpdateWidgetVisuals();
}

void UInteractionWidget::SetDetectorComponent(
    UInteractionDetectorComponent *InDetector) {
  DetectorComponent = InDetector;
}

FText UInteractionWidget::GetDisplayText() const {
  if (DetectorComponent) {
    return DetectorComponent->GetCurrentDisplayText();
  }
  return FText::GetEmpty();
}

float UInteractionWidget::GetOpacity() const {
  if (DetectorComponent) {
    return DetectorComponent->GetCurrentOpacity();
  }
  return 0.0f;
}

float UInteractionWidget::GetScale() const {
  if (DetectorComponent) {
    return DetectorComponent->GetCurrentScale();
  }
  return 0.0f;
}

void UInteractionWidget::UpdateWidgetVisuals_Implementation() {
  if (!DetectorComponent) {
    return;
  }

  // Update text
  if (TextBlock_DisplayText) {
    TextBlock_DisplayText->SetText(GetDisplayText());
  }

  // Note: Opacity and Scale are applied by the DetectorComponent
  // via SetRenderOpacity and SetRenderScale on the entire widget.
  // Individual element animations can be done in Blueprint by overriding this
  // function.
}
