#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletPool.generated.h"

class ABullet;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYSHOOTINGGAME_API UBulletPool : public UActorComponent
{
	GENERATED_BODY()

public:
	UBulletPool();

	virtual void BeginPlay() override;

	/** 获取一发未激活的子弹 */
	ABullet* GetBullet();

	/** 子弹回收时调用 */
	void ReturnBullet(ABullet* Bullet);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(EditAnywhere)
	int32 BulletCacheSize = 20;

	/** 子弹缓存 */
	UPROPERTY()
	TArray<TObjectPtr<ABullet>> Pool;
};
