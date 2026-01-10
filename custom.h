#pragma once

#include "imgui/imgui.h"
#include <d3d11.h>

namespace Custom {
    void Shutdown();
    void RenderTabs(int* selected_tab, const float footerHeight, ID3D11ShaderResourceView** icons, int num_tabs);
    bool LoadTextureFromMemory(ID3D11Device* device, const unsigned char* data, int width, int height, ID3D11ShaderResourceView** out_srv);
}
