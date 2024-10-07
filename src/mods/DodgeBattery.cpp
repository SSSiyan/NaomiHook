#include "DodgeBattery.hpp"
#if 1
bool DodgeBattery::mod_enabled = false;
uintptr_t DodgeBattery::jmp_ret1 = NULL;
uintptr_t DodgeBattery::gpBattle = NULL;
uintptr_t DodgeBattery::PC_INPUT_CHARGE_BAT_ENTER = NULL;
uintptr_t DodgeBattery::mCheckInputCharge = NULL;
uintptr_t DodgeBattery::mGetBtEffect = NULL;
uintptr_t DodgeBattery::SetStatusBatterySE_Flag = NULL;
uintptr_t DodgeBattery::dLeavingExceptBattery = NULL;
uintptr_t DodgeBattery::dAppearKeyGuide = NULL;
uintptr_t DodgeBattery::mPlayCamMotFromCharMot = NULL;
uintptr_t DodgeBattery::SoundPrep = NULL;
uintptr_t DodgeBattery::SoundSetOutput = NULL;
uintptr_t DodgeBattery::SoundPrepPlay = NULL;
uintptr_t DodgeBattery::mUpdateLockOnTarget = NULL;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        // 
            cmp byte ptr [DodgeBattery::mod_enabled], 0
            je originalcode
        // 
            //nmh.mHRPc::mFrameProc+133D
            cmp [edi+0x18c], 26 // front roll
            je originalcode
            cmp [edi+0x18c], 27 // back roll
            je originalcode
            cmp [edi+0x18c], 30 // right roll
            je originalcode
            cmp [edi+0x18c], 31 // left roll
            je originalcode

            push eax
            push ecx
            push edx
            call dword ptr [DodgeBattery::PC_INPUT_CHARGE_BAT_ENTER]
            cmp al, 1
            jne popcode
            mov ecx, edi
            call dword ptr [DodgeBattery::mCheckInputCharge]
            cmp al, 1
            jne popcode
            mov ecx, [DodgeBattery::gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeBattery::mGetBtEffect]
            push 01
            mov ecx, [eax]
            call dword ptr [DodgeBattery::SetStatusBatterySE_Flag]
            mov ecx, [DodgeBattery::gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeBattery::mGetBtEffect]
            push 01
            mov ecx, [eax]
            call dword ptr [DodgeBattery::dLeavingExceptBattery]
            mov ecx, [DodgeBattery::gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeBattery::mGetBtEffect]
            push 01
            push 00
            push 00
            mov ecx, [eax]
            push 00
            push 0x0E
            call dword ptr [DodgeBattery::dAppearKeyGuide]
            mov eax, [edi]
            push ecx
            mov dword ptr [esp], 0x3DCCCCCD
            mov ecx, edi
            push 0x00
            push 0x00
            push 0x00
            push 0x2C
            mov byte ptr [edi+0x00001345], 00
            call dword ptr [eax+0x000000A0]
            push 0x00
            push 0x01
            push 0x00
            push -01
            push 0x2C
            mov ecx, edi
            call dword ptr [DodgeBattery::mPlayCamMotFromCharMot]
            mov edx, 0x00000002
            mov ecx, 0x00000275
            call dword ptr [DodgeBattery::SoundPrep]
            mov esi,eax
            mov edx, 0x00000001
            mov ecx,esi
            call dword ptr [DodgeBattery::SoundSetOutput]
            mov ecx,esi
            call dword ptr [DodgeBattery::SoundPrepPlay]
            mov ecx,edi
            mov esi, 0x00000003
            call dword ptr [DodgeBattery::mUpdateLockOnTarget]
            mov eax, [DodgeBattery::gpBattle]
            mov eax, [eax]
            mov dword ptr [eax+0x158], 0 // bulletSlow

        popcode:
            pop edx
            pop ecx
            pop eax
        originalcode:
            mov ecx, [edi+0x00000320]
            jmp dword ptr [DodgeBattery::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> DodgeBattery::on_initialize() {
    DodgeBattery::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    DodgeBattery::PC_INPUT_CHARGE_BAT_ENTER = g_framework->get_module().as<uintptr_t>() + 0x3DDBB0;
    DodgeBattery::mCheckInputCharge = g_framework->get_module().as<uintptr_t>() + 0x3C38A0;
    DodgeBattery::mGetBtEffect = g_framework->get_module().as<uintptr_t>() + 0x9D670;
    DodgeBattery::SetStatusBatterySE_Flag = g_framework->get_module().as<uintptr_t>() + 0x9D4D0;
    DodgeBattery::dLeavingExceptBattery = g_framework->get_module().as<uintptr_t>() + 0x40E010;
    DodgeBattery::dAppearKeyGuide = g_framework->get_module().as<uintptr_t>() + 0x40EC60;
    DodgeBattery::mPlayCamMotFromCharMot = g_framework->get_module().as<uintptr_t>() + 0x402340;
    DodgeBattery::SoundPrep = g_framework->get_module().as<uintptr_t>() + 0x5F3500;
    DodgeBattery::SoundSetOutput = g_framework->get_module().as<uintptr_t>() + 0x5F3450;
    DodgeBattery::SoundPrepPlay = g_framework->get_module().as<uintptr_t>() + 0x5F3500;
    DodgeBattery::mUpdateLockOnTarget = g_framework->get_module().as<uintptr_t>() + 0x3C4230;

    if (!install_hook_offset(0x3DB2C6, m_hook1, &detour1, &DodgeBattery::jmp_ret1, 6)) {
        spdlog::error("Failed to init DodgeBattery mod\n");
        return "Failed to init DodgeBattery mod";
    }

    return Mod::on_initialize();
}

void DodgeBattery::on_draw_ui() {
    ImGui::Checkbox("Charge While Dodging", &mod_enabled);
}

// during load
void DodgeBattery::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("chargeWhileDodging").value_or(false);
}
// during save
void DodgeBattery::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("chargeWhileDodging", mod_enabled);
}

// do something every frame
//void DodgeBattery::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DodgeBattery::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
