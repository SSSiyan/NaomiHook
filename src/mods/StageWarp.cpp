#include "StageWarp.hpp"
#include "utility/Compressed.hpp"
#include "intrin.h"
#include "fw-imgui/Texture2DD3D11.hpp"
#include "fw-imgui/StageWarpAtlas.cpp"
#if 1

static int setStageArgs[7]{0, -1, -1, 0, 0, 0, 0};
static int64_t inSetVolRateArg = 0;

static ImVec4 stageNameColor{ 1.00f, 0.79f, 0.45f, 1.0f };
static ImVec4 stageInfoColor{ 0.81f, 0.40f, 0.38f, 1.0f };
static ImVec4 stageHoveredColor{ 0.37f, 0.71f, 0.34f, 1.0f };

// directx stuff
static std::unique_ptr<Texture2DD3D11> g_stage_warp_texture_atlas{};

#pragma region TextureAtlasDefinitions

struct Frame {
    ImVec2 pos, size;
    ImVec2 uv0, uv1;
};

struct TextureAtlas {
    static constexpr auto getCoordinates(int index) {
        if (index < 0 || index > 78) {
            return Frame{
                ImVec2{ 3584.0f, 1024.0f },
                ImVec2{ 3840.0f, 1280.0f },
                ImVec2{ 0.875f, 0.5f },
                ImVec2{ 0.9375f, 0.625f }
            }; // 78, "none"
        }

        constexpr float atlasWidth = 4096.0f;
        constexpr float atlasHeight = 2048.0f;
        constexpr float imageSize = 256.0f;

        int row = index / 16;
        int col = index % 16;

        float xStart = col * imageSize;
        float yStart = row * imageSize;

        float xEnd = xStart + imageSize;
        float yEnd = yStart + imageSize;

        return Frame{
            ImVec2{ xStart, yStart },
            ImVec2{ xEnd, yEnd },
            ImVec2{ xStart / atlasWidth, yStart / atlasHeight },
            ImVec2{ xEnd / atlasWidth, yEnd / atlasHeight }
        };
    }

    static constexpr auto meta_size() {
        return ImVec2{ 4096.0f, 2048.0f };
    }
};

#pragma endregion

// yeah all this is needed to update the bottom panel
const char* StageWarp::defaultDescription = "Teleport to any stage in the game. 'Wii/Unused' contains warps to stages which aren't included with the Steam release of NMH1, so to make use of these, you'll need to have the files from the Wii version.";
const char* StageWarp::hoveredDescription = defaultDescription;
const char* StageWarp::defaultStageName = "Hover over a stage to see details.";
const char* StageWarp::stageName = defaultStageName;
Frame stageImage;
//

void StageWarp::render_description() const {
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    //ImGui::Button(buttonIcon, ImVec2(availableSpace.y, availableSpace.y));
    ImGui::Image(g_stage_warp_texture_atlas->GetTexture(), ImVec2(availableSpace.y, availableSpace.y), stageImage.uv0, stageImage.uv1, ImVec4(1,1,1,1));
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::TextColored(stageNameColor, stageName);
    ImGui::TextWrapped(hoveredDescription);
    ImGui::EndGroup();
}

void update_description(const char* name, const char* info, Frame image) {
    StageWarp::stageName = name;
    StageWarp::hoveredDescription = info;
    stageImage = image;
}

