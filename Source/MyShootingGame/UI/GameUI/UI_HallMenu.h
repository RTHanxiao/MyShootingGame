// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "UI_HallMenu.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_HallMenu : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Single;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Multi;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Cfg;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Single;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Multi;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Cfg;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Exit;

	UFUNCTION(BlueprintCallable)
	void BtnSingleHover();
	UFUNCTION(BlueprintCallable)
	void BtnMultiHover();
	UFUNCTION(BlueprintCallable)
	void BtnCfgHover();
	UFUNCTION(BlueprintCallable)
	void BtnExitHover();

	UFUNCTION(BlueprintCallable)
	void BtnSingleUnHover();
	UFUNCTION(BlueprintCallable)
	void BtnMultiUnHover();
	UFUNCTION(BlueprintCallable)
	void BtnCfgUnHover();
	UFUNCTION(BlueprintCallable)
	void BtnExitUnHover();

	UFUNCTION(BlueprintCallable)
	void BtnSingleClicked();
	UFUNCTION(BlueprintCallable)
	void BtnMultiClicked();
	UFUNCTION(BlueprintCallable)
	void BtnCfgClicked();
	UFUNCTION(BlueprintCallable)
	void BtnExitClicked();


protected:

	// 绑定蓝图里的动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SingleTextHoverAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* MultiTextHoverAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CfgTextHoverAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ExitTextHoverAnim;

public:

	void PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover);

};
