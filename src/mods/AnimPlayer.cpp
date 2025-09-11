#include "AnimPlayer.hpp"
#if 1
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm> // move/insert/erase
#include <cmath>     // fmodf, sinf, floorf
#include <cstring>   // std::strstr
#include "StanceControl.hpp" // for mod_enabled_disable_combo_extend_speedup

bool AnimPlayer::imguiPopout             = false;
float AnimPlayer::custom_anim_speed      = 1.0f;
static bool isPlayingAnimPlaylist        = false;
uintptr_t AnimPlayer::anim_speed_jmp_ret = NULL;
static bool enemyMotPlayer               = false;
static int currentSelectedEnemy           = 0;
static bool useCurrentSelectedEnemySlider = true;

static std::unordered_map<std::string, std::vector<AnimationEntry>> animationPlaylists;
static size_t currentIndex       = 0;
static float timeSinceLast       = 0.0f;
static float lastTime            = 0.0f;
static int lastMotionNo          = -1;
static float motionCheckDelay    = 0.0f;
static std::string currentWeapon = "Berry";

// ------------------------------
// Small math helpers
// ------------------------------
static inline float clampf(float v, float mn, float mx) {
    return v < mn ? mn : (v > mx ? mx : v);
}
static inline float maxf(float a, float b) {
    return (a > b) ? a : b;
}
static inline ImVec2 lerp2(const ImVec2& a, const ImVec2& b, float t) {
    return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}
static inline ImVec2 bezier_cubic(const ImVec2& p0, const ImVec2& c1, const ImVec2& c2, const ImVec2& p1, float t) {
    const float it  = 1.0f - t;
    const float it2 = it * it;
    const float t2  = t * t;
    const float a   = it2 * it;
    const float b   = 3.0f * it2 * t;
    const float c   = 3.0f * it * t2;
    const float d   = t * t2;
    return ImVec2(a * p0.x + b * c1.x + c * c2.x + d * p1.x, a * p0.y + b * c1.y + c * c2.y + d * p1.y);
}

// ------------------------------
// Drag-reorder support (handle in first column)
// ------------------------------
static const char* kDragPayloadType = "AP_ROW";
static int g_drag_source_index      = -1;

// ------------------------------
// Connector stem animation
// ------------------------------
struct ConnectorStemAnim {
    bool active       = false;
    bool target_ready = false;
    int target_index  = -1; // row index in playlist we just appended
    ImVec2 p0;              // start (button center)
    ImVec2 p1;              // end (row anchor, resolved after layout)
    double t0      = 0.0;   // start time
    float duration = 0.45f; // seconds
};
static ConnectorStemAnim g_connector;

// ------------------------------
// Cue Strip state
// ------------------------------
static bool g_timeline_scrubbing = false; // retained to freeze clock if needed
static float g_scrub_time_accum  = 0.0f;

// Snap and resize behavior
static bool g_cue_snap_enabled = true;
static float g_cue_snap_step   = 0.05f; // 50 ms
static inline float snapf(float v, float step) {
    if (step <= 0.0f)
        return v;
    return floorf((v / step) + 0.5f) * step;
}

// Right-edge resize interaction
struct CueResizeState {
    bool active        = false;
    int index          = -1;   // which cue we are resizing
    float start_delay  = 0.0f; // original delay at drag start
    float start_total  = 0.0f; // total duration at drag start
    float start_mousex = 0.0f; // mouse x at drag start
};
static CueResizeState g_cue_resize;

// ---------------------------------------------
// Helper: move vector element (src -> dst)
// ---------------------------------------------
template <class T> static inline void move_item(std::vector<T>& v, int src, int dst) {
    if (src == dst || src < 0 || dst < 0 || src >= (int)v.size() || dst > (int)v.size())
        return;
    T tmp = v[src];
    if (src < dst) {
        v.erase(v.begin() + src);
        v.insert(v.begin() + (dst - 1), tmp);
    } else {
        v.erase(v.begin() + src);
        v.insert(v.begin() + dst, tmp);
    }
}

