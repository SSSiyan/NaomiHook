#include "LockOnSettings.hpp"

#if 1
const char* LockOnSettings::defaultDescription = "Lock On Settings";
const char* LockOnSettings::hoveredDescription = defaultDescription;

bool      LockOnSettings::lockon_more_actions = false;
bool      LockOnSettings::lockon_deathblows = false;
bool      LockOnSettings::lockon_deathblows_start = false;
bool      LockOnSettings::lockon_parry_qtes = false;
//////////////////////////////////////////////////
uintptr_t LockOnSettings::horizontal_limit_jmp_ret = NULL;
bool      LockOnSettings::horizontal_limit_toggle = false;
float     LockOnSettings::horizontal_limit_default_search_degrees = 1.57f;
float     LockOnSettings::horizontal_limit_custom_search_degrees = 0.0f;
//////////////////////////////////////////////////
uintptr_t LockOnSettings::more_actions_jmp_ret = NULL;
uintptr_t LockOnSettings::more_actions_jmp_ja = NULL;
uintptr_t LockOnSettings::more_actions_jmp_retalt = NULL;
//////////////////////////////////////////////////
bool      LockOnSettings::disable_throws_mod_enabled = false;
uintptr_t LockOnSettings::disable_throws_jmp_ret1 = NULL;
uintptr_t LockOnSettings::disable_throws_jmp_jne1 = NULL;

uintptr_t LockOnSettings::disable_throws_jmp_ret2 = NULL;
uintptr_t LockOnSettings::disable_throws_jmp_jne2 = NULL;
float     LockOnSettings::disable_throws_comiss2 = 0.0f;
//////////////////////////////////////////////////
void LockOnSettings::toggle_deathblow_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4429, patch_toggle_deathblow_during, "\xEB\x58", 2); // jmp nmh.exe+3C4483
    }
    else {
        install_patch_offset(0x3C4429, patch_toggle_deathblow_during, "\x75\x58", 2); // jne nmh.exe+3C4483
    }
}

void LockOnSettings::toggle_deathblow_lockon_start(bool enable) {
    if (enable) {
        install_patch_offset(0x3D08E5, patch_toggle_deathblow_start, "\x90\x90\x90\x90\x90", 5); // 
    }
    else {
        install_patch_offset(0x3D08E5, patch_toggle_deathblow_start, "\xE8\xF6\x44\xFF\xFF", 5); // 
    }
}

void LockOnSettings::toggle_parry_qte_lockon(bool enable) {
    if (enable) {
        install_patch_offset(0x3C4268, patch_parry_qte_lockon, "\xE9\x2F\x06\x00\x00\x90", 6); // jmp nmh.mHRPc::mUpdateLockOnTarget+66C
    }
    else {
        install_patch_offset(0x3C4268, patch_parry_qte_lockon, "\x0F\x84\x2E\x06\x00\x00", 6); // je nmh.mHRPc::mUpdateLockOnTarget+66C
    }
}

// clang-format off
naked void detour_horizontal_limit() { // Horizontal Limit // player in ecx
    __asm {
        // 
            cmp byte ptr [LockOnSettings::horizontal_limit_toggle], 0
            je originalcode
        // 
            movss xmm0, [LockOnSettings::horizontal_limit_custom_search_degrees]
            jmp retcode

        originalcode:
            movss xmm0, [LockOnSettings::horizontal_limit_default_search_degrees]
        retcode:
            jmp dword ptr [LockOnSettings::horizontal_limit_jmp_ret]
    }
}

naked void detour_more_actions_lockon() { // Lock on during more actions
    __asm {
        // 
            cmp byte ptr [LockOnSettings::lockon_more_actions], 0
            je originalcode
        // 
            //lea eax,[edi-0x1B]
            //cmp eax, 06
            //ja jacode
            jmp qword ptr [LockOnSettings::more_actions_jmp_retalt]

        originalcode:
            lea eax,[edi-0x1C]
            cmp eax, 06
            ja jacode
            jmp dword ptr [LockOnSettings::more_actions_jmp_ret]

        jacode:
            jmp dword ptr [LockOnSettings::more_actions_jmp_ja]
    }
}

naked void disable_throws_detour1() { // player in ecx
    __asm {
        // 
            cmp byte ptr [LockOnSettings::disable_throws_mod_enabled], 0
            je originalcode

            test ecx, ecx // probably isn't necessary but lets play it safe
            je originalcode
            cmp dword ptr [ecx+0x2b60], 0
            jne jnecode // if locking on, ret
       
        originalcode:
            mov eax, [edi+0x00002FA8]
            jmp dword ptr [LockOnSettings::disable_throws_jmp_ret1]

        jnecode:
            jmp dword ptr [LockOnSettings::disable_throws_jmp_jne1]
    }
}
 
naked void disable_throws_detour2() { // player in ebx
    __asm {
        // 
            cmp byte ptr [LockOnSettings::disable_throws_mod_enabled], 0
            je originalcode

            cmp dword ptr [ebx+0x2b60], 0
            jne jnecode
       
        originalcode:
            comiss xmm0, [LockOnSettings::disable_throws_comiss2] // 0.0f
            jmp dword ptr [LockOnSettings::disable_throws_jmp_ret2]

        jnecode:
            jmp dword ptr [LockOnSettings::disable_throws_jmp_jne2]
    }
}
 // clang-format on

