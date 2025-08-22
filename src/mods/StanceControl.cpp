#include "StanceControl.hpp"
#include "KbmControls.hpp"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/KanaeTextureAtlas.cpp"
#include "utility/Compressed.hpp"
#include "imgui_internal.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include <filesystem>
#include "EnemySpawn.hpp" // for chara type strings

#if 1
bool StanceControl::mod_enabled = false;
bool StanceControl::invert_input = false;
bool StanceControl::invert_mid = false;
bool StanceControl::show_new_ui = false;
bool StanceControl::edit_old_ui = false;
uintptr_t StanceControl::gpPad = NULL;
uintptr_t StanceControl::jmp_ret1 = NULL;

float StanceControl::r2Mult = 127.5f;
float StanceControl::r2Sub = 1.0f;
float StanceControl::highBound = 0.0f;
float StanceControl::lowBound = 0.0f;
float StanceControl::invert = -1.0f;

float StanceControl::r2MultGuard = 255.0f;
float StanceControl::highBoundGuard = 0.0f;
float StanceControl::lowBoundGuard = 0.0f;
float StanceControl::invertGuard = 1.0f;

uintptr_t StanceControl::jmp_ret2 = NULL;
uintptr_t StanceControl::jmp2je = NULL;
bool StanceControl::wasL3PressedLastFrame = false;

uintptr_t StanceControl::jmp_ret3 = NULL;
uintptr_t StanceControl::jmp_jne3 = NULL;
uintptr_t StanceControl::clashing = NULL;

bool StanceControl::swapIdleStances = false;

bool StanceControl::mod_enabled_disable_combo_extend_speedup = false;

bool StanceControl::mod_enabled_faster_nu_lows = false;
uintptr_t StanceControl::jmp_ret4;

static uintptr_t mCheckNormalAttack = NULL;

bool StanceControl::mod_enabled_gear_system = false;
bool StanceControl::gear_system_holds = false;

bool StanceControl::mod_enabled_stance_guards = false;
uintptr_t StanceControl::jmp_ret5 = NULL;
uintptr_t StanceControl::jmp5je = NULL;

// directx stuff
static std::unique_ptr<Texture2DD3D11> g_kanae_texture_atlas{};
#pragma region TextureAtlasDefinitions
// https://mustache.github.io/
// https://github.com/odrick/free-tex-packer

struct Frame {
    ImVec2 pos,size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto BACK_HIGH() {
        return Frame {
            ImVec2 { 4.0f, 4.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 4.0f / 4096.0f, 4.0f / 4096.0f },
            ImVec2 { ( 4.0f + 1920.0f ) / 4096.0f, ( 4.0f + 1080.0f ) / 4096.0f }
        }; //BACK_HIGH
    };
    static constexpr auto BACK_LOW() {
        return Frame {
            ImVec2 { 1932.0f, 4.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1932.0f / 4096.0f, 4.0f / 4096.0f },
            ImVec2 { ( 1932.0f + 1920.0f ) / 4096.0f, ( 4.0f + 1080.0f ) / 4096.0f }
        }; //BACK_LOW
    };
    static constexpr auto BACK_MID() {
        return Frame {
            ImVec2 { 4.0f, 1092.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 4.0f / 4096.0f, 1092.0f / 4096.0f },
            ImVec2 { ( 4.0f + 1920.0f ) / 4096.0f, ( 1092.0f + 1080.0f ) / 4096.0f }
        }; //BACK_MID
    };
    static constexpr auto HIGH_GLOW() {
        return Frame {
            ImVec2 { 1932.0f, 1092.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1932.0f / 4096.0f, 1092.0f / 4096.0f },
            ImVec2 { ( 1932.0f + 1920.0f ) / 4096.0f, ( 1092.0f + 1080.0f ) / 4096.0f }
        }; //HIGH_GLOW
    };
    static constexpr auto LOW_GLOW() {
        return Frame {
            ImVec2 { 4.0f, 2180.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 4.0f / 4096.0f, 2180.0f / 4096.0f },
            ImVec2 { ( 4.0f + 1920.0f ) / 4096.0f, ( 2180.0f + 1080.0f ) / 4096.0f }
        }; //LOW_GLOW
    };
    static constexpr auto MID_GLOW() {
        return Frame {
            ImVec2 { 1932.0f, 2180.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1932.0f / 4096.0f, 2180.0f / 4096.0f },
            ImVec2 { ( 1932.0f + 1920.0f ) / 4096.0f, ( 2180.0f + 1080.0f ) / 4096.0f }
        }; //MID_GLOW
    };

    static constexpr auto meta_size() {
        return ImVec2{ 4096.0f, 4096.0f  };
    };
};

#pragma endregion


static std::unique_ptr<FunctionHook> g_kanae_himitsu;

void StanceControl::toggleSwapIdleStances(bool enable) {
    if (enable) {
        install_patch_offset(0x3D7D4A, patch_swap_idle_stance1, "\x75", 1); // jne nmh.exe+3D7D5B
        install_patch_offset(0x3D7D4F, patch_swap_idle_stance2, "\x75", 1); // jne nmh.exe+3D7D56
    }
    else {
        install_patch_offset(0x3D7D4A, patch_swap_idle_stance1, "\x74", 1); // je nmh.exe+3D7D5B
        install_patch_offset(0x3D7D4F, patch_swap_idle_stance2, "\x74", 1); // je nmh.exe+3D7D56
    }
}

