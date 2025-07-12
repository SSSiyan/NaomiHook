#include "Cheats.hpp"
#include <unordered_set>
#include "../Config.hpp" // for CONFIG_FILENAME
#include "ReprisalSwap.hpp"
#if 1
bool Cheats::take_no_damage = false;
bool Cheats::deal_no_damage = false;
bool Cheats::one_hit_kill = false;
uintptr_t Cheats::damage_modifier_jmp_ret = NULL;
bool Cheats::spend_no_battery = false;
bool Cheats::enemies_dont_attack = false;
bool Cheats::invincible = false; // DodgeSettings handles this

const char* Cheats::defaultDescription = "Cheats";
const char* Cheats::hoveredDescription = defaultDescription;

static char password_input[64] = "";
std::unordered_set<std::string> unlocked_cheats;
std::unordered_map<std::string, std::string> cheat_passwords = {
    {"invincible", "SUNDOWNER"},
    {"take_no_damage", "HESOYAM"},
    {"deal_no_damage", "JUSTAPRANK"},
    {"one_hit_kill", "FUCKHEAD"},
    {"spend_no_battery", "BANDANA"},
    {"enemies_dont_attack", "LEAVEMEALONE"},
    {"start_777", "JACKPOT"},
    {"start_bar", "WINDOWS"},
    {"start_bell", "BELLEND"},
    {"start_hopper", "GYARU"},
    {"start_cherry", "MRWHOOPEE"}
};

void save_unlocked_cheats() {
    utility::Config cfg;
    cfg.load(CONFIG_FILENAME);
        
    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = unlocked_cheats.find(pair.first) != unlocked_cheats.end();
        cfg.set<bool>("unlocked_" + pair.first, is_unlocked);
    }
    
    cfg.save(CONFIG_FILENAME);
}

void check_password() {
    std::string upper_input = std::string(password_input);
    std::transform(upper_input.begin(), upper_input.end(), upper_input.begin(), ::toupper);
        
    for (const auto& pair : cheat_passwords) {
        if (pair.second == upper_input) {
            unlocked_cheats.insert(pair.first);
            memset(password_input, 0, sizeof(password_input));
                
            save_unlocked_cheats();
            return;
        }
    }
}

bool is_cheat_unlocked(const std::string& cheat_name) {
    // NOTE(deep): too lazy to type those every time i juggle configs
#ifndef NDEBUG
        return true;
#endif

    return unlocked_cheats.find(cheat_name) != unlocked_cheats.end();
}

void Cheats::toggleTakeNoDamage(bool enable) {
    if (enable) {
        install_patch_offset(0x3D680D, patchTakeNoDamage, "\x90\x90\x90\x90", 4); // nop 4
    }
    else {
        install_patch_offset(0x3D680D, patchTakeNoDamage, "\xF3\x0F\x5C\xC1", 4); // subss xmm0,xmm1
    }
}

void Cheats::toggleDealNoDamage(bool enable) {
    if (enable) {
        install_patch_offset(0xA4165, patchDealNoDamage, "\x90\x90\x90\x90", 4); // nop 4
    }
    else {
        install_patch_offset(0xA4165, patchDealNoDamage, "\xF3\x0F\x5C\xC1", 4); // subss xmm0,xmm1
    }
}

// clang-format off
static constexpr float oneHitKillDamage = 9999.0f;
static constexpr float reprisalDamageModifier = 0.1f;
naked void detour_damage_modifier() { 
    __asm {
        cmp byte ptr [Cheats::one_hit_kill], 1
        je oneHitKill
        cmp byte ptr [ReprisalSwap::mod_enabled], 1
        je reprisalMoveIDCheck
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 1
        je reprisalMoveIDCheck
        jmp originalcode

        reprisalMoveIDCheck:
        cmp byte ptr [edi+0x1707], 1 // justAttack
        jne originalcode
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChgUp
        je checkHighReprisalCheatTicked
        cmp dword ptr [edi+0x18C], ePcMtBtAtkChg
        je checkMidReprisalCheatTicked
        jmp originalcode

    checkHighReprisalCheatTicked:
        cmp byte ptr [ReprisalSwap::mod_enabled], 1
        je newReprisalDamage
        jmp originalcode

    checkMidReprisalCheatTicked:
        cmp byte ptr [ReprisalSwap::mid_stance_enabled], 1
        je newReprisalDamage
        jmp originalcode

    oneHitKill:
        movss xmm0, [oneHitKillDamage]
        jmp originalcode
    newReprisalDamage:
        mulss xmm0, [reprisalDamageModifier]
    originalcode:
        movss [esp],xmm0
    retcode:
        jmp dword ptr [Cheats::damage_modifier_jmp_ret]
    }
}
// clang-format on

void Cheats::toggleSpendNoBattery(bool enable) {
    if (enable) {
        install_patch_offset(0x3C2AAF, patchSpendNoBattery, "\x90\x90\x90\x90\x90\x90\x90", 7); // nop 7
    }
    else {
        install_patch_offset(0x3C2AAF, patchSpendNoBattery, "\x66\x29\x81\x14\x05\x00\x00", 7); // sub [ecx+00000514],ax
    }
}

void Cheats::toggleEnemiesDontAttack(bool enable) {
    if (enable) {
        install_patch_offset(0x43536E, patchEnemiesDontAttack, "\x90\x90", 2); // nop 2
    }
    else {
        install_patch_offset(0x43536E, patchEnemiesDontAttack, "\x7A\x1C", 2); // jp nmh.HRZAKO::mFrameProc+5C
    }
}

