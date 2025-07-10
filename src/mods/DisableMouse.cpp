#include "DisableMouse.hpp"
#if 1
bool DisableMouse::mod_enabled = false;
uintptr_t DisableMouse::jmp_ret1 = NULL;
bool DisableMouse::any_gui_open = false;

bool DisableMouse::dontHideMouseWhenHookIsOpen = false;
uintptr_t DisableMouse::dont_hide_mouse_jmp_ret = NULL;

// clang-format off
naked void detour1() {
    __asm {
        cmp byte ptr [DisableMouse::mod_enabled], 0
        je originalcode

        cmp byte ptr [DisableMouse::any_gui_open], 0
        je originalcode

        cmp byte ptr [esi+0x00000090], 01
        jmp dword ptr [DisableMouse::jmp_ret1]

    originalcode:
        cmp byte ptr [esi+0x00000090], 00
        jmp dword ptr [DisableMouse::jmp_ret1]
    }
}

naked void dont_hide_mouse_detour() {
    __asm {
    // originalcode
        cmp byte ptr [DisableMouse::dontHideMouseWhenHookIsOpen], 0
        je originalcode

        cmp byte ptr [DisableMouse::any_gui_open], 1
        je contcode

    originalcode:
        mov byte ptr [ebx], 01
    contcode:
        cmp byte ptr [edi+0x00000288], 00
        jmp dword ptr [DisableMouse::dont_hide_mouse_jmp_ret]
    }
}
 // clang-format on

void DisableMouse::on_draw_ui() {
    ImGui::Checkbox("Disable Mouse While GUI Is Open", &mod_enabled);

    ImGui::Checkbox("Don't Hide the Cursor While NaomiHook Is Open", &dontHideMouseWhenHookIsOpen);
}

std::optional<std::string> DisableMouse::on_initialize() {
    if (!install_hook_offset(0x6556E3, m_hook1, &detour1, &DisableMouse::jmp_ret1, 7)) {
        spdlog::error("Failed to init DisableMouse mod\n");
        return "Failed to init DisableMouse mod";
    }

    if (!install_hook_offset(0x1C09A8, dont_hide_mouse_hook1, &dont_hide_mouse_detour, &DisableMouse::dont_hide_mouse_jmp_ret, 10)) {
        spdlog::error("Failed to init dont_hide_mouse mod\n");
        return "Failed to init dont_hide_mouse mod";
    }
    return Mod::on_initialize();
}

// during load
void DisableMouse::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("disableMouse").value_or(true);
    dontHideMouseWhenHookIsOpen = cfg.get<bool>("dontHideMouseWhenHookIsOpen").value_or(true);
}

// during save
void DisableMouse::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("disableMouse", mod_enabled);
    cfg.set<bool>("dontHideMouseWhenHookIsOpen", dontHideMouseWhenHookIsOpen);
}

// do something every frame
//void DisableMouse::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableMouse::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
