#include "PlayerTracker.hpp"
#if 1

std::optional<std::string> PlayerTracker::on_initialize() {
    return Mod::on_initialize();
}

void PlayerTracker::on_draw_ui() {
}

// during load
//void PlayerTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void PlayerTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
//void PlayerTracker::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void PlayerTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
