// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/Utils/Inv_WidgetUtils.h"
#include "Components/Widget.h"
#include "../../../../../../../Source/Runtime/SlateCore/Public/Layout/Geometry.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/SlateBlueprintLibrary.h"


FVector2D UInv_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D PixelPos;
	FVector2D ViewPortPos;

	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPos, ViewPortPos);
	return ViewPortPos;
}


FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

bool UInv_WidgetUtils::IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize, const FVector2D& MosuePos)
{
	return MosuePos.X >= BoundaryPos.X &&
		MosuePos.X <= BoundaryPos.X + WidgetSize.X &&
		MosuePos.Y >= BoundaryPos.Y &&
		MosuePos.Y <= BoundaryPos.Y + WidgetSize.Y;
}

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.Y * Columns + Position.X;
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize, const FVector2D& MousePos)
{
	FVector2D ClampedPos = MousePos;

	//通过调整保证UI不过界
	// 右边界
	if (MousePos.X + WidgetSize.X > Boundary.X)
	{
		ClampedPos.X = Boundary.X - WidgetSize.X;
	}

	// 左边界
	if (ClampedPos.X < 0.f)
	{
		ClampedPos.X = 0.f;
	}

	// 下边界
	if (MousePos.Y + WidgetSize.Y > Boundary.Y)
	{
		ClampedPos.Y = Boundary.Y - WidgetSize.Y;
	}

	// 上边界
	if (ClampedPos.Y < 0.f)
	{
		ClampedPos.Y = 0.f;
	}

	return ClampedPos;
}


