#include "SwordColours.hpp"
#include "fw-imgui/SwordsTextureAtlas.cpp"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "intrin.h"
#include "utility/Compressed.hpp"
#include <array> // for swords array
#include <cmath> // fmodf, floorf

// --- NEW: picker layout toggle (file-scope, no header change required)
static bool g_use_color_wheel = true;

// --- NEW: RGB cycle controls (file-scope, no header change required)
static bool g_rgb_cycle        = false;
static float g_rgb_cycle_speed = 1.00f; // cycles per second

// --- NEW: limit glow to specific attack motions
// Blood Berry: 223-247
// MK-III:      271-293
// MK-I:        319-341
// MK-II:       366-387
static bool g_glow_only_on_attack_mots = false;
static inline bool is_attack_motion_for_glow(int mot) {
    return (mot >= 223 && mot <= 247) || (mot >= 271 && mot <= 293) || (mot >= 319 && mot <= 341) || (mot >= 366 && mot <= 387);
}

// --------------------------------------------------------------------------------------
// Make sure IntColor and the existing color arrays are declared BEFORE any new references
// --------------------------------------------------------------------------------------
struct IntColor {
    int r;
    int g;
    int b;
    int a;
};

static ImColor colours_picked_rgba[5];
static IntColor colours_picked_rgbaInt[5];

static IntColor heart_colour_rgbaInt{};
static float lastHpScale    = 1.0f;
static IntColor frozenColor = {255, 255, 255, 255};

// --- NEW: non-destructive backup of user picks while RGB cycle is active
static bool g_rgb_saved_valid = false;
static ImColor g_saved_colours_rgba[5];
static IntColor g_saved_colours_rgbaInt[5];

// --- NEW: HSV->RGB helper (file-scope)
static inline void hsv_to_rgb(float h, float s, float v, float& r, float& g, float& b) {
    // h,s,v in [0,1]
    if (s <= 0.0f) {
        r = v;
        g = v;
        b = v;
        return;
    }
    h       = std::fmod(std::fmod(h, 1.0f) + 1.0f, 1.0f) * 6.0f;
    int i   = (int)std::floor(h);
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    switch (i % 6) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
    }
}

// --- NEW: update preview + detour sources from one RGB (temporary use)
static inline void set_all_colors_from_rgb_255(int R, int G, int B, int A = 255) {
    for (int i = 0; i < 5; ++i) {
        colours_picked_rgbaInt[i].r = R;
        colours_picked_rgbaInt[i].g = G;
        colours_picked_rgbaInt[i].b = B;
        colours_picked_rgbaInt[i].a = A;
        colours_picked_rgba[i]      = ImColor(R, G, B, A);
    }
}

// --- NEW: helpers to back up and restore user selections
static inline void backup_user_colors() {
    for (int i = 0; i < 5; ++i) {
        g_saved_colours_rgba[i]    = colours_picked_rgba[i];
        g_saved_colours_rgbaInt[i] = colours_picked_rgbaInt[i];
    }
    g_rgb_saved_valid = true;
}
static inline void restore_user_colors() {
    if (!g_rgb_saved_valid)
        return;
    for (int i = 0; i < 5; ++i) {
        colours_picked_rgba[i]    = g_saved_colours_rgba[i];
        colours_picked_rgbaInt[i] = g_saved_colours_rgbaInt[i];
    }
    g_rgb_saved_valid = false;
}

bool SwordColours::mod_enabled        = false;
bool SwordColours::sword_glow_enabled = false;
bool SwordColours::always_trail       = false;
bool SwordColours::heart_colours      = false;

bool SwordColours::always_sword_blur = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;

uintptr_t SwordColours::jmp_ret2 = NULL;

uintptr_t SwordColours::jmp_ret3    = NULL;
int SwordColours::deathblowTimer    = 0;
int SwordColours::setDeathblowTimer = 0;
float SwordColours::swordGlowAmount = 0.0f;

