// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "MyShootingGame/Table/HallTemplate.h"
#include "UI_OutfitMenu.generated.h"

class UButton;
class UTextBlock;
class UWrapBox;
class UWidgetAnimation;
class DataTable;
class UUI_PrevSlot;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_OutfitMenu : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Map;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Map;
	UPROPERTY(meta = (BindWidget))
	UWrapBox* WB_SkinSlotPanel;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* MapTextHoverAnim;
	
	UFUNCTION()
	void BtnBackClicked();

	UFUNCTION()
	void BtnMapHover();

	UFUNCTION()
	void BtnMapUnHover();

	UFUNCTION()
	void BtnMapClicked();

	void PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover);

	void InitWrapBoxWithSkinSlots();

	TArray<UUI_PrevSlot*> SkinSlotWidgetArray;

public:
	UPROPERTY()
	UDataTable* HallSlotDataTable;

	TArray<FHallTemplate*> CachedSkinDataArray;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUI_PrevSlot> SlotClass;
};
