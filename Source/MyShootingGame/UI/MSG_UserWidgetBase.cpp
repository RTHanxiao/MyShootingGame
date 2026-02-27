// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/UI/MSG_UserWidgetBase.h"



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



