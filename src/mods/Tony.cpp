#include "Tony.hpp"
#include "GuiFunctions.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <string>
#include <vector>

#if 1

// ============================================================================
//  UAA / Stat Tracker state (declare FIRST so everything below can use it)
// ============================================================================
static bool g_uaa_enabled       = false; // master toggle for tracking
static bool g_uaa_report_open   = false; // UAA Performance Report popout
static bool g_uaa_hide_zeros    = true;  // hide motions with 0 use
static int g_uaa_last_motion_no = -1;    // last seen motion id

static constexpr int UAA_MAX_MOTIONS          = 403; // 0..402
static int g_uaa_motion_uses[UAA_MAX_MOTIONS] = {0};

// ----------------------------- Schpeltiger (internal-only) -------------------
static float g_schpeltiger_time_sec       = 0.0f;
static float g_schpeltiger_donut_time_sec = 0.0f;
static int g_schpeltiger_crash_count      = 0;
static float g_schpeltiger_nitro_time_sec = 0.0f;
static int g_schpeltiger_nitro_uses       = 0;

static float g_s_last_game_time = -1.0f;
static float g_s_last_spinY     = 0.0f;
static bool g_s_last_clash      = false;
static bool g_s_last_useNitro   = false;

static constexpr float DONUT_SPINY_RATE_EPS = 0.005f; // donut detection

// ============================================================================
//  Assassin Experience (XP + Ranks) - minimal overlay (pretty text only)
// ============================================================================
static bool g_ae_open         = false; // compatibility no-op
static uint64_t g_ae_total_xp = 0;     // persistent XP bucket

// Rank CA trigger
static int g_ae_last_level        = 1;
static float g_ae_rank_ca_start_t = -1.0f; // <0 means inactive

// Subtle CA accumulator (still used by oscilloscope bumps)
static float g_ae_chip_ca_energy      = 0.0f; // 0..1 accumulated pulses
static float g_ae_chip_ca_last_update = -1.0f;

// Position (user-draggable) and persistence
static float g_ae_pos_x = 560.0f; // defaults; saved/loaded
static float g_ae_pos_y = 60.0f;

// Slide animation state
enum AESlideState { AE_Hidden = 0, AE_SlidingIn, AE_Visible, AE_SlidingOut };
static int g_ae_slide_state = AE_Hidden;
static float g_ae_slide_t   = 0.0f; // 0..1 per transition

// Window metrics
static constexpr float AE_WINDOW_W = 520.0f;
static constexpr float AE_WINDOW_H = 56.0f;

static float getGameTimeSeconds(); // forward (implemented later)

// ------------------- AE visual theme --------------------
static constexpr ImU32 AE_BG_U32     = IM_COL32(10, 12, 10, 60);
static constexpr ImU32 AE_BORDER_U32 = IM_COL32(0, 0, 0, 150);

// Kill Reward gold (label color)
static constexpr ImU32 AE_CHIP_START_U32 = IM_COL32(247, 205, 42, 200);
static constexpr ImU32 AE_CHIP_END_U32   = IM_COL32(247, 205, 42, 200);

// -------- Stronger chromatic aberration for actual Rank-Up -------------------
static constexpr float AE_CA_STRONG_DURATION     = 0.28f;
static constexpr float AE_CA_STRONG_OFFSET_F     = 1.35f;
static constexpr float AE_CA_STRONG_BASE_ALPHA   = 0.95f;
static constexpr int AE_CA_STRONG_EXTRA_PASSES   = 2;
static constexpr float AE_CA_STRONG_PASS_OFFSET  = 0.70f;
static constexpr float AE_CA_STRONG_CENTER_NUDGE = 0.80f;

// -------- Subtle CA for intake bumps ----------------------------------------
static constexpr float AE_CHIP_CA_DECAY_RATE   = 4.0f;
static constexpr float AE_CHIP_CA_MAX_OFFSET_F = 0.18f;
static constexpr float AE_CHIP_CA_ALPHA        = 0.35f;
static constexpr float AE_CHIP_CA_JITTER       = 0.45f;

// Intake anchor (screen-space), updated every frame by AE overlay
static ImVec2 g_ae_intake_anchor = ImVec2(0.0f, 0.0f);

// Helper to lerp U32 colors
static inline ImU32 u32_lerp(ImU32 a, ImU32 b, float t) {
    t       = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
    int ar  = (a >> IM_COL32_R_SHIFT) & 0xFF;
    int ag  = (a >> IM_COL32_G_SHIFT) & 0xFF;
    int ab  = (a >> IM_COL32_B_SHIFT) & 0xFF;
    int aa  = (a >> IM_COL32_A_SHIFT) & 0xFF;
    int br  = (b >> IM_COL32_R_SHIFT) & 0xFF;
    int bg  = (b >> IM_COL32_G_SHIFT) & 0xFF;
    int bb  = (b >> IM_COL32_B_SHIFT) & 0xFF;
    int ba  = (b >> IM_COL32_A_SHIFT) & 0xFF;
    int rr  = (int)std::round(ar + (br - ar) * t);
    int gg  = (int)std::round(ag + (bg - ag) * t);
    int bb2 = (int)std::round(ab + (bb - ab) * t);
    int aa2 = (int)std::round(aa + (ba - aa) * t);
    return IM_COL32(rr, gg, bb2, aa2);
}

