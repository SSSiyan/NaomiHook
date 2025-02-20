#include "KillObscuringTreasureChests.hpp"
#if 1
bool KillObscuringTreasureChests::mod_enabled = false;

void KillObscuringTreasureChests::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x4E5470, patch1, "\x90\x90\x90\x90\x90", 5); // nop 5
    }
    else {
        install_patch_offset(0x4E5470, patch1, "\xE8\xEB\x00\x00\x00", 5); // call nmh.exe+4E5470
    }
}

std::optional<std::string> KillObscuringTreasureChests::on_initialize() {
    return Mod::on_initialize();
}

void KillObscuringTreasureChests::on_draw_ui() {
    if (ImGui::Checkbox("Kill Obscuring Treasure Chests", &mod_enabled)) {
        toggle(mod_enabled);
    }
}

// during load
void KillObscuringTreasureChests::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("kill_obscuring_treasure_chests").value_or(false);
    toggle(mod_enabled);
}
// during save
void KillObscuringTreasureChests::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("kill_obscuring_treasure_chests", mod_enabled);
}

// do something every frame
//void KillObscuringTreasureChests::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void KillObscuringTreasureChests::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
