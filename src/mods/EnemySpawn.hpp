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
