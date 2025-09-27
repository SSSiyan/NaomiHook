#include "WorldVisualizer.hpp"

std::optional<std::string> WorldVisualizer::on_initialize() {
    return Mod::on_initialize();
}

ImDrawList* WorldVisualizer::currentDrawList = nullptr;

bool WorldVisualizer::IsValidScreenPos(const Vec& screenPos) {
    return screenPos.x >= -100.0f && screenPos.y >= -100.0f && screenPos.z >= 0.0f && screenPos.z <= 1.0f;
}

bool WorldVisualizer::WorldToScreen(const Vec& worldPos, ImVec2& screenPos) {
    Vec result;
    if (nmh_sdk::GetScreenPos(const_cast<Vec*>(&worldPos), &result)) {
        if (IsValidScreenPos(result)) {
            // Scale from game's fixed resolution (854x480) to actual display resolution
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            float scaleX       = displaySize.x / 854.0f;
            float scaleY       = displaySize.y / 480.0f;

            screenPos = ImVec2(result.x * scaleX, result.y * scaleY);
            return true;
        }
    }
    return false;
}

void WorldVisualizer::SetDrawList(ImDrawList* list) {
    currentDrawList = list;
}

ImDrawList* WorldVisualizer::GetDrawList() {
    if (!currentDrawList) {
        currentDrawList = ImGui::GetBackgroundDrawList();
    }
    return currentDrawList;
}

void WorldVisualizer::DrawWorldLine(const Vec& start, const Vec& end, ImU32 color, float thickness) {
    ImVec2 screenStart, screenEnd;
    if (WorldToScreen(start, screenStart) && WorldToScreen(end, screenEnd)) {
        GetDrawList()->AddLine(screenStart, screenEnd, color, thickness);
    }
}

void WorldVisualizer::DrawWorldSphere(const Vec& center, float radius, ImU32 color, int segments, float thickness) {
    const float angleStep = 2.0f * M_PI / segments;

    // XY plane circle
    for (int i = 0; i < segments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vec p1, p2;
        p1.x = center.x + radius * cosf(angle1);
        p1.y = center.y + radius * sinf(angle1);
        p1.z = center.z;
        p2.x = center.x + radius * cosf(angle2);
        p2.y = center.y + radius * sinf(angle2);
        p2.z = center.z;

        DrawWorldLine(p1, p2, color, thickness);
    }

    // XZ plane circle
    for (int i = 0; i < segments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vec p1, p2;
        p1.x = center.x + radius * cosf(angle1);
        p1.y = center.y;
        p1.z = center.z + radius * sinf(angle1);
        p2.x = center.x + radius * cosf(angle2);
        p2.y = center.y;
        p2.z = center.z + radius * sinf(angle2);

        DrawWorldLine(p1, p2, color, thickness);
    }

    // YZ plane circle
    for (int i = 0; i < segments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vec p1, p2;
        p1.x = center.x;
        p1.y = center.y + radius * cosf(angle1);
        p1.z = center.z + radius * sinf(angle1);
        p2.x = center.x;
        p2.y = center.y + radius * cosf(angle2);
        p2.z = center.z + radius * sinf(angle2);

        DrawWorldLine(p1, p2, color, thickness);
    }
}

void WorldVisualizer::DrawWorldFilledSphere(const Vec& center, float radius, ImU32 color) {
    ImVec2 screenPos;
    if (WorldToScreen(center, screenPos)) {
        GetDrawList()->AddCircleFilled(screenPos, radius, color);
    }
}

