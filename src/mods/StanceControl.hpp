#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class StanceControl : public Mod {
public:
  StanceControl() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled;
  static bool invert_input;
  static bool invert_mid;
  static bool show_new_ui;
  static bool edit_old_ui;
  static uintptr_t gpPad;
  static uintptr_t jmp_ret1;

  static float r2Mult;
  static float r2Sub;
  static float highBound;
  static float lowBound;
  static float invert;

  static float r2MultGuard;
  static float highBoundGuard;
  static float lowBoundGuard;
  static float invertGuard;

  static uintptr_t jmp_ret2;
  static uintptr_t jmp2je;
  static bool wasL3PressedLastFrame;

  static uintptr_t jmp_ret3;
  static uintptr_t jmp_jne3;
  static uintptr_t clashing;

  static bool swapIdleStances;
  void toggleSwapIdleStances(bool enable);

  void toggle(bool enable);
  void toggle_display_edit(bool enable);

  static bool mod_enabled_disable_combo_extend_speedup;
  void toggle_disable_combo_extend_speedup(bool enable);

  static bool mod_enabled_gear_system;
  static bool gear_system_holds;
  void disable_cam_reset(bool enable);
  void GearControls(mHRPc* player);
  float SetSmoothStance(mHRPc* player);

  static bool mod_enabled_faster_nu_lows;
  static uintptr_t jmp_ret4;

  static bool mod_enabled_stance_guards;
  static uintptr_t jmp_ret5;
  static uintptr_t jmp5je;

  static const char* defaultDescription;
  static const char* hoveredDescription;

  // mod name string for config
  std::string get_mod_name() const override { return "StanceControl"; }
  std::string get_human_readable_name() const { return "Stance Control"; }
  //const char* get_description() const override { return R"(Manually adjust the current stance using R2/RT like you can with motion controls. Our version of this also restores the original and unused Low stance for all beam katanas except the MK3.)"; };
  void render_description() const override;

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
	std::unique_ptr<FunctionHook> m_hook1, m_hook2, m_hook3, m_hook4, m_hook5;
	std::unique_ptr<Patch> m_patch1, m_patch2, m_patch3, m_patch4;
	std::unique_ptr<Patch> patch_swap_idle_stance1, patch_swap_idle_stance2;
	std::unique_ptr<Patch> patch_disable_combo_extend_speedup;
	std::unique_ptr<Patch> patch_disable_cam_reset;
};
