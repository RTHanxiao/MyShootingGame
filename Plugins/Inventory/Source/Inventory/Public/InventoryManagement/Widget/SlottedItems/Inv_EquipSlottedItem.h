

#pragma once

#include "CoreMinimal.h"
#include "Inv_SlottedItem.h"
#include "GameplayTagContainer.h"
#include "Inv_EquipSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedGridSlotItemClicked,class UInv_EquipSlottedItem*, SlottedItem);


UCLASS( )
class INVENTORY_API UInv_EquipSlottedItem : public UInv_SlottedItem
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetEquipmentTypeTag(const FGameplayTag& Tag) { EquipmentTypeTag = Tag; }
	FGameplayTag GetEquipmentTypeTag(){return EquipmentTypeTag; }

	FOnEquippedGridSlotItemClicked OnEquippedGridSlotClicked;
private:

	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