void StanceControl::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3DE09F, m_patch1, "\xEB\x10", 2); // jmp nmh.exe+3DE0B1 // disable low stance set
        install_patch_offset(0x3DE067, m_patch2, "\xEB\x0A", 2); // jmp nmh.exe+3DE073 // disable high stance set
        install_patch_offset(0x3D7E48, m_patch3, "\x80\xBE\x50\x13\x00\x00\x01", 7); // cmp byte ptr [esi+00001350],01 // force mid stance
    }
    else {
         m_patch1.reset();
         m_patch2.reset();
         m_patch3.reset();
    }
} 

void StanceControl::toggle_display_edit(bool enable) {
    if (enable) {
        install_patch_offset(0x409B70, m_patch4, "\x83\xE8\x01", 3); // sub eax,01
    }
    else {
        install_patch_offset(0x409B70, m_patch4, "\x83\xE8\x02", 3); // sub eax,02
    }
}

void StanceControl::toggle_disable_combo_extend_speedup (bool enable) {
    if (enable) {
        install_patch_offset(0x3C72DA, patch_disable_combo_extend_speedup, "\xEB\x2A", 2); // 
    }
    else {
        install_patch_offset(0x3C72DA, patch_disable_combo_extend_speedup, "\x75\x2A", 2); // 
    }
}

void StanceControl::disable_cam_reset(bool enable) {
    if (enable) {
        install_patch_offset(0x3D711D, patch_disable_cam_reset, "\xE9\xB8\x00\x00\x00\x00", 6); // jmp nmh.exe+3D71DA
    }
    else {
        install_patch_offset(0x3D711D, patch_disable_cam_reset, "\x0F\x84\xB7\x00\x00\x00", 6); // je nmh.exe+3D71DA
    }
}

static constexpr float midStanceBlend = 0.0f;
static constexpr float lowGuardBlend = 0.0f;
static constexpr float midGuardBlend = 0.5f;
static constexpr float highGuardBlend = 1.0f;
static float blendTick = 0.1f;
static float blendTickNotLockedOn = 0.1f;
static float blendTickLockedOn = 0.4f;
static float newTilt = 0.0f;
// static bool verySmooth = true;

static float gearSysXmm0backup = 0.0f;
static float gearSysXmm1backup = 0.0f;
static float gearSysXmm2backup = 0.0f;
static float gearSysXmm3backup = 0.0f;
static float gearSysXmm4backup = 0.0f;
static float gearSysXmm5backup = 0.0f;

// 2 is mid, 0 is high, 1 is low
float StanceControl::SetSmoothStance(mHRPc* player) {
    auto currentPose = player->mPcStatus.pose;
    auto moveID = player->mCharaStatus.motionNo;
    
    float targets[] = {1.0f, -1.0f, 0.0f}; // high, low, mid
    float guardTargets[] = {1.0f, 0.0f, 0.5f}; // high, low, mid
    
    if (currentPose >= 0 && currentPose <= 2) {
        static float target = 0.0f;
        if (moveID == ePcMtGrdDfltLp) {
            target = guardTargets[currentPose];
            blendTick = blendTickLockedOn;
        }
        else {
            target = targets[currentPose];
            blendTick = blendTickNotLockedOn;
        }
        //if (verySmooth) {
            newTilt = glm::mix(newTilt, target, blendTick);
        //}
        /*else {
            if (newTilt < target) {
                newTilt += blendTick;
            }
            else if (newTilt > target) {
                newTilt -= blendTick;
            }
        }*/
    }

    return newTilt;
}

