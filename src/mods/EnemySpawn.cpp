#include "EnemySpawn.hpp"
#if 1
int EnemySpawn::inResNo = 0;
int EnemySpawn::inRepop = 0;
int EnemySpawn::inChType = 0;
bool EnemySpawn::spawnAtPlayerPos = false;
Vec EnemySpawn::inPos{ 0.0f, 0.0f, 0.0f };
Vec EnemySpawn::inRot{ 0.0f, 0.0f, 0.0f };
int EnemySpawn::inPopType = 0;
bool EnemySpawn::inDisEnableCollision = false;

const char* EnemySpawn::defaultDescription = "Spawn in any enemy that exists in the level you're currently playing. This is a very fragile feature, so expect crashes as you experiment.";
const char* EnemySpawn::hoveredDescription = defaultDescription;

void EnemySpawn::render_description() const {
    ImGui::TextWrapped(EnemySpawn::hoveredDescription);
}

static const char* charaTypeStrings[91] {
  "eCharaTypeNone",                  // 0,
  "eCharaTypePC PlayerCharacter",    // 1,
  "Sylvia",                          // 2,  eCharaTypeSLV
  "eCharaTypeNPC",                   // 3,
  "eCharaTypeNPCNotMove",            // 4,
  "eCharaTypeOBJ",                   // 5,
  "eCharaTypeOBJNotMove",            // 6,
  "Zako Pipe",                       // 7,  eCharaTypeZKPipe
  "Boss Shinobu",                    // 8,  eCharaTypeTGR
  "Boss BadGirl",                    // 9,  eCharaTypeTYG
  "Boss Destroyman",                 // 10, eCharaTypeJST
  "Boss Holly Summers",              // 11, eCharaTypeSFF
  "Boss Speedbuster",                // 12, eCharaTypeEFL
  "eCharaTypeOBJ700",                // 13,
  "Boss Dr.Peace",                   // 14, eCharaTypeMGE
  "eCharaTypeENX",                   // 15,
  "Boss Death Metal",                // 16, eCharaTypeMAM
  "Zako Clone",                      // 17, eCharaTypeZKClone
  "Boss Harvey",                     // 18, eCharaTypeTET
  "Boss Jeane",                      // 19, eCharaTypeGEN
  "Boss Henry",                      // 20, eCharaTypeTKL
  "Zako Katana",                     // 21, eCharaTypeZKKatana
  "Zako Axe",                        // 22, eCharaTypeZKAxe
  "Zako BeamKatanaA",                // 23, eCharaTypeZKBeamKatanaA
  "Zako BeamKatanaB",                // 24, eCharaTypeZKBeamKatanaB
  "Zako HandGun",                    // 25, eCharaTypeZKHandGun
  "Zako SubMachineGun",              // 26, eCharaTypeZKSubMachineGun
  "Zako ShotGun",                    // 27, eCharaTypeZKShotGun
  "Zako Ball",                       // 28, eCharaTypeZKBall
  "eCharaTypeOBJ013_0",              // 29,
  "eCharaTypeOBJ013_1",              // 30,
  "eCharaTypeOBJ013_2",              // 31,
  "eCharaTypeOBJ630",                // 32,
  "eCharaTypeOBJ631",                // 33,
  "eCharaTypeOBJ632",                // 34,
  "Death Metal Clone",               // 35, eCharaTypeMAM_Jr
  "eCharaTypeHML",                   // 36,
  "eCharaTypeNZN",                   // 37,
  "Zako Bike",                       // 38, eCharaTypeZakoBike
  "eCharaTypeOBJ161",                // 39,
  "eCharaTypeOBJ162",                // 40,
  "eCharaTypeSCP",                   // 41,
  "eCharaTypeOBJ164",                // 42,
  "eCharaTypeZKNone",                // 43,
  "eCharaTypeDAG Lovikov",           // 44,
  "eCharaTypeOBJ169 Electric Fence", // 45,
  "Zako Hammer",                     // 46, eCharaTypeZKHammer
  "Zako Torch",                      // 47, eCharaTypeZKKakuzai 
  "Zako Bat",                        // 48, eCharaTypeZKBat
  "Zako Shovel",                     // 49, eCharaTypeZKSchop
  "Zako Signboard",                  // 50, eCharaTypeZKSignboard
  "Zako Oxtail Sabre",               // 51, eCharaTypeZKSword
  "Zako Molotov",                    // 52, eCharaTypeZKKaeinbin
  "eCharaTypeOBJ180",                // 53,
  "eCharaTypeOBJ181",                // 54,
  "eCharaTypeOBJ182",                // 55,
  "eCharaTypeETG Defeated Shinobu",  // 56,
  "eCharaTypeSCT",                   // 57,
  "eCharaTypeOBJ955",                // 58,
  "eCharaTypeSLV2",                  // 59,
  "eCharaTypeWEL",                   // 60,
  "eCharaTypeSLV3",                  // 61,
  "eCharaTypeOBJ095",                // 62,
  "eCharaTypeOBJ431",                // 63,
  "eCharaTypeOBJ629",                // 64,
  "eCharaTypeOBJ183",                // 65,
  "eCharaTypeSubMissionNpc",         // 500, (66)
  "eCharaTypeOBJ021",                // 501,
  "eCharaTypeSMJ",                   // 502,
  "eCharaTypeOBJ008",                // 503,
  "eCharaTypeOBJ009",                // 504,
  "eCharaTypeOBJ010",                // 505,
  "eCharaTypeOBJ011",                // 506,
  "eCharaTypeOBJ012",                // 507,
  "eCharaTypeOBJ106",                // 508,
  "eCharaTypeOBJ107",                // 509,
  "eCharaTypeOBJ108",                // 510,
  "eCharaTypeOBJ019",                // 511,
  "eCharaTypeOBJ435",                // 512,
  "eCharaTypeOBJ065_1",              // 513,
  "eCharaTypeOBJ065_2",              // 514,
  "eCharaTypeOBJ065_3",              // 515,
  "eCharaTypeSML",                   // 516,
  "eCharaTypeOBJ156_1",              // 517,
  "eCharaTypeOBJ156_2",              // 518,
  "eCharaTypeOBJ155",                // 519,
  "eCharaTypeOBJ159",                // 520,
  "eCharaTypeOBJ158",                // 521,
  "eCharaTypeSMQ",                   // 522,
  "eCharaTypeOBJ_GAS",               // 523,
  "eCharaTypeSM_CAT",                // 524,
};

