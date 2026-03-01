local HotReload = {}

-- 需要卸载的“模块前缀”列表
-- 你的框架：Plugin.LuaFramework.*
-- 你的 UI 脚本：UI.*
HotReload.UnloadPrefixes = {
    "Plugin.LuaFramework.",
    "UI.",
}

-- 可选：卸载后强制 require 的入口（确保关键模块重新加载）
HotReload.EntryModules = {
    "Plugin.LuaFramework.Service.GameService",
    "Plugin.LuaFramework.Service.WorldService",
}

function HotReload:UnloadByPrefix(prefix)
    local n = 0
    for name, _ in pairs(package.loaded) do
        if type(name) == "string" and name:sub(1, #prefix) == prefix then
            package.loaded[name] = nil
            n = n + 1
        end
    end
    return n
end

function HotReload:ReloadAll()
    local total = 0
    for _, prefix in ipairs(self.UnloadPrefixes) do
        total = total + self:UnloadByPrefix(prefix)
    end
    print(string.format("[HotReload] Unloaded %d modules (prefixes: %s, %s)",
        total, self.UnloadPrefixes[1], self.UnloadPrefixes[2]))

    for _, name in ipairs(self.EntryModules) do
        local ok, err = xpcall(function() require(name) end, debug.traceback)
        if not ok then
            print("[HotReload] require entry failed: " .. tostring(err))
        end
    end
end

return HotReload