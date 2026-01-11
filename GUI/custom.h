#pragma once

#include "imgui/imgui.h"
#include <string>
#include <vector>

struct TabInfo {
    const char* icon;
    const char* name;
};

namespace Custom {
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font);
}
