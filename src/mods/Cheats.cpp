#include "Cheats.hpp"
#include "../Config.hpp" // for CONFIG_FILENAME
#include "ChargeSubsBattery.hpp"
#include "ReprisalSwap.hpp"
#include "imgui.h"
#include "imgui_internal.h" // FindWindowByName, windows list
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

#if 1
bool Cheats::take_no_damage               = false;
bool Cheats::deal_no_damage               = false;
bool Cheats::one_hit_kill                 = false;
uintptr_t Cheats::damage_modifier_jmp_ret = NULL;
bool Cheats::spend_no_battery             = false;
bool Cheats::enemies_dont_attack          = false;
bool Cheats::invincible                   = false; // DodgeSettings handles this
bool Cheats::disable_free_fight_timers    = false; // added state

const char* Cheats::defaultDescription = "Cheats";
const char* Cheats::hoveredDescription = defaultDescription;

// Visual FX helpers/state
#define JACKPOT_LIME IM_COL32(40, 254, 32, 255)
#define FLASH_RED IM_COL32(255, 64, 64, 255)
#define JACKPOT_STAMP_ENABLED 1 // set to 0 to disable the "UNLOCKED!" stamp

static inline float _clamp01(float x) {
    return x < 0.f ? 0.f : (x > 1.f ? 1.f : x);
}
static inline float _ease_smooth(float t) {
    t = _clamp01(t);
    return t * t * (3.f - 2.f * t);
}
static inline float _ease_out_quad(float t) {
    t       = _clamp01(t);
    float u = 1.f - t;
    return 1.f - u * u;
}
static inline ImU32 _with_alpha(ImU32 c, int a) {
    ImVec4 v = ImGui::ColorConvertU32ToFloat4(c);
    v.w      = _clamp01(a / 255.f);
    return ImGui::ColorConvertFloat4ToU32(v);
}

// Cheats-panel (this window) red flash + shake
static bool g_cheats_panel_flash        = false;
static double g_cheats_panel_t0         = 0.0;
static const float kCheatsPanelFlashSec = 0.25f;

// Shake (applies only during red flash)
static bool g_shake_have_base   = false;
static ImVec2 g_shake_base_pos  = ImVec2(0, 0);
static const float kShakeAmpPx  = 5.0f;  // peak pixels
static const float kShakeFreqHz = 32.0f; // tight buzz

// Sequenced lime glow (Cheats -> Info -> Main), repeat twice
static bool g_seq_active        = false;
static double g_seq_t0          = 0.0;
static int g_seq_step           = 0;     // 0: Cheats, 1: Info, 2: Main
static int g_seq_loops          = 0;     // how many loops completed
static const float kSeqFlashDur = 0.14f; // on-time per panel
static const float kSeqGap      = 0.06f; // small blank gap
static const int kSeqRepeats    = 2;

// "UNLOCKED!" overlay text (optional)
static bool g_jackpot_active = false;
static double g_jackpot_t0   = 0.0;

// Rect helpers
static bool get_window_rect_by_name(const char* name, ImRect& out) {
    ImGuiWindow* w = ImGui::FindWindowByName(name);
    if (!w || !w->WasActive)
        return false;
    out.Min = w->Pos;
    out.Max = ImVec2(w->Pos.x + w->Size.x, w->Pos.y + w->Size.y);
    return true;
}
static bool get_guess_main_rect(ImRect& out) {
    ImGuiContext* g   = ImGui::GetCurrentContext();
    ImGuiWindow* best = nullptr;
    float best_area   = 0.f;
    for (int i = 0; i < g->Windows.Size; ++i) {
        ImGuiWindow* w = g->Windows[i];
        if (!w->WasActive || !w->Active)
            continue;
        if (w->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip))
            continue;
        if (strcmp(w->Name, "Settings window") == 0)
            continue;
        if (strcmp(w->Name, "Info window") == 0)
            continue;
        float area = w->Size.x * w->Size.y;
        if (area > best_area) {
            best_area = area;
            best      = w;
        }
    }
    if (!best)
        return false;
    out.Min = best->Pos;
    out.Max = ImVec2(best->Pos.x + best->Size.x, best->Pos.y + best->Size.y);
    return true;
}
static void draw_glow_rect(const ImRect& rc, float alpha01, ImU32 base_col) {
    if (alpha01 <= 0.f)
        return;
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    float round    = ImGui::GetStyle().WindowRounding;
    int a_fill     = (int)(170.f * _clamp01(alpha01));
    int a_edge     = (int)(220.f * _clamp01(alpha01));
    fg->AddRectFilled(rc.Min, rc.Max, _with_alpha(base_col, a_fill), round);
    fg->AddRect(rc.Min, rc.Max, _with_alpha(base_col, a_edge), round, 0, 2.f);
}

