#pragma once

#include "Mods.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Shellapi.h"
#include "glm/glm.hpp"

enum AnimationState {
    NONE,
    START,
    WORK,
    END,
};

struct AnimValue {
    ImRect a, b;
};

struct Keyframe {

    float tstore{};
    float time_start, time_end{};

    float get_value(float time) { 
        tstore += time;
        return glm::lerp(0.0f, 1.0f, glm::smoothstep(time_start, time_end, tstore)); 
    }
    bool is_end() {
        return tstore >= time_end;
    }
};

struct ImGuiRaiiFont {
    explicit ImGuiRaiiFont(ImFont* font, float scale) { ImGui::PushFont((ImFont*)font, scale); };
    ~ImGuiRaiiFont() noexcept { ImGui::PopFont(); }
    // no copies
    ImGuiRaiiFont(const ImGuiRaiiFont&)            = delete;
    ImGuiRaiiFont& operator=(const ImGuiRaiiFont&) = delete;
    // no moves
    ImGuiRaiiFont(ImGuiRaiiFont&&)            = delete;
    ImGuiRaiiFont& operator=(ImGuiRaiiFont&&) = delete;
};

struct OurImGuiContext {
    int wflags{};

    Mod* selected_mod{nullptr};

    ImRect main_window_rect{};
    ImRect left_window_rect{};

    ImFont* main_font{};
    ImFont* fancy_font{};
    ImFont* infobox_font{};
    ImFont* modname_font{};
    ImFont* tony_font{};

    ModCategory selected_category{};
    AnimationState an_settings_state{};
    AnimationState an_info_state{};

    Keyframe main_window_anim{
        .time_start = 0.0f,
        .time_end   = 2.2f,
    };
    Keyframe left_window_anim{
        .time_start = 0.0,
        .time_end   = 1.2f,
    };
    Keyframe bottom_window_anim{.time_start = 1.3f, .time_end = 3.2f};
};

namespace gui {
// gui function definitions
inline void under_line(const ImColor& col) {
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    min.y      = max.y;
    ImGui::GetWindowDrawList()->AddLine(min, max, col, 1.0f);
}
void dark_theme(unsigned int dpi);

// helpmarker function, to avoid typing it every time in the gui section
void help_marker(const char* desc);

void imgui_main_window_proc(OurImGuiContext* ctx, Mods* pmods, bool* window_open);
void imgui_left_window_proc(OurImGuiContext* ctx, Mods* pmods);
void imgui_bottom_window_proc(OurImGuiContext* ctx, Mods* pmods);

struct ImGuiURL {
    std::string text;
    std::string url;
    const ImVec4 color_hover{0.356f, 0.764f, 0.960f, 1.00f};
    const ImVec4 color_regular{0.950f, 0.960f, 0.980f, 1.00f};

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

} // namespace gui