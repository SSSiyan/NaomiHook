#include "BrainAge.hpp"
#if 1
bool BrainAge::imguiPopout = false;

template<typename T>
bool getBit(T flags, int bit) {
    return (flags & (1 << bit)) != 0;
}

template<typename T>
void setBit(T& flags, int bit, bool value) {
    if (value) {
        flags |= (1 << bit);  // Set the bit
    } else {
        flags &= ~(1 << bit); // Clear the bit
    }
}

void BrainAge::Stuff() {
    if (ImGui::CollapsingHeader("Force Camera")) {
        HrCamera* hrCamera = nmh_sdk::get_HrCamera();
        if (ImGui::TreeNodeEx("MOVE2", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Pc Pos", &hrCamera->MAIN.mov2.PcPos.x);
            ImGui::InputFloat("Pc Angle", &hrCamera->MAIN.mov2.PcAngle);
            ImGui::InputFloat("Cam Angle", &hrCamera->MAIN.mov2.CamAngle);
            ImGui::InputFloat("Cam Targ Length", &hrCamera->MAIN.mov2.CamTargLength);
            ImGui::InputFloat("Cam Y Angle Rate", &hrCamera->MAIN.mov2.CamYAngleRate);
            ImGui::InputFloat3("Abs Cam Pos", &hrCamera->MAIN.mov2.AbsCamPos.x);
            ImGui::InputFloat3("Abs Targ Pos", &hrCamera->MAIN.mov2.AbsTargPos.x);
            ImGui::InputFloat("Pc Look Rate", &hrCamera->MAIN.mov2.PcLookRate);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("BATTLE2", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Player Pos", &hrCamera->MAIN.bat2.PPos.x);
            ImGui::InputFloat3("Player Pos Offset", &hrCamera->MAIN.bat2.PPosOffset.x);
            ImGui::InputFloat3("Enemy Pos", &hrCamera->MAIN.bat2.EPos.x);
            ImGui::Checkbox("Debug Mode", &hrCamera->MAIN.bat2.DebugMode);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("MOTION", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputScalar("Gan Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGan);
            ImGui::InputScalar("GanPlay Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGanPlay);
            ImGui::InputScalar("GanPlayNode Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGanPlayNode);
            ImGui::InputFloat3("Translate", &hrCamera->MAIN.motion.Translate.x);
            ImGui::InputFloat("Rotate Y", &hrCamera->MAIN.motion.RotateY);
            ImGui::InputFloat("Fov", &hrCamera->MAIN.motion.Fov);
            ImGui::InputFloat("Roll", &hrCamera->MAIN.motion.Roll);
            ImGui::InputFloat("Motion Rate Time", &hrCamera->MAIN.motion.MotionRateTime);
            ImGui::Checkbox("Valid Fov", &hrCamera->MAIN.motion.ValidFov);
            ImGui::Checkbox("Valid Roll", &hrCamera->MAIN.motion.ValidRoll);
            ImGui::Checkbox("Pause", &hrCamera->MAIN.motion.pause);
            ImGui::Checkbox("Coll", &hrCamera->MAIN.motion.coll);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("NORMAL", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Camera Pos", &hrCamera->MAIN.nrm.CPos.x);
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.nrm.TPos.x);
            ImGui::Checkbox("Valid Fov", &hrCamera->MAIN.nrm.ValidFov);
            ImGui::InputFloat("Fov", &hrCamera->MAIN.nrm.Fov);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("HOMING", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.homing.T_Pos.x);
            ImGui::InputFloat3("Camera Pos", &hrCamera->MAIN.homing.C_Pos.x);
            ImGui::InputFloat("Max Length", &hrCamera->MAIN.homing.C_T_MaxLen);
            ImGui::InputFloat("Limit Length", &hrCamera->MAIN.homing.C_T_LimitLen);
            ImGui::InputFloat("Min Length", &hrCamera->MAIN.homing.C_T_MinLen);
            ImGui::InputFloat("Order Length", &hrCamera->MAIN.homing.C_T_OrderLen);
            ImGui::InputFloat("Angle", &hrCamera->MAIN.homing.C_T_Angle);
            ImGui::InputFloat("Add Y", &hrCamera->MAIN.homing.T_PosAddY);
            ImGui::InputInt("Max Over Count", &hrCamera->MAIN.homing.MaxOverCount);
            ImGui::Checkbox("Setup", &hrCamera->MAIN.homing.Setup);
            ImGui::Checkbox("Move Flag", &hrCamera->MAIN.homing.MoveFlag);
            for (int i = 0; i < 20; i++) {
                ImGui::InputFloat3(("T_PosLog[" + std::to_string(i) + "]").c_str(), &hrCamera->MAIN.homing.T_PosLog[i].x);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("FREE", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Camera-Target Pos", &hrCamera->MAIN.free.C_T_Pos.x);
            ImGui::InputFloat("Target VDir", &hrCamera->MAIN.free.T_VDir);
            ImGui::InputFloat("VDir", &hrCamera->MAIN.free.VDir);
            ImGui::InputFloat("Target YDir", &hrCamera->MAIN.free.T_YDir);
            ImGui::InputFloat("YDir", &hrCamera->MAIN.free.YDir);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("MOVE", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Player Pos", &hrCamera->MAIN.mov.P_Pos.x);
            ImGui::Checkbox("Player Pos Valid", &hrCamera->MAIN.mov.P_PosValid);
            ImGui::Checkbox("Coll Valid", &hrCamera->MAIN.mov.CollValid);
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.mov.T_Pos.x);
            ImGui::InputFloat("Add Y", &hrCamera->MAIN.mov.T_PosAddY);
            ImGui::InputFloat3("Target Dir", &hrCamera->MAIN.mov.T_Dir.x);
            ImGui::InputFloat("Target Rot Y", &hrCamera->MAIN.mov.T_RotY);
            ImGui::TreePop();
        }

        // === FORCE CAMERA MODE CONTROLS ===
        static bool forceCameraMode = false;
        static int forcedMode = 0;

        ImGui::Checkbox("Force Camera Mode", &forceCameraMode);

        if (forceCameraMode) {
            ImGui::Combo("Forced Mode", &forcedMode,
                "HRCAMERA_MODE_HOMING\0HRCAMERA_MODE_MOTION\0HRCAMERA_MODE_FREE\0HRCAMERA_MODE_MOVE\0HRCAMERA_MODE_BATTLE\0HRCAMERA_MODE_IDLE\0HRCAMERA_MODE_NORMAL\0HRCAMERA_MODE_BATTLE2\0HRCAMERA_MODE_MOVE2\0");
            hrCamera->MAIN.Mode = static_cast<HRCAMERA_MODE>(forcedMode);
        } else {
            ImGui::Combo("Mode", reinterpret_cast<int*>(&hrCamera->MAIN.Mode),
                "HRCAMERA_MODE_HOMING\0HRCAMERA_MODE_MOTION\0HRCAMERA_MODE_FREE\0HRCAMERA_MODE_MOVE\0HRCAMERA_MODE_BATTLE\0HRCAMERA_MODE_IDLE\0HRCAMERA_MODE_NORMAL\0HRCAMERA_MODE_BATTLE2\0HRCAMERA_MODE_MOVE2\0");
        }

        ImGui::InputFloat3("Position", &hrCamera->MAIN.Pos.x);
        ImGui::InputFloat3("Target", &hrCamera->MAIN.Targ.x);
        ImGui::InputFloat("Twist Angle", &hrCamera->MAIN.TwistAngle);
        ImGui::InputInt("Frame Counter", &hrCamera->MAIN.FrameCounter);
        ImGui::Checkbox("Always", &hrCamera->MAIN.Always);
        ImGui::Checkbox("Change", &hrCamera->MAIN.Change);
    }

    if (ImGui::CollapsingHeader("New thing 2")) {

    }

    if (ImGui::CollapsingHeader("New thing 3")) {

    }

    static bool disableCamBeingAutoSet = false;
    if (ImGui::Checkbox("Disable the game setting cam mode", &disableCamBeingAutoSet)) {
        BrainAge::toggleCam(disableCamBeingAutoSet);
    }
}

void BrainAge::on_draw_ui() {
    ImGui::Checkbox("Brain Age Popout", &imguiPopout);
    if (!imguiPopout)
        Stuff();
}

//void BrainAge::custom_imgui_window() {}

// during load
//void BrainAge::on_config_load(const utility::Config &cfg) {}
// during save
//void BrainAge::on_config_save(utility::Config &cfg) {}
// do something every frame

void BrainAge::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("imguiPopout", &imguiPopout);
        Stuff();
        ImGui::End();
    }
}

void BrainAge::toggleCam(bool enable) {
    if (enable) {
        // stops cam being set
        install_patch_offset(0x3EC616, m_patch1, "\x90\x90\x90\x90\x90\x90", 6); // 
    }
    else {
        install_patch_offset(0x3EC616, m_patch1, "\x89\x0D\xA0\xA4\x7E\x01", 6); // mov [nmh.gHrCamera],ecx
    }
}

std::optional<std::string> BrainAge::on_initialize() {
    return Mod::on_initialize();
}

// will show up in debug window, dump ImGui widgets you want here
//void BrainAge::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
