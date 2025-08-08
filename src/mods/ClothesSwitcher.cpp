#include "ClothesSwitcher.hpp"
#if 1
#include <algorithm>
#include <deque>
#include <optional>
#include <string>
#include <vector>
#include <random>
#include <chrono>

struct ComboInfo {
    const char* label;
    size_t start_idx;
    size_t end_idx;
};

static constexpr std::array<ComboInfo, 7> combo_boxes = {
    ComboInfo{"## Sword Combo", 0, 16},
    ComboInfo{"## Glasses Combo", 100, 105},
    ComboInfo{"## Jacket Combo", 200, 215},
    ComboInfo{"## Shoes Combo", 300, 309},
    ComboInfo{"## Jeans Combo", 400, 411},
    ComboInfo{"## Belt Combo", 500, 509},
    ComboInfo{"## Shirt Combo", 600, 744},
};

std::optional<std::string> ClothesSwitcher::on_initialize() {
    return Mod::on_initialize();
}

static int find_room_index_by_id(int id) {
    auto it = std::find_if(clothing_items.begin(), clothing_items.end(), [id](const Equip_Item& room) { return room.id == id; });
    IM_ASSERT(it != clothing_items.end());
    return (int)std::distance(clothing_items.begin(), it);
};

// -------------------------- Simple Toast (top-right, single) --------------------------
struct ToastItem {
    std::string text;
    float age = 0.0f;
};

static std::deque<std::string> s_toastQueue;   // pending texts (kept for future use)
static std::optional<ToastItem> s_activeToast; // only one on-screen at a time

static inline float clamp01(float t) {
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    return t;
}

static inline float smooth01(float t) { // smoothstep
    t = clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}

static void push_toast(const std::string& txt) {
    s_toastQueue.emplace_back(txt);
}

// --- Improved random selection: avoid recent repeats ---
static std::string pick_nonrepeating_toast()
{
    static const char* k_toast_msgs[] = {
        "Hope you like the new color",
        "Thanks for shopping with us!",
        "Does this make my katana look big?",
        "Buy 1 get 1 free aficionado",
        "Coupon Collector",
        "Do I see sheets of plastic in your future?",
        "Hi there Fuzzball",
        "I know it was you, Fredo.",
        "Funny how?",
        "I'm Johnny on the spot, I'll hook ya up!",
        "YOUR HEAD A SPLODE",
        "140.15",
        "Cryptic Metaphor!",
        "400%",
        "May you find your proper path",
        "Mr. Angelo?",
        "This is just a tribute...",
        "First Blood!",
        "Secured C!",
        "Rangers lead the way!",
        "HIDEO", 
        "Cool Trasition"
    };

    const int N = (int)(sizeof(k_toast_msgs) / sizeof(k_toast_msgs[0]));
    if (N <= 1) return k_toast_msgs[0];

    // RNG (seed once)
    static std::mt19937 rng((uint32_t)std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Keep a small history of recently used indices to avoid repeats
    static std::deque<int> history;            // most recent first
    static const int HISTORY_LIMIT = 4;        // avoid the last 4

    // Build a pool excluding recent history
    std::vector<int> pool;
    pool.reserve(N);
    for (int i = 0; i < N; ++i) {
        if (std::find(history.begin(), history.end(), i) == history.end()) {
            pool.push_back(i);
        }
    }
    if (pool.empty()) {
        // If history covered all, clear oldest and rebuild
        history.clear();
        for (int i = 0; i < N; ++i) pool.push_back(i);
    }

    std::uniform_int_distribution<int> dist(0, (int)pool.size() - 1);
    int idx = pool[dist(rng)];

    // Update history
    history.push_front(idx);
    while ((int)history.size() > HISTORY_LIMIT) history.pop_back();

    return std::string(k_toast_msgs[idx]);
}

// Replace the current toast immediately (force out old one)
static void show_toast_now(const std::string& txt) {
    s_toastQueue.clear();
    s_activeToast = ToastItem{ txt, 0.0f }; // new toast starts fresh, old one is gone
}

// Call each frame from on_draw_ui()
static void render_toast() {
    // Timing (longer hold)
    constexpr float IN_TIME   = 0.22f;
    constexpr float HOLD_TIME = 1.85f; // longer dwell
    constexpr float OUT_TIME  = 0.32f;
    constexpr float TOTAL     = IN_TIME + HOLD_TIME + OUT_TIME;

    ImGuiIO& io = ImGui::GetIO();

    // Promote next queued toast if none active
    if (!s_activeToast.has_value() && !s_toastQueue.empty()) {
        s_activeToast = ToastItem{s_toastQueue.front(), 0.0f};
        s_toastQueue.pop_front();
    }
    if (!s_activeToast)
        return;

    ToastItem& t = *s_activeToast;
    t.age += io.DeltaTime;

    float alpha            = 1.0f;
    float slidePx          = 0.0f; // +X = slide to the right (off-screen)
    const float SLIDE_DIST = 64.0f;

    if (t.age < IN_TIME) {
        float k = smooth01(t.age / IN_TIME);
        alpha   = 0.85f + 0.15f * k;
        slidePx = (1.0f - k) * SLIDE_DIST; // slide in from right
    } else if (t.age < IN_TIME + HOLD_TIME) {
        alpha   = 1.0f;
        slidePx = 0.0f;
    } else if (t.age < TOTAL) {
        float k = smooth01((t.age - (IN_TIME + HOLD_TIME)) / OUT_TIME);
        alpha   = 1.0f - 0.85f * k;
        slidePx = k * SLIDE_DIST; // slide back to right
    } else {
        s_activeToast.reset();
        return;
    }

    // Window at top-right (square corners)
    ImGuiViewport* vp  = ImGui::GetMainViewport();
    const float margin = 12.0f;
    ImVec2 basePos     = ImVec2(vp->WorkPos.x + vp->WorkSize.x - margin + slidePx, vp->WorkPos.y + margin);

    ImGui::SetNextWindowPos(basePos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.9f * alpha);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);         // sharp corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8)); // simple

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("##clothes_toast_window", nullptr, flags)) {
        // Soft shadow
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wmin    = ImGui::GetWindowPos();
        ImVec2 wmax    = ImVec2(wmin.x + ImGui::GetWindowSize().x, wmin.y + ImGui::GetWindowSize().y);
        dl->AddRectFilled(
            ImVec2(wmin.x + 3, wmin.y + 3), ImVec2(wmax.x + 3, wmax.y + 3), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.25f * alpha)), 0.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
        ImGui::TextUnformatted(t.text.c_str());
        ImGui::PopStyleColor();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}
