#include "WeaponSwitcher.hpp"
#include "utility/Compressed.hpp"
#include "intrin.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/SwordSwitchAtlas.cpp"

#if 1

// directx stuff
static std::unique_ptr<Texture2DD3D11> g_sword_switch_texture_atlas{};

#pragma region TextureAtlasDefinitions

struct Frame {
    ImVec2 pos, size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto Blood_Berry() {
        return Frame{
            ImVec2{ 0.0f, 0.0f },
            ImVec2{ 512.0f, 512.0f },
            ImVec2{ 0.0f / 1024.0f, 0.0f / 1024.0f },
            ImVec2{ 512.0f / 1024.0f, 512.0f / 1024.0f }
        };
    }
    static constexpr auto Mk_1() {
        return Frame{
            ImVec2{ 512.0f, 0.0f },
            ImVec2{ 512.0f, 512.0f },
            ImVec2{ 512.0f / 1024.0f, 0.0f / 1024.0f },
            ImVec2{ 1024.0f / 1024.0f, 512.0f / 1024.0f }
        };
    }
    static constexpr auto Mk_2() {
        return Frame{
            ImVec2{ 0.0f, 512.0f },
            ImVec2{ 512.0f, 512.0f },
            ImVec2{ 0.0f / 1024.0f, 512.0f / 1024.0f },
            ImVec2{ 512.0f / 1024.0f, 1024.0f / 1024.0f }
        };
    }
    static constexpr auto Mk_3() {
        return Frame{
            ImVec2{ 512.0f, 512.0f },
            ImVec2{ 512.0f, 512.0f },
            ImVec2{ 512.0f / 1024.0f, 512.0f / 1024.0f },
            ImVec2{ 1024.0f / 1024.0f, 1024.0f / 1024.0f }
        };
    }
    static constexpr auto meta_size() {
        return ImVec2{ 1024.0f, 1024.0f };
    }
};


static constexpr TextureAtlas g_atlas{};
Frame Blood_Berry = g_atlas.Blood_Berry();
Frame Mk_1 = g_atlas.Mk_1();
Frame Mk_2 = g_atlas.Mk_2();
Frame Mk_3 = g_atlas.Mk_3();

#pragma endregion

bool WeaponSwitcher::mod_enabled = false;
uintptr_t WeaponSwitcher::jmp_ret1 = NULL;
uintptr_t WeaponSwitcher::jmp_ret2 = NULL;
uintptr_t WeaponSwitcher::jmp_ret3 = NULL;
    uintptr_t detour3_je = NULL;
int WeaponSwitcher::weaponSwitchCooldown = 80; // this is what ticks
static int weaponSwitchLockedFrames = 10; // this locks you out from weapon switching
static int animationDuration = WeaponSwitcher::weaponSwitchCooldown;

enum WEAPON_SWITCH_DIRECTION {
    WS_LEFT = 0,
    WS_DOWN = 1,
    WS_RIGHT = 2,
    WS_UP = 3,
    WS_COUNT,
};
static WEAPON_SWITCH_DIRECTION directionPressed = WS_LEFT;

//                                   <   v   >   ^
static int selectedSword[WS_COUNT]{ -1, -1, -1, -1 }; // pcItem
static int savedSword[WS_COUNT]{ -1, -1, -1, -1 }; // pcItem

// bool WeaponSwitcher::weapon_switcher_ui = false;

// Disable toggling the map while Weapon Switcher is active
void WeaponSwitcher::toggleForceMap(bool enable) {
    if (enable) {
        install_patch_offset(0x3DC204, m_patch, "\xEB", 1); // jmp nmh.exe+3DC21A
    }
    else {
        install_patch_offset(0x3DC204, m_patch, "\x75", 1); // jne nmh.exe+3DC21A
    }
}

