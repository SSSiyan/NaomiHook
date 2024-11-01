#include "StageWarp.hpp"
#if 1

std::array<StageWarp::Stage, 78> StageWarp::stage_items = { // used elsewhere so part of StageWarp
    Stage {"STG000",   69, "Santa Destroy Overworld"},
    Stage {"STG0001",  69, "Beach Boss (Holly Summers)"},
    Stage {"STG0002",  69, "Holly Summers Zako"},
    Stage {"STG0003",  69, "Henry Fight True Ending"},
    Stage {"STG0004",  69, "Free Fight 3"},
    Stage {"STG0007",  69, "Bar Plastic Model Lovikov"},
    Stage {"STG0008",  69, "Cut Bike Race?"},
    Stage {"STG0009",  69, "Cut Bike Race? 2"},
    Stage {"STG00010", 69, "Cut Bike Race? 3"},
    Stage {"STG010",   69, "Shinobu Zako 1"},
    Stage {"STG00011", 69, "Motel Exterior"},
    Stage {"STG011",   69, "Shinobu Zako 2"},
    Stage {"STG012",   69, "Shinobu Zako 3"},
    Stage {"STG00013", 69, "City Street"},
    Stage {"STG013",   69, "Shinobu Boss Fight"},
    Stage {"STG00014", 69, "Speedbuster Zako"},
    Stage {"STG00015", 69, "Beach Spawn"},
    Stage {"STG00016", 69, "Homes Near Beach"},
    Stage {"STG00017", 69, "Northern Island Spawn"},
    Stage {"STG00018", 69, "Free Fight Basketball"},
    Stage {"STG00019", 69, "Northern Island Spawn"},
    Stage {"STG00020", 69, "Free Fight 5"},
    Stage {"STG020",   69, "Bad Girl Fight"},
    Stage {"STG020J",  69, "Bad Girl Fight JP"},
    Stage {"STG00021", 69, "Beach Spawn"},
    Stage {"STG021",   69, "Bad Girl Zako"},
    Stage {"STG021J",  69, "Bad Girl Zako JP"},
    Stage {"STG022",   69, "Bad Girl Hall"},
    Stage {"STG023",   69, "Bad Girl ZAKO DEBUG"},
    Stage {"STG023J",  69, "Bad Girl ZAKO DEBUG"},
    Stage {"STG024",   69, "Bad Girl ZAKO DEBUG 2"},
    Stage {"STG024J",  69, "Bad Girl ZAKO DEBUG 2"},
    Stage {"STG030",   69, "Destroyman Zako 2"},
    Stage {"STG030T",  69, "Destroyman TGS Zako"},
    Stage {"STG031",   69, "Destroyman Fight"},
    Stage {"STG031T",  69, "Destroyman TGS Fight"},
    Stage {"STG041",   69, "Dr.Peace Zako 1"},
    Stage {"STG042",   69, "Dr.Peace Fight"},
    Stage {"STG051",   69, "Letz Shake Zako"},
    Stage {"STG060",   69, "SpeedBuster"},
    Stage {"STG080",   69, "Deathmetal Zako 2"},
    Stage {"STG081",   69, "Deathmetal Fight"},
    Stage {"STG083",   69, "Deathmetal Zako 1"},
    Stage {"STG084",   69, "Deathmetal Sylvia Call"},
    Stage {"STG090",   69, "Harvey Fight"},
    Stage {"STG100",   69, "Darkstar Bike Zako"},
    Stage {"STG101",   69, "Darkstar Zako"},
    Stage {"STG103",   69, "Darkstar/Jeane Fight"},
    Stage {"STG170",   69, "Destroyman Zako 1"},
    Stage {"STG0411",  69, "Dr.Peace Zako 3"},
    Stage {"STG0412",  69, "Dr.Peace Zako 2"},
    Stage {"STG0421",  69, "Stadium Free Fight"},
    Stage {"STG0422",  69, "???"},
    Stage {"STG500",   69, "Motel"},
    Stage {"STG500US", 69, "Motel"},
    Stage {"STG510",   69, "Beefhead Videos"},
    Stage {"STG520",   69, "K-Entertainment"},
    Stage {"STG540",   69, "Job Center"},
    Stage {"STG550",   69, "Evolution Gym"},
    Stage {"STG560",   69, "Naomi's Lab"},
    Stage {"STG570",   69, "Area51"},
    Stage {"STG580",   69, "???"},
    Stage {"STG1702",  69, "Train"},
    Stage {"STG1703",  69, "Train Station (Boarding)"},
    Stage {"STG1707",  69, "Train Station Exit"},
    Stage {"STG1708",  69, "Train Station Harvey (Boarding)"},
    Stage {"STG1709",  69, "Train Station Harvey (Exit)"},
    Stage {"STG9000",  69, "DeathMetal Toilet"},
    Stage {"STG9001",  69, "Darkstar Toilet"},
    Stage {"STG9002",  69, "Badgirl Toilet Stadium Basement"},
    Stage {"STG9003",  69, "Speedbuster Toilet Speed City"},
    Stage {"STG9004",  69, "Harvey Toilet"},
    Stage {"STG9005",  69, "Letz Shake Toilet Senton Splash Tunnel"},
    Stage {"STG9006",  69, "Holly Toilet Beach"},
    Stage {"STG9007",  69, "Destroyman Toilet Bear Hug"},
    Stage {"STG9007T", 69, "Destroyman Toilet TGS"},
    Stage {"STG9008",  69, "Shinobu Toilet High School"},
    Stage {"STG9009",  69, "Dr.Peace Toilet Stadium"},
};

