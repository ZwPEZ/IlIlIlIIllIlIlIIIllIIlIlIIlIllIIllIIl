#include "custom.h"
#include "../Settings/settings.h"
#include "imgui/imgui_internal.h"

#include <vector>
#include <algorithm>

namespace Custom {
    static float g_RowStartY = 0.0f;
    static float g_RowMaxHeight = 0.0f;

    void RenderTextGradient(const char* text, ImVec2 pos, ImVec4 topColor, ImVec4 bottomColor) {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        const int slices = 150;
        float slice_h = textSize.y / slices;

        for (int i = 0; i < slices; i++) {
            float t = (float)i / (slices - 1);
            ImVec4 col;
            col.x = topColor.x + (bottomColor.x - topColor.x) * t;
            col.y = topColor.y + (bottomColor.y - topColor.y) * t;
            col.z = topColor.z + (bottomColor.z - topColor.z) * t;
            col.w = topColor.w + (bottomColor.w - topColor.w) * t;

            ImU32 color = ImGui::GetColorU32(col);

            ImVec2 clip_min(pos.x, pos.y + slice_h * i);
            ImVec2 clip_max(pos.x + textSize.x, clip_min.y + slice_h);

            ImGui::GetWindowDrawList()->PushClipRect(clip_min, clip_max, true);
            ImGui::GetWindowDrawList()->AddText(pos, color, text);
            ImGui::GetWindowDrawList()->PopClipRect();
        }
    }

    bool BeginSection(const char* label, float height, Side side)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // ── Layout tuning ────────────────────────────────────────────
        const float column_spacing = 18.0f;
        const float header_height = 28.0f;
        const float rounding = 6.0f;

        // ── Available area ──────────────────────────────────────────
        float avail_width = ImGui::GetWindowContentRegionWidth();

        int columns = (side == Side::None) ? 1 : 3;
        float section_width =
            (columns == 1)
            ? avail_width
            : (avail_width - column_spacing * (columns - 1)) / columns;

        float total_row_width =
            (columns == 1)
            ? section_width
            : section_width * columns + column_spacing * (columns - 1);

        // ── Row start (LEFT only) ────────────────────────────────────
        if (side == Side::Left || side == Side::None)
        {
            g_RowStartY = ImGui::GetCursorPosY();
            g_RowMaxHeight = 0.0f;
        }

        // ── Center row horizontally ──────────────────────────────────
        float content_start_x = ImGui::GetWindowContentRegionMin().x - ImGui::GetWindowPos().x;
        float start_x = content_start_x + (avail_width - total_row_width) * 0.5f;

        if (side == Side::Middle)
            start_x += section_width + column_spacing;

        if (side == Side::Right)
            start_x += (section_width + column_spacing) * 2;

        // ── Lock cursor ──────────────────────────────────────────────
        ImGui::SetCursorPos(ImVec2(start_x, g_RowStartY));
        ImVec2 start_pos = ImGui::GetCursorScreenPos();

        g_RowMaxHeight = ImMax(g_RowMaxHeight, height);

        // ── Colors ───────────────────────────────────────────────────
        ImU32 accent_col = ImGui::GetColorU32(ImVec4(
            Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));

        ImU32 border_col = ImGui::GetColorU32(ImVec4(50 / 255.f, 50 / 255.f, 50 / 255.f, 1));
        ImU32 header_col = ImGui::GetColorU32(ImVec4(22 / 255.f, 22 / 255.f, 22 / 255.f, 1));
        ImU32 body_col = ImGui::GetColorU32(ImVec4(17 / 255.f, 17 / 255.f, 17 / 255.f, 1));

        ImRect bb(
            start_pos,
            ImVec2(start_pos.x + section_width, start_pos.y + height)
        );

        // ── Draw ─────────────────────────────────────────────────────
        draw->AddRectFilled(
            ImVec2(bb.Min.x, bb.Min.y + header_height),
            bb.Max,
            body_col,
            rounding,
            ImDrawFlags_RoundCornersBottom
        );

        draw->AddRectFilled(
            bb.Min,
            ImVec2(bb.Max.x, bb.Min.y + header_height),
            header_col,
            rounding,
            ImDrawFlags_RoundCornersTop
        );

        draw->AddLine(
            ImVec2(bb.Min.x + 1, bb.Min.y + header_height),
            ImVec2(bb.Max.x - 1, bb.Min.y + header_height),
            accent_col
        );

        draw->AddRect(bb.Min, bb.Max, border_col, rounding);

        draw->AddText(
            ImVec2(bb.Min.x + 10,
                bb.Min.y + (header_height - ImGui::GetFontSize()) * 0.5f),
            ImGui::GetColorU32(ImGuiCol_Text),
            label
        );

        // ── Content ──────────────────────────────────────────────────
        ImGui::SetCursorScreenPos(ImVec2(
            bb.Min.x + style.WindowPadding.x,
            bb.Min.y + header_height + style.WindowPadding.y));

        ImGui::BeginChild(
            label,
            ImVec2(
                section_width - style.WindowPadding.x * 2,
                height - header_height - style.WindowPadding.y * 2),
            false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        return true;
    }

    void EndSection(Side side)
    {
        ImGui::EndChild();

        if (side == Side::Right || side == Side::None)
        {
            ImGui::SetCursorPosY(g_RowStartY + g_RowMaxHeight + 16.0f);
        }
    }
}

