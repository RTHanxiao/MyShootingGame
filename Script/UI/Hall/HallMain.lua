-- Content/Script/UI/GameUI/UI_HallMain.lua
local M = UnLua.Class()
local Log = require("Utility.Log")

local PLAY_FORWARD = 0
local PLAY_REVERSE = 1

-- token：防止旧 Finished 干扰新状态
local function BumpToken(self, key)
    self[key] = (self[key] or 0) + 1
    return self[key]
end

local function PlayAnim(widget, anim, forward)
    if not widget or not anim then return end

    -- 先 Stop，清掉历史播放残留
    if widget.StopAnimation then
        widget:StopAnimation(anim)
    end

    if forward then
        widget:PlayAnimation(anim, 0.0, 1, PLAY_FORWARD, 1.0)
    else
        local t = 0.0
        if anim.GetEndTime then t = anim:GetEndTime() end
        widget:PlayAnimation(anim, t, 1, PLAY_REVERSE, 1.0)
    end
end

function M:Construct()
    Log.I("[HallMain] Construct")

    if self.__animBound then
        Log.I("[HallMain] Construct skipped (already bound)")
        return
    end
    self.__animBound = true

    self:BindAnimFinished(self.MainMenuAnim,   "OnMainMenuAnimFinished")
    self:BindAnimFinished(self.OutfitMenuAnim, "OnOutfitMenuAnimFinished")
    self:BindAnimFinished(self.MapMenuAnim,    "OnMapMenuAnimFinished")
    self:BindAnimFinished(self.ConfigMenuAnim, "OnConfigMenuAnimFinished")

    if self.HallMenu then self.HallMenu.HallMainRef = self end
    if self.OutfitMenu then self.OutfitMenu.HallMainRef = self end
    if self.MapMenu then self.MapMenu.HallMainRef = self end
    if self.ConfigMenu then self.ConfigMenu.HallMainRef = self end

    Log.I(("[HallMain] Widgets HallMenu=%s Outfit=%s Map=%s Config=%s | Anims Main=%s Outfit=%s Map=%s Config=%s"):format(
        tostring(self.HallMenu),
        tostring(self.OutfitMenu),
        tostring(self.MapMenu),
        tostring(self.ConfigMenu),
        tostring(self.MainMenuAnim),
        tostring(self.OutfitMenuAnim),
        tostring(self.MapMenuAnim),
        tostring(self.ConfigMenuAnim)
    ))
end

function M:ForceUIInput(focusWidget)
    local pc = self:GetOwningPlayer()
    if not pc then
        Log.I("[HallMain] ForceUIInput: pc=nil")
        return
    end

    if pc.SetShowMouseCursor then pc:SetShowMouseCursor(true) end
    if pc.bEnableClickEvents ~= nil then pc.bEnableClickEvents = true end
    if pc.bEnableMouseOverEvents ~= nil then pc.bEnableMouseOverEvents = true end

    local KSL = (UE and UE.UKismetSystemLibrary) or (UE5 and UE5.UKismetSystemLibrary) or (UE4 and UE4.UKismetSystemLibrary) or _G.UKismetSystemLibrary
    if KSL then
        local target = focusWidget or self
        if KSL.SetInputMode_GameAndUIEx then
            KSL.SetInputMode_GameAndUIEx(pc, target, 0, false)
        elseif KSL.SetInputMode_UIOnlyEx then
            KSL.SetInputMode_UIOnlyEx(pc, target, 0)
        end
    end

    if focusWidget and focusWidget.SetKeyboardFocus then
        focusWidget:SetKeyboardFocus()
    end

    Log.I(("[HallMain] ForceUIInput done. ShowMouse=%s Click=%s Over=%s focus=%s"):format(
        tostring(pc.bShowMouseCursor),
        tostring(pc.bEnableClickEvents),
        tostring(pc.bEnableMouseOverEvents),
        tostring(focusWidget)
    ))
end

function M:PlayMainMenuAnim(forward)
    Log.I("[HallMain] PlayMainMenuAnim forward=" .. tostring(forward))
    self.bMainMenuForward = forward

    if self.HallMenu then
        if forward then
            self.HallMenu:SetVisibility(0) -- Visible
        else
            if not self.MainMenuAnim then
                self.HallMenu:SetVisibility(1)
                return
            end
        end
    end

    PlayAnim(self, self.MainMenuAnim, forward)
end

