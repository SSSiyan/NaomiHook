#include "DrawHitboxes.hpp"
#include "WorldVisualizer.hpp"
#if 1
bool drawHitboxes = false;
bool drawBullshit              = false;
bool drawWeaponAttempt         = false;
uintptr_t gPcCommonTable       = NULL;

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
        AttackData data = {};

        int atkNo = nmh_sdk::GetAtkNo(player, motionNo);

        data.range = *(float*)(gPcCommonTable + (44 * atkNo) + 16);

        // Extract angle data - this would need to be derived from checkInAngle parameters
        // The original code doesn't store angle directly, so we'll estimate based on motion types
        data.angle = GetMotionAngle(player, motionNo);

        // Check for special range extensions
        data.hasExtendedRange  = IsExtendedRangeMotion(motionNo);
        data.isOmnidirectional = IsOmnidirectionalMotion(motionNo);

        return data;
    }

    float GetMotionAngle(mHRPc* player, int motionNo) {
        int atkNo = nmh_sdk::GetAtkNo(player, motionNo);
        float* anglePtr        = (float*)(gPcCommonTable + (44 * atkNo) + 20);
        float halfAngleRadians = *anglePtr;
        return (halfAngleRadians * 2.0f) * (180.0f / M_PI);
    }

    bool IsExtendedRangeMotion(int motionNo) {
        // From the original code's extended range check
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
        // 360-degree attacks
        switch (motionNo) {
        case 169:
        case 170:
        case 246:
        case 290:
        case 291:
        case 292:
        case 340:
        case 386:
            return true;
        default:
            return false;
        }
    }

    void DrawCone(const Vec& center, float radius, float startAngle, float endAngle, uint32_t color, int segments = 16) {
        if (radius <= 0)
            return;

        std::vector<Vec> points;
        points.push_back(center); // Center point

        // Generate arc points
        float angleStep = (endAngle - startAngle) / segments;
        for (int i = 0; i <= segments; i++) {
            float angle = startAngle + (angleStep * i);
            Vec point;
            point.x = center.x + sin(angle) * radius;
            point.y = center.y;
            point.z = center.z + cos(angle) * radius;
            points.push_back(point);
        }

        // Draw the cone
        for (size_t i = 1; i < points.size() - 1; i++) {
            WorldVisualizer::DrawWorldLine(center, points[i], color);
            WorldVisualizer::DrawWorldLine(points[i], points[i + 1], color);
        }
        WorldVisualizer::DrawWorldLine(center, points.back(), color);
    }

public:
    void VisualizeAttackRange(mHRPc* player) {
        if (!player || !player->mOperate)
            return;

        Vec playerPos    = player->mCharaStatus.pos;
        int motionNo     = player->mCharaStatus.motionNo;
        float playerRotY = player->mCharaStatus.rot.y;

        // Skip non-attack motions
        if ((unsigned int)(motionNo - 167) <= 1)
            return;

        AttackData attackData = GetAttackData(player, motionNo);

        // Color coding based on attack type
        uint32_t rangeColor     = IM_COL32(255, 100, 100, 128); // Red for damage range
        uint32_t detectionColor = IM_COL32(255, 255, 100, 64);  // Yellow for detection range
        uint32_t activeColor    = IM_COL32(0, 255, 0, 128);     // Green for active frames

        // Check if attack is in active frames
        float currentTick = player->tagMain->Motion[0].MotionType3Anm[0].Main.PlayTick;

        int atkNo        = nmh_sdk::GetAtkNo(player, motionNo);
        float startFrame = *(float*)(gPcCommonTable + (44 * atkNo) + 4) * 100.0f;
        float endFrame   = *(float*)(gPcCommonTable + (44 * atkNo) + 12) * 100.0f;

        bool isActiveFrame    = (currentTick >= startFrame && currentTick < endFrame);
        uint32_t currentColor = isActiveFrame ? activeColor : rangeColor;

        // Draw based on attack pattern
        if (attackData.isOmnidirectional) {
            // 360-degree attacks
            WorldVisualizer::DrawWorldSphere(playerPos, attackData.range, currentColor);

            // Show different radius for different enemy types (from the 7.5x and 3.0x multipliers)
            WorldVisualizer::DrawWorldSphere(playerPos, attackData.range * 0.8f, IM_COL32(255, 150, 0, 64)); // Small enemies
        } else {
            // Directional cone attacks
            float halfAngle  = (attackData.angle * 0.5f) * (M_PI / 180.0f);
            float startAngle = playerRotY - halfAngle;
            float endAngle   = playerRotY + halfAngle;

            DrawCone(playerPos, attackData.range, startAngle, endAngle, currentColor);

            // Extended range for certain attacks
            if (attackData.hasExtendedRange) {
                DrawCone(playerPos, attackData.range * 1.5f, startAngle, endAngle, IM_COL32(100, 100, 255, 64));
            }
        }

        // Draw grab range for throw attacks
        if (motionNo == 172) {
            WorldVisualizer::DrawWorldSphere(playerPos, 28.0f, IM_COL32(255, 0, 255, 128));
        }

        // Draw line-of-sight check for certain character types
        if (ShouldCheckLineOfSight(motionNo)) {
            // This would draw lines to potential targets to show LOS blocking
            // DrawLineOfSightChecks(player, attackData.range);
        }

        // Display motion info
        DrawMotionInfo(player, motionNo, attackData, isActiveFrame);
    }

