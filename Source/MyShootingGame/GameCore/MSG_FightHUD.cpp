// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/GameCore/MSG_FightHUD.h"
#include "../UI/GameUI/UI_FightMain.h"
#include "../UI/GameUI/UI_ShowItem.h"
#include "../Logic/MSG_EventManager.h"
#include "MSG_PlayerController.h"
#include "../Character/PlayerCharacter.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"



AMSG_FightHUD::AMSG_FightHUD()
{
	static ConstructorHelpers::FClassFinder<UUI_FightMain> BP_FightMainUIClass(TEXT("/Game/_Game/BP/UI/GameUI/BP_UI_FightMain"));
	if (BP_FightMainUIClass.Class)
	{
		FightMainUIClass = BP_FightMainUIClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUI_ShowItem> BP_ShowItemUIClass(TEXT("/Game/_Game/BP/UI/GameUI/BP_UI_ShowItem"));
	if (BP_ShowItemUIClass.Class)
	{
		ShowItemUIClass = BP_ShowItemUIClass.Class;
	}
}

void AMSG_FightHUD::BeginPlay()
{
	Super::BeginPlay();
	CreateFightMainUI();
	UMSG_EventManager::GetEventManagerInstance()->ShowItemUIDelegate.BindUObject(this, &AMSG_FightHUD::CreateShowItemUI);
}

void AMSG_FightHUD::CreateFightMainUI()
{
	FightMainUI = CreateWidget<UUI_FightMain>(GetWorld(), FightMainUIClass);
	FightMainUI->AddToViewport(0);
}

void AMSG_FightHUD::CreateShowItemUI(AActor* PickItem)
{
	ShowItemUI = CreateWidget<UUI_ShowItem>(GetWorld(), ShowItemUIClass);
	ShowItemUI->AddToViewport(0);
	ShowItemUI->InspectedItemActor = PickItem;
	//ShowItemUI->SetInspectedItemMesh();

	if(AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayerController()))
	{
		PC->bShowMouseCursor = true;
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, ShowItemUI,EMouseLockMode::DoNotLock);
	}
	if (APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(GetOwningPawn()))
	{
		
		MyPlayer->SetMappingContext(false);
		MyPlayer->IsPickItemUIShow = true;
	}
}


void AMSG_FightHUD::CloseShowItemUI()
{
	ShowItemUI->RemoveFromParent();
	ShowItemUI = nullptr;
	if (AMSG_PlayerController* PC = Cast<AMSG_PlayerController>(GetOwningPlayerController()))
	{
		PC->bShowMouseCursor = false;
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);
	}
	if (APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(GetOwningPawn()))
	{
		MyPlayer->SetMappingContext(true);
		MyPlayer->IsPickItemUIShow = false;
	}
}