// check a weapon switch ban list of states and animations
bool WeaponSwitcher::CanWeaponSwitch(pcItem desiredWeapon) {
    if (mHRPc* playerPtr = nmh_sdk::get_mHRPc()) {
        enPcInputMode currentMode = playerPtr->mInputMode;
        pcItem currentWeapon = playerPtr->mPcStatus.equip[0].id;
        pcMotion currentMoveID = playerPtr->mCharaStatus.motionNo;
        enCharaCondition condition = playerPtr->mCharaStatus.condition;
        if (desiredWeapon != currentWeapon && 
            desiredWeapon != -1 &&
            currentMode == ePcInputBattleIdle &&
            condition == eGood &&
            nmh_sdk::CheckCanAttack() &&
            !nmh_sdk::CheckGuardMotion(false) && 
            //!nmh_sdk::CheckHajikare() && // eating a hit @siy this is bad
            !nmh_sdk::CheckTsubazering(-1) && // clashing
            !nmh_sdk::CheckSideStep(-1) && // dodging back or left or right
            currentMoveID != ePcMtBtryChrgSt &&
            currentMoveID != ePcMtBtryChrgLp &&
            currentMoveID != ePcMtStpF &&
            currentMoveID != ePcMtAvdR &&
            currentMoveID != ePcMtAvdL) {
            return true;
        }
    }
    return false;
}

const char* pcItemToString(pcItem item) {
    switch (item) {
        case BLOOD_BERRY: return "Blood Berry";
        case TSUBAKI_MK3: return "Tsubaki Mk3";
        case TSUBAKI_MK1: return "Tsubaki Mk1";
        case TSUBAKI_MK2: return "Tsubaki Mk2";
        case BLOOD_BERRY_DAMAGE: return "Blood Berry +Damage";
        case TSUBAKI_MK3_DAMAGE: return "Tsubaki Mk3 +Damage";
        case TSUBAKI_MK1_DAMAGE: return "Tsubaki Mk1 +Damage";
        case TSUBAKI_MK2_DAMAGE: return "Tsubaki Mk2 +Damage";
        case BLOOD_BERRY_BATTERY: return "Blood Berry +Battery";
        case TSUBAKI_MK3_BATTERY: return "Tsubaki Mk3 +Battery";
        case TSUBAKI_MK1_BATTERY: return "Tsubaki Mk1 +Battery";
        case TSUBAKI_MK2_BATTERY: return "Tsubaki Mk2 +Battery";
        case BLOOD_BERRY_BATTERY_DAMAGE: return "Blood Berry +Battery +Damage";
        case TSUBAKI_MK3_BATTERY_DAMAGE: return "Tsubaki Mk3 +Battery +Damage";
        case TSUBAKI_MK1_BATTERY_DAMAGE: return "Tsubaki Mk1 +Battery +Damage";
        case TSUBAKI_MK2_BATTERY_DAMAGE: return "Tsubaki Mk2 +Battery +Damage";
        default: return "Unknown";
    }
}

std::vector<pcItem> FindMatchingItemsForSlot(WEAPON_SWITCH_DIRECTION slotID) {
    std::vector<pcItem> matchingItems;

    pcItem equippedWeapon = static_cast<pcItem>(nmh_sdk::get_mHRPc()->mPcStatus.equip[0].id); // Get the equipped item // mPcStatus // mPcSaveData

    for (int i = 0; i < 200; i++) {
        auto lockerItem = nmh_sdk::get_mHRPc()->mPcStatus.locker[i];
        pcItem weapon = static_cast<pcItem>(lockerItem.id);

        switch (slotID) {
        case WS_LEFT:
                if (weapon == TSUBAKI_MK1 || weapon == TSUBAKI_MK1_DAMAGE || weapon == TSUBAKI_MK1_BATTERY || weapon == TSUBAKI_MK1_BATTERY_DAMAGE) {
                    matchingItems.push_back(weapon);
                }
                break;
            case WS_DOWN:
                if (weapon == TSUBAKI_MK2 || weapon == TSUBAKI_MK2_DAMAGE || weapon == TSUBAKI_MK2_BATTERY || weapon == TSUBAKI_MK2_BATTERY_DAMAGE) {
                    matchingItems.push_back(weapon);
                }
                break;
            case WS_RIGHT:
                if (weapon == TSUBAKI_MK3 || weapon == TSUBAKI_MK3_DAMAGE || weapon == TSUBAKI_MK3_BATTERY || weapon == TSUBAKI_MK3_BATTERY_DAMAGE) {
                    matchingItems.push_back(weapon);
                }
                break;
            case WS_UP:
                if (weapon == BLOOD_BERRY || weapon == BLOOD_BERRY_DAMAGE || weapon == BLOOD_BERRY_BATTERY || weapon == BLOOD_BERRY_BATTERY_DAMAGE) {
                    matchingItems.push_back(weapon);
                }
                break;
            default:
                break;
        }
    }

    if ((slotID == WS_LEFT  && (equippedWeapon == TSUBAKI_MK1 || equippedWeapon == TSUBAKI_MK1_DAMAGE || equippedWeapon == TSUBAKI_MK1_BATTERY || equippedWeapon == TSUBAKI_MK1_BATTERY_DAMAGE)) ||
        (slotID == WS_DOWN  && (equippedWeapon == TSUBAKI_MK2 || equippedWeapon == TSUBAKI_MK2_DAMAGE || equippedWeapon == TSUBAKI_MK2_BATTERY || equippedWeapon == TSUBAKI_MK2_BATTERY_DAMAGE)) ||
        (slotID == WS_RIGHT && (equippedWeapon == TSUBAKI_MK3 || equippedWeapon == TSUBAKI_MK3_DAMAGE || equippedWeapon == TSUBAKI_MK3_BATTERY || equippedWeapon == TSUBAKI_MK3_BATTERY_DAMAGE)) ||
        (slotID == WS_UP    && (equippedWeapon == BLOOD_BERRY || equippedWeapon == BLOOD_BERRY_DAMAGE || equippedWeapon == BLOOD_BERRY_BATTERY || equippedWeapon == BLOOD_BERRY_BATTERY_DAMAGE))) {
        matchingItems.push_back(equippedWeapon);
    }

    return matchingItems;
}

