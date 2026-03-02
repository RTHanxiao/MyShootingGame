#include "MyShootingGame/AI/BTS_CheckTargetAlive.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "MyShootingGame/AI/ZombieAIController.h"

// 你的玩家/角色基类，按实际 include
#include "MyShootingGame/Character/MyCharacter.h"        // 如果 bDead 在这里
// #include "MyShootingGame/Character/PlayerCharacter.h" // 如果 bDead 在 PlayerCharacter

void UBTS_CheckTargetAlive::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AZombieAIController::BB_TargetPlayer));
	if (!Target) return;

	// 目标对象无效 / 被销毁
	if (!IsValid(Target) || Target->IsActorBeingDestroyed())
	{
		BB->ClearValue(AZombieAIController::BB_TargetPlayer);
		BB->SetValueAsBool(AZombieAIController::BB_IsInSight, false);
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	// 目标已死亡
	if (AMyCharacter* Ch = Cast<AMyCharacter>(Target))
	{
		if (Ch->bDead)
		{
			BB->ClearValue(AZombieAIController::BB_TargetPlayer);
			BB->SetValueAsBool(AZombieAIController::BB_IsInSight, false);

			// 可选：也清调查点，避免死了还去“最后位置”
			BB->ClearValue(AZombieAIController::BB_LastKnownLocation);

			AIController->StopMovement();
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
}