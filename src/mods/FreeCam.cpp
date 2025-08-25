#include "FreeCam.hpp"
#include "GuiFunctions.hpp" // for g_framework
#include "KbmControls.hpp"  // for gamepad button structs
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <cmath> // sin, cos, atan2, fabsf, sqrtf
#include <optional>

#if 1
static bool mod_enabled            = false;
PAD_UNI* pad                       = nullptr;
float FreeCam::sens                = 0.0f;
float FreeCam::modifierSens        = 0.0f;
float FreeCam::deadZone            = 0.0f;
bool FreeCam::toggle_pause_enabled = false;

// frame-advance state 
static bool g_step_pending             = false;
static bool g_step_unpaused_this_frame = false;
static bool g_step_restore_pause       = false;

// ==========================================================================
// FreeCam Crap
// ==========================================================================

// Look-At Lock (orientation)
static bool g_look_enabled         = false;
static bool g_look_target_player   = true;               // if false, uses g_look_custom
static float g_look_strength       = 0.35f;              // 0..1, blend each frame toward target aim
static float g_look_max_pitch      = 1.50f;              // matches your existing clamp
static float g_look_responsiveness = 10.0f;              // Hz (how quickly to converge)
static Vec g_look_custom           = {0.0f, 0.0f, 0.0f}; // optional custom point

// Look-At aim offsets (adjust X/Y of the point we look at)
static float g_look_aim_offset_x = 0.0f; // world X offset added to aim target
static float g_look_aim_offset_y = 0.0f; // world Y offset added to aim target

// Look-At Follow (position) to "keep up" with target while using Look-At Lock
static bool g_look_follow_enabled     = false;                // enable position follow
static bool g_look_follow_have_offset = false;                // internal: captured once when enabling
static Vec g_look_follow_offset       = {0.0f, 6.0f, -12.0f}; // world-space offset from target (captured from current by default)
static float g_look_follow_damp_hz    = 8.0f;                 // smoothing for camera position toward target+offset

// Turntable Orbit
static bool g_orbit_enabled        = false;
static bool g_orbit_follow_player  = true; // center follows player each frame
static bool g_orbit_face_center    = true; // force camera look-at center while orbiting
static Vec g_orbit_center          = {0.0f, 0.0f, 0.0f};
static float g_orbit_radius        = 12.0f;
static float g_orbit_height_offset = 2.0f;
static float g_orbit_speed_deg     = 35.0f; // deg/sec
static float g_orbit_angle_deg     = 0.0f;  // accumulates over time

// Orbit center offsets (adjust X/Y of the orbit center)
static float g_orbit_center_off_x = 0.0f; // world X offset added to center
static float g_orbit_center_off_y = 0.0f; // world Y offset added to center

// Guides & Shot HUD
static bool g_guides_enabled = false;
static bool g_guide_thirds   = true;
static bool g_guide_cross    = true;
static bool g_guide_golden   = false; // golden ratio grid
static bool g_guide_safe     = false;
static bool g_guide_horizon  = false; // horizontal midline
static float g_guide_opacity = 0.55f; // 0..1
static float g_safe_aspect   = 2.35f; // widescreen letterbox frame

static bool g_hud_enabled = true;

// Helpers
static inline float rad_to_deg(float r) {
    return r * 57.2957795131f;
}
static inline float deg_to_rad(float d) {
    return d * 0.01745329252f;
}

static inline float wrap_angle(float a) {
    const float PI  = 3.14159265359f;
    const float TAU = 6.28318530718f;
    if (a > PI)
        a -= TAU;
    if (a < -PI)
        a += TAU;
    return a;
}

static inline float damp_alpha(float freq_hz, float dt) {
    // alpha = 1 - exp(-2*pi*f*dt)
    float f = std::max(0.0f, freq_hz);
    return 1.0f - expf(-6.2831853f * f * dt);
}

static void apply_look_at_lock(HrCamera* cam, const Vec& target, float dt) {
    Vec camPos = cam->MAIN.free.C_T_Pos;
    float dx   = target.x - camPos.x;
    float dy   = target.y - camPos.y;
    float dz   = target.z - camPos.z;

    float desired_yaw   = atan2f(dx, dz); // yaw as atan2(x, z)
    float horiz_len     = sqrtf(dx * dx + dz * dz);
    float desired_pitch = atan2f(dy, std::max(0.0001f, horiz_len));
    desired_pitch       = std::max(-g_look_max_pitch, std::min(g_look_max_pitch, desired_pitch));

    float cur_yaw   = cam->MAIN.free.T_YDir;
    float cur_pitch = cam->MAIN.free.T_VDir;

    float ay = wrap_angle(desired_yaw - cur_yaw);
    float ap = wrap_angle(desired_pitch - cur_pitch);

    float a               = damp_alpha(g_look_responsiveness * g_look_strength, dt);
    cam->MAIN.free.T_YDir = cur_yaw + ay * a;
    cam->MAIN.free.T_VDir = cur_pitch + ap * a;

    cam->MAIN.free.YDir = cam->MAIN.free.T_YDir;
    cam->MAIN.free.VDir = cam->MAIN.free.T_VDir;
}