// Rank curve helpers
static inline uint64_t AE_TotalXPForLevel(int L) {
    if (L <= 1)
        return 0ULL;
    double x = double(L - 1);
    return (uint64_t)std::llround(500.0 * x * x + 1500.0 * x);
}
static inline int AE_LevelFromXP(uint64_t xp) {
    int lo = 1, hi = 300;
    while (lo < hi) {
        int mid = (lo + hi + 1) / 2;
        if (AE_TotalXPForLevel(mid) <= xp)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}
static inline uint64_t AE_XPIntoLevel(uint64_t xp, int lvl) {
    uint64_t base = AE_TotalXPForLevel(lvl);
    return xp > base ? (xp - base) : 0ULL;
}
static inline uint64_t AE_XPForNext(uint64_t xp, int lvl) {
    uint64_t base = AE_TotalXPForLevel(lvl), next = AE_TotalXPForLevel(lvl + 1);
    return next > base ? (next - base) : 1ULL;
}

// ------------------------ Intake anchor getter -------------------------------
static inline ImVec2 AE_GetIntakeAnchor() {
    return g_ae_intake_anchor;
}

// ============================================================================
//  Noise helpers (hash) used by oscilloscope
// ============================================================================
static inline float sc_fract(float x) {
    return x - std::floor(x);
}
static inline float sc_hash11(float x) {
    return sc_fract(std::sin(x) * 43758.5453f);
}
static inline float sc_hash21(float x, float y) {
    return sc_fract(std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f);
}

// ============================================================================
//  OSCILLOSCOPE XP VISUALIZER (replaces chip intake effect)
//  - Draws a CRT-style scope near the AE overlay
//  - Pulses on XP payouts (bigger pulses for larger rewards)
//  - Pause-safe timing via getGameTimeSeconds()
// ============================================================================
static constexpr float OSC_PI        = 3.1415926535f;
static constexpr float OSC_SAMPLE_HZ = 240.0f; // internal sample rate
static constexpr int OSC_SAMPLES     = 260;    // width in samples
static constexpr float OSC_W         = 260.0f; // pixels
static constexpr float OSC_H         = 72.0f;  // pixels

// Colors (green CRT-ish)
static constexpr ImU32 OSC_BG       = IM_COL32(6, 8, 7, 160);
static constexpr ImU32 OSC_GRID     = IM_COL32(84, 130, 98, 36);
static constexpr ImU32 OSC_TRACE    = IM_COL32(140, 255, 180, 235);
static constexpr ImU32 OSC_TRACE_G1 = IM_COL32(120, 255, 170, 70);
static constexpr ImU32 OSC_TRACE_G2 = IM_COL32(120, 255, 170, 30);
static constexpr ImU32 OSC_BEZEL    = IM_COL32(0, 0, 0, 170);
static constexpr ImU32 OSC_TEXT     = IM_COL32(140, 255, 180, 180);

struct OscPulse {
    float t0;    // start time
    float mag;   // amplitude (0..1)
    float freq;  // Hz
    float phase; // radians
    float tau;   // decay constant (seconds)
};

static std::vector<OscPulse> g_osc_pulses;
static std::deque<float> g_osc_samples;
static float g_osc_last_sample_t = -1.0f;
static float g_osc_flash_t       = -1.0f; // for brief screen glow on big hits

static inline float osc_clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Convert a "budget" (xp) to a pulse
static void OSC_AddPulse(int budget) {
    if (budget <= 0)
        return;

    float now = getGameTimeSeconds();

    // Map budget -> amplitude + decay with gentle compression
    // small hits still visible, big hits punchy but not clipped
    float b     = (float)budget;
    float mag   = osc_clamp(0.12f + 0.015f * std::sqrt(b), 0.12f, 1.0f);
    float tau   = 0.35f + 0.0018f * std::min(b, 8000.0f); // bigger reward -> longer decay, clamped
    float seed  = sc_hash21(b, now) * 1000.0f;
    float freq  = 5.0f + 6.0f * sc_hash11(seed + 7.3f); // 5..11 Hz
    float phase = sc_hash11(seed + 1.7f) * 2.0f * OSC_PI;

    g_osc_pulses.push_back({now, mag, freq, phase, tau});

    // Brief flash and a subtle CA bump so the AE label shimmers on hits
    g_osc_flash_t       = now;
    g_ae_chip_ca_energy = std::min(1.0f, g_ae_chip_ca_energy + osc_clamp(mag * 0.35f, 0.05f, 0.35f));
}

// Produce one sample from active pulses (+ small instrumentation noise)
static inline float OSC_SampleAt(float t) {
    float y = 0.0f;

    // Sum of exponentially decaying sines
    for (size_t i = 0; i < g_osc_pulses.size(); ++i) {
        const OscPulse& p = g_osc_pulses[i];
        float u           = t - p.t0;
        if (u < 0.0f)
            continue;
        float env = std::exp(-u / p.tau);
        if (env < 0.002f)
            continue;
        y += p.mag * env * std::sin(2.0f * OSC_PI * p.freq * u + p.phase);
    }

    // Light noise + 60Hz hum for texture
    y += 0.02f * std::sin(2.0f * OSC_PI * 60.0f * t + 0.8f);
    y += (sc_hash11(t * 37.0f) - 0.5f) * 0.02f;

    // Soft clip to avoid harsh corners
    y = std::tanh(y * 1.7f);
    return osc_clamp(y, -1.2f, 1.2f);
}

// Maintain sample buffer and prune dead pulses
static void OSC_UpdateSamples(float now) {
    if (g_osc_last_sample_t < 0.0f) {
        g_osc_last_sample_t = now;
        g_osc_samples.assign(OSC_SAMPLES, 0.0f);
        return;
    }
    float dt = now - g_osc_last_sample_t;
    if (dt < 0.0f)
        dt = 0.0f;

    int to_add = (int)std::floor(dt * OSC_SAMPLE_HZ);
    if (to_add <= 0)
        return;

    float t = g_osc_last_sample_t;
    for (int i = 0; i < to_add; ++i) {
        t += 1.0f / OSC_SAMPLE_HZ;
        float s = OSC_SampleAt(t);
        g_osc_samples.push_back(s);
        if ((int)g_osc_samples.size() > OSC_SAMPLES)
            g_osc_samples.pop_front();
    }
    g_osc_last_sample_t = t;

    // Cull finished pulses
    g_osc_pulses.erase(
        std::remove_if(g_osc_pulses.begin(), g_osc_pulses.end(), [now](const OscPulse& p) { return (now - p.t0) > (p.tau * 6.0f); }),
        g_osc_pulses.end());
}

// Draw a small scope near the AE overlay (anchored to XP digits)
static void OSC_DrawScope() {
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImVec2 vp      = ImGui::GetIO().DisplaySize;

    // Anchor scope to the XP "into" digits, offset a bit to the right and up
    ImVec2 anchor = AE_GetIntakeAnchor();
    ImVec2 tl     = ImVec2(anchor.x + 52.0f, anchor.y - OSC_H - 18.0f);

    // Clamp into screen so it never runs off
    tl.x = osc_clamp(tl.x, 6.0f, vp.x - OSC_W - 6.0f);
    tl.y = osc_clamp(tl.y, 6.0f, vp.y - OSC_H - 6.0f);

    ImVec2 br = ImVec2(tl.x + OSC_W, tl.y + OSC_H);

    // Bezel + background
    dl->AddRectFilled(ImVec2(tl.x - 2, tl.y - 2), ImVec2(br.x + 2, br.y + 2), OSC_BEZEL, 6.0f);
    dl->AddRectFilled(tl, br, OSC_BG, 4.0f);

    // Grid (vert every 26px, horiz every 18px)
    for (float x = tl.x + 26.0f; x < br.x; x += 26.0f)
        dl->AddLine(ImVec2(x, tl.y), ImVec2(x, br.y), OSC_GRID, 1.0f);
    for (float y = tl.y + 18.0f; y < br.y; y += 18.0f)
        dl->AddLine(ImVec2(tl.x, y), ImVec2(br.x, y), OSC_GRID, 1.0f);

    // Scanline glow when a new pulse just happened
    float now = getGameTimeSeconds();
    if (g_osc_flash_t >= 0.0f) {
        float age = now - g_osc_flash_t;
        if (age < 0.18f) {
            float a    = (1.0f - (age / 0.18f));
            ImU32 glow = IM_COL32(140, 255, 180, (int)std::round(120.0f * a));
            dl->AddRectFilled(tl, br, glow, 4.0f);
        }
    }

    // Build polyline from sample buffer
    if (!g_osc_samples.empty()) {
        const int n = (int)g_osc_samples.size();
        std::vector<ImVec2> path;
        path.reserve(n);

        for (int i = 0; i < n; ++i) {
            float x   = tl.x + (float)i * (OSC_W / (float)OSC_SAMPLES);
            float y01 = (g_osc_samples[i] * 0.45f + 0.5f); // map -1..1 -> 0..1
            float y   = tl.y + (1.0f - y01) * OSC_H;
            path.emplace_back(x, y);
        }

        // Glow passes
        dl->AddPolyline(path.data(), (int)path.size(), OSC_TRACE_G2, false, 6.0f);
        dl->AddPolyline(path.data(), (int)path.size(), OSC_TRACE_G1, false, 3.5f);
        // Main trace
        dl->AddPolyline(path.data(), (int)path.size(), OSC_TRACE, false, 1.6f);

        // Hot "beam" at the latest sample
        ImVec2 tip = path.back();
        dl->AddCircleFilled(tip, 2.0f, OSC_TRACE);
    }

    // Tiny label
    ImFont* tf = g_framework->get_our_imgui_ctx()->tony_font;
    dl->AddText(tf, 10.0f, ImVec2(tl.x + 6.0f, tl.y + 4.0f), OSC_TEXT, "XP-Scope");

    // ---------------------------------------------------------------------
    // Bottom-row: "RANK N    into / toNext" INSIDE the meter (no resize)
    // ---------------------------------------------------------------------
    int lvl         = AE_LevelFromXP(g_ae_total_xp);
    uint64_t into   = AE_XPIntoLevel(g_ae_total_xp, lvl);
    uint64_t toNext = AE_XPForNext(g_ae_total_xp, lvl);

    char rankLine[160];
    std::snprintf(rankLine, sizeof(rankLine), "RANK %d    %llu / %llu", lvl, (unsigned long long)into, (unsigned long long)toNext);

    // Start from a comfortable size; downscale until it fits horizontally.
    const float padX = 6.0f;
    const float padY = 4.0f;
    const float maxW = OSC_W - padX * 2.0f;

    float uiScale     = ImGui::GetIO().DisplaySize.y / 1080.0f;
    float sizePx      = std::max(10.0f, 12.0f * uiScale); // starting size
    const float minPx = 8.0f;

    ImVec2 tsize = tf->CalcTextSizeA(sizePx, FLT_MAX, 0.0f, rankLine);
    while (tsize.x > maxW && sizePx > minPx) {
        sizePx -= 0.25f;
        tsize = tf->CalcTextSizeA(sizePx, FLT_MAX, 0.0f, rankLine);
    }

    // Center horizontally on the bottom row
    float tx = tl.x + (OSC_W - tsize.x) * 0.5f;
    float ty = br.y - padY - tsize.y;

    // Shadow + main text for readability over the trace
    dl->AddText(tf, sizePx, ImVec2(tx + 1.0f, ty + 1.0f), IM_COL32(0, 0, 0, 200), rankLine);
    dl->AddText(tf, sizePx, ImVec2(tx, ty), OSC_TEXT, rankLine);
}

// Entry: update + draw
static void OSC_UpdateAndDraw() {
    // Only show when AE overlay is visible
    if (g_ae_slide_state == AE_Hidden)
        return;
    float now = getGameTimeSeconds();
    OSC_UpdateSamples(now);
    OSC_DrawScope();
}

// ============================================================================
//  UAA Performance Report UI (persisted via on_frame)
// ============================================================================
static float AE_UpdateSlideAndGetOffsetX(float dt); // fwd

// --------- CA render helpers (strong rank-up and subtle chip-intake) --------
static void AE_RenderRankCA_Strong(const char* text, ImVec2 pos, float fontPx) {
    float now = getGameTimeSeconds();
    float t0  = g_ae_rank_ca_start_t;
    if (t0 < 0.0f)
        return;
    float dt = now - t0;
    if (dt < 0.0f || dt > AE_CA_STRONG_DURATION)
        return;

    float u   = dt / AE_CA_STRONG_DURATION;     // 0..1
    float e   = 1.0f - (1.0f - u) * (1.0f - u); // fast out
    float off = (1.0f - e) * (fontPx * AE_CA_STRONG_OFFSET_F);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* f      = g_framework->get_our_imgui_ctx()->tony_font;

    float jitterX = (std::sin(now * 73.0f) * AE_CA_STRONG_CENTER_NUDGE);
    float jitterY = (std::cos(now * 61.0f) * AE_CA_STRONG_CENTER_NUDGE * 0.6f);

    ImU32 cR = IM_COL32(255, 40, 40, (int)std::round(255.0f * AE_CA_STRONG_BASE_ALPHA));
    ImU32 cC = IM_COL32(64, 255, 255, (int)std::round(255.0f * AE_CA_STRONG_BASE_ALPHA));

    dl->AddText(f, fontPx, ImVec2(pos.x - off + jitterX, pos.y + jitterY), cR, text);
    dl->AddText(f, fontPx, ImVec2(pos.x + off - jitterX, pos.y - jitterY), cC, text);

    for (int p = 0; p < AE_CA_STRONG_EXTRA_PASSES; ++p) {
        float o = (p + 1) * AE_CA_STRONG_PASS_OFFSET;
        dl->AddText(f, fontPx, ImVec2(pos.x - off - o, pos.y), cR, text);
        dl->AddText(f, fontPx, ImVec2(pos.x + off + o, pos.y), cC, text);
    }
}

static void AE_RenderChipCA_Accum(const char* text, ImVec2 pos, float fontPx) {
    float now = getGameTimeSeconds();
    if (g_ae_chip_ca_last_update < 0.0f)
        g_ae_chip_ca_last_update = now;
    float dt = now - g_ae_chip_ca_last_update;
    if (dt < 0.0f)
        dt = 0.0f;
    g_ae_chip_ca_last_update = now;

    if (g_ae_chip_ca_energy > 0.0f) {
        g_ae_chip_ca_energy = std::max(0.0f, g_ae_chip_ca_energy - AE_CHIP_CA_DECAY_RATE * dt);
    }
    float e = g_ae_chip_ca_energy;
    if (e <= 0.001f)
        return;

    float s   = std::sqrt(std::min(1.0f, e));
    float off = fontPx * AE_CHIP_CA_MAX_OFFSET_F * s;

    float jx = (sc_hash21(e + 12.3f, now + 3.7f) - 0.5f) * 2.0f * AE_CHIP_CA_JITTER;
    float jy = (sc_hash21(e + 7.7f, now + 5.1f) - 0.5f) * 2.0f * (AE_CHIP_CA_JITTER * 0.6f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* f      = g_framework->get_our_imgui_ctx()->tony_font;

    int a255 = (int)std::round(AE_CHIP_CA_ALPHA * 255.0f * s);
    ImU32 cR = IM_COL32(255, 64, 64, a255);
    ImU32 cC = IM_COL32(64, 255, 255, a255);

    dl->AddText(f, fontPx, ImVec2(pos.x - off + jx, pos.y + jy), cR, text);
    dl->AddText(f, fontPx, ImVec2(pos.x + off - jx, pos.y - jy), cC, text);
}

// Rank+XP label only (no bar). Intake anchor set to center of CURRENT XP digits.
static void AE_DrawWindow() {
    static float s_last_t = -1.0f;
    float now             = getGameTimeSeconds();
    if (s_last_t < 0.0f)
        s_last_t = now;
    float dt = now - s_last_t;
    if (dt < 0.0f)
        dt = 0.0f;
    s_last_t = now;

    float slideOffX = AE_UpdateSlideAndGetOffsetX(dt);
    if (g_ae_slide_state == AE_Hidden)
        return;

    ImVec2 vp  = ImGui::GetIO().DisplaySize;
    g_ae_pos_x = std::max(8.0f, std::min(g_ae_pos_x, vp.x - AE_WINDOW_W - 8.0f));
    g_ae_pos_y = std::max(8.0f, std::min(g_ae_pos_y, vp.y - AE_WINDOW_H - 8.0f));

    ImVec2 drawPos(g_ae_pos_x + slideOffX, g_ae_pos_y);
    ImGui::SetNextWindowPos(drawPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(AE_WINDOW_W, AE_WINDOW_H), ImGuiCond_Always);

    if (ImGui::Begin("Assassin Experience", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings)) {

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("AE_DRAG", ImGui::GetWindowSize());
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            ImVec2 d = ImGui::GetIO().MouseDelta;
            g_ae_pos_x += d.x;
            g_ae_pos_y += d.y;
        }

        ImGui::SetCursorPos(ImVec2(0, 4.0f));

        int lvl         = AE_LevelFromXP(g_ae_total_xp);
        uint64_t into   = AE_XPIntoLevel(g_ae_total_xp, lvl);
        uint64_t toNext = AE_XPForNext(g_ae_total_xp, lvl);

        char rankPrefix[64];
        std::snprintf(rankPrefix, sizeof(rankPrefix), "Rank %d    ", lvl);
        char intoStr[32];
        std::snprintf(intoStr, sizeof(intoStr), "%llu", (unsigned long long)into);
        const char* sepStr = " / ";
        char nextStr[32];
        std::snprintf(nextStr, sizeof(nextStr), "%llu", (unsigned long long)toNext);

        // NOTE: old composite label "fullBuf" and its rendering are disabled.
        // Keeping the strings above so we can still compute the intake anchor.
        // char fullBuf[160];
        // std::snprintf(fullBuf, sizeof(fullBuf), "%s%s%s%s", rankPrefix, intoStr, sepStr, nextStr);

        float scale     = ImGui::GetIO().DisplaySize.y / 1080.0f;
        float labelSize = std::max(14.0f, 16.0f * scale);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x += 8.0f;

        // ImDrawList* dl = ImGui::GetWindowDrawList(); // unused when label is disabled
        ImFont* tf = g_framework->get_our_imgui_ctx()->tony_font;

        // DISABLED: old on-screen rank/xp text and CA passes (now shown inside meter)
        // dl->AddText(tf, labelSize, ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 190), fullBuf);
        // AE_RenderChipCA_Accum(fullBuf, pos, labelSize);
        // AE_RenderRankCA_Strong(fullBuf, pos, labelSize);
        // dl->AddText(tf, labelSize, pos, IM_COL32(255, 255, 255, 235), fullBuf);

        // We still compute the intake anchor based on the numeric widths
        ImVec2 wPrefix     = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, rankPrefix);
        ImVec2 wInto       = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, intoStr);
        ImVec2 hDigit      = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, "0");
        g_ae_intake_anchor = ImVec2(pos.x + wPrefix.x + (wInto.x * 0.5f), pos.y + (hDigit.y * 0.5f));

        ImGui::Dummy(ImVec2(0, 4.0f));
    }
    ImGui::End();
}

