#include "HitstopSettings.hpp"
#if 1
bool HitstopSettings::mod_enabled = false;
uintptr_t HitstopSettings::jmp_ret1 = NULL;
uintptr_t HitstopSettings::gpBattle = NULL;

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
        ja originalcode

        nohitstop:
        mov dword ptr  [edi+0x000029AC],3
        jmp qword ptr [HitstopSettings::jmp_ret1]

        originalcode:
            mov dword ptr [edi+0x000029AC],8
            jmp qword ptr [HitstopSettings::jmp_ret1]
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
    ImGui::Checkbox("Less Hitstop", &mod_enabled);
}

// during load
void HitstopSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("less_hitstop").value_or(false);
}
// during save
void HitstopSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("less_hitstop", mod_enabled);
}

// do something every frame
//void HitstopSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void HitstopSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
