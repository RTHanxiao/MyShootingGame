#include "MyShootingGame/UI/GameUI/UI_ConfigMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyShootingGame/GameCore/MSG_PlayerController.h"
#include "MyShootingGame/Hall/HallHUD.h"
#include "UI_HallMain.h"

void UUI_ConfigMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UUI_ConfigMenu::BtnBackClicked);
	}

	if (Btn_Apply)
	{
		Btn_Apply->OnClicked.AddDynamic(this, &UUI_ConfigMenu::BtnApplyClicked);
		Btn_Apply->OnHovered.AddDynamic(this, &UUI_ConfigMenu::BtnApplyHovered);
		Btn_Apply->OnUnhovered.AddDynamic(this, &UUI_ConfigMenu::BtnApplyUnHovered);
	}
}

void UUI_ConfigMenu::BtnBackClicked()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			UUI_HallMain* MainUI = HUD->GetHallMainUI();

			MainUI->PlayMainMenuAnim(true);       // MainMenu 正向出现
			MainUI->PlayConfigMenuAnim(false);    // Config 反向退场
		}
	}

}

void UUI_ConfigMenu::BtnApplyHovered()
{
	if (Tb_Apply)
	{
		Tb_Apply->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f)); // 黑
	}

	PlayHoverAnim(ApplyHoverAnim, true);
}

void UUI_ConfigMenu::BtnApplyUnHovered()
{
	if (Tb_Apply)
	{
		Tb_Apply->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f)); // 白
	}

	PlayHoverAnim(ApplyHoverAnim, false);
}

void UUI_ConfigMenu::BtnApplyClicked()
{
	// TODO: 应用设置（保存音量、分辨率等）
}

void UUI_ConfigMenu::PlayHoverAnim(UWidgetAnimation* Anim, bool bHover)
{
	if (!Anim) return;

	PlayAnimation(
		Anim,
		0.f,
		1,
		bHover ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		1.f
	);
}
