#include "CameraSettings.hpp"
#if 1
const char* CameraSettings::defaultDescription = "Camera Settings";
const char* CameraSettings::hoveredDescription = defaultDescription;

bool CameraSettings::mod_enabled_screenshake = false;
uintptr_t CameraSettings::screenshake_jmp_ret1 = NULL;
uintptr_t CameraSettings::Offset_84BA18 = NULL;
int CameraSettings::customBasicScreenshakeAmount = 6;

bool CameraSettings::mod_enabled_first_person_x = false;
bool CameraSettings::CameraSettings::mod_enabled_darkside_x = false;

bool CameraSettings::force_fov = false;
bool CameraSettings::disable_attack_zoom = false;
float CameraSettings::custom_fov = 0.0f;
float CameraSettings::default_fov = 0.0f;
uintptr_t CameraSettings::fov_jmp_ret1 = NULL;
uintptr_t CameraSettings::fov_jmp_ret2 = NULL;

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        push eax
        mov eax, [CameraSettings::Offset_84BA18]
        mov [eax], 6
        pop eax
        cmp byte ptr [CameraSettings::mod_enabled_screenshake], 0
        je originalcode

        push dword ptr [CameraSettings::customBasicScreenshakeAmount]
        jmp dword ptr [CameraSettings::screenshake_jmp_ret1]

        originalcode:
        push 6
        jmp dword ptr [CameraSettings::screenshake_jmp_ret1]
    }
}

void CameraSettings::toggle_first_person_x(bool enable) {
    if (enable) {
        install_patch_offset(0x3DA503, m_patch_first_person_x, "\x80\x3D\x1A", 3); // cmp byte ptr [nmh.mHRPc::CameraXReverseControlFlag]
    }
    else {
        install_patch_offset(0x3DA503, m_patch_first_person_x, "\x80\x3D\x1B", 3); // cmp byte ptr [nmh.mHRPc::CameraYReverseControlFlag]
    }
}

void CameraSettings::toggle_darkside_x(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC2C3, m_patch_darkside_x, "\x80\x3D\x1A", 3); // cmp byte ptr [nmh.mHRPc::CameraXReverseControlFlag]
    }
    else {
        install_patch_offset(0x3DC2C3, m_patch_darkside_x, "\x80\x3D\x1B", 3); // cmp byte ptr [nmh.mHRPc::CameraYReverseControlFlag]
    }
}

void CameraSettings::fov_toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3C6BF2, m_fov_patch, "\x90\x90\x90\x90", 4); // nop 4
    }
    else {
        install_patch_offset(0x3C6BF2, m_fov_patch, "\xF3\x0F\x5C\xC8", 4); // subss xmm1,xmm0
    }          
}

// clang-format off
naked void fov_detour1() { // used most of the time
    __asm {
        cmp byte ptr [CameraSettings::force_fov], 0
        je originalcode

        addss xmm1, [CameraSettings::custom_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret1]

        originalcode:
        addss xmm1, [CameraSettings::default_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret1]
    }
}

naked void fov_detour2() { // used during attacks
    __asm {
        cmp byte ptr [CameraSettings::force_fov], 0
        je originalcode

        movss xmm1, [CameraSettings::custom_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret2]

        originalcode:
        movss xmm1, [CameraSettings::default_fov]
        jmp dword ptr [CameraSettings::fov_jmp_ret2]
    }
}

 // clang-format on

