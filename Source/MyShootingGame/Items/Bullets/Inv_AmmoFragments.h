#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Inv_ItemFragment.h"

#include "Inv_AmmoFragments.generated.h"

class UInv_InventoryItem;
class APlayerController;

/**
 * 子弹拾取：把“子弹盒”转换成对武器弹药的补给
 * 建议：把 AmmoType / AddAmount 设计成数据可配
 */
USTRUCT(BlueprintType)
struct MYSHOOTINGGAME_API FInv_AmmoPickupFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	/** 这个子弹盒提供多少发（也可以改成从 Item 的 StackCount 读） */
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 AddAmmo = 30;

	/** 可选：子弹类型（跟 WeaponAmmoFragment::AmmoItemType 对齐） */
	UPROPERTY(EditAnywhere, Category = "Ammo")
	FGameplayTag AmmoType;

	/** 被拾取时执行（由 ItemComponent 或 InventoryComponent 触发） */
	void OnPickedUp(APlayerController* PC, UInv_InventoryItem* Item) const;
};
