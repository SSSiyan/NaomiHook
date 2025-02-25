#include "ThrowBattery.hpp"
#if 1
bool ThrowBattery::mod_enabled = false;
uintptr_t ThrowBattery::jmp_ret1 = NULL;
uintptr_t ThrowBattery::gpBattle = NULL;
uintptr_t ThrowBattery::mAddBattery = NULL;
uintptr_t ThrowBattery::dAppearLog = NULL;
uintptr_t ThrowBattery::Play3D = NULL;
uintptr_t ThrowBattery::mSetSuccessThrow = NULL;
int ThrowBattery::batteryThrowReward = 120;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        // 
            cmp byte ptr [ThrowBattery::mod_enabled], 0
            je originalcode
        // 
            push edx
            push ecx
            push eax
            mov ecx,edi // put player in ecx for calls

        // add battery
            push dword ptr [ThrowBattery::batteryThrowReward]
            call dword ptr [ThrowBattery::mAddBattery] // rets 4, fucks eax, ecx, edx

        // play visual
            mov ecx, [ThrowBattery::gpBattle]
            mov ecx, [ecx]
            // mov edi, [ecx+0x164] // player already in edi
            mov ecx, [ecx+0x14C] // hrScreenStatus
            push 0x0d
            push 0x3ea
            call [ThrowBattery::dAppearLog] // rets 8, fucks eax, ecx, edx

        // play sound
            movss xmm2, [edi+0x18]
            xor edx, edx
            movss xmm1, [edi+0x14]
            mov ecx, 0x000001F7
            movss xmm0, [edi+0x10]
            call dword ptr [ThrowBattery::Play3D] // fucks eax, edx
            popcode:
            pop eax
            pop ecx
            pop edx

        originalcode:
            call dword ptr [ThrowBattery::mSetSuccessThrow]
            jmp dword ptr [ThrowBattery::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> ThrowBattery::on_initialize() {
    ThrowBattery::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    ThrowBattery::mAddBattery = g_framework->get_module().as<uintptr_t>() + 0x3E1430;
    ThrowBattery::dAppearLog = g_framework->get_module().as<uintptr_t>() + 0x40F0D0;
    ThrowBattery::Play3D = g_framework->get_module().as<uintptr_t>() + 0x5F3650;
    ThrowBattery::mSetSuccessThrow = g_framework->get_module().as<uintptr_t>() + 0xA0D30;

    if (!install_hook_offset(0x3CF3F0, m_hook1, &detour1, &ThrowBattery::jmp_ret1, 5)) {
        spdlog::error("Failed to init ThrowBattery mod\n");
        return "Failed to init ThrowBattery mod";
    }

    return Mod::on_initialize();
}

void ThrowBattery::on_draw_ui() {
    ImGui::Checkbox("Gain Battery from throws", &mod_enabled);
    if (mod_enabled) {
        ImGui::Indent();
        ImGui::Text("Custom Battery Amount");
        ImGui::SliderInt("##CustomScreenshakeAmountSliderInt", &batteryThrowReward, 0, 200);
        help_marker("120 Default");
        ImGui::Unindent();
    }
}

// during load
void ThrowBattery::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("getBatteryFromThrows").value_or(false);
    batteryThrowReward = cfg.get<int>("batteryThrowReward").value_or(120);
}
// during save
void ThrowBattery::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("getBatteryFromThrows", mod_enabled);
    cfg.set<int>("batteryThrowReward", batteryThrowReward);
}

// do something every frame
//void ThrowBattery::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ThrowBattery::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
