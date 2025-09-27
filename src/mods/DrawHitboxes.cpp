#include "DrawHitboxes.hpp"
#include "WorldVisualizer.hpp"
#if 1
bool DrawHitboxes::imguiPopout = true;

std::optional<std::string> DrawHitboxes::on_initialize() {
    return Mod::on_initialize();
}

void RenderWorldVisualization() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("WorldViz", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
    WorldVisualizer::SetDrawList(ImGui::GetWindowDrawList());

    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        Vec playerPos     = player->mCharaStatus.pos;
        Vec targetPos     = player->mLockOnPos;
        Vec swordPos      = player->mCharaStatus.hitColl.mMoveShape.mRectangle.mCenter; // this was a guess and it seems I was nowhere near correct
        float swordRadius = player->mCharaStatus.hitColl.mMoveShape.mRadius; // this is probably the same

        // Draw a sphere at player position
        WorldVisualizer::DrawWorldSphere(playerPos, 10.0f, IM_COL32(0, 255, 0, 255));

        // sword pos
        WorldVisualizer::DrawWorldSphere(swordPos, swordRadius, IM_COL32(0, 255, 0, 255));

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
    ImGui::End();
}

void DrawHitboxes::Stuff() {
    RenderWorldVisualization();
}

void DrawHitboxes::on_draw_ui() {
    ImGui::Checkbox("Draw Hitboxe", &imguiPopout);
}

// void DrawHitboxes::custom_imgui_window() {}

// during load
// void DrawHitboxes::on_config_load(const utility::Config &cfg) {}
// during save
// void DrawHitboxes::on_config_save(utility::Config &cfg) {}
// do something every frame
void DrawHitboxes::on_frame() {
    if (imguiPopout) {
        DrawHitboxes::Stuff();
    }
}
// will show up in debug window, dump ImGui widgets you want here
// void DrawHitboxes::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
