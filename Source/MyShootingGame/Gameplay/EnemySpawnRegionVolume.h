#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "EnemySpawnRegionVolume.generated.h"

class UBoxComponent;
class AEnemyWaveDirector;

UCLASS()
class MYSHOOTINGGAME_API AEnemySpawnRegionVolume : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawnRegionVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Region")
	TObjectPtr<UBoxComponent> Box;

	// 这个盒子代表哪个区域
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Region", meta = (Categories = "Region"))
	FGameplayTag RegionId;

	// 是否自动向 Director 注册
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Region")
	bool bAutoRegisterToDirector = true;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<AEnemyWaveDirector> CachedDirector;

	UFUNCTION()
	void OnBoxBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnBoxEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsPlayerPawn(AActor* OtherActor) const;
};