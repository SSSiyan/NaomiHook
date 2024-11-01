#include "ReprisalSwap.hpp"
#if 1
bool ReprisalSwap::mod_enabled = false;
uintptr_t ReprisalSwap::jmp_ret1 = NULL;
uintptr_t ReprisalSwap::gpPad = NULL;

// clang-format off
naked void detour1() {
    __asm {
        cmp byte ptr [ReprisalSwap::mod_enabled], 0
        je originalcode

        push eax
        mov eax, [ReprisalSwap::gpPad]
        cmp byte ptr [eax+0x1CC], 1 // high attack // From nmh.PC_INPUT_ATTACK+99
        pop eax
        jne originalcode
        mov eax, 169 // new motNum
    originalcode:
        push eax // default eax
        call esi
        movss xmm3, [edi+0x40]
        jmp dword ptr [ReprisalSwap::jmp_ret1]
    }
}
 // clang-format on

std::optional<std::string> ReprisalSwap::on_initialize() {
    gpPad = g_framework->get_module().as<uintptr_t>() + 0x849D10; 
    if (!install_hook_offset(0x3CE22E, m_hook1, &detour1, &ReprisalSwap::jmp_ret1, 8)) {
        spdlog::error("Failed to init ReprisalSwap mod\n");
        return "Failed to init ReprisalSwap mod";
    }
    return Mod::on_initialize();
}

void ReprisalSwap::on_draw_ui() {
    ImGui::Checkbox("Reprisal Swap", &mod_enabled);
}

// during load
void ReprisalSwap::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("reprisal_swap").value_or(false);
}
// during save
void ReprisalSwap::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("reprisal_swap", mod_enabled);
}

// do something every frame
//void ReprisalSwap::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ReprisalSwap::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
