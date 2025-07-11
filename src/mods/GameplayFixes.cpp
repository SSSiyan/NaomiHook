#include "GameplayFixes.hpp"

#if 1
bool GameplayFixes::money_on_kill_fix_enabled = false;

uintptr_t GameplayFixes::money_on_kill_fix_jmp_ret1 = NULL;
uintptr_t GameplayFixes::money_on_kill_fix_jmp_ret2 = NULL;
uintptr_t GameplayFixes::money_on_kill_fix_jmp_ret3 = NULL;

bool GameplayFixes::baseball_power_fix_enabled = false;
uintptr_t GameplayFixes::baseball_power_fix_jmp_ret1 = NULL;
uintptr_t GameplayFixes::baseball_power_fix_jmp_ret2 = NULL;

const char* GameplayFixes::defaultDescription = "Here you'll find various fixes for gameplay bugs.";
const char* GameplayFixes::hoveredDescription = defaultDescription;

void GameplayFixes::render_description() const {
    ImGui::TextWrapped(GameplayFixes::hoveredDescription);
}

// clang-format off
// MONEY ON KILL FIX
naked void money_on_kill_fix_detour1() { // Throws
    __asm {
            cmp byte ptr [GameplayFixes::money_on_kill_fix_enabled], 0
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
            jmp dword ptr [GameplayFixes::money_on_kill_fix_jmp_ret1]
    }
}
naked void money_on_kill_fix_detour2() { // Beat + Sword attacks
    __asm {
            cmp byte ptr [GameplayFixes::money_on_kill_fix_enabled], 0
            je originalcode
        // 
            cmp word ptr [ebx+0x28], 30000 // is value currently above 30k
            jb originalcode
            mov word ptr [ebx+0x28], 30000 // limit to 30k
            jmp retcode

        originalcode:
            add word ptr [ebx+0x28], 05
        retcode:
            jmp dword ptr [GameplayFixes::money_on_kill_fix_jmp_ret2]
    }
}
naked void money_on_kill_fix_detour3() { // FinishBonus
    __asm {
            cmp byte ptr [GameplayFixes::money_on_kill_fix_enabled], 0
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
            jmp dword ptr [GameplayFixes::money_on_kill_fix_jmp_ret3]
    }
}
// BASEBALL FIX
naked void baseball_power_fix_detour1() { // STG0412
    __asm {
            cmp byte ptr [GameplayFixes::baseball_power_fix_enabled], 0
            je originalcode

            addss xmm0, [GameplayFixes::new_baseball_power]

        originalcode:
            addss xmm0, [GameplayFixes::baseball_power_fix_original_value]
            jmp dword ptr [GameplayFixes::baseball_power_fix_jmp_ret1]
    }
}
naked void baseball_power_fix_detour2() { // Other stages
    __asm {
            cmp byte ptr [GameplayFixes::baseball_power_fix_enabled], 0
            je originalcode

            addss xmm0, [GameplayFixes::new_baseball_power]

        originalcode:
            addss xmm0, [GameplayFixes::baseball_power_fix_original_value]
            jmp dword ptr [GameplayFixes::baseball_power_fix_jmp_ret2]
    }
}
 // clang-format on

std::optional<std::string> GameplayFixes::on_initialize() {
    // MONEY ON KILL FIX
    if (!install_hook_offset(0x43640E, money_on_kill_fix_hook1, &money_on_kill_fix_detour1, &GameplayFixes::money_on_kill_fix_jmp_ret1, 7)) {
        spdlog::error("Failed to init money_on_kill_fix mod 1\n");
        return "Failed to init money_on_kill_fix mod 1";
    }
    if (!install_hook_offset(0x3CB950, money_on_kill_fix_hook2, &money_on_kill_fix_detour2, &GameplayFixes::money_on_kill_fix_jmp_ret2, 5)) {
        spdlog::error("Failed to init money_on_kill_fix mod 2\n");
        return "Failed to init money_on_kill_fix mod 2";
    }
    if (!install_hook_offset(0x3CBC15, money_on_kill_fix_hook3, &money_on_kill_fix_detour3, &GameplayFixes::money_on_kill_fix_jmp_ret3, 10)) {
        spdlog::error("Failed to init money_on_kill_fix mod 3\n");
        return "Failed to init money_on_kill_fix mod 3";
    }
    // BASEBALL FIX
    if (!install_hook_offset(0x4E314C, baseball_power_fix_hook1, &baseball_power_fix_detour1, &GameplayFixes::baseball_power_fix_jmp_ret1, 8)) {
        spdlog::error("Failed to init baseball_power_fix mod 1\n");
        return "Failed to init baseball_power_fix mod 1";
    }
    if (!install_hook_offset(0x4E0ABC, baseball_power_fix_hook2, &baseball_power_fix_detour2, &GameplayFixes::baseball_power_fix_jmp_ret2, 8)) {
        spdlog::error("Failed to init baseball_power_fix mod 2\n");
        return "Failed to init baseball_power_fix mod 2";
    }

    return Mod::on_initialize();
}

void GameplayFixes::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) GameplayFixes::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Limit Money On Kill Reward", &money_on_kill_fix_enabled);
    if (ImGui::IsItemHovered()) GameplayFixes::hoveredDescription = "Fixes an overflow issue affecting the cash reward for killing enemies. "
    "Our change fixes this issue the same way Heroes Paradise does by limiting the maximum reward to 30,000.";

    ImGui::Checkbox("Baseball Power Fix", &baseball_power_fix_enabled);
    if (ImGui::IsItemHovered()) GameplayFixes::hoveredDescription = "Changes the strength of your swing during baseball sequences, making the mini-game based entirely on timing.";
}

// during load
void GameplayFixes::on_config_load(const utility::Config &cfg) {
    money_on_kill_fix_enabled = cfg.get<bool>("money_on_kill_fix").value_or(false);
    baseball_power_fix_enabled = cfg.get<bool>("baseball_power_fix").value_or(false);
}

// during save
void GameplayFixes::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("money_on_kill_fix", money_on_kill_fix_enabled);
    cfg.set<bool>("baseball_power_fix", baseball_power_fix_enabled);
}

// do something every frame
//void NMHFixes::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void NMHFixes::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
