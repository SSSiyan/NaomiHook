#include "DisableMouse.hpp"
#if 1
bool DisableMouse::mod_enabled = false;
uintptr_t DisableMouse::jmp_ret1 = NULL;
bool DisableMouse::gui_open = false;

// clang-format off
naked void detour1() {
    __asm {
        cmp byte ptr [DisableMouse::mod_enabled], 0
        je originalcode

        cmp byte ptr [DisableMouse::gui_open], 0
        je originalcode

        cmp byte ptr [esi+0x00000090], 01
        jmp dword ptr [DisableMouse::jmp_ret1]

    originalcode:
        cmp byte ptr [esi+0x00000090], 00
        jmp dword ptr [DisableMouse::jmp_ret1]
    }
}
 // clang-format on

std::optional<std::string> DisableMouse::on_initialize() {
    if (!install_hook_offset(0x6556E3, m_hook1, &detour1, &DisableMouse::jmp_ret1, 7)) {
        spdlog::error("Failed to init DisableMouse mod\n");
        return "Failed to init DisableMouse mod";
    }
    return Mod::on_initialize();
}

void DisableMouse::on_draw_ui() {
    ImGui::Checkbox("Disable Mouse While GUI is open", &mod_enabled);
}

// during load
void DisableMouse::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("disableMouse").value_or(true);
}

// during save
void DisableMouse::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("disableMouse", mod_enabled);
}

// do something every frame
//void DisableMouse::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableMouse::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
