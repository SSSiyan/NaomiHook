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
//  Assassin Experience (XP + Ranks) — minimal overlay (pretty text only)
// ============================================================================
static bool g_ae_open         = false; // compatibility no-op
static uint64_t g_ae_total_xp = 0;     // persistent XP bucket

// Rank CA trigger
static int g_ae_last_level        = 1;
static float g_ae_rank_ca_start_t = -1.0f; // <0 means inactive

// Position (user-draggable) and persistence
static float g_ae_pos_x = 560.0f; // defaults; saved/loaded
static float g_ae_pos_y = 60.0f;

// Slide animation state
enum AESlideState { AE_Hidden = 0, AE_SlidingIn, AE_Visible, AE_SlidingOut };
static int g_ae_slide_state = AE_Hidden;
static float g_ae_slide_t   = 0.0f; // 0..1 per transition

// Window metrics (shorter height: bar removed)
static constexpr float AE_WINDOW_W = 520.0f;
static constexpr float AE_WINDOW_H = 56.0f;

static float getGameTimeSeconds(); // forward (implemented later)

// ------------------- AE visual theme (chips lerp to lime) --------------------
static constexpr ImU32 AE_BG_U32     = IM_COL32(10, 12, 10, 60);
static constexpr ImU32 AE_BORDER_U32 = IM_COL32(0, 0, 0, 150);

// Kill Reward gold (same as label) both start/end so chips stay gold
static constexpr ImU32 AE_CHIP_START_U32 = IM_COL32(247, 205, 42, 255);
static constexpr ImU32 AE_CHIP_END_U32   = IM_COL32(247, 205, 42, 255);

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

// Rank curve: cumulative XP to reach level L (L>=1). L1 -> 0xp, L2 -> 2k, etc.
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

// -------- Quick chromatic aberration for the RANK label (sharp + short) -----
static constexpr float AE_CA_DURATION = 0.16f; // seconds
static void AE_RenderRankCA(const char* text, ImVec2 pos, float fontPx) {
    float now = getGameTimeSeconds();
    float t0  = g_ae_rank_ca_start_t;
    if (t0 < 0.0f)
        return;
    float dt = now - t0;
    if (dt < 0.0f || dt > AE_CA_DURATION)
        return;

    float u   = dt / AE_CA_DURATION;            // 0..1
    float e   = 1.0f - (1.0f - u) * (1.0f - u); // fast out
    float off = (1.0f - e) * (fontPx * 0.85f);  // px offset shrinking to 0

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* f      = g_framework->get_our_imgui_ctx()->tony_font;

    float jy = (std::sin(now * 60.0f) * 0.5f); // tiny flicker
    dl->AddText(f, fontPx, ImVec2(pos.x - off, pos.y + jy), IM_COL32(255, 64, 64, 210), text);
    dl->AddText(f, fontPx, ImVec2(pos.x + off, pos.y - jy), IM_COL32(64, 255, 255, 210), text);
}

// ------------------------ Intake anchor getter -------------------------------
static inline ImVec2 AE_GetIntakeAnchor() {
    return g_ae_intake_anchor;
}

// =========================== Kill Reward Chip System =========================
// Particles use pause-safe time (getGameTimeSeconds). Drawn on foreground list.

// Tweaked for long travel to lower-left UI without being clipped
static constexpr int KB_MAX_CONCURRENT_BURSTS = 2;
static constexpr int KB_TARGET_CHIP_COUNT     = 90;    // soft target
static constexpr int KB_ABS_MAX_CHIPS         = 120;   // hard cap
static constexpr float KB_PHASEA_MIN          = 0.08f; // shatter phase
static constexpr float KB_PHASEA_MAX          = 0.12f;
static constexpr float KB_TTL_MAX             = 3.50f;  // longer: allow cross-screen flights
static constexpr float KB_INTake_RADIUS       = 7.0f;   // arrival radius
static constexpr float KB_ATTRACT_K           = 36.0f;  // stronger pull
static constexpr float KB_ATTRACT_DIST_MUL    = 0.06f;  // distance-based boost
static constexpr float KB_DAMP                = 0.90f;  // velocity damping
static constexpr float KB_SPEED_MIN           = 220.0f; // px/sec (min cruise)
static constexpr float KB_SPEED_MAX           = 520.0f; // px/sec (base cap; rises with distance)
// strict square sizes (integers) — slightly larger
static constexpr int KB_SIZE_OPTIONS[2] = {3, 4};

