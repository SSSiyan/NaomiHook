#include "ImageViewer.hpp"
#include "ModFramework.hpp"
#include "imgui.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include <memory>

static std::unique_ptr<Texture2DD3D11> g_test_image{};
static bool g_init{false};

struct Frame {
    ImVec2 pos,size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto nomohero() {
        return Frame {
            ImVec2 { 0.0f, 0.0f },  
            ImVec2 { 2550.0f, 3258.0f },
            ImVec2 { 0.0f / 4096.0f, 0.0f / 4096.0f },
            ImVec2 { ( 0.0f + 2550.0f ) / 4096.0f, ( 0.0f + 3258.0f ) / 4096.0f }
        }; //nomohero.png
    };

    static constexpr auto meta_size() {
        return ImVec2{ 4096.0f, 4096.0f  };
    };
};


void imview_init() {
    g_test_image = std::make_unique<Texture2DD3D11>("nomohero.png", g_framework->d3d11()->get_device());
    assert(g_test_image->IsValid());
    g_init = g_test_image->IsValid();
}

void imview_draw() {

    static float zoom = 1.0f;
    static ImVec2 scrolling(0.0f, 0.0f);
    static ImVec2 pan_offset(0.0f, 0.0f);
    static bool opt_enable_context_menu = true;

    ImTextureID my_tex_id = (ImTextureID)g_test_image->GetTexture();
    ImVec2 image_size(2550, 3258);

    if (!g_init) {
        return;
    }

    ImGui::Begin("Image Viewer", nullptr);

    ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
    ImGui::Text("Mouse Left: N/A\nMouse Right: drag to pan");

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));

    ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImVec2 content_avail = ImGui::GetContentRegionAvail();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("##pan_area", content_avail, ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered();
    const bool is_active = ImGui::IsItemActive();

    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
    {
        const ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        pan_offset.x += mouse_delta.x;
        pan_offset.y += mouse_delta.y;
    }

    if (opt_enable_context_menu && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !is_active)
        ImGui::OpenPopup("context");

    if (ImGui::BeginPopup("context"))
    {
        if (ImGui::MenuItem("Reset view")) {
            pan_offset = ImVec2(0.0f, 0.0f);
            zoom = 1.0f;
        }
        if (ImGui::MenuItem("Center image")) {
            pan_offset.x = (content_avail.x - image_size.x * zoom) * 0.5f;
            pan_offset.y = (content_avail.y - image_size.y * zoom) * 0.5f;
        }
        ImGui::EndPopup();
    }

    if (is_hovered && ImGui::GetIO().MouseWheel != 0.0f)
    {
        float wheel = ImGui::GetIO().MouseWheel;
        float zoom_factor = 1.0f + wheel * 0.1f;

        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        ImVec2 mouse_pos_in_canvas = ImVec2(mouse_pos.x - canvas_p0.x, mouse_pos.y - canvas_p0.y);

        zoom *= zoom_factor;
        if (zoom < 0.1f) zoom = 0.1f;

        pan_offset.x = mouse_pos_in_canvas.x - (mouse_pos_in_canvas.x - pan_offset.x) * zoom_factor;
        pan_offset.y = mouse_pos_in_canvas.y - (mouse_pos_in_canvas.y - pan_offset.y) * zoom_factor;
    }

    ImVec2 scaled_image_size = ImVec2(image_size.x * zoom, image_size.y * zoom);
    pan_offset.x = ImClamp(pan_offset.x, ImMin(0.0f, content_avail.x - scaled_image_size.x), 0.0f);
    pan_offset.y = ImClamp(pan_offset.y, ImMin(0.0f, content_avail.y - scaled_image_size.y), 0.0f);

    draw_list->AddImage(my_tex_id,
        ImVec2(canvas_p0.x + pan_offset.x, canvas_p0.y + pan_offset.y),
        ImVec2(canvas_p0.x + pan_offset.x + scaled_image_size.x, canvas_p0.y + pan_offset.y + scaled_image_size.y));

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::End();
}