// clang-format off
naked void detour1() { // originalcode writes stance blend to 0, we write actual values and set stance using it
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled_gear_system], 1 // one or the other
        je GearSystemCode
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
        push eax
        mov eax, [StanceControl::gpPad]
        movss xmm0, [eax+0x64]
        pop eax
        cmp dword ptr [esi+0x18C], ePcMtGrdDfltLp // 48, guarding
        je guardMath
        divss xmm0, [StanceControl::r2Mult] // Gamepad reads 0-255
        subss xmm0, [StanceControl::r2Sub] // we need it to read -1 to 1
    contAfterMath:
        cmp byte ptr [StanceControl::invert_input], 0
        je skipInvert
        mulss xmm0, [StanceControl::invert]
    skipInvert:
        comiss xmm0, [StanceControl::highBound]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBound]
        jb writeLow
    writeMid:
        cmp byte ptr [StanceControl::invert_mid], 1
        je writeLowInstead
        mov dword ptr [esi+0x1350], 1 // 2
        jmp originalcode

    writeHigh:
        mov dword ptr [esi+0x1350], 0
        jmp originalcode

    writeLow:
        cmp byte ptr [StanceControl::invert_mid], 1
        je writeMidInstead
        mov dword ptr [esi+0x1350], 2
        jmp originalcode

    writeMidInstead:
        mov dword ptr [esi+0x1350], 1
        jmp originalcode
    writeLowInstead:
        mov dword ptr [esi+0x1350], 2
        jmp originalcode

    originalcode:
        movss [esi+0x0000139C], xmm0
        jmp dword ptr [StanceControl::jmp_ret1]

    guardMath:
        divss xmm0, [StanceControl::r2MultGuard] // Gamepad reads 0-255
        cmp byte ptr [StanceControl::invert_input], 0
        je skipInvert2
        movss [gearSysXmm3backup], xmm3
        movss xmm3, [StanceControl::invertGuard] // backup xmm3 just in case
        subss xmm3, xmm0
        movss xmm0, xmm3
        movss xmm3, [gearSysXmm3backup]
    skipInvert2:
        comiss xmm0, [StanceControl::highBoundGuard]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBoundGuard]
        jb writeLow
        jmp writeMid

    GearSystemCode: // set stance blend depending on stance. Actual pcPose is set by void StanceControl::GearControls(mHRPc* player)
        movss [gearSysXmm0backup], xmm0
        movss [gearSysXmm1backup], xmm1
        movss [gearSysXmm2backup], xmm2
        movss [gearSysXmm3backup], xmm3
        movss [gearSysXmm4backup], xmm4
        movss [gearSysXmm5backup], xmm5
        pushad
        push esi // player arg
        call dword ptr [StanceControl::GearControls] // set actual pos
        popad
        pushad
        push esi // player arg
        call dword ptr [StanceControl::SetSmoothStance]
        sub esp, 4
        fstp dword ptr [esp]
        movss xmm0, [esp]
        add esp, 4
        popad
        //movss xmm0, [gearSysXmm0backup] 
        movss xmm1, [gearSysXmm1backup] 
        movss xmm2, [gearSysXmm2backup] 
        movss xmm3, [gearSysXmm3backup] 
        movss xmm4, [gearSysXmm4backup] 
        movss xmm5, [gearSysXmm5backup] 
        jmp originalcode

        cmp dword ptr [esi+0x18C], ePcMtGrdDfltLp // 48, guarding
        je GearSystemGuarding
    // GearSystemNotGuarding:
        cmp dword ptr [esi+0x1350], 2 // mid stance
        jne originalcode
        movss xmm0, [midStanceBlend]
        jmp originalcode

    GearSystemGuarding:
        cmp byte ptr [esi+0x1350], ePcPoseUpper // 0
        je GearHigh
        cmp byte ptr [esi+0x1350], ePcPoseMiddle // 1
        je GearMid
    // GearLow:
        movss xmm0,[midGuardBlend]
        jmp originalcode
    GearHigh:
        movss xmm0,[highGuardBlend]
        jmp originalcode
    GearMid:
        movss xmm0, [lowGuardBlend]
        jmp originalcode
    }
}

naked void detour2() { // remap lock on cycle
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 1
        je cheatcode
        cmp byte ptr [StanceControl::mod_enabled_gear_system], 1
        je cheatcode
        jmp originalcode

    cheatcode:
        push eax
        push ecx
        mov eax, [StanceControl::gpPad]
        mov al, [eax+0x6] // R3 && L3
        mov cl, [StanceControl::wasL3PressedLastFrame]
        and al, 2
        and cl, 2
        test al, al
        je nopress
        test cl, cl
        jne nopress
        mov [StanceControl::wasL3PressedLastFrame], al
        pop ecx
        pop eax
        jmp retcode

    nopress:
        mov [StanceControl::wasL3PressedLastFrame], al
        pop ecx
        pop eax
    jecode:
        jmp dword ptr [StanceControl::jmp2je]

    originalcode:
        push eax
        mov eax, [StanceControl::gpPad]
        cmp byte ptr [eax+0x1C16], 00
        pop eax
        je jecode
    retcode:
        jmp dword ptr [StanceControl::jmp_ret2]
    }
}

naked void detour3() { // enable guard stance blend unless clashing
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 1
        je cheat
        cmp byte ptr [StanceControl::mod_enabled_gear_system], 1
        je cheat
        jmp originalcode

    cheat:
        push eax
        push ecx
        push edx
        push -1
        call dword ptr [StanceControl::clashing]
        cmp al, 1
        pop edx
        pop ecx
        pop eax
        jne jnecode
    originalcode:
        cmp dword ptr [esi+0x00002990],03
        jmp dword ptr [StanceControl::jmp_ret3]
    jnecode:
        jmp dword ptr [StanceControl::jmp_jne3]
    }
}

