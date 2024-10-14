#include "Invincibility.hpp"
#if 1
bool Invincibility::mod_enabled = false;
uintptr_t Invincibility::jmp_ret1 = NULL;
uintptr_t Invincibility::jmp_ja1 = NULL;
uintptr_t Invincibility::CBgCtrl = NULL;

// clang-format off
naked void detour1() { // mSetDamage+F9 // damage receiver in esi
    __asm {
        //
            cmp byte ptr [Invincibility::mod_enabled], 0
            je originalcode

            push eax
            mov eax, [Invincibility::CBgCtrl]
            mov eax, [eax]
            cmp dword ptr [eax+0xaa8], 0 // is screen m_DarkSideModeColor?
            pop eax
            ja jmp_ja

        originalcode:
            cmp byte ptr [esi+0x000029A7], 01 // mStageChangeMuteki
            jmp dword ptr [Invincibility::jmp_ret1]

        jmp_ja:
            jmp dword ptr [Invincibility::jmp_ja1]
    }
}
 
 // clang-format on

std::optional<std::string> Invincibility::on_initialize() {
    Invincibility::CBgCtrl = g_framework->get_module().as<uintptr_t>() + 0x8446F0;
    Invincibility::jmp_ja1 = g_framework->get_module().as<uintptr_t>() + 0x3D68FB;
    if (!install_hook_offset(0x3D5869, m_hook1, &detour1, &Invincibility::jmp_ret1, 7)) {
        spdlog::error("Failed to init Invincibility mod\n");
        return "Failed to init Invincibility mod";
    }

    return Mod::on_initialize();
}

void Invincibility::on_draw_ui() {
    ImGui::Checkbox("Darkstep Invincibility", &mod_enabled);
}

// during load
void Invincibility::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("darkstep_invincibility").value_or(false);
}
// during save
void Invincibility::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("darkstep_invincibility", mod_enabled);
}

// do something every frame
//void Invincibility::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void Invincibility::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
