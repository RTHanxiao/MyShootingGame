// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/UI_OutfitMenu.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h"
#include "MyShootingGame/Hall/HallPlayerController.h"
#include "MyShootingGame/Hall/HallHUD.h"
#include "UI_HallMain.h"
#include "MyShootingGame/Hall/HallPawn.h"
#include "Slot/UI_PrevSlot.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/WrapBox.h"
#include "MyShootingGame/GameCore/MSG_PlayerController.h"


void UUI_OutfitMenu::NativeConstruct()
{
	Super::NativeConstruct();

	HallSlotDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_Game/Data/DT_HallSlot"));
	if (!CachedSkinDataArray.Num())
	{
		HallSlotDataTable->GetAllRows<FHallTemplate>(TEXT("SkinData"), CachedSkinDataArray);
	}

	InitWrapBoxWithSkinSlots();
	
	//Btn_Map->OnHovered.AddDynamic(this, &UUI_OutfitMenu::BtnMapHover);
	//Btn_Map->OnUnhovered.AddDynamic(this, &UUI_OutfitMenu::BtnMapUnHover);
	//Btn_Map->OnClicked.AddDynamic(this, &UUI_OutfitMenu::BtnMapClicked);
	//
	//Btn_Back->OnClicked.AddDynamic(this, &UUI_OutfitMenu::BtnBackClicked);

	
}


void UUI_OutfitMenu::PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover)
{
	if (!InAnim)
	{
		return;
	}

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


void UUI_OutfitMenu::InitWrapBoxWithSkinSlots()
{
	for (FHallTemplate* SkinData : CachedSkinDataArray)
	{
		UUI_PrevSlot* NewSkinSlotWidget = CreateWidget<UUI_PrevSlot>(GetWorld(), SlotClass);
		WB_SkinSlotPanel->AddChild(NewSkinSlotWidget);
		SkinSlotWidgetArray.Add(NewSkinSlotWidget);
	}
	for (int32 i = 0; i < SkinSlotWidgetArray.Num(); ++i)
	{
		SkinSlotWidgetArray[i]->ButtonImg = CachedSkinDataArray[i]->SlotImage;
		SkinSlotWidgetArray[i]->ButtonSkeletalMesh = CachedSkinDataArray[i]->SlotSkeletalMesh;
		SkinSlotWidgetArray[i]->UpdateSlot();
	}
}

void UUI_OutfitMenu::BtnBackClicked()
{
	if (AHallPawn* Pawn = Cast<AHallPawn>(GetOwningPlayerPawn()))
	{
		Pawn->SetHallCameraFromMainToOutfit(false);
	}

	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			HUD->GetHallMainUI()->PlayOutfitMenuAnim(false);
			HUD->GetHallMainUI()->PlayMainMenuAnim(true);
		}
	}
}

void UUI_OutfitMenu::BtnMapHover()
{
	if (Tb_Map)
	{
		// ÐüÍ£±äºÚ
		Tb_Map->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	}
	PlayHoverAnim(MapTextHoverAnim, true);
}

void UUI_OutfitMenu::BtnMapUnHover()
{
	if (Tb_Map)
	{
		// Àë¿ª±ä°×
		Tb_Map->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
	PlayHoverAnim(MapTextHoverAnim, false);
}

void UUI_OutfitMenu::BtnMapClicked()
{
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayer()))
	{
		if (AHallHUD* HUD = Cast<AHallHUD>(PC->GetHUD()))
		{
			HUD->GetHallMainUI()->PlayOutfitMenuAnim(false);
			HUD->GetHallMainUI()->PlayMapMenuAnim(true);
		}
	}
}
