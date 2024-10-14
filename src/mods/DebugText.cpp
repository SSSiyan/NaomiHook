#include "DebugText.hpp"
#if 1
bool DebugText::mod_enabled = false;
uintptr_t DebugText::jmp_ret1 = NULL;
uintptr_t DebugText::DrawFunc = NULL;
char DebugText::customDebugString[256] = "Debug Text";
float DebugText::xPos = 0.0f;
float DebugText::yPos = 0.0f;

// clang-format off
naked void detour1() { // player in edi
    __asm {
        // 
            cmp byte ptr [DebugText::mod_enabled], 0
            je originalcode

            push eax
            push ecx
            push edx

            push offset [DebugText::customDebugString] // text*
            push dword ptr [DebugText::yPos]
            push dword ptr [DebugText::xPos]
            call dword ptr [DebugText::DrawFunc] // eax, ecx, edx
            add esp, 0x0C

        skipcall:
            pop edx
            pop ecx
            pop eax

        originalcode:
            mov eax, [edi+0x00000158]
            jmp dword ptr [DebugText::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> DebugText::on_initialize() {
    DebugText::customDebugString[255] = '\0';
    DebugText::DrawFunc = g_framework->get_module().as<uintptr_t>() + 0x5F7030;
    if (!install_hook_offset(0x3B64E4, m_hook1, &detour1, &DebugText::jmp_ret1, 6)) {
        spdlog::error("Failed to init DebugText mod\n");
        return "Failed to init DebugText mod";
    }

    return Mod::on_initialize();
}

void DebugText::on_draw_ui() {
    ImGui::Checkbox("Draw Debug Text", &mod_enabled);
    if (mod_enabled) {
        ImGui::SliderFloat("xPos", &xPos, 0.0f, 854.0f, "%.0f");
        ImGui::SliderFloat("yPos", &yPos, 0.0f, 480.0f, "%.0f");
        ImGui::InputText("##customDebugInputText", &customDebugString[0], 256);
    }
}

// during load
void DebugText::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("debug_text").value_or(false);
}
// during save
void DebugText::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("debug_text", mod_enabled);
}

// do something every frame
//void DebugText::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DebugText::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
