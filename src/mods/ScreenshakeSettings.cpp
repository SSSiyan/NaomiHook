#include "ScreenshakeSettings.hpp"
#if 1
bool ScreenshakeSettings::mod_enabled = false;
uintptr_t ScreenshakeSettings::jmp_ret1 = NULL;
uintptr_t ScreenshakeSettings::Offset_84BA18 = NULL;
int ScreenshakeSettings::customBasicScreenshakeAmount = 6;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        push eax
        // mov dword ptr [ScreenshakeSettings::Offset_84BA18], 6 // probably pad rumble
        mov eax,[ScreenshakeSettings::Offset_84BA18]
        mov [eax],6
        pop eax
        cmp byte ptr [ScreenshakeSettings::mod_enabled], 0
        je originalcode

        push dword ptr [ScreenshakeSettings::customBasicScreenshakeAmount]
        jmp dword ptr [ScreenshakeSettings::jmp_ret1]

        originalcode:
        push 6
        jmp dword ptr [ScreenshakeSettings::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> ScreenshakeSettings::on_initialize() {
    ScreenshakeSettings::Offset_84BA18 = g_framework->get_module().as<uintptr_t>() + 0x84BA18;
    if (!install_hook_offset(0x3C03BC, m_hook1, &detour1, &ScreenshakeSettings::jmp_ret1, 12)) {
        spdlog::error("Failed to init ScreenshakeSettings mod\n");
        return "Failed to init ScreenshakeSettings mod";
    }

    return Mod::on_initialize();
}

void ScreenshakeSettings::on_draw_ui() {
    ImGui::Checkbox("Custom Screenshake On Normal Attacks", &mod_enabled);
    if (mod_enabled) {
        ImGui::Text("Custom Screenshake Amount");
        ImGui::SliderInt("##CustomScreenshakeAmountSliderInt", &customBasicScreenshakeAmount, 0, 20);
        help_marker("Default 6");
    }
}

// during load
void ScreenshakeSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("custom_screenshake").value_or(false);
    customBasicScreenshakeAmount = cfg.get<int>("customBasicScreenshakeAmount").value_or(6);
}
// during save
void ScreenshakeSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_screenshake", mod_enabled);
    cfg.set<int>("customBasicScreenshakeAmount", customBasicScreenshakeAmount);
}

// do something every frame
//void ScreenshakeSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ScreenshakeSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
