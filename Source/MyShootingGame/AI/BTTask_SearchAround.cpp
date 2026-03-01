#include "MyShootingGame/AI/BTTask_SearchAround.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_SearchAround::UBTTask_SearchAround()
{
	bNotifyTick = true;
}

uint16 UBTTask_SearchAround::GetInstanceMemorySize() const
{
	return sizeof(FSearchMem);
}

EBTNodeResult::Type UBTTask_SearchAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FSearchMem* Mem = (FSearchMem*)NodeMemory;
	Mem->Elapsed = 0.f;
	Mem->State = ESearchState::Pausing;
	Mem->PauseLeft = FMath::FRandRange(PauseTimeMin, PauseTimeMax);
	Mem->RetryLeft = 0.f;
	Mem->bHasMoveGoal = false;
	Mem->CurrentMoveGoal = FVector::ZeroVector;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_SearchAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FSearchMem* Mem = (FSearchMem*)NodeMemory;
	Mem->Elapsed += DeltaSeconds;

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 搜索时间到：结束
	if (Mem->Elapsed >= SearchDuration)
	{
		if (AIController)
		{
			AIController->StopMovement();
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 失败重试节流
	if (Mem->RetryLeft > 0.f)
	{
		Mem->RetryLeft -= DeltaSeconds;
	}

	switch (Mem->State)
	{
	case ESearchState::Moving:
	{
		// 到达/停止移动则进入停顿
		const EPathFollowingStatus::Type Status = AIController->GetMoveStatus();
		if (Status != EPathFollowingStatus::Moving)
		{
			StartPause(AIController, *Mem);
		}
		break;
	}
	case ESearchState::Pausing:
	{
		Mem->PauseLeft -= DeltaSeconds;
		if (Mem->PauseLeft > 0.f)
		{
			break;
		}

		// 停顿结束，开始选择下一个点移动
		if (Mem->RetryLeft > 0.f)
		{
			break;
		}

		FVector NewGoal;
		if (TryPickWanderPoint(Pawn, NewGoal))
		{
			StartMove(AIController, NewGoal, *Mem);
		}
		else
		{
			// 抽点失败就稍后再试
			Mem->RetryLeft = RetryInterval;
			Mem->PauseLeft = FMath::FRandRange(PauseTimeMin, PauseTimeMax) * 0.5f;
		}
		break;
	}
	default:
		break;
	}
}

void UBTTask_SearchAround::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

bool UBTTask_SearchAround::TryPickWanderPoint(APawn* Pawn, FVector& OutPoint) const
{
	UWorld* World = Pawn->GetWorld();
	if (!World) return false;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return false;

	const FVector Origin = Pawn->GetActorLocation();

	for (int32 TryIdx = 0; TryIdx < 12; ++TryIdx)
	{
		FNavLocation NavLoc;
		if (!NavSys->GetRandomReachablePointInRadius(Origin, WanderRadius, NavLoc))
		{
			continue;
		}

		const float Dist = FVector::Dist(Origin, NavLoc.Location);
		if (Dist < MinWanderDist)
		{
			continue;
		}

		OutPoint = NavLoc.Location;
		return true;
	}

	return false;
}

void UBTTask_SearchAround::StartMove(AAIController* AIController, const FVector& Goal, FSearchMem& Mem) const
{
	if (!AIController) return;

	// 新旧目标太近就不换（避免方向抖）
	if (Mem.bHasMoveGoal && FVector::DistSquared(Mem.CurrentMoveGoal, Goal) < (MinWanderDist * 0.5f) * (MinWanderDist * 0.5f))
	{
		Mem.State = ESearchState::Pausing;
		Mem.PauseLeft = FMath::FRandRange(PauseTimeMin, PauseTimeMax);
		return;
	}

	FAIMoveRequest Req;
	Req.SetGoalLocation(Goal);
	Req.SetAcceptanceRadius(80.f);
	Req.SetUsePathfinding(true);
	Req.SetAllowPartialPath(true);

	AIController->MoveTo(Req);

	Mem.CurrentMoveGoal = Goal;
	Mem.bHasMoveGoal = true;
	Mem.State = ESearchState::Moving;
}

void UBTTask_SearchAround::StartPause(AAIController* AIController, FSearchMem& Mem) const
{
	if (AIController)
	{
		AIController->StopMovement();
	}

	Mem.State = ESearchState::Pausing;
	Mem.PauseLeft = FMath::FRandRange(PauseTimeMin, PauseTimeMax);
}