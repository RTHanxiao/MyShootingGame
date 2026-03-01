--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_UI_OutfitMenu_C
local M = UnLua.Class()
local Log = require("Utility.Log")
local UEU = require("Utility.UE")

-- PlayMode 数字最稳：Forward=0 Reverse=1
local PLAY_FORWARD = 0
local PLAY_REVERSE = 1

local function PlayHover(widget, anim, bHover)
    if not widget or not anim then return end

    if bHover then
        widget:PlayAnimation(anim, 0.0, 1, PLAY_FORWARD, 1.0)
    else
        -- 反向从末尾开始播，避免“瞬间结束”
        local t = 0.0
        if anim.GetEndTime then t = anim:GetEndTime() end
        widget:PlayAnimation(anim, t, 1, PLAY_REVERSE, 1.0)
    end
end

function M:Construct()
    Log.I("[OutfitMenu] Construct")

    -- 只做事件绑定；WrapBox 的初始化先由 C++ NativeConstruct 保留（混跑阶段最稳）
    self:_BindHover(self.Btn_Map, "BtnMapHover", "BtnMapUnHover")
    self:_BindClick(self.Btn_Map, "BtnMapClicked")
    self:_BindClick(self.Btn_Back, "BtnBackClicked")

    -- 可选：确认关键控件/动画
    Log.I(("[OutfitMenu] Widgets Back=%s MapBtn=%s Tb_Map=%s Wrap=%s Anim=%s HallMainRef=%s"):format(
        tostring(self.Btn_Back),
        tostring(self.Btn_Map),
        tostring(self.Tb_Map),
        tostring(self.WB_SkinSlotPanel),
        tostring(self.MapTextHoverAnim),
        tostring(self.HallMainRef)
    ))
end

-- ===== Hover / Press =====
function M:BtnMapHover()
    -- 黑字
    UEU.SetTextColor(self.Tb_Map, 0, 0, 0, 1)
    PlayHover(self, self.MapTextHoverAnim, true)
end

function M:BtnMapUnHover()
    -- 白字
    UEU.SetTextColor(self.Tb_Map, 1, 1, 1, 1)
    PlayHover(self, self.MapTextHoverAnim, false)
end

-- ===== Click =====
function M:BtnBackClicked()
    Log.I("[OutfitMenu] BtnBackClicked")

    -- 切相机 Outfit -> Main
    local pawn = self:GetOwningPlayerPawn()
    if pawn and pawn.SetHallCameraFromMainToOutfit then
        pawn:SetHallCameraFromMainToOutfit(false)
    end

    -- 切 UI：Outfit 退，Main 进
    local hallMain = self.HallMainRef
    if not hallMain then
        Log.I("[OutfitMenu] HallMainRef nil. Ensure HallMain injects OutfitMenu.HallMainRef = self")
        return
    end

    hallMain:PlayOutfitMenuAnim(false)
    hallMain:PlayMainMenuAnim(true)

    Log.I(("[OutfitMenu] BackClicked before: selfVis=%s hit=%s"):format(
    tostring(self:GetVisibility()),
    tostring(self:IsInteractable())
))
end

function M:BtnMapClicked()
    Log.I("[OutfitMenu] BtnMapClicked ")

    local hallMain = self.HallMainRef
    if not hallMain then
        Log.I("[OutfitMenu] HallMainRef nil.")
        return
    end

    -- Outfit 退，Map 进
    hallMain:PlayOutfitMenuAnim(false)
    hallMain:PlayMapMenuAnim(true)
end

-- ===== 绑定工具 =====
function M:_BindHover(btn, hoverFuncName, unhoverFuncName)
    if not btn then return end

    -- 鼠标悬浮
    if btn.OnHovered then btn.OnHovered:Add(self, self[hoverFuncName]) end
    if btn.OnUnhovered then btn.OnUnhovered:Add(self, self[unhoverFuncName]) end

    -- 触屏/手柄：按下/松开
    if btn.OnPressed then btn.OnPressed:Add(self, self[hoverFuncName]) end
    if btn.OnReleased then btn.OnReleased:Add(self, self[unhoverFuncName]) end
end

function M:_BindClick(btn, clickFuncName)
    if not btn then return end
    btn.OnClicked:Add(self, self[clickFuncName])
end

return M