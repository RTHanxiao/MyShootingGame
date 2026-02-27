// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Character/ZombieCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerCharacter.h"
#include "MyShootingGame/AI/ZombieAIController.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <Kismet/KismetMathLibrary.h>





void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	AIC_Zombie = Cast<AZombieAIController>(GetController());
}

AZombieCharacter::AZombieCharacter()
{
	TargetPlayer = nullptr;
	AIC_Zombie = nullptr;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}


float AZombieCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AIC_Zombie->GetBlackboardComponent()->SetValueAsBool("IsDead", bDead);

	return DamageAmount;
}

void AZombieCharacter::Attack()
{
	if (!IsAttacking())
	{
		SetIsAttacking(true);
		PlayRandomMontageFromArray(AttackMontages);
	}
}

void AZombieCharacter::SetTargetPlayer(AActor* Found)
{
	TargetPlayer = Cast<APlayerCharacter>(Found);
	if (!AIC_Zombie || !TargetPlayer) return;
	AIC_Zombie->SetFocus(TargetPlayer);
	AIC_Zombie->GetBlackboardComponent()->SetValueAsObject("Target_Player", TargetPlayer);
}
