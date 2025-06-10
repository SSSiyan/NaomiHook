#include "ChargeSubsBattery.hpp"
#if 1
bool ChargeSubsBattery::mod_enabled = false;
uintptr_t ChargeSubsBattery::jmp_ret1 = NULL;
uintptr_t ChargeSubsBattery::mSubBattery = NULL;
int ChargeSubsBattery::BatterySubCounter = 0;

uintptr_t ChargeSubsBattery::jmp_ret2 = NULL;
uintptr_t detour2_je1 = NULL;
uintptr_t detour2_je2 = NULL;

uintptr_t ChargeSubsBattery::jmp_ret3 = NULL;

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
naked void detour2() { // mid sub battery func, skip mk3 compares if in certain moveids // player in esi
    __asm {
        // 
            cmp byte ptr [ChargeSubsBattery::mod_enabled], 0
            je originalcode

            cmp dword ptr [esi+0x18C], 167 // low charging
            je je_skipmk3
            cmp dword ptr [esi+0x18C], 168 // low charging pt2
            je je_skipmk3
            cmp dword ptr [esi+0x18C], 170 // high charge
            je je_skipmk3

            cmp dword ptr [esi+0x18C], 169 // low mid charge hit
            je je_skipmk3
            cmp dword ptr [esi+0x18C], 290 // low full charge hit
            je je_skipmk3
            cmp dword ptr [esi+0x18C], 291 // low full charge hit 2
            je je_skipmk3
            cmp dword ptr [esi+0x18C], 292 // low full charge hit 3
            je je_skipmk3

        originalcode:
            cmp eax, 9
            je je_orig
            jmp dword ptr [ChargeSubsBattery::jmp_ret2]

        je_skipmk3:
            jmp dword ptr [detour2_je1]

        je_orig:
            jmp dword ptr [detour2_je2]
    }
}

static constexpr float lowJustCharge = 1.250f;
static constexpr float lowJustChargeEnd = 1.263f;
static constexpr float lowHalfCharge = 1.300f;

static constexpr float highJustCharge = 0.875f;
static constexpr float highJustChargeEnd = 0.887f;
static constexpr float highHalfCharge = 0.900f;

static constexpr float halfChargeReduction = 0.8f;

naked void detour3() { // just charges // player in ecx
    __asm {
        // 
            cmp byte ptr [ChargeSubsBattery::mod_enabled], 0
            je originalcode

            cmp dword ptr [ecx+0x18C], ePcMtBtAtkChgUp
            je high_charge

        // low_charge:
            movss xmm4, [ecx+0x28F0+0x4C+0x4] // player->mSnd.pitchCharge.mCurValue
            comiss xmm4, [lowJustCharge]
            jb originalcode
            comiss xmm4, [lowJustChargeEnd]
            ja check_half_low_charge
            jmp retcode
            
        check_half_low_charge:
            comiss xmm4, [lowHalfCharge]
            jae originalcode // default, divide battery by 0.25
            cvtsi2ss xmm0, eax
            mulss xmm0, [halfChargeReduction]
            cvttss2si eax, xmm0
            jmp originalcode

        high_charge:
            movss xmm4, [ecx+0x28F0+0x4C+0x4] // player->mSnd.pitchCharge.mCurValue
            comiss xmm4, [highJustCharge]
            jb originalcode
            comiss xmm4, [highJustChargeEnd]
            ja check_half_high_charge
            jmp retcode
            
        check_half_high_charge:
            comiss xmm4, [highHalfCharge]
            jae originalcode // default, divide battery by 0.25
            cvtsi2ss xmm0, eax
            mulss xmm0, [halfChargeReduction]
            cvttss2si eax, xmm0
            jmp originalcode
            
        originalcode:
            push eax
            call dword ptr [ChargeSubsBattery::mSubBattery] // fucks eax, edx
        retcode:
            jmp dword ptr [ChargeSubsBattery::jmp_ret3]
    }
}
 // clang-format on

std::optional<std::string> ChargeSubsBattery::on_initialize() {
    ChargeSubsBattery::mSubBattery = g_framework->get_module().as<uintptr_t>() + 0x3C2A80;
    if (!install_hook_offset(0x3C57E7, m_hook1, &detour1, &ChargeSubsBattery::jmp_ret1, 6)) {
        spdlog::error("Failed to init ChargeSubsBattery mod\n");
        return "Failed to init ChargeSubsBattery mod";
    }

    detour2_je1 = g_framework->get_module().as<uintptr_t>() + 0x3C2A9F;
    detour2_je2 = g_framework->get_module().as<uintptr_t>() + 0x3C2AE6;
    if (!install_hook_offset(0x3C2A95, m_hook2, &detour2, &ChargeSubsBattery::jmp_ret2, 5)) {
        spdlog::error("Failed to init ChargeSubsBattery 2 mod\n");
        return "Failed to init ChargeSubsBattery 2 mod";
    }

    if (!install_hook_offset(0x3CB921, m_hook3, &detour3, &ChargeSubsBattery::jmp_ret3, 6)) { // just charges
        spdlog::error("Failed to init ChargeSubsBattery 2 mod\n");
        return "Failed to init ChargeSubsBattery 2 mod";
    }
    return Mod::on_initialize();
}

void ChargeSubsBattery::on_draw_ui() {
    ImGui::Checkbox("Charging Uses Battery", &mod_enabled);
    /*if (mod_enabled) {
        ImGui::Text("How many ticks required to sub 1 battery");
        ImGui::SliderInt("##customBatterySubSliderInt", &subWhenOver, 0, 10);
        help_marker("Default 0");
    }*/
}

// during load
void ChargeSubsBattery::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("charge_subs_battery").value_or(false);
}
// during save
void ChargeSubsBattery::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("charge_subs_battery", mod_enabled);
}

// do something every frame
//void ChargeSubsBattery::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ChargeSubsBattery::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif