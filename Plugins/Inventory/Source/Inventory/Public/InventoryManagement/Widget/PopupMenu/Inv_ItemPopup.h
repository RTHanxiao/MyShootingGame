// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopup.generated.h"

/**
 * 
 */

class UButton;
class USlider;
class UTextBlock;
class USizeBox;

DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, Amout, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);

UCLASS()
class INVENTORY_API UInv_ItemPopup : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	FPopUpMenuConsume OnConsume;
	FPopUpMenuSplit OnSplit;
	FPopUpMenuDrop OnDrop;

	int32 GetSplitAmout() const;
	void CollapseButtonConsume(); 
	void CollapseButtonSplit();
	void CollapseButtonDrop();
	void SetSliderValue(const int32 Max,const float Value) const;
	FVector2D GetBoxSize() const;

	void SetGridIndex(int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }

private:

	int32 GridIndex{ INDEX_NONE };

	UPROPERTY(meta =  (BindWidget))
	TObjectPtr<UButton> Btn_Consume;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Split;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Drop;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Split;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;

	UFUNCTION()
	void OnConsumeBtnClicked();
	UFUNCTION()
	void OnSplitBtnClicked();
	UFUNCTION()
	void OnDropBtnClicked();
	UFUNCTION()
	void OnSliderValueChanged(float Value);




	
	
};
