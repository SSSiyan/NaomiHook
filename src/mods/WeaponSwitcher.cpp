#include "WeaponSwitcher.hpp"
#if 1

bool WeaponSwitcher::mod_enabled = false;
float WeaponSwitcher::weaponSwitchCooldown = 20.0f;

// Disable toggling the map while Weapon Switcher is active
void WeaponSwitcher::toggleForceMap(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC204, m_patch, "\xEB", 1); // jmp nmh.exe+3DC21A
    }
    else {
        install_patch_offset(0x3DC204, m_patch, "\x75", 1); // jne nmh.exe+3DC21A
    }
}

// check a weapon switch ban list of states and animations
bool WeaponSwitcher::CanWeaponSwitch(pcItem desiredWeapon) {
    if (mHRPc* playerPtr = nmh_sdk::get_mHRPc()) {
        enPcInputMode currentMode = playerPtr->mInputMode;
        pcItem currentWeapon = playerPtr->mPcStatus.equip[0].id;
        pcMotion currentMoveID = playerPtr->mCharaStatus.motionNo;
        if (desiredWeapon != currentWeapon && 
            currentMode == ePcInputBattleIdle &&
            nmh_sdk::CheckCanAttack() &&
            !nmh_sdk::CheckGuardMotion(false) && 
            !nmh_sdk::CheckHajikare() && // eating a hit
            !nmh_sdk::CheckTsubazering(-1) && // clashing
            !nmh_sdk::CheckSideStep(-1) && // dodging back or left or right
            currentMoveID != ePcMtBtryChrgSt &&
            currentMoveID != ePcMtBtryChrgLp &&
            currentMoveID != ePcMtStpF &&
            currentMoveID != ePcMtAvdR &&
            currentMoveID != ePcMtAvdL) {
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
    static pcMotion motionID = pcMotion::ePcMtBtLSSonic;
    ImGui::InputInt("Motion ID", (int*)&motionID);
    if (ImGui::Button("Play Move")) {
        nmh_sdk::PlayMotion(motionID, 0, 0, 0, 0.1f);
    }

    static pcItem equipID = SHIRT1;
    ImGui::InputInt("Equip ID", (int*)&equipID);
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
            if (dPadInputsAddr) {
                int8_t dPadInput = *(int8_t*)dPadInputsAddr;
                switch (dPadInput) {
                case DPAD_LEFT:
                    if (CanWeaponSwitch(TSUBAKI_MK1)) {
                        nmh_sdk::SetEquip(TSUBAKI_MK1, false);
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case DPAD_RIGHT:
                    if (CanWeaponSwitch(TSUBAKI_MK3)) {
                        nmh_sdk::SetMk3Equip();
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case DPAD_DOWN:
                    if (CanWeaponSwitch(TSUBAKI_MK2)) {
                        nmh_sdk::SetEquip(TSUBAKI_MK2, false);
                        weaponSwitchCooldown = 0.0f;
                    }
                    break;
                case DPAD_UP:
                    if (CanWeaponSwitch(BLOOD_BERRY)) {
                        nmh_sdk::SetEquip(BLOOD_BERRY, false);
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
