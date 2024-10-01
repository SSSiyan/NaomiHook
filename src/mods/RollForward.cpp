#include "RollForward.hpp"
#if 1
bool RollForward::mod_enabled = false;
uintptr_t RollForward::jmp_ret1 = NULL;
uintptr_t RollForward::Offset_LeftStickUp = NULL;
uintptr_t RollForward::Offset_LeftStickLeft = NULL;
uintptr_t RollForward::Offset_JNE_1 = NULL;

uintptr_t RollForward::jmp_ret2 = NULL;

uintptr_t RollForward::jmp_ret3 = NULL;
uintptr_t RollForward::Offset_GetEdgeButton = NULL;
uintptr_t RollForward::Offset_JNE3 = NULL;

// clang-format off
naked void detour1() {
    __asm {
        cmp byte ptr [RollForward::mod_enabled], 0
        je originalcode
        push eax
        // cmp byte ptr [RollForward::Offset_LeftStickUp], 0
        mov eax, [RollForward::Offset_LeftStickUp]
        mov eax, [eax] // why
        cmp al, 0
        pop eax
        jne jnecode

        originalcode:
        push eax
        // cmp byte ptr [RollForward::Offset_LeftStickLeft], 0
        mov eax, [RollForward::Offset_LeftStickLeft]
        mov eax, [eax]
        cmp al, 0
        pop eax
        jmp dword ptr [RollForward::jmp_ret1]

        jnecode:
        jmp dword ptr [RollForward::Offset_JNE_1]
    }
}

naked void detour2() {
    __asm {
        cmp byte ptr [RollForward::mod_enabled], 0
        je originalcode

        push eax
        //cmp byte ptr [RollForward::Offset_LeftStickUp], 0
        mov eax, [RollForward::Offset_LeftStickUp]
        mov eax, [eax]
        cmp al, 0
        pop eax
        je originalcode
        mov ecx, 0x1A
        jmp dword ptr [RollForward::jmp_ret2]

        originalcode:
        mov ecx, 0x1B
        jmp dword ptr [RollForward::jmp_ret2]
    }
}

naked void detour3() {
    __asm {
        cmp byte ptr [RollForward::mod_enabled], 0
        je originalcode
        xor edx,edx
        lea ecx, [edx+0x0F]
        push 0
        // call dword ptr [RollForward::Offset_GetEdgeButton]
        call dword ptr [RollForward::Offset_GetEdgeButton]
        add esp, 4
        test al, al
        jne jnecode

        originalcode:
        xor edx,edx
        push 0
        lea ecx, [edx+0x0C]
        jmp dword ptr [RollForward::jmp_ret3]

        jnecode:
        jmp dword ptr [RollForward::Offset_JNE3]
    }
}
 
 // clang-format on

std::optional<std::string> RollForward::on_initialize() {
    RollForward::Offset_JNE_1 = g_framework->get_module().as<uintptr_t>() + 0x3D2628;
    RollForward::Offset_LeftStickUp = g_framework->get_module().as<uintptr_t>() + 0x84B91A;
    RollForward::Offset_LeftStickLeft = g_framework->get_module().as<uintptr_t>() + 0x84B8F6;
    
    if (!install_hook_offset(0x3D25F2, m_hook1, &detour1, &RollForward::jmp_ret1, 7)) {
        spdlog::error("Failed to init RollForward mod\n");
        return "Failed to init RollForward mod";
    }
    
    if (!install_hook_offset(0x3D2694, m_hook2, &detour2, &RollForward::jmp_ret2, 5)) {
        spdlog::error("Failed to init RollForward mod\n");
        return "Failed to init RollForward mod";
    }
    
    RollForward::Offset_GetEdgeButton = g_framework->get_module().as<uintptr_t>() + 0x5F8550;
    RollForward::Offset_JNE3 = g_framework->get_module().as<uintptr_t>() + 0x3D00F1;
    if (!install_hook_offset(0x3D009A, m_hook3, &detour3, &RollForward::jmp_ret3, 7)) {
        spdlog::error("Failed to init RollForward mod\n");
        return "Failed to init RollForward mod";
    }
    
    return Mod::on_initialize();
}

void RollForward::on_draw_ui() {
    ImGui::Checkbox("Roll Forward", &mod_enabled);
}

// during load
void RollForward::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("roll_forward").value_or(false);
}

// during save
void RollForward::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("roll_forward", mod_enabled);
}

// do something every frame
//void RollForward::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void RollForward::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
