#include "ChargeSubsBattery.hpp"
#if 1
bool ChargeSubsBattery::mod_enabled = false;
uintptr_t ChargeSubsBattery::jmp_ret1 = NULL;
uintptr_t ChargeSubsBattery::mSubBattery = NULL;
int ChargeSubsBattery::BatterySubCounter = 0;
int ChargeSubsBattery::subWhenOver = 0;

// clang-format off
naked void detour1() { // ticks when effect starts // player in ebx
    __asm {
        // 
            cmp byte ptr [ChargeSubsBattery::mod_enabled], 0
            je originalcode

            push eax
            mov eax, [ChargeSubsBattery::BatterySubCounter]
            cmp eax, [ChargeSubsBattery::subWhenOver]
            pop eax
            jae subBattery
            inc dword ptr [ChargeSubsBattery::BatterySubCounter]
            jmp originalcode

        subBattery:
            mov dword ptr [ChargeSubsBattery::BatterySubCounter], 0
            push eax
            push edx
            push ecx
            mov ecx, ebx // player
            push 1
            call dword ptr [ChargeSubsBattery::mSubBattery] // fucks eax, edx
            pop ecx
            pop edx
            pop eax
        originalcode:
            mov eax, [ebx+0x000012A4]
            jmp dword ptr [ChargeSubsBattery::jmp_ret1]
    }
}
 
 // clang-format on

std::optional<std::string> ChargeSubsBattery::on_initialize() {
    ChargeSubsBattery::mSubBattery = g_framework->get_module().as<uintptr_t>() + 0x3C2A80;
    if (!install_hook_offset(0x3C57E7, m_hook1, &detour1, &ChargeSubsBattery::jmp_ret1, 6)) {
        spdlog::error("Failed to init ChargeSubsBattery mod\n");
        return "Failed to init ChargeSubsBattery mod";
    }

    return Mod::on_initialize();
}

void ChargeSubsBattery::on_draw_ui() {
    ImGui::Checkbox("Charging Subtracts Battery", &mod_enabled);
    /*if (mod_enabled) {
        ImGui::Text("How many ticks required to sub 1 battery");
        ImGui::SliderInt("##customBatterySubSliderInt", &subWhenOver, 0, 10);
        help_marker("Default 0");
    }*/
}

// during load
void ChargeSubsBattery::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("charge_subs_battery").value_or(false);
    //subWhenOver = cfg.get<int>("customBatterySubAmount").value_or(0); // removed option so removing load too
}
// during save
void ChargeSubsBattery::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("charge_subs_battery", mod_enabled);
    //cfg.set<int>("customBatterySubAmount", subWhenOver);
}

// do something every frame
//void ChargeSubsBattery::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ChargeSubsBattery::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
