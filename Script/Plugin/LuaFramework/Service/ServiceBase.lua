local M = UnLua.Class()

function M:LuaInit() end
function M:LuaPostInit() end
function M:LuaDeinit() end

-- 热重载后回调（由 WorldService/GameService 调用）
function M:OnHotReloaded(reason) end

return M