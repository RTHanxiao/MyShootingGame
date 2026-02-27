#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Items/Fragments/Inv_ItemFragment.h"

#include "Inv_WeaponFragments.generated.h"

class UAnimationAsset;
class UParticleSystem;
class USoundBase;
class UMaterialInterface;
class UDamageType;
class UInv_InventoryItem;
class AZombieCharacter;

/** 弹药与装填：运行态存在 ItemInstance 的 Manifest 里 */
USTRUCT(BlueprintType)
struct FInv_WeaponAmmoFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Ammo")
	int32 MagazineSize = 30;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Ammo")
	int32 CurrentAmmoInMag = 30;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Ammo")
	int32 TotalAmmo = 90;

	UPROPERTY(EditAnywhere, Category = "Weapon|Ammo")
	FGameplayTag AmmoItemType;

	bool ConsumeOne()
	{
		if (CurrentAmmoInMag <= 0) return false;
		--CurrentAmmoInMag;
		return true;
	}

	bool CanReload() const { return (CurrentAmmoInMag < MagazineSize) && (TotalAmmo > 0); }

	void Reload()
	{
		const int32 Need = MagazineSize - CurrentAmmoInMag;
		const int32 Take = FMath::Min(Need, TotalAmmo);
		CurrentAmmoInMag += Take;
		TotalAmmo -= Take;
	}
};

/** 开火控制：射速/单发连发（给 PlayerCharacter 的 Timer 用） */
USTRUCT(BlueprintType)
struct FInv_WeaponFireControlFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
	float AttackRate = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
	bool bAutoFire = true;
};

/** 执行配置：武器自身动画、弹速 */
USTRUCT(BlueprintType)
struct FInv_WeaponFireConfigFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
	TObjectPtr<UAnimationAsset> FireAnimation = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
	float BulletSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
	TObjectPtr<USoundBase> FireSound = nullptr;
};

/** 伤害参数（倍率等） */
USTRUCT(BlueprintType)
struct FInv_WeaponDamageFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float HeadshotMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float ArmMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float LegMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float ChestMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Damage")
	float AbsMultiplier = 1.0f;
};

/** 命中反馈：SurfaceType -> 粒子/音效 + 贴花 */
USTRUCT(BlueprintType)
struct FInv_WeaponImpactFxFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Impact")
	TObjectPtr<UMaterialInterface> BulletDecalMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon|Impact")
	TMap<TEnumAsByte<EPhysicalSurface>, TObjectPtr<UParticleSystem>> SurfaceParticles;

	UPROPERTY(EditAnywhere, Category = "Weapon|Impact")
	TMap<TEnumAsByte<EPhysicalSurface>, TObjectPtr<USoundBase>> SurfaceSounds;

	// 特殊：血液粒子（你原代码用 FleshParticle）
	UPROPERTY(EditAnywhere, Category = "Weapon|Impact")
	TObjectPtr<UParticleSystem> FleshParticle = nullptr;

	// 可选：爆头声音
	UPROPERTY(EditAnywhere, Category = "Weapon|Impact")
	TObjectPtr<USoundBase> HeadShotSound = nullptr;
};

/**
 * ✅ 命中结算 Fragment：伤害、击杀/命中反馈回传等全部写在这里（Item 执行）
 * 你原 WeaponBase::HandleBulletHit 逻辑就迁到这里
 */
USTRUCT(BlueprintType)
struct FInv_WeaponHitResolveFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	// 注意：这里需要依赖你项目类（AZombieCharacter、事件系统等）
	// 如果你不想 fragment 直接 include 项目类，也可以把实现挪到 .cpp 里
	virtual void ResolveHit(const UInv_InventoryItem* Item, APawn* InstigatorPawn, AActor* WeaponActor, const FHitResult& Hit) const;
};

USTRUCT(BlueprintType)
struct FInv_WeaponRecoilFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	float VerticalRecoil = 0.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Recoil")
	float HorizontalRecoil = 0.f;
};

USTRUCT(BlueprintType)
struct FInv_WeaponReloadConfigFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Weapon|Reload")
	TObjectPtr<UAnimationAsset> ReloadWeaponAnimation = nullptr;

};