std::array<StageWarp::Stage, 78> StageWarp::stage_items = { 
    Stage {"STG000",    0, "Santa Destroy Overworld", ""},                //  0
    Stage {"STG0001",   1, "Holly Summers Fight", ""},                    //  1
    Stage {"STG0002",   2, "Holly Summers Zako", ""},                     //  2
    Stage {"STG0003",   3, "Henry Fight True Ending", ""},                //  3
    Stage {"STG0004",   4, "Free Fight 3", ""},                           //  4
    Stage {"STG0007",   5, "Bar Plastic Model Lovikov", ""},              //  5
    Stage {"STG0008",   6, "Cut Bike Race?", ""},                         //  6
    Stage {"STG0009",   7, "Cut Bike Race? 2", ""},                       //  7
    Stage {"STG00010",  8, "Cut Bike Race? 3", ""},                       //  8
    Stage {"STG010",    9, "Shinobu Zako 1", ""},                         //  9
    Stage {"STG00011", 10, "Motel Exterior", ""},                         // 10
    Stage {"STG011",   11, "Shinobu Zako 2", ""},                         // 11
    Stage {"STG012",   12, "Shinobu Zako 3", ""},                         // 12
    Stage {"STG00013", 13, "City Street", ""},                            // 13
    Stage {"STG013",   14, "Shinobu Fight", ""},                          // 14
    Stage {"STG00014", 15, "Speedbuster Zako", ""},                       // 15
    Stage {"STG00015", 16, "Beach Spawn", ""},                            // 16
    Stage {"STG00016", 17, "Homes Near Beach", ""},                       // 17
    Stage {"STG00017", 18, "Northern Island Spawn", ""},                  // 18
    Stage {"STG00018", 19, "Free Fight Basketball", ""},                  // 19
    Stage {"STG00019", 20, "Northern Island Spawn", ""},                  // 20
    Stage {"STG00020", 21, "Free Fight 5", ""},                           // 21
    Stage {"STG020",   22, "Bad Girl Fight", ""},                         // 22
    Stage {"STG020J",  23, "Bad Girl Fight JP", ""},                      // 23
    Stage {"STG00021", 24, "Beach Spawn", ""},                            // 24
    Stage {"STG021",   25, "Bad Girl Zako", ""},                          // 25
    Stage {"STG021J",  26, "Bad Girl Zako JP", ""},                       // 26
    Stage {"STG022",   27, "Bad Girl Hall", ""},                          // 27
    Stage {"STG023",   28, "Bad Girl ZAKO DEBUG", ""},                    // 28
    Stage {"STG023J",  29, "Bad Girl ZAKO DEBUG JP", ""},                 // 29
    Stage {"STG024",   30, "Bad Girl ZAKO DEBUG 2", ""},                  // 30
    Stage {"STG024J",  31, "Bad Girl ZAKO DEBUG 2 JP", ""},               // 31
    Stage {"STG030",   32, "Destroyman Zako 2", ""},                      // 32
    Stage {"STG030T",  33, "Destroyman TGS Zako", ""},                    // 33
    Stage {"STG031",   34, "Destroyman Fight", ""},                       // 34
    Stage {"STG031T",  35, "Destroyman TGS Fight", ""},                   // 35
    Stage {"STG041",   36, "Dr.Peace Zako 1", ""},                        // 36
    Stage {"STG042",   37, "Dr.Peace Fight", ""},                         // 37
    Stage {"STG051",   38, "Letz Shake Zako", ""},                        // 38
    Stage {"STG060",   39, "SpeedBuster", ""},                            // 39
    Stage {"STG080",   40, "Deathmetal Zako 2", ""},                      // 40
    Stage {"STG081",   41, "Deathmetal Fight", ""},                       // 41
    Stage {"STG083",   42, "Deathmetal Zako 1", ""},                      // 42
    Stage {"STG084",   43, "Deathmetal Sylvia Call", ""},                 // 43
    Stage {"STG090",   44, "Harvey Fight", ""},                           // 44
    Stage {"STG100",   45, "Darkstar Bike Zako", ""},                     // 45
    Stage {"STG101",   46, "Darkstar Zako", ""},                          // 46
    Stage {"STG103",   47, "Darkstar/Jeane Fight", ""},                   // 47
    Stage {"STG170",   48, "Destroyman Zako 1", ""},                      // 48
    Stage {"STG0411",  49, "Dr.Peace Zako 3", ""},                        // 49
    Stage {"STG0412",  50, "Dr.Peace Zako 2", ""},                        // 50
    Stage {"STG0421",  51, "Stadium Free Fight", ""},                     // 51
    Stage {"STG0422",  52, "???", ""},                                    // 52
    Stage {"STG500",   53, "Motel", ""},                                  // 53
    Stage {"STG500US", 54, "Motel Wii", ""},                              // 54
    Stage {"STG510",   55, "Beefhead Videos", ""},                        // 55
    Stage {"STG520",   56, "K-Entertainment", ""},                        // 56
    Stage {"STG540",   57, "Job Center", ""},                             // 57
    Stage {"STG550",   58, "Evolution Gym", ""},                          // 58
    Stage {"STG560",   59, "Naomi's Lab", ""},                            // 59
    Stage {"STG570",   60, "Area51", ""},                                 // 60
    Stage {"STG580",   61, "??? 2", ""},                                  // 61
    Stage {"STG1702",  62, "Train", ""},                                  // 62
    Stage {"STG1703",  63, "Train Station (Boarding)", ""},               // 63
    Stage {"STG1707",  64, "Train Station Exit", ""},                     // 64
    Stage {"STG1708",  65, "Harvey Zako", ""},        // 65
    Stage {"STG1709",  66, "Train Station Harvey (Exit)", ""},            // 66
    Stage {"STG9000",  67, "DeathMetal Toilet", ""},                      // 67
    Stage {"STG9001",  68, "Darkstar Toilet", ""},                        // 68
    Stage {"STG9002",  69, "Badgirl Toilet Stadium Basement", ""},        // 69
    Stage {"STG9003",  70, "Speedbuster Toilet Speed City", ""},          // 70
    Stage {"STG9004",  71, "Harvey Toilet", ""},                          // 71
    Stage {"STG9005",  72, "Letz Shake Toilet Senton Splash Tunnel", ""}, // 72
    Stage {"STG9006",  73, "Holly Toilet Beach", ""},                     // 73
    Stage {"STG9007",  74, "Destroyman Toilet Bear Hug", ""},             // 74
    Stage {"STG9007T", 75, "Destroyman Toilet TGS", ""},                  // 75
    Stage {"STG9008",  76, "Shinobu Toilet High School", ""},             // 76
    Stage {"STG9009",  77, "Dr.Peace Toilet Stadium", ""},                // 77
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
    StageWarp::stage_items[16], // Free Fight Beach Spawn
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
    StageWarp::stage_items[54], // Motel Wii
    StageWarp::stage_items[55], // Beefhead Videos
    StageWarp::stage_items[56], // K-Entertainment
    StageWarp::stage_items[57], // Job Center
    StageWarp::stage_items[58], // Evolution Gym
    StageWarp::stage_items[59], // Naomi's Lab
    StageWarp::stage_items[60], // Area51
    StageWarp::stage_items[61], // ??? 2
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
    StageWarp::stage_items[65],  // Train Station Harvey (Boarding)
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

std::array<std::reference_wrapper<StageWarp::Stage>, 15> wii_stages = {
    StageWarp::stage_items[6],   // Cut Bike Race?
    StageWarp::stage_items[7],   // Cut Bike Race? 2
    StageWarp::stage_items[8],   // Cut Bike Race? 3
    StageWarp::stage_items[23],  // Bad Girl Fight JP
    StageWarp::stage_items[26],  // Bad Girl Zako JP
    StageWarp::stage_items[28],  // Bad Girl ZAKO DEBUG
    StageWarp::stage_items[29],  // Bad Girl ZAKO DEBUG JP
    StageWarp::stage_items[30],  // Bad Girl ZAKO DEBUG 2
    StageWarp::stage_items[31],  // Bad Girl ZAKO DEBUG 2 JP
    StageWarp::stage_items[33],  // Destroyman TGS Zako
    StageWarp::stage_items[35],  // Destroyman TGS Fight
    StageWarp::stage_items[75],  // Destroyman Toilet TGS
    StageWarp::stage_items[54],  // Motel Wii
    StageWarp::stage_items[52],  // ???
    StageWarp::stage_items[61],  // ??? 2
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
    StageWarp::stage_items[24],  // Beach Spawn
    StageWarp::stage_items[27],  // Bad Girl Hall
    StageWarp::stage_items[43],  // Deathmetal Sylvia Call
    StageWarp::stage_items[51],  // Stadium Free Fight
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

bool load_stage_warp_texture() {
    auto [data, size] = utility::decompress_file_from_memory_with_size(stage_warp_compressed_data, stage_warp_compressed_size);
    if (!data) {
        return false;
    }

    g_stage_warp_texture_atlas = std::make_unique<Texture2DD3D11>((const unsigned char*)data, size, g_framework->d3d11()->get_device());
    if (!g_stage_warp_texture_atlas->IsValid()) {
        return false;
    }

    return true;
}

std::optional<std::string> StageWarp::on_initialize() {
    load_stage_warp_texture();
    update_description(defaultStageName, defaultDescription, TextureAtlas::getCoordinates(78));
    return Mod::on_initialize();
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

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); 
            ImGui::TextColored(stageNameColor, buttonLabel);
            if (ImGui::IsItemHovered()) {
                update_description(stage.name, stage.info, TextureAtlas::getCoordinates(stage.arrayItem));
            }

            if (ImGui::IsItemClicked()) {
                nmh_sdk::SetStage(stage.name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                    setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
            }
            ImGui::SameLine(ImGui::GetFontSize()*5.0f);
            ImGui::TextColored(stageInfoColor, buttonInfo);
            if (ImGui::IsItemHovered()) {
                update_description(stage.name, stage.info, TextureAtlas::getCoordinates(stage.arrayItem));
            }

            if (ImGui::IsItemClicked()) {
                nmh_sdk::SetStage(stage.name, setStageArgs[0], setStageArgs[1], setStageArgs[2], setStageArgs[3],
                    setStageArgs[4], inSetVolRateArg, setStageArgs[5], setStageArgs[6]);
            }
            ImGui::PopStyleVar();
        }
    }
}

