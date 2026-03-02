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
#include "../MyShootingGameModeBase.h"

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

	if (IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[InputState] PC=%s MoveIgnore=%d LookIgnore=%d Mouse=%d"),
			*GetNameSafe(this),
			IsMoveInputIgnored() ? 1 : 0,
			IsLookInputIgnored() ? 1 : 0,
			bShowMouseCursor ? 1 : 0);
	}

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
	//SetHallControlMode(bEnableHallMode);

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();

	UE_LOG(LogTemp, Warning, TEXT("[Inventory] PC BeginPlay: InvComponent = %s"),
		InventoryComponent.Get() != nullptr ? TEXT("Valid") : TEXT("Invalid"));

	UE_LOG(LogTemp, Warning, TEXT("[PC] %s Local=%d HasAuth=%d Pawn=%s"),
		*GetNameSafe(this),
		IsLocalController(),
		HasAuthority(),
		*GetNameSafe(GetPawn()));

	ShowStartupUI_Local();
}

void AMSG_PlayerController::Client_ShowStartupUI_Implementation()
{
	ShowStartupUI_Local();
}

void AMSG_PlayerController::ShowStartupUI_Local()
{
	if (!IsLocalController()) return;
	if (!StartupWidgetClass) return;

	if (!StartupWidget)
	{
		StartupWidget = CreateWidget<UUserWidget>(this, StartupWidgetClass);
	}

	if (StartupWidget && !StartupWidget->IsInViewport())
	{
		StartupWidget->AddToViewport(9999);
	}

	// 先 UIOnly，避免玩家在没初始化好时乱动
	bShowMouseCursor = true;
	FInputModeUIOnly M;
	M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(M);
}

void AMSG_PlayerController::Server_TryAddItem_Implementation(UInv_ItemComponent* ItemComp)
{
	if (!ItemComp) return;

	// 服务器侧找 InventoryComponent
	if (!GetInventoryComponent().IsValid())
	{
		return;
	}

	GetInventoryComponent().Get()->TryAddItem(ItemComp);
}

void AMSG_PlayerController::Server_TryPickupActor_Implementation(AActor* ItemActor)
{
	UE_LOG(LogTemp, Warning, TEXT("[Pickup][Server] PCClass=%s"), *GetNameSafe(GetClass()));
	if (!ItemActor) return;
	TArray<UActorComponent*> Comps;
	GetComponents(Comps);
	for (UActorComponent* C : Comps)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Pickup][Server] PCComp=%s Class=%s"),
			*GetNameSafe(C), *GetNameSafe(C->GetClass()));
	}
	APawn* P = GetPawn();
	if (!P) return;

	// 距离校验
	const float DistSq = FVector::DistSquared(P->GetActorLocation(), ItemActor->GetActorLocation());
	if (DistSq > FMath::Square(250.f))
	{
		return;
	}

	// 服务器侧找物品组件（不要依赖 PC 成员变量）
	UInv_ItemComponent* ItemComp = ItemActor->FindComponentByClass<UInv_ItemComponent>();
	UE_LOG(LogTemp, Warning, TEXT("[Pickup][Server] ItemActor=%s ItemComp=%s"),
		*GetNameSafe(ItemActor), *GetNameSafe(ItemComp));
	if (!ItemComp) return;

	UInv_InventoryComponent* Inv = nullptr;

	// 先找基类
	Inv = FindComponentByClass<UInv_InventoryComponent>();

	// 如果找不到，尝试从所有组件里找“名字包含 InventoryComponent 的”（临时兜底排查用）
	if (!Inv)
	{
		GetComponents(Comps);
		for (UActorComponent* C : Comps)
		{
			if (C && C->GetName().Contains(TEXT("Inventory")))
			{
				Inv = Cast<UInv_InventoryComponent>(C);
				if (Inv) break;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Pickup][Server] PCInv=%s"), *GetNameSafe(Inv));
	if (!Inv) return;

	// 服务器权威加入背包
	Inv->TryAddItem(ItemComp);

	// 服务器销毁地上物品
	ItemActor->Destroy();
}

void AMSG_PlayerController::HideStartupUI_Local()
{
	if (!IsLocalController()) return;

	if (StartupWidget)
	{
		StartupWidget->RemoveFromParent();
	}
}

void AMSG_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocalController()) return;

	// 只有在大厅模式才根据鼠标位置旋转视角
	if (bEnableHallMode)
	{
		RotateView(ScreenRotationDirection());
	}
}

void AMSG_PlayerController::SetHallControlMode(bool bEnable)
{
	// 只允许本地控制器改输入模式，否则会出现“一端能动一端不能动/输入串台”
	if (!IsLocalController())
	{
		return;
	}

	bEnableHallMode = bEnable;

	if (bEnableHallMode)
	{
		bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}

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
	APawn* InPawn,
	TSubclassOf<AHUD> InHUDClass)
{
	// 只做“客户端表现层”的事：HUD / 输入模式 / 镜头
	if (InHUDClass)
	{
		ClientSetHUD(InHUDClass);
	}

	ShowStartupUI_Local();

	// Possess 由服务器完成；客户端这边只跟随服务器同步的 Pawn
	APawn* TargetPawn = InPawn;
	if (!IsValid(TargetPawn))
	{
		TargetPawn = Cast<APawn>(GetPawn());
	}

	if (!IsValid(TargetPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_ApplyHallMode: TargetPawn invalid (waiting for replication)."));
		// 不强行 Spawn，避免联机错乱；等服务器把 Pawn 同步过来即可
		return;
	}

	SetHallControlMode(true);   // 大厅模式：UIOnly + 鼠标边缘旋转
	SetViewTargetWithBlend(TargetPawn, 0.5f);
}

// =========================
// Client：战斗模式切换执行
// =========================

void AMSG_PlayerController::Client_ApplyFightMode_Implementation(
	APawn* InPawn,
	TSubclassOf<AHUD> InHUDClass)
{
	// 只做“客户端表现层”的事：HUD / 输入模式 / 镜头
	if (InHUDClass)
	{
		ClientSetHUD(InHUDClass);
	}

	HideStartupUI_Local();

	// Possess 由服务器完成；客户端这边只跟随服务器同步的 Pawn
	APawn* TargetPawn = InPawn;
	if (!IsValid(TargetPawn))
	{
		TargetPawn = Cast<APawn>(GetPawn());
	}

	if (!IsValid(TargetPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Client_ApplyFightMode: TargetPawn invalid (waiting for replication)."));
		return;
	}

	SetHallControlMode(false);      // 战斗模式：GameOnly
	SetViewTargetWithBlend(TargetPawn, 0.5f);
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

void AMSG_PlayerController::Server_StartGameTravel_Implementation(FName MapName)
{
	if (!HasAuthority()) return;

	// 需要全体切世界的地图：ServerTravel
	if (MapName == FName("TestMap"))
	{
		const FString URL = MapName.ToString();
		GetWorld()->ServerTravel(URL, false);
		return;
	}

	// 流加载子关卡：只让服务器触发加载
	// ⚠️ 不要在这里立刻 SwitchWorldMode，因为流加载通常是异步的：
	// 应该在 GameMode 蓝图里“流加载完成 / 显示完成”后再调用 SwitchWorldMode(MapName)
	if (AMyShootingGameModeBase* GM = GetWorld()->GetAuthGameMode<AMyShootingGameModeBase>())
	{
		GM->BP_LoadSubLevel(MapName);
	}
}

// =========================
// 鼠标边缘旋转逻辑
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


