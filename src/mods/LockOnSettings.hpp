#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class LockOnSettings : public Mod {
public:
  LockOnSettings() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  //////////////////////////////////////////////////
  static bool      lockon_more_actions;
  static bool      lockon_deathblows;
  void             toggle_deathblow_lockon(bool enable);
  static bool      lockon_deathblows_start;
  void			   toggle_deathblow_lockon_start(bool enable);
  static bool      lockon_parry_qtes;
  void			   toggle_parry_qte_lockon(bool enable);
  //////////////////////////////////////////////////
  static bool      horizontal_limit_toggle;
  static uintptr_t horizontal_limit_jmp_ret;
  static float     horizontal_limit_default_search_degrees;
  static float     horizontal_limit_custom_search_degrees;
  //////////////////////////////////////////////////
  static uintptr_t more_actions_jmp_ret;
  static uintptr_t more_actions_jmp_ja;
  static uintptr_t more_actions_jmp_retalt;
  //////////////////////////////////////////////////
  static bool      disable_throws_mod_enabled;
  static uintptr_t disable_throws_jmp_ret1;
  static uintptr_t disable_throws_jmp_jne1;

  static uintptr_t disable_throws_jmp_ret2;
  static uintptr_t disable_throws_jmp_jne2;
  static float     disable_throws_comiss2;
  //////////////////////////////////////////////////

  // mod name string for config
  std::string get_mod_name() const override { return "LockOnSettings"; }
  std::string get_human_readable_name() const { return "Lock On Settings"; }
  // const char* get_description() const override { return R"(Lock On Settings)"; };
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
	std::unique_ptr<FunctionHook> lockon_horizontal_limit_hook, lockon_during_more_actions_hook;
	std::unique_ptr<Patch> patch_toggle_deathblow_during, patch_toggle_deathblow_start, patch_parry_qte_lockon;
	std::unique_ptr<FunctionHook> disable_throws_hook1, disable_throws_hook2;
};
