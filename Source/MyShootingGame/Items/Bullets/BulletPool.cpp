#include "BulletPool.h"

#include "Bullet.h"
#include "Engine/World.h"

UBulletPool::UBulletPool()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBulletPool::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World || !BulletClass)
	{
		return;
	}

	Pool.Reserve(BulletCacheSize);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < BulletCacheSize; i++)
	{
		ABullet* NewBullet = World->SpawnActor<ABullet>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (!NewBullet) continue;

		NewBullet->SetOwningPool(this);
		NewBullet->DeactivateBullet();
		Pool.Add(NewBullet);
	}
}

ABullet* UBulletPool::GetBullet()
{
	for (ABullet* Bullet : Pool)
	{
		if (Bullet && !Bullet->IsActive())
		{
			return Bullet;
		}
	}
	return nullptr; // 若全忙，可扩容
}

void UBulletPool::ReturnBullet(ABullet* Bullet)
{
	if (!Bullet) return;

	// 你原来的逻辑保留：回收时挪到地下
	Bullet->SetActorLocation(FVector(0, 0, -99999));
}
