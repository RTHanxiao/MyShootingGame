

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "../../../../../../../Source/Runtime/CoreUObject/Public/StructUtils/InstancedStruct.h"
#include "Inv_ItemFragment.generated.h"

class UTexture2D;
class APlayerController;
class UInv_CompositeBase;
class AInv_EquipActor;

USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	FInv_ItemFragment() = default;

	FInv_ItemFragment(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment(FInv_ItemFragment&& Other) = default;
	FInv_ItemFragment& operator=(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment& operator=(FInv_ItemFragment&& Other) = default;
	virtual ~FInv_ItemFragment() {};

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag& NewTag) { FragmentTag = NewTag; }
	virtual void Manifest() {};
private:

	UPROPERTY(EditAnywhere, Category = "Inventory|Item", meta = (Categories= "FragmentTags"))
	FGameplayTag FragmentTag =  FGameplayTag::EmptyTag;

};

//用于UI获取信息
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UInv_CompositeBase* Composite) const;
protected:
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};

USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& NewSize) { GridSize = NewSize; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float NewPadding) { GridPadding = NewPadding; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{ 1,1 };

	UPROPERTY(EditAnywhere,Category = "Inventory")
	float GridPadding{ 0.f };

};

USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f ,44.f };

};

USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	FText GetText() const { return FragmentText; }
	void SetText(const FText& Text) { FragmentText = Text; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

private:
	UPROPERTY(EditAnywhere,Category = "Inventory")
	FText FragmentText;

};

USTRUCT(BlueprintType)
struct FInv_LabeledNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const;

	float GetValue() { return Value; }

	//初次创建的时候，随机生成.一旦装备或丢弃，物品维持相同数值
	bool bRandomizeOnManifest{ true };

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText{};
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{ .0f };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{ 0 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{0};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{ false };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{ 1 };
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{ 1 };

};

USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 NewCount) { StackCount = FMath::Clamp(NewCount, 1, MaxStackSize); }
	void SetMaxStackSize(int32 NewMax) { MaxStackSize = FMath::Max(1, NewMax); }


private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{ 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{ 1 };
};
//消耗品
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC);
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void Manifest() override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ConsumeModifier>> ConsumeModifiers;
};

USTRUCT(BlueprintType)
struct FInv_ConsumeModifier : public FInv_LabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}
};

USTRUCT(BlueprintType)
struct FInv_HealerFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;
};

//装备
USTRUCT(BlueprintType)
struct FInv_EquipmentFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	bool bEquipped{ false };
	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);
	virtual void Assimilate(UInv_CompositeBase* Composite)const override;
	virtual void Manifest() override;

	AInv_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh)const;
	void DestroyAttachedActor() const;
	FGameplayTag GetEquipmentType()const { return EquipmentType; }
	void SetEquippedActor(AInv_EquipActor* EquipActor);
private:
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_EquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AInv_EquipActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AInv_EquipActor> EquippedActor = nullptr;

	UPROPERTY(EditAnywhere,Category = "Inventory")
	FName SocketAttachPoint{NAME_None};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FInv_EquipModifier : public FInv_LabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) {}
	virtual void OnUnequip(APlayerController* PC) {}


};

USTRUCT(BlueprintType)
struct FInv_DamageModifier : public FInv_EquipModifier
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController * PC) override;

};

