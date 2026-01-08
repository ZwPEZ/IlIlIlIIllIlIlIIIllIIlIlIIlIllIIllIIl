#include "overlay.h"
#include <algorithm>
#include <cmath>

// This is a dependency that must be available in the include path during compilation.
#include "imgui.h"

Overlay::Overlay() : m_state(State::Loading), m_start_time(std::chrono::steady_clock::now()) {}

void Overlay::Render() {
    if (m_state == State::Loading) {
        RenderLoadingAnimation();
    } else {
        RenderUI();
    }
}

void Overlay::RenderUI() {
    // Placeholder for the main UI after the loading animation
    ImGui::Begin("Main UI");
    ImGui::Text("Animation finished.");
    ImGui::End();
}

void Overlay::RenderLoadingAnimation() {
    const float slide_in_duration = 1.2f;
    const float fade_in_duration = 1.5f;
    const float stay_duration = 1.0f;
    const float fade_out_duration = 0.5f;

    const float total_duration = slide_in_duration + stay_duration + fade_out_duration;

    auto now = std::chrono::steady_clock::now();
    float time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time).count() / 1000.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 display_size = io.DisplaySize;

    // --- Animation Progress ---
    float alpha = 0.0f;
    if (time_elapsed < fade_in_duration) {
        alpha = std::min(1.0f, time_elapsed / fade_in_duration);
    } else if (time_elapsed < fade_in_duration + stay_duration) {
        alpha = 1.0f;
    } else if (time_elapsed < total_duration) {
        alpha = 1.0f - std::min(1.0f, (time_elapsed - (fade_in_duration + stay_duration)) / fade_out_duration);
    }

    float slide_progress = std::min(1.0f, time_elapsed / slide_in_duration);
    float eased_slide = 1.0f - powf(1.0f - slide_progress, 4.0f); // EaseOutQuart

    // --- Font & Text Setup ---
    const char* text = "Flawless";
    // NOTE: Removed dynamic font scaling to fix a compilation error.
    // The text will now render at the default size provided by the font.
    ImVec2 text_size = ImGui::CalcTextSize(text);

    // --- Positioning ---
    const float initial_y_offset = text_size.y / 2.0f;
    ImVec2 text_pos_center = ImVec2(
        display_size.x / 2.0f - text_size.x / 2.0f,
        display_size.y / 2.0f - text_size.y / 2.0f
    );
    float current_y = text_pos_center.y - (initial_y_offset * (1.0f - eased_slide));
    ImVec2 final_text_pos = ImVec2(text_pos_center.x, current_y);

    // --- Drawing ---
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    ImU32 color_top = IM_COL32(200, 200, 200, (int)(255 * alpha));
    ImU32 color_bottom = IM_COL32(120, 120, 120, (int)(255 * alpha));
    const float shadow_offset = 1.0f;

    // Render bottom/shadow text
    draw_list->AddText(ImVec2(final_text_pos.x, final_text_pos.y + shadow_offset), color_bottom, text);

    // Render top/main text
    draw_list->AddText(final_text_pos, color_top, text);

    // --- State Transition ---
    if (time_elapsed >= total_duration) {
        m_state = State::Running;
    }
}
