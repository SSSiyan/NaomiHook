#include "SwordColours.hpp"
#include "utility/Compressed.hpp"
#include "intrin.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/SwordsTextureAtlas.cpp"

bool SwordColours::mod_enabled = false;
bool SwordColours::sword_glow_enabled = false;
bool SwordColours::always_trail = false;
bool SwordColours::heart_colours = false;

bool SwordColours::always_sword_blur = false;

uintptr_t SwordColours::jmp_ret1 = NULL;
uintptr_t SwordColours::gpBattle = NULL;

uintptr_t SwordColours::jmp_ret2 = NULL;

uintptr_t SwordColours::jmp_ret3 = NULL;
int SwordColours::deathblowTimer = 0;
int SwordColours::setDeathblowTimer = 0;
float SwordColours::swordGlowAmount = 0.0f;

uintptr_t SwordColours::jmp_ret4 = NULL;
uintptr_t SwordColours::hrScreenStatus = NULL;
bool SwordColours::force_girth = false;
float SwordColours::force_girth_amount = 0.5f;
float SwordColours::girth_normalizer = 0.5f;
bool SwordColours::custom_flicker = false;
float SwordColours::flicker_amount = 1.0f;
bool SwordColours::disable_girth_randomization = false;
bool SwordColours::heart_girth = false;
float SwordColours::base_heart_girth = 0.5f;
float SwordColours::heartbeat_girth_amount = 2.0f;
float SwordColours::heart_normalizer = 1.0f;

const char* SwordColours::defaultDescription = "Customize your beam katana colors. You can also set a unique color specifically for Death Blows.";
const char* SwordColours::hoveredDescription = defaultDescription;

struct IntColor {
    int r;
    int g;
    int b;
    int a;
};

static ImColor colours_picked_rgba[5];
static IntColor colours_picked_rgbaInt[5];

static IntColor heart_colour_rgbaInt{};
static float lastHpScale = 1.0f;
static IntColor frozenColor = {255, 255, 255, 255};

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

