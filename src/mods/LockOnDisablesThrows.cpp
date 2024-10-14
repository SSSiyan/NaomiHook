#include "LockOnDisablesThrows.hpp"
#if 1
bool LockOnDisablesThrows::mod_enabled = false;
uintptr_t LockOnDisablesThrows::jmp_ret1 = NULL;
uintptr_t LockOnDisablesThrows::jmp_jne1 = NULL;

uintptr_t LockOnDisablesThrows::jmp_ret2 = NULL;
uintptr_t LockOnDisablesThrows::jmp_jne2 = NULL;
float LockOnDisablesThrows::comiss2 = 0.0f;

// clang-format off
naked void detour1() { // player in ecx
    __asm {
        // 
            cmp byte ptr [LockOnDisablesThrows::mod_enabled], 0
            je originalcode

            test ecx, ecx // probably isn't necessary but lets play it safe
            je originalcode
            cmp dword ptr [ecx+0x2b60], 0
            jne jnecode // if locking on, ret
       
        originalcode:
            mov eax, [edi+0x00002FA8]
            jmp dword ptr [LockOnDisablesThrows::jmp_ret1]

        jnecode:
            jmp dword ptr [LockOnDisablesThrows::jmp_jne1]
    }
}
 
naked void detour2() { // player in ebx
    __asm {
        // 
            cmp byte ptr [LockOnDisablesThrows::mod_enabled], 0
            je originalcode

            cmp dword ptr [ebx+0x2b60], 0
            jne jnecode
       
        originalcode:
            comiss xmm0, [LockOnDisablesThrows::comiss2] // 0.0f
            jmp dword ptr [LockOnDisablesThrows::jmp_ret2]

        jnecode:
            jmp dword ptr [LockOnDisablesThrows::jmp_jne2]
    }
}
 // clang-format on

std::optional<std::string> LockOnDisablesThrows::on_initialize() {
    LockOnDisablesThrows::jmp_jne1 = g_framework->get_module().as<uintptr_t>() + 0x3C804A;
    if (!install_hook_offset(0x3C7F1F, m_hook1, &detour1, &LockOnDisablesThrows::jmp_ret1, 6)) {
        spdlog::error("Failed to init LockOnDisablesThrows mod\n");
        return "Failed to init LockOnDisablesThrows mod";
    }
    LockOnDisablesThrows::jmp_jne2 = g_framework->get_module().as<uintptr_t>() + 0x3C5545;
    if (!install_hook_offset(0x3C54EF, m_hook2, &detour2, &LockOnDisablesThrows::jmp_ret2, 7)) {
        spdlog::error("Failed to init LockOnDisablesThrows mod\n");
        return "Failed to init LockOnDisablesThrows mod";
    }

    return Mod::on_initialize();
}

void LockOnDisablesThrows::on_draw_ui() {
    ImGui::Checkbox("Lockon Disables Throws", &mod_enabled);
}

// during load
void LockOnDisablesThrows::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("lockon_disables_throws").value_or(false);
}
// during save
void LockOnDisablesThrows::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("lockon_disables_throws", mod_enabled);
}

// do something every frame
//void LockOnDisablesThrows::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void LockOnDisablesThrows::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
