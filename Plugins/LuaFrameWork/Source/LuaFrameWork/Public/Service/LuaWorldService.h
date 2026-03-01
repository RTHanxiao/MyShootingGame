#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UnLuaInterface.h"
#include "Blueprint/UserWidget.h"
#include "LuaWorldService.generated.h"

UCLASS()
class LUAFRAMEWORK_API ULuaWorldService : public UWorldSubsystem, public IUnLuaInterface
{
	GENERATED_BODY()
public:
	// 对应 Script/Service/WorldService.lua
	virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Plugin.LuaFramework.Service.WorldService");
	}

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	// ---- 生命周期事件：Lua 覆盖 ----
	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaInit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaPostInit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaOnWorldBeginPlay(UWorld* InWorld);

	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaDeinit();

	// ---- 常用桥接：用路径创建 Widget（可用于 Lua/蓝图） ----
	UFUNCTION(BlueprintCallable, Category = "UnLua|UI")
	UUserWidget* CreateWidgetByPath(const FString& WidgetBPPath, APlayerController* OwningPlayer, int32 ZOrder = 0, bool bAddToViewport = true);

	// ---- 控制台入口，热重载 + Lua 刷新 ----
	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|HotReload")
	void LuaOnHotReloaded(int32 HotReloadedEnvCount);

	UFUNCTION(Exec)
	void HotReloadLua();
};