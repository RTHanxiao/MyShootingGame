// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "MSG_PlayerController.generated.h"

class UInputMappingContext;
class UInv_InventoryComponent;

/** 屏幕八方向，用于大厅视角旋转 */
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
 * 统一的玩家控制器：
 * - 大厅模式：显示鼠标，UIOnly 输入，鼠标挪到屏幕边缘时旋转视角
 * - 战斗模式：游戏输入为主（可在蓝图/子类里继续扩展）
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

	/** 在 C++ 或蓝图里切换是否使用“大厅控制模式” */
	UFUNCTION(BlueprintCallable, Category = "MSG|Mode")
	void SetHallControlMode(bool bEnableHallMode);

	// 战斗用 MappingContext（在 BP 中指定）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MSG|Input")
	UInputMappingContext* FightMappingContext;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	TWeakObjectPtr<UInv_InventoryComponent> GetInventoryComponent() const { return InventoryComponent; }

	// =========================
	// Client 执行：大厅/战斗切换
	// =========================

	UFUNCTION(Client, Reliable)
	void Client_ApplyHallMode(TSubclassOf<APawn> InPawnClass, TSubclassOf<AHUD> InHUDClass);

	UFUNCTION(Client, Reliable)
	void Client_ApplyFightMode(TSubclassOf<APawn> InPawnClass, TSubclassOf<AHUD> InHUDClass);

protected:
	/** 是否启用大厅模式（鼠标边缘旋转视角 + UI Only 输入） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MSG|Hall")
	bool bEnableHallMode;

	/** 光标位置阈值（越小越敏感） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MSG|Hall", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CursorThreshold;

	EScreenRotationDirection ScreenRotationDirection();
	bool RotateView(EScreenRotationDirection Dir, const float& Speed = 0.1f);

	// 背包指针
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
};
