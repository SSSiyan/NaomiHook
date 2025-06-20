#include "ResolutionScaleFix.hpp"

// move everything out of memeber variables since this thing initializes before g_framework is in valid state :shrug:
static uintptr_t ee_app_windows_constructor_jump {NULL};
static std::unique_ptr<FunctionHook> g_hook;

void winapi_set_dpi_awareness() {
    BOOL dpi_success = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    assert(dpi_success);
}

// clang-format off
naked void detour() {
    __asm {
        pushad
        call winapi_set_dpi_awareness
        popad
original:
        push ebp
        mov ebp, esp
        and esp, 0FFFFFFF8h

        jmp qword ptr [ee_app_windows_constructor_jump]
    }
}
// clang-format on

std::optional<std::string> ResolutionScaleFix::on_initialize() {
    ptrdiff_t offset = (ptrdiff_t)GetModuleHandleA(NULL) + 0x20EFD0;
    // cant use install_hook_offset since m_module is not valid yet
    if (!install_hook_absolute(offset, g_hook, &detour, &ee_app_windows_constructor_jump, 6)) {
        return "failed to initialize dpi hook";
    }
#if 0 // d3d11 bb size
    if (!install_hook_absolute(0x0060ADB5, m_hook, &detour, &jump_back, 6)) {
        return "failed to initialize dpi hook";
    }
#endif
    return Mod::on_initialize();
}
