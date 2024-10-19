#include "ClothesSwitcher.hpp"
#if 1

struct Equip_Item {
    int id;
    const char* name;
};

static constexpr std::array<Equip_Item, 120> clothing_items = {
    Equip_Item  {GLASSES0,  "Glasses 0"},
    Equip_Item  {GLASSES1,  "Glasses 1"},
    Equip_Item  {GLASSES2,  "Glasses 2"},
    Equip_Item  {GLASSES3,  "Glasses 3"},
    Equip_Item  {GLASSES4,  "Glasses 4"},
    Equip_Item  {GLASSES5,  "Glasses 5"},
    Equip_Item  {GLASSES6,  "Glasses 6"},
    Equip_Item  {GLASSES7,  "Glasses 7"},
    Equip_Item  {GLASSES8,  "Glasses 8"},
    Equip_Item  {GLASSES9,  "Glasses 9"},
    Equip_Item  {GLASSES10, "Glasses 10"},
    Equip_Item  {GLASSES11, "Glasses 11"},
    Equip_Item  {GLASSES12, "Glasses 12"},
    Equip_Item  {GLASSES13, "Glasses 13"},
    Equip_Item  {GLASSES14, "Glasses 14"},
    Equip_Item  {GLASSES15, "Glasses 15"},
    Equip_Item  {GLASSES16, "Glasses 16"},
    Equip_Item  {GLASSES17, "Glasses 17"},
    Equip_Item  {GLASSES18, "Glasses 18"},
    Equip_Item  {GLASSES19, "Glasses 19"},

    Equip_Item  {JACKET0,  "Jacket 0"},
    Equip_Item  {JACKET1,  "Jacket 1"},
    Equip_Item  {JACKET2,  "Jacket 2"},
    Equip_Item  {JACKET3,  "Jacket 3"},
    Equip_Item  {JACKET4,  "Jacket 4"},
    Equip_Item  {JACKET5,  "Jacket 5"},
    Equip_Item  {JACKET6,  "Jacket 6"},
    Equip_Item  {JACKET7,  "Jacket 7"},
    Equip_Item  {JACKET8,  "Jacket 8"},
    Equip_Item  {JACKET9,  "Jacket 9"},
    Equip_Item  {JACKET10, "Jacket 10"},
    Equip_Item  {JACKET11, "Jacket 11"},
    Equip_Item  {JACKET12, "Jacket 12"},
    Equip_Item  {JACKET13, "Jacket 13"},
    Equip_Item  {JACKET14, "Jacket 14"},
    Equip_Item  {JACKET15, "Jacket 15"},
    Equip_Item  {JACKET16, "Jacket 16"},
    Equip_Item  {JACKET17, "Jacket 17"},
    Equip_Item  {JACKET18, "Jacket 18"},
    Equip_Item  {JACKET19, "Jacket 19"},

    Equip_Item  {SHOES0,  "Shoes 0"},
    Equip_Item  {SHOES1,  "Shoes 1"},
    Equip_Item  {SHOES2,  "Shoes 2"},
    Equip_Item  {SHOES3,  "Shoes 3"},
    Equip_Item  {SHOES4,  "Shoes 4"},
    Equip_Item  {SHOES5,  "Shoes 5"},
    Equip_Item  {SHOES6,  "Shoes 6"},
    Equip_Item  {SHOES7,  "Shoes 7"},
    Equip_Item  {SHOES8,  "Shoes 8"},
    Equip_Item  {SHOES9,  "Shoes 9"},
    Equip_Item  {SHOES10, "Shoes 10"},
    Equip_Item  {SHOES11, "Shoes 11"},
    Equip_Item  {SHOES12, "Shoes 12"},
    Equip_Item  {SHOES13, "Shoes 13"},
    Equip_Item  {SHOES14, "Shoes 14"},
    Equip_Item  {SHOES15, "Shoes 15"},
    Equip_Item  {SHOES16, "Shoes 16"},
    Equip_Item  {SHOES17, "Shoes 17"},
    Equip_Item  {SHOES18, "Shoes 18"},
    Equip_Item  {SHOES19, "Shoes 19"},

    Equip_Item  {JEANS0,  "Jeans 0"},
    Equip_Item  {JEANS1,  "Jeans 1"},
    Equip_Item  {JEANS2,  "Jeans 2"},
    Equip_Item  {JEANS3,  "Jeans 3"},
    Equip_Item  {JEANS4,  "Jeans 4"},
    Equip_Item  {JEANS5,  "Jeans 5"},
    Equip_Item  {JEANS6,  "Jeans 6"},
    Equip_Item  {JEANS7,  "Jeans 7"},
    Equip_Item  {JEANS8,  "Jeans 8"},
    Equip_Item  {JEANS9,  "Jeans 9"},
    Equip_Item  {JEANS10, "Jeans 10"},
    Equip_Item  {JEANS11, "Jeans 11"},
    Equip_Item  {JEANS12, "Jeans 12"},
    Equip_Item  {JEANS13, "Jeans 13"},
    Equip_Item  {JEANS14, "Jeans 14"},
    Equip_Item  {JEANS15, "Jeans 15"},
    Equip_Item  {JEANS16, "Jeans 16"},
    Equip_Item  {JEANS17, "Jeans 17"},
    Equip_Item  {JEANS18, "Jeans 18"},
    Equip_Item  {JEANS19, "Jeans 19"},

    Equip_Item  {BELT0,  "Belt 0"},
    Equip_Item  {BELT1,  "Belt 1"},
    Equip_Item  {BELT2,  "Belt 2"},
    Equip_Item  {BELT3,  "Belt 3"},
    Equip_Item  {BELT4,  "Belt 4"},
    Equip_Item  {BELT5,  "Belt 5"},
    Equip_Item  {BELT6,  "Belt 6"},
    Equip_Item  {BELT7,  "Belt 7"},
    Equip_Item  {BELT8,  "Belt 8"},
    Equip_Item  {BELT9,  "Belt 9"},
    Equip_Item  {BELT10, "Belt 10"},
    Equip_Item  {BELT11, "Belt 11"},
    Equip_Item  {BELT12, "Belt 12"},
    Equip_Item  {BELT13, "Belt 13"},
    Equip_Item  {BELT14, "Belt 14"},
    Equip_Item  {BELT15, "Belt 15"},
    Equip_Item  {BELT16, "Belt 16"},
    Equip_Item  {BELT17, "Belt 17"},
    Equip_Item  {BELT18, "Belt 18"},
    Equip_Item  {BELT19, "Belt 19"},

    Equip_Item  {SHIRT0,  "Shirt 0"},
    Equip_Item  {SHIRT1,  "Shirt 1"},
    Equip_Item  {SHIRT2,  "Shirt 2"},
    Equip_Item  {SHIRT3,  "Shirt 3"},
    Equip_Item  {SHIRT4,  "Shirt 4"},
    Equip_Item  {SHIRT5,  "Shirt 5"},
    Equip_Item  {SHIRT6,  "Shirt 6"},
    Equip_Item  {SHIRT7,  "Shirt 7"},
    Equip_Item  {SHIRT8,  "Shirt 8"},
    Equip_Item  {SHIRT9,  "Shirt 9"},
    Equip_Item  {SHIRT10, "Shirt 10"},
    Equip_Item  {SHIRT11, "Shirt 11"},
    Equip_Item  {SHIRT12, "Shirt 12"},
    Equip_Item  {SHIRT13, "Shirt 13"},
    Equip_Item  {SHIRT14, "Shirt 14"},
    Equip_Item  {SHIRT15, "Shirt 15"},
    Equip_Item  {SHIRT16, "Shirt 16"},
    Equip_Item  {SHIRT17, "Shirt 17"},
    Equip_Item  {SHIRT18, "Shirt 18"},
    Equip_Item  {SHIRT19, "Shirt 19"},
};

