// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/UI_HallMenu.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include <Components/TextBlock.h>
#include "MyShootingGame/Hall/HallPlayerController.h"
#include "MyShootingGame/Hall/HallHUD.h"
#include "UI_HallMain.h"
#include "MyShootingGame/Hall/HallPawn.h"
#include "MyShootingGame/GameCore/MSG_PlayerController.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"





void UUI_HallMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("UUI_HallMenu::NativeConstruct"));

	//悬停函数绑定
	Btn_Single->OnHovered.AddDynamic(this, &UUI_HallMenu::BtnSingleHover);
	Btn_Single->OnUnhovered.AddDynamic(this, &UUI_HallMenu::BtnSingleUnHover);

	Btn_Multi->OnHovered.AddDynamic(this, &UUI_HallMenu::BtnMultiHover);
	Btn_Multi->OnUnhovered.AddDynamic(this, &UUI_HallMenu::BtnMultiUnHover);

	Btn_Cfg->OnHovered.AddDynamic(this, &UUI_HallMenu::BtnCfgHover);
	Btn_Cfg->OnUnhovered.AddDynamic(this, &UUI_HallMenu::BtnCfgUnHover);

	Btn_Exit->OnHovered.AddDynamic(this, &UUI_HallMenu::BtnExitHover);
	Btn_Exit->OnUnhovered.AddDynamic(this, &UUI_HallMenu::BtnExitUnHover);

	//点击函数绑定
	Btn_Single->OnClicked.AddDynamic(this, &UUI_HallMenu::BtnSingleClicked);
	Btn_Multi->OnClicked.AddDynamic(this, &UUI_HallMenu::BtnMultiClicked);
	Btn_Cfg->OnClicked.AddDynamic(this, &UUI_HallMenu::BtnCfgClicked);
	Btn_Exit->OnClicked.AddDynamic(this, &UUI_HallMenu::BtnExitClicked);
}

/// <summary>
/// 悬停
/// </summary>

void UUI_HallMenu::BtnSingleHover()
{
	Tb_Single->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	PlayHoverAnim(SingleTextHoverAnim, true);
}

void UUI_HallMenu::BtnMultiHover()
{
	Tb_Multi->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	PlayHoverAnim(MultiTextHoverAnim, true);
}

void UUI_HallMenu::BtnCfgHover()
{
	Tb_Cfg->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	PlayHoverAnim(CfgTextHoverAnim, true);
}

void UUI_HallMenu::BtnExitHover()
{
	Tb_Exit->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	PlayHoverAnim(ExitTextHoverAnim, true);
}



/// <summary>
/// 离开悬停
/// </summary>

void UUI_HallMenu::BtnSingleUnHover()
{
	Tb_Single->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PlayHoverAnim(SingleTextHoverAnim, false);
}

void UUI_HallMenu::BtnMultiUnHover()
{
	Tb_Multi->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PlayHoverAnim(MultiTextHoverAnim, false);
}

void UUI_HallMenu::BtnCfgUnHover()
{
	Tb_Cfg->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PlayHoverAnim(CfgTextHoverAnim, false);
}

void UUI_HallMenu::BtnExitUnHover()
{
	Tb_Exit->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PlayHoverAnim(ExitTextHoverAnim, false);
}



void UUI_HallMenu::BtnSingleClicked()
{
	if(AHallPawn* Pawn = Cast<AHallPawn>(GetOwningPlayerPawn()))
	{
		Pawn->SetHallCameraFromMainToOutfit(true);
	}

	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			HUD->GetHallMainUI()->PlayOutfitMenuAnim(true);
			HUD->GetHallMainUI()->PlayMainMenuAnim(false);
		}
	}
}


void UUI_HallMenu::BtnMultiClicked()
{

}


void UUI_HallMenu::BtnCfgClicked()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			HUD->GetHallMainUI()->PlayConfigMenuAnim(true);
			HUD->GetHallMainUI()->PlayMainMenuAnim(false);
		}
	}
}


void UUI_HallMenu::BtnExitClicked()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, true);
	}
}

void UUI_HallMenu::PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover)
{
	if (!InAnim)
	{
		return;
	}

	// bHover = true → 正向播放（放大）
	// bHover = false → 反向播放（缩回去）
	const float StartTime = 0.f;
	const int32 NumLoopsToPlay = 1;
	const float PlaySpeed = 1.f;

	if (bHover)
	{
		PlayAnimation(InAnim, StartTime, NumLoopsToPlay, EUMGSequencePlayMode::Forward, PlaySpeed);
	}
	else
	{
		PlayAnimation(InAnim, StartTime, NumLoopsToPlay, EUMGSequencePlayMode::Reverse, PlaySpeed);
	}
}

