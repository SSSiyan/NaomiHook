#include "LockOnSettings.hpp"
#if 1
bool LockOnSettings::lockon_sidesteps = false;
bool LockOnSettings::lockon_deathblows = false;
bool LockOnSettings::lockon_parry_qtes = false;

void LockOnSettings::toggle_sidestep_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4893, patch1, "\xEB", 1); // jmp nmh.exe+3C489C
    }
    else {
        install_patch_offset(0x3C4893, patch1, "\x74", 1); // je nmh.exe+3C489C
    }
}

void LockOnSettings::toggle_deathblow_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3D08E5, patch2, "\x90\x90\x90\x90\x90", 5); // nop 5
    }
    else {
        install_patch_offset(0x3D08E5, patch2, "\x0F\x84\x2E\x00\x00", 5); // call nmh.mHRPc::mInitLockOn
    }
}

void LockOnSettings::toggle_parry_qte_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4268, patch3, "\xE9\x2F\x06\x00\x00\x90", 6); // jmp nmh.mHRPc::mUpdateLockOnTarget+66C
    }
    else {
        install_patch_offset(0x3C4268, patch3, "\x0F\x84\x2E\x06\x00\x00", 6); // je nmh.mHRPc::mUpdateLockOnTarget+66C
    }
}

std::optional<std::string> LockOnSettings::on_initialize() {
    return Mod::on_initialize();
}

void LockOnSettings::on_draw_ui() {
    if (ImGui::Checkbox("Lock On During Sidesteps", &lockon_sidesteps)) {
        toggle_sidestep_lockon(lockon_sidesteps);
    }
    if (ImGui::Checkbox("Lock On During Deathblows", &lockon_deathblows)) {
        toggle_deathblow_lockon(lockon_deathblows);
    }
    if (ImGui::Checkbox("Lock On During Parry QTEs", &lockon_parry_qtes)) {
        toggle_parry_qte_lockon(lockon_parry_qtes);
    }
}

// during load
void LockOnSettings::on_config_load(const utility::Config &cfg) {
    lockon_sidesteps = cfg.get<bool>("lockon_sidesteps").value_or(false);
    toggle_sidestep_lockon(lockon_sidesteps);
    lockon_deathblows = cfg.get<bool>("lockon_deathblows").value_or(false);
    toggle_deathblow_lockon(lockon_deathblows);
    lockon_parry_qtes = cfg.get<bool>("lockon_parry_qtes").value_or(false);
    toggle_parry_qte_lockon(lockon_parry_qtes);
}

// during save
void LockOnSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("lockon_sidesteps", lockon_sidesteps);
    cfg.set<bool>("lockon_deathblows", lockon_deathblows);
    cfg.set<bool>("lockon_parry_qtes", lockon_parry_qtes);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