naked void detour4() { // faster nu lows if combo extend
    __asm {
        //
            cmp byte ptr [StanceControl::mod_enabled], 1
            je checkCheat
        // check2:
            cmp byte ptr [StanceControl::mod_enabled_gear_system], 0
            je originalcode
        checkCheat:
            cmp byte ptr [StanceControl::mod_enabled_faster_nu_lows], 0
            je originalcode

            call dword ptr mCheckNormalAttack
            cmp al,1
            je retcode
            push eax
            mov eax,[ecx+0x0000018C] // motionID

        //BB
            cmp eax,237
            je succeedcode
            cmp eax,238
            je succeedcode
            cmp eax,239
            je succeedcode
            cmp eax,240
            je succeedcode
            cmp eax,241
            je succeedcode
            cmp eax,242
            je succeedcode
            cmp eax,243
            je succeedcode
        //Mk1
            cmp eax,334
            je succeedcode
            cmp eax,335
            je succeedcode
            cmp eax,336
            je succeedcode
            cmp eax,337
            je succeedcode
        //Mk2
            cmp eax,379
            je succeedcode
            cmp eax,380
            je succeedcode
            cmp eax,381
            je succeedcode
            cmp eax,382
            je succeedcode

            pop eax
            xor al,al
            jmp retcode

        succeedcode:
            pop eax
            mov al,1
            jmp retcode

        originalcode:
            call dword ptr mCheckNormalAttack
            cmp al,1
            jmp retcode
        retcode:
            jmp dword ptr [StanceControl::jmp_ret4]
    }
}

static uintptr_t last_enemy_attacked_you = NULL;
naked void detour5() { // stance guards
    __asm {
        //
            cmp byte ptr [StanceControl::mod_enabled_stance_guards], 0
            je originalcode

            push eax
            mov eax, [esp+0x4+0x6C] // probably not reliable
            mov [last_enemy_attacked_you], eax
            pop eax

            cmp edi, 57
            je HighAttack
            cmp edi, 58
            je HighAttack
            cmp edi, 61
            je HighAttack
            cmp edi, 68
            je HighAttack
            cmp edi, 79
            je HighAttack
            cmp edi, 81
            je HighAttack
            cmp edi, 82
            je HighAttack
            cmp edi, 84
            je HighAttack
            cmp edi, 89
            je HighAttack

            cmp edi, 48
            je MidAttack
            cmp edi, 50
            je MidAttack
            cmp edi, 65
            je MidAttack
            cmp edi, 66
            je MidAttack
            cmp edi, 77
            je MidAttack
            cmp edi, 86
            je MidAttack
            cmp edi, 99 // bullet
            je MidAttack
            cmp edi, 100 // bullet
            je MidAttack

            cmp edi, 71
            je LowAttack
            cmp edi, 72
            je LowAttack
            cmp edi, 73
            je LowAttack
            cmp edi, 92
            je LowAttack
            cmp edi, 93
            je LowAttack
            cmp edi, 94
            je LowAttack
            cmp edi, 95
            je LowAttack

            jmp originalcode // don't care about stance for other anims

            HighAttack:
            cmp dword ptr [esi+0x1350], 0
            je originalcode
            jmp jecode

            MidAttack:
            cmp dword ptr [esi+0x1350], 2
            je originalcode
            jmp jecode

            LowAttack:
            cmp dword ptr [esi+0x1350], 1
            je originalcode
            jmp jecode


        originalcode:
            mov eax, [esi+0x0000018C]
        retcode:
            jmp dword ptr [StanceControl::jmp_ret5]
        jecode:
            jmp dword ptr [StanceControl::jmp5je]
    }
}

static bool g_kanae_drawcall { false };

static uintptr_t g_kanae_himitsu_skip       { NULL };
static uintptr_t g_kanae_himitsu_return     { NULL };
static uintptr_t g_kanae_ghm_draw_text_addr { NULL };
naked void kanae_himitsu_detour() {

    __asm {
        cmp byte ptr [StanceControl::mod_enabled_gear_system], 1
        je new_code
        cmp byte ptr [StanceControl::mod_enabled], 0
        je original_code

    new_code:
        push eax
        mov al, 1
        mov byte ptr [g_kanae_drawcall], al
        pop eax
        add esp, 8
        jmp dword ptr [g_kanae_himitsu_skip]

    original_code:
        call dword ptr [g_kanae_ghm_draw_text_addr]
        jmp dword ptr [g_kanae_himitsu_return]
    }
}

 // clang-format on

const char* StanceControl::defaultDescription = "Manually adjust the current stance using R2/RT like you can with motion controls. "
                                                "Our version of this also restores the original and unused Low stance for all beam katanas except the MK3.";
const char* StanceControl::hoveredDescription = defaultDescription;
//

void StanceControl::render_description() const {
    ImGui::TextWrapped(StanceControl::hoveredDescription);
}

