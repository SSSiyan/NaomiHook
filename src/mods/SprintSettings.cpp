#include "SprintSettings.hpp"
#if 1
bool SprintSettings::battleSprint = false;

uintptr_t SprintSettings::jmp_ret1 = NULL;
uintptr_t SprintSettings::l3Addr = NULL;
uintptr_t SprintSettings::jneAddr = NULL;
uintptr_t SprintSettings::closeQuartersAddr = NULL;

uintptr_t SprintSettings::jmp_ret2 = NULL;
float SprintSettings::sprintSpeed = 2.0f;
float SprintSettings::battleSprintSpeed = 1.35f;
bool SprintSettings::sprintFlag = false;
uintptr_t detour2_jmp_je = NULL;

uintptr_t SprintSettings::jmp_ret3 = NULL;

uintptr_t SprintSettings::jmp_ret4 = NULL;
uintptr_t SprintSettings::gpBattle = NULL;

// clang-format off
naked void detour1() { // Enable sprint in combat // sprint's mode check // player in esi
    __asm {
        //
            cmp byte ptr [SprintSettings::battleSprint], 0
            je originalcode
        // 
            cmp dword ptr [esi+0x2990], 3 // mode
            jne originalCode
        //
            push edx
            push ecx
            push eax
            mov edx, [esi+0x2b60] // lock on target in edx
            test edx, edx // are we locking on?
            je notLockedOn
            cmp dword ptr [edx+0x18], 0 // check target isn't lockOnDummy
            je popCode
            mov eax, [SprintSettings::closeQuartersAddr]
            movss xmm1, [eax] // darkstep range
            comiss xmm1, [edx+0x3f0] // compare darkstep to current locked on enemy distance
            jae popCode // if inside darkstep range, disable sprint
            // cmp dword ptr [esi+0x18c], ePcMtBtDefRunR // 24 lock on strafe right
            // je canSprint
            // cmp dword ptr [esi+0x18c], ePcMtBtDefRunL // 25 lock on strafe left
            // je canSprint
        notLockedOn:
            cmp byte ptr [esi+0x1770], 1 // is telephone Ramble
            je popCode
            cmp dword ptr [esi+0x18c], ePcMtBt01Rn // 221 berry run
            je canSprint
            cmp dword ptr [esi+0x18c], ePcMtBt02Rn // 269 tsubaki mk3 run
            je canSprint
            cmp dword ptr [esi+0x18c], ePcMtBt03Rn // 317 tsubaki mk1 run
            je canSprint
            cmp dword ptr [esi+0x18c], ePcMtBt04Rn // 364 tsubaki mk2 run
            je canSprint
            jmp popCode
        canSprint:
            pop eax
            pop ecx
            pop eax
            cmp byte ptr [SprintSettings::sprintFlag], 1 // are we already sprinting?
            je jneAddr
            push eax
            mov eax, [SprintSettings::l3Addr] // if not, see if sprint was pressed
            test byte ptr [eax], 1
            pop eax
            jne jneAddr
            jmp originalCode
        popCode:
            pop eax
            pop ecx
            pop edx
        originalCode:
            mov byte ptr [SprintSettings::sprintFlag], 0 // if any conditionals fail, set sprint flag false
            cmp dword ptr [esi+0x00002990], 03
            jmp dword ptr [SprintSettings::jmp_ret1]

        jneAddr: // sprint
            mov byte ptr [SprintSettings::sprintFlag], 1 // if under specific conditions, set sprint flag true
            jmp dword ptr [SprintSettings::jneAddr]
    }
}

naked void detour2() { // Set sprint speeds
    __asm {
        //
            cmp byte ptr [SprintSettings::battleSprint], 0
            je originalcode
        // 
            cmp dword ptr [esi+0x2990], 3 // mode
            jne originalcode
        // cheatcode
            // cmp dword ptr [esi+0x0000018C], 0x0E
            lea edi, [esi+0x00000090]
            mov dword ptr [edi], 0x3faccccd // 1.35f
            jmp jecode

        originalcode:
            cmp dword ptr [esi+0x0000018C], 0x0E
            lea edi, [esi+0x00000090]
            mov dword ptr [edi], 0x40000000 // 2.0f
            je jecode
        retcode:
            jmp dword ptr [SprintSettings::jmp_ret2]
        jecode:
            jmp dword ptr [detour2_jmp_je]
    }
}

