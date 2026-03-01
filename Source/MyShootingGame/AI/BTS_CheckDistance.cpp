// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/AI/BTS_CheckDistance.h"
#include "../../../../../../../Source/Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"

#include "MyShootingGame/Character/ZombieCharacter.h"
#include "MyShootingGame/Character/PlayerCharacter.h"
#include "MyShootingGame/AI/ZombieAIController.h"



void UBTS_CheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return;

	AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(AIController->GetPawn());
	if (!ZombieCharacter) return;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject("Target_Player"));
	if (!Target)
	{
		BB->SetValueAsBool("InAttackRange", false);
		return;
	}

	const float Dist = FVector::Dist(ZombieCharacter->GetActorLocation(), Target->GetActorLocation());
	BB->SetValueAsFloat("TargetDistance", Dist);

	const bool bInAttackRange = (Dist <= 120.f);
	BB->SetValueAsBool("InAttackRange", bInAttackRange);

	 UE_LOG(LogTemp, Warning, TEXT("[CheckDistance] Dist=%.1f InAttackRange=%d"), Dist, bInAttackRange ? 1 : 0);
}