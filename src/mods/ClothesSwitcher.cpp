#include "ClothesSwitcher.hpp"
#if 1

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
    return static_cast<int>(std::distance(clothing_items.begin(), it));
};

void ClothesSwitcher::on_draw_ui() {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        std::vector<int> selected_indices(combo_boxes.size(), 0);

        // Thin randomizer button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.55f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        if (ImGui::Button("Randomize Outfit", ImVec2(-FLT_MIN, 28.0f))) {
            for (size_t i = 1; i < combo_boxes.size(); ++i) { // Skip Sword (index 0)
                const auto& combo = combo_boxes[i];
                size_t range      = combo.end_idx - combo.start_idx;
                if (range > 0) {
                    size_t random_idx = combo.start_idx + (rand() % range);
                    if (random_idx < clothing_items.size()) {
                        selected_indices[i] = static_cast<int>(random_idx - combo.start_idx);
                        nmh_sdk::SetEquip((pcItem)clothing_items[random_idx].id, 0);
                    }
                }
            }
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        if (ImGui::BeginTabBar("##clothes switcher")) {
            for (size_t i = 0; i < combo_boxes.size(); ++i) {
                const auto& combo     = combo_boxes[i];
                const char* tab_label = nullptr;

                switch (i) {
                case 0:
                    tab_label = "Sword";
                    break;
                case 1:
                    tab_label = "Glasses";
                    break;
                case 2:
                    tab_label = "Jacket";
                    break;
                case 3:
                    tab_label = "Shoes";
                    break;
                case 4:
                    tab_label = "Jeans";
                    break;
                case 5:
                    tab_label = "Belt";
                    break;
                case 6:
                    tab_label = "Shirt";
                    break;
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
