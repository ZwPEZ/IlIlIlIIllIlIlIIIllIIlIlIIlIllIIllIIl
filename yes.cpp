#define IMGUI_DEFINE_MATH_OPERATORS
#include "menu.hpp"
#include "globals.hpp"
#include "hooks.hpp"
#include "interfaces.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_freetype.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

#include "../framework/settings/functions.h"
#include "../framework/settings/search.h"

#include "../framework/data/fonts.h"
#include "../framework/data/images.h"

#include <stdexcept>

static WNDPROC original_wndproc = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hook_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_KEYDOWN && wparam == VK_INSERT) {
		globals::menu_opened = !globals::menu_opened;
		hooks::original_set_relative_mouse_mode(interfaces::input_system, globals::menu_opened ? false : globals::relative_mouse_mode);
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	if (globals::menu_opened) {
		return true;
	}

	return CallWindowProcA(original_wndproc, hwnd, msg, wparam, lparam);
}

namespace menu {
	void create() {
		if (!interfaces::d3d11_device || !interfaces::d3d11_device_context || !interfaces::hwnd) {
			throw std::runtime_error("interfaces not initialized");
		}

		original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(interfaces::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hook_wndproc)));

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.IniFilename = NULL;

		ImFontConfig cfg;
		cfg.FontDataOwnedByAtlas = false;
		cfg.PixelSnapH = true;
		cfg.OversampleH = cfg.OversampleV = 1;
		cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags_Bitmap;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x052F,
			0x2DE0, 0x2DFF,
			0xA640, 0xA69F,
			0x3131, 0x3163,
			0xAC00, 0xD79D,
			0
		};

		{
			var->font.instrument_bold[0] = io.Fonts->AddFontFromMemoryTTF(instrument_sans_bold, sizeof(instrument_sans_bold), 11.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->font.instrument_medium[0] = io.Fonts->AddFontFromMemoryTTF(instrument_sans_medium, sizeof(instrument_sans_medium), 11.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.instrument_medium[1] = io.Fonts->AddFontFromMemoryTTF(instrument_sans_medium, sizeof(instrument_sans_medium), 10.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.instrument_medium[2] = io.Fonts->AddFontFromMemoryTTF(instrument_sans_medium, sizeof(instrument_sans_medium), 12.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->font.icons[0] = io.Fonts->AddFontFromMemoryTTF(icons, sizeof(icons), 11.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icons[1] = io.Fonts->AddFontFromMemoryTTF(icons, sizeof(icons), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icons[2] = io.Fonts->AddFontFromMemoryTTF(icons, sizeof(icons), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icons[3] = io.Fonts->AddFontFromMemoryTTF(icons, sizeof(icons), 12.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
			var->font.icons[4] = io.Fonts->AddFontFromMemoryTTF(icons, sizeof(icons), 10.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->font.code = io.Fonts->AddFontFromMemoryTTF(code_font_data, sizeof(code_font_data), 10.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

			var->font.pixel[0] = io.Fonts->AddFontFromMemoryTTF(_smallest_pixel, sizeof(_smallest_pixel), 10.0f, &cfg, ranges);
			var->font.pixel[1] = io.Fonts->AddFontFromMemoryTTF(_smallest_pixel, sizeof(_smallest_pixel), 11.0f, &cfg, ranges);
			var->font.pixel[2] = io.Fonts->AddFontFromMemoryTTF(_smallest_pixel, sizeof(_smallest_pixel), 12.0f, &cfg, ranges);
            var->font.pixel[3] = io.Fonts->AddFontFromMemoryTTF(_smallest_pixel, sizeof(_smallest_pixel), 14.0f, &cfg, ranges);

			var->font.weapon_icons[0] = io.Fonts->AddFontFromMemoryTTF(_weapon_icons, sizeof(_weapon_icons), 10.0f, &cfg, ranges);
			var->font.weapon_icons[1] = io.Fonts->AddFontFromMemoryTTF(_weapon_icons, sizeof(_weapon_icons), 11.0f, &cfg, ranges);
			var->font.weapon_icons[2] = io.Fonts->AddFontFromMemoryTTF(_weapon_icons, sizeof(_weapon_icons), 12.0f, &cfg, ranges);
		}

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(interfaces::hwnd);
		ImGui_ImplDX11_Init(interfaces::d3d11_device, interfaces::d3d11_device_context);
	}

	void destroy() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		if (original_wndproc) {
			SetWindowLongPtrA(interfaces::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wndproc));
			original_wndproc = nullptr;
		}
	}

	void renderWatermark() {
        if (!Settings::Watermark) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        int FPS = static_cast<int>(io.Framerate);

        time_t now = time(NULL);
        tm ltStruct;
        localtime_s(&ltStruct, &now);
        tm* lt = &ltStruct;

        char timeBuf[32];
        strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", lt);

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        ImVec2 pos(10, 5);

        ImFont* font = var->font.pixel[3];

        ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
        ImU32 whiteColor = IM_COL32(255, 255, 255, 255);
        ImVec4 accentVec = ImVec4(var->gui.accent_clr[0], var->gui.accent_clr[1], var->gui.accent_clr[2], 1.f);
        ImU32 accentColor = ImColor(accentVec);

        const ImVec2 offsets[8] = {
            ImVec2(-1,-1), ImVec2(0,-1), ImVec2(1,-1),
            ImVec2(-1,0),              ImVec2(1,0),
            ImVec2(-1,1), ImVec2(0,1), ImVec2(1,1)
        };

        const char* part1 = "Nixius";
        const char* part2 = ".xyz";
        char buffer[128];
        snprintf(buffer, sizeof(buffer), " | %s | FPS: %d", timeBuf, FPS);

        ImVec2 cursor = pos;

        for (int i = 0; i < 8; i++)
            draw->AddText(font, font->FontSize, ImVec2(cursor.x + offsets[i].x, cursor.y + offsets[i].y), outlineColor, part1);
        draw->AddText(font, font->FontSize, cursor, whiteColor, part1);
        cursor.x += font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, part1).x;

        for (int i = 0; i < 8; i++)
            draw->AddText(font, font->FontSize, ImVec2(cursor.x + offsets[i].x, cursor.y + offsets[i].y), outlineColor, part2);
        draw->AddText(font, font->FontSize, cursor, accentColor, part2);
        cursor.x += font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, part2).x;

        for (int i = 0; i < 8; i++)
            draw->AddText(font, font->FontSize, ImVec2(cursor.x + offsets[i].x, cursor.y + offsets[i].y), outlineColor, buffer);
        draw->AddText(font, font->FontSize, cursor, whiteColor, buffer);
    }

	void render() {
		if (!globals::menu_opened) {
			return;
		}

        search->init_options();

        static ImVec2 window_size = SCALE(ImVec2(785, 485));
        gui->set_next_window_size(window_size);
        ImGui::SetNextWindowPos(ImVec2((GetSystemMetrics(SM_CXSCREEN) - window_size.x) * 0.5f, (GetSystemMetrics(SM_CYSCREEN) - window_size.y) * 0.5f), ImGuiCond_Once);

        gui->begin();
        {
            gui->set_style();
            gui->draw_decorations();

            clr->base_colors.accent_clr = ImVec4(var->gui.accent_clr[0], var->gui.accent_clr[1], var->gui.accent_clr[2], 1.f);

            const ImVec2 pos = gui->window_pos();
            const ImVec2 size = gui->window_size();
            ImDrawList* drawlist = GetWindowDrawList();

            gui->begin_content("section", ImVec2(SCALE(165), gui->content_avail().y), SCALE(0, 0), SCALE(0, 0));
            {
                gui->set_pos(SCALE(50), pos_y);

                for (int i = 0; i < elements->section.section_list[0].size(); i++)
                {
                    switch (i)
                    {
                    case 0:
                        widgets->category(elements->category.category_list[0]);
                        break;
                    case 2:
                        widgets->category(elements->category.category_list[1]);
                        break;
                    }

                    widgets->section(elements->section.section_list[0][i].data(), elements->section.section_list[1][i].data(), i, elements->section.section_count);
                }
            }
            gui->end_content();

            gui->sameline();

            gui->begin_group();
            {
                gui->begin_content("header", ImVec2(gui->content_avail().x, SCALE(50.f)), SCALE(10, 10), SCALE(10, 10), ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
                gui->push_var(ImGuiStyleVar_Alpha, elements->section.section_alpha * GetStyle().Alpha);
                {
                    widgets->text_field(ICON_SEARCH, "", "Search type ...", search->search_buf, IM_ARRAYSIZE(search->search_buf), &search->active_searcing, SCALE(200, 30));
                    search->search_element = search->search_buf;
                }
                gui->pop_var(1);

                gui->sameline(0, gui->content_avail().x - SCALE(230));

                if (widgets->settings_button("settings", elements->section.section_count == 7, SCALE(30, 30)))
                {
                    elements->section.section_count = 7;
                };
                gui->end_content();

                gui->easing(elements->section.section_alpha, elements->section.section_count == elements->section.section_count_active ? 1.f : 0.f, 6.f, static_easing);
                gui->easing(elements->section.sub_section_alpha, elements->section.sub_section_count == elements->section.sub_section_count_active ? 1.f : 0.f, 6.f, static_easing);
                if (elements->section.section_alpha == 0.f && elements->section.section_add == 0.f) elements->section.section_count_active = elements->section.section_count;
                if (elements->section.sub_section_alpha == 0.f) elements->section.sub_section_count_active = elements->section.sub_section_count;

                gui->push_var(ImGuiStyleVar_Alpha, elements->section.section_alpha * elements->section.sub_section_alpha * GetStyle().Alpha);
                gui->begin_content("content", elements->section.section_count_active == 8 ? SCALE(0, 0) : gui->content_avail(), SCALE(10, 10), SCALE(10, 10));
                if (elements->section.section_count_active == 7)
                {
                    gui->begin_group();
                    {
                        gui->begin_child("Menu", ICON_GEAR);
                        {
                            widgets->color_picker("Accent Color", var->gui.accent_clr, false);
                            if (widgets->button("Unload Menu")) { globals::should_exit = true; };
                        }
                        gui->end_child();
                    }
                    gui->end_group();

                    gui->sameline();

                    gui->begin_group();
                    {
                        gui->begin_child("Overlay Elements", ICON_GEAR);
                        {
                            widgets->begin_list("LISTBOX", ImVec2(gui->content_avail().x, 0));
                            {
                                static std::vector<multi_combo> containers = {
                                    { true, "Watermark" },
                                };

                                for (int i = 0; i < containers.size(); i++)
                                {
                                    if (widgets->list_content(containers[i].name.data(), containers[i].active))
                                    {
                                        containers[i].active = !containers[i].active;

                                        if (containers[i].name == "Watermark")
                                        {
                                            Settings::Watermark = containers[i].active;
                                        }
                                    }
                                }
                            }
                            widgets->end_list();
                        }
                        gui->end_child();
                    }
                    gui->end_group();
                }
                gui->end_content();
                gui->pop_var();

                gui->easing(search->window_alpha, search->active_searcing ? 1.f : 0.f, 8.f, static_easing);

                if (search->window_alpha >= 0.01f)
                {
                    gui->set_pos(ImVec2(0, 0), pos_all);
                    gui->push_var(ImGuiStyleVar_Alpha, search->window_alpha);
                    gui->begin_def_child("search__window_bg", GetWindowSize(), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
                    {
                        draw->rect_filled(GetWindowDrawList(), GetWindowPos(), ImVec2(GetWindowPos().x + GetWindowSize().x, GetWindowPos().y + GetWindowSize().y), draw->get_clr({ 0.f, 0.f, 0.f, 1.f }, 0.7f), 4);

                        gui->set_pos(SCALE(175, 60), pos_all);
                        gui->push_var(ImGuiStyleVar_WindowPadding, SCALE(elements->child.child_padding));
                        gui->push_var(ImGuiStyleVar_ItemSpacing, SCALE(elements->child.child_spacing));
                        gui->push_var(ImGuiStyleVar_ChildBorderSize, 0);
                        gui->begin_def_child("search_window", SCALE(585, 300), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
                        {
                            draw->rect_filled(GetWindowDrawList(), GetWindowPos(), ImVec2(GetWindowPos().x + GetWindowSize().x, GetWindowPos().y + GetWindowSize().y), draw->get_clr(clr->child.child_layout), SCALE(elements->child.rounding));
                            draw->rect(GetWindowDrawList(), GetWindowPos(), ImVec2(GetWindowPos().x + GetWindowSize().x, GetWindowPos().y + GetWindowSize().y), draw->get_clr(clr->child.child_stroke), SCALE(elements->child.rounding), 0, SCALE(1));

                            search->search();
                        }
                        gui->end_def_child();
                        gui->pop_var(3);
                    }
                    gui->end_def_child();
                    gui->pop_var();
                }

            }
            gui->end_group();
        }
        gui->end();
	}
}