#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class SwordColours : public Mod {
public:
  SwordColours() = default;
  
  ModCategory get_category() { return ModCategory::COSMETICS; };
  static bool mod_enabled;

  static uintptr_t jmp_ret1;
  static uintptr_t gpBattle;
  static uint8_t coloursPicked[5][4]; // bgra
  static float coloursPickedFloat[5][4]; //rgba

  static uintptr_t jmp_ret2;
  static int deathblowTimer;
  static uintptr_t offset_mCheckHajikare;
  static int setDeathblowTimer;

  // mod name string for config
  std::string get_mod_name() const override { return "SwordColours"; }
  std::string get_human_readable_name() const { return "Sword Colours"; }
  const char* get_description() const override { return R"(Sword Colours)"; };

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  // void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	std::unique_ptr<FunctionHook> m_hook1, m_hook2;
	// std::unique_ptr<Patch> m_patch;
};
