#include "BrainAge.hpp"
#if 1
bool BrainAge::imguiPopout = false;

bool BrainAge::forceCameraMode = false;
int BrainAge::forcedMode       = 0;

bool BrainAge::guard_cooldown_enabled = false;
static bool isGuardingNow             = false;
static int justGuardCooldown          = 0;
static int justGuardToggleCount       = 0;
static int guardToggleWindow          = 0;
static bool lastGuardState            = false;

// -------------------- New Thing 3: BATTLE2 preset state --------------------
static bool g_b2_lock_preset = false;   // public-facing checkbox
static bool g_b2_prev_lock   = false;   // edge detect for untick
static float g_b2_campos_z   = 22.093f; // runtime Z that we ease

// Start/stop control using Success Input Finish (start) and finishSlowTick (finish)
static bool g_b2_zoom_armed        = false; // SIF fired, waiting for delay
static bool g_b2_zoom_active       = false; // currently zooming
static bool g_b2_slow_seen         = false; // finishSlowTick has been >0 at least once this sequence
static bool g_b2_sif_prev          = false; // last frame SIF value
static double g_b2_sif_time        = 0.0;   // when SIF turned on
static double g_b2_zoom_start_time = 0.0;   // when zoom actually began

// Safety-net state for finishSlowTick not changing
static int g_b2_prev_slow_tick           = 0;
static double g_b2_last_slow_change_time = 0.0;

// Exact values for the "locked" preset
static const float kB2_CamPos_X      = -15.294f;
static const float kB2_CamPos_Y      = 13.529f;
static const float kB2_CamPos_Z_BASE = 22.093f;
static const float kB2_TgtPos_X      = -7.500f;
static const float kB2_TgtPos_Y      = 9.000f;
static const float kB2_TgtPos_Z      = 25.919f;

// Values to restore when unticked
static const float kB2_Restore_CamPos_X = -7.000f;
static const float kB2_Restore_CamPos_Y = 17.000f;
static const float kB2_Restore_CamPos_Z = 14.000f;
static const float kB2_Restore_TgtPos_X = -4.000f;
static const float kB2_Restore_TgtPos_Y = 10.000f;
static const float kB2_Restore_TgtPos_Z = 1.500f;

// Tuning
static const float kB2_Z_PULL_MAX     = 15.0f; // main inward distance (amount of zoom)
static const float kB2_Z_EXTRA        = 6.0f;  // extra tiny inward zoom after main max
static const float kB2_LERP_IN        = 0.06f; // during main zoom (higher = faster approach)
static const float kB2_LERP_CREEP     = 0.01f; // very slow creep after reaching main max
static const float kB2_LERP_OUT       = 1.50f; // when returning (higher = faster return)
static const double kB2_SIF_DELAY_SEC = 0.180; // start this long after Success Input Finish
static const double kB2_EASE_IN_SEC   = 0.35;  // time to reach full pull after zoom starts

// Safety-net tuning (reset even if finishSlowTick never changes)
static const double kB2_SLOW_STALL_SEC = 1.25; // if slow tick does not change this long while zooming, cancel
static const double kB2_ZOOM_MAX_SEC   = 3.00; // hard cap on a single zoom sequence

static inline float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
static inline float smoothstep01(float t) {
    t = clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}

