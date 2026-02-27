// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/Slot/UI_PrevSlot.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Image.h"
#include "MyShootingGame/Logic/MSG_EventManager.h"

void UUI_PrevSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_PrevSlot)
	{
		Button_PrevSlot->OnClicked.AddDynamic(this, &UUI_PrevSlot::OnClicked_PrevSlot);
		Button_PrevSlot->OnHovered.AddDynamic(this, &UUI_PrevSlot::Hovered_PrevSlot);
		Button_PrevSlot->OnUnhovered.AddDynamic(this, &UUI_PrevSlot::Unhovered_PrevSlot);
	}
}

void UUI_PrevSlot::OnClicked_PrevSlot()
{
	UMSG_EventManager::GetEventManagerInstance()->ChangeHallCharacterMeshDelegate.ExecuteIfBound(ButtonSkeletalMesh);
}

void UUI_PrevSlot::Hovered_PrevSlot()
{
	PlayAnim(true);
}

void UUI_PrevSlot::Unhovered_PrevSlot()
{
	PlayAnim(false);
}

void UUI_PrevSlot::UpdateSlot()
{
	if (Button_PrevSlot)
	{
		Image_Preview->SetBrushFromTexture(ButtonImg);
	}
}
