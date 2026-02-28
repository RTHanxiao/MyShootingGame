// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShootingGame/Character/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include <EnhancedInputSubsystems.h>

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "../Logic/MSG_EventManager.h"
#include "../GameCore/MSG_PlayerController.h"

#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"

#include "Items/Inv_InventoryItem.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Fragments/Inv_ItemFragment.h"

#include "GameplayTagContainer.h"
#include "MyShootingGame/Items/Weapon/Inv_EquipWeaponActor.h"
#include "MyShootingGame/Items/Weapon/Inv_WeaponFragments.h"
#include "MyShootingGame/Logic/Fire/MSG_FireRule.h"
#include "MyShootingGame/Logic/Fire/MSG_FireRuleComponent.h"
#include "Items/Component/Inv_ItemComponent.h"



static FGameplayTag GetRifleRootTag()
{
	return FGameplayTag::RequestGameplayTag(TEXT("GameItems.Equipment.Weapons.Rifle"));
}

APlayerCharacter::APlayerCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	UMSG_EventManager::GetEventManagerInstance()->ChangeHallCharacterMeshDelegate.BindUObject(this, &APlayerCharacter::SetSkeletalMesh);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultArmLength;
	SpringArmComp->SocketOffset.Y = DefaultSocketOffsetY;
	SpringArmComp->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArmComp);

	DeathCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCameraSpringArm"));
	DeathCameraSpringArm->SetupAttachment(RootComponent);
	DeathCameraSpringArm->TargetArmLength = 300.f;
	DeathCameraSpringArm->bDoCollisionTest = false;

	DeathCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera"));
	DeathCamera->SetupAttachment(DeathCameraSpringArm);

	FireRuleComp = CreateDefaultSubobject<UMSG_FireRuleComponent>(TEXT("FireRule"));

	CurrentWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CurrentWeapon"));
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSlot"));

	CurrentArmLength = DefaultArmLength;
	CurrentSocketOffsetY = DefaultSocketOffsetY;

	CrossHairOnMoveMappingMax = GetCharacterMovement()->MaxWalkSpeed;

	CurrentFOV = DefaulFOV;
	TargetFOV = DefaulFOV;
	FOVLerpSpeed = 8.0f;
	bIsFOVTransitioning = false;

	if (Camera)
	{
		Camera->SetFieldOfView(CurrentFOV);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetMappingContext(true);

	CachedEquipmentComp = GetEquipmentComponent();
	BindInventoryEquipEvents();

	// 初始同步一次（防止进场已有装备但没有触发广播）
	RefreshActiveWeaponFromEquipment();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrosshair(DeltaTime);
	UpdateCamera(DeltaTime);
	UpdateFOV(DeltaTime);

	if (bAiming)
	{
		FaceToCameraDirectionSmoothly(DeltaTime);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerMove);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerLook);
		}
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::PlayerAttack);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopFire);
		}
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::Reload);
		}
		if (ShootModeAction)
		{
			EnhancedInputComponent->BindAction(ShootModeAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchShootMode);
		}
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAim);
		}
		if (EquipAction)
		{
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &APlayerCharacter::ChangeWeaponState);
		}
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
		}
		if (ToggleInventoryAction)
		{
			EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
		}
		if (SwitchPrimaryWeaponAction)
		{
			EnhancedInputComponent->BindAction(SwitchPrimaryWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchToPrimaryWeapon);
		}
		if (SwitchSecondaryWeaponAction)
		{
			EnhancedInputComponent->BindAction(SwitchSecondaryWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchToSecondaryWeapon);
		}
	}
}

void APlayerCharacter::PlayerMove(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetController()))
	{
		const FRotator Rotation = PC->GetControlRotation();
		const FVector Forward = FRotationMatrix(FRotator(0.f, Rotation.Yaw, 0.f)).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(FRotator(0.f, Rotation.Yaw, 0.f)).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, MoveVector.Y);
		AddMovementInput(Right, MoveVector.X);
	}
}