// sequence helpers
static void start_seq_glow() {
    g_seq_active = true;
    g_seq_t0     = ImGui::GetTime();
    g_seq_step   = 0;
    g_seq_loops  = 0;
#if JACKPOT_STAMP_ENABLED
    g_jackpot_active = true;
    g_jackpot_t0     = g_seq_t0;
#endif
}
static double total_seq_duration() {
    // each loop = 3*(dur+gap). Repeat kSeqRepeats.
    return (double)kSeqRepeats * 3.0 * (kSeqFlashDur + kSeqGap);
}
// ===============================================================

static char password_input[64] = "";
std::unordered_set<std::string> unlocked_cheats;
std::unordered_map<std::string, std::string> cheat_passwords = {
    {"invincible", "SUNDOWNER"},
    {"take_no_damage", "HESOYAM"},
    {"deal_no_damage", "JUSTAPRANK"},
    {"one_hit_kill", "FUCKHEAD"},
    {"spend_no_battery", "BANDANA"},
    {"enemies_dont_attack", "LEAVEMEALONE"},
    {"disable_free_fight_timers", "OUTOFTOUCH"},
    {"start_777", "JACKPOT"},
    {"start_bar", "WINDOWS"}, 
    {"start_bell", "BELLEND"},
    {"start_hopper", "GYARU"},
    {"start_cherry", "MRWHOOPEE"}
};

void save_unlocked_cheats() {
    utility::Config cfg;
    cfg.load(CONFIG_FILENAME);

    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = unlocked_cheats.find(pair.first) != unlocked_cheats.end();
        cfg.set<bool>("unlocked_" + pair.first, is_unlocked);
    }

    cfg.save(CONFIG_FILENAME);
}

// Try a password and trigger visual feedback (red for wrong, lime seq for success)
static bool try_password_and_feedback() {
    std::string upper_input = std::string(password_input);
    std::transform(upper_input.begin(), upper_input.end(), upper_input.begin(), ::toupper);
    if (upper_input.empty())
        return false;

    bool matched = false;
    for (const auto& pair : cheat_passwords) {
        if (pair.second == upper_input) {
            unlocked_cheats.insert(pair.first);
            save_unlocked_cheats();
            matched = true;
            break;
        }
    }

    if (matched) {
        memset(password_input, 0, sizeof(password_input));
        start_seq_glow(); // success sequence (+ optional UNLOCKED! stamp)
    } else {
        // wrong code -> red flash + shake on cheats/settings panel
        g_cheats_panel_flash = true;
        g_cheats_panel_t0    = ImGui::GetTime();
        g_shake_have_base    = false; // re-capture base position on next draw
    }
    return matched;
}

void check_password() {
    // Keep the old name for your existing call sites; now routes to the fancy version.
    (void)try_password_and_feedback();
}

bool is_cheat_unlocked(const std::string& cheat_name) {
#ifndef NDEBUG
    return true;
#endif
    return unlocked_cheats.find(cheat_name) != unlocked_cheats.end();
}

void Cheats::toggleTakeNoDamage(bool enable) {
    if (enable) {
        install_patch_offset(0x3D680D, patchTakeNoDamage, "\x90\x90\x90\x90", 4); // nop 4
    } else {
        install_patch_offset(0x3D680D, patchTakeNoDamage, "\xF3\x0F\x5C\xC1", 4); // subss xmm0,xmm1
    }
}

