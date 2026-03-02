#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Items/Inv_InventoryItem.h"

#include "Inv_EquipActor.generated.h"

class UInv_InventoryItem;

UCLASS()
class INVENTORY_API AInv_EquipActor : public AActor
{
	GENERATED_BODY()

public:
	AInv_EquipActor();
	FGameplayTag GetEquipmentType() const { return EquipmentType; }
	void SetEquipmentType(FGameplayTag Type) { EquipmentType = Type; }

	virtual void InitFromItem(UInv_InventoryItem* InItem) { EquippedItem = InItem; }
	virtual void BindItem(UInv_InventoryItem* InItem) {}
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(EditAnywhere, Replicated, Category = "Inventory")
	FGameplayTag EquipmentType;

	UPROPERTY()
	TWeakObjectPtr<UInv_InventoryItem> EquippedItem;
};
