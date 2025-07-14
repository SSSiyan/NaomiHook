#if 1
#include "ArcadeMode.hpp"
#include "QuickBoot.hpp" // Quick boot uses this detour
#include <shlobj.h> // for ShellExecuteA
#include "ClothesSwitcher.hpp" // for pcItem names
#include "PlayerTracker.hpp" // for throw names

const char* ArcadeMode::defaultDescription = "Play through the entire game in one sitting while skipping all the stuff in between. Nothing but gameplay in this mode.";
const char* ArcadeMode::hoveredDescription = defaultDescription;

bool ArcadeMode::arcade_enabled = false;
bool ArcadeMode::boss_rush_enabled = false;

uintptr_t ArcadeMode::jmp_ret1 = NULL;
uintptr_t ArcadeMode::gpBattle = NULL;
uintptr_t ArcadeMode::mSetVisible = NULL;

static constexpr std::array<const char*, 28> arcadeMode = {
    "STG500",   // Motel
    "STG083",   // Deathmetal Zako 1
    "STG080",   // Deathmetal Zako 2
    "STG081",   // Deathmetal
    "STG041",   // Dr.Peace Zako 1
    "STG0412",  // Dr.Peace Zako 2
    "STG0411",  // Dr.Peace Zako 3
    "STG042",   // Dr.Peace
    "STG010",   // Shinobu Zako 1
    "STG011",   // Shinobu Zako 2
    "STG012",   // Shinobu Zako 3
    "STG013",   // Shinobu
    "STG170",   // Destroyman Zako 1
    "STG030",   // Destroyman Zako 2 // DOOR STUCK
    "STG031",   // Destroyman
    "STG0002",  // Holly Summers Zako
    "STG0001",  // Holly Summers
    "STG051",   // Letz Shake Zako
    "STG1708",  // Harvey Zako
    "STG090",   // Harvey
    "STG00014", // Speedbuster Zako
    "STG060",   // SpeedBuster
    "STG021",   // Bad Girl Zako
    "STG020",   // Bad Girl
    "STG101",   // Darkstar Zako
    "STG103",   // Darkstar/Jeane
    "STG0003",  // Henry
};

static constexpr std::array<const char*, 11> bossRushMode = {
    "STG500",   // Motel
    "STG081",   // Deathmetal
    "STG042",   // Dr.Peace
    "STG013",   // Shinobu
    "STG031",   // Destroyman
    "STG0001",  // Holly Summers
    "STG090",   // Harvey
    "STG060",   // SpeedBuster
    "STG020",   // Bad Girl
    "STG103",   // Darkstar/Jeane
    "STG0003",  // Henry
};

// cba to add hash lookups at this moment, sorry che
static const char* get_next_room_by_name(const char* currentName) {
    if (QuickBoot::mod_enabled) {
        if (_stricmp(QuickBoot::originalBootStage, currentName) == 0) {
            return QuickBoot::stage_names[QuickBoot::newBootStageIndex];
        }
    }

    if (ArcadeMode::arcade_enabled) {
        auto it = std::find_if(arcadeMode.begin(), arcadeMode.end(),
            [&currentName](const char* room) {
                return room != nullptr && _stricmp(room, currentName) == 0;
            });

        if (it != arcadeMode.end()) {
            auto next_it = std::next(it);
            if (next_it == arcadeMode.end()) {
                next_it = arcadeMode.begin();
            }
            return *next_it;
        }
    }

    if (ArcadeMode::boss_rush_enabled) {
        auto it = std::find_if(bossRushMode.begin(), bossRushMode.end(),
            [&currentName](const char* room) {
                return room != nullptr && _stricmp(room, currentName) == 0;
            });

        if (it != bossRushMode.end()) {
            auto next_it = std::next(it);
            if (next_it == bossRushMode.end()) {
                next_it = bossRushMode.begin();
            }
            return *next_it;
        }
    }

    return nullptr;
}

const char* GetNextStage() {
    if (const char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName){
        return get_next_room_by_name(currentStage);
    }
}

