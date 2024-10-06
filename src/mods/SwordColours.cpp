#include "SwordColours.hpp"
#if 1

bool SwordColours::mod_enabled = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;
float SwordColours::coloursPickedFloat[5][4]{}; // rgba
uint8_t SwordColours::coloursPicked[5][4]{}; // abgr

// clang-format off
naked void detour1() { // custom colours
    __asm {
        //
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
        //
            push eax
            mov eax, [SwordColours::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164]
            test eax,eax
            je popcode

            // get sword no
            cmp [eax+0x42C], BLOOD_BERRY // berry
            je berryColour
            cmp [eax+0x42C], TSUBAKI_MK3 // mk3
            je mk3Colour
            cmp [eax+0x42C], TSUBAKI_MK1 // mk1
            je mk1Colour
            cmp [eax+0x42C], TSUBAKI_MK2 // mk2
            je mk2Colour
            jmp popcode // should be impossible

            berryColour:
            // filter out non players
            cmp esi, [eax+0x350]
            jne popcode
            mov edi, [SwordColours::coloursPicked]
            jmp popret

            mk3Colour:
            cmp esi, [eax+0x350]
            jne popcode
            mov edi, [SwordColours::coloursPicked+0x4]
            jmp popret

            mk1Colour:
            cmp esi, [eax+0x350]
            jne popcode
            mov edi, [SwordColours::coloursPicked+0x8]
            jmp popret

            mk2Colour:
            cmp esi, [eax+0x2aa0]
            je applyMk2Colour
            cmp esi, [eax+0x2aa4]
            je applyMk2Colour
            cmp esi, [eax+0x2aa8]
            je applyMk2Colour
            cmp esi, [eax+0x2aaC]
            je applyMk2Colour
            cmp esi, [eax+0x2ab0]
            je applyMk2Colour
            jmp popcode
            applyMk2Colour:
            mov edi, [SwordColours::coloursPicked+0xC]
            jmp popret

            mov edi, [SwordColours::coloursPicked]
            jmp retcode

        popret:
            pop eax
            jmp retcode

        popcode:
            pop eax
        originalcode:
            mov edi, [esi+0x000000A4]
        retcode:
            jmp dword ptr [SwordColours::jmp_ret1]
    }
}
 // clang-format on

std::optional<std::string> SwordColours::on_initialize() {
    SwordColours::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;

    if (!install_hook_offset(0x4C9AED, m_hook1, &detour1, &SwordColours::jmp_ret1, 6)) {
        spdlog::error("Failed to init SprintSettings mod\n");
        return "Failed to init SprintSettings mod";
    }

    return Mod::on_initialize();
}

const char* colorPickerNames[] = {
    "Blood Berry",
    "Tsubaki Mk3",
    "Tsubaki Mk1",
    "Tsubaki Mk2",
    "Other"
};

void SwordColours::on_draw_ui() {
    ImGui::Checkbox("Custom Colours", &mod_enabled);

    for (int i = 0; i < 5; ++i) {
        if (ImGui::ColorPicker4(colorPickerNames[i], coloursPickedFloat[i], ImGuiColorEditFlags_AlphaBar)) {
            // Convert rgba floats into abgr int8s
            coloursPicked[i][3] = (uint8_t)(coloursPickedFloat[i][0] * 255); // Red
            coloursPicked[i][2] = (uint8_t)(coloursPickedFloat[i][1] * 255); // Green
            coloursPicked[i][1] = (uint8_t)(coloursPickedFloat[i][2] * 255); // Blue
            coloursPicked[i][0] = (uint8_t)(coloursPickedFloat[i][3] * 255); // Alpha
        }
    }
}

// during load
void SwordColours::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("custom_colours").value_or(false);

    for (int i = 0; i < 5; ++i) {
        coloursPicked[i][0] = cfg.get<uint8_t>(fmt::format("colours_picked [{}][0]", i).c_str()).value_or(255); // A
        coloursPicked[i][1] = cfg.get<uint8_t>(fmt::format("colours_picked [{}][1]", i).c_str()).value_or(255); // B
        coloursPicked[i][2] = cfg.get<uint8_t>(fmt::format("colours_picked [{}][2]", i).c_str()).value_or(255); // G
        coloursPicked[i][3] = cfg.get<uint8_t>(fmt::format("colours_picked [{}][3]", i).c_str()).value_or(0); // R

        coloursPickedFloat[i][3] = (float)(coloursPicked[i][0] / 255.0f);
        coloursPickedFloat[i][2] = (float)(coloursPicked[i][1] / 255.0f);
        coloursPickedFloat[i][1] = (float)(coloursPicked[i][2] / 255.0f);
        coloursPickedFloat[i][0] = (float)(coloursPicked[i][3] / 255.0f);
    }
}

// during save
void SwordColours::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_colours", mod_enabled);

    for (int i = 0; i < 5; ++i) {
        cfg.set<uint8_t>(fmt::format("colours_picked [{}][0]", i).c_str(), coloursPicked[i][0]); // a
        cfg.set<uint8_t>(fmt::format("colours_picked [{}][1]", i).c_str(), coloursPicked[i][1]); // b
        cfg.set<uint8_t>(fmt::format("colours_picked [{}][2]", i).c_str(), coloursPicked[i][2]); // g
        cfg.set<uint8_t>(fmt::format("colours_picked [{}][3]", i).c_str(), coloursPicked[i][3]); // r
    }
}
//void SwordColours::on_frame() {}

// will show up in debug window, dump ImGui widgets you want here
//void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
