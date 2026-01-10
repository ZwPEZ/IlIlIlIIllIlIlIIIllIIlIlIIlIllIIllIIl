#pragma once

#include "imgui/imgui.h"
#include <vector>

struct ID3D11ShaderResourceView;

namespace Custom {
    void RenderTabs(int& selected_tab, const std::vector<ID3D11ShaderResourceView*>& icons);
}
