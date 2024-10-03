#include "ClothesSwitcher.hpp"
#if 1

struct Equip_Item {
    int id;
    const char* name;
};

static constexpr std::array<Equip_Item, 120> clothing_items = {
    Equip_Item  {100, "Glasses 0"},
    Equip_Item  {101, "Glasses 1"},
    Equip_Item  {102, "Glasses 2"},
    Equip_Item  {103, "Glasses 3"},
    Equip_Item  {104, "Glasses 4"},
    Equip_Item  {105, "Glasses 5"},
    Equip_Item  {106, "Glasses 6"},
    Equip_Item  {107, "Glasses 7"},
    Equip_Item  {108, "Glasses 8"},
    Equip_Item  {109, "Glasses 9"},
    Equip_Item  {110, "Glasses 10"},
    Equip_Item  {111, "Glasses 11"},
    Equip_Item  {112, "Glasses 12"},
    Equip_Item  {113, "Glasses 13"},
    Equip_Item  {114, "Glasses 14"},
    Equip_Item  {115, "Glasses 15"},
    Equip_Item  {116, "Glasses 16"},
    Equip_Item  {117, "Glasses 17"},
    Equip_Item  {118, "Glasses 18"},
    Equip_Item  {119, "Glasses 19"},

    Equip_Item  {200, "Jacket 0"},
    Equip_Item  {201, "Jacket 1"},
    Equip_Item  {202, "Jacket 2"},
    Equip_Item  {203, "Jacket 3"},
    Equip_Item  {204, "Jacket 4"},
    Equip_Item  {205, "Jacket 5"},
    Equip_Item  {206, "Jacket 6"},
    Equip_Item  {207, "Jacket 7"},
    Equip_Item  {208, "Jacket 8"},
    Equip_Item  {209, "Jacket 9"},
    Equip_Item  {210, "Jacket 10"},
    Equip_Item  {211, "Jacket 11"},
    Equip_Item  {212, "Jacket 12"},
    Equip_Item  {213, "Jacket 13"},
    Equip_Item  {214, "Jacket 14"},
    Equip_Item  {215, "Jacket 15"},
    Equip_Item  {216, "Jacket 16"},
    Equip_Item  {217, "Jacket 17"},
    Equip_Item  {218, "Jacket 18"},
    Equip_Item  {219, "Jacket 19"},

    Equip_Item  {300, "Shoes 0"},
    Equip_Item  {301, "Shoes 1"},
    Equip_Item  {302, "Shoes 2"},
    Equip_Item  {303, "Shoes 3"},
    Equip_Item  {304, "Shoes 4"},
    Equip_Item  {305, "Shoes 5"},
    Equip_Item  {306, "Shoes 6"},
    Equip_Item  {307, "Shoes 7"},
    Equip_Item  {308, "Shoes 8"},
    Equip_Item  {309, "Shoes 9"},
    Equip_Item  {310, "Shoes 10"},
    Equip_Item  {311, "Shoes 11"},
    Equip_Item  {312, "Shoes 12"},
    Equip_Item  {313, "Shoes 13"},
    Equip_Item  {314, "Shoes 14"},
    Equip_Item  {315, "Shoes 15"},
    Equip_Item  {316, "Shoes 16"},
    Equip_Item  {317, "Shoes 17"},
    Equip_Item  {318, "Shoes 18"},
    Equip_Item  {319, "Shoes 19"},

    Equip_Item  {400, "Jeans 0"},
    Equip_Item  {401, "Jeans 1"},
    Equip_Item  {402, "Jeans 2"},
    Equip_Item  {403, "Jeans 3"},
    Equip_Item  {404, "Jeans 4"},
    Equip_Item  {405, "Jeans 5"},
    Equip_Item  {406, "Jeans 6"},
    Equip_Item  {407, "Jeans 7"},
    Equip_Item  {408, "Jeans 8"},
    Equip_Item  {409, "Jeans 9"},
    Equip_Item  {410, "Jeans 10"},
    Equip_Item  {411, "Jeans 11"},
    Equip_Item  {412, "Jeans 12"},
    Equip_Item  {413, "Jeans 13"},
    Equip_Item  {414, "Jeans 14"},
    Equip_Item  {415, "Jeans 15"},
    Equip_Item  {416, "Jeans 16"},
    Equip_Item  {417, "Jeans 17"},
    Equip_Item  {418, "Jeans 18"},
    Equip_Item  {419, "Jeans 19"},

    Equip_Item  {500, "Belt 0"},
    Equip_Item  {501, "Belt 1"},
    Equip_Item  {502, "Belt 2"},
    Equip_Item  {503, "Belt 3"},
    Equip_Item  {504, "Belt 4"},
    Equip_Item  {505, "Belt 5"},
    Equip_Item  {506, "Belt 6"},
    Equip_Item  {507, "Belt 7"},
    Equip_Item  {508, "Belt 8"},
    Equip_Item  {509, "Belt 9"},
    Equip_Item  {510, "Belt 10"},
    Equip_Item  {511, "Belt 11"},
    Equip_Item  {512, "Belt 12"},
    Equip_Item  {513, "Belt 13"},
    Equip_Item  {514, "Belt 14"},
    Equip_Item  {515, "Belt 15"},
    Equip_Item  {516, "Belt 16"},
    Equip_Item  {517, "Belt 17"},
    Equip_Item  {518, "Belt 18"},
    Equip_Item  {519, "Belt 19"},

    Equip_Item  {600, "Shirt 0"},
    Equip_Item  {601, "Shirt 1"},
    Equip_Item  {602, "Shirt 2"},
    Equip_Item  {603, "Shirt 3"},
    Equip_Item  {604, "Shirt 4"},
    Equip_Item  {605, "Shirt 5"},
    Equip_Item  {606, "Shirt 6"},
    Equip_Item  {607, "Shirt 7"},
    Equip_Item  {608, "Shirt 8"},
    Equip_Item  {609, "Shirt 9"},
    Equip_Item  {610, "Shirt 10"},
    Equip_Item  {611, "Shirt 11"},
    Equip_Item  {612, "Shirt 12"},
    Equip_Item  {613, "Shirt 13"},
    Equip_Item  {614, "Shirt 14"},
    Equip_Item  {615, "Shirt 15"},
    Equip_Item  {616, "Shirt 16"},
    Equip_Item  {617, "Shirt 17"},
    Equip_Item  {618, "Shirt 18"},
    Equip_Item  {619, "Shirt 19"},
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
                    nmh_sdk::SetEquip(clothing_items[n].id, 0);
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
