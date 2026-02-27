//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "../Table/WeaponTemplate.h"
//#include "Particles/ParticleSystem.h"
//#include "../../../../../../../Source/Runtime/CoreUObject/Public/Templates/SubclassOf.h"
//#include "ItemBase.h"
//#include "WeaponBase.generated.h"
//
//class USceneComponent;
//class USkeletalMeshComponent;
//class USphereComponent;
//class UDataTable;
//class APlayerCharacter;
//class UParticleSystem;
//class UMaterialInterface;
//class USoundBase;
//class UBulletPool;
//
//UCLASS()
//class MYSHOOTINGGAME_API AWeaponBase : public AItemBase
//{
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	AWeaponBase();
//
//	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
//	//USceneComponent* SceneRoot;
//
//	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
//	USkeletalMeshComponent* WeaponSkeletal;
//
//	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
//	//USphereComponent* PickCollision;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
//	UBulletPool* BulletPool;
//
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	virtual void OnEnterInteractRange(
//		UPrimitiveComponent* OverlappedComponent,
//		AActor* OtherActor,
//		UPrimitiveComponent* OtherComp,
//		int32 OtherBodyIndex,
//		bool bFromSweep,
//		const FHitResult& SweepResult
//	) override;
//
//	virtual void OnLeaveInteractRange(
//		UPrimitiveComponent* OverlappedComponent,
//		AActor* OtherActor,
//		UPrimitiveComponent* OtherComp,
//		int32 OtherBodyIndex
//	) override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// 初始化武器数据
//	void InitWeaponData();
//	
//	// 获取弹壳特效 - 蓝图可调用
//	UFUNCTION(BlueprintCallable, Category = "Weapon")
//	UParticleSystem* GetShellEffect();
//
//	//生成弹壳
//	UFUNCTION(BlueprintImplementableEvent)
//	void SpawnShellEject();
//
//	UStaticMesh* GetStaticMesh() const { return WeaponStruct->WeaponMesh; }
//
//	FName GetWeaponName() const { return WeaponStruct->WeaponName; }
//
//	FText GetWeaponDescription() const { return WeaponStruct->WeaponDescription; }
//
//	UPROPERTY()
//	UDataTable* WeaponData;
//
//	FWeaponTemplateStructure*  WeaponStruct;
//
//	UPROPERTY(EditAnywhere)
//	FName WeaponRowName = FName(TEXT("AK47"));
//
//	//当前弹匣内子弹数
//	int32 CurrentAmmoInMag;
//	//总子弹数
//	int32 TotalAmmo;
//	//能否换弹
//	bool CanReload() const { return CurrentAmmoInMag < WeaponStruct->MagazineSize && TotalAmmo > 0; }
//	//换弹
//	UFUNCTION(BlueprintCallable)
//	void Reload();
//	//枪械换弹蒙太奇
//	UPROPERTY(EditDefaultsOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
//	TArray<UAnimationAsset*> ReloadMontages;
//	//播放换弹蒙太奇
//	UFUNCTION(BlueprintCallable)
//	void PlayReloadMontage();
//	//获取弹匣内子弹数
//	UFUNCTION(BlueprintCallable, Category = "Weapon")
//	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }
//	//获取总子弹数
//	UFUNCTION(BlueprintCallable, Category = "Weapon")
//	int32 GetTotalAmmo() const { return TotalAmmo; }
//	//设置总子弹数
//	void SetTotalAmmo(int32 NewTotalAmmo) { TotalAmmo = NewTotalAmmo; }
//	//设置弹匣内子弹数
//	void SetCurrentAmmoInMag(int32 NewAmmoInMag) { CurrentAmmoInMag = NewAmmoInMag; }
//	//弹道对象池当前Index
//	int32 BulletPoolIndex = 0;
//
//	//判断是否能开枪
//	bool EnoughAmmo() const { return CurrentAmmoInMag > 0; }
//
//	UFUNCTION()
//	void AttachGunToHand(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
//
//	UPROPERTY()
//	APlayerCharacter* WeaponOwner = nullptr;
//
//	//射击事件
//	void ShootEvent();
//
//	//处理子弹击中
//	void HandleBulletHit(const FHitResult& HitResult);
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// 地面子弹击中物理材质粒子
//	UPROPERTY(EditDefaultsOnly,Category = "PhysicalParticle")
//	UParticleSystem* DirtParticle;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalParticle")
//	UParticleSystem* GrassParticle;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalParticle")
//	UParticleSystem* RockParticle;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalParticle")
//	UParticleSystem* MetalParticle;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalParticle")
//	UParticleSystem* WoodParticle;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalParticle")
//	UParticleSystem* FleshParticle;
//
//	///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// 地面子弹击中物理材质音效
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* DirtSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* GrassSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* RockSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* MetalSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* WoodSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* FleshSound;
//	UPROPERTY(EditDefaultsOnly, Category = "PhysicalSound")
//	USoundBase* HeadShotSound;
//
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	//弹孔贴花
//	UPROPERTY(EditDefaultsOnly, Category = "Decal")
//	UMaterialInterface* BulletDecalMaterial;
//
//	//伤害类型
//	UPROPERTY(EditDefaultsOnly, Category = "Damage")
//	TSubclassOf<UDamageType> DamageTypeClass;
//
//	//垂直后座
//	UPROPERTY(BlueprintReadOnly, Category = "Recoil")
//	float VerticalRecoil = 0.f;
//
//	//水平后座
//	UPROPERTY(BlueprintReadOnly, Category = "Recoil")
//	float HorizontalRecoil = 0.f;
//
//	//开火模式false为单发，true为连发
//	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
//	bool ShootMode = true; 
//};
