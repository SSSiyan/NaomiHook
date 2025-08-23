#include "SwordColours.hpp"
#include "fw-imgui/SwordsTextureAtlas.cpp"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "intrin.h"
#include "utility/Compressed.hpp"
#include <array> // for swords array
#include <cmath> // fmodf, floorf, exp

// --- picker layout toggle
static bool g_use_color_wheel = true;

// --- Caramelldansen cycle controls
static bool g_rgb_cycle        = false;
static float g_rgb_cycle_speed = 1.00f; // cycles per second

// --- limit glow to specific attack motions
// Blood Berry: 223-247
// MK-III:      271-293
// MK-I:        319-341
// MK-II:       366-387
// Extra guarded / charge / battery anims: 41-101
static bool g_glow_only_on_attack_mots = false;
static inline bool is_attack_motion_for_glow(int mot) {
    // existing attack ranges
    if ((mot >= 223 && mot <= 247) || (mot >= 271 && mot <= 293) || (mot >= 319 && mot <= 341) || (mot >= 366 && mot <= 387)) {
        return true;
    }

    // new glow-eligible anims (briefly apply glow)
    switch (mot) {
    case 41:  // ePcMtTbLpGd
    case 42:  // ePcMtTbLp
    case 43:  // ePcMtTbLpBd
    case 44:  // ePcMtBtryChrgSt
    case 45:  // ePcMtBtryChrgLp
    case 46:  // ePcMtBtryChrgEnd
    case 47:  // ePcMtBtryChrgQkLp
    case 50:  // ePcMtGrdDfltDmg
    case 51:  // ePcMtGrdDflt2RU
    case 52:  // ePcMtGrdDflt2R
    case 53:  // ePcMtGrdDflt2RD
    case 54:  // ePcMtGrdDflt2LU
    case 55:  // ePcMtGrdDflt2L
    case 56:  // ePcMtGrdDflt2LD
    case 57:  // ePcMtGrdRULp
    case 58:  // ePcMtGrdRUDmg
    case 59:  // ePcMtGrdRU2R
    case 60:  // ePcMtGrdRU2RD
    case 61:  // ePcMtGrdRU2LU
    case 62:  // ePcMtGrdRU2L
    case 63:  // ePcMtGrdRU2LD
    case 64:  // ePcMtGrdRLp
    case 65:  // ePcMtGrdRDmg
    case 66:  // ePcMtGrdR2RU
    case 67:  // ePcMtGrdR2RD
    case 68:  // ePcMtGrdR2LU
    case 69:  // ePcMtGrdR2L
    case 70:  // ePcMtGrdR2LD
    case 71:  // ePcMtGrdRDLp
    case 72:  // ePcMtGrdRDDmg
    case 73:  // ePcMtGrdRD2RU
    case 74:  // ePcMtGrdRD2R
    case 75:  // ePcMtGrdRD2LU
    case 76:  // ePcMtGrdRD2L
    case 77:  // ePcMtGrdRD2LD
    case 78:  // ePcMtGrdLULp
    case 79:  // ePcMtGrdLUDmg
    case 80:  // ePcMtGrdLU2RU
    case 81:  // ePcMtGrdLU2R
    case 82:  // ePcMtGrdLU2RD
    case 83:  // ePcMtGrdLU2L
    case 84:  // ePcMtGrdLU2LD
    case 85:  // ePcMtGrdLLp
    case 86:  // ePcMtGrdLDmg
    case 87:  // ePcMtGrdL2RU
    case 88:  // ePcMtGrdL2R
    case 89:  // ePcMtGrdL2RD
    case 90:  // ePcMtGrdL2LU
    case 91:  // ePcMtGrdL2LD
    case 92:  // ePcMtGrdLDLp
    case 93:  // ePcMtGrdLDDmg
    case 94:  // ePcMtGrdLD2RU
    case 95:  // ePcMtGrdLD2R
    case 96:  // ePcMtGrdLD2RD
    case 97:  // ePcMtGrdLD2LU
    case 98:  // ePcMtGrdLD2L
    case 99:  // ePcMtGrdRngR
    case 100: // ePcMtGrdRngL
    case 101: // ePcMtGrdRngBck
        return true;
    default:
        return false;
    }
}

