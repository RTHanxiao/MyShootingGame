#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "InventoryManagement/Type/Inv_GridTypes.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemManifest.generated.h"


class UInv_InventoryItem;
struct FInv_ItemFragment;
class UObject;
class UInv_CompositeBase;

/**
 * 物品清单所需的数据
 * 用于创建物品
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInv_ItemManifest
{
	GENERATED_BODY()

	TArray<TInstancedStruct<FInv_ItemFragment>>& GetFragmentsMutable() { return Fragments; }

	UInv_InventoryItem* Manifest(UObject* NewOuter);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }
	void AssimilateFragments(UInv_CompositeBase* Composite) const;

	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& Tag) const;

	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	const T* GetFragmentOfType() const;

	template<typename T> requires std::derived_from<T, FInv_ItemFragment>
	T* GetFragmentOfTypeMutable() ;

	void SpawnItemActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	template<typename T> requires std::derived_from<T,FInv_ItemFragment>
	TArray<const T*> GetAllFragmentsOfType()const;

	void ClearFragments();

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory{ EInv_ItemCategory::None };

	UPROPERTY(EditAnywhere, Category = "Inventory",meta = (Categories = "GameItems"))
	FGameplayTag ItemType;

	UPROPERTY(EditAnywhere,Category = "Inventory")
	TSubclassOf<AActor> ItemActorClass;

};

template<typename T> 
	requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& Tag) const
{
	for(const TInstancedStruct<FInv_ItemFragment>& FragmentStruct : Fragments)
	{
		if (const T* Fragment = FragmentStruct.GetPtr<T>())
		{
			if (!Fragment->GetFragmentTag().MatchesTagExact(Tag)) continue;
			return Fragment;
		}
	}
	return nullptr;
}

template<typename T> requires std::derived_from<T, FInv_ItemFragment>
const T* FInv_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FInv_ItemFragment>& FragmentStruct : Fragments)
	{
		if (const T* Fragment = FragmentStruct.GetPtr<T>())
		{
			return Fragment;
		}
	}
	return nullptr;
}

template<typename T> requires std::derived_from<T, FInv_ItemFragment>
T* FInv_ItemManifest::GetFragmentOfTypeMutable()
{
	for ( TInstancedStruct<FInv_ItemFragment>& FragmentStruct : Fragments)
	{
		if (T* Fragment = FragmentStruct.GetMutablePtr<T>())
		{
			return Fragment;
		}
	}
	return nullptr;
}

template<typename T> requires std::derived_from<T, FInv_ItemFragment>
TArray<const T*>
FInv_ItemManifest::GetAllFragmentsOfType() const
{
	TArray<const T*> Result;
	for (const TInstancedStruct<FInv_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			Result.Add(FragmentPtr);
		}
	}
	return Result;
}
