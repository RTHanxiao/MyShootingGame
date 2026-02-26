// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/HoverItem/Inv_HoverItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/Inv_InventoryItem.h"




void UInv_HoverItem::SetImageBrush(const FSlateBrush& InBrush) const
{
	ItemImage->SetBrush(InBrush);
}

void UInv_HoverItem::UpdateStackCount(const int32 NewStackCount) 
{
	StackCount = NewStackCount;
	if (NewStackCount > 0)
	{
		TextStackCount->SetText(FText::AsNumber(NewStackCount));
		TextStackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextStackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FGameplayTag UInv_HoverItem::GetItemTypeTag() const
{
	if(InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag();
}

void UInv_HoverItem::SetIsStackable(bool bNewIsStackable)
{
	bIsStackable = bNewIsStackable;
	if (!bNewIsStackable)
	{
		TextStackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