// ---------------------------------------------
// Draw connector stem if animating
// ---------------------------------------------
static void draw_connector_stem() {
    if (!g_connector.active)
        return;

    float t = (float)((ImGui::GetTime() - g_connector.t0) / g_connector.duration);
    if (t >= 1.0f) {
        g_connector.active = false;
        return;
    }

    float e = t * t * (3.0f - 2.0f * t); // smoothstep

    ImVec2 start = g_connector.p0;
    ImVec2 end   = g_connector.target_ready ? g_connector.p1 : g_connector.p0;

    ImVec2 end_now = lerp2(start, end, e);

    float dx  = (end.x - start.x) * 0.45f;
    ImVec2 c1 = ImVec2(start.x + dx, start.y);
    ImVec2 c2 = ImVec2(end.x - dx, end.y);

    ImDrawList* dl = ImGui::GetForegroundDrawList();

    ImU32 lime    = IM_COL32(40, 254, 32, 220);
    const int SEG = 24;
    ImVec2 prev   = start;
    for (int i = 1; i <= SEG; ++i) {
        float tt = (float)i / (float)SEG;
        tt       = clampf(tt, 0.0f, e);
        ImVec2 p = bezier_cubic(start, c1, c2, end, tt);
        dl->AddLine(prev, p, lime, 1.75f);
        prev = p;
    }
    dl->AddCircleFilled(end_now, 2.0f, lime, 12);
}

