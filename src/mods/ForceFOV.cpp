#include "ForceFOV.hpp"
#if 1
bool ForceFOV::mod_enabled = false;
float ForceFOV::custom_fov = 85.0f;
float ForceFOV::default_fov = 0.0f;
uintptr_t ForceFOV::jmp_ret1 = NULL;
uintptr_t ForceFOV::jmp_ret2 = NULL;

// clang-format off
naked void detour1() { // used most of the time
    __asm {
        cmp byte ptr [ForceFOV::mod_enabled], 0
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
        cmp byte ptr [ForceFOV::mod_enabled], 0
        je originalcode

        movss xmm1, [ForceFOV::custom_fov]
        jmp dword ptr [ForceFOV::jmp_ret1]

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
    ImGui::Checkbox("Force FOV", &mod_enabled);
    ImGui::InputFloat("InputFloat Example", &ForceFOV::custom_fov, 1.0f, 180.0f, "%.0f");
    ImGui::SliderFloat("SliderFloat Example", &ForceFOV::custom_fov, 1.0f, 180.0f, "%.0f");
    ImGui::DragFloat("DragFloat Example", &ForceFOV::custom_fov, 1.0f, 1.0f, 180.0f, "%.0f");
}

// during load
void ForceFOV::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("force_fov").value_or(false);
    custom_fov = cfg.get<float>("custom_fov").value_or(55.0f);
}
// during save
void ForceFOV::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("force_fov", mod_enabled);
    cfg.set<float>("custom_fov", custom_fov);
}

// do something every frame
//void ForceFOV::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ForceFOV::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
