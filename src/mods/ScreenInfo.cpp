#include "ScreenInfo.hpp"

// TODO(deep): move to sdk?
static ScreenInfoGame* g_screen_info_ptr {nullptr};
static uintptr_t g_screen_info_jmp_ret {NULL};
static std::unique_ptr<FunctionHook> g_screen_info_hook;

// clang-format off
naked void screen_info_detour() {
    __asm {

        mov dword ptr [g_screen_info_ptr], esi

        originalCode:
        mov [ebp-14h],00000001

        jmp dword ptr [g_screen_info_jmp_ret]
    }
}
// clang-format on

std::optional<std::string> ScreenInfo::on_initialize() {

    ptrdiff_t offset = (ptrdiff_t)GetModuleHandleA(NULL) + 0x1C0C15;
    if (!install_hook_absolute(offset, g_screen_info_hook, &screen_info_detour, &g_screen_info_jmp_ret, 7)) {
        constexpr auto err = "Failed to install screen info hook";
        return err;
    }

    return Mod::on_initialize();
}

ScreenInfoGame* ScreenInfo::get_screen_info() {
    return g_screen_info_ptr;
}
