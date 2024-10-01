#include "WeaponSwitcher.hpp"
#if 1

bool WeaponSwitcher::mod_enabled = false;
float WeaponSwitcher::weaponSwitchCooldown = 20.0f;

typedef bool(__thiscall* mPlayMotionFunc)(uintptr_t* playerPtr, int inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate); // 0x402CF0 // ida says __userpurge
typedef char(__thiscall* mSetEquipFunc)(uintptr_t* playerPtr, int inID, bool inPowUp); // 0x3E2240
typedef char(__thiscall* mSetEquipMk3Func)(uintptr_t* playerPtr); // 0x3E29C0 
typedef bool(__thiscall* mCheckCanAttackFunc)(uintptr_t* playerPtr); // 0x3D47B0 
typedef bool(__thiscall* mCheckGuardMotionFunc)(uintptr_t* playerPtr, bool inDefGuardNoChk); // 0x3E26C0 
typedef bool(__thiscall* mCheckHajikareFunc)(uintptr_t* playerPtr); // 0x3D4BA0 
typedef bool(__thiscall* mCheckTsubazeringFunc)(uintptr_t* playerPtr, int unkn); // 0x3DFFC0 
typedef bool(__thiscall* mCheckSideStepFunc)(uintptr_t* playerPtr, int unkn); // call this with -1 // 0x3E2730

// check player is != 0 after calling this
uintptr_t* WeaponSwitcher::GetPlayerPtr(void){
    uintptr_t gpBattleAddr = g_framework->get_module().as<uintptr_t>() + 0x843584;
    uintptr_t* gpBattle = (uintptr_t*)gpBattleAddr;
    if (gpBattle) {
        uintptr_t* gpBattleBase = *(uintptr_t**)gpBattle;
        if (gpBattleBase) {
        uintptr_t* mHRPcAddr = (uintptr_t*)((char*)gpBattleBase + 0x164); // add as bytes
            uintptr_t* mHRPc = *(uintptr_t**)mHRPcAddr;
            return mHRPc;
        }
    }
    return NULL;
}

void WeaponSwitcher::PlayMotion(int inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate) {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
    mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
    if (mHRPc) {
        playMotion(mHRPc, inMotNo, inLoop, inStartFrame, inOverWrite, inInterpolate);
    }
}

void WeaponSwitcher::SetEquip(int inID, bool inPowUp) {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
    mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
    if (mHRPc) {
        setEquip(mHRPc, inID, inPowUp);
    }
}

void WeaponSwitcher::SetMk3Equip() {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t setEquipMk3Address = (g_framework->get_module().as<uintptr_t>() + 0x3E29C0);
    mSetEquipMk3Func setEquipMk3 = (mSetEquipMk3Func)setEquipMk3Address;
    if (mHRPc) {
        setEquipMk3(mHRPc);
    }
}

bool WeaponSwitcher::CheckCanAttack() {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t checkCanAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D47B0);
    mCheckCanAttackFunc checkCanAttack = (mCheckCanAttackFunc)checkCanAttackAddress;
    if (mHRPc) {
        return checkCanAttack(mHRPc);
    }
    return false; // player ptr is invalid, treat as if we can't attack
}

bool WeaponSwitcher::CheckGuardMotion(bool inDefGuardNoChk) {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t checkGuardMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E26C0);
    mCheckGuardMotionFunc checkGuardMotion = (mCheckGuardMotionFunc)checkGuardMotionAddress;
    if (mHRPc) {
        return checkGuardMotion(mHRPc, inDefGuardNoChk);
    }
    return true; // player ptr is invalid, treat as if we're guarding
}

bool WeaponSwitcher::CheckHajikare() {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t checkHajikareAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D4BA0);
    mCheckHajikareFunc checkHajikare = (mCheckHajikareFunc)checkHajikareAddress;
    if (mHRPc) {
        return checkHajikare(mHRPc);
    }
    return true; // player ptr is invalid, treat as if we're taking a hit
}

