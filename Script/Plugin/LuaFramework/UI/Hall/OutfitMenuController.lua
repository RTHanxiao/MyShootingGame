local C = UnLua.Class()

function C:Init(widget, args, ui)
    self.widget = widget
    self.args = args
    self.ui = ui

    print("[OutfitMenuController] Init v1 args=" .. tostring(args and args.from))

    local btn = widget:GetWidgetFromName("BtnBack") -- 你 UI 里的按钮名
    if btn then
        btn.OnClicked:Add(self, self.OnBackClicked)
    else
        print("[OutfitMenuController] BtnBack not found")
    end
end

function C:OnBackClicked()
    print("[OutfitMenuController] OnBackClicked v1")
    if self.ui then
        self.ui:CloseTop()
    end
end

function C:Deinit()
    -- 可选：如果你做了手动解绑，就放这里
end

return C