void FillComboBoxWithImGui(WEAPON_SWITCH_DIRECTION slotID, const char* comboBoxLabel) {
    std::vector<pcItem> matchingItems = FindMatchingItemsForSlot(slotID);

    std::vector<const char*> itemNames;
    for (const auto& item : matchingItems) {
        itemNames.push_back(pcItemToString(item));
    }
    int selectedIndex = -1;
    
    for (int i = 0; i < matchingItems.size(); ++i) {
        if (matchingItems[i] == selectedSword[slotID]) {
            selectedIndex = i;
            break;
        }
    }

    if (ImGui::Combo(comboBoxLabel, &selectedIndex, itemNames.data(), itemNames.size())) {
        selectedSword[slotID] = matchingItems[selectedIndex];
    }
}

// clang-format off
naked void detour1() { // play weapon anims // player in ecx // called last
    __asm {
        //
            cmp byte ptr [WeaponSwitcher::mod_enabled], 0
            je originalcode
        //
            cmp dword ptr [ecx+0x2990], ePcInputBattleIdle // enPcInputMode
            jne originalcode
            cmp dword ptr [WeaponSwitcher::weaponSwitchCooldown], 10 // only edit anim if weapon switcher changed the value
            jae originalcode
            push 01 // interrupt
            push 00 // frame
            push 00 // repeat
            push 03 // id
            jmp dword ptr [WeaponSwitcher::jmp_ret1]


            cmp dword ptr [ecx+0x42C], BLOOD_BERRY
            je berry
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK3
            je mk3
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK1
            je mk1
            cmp dword ptr [ecx+0x42C], TSUBAKI_MK2
            je mk2
            jmp originalcode

        berry:
            push 01
            push 70
            push 00
            push ePcMtBattou01
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk3:
            push 01
            push 00
            push 00
            push ePcMtBattou02Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk1:
            push 01
            push 00
            push 00
            push ePcMtBattou03Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        mk2:
            push 01
            push 00
            push 00
            push ePcMtBattou04Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret1]

        originalcode:
            push 01
            push 00
            push 01
            push 0x30
            jmp dword ptr [WeaponSwitcher::jmp_ret1]
    }
}

naked void detour2() { // play weapon anims // player in esi
    __asm {
        //
            cmp byte ptr [WeaponSwitcher::mod_enabled], 0
            je originalcode
        //
            cmp dword ptr [esi+0x2990], ePcInputBattleIdle // enPcInputMode
            jne originalcode
            cmp dword ptr [WeaponSwitcher::weaponSwitchCooldown], 10 // only edit anim if weapon switcher changed the value
            jae originalcode
            push 01 // interrupt
            push 00 // frame
            push 00 // repeat
            push 03 // id
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

            cmp dword ptr [esi+0x42C], BLOOD_BERRY
            je berry
            cmp dword ptr [esi+0x42C], TSUBAKI_MK3
            je mk3
            cmp dword ptr [esi+0x42C], TSUBAKI_MK1
            je mk1
            cmp dword ptr [esi+0x42C], TSUBAKI_MK2
            je mk2
            jmp originalcode

        berry:
            push 00
            push 70
            push 00
            push ePcMtBattou01
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk3:
            push 00
            push 00
            push 00
            push ePcMtBattou02Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk1:
            push 00
            push 00
            push 00
            push ePcMtBattou03Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        mk2:
            push 00
            push 00
            push 00
            push ePcMtBattou04Ed
            jmp dword ptr [WeaponSwitcher::jmp_ret2]

        originalcode:
            push 00
            push 00
            push 00
            push ePcMtGrdDfltLp
            jmp dword ptr [WeaponSwitcher::jmp_ret2]
    }
}

