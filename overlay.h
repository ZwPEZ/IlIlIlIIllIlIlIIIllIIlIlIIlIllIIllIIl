#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <chrono>
#include "imgui/imgui.h"
#include <vector>

enum class State {
    Loading,
    Running
};

class Overlay {
public:
    Overlay();
    ~Overlay();

    void Run();

private:
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool CreateOverlayWindow();
    bool InitDX11();
    void InitImGui();

    void Cleanup();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    void Render();
    void RenderMenu();
    void RenderLoadingAnimation();

    HWND m_hwnd = nullptr;
    HWND m_target_hwnd = nullptr;
    std::wstring m_window_name;

    ID3D11Device* m_pd3dDevice = nullptr;
    ID3D11DeviceContext* m_pd3dDeviceContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;

    ImFont* m_poppins_semibold = nullptr;
    ImFont* m_poppins_regular = nullptr;
    ImFont* m_poppins_bold = nullptr;
    ImFont* m_poppins_extrabold = nullptr;

    ID3D11ShaderResourceView* m_eye_icon_texture = nullptr;
    std::vector<ID3D11ShaderResourceView*> m_tab_icons;
    int m_selected_tab = 0;

    State m_state = State::Loading;
    std::chrono::steady_clock::time_point m_start_time;
};
