// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/PopupMenu/Inv_ItemPopup.h"
#include "Components/Button.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Slider.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/SizeBox.h"




void UInv_ItemPopup::NativeOnInitialized()
{  
	Super::NativeOnInitialized();

	Btn_Consume->OnClicked.AddDynamic(this, &UInv_ItemPopup::OnConsumeBtnClicked);
	Btn_Split->OnClicked.AddDynamic(this, &UInv_ItemPopup::OnSplitBtnClicked);
	Btn_Drop->OnClicked.AddDynamic(this, &UInv_ItemPopup::OnDropBtnClicked);
	Slider->OnValueChanged.AddDynamic(this, &UInv_ItemPopup::OnSliderValueChanged);

}


int32 UInv_ItemPopup::GetSplitAmout() const
{
	return Slider->GetValue();
}

void UInv_ItemPopup::OnConsumeBtnClicked()
{
	if (OnConsume.ExecuteIfBound(GridIndex))
	{
		RemoveFromParent();
	}
}

void UInv_ItemPopup::OnSplitBtnClicked()
{
	if (OnSplit.ExecuteIfBound(GetSplitAmout(), GridIndex))
	{
		RemoveFromParent();
	}
}

void UInv_ItemPopup::OnDropBtnClicked()
{
	if (OnDrop.ExecuteIfBound(GridIndex))
	{
		RemoveFromParent();
	}
	;
}

void UInv_ItemPopup::OnSliderValueChanged(float Value)
{
	Text_Split->SetText(FText::AsNumber(FMath::Floor(Value)));
}

void UInv_ItemPopup::CollapseButtonConsume()
{
	Btn_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopup::CollapseButtonSplit()
{
	Btn_Split->SetVisibility(ESlateVisibility::Collapsed);
	Slider->SetVisibility(ESlateVisibility::Collapsed);
	Text_Split->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopup::CollapseButtonDrop()
{
	Btn_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopup::SetSliderValue(const int32 Max, const float Value) const
{
	Slider->SetMaxValue(Max);
	Slider->SetMinValue(1);
	Slider->SetValue(Value);
	Text_Split->SetText(FText::AsNumber(FMath::Floor(Value)));
}

FVector2D UInv_ItemPopup::GetBoxSize() const
{
	return FVector2D(SizeBox->GetWidthOverride(), SizeBox->GetHeightOverride());
}