// ============================================================================
// MAIN UI BODY
// ============================================================================
void AnimPlayer::Stuff() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player)
        return;
    ImGui::SliderFloat("Current attack anim progress", &player->tagMain->Motion[0].MotionType3Anm[0].Main.PlayTick, 0.0f, player->tagMain->Motion[0].MotionType3Anm[0].Main.EndTick);
    // you can also use player->tagMain->Motion[0].PlayMotionTick; for current frame, this also includes non attack anims and loops

    // Square scrollbars for this scope
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 0.0f);

    const ImGuiStyle& style     = ImGui::GetStyle();
    const float frame_h         = ImGui::GetFrameHeight();              // standard widget height
    const float cue_block_h     = frame_h;                              // thin blocks = button height
    const float cue_vpad        = 8.0f;                                 // vertical padding around blocks
    const float cue_border_thk  = 3.0f;                                 // clear border
    const float cue_resize_zone = 14.0f;                                // fat right edge
    const float cue_strip_h     = cue_block_h + cue_vpad * 2;           // draw area height
    const float cue_controls_h  = frame_h + style.ItemSpacing.y * 1.5f; // "Cue Strip ..." + Snap + Step
    const float cue_total_h     = cue_controls_h + cue_strip_h;         // total reserved height

    if (ImGui::BeginTabBar("##weapon_tabs")) {
        for (const auto& weaponData : weaponAnimationData) {
            if (!weaponData.name)
                continue;

            if (ImGui::BeginTabItem(weaponData.name)) {
                currentWeapon = weaponData.name;

                // ------------------------------------------------------------
                // Compute left panel width: slightly larger than longest label
                // ------------------------------------------------------------
                float max_label_px = 0.0f;
                for (const auto& anim : weaponData.animations) {
                    std::string label = std::string(anim.first) + " (" + std::to_string(anim.second) + ")";
                    ImVec2 sz         = ImGui::CalcTextSize(label.c_str());
                    if (sz.x > max_label_px)
                        max_label_px = sz.x;
                }
                const float left_padding_px = 28.0f;
                float list_width            = max_label_px + left_padding_px;
                float max_cap               = ImGui::GetContentRegionAvail().x * 0.55f;
                if (list_width < 220.0f)
                    list_width = 220.0f;
                if (list_width > max_cap)
                    list_width = max_cap;

                // -------------------------
                // Search bar (always visible)
                // -------------------------
                static char filterBuf[96] = {};
                ImGui::SetNextItemWidth(list_width);
                ImGui::InputTextWithHint("##filter", "Search...", filterBuf, IM_ARRAYSIZE(filterBuf));
                bool hasFilter = filterBuf[0] != 0;

                // ------------------------------------------------------------
                // Compute remaining height AFTER drawing the search line,
                // then reserve a fixed slice at the BOTTOM for the cue panel.
                // ------------------------------------------------------------
                float avail_h      = ImGui::GetContentRegionAvail().y;
                float panes_height = avail_h - cue_total_h - style.ItemSpacing.y; // leave spacing above bottom child
                if (panes_height < 80.0f)
                    panes_height = 80.0f; // minimum workable area

                // -------------------------
                // LEFT: Catalog (fixed width, non-scaling)
                // -------------------------
                ImGui::BeginChild("##anim_buttons", ImVec2(list_width, panes_height), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
                ImGui::Separator();
                for (const auto& anim : weaponData.animations) {
                    if (hasFilter && std::strstr(anim.first, filterBuf) == nullptr)
                        continue;
                    std::string buttonLabel = std::string(anim.first) + " (" + std::to_string(anim.second) + ")";
                    ImGui::PushID(buttonLabel.c_str()); // unique
                    if (ImGui::Button(buttonLabel.c_str(), ImVec2(-1.0f, 0.0f))) {
                        animationPlaylists[weaponData.name].emplace_back(anim.second, 0.2f, 1.0f);

                        g_connector.active       = true;
                        g_connector.target_ready = false;
                        g_connector.target_index = (int)animationPlaylists[weaponData.name].size() - 1;
                        g_connector.t0           = ImGui::GetTime();

                        ImVec2 mn      = ImGui::GetItemRectMin();
                        ImVec2 mx      = ImGui::GetItemRectMax();
                        g_connector.p0 = ImVec2((mn.x + mx.x) * 0.5f, (mn.y + mx.y) * 0.5f);
                    }
                    ImGui::PopID();
                }
                ImGui::EndChild();

                ImGui::SameLine();

                // -------------------------
                // RIGHT: Playlist + controls
                // -------------------------
                ImGui::BeginChild("##playlist_right", ImVec2(0, panes_height), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

                auto& playlist = animationPlaylists[currentWeapon];

                // Toolbar + Now/Next HUD
                {
                    bool playing = isPlayingAnimPlaylist;
                    if (ImGui::Button(playing ? "Pause" : "Play All")) {
                        if (!playlist.empty()) {
                            isPlayingAnimPlaylist = !playing || !isPlayingAnimPlaylist;
                            if (isPlayingAnimPlaylist) {
                                if (!playing) {
                                    currentIndex  = 0;
                                    timeSinceLast = 0.0f;
                                }
                                lastTime = ImGui::GetTime();
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Clear")) {
                        playlist.clear();
                        isPlayingAnimPlaylist = false;
                        currentIndex          = 0;
                        lastMotionNo          = -1;
                    }

                    ImGui::SameLine();
                    ImGui::TextDisabled("|");
                    ImGui::SameLine();
                    ImGui::Text("Animation Playlist");

                    if (!playlist.empty()) {
                        ImGui::SameLine();
                        ImGui::TextDisabled(" | ");
                        ImGui::SameLine();
                        if (isPlayingAnimPlaylist && currentIndex < playlist.size()) {
                            const AnimationEntry& now = playlist[currentIndex];
                            float remain              = maxf(0.0f, now.delay - timeSinceLast);
                            int next_i                = (int)currentIndex + 1;
                            if (next_i < (int)playlist.size()) {
                                const AnimationEntry& nxt = playlist[next_i];
                                ImGui::Text("Now: wait %.2fs -> %d  |  Next: %d", remain, now.id, nxt.id);
                            } else {
                                ImGui::Text("Now: wait %.2fs -> %d  |  Next: end", remain, now.id);
                            }
                        } else {
                            ImGui::Text("Ready");
                        }
                    }
                }

                ImGui::Separator();

                // Table (excel-like)
                ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit |
                                         ImGuiTableFlags_Resizable | ImGuiTableFlags_NoHostExtendX;

                if (ImGui::BeginTable("##playlist_table", 5, tflags, ImVec2(-1.0f, 0.0f))) {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 28.0f); // drag handle
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                    ImGui::TableSetupColumn("Delay (s)", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthStretch, 0.0f);
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < (int)playlist.size();) {
                        AnimationEntry& entry = playlist[i];
                        ImGui::PushID(i);

                        ImGui::TableNextRow();

                        // Column 0: drag handle
                        ImGui::TableSetColumnIndex(0);
                        if (ImGui::SmallButton("|||")) { /* no-op */
                        }
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            g_drag_source_index = i;
                            ImGui::SetDragDropPayload(kDragPayloadType, &g_drag_source_index, sizeof(int));
                            ImGui::Text("Move %d", i + 1);
                            ImGui::EndDragDropSource();
                        }
                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(kDragPayloadType)) {
                                int src = *(const int*)p->Data;
                                int dst = i;
                                if (src != dst && src >= 0 && src < (int)playlist.size()) {
                                    move_item<AnimationEntry>(playlist, src, dst);
                                    ImGui::PopID();
                                    continue; // re-render this slot after move
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }

                        // Column 1: ID number only
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%d", entry.id);
                        if (g_connector.active && !g_connector.target_ready && g_connector.target_index == i) {
                            ImVec2 txt_min           = ImGui::GetItemRectMin();
                            ImVec2 txt_max           = ImGui::GetItemRectMax();
                            g_connector.p1           = ImVec2(txt_min.x - 8.0f, (txt_min.y + txt_max.y) * 0.5f);
                            g_connector.target_ready = true;
                        }

                        // Column 2: Delay
                        ImGui::TableSetColumnIndex(2);
                        ImGui::SetNextItemWidth(-1.0f);
                        ImGui::DragFloat("##Delay", &entry.delay, 0.01f, 0.01f, 60.0f, "%.2f");
                        if (entry.delay < 0.01f)
                            entry.delay = 0.01f;

                        // Column 3: Speed
                        ImGui::TableSetColumnIndex(3);
                        ImGui::SetNextItemWidth(-1.0f);
                        ImGui::DragFloat("##Speed", &entry.speed, 0.01f, 0.01f, 10.0f, "%.2f");

                        // Column 4: Actions
                        ImGui::TableSetColumnIndex(4);
                        bool remove = ImGui::SmallButton("Remove");
                        if (remove) {
                            playlist.erase(playlist.begin() + i);
                            if ((int)currentIndex > i)
                                currentIndex--;
                            else if ((int)currentIndex >= (int)playlist.size())
                                currentIndex = (int)playlist.size();
                            ImGui::PopID();
                            continue;
                        }

                        ImGui::PopID();
                        ++i;
                    }

                    ImGui::EndTable();
                }

                ImGui::EndChild(); // playlist_right

                // -------------------------
                // BOTTOM: Cue Panel in its own fixed-height child
                // -------------------------
                ImGui::Dummy(ImVec2(0, style.ItemSpacing.y)); // spacing above bottom bar
                ImGui::BeginChild(
                    "##cue_panel", ImVec2(0, cue_total_h), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                // Controls row
                {
                    ImGui::TextDisabled("Cue Strip: Click block to jump. Drag RIGHT edge to resize (Ctrl = slip next, Alt = no snap).");
                    ImGui::SameLine();
                    ImGui::Checkbox("Snap", &g_cue_snap_enabled);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(80.0f);
                    ImGui::DragFloat("Step", &g_cue_snap_step, 0.01f, 0.0f, 1.0f, "%.2f");
                    g_cue_snap_step = clampf(g_cue_snap_step, 0.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0, style.ItemSpacing.y * 0.5f));
                }

                // Strip drawing area fills the rest of this child and is ALWAYS visible
                {
                    ImVec2 tl_min  = ImGui::GetCursorScreenPos();
                    ImVec2 tl_max  = ImVec2(tl_min.x + ImGui::GetContentRegionAvail().x, tl_min.y + cue_strip_h);
                    ImDrawList* dl = ImGui::GetWindowDrawList();

                    // Background + top accent
                    dl->AddRectFilled(tl_min, tl_max, IM_COL32(0, 0, 0, 40), 0.0f);
                    ImU32 lime_line = IM_COL32(40, 254, 32, 200);
                    dl->AddLine(ImVec2(tl_min.x, tl_min.y + 2.0f), ImVec2(tl_max.x, tl_min.y + 2.0f), lime_line, 2.0f);

                    // Data
                    auto& playlist2 = animationPlaylists[currentWeapon];
                    float total     = 0.0f;
                    for (auto& e : playlist2)
                        total += maxf(0.01f, e.delay);
                    if (total <= 0.0f)
                        total = 1.0f;

                    // Geometry
                    float x        = tl_min.x + 8.0f;
                    float inner_y0 = tl_min.y + cue_vpad;
                    float inner_y1 = inner_y0 + cue_block_h;
                    float w_total  = (tl_max.x - tl_min.x) - 16.0f;

                    float acc_time = 0.0f;
                    for (int i = 0; i < (int)playlist2.size(); ++i) {
                        float seg = maxf(0.01f, playlist2[i].delay);
                        float w   = w_total * (seg / total);

                        ImVec2 bmin = ImVec2(x, inner_y0);
                        ImVec2 bmax = ImVec2(x + w, inner_y1);

                        // Base + border (square)
                        ImU32 base_col = (i % 2 == 0) ? IM_COL32(255, 255, 255, 30) : IM_COL32(255, 255, 255, 18);
                        dl->AddRectFilled(bmin, bmax, base_col, 0.0f);
                        dl->AddRect(bmin, bmax, IM_COL32(180, 180, 180, 220), 0.0f, 0, cue_border_thk);

                        // Fill state
                        if ((size_t)i < currentIndex) {
                            dl->AddRectFilled(bmin, bmax, IM_COL32(80, 255, 80, 90), 0.0f);
                        } else if ((size_t)i == currentIndex && isPlayingAnimPlaylist) {
                            float local_d = maxf(0.01f, playlist2[i].delay);
                            float t       = clampf(timeSinceLast / local_d, 0.0f, 1.0f);
                            float fx      = bmin.x + (bmax.x - bmin.x) * t;
                            dl->AddRectFilled(bmin, ImVec2(fx, bmax.y), IM_COL32(40, 254, 32, 140), 0.0f);

                            char cd[64];
                            float remain = maxf(0.0f, local_d - timeSinceLast);
                            snprintf(cd, sizeof(cd), "%.2fs -> %d", remain, playlist2[i].id);
                            ImVec2 ts = ImGui::CalcTextSize(cd);
                            float tx  = bmin.x + (w - ts.x) * 0.5f;
                            float ty  = inner_y0 + (cue_block_h - ts.y) * 0.5f;
                            if (w >= ts.x + 6.0f)
                                dl->AddText(ImVec2(tx, ty), IM_COL32(230, 230, 230, 240), cd);
                        } else {
                            char idbuf[16];
                            snprintf(idbuf, sizeof(idbuf), "%d", playlist2[i].id);
                            ImVec2 ts = ImGui::CalcTextSize(idbuf);
                            float tx  = bmin.x + (w - ts.x) * 0.5f;
                            float ty  = inner_y0 + (cue_block_h - ts.y) * 0.5f;
                            if (w >= ts.x + 6.0f)
                                dl->AddText(ImVec2(tx, ty), IM_COL32(220, 220, 220, 180), idbuf);
                        }

                        // Tooltip
                        char ttbuf[96];
                        snprintf(ttbuf, sizeof(ttbuf), "Starts at %.2fs\nID %d\nDelay %.2fs @ Speed %.2f", acc_time, playlist2[i].id,
                            playlist2[i].delay, playlist2[i].speed);

                        // Interactive zones with unique IDs
                        ImGui::PushID(i);

                        // Right-edge resize handle
                        ImVec2 edge_min = ImVec2(bmax.x - cue_resize_zone, bmin.y);
                        ImVec2 edge_max = ImVec2(bmax.x, bmax.y);
                        ImGui::SetCursorScreenPos(edge_min);
                        ImGui::InvisibleButton("##cue_edge", ImVec2(edge_max.x - edge_min.x, edge_max.y - edge_min.y));
                        bool edge_hovered = ImGui::IsItemHovered();
                        if (edge_hovered) {
                            ImGui::SetTooltip("Drag to resize (Ctrl = slip next, Alt = no snap)");
                            dl->AddLine(ImVec2(edge_min.x, edge_min.y), ImVec2(edge_min.x, edge_max.y), IM_COL32(240, 240, 240, 160), 2.0f);
                            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                        }
                        if (edge_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !g_cue_resize.active) {
                            g_cue_resize.active       = true;
                            g_cue_resize.index        = i;
                            g_cue_resize.start_delay  = playlist2[i].delay;
                            g_cue_resize.start_total  = total;
                            g_cue_resize.start_mousex = ImGui::GetIO().MousePos.x;
                        }

                        // Body click to jump (excluding resize zone)
                        ImVec2 body_min = ImVec2(bmin.x, bmin.y);
                        ImVec2 body_max = ImVec2(bmax.x - cue_resize_zone, bmax.y);
                        if (body_max.x < body_min.x)
                            body_max.x = body_min.x;
                        ImGui::SetCursorScreenPos(body_min);
                        ImGui::InvisibleButton("##cue_body", ImVec2(body_max.x - body_min.x, body_max.y - body_min.y));
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("%s\n(Click to jump)", ttbuf);
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                            currentIndex          = (size_t)i;
                            timeSinceLast         = 0.0f;
                            isPlayingAnimPlaylist = true;
                            lastTime              = ImGui::GetTime();
                        }

                        ImGui::PopID();

                        x += w;
                        acc_time += seg;
                    }

                    // Resize handling
                    if (g_cue_resize.active && g_cue_resize.index >= 0 && g_cue_resize.index < (int)playlist2.size()) {
                        float curr_total = 0.0f;
                        for (auto& e : playlist2)
                            curr_total += maxf(0.01f, e.delay);
                        if (curr_total <= 0.0f)
                            curr_total = 1.0f;

                        float w_total    = (tl_max.x - tl_min.x) - 16.0f;
                        float sec_per_px = curr_total / (w_total > 1.0f ? w_total : 1.0f);

                        float mx    = ImGui::GetIO().MousePos.x;
                        float delta = (mx - g_cue_resize.start_mousex) * sec_per_px;

                        float new_delay = g_cue_resize.start_delay + delta;
                        if (g_cue_snap_enabled && !ImGui::GetIO().KeyAlt)
                            new_delay = snapf(new_delay, g_cue_snap_step);
                        new_delay = clampf(new_delay, 0.01f, 60.0f);

                        int i = g_cue_resize.index;
                        if (ImGui::GetIO().KeyCtrl && (i + 1) < (int)playlist2.size()) {
                            float delta_applied = new_delay - playlist2[i].delay;
                            float& next_delay   = playlist2[i + 1].delay;
                            float new_next      = clampf(next_delay - delta_applied, 0.01f, 60.0f);
                            playlist2[i].delay  = new_delay;
                            next_delay          = new_next;
                        } else {
                            playlist2[i].delay = new_delay;
                        }

                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                            g_cue_resize = {};
                    }

                    // Occupy space inside the child so layout accounts for it
                    ImGui::Dummy(ImVec2(0, cue_strip_h));
                }

                ImGui::EndChild(); // cue_panel

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    draw_connector_stem();

    ImGui::PopStyleVar(); // ScrollbarRounding
}

static mHRChara* currentNPC = nullptr;
static void EnemyMotPlayerDisplay(mHRChara* currentNPC, int it) {
    if (currentNPC) {
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        Vec enemyPos      = currentNPC->mStatus.pos;
        Vec screenEnemyPos{0.0f, 0.0f, 0.0f};
        nmh_sdk::GetScreenPos(&enemyPos, &screenEnemyPos);
        float scaleX = screenSize.x / 854.0f;
        float scaleY = screenSize.y / 480.0f;
        ImVec2 scaledPos(screenEnemyPos.x * scaleX, screenEnemyPos.y * scaleY);
        std::string imguiWindowName = "Mot Player##" + std::to_string(it);
        ImGui::Begin(imguiWindowName.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(scaledPos.x, scaledPos.y));
        int charaType = currentNPC->mStatus.charaType;
        if (charaType < 525) { // filter out non zakos here
            static int motion        = 0;
            static bool loop         = false;
            static float startFrame  = 0.0f;
            static bool overwrite    = true;
            static float interpolate = 0.1f;
            float fontSize           = ImGui::GetFontSize();
            ImGui::PushItemWidth(fontSize * 5.0f);
            ImGui::InputInt("Motion ID", &motion, 1, 10);
            ImGui::Checkbox("Overwrite", &overwrite);
            ImGui::SliderFloat("Interpolation", &interpolate, 0.0f, 1.0f);
            ImGui::SliderFloat("Start Frame", &startFrame, 0.0f, 30.0f);
            ImGui::Checkbox("Loop", &loop);
            if (ImGui::Button("Play")) {
                nmh_sdk::PlayZakoMotion((HRZAKO*)currentNPC, motion, loop, startFrame, overwrite, interpolate);
            }
            ImGui::PopItemWidth();
        }
        ImGui::End();
    }
}

void AnimPlayer::on_draw_ui() {
    ImGui::Checkbox("View In Popout Window", &imguiPopout);
    if (!imguiPopout) {
        Stuff();
    }

    ImGui::Checkbox("Enemy Mot Player", &enemyMotPlayer);
    if (enemyMotPlayer) {
        ImGui::Indent();
        ImGui::Checkbox("Select Enemy With Slider", &useCurrentSelectedEnemySlider);
        if (useCurrentSelectedEnemySlider) {
            ImGui::Indent();
            ImGui::SliderInt("Current Enemy", &currentSelectedEnemy, 0, 29);
        }
    }
}

// do something every frame

void AnimPlayer::anim_player() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        float currentTime = ImGui::GetTime();
        float deltaTime   = currentTime - lastTime;
        lastTime          = currentTime;

        if (!g_timeline_scrubbing)
            timeSinceLast += deltaTime;

        auto& playlist = animationPlaylists[currentWeapon];

        if (isPlayingAnimPlaylist && currentIndex < playlist.size()) {
            if (timeSinceLast >= playlist[currentIndex].delay) {
                timeSinceLast = 0.0f;

                const AnimationEntry& entry = playlist[currentIndex];
                if (auto* pc = nmh_sdk::get_mHRPc()) {
                    (void)pc;
                    custom_anim_speed = entry.speed;
                    nmh_sdk::PlayMotion((pcMotion)entry.id, 0, 0, 1, 0.1f);
                    lastMotionNo = entry.id;
                }

                ++currentIndex;

                if (currentIndex >= playlist.size()) {
                    motionCheckDelay = 0.1f;
                }
            }
        }

        if (isPlayingAnimPlaylist && currentIndex >= playlist.size()) {
            motionCheckDelay -= deltaTime;
            if (motionCheckDelay <= 0.0f) {
                int currentMotion = player->mCharaStatus.motionNo;
                if (currentMotion != lastMotionNo) {
                    isPlayingAnimPlaylist = false;
                    currentIndex          = 0;
                    lastMotionNo          = -1;
                }
            }
        }
    }
}