naked void detour3() { // add tsubaki mk1 weapon swap to canAttack ban list
    __asm {
            cmp byte ptr [WeaponSwitcher::mod_enabled], 0
            je originalcode

            cmp eax, ePcMtBattou03Ed
            je jmp_je

        originalcode:
            cmp eax,0x000000FA
            jmp dword ptr [WeaponSwitcher::jmp_ret3]

        jmp_je:
            jmp dword ptr [detour3_je]
    }
}
 // clang-format on

static bool load_weapon_switcher_texture() {
    auto [data, size] = utility::decompress_file_from_memory_with_size(sword_switch_compressed_data, sword_switch_compressed_size);
    if (!data) {
        return false;
    }

    g_sword_switch_texture_atlas = std::make_unique<Texture2DD3D11>((const unsigned char*)data, size, g_framework->d3d11()->get_device());
    if (!g_sword_switch_texture_atlas->IsValid()) {
        return false;
    }

    return true;
}

void WeaponSwitcher::on_draw_ui() {
    if (ImGui::Checkbox("Weapon Switcher", &mod_enabled)) {
        toggleForceMap(mod_enabled);
    }

    if (mod_enabled) {
        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            float windowWidth = ImGui::GetContentRegionAvail().x;
            float centerX = windowWidth * 0.5f;

            ImVec2 cursorStart = ImGui::GetCursorPos();

            float comboBoxWidth = windowWidth * 0.3f;

            ImGui::SetCursorPos(ImVec2(centerX - (comboBoxWidth * 0.5f), cursorStart.y));
            ImGui::PushItemWidth(comboBoxWidth);
            FillComboBoxWithImGui(WS_UP, "## Up Sword Combo");
            ImGui::PopItemWidth();

            ImGui::SetCursorPos(ImVec2(centerX - comboBoxWidth - ImGui::GetFontSize(), cursorStart.y + ImGui::GetFontSize() * 2));
            ImGui::PushItemWidth(comboBoxWidth);
            FillComboBoxWithImGui(WS_LEFT, "## Left Sword Combo");
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(centerX + ImGui::GetFontSize(), cursorStart.y + ImGui::GetFontSize() * 2));
            ImGui::PushItemWidth(comboBoxWidth);
            FillComboBoxWithImGui(WS_RIGHT, "## Right Sword Combo");
            ImGui::PopItemWidth();

            ImGui::SetCursorPos(ImVec2(centerX - (comboBoxWidth * 0.5f), cursorStart.y + ImGui::GetFontSize() * 4));
            ImGui::PushItemWidth(comboBoxWidth);
            FillComboBoxWithImGui(WS_DOWN, "## Down Sword Combo");
            ImGui::PopItemWidth();

            ImGui::Text("Current Weapon: %s", pcItemToString(player->mPcStatus.equip[0].id));

            // ImGui::Checkbox("Display UI", &weapon_switcher_ui);
        }
    }
}

void WeaponSwitcher::on_d3d_reset() {
    // explicitly call the destructor first
    g_sword_switch_texture_atlas.reset();
    load_weapon_switcher_texture();
}

