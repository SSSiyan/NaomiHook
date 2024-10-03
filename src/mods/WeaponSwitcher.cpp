#include "WeaponSwitcher.hpp"
#if 1

bool WeaponSwitcher::mod_enabled = false;
float WeaponSwitcher::weaponSwitchCooldown = 20.0f;

void WeaponSwitcher::toggleForceMap(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC204, m_patch, "\xEB", 1); // jmp nmh.exe+3DC21A
    }
    else {
        install_patch_offset(0x3DC204, m_patch, "\x75", 1); // jne nmh.exe+3DC21A
    }
}

bool WeaponSwitcher::CanWeaponSwitch(int newWeapon) {
    if (mHRPc* playerPtr = nmh_sdk::get_mHRPc()) {
        int currentMode = playerPtr->mInputMode;
        int currentWeapon = playerPtr->mPcStatus.equip[0].id;
        int currentMoveID = playerPtr->mCharaStatus.motionNo;
        if (newWeapon != currentWeapon && currentMode == 4 && nmh_sdk::CheckCanAttack() &&
            !nmh_sdk::CheckGuardMotion(false) && !nmh_sdk::CheckHajikare() && !nmh_sdk::CheckTsubazering() && !nmh_sdk::CheckSideStep(-1)
            && currentMoveID != 44 && currentMoveID != 45 && currentMoveID != 26 && currentMoveID != 28 && currentMoveID != 29) {
            return true;
        }
    }
    return false;
}

std::optional<std::string> WeaponSwitcher::on_initialize() {
    return Mod::on_initialize();
}

void WeaponSwitcher::on_draw_ui() {
    if (ImGui::Checkbox("Enable Weapon Switcher", &mod_enabled)) {
        toggleForceMap(mod_enabled);
    }
    help_marker("Enable the main mod. This binds weapon swaps to Dpad.");
    static int motionID = 174;
    ImGui::InputInt("Motion ID", &motionID);
    if (ImGui::Button("Play Move")) {
        nmh_sdk::PlayMotion(motionID, 0, 0, 0, 0.1f);
    }

    static int equipID = 601;
    ImGui::InputInt("Equip ID", &equipID);
    if (ImGui::Button("Set Equip")) {
        nmh_sdk::SetEquip(equipID, false);
    }
}

// during load
void WeaponSwitcher::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("weapon_switcher").value_or(false);
    toggleForceMap(mod_enabled);
}
// during save
void WeaponSwitcher::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("weapon_switcher", mod_enabled);
}

void WeaponSwitcher::on_frame() {
    if (mod_enabled) {
        if (weaponSwitchCooldown > 20.0f) {
            uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
            int8_t dPadInput = *(int8_t*)dPadInputsAddr;
            if (dPadInputsAddr) {
                switch (dPadInput) {
                case 1: // left, mk1
                    if (CanWeaponSwitch(2)) {
                        nmh_sdk::SetEquip(2, false);
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case 2: // right, mk3
                    if (CanWeaponSwitch(1)) {
                        nmh_sdk::SetMk3Equip();
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case 4: // down, mk2
                    if (CanWeaponSwitch(3)) {
                        nmh_sdk::SetEquip(3, false);
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case 8: // up, berry
                    if (CanWeaponSwitch(0)) {
                        nmh_sdk::SetEquip(0, false);
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        weaponSwitchCooldown++; // get delta sometime :)
    }
}

// will show up in debug window, dump ImGui widgets you want here
//void WeaponSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
