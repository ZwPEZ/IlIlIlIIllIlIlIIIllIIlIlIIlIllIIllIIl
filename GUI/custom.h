#pragma once
#include <vector>

struct ImFont;

struct TabInfo {
    const char* icon;
    const char* name;
};

struct ImVec4;

namespace Custom {
    inline constexpr float TAB_HEIGHT = 40.0f;
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font, ImFont* smaller_font);
    void RenderTextGradient(const char* text, ImVec2 pos, ImVec4 topColor, ImVec4 bottomColor);
}
