#include "BrainAge.hpp"
#if 1
#include <cmath> // expf, fabsf

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
static bool g_b2_zoom_armed        = false;
static bool g_b2_zoom_active       = false;
static bool g_b2_slow_seen         = false;
static bool g_b2_sif_prev          = false;
static double g_b2_sif_time        = 0.0;
static double g_b2_zoom_start_time = 0.0;

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
static const float kB2_Z_PULL_MAX     = 15.0f;
static const float kB2_Z_EXTRA        = 6.0f;
static const float kB2_LERP_IN        = 0.06f;
static const float kB2_LERP_CREEP     = 0.01f;
static const float kB2_LERP_OUT       = 1.50f;
static const double kB2_SIF_DELAY_SEC = 0.180;
static const double kB2_EASE_IN_SEC   = 0.35;

// Safety-net tuning
static const double kB2_SLOW_STALL_SEC = 1.25;
static const double kB2_ZOOM_MAX_SEC   = 3.00;

// -------------------- helpers (used by zoom; KEEP) --------------------
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

// -------------------- BATTLE2 lag (adjustable) — DISABLED ------------------
// Everything in this section is commented out to preserve code for later.
// Uncomment to restore lag behavior.
/*
struct BA_Vec3 { float x, y, z; };

static bool g_b2_lag_inited       = false;
static bool g_b2_lag_enable       = true;
static BA_Vec3 g_b2_smooth_player = {0.0f, 0.0f, 0.0f};
static BA_Vec3 g_b2_smooth_enemy  = {0.0f, 0.0f, 0.0f};
static BA_Vec3 g_b2_prev_player   = {0.0f, 0.0f, 0.0f};
static BA_Vec3 g_b2_prev_enemy    = {0.0f, 0.0f, 0.0f};
static bool g_b2_have_prev        = false;

// Time-constant smoothing (ms) -> per-frame alpha = 1 - exp(-dt/tau)
static float g_b2_tau_player_ms = 140.0f;
static float g_b2_tau_enemy_ms  = 90.0f;

// Split strengths (how much lag delta we apply)
static float g_b2_player_scale_x = 0.60f;
static float g_b2_player_scale_y = 0.45f;
static float g_b2_enemy_scale_x  = 0.35f;
static float g_b2_enemy_scale_y  = 0.30f;
// NEW: Z strength + max clamp for target
static float g_b2_enemy_scale_z = 0.20f;
static float g_b2_max_tgt_z     = 4.0f;

// Deadzone to kill world-space jitter
static float g_b2_deadzone_xy = 0.02f;

// Max XY offsets so we never drift too far
static float g_b2_max_cam_xy = 3.0f;
static float g_b2_max_tgt_xy = 3.0f;

// Catch-up when far (snappier response)
static bool  g_b2_catchup_enable = true;
static float g_b2_far_thresh_xy  = 2.5f;
static float g_b2_catchup_gain   = 2.0f;

// Look-ahead (player and enemy amounts split)
static float g_b2_lookahead_ms         = 60.0f;
static float g_b2_lookahead_player_amt = 0.50f;
static float g_b2_lookahead_enemy_amt  = 0.00f; // default 0 to avoid amplifying enemy jitter

// Enemy angular deadzone and target slew limiter (to tame lock-on twitch)
static float g_b2_enemy_ang_deadzone_deg    = 0.35f; // ignore enemy moves that would rotate camera less than this
static float g_b2_target_slew_units_per_sec = 20.0f; // max target XYZ change per second (now XYZ)

// helpers (lag-only)
static inline void v3_lerp_inplace(BA_Vec3& a, const BA_Vec3& b, float t) {
    a.x += (b.x - a.x) * t;
    a.y += (b.y - a.y) * t;
    a.z += (b.z - a.z) * t;
}
static inline float clampf(float v, float mn, float mx) {
    return v < mn ? mn : (v > mx ? mx : v);
}
static inline float alpha_from_tau_ms(float tau_ms, float dt) {
    if (tau_ms <= 1.0f) return 1.0f;
    float k = -1000.0f * dt / tau_ms;
    if (k < -50.0f) k = -50.0f;
    return 1.0f - std::exp(k);
}
*/

// ---------------------------------------------------------------

