#include "FreeCam.hpp"
#include "GuiFunctions.hpp" // for g_framework
#include "KbmControls.hpp" // for gamepad button structs
#if 1
static bool mod_enabled = false;
PAD_UNI* pad = nullptr;
float FreeCam::sens = 0.0f;
float FreeCam::modifierSens = 0.0f;
float FreeCam::deadZone = 0.0f;

const char* FreeCam::defaultDescription = "Controls:\n- Left Stick and Right Stick = Movement and Rotation\n- R1 & R2 = Move Up and Down\n- L3 = Use Modifier Speed\n- R3 = Reset Position";
const char* FreeCam::hoveredDescription = defaultDescription;

void FreeCam::render_description() const {
    ImGui::TextWrapped(FreeCam::hoveredDescription);
}

// stops cam being set back to 8 mid battle
void FreeCam::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3B635F, battle_freecam_patch, "\xEB\x07", 2); // jmp nmh.mHRBattle::mFrameProc+208
        install_patch_offset(0x3B55BA, bike_freecam_patch, "\xEB\x35", 2); // jmp nmh.mHRBattle::mCameraFrameProc+D81


    }
    else {
        install_patch_offset(0x3B635F, battle_freecam_patch, "\x74\x07", 2); // je nmh.mHRBattle::mFrameProc+208
        install_patch_offset(0x3B55BA, bike_freecam_patch, "\x74\x35", 2); // je nmh.mHRBattle::mCameraFrameProc+D81
    }
}

std::optional<std::string> FreeCam::on_initialize() {
    uintptr_t padAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D10);
    pad = (PAD_UNI*)padAddr;
    return Mod::on_initialize();
}

void FreeCam::on_frame() {
    if (!mod_enabled) { return; }
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { return; }
    
    HrCamera* cam = nmh_sdk::get_HrCamera();
    if (!cam) { return; }
    
    if (cam->MAIN.Mode != 2) { 
        cam->MAIN.Mode = HRCAMERA_MODE_FREE; 
    }
    
    if (!pad) { return; }
    
    auto buttons = pad->WiiStatusBuffer[0][0].ex_status.cl.hold;
    auto& cl = pad->WiiStatusBuffer[0][0].ex_status.cl;
    
    float currentSens = sens;
    float rotSens = 0.1f;
    
    // camera sprint
    if (buttons & KEY_L3) {
        currentSens = modifierSens;
    }
    
    // get forward and right camera facing
    float yaw = cam->MAIN.free.YDir;
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    Vec forward = {sinYaw, 0.0f, cosYaw};
    Vec right = {cosYaw, 0.0f, -sinYaw};

    // trigger/bumper vertical
    if (buttons & KEY_RT) {
        cam->MAIN.free.C_T_Pos.y -= currentSens;
    }
    if (buttons & KEY_RB) {
        cam->MAIN.free.C_T_Pos.y += currentSens;
    }
    
    // left stick
    float leftX = cl.lstick.x;
    float leftY = cl.lstick.y;
    
    if (abs(leftX) > deadZone) {
        cam->MAIN.free.C_T_Pos.x -= right.x * leftX * currentSens;
        cam->MAIN.free.C_T_Pos.z -= right.z * leftX * currentSens;
    }
    if (abs(leftY) > deadZone) {
        cam->MAIN.free.C_T_Pos.x += forward.x * leftY * currentSens;
        cam->MAIN.free.C_T_Pos.z += forward.z * leftY * currentSens;
    }
    
    // right stick
    float rightX = cl.rstick.x;
    float rightY = cl.rstick.y;
    
    if (abs(rightX) > deadZone) {
        cam->MAIN.free.T_YDir -= rightX * rotSens;
        cam->MAIN.free.YDir = cam->MAIN.free.T_YDir;
    }
    if (abs(rightY) > deadZone) {
        cam->MAIN.free.T_VDir += rightY * rotSens;
        cam->MAIN.free.VDir = cam->MAIN.free.T_VDir;
        
        // dont go past up/down
        if (cam->MAIN.free.T_VDir > 1.5f) cam->MAIN.free.T_VDir = 1.5f;
        if (cam->MAIN.free.T_VDir < -1.5f) cam->MAIN.free.T_VDir = -1.5f;
        cam->MAIN.free.VDir = cam->MAIN.free.T_VDir;
    }
    
    // reset cam
    if (buttons & KEY_R3) {
        cam->MAIN.free.C_T_Pos.x = player->mCharaStatus.pos.x;
        cam->MAIN.free.C_T_Pos.y = player->mCharaStatus.pos.y + 20.0f;
        cam->MAIN.free.C_T_Pos.z = player->mCharaStatus.pos.z;
    }
}

void FreeCam::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) FreeCam::hoveredDescription = defaultDescription;
    if (ImGui::Checkbox("Free Cam", &mod_enabled)) {
        toggle(mod_enabled);
    }
    if (ImGui::IsItemHovered()) FreeCam::hoveredDescription = defaultDescription;
    ImGui::Text("Sensitivity");
    ImGui::SliderFloat("##SensitivitySliderFloat", &sens, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered()) FreeCam::hoveredDescription = "Camera movement sensitivity";
    ImGui::Text("Modifier Sensitivity");
    ImGui::SliderFloat("##ModifierSensitivitySliderFloat", &modifierSens, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered()) FreeCam::hoveredDescription = "Camera movement sensitivity while holding L3";
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        ImGui::Checkbox("Enable inputs to player", &player->mOperate);
        if (ImGui::IsItemHovered()) FreeCam::hoveredDescription = "Disable player movement/actions";
    }
}

// during load
void FreeCam::on_config_load(const utility::Config &cfg) {
    sens = cfg.get<float>("freecam_sens").value_or(1.0f);
    modifierSens = cfg.get<float>("modifier_freecam_sens").value_or(2.0f);
    deadZone = cfg.get<float>("freecam_deadzone").value_or(0.05f);
}
// during save
void FreeCam::on_config_save(utility::Config &cfg) {
    cfg.set<float>("freecam_sens", sens);
    cfg.set<float>("modifier_freecam_sens", modifierSens);
    cfg.set<float>("freecam_deadzone", deadZone);
}
// do something every frame
// will show up in debug window, dump ImGui widgets you want here
//void FreeCam::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
