// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/Character/MyCharacter.h"
#include "../../../../../../../Source/Programs/VirtualProduction/TextureShare/Public/Containers/UnrealEngine/TextureShareSDKUnrealEngineArray.h"
#include "ZombieCharacter.generated.h"


class APlayerCharacter;
class AZombieAIController;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API AZombieCharacter : public AMyCharacter
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	AZombieCharacter();
	
	AZombieAIController* AIC_Zombie;
	
	bool bIsAttacking = false;

	

public:
	APlayerCharacter* TargetPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TArray<UAnimMontage*> AttackMontages;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Attack();

	bool IsAttacking() const { return bIsAttacking; }
	UFUNCTION(BlueprintCallable)
	void SetIsAttacking(bool bAttacking) { bIsAttacking = bAttacking; }

	UFUNCTION(BlueprintCallable)
	void SetTargetPlayer(AActor* Found);
};
