#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "MyShootingGame/Table/HallTemplate.h"
#include "UI_MapMenu.generated.h"

class UScrollBox;
class UButton;
class UTextBlock;
class UWidgetAnimation;
class UDataTable;
class UUI_MapSlot;

UCLASS()
class MYSHOOTINGGAME_API UUI_MapMenu : public UMSG_UserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TB_Start;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Start;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* SB_Map;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StartTextHoverAnim;

	UDataTable* MapSlotDataTable;

	TArray<FMapPreviewTemplate*> CachedMapData;

	TArray<UUI_MapSlot*> MapSlotWidgetArray;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUI_MapSlot> SlotClass;
			
	void InitMapSlots();

	// 按钮事件
	UFUNCTION()
	void BtnStartClicked();

	UFUNCTION()
	void BtnStartHoverd();

	UFUNCTION()
	void BtnStartUnHoverd();

	UFUNCTION()
	void BtnBackClicked();

	// 通用：播放动画并控制方向
	void PlayHoverAnim(UWidgetAnimation* InAnim, bool bHover);

public:
	UPROPERTY()
	UUI_MapSlot* CurSelecetedMap = nullptr;
		
	UFUNCTION(BlueprintImplementableEvent)
	void LoadSubLevel(FName CurSelecetedMapName);
};