uintptr_t SwordColours::jmp_ret4               = NULL;
uintptr_t SwordColours::hrScreenStatus         = NULL;
bool SwordColours::force_girth                 = false;
float SwordColours::force_girth_amount         = 0.5f;
float SwordColours::girth_normalizer           = 0.5f;
bool SwordColours::custom_flicker              = false;
float SwordColours::flicker_amount             = 1.0f;
bool SwordColours::disable_girth_randomization = false;
bool SwordColours::heart_girth                 = false;
float SwordColours::base_heart_girth           = 0.5f;
float SwordColours::heartbeat_girth_amount     = 2.0f;
float SwordColours::heart_normalizer           = 1.0f;

const char* SwordColours::defaultDescription =
    "Customize your beam katana colors. You can also set a unique color specifically for Death Blows.";
const char* SwordColours::hoveredDescription = defaultDescription;

// directx stuff
static std::unique_ptr<Texture2DD3D11> g_sword_texture_atlas{};

#pragma region TextureAtlasDefinitions

struct Frame {
    ImVec2 pos, size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto Empty() {
        return Frame{ImVec2{555.0f, 650.0f}, ImVec2{2.0f, 2.0f}, ImVec2{555.0f / 1024.0f, 650.0f / 1024.0f},
            ImVec2{(555.0f + 2.0f) / 1024.0f, (650.0f + 2.0f) / 1024.0f}}; // Empty
    }
    static constexpr auto Berry_Blade() {
        return Frame{ImVec2{5.0f, 5.0f}, ImVec2{200.0f, 264.0f}, ImVec2{5.0f / 1024.0f, 5.0f / 1024.0f},
            ImVec2{(5.0f + 200.0f) / 1024.0f, (5.0f + 264.0f) / 1024.0f}}; // Berry_Blade
    };
    static constexpr auto Berry_Hilt() {
        return Frame{ImVec2{215.0f, 5.0f}, ImVec2{200.0f, 264.0f}, ImVec2{215.0f / 1024.0f, 5.0f / 1024.0f},
            ImVec2{(215.0f + 200.0f) / 1024.0f, (5.0f + 264.0f) / 1024.0f}}; // Berry_Hilt
    };
    static constexpr auto Tsubaki_1_Blade() {
        return Frame{ImVec2{5.0f, 279.0f}, ImVec2{200.0f, 264.0f}, ImVec2{5.0f / 1024.0f, 279.0f / 1024.0f},
            ImVec2{(5.0f + 200.0f) / 1024.0f, (279.0f + 264.0f) / 1024.0f}}; // Tsubaki_1_Blade
    };
    static constexpr auto Tsubaki_1_Hilt() {
        return Frame{ImVec2{215.0f, 279.0f}, ImVec2{200.0f, 264.0f}, ImVec2{215.0f / 1024.0f, 279.0f / 1024.0f},
            ImVec2{(215.0f + 200.0f) / 1024.0f, (279.0f + 264.0f) / 1024.0f}}; // Tsubaki_1_Hilt
    };
    static constexpr auto Tsubaki_2_Blade() {
        return Frame{ImVec2{425.0f, 5.0f}, ImVec2{200.0f, 264.0f}, ImVec2{425.0f / 1024.0f, 5.0f / 1024.0f},
            ImVec2{(425.0f + 200.0f) / 1024.0f, (5.0f + 264.0f) / 1024.0f}}; // Tsubaki_2_Blade
    };
    static constexpr auto Tsubaki_2_Hilt() {
        return Frame{ImVec2{425.0f, 279.0f}, ImVec2{200.0f, 264.0f}, ImVec2{425.0f / 1024.0f, 279.0f / 1024.0f},
            ImVec2{(425.0f + 200.0f) / 1024.0f, (279.0f + 264.0f) / 1024.0f}}; // Tsubaki_2_Hilt
    };
    static constexpr auto Tsubaki_3_Blade() {
        return Frame{ImVec2{5.0f, 553.0f}, ImVec2{200.0f, 264.0f}, ImVec2{5.0f / 1024.0f, 553.0f / 1024.0f},
            ImVec2{(5.0f + 200.0f) / 1024.0f, (553.0f + 264.0f) / 1024.0f}}; // Tsubaki_3_Blade
    };
    static constexpr auto Tsubaki_3_Hilt() {
        return Frame{ImVec2{215.0f, 553.0f}, ImVec2{200.0f, 264.0f}, ImVec2{215.0f / 1024.0f, 553.0f / 1024.0f},
            ImVec2{(215.0f + 200.0f) / 1024.0f, (553.0f + 264.0f) / 1024.0f}}; // Tsubaki_3_Hilt
    };

