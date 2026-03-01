#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SearchAround.generated.h"

UCLASS()
class MYSHOOTINGGAME_API UBTTask_SearchAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SearchAround();

protected:
	UPROPERTY(EditAnywhere, Category = "Search")
	float SearchDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Search")
	float WanderRadius = 900.f;

	UPROPERTY(EditAnywhere, Category = "Search")
	float MinWanderDist = 350.f;

	// 到达一个点后，随机停顿时间（换方向前停一下）
	UPROPERTY(EditAnywhere, Category = "Search|Pause")
	float PauseTimeMin = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Search|Pause")
	float PauseTimeMax = 0.9f;

	// 抽点失败时的重试间隔（避免每帧猛抽）
	UPROPERTY(EditAnywhere, Category = "Search")
	float RetryInterval = 0.3f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	enum class ESearchState : uint8
	{
		Moving,
		Pausing
	};

	struct FSearchMem
	{
		float Elapsed = 0.f;
		ESearchState State = ESearchState::Pausing;

		float PauseLeft = 0.f;
		float RetryLeft = 0.f;

		FVector CurrentMoveGoal = FVector::ZeroVector;
		bool bHasMoveGoal = false;
	};

	bool TryPickWanderPoint(APawn* Pawn, FVector& OutPoint) const;
	void StartMove(AAIController* AIController, const FVector& Goal, FSearchMem& Mem) const;
	void StartPause(AAIController* AIController, FSearchMem& Mem) const;
};