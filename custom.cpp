#include "custom.h"
#include "overlay.h"
#include "../Settings/settings.h"
#include "imgui/imgui_internal.h"

#include <vector>
#include <algorithm>

void Custom::RenderTabs(int& selected_tab, const std::vector<ID3D11ShaderResourceView*>& icons)
{
    if (icons.empty()) {
        return;
    }

    static float indicator_pos_x = 0.0f;
    static float indicator_width = 40.0f;

    const float tab_width = 40.0f;
    const float tab_height = 40.0f;
    const float tab_spacing = 20.0f;
    const ImVec2 tab_size(tab_width, tab_height);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    const float footer_height = 55.0f;

    float total_tabs_width = (icons.size() * tab_width) + (std::max(0, (int)icons.size() - 1) * tab_spacing);
    float start_x = win_pos.x + (win_size.x - total_tabs_width) * 0.5f;
    float cursor_y = win_pos.y + win_size.y - footer_height + (footer_height - tab_height) * 0.5f;

    ImGui::SetCursorScreenPos(ImVec2(start_x, cursor_y));

    float target_indicator_pos_x = indicator_pos_x;
    float target_indicator_width = indicator_width;

    for (int i = 0; i < icons.size(); ++i) {
        ImGui::PushID(i);

        if (i > 0) {
            ImGui::SameLine(0, tab_spacing);
        }

        bool is_selected = (selected_tab == i);
        ImVec4 tint_color = is_selected
            ? ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f)
            : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (ImGui::ImageButton("##tab", (void*)icons[i], tab_size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0,0,0,0), tint_color)) {
            selected_tab = i;
        }

        ImGui::PopStyleColor(3);

        if (is_selected) {
            ImRect tab_rect = ImGui::GetItemRect();
            target_indicator_pos_x = tab_rect.Min.x;
            target_indicator_width = tab_rect.GetWidth();
        }

        ImGui::PopID();
    }

    if (indicator_pos_x == 0.0f) {
        float initial_tab_offset = (selected_tab * tab_width) + (std::max(0, selected_tab) * tab_spacing);
        indicator_pos_x = start_x + initial_tab_offset;
        target_indicator_pos_x = indicator_pos_x;
    }

    float animation_speed = 15.0f;
    indicator_pos_x = ImLerp(indicator_pos_x, target_indicator_pos_x, ImGui::GetIO().DeltaTime * animation_speed);
    indicator_width = ImLerp(indicator_width, target_indicator_width, ImGui::GetIO().DeltaTime * animation_speed);

    float indicator_y = win_pos.y + win_size.y - footer_height + 1.0f;
    float indicator_height = 2.0f;

    ImU32 accent_color = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));
    ImU32 glow_color_top = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.5f));
    ImU32 glow_color_bottom = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.0f));

    draw_list->AddRectFilledMultiColor(
        ImVec2(indicator_pos_x - 5.0f, indicator_y - 8.0f),
        ImVec2(indicator_pos_x + indicator_width + 5.0f, indicator_y + indicator_height),
        glow_color_top,
        glow_color_top,
        glow_color_bottom,
        glow_color_bottom
    );

    draw_list->AddRectFilled(
        ImVec2(indicator_pos_x, indicator_y),
        ImVec2(indicator_pos_x + indicator_width, indicator_y + indicator_height),
        accent_color,
        5.0f
    );
}
