#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class DodgeBattery : public Mod {
public:
  DodgeBattery() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled;
  static uintptr_t jmp_ret1;
  static uintptr_t gpBattle;
  static uintptr_t PC_INPUT_CHARGE_BAT_ENTER;
  static uintptr_t mCheckInputCharge;
  static uintptr_t mGetBtEffect;
  static uintptr_t SetStatusBatterySE_Flag;
  static uintptr_t dLeavingExceptBattery;
  static uintptr_t dAppearKeyGuide;
  static uintptr_t mPlayCamMotFromCharMot;
  static uintptr_t SoundPrep;
  static uintptr_t SoundSetOutput;
  static uintptr_t SoundPrepPlay;
  static uintptr_t mUpdateLockOnTarget;

  // mod name string for config
  std::string get_mod_name() const override { return "DodgeBattery"; }
  std::string get_human_readable_name() const { return "Dodge Battery"; }
  const char* get_description() const override { return R"(Dodge Battery)"; };

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
	std::unique_ptr<FunctionHook> m_hook1;
	// std::unique_ptr<Patch> m_patch;
};
