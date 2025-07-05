#include "NMHFixes.hpp"
#if 1
bool NMHFixes::disableCrashDumps = false;
bool NMHFixes::rankUpCrashFix = false;
uintptr_t NMHFixes::rank_up_crash_fix_jmp_ret = NULL;
uintptr_t NMHFixes::rank_up_crash_fix_jmp_je = NULL;

// clang-format off
naked void rank_up_crash_fix_detour() {
    __asm {
    // originalcode
        lea eax, [ecx+0x01]
        mov [esp+0x1C], eax
        cmp byte ptr [NMHFixes::rankUpCrashFix], 0
        je retcode

    // newcode
        test ecx, ecx
        je je_code
    retcode:
        jmp dword ptr [NMHFixes::rank_up_crash_fix_jmp_ret]

    je_code:
        jmp dword ptr [NMHFixes::rank_up_crash_fix_jmp_je]
    }
}

 // clang-format on

void NMHFixes::disableCrashDumpsToggle(bool enable) {
    if (enable) {
        install_patch_offset(0x1F4A38, disable_crash_dumps_patch, "\x59\xED\x02\x00\x00\x90", 6); // jmp nmh.exe+1F4C9A
    }
    else {
        install_patch_offset(0x1F4A38, disable_crash_dumps_patch, "\x0F\x85\x5C\x02\x00\x00", 6); // jne nmh.exe+1F4C9A
    }
}

std::optional<std::string> NMHFixes::on_initialize() {
    rank_up_crash_fix_jmp_je = g_framework->get_module().as<uintptr_t>() + 0x4585A9;
    if (!install_hook_offset(0x45853A, rank_up_crash_fix_hook1, &rank_up_crash_fix_detour, &NMHFixes::rank_up_crash_fix_jmp_ret, 7)) {
        spdlog::error("Failed to init rank_up_crash_fix mod\n");
        return "Failed to init rank_up_crash_fix mod";
    }
    return Mod::on_initialize();
}

void NMHFixes::on_draw_ui() {
    if (ImGui::Checkbox("Disable Crash Dumps", &disableCrashDumps)) {
        disableCrashDumpsToggle(disableCrashDumps);
    }
    ImGui::Checkbox("Enable Rank Up Crash Fix", &rankUpCrashFix);
}

// during load
void NMHFixes::on_config_load(const utility::Config &cfg) {
    disableCrashDumps = cfg.get<bool>("disable_crash_dumps").value_or(true);
    if (disableCrashDumps) disableCrashDumpsToggle(disableCrashDumps);

    rankUpCrashFix = cfg.get<bool>("rankUpCrashFix").value_or(true);
}
// during save
void NMHFixes::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("disable_crash_dumps", disableCrashDumps);

    cfg.set<bool>("rankUpCrashFix", rankUpCrashFix);
}

// do something every frame
//void NMHFixes::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void NMHFixes::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
