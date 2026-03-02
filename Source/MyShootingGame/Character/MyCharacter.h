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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MeleeDamage = 20.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dead)
	bool bDead = false;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth = 100.f;

	UFUNCTION()
	void OnRep_Dead();

	UFUNCTION()
	void OnRep_CurrentHealth();

	// RepNotify 辅助：用于判断掉血幅度（不需要复制）
	UPROPERTY()
	float LastRepHealth = 100.f;

	// 防止死亡表现重复执行（不需要复制）
	UPROPERTY()
	bool bDeathHandled = false;

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

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetCurrentHealth(float NewHealth);

	void Ragdoll();

	bool IsDead() const { return bDead; }

	virtual void PlayRandomMontageFromArray(const TArray<UAnimMontage*>& MontageArray);

	// Cosmetic hooks (called on clients via RepNotify)
	virtual void OnTookDamage(float DamageAmount, AActor* DamageCauser);
	virtual void OnDied();
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
	void BP_MeleeAttackCollision();

	UFUNCTION(Server, Reliable)
	void Server_MeleeAttackCollision(AController* EventInstigator);

	UFUNCTION(BlueprintCallable)
	virtual void MeleeAttackCollision(AController* EventInstigator);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMyCharacter> AttackableTargetClass;

	int32 LastIndex;
};
