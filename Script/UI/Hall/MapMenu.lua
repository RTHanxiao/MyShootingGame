-- Content/Script/UI/Hall/MapMenu.lua
local M = UnLua.Class()
local Log = require("Utility.Log")
local UEU = require("Utility.UE")

local PLAY_FORWARD = 0
local PLAY_REVERSE = 1

local function PlayHover(widget, anim, bHover)
    if not widget or not anim then return end
    if bHover then
        widget:PlayAnimation(anim, 0.0, 1, PLAY_FORWARD, 1.0)
    else
        local t = 0.0
        if anim.GetEndTime then t = anim:GetEndTime() end
        widget:PlayAnimation(anim, t, 1, PLAY_REVERSE, 1.0)
    end
end

function M:Construct()
    Log.I("[MapMenu] Construct HallMainRef=" .. tostring(self.HallMainRef))

    if self.__bound then
        Log.I("[MapMenu] Construct skipped (already bound)")
        return
    end
    self.__bound = true


    -- 绑定按钮（Start/Back）
    self:_BindHover(self.Btn_Start, "BtnStartHover", "BtnStartUnHover")
    self:_BindHover(self.Btn_Back,  "BtnBackHover",  "BtnBackUnHover")

    self:_BindClick(self.Btn_Start, "BtnStartClicked_Lua")
    self:_BindClick(self.Btn_Back,  "BtnBackClicked_Lua")

    -- SB_Map 里的子项（MapSlot）如果已经由 C++/蓝图填充，这里只负责绑定选择事件
    self:_BindExistingMapSlots()

    Log.I(("[MapMenu] Widgets StartBtn=%s BackBtn=%s TB_Start=%s SB_Map=%s"):format(
        tostring(self.Btn_Start),
        tostring(self.Btn_Back),
        tostring(self.TB_Start),
        tostring(self.SB_Map)
    ))
end

-- ===== Hover: 你现在只给了 TB_Start，所以先只处理 Start 文本色 =====
function M:BtnStartHover()
    UEU.SetTextColor(self.TB_Start, 0,0,0,1)
end

function M:BtnStartUnHover()
    UEU.SetTextColor(self.TB_Start, 1,1,1,1)
end

function M:BtnBackHover()
    -- TODO
end

function M:BtnBackUnHover()
end

-- ===== Click =====
function M:BtnStartClicked_Lua()
    Log.I("[MapMenu] BtnStartClicked")

    -- 优先：如果 C++ 有现成 BtnStartClicked（实现 OpenLevel/LoadSubLevel），直接调用
    if self.BtnStartClicked then
        self:BtnStartClicked()
        return
    end

    -- 次优：如果有 LoadSubLevel（你项目里常见）
    if self.LoadSubLevel and self.CurSelectedMapName then
        self:LoadSubLevel(self.CurSelectedMapName)
        return
    end

    -- 兜底：Lua OpenLevel（仅建议单机调试用）
    local world = self:GetWorld()
    if not world then return end

    local mapName = self.CurSelectedMapName
    if not mapName or mapName == "" then
        Log.E("[MapMenu] No map selected")
        return
    end

    if UEU.Gameplay and UEU.Gameplay.OpenLevel then
        UEU.Gameplay.OpenLevel(world, mapName)
    else
        Log.E("[MapMenu] UGameplayStatics not available; keep C++ BtnStartClicked/LoadSubLevel")
    end
end

function M:BtnBackClicked_Lua()
    Log.I("[MapMenu] BtnBackClicked_Lua")

    local hallMain = self.HallMainRef
    if not hallMain then
        Log.E("[MapMenu] HallMainRef nil")
        return
    end

    -- 先目标 forward，再来源 reverse（避免中间空窗 + finished 时序更稳定）
    hallMain:PlayOutfitMenuAnim(true)
    hallMain:PlayMapMenuAnim(false)
end

-- ===== ScrollBox 子项绑定 =====
function M:_BindExistingMapSlots()
    if not self.SB_Map then
        Log.I("[MapMenu] SB_Map nil (skip slot bind)")
        return
    end
    if not self.SB_Map.GetChildrenCount then
        Log.I("[MapMenu] ScrollBox API not exported (skip slot bind)")
        return
    end

    local n = self.SB_Map:GetChildrenCount()
    Log.I("[MapMenu] Slot count=" .. tostring(n))

    for i = 0, n - 1 do
        local child = self.SB_Map:GetChildAt(i)
        if child then
            -- 兼容：子项里可能有不同命名的按钮
            local btn = child.Btn_Select or child.Btn_Slot or child.Button or child.Btn
            if btn and btn.OnClicked then
                btn.OnClicked:Add(self, function()
                    self:OnMapSlotSelected(child)
                end)
            elseif child.OnClicked then
                child.OnClicked:Add(self, function()
                    self:OnMapSlotSelected(child)
                end)
            end
        end
    end
end

function M:OnMapSlotSelected(slot)
    local name =
        slot.MapName or
        slot.CurMapName or
        (slot.CurSelectedMap and slot.CurSelectedMap.Name) or
        (slot.MapData and slot.MapData.Name)

    Log.I("[MapMenu] SlotSelected=" .. tostring(name))
    if name then
        self.CurSelectedMapName = name
    end

    if slot.SetSelected then
        slot:SetSelected(true)
    end
end

-- ===== 绑定工具 =====
function M:_BindHover(btn, hoverFuncName, unhoverFuncName)
    if not btn then return end
    if btn.OnHovered then btn.OnHovered:Add(self, self[hoverFuncName]) end
    if btn.OnUnhovered then btn.OnUnhovered:Add(self, self[unhoverFuncName]) end
    if btn.OnPressed then btn.OnPressed:Add(self, self[hoverFuncName]) end
    if btn.OnReleased then btn.OnReleased:Add(self, self[unhoverFuncName]) end
end

function M:_BindClick(btn, clickFuncName)
    if not btn then return end
    btn.OnClicked:Add(self, self[clickFuncName])
end

return M