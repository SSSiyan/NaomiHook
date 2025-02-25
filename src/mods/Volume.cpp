#include "Volume.hpp"
#if 1
bool Volume::mod_enabled = false;
uintptr_t Volume::HrSoundMusicVolume = NULL; // nmh.HrSound::uni+4
uintptr_t Volume::HrSoundSoundEffectVolume = NULL; // nmh.HrSound::uni+8
int music_volume = 0;
int sound_effect_volume = 0;

std::optional<std::string> Volume::on_initialize() {
    Volume::HrSoundMusicVolume = g_framework->get_module().as<uintptr_t>() + 0x8434F0+4;
    Volume::HrSoundSoundEffectVolume = g_framework->get_module().as<uintptr_t>() + 0x8434F0+8;

    return Mod::on_initialize();
}

void Volume::on_draw_ui() {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        ImGui::SliderInt("Music Volume", (int*)HrSoundMusicVolume, 0, 128);
            //music_volume = *(int*)HrSoundMusicVolume;
        ImGui::SliderInt("Sound Effect Volume", (int*)HrSoundSoundEffectVolume, 0, 128);
            //sound_effect_volume = *(int*)HrSoundSoundEffectVolume;
    }
}

// during load
void Volume::on_config_load(const utility::Config &cfg) {
    // music_volume = cfg.get<int>("music_volume").value_or(128);
    // sound_effect_volume = cfg.get<int>("sound_effect_volume").value_or(128);
}
// during save
void Volume::on_config_save(utility::Config &cfg) {
    // cfg.set<int>("music_volume", music_volume);
    // cfg.set<int>("sound_effect_volume", sound_effect_volume);
}

// do something every frame
//void Volume::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void Volume::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
