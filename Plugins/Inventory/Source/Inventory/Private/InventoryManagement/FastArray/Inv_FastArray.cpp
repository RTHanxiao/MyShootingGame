// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "Items/Inv_InventoryItem.h"


TArray<UInv_InventoryItem*> FInv_InventoryFastArray::GetAllItems() const
{
	TArray<UInv_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const FInv_InventoryEntry& Entry : Entries)
	{
		if (Entry.Item)
		{
			Results.Add(Entry.Item);
		}
	}
	return Results;
}


void FInv_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* InventoryComp = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!InventoryComp) return;
	for (int32 Index : RemovedIndices)
	{
		if (Entries.IsValidIndex(Index))
		{
			FInv_InventoryEntry& Entry = Entries[Index];
			if (Entry.Item)
			{
				InventoryComp->OnItemRemoved.Broadcast(Entry.Item);
			}
		}
	}
}


void FInv_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* InventoryComp = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!InventoryComp) return;
	for (int32 Index : AddedIndices)
	{
		if (Entries.IsValidIndex(Index))
		{
			FInv_InventoryEntry& Entry = Entries[Index];
			if (Entry.Item)
			{
				InventoryComp->OnItemAdded.Broadcast(Entry.Item);
			}
		}
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_InventoryItem* ItemComp)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComp;

	MarkItemDirty(NewEntry);
	return ItemComp;
}


UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_ItemComponent* ItemComp)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	UInv_InventoryComponent* InventoryComp = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!InventoryComp) return nullptr;

	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComp->GetItemManifest().Manifest(OwningActor);

	InventoryComp->AddReplicatedSubObject(NewEntry.Item);
	MarkItemDirty(NewEntry);

	return NewEntry.Item;

}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem* ItemToRemove)
{
	for(auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FInv_InventoryEntry& Entry = *EntryIt;
		if(Entry.Item == ItemToRemove)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
			return;
		}
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::FindFirstItemByTag(const FGameplayTag& ItemTag)
{
	auto Found = Entries.FindByPredicate([ItemTag](const FInv_InventoryEntry& Entry)
		{
			return IsValid(Entry.Item) && Entry.Item->GetItemManifest().GetItemType().MatchesTagExact(ItemTag);
		});
	return Found ? Found->Item : nullptr;
}
