#include "EnemySpawnRegionVolume.h"
#include "Components/BoxComponent.h"
#include "EnemyWaveDirector.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AEnemySpawnRegionVolume::AEnemySpawnRegionVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("RegionBox"));
	RootComponent = Box;

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Box->SetGenerateOverlapEvents(true);
}

void AEnemySpawnRegionVolume::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawnRegionVolume::OnBoxBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawnRegionVolume::OnBoxEnd);

	if (!bAutoRegisterToDirector) return;

	// 找场景里唯一 Director（你也可以改成 GameState/SubSystem）
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyWaveDirector::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		CachedDirector = Cast<AEnemyWaveDirector>(Found[0]);
		if (CachedDirector)
		{
			CachedDirector->RegisterRegion(this);
		}
	}
}

bool AEnemySpawnRegionVolume::IsPlayerPawn(AActor* OtherActor) const
{
	const APawn* P = Cast<APawn>(OtherActor);
	return P && P->IsPlayerControlled();
}

void AEnemySpawnRegionVolume::OnBoxBegin(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!CachedDirector || !IsPlayerPawn(OtherActor) || !RegionId.IsValid()) return;
	CachedDirector->NotifyPlayerEnterRegion(RegionId, Cast<APawn>(OtherActor));
}

void AEnemySpawnRegionVolume::OnBoxEnd(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32)
{
	if (!CachedDirector || !IsPlayerPawn(OtherActor) || !RegionId.IsValid()) return;
	CachedDirector->NotifyPlayerExitRegion(RegionId, Cast<APawn>(OtherActor));
}