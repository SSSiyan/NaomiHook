#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class ChargeSubsBattery : public Mod {
public:
  ChargeSubsBattery() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled;
  static uintptr_t mSubBattery;
  static uintptr_t jmp_ret1;
  static int BatterySubCounter;
  static constexpr int subWhenOver = 0;

  static uintptr_t jmp_ret2;

  static uintptr_t jmp_ret3;

  static uintptr_t jmp_ret4;
  static uintptr_t midChargeSelector;
  static uintptr_t midChargeSelectorSkip;

  // mod name string for config
  std::string get_mod_name() const override { return "ChargeSubsBattery"; }
  std::string get_human_readable_name() const { return "Reworked Charged Slashes"; }
  const char* get_description() const override { return R"(Shuffles around the rules on charged slash cost and their rewards. Frame-perfect releases of Charged Slashes consume no battery, but the act of charging a slash will constantly drain the battery.
MK-III battery upgrade also has had its Heroes Paradise nerf applied, meaning ordinary slashes are free but charged slashes follow standard drain rules.)"; };

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
	std::unique_ptr<FunctionHook> m_hook1, m_hook2, m_hook3, m_hook4;
	// std::unique_ptr<Patch> m_patch;
};