static const char* enPopReqTypeStrings[3] {
  "ePopTypeDistance", // 0
  "ePopTypeAlways",   // 1
  "ePopTypeOrder",    // 2
};

std::optional<std::string> EnemySpawn::on_initialize() {
    return Mod::on_initialize();
}

struct StageInResNo {
    const char* stageName;
    int inResNo;
};

int GetNumOfInResNoAvailableInThisStage(const char* currentStage) {
    static const StageInResNo stageData[] = {
        {"STG083", 9},
        {"STG080", 9},
        {"STG081", 4},
        {"STG041", 9},
        {"STG0412", 9},
        {"STG0411", 9},
        {"STG042", 3},
        {"STG010", 9},
        {"STG011", 9},
        {"STG012", 9},
        {"STG013", 4},
        {"STG170", 9},
        {"STG030", 9},
        {"STG031", 4},
        {"STG0002", 11},
        {"STG0001", 4},
        {"STG051", 7},
        {"STG1708", 9},
        {"STG090", 4},
        {"STG00014", 3},
        {"STG060", 6},
        {"STG021", 9},
        {"STG022", 1},
        {"STG010", 4},
        {"STG100", 0},
        {"STG101", 3},
        {"STG103", 3},
    };
    
    for (int i = 0; i < sizeof(stageData) / sizeof(stageData[0]); i++) {
        if (strcmp(currentStage, stageData[i].stageName) == 0) {
            return stageData[i].inResNo;
        }
    }
    return 0;
}

