#include "SwordColours.hpp"
#include "utility/Compressed.hpp"
#include "intrin.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/SwordsTextureAtlas.cpp"

bool SwordColours::mod_enabled = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;

uintptr_t SwordColours::jmp_ret2 = NULL;

uintptr_t SwordColours::jmp_ret3 = NULL;
int SwordColours::deathblowTimer = 0;
int SwordColours::setDeathblowTimer = 0;

struct IntColor {
    int r;
    int g;
    int b;
    int a;
};

static ImColor colours_picked_rgba[5];
static IntColor colours_picked_rgbaInt[5];

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

// clang-format off
naked void detour1() { // swords, player in ebx
    __asm {
        //
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
            
            // check this is player
            push eax
            mov eax, [SwordColours::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164]
            cmp eax, ebx
            pop eax
            jne originalcode

            cmp dword ptr [SwordColours::deathblowTimer], 100
            jbe deathblowColour
            jmp getSwordID

        getSwordID:
            cmp [ebx+0x42C], BLOOD_BERRY
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_BATTERY
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_DAMAGE
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_BATTERY_DAMAGE
            je berryColour

            cmp [ebx+0x42C], TSUBAKI_MK3
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_BATTERY
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_DAMAGE
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_BATTERY_DAMAGE
            je mk3Colour

            cmp [ebx+0x42C], TSUBAKI_MK1
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_BATTERY
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_DAMAGE
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_BATTERY_DAMAGE
            je mk1Colour

            cmp [ebx+0x42C], TSUBAKI_MK2
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_BATTERY
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_DAMAGE
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_BATTERY_DAMAGE
            je mk2Colour

            jmp originalcode

        berryColour: // [0]
            push eax // 4
            mov eax,[colours_picked_rgbaInt]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk3Colour: // [1]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x10]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x10+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x10+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk1Colour: // [2]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x20]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x20+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x20+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk2Colour: // [3]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x30]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x30+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x30+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        deathblowColour: // [4]
            cmp dword ptr [SwordColours::deathblowTimer], 0
            je getSwordID
            dec dword ptr [SwordColours::deathblowTimer]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x40]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x40+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x40+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        originalcode:
            push ebp
            mov ebp, esp
            mov edx, ecx
        retcode:
            jmp dword ptr [SwordColours::jmp_ret1]
    }
}

naked void detour2() { // trails, player in ebx
    __asm {
        //
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
            
            // check this is player
            push eax
            mov eax, [SwordColours::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164]
            cmp eax, ebx
            pop eax
            jne originalcode

            cmp dword ptr [SwordColours::deathblowTimer], 100
            jbe deathblowColour
            jmp getSwordID

        getSwordID:
            cmp [ebx+0x42C], BLOOD_BERRY
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_BATTERY
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_DAMAGE
            je berryColour
            cmp [ebx+0x42C], BLOOD_BERRY_BATTERY_DAMAGE
            je berryColour

            cmp [ebx+0x42C], TSUBAKI_MK3
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_BATTERY
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_DAMAGE
            je mk3Colour
            cmp [ebx+0x42C], TSUBAKI_MK3_BATTERY_DAMAGE
            je mk3Colour

            cmp [ebx+0x42C], TSUBAKI_MK1
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_BATTERY
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_DAMAGE
            je mk1Colour
            cmp [ebx+0x42C], TSUBAKI_MK1_BATTERY_DAMAGE
            je mk1Colour

            cmp [ebx+0x42C], TSUBAKI_MK2
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_BATTERY
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_DAMAGE
            je mk2Colour
            cmp [ebx+0x42C], TSUBAKI_MK2_BATTERY_DAMAGE
            je mk2Colour
            jmp originalcode

        berryColour: // [0]
            push eax // 4
            mov eax,[colours_picked_rgbaInt]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk3Colour: // [1]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x10]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x10+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x10+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk1Colour: // [2]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x20]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x20+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x20+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        mk2Colour: // [3]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x30]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x30+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x30+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        deathblowColour: // [4]
            cmp dword ptr [SwordColours::deathblowTimer], 0
            je getSwordID
            dec dword ptr [SwordColours::deathblowTimer]
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x40]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x40+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x40+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        originalcode:
            push ebp
            mov ebp,esp
            push ebx
            mov ebx,ecx
        retcode:
            jmp dword ptr [SwordColours::jmp_ret2]
    }
}

naked void detour3() { // set deathblow timer, player in ebx
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
            jmp dword ptr [SwordColours::jmp_ret3]
    }
}
 // clang-format on

bool load_texture() {

    auto [data, size] = utility::decompress_file_from_memory_with_size(swords_ta_compressed_data, swords_ta_compressed_size);
    if (!data) {
        return false;
    }

    g_sword_texture_atlas = std::make_unique<Texture2DD3D11>((const unsigned char*)data, size, g_framework->d3d11()->get_device());
    if (!g_sword_texture_atlas->IsValid()) {
        return false;
    }

    return true;
}

