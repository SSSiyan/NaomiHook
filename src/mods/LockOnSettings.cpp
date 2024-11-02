#include "LockOnSettings.hpp"

#if 1
bool LockOnSettings::mod_enabled = false;
// bool LockOnSettings::lockon_sidesteps = false;
bool LockOnSettings::lockon_deathblows = false;
bool LockOnSettings::lockon_deathblows_start = false;
bool LockOnSettings::lockon_parry_qtes = false;

uintptr_t LockOnSettings::jmp_ret1 = NULL;
bool LockOnSettings::target_switch_degrees_toggle = false;
float LockOnSettings::default_search_degrees = 1.57f;
float LockOnSettings::custom_search_degrees = 0.0f;

uintptr_t LockOnSettings::jmp_ret2 = NULL;
uintptr_t LockOnSettings::jmp_ja2 = NULL;
uintptr_t LockOnSettings::jmp_ret2alt = NULL;

/*void LockOnSettings::toggle_sidestep_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C42BE, patch0, "\x74\x12\x90\x90\x90\x90", 6); // je nmh.mHRPc::mUpdateLockOnTarget+A2 nop 4 // roll back
        install_patch_offset(0x3C42CC, patch1, "\x90\x90\x90\x90\x90\x90", 6); // nop 6 // other rolls
    }
    else {
        install_patch_offset(0x3C42BE, patch0, "\x0F\x84\xC8\x05\x00\x00", 6); // je nmh.mHRPc::mUpdateLockOnTarget+65C // roll back
        install_patch_offset(0x3C42CC, patch1, "\x0F\x8E\xBA\x05\x00\x00", 6); // jng nmh.mHRPc::mUpdateLockOnTarget+65C // other rolls
    }
}*/

void LockOnSettings::toggle_deathblow_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4429, patch2, "\xEB\x58", 2); // jmp nmh.exe+3C4483
    }
    else {
        install_patch_offset(0x3C4429, patch2, "\x75\x58", 2); // jne nmh.exe+3C4483
    }
}

void LockOnSettings::toggle_deathblow_lockon_start(bool enable) {
    if (enable) {
        install_patch_offset(0x3D08E5, patch3, "\x90\x90\x90\x90\x90", 5); // jmp nmh.exe+3C4483
    }
    else {
        install_patch_offset(0x3D08E5, patch3, "\xE8\xF6\x44\xFF\xFF", 5); // jne nmh.exe+3C4483
    }
}

void LockOnSettings::toggle_parry_qte_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4268, patch4, "\xE9\x2F\x06\x00\x00\x90", 6); // jmp nmh.mHRPc::mUpdateLockOnTarget+66C
    }
    else {
        install_patch_offset(0x3C4268, patch4, "\x0F\x84\x2E\x06\x00\x00", 6); // je nmh.mHRPc::mUpdateLockOnTarget+66C
    }
}

// clang-format off
naked void detour1() { // Horizontal Limit // player in ecx
    __asm {
        // 
            cmp byte ptr [LockOnSettings::target_switch_degrees_toggle], 0
            je originalcode
        // 
            movss xmm0, [LockOnSettings::custom_search_degrees]
            jmp retcode

        originalcode:
            movss xmm0, [LockOnSettings::default_search_degrees]
        retcode:
            jmp dword ptr [LockOnSettings::jmp_ret1]
    }
}

naked void detour2() { // Lock on during more actions
    __asm {
        // 
            cmp byte ptr [LockOnSettings::mod_enabled], 0
            je originalcode
        // 
            //lea eax,[edi-0x1B]
            //cmp eax, 06
            //ja jacode
            jmp qword ptr [LockOnSettings::jmp_ret2alt]

        originalcode:
            lea eax,[edi-0x1C]
            cmp eax, 06
            ja jacode
            jmp dword ptr [LockOnSettings::jmp_ret2]

        jacode:
            jmp dword ptr [LockOnSettings::jmp_ja2]
    }
}
 // clang-format on