static void ApplyBattle2PresetTick() {
    HrCamera* cam = nmh_sdk::get_HrCamera();
    if (!cam) {
        g_b2_prev_lock = g_b2_lock_preset;
        // Lag state resets (disabled)
        // g_b2_lag_inited = false;
        // g_b2_have_prev  = false;
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
    const float dt   = (float)ImGui::GetIO().DeltaTime > 0.0f ? (float)ImGui::GetIO().DeltaTime : (1.0f / 60.0f);

    if (slowTick != g_b2_prev_slow_tick) {
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }

    // Transition: locked -> unlocked
    if (g_b2_prev_lock && !g_b2_lock_preset) {
        cam->MAIN.bat2.DebugMode             = true;
        cam->MAIN.bat2.DebugInfo.CameraPos.x = kB2_Restore_CamPos_X;
        cam->MAIN.bat2.DebugInfo.CameraPos.y = kB2_Restore_CamPos_Y;
        cam->MAIN.bat2.DebugInfo.CameraPos.z = kB2_Restore_CamPos_Z;
        cam->MAIN.bat2.DebugInfo.TargetPos.x = kB2_Restore_TgtPos_X;
        cam->MAIN.bat2.DebugInfo.TargetPos.y = kB2_Restore_TgtPos_Y;
        cam->MAIN.bat2.DebugInfo.TargetPos.z = kB2_Restore_TgtPos_Z;

        g_b2_campos_z              = kB2_CamPos_Z_BASE;
        g_b2_zoom_armed            = false;
        g_b2_zoom_active           = false;
        g_b2_slow_seen             = false;
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;

        // Lag state resets (disabled)
        // g_b2_lag_inited = false;
        // g_b2_have_prev  = false;
    }

    if (!g_b2_lock_preset) {
        g_b2_campos_z  = lerp(g_b2_campos_z, kB2_CamPos_Z_BASE, kB2_LERP_OUT);
        g_b2_prev_lock = g_b2_lock_preset;
        g_b2_sif_prev  = sif_now;

        // Lag state resets (disabled)
        // g_b2_lag_inited = false;
        // g_b2_have_prev  = false;
        return;
    }

    // Locked preset path
    cam->MAIN.bat2.DebugMode = true;

    // Fixed XY; Z eased below
    cam->MAIN.bat2.DebugInfo.CameraPos.x = kB2_CamPos_X;
    cam->MAIN.bat2.DebugInfo.CameraPos.y = kB2_CamPos_Y;

    cam->MAIN.bat2.DebugInfo.TargetPos.x = kB2_TgtPos_X;
    cam->MAIN.bat2.DebugInfo.TargetPos.y = kB2_TgtPos_Y;
    cam->MAIN.bat2.DebugInfo.TargetPos.z = kB2_TgtPos_Z;

    // --- Lag with jitter suppression (ENTIRE BLOCK DISABLED) ---
    /*
    if (g_b2_lag_enable) {
        // Live anchors from the game
        BA_Vec3 curPlayerAnchor = {
            cam->MAIN.bat2.PPos.x + cam->MAIN.bat2.PPosOffset.x,
            cam->MAIN.bat2.PPos.y + cam->MAIN.bat2.PPosOffset.y,
            cam->MAIN.bat2.PPos.z + cam->MAIN.bat2.PPosOffset.z
        };
        BA_Vec3 curEnemy = {
            cam->MAIN.bat2.EPos.x,
            cam->MAIN.bat2.EPos.y,
            cam->MAIN.bat2.EPos.z
        };

        // Init smoothing and previous frames
        if (!g_b2_lag_inited) {
            g_b2_smooth_player = curPlayerAnchor;
            g_b2_smooth_enemy  = curEnemy;
            g_b2_lag_inited    = true;
        }
        if (!g_b2_have_prev) {
            g_b2_prev_player = curPlayerAnchor;
            g_b2_prev_enemy  = curEnemy;
            g_b2_have_prev   = true;
        }

        // Alphas from time constants
        float aP = alpha_from_tau_ms(g_b2_tau_player_ms, dt);
        float aE = alpha_from_tau_ms(g_b2_tau_enemy_ms, dt);

        // Catch-up when far
        float pFar = std::fabs(g_b2_smooth_player.x - curPlayerAnchor.x)
                   + std::fabs(g_b2_smooth_player.y - curPlayerAnchor.y);
        float eFar = std::fabs(g_b2_smooth_enemy.x - curEnemy.x)
                   + std::fabs(g_b2_smooth_enemy.y - curEnemy.y);
        if (g_b2_catchup_enable) {
            if (pFar > g_b2_far_thresh_xy) aP = clampf(aP * g_b2_catchup_gain, 0.0f, 1.0f);
            if (eFar > g_b2_far_thresh_xy) aE = clampf(aE * g_b2_catchup_gain, 0.0f, 1.0f);
        }

        // Smooth toward anchors
        v3_lerp_inplace(g_b2_smooth_player, curPlayerAnchor, aP);
        v3_lerp_inplace(g_b2_smooth_enemy,  curEnemy,        aE);

        // Velocities (now XYZ for enemy)
        BA_Vec3 vPlayer = {
            (curPlayerAnchor.x - g_b2_prev_player.x) / dt,
            (curPlayerAnchor.y - g_b2_prev_player.y) / dt,
            (curPlayerAnchor.z - g_b2_prev_player.z) / dt
        };
        BA_Vec3 vEnemy  = {
            (curEnemy.x - g_b2_prev_enemy.x) / dt,
            (curEnemy.y - g_b2_prev_enemy.y) / dt,
            (curEnemy.z - g_b2_prev_enemy.z) / dt
        };

        // Look-ahead
        const float leadSec = g_b2_lookahead_ms * 0.001f;
        BA_Vec3 leadP = {
            vPlayer.x * leadSec * g_b2_lookahead_player_amt,
            vPlayer.y * leadSec * g_b2_lookahead_player_amt,
            0.0f
        };
        BA_Vec3 leadE = {
            vEnemy.x * leadSec * g_b2_lookahead_enemy_amt,
            vEnemy.y * leadSec * g_b2_lookahead_enemy_amt,
            vEnemy.z * leadSec * g_b2_lookahead_enemy_amt
        };

        // Lag deltas (now XYZ for enemy)
        BA_Vec3 pLag = {
            (g_b2_smooth_player.x - curPlayerAnchor.x),
            (g_b2_smooth_player.y - curPlayerAnchor.y),
            0.0f
        };
        BA_Vec3 eLag = {
            (g_b2_smooth_enemy.x - curEnemy.x),
            (g_b2_smooth_enemy.y - curEnemy.y),
            (g_b2_smooth_enemy.z - curEnemy.z)
        };

        // Deadzone (world units) — reuse XY value for Z too
        if (std::fabs(pLag.x) < g_b2_deadzone_xy) pLag.x = 0.0f;
        if (std::fabs(pLag.y) < g_b2_deadzone_xy) pLag.y = 0.0f;
        if (std::fabs(eLag.x) < g_b2_deadzone_xy) eLag.x = 0.0f;
        if (std::fabs(eLag.y) < g_b2_deadzone_xy) eLag.y = 0.0f;
        if (std::fabs(eLag.z) < g_b2_deadzone_xy) eLag.z = 0.0f;

        // Apply strengths + look-ahead
        float cam_dx = pLag.x * g_b2_player_scale_x + leadP.x;
        float cam_dy = pLag.y * g_b2_player_scale_y + leadP.y;
        float tgt_dx = eLag.x * g_b2_enemy_scale_x  + leadE.x;
        float tgt_dy = eLag.y * g_b2_enemy_scale_y  + leadE.y;
        float tgt_dz = eLag.z * g_b2_enemy_scale_z  + leadE.z; // NEW Z path

        // Clamp ranges
        cam_dx = clampf(cam_dx, -g_b2_max_cam_xy, g_b2_max_cam_xy);
        cam_dy = clampf(cam_dy, -g_b2_max_cam_xy, g_b2_max_cam_xy);
        tgt_dx = clampf(tgt_dx, -g_b2_max_tgt_xy, g_b2_max_tgt_xy);
        tgt_dy = clampf(tgt_dy, -g_b2_max_tgt_xy, g_b2_max_tgt_xy);
        tgt_dz = clampf(tgt_dz, -g_b2_max_tgt_z,  g_b2_max_tgt_z);

        // -------- Lock-on jitter killers --------
        // 1) Angular deadzone (XY only): ignore tiny rotations
        {
            const float cx = cam->MAIN.bat2.DebugInfo.CameraPos.x;
            const float cy = cam->MAIN.bat2.DebugInfo.CameraPos.y;
            const float tx = cam->MAIN.bat2.DebugInfo.TargetPos.x;
            const float ty = cam->MAIN.bat2.DebugInfo.TargetPos.y;

            const float dist = std::sqrt((tx - cx) * (tx - cx) + (ty - cy) * (ty - cy));
            if (dist > 0.0001f) {
                const float dlen   = std::sqrt(tgt_dx * tgt_dx + tgt_dy * tgt_dy);
                const float angRad = std::atan2(dlen, dist);
                const float angDeg = angRad * 57.2957795f;
                if (angDeg < g_b2_enemy_ang_deadzone_deg) {
                    tgt_dx = 0.0f;
                    tgt_dy = 0.0f;
                }
            }
        }

        // 2) Slew rate limit for TargetPos — now XYZ
        {
            const float maxStep = g_b2_target_slew_units_per_sec * dt;
            const float len3    = std::sqrt(tgt_dx * tgt_dx + tgt_dy * tgt_dy + tgt_dz * tgt_dz);
            if (len3 > maxStep && len3 > 0.0f) {
                const float s = maxStep / len3;
                tgt_dx *= s;
                tgt_dy *= s;
                tgt_dz *= s;
            }
        }
        // ----------------------------------------

        cam->MAIN.bat2.DebugInfo.CameraPos.x += cam_dx;
        cam->MAIN.bat2.DebugInfo.CameraPos.y += cam_dy;
        cam->MAIN.bat2.DebugInfo.TargetPos.x += tgt_dx;
        cam->MAIN.bat2.DebugInfo.TargetPos.y += tgt_dy;
        cam->MAIN.bat2.DebugInfo.TargetPos.z += tgt_dz; // NEW: apply Z delta

        // Update prevs
        g_b2_prev_player = curPlayerAnchor;
        g_b2_prev_enemy  = curEnemy;
    }
    */

    // -------------------- Deathblow zoom logic (KEEP) --------------------
    // Start condition (SIF)
    if (sif_now && !g_b2_sif_prev) {
        g_b2_sif_time              = now;
        g_b2_zoom_armed            = true;
        g_b2_slow_seen             = false;
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }

    // Arm -> active after delay
    if (g_b2_zoom_armed && !g_b2_zoom_active) {
        if ((now - g_b2_sif_time) >= kB2_SIF_DELAY_SEC) {
            g_b2_zoom_active           = true;
            g_b2_zoom_start_time       = now;
            g_b2_prev_slow_tick        = slowTick;
            g_b2_last_slow_change_time = now;
        }
    }

    if (slowTick > 0)
        g_b2_slow_seen = true;

    // Finish when slow-mo started and then ended
    if (g_b2_slow_seen && slowTick <= 0) {
        g_b2_zoom_active = false;
        g_b2_zoom_armed  = false;
    }

    // Safety nets
    if (g_b2_zoom_active) {
        if ((now - g_b2_last_slow_change_time) >= kB2_SLOW_STALL_SEC) {
            g_b2_zoom_active = false;
            g_b2_zoom_armed  = false;
            g_b2_slow_seen   = false;
        } else if ((now - g_b2_zoom_start_time) >= kB2_ZOOM_MAX_SEC) {
            g_b2_zoom_active = false;
            g_b2_zoom_armed  = false;
        }
    }

    // Drive CameraPos.z
    if (g_b2_zoom_active) {
        const double elapsed = now - g_b2_zoom_start_time;
        const float tNorm    = (float)clamp01(elapsed / kB2_EASE_IN_SEC);
        const float tEase    = smoothstep01(tNorm);

        const float mainTargetZ  = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX * tEase);
        const float creepTargetZ = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX + kB2_Z_EXTRA);

        if (tNorm < 1.0f)
            g_b2_campos_z = lerp(g_b2_campos_z, mainTargetZ, kB2_LERP_IN);
        else
            g_b2_campos_z = lerp(g_b2_campos_z, creepTargetZ, kB2_LERP_CREEP);
    } else {
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
    if (value)
        flags |= (1 << bit);
    else
        flags &= ~(1 << bit);
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
            "HRCAMERA_MODE_HOMING\0HRCAMERA_MODE_MOTION\0HRCAMERA_MODE_FREE\0HRCAMERA_MODE_MOVE\0"
            "HRCAMERA_MODE_BATTLE\0HRCAMERA_MODE_IDLE\0HRCAMERA_MODE_NORMAL\0"
            "HRCAMERA_MODE_BATTLE2\0HRCAMERA_MODE_MOVE2\0");

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
            ImGui::Checkbox("Just Attack", &player->mPcStatus.justAttack);
            ImGui::InputInt("Just Input Tick", &player->mPcStatus.justInputTick);
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
            ImGui::Checkbox("Lock BATTLE2 Camera Preset", &g_b2_lock_preset);

            // ------------- BATTLE2 Lag controls (DISABLED UI) -------------
            /*
            ImGui::SeparatorText("BATTLE2 Lag");
            if (ImGui::BeginTable("b2_lag_table", 2, ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Checkbox("Enable Lag", &g_b2_lag_enable);
                ImGui::TableSetColumnIndex(1);
                ImGui::Checkbox("Catch-up when far", &g_b2_catchup_enable);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Player smooth (ms)", &g_b2_tau_player_ms, 10.0f, 600.0f, "%.0f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Enemy smooth (ms)", &g_b2_tau_enemy_ms, 10.0f, 600.0f, "%.0f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Deadzone", &g_b2_deadzone_xy, 0.0f, 0.20f, "%.3f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Far threshold", &g_b2_far_thresh_xy, 0.2f, 8.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Player strength X", &g_b2_player_scale_x, 0.0f, 2.0f, "%.2f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Enemy strength X", &g_b2_enemy_scale_x, 0.0f, 2.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Player strength Y", &g_b2_player_scale_y, 0.0f, 2.0f, "%.2f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Enemy strength Y", &g_b2_enemy_scale_y, 0.0f, 2.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Enemy strength Z", &g_b2_enemy_scale_z, 0.0f, 2.0f, "%.2f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Max tgt Z", &g_b2_max_tgt_z, 0.2f, 10.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Max cam XY", &g_b2_max_cam_xy, 0.2f, 10.0f, "%.2f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Max tgt XY", &g_b2_max_tgt_xy, 0.2f, 10.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Look-ahead (ms)", &g_b2_lookahead_ms, 0.0f, 200.0f, "%.0f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Enemy ang deadzone (deg)", &g_b2_enemy_ang_deadzone_deg, 0.0f, 2.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Look-ahead player amt", &g_b2_lookahead_player_amt, 0.0f, 2.0f, "%.2f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SliderFloat("Look-ahead enemy amt", &g_b2_lookahead_enemy_amt, 0.0f, 2.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SliderFloat("Target slew (u/s)", &g_b2_target_slew_units_per_sec, 1.0f, 60.0f, "%.1f");
                ImGui::TableSetColumnIndex(1);
                // reserved for future sliders

                ImGui::EndTable();
            }
            */
            // ---------------------------------------------------------------
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
        if (isGuardingNow != lastGuardState) {
            justGuardToggleCount++;
            guardToggleWindow = 30;
        }
        lastGuardState = isGuardingNow;

        if (guardToggleWindow > 0)
            guardToggleWindow--;
        else
            justGuardToggleCount = 0;

        if (justGuardToggleCount >= 4) {
            justGuardCooldown    = 20;
            justGuardToggleCount = 0;
            guardToggleWindow    = 0;
        }

        if (justGuardCooldown > 0) {
            justGuardCooldown--;
            player->mPcStatus.justGuard     = false;
            player->mPcStatus.justInputTick = nmh_sdk::GetJustGuardJudgeTick(player);
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
    if (forceCameraMode)
        ForceCameraModes();
    if (guard_cooldown_enabled)
        GuardCooldown();
    ApplyBattle2PresetTick();
}

void BrainAge::toggleCam(bool enable) {
    if (enable) {
        install_patch_offset(0x3EC616, m_patch1, "\x90\x90\x90\x90\x90\x90", 6);
    } else {
        m_patch1.reset();
    }
}

std::optional<std::string> BrainAge::on_initialize() {
    return Mod::on_initialize();
}
// void BrainAge::on_draw_debug_ui() {}
// void BrainAge::on_config_load(const utility::Config &cfg) {}
// void BrainAge::on_config_save(utility::Config &cfg) {}
#endif
