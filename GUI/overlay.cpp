#include "overlay.h"
#include "custom.h"
#include "../Settings/settings.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

void Overlay::RenderMenu()
{
    const float menuWidth = 870.0f;
    const float menuHeight = 550.0f;

    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 160));
    ImVec2 centerPos((ImGui::GetIO().DisplaySize.x - menuWidth) * 0.5f, (ImGui::GetIO().DisplaySize.y - menuHeight) * 0.5f);

    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_Once);

    if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
    {
        const float headerHeight = 55.0f;
        const float footerHeight = 55.0f;

        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();

        ImU32 accentColor = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], Theme::Accent[3]));
        ImU32 topColor = IM_COL32(static_cast<int>(Theme::Accent[0] * 255), static_cast<int>(Theme::Accent[1] * 255), static_cast<int>(Theme::Accent[2] * 255), 50);
        ImU32 topTransparent = IM_COL32(static_cast<int>(Theme::Accent[0] * 255), static_cast<int>(Theme::Accent[1] * 255), static_cast<int>(Theme::Accent[2] * 255), 0);

        ImGui::GetWindowDrawList()->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + headerHeight), IM_COL32(22, 22, 22, 255), 9.0f, ImDrawFlags_RoundCornersTop);

        const char* titleMain = "Flawless";
        const char* titleAccent = ".win";

        ImVec2 mainSize = ImGui::CalcTextSize(titleMain);
        ImVec2 accentSize = ImGui::CalcTextSize(titleAccent);

        float textY = winPos.y + (headerHeight - mainSize.y) * 0.5f;
        float textX = winPos.x + 15.0f;

        ImVec4 topColMain = ImVec4(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f);
        ImVec4 botColMain = ImVec4(150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f, 1.0f);
        ImVec4 topCol = ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f );
        ImVec4 botCol = ImVec4(Theme::Accent[0] * 0.7f, Theme::Accent[1] * 0.7f, Theme::Accent[2] * 0.7f, 1.0f);

        Custom::RenderTextGradient(titleMain, ImVec2(textX, textY), topColMain, botColMain);
        Custom::RenderTextGradient(titleAccent, ImVec2(textX + mainSize.x, textY), topCol, botCol);

        ImGui::GetWindowDrawList()->AddLine(ImVec2(winPos.x, winPos.y + headerHeight), ImVec2(winPos.x + winSize.x, winPos.y + headerHeight), accentColor, 1.0f);
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(winPos.x, winPos.y + headerHeight), ImVec2(winPos.x + winSize.x, winPos.y + headerHeight + 45), topColor, topColor, topTransparent, topTransparent);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), IM_COL32(22, 22, 22, 255), 9.0f, ImDrawFlags_RoundCornersBottom);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y - footerHeight), accentColor, 1.0f);
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight - 45), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y - footerHeight), topTransparent, topTransparent, topColor, topColor);

        ImGui::Dummy(ImVec2(0, headerHeight));

        // Content
        if (m_selected_tab == 0) {
            Custom::BeginSectionLayout(2);
            if (Custom::BeginSection("Targeting")) {
                ImGui::Text("Some targeting settings here...");
                Custom::EndSection();
            }
            Custom::NextSection();
            if (Custom::BeginSection("Visuals")) {
                ImGui::Text("Some visuals settings here...");
                Custom::EndSection();
            }
            Custom::EndSectionLayout();
        }
        else if (m_selected_tab == 1) {
            if (Custom::BeginSection("Aimbot")) {
                ImGui::Text("Some aimbot settings here...");
                Custom::EndSection();
            }
        }

        float footerCenterY = winSize.y - footerHeight + (footerHeight - Custom::TAB_HEIGHT) * 0.5f;
        ImGui::SetCursorPosY(footerCenterY);
        Custom::RenderTabs(m_selected_tab, m_tabs, m_icon_font, m_smaller_font);

        ImGui::End();
    }
}
