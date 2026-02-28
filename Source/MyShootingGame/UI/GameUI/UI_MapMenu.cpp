#include "MyShootingGame/UI/GameUI/UI_MapMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "MyShootingGame/Hall/HallPlayerController.h"
#include "MyShootingGame/Hall/HallHUD.h"
#include "UI_HallMain.h"
#include "MyShootingGame/UI/GameUI/Slot/UI_MapSlot.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MyShootingGame/GameCore/MSG_PlayerController.h"


void UUI_MapMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MapSlotDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_Game/Data/DT_MapSlot.DT_MapSlot"));
	if (!CachedMapData.Num())
	{
		MapSlotDataTable->GetAllRows<FMapPreviewTemplate>(TEXT("SkinData"), CachedMapData);
	}

	InitMapSlots();

	if (Btn_Start)
	{
		//Btn_Start->OnClicked.AddDynamic(this, &UUI_MapMenu::BtnStartClicked);
		//Btn_Start->OnHovered.AddDynamic(this, &UUI_MapMenu::BtnStartHoverd);
		//Btn_Start->OnUnhovered.AddDynamic(this, &UUI_MapMenu::BtnStartUnHoverd);
	}

	Btn_Back->OnClicked.AddDynamic(this, &UUI_MapMenu::BtnBackClicked);
}


void UUI_MapMenu::InitMapSlots()
{
	if (SlotClass)
	{
		for (int32 i = 0; i < CachedMapData.Num(); i++)
		{
			UUI_MapSlot* NewMapSlotWidget = CreateWidget<UUI_MapSlot>(GetWorld(), SlotClass);
			if (NewMapSlotWidget && SB_Map)
			{
				SB_Map->AddChild(NewMapSlotWidget);
				MapSlotWidgetArray.Add(NewMapSlotWidget);
			}
			NewMapSlotWidget->FatherUI = this;
			NewMapSlotWidget->Texture = CachedMapData[i]->MapImage;
			NewMapSlotWidget->Name = CachedMapData[i]->MapLevelName;
			NewMapSlotWidget->Update();
		}
	}
}

void UUI_MapMenu::BtnStartClicked()
{
	if (!CurSelecetedMap) return;
	// TODO：开始游戏逻辑（加载地图等）
	FName CurSelecetedMapName = CurSelecetedMap->Name;

	if (CurSelecetedMapName == FName("TestMap"))
	{
		UGameplayStatics::OpenLevel(this, CurSelecetedMap->Name);
		
	}
	else {
		LoadSubLevel(CurSelecetedMapName);
	}

	RemoveFromParent();
}

void UUI_MapMenu::BtnStartHoverd()
{
	if (TB_Start)
	{
		TB_Start->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f)); // 悬停变黑
	}

	PlayHoverAnim(StartTextHoverAnim, true);
}

void UUI_MapMenu::BtnStartUnHoverd()
{
	if (TB_Start)
	{
		TB_Start->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f)); // 离开变白
	}

	PlayHoverAnim(StartTextHoverAnim, false);
}


void UUI_MapMenu::BtnBackClicked()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			HUD->GetHallMainUI()->PlayMapMenuAnim(false);
			HUD->GetHallMainUI()->PlayOutfitMenuAnim(true);
		}
	}
}

void UUI_MapMenu::PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover)
{
	if (!InAnim)
		return;

	const float StartAtTime = 0.f;
	const int32 NumLoops = 1;
	const float PlaySpeed = 1.f;

	PlayAnimation(
		InAnim,
		StartAtTime,
		NumLoops,
		bHover ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse,
		PlaySpeed
	);
}
