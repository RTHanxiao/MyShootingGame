// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);

class UImage;
class UInv_InventoryItem;
class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;


	void SetIsStackable(const bool bStackable) { bIsStackable = bStackable; }
	bool IsStackable() const { return bIsStackable; }
	UImage* GetImageIcon() const { return Image_Icon; }
	void SetGridIndex(const int32 NewIndex) { GridIndex = NewIndex; }
	int32 GetGridIndex() const { return GridIndex; }
	void SetGridDimensions(const FIntPoint& NewDimensions) {GridDimensions = NewDimensions;}
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetInventoryItem(UInv_InventoryItem* NewItem);
	UInv_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }
	void SetImageBrush(const FSlateBrush& NewBrush) const ;
	void UpdateStackCount(int32 StackCount);

	FSlottedItemClicked OnSlottedItemClicked;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage>Image_Icon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>Text_StackCount;

	
	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{ false };
};