std::optional<std::string> SwordColours::on_initialize() {
    SwordColours::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584; 
    if (!install_hook_offset(0x3BF640, m_hook1, &detour1, &SwordColours::jmp_ret1, 5)) { // swords
        spdlog::error("Failed to init SwordColours mod\n");
        return "Failed to init SwordColours mod";
    }

    if (!install_hook_offset(0x4C7180, m_hook2, &detour2, &SwordColours::jmp_ret2, 6)) { // trails
        spdlog::error("Failed to init SwordColours mod\n");
        return "Failed to init SwordColours mod";
    }

    if (!install_hook_offset(0x3C6279, m_hook3, &detour3, &SwordColours::jmp_ret3, 6)) { // speedblur for deathblow timer
        spdlog::error("Failed to init SwordColours mod\n");
        return "Failed to init SwordColours mod";
    }

    load_texture();

    return Mod::on_initialize();
}

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

struct RgbaDefaults {
    const char* cfg_name;
    IntColor* color;
    IntColor default_value;
};

static constexpr std::array cfg_defaults = {
    RgbaDefaults { "colors_picked[0]", &colours_picked_rgbaInt[0], {0x64, 0x64, 0xFF, 0xFF}}, // BB
    RgbaDefaults { "colors_picked[1]", &colours_picked_rgbaInt[1], {0x64, 0xFF, 0x74, 0xFF}}, // MK3
    RgbaDefaults { "colors_picked[2]", &colours_picked_rgbaInt[2], {0x64, 0x64, 0xFF, 0xFF}}, // MK1
    RgbaDefaults { "colors_picked[3]", &colours_picked_rgbaInt[3], {0x64, 0x64, 0xFF, 0xFF}}, // MK2
    RgbaDefaults { "colors_picked[4]", &colours_picked_rgbaInt[4], {0xFF, 0x00, 0x00, 0xFF}}, // DB
};

static void draw_sword_behavior(const char* name, Frame blade, Frame hilt, ImColor& rgba, IntColor& rgbaInt, const RgbaDefaults& color_default) {

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
        rgbaInt = IntColor(
            (int)(rgba.Value.x * 255.0f),
            (int)(rgba.Value.y * 255.0f),
            (int)(rgba.Value.z * 255.0f),
            (int)(rgba.Value.w * 255.0f));
    }

    if (ImGui::Button(("Reset ##" + std::string(name)).c_str(), ImVec2(ImGui::CalcItemWidth(), NULL))) {
        rgba.Value = ImVec4(
            color_default.default_value.r / 255.0f,
            color_default.default_value.g / 255.0f,
            color_default.default_value.b / 255.0f,
            color_default.default_value.a / 255.0f
        );
        *color_default.color = color_default.default_value;
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
        ImGui::Indent();

        for (size_t i = 0; i < swords.size(); i++) {
            draw_sword_behavior(swords[i].name, swords[i].blade, swords[i].hilt, 
                colours_picked_rgba[i], colours_picked_rgbaInt[i], cfg_defaults[i]);
        }

        static int i = 0;
        int frame = ImGui::GetFrameCount();
        if (frame % 40 == 0) {
            i = (i + 1) % 4;
        }

        draw_sword_behavior("Deathblow", swords[i].blade, swords[i].hilt, colours_picked_rgba[4], colours_picked_rgbaInt[4], cfg_defaults[4]);
        ImGui::Text("Deathblow Timer");
        ImGui::InputInt("##DeathblowTimerInputInt", &setDeathblowTimer);
        help_marker("Turn the Beam Katana the color of your choice during a Death Blow."
            "A feature inspired by NMH3, this timer controls how long the colour will stay applied when initiating a Deathblow");
        ImGui::Unindent();
    }
}

void SwordColours::on_d3d_reset() {
    // explicitly call the destructor first
    g_sword_texture_atlas.reset();
    load_texture();
}

// during load
void SwordColours::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("custom_colours").value_or(false);
    {
        size_t i = 0;
        for (const auto& RgbaDefault : cfg_defaults) {
            RgbaDefault.color->r = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".r").value_or(RgbaDefault.default_value.r);
            RgbaDefault.color->g = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".g").value_or(RgbaDefault.default_value.g);
            RgbaDefault.color->b = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".b").value_or(RgbaDefault.default_value.b);
            RgbaDefault.color->a = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".a").value_or(RgbaDefault.default_value.a);
            colours_picked_rgba[i] = ImColor(RgbaDefault.color->r, RgbaDefault.color->g, RgbaDefault.color->b, RgbaDefault.color->a);
            i += 1;
        }
    }
    setDeathblowTimer = cfg.get<int>("setDeathblowTimer").value_or(50);
}

// during save
void SwordColours::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("custom_colours", mod_enabled);
    for (const auto& RgbaDefault : cfg_defaults) {
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".r", RgbaDefault.color->r);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".g", RgbaDefault.color->g);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".b", RgbaDefault.color->b);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".a", RgbaDefault.color->a);
    }
    cfg.set<int>("setDeathblowTimer", setDeathblowTimer);
}

//void SwordColours::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