naked void detour3() { // Set sprint vfx
    __asm {
        //
            cmp byte ptr [SprintSettings::battleSprint], 0
            je originalcode
        // 
            cmp dword ptr [esi+0x2990], 3
            jne originalCode
        //
            push 0x3c // lifetime
            push ecx
            mov dword ptr [esp], 0x3F7AE148 // 0.98f
            lea eax, [esp+0x3C]
            push 0x02 // default smokes per second
            push 0x03 // default idk1
            sub esp, 0x08
            //lea edx,[esi+50] // navel
            lea edx, [esi+0x38]
            xor ecx,ecx
            mov dword ptr [esp+0x04], 0x40000000 // 2.0f
            mov dword ptr [esp], 0x3dcccccd // 0.1f
            mov dword ptr [esi+0x1298], 0x3f800000 // 1.0f
            jmp returnCode

        originalCode:
            push 0x3c // lifetime
            push ecx
            mov dword ptr [esp], 0x3F7AE148 // 0.98f
            lea eax, [esp+0x3C]
            push 0x08 // default smokes per second
            push 0x05 // default idk1
            sub esp, 0x08
            //lea edx,[esi+50] // navel
            lea edx, [esi+0x38]
            xor ecx, ecx
            mov dword ptr [esp+0x04], 0x41200000 // default size // 10.0f
            mov dword ptr [esp], 0x3F000000 // default idk2 // 0.5f
        returnCode:
            jmp dword ptr [SprintSettings::jmp_ret3]
    }
}

naked void detour4() { // Set sprint flag false after any action
    __asm {
        //
            cmp byte ptr [SprintSettings::battleSprint], 0
            je originalcode
        // 
            push eax
            mov eax, [SprintSettings::gpBattle]
            mov eax, [eax+0x164]
            cmp eax, esi
            pop eax
            jne originalCode

            mov byte ptr [SprintSettings::sprintFlag], 0

        originalCode:
            mov [esi+0x0000018C], ebx
            jmp dword ptr [SprintSettings::jmp_ret4]
    }
}

 // clang-format on

std::optional<std::string> SprintSettings::on_initialize() {
    SprintSettings::l3Addr = g_framework->get_module().as<uintptr_t>() + 0x849D16;
    SprintSettings::jneAddr = g_framework->get_module().as<uintptr_t>() + 0x3D2BEB;
    SprintSettings::closeQuartersAddr = g_framework->get_module().as<uintptr_t>() + 0x7A4414;
    if (!install_hook_offset(0x3D287C, m_hook1, &detour1, &SprintSettings::jmp_ret1, 7)) {
        spdlog::error("Failed to init SprintSettings mod 1\n");
        return "Failed to init SprintSettings mod 1";
    }

    detour2_jmp_je = g_framework->get_module().as<uintptr_t>() + 0x3D2D48;
    if (!install_hook_offset(0x3D2C91, m_hook2, &detour2, &SprintSettings::jmp_ret2, 25)) {
        spdlog::error("Failed to init SprintSettings mod 2\n");
        return "Failed to init SprintSettings mod 2";
    }

    if (!install_hook_offset(0x3D2DC3, m_hook3, &detour3, &SprintSettings::jmp_ret3, 41)) {
        spdlog::error("Failed to init SprintSettings mod 3\n");
        return "Failed to init SprintSettings mod 3";
    }

    SprintSettings::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    if (!install_hook_offset(0x402956, m_hook4, &detour4, &SprintSettings::jmp_ret4, 6)) {
        spdlog::error("Failed to init SprintSettings mod 4\n");
        return "Failed to init SprintSettings mod 4";
    }

    return Mod::on_initialize();
} 

void SprintSettings::on_draw_ui() {
    ImGui::Checkbox("Combat Sprint", &battleSprint);
    /*if (battleSprint) {
        ImGui::Indent();
        ImGui::Text("Sprint Speed");
        ImGui::SliderFloat("##SprintSpeedSliderFloat", &sprintSpeed, 0, 5, "%.2f");
        help_marker("Default 2.0f");
        ImGui::Text("Battle Sprint Speed");
        ImGui::SliderFloat("##battleSprintSpeedSliderFloat", &battleSprintSpeed, 0, 5, "%.2f");
        help_marker("Default 1.35f");
        ImGui::Unindent();
    }*/
}

// during load
void SprintSettings::on_config_load(const utility::Config &cfg) {
    battleSprint = cfg.get<bool>("battleSprint").value_or(false);
    //sprintSpeed = cfg.get<float>("sprintSpeed").value_or(2.0f);
    //battleSprintSpeed = cfg.get<float>("battleSprintSpeed").value_or(1.35f);
}
// during save
void SprintSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("battleSprint", battleSprint);
    //cfg.set<float>("sprintSpeed", sprintSpeed);
    //cfg.set<float>("battleSprintSpeed", battleSprintSpeed);
}

// do something every frame
//void SprintSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void SprintSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
