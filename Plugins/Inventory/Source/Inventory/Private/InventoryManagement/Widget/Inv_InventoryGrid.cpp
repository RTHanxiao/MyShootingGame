// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/Inv_InventoryGrid.h"
#include "InventoryManagement/Widget/GridSlots/Inv_GridSlot.h"
#include "Components/CanvasPanel.h"
#include "InventoryManagement/Widget/Utils/Inv_WidgetUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Component/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "InventoryManagement/Widget/SlottedItems/Inv_SlottedItem.h"
#include "InventoryManagement/Widget/HoverItem/Inv_HoverItem.h"
#include "InventoryManagement/Widget/PopupMenu/Inv_ItemPopup.h"



void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 1) 先构建网格 Slot UI
	ConstructGrid();

	// 2) 绑定背包组件事件：当有物品加入时更新信息
	InventoryComp = UInv_InventoryStatic::GetInventoryComponent(GetOwningPlayer());
	if (!InventoryComp.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryGrid::NativeOnInitialized InventoryComp is null."));
		return;
	}
	InventoryComp->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
	InventoryComp->OnStackChanged.AddDynamic(this, &UInv_InventoryGrid::AddStack);
	InventoryComp->OnInventoryMenuToggled.AddDynamic(this, &ThisClass::OnInventoryMenuToggled);

}


void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPos = UInv_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitCanvas(CanvasPos, UInv_WidgetUtils::GetWidgetSize(CanvasPanel), MousePos))
	{
		return;
	}

	UpdateTileParameters(CanvasPos, MousePos);
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPos, const FVector2D& MousePos)
{
	if (!bMouseWithCanvas) return;

	//得到鼠标悬停的格子坐标
	const FIntPoint HoverCoord = GetHoverCoord(CanvasPos, MousePos);
	 
	//更新 TileParameters
	PreviousTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoverCoord;
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoverCoord, Columns);
	TileParameters.TileQuadrant = GetTileQuadrant(CanvasPos, MousePos);//计算鼠标在格子的第几象限

	//高亮相对应的格子
	OnTileParametersChanged(TileParameters);
	
}

void UInv_InventoryGrid::OnTileParametersChanged(const FInv_TileParameters& Parameter)
{
	if (!IsValid(HoverItem)) return;

	//得到悬停物品的大小
	const FIntPoint Dimensions = HoverItem->GetGridDimisions();

	//计算需要高亮的格子起始索引
	const FIntPoint StartCoord = GetStartCoord(Parameter.TileCoordinates, Dimensions, Parameter.TileQuadrant);
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartCoord, Columns);
	
	//检查悬浮位置
	CurrentHoverResult = CheckHoverPosSpace(StartCoord, Dimensions);

	if(CurrentHoverResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnhighlightSlots(PreviousHighlightedIndex, PreviousHighlightedDimension);

	if(CurrentHoverResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentHoverResult.UpperLeftIndex))
	{
		//只有单个物品在当前空间内，可以交换
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentHoverResult.ValidItem.Get(), FragmentTags::GridFragment);
		if (!GridFragment) return;

		ChangeHoverType(CurrentHoverResult.UpperLeftIndex, GridFragment->GetGridSize(), EInv_GridSlotState::GrayedOut);
	}

}

