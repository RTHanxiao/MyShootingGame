#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UnLuaInterface.h"
#include "LuaGameService.generated.h"

UCLASS()
class LUAFRAMEWORK_API ULuaGameService : public UGameInstanceSubsystem, public IUnLuaInterface
{
	GENERATED_BODY()
public:
	// 对应 Script/Service/GameService.lua
	virtual FString GetModuleName_Implementation() const override
	{
		return TEXT("Plugin.LuaFramework.Service.GameService");
	}

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- 生命周期事件：Lua 覆盖 ----
	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaInit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaPostInit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|Service")
	void LuaDeinit();

	// ---- 热重载入口：C++ 触发 Lua 框架刷新 ----
	UFUNCTION(BlueprintImplementableEvent, Category = "UnLua|HotReload")
	void LuaOnHotReloaded(int32 HotReloadedEnvCount);

	// 给控制台/蓝图调用
	UFUNCTION(Exec)
	void HotReloadLua();
};