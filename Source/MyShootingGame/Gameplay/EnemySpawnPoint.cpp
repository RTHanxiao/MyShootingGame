#include "EnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "EnemyWaveDirector.h"

AEnemySpawnPoint::AEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;

	SpawnArea->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FTransform AEnemySpawnPoint::GetSpawnTransform() const
{
	const FVector Origin = SpawnArea->GetComponentLocation();
	const FVector Extent = SpawnArea->GetScaledBoxExtent();

	const FVector RandLoc = Origin + FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		FMath::FRandRange(-Extent.Z, Extent.Z)
	);

	FRotator Rot = GetActorRotation();
	Rot.Yaw = FMath::FRandRange(-180.f, 180.f);

	return FTransform(Rot, RandLoc);
}

void AEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyWaveDirector::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		CachedDirector = Cast<AEnemyWaveDirector>(Found[0]);
	}

	if (CachedDirector)
	{
		CachedDirector->OnRegionActivated.AddDynamic(this, &AEnemySpawnPoint::HandleRegionActivated);
		CachedDirector->OnRegionDeactivated.AddDynamic(this, &AEnemySpawnPoint::HandleRegionDeactivated);

		// 初始计算一次（如果玩家已经在区域里）
		RecalcActive();
	}
}

void AEnemySpawnPoint::HandleRegionActivated(FGameplayTag RegionId)
{
	if (BelongRegions.HasTagExact(RegionId) || BelongRegions.HasTag(RegionId))
	{
		RecalcActive();
	}
}

void AEnemySpawnPoint::HandleRegionDeactivated(FGameplayTag RegionId)
{
	if (BelongRegions.HasTagExact(RegionId) || BelongRegions.HasTag(RegionId))
	{
		RecalcActive();
	}
}

void AEnemySpawnPoint::RecalcActive()
{
	bActive = false;
	if (!CachedDirector) return;

	for (const FGameplayTag& Tag : BelongRegions)
	{
		if (CachedDirector->IsRegionActive(Tag))
		{
			bActive = true;
			return;
		}
	}
}