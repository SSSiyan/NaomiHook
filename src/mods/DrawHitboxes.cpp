#include "DrawHitboxes.hpp"
#include "WorldVisualizer.hpp"
#if 1
bool drawHitboxes = true;
bool drawBullshit              = false;
bool drawWeaponAttempt         = true;

std::optional<std::string> DrawHitboxes::on_initialize() {
    return Mod::on_initialize();
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

            // Draw a sphere at player position
            WorldVisualizer::DrawWorldSphere(playerPos, 10.0f, IM_COL32(0, 255, 0, 255));

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