static inline float sc_fract(float x) {
    return x - std::floor(x);
}
static inline float sc_hash11(float x) {
    return sc_fract(std::sin(x) * 43758.5453f);
}
static inline float sc_hash21(float x, float y) {
    return sc_fract(std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f);
}

struct KBChip {
    ImVec2 pos;
    ImVec2 vel;
    float delay;    // start delay (stagger)
    float age;      // time since activation (excludes delay)
    float phaseA;   // shatter duration for this chip
    int size;       // 3 or 4 px (integer)
    bool delivered; // landed at intake
};

struct KBBurst {
    bool alive;
    float startTime;
    ImRect spawnRect;
    int xpBudget; // total payout to deliver
    int remainingBudget;
    int xpPerChip;
    std::vector<KBChip> chips;
};

static std::vector<KBBurst> g_kb_bursts;

// Utility: spawn a burst (cap concurrent; if cannot spawn, flush budget immediately)
static void KB_FlushBudget(int budget) {
    if (budget > 0)
        g_ae_total_xp += (uint64_t)budget;
}
static bool KB_SpawnBurst(const ImRect& spawn, int budget, float seedBase) {
    if (budget <= 0)
        return false;

    // Enforce concurrent cap
    if ((int)g_kb_bursts.size() >= KB_MAX_CONCURRENT_BURSTS) {
        // No room: fail gracefully by paying XP instantly
        KB_FlushBudget(budget);
        return false;
    }

    KBBurst b{};
    b.alive           = true;
    b.startTime       = getGameTimeSeconds();
    b.spawnRect       = spawn;
    b.xpBudget        = budget;
    b.remainingBudget = budget;

    // Determine chip payout
    int target  = KB_TARGET_CHIP_COUNT;
    int per     = std::max(1, budget / std::max(1, target));
    int count   = std::min(KB_ABS_MAX_CHIPS, std::max(1, budget / per));
    b.xpPerChip = per;
    b.chips.reserve((size_t)count);

    ImVec2 sz(spawn.Max.x - spawn.Min.x, spawn.Max.y - spawn.Min.y);
    float now = b.startTime;

    for (int i = 0; i < count; ++i) {
        float s = seedBase + (float)i * 19.19f;

        // Jittered grid inside spawnRect
        float jx = sc_hash21(s + 0.13f, now + 3.7f);
        float jy = sc_hash21(s + 2.81f, now + 1.1f);
        ImVec2 p(spawn.Min.x + jx * (sz.x > 1.f ? sz.x : 1.f), spawn.Min.y + jy * (sz.y > 1.f ? sz.y : 1.f));

        // Outward pop with slight upward bias
        float ang = sc_hash21(s + 9.31f, now + 7.77f) * 6.2831853f;
        float spd = KB_SPEED_MIN + (KB_SPEED_MAX - KB_SPEED_MIN) * sc_hash21(s + 4.2f, now + 0.55f);
        ImVec2 v(std::cos(ang) * spd, (std::sin(ang) * spd) - 30.0f);

        // strict integer size (3 or 4 px)
        int szpx = KB_SIZE_OPTIONS[(sc_hash21(s + 6.66f, now + 2.22f) < 0.5f) ? 0 : 1];

        KBChip c{};
        c.pos       = p;
        c.vel       = v;
        c.delay     = sc_hash21(s + 5.55f, now + 8.81f) * 0.08f; // up to 80ms stagger
        c.age       = 0.0f;
        c.phaseA    = KB_PHASEA_MIN + (KB_PHASEA_MAX - KB_PHASEA_MIN) * sc_hash21(s + 0.77f, now + 0.33f);
        c.size      = szpx;
        c.delivered = false;

        b.chips.push_back(c);
    }

    g_kb_bursts.push_back(std::move(b));
    return true;
}

// Snap to pixel grid and draw crisp squares (no rounding, integer edges)
static inline void KB_DrawSquare(ImDrawList* dl, float cx, float cy, int size, ImU32 col) {
    float x0 = std::floor(cx - (float)size * 0.5f);
    float y0 = std::floor(cy - (float)size * 0.5f);
    ImVec2 p0(x0, y0);
    ImVec2 p1(x0 + (float)size, y0 + (float)size);
    dl->AddRectFilled(p0, p1, col); // no rounding: perfect square
}

