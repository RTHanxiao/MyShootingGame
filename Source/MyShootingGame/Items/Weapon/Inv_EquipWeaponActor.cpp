
#include "Inv_EquipWeaponActor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

#include "Inv_WeaponFragments.h"
#include "../Bullets/Bullet.h"
#include "../Bullets/BulletPool.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"




AInv_EquipWeaponActor::AInv_EquipWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletal"));
	WeaponSkeletal->SetupAttachment(Root);

	// Pickup components（地面拾取模式）
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(Root);
	PickupCollision->InitSphereRadius(150.f);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AInv_EquipWeaponActor::OnPickupEnter);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &AInv_EquipWeaponActor::OnPickupLeave);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(Root);
	PickupWidget->SetVisibility(false);

	ItemComponent = CreateDefaultSubobject<UInv_ItemComponent>(TEXT("ItemComponent"));
}

void AInv_EquipWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	// Owner 可能是 PC（你的 EquipmentComponent 挂在 PC 上，并 SetOwner(PC)）
	if (AController* C = Cast<AController>(GetOwner()))
	{
		CachedOwningPawn = C->GetPawn();
	}
	else
	{
		CachedOwningPawn = Cast<APawn>(GetOwner());
	}

	if (CachedOwningPawn.IsValid())
	{
		CachedBulletPool = CachedOwningPawn->FindComponentByClass<UBulletPool>();
	}

}

// Inv_EquipWeaponActor.cpp (主工程)
void AInv_EquipWeaponActor::BindItem(UInv_InventoryItem* InItem)
{
	BoundItem = InItem;

	if (PickupCollision) PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (PickupWidget)    PickupWidget->SetVisibility(false);
}

void AInv_EquipWeaponActor::ShootOnce()
{
	// ===== 0) world / mesh / item =====
	if (!GetWorld() || !WeaponSkeletal || !IsValid(BoundItem))
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: World/Mesh/BoundItem invalid"));
		return;
	}

	// ===== 1) resolve PC + Pawn (不要依赖 CachedOwningPawn) =====
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		// 兜底：Owner 不是 PC 的情况
		if (APawn* P = GetInstigator())
		{
			PC = Cast<APlayerController>(P->GetController());
		}
	}
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: PC null. Owner=%s"), *GetNameSafe(GetOwner()));
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: PC->GetPawn()==null. PC=%s"), *GetNameSafe(PC));
		return;
	}

	// ===== 2) resolve BulletPool every time (缓存可有可无) =====
	UBulletPool* BulletPool = Pawn->FindComponentByClass<UBulletPool>();
	if (!BulletPool)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: BulletPool null on Pawn=%s"), *GetNameSafe(Pawn));
		return;
	}

	// ===== 3) ammo consume =====
	FInv_ItemManifest& Manifest = BoundItem->GetItemManifestMutable();
	FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfTypeMutable<FInv_WeaponAmmoFragment>();
	if (!Ammo)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: AmmoFragment missing on item=%s"), *GetNameSafe(BoundItem));
		return;
	}
	if (!Ammo->ConsumeOne())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ShootOnce] No ammo in mag"));
		return;
	}

	// ===== 4) FireConfig (anim/sound/speed) =====
	const FInv_WeaponFireConfigFragment* FireCfg = Manifest.GetFragmentOfType<FInv_WeaponFireConfigFragment>();
	const FVector MuzzleLocation = WeaponSkeletal->GetSocketLocation(MuzzleSocketName);

	if (FireCfg)
	{
		if (FireCfg->FireAnimation)
		{
			WeaponSkeletal->PlayAnimation(FireCfg->FireAnimation, false);
		}
		if (FireCfg->FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireCfg->FireSound, MuzzleLocation);
		}
	}

	SpawnShellEject();
	SpawnMuzzleFlash();

	// ===== 5) camera trace (screen center) =====
	int32 SizeX = 0, SizeY = 0;
	PC->GetViewportSize(SizeX, SizeY);
	const FVector2D ScreenCenter(SizeX * 0.5f, SizeY * 0.5f);

	FVector CamOrigin, CamDir;
	PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, CamOrigin, CamDir);
	CamDir = CamDir.GetSafeNormal();

	const FVector CameraEnd = CamOrigin + CamDir * AimTraceDistance;

	FHitResult CameraHit;
	FCollisionQueryParams CamParams(SCENE_QUERY_STAT(CameraTrace), true);
	CamParams.AddIgnoredActor(Pawn);
	CamParams.AddIgnoredActor(this);

	const bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
		CameraHit, CamOrigin, CameraEnd, ECC_GameTraceChannel1, CamParams);

	const FVector AimPoint = bCameraHit ? CameraHit.ImpactPoint : CameraEnd;

	// ===== 6) muzzle trace correction =====
	FHitResult MuzzleHit;
	FCollisionQueryParams MuzzleParams(SCENE_QUERY_STAT(MuzzleTrace), true);
	MuzzleParams.AddIgnoredActor(Pawn);
	MuzzleParams.AddIgnoredActor(this);

	const bool bMuzzleHit = GetWorld()->LineTraceSingleByChannel(
		MuzzleHit, MuzzleLocation, AimPoint, ECC_Visibility, MuzzleParams);

	const FVector FinalPoint = bMuzzleHit ? MuzzleHit.ImpactPoint : AimPoint;

	FVector ShootDir = (FinalPoint - MuzzleLocation).GetSafeNormal();
	if (ShootDir.IsNearlyZero()) ShootDir = CamDir;

	// ===== 7) spawn bullet =====
	ABullet* Bullet = BulletPool->GetBullet();
	if (!Bullet)
	{
		UE_LOG(LogTemp, Error, TEXT("[ShootOnce] Abort: BulletPool returned NULL bullet!"));
		return;
	}

	Bullet->SetOwner(Pawn);
	const float BulletSpeed = FireCfg ? FireCfg->BulletSpeed : 5000.f;
	Bullet->ActivateBullet(MuzzleLocation, ShootDir.Rotation(), BulletSpeed, this);
}



