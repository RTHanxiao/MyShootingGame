// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_HoverItem.generated.h"

/**
 * 点击后跟随鼠标移动的物品Widget
 */
class UImage;
class UInv_InventoryItem;
class UTextBlock;
struct FGameplayTag;
UCLASS()
class INVENTORY_API UInv_HoverItem : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetImageBrush(const FSlateBrush& InBrush) const;
	void UpdateStackCount(const int32 NewStackCount) ;

	FGameplayTag GetItemTypeTag() const;
	int32 GetStackCount() const { return StackCount; }
	bool IsStackable() const { return bIsStackable; }
	void SetIsStackable(bool bNewIsStackable);
	int32 GetPreviousGridIndex() const { return PreviousGridIndex; }
	void SetPreviousGridIndex(const int32 NewIndex) { PreviousGridIndex = NewIndex; }
	FIntPoint GetGridDimisions() const { return GridDimisions; }
	void SetGridDimisions(const FIntPoint& NewDimisions) { GridDimisions = NewDimisions; }
	UInv_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }
	void SetInventoryItem(UInv_InventoryItem* NewItem) { InventoryItem = NewItem; }

private:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextStackCount;

	int32 PreviousGridIndex;
	FIntPoint GridDimisions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{ false };
	int32 StackCount{ 0 };


	
};
