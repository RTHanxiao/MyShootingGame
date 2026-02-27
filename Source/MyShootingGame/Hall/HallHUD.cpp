// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Hall/HallHUD.h"
#include "../UI/GameUI/UI_HallMain.h"




AHallHUD::AHallHUD()
{
	static ConstructorHelpers::FClassFinder<UUI_HallMain> BP_UI_HallMainClass(TEXT("/Game/_Game/BP/UI/HallUI/BP_UI_HallMain"));
	if(BP_UI_HallMainClass.Class)
	{
		HallMainClass = BP_UI_HallMainClass.Class;
	}
}

void AHallHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateHallMainUI();
}

void AHallHUD::CreateHallMainUI()
{
	HallMainUI = CreateWidget<UUI_HallMain>(GetWorld(), HallMainClass);
	HallMainUI->AddToViewport(0);
}
