#include "InventoryManagement/Widget/Composite/Inv_Leaf_LabeledValue.h"
#include "Components/TextBlock.h"


void UInv_Leaf_LabeledValue::SetTextLabel(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Label->SetText(Text);
}

void UInv_Leaf_LabeledValue::SetTextValue(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Value->SetText(Text);
}

void UInv_Leaf_LabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo LabelFont = Text_Label->GetFont();
	LabelFont.Size = FontSize_Label;
	Text_Label->SetFont(LabelFont);

	FSlateFontInfo ValueFont = Text_Value->GetFont();
	ValueFont.Size = FontSize_Value;
	Text_Value->SetFont(ValueFont);

}