// clang-format off
naked void detour1() {
    __asm {
        cmp byte ptr [ArcadeMode::arcade_enabled], 1
        je newcode
        cmp byte ptr [ArcadeMode::boss_rush_enabled], 1
        je newcode
        cmp byte ptr [QuickBoot::mod_enabled], 1
        je newcode
        jmp originalcode

    newcode:
        push eax // 4
        push ecx // 8
        push edx // C
        mov ecx, [ArcadeMode::gpBattle]
        mov ecx, [ecx]
        mov ecx, [ecx+0x164] // mHRPc
        test ecx, ecx
        je noStageEdit
        cmp byte ptr [ecx+0x29a2], 1 // if mDeadPause, do not edit teleport
        je noStageEdit
        mov byte ptr [ecx+0x1704], 0 // reset rouletteHitRate
        mov dword ptr [ecx+0x1780], 0 // reset ikasamaSlot
        push 1
        call dword ptr [ArcadeMode::mSetVisible] // set char visible after cutscenes
        call dword ptr GetNextStage // put nextStage* in eax
        test eax, eax
        je noStageEdit
        mov [esp+0xC+0x4], eax // stageName
        mov [esp+0xC+0x8], 0 // force stagesToAdd
    noStageEdit:
        pop edx
        pop ecx
        pop eax
    originalcode:
        push ebx
        mov ebx,esp
        sub esp, 8
        jmp dword ptr [ArcadeMode::jmp_ret1]
    }
}
// clang-format on

std::optional<std::string> ArcadeMode::on_initialize() {
    gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    mSetVisible = g_framework->get_module().as<uintptr_t>() + 0x3D6D90; 
    if (!install_hook_offset(0x3FD690, m_hook1, &detour1, &ArcadeMode::jmp_ret1, 6)) {
        spdlog::error("Failed to init ArcadeMode mod\n");
        return "Failed to init ArcadeMode mod";
    }
    return Mod::on_initialize();
}

void ArcadeMode::render_description() const {
    ImGui::TextWrapped(ArcadeMode::hoveredDescription);
}

bool BuyThing(int price) {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { return false; }
    if (player->mPcStatus.money < price) {
        return false;
    }
    nmh_sdk::SubPcMoney(price);
    return true;
}

// returns true if already owned
bool CheckIfPlayerAlreadyOwnsLockerItem(pcItem itemID) {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { return false; }
    // Check if player owns the item (either equipped or in locker)
    if (player->mPcStatus.equip[0].id == itemID || nmh_sdk::CheckLocker(itemID)) {
        return true;
    }
    return false;
}

// returns true if already owned
bool RenderShopLockerItem(pcItem itemID, int price) {
    bool isOwned = CheckIfPlayerAlreadyOwnsLockerItem(itemID);
    
    if (isOwned) {
        ImGui::BeginDisabled();
    }
    
    char buttonText[256];
    snprintf(buttonText, sizeof(buttonText), "%s ($%d)", clothing_items[itemID].name, price);
    
    if (ImGui::Button(buttonText)) {
        if (BuyThing(price)) {
            nmh_sdk::AddLocker(itemID);
        }
    }
    
    if (isOwned) {
        ImGui::EndDisabled();
        // ImGui::SameLine();
        // ImGui::Text("(Owned)");
    }
    
    return isOwned;
}

// returns true if already owned
bool RenderWrestlingUnlock(mHRPc* player, int wrestlingID, const char* wrestlingName, int price) {
    bool isOwned = player->mPcStatus.skillCatch[wrestlingID];
    
    if (isOwned) {
        ImGui::BeginDisabled();
    }
    
    char buttonText[256];
    snprintf(buttonText, sizeof(buttonText), "%s ($%d)", wrestlingNames[wrestlingID], price);
    
    if (ImGui::Button(buttonText)) {
        if (BuyThing(price)) {
            player->mPcStatus.skillCatch[wrestlingID] = true;
        }
    }
    
    if (isOwned) {
        ImGui::EndDisabled();
        // ImGui::SameLine();
        // ImGui::Text("(Owned)");
    }
    
    return isOwned;
}

// returns true if already owned
bool RenderK7Unlock(mHRPc* player, int id, const char* skillName, int price) {
    bool isOwned = player->mPcStatus.skillK7[id];
    
    if (isOwned) {
        ImGui::BeginDisabled();
    }
    
    char buttonText[256];
    snprintf(buttonText, sizeof(buttonText), "%s ($%d)", k7Names[id], price);
    
    if (ImGui::Button(buttonText)) {
        if (BuyThing(price)) {
            player->mPcStatus.skillK7[id] = true;
        }
    }
    
    if (isOwned) {
        ImGui::EndDisabled();
        // ImGui::SameLine();
        // ImGui::Text("(Owned)");
    }
    
    return isOwned;
}

