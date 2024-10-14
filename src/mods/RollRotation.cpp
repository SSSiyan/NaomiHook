#include "RollRotation.hpp"
#if 1
bool RollRotation::mod_enabled = false;
uintptr_t RollRotation::jmp_ret1 = NULL;
uintptr_t RollRotation::mRotate2LockOnTarget = NULL;
uintptr_t RollRotation::GetMotionRunState = NULL;

// clang-format off
naked void detour1() { // mSetDamage+F9 // damage receiver in esi
    __asm {
        //
            cmp byte ptr [RollRotation::mod_enabled], 0
            je originalcode

            push eax
            push ecx
            push edx
            mov ecx, edi // player
            call dword ptr [RollRotation::mRotate2LockOnTarget]
            pop edx
            pop ecx
            pop eax

        originalcode:
            call dword ptr [RollRotation::GetMotionRunState]
            jmp dword ptr [RollRotation::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> RollRotation::on_initialize() {
    RollRotation::mRotate2LockOnTarget = g_framework->get_module().as<uintptr_t>() + 0x3C4C70;
    RollRotation::GetMotionRunState = g_framework->get_module().as<uintptr_t>() + 0x36BBF0;
    if (!install_hook_offset(0x3DB2CE, m_hook1, &detour1, &RollRotation::jmp_ret1, 5)) {
        spdlog::error("Failed to init RollRotation mod\n");
        return "Failed to init RollRotation mod";
    }

    return Mod::on_initialize();
}

void RollRotation::on_draw_ui() {
    ImGui::Checkbox("Roll Rotation", &mod_enabled);
}

// during load
void RollRotation::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("roll_rotation").value_or(false);
}
// during save
void RollRotation::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("roll_rotation", mod_enabled);
}

// do something every frame
//void RollRotation::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void RollRotation::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
