// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShootingGame/GameCore/MSG_PlayerController.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameViewportClient.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Service/LuaGameService.h"
#include "Service/LuaWorldService.h"
#include "Service/HotReload.h"

AMSG_PlayerController::AMSG_PlayerController()
{
	bReplicates = true;

	// 默认从大厅模式开始（你也可以在 GameMode 里切）
	bEnableHallMode = true;
	CursorThreshold = 0.2f;
	bShowMouseCursor = true;
}

void AMSG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			if (FightMappingContext)
			{
				Subsys->AddMappingContext(FightMappingContext, 0);
			}
		}
	}

	// 按当前模式设置输入方式
	SetHallControlMode(bEnableHallMode);

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();

	UE_LOG(LogTemp, Warning, TEXT("[Inventory] PC BeginPlay: InvComponent = %s"),
		InventoryComponent.Get() != nullptr ? TEXT("Valid") : TEXT("Invalid"));
}

void AMSG_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 只有在大厅模式才根据鼠标位置旋转视角
	if (bEnableHallMode)
	{
		RotateView(ScreenRotationDirection());
	}
}

void AMSG_PlayerController::SetHallControlMode(bool bEnable)
{
	bEnableHallMode = bEnable;

	if (bEnableHallMode)
	{
		// —— 大厅：UIOnly + 显示鼠标 ——
		bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		// —— 战斗：GameOnly + 隐藏鼠标 ——
		bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}

	// 确保没有被别的地方关掉输入
	SetIgnoreLookInput(false);
	SetIgnoreMoveInput(false);

	if (GetWorld() && GetWorld()->GetGameViewport())
	{
		GetWorld()->GetGameViewport()->SetIgnoreInput(false);
	}
}

void AMSG_PlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] PC ToggleInventory: InventoryComponent is invalid!"));
		return;
	}
	InventoryComponent->ToggleInventoryMenu();
}

// =========================
// Client：大厅模式切换执行
// =========================

void AMSG_PlayerController::Client_ApplyHallMode_Implementation(
	TSubclassOf<APawn> InPawnClass,
	TSubclassOf<AHUD> InHUDClass)
{
	// 切 HUD
	if (InHUDClass)
	{
		ClientSetHUD(InHUDClass);
	}

	// 干掉旧 Pawn
	if (APawn* CurrentPawn = GetPawn())
	{
		CurrentPawn->Destroy();
	}

	// 找大厅出生点（等价于 FindPlayerStart 的 tag 行为）
	AActor* HallStartActor = nullptr;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->PlayerStartTag == TEXT("HallStart"))
		{
			HallStartActor = *It;
			break;
		}
	}

	const FTransform SpawnTM = HallStartActor
		? HallStartActor->GetActorTransform()
		: FTransform::Identity;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 生成大厅 Pawn（用 BP_HallPawn）
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
		InPawnClass,
		SpawnTM.GetLocation(),
		SpawnTM.GetRotation().Rotator(),
		Params
	);

	if (NewPawn)
	{
		Possess(NewPawn);

		// ⭐ 必须放这里：视角绑定完成后切换输入模式
		SetHallControlMode(true);   // 大厅模式：UIOnly + 鼠标边缘旋转

		SetViewTargetWithBlend(NewPawn, 0.5f);
	}
}

// =========================
// Client：战斗模式切换执行
// =========================

void AMSG_PlayerController::Client_ApplyFightMode_Implementation(
	TSubclassOf<APawn> InPawnClass,
	TSubclassOf<AHUD> InHUDClass)
{
	// 切 HUD
	if (InHUDClass)
	{
		ClientSetHUD(InHUDClass);
	}

	// 这里只用“场景中已有的 BP_Player”
	APawn* ExistPawn = nullptr;

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* IterPawn = *It;
		if (IterPawn && InPawnClass && IterPawn->IsA(InPawnClass))
		{
			ExistPawn = IterPawn;
			break;
		}
	}

	if (!ExistPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_ApplyFightMode: No BP_Player found in level, spawning one."));

		// 如果没找到，就直接在原地 Spawn 一个，避免彻底黑屏
		FTransform SpawnTM = FTransform::Identity;
		if (APawn* Old = GetPawn())
		{
			SpawnTM.SetLocation(Old->GetActorLocation());
			SpawnTM.SetRotation(Old->GetActorQuat());
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ExistPawn = GetWorld()->SpawnActor<APawn>(
			InPawnClass,
			SpawnTM.GetLocation(),
			SpawnTM.GetRotation().Rotator(),
			Params
		);
	}

	if (ExistPawn)
	{
		// 先把旧 Pawn（比如 HallPawn）干掉
		if (APawn* CurrentPawn = GetPawn())
		{
			if (CurrentPawn != ExistPawn)
			{
				CurrentPawn->Destroy();
			}
		}

		Possess(ExistPawn);

		// ⭐ 放这里：Possess完后切换到战斗控制模式
		SetHallControlMode(false);      // 战斗模式：GameOnly

		SetViewTargetWithBlend(ExistPawn, 0.5f);
	}
}

