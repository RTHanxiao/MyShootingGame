#include "MyShootingGame/Items/Bullets/BulletActor.h"

#include "Items/Component/Inv_ItemComponent.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "BulletActor.h"

ABulletActor::ABulletActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABulletActor::BeginPlay()
{
	Super::BeginPlay();

	// ✅ BulletNum 不再由世界物体随机生成
	// 如果你一定要随机：把随机写进 ItemComp 初始化的 Item(StackCount) 或 Fragment(AmmoPickup.AddAmmo)
}

void ABulletActor::OnPickedUp(AActor* Picker)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Picker);
	if (!Player) return;

	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC) return;

	UInv_InventoryComponent* InvComp = PC->FindComponentByClass<UInv_InventoryComponent>();
	if (!InvComp || !ItemComponent) return;

	//  统一入口：把世界物体的 ItemComp 交给背包
	const bool bAdded = InvComp->TryAddItem(ItemComponent);
	if (bAdded)
	{
		Destroy();
	}
	// else: 背包满/不匹配，就不销毁
}

void ABulletActor::OnEnterInteractRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		Super::OnEnterInteractRange(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		if (ItemMesh)
		{
			ItemMesh->bRenderCustomDepth = true;
		}
	}
}

void ABulletActor::OnLeaveInteractRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		Super::OnLeaveInteractRange(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

		if (ItemMesh)
		{
			ItemMesh->bRenderCustomDepth = false;
		}
	}
}
