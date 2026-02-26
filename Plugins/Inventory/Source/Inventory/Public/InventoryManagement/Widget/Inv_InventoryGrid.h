// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryManagement/Type/Inv_GridTypes.h"
#include "Items/Inv_InventoryItem.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_GridSlot;
class UCanvasPanel;
class UInv_InventoryComponent;
struct FInv_SlotAvailabilityResult;
class UInv_SlottedItem;
struct FInv_GridFragment;
struct FInv_ImageFragment;
class UInv_HoverItem;
class UInv_ItemPopup;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComp) ;

	void ShowCursor();
	void HideCursor();
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);
	void DropItem();
	bool HasHoverItem() const;
	UInv_HoverItem* GetHoverItem() const;
	float GetTileSize() const { return TileSize; }
	void ClearHoverItem();
	void AssignHoverItem(UInv_InventoryItem* InventoryItem);

	void OnHide();

	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);

private:

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComp;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;

	void ConstructGrid();
	FInv_SlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* Item,const int32 StackAmout = -1);
	FInv_SlotAvailabilityResult HasRoomForItem(const FInv_ItemManifest& Manifest, const int32 StackAmout = -1);
	void AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	bool MatchesCategory(const UInv_InventoryItem* Item) const;

	FVector2D GetDrawSize(const FInv_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment) const;
	void AddItemAtIndex( UInv_InventoryItem* NewItem, const int32 Index,const bool bStackable, const int32 StackAmount);
	UInv_SlottedItem* CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment, const int32 Index);
	void AddSlottedItemToCanvas(UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const int32 Index) const;
	void UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount);
	bool IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const;
	FIntPoint GetItemDimensions(const FInv_ItemManifest& Manifest) const;
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot,const FIntPoint& Dimensions,const TSet<int32>& CheckedIndices,TSet<int32>& ClaimedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize);
	bool CheckSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, TSet<int32>& ClaimedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize) const;
	bool HasValidItem(const UInv_GridSlot* GridSlot)const;
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem,const FGameplayTag& ItemType) const;
	bool IsWithinBounds(const int32 Index, const FIntPoint& Dimensions) const;
	int32 HowManySlotsToFill(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UInv_GridSlot* GridSlot)const;

	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	void RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex);
	
	void UpdateTileParameters(const FVector2D& CanvasPos,const FVector2D& MousePos);
	FIntPoint GetHoverCoord(const FVector2D& CanvasPos, const FVector2D& MousePos) const;
	EInv_TileQuadrant GetTileQuadrant(const FVector2D& CanvasPos, const FVector2D& MousePos) const;
	void OnTileParametersChanged(const FInv_TileParameters& Parameter);
	FIntPoint GetStartCoord(const FIntPoint& Coord, const FIntPoint& Dimension, const EInv_TileQuadrant Quadrant) const;
	FInv_SpaceQueryResult CheckHoverPosSpace(const FIntPoint& Position, const FIntPoint& Dimensions) ;
	bool CursorExitCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize,const FVector2D& Location) ;
	void HighlightSlots(const int32 Index, const FIntPoint& Dimension);
	void UnhighlightSlots(const int32 Index, const FIntPoint& Dimension);
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimension, EInv_GridSlotState GridSlotState);
	void PutDownOnIndex(const int32 Index);
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	bool IsSameStackableItem(const UInv_InventoryItem* ClickedInventoryItem) const;
	void SwapHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	bool SwapCheckStackCount(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;
	void SwapStackCount(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	void MergeItemsStackWithHover(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	void FillinStack(const int32 Fill, const int32 Remainder, const int32 index);

	void CreatePopUpMenu(const int32 GridIndex);

	void PutHoverItemBack();
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemPopup> PopUpMenuClass;
	UPROPERTY(EditAnywhere,Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;
	UPROPERTY()
	TObjectPtr<UInv_ItemPopup> PopUpMenu;
	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;


	UFUNCTION()
	void AddStack(const FInv_SlotAvailabilityResult& Result);

	UFUNCTION()
	void OnSlottedItemClicked(const int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);
	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmout,int32 Index);
	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);
	UFUNCTION()
	void OnInventoryMenuToggled(bool bOpen);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	EInv_ItemCategory ItemCategory;
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInv_GridSlot> GridSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInv_SlottedItem> SlottedItemClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UInv_SlottedItem>> SlottedItemsMap;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 Columns;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	float TileSize;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInv_HoverItem> HoverItemClass;

	UPROPERTY()
	TObjectPtr<UInv_HoverItem> HoverItem;

	FInv_TileParameters TileParameters;
	FInv_TileParameters PreviousTileParameters;

	//放置物品时的物品位置索引，鼠标实际悬停的位置为图标的正中心，
	//但实际需要存放的地方（UpperLeftIndex）并非一定是悬停的地方
	int32 ItemDropIndex{ INDEX_NONE };
	FInv_SpaceQueryResult CurrentHoverResult;
	bool bMouseWithCanvas;
	bool bWasMouseWithinCanvas;
	int32 PreviousHighlightedIndex{ INDEX_NONE };
	FIntPoint PreviousHighlightedDimension{ FIntPoint::ZeroValue };
};
