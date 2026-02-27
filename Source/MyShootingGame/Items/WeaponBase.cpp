//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "MyShootingGame/Items/WeaponBase.h"
//#include "Components/SkeletalMeshComponent.h"
//#include "Components/SphereComponent.h"
//#include "Components/SceneComponent.h"
//#include "../Character/PlayerCharacter.h"
//#include "../Logic/MSG_EventManager.h"
//#include "Kismet/GameplayStatics.h"
//#include "Blueprint/WidgetLayoutLibrary.h"
//#include "../GameCore/MSG_PlayerController.h"
//#include "PhysicalMaterials/PhysicalMaterial.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "../Character/ZombieCharacter.h"
//#include "Engine/DamageEvents.h"
//#include "BulletPool.h"
//#include "Bullet.h"
//
//// Sets default values
//AWeaponBase::AWeaponBase()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//	
//	RootComponent = Root;
//
//	////创建拾取碰撞组件
//	//PickCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickCollision"));
//	//PickCollision->SetupAttachment(SceneRoot);
//	//PickCollision->SetSphereRadius(150.f);
//	ItemCollision->SetSphereRadius(150.f);
//
//	//创建武器网格组件
//	WeaponSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletal"));
//	WeaponSkeletal->SetupAttachment(Root);
//
//	BulletPool = CreateDefaultSubobject<UBulletPool>(TEXT("BulletPool"));
//
//}
//
//// Called when the game starts or when spawned
//void AWeaponBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//	InitWeaponData();
//
//	//绑定碰撞拾取事件
//	//PickCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::AttachGunToHand);
//
//	VerticalRecoil = WeaponStruct->RecoilPitch;
//	HorizontalRecoil = WeaponStruct->RecoilYaw;
//}
//
//void AWeaponBase::OnEnterInteractRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
//	{
//		// 基类负责：PickWidget 显示 + Outline 开关（若你在基类实现了）
//		Super::OnEnterInteractRange(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
//
//		// 子类额外：自定义深度描边
//		if (WeaponSkeletal)
//		{
//			WeaponSkeletal->bRenderCustomDepth = true;
//		}
//	}
//}
//
//void AWeaponBase::OnLeaveInteractRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
//	{
//		if (Player->PickItem == this)
//		{
//			Player->PickItem = nullptr;
//		}
//
//		// PickWidget 隐藏 + Outline 关闭
//		Super::OnLeaveInteractRange(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
//
//		// 子类额外：关闭自定义深度描边
//		if (ItemMesh)
//		{
//			ItemMesh->bRenderCustomDepth = false;
//		}
//	}
//}
//
//// Called every frame
//void AWeaponBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void AWeaponBase::InitWeaponData()
//{
//	WeaponData = LoadObject<UDataTable>(nullptr, TEXT("/Game/_Game/Data/DT_Weapon"));
//	WeaponStruct = WeaponData->FindRow<FWeaponTemplateStructure>(WeaponRowName, TEXT(""));
//	WeaponSkeletal->SetSkeletalMeshAsset(WeaponStruct->WeaponSkeletal);
//	CurrentAmmoInMag = WeaponStruct->MagazineSize;
//	TotalAmmo = WeaponStruct->Ammo;
//}
//
//void AWeaponBase::AttachGunToHand(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor)) 
//	{
//		Player->PickUpWeapon(this);
//	}
//}
//
////射线检测
////void AWeaponBase::ShootEvent()
////{
////	//减少弹匣内子弹数
////	CurrentAmmoInMag--;
////
////	//播放开火动画
////	UAnimationAsset* WeaponShootAnim = WeaponStruct->FireAnimation;
////	if (WeaponShootAnim)
////	{
////		WeaponSkeletal->PlayAnimation(WeaponShootAnim, false);
////	}
////
////	//空弹壳生成
////	SpawnShellEject();
////
////	//计算射线起止
////	FVector StartVector;
////	FVector WorldDir;
////	FVector2D ScreenPos = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) * 0.5;
////	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(WeaponOwner->GetController()))
////	{
////		UGameplayStatics::DeprojectScreenToWorld(PC, ScreenPos, StartVector, WorldDir);
////	}
////	FVector EndVector = (WorldDir * 10000.f) + StartVector;
////
////	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
////	ObjectType.Add(EObjectTypeQuery::ObjectTypeQuery1);
////	ObjectType.Add(EObjectTypeQuery::ObjectTypeQuery3);
////
////	//忽略的Actror
////	TArray<AActor*> IgnoreActors;
////
////	//射线debug类
////	//EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::Persistent;
////	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
////
////	FHitResult HitResult;
////	//定义射线
////	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartVector, EndVector, ObjectType, false, IgnoreActors, DrawDebugType, HitResult, true);
////
////	UPhysicalMaterial* HistPhysicalMaterial = HitResult.PhysMaterial.Get();
////	AActor* HitActor = HitResult.GetActor();
////	
////	FRotator BulletSpawnRotation = FRotator::ZeroRotator;
////	if (HitResult.bBlockingHit)
////	{
////		BulletSpawnRotation = UKismetMathLibrary::Conv_VectorToRotator(HitResult.Location - WeaponSkeletal->GetSocketLocation("MUZZLE"));
////	}
////	else
////	{
////		BulletSpawnRotation = UKismetMathLibrary::Conv_VectorToRotator(HitResult.TraceEnd - WeaponSkeletal->GetSocketLocation("MUZZLE"));
////	}
////	//子弹类对象池
////	if (ABullet* Bullet = BulletPool->GetBullet())
////	{
////		FVector MuzzleLocation = WeaponSkeletal->GetSocketLocation("MUZZLE");
////		FRotator MuzzleRotation = WeaponSkeletal->GetSocketRotation("MUZZLE");
////
////		Bullet->ActivateBullet(MuzzleLocation, BulletSpawnRotation, 5000.f);
////	}
////
////	//伤害处理
////	if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(HitResult.GetActor()))
////	{ 
////		if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(WeaponOwner->GetController()))
////		{
////			float Damage = WeaponStruct->AttackDamage;
////			bool bHeadShot = false;
////			//判断受击部位，伤害=基础伤害*部位倍率*减伤系数
////			switch (HistPhysicalMaterial->SurfaceType)
////			{
////			case EPhysicalSurface::SurfaceType7:
////				Damage *= WeaponStruct->HeadshotMultiplier;
////				bHeadShot = true;
////				break;
////			case EPhysicalSurface::SurfaceType8:
////				Damage *= WeaponStruct->ArmMultiplier;				
////				break;
////			case EPhysicalSurface::SurfaceType9:
////				Damage *= WeaponStruct->LegMultiplier;				
////				break;
////			case EPhysicalSurface::SurfaceType10:
////				Damage *= WeaponStruct->ChestMultiplier;
////				break;
////			case EPhysicalSurface::SurfaceType11:
////				Damage *= WeaponStruct->AbsMultiplier;
////				break;
////			default:
////				break;
////			}
////			//应用减伤系数
////			Damage *= Zombie->DamageReductionFactor;
////
////			//造成伤害
////			Zombie->TakeDamage(Damage, FDamageEvent(), PC, this);
////
////			bool bDead = Zombie->IsDead();
////
////			if (bDead)
////			{
////				if (bHeadShot)
////				{
////					UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(true);
////					UMSG_EventManager::GetEventManagerInstance()->HeadShotEventDelegate.ExecuteIfBound();
////					UE_LOG(LogTemp, Warning, TEXT("HeadShot Kill"));
////				}
////				else
////				{
////					UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(false);
////					UE_LOG(LogTemp, Warning, TEXT("Normal Kill"));
////				}
////			}
////			else
////			{
////				UMSG_EventManager::GetEventManagerInstance()->ShowHitFeedbackDelegate.ExecuteIfBound(bHeadShot);
////				UE_LOG(LogTemp, Warning, TEXT("Hit Zombie %s"), bHeadShot ? TEXT("HeadShot") : TEXT("Body"));
////			}
////
////			//生成血液特效
////			UGameplayStatics::SpawnEmitterAtLocation(
////				GetWorld(), 
////				FleshParticle, 
////				HitResult.Location, 
////				UKismetMathLibrary::Conv_VectorToRotator(HitResult.Normal), 
////				FVector(.5f, .5f, .5f));
////		}
////	}
////
////	if (HistPhysicalMaterial)
////	{
////		FVector SpawnParticleLocation = HitResult.Location;
////		FRotator SpawnParticleRotation = UKismetMathLibrary::Conv_VectorToRotator(HitResult.Normal);
////		FVector SpawnParticleScale = FVector(.5f, .5f, .5f);
////		switch (HistPhysicalMaterial->SurfaceType)
////		{
////			case EPhysicalSurface::SurfaceType1:
////				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DirtParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DirtSound, SpawnParticleLocation);
////				UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
////				break;
////			case EPhysicalSurface::SurfaceType2:
////				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GrassParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), GrassSound, SpawnParticleLocation);
////				UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
////				break;
////			case EPhysicalSurface::SurfaceType3:
////				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RockParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), RockSound, SpawnParticleLocation);
////				UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
////				break;
////			case EPhysicalSurface::SurfaceType4:
////				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MetalParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), MetalSound, SpawnParticleLocation);
////				UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
////				break;
////			case EPhysicalSurface::SurfaceType5:
////				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WoodParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), WoodSound, SpawnParticleLocation);
////				UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
////				break;
////			case EPhysicalSurface::SurfaceType6:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
////				break;
////			case EPhysicalSurface::SurfaceType7:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HeadShotSound, SpawnParticleLocation);
////				break;
////			case EPhysicalSurface::SurfaceType8:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
////				break;
////			case EPhysicalSurface::SurfaceType9:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
////				break;
////			case EPhysicalSurface::SurfaceType10:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
////				break;
////			case EPhysicalSurface::SurfaceType11:
////				UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
////				break;
////		default:
////			break;
////		}
////	}
////}
//
////碰撞检测
//void AWeaponBase::ShootEvent()
//{
//	// 1) 减少弹匣内子弹数（建议你外层先判空弹再调用 ShootEvent）
//	CurrentAmmoInMag--;
//
//	// 2) 播放开火动画
//	UAnimationAsset* WeaponShootAnim = WeaponStruct->FireAnimation;
//	if (WeaponShootAnim && WeaponSkeletal)
//	{
//		WeaponSkeletal->PlayAnimation(WeaponShootAnim, false);
//	}
//
//	// 3) 空弹壳生成
//	SpawnShellEject();
//
//	// 4) 计算准星命中点
//	AMSG_PlayerController* PC = WeaponOwner ? Cast<AMSG_PlayerController>(WeaponOwner->GetController()) : nullptr;
//	if (!PC || !GetWorld() || !WeaponSkeletal || !BulletPool) return;
//
//	// 屏幕中心反投影
//	FVector CamWorldOrigin = FVector::ZeroVector;
//	FVector CamWorldDir = FVector::ForwardVector;
//
//	int32 SizeX = 0, SizeY = 0;
//	PC->GetViewportSize(SizeX, SizeY);
//	const FVector2D ScreenCenter(SizeX * 0.5f, SizeY * 0.5f);
//
//	PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, CamWorldOrigin, CamWorldDir);
//	CamWorldDir = CamWorldDir.GetSafeNormal();
//
//	const float TraceDistance = 100000.f; // 1000m 按需调
//	const FVector TraceStart = CamWorldOrigin;
//	const FVector TraceEnd = TraceStart + CamWorldDir * TraceDistance;
//
//	FHitResult AimHit;
//	FCollisionQueryParams Params(SCENE_QUERY_STAT(AimTrace), true);
//	Params.AddIgnoredActor(WeaponOwner);
//	Params.AddIgnoredActor(this); // 武器本体
//	
//	const bool bHit = GetWorld()->LineTraceSingleByChannel(
//		AimHit,
//		TraceStart,
//		TraceEnd,
//		ECC_GameTraceChannel1,
//		Params
//	);
//
//	// AimPoint命中点或远点
//	const FVector AimPoint = bHit ? AimHit.ImpactPoint : TraceEnd;
//
//	// 5) 枪口到 AimPoint 做二次校正，避免枪口视差/近距离遮挡导致偏移
//	FHitResult MuzzleHit;
//	FCollisionQueryParams MuzzleParams(SCENE_QUERY_STAT(MuzzleTrace), true);
//	MuzzleParams.AddIgnoredActor(WeaponOwner);
//	MuzzleParams.AddIgnoredActor(this);
//
//	const FVector MuzzleLocation = WeaponSkeletal->GetSocketLocation(TEXT("MUZZLE"));
//	const FVector MuzzleTraceStart = MuzzleLocation;
//	const FVector MuzzleTraceEnd = AimPoint; // 只追到 AimPoint，不追更远
//
//	const bool bMuzzleHit = GetWorld()->LineTraceSingleByChannel(
//		MuzzleHit,
//		MuzzleTraceStart,
//		MuzzleTraceEnd,
//		ECC_Visibility,
//		MuzzleParams
//	);
//
//	// 真正子弹应该瞄准的点：如果枪口到 AimPoint 路上有遮挡，就瞄准遮挡点
//	const FVector MuzzleAimPoint = bMuzzleHit ? MuzzleHit.ImpactPoint : AimPoint;
//
//	// 重新计算 ShootDir（用校正后的目标点）
//	FVector ShootDir = (MuzzleAimPoint - MuzzleLocation).GetSafeNormal();
//	if (ShootDir.IsNearlyZero())
//	{
//		ShootDir = CamWorldDir;
//	}
//
//	// 子弹出生点前移，避免一出生就撞到玩家/枪
//	const float MuzzleForwardOffset = 20.f;
//	const FVector BulletSpawnLoc = MuzzleLocation + ShootDir * MuzzleForwardOffset;
//
//
//
//	// 6) 激活子弹（由 Bullet 的 OnHit 做真实命中反馈/伤害）
//	if (ABullet* Bullet = BulletPool->GetBullet())
//	{
//		// 给子弹设置 Owner，便于 IgnoreActorWhenMoving 生效
//		Bullet->SetOwner(WeaponOwner);
//
//		const float BulletSpeed = 5000.f; 
//		Bullet->ActivateBullet(BulletSpawnLoc, ShootDir.Rotation(), BulletSpeed, this);
//	}
//}
//
//UParticleSystem* AWeaponBase::GetShellEffect() 
//{
//	if (WeaponStruct && WeaponStruct->WeaponShell)
//	{
//		return WeaponStruct->WeaponShell;
//	}
//	return nullptr;
//}
//
//void AWeaponBase::Reload()
//{
//	//计算需要补充的子弹数
//	int32 AmmoNeeded = WeaponStruct->MagazineSize - CurrentAmmoInMag;
//	if (TotalAmmo >= AmmoNeeded)
//	{
//		CurrentAmmoInMag += AmmoNeeded;
//		TotalAmmo -= AmmoNeeded;
//	}
//	else
//	{
//		CurrentAmmoInMag += TotalAmmo;
//		TotalAmmo = 0;
//	}
//}
//
//void AWeaponBase::PlayReloadMontage()
//{
//	//从蒙太奇数组中随机播放一个蒙太奇
//
//	if (ReloadMontages.Num() == 0) return;
//	int32 RandomIndex = UKismetMathLibrary::RandomInteger(ReloadMontages.Num() - 1);
//
//	UAnimationAsset* SelectedMontage = ReloadMontages[RandomIndex];
//
//	// 检查蒙太奇是否有效
//	if (!SelectedMontage)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Selected reload montage is null at index %d"), RandomIndex);
//		return;
//	}
//
//	// 播放蒙太奇
//	if (WeaponSkeletal->GetAnimInstance())
//	{
//		WeaponSkeletal->PlayAnimation(SelectedMontage,false);
//
//		UE_LOG(LogTemp, Log, TEXT("Playing reload montage: %s"), *SelectedMontage->GetName());
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Weapon anim instance not found!"));
//	}
//
//}
//
//void AWeaponBase::HandleBulletHit(const FHitResult& Hit)
//{
//	UPhysicalMaterial* HistPhysicalMaterial = Hit.PhysMaterial.Get();
//	AActor* HitActor = Hit.GetActor();
//	UPrimitiveComponent* HitComp = Hit.GetComponent();
//
//	UE_LOG(LogTemp, Warning, TEXT("[Hit] Actor=%s Comp=%s Bone=%s PM=%s Surface=%d"),
//		HitActor ? *HitActor->GetName() : TEXT("None"),
//		HitComp ? *HitComp->GetName() : TEXT("None"),
//		*Hit.BoneName.ToString(),
//		HistPhysicalMaterial ? *HistPhysicalMaterial->GetName() : TEXT("None"),
//		HistPhysicalMaterial ? (int32)HistPhysicalMaterial->SurfaceType : -1
//	);
//
//	// 伤害处理：打到僵尸才算
//	if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(HitActor))
//	{
//		if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(WeaponOwner->GetController()))
//		{
//			float Damage = WeaponStruct->AttackDamage;
//			bool bHeadShot = false;
//
//			if (HistPhysicalMaterial)
//			{
//				switch (HistPhysicalMaterial->SurfaceType)
//				{
//				case EPhysicalSurface::SurfaceType7:
//					Damage *= WeaponStruct->HeadshotMultiplier;
//					bHeadShot = true;
//					break;
//				case EPhysicalSurface::SurfaceType8:
//					Damage *= WeaponStruct->ArmMultiplier;
//					break;
//				case EPhysicalSurface::SurfaceType9:
//					Damage *= WeaponStruct->LegMultiplier;
//					break;
//				case EPhysicalSurface::SurfaceType10:
//					Damage *= WeaponStruct->ChestMultiplier;
//					break;
//				case EPhysicalSurface::SurfaceType11:
//					Damage *= WeaponStruct->AbsMultiplier;
//					break;
//				default:
//					break;
//				}
//			}
//
//			Damage *= Zombie->DamageReductionFactor;
//
//			Zombie->TakeDamage(Damage, FDamageEvent(), PC, this);
//
//			const bool bDead = Zombie->IsDead();
//
//			if (bDead)
//			{
//				if (bHeadShot)
//				{
//					UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(true);
//					UMSG_EventManager::GetEventManagerInstance()->HeadShotEventDelegate.ExecuteIfBound();
//				}
//				else
//				{
//					UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(false);
//				}
//			}
//			else
//			{
//				UMSG_EventManager::GetEventManagerInstance()->ShowHitFeedbackDelegate.ExecuteIfBound(bHeadShot);
//			}
//
//			// 生成血液特效
//			UGameplayStatics::SpawnEmitterAtLocation(
//				GetWorld(),
//				FleshParticle,
//				Hit.Location,
//				UKismetMathLibrary::Conv_VectorToRotator(Hit.Normal),
//				FVector(.5f, .5f, .5f));
//		}
//	}
//
//	// 命中材质反馈（粒子/声音/贴花）
//	if (HistPhysicalMaterial)
//	{
//		const FVector SpawnParticleLocation = Hit.Location;
//		const FRotator SpawnParticleRotation = UKismetMathLibrary::Conv_VectorToRotator(Hit.Normal);
//		const FVector SpawnParticleScale = FVector(.5f, .5f, .5f);
//
//		switch (HistPhysicalMaterial->SurfaceType)
//		{
//		case EPhysicalSurface::SurfaceType1:
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DirtParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DirtSound, SpawnParticleLocation);
//			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
//			break;
//		case EPhysicalSurface::SurfaceType2:
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GrassParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), GrassSound, SpawnParticleLocation);
//			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
//			break;
//		case EPhysicalSurface::SurfaceType3:
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RockParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RockSound, SpawnParticleLocation);
//			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
//			break;
//		case EPhysicalSurface::SurfaceType4:
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MetalParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MetalSound, SpawnParticleLocation);
//			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
//			break;
//		case EPhysicalSurface::SurfaceType5:
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WoodParticle, SpawnParticleLocation, SpawnParticleRotation, SpawnParticleScale);
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WoodSound, SpawnParticleLocation);
//			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalMaterial, FVector(5.f, 5.f, 5.f), SpawnParticleLocation, SpawnParticleRotation);
//			break;
//		case EPhysicalSurface::SurfaceType6:
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
//			break;
//		case EPhysicalSurface::SurfaceType7:
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HeadShotSound, SpawnParticleLocation);
//			break;
//		case EPhysicalSurface::SurfaceType8:
//		case EPhysicalSurface::SurfaceType9:
//		case EPhysicalSurface::SurfaceType10:
//		case EPhysicalSurface::SurfaceType11:
//			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FleshSound, SpawnParticleLocation);
//			break;
//		default:
//			break;
//		}
//	}
//}
