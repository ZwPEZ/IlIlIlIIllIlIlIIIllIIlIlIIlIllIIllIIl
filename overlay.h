#pragma once

#include <d3d11.h>
#include <string>
#include <chrono>

class Overlay {
public:
    Overlay();
    ~Overlay();

    void Run();

private:
    bool CreateOverlayWindow();
    bool InitDX11();
    void InitImGui();
    void Cleanup();

    void Render();
    void RenderMenu();
    void RenderLoadingAnimation();

    void CreateRenderTarget();
    void CleanupRenderTarget();

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
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

    enum class State {
        Loading,
        Running
    };

    State m_state = State::Loading;
    std::chrono::steady_clock::time_point m_start_time;

public:
    ID3D11ShaderResourceView* m_eye_icon_texture = nullptr;
    ID3D11ShaderResourceView* m_cog_icon_texture = nullptr;
    ID3D11ShaderResourceView* m_user_icon_texture = nullptr;
};
