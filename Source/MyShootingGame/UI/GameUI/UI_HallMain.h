// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "UI_HallMain.generated.h"

class UUI_HallMenu;
class UUI_OutfitMenu;
class UWidgetAnimation;
class UUI_MapMenu;
class UUI_ConfigMenu;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_HallMain : public UMSG_UserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUI_HallMenu* HallMenu;
	UPROPERTY(meta = (BindWidget))
	UUI_OutfitMenu* OutfitMenu;
	UPROPERTY(meta = (BindWidget))
	UUI_MapMenu* MapMenu;
	UPROPERTY(meta = (BindWidget))
	UUI_ConfigMenu* ConfigMenu;


	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* MainMenuAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OutfitMenuAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* MapMenuAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ConfigMenuAnim;



	//记录这次播放是正向还是反向
	bool bMainMenuForward = true;
	bool bOutfitMenuForward = true;
	bool bMapMenuForward = true;
	bool bConfigMenuForward = true;


protected:
	virtual void NativeConstruct() override;

	// 动画结束回调
	UFUNCTION()
	void OnMainMenuAnimFinished();
	UFUNCTION()
	void OnOutfitMenuAnimFinished();
	UFUNCTION()
	void OnMapMenuAnimFinished();
	UFUNCTION()
	void OnConfigMenuAnimFinished();

	void BindAnimFinished(UWidgetAnimation* Anim, FName FuncName);

public:
	UFUNCTION(BlueprintCallable)
	void PlayMainMenuAnim(bool forward);

	UFUNCTION(BlueprintCallable)
	void PlayOutfitMenuAnim(bool forward);

	UFUNCTION(BlueprintCallable)
	void PlayMapMenuAnim(bool forward);

	UFUNCTION(BlueprintCallable)
	void PlayConfigMenuAnim(bool forward);
};
