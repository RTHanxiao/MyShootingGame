// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/AI/BTNode_Attack.h"
#include "ZombieAIController.h"
#include "MyShootingGame/Character/ZombieCharacter.h"




EBTNodeResult::Type UBTNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn());
	if (!ZombieCharacter) return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;

	if (!ZombieCharacter->OnAttackEnded.IsAlreadyBound(this, &UBTNode_Attack::OnAttackFinished))
	{
		ZombieCharacter->OnAttackEnded.AddDynamic(this, &UBTNode_Attack::OnAttackFinished);
	}

	// 关键：只在“不在攻击中”时触发一次
	if (!ZombieCharacter->IsAttacking())
	{
		AIController->StopMovement(); // 建议：进攻时停一下移动，避免边跑边播
		ZombieCharacter->Attack();
	}

	return EBTNodeResult::InProgress;
}

void UBTNode_Attack::OnAttackFinished()
{
	if (!CachedOwnerComp) return;

	AZombieAIController* AIController = Cast<AZombieAIController>(CachedOwnerComp->GetAIOwner());
	if (AIController)
	{
		if (AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn()))
		{
			ZombieCharacter->OnAttackEnded.RemoveDynamic(this, &UBTNode_Attack::OnAttackFinished);
		}
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp = nullptr;
}

void UBTNode_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (AIController)
	{
		if (AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn()))
		{
			ZombieCharacter->OnAttackEnded.RemoveDynamic(this, &UBTNode_Attack::OnAttackFinished);
		}
	}

	CachedOwnerComp = nullptr;
}