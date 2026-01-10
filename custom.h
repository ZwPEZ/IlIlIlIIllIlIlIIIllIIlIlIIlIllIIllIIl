#pragma once

#include "imgui/imgui.h"
#include <vector>

struct TabInfo;

namespace Custom {
    void RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs);
}
