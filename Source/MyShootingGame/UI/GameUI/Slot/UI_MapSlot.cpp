// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/GameUI/Slot/UI_MapSlot.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Border.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Image.h"
#include "../UI_MapMenu.h"

void UUI_MapSlot::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Map->OnClicked.AddDynamic(this, &UUI_MapSlot::OnClicked_MapSlot);
	Button_Map->OnHovered.AddDynamic(this, &UUI_MapSlot::Hovered_MapSlot);
	Button_Map->OnUnhovered.AddDynamic(this, &UUI_MapSlot::Unhovered_MapSlot);
	Border_Anim->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_MapSlot::OnClicked_MapSlot()
{
	if (!FatherUI) return;
	if (FatherUI->CurSelecetedMap)
	{
		FatherUI->CurSelecetedMap->ResetSlot();
	}
	FatherUI->CurSelecetedMap = this;
	bIsSelected = true;

	Border_MapSlot->SetVisibility(ESlateVisibility::Visible);
}

void UUI_MapSlot::Hovered_MapSlot()
{
	if (FatherUI->CurSelecetedMap != this)
	{
		PlayAnim(true);
	}
	
}

void UUI_MapSlot::Unhovered_MapSlot( )
{
	if (!bIsSelected)
	{
		PlayAnim(false);
	}
	
}

void UUI_MapSlot::Update()
{
	if (Texture)
	{
		Image_Prev->SetBrushFromTexture(Texture);
	}
}

void UUI_MapSlot::ResetSlot()
{
	bIsSelected = false;
	Border_MapSlot->SetVisibility(ESlateVisibility::Hidden);
	PlayAnim(false);
}