void Cheats::toggleDealNoDamage(bool enable) {
    if (enable) {
        install_patch_offset(0xA4165, patchDealNoDamage, "\x90\x90\x90\x90", 4); // nop 4
    } else {
        install_patch_offset(0xA4165, patchDealNoDamage, "\xF3\x0F\x5C\xC1", 4); // subss xmm0,xmm1
    }
}

// clang-format off
static constexpr float oneHitKillDamage = 9999.0f;
static constexpr float reprisalDamageModifier = 0.1f;
naked void detour_damage_modifier() { 
    __asm {
        cmp byte ptr [Cheats::one_hit_kill], 1
        je oneHitKill
        cmp byte ptr [Cheats::deal_no_damage], 1
        je noDamage
        cmp byte ptr [ChargeSubsBattery::mod_enabled], 1
        je chargesMoveIDCheck
        jmp reprisalMoveIDCheck

        chargesMoveIDCheck:
        cmp byte ptr [edi+0x1707], 1 // justAttack // limit damage edits to reprisals
        jne originalcode
        cmp dword ptr [edi+0x18C], ePcMtBtAtk01Rng // 246
        je checkChargeCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtk03Rng // 340
        je checkChargeCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtk04Rng // 386
        je checkChargeCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtk02RngCmbA // 290
        je checkChargeCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtk02RngCmbB // 291
        je checkChargeCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtk02RngCmbC // 292
        je checkChargeCheatTicked

        cmp byte ptr [ReprisalSwap::mod_enabled], 1
        je reprisalMoveIDCheck
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 1
        je reprisalMoveIDCheck
        jmp originalcode

        reprisalMoveIDCheck:
        cmp byte ptr [edi+0x1707], 1 // justAttack // limit damage edits to reprisals
        jne originalcode
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChgUp // 170
        je checkHighReprisalCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChg // 169
        je checkMidReprisalCheatTicked
        jmp originalcode

    checkHighReprisalCheatTicked:
        cmp byte ptr [ReprisalSwap::mod_enabled], 1
        je newReprisalDamage
        jmp originalcode

    checkChargeCheatTicked:
        cmp byte ptr [ChargeSubsBattery::mod_enabled], 1
        je newReprisalDamage
        jmp originalcode

    checkMidReprisalCheatTicked:
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 1
        je newReprisalDamage
        jmp originalcode

    oneHitKill:
        movss xmm0, [oneHitKillDamage]
        jmp originalcode
    newReprisalDamage:
        mulss xmm0, [reprisalDamageModifier]
    originalcode:
        movss [esp],xmm0
    retcode:
        jmp dword ptr [Cheats::damage_modifier_jmp_ret]
    noDamage:
        mov dword ptr [esp], 0
        jmp retcode
    }
}
// clang-format on

void Cheats::toggleSpendNoBattery(bool enable) {
    if (enable) {
        install_patch_offset(0x3C2AAF, patchSpendNoBattery, "\x90\x90\x90\x90\x90\x90\x90", 7); // nop 7
    } else {
        install_patch_offset(0x3C2AAF, patchSpendNoBattery, "\x66\x29\x81\x14\x05\x00\x00", 7); // sub [ecx+00000514],ax
    }
}

void Cheats::toggleEnemiesDontAttack(bool enable) {
    if (enable) {
        install_patch_offset(0x43536E, patchEnemiesDontAttack, "\x90\x90", 2); // nop 2
    } else {
        install_patch_offset(0x43536E, patchEnemiesDontAttack, "\x7A\x1C", 2); // jp nmh.HRZAKO::mFrameProc+5C
    }
}

void Cheats::toggleDisableFreeFightTimers(bool enable) {
    if (enable) {
        install_patch_offset(0x38D261, patchFreeFightTimers, "\xEB", 1); // jmp
    } else {
        patchFreeFightTimers.reset(); // restore original (likely JNE 0x75)
    }
}

