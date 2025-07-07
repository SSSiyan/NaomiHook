#include "CameraSettings.hpp"
#if 1
const char* CameraSettings::defaultDescription = "Camera Settings";
const char* CameraSettings::hoveredDescription = defaultDescription;

bool CameraSettings::mod_enabled_screenshake = false;
uintptr_t CameraSettings::screenshake_jmp_ret1 = NULL;
uintptr_t CameraSettings::Offset_84BA18 = NULL;
int CameraSettings::customBasicScreenshakeAmount = 6;

bool CameraSettings::mod_enabled_first_person_x = false;
bool CameraSettings::CameraSettings::mod_enabled_darkside_x = false;

bool CameraSettings::force_fov = false;
bool CameraSettings::disable_attack_zoom = false;
float CameraSettings::custom_fov = 0.0f;
float CameraSettings::default_fov = 0.0f;
uintptr_t CameraSettings::fov_jmp_ret1 = NULL;
uintptr_t CameraSettings::fov_jmp_ret2 = NULL;

bool CameraSettings::deathblow_blur_enable = false;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        push eax
        mov eax, [CameraSettings::Offset_84BA18]
        mov [eax], 6
        pop eax
        cmp byte ptr [CameraSettings::mod_enabled_screenshake], 0
        je originalcode

        push dword ptr [CameraSettings::customBasicScreenshakeAmount]
        jmp dword ptr [CameraSettings::screenshake_jmp_ret1]

        originalcode:
        push 6
        jmp dword ptr [CameraSettings::screenshake_jmp_ret1]
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

void CameraSettings::fov_toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3C6BF2, m_fov_patch, "\x90\x90\x90\x90", 4); // nop 4
    }
    else {
        install_patch_offset(0x3C6BF2, m_fov_patch, "\xF3\x0F\x5C\xC8", 4); // subss xmm1,xmm0
    }
}


void CameraSettings::deathblow_blur_toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3C7521, deathblow_blur_patch, "\xC7\x87\xC8\x29\x00\x00\xC4\x09\x00\x00\xC7\x87\xCC\x29\x00\x00\x00\x00\xC0\x40\xC7\x87\xD0\x29\x00\x00\x00\x00\x20\x41\xC7\x87\xD4\x29\x00\x00\x02\x00\x00\x00", 40);
        // mov [edi+000029C8],#2500
        // mov [edi+000029CC],(float)6
        // mov [edi+000029D0],(float)10
        // mov [edi+000029D4],00000002
    }
    else {
        install_patch_offset(0x3C7521, deathblow_blur_patch, "\xC7\x87\xC8\x29\x00\x00\xC4\x09\x00\x00\xC7\x87\xCC\x29\x00\x00\x00\x00\x40\x40\xC7\x87\xD0\x29\x00\x00\x00\x00\x00\xC1\xC7\x87\xD4\x29\x00\x00\x01\x00\x00\x00", 40);
        // mov [edi+000029C8],#2500
        // mov [edi+000029CC],(float)3.0
        // mov [edi+000029D0],(float)-8.0
        // mov [edi+000029D4],00000001
    }
}

// clang-format off
naked void fov_detour1() { // used most of the time
    __asm {
        cmp byte ptr [CameraSettings::force_fov], 0
        je originalcode

        addss xmm1, [CameraSettings::custom_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret1]

        originalcode:
        addss xmm1, [CameraSettings::default_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret1]
    }
}

naked void fov_detour2() { // used during attacks
    __asm {
        cmp byte ptr [CameraSettings::force_fov], 0
        je originalcode

        movss xmm1, [CameraSettings::custom_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret2]

        originalcode:
        movss xmm1, [CameraSettings::default_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret2]
    }
}

 // clang-format on