// --- smooth glow in/out (ms controls + current value)
static float g_glow_ease_in_ms  = 120.0f; // 0 = instant
static float g_glow_ease_out_ms = 160.0f; // 0 = instant
static float g_glow_current     = 0.0f;

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

// --- non-destructive backup of user picks while Caramelldansen is active
static bool g_rgb_saved_valid = false;
static ImColor g_saved_colours_rgba[5];
static IntColor g_saved_colours_rgbaInt[5];

// --- HSV->RGB helper
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

// --- update preview + detour sources from one RGB (temporary use)
static inline void set_all_colors_from_rgb_255(int R, int G, int B, int A = 255) {
    for (int i = 0; i < 5; ++i) {
        colours_picked_rgbaInt[i].r = R;
        colours_picked_rgbaInt[i].g = G;
        colours_picked_rgbaInt[i].b = B;
        colours_picked_rgbaInt[i].a = A;
        colours_picked_rgba[i]      = ImColor(R, G, B, A);
    }
}

// --- helpers to back up and restore user selections
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

const char* SwordColours::defaultDescription = "Customize your Beam Katana with selectable colors, adjustable width, heartbeat-synced "
                                               "pulsing, restored light reflections or even persisting trails.";
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

// ============================================================================
// Per-motion mapping for combo motions (Berry, MK1, MK2, MK3)
// Used ONLY for Per-Motion Flicker multiplier
// ============================================================================

static bool  g_per_motion_flicker    = false; // UI toggle (flicker multiplier)
static bool  g_per_motion_active     = false; // set true when current motion matches
static float g_per_motion_value      = 0.5f;  // value from tables
static float g_pm_flicker_current    = 1.0f;  // smoothed flicker multiplier (runtime)

struct MotWidthEntry {
    int mot;
    const char* label;
    float width; // reused as flicker multiplier too (0..5)
};

// ---- Blood Berry ----
static MotWidthEntry g_bb_widths[] = {
    // Up
    {223, "UpA",  0.8f},
    {224, "UpB",  0.9f},
    {225, "UpC",  1.0f},
    {226, "UpD",  1.1f},

    // Mid
    {228, "MidA", 0.9f},
    {229, "MidB", 1.0f},
    {230, "MidC", 1.2f},
    {231, "MidD", 1.1f},
    {232, "MidE", 1.0f},
    {233, "MidF", 1.0f},
    {234, "MidG", 0.9f},
    {235, "MidH", 1.4f},

    // Bottom
    {237, "BtmA", 1.2f},
    {238, "BtmB", 1.0f},
    {239, "BtmC", 1.2f},
    {240, "BtmD", 1.0f},
    {241, "BtmE", 1.0f},
    {242, "BtmF", 0.9f},
    {243, "BtmG", 1.3f},
};
static constexpr int g_bb_widths_count = (int)(sizeof(g_bb_widths)/sizeof(g_bb_widths[0]));

// ---- MK1 ----
static MotWidthEntry g_mk1_widths[] = {
    {319, "UpA",  1.0f},
    {320, "UpB",  1.0f},
    {321, "UpC",  1.0f},
    {322, "UpD",  1.3f},
    {323, "UpE",  1.0f},

    {325, "MidA", 0.5f},
    {326, "MidB", 0.5f},
    {327, "MidC", 1.1f},
    {328, "MidD", 0.5f},
    {329, "MidE", 1.2f},
    {330, "MidF", 1.0f},
    {331, "MidG", 1.0f},
    {332, "MidH", 1.3f},

    {334, "BtmA", 1.2f},
    {335, "BtmB", 1.0f},
    {336, "BtmC", 1.1f},
    {337, "BtmD", 1.3f},
};
static constexpr int g_mk1_widths_count = (int)(sizeof(g_mk1_widths)/sizeof(g_mk1_widths[0]));

// ---- MK2 ----
static MotWidthEntry g_mk2_widths[] = {
    // Up
    {366, "UpA",  1.0f},
    {367, "UpB",  1.0f},
    {368, "UpC",  1.1f},
    {369, "UpD",  1.3f},

    // Mid
    {371, "MidA", 1.0f},
    {372, "MidB", 1.1f},
    {373, "MidC", 1.0f},
    {374, "MidD", 1.0f},
    {375, "MidE", 1.1f},
    {376, "MidF", 1.1f},

    // Bottom
    {379, "BtmA", 1.0f},
    {380, "BtmB", 1.0f},
    {381, "BtmC", 1.1f},
    {382, "BtmD", 1.2f},
    {383, "BtmE", 1.3f},
};
static constexpr int g_mk2_widths_count = (int)(sizeof(g_mk2_widths)/sizeof(g_mk2_widths[0]));

