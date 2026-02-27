// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "UI_MapSlot.generated.h"

class UBorder;
class UImage;
class UButton;
class UUI_MapMenu;
 
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_MapSlot : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_MapSlot;
	UPROPERTY(meta = (BindWidget))
	UBorder* Border_Anim;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Prev;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Map;
	
	UFUNCTION()
	void OnClicked_MapSlot();

	UFUNCTION()
	void Hovered_MapSlot();
	UFUNCTION()
	void Unhovered_MapSlot();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnim(bool forward);

	UPROPERTY()
	UTexture2D* Texture;
	UPROPERTY()
	FName Name;

	void Update();

	UPROPERTY()
	UUI_MapMenu* FatherUI = nullptr;

	UPROPERTY()
	bool bIsSelected = false;

	void ResetSlot();
};
