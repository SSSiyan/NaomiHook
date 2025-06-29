#pragma once

#include "Mod.hpp"
#include "sdk/ReClass.hpp"

struct GHM_PAD_PAD;

struct RAIIHooks {
    RAIIHooks();

    std::unique_ptr<FunctionHook> g_hook_ghm_pad_pr_wii_pad_sampling_callback;
    Patch::Ptr g_hr_cam_angle_horizontal_patch;
    std::unique_ptr<FunctionHook> m_camera_rot_lr;
};

class KbmControls : public Mod {
public:
  KbmControls() = default;
  
  void register_raw_input_mouse(HWND handle);
  bool window_proc_handler(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);

  ModCategory get_category() { return ModCategory::GAMEPLAY; };

  // mod name string for config
  std::string get_mod_name() const override { return "KbmControls"; }
  std::string get_human_readable_name() const { return "KbmControls"; }
  const char* get_description() const override { return R"(KbmControls Description)"; };


  void custom_imgui_window();

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
  
  const ModToggle::Ptr m_mod_enabled            { ModToggle::create(generate_name("mode_enable")) };
  const ModFloat::Ptr  m_mouse_range            { ModFloat::create(generate_name("mouse_range"), 200.0f) };
  const ModFloat::Ptr  m_base_mouse_sens        { ModFloat::create(generate_name("mouse_base_sensitivity"), 3.0f) };
  const ModFloat::Ptr  m_cams_mouse_sens        { ModFloat::create(generate_name("mouse_cams_sensitivity"), 0.04f) };

  HWND m_wnd               { NULL  };
  bool m_capture_mouse     { false };
  bool m_capture_mouse_old { false };
  bool m_ri_registered     { false };

  ValueList mod_vars {
      *m_mod_enabled,
      *m_mouse_range,
      *m_cams_mouse_sens,
      *m_base_mouse_sens
  };

    std::unique_ptr<RAIIHooks> m_hooks;
private:
  // std::unique_ptr<FunctionHook> m_hook;
};