FInv_SpaceQueryResult UInv_InventoryGrid::CheckHoverPosSpace(const FIntPoint& Position, const FIntPoint& Dimensions) 
{
	FInv_SpaceQueryResult Result;

	//在范围内吗
	if (!IsWithinBounds(UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;

	//范围内的所有格子都有相同的UpperLeft 吗
	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatic::ForEach2D(GridSlots, UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns,[&](const UInv_GridSlot* GridSlot)
	{
			if (GridSlot->GetInventoryItem().IsValid())
			{
				OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
				Result.bHasSpace = false;
			}
	});

	//可以交换吗
	if (OccupiedUpperLeftIndices.Num() == 1)
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}

	return Result;
}



bool UInv_InventoryGrid::CursorExitCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location) 
{
	bWasMouseWithinCanvas = bMouseWithCanvas;
	bMouseWithCanvas = UInv_WidgetUtils::IsWithinBounds(BoundaryPos,BoundarySize,Location);

	if(!bMouseWithCanvas && bWasMouseWithinCanvas)
	{
		//取消高亮
		UnhighlightSlots(PreviousHighlightedIndex, PreviousHighlightedDimension);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimension)
{
	if (!bMouseWithCanvas) return;
	UnhighlightSlots(PreviousHighlightedIndex, PreviousHighlightedDimension);
	UInv_InventoryStatic::ForEach2D(GridSlots, Index, Dimension, Columns, [&](UInv_GridSlot* GridSlot)
	{
			GridSlot->SetOccupiedTexture();
	});
	PreviousHighlightedIndex = Index;
	PreviousHighlightedDimension = Dimension;
}

void UInv_InventoryGrid::UnhighlightSlots(const int32 Index, const FIntPoint& Dimension)
{
	UInv_InventoryStatic::ForEach2D(GridSlots, Index, Dimension, Columns, [&](UInv_GridSlot* GridSlot)
		{
			if (GridSlot->IsAvailable())
			{
				GridSlot->SetUnoccupiedTexture();
			}
			else
			{
				GridSlot->SetOccupiedTexture();
			}
		});
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimension, EInv_GridSlotState GridSlotState)
{
	UnhighlightSlots(PreviousHighlightedIndex, PreviousHighlightedDimension);
	UInv_InventoryStatic::ForEach2D(GridSlots, Index, Dimension, Columns, [State = GridSlotState](UInv_GridSlot* GridSlot)
		{
			switch (State)
			{
			case EInv_GridSlotState::Occupied:
				GridSlot->SetOccupiedTexture();
				break;
			case EInv_GridSlotState::Selected:
				GridSlot->SetSelectedTexture();
				break;
			case EInv_GridSlotState::GrayedOut:
				GridSlot->SetGrayedOutTexture();
				break;
			case EInv_GridSlotState::Unoccupied:
				GridSlot->SetUnoccupiedTexture();
				break;
			}
		});
	PreviousHighlightedDimension = Dimension;
	PreviousHighlightedIndex = Index;
}

FIntPoint UInv_InventoryGrid::GetStartCoord(const FIntPoint& Coord, const FIntPoint& Dimension, const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimension.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimension.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartCoord = Coord;
	switch (Quadrant)
	{
	case EInv_TileQuadrant::TopLeft:
		StartCoord.X = Coord.X - FMath::FloorToInt(.5f * Dimension.X);
		StartCoord.Y = Coord.Y - FMath::FloorToInt(.5f * Dimension.Y);
		break;
	case EInv_TileQuadrant::TopRight:
		StartCoord.X = Coord.X - FMath::FloorToInt(.5f * Dimension.X) + HasEvenWidth;
		StartCoord.Y = Coord.Y - FMath::FloorToInt(.5f * Dimension.Y);
		break;
	case EInv_TileQuadrant::BottomLeft:
		StartCoord.X = Coord.X - FMath::FloorToInt(.5f * Dimension.X);
		StartCoord.Y = Coord.Y - FMath::FloorToInt(.5f * Dimension.Y) + HasEvenHeight;
		break;
	case EInv_TileQuadrant::BottomRight:
		StartCoord.X = Coord.X - FMath::FloorToInt(.5f * Dimension.X) + HasEvenWidth;
		StartCoord.Y = Coord.Y - FMath::FloorToInt(.5f * Dimension.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Warning, TEXT("InventoryGrid::GetStartCoord Invalid Quadrant."));
		return FIntPoint{ INDEX_NONE, INDEX_NONE };
		break;
	}
	return StartCoord;
}

FIntPoint UInv_InventoryGrid::GetHoverCoord(const FVector2D& CanvasPos, const FVector2D& MousePos) const
{
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePos.X - CanvasPos.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePos.Y - CanvasPos.Y) / TileSize))
	};
}

EInv_TileQuadrant UInv_InventoryGrid::GetTileQuadrant(const FVector2D& CanvasPos, const FVector2D& MousePos) const
{
	//计算鼠标在格子的本地坐标
	const float TileLocalX = FMath::Fmod((MousePos.X - CanvasPos.X), TileSize);
	const float TileLocalY = FMath::Fmod((MousePos.Y - CanvasPos.Y), TileSize);

	//判断象限
	const bool bIsLeft = TileLocalX < (TileSize / 2);
	const bool bIsTop = TileLocalY < (TileSize / 2);

	EInv_TileQuadrant Result;
	if(bIsLeft && bIsTop)
	{
		Result = EInv_TileQuadrant::TopLeft;
	}
	else if(!bIsLeft && bIsTop)
	{
		Result = EInv_TileQuadrant::TopRight;
	}
	else if(bIsLeft && !bIsTop)
	{
		Result = EInv_TileQuadrant::BottomLeft;
	}
	else //if(!bIsLeft && !bIsTop)
	{
		Result = EInv_TileQuadrant::BottomRight;
	}
	return Result;
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComp)
{
	return HasRoomForItem(ItemComp->GetItemManifest());
}


FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item, const int32 StackAmout)
{
	return HasRoomForItem(Item->GetItemManifest(),StackAmout);
}

	// - 非堆叠物品：只要找到一个满足尺寸的空区域即可（占用多个格子）
	// - 可堆叠物品：优先找“同类型且可堆叠且未满”的位置，其次找空区域
	// - 返回值包含：可用位置列表、每个位置能填充多少、最终剩余多少（RemainingToFill）,Result的Index指向 UpperLeft 格子
FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& Manifest, const int32 StackAmout)
{
	FInv_SlotAvailabilityResult Result;
	
	//确定是否可堆叠，有 StackableFragment 代表该 Item 支持堆叠
	const FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	//最大堆叠与需要填充的数量
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmoutToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;
	if (StackAmout != -1 && Result.bStackable)
	{
		AmoutToFill = StackAmout;
	}

	//CheckedIndices用于防止对同一 Index 重复做“区域检查”（尤其是多格物品时，某些格子会在不同起点下反复被覆盖）
	TSet<int32> CheckedIndices;
	//遍历格子寻找空位或可堆叠位 for each gridslot
	for (const auto& GridSlot : GridSlots)
	{
		//更新完了退出
		if (AmoutToFill == 0) break;

		//下标位置格子被声明了吗，声明/占用过就跳过
		if(IsIndexClaimed(CheckedIndices,GridSlot->GetIndex())) continue;

		//是否在边界内，避免过界
		if (!IsWithinBounds(GridSlot->GetIndex(), GetItemDimensions(Manifest))) continue;

		//放得下吗
		TSet<int32> TempCheckedIndices; //物品可能占多格，则需要检查所有格子，这里用于防止重复检查
		if (!HasRoomAtIndex(GridSlot,GetItemDimensions(Manifest),CheckedIndices,TempCheckedIndices,Manifest.GetItemType(),MaxStackSize))
		{
			continue;
		}

		//本次检查覆盖到的格子，加入全局 CheckedIndices
		CheckedIndices.Append(TempCheckedIndices);
		
		//计算在该 Slot 上能填充多少：
		// 堆叠-min(剩余数量, 该 Slot 剩余空间)
		//  非堆叠-固定为 1（放一个物品实例）
		const int32 AmoutToFillInSlot = HowManySlotsToFill(Result.bStackable, MaxStackSize, AmoutToFill, GridSlot);
		if(AmoutToFillInSlot == 0)	continue;

		//更新仍需填充的数量
		Result.TotalRoomToFill += AmoutToFillInSlot;

		//记录本次可用位置：
		//如果目标格子已有物品（堆叠），需要定位到该物品的 UpperLeftIndex
		//否则当前位置即 UpperLeft
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetIndex(),
				Result.bStackable ? AmoutToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);
		AmoutToFill -= AmoutToFillInSlot;

		//剩余数量
		Result.RemainingToFill = AmoutToFill;

		if (AmoutToFill == 0) return Result;
	}
	return Result;
}


// 以 GridSlot 作为“假定左上角”，遍历 Dimensions 覆盖的所有子格，逐一校验：
// 子格是否越界/重复检查
// 子格是否空
// 若非空：是否为同类型可堆叠、且当前起点确实为该物品的 UpperLeft
// ClaimedIndices：本次起点覆盖到的格子集合（用于后续 CheckedIndices 去重）
bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, 
																	  const TSet<int32>& CheckedIndices, TSet<int32>& ClaimedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	bool bHasRoomAtIndex = true;

	//GridSlot作为左上角（希望如此），检查物品占用的所有格子
	UInv_InventoryStatic::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimensions, Columns, [&](const UInv_GridSlot* SubGridSlot)
		{
			if (CheckSlot(GridSlot,SubGridSlot,CheckedIndices,ClaimedIndices,ItemType,MaxStackSize))
			{
				ClaimedIndices.Add(SubGridSlot->GetIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		});

	return bHasRoomAtIndex;
}



FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}