void APlayerCharacter::PlayerLook(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::BindInventoryEquipEvents()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipBind] PC null"));
		return;
	}

	CachedInventoryComp = PC->FindComponentByClass<UInv_InventoryComponent>();
	if (!CachedInventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipBind] InventoryComponent not found on PC"));
		return;
	}

	if (!CachedInventoryComp->OnItemEquip.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		CachedInventoryComp->OnItemEquip.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	if (!CachedInventoryComp->OnItemUnequip.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		CachedInventoryComp->OnItemUnequip.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}

	UE_LOG(LogTemp, Warning, TEXT("[EquipBind] Inventory equip events bound"));
}

void APlayerCharacter::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipEvent] OnItemEquipped: %s"), *GetNameSafe(EquippedItem));

	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[EquipEvent] EquipComp null"));
		return;
	}

	// 关键：刷新一下当前 Active 武器的 Item/Actor（确保已 Spawn 出 Actor）
	RefreshActiveWeaponFromEquipment();

	// 如果现在已经 armed 了，就别重复做（防止多次广播导致重复播动画/重复attach）
	if (bIsArmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipEvent] Already armed, skip auto-equip"));
		return;
	}

	// ✅ 只要当前 ActiveSlot 里有武器，就自动拿到手上
	UInv_InventoryItem* ActiveItem = EquipComp->GetActiveWeaponItem();
	AActor* ActiveActor = EquipComp->GetActiveWeaponActor();

	if (!IsValid(ActiveItem) || !IsValid(ActiveActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipEvent] ActiveItem/Actor invalid, skip auto-equip"));
		return;
	}

	// ✅ 你的规则：HandWeaponSlotIndex 0=收枪 1=主 2=副
	HandWeaponSlotIndex = (EquipComp->GetActiveSlot() == EInv_EquipSlot::Primary) ? 1 : 2;

	// ✅ 立刻把枪挂到 WeaponSlot，并把 bIsArmed 置 true、打开准星
	EquipWeapon();

	UE_LOG(LogTemp, Warning, TEXT("[EquipEvent] Auto armed. HandSlot=%d bIsArmed=%d"),
		HandWeaponSlotIndex, bIsArmed);
}

void APlayerCharacter::OnItemUnequipped(UInv_InventoryItem* UnequippedItem)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipEvent] OnItemUnequipped: %s"), *GetNameSafe(UnequippedItem));

	if (!CachedEquipmentComp) CachedEquipmentComp = GetEquipmentComponent();
	if (!CachedEquipmentComp) return;

	// 先刷新一下，看看当前 ActiveSlot 是否还存在武器
	UInv_InventoryItem* ActiveItem = CachedEquipmentComp->GetActiveWeaponItem();
	AActor* ActiveActor = CachedEquipmentComp->GetActiveWeaponActor();

	const bool bHasActive = IsValid(ActiveItem) && IsValid(ActiveActor);
	if (!bHasActive)
	{
		// ✅手上没枪了 -> 收枪状态
		if (bIsArmed)
		{
			if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
			{
				if (HolsterMontage) Anim->Montage_Play(HolsterMontage);
			}
			HolsterWeapon();
		}
		HandWeaponSlotIndex = 0;
	}
}

void APlayerCharacter::RefreshActiveWeaponFromEquipment()
{
	if (!CachedEquipmentComp) CachedEquipmentComp = GetEquipmentComponent();
	if (!CachedEquipmentComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RefreshActive] EquipComp null"));
		return;
	}

	UInv_InventoryItem* ActiveItem = CachedEquipmentComp->GetActiveWeaponItem();
	AActor* ActiveActor = CachedEquipmentComp->GetActiveWeaponActor();

	UE_LOG(LogTemp, Warning, TEXT("[RefreshActive] ActiveItem=%s ActiveActor=%s"),
		*GetNameSafe(ActiveItem),
		*GetNameSafe(ActiveActor));
}

UInv_EquipmentComponent* APlayerCharacter::GetEquipmentComponent() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	return PC ? PC->FindComponentByClass<UInv_EquipmentComponent>() : nullptr;
}

AInv_EquipWeaponActor* APlayerCharacter::GetActiveEquipWeaponActor() const
{
	if (UInv_EquipmentComponent* EquipComp = GetEquipmentComponent())
	{
		return Cast<AInv_EquipWeaponActor>(EquipComp->GetActiveWeaponActor());
	}
	return nullptr;
}