static void ApplyBattle2PresetTick() {
    HrCamera* cam = nmh_sdk::get_HrCamera();
    if (!cam) {
        g_b2_prev_lock = g_b2_lock_preset;
        return;
    }

    mHRPc* player = nmh_sdk::get_mHRPc();
    bool sif_now  = false;
    int slowTick  = 0;
    if (player) {
        sif_now  = player->mPcStatus.successInputFinish;
        slowTick = player->mPcStatus.finishSlowTick;
    }

    const double now = ImGui::GetTime();

    // Track changes in finishSlowTick for the safety net
    if (slowTick != g_b2_prev_slow_tick) {
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }

    // On transition: locked -> unlocked, restore the BATTLE2 values
    if (g_b2_prev_lock && !g_b2_lock_preset) {
        cam->MAIN.bat2.DebugMode             = true; // ensure DebugInfo is used
        cam->MAIN.bat2.DebugInfo.CameraPos.x = kB2_Restore_CamPos_X;
        cam->MAIN.bat2.DebugInfo.CameraPos.y = kB2_Restore_CamPos_Y;
        cam->MAIN.bat2.DebugInfo.CameraPos.z = kB2_Restore_CamPos_Z;

        cam->MAIN.bat2.DebugInfo.TargetPos.x = kB2_Restore_TgtPos_X;
        cam->MAIN.bat2.DebugInfo.TargetPos.y = kB2_Restore_TgtPos_Y;
        cam->MAIN.bat2.DebugInfo.TargetPos.z = kB2_Restore_TgtPos_Z;

        // reset internal state
        g_b2_campos_z              = kB2_CamPos_Z_BASE;
        g_b2_zoom_armed            = false;
        g_b2_zoom_active           = false;
        g_b2_slow_seen             = false;
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }

    if (!g_b2_lock_preset) {
        // When unlocked, relax Z toward base so it never snaps if re-enabled
        g_b2_campos_z  = lerp(g_b2_campos_z, kB2_CamPos_Z_BASE, kB2_LERP_OUT);
        g_b2_prev_lock = g_b2_lock_preset;
        g_b2_sif_prev  = sif_now;
        return;
    }

    // Locked preset path: drive BATTLE2 via DebugInfo
    cam->MAIN.bat2.DebugMode = true;

    // Fixed XY; Z eased below
    cam->MAIN.bat2.DebugInfo.CameraPos.x = kB2_CamPos_X;
    cam->MAIN.bat2.DebugInfo.CameraPos.y = kB2_CamPos_Y;

    cam->MAIN.bat2.DebugInfo.TargetPos.x = kB2_TgtPos_X;
    cam->MAIN.bat2.DebugInfo.TargetPos.y = kB2_TgtPos_Y;
    cam->MAIN.bat2.DebugInfo.TargetPos.z = kB2_TgtPos_Z;

    // --- Start condition (Success Input Finish) ---
    // Rising edge detection: SIF toggles on when the correct direction is pressed
    if (sif_now && !g_b2_sif_prev) {
        g_b2_sif_time              = now;
        g_b2_zoom_armed            = true;  // start the delay
        g_b2_slow_seen             = false; // we have not yet seen the slow-mo start
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }

    // After the delay, begin the zoom regardless of slowTick.
    if (g_b2_zoom_armed && !g_b2_zoom_active) {
        if ((now - g_b2_sif_time) >= kB2_SIF_DELAY_SEC) {
            g_b2_zoom_active     = true;
            g_b2_zoom_start_time = now;
            // reset stall watchdog window at start
            g_b2_prev_slow_tick        = slowTick;
            g_b2_last_slow_change_time = now;
        }
    }

    // Track if slow-mo has started at least once
    if (slowTick > 0) {
        g_b2_slow_seen = true;
    }

    // --- Finish condition (finishSlowTick ends) ---
    // Only stop once the slow-mo has started and then finished.
    if (g_b2_slow_seen && slowTick <= 0) {
        g_b2_zoom_active = false;
        g_b2_zoom_armed  = false;
    }

    // --- Safety nets ---
    if (g_b2_zoom_active) {
        // 1) If finishSlowTick does not change for too long, cancel the zoom.
        if ((now - g_b2_last_slow_change_time) >= kB2_SLOW_STALL_SEC) {
            g_b2_zoom_active = false;
            g_b2_zoom_armed  = false;
            g_b2_slow_seen   = false;
        }
        // 2) Hard cap duration for an active zoom, regardless of slowTick.
        else if ((now - g_b2_zoom_start_time) >= kB2_ZOOM_MAX_SEC) {
            g_b2_zoom_active = false;
            g_b2_zoom_armed  = false;
        }
    }

    // --- Drive CameraPos.z (two-phase: main ease-in, then very slow creep) ---
    if (g_b2_zoom_active) {
        const double elapsed = now - g_b2_zoom_start_time;
        const float tNorm    = (float)clamp01(elapsed / kB2_EASE_IN_SEC);
        const float tEase    = smoothstep01(tNorm);

        const float mainTargetZ  = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX * tEase);
        const float creepTargetZ = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX + kB2_Z_EXTRA);

        if (tNorm < 1.0f) {
            // Phase 1: approach the main target quickly
            g_b2_campos_z = lerp(g_b2_campos_z, mainTargetZ, kB2_LERP_IN);
        } else {
            // Phase 2: creep very slowly the last tiny bit
            g_b2_campos_z = lerp(g_b2_campos_z, creepTargetZ, kB2_LERP_CREEP);
        }
    } else {
        // Return toward base while not active
        g_b2_campos_z = lerp(g_b2_campos_z, kB2_CamPos_Z_BASE, kB2_LERP_OUT);
    }

    cam->MAIN.bat2.DebugInfo.CameraPos.z = g_b2_campos_z;

    g_b2_prev_lock = g_b2_lock_preset;
    g_b2_sif_prev  = sif_now;
}
// --------------------------------------------------------------------------

template <typename T> bool getBit(T flags, int bit) {
    return (flags & (1 << bit)) != 0;
}