static void KB_UpdateAndDraw() {
    if (g_kb_bursts.empty())
        return;

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float now      = getGameTimeSeconds();

    // Ensure absolute full-screen clip while drawing chips (prevents any window clipping)
    ImVec2 vp_sz = ImGui::GetIO().DisplaySize;
    dl->PushClipRect(ImVec2(0.0f, 0.0f), ImVec2(vp_sz.x, vp_sz.y), false);

    for (size_t bi = 0; bi < g_kb_bursts.size(); ++bi) {
        KBBurst& b = g_kb_bursts[bi];
        if (!b.alive)
            continue;

        float life = now - b.startTime;
        if (life > KB_TTL_MAX) {
            // Safety flush on timeout
            KB_FlushBudget(b.remainingBudget);
            b.remainingBudget = 0;
            b.alive           = false;
            continue;
        }

        ImVec2 anchor = AE_GetIntakeAnchor();

        for (KBChip& c : b.chips) {
            if (c.delivered)
                continue;

            // Handle startup delay (stagger)
            float ageGlobal = life;
            if (ageGlobal < c.delay)
                continue;
            float dt = std::max(0.0f, (ageGlobal - c.age - c.delay)); // per-frame step in pause-safe time
            c.age    = ageGlobal - c.delay;

            if (dt <= 0.0f)
                continue;

            // Phase A: shatter motion
            if (c.age < c.phaseA) {
                c.pos.x += c.vel.x * dt;
                c.pos.y += c.vel.y * dt;
            } else {
                // Phase B: magnet attraction with distance-based acceleration
                ImVec2 to(anchor.x - c.pos.x, anchor.y - c.pos.y);
                float len = std::sqrt(to.x * to.x + to.y * to.y);
                if (len > 0.0001f) {
                    ImVec2 dir(to.x / len, to.y / len);
                    // Tiny perpendicular curl so the paths differ
                    ImVec2 perp(-dir.y, dir.x);
                    float curl = 4.0f * sc_hash21((float)(uintptr_t)&c, c.age + 1.23f) - 2.0f;

                    // Distance-based pull boost: farther = stronger attraction and higher max cruise
                    float dist_scale    = (len / (vp_sz.y + 1.0f)); // 0..~1 across screen height
                    float K             = KB_ATTRACT_K * (1.0f + KB_ATTRACT_DIST_MUL * (dist_scale * 10.0f));
                    float dyn_max_speed = KB_SPEED_MAX + (len * 0.60f); // allow faster cruise when far

                    // Update velocity (damped)
                    c.vel.x = (c.vel.x + (dir.x * K + perp.x * curl));
                    c.vel.y = (c.vel.y + (dir.y * K + perp.y * curl));
                    c.vel.x *= KB_DAMP;
                    c.vel.y *= KB_DAMP;

                    // Clamp speed into [min, dynamic max]
                    float vlen = std::sqrt(c.vel.x * c.vel.x + c.vel.y * c.vel.y);
                    if (vlen > 0.0001f) {
                        float min_spd = KB_SPEED_MIN;
                        float max_spd = dyn_max_speed;
                        if (vlen < min_spd) {
                            float s = min_spd / vlen;
                            c.vel.x *= s;
                            c.vel.y *= s;
                        } else if (vlen > max_spd) {
                            float s = max_spd / vlen;
                            c.vel.x *= s;
                            c.vel.y *= s;
                        }
                    }

                    // Integrate
                    c.pos.x += c.vel.x * dt;
                    c.pos.y += c.vel.y * dt;
                }
            }

            // Arrival test
            ImVec2 d(anchor.x - c.pos.x, anchor.y - c.pos.y);
            float dist2 = (d.x * d.x + d.y * d.y);
            if (dist2 <= (KB_INTake_RADIUS * KB_INTake_RADIUS)) {
                c.delivered = true;
                if (b.remainingBudget > 0) {
                    int pay = std::min(b.xpPerChip, b.remainingBudget);
                    g_ae_total_xp += (uint64_t)pay;
                    b.remainingBudget -= pay;
                }
            }

            // Chip color (gold)
            float dist = std::sqrt(dist2);
            float tcol = (dist <= 1.0f) ? 1.0f : std::min(1.0f, 1.0f - (dist / 240.0f));
            ImU32 col  = u32_lerp(AE_CHIP_START_U32, AE_CHIP_END_U32, tcol);

            // Draw chip (full-screen clip ensures no cuts)
            KB_DrawSquare(dl, c.pos.x, c.pos.y, c.size, col);
        }

        // If everyone delivered or budget exhausted, finish and flush remainder (should be 0)
        bool anyAlive = false;
        for (const KBChip& c : b.chips) {
            if (!c.delivered) {
                anyAlive = true;
                break;
            }
        }
        if (!anyAlive || b.remainingBudget <= 0) {
            if (b.remainingBudget > 0) {
                KB_FlushBudget(b.remainingBudget);
                b.remainingBudget = 0;
            }
            b.alive = false;
        }
    }

    // Compact dead bursts
    g_kb_bursts.erase(std::remove_if(g_kb_bursts.begin(), g_kb_bursts.end(), [](const KBBurst& b) { return !b.alive; }), g_kb_bursts.end());

    // Pop full-screen clip
    dl->PopClipRect();
}

