#pragma once

#include "imgui/imgui.h"
#include <vector>

struct TabInfo;
struct ImFont;

namespace Custom {
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font);
}
