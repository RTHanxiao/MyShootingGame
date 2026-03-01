local Log = {}

-- Output Log
function Log.I(msg)
    print("[Lua] " .. tostring(msg))
end

-- On-screen
function Log.S(msg, time, color)
    time = time or 2.0
    if not GEngine then
        print("[LuaScreenFallback] " .. tostring(msg))
        return
    end

    -- 默认绿色
    color = color or (FColor and FColor(0,255,0) or FLinearColor(0,1,0,1))
    GEngine:AddOnScreenDebugMessage(-1, time, color, tostring(msg))
end

return Log