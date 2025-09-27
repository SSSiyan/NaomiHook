#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class WorldVisualizer : public Mod {
public:
    WorldVisualizer() = default;
    ModCategory get_category() { return ModCategory::SYSTEM; };
    std::string get_mod_name() const override { return "WorldVisualizer"; }
    std::string get_human_readable_name() const { return "WorldVisualizer"; }
    const char* get_description() const override { return R"(WorldVisualizer)"; };
    std::optional<std::string> on_initialize() override;

    // Set the draw list to use for all subsequent draw calls
    static void SetDrawList(ImDrawList* list = nullptr);

    // Get current draw list (creates default if none set)
    static ImDrawList* GetDrawList();

    // Draw a line between two world positions
    static void DrawWorldLine(const Vec& start, const Vec& end, ImU32 color = IM_COL32(255, 255, 255, 255), float thickness = 1.0f);

    // Draw a wireframe sphere
    static void DrawWorldSphere(
        const Vec& center, float radius, ImU32 color = IM_COL32(255, 255, 255, 255), int segments = 32, float thickness = 1.0f);

    // Draw a filled sphere (using screen-space circle)
    static void DrawWorldFilledSphere(const Vec& center, float radius, ImU32 color = IM_COL32(255, 255, 255, 128));

    // Draw a wireframe box
    static void DrawWorldBox(const Vec& min, const Vec& max, ImU32 color = IM_COL32(255, 255, 255, 255), float thickness = 1.0f);

    // Draw a world-space point as a small circle
    static void DrawWorldPoint(const Vec& pos, float size = 3.0f, ImU32 color = IM_COL32(255, 0, 0, 255));

    // Draw text at a world position
    static void DrawWorldText(const Vec& pos, const char* text, ImU32 color = IM_COL32(255, 255, 255, 255));

    // Draw an arrow from start to end
    static void DrawWorldArrow(
        const Vec& start, const Vec& end, ImU32 color = IM_COL32(255, 255, 255, 255), float thickness = 2.0f, float arrowSize = 10.0f);

    // Draw a polyline (connected line segments)
    static void DrawWorldPolyline(
        const std::vector<Vec>& points, ImU32 color = IM_COL32(255, 255, 255, 255), float thickness = 1.0f, bool closed = false);

    // Draw a grid in world space
    static void DrawWorldGrid(
        const Vec& center, float size, int divisions = 10, ImU32 color = IM_COL32(128, 128, 128, 255), float thickness = 1.0f);

private:
    static ImDrawList* currentDrawList;

    // Helper function to check if a screen position is valid and on screen
    static bool IsValidScreenPos(const Vec& screenPos);
    // Helper to convert world position to ImVec2 screen coordinates
    static bool WorldToScreen(const Vec& worldPos, ImVec2& screenPos);
};