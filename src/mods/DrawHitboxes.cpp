// ASCII-ONLY
#include "DrawHitboxes.hpp"
#include "WorldVisualizer.hpp"
#if 1
bool drawHitboxes        = false;
bool drawBullshit        = false;
bool drawWeaponAttempt   = false;
uintptr_t gPcCommonTable = NULL;

std::optional<std::string> DrawHitboxes::on_initialize() {
    gPcCommonTable = g_framework->get_module().as<uintptr_t>() + 0x7421E0;
    return Mod::on_initialize();
}

class AttackRangeVisualizer {
private:
    struct AttackData {
        float range;
        float angle;
        bool isOmnidirectional;
        bool hasExtendedRange;
    };

    AttackData GetAttackData(mHRPc* player, int motionNo) {
        AttackData data        = {};
        int atkNo              = nmh_sdk::GetAtkNo(player, motionNo);
        data.range             = *(float*)(gPcCommonTable + (44 * atkNo) + 16);
        data.angle             = GetMotionAngle(player, motionNo);
        data.hasExtendedRange  = IsExtendedRangeMotion(motionNo);
        data.isOmnidirectional = IsOmnidirectionalMotion(motionNo);
        return data;
    }

    float GetMotionAngle(mHRPc* player, int motionNo) {
        int atkNo       = nmh_sdk::GetAtkNo(player, motionNo);
        float* anglePtr = (float*)(gPcCommonTable + (44 * atkNo) + 20);
        float fullAngle = *anglePtr; // radians, full cone width
        if (fullAngle < 0.05f)
            fullAngle = 0.05f;
        if (fullAngle > 3.14f)
            fullAngle = 3.14f;
        return fullAngle;
    }

    bool IsExtendedRangeMotion(int motionNo) {
        switch (motionNo) {
        case 0xC1:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xF7:
        case 0x125:
        case 0x155:
        case 0x183:
            return true;
        default:
            return false;
        }
    }

    bool IsOmnidirectionalMotion(int motionNo) {
        switch (motionNo) {
            return true;
        default:
            return false;
        }
    }

    bool IsDualHemisphereAttack(int motionNo) {
        // attacks that check both front and back hemispheres
        switch (motionNo) {
        case 278: // ePcMtBtAtk04CmbUpFnsh
            return true;
        default:
            return false;
        }
    }

