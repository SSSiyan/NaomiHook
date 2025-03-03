#include "StageWarp.hpp"
#if 1

static int setStageArgs[7]{0, -1, -1, 0, 0, 0, 0};
static int64_t inSetVolRateArg = 0;
const char* DefaultDescription = "Hover over a stage to see details.";
const char* StageWarp::hoveredDescription = DefaultDescription;

void StageWarp::render_description() const {
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    if (hoveredDescription != DefaultDescription) {
        ImGui::Button(hoveredDescription, ImVec2(availableSpace.y, availableSpace.y));
        ImGui::SameLine();
        ImGui::TextWrapped("%s", hoveredDescription);
    }
    else {
        ImGui::Button(hoveredDescription, ImVec2(availableSpace.y, availableSpace.y));
        ImGui::SameLine();
        ImGui::TextWrapped("%s", hoveredDescription);
    }
}

std::array<StageWarp::Stage, 78> StageWarp::stage_items = { 
    Stage {"STG000",   69, "Santa Destroy Overworld", ""},
    Stage {"STG0001",  69, "Beach Boss (Holly Summers)", ""},
    Stage {"STG0002",  69, "Holly Summers Zako", ""},
    Stage {"STG0003",  69, "Henry Fight True Ending", ""},
    Stage {"STG0004",  69, "Free Fight 3", ""},
    Stage {"STG0007",  69, "Bar Plastic Model Lovikov", ""},
    Stage {"STG0008",  69, "Cut Bike Race?", ""},
    Stage {"STG0009",  69, "Cut Bike Race? 2", ""},
    Stage {"STG00010", 69, "Cut Bike Race? 3", ""},
    Stage {"STG010",   69, "Shinobu Zako 1", ""},
    Stage {"STG00011", 69, "Motel Exterior", ""},
    Stage {"STG011",   69, "Shinobu Zako 2", ""},
    Stage {"STG012",   69, "Shinobu Zako 3", ""},
    Stage {"STG00013", 69, "City Street", ""},
    Stage {"STG013",   69, "Shinobu Boss Fight", ""},
    Stage {"STG00014", 69, "Speedbuster Zako", ""},
    Stage {"STG00015", 69, "Beach Spawn", ""},
    Stage {"STG00016", 69, "Homes Near Beach", ""},
    Stage {"STG00017", 69, "Northern Island Spawn", ""},
    Stage {"STG00018", 69, "Free Fight Basketball", ""},
    Stage {"STG00019", 69, "Northern Island Spawn", ""},
    Stage {"STG00020", 69, "Free Fight 5", ""},
    Stage {"STG020",   69, "Bad Girl Fight", ""},
    Stage {"STG020J",  69, "Bad Girl Fight JP", ""},
    Stage {"STG00021", 69, "Beach Spawn", ""},
    Stage {"STG021",   69, "Bad Girl Zako", ""},
    Stage {"STG021J",  69, "Bad Girl Zako JP", ""},
    Stage {"STG022",   69, "Bad Girl Hall", ""},
    Stage {"STG023",   69, "Bad Girl ZAKO DEBUG", ""},
    Stage {"STG023J",  69, "Bad Girl ZAKO DEBUG", ""},
    Stage {"STG024",   69, "Bad Girl ZAKO DEBUG 2", ""},
    Stage {"STG024J",  69, "Bad Girl ZAKO DEBUG 2", ""},
    Stage {"STG030",   69, "Destroyman Zako 2", ""},
    Stage {"STG030T",  69, "Destroyman TGS Zako", ""},
    Stage {"STG031",   69, "Destroyman Fight", ""},
    Stage {"STG031T",  69, "Destroyman TGS Fight", ""},
    Stage {"STG041",   69, "Dr.Peace Zako 1", ""},
    Stage {"STG042",   69, "Dr.Peace Fight", ""},
    Stage {"STG051",   69, "Letz Shake Zako", ""},
    Stage {"STG060",   69, "SpeedBuster", ""},
    Stage {"STG080",   69, "Deathmetal Zako 2", ""},
    Stage {"STG081",   69, "Deathmetal Fight", ""},
    Stage {"STG083",   69, "Deathmetal Zako 1", ""},
    Stage {"STG084",   69, "Deathmetal Sylvia Call", ""},
    Stage {"STG090",   69, "Harvey Fight", ""},
    Stage {"STG100",   69, "Darkstar Bike Zako", ""},
    Stage {"STG101",   69, "Darkstar Zako", ""},
    Stage {"STG103",   69, "Darkstar/Jeane Fight", ""},
    Stage {"STG170",   69, "Destroyman Zako 1", ""},
    Stage {"STG0411",  69, "Dr.Peace Zako 3", ""},
    Stage {"STG0412",  69, "Dr.Peace Zako 2", ""},
    Stage {"STG0421",  69, "Stadium Free Fight", ""},
    Stage {"STG0422",  69, "???", ""},
    Stage {"STG500",   69, "Motel", ""},
    Stage {"STG500US", 69, "Motel", ""},
    Stage {"STG510",   69, "Beefhead Videos", ""},
    Stage {"STG520",   69, "K-Entertainment", ""},
    Stage {"STG540",   69, "Job Center", ""},
    Stage {"STG550",   69, "Evolution Gym", ""},
    Stage {"STG560",   69, "Naomi's Lab", ""},
    Stage {"STG570",   69, "Area51", ""},
    Stage {"STG580",   69, "???", ""},
    Stage {"STG1702",  69, "Train", ""},
    Stage {"STG1703",  69, "Train Station (Boarding)", ""},
    Stage {"STG1707",  69, "Train Station Exit", ""},
    Stage {"STG1708",  69, "Train Station Harvey (Boarding)", ""},
    Stage {"STG1709",  69, "Train Station Harvey (Exit)", ""},
    Stage {"STG9000",  69, "DeathMetal Toilet", ""},
    Stage {"STG9001",  69, "Darkstar Toilet", ""},
    Stage {"STG9002",  69, "Badgirl Toilet Stadium Basement", ""},
    Stage {"STG9003",  69, "Speedbuster Toilet Speed City", ""},
    Stage {"STG9004",  69, "Harvey Toilet", ""},
    Stage {"STG9005",  69, "Letz Shake Toilet Senton Splash Tunnel", ""},
    Stage {"STG9006",  69, "Holly Toilet Beach", ""},
    Stage {"STG9007",  69, "Destroyman Toilet Bear Hug", ""},
    Stage {"STG9007T", 69, "Destroyman Toilet TGS", ""},
    Stage {"STG9008",  69, "Shinobu Toilet High School", ""},
    Stage {"STG9009",  69, "Dr.Peace Toilet Stadium", ""}
};

