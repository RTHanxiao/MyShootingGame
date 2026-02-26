// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/GridSlots/Inv_GridSlot.h"
#include "Components/Image.h"
#include "InventoryManagement/Widget/PopupMenu/Inv_ItemPopup.h"




void UInv_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	OnGridSlotHovered.Broadcast(TileIndex, InMouseEvent);
}


void UInv_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	OnGridSlotUnhovered.Broadcast(TileIndex, InMouseEvent);
}

FReply UInv_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotClicked.Broadcast(TileIndex, InMouseEvent);

	return FReply::Handled();
}


void UInv_GridSlot::SetPopUpMenu(UInv_ItemPopup* PopUp)
{
	PopUpMenu = PopUp;

	PopUpMenu->SetGridIndex(GetIndex());
	PopUpMenu->OnNativeDestruct.AddUObject(this, &UInv_GridSlot::OnPopUpMenuDestruct);
		
}

void UInv_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EInv_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EInv_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
	GridSlotState = EInv_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
	GridSlotState = EInv_GridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}

void UInv_GridSlot::OnPopUpMenuDestruct(UUserWidget* Menu)
{
	PopUpMenu.Reset();
}
