#include "ClothesSwitcher.hpp"
#if 1

struct ComboInfo {
    const char* label;
    size_t start_idx;
    size_t end_idx;
};

static constexpr std::array<ComboInfo, 7> combo_boxes = {
    ComboInfo {"## Sword Combo", 0, 16},
    ComboInfo {"## Glasses Combo", 100, 105},
    ComboInfo {"## Jacket Combo", 200, 215},
    ComboInfo {"## Shoes Combo", 300, 309},
    ComboInfo {"## Jeans Combo", 400, 411},
    ComboInfo {"## Belt Combo", 500, 509},
    ComboInfo {"## Shirt Combo", 600, 744},
};

std::optional<std::string> ClothesSwitcher::on_initialize() {
    return Mod::on_initialize();
}

static const Equip_Item* find_room_by_id(int id) {
    auto it = std::find_if(clothing_items.begin(), clothing_items.end(), [id](const Equip_Item& room) { return room.id == id; });
    IM_ASSERT(it != clothing_items.end()); // crash if we passed wrong index
    size_t index = std::distance(clothing_items.begin(), it);
    IM_ASSERT(index > 0);
    return &clothing_items[index];
};

static int find_room_index_by_id(int id) {
    auto it = std::find_if(clothing_items.begin(), clothing_items.end(), [id](const Equip_Item& room) { return room.id == id; });
    IM_ASSERT(it != clothing_items.end()); // crash if we passed wrong index
    size_t index = std::distance(clothing_items.begin(), it);
    // IM_ASSERT(index > 0);
    return index;
};

void ClothesSwitcher::on_draw_ui() {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        // Unrolled for loop because we want to print in a different order
        std::vector<int> selected_indices(combo_boxes.size(), 0);
        if (ImGui::BeginTabBar("##clothes switcher")) {

        // Combo 0: Sword
        if (ImGui::BeginTabItem("Sword"))
        {
            const auto& combo = combo_boxes[0];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[0].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[0] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[0];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN)/*, clothing_items[playerItem].name */)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[0] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[0] = n - combo.start_idx;
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

        // Combo 1: Glasses
        if (ImGui::BeginTabItem("Glasses")) {
            const auto& combo = combo_boxes[1];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[1].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[1] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[1];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN)/*, clothing_items[playerItem].name)*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[1] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[1] = n - combo.start_idx;
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

        // Combo 2: Jacket
        if (ImGui::BeginTabItem("Jacket")) {
            const auto& combo = combo_boxes[2];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[2].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[2] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[2];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN) /*, clothing_items[playerItem].name*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[2] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[2] = n - combo.start_idx;
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

        // Combo 6: Shirt
        if (ImGui::BeginTabItem("Shirt")) {
            const auto& combo = combo_boxes[6];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[6].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[6] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[6];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN) /*, clothing_items[playerItem].name*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[6] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[6] = n - combo.start_idx;
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

        // Combo 5: Belt
        if (ImGui::BeginTabItem("Belt")) {
            const auto& combo = combo_boxes[5];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[5].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[5] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[5];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN)/*, clothing_items[playerItem].name*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[5] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[5] = n - combo.start_idx;
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
        
        // Combo 4: Jeans
        if (ImGui::BeginTabItem("Jeans")) {
            const auto& combo = combo_boxes[4];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[4].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[4] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[4];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN) /*, clothing_items[playerItem].name*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[4] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[4] = n - combo.start_idx;
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

        // Combo 3: Shoes
        if (ImGui::BeginTabItem("Shoes")) {
            const auto& combo = combo_boxes[3];
            int playerItem = find_room_index_by_id(player->mPcStatus.equip[3].id);
            if (playerItem >= 0 && playerItem < clothing_items.size()) {
                selected_indices[3] = playerItem - combo.start_idx;
            }
            size_t selected_item_index = combo.start_idx + selected_indices[3];
            if (playerItem >= 0 && playerItem < clothing_items.size() && ImGui::BeginListBox(combo.label, ImVec2(-FLT_MIN, -FLT_MIN) /*, clothing_items[playerItem].name*/)) {
                for (size_t n = combo.start_idx; n < combo.end_idx && n < clothing_items.size(); n++) {
                    const bool is_selected = (selected_indices[3] == n - combo.start_idx);
                    if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                        selected_indices[3] = n - combo.start_idx;
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
        ImGui::EndTabBar();
        }

    }
}

// during load
//void ClothesSwitcher::on_config_load(const utility::Config &cfg) {}
// during save
//void ClothesSwitcher::on_config_save(utility::Config &cfg) {}
// do something every frame
//void ClothesSwitcher::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ClothesSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