// ----------------------------------------------------------------------

void ClothesSwitcher::on_draw_ui() {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        std::vector<int> selected_indices(combo_boxes.size(), 0);

        // ---------------- Hold-to-Randomize (fires on release after 1s) ----------------
        static float s_holdTimer = 0.0f;
        static bool s_wasActive  = false;
        const float HOLD_SECONDS = 1.0f;

        auto do_randomize = [&]() {
            for (size_t i = 1; i < combo_boxes.size(); ++i) { // Skip Sword (index 0)
                const auto& combo  = combo_boxes[i];
                const size_t range = combo.end_idx - combo.start_idx;
                if (range > 0) {
                    const size_t random_idx = combo.start_idx + (rand() % range);
                    if (random_idx < clothing_items.size()) {
                        selected_indices[i] = static_cast<int>(random_idx - combo.start_idx);
                        nmh_sdk::SetEquip((pcItem)clothing_items[random_idx].id, 0);
                    }
                }
            }

            // --- NEW: pick a toast with low repetition and show it immediately
            show_toast_now(pick_nonrepeating_toast());
        };

        // Button UI
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.55f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImGui::Button("Hold To Randomize Outfit", ImVec2(-FLT_MIN, 28.0f));

        // Logic + progress overlay
        ImGuiIO& io        = ImGui::GetIO();
        const bool hovered = ImGui::IsItemHovered();
        const bool active  = ImGui::IsItemActive();

        if (active) {
            s_wasActive = true;
            s_holdTimer += io.DeltaTime;

            // Progress fill (left-to-right), manual clamp
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            float w    = max.x - min.x;
            float t    = s_holdTimer / HOLD_SECONDS;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            ImGui::GetWindowDrawList()->AddRectFilled(min, ImVec2(min.x + w * t, max.y), ImGui::GetColorU32(ImVec4(1, 1, 1, 0.15f)));
        }

        // Fire only when released AND held long enough
        if (!active && s_wasActive) {
            if (s_holdTimer >= HOLD_SECONDS) {
                do_randomize();
            }
            s_holdTimer = 0.0f;
            s_wasActive = false;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        // -------------------------------------------------------------------------------

        // Draw the toast (top-right)
        render_toast();

        if (ImGui::BeginTabBar("##clothes switcher")) {
            for (size_t i = 0; i < combo_boxes.size(); ++i) {
                const auto& combo     = combo_boxes[i];
                const char* tab_label = nullptr;

                switch (i) {
                case 0: tab_label = "Sword";   break;
                case 1: tab_label = "Glasses"; break;
                case 2: tab_label = "Jacket";  break;
                case 3: tab_label = "Shoes";   break;
                case 4: tab_label = "Jeans";   break;
                case 5: tab_label = "Belt";    break;
                case 6: tab_label = "Shirt";   break;
                }

                if (ImGui::BeginTabItem(tab_label)) {
                    int equip_slot = static_cast<int>(i);
                    int playerItem = find_room_index_by_id(player->mPcStatus.equip[equip_slot].id);
                    if (playerItem >= 0 && playerItem < static_cast<int>(clothing_items.size())) {
                        selected_indices[i] = playerItem - static_cast<int>(combo.start_idx);
                    }

                    if (ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN))) {
                        for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); ++n) {
                            bool is_selected = (selected_indices[i] == static_cast<int>(n - combo.start_idx));
                            if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                                selected_indices[i] = static_cast<int>(n - combo.start_idx);
                                nmh_sdk::SetEquip((pcItem)clothing_items[n].id, 0);
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
}
#endif
