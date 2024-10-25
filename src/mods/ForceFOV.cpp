#include "ForceFOV.hpp"
#if 1
bool ForceFOV::force_fov = false;
bool ForceFOV::disable_attack_zoom = false;
float ForceFOV::custom_fov = 0.0f;
float ForceFOV::default_fov = 0.0f;
uintptr_t ForceFOV::jmp_ret1 = NULL;
uintptr_t ForceFOV::jmp_ret2 = NULL;

void ForceFOV::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3C6BF2, m_patch, "\x90\x90\x90\x90", 4); // nop 4
    }
    else {
        install_patch_offset(0x3C6BF2, m_patch, "\xF3\x0F\x5C\xC8", 4); // subss xmm1,xmm0
    }          
}

// clang-format off
naked void detour1() { // used most of the time
    __asm {
        cmp byte ptr [ForceFOV::force_fov], 0
        je originalcode

        addss xmm1, [ForceFOV::custom_fov]
        jmp dword ptr [ForceFOV::jmp_ret1]

        originalcode:
        addss xmm1, [ForceFOV::default_fov]
        jmp dword ptr [ForceFOV::jmp_ret1]
    }
}

naked void detour2() { // used during attacks
    __asm {
        cmp byte ptr [ForceFOV::force_fov], 0
        je originalcode

        movss xmm1, [ForceFOV::custom_fov]
        jmp dword ptr [ForceFOV::jmp_ret2]

        originalcode:
        movss xmm1, [ForceFOV::default_fov]
        jmp dword ptr [ForceFOV::jmp_ret2]
    }
}

 // clang-format on

std::optional<std::string> ForceFOV::on_initialize() {
    default_fov = *(float*)(g_framework->get_module().as<uintptr_t>() + 0x7A44E0);
    if (!install_hook_offset(0x3C55B7, m_hook1, &detour1, &ForceFOV::jmp_ret1, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    if (!install_hook_offset(0x3C6BE4, m_hook2, &detour2, &ForceFOV::jmp_ret2, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    return Mod::on_initialize();
}

void ForceFOV::on_draw_ui() {
    ImGui::Checkbox("Custom FOV", &force_fov);
    if (force_fov) {
        ImGui::SliderFloat("##CustomFOVSliderFloat", &ForceFOV::custom_fov, 1.0f, 180.0f, "%.0f");
        help_marker("Default 55");
    }
    if (ImGui::Checkbox("Disable Attack Camera Zoom", &disable_attack_zoom)) {
        toggle(disable_attack_zoom);
    }
}

// during load
void ForceFOV::on_config_load(const utility::Config &cfg) {
    force_fov = cfg.get<bool>("force_fov").value_or(false);
    custom_fov = cfg.get<float>("custom_fov_value").value_or(55.0f);
    disable_attack_zoom = cfg.get<bool>("disable_attack_zoom").value_or(false);
    toggle(disable_attack_zoom);
}
// during save
void ForceFOV::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("force_fov", force_fov);
    cfg.set<float>("custom_fov_value", custom_fov);
    cfg.set<bool>("disable_attack_zoom", disable_attack_zoom);
}

// do something every frame
//void ForceFOV::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ForceFOV::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
