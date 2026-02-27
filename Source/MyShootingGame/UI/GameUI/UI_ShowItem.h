// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyShootingGame/UI/MSG_UserWidgetBase.h"
#include "MyShootingGame/Items/InspectedItem.h"
#include "UI_ShowItem.generated.h"

class UImage;
class UTextBlock;
class UButton;
class AItemPreviewActor;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UUI_ShowItem : public UMSG_UserWidgetBase
{
	GENERATED_BODY()
	
	AItemPreviewActor* PreviewActor;

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
	

public:
	UPROPERTY(BlueprintReadWrite)
	AActor* InspectedItemActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AInspectedItem* InspectedItem = nullptr;

	//设置显示的物品
	//void SetInspectedItemMesh();

	UFUNCTION()
	void CloseButtonClicked();
};
