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

static const char* charaTypeStrings[91] {
  "eCharaTypeNone", // 0,
  "eCharaTypePC", // 1,
  "eCharaTypeSLV", // 2,
  "eCharaTypeNPC", // 3,
  "eCharaTypeNPCNotMove", // 4,
  "eCharaTypeOBJ", // 5,
  "eCharaTypeOBJNotMove", // 6,
  "eCharaTypeZKPipe", // 7,
  "eCharaTypeTGR", // 8,
  "eCharaTypeTYG", // 9,
  "eCharaTypeJST", // 10,
  "eCharaTypeSFF", // 11,
  "eCharaTypeEFL", // 12,
  "eCharaTypeOBJ700", // 13,
  "eCharaTypeMGE", // 14,
  "eCharaTypeENX", // 15,
  "eCharaTypeMAM", // 16,
  "eCharaTypeZKClone", // 17,
  "eCharaTypeTET", // 18,
  "eCharaTypeGEN", // 19,
  "eCharaTypeTKL", // 20,
  "eCharaTypeZKKatana", // 21,
  "eCharaTypeZKAxe", // 22,
  "eCharaTypeZKBeamKatanaA", // 23,
  "eCharaTypeZKBeamKatanaB", // 24,
  "eCharaTypeZKHandGun", // 25,
  "eCharaTypeZKSubMachineGun", // 26,
  "eCharaTypeZKShotGun", // 27,
  "eCharaTypeZKBall", // 28,
  "eCharaTypeOBJ013_0", // 29,
  "eCharaTypeOBJ013_1", // 30,
  "eCharaTypeOBJ013_2", // 31,
  "eCharaTypeOBJ630", // 32,
  "eCharaTypeOBJ631", // 33,
  "eCharaTypeOBJ632", // 34,
  "eCharaTypeMAM_Jr", // 35,
  "eCharaTypeHML", // 36,
  "eCharaTypeNZN", // 37,
  "eCharaTypeZakoBike", // 38,
  "eCharaTypeOBJ161", // 39,
  "eCharaTypeOBJ162", // 40,
  "eCharaTypeSCP", // 41,
  "eCharaTypeOBJ164", // 42,
  "eCharaTypeZKNone", // 43,
  "eCharaTypeDAG", // 44,
  "eCharaTypeOBJ169", // 45,
  "eCharaTypeZKHammer", // 46,
  "eCharaTypeZKKakuzai", // 47,
  "eCharaTypeZKBat", // 48,
  "eCharaTypeZKSchop", // 49,
  "eCharaTypeZKSignboard", // 50,
  "eCharaTypeZKSword", // 51,
  "eCharaTypeZKKaeinbin", // 52,
  "eCharaTypeOBJ180", // 53,
  "eCharaTypeOBJ181", // 54,
  "eCharaTypeOBJ182", // 55,
  "eCharaTypeETG", // 56,
  "eCharaTypeSCT", // 57,
  "eCharaTypeOBJ955", // 58,
  "eCharaTypeSLV2", // 59,
  "eCharaTypeWEL", // 60,
  "eCharaTypeSLV3", // 61,
  "eCharaTypeOBJ095", // 62,
  "eCharaTypeOBJ431", // 63,
  "eCharaTypeOBJ629", // 64,
  "eCharaTypeOBJ183", // 65,
  "eCharaTypeSubMissionNpc", // 500, (66)
  "eCharaTypeOBJ021", // 501,
  "eCharaTypeSMJ", // 502,
  "eCharaTypeOBJ008", // 503,
  "eCharaTypeOBJ009", // 504,
  "eCharaTypeOBJ010", // 505,
  "eCharaTypeOBJ011", // 506,
  "eCharaTypeOBJ012", // 507,
  "eCharaTypeOBJ106", // 508,
  "eCharaTypeOBJ107", // 509,
  "eCharaTypeOBJ108", // 510,
  "eCharaTypeOBJ019", // 511,
  "eCharaTypeOBJ435", // 512,
  "eCharaTypeOBJ065_1", // 513,
  "eCharaTypeOBJ065_2", // 514,
  "eCharaTypeOBJ065_3", // 515,
  "eCharaTypeSML", // 516,
  "eCharaTypeOBJ156_1", // 517,
  "eCharaTypeOBJ156_2", // 518,
  "eCharaTypeOBJ155", // 519,
  "eCharaTypeOBJ159", // 520,
  "eCharaTypeOBJ158", // 521,
  "eCharaTypeSMQ", // 522,
  "eCharaTypeOBJ_GAS", // 523,
  "eCharaTypeSM_CAT", // 524,
};

static const char* enPopReqTypeStrings[3] {
  "ePopTypeDistance", // 0
  "ePopTypeAlways",   // 1
  "ePopTypeOrder",    // 2
};

std::optional<std::string> EnemySpawn::on_initialize() {
    return Mod::on_initialize();
}

void EnemySpawn::on_draw_ui() {
    ImGui::InputInt("inResNo", &inResNo);
    ImGui::InputInt("inRepop", &inRepop);
    ImGui::Checkbox("Spawn At Player Pos", &spawnAtPlayerPos);
    if (!spawnAtPlayerPos){
        ImGui::InputFloat3("Custom Position", &inPos.x);
    }
    else {
        if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
            ImGui::InputFloat3("Player Position", &mHRPc->mCharaStatus.pos.x);
            inPos = mHRPc->mCharaStatus.pos;
        }
    }
    ImGui::InputFloat3("Rotation", &inRot.x);
    ImGui::Checkbox("inDisEnableCollision", &inDisEnableCollision);

    if (ImGui::BeginCombo("enPopReqType", enPopReqTypeStrings[inPopType])) {
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

    if (ImGui::BeginCombo("inChType", charaTypeStrings[inChType])) {
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
    if (ImGui::Button("Spawn Enemy")){
        if (inChType > 65)
            nmh_sdk::setInitNpcDat(inResNo, (enCharaType)(inChType + 434), inRepop, &inPos, &inRot, (enPopReqType)inPopType, inDisEnableCollision);
        else
            nmh_sdk::setInitNpcDat(inResNo, (enCharaType)inChType, inRepop, &inPos, &inRot, (enPopReqType)inPopType, inDisEnableCollision);
    }
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
