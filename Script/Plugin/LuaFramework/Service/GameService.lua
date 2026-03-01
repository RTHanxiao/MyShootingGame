local GameService = UnLua.Class()
local Log = require("Utility.Log")
local M = require("Plugin.LuaFramework.Service.Module")
local HotReload = require(M.ServiceRoot .. "HotReload")

-- 全局服务列表（常驻，跨地图）
local ServicesList = {
    UIManager = "Plugin.LuaFramework.Service.UIManager",
}

function GameService:LuaInit()
    Log.I("[GameService] LuaInit")
    self._services = {}

    for key, mod in pairs(ServicesList) do
        local SvcClassOrObj = require(mod)
        local svc = SvcClassOrObj
    
        -- 如果模块返回的是 UnLua.Class()（可调用的“类函数”），这里实例化
        if type(SvcClassOrObj) == "function" then
            svc = SvcClassOrObj()
        end
    
        if svc and svc.LuaInit then
            svc:LuaInit()
        end
    
        self._services[key] = svc
        self[key] = svc     --  允许 GameService.UIManager
        _G[key] = svc       --  允许 _G.UIManager
    end

    _G.GameService = self
end

function GameService:LuaPostInit()
    Log.I("[GameService] LuaPostInit")
    for _, svc in pairs(self._services) do
        if svc.LuaPostInit then svc:LuaPostInit() end
    end
end

function GameService:LuaDeinit()
    Log.I("[GameService] LuaDeinit")
    for _, svc in pairs(self._services) do
        if svc.LuaDeinit then svc:LuaDeinit() end
    end
end

-- 由 C++: ULuaGameService::HotReloadLua() 触发
function GameService:LuaOnHotReloaded(envCount)
    Log.I(("[GameService] LuaOnHotReloaded envCount=%s"):format(tostring(envCount)))

    -- 1) 可选：Lua 侧强制 require 刷新部分模块
    HotReload:ReloadAll()

    -- 2) 通知子服务做重绑定
    for _, svc in pairs(self._services) do
        if svc.OnHotReloaded then svc:OnHotReloaded("C++HotReload") end
    end
end

return GameService