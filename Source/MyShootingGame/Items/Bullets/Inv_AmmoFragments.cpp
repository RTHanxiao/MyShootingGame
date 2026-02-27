#include "MyShootingGame/Items/Bullets/Inv_AmmoFragments.h"
#include "Items/Inv_InventoryItem.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "../Weapon/Inv_WeaponFragments.h" 

#include "GameFramework/PlayerController.h"
#include "../Weapon/Inv_WeaponFragments.h"

void FInv_AmmoPickupFragment::OnPickedUp(APlayerController* PC, UInv_InventoryItem* Item) const
{
	if (!PC || !Item) return;

	// 你自己项目里 InventoryComponent 的获取方式调整一下
	UInv_EquipmentComponent* EquipComp = PC->FindComponentByClass<UInv_EquipmentComponent>();
	if (!EquipComp) return;

	// ✅ 给“当前 Active 武器”加备弹（你也可以做成：遍历所有武器里 AmmoType 匹配者）
	UInv_InventoryItem* WeaponItem = EquipComp->GetActiveWeaponItem();
	if (!WeaponItem) return;

	FInv_ItemManifest& WeaponManifest = WeaponItem->GetItemManifestMutable();
	FInv_WeaponAmmoFragment* AmmoFrag = WeaponManifest.GetFragmentOfTypeMutable<FInv_WeaponAmmoFragment>();
	if (!AmmoFrag) return;

	// AmmoType 匹配才加（如果你不需要类型，就删掉这个判断）
	if (AmmoType.IsValid() && AmmoFrag->AmmoItemType.IsValid() && !AmmoFrag->AmmoItemType.MatchesTagExact(AmmoType))
	{
		return;
	}

	AmmoFrag->TotalAmmo += AddAmmo;
}
