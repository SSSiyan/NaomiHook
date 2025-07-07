#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class HudSettings : public Mod {
public:
  HudSettings() = default;

  static bool hide_drawbpber;
  static bool hide_drawhber;
  static bool hide_drawbattery;
  static bool hide_drawmoney;
  static bool hide_drawtension;
  static bool hide_drawkamae;
  static bool hide_drawspeedmeter;
  static bool hide_drawmap;
  static bool hide_drawsilvia;
  static bool hide_drawlmode;
  static bool hide_drawkeyguide;
  static bool hide_drawcheckpo;
  static void DisplayHUDCheckboxes();
  static void SetHUDFlagsOnFrame();

  static bool custom_lock_on_colour_toggle;
  static uint8_t lockOnColour[3];
  static void SetLockOnColourOnFrame();

  static bool custom_lock_on_block_size_toggle;
  static float lockOnBlockSize;
  static void SetLockOnBlockSizeOnFrame();
  
  ModCategory get_category() { return ModCategory::COSMETICS; };

  // mod name string for config
  std::string get_mod_name() const override { return "HudSettings"; }
  std::string get_human_readable_name() const { return "HUD Settings"; }
  // const char* get_description() const override { return R"(HudSettings Description)"; };
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
  // std::unique_ptr<FunctionHook> m_hook;
};
