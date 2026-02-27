#include "Bullet.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"

#include "BulletPool.h"
#include "MyShootingGame/Items/Weapon/Inv_EquipWeaponActor.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = false;

	// 用碰撞体作为 Root（ProjectileMovement 才能正确基于它移动并产生 Hit）
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(4.f);

	// 对象池默认关闭碰撞，激活时再打开
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);

	// 先全部 Block（后续你可以按项目需求精调通道）
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);

	// 让 HitResult 能返回 PhysMaterial（否则 Hit.PhysMaterial 经常为空）
	CollisionComp->bReturnMaterialOnMove = true;

	// 生成 Hit 事件
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnBulletHit);

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(CollisionComp);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->UpdatedComponent = CollisionComp;
	ProjectileComp->InitialSpeed = 0.f;
	ProjectileComp->MaxSpeed = 10000.f;
	ProjectileComp->ProjectileGravityScale = 0.f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->bShouldBounce = false;

	// 对象池默认隐藏、无碰撞
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::ActivateBullet(const FVector& SpawnLocation, const FRotator& SpawnRotation, float Speed, AInv_EquipWeaponActor* InSourceWeapon)
{
	if (bActive) DeactivateBullet();

	bActive = true;
	SourceWeapon = InSourceWeapon;

	SetActorLocationAndRotation(SpawnLocation, SpawnRotation);

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	// 开碰撞
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 激活粒子（如果你是 Niagara 就替换组件类型）
	if (ParticleComp)
	{
		ParticleComp->Activate(true);
	}

	// 设置速度
	if (ProjectileComp)
	{
		ProjectileComp->SetUpdatedComponent(CollisionComp);
		ProjectileComp->StopMovementImmediately();

		ProjectileComp->SetComponentTickEnabled(true);
		ProjectileComp->Activate(true);

		ProjectileComp->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
		ProjectileComp->UpdateComponentVelocity();
	}

	// 清理旧 timer（对象池复用的关键）
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LifeTimerHandle);
		World->GetTimerManager().SetTimer(LifeTimerHandle, this, &ABullet::DeactivateBullet, LifeTime, false);
	}
}

void ABullet::DeactivateBullet()
{
	if (!bActive) return;

	bActive = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LifeTimerHandle);
	}

	// 关碰撞并隐藏
	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (ParticleComp)
	{
		ParticleComp->Deactivate();
	}

	// 关键：回收时明确 Deactivate + 关闭 Tick，避免下次复用状态混乱
	if (ProjectileComp)
	{
		ProjectileComp->StopMovementImmediately();
		ProjectileComp->Deactivate();
		ProjectileComp->SetComponentTickEnabled(false);
	}

	SourceWeapon = nullptr;

	if (OwningPool)
	{
		OwningPool->ReturnBullet(this);
	}
}

void ABullet::OnBulletHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!bActive) return;

	// 无效命中直接回收
	if (!OtherActor || OtherActor == this)
	{
		DeactivateBullet();
		return;
	}

	// ✅ 忽略自身/持枪 Pawn（避免自伤）
	if (SourceWeapon.IsValid())
	{
		AActor* WeaponActor = SourceWeapon.Get();
		APawn* OwnerPawn = SourceWeapon->GetOwningPawn();

		if (OtherActor == WeaponActor || (OwnerPawn && OtherActor == OwnerPawn))
		{
			DeactivateBullet();
			return;
		}

		// ✅ 把命中回调交回武器执行体，最终由 Item 的 HitResolveFragment 结算
		SourceWeapon->HandleBulletHit(Hit);
	}

	// 命中即回收（若需要穿透/反弹就在这里扩展）
	DeactivateBullet();
}
