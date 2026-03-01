#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HotReload.generated.h"

UCLASS()
class LUAFRAMEWORK_API ULuaHotReloadLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 热重载：对所有 UnLua Env 执行 HotReload()
	UFUNCTION(BlueprintCallable, Category = "UnLua|HotReload")
	static int32 HotReloadAllLuaEnvs();
};