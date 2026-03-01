-- Content/Script/UI/Hall/HallMenu.lua
local M = UnLua.Class()
local Log = require("Utility.Log")
local UEU = require("Utility.UE")

-- PlayAnimation 的 PlayMode：Forward=0, Reverse=1（用数字最稳）
local PLAY_FORWARD = 0
local PLAY_REVERSE = 1

local function SetTextColor(tb, r, g, b, a)
    UEU.SetTextColor(tb, r, g, b, a)
end

function M:Construct()
    Log.I("[HallMenu] Construct")

    -- Hover/UnHover（鼠标）+ Press/Release（触屏/手柄）都绑定
    self:_BindHover(self.Btn_Single, "BtnSingleHover", "BtnSingleUnHover")
    self:_BindHover(self.Btn_Multi,  "BtnMultiHover",  "BtnMultiUnHover")
    self:_BindHover(self.Btn_Cfg,    "BtnCfgHover",    "BtnCfgUnHover")
    self:_BindHover(self.Btn_Exit,   "BtnExitHover",   "BtnExitUnHover")

    -- Click
    self:_BindClick(self.Btn_Single, "BtnSingleClicked")
    self:_BindClick(self.Btn_Multi,  "BtnMultiClicked")
    self:_BindClick(self.Btn_Cfg,    "BtnCfgClicked")
    self:_BindClick(self.Btn_Exit,   "BtnExitClicked")

    -- 文本
    if self.Tb_Single then self.Tb_Single:SetText("单人模式") end
    if self.Tb_Multi  then self.Tb_Multi:SetText("多人模式") end
    if self.Tb_Cfg    then self.Tb_Cfg:SetText("设置") end
    if self.Tb_Exit   then self.Tb_Exit:SetText("退出游戏") end

    -- 可选：确认 hover 动画是否拿到了（nil 的话当然播不了）
    Log.I(("[HallMenu] HoverAnim Single=%s Multi=%s Cfg=%s Exit=%s"):format(
        tostring(self.SingleTextHoverAnim),
        tostring(self.MultiTextHoverAnim),
        tostring(self.CfgTextHoverAnim),
        tostring(self.ExitTextHoverAnim)
    ))
end

-- ===== Hover / Press =====
function M:BtnSingleHover()
    SetTextColor(self.Tb_Single, 0, 0, 0, 1)
    self:PlayHoverAnim(self.SingleTextHoverAnim, true)
end

function M:BtnMultiHover()
    SetTextColor(self.Tb_Multi, 0, 0, 0, 1)
    self:PlayHoverAnim(self.MultiTextHoverAnim, true)
end

function M:BtnCfgHover()
    SetTextColor(self.Tb_Cfg, 0, 0, 0, 1)
    self:PlayHoverAnim(self.CfgTextHoverAnim, true)
end

function M:BtnExitHover()
    SetTextColor(self.Tb_Exit, 0, 0, 0, 1)
    self:PlayHoverAnim(self.ExitTextHoverAnim, true)
end

-- ===== UnHover / Release =====
function M:BtnSingleUnHover()
    SetTextColor(self.Tb_Single, 1, 1, 1, 1)
    self:PlayHoverAnim(self.SingleTextHoverAnim, false)
end

function M:BtnMultiUnHover()
    SetTextColor(self.Tb_Multi, 1, 1, 1, 1)
    self:PlayHoverAnim(self.MultiTextHoverAnim, false)
end

function M:BtnCfgUnHover()
    SetTextColor(self.Tb_Cfg, 1, 1, 1, 1)
    self:PlayHoverAnim(self.CfgTextHoverAnim, false)
end

function M:BtnExitUnHover()
    SetTextColor(self.Tb_Exit, 1, 1, 1, 1)
    self:PlayHoverAnim(self.ExitTextHoverAnim, false)
end

-- ===== Click =====
function M:BtnSingleClicked()
   Log.I("[HallMenu] BtnSingleClicked")

   local pawn = self:GetOwningPlayerPawn()
   if pawn and pawn.SetHallCameraFromMainToOutfit then
       pawn:SetHallCameraFromMainToOutfit(true)
   end

   local hallMain = self.HallMainRef
   Log.I("[HallMenu] HallMainRef=" .. tostring(hallMain))
   if not hallMain then
       Log.E("[HallMenu] HallMainRef is nil. Inject it from HallMain Construct: self.HallMenu.HallMainRef = self")
       return
   end

   Log.I(("[HallMenu] HallMain=%s MainAnim=%s OutfitAnim=%s MapAnim=%s CfgAnim=%s"):format(
       tostring(hallMain),
       tostring(hallMain.MainMenuAnim),
       tostring(hallMain.OutfitMenuAnim),
       tostring(hallMain.MapMenuAnim),
       tostring(hallMain.ConfigMenuAnim)
   ))

   hallMain:PlayOutfitMenuAnim(true)
   hallMain:PlayMainMenuAnim(false)
end

function M:BtnCfgClicked()
   Log.I("[HallMenu] BtnCfgClicked")

   local hallMain = self.HallMainRef
   Log.I("[HallMenu] HallMainRef=" .. tostring(hallMain))
   if not hallMain then
       Log.E("[HallMenu] HallMainRef is nil.")
       return
   end

   Log.I(("[HallMenu] HallMain=%s MainAnim=%s OutfitAnim=%s MapAnim=%s CfgAnim=%s"):format(
       tostring(hallMain),
       tostring(hallMain.MainMenuAnim),
       tostring(hallMain.OutfitMenuAnim),
       tostring(hallMain.MapMenuAnim),
       tostring(hallMain.ConfigMenuAnim)
   ))

   hallMain:PlayConfigMenuAnim(true)
   hallMain:PlayMainMenuAnim(false)
end

function M:BtnMultiClicked()
   Log.I("[HallMenu] BtnMultiClicked")
   -- TODO
end

function M:BtnExitClicked()
    Log.I("[HallMenu] BtnExitClicked")

    local world = self:GetWorld()
    local pc = self:GetOwningPlayer()
    if not world or not pc then return end

    local quitPref = (EQuitPreference and EQuitPreference.Quit) or 0
    UEU.QuitGame(world, pc, quitPref, true)
end

-- ===== HoverAnim =====
function M:PlayHoverAnim(anim, bHover)
   if not anim then return end
   if bHover then
       self:PlayAnimation(anim, 0.0, 1, PLAY_FORWARD, 1.0)
   else
       local t = 0.0
       if anim.GetEndTime then t = anim:GetEndTime() end
       self:PlayAnimation(anim, t, 1, PLAY_REVERSE, 1.0)
   end
end

-- ===== 绑定工具 =====
function M:_BindHover(btn, hoverFuncName, unhoverFuncName)
    if not btn then return end

    -- 鼠标悬浮
    if btn.OnHovered then
        btn.OnHovered:Add(self, self[hoverFuncName])
    end
    if btn.OnUnhovered then
        btn.OnUnhovered:Add(self, self[unhoverFuncName])
    end

    -- 触屏/手柄：按下/松开（解决“按下没有悬浮动画”）
    if btn.OnPressed then
        btn.OnPressed:Add(self, self[hoverFuncName])
    end
    if btn.OnReleased then
        btn.OnReleased:Add(self, self[unhoverFuncName])
    end
end

function M:_BindClick(btn, clickFuncName)
    if not btn then return end
    btn.OnClicked:Add(self, self[clickFuncName])
end

return M