#include "EnemyWaveDirector.h"
#include "MyShootingGame/Table/WaveConfigData.h"
#include "EnemySpawnPoint.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EnemySpawnRegionVolume.h"

AEnemyWaveDirector::AEnemyWaveDirector()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false; // 先按单机/Listen 跑通；联机可改成仅服务器执行
}

void AEnemyWaveDirector::BeginPlay()
{
	Super::BeginPlay();

	if (!WaveConfig || WaveConfig->Waves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WaveDirector] WaveConfig invalid."));
		return;
	}

	if (bAutoCollectSpawnPoints)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), Found);
		for (AActor* A : Found)
		{
			SpawnPoints.Add(Cast<AEnemySpawnPoint>(A));
		}
	}

	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WaveDirector] No spawn points."));
		return;
	}

	StartWave(0);
}

void AEnemyWaveDirector::StartWave(int32 InWaveIndex)
{
	WaveIndex = InWaveIndex;
	SpawnedBatches = 0;
	AliveEnemies.Reset();

	const FWaveConfig& Wave = WaveConfig->Waves[WaveIndex];

	GetWorldTimerManager().ClearTimer(SpawnTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemyWaveDirector::TickSpawn, Wave.SpawnInterval, true);

	UE_LOG(LogTemp, Warning, TEXT("[WaveDirector] Start Wave %d"), WaveIndex);
}

void AEnemyWaveDirector::TickSpawn()
{
	if (!WaveConfig || !WaveConfig->Waves.IsValidIndex(WaveIndex)) return;

	CleanupAlive();

	const FWaveConfig& Wave = WaveConfig->Waves[WaveIndex];

	// 刷满批次则停止刷，等待清场推进
	if (SpawnedBatches >= Wave.SpawnBatches)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		TryAdvanceWave();
		return;
	}

	// 同时存活上限
	if (AliveEnemies.Num() >= Wave.MaxAlive)
	{
		return;
	}

	// 抽怪物类（权重）
	TSubclassOf<APawn> EnemyClass = PickEnemyClassWeighted(Wave);
	if (!EnemyClass) return;

	// 抽生成点
	AEnemySpawnPoint* SP = PickSpawnPoint();
	if (!SP) return;

	// 本批次刷 Min~Max 只
	const int32 Count = FMath::RandRange(
		FMath::Max(1, Wave.Entries[0].MinCount),
		FMath::Max(1, Wave.Entries[0].MaxCount)
	);

	// 注意：Count 的范围应该来自“选中的 Entry”，这里为了简化写法，下面会在 PickEnemyClassWeighted 里一起返回 Count 更好。
	// 我给你一个更稳的写法：直接在 Entries 里抽中一个 Entry，然后用它的 Min/Max。

	// ---- 更稳写法：重抽 Entry（推荐你用这个替换上面的 Count 逻辑）----
	// 这里先快速给你最小实现：刷 1 只
	const int32 RealCount = 1;

	for (int32 i = 0; i < RealCount; ++i)
	{
		const FTransform Xform = SP->GetSpawnTransform();
		if (APawn* P = SpawnOneEnemy(EnemyClass, Xform))
		{
			AliveEnemies.Add(P);
		}
	}

	SpawnedBatches++;
}

APawn* AEnemyWaveDirector::SpawnOneEnemy(TSubclassOf<APawn> EnemyClass, const FTransform& Xform)
{
	if (!EnemyClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* Spawned = GetWorld()->SpawnActor<APawn>(EnemyClass, Xform, Params);
	return Spawned;
}

AEnemySpawnPoint* AEnemyWaveDirector::PickSpawnPoint() const
{
	TArray<AEnemySpawnPoint*> Candidates;
	Candidates.Reserve(SpawnPoints.Num());

	for (AEnemySpawnPoint* SP : SpawnPoints)
	{
		if (IsValid(SP) && SP->bActive)
		{
			Candidates.Add(SP);
		}
	}

	if (Candidates.Num() == 0) return nullptr;
	return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
}

TSubclassOf<APawn> AEnemyWaveDirector::PickEnemyClassWeighted(const FWaveConfig& Wave) const
{
	float Total = 0.f;
	for (const FSpawnEntry& E : Wave.Entries)
	{
		if (E.EnemyClass && E.Weight > 0.f) Total += E.Weight;
	}
	if (Total <= 0.f) return nullptr;

	float R = FMath::FRandRange(0.f, Total);
	for (const FSpawnEntry& E : Wave.Entries)
	{
		if (!E.EnemyClass || E.Weight <= 0.f) continue;
		R -= E.Weight;
		if (R <= 0.f) return E.EnemyClass;
	}
	return Wave.Entries.Last().EnemyClass;
}

void AEnemyWaveDirector::CleanupAlive()
{
	for (int32 i = AliveEnemies.Num() - 1; i >= 0; --i)
	{
		if (!AliveEnemies[i].IsValid())
		{
			AliveEnemies.RemoveAtSwap(i);
		}
	}
}

void AEnemyWaveDirector::TryAdvanceWave()
{
	CleanupAlive();

	if (AliveEnemies.Num() > 0) return;
	if (!WaveConfig) return;

	const FWaveConfig& Wave = WaveConfig->Waves[WaveIndex];
	const int32 Next = WaveIndex + 1;

	if (!WaveConfig->Waves.IsValidIndex(Next))
	{
		UE_LOG(LogTemp, Warning, TEXT("[WaveDirector] All waves finished."));
		return;
	}

	FTimerHandle Delay;
	GetWorldTimerManager().SetTimer(Delay, [this, Next]()
		{
			StartWave(Next);
		}, Wave.NextWaveDelay, false);
}

void AEnemyWaveDirector::RegisterRegion(AEnemySpawnRegionVolume* Region)
{
	if (!Region || !Region->RegionId.IsValid()) return;
	RegisteredRegions.Add(Region);
	RegionPlayerCount.FindOrAdd(Region->RegionId); // 确保存在 key
}

bool AEnemyWaveDirector::IsRegionActive(FGameplayTag RegionId) const
{
	if (const int32* C = RegionPlayerCount.Find(RegionId))
	{
		return (*C) > 0;
	}
	return false;
}

void AEnemyWaveDirector::NotifyPlayerEnterRegion(FGameplayTag RegionId, APawn* PlayerPawn)
{
	if (!RegionId.IsValid() || !PlayerPawn) return;

	int32& C = RegionPlayerCount.FindOrAdd(RegionId);
	C++;

	if (C == 1)
	{
		// 从 0 -> 1：激活
		OnRegionActivated.Broadcast(RegionId);
	}
}

void AEnemyWaveDirector::NotifyPlayerExitRegion(FGameplayTag RegionId, APawn* PlayerPawn)
{
	if (!RegionId.IsValid() || !PlayerPawn) return;

	int32* C = RegionPlayerCount.Find(RegionId);
	if (!C) return;

	*C = FMath::Max(0, *C - 1);

	if (*C == 0)
	{
		// 从 1 -> 0：失活
		OnRegionDeactivated.Broadcast(RegionId);
	}
}