


#include "InventoryManagement/Widget/Inv_InfoMessage.h"
#include "Components/TextBlock.h"

void UInv_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MessageText->SetText(FText::GetEmpty());
	HideMessage();
}

void UInv_InfoMessage::SetMessage(const FText& Message)
{
	MessageText->SetText(Message);
	UE_LOG(LogTemp, Verbose,TEXT("SetMessage Called"));
	if(bIsMessageActive)
	{
		ShowMessage();
	}
	bIsMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer, [this]()
		{
			HideMessage();
			bIsMessageActive = false;
		}, MessageLifetime, false);
}
