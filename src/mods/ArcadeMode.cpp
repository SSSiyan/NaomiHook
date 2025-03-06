#if 1
#include "ArcadeMode.hpp"
#include "QuickBoot.hpp" // Quick boot uses this detour

bool ArcadeMode::mod_enabled = false;
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

// cba to add hash lookups at this moment, sorry che
static const char* get_next_room_by_name(const char* currentName) {
    if (QuickBoot::mod_enabled) {
        if (_stricmp(QuickBoot::originalBootStage, currentName) == 0) {
            return QuickBoot::stage_names[QuickBoot::newBootStageIndex];
        }
    }

    if (ArcadeMode::mod_enabled) {
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
        cmp byte ptr [ArcadeMode::mod_enabled], 1
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

void ArcadeMode::on_draw_ui() {
    ImGui::Checkbox("Arcade Mode", &mod_enabled);
    help_marker("Enable this option in the Motel then exit through the door to begin.\n"
        "To function flawlessly this feature currently requires a savegame with no story progress or you will get stuck. We're working on it!\n"
        "For now if you get stuck you can teleport to a new area in Stage Warp while this is ticked to jump to the next part.");
}

// during load
void ArcadeMode::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("arcade_mode").value_or(false);
}
// during save
void ArcadeMode::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("arcade_mode", mod_enabled);
}

// do something every frame
//void ArcadeMode::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ArcadeMode::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
