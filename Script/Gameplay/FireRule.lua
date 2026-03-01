-- Content/Script/Gameplay/FireRule.lua
local M = UnLua.Class()
local Log = require("Utility.Log")
Log.I("[FireRule] Lua loaded")

-- 全局函数：给 C++ UnLua::Call 直接调用
function M:EvaluateFireRule(ctx)
    -- 默认：不覆盖
    local r = {
        bCanFire = true,
        FireIntervalOverride = -1.0,
        SpreadIncrementOverride = -1.0,
    }

    -- 1) 冲刺/换弹：禁开火（你也可以改成只禁 ADS）
    if ctx.bIsSprinting or ctx.bIsReloading then
        r.bCanFire = false
        return r
    end

    -- 2) 瞄准：更稳（扩散更小），射速略慢（可选）
    if ctx.bIsAiming then
        r.FireIntervalOverride = math.max(0.01, ctx.WeaponAttackRate * 1.10)
        r.SpreadIncrementOverride = math.max(0.0, ctx.CrossHairSpreadIncrement * 0.55)
        return r
    end

    -- 3) 非瞄准：跑射更飘（如果你想要移动影响，后面再加 bIsMoving）
    -- 现在先保持不覆盖
    
    Log.I(string.format("[FireRule] aiming=%s sprint=%s reload=%s",
    tostring(ctx.bIsAiming), tostring(ctx.bIsSprinting), tostring(ctx.bIsReloading)))

    return r
end

return M