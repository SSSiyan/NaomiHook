#include "StanceControl.hpp"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/KanaeTextureAtlas.cpp"
#include "utility/Compressed.hpp"
#include "imgui_internal.h"
#include "ScreenInfo.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

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
    static constexpr auto BACK_GLOW_HIGH() {
        return Frame {
            ImVec2 { 0.0f, 0.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 0.0f / 4096.0f, 0.0f / 4096.0f },
            ImVec2 { ( 0.0f + 1920.0f ) / 4096.0f, ( 0.0f + 1080.0f ) / 4096.0f }
        }; //BACK_GLOW_HIGH
    };
    static constexpr auto BACK_GLOW_LOW() {
        return Frame {
            ImVec2 { 0.0f, 1080.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 0.0f / 4096.0f, 1080.0f / 4096.0f },
            ImVec2 { ( 0.0f + 1920.0f ) / 4096.0f, ( 1080.0f + 1080.0f ) / 4096.0f }
        }; //BACK_GLOW_LOW
    };
    static constexpr auto BACK_GLOW_MID() {
        return Frame {
            ImVec2 { 1920.0f, 0.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1920.0f / 4096.0f, 0.0f / 4096.0f },
            ImVec2 { ( 1920.0f + 1920.0f ) / 4096.0f, ( 0.0f + 1080.0f ) / 4096.0f }
        }; //BACK_GLOW_MID
    };
    static constexpr auto HIGH_GLOW_1() {
        return Frame {
            ImVec2 { 1920.0f, 1080.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1920.0f / 4096.0f, 1080.0f / 4096.0f },
            ImVec2 { ( 1920.0f + 1920.0f ) / 4096.0f, ( 1080.0f + 1080.0f ) / 4096.0f }
        }; //HIGH_GLOW_1
    };
    static constexpr auto LOW_GLOW_1() {
        return Frame {
            ImVec2 { 0.0f, 2160.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 0.0f / 4096.0f, 2160.0f / 4096.0f },
            ImVec2 { ( 0.0f + 1920.0f ) / 4096.0f, ( 2160.0f + 1080.0f ) / 4096.0f }
        }; //LOW_GLOW_1
    };
    static constexpr auto MID_GLOW_1() {
        return Frame {
            ImVec2 { 1920.0f, 2160.0f },  
            ImVec2 { 1920.0f, 1080.0f },
            ImVec2 { 1920.0f / 4096.0f, 2160.0f / 4096.0f },
            ImVec2 { ( 1920.0f + 1920.0f ) / 4096.0f, ( 2160.0f + 1080.0f ) / 4096.0f }
        }; //MID_GLOW_1
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
        install_patch_offset(0x3DE09F, m_patch1, "\x74\x10", 2); // jmp nmh.exe+3DE0B1 // enable low stance set
        install_patch_offset(0x3DE067, m_patch2, "\x74\x0A", 2); // je nmh.exe+3DE073 // enable high stance set
        install_patch_offset(0x3D7E48, m_patch3, "\x80\xBE\x49\x16\x00\x00\x01", 7); // cmp byte ptr [esi+00001649],01 // disable mid stance
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

// clang-format off
naked void detour1() { // originalcode writes stance blend to 0, we write actual values and set stance using it
    __asm {
    //
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
        movss xmm3, [StanceControl::invertGuard]
        subss xmm3, xmm0
        movss xmm0, xmm3 // @siy potentially unsafe, I haven't checked xmm3's use
    skipInvert2:
        comiss xmm0, [StanceControl::highBoundGuard]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBoundGuard]
        jb writeLow
        jmp writeMid
    }
}

