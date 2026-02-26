#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "EquipmentManagement/Actors/Inv_EquipActor.h"
#include "Components/SkeletalMeshComponent.h"




void UInv_EquipmentComponent::SetOwningSkeletal(USkeletalMeshComponent* Mesh)
{
	OwningSkeletal = Mesh;
}

void UInv_EquipmentComponent::InitializeOwner(APlayerController* PC)
{
	if (IsValid(PC))
	{
		OwningPC = PC;
	}
	InitInventoryComponent();
}

AInv_EquipActor* UInv_EquipmentComponent::GetActiveWeaponActor() const
{
	return (ActiveSlot == EInv_EquipSlot::Primary) ? PrimarySlot.Actor.Get() : SecondarySlot.Actor.Get();
}

UInv_InventoryItem* UInv_EquipmentComponent::GetActiveWeaponItem() const
{
	return (ActiveSlot == EInv_EquipSlot::Primary) ? PrimarySlot.Item.Get() : SecondarySlot.Item.Get();
}

void UInv_EquipmentComponent::SetActiveSlot(EInv_EquipSlot NewSlot)
{
	ActiveSlot = NewSlot;

	AInv_EquipActor* Primary = PrimarySlot.Actor.Get();
	AInv_EquipActor* Secondary = SecondarySlot.Actor.Get();

	if (Primary)
	{
		const bool bActive = (ActiveSlot == EInv_EquipSlot::Primary);
		Primary->SetActorHiddenInGame(!bActive);
		Primary->SetActorEnableCollision(bActive);
	}

	if (Secondary)
	{
		const bool bActive = (ActiveSlot == EInv_EquipSlot::Secondary);
		Secondary->SetActorHiddenInGame(!bActive);
		Secondary->SetActorEnableCollision(bActive);
	}
}


void UInv_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPlayerController();
}

void UInv_EquipmentComponent::InitInventoryComponent()
{
	InventoryComp = UInv_InventoryStatic::GetInventoryComponent(OwningPC.Get());
	if (!InventoryComp.IsValid()) return;

	if (!InventoryComp->OnItemEquip.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComp->OnItemEquip.AddDynamic(this,&ThisClass::OnItemEquipped);
	}
	if (!InventoryComp->OnItemUnequip.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComp->OnItemUnequip.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}
}

void UInv_EquipmentComponent::InitPlayerController()
{
	OwningPC = Cast<APlayerController>(GetOwner());
	if (!OwningPC.IsValid()) return;

	// 已经有 Pawn 直接走初始化
	if (ACharacter* OwnerChara = Cast<ACharacter>(OwningPC->GetPawn()); IsValid(OwnerChara))
	{
		OwningSkeletal = OwnerChara->GetMesh();
		InitInventoryComponent();
		return;
	}

	// 没 Pawn 避免重复绑定
	if (!OwningPC->OnPossessedPawnChanged.IsAlreadyBound(this, &ThisClass::OnPossessedPawnChanged))
	{
		OwningPC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}
	
}

void UInv_EquipmentComponent::OnPossessedPawnChanged(APawn* OP, APawn* NP)
{
	// 如果只需要初始化一次,拿到新 Pawn 后解绑,防止后续重复触发
	if (OwningPC.IsValid() && OwningPC->OnPossessedPawnChanged.IsAlreadyBound(this, &ThisClass::OnPossessedPawnChanged))
	{
		OwningPC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}

	if (ACharacter* NewChara = Cast<ACharacter>(NP); IsValid(NewChara))
	{
		OwningSkeletal = NewChara->GetMesh();
		InitInventoryComponent();
	}
}

FInv_EquipSlotState& UInv_EquipmentComponent::GetSlotStateMutable(EInv_EquipSlot Slot)
{
	return (Slot == EInv_EquipSlot::Primary) ? PrimarySlot : SecondarySlot;
}

AInv_EquipActor* UInv_EquipmentComponent::SpawnEquippedActor(
	EInv_EquipSlot Slot,
	FInv_EquipmentFragment* EquipmentFragment,
	UInv_InventoryItem* Item,
	USkeletalMeshComponent* AttachMesh)
{
	if (!EquipmentFragment || !IsValid(Item) || !IsValid(AttachMesh)) return nullptr;

	// 如果该槽之前有旧Actor，先处理（Destroy/Detach 你按需求）
	FInv_EquipSlotState& SlotState = GetSlotStateMutable(Slot);
	if (AInv_EquipActor* Old = SlotState.Actor.Get())
	{
		Old->Destroy();
		SlotState.Actor = nullptr;
	}

	AInv_EquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	if (!IsValid(SpawnedEquipActor)) return nullptr;

	SpawnedEquipActor->SetOwner(GetOwner());
	if (IsValid(SpawnedEquipActor))
	{
		SpawnedEquipActor->BindItem(Item);
	}

	SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
	SpawnedEquipActor->SetOwner(GetOwner());

	SpawnedEquipActor->InitFromItem(Item);

	EquipmentFragment->SetEquippedActor(SpawnedEquipActor);

	SlotState.Item = Item;
	SlotState.Actor = SpawnedEquipActor;

	SetActiveSlot(ActiveSlot);

	return SpawnedEquipActor;

}

AInv_EquipActor* UInv_EquipmentComponent::FindEquipedActorByTag(const FGameplayTag& EquipmentTypeTag)
{
	auto FoundActor = EquippedActors.FindByPredicate([&EquipmentTypeTag](const AInv_EquipActor* EquippedActor)
		{
			return EquippedActor->GetEquipmentType().MatchesTagExact(EquipmentTypeTag);
		});
	return FoundActor ? *FoundActor : nullptr;
}

void UInv_EquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag)
{
	if (AInv_EquipActor* EquippedActor = FindEquipedActorByTag(EquipmentTypeTag); IsValid(EquippedActor))
	{
		EquippedActors.Remove(EquippedActor);
		EquippedActor->Destroy();
	}
}

void UInv_EquipmentComponent::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem))return;
	if (!OwningPC->HasAuthority()) return;

	FInv_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsPreview)
	{
		EquipmentFragment->OnEquip(OwningPC.Get());
	}
	
	if (!OwningSkeletal.IsValid()) return;
	AInv_EquipActor* SpawnEquippActor = SpawnEquippedActor(ActiveSlot, EquipmentFragment, EquippedItem, OwningSkeletal.Get());

	EquippedActors.Add(SpawnEquippActor);
	SetActiveSlot(ActiveSlot);

}

void UInv_EquipmentComponent::OnItemUnequipped(UInv_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem))return;
	if (!OwningPC->HasAuthority()) return;

	FInv_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsPreview)
	{
		EquipmentFragment->OnUnequip(OwningPC.Get());
	}
	
	RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}
