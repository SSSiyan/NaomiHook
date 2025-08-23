#include "GuiFunctions.hpp"
#include "Config.hpp"
#include "LicenseStrings.hpp"
#include "mods/KbmControls.hpp"
#include "utility/Hash.hpp"
#include <cmath> // expf, fabsf
#include <string>
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include "glm/vec2.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#define GUI_VERSION "- Vol.1"
#define IMGUI_WINDOW_PADDING 10.0f

#define IMGUI_WINDOW_BG_COLOR IM_COL32(22, 22, 22, 170)    // background
#define IMGUI_WINDOW_CT_COLOR IM_COL32(255, 201, 115, 255) // header
#define IMGUI_WINDOW_IN_COLOR IM_COL32(255, 255, 255, 255) // description

#define IMGUI_WINDOW_ST_MODNAME_COLOR 0xffbfe6ff // mod title at top right

// --- hold-to-save state ---
static bool g_save_hold_tracking     = false;
static bool g_save_hold_done         = false;
static double g_save_hold_t0         = 0.0;
static const float SAVE_HOLD_SECONDS = 1.5f;

// --- folder slide animation state ---
static const float kFolderSlideSec = 0.12f; // seconds
struct TreeAnimState {
    double t0      = 0.0;    // when current anim started
    float last_h   = 140.0f; // measured full content height
    bool animating = false;  // anim currently running
    bool opening   = false;  // direction
    bool was_open  = false;  // last known open state
};
static std::unordered_map<ImGuiID, TreeAnimState> g_treeAnim;

// --- category header slide-in on selection/collapse ---
static const float kCategorySlideSec = 0.14f;
static const float kCategorySlidePx  = 12.0f;
struct CategorySlideState {
    double t0      = 0.0;
    bool animating = false; // plays on open/select and on collapse
    int dir        = +1;    // +1 = nudge from right (open), -1 = nudge from left (collapse)
};
static std::unordered_map<ImGuiID, CategorySlideState> g_catSlide;

// --- infobox content fade-in state ---
static const float kInfoFadeSec   = 0.10f; // quick fade (seconds)
static ImGuiID g_info_last_mod_id = 0;
static double g_info_fade_t0      = 0.0;

static inline float ease_smoothstep(float t) {
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

static float linear_map(float edge0, float edge1, float x) {
    x = ((x - edge0) / (edge1 - edge0));
    return glm::clamp(x, 0.0f, 1.0f);
}

// --- Smooth scroll (per-window) ---
struct SmoothScrollState {
    float target = 0.0f;
    float last   = 0.0f;
    bool init    = false;
};
static std::unordered_map<ImGuiID, SmoothScrollState> g_smoothScroll;

static inline float _ss_clamp(float v, float mn, float mx) {
    return v < mn ? mn : (v > mx ? mx : v);
}

// Call right after Begin(...) / inside a scrolling BeginChild(...)
static void apply_smooth_scroll_for_current_window(float lines_per_notch = 6.0f, float frequency = 14.0f) {
    ImGuiIO& io    = ImGui::GetIO();
    const float dt = io.DeltaTime > 0.0f ? io.DeltaTime : (1.0f / 60.0f);

    ImGuiID id           = ImGui::GetID((void*)ImGui::GetCurrentWindow());
    SmoothScrollState& S = g_smoothScroll[id];

    const float curY   = ImGui::GetScrollY();
    const float maxY   = ImGui::GetScrollMaxY();
    const float stepPx = ImGui::GetTextLineHeightWithSpacing() * lines_per_notch;

    if (!S.init) {
        S.init   = true;
        S.target = curY;
        S.last   = curY;
    }

    const bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows);
    if (hovered && io.MouseWheel != 0.0f) {
        S.target = _ss_clamp(S.target - io.MouseWheel * stepPx, 0.0f, maxY);
    }

    if (io.MouseDown[0] && fabsf(curY - S.last) > 1.0f) {
        S.target = curY;
    }
    S.target = _ss_clamp(S.target, 0.0f, maxY);

    const float alpha = 1.0f - expf(-frequency * dt);
    const float newY  = curY + (S.target - curY) * alpha;

    if (fabsf(newY - curY) > 0.01f)
        ImGui::SetScrollY(_ss_clamp(newY, 0.0f, maxY));

    S.last = ImGui::GetScrollY();
}