// ---- MK3 ----
static MotWidthEntry g_mk3_widths[] = {
    // Up
    {271, "UpA",  1.1f},
    {272, "UpB",  1.2f},
    {273, "UpC",  1.1f},
    {274, "UpD",  1.2f},
    {275, "UpE",  1.0f},
    {276, "UpF",  1.1f},
    {277, "UpG",  1.8f},

    // Bottom
    {279, "BtmA", 1.1f},
    {280, "BtmB", 1.2f},
    {281, "BtmC", 1.4f},
    {282, "BtmD", 1.1f},
    {283, "BtmE", 1.0f},
    {284, "BtmF", 1.2f},
    {285, "BtmG", 1.2f},
    {286, "BtmH", 1.6f},
};
static constexpr int g_mk3_widths_count = (int)(sizeof(g_mk3_widths)/sizeof(g_mk3_widths[0]));

// Helper: search one table
static inline bool find_width_in_table(const MotWidthEntry* tbl, int count, int mot, float& out_width) {
    for (int i = 0; i < count; ++i) {
        if (tbl[i].mot == mot) {
            out_width = tbl[i].width;
            return true;
        }
    }
    return false;
}

// Update the active per-motion target each frame (sets g_per_motion_active/value)
static inline void update_per_motion_target() {
    g_per_motion_active = false;

    // Only used by per-motion flicker now
    if (!g_per_motion_flicker) {
        return;
    }

    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) {
        return;
    }

    const int mot = player->mCharaStatus.motionNo;

    float w = 1.0f;
    // Blood Berry first (per request), then MK1, MK2, MK3
    if (find_width_in_table(g_bb_widths,  g_bb_widths_count,  mot, w) ||
        find_width_in_table(g_mk1_widths, g_mk1_widths_count, mot, w) ||
        find_width_in_table(g_mk2_widths, g_mk2_widths_count, mot, w) ||
        find_width_in_table(g_mk3_widths, g_mk3_widths_count, mot, w)) {
        g_per_motion_value  = w;
        g_per_motion_active = true;
    }
}

