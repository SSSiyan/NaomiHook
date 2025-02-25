#include "GuiFunctions.hpp"
#include "Config.hpp"
#include "utility/Hash.hpp"
#include <string>
#include "LicenseStrings.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "glm/vec2.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#define GUI_VERSION "0.0.0"
#define IMGUI_WINDOW_PADDING 10.0f

#define IMGUI_WINDOW_BG_COLOR IM_COL32(48, 48, 48, 222) // background
#define IMGUI_WINDOW_CT_COLOR IM_COL32(255, 201, 115, 255) // header
#define IMGUI_WINDOW_IN_COLOR IM_COL32(189, 95, 88, 255) // description

#define IMGUI_WINDOW_ST_MODNAME_COLOR 0xffbfe6ff // mod title at top right

namespace gui {
    void dark_theme() {
        {
            // Deep Dark style by janekb04 from ImThemes
            ImGuiStyle& style = ImGui::GetStyle();
            style.Alpha = 1.0f;
            style.DisabledAlpha = 0.6000000238418579f;
            style.WindowPadding = ImVec2(8.0f, 8.0f);
            style.WindowBorderSize = 1.0f;
            style.WindowMinSize = ImVec2(32.0f, 32.0f);
            style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
            style.WindowMenuButtonPosition = ImGuiDir_Left;
            style.ChildBorderSize = 1.0f;
            style.PopupBorderSize = 1.0f;
            style.FramePadding = ImVec2(5.0f, 2.0f);
            style.FrameBorderSize = 1.0f;
            style.ItemSpacing = ImVec2(6.0f, 2.0f);
            style.ItemInnerSpacing = ImVec2(7.0f, 6.0f);
            style.CellPadding = ImVec2(6.0f, 6.0f);
            style.IndentSpacing = 25.0f;
            style.ColumnsMinSpacing = 6.0f;
            style.ScrollbarSize = 15.0f;
            style.GrabMinSize = 10.0f;
            style.TabBorderSize = 1.0f;
            style.TabMinWidthForCloseButton = 0.0f;
            style.ColorButtonPosition = ImGuiDir_Right;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

            style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.239999994635582f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
            style.Colors[ImGuiCol_CheckMark] = ImColor::ImColor(IMGUI_WINDOW_CT_COLOR);
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
            style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
            style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
            style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
            style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
            style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
            style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
            style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
            style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
            style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
            style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);
        }