static constexpr std::array<StageWarp::Stage, 10> boss_stages = {
    StageWarp::Stage {"STG081",  69, "Deathmetal"},
    StageWarp::Stage {"STG042",  69, "Dr.Peace"},
    StageWarp::Stage {"STG013",  69, "Shinobu"},
    StageWarp::Stage {"STG031",  69, "Destroyman"},
    StageWarp::Stage {"STG0001", 69, "Holly Summers"},
    StageWarp::Stage {"STG090",  69, "Harvey"},
    StageWarp::Stage {"STG060",  69, "SpeedBuster"},
    StageWarp::Stage {"STG020",  69, "Bad Girl"},
    StageWarp::Stage {"STG103",  69, "Darkstar/Jeane"},
    StageWarp::Stage {"STG0003", 69, "Henry"},
};

static constexpr std::array<StageWarp::Stage, 17> zako_stages = {
    StageWarp::Stage {"STG083",   69, "Deathmetal Zako 1"},
    StageWarp::Stage {"STG080",   69, "Deathmetal Zako 2"},
    StageWarp::Stage {"STG041",   69, "Dr.Peace Zako 1"},
    StageWarp::Stage {"STG0412",  69, "Dr.Peace Zako 2"},
    StageWarp::Stage {"STG0411",  69, "Dr.Peace Zako 3"},
    StageWarp::Stage {"STG010",   69, "Shinobu Zako 1"},
    StageWarp::Stage {"STG011",   69, "Shinobu Zako 2"},
    StageWarp::Stage {"STG012",   69, "Shinobu Zako 3"},
    StageWarp::Stage {"STG170",   69, "Destroyman Zako 1"},
    StageWarp::Stage {"STG030",   69, "Destroyman Zako 2"},
    StageWarp::Stage {"STG0002",  69, "Holly Summers Zako"},
    StageWarp::Stage {"STG051",   69, "Letz Shake Zako"},
    StageWarp::Stage {"STG1708",   69, "Harvey Zako"},
    StageWarp::Stage {"STG00014", 69, "Speedbuster Zako"},
    StageWarp::Stage {"STG021",   69, "Bad Girl Zako"},
    StageWarp::Stage {"STG100",   69, "Darkstar Bike Zako"},
    StageWarp::Stage {"STG101",   69, "Darkstar Zako"},
};