// Follow the target position using a captured world offset and damping
static void apply_look_follow(HrCamera* cam, const Vec& target, float dt) {
    Vec desired;
    desired.x = target.x + g_look_follow_offset.x;
    desired.y = target.y + g_look_follow_offset.y;
    desired.z = target.z + g_look_follow_offset.z;

    float a = damp_alpha(g_look_follow_damp_hz, dt);

    cam->MAIN.free.C_T_Pos.x += (desired.x - cam->MAIN.free.C_T_Pos.x) * a;
    cam->MAIN.free.C_T_Pos.y += (desired.y - cam->MAIN.free.C_T_Pos.y) * a;
    cam->MAIN.free.C_T_Pos.z += (desired.z - cam->MAIN.free.C_T_Pos.z) * a;
}

static void apply_turntable_orbit(HrCamera* cam, const Vec& center, float dt) {
    g_orbit_angle_deg += g_orbit_speed_deg * dt;
    if (g_orbit_angle_deg >= 360.0f)
        g_orbit_angle_deg -= 360.0f;
    if (g_orbit_angle_deg < 0.0f)
        g_orbit_angle_deg += 360.0f;

    float ang = deg_to_rad(g_orbit_angle_deg);
    float cx  = center.x;
    float cy  = center.y + g_orbit_height_offset;
    float cz  = center.z;

    float x = cx + sinf(ang) * g_orbit_radius;
    float z = cz + cosf(ang) * g_orbit_radius;

    cam->MAIN.free.C_T_Pos.x = x;
    cam->MAIN.free.C_T_Pos.y = cy;
    cam->MAIN.free.C_T_Pos.z = z;

    if (g_orbit_face_center) {
        apply_look_at_lock(cam, center, 1.0f / 60.0f); // small dt; orbit is deterministic
    }
}