static float detour4_xmm1backup = 0.0f;
naked void detour4() { // girth randomization
    __asm {
            // Per-motion FLICKER has highest priority (multiplies base randomization)
            cmp byte ptr [g_per_motion_flicker], 1
            jne checkOtherModes
            mulss xmm0, [g_pm_flicker_current]
            jmp originalcode

        checkOtherModes:
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

    // picker layout selection
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
    // Per-motion target detection (used by flicker)
    update_per_motion_target();

    // Per-motion FLICKER smoothing with same timings as glow.
    // Uses table value as a multiplier for the game's base flicker randomization.
    {
        if (g_per_motion_flicker) {
            float desired = g_per_motion_active ? g_per_motion_value : 1.0f;

            const bool wants_boost     = (desired > 1.0f);
            const bool is_boosting_now = (g_pm_flicker_current > 1.0f);
            const bool use_smoothing   = wants_boost || is_boosting_now;

            if (use_smoothing) {
                const float dt  = ImGui::GetIO().DeltaTime;
                const float tau = (desired > g_pm_flicker_current ? g_glow_ease_in_ms : g_glow_ease_out_ms) * 0.001f;
                const float a   = (tau <= 0.0f) ? 1.0f : (1.0f - std::exp(-dt / tau));
                g_pm_flicker_current += (desired - g_pm_flicker_current) * a;

                if (desired <= 1.0f && g_pm_flicker_current < 1.001f) {
                    g_pm_flicker_current = 1.0f; // snap to baseline
                }
            } else {
                g_pm_flicker_current = desired;
            }
        } else {
            g_pm_flicker_current = 1.0f;
        }
    }

    // Caramelldansen cycle driver
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

    // Smoothed glow with Finish Bonus multiplier
    {
        mHRPc* player = sword_glow_enabled ? nmh_sdk::get_mHRPc() : nullptr;
        if (player) {
            const int mot = player->mCharaStatus.motionNo;

            const bool allow_now = player->mCharaStatus.visibleWepEffect && (!g_glow_only_on_attack_mots || is_attack_motion_for_glow(mot));

            float base_target = swordGlowAmount;
            if (allow_now && player->mPcStatus.finishBonus) {
                base_target *= 3.0f;
            }

            const float target = allow_now ? base_target : 0.0f;

            const float dt  = ImGui::GetIO().DeltaTime;
            const float tau = (target > g_glow_current ? g_glow_ease_in_ms : g_glow_ease_out_ms) * 0.001f;
            const float a   = (tau <= 0.0f) ? 1.0f : (1.0f - std::exp(-dt / tau));

            g_glow_current += (target - g_glow_current) * a;

            if (g_glow_current < 0.001f)
                g_glow_current = 0.0f;

            if (g_glow_current > 0.0f) {
                uint32_t currentCol = nmh_sdk::GetLaserColor();
                nmh_sdk::SetLightReflect(player, g_glow_current, &player->mPcEffect.posHitSlash, currentCol, 0);
            }
        } else {
            if (g_glow_current > 0.0f) {
                const float dt  = ImGui::GetIO().DeltaTime;
                const float tau = g_glow_ease_out_ms * 0.001f;
                const float a   = (tau <= 0.0f) ? 1.0f : (1.0f - std::exp(-dt / tau));
                g_glow_current += (0.0f - g_glow_current) * a;
                if (g_glow_current < 0.001f)
                    g_glow_current = 0.0f;
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
        install_patch_offset(0x3BF53A, patch_force_trail, "\x90\x90\x90", 3);
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

        // easing controls
        ImGui::SliderFloat("Ease In (ms)", &g_glow_ease_in_ms, 0.0f, 1000.0f, "%.0f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "How quickly the glow ramps up. 0 = instant.";
        ImGui::SliderFloat("Ease Out (ms)", &g_glow_ease_out_ms, 0.0f, 1000.0f, "%.0f");
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "How quickly the glow fades after the attack ends. 0 = instant.";
        if (ImGui::Button("Default##GlowEaseDefaults")) {
            g_glow_ease_in_ms  = 120.0f;
            g_glow_ease_out_ms = 160.0f;
        }

        // Gate glow by current motion ID ranges for attacks/executions
        ImGui::Checkbox("NMH2 Glow", &g_glow_only_on_attack_mots);
        if (ImGui::IsItemHovered())
            SwordColours::hoveredDescription = "When enabled, glow applies only while performing Slash attacks. Max strength Death Blows "
                                               "have triple the intensity you assign.";

        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Always Display Laser Trails", &always_trail)) {
        toggleForceTrail(always_trail);
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Force the Beam Katana trails to always display.";

    if (ImGui::Checkbox("Always Display Attack Trails", &always_sword_blur)) {
        toggleForceSwordBlur(always_sword_blur);
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Force the Beam Katana attack trail to always display.";

    ImGui::SeparatorText("Width / Flicker");

    if (ImGui::Checkbox("Custom Flicker", &custom_flicker)) {
        force_girth = false;
        heart_girth = false;
        if (custom_flicker) {
            g_per_motion_flicker = false; // avoid double-multiplying
        }
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
        if (force_girth) {
            g_per_motion_flicker = false;
        }
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
        if (heart_girth) {
            g_per_motion_flicker = false;
        }
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

    // Per-motion FLICKER (reuses the per-motion tables as multipliers)
    if (ImGui::Checkbox("Per-Motion Flicker (Combo IDs)", &g_per_motion_flicker)) {
        if (g_per_motion_flicker) {
            SwordColours::force_girth    = false;
            SwordColours::custom_flicker = false;
            SwordColours::heart_girth    = false;
        }
    }
    if (ImGui::IsItemHovered())
        SwordColours::hoveredDescription = "Use the per-motion values as a flicker multiplier. 1.0 = normal; >1.0 = stronger flicker; <1.0 "
                                           "= calmer. Ramps/fizzles with Glow easing.";

    // Shared editor for per-motion values (shown only when Flicker is enabled)
    if (g_per_motion_flicker) {
        ImGui::Indent();

        if (ImGui::TreeNode("Edit Per-Motion Values")) {
            // -------- Blood Berry --------
            ImGui::SeparatorText("Blood Berry");
            ImGui::TextUnformatted("Up");
            for (int i = 0; i < g_bb_widths_count; ++i) {
                const int id = g_bb_widths[i].mot;
                if (id >= 223 && id <= 226) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_bb_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_bb_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Mid");
            for (int i = 0; i < g_bb_widths_count; ++i) {
                const int id = g_bb_widths[i].mot;
                if (id >= 228 && id <= 235) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_bb_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_bb_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Bottom");
            for (int i = 0; i < g_bb_widths_count; ++i) {
                const int id = g_bb_widths[i].mot;
                if (id >= 237 && id <= 243) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_bb_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_bb_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }

            // -------- Tsubaki Mk1 --------
            ImGui::SeparatorText("Tsubaki Mk1");
            ImGui::TextUnformatted("Up");
            for (int i = 0; i < g_mk1_widths_count; ++i) {
                const int id = g_mk1_widths[i].mot;
                if (id >= 319 && id <= 323) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk1_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk1_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Mid");
            for (int i = 0; i < g_mk1_widths_count; ++i) {
                const int id = g_mk1_widths[i].mot;
                if (id >= 325 && id <= 332) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk1_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk1_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Bottom");
            for (int i = 0; i < g_mk1_widths_count; ++i) {
                const int id = g_mk1_widths[i].mot;
                if (id >= 334 && id <= 337) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk1_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk1_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }

            // -------- Tsubaki Mk2 --------
            ImGui::SeparatorText("Tsubaki Mk2");
            ImGui::TextUnformatted("Up");
            for (int i = 0; i < g_mk2_widths_count; ++i) {
                const int id = g_mk2_widths[i].mot;
                if (id >= 366 && id <= 369) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk2_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk2_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Mid");
            for (int i = 0; i < g_mk2_widths_count; ++i) {
                const int id = g_mk2_widths[i].mot;
                if (id >= 371 && id <= 376) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk2_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk2_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Bottom");
            for (int i = 0; i < g_mk2_widths_count; ++i) {
                const int id = g_mk2_widths[i].mot;
                if (id >= 379 && id <= 383) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk2_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk2_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }

            // -------- Tsubaki Mk3 --------
            ImGui::SeparatorText("Tsubaki Mk3");
            ImGui::TextUnformatted("Up");
            for (int i = 0; i < g_mk3_widths_count; ++i) {
                const int id = g_mk3_widths[i].mot;
                if (id >= 271 && id <= 277) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk3_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk3_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }
            ImGui::TextUnformatted("Bottom");
            for (int i = 0; i < g_mk3_widths_count; ++i) {
                const int id = g_mk3_widths[i].mot;
                if (id >= 279 && id <= 286) {
                    char lbl[64];
                    sprintf(lbl, "%d %s", id, g_mk3_widths[i].label);
                    ImGui::SliderFloat(lbl, &g_mk3_widths[i].width, 0.0f, 5.0f, "%.1f");
                }
            }

            if (ImGui::Button("Defaults##PerMotionValuesDefaults")) {
                for (int i = 0; i < g_bb_widths_count; ++i)
                    g_bb_widths[i].width = 0.5f;
                for (int i = 0; i < g_mk1_widths_count; ++i)
                    g_mk1_widths[i].width = 0.5f;
                for (int i = 0; i < g_mk2_widths_count; ++i)
                    g_mk2_widths[i].width = 0.5f;
                for (int i = 0; i < g_mk3_widths_count; ++i)
                    g_mk3_widths[i].width = 0.5f;
            }
            ImGui::TreePop();
        }

        ImGui::Unindent();
    }

    ImGui::PopItemWidth();

    ImGui::SeparatorText("Color");

    // layout toggle
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

    // Caramelldansen UI
    if (ImGui::Checkbox("Caramelldansen", &g_rgb_cycle)) {
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
        SwordColours::hoveredDescription = "Missa inte chansen Nu ar vi har med Caramelldansen";

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
                "Turn the Beam Katana the color of your choice during a Death Blow. A feature inspired by NMH3, this timer controls how "
                "long the colour will stay applied when initiating a Deathblow";
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

    // width/flicker stuff
    force_girth                 = cfg.get<bool>("force_girth").value_or(false);
    force_girth_amount          = cfg.get<float>("force_girth_amount").value_or(0.5f);
    custom_flicker              = cfg.get<bool>("custom_flicker").value_or(false);
    flicker_amount              = cfg.get<float>("flicker_amount").value_or(1.0f);
    disable_girth_randomization = cfg.get<bool>("disable_girth_randomization").value_or(false);
    heart_girth                 = cfg.get<bool>("heart_girth").value_or(false);
    base_heart_girth            = cfg.get<float>("base_heart_girth").value_or(0.5f);
    heartbeat_girth_amount      = cfg.get<float>("heartbeat_girth_amount").value_or(2.0f);

    // per-motion toggle
    g_per_motion_flicker = cfg.get<bool>("per_motion_flicker").value_or(false);

    // per-motion values
    for (int j = 0; j < g_bb_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_bb_widths[j].mot);
        g_bb_widths[j].width = cfg.get<float>(key).value_or(0.5f);
    }
    for (int j = 0; j < g_mk1_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk1_widths[j].mot);
        g_mk1_widths[j].width = cfg.get<float>(key).value_or(0.5f);
    }
    for (int j = 0; j < g_mk2_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk2_widths[j].mot);
        g_mk2_widths[j].width = cfg.get<float>(key).value_or(0.5f);
    }
    for (int j = 0; j < g_mk3_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk3_widths[j].mot);
        g_mk3_widths[j].width = cfg.get<float>(key).value_or(0.5f);
    }

    // picker layout preference
    g_use_color_wheel = cfg.get<bool>("use_color_wheel").value_or(true);

    // Caramelldansen prefs
    g_rgb_cycle       = cfg.get<bool>("rgb_cycle").value_or(false);
    g_rgb_cycle_speed = cfg.get<float>("rgb_cycle_speed").value_or(1.0f);

    // glow gating preference
    g_glow_only_on_attack_mots = cfg.get<bool>("glow_only_on_attack_mots").value_or(false);

    // easing prefs
    g_glow_ease_in_ms  = cfg.get<float>("glow_ease_in_ms").value_or(120.0f);
    g_glow_ease_out_ms = cfg.get<float>("glow_ease_out_ms").value_or(160.0f);

    // If starting with Caramelldansen on, back up the loaded picks once
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

    // Save user's original picks if Caramelldansen is active
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

    // width/flicker stuff
    cfg.set<bool>("force_girth", force_girth);
    cfg.set<float>("force_girth_amount", force_girth_amount);
    cfg.set<bool>("custom_flicker", custom_flicker);
    cfg.set<float>("flicker_amount", flicker_amount);
    cfg.set<bool>("disable_girth_randomization", disable_girth_randomization);
    cfg.set<bool>("heart_girth", heart_girth);
    cfg.set<float>("base_heart_girth", base_heart_girth);
    cfg.set<float>("heartbeat_girth_amount", heartbeat_girth_amount);

    // per-motion toggle
    cfg.set<bool>("per_motion_flicker", g_per_motion_flicker);

    // per-motion values
    for (int j = 0; j < g_bb_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_bb_widths[j].mot);
        cfg.set<float>(key, g_bb_widths[j].width);
    }
    for (int j = 0; j < g_mk1_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk1_widths[j].mot);
        cfg.set<float>(key, g_mk1_widths[j].width);
    }
    for (int j = 0; j < g_mk2_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk2_widths[j].mot);
        cfg.set<float>(key, g_mk2_widths[j].width);
    }
    for (int j = 0; j < g_mk3_widths_count; ++j) {
        char key[64];
        sprintf(key, "per_mot_width_%d", g_mk3_widths[j].mot);
        cfg.set<float>(key, g_mk3_widths[j].width);
    }

    // picker layout preference
    cfg.set<bool>("use_color_wheel", g_use_color_wheel);

    // Caramelldansen prefs
    cfg.set<bool>("rgb_cycle", g_rgb_cycle);
    cfg.set<float>("rgb_cycle_speed", g_rgb_cycle_speed);

    // glow gating preference
    cfg.set<bool>("glow_only_on_attack_mots", g_glow_only_on_attack_mots);

    // easing prefs
    cfg.set<float>("glow_ease_in_ms", g_glow_ease_in_ms);
    cfg.set<float>("glow_ease_out_ms", g_glow_ease_out_ms);
}

// will show up in debug window, dump ImGui widgets you want here
// void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
