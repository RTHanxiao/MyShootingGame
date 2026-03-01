#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveConfigData.generated.h"

USTRUCT(BlueprintType)
struct FSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APawn> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MaxCount = 1;
};

USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	// 这一波会刷多少“批次”（每批次会从 Entry 里抽一个类，然后刷 Min~Max 只）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 SpawnBatches = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.05"))
	float SpawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MaxAlive = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSpawnEntry> Entries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float NextWaveDelay = 3.0f;
};

UCLASS(BlueprintType)
class MYSHOOTINGGAME_API UWaveConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWaveConfig> Waves;
};