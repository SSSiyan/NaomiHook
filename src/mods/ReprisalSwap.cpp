#include "ReprisalSwap.hpp"
#include "StanceControl.hpp"
#include "ChargeSubsBattery.hpp"
#if 1
bool ReprisalSwap::mod_enabled = false;
bool ReprisalSwap::mid_stance_enabled = false;
uintptr_t ReprisalSwap::jmp_ret1 = NULL;
uintptr_t ReprisalSwap::gpPadUni = NULL;

uintptr_t ReprisalSwap::jmp_ret2 = NULL;
uintptr_t ReprisalSwap::detour2je = NULL;

// clang-format off
naked void detour1() { // player in ecx
    __asm {
        cmp dword ptr [ecx+0x1350], 2 // mid
        jne highchargecheck
        mov eax, ePcMtBtAtkChg // mid charge
        jmp originalcode

    highchargecheck:
        cmp byte ptr [ReprisalSwap::mod_enabled], 0
        je originalcode

    // if any 3 stance mod is enabled, don't check for input
        cmp dword ptr [ecx+0x1350], 0 // high
        jne originalcode // this leaves only low
        cmp byte ptr [StanceControl::mod_enabled], 1
        je skipInputCheck
        cmp byte ptr [StanceControl::mod_enabled_gear_system], 1
        je skipInputCheck

    // no 3 stance mods are enabled, check for input
        push eax
        mov eax, [ReprisalSwap::gpPadUni]
        mov eax, [eax]
        cmp byte ptr [eax+0x1CC], 1 // high attack // From nmh.PC_INPUT_ATTACK+99
        pop eax
        jne originalcode
    skipInputCheck:
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 0
        je highCharge
    // midCheck:

    highcharge:
        mov eax, ePcMtBtAtkChgUp // high charge
    originalcode:
        push eax // default eax
        call esi
        movss xmm3, [edi+0x40]
        jmp dword ptr [ReprisalSwap::jmp_ret1]
    }
}

naked void detour2() { // disable clashes on new reprisals // disable clashes on charges when Charging Slashes cheat is enabled // player in edi
    __asm {
        cmp byte ptr [ReprisalSwap::mod_enabled], 0
        je check2
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChg
        je jmp_je
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChgUp
        je jmp_je
    check2:
        cmp byte ptr [ChargeSubsBattery::mod_enabled], 1
        jne originalcode
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChg // 169 Mid Charged Slash
        je jmp_je
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChgUp // 170 High Charged Slash
        je jmp_je
    originalcode:
        cmp byte ptr [edi+0x00001664], 00
        jmp dword ptr [ReprisalSwap::jmp_ret2]

    jmp_je:
        jmp dword ptr [ReprisalSwap::detour2je]
    }
}
 // clang-format on

std::optional<std::string> ReprisalSwap::on_initialize() {
    gpPadUni = g_framework->get_module().as<uintptr_t>() + 0x843588; 
    if (!install_hook_offset(0x3CE22E, m_hook1, &detour1, &ReprisalSwap::jmp_ret1, 8)) {
        spdlog::error("Failed to init ReprisalSwap mod\n");
        return "Failed to init ReprisalSwap mod";
    }
    detour2je = g_framework->get_module().as<uintptr_t>() + 0x3CAB6F;
    if (!install_hook_offset(0x3CAB55, m_hook2, &detour2, &ReprisalSwap::jmp_ret2, 7)) {
        spdlog::error("Failed to init ReprisalSwap mod 2\n");
        return "Failed to init ReprisalSwap mod 2";
    }
    return Mod::on_initialize();
}

void ReprisalSwap::render_description() const {
    ImGui::TextWrapped(ReprisalSwap::hoveredDescription);
}

const char* ReprisalSwap::defaultDescription = "Reworks parry reprisals by assigning a new attack to each stance. Restores the original Low charge attack and repurposes it as the Mid charge.";
const char* ReprisalSwap::hoveredDescription = defaultDescription;

void ReprisalSwap::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) ReprisalSwap::hoveredDescription = defaultDescription;

    ImGui::Checkbox("High Charge Reprisal", &mod_enabled); // high is pressed after gold block
    if (ImGui::IsItemHovered()) ReprisalSwap::hoveredDescription = "Changes the parry reprisal to HIGH Charged Slash when performed in HIGH stance.";

    ImGui::Checkbox("Mid Charge Reprisal", &mid_stance_enabled); // high is pressed after gold block while in mid stance
    if (ImGui::IsItemHovered()) ReprisalSwap::hoveredDescription = "Changes the parry reprisal to MID Charged Slash when performed in MID stance.";
}

// during load
void ReprisalSwap::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("reprisal_swap").value_or(false);
    mid_stance_enabled = cfg.get<bool>("reprisal_mid_stance").value_or(false);
}
// during save
void ReprisalSwap::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("reprisal_swap", mod_enabled);
    cfg.set<bool>("reprisal_mid_stance", mid_stance_enabled);
}

// do something every frame
//void ReprisalSwap::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ReprisalSwap::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
