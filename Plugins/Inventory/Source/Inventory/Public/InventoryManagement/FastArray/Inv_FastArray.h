// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

class UInv_InventoryItem;
class UInv_InventoryComponent;
class UInv_ItemComponent;
struct FGameplayTag;

/**
 * 仓储系统单一物品条目
 */

USTRUCT()
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInv_InventoryEntry() {}

private:
	friend struct FInv_InventoryFastArray;
	friend UInv_InventoryComponent; 

	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};


/**
 * 仓储系统物品
 */
USTRUCT()
struct INVENTORY_API FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UInv_InventoryItem*> GetAllItems() const;

	//父类复用
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaParms, *this);
	}
	
	//TODO : 把主逻辑的物品逻辑抽成Component
	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComp);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* ItemComp);
	void RemoveEntry(UInv_InventoryItem* ItemToRemove);
	UInv_InventoryItem* FindFirstItemByTag(const FGameplayTag& ItemTag) ;

private:
	friend UInv_InventoryComponent;

	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent ;


};

template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