private:
    bool ShouldCheckLineOfSight(int motionNo) {
        // Based on the character type checks in the original code
        return (motionNo >= 248 && motionNo <= 388) || (motionNo >= 193 && motionNo <= 198);
    }

    void DrawLineOfSightChecks(mHRPc* player, float range) {
        // Iterate through potential targets and draw LOS lines
        mHRChara* target = (mHRChara*)nmh_sdk::get_mHRBattle()->mpNpc[0]->mCharaAnchor.mpNext;
        Vec playerPos    = player->mCharaStatus.pos;

        while (target) {
            // Skip invalid targets (same logic as original)
            int charaType    = target->mStatus.charaType;
            bool validTarget = !(charaType == 500 || charaType == 520 || charaType == 524 || charaType == 63 || charaType == 65);

            if (validTarget) {
                Vec targetPos  = target->mStatus.pos;
                float distance = sqrtf(pow(targetPos.x - playerPos.x, 2) + pow(targetPos.z - playerPos.z, 2));

                if (distance <= range) {
                    uint32_t losColor = IM_COL32(0, 255, 255, 128); // Cyan for LOS
                    WorldVisualizer::DrawWorldLine(playerPos, targetPos, losColor);
                }
            }

            target = (mHRChara*)target->mCharaAnchor.mpNext;
        }
    }

    void DrawMotionInfo(mHRPc* player, int motionNo, const AttackData& data, bool isActive) {
        // Display attack info on screen
        ImGui::Begin("Attack Info");
        ImGui::Text("Motion: %d", motionNo);
        ImGui::Text("Range: %.1f", data.range);
        ImGui::Text("Angle: %.1f degrees", data.angle);
        ImGui::Text("Active Frame: %s", isActive ? "YES" : "NO");
        ImGui::Text("Extended Range: %s", data.hasExtendedRange ? "YES" : "NO");
        ImGui::Text("Omnidirectional: %s", data.isOmnidirectional ? "YES" : "NO");
        float currentTick = player->tagMain->Motion[0].MotionType3Anm[0].Main.PlayTick;

        // this is how the game does it:
        // float currentTick = player->mResource.pGmf->dat.Motion[0].PlayMotionTick;
        ImGui::Text("Current Tick: %.1f", currentTick);
        ImGui::End();
    }
};

// Usage example:
void UpdateAttackVisualization(mHRPc* player) {
    static AttackRangeVisualizer visualizer;
    visualizer.VisualizeAttackRange(player);
}


