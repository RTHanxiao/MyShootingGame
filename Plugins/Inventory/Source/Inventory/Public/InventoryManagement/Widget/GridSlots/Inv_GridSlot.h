// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UImage;
class FInv_InventoryItem;
class UInv_ItemPopup;
class UInv_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent& ,MouseEvent);

enum class EInv_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};


/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetTileIndex(int32 NewIndex) { TileIndex = NewIndex; }
	int32 GetTileIndex() const { return TileIndex; }

	EInv_GridSlotState GetGridSlotState() const { return GridSlotState; }

	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UInv_InventoryItem* NewItem) { InventoryItem = NewItem; }
	int32 GetIndex() const { return TileIndex; }
	void SetIndex(int32 NewIndex) { TileIndex = NewIndex; }
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(int32 NewIndex) { UpperLeftIndex = NewIndex; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 NewStackCount) { StackCount = NewStackCount; }
	bool IsAvailable() const { return bAvailable; }
	void SetAvailable(bool bNewAvailable) { bAvailable = bNewAvailable; }
	UInv_ItemPopup* GetPopUpMenu() const { return PopUpMenu.Get(); }
	void SetPopUpMenu(UInv_ItemPopup* PopUp);
	
	void SetOccupiedTexture();
	void SetUnoccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();

	FGridSlotEvent OnGridSlotClicked;
	FGridSlotEvent OnGridSlotHovered;
	FGridSlotEvent OnGridSlotUnhovered;

private:
	int32 TileIndex{ INDEX_NONE };
	int32 StackCount{ 0 };
	int32 UpperLeftIndex{ INDEX_NONE };
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bAvailable{ true };
	TWeakObjectPtr<UInv_ItemPopup> PopUpMenu;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere,Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;

	EInv_GridSlotState	GridSlotState;

	UFUNCTION()
	void OnPopUpMenuDestruct(UUserWidget* Menu);

};
