

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Type/Inv_GridTypes.h"
#include "../Widget/Utils/Inv_WidgetUtils.h"
#include "Inv_InventoryStatic.generated.h"

class UInv_InventoryComponent;
class UInv_HoverItem;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_InventoryComponent* GetInventoryComponent(const APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EInv_ItemCategory GetItemCategoryFromItemComp(UInv_ItemComponent* ItemComp);

	template<typename T,typename FunctionT>
	static void ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FunctionT& Function);

	UFUNCTION(BlueprintCallable,Category = "Inventory")
	static void ItemHovered(APlayerController* PC, UInv_InventoryItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemUnhovered(APlayerController* PC);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_HoverItem* GetHoverItem(APlayerController* PC);

	static UInv_InventoryBase* GetInventoryWidget(APlayerController* PC);
};

template<typename T, typename FunctionT>
void UInv_InventoryStatic::ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FunctionT& Function)
{
	for(int32 j = 0;j < Range2D.Y; ++j)
	{
		for(int32 i = 0; i < Range2D.X; ++i)
		{
			//一维转二维变成格子坐标
			const FIntPoint Coordinates = UInv_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);
			const int32 TileIndex = UInv_WidgetUtils::GetIndexFromPosition(Coordinates, GridColumns);
			if(Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
