// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagement/Widget/Inv_InventoryBase.h"
#include "Components/WidgetSwitcher.h"
#include "Inv_SpatialInventory.generated.h"

class UInv_InventoryGrid;
class UButton;
class UCanvasPanel;
class UInv_EquippedGrid;
class UInv_HoverItem;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime);
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComp) const override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	virtual void OnItemHoverd(UInv_InventoryItem* Item)override;
	virtual void OnItemUnhoverd()override;
	virtual bool HasHoverItem() const override;
	virtual UInv_HoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const;
private:
	UPROPERTY()
	TArray<TObjectPtr<UInv_EquippedGrid>> EquippedGridSlots;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equipment;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Misc;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Equipment;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Consumable;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_MISC;

	UPROPERTY(EditAnywhere,Category = "Inventory")
	TSubclassOf<UInv_ItemDescription> ItemDescrioptionClass;

	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> ItemDescription;

	UInv_ItemDescription* GetItemDescription();

	FTimerHandle DescriptionTimer;

	UPROPERTY(EditAnywhere,Category = "Inventory")
	float DescriptionTimerDelay = .5f;

	UFUNCTION()
	void ShowEquipment();

	UFUNCTION()
	void ShowConsumable();

	UFUNCTION()
	void ShowMISC();

	UFUNCTION()
	void OnEquippedGridClicked(UInv_EquippedGrid* EquippedGrid, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UInv_EquipSlottedItem* SlottedItem);

	void SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Btn);
	void DisableBtn(UButton* Btn);
	void SetItemDescriptionSizeAndPos(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const;
	bool CanEquipHoverItem(UInv_EquippedGrid* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)const;
	UInv_EquippedGrid* FindSlotWithEquippedItem(UInv_InventoryItem* Item)const;
	void ClearSlotOfItem(UInv_EquippedGrid* Slot);
	void RemoveEquippedSlottedItem(UInv_EquipSlottedItem* Item);
	void MakeEquippedSlottedItem(UInv_EquipSlottedItem* Equipped, UInv_EquippedGrid* Slot, UInv_InventoryItem* Item);
	void BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const;

	TWeakObjectPtr<UInv_InventoryGrid> ActiveGrid;
};
