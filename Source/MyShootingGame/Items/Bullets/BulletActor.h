#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"

#include "BulletActor.generated.h"

class UInv_ItemComponent;

UCLASS()
class MYSHOOTINGGAME_API ABulletActor : public AItemBase
{
	GENERATED_BODY()

public:
	ABulletActor();

protected:
	virtual void BeginPlay() override;

	virtual void OnEnterInteractRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	virtual void OnLeaveInteractRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	) override;

public:
	virtual void OnPickedUp(AActor* Picker) override;
};
