// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "InteractableComponent.h"
#include "InteractionSettings.h"

void UInteractionWidget::NativeConstruct() {
  Super::NativeConstruct();

  // Initialize text panel as hidden
  if (TextPanel) {
    TextPanel->SetRenderOpacity(0.0f);
    TextPanel->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void UInteractionWidget::NativeTick(const FGeometry &MyGeometry,
                                    float InDeltaTime) {
  Super::NativeTick(MyGeometry, InDeltaTime);

  UpdateWidgetVisuals();

  if (bIsTextPanelAnimating) {
    UpdateTextPanelAnimation(InDeltaTime);
  }
}

void UInteractionWidget::SetOwnerInteractable(
    UInteractableComponent *InInteractable) {
  OwnerInteractable = InInteractable;
}

FText UInteractionWidget::GetDisplayText() const {
  if (OwnerInteractable) {
    return OwnerInteractable->GetDisplayText();
  }
  return FText::GetEmpty();
}

void UInteractionWidget::ShowTextPanel() {
  TargetTextPanelOpacity = 1.0f;
  TargetTextPanelScale = 1.0f;
  bIsTextPanelAnimating = true;

  if (TextPanel) {
    TextPanel->SetVisibility(ESlateVisibility::Visible);
  }
}

void UInteractionWidget::HideTextPanelImmediate() {
  // Immediately hide without animation
  TargetTextPanelOpacity = 0.0f;
  CurrentTextPanelOpacity = 0.0f;
  TargetTextPanelScale = 0.0f;
  CurrentTextPanelScale = 0.0f;
  TextPanelScaleVelocity = 0.0f;
  bIsTextPanelAnimating = false;

  if (TextPanel) {
    TextPanel->SetRenderOpacity(0.0f);
    TextPanel->SetRenderScale(FVector2D(0.0f, 0.0f));
    TextPanel->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void UInteractionWidget::SetMarkerOpacity(float InOpacity) {
  // Apply opacity to marker elements (Ring and Dot)
  if (Image_Ring) {
    Image_Ring->SetRenderOpacity(InOpacity);
  }
  if (Image_Dot) {
    Image_Dot->SetRenderOpacity(InOpacity);
  }
}

void UInteractionWidget::SetMarkerScale(float InScale) {
  // Apply scale to marker elements (Ring and Dot)
  const FVector2D Scale2D(InScale, InScale);
  if (Image_Ring) {
    Image_Ring->SetRenderScale(Scale2D);
  }
  if (Image_Dot) {
    Image_Dot->SetRenderScale(Scale2D);
  }
}

bool UInteractionWidget::IsCurrentTarget() const {
  if (OwnerInteractable) {
    return OwnerInteractable->IsCurrentTarget();
  }
  return false;
}

void UInteractionWidget::UpdateWidgetVisuals_Implementation() {
  // Update display text
  if (TextBlock_DisplayText) {
    TextBlock_DisplayText->SetText(GetDisplayText());
  }
}

void UInteractionWidget::UpdateTextPanelAnimation(float DeltaTime) {
  if (!TextPanel) {
    bIsTextPanelAnimating = false;
    return;
  }

  // Get settings
  float FadeDuration = 0.3f;
  float Elasticity = 2.0f;
  if (const UInteractionSettings *Settings = UInteractionSettings::Get()) {
    FadeDuration = Settings->DefaultFadeDuration;
    Elasticity = Settings->DefaultAnimationElasticity;
  }

  // Spring parameters for elastic scale
  const float SpringStiffness = 300.0f;
  const float Damping = 15.0f + (10.0f - Elasticity) * 2.0f;

  // Elastic scale animation
  const float ScaleDisplacement = TargetTextPanelScale - CurrentTextPanelScale;
  const float SpringForce = ScaleDisplacement * SpringStiffness;
  const float DampingForce = -TextPanelScaleVelocity * Damping;
  const float Acceleration = SpringForce + DampingForce;

  TextPanelScaleVelocity += Acceleration * DeltaTime;
  CurrentTextPanelScale += TextPanelScaleVelocity * DeltaTime;
  CurrentTextPanelScale = FMath::Max(0.0f, CurrentTextPanelScale);

  // Linear opacity animation
  const float FadeSpeed = FadeDuration > 0.0f ? 1.0f / FadeDuration : 100.0f;
  if (CurrentTextPanelOpacity < TargetTextPanelOpacity) {
    CurrentTextPanelOpacity =
        FMath::Min(CurrentTextPanelOpacity + FadeSpeed * DeltaTime,
                   TargetTextPanelOpacity);
  } else if (CurrentTextPanelOpacity > TargetTextPanelOpacity) {
    CurrentTextPanelOpacity =
        FMath::Max(CurrentTextPanelOpacity - FadeSpeed * DeltaTime,
                   TargetTextPanelOpacity);
  }

  // Apply to TextPanel
  TextPanel->SetRenderOpacity(CurrentTextPanelOpacity);
  TextPanel->SetRenderScale(
      FVector2D(CurrentTextPanelScale, CurrentTextPanelScale));

  // Check if animation complete
  const bool bScaleSettled =
      FMath::IsNearlyEqual(CurrentTextPanelScale, TargetTextPanelScale,
                           0.01f) &&
      FMath::Abs(TextPanelScaleVelocity) < 0.1f;
  const bool bOpacitySettled = FMath::IsNearlyEqual(
      CurrentTextPanelOpacity, TargetTextPanelOpacity, 0.01f);

  if (bScaleSettled && bOpacitySettled) {
    CurrentTextPanelOpacity = TargetTextPanelOpacity;
    CurrentTextPanelScale = TargetTextPanelScale;
    TextPanelScaleVelocity = 0.0f;
    bIsTextPanelAnimating = false;

    if (TargetTextPanelOpacity == 0.0f) {
      TextPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
  }
}
