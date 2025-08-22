#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class EnemySpawn : public Mod {
public:
  EnemySpawn() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };

  static int inResNo;
  static int inRepop;
  static int inChType;
  static bool spawnAtPlayerPos;
  static bool spawnAtFreecamPos;
  static Vec inPos;
  static Vec inRot;
  static int inPopType;
  static bool inDisEnableCollision;

  // mod name string for config
  std::string get_mod_name() const override { return "EnemySpawner"; }
  std::string get_human_readable_name() const { return "Enemy Spawner"; }
  //const char* get_description() const override { return R"(Spawn in any enemy that exists in the level you're currently playing. This is a very fragile feature, so expect crashes as you experiment.)"; };
  void render_description() const override;
  static const char* defaultDescription;
  static const char* hoveredDescription;

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  //void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	// std::unique_ptr<FunctionHook> m_hook1;
	// std::unique_ptr<Patch> m_patch;
};

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