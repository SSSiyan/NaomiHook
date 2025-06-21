#include "ReprisalSwap.hpp"
#if 1
bool ReprisalSwap::mod_enabled = false;
bool ReprisalSwap::mid_stance_enabled = false;
uintptr_t ReprisalSwap::jmp_ret1 = NULL;
uintptr_t ReprisalSwap::gpPadUni = NULL;

// clang-format off
naked void detour1() { // player in ecx
    __asm {
        cmp byte ptr [ReprisalSwap::mod_enabled], 0
        je originalcode

        push eax
        mov eax, [ReprisalSwap::gpPadUni]
        mov eax, [eax]
        cmp byte ptr [eax+0x1CC], 1 // high attack // From nmh.PC_INPUT_ATTACK+99
        pop eax
        jne originalcode
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 0
        je highCharge
    // midCheck:
        cmp [ecx+0x1350], 2 // mid
        jne highcharge
        mov eax, ePcMtBtAtkChg // mid charge
        jmp originalcode

    highcharge:
        mov eax, ePcMtBtAtkChgUp // high charge
    originalcode:
        push eax // default eax
        call esi
        movss xmm3, [edi+0x40]
        jmp dword ptr [ReprisalSwap::jmp_ret1]
    }
}
 // clang-format on

std::optional<std::string> ReprisalSwap::on_initialize() {
    gpPadUni = g_framework->get_module().as<uintptr_t>() + 0x843588; 
    if (!install_hook_offset(0x3CE22E, m_hook1, &detour1, &ReprisalSwap::jmp_ret1, 8)) {
        spdlog::error("Failed to init ReprisalSwap mod\n");
        return "Failed to init ReprisalSwap mod";
    }
    return Mod::on_initialize();
}

void ReprisalSwap::render_description() const {
    ImGui::TextWrapped(ReprisalSwap::hoveredDescription);
}

const char* ReprisalSwap::defaultDescription = "@DHMALICE";
const char* ReprisalSwap::hoveredDescription = defaultDescription;

void ReprisalSwap::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered())
        ReprisalSwap::hoveredDescription = defaultDescription;
    ImGui::Checkbox("High Attack Reprisal", &mod_enabled); // high is pressed after gold block
    if (ImGui::IsItemHovered())
        ReprisalSwap::hoveredDescription = "@DHMALICE";
    ImGui::Checkbox("Mid Stance Reprisal", &mid_stance_enabled); // high is pressed after gold block while in mid stance
    if (ImGui::IsItemHovered())
        ReprisalSwap::hoveredDescription = "@DHMALICE";
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