void WeaponSwitcher::Display_UI() {
    if (weaponSwitchCooldown > animationDuration) { return; } // don't make a window if no image is displaying
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(screenSize);
    ImGui::Begin("WeaponSwitchPopup", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);

    ImVec2 weaponSize(200, 200);
    ImVec2 centerPos = ImVec2((screenSize.x * 0.5f), (screenSize.y * 0.5f) + 200);
    static ImVec2 weaponOffset(0, 0);
    float alphaValue = 0.0f;

    if (weaponSwitchCooldown > 0 && weaponSwitchCooldown <= animationDuration) {
        float progress = weaponSwitchCooldown / (float)animationDuration;
        float moveDistance = 40.0f * progress;
        
        // Fade
        if (weaponSwitchCooldown <= (animationDuration / 2)) {
            alphaValue = (weaponSwitchCooldown / (float)(animationDuration / 2));
        } else {
            alphaValue = ((animationDuration - weaponSwitchCooldown) / (float)(animationDuration / 2));
        }
        
        // Movement
        weaponOffset = ImVec2(0, 0);
        if (directionPressed == WS_UP) { weaponOffset.y = -moveDistance;} // Up
        if (directionPressed == WS_LEFT) { weaponOffset.x = -moveDistance;} // Left
        if (directionPressed == WS_RIGHT) { weaponOffset.x =  moveDistance;} // Right
        if (directionPressed == WS_DOWN) { weaponOffset.y =  moveDistance;} // Down
    }

    if (weaponSwitchCooldown > animationDuration) {
        weaponOffset = ImVec2(0, 0);
        alphaValue = 0.0f;
    }

    if (alphaValue > 0.0f) {
        // attach to the character
        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            Vec playerPos = player->mCharaStatus.navelPos;
            //playerPos.y += 10.0f; // how far up the character
            Vec screenPlayerPos{ 0.0f, 0.0f, 0.0f };
            nmh_sdk::GetScreenPos(&playerPos, &screenPlayerPos);
            float scaleX = screenSize.x / 854.0f; // scale from internal res to game res
            float scaleY = screenSize.y / 480.0f;
            ImVec2 scaledPos(screenPlayerPos.x * scaleX, screenPlayerPos.y * scaleY);
            ImGui::SetCursorPos(ImVec2(scaledPos.x - weaponSize.x * 0.5f + weaponOffset.x, scaledPos.y - weaponSize.y * 0.5f + weaponOffset.y));
        }

        // or just let it float
        // ImGui::SetCursorPos(ImVec2(centerPos.x - weaponSize.x * 0.5f + weaponOffset.x, centerPos.y - weaponSize.y * 0.5f + weaponOffset.y));

        if (directionPressed == WS_UP) {ImGui::ImageWithBg((ImTextureID)g_sword_switch_texture_atlas->GetTexture(), weaponSize, Blood_Berry.uv0, Blood_Berry.uv1, ImVec4(0,0,0,0), ImVec4(1,1,1,alphaValue));}
        if (directionPressed == WS_RIGHT) {ImGui::ImageWithBg((ImTextureID)g_sword_switch_texture_atlas->GetTexture(), weaponSize, Mk_3.uv0, Mk_3.uv1, ImVec4(0,0,0,0), ImVec4(1,1,1,alphaValue));}
        if (directionPressed == WS_LEFT) {ImGui::ImageWithBg((ImTextureID)g_sword_switch_texture_atlas->GetTexture(), weaponSize, Mk_1.uv0, Mk_1.uv1, ImVec4(0,0,0,0), ImVec4(1,1,1,alphaValue));}
        if (directionPressed == WS_DOWN) {ImGui::ImageWithBg((ImTextureID)g_sword_switch_texture_atlas->GetTexture(), weaponSize, Mk_2.uv0, Mk_2.uv1, ImVec4(0,0,0,0), ImVec4(1,1,1,alphaValue));}
    }
    ImGui::End();
}

/*pcItem FindBestWeapon(int weaponID) {
    auto locker = nmh_sdk::get_mHRPc()->mPcStatus.locker;

    std::vector<pcItem> preferredWeapons;

    if (weaponID == BLOOD_BERRY) {
        preferredWeapons = {BLOOD_BERRY_BATTERY_DAMAGE, BLOOD_BERRY_BATTERY, BLOOD_BERRY_DAMAGE, BLOOD_BERRY};
    }
    else if (weaponID == TSUBAKI_MK3) {
        preferredWeapons = {TSUBAKI_MK3_BATTERY_DAMAGE, TSUBAKI_MK3_BATTERY, TSUBAKI_MK3_DAMAGE, TSUBAKI_MK3};
    }
    else if (weaponID == TSUBAKI_MK1) {
        preferredWeapons = {TSUBAKI_MK1_BATTERY_DAMAGE, TSUBAKI_MK1_BATTERY, TSUBAKI_MK1_DAMAGE, TSUBAKI_MK1};
    }
    else if (weaponID == TSUBAKI_MK2) {
        preferredWeapons = {TSUBAKI_MK2_BATTERY_DAMAGE, TSUBAKI_MK2_BATTERY, TSUBAKI_MK2_DAMAGE, TSUBAKI_MK2};
    }
    else {
        return {};
    }

    for (pcItem target : preferredWeapons) {
        for (int i = 0; i < 200; i++) {
            if (locker[i].id == target) {
                return target;
            }
        }
    }

    return (pcItem)-1;
}*/

