#include "DodgeSettings.hpp"
#if 1
bool DodgeSettings::roll_forward_mod_enabled = false;
uintptr_t DodgeSettings::roll_forward_jmp_ret1 = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_LeftStickUp = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_LeftStickLeft = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_JNE_1 = NULL;

uintptr_t DodgeSettings::roll_forward_jmp_ret2 = NULL;

uintptr_t DodgeSettings::roll_forward_jmp_ret3 = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_GetEdgeButton = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_JNE3 = NULL;

uintptr_t DodgeSettings::roll_forward_jmp_ret4 = NULL;
uintptr_t DodgeSettings::roll_forward_Offset_mCheckCanOperate = NULL;
/////////////////////////////////////////////////////////////
bool      DodgeSettings::dodge_battery_mod_enabled;
uintptr_t DodgeSettings::dodge_battery_jmp_ret1;
uintptr_t DodgeSettings::dodge_battery_gpBattle;
uintptr_t DodgeSettings::dodge_battery_PC_INPUT_CHARGE_BAT_ENTER;
uintptr_t DodgeSettings::dodge_battery_mCheckInputCharge;
uintptr_t DodgeSettings::dodge_battery_mGetBtEffect;
uintptr_t DodgeSettings::dodge_battery_SetStatusBatterySE_Flag;
uintptr_t DodgeSettings::dodge_battery_dLeavingExceptBattery;
uintptr_t DodgeSettings::dodge_battery_dAppearKeyGuide;
uintptr_t DodgeSettings::dodge_battery_mPlayCamMotFromCharMot;
uintptr_t DodgeSettings::dodge_battery_SoundPrep;
uintptr_t DodgeSettings::dodge_battery_SoundSetOutput;
uintptr_t DodgeSettings::dodge_battery_SoundPrepPlay;
uintptr_t DodgeSettings::dodge_battery_mUpdateLockOnTarget;
/////////////////////////////////////////////////////////////
bool      DodgeSettings::disable_darkstep_slowmo_mod_enabled = false;
/////////////////////////////////////////////////////////////
bool      DodgeSettings::roll_rotation_mod_enabled = false;
uintptr_t DodgeSettings::roll_rotation_jmp_ret1 = NULL;
uintptr_t DodgeSettings::roll_rotation_mRotate2LockOnTarget = NULL;
uintptr_t DodgeSettings::roll_rotation_GetMotionRunState = NULL;

// clang-format off
naked void roll_forward_detour1() { // roll forward: make lockon+forward input (also used by buffer)
    __asm {
        cmp byte ptr [DodgeSettings::roll_forward_mod_enabled], 0
        je originalcode
        cmp edi,5 // buffered input
        je jnecode
        push eax
        mov eax, [DodgeSettings::roll_forward_Offset_LeftStickUp] // right stick up input
        mov eax, [eax]
        cmp al, 0
        pop eax
        jne jnecode

        originalcode:
        push eax
        mov eax, [DodgeSettings::roll_forward_Offset_LeftStickLeft]
        mov eax, [eax]
        cmp al, 0
        pop eax
        jmp dword ptr [DodgeSettings::roll_forward_jmp_ret1]

        jnecode:
        jmp dword ptr [DodgeSettings::roll_forward_Offset_JNE_1]
    }
}

naked void roll_forward_detour2() { // roll forward: add roll forward animation
    __asm {
        cmp byte ptr [DodgeSettings::roll_forward_mod_enabled], 0
        je originalcode

        cmp edi,5
        je forwardDodge
        push eax
        mov eax, [DodgeSettings::roll_forward_Offset_LeftStickUp]
        mov eax, [eax]
        cmp al, 1
        pop eax
        je forwardDodge
        originalcode:
        mov ecx, 0x1B
        jmp dword ptr [DodgeSettings::roll_forward_jmp_ret2]

        forwardDodge:
        mov ecx, 0x1A
        jmp dword ptr [DodgeSettings::roll_forward_jmp_ret2]
    }
}

