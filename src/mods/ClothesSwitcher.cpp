#include "ClothesSwitcher.hpp"
#if 1

uintptr_t* ClothesSwitcher::GetPlayerPtr(void){
    uintptr_t gpBattleAddr = g_framework->get_module().as<uintptr_t>() + 0x843584;
    uintptr_t* gpBattle = (uintptr_t*)gpBattleAddr;
    uintptr_t* gpBattleBase = *(uintptr_t**)gpBattle;
    uintptr_t* mHRPcAddr = (uintptr_t*)((char*)gpBattleBase + 0x164); // add as bytes
    uintptr_t* mHRPc = *(uintptr_t**)mHRPcAddr;
    return mHRPc;
}

std::optional<std::string> ClothesSwitcher::on_initialize() {
    return Mod::on_initialize();
}

typedef char(__thiscall* mSetEquipFunc)(
    uintptr_t* playerPtr, 
    int inID,
    bool inPowUp);

void ClothesSwitcher::on_draw_ui() {
    if (ImGui::Button("Set Equip")) {
        uintptr_t* mHRPc = ClothesSwitcher::GetPlayerPtr();
        uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
        mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
        if (mHRPc) {
            if (mHRPc) {
                setEquip(mHRPc, 100, 0);
            }
        }
    }
}

// during load
//void ClothesSwitcher::on_config_load(const utility::Config &cfg) {}
// during save
//void ClothesSwitcher::on_config_save(utility::Config &cfg) {}
// do something every frame
//void ClothesSwitcher::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ClothesSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
