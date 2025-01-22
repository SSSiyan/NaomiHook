#include "WeaponSwitcher.hpp"
#if 1

bool WeaponSwitcher::mod_enabled = false;
uintptr_t WeaponSwitcher::jmp_ret1 = NULL;
uintptr_t WeaponSwitcher::jmp_ret2 = NULL;
int WeaponSwitcher::weaponSwitchCooldown = 20;

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
        enCharaCondition condition = playerPtr->mCharaStatus.condition;
        if (desiredWeapon != currentWeapon && 
            currentMode == ePcInputBattleIdle &&
            condition == eGood &&
            nmh_sdk::CheckCanAttack() &&
            !nmh_sdk::CheckGuardMotion(false) && 
            //!nmh_sdk::CheckHajikare() && // eating a hit @siy this is bad
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

// clang-format off
naked void detour1() { // play weapon anims // player in ecx // called last
    __asm {
        //
            cmp byte ptr [WeaponSwitcher::mod_enabled], 0
            je originalcode
        //
            cmp dword ptr [ecx+0x2990], ePcInputBattleIdle // enPcInputMode
            jne originalcode
            cmp dword ptr [WeaponSwitcher::weaponSwitchCooldown], 10 // only edit anim if weapon switcher changed the value
            jae originalcode
            push 01 // interrupt
            push 00 // frame
            push 00 // repeat
            push 03 // id
            jmp dword ptr [WeaponSwitcher::jmp_ret1]


            cmp dword ptr [ecx+0x42C], BLOOD_BERRY
            je berry
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK3
            je mk3
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK1
            je mk1
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK2
            je mk2
            jmp originalcode

        berry:
            push 01
            push 70
            push 00
            push 249
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk3:
            push 01
            push 00
            push 00
            push 297
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk1:
            push 01
            push 00
            push 00
            push 345
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk2:
            push 01
            push 00
            push 00
            push 391
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        originalcode:
            push 01
            push 00
            push 01
            push 0x30
            jmp dword ptr [WeaponSwitcher::jmp_ret1]
    }
}

naked void detour2() { // play weapon anims // player in esi
    __asm {
        //
            cmp byte ptr [WeaponSwitcher::mod_enabled], 0
            je originalcode
        //
            cmp dword ptr [esi+0x2990], ePcInputBattleIdle // enPcInputMode
            jne originalcode
            cmp dword ptr [WeaponSwitcher::weaponSwitchCooldown], 10 // only edit anim if weapon switcher changed the value
            jae originalcode
            push 01 // interrupt
            push 00 // frame
            push 00 // repeat
            push 03 // id
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

            cmp dword ptr [esi+0x42C], BLOOD_BERRY
            je berry
            cmp dword ptr [esi+0x42C], TSUBAKI_MK3
            je mk3
            cmp dword ptr [esi+0x42C], TSUBAKI_MK1
            je mk1
            cmp dword ptr [esi+0x42C], TSUBAKI_MK2
            je mk2
            jmp originalcode

        berry:
            push 00
            push 70
            push 00
            push 249
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk3:
            push 00
            push 00
            push 00
            push 297
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk1:
            push 00
            push 00
            push 00
            push 345
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk2:
            push 00
            push 00
            push 00
            push 391
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        originalcode:
            push 00
            push 00
            push 00
            push 0x30
            jmp dword ptr [WeaponSwitcher::jmp_ret2]
    }
}
 // clang-format on

std::optional<std::string> WeaponSwitcher::on_initialize() {
    if (!install_hook_offset(0x3DC561, m_hook1, &detour1, &WeaponSwitcher::jmp_ret1, 8)) {
        spdlog::error("Failed to init WeaponSwitcher mod\n");
        return "Failed to init WeaponSwitcher mod";
    }

    if (!install_hook_offset(0x3D905B, m_hook2, &detour2, &WeaponSwitcher::jmp_ret2, 8)) {
        spdlog::error("Failed to init WeaponSwitcher mod\n");
        return "Failed to init WeaponSwitcher mod";
    }

    return Mod::on_initialize();
}

void WeaponSwitcher::on_draw_ui() {
    if (ImGui::Checkbox("Weapon Switcher", &mod_enabled)) {
        toggleForceMap(mod_enabled);
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
    //static int previousSwordEquipRead = 0;
    //static int checkmotReadProc = 0;
    if (mod_enabled) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
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
                            nmh_sdk::SetEquip(TSUBAKI_MK3, false);
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

            // play an anim after the sword loads
            /*if (previousSwordEquipRead == eEqWait1Frame && player->mInputMode == ePcInputBattleIdle) {
                switch (player->mPcStatus.equip[0].id) {
                case BLOOD_BERRY:
                    nmh_sdk::PlayMotion(ePcMtBattou01, 0, 70, 1, 0.1f); // 249
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK3:
                    nmh_sdk::PlayMotion(ePcMtBattou02Ed, 0, 0, 1, 0.1f); // 297
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK1:
                    nmh_sdk::PlayMotion(ePcMtBattou03Ed, 0, 0, 1, 0.1f); // 345
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK2:
                    nmh_sdk::PlayMotion(ePcMtBattou04Ed, 0, 0, 1, 0.1f); // 391
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                default:
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                }
            }*/

            // effectively pause the game while loading a sword. Optional but should be safer
            // Can't pause all because then the loading process pauses too
            // Of course this lets you move during cutscenes I am so dumb
            /*if (player->mInputMode == ePcInputBattleIdle) {
                if (player->mPcStatus.equip[0].readProc != eEqReadMax) {
                    player->mPauseNpc = true;
                    player->mOperate = false;
                }
                else {
                    player->mPauseNpc = false;
                    player->mOperate = true;
                }
            }
            previousSwordEquipRead = player->mPcStatus.equip[0].readProc;*/
            weaponSwitchCooldown++;
        }
    }
}

// will show up in debug window, dump ImGui widgets you want here
//void WeaponSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
