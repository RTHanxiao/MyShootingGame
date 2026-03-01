// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/Character/MyCharacter.h"
#include "ZombieCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZombieAttackEnded);

class APlayerCharacter;
class AZombieAIController;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API AZombieCharacter : public AMyCharacter
{
	GENERATED_BODY()

	bool bIsAttacking = false;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	

public:
	AZombieCharacter();
	APlayerCharacter* TargetPlayer;

	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FZombieAttackEnded OnAttackEnded;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY()
	TObjectPtr<AZombieAIController> AIC_Zombie = nullptr;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Attack();

	bool IsAttacking() const { return bIsAttacking; }
	UFUNCTION(BlueprintCallable)
	void SetIsAttacking(bool bAttacking) { bIsAttacking = bAttacking; }

	UFUNCTION(BlueprintCallable)
	void SetTargetPlayer(AActor* Found);
};
