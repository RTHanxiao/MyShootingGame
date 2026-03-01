#include "Service/HotReload.h"
#include "UnLua.h"          // UnLua::FLuaEnv
#include "LuaEnv.h"         // UnLua::FLuaEnv::GetAll()

int32 ULuaHotReloadLibrary::HotReloadAllLuaEnvs()
{
	int32 Count = 0;
	auto& All = UnLua::FLuaEnv::GetAll();
	for (auto& KV : All)
	{
		UnLua::FLuaEnv* Env = KV.Value;
		if (!Env) continue;
		Env->HotReload();
		++Count;
	}
	return Count;
}