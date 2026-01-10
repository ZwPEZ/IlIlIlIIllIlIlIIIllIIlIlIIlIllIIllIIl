#include <dwmapi.h>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "imgui/fonts/poppins_extrabold.h"
#include "imgui/fonts/poppins_bold.h"
#include "imgui/fonts/poppins_semibold.h"
#include "imgui/fonts/poppins_regular.h"

#include "imgui/icons/eye_icon.h"

#include "overlay.h"
#include "custom.h"
#include "helpers.hpp"
#include "Settings/settings.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

Overlay::Overlay() {
    m_target_hwnd = FindWindow(nullptr, L"Roblox");
    m_window_name = GetRandomOverlayName();
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
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, m_window_name.c_str(), nullptr };
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
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        m_window_name.c_str(),
        m_window_name.c_str(),
        WS_POPUP,
        top_left.x, top_left.y, width, height,
        nullptr, nullptr, wc.hInstance, this);

    if (!m_hwnd) return false;

    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

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
    ID3D11Texture2D* pBackBuffer = nullptr;

    HRESULT hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr) || !pBackBuffer) {
        m_mainRenderTargetView = nullptr;
        return;
    }

    hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr)) {
        m_mainRenderTargetView = nullptr;
    }
}

void Overlay::CleanupRenderTarget() {
    if (m_mainRenderTargetView) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

void Overlay::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = NULL;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameRounding = 5.0f;
    style.FrameBorderSize = 1.0f;

    style.Colors[ImGuiCol_WindowBg] = ImVec4(15.0f / 255.0f, 15.0f / 255.0f, 15.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_pd3dDevice, m_pd3dDeviceContext);

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    cfg.PixelSnapH = true;
    cfg.OversampleH = cfg.OversampleV = 1;

    m_poppins_semibold = io.Fonts->AddFontFromMemoryTTF(poppins_semibold, sizeof(poppins_semibold), 20.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    m_poppins_regular = io.Fonts->AddFontFromMemoryTTF(poppins_regular, sizeof(poppins_regular), 20.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    m_poppins_bold = io.Fonts->AddFontFromMemoryTTF(poppins_bold, sizeof(poppins_bold), 20.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    m_poppins_extrabold = io.Fonts->AddFontFromMemoryTTF(poppins_extrabold, sizeof(poppins_extrabold), 20.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    LoadTextureFromMemory(m_pd3dDevice, eye_icon_data, sizeof(eye_icon_data), &m_eye_icon_texture);
    m_tab_icons.push_back(m_eye_icon_texture);
}

void Overlay::Cleanup() {
    if (m_eye_icon_texture) {
        m_eye_icon_texture->Release();
        m_eye_icon_texture = nullptr;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = nullptr; }
    if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = nullptr; }
    if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        UnregisterClass(m_window_name.c_str(), GetModuleHandle(nullptr));
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
        }
        else {
            ShowWindow(m_hwnd, SW_HIDE);
        }
    }
    else {
        PostQuitMessage(0);
    }

    HWND foreground_hwnd = GetForegroundWindow();
    if (foreground_hwnd != m_target_hwnd && foreground_hwnd != m_hwnd) {
        Sleep(100);
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_state == State::Loading) {
        RenderLoadingAnimation();
    }
    else {
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            NoSave::menuOpened = !NoSave::menuOpened;
        }

        if (NoSave::debugMode) {
            ImGui::ShowDemoWindow();
        }

        if (NoSave::menuOpened) {
            RenderMenu();
        }
    }

    ImGui::Render();
    const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
    m_pd3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    LONG exStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
    if (NoSave::menuOpened)
        SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
    else
        SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);

    m_pSwapChain->Present(1, 0);
}

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

        ImU32 accentColor = ImGui::ColorConvertFloat4ToU32(ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f));
        ImU32 topColor = IM_COL32(static_cast<int>(Theme::Accent[0] * 255), static_cast<int>(Theme::Accent[1] * 255), static_cast<int>(Theme::Accent[2] * 255), 50);
        ImU32 topTransparent = IM_COL32(static_cast<int>(Theme::Accent[0] * 255), static_cast<int>(Theme::Accent[1] * 255), static_cast<int>(Theme::Accent[2] * 255), 0);

        ImGui::GetWindowDrawList()->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + headerHeight), IM_COL32(22, 22, 22, 255), 9.0f, ImDrawFlags_RoundCornersTop);

        const char* titleMain = "Flawless";
        const char* titleAccent = ".win";

        ImVec2 mainSize = ImGui::CalcTextSize(titleMain);
        ImVec2 accentSize = ImGui::CalcTextSize(titleAccent);

        float textY = winPos.y + (headerHeight - mainSize.y) * 0.5f;
        float textX = winPos.x + 15.0f;
        const int slices = 130;

        ImVec4 topColMain = ImVec4(220.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f);
        ImVec4 botColMain = ImVec4(150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f, 1.0f);
        ImVec4 topCol = ImVec4(Theme::Accent[0], Theme::Accent[1], Theme::Accent[2], 1.0f );
        ImVec4 botCol = ImVec4(Theme::Accent[0] * 0.7f, Theme::Accent[1] * 0.7f, Theme::Accent[2] * 0.7f, 1.0f);

        float slice_h_main = mainSize.y / slices;

        for (int i = 0; i < slices; i++)
        {
            float t = (float)i / (slices - 1);
            ImVec4 col;
            col.x = topColMain.x + (botColMain.x - topColMain.x) * t;
            col.y = topColMain.y + (botColMain.y - topColMain.y) * t;
            col.z = topColMain.z + (botColMain.z - topColMain.z) * t;
            col.w = topColMain.w + (botColMain.w - topColMain.w) * t;

            ImU32 color = ImGui::GetColorU32(col);

            ImVec2 clip_min(textX, textY + slice_h_main * i);
            ImVec2 clip_max(textX + mainSize.x, clip_min.y + slice_h_main);

            ImGui::GetWindowDrawList()->PushClipRect(clip_min, clip_max, true);
            ImGui::GetWindowDrawList()->AddText(ImVec2(textX, textY), color, titleMain);
            ImGui::GetWindowDrawList()->PopClipRect();
        }

        float slice_h_accent = accentSize.y / slices;
        float accentX = textX + mainSize.x;

        for (int i = 0; i < slices; i++)
        {
            float t = (float)i / (slices - 1);
            ImVec4 col;
            col.x = topCol.x + (botCol.x - topCol.x) * t;
            col.y = topCol.y + (botCol.y - topCol.y) * t;
            col.z = topCol.z + (botCol.z - topCol.z) * t;
            col.w = topCol.w + (botCol.w - topCol.w) * t;

            ImU32 color = ImGui::GetColorU32(col);

            ImVec2 clip_min(accentX, textY + slice_h_accent * i);
            ImVec2 clip_max(accentX + accentSize.x, clip_min.y + slice_h_accent);

            ImGui::GetWindowDrawList()->PushClipRect(clip_min, clip_max, true);
            ImGui::GetWindowDrawList()->AddText(ImVec2(accentX, textY), color, titleAccent);
            ImGui::GetWindowDrawList()->PopClipRect();
        }

        ImGui::GetWindowDrawList()->AddLine(ImVec2(winPos.x, winPos.y + headerHeight), ImVec2(winPos.x + winSize.x, winPos.y + headerHeight), accentColor, 1.0f);
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(winPos.x, winPos.y + headerHeight), ImVec2(winPos.x + winSize.x, winPos.y + headerHeight + 45), topColor, topColor, topTransparent, topTransparent);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y), IM_COL32(22, 22, 22, 255), 9.0f, ImDrawFlags_RoundCornersBottom);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y - footerHeight), accentColor, 1.0f);
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(winPos.x, winPos.y + winSize.y - footerHeight - 45), ImVec2(winPos.x + winSize.x, winPos.y + winSize.y - footerHeight), topTransparent, topTransparent, topColor, topColor);

        ImGui::Dummy(ImVec2(0, headerHeight));

        // Content
        if (m_selected_tab == 0) {
            ImGui::Text("Visuals Tab Content");
        }

        ImGui::SetCursorPosY(winSize.y - footerHeight);
        Custom::RenderTabs(m_selected_tab, m_tab_icons);

        ImGui::End();
    }
}

