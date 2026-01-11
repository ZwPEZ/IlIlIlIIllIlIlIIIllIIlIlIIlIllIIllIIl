#pragma once
#include <vector>
#include "tab_info.h"
#include "imgui/imgui.h"

namespace Custom {
    enum class Side {
        None,
        Left,
        Middle,
        Right
    };

    inline constexpr float TAB_HEIGHT = 40.0f;
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font, ImFont* smaller_font);
    void RenderTextGradient(const char* text, ImVec2 pos, ImVec4 topColor, ImVec4 bottomColor);
    bool BeginSection(const char* name, float height, Side side = Side::None);
    void EndSection();
}