naked void roll_forward_detour3() { // roll forward: make lockon+forward (while attacking) input by copying down input
    __asm {
        cmp byte ptr [DodgeSettings::roll_forward_mod_enabled], 0
        je originalcode
        xor edx,edx
        push 0
        lea ecx, [edx+0x0F]
        call dword ptr [DodgeSettings::roll_forward_Offset_GetEdgeButton]
        add esp, 4
        test al, al
        jne jnecode

        originalcode:
        xor edx,edx
        push 0
        lea ecx, [edx+0x0C]
        jmp dword ptr [DodgeSettings::roll_forward_jmp_ret3]

        jnecode:
        jmp dword ptr [DodgeSettings::roll_forward_Offset_JNE3]
    }
}

naked void roll_forward_detour4() { // roll forward: make right stick up and buffer value accepted inputs while attacking
    __asm {
        cmp byte ptr [DodgeSettings::roll_forward_mod_enabled], 0
        je originalcode

        mov ecx,edi
        call dword ptr [DodgeSettings::roll_forward_Offset_mCheckCanOperate]
        test al,al
        je originalcode
        xor edx,edx
        push 00
        lea ecx,[edx+0x0F] // right stick up
        call dword ptr [DodgeSettings::roll_forward_Offset_GetEdgeButton]
        add esp,04
        test al, al
        je originalcode
        mov [edi+0x0000164C],00000005 // new forward dodge
        jmp retcode

        originalcode:
        mov [edi+0x0000164C],00000001 // back dodge buffer
        retcode:
        jmp dword ptr [DodgeSettings::roll_forward_jmp_ret4]
    }
}

naked void detour_dodge_battery() { // dodge battery
    __asm {
        // 
            cmp byte ptr [DodgeSettings::dodge_battery_mod_enabled], 0
            je originalcode
        // 
            //nmh.mHRPc::mFrameProc+133D
            cmp dword ptr [edi+0x18c], ePcMtStpF // 26 front roll
            je originalcode
            cmp dword ptr [edi+0x18c], ePcMtRollB // 27 back roll
            je originalcode
            cmp dword ptr [edi+0x18c], ePcMtRollR // 30 right roll
            je originalcode
            cmp dword ptr [edi+0x18c], ePcMtRollL // 31 left roll
            je originalcode

            push eax
            push ecx
            push edx
            call dword ptr [DodgeSettings::dodge_battery_PC_INPUT_CHARGE_BAT_ENTER]
            cmp al, 1
            jne popcode
            mov ecx, edi
            call dword ptr [DodgeSettings::dodge_battery_mCheckInputCharge]
            cmp al, 1
            jne popcode
            mov ecx, [DodgeSettings::dodge_battery_gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeSettings::dodge_battery_mGetBtEffect]
            push 01
            mov ecx, [eax]
            call dword ptr [DodgeSettings::dodge_battery_SetStatusBatterySE_Flag]
            mov ecx, [DodgeSettings::dodge_battery_gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeSettings::dodge_battery_mGetBtEffect]
            push 01
            mov ecx, [eax]
            call dword ptr [DodgeSettings::dodge_battery_dLeavingExceptBattery]
            mov ecx, [DodgeSettings::dodge_battery_gpBattle]
            mov ecx, [ecx]
            call dword ptr [DodgeSettings::dodge_battery_mGetBtEffect]
            push 01
            push 00
            push 00
            mov ecx, [eax]
            push 00
            push 0x0E
            call dword ptr [DodgeSettings::dodge_battery_dAppearKeyGuide]
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
            call dword ptr [DodgeSettings::dodge_battery_mPlayCamMotFromCharMot]
            mov edx, 0x00000002
            mov ecx, 0x00000275
            call dword ptr [DodgeSettings::dodge_battery_SoundPrep]
            mov esi,eax
            mov edx, 0x00000001
            mov ecx,esi
            call dword ptr [DodgeSettings::dodge_battery_SoundSetOutput]
            mov ecx,esi
            call dword ptr [DodgeSettings::dodge_battery_SoundPrepPlay]
            mov ecx,edi
            mov esi, 0x00000003
            call dword ptr [DodgeSettings::dodge_battery_mUpdateLockOnTarget]
            mov eax, [DodgeSettings::dodge_battery_gpBattle]
            mov eax, [eax]
            mov dword ptr [eax+0x158], 0 // bulletSlow

        popcode:
            pop edx
            pop ecx
            pop eax
        originalcode:
            mov ecx, [edi+0x00000320]
            jmp dword ptr [DodgeSettings::dodge_battery_jmp_ret1]
    }
}

