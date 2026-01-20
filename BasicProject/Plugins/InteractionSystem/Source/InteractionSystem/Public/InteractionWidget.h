// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "InteractionWidget.generated.h"


class UInteractionDetectorComponent;
class UTextBlock;
class UImage;

/**
 * Base widget class for interaction UI
 * WBP_Interaction should inherit from this class
 *
 * 상호작용 UI 베이스 위젯 클래스
 */
UCLASS(Blueprintable, BlueprintType)
class INTERACTIONSYSTEM_API UInteractionWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void NativeTick(const FGeometry &MyGeometry,
                          float InDeltaTime) override;

  /** Set the detector component reference */
  UFUNCTION(BlueprintCallable, Category = "Interaction")
  void SetDetectorComponent(UInteractionDetectorComponent *InDetector);

  /** Get current display text from detector */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  FText GetDisplayText() const;

  /** Get current opacity from detector */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  float GetOpacity() const;

  /** Get current scale from detector */
  UFUNCTION(BlueprintPure, Category = "Interaction")
  float GetScale() const;

protected:
  /** Called every tick to update widget visuals */
  UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
  void UpdateWidgetVisuals();
  virtual void UpdateWidgetVisuals_Implementation();

  /** Reference to the detector component */
  UPROPERTY(BlueprintReadOnly, Category = "Interaction")
  TObjectPtr<UInteractionDetectorComponent> DetectorComponent;

  /** Text block to display interaction text (bind in BP) */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UTextBlock> TextBlock_DisplayText;

  /** Marker container for applying scale animation */
  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UImage> Image_Dot;

  UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true),
            Category = "Interaction")
  TObjectPtr<UImage> Image_Ring;
};
