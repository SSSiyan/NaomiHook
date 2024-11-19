#include "HitstopSettings.hpp"
#if 1
bool HitstopSettings::mod_enabled = false;
uintptr_t HitstopSettings::jmp_ret1 = NULL;
uintptr_t HitstopSettings::gpBattle = NULL;
int HitstopSettings::customBasicHitstopAmount = 0;
int HitstopSettings::customDarkHitstopAmount = 0;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        cmp byte ptr [HitstopSettings::mod_enabled], 0
        je originalcode

        push eax
        mov eax, [HitstopSettings::gpBattle]
        mov eax, [eax]
        cmp dword ptr [eax+0x15c], 0 // bulletSlow
        pop eax
        ja darkhitstop

        // newhitstop:
        push eax
        mov eax, [HitstopSettings::customBasicHitstopAmount]
        mov dword ptr  [edi+0x000029AC],eax
        pop eax
        jmp dword ptr [HitstopSettings::jmp_ret1]

        darkhitstop:
        push eax
        mov eax, [HitstopSettings::customDarkHitstopAmount]
        mov dword ptr  [edi+0x000029AC],eax
        pop eax
        jmp dword ptr [HitstopSettings::jmp_ret1]

        originalcode:
        mov dword ptr [edi+0x000029AC],8
        jmp dword ptr [HitstopSettings::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> HitstopSettings::on_initialize() {
    HitstopSettings::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    if (!install_hook_offset(0x3CB8C6, m_hook1, &detour1, &HitstopSettings::jmp_ret1, 10)) {
        spdlog::error("Failed to init HitstopSettings mod\n");
        return "Failed to init HitstopSettings mod";
    }

    return Mod::on_initialize();
}

void HitstopSettings::on_draw_ui() {
    ImGui::Checkbox("Custom Hitstop On Normal Attacks", &mod_enabled);
    help_marker("Adjust the duration of hitstop on attacks.");
    if (mod_enabled) {
        ImGui::Text("Custom Hitstop Amount");
        ImGui::SliderInt("##customBasicHitstopAmountSliderInt", &customBasicHitstopAmount, 0, 20);
        help_marker("Default 8");
        ImGui::Text("Custom Darkstep Hitstop Amount");
        ImGui::SliderInt("##customDarkHitstopAmountSliderInt", &customDarkHitstopAmount, 0, 20);
        help_marker("Default 8");
    }
}

// during load
void HitstopSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("custom_hitstop").value_or(false);
    customBasicHitstopAmount = cfg.get<int>("customBasicHitstopAmount").value_or(8);
    customDarkHitstopAmount = cfg.get<int>("customDarkHitstopAmount").value_or(8);
}
// during save
void HitstopSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_hitstop", mod_enabled);
    cfg.set<int>("customBasicHitstopAmount", customBasicHitstopAmount);
    cfg.set<int>("customDarkHitstopAmount", customDarkHitstopAmount);
}

// do something every frame
//void HitstopSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void HitstopSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
