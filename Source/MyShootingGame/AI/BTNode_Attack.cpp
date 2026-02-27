// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/AI/BTNode_Attack.h"
#include "ZombieAIController.h"
#include "MyShootingGame/Character/ZombieCharacter.h"




EBTNodeResult::Type UBTNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner()))
	{
		if(AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn()))
		{
			ZombieCharacter->Attack();
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
