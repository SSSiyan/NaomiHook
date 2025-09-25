// ASCII-ONLY
#include "BrainAge.hpp"
#if 1
#include <cmath> // expf, fabsf, sqrtf, atan2f, sinf

bool BrainAge::imguiPopout = false;

bool BrainAge::forceCameraMode = false;
int BrainAge::forcedMode       = 0;

bool BrainAge::guard_cooldown_enabled = false;

// ================== Guard Cooldown (press-burst -> timed lockout) ==================
// Tunables
static const int GC_PRESS_COUNT     = 3;    // number of presses required
static const double GC_WINDOW_SEC   = 1.0;  // press window to trigger cooldown
static const double GC_COOLDOWN_SEC = 1.50; // how long we keep parry disabled

// State
static bool gc_prev_down                     = false;           // last frame "lock-on" held?
static bool gc_active                        = false;           // currently disabling parry
static double gc_cooldown_end_time           = 0.0;             // when cooldown ends
static double gc_press_times[GC_PRESS_COUNT] = {0.0, 0.0, 0.0}; // tiny ring buffer
static int gc_press_idx                      = 0;               // next write slot
static int gc_press_total                    = 0;               // total presses seen
static bool gc_saved_flag_valid              = false;           // did we snapshot original flag?
static bool gc_saved_justGuardDisEnable      = false;           // original value of justGuardDisEnable

static inline bool gc_last_n_within(double now, int n, double window_sec) {
    if (gc_press_total < n)
        return false;
    int oldest = (gc_press_total - n) % GC_PRESS_COUNT;
    return (now - gc_press_times[oldest]) <= window_sec;
}

// ====================================================================================

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

// Tuning (deathblow zoom)
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

// -------------------- Rails tuning --------------------
// Z rail (depth) based on planar P-E distance
static const float kB2_RAIL_DIST_NEAR = 4.0f;
static const float kB2_RAIL_DIST_FAR  = 24.0f;
static const float kB2_RAIL_Z_NEAR    = kB2_CamPos_Z_BASE - 8.0f;
static const float kB2_RAIL_Z_FAR     = kB2_CamPos_Z_BASE + 6.0f;
static const float kB2_RAIL_Z_LERP    = 0.08f;

// Left/Right ARC rail (non-zoom only)
// Map the signed angle between the base camera direction (Target->BaseCam) and the
// current Target->Player vector into a yaw offset around the target. This creates a true
// orbital left/right feel instead of just sliding in world X.
static const float kB2_LR_YAW_MAX_DEG = 14.0f; // hard clamp for arc
static const float kB2_LR_YAW_GAIN    = 0.55f; // scale from signed angle to target yaw
static const float kB2_LR_LERP        = 0.12f; // smoothing for yaw response
static float g_b2_lr_yaw_cur          = 0.0f;  // smoothed yaw (radians)

// === NEW: Shinku Over-Shoulder, Rail 2.0, LR Player Bias (additive) ===
// Shinku over-shoulder (applies only when shinku active AND PPosOffset is zero)
static const float kB2_SHINKU_OVER_X = 0.35f;  // +right
static const float kB2_SHINKU_OVER_Z = -0.80f; // toward enemy
static const float kB2_SHINKU_OVER_Y = 0.15f;  // slight lift
static const float kB2_SHINKU_BLEND  = 0.25f;  // blend toward shoulder
static float g_b2_ppos_add_x         = 0.0f;   // persistent shoulder add
static float g_b2_ppos_add_y         = 0.0f;
static float g_b2_ppos_add_z         = 0.0f;

// Rail 2.0 soft bands and hysteresis (adds on top of existing rail)
static const float kB2_RAIL_SOFT_IN    = 6.0f;  // begin soft push-out near min
static const float kB2_RAIL_SOFT_OUT   = 18.0f; // begin soft pull-in near max
static const float kB2_RAIL_HYSTERESIS = 0.15f; // meters; ignore tiny target changes

// LR orbit with player bias (multiplies yaw target when player is not facing enemy)
static const float kB2_LR_BIAS_MULT = 1.75f; // 1.0 = none, 2.0 = strong
static float g_b2_prev_ppos_x       = 0.0f;  // for movement-based forward estimate
static float g_b2_prev_ppos_z       = 0.0f;
static bool g_b2_prev_ppos_init     = false;

// -------------------- helpers --------------------
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
static inline float deg2rad(float d) {
    return d * 3.1415926535f / 180.0f;
}
static inline float rad_wrap_pi(float a) {
    // wrap to [-pi, pi]
    while (a > 3.1415926535f)
        a -= 6.283185307f;
    while (a < -3.1415926535f)
        a += 6.283185307f;
    return a;
}
static inline float deadband(float v, float band) {
    return (fabsf(v) <= band) ? 0.0f : (v > 0.0f ? v - band : v + band);
}

