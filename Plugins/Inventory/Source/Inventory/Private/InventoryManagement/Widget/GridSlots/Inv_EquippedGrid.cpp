#include "InventoryManagement/Widget/GridSlots/Inv_EquippedGrid.h"
#include "InventoryManagement/Widget/HoverItem/Inv_HoverItem.h"
#include "InventoryManagement/Utils/Inv_InventoryStatic.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Inv_InventoryItem.h"
#include "InventoryManagement/Widget/SlottedItems/Inv_EquipSlottedItem.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Overlay.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/OverlaySlot.h"



void UInv_EquippedGrid::NativeOnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatic::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))return;

	if (HoverItem->GetItemTypeTag().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
	}
}

void UInv_EquippedGrid::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatic::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))return;

	if (IsValid(EquipSlottedItem))return; 

	if (HoverItem->GetItemTypeTag().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
	}
}

FReply UInv_EquippedGrid::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	OnEquippedGridClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

UInv_EquipSlottedItem* UInv_EquippedGrid::OnItemEquiped(UInv_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize)
{
	//检查装备Tag
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;

	//设置大小
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	if (!GridFragment) return nullptr;

	const FIntPoint Dimension = GridFragment->GetGridSize();

	const float IconWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = Dimension * IconWidth;

	//创建控件
	EquipSlottedItem = CreateWidget<UInv_EquipSlottedItem>(GetOwningPlayer(), EquippSlotItemClass);

	//设置SlottedItem的相关参数
	EquipSlottedItem->SetInventoryItem(Item);
	EquipSlottedItem->SetEquipmentTypeTag(EquipmentTag);

	//根据是否可堆叠这是显隐
	EquipSlottedItem->UpdateStackCount(0);

	//设置当前类的Item
	SetInventoryItem(Item);

	//设置Brush
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!ImageFragment)return nullptr;

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;

	EquipSlottedItem->SetImageBrush(Brush);

	//向Overlay添加子控件
	Overlay_Root->AddChildToOverlay(EquipSlottedItem);
	FGeometry OverlayGeo = Overlay_Root->GetCachedGeometry();
	FVector2f OverlayPos = OverlayGeo.Position;
	FDeprecateSlateVector2D OverlaySize = OverlayGeo.Size;

	const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
	const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquipSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));

	//返回控件
	return EquipSlottedItem;
}
