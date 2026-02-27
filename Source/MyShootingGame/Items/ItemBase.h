#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UInv_ItemComponent;

UCLASS(Abstract)
class MYSHOOTINGGAME_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

protected:
	virtual void BeginPlay() override;

	// ===== Components =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ItemCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* PickWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInv_ItemComponent* ItemComponent;

	// ===== Interaction Hooks =====
	UFUNCTION(BlueprintImplementableEvent)
	void SetOutLineVisibility(bool bIsVisible);

	// 进入/离开交互范围：默认实现负责显示/隐藏 PickWidget、Outline
	UFUNCTION()
	virtual void OnEnterInteractRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnLeaveInteractRange(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// “拾取”逻辑：子类覆盖实现自己的加背包/加子弹等行为
	UFUNCTION(BlueprintCallable, Category = "Item|Interaction")
	virtual void OnPickedUp(AActor* Picker);

public:
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return ItemMesh; }
};