function M:PlayOutfitMenuAnim(forward)
    Log.I("[HallMain] PlayOutfitMenuAnim forward=" .. tostring(forward))
    self.bOutfitMenuForward = forward

    -- token：每次播放递增
    self.__OutfitToken = BumpToken(self, "__OutfitToken")
    local token = self.__OutfitToken

    if self.OutfitMenu then
        if forward then
            self.OutfitMenu:SetVisibility(0) -- Visible
        else
            if not self.OutfitMenuAnim then
                self.OutfitMenu:SetVisibility(1)
                return
            end
        end
    end

    PlayAnim(self, self.OutfitMenuAnim, forward)

    -- 记录“本次播放 token”，Finished 只处理这一轮
    self.__OutfitTokenLast = token
end

function M:PlayMapMenuAnim(forward)
    Log.I("[HallMain] PlayMapMenuAnim forward=" .. tostring(forward))
    self.bMapMenuForward = forward

    self.__MapToken = BumpToken(self, "__MapToken")
    local token = self.__MapToken

    if self.MapMenu then
        if forward then
            self.MapMenu:SetVisibility(0)
        else
            if not self.MapMenuAnim then
                self.MapMenu:SetVisibility(1)
                return
            end
        end
    end

    PlayAnim(self, self.MapMenuAnim, forward)

    self.__MapTokenLast = token
end

function M:PlayConfigMenuAnim(forward)
    Log.I("[HallMain] PlayConfigMenuAnim forward=" .. tostring(forward))
    self.bConfigMenuForward = forward

    if self.ConfigMenu then
        if forward then
            self.ConfigMenu:SetVisibility(0)
        else
            if not self.ConfigMenuAnim then
                self.ConfigMenu:SetVisibility(1)
                return
            end
        end
    end

    PlayAnim(self, self.ConfigMenuAnim, forward)
end

function M:OnMainMenuAnimFinished()
    Log.I("[HallMain] OnMainMenuAnimFinished forward=" .. tostring(self.bMainMenuForward))

    if self.bMainMenuForward then
        if self.HallMenu then self.HallMenu:SetVisibility(0) end

        local focus = nil
        if self.HallMenu and self.HallMenu.Btn_Single then
            focus = self.HallMenu.Btn_Single
        elseif self.HallMenu then
            focus = self.HallMenu
        end
        self:ForceUIInput(focus)

        if self.OutfitMenu then self.OutfitMenu:SetVisibility(1) end
    else
        if self.HallMenu then self.HallMenu:SetVisibility(1) end
    end
end

function M:OnOutfitMenuAnimFinished()
    Log.I("[HallMain] OnOutfitMenuAnimFinished forward=" .. tostring(self.bOutfitMenuForward))

    -- token 校验：丢弃旧 finished
    if self.__OutfitTokenLast ~= self.__OutfitToken then
        Log.I("[HallMain] Outfit finished ignored (stale)")
        return
    end

    if self.bOutfitMenuForward then
        if self.HallMenu then self.HallMenu:SetVisibility(1) end
    else
        if self.OutfitMenu then self.OutfitMenu:SetVisibility(1) end
    end
end

function M:OnMapMenuAnimFinished()
    Log.I("[HallMain] OnMapMenuAnimFinished forward=" .. tostring(self.bMapMenuForward))

    if self.__MapTokenLast ~= self.__MapToken then
        Log.I("[HallMain] Map finished ignored (stale)")
        return
    end

    if self.bMapMenuForward then
        if self.OutfitMenu then self.OutfitMenu:SetVisibility(1) end
    else
        if self.MapMenu then self.MapMenu:SetVisibility(1) end
    end
end

function M:OnConfigMenuAnimFinished()
    Log.I("[HallMain] OnConfigMenuAnimFinished forward=" .. tostring(self.bConfigMenuForward))
    if self.bConfigMenuForward then
        if self.HallMenu then self.HallMenu:SetVisibility(1) end
    else
        if self.ConfigMenu then self.ConfigMenu:SetVisibility(1) end
    end
end

function M:BindAnimFinished(anim, funcName)
    if not anim then return end
    if type(self[funcName]) ~= "function" then
        Log.E("[HallMain] BindAnimFinished: missing func " .. tostring(funcName))
        return
    end

    -- ✅ 你验证过：闭包绑定在你的 UnLua 环境下可用
    self:BindToAnimationFinished(anim, function()
        self[funcName](self)
    end)
end

return M