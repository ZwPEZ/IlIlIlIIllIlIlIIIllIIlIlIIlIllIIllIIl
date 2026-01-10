#include "custom.h"
#include "../Settings/settings.h"
#include "imgui/imgui_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "overlay/stb/stb_image.h"

bool Custom::LoadTextureFromMemory(ID3D11Device* device, const unsigned char* data, unsigned int size, ID3D11ShaderResourceView** out_srv) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(data, size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &pTexture);
    if (FAILED(hr)) {
        stbi_image_free(image_data);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    stbi_image_free(image_data);

    return !FAILED(hr);
}


void Custom::RenderTabs(int* selected_tab, const float footerHeight, ID3D11ShaderResourceView** icons, int num_tabs) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImDrawList* draw_list = window->DrawList;
    const ImVec2 winPos = ImGui::GetWindowPos();
    const ImVec2 winSize = ImGui::GetWindowSize();
    const ImGuiStyle& style = ImGui::GetStyle();

    const float button_width = 80.0f;
    const float button_height = footerHeight;

    const float total_buttons_width = (num_tabs * button_width) + ((num_tabs - 1) * style.ItemSpacing.x);
    const float start_x = (winSize.x - total_buttons_width) / 2.0f;
    const float cursor_y = winSize.y - footerHeight;

    static ImVec2 indicator_pos;
    static ImVec2 indicator_size;

    ImVec2 current_button_pos;
    float current_button_width = 0.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    ImGui::SetCursorPos(ImVec2(start_x, cursor_y));
    for (int i = 0; i < num_tabs; ++i) {
        ImGui::SameLine(start_x + i * (button_width + style.ItemSpacing.x));

        bool is_selected = (*selected_tab == i);
        ImVec4 tint_color = is_selected ? ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        char button_id[32];
        sprintf(button_id, "tab_icon_%d", i);

        if (ImGui::ImageButton(button_id, (void*)icons[i], ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), tint_color)) {
            *selected_tab = i;
        }

        if (is_selected) {
            current_button_pos = ImGui::GetItemRectMin();
            current_button_width = ImGui::GetItemRectSize().x;
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImVec2 target_pos = ImVec2(current_button_pos.x, winPos.y + winSize.y - 4.0f);
    ImVec2 target_size = ImVec2(current_button_width, 4.0f);

    if (indicator_pos.x == 0) { // First frame init
        indicator_pos = target_pos;
        indicator_size = target_size;
    }
    else {
        indicator_pos.x = ImLerp(indicator_pos.x, target_pos.x, ImGui::GetIO().DeltaTime * 15.0f);
        indicator_pos.y = target_pos.y;
        indicator_size.x = ImLerp(indicator_size.x, target_size.x, ImGui::GetIO().DeltaTime * 15.0f);
        indicator_size.y = target_size.y;
    }

    ImU32 accentColor = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));
    ImU32 accentColorTransparent = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.0f));

    draw_list->AddRectFilledMultiColor(
        ImVec2(indicator_pos.x, indicator_pos.y - 10.0f),
        ImVec2(indicator_pos.x + indicator_size.x, indicator_pos.y),
        accentColorTransparent,
        accentColorTransparent,
        accentColor,
        accentColor
    );

    draw_list->AddRectFilled(
        indicator_pos,
        ImVec2(indicator_pos.x + indicator_size.x, indicator_pos.y + indicator_size.y),
        accentColor,
        5.0f
    );
}