void Custom::RenderTabs(int& selected_tab, const std::vector<TabInfo>& tabs, ImFont* icon_font, ImFont* smaller_font)
{
    if (tabs.empty()) {
        return;
    }

    static float indicator_pos_x = 0.0f;
    static float indicator_width = 0.0f;
    float target_indicator_pos_x = indicator_pos_x;
    float target_indicator_width = indicator_width;
    static std::vector<ImVec4> tab_colors;

    if (tab_colors.empty() || tab_colors.size() != tabs.size()) {
        tab_colors.resize(tabs.size(), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    const float tab_spacing = 40.0f;
    const float icon_text_spacing = 0.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();

    ImGui::PushFont(smaller_font);

    float total_tabs_width = 0.0f;
    for (const auto& tab : tabs) {
        ImGui::PushFont(icon_font);
        float icon_width = ImGui::CalcTextSize(tab.icon).x;
        ImGui::PopFont();
        float name_width = ImGui::CalcTextSize(tab.name).x;
        total_tabs_width += (std::max)(icon_width, name_width);
    }
    total_tabs_width += (tabs.size() - 1) * tab_spacing;

    if (indicator_width == 0.0f) {
        float initial_tab_offset = 0.0f;
        for (int i = 0; i < selected_tab; ++i) {
            ImGui::PushFont(icon_font);
            float icon_width = ImGui::CalcTextSize(tabs[i].icon).x;
            ImGui::PopFont();
            float name_width = ImGui::CalcTextSize(tabs[i].name).x;
            initial_tab_offset += (std::max)(icon_width, name_width) + tab_spacing;
        }
        indicator_pos_x = (win_size.x - total_tabs_width) * 0.5f + initial_tab_offset;
        ImGui::PushFont(icon_font);
        float icon_width = ImGui::CalcTextSize(tabs[selected_tab].icon).x;
        ImGui::PopFont();
        float name_width = ImGui::CalcTextSize(tabs[selected_tab].name).x;
        indicator_width = (std::max)(icon_width, name_width);
    }

    ImGui::SetCursorPosX((win_size.x - total_tabs_width) * 0.5f);

    for (int i = 0; i < tabs.size(); ++i) {
        const auto& tab = tabs[i];
        ImGui::PushID(i);

        if (i > 0) {
            ImGui::SameLine(0, tab_spacing);
        }

        ImVec2 tab_start_pos = ImGui::GetCursorScreenPos();

        ImGui::PushFont(icon_font);
        ImVec2 icon_size = ImGui::CalcTextSize(tab.icon);
        ImGui::PopFont();
        ImVec2 name_size = ImGui::CalcTextSize(tab.name);

        float tab_width = (std::max)(icon_size.x, name_size.x);
        float tab_height = icon_size.y + name_size.y + icon_text_spacing;

        bool is_selected = (selected_tab == i);

        ImVec4 target_color = is_selected ? ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        tab_colors[i] = ImLerp(tab_colors[i], target_color, ImGui::GetIO().DeltaTime * 15.0f);

        ImVec4 top_color = tab_colors[i];
        ImVec4 bottom_color = ImVec4(tab_colors[i].x * 0.7f, tab_colors[i].y * 0.7f, tab_colors[i].z * 0.7f, 1.0f);

        float total_height = icon_size.y + name_size.y + icon_text_spacing;
        float start_y = tab_start_pos.y + (Custom::TAB_HEIGHT - total_height) * 0.5f;

        ImGui::PushFont(icon_font);
        RenderTextGradient(tab.icon, ImVec2(tab_start_pos.x + (tab_width - icon_size.x) * 0.5f, start_y), top_color, bottom_color);
        ImGui::PopFont();

        RenderTextGradient(tab.name, ImVec2(tab_start_pos.x + (tab_width - name_size.x) * 0.5f, start_y + icon_size.y + icon_text_spacing), top_color, bottom_color);

        ImGui::SetCursorScreenPos(tab_start_pos);
        if (ImGui::InvisibleButton("##tab", ImVec2(tab_width, Custom::TAB_HEIGHT))) {
            selected_tab = i;
        }

        if (is_selected) {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            target_indicator_pos_x = min.x - win_pos.x;
            target_indicator_width = max.x - min.x;
        }

        ImGui::PopID();
    }

    ImGui::PopFont();

    float animation_speed = 15.0f;
    indicator_pos_x = ImLerp(indicator_pos_x, target_indicator_pos_x, ImGui::GetIO().DeltaTime * animation_speed);
    indicator_width = ImLerp(indicator_width, target_indicator_width, ImGui::GetIO().DeltaTime * animation_speed);

    float indicator_y = win_pos.y + win_size.y - 5.0f;
    float indicator_height = 2.0f;

    ImU32 accent_color = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));
    ImU32 glow_color_1 = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.25f));
    ImU32 glow_color_2 = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 0.1f));

    const float rounding = 5.0f;

    draw_list->AddRectFilled(
        ImVec2(win_pos.x + indicator_pos_x - 3.0f, indicator_y - 3.0f),
        ImVec2(win_pos.x + indicator_pos_x + indicator_width + 3.0f, indicator_y + indicator_height + 3.0f),
        glow_color_2,
        rounding
    );

    draw_list->AddRectFilled(
        ImVec2(win_pos.x + indicator_pos_x - 1.5f, indicator_y - 1.5f),
        ImVec2(win_pos.x + indicator_pos_x + indicator_width + 1.5f, indicator_y + indicator_height + 1.5f),
        glow_color_1,
        rounding
    );

    draw_list->AddRectFilled(
        ImVec2(win_pos.x + indicator_pos_x, indicator_y),
        ImVec2(win_pos.x + indicator_pos_x + indicator_width, indicator_y + indicator_height),
        accent_color,
        rounding
    );
}
