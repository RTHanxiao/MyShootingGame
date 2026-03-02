

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inv_EquipmentComponent.generated.h"

class UInv_InventoryComponent;
class UInv_InventoryItem;
class USkeletalMeshComponent;
struct FInv_EquipmentFragment;
struct FInv_ItemManifest;
struct FGameplayTag;
class AInv_EquipActor;

UENUM(BlueprintType)
enum class EInv_EquipSlot : uint8
{
	Primary,
	Secondary
};

USTRUCT()
struct FInv_EquipSlotState
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UInv_InventoryItem> Item;

	UPROPERTY()
	TWeakObjectPtr<AInv_EquipActor> Actor;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_EquipmentComponent();

	void SetOwningSkeletal(USkeletalMeshComponent* Mesh);
	void SetIsPreview(bool bPreview) { bIsPreview = bPreview; }
	void InitializeOwner(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	AInv_EquipActor* GetActiveWeaponActor() const;
	UInv_InventoryItem* GetActiveWeaponItem() const;
	EInv_EquipSlot GetActiveSlot() { return ActiveSlot; }
	void SetActiveSlot(EInv_EquipSlot NewSlot);
protected:

	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComp;
	TWeakObjectPtr<APlayerController> OwningPC;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletal;

	bool bIsPreview{ false };

	UPROPERTY()
	FInv_EquipSlotState PrimarySlot;

	UPROPERTY()
	FInv_EquipSlotState SecondarySlot;

	UPROPERTY()
	EInv_EquipSlot ActiveSlot = EInv_EquipSlot::Primary;

	UFUNCTION()
	void OnItemEquipped(UInv_InventoryItem* EquippedItem);

	UFUNCTION()
	void OnItemUnequipped(UInv_InventoryItem* UnequippedItem);

	void InitPlayerController();
	void InitInventoryComponent();
	AInv_EquipActor* SpawnEquippedActor(EInv_EquipSlot Slot, FInv_EquipmentFragment* EquipmentFragment,
		UInv_InventoryItem* ItemInstance, USkeletalMeshComponent* AttachMesh);

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	TObjectPtr<UInv_InventoryItem> ActiveWeaponItem;

	UFUNCTION()
	void OnRep_ActiveWeapon();

	UPROPERTY()
	TArray<TObjectPtr<AInv_EquipActor>> EquippedActors;

	AInv_EquipActor* FindEquipedActorByTag(const FGameplayTag& EquipmentTypeTag);
	void RemoveEquippedActor(const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OP, APawn* NP);

	FInv_EquipSlotState& GetSlotStateMutable(EInv_EquipSlot Slot);

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeaponActor)
	TObjectPtr<AInv_EquipActor> ActiveWeaponActor;

	UFUNCTION()
	void OnRep_ActiveWeaponActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
