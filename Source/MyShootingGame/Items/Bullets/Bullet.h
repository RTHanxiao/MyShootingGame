#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UProjectileMovementComponent;
class UParticleSystemComponent;
class UBulletPool;
class USphereComponent;
class AInv_EquipWeaponActor;

UCLASS()
class MYSHOOTINGGAME_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	ABullet();

	virtual void BeginPlay() override;

	/** 激活子弹（对象池调用） */
	void ActivateBullet(
		const FVector& SpawnLocation,
		const FRotator& SpawnRotation,
		float Speed,
		AInv_EquipWeaponActor* InSourceWeapon
	);

	/** 回收子弹 */
	void DeactivateBullet();

	/** 是否正在飞行 */
	bool IsActive() const { return bActive; }

	/** 关联对象池 */
	void SetOwningPool(UBulletPool* InPool) { OwningPool = InPool; }

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleComp = nullptr;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileComp = nullptr;

	bool bActive = false;

	FTimerHandle LifeTimerHandle;

	/** 所属对象池，用于回收 */
	UPROPERTY()
	TObjectPtr<UBulletPool> OwningPool = nullptr;

	/** 生命周期长度（可在编辑器设置） */
	UPROPERTY(EditAnywhere)
	float LifeTime = 2.0f;

	UFUNCTION()
	void OnBulletHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	/** ✅ 新：来源武器（装备执行体） */
	TWeakObjectPtr<AInv_EquipWeaponActor> SourceWeapon = nullptr;
};
