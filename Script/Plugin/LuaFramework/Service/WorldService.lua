local WorldService = UnLua.Class()
local Log = require("Utility.Log")
local M = require("Plugin.LuaFramework.Service.Module")
local HotReload = require(M.ServiceRoot .. "HotReload")

-- World 级服务（随 World 创建/销毁）
local ServicesList = {
    -- 例如：WorldUI = "Service.WorldUIManager"
}

function WorldService:LuaInit()
    Log.I("[WorldService] LuaInit")
    self._services = {}
    self._world = nil

    for key, mod in pairs(ServicesList) do
        local svc = require(mod)
        if svc and svc.LuaInit then
            svc:LuaInit()
            self._services[key] = svc
        end
    end

    _G.WorldService = self
end

function WorldService:LuaPostInit()
    Log.I("[WorldService] LuaPostInit")
    for _, svc in pairs(self._services) do
        if svc.LuaPostInit then svc:LuaPostInit() end
    end
end

function WorldService:LuaOnWorldBeginPlay(InWorld)
    self._world = InWorld
    Log.I("[WorldService] LuaOnWorldBeginPlay " .. tostring(InWorld))

    if _G.UIManager and _G.UIManager.Open then
        _G.UIManager:Open("/Script/UMGEditor.WidgetBlueprint'/Game/_Game/BP/UI/HallUI/BP_UI_OutfitMenu.BP_UI_OutfitMenu'", nil, 10)
    else
        Log.I("[WorldService] UIManager not ready; skip opening UI")
    end
end

function WorldService:LuaDeinit()
    Log.I("[WorldService] LuaDeinit")
    for _, svc in pairs(self._services) do
        if svc.LuaDeinit then svc:LuaDeinit() end
    end
end

-- 由 C++: ULuaWorldService::HotReloadLua() 触发
function WorldService:LuaOnHotReloaded(envCount)
    Log.I(("[WorldService] 11111 LuaOnHotReloaded envCount=%s"):format(tostring(envCount)))

    HotReload:ReloadAll()

    for _, svc in pairs(self._services) do
        if svc.OnHotReloaded then svc:OnHotReloaded("C++HotReload") end
    end
end

return WorldService