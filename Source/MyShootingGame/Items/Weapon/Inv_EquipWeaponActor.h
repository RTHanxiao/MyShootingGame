#pragma once

#include "CoreMinimal.h"
#include "EquipmentManagement/Actors/Inv_EquipActor.h"

#include "Inv_EquipWeaponActor.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UBulletPool;
class UInv_InventoryItem;
struct FHitResult;
class APawn;

UCLASS()
class MYSHOOTINGGAME_API AInv_EquipWeaponActor : public AInv_EquipActor
{
	GENERATED_BODY()

public:
	AInv_EquipWeaponActor();

	/** 绑定当前装备槽对应的 InventoryItem（唯一数据源） */
	virtual void BindItem(UInv_InventoryItem* InItem) override;

	/** 执行“一次射击”（一发） */
	void ShootOnce();

	/** 子弹命中回调入口（由 ABullet 调用） */
	void HandleBulletHit(const FHitResult& Hit);

	/** 给 Bullet 忽略自伤用 */
	APawn* GetOwningPawn() const { return CachedOwningPawn.Get(); }

	float GetAttackRate() const;
	bool IsAutoFire() const;
	bool HasAmmoInMag() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayWeaponReloadAnim();

protected:
	virtual void BeginPlay() override;

	/** 地面拾取模式：进入/离开范围（可按需改成你项目现有交互） */
	UFUNCTION()
	void OnPickupEnter(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnPickupLeave(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/** 蓝图实现：弹壳、枪口火光等表现 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|VFX")
	void SpawnShellEject();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|VFX")
	void SpawnMuzzleFlash();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|VFX")
	void SetOutlineVisibility(bool bVisibility);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInv_ItemComponent* ItemComponent;

	UFUNCTION(BlueprintPure, Category = "Weapon|Recoil")
	bool GetRecoil(float& OutVertical, float& OutHorizontal) const;

private:
	/** ========== Components（表现/执行必须） ========== */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess =  "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletal = nullptr;

	/** ========== Pickup（地面拾取模式可用） ========== */
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> PickupCollision = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	TObjectPtr<UWidgetComponent> PickupWidget = nullptr;

	/** ========== Runtime Binding ========== */
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> BoundItem = nullptr;

	/** Owner 是 PC 时，用它拿到 Pawn 供忽略自伤/反投影用 */
	TWeakObjectPtr<APawn> CachedOwningPawn = nullptr;

	/** Socket/射线等通用执行参数（不放数据，不放状态） */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName = TEXT("MUZZLE");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float AimTraceDistance = 100000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MuzzleForwardOffset = 20.f;

	TWeakObjectPtr<UBulletPool> CachedBulletPool;
};
