#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class GameplayFixes : public Mod {
public:
  GameplayFixes() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };

  static bool money_on_kill_fix_enabled;
  static uintptr_t money_on_kill_fix_jmp_ret1;
  static uintptr_t money_on_kill_fix_jmp_ret2;
  static uintptr_t money_on_kill_fix_jmp_ret3;

  static bool baseball_power_fix_enabled;
  static constexpr float new_baseball_power = 2.0f;
  static uintptr_t baseball_power_fix_jmp_ret1;
  static uintptr_t baseball_power_fix_jmp_ret2;
  static constexpr float baseball_power_fix_original_value = 0.4f;

  // mod name string for config
  std::string get_mod_name() const override { return "GameplayFixes"; }
  std::string get_human_readable_name() const { return "Gameplay Fixes"; }
  //const char* get_description() const override { return R"(GameplayFixes)"; };
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
	std::unique_ptr<FunctionHook> money_on_kill_fix_hook1, money_on_kill_fix_hook2, money_on_kill_fix_hook3;
	std::unique_ptr<FunctionHook> baseball_power_fix_hook1, baseball_power_fix_hook2;
	// std::unique_ptr<Patch> patch0;
};
