// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "MSG_PlayerController.generated.h"

class UInputMappingContext;

UENUM(BlueprintType)
enum class EScreenRotationDirection : uint8
{
	ESRD_None      UMETA(DisplayName = "None"),
	ESRD_Left      UMETA(DisplayName = "Left"),
	ESRD_Right     UMETA(DisplayName = "Right"),
	ESRD_Up        UMETA(DisplayName = "Up"),
	ESRD_Down      UMETA(DisplayName = "Down"),
	ESRD_RightUp   UMETA(DisplayName = "RightUp"),
	ESRD_RightDown UMETA(DisplayName = "RightDown"),
	ESRD_LeftUp    UMETA(DisplayName = "LeftUp"),
	ESRD_LeftDown  UMETA(DisplayName = "LeftDown"),
	ESRD_Max       UMETA(Hidden)
};

/**
 * ͳһҿ
 * - ģʽʾ꣬UIOnly 룬ŲĻԵʱתӽ
 * - սģʽϷΪͼ/չ
 */
UCLASS()
class MYSHOOTINGGAME_API AMSG_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMSG_PlayerController();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UInv_EquipmentComponent* GetEquipmentComponent() { return FindComponentByClass<UInv_EquipmentComponent>(); }

	UFUNCTION(BlueprintCallable, Category = "MSG|Mode")
	void SetHallControlMode(bool bEnableHallMode);

	UPROPERTY(EditAnywhere, Category = "MSG|UI")
	TSubclassOf<UUserWidget> StartupWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> StartupWidget;

	UFUNCTION(Client, Reliable)
	void Client_ShowStartupUI();

	void ShowStartupUI_Local();
	void HideStartupUI_Local();

	// MappingContext BP ָ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MSG|Input")
	UInputMappingContext* FightMappingContext;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	TWeakObjectPtr<UInv_InventoryComponent>GetInventoryComponent() const
	{
		// 如果已有缓存且有效，直接返回
		if (InventoryComponent.IsValid())
		{
			return InventoryComponent;
		}

		// 否则从自身组件里找（PC蓝图挂载）
		UInv_InventoryComponent* Found = FindComponentByClass<UInv_InventoryComponent>();
		return Found;
	}

	// =========================
	// Client 
	// =========================

	UFUNCTION(Client, Reliable)
	void Client_ApplyHallMode(APawn* InPawn, TSubclassOf<AHUD> InHUDClass);

	UFUNCTION(Client, Reliable)
	void Client_ApplyFightMode(APawn* InPawn, TSubclassOf<AHUD> InHUDClass);

	UFUNCTION(Exec)
	void HotReloadLua();

	UFUNCTION(Server, Reliable)
	void Server_StartGameTravel(FName MapName);

	UFUNCTION(Server, Reliable)
	void Server_TryAddItem(UInv_ItemComponent* ItemComp);

	UFUNCTION(Server, Reliable)
	void Server_TryPickupActor(AActor* ItemActor);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MSG|Hall")
	bool bEnableHallMode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MSG|Hall", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CursorThreshold;

	EScreenRotationDirection ScreenRotationDirection();
	bool RotateView(EScreenRotationDirection Dir, const float& Speed = 0.1f);

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
};