struct ComboInfo {
    const char* label;
    size_t start_idx;
    size_t end_idx;
};

ComboInfo combo_boxes[] = {
    {"##GlassesCombo", 0, 20},
    {"##JacketCombo", 20, 40},
    {"##ShoesCombo", 40, 60},
    {"##JeansCombo", 60, 80},
    {"##BeltCombo", 80, 100},
    {"##ShirtCombo", 100, 120}
};

std::optional<std::string> ClothesSwitcher::on_initialize() {
    return Mod::on_initialize();
}

void ClothesSwitcher::on_draw_ui() {
    static int selected_indices[sizeof(combo_boxes) / sizeof(combo_boxes[0])] = {0};
    for (size_t i = 0; i < sizeof(combo_boxes) / sizeof(combo_boxes[0]); i++) {
        const auto& combo = combo_boxes[i];
        size_t selected_item_index = combo.start_idx + selected_indices[i];
        if (ImGui::BeginCombo(combo.label, clothing_items[selected_item_index].name)) {
            for (size_t n = combo.start_idx; n < combo.end_idx; n++) {
                const bool is_selected = (selected_indices[i] == n - combo.start_idx);
                if (ImGui::Selectable(clothing_items[n].name, is_selected)) {
                    selected_indices[i] = n - combo.start_idx;
                    nmh_sdk::SetEquip((pcItem)clothing_items[n].id, 0);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
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
