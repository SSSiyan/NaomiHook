#pragma once

#include "Mods.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Shellapi.h"
#include "glm/glm.hpp"


    enum IMCTX_WINFLAGS : int {
        NONE_OPEN = 0,
        MAIN_OPEN = 1,
        LEFT_OPEN = 2,
        BOTM_OPTN = 4,
    };

    struct AnimValue {
        ImRect a, b;
    };

    struct Keyframe {

        float time_start, time_end {};

        float get_value(float time) const {
            return glm::lerp(0.0f, 1.0f, glm::smoothstep(time_start,time_end, time));
        }
    };

    struct ImGuiRaiiFont {
        explicit ImGuiRaiiFont(ImFont* font) {
            ImGui::PushFont(font);
        };
        ~ImGuiRaiiFont() noexcept {
            ImGui::PopFont();
        }
        // no copies
        ImGuiRaiiFont(const ImGuiRaiiFont&) = delete;
        ImGuiRaiiFont& operator=(const ImGuiRaiiFont&) = delete;
        // no moves
        ImGuiRaiiFont(ImGuiRaiiFont&&) = delete;
        ImGuiRaiiFont& operator=(ImGuiRaiiFont&&) = delete;
    };

    struct OurImGuiContext {
        int wflags{};

        Mod* selected_mod{nullptr};

        ImRect main_window_rect {};
        ImRect left_window_rect {};

        ImFont* main_font{};
        ImFont* fancy_font{};
        ImFont* infobox_font{};
        ImFont* modname_font{};

        ModCategory selected_category{};

        Keyframe main_window_anim { 
            .time_start = 0.0f,
            .time_end   = 2.2f,
        };
        Keyframe left_window_anim {
            .time_start = 1.1f,
            .time_end   = 3.2f,
        };
        Keyframe bottom_window_anim {
            .time_start = 1.3f,
            .time_end   = 3.2f
        };
    };

namespace gui {
    //gui function definitions
    inline void under_line(const ImColor& col) {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, col, 1.0f);
    }
    void dark_theme();

    //helpmarker function, to avoid typing it every time in the gui section
    void help_marker(const char* desc);

    void imgui_main_window_proc  (OurImGuiContext* ctx, Mods* pmods, bool* window_open);
    void imgui_left_window_proc  (OurImGuiContext* ctx, Mods* pmods);
    void imgui_bottom_window_proc(OurImGuiContext* ctx, Mods* pmods);

    struct ImGuiURL {
        std::string text;
        std::string url;
        const ImVec4 color_hover{ 0.356f, 0.764f, 0.960f, 1.00f };
        const ImVec4 color_regular{ 0.950f, 0.960f, 0.980f, 1.00f };

        void draw() {

            ImGui::TextColored(color_regular, text.c_str());
            if (ImGui::IsItemHovered()) {
                under_line(color_hover);
            }
            if (ImGui::IsItemClicked()) {
                ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }
    };

}