void AInv_EquipWeaponActor::HandleBulletHit(const FHitResult& Hit)
{
	if (!IsValid(BoundItem)) return;

	const FInv_ItemManifest& Manifest = BoundItem->GetItemManifest();

	// ✅ 结算：完全交给 Item 的 fragments（数据/状态/伤害/回传）
	const FInv_WeaponHitResolveFragment* Resolver = Manifest.GetFragmentOfType<FInv_WeaponHitResolveFragment>();
	if (Resolver)
	{
		Resolver->ResolveHit(BoundItem, CachedOwningPawn.Get(), this, Hit);
	}
}

void AInv_EquipWeaponActor::OnPickupEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 地面拾取提示：仅示例，实际你可以在这里触发 UI / Outline
	if (OtherActor && OtherActor->IsA<APawn>())
	{
		if (PickupWidget) PickupWidget->SetVisibility(true);
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->PickItem = this;
		UE_LOG(LogTemp, Warning, TEXT("[ItemBase]  Player PickItem Set"));
	}


}

void AInv_EquipWeaponActor::OnPickupLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<APawn>())
	{
		if (PickupWidget) PickupWidget->SetVisibility(false);
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		if (Player->PickItem == this)
		{
			Player->PickItem = nullptr;
		}

		UE_LOG(LogTemp, Warning, TEXT("[ItemBase]  Player PickItem Set to Null"));
	}
}

void AInv_EquipWeaponActor::PlayWeaponReloadAnim()
{
	if (!BoundItem || !WeaponSkeletal) return;

	const FInv_ItemManifest& M = BoundItem->GetItemManifest();
	const FInv_WeaponReloadConfigFragment* R = M.GetFragmentOfType<FInv_WeaponReloadConfigFragment>();
	if (R && R->ReloadWeaponAnimation)
	{
		WeaponSkeletal->PlayAnimation(R->ReloadWeaponAnimation, false);
	}
}

bool AInv_EquipWeaponActor::GetRecoil(float& OutVertical, float& OutHorizontal) const
{
	OutVertical = 0.f;
	OutHorizontal = 0.f;

	if (!IsValid(BoundItem))
	{
		return false;
	}

	const FInv_ItemManifest& Manifest = BoundItem->GetItemManifest();

	const FInv_WeaponRecoilFragment* Recoil =
		Manifest.GetFragmentOfType<FInv_WeaponRecoilFragment>();

	if (!Recoil)
	{
		return false;
	}

	OutVertical = Recoil->VerticalRecoil;
	OutHorizontal = Recoil->HorizontalRecoil;
	return true;
}


float AInv_EquipWeaponActor::GetAttackRate() const
{
	if (!BoundItem) return 0.2f;
	const FInv_ItemManifest& M = BoundItem->GetItemManifest();
	if (const FInv_WeaponFireControlFragment* F = M.GetFragmentOfType<FInv_WeaponFireControlFragment>())
	{
		return FMath::Max(0.01f, F->AttackRate);
	}
	return 0.2f;
}

bool AInv_EquipWeaponActor::IsAutoFire() const
{
	if (!BoundItem) return true;
	const FInv_ItemManifest& M = BoundItem->GetItemManifest();
	if (const FInv_WeaponFireControlFragment* F = M.GetFragmentOfType<FInv_WeaponFireControlFragment>())
	{
		return F->bAutoFire;
	}
	return true;
}

bool AInv_EquipWeaponActor::HasAmmoInMag() const
{
	if (!BoundItem) return false;
	const FInv_ItemManifest& M = BoundItem->GetItemManifest();
	if (const FInv_WeaponAmmoFragment* A = M.GetFragmentOfType<FInv_WeaponAmmoFragment>())
	{
		return A->CurrentAmmoInMag > 0;
	}
	return false;
}