// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "Components/TextBlock.h"

void UMSG_UserWidgetBase::SetTextBlockColor(UTextBlock* Text, const FLinearColor& Color)
{
	if (!IsValid(Text)) return;

	// 这里走 UE 原生路径：SetColorAndOpacity(FSlateColor(Color))
	Text->SetColorAndOpacity(FSlateColor(Color));
}

void UMSG_UserWidgetBase::SetPreviewActor(AItemPreviewActor* InActor)
{
	PreviewActor = InActor;
}

FReply UMSG_UserWidgetBase::NativeOnMouseMove(const FGeometry& Geo, const FPointerEvent& Event)
{
	return FReply::Handled();
}

FReply UMSG_UserWidgetBase::NativeOnMouseWheel(const FGeometry& Geo, const FPointerEvent& Event)
{
	return FReply::Handled();
}



