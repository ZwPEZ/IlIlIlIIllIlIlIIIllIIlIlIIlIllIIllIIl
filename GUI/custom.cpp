#include "custom.h"
#include "overlay.h"
#include "../Settings/settings.h"
#include "imgui/imgui_internal.h"

#include <vector>
#include <algorithm>

void Custom::RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font)
{
    if (tabs.empty()) {
        return;
    }

    static float indicator_pos_x = 0.0f;
    static float indicator_width = 0.0f;

    const float tab_spacing = 40.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();

    float total_tabs_width = 0.0f;
    for (const auto& tab : tabs) {
        float icon_width = ImGui::CalcTextSize(tab.icon).x;
        float name_width = ImGui::CalcTextSize(tab.name).x;
        total_tabs_width += (std::max)(icon_width, name_width);
    }
    total_tabs_width += (tabs.size() - 1) * tab_spacing;

    // Center the tabs horizontally
    ImGui::SetCursorPosX((win_size.x - total_tabs_width) * 0.5f);

    float target_indicator_pos_x = indicator_pos_x;
    float target_indicator_width = indicator_width;

    for (int i = 0; i < tabs.size(); ++i) {
        const auto& tab = tabs[i];
        ImGui::PushID(i);

        if (i > 0) {
            ImGui::SameLine(0, tab_spacing);
        }

        ImVec2 tab_start_pos = ImGui::GetCursorPos();

        float icon_width = ImGui::CalcTextSize(tab.icon).x;
        float name_width = ImGui::CalcTextSize(tab.name).x;
        float tab_width = (std::max)(icon_width, name_width);

        bool is_selected = (selected_tab == i);

        ImGui::PushStyleColor(ImGuiCol_Text, is_selected ? ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::PushFont(icon_font);
        ImGui::SetCursorPosX(tab_start_pos.x + (tab_width - icon_width) * 0.5f);
        ImGui::Text("%s", tab.icon);
        ImGui::PopFont();

        ImGui::SetCursorPosX(tab_start_pos.x + (tab_width - name_width) * 0.5f);
        ImGui::Text("%s", tab.name);
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(tab_start_pos);
        if (ImGui::InvisibleButton("##tab", ImVec2(tab_width, TAB_HEIGHT))) {
            selected_tab = i;
        }

        if (is_selected) {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            target_indicator_pos_x = min.x;
            target_indicator_width = max.x - min.x;
        }

        ImGui::PopID();
    }

    if (indicator_width == 0.0f) {
        float start_x_screen = win_pos.x + (win_size.x - total_tabs_width) * 0.5f;
        float initial_tab_offset = 0.0f;
        for (int i = 0; i < selected_tab; ++i) {
            initial_tab_offset += (std::max)(ImGui::CalcTextSize(tabs[i].icon).x, ImGui::CalcTextSize(tabs[i].name).x) + tab_spacing;
        }
        indicator_pos_x = start_x_screen + initial_tab_offset;
        indicator_width = (std::max)(ImGui::CalcTextSize(tabs[selected_tab].icon).x, ImGui::CalcTextSize(tabs[selected_tab].name).x);
        target_indicator_pos_x = indicator_pos_x;
        target_indicator_width = indicator_width;
    }

    float animation_speed = 15.0f;
    indicator_pos_x = ImLerp(indicator_pos_x, target_indicator_pos_x, ImGui::GetIO().DeltaTime * animation_speed);
    indicator_width = ImLerp(indicator_width, target_indicator_width, ImGui::GetIO().DeltaTime * animation_speed);

    float indicator_y = win_pos.y + win_size.y - 5.0f;
    float indicator_height = 2.0f;

    ImU32 accent_color = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));
    ImU32 glow_color_1 = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.35f));
    ImU32 glow_color_2 = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.2f));

    const float rounding = 5.0f;

    draw_list->AddRectFilled(
        ImVec2(indicator_pos_x - 3.0f, indicator_y - 3.0f),
        ImVec2(indicator_pos_x + indicator_width + 3.0f, indicator_y + indicator_height + 3.0f),
        glow_color_2,
        rounding
    );

    draw_list->AddRectFilled(
        ImVec2(indicator_pos_x - 1.5f, indicator_y - 1.5f),
        ImVec2(indicator_pos_x + indicator_width + 1.5f, indicator_y + indicator_height + 1.5f),
        glow_color_1,
        rounding
    );

    draw_list->AddRectFilled(
        ImVec2(indicator_pos_x, indicator_y),
        ImVec2(indicator_pos_x + indicator_width, indicator_y + indicator_height),
        accent_color,
        rounding
    );
}
