// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/AI/ZombieAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/Character.h"

const FName AZombieAIController::BB_TargetPlayer(TEXT("Target_Player"));
const FName AZombieAIController::BB_IsAttacking(TEXT("IsAttacking"));
const FName AZombieAIController::BB_IsDead(TEXT("IsDead"));
const FName AZombieAIController::BB_LastKnownLocation(TEXT("LastKnowLocation"));
const FName AZombieAIController::BB_IsInSight(TEXT("IsInSight"));
const FName AZombieAIController::BB_TargetDistance(TEXT("TargetDistance"));
const FName AZombieAIController::BB_SenceState(TEXT("SenceState"));

AZombieAIController::AZombieAIController()
{
	BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComp"));
	BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2400.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(2.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnPerceptionUpdated);
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBehaviorTree* BTAsset = BehaviorTreeAsset.Get();
	if (!BTAsset || !BTAsset->BlackboardAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("[ZombieAIController] BehaviorTreeAsset or BlackboardAsset is null."));
		return;
	}

	// 初始化黑板（UseBlackboard 会填充 Controller 的 BlackboardComponent）
	UBlackboardComponent* BBD = BBComp.Get();
	if (!IsValid(BBD)) return;

	if (!UseBlackboard(BTAsset->BlackboardAsset, BBD))
	{
		UE_LOG(LogTemp, Error, TEXT("[ZombieAIController] UseBlackboard failed."));
		return;
	}

	RunBehaviorTree(BTAsset);
	if (BTComp)
	{
		BTComp->StartTree(*BTAsset);
	}

	// 可选：初始化默认值，避免脏数据
	BBComp->SetValueAsBool(BB_IsInSight, false);
	BBComp->ClearValue(BB_TargetPlayer);
}

void AZombieAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BBComp || !Actor) return;

	const bool bSensed = Stimulus.WasSuccessfullySensed();
	AActor* CurTarget = Cast<AActor>(BBComp->GetValueAsObject(BB_TargetPlayer));

	if (bSensed)
	{
		// 看到：设当前目标，更新状态与最后位置
		BBComp->SetValueAsObject(BB_TargetPlayer, Actor);
		BBComp->SetValueAsBool(BB_IsInSight, true);

		// Seen 时用 Actor 位置更稳定
		BBComp->SetValueAsVector(BB_LastKnownLocation, Actor->GetActorLocation());
		BBComp->SetValueAsEnum(BB_SenceState, 0); // 0=Seeing
		return;
	}

	// 丢失：只处理“当前目标”的丢失，避免别的 Actor 干扰
	if (CurTarget && CurTarget != Actor)
	{
		return;
	}

	BBComp->SetValueAsBool(BB_IsInSight, false);
	BBComp->SetValueAsEnum(BB_SenceState, 1); // 1=Lost

	// 关键：记录“消失点”
	// StimulusLocation 通常就是目标最后被感知到的位置；不可用时退化为 Actor 位置
	const FVector LostPos = Stimulus.StimulusLocation.IsNearlyZero()
		? Actor->GetActorLocation()
		: Stimulus.StimulusLocation;

	BBComp->SetValueAsVector(BB_LastKnownLocation, LostPos);
}