namespace gui {
void dark_theme(unsigned int dpi) {
    {
        static constexpr float default_dpi = 96.0f;
        const float dpi_fuqtor             = (float)dpi / default_dpi;

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(dpi_fuqtor);

        // Deep Dark style by janekb04 from ImThemes
        style.Alpha                            = 1.0f;
        style.DisabledAlpha                    = 0.6000000238418579f;
        style.WindowPadding                    = ImVec2(8.0f, 8.0f);
        style.WindowBorderSize                 = 1.0f;
        style.WindowMinSize                    = ImVec2(32.0f, 32.0f);
        style.WindowTitleAlign                 = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition         = ImGuiDir_Left;
        style.ChildBorderSize                  = 1.0f;
        style.PopupBorderSize                  = 1.0f;
        style.FramePadding                     = ImVec2(5.0f, 2.0f);
        style.FrameBorderSize                  = 1.0f;
        style.ItemSpacing                      = ImVec2(6.0f, 2.0f);
        style.ItemInnerSpacing                 = ImVec2(7.0f, 6.0f);
        style.CellPadding                      = ImVec2(6.0f, 6.0f);
        style.IndentSpacing                    = 25.0f;
        style.ColumnsMinSpacing                = 6.0f;
        style.ScrollbarSize                    = 18.0f;
        style.GrabMinSize                      = 10.0f;
        style.TabBorderSize                    = 1.0f;
        style.TabCloseButtonMinWidthSelected   = 0.0f;
        style.TabCloseButtonMinWidthUnselected = 0.0f;
        style.ColorButtonPosition              = ImGuiDir_Right;
        style.ButtonTextAlign                  = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign              = ImVec2(0.0f, 0.0f);

        style.Colors[ImGuiCol_Text]           = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled]   = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
        style.Colors[ImGuiCol_WindowBg]       = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
        style.Colors[ImGuiCol_ChildBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_PopupBg]        = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
        style.Colors[ImGuiCol_Border]         = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
        style.Colors[ImGuiCol_BorderShadow]   = ImVec4(0.0f, 0.0f, 0.0f, 0.239999994635582f);
        style.Colors[ImGuiCol_FrameBg]        = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
        style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
        style.Colors[ImGuiCol_TitleBg]        = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive]  = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg]   = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =
            ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =
            ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
        style.Colors[ImGuiCol_CheckMark]  = ImColor::ImColor(IMGUI_WINDOW_CT_COLOR);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
        style.Colors[ImGuiCol_SliderGrabActive] =
            ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
        style.Colors[ImGuiCol_Button]        = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
        style.Colors[ImGuiCol_ButtonActive]  = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
        style.Colors[ImGuiCol_Header]        = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
        style.Colors[ImGuiCol_HeaderActive]  = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
        style.Colors[ImGuiCol_Separator]     = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_SeparatorHovered] =
            ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip]      = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_ResizeGripHovered] =
            ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
        style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
        style.Colors[ImGuiCol_Tab]                  = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TabHovered]           = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
        style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TableBorderStrong]    = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
        style.Colors[ImGuiCol_TableBorderLight] =
            ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
        style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
        style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
        style.Colors[ImGuiCol_NavCursor]             = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
        style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
        style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);
    }

    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_WindowBg]             = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
    colors[ImGuiCol_ChildBg]              = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
    colors[ImGuiCol_PopupBg]              = ImColor::ImColor(IMGUI_WINDOW_BG_COLOR);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.129f, 0.184f, 0.859f, 1.00f);
    colors[ImGuiCol_TabSelected]          = ImVec4(0.51f, 0.40f, 0.24f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline]  = ImVec4(1.00f, 0.79f, 0.45f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]    = ImVec4(0.42f, 0.31f, 0.14f, 1.00f);
    colors[ImGuiCol_TextLink]             = ImVec4(0.98f, 0.72f, 0.26f, 1.00f);
    colors[ImGuiCol_DragDropTarget]       = ImVec4(0.86f, 0.65f, 0.33f, 1.00f);
    colors[ImGuiCol_Header]               = ImVec4(0.31f, 0.70f, 0.15f, 1.00f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(1.000f, 0.525f, 0.132f, 1.000f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.155f, 1.000f, 0.128f, 1.000f);
    colors[ImGuiCol_BorderShadow]         = ImVec4(1.000f, 1.000f, 1.000f, 0.051f);
    colors[ImGuiCol_Border]               = ImVec4(0.000f, 0.000f, 0.000f, 0.153f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.129f, 0.184f, 0.859f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.084f, 0.117f, 0.528f, 1.000f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.129f, 0.184f, 0.859f, 1.000f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.084f, 0.117f, 0.528f, 1.000f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.082f, 0.118f, 0.529f, 1.000f);
    colors[ImGuiCol_Separator]            = ImVec4(0.04f, 1.00f, 0.12f, 0.83f);
    colors[ImGuiCol_TreeLines]            = ImVec4(0.04f, 1.00f, 0.12f, 0.65f);
}

