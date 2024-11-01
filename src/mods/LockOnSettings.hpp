#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class LockOnSettings : public Mod {
public:
  LockOnSettings() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };

  static bool mod_enabled;
  // static bool lockon_sidesteps;
  // static bool lockon_deathblows;
  // static bool lockon_parry_qtes;

  static uintptr_t jmp_ret1;
  static bool target_switch_degrees_toggle;
  static float default_search_degrees;
  static float custom_search_degrees;

  static uintptr_t jmp_ret2;
  static uintptr_t jmp_ja2;
  static uintptr_t jmp_ret2alt;

  // void toggle_sidestep_lockon(bool enable);
  // void toggle_deathblow_lockon(bool enable);
  // void toggle_parry_qte_lockon(bool enable);

  // mod name string for config
  std::string get_mod_name() const override { return "LockOnSettings"; }
  std::string get_human_readable_name() const { return "Lock On Settings"; }
  const char* get_description() const override { return R"(Lock On Settings)"; };

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
	std::unique_ptr<FunctionHook> m_hook1, m_hook2;
	// std::unique_ptr<Patch> patch0, patch1, patch2, patch3;
};
