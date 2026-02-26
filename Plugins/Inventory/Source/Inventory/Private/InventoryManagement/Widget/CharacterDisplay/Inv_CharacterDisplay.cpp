#include "InventoryManagement/Widget/CharacterDisplay/Inv_CharacterDisplay.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "EquipmentManagement/Actors/Inv_PreviewMesh.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "../../../../../../../Source/Runtime/CoreUObject/Public/UObject/NoExportTypes.h"

FReply UInv_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	CurPos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	LastPos = CurPos;

	bIsDragging = true;
	return FReply::Handled();
}

FReply UInv_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	bIsDragging = false;
	return Super::NativeOnMouseButtonUp(Geometry, MouseEvent);
}

void UInv_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	bIsDragging = false;
}

void UInv_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<AActor*>Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AInv_PreviewMesh::StaticClass(),Actors);

	if (!Actors.IsValidIndex(0)) return;

	AInv_PreviewMesh* PreviewMesh = Cast<AInv_PreviewMesh>(Actors[0]);
	if (!IsValid(PreviewMesh)) return;

	Mesh = PreviewMesh->GetMesh();
}

void UInv_CharacterDisplay::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry,DeltaTime);
	if (!bIsDragging) return;

	LastPos = CurPos;
	CurPos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	const float HorizontalDelta = LastPos.X - CurPos.X;
	Mesh->AddRelativeRotation(FRotator(.0f, HorizontalDelta, .0f));
}
