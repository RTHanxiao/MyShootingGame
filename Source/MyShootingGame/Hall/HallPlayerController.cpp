//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "MyShootingGame/Hall/HallPlayerController.h"
//#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
//#include "../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h"
//
//
//
//
//void AHallPlayerController::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	RotateView(ScreenRotationDirection());
//}
//
//
//void AHallPlayerController::BeginPlay()
//{
//	Super::BeginPlay();
//
//	bShowMouseCursor = true;
//
//	FInputModeUIOnly InputMode;
//	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//	SetInputMode(InputMode);
//
//	//// 在世界里找 BP_LevelStream 的实例
//	//for (TActorIterator<ABP_LevelStream> It(GetWorld()); It; ++It)
//	//{
//	//	LevelStreamRef = *It;
//	//	break; // 找到第一个就够了
//	//}
//
//	//if (!LevelStreamRef)
//	//{
//	//	UE_LOG(LogTemp, Error, TEXT("HallPlayerController: 找不到 BP_LevelStream 实例！"));
//	//}
//}
//
//EScreenRotationDirection AHallPlayerController::ScreenRotationDirection()
//{
//	int32 SizeX = 0, SizeY = 0;
//	GetViewportSize(SizeX, SizeY);
//
//	float MouseX = -1.f, MouseY = -1.f;
//	bool bGotMouse = GetMousePosition(MouseX, MouseY);
//
//	// 安全检查：鼠标无法获取 或 Viewport 未初始化
//	if (!bGotMouse || SizeX <= 0 || SizeY <= 0)
//		return ESRD_None;
//
//	 
//	const FVector2D Center(SizeX * 0.5f, SizeY * 0.5f);
//	const FVector2D Dir(MouseX - Center.X, MouseY - Center.Y);
//	const FVector2D Norm = FVector2D(Dir.X / Center.X, Dir.Y / Center.Y); // [-1, 1]
//
//	// 阈值范围内，不旋转
//	if (FMath::Abs(Norm.X) < CursorThreshold && FMath::Abs(Norm.Y) < CursorThreshold)
//		return ESRD_None;
//
//	// 转为角度（注意屏幕Y轴向下）
//	float AngleRad = FMath::Atan2(-Norm.Y, Norm.X);
//	float AngleDeg = FMath::RadiansToDegrees(AngleRad);
//
//	// 八方向判断
//	if (AngleDeg >= -22.5f && AngleDeg < 22.5f)    return ESRD_Right;
//	if (AngleDeg >= 22.5f && AngleDeg < 67.5f)    return ESRD_RightUp;
//	if (AngleDeg >= 67.5f && AngleDeg < 112.5f)   return ESRD_Up;
//	if (AngleDeg >= 112.5f && AngleDeg < 157.5f)   return ESRD_LeftUp;
//	if (AngleDeg >= -157.5f && AngleDeg < -112.5f) return ESRD_LeftDown;
//	if (AngleDeg >= -112.5f && AngleDeg < -67.5f)  return ESRD_Down;
//	if (AngleDeg >= -67.5f && AngleDeg < -22.5f)   return ESRD_RightDown;
//
//	return ESRD_Left;
//}
//
//
//bool AHallPlayerController::RotateView(EScreenRotationDirection Dir, const float& Speed)
//{
//	if (!GetPawn())
//		return false;
//
//	FRotator Offset = FRotator::ZeroRotator;
//
//	switch (Dir)
//	{
//	case ESRD_Left:       Offset.Yaw = -Speed; break;
//	case ESRD_Right:      Offset.Yaw = Speed; break;
//	case ESRD_Up:         Offset.Pitch = Speed; break;
//	case ESRD_Down:       Offset.Pitch = -Speed; break;
//	case ESRD_RightUp:    Offset = FRotator(Speed, Speed, 0); break;
//	case ESRD_RightDown:  Offset = FRotator(-Speed, Speed, 0); break;
//	case ESRD_LeftUp:     Offset = FRotator(Speed, -Speed, 0); break;
//	case ESRD_LeftDown:   Offset = FRotator(-Speed, -Speed, 0); break;
//	default: break;
//	}
//
//	if (Offset.IsZero())
//		return false;
//
//	FRotator Rot = GetPawn()->GetActorRotation();
//	Rot.Yaw = FMath::Clamp(Rot.Yaw + Offset.Yaw, -120.f, 0.f);
//	Rot.Pitch = FMath::Clamp(Rot.Pitch + Offset.Pitch, -45.f, 45.f);
//
//	GetPawn()->SetActorRotation(Rot);
//	return true;
//}
//
