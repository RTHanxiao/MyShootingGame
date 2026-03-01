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

	if (!AIC_Zombie)
	{
		AIC_Zombie = Cast<AZombieAIController>(GetController());
	}

	bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		// 面向移动方向
		Move->bOrientRotationToMovement = true;

		// 转向速度（越大越快越“跟手”，越小越慢越平滑）
		Move->RotationRate = FRotator(0.f, 360.f, 0.f);

		// 防止停下时抖动
		Move->bUseControllerDesiredRotation = false;
	}
}


AZombieCharacter::AZombieCharacter()
{
	TargetPlayer = nullptr;
	AIC_Zombie = nullptr;

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}



float AZombieCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,AController* EventInstigator, AActor* DamageCauser)
{
	const float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (AIC_Zombie && AIC_Zombie->GetBlackboardComponent())
	{
		AIC_Zombie->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), bDead);
	}

	return Actual;
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

	if (UBlackboardComponent* BB = AIC_Zombie->GetBlackboardComponent())
	{
		BB->SetValueAsObject(TEXT("Target_Player"), TargetPlayer);
	}
}


void AZombieCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AIC_Zombie = Cast<AZombieAIController>(NewController);
}

void AZombieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 攻击 Montage 结束或被打断：回收攻击状态
	SetIsAttacking(false);

	// 通知 BTTask：攻击结束
	OnAttackEnded.Broadcast();
}