std::array<std::reference_wrapper<StageWarp::Stage>, 78> all_stages = {
    StageWarp::stage_items[0],  // Santa Destroy Overworld
    StageWarp::stage_items[1],  // Beach Boss (Holly Summers)
    StageWarp::stage_items[2],  // Holly Summers Zako
    StageWarp::stage_items[3],  // Henry Fight True Ending
    StageWarp::stage_items[4],  // Free Fight 3
    StageWarp::stage_items[5],  // Bar Plastic Model Lovikov
    StageWarp::stage_items[6],  // Cut Bike Race?
    StageWarp::stage_items[7],  // Cut Bike Race? 2
    StageWarp::stage_items[8],  // Cut Bike Race? 3
    StageWarp::stage_items[9],  // Shinobu Zako 1
    StageWarp::stage_items[10], // Motel Exterior
    StageWarp::stage_items[11], // Shinobu Zako 2
    StageWarp::stage_items[12], // Shinobu Zako 3
    StageWarp::stage_items[13], // City Street
    StageWarp::stage_items[14], // Shinobu Boss Fight
    StageWarp::stage_items[15], // Speedbuster Zako
    StageWarp::stage_items[16], // Beach Spawn
    StageWarp::stage_items[17], // Homes Near Beach
    StageWarp::stage_items[18], // Northern Island Spawn
    StageWarp::stage_items[19], // Free Fight Basketball
    StageWarp::stage_items[20], // Northern Island Spawn
    StageWarp::stage_items[21], // Free Fight 5
    StageWarp::stage_items[22], // Bad Girl Fight
    StageWarp::stage_items[23], // Bad Girl Fight JP
    StageWarp::stage_items[24], // Beach Spawn
    StageWarp::stage_items[25], // Bad Girl Zako
    StageWarp::stage_items[26], // Bad Girl Zako JP
    StageWarp::stage_items[27], // Bad Girl Hall
    StageWarp::stage_items[28], // Bad Girl ZAKO DEBUG
    StageWarp::stage_items[29], // Bad Girl ZAKO DEBUG
    StageWarp::stage_items[30], // Bad Girl ZAKO DEBUG 2
    StageWarp::stage_items[31], // Bad Girl ZAKO DEBUG 2
    StageWarp::stage_items[32], // Destroyman Zako 2
    StageWarp::stage_items[33], // Destroyman TGS Zako
    StageWarp::stage_items[34], // Destroyman Fight
    StageWarp::stage_items[35], // Destroyman TGS Fight
    StageWarp::stage_items[36], // Dr.Peace Zako 1
    StageWarp::stage_items[37], // Dr.Peace Fight
    StageWarp::stage_items[38], // Letz Shake Zako
    StageWarp::stage_items[39], // SpeedBuster
    StageWarp::stage_items[40], // Deathmetal Zako 2
    StageWarp::stage_items[41], // Deathmetal Fight
    StageWarp::stage_items[42], // Deathmetal Zako 1
    StageWarp::stage_items[43], // Deathmetal Sylvia Call
    StageWarp::stage_items[44], // Harvey Fight
    StageWarp::stage_items[45], // Darkstar Bike Zako
    StageWarp::stage_items[46], // Darkstar Zako
    StageWarp::stage_items[47], // Darkstar/Jeane Fight
    StageWarp::stage_items[48], // Destroyman Zako 1
    StageWarp::stage_items[49], // Dr.Peace Zako 3
    StageWarp::stage_items[50], // Dr.Peace Zako 2
    StageWarp::stage_items[51], // Stadium Free Fight
    StageWarp::stage_items[52], // ???
    StageWarp::stage_items[53], // Motel
    StageWarp::stage_items[54], // Motel
    StageWarp::stage_items[55], // Beefhead Videos
    StageWarp::stage_items[56], // K-Entertainment
    StageWarp::stage_items[57], // Job Center
    StageWarp::stage_items[58], // Evolution Gym
    StageWarp::stage_items[59], // Naomi's Lab
    StageWarp::stage_items[60], // Area51
    StageWarp::stage_items[61], // ???
    StageWarp::stage_items[62], // Train
    StageWarp::stage_items[63], // Train Station (Boarding)
    StageWarp::stage_items[64], // Train Station Exit
    StageWarp::stage_items[65], // Train Station Harvey (Boarding)
    StageWarp::stage_items[66], // Train Station Harvey (Exit)
    StageWarp::stage_items[67], // DeathMetal Toilet
    StageWarp::stage_items[68], // Darkstar Toilet
    StageWarp::stage_items[69], // Badgirl Toilet Stadium Basement
    StageWarp::stage_items[70], // Speedbuster Toilet Speed City
    StageWarp::stage_items[71], // Harvey Toilet
    StageWarp::stage_items[72], // Letz Shake Toilet Senton Splash Tunnel
    StageWarp::stage_items[73], // Holly Toilet Beach
    StageWarp::stage_items[74], // Destroyman Toilet Bear Hug
    StageWarp::stage_items[75], // Destroyman Toilet TGS
    StageWarp::stage_items[76], // Shinobu Toilet High School
    StageWarp::stage_items[77]  // Dr.Peace Toilet Stadium
};

