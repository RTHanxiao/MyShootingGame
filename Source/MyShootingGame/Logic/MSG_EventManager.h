// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MSG_EventManager.generated.h"

//切换武器UI的单播委托
DECLARE_DELEGATE(SwitchGunUI)
//设置准心显隐
DECLARE_DELEGATE_OneParam(ShowCrossHair,bool)
//设置UI射击模式
DECLARE_DELEGATE_OneParam(SetShootModeUI, bool)
//创建拾取物UI
DECLARE_DELEGATE_OneParam(ShowItemUI,AActor* )
//显示爆头图标
DECLARE_DELEGATE(HeadShotEvent)
//显示击中反馈
DECLARE_DELEGATE_OneParam(ShowHitFeedback, bool)
//击杀反馈
DECLARE_DELEGATE_OneParam(ShowKillFeedback, bool)
//角色受击
DECLARE_DELEGATE(PlayerInjured)
//大厅角色Mesh修改
DECLARE_DELEGATE_OneParam(ChangeHallCharacterMesh, USkeletalMesh*)

/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UMSG_EventManager : public UObject
{
	GENERATED_BODY()
	
public:
		// Sets default values for this actor's properties
		//UMSG_EventManager();
		UFUNCTION(BlueprintCallable, Category = "EventManager")
		//void TriggerEvent(FName EventName);
		static UMSG_EventManager* GetEventManagerInstance();

		SwitchGunUI SwitchGunUIDelegate;
		ShowCrossHair ShowCrossHairDelegate;
		SetShootModeUI SetShootModeUIDelegate;
		ShowItemUI ShowItemUIDelegate;
		HeadShotEvent HeadShotEventDelegate;
		ShowHitFeedback ShowHitFeedbackDelegate;
		ShowKillFeedback ShowKillFeedbackDelegate;
		PlayerInjured PlayerInjuredDelegate;
		ChangeHallCharacterMesh ChangeHallCharacterMeshDelegate;
private:
	static UMSG_EventManager* Instance;
	
};
