#include "HpColor.hpp"

// clang-format off
#if 0
static naked void detour() {
    __asm {
        mov qword ptr [HpColor::variable], rbx
        mov rax, 0xDEADBEEF
        jmp qword ptr [jmp_ret]
    }
}
#endif
// clang-format on

std::optional<std::string> HpColor::on_initialize() {
    return Mod::on_initialize();
}

// during load
//void HpColor::on_config_load(const utility::Config &cfg) {}
// during save
//void HpColor::on_config_save(utility::Config &cfg) {}
// do something every frame
//void HpColor::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void HpColor::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
//void HpColor::on_draw_ui() {}
