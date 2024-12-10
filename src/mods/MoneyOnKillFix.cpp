#include "MoneyOnKillFix.hpp"

#if 1
bool MoneyOnKillFix::mod_enabled = false;

uintptr_t MoneyOnKillFix::jmp_ret1 = NULL;
uintptr_t MoneyOnKillFix::jmp_ret2 = NULL;
uintptr_t MoneyOnKillFix::jmp_ret3 = NULL;

// clang-format off
naked void detour1() { // Throws
    __asm {
        // 
            cmp byte ptr [MoneyOnKillFix::mod_enabled], 0
            je originalcode
        // 
            // cmp ax, [edi+0x28] // is new value lower than old value
            // jb cont
            cmp ax, 30000 // is new value above 30k
            jb originalcode
            mov ax, 30000 // limit to 30k

        originalcode:
            mov [edi+0x28], ax
        cont:
            cmp esi, 0x0F
            jmp dword ptr [MoneyOnKillFix::jmp_ret1]
    }
}

naked void detour2() { // Beat + Sword attacks
    __asm {
        // 
            cmp byte ptr [MoneyOnKillFix::mod_enabled], 0
            je originalcode
        // 
            cmp word ptr [ebx+0x28], 30000 // is value currently above 30k
            jb originalcode
            mov word ptr [ebx+0x28], 30000 // limit to 30k
            jmp retcode

        originalcode:
            add word ptr [ebx+0x28], 05
        retcode:
            jmp dword ptr [MoneyOnKillFix::jmp_ret2]
    }
}

naked void detour3() { // FinishBonus
    __asm {
        // 
            cmp byte ptr [MoneyOnKillFix::mod_enabled], 0
            je originalcode
        // 
            cmp ax, [ebx+0x28] // is new value lower than old value
            jb cont
            cmp ax, 30000 // is new value above 30k
            jb originalcode
            mov ax, 30000 // limit to 30k

        originalcode:
            mov [ebx+0x28], ax
        cont:
            mov ecx, [ebx+0x00000360]
            jmp dword ptr [MoneyOnKillFix::jmp_ret3]
    }
}
 // clang-format on

std::optional<std::string> MoneyOnKillFix::on_initialize() {
    if (!install_hook_offset(0x43640E, m_hook1, &detour1, &MoneyOnKillFix::jmp_ret1, 7)) {
        spdlog::error("Failed to init MoneyOnKillFix mod\n");
        return "Failed to init MoneyOnKillFix mod";
    } 

    if (!install_hook_offset(0x3CB950, m_hook2, &detour2, &MoneyOnKillFix::jmp_ret2, 5)) {
        spdlog::error("Failed to init MoneyOnKillFix mod\n");
        return "Failed to init MoneyOnKillFix mod";
    } 

    if (!install_hook_offset(0x3CBC15, m_hook3, &detour3, &MoneyOnKillFix::jmp_ret3, 10)) {
        spdlog::error("Failed to init MoneyOnKillFix mod\n");
        return "Failed to init MoneyOnKillFix mod";
    } 

    return Mod::on_initialize();
}

void MoneyOnKillFix::on_draw_ui() {
    ImGui::Checkbox("Limit Money On Kill Reward", &mod_enabled);
}

// during load
void MoneyOnKillFix::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("money_on_kill_fix").value_or(false);
}

// during save
void MoneyOnKillFix::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("money_on_kill_fix", mod_enabled);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