bool IsPlayingSwordChangeAnim(int motID) {
    if (motID == ePcMtBattou01 || motID == ePcMtBattou02Ed || motID == ePcMtBattou03Ed || motID == ePcMtBattou04Ed) {
        return true;
    }
    return false;
}

void WeaponSwitcher::on_frame() {
    static int previousSwordEquipRead = 0;
    static bool tryPlayAnimation = false;
    //static int checkmotReadProc = 0;
    // Check unlocked weapons when eEqWait1Frame is seen
    if (mod_enabled) {
        mHRPc* player = nmh_sdk::get_mHRPc();
        if (!player) { return; }
        if (player->mPcStatus.equip[0].readProc == eEqWait1Frame) {
            for (int i = 0; i < 4; i++) {
                std::vector<pcItem> matchingItems = FindMatchingItemsForSlot((WEAPON_SWITCH_DIRECTION)i);
                for (auto item : matchingItems) {
                    if (item == savedSword[i]) {
                        selectedSword[i] = savedSword[i];
                        break;
                    }
                }
            }
            if (selectedSword[WS_UP] == -1) {
                selectedSword[WS_UP] = BLOOD_BERRY; // default to Blood Berry
            }
        }

        // Weapon switching
        if (weaponSwitchCooldown > weaponSwitchLockedFrames) {
            uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
            if (dPadInputsAddr) {
                int8_t dPadInput = *(int8_t*)dPadInputsAddr;
                switch (dPadInput) {
                if (selectedSword[dPadInput] == -1) break;
                case DPAD_LEFT:
                    if (CanWeaponSwitch((pcItem)selectedSword[WS_LEFT])) {
                            nmh_sdk::SetEquip((pcItem)selectedSword[WS_LEFT], false);
                            weaponSwitchCooldown = 0;
                            directionPressed = WS_LEFT; // TSUBAKI_MK1
                    }
                    break;
                case DPAD_RIGHT:
                    if (CanWeaponSwitch((pcItem)selectedSword[WS_RIGHT])) {
                            nmh_sdk::SetEquip((pcItem)selectedSword[WS_RIGHT], false);
                            weaponSwitchCooldown = 0;
                            directionPressed = WS_RIGHT; // TSUBAKI_MK3
                    }
                    break;
                case DPAD_DOWN:
                    if (CanWeaponSwitch((pcItem)selectedSword[WS_DOWN])) {
                            nmh_sdk::SetEquip((pcItem)selectedSword[WS_DOWN], false);
                            weaponSwitchCooldown = 0;
                            directionPressed = WS_DOWN; // TSUBAKI_MK2
                    }
                    break;
                case DPAD_UP:
                    if (CanWeaponSwitch((pcItem)selectedSword[WS_UP])) {
                            nmh_sdk::SetEquip((pcItem)selectedSword[WS_UP], false);
                            weaponSwitchCooldown = 0;
                            directionPressed = WS_UP; // BLOOD_BERRY
                    }
                    break;
                default:
                    break;
                }
                if (weaponSwitchCooldown == 0) { tryPlayAnimation = true; }
            }
        }

        // play an anim after the sword loads (eEqReadMax)
        // we have to keep trying on frame until it plays (IsPlayingSwordChangeAnim)
        int motID = player->mCharaStatus.motionNo;
        if (weaponSwitchCooldown > weaponSwitchLockedFrames) {
            tryPlayAnimation = false;
        }
        if (tryPlayAnimation && !IsPlayingSwordChangeAnim(motID)) {
            if (/*previousSwordEquipRead*/ player->mPcStatus.equip[0].readProc == eEqReadMax) {
                switch (player->mPcStatus.equip[0].id) {
                case BLOOD_BERRY:
                case BLOOD_BERRY_BATTERY:
                case BLOOD_BERRY_BATTERY_DAMAGE:
                    nmh_sdk::PlayMotion(ePcMtBattou01, 0, 70, 1, 0.1f); // 249
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK3:
                case TSUBAKI_MK3_BATTERY:
                case TSUBAKI_MK3_BATTERY_DAMAGE:
                    nmh_sdk::PlayMotion(ePcMtBattou02Ed, 0, 0, 1, 0.1f); // 297
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK1:
                case TSUBAKI_MK1_BATTERY:
                case TSUBAKI_MK1_BATTERY_DAMAGE:
                    nmh_sdk::PlayMotion(ePcMtBattou03Ed, 0, 0, 1, 0.1f); // 345
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                case TSUBAKI_MK2:
                case TSUBAKI_MK2_BATTERY:
                case TSUBAKI_MK2_BATTERY_DAMAGE:
                    nmh_sdk::PlayMotion(ePcMtBattou04Ed, 0, 0, 1, 0.1f); // 391
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                default:
                    //nmh_sdk::PlayMotion(ePcMtBtIdl0, 0, 0, 0, 0.1f); // 3
                    break;
                }
                //nmh_sdk::PlayCamMotFromCharMot(5, 1, true, false, true);
                if (IsPlayingSwordChangeAnim(motID)) {
                    tryPlayAnimation = false;
                }
            }
        }

        // effectively pause the game while loading a sword. Optional but should be safer
        // Can't pause all because then the loading process pauses too
        // Of course this lets you move during cutscenes I am so dumb
        /*if (player->mInputMode == ePcInputBattleIdle) {
            if (player->mPcStatus.equip[0].readProc != eEqReadMax) {
                player->mPauseNpc = true;
                player->mOperate = false;
            }
            else {
                player->mPauseNpc = false;
                player->mOperate = true;
            }
        }*/
        // previousSwordEquipRead = player->mPcStatus.equip[0].readProc;
        weaponSwitchCooldown++;
        //if (weapon_switcher_ui) {
        Display_UI();
        //}
    }
}

