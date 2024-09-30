#include "SimpleMod.hpp"

#if 0
// clang-format off
static uintptr_t return_address {};
naked void detour(void) {
    __asm {
        //int 3
        //jmp DWORD PTR[return_address]
    }
}
// clang-format on
#endif

std::optional<std::string> SimpleMod::on_initialize() {
    return Mod::on_initialize();
}

// during load
// void SimpleMod::on_config_load(const utility::Config &cfg) {}
// during save
// void SimpleMod::on_config_save(utility::Config &cfg) {}
// do something every frame
// void SimpleMod::on_frame() {}
#if 0
// will show up in debug window, dump ImGui widgets you want here
void SimpleMod::on_draw_debug_ui() {
}
#endif
#if 0
// will show up in main window, dump ImGui widgets you want here
void SimpleMod::on_draw_ui() {
}
#endif