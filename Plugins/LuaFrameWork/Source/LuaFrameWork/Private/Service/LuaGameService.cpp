#include "Service/LuaGameService.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Service/HotReload.h"

bool ULuaGameService::ShouldCreateSubsystem(UObject* Outer) const
{
	// 只在真实 GameInstance 下创建（PIE/Game 都会有）
	return Outer != nullptr;
}

void ULuaGameService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LuaInit();
	LuaPostInit();
}

void ULuaGameService::Deinitialize()
{
	LuaDeinit();
	Super::Deinitialize();
}

void ULuaGameService::HotReloadLua()
{
	const int32 EnvCount = ULuaHotReloadLibrary::HotReloadAllLuaEnvs();
	UE_LOG(LogTemp, Warning, TEXT("[LuaWorldService] HotReloadLua called"));
	LuaOnHotReloaded(EnvCount);
}