// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "UI_PrevSlot.generated.h"

class UBorder;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_PrevSlot : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border_PrevSlot;
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Preview;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_PrevSlot;
	
public:
	UFUNCTION()
	void OnClicked_PrevSlot();

	UFUNCTION()
	void Hovered_PrevSlot();
	UFUNCTION()
	void Unhovered_PrevSlot();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnim(bool forward);

	UPROPERTY()
	UTexture2D* ButtonImg;
	UPROPERTY()
	USkeletalMesh* ButtonSkeletalMesh;

	void UpdateSlot();
};