// ============================================================================
//  UAA Performance Report UI (persisted via on_frame)
// ============================================================================
static float AE_UpdateSlideAndGetOffsetX(float dt); // fwd

// Rank+XP label only (no bar). Intake anchor set to the center of CURRENT XP digits.
static void AE_DrawWindow() {
    // Small dt from game time
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
        return; // not visible

    // Clamp window into the visible viewport so the intake anchor never sits offscreen
    ImVec2 vp  = ImGui::GetIO().DisplaySize;
    g_ae_pos_x = std::max(8.0f, std::min(g_ae_pos_x, vp.x - AE_WINDOW_W - 8.0f));
    g_ae_pos_y = std::max(8.0f, std::min(g_ae_pos_y, vp.y - AE_WINDOW_H - 8.0f));

    // Window placement
    ImVec2 drawPos(g_ae_pos_x + slideOffX, g_ae_pos_y);
    ImGui::SetNextWindowPos(drawPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(AE_WINDOW_W, AE_WINDOW_H), ImGuiCond_Always);

    if (ImGui::Begin("Assassin Experience", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings)) {

        // Drag handle
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("AE_DRAG", ImGui::GetWindowSize());
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            ImVec2 d = ImGui::GetIO().MouseDelta;
            g_ae_pos_x += d.x;
            g_ae_pos_y += d.y;
        }

        // Content
        ImGui::SetCursorPos(ImVec2(0, 4.0f));

        int lvl         = AE_LevelFromXP(g_ae_total_xp);
        uint64_t into   = AE_XPIntoLevel(g_ae_total_xp, lvl);
        uint64_t toNext = AE_XPForNext(g_ae_total_xp, lvl);

        // We draw one string, but compute the anchor using exact font metrics.
        char rankPrefix[64];
        std::snprintf(rankPrefix, sizeof(rankPrefix), "Rank %d    ", lvl);

        char intoStr[32];
        std::snprintf(intoStr, sizeof(intoStr), "%llu", (unsigned long long)into);

        const char* sepStr = " / ";

        char nextStr[32];
        std::snprintf(nextStr, sizeof(nextStr), "%llu", (unsigned long long)toNext);

        // Full line for drawing/CA
        char fullBuf[160];
        std::snprintf(fullBuf, sizeof(fullBuf), "%s%s%s%s", rankPrefix, intoStr, sepStr, nextStr);

        float scale     = ImGui::GetIO().DisplaySize.y / 1080.0f;
        float labelSize = std::max(14.0f, 16.0f * scale);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x += 8.0f; // slight inset

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImFont* tf     = g_framework->get_our_imgui_ctx()->tony_font;

        // Shadow
        dl->AddText(tf, labelSize, ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 190), fullBuf);
        // CA burst if active
        AE_RenderRankCA(fullBuf, pos, labelSize);
        // Main text
        dl->AddText(tf, labelSize, pos, IM_COL32(255, 255, 255, 235), fullBuf);

        // --- Compute intake anchor at the center of the CURRENT XP digits ---
        ImVec2 wPrefix = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, rankPrefix);
        ImVec2 wInto   = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, intoStr);
        ImVec2 hDigit  = tf->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, "0");

        g_ae_intake_anchor = ImVec2(pos.x + wPrefix.x + (wInto.x * 0.5f), // center of the "into" substring (CURRENT XP)
            pos.y + (hDigit.y * 0.5f)                                     // vertical center of the line
        );

        // Tiny bottom padding
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

    // Initialize slide state based on current toggle
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
// Existing Score Visualizer code (kept; now spawns chip bursts on Kill Reward)
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

    // Chip burst integration
    bool rewardBurstSpawned    = false; // spawn only once per completed count
    float rewardBurstStartTime = -1.0f; // used to fade the number after burst starts

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