void Cheats::render_description() const {
    ImGui::TextWrapped(Cheats::hoveredDescription);
}

void Cheats::on_draw_ui() {
    // Apply tiny buzz-shake to the settings window while flashing red
    if (g_cheats_panel_flash) {
        float tnorm = (float)((ImGui::GetTime() - g_cheats_panel_t0) / kCheatsPanelFlashSec);
        if (tnorm >= 1.f) {
            if (g_shake_have_base) {
                ImGui::SetWindowPos(g_shake_base_pos, ImGuiCond_Always);
                g_shake_have_base = false;
            }
        } else {
            if (!g_shake_have_base) {
                g_shake_base_pos  = ImGui::GetWindowPos();
                g_shake_have_base = true;
            }
            float amp = kShakeAmpPx * _ease_out_quad(1.f - tnorm);
            float tt  = (float)ImGui::GetTime() * (6.28318f * kShakeFreqHz); // 2*pi*f*t
            ImVec2 jitter(amp * sinf(tt), amp * cosf(tt * 0.9f));
            ImGui::SetWindowPos(ImVec2(g_shake_base_pos.x + jitter.x, g_shake_base_pos.y + jitter.y), ImGuiCond_Always);
        }
    }

    if (!ImGui::IsAnyItemHovered())
        Cheats::hoveredDescription = defaultDescription;

    // Password entry
    ImGui::Text("Enter cheat password:");
    float fontSize = ImGui::GetFontSize();
    ImGui::SetNextItemWidth(fontSize * 8.0f);
    if (ImGui::InputText("##password", password_input, sizeof(password_input),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsUppercase)) {
        check_password();
    }
    ImGui::SameLine();
    if (ImGui::Button("Submit")) {
        check_password();
    }
    ImGui::Separator();

    // Toggles
    if (is_cheat_unlocked("invincible")) {
        ImGui::Checkbox("Invincible", &invincible);
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = "Disable the player taking damage and animating when hit";
    }

    if (is_cheat_unlocked("take_no_damage")) {
        if (ImGui::Checkbox("Take No Damage", &take_no_damage)) {
            toggleTakeNoDamage(take_no_damage);
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = "Take no damage, but still receive hit effects";
    }

    if (is_cheat_unlocked("deal_no_damage")) {
        ImGui::Checkbox("Deal No Damage", &deal_no_damage);
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = "Deal no damage";
    }

    if (is_cheat_unlocked("one_hit_kill")) {
        ImGui::Checkbox("One Hit Kills", &one_hit_kill);
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = "One hit kill all enemies";
    }

    if (is_cheat_unlocked("spend_no_battery")) {
        if (ImGui::Checkbox("Infinite Battery", &spend_no_battery)) {
            toggleSpendNoBattery(spend_no_battery);
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("enemies_dont_attack")) {
        if (ImGui::Checkbox("Enemies Don't Attack", &enemies_dont_attack)) {
            toggleEnemiesDontAttack(enemies_dont_attack);
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("disable_free_fight_timers")) {
        if (ImGui::Checkbox("Disable Free Fight Timers", &disable_free_fight_timers)) {
            toggleDisableFreeFightTimers(disable_free_fight_timers);
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    float combo_width = ImGui::CalcItemWidth();

    if (is_cheat_unlocked("start_777")) {
        if (ImGui::Button("Activate Anarchy In The Galaxy", ImVec2(combo_width, NULL))) {
            nmh_sdk::Start777();
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("start_bar")) {
        if (ImGui::Button("Activate Cranberry Chocolate Sundae", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartBar(false, 0);
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("start_bell")) {
        if (ImGui::Button("Activate Blueberry Cheese Brownie", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartBell();
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("start_hopper")) {
        if (ImGui::Button("Activate Strawberry on The Shortcake", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartHopper();
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    if (is_cheat_unlocked("start_cherry")) {
        if (ImGui::Button("Activate Cherries", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartCherry();
        }
        if (ImGui::IsItemHovered())
            Cheats::hoveredDescription = defaultDescription;
    }

    // Draw flashes & overlays

    // Red flash overlay (stays constrained to this settings window)
    if (g_cheats_panel_flash) {
        float t = (float)((ImGui::GetTime() - g_cheats_panel_t0) / kCheatsPanelFlashSec);
        if (t >= 1.f) {
            g_cheats_panel_flash = false;
        } else {
            float a        = _ease_out_quad(1.f - t);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 wp      = ImGui::GetWindowPos();
            ImVec2 ws      = ImGui::GetWindowSize();
            int alpha      = (int)(200.f * a);
            dl->AddRectFilled(wp, ImVec2(wp.x + ws.x, wp.y + ws.y), _with_alpha(FLASH_RED, alpha), ImGui::GetStyle().WindowRounding);
            dl->AddRect(
                wp, ImVec2(wp.x + ws.x, wp.y + ws.y), _with_alpha(FLASH_RED, (int)(220.f * a)), ImGui::GetStyle().WindowRounding, 0, 2.f);
        }
    }

    // Sequenced lime glow: Cheats -> Info -> Main, repeats twice
    if (g_seq_active) {
        double now     = ImGui::GetTime();
        double elapsed = now - g_seq_t0;

        if (elapsed > (kSeqFlashDur + kSeqGap)) {
            g_seq_t0 = now;
            g_seq_step++;
            if (g_seq_step >= 3) {
                g_seq_step = 0;
                g_seq_loops++;
                if (g_seq_loops >= kSeqRepeats) {
                    g_seq_active = false;
                }
            }
        } else {
            if (elapsed <= kSeqFlashDur) {
                float a = _ease_smooth(1.f - (float)(elapsed / kSeqFlashDur));
                ImRect rc;
                bool ok = false;

                if (g_seq_step == 0) { // Cheats/settings window (this one)
                    ImVec2 wp = ImGui::GetWindowPos();
                    ImVec2 ws = ImGui::GetWindowSize();
                    rc        = ImRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y));
                    ok        = true;
                } else if (g_seq_step == 1) { // Info window
                    ok = get_window_rect_by_name("Info window", rc);
                } else { // Main window
                    ok = get_guess_main_rect(rc);
                }

                if (ok) {
                    // Fill glow (no neon trace)
                    draw_glow_rect(rc, a, JACKPOT_LIME);
                }
            }
        }
    }

#if JACKPOT_STAMP_ENABLED
    // UNLOCKED text overlay: quick stamp with tiny chromatic misregistration
    if (g_jackpot_active) {
        double t     = ImGui::GetTime() - g_jackpot_t0;
        double total = total_seq_duration();
        if (t >= total) {
            g_jackpot_active = false;
        } else {
            // Find union rect of the 3 panels for centering
            ImRect rcheat, rinfo, rmain, runion;
            {
                ImVec2 wp = ImGui::GetWindowPos();
                ImVec2 ws = ImGui::GetWindowSize();
                rcheat    = ImRect(wp, ImVec2(wp.x + ws.x, wp.y + ws.y));
            }
            bool okI = get_window_rect_by_name("Info window", rinfo);
            bool okM = get_guess_main_rect(rmain);

            runion = rcheat;
            if (okI) {
                runion.Min.x = ImMin(runion.Min.x, rinfo.Min.x);
                runion.Min.y = ImMin(runion.Min.y, rinfo.Min.y);
                runion.Max.x = ImMax(runion.Max.x, rinfo.Max.x);
                runion.Max.y = ImMax(runion.Max.y, rinfo.Max.y);
            }
            if (okM) {
                runion.Min.x = ImMin(runion.Min.x, rmain.Min.x);
                runion.Min.y = ImMin(runion.Min.y, rmain.Min.y);
                runion.Max.x = ImMax(runion.Max.x, rmain.Max.x);
                runion.Max.y = ImMax(runion.Max.y, rmain.Max.y);
            }

            ImVec2 center((runion.Min.x + runion.Max.x) * 0.5f, (runion.Min.y + runion.Max.y) * 0.5f);

            const char* txt = "UNLOCKED!";
            ImDrawList* fg  = ImGui::GetForegroundDrawList();
            ImFont* fnt     = ImGui::GetFont();
            float base      = ImGui::GetFontSize();
            float k         = 2.2f * (1.0f + 0.08f * sinf((float)ImGui::GetTime() * 18.0f));
            ImVec2 ts       = ImGui::CalcTextSize(txt);
            ImVec2 ts_scaled(ts.x * k, ts.y * k);
            ImVec2 pos(center.x - ts_scaled.x * 0.5f, center.y - ts_scaled.y * 0.5f);

            float fade  = 1.0f;
            double edge = 0.18; // seconds for edge fade
            if (t < edge)
                fade = _clamp01((float)(t / edge));
            if (t > total - edge)
                fade = _clamp01((float)((total - t) / edge));

            ImVec2 offR(-1.5f, -1.0f);
            ImVec2 offB(+1.5f, +1.0f);

            fg->AddText(
                fnt, base * k, ImVec2(pos.x + offR.x, pos.y + offR.y), _with_alpha(IM_COL32(255, 80, 80, 255), (int)(180 * fade)), txt);
            fg->AddText(
                fnt, base * k, ImVec2(pos.x + offB.x, pos.y + offB.y), _with_alpha(IM_COL32(80, 160, 255, 255), (int)(180 * fade)), txt);
            fg->AddText(fnt, base * k, pos, _with_alpha(JACKPOT_LIME, (int)(255 * fade)), txt);
        }
    }
#endif
    // -------------------------------------------------
}

std::optional<std::string> Cheats::on_initialize() {
    if (!install_hook_offset(0x3CB82F, damage_modifier_hook, &detour_damage_modifier, &Cheats::damage_modifier_jmp_ret, 5)) {
        spdlog::error("Failed to init DamageModifier mod\n");
        return "Failed to init DamageModifier mod";
    }

    return Mod::on_initialize();
}

void Cheats::on_config_load(const utility::Config& cfg) {
    take_no_damage = cfg.get<bool>("take_no_damage").value_or(false);
    if (take_no_damage)
        toggleTakeNoDamage(take_no_damage);
    deal_no_damage = cfg.get<bool>("deal_no_damage").value_or(false);
    if (deal_no_damage)
        toggleDealNoDamage(deal_no_damage);
    one_hit_kill     = cfg.get<bool>("one_hit_kill").value_or(false);
    spend_no_battery = cfg.get<bool>("spend_no_battery").value_or(false);
    if (spend_no_battery)
        toggleSpendNoBattery(spend_no_battery);
    enemies_dont_attack = cfg.get<bool>("enemies_dont_attack").value_or(false);
    if (enemies_dont_attack)
        toggleEnemiesDontAttack(enemies_dont_attack);
    invincible                = cfg.get<bool>("invincible").value_or(false);
    disable_free_fight_timers = cfg.get<bool>("disable_free_fight_timers").value_or(false);
    if (disable_free_fight_timers)
        toggleDisableFreeFightTimers(disable_free_fight_timers);

    unlocked_cheats.clear();
    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = cfg.get<bool>("unlocked_" + pair.first).value_or(false);
        if (is_unlocked) {
            unlocked_cheats.insert(pair.first);
        }
    }
}

void Cheats::on_config_save(utility::Config& cfg) {
    cfg.set<bool>("take_no_damage", take_no_damage);
    cfg.set<bool>("deal_no_damage", deal_no_damage);
    cfg.set<bool>("one_hit_kill", one_hit_kill);
    cfg.set<bool>("spend_no_battery", spend_no_battery);
    cfg.set<bool>("enemies_dont_attack", enemies_dont_attack);
    cfg.set<bool>("invincible", invincible);
    cfg.set<bool>("disable_free_fight_timers", disable_free_fight_timers);

    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = unlocked_cheats.find(pair.first) != unlocked_cheats.end();
        cfg.set<bool>("unlocked_" + pair.first, is_unlocked);
    }
}

// do something every frame
// void Cheats::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
// void Cheats::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