std::array<std::reference_wrapper<StageWarp::Stage>, 10> boss_stages = {
    StageWarp::stage_items[41],  // Deathmetal
    StageWarp::stage_items[37],  // Dr.Peace
    StageWarp::stage_items[14],  // Shinobu
    StageWarp::stage_items[34],  // Destroyman
    StageWarp::stage_items[1],   // Holly Summers
    StageWarp::stage_items[44],  // Harvey
    StageWarp::stage_items[39],  // SpeedBuster
    StageWarp::stage_items[22],  // Bad Girl
    StageWarp::stage_items[47],  // Darkstar/Jeane
    StageWarp::stage_items[3]    // Henry
};

std::array<std::reference_wrapper<StageWarp::Stage>, 17> zako_stages = {
    StageWarp::stage_items[42],  // Deathmetal Zako 1
    StageWarp::stage_items[40],  // Deathmetal Zako 2
    StageWarp::stage_items[36],  // Dr.Peace Zako 1
    StageWarp::stage_items[50],  // Dr.Peace Zako 2
    StageWarp::stage_items[49],  // Dr.Peace Zako 3
    StageWarp::stage_items[9],   // Shinobu Zako 1
    StageWarp::stage_items[11],  // Shinobu Zako 2
    StageWarp::stage_items[12],  // Shinobu Zako 3
    StageWarp::stage_items[48],  // Destroyman Zako 1
    StageWarp::stage_items[32],  // Destroyman Zako 2
    StageWarp::stage_items[2],   // Holly Summers Zako
    StageWarp::stage_items[38],  // Letz Shake Zako
    StageWarp::stage_items[44],  // Harvey Zako
    StageWarp::stage_items[15],  // Speedbuster Zako
    StageWarp::stage_items[25],  // Bad Girl Zako
    StageWarp::stage_items[45],  // Darkstar Bike Zako
    StageWarp::stage_items[46]   // Darkstar Zako
};

