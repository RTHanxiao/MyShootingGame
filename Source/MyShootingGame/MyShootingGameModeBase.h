#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyShootingGameModeBase.generated.h"

UCLASS()
class MYSHOOTINGGAME_API AMyShootingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyShootingGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 服务器侧（GameMode 蓝图里实现）：流加载子关卡
	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void BP_LoadSubLevel(FName LevelName);

	// 服务器侧（由蓝图在“流加载完成”时回调），统一切模式
	UFUNCTION(BlueprintCallable, Category = "MSG|WorldSwitch")
	void SwitchWorldMode(FName LoadedLevelName);

	/** 切换成大厅模式（对所有玩家） */
	void SetHallMode();

	/** 切换成战斗模式（对所有玩家） */
	void SetFightMode();

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

	// 新玩家加入时，对该玩家补齐当前模式（避免第二个客户端没 UI）
	void ApplyModeForPlayer(APlayerController* PC);
	void SetHallModeForPlayer(APlayerController* PC);
	void SetFightModeForPlayer(APlayerController* PC);

	enum class EMSGWorldMode : uint8
	{
		Hall,
		Fight
	};

	EMSGWorldMode CurrentWorldMode = EMSGWorldMode::Hall;

	void ApplyModeForPlayer_Delayed(APlayerController* PC);
};