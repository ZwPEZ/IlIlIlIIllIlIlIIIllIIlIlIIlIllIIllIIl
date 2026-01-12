#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <chrono>

class Overlay {
public:
    enum class State {
        Loading,
        Running
    };

    Overlay();
    ~Overlay();

    void Run();

private:
    bool CreateOverlayWindow();
    bool InitDX11();
    void InitImGui();
    void Cleanup();
    void Render();
    void RenderLoadingAnimation();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HWND m_target_hwnd = nullptr;

    ID3D11Device* m_pd3dDevice = nullptr;
    ID3D11DeviceContext* m_pd3dDeviceContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;

    State m_state = State::Loading;
    std::chrono::steady_clock::time_point m_start_time;
};
