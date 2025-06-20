#if 0
#include "ModSample.hpp"

// clang-format off
#if 0
naked void detour() {
    __asm {
        mov qword ptr [ModSample::variable], rbx
        mov rax, 0xDEADBEEF
        jmp qword ptr [jmp_ret]
    }
}
#endif
// clang-format on

std::optional<std::string> ModSample::on_initialize() {
    return Mod::on_initialize();
}

// during load
//void ModSample::on_config_load(const utility::Config &cfg) {}
// during save
//void ModSample::on_config_save(utility::Config &cfg) {}
// do something every frame
//void ModSample::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ModSample::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
//void ModSample::on_draw_ui() {}
#endif