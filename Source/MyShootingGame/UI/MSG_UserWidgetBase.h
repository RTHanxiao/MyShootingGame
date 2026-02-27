// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnLuaInterface.h"
#include "MSG_UserWidgetBase.generated.h"

class AItemPreviewActor;
/**
 * 
 */
UCLASS()
class MYSHOOTINGGAME_API UMSG_UserWidgetBase : public UUserWidget, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnLua")
	FString LuaModuleName;

	virtual FString GetModuleName_Implementation() const override
	{
		return LuaModuleName;
	}

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetPreviewActor(AItemPreviewActor* InActor);

protected:
	AItemPreviewActor* PreviewActor;
	
	
};
