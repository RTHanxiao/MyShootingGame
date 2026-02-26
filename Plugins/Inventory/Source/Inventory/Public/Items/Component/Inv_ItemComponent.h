// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORY_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInv_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitItemManifest(FInv_ItemManifest ManifestCopy);
	FInv_ItemManifest GetItemManifest() const { return ItemManifest; }
	FString GetPickUpMessage() const { return PickUpMessage; }
	void PickedUp();

protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickUp();

private:
	UPROPERTY()
	FString PickUpMessage;
		
	UPROPERTY(Replicated, EditAnywhere,Category = "Inventory")
	FInv_ItemManifest ItemManifest;
	
};