void AMSG_PlayerController::HotReloadLua()
{
	const int32 EnvCount = ULuaHotReloadLibrary::HotReloadAllLuaEnvs();

	// 通知 GameService（跨地图）
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULuaGameService* GS = GI->GetSubsystem<ULuaGameService>())
		{
			GS->LuaOnHotReloaded(EnvCount);
		}
	}

	// 通知 WorldService（当前世界）
	if (UWorld* World = GetWorld())
	{
		if (ULuaWorldService* WS = World->GetSubsystem<ULuaWorldService>())
		{
			WS->LuaOnHotReloaded(EnvCount);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[HotReloadLua] done, envCount=%d"), EnvCount);
}

// =========================
// 你的原有鼠标边缘旋转逻辑
// =========================

EScreenRotationDirection AMSG_PlayerController::ScreenRotationDirection()
{
	int32 SizeX = 0, SizeY = 0;
	GetViewportSize(SizeX, SizeY);

	float MouseX = -1.f, MouseY = -1.f;
	const bool bGotMouse = GetMousePosition(MouseX, MouseY);

	// 安全检查：鼠标无法获取 或 Viewport 未初始化
	if (!bGotMouse || SizeX <= 0 || SizeY <= 0)
	{
		return EScreenRotationDirection::ESRD_None;
	}

	const FVector2D Center(SizeX * 0.5f, SizeY * 0.5f);
	const FVector2D Dir(MouseX - Center.X, MouseY - Center.Y);
	const FVector2D Norm(Dir.X / Center.X, Dir.Y / Center.Y); // [-1, 1]

	// 阈值范围内，不旋转
	if (FMath::Abs(Norm.X) < CursorThreshold && FMath::Abs(Norm.Y) < CursorThreshold)
	{
		return EScreenRotationDirection::ESRD_None;
	}

	// 转为角度（注意屏幕Y轴向下）
	const float AngleRad = FMath::Atan2(-Norm.Y, Norm.X);
	const float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	// 八方向判断
	if (AngleDeg >= -22.5f && AngleDeg < 22.5f)       return EScreenRotationDirection::ESRD_Right;
	if (AngleDeg >= 22.5f && AngleDeg < 67.5f)        return EScreenRotationDirection::ESRD_RightUp;
	if (AngleDeg >= 67.5f && AngleDeg < 112.5f)       return EScreenRotationDirection::ESRD_Up;
	if (AngleDeg >= 112.5f && AngleDeg < 157.5f)      return EScreenRotationDirection::ESRD_LeftUp;
	if (AngleDeg >= -157.5f && AngleDeg < -112.5f)    return EScreenRotationDirection::ESRD_LeftDown;
	if (AngleDeg >= -112.5f && AngleDeg < -67.5f)     return EScreenRotationDirection::ESRD_Down;
	if (AngleDeg >= -67.5f && AngleDeg < -22.5f)      return EScreenRotationDirection::ESRD_RightDown;

	return EScreenRotationDirection::ESRD_Left;
}

bool AMSG_PlayerController::RotateView(EScreenRotationDirection Dir, const float& Speed)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return false;
	}

	FRotator Offset = FRotator::ZeroRotator;

	switch (Dir)
	{
	case EScreenRotationDirection::ESRD_Left:       Offset.Yaw = -Speed; break;
	case EScreenRotationDirection::ESRD_Right:      Offset.Yaw = Speed; break;
	case EScreenRotationDirection::ESRD_Up:         Offset.Pitch = Speed; break;
	case EScreenRotationDirection::ESRD_Down:       Offset.Pitch = -Speed; break;
	case EScreenRotationDirection::ESRD_RightUp:    Offset = FRotator(Speed, Speed, 0); break;
	case EScreenRotationDirection::ESRD_RightDown:  Offset = FRotator(-Speed, Speed, 0); break;
	case EScreenRotationDirection::ESRD_LeftUp:     Offset = FRotator(Speed, -Speed, 0); break;
	case EScreenRotationDirection::ESRD_LeftDown:   Offset = FRotator(-Speed, -Speed, 0); break;
	default: break;
	}

	if (Offset.IsZero())
	{
		return false;
	}

	FRotator Rot = MyPawn->GetActorRotation();
	Rot.Yaw = FMath::Clamp(Rot.Yaw + Offset.Yaw, -120.f, 0.f);
	Rot.Pitch = FMath::Clamp(Rot.Pitch + Offset.Pitch, -45.f, 45.f);

	MyPawn->SetActorRotation(Rot);
	return true;
}


