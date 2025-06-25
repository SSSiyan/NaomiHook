#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class DodgeSettings : public Mod {
public:
  DodgeSettings() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  /////////////////////////////////////////////////////////////
  static bool      roll_forward_mod_enabled;
  static uintptr_t roll_forward_jmp_ret1;
  static uintptr_t roll_forward_Offset_LeftStickUp;
  static uintptr_t roll_forward_Offset_LeftStickLeft;
  static uintptr_t roll_forward_Offset_JNE_1;

  static uintptr_t roll_forward_jmp_ret2;

  static uintptr_t roll_forward_jmp_ret3;
  static uintptr_t roll_forward_Offset_GetEdgeButton;
  static uintptr_t roll_forward_Offset_JNE3;

  static uintptr_t roll_forward_jmp_ret4;
  static uintptr_t roll_forward_Offset_mCheckCanOperate;
  /////////////////////////////////////////////////////////////
  static bool disable_darkstep_slowmo_mod_enabled;
  void toggle_disable_slowmo_darkstep(bool enable);
  /////////////////////////////////////////////////////////////
  static bool      dodge_battery_mod_enabled;
  static uintptr_t dodge_battery_jmp_ret1;
  static uintptr_t dodge_battery_gpBattle;
  static uintptr_t dodge_battery_PC_INPUT_CHARGE_BAT_ENTER;
  static uintptr_t dodge_battery_mCheckInputCharge;
  static uintptr_t dodge_battery_mGetBtEffect;
  static uintptr_t dodge_battery_SetStatusBatterySE_Flag;
  static uintptr_t dodge_battery_dLeavingExceptBattery;
  static uintptr_t dodge_battery_dAppearKeyGuide;
  static uintptr_t dodge_battery_mPlayCamMotFromCharMot;
  static uintptr_t dodge_battery_SoundPrep;
  static uintptr_t dodge_battery_SoundSetOutput;
  static uintptr_t dodge_battery_SoundPrepPlay;
  static uintptr_t dodge_battery_mUpdateLockOnTarget;
  /////////////////////////////////////////////////////////////
  static bool      roll_rotation_mod_enabled;
  static uintptr_t roll_rotation_jmp_ret1;
  static uintptr_t roll_rotation_mRotate2LockOnTarget;
  static uintptr_t roll_rotation_GetMotionRunState;
  /////////////////////////////////////////////////////////////
  static bool darkstep_invinc_mod_enabled;
  static uintptr_t darkstep_invinc_jmp_ret1;
  static uintptr_t darkstep_invinc_jmp_ja1;
  static uintptr_t darkstep_invinc_CBgCtrl;
  static uintptr_t darkstep_invinc_gpBattle;
  static uintptr_t darkstep_check_throw_attack;
  /////////////////////////////////////////////////////////////

  static const char* defaultDescription;
  static const char* hoveredDescription;
  // mod name string for config
  std::string get_mod_name() const override { return "DodgeSettings"; }
  std::string get_human_readable_name() const { return "Dodge Settings"; }
  //const char* get_description() const override { return R"(Dodge Settings)"; };
  void render_description() const override;

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
	std::unique_ptr<FunctionHook> roll_forward_hook1, roll_forward_hook2, roll_forward_hook3, roll_forward_hook4;
	std::unique_ptr<Patch> disable_slowmo_darkstep_patch;
	std::unique_ptr<FunctionHook> m_hook_dodge_battery1;
	std::unique_ptr<FunctionHook> m_hook_roll_rotation1;
	std::unique_ptr<FunctionHook> darkstep_invinc_m_hook1;
};