void APlayerCharacter::PlayerAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Attack] PlayerAttack triggered"));

	if (bOnHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] Abort: bOnHit"));
		return;
	}

	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] EquipComp is NULL"));
		return;
	}

	UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	AActor* EquipActor = EquipComp->GetActiveWeaponActor();

	UE_LOG(LogTemp, Warning, TEXT("[Attack] ActiveItem=%s  ActiveActor=%s"),
		*GetNameSafe(Item),
		*GetNameSafe(EquipActor));

	if (!IsValid(Item) || !IsValid(EquipActor))
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] Abort: Item or EquipActor invalid"));
		return;
	}

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	const FInv_EquipmentFragment* EquipFrag = Manifest.GetFragmentOfType<FInv_EquipmentFragment>();
	if (!EquipFrag)
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] No EquipmentFragment on Item"));
		return;
	}

	const FGameplayTag EquipType = EquipFrag->GetEquipmentType();
	UE_LOG(LogTemp, Warning, TEXT("[Attack] EquipmentType = %s"), *EquipType.ToString());

	const FGameplayTag RifleRoot = GetRifleRootTag();
	UE_LOG(LogTemp, Warning, TEXT("[Attack] RifleRoot = %s  Matches=%d"),
		*RifleRoot.ToString(),
		EquipType.MatchesTag(RifleRoot) ? 1 : 0);

	if (EquipType.IsValid() && EquipType.MatchesTag(RifleRoot))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] → Fire()"));
		Fire();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Attack] → Melee()"));
	Melee();
}

void APlayerCharacter::Melee()
{
	// TODO
}

void APlayerCharacter::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("[Fire] Enter Fire"));

	if (bOnHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Abort: bOnHit"));
		return;
	}

	if (!bIsArmed)
	{
		UE_LOG(LogTemp, Error, TEXT("[Fire] Abort: bIsArmed == false (HandSlot=%d)"), HandWeaponSlotIndex);
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(FireRateHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Abort: Timer already active"));
		return;
	}

	AInv_EquipWeaponActor* WeaponActor = GetActiveEquipWeaponActor();
	UE_LOG(LogTemp, Warning, TEXT("[Fire] ActiveWeaponActor = %s"), *GetNameSafe(WeaponActor));

	if (!IsValid(WeaponActor))
	{
		UE_LOG(LogTemp, Error, TEXT("[Fire] Abort: WeaponActor invalid"));
		return;
	}

	// ✅ 空弹匣直接返回：避免无意义规则评估
	if (!WeaponActor->HasAmmoInMag())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Empty mag"));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyMagSound, GetActorLocation());
		return;
	}

	// --- FireRule: 构建上下文 ---
	FMSG_FireRuleContext Ctx;
	Ctx.bOnHit = bOnHit;
	Ctx.bIsArmed = bIsArmed;
	Ctx.bWeaponAutoFire = WeaponActor->IsAutoFire();
	Ctx.WeaponAttackRate = WeaponActor->GetAttackRate();
	Ctx.CrossHairSpreadIncrement = CrossHairSpreadIncrement;
	Ctx.bIsAiming = bAiming;
	// Ctx.bIsSprinting = bIsSprinting;
	// Ctx.bIsReloading = bIsReloading;

	FMSG_FireRuleResult Rule; // 默认：允许开火，不覆盖参数
	if (FireRuleComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FireRule] Comp=%s"), *GetNameSafe(FireRuleComp));
		Rule = FireRuleComp->EvaluateRule(Ctx);
	}

	UE_LOG(LogTemp, Warning, TEXT("[FireRule] Result: CanFire=%d RateOv=%.3f SpreadOv=%.3f"),
		Rule.bCanFire, Rule.FireIntervalOverride, Rule.SpreadIncrementOverride);

	if (!Rule.bCanFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Abort: blocked by FireRule"));
		return;
	}

	// 扩散增量覆盖
	const float SpreadInc = (Rule.SpreadIncrementOverride >= 0.f)
		? Rule.SpreadIncrementOverride
		: CrossHairSpreadIncrement;

	UE_LOG(LogTemp, Warning, TEXT("[Fire] ShootOnce (first shot)"));
	WeaponActor->ShootOnce();

	StartPitchRecoilEvent(CheckPitchOffset());
	CrossHairSpreadInc(SpreadInc);

	if (!WeaponActor->IsAutoFire())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Single fire weapon, stop here"));
		return;
	}

	// 自动射速覆盖
	const float RateBase = WeaponActor->GetAttackRate();
	const float RateLua = (Rule.FireIntervalOverride > 0.f) ? Rule.FireIntervalOverride : RateBase;
	const float Rate = FMath::Max(0.01f, RateLua);

	UE_LOG(LogTemp, Warning, TEXT("[Fire] AutoFire ON, Rate = %.3f"), Rate);

	GetWorld()->GetTimerManager().SetTimer(
		FireRateHandle,
		[this, SpreadInc]()
		{
			AInv_EquipWeaponActor* CurWeaponActor = GetActiveEquipWeaponActor();
			UE_LOG(LogTemp, Warning, TEXT("[Fire-Timer] Tick WeaponActor=%s"), *GetNameSafe(CurWeaponActor));

			if (!IsValid(CurWeaponActor))
			{
				UE_LOG(LogTemp, Error, TEXT("[Fire-Timer] WeaponActor invalid → StopFire"));
				StopFire();
				return;
			}

			if (!CurWeaponActor->HasAmmoInMag())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Fire-Timer] Empty mag → StopFire"));
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyMagSound, GetActorLocation());
				StopFire();
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("[Fire-Timer] ShootOnce"));
			CurWeaponActor->ShootOnce();

			StartPitchRecoilEvent(CheckPitchOffset());
			CrossHairSpreadInc(SpreadInc);

			if (!CurWeaponActor->IsAutoFire())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Fire-Timer] AutoFire OFF → StopFire"));
				StopFire();
			}
		},
		Rate,
		true,
		Rate
	);
}

