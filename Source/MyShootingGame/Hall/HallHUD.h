// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HallHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API AHallHUD : public AHUD
{
	GENERATED_BODY()
	
	AHallHUD();

	virtual void BeginPlay() override;
	
	void CreateHallMainUI();

	TSubclassOf<class UUI_HallMain> HallMainClass;
	class UUI_HallMain* HallMainUI;


public:
	UUI_HallMain* GetHallMainUI() const { return HallMainUI; }
};
