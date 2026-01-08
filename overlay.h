#pragma once

#include <chrono>

// Forward declaration for ImGui types
struct ImVec2;
struct ImDrawList;

class Overlay {
public:
    Overlay();
    void Render();

private:
    enum class State {
        Loading,
        Running
    };

    void RenderLoadingAnimation();
    void RenderUI();

    State m_state;
    std::chrono::steady_clock::time_point m_start_time;
};
