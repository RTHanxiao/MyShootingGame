// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Component/Inv_ItemComponent.h"


UInv_ItemComponent::UInv_ItemComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

void UInv_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}


void UInv_ItemComponent::InitItemManifest(FInv_ItemManifest ManifestCopy)
{
	ItemManifest = ManifestCopy; 
}

void UInv_ItemComponent::PickedUp()
{
	OnPickUp();
	GetOwner()->Destroy();
}


