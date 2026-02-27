#include "ItemBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Items/Component/Inv_ItemComponent.h" 
#include "MyShootingGame/Character/PlayerCharacter.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(Root);

	ItemCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ItemCollision"));
	ItemCollision->SetupAttachment(Root);
	ItemCollision->SetGenerateOverlapEvents(true);

	PickWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickWidget"));
	PickWidget->SetupAttachment(Root);
	PickWidget->SetDrawSize(FVector2D(200,60));

	ItemComponent = CreateDefaultSubobject<UInv_ItemComponent>(TEXT("ItemComponent"));

	// 默认先隐藏提示
	if (PickWidget)
	{
		PickWidget->SetVisibility(false);
	}
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemCollision)
	{
		ItemCollision->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnEnterInteractRange);
		ItemCollision->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnLeaveInteractRange);
	}
}

void AItemBase::OnEnterInteractRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->PickItem = this;
		UE_LOG(LogTemp, Warning, TEXT("[ItemBase]  Player PickItem Set"));
	}

	// 基类默认：显示拾取提示 + Outline
	if (PickWidget)
	{
		PickWidget->SetVisibility(true);
	}


	SetOutLineVisibility(true);
}

void AItemBase::OnLeaveInteractRange(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		if (Player->PickItem == this)
		{
			Player->PickItem = nullptr;
		}

		UE_LOG(LogTemp, Warning, TEXT("[ItemBase]  Player PickItem Set to Null"));
	}
	// 基类默认：隐藏拾取提示 + Outline
	if (PickWidget)
	{
		PickWidget->SetVisibility(false);
	}
	SetOutLineVisibility(false);
}

void AItemBase::OnPickedUp(AActor* Picker)
{
	// 基类默认不做事；子类实现“加入背包/加资源/销毁道具”等
}
