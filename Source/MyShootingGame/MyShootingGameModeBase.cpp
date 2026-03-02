// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShootingGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "GameCore/MSG_PlayerController.h"
#include "GameCore/MSG_FightHUD.h"

#include "Hall/HallHUD.h"
#include "Hall/HallPlayerController.h"
#include "Hall/HallPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/PackageName.h"

AMyShootingGameModeBase::AMyShootingGameModeBase()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	// 默认都用战斗 PC（或者你喜欢的那个）
	PlayerControllerClass = AMSG_PlayerController::StaticClass();

	// Pawn 类走 TSubclassOf，在 BP_GameMode 里配置：
	// HallPawnClass = BP_HallPawn
	// FightPawnClass = BP_Player
	bStartPlayersAsSpectators = true;
	DefaultPawnClass = nullptr;

	HallHUDClass = AHallHUD::StaticClass();
	FightHUDClass = AMSG_FightHUD::StaticClass();
	HUDClass = HallHUDClass;
}

void AMyShootingGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	FString CurrentMap = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	// 记录当前模式（以后新玩家进来就用这个，不靠 MapName 推断）
	if (CurrentMap == "TestMap")
	{
		CurrentWorldMode = EMSGWorldMode::Fight;
		SetFightMode();
	}
	else
	{
		CurrentWorldMode = EMSGWorldMode::Hall;
		SetHallMode();
	}
}

void AMyShootingGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!HasAuthority() || !NewPlayer) return;

	// 关键：延迟一下，避免新加入客户端 PC/HUD 未 ready 导致 Client RPC 不生效
	ApplyModeForPlayer_Delayed(NewPlayer);
}

void AMyShootingGameModeBase::ApplyModeForPlayer_Delayed(APlayerController* PC)
{
	if (!HasAuthority() || !PC) return;

	FTimerHandle TH;
	GetWorldTimerManager().SetTimer(
		TH,
		FTimerDelegate::CreateUObject(this, &AMyShootingGameModeBase::ApplyModeForPlayer, PC),
		0.2f,
		false
	);
}

void AMyShootingGameModeBase::SetHallMode()
{
	if (!HasAuthority() || !HallPawnClass) return;

	// 找大厅出生点（PlayerStartTag == HallStart）
	AActor* HallStartActor = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->PlayerStartTag == TEXT("HallStart"))
		{
			HallStartActor = *It;
			break;
		}
	}
	const FTransform SpawnTM = HallStartActor ? HallStartActor->GetActorTransform() : FTransform::Identity;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		// 干掉旧 Pawn（例如 FightPawn）
		if (APawn* OldPawn = PC->GetPawn())
		{
			OldPawn->Destroy();
		}

		FActorSpawnParameters Params;
		Params.Owner = PC;
		Params.Instigator = PC->GetPawnOrSpectator();
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
			HallPawnClass,
			SpawnTM.GetLocation(),
			SpawnTM.GetRotation().Rotator(),
			Params
		);
		if (!NewPawn) continue;

		PC->Possess(NewPawn);

		if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
		{
			MSGPC->Client_ApplyHallMode(NewPawn, HallHUDClass);
		}
	}
}

void AMyShootingGameModeBase::SetFightMode()
{
	if (!HasAuthority() || !FightPawnClass) return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		// 干掉旧 Pawn（例如 HallPawn）
		if (APawn* OldPawn = PC->GetPawn())
		{
			OldPawn->Destroy();
		}

		AActor* StartSpot = FindPlayerStart(PC);
		const FTransform SpawnTM = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

		FActorSpawnParameters Params;
		Params.Owner = PC;
		Params.Instigator = PC->GetPawnOrSpectator();
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
			FightPawnClass,
			SpawnTM.GetLocation(),
			SpawnTM.GetRotation().Rotator(),
			Params
		);
		if (!NewPawn) continue;

		PC->Possess(NewPawn);

		if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
		{
			MSGPC->Client_ApplyFightMode(NewPawn, FightHUDClass);
		}
	}
}

/** 蓝图调用：根据地图选择模式 */
void AMyShootingGameModeBase::SwitchWorldMode(FName LoadedLevelName)
{
	if (!HasAuthority())
	{
		return;
	}

	if (LoadedLevelName == "Demonstration")
	{
		CurrentWorldMode = EMSGWorldMode::Hall;
		SetHallMode();
	}
	else if (LoadedLevelName == "Highway")
	{
		CurrentWorldMode = EMSGWorldMode::Fight;
		SetFightMode();
	}
	else
	{
		CurrentWorldMode = EMSGWorldMode::Hall;
		SetHallMode();
	}
}


void AMyShootingGameModeBase::ApplyModeForPlayer(APlayerController* PC)
{
	if (!HasAuthority() || !PC) return;

	if (CurrentWorldMode == EMSGWorldMode::Fight)
	{
		SetFightModeForPlayer(PC);
	}
	else
	{
		SetHallModeForPlayer(PC);
	}
}

void AMyShootingGameModeBase::SetHallModeForPlayer(APlayerController* PC)
{
	if (!HasAuthority() || !PC || !HallPawnClass) return;

	APawn* Pawn = PC->GetPawn();

	// ✅ PostLogin 场景：如果已经有 Pawn，不要 Destroy/重生（避免误伤 server0）
	if (!Pawn)
	{
		// 找大厅出生点（PlayerStartTag == HallStart）
		AActor* HallStartActor = nullptr;
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			if (It->PlayerStartTag == TEXT("HallStart"))
			{
				HallStartActor = *It;
				break;
			}
		}
		const FTransform SpawnTM = HallStartActor ? HallStartActor->GetActorTransform() : FTransform::Identity;

		FActorSpawnParameters Params;
		Params.Owner = PC;
		Params.Instigator = PC->GetPawnOrSpectator();
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		Pawn = GetWorld()->SpawnActor<APawn>(
			HallPawnClass,
			SpawnTM.GetLocation(),
			SpawnTM.GetRotation().Rotator(),
			Params
		);

		if (!Pawn) return;
		PC->Possess(Pawn);
	}

	// ✅ 无论是否重生，都只做客户端表现
	if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
	{
		MSGPC->Client_ApplyHallMode(Pawn, HallHUDClass);
	}
}

void AMyShootingGameModeBase::SetFightModeForPlayer(APlayerController* PC)
{
	if (!HasAuthority() || !PC || !FightPawnClass) return;

	APawn* Pawn = PC->GetPawn();

	// ✅ PostLogin 场景：如果已经有 Pawn，不要 Destroy/重生（避免误伤 server0）
	if (!Pawn)
	{
		AActor* StartSpot = FindPlayerStart(PC);
		const FTransform SpawnTM = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

		FActorSpawnParameters Params;
		Params.Owner = PC;
		Params.Instigator = PC->GetPawnOrSpectator();
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		Pawn = GetWorld()->SpawnActor<APawn>(
			FightPawnClass,
			SpawnTM.GetLocation(),
			SpawnTM.GetRotation().Rotator(),
			Params
		);

		if (!Pawn) return;
		PC->Possess(Pawn);
	}

	if (AMSG_PlayerController* MSGPC = Cast<AMSG_PlayerController>(PC))
	{
		MSGPC->Client_ApplyFightMode(Pawn, FightHUDClass);
	}
}