std::optional<std::string> CameraSettings::on_initialize() {
    CameraSettings::Offset_84BA18 = g_framework->get_module().as<uintptr_t>() + 0x84BA18;
    if (!install_hook_offset(0x3C03BC, m_hook1, &detour1, &CameraSettings::screenshake_jmp_ret1, 12)) {
        spdlog::error("Failed to init CameraSettings mod\n");
        return "Failed to init CameraSettings mod";
    }

    default_fov = *(float*)(g_framework->get_module().as<uintptr_t>() + 0x7A44E0);
    if (!install_hook_offset(0x3C55B7, m_fov_hook1, &fov_detour1, &CameraSettings::fov_jmp_ret1, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    if (!install_hook_offset(0x3C6BE4, m_fov_hook2, &fov_detour2, &CameraSettings::fov_jmp_ret2, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    return Mod::on_initialize();
}

void CameraSettings::render_description() const {
    ImGui::TextWrapped(CameraSettings::hoveredDescription);
}

void CameraSettings::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Custom Screenshake On Normal Attacks", &mod_enabled_screenshake);
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;
    if (mod_enabled_screenshake) {
        ImGui::Indent();
        ImGui::Text("Custom Screenshake Amount");
        ImGui::SliderInt("##CustomScreenshakeAmountSliderInt", &customBasicScreenshakeAmount, 0, 20);
        if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Default 6";
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Invert X Axis During First Person", &mod_enabled_first_person_x)) {
        toggle_first_person_x(mod_enabled_first_person_x);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    if (ImGui::Checkbox("Invert X Axis During Darkside", &mod_enabled_darkside_x)) {
        toggle_darkside_x(mod_enabled_darkside_x);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Custom FOV", &force_fov);
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;
    if (force_fov) {
        ImGui::Indent();
        ImGui::SliderFloat("##CustomFOVSliderFloat", &CameraSettings::custom_fov, 1.0f, 180.0f, "%.0f");
        if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Default 55";
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Disable Attack Camera Zoom", &disable_attack_zoom)) {
        fov_toggle(disable_attack_zoom);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Disable the zoom when attacking enemies while not locked on.";

    if (ImGui::Checkbox("Deathblow Blur but its more like wii @DHMalice", &deathblow_blur_enable)) {
        deathblow_blur_toggle(deathblow_blur_enable);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Increases the intensity of the radial blur effect during Deathblows to more closely resemble the effect on Wii";
}

// during load
void CameraSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled_screenshake = cfg.get<bool>("custom_screenshake").value_or(false);
    customBasicScreenshakeAmount = cfg.get<int>("customBasicScreenshakeAmount").value_or(6);

    mod_enabled_first_person_x = cfg.get<bool>("invert_x_first_person").value_or(false);
    if (mod_enabled_first_person_x) toggle_first_person_x(mod_enabled_first_person_x);
    mod_enabled_darkside_x = cfg.get<bool>("invert_x_darkside").value_or(false);
    if (mod_enabled_darkside_x) toggle_darkside_x(mod_enabled_darkside_x);

    force_fov = cfg.get<bool>("force_fov").value_or(false);
    custom_fov = cfg.get<float>("custom_fov_value").value_or(55.0f);
    disable_attack_zoom = cfg.get<bool>("disable_attack_zoom").value_or(false);
    if (disable_attack_zoom) fov_toggle(disable_attack_zoom);

    deathblow_blur_enable = cfg.get<bool>("deathblow_blur_enable").value_or(false);
    if (deathblow_blur_enable) deathblow_blur_toggle(deathblow_blur_enable);
} 
// during save
void CameraSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_screenshake", mod_enabled_screenshake);
    cfg.set<int>("customBasicScreenshakeAmount", customBasicScreenshakeAmount);

    cfg.set<bool>("invert_x_first_person", mod_enabled_first_person_x);
    cfg.set<bool>("invert_x_darkside", mod_enabled_darkside_x);

    cfg.set<bool>("force_fov", force_fov);
    cfg.set<float>("custom_fov_value", custom_fov);
    cfg.set<bool>("disable_attack_zoom", disable_attack_zoom);

    cfg.set<bool>("deathblow_blur_enable", deathblow_blur_enable);
}

// do something every frame
//void CameraSettings::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void CameraSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