naked void roll_rotation_detour1() { // player in edi
    __asm {
        //
            cmp byte ptr [DodgeSettings::roll_rotation_mod_enabled], 0
            je originalcode

            push eax
            push ecx
            push edx
            mov ecx, edi // player
            call dword ptr [DodgeSettings::roll_rotation_mRotate2LockOnTarget]
            pop edx
            pop ecx
            pop eax

        originalcode:
            call dword ptr [DodgeSettings::roll_rotation_GetMotionRunState]
            jmp dword ptr [DodgeSettings::roll_rotation_jmp_ret1]
    }
}
 // clang-format on

void DodgeSettings::toggle_disable_slowmo_darkstep(bool enable) {
    if (enable) {
        install_patch_offset(0x3DB3CE, disable_slowmo_darkstep_patch, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10); // nop 10
    }
    else {
        install_patch_offset(0x3DB3CE, disable_slowmo_darkstep_patch, "\x68\xE0\x2E\x00\x00\xE8\x78\x74\xFD\xFF", 10); // push 00002EE0, call nmh.mHRBattle::mSetSlowMotionTick
    }
}

std::optional<std::string> DodgeSettings::on_initialize() {
    // roll forward
    DodgeSettings::roll_forward_Offset_JNE_1 = g_framework->get_module().as<uintptr_t>() + 0x3D2628;
    DodgeSettings::roll_forward_Offset_LeftStickUp = g_framework->get_module().as<uintptr_t>() + 0x84B91A;
    DodgeSettings::roll_forward_Offset_LeftStickLeft = g_framework->get_module().as<uintptr_t>() + 0x84B8F6;
    DodgeSettings::roll_forward_Offset_mCheckCanOperate = g_framework->get_module().as<uintptr_t>() + 0x3DD970;
    if (!install_hook_offset(0x3D25F2, roll_forward_hook1, &roll_forward_detour1, &DodgeSettings::roll_forward_jmp_ret1, 7)) {
        spdlog::error("Failed to init DodgeSettings mod\n");
        return "Failed to init DodgeSettings mod";
    }
    if (!install_hook_offset(0x3D2694, roll_forward_hook2, &roll_forward_detour2, &DodgeSettings::roll_forward_jmp_ret2, 5)) {
        spdlog::error("Failed to init DodgeSettings mod\n");
        return "Failed to init DodgeSettings mod";
    }
    DodgeSettings::roll_forward_Offset_GetEdgeButton = g_framework->get_module().as<uintptr_t>() + 0x5F8550;
    DodgeSettings::roll_forward_Offset_JNE3 = g_framework->get_module().as<uintptr_t>() + 0x3D00F1;
    if (!install_hook_offset(0x3D009A, roll_forward_hook3, &roll_forward_detour3, &DodgeSettings::roll_forward_jmp_ret3, 7)) {
        spdlog::error("Failed to init DodgeSettings mod\n");
        return "Failed to init DodgeSettings mod";
    }
    if (!install_hook_offset(0x3D0156, roll_forward_hook4, &roll_forward_detour4, &DodgeSettings::roll_forward_jmp_ret4, 10)) {
        spdlog::error("Failed to init DodgeSettings mod\n");
        return "Failed to init DodgeSettings mod";
    }

    // dodge battery
    DodgeSettings::dodge_battery_gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    DodgeSettings::dodge_battery_PC_INPUT_CHARGE_BAT_ENTER = g_framework->get_module().as<uintptr_t>() + 0x3DDBB0;
    DodgeSettings::dodge_battery_mCheckInputCharge = g_framework->get_module().as<uintptr_t>() + 0x3C38A0;
    DodgeSettings::dodge_battery_mGetBtEffect = g_framework->get_module().as<uintptr_t>() + 0x9D670;
    DodgeSettings::dodge_battery_SetStatusBatterySE_Flag = g_framework->get_module().as<uintptr_t>() + 0x9D4D0;
    DodgeSettings::dodge_battery_dLeavingExceptBattery = g_framework->get_module().as<uintptr_t>() + 0x40E010;
    DodgeSettings::dodge_battery_dAppearKeyGuide = g_framework->get_module().as<uintptr_t>() + 0x40EC60;
    DodgeSettings::dodge_battery_mPlayCamMotFromCharMot = g_framework->get_module().as<uintptr_t>() + 0x402340;
    DodgeSettings::dodge_battery_SoundPrep = g_framework->get_module().as<uintptr_t>() + 0x5F3500;
    DodgeSettings::dodge_battery_SoundSetOutput = g_framework->get_module().as<uintptr_t>() + 0x5F3450;
    DodgeSettings::dodge_battery_SoundPrepPlay = g_framework->get_module().as<uintptr_t>() + 0x5F3500;
    DodgeSettings::dodge_battery_mUpdateLockOnTarget = g_framework->get_module().as<uintptr_t>() + 0x3C4230;
    if (!install_hook_offset(0x3DB2C6, m_hook_dodge_battery1, &detour_dodge_battery, &DodgeSettings::dodge_battery_jmp_ret1, 6)) {
        spdlog::error("Failed to init DodgeBattery mod\n");
        return "Failed to init DodgeBattery mod";
    }

    // roll rotation
    DodgeSettings::roll_rotation_mRotate2LockOnTarget = g_framework->get_module().as<uintptr_t>() + 0x3C4C70;
    DodgeSettings::roll_rotation_GetMotionRunState = g_framework->get_module().as<uintptr_t>() + 0x36BBF0;
    if (!install_hook_offset(0x3DB2CE, m_hook_roll_rotation1, &roll_rotation_detour1, &DodgeSettings::roll_rotation_jmp_ret1, 5)) {
        spdlog::error("Failed to init DodgeSettings mod\n");
        return "Failed to init DodgeSettings mod";
    }

    return Mod::on_initialize();
}

