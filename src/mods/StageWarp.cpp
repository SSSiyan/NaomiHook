#include "StageWarp.hpp"
#if 1

struct Stage {
    const char* name;
    int id;
    const char* info;
};

static constexpr std::array<Stage, 78> stage_items = {
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

static constexpr std::array<Stage, 10> boss_stages = {
    Stage {"STG081",  69, "Deathmetal"},
    Stage {"STG042",  69, "Dr.Peace"},
    Stage {"STG013",  69, "Shinobu"},
    Stage {"STG031",  69, "Destroyman"},
    Stage {"STG0001", 69, "Holly Summers"},
    Stage {"STG090",  69, "Harvey"},
    Stage {"STG060",  69, "SpeedBuster"},
    Stage {"STG020",  69, "Bad Girl"},
    Stage {"STG103",  69, "Darkstar/Jeane"},
    Stage {"STG0003", 69, "Henry"},
};

static constexpr std::array<Stage, 17> zako_stages = {
    Stage {"STG083",   69, "Deathmetal Zako 1"},
    Stage {"STG080",   69, "Deathmetal Zako 2"},
    Stage {"STG041",   69, "Dr.Peace Zako 1"},
    Stage {"STG0412",  69, "Dr.Peace Zako 2"},
    Stage {"STG0411",  69, "Dr.Peace Zako 3"},
    Stage {"STG010",   69, "Shinobu Zako 1"},
    Stage {"STG011",   69, "Shinobu Zako 2"},
    Stage {"STG012",   69, "Shinobu Zako 3"},
    Stage {"STG170",   69, "Destroyman Zako 1"},
    Stage {"STG030",   69, "Destroyman Zako 2"},
    Stage {"STG0002",  69, "Holly Summers Zako"},
    Stage {"STG051",   69, "Letz Shake Zako"},
    Stage{"STG1708",   69, "Harvey Zako"},
    Stage {"STG00014", 69, "Speedbuster Zako"},
    Stage {"STG021",   69, "Bad Girl Zako"},
    Stage {"STG100",   69, "Darkstar Bike Zako"},
    Stage {"STG101",   69, "Darkstar Zako"},

};

static constexpr std::array<Stage, 8> city_stages = {
    Stage {"STG500",  69, "Motel"},
    Stage {"STG510",  69, "Beefhead Videos"},
    Stage {"STG520",  69, "K-Entertainment"},
    Stage {"STG540",  69, "Job Center"},
    Stage {"STG0007", 69, "Bar Plastic Model Lovikov"},
    Stage {"STG550",  69, "Evolution Gym"},
    Stage {"STG560",  69, "Naomi's Lab"},
    Stage {"STG570",  69, "Area51"},

};

static constexpr std::array<Stage, 14> wii_stages = {
    Stage {"STG0008",  69, "Cut Bike Race?"},
    Stage {"STG0009",  69, "Cut Bike Race? 2"},
    Stage {"STG00010", 69, "Cut Bike Race? 3"},
    Stage {"STG020J",  69, "Bad Girl Fight JP"},
    Stage {"STG021J",  69, "Bad Girl Zako JP"},
    Stage {"STG023",   69, "Bad Girl ZAKO DEBUG"},
    Stage {"STG023J",  69, "Bad Girl ZAKO DEBUG"},
    Stage {"STG024",   69, "Bad Girl ZAKO DEBUG 2"},
    Stage {"STG024J",  69, "Bad Girl ZAKO DEBUG 2"},
    Stage {"STG030T",  69, "Destroyman TGS Zako"},
    Stage {"STG031T",  69, "Destroyman TGS Fight"},
    Stage {"STG9007T", 69, "Destroyman Toilet TGS"},
    Stage {"STG500US", 69, "Motel"},
    Stage {"STG580",   69, "???"},

};

static constexpr std::array<Stage, 18> misc_stages = {
    Stage {"STG000",   69, "Santa Destroy Overworld"},
    Stage {"STG0004",  69, "Free Fight 3"},
    Stage {"STG00011", 69, "Motel Exterior"},
    Stage {"STG00013", 69, "City Street"},
    Stage {"STG00015", 69, "Beach Spawn"},
    Stage {"STG00016", 69, "Homes Near Beach"},
    Stage {"STG00017", 69, "Northern Island Spawn"},
    Stage {"STG00018", 69, "Free Fight Basketball"},
    Stage {"STG00019", 69, "Northern Island Spawn"},
    Stage {"STG00020", 69, "Free Fight 5"},
    Stage {"STG00021", 69, "Beach Spawn"},
    Stage {"STG084",   69, "Deathmetal Sylvia Call"},
    Stage {"STG0421",  69, "Stadium Free Fight"},
    Stage {"STG0422",  69, "???"},
    Stage {"STG1702",  69, "Train"},
    Stage {"STG1703",  69, "Train Station (Boarding)"},
    Stage {"STG1707",  69, "Train Station Exit"},
    Stage {"STG1709",  69, "Train Station Harvey (Exit)"},

};

static constexpr std::array<Stage, 10> save_stages = {
    Stage {"STG9000",  69, "DeathMetal Toilet"},
    Stage {"STG9001",  69, "Darkstar Toilet"},
    Stage {"STG9002",  69, "Badgirl Toilet"},
    Stage {"STG9003",  69, "Speedbuster Toilet"},
    Stage {"STG9004",  69, "Harvey Toilet"},
    Stage {"STG9005",  69, "Letz Shake Toilet"},
    Stage {"STG9006",  69, "Holly Toilet"},
    Stage {"STG9007",  69, "Destroyman Toilet"},
    Stage {"STG9008",  69, "Shinobu Toilet"},
    Stage {"STG9009",  69, "Dr.Peace Toilet"},

};

std::optional<std::string> StageWarp::on_initialize() {
    return Mod::on_initialize();
}

void StageWarp::on_draw_ui() {
    if (nmh_sdk::get_CBgCtrl()) {
        char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName;
        if (currentStage && (strlen(currentStage) < 20)) {
            ImGui::Text("Current Stage: %s", nmh_sdk::get_CBgCtrl()->m_NowStageName);
        } else {
            ImGui::Text("Current Stage: ?");
        }
        help_marker("These args are exposed so we can figure out if there's a way to make more warps possible without crashing.\n"
        "The warps you've been using up to this point have left all of these values at 0.");

        static int setStageArgs[7]{};
        static int64_t inSetVolRateArg = 0;
        if (ImGui::CollapsingHeader("SetStage args")) {
            ImGui::InputInt("AddedStages", &setStageArgs[0]);
            help_marker("I think this adds n to stageID? Not sure how else it would get next stage");
            ImGui::InputInt("_Arg1", &setStageArgs[1]);
            ImGui::InputInt("_Arg2", &setStageArgs[2]);
            ImGui::InputInt("inBossInfoDisp", &setStageArgs[3]);
            help_marker("Shows the boss popup");
            ImGui::InputInt("inFadeType", &setStageArgs[4]);
            help_marker("Sets the transition that plays, e.g. a black fade or stickers");
            ImGui::InputScalar("inSetVolRate", ImGuiDataType_S64, &inSetVolRateArg);
            ImGui::InputInt("inPause", &setStageArgs[5]);
            ImGui::InputInt("a10", &setStageArgs[6]);
        }

        if (ImGui::CollapsingHeader("All")) {
            for (int i = 0; i < stage_items.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", stage_items[i].name, stage_items[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(stage_items[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2],
                        setStageArgs[3], setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("Bosses")) {
            for (int i = 0; i < boss_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", boss_stages[i].name, boss_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(boss_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2],
                        setStageArgs[3], setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("Zako")) {
            for (int i = 0; i < zako_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", zako_stages[i].name, zako_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(zako_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                        setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("City Interiors")) {
            for (int i = 0; i < city_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", city_stages[i].name, city_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(city_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                        setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("Wii/Unused")) {
            for (int i = 0; i < wii_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", wii_stages[i].name, wii_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(wii_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                        setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("Miscellaneous")) {
            for (int i = 0; i < misc_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", misc_stages[i].name, misc_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(misc_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                        setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }

        if (ImGui::CollapsingHeader("Toilets")) {
            for (int i = 0; i < save_stages.size(); ++i) {
                char buttonLabel[64];
                snprintf(buttonLabel, sizeof(buttonLabel), "Warp to %s: %s", save_stages[i].name, save_stages[i].info);
                if (ImGui::Button(buttonLabel)) {
                    nmh_sdk::SetStage(save_stages[i].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                        setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                }
            }
        }
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