static constexpr std::array<StageWarp::Stage, 8> city_stages = {
    StageWarp::Stage {"STG500",  69, "Motel"},
    StageWarp::Stage {"STG510",  69, "Beefhead Videos"},
    StageWarp::Stage {"STG520",  69, "K-Entertainment"},
    StageWarp::Stage {"STG540",  69, "Job Center"},
    StageWarp::Stage {"STG0007", 69, "Bar Plastic Model Lovikov"},
    StageWarp::Stage {"STG550",  69, "Evolution Gym"},
    StageWarp::Stage {"STG560",  69, "Naomi's Lab"},
    StageWarp::Stage {"STG570",  69, "Area51"},
};

static constexpr std::array<StageWarp::Stage, 14> wii_stages = {
    StageWarp::Stage {"STG0008",  69, "Cut Bike Race?"},
    StageWarp::Stage {"STG0009",  69, "Cut Bike Race? 2"},
    StageWarp::Stage {"STG00010", 69, "Cut Bike Race? 3"},
    StageWarp::Stage {"STG020J",  69, "Bad Girl Fight JP"},
    StageWarp::Stage {"STG021J",  69, "Bad Girl Zako JP"},
    StageWarp::Stage {"STG023",   69, "Bad Girl ZAKO DEBUG"},
    StageWarp::Stage {"STG023J",  69, "Bad Girl ZAKO DEBUG"},
    StageWarp::Stage {"STG024",   69, "Bad Girl ZAKO DEBUG 2"},
    StageWarp::Stage {"STG024J",  69, "Bad Girl ZAKO DEBUG 2"},
    StageWarp::Stage {"STG030T",  69, "Destroyman TGS Zako"},
    StageWarp::Stage {"STG031T",  69, "Destroyman TGS Fight"},
    StageWarp::Stage {"STG9007T", 69, "Destroyman Toilet TGS"},
    StageWarp::Stage {"STG500US", 69, "Motel"},
    StageWarp::Stage {"STG580",   69, "???"},
};

static constexpr std::array<StageWarp::Stage, 18> misc_stages = {
    StageWarp::Stage {"STG000",   69, "Santa Destroy Overworld"},
    StageWarp::Stage {"STG0004",  69, "Free Fight 3"},
    StageWarp::Stage {"STG00011", 69, "Motel Exterior"},
    StageWarp::Stage {"STG00013", 69, "City Street"},
    StageWarp::Stage {"STG00015", 69, "Beach Spawn"},
    StageWarp::Stage {"STG00016", 69, "Homes Near Beach"},
    StageWarp::Stage {"STG00017", 69, "Northern Island Spawn"},
    StageWarp::Stage {"STG00018", 69, "Free Fight Basketball"},
    StageWarp::Stage {"STG00019", 69, "Northern Island Spawn"},
    StageWarp::Stage {"STG00020", 69, "Free Fight 5"},
    StageWarp::Stage {"STG00021", 69, "Beach Spawn"},
    StageWarp::Stage {"STG084",   69, "Deathmetal Sylvia Call"},
    StageWarp::Stage {"STG0421",  69, "Stadium Free Fight"},
    StageWarp::Stage {"STG0422",  69, "???"},
    StageWarp::Stage {"STG1702",  69, "Train"},
    StageWarp::Stage {"STG1703",  69, "Train Station (Boarding)"},
    StageWarp::Stage {"STG1707",  69, "Train Station Exit"},
    StageWarp::Stage {"STG1709",  69, "Train Station Harvey (Exit)"},
};

