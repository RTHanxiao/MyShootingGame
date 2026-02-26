#include "InventoryManagement/Widget/SlottedItems/Inv_EquipSlottedItem.h"



FReply UInv_EquipSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedGridSlotClicked.Broadcast(this);
	return FReply::Handled();
}
