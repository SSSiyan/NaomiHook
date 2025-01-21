#include "CameraSettings.hpp"
#if 1
bool CameraSettings::mod_enabled_screenshake = false;
uintptr_t CameraSettings::jmp_ret1 = NULL;
uintptr_t CameraSettings::Offset_84BA18 = NULL;
int CameraSettings::customBasicScreenshakeAmount = 6;

bool CameraSettings::mod_enabled_first_person_x = false;
bool CameraSettings::CameraSettings::mod_enabled_darkside_x = false;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        push eax
        mov eax,[CameraSettings::Offset_84BA18]
        mov [eax],6
        pop eax
        cmp byte ptr [CameraSettings::mod_enabled_screenshake], 0
        je originalcode

        push dword ptr [CameraSettings::customBasicScreenshakeAmount]
        jmp dword ptr [CameraSettings::jmp_ret1]

        originalcode:
        push 6
        jmp dword ptr [CameraSettings::jmp_ret1]
    }
}

void CameraSettings::toggle_first_person_x(bool enable) {
    if (enable) {
        install_patch_offset(0x3DA503, m_patch_first_person_x, "\x80\x3D\x1A", 3); // cmp byte ptr [nmh.mHRPc::CameraXReverseControlFlag]
    }
    else {
        install_patch_offset(0x3DA503, m_patch_first_person_x, "\x80\x3D\x1B", 3); // cmp byte ptr [nmh.mHRPc::CameraYReverseControlFlag]
    }
}

void CameraSettings::toggle_darkside_x(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC2C3, m_patch_darkside_x, "\x80\x3D\x1A", 3); // cmp byte ptr [nmh.mHRPc::CameraXReverseControlFlag]
    }
    else {
        install_patch_offset(0x3DC2C3, m_patch_darkside_x, "\x80\x3D\x1B", 3); // cmp byte ptr [nmh.mHRPc::CameraYReverseControlFlag]
    }
}

 // clang-format on

std::optional<std::string> CameraSettings::on_initialize() {
    CameraSettings::Offset_84BA18 = g_framework->get_module().as<uintptr_t>() + 0x84BA18;
    if (!install_hook_offset(0x3C03BC, m_hook1, &detour1, &CameraSettings::jmp_ret1, 12)) {
        spdlog::error("Failed to init CameraSettings mod\n");
        return "Failed to init CameraSettings mod";
    }

    return Mod::on_initialize();
}

void CameraSettings::on_draw_ui() {
    ImGui::Checkbox("Custom Screenshake On Normal Attacks", &mod_enabled_screenshake);
    if (mod_enabled_screenshake) {
        ImGui::Text("Custom Screenshake Amount");
        ImGui::SliderInt("##CustomScreenshakeAmountSliderInt", &customBasicScreenshakeAmount, 0, 20);
        help_marker("Default 6");
    }

    if (ImGui::Checkbox("Invert X Axis First Person", &mod_enabled_first_person_x)) {
        toggle_first_person_x(mod_enabled_first_person_x);
    }
    if (ImGui::Checkbox("Invert X Axis Darkside", &mod_enabled_darkside_x)) {
        toggle_darkside_x(mod_enabled_darkside_x);
    }
}

// during load
void CameraSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled_screenshake = cfg.get<bool>("custom_screenshake").value_or(false);
    customBasicScreenshakeAmount = cfg.get<int>("customBasicScreenshakeAmount").value_or(6);
    mod_enabled_first_person_x = cfg.get<bool>("invert_x_first_person").value_or(false);
    toggle_first_person_x(mod_enabled_first_person_x);
    mod_enabled_darkside_x = cfg.get<bool>("invert_x_darkside").value_or(false);
    toggle_darkside_x(mod_enabled_darkside_x);
} 
// during save
void CameraSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_screenshake", mod_enabled_screenshake);
    cfg.set<int>("customBasicScreenshakeAmount", customBasicScreenshakeAmount);
    cfg.set<bool>("invert_x_first_person", mod_enabled_first_person_x);
    cfg.set<bool>("invert_x_darkside", mod_enabled_darkside_x);
}

// do something every frame
//void CameraSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void CameraSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
