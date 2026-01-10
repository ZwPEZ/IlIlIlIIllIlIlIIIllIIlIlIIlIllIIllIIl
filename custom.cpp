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

    const float tab_height = 40.0f;
    const float tab_spacing = 40.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    const float footer_height = 55.0f;

    float total_tabs_width = 0.0f;
    for (const auto& tab : tabs) {
        float icon_width = ImGui::CalcTextSize(tab.icon).x;
        float name_width = ImGui::CalcTextSize(tab.name).x;
        total_tabs_width += std::max(icon_width, name_width);
    }
    total_tabs_width += (tabs.size() - 1) * tab_spacing;

    float start_x = win_pos.x + (win_size.x - total_tabs_width) * 0.5f;
    float cursor_y = win_pos.y + win_size.y - footer_height + 5.0f;

    ImGui::SetCursorScreenPos(ImVec2(start_x, cursor_y));

    float target_indicator_pos_x = indicator_pos_x;
    float target_indicator_width = indicator_width;

    for (int i = 0; i < tabs.size(); ++i) {
        const auto& tab = tabs[i];
        ImGui::PushID(i);

        if (i > 0) {
            ImGui::SameLine(0, tab_spacing);
        }

        float icon_width = ImGui::CalcTextSize(tab.icon).x;
        float name_width = ImGui::CalcTextSize(tab.name).x;
        float tab_width = std::max(icon_width, name_width);

        bool is_selected = (selected_tab == i);

        ImGui::PushStyleColor(ImGuiCol_Text, is_selected ? ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::PushFont(icon_font);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (tab_width - icon_width) * 0.5f);
        ImGui::Text("%s", tab.icon);
        ImGui::PopFont();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (tab_width - name_width) * 0.5f);
        ImGui::Text("%s", tab.name);
        ImGui::PopStyleColor();

        ImVec2 tab_min = ImGui::GetItemRectMin();
        ImGui::SetCursorScreenPos(ImVec2(tab_min.x, cursor_y));
        if (ImGui::InvisibleButton("##tab", ImVec2(tab_width, tab_height))) {
            selected_tab = i;
        }

        if (is_selected) {
            ImRect tab_rect = ImGui::GetItemRect();
            target_indicator_pos_x = tab_rect.Min.x;
            target_indicator_width = tab_rect.GetWidth();
        }

        ImGui::PopID();
    }

    if (indicator_width == 0.0f) {
        float initial_tab_offset = 0.0f;
        for (int i = 0; i < selected_tab; ++i) {
            initial_tab_offset += std::max(ImGui::CalcTextSize(tabs[i].icon).x, ImGui::CalcTextSize(tabs[i].name).x) + tab_spacing;
        }
        indicator_pos_x = start_x + initial_tab_offset;
        indicator_width = std::max(ImGui::CalcTextSize(tabs[selected_tab].icon).x, ImGui::CalcTextSize(tabs[selected_tab].name).x);
        target_indicator_pos_x = indicator_pos_x;
        target_indicator_width = indicator_width;
    }

    float animation_speed = 15.0f;
    indicator_pos_x = ImLerp(indicator_pos_x, target_indicator_pos_x, ImGui::GetIO().DeltaTime * animation_speed);
    indicator_width = ImLerp(indicator_width, target_indicator_width, ImGui::GetIO().DeltaTime * animation_speed);

    float indicator_y = win_pos.y + win_size.y - 5.0f;
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
