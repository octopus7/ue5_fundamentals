// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "InteractionWidget.generated.h"

class UInteractableComponent;
class UTextBlock;
class UImage;
class UWidget;
class UPanelWidget;

/**
 * Base widget class for interaction UI markers
 * Supports separate fade for marker (dot/ring) and text panel
 *
 * 상호작용 UI 마커 베이스 위젯 클래스
 * 마커(닷/링)와 텍스트 패널 개별 페이드 지원
 */
UCLASS(Blueprintable, BlueprintType)
class INTERACTIONSYSTEM_API UInteractionWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void NativeTick(const FGeometry &MyGeometry,
                          float InDeltaTime) override;

  /** Set the owner interactable component */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetOwnerInteractable(UInteractableComponent *InInteractable);

  /** Get display text from owner */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  FText GetDisplayText() const;

  /** Show text panel with fade animation */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void ShowTextPanel();

  /** Hide text panel immediately (no animation) */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void HideTextPanelImmediate();

  /** Set marker opacity (applied to Ring and Dot) */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetMarkerOpacity(float InOpacity);

  /** Set marker scale (applied to Ring and Dot) */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetMarkerScale(float InScale);

  /** Check if this is the current interaction target */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  bool IsCurrentTarget() const;

protected:
  /** Called every tick to update widget visuals */
  UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
  void UpdateWidgetVisuals();
  virtual void UpdateWidgetVisuals_Implementation();

  /** Update text panel fade animation */
  void UpdateTextPanelAnimation(float DeltaTime);

  /** Reference to the owner interactable component */
  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  TObjectPtr<UInteractableComponent> OwnerInteractable;

  //~ BindWidget elements (auto-bind by name in BP)

  /** Marker dot image */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UImage> Image_Dot;

  /** Marker ring image */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UImage> Image_Ring;

  /** Text panel container (for fade control) */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UWidget> TextPanel;

  /** Text block for display text */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UTextBlock> TextBlock_DisplayText;

private:
  /** Text panel animation state */
  float CurrentTextPanelOpacity = 0.0f;
  float TargetTextPanelOpacity = 0.0f;
  bool bIsTextPanelAnimating = false;
};
