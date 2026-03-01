local UIManager = UnLua.Class()

-- WidgetShortName -> ControllerModule
-- shortName 取自路径最后段：BP_UI_OutfitMenu
local ControllerMap = {
    ["BP_UI_OutfitMenu"] = "Plugin.LuaFramework.UI.Hall.OutfitMenuController",
}

local function GetShortName(widgetPath)
    local p = tostring(widgetPath or "")
    if p == "" then return nil end
    local inside = p:match("'([^']+)'")
    if inside then p = inside end
    local name = p:match("/([^/]+)%.") or p:match("/([^/]+)$")
    if name then name = name:gsub("_C$", "") end
    return name
end

local function CreateController(CtrlClass)
    if type(CtrlClass) == "function" then
        return CtrlClass() -- UnLua.Class()
    end
    if type(CtrlClass) == "table" and type(CtrlClass.New) == "function" then
        return CtrlClass:New()
    end
    return nil
end

function UIManager:LuaInit()
    print("[UIManager] LuaInit")
    self._stack = {}        -- entries: {path,args,z,widget,controller}
    self._openByPath = {}   -- path -> entry (for unique / fast close)
end

function UIManager:LuaDeinit()
    print("[UIManager] LuaDeinit")
    self:CloseAll()
end

-- bUnique: if true, same widgetPath will not be opened twice (bring-to-front instead)
function UIManager:Open(widgetPath, args, zOrder, bUnique)
    local ws = _G.WorldService
    if not (ws and ws._world) then
        print("[UIManager] Open failed: WorldService/world nil")
        return nil
    end

    local pc = UE.UGameplayStatics.GetPlayerController(ws._world, 0)
    if not pc then
        print("[UIManager] Open failed: PlayerController nil")
        return nil
    end

    local z = zOrder or 0

    -- Unique: already opened -> bring to front
    if bUnique and self._openByPath[widgetPath] then
        local e = self._openByPath[widgetPath]
        if e.widget then
            e.z = z
            -- simple bring-to-front: re-add to viewport
            e.widget:RemoveFromParent()
            e.widget:AddToViewport(z)
        end
        return e.widget
    end

    local widget = ws:CreateWidgetByPath(widgetPath, pc, z, true)
    if not widget then
        print("[UIManager] Open failed: CreateWidgetByPath nil, path=" .. tostring(widgetPath))
        return nil
    end

    local entry = { path = widgetPath, args = args, z = z, widget = widget, controller = nil }

    -- 如果 Widget 脚本自己实现了 OnUIOpen，就在这里注入参数（可选）
    if widget and widget.OnUIOpen then
        local ok, err = xpcall(function()
            widget:OnUIOpen(args, self)
        end, debug.traceback)
        if not ok then
            print("[UIManager] widget:OnUIOpen failed: " .. tostring(err))
        end
    end

    table.insert(self._stack, entry)
    self._openByPath[widgetPath] = entry
    return widget
end

function UIManager:IsOpen(widgetPath)
    return self._openByPath[widgetPath] ~= nil
end

function UIManager:GetTop()
    local entry = self._stack[#self._stack]
    return entry and entry.widget or nil
end

function UIManager:CloseTop()
    local entry = self._stack[#self._stack]
    if not entry then return end
    self:_CloseEntry(entry)
    self._stack[#self._stack] = nil
end

function UIManager:Close(widgetPath)
    local entry = self._openByPath[widgetPath]
    if not entry then return end

    for i = #self._stack, 1, -1 do
        if self._stack[i] == entry then
            self:_CloseEntry(entry)
            table.remove(self._stack, i)
            break
        end
    end
end

function UIManager:CloseAll()
    for i = #self._stack, 1, -1 do
        self:_CloseEntry(self._stack[i])
        self._stack[i] = nil
    end
end

function UIManager:_CloseEntry(entry)
    if entry and entry.path then
        self._openByPath[entry.path] = nil
    end

    if entry.widget and entry.widget.OnUIClose then
        local ok, err = xpcall(function()
            entry.widget:OnUIClose()
        end, debug.traceback)
        if not ok then
            print("[UIManager] widget:OnUIClose failed: " .. tostring(err))
        end
    end

    if entry.widget then
        entry.widget:RemoveFromParent()
    end
end

-- HotReload: close all old widgets (old closures), then rebuild from snapshot
function UIManager:OnHotReloaded(reason)
    print("[UIManager] OnHotReloaded reason=" .. tostring(reason))

    local snapshot = {}
    for i, e in ipairs(self._stack) do
        snapshot[i] = { path = e.path, args = e.args, z = e.z }
    end

    self:CloseAll()

    for _, s in ipairs(snapshot) do
        self:Open(s.path, s.args, s.z, true)
    end
end

return UIManager