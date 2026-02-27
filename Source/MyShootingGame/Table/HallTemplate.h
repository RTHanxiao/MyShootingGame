#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HallTemplate.generated.h"

USTRUCT(BlueprintType)
struct FHallTemplate : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HallTemplate")
	class UTexture2D* SlotImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HallTemplate")
	class USkeletalMesh* SlotSkeletalMesh;
};

USTRUCT(BlueprintType)	
struct FMapPreviewTemplate : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapPreviewTemplate")
	class UTexture2D* MapImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapPreviewTemplate")
	FName MapLevelName;
};