std::optional<std::string> CameraSettings::on_initialize() {
    CameraSettings::Offset_84BA18 = g_framework->get_module().as<uintptr_t>() + 0x84BA18;
    if (!install_hook_offset(0x3C03BC, m_hook1, &detour1, &CameraSettings::screenshake_jmp_ret1, 12)) {
        spdlog::error("Failed to init CameraSettings mod\n");
        return "Failed to init CameraSettings mod";
    }

    default_fov = *(float*)(g_framework->get_module().as<uintptr_t>() + 0x7A44E0);
    if (!install_hook_offset(0x3C55B7, m_fov_hook1, &fov_detour1, &CameraSettings::fov_jmp_ret1, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    if (!install_hook_offset(0x3C6BE4, m_fov_hook2, &fov_detour2, &CameraSettings::fov_jmp_ret2, 8)) {
        spdlog::error("Failed to init ForceFOV mod\n");
        return "Failed to init ForceFOV mod";
    }

    return Mod::on_initialize();
}

static bool hide_drawbpber = false;
static bool hide_drawhber = false;
static bool hide_drawbattery = false;
static bool hide_drawmoney = false;
static bool hide_drawtension = false;
static bool hide_drawkamae = false;
static bool hide_drawspeedmeter = false;
static bool hide_drawmap = false;
static bool hide_drawsilvia = false;
static bool hide_drawlmode = false;
static bool hide_drawkeyguide = false;
static bool hide_drawcheckpo = false;

static void DisplayHUDCheckboxes() {
    mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
    if (!mHRBattle) { return; }
    HrScreenStatus* hrScreenStatus = mHRBattle->mBtEffect.pScreenStatus;
    if (!hrScreenStatus) { return; }

    ImGui::SeparatorText("HUD Toggles");

    if (ImGui::BeginTable("UIControlTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Force Override", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Game State", ImGuiTableColumnFlags_WidthStretch);
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

static void SetHUDFlagsOnFrame() {
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

void CameraSettings::render_description() const {
    ImGui::TextWrapped(CameraSettings::hoveredDescription);
}

void CameraSettings::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Custom Screenshake On Normal Attacks", &mod_enabled_screenshake);
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;
    if (mod_enabled_screenshake) {
        ImGui::Indent();
        ImGui::Text("Custom Screenshake Amount");
        ImGui::SliderInt("##CustomScreenshakeAmountSliderInt", &customBasicScreenshakeAmount, 0, 20);
        if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Default 6";
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Invert X Axis During First Person", &mod_enabled_first_person_x)) {
        toggle_first_person_x(mod_enabled_first_person_x);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    if (ImGui::Checkbox("Invert X Axis During Darkside", &mod_enabled_darkside_x)) {
        toggle_darkside_x(mod_enabled_darkside_x);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;

    ImGui::Checkbox("Custom FOV", &force_fov);
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = defaultDescription;
    if (force_fov) {
        ImGui::Indent();
        ImGui::SliderFloat("##CustomFOVSliderFloat", &CameraSettings::custom_fov, 1.0f, 180.0f, "%.0f");
        if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Default 55";
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Disable Attack Camera Zoom", &disable_attack_zoom)) {
        fov_toggle(disable_attack_zoom);
    }
    if (ImGui::IsItemHovered()) CameraSettings::hoveredDescription = "Disable the zoom when attacking enemies while not locked on.";

    DisplayHUDCheckboxes();
}

// during load
void CameraSettings::on_config_load(const utility::Config &cfg) {
    mod_enabled_screenshake = cfg.get<bool>("custom_screenshake").value_or(false);
    customBasicScreenshakeAmount = cfg.get<int>("customBasicScreenshakeAmount").value_or(6);

    mod_enabled_first_person_x = cfg.get<bool>("invert_x_first_person").value_or(false);
    if (mod_enabled_first_person_x) toggle_first_person_x(mod_enabled_first_person_x);
    mod_enabled_darkside_x = cfg.get<bool>("invert_x_darkside").value_or(false);
    if (mod_enabled_darkside_x) toggle_darkside_x(mod_enabled_darkside_x);

    force_fov = cfg.get<bool>("force_fov").value_or(false);
    custom_fov = cfg.get<float>("custom_fov_value").value_or(55.0f);
    disable_attack_zoom = cfg.get<bool>("disable_attack_zoom").value_or(false);
    if (disable_attack_zoom) fov_toggle(disable_attack_zoom);
} 
// during save
void CameraSettings::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_screenshake", mod_enabled_screenshake);
    cfg.set<int>("customBasicScreenshakeAmount", customBasicScreenshakeAmount);

    cfg.set<bool>("invert_x_first_person", mod_enabled_first_person_x);
    cfg.set<bool>("invert_x_darkside", mod_enabled_darkside_x);

    cfg.set<bool>("force_fov", force_fov);
    cfg.set<float>("custom_fov_value", custom_fov);
    cfg.set<bool>("disable_attack_zoom", disable_attack_zoom);
}

// do something every frame
void CameraSettings::on_frame() {
    SetHUDFlagsOnFrame();
}
// will show up in debug window, dump ImGui widgets you want here
//void CameraSettings::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