    static constexpr auto meta_size() { return ImVec2{1024.0f, 1024.0f}; };
};

static constexpr TextureAtlas g_atlas{};
#pragma endregion

/*
static constexpr float frameTime = 0.0125;
// ...
*/
// clang-format off
naked void detour1() { // swords, player in ebx
    __asm {
            push eax
            mov al, [SwordColours::heart_colours]
            or  al, [SwordColours::mod_enabled]
            cmp eax, 0
            pop eax
            je originalcode
        // check this is player
            push eax
            mov eax, [SwordColours::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164]
            cmp eax, ebx
            pop eax
            jne originalcode
            
            cmp byte ptr [SwordColours::heart_colours], 1
            je heartColours
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
            cmp dword ptr [SwordColours::deathblowTimer], 100
            jbe deathblowColour
            jmp getSwordID
        // CheckCharges:
        /*
            ...
        */

        heartColours:
            push eax // 4
            mov eax,[heart_colour_rgbaInt]
            mov [esp+0x4+0x4],eax
            mov eax,[heart_colour_rgbaInt+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[heart_colour_rgbaInt+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        getSwordID:
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_BATTERY
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_DAMAGE
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_BATTERY_DAMAGE
            je berryColour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_BATTERY
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_DAMAGE
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_BATTERY_DAMAGE
            je mk3Colour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_BATTERY
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_DAMAGE
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_BATTERY_DAMAGE
            je mk1Colour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_BATTERY
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_DAMAGE
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_BATTERY_DAMAGE
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
            push eax
            mov al, [SwordColours::sword_glow_enabled]
            or  al, [SwordColours::mod_enabled]
            cmp eax, 0
            pop eax
            je originalcode
        // check this is player
            push eax
            mov eax, [SwordColours::gpBattle]
            mov eax, [eax]
            mov eax, [eax+0x164]
            cmp eax, ebx
            pop eax
            jne originalcode

            cmp byte ptr [SwordColours::heart_colours], 1
            je heartColours
            cmp byte ptr [SwordColours::mod_enabled], 0
            je originalcode
            cmp dword ptr [SwordColours::deathblowTimer], 100
            jbe deathblowColour
            jmp getSwordID

        heartColours:
            push eax // 4
            mov eax,[heart_colour_rgbaInt]
            mov [esp+0x4+0x4],eax
            mov eax,[heart_colour_rgbaInt+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[heart_colour_rgbaInt+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        getSwordID:
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_BATTERY
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_DAMAGE
            je berryColour
            cmp dword ptr [ebx+0x42C], BLOOD_BERRY_BATTERY_DAMAGE
            je berryColour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_BATTERY
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_DAMAGE
            je mk3Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK3_BATTERY_DAMAGE
            je mk3Colour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_BATTERY
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_DAMAGE
            je mk1Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK1_BATTERY_DAMAGE
            je mk1Colour

            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_BATTERY
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_DAMAGE
            je mk2Colour
            cmp dword ptr [ebx+0x42C], TSUBAKI_MK2_BATTERY_DAMAGE
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

static float detour4_xmm1backup = 0.0f;
naked void detour4() { // girth randomization
    __asm {
        //
            cmp byte ptr [SwordColours::force_girth], 1
            je girthCode
            cmp byte ptr [SwordColours::custom_flicker], 1
            je flickerCode
            cmp byte ptr [SwordColours::heart_girth], 1
            je heartCode
            jmp originalcode

        girthCode:
            movss xmm0, [SwordColours::force_girth_amount]
            subss xmm0, [SwordColours::girth_normalizer]
            jmp originalcode

        flickerCode:
            mulss xmm0, [SwordColours::flicker_amount]
            jmp originalcode

        heartCode:
            push eax
            mov eax, [SwordColours::hrScreenStatus]
            mov eax, [eax]
            test eax,eax
            je popeaxcode
            movss [detour4_xmm1backup], xmm1
            movss xmm0, [SwordColours::base_heart_girth]
            subss xmm0, [SwordColours::girth_normalizer]
            movss xmm1, [eax+0x3a708]
            subss xmm1, [SwordColours::heart_normalizer]
            mulss xmm1, [SwordColours::heartbeat_girth_amount]
            addss xmm0, xmm1
            movss xmm1, [detour4_xmm1backup]
        popeaxcode:
            pop eax
        originalcode:
            movss [esi+0x000000A0],xmm0
        retcode:
            jmp dword ptr [SwordColours::jmp_ret4]
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

struct SwordDef {
    const char* name;
    Frame blade, hilt;
};

std::array swords = {
    SwordDef{"Blood Berry", g_atlas.Berry_Blade(), g_atlas.Berry_Hilt()},
    SwordDef{"Tsubaki Mk3", g_atlas.Tsubaki_3_Blade(), g_atlas.Tsubaki_3_Hilt()},
    SwordDef{"Tsubaki Mk1", g_atlas.Tsubaki_1_Blade(), g_atlas.Tsubaki_1_Hilt()},
    SwordDef{"Tsubaki Mk2", g_atlas.Tsubaki_2_Blade(), g_atlas.Tsubaki_2_Hilt()},
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
    RgbaDefaults{"colors_picked[0]", &colours_picked_rgbaInt[0], {0x64, 0x64, 0xFF, 0xFF}}, // BB
    RgbaDefaults{"colors_picked[1]", &colours_picked_rgbaInt[1], {0x64, 0xFF, 0x74, 0xFF}}, // MK3
    RgbaDefaults{"colors_picked[2]", &colours_picked_rgbaInt[2], {0x64, 0x64, 0xFF, 0xFF}}, // MK1
    RgbaDefaults{"colors_picked[3]", &colours_picked_rgbaInt[3], {0x64, 0x64, 0xFF, 0xFF}}, // MK2
    RgbaDefaults{"colors_picked[4]", &colours_picked_rgbaInt[4], {0xFF, 0x00, 0x00, 0xFF}}, // DB
};

static void draw_sword_behavior(
    const char* name, Frame blade, Frame hilt, ImColor& rgba, IntColor& rgbaInt, const RgbaDefaults& color_default) {

#if 0   
    ImVec2 spos = ImGui::GetCursorScreenPos();
    ImVec2 text_size = ImGui::CalcTextSize(name);
#endif
    ImGui::Text(name);

    const float inner_width = 150.0f;
    char buffer[MAX_PATH];
    sprintf(buffer, "%s_invisible_table", name);
    ImGui::BeginTable(buffer, 2, 0, ImVec2(-1.0f, 0.0f));

    // Add first row
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImVec2 pos = ImGui::GetCursorPos();
    auto& io   = ImGui::GetIO();
    // similar size for all textures so grab whatever
    ImGui::ImageWithBg((ImTextureID)g_sword_texture_atlas->GetTexture(),
        (ImVec2(blade.size.x * (io.DisplaySize.y / 1080.0f), blade.size.y * (io.DisplaySize.y / 1080.0f))), blade.uv0, blade.uv1,
        ImColor(0.0f, 0.0f, 0.0f, 0.0f), rgba);

    ImGui::SetCursorPos(pos);

    ImGui::ImageWithBg((ImTextureID)g_sword_texture_atlas->GetTexture(),
        (ImVec2(hilt.size.x * (io.DisplaySize.y / 1080.0f), hilt.size.y * (io.DisplaySize.y / 1080.0f))), hilt.uv0, hilt.uv1,
        ImColor(0.0f, 0.0f, 0.0f, 0.0f));

    // Add second row
    ImGui::TableSetColumnIndex(1);

    // --- CHANGED: picker layout selection ---
    ImGuiColorEditFlags pickerFlags = ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview |
                                      (g_use_color_wheel ? ImGuiColorEditFlags_PickerHueWheel : ImGuiColorEditFlags_PickerHueBar) |
                                      ImGuiColorEditFlags_AlphaBar;

    if (ImGui::ColorPicker4(name, (float*)&rgba.Value, pickerFlags)) {
        rgbaInt = IntColor(
            (int)(rgba.Value.x * 255.0f), (int)(rgba.Value.y * 255.0f), (int)(rgba.Value.z * 255.0f), (int)(rgba.Value.w * 255.0f));
    }

    if (ImGui::Button(("Reset ##" + std::string(name)).c_str(), ImVec2(ImGui::CalcItemWidth(), NULL))) {
        rgba.Value           = ImVec4(color_default.default_value.r / 255.0f, color_default.default_value.g / 255.0f,
                      color_default.default_value.b / 255.0f, color_default.default_value.a / 255.0f);
        *color_default.color = color_default.default_value;
    }

    ImGui::TableNextRow();

    ImGui::EndTable();
}

void SwordColours::on_frame() {
    // --- NEW: RGB cycle driver (non-destructive, like Heartbeat)
    if (g_rgb_cycle) {
        if (!g_rgb_saved_valid) {
            backup_user_colors();
        }

        const float t   = (float)ImGui::GetTime();
        const float hue = std::fmod(t * g_rgb_cycle_speed, 1.0f); // cycles per second
        float rf, gf, bf;
        hsv_to_rgb(hue, 1.0f, 1.0f, rf, gf, bf);
        const int R = (int)(rf * 255.0f);
        const int G = (int)(gf * 255.0f);
        const int B = (int)(bf * 255.0f);

        // Temporarily feed the detours the live color
        set_all_colors_from_rgb_255(R, G, B, 255);

        // Ensure detours use custom-colour path (heartbeat is exclusive)
        mod_enabled   = true;
        heart_colours = false;
    } else {
        if (g_rgb_saved_valid) {
            restore_user_colors();
        }
    }

    if (sword_glow_enabled) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            // Only apply glow if:
            //  - weapon effect is visible
            //  - AND either the checkbox is off, or the current motion is in the allowed list
            const int mot        = player->mCharaStatus.motionNo;
            const bool allow_now = !g_glow_only_on_attack_mots || is_attack_motion_for_glow(mot);
            if (allow_now && player->mCharaStatus.visibleWepEffect) {
                int currentSword    = player->mPcStatus.equip[0].id;
                uint32_t currentCol = nmh_sdk::GetLaserColor();
                nmh_sdk::SetLightReflect(player, swordGlowAmount, &player->mPcEffect.posHitSlash, currentCol, 0);
            }
        }
    }
    if (heart_colours) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (!player) {
            return;
        }
        mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
        if (!mHRBattle) {
            return;
        }
        HrScreenStatus* screenStatus = mHRBattle->mBtEffect.pScreenStatus;
        if (screenStatus) {
            static float colorPhase = 0.0f;
            if (screenStatus->m_HpScale > 1.0f) {
                float beatIntensity = (screenStatus->m_HpScale - 1.0f) / 0.2f;
                beatIntensity       = std::min(beatIntensity, 1.0f);

                if (lastHpScale <= 1.0f && screenStatus->m_HpScale > 1.0f) {
                    colorPhase += screenStatus->m_HpScale;
                }

                float r = sin(colorPhase * 1.3f) * 0.3f + 0.7f;
                float g = sin(colorPhase * 0.8f + 2.0f) * 0.3f + 0.5f;
                float b = sin(colorPhase * 1.1f + 4.0f) * 0.3f + 0.6f;

                r += beatIntensity * 0.2f;
                g += beatIntensity * 0.1f;
                b += beatIntensity * 0.1f;

                r = std::min(r, 1.0f);
                g = std::min(g, 1.0f);
                b = std::min(b, 1.0f);

                heart_colour_rgbaInt.r = (int)(r * 255.0f);
                heart_colour_rgbaInt.g = (int)(g * 255.0f);
                heart_colour_rgbaInt.b = (int)(b * 255.0f);
                heart_colour_rgbaInt.a = 255;

                frozenColor.r = heart_colour_rgbaInt.r;
                frozenColor.g = heart_colour_rgbaInt.g;
                frozenColor.b = heart_colour_rgbaInt.b;
                frozenColor.a = heart_colour_rgbaInt.a;

            } else if (screenStatus->m_HpScale == 1.0f) {
                heart_colour_rgbaInt.r = frozenColor.r;
                heart_colour_rgbaInt.g = frozenColor.g;
                heart_colour_rgbaInt.b = frozenColor.b;
                heart_colour_rgbaInt.a = frozenColor.a;
            }

            lastHpScale = screenStatus->m_HpScale;
        }
    }
}

void SwordColours::toggleForceTrail(bool enable) {
    if (enable) {
        install_patch_offset(0x3BF53A, patch_force_trail, "\x90\x90\x90", 3); //
    } else {
        install_patch_offset(0x3BF53A, patch_force_trail, "\x8A\x55\x08", 3); // mov dl,[ebp+08]
    }
}

void SwordColours::toggleForceSwordBlur(bool enable) {
    if (enable) {
        install_patch_offset(0x3C5C9A, patch_force_sword_blur, "\x6A\x01\x90\x90", 4); // push 01 nop 2
    } else {
        install_patch_offset(0x3C5C9A, patch_force_sword_blur, "\xFF\x74\x24\x20", 4); // push [esp+20]
    }
}

void SwordColours::render_description() const {
    ImGui::TextWrapped(SwordColours::hoveredDescription);
}

void SwordColours::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered())
        SwordColours::hoveredDescription = defaultDescription;
    float fontSize = ImGui::GetFontSize();

    ImGui::PushItemWidth(fontSize * 5.0f);

    ImGui::SeparatorText("Misc");

    ImGui::Checkbox("Glow", &sword_glow_enabled);
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Restores the beam reflections seen in early gameplay footage. This is the same glow that would "
                                           "later be used in NMH2 and Heroes Paradise";
    if (sword_glow_enabled) {
        ImGui::Indent();
        ImGui::SliderFloat("Glow Intensity", &swordGlowAmount, 1.0f, 5.0f, "%.0f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "Set how bright the glow from your sword is.";

        // NEW: gate glow by current motion ID ranges for attacks/executions
        ImGui::Checkbox("NMH2 Glow", &g_glow_only_on_attack_mots);
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "When enabled, glow applies only while attacking";

        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Always Display Laser Trails", &always_trail)) {
        toggleForceTrail(always_trail);
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Force the Beam Katana trails to always display.";

    if (ImGui::Checkbox("Always Display Laser Speed Blur", &always_sword_blur)) {
        toggleForceSwordBlur(always_sword_blur);
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Force the Beam Katana speed blur to always display.";

    ImGui::SeparatorText("Width");

    if (ImGui::Checkbox("Custom Flicker", &custom_flicker)) {
        force_girth = false;
        heart_girth = false;
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Set how much the beam width flickers.";

    if (custom_flicker) {
        ImGui::Indent();
        ImGui::SliderFloat("Flicker Amount", &flicker_amount, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "Set flicker amount.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Beam Flicker Default")) {
            flicker_amount = 1.0f;
        }
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Custom Beam Width", &force_girth)) {
        heart_girth    = false;
        custom_flicker = false;
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Set the width of sword lasers.";

    if (force_girth) {
        ImGui::Indent();
        ImGui::SliderFloat("Beam Width", &force_girth_amount, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "Set beam width.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Beam Width Default")) {
            force_girth_amount = 0.5f;
        }
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Heartbeat Beam Width", &heart_girth)) {
        force_girth    = false;
        custom_flicker = false;
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Use the heart beat to set the girth of the sword.";

    if (heart_girth) {
        ImGui::Indent();
        ImGui::SliderFloat("Base Width", &base_heart_girth, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "Base width before the heart beats.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Heart Base Default")) {
            base_heart_girth = 0.5f;
        }
        ImGui::SliderFloat("Beat Width", &heartbeat_girth_amount, -5.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "Set how much width the heartbeat adds.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Heart Beat Default")) {
            heartbeat_girth_amount = 2.0f;
        }
        ImGui::Unindent();
    }

    ImGui::PopItemWidth();

    ImGui::SeparatorText("Color");

    // --- NEW: layout toggle ---
    ImGui::Checkbox("Use Wheel Picker", &g_use_color_wheel);
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Switch between the Hue Wheel picker and the classic Hue Bar picker.";

    if (ImGui::Checkbox("Heartbeat Color Sync", &heart_colours)) {
        mod_enabled = false;
        g_rgb_cycle = false; // exclusive
        if (g_rgb_saved_valid) {
            restore_user_colors(); // safety: if switching while cycling
        }
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription =
            "Randomly cycles through beam colors and pulses with each color switch. This is synced to the HP's heart beat.";

    if (ImGui::Checkbox("Custom Colours", &mod_enabled)) {
        heart_colours = false;
        // RGB Cycle can still be toggled independently; it will force detours to use custom path while active
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = defaultDescription;

    // --- NEW: RGB Cycle UI (non-destructive)
    if (ImGui::Checkbox("RGB Cycle (Keyboard Mode)", &g_rgb_cycle)) {
        if (g_rgb_cycle) {
            heart_colours = false;
            if (!g_rgb_saved_valid)
                backup_user_colors();
            mod_enabled = true; // ensure detours use picked array path
        } else {
            restore_user_colors();
        }
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Smooth hue rotation. Your saved per-sword picks are preserved and restored when turned off.";

    if (g_rgb_cycle) {
        ImGui::Indent();
        ImGui::SliderFloat("Cycle Speed", &g_rgb_cycle_speed, 0.10f, 5.00f, "%.2f x");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "How many full hue cycles per second.";
        ImGui::Unindent();
    }

    if (mod_enabled) {
        ImGui::Indent();

        if (!g_rgb_cycle) {
            for (size_t i = 0; i < swords.size(); i++) {
                draw_sword_behavior(
                    swords[i].name, swords[i].blade, swords[i].hilt, colours_picked_rgba[i], colours_picked_rgbaInt[i], cfg_defaults[i]);
            }

            static int i = 0;
            int frame    = ImGui::GetFrameCount();
            if (frame % 40 == 0) {
                i = (i + 1) % 4;
            }

            draw_sword_behavior(
                "Deathblow", swords[i].blade, swords[i].hilt, colours_picked_rgba[4], colours_picked_rgbaInt[4], cfg_defaults[4]);
        } else {
        }

        ImGui::Text("Deathblow Timer");
        ImGui::InputInt("##DeathblowTimerInputInt", &setDeathblowTimer);
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription =
                "Turn the Beam Katana the color of your choice during a Death Blow."
                "A feature inspired by NMH3, this timer controls how long the colour will stay applied when initiating a Deathblow";
        ImGui::Unindent();
    }
}

void SwordColours::on_d3d_reset() {
    // explicitly call the destructor first
    g_sword_texture_atlas.reset();
    load_texture();
}

std::optional<std::string> SwordColours::on_initialize() {
    SwordColours::gpBattle       = g_framework->get_module().as<uintptr_t>() + 0x843584;
    SwordColours::hrScreenStatus = g_framework->get_module().as<uintptr_t>() + 0x8417F0;
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

    if (!install_hook_offset(0x4CAE36, m_hook4, &detour4, &SwordColours::jmp_ret4, 8)) { // girth randomization
        spdlog::error("Failed to init SwordColours mod\n");
        return "Failed to init SwordColours mod";
    }

    load_texture();

    return Mod::on_initialize();
}

// during load
void SwordColours::on_config_load(const utility::Config& cfg) {
    sword_glow_enabled = cfg.get<bool>("sword_glow_enabled").value_or(false);
    mod_enabled        = cfg.get<bool>("custom_colours").value_or(false);
    always_trail       = cfg.get<bool>("always_trail").value_or(false);
    if (always_trail)
        toggleForceTrail(always_trail);
    heart_colours     = cfg.get<bool>("heart_colours").value_or(false);
    always_sword_blur = cfg.get<bool>("always_sword_blur").value_or(false);
    if (always_sword_blur)
        toggleForceSwordBlur(always_sword_blur);
    {
        size_t i = 0;
        for (const auto& RgbaDefault : cfg_defaults) {
            RgbaDefault.color->r   = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".r").value_or(RgbaDefault.default_value.r);
            RgbaDefault.color->g   = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".g").value_or(RgbaDefault.default_value.g);
            RgbaDefault.color->b   = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".b").value_or(RgbaDefault.default_value.b);
            RgbaDefault.color->a   = cfg.get<int>(std::string(RgbaDefault.cfg_name) + ".a").value_or(RgbaDefault.default_value.a);
            colours_picked_rgba[i] = ImColor(RgbaDefault.color->r, RgbaDefault.color->g, RgbaDefault.color->b, RgbaDefault.color->a);
            i += 1;
        }
    }
    setDeathblowTimer = cfg.get<int>("setDeathblowTimer").value_or(50);

    swordGlowAmount = cfg.get<float>("swordGlowAmount").value_or(4.0f);

    // width stuff
    force_girth                 = cfg.get<bool>("force_girth").value_or(false);
    force_girth_amount          = cfg.get<float>("force_girth_amount").value_or(0.5f);
    custom_flicker              = cfg.get<bool>("custom_flicker").value_or(false);
    flicker_amount              = cfg.get<float>("flicker_amount").value_or(1.0f);
    disable_girth_randomization = cfg.get<bool>("disable_girth_randomization").value_or(false);
    heart_girth                 = cfg.get<bool>("heart_girth").value_or(false);
    base_heart_girth            = cfg.get<float>("base_heart_girth").value_or(0.5f);
    heartbeat_girth_amount      = cfg.get<float>("heartbeat_girth_amount").value_or(2.0f);

    // picker layout preference
    g_use_color_wheel = cfg.get<bool>("use_color_wheel").value_or(true);

    // RGB cycle prefs
    g_rgb_cycle       = cfg.get<bool>("rgb_cycle").value_or(false);
    g_rgb_cycle_speed = cfg.get<float>("rgb_cycle_speed").value_or(1.0f);

    // NEW: glow gating preference
    g_glow_only_on_attack_mots = cfg.get<bool>("glow_only_on_attack_mots").value_or(false);

    // If starting with RGB cycle on, back up the loaded picks once
    if (g_rgb_cycle && !g_rgb_saved_valid) {
        backup_user_colors();
        mod_enabled   = true;
        heart_colours = false;
    }
}

// during save
void SwordColours::on_config_save(utility::Config& cfg) {
    cfg.set<bool>("sword_glow_enabled", sword_glow_enabled);
    cfg.set<bool>("custom_colours", mod_enabled);
    cfg.set<bool>("always_trail", always_trail);
    cfg.set<bool>("heart_colours", heart_colours);
    cfg.set<bool>("always_sword_blur", always_sword_blur);

    // Save user's original picks if RGB Cycle is active
    if (g_rgb_cycle && g_rgb_saved_valid) {
        for (size_t i = 0; i < cfg_defaults.size(); ++i) {
            const char* key     = cfg_defaults[i].cfg_name;
            const IntColor& src = g_saved_colours_rgbaInt[i];
            cfg.set<int>(std::string(key) + ".r", src.r);
            cfg.set<int>(std::string(key) + ".g", src.g);
            cfg.set<int>(std::string(key) + ".b", src.b);
            cfg.set<int>(std::string(key) + ".a", src.a);
        }
    } else {
        for (const auto& RgbaDefault : cfg_defaults) {
            cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".r", RgbaDefault.color->r);
            cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".g", RgbaDefault.color->g);
            cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".b", RgbaDefault.color->b);
            cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".a", RgbaDefault.color->a);
        }
    }
    cfg.set<int>("setDeathblowTimer", setDeathblowTimer);
    cfg.set<float>("swordGlowAmount", swordGlowAmount);

    // width stuff
    cfg.set<bool>("force_girth", force_girth);
    cfg.set<float>("force_girth_amount", force_girth_amount);
    cfg.set<bool>("custom_flicker", custom_flicker);
    cfg.set<float>("flicker_amount", flicker_amount);
    cfg.set<bool>("disable_girth_randomization", disable_girth_randomization);
    cfg.set<bool>("heart_girth", heart_girth);
    cfg.set<float>("base_heart_girth", base_heart_girth);
    cfg.set<float>("heartbeat_girth_amount", heartbeat_girth_amount);

    // picker layout preference
    cfg.set<bool>("use_color_wheel", g_use_color_wheel);

    // RGB cycle prefs
    cfg.set<bool>("rgb_cycle", g_rgb_cycle);
    cfg.set<float>("rgb_cycle_speed", g_rgb_cycle_speed);

    // NEW: glow gating preference
    cfg.set<bool>("glow_only_on_attack_mots", g_glow_only_on_attack_mots);
}

// will show up in debug window, dump ImGui widgets you want here
// void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