// will show up in debug window, dump ImGui widgets you want here
//void WeaponSwitcher::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here

std::optional<std::string> WeaponSwitcher::on_initialize() {
    /*if (!install_hook_offset(0x3DC561, m_hook1, &detour1, &WeaponSwitcher::jmp_ret1, 8)) { // play weapon anims // player in ecx // called last
        spdlog::error("Failed to init WeaponSwitcher mod 1\n");
        return "Failed to init WeaponSwitcher mod 1";
    }

    if (!install_hook_offset(0x3D905B, m_hook2, &detour2, &WeaponSwitcher::jmp_ret2, 8)) { // play weapon anims // player in esi
        spdlog::error("Failed to init WeaponSwitcher mod 2\n");
        return "Failed to init WeaponSwitcher mod 2";
    }*/

    detour3_je = (g_framework->get_module().as<uintptr_t>() + 0x3D4806);
    if (!install_hook_offset(0x3D47C4, m_hook3, &detour3, &WeaponSwitcher::jmp_ret3, 5)) { // add tsubaki mk1 weapon swap to canAttack ban list
        spdlog::error("Failed to init WeaponSwitcher mod 3\n");
        return "Failed to init WeaponSwitcher mod 3";
    }

    load_weapon_switcher_texture();

    return Mod::on_initialize();
}

// during load
void WeaponSwitcher::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("weapon_switcher").value_or(false);
    if (mod_enabled) toggleForceMap(mod_enabled);
    savedSword[WS_LEFT] = cfg.get<int>("savedSword[0]").value_or(TSUBAKI_MK1);
    savedSword[WS_DOWN] = cfg.get<int>("savedSword[1]").value_or(TSUBAKI_MK2);
    savedSword[WS_RIGHT] = cfg.get<int>("savedSword[2]").value_or(TSUBAKI_MK3);
    savedSword[WS_UP] = cfg.get<int>("savedSword[3]").value_or(BLOOD_BERRY);
}

// during save
void WeaponSwitcher::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("weapon_switcher", mod_enabled);
    for (int i = 0; i < 4; i++) {
        cfg.set<int>(("savedSword[" + std::to_string(i) + "]"), selectedSword[i]);
    }
}

#endif