void APlayerCharacter::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateHandle);
	CurPitchOffsetValue = 0.f;
}

bool APlayerCharacter::CanReload() const
{
	if (!bIsArmed) return false;
	if (bReloading) return false;

	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return false;

	UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return false;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	const FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfType<FInv_WeaponAmmoFragment>();
	return Ammo && Ammo->CanReload();
}

void APlayerCharacter::Reload()
{
	if (!bIsArmed) return;
	if (!CanReload()) return;

	StopFire();
	bReloading = true;

	if (UAnimInstance* CurAnimInstance = GetMesh()->GetAnimInstance())
	{
		if (ReloadMontage)
		{
			CurAnimInstance->Montage_Play(ReloadMontage);
		}
	}
}

void APlayerCharacter::Notify_ReloadWeaponAnim()
{
	AInv_EquipWeaponActor* WeaponActor = GetActiveEquipWeaponActor();
	if (IsValid(WeaponActor))
	{
		WeaponActor->PlayWeaponReloadAnim();
	}
}


FMSG_FireRuleResult APlayerCharacter::EvaluateFireRule_Implementation(const FMSG_FireRuleContext& Ctx)
{
	return FMSG_FireRuleResult{};
}

void APlayerCharacter::CommitReload()
{
	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return;

	UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return;

	FInv_ItemManifest& Manifest = Item->GetItemManifestMutable();
	FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfTypeMutable<FInv_WeaponAmmoFragment>();
	if (!Ammo) return;

	Ammo->Reload();
}

void APlayerCharacter::EndReload()
{
	bReloading = false;
}

void APlayerCharacter::SwitchShootMode()
{
	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return;

	UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return;

	FInv_ItemManifest& Manifest = Item->GetItemManifestMutable();
	FInv_WeaponFireControlFragment* FireCtrl = Manifest.GetFragmentOfTypeMutable<FInv_WeaponFireControlFragment>();
	if (!FireCtrl) return;

	FireCtrl->bAutoFire = !FireCtrl->bAutoFire;

	StopFire();
	UMSG_EventManager::GetEventManagerInstance()->SetShootModeUIDelegate.ExecuteIfBound(FireCtrl->bAutoFire);
}

void APlayerCharacter::StartAim()
{
	if (!bIsArmed) return;

	CurLerpSpeed = AimLerpSpeed;
	CurrentArmLength = AimArmLength;
	CurrentSocketOffsetY = AimSocketOffsetY;
	SetFOV(1.5f);

	bAiming = true;
}