void EnemySpawn::on_draw_ui() {
    if (!ImGui::IsAnyItemHovered()) EnemySpawn::hoveredDescription = defaultDescription;
    static int numOfInResNoAvailableInThisStage = 0;
    char* currentStage = nmh_sdk::get_CBgCtrl()->m_NowStageName;
    if (currentStage && (strlen(currentStage) < 20)) {
        nmh_sdk::get_CBgCtrl()->m_NowStageName;
        numOfInResNoAvailableInThisStage = GetNumOfInResNoAvailableInThisStage(currentStage);
    }
    if (inResNo > numOfInResNoAvailableInThisStage) { inResNo = numOfInResNoAvailableInThisStage; }
    ImGui::Text("inResNo");
    ImGui::SliderInt("## inResNo Input Int", &inResNo, 0, numOfInResNoAvailableInThisStage);
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "@DHMalice, explain inResNo here";
    ImGui::Text("inRepop");
    ImGui::InputInt("## inRepop Input Int", &inRepop);
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "@DHMalice, explain inRepop here";
    ImGui::Checkbox("Spawn At Player Pos", &spawnAtPlayerPos);
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "The spawned enemy will spawn at your coords";
    if (!spawnAtPlayerPos){
        ImGui::Text("Custom Position");
        ImGui::InputFloat3("## Custom Position Input Float", &inPos.x);
        if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "The spawned enemy will spawn at custom coords";
    }
    else {
        if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
            ImGui::Text("Player Position");
            ImGui::InputFloat3("## Player Position Input Float", &mHRPc->mCharaStatus.pos.x);
            if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "Your current coords";
            inPos = mHRPc->mCharaStatus.pos;
        }
    }
    ImGui::Text("Rotation");
    ImGui::InputFloat3("## Rotation Input Float", &inRot.x);
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "Which way the spawned enemy will face";
    ImGui::Checkbox("inDisEnableCollision", &inDisEnableCollision);
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "The spawned enemy will ignore collisions";
    ImGui::Text("enPopReqType");
    if (ImGui::BeginCombo("## enPopReqType Combo", enPopReqTypeStrings[inPopType])) {
        for (int i = 0; i < IM_ARRAYSIZE(enPopReqTypeStrings); i++) {
            bool isSelected = (inPopType == i);
            if (ImGui::Selectable(enPopReqTypeStrings[i], isSelected)) {
                inPopType = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "@DHMalice, explain enPopReqType here";
    ImGui::Text("inChType");
    if (ImGui::BeginCombo("## inChType Combo", charaTypeStrings[inChType])) {
        for (int i = 0; i < IM_ARRAYSIZE(charaTypeStrings); i++) {
            bool isSelected = (inChType == i);
            if (ImGui::Selectable(charaTypeStrings[i], isSelected)) {
                inChType = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "@DHMalice, explain inChType here";
    float combo_width = ImGui::CalcItemWidth();
    if (ImGui::Button("Spawn Enemy", ImVec2(combo_width, NULL))) {
        if (inChType > 65)
            nmh_sdk::setInitNpcDat(inResNo, (enCharaType)(inChType + 434), inRepop, &inPos, &inRot, (enPopReqType)inPopType, inDisEnableCollision);
        else
            nmh_sdk::setInitNpcDat(inResNo, (enCharaType)inChType, inRepop, &inPos, &inRot, (enPopReqType)inPopType, inDisEnableCollision);
    }
    if (ImGui::IsItemHovered()) EnemySpawn::hoveredDescription = "Spawn an enemy with the above parameters";
}

// during load
void EnemySpawn::on_config_load(const utility::Config &cfg) {
    inResNo = cfg.get<int>("inResNo").value_or(0);
    inRepop = cfg.get<int>("inRepop").value_or(1);
    inChType = cfg.get<int>("inChType").value_or(7);
    spawnAtPlayerPos = cfg.get<bool>("spawnAtPlayerPos").value_or(true);
    inPopType = cfg.get<int>("inPopType").value_or(1);
}

// during save
void EnemySpawn::on_config_save(utility::Config &cfg) {
    cfg.set<int>("inResNo", inResNo);
    cfg.set<int>("inRepop", inRepop);
    cfg.set<int>("inChType", inChType);
    cfg.set<bool>("spawnAtPlayerPos", spawnAtPlayerPos);
    cfg.set<int>("inPopType", inPopType);
}

// do something every frame
//void EnemySpawn::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void EnemySpawn::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