template <typename T> void setBit(T& flags, int bit, bool value) {
    if (value) {
        flags |= (1 << bit); // Set the bit
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
            ImGui::InputFloat("Length", &hrCamera->MAIN.bat2.DebugInfo.Length);
            ImGui::SliderFloat3("CameraPos", &hrCamera->MAIN.bat2.DebugInfo.CameraPos.x, -100.0f, 200.0f);
            ImGui::SliderFloat3("TargetPos", &hrCamera->MAIN.bat2.DebugInfo.TargetPos.x, -100.0f, 200.0f);

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

        ImGui::Checkbox("Force Camera Mode", &forceCameraMode);
        ImGui::Combo("Mode", reinterpret_cast<int*>(&hrCamera->MAIN.Mode),
            "HRCAMERA_MODE_HOMING\0HRCAMERA_MODE_MOTION\0HRCAMERA_MODE_FREE\0HRCAMERA_MODE_MOVE\0HRCAMERA_MODE_BATTLE\0HRCAMERA_MODE_"
            "IDLE\0HRCAMERA_MODE_NORMAL\0HRCAMERA_MODE_BATTLE2\0HRCAMERA_MODE_MOVE2\0");

        ImGui::InputFloat3("Position", &hrCamera->MAIN.Pos.x);
        ImGui::InputFloat3("Target", &hrCamera->MAIN.Targ.x);
        ImGui::InputFloat("Twist Angle", &hrCamera->MAIN.TwistAngle);
        ImGui::InputInt("Frame Counter", &hrCamera->MAIN.FrameCounter);
        ImGui::Checkbox("Always", &hrCamera->MAIN.Always);
        ImGui::Checkbox("Change", &hrCamera->MAIN.Change);
    }

    if (ImGui::CollapsingHeader("Guard Cooldown##CollapsingHeader")) {
        ImGui::Checkbox("Guard Cooldown", &guard_cooldown_enabled);
        ImGui::Text("Debug info:");
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            ImGui::Checkbox("Just Guard", &player->mPcStatus.justGuard);
            help_marker("Ticks when a Parry is performed");

            ImGui::Checkbox("Just Attack", &player->mPcStatus.justAttack);
            help_marker("Parry reprisal");

            ImGui::InputInt("Just Input Tick", &player->mPcStatus.justInputTick);
            help_marker("Parry window length");

            ImGui::InputInt("Just Atk Input Start Tick", &player->mPcStatus.justAtkInputStartTick);
            ImGui::InputInt("Just Atk Input End Tick", &player->mPcStatus.justAtkInputEndTick);

            ImGui::Text("JustGuard Cooldown: %d", justGuardCooldown);
            ImGui::Text("Toggle Count: %d", justGuardToggleCount);
            ImGui::Text("Toggle Window: %d", guardToggleWindow);
        }
    }

    if (ImGui::CollapsingHeader("New thing 3")) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            // Public toggle: locks BATTLE2 to fixed CameraPos/TargetPos and
            // eases CameraPos.z on SIF->delay start, stops when finishSlowTick ends.
            ImGui::Checkbox("Lock BATTLE2 Camera Preset", &g_b2_lock_preset);
        }
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

void BrainAge::GuardCooldown() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        isGuardingNow = player->mPcStatus.justInputTick > 0;
        // Count toggles
        if (isGuardingNow != lastGuardState) {
            justGuardToggleCount++;
            guardToggleWindow = 30;
        }
        lastGuardState = isGuardingNow;

        // Tick down toggle window
        if (guardToggleWindow > 0) {
            guardToggleWindow--;
        } else {
            justGuardToggleCount = 0;
        }

        // Apply cooldown if spam detected
        if (justGuardToggleCount >= 4) {
            justGuardCooldown    = 20;
            justGuardToggleCount = 0;
            guardToggleWindow    = 0;
        }

        // Disable parry input entirely during cooldown
        if (justGuardCooldown > 0) {
            justGuardCooldown--;
            player->mPcStatus.justGuard     = false;
            player->mPcStatus.justInputTick = nmh_sdk::GetJustGuardJudgeTick(player); // Prevent parry window
        }
    }
}

void BrainAge::ForceCameraModes() {
    if (forceCameraMode) {
        HrCamera* hrCamera  = nmh_sdk::get_HrCamera();
        hrCamera->MAIN.Mode = static_cast<HRCAMERA_MODE>(forcedMode);
    }
}

void BrainAge::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("imguiPopout", &imguiPopout);
        Stuff();
        ImGui::End();
    }
    if (forceCameraMode) {
        ForceCameraModes();
    }
    if (guard_cooldown_enabled) {
        GuardCooldown();
    }
    // Apply New Thing 3 preset every frame
    ApplyBattle2PresetTick();
}

void BrainAge::toggleCam(bool enable) {
    if (enable) {
        // stops cam being set
        install_patch_offset(0x3EC616, m_patch1, "\x90\x90\x90\x90\x90\x90", 6); //
    } else {
        m_patch1.reset();
    }
}

std::optional<std::string> BrainAge::on_initialize() {
    return Mod::on_initialize();
}

// void BrainAge::on_draw_debug_ui() {}
//  will show up in main window, dump ImGui widgets you want here
//  during load
// void BrainAge::on_config_load(const utility::Config &cfg) {}
//  during save
// void BrainAge::on_config_save(utility::Config &cfg) {}
//  do something every frame
#endif
