#include "SwordColours.hpp"
#if 1

bool SwordColours::mod_enabled = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;
float SwordColours::coloursPickedFloat[6][4]{}; // rgba
uint8_t SwordColours::coloursPicked[6][4]{}; // abgr

bool Check_Event(void) {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        if (nmh_sdk::CheckTsubazering(-1)) {
            return true;
        }
    }
    return false;
}

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

            // fucks eax, ecx, edx on release build
            push eax
            push ebx
            push ecx
            push edx
            push esi
            push edi
            call dword ptr [Check_Event]
            cmp al,1
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
            pop eax
            je specialColour

        // get sword no
            cmp [eax+0x42C], BLOOD_BERRY // berry
            je berryColour
            cmp [eax+0x42C], TSUBAKI_MK3 // mk3
            je mk3Colour
            cmp [eax+0x42C], TSUBAKI_MK1 // mk1
            je mk1Colour
            cmp [eax+0x42C], TSUBAKI_MK2 // mk2
            je mk2Colour
            jmp otherColour // should be impossible

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

        otherColour:
            mov edi, [SwordColours::coloursPicked+0x10]
            jmp popret

        specialColour:
            mov edi, [SwordColours::coloursPicked+0x14]
            jmp popret

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
    "Other",
    "Special",
};

void SwordColours::on_draw_ui() {
    ImGui::Checkbox("Custom Colours", &mod_enabled);

    for (int i = 0; i < 6; ++i) {
        if (ImGui::ColorPicker4(colorPickerNames[i], coloursPickedFloat[i])) {
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

    // berry
    coloursPicked[0][0] = cfg.get<uint8_t>("colours_picked [0][0]").value_or(0xFF); // A
    coloursPicked[0][1] = cfg.get<uint8_t>("colours_picked [0][1]").value_or(0x2A); // B
    coloursPicked[0][2] = cfg.get<uint8_t>("colours_picked [0][2]").value_or(0xFF); // G
    coloursPicked[0][3] = cfg.get<uint8_t>("colours_picked [0][3]").value_or(0x12); // R
    // mk3
    coloursPicked[1][0] = cfg.get<uint8_t>("colours_picked [1][0]").value_or(0x40); // A
    coloursPicked[1][1] = cfg.get<uint8_t>("colours_picked [1][1]").value_or(0xFF); // B
    coloursPicked[1][2] = cfg.get<uint8_t>("colours_picked [1][2]").value_or(0x64); // G
    coloursPicked[1][3] = cfg.get<uint8_t>("colours_picked [1][3]").value_or(0x64); // R
    // mk1
    coloursPicked[2][0] = cfg.get<uint8_t>("colours_picked [2][0]").value_or(0xFF); // A
    coloursPicked[2][1] = cfg.get<uint8_t>("colours_picked [2][1]").value_or(0xFF); // B
    coloursPicked[2][2] = cfg.get<uint8_t>("colours_picked [2][2]").value_or(0x55); // G
    coloursPicked[2][3] = cfg.get<uint8_t>("colours_picked [2][3]").value_or(0x00); // R
    // mk2
    coloursPicked[3][0] = cfg.get<uint8_t>("colours_picked [3][0]").value_or(0xFF); // A
    coloursPicked[3][1] = cfg.get<uint8_t>("colours_picked [3][1]").value_or(0xFF); // B
    coloursPicked[3][2] = cfg.get<uint8_t>("colours_picked [3][2]").value_or(0x55); // G
    coloursPicked[3][3] = cfg.get<uint8_t>("colours_picked [3][3]").value_or(0x00); // R
    // other
    coloursPicked[4][0] = cfg.get<uint8_t>("colours_picked [4][0]").value_or(0xFF); // A
    coloursPicked[4][1] = cfg.get<uint8_t>("colours_picked [4][1]").value_or(0xFF); // B
    coloursPicked[4][2] = cfg.get<uint8_t>("colours_picked [4][2]").value_or(0xFF); // G
    coloursPicked[4][3] = cfg.get<uint8_t>("colours_picked [4][3]").value_or(0xFF); // R
    // special
    coloursPicked[5][0] = cfg.get<uint8_t>("colours_picked [5][0]").value_or(0xFF); // A
    coloursPicked[5][1] = cfg.get<uint8_t>("colours_picked [5][1]").value_or(0x00); // B
    coloursPicked[5][2] = cfg.get<uint8_t>("colours_picked [5][2]").value_or(0x00); // G
    coloursPicked[5][3] = cfg.get<uint8_t>("colours_picked [5][3]").value_or(0xFF); // R

    for (int i = 0; i < 6; ++i) {
        coloursPickedFloat[i][3] = (float)(coloursPicked[i][0] / 255.0f);
        coloursPickedFloat[i][2] = (float)(coloursPicked[i][1] / 255.0f);
        coloursPickedFloat[i][1] = (float)(coloursPicked[i][2] / 255.0f);
        coloursPickedFloat[i][0] = (float)(coloursPicked[i][3] / 255.0f);
    }
}

// during save
void SwordColours::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_colours", mod_enabled);

    for (int i = 0; i < 6; ++i) {
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
