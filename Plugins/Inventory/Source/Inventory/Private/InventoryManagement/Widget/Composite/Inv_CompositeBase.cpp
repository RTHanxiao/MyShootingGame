#include "InventoryManagement/Widget/Composite/Inv_CompositeBase.h"



// Add default functionality here for any IInv_CompositeBase functions that are not pure virtual.

void UInv_CompositeBase::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_CompositeBase::Expand()
{
	SetVisibility(ESlateVisibility::Visible);
}