// ============================================================================
//  UAA Performance Report UI (persisted via on_frame)
// ============================================================================
enum UAAWeaponCat { UAA_CAT_ALL = 0, UAA_CAT_BLOOD_BERRY, UAA_CAT_MK_I, UAA_CAT_MK_II, UAA_CAT_MK_III, UAA_CAT_OTHER, UAA_CAT_SCHPELTIGER };
static int g_uaa_weapon_filter      = UAA_CAT_ALL;
static const char* UAA_CAT_LABELS[] = {"All", "Blood Berry", "MK-I", "MK-II", "MK-III", "Other", "Schpeltiger"};

static inline const char* UAA_GetMotionNameSafe(int id) {
    if (id >= 0 && id < UAA_MAX_MOTIONS) {
        const char* n = MoveNames[id];
        if (n && n[0] != '\0')
            return n;
    }
    return "Motion";
}
static inline UAAWeaponCat UAA_ClassifyWeaponById(int id) {
    if (id >= 206 && id <= 253)
        return UAA_CAT_BLOOD_BERRY;
    if (id >= 302 && id <= 348)
        return UAA_CAT_MK_I;
    if (id >= 349 && id <= 402)
        return UAA_CAT_MK_II;
    if (id >= 254 && id <= 301)
        return UAA_CAT_MK_III;
    return UAA_CAT_OTHER;
}
static inline const char* UAA_CatName(UAAWeaponCat c) {
    return UAA_CAT_LABELS[(int)c];
}

