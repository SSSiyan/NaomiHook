#include "DisableCrashDumps.hpp"
#if 1
bool DisableCrashDumps::mod_enabled = false;

// clang-format off
/*naked void detour() {
    __asm {
        jmp dword ptr [jmp_ret]
    }
}*/

 // clang-format on

void DisableCrashDumps::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x1F4A38, patch, "\x59\xED\x02\x00\x00\x90", 6); // jmp nmh.exe+1F4C9A
    }
    else {
        install_patch_offset(0x1F4A38, patch, "\x0F\x85\x5C\x02\x00\x00", 6); // jne nmh.exe+1F4C9A
    }
}

std::optional<std::string> DisableCrashDumps::on_initialize() {
    return Mod::on_initialize();
}

void DisableCrashDumps::on_draw_ui() {
    if (ImGui::Checkbox("Disable Crash Dumps", &mod_enabled)) {
        toggle(mod_enabled);
    }
}

// during load
void DisableCrashDumps::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("disable_crash_dumps").value_or(true);
    toggle(mod_enabled);
}
// during save
void DisableCrashDumps::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("disable_crash_dumps", mod_enabled);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
