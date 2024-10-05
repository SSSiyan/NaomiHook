#include "SprintSettings.hpp"
#if 1
bool SprintSettings::battle_sprint = false;
uintptr_t SprintSettings::jmp_ret1 = NULL;
uintptr_t SprintSettings::l3Addr = NULL;
int SprintSettings::battleSprintSpeed = 8;
uintptr_t SprintSettings::jneAddr = NULL;
uintptr_t SprintSettings::closeQuartersAddr = NULL;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        //
            cmp byte ptr [SprintSettings::battle_sprint], 0
            je originalcode
        // 
            push edx
            push ecx
            push eax
            mov edx,[esi+0x2b60] // lock on target
            test edx,edx // valid?
            je notLockingOn
            cmp [edx+0x18],0 // check target isn't lockOnDummy
            je noTarget
            mov eax, [SprintSettings::closeQuartersAddr]
            movss xmm1, [eax] // darkstep range
            comiss xmm1, [edx+0x3f0] // compare darkstep to current locked on enemy distance
        notLockingOn:
            pop eax
            pop ecx
            pop eax
            ja originalCode // if inside darkstep range, disable sprint
            push eax
            mov eax, [SprintSettings::l3Addr]
            test byte ptr [eax], 1
            pop eax
            jne jneAddr
            jmp originalCode
        noTarget:
            pop eax
            pop ecx
            pop edx
        originalCode:
            cmp dword ptr [esi+0x00002990],03
            jmp dword ptr [SprintSettings::jmp_ret1]

        jneAddr:
            jmp dword ptr [SprintSettings::jneAddr]
    }
}
 
 // clang-format on

std::optional<std::string> SprintSettings::on_initialize() {
    SprintSettings::l3Addr = g_framework->get_module().as<uintptr_t>() + 0x849D16;
    SprintSettings::jneAddr = g_framework->get_module().as<uintptr_t>() + 0x3D2BEB;
    SprintSettings::closeQuartersAddr = g_framework->get_module().as<uintptr_t>() + 0x7A4414;
    if (!install_hook_offset(0x3D287C, m_hook1, &detour1, &SprintSettings::jmp_ret1, 7)) {
        spdlog::error("Failed to init SprintSettings mod\n");
        return "Failed to init SprintSettings mod";
    }

    return Mod::on_initialize();
}

void SprintSettings::on_draw_ui() {
    ImGui::Checkbox("Battle Sprint", &battle_sprint);
    // if (battle_sprint) {
    //     ImGui::Text("Battle Sprint Speed");
    //     ImGui::SliderInt("##battleSprintSpeed", &battleSprintSpeed, 0, 20);
    //     help_marker("Default ?");
    // }
}

// during load
void SprintSettings::on_config_load(const utility::Config &cfg) {
    battle_sprint = cfg.get<bool>("battle_sprint").value_or(false);
    // battleSprintSpeed = cfg.get<int>("battleSprintSpeed").value_or(8);
}
// during save
void SprintSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("battle_sprint", battle_sprint);
    // cfg.set<int>("battleSprintSpeed", battleSprintSpeed);
}

// do something every frame
//void SprintSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void SprintSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
