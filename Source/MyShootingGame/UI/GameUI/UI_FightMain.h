// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Image.h"
#include "UI_FightMain.generated.h"

class UWidgetSwitcher;
class UInv_InfoMessage;
/** 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_FightMain : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WeaponInfoSwitcher;
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairUP;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairDown;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairRight;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairCenter;

	//信息提示UI
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InfoMessage> InfoMessageUI;
	UFUNCTION()
	void OnNoRoom();

	//设置准心显示状态
	void SetCrossHairVisibility(bool bIsVisible);

	//修改武器信息显示
	void ChangWeaponInfo();
	
	//更新射击模式UI
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateShootModeUI(bool bIsAuto);

	//设置爆头标志显示状态
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHeadShotUIAnim();

	//击中反馈显示
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitFeedbackUI(bool IsHeadShot);
	//击杀反馈
	UFUNCTION(BlueprintImplementableEvent)
	void ShowKillFeedbackUI(bool IsHeadShot);
	//角色受击反馈
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPlayerInjuredUI();

};