static void UAA_ResetAll() {
    for (int i = 0; i < UAA_MAX_MOTIONS; ++i)
        g_uaa_motion_uses[i] = 0;
    g_uaa_last_motion_no = -1;
}
static void UAA_TrackCurrentMotion() {
    if (!g_uaa_enabled)
        return;
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player)
        return;

    int cur = (int)player->mCharaStatus.motionNo;
    if (cur != g_uaa_last_motion_no) {
        if (cur >= 0 && cur < UAA_MAX_MOTIONS)
            g_uaa_motion_uses[cur] += 1;
        g_uaa_last_motion_no = cur;
    }
}

// ----------------------------- Persistence helpers --------------------------
static void UAA_SaveToConfig(utility::Config& cfg) {
    cfg.set<bool>("Tony", Tony::mod_enabled);
    cfg.set<bool>("Tony_UAA_AssassinEvaluation", g_uaa_enabled);

    for (int i = 0; i < UAA_MAX_MOTIONS; ++i) {
        char key[32];
        std::snprintf(key, sizeof(key), "Tony_UAA_MU_%03d", i);
        cfg.set<int>(key, g_uaa_motion_uses[i]);
    }

    cfg.set<int>("Tony_UAA_S_Time", (int)std::round(g_schpeltiger_time_sec));
    cfg.set<int>("Tony_UAA_S_DonutTime", (int)std::round(g_schpeltiger_donut_time_sec));
    cfg.set<int>("Tony_UAA_S_CrashCount", g_schpeltiger_crash_count);
    cfg.set<int>("Tony_UAA_S_NitroTime", (int)std::round(g_schpeltiger_nitro_time_sec));
    cfg.set<int>("Tony_UAA_S_NitroUses", g_schpeltiger_nitro_uses);

    cfg.set<long long>("Tony_AE_TotalXP", (long long)g_ae_total_xp);
    cfg.set<float>("Tony_AE_PosX", g_ae_pos_x);
    cfg.set<float>("Tony_AE_PosY", g_ae_pos_y);
}
static void UAA_LoadFromConfig(const utility::Config& cfg) {
    Tony::mod_enabled = cfg.get<bool>("Tony").value_or(false);
    g_uaa_enabled     = cfg.get<bool>("Tony_UAA_AssassinEvaluation").value_or(false);

    for (int i = 0; i < UAA_MAX_MOTIONS; ++i) {
        char key[32];
        std::snprintf(key, sizeof(key), "Tony_UAA_MU_%03d", i);
        g_uaa_motion_uses[i] = cfg.get<int>(key).value_or(0);
    }

    g_schpeltiger_time_sec       = (float)cfg.get<int>("Tony_UAA_S_Time").value_or(0);
    g_schpeltiger_donut_time_sec = (float)cfg.get<int>("Tony_UAA_S_DonutTime").value_or(0);
    g_schpeltiger_crash_count    = cfg.get<int>("Tony_UAA_S_CrashCount").value_or(0);
    g_schpeltiger_nitro_time_sec = (float)cfg.get<int>("Tony_UAA_S_NitroTime").value_or(0);
    g_schpeltiger_nitro_uses     = cfg.get<int>("Tony_UAA_S_NitroUses").value_or(0);

    g_ae_total_xp   = (uint64_t)cfg.get<long long>("Tony_AE_TotalXP").value_or(0);
    g_ae_last_level = AE_LevelFromXP(g_ae_total_xp); // prevent CA on load

    g_ae_pos_x = cfg.get<float>("Tony_AE_PosX").value_or(g_ae_pos_x);
    g_ae_pos_y = cfg.get<float>("Tony_AE_PosY").value_or(g_ae_pos_y);

    g_ae_slide_state = g_uaa_enabled ? AE_Visible : AE_Hidden;
    g_ae_slide_t     = 0.0f;
}