void StanceControl::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) StanceControl::hoveredDescription = defaultDescription;

    if (ImGui::Checkbox("Gradual", &mod_enabled)) {
        toggle(mod_enabled);
        if (mod_enabled_gear_system) {
            mod_enabled_gear_system = false;
            disable_cam_reset(mod_enabled_gear_system); // re-enable cam reset
        }
    }
    if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Remaps lock on cycle to R3. This is needed to avoid switching targets with every press of R2 when using this feature.";
    if (mod_enabled) {
        ImGui::Indent();

        ImGui::Text("High Bound");
        if (ImGui::SliderFloat("## highBound sliderfloat", &StanceControl::highBound, 0.0f, 1.0f, "%.2f")) {
            highBoundGuard = (highBound + 1.0f) / 2.0f;
        }
        if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "How far should r2 be pushed to enter high stance\n0.9 default";

        ImGui::Text("Low Bound");
        if (ImGui::SliderFloat("## lowBound sliderfloat", &StanceControl::lowBound, -1.0f, 0.0f, "%.2f")) {
            lowBoundGuard = (lowBound + 1.0f) / 2.0f;
        }
        if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "How little should r2 be pushed to enter low stance\n-0.9 default";

        // ImGui::Checkbox("Invert", &StanceControl::invert_input);
        // if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Swap Low and High";
        // 
        // ImGui::Checkbox("Invert Mid", &StanceControl::invert_mid);
        // if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Swap Mid and Low. The unused combos assigned to Mid stance are actually the original Low attacks. "
        //     "For this feature to make more sense, you can tick this to reorganize the stance order.";

        // ImGui::Checkbox("Show Custom Stance UI", &StanceControl::show_new_ui);

        ImGui::Checkbox("Combo Speed Upgrade On Low Attacks", &mod_enabled_faster_nu_lows);
        if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "The unused Low stance attacks don't utilize the speed buff that comes with the combo extension upgrades. Tick this to apply it.";

        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Toggle Mode", &mod_enabled_gear_system)) {
        toggle(mod_enabled_gear_system); // disable stance switching when pressing face buttons
        disable_cam_reset(mod_enabled_gear_system); // disable cam reset, we need the button
        mod_enabled = false;
    }
    if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Pressing R1 moves up a stance, pressing R2 moves down a stance";

    if (mod_enabled_gear_system) {
        ImGui::Indent();
        ImGui::Checkbox("Hold Mode", &gear_system_holds);
        if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Instead of toggling stances, access them with hold inputs. Hold R1 for High, R2 for low";

        ImGui::Checkbox("Combo Extend Speedup On Low Attacks", &mod_enabled_faster_nu_lows);
        if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Apply the default combo extension speed upgrade to modded low stance attacks. This still requires you to purchase the upgrade.";

        // ImGui::Checkbox("Very Smooth", &verySmooth);
        // if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "DEV ONLY. How smooth do you want going between stances to look? Untick for linear. Imo it's cool but too dmc5 for this game";

        ImGui::Unindent();
    }

    ImGui::Spacing();

    // if (ImGui::Checkbox("Swap Vanilla Mid and Low UI", &StanceControl::edit_old_ui)) {
    //     toggle_display_edit(edit_old_ui);
    // }
    // if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Makes the vanilla stance display consider the default low stance as mid stance.";

    if (ImGui::Checkbox("Swap Idle Stances", &swapIdleStances)) {
        toggleSwapIdleStances(swapIdleStances);
    }
    if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "The High/Low stances are mistakenly inverted by default, forcing Travis to take on the incorrect stance. This setting "
                "corrects that issue. This is purely cosmetic.";

    if (ImGui::Checkbox("Disable Combo Speed Upgrade", &mod_enabled_disable_combo_extend_speedup)) {
        toggle_disable_combo_extend_speedup(mod_enabled_disable_combo_extend_speedup);
    }
    if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "This takes priority over \"Combo Extend Speedup On Low Attacks\"";

    ImGui::SliderFloat("blendTickNotLockedOn", &blendTickNotLockedOn, 0.01f, 1.0f, "%.1f");
    ImGui::SliderFloat("blendTickLockedOn", &blendTickLockedOn, 0.01f, 1.0f, "%.1f");
    ImGui::Checkbox("Manual Guarding", &mod_enabled_stance_guards);
    if (ImGui::IsItemHovered()) StanceControl::hoveredDescription = "Completely disables auto guarding in favor of manual, stance-dependent guarding.";
}

void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

static constexpr size_t    templeos_hymn_risen_range  = 677;
static constexpr uint8_t   templeos_hymn_risen_values[] = {
    217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 187, 187, 187, 187, 187, 187, 187, 307, 307, 307, 307, 187, 307, 307, 268, 268, 268, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 187, 187, 187, 187, 187, 187, 187, 307, 307, 307, 307, 307, 307, 307, 268, 268, 268, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 250, 250, 250, 250, 250, 250, 250, 217, 217, 217, 217, 217, 217, 217, 307, 307, 307, 307, 307, 307, 307, 174, 174, 174, 174, 174, 174, 174, 217, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 149, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 268, 268, 268, 268, 268, 268, 268, 250, 250, 250, 250, 250, 250, 250, 217, 217, 217, 217, 217, 217, 217, 307, 307, 307, 307, 307, 307, 307, 174, 174, 174, 174, 174, 174, 174, 217, 217, 217, 217, 217, 217, 217, 187, 187, 187, 187, 187, 187, 187, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 268, 217, };
 
