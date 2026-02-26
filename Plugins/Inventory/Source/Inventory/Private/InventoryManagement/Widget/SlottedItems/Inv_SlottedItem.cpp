// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/SlottedItems/Inv_SlottedItem.h"
#include "Components/Image.h"
#include "Items/Inv_InventoryItem.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"




FReply UInv_SlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, InMouseEvent);
	return FReply::Handled();
}

void UInv_SlottedItem::NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	UInv_InventoryStatic::ItemHovered(GetOwningPlayer(), InventoryItem.Get());
}

void UInv_SlottedItem::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	UInv_InventoryStatic::ItemUnhovered(GetOwningPlayer());
}

void UInv_SlottedItem::SetInventoryItem(UInv_InventoryItem* NewItem)
{
	InventoryItem = NewItem; 
}

void UInv_SlottedItem::SetImageBrush(const FSlateBrush& NewBrush) const
{
	Image_Icon->SetBrush(NewBrush);
}

void UInv_SlottedItem::UpdateStackCount(int32 StackCount)
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
