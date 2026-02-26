

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_LabeledValue.generated.h"

class UTextBlock;
class FText;

UCLASS(  )
class INVENTORY_API UInv_Leaf_LabeledValue : public UInv_Leaf
{
	GENERATED_BODY()

public:	
	
	void SetTextLabel(const FText& Text,bool bCollapse) const;
	void SetTextValue(const FText& Text, bool bCollapse) const;
	virtual void NativePreConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Label;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Value;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Label{ 12 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Value{ 15 };
	
};
