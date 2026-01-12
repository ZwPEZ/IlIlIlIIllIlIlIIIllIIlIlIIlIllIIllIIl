#pragma once
#include "imgui/imgui.h"
#include <vector>

namespace Custom {
    enum class Side {
        None,
        Left,
        Middle,
        Right
    };

    struct TabInfo {
        const char* icon;
        const char* name;
    };

    inline constexpr float TAB_HEIGHT = 50.0f;

    void RenderTextGradient(const char* text, ImVec2 pos, ImVec4 topColor, ImVec4 bottomColor);
    bool BeginSection(const char* label, float height, Side side);

    // Note: The new column-based layout system does not automatically advance the cursor
    // after the last section. You must manually position subsequent UI elements
    // (like a footer) using ImGui::SetCursorPosY or ImGui::Dummy.
    void EndSection(Side side);

    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font, ImFont* smaller_font);
}