static constexpr std::array<StageWarp::Stage, 10> save_stages = {
    StageWarp::Stage {"STG9000",  69, "DeathMetal Toilet"},
    StageWarp::Stage {"STG9001",  69, "Darkstar Toilet"},
    StageWarp::Stage {"STG9002",  69, "Badgirl Toilet"},
    StageWarp::Stage {"STG9003",  69, "Speedbuster Toilet"},
    StageWarp::Stage {"STG9004",  69, "Harvey Toilet"},
    StageWarp::Stage {"STG9005",  69, "Letz Shake Toilet"},
    StageWarp::Stage {"STG9006",  69, "Holly Toilet"},
    StageWarp::Stage {"STG9007",  69, "Destroyman Toilet"},
    StageWarp::Stage {"STG9008",  69, "Shinobu Toilet"},
    StageWarp::Stage {"STG9009",  69, "Dr.Peace Toilet"},
};

std::optional<std::string> StageWarp::on_initialize() {
    return Mod::on_initialize();
}

static int setStageArgs[7]{0, -1, -1, 0, 0, 0, 0};
static int64_t inSetVolRateArg = 0;

template <size_t N> // ??
void DisplayStageSection(const char* headerName, const std::array<StageWarp::Stage, N>& stages) {
    if (ImGui::CollapsingHeader(headerName)) {
        for (size_t i = 0; i < stages.size(); ++i) {
            char buttonLabel[64];
            snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", stages[i].name, stages[i].info);
            if (ImGui::Button(buttonLabel)) {
                nmh_sdk::SetStage(stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                                  setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
            }
        }
    }
}

void StageWarp::on_draw_ui() {
    if (nmh_sdk::get_CBgCtrl()) {
        char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName;
        if (currentStage && (strlen(currentStage) < 20)) {
            ImGui::Text("Current Stage: %s", nmh_sdk::get_CBgCtrl()->m_NowStageName);
        } else {
            ImGui::Text("Current Stage: ?");
        }

        static const char* argsHelpMarker("These args are exposed so we can figure out if there's a way to make more warps possible without crashing.\n"
            "The warps you've been using up to this point have left all of these values at 0.\n"
            "I have recently edited _Arg1 and _Arg2 to -1, so let me know if there are more issues than usual");

        if (ImGui::CollapsingHeader("SetStage args")) {
            help_marker(argsHelpMarker);
            ImGui::InputInt("AddedStages", &setStageArgs[0]);
            help_marker("I think this adds n to stageID? Not sure how else it would get next stage\nMotel>Overworld has this set to 2");
            ImGui::InputInt("_Arg1", &setStageArgs[1]);
            help_marker("Motel>Overworld sets this to -1");
            ImGui::InputInt("_Arg2", &setStageArgs[2]);
            help_marker("Motel>Overworld sets this to -1");
            ImGui::InputInt("inBossInfoDisp", &setStageArgs[3]);
            help_marker("Shows the boss popup");
            ImGui::InputInt("inFadeType", &setStageArgs[4]);
            help_marker("Sets the transition that plays, e.g. a black fade or stickers");
            ImGui::InputScalar("inSetVolRate", ImGuiDataType_S64, &inSetVolRateArg);
            ImGui::InputInt("inPause", &setStageArgs[5]);
            ImGui::InputInt("a10", &setStageArgs[6]);
        }
        else {
            help_marker(argsHelpMarker);
        }

        DisplayStageSection("All", StageWarp::stage_items);
        DisplayStageSection("Bosses", boss_stages);
        DisplayStageSection("Zako", zako_stages);
        DisplayStageSection("City Interiors", city_stages);
        DisplayStageSection("Wii/Unused", wii_stages);
        DisplayStageSection("Miscellaneous", misc_stages);
        DisplayStageSection("Toilets", save_stages);
    }
}

// during load
//void StageWarp::on_config_load(const utility::Config &cfg) {}
// during save
//void StageWarp::on_config_save(utility::Config &cfg) {}
// do something every frame
//void StageWarp::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void StageWarp::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
