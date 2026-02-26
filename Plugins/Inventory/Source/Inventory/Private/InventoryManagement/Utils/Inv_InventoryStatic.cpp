#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "InventoryManagement/Widget/Inv_InventoryBase.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"





UInv_InventoryComponent* UInv_InventoryStatic::GetInventoryComponent(const APlayerController* PC)
{
	if(!PC)
	{
		return nullptr;
	}
	UInv_InventoryComponent* InventoryComp = PC->FindComponentByClass<UInv_InventoryComponent>();
	return InventoryComp;
}

EInv_ItemCategory UInv_InventoryStatic::GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComp)
{
	if (!IsValid(ItemComp)) return EInv_ItemCategory::None;
	return ItemComp->GetItemManifest().GetItemCategory();
}

void UInv_InventoryStatic::ItemHovered(APlayerController* PC, UInv_InventoryItem* Item)
{
	UInv_InventoryComponent* InventoryComp = GetInventoryComponent(PC);
	if (!IsValid(InventoryComp))return;

	UInv_InventoryBase* InventoryBase = InventoryComp->GetInventoryMenu();
	if (!IsValid(InventoryBase))return;

	if (InventoryBase->HasHoverItem()) return;

	InventoryBase->OnItemHoverd(Item);
}

void UInv_InventoryStatic::ItemUnhovered(APlayerController* PC)
{
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	InventoryBase->OnItemUnhoverd();
}

UInv_HoverItem* UInv_InventoryStatic::GetHoverItem(APlayerController* PC)
{
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	UInv_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;

	return InventoryBase->GetHoverItem();
}

UInv_InventoryBase* UInv_InventoryStatic::GetInventoryWidget(APlayerController* PC)
{
	UInv_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	return IC->GetInventoryMenu();
}
