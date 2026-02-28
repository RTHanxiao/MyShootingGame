// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/Character/MyCharacter.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "UnLuaInterface.h"
#include "../Logic/Fire/MSG_FireRUleOwnerInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

class UInv_InventoryComponent;
class UInv_EquipmentComponent;
class UInv_InventoryItem;

class AInv_EquipWeaponActor;
class UMSG_FireRuleComponent;
struct FMSG_ReloadDecision;
struct FMSG_ReloadContext;

/**
 *
 */
UCLASS()
class MYSHOOTINGGAME_API APlayerCharacter : public AMyCharacter, public IUnLuaInterface, public IMSG_FireRuleOwnerInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

public:
	UPROPERTY(BlueprintReadOnly)
	bool IsPickItemUIShow = false;

	UPROPERTY()
	AActor* PickItem = nullptr;

	// ===== Inventory / Equipment Access =====
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInv_EquipmentComponent* GetEquipmentComponent() const;

	// ===== Weapon Query (Tag / Ammo) =====
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FGameplayTag GetCurrentWeaponTypeTag() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmoInMag() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentMagazineSize() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentTotalAmmo() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ===== Components =====
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CurrentWeapon = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* DeathCamera = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* DeathCameraSpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMSG_FireRuleComponent> FireRuleComp;

	// ===== Input =====
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PlayerMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootModeAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleInventoryAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchPrimaryWeaponAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchSecondaryWeaponAction = nullptr;

	// ===== Assets (Anim / Sound) =====
	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShootMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HolsterMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> OnHitMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* EmptyMagSound = nullptr;

	// ===== Weapon State / Equip Events =====
	// 0 = Holster/Unarmed, 1 = Primary in hand, 2 = Secondary in hand
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 HandWeaponSlotIndex = 0;

	UFUNCTION()
	void OnItemEquipped(UInv_InventoryItem* EquippedItem);

	UFUNCTION()
	void OnItemUnequipped(UInv_InventoryItem* UnequippedItem);

	// 1/2 key actions -> toggle slot (same slot pressed while armed => holster)
	void SwitchToPrimaryWeapon();
	void SwitchToSecondaryWeapon();
	void ToggleWeaponSlot(int32 TargetSlotIndex); // 1=primary,2=secondary

	UPROPERTY()
	TObjectPtr<UInv_InventoryComponent> CachedInventoryComp = nullptr;

	UPROPERTY()
	TObjectPtr<UInv_EquipmentComponent> CachedEquipmentComp = nullptr;

	void BindInventoryEquipEvents();
	void RefreshActiveWeaponFromEquipment();

	// ===== Movement / Look =====
	void PlayerMove(const FInputActionValue& Value);
	void PlayerLook(const FInputActionValue& Value);

	// ===== Combat =====
	void PlayerAttack();
	void Melee();
	void Fire();
	void StopFire();

	UPROPERTY(EditAnywhere, Category = "UnLua")
	FString LuaModuleName = TEXT("Gameplay.FireRule");

	virtual FString GetModuleName_Implementation() const override { return LuaModuleName; }

	// 接口的默认实现（Lua 不工作时仍能跑）
	virtual FMSG_FireRuleResult EvaluateFireRule_Implementation(const FMSG_FireRuleContext& Ctx) override;

	// ===== Reload =====
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CommitReload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EndReload();

	void Reload();
	bool CanReload() const;

	// ===== Fire Mode =====
	void SwitchShootMode();

	// ===== Aim =====
	void StartAim();
	void StopAim();

	// ===== Equip / Holster =====
	void ChangeWeaponState();

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bReloading = false;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Notify_ReloadWeaponAnim();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void HolsterWeapon();

	void SwitchWeapon();

	// ===== Interact / UI =====
	void Interact();

	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

	// ===== Helpers =====
	AInv_EquipWeaponActor* GetActiveEquipWeaponActor() const;

	//void SwitchWeaponToSlot(int32 SlotIndex);

	void AttachActorToSocket(AActor* Actor, FName SocketName, const FRotator& RelRot) const;

	// ===== States =====
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bOnHit = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsArmed = false;

	FTimerHandle FireRateHandle;

	// ===== Camera =====
	float CurrentArmLength = 0.f;
	float CurrentSocketOffsetY = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CurrentFOV = 90.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float TargetFOV = 90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float FOVLerpSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bIsFOVTransitioning = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MinFOV = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxFOV = 120.0f;

	void UpdateFOV(float DeltaTime);
	void UpdateCamera(float DeltaTime);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float DefaulFOV = 90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float DefaultArmLength = 175.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float WeaponArmLength = 125.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float AimArmLength = 75.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float DefaultSocketOffsetY = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float WeaponSocketOffsetY = 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float AimSocketOffsetY = 20.f;

	float CurLerpSpeed = 2.0f;
	float CameraLerpSpeed = 2.0f;
	float AimLerpSpeed = 5.0f;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetFOV(float ZoomRate);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ResetFOV();

	// ===== Crosshair =====
	void UpdateCrosshair(float DeltaTime);

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CurCrossHairSpread = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxCrossHairSpread = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinCrossHairSpread = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CrossHairSpreadIncrement = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CrossHairOnMoveMappingMin = .0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CrossHairOnMoveMappingMax = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CrossHairSpreadDecrement = 0.5f;

	void CrossHairSpreadInc(float SpreadIncVal);
	void CrossHairSpreadDec(float SpreadDecVal);

	// ===== Misc =====
	void SetMappingContext(bool bBind);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void SetOnHit(bool OnHit) { bOnHit = OnHit; }

	UFUNCTION(BlueprintImplementableEvent)
	void DeathCameraEvent();

	bool bAiming = false;

	// 后坐力（你原来的）
	bool CheckPitchOffset();

	UPROPERTY(BlueprintReadWrite)
	float CurPitchOffsetValue = 0.f;

	UFUNCTION(BlueprintImplementableEvent)
	void StartPitchRecoilEvent(bool IsNeedLimit);

	void FaceToCameraDirectionSmoothly(float DeltaTime);

	void SetSkeletalMesh(USkeletalMesh* NewMesh);
};
