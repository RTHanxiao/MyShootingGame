// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShootingGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "GameCore/MSG_PlayerController.h"
#include "GameCore/MSG_FightHUD.h"

#include "Hall/HallHUD.h"
#include "Hall/HallPlayerController.h"
#include "Hall/HallPawn.h"

AMyShootingGameModeBase::AMyShootingGameModeBase()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	// 默认都用战斗 PC（或者你喜欢的那个）
	PlayerControllerClass = AMSG_PlayerController::StaticClass();

	// Pawn 类走 TSubclassOf，在 BP_GameMode 里配置：
	// HallPawnClass = BP_HallPawn
	// FightPawnClass = BP_Player
	DefaultPawnClass = HallPawnClass;

	HallHUDClass = AHallHUD::StaticClass();
	FightHUDClass = AMSG_FightHUD::StaticClass();
	HUDClass = HallHUDClass;
}

void AMyShootingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// GameMode 只在 Server 有意义；Client 下不做模式决策
	if (!HasAuthority())
	{
		return;
	}

	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	// 默认为 Hall；若是测试图(TestMap)则直接转换成 FightMode
	if (CurrentMap == "TestMap")
	{
		SetFightMode();
	}
	else
	{
		SetHallMode();
	}
}

void AMyShootingGameModeBase::SetHallMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	// 由 Server 决策，并通过 Client RPC 让客户端执行（HUD/Input/视角）逻辑
	if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
	{
		MSGPC->Client_ApplyHallMode(HallPawnClass, HallHUDClass);
	}
}

void AMyShootingGameModeBase::SetFightMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
	{
		MSGPC->Client_ApplyFightMode(FightPawnClass, FightHUDClass);
	}
}

/** 蓝图调用：根据地图选择模式 */
void AMyShootingGameModeBase::SwitchWorldMode(FName LoadedLevelName)
{
	// 保持你原逻辑（注意：依然由 Server 调用更合理）
	if (!HasAuthority())
	{
		return;
	}

	if (LoadedLevelName == "Demonstration")
	{
		SetHallMode();
	}
	else if (LoadedLevelName == "Highway")
	{
		SetFightMode();
	}
	else
	{
		SetHallMode();
	}
}