        ImVec4* colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_WindowBg]            = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
        colors[ImGuiCol_ChildBg]             = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
        colors[ImGuiCol_PopupBg]             = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
        colors[ImGuiCol_CheckMark]           = ImVec4(0.129f, 0.184f, 0.859f, 1.00f);
        colors[ImGuiCol_TabSelected]         = ImVec4(0.51f, 0.40f, 0.24f, 1.00f);
        colors[ImGuiCol_TabSelectedOverline] = ImVec4(1.00f, 0.79f, 0.45f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected]   = ImVec4(0.42f, 0.31f, 0.14f, 1.00f);
        colors[ImGuiCol_TextLink]            = ImVec4(0.98f, 0.72f, 0.26f, 1.00f);
        colors[ImGuiCol_DragDropTarget]      = ImVec4(0.86f, 0.65f, 0.33f, 1.00f);
        colors[ImGuiCol_Header]              = ImVec4(0.31f, 0.70f, 0.15f, 1.00f);
        colors[ImGuiCol_HeaderHovered]       = ImVec4(0.31f, 0.70f, 0.15f, 1.00f);
    }

    void fps_drawing() {
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    void credits_drawing() {
        if (ImGui::BeginTabItem("About")) {
            ImGui::BeginChild("AboutChild");
            ImGui::Spacing();
            ImGui::Text("DMC4Hook - Devil May Cry 4 Trainer");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Mstislav Capusta");
            ImGui::Text("SSSiyan");
            ImGui::Spacing();
            ImGui::Text("Special Thanks:");
            ImGui::Text("socks");
            ImGui::Text("Whirling");
            ImGui::Text("Terrutas");
            ImGui::Text("Boey");
            ImGui::Text("DelusionaryKiller");
            ImGui::Text("DJMalice");
            ImGui::Text("cheburrat0r");
            ImGui::Text("endneo");
            ImGui::Text("CrazyMelody");
            ImGui::Text("Dlupx");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("For more info and updates visit the github:");

            ImGuiURL repo{ "https://github.com/muhopensores/dmc4_hook", "https://github.com/muhopensores/dmc4_hook" };
            repo.draw();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("This trainer was made using:");

            static std::array<ImGuiURL, 5> links {
                ImGuiURL { "REFramework https://github.com/praydog/REFramework", "https://github.com/praydog/REFramework" },
                ImGuiURL { "GLM https://github.com/g-truc/glm", "https://github.com/g-truc/glm"},
                ImGuiURL { "Dear ImGui https://github.com/ocornut/imgui", "https://github.com/ocornut/imgui" },
                ImGuiURL { "MinHook https://github.com/TsudaKageyu/minhook", "https://github.com/TsudaKageyu/minhook" },
                ImGuiURL { "spdlog https://github.com/gabime/spdlog", "https://github.com/gabime/spdlog" }
            };
            for (auto& link: links) {
                link.draw();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::CollapsingHeader("Licenses")) {
                ImGui::TreePush("Licenses");

                struct License {
                    std::string name;
                    std::string text;
                };

                static std::array licenses{
                    License{ "REFramework", license::reframework},
                    License{ "GLM", license::glm },
                    License{ "ImGui", license::imgui },
                    License{ "MinHook", license::minhook },
                    License{ "spdlog", license::spdlog },
                    License{ "csys", license::fmtlib },
                };

                for (const auto& license : licenses) {
                    if (ImGui::CollapsingHeader(license.name.c_str())) {
                        ImGui::TextWrapped(license.text.c_str());
                    }
                }

                ImGui::TreePop();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    // helpmarker function, to avoid typing it every time in the gui section
    void help_marker(const char* desc) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void imgui_bottom_window_proc(OurImGuiContext* ctx, Mods* pmods) {
        auto& io = ImGui::GetIO();

        glm::vec2 offset{
            ctx->main_window_rect.Min.x,
            ctx->main_window_rect.Max.y + IMGUI_WINDOW_PADDING
        };
        glm::vec2 size {
            ctx->left_window_rect.Max.x - ctx->main_window_rect.Min.x,
            io.DisplaySize.y * 0.16f
        };

        ImGui::SetNextWindowPos(offset);
        ImGui::SetNextWindowSize(size);

        bool window_open = ctx->selected_mod != nullptr;
        ImGui::Begin("Info window", &window_open, ImGuiWindowFlags_NoDecoration);
        {
            ImGui::PushFont(ctx->infobox_font);
            ImGui::PushStyleColor(ImGuiCol_Text, IMGUI_WINDOW_IN_COLOR);

            if (ctx->selected_mod) {
                ImGui::TextWrapped(ctx->selected_mod->get_description());
            }

            ImGui::PopStyleColor();
            ImGui::PopFont();
        }
        ImGui::End();
    }

    void imgui_left_window_proc(OurImGuiContext* ctx, Mods* pmods) {

        bool window_open = ctx->selected_mod;

        glm::vec2 offset { 
            ctx->main_window_rect.Max.x  + IMGUI_WINDOW_PADDING,
            ctx->main_window_rect.Min.y
        };

        glm::vec2 size = {ctx->main_window_rect.GetWidth(), ctx->main_window_rect.GetHeight()};

        ctx->left_window_rect.Min = offset;
        ctx->left_window_rect.Max = offset + size;

        ImGui::SetNextWindowPos(offset);
        ImGui::SetNextWindowSize(size);

        ImGui::Begin("Settings window", &window_open, ImGuiWindowFlags_NoDecoration);
        {

            if (ctx->selected_mod) {
                // mod name in diff font
                {
                    ImGuiRaiiFont font{ctx->modname_font};
                    ImGui::PushStyleColor(ImGuiCol_Text, IMGUI_WINDOW_ST_MODNAME_COLOR);
                    ImGui::Text("%s", ctx->selected_mod->get_human_readable_name().c_str());
                    ImGui::PopStyleColor();
                }
                ImGuiRaiiFont font{ctx->main_font};
                ctx->selected_mod->on_draw_ui();
            }

        }
        ImGui::End();

    }

    static void draw_category(OurImGuiContext* ctx, const char* category_name, ModCategory category_enum) {

        static constexpr auto entry_func = [](OurImGuiContext* ctx, Mod* mod) {
            ImColor color        = ImColor(170, 94, 88); // cheat entry colour
            ImVec2 pos           = ImGui::GetCursorPos();
            std::string btn_name = mod->get_mod_name();
            btn_name += "_btn";
            ImVec2 sz = ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing());
            if (ImGui::InvisibleButton(btn_name.c_str(), sz)) {
                ctx->selected_mod = mod;
            }
            ImGui::SetCursorPos(pos);
            if (ImGui::IsItemHovered()) {
                color = ImColor(94, 180, 88);
            }
            ImGui::TextColored(color, mod->get_human_readable_name().c_str());
        };

#if 0
        ImVec2 pos = ImGui::GetCursorPos();
        ImVec2 sz  = ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing());
        std::string btn_name{category_name};
        btn_name += "_btn";
        if (ImGui::InvisibleButton(btn_name.c_str(), sz)) {
            ctx->selected_category = category_enum;
        }
#endif
        ImGuiTreeNodeFlags flags{};
#if 1
        ImVec2 ps = ImGui::GetCursorScreenPos();
        int k = 0;
        ImVec2 sz = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight());
        ImRect rect {ps, ImVec2(ps.x + sz.x, ps.y + sz.y)};
        bool hovered = ImGui::IsMouseHoveringRect(rect.Min, rect.Max);
#endif
        if (ctx->selected_category == category_enum || hovered) {
            ImGui::PushStyleColor(ImGuiCol_Text, 0xFF222B2B);
            flags = ImGuiTreeNodeFlags_Selected;
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text,     IMGUI_WINDOW_CT_COLOR);
            ImGui::SetNextItemOpen(false, ImGuiCond_Always);
        }
        if (ImGui::TreeNodeEx(category_name, flags)) {
            ctx->selected_category = category_enum;
            ImGui::PushFont(ctx->main_font);

            for (auto& mod : g_framework->get_mods()->m_mods) {
                if (mod->get_category() != category_enum) {
                    continue;
                }
                entry_func(ctx, mod.get());
            }

            ImGui::PopFont();
            ImGui::TreePop();
        }
        ImGui::PopStyleColor(1);
    }

    void imgui_main_window_proc(OurImGuiContext* ctx, Mods* pmods, bool* window_open) {
        {
            auto& io = ImGui::GetIO();

            glm::vec2 offset{50.0f, 30.0f};
            glm::vec2 size{io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.58f};

            ctx->main_window_rect.Min = offset;
            ctx->main_window_rect.Max = offset + size;

            ImGui::SetNextWindowPos(offset);
            ImGui::SetNextWindowSize(size);

            ImGui::Begin(PROJECT_NAME " " GUI_VERSION, window_open, ImGuiWindowFlags_NoCollapse);
            {
                if (ImGui::Button("Save Config")) {
                    pmods->on_config_save();
                }
                ImGui::Spacing();

                ImGui::PushFont(ctx->fancy_font);
                draw_category(ctx, "GAMEPLAY",  ModCategory::GAMEPLAY);
                draw_category(ctx, "COSMETICS", ModCategory::COSMETICS);
                draw_category(ctx, "STAGE",     ModCategory::STAGE);
                draw_category(ctx, "SOUND",     ModCategory::SOUND);
                draw_category(ctx, "SYSTEM",    ModCategory::SYSTEM);

                ImGui::PopFont();
            }
            /*static bool showImGuiDemoWindow = false;
            ImGui::Checkbox("Show ImGui Demo Window", &showImGuiDemoWindow);
            help_marker("Check out tools>style editor to play with changes to appearance.\n"
                "Check out widgets to see if there's anything else you wanna use somewhere");
            if (showImGuiDemoWindow) {
                ImGui::ShowDemoWindow();
            }*/
            ImGui::End();
        }
    }
} // namespace gui