static void draw_guides_and_hud(HrCamera* cam) {
    ImGuiIO& io    = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetForegroundDrawList(); // top-most overlay

    const float W = io.DisplaySize.x;
    const float H = io.DisplaySize.y;
    if (W <= 1.0f || H <= 1.0f)
        return;

    const int a     = (int)(255.0f * std::max(0.0f, std::min(1.0f, g_guide_opacity)));
    const ImU32 col = IM_COL32(255, 255, 255, a);

    if (g_guides_enabled) {
        // Rule of thirds
        if (g_guide_thirds) {
            float x1 = W / 3.0f;
            float x2 = 2.0f * W / 3.0f;
            float y1 = H / 3.0f;
            float y2 = 2.0f * H / 3.0f;
            dl->AddLine(ImVec2(x1, 0.0f), ImVec2(x1, H), col, 1.0f);
            dl->AddLine(ImVec2(x2, 0.0f), ImVec2(x2, H), col, 1.0f);
            dl->AddLine(ImVec2(0.0f, y1), ImVec2(W, y1), col, 1.0f);
            dl->AddLine(ImVec2(0.0f, y2), ImVec2(W, y2), col, 1.0f);
        }

        // Center crosshair
        if (g_guide_cross) {
            float cx  = W * 0.5f;
            float cy  = H * 0.5f;
            float len = 8.0f;
            dl->AddLine(ImVec2(cx - len, cy), ImVec2(cx + len, cy), col, 1.0f);
            dl->AddLine(ImVec2(cx, cy - len), ImVec2(cx, cy + len), col, 1.0f);
        }

        // Golden ratio grid (I think back to that Calibur meme everytime)
        if (g_guide_golden) {
            const float phi = 1.6180339887f;
            float gx        = W / phi; // vertical division
            float gy        = H / phi; // horizontal division
            dl->AddLine(ImVec2(gx, 0.0f), ImVec2(gx, H), col, 1.0f);
            dl->AddLine(ImVec2(0.0f, gy), ImVec2(W, gy), col, 1.0f);
        }

        // Safe frame box with user aspect
        if (g_guide_safe && g_safe_aspect > 0.01f) {
            float wantAspect = g_safe_aspect;
            float frameW     = W;
            float frameH     = frameW / wantAspect;
            if (frameH > H) {
                frameH = H;
                frameW = frameH * wantAspect;
            }
            float x0 = (W - frameW) * 0.5f;
            float y0 = (H - frameH) * 0.5f;
            float x1 = x0 + frameW;
            float y1 = y0 + frameH;
            dl->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), col, 0.0f, 0, 1.0f);
        }

        // Horizon (screen midline)
        if (g_guide_horizon) {
            float y = H * 0.5f;
            dl->AddLine(ImVec2(0.0f, y), ImVec2(W, y), col, 1.0f);
        }
    }

    // Shot HUD
    if (g_hud_enabled && cam) {
        ImVec2 pad(8.0f, 6.0f);
        ImVec2 pos(12.0f, 12.0f);
        const float line_h = ImGui::GetFontSize() + 2.0f;

        // Compose text
        char buf[256];
        Vec p       = cam->MAIN.free.C_T_Pos;
        float yaw   = cam->MAIN.free.YDir;
        float pitch = cam->MAIN.free.VDir;
        float roll  = cam->MAIN.TwistAngle;

        // Box background and border
        ImU32 bg = IM_COL32(0, 0, 0, 140);
        ImU32 br = IM_COL32(255, 255, 255, 40);

        // increased to 7 lines
        float box_w = 260.0f;
        float box_h = line_h * 7 + pad.y * 2.0f;

        dl->AddRectFilled(pos, ImVec2(pos.x + box_w, pos.y + box_h), bg, 6.0f);
        dl->AddRect(pos, ImVec2(pos.x + box_w, pos.y + box_h), br, 6.0f);

        // Text origin = pos + pad
        ImVec2 text_pos(pos.x + pad.x, pos.y + pad.y);
        ImVec2 cur = text_pos;

        // Lines
        snprintf(buf, sizeof(buf), "Pos:  %.2f  %.2f  %.2f", p.x, p.y, p.z);
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        snprintf(buf, sizeof(buf), "Yaw/Pitch/Roll:  %.1f  %.1f  %.1f", rad_to_deg(yaw), rad_to_deg(pitch), rad_to_deg(roll));
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        float curSpeed = FreeCam::sens;
        snprintf(buf, sizeof(buf), "Speed:  %.2f  (Mod: %.2f)", curSpeed, FreeCam::modifierSens);
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        snprintf(buf, sizeof(buf), "Look-Lock: %s  (%.2f)", g_look_enabled ? "ON" : "OFF", g_look_strength);
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        // show follow state
        snprintf(buf, sizeof(buf), "Follow: %s  d=%.1f hz=%.1f", g_look_follow_enabled ? "ON" : "OFF",
            sqrtf(g_look_follow_offset.x * g_look_follow_offset.x + g_look_follow_offset.y * g_look_follow_offset.y +
                  g_look_follow_offset.z * g_look_follow_offset.z),
            g_look_follow_damp_hz);
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        snprintf(buf, sizeof(buf), "Orbit: %s  r=%.1f  s=%.0f deg/s", g_orbit_enabled ? "ON" : "OFF", g_orbit_radius, g_orbit_speed_deg);
        dl->AddText(cur, IM_COL32_WHITE, buf);
        cur.y += line_h;

        snprintf(buf, sizeof(buf), "Guides: %s   HUD: %s", g_guides_enabled ? "ON" : "OFF", g_hud_enabled ? "ON" : "OFF");
        dl->AddText(cur, IM_COL32_WHITE, buf);
    }
}

// ==========================================================================

const char* FreeCam::defaultDescription = "Controls:\n- Left Stick and Right Stick = Movement and Rotation\n"
                                          "- L2 & R2 = Move Up and Down\n- L1 & R1 = Roll Left/Right\n- L3 = Use Modifier Speed\n- R3 = "
                                          "Reset\n- F1 & F2 = Enter Free Cam, Pause\n"
                                          "- F3 = Frame Advance (while paused)";
const char* FreeCam::hoveredDescription = defaultDescription;

void FreeCam::render_description() const {
    ImGui::TextWrapped(FreeCam::hoveredDescription);
}

void FreeCam::toggle(bool enable) {
    if (enable) {
        // stops cam being set back to 8 mid battle
        install_patch_offset(0x3B635F, battle_freecam_patch, "\xEB\x07", 2); // jmp nmh.mHRBattle::mFrameProc+208
        // stops cam being set back to 8 mid bike cam
        install_patch_offset(0x3B55BA, bike_freecam_patch, "\xEB\x35", 2); // jmp nmh.mHRBattle::mCameraFrameProc+D81
    } else {
        battle_freecam_patch.reset();
        bike_freecam_patch.reset();
    }
}

