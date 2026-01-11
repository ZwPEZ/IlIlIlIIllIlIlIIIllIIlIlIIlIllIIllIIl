#pragma once
#include <vector>
#include "tab_info.h"
#include "imgui/imgui.h"

namespace Custom {
    inline constexpr float TAB_HEIGHT = 40.0f;
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font, ImFont* smaller_font);
    void RenderTextGradient(const char* text, ImVec2 pos, ImVec4 topColor, ImVec4 bottomColor);
    // height = 0.0f for auto-sizing
    bool BeginSection(const char* name, float height = 0.0f);
    void EndSection();
    void BeginSectionLayout(int columns);
    void NextSection();
    void EndSectionLayout();
}
