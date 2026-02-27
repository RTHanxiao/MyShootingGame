// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/AI/BTS_CheckDistance.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"

#include "MyShootingGame/Character/ZombieCharacter.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "MyShootingGame/AI/ZombieAIController.h"



void UBTS_CheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner()))
	{
		if (AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn()))
		{
			if (ZombieCharacter->TargetPlayer)
			{
				float PlayerDistance = FVector::Dist(ZombieCharacter->GetActorLocation(), ZombieCharacter->TargetPlayer->GetActorLocation());
				if (PlayerDistance < 120.f)
				{
					AIController->GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);
				}
				else
				{
					AIController->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
				}
			}
		}
	}
}