std::array<std::reference_wrapper<StageWarp::Stage>, 8> city_stages = {
    StageWarp::stage_items[53],  // Motel
    StageWarp::stage_items[55],  // Beefhead Videos
    StageWarp::stage_items[56],  // K-Entertainment
    StageWarp::stage_items[57],  // Job Center
    StageWarp::stage_items[5],   // Bar Plastic Model Lovikov
    StageWarp::stage_items[58],  // Evolution Gym
    StageWarp::stage_items[59],  // Naomi's Lab
    StageWarp::stage_items[60]   // Area51
};

std::array<std::reference_wrapper<StageWarp::Stage>, 14> wii_stages = {
    StageWarp::stage_items[6],   // Cut Bike Race?
    StageWarp::stage_items[7],   // Cut Bike Race? 2
    StageWarp::stage_items[8],   // Cut Bike Race? 3
    StageWarp::stage_items[23],  // Bad Girl Fight JP
    StageWarp::stage_items[26],  // Bad Girl Zako JP
    StageWarp::stage_items[28],  // Bad Girl ZAKO DEBUG
    StageWarp::stage_items[29],  // Bad Girl ZAKO DEBUG
    StageWarp::stage_items[30],  // Bad Girl ZAKO DEBUG 2
    StageWarp::stage_items[31],  // Bad Girl ZAKO DEBUG 2
    StageWarp::stage_items[33],  // Destroyman TGS Zako
    StageWarp::stage_items[35],  // Destroyman TGS Fight
    StageWarp::stage_items[75],  // Destroyman Toilet TGS
    StageWarp::stage_items[53],  // Motel
    StageWarp::stage_items[52]   // ???
};

std::array<std::reference_wrapper<StageWarp::Stage>, 18> misc_stages = {
    StageWarp::stage_items[0],   // Santa Destroy Overworld
    StageWarp::stage_items[4],   // Free Fight 3
    StageWarp::stage_items[10],  // Motel Exterior
    StageWarp::stage_items[13],  // City Street
    StageWarp::stage_items[16],  // Free Fight Beach Spawn
    StageWarp::stage_items[17],  // Homes Near Beach
    StageWarp::stage_items[18],  // Northern Island Spawn
    StageWarp::stage_items[19],  // Free Fight Basketball
    StageWarp::stage_items[20],  // Northern Island Spawn
    StageWarp::stage_items[21],  // Free Fight 5
    StageWarp::stage_items[16],  // Mine Sweeping Beach Spawn
    StageWarp::stage_items[43],  // Deathmetal Sylvia Call
    StageWarp::stage_items[51],  // Stadium Free Fight
    StageWarp::stage_items[62],  // BROKEN Bike/Stadium stage
    StageWarp::stage_items[62],  // Train
    StageWarp::stage_items[63],  // Train Station (Boarding)
    StageWarp::stage_items[64],  // Train Station Exit
    StageWarp::stage_items[66]   // Train Station Harvey (Exit)
};

