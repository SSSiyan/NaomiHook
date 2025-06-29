#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class CameraSettings : public Mod {
public:
  CameraSettings() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled_screenshake;
  static uintptr_t Offset_84BA18;
  static uintptr_t screenshake_jmp_ret1;
  static int customBasicScreenshakeAmount;

  static bool mod_enabled_first_person_x;
  static bool mod_enabled_darkside_x;
  void toggle_first_person_x(bool enable);
  void toggle_darkside_x(bool enable);

  static bool force_fov;
  static bool disable_attack_zoom;
  static float custom_fov;
  static float default_fov;
  static uintptr_t fov_jmp_ret1;
  static uintptr_t fov_jmp_ret2;
  void fov_toggle(bool enable);

  // mod name string for config
  std::string get_mod_name() const override { return "CameraSettings"; }
  std::string get_human_readable_name() const { return "Camera Settings"; }
  // const char* get_description() const override { return R"(Camera Settings)"; };
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
private:
	std::unique_ptr<FunctionHook> m_hook1, m_hook2;
	std::unique_ptr<Patch> m_patch_first_person_x, m_patch_darkside_x;

	std::unique_ptr<FunctionHook> m_fov_hook1, m_fov_hook2;
	std::unique_ptr<Patch> m_fov_patch;
};
