#include "Cheats.hpp"
#if 1
bool Cheats::take_no_damage = false;
bool Cheats::deal_no_damage = false;
bool Cheats::spend_no_battery = false;
bool Cheats::enemies_dont_attack = false;
bool Cheats::invincible = false; // DodgeSettings handles this

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

std::optional<std::string> Cheats::on_initialize() {
    return Mod::on_initialize();
}

void Cheats::on_draw_ui() {
    ImGui::Checkbox("Invincible", &invincible);

    if (ImGui::Checkbox("Take No Damage", &take_no_damage)) {
        toggleTakeNoDamage(take_no_damage);
    }

    if (ImGui::Checkbox("Deal No Damage", &deal_no_damage)) {
        toggleDealNoDamage(deal_no_damage);
    }
    help_marker("Lethal Throws, Deathblows, and Charged Slashes can still kill enemies who don't have Endurance.");

    if (ImGui::Checkbox("Spend No Battery", &spend_no_battery)) {
        toggleSpendNoBattery(spend_no_battery);
    }

    if (ImGui::Checkbox("Enemies Don't Attack", &enemies_dont_attack)) {
        toggleEnemiesDontAttack(enemies_dont_attack);
    }

    float combo_width = ImGui::CalcItemWidth();
    if (ImGui::Button("Start 777", ImVec2(combo_width, NULL))) {
        nmh_sdk::Start777();
    }
    //static bool dont_call_stencil = false;
    //static int inTick = 0;
    if (ImGui::Button("Start Bar", ImVec2(combo_width, NULL))) {
        //nmh_sdk::StartBar(dont_call_stencil, inTick);
        nmh_sdk::StartBar(false, 0);
    }
    //ImGui::SameLine();
    //ImGui::Checkbox("Dont Stencil", &dont_call_stencil);
    //ImGui::SameLine();
    //ImGui::PushItemWidth(120);
    //ImGui::InputInt("inTick", &inTick);
    //ImGui::PopItemWidth();
    if (ImGui::Button("Start Bell", ImVec2(combo_width, NULL))) {
        nmh_sdk::StartBell();
    }
    if (ImGui::Button("Start Hopper", ImVec2(combo_width, NULL))) {
        nmh_sdk::StartHopper();
    }
    if (ImGui::Button("Start Cherry", ImVec2(combo_width, NULL))) {
        nmh_sdk::StartCherry();
    }
}

// during load
void Cheats::on_config_load(const utility::Config &cfg) {
    take_no_damage = cfg.get<bool>("take_no_damage").value_or(false);
    toggleTakeNoDamage(take_no_damage);

    deal_no_damage = cfg.get<bool>("deal_no_damage").value_or(false);
    toggleDealNoDamage(deal_no_damage);

    spend_no_battery = cfg.get<bool>("spend_no_battery").value_or(false);
    toggleSpendNoBattery(spend_no_battery);

    enemies_dont_attack = cfg.get<bool>("enemies_dont_attack").value_or(false);
    toggleEnemiesDontAttack(enemies_dont_attack);

    invincible = cfg.get<bool>("invincible").value_or(false);
}
// during save
void Cheats::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("take_no_damage", take_no_damage);

    cfg.set<bool>("deal_no_damage", deal_no_damage);

    cfg.set<bool>("spend_no_battery", spend_no_battery);

    cfg.set<bool>("enemies_dont_attack", enemies_dont_attack);

    cfg.set<bool>("invincible", invincible);
}

// do something every frame
//void Cheats::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void Cheats::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