std::array<std::reference_wrapper<StageWarp::Stage>, 10> save_stages = {
    StageWarp::stage_items[67],  // DeathMetal Toilet
    StageWarp::stage_items[68],  // Darkstar Toilet
    StageWarp::stage_items[69],  // Badgirl Toilet
    StageWarp::stage_items[70],  // Speedbuster Toilet
    StageWarp::stage_items[71],  // Harvey Toilet
    StageWarp::stage_items[72],  // Letz Shake Toilet
    StageWarp::stage_items[73],  // Holly Toilet
    StageWarp::stage_items[74],  // Destroyman Toilet
    StageWarp::stage_items[76],  // Shinobu Toilet
    StageWarp::stage_items[77]   // Dr.Peace Toilet
};

std::optional<std::string> StageWarp::on_initialize() {
    return Mod::on_initialize();
}

struct CellData {
    ImTextureID image = nullptr;
    char text[128] = "";
};

template <size_t N>
void ShowImageTable(const char* headerName, const std::array<StageWarp::Stage, N>& stage_items) {
    if (ImGui::CollapsingHeader(headerName)) {
        const int columns = 3;
        int rows = (stage_items.size() + columns - 1) / columns;
        if (ImGui::BeginTable("ImageTable", columns, ImGuiTableFlags_Borders)) {
            for (int row = 0; row < rows; ++row) {
                ImGui::TableNextRow();
                for (int column = 0; column < columns; ++column) {
                    ImGui::TableSetColumnIndex(column);
                    int cellIndex = row * columns + column;
                    if (cellIndex >= stage_items.size()) {
                        continue;
                    }
                    const auto& stage = stage_items[cellIndex];
                    ImVec2 squareSize = ImVec2(100, 100);
                    if (ImGui::Button(stage.name, squareSize)) {
                        nmh_sdk::SetStage(stage_items[cellIndex].name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                            setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
                    }
                    ImGui::TextWrapped(stage.info);
                }
            }
            ImGui::EndTable();
        }
    }
}

template <typename T>
void DisplayStageSection(const char* headerName, const T& stages) {
    if (ImGui::CollapsingHeader(headerName)) {
        for (size_t i = 0; i < stages.size(); ++i) {
            char buttonLabel[64];
            char buttonInfo[64];
            auto& stage = stages[i].get();  // actual Stage object
            snprintf(buttonLabel, sizeof(buttonLabel), "%s", stage.name);
            snprintf(buttonInfo, sizeof(buttonInfo), "%s", stage.info);
            ImGui::TextColored(ImVec4(1.00, 0.79, 0.45, 1.0), buttonLabel);

            if (ImGui::IsItemHovered()) {
                if (*stage.longerInfo)
                    StageWarp::hoveredDescription = stage.longerInfo;
                else 
                    StageWarp::hoveredDescription = stage.info;
            }

            if (ImGui::IsItemClicked()) {
                nmh_sdk::SetStage(stage.name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                                  setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
            }
            ImGui::SameLine(100.0f);
            ImGui::TextColored(ImVec4(0.81, 0.40, 0.38, 1.0), buttonInfo);

            if (ImGui::IsItemHovered()) {
                if (*stage.longerInfo)
                    StageWarp::hoveredDescription = stage.longerInfo;
                else 
                    StageWarp::hoveredDescription = stage.info;
            }

            if (ImGui::IsItemClicked()) {
                nmh_sdk::SetStage(stage.name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                                  setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
            }
        }
    }
}


void StageWarp::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) {
            hoveredDescription = DefaultDescription;
    }
    if (nmh_sdk::get_CBgCtrl()) {
        char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName;
        if (currentStage && (strlen(currentStage) < 20)) {
            ImGui::Text("Current Stage: %s", nmh_sdk::get_CBgCtrl()->m_NowStageName);
        } else {
            ImGui::Text("Current Stage: ?");
        }

        DisplayStageSection("All", all_stages);
        DisplayStageSection("Bosses", boss_stages);
        DisplayStageSection("Zako", zako_stages);
        DisplayStageSection("City Interiors", city_stages);
        DisplayStageSection("Wii/Unused", wii_stages);
        DisplayStageSection("Miscellaneous", misc_stages);
        DisplayStageSection("Toilets", save_stages);

        // DisplayStageSection("All", StageWarp::stage_items); // old version

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
    }
    // hopefully you can slot images into this table or maybe my table sucks idk
    // ShowImageTable("All", StageWarp::stage_items);
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
