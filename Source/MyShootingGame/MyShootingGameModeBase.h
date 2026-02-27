// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyShootingGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class MYSHOOTINGGAME_API AMyShootingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyShootingGameModeBase();

	virtual void BeginPlay() override;

protected:

	// ==== 预加载的两种模式 ==== //

	UPROPERTY(EditAnywhere, Category = "MSG|Hall")
	TSubclassOf<class APawn> HallPawnClass;

	UPROPERTY(EditAnywhere, Category = "MSG|Hall")
	TSubclassOf<class APlayerController> HallControllerClass;

	UPROPERTY(EditAnywhere, Category = "MSG|Hall")
	TSubclassOf<class AHUD> HallHUDClass;

	UPROPERTY(EditAnywhere, Category = "MSG|Fight")
	TSubclassOf<class APawn> FightPawnClass;

	UPROPERTY(EditAnywhere, Category = "MSG|Fight")
	TSubclassOf<class APlayerController> FightControllerClass;

	UPROPERTY(EditAnywhere, Category = "MSG|Fight")
	TSubclassOf<class AHUD> FightHUDClass;

public:

	/** 蓝图可调用：根据当前世界切换模式（大厅 / 战斗） */
	UFUNCTION(BlueprintCallable, Category = "MSG|WorldSwitch")
	void SwitchWorldMode(FName LoadedLevelName);

	/** 切换成大厅模式 */
	void SetHallMode();

	/** 切换成战斗模式 */
	void SetFightMode();
};