void StanceControl::GearControls(mHRPc* player) { 
    if (!player || !gpPad) return;
    
    static uintptr_t base = g_framework->get_module().as<uintptr_t>();
    int8_t* r1Press = (int8_t*)(base+0x84B930);  
    float* r2Press = (float*)(gpPad + 0x64);
    int8_t* stance = (int8_t*)&(player->mPcStatus.pose);
    static constexpr float r2PressThreshold = 20.0f; // 20/255
    
    if (!r1Press || !stance) return;
    
    if (!gear_system_holds) {
        static bool r1WasPressed = false;
        static bool r2WasPressed = false;
        static int previousStance = *stance;
        
        bool r1JustPressed = (*r1Press && !r1WasPressed);
        bool r2JustPressed = (*r2Press > r2PressThreshold && !r2WasPressed);
        
        if (r1JustPressed) {
            if (previousStance == 2) {
                previousStance = 0;
            } else if (previousStance == 1) {
                previousStance = 2;
            }
            *stance = previousStance;
        } else if (r2JustPressed) {
            if (previousStance == 0) {
                previousStance = 2;
            } else if (previousStance == 2) {
                previousStance = 1;
            }
            *stance = previousStance;
        } else {
            *stance = previousStance;
        }
        
        r1WasPressed = *r1Press;
        r2WasPressed = (*r2Press > r2PressThreshold);
    } else {
        bool r1Held = *r1Press;
        bool r2Held = (*r2Press > r2PressThreshold);
        
        if (r1Held) {
            *stance = 0;
        } else if (r2Held) {
            *stance = 1;
        } else {
            *stance = 2; // default
        }
    }
}

// do something every frame
static uint32_t g_frame_counter = 0;
#ifdef HOT_RELOAD
struct DebugTexture {
    const char* filename;
    std::unique_ptr<Texture2DD3D11> texture;
    std::filesystem::file_time_type filetime;
    std::filesystem::path filepath;
};

std::array g_textures_debug = {
    // backdrop
    DebugTexture { "BACK_HIGH.png", nullptr },
    DebugTexture { "BACK_LOW.png",  nullptr },
    DebugTexture { "BACK_MID.png",  nullptr },
    // glows
    DebugTexture { "HIGH_GLOW.PNG", nullptr },
    DebugTexture { "LOW_GLOW.PNG",  nullptr },
    DebugTexture { "MID_GLOW.PNG",  nullptr },

};

