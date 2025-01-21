#include "HPHitNumber.hpp"
#if 1
bool HPHitNumber::mod_enabled = false;
uintptr_t HPHitNumber::jmp_ret1 = NULL;
uintptr_t HPHitNumber::jmp_ret2 = NULL;
uintptr_t HPHitNumber::gpBattle = NULL;
float HPHitNumber::verticalOffset = 0.0f;

void HPHitNumber::toggle(bool enable) { // hide "HIT" text
    if (enable) {
        install_patch_offset(0x428612, m_patch1, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10); // nop A
    }
    else {
        install_patch_offset(0x428612, m_patch1, "\x6A\x01\x6A\x01\x50\xE8\x44\xA1\x1D\x00", 10); // push 01, push 01, push eax, call nmh.exe+602760
    }
}

// clang-format off
naked void detour1() { // show hp instead of hit number
    __asm {
        // 
            cmp byte ptr [HPHitNumber::mod_enabled], 0
            je originalcode
        // 
            mov eax, [HPHitNumber::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164] // player
            test eax, eax
            je originalcode
            mov eax, [eax+0x2b60] // enemy
            test eax, eax
            je originalcode
            push ecx
            mov ecx, [eax+0x360] // lock on HUD
            test ecx, ecx
            je popcode
            mov word ptr [ecx+0x8E], 3 // which lockon to show
            mov word ptr [ecx+0x95], 1 // display HIT
            pop ecx
            cvttss2si eax, [eax+0x24] // HP
            jmp retcode

        popcode:
            pop ecx
        originalcode:
            movsx eax,word ptr [ebx+0x00000154]
        retcode:
            jmp dword ptr [HPHitNumber::jmp_ret1]
    }
}

naked void detour2() { // Move number to centre of lockon circle
    __asm {
        // 
            cmp byte ptr [HPHitNumber::mod_enabled], 0
            je originalcode
        // 
            movss xmm2, [HPHitNumber::verticalOffset]
            jmp retcode
        originalcode:
            movss xmm2, [esp+0x10]
        retcode:
            jmp dword ptr [HPHitNumber::jmp_ret2]
    }
}
 
 // clang-format on

std::optional<std::string> HPHitNumber::on_initialize() {
    HPHitNumber::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;

    if (!install_hook_offset(0x42851A, m_hook1, &detour1, &HPHitNumber::jmp_ret1, 7)) {
        spdlog::error("Failed to init HPHitNumber mod\n");
        return "Failed to init HPHitNumber mod";
    }

    if (!install_hook_offset(0x4285A5, m_hook2, &detour2, &HPHitNumber::jmp_ret2, 6)) {
        spdlog::error("Failed to init HPHitNumber mod\n");
        return "Failed to init HPHitNumber mod";
    }

    return Mod::on_initialize();
}

void HPHitNumber::on_draw_ui() {
    if (ImGui::Checkbox("HP Hit Number", &mod_enabled)) {
        toggle(mod_enabled);
    }
    ImGui::Text("HP Number Vertical Offset");
    ImGui::SliderFloat("## HP number vertical offset inputfloat", &verticalOffset, 0.0f, 500.0f, "%.0f");
    help_marker("I never tested this at other resolutions so leaving the option here\nDefault 285");
}

// during load
void HPHitNumber::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("hpHitNumber").value_or(false);
    toggle(mod_enabled);
    verticalOffset = cfg.get<float>("hpHitNumberVerticalOffset").value_or(285.0f);
}
// during save
void HPHitNumber::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("hpHitNumber", mod_enabled);
    cfg.set<float>("hpHitNumberVerticalOffset", verticalOffset);
}

// do something every frame
//void HPHitNumber::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void HPHitNumber::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