bool WeaponSwitcher::CheckTsubazering() {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t checkTsubazeringAddress = (g_framework->get_module().as<uintptr_t>() + 0x3DFFC0);
    mCheckTsubazeringFunc checkTsubazering = (mCheckTsubazeringFunc)checkTsubazeringAddress;
    if (mHRPc) {
        return checkTsubazering(mHRPc, -1);
    }
    return true; // player ptr is invalid, treat as if we're clashing
}

bool WeaponSwitcher::CheckSideStep(int unkn) {
    uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
    uintptr_t checkSideStepAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2730);
    mCheckSideStepFunc checkSideStep = (mCheckSideStepFunc)checkSideStepAddress;
    if (mHRPc) {
        return checkSideStep(mHRPc, unkn);
    }
    return true; // player ptr is invalid, treat as if we're dodging
}

void WeaponSwitcher::toggleForceMap(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC204, m_patch, "\xEB", 1); // jmp nmh.exe+3DC21A
    }
    else {
        install_patch_offset(0x3DC204, m_patch, "\x75", 1); // jne nmh.exe+3DC21A
    }
}

bool IsNewWeaponActuallyNew(int dPadInput, int currentWeapon) {
    // dPadInput, currentWeapon
    std::array<std::pair<int, int>, 4> inputs = {{
        {1, 2},
        {2, 1},
        {4, 3},
        {8, 0}
    }};

    for (const auto& input : inputs) {
        if (dPadInput == input.first && currentWeapon == input.second) {
            return false;
        }
    }
    return true;
}

bool WeaponSwitcher::CanWeaponSwitch(int dPadInput, int currentWeapon) {
    uintptr_t* playerPtr = GetPlayerPtr();
    if (playerPtr) {
        char* playerBaseChar = (char*)playerPtr;
        int* currentMoveIDAddress = (int*)(playerBaseChar + 0x18C);
        int currentMoveID = *currentMoveIDAddress;

        int* currentModeAddress = (int*)(playerBaseChar + 0x2990);
        int currentMode = *currentModeAddress;

        if (IsNewWeaponActuallyNew(currentWeapon, dPadInput) && CheckCanAttack() &&
            !CheckGuardMotion(false) && !CheckHajikare() && !CheckTsubazering() && !CheckSideStep(-1)
            && currentMode == 4 && currentMoveID != 44 && currentMoveID != 45 && currentMoveID != 26 && currentMoveID != 28 && currentMoveID != 29) {
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
        PlayMotion(motionID, 0, 0, 0, 0.1f);
    }

    static int equipID = 601;
    ImGui::InputInt("Equip ID", &equipID);
    if (ImGui::Button("Set Equip")) {
        SetEquip(equipID, false);
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
            uintptr_t* playerPtr = GetPlayerPtr();
            if (playerPtr) {
                char* playerBaseChar = (char*)playerPtr;
                uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
                int8_t dPadInput = *(int8_t*)dPadInputsAddr;
                int* currentWeaponAddr = (int*)(playerBaseChar + 0x42C);
                int currentWeapon = *currentWeaponAddr;
                if (dPadInputsAddr) {
                    switch (dPadInput) {
                    case 1: // left, mk1
                        if (CanWeaponSwitch(dPadInput, currentWeapon)) {
                            SetEquip(2, false);
                            weaponSwitchCooldown = 0.0f;
                        }
                        break;
                    case 2: // right, mk3
                        if (CanWeaponSwitch(dPadInput, currentWeapon)) {
                            SetMk3Equip();
                            weaponSwitchCooldown = 0.0f;
                        }
                        break;
                    case 4: // down, mk2
                        if (CanWeaponSwitch(dPadInput, currentWeapon)) {
                            SetEquip(3, false);
                            weaponSwitchCooldown = 0.0f;
                        }
                        break;
                    case 8: // up, berry
                        if (CanWeaponSwitch(dPadInput, currentWeapon)) {
                            SetEquip(0, false);
                            weaponSwitchCooldown = 0.0f;
                        }
                        break;
                    default:
                        break;
                    }
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
