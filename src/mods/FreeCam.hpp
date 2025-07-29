#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class FreeCam : public Mod {
public:
  FreeCam() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };

  static float sens;
  static float modifierSens;
  static float deadZone;
  void toggle(bool enable);
  static bool toggle_pause_enabled;
  void togglePause(bool enable);

  // mod name string for config
  std::string get_mod_name() const override { return "FreeCam"; }
  std::string get_human_readable_name() const { return "Free Cam"; }
  //const char* get_description() const override { return R"(Free Cam)"; };
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
	// std::unique_ptr<FunctionHook> m_hook1;
	std::unique_ptr<Patch> battle_freecam_patch, bike_freecam_patch, pause_all_patch;
};