#endif
void StanceControl::on_frame() {
    if (mod_enabled_stance_guards) {
        ImGui::Begin("Last Enemy Attacking Your Guard", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        if (last_enemy_attacked_you) {
            mHRChara* enemy = (mHRChara*)last_enemy_attacked_you;
            ImGui::Text("Character Type: %s", charaTypeStrings[enemy->mStatus.charaType]);
        }
        ImGui::End();
    }
    if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
        // if (mod_enabled_gear_system) GearControls(mHRPc);
        auto mode = mHRPc->mInputMode;
        uintptr_t baseAddress = g_framework->get_module().as<uintptr_t>();
        HrCamera* hrCamera = reinterpret_cast<HrCamera*>(baseAddress + 0x82A4A0);
        mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
        HrScreenStatus* hrScreenStatus = mHRBattle->mBtEffect.pScreenStatus;
        if (!mHRBattle || !hrCamera || !hrScreenStatus) { return; }
        bool showStanceUIThisFrame = false;
        showStanceUIThisFrame = (hrScreenStatus->flag & (1 << 2)) != 0; // drawbattery bit
        int camMode = hrCamera->MAIN.Mode;
        if (mode == ePcInputMenu) { return; }
        if (/*mHRPc->mOperate && */showStanceUIThisFrame && ((StanceControl::mod_enabled && show_new_ui) || mod_enabled_gear_system)) {

            static constexpr TextureAtlas atlas{};
            struct KanaeDisp {
                std::array<Frame, 3> f;
            };

            static constexpr KanaeDisp kanae {
                { atlas.BACK_HIGH(), atlas.BACK_LOW(), atlas.BACK_MID() } 
            };
            static constexpr KanaeDisp kanae_glow {
                { atlas.HIGH_GLOW(), atlas.LOW_GLOW(), atlas.MID_GLOW() }
            };

            int pose = mHRPc->mPcStatus.pose;

            const auto& io = ImGui::GetIO();
            const auto& tex = g_kanae_texture_atlas->GetTexture();
            ImDrawList* dl = ImGui::GetForegroundDrawList();

            DolphinGame* scinfo = nmh_sdk::get_DolphinApp()->game;
            assert(scinfo);

            ImVec2 points[] = {
                ImVec2(scinfo->screenRect.left,  scinfo->screenRect.top),
                ImVec2(scinfo->screenRect.right, scinfo->screenRect.bottom),

            };
            ImVec2 kanae_uvs[] = {
                ImVec2(kanae.f[pose].uv0),
                ImVec2(kanae.f[pose].uv1),
            };
            ImVec2 glow_uvs[] = {
                ImVec2(kanae_glow.f[pose].uv0),
                ImVec2(kanae_glow.f[pose].uv1),
            };

#ifdef HOT_RELOAD 
            DebugTexture* textures[] = {
                &g_textures_debug[0],
                &g_textures_debug[1],
                &g_textures_debug[2],
            };
            dl->AddImage((ImTextureID)textures[pose]->texture->GetTexture(), points[0], points[1]);
#else
            dl->AddImage((ImTextureID)tex, points[0], points[1], kanae_uvs[0], kanae_uvs[1]);
#endif
            //dl->AddImageQuad(tex, points[0], points[1], points[2], points[3], kanae_uvs[0], kanae_uvs[1], kanae_uvs[2], kanae_uvs[3]);
            
#if 1
            float meme = (float)templeos_hymn_risen_values[g_frame_counter % templeos_hymn_risen_range];
            float glow = glm::clamp(meme, 179.0f, 255.0f);
#else // game logic i think

            HrScreenStatus* v4 = nmh_sdk::get_mHRBattle()->mBtEffect.pScreenStatus;
            float glow = (((float)v4->m_GearRandCounter[1] / (float)v4->m_GearRandCounter[0]) * 255.0);
#endif
            ImU32 oppacity = IM_COL32(255, 255, 255, (char)(glow));
#ifdef HOT_RELOAD
            DebugTexture* glow_textures[] = {
                &g_textures_debug[3],
                &g_textures_debug[4],
                &g_textures_debug[5],
            };
                dl->AddImage((ImTextureID)glow_textures[pose]->texture->GetTexture(), points[0], points[1], ImVec2(0.0f, 0.0f), ImVec2(1.0f,1.0f), oppacity);
#else
                dl->AddImage((ImTextureID)tex, points[0], points[1], glow_uvs[0], glow_uvs[1], oppacity);
#endif // !HOT_RELOAD

            //dl->AddImageQuad(tex, points[0], points[1], points[2], points[3], glow_uvs[0], glow_uvs[1], glow_uvs[2], glow_uvs[3], oppacity);
            //dl->AddRectFilled(p0, p1, -1, 2.0f);

#ifdef HOT_RELOAD
            // NOTE(): load balancing :kappa:
            if ((g_frame_counter % 60) == 0) {
                for (auto& tex : g_textures_debug) {
                    auto cwt = std::filesystem::last_write_time(tex.filepath);
                    if (cwt > tex.filetime) {
                        tex.texture.reset();
                        tex.texture = std::make_unique<Texture2DD3D11>(tex.filename, g_framework->d3d11()->get_device());
                        tex.filetime = cwt;
                    }
                }
                
            }

#endif // !HOT_RELOAD


#if 0

            ImVec2 windowPos = g_kanae_ichi;//ImVec2((ImGui::GetIO().DisplaySize.x * 0.924f), (ImGui::GetIO().DisplaySize.y * 0.2f));
            ImVec2 windowSize = (ImVec2((ImGui::GetIO().DisplaySize.x * 0.05f), (ImGui::GetIO().DisplaySize.y * 0.1f)));
            ImGui::SetNextWindowPos(windowPos);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Begin("StanceControl", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            if (ImGui::BeginTable("StanceControlTable", 1, NULL, ImVec2(windowSize.x, windowSize.y / 3.0f))) {
                int pose = mHRPc->mPcStatus.pose;
                ImU32 redBgColor = IM_COL32(255, 0, 0, 255);
                ImU32 defaultBgColor = IM_COL32(0, 0, 0, 255);
                ImGui::TableNextColumn();
                if (pose == 0) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("HIGH");

                ImGui::TableNextColumn();
                if (pose == 1 && !invert_mid || (pose == 2 && invert_mid)) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("MID");

                ImGui::TableNextColumn();
                if (pose == 2 && !invert_mid || pose == 1 && invert_mid) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("LOW");
            }
            ImGui::EndTable();
            ImGui::PopStyleColor();
            ImGui::End();
#endif
            g_frame_counter += 1;
        }
        
        g_kanae_drawcall = false;
    }
}

static bool load_kanae_texture() {
#ifdef HOT_RELOAD
    for (auto& tex : g_textures_debug) {
        tex.texture = std::make_unique<Texture2DD3D11>(tex.filename, g_framework->d3d11()->get_device());
        tex.filepath = std::filesystem::path(std::filesystem::current_path().string() + fmt::format("\\{}", tex.filename));
        try {
            tex.filetime = std::filesystem::last_write_time(tex.filepath);
        }
        catch (std::exception& e) {
            OutputDebugStringA(e.what());
        }
    }
#endif // !HOT_RELOAD

    auto [data, size] = utility::decompress_file_from_memory_with_size(kanae_ta_compressed_data, kanae_ta_compressed_size);
    if (!data) {
        return false;
    }

    g_kanae_texture_atlas = std::make_unique<Texture2DD3D11>((const unsigned char*)data, size, g_framework->d3d11()->get_device());

    assert(g_kanae_texture_atlas->IsValid());
    if (!g_kanae_texture_atlas->IsValid()) {
        return false;
    }

    return true;
}

void StanceControl::on_d3d_reset() {
#ifdef HOT_RELOAD
    for (auto& tex : g_textures_debug) {
        tex.texture.reset();
    }
#endif // !HOT_RELOAD

    g_kanae_texture_atlas.reset();
    load_kanae_texture();
}

std::optional<std::string> StanceControl::on_initialize() {

    gpPad                      = g_framework->get_module().as<uintptr_t>() + 0x849D10;
    clashing                   = g_framework->get_module().as<uintptr_t>() + 0x3DFFC0;
    g_kanae_himitsu_skip       = g_framework->get_module().as<uintptr_t>() + 0x409D1D;
    g_kanae_ghm_draw_text_addr = g_framework->get_module().as<uintptr_t>() + 0x5E8BC0;

    if (!install_hook_offset(0x3D7D6B, m_hook1, &detour1, &StanceControl::jmp_ret1, 8)) {
        spdlog::error("Failed to init StanceControl mod 1\n");
        return "Failed to init StanceControl mod 1";
    }

    jmp2je = g_framework->get_module().as<uintptr_t>() + 0x3C46F8;
    if (!install_hook_offset(0x3C4645, m_hook2, &detour2, &StanceControl::jmp_ret2, 13)) {
        spdlog::error("Failed to init StanceControl mod 2\n");
        return "Failed to init StanceControl mod 2";
    }

    jmp_jne3 = g_framework->get_module().as<uintptr_t>() + 0x3D7EC8;
    if (!install_hook_offset(0x3D7EBF, m_hook3, &detour3, &StanceControl::jmp_ret3, 7)) {
        spdlog::error("Failed to init StanceControl mod 3\n");
        return "Failed to init StanceControl mod 3";
    }

    mCheckNormalAttack = g_framework->get_module().as<uintptr_t>() + 0x3D3D60;
    if (!install_hook_offset(0x3C72DD, m_hook4, &detour4, &StanceControl::jmp_ret4, 5)) {
        spdlog::error("Failed to init StanceControl mod 4\n");
        return "Failed to init StanceControl mod 4";
    }

    jmp5je = g_framework->get_module().as<uintptr_t>() + 0x3D5C1E;
    if (!install_hook_offset(0x3D5C29, m_hook5, &detour5, &StanceControl::jmp_ret5, 6)) {
        spdlog::error("Failed to init StanceControl mod 5\n");
        return "Failed to init StanceControl mod 5";
    }

    if (!install_hook_offset(0x409A8D, g_kanae_himitsu, &kanae_himitsu_detour, &g_kanae_himitsu_return, 5)) {
        spdlog::error("Failed to init StanceControl kanae detour\n");
        return "Failed to init StanceControl kanae detour\n";
    }

    if (!load_kanae_texture()) {
        spdlog::error("Failed to load kanae texture\n");
        return "Failed to load kanae texture\n";
    }

    return Mod::on_initialize();
}

// during load
void StanceControl::on_config_load(const utility::Config& cfg) {
    mod_enabled = cfg.get<bool>("stance_control").value_or(false);
    if (mod_enabled) toggle(mod_enabled);
    invert_input = cfg.get<bool>("stance_control_invert").value_or(false);
    show_new_ui = cfg.get<bool>("stance_control_ui").value_or(true);
    edit_old_ui = cfg.get<bool>("stance_control_edit_old_ui").value_or(false);
    if (edit_old_ui) toggle_display_edit(edit_old_ui);
    invert_mid = cfg.get<bool>("stance_control_invert_mid").value_or(true);
    highBound = cfg.get<float>("stance_control_high_bound").value_or(0.9f);
    lowBound = cfg.get<float>("stance_control_low_bound").value_or(-0.9f);
    highBoundGuard = (highBound + 1.0f) / 2.0f;
    lowBoundGuard = (lowBound + 1.0f) / 2.0f;

    swapIdleStances = cfg.get<bool>("swap_idle_stances").value_or(false);
    if (swapIdleStances) toggleSwapIdleStances(swapIdleStances);

    mod_enabled_disable_combo_extend_speedup = cfg.get<bool>("disable_combo_extend_speedup").value_or(false);
    if (mod_enabled_disable_combo_extend_speedup) toggle_disable_combo_extend_speedup(mod_enabled_disable_combo_extend_speedup);

    mod_enabled_faster_nu_lows = cfg.get<bool>("faster_nu_lows").value_or(false);

    mod_enabled_gear_system = cfg.get<bool>("gear_system").value_or(false);
    gear_system_holds = cfg.get<bool>("gear_system_holds").value_or(false);
    if (mod_enabled_gear_system) {
        toggle(mod_enabled_gear_system); // disable stance switching when pressing face buttons
        disable_cam_reset(mod_enabled_gear_system); // disable cam reset, we need the button
    }

    mod_enabled_stance_guards = cfg.get<bool>("stance_guards").value_or(false);
}
// during save
void StanceControl::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("stance_control", mod_enabled);
    cfg.set<bool>("stance_control_invert", invert_input);
    cfg.set<bool>("stance_control_ui", show_new_ui);
    cfg.set<bool>("stance_control_edit_old_ui", edit_old_ui);
    cfg.set<bool>("stance_control_invert_mid", invert_mid);
    cfg.set<float>("stance_control_high_bound", highBound);
    cfg.set<float>("stance_control_low_bound", lowBound);

    cfg.set<bool>("swap_idle_stances", swapIdleStances);

    cfg.set<bool>("disable_combo_extend_speedup", mod_enabled_disable_combo_extend_speedup);

    cfg.set<bool>("faster_nu_lows", mod_enabled_faster_nu_lows);

    cfg.set<bool>("gear_system", mod_enabled_gear_system);
    cfg.set<bool>("gear_system_holds", gear_system_holds);

    cfg.set<bool>("stance_guards", mod_enabled_stance_guards);
}

// will show up in debug window, dump ImGui widgets you want here
//void StanceControl::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
