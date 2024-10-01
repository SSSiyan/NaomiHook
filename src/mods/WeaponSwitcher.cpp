#include "WeaponSwitcher.hpp"
#if 1

uintptr_t* WeaponSwitcher::GetPlayerPtr(void){
    uintptr_t gpBattleAddr = g_framework->get_module().as<uintptr_t>() + 0x843584;
    uintptr_t* gpBattle = (uintptr_t*)gpBattleAddr;
    uintptr_t* gpBattleBase = *(uintptr_t**)gpBattle;
    uintptr_t* mHRPcAddr = (uintptr_t*)((char*)gpBattleBase + 0x164); // add as bytes
    uintptr_t* mHRPc = *(uintptr_t**)mHRPcAddr;
    return mHRPc;
}

std::optional<std::string> WeaponSwitcher::on_initialize() {
    return Mod::on_initialize();
}

typedef bool(__thiscall* mPlayMotionFunc)(
    uintptr_t* playerPtr,
    int inMotNo,
    bool inLoop,
    int inStartFrame,
    bool inOverWrite,
    float inInterpolate);

typedef char(__thiscall* mSetEquipFunc)(
    uintptr_t* playerPtr, 
    int inID,
    bool inPowUp);

void WeaponSwitcher::on_draw_ui() {
    static int motionID = 174;
    ImGui::InputInt("Motion ID", &motionID);
    if (ImGui::Button("Play Move")) {
        uintptr_t* mHRPc = WeaponSwitcher::GetPlayerPtr();
        uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
        mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
        if (mHRPc) {
            if (mHRPc) {
                playMotion(mHRPc, motionID, 0, 0, 0, 0.1f);
            }
        }
    }

    static int equipID = 601;
    ImGui::InputInt("Equip ID", &equipID);
    if (ImGui::Button("Set Equip")) {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
        mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
        if (mHRPc) {
            if (mHRPc) {
                setEquip(mHRPc, equipID, 0);
            }
        }
    }
}

// during load
//void WeaponSwitcher::on_config_load(const utility::Config &cfg) {}
// during save
//void WeaponSwitcher::on_config_save(utility::Config &cfg) {}
// do something every frame
//void WeaponSwitcher::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void WeaponSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