void FreeCam::togglePause(bool enable) {
    if (enable) {
        install_patch_offset(0x3B643B, pause_all_patch, "\xEB", 1); // jmp nmh.exe+3B649F
    } else {
        pause_all_patch.reset(); // je nmh.exe+3B649F
    }
}

std::optional<std::string> FreeCam::on_initialize() {
    uintptr_t padAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D10);
    pad               = (PAD_UNI*)padAddr;
    return Mod::on_initialize();
}

void FreeCam::on_frame() {

    if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
        mod_enabled = !mod_enabled;
        toggle(mod_enabled);
        toggle_pause_enabled = mod_enabled;
        togglePause(toggle_pause_enabled);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_F2)) {
        toggle_pause_enabled = !toggle_pause_enabled;
        togglePause(toggle_pause_enabled);
    }

    // F3 = frame advance (when paused)
    if (ImGui::IsKeyPressed(ImGuiKey_F3)) {
        if (mod_enabled && toggle_pause_enabled && !g_step_pending && !g_step_unpaused_this_frame) {
            g_step_pending       = true; // request begins this frame
            g_step_restore_pause = true; // remember to restore pause after 1 frame
        }
    }

    // frame-advance state machine
    if (g_step_pending) {
        toggle_pause_enabled = false;
        togglePause(false);
        g_step_unpaused_this_frame = true;
        g_step_pending             = false;
    } else if (g_step_unpaused_this_frame) {
        if (g_step_restore_pause) {
            toggle_pause_enabled = true;
            togglePause(true);
        }
        g_step_unpaused_this_frame = false;
        g_step_restore_pause       = false;
    }

    if (!mod_enabled) {
        return;
    }

    HrCamera* cam = nmh_sdk::get_HrCamera();
    if (!cam) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    float dt    = io.DeltaTime > 0.0f ? io.DeltaTime : (1.0f / 60.0f);

    if (cam->MAIN.Mode != HRCAMERA_MODE_FREE) {
        cam->MAIN.Mode = HRCAMERA_MODE_FREE;
    }

    if (!pad) {
        return;
    }

    auto buttons = pad->WiiStatusBuffer[0][0].ex_status.cl.hold;
    auto& cl     = pad->WiiStatusBuffer[0][0].ex_status.cl;

    float currentSens = sens;
    float rotSens     = 0.1f;
    float rollSens    = 0.01f;

    // camera sprint
    if (buttons & KEY_L3) {
        currentSens = modifierSens;
        rollSens    = modifierSens * 0.02f;
    }

    // get forward and right camera facing
    float yaw    = cam->MAIN.free.YDir;
    float cosYaw = cosf(yaw);
    float sinYaw = sinf(yaw);
    Vec forward  = {sinYaw, 0.0f, cosYaw};
    Vec right    = {cosYaw, 0.0f, -sinYaw};

    // Turntable Orbit (before manual movement)
    mHRPc* player_for_centers = nmh_sdk::get_mHRPc();
    if (g_orbit_enabled) {
        Vec center = g_orbit_center;
        if (g_orbit_follow_player && player_for_centers) {
            center.x = player_for_centers->mCharaStatus.pos.x;
            center.y = player_for_centers->mCharaStatus.pos.y;
            center.z = player_for_centers->mCharaStatus.pos.z;
        }
        // apply requested X/Y offsets to the orbit center
        center.x += g_orbit_center_off_x;
        center.y += g_orbit_center_off_y;

        apply_turntable_orbit(cam, center, dt);
    }

    // Movement (disabled while orbiting so they don't fight)
    if (!g_orbit_enabled) {
        // triggers up/down
        if (buttons & KEY_LT) {
            cam->MAIN.free.C_T_Pos.y -= currentSens;
        }
        if (buttons & KEY_RT) {
            cam->MAIN.free.C_T_Pos.y += currentSens;
        }

        // left stick
        float leftX = cl.lstick.x;
        float leftY = cl.lstick.y;

        if (fabsf(leftX) > deadZone) {
            cam->MAIN.free.C_T_Pos.x -= right.x * leftX * currentSens;
            cam->MAIN.free.C_T_Pos.z -= right.z * leftX * currentSens;
        }
        if (fabsf(leftY) > deadZone) {
            cam->MAIN.free.C_T_Pos.x += forward.x * leftY * currentSens;
            cam->MAIN.free.C_T_Pos.z += forward.z * leftY * currentSens;
        }
    }

    // right stick rotation
    float rightX = cl.rstick.x;
    float rightY = cl.rstick.y;

    if (fabsf(rightX) > deadZone) {
        cam->MAIN.free.T_YDir -= rightX * rotSens;
        cam->MAIN.free.YDir = cam->MAIN.free.T_YDir;
    }
    if (fabsf(rightY) > deadZone) {
        cam->MAIN.free.T_VDir += rightY * rotSens;
        cam->MAIN.free.VDir = cam->MAIN.free.T_VDir;

        // dont go past up/down
        if (cam->MAIN.free.T_VDir > 1.5f)
            cam->MAIN.free.T_VDir = 1.5f;
        if (cam->MAIN.free.T_VDir < -1.5f)
            cam->MAIN.free.T_VDir = -1.5f;
        cam->MAIN.free.VDir = cam->MAIN.free.T_VDir;
    }

    // roll on l1/r1
    if (buttons & KEY_LB) {
        cam->MAIN.TwistAngle += rollSens;
    }
    if (buttons & KEY_RB) {
        cam->MAIN.TwistAngle -= rollSens;
    }

    // reset cam
    if (buttons & KEY_R3) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            cam->MAIN.free.C_T_Pos.x = player->mCharaStatus.pos.x;
            cam->MAIN.free.C_T_Pos.y = player->mCharaStatus.pos.y + 20.0f;
            cam->MAIN.free.C_T_Pos.z = player->mCharaStatus.pos.z;
            cam->MAIN.TwistAngle     = 0.0f;
        }
    }

    // --- Build a single target ---
    Vec base_tgt = g_look_custom;
    if (g_look_target_player && player_for_centers) {
        base_tgt.x = player_for_centers->mCharaStatus.pos.x;
        base_tgt.y = player_for_centers->mCharaStatus.pos.y;
        base_tgt.z = player_for_centers->mCharaStatus.pos.z;
    }

    // Look-At Follow (position) before Look-At orientation so aim uses new position
    if (g_look_enabled && g_look_follow_enabled && !g_orbit_enabled) {
        // capture offset once when enabling follow, if not captured yet
        if (!g_look_follow_have_offset) {
            g_look_follow_offset.x    = cam->MAIN.free.C_T_Pos.x - base_tgt.x;
            g_look_follow_offset.y    = cam->MAIN.free.C_T_Pos.y - base_tgt.y;
            g_look_follow_offset.z    = cam->MAIN.free.C_T_Pos.z - base_tgt.z;
            g_look_follow_have_offset = true;
        }
        apply_look_follow(cam, base_tgt, dt);
    } else {
        g_look_follow_have_offset = false; // reset so next enable re-captures
    }

    // Apply aim offsets only to the look-at target (does not affect follow position)
    Vec orient_tgt = base_tgt;
    orient_tgt.x += g_look_aim_offset_x;
    orient_tgt.y += g_look_aim_offset_y;

    // Look-At Lock (after inputs/orbit/follow)
    if (g_look_enabled) {
        apply_look_at_lock(cam, orient_tgt, dt);
    }

    // Guides + Shot HUD
    draw_guides_and_hud(cam);
}

