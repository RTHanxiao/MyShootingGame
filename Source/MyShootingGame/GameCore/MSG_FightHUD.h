// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"GameFramework/HUD.h"
#include "MSG_FightHUD.generated.h"

class UUI_FightMain;
class UUI_ShowItem;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API AMSG_FightHUD : public AHUD
{
	GENERATED_BODY()

	AMSG_FightHUD();

	virtual void BeginPlay() override;

	void CreateFightMainUI();

//创建拾取物品
	void CreateShowItemUI(AActor* PickItem);

	//战斗UI
	TSubclassOf<UUI_FightMain> FightMainUIClass;
	UUI_FightMain* FightMainUI;

	//显示拾取物品
	TSubclassOf<UUI_ShowItem> ShowItemUIClass;
	UUI_ShowItem* ShowItemUI;

	
public:
	//关闭显示拾取物品UI
	void CloseShowItemUI();
	
	UFUNCTION(Blueprintcallable)
	UUI_ShowItem* GetShowItemUI() { return ShowItemUI; }
};
 