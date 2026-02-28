// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/UI_HallMain.h"
#include "MyShootingGame/UI/GameUI/UI_HallMenu.h"
#include "MyShootingGame/UI/GameUI/UI_OutfitMenu.h"
#include "UI_MapMenu.h"
#include "UI_ConfigMenu.h"


void UUI_HallMain::NativeConstruct()
{
	Super::NativeConstruct();

	//// 绑定动画结束回调
	//BindAnimFinished(MainMenuAnim, TEXT("OnMainMenuAnimFinished"));
	//BindAnimFinished(OutfitMenuAnim, TEXT("OnOutfitMenuAnimFinished"));
	//BindAnimFinished(MapMenuAnim, TEXT("OnMapMenuAnimFinished"));
	//BindAnimFinished(ConfigMenuAnim, TEXT("OnConfigMenuAnimFinished"));

}


void UUI_HallMain::PlayMainMenuAnim(bool forward)
{
	bMainMenuForward = forward;

	if (forward)
	{
		// 准备从 Outfit 回到 Main, 先让 HallMenu 可见，再播放入场动画
		if (HallMenu)
		{
			HallMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}

	PlayAnimation(
		MainMenuAnim,
		0.f,
		1,
		forward ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		1.f
	);
}

void UUI_HallMain::PlayOutfitMenuAnim(bool forward)
{
	bOutfitMenuForward = forward;

	if (forward)
	{
		// 准备从 Main 进入 Outfit, 先让 Outfit 可见，再播放入场动画
		if (OutfitMenu)
		{
			OutfitMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}

	PlayAnimation(
		OutfitMenuAnim,
		0.f,
		1,
		forward ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		1.f
	);
}

void UUI_HallMain::PlayMapMenuAnim(bool forward)
{
	bMapMenuForward = forward;

	if (forward)
	{
		// 从 Outfit 进入 Map：先把 MapMenu 显示出来，再播入场动画
		if (MapMenu)
		{
			MapMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}

	PlayAnimation(
		MapMenuAnim,
		0.f,
		1,
		forward ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		1.f
	);
}

void UUI_HallMain::PlayConfigMenuAnim(bool forward)
{
	bConfigMenuForward = forward;

	if (forward)
	{
		// 从 MainMenu 进入 ConfigMenu：先让 ConfigMenu 显示
		if (ConfigMenu)
		{
			ConfigMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}

	PlayAnimation(
		ConfigMenuAnim,
		0.f,
		1,
		forward ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		1.f
	);
}


void UUI_HallMain::OnMainMenuAnimFinished()
{
	if (bMainMenuForward)
	{
		// 正向播放完：Main 完全出现了，可以把 Outfit 隐藏
		if (OutfitMenu)
		{
			OutfitMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		// 反向播放完：Main 完全收起了，可以把 Main 隐藏
		if (HallMenu)
		{
			HallMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUI_HallMain::OnOutfitMenuAnimFinished()
{
	if (bOutfitMenuForward)
	{
		// 正向播放完：Outfit 完全出现了，可以把 Main 隐藏
		if (HallMenu)
		{
			HallMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		// 反向播放完：Outfit 完全收起了，可以把 Outfit 隐藏
		if (OutfitMenu)
		{
			OutfitMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UUI_HallMain::OnMapMenuAnimFinished()
{
	if (bMapMenuForward)
	{
		// 正向播放完：Map 完全展开，可以把 Outfit 隐藏
		if (OutfitMenu)
		{
			OutfitMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		// 反向播放完：Map 完全收起，自己隐藏
		if (MapMenu)
		{
			MapMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUI_HallMain::OnConfigMenuAnimFinished()
{
	if (bConfigMenuForward)
	{
		// 正向：Config 出现后 → MainMenu 隐藏
		if (HallMenu)
		{
			HallMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		// 反向：Config 收起后 → 隐藏 Config
		if (ConfigMenu)
		{
			ConfigMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUI_HallMain::BindAnimFinished(UWidgetAnimation* Anim, FName FuncName)
{
	if (!Anim) return;

	FWidgetAnimationDynamicEvent FinishedDelegate;
	FinishedDelegate.BindUFunction(this, FuncName);
	BindToAnimationFinished(Anim, FinishedDelegate);
}