    void DrawCone(const Vec& center, float radius, float startAngle, float endAngle, uint32_t color, int segments = 24) {
        if (radius <= 0)
            return;
        std::vector<Vec> pts;
        pts.push_back(center);
        float step = (endAngle - startAngle) / (float)segments;
        for (int i = 0; i <= segments; ++i) {
            float a = startAngle + step * i;
            Vec p;
            p.x = center.x + sinf(a) * radius;
            p.y = center.y;
            p.z = center.z + cosf(a) * radius;
            pts.push_back(p);
        }
        for (size_t i = 1; i < pts.size() - 1; ++i) {
            WorldVisualizer::DrawWorldLine(center, pts[i], color);
            WorldVisualizer::DrawWorldLine(pts[i], pts[i + 1], color);
        }
        WorldVisualizer::DrawWorldLine(center, pts.back(), color);
    }

public:
    void VisualizeAttackRange(mHRPc* player) {
        if (!player || !player->mOperate)
            return;

        Vec pos      = player->mCharaStatus.pos;
        int motionNo = player->mCharaStatus.motionNo;
        float rotY   = player->mCharaStatus.rot.y;

        if ((unsigned int)(motionNo - 167) <= 1)
            return;

        AttackData atk = GetAttackData(player, motionNo);

        uint32_t colorBase   = IM_COL32(255, 100, 100, 128);
        uint32_t colorActive = IM_COL32(0, 255, 0, 128);

        float tick         = player->tagMain->Motion[0].MotionType3Anm[0].Main.PlayTick;
        int atkNo          = nmh_sdk::GetAtkNo(player, motionNo);
        float startFrame   = *(float*)(gPcCommonTable + (44 * atkNo) + 4) * 100.0f;
        float endFrame     = *(float*)(gPcCommonTable + (44 * atkNo) + 12) * 100.0f;
        bool active        = (tick >= startFrame && tick < endFrame);
        uint32_t drawColor = active ? colorActive : colorBase;

        if (atk.isOmnidirectional) {
            WorldVisualizer::DrawWorldSphere(pos, atk.range, drawColor);
            WorldVisualizer::DrawWorldSphere(pos, atk.range * 0.8f, IM_COL32(255, 150, 0, 64));
        } else {
            float half  = atk.angle * 0.5f;
            float start = rotY - half;
            float end   = rotY + half;

            // draw main forward cone
            DrawCone(pos, atk.range, start, end, drawColor);

            // if dual hemisphere, draw mirrored rear cone
            if (IsDualHemisphereAttack(motionNo)) {
                float rearStart = rotY + M_PI - half;
                float rearEnd   = rotY + M_PI + half;
                DrawCone(pos, atk.range, rearStart, rearEnd, drawColor);
            }

            // extended range overlay
            if (atk.hasExtendedRange) {
                DrawCone(pos, atk.range * 1.5f, start, end, IM_COL32(100, 100, 255, 64));
                if (IsDualHemisphereAttack(motionNo)) {
                    float rearStart = rotY + M_PI - half;
                    float rearEnd   = rotY + M_PI + half;
                    DrawCone(pos, atk.range * 1.5f, rearStart, rearEnd, IM_COL32(100, 100, 255, 64));
                }
            }
        }

        if (motionNo == 172)
            WorldVisualizer::DrawWorldSphere(pos, 28.0f, IM_COL32(255, 0, 255, 128));

        DrawMotionInfo(player, motionNo, atk, active);
    }

private:
    void DrawMotionInfo(mHRPc* player, int motionNo, const AttackData& d, bool active) {
        ImGui::Begin("Attack Info");
        ImGui::Text("Motion: %d", motionNo);
        ImGui::Text("Range: %.1f", d.range);
        ImGui::Text("Angle (rad): %.3f", d.angle);
        ImGui::Text("Angle (deg): %.1f", d.angle * (180.0f / M_PI));
        ImGui::Text("Active: %s", active ? "YES" : "NO");
        ImGui::Text("Extended: %s", d.hasExtendedRange ? "YES" : "NO");
        ImGui::Text("Omnidirectional: %s", d.isOmnidirectional ? "YES" : "NO");
        float tick = player->tagMain->Motion[0].MotionType3Anm[0].Main.PlayTick;
        ImGui::Text("Current Tick: %.1f", tick);
        ImGui::End();
    }
};

void UpdateAttackVisualization(mHRPc* player) {
    static AttackRangeVisualizer vis;
    vis.VisualizeAttackRange(player);
}

void DrawHitboxes::Stuff() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("WorldViz", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
    WorldVisualizer::SetDrawList(ImGui::GetWindowDrawList());

    mHRPc* pc = nmh_sdk::get_mHRPc();
    if (pc) {
        if (drawBullshit) {
            Vec pos = pc->mCharaStatus.pos;
            Vec tgt = pc->mLockOnPos;
            WorldVisualizer::DrawWorldSphere(pos, 28.0f, IM_COL32(0, 255, 0, 255));
            WorldVisualizer::DrawWorldLine(pos, tgt, IM_COL32(255, 0, 0, 255), 2.0f);
            Vec bmin = {tgt.x - 5, tgt.y - 5, tgt.z - 5};
            Vec bmax = {tgt.x + 5, tgt.y + 5, tgt.z + 5};
            WorldVisualizer::DrawWorldBox(bmin, bmax, IM_COL32(255, 255, 0, 255));
            Vec txt = {tgt.x, tgt.y + 20, tgt.z};
            WorldVisualizer::DrawWorldText(txt, "Target");
            Vec grid = {0, 0, 0};
            WorldVisualizer::DrawWorldGrid(grid, 200.0f, 10, IM_COL32(64, 64, 64, 255));
        }
        if (drawWeaponAttempt) {
            static AttackRangeVisualizer vis;
            vis.VisualizeAttackRange(pc);
        }
    }
    ImGui::End();
}

void DrawHitboxes::on_draw_ui() {
    ImGui::Checkbox("Draw Hitboxes", &drawHitboxes);
    ImGui::Checkbox("Draw Weapon Attempt", &drawWeaponAttempt);
    ImGui::Checkbox("Draw Bullshit", &drawBullshit);
}

void DrawHitboxes::on_frame() {
    if (drawHitboxes)
        DrawHitboxes::Stuff();
}
#endif