void DodgeSettings::on_draw_ui() {
    ImGui::Checkbox("Cancel Darkstep into Recharge", &dodge_battery_mod_enabled);
    help_marker("Allows you to cancel Darkstep into the katana recharge. Take advantage of the enemy's confusion and use the opportunity to recharge your battery.");

    ImGui::Checkbox("Roll Forward", &roll_forward_mod_enabled);
    help_marker("Restores the unused forward roll and works exactly as it does in NMH2. Bound to Lock On + Forward (Right Stick)");

    if (ImGui::Checkbox("NMH2 Darkstep", &disable_darkstep_slowmo_mod_enabled)) {
        toggle_disable_slowmo_darkstep(disable_darkstep_slowmo_mod_enabled);
    }
    help_marker("Functionally alters Darksteps to function like it does in NMH2. When performing a Darkstep, Travis will retain his normal speed instead of slowing down.");
    
    ImGui::Checkbox("Roll Rotation", &roll_rotation_mod_enabled);
    help_marker("Allows rolls to orbit around enemies. This replicates how rolling functions in NMH2 and provides more control for staying on target. Rolls will cease to orbit when releasing lock on.");
}

// during load
void DodgeSettings::on_config_load(const utility::Config &cfg) {
    dodge_battery_mod_enabled = cfg.get<bool>("chargeWhileDodging").value_or(false);

    roll_forward_mod_enabled = cfg.get<bool>("roll_forward").value_or(false);

    disable_darkstep_slowmo_mod_enabled = cfg.get<bool>("disable_darkstep_slowmo").value_or(false);
    toggle_disable_slowmo_darkstep(disable_darkstep_slowmo_mod_enabled);

    roll_rotation_mod_enabled = cfg.get<bool>("roll_rotation").value_or(false);
}

// during save
void DodgeSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("chargeWhileDodging", dodge_battery_mod_enabled);

    cfg.set<bool>("roll_forward", roll_forward_mod_enabled);

    cfg.set<bool>("disable_darkstep_slowmo", disable_darkstep_slowmo_mod_enabled);

    cfg.set<bool>("roll_rotation", roll_rotation_mod_enabled);
}

// do something every frame
//void DodgeSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DodgeSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
