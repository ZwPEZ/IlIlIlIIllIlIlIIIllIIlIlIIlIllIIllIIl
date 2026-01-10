#pragma once
#include "custom.h"
#include <vector>

class Overlay {
public:
    void RenderMenu();

private:
    int m_selected_tab = 0;
    std::vector<TabInfo> m_tabs = { {"C", "Visuals"}, {"D", "Aimbot"} };
    ImFont* m_icon_font = nullptr;
    ImFont* m_smaller_font = nullptr;
};