void StageWarp::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered())
        update_description(defaultStageName, defaultDescription, TextureAtlas::getCoordinates(78));
    if (nmh_sdk::get_CBgCtrl()) {
        char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName;
        if (currentStage && (strlen(currentStage) < 20)) {
            ImGui::Text("Current Stage: %s", nmh_sdk::get_CBgCtrl()->m_NowStageName);
        } else {
            ImGui::Text("Current Stage: ?");
        }

        // float available_width = ImGui::GetContentRegionAvail().x;
        // float combo_width = ImGui::CalcItemWidth();
        // ImGui::SetCursorPosX((available_width - combo_width) * 0.5f);
        ImGui::Combo("##inFadeType", &setStageArgs[4], "Punk\0Fade\0Grey\0Stamps\0Instant (Soft Lock)\0");
        if (ImGui::IsItemHovered()) {
            update_description("Fade Type", "Set the transition that plays when you teleport", TextureAtlas::getCoordinates(78));
        }

        DisplayStageSection("All", all_stages);
        DisplayStageSection("Bosses", boss_stages);
        DisplayStageSection("Zako", zako_stages);
        DisplayStageSection("City Interiors", city_stages);
        DisplayStageSection("Wii/Unused", wii_stages);
        DisplayStageSection("Miscellaneous", misc_stages);
        DisplayStageSection("Toilets", save_stages);

        //static const char* argsHelpMarker("These args are exposed so we can figure out if there's a way to make more warps possible without crashing.\n");
        /*if (ImGui::CollapsingHeader("SetStage args")) {
            help_marker(argsHelpMarker);
            ImGui::InputInt("AddedStages", &setStageArgs[0]);
            help_marker("I think this adds n to stageID? Not sure how else it would get next stage\nMotel>Overworld has this set to 2");
            ImGui::InputInt("_Arg1", &setStageArgs[1]);
            help_marker("Motel>Overworld sets this to -1");
            ImGui::InputInt("_Arg2", &setStageArgs[2]);
            help_marker("Motel>Overworld sets this to -1");
            ImGui::InputInt("inBossInfoDisp", &setStageArgs[3]);
            help_marker("Shows the boss popup");
            
            ImGui::InputScalar("inSetVolRate", ImGuiDataType_S64, &inSetVolRateArg);
            ImGui::InputInt("inPause", &setStageArgs[5]);
            ImGui::InputInt("a10", &setStageArgs[6]);
        }
        else {
            help_marker(argsHelpMarker);
        }*/
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
