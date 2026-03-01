#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

class UBoxComponent;

UCLASS()
class MYSHOOTINGGAME_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawnPoint();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FTransform GetSpawnTransform() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> SpawnArea;

	// 这个 SpawnPoint 属于哪些区域（多对多）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn|Region", meta = (Categories = "Region"))
	FGameplayTagContainer BelongRegions;

	// 当前是否激活（是否允许参与刷怪）
	UPROPERTY(BlueprintReadOnly, Category = "Spawn|Region")
	bool bActive = false;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<class AEnemyWaveDirector> CachedDirector;

	UFUNCTION()
	void HandleRegionActivated(FGameplayTag RegionId);

	UFUNCTION()
	void HandleRegionDeactivated(FGameplayTag RegionId);

	void RecalcActive(); // 根据 BelongRegions 与 Director 状态刷新 bActive
};