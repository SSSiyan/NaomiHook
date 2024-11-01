#include "StanceControl.hpp"
#if 1
bool StanceControl::mod_enabled = false;
bool StanceControl::invert_input = false;
uintptr_t StanceControl::r2PushAddr = NULL;
uintptr_t StanceControl::jmp_ret1 = NULL;

float StanceControl::r2Mult = 127.5;
float StanceControl::r2Sub = 1;
float StanceControl::invert = -1;
float StanceControl::highBound = 0.9;
float StanceControl::lowBound = -0.9;

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

// clang-format off
naked void detour1() { // 
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
        push eax
        mov eax, [StanceControl::r2PushAddr]
        movss xmm0, [eax]
        pop eax
        divss xmm0, [StanceControl::r2Mult] // Gamepad reads 0-255
        subss xmm0, [StanceControl::r2Sub] // we need it to read -1 to 1
        cmp byte ptr [StanceControl::invert_input], 0
        je skipInvert
        mulss xmm0, [StanceControl::invert]
    skipInvert:
        comiss xmm0, [StanceControl::highBound]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBound]
        jb writeLow
    // writeMid
        mov dword ptr [esi+0x1350], 1
        jmp originalcode

    writeHigh:
        mov dword ptr [esi+0x1350], 0
        jmp originalcode

    writeLow:
        mov dword ptr [esi+0x1350], 2
    originalcode:
        movss [esi+0x0000139C], xmm0
        jmp dword ptr [StanceControl::jmp_ret1]
    }
}

 // clang-format on

std::optional<std::string> StanceControl::on_initialize() {
    r2PushAddr = g_framework->get_module().as<uintptr_t>() + 0x849D74; 
    if (!install_hook_offset(0x3D7D6B, m_hook1, &detour1, &StanceControl::jmp_ret1, 8)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    return Mod::on_initialize();
}

void StanceControl::on_draw_ui() {
    if (ImGui::Checkbox("Stance Control on R2", &mod_enabled)) {
        toggle(mod_enabled);
    }
    ImGui::SliderFloat("highBound", &StanceControl::highBound, 0.0f, 1.0f);
    help_marker("How far should r2 be pushed to enter high stance");
    ImGui::SliderFloat("lowBound", &StanceControl::lowBound, -1.0f, 0.0f);
    help_marker("How little should r2 be pushed to enter low stance");
    ImGui::Checkbox("Invert", &StanceControl::invert_input);
}

// during load
void StanceControl::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("stance_control").value_or(false);
    toggle(mod_enabled);
    invert_input = cfg.get<bool>("stance_control_invert").value_or(false);
    highBound = cfg.get<float>("stance_control_high_bound").value_or(0.9f);
    lowBound = cfg.get<float>("stance_control_low_bound").value_or(-0.9f);
}
// during save
void StanceControl::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("stance_control", mod_enabled);
    cfg.set<bool>("stance_control_invert", invert_input);
    cfg.set<float>("stance_control_high_bound", highBound);
    cfg.set<float>("stance_control_low_bound", lowBound);
}

// do something every frame
//void StanceControl::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void StanceControl::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