// returns true if already owned
bool RenderCmbExtendUnlock(mHRPc* player, int section, const char* swordName, int price) {
    int baseIndex = section * 4;
    
    bool isOwned = true;
    for (int i = 0; i < 4; i++) {
        if (!player->mPcStatus.wepInfo[baseIndex + i].cmbExtend) {
            isOwned = false;
            break;
        }
    }
    
    if (isOwned) {
        ImGui::BeginDisabled();
    }
    
    char buttonText[256];
    snprintf(buttonText, sizeof(buttonText), "%s ($%d)", swordName, price);
    
    if (ImGui::Button(buttonText)) {
        if (BuyThing(price)) {
            for (int i = 0; i < 4; i++) {
                player->mPcStatus.wepInfo[baseIndex + i].cmbExtend = true;
            }
        }
    }
    
    if (isOwned) {
        ImGui::EndDisabled();
    }
    
    return isOwned;
}

void DisplayShop(mHRPc* player, bool toggle) {
    ImGui::Begin("Arcade Shop", &toggle);
    ImGui::Text("Current Money: $%i", player->mPcStatus.money);
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetWindowPos(ImVec2(windowSize.x * 0.535f, windowSize.y * 0.029f));
    ImGui::SetWindowSize(ImVec2(windowSize.x * 0.3f, windowSize.y * 0.7475f));
    if (ImGui::TreeNodeEx("Inventory", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (RenderShopLockerItem(BLOOD_BERRY, 100)) {
            if (RenderShopLockerItem(BLOOD_BERRY_DAMAGE, 35000)) {
                RenderShopLockerItem(BLOOD_BERRY_BATTERY_DAMAGE, 95000);
            }
        }

        if (RenderShopLockerItem(TSUBAKI_MK1, 98000)) {
            if (RenderShopLockerItem(TSUBAKI_MK1_DAMAGE, 129800)) {
                RenderShopLockerItem(TSUBAKI_MK1_BATTERY_DAMAGE, 129800);
            }
        }

        if (RenderShopLockerItem(TSUBAKI_MK2, 148000)) {
            if (RenderShopLockerItem(TSUBAKI_MK2_DAMAGE, 150000)) {
                RenderShopLockerItem(TSUBAKI_MK2_BATTERY_DAMAGE, 150000);
            }
        }

        if (RenderShopLockerItem(TSUBAKI_MK3, 498000)) {
            if (RenderShopLockerItem(TSUBAKI_MK3_DAMAGE, 200000)) {
                RenderShopLockerItem(TSUBAKI_MK3_BATTERY_DAMAGE, 999999);
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Wrestling", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
        static constexpr int num = 16;
        static constexpr int wrestlingPrices[num] = {
            200, // "Dragon Suplex",           
            200, // "Brain Buster",            
            200, // "default##2",              
            200, // "default##3",              
            35000, // "Hurricanrana",            
            35000, // "Power Bomb",              
            35000, // "Brain Buster Slam",       
            35000, // "Quebradora Con Giro",     
            30000, // "German Suplex",           
            30000, // "Tiger Suplex",            
            30000, // "Belly To Belly",          
            30000, // "Front Neck Chancery Drop",
            30000, // "Captured",                
            30000, // "Reverse Armsault",        
            30000, // "Double Arm Suplex",       
            30000  // "Double Wrist Armsault",   
        };
        for (int i = 0; i < num; ++i) {
            RenderWrestlingUnlock(player, i, wrestlingNames[i], wrestlingPrices[i]);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Skills", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
        static constexpr int num = 7;
        static constexpr int k7Prices[num] = {
            35000,   // "Memory of Demon - Jumping Slash",      
            2500,    // "Memory of Child - Sprint",             
            1000,    // "Memory of Three - Mini Map",           
            20000,   // "Memory of Woman - Darkside Extension", 
            15000,   // "Memory of Mask - Wrestling Grab Range",
            30000,   // "Memory of Tattoo - Total Rank Bonus",  
            10000    // "Memory of White - Jumping Down Attack",
        };
        for (int i = 0; i < num; ++i) {
            RenderK7Unlock(player, i, k7Names[i], k7Prices[i]);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Combo Extend Upgrade", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
        RenderCmbExtendUnlock(player, 0, "Blood Berry Combo Extend", 10000);
        RenderCmbExtendUnlock(player, 1, "Tsubaki Mk3 Combo Extend", 15000);
        RenderCmbExtendUnlock(player, 2, "Tsubaki Mk1 Combo Extend", 12000);
        RenderCmbExtendUnlock(player, 3, "Tsubaki Mk2 Combo Extend", 13000);
        ImGui::TreePop();
    }
    ImGui::End();
}

void ArcadeMode::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) ArcadeMode::hoveredDescription = defaultDescription;

    if (ImGui::Checkbox("Arcade Mode", &arcade_enabled)) {
        boss_rush_enabled = false;
    }
    if (ImGui::IsItemHovered()) ArcadeMode::hoveredDescription = "Enable this option in the Motel then exit through the door to begin.\n"
        "To function flawlessly this feature currently requires a savegame with no story progress or you will get stuck. We're working on it!\n"
        "For now if you get stuck you can teleport to a new area in Stage Warp while this is ticked to jump to the next part. Please be aware that if your game save was created on the latest, official NMH1 exe, the file for your data will be found in a different directory.";
    ImGui::Indent();
    if (ImGui::Button("Open Saved Games Folder")) {
        char savedGamesPath[MAX_PATH];
        if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, savedGamesPath) == S_OK) {
            std::string folderPath = std::string(savedGamesPath) + "\\Saved Games\\NMH\\SAVES";
            ShellExecuteA(NULL, "explore", folderPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
    }
    if (ImGui::IsItemHovered()) ArcadeMode::hoveredDescription = "If you want to temporarily reset your save:\n"
        "  - Rename the save found in this folder OR make a backup copy of it and delete the original\n"
        "  - Restart the game\n"
        "Once you're done, close the game and restore your save. The original name was hrSave.dat";
    ImGui::Unindent();
    if (ImGui::Checkbox("Boss Rush Mode", &boss_rush_enabled)) {
        arcade_enabled = false;
    }
    if (ImGui::IsItemHovered()) ArcadeMode::hoveredDescription = "Enable this option in the Motel then exit through the door to begin";

    static bool arcadeModeShopToggle = false;
    ImGui::Checkbox("Display Arcade Shop", &arcadeModeShopToggle);
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (arcadeModeShopToggle && player) {
        DisplayShop(player, arcadeModeShopToggle);

        static bool shouldDisplayDebugShop = false; // change this to false to hide the debug shop
        if (shouldDisplayDebugShop) {
            if (ImGui::CollapsingHeader("Debug Shop")) {
                if (ImGui::TreeNodeEx("Locker Items", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::Text("Current Money:");
                    ImGui::InputInt("##Current Money InputInt", &player->mPcStatus.money);
                    static int giveItemID = 0;
                    ImGui::Text("Desired Item ID:");
                    ImGui::InputInt("##Give Item InputInt", &giveItemID);
                    ImGui::Text(clothing_items[giveItemID].name);
                    if (ImGui::Button("Give Item")) {
                        nmh_sdk::AddLocker(pcItem(giveItemID));
                    }
                    bool alreadyOwnsItem = CheckIfPlayerAlreadyOwnsLockerItem(pcItem(giveItemID));
                    ImGui::Checkbox("Is this item in your locker or your hand?", &alreadyOwnsItem);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Wrestling Moves", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 16; i++) {
                        ImGui::Checkbox(wrestlingNames[i], &player->mPcStatus.skillCatch[i]);
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Skills", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 7; i++) {
                        ImGui::Checkbox(k7Names[i], &player->mPcStatus.skillK7[i]);
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("cmbExtend", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 16; i++) {
                        int id = player->mPcStatus.wepInfo[i].id;
                        ImGui::Checkbox(clothing_items[id].name, &player->mPcStatus.wepInfo[i].cmbExtend);
                    }
                    ImGui::TreePop();
                }
            }
        }
    }
}

// during load
void ArcadeMode::on_config_load(const utility::Config &cfg) {
    arcade_enabled = cfg.get<bool>("arcade_mode").value_or(false);
    boss_rush_enabled = cfg.get<bool>("boss_rush_mode").value_or(false);
}
// during save
void ArcadeMode::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("arcade_mode", arcade_enabled);
    cfg.set<bool>("boss_rush_mode", boss_rush_enabled);
}

// do something every frame
//void ArcadeMode::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ArcadeMode::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
