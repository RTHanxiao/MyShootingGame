#pragma once
#include "CoreMinimal.h"
#include "MSG_FireRule.generated.h"

USTRUCT(BlueprintType)
struct FMSG_FireRuleContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float WeaponAttackRate = 0.1f;
	UPROPERTY(BlueprintReadWrite) bool  bWeaponAutoFire = false;

	UPROPERTY(BlueprintReadWrite) bool  bOnHit = false;
	UPROPERTY(BlueprintReadWrite) bool  bIsArmed = false;

	// 你后面如果有瞄准/冲刺/换弹等状态，再加
	UPROPERTY(BlueprintReadWrite) bool  bIsAiming = false;
	UPROPERTY(BlueprintReadWrite) bool  bIsSprinting = false;
	UPROPERTY(BlueprintReadWrite) bool  bIsReloading = false;

	UPROPERTY(BlueprintReadWrite) float CrossHairSpreadIncrement = 0.0f;
};

USTRUCT(BlueprintType)
struct FMSG_FireRuleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) bool  bCanFire = true;

	// 覆盖自动射击间隔（<=0 表示不覆盖）
	UPROPERTY(BlueprintReadWrite) float FireIntervalOverride = -1.f;

	// 覆盖扩散增量（<0 表示不覆盖）
	UPROPERTY(BlueprintReadWrite) float SpreadIncrementOverride = -1.f;
};