void FreeCam::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered())
        FreeCam::hoveredDescription = defaultDescription;

    if (ImGui::Checkbox("Free Cam", &mod_enabled)) {
        toggle(mod_enabled);
        if (!mod_enabled) {
            toggle_pause_enabled = false;
            togglePause(toggle_pause_enabled);
        }
    }
    if (ImGui::IsItemHovered())
        FreeCam::hoveredDescription = defaultDescription;

    ImGui::Text("Sensitivity");
    ImGui::SliderFloat("##SensitivitySliderFloat", &sens, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered())
        FreeCam::hoveredDescription = "Camera movement sensitivity";

    ImGui::Text("Modifier Sensitivity");
    ImGui::SliderFloat("##ModifierSensitivitySliderFloat", &modifierSens, 0.0f, 10.0f, "%.1f");
    if (ImGui::IsItemHovered())
        FreeCam::hoveredDescription = "Camera movement sensitivity while holding L3";

    ImGui::Text("Stick Deadzones");
    ImGui::SliderFloat("##DeadzoneSliderFloat", &deadZone, 0.0f, 0.2f, "%.2f");
    if (ImGui::IsItemHovered())
        FreeCam::hoveredDescription = "How far the stick has to move from centre before your input is registered";

    if (ImGui::Checkbox("Pause", &toggle_pause_enabled)) {
        togglePause(toggle_pause_enabled);
    }
    if (ImGui::IsItemHovered())
        FreeCam::hoveredDescription = "Pause the player and enemies";

    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        ImGui::Checkbox("Enable Inputs To Player", &player->mOperate);
        if (ImGui::IsItemHovered())
            FreeCam::hoveredDescription = "Disable player movement/actions";
    }

    // Look-At Lock
    if (ImGui::CollapsingHeader("Look-At Lock")) {
        ImGui::Checkbox("Enable Look-At Lock", &g_look_enabled);
        ImGui::BeginDisabled(!g_look_enabled);
        ImGui::Checkbox("Target Player", &g_look_target_player);
        if (!g_look_target_player) {
            ImGui::Text("Custom Target (world)");
            ImGui::InputFloat3("##custom_target", (float*)&g_look_custom);
            if (player && ImGui::Button("Set Custom To Player")) {
                g_look_custom.x = player->mCharaStatus.pos.x;
                g_look_custom.y = player->mCharaStatus.pos.y;
                g_look_custom.z = player->mCharaStatus.pos.z;
            }
            ImGui::SameLine();
            if (ImGui::Button("Set Custom To Camera")) {
                HrCamera* cam = nmh_sdk::get_HrCamera();
                if (cam) {
                    g_look_custom = cam->MAIN.free.C_T_Pos;
                }
            }
        }
        ImGui::SliderFloat("Strength", &g_look_strength, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Responsiveness (Hz)", &g_look_responsiveness, 1.0f, 30.0f, "%.1f");
        ImGui::SliderFloat("Max Pitch (rad)", &g_look_max_pitch, 0.2f, 1.5f, "%.2f");

        // Aim offset sliders (X/Y) This is so you can orbit something other than Travis's Velcro light-up Skechers
        ImGui::Separator();
        ImGui::SliderFloat("Aim Offset X (world)", &g_look_aim_offset_x, -50.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Aim Offset Y (world)", &g_look_aim_offset_y, -50.0f, 50.0f, "%.2f");

        // Follow options
        ImGui::Separator();
        ImGui::Checkbox("Keep Up With Target (Follow Position)", &g_look_follow_enabled);
        ImGui::BeginDisabled(!g_look_follow_enabled);
        ImGui::SliderFloat("Follow Damping (Hz)", &g_look_follow_damp_hz, 1.0f, 30.0f, "%.1f");
        ImGui::InputFloat3("Follow Offset (world)", (float*)&g_look_follow_offset);
        if (ImGui::Button("Capture Offset From Current")) {
            HrCamera* cam = nmh_sdk::get_HrCamera();
            if (cam) {
                Vec tgt = g_look_custom;
                if (g_look_target_player && player) {
                    tgt.x = player->mCharaStatus.pos.x;
                    tgt.y = player->mCharaStatus.pos.y;
                    tgt.z = player->mCharaStatus.pos.z;
                }
                g_look_follow_offset.x    = cam->MAIN.free.C_T_Pos.x - tgt.x;
                g_look_follow_offset.y    = cam->MAIN.free.C_T_Pos.y - tgt.y;
                g_look_follow_offset.z    = cam->MAIN.free.C_T_Pos.z - tgt.z;
                g_look_follow_have_offset = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Offset = Behind/Above Default")) {
            // simple default: 6 up, 12 back in world -Z
            g_look_follow_offset      = {0.0f, 6.0f, -12.0f};
            g_look_follow_have_offset = true;
        }
        ImGui::EndDisabled();

        if (ImGui::Button("Snap Now")) {
            HrCamera* cam = nmh_sdk::get_HrCamera();
            if (cam) {
                Vec tgt = g_look_custom;
                if (g_look_target_player && player) {
                    tgt.x = player->mCharaStatus.pos.x;
                    tgt.y = player->mCharaStatus.pos.y;
                    tgt.z = player->mCharaStatus.pos.z;
                }
                // apply aim offsets for the snap as well
                tgt.x += g_look_aim_offset_x;
                tgt.y += g_look_aim_offset_y;

                float old_resp        = g_look_responsiveness;
                float old_str         = g_look_strength;
                g_look_responsiveness = 1000.0f;
                g_look_strength       = 1.0f;
                apply_look_at_lock(cam, tgt, 1.0f / 60.0f);
                g_look_responsiveness = old_resp;
                g_look_strength       = old_str;
            }
        }
        ImGui::EndDisabled();
    }

    // Turntable Orbit
    if (ImGui::CollapsingHeader("Turntable Orbit")) {
        ImGui::Checkbox("Enable Orbit", &g_orbit_enabled);
        ImGui::BeginDisabled(!g_orbit_enabled);
        ImGui::Checkbox("Follow Player As Center", &g_orbit_follow_player);
        ImGui::Checkbox("Face Center While Orbiting", &g_orbit_face_center);
        if (!g_orbit_follow_player) {
            ImGui::Text("Orbit Center (world)");
            ImGui::InputFloat3("##orbit_center", (float*)&g_orbit_center);
        }
        if (ImGui::Button("Center = Player") && player) {
            g_orbit_center.x = player->mCharaStatus.pos.x;
            g_orbit_center.y = player->mCharaStatus.pos.y;
            g_orbit_center.z = player->mCharaStatus.pos.z;
        }
        ImGui::SameLine();
        if (ImGui::Button("Center = Camera")) {
            HrCamera* cam = nmh_sdk::get_HrCamera();
            if (cam) {
                g_orbit_center = cam->MAIN.free.C_T_Pos;
            }
        }
        ImGui::SliderFloat("Radius", &g_orbit_radius, 1.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Height Offset", &g_orbit_height_offset, -20.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Center Offset X", &g_orbit_center_off_x, -50.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Center Offset Y", &g_orbit_center_off_y, -50.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Speed (deg/s)", &g_orbit_speed_deg, -360.0f, 360.0f, "%.0f");
        ImGui::SliderFloat("Angle (deg)", &g_orbit_angle_deg, 0.0f, 360.0f, "%.0f");
        ImGui::EndDisabled();
    }

    // Guides and HUD
    if (ImGui::CollapsingHeader("Guides & Shot HUD")) {
        ImGui::Checkbox("Show Guides", &g_guides_enabled);
        ImGui::BeginDisabled(!g_guides_enabled);
        ImGui::Checkbox("Rule of Thirds", &g_guide_thirds);
        ImGui::Checkbox("Center Cross", &g_guide_cross);
        ImGui::Checkbox("Golden Ratio Grid", &g_guide_golden);
        ImGui::Checkbox("Safe Frame", &g_guide_safe);
        ImGui::Checkbox("Horizon Line", &g_guide_horizon);
        ImGui::SliderFloat("Guides Opacity", &g_guide_opacity, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Safe Aspect", &g_safe_aspect, 1.00f, 3.00f, "%.2f");
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Checkbox("Shot HUD", &g_hud_enabled);
    }
}

// during load
void FreeCam::on_config_load(const utility::Config& cfg) {
    sens         = cfg.get<float>("freecam_sens").value_or(1.0f);
    modifierSens = cfg.get<float>("modifier_freecam_sens").value_or(2.0f);
    deadZone     = cfg.get<float>("freecam_deadzone").value_or(0.0f);

    g_look_enabled        = cfg.get<bool>("freecam_look_enabled").value_or(false);
    g_look_target_player  = cfg.get<bool>("freecam_look_target_player").value_or(true);
    g_look_strength       = cfg.get<float>("freecam_look_strength").value_or(0.35f);
    g_look_responsiveness = cfg.get<float>("freecam_look_resp").value_or(10.0f);
    g_look_max_pitch      = cfg.get<float>("freecam_look_max_pitch").value_or(1.50f);

    // aim offsets
    g_look_aim_offset_x = cfg.get<float>("freecam_look_aim_off_x").value_or(0.0f);
    g_look_aim_offset_y = cfg.get<float>("freecam_look_aim_off_y").value_or(0.0f);

    // follow settings
    g_look_follow_enabled     = cfg.get<bool>("freecam_look_follow_enabled").value_or(false);
    g_look_follow_offset.x    = cfg.get<float>("freecam_look_follow_off_x").value_or(0.0f);
    g_look_follow_offset.y    = cfg.get<float>("freecam_look_follow_off_y").value_or(6.0f);
    g_look_follow_offset.z    = cfg.get<float>("freecam_look_follow_off_z").value_or(-12.0f);
    g_look_follow_damp_hz     = cfg.get<float>("freecam_look_follow_damp_hz").value_or(8.0f);
    g_look_follow_have_offset = false; // recapture on next enable

    g_orbit_enabled       = cfg.get<bool>("freecam_orbit_enabled").value_or(false);
    g_orbit_follow_player = cfg.get<bool>("freecam_orbit_follow_player").value_or(true);
    g_orbit_face_center   = cfg.get<bool>("freecam_orbit_face_center").value_or(true);
    g_orbit_radius        = cfg.get<float>("freecam_orbit_radius").value_or(12.0f);
    g_orbit_height_offset = cfg.get<float>("freecam_orbit_height").value_or(2.0f);
    g_orbit_speed_deg     = cfg.get<float>("freecam_orbit_speed_deg").value_or(35.0f);
    g_orbit_angle_deg     = cfg.get<float>("freecam_orbit_angle_deg").value_or(0.0f);

    // orbit center offsets
    g_orbit_center_off_x = cfg.get<float>("freecam_orbit_center_off_x").value_or(0.0f);
    g_orbit_center_off_y = cfg.get<float>("freecam_orbit_center_off_y").value_or(0.0f);

    g_guides_enabled = cfg.get<bool>("freecam_guides_enabled").value_or(false);
    g_guide_thirds   = cfg.get<bool>("freecam_guide_thirds").value_or(true);
    g_guide_cross    = cfg.get<bool>("freecam_guide_cross").value_or(true);
    g_guide_golden   = cfg.get<bool>("freecam_guide_golden").value_or(false);
    g_guide_safe     = cfg.get<bool>("freecam_guide_safe").value_or(false);
    g_guide_horizon  = cfg.get<bool>("freecam_guide_horizon").value_or(false);
    g_guide_opacity  = cfg.get<float>("freecam_guide_opacity").value_or(0.55f);
    g_safe_aspect    = cfg.get<float>("freecam_safe_aspect").value_or(2.35f);

    g_hud_enabled = cfg.get<bool>("freecam_hud_enabled").value_or(true);
}

// during save
void FreeCam::on_config_save(utility::Config& cfg) {
    cfg.set<float>("freecam_sens", sens);
    cfg.set<float>("modifier_freecam_sens", modifierSens);
    cfg.set<float>("freecam_deadzone", deadZone);

    cfg.set<bool>("freecam_look_enabled", g_look_enabled);
    cfg.set<bool>("freecam_look_target_player", g_look_target_player);
    cfg.set<float>("freecam_look_strength", g_look_strength);
    cfg.set<float>("freecam_look_resp", g_look_responsiveness);
    cfg.set<float>("freecam_look_max_pitch", g_look_max_pitch);

    // aim offsets
    cfg.set<float>("freecam_look_aim_off_x", g_look_aim_offset_x);
    cfg.set<float>("freecam_look_aim_off_y", g_look_aim_offset_y);

    // follow settings
    cfg.set<bool>("freecam_look_follow_enabled", g_look_follow_enabled);
    cfg.set<float>("freecam_look_follow_off_x", g_look_follow_offset.x);
    cfg.set<float>("freecam_look_follow_off_y", g_look_follow_offset.y);
    cfg.set<float>("freecam_look_follow_off_z", g_look_follow_offset.z);
    cfg.set<float>("freecam_look_follow_damp_hz", g_look_follow_damp_hz);

    cfg.set<bool>("freecam_orbit_enabled", g_orbit_enabled);
    cfg.set<bool>("freecam_orbit_follow_player", g_orbit_follow_player);
    cfg.set<bool>("freecam_orbit_face_center", g_orbit_face_center);
    cfg.set<float>("freecam_orbit_radius", g_orbit_radius);
    cfg.set<float>("freecam_orbit_height", g_orbit_height_offset);
    cfg.set<float>("freecam_orbit_speed_deg", g_orbit_speed_deg);
    cfg.set<float>("freecam_orbit_angle_deg", g_orbit_angle_deg);

    // orbit center offsets
    cfg.set<float>("freecam_orbit_center_off_x", g_orbit_center_off_x);
    cfg.set<float>("freecam_orbit_center_off_y", g_orbit_center_off_y);

    cfg.set<bool>("freecam_guides_enabled", g_guides_enabled);
    cfg.set<bool>("freecam_guide_thirds", g_guide_thirds);
    cfg.set<bool>("freecam_guide_cross", g_guide_cross);
    cfg.set<bool>("freecam_guide_golden", g_guide_golden);
    cfg.set<bool>("freecam_guide_safe", g_guide_safe);
    cfg.set<bool>("freecam_guide_horizon", g_guide_horizon);
    cfg.set<float>("freecam_guide_opacity", g_guide_opacity);
    cfg.set<float>("freecam_safe_aspect", g_safe_aspect);

    cfg.set<bool>("freecam_hud_enabled", g_hud_enabled);
}

// void FreeCam::on_draw_debug_ui() {}
#endif
