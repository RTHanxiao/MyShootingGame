#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Widget/Inv_InventoryBase.h"
#include <Inventory.h>
#include "InventoryManagement/Type/Inv_GridTypes.h"
#include "Items/Inv_InventoryItem.h"
#include "Net/UnrealNetwork.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"




UInv_InventoryComponent::UInv_InventoryComponent(): InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	bIsInventoryMenuOpen = false;

}


void UInv_InventoryComponent::ToggleInventoryMenu()
{
	if (bIsInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
	OnInventoryMenuToggled.Broadcast(bIsInventoryMenuOpen);
}

void UInv_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UInv_InventoryComponent::Server_AddNewItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount)
{
	UInv_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	if (IsNetMode(NM_Standalone) || IsNetMode(NM_ListenServer))
	{
		OnItemAdded.Broadcast(NewItem);
	 }

	//告诉物品组件销毁主actor
	ItemComponent->PickedUp();

}

void UInv_InventoryComponent::Server_AddStacksToItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	const FGameplayTag& ItemType = IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag();
	UInv_InventoryItem* Item = InventoryList.FindFirstItemByTag(ItemType);
	if (!IsValid(Item))return;

	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	//若剩余为0，销毁物品
	//否则，更新物品组件上的数量
	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if(FInv_StackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}


}

void UInv_InventoryComponent::Server_DropItem_Implementation(UInv_InventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	SpawnDroppedItem(Item, StackCount);

}

void UInv_InventoryComponent::Server_ConsumeItem_Implementation(UInv_InventoryItem* Item,const int32 ConsumeAmount = 1)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - ConsumeAmount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	//调用相应使用函数 
	if (FInv_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningPlayerController.Get());
	}
}

void UInv_InventoryComponent::Server_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnequip);
}

void UInv_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip)
{
	// 装备组件监听委托
	OnItemEquip.Broadcast(ItemToEquip);
	OnItemUnequip.Broadcast(ItemToUnequip);
}

void UInv_InventoryComponent::SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount)
{
	//世界中生成丢弃的物品
	const APawn* OwningPawn = OwningPlayerController->GetPawn();

	FVector Forward = OwningPawn->GetActorForwardVector();
	Forward = Forward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLoc = OwningPawn->GetActorLocation() + Forward * FMath::FRandRange(DropSpawnDistanceMin,DropSpawnDistanceMax);
	SpawnLoc.Z -= 50;
	const FRotator SpawnRot = FRotator::ZeroRotator;

	//通过ItemManifest生成掉落物
	FInv_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FInv_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnItemActor(this, SpawnLoc, SpawnRot);
}

void UInv_InventoryComponent::AddReplicatedSubObject(UObject* SubObject)
{
	if(IsUsingRegisteredSubObjectList()&& IsReadyForReplication() && IsValid(SubObject))
	{
		Super::AddReplicatedSubObject(SubObject);
	}
}

bool UInv_InventoryComponent::TryAddItem(UInv_ItemComponent* ItemComp)
{
	if (!ItemComp) return false;

	// 服务器/无UI时：不要访问 InventoryMenu
	if (!InventoryMenu)
	{
		// 直接走“新增物品”服务器流程（不做容量判定）
		Server_AddNewItem(ItemComp, 0);
		return true;
	}

	FInv_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComp);

	UInv_InventoryItem* FoundItem = InventoryList.FindFirstItemByTag(ItemComp->GetItemManifest().GetItemType());
	Result.Item = FoundItem;

	if (Result.TotalRoomToFill == 0)
	{
		NoRoom.Broadcast();
		return false;
	}

	if(Result.Item.IsValid() && Result.bStackable)
	{
		//添加到现有堆叠，只改动已有堆的数量
		OnStackChanged.Broadcast(Result);
		Server_AddStacksToItem(ItemComp, Result.TotalRoomToFill,Result.RemainingToFill);
		return true;
	}
	else if (Result.TotalRoomToFill > 0)
	{
		//新增物品
		Server_AddNewItem(ItemComp, Result.bStackable ?Result.TotalRoomToFill:0);
		return true;
	}

	return false;
}

void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	ConstructInventory();
}


void UInv_InventoryComponent::ConstructInventory()
{	
	OwningPlayerController = Cast<APlayerController>(GetOwner());
	checkf(OwningPlayerController.IsValid(), TEXT("InventortComponent need a Owner(PlayerController)"));
	if (!OwningPlayerController->IsLocalController())return;

	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningPlayerController.Get(), InventoryMenuClass);
	if (!InventoryMenu)
	{
		UE_LOG(LogInventory, Warning, TEXT("Missing Inventory Menu Widget"));
	}
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UInv_InventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu))	return;
		
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	bIsInventoryMenuOpen = true;

	if (!OwningPlayerController.IsValid())return;

	FInputModeGameAndUI InputMode;
	OwningPlayerController->SetInputMode(InputMode);
	OwningPlayerController->SetShowMouseCursor(true);
}

void UInv_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu))	return;

	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	bIsInventoryMenuOpen = false;

	if (!OwningPlayerController.IsValid())return;

	FInputModeGameOnly InputMode;
	OwningPlayerController->SetInputMode(InputMode);
	OwningPlayerController->SetShowMouseCursor(false);
}


