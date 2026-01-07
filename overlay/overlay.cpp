#include "overlay.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Overlay::Overlay() {
    m_target_hwnd = FindWindow(nullptr, L"Roblox");
}

Overlay::~Overlay() {
    Cleanup();
}

void Overlay::Run() {
    if (!m_target_hwnd) return;

    SetForegroundWindow(m_target_hwnd);

    if (!CreateOverlayWindow()) return;
    if (!InitDX11()) return;
    InitImGui();

    m_start_time = std::chrono::steady_clock::now();

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        Render();
    }
}

bool Overlay::CreateOverlayWindow() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Overlay", nullptr };
    RegisterClassEx(&wc);

    RECT client_rect;
    GetClientRect(m_target_hwnd, &client_rect);

    POINT top_left = { client_rect.left, client_rect.top };
    ClientToScreen(m_target_hwnd, &top_left);

    POINT bottom_right = { client_rect.right, client_rect.bottom };
    ClientToScreen(m_target_hwnd, &bottom_right);

    int width = bottom_right.x - top_left.x;
    int height = bottom_right.y - top_left.y;

    m_hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"Overlay", L"Overlay",
        WS_POPUP,
        top_left.x, top_left.y, width, height,
        nullptr, nullptr, wc.hInstance, this);

    if (!m_hwnd) return false;

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(m_hwnd, &margins);

    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    return true;
}

bool Overlay::InitDX11() {
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferCount = 2;
    scd.BufferDesc.Width = 0;
    scd.BufferDesc.Height = 0;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = m_hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &scd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Overlay::CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
    pBackBuffer->Release();
}

void Overlay::CleanupRenderTarget() {
    if (m_mainRenderTargetView) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

void Overlay::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_pd3dDevice, m_pd3dDeviceContext);
}

void Overlay::Cleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = nullptr; }
    if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = nullptr; }
    if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        UnregisterClass(L"Overlay", GetModuleHandle(nullptr));
        m_hwnd = nullptr;
    }
}

void Overlay::Render() {
    if (IsWindow(m_target_hwnd)) {
        RECT client_rect;
        GetClientRect(m_target_hwnd, &client_rect);

        POINT top_left = { client_rect.left, client_rect.top };
        ClientToScreen(m_target_hwnd, &top_left);

        POINT bottom_right = { client_rect.right, client_rect.bottom };
        ClientToScreen(m_target_hwnd, &bottom_right);

        int width = bottom_right.x - top_left.x;
        int height = bottom_right.y - top_left.y;

        SetWindowPos(m_hwnd, HWND_TOPMOST, top_left.x, top_left.y, width, height, SWP_NOACTIVATE);

        HWND foreground_hwnd = GetForegroundWindow();
        if (foreground_hwnd == m_target_hwnd || foreground_hwnd == m_hwnd) {
            ShowWindow(m_hwnd, SW_SHOW);
        } else {
            ShowWindow(m_hwnd, SW_HIDE);
        }

    } else {
        PostQuitMessage(0);
    }

    HWND foreground_hwnd = GetForegroundWindow();
    if (foreground_hwnd != m_target_hwnd && foreground_hwnd != m_hwnd) {
        // To reduce CPU usage when the overlay is not visible
        Sleep(100);
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_state == State::Loading) {
        RenderLoadingAnimation();
    } else {
        ImGui::ShowDemoWindow();
    }

    ImGui::Render();
    const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
    m_pd3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_pSwapChain->Present(1, 0);
}

LRESULT WINAPI Overlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Overlay* overlay = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        overlay = (Overlay*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)overlay);
    }
    else {
        overlay = (Overlay*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (overlay && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (overlay) {
        switch (msg) {
        case WM_SIZE:
            if (overlay->m_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
                overlay->CleanupRenderTarget();
                overlay->m_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                overlay->CreateRenderTarget();
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Overlay::RenderLoadingAnimation() {
    const float animation_duration = 3.0f;
    auto now = std::chrono::steady_clock::now();
    float time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time).count() / 1000.0f;
    float progress = std::min(time_elapsed / animation_duration, 1.0f);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 display_size = io.DisplaySize;

    // Darkening overlay
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), display_size, IM_COL32(0, 0, 0, (int)(progress * 128)));

    // "Loading..." text
    const char* text = "Loading...";
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImGui::GetBackgroundDrawList()->AddText(
        ImVec2((display_size.x - text_size.x) / 2, (display_size.y - text_size.y) / 2 - 20),
        IM_COL32(255, 255, 255, 255),
        text
    );

    // Progress bar
    float progress_bar_width = 200.0f;
    float progress_bar_height = 10.0f;
    ImVec2 progress_bar_pos = ImVec2((display_size.x - progress_bar_width) / 2, (display_size.y + text_size.y) / 2);
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        progress_bar_pos,
        ImVec2(progress_bar_pos.x + progress_bar_width * progress, progress_bar_pos.y + progress_bar_height),
        IM_COL32(255, 255, 255, 255)
    );

    if (progress >= 1.0f) {
        m_state = State::Running;
    }
}
