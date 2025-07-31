#include "Volume.hpp"
#if 1
bool Volume::mod_enabled                   = false;
uintptr_t Volume::HrSoundBase              = 0;

// User-controlled toggles (off by default)
static bool enable_local_volume  = false;
static bool enable_exmenu_volume = false;

std::optional<std::string> Volume::on_initialize() {
    HrSoundBase = g_framework->get_module().as<uintptr_t>() + 0x8434F0; // nmh.HrSound::uni
    return Mod::on_initialize();
}

void Volume::on_draw_ui() {
    HrSound* hrSound = (HrSound*)HrSoundBase;
    if (hrSound) {
        ImGui::Text("Global Volume Controls");
        ImGui::SliderInt("Music Volume", (int*)&hrSound->BGMVolume, 0, 128);
        ImGui::SliderInt("Sound Effect Volume", (int*)&hrSound->SEVolume, 0, 128);

        ImGui::Separator();

        ImGui::Checkbox("Enable Local Volume Overrides", &enable_local_volume);
        ImGui::BeginDisabled(!enable_local_volume);
        ImGui::SliderInt("Local BGM Volume", (int*)&hrSound->LocalBGMVolume, 0, 128);
        ImGui::SliderInt("Local SE Volume", (int*)&hrSound->LocalSEVolume, 0, 128);
        ImGui::EndDisabled();

        ImGui::Separator();

        ImGui::Checkbox("Enable EX Menu Volume Controls", &enable_exmenu_volume);
        ImGui::BeginDisabled(!enable_exmenu_volume);
        ImGui::SliderInt("EX Menu BGM Volume", (int*)&hrSound->ExMenuBGMVolume, 0, 128);
        ImGui::SliderInt("EX Menu SE Volume", (int*)&hrSound->ExMenuSEVolume, 0, 128);
        ImGui::EndDisabled();
    }
}

void Volume::on_config_load(const utility::Config& cfg) {
    enable_local_volume  = cfg.get<bool>("enable_local_volume").value_or(false);
    enable_exmenu_volume = cfg.get<bool>("enable_exmenu_volume").value_or(false);
}

void Volume::on_config_save(utility::Config& cfg) {
    cfg.set<bool>("enable_local_volume", enable_local_volume);
    cfg.set<bool>("enable_exmenu_volume", enable_exmenu_volume);
}
#endif
