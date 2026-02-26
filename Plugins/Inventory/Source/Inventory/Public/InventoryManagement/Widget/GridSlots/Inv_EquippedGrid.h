

#pragma once

#include "CoreMinimal.h"
#include "Inv_GridSlot.h"
#include "GameplayTagContainer.h"
#include "Inv_EquippedGrid.generated.h"

class UInv_EquipSlottedItem;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UInv_EquippedGrid*, GridSlot, const FGameplayTag&, EquipmentTypeTag);

UCLASS()
class INVENTORY_API UInv_EquippedGrid : public UInv_GridSlot
{
	GENERATED_BODY()

public:
	virtual void NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
		
	UInv_EquipSlottedItem* OnItemEquiped(UInv_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize);
	void SetEquippedSlottedItem(UInv_EquipSlottedItem* Item) { EquipSlottedItem = Item; }

	FEquippedGridSlotClicked OnEquippedGridClicked;

	

private:
	UPROPERTY(EditAnyWhere, Category = "Inventory", meta =(Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf <UInv_EquipSlottedItem> EquippSlotItemClass;

	UPROPERTY()
	TObjectPtr<UInv_EquipSlottedItem> EquipSlottedItem;

	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;
};
