#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class WeaponSwitcher : public Mod {
public:
  WeaponSwitcher() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled;
  static bool animations_enabled;
  static uintptr_t jmp_ret1;
  static uintptr_t jmp_ret2;
  static uintptr_t jmp_ret3;
  static int weaponSwitchCooldown;
  // static bool weapon_switcher_ui;

  void WeaponSwapLogic();
  void toggleForceMap(bool enable);
  static bool CanWeaponSwitch(pcItem desiredWeapon);
  void Display_UI();

  // mod name string for config
  std::string get_mod_name() const override { return "WeaponSwitcher"; }
  std::string get_human_readable_name() const { return "Weapon Switcher"; }
  //const char* get_description() const override { return R"(Enable NMH2 styled weapon switching. While in combat, stand still and use the directional pad to select a katana.)"; };
  void render_description() const override;
  static const char* defaultDescription;
  static const char* hoveredDescription;

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
  void on_d3d_reset() override;
private:
	std::unique_ptr<FunctionHook> m_hook1, m_hook2, m_hook3;
	std::unique_ptr<Patch> m_patch;
};
