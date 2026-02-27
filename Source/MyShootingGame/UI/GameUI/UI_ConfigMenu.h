#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "UI_ConfigMenu.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class MYSHOOTINGGAME_API UUI_ConfigMenu : public UMSG_UserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 按钮
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Apply;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;

	// Apply 文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Apply;

	// Apply 悬停放大动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ApplyHoverAnim;

	// 事件
	UFUNCTION()
	void BtnBackClicked();

	UFUNCTION()
	void BtnApplyHovered();

	UFUNCTION()
	void BtnApplyUnHovered();

	UFUNCTION()
	void BtnApplyClicked();

protected:
	// 通用动画播放：悬停 true 正放、离开 false 反放
	void PlayHoverAnim(UWidgetAnimation* Anim, bool bHover);
};
