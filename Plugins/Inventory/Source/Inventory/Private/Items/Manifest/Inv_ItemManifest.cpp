#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"
#include "Engine/World.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "InventoryManagement/Widget/Composite/Inv_CompositeBase.h"



UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);

	for (auto& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();

	return Item;
}

void FInv_ItemManifest::AssimilateFragments(UInv_CompositeBase* Composite)  const
{
	const auto& InventoryItemFragments = GetAllFragmentsOfType<FInv_InventoryItemFragment>();
	for (const auto* Fragment : InventoryItemFragments)
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget) 
			{
				Fragment->Assimilate(Widget);
			});
	}
}

void FInv_ItemManifest::SpawnItemActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!IsValid(WorldContextObject) || !IsValid(ItemActorClass)) return;

	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(ItemActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;

	//ÉèÖÃManifest,CategoryµÈÊôÐÔ
	UInv_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UInv_ItemComponent>();
	check(ItemComp);

	ItemComp->InitItemManifest(*this);
}

void FInv_ItemManifest::ClearFragments()
{
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}
	Fragments.Empty();
}
