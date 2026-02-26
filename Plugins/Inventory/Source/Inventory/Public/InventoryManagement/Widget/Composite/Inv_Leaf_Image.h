#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Math/Vector2D.h"
#include "Inv_Leaf_Image.generated.h"

class USizeBox;
class UImage;


UCLASS()
class INVENTORY_API UInv_Leaf_Image : public UInv_Leaf
{
	GENERATED_BODY()

public:

	void SetImage(UTexture2D* Texture);
	void SetBoxSize(const FVector2D& Size);
	void SetImageSize(const FVector2D& Size);
	FVector2D GetImageSize();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Icon;
};
