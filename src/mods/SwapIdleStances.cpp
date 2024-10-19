#include "SwapIdleStances.hpp"
#if 1
bool SwapIdleStances::mod_enabled = false;

void SwapIdleStances::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3D7D4A, patch0, "\x75", 1); // jne nmh.exe+3D7D5B
        install_patch_offset(0x3D7D4F, patch1, "\x75", 1); // jne nmh.exe+3D7D56
    }
    else {
        install_patch_offset(0x3D7D4A, patch0, "\x74", 1); // je nmh.exe+3D7D5B
        install_patch_offset(0x3D7D4F, patch1, "\x74", 1); // je nmh.exe+3D7D56
    }
}

std::optional<std::string> SwapIdleStances::on_initialize() {
    return Mod::on_initialize();
}

void SwapIdleStances::on_draw_ui() {
    if (ImGui::Checkbox("Swap Idle Stances", &mod_enabled)) {
        toggle(mod_enabled);
    }
}

// during load
void SwapIdleStances::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("swap_idle_stances").value_or(false);
    toggle(mod_enabled);
}

// during save
void SwapIdleStances::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("swap_idle_stances", mod_enabled);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