void Cheats::render_description() const {
    ImGui::TextWrapped(Cheats::hoveredDescription);
}

void Cheats::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) Cheats::hoveredDescription = defaultDescription;
    
    ImGui::Text("Enter cheat password:");
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputText("##password", password_input, sizeof(password_input), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsUppercase)) {
        check_password();
    }
    ImGui::SameLine();
    if (ImGui::Button("Submit")) {
        check_password();
    }
    /*if (ImGui::Button("Clear all unlocked cheats")) {
        unlocked_cheats.clear();
        save_unlocked_cheats();
    }*/
    ImGui::Separator();
    
    if (is_cheat_unlocked("invincible")) {
        ImGui::Checkbox("Invincible", &invincible);
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = "Disable the player taking damage and animating when hit";
    }
    
    if (is_cheat_unlocked("take_no_damage")) {
        if (ImGui::Checkbox("Take No Damage", &take_no_damage)) {
            toggleTakeNoDamage(take_no_damage);
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = "Take no damage, but still receive hit effects";
    }
    
    if (is_cheat_unlocked("deal_no_damage")) {
        if (ImGui::Checkbox("Deal No Damage", &deal_no_damage)) {
            toggleDealNoDamage(deal_no_damage);
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = "Lethal Throws, Deathblows, Jumping Slash, and Charged Slashes can still kill enemies who don't have Endurance.";
    }

    if (is_cheat_unlocked("one_hit_kill")) {
        ImGui::Checkbox("One Hit Kills", &one_hit_kill);
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = "One hit kill all enemies";
    }
    
    if (is_cheat_unlocked("spend_no_battery")) {
        if (ImGui::Checkbox("Infinite Battery", &spend_no_battery)) {
            toggleSpendNoBattery(spend_no_battery);
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    if (is_cheat_unlocked("enemies_dont_attack")) {
        if (ImGui::Checkbox("Enemies Don't Attack", &enemies_dont_attack)) {
            toggleEnemiesDontAttack(enemies_dont_attack);
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    float combo_width = ImGui::CalcItemWidth();
    
    if (is_cheat_unlocked("start_777")) {
        if (ImGui::Button("Activate Anarchy In The Galaxy", ImVec2(combo_width, NULL))) {
            nmh_sdk::Start777();
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    if (is_cheat_unlocked("start_bar")) {
        if (ImGui::Button("Activate Cranberry Chocolate Sundae", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartBar(false, 0);
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    if (is_cheat_unlocked("start_bell")) {
        if (ImGui::Button("Activate Blueberry Cheese Brownie", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartBell();
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    if (is_cheat_unlocked("start_hopper")) {
        if (ImGui::Button("Activate Strawberry on The Shortcake", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartHopper();
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
    
    if (is_cheat_unlocked("start_cherry")) {
        if (ImGui::Button("Activate Cherries", ImVec2(combo_width, NULL))) {
            nmh_sdk::StartCherry();
        }
        if (ImGui::IsItemHovered()) Cheats::hoveredDescription = defaultDescription;
    }
}

std::optional<std::string> Cheats::on_initialize() {
    if (!install_hook_offset(0x3CB82F, damage_modifier_hook, &detour_damage_modifier, &Cheats::damage_modifier_jmp_ret, 5)) {
        spdlog::error("Failed to init DamageModifier mod\n");
        return "Failed to init DamageModifier mod";
    }

    return Mod::on_initialize();
}

void Cheats::on_config_load(const utility::Config &cfg) {
    take_no_damage = cfg.get<bool>("take_no_damage").value_or(false);
    if (take_no_damage) toggleTakeNoDamage(take_no_damage);
    deal_no_damage = cfg.get<bool>("deal_no_damage").value_or(false);
    if (deal_no_damage) toggleDealNoDamage(deal_no_damage);
    one_hit_kill = cfg.get<bool>("one_hit_kill").value_or(false);
    spend_no_battery = cfg.get<bool>("spend_no_battery").value_or(false);
    if (spend_no_battery) toggleSpendNoBattery(spend_no_battery);
    enemies_dont_attack = cfg.get<bool>("enemies_dont_attack").value_or(false);
    if (enemies_dont_attack) toggleEnemiesDontAttack(enemies_dont_attack);
    invincible = cfg.get<bool>("invincible").value_or(false);
    
    unlocked_cheats.clear();
    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = cfg.get<bool>("unlocked_" + pair.first).value_or(false);
        if (is_unlocked) {
            unlocked_cheats.insert(pair.first);
        }
    }
}

void Cheats::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("take_no_damage", take_no_damage);
    cfg.set<bool>("deal_no_damage", deal_no_damage);
    cfg.set<bool>("one_hit_kill", one_hit_kill);
    cfg.set<bool>("spend_no_battery", spend_no_battery);
    cfg.set<bool>("enemies_dont_attack", enemies_dont_attack);
    cfg.set<bool>("invincible", invincible);
    
    for (const auto& pair : cheat_passwords) {
        bool is_unlocked = unlocked_cheats.find(pair.first) != unlocked_cheats.end();
        cfg.set<bool>("unlocked_" + pair.first, is_unlocked);
    }
}

// do something every frame
//void Cheats::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void Cheats::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