std::optional<std::string> LockOnSettings::on_initialize() {
    if (!install_hook_offset(0x3C4B90, lockon_horizontal_limit_hook, &detour_horizontal_limit, &LockOnSettings::horizontal_limit_jmp_ret, 8)) { // Horizontal Limit
        spdlog::error("Failed to init LockOnSettings mod\n");
        return "Failed to init LockOnSettings mod";
    }
    more_actions_jmp_retalt = g_framework->get_module().as<uintptr_t>() + 0x3C43C0; // 0x3C4483
    more_actions_jmp_ja = g_framework->get_module().as<uintptr_t>() + 0x3C42BB;
    if (!install_hook_offset(0x3C42A5, lockon_during_more_actions_hook, &detour_more_actions_lockon, &LockOnSettings::more_actions_jmp_ret, 8)) {
        spdlog::error("Failed to init LockOnSettings mod\n");
        return "Failed to init LockOnSettings mod";
    }

    LockOnSettings::disable_throws_jmp_jne1 = g_framework->get_module().as<uintptr_t>() + 0x3C804A;
    if (!install_hook_offset(0x3C7F1F, disable_throws_hook1, &disable_throws_detour1, &LockOnSettings::disable_throws_jmp_ret1, 6)) {
        spdlog::error("Failed to init LockOnDisablesThrows mod\n");
        return "Failed to init LockOnDisablesThrows mod";
    }
    LockOnSettings::disable_throws_jmp_jne2 = g_framework->get_module().as<uintptr_t>() + 0x3C5545;
    if (!install_hook_offset(0x3C54EF, disable_throws_hook2, &disable_throws_detour2, &LockOnSettings::disable_throws_jmp_ret2, 7)) {
        spdlog::error("Failed to init LockOnDisablesThrows mod\n");
        return "Failed to init LockOnDisablesThrows mod";
    }

    return Mod::on_initialize();
}

static void toggle_kick_cancel(bool enable) {
    static uintptr_t gPcCommonTable = g_framework->get_module().as<uintptr_t>() + 0x7421E0;
    static float* OverheadKickAddr = (float*)(gPcCommonTable + 0x13F0);
    static DWORD oldProtect;
    VirtualProtect(OverheadKickAddr, sizeof(float), PAGE_READWRITE, &oldProtect);
    if (enable) {
        *OverheadKickAddr = 42.0f;
    }
    else {
        *OverheadKickAddr = 999.0f;
    }
    VirtualProtect(OverheadKickAddr, sizeof(float), oldProtect, &oldProtect);
}

void LockOnSettings::render_description() const {
    ImGui::TextWrapped(LockOnSettings::hoveredDescription);
}

void LockOnSettings::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) LockOnSettings::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Enable lockon during more actions", &lockon_more_actions);
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";

    if (ImGui::Checkbox("Full Lockon Freedom During Deathblows", &lockon_deathblows)) {
        toggle_deathblow_lockon(lockon_deathblows);
    }
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";

    if (ImGui::Checkbox("Disable Deathblows removing your lockon", &lockon_deathblows_start)) {
        toggle_deathblow_lockon_start(lockon_deathblows_start);
    }
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";

    if (ImGui::Checkbox("Disable Parry QTEs removing your lockon", &lockon_parry_qtes)) {
        toggle_parry_qte_lockon(lockon_parry_qtes);
    }
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";

    ImGui::Checkbox("Custom Target Switch Horizontal Limit", &horizontal_limit_toggle);
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";
    if (horizontal_limit_toggle) {
        ImGui::Indent();
        ImGui::SliderAngle("##CustomSearchDegreesSliderFloat", &horizontal_limit_custom_search_degrees, 0.0f, 360.0f, "%.0f");
        if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "90 degrees default";
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Lockon Disables Throws", &disable_throws_mod_enabled)) {
        toggle_kick_cancel(disable_throws_mod_enabled);
    }
    if (ImGui::IsItemHovered()) LockOnSettings::hoveredDescription = "@DHMalice";
}

// during load
void LockOnSettings::on_config_load(const utility::Config &cfg) {
    lockon_deathblows = cfg.get<bool>("lockon_deathblows").value_or(false);
    if (lockon_deathblows) toggle_deathblow_lockon(lockon_deathblows);
    lockon_deathblows_start = cfg.get<bool>("lockon_deathblows_start").value_or(false);
    if (lockon_deathblows_start) toggle_deathblow_lockon_start(lockon_deathblows_start);
    lockon_parry_qtes = cfg.get<bool>("lockon_parry_qtes").value_or(false);
    if (lockon_parry_qtes) toggle_parry_qte_lockon(lockon_parry_qtes);

    lockon_more_actions = cfg.get<bool>("more_lockon_actions").value_or(false);
    horizontal_limit_toggle = cfg.get<bool>("target_switch_degrees_toggle").value_or(false);
    horizontal_limit_custom_search_degrees = cfg.get<float>("custom_search_degrees").value_or(1.57f);

    disable_throws_mod_enabled = cfg.get<bool>("lockon_disables_throws").value_or(false);
    if (disable_throws_mod_enabled) toggle_kick_cancel(disable_throws_mod_enabled);
}

// during save
void LockOnSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("lockon_deathblows", lockon_deathblows);
    cfg.set<bool>("lockon_deathblows_start", lockon_deathblows_start);
    cfg.set<bool>("lockon_parry_qtes", lockon_parry_qtes);

    cfg.set<bool>("more_lockon_actions", lockon_more_actions);
    cfg.set<bool>("target_switch_degrees_toggle", horizontal_limit_toggle);
    cfg.set<float>("custom_search_degrees", horizontal_limit_custom_search_degrees);

    cfg.set<bool>("lockon_disables_throws", disable_throws_mod_enabled);
}

// do something every frame
//void DisableCrashDumps::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void DisableCrashDumps::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