/*
static constexpr float frameTime = 0.0125;

static constexpr float lowJustCharge = 1.250f;
static constexpr float lowJustChargeEnd = lowJustCharge + (frameTime * 2.0f);
static constexpr float lowHalfCharge = lowJustChargeEnd + (frameTime * 2.0f);

static constexpr float highJustCharge = 0.875f;
static constexpr float highJustChargeEnd = highJustCharge + (frameTime);
static constexpr float highHalfCharge = highJustChargeEnd + (frameTime);
*/
// clang-format off
naked void detour1() { // swords, player in ebx
    __asm {
            push eax
            mov al, [SwordColours::heart_colours]
            or al, [SwordColours::mod_enabled]
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
            // cmp dword ptr [ebx+0x18C], ePcMtBtAtkChgUp
            // je high_charge
        /*
        // low_charge:
            movss xmm4, [ebx+0x28F0+0x4C+0x4] // player->mSnd.pitchCharge.mCurValue
            comiss xmm4, [lowJustCharge]
            jb getSwordID
            comiss xmm4, [lowJustChargeEnd]
            jae check_half_low_charge
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x50]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x50+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x50+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode
            
        check_half_low_charge:
            comiss xmm4, [lowHalfCharge]
            ja getSwordID // default, divide battery by 0.25
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x60]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x60+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x60+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode

        high_charge:
            movss xmm4, [ebx+0x28F0+0x4C+0x4] // player->mSnd.pitchCharge.mCurValue
            comiss xmm4, [highJustCharge]
            jb getSwordID
            comiss xmm4, [highJustChargeEnd]
            jae check_half_high_charge
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x50]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x50+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x50+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode
            
        check_half_high_charge:
            comiss xmm4, [highHalfCharge]
            ja getSwordID // default, divide battery by 0.25
            push eax // 4
            mov eax,[colours_picked_rgbaInt+0x60]
            mov [esp+0x4+0x4],eax
            mov eax,[colours_picked_rgbaInt+0x60+0x4]
            mov [esp+0x4+0x8],eax
            mov eax,[colours_picked_rgbaInt+0x60+0x8]
            mov [esp+0x4+0xC],eax
            pop eax
            jmp originalcode
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
            or al, [SwordColours::mod_enabled]
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
    auto& io = ImGui::GetIO();
    // similar size for all textures so grab whatever
    ImGui::ImageWithBg((ImTextureID)g_sword_texture_atlas->GetTexture(), 
        (ImVec2(blade.size.x * (io.DisplaySize.y / 1080.0f), blade.size.y * (io.DisplaySize.y / 1080.0f))), blade.uv0, blade.uv1, ImColor(0.0f, 0.0f, 0.0f, 0.0f), rgba);

    ImGui::SetCursorPos(pos);

    ImGui::ImageWithBg((ImTextureID)g_sword_texture_atlas->GetTexture(), 
        (ImVec2(hilt.size.x * (io.DisplaySize.y / 1080.0f), hilt.size.y * (io.DisplaySize.y / 1080.0f))), hilt.uv0, hilt.uv1, ImColor(0.0f, 0.0f, 0.0f, 0.0f));

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

void SwordColours::on_frame() {
    if (sword_glow_enabled) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (player) {
            if (player->mCharaStatus.visibleWepEffect) {
                int currentSword = player->mPcStatus.equip[0].id;
                uint32_t currentCol = nmh_sdk::GetLaserColor();
                nmh_sdk::SetLightReflect(player, swordGlowAmount, &player->mPcEffect.posHitSlash, currentCol, 0);
            }
        }
    }
    if (heart_colours) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (!player) { return; }
        mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle();
        if (!mHRBattle) { return; }
        HrScreenStatus* screenStatus = mHRBattle->mBtEffect.pScreenStatus;
        if (screenStatus) {
            static float colorPhase = 0.0f;
            if (screenStatus->m_HpScale > 1.0f) {
                float beatIntensity = (screenStatus->m_HpScale - 1.0f) / 0.2f;
                beatIntensity = std::min(beatIntensity, 1.0f);
        
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
    }
    else {
        install_patch_offset(0x3BF53A, patch_force_trail, "\x8A\x55\x08", 3); // mov dl,[ebp+08]
    }
}

void SwordColours::toggleForceSwordBlur(bool enable) {
    if (enable) {
        install_patch_offset(0x3C5C9A, patch_force_sword_blur, "\x6A\x01\x90\x90", 4); // push 01 nop 2
    }
    else {
        install_patch_offset(0x3C5C9A, patch_force_sword_blur, "\xFF\x74\x24\x20", 4); // push [esp+20]
    }
}

void SwordColours::render_description() const {
    ImGui::TextWrapped(SwordColours::hoveredDescription);
}

void SwordColours::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) SwordColours::hoveredDescription = defaultDescription;
    float fontSize = ImGui::GetFontSize();

    ImGui::PushItemWidth(fontSize * 5.0f);

    ImGui::SeparatorText("Misc");

    ImGui::Checkbox("Glow", &sword_glow_enabled);
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Restores the beam reflections seen in early gameplay footage. This is the same glow that would later be used in NMH2 and Heroes Paradise";
    if (sword_glow_enabled) {
        ImGui::Indent();
        ImGui::SliderFloat("Glow Intensity", &swordGlowAmount, 1.0f, 5.0f, "%.0f");
            if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set how bright the glow from your sword is.";

        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Always Display Laser Trails", &always_trail)) {
        toggleForceTrail(always_trail);
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Force the Beam Katana trails to always display.";

    if (ImGui::Checkbox("Always Display Laser Speed Blur", &always_sword_blur)) {
        toggleForceSwordBlur(always_sword_blur);
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Force the Beam Katana speed blur to always display.";

    ImGui::SeparatorText("Width");
    
    if (ImGui::Checkbox("Custom Flicker", &custom_flicker)) {
        force_girth = false;
        heart_girth = false;
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set how much the beam width flickers.";

    if (custom_flicker) {
        ImGui::Indent();
        ImGui::SliderFloat("Flicker Amount", &flicker_amount, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set flicker amount.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Beam Flicker Default")) {
            flicker_amount = 1.0f;
        }
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Custom Beam Width", &force_girth)) {
        heart_girth = false;
        custom_flicker = false;
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set the width of sword lasers.";

    if (force_girth) {
        ImGui::Indent();
        ImGui::SliderFloat("Beam Width", &force_girth_amount, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set beam width.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Beam Width Default")) {
            force_girth_amount = 0.5f;
        }
        ImGui::Unindent();
    }

    if (ImGui::Checkbox("Heartbeat Beam Width", &heart_girth)) {
        force_girth = false;
        custom_flicker = false;
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Use the heart beat to set the girth of the sword.";

    if (heart_girth) {
        ImGui::Indent();
        ImGui::SliderFloat("Base Width", &base_heart_girth, 0.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Base width before the heart beats.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Heart Base Default")) {
            base_heart_girth = 0.5f;
        }
        ImGui::SliderFloat("Beat Width", &heartbeat_girth_amount, -5.0f, 5.0f, "%.1f");
        if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Set how much width the heartbeat adds.";
        ImGui::SameLine();
        if (ImGui::Button("Default##Heart Beat Default")) {
            heartbeat_girth_amount = 2.0f;
        }
        ImGui::Unindent();
    }

    ImGui::PopItemWidth();

    ImGui::SeparatorText("Color");

    if (ImGui::Checkbox("Heartbeat Color Sync", &heart_colours)) {
        mod_enabled =false;
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Randomly cycles through beam colors and pulses with each color switch. This is synced to the HP's heart beat.";

    if (ImGui::Checkbox("Custom Colours", &mod_enabled)) {
        heart_colours = false;
    }
    if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = defaultDescription;

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
        if (ImGui::IsItemHovered()) SwordColours::hoveredDescription = "Turn the Beam Katana the color of your choice during a Death Blow."
            "A feature inspired by NMH3, this timer controls how long the colour will stay applied when initiating a Deathblow";
        ImGui::Unindent();

        // draw_sword_behavior("Just Charge", swords[1].blade, swords[1].hilt, colours_picked_rgba[5], colours_picked_rgbaInt[5], cfg_defaults[4]);
        // draw_sword_behavior("Late Charge", swords[1].blade, swords[1].hilt, colours_picked_rgba[6], colours_picked_rgbaInt[6], cfg_defaults[4]);
    }
}

void SwordColours::on_d3d_reset() {
    // explicitly call the destructor first
    g_sword_texture_atlas.reset();
    load_texture();
}

std::optional<std::string> SwordColours::on_initialize() {
    SwordColours::gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584; 
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
void SwordColours::on_config_load(const utility::Config &cfg) {
    sword_glow_enabled = cfg.get<bool>("sword_glow_enabled").value_or(false);
    mod_enabled = cfg.get<bool>("custom_colours").value_or(false);
    always_trail = cfg.get<bool>("always_trail").value_or(false);
    if (always_trail) toggleForceTrail(always_trail);
    heart_colours = cfg.get<bool>("heart_colours").value_or(false);
    always_sword_blur   = cfg.get<bool>("always_sword_blur").value_or(false);
    if (always_sword_blur) toggleForceSwordBlur(always_sword_blur);
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

    swordGlowAmount = cfg.get<float>("swordGlowAmount").value_or(4.0f);

    // width stuff
    force_girth = cfg.get<bool>("force_girth").value_or(false);
    force_girth_amount = cfg.get<float>("force_girth_amount").value_or(0.5f);
    custom_flicker     = cfg.get<bool>("custom_flicker").value_or(false);
    flicker_amount     = cfg.get<float>("flicker_amount").value_or(1.0f);
    disable_girth_randomization = cfg.get<bool>("disable_girth_randomization").value_or(false);
    heart_girth                 = cfg.get<bool>("heart_girth").value_or(false);
    base_heart_girth            = cfg.get<float>("base_heart_girth").value_or(0.5f);
    heartbeat_girth_amount      = cfg.get<float>("heartbeat_girth_amount").value_or(2.0f);
}

// during save
void SwordColours::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("sword_glow_enabled", sword_glow_enabled);
    cfg.set<bool>("custom_colours", mod_enabled);
    cfg.set<bool>("always_trail", always_trail);
    cfg.set<bool>("heart_colours", heart_colours);
    cfg.set<bool>("always_sword_blur", always_sword_blur);
    for (const auto& RgbaDefault : cfg_defaults) {
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".r", RgbaDefault.color->r);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".g", RgbaDefault.color->g);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".b", RgbaDefault.color->b);
        cfg.set<int>(std::string(RgbaDefault.cfg_name) + ".a", RgbaDefault.color->a);
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
}

// will show up in debug window, dump ImGui widgets you want here
//void SwordColours::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