// ----------------------------- UAA report window -----------------------------
static void UAA_DrawReportWindow() {
    if (!g_uaa_report_open)
        return;

    ImGui::SetNextWindowSize(ImVec2(900.0f, 600.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(
            "UAA Assassin Performance Report", &g_uaa_report_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {

        ImGui::SetNextItemWidth(220.0f);
        ImGui::Combo("Category", &g_uaa_weapon_filter, UAA_CAT_LABELS, IM_ARRAYSIZE(UAA_CAT_LABELS));

        if (g_uaa_weapon_filter == UAA_CAT_SCHPELTIGER) {
            if (ImGui::BeginTable(
                    "UAA_BikeTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthStretch, 0.6f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.4f);
                ImGui::TableHeadersRow();

                auto fmt_time = [](char* out, size_t outsz, float sec) {
                    int isec = (int)sec, h = isec / 3600, m = isec % 3600 / 60, s = isec % 60;
                    if (h > 0)
                        std::snprintf(out, outsz, "%d:%02d:%02d (%.2fs)", h, m, s, sec);
                    else
                        std::snprintf(out, outsz, "%02d:%02d (%.2fs)", m, s, sec);
                };
                char buf[96];

                fmt_time(buf, sizeof(buf), g_schpeltiger_time_sec);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Time Spent On Schpeltiger");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", buf);

                fmt_time(buf, sizeof(buf), g_schpeltiger_donut_time_sec);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Time Spent Doing Donuts");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", buf);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Number of Times Crashed");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", g_schpeltiger_crash_count);

                fmt_time(buf, sizeof(buf), g_schpeltiger_nitro_time_sec);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Time Spent Using Nitro");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", buf);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Times Used Nitro");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", g_schpeltiger_nitro_uses);

                ImGui::EndTable();
            }
            ImGui::TextDisabled("Note: bike timers accumulate only while UAA Assassin Evaluation is enabled.");
        } else {
            ImGui::SameLine();
            ImGui::Checkbox("Show only used motions", &g_uaa_hide_zeros);

            int distinct_used     = 0;
            long long total_plays = 0;
            for (int i = 0; i < UAA_MAX_MOTIONS; ++i) {
                if (g_uaa_motion_uses[i] > 0)
                    distinct_used++;
                total_plays += g_uaa_motion_uses[i];
            }
            ImGui::SameLine();
            ImGui::Text("| Distinct used: %d / %d  | Total plays: %lld", distinct_used, UAA_MAX_MOTIONS, total_plays);

            ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
                                    ImGuiTableFlags_SizingStretchProp;
            if (ImGui::BeginTable("UAA_Table", 4, flags)) {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Weapon", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                ImGui::TableSetupColumn("Motion Name", ImGuiTableColumnFlags_WidthStretch, 0.0f);
                ImGui::TableSetupColumn("Uses", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableHeadersRow();

                for (int i = 0; i < UAA_MAX_MOTIONS; ++i) {
                    const int uses = g_uaa_motion_uses[i];
                    if (g_uaa_hide_zeros && uses == 0)
                        continue;
                    UAAWeaponCat cat = UAA_ClassifyWeaponById(i);
                    if (g_uaa_weapon_filter != UAA_CAT_ALL && cat != (UAAWeaponCat)g_uaa_weapon_filter)
                        continue;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", i);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", UAA_CatName(cat));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", UAA_GetMotionNameSafe(i));
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", uses);
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
}

// ============================================================================
// Existing Score Visualizer code (unchanged UI; now triggers scope pulses)
// ============================================================================
static constexpr float DISPLAY_DURATION           = 2.0f;
static constexpr float SLIDE_OUT_DURATION         = 0.3f;
static constexpr float ANIMATION_DURATION         = 0.2f;
static constexpr float SCREEN_WIDTH_RATIO         = 0.3f;
static constexpr float REWARD_WINDOW_HEIGHT_RATIO = 0.15f;
static constexpr float TRICK_WINDOW_HEIGHT_RATIO  = 0.3f;
static constexpr float REWARD_START_Y_RATIO       = 0.25f;
static constexpr float TRICK_START_Y_RATIO        = 0.35f;
static constexpr float TEXT_SPACING_RATIO         = 0.5f;
static constexpr float SCREEN_MARGIN_RATIO        = 0.01f;
static constexpr float LINE_HEIGHT_MULTIPLIER     = 2.0f;

static constexpr float SCORE_POP_DURATION  = 0.15f;
static constexpr float SCORE_POP_AMPLITUDE = 0.35f;

static constexpr float CA_DURATION                = 0.0090f;
static constexpr float CA_MAX_OFFSET_F            = 0.6f;
static constexpr float CA_BASE_ALPHA              = 0.75f;
static constexpr bool CA_TRAILER_STYLE            = true;
static constexpr float CA_TRAILER_OFFSET_MULT     = 1.75f;
static constexpr float CA_TRAILER_ALPHA_MULT      = 1.35f;
static constexpr int CA_TRAILER_EXTRA_PASSES      = 1;
static constexpr float CA_TRAILER_PASS_OFFSET_PX  = 0.5f;
static constexpr float CA_TRAILER_CENTER_NUDGE_PX = 0.6f;

static constexpr float AMT_MIN_MULT = 0.35f;
static constexpr float AMT_MAX_MULT = 3.00f;

static constexpr float COUNT_RATE_PER_SEC = 200.0f;
static constexpr float COUNT_MIN_DURATION = 0.18f;
static constexpr float COUNT_MAX_DURATION = 0.90f;

static inline float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static inline float easeOutCubic(float t) {
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

bool Tony::mod_enabled    = false;
uintptr_t Tony::jmp_ret1  = NULL;
uintptr_t Tony::jmp_ret2  = NULL;
uintptr_t Tony::jmp_ret3  = NULL;
uintptr_t Tony::jmp_ret4  = NULL;
uintptr_t Tony::jmp_ret5  = NULL;
uintptr_t Tony::jmp_ret6  = NULL;
static uintptr_t gpBattle = NULL;

static std::chrono::steady_clock::time_point lastRealTime = std::chrono::steady_clock::now();
static std::chrono::duration<float> accumulatedGameTime   = std::chrono::duration<float>::zero();
static bool wasGamePaused                                 = false;

static float getGameTimeSeconds() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player)
        return accumulatedGameTime.count();

    bool isPaused        = player->mInputMode == 6;
    auto currentRealTime = std::chrono::steady_clock::now();

    if (!isPaused) {
        if (wasGamePaused) {
            lastRealTime  = currentRealTime;
            wasGamePaused = false;
        } else {
            accumulatedGameTime += std::chrono::duration_cast<std::chrono::duration<float>>(currentRealTime - lastRealTime);
            lastRealTime = currentRealTime;
        }
    } else {
        wasGamePaused = true;
    }
    return accumulatedGameTime.count();
}

struct TrickGroup {
    std::string trickName;
    int money;
    bool isReward;
    int count;
    float mostRecentTime;
    float firstAppearanceTime;
    bool isNew;
    float timeRemainingWhenPaused;
    float animationTimeRemainingWhenPaused;
    bool wasSlideOutActive;
    float slideOutProgressWhenPaused;
    float scorePopStartTime;
    int rewardStartValue;
    int rewardEndValue;
    float rewardCountStartTime;

    // Deathblow CA
    bool isDeathblow       = false;
    float labelCAStartTime = -1.0f;
    bool dbcaSoloBoost     = false;
    float labelCASeed      = 0.0f;

    // Scope pulse gating (fade numbers when we punch the scope)
    bool rewardBurstSpawned    = false;
    float rewardBurstStartTime = -1.0f;

    TrickGroup() = default;
    TrickGroup(const std::string& name, int moneyAmount, bool reward, float currentTime)
        : trickName(name)
        , money(moneyAmount)
        , isReward(reward)
        , count(1)
        , mostRecentTime(currentTime)
        , firstAppearanceTime(currentTime)
        , isNew(true)
        , timeRemainingWhenPaused(DISPLAY_DURATION)
        , animationTimeRemainingWhenPaused(ANIMATION_DURATION)
        , wasSlideOutActive(false)
        , slideOutProgressWhenPaused(0.0f)
        , scorePopStartTime(-1.0f)
        , rewardStartValue(reward ? 0 : 0)
        , rewardEndValue(reward ? moneyAmount : 0)
        , rewardCountStartTime(reward ? currentTime : -1.0f) {}
};
static std::vector<TrickGroup> trickGroups;

static inline bool IsDeathblowID(int id) {
    switch (id) {
    case 227:
    case 236:
    case 244:
    case 245:
    case 278:
    case 287:
    case 288:
    case 289:
    case 324:
    case 333:
    case 338:
    case 339:
    case 370:
    case 378:
    case 384:
    case 385:
        return true;
    default:
        return false;
    }
}

static bool IsGamePaused() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    return player && player->mInputMode == 6;
}

static void UpdateTimestampsOnUnpause() {
    float now = getGameTimeSeconds();
    for (auto& group : trickGroups) {
        bool wasSlideOutActive = (group.timeRemainingWhenPaused < 0.0f);
        if (wasSlideOutActive) {
            float slideOutProgress  = std::min(1.0f, -group.timeRemainingWhenPaused / SLIDE_OUT_DURATION);
            float remainingSlideOut = SLIDE_OUT_DURATION * (1.0f - slideOutProgress);
            float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
            group.mostRecentTime    = now - slideOutStartTime - (SLIDE_OUT_DURATION - remainingSlideOut);
        } else {
            float desiredRemainingTime = std::max(0.0f, group.timeRemainingWhenPaused);
            group.mostRecentTime       = now - (DISPLAY_DURATION - desiredRemainingTime);
        }
        if (group.isNew) {
            float desiredAnimationRemainingTime = group.animationTimeRemainingWhenPaused;
            group.firstAppearanceTime           = now - (ANIMATION_DURATION - desiredAnimationRemainingTime);
        }
    }
}
static void StoreRemainingTimeOnPause() {
    float now = getGameTimeSeconds();
    for (auto& group : trickGroups) {
        float elapsed                 = now - group.mostRecentTime;
        group.timeRemainingWhenPaused = DISPLAY_DURATION - elapsed;
        if (group.isNew) {
            float animationElapsed                 = now - group.firstAppearanceTime;
            group.animationTimeRemainingWhenPaused = std::max(0.0f, ANIMATION_DURATION - animationElapsed);
        }
        float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
        if (elapsed > slideOutStartTime) {
            group.wasSlideOutActive          = true;
            float slideOutProgress           = (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION;
            group.slideOutProgressWhenPaused = std::max(0.0f, std::min(1.0f, slideOutProgress));
        } else {
            group.wasSlideOutActive          = false;
            group.slideOutProgressWhenPaused = 0.0f;
        }
    }
}
static void CleanupExpiredGroups() {
    if (IsGamePaused())
        return;
    float now = getGameTimeSeconds();
    trickGroups.erase(std::remove_if(trickGroups.begin(), trickGroups.end(),
                          [now](const TrickGroup& group) { return (now - group.mostRecentTime) > DISPLAY_DURATION; }),
        trickGroups.end());
}

static float CalculateAnimationOffset(const TrickGroup& group, float now, float screenWidth) {
    float elapsed           = now - group.mostRecentTime;
    float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
    if (group.isNew) {
        float t        = std::max(0.0f, std::min(1.0f, (now - group.firstAppearanceTime) / ANIMATION_DURATION));
        const float c1 = 0.7f, c3 = c1 + 1.0f;
        float t1    = t - 1.0f;
        float eased = 1.0f + c3 * (t1 * t1 * t1) + c1 * (t1 * t1);
        return -screenWidth * (1.0f - eased);
    } else if (elapsed > slideOutStartTime) {
        float slideOutProgress = std::max(0.0f, std::min(1.0f, (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION));
        slideOutProgress       = slideOutProgress * slideOutProgress * slideOutProgress;
        return -screenWidth * slideOutProgress;
    }
    return 0.0f;
}
static void MarkAnimationComplete(TrickGroup& group, float now) {
    if (group.isNew) {
        float timeSinceFirstAppearance = now - group.firstAppearanceTime;
        if (timeSinceFirstAppearance >= ANIMATION_DURATION)
            group.isNew = false;
    }
}
static int RewardDisplayValue(const TrickGroup& group, float now) {
    if (!group.isReward)
        return group.money;
    int start = group.rewardStartValue, end = group.rewardEndValue;
    float t0 = group.rewardCountStartTime;
    if (t0 < 0.0f || start == end)
        return end;
    int delta = std::abs(end - start);
    float dur = std::min(COUNT_MAX_DURATION, std::max(COUNT_MIN_DURATION, (float)delta / COUNT_RATE_PER_SEC));
    float t   = std::min(1.0f, std::max(0.0f, (getGameTimeSeconds() - t0) / dur));
    float e   = 1.0f - (1.0f - t) * (1.0f - t);
    int value = start + (int)std::round((end - start) * e);
    if ((end - start) > 0)
        value = std::min(value, end);
    else
        value = std::max(value, end);
    return value;
}

// ---------------------- Deathblow CA -----------------------------------------
static constexpr bool DBCA_ENABLED = true;
static float DBCA_DURATION         = 0.22f;
static float DBCA_OFFSET_F         = 0.75f;
static float DBCA_ALPHA            = 0.90f;
static float DBCA_FADE_POW         = 1.30f;
static int DBCA_EXTRA_PASSES       = 1;
static float DBCA_PASS_OFFSET_PX   = 0.50f;
static float DBCA_CENTER_NUDGE     = 0.40f;

static float DBCA_SOLO_MULT       = 2.00f;
static float DBCA_SOLO_ALPHA_MULT = 1.20f;

static void RenderDeathblowLabelCA(const std::string& labelText, float leftAlignX, float yPos, float fontSize, const TrickGroup& group) {
    if (!DBCA_ENABLED)
        return;
    if (!group.isDeathblow)
        return;
    if (group.labelCAStartTime < 0.0f)
        return;

    float now = getGameTimeSeconds();
    float dt  = now - group.labelCAStartTime;
    if (dt < 0.0f || dt > DBCA_DURATION)
        return;

    float t01  = clamp01(dt / DBCA_DURATION);
    float fade = std::pow(1.0f - t01, DBCA_FADE_POW);

    float boost    = group.dbcaSoloBoost ? DBCA_SOLO_MULT : 1.0f;
    float alphaMul = group.dbcaSoloBoost ? DBCA_SOLO_ALPHA_MULT : 1.0f;

    float px = fontSize * DBCA_OFFSET_F * easeOutCubic(1.0f - t01) * boost;

    float seed = (group.labelCASeed != 0.0f) ? group.labelCASeed : (float)((uintptr_t)&group & 0xFFFF);
    float jx   = (sc_hash21(seed + 12.3f, group.labelCAStartTime + 19.7f) - 0.5f) * 2.0f * DBCA_CENTER_NUDGE;
    float jy   = (sc_hash21(seed + 21.1f, group.labelCAStartTime + 7.3f) - 0.5f) * 2.0f * (DBCA_CENTER_NUDGE * 0.6f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* font   = ImGui::GetFont();
    ImVec2 win     = ImGui::GetWindowPos();

    ImU32 cR = IM_COL32(255, 26, 26, (int)std::round(255.0f * std::min(1.0f, 0.90f * fade * alphaMul)));
    ImU32 cG = IM_COL32(26, 255, 26, (int)std::round(255.0f * std::min(1.0f, 0.90f * fade * alphaMul)));
    ImU32 cB = IM_COL32(38, 115, 255, (int)std::round(255.0f * std::min(1.0f, 0.90f * fade * alphaMul)));

    const int maps[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    int permPick         = (int)(sc_hash21(seed + 0.123f, group.labelCAStartTime + 0.789f) * 6.0f) % 6;

    bool flipLR  = sc_hash11(seed + 3.14159f) > 0.5f;
    float leftX  = flipLR ? (+px) : (-px);
    float rightX = -leftX;

    float posX[3] = {leftX, jx, rightX};
    float posY[3] = {0.0f, jy, 0.0f};

    ImVec2 posBase(win.x + leftAlignX, win.y + yPos);
    ImU32 cols[3] = {cR, IM_COL32(255, 255, 255, (int)std::round(255.0f * 0.9f * fade * alphaMul)), cB};

    for (int i = 0; i < 3; ++i) {
        int colorIdx = maps[permPick][i];
        float x = posX[i], y = posY[i];
        dl->AddText(font, fontSize, ImVec2(posBase.x + x, posBase.y + y), cols[colorIdx], labelText.c_str());
        if (i != 1) {
            for (int p = 0; p < DBCA_EXTRA_PASSES; ++p) {
                float o = (p + 1) * DBCA_PASS_OFFSET_PX * (i == 0 ? -1.0f : +1.0f);
                dl->AddText(font, fontSize, ImVec2(posBase.x + x + o, posBase.y + y), cols[colorIdx], labelText.c_str());
            }
        }
    }
}

// ------------------------------ Group renderer --------------------------------
static void RenderGroupText(TrickGroup& group, float animationOffset, float yPos, float fontSize, float screenWidth, bool isRewardGroup) {
    ImColor orangeCol = ImColor(0.970f, 0.803f, 0.165f, 1.00f);
    ImColor whiteCol  = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 textColor  = isRewardGroup ? (ImVec4)orangeCol : (ImVec4)whiteCol;

    ImVec4 shadowColor(0.0f, 0.0f, 0.0f, 1.0f);
    float shadowOffsetX = 2.0f, shadowOffsetY = 2.0f;

    std::string displayText = group.trickName;
    float now               = getGameTimeSeconds();

    float popAlpha = 0.0f;
    if (group.isNew) {
        float t  = std::max(0.0f, std::min(1.0f, (now - group.firstAppearanceTime) / ANIMATION_DURATION));
        popAlpha = (1.0f - t);
    }

    float textWidth     = ImGui::CalcTextSize(displayText.c_str()).x;
    float customSpacing = fontSize * TEXT_SPACING_RATIO;
    float targetX       = screenWidth * SCREEN_MARGIN_RATIO;
    float leftAlignX    = targetX + animationOffset;
    float scoreX        = targetX + textWidth + customSpacing + animationOffset;

    if (!isRewardGroup && group.isDeathblow) {
        RenderDeathblowLabelCA(displayText, leftAlignX, yPos, fontSize, group);
    }

    ImGui::SetCursorPos(ImVec2(leftAlignX + shadowOffsetX, yPos + shadowOffsetY));
    ImGui::PushStyleColor(ImGuiCol_Text, shadowColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();

    if (popAlpha > 0.001f) {
        ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, std::min(1.0f, 0.6f * popAlpha)));
        ImGui::Text("%s", displayText.c_str());
        ImGui::PopStyleColor();
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* font   = ImGui::GetFont();
    ImVec2 win     = ImGui::GetWindowPos();

    float popScale = 1.0f;
    ImVec2 popOffset(0.0f, 0.0f);

    if (!isRewardGroup) {
        if (group.scorePopStartTime >= 0.0f) {
            float dtPop = now - group.scorePopStartTime;
            if (dtPop >= 0.0f && dtPop <= SCORE_POP_DURATION) {
                float t     = dtPop / SCORE_POP_DURATION;
                float decay = (1.0f - t);
                float amp   = SCORE_POP_AMPLITUDE;
                popScale    = 1.0f + amp * decay * decay;

                char previewBuf[64];
                std::snprintf(previewBuf, sizeof(previewBuf), " +%d", group.money);
                ImVec2 scoreSize = ImGui::CalcTextSize(previewBuf);

                float seed = (float)((uintptr_t)&group & 0xFFFF);
                float ang  = sc_hash21(seed + 33.0f, group.scorePopStartTime + 77.0f) * 6.2831853f;
                float dirx = std::cos(ang);
                float diry = std::sin(ang);

                popOffset.x = -(popScale - 1.0f) * 0.5f * scoreSize.x * dirx;
                popOffset.y = -(popScale - 1.0f) * 0.5f * fontSize * diry;
            }
        }
    }

    // Numeric text
    char buf[64];
    int dispValue = 0;
    if (isRewardGroup) {
        int disp  = RewardDisplayValue(group, now);
        dispValue = disp;
        std::snprintf(buf, sizeof(buf), " =%d", disp);
    } else {
        std::snprintf(buf, sizeof(buf), " +%d", group.money);
    }

    ImVec2 posShadow(win.x + scoreX + 2.0f + popOffset.x, win.y + yPos + 2.0f + popOffset.y);
    ImVec2 posMain(win.x + scoreX + popOffset.x, win.y + yPos + popOffset.y);

    float alphaMul = 1.0f;
    if (isRewardGroup && group.rewardBurstStartTime >= 0.0f) {
        float dtf = now - group.rewardBurstStartTime;
        float t   = std::min(1.0f, std::max(0.0f, dtf / 0.12f));
        alphaMul  = 1.0f - 0.70f * t;
    }
    ImU32 mainCol = IM_COL32(247, 205, 42, (int)std::round(255.0f * alphaMul));
    ImU32 shadCol = IM_COL32(0, 0, 0, (int)std::round(255.0f * alphaMul));

    dl->AddText(font, fontSize * popScale, posShadow, shadCol, buf);
    dl->AddText(font, fontSize * popScale, posMain, mainCol, buf);

    // Trigger an oscilloscope pulse when the count finishes (once)
    if (isRewardGroup) {
        bool finished = (dispValue == group.rewardEndValue) && (group.rewardEndValue > 0);
        if (finished && !group.rewardBurstSpawned) {
            int budget = group.rewardEndValue;
            OSC_AddPulse(budget); // replaces chip burst

            group.rewardBurstSpawned   = true;
            group.rewardBurstStartTime = now;
        }
    }
}

// ------------------------------ Display groups --------------------------------
static void DisplayGroups(
    const ImVec2& screenSize, float fontSize, float startY, bool isRewardGroup, const char* windowName, float windowHeightRatio) {
    if (IsGamePaused())
        return;

    float now = getGameTimeSeconds();

    std::vector<std::reference_wrapper<TrickGroup>> activeGroups;
    for (auto& group : trickGroups)
        if (group.isReward == isRewardGroup)
            activeGroups.emplace_back(group);
    if (activeGroups.empty())
        return;

    std::sort(activeGroups.begin(), activeGroups.end(),
        [](const TrickGroup& a, const TrickGroup& b) { return a.firstAppearanceTime > b.firstAppearanceTime; });

    ImGui::SetNextWindowPos(ImVec2(0.0f, startY));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * SCREEN_WIDTH_RATIO, screenSize.y * windowHeightRatio));
    ImGui::Begin(windowName, nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(g_framework->get_our_imgui_ctx()->tony_font, fontSize);

    float lineHeight = fontSize * LINE_HEIGHT_MULTIPLIER;
    for (size_t i = 0; i < activeGroups.size(); ++i) {
        TrickGroup& group     = activeGroups[i].get();
        float animationOffset = CalculateAnimationOffset(group, now, screenSize.x);
        float yPos            = lineHeight * (float)i;

        RenderGroupText(group, animationOffset, yPos, fontSize, screenSize.x, isRewardGroup);
        MarkAnimationComplete(group, now);
    }

    ImGui::PopFont();
    ImGui::End();
}

static void StartOrRestartRewardCount(TrickGroup& g, float now) {
    int currentDisplay     = RewardDisplayValue(g, now);
    g.rewardStartValue     = currentDisplay;
    g.rewardEndValue       = g.money;
    g.rewardCountStartTime = now;

    g.rewardBurstSpawned   = false;
    g.rewardBurstStartTime = -1.0f;
}

static bool AnyActiveDeathblowVisible(float now) {
    for (const auto& tg : trickGroups) {
        if (!tg.isReward && tg.isDeathblow) {
            float alive = now - tg.mostRecentTime;
            if (alive <= DISPLAY_DURATION)
                return true;
        }
    }
    return false;
}

// Core entry point for Score Visualizer (+ feeds Assassin Experience)
static void AddTrickScore(int id, int money, bool isReward) {
    if (IsGamePaused())
        return;

    if (!isReward) {
        if (money > 0)
            g_ae_total_xp += (uint64_t)money; // per-hit XP stays immediate
    } else {
        // If the visualizer itself is off, apply reward immediately
        if (!Tony::mod_enabled) {
            if (money > 0)
                g_ae_total_xp += (uint64_t)money;
        }
    }

    constexpr int MOVE_NAMES_SIZE = sizeof(MoveNames) / sizeof(MoveNames[0]);
    std::string trickName;
    if (isReward) {
        trickName = "Kill Reward";
    } else if (id >= 0 && id < MOVE_NAMES_SIZE && MoveNames[id] != nullptr) {
        trickName = MoveNames[id];
    } else {
        trickName = std::to_string(id);
    }

    float now             = getGameTimeSeconds();

    bool foundExisting = false;
    if (!trickGroups.empty()) {
        for (auto it = trickGroups.rbegin(); it != trickGroups.rend(); ++it) {
            if (it->isReward == isReward && it->trickName == trickName) {
                it->count++;
                it->money += money;
                it->mostRecentTime = now;
                it->isNew          = false;

                if (!isReward) {
                    it->scorePopStartTime = now;
                    if (it->isDeathblow) {
                        it->labelCAStartTime = now;
                        it->dbcaSoloBoost    = false;
                        it->labelCASeed      = sc_hash21((float)id + 0.5f, now + 1.0f);
                    }
                } else {
                    StartOrRestartRewardCount(*it, now);
                }
                foundExisting = true;
                break;
            }
        }
    }

    if (!foundExisting) {
        TrickGroup newGroup(trickName, money, isReward, now);
        if (isReward) {
            newGroup.rewardStartValue     = 0;
            newGroup.rewardEndValue       = money;
            newGroup.rewardCountStartTime = now;
            trickGroups.push_back(newGroup);
        } else {
            newGroup.isDeathblow = IsDeathblowID(id);
            if (newGroup.isDeathblow) {
                bool anyActive            = AnyActiveDeathblowVisible(now);
                newGroup.dbcaSoloBoost    = !anyActive;
                newGroup.labelCAStartTime = now;
                newGroup.labelCASeed      = sc_hash21((float)id + 0.5f, now + 1.0f);
            }
            trickGroups.insert(std::begin(trickGroups), newGroup);
        }
    }
}

static void AddTrickScore2(const char* trickName, int money, bool isReward) {
    if (IsGamePaused())
        return;

    if (!isReward) {
        if (money > 0)
            g_ae_total_xp += (uint64_t)money;
    } else {
        if (!Tony::mod_enabled) {
            if (money > 0)
                g_ae_total_xp += (uint64_t)money;
        }
    }

    std::string trickNameStr = trickName;
    float now                = getGameTimeSeconds();

    bool foundExisting = false;
    if (!trickGroups.empty()) {
        for (auto it = trickGroups.rbegin(); it != trickGroups.rend(); ++it) {
            if (it->isReward == isReward && it->trickName == trickNameStr) {
                it->count++;
                it->money += money;
                it->mostRecentTime = now;
                it->isNew          = false;

                if (!isReward) {
                    it->scorePopStartTime = now;
                    if (it->isDeathblow) {
                        it->labelCAStartTime = now;
                        it->dbcaSoloBoost    = false;
                        it->labelCASeed      = sc_hash21((float)it->count + 0.5f, now + 1.0f);
                    }
                } else {
                    StartOrRestartRewardCount(*it, now);
                }
                foundExisting = true;
                break;
            }
        }
    }

    if (!foundExisting) {
        TrickGroup newGroup(trickNameStr, money, isReward, now);
        if (isReward) {
            newGroup.rewardStartValue     = 0;
            newGroup.rewardEndValue       = money;
            newGroup.rewardCountStartTime = now;
            trickGroups.push_back(newGroup);
        } else {
            newGroup.isDeathblow = (trickNameStr.find("DEATH BLOW") != std::string::npos);
            if (newGroup.isDeathblow) {
                bool anyActive            = AnyActiveDeathblowVisible(now);
                newGroup.dbcaSoloBoost    = !anyActive;
                newGroup.labelCAStartTime = now;
                newGroup.labelCASeed      = sc_hash21((float)trickNameStr.size() + 0.25f, now + 1.0f);
            }
            trickGroups.insert(std::begin(trickGroups), newGroup);
        }
    }
}

// ============================================================================
// Slide logic used by AE overlay
// ============================================================================
static float AE_UpdateSlideAndGetOffsetX(float dt) {
    if (g_uaa_enabled) {
        if (g_ae_slide_state == AE_Hidden || g_ae_slide_state == AE_SlidingOut) {
            g_ae_slide_state = AE_SlidingIn;
            g_ae_slide_t     = 0.0f;
        }
    } else {
        if (g_ae_slide_state == AE_Visible || g_ae_slide_state == AE_SlidingIn) {
            g_ae_slide_state = AE_SlidingOut;
            g_ae_slide_t     = 0.0f;
        }
    }

    const float speed = 8.0f;
    if (g_ae_slide_state == AE_SlidingIn || g_ae_slide_state == AE_SlidingOut) {
        g_ae_slide_t += dt * speed;
        if (g_ae_slide_t >= 1.0f) {
            g_ae_slide_t     = 1.0f;
            g_ae_slide_state = (g_ae_slide_state == AE_SlidingIn) ? AE_Visible : AE_Hidden;
        }
    }

    const float off = AE_WINDOW_W + 60.0f;
    if (g_ae_slide_state == AE_SlidingIn) {
        float t = g_ae_slide_t;
        float e = 1.0f - (1.0f - t) * (1.0f - t);
        return -(1.0f - e) * off;
    }
    if (g_ae_slide_state == AE_SlidingOut) {
        float t = g_ae_slide_t;
        float e = t * t;
        return e * off;
    }
    if (g_ae_slide_state == AE_Visible)
        return 0.0f;
    return -off;
}

// ============================================================================
// Frame hook
// ============================================================================
void Tony::on_frame() {
    // UAA motion tracking
    UAA_TrackCurrentMotion();

    // Schpeltiger timers/counters (only when UAA enabled)
    {
        float now = getGameTimeSeconds();
        if (g_s_last_game_time < 0.0f)
            g_s_last_game_time = now;
        float dt = now - g_s_last_game_time;
        if (dt < 0.0f)
            dt = 0.0f;
        g_s_last_game_time = now;

        if (g_uaa_enabled && dt > 0.0f) {
            mHRPc* player = nmh_sdk::get_mHRPc();
            if (player) {
                if ((int)player->mInputMode == 5) {
                    g_schpeltiger_time_sec += dt;
                }

                if (player->mpBike) {
                    float spinY = player->mpBike->mBike.spinY;
                    float rate  = (dt > 0.0f) ? std::fabs(spinY - g_s_last_spinY) / dt : 0.0f;
                    if (rate > DONUT_SPINY_RATE_EPS) {
                        g_schpeltiger_donut_time_sec += dt;
                    }
                    g_s_last_spinY = spinY;

                    bool curClash = player->mpBike->mBike.clash;
                    if (!g_s_last_clash && curClash) {
                        g_schpeltiger_crash_count += 1;
                    }
                    g_s_last_clash = curClash;

                    bool curNitro = player->mpBike->mBike.useNitro;
                    if (curNitro) {
                        g_schpeltiger_nitro_time_sec += dt;
                    }
                    if (!g_s_last_useNitro && curNitro) {
                        g_schpeltiger_nitro_uses += 1;
                    }
                    g_s_last_useNitro = curNitro;
                }
            }
        }
    }

    // Detect rank-up (trigger CA burst)
    {
        int curLvl = AE_LevelFromXP(g_ae_total_xp);
        if (curLvl > g_ae_last_level) {
            g_ae_rank_ca_start_t = getGameTimeSeconds();
            g_ae_last_level      = curLvl;
        }
    }

    // Score Visualizer draw (when enabled)
    if (mod_enabled) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            bool isPaused               = IsGamePaused();
            static bool lastFramePaused = false;

            if (!lastFramePaused && isPaused) {
                StoreRemainingTimeOnPause();
            } else if (lastFramePaused && !isPaused) {
                UpdateTimestampsOnUnpause();
            }
            lastFramePaused = isPaused;

            if (!isPaused)
                CleanupExpiredGroups();

            bool hasRewards = false, hasTricks = false;
            for (const auto& group : trickGroups) {
                if (group.isReward)
                    hasRewards = true;
                else
                    hasTricks = true;
                if (hasRewards && hasTricks)
                    break;
            }

            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            float fontSize    = 14.0f * (screenSize.y / 1080.0f);
            if (fontSize <= 0.0f)
                fontSize = 14.0f;

            float rewardStartY = screenSize.y * REWARD_START_Y_RATIO;
            float trickStartY  = screenSize.y * TRICK_START_Y_RATIO;

            if (hasRewards)
                DisplayGroups(screenSize, fontSize, rewardStartY, true, "RewardWindow", REWARD_WINDOW_HEIGHT_RATIO);
            if (hasTricks)
                DisplayGroups(screenSize, fontSize, trickStartY, false, "TrickWindow", TRICK_WINDOW_HEIGHT_RATIO);
        }
    }

    // Draw pop-outs and overlays
    UAA_DrawReportWindow();
    AE_DrawWindow();     // updates intake anchor
    OSC_UpdateAndDraw(); // draw scope after AE so anchor is fresh
}

// ============================================================================
// Detours (unchanged except we rely on AddTrickScore feeding AE/scope)
// ============================================================================
naked void detour1() { // most attacks // player in edi
    __asm {
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode

        pushad
        push [esp+0x20+0xC] // damage
        push ecx       // moveID
        call AddTrickScore
        add esp, 8
        popad

    originalcode:
        cmp ecx,0x000000AB
        jmp dword ptr [Tony::jmp_ret1]
    }
}
naked void detour2() {                            // execution qtes // player in edi
    __asm {
        cmp byte ptr[Tony::mod_enabled], 0
        je originalcode

        pushad
        push 0
        push 0
        push [edi+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC
        popad

    originalcode:
        mov edx, 0x00000379
        jmp dword ptr [Tony::jmp_ret2]
    }
}

static constexpr const char* throwSuccessName = "Successful input";
naked void detour3() {   // throw input success // player in edi
    __asm {
        mov eax, [ecx+0x0000030C]
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode

        pushad
        push 0 // is reward
        push 0 // money
        push [edi+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC

    popcode:
        popad
    originalcode:
        jmp dword ptr [Tony::jmp_ret3]
    }
}
naked void detour4() {   // +5 money gains // player in edi
    __asm {
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode

        pushad
        push 0 // is reward
        push 5 // money gain (per-hit increment trigger)
        push [edi+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC
        popad

    originalcode:
        cmp ecx, 0x000000C2
        jmp dword ptr [Tony::jmp_ret4]
    }
}
naked void detour5() {       // money rewards // player in edi
    __asm {
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode

        pushad
        push 1 // is reward
        push eax // money gained
        push [ecx+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC
        popad

    originalcode:
        add [ecx+0x000012EC], eax
        jmp dword ptr [Tony::jmp_ret5]
    }
}

static void CheckNonDamagingAnims(int moveID) {
    // compare current moveid to a list of player anims that do not deal damage
    // if it is the move id you want to display on tony, call AddTrickScore.
    // if not, do not call AddTrickScore.
    AddTrickScore(moveID, 0, false);
}

static bool displayEveryAnim = false;
naked void detour6() { // money rewards // player in edi
    __asm {
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode

        cmp byte ptr [displayEveryAnim], 1
        jne originalcode
        pushad
        push edx // moveID
        call CheckNonDamagingAnims
        add esp,4
        popad

    originalcode:
        cmp edx, 0x000000CD
        jmp dword ptr [Tony::jmp_ret6]
    }
}

// ============================================================================
// UI toggles (main menu panel) - keep simple and clear
// ============================================================================
void Tony::on_draw_ui() {
    ImGui::SeparatorText("UAA Assassin Evaluation");
    ImGui::Checkbox("UAA Assassin Evaluation", &g_uaa_enabled);

    ImGui::Indent();
    if (ImGui::Button("Assassin Performance Report")) {
        g_uaa_report_open = true;
    }
    ImGui::Unindent();

    ImGui::Separator();
    ImGui::Checkbox("Score Visualizer", &mod_enabled);
    ImGui::Checkbox("[DEBUG] Show ALL animations", &displayEveryAnim);
}

std::optional<std::string> Tony::on_initialize() {
    gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;

    /*if (!install_hook_offset(0x3CB850, m_hook1, &detour1, &Tony::jmp_ret1, 6)) {
        spdlog::error("Failed to init Tony mod");
        return "Failed to init Tony mod";
    }
    if (!install_hook_offset(0x3CAFA1, m_hook2, &detour2, &Tony::jmp_ret2, 5)) {
        spdlog::error("Failed to init Tony mod 2");
        return "Failed to init Tony mod 2";
    }*/
    if (!install_hook_offset(0xA0D33, m_hook3, &detour3, &Tony::jmp_ret3, 6)) {
        spdlog::error("Failed to init Tony mod 3");
        return "Failed to init Tony mod 3";
    }
    if (!install_hook_offset(0x3CB92D, m_hook4, &detour4, &Tony::jmp_ret4, 6)) {
        spdlog::error("Failed to init Tony mod 4");
        return "Failed to init Tony mod 4";
    }
    if (!install_hook_offset(0x3E1CD6, m_hook5, &detour5, &Tony::jmp_ret5, 6)) {
        spdlog::error("Failed to init Tony mod 5");
        return "Failed to init Tony mod 5";
    }

    if (!install_hook_offset(0x3D7F9C, m_hook6, &detour6, &Tony::jmp_ret6, 6)) {
        spdlog::error("Failed to init Tony mod 6");
        return "Failed to init Tony mod 6";
    }

    return Mod::on_initialize();
}

void Tony::on_config_load(const utility::Config& cfg) {
    UAA_LoadFromConfig(cfg);
}
void Tony::on_config_save(utility::Config& cfg) {
    UAA_SaveToConfig(cfg);
}

#endif
