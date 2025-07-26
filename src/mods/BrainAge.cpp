#include "BrainAge.hpp"
#if 1
bool BrainAge::imguiPopout = false;

template<typename T>
bool getBit(T flags, int bit) {
    return (flags & (1 << bit)) != 0;
}

template<typename T>
void setBit(T& flags, int bit, bool value) {
    if (value) {
        flags |= (1 << bit);  // Set the bit
    } else {
        flags &= ~(1 << bit); // Clear the bit
    }
}
void BrainAge::Stuff() {
    if (ImGui::CollapsingHeader("New thing 1")) {

    }

    if (ImGui::CollapsingHeader("New thing 2")) {

    }

    static bool disableCamBeingAutoSet = false;
    if (ImGui::Checkbox("Disable the game setting cam mode", &disableCamBeingAutoSet)) {
        BrainAge::toggleCam(disableCamBeingAutoSet);
    }
}

void BrainAge::on_draw_ui() {
    ImGui::Checkbox("Brain Age Popout", &imguiPopout);
    if (!imguiPopout)
        Stuff();
}

//void BrainAge::custom_imgui_window() {}

// during load
//void BrainAge::on_config_load(const utility::Config &cfg) {}
// during save
//void BrainAge::on_config_save(utility::Config &cfg) {}
// do something every frame

void BrainAge::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("imguiPopout", &imguiPopout);
        Stuff();
        ImGui::End();
    }
}

void BrainAge::toggleCam(bool enable) {
    if (enable) {
        // stops cam being set
        install_patch_offset(0x3EC616, m_patch1, "\x90\x90\x90\x90\x90\x90", 6); // 
    }
    else {
        install_patch_offset(0x3EC616, m_patch1, "\x89\x0D\xA0\xA4\x7E\x01", 6); // mov [nmh.gHrCamera],ecx
    }
}

std::optional<std::string> BrainAge::on_initialize() {
    return Mod::on_initialize();
}

// will show up in debug window, dump ImGui widgets you want here
//void BrainAge::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
