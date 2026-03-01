-- Content/Script/Utility/UE.lua
local UEU = {}

-- 统一从 UE / UE5 / UE4 / _G 取 UClass/Enum/Struct
function UEU.Get(name)
    return (UE and UE[name]) or (UE5 and UE5[name]) or (UE4 and UE4[name]) or _G[name]
end

function UEU.Class(name)
    local c = UEU.Get(name)
    assert(c, ("[UEU] Class not found: %s"):format(name))
    return c
end

function UEU.Optional(name)
    return UEU.Get(name)
end

-- 常用类型/库
UEU.FLinearColor  = UEU.Class("FLinearColor")
UEU.MSGBase       = UEU.Class("UMSG_UserWidgetBase")
UEU.Gameplay      = UEU.Optional("UGameplayStatics")
UEU.KismetSystem  = UEU.Optional("UKismetSystemLibrary")

-- 文本颜色：走你 C++ 包装函数 SetTextBlockColor(UTextBlock*, FLinearColor)
function UEU.SetTextColor(TextBlock, r, g, b, a)
    if not TextBlock then return end
    UEU.MSGBase.SetTextBlockColor(TextBlock, UEU.FLinearColor(r, g, b, a))
end

-- 播放 WidgetAnimation：用数字 playmode 最稳（Forward=0 Reverse=1）
function UEU.PlayAnim(UserWidget, Anim, bForward, speed)
    if not UserWidget or not Anim then return end
    UserWidget:PlayAnimation(Anim, 0.0, 1, bForward and 0 or 1, speed or 1.0)
end

-- QuitGame：优先用 OwningPlayer，避免依赖 GameplayStatics
function UEU.QuitGame(World, PlayerController, quitPref, ignorePlatformRestrictions)
    local KSL = UEU.KismetSystem or UEU.Get("UKismetSystemLibrary")
    assert(KSL, "[UEU] UKismetSystemLibrary not found")
    KSL.QuitGame(World, PlayerController, quitPref or 0, ignorePlatformRestrictions == true)
end

return UEU