#include "BTTask_ClearInvestigate.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyShootingGame/AI/ZombieAIController.h"

EBTNodeResult::Type UBTTask_ClearInvestigate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	BB->ClearValue(AZombieAIController::BB_TargetPlayer);
	BB->ClearValue(AZombieAIController::BB_LastKnownLocation);
	BB->SetValueAsBool(AZombieAIController::BB_IsInSight, false);
	BB->SetValueAsEnum(AZombieAIController::BB_SenceState, 2); // 2=GiveUp（你可以约定）

	AIController->StopMovement();
	return EBTNodeResult::Succeeded;
}