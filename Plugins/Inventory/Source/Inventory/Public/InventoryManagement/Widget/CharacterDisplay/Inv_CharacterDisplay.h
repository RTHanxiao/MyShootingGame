#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Inv_CharacterDisplay.generated.h"

struct FPointerEvent;
struct FGeometry;
class USkeletalMeshComponent;


/**
	*
	*/

UCLASS()
class INVENTORY_API UInv_CharacterDisplay : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;

private:

	bool bIsDragging{ false };
	TWeakObjectPtr<USkeletalMeshComponent> Mesh;

	FVector2D CurPos;
	FVector2D LastPos;
};