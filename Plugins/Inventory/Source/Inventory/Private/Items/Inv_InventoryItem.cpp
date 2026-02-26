// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_ItemFragment.h"




void UInv_InventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
	DOREPLIFETIME(ThisClass, TotalStackCount);
}

void UInv_InventoryItem::SetItemManifest(const FInv_ItemManifest& NewItemManifest)
{
	ItemManifest = FInstancedStruct::Make<FInv_ItemManifest>(NewItemManifest);
}

bool UInv_InventoryItem::IsStackable() const
{
	const FInv_StackableFragment* StackableFragment = GetItemManifest().GetFragmentOfType< FInv_StackableFragment > ();
	return StackableFragment != nullptr;
}

bool UInv_InventoryItem::IsConsumable() const
{
	return GetItemManifest().GetItemCategory() == EInv_ItemCategory::Consumable;
}
