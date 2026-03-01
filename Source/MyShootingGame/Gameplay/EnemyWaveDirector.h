#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../../../../../Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h"
#include "EnemyWaveDirector.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionActivated, FGameplayTag, RegionId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegionDeactivated, FGameplayTag, RegionId);


class UWaveConfigDataAsset;
class AEnemySpawnPoint;



UCLASS()
class MYSHOOTINGGAME_API AEnemyWaveDirector : public AActor
{
	GENERATED_BODY()

public:
	AEnemyWaveDirector();

	// Detector -> Director 注册
	UFUNCTION(BlueprintCallable, Category = "Region")
	void RegisterRegion(class AEnemySpawnRegionVolume* Region);

	// Detector -> Director 通知
	void NotifyPlayerEnterRegion(FGameplayTag RegionId, APawn* PlayerPawn);
	void NotifyPlayerExitRegion(FGameplayTag RegionId, APawn* PlayerPawn);

	// 广播
	UPROPERTY(BlueprintAssignable, Category = "Region")
	FOnRegionActivated OnRegionActivated;

	UPROPERTY(BlueprintAssignable, Category = "Region")
	FOnRegionDeactivated OnRegionDeactivated;

	// 查询（可选）：当前是否激活
	UFUNCTION(BlueprintCallable, Category = "Region")
	bool IsRegionActive(FGameplayTag RegionId) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TObjectPtr<UWaveConfigDataAsset> WaveConfig;

	UPROPERTY(EditAnywhere, Category = "Wave")
	bool bAutoCollectSpawnPoints = true;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<TObjectPtr<AEnemySpawnPoint>> SpawnPoints;

private:

	// 每个 Region 当前有多少玩家在内（用于多玩家）
	TMap<FGameplayTag, int32> RegionPlayerCount;

	// 已注册 Region
	TSet<TWeakObjectPtr<class AEnemySpawnRegionVolume>> RegisteredRegions;

	FTimerHandle SpawnTimer;

	int32 WaveIndex = 0;
	int32 SpawnedBatches = 0;

	TArray<TWeakObjectPtr<APawn>> AliveEnemies;

	void StartWave(int32 InWaveIndex);
	void TickSpawn();
	void TryAdvanceWave();

	APawn* SpawnOneEnemy(TSubclassOf<APawn> EnemyClass, const FTransform& Xform);

	AEnemySpawnPoint* PickSpawnPoint() const;
	TSubclassOf<APawn> PickEnemyClassWeighted(const struct FWaveConfig& Wave) const;
	void CleanupAlive();
};