void DrawHitboxes::Stuff() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("WorldViz", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
    WorldVisualizer::SetDrawList(ImGui::GetWindowDrawList());

    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        if (drawBullshit) {
            Vec playerPos = player->mCharaStatus.pos;
            Vec targetPos = player->mLockOnPos;
            player->mCharaStatus.motionNo;

            // Draw a sphere at player position
            WorldVisualizer::DrawWorldSphere(playerPos, 28.0f, IM_COL32(0, 255, 0, 255));

            // Draw a line to target
            WorldVisualizer::DrawWorldLine(playerPos, targetPos, IM_COL32(255, 0, 0, 255), 2.0f);

            // Draw a box around target
            Vec boxMin = {targetPos.x - 5, targetPos.y - 5, targetPos.z - 5};
            Vec boxMax = {targetPos.x + 5, targetPos.y + 5, targetPos.z + 5};
            WorldVisualizer::DrawWorldBox(boxMin, boxMax, IM_COL32(255, 255, 0, 255));

            // Draw text above target
            Vec textPos = {targetPos.x, targetPos.y + 20, targetPos.z};
            WorldVisualizer::DrawWorldText(textPos, "Target");

            // Draw a grid
            Vec gridCenter = {0, 0, 0};
            WorldVisualizer::DrawWorldGrid(gridCenter, 200.0f, 10, IM_COL32(64, 64, 64, 255));
        }
        if (drawWeaponAttempt) {
            static AttackRangeVisualizer visualizer;
            visualizer.VisualizeAttackRange(player);
            #if 0
            // Get the three collision points (after game's interpolation processing)
            Vec processedTip  = player->mWepColl.mPoint[0]; // Interpolated current position
            Vec processedPrev = player->mWepColl.mPoint[1]; // Interpolated previous position
            Vec baseRef       = player->mWepColl.mPoint[2]; // Base reference point

            // Draw the collision triangle with role-coded colors
            WorldVisualizer::DrawWorldTriangle(processedTip, processedPrev, baseRef, IM_COL32(255, 255, 0, 255), 2.0f);

            // Draw vertices with different colors to show their roles
            WorldVisualizer::DrawWorldPoint(processedTip, 5.0f, IM_COL32(255, 0, 0, 255));    // Red - Current tip
            WorldVisualizer::DrawWorldPoint(processedPrev, 5.0f, IM_COL32(255, 128, 0, 255)); // Orange - Previous tip
            WorldVisualizer::DrawWorldPoint(baseRef, 5.0f, IM_COL32(0, 255, 0, 255));         // Green - Base reference

            // Draw motion trail from previous to current
            WorldVisualizer::DrawWorldLine(processedPrev, processedTip, IM_COL32(255, 0, 255, 255), 3.0f);

            // Draw semi-transparent filled triangle to show collision area
            WorldVisualizer::DrawWorldFilledTriangle(processedTip, processedPrev, baseRef, IM_COL32(255, 255, 0, 64));

            // Draw labels
            WorldVisualizer::DrawWorldText(processedTip, "mPoint[0]", IM_COL32(255, 255, 255, 255));
            WorldVisualizer::DrawWorldText(processedPrev, "mPoint[1]", IM_COL32(255, 255, 255, 255));
            WorldVisualizer::DrawWorldText(baseRef, "mPoint[2]", IM_COL32(255, 255, 255, 255));
            #endif
        }
    }
    ImGui::End();
}

void DrawHitboxes::on_draw_ui() {
    ImGui::Checkbox("Draw Hitboxes", &drawHitboxes);
    ImGui::Checkbox("Draw Weapon Attempt", &drawWeaponAttempt);
    ImGui::Checkbox("Draw Bullshit", &drawBullshit);
}

// void DrawHitboxes::custom_imgui_window() {}

// during load
// void DrawHitboxes::on_config_load(const utility::Config &cfg) {}
// during save
// void DrawHitboxes::on_config_save(utility::Config &cfg) {}
// do something every frame
void DrawHitboxes::on_frame() {
    if (drawHitboxes) {
        DrawHitboxes::Stuff();
    }
}
// will show up in debug window, dump ImGui widgets you want here
// void DrawHitboxes::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