void Overlay::RenderLoadingAnimation() {
    const float bg_fade_time = 0.6f;
    const float drop_time = 1.7f;
    const float start_offset_y = 35.0f;
    const float dot_fade_time = 0.8f;
    const float shift_amount = 25.0f;
    const float fade_out_time = 0.2f;
    const float text_fade_in_time = 1.0f;

    auto now = std::chrono::steady_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time).count() / 1000.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screen = io.DisplaySize;

    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    ImDrawList* fg = ImGui::GetForegroundDrawList();

    float bg_t = (std::min)(time / bg_fade_time, 1.0f);
    float bg_alpha = EaseOutCubic(bg_t) * 160.0f;
    bg->AddRectFilled(ImVec2(0, 0), screen, IM_COL32(0, 0, 0, (int)bg_alpha));

    float anim_time = (std::min)(time / drop_time, 1.0f);
    float anim = EaseOutCubic(anim_time);

    float y_offset = (1.0f - anim) * start_offset_y;
    float alpha = anim;

    float fade_in_alpha = 1.0f;
    if (time < text_fade_in_time) {
        float fade_t = (std::min)(time / text_fade_in_time, 1.0f);
        fade_in_alpha = EaseOutCubic(fade_t);
    }

    const char* title = "FLAWLESS";
    ImFont* font = ImGui::GetFont();
    float font_size = 125.0f;

    ImVec2 size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, title);

    ImVec2 pos(screen.x * 0.5f - size.x * 0.5f, screen.y * 0.5f - size.y * 0.5f - y_offset);

    float dot_anim = 0.0f;
    if (anim_time >= 1.0f) {
        float dot_time = (time - drop_time) / dot_fade_time;
        dot_time = (std::min)((std::max)(dot_time, 0.0f), 1.0f);
        dot_anim = EaseOutCubic(dot_time);

        pos.x -= shift_amount * dot_anim;
    }

    float fade_out_alpha = 1.0f;
    if (anim_time >= 1.0f) {
        float time_since_dot = time - drop_time - dot_fade_time;
        if (time_since_dot >= 0.0f) {
            float fade_t = (std::min)(time_since_dot / fade_out_time, 1.0f);
            fade_out_alpha = 1.0f - EaseOutCubic(fade_t);
        }
    }

    float text_alpha = alpha * fade_in_alpha * fade_out_alpha;

    const int slices = 150;
    float slice_h = size.y / slices;
    for (int i = 0; i < slices; i++) {
        float g = (float)i / (slices - 1);
        ImU8 col = static_cast<ImU8>(200.0f - g * 120.0f);
        ImU32 color = IM_COL32(col, col, col, (int)(255 * text_alpha));

        ImVec2 clip_min(pos.x, pos.y + slice_h * i);
        ImVec2 clip_max(pos.x + size.x, clip_min.y + slice_h);

        fg->PushClipRect(clip_min, clip_max, true);

        fg->AddText(font, font_size, pos, color, title);
        fg->AddText(font, font_size, ImVec2(pos.x + 1.0f, pos.y), color, title);
        fg->AddText(font, font_size, ImVec2(pos.x, pos.y + 1.0f), color, title);

        fg->PopClipRect();
    }

    if (dot_anim > 0.0f) {
        const char* dot = ".win";
        float dot_font_size = font_size / 2.0f;

        ImVec2 dot_size = font->CalcTextSizeA(dot_font_size, FLT_MAX, 0.0f, dot);

        ImVec2 dot_pos(
            pos.x + size.x + 8.0f * (1.0f - dot_anim),
            pos.y + size.y - dot_size.y - 15.5f
        );

        const int dot_slices = 150;
        float dot_slice_h = dot_size.y / dot_slices;

        for (int i = 0; i < dot_slices; i++) {
            float g = (float)i / (dot_slices - 1);
            ImU8 r = static_cast<ImU8>(Theme::Accent[0] * 255.0f - g * 135.0f);
            ImU8 gr = static_cast<ImU8>(Theme::Accent[1] * 255.0f - g * 50.0f);
            ImU8 b = static_cast<ImU8>(Theme::Accent[2] * 255.0f - g * 50.0f);
            ImU32 color = IM_COL32(r, gr, b, (int)(255 * dot_anim * text_alpha));

            ImVec2 clip_min(dot_pos.x, dot_pos.y + dot_slice_h * i);
            ImVec2 clip_max(dot_pos.x + dot_size.x, clip_min.y + dot_slice_h);

            fg->PushClipRect(clip_min, clip_max, true);
            fg->AddText(font, dot_font_size, dot_pos, color, dot);
            fg->PopClipRect();
        }
    }

    if (anim_time >= 1.0f && (time - drop_time) >= dot_fade_time + fade_out_time)
        m_state = State::Running;
}
