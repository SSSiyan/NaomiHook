#include "SwordColours.hpp"
#include "utility/Compressed.hpp"
#include "intrin.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/SwordsTextureAtlas.cpp"

// directx stuff
static std::unique_ptr<Texture2DD3D11> g_sword_texture_atlas{};

#pragma region TextureAtlasDefinitions

struct Frame {
    ImVec2 pos,size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto Empty() {
        return Frame{
            ImVec2 { 555.0f, 650.0f },  
            ImVec2 { 2.0f, 2.0f },
            ImVec2 { 555.0f / 1024.0f, 650.0f / 1024.0f },
            ImVec2 { ( 555.0f + 2.0f ) / 1024.0f, ( 650.0f + 2.0f ) / 1024.0f }
        }; // Empty
    }
    static constexpr auto Berry_Blade() {
        return Frame {
            ImVec2 { 5.0f, 5.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 5.0f / 1024.0f, 5.0f / 1024.0f },
            ImVec2 { ( 5.0f + 200.0f ) / 1024.0f, ( 5.0f + 264.0f ) / 1024.0f }
        }; //Berry_Blade
    };
    static constexpr auto Berry_Hilt() {
        return Frame {
            ImVec2 { 215.0f, 5.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 215.0f / 1024.0f, 5.0f / 1024.0f },
            ImVec2 { ( 215.0f + 200.0f ) / 1024.0f, ( 5.0f + 264.0f ) / 1024.0f }
        }; //Berry_Hilt
    };
    static constexpr auto Tsubaki_1_Blade() {
        return Frame {
            ImVec2 { 5.0f, 279.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 5.0f / 1024.0f, 279.0f / 1024.0f },
            ImVec2 { ( 5.0f + 200.0f ) / 1024.0f, ( 279.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_1_Blade
    };
    static constexpr auto Tsubaki_1_Hilt() {
        return Frame {
            ImVec2 { 215.0f, 279.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 215.0f / 1024.0f, 279.0f / 1024.0f },
            ImVec2 { ( 215.0f + 200.0f ) / 1024.0f, ( 279.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_1_Hilt
    };
    static constexpr auto Tsubaki_2_Blade() {
        return Frame {
            ImVec2 { 425.0f, 5.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 425.0f / 1024.0f, 5.0f / 1024.0f },
            ImVec2 { ( 425.0f + 200.0f ) / 1024.0f, ( 5.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_2_Blade
    };
    static constexpr auto Tsubaki_2_Hilt() {
        return Frame {
            ImVec2 { 425.0f, 279.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 425.0f / 1024.0f, 279.0f / 1024.0f },
            ImVec2 { ( 425.0f + 200.0f ) / 1024.0f, ( 279.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_2_Hilt
    };
    static constexpr auto Tsubaki_3_Blade() {
        return Frame {
            ImVec2 { 5.0f, 553.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 5.0f / 1024.0f, 553.0f / 1024.0f },
            ImVec2 { ( 5.0f + 200.0f ) / 1024.0f, ( 553.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_3_Blade
    };
    static constexpr auto Tsubaki_3_Hilt() {
        return Frame {
            ImVec2 { 215.0f, 553.0f },  
            ImVec2 { 200.0f, 264.0f },
            ImVec2 { 215.0f / 1024.0f, 553.0f / 1024.0f },
            ImVec2 { ( 215.0f + 200.0f ) / 1024.0f, ( 553.0f + 264.0f ) / 1024.0f }
        }; //Tsubaki_3_Hilt
    };

    static constexpr auto meta_size() {
        return ImVec2{ 1024.0f, 1024.0f  };
    };
};

static constexpr TextureAtlas g_atlas{};
#pragma endregion


bool SwordColours::mod_enabled = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;

#if 0
float SwordColours::coloursPickedFloat[5][4]{}; // rgba
uint8_t SwordColours::coloursPicked[5][4]{}; // abgr
#endif

static ImColor colours_picked_rgba[5];
static ImU32   colours_picked_abgr[5];



uintptr_t SwordColours::jmp_ret2 = NULL;
int SwordColours::deathblowTimer = 0;
int SwordColours::setDeathblowTimer = 0;

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

            cmp dword ptr [SwordColours::deathblowTimer], 100
            jbe deathblowColour
            jmp getSwordID

        deathblowColour:
            cmp dword ptr [SwordColours::deathblowTimer], 0
            je getSwordID
            dec dword ptr [SwordColours::deathblowTimer]
            mov edi, [colours_picked_abgr+0x10]
            jmp popret

        getSwordID:
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
            mov edi, [colours_picked_abgr]
            jmp popret

        mk3Colour:
            cmp esi, [eax+0x350]
            jne popcode
            mov edi, [colours_picked_abgr+0x4]
            jmp popret

        mk1Colour:
            cmp esi, [eax+0x350]
            jne popcode
            mov edi, [colours_picked_abgr+0x8]
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
            mov edi, [colours_picked_abgr+0xC]
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

naked void detour2() { // set deathblow timer
    __asm {
        //
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
            cmp dword ptr [ebx+0x198], eGood // condition // 0 = not taking damage
            jne originalcode

        // filter out non deathblows
            cmp dword ptr [ebx+0x18c], ePcMtBtChgLp // 168 low charge p2
            je originalcode
            cmp dword ptr [ebx+0x18c], ePcMtBtAtkChgUp // 170 high charge
            je originalcode
            cmp dword ptr [ebx+0x18c], ePcMtBtChgWlkF // 37 Walk Charge Forward
            je originalcode
            cmp dword ptr [ebx+0x18c], ePcMtBtChgWlkB // 38 Walk Charge Back
            je originalcode
            cmp dword ptr [ebx+0x18c], ePcMtBtChgWlkL // 39 Walk Charge Left
            je originalcode
            cmp dword ptr [ebx+0x18c], ePcMtBtChgWlkR // 40 Walk Charge Right
            je originalcode
            // enum pcMotion {
            // ePcMtBtChgStrt = 167,
            // ePcMtBtChgLp = 168,
            // ePcMtBtAtkChg = 169,
            // ePcMtBtAtkChgUp = 170,

        // set deathblow timer
            push eax
            mov eax, [SwordColours::setDeathblowTimer]
            mov [SwordColours::deathblowTimer], eax
            pop eax
        originalcode:
            push [ebx+0x000029D4]
        retcode:
            jmp dword ptr [SwordColours::jmp_ret2]
    }
}
 // clang-format on

std::optional<std::string> SwordColours::on_initialize() {
    SwordColours::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584; 
    if (!install_hook_offset(0x4C9AED, m_hook1, &detour1, &SwordColours::jmp_ret1, 6)) {
        spdlog::error("Failed to init SprintSettings mod\n");
        return "Failed to init SprintSettings mod";
    }

    if (!install_hook_offset(0x3C6279, m_hook2, &detour2, &SwordColours::jmp_ret2, 6)) { // speedblur for deathblows
        spdlog::error("Failed to init SprintSettings mod\n");
        return "Failed to init SprintSettings mod";
    }
    auto [data, size] = utility::decompress_file_from_memory_with_size(swords_ta_compressed_data, swords_ta_compressed_size);
    if (!data) {
        return "Failed to load sword texture atlas";
    }

    g_sword_texture_atlas = std::make_unique<Texture2DD3D11>((const unsigned char*)data, size, g_framework->d3d11()->get_device());
    if (!g_sword_texture_atlas->IsValid()) {
        return "Failed to create sword texture atlas";
    }

    return Mod::on_initialize();
}

#if 0
const char* colorPickerNames[] = {
    "Blood Berry",
    "Tsubaki Mk3",
    "Tsubaki Mk1",
    "Tsubaki Mk2",
    "Deathblows",
};
#endif


struct SwordDef {
    const char* name;
    Frame blade,hilt;
};

std::array swords = {
    SwordDef { "Blood Berry", g_atlas.Berry_Blade(),     g_atlas.Berry_Hilt()     },
    SwordDef { "Tsubaki Mk3", g_atlas.Tsubaki_3_Blade(), g_atlas.Tsubaki_3_Hilt() },
    SwordDef { "Tsubaki Mk1", g_atlas.Tsubaki_1_Blade(), g_atlas.Tsubaki_1_Hilt() },
    SwordDef { "Tsubaki Mk2", g_atlas.Tsubaki_2_Blade(), g_atlas.Tsubaki_2_Hilt() },
};

#if 0
static float sin_pulse(float frequency) {
    const float pi = 3.14f;
    return 0.5f * (1 + (float)std::sin(2 * pi * frequency * ImGui::GetTime()));
}
#endif

static void draw_sword_behavior(const char* name, Frame blade, Frame hilt, ImColor& rgba, ImU32& abgr) {

#if 0
    ImVec2 spos = ImGui::GetCursorScreenPos();
    ImVec2 text_size = ImGui::CalcTextSize(name);
#endif
    ImGui::Text(name);

    const float inner_width = 150.0f;
    char buffer[MAX_PATH];
    sprintf(buffer, "%s_invisible_table", name);
    ImGui::BeginTable(buffer, 2, /*ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoBordersInBody*/0, ImVec2(-1.0f, 0.0f));

    // Add first row
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImVec2 pos = ImGui::GetCursorPos();

    // similar size for all textures so grab whatever
    ImGui::Image(g_sword_texture_atlas->GetTexture(), 
        blade.size, blade.uv0, blade.uv1, rgba);

    ImGui::SetCursorPos(pos);

    ImGui::Image(g_sword_texture_atlas->GetTexture(), 
        hilt.size, hilt.uv0, hilt.uv1);

    // Add second row
    ImGui::TableSetColumnIndex(1);

    if (ImGui::ColorPicker4(name, (float*)&rgba.Value, 
        ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)) {
        abgr = _byteswap_ulong(rgba);
    }

    ImGui::TableNextRow();

    ImGui::EndTable();

#if 0
    ImColor col{};
    if(ImGui::IsItemHovered()) {
        col = ImColor::HSV(sin_pulse(0.2), 1.0f, 1.0f, 1.0f);
    }
    else {
        col = ImColor::ImColor(0xffbfe6ff);
    }

    pos = ImGui::GetCursorPos();
    auto dl = ImGui::GetForegroundDrawList();
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();

    ImGui::SetCursorScreenPos(ImVec2((p0.x + p1.x) / 2.0f - (text_size.x / 2.0f), p0.y - ImGui::GetTextLineHeight()));
    ImGui::TextColored(col, name);
    ImGui::SetCursorPos(pos);
    ImGui::Dummy(text_size);
#endif

#if 0
    dl->AddText(
        ImVec2((p0.x + p1.x) / 2.0f - (text_size.x / 2.0f), p0.y - ImGui::GetTextLineHeight()),
        0xffbfe6ff, name);

    dl->AddRectFilled(
        ImVec2(p0.x + (textsize.x * 2.0f), p1.y - textsize.y),
        ImVec2(p1.x - (textsize.x * 2.0f), p0.y),
        IM_COL32(48, 48, 48, 222));
#endif

}
void SwordColours::on_draw_ui() {
    ImGui::Checkbox("Custom Colours", &mod_enabled);
    if (mod_enabled) {
        ImGui::InputInt("Deathblow Timer", &setDeathblowTimer);
        help_marker("How long the deathblow colour will stay applied when initiating a Deathblow");
        for (size_t i = 0; i < swords.size(); i++) {
            draw_sword_behavior(swords[i].name, swords[i].blade, swords[i].hilt, colours_picked_rgba[i], colours_picked_abgr[i]);
        }

        static int i = 0;
        int frame = ImGui::GetFrameCount();
        if (frame % 40 == 0) {
            i = (i + 1) % 4;
        }
        draw_sword_behavior("Deathblow", swords[i].blade, swords[i].hilt, colours_picked_rgba[4], colours_picked_abgr[4]);
    }
}

struct ArgbDefaults {
    const char* cfg_name;
    ImU32* col;
    ImU32 default_value;
};

static constexpr std::array cfg_defaults = {
    ArgbDefaults { "colors_picked[0]", &colours_picked_abgr[0], /*0xFF2AFF12*/ 0x12FF2AFF},
    ArgbDefaults { "colors_picked[1]", &colours_picked_abgr[1], /*0xFFFF6464*/ 0x6464FFFF},
    ArgbDefaults { "colors_picked[2]", &colours_picked_abgr[2], /*0xFFFF5500*/ 0x0055FFFF},
    ArgbDefaults { "colors_picked[3]", &colours_picked_abgr[3], /*0xFFFF5500*/ 0x0055FFFF},
    ArgbDefaults { "colors_picked[4]", &colours_picked_abgr[4], 0xFF0000FF},
};

// during load
void SwordColours::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("custom_colours").value_or(false);
    {
        size_t i = 0;
        for (const auto& def : cfg_defaults) {
            *def.col = cfg.get<ImU32>(def.cfg_name).value_or(def.default_value);
            colours_picked_rgba[i] = ImColor(_byteswap_ulong(*def.col));
            i = i+1;
        }
    }
    setDeathblowTimer = cfg.get<int>("setDeathblowTimer").value_or(50);
}

// during save
void SwordColours::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_colours", mod_enabled);
    for (const auto& def : cfg_defaults) {
        cfg.set<ImU32>(def.cfg_name, *def.col);
    }
    cfg.set<int>("setDeathblowTimer", setDeathblowTimer);
}
//void SwordColours::on_frame() {}

// will show up in debug window, dump ImGui widgets you want here
//void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