void APlayerCharacter::StopAim()
{
	if (!bIsArmed) return;

	CurrentArmLength = WeaponArmLength;
	CurrentSocketOffsetY = WeaponSocketOffsetY;
	ResetFOV();

	bAiming = false;
}

void APlayerCharacter::ChangeWeaponState()
{
	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim) return;

	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!IsValid(EquipComp)) return;

	if (bIsArmed)
	{
		if (HolsterMontage) Anim->Montage_Play(HolsterMontage);
		StopFire();
		HolsterWeapon();
		HandWeaponSlotIndex = 0;
		return;
	}

	// 收枪状态 -> 拿起当前 ActiveSlot
	// 需要 EquipComp 提供 GetActiveSlot()；如果你没有这个接口，就把这里改成你现有的取槽方式
	const EInv_EquipSlot CurSlot = EquipComp->GetActiveSlot();
	const int32 SlotIndex = (CurSlot == EInv_EquipSlot::Primary) ? 1 : 2;

	if (!IsValid(EquipComp->GetActiveWeaponActor()) || !IsValid(EquipComp->GetActiveWeaponItem()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipAction] ActiveSlot has no weapon, abort"));
		return;
	}

	if (DrawMontage) Anim->Montage_Play(DrawMontage);
	HandWeaponSlotIndex = SlotIndex;
	EquipWeapon();
}

void APlayerCharacter::AttachActorToSocket(AActor* Actor, FName SocketName, const FRotator& RelRot) const
{
	if (!IsValid(Actor) || !IsValid(GetMesh())) return;

	Actor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	Actor->SetActorRelativeRotation(RelRot);
}

void APlayerCharacter::EquipWeapon()
{
	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!IsValid(EquipComp)) return;

	AActor* ActiveActor = EquipComp->GetActiveWeaponActor();
	if (!IsValid(ActiveActor)) return;

	AttachActorToSocket(ActiveActor, TEXT("WeaponSlot"), FRotator(0.f, 0.f, 0.f));

	// ✅ 兜底：外部没设置 HandWeaponSlotIndex 时，按 ActiveSlot 推一次
	if (HandWeaponSlotIndex == 0)
	{
		const EInv_EquipSlot CurSlot = EquipComp->GetActiveSlot();
		HandWeaponSlotIndex = (CurSlot == EInv_EquipSlot::Primary) ? 1 : 2;
	}

	bIsArmed = true;
	CurrentArmLength = WeaponArmLength;
	CurrentSocketOffsetY = WeaponSocketOffsetY;
	CurLerpSpeed = CameraLerpSpeed;

	UMSG_EventManager::GetEventManagerInstance()->ShowCrossHairDelegate.ExecuteIfBound(true);
	UMSG_EventManager::GetEventManagerInstance()->SwitchGunUIDelegate.ExecuteIfBound();
}

void APlayerCharacter::HolsterWeapon()
{
	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!IsValid(EquipComp)) return;

	AActor* ActiveActor = EquipComp->GetActiveWeaponActor();
	if (!IsValid(ActiveActor)) return;

	AttachActorToSocket(ActiveActor, TEXT("Weapon_R"), FRotator::ZeroRotator);

	bIsArmed = false;
	HandWeaponSlotIndex = 0;

	CurrentArmLength = DefaultArmLength;
	CurrentSocketOffsetY = DefaultSocketOffsetY;
	CurLerpSpeed = CameraLerpSpeed;

	// ✅ 收枪强制退镜
	bAiming = false;
	ResetFOV();

	UMSG_EventManager::GetEventManagerInstance()->ShowCrossHairDelegate.ExecuteIfBound(false);
	UMSG_EventManager::GetEventManagerInstance()->SwitchGunUIDelegate.ExecuteIfBound();
}