std::optional<std::string> LockOnSettings::on_initialize() {
    if (!install_hook_offset(0x3C4B90, m_hook1, &detour1, &LockOnSettings::jmp_ret1, 8)) { // Horizontal Limit
        spdlog::error("Failed to init LockOnSettings mod\n");
        return "Failed to init LockOnSettings mod";
    }

    jmp_ret2alt = g_framework->get_module().as<uintptr_t>() + 0x3C43C0; // 0x3C4483
    jmp_ja2 = g_framework->get_module().as<uintptr_t>() + 0x3C42BB;
    if (!install_hook_offset(0x3C42A5, m_hook2, &detour2, &LockOnSettings::jmp_ret2, 8)) {
        spdlog::error("Failed to init LockOnSettings mod\n");
        return "Failed to init LockOnSettings mod";
    }

    return Mod::on_initialize();
}

void LockOnSettings::on_draw_ui() {
    ImGui::TextWrapped("pls check to see which of these still need to be ticked and I'll remove the ones that don't");

    ImGui::Checkbox("Enable lockon during more actions", &mod_enabled);

    // if (ImGui::Checkbox("Lock On During Sidesteps", &lockon_sidesteps)) {
    //     toggle_sidestep_lockon(lockon_sidesteps);
    // }
    if (ImGui::Checkbox("Full Lockon Freedom During Deathblows", &lockon_deathblows)) {
        toggle_deathblow_lockon(lockon_deathblows);
    }
    if (ImGui::Checkbox("Disable Deathblows removing your lockon (the real old one)", &lockon_deathblows_start)) {
        toggle_deathblow_lockon_start(lockon_deathblows_start);
    }
    if (ImGui::Checkbox("Disable Parry QTEs removing your lockon", &lockon_parry_qtes)) {
        toggle_parry_qte_lockon(lockon_parry_qtes);
    }

    ImGui::Checkbox("Custom Target Switch Horizontal Limit", &target_switch_degrees_toggle);
    if (target_switch_degrees_toggle) {
        ImGui::SliderFloat("Radians ##CustomSearchDegreesSliderFloat", &custom_search_degrees, 0.0f, 6.28f, "%.2f");
        help_marker("1.57 default (90 degrees)\n6.28 is a full circle");
    }
}

// during load
void LockOnSettings::on_config_load(const utility::Config &cfg) {
    // lockon_sidesteps = cfg.get<bool>("lockon_sidesteps").value_or(false);
    // toggle_sidestep_lockon(lockon_sidesteps);
    lockon_deathblows = cfg.get<bool>("lockon_deathblows").value_or(false);
    toggle_deathblow_lockon(lockon_deathblows);
    lockon_deathblows_start = cfg.get<bool>("lockon_deathblows_start").value_or(false);
    toggle_deathblow_lockon(lockon_deathblows_start);
    lockon_parry_qtes = cfg.get<bool>("lockon_parry_qtes").value_or(false);
    toggle_parry_qte_lockon(lockon_parry_qtes);

    mod_enabled = cfg.get<bool>("more_lockon_actions").value_or(false);
    target_switch_degrees_toggle = cfg.get<bool>("target_switch_degrees_toggle").value_or(false);
    custom_search_degrees = cfg.get<float>("custom_search_degrees").value_or(1.57f);
}

// during save
void LockOnSettings::on_config_save(utility::Config &cfg) {
    // cfg.set<bool>("lockon_sidesteps", lockon_sidesteps);
    cfg.set<bool>("lockon_deathblows", lockon_deathblows);
    cfg.set<bool>("lockon_deathblows_start", lockon_deathblows_start);
    cfg.set<bool>("lockon_parry_qtes", lockon_parry_qtes);

    cfg.set<bool>("more_lockon_actions", mod_enabled);
    cfg.set<bool>("target_switch_degrees_toggle", target_switch_degrees_toggle);
    cfg.set<float>("custom_search_degrees", custom_search_degrees);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