void WorldVisualizer::DrawWorldBox(const Vec& min, const Vec& max, ImU32 color, float thickness) {
    // Define the 8 corners of the box
    Vec corners[8];
    corners[0] = Vec(min.x, min.y, min.z); // 0
    corners[1] = Vec(max.x, min.y, min.z); // 1
    corners[2] = Vec(max.x, max.y, min.z); // 2
    corners[3] = Vec(min.x, max.y, min.z); // 3
    corners[4] = Vec(min.x, min.y, max.z); // 4
    corners[5] = Vec(max.x, min.y, max.z); // 5
    corners[6] = Vec(max.x, max.y, max.z); // 6
    corners[7] = Vec(min.x, max.y, max.z); // 7
    
    // Draw bottom face
    DrawWorldLine(corners[0], corners[1], color, thickness);
    DrawWorldLine(corners[1], corners[2], color, thickness);
    DrawWorldLine(corners[2], corners[3], color, thickness);
    DrawWorldLine(corners[3], corners[0], color, thickness);
    
    // Draw top face
    DrawWorldLine(corners[4], corners[5], color, thickness);
    DrawWorldLine(corners[5], corners[6], color, thickness);
    DrawWorldLine(corners[6], corners[7], color, thickness);
    DrawWorldLine(corners[7], corners[4], color, thickness);
    
    // Draw vertical edges
    DrawWorldLine(corners[0], corners[4], color, thickness);
    DrawWorldLine(corners[1], corners[5], color, thickness);
    DrawWorldLine(corners[2], corners[6], color, thickness);
    DrawWorldLine(corners[3], corners[7], color, thickness);
}

void WorldVisualizer::DrawWorldPoint(const Vec& pos, float size, ImU32 color) {
    ImVec2 screenPos;
    if (WorldToScreen(pos, screenPos)) {
        GetDrawList()->AddCircleFilled(screenPos, size, color);
    }
}

void WorldVisualizer::DrawWorldText(const Vec& pos, const char* text, ImU32 color) {
    ImVec2 screenPos;
    if (WorldToScreen(pos, screenPos)) {
        GetDrawList()->AddText(screenPos, color, text);
    }
}

void WorldVisualizer::DrawWorldArrow(const Vec& start, const Vec& end, ImU32 color, float thickness, float arrowSize) {
    ImVec2 screenStart, screenEnd;
    if (WorldToScreen(start, screenStart) && WorldToScreen(end, screenEnd)) {
        // Draw main line
        GetDrawList()->AddLine(screenStart, screenEnd, color, thickness);

        // Calculate arrow head
        ImVec2 dir   = ImVec2(screenEnd.x - screenStart.x, screenEnd.y - screenStart.y);
        float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (length > 0) {
            dir.x /= length;
            dir.y /= length;

            // Arrow head points
            ImVec2 arrowP1 =
                ImVec2(screenEnd.x - arrowSize * (dir.x * 0.8f + dir.y * 0.6f), screenEnd.y - arrowSize * (dir.y * 0.8f - dir.x * 0.6f));
            ImVec2 arrowP2 =
                ImVec2(screenEnd.x - arrowSize * (dir.x * 0.8f - dir.y * 0.6f), screenEnd.y - arrowSize * (dir.y * 0.8f + dir.x * 0.6f));

            GetDrawList()->AddLine(screenEnd, arrowP1, color, thickness);
            GetDrawList()->AddLine(screenEnd, arrowP2, color, thickness);
        }
    }
}

void WorldVisualizer::DrawWorldPolyline(const std::vector<Vec>& points, ImU32 color, float thickness, bool closed) {
    if (points.size() < 2)
        return;

    for (size_t i = 0; i < points.size() - 1; ++i) {
        DrawWorldLine(points[i], points[i + 1], color, thickness);
    }

    if (closed && points.size() > 2) {
        DrawWorldLine(points.back(), points.front(), color, thickness);
    }
}

void WorldVisualizer::DrawWorldGrid(const Vec& center, float size, int divisions, ImU32 color, float thickness) {
    float step = size / divisions;
    float half = size * 0.5f;

    // Draw lines parallel to X axis
    for (int i = 0; i <= divisions; ++i) {
        float offset = -half + i * step;
        Vec start, end;
        start.x = center.x - half;
        start.y = center.y;
        start.z = center.z + offset;
        end.x   = center.x + half;
        end.y   = center.y;
        end.z   = center.z + offset;
        DrawWorldLine(start, end, color, thickness);
    }

    // Draw lines parallel to Z axis
    for (int i = 0; i <= divisions; ++i) {
        float offset = -half + i * step;
        Vec start, end;
        start.x = center.x + offset;
        start.y = center.y;
        start.z = center.z - half;
        end.x   = center.x + offset;
        end.y   = center.y;
        end.z   = center.z + half;
        DrawWorldLine(start, end, color, thickness);
    }
}