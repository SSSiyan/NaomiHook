#include "AcceptPadInputsTabbedOut.hpp"
#if 1
bool AcceptPadInputsTabbedOut::mod_enabled = false;

// clang-format off
/*naked void detour() {
    __asm {
        jmp qword ptr [jmp_ret]
    }
}*/

 // clang-format on

void AcceptPadInputsTabbedOut::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x20DCE9, patch, "\xE9\xA4\x00\x00\x00\x90", 6); // jmp nmh.exe+20DD92 nop
    }
    else {
        install_patch_offset(0x20DCE9, patch, "\x0F\x85\xA3\x00\x00\x00", 6); // jne nmh.exe+20DD92
    }
}

std::optional<std::string> AcceptPadInputsTabbedOut::on_initialize() {
    return Mod::on_initialize();
}

void AcceptPadInputsTabbedOut::on_draw_ui() {
    if (ImGui::Checkbox("Accept Pad Inputs Tabbed Out", &mod_enabled)) {
        toggle(mod_enabled);
    }
}

// during load
void AcceptPadInputsTabbedOut::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("accept_pad_inputs_tabbed_out").value_or(true);
    toggle(mod_enabled);
}
// during save
void AcceptPadInputsTabbedOut::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("accept_pad_inputs_tabbed_out", mod_enabled);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