naked void detour2() { // remap lock on cycle
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
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
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
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
            cmp byte ptr [StanceControl::mod_enabled], 0
            je originalcode
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

static bool g_kanae_drawcall { false };

static uintptr_t g_kanae_himitsu_skip       { NULL };
static uintptr_t g_kanae_himitsu_return     { NULL };
static uintptr_t g_kanae_ghm_draw_text_addr { NULL };
naked void kanae_himitsu_detour() {

    __asm {

        cmp byte ptr [StanceControl::mod_enabled], 0
        je original_code

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
    if (!ImGui::IsAnyItemHovered())
        StanceControl::hoveredDescription = defaultDescription;
    if (ImGui::Checkbox("Stance Control on R2", &mod_enabled)) {
        toggle(mod_enabled);
    }
    if (ImGui::IsItemHovered())
        StanceControl::hoveredDescription = "Remaps lock on cycle to R3. This is needed to avoid switching targets with every press of R2 when using this feature.";
    if (mod_enabled) {
        ImGui::Indent();

        ImGui::Text("High Bound");
        if (ImGui::SliderFloat("## highBound sliderfloat", &StanceControl::highBound, 0.0f, 1.0f, "%.2f")) {
            highBoundGuard = (highBound + 1.0f) / 2.0f;
        }
        if (ImGui::IsItemHovered())
            StanceControl::hoveredDescription = "How far should r2 be pushed to enter high stance\n0.9 default";

        ImGui::Text("Low Bound");
        if (ImGui::SliderFloat("## lowBound sliderfloat", &StanceControl::lowBound, -1.0f, 0.0f, "%.2f")) {
            lowBoundGuard = (lowBound + 1.0f) / 2.0f;
        }
        if (ImGui::IsItemHovered())
            StanceControl::hoveredDescription = "How little should r2 be pushed to enter low stance\n-0.9 default";

        ImGui::Checkbox("Invert", &StanceControl::invert_input);
        if (ImGui::IsItemHovered())
            StanceControl::hoveredDescription = "Swap Low and High";

        ImGui::Checkbox("Invert Mid", &StanceControl::invert_mid);
        if (ImGui::IsItemHovered())
            StanceControl::hoveredDescription = "Swap Mid and Low. The unused combos assigned to Mid stance are actually the original Low attacks. "
            "For this feature to make more sense, you can tick this to reorganize the stance order.";

        ImGui::Checkbox("Show Custom Stance UI", &StanceControl::show_new_ui);

        ImGui::Checkbox("Combo Extend Speedup On Low Attacks", &mod_enabled_faster_nu_lows);
        if (ImGui::IsItemHovered())
            StanceControl::hoveredDescription = "Apply the default combo extension speed upgrade to modded low stance attacks";

        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Swap Vanilla Mid and Low UI", &StanceControl::edit_old_ui)) {
        toggle_display_edit(edit_old_ui);
    }
    if (ImGui::IsItemHovered())
        StanceControl::hoveredDescription = "Makes the vanilla stance display consider the default low stance as mid stance.";

    if (ImGui::Checkbox("Swap Idle Stances", &swapIdleStances)) {
        toggleSwapIdleStances(swapIdleStances);
    }
    if (ImGui::IsItemHovered())
        StanceControl::hoveredDescription = "The High/Low stances are mistakenly inverted by default, forcing Travis to take on the incorrect stance. This setting "
                "corrects that issue and is purely cosmetic.";

    if (ImGui::Checkbox("Disable Combo Extend Speedup", &mod_enabled_disable_combo_extend_speedup)) {
        toggle_disable_combo_extend_speedup(mod_enabled_disable_combo_extend_speedup);
    }
    if (ImGui::IsItemHovered())
        StanceControl::hoveredDescription = "This takes priority over \"Combo Extend Speedup On Low Attacks\"";
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

// do something every frame
static uint32_t g_frame_counter = 0;
void StanceControl::on_frame() {
    if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
        auto mode = mHRPc->mInputMode;
        if (mode == ePcInputMenu) { return; }
        if (mHRPc->mOperate && StanceControl::mod_enabled && show_new_ui) {

            static constexpr TextureAtlas atlas{};
            struct KanaeDisp {
                std::array<Frame, 3> f;
            };

            static constexpr KanaeDisp kanae {
                { atlas.BACK_GLOW_HIGH(), atlas.BACK_GLOW_LOW(), atlas.BACK_GLOW_MID() } 
            };
            static constexpr KanaeDisp kanae_glow {
                { atlas.HIGH_GLOW_1(), atlas.LOW_GLOW_1(), atlas.MID_GLOW_1() }
            };

            int pose = mHRPc->mPcStatus.pose;

            const auto& io = ImGui::GetIO();
            const auto& tex = g_kanae_texture_atlas->GetTexture();
            ImDrawList* dl = ImGui::GetForegroundDrawList();

            auto* scinfo = ScreenInfo::get_screen_info();
            assert(scinfo);

            ImVec2 points[] = {
                ImVec2(scinfo->screen_left,  scinfo->screen_top),
                ImVec2(scinfo->screen_right, scinfo->screen_bottom),

            };
            ImVec2 kanae_uvs[] = {
                ImVec2(kanae.f[pose].uv0),
                ImVec2(kanae.f[pose].uv1),
            };
            ImVec2 glow_uvs[] = {
                ImVec2(kanae_glow.f[pose].uv0),
                ImVec2(kanae_glow.f[pose].uv1),
            };

            dl->AddImage(tex, points[0], points[1], kanae_uvs[0], kanae_uvs[1]);
            //dl->AddImageQuad(tex, points[0], points[1], points[2], points[3], kanae_uvs[0], kanae_uvs[1], kanae_uvs[2], kanae_uvs[3]);
            
#if 1
            float meme = (float)templeos_hymn_risen_values[g_frame_counter % templeos_hymn_risen_range];
            float glow = glm::clamp(meme, 179.0f, 255.0f);
#else // game logic i think

            HrScreenStatus* v4 = nmh_sdk::get_mHRBattle()->mBtEffect.pScreenStatus;
            float glow = (((float)v4->m_GearRandCounter[1] / (float)v4->m_GearRandCounter[0]) * 255.0);
#endif
            ImU32 oppacity = IM_COL32(255, 255, 255, (char)glow);
            dl->AddImage(tex, points[0], points[1], glow_uvs[0], glow_uvs[1], oppacity);
            //dl->AddImageQuad(tex, points[0], points[1], points[2], points[3], glow_uvs[0], glow_uvs[1], glow_uvs[2], glow_uvs[3], oppacity);
            //dl->AddRectFilled(p0, p1, -1, 2.0f);

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
    g_kanae_texture_atlas.reset();
    load_kanae_texture();
}

std::optional<std::string> StanceControl::on_initialize() {

    gpPad                      = g_framework->get_module().as<uintptr_t>() + 0x849D10;
    clashing                   = g_framework->get_module().as<uintptr_t>() + 0x3DFFC0;
    g_kanae_himitsu_skip       = g_framework->get_module().as<uintptr_t>() + 0x409D1D;
    g_kanae_ghm_draw_text_addr = g_framework->get_module().as<uintptr_t>() + 0x5E8BC0;

    if (!install_hook_offset(0x3D7D6B, m_hook1, &detour1, &StanceControl::jmp_ret1, 8)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    jmp2je = g_framework->get_module().as<uintptr_t>() + 0x3C46F8;
    if (!install_hook_offset(0x3C4645, m_hook2, &detour2, &StanceControl::jmp_ret2, 13)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    jmp_jne3 = g_framework->get_module().as<uintptr_t>() + 0x3D7EC8;
    if (!install_hook_offset(0x3D7EBF, m_hook3, &detour3, &StanceControl::jmp_ret3, 7)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    mCheckNormalAttack = g_framework->get_module().as<uintptr_t>() + 0x3D3D60;
    if (!install_hook_offset(0x3C72DD, m_hook4, &detour4, &StanceControl::jmp_ret4, 5)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
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
void StanceControl::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("stance_control").value_or(false);
    toggle(mod_enabled);
    invert_input = cfg.get<bool>("stance_control_invert").value_or(false);
    show_new_ui = cfg.get<bool>("stance_control_ui").value_or(true);
    edit_old_ui = cfg.get<bool>("stance_control_edit_old_ui").value_or(false);
    toggle_display_edit(edit_old_ui);
    invert_mid = cfg.get<bool>("stance_control_invert_mid").value_or(true);
    highBound = cfg.get<float>("stance_control_high_bound").value_or(0.9f);
    lowBound = cfg.get<float>("stance_control_low_bound").value_or(-0.9f);
    highBoundGuard = (highBound + 1.0f) / 2.0f;
    lowBoundGuard = (lowBound + 1.0f) / 2.0f;

    swapIdleStances = cfg.get<bool>("swap_idle_stances").value_or(false);
    toggleSwapIdleStances(swapIdleStances);

    mod_enabled_disable_combo_extend_speedup = cfg.get<bool>("disable_combo_extend_speedup").value_or(false);
    toggle_disable_combo_extend_speedup(mod_enabled_disable_combo_extend_speedup);

    mod_enabled_faster_nu_lows = cfg.get<bool>("faster_nu_lows").value_or(false);
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
}

// will show up in debug window, dump ImGui widgets you want here
//void StanceControl::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