// 单格校验规则：
// 1) 若 SubGridSlot 已在 CheckedIndices 中，直接失败（避免重复区域）。
// 2) 若为空格：通过，并声明占用。
// 3) 若非空：必须满足
// 当前起点 GridSlot 是该物品的 UpperLeft（否则会把“物品中间格”当成起点）
// 该物品可堆叠
//  ItemType 匹配
//  未达到 MaxStackSize
bool UInv_InventoryGrid::CheckSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot,
														  const TSet<int32>& CheckedIndices,
														  TSet<int32>& ClaimedIndices, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	//检查其他情况
	
	//如果不是1*1，格子被占用了吗
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetIndex())) return false;

	//有合法物品吗
	if (!HasValidItem(SubGridSlot))
	{
		ClaimedIndices.Add(SubGridSlot->GetIndex());
		return true;
	}
	//当前Slot是左上角吗
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;

	//是尝试添加到相同物品吗,可堆叠吗
	const UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;

	//有空间吗
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;

	return true;
}


bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}


bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const 
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetIndex();
}


bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem,const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}


bool UInv_InventoryGrid::IsWithinBounds(const int32 Index, const FIntPoint& Dimensions) const
{
	if (Index < 0 || Index >= GridSlots.Num()) return false;
	const int32 EndColumn = (Index % Columns) + Dimensions.X;
	const int32 EndRow = (Index / Columns) + Dimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::HowManySlotsToFill(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	//计算需要填充的数量
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);

	//如果可堆叠，取需要填充数量和剩余空间的最小值
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	//如果当前格子没有堆叠数量，则需要实际堆叠数量，通常用于非单格物品
	if (const int32 UpperLeft = GridSlot->GetUpperLeftIndex(); UpperLeft != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftSlot = GridSlots.IsValidIndex(UpperLeft) ? GridSlots[UpperLeft] : nullptr;
		CurrentSlotStackCount = UpperLeftSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

bool UInv_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;

	const FVector2D DrawSize = GetDrawSize(GridFragment);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(Brush);
	HoverItem->SetGridDimisions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::OnHide()
{
	PutHoverItemBack();
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;

	UInv_InventoryStatic::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UInv_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(nullptr);
			GridSlot->SetUpperLeftIndex(INDEX_NONE);
			GridSlot->SetUnoccupiedTexture();
			GridSlot->SetAvailable(true);
			GridSlot->SetStackCount(0);
		});

	if (SlottedItemsMap.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItemsMap.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::AddStack (const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;

	for(const auto& Availability : Result.SlotAvailabilities)
	{
		// 情况 1：该 Index 上已经存在合法物品（可堆叠）
		if (Availability.bItemAtIndex)
		{
			// 当前 GridSlot（用于获取已有堆叠数量）
			const auto& GridSlot = GridSlots[Availability.Index];
			// 已存在的 SlottedItem Widget（用于更新 UI 上的堆叠显示）
			const auto& SlottedItem = SlottedItemsMap.FindChecked(Availability.Index);
			// 更新堆叠数量：
			// 新数量 = 当前格子已有堆叠数 + 本次需要填充的数量
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.RoomToFill);
		}
		// 情况 2：该 Index 为空，需要创建新的物品实例并放入格子
		else
		{
			// 在指定 Index 创建新的 SlottedItem（UI 层物品）并同步更新 GridSlots 数据状态：
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.RoomToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.RoomToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClicked(const int32 GridIndex, const FPointerEvent& MouseEvent)
{
	UInv_InventoryStatic::ItemUnhovered(GetOwningPlayer());

	check(GridSlots.IsValidIndex(GridIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem)&&IsLeftClick(MouseEvent))
	{
		//创建悬停物品图标, 删除原有物品图标
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	if (IsRightClick(MouseEvent))
	{
		CreatePopUpMenu(GridIndex);
		return;
	}

	//如果这时候有悬停的物品，判断与当前物品的关系，是否相同，是否可叠加，是否可替换
	if (IsSameStackableItem(ClickedInventoryItem))
	{
		//需要交换堆叠数量吗，例如手上物品到达最大堆叠数，Grid里的物品数量为X，点击后则手上为X，Grid为最大堆叠数
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		const FInv_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FInv_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 Room = MaxStackSize - ClickedStackCount;

		if (SwapCheckStackCount(Room, HoveredStackCount, MaxStackSize))
		{
			SwapStackCount(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}

		//这俩一样的话，二合一吗(剩余空间>悬浮的数量
		if (Room >= HoveredStackCount)
		{
			MergeItemsStackWithHover(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		//如果不二合一，需要填满其中一个吗
		if (Room < HoveredStackCount)
		{
			FillinStack(Room, HoveredStackCount - Room, GridIndex);
			return;
		}

		//被点击的格子堆叠是否已满
		if (Room == 0)return;
	}
			
	//保证大小不同的物品不会重叠
	if (CurrentHoverResult.ValidItem.IsValid())
	{
		//与悬停物品交换
		SwapHoverItem(ClickedInventoryItem, GridIndex);
	}
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	UE_LOG(LogTemp, Warning, TEXT("InventoryGrid::AddItem"));
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item);

	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	for(const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.RoomToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable,Availability.RoomToFill);
	}
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* NewItem, const int32 Index, const bool bStackable, const int32 StackAmount)
{
		//获取Fragment,其中包含格子大小和icon
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
		const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(NewItem, FragmentTags::IconFragment);
		if (!GridFragment || !ImageFragment) return;

		//创建SlottedItem
		UInv_SlottedItem* SlottedItem = CreateSlottedItem(NewItem, bStackable, StackAmount, GridFragment, ImageFragment, Index);
		UE_LOG(LogTemp, Warning, TEXT("InventoryGrid::AddItemToIndices Index:%d Room:%d"), Index, StackAmount);
		
		//创建widget填入格子
		AddSlottedItemToCanvas(SlottedItem, GridFragment, Index);

		//Map存新widget,用于后续移动删除等操作
		SlottedItemsMap.Add(Index, SlottedItem);



}


UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment, const int32 Index)
{
		UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
		SlottedItem->SetInventoryItem(Item);
		SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
		SlottedItem->SetGridIndex(Index);
		SlottedItem->SetIsStackable(bStackable);
		const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
		SlottedItem->UpdateStackCount(StackUpdateAmount);
		SlottedItem->OnSlottedItemClicked.AddDynamic(this, &UInv_InventoryGrid::OnSlottedItemClicked);

		return SlottedItem;
}


void UInv_InventoryGrid::AddSlottedItemToCanvas(UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment, const int32 Index) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));

	//计算位置
	const FVector2D DrawPos = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D PaddingOffset = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(PaddingOffset);
}


void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	if (!NewItem)return;
	check(GridSlots.IsValidIndex(Index));

	// 堆叠物品：在“左上角格子”记录堆叠数量（其余被占用格子通常不需要重复记录）
	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	//获取物品尺寸（默认 1x1）
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	// 将覆盖区域内所有格子标记为占用，并回填 UpperLeftIndex，便于后续：
	// - 堆叠判断定位到真实物品
	// - 拖拽移动/删除时，能快速从任意子格追溯到左上角
	UInv_InventoryStatic::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetAvailable(false);
		});
}


bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const
{
	return CheckIndices.Contains(Index);
}

FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

// 为 SlottedItem 构建并设置图标 Brush
void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, 
																			const FInv_GridFragment* GridFragment,
																			const FInv_ImageFragment* ImageFragment) const
{
	if (!SlottedItem || !GridFragment || !ImageFragment) return;

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);

	SlottedItem->SetImageBrush(Brush);
}

void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			if (GridSlotClass)
			{
				UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
				CanvasPanel->AddChild(GridSlot);

				//设置索引和位置
				const FIntPoint TilePosition(i, j);
				GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

				//设置大小和位置
				UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
				CanvasSlot->SetSize(FVector2D(TileSize));
				CanvasSlot->SetPosition(TilePosition * TileSize);

				GridSlots.Add(GridSlot);
				GridSlot->OnGridSlotClicked.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotClicked);
				GridSlot->OnGridSlotHovered.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotHovered);
				GridSlot->OnGridSlotUnhovered.AddDynamic(this, &UInv_InventoryGrid::OnGridSlotUnhovered);
			}
		}
	}
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}



void UInv_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;

	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	//显示鼠标指针
	ShowCursor();


}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer()))return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer()))return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

bool UInv_InventoryGrid::IsSameStackableItem(const UInv_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsStackable && bIsSameItem && HoverItem->GetItemTypeTag().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UInv_InventoryGrid::SwapHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem))return;

	UInv_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->IsStackable();

	// 保持相同索引
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UInv_InventoryGrid::SwapCheckStackCount(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UInv_InventoryGrid::SwapStackCount(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);

	UInv_SlottedItem* ClickedSlottedItem = SlottedItemsMap.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

void UInv_InventoryGrid::MergeItemsStackWithHover(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	const int32 NewCount = ClickedStackCount + HoveredStackCount;

	GridSlots[Index]->SetStackCount(NewCount);
	SlottedItemsMap.FindChecked(Index)->UpdateStackCount(NewCount);
	ClearHoverItem();
	ShowCursor();

	const FInv_GridFragment* GridFragent = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType < FInv_GridFragment>();
	const FIntPoint Dimension = GridFragent ? GridFragent->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimension);
}