void fps_drawing() {
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void credits_drawing() {
    if (ImGui::BeginTabItem("About")) {
        ImGui::BeginChild("AboutChild", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollWithMouse);
        apply_smooth_scroll_for_current_window();
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

        ImGuiURL repo{"https://github.com/muhopensores/dmc4_hook", "https://github.com/muhopensores/dmc4_hook"};
        repo.draw();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("This trainer was made using:");

        static std::array<ImGuiURL, 5> links{
            ImGuiURL{"REFramework https://github.com/praydog/REFramework", "https://github.com/praydog/REFramework"},
            ImGuiURL{"GLM https://github.com/g-truc/glm", "https://github.com/g-truc/glm"},
            ImGuiURL{"Dear ImGui https://github.com/ocornut/imgui", "https://github.com/ocornut/imgui"},
            ImGuiURL{"MinHook https://github.com/TsudaKageyu/minhook", "https://github.com/TsudaKageyu/minhook"},
            ImGuiURL{"spdlog https://github.com/gabime/spdlog", "https://github.com/gabime/spdlog"}};
        for (auto& link : links) {
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
                License{"REFramework", license::reframework},
                License{"GLM", license::glm},
                License{"ImGui", license::imgui},
                License{"MinHook", license::minhook},
                License{"spdlog", license::spdlog},
                License{"csys", license::fmtlib},
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

// --- SLIDE-IN/OUT FROM LEFT: Bottom info panel ---
void imgui_bottom_window_proc(OurImGuiContext* ctx, Mods* pmods) {
    auto& io = ImGui::GetIO();

    // Animation progress -> eased
    float prog = linear_map(ctx->bottom_window_anim.time_start, ctx->bottom_window_anim.time_end, ctx->an_accumulator);
    float e    = ease_smoothstep(prog);

    // Final (resting) rect
    glm::vec2 base_pos{ctx->main_window_rect.Min.x, ctx->main_window_rect.Max.y + IMGUI_WINDOW_PADDING};

    // Width spans from main panel left to settings panel right; fall back to main width if needed
    float span_w = ctx->left_window_rect.Max.x - ctx->main_window_rect.Min.x;
    if (span_w < ctx->main_window_rect.GetWidth())
        span_w = ctx->main_window_rect.GetWidth();

    glm::vec2 size{span_w, io.DisplaySize.y * 0.16f};

    // Slide distance equals panel width plus margin
    float slide = (1.0f - e) * (size.x + 40.0f);
    glm::vec2 pos{base_pos.x - slide, base_pos.y};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    bool window_open = ctx->selected_mod != nullptr;
    ImGui::PushFont(ctx->fancy_font, 18.0f * (io.DisplaySize.y / 1080.0f));
    ImGuiWindowFlags info_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("Info window", &window_open, info_flags);
    {
        apply_smooth_scroll_for_current_window();

        // Text color
        ImGui::PushStyleColor(ImGuiCol_Text, IMGUI_WINDOW_IN_COLOR);

        if (ctx->selected_mod) {
            // Per-mod quick fade-in of content, clipped to the box, with NO child background.
            ImGuiID cur_mod_id = ImGui::GetID((void*)ctx->selected_mod);
            if (cur_mod_id != g_info_last_mod_id) {
                g_info_last_mod_id = cur_mod_id;
                g_info_fade_t0     = ImGui::GetTime();
            }

            float t = (float)((ImGui::GetTime() - g_info_fade_t0) / kInfoFadeSec);
            if (t < 0.0f)
                t = 0.0f;
            if (t > 1.0f)
                t = 1.0f;
            float a = ease_smoothstep(t); // 0 -> 1

            ImVec2 avail = ImGui::GetContentRegionAvail();

            // Ensure the child has no background (remove that dark rectangle)
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
            ImGui::BeginChild("InfoContentFadeChild", ImVec2(avail.x, avail.y), false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * a);
            ctx->selected_mod->render_description();
            ImGui::PopStyleVar();

            ImGui::EndChild();
            ImGui::PopStyleColor(); // ChildBg
        }

        ImGui::PopStyleColor(); // Text
    }
    ImGui::PopFont();
    ImGui::End();
}

// --- Animated title underline state ---
static ImGuiID g_last_mod_title_id = 0;
static float g_title_underline_t   = 0.0f;

// --- SLIDE-IN/OUT FROM LEFT: Right settings panel ---
void imgui_right_window_proc(OurImGuiContext* ctx, Mods* pmods) {

    bool window_open = ctx->selected_mod;

    // Animation progress -> eased
    float prog = linear_map(ctx->left_window_anim.time_start, ctx->left_window_anim.time_end, ctx->an_accumulator);
    float e    = ease_smoothstep(prog);

    // Final (resting) rect sits to the right of the main panel
    glm::vec2 base_pos{ctx->main_window_rect.Max.x + IMGUI_WINDOW_PADDING, ctx->main_window_rect.Min.y};
    glm::vec2 size{ctx->main_window_rect.GetWidth(), ctx->main_window_rect.GetHeight()};

    // Slide from the left
    float slide = (1.0f - e) * (size.x + 40.0f);
    glm::vec2 pos{base_pos.x - slide, base_pos.y};

    // Expose to other panes (e.g., bottom bar)
    ctx->left_window_rect.Min = pos;
    ctx->left_window_rect.Max = pos + size;

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    auto& io = ImGui::GetIO();
    ImGuiRaiiFont font{ctx->modname_font, 38.0f * (io.DisplaySize.y / 1080.0f)};
    ImGuiWindowFlags settings_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("Settings window", &window_open, settings_flags);
    {
        apply_smooth_scroll_for_current_window();

        if (ctx->selected_mod) {
            // mod name in diff font
            {
                const std::string& mod_name = ctx->selected_mod->get_human_readable_name();
                ImGui::PushStyleColor(ImGuiCol_Text, IMGUI_WINDOW_ST_MODNAME_COLOR);
                ImGui::Text("%s", mod_name.c_str());
                ImGui::PopStyleColor();

                // Animated title underline that eases in
                ImGuiID cur_id = ImGui::GetID(mod_name.c_str());
                if (cur_id != g_last_mod_title_id) {
                    g_last_mod_title_id = cur_id;
                    g_title_underline_t = 0.0f;
                }
                g_title_underline_t = ImLerp(g_title_underline_t, 1.0f, 0.15f);

                ImVec2 line_start = ImGui::GetItemRectMax();
                ImVec2 a          = ImVec2(line_start.x - ImGui::CalcTextSize(mod_name.c_str()).x, line_start.y + 2.0f);
                float w           = ImGui::GetContentRegionAvail().x * g_title_underline_t;
                ImDrawList* dl    = ImGui::GetWindowDrawList();
                dl->AddLine(a, ImVec2(a.x + w, a.y), IM_COL32(255, 201, 115, 180), 2.0f);
                ImGui::Dummy(ImVec2(0, 8));
            }

            ImGuiRaiiFont font2{ctx->main_font, 24.0f * (io.DisplaySize.y / 1080.0f)};
            ctx->selected_mod->on_draw_ui();
        }
    }
    ImGui::End();
}

// Animated draw_category: quick slide open/close (header nudges on open AND collapse)
static void draw_category(OurImGuiContext* ctx, const char* category_name, ModCategory category_enum) {

    static constexpr auto entry_func = [](OurImGuiContext* ctx, Mod* mod) {
        ImColor color        = ImColor(240, 240, 240);
        ImVec2 pos           = ImGui::GetCursorPos();
        std::string btn_name = mod->get_mod_name();
        btn_name += "_btn";
        ImVec2 sz = ImVec2(-FLT_MIN, ImGui::GetTextLineHeight());
        if (ImGui::InvisibleButton(btn_name.c_str(), sz)) {
            ctx->selected_mod = mod;
        }
        ImGui::SetCursorPos(pos);
        if (ImGui::IsItemHovered()) {
            color = ImColor(40, 255, 33);
        }
        ImGui::TextColored(color, mod->get_human_readable_name().c_str());
    };

    // Per-category slide-in offset if just selected/opened or collapsing
    ImGuiID id_for_slide   = ImGui::GetID(category_name);
    CategorySlideState& CS = g_catSlide[id_for_slide];
    float slide_off_x      = 0.0f;
    if (CS.animating) {
        float t = (float)((ImGui::GetTime() - CS.t0) / kCategorySlideSec);
        if (t >= 1.0f) {
            CS.animating = false;
            t            = 1.0f;
        }
        float e     = ease_smoothstep(t);
        slide_off_x = (1.0f - e) * kCategorySlidePx * (float)CS.dir; // dir: +1 open, -1 collapse
    }

    // Apply the temporary x offset before drawing the header row
    float start_x = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(start_x + slide_off_x);

    ImGuiTreeNodeFlags flags{};
    ImVec2 ps = ImGui::GetCursorScreenPos();
    ImVec2 sz = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight());
    ImRect rect{ps, ImVec2(ps.x + sz.x, ps.y + sz.y)};
    bool hovered = ImGui::IsMouseHoveringRect(rect.Min, rect.Max);

    if (ctx->selected_category == category_enum || hovered) {
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFF222B2B);
        flags = ImGuiTreeNodeFlags_Selected;
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, IMGUI_WINDOW_CT_COLOR);
        ImGui::SetNextItemOpen(false, ImGuiCond_Always);
    }

    bool open_now = ImGui::TreeNodeEx(category_name, flags);
    bool toggled  = ImGui::IsItemToggledOpen();
    ImGui::PopStyleColor(1);

    ImGuiID id       = ImGui::GetID(category_name);
    TreeAnimState& A = g_treeAnim[id];

    if (toggled) {
        A.animating = true;
        A.opening   = open_now;
        A.t0        = ImGui::GetTime();

        // Start header nudge on both opening and collapsing
        CS.t0        = ImGui::GetTime();
        CS.animating = true;
        CS.dir       = open_now ? +1 : -1; // +1 on open/select, -1 on collapse
    }

    bool need_draw_body = open_now || A.animating;
    if (!need_draw_body) {
        return;
    }

    bool manual_push = !open_now;
    if (manual_push) {
        ImGui::TreePush(category_name);
    } else {
        ctx->selected_category = category_enum;
    }

    ImVec2 body_start_screen = ImGui::GetCursorScreenPos();
    float full_height_guess  = (A.last_h > 1.0f ? A.last_h : 1.0f);

    float t = (float)((ImGui::GetTime() - A.t0) / kFolderSlideSec);
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    float e = ease_smoothstep(t);

    float reveal_h;
    if (!A.animating) {
        reveal_h = full_height_guess;
    } else {
        if (A.opening) {
            reveal_h = ImLerp(0.0f, full_height_guess, e);
        } else {
            reveal_h = ImLerp(full_height_guess, 0.0f, e);
        }
    }
    if (reveal_h < 1.0f)
        reveal_h = 1.0f;

    ImDrawList* dl  = ImGui::GetWindowDrawList();
    ImVec2 clip_min = body_start_screen;
    ImVec2 clip_max = ImVec2(body_start_screen.x + ImGui::GetContentRegionAvail().x, body_start_screen.y + reveal_h);
    dl->PushClipRect(clip_min, clip_max, true);

    float fade = ImClamp(reveal_h / ImMax(1.0f, full_height_guess), 0.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (0.25f + 0.75f * fade));

    // --- body contents (original list) ---
    {
        auto& io = ImGui::GetIO();
        ImGui::PushFont(ctx->tony_font, 12.0f * (io.DisplaySize.y / 1080.0f));
        ImVec2 currentSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(currentSpacing.x, 10.0f * (io.DisplaySize.y / 1080.0f)));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f * (io.DisplaySize.y / 1080.0f));
        for (auto& mod : g_framework->get_mods()->m_mods) {
            if (mod->get_category() != category_enum)
                continue;
            entry_func(ctx, mod.get());
        }
        ImGui::PopStyleVar(); // ItemSpacing
        ImGui::PopFont();
    }
    // -------------------------------------

    ImGui::PopStyleVar(); // Alpha fade
    dl->PopClipRect();

    float new_full_h = ImGui::GetCursorScreenPos().y - body_start_screen.y;
    if (new_full_h > 1.0f)
        A.last_h = new_full_h;

    if (A.animating && t >= 1.0f) {
        A.animating = false;
        A.was_open  = open_now;
    }

    if (open_now) {
        ImGui::TreePop();
    } else if (manual_push) {
        ImGui::TreePop();
    }
}