void APlayerCharacter::ToggleWeaponSlot(int32 TargetSlotIndex)
{
	if (TargetSlotIndex != 1 && TargetSlotIndex != 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] Invalid TargetSlotIndex=%d"), TargetSlotIndex);
		return;
	}

	UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!IsValid(EquipComp))
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponSlot] EquipComp NULL"));
		return;
	}

	StopFire();

	// 目标槽（1=Primary,2=Secondary）
	const EInv_EquipSlot TargetSlot = (TargetSlotIndex == 1) ? EInv_EquipSlot::Primary : EInv_EquipSlot::Secondary;

	// 关键：如果“同槽再次按下” => 收枪
	if (HandWeaponSlotIndex == TargetSlotIndex && bIsArmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] Same slot pressed -> Holster (Slot=%d)"), TargetSlotIndex);

		EquipComp->SetActiveSlot(TargetSlot); // 确保 ActiveSlot 对应正确 Actor
		HolsterWeapon();

		HandWeaponSlotIndex = 0;
		return;
	}

	// 如果当前手上有枪，但要切换到另一个槽：先把当前 active 的那把收起来（按你的 socket 方案）
	if (bIsArmed && HandWeaponSlotIndex != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] Switching hand weapon %d -> %d, holster current first"),
			HandWeaponSlotIndex, TargetSlotIndex);

		// 把“当前手上那把”收起来：这里假设它一定是 EquipComp 当前 ActiveSlot 的 Actor
		// 为了更稳：先把 ActiveSlot 切到当前手上槽，再 Holster
		const EInv_EquipSlot CurHandSlot = (HandWeaponSlotIndex == 1) ? EInv_EquipSlot::Primary : EInv_EquipSlot::Secondary;
		EquipComp->SetActiveSlot(CurHandSlot);
		HolsterWeapon();
	}

	// 切到目标槽成为 ActiveSlot
	EquipComp->SetActiveSlot(TargetSlot);

	// 检查目标槽有没有武器实例/Actor
	UInv_InventoryItem* TargetItem = EquipComp->GetActiveWeaponItem();
	AActor* TargetActor = EquipComp->GetActiveWeaponActor();

	UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] TargetSlot=%d ActiveItem=%s ActiveActor=%s"),
		TargetSlotIndex, *GetNameSafe(TargetItem), *GetNameSafe(TargetActor));

	if (!IsValid(TargetItem) || !IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] Target slot has no weapon -> stay holstered"));
		HandWeaponSlotIndex = 0;
		bIsArmed = false;
		return;
	}

	// 拿起目标槽武器
	EquipWeapon();
	HandWeaponSlotIndex = TargetSlotIndex;

	UE_LOG(LogTemp, Warning, TEXT("[WeaponSlot] Equipped slot in hand: %d"), HandWeaponSlotIndex);
}

void APlayerCharacter::SwitchToPrimaryWeapon()
{
	ToggleWeaponSlot(1);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	ToggleWeaponSlot(2);
}

void APlayerCharacter::Interact()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetController()))
	{
		if (!PickItem) return;

		UInv_ItemComponent* ItemComp = PickItem->FindComponentByClass<UInv_ItemComponent>();
		if (!ItemComp) return;

		PC->GetInventoryComponent().Get()->TryAddItem(ItemComp);
	}
}

void APlayerCharacter::ToggleInventory()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetController()))
	{
		PC->ToggleInventory();
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	PlayRandomMontageFromArray(OnHitMontages);
	bOnHit = true;

	UMSG_EventManager::GetEventManagerInstance()->PlayerInjuredDelegate.ExecuteIfBound();

	if (GetCurrentHealth() <= 0.f)
	{
		DeathCameraEvent();
	}
	return DamageAmount;
}