void UInv_InventoryGrid::FillinStack(const int32 Fill, const int32 Remainder, const int32 index)
{
	UInv_GridSlot* GridSlot = GridSlots[index];
	const int32 NewCount = GridSlot->GetStackCount() + Fill;

	GridSlot->SetStackCount(NewCount);

	UInv_SlottedItem* ClickedSlottedItem = SlottedItemsMap.FindChecked(index);
	ClickedSlottedItem->UpdateStackCount(NewCount);

	HoverItem->UpdateStackCount(Remainder);
}

void UInv_InventoryGrid::CreatePopUpMenu(const int32 GridIndex)
{
	UInv_InventoryItem* Item = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(Item)) return;
	if(IsValid(GridSlots[GridIndex]->GetPopUpMenu())) return;

	if (PopUpMenu) PopUpMenu->RemoveFromParent();

	PopUpMenu = CreateWidget<UInv_ItemPopup>(this, PopUpMenuClass);
	GridSlots[GridIndex]->SetPopUpMenu(PopUpMenu);

	OwningCanvasPanel->AddChild(PopUpMenu);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(PopUpMenu);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePos);
	CanvasSlot->SetSize(PopUpMenu->GetBoxSize());
	
	if (Item->IsConsumable())
	{
		PopUpMenu->OnConsume.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuConsume);
	}
	else
	{
		PopUpMenu->CollapseButtonConsume();
	}

	const int32 SliderMaxVal = GridSlots[GridIndex]->GetStackCount() - 1;
	if (Item->IsStackable() && SliderMaxVal > 0)
	{
		PopUpMenu->OnSplit.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuSplit);
		PopUpMenu->SetSliderValue(SliderMaxVal, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		PopUpMenu->CollapseButtonSplit();
	}

	PopUpMenu->OnDrop.BindDynamic(this, &UInv_InventoryGrid::OnPopUpMenuDrop);

	
}

void UInv_InventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem))return;

	FInv_SlotAvailabilityResult Res = HasRoomForItem(HoverItem->GetInventoryItem(),HoverItem->GetStackCount());
	Res.Item = HoverItem->GetInventoryItem();

	AddStack(Res);
	ClearHoverItem();
}

void UInv_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem))return;
	if (!IsValid(HoverItem->GetInventoryItem()))return;

	//告知服务器
	InventoryComp->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowCursor();
}

bool UInv_InventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

UInv_HoverItem* UInv_InventoryGrid::GetHoverItem() const
{
	return HoverItem;
}

void UInv_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
	
}

void UInv_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UInv_InventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}

void UInv_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	//点击了一个存了物品的格子
	if (CurrentHoverResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentHoverResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentHoverResult.UpperLeftIndex, MouseEvent);
		return;
	}

	if (!IsWithinBounds(ItemDropIndex, HoverItem->GetGridDimisions()))return;

	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		//放置
		PutDownOnIndex(ItemDropIndex);
	}

}

void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInv_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

void UInv_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	UInv_InventoryItem* Item = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(Item)) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftSlot = GridSlots[UpperLeftIndex];
	const int32 NewStackCount = UpperLeftSlot->GetStackCount() - 1;

	UpperLeftSlot->SetStackCount(NewStackCount);
	SlottedItemsMap.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	//通知服务器消耗了一个
	InventoryComp->Server_ConsumeItem(Item);

	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(Item, UpperLeftIndex);
	}
} 

void UInv_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmout, int32 Index)
{
	UInv_InventoryItem* Item = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(Item))return;
	if (!Item->IsStackable())return;

	const int32 UpperLeft = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftSlot = GridSlots[UpperLeft];
	const int32 StackCount = UpperLeftSlot->GetStackCount();
	const int32 NewCount = StackCount - SplitAmout;

	UpperLeftSlot->SetStackCount(NewCount);
	SlottedItemsMap.FindChecked(UpperLeft)->UpdateStackCount(NewCount);

	AssignHoverItem(Item, UpperLeft, UpperLeft);
	HoverItem->UpdateStackCount(SplitAmout);
}

void UInv_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem* Item = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(Item))return;

	PickUp(Item, Index);
	DropItem();
}

void UInv_InventoryGrid::OnInventoryMenuToggled(bool bOpen)
{
	if (!bOpen)
	{
		PutHoverItemBack();
	}
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	 return Item->GetItemManifest().GetItemCategory() == ItemCategory; 
}