// ---------------------- Deathblow CA (unchanged behavior) --------------------
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
    ImU32 cols[3] = {cR, cG, cB};

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
    ImU32 scoreCol    = (ImU32)orangeCol;

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

    // Build the numeric text and compute its positions
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

    // If reward burst started, fade the number quickly to reduce overdraw
    float alphaMul = 1.0f;
    if (isRewardGroup && group.rewardBurstStartTime >= 0.0f) {
        float dt = now - group.rewardBurstStartTime;
        float t  = std::min(1.0f, std::max(0.0f, dt / 0.12f)); // 120ms to 30% alpha
        alphaMul = 1.0f - 0.70f * t;
    }
    ImU32 mainCol = IM_COL32(247, 205, 42, (int)std::round(255.0f * alphaMul));
    ImU32 shadCol = IM_COL32(0, 0, 0, (int)std::round(255.0f * alphaMul));

    dl->AddText(font, fontSize * popScale, posShadow, shadCol, buf);
    dl->AddText(font, fontSize * popScale, posMain, mainCol, buf);

    // --- Spawn a chip burst exactly when the count finishes (and only once) ---
    if (isRewardGroup) {
        bool finished = (dispValue == group.rewardEndValue) && (group.rewardEndValue > 0);
        if (finished && !group.rewardBurstSpawned) {
            // Tight spawn rect around the number we just drew
            ImVec2 numSize = ImGui::CalcTextSize(buf);
            ImRect spawnRect(posMain, ImVec2(posMain.x + numSize.x, posMain.y + fontSize));

            // Create a burst; if cap is reached, budget flushes immediately
            int budget = group.rewardEndValue;
            float seed = (float)((uintptr_t)&group & 0xFFFF) + now;
            KB_SpawnBurst(spawnRect, budget, seed);

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

    // Reset burst gate so a new burst can spawn when this count completes
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

    // Assassin Experience feed:
    if (!isReward) {
        if (money > 0)
            g_ae_total_xp += (uint64_t)money; // per-hit XP stays immediate
    } else {
        // Kill reward: if visualizer is enabled, deliver via chip budget.
        // If not enabled, fall back to immediate add to avoid lost XP.
        if (!Tony::mod_enabled) {
            if (money > 0)
                g_ae_total_xp += (uint64_t)money;
        }
        // else: budget is handled by the chip burst when the count finishes
    }

    // Existing visualizer behavior
    std::string trickName = isReward ? "Kill Reward" : MoveNames[id];
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
    // Transitions triggered by g_uaa_enabled
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

    // Advance animation
    const float speed = 8.0f; // fast slide
    if (g_ae_slide_state == AE_SlidingIn || g_ae_slide_state == AE_SlidingOut) {
        g_ae_slide_t += dt * speed;
        if (g_ae_slide_t >= 1.0f) {
            g_ae_slide_t     = 1.0f;
            g_ae_slide_state = (g_ae_slide_state == AE_SlidingIn) ? AE_Visible : AE_Hidden;
        }
    }

    // Off-screen distances
    const float off = AE_WINDOW_W + 60.0f;

    // Compute pixel offset
    if (g_ae_slide_state == AE_SlidingIn) {
        float t = g_ae_slide_t;                   // 0..1
        float e = 1.0f - (1.0f - t) * (1.0f - t); // ease out
        return -(1.0f - e) * off;                 // from left (-off) -> 0
    } else if (g_ae_slide_state == AE_SlidingOut) {
        float t = g_ae_slide_t; // 0..1
        float e = t * t;        // ease in a bit
        return e * off;         // 0 -> +off (to right)
    } else if (g_ae_slide_state == AE_Visible) {
        return 0.0f;
    } else { // hidden: keep it off-screen to the left so it is not visible
        return -off;
    }
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

    // Draw chip bursts over everything (foreground DL)
    KB_UpdateAndDraw();

    // Draw pop-outs LAST so they persist independently of the main menu
    UAA_DrawReportWindow();
    AE_DrawWindow(); // minimal pretty text; anchor updated to the current number
}

// ============================================================================
// Detours (unchanged except we rely on AddTrickScore feeding AE/bursts)
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

// ============================================================================
// UI toggles (main menu panel) — keep simple and clear
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

    return Mod::on_initialize();
}

void Tony::on_config_load(const utility::Config& cfg) {
    UAA_LoadFromConfig(cfg);
}
void Tony::on_config_save(utility::Config& cfg) {
    UAA_SaveToConfig(cfg);
}

#endif