FGameplayTag APlayerCharacter::GetCurrentWeaponTypeTag() const
{
	if (UInv_EquipmentComponent* EquipComp = GetEquipmentComponent())
	{
		if (UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem())
		{
			const FInv_ItemManifest& Manifest = Item->GetItemManifest();
			if (const FInv_EquipmentFragment* EquipFrag = Manifest.GetFragmentOfType<FInv_EquipmentFragment>())
			{
				return EquipFrag->GetEquipmentType();
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

void APlayerCharacter::UpdateFOV(float DeltaTime)
{
	if (FMath::Abs(CurrentFOV - TargetFOV) > 0.1f)
	{
		bIsFOVTransitioning = true;
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVLerpSpeed);

		if (Camera) Camera->SetFieldOfView(CurrentFOV);
	}
	else if (bIsFOVTransitioning)
	{
		bIsFOVTransitioning = false;
		CurrentFOV = TargetFOV;
		if (Camera) Camera->SetFieldOfView(CurrentFOV);
	}
}

void APlayerCharacter::UpdateCamera(float DeltaTime)
{
	SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, CurrentArmLength, DeltaTime, CurLerpSpeed);
	SpringArmComp->SocketOffset.Y = FMath::FInterpTo(SpringArmComp->SocketOffset.Y, CurrentSocketOffsetY, DeltaTime, CurLerpSpeed);
}

void APlayerCharacter::SetFOV(float ZoomRate)
{
	if (ZoomRate <= 0.0f) return;

	const float NewFOV = DefaulFOV / ZoomRate;
	TargetFOV = FMath::Clamp(NewFOV, MinFOV, MaxFOV);
}

void APlayerCharacter::ResetFOV()
{
	TargetFOV = DefaulFOV;
}

void APlayerCharacter::UpdateCrosshair(float DeltaTime)
{
	if (CurCrossHairSpread >= MinCrossHairSpread)
	{
		CrossHairSpreadDec(CrossHairSpreadDecrement * DeltaTime);
	}

	const float CurSpeed = UKismetMathLibrary::VSize(GetVelocity());
	const float MapValue = UKismetMathLibrary::MapRangeClamped(
		CurSpeed,
		CrossHairOnMoveMappingMin,
		CrossHairOnMoveMappingMax,
		MinCrossHairSpread,
		MaxCrossHairSpread
	);

	CrossHairSpreadInc(MapValue);
}

void APlayerCharacter::CrossHairSpreadInc(float SpreadIncVal)
{
	if (CurCrossHairSpread < MaxCrossHairSpread)
	{
		CurCrossHairSpread += SpreadIncVal;
	}
}

void APlayerCharacter::CrossHairSpreadDec(float SpreadDecVal)
{
	if (CurCrossHairSpread > MinCrossHairSpread)
	{
		CurCrossHairSpread -= SpreadDecVal;
	}
}

void APlayerCharacter::SetMappingContext(bool bBind)
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (bBind) Subsystem->AddMappingContext(PlayerMappingContext, 0);
			else Subsystem->RemoveMappingContext(PlayerMappingContext);
		}
	}
}

bool APlayerCharacter::CheckPitchOffset()
{
	return UKismetMathLibrary::Abs(CurPitchOffsetValue) > 10.f;
}

void APlayerCharacter::FaceToCameraDirectionSmoothly(float DeltaTime)
{
	AController* Ctrl = GetController();
	if (!Ctrl) return;

	const float TargetYaw = Ctrl->GetControlRotation().Yaw;
	const float CurrentYaw2 = GetActorRotation().Yaw;
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw2, TargetYaw);

	const float MaxYawSpeedDegPerSec = 720.f;
	const float Step = FMath::Clamp(MaxYawSpeedDegPerSec * DeltaTime, 0.f, FMath::Abs(DeltaYaw));
	const float NewYaw = CurrentYaw2 + FMath::Sign(DeltaYaw) * Step;

	SetActorRotation(FRotator(0.f, NewYaw, 0.f));
}

void APlayerCharacter::SetSkeletalMesh(USkeletalMesh* NewMesh)
{
	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

int32 APlayerCharacter::GetCurrentAmmoInMag() const
{
	const UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return 0;

	const UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return 0;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	if (const FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfType<FInv_WeaponAmmoFragment>())
	{
		return Ammo->CurrentAmmoInMag;
	}
	return 0;
}

int32 APlayerCharacter::GetCurrentMagazineSize() const
{
	const UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return 0;

	const UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return 0;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	if (const FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfType<FInv_WeaponAmmoFragment>())
	{
		return Ammo->MagazineSize;
	}
	return 0;
}

int32 APlayerCharacter::GetCurrentTotalAmmo() const
{
	const UInv_EquipmentComponent* EquipComp = GetEquipmentComponent();
	if (!EquipComp) return 0;

	const UInv_InventoryItem* Item = EquipComp->GetActiveWeaponItem();
	if (!IsValid(Item)) return 0;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	if (const FInv_WeaponAmmoFragment* Ammo = Manifest.GetFragmentOfType<FInv_WeaponAmmoFragment>())
	{
		return Ammo->TotalAmmo;
	}
	return 0;
}
