

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_InfoMessage.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InfoMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void ShowMessage();

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void HideMessage();

	void SetMessage(const FText& Message);


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;

	float MessageLifetime{ 3.f };

	FTimerHandle MessageTimer;
	bool bIsMessageActive{ false };
};