// ---------------------------------------------------------------

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

        g_b2_campos_z   = kB2_CamPos_Z_BASE;
        g_b2_zoom_armed = g_b2_zoom_active = g_b2_slow_seen = false;
        g_b2_prev_slow_tick                                 = slowTick;
        g_b2_last_slow_change_time                          = now;
        g_b2_lr_yaw_cur                                     = 0.0f;
    }

    if (!g_b2_lock_preset) {
        g_b2_campos_z   = lerp(g_b2_campos_z, kB2_CamPos_Z_BASE, kB2_LERP_OUT);
        g_b2_prev_lock  = g_b2_lock_preset;
        g_b2_sif_prev   = sif_now;
        g_b2_lr_yaw_cur = lerp(g_b2_lr_yaw_cur, 0.0f, 0.25f);
        return;
    }

    // Locked preset path
    cam->MAIN.bat2.DebugMode = true;

    // Base XY; Z driven below; X may be offset by LR arc when NOT zooming
    float camPosX = kB2_CamPos_X;
    float camPosY = kB2_CamPos_Y;

    cam->MAIN.bat2.DebugInfo.TargetPos.x = kB2_TgtPos_X;
    cam->MAIN.bat2.DebugInfo.TargetPos.y = kB2_TgtPos_Y;
    cam->MAIN.bat2.DebugInfo.TargetPos.z = kB2_TgtPos_Z;

    // -------------------- Deathblow zoom logic (unchanged) --------------------
    if (sif_now && !g_b2_sif_prev) {
        g_b2_sif_time              = now;
        g_b2_zoom_armed            = true;
        g_b2_slow_seen             = false;
        g_b2_prev_slow_tick        = slowTick;
        g_b2_last_slow_change_time = now;
    }
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
    if (g_b2_slow_seen && slowTick <= 0) {
        g_b2_zoom_active = false;
        g_b2_zoom_armed  = false;
    }
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

    // -------------------- Z drive (zoom vs rail) --------------------
    if (g_b2_zoom_active) {
        const double elapsed     = now - g_b2_zoom_start_time;
        const float tNorm        = (float)clamp01(elapsed / kB2_EASE_IN_SEC);
        const float tEase        = smoothstep01(tNorm);
        const float mainTargetZ  = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX * tEase);
        const float creepTargetZ = kB2_CamPos_Z_BASE - (kB2_Z_PULL_MAX + kB2_Z_EXTRA);
        g_b2_campos_z = (tNorm < 1.0f) ? lerp(g_b2_campos_z, mainTargetZ, kB2_LERP_IN) : lerp(g_b2_campos_z, creepTargetZ, kB2_LERP_CREEP);
    } else {
        // Rail based on planar P-E distance
        const float dx          = cam->MAIN.bat2.PPos.x - cam->MAIN.bat2.EPos.x;
        const float dz          = cam->MAIN.bat2.PPos.z - cam->MAIN.bat2.EPos.z;
        const float dist        = sqrtf(dx * dx + dz * dz);
        float tz                = (dist - kB2_RAIL_DIST_NEAR) / (kB2_RAIL_DIST_FAR - kB2_RAIL_DIST_NEAR);
        tz                      = clamp01(tz);
        const float railTargetZ = lerp(kB2_RAIL_Z_NEAR, kB2_RAIL_Z_FAR, smoothstep01(tz));
        g_b2_campos_z           = lerp(g_b2_campos_z, railTargetZ, kB2_RAIL_Z_LERP);

        // --- ADDITIVE Rail 2.0: soften edges + hysteresis (overrides prior lerp result) ---
        {
            const float centerZ = 0.5f * (kB2_RAIL_Z_NEAR + kB2_RAIL_Z_FAR);
            const float tIn     = clamp01((dist - kB2_RAIL_DIST_NEAR) / (kB2_RAIL_SOFT_IN - kB2_RAIL_DIST_NEAR));
            const float tOut    = clamp01((dist - kB2_RAIL_SOFT_OUT) / (kB2_RAIL_DIST_FAR - kB2_RAIL_SOFT_OUT));
            const float biasIn  = 1.0f - tIn; // near min -> push outward
            const float biasOut = tOut;       // near max -> pull inward
            const float signedB = biasIn - biasOut;

            float softTargetZ    = centerZ + signedB * ((kB2_RAIL_Z_FAR - kB2_RAIL_Z_NEAR) * 0.33f);
            float blendedTargetZ = lerp(railTargetZ, softTargetZ, 0.5f);

            float deltaZ   = blendedTargetZ - g_b2_campos_z;
            deltaZ         = deadband(deltaZ, kB2_RAIL_HYSTERESIS);
            blendedTargetZ = g_b2_campos_z + deltaZ;

            g_b2_campos_z = lerp(g_b2_campos_z, blendedTargetZ, kB2_RAIL_Z_LERP);
        }
    }

    // Commit Z first
    cam->MAIN.bat2.DebugInfo.CameraPos.z = g_b2_campos_z;

    // -------------------- NEW: True LR orbit (non-zoom only) --------------------
    if (!g_b2_zoom_active) {
        // Base reference vector: Target -> BaseCam (XZ)
        const float refX   = (kB2_CamPos_X - kB2_TgtPos_X);
        const float refZ   = (g_b2_campos_z - kB2_TgtPos_Z); // use current Z rail for radius
        const float radius = sqrtf(refX * refX + refZ * refZ);
        float refYaw       = atan2f(refZ, refX);

        // Player vector: Target -> Player (XZ)
        const float pX = cam->MAIN.bat2.PPos.x - kB2_TgtPos_X;
        const float pZ = cam->MAIN.bat2.PPos.z - kB2_TgtPos_Z;
        float pYaw     = atan2f(pZ, pX);

        // Signed delta yaw, scaled and clamped
        float dYaw         = rad_wrap_pi(pYaw - refYaw);
        float yawTarget    = dYaw * kB2_LR_YAW_GAIN;
        const float yawMax = deg2rad(kB2_LR_YAW_MAX_DEG);
        if (yawTarget > yawMax)
            yawTarget = yawMax;
        if (yawTarget < -yawMax)
            yawTarget = -yawMax;

        // --- ADDITIVE: Player-bias multiplier using movement direction vs. toEN ---
        {
            float biasMult   = 1.0f;
            const float curX = cam->MAIN.bat2.PPos.x;
            const float curZ = cam->MAIN.bat2.PPos.z;
            if (!g_b2_prev_ppos_init) {
                g_b2_prev_ppos_x    = curX;
                g_b2_prev_ppos_z    = curZ;
                g_b2_prev_ppos_init = true;
            }
            const float vX   = curX - g_b2_prev_ppos_x;
            const float vZ   = curZ - g_b2_prev_ppos_z;
            const float vLen = sqrtf(vX * vX + vZ * vZ);

            // toEN vector from player to enemy (XZ)
            const float toX   = (kB2_TgtPos_X - curX);
            const float toZ   = (kB2_TgtPos_Z - curZ);
            const float toLen = sqrtf(toX * toX + toZ * toZ) + 1e-6f;

            // Forward estimate: movement direction if moving, otherwise default toward enemy
            float fx = toX / toLen, fz = toZ / toLen;
            if (vLen > 0.01f) {
                float inv = 1.0f / vLen;
                fx        = vX * inv;
                fz        = vZ * inv;
            }

            // Angle between forward estimate and toEN (0..pi) -> 0..1 bias
            float c = ((fx * toX + fz * toZ) / (toLen));
            if (c > 1.0f)
                c = 1.0f;
            if (c < -1.0f)
                c = -1.0f;
            float ang    = acosf(c);
            float bias01 = ang / 3.1415926535f; // 0 facing enemy, 1 facing away

            biasMult = lerp(1.0f, kB2_LR_BIAS_MULT, bias01);
            yawTarget *= biasMult;

            g_b2_prev_ppos_x = curX;
            g_b2_prev_ppos_z = curZ;
        }

        // Smooth
        g_b2_lr_yaw_cur = lerp(g_b2_lr_yaw_cur, yawTarget, kB2_LR_LERP);

        // Convert yaw to X offset along orbit around target; keep Z fixed from the rail.
        const float deltaX = radius * sinf(g_b2_lr_yaw_cur);
        camPosX            = kB2_CamPos_X + deltaX;
    } else {
        // During zoom, freeze LR and recenter X so deathblow shot is unaffected
        g_b2_lr_yaw_cur = lerp(g_b2_lr_yaw_cur, 0.0f, 0.25f);
        camPosX         = kB2_CamPos_X;
    }

    // Commit XY
    cam->MAIN.bat2.DebugInfo.CameraPos.x = camPosX;
    cam->MAIN.bat2.DebugInfo.CameraPos.y = camPosY;

    // === ADDITIVE: Shinku over-shoulder offset on PPosOffset (non-destructive) ===
    {
        const bool shinkuActive = (player && player->mPcStatus.shinkuTick > 0);
        // Snapshot base (so we don't accumulate on already-modified values)
        const float baseX       = cam->MAIN.bat2.PPosOffset.x;
        const float baseY       = cam->MAIN.bat2.PPosOffset.y;
        const float baseZ       = cam->MAIN.bat2.PPosOffset.z;
        const bool offsetIsZero = (fabsf(baseX) < 1e-4f && fabsf(baseY) < 1e-4f && fabsf(baseZ) < 1e-4f);

        if (shinkuActive && offsetIsZero) {
            // Build shoulder offset from PC->EN direction on XZ
            float fx   = cam->MAIN.bat2.EPos.x - cam->MAIN.bat2.PPos.x;
            float fz   = cam->MAIN.bat2.EPos.z - cam->MAIN.bat2.PPos.z;
            float fLen = sqrtf(fx * fx + fz * fz) + 1e-6f;
            fx /= fLen;
            fz /= fLen;

            // right = cross(up, forward) on XZ -> (-fz, +fx)
            float rx = -fz, rz = fx;

            float addX = rx * kB2_SHINKU_OVER_X + fx * kB2_SHINKU_OVER_Z;
            float addY = kB2_SHINKU_OVER_Y;
            float addZ = rz * kB2_SHINKU_OVER_X + fz * kB2_SHINKU_OVER_Z;

            g_b2_ppos_add_x = lerp(g_b2_ppos_add_x, addX, kB2_SHINKU_BLEND);
            g_b2_ppos_add_y = lerp(g_b2_ppos_add_y, addY, kB2_SHINKU_BLEND);
            g_b2_ppos_add_z = lerp(g_b2_ppos_add_z, addZ, kB2_SHINKU_BLEND);
        } else {
            // decay toward zero when not active or base offset is non-zero
            g_b2_ppos_add_x = lerp(g_b2_ppos_add_x, 0.0f, 0.25f);
            g_b2_ppos_add_y = lerp(g_b2_ppos_add_y, 0.0f, 0.25f);
            g_b2_ppos_add_z = lerp(g_b2_ppos_add_z, 0.0f, 0.25f);
        }

        // Non-destructive write (base + addition)
        cam->MAIN.bat2.PPosOffset.x = baseX + g_b2_ppos_add_x;
        cam->MAIN.bat2.PPosOffset.y = baseY + g_b2_ppos_add_y;
        cam->MAIN.bat2.PPosOffset.z = baseZ + g_b2_ppos_add_z;
    }

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

            const double now2 = ImGui::GetTime();
            bool active       = gc_active && (now2 < gc_cooldown_end_time);
            double remain     = active ? (gc_cooldown_end_time - now2) : 0.0;
            ImGui::Text("Cooldown Active: %s", active ? "Yes" : "No");
            if (active)
                ImGui::Text("Time Remaining: %.2f sec", remain);
        }
    }

    if (ImGui::CollapsingHeader("New thing 3")) {
        mHRPc* player2 = nmh_sdk::get_mHRPc();
        if (player2) {
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
    if (!player) {
        gc_active           = false;
        gc_prev_down        = false;
        gc_saved_flag_valid = false;
        gc_press_idx        = 0;
        gc_press_total      = 0;
        for (int i = 0; i < GC_PRESS_COUNT; ++i)
            gc_press_times[i] = 0.0;
        return;
    }

    const double now = ImGui::GetTime();

    if (!guard_cooldown_enabled) {
        if (gc_active && gc_saved_flag_valid) {
            player->mPcStatus.justGuardDisEnable = gc_saved_justGuardDisEnable;
        }
        gc_active           = false;
        gc_saved_flag_valid = false;
        gc_prev_down        = (player->mPcStatus.justInputTick > 0);
        return;
    }

    const bool down = (player->mPcStatus.justInputTick > 0);

    if (down && !gc_prev_down) {
        if (gc_active && now < gc_cooldown_end_time) {
            gc_cooldown_end_time = now + GC_COOLDOWN_SEC;
        } else {
            gc_press_times[gc_press_idx] = now;
            gc_press_idx                 = (gc_press_idx + 1) % GC_PRESS_COUNT;
            gc_press_total++;
            if (gc_last_n_within(now, GC_PRESS_COUNT, GC_WINDOW_SEC)) {
                gc_active                   = true;
                gc_cooldown_end_time        = now + GC_COOLDOWN_SEC;
                gc_saved_justGuardDisEnable = player->mPcStatus.justGuardDisEnable;
                gc_saved_flag_valid         = true;
                gc_press_idx                = 0;
                gc_press_total              = 0;
                for (int i = 0; i < GC_PRESS_COUNT; ++i)
                    gc_press_times[i] = 0.0;
            }
        }
    }

    if (gc_active) {
        if (now < gc_cooldown_end_time) {
            player->mPcStatus.justGuardDisEnable = true;
            player->mPcStatus.justGuard          = false;
            player->mPcStatus.justInputTick      = 0;
        } else {
            gc_active = false;
            if (gc_saved_flag_valid) {
                player->mPcStatus.justGuardDisEnable = gc_saved_justGuardDisEnable;
            }
            gc_saved_flag_valid = false;
        }
    }

    gc_prev_down = down;
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