void AnimPlayer::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("Animation Player", &imguiPopout);
        Stuff();
        ImGui::End();
    }
    anim_player();
    if (enemyMotPlayer) {
        if (useCurrentSelectedEnemySlider) {
            currentNPC = nmh_sdk::get_mHRBattle()->mpNpc[currentSelectedEnemy];
            EnemyMotPlayerDisplay(currentNPC, 0);
        } else {
            for (uint32_t i = 0; i < 30; i++) {
                currentNPC = nmh_sdk::get_mHRBattle()->mpNpc[i];
                EnemyMotPlayerDisplay(currentNPC, i);
            }
        }
    }
}

// clang-format off
static constexpr float oneFloat = 1.0f;
naked void detour_anim_speed() { 
    __asm {
        cmp byte ptr [isPlayingAnimPlaylist], 1
        je animPlaylistCode
        cmp byte ptr [StanceControl::mod_enabled_force_1_speed_anims], 1
        je writeOne
        jmp originalcode

        animPlaylistCode:
        movss xmm0, [AnimPlayer::custom_anim_speed]
        jmp retcode

        writeOne:
        movss xmm0, [oneFloat]
        jmp retcode

        originalcode:
        mulss xmm0, [edi+0x00000194]
        retcode:
        jmp dword ptr [AnimPlayer::anim_speed_jmp_ret]
    }
}
// clang-format on

std::optional<std::string> AnimPlayer::on_initialize() {
    if (!install_hook_offset(0x403CA7, anim_speed_hook, &detour_anim_speed, &AnimPlayer::anim_speed_jmp_ret, 8)) {
        spdlog::error("Failed to init anim_speed mod\n");
        return "Failed to init anim_speed mod";
    }

    return Mod::on_initialize();
}

// void AnimPlayer::on_draw_debug_ui() {}
#endif