// --- SLIDE-IN/OUT FROM LEFT: Main (categories) panel ---
void imgui_main_window_proc(OurImGuiContext* ctx, Mods* pmods, bool* window_open) {
    {
        auto& io = ImGui::GetIO();

        // Animation progress -> eased
        float prog = linear_map(ctx->main_window_anim.time_start, ctx->main_window_anim.time_end, ctx->an_accumulator);
        float e    = ease_smoothstep(prog);

        // Final (resting) rect
        glm::vec2 final_pos{50.0f, 30.0f};
        glm::vec2 size{io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.58f};

        // Slide in/out from the left; keep size constant
        float slide = (1.0f - e) * (size.x + 40.0f);
        glm::vec2 pos{final_pos.x - slide, final_pos.y};

        ctx->main_window_rect.Min = pos;
        ctx->main_window_rect.Max = pos + size;

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGui::PushFont(ctx->main_font, 24.0f * (io.DisplaySize.y / 1080.0f));
        ImGuiWindowFlags main_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::Begin(PROJECT_NAME " " GUI_VERSION, window_open, main_flags);
        {
            apply_smooth_scroll_for_current_window();

            // Top gradient strip (subtle)
            {
                ImDrawList* dl = ImGui::GetWindowDrawList();
                ImVec2 wp      = ImGui::GetWindowPos();
                ImVec2 ws      = ImGui::GetWindowSize();
                ImU32 c0       = IM_COL32(255, 201, 115, 35);
                ImU32 c1       = IM_COL32(255, 201, 115, 0);
                dl->AddRectFilledMultiColor(ImVec2(wp.x, wp.y), ImVec2(wp.x + ws.x, wp.y + 6.0f), c0, c0, c1, c1);
            }

            // Hold-to-save button: 1.5s hold, fires at full bar; label switches to "Saved!"
            ImVec2 btn_sz = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.2f);

            // Draw a label-less button for full control over text
            ImGui::Button("##SaveConfig", btn_sz);

            // Geometry for overlay
            ImVec2 btn_min = ImGui::GetItemRectMin();
            ImVec2 btn_max = ImGui::GetItemRectMax();
            ImRect btn_rect(btn_min, btn_max);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float rounding = ImGui::GetStyle().FrameRounding;

            if (ImGui::IsItemActive()) {
                // Start tracking on initial press
                if (!g_save_hold_tracking) {
                    g_save_hold_tracking = true;
                    g_save_hold_done     = false;
                    g_save_hold_t0       = ImGui::GetTime();
                }

                // Progress toward hold threshold
                double elapsed = ImGui::GetTime() - g_save_hold_t0;
                float frac     = (float)(elapsed / SAVE_HOLD_SECONDS);
                if (frac < 0.0f)
                    frac = 0.0f;
                if (frac > 1.0f)
                    frac = 1.0f;

                // Trigger save exactly once when bar fills completely (no need to release)
                if (!g_save_hold_done && frac >= 1.0f) {
                    pmods->on_config_save();
                    g_save_hold_done = true;
                }

                // Dim the button and show status text
                dl->AddRectFilled(btn_rect.Min, btn_rect.Max, IM_COL32(0, 0, 0, 140), rounding);

                const char* status_txt = g_save_hold_done ? "Saved!" : "Now Saving...";
                ImVec2 txt_sz          = ImGui::CalcTextSize(status_txt);
                ImVec2 txt_pos(btn_rect.GetCenter().x - txt_sz.x * 0.5f, btn_rect.GetCenter().y - txt_sz.y * 0.5f);
                dl->AddText(txt_pos, IM_COL32(255, 255, 255, 255), status_txt);

                // Progress bar along bottom edge
                dl->AddRectFilled(ImVec2(btn_rect.Min.x, btn_rect.Max.y - 3.0f),
                    ImVec2(btn_rect.Min.x + btn_rect.GetWidth() * frac, btn_rect.Max.y), IM_COL32(255, 255, 255, 70), 1.0f);
            } else {
                // Not held: draw normal label text centered
                const char* idle_txt = "Save Config";
                ImVec2 txt_sz        = ImGui::CalcTextSize(idle_txt);
                ImVec2 txt_pos(btn_rect.GetCenter().x - txt_sz.x * 0.5f, btn_rect.GetCenter().y - txt_sz.y * 0.5f);
                dl->AddText(txt_pos, ImGui::GetColorU32(ImGuiCol_Text), idle_txt);

                // Reset when the press is fully released/cancelled
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    g_save_hold_tracking = false;
                    g_save_hold_done     = false;
                }
            }

            ImGui::PushFont(ctx->fancy_font, 48.0f * (io.DisplaySize.y / 1080.0f));

            draw_category(ctx, "GAMEPLAY", ModCategory::GAMEPLAY);
            draw_category(ctx, "COSMETICS", ModCategory::COSMETICS);
            draw_category(ctx, "STAGE", ModCategory::STAGE);
            draw_category(ctx, "SOUND", ModCategory::SOUND);
            draw_category(ctx, "SYSTEM", ModCategory::SYSTEM);

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
        ImGui::PopFont();
    }
}
} // namespace gui
