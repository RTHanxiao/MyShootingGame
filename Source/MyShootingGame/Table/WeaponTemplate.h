#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponTemplate.generated.h"

//UENUM(BlueprintType)
//enum class EWeaponCategory : uint8 
//{
//	MELEE,
//	RIFLE,
//	PISTOL,
//	MACHINEGUN,
//	THROWABLE
//};

//UENUM(BlueprintType)
//enum class EWeaponType : uint8 
//{
//	FIST,
//
//	//Rifles
//	M4A1,
//	AK47,
//
//	//Pistols
//	GLOCK,
//
//	//Melee
//	KNIFE,
//
//	//Throwables
//	GRENADE
//};

USTRUCT(BlueprintType)
struct FWeaponTemplateStructure : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable) // 修正 UPROPOERTY/BluePrintReadOnly/EditDefaultOnly
	FName WeaponName;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	//EWeaponCategory WeaponCategory;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	//EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class USkeletalMesh* WeaponSkeletal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class UStaticMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float AttackDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float AttackRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	int32 Ammo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	int32 MagazineSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class UParticleSystem* WeaponShell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	class UTexture2D* WeaponIcon;

	//水平后座
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float RecoilYaw;

	//垂直后座
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float RecoilPitch;

	//武器简介
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	FText WeaponDescription;

	//不同部位的伤害倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float HeadshotMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float ChestMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float AbsMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float ArmMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable)
	float LegMultiplier;
};