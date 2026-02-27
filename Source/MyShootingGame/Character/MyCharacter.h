 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class MYSHOOTINGGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Actor基础近战伤害
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float MeleeDamage = 20.f;

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;
	
	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 100.f;

	//减伤倍率
	UPROPERTY(BlueprintReadOnly)
	float DamageReductionFactor = 1.f;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const
	{
		return CurrentHealth / MaxHealth;
	}

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintCallable)	
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}

	void Initilize();

	//重写受伤函数
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//设置当前血量
	void SetCurrentHealth(float NewHealth);

	//死亡布娃娃
	void Ragdoll();

	bool IsDead() const { return bDead; }

	virtual void PlayRandomMontageFromArray(const TArray<UAnimMontage*>& MontageArray);

	//近战攻击
	UFUNCTION(BlueprintCallable)
	virtual void MeleeAttackCollision(AController* EventInstigator);

	//可攻击对象
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMyCharacter> AttackableTargetClass;

	//上一个播放的攻击动画
	int32 LastIndex;
};
