// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Widget/ItemDescription/Inv_ItemDescription.h"
#include "Components/SizeBox.h"

FVector2D UInv_ItemDescription::GetBoxSize() const
{
	return SizeBox->GetDesiredSize();
	
}
