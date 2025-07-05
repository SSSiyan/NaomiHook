#include "HudSettings.hpp"

bool HudSettings::hide_drawbpber = false;
bool HudSettings::hide_drawhber = false;
bool HudSettings::hide_drawbattery = false;
bool HudSettings::hide_drawmoney = false;
bool HudSettings::hide_drawtension = false;
bool HudSettings::hide_drawkamae = false;
bool HudSettings::hide_drawspeedmeter = false;
bool HudSettings::hide_drawmap = false;
bool HudSettings::hide_drawsilvia = false;
bool HudSettings::hide_drawlmode = false;
bool HudSettings::hide_drawkeyguide = false;
bool HudSettings::hide_drawcheckpo = false;

bool HudSettings::custom_lock_on_colour_toggle = false;
uint8_t HudSettings::lockOnColour[3] = {0, 0, 0};

const char* HudSettings::defaultDescription = "@DHMalice";
const char* HudSettings::hoveredDescription = defaultDescription;

// clang-format off
/*naked void detour() {
    __asm {
        mov qword ptr [HudSettings::variable], rbx
        mov rax, 0xDEADBEEF
        jmp qword ptr [jmp_ret]
    }
}*/
// clang-format on

void HudSettings::DisplayHUDCheckboxes() {
    mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
    if (!mHRBattle) { return; }
    HrScreenStatus* hrScreenStatus = mHRBattle->mBtEffect.pScreenStatus;
    if (!hrScreenStatus) { return; }

    ImGui::SeparatorText("Display Toggles");

    if (ImGui::BeginTable("UIControlTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Force Override", ImGuiTableColumnFlags_WidthStretch);
        if (ImGui::IsItemHovered()) HudSettings::hoveredDescription = "@DHMalice";
        ImGui::TableSetupColumn("Game State", ImGuiTableColumnFlags_WidthStretch);
        if (ImGui::IsItemHovered()) HudSettings::hoveredDescription = "@DHMalice";
        ImGui::TableHeadersRow();

        const char* ui_names[] = {
            "drawbpber", "drawhber", "drawbattery", "drawmoney",
            "drawtension", "drawkamae", "drawspeedmeter", "drawmap",
            "drawsilvia", "drawlmode", "drawkeyguide", "drawcheckpo"
        };
        
        bool* force_flags[] = {
            &hide_drawbpber, &hide_drawhber, &hide_drawbattery, &hide_drawmoney,
            &hide_drawtension, &hide_drawkamae, &hide_drawspeedmeter, &hide_drawmap,
            &hide_drawsilvia, &hide_drawlmode, &hide_drawkeyguide, &hide_drawcheckpo
        };
        
        for (int i = 0; i < 12; i++) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Checkbox(("Hide " + std::string(ui_names[i])).c_str(), force_flags[i]);
            ImGui::TableSetColumnIndex(1);
            if (hrScreenStatus) {
                bool current_state = (hrScreenStatus->flag & (1 << i)) != 0;
                if (ImGui::Checkbox(("Show " + std::string(ui_names[i])).c_str(), &current_state)) {
                    if (current_state) {
                        hrScreenStatus->flag |= (1 << i);
                    } else {
                        hrScreenStatus->flag &= ~(1 << i);
                    }
                }
            }
        }
        ImGui::EndTable();
    }
}

void HudSettings::SetHUDFlagsOnFrame() {
    mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
    if (!mHRBattle) { return; }
    HrScreenStatus* hrScreenStatus = mHRBattle->mBtEffect.pScreenStatus;
    if (!hrScreenStatus) { return; }
    
    if (hide_drawbpber) hrScreenStatus->flag &= ~(1 << 0);
    if (hide_drawhber)  hrScreenStatus->flag &= ~(1 << 1);
    if (hide_drawbattery) hrScreenStatus->flag &= ~(1 << 2);
    if (hide_drawmoney) hrScreenStatus->flag &= ~(1 << 3);
    if (hide_drawtension) hrScreenStatus->flag &= ~(1 << 4);
    if (hide_drawkamae) hrScreenStatus->flag &= ~(1 << 5);
    if (hide_drawspeedmeter) hrScreenStatus->flag &= ~(1 << 6);
    if (hide_drawmap) hrScreenStatus->flag &= ~(1 << 7);
    if (hide_drawsilvia) hrScreenStatus->flag &= ~(1 << 8);
    if (hide_drawlmode) hrScreenStatus->flag &= ~(1 << 9);
    if (hide_drawkeyguide) hrScreenStatus->flag &= ~(1 << 10);
    if (hide_drawcheckpo) hrScreenStatus->flag &= ~(1 << 11);
}

void HudSettings::SetLockOnColourOnFrame() {
    if (!custom_lock_on_colour_toggle) { return; }
    mHRPc* player = nmh_sdk::get_mHRPc();
    auto lockOnNpc = player->mpLockOnNpc;
    if (!lockOnNpc) { return; }
    auto battleIcon = lockOnNpc->mEffect.pBattleIcon;
    if (!battleIcon) { return; }
    for (int i = 0; i < 3; i++) {
        battleIcon->m_blockcolor[i] = lockOnColour[i];
    }
}

// do something every frame
void HudSettings::on_frame() {
    HudSettings::SetHUDFlagsOnFrame();
    HudSettings::SetLockOnColourOnFrame();
}

void HudSettings::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) HudSettings::hoveredDescription = defaultDescription;
    DisplayHUDCheckboxes();

    ImGui::SeparatorText("Lock On");

    ImGui::Checkbox("Custom Lock On Colour", &custom_lock_on_colour_toggle);
    if (custom_lock_on_colour_toggle) {
        float colourFloat[3] = {
            lockOnColour[0] / 255.0f,
            lockOnColour[1] / 255.0f,
            lockOnColour[2] / 255.0f
        };

        if (ImGui::ColorPicker3("New Lock On Colour", colourFloat)) {
            lockOnColour[0] = (uint8_t)(colourFloat[0] * 255.0f);
            lockOnColour[1] = (uint8_t)(colourFloat[1] * 255.0f);
            lockOnColour[2] = (uint8_t)(colourFloat[2] * 255.0f);
        }
        if (ImGui::Button("Default")) {
            lockOnColour[0] = 219;
            lockOnColour[1] = 170;
            lockOnColour[2] = 230;
        }
    }
}

void HudSettings::render_description() const {
    ImGui::TextWrapped(HudSettings::hoveredDescription);
}

std::optional<std::string> HudSettings::on_initialize() {
    return Mod::on_initialize();
}
// during load
void HudSettings::on_config_load(const utility::Config &cfg) {
    custom_lock_on_colour_toggle = cfg.get<bool>("custom_lock_on_colour_toggle").value_or(false);
    lockOnColour[0] = cfg.get<uint8_t>("lockOnColour[0]").value_or(219);
    lockOnColour[1] = cfg.get<uint8_t>("lockOnColour[1]").value_or(170);
    lockOnColour[2] = cfg.get<uint8_t>("lockOnColour[2]").value_or(230);
}
// during save
void HudSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_lock_on_colour_toggle", custom_lock_on_colour_toggle);
    cfg.set<uint8_t>("lockOnColour[0]", lockOnColour[0]);
    cfg.set<uint8_t>("lockOnColour[1]", lockOnColour[1]);
    cfg.set<uint8_t>("lockOnColour[2]", lockOnColour[2]);
}
