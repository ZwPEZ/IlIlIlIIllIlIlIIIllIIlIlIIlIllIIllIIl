#define IMGUI_DEFINE_MATH_OPERATORS

#include <dwmapi.h>
#include <algorithm>
#include <chrono>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/custom_popup.hpp"
#include "imgui/custom_widgets.hpp"
#include "imgui/SatoshiFont.hpp"
#include "imgui/font.h"
#include "imgui/image.h"

#include "overlay.h"
#include "helpers.hpp"
#include "../settings/settings.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Static pointer for the high-resolution loading font
static ImFont* s_loading_font = nullptr;

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
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, m_window_name.c_str(), nullptr };
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

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_pd3dDevice, m_pd3dDeviceContext);

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    cfg.PixelSnapH = true;
    cfg.OversampleH = cfg.OversampleV = 1;

    io.Fonts->AddFontFromMemoryTTF(SatoshiMedium, sizeof(SatoshiMedium), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    static ImWchar icomoon_ranges[] = { 0x1, 0x10FFFD, 0 };

    static ImFontConfig icomoon_config;
    icomoon_config.OversampleH = icomoon_config.OversampleV = 1;
    icomoon_config.MergeMode = true;
    icomoon_config.GlyphOffset.y = 2;

    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges);
    font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF(SatoshiBold, sizeof(SatoshiBold), 19.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 20.f, &icomoon_config, icomoon_ranges);
    font::bold_small = io.Fonts->AddFontFromMemoryTTF(SatoshiBold, sizeof(SatoshiBold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 17.f, &icomoon_config, icomoon_ranges);
    font::medium_small = io.Fonts->AddFontFromMemoryTTF(SatoshiMedium, sizeof(SatoshiMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 16.f, &icomoon_config, icomoon_ranges);
    font::inter_bold = io.Fonts->AddFontFromMemoryTTF(SatoshiBold, sizeof(SatoshiBold), 18.5f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 19.f, &icomoon_config, icomoon_ranges);
    font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(SatoshiBold, sizeof(SatoshiBold), 35.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_regular = io.Fonts->AddFontFromMemoryTTF(SatoshiMedium, sizeof(SatoshiMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_medium = io.Fonts->AddFontFromMemoryTTF(SatoshiMedium, sizeof(SatoshiMedium), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 20.f, &icomoon_config, icomoon_ranges);
    font::inter_medium2 = io.Fonts->AddFontFromMemoryTTF(SatoshiMedium, sizeof(SatoshiMedium), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    // FIX: Changed ranges from Cyrillic to icomoon_ranges for all icon-specific fonts
    font::icomoon_page = io.Fonts->AddFontFromMemoryTTF(icomoon_page, sizeof(icomoon_page), 17.f, &cfg, icomoon_ranges);
    font::icomoon_childs = io.Fonts->AddFontFromMemoryTTF(icomoon_page, sizeof(icomoon_page), 27.f, &cfg, icomoon_ranges);
    font::icomoon_logo = io.Fonts->AddFontFromMemoryTTF(icomoon_page, sizeof(icomoon_page), 30.f, &cfg, icomoon_ranges);

    icomoon_config.GlyphOffset.y = 0;
    font::icomoon_tabs = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 25.f, &cfg, icomoon_ranges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 25.f, &icomoon_config, icomoon_ranges);

    font::icon_child = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 30.f, &cfg, icomoon_ranges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 30.f, &icomoon_config, icomoon_ranges);

    // NEW: Load a high-resolution font specifically for the loading animation to avoid scaling blur
    s_loading_font = io.Fonts->AddFontFromMemoryTTF(SatoshiBold, sizeof(SatoshiBold), 120.f, &cfg, io.Fonts->GetGlyphRangesDefault());

    ImGuiStyle& s = ImGui::GetStyle();
    s.FramePadding = ImVec2(10, 10);
    s.ItemSpacing = ImVec2(0, 0);
    s.FrameRounding = 2.f;
    s.WindowRounding = 10.f;
    s.WindowBorderSize = 0.f;
    s.PopupBorderSize = 0.f;
    s.WindowPadding = ImVec2(10, 10);
    s.ChildBorderSize = 1.f;
    s.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
    s.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
    s.WindowShadowSize = 0;
    s.PopupRounding = 5.f;
    s.ScrollbarSize = 1;
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
            if (!NoSave::menuOpened) {
                NoSave::menuOpened = true;
            }
            else {
                NoSave::menuOpened = false;
            }
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

    LONG exStyle = GetWindowLong(Overlay::m_hwnd, GWL_EXSTYLE);
    if (NoSave::menuOpened)
        SetWindowLong(Overlay::m_hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
    else
        SetWindowLong(Overlay::m_hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);

    m_pSwapChain->Present(1, 0);
}

void Overlay::RenderMenu()
{
    // Begin ImGui window
    Begin("imgui menu", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground);

    {
        // Animation speed based on frame time
        c::anim::speed = ImGui::GetIO().DeltaTime * 12.f;

        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        // Render background particle effects
        particle::RenderEffects(draw_list, display_size, 24.0f);

        // Tab logic
        static int iTabs = 0;
        fTabOffset = ImLerp(fTabOffset, bTabState ? 700.f : 0.f, GetAnimSpeed());

        if (fTabOffset > 695.f && bTabState) {
            iTabs = iTabTarget;
            bTabState = false;
        }

        const ImVec2& pos = ImGui::GetWindowPos();
        const ImVec2& region = ImGui::GetContentRegionMax();
        glow_text_drawlist = GetWindowDrawList();

        ImGui::SetScrollFromPosY(0);

        // ---- Sidebar Tabs ----
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 8));
        ImGui::SetCursorPos(ImVec2(12.5f, 87.5f + ImGui::GetScrollY()));
        ImGui::BeginChild("Tabs", ImVec2(160, c::bg::size.y - 80), false);

        const char* tab_list[] = { "Combat", "World", "ESP", "Tracking", "Dashboard", "Prefer", "Logic", "Entities", "Navigation" };
        const char* tab_ico_list[] = { ICON_AIMING_LINE, ICON_EARTH_2_LINE, ICON_EYE_2_LINE, ICON_ROUTE_LINE,
                                       ICON_DASHBOARD_3_LINE, ICON_ANGEL_LINE, ICON_PUZZLED_LINE, ICON_USER_1_LINE,
                                       ICON_COMPASS_2_LINE };

        for (int i = 0; i < IM_ARRAYSIZE(tab_list); i++)
            custom::Tab(tab_list[i], tab_ico_list[i], &iTabs, i);

        ImGui::EndChild(false);
        ImGui::PopStyleVar();

        // ---- SubTabs ----
        static int iSubTabs = 0;
        ImGui::SetCursorPos(ImVec2(180.f, 17.5f));
        ImGui::BeginChild("SubTabs", ImVec2(ImGui::GetContentRegionAvail().x, 50), false);

        custom::SubTab("Enemies", &iSubTabs, 0); ImGui::SameLine();
        custom::SubTab("Teammates", &iSubTabs, 1); ImGui::SameLine();
        custom::SubTab("Self", &iSubTabs, 2);

        ImGui::EndChild(false);

        // ---- Main Tab Content ----
        custom::chroma rainbow;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

        if (iTabs == 0) {
            ImGui::SetCursorPos(ImVec2(560, 85 + fTabOffset));
            ImGui::BeginChild("Main#2", ImVec2(370, 540), false, ImGuiWindowFlags_NoScrollbar);

            custom::Child("Visual Customization", ICON_PALETTE_LINE, "Customize UI colors", ImVec2(370, 210), true, 0);

            std::string button_name = std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL) +
                " Change color theme" +
                std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL);

            if (custom::Button(button_name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 35)))
                bTheme = !bTheme;

            custom::ColorEditor("Primary Color", &c::main_color);
            custom::ColorEditor("Secondary Color", &c::accent_color);

            custom::EndChild();
            ImGui::EndChild(false);
        }

        ImGui::PopStyleVar();

        // ---- Background ----
        draw_background_blur(GetBackgroundDrawList(), m_pSwapChain, m_pd3dDevice, m_pd3dDeviceContext, pos, pos + c::bg::size, 1.f);

        ImGui::PushClipRect(ImVec2(0, 0), ImVec2(4000, 4000), false);

        auto bg = GetBackgroundDrawList();
        bg->AddRectFilled(pos, pos + c::bg::size, c::window_bg_color, c::bg::rounding);
        bg->AddRectFilled(pos + ImVec2(160, 0), pos + ImVec2(c::bg::size.x, 75),
            utils::GetColorWithAlpha(c::child::background, c::child::background.w / 2),
            c::bg::rounding, ImDrawFlags_RoundCornersTop);
        bg->AddRectFilled(pos, pos + ImVec2(170, c::bg::size.y), ImGui::GetColorU32(c::child::background),
            c::bg::rounding, ImDrawFlags_RoundCornersLeft);
        bg->AddRectFilled(pos + ImVec2(0, 75), pos + ImVec2(c::bg::size.x, 76), ImGui::GetColorU32(c::child::stroke),
            c::bg::rounding);
        bg->AddRectFilled(pos + ImVec2(170, 75), pos + ImVec2(171, c::bg::size.y), ImGui::GetColorU32(c::child::stroke),
            c::bg::rounding);

        ImGui::PopClipRect();

        // ---- Title Text ----
        PushFont(font::inter_semibold);
        rainbow.RenderText(utils::center_text(pos, pos + ImVec2(160, 75), "Flawless.win"), "Flawless.win", 1.f, 1.f);
        PopFont();
    }

    End();
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
    // FIX: Use the dedicated high-resolution font to ensure text is crisp at large sizes
    ImFont* font = s_loading_font ? s_loading_font : ImGui::GetFont();
    float font_size = 120.0f;

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

    // FIX: Reduced number of slices to 60 for efficiency and to avoid sub-pixel artifacts.
    // Also removed redundant AddText calls that were smearing/blurring the text.
    const int slices = 60;
    float slice_h = size.y / slices;
    for (int i = 0; i < slices; i++) {
        float g = (float)i / (slices - 1);
        ImU8 col = static_cast<ImU8>(200.0f - g * 120.0f);
        ImU32 color = IM_COL32(col, col, col, (int)(255 * text_alpha));

        ImVec2 clip_min(pos.x, pos.y + slice_h * i);
        ImVec2 clip_max(pos.x + size.x, clip_min.y + slice_h);

        fg->PushClipRect(clip_min, clip_max, true);

        fg->AddText(font, font_size, pos, color, title);

        fg->PopClipRect();
    }

    if (dot_anim > 0.0f) {
        const char* dot = ".win";
        float dot_font_size = font_size / 2.0f;

        ImVec2 dot_size = font->CalcTextSizeA(dot_font_size, FLT_MAX, 0.0f, dot);

        ImVec2 dot_pos(
            pos.x + size.x + 8.0f * (1.0f - dot_anim),
            pos.y + size.y - dot_size.y - 15.0f
        );

        const int dot_slices = 60;
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
