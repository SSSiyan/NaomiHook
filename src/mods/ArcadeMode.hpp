#pragma once
#if 1
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class ArcadeMode : public Mod {
public:
  ArcadeMode() = default;
  static bool arcade_enabled;
  static bool boss_rush_enabled;
  static uintptr_t jmp_ret1;
  static uintptr_t gpBattle;
  static uintptr_t mSetVisible;
  static bool quickBoot;

  ModCategory get_category() { return ModCategory::STAGE; };

  // mod name string for config
  std::string get_mod_name() const override { return "ArcadeMode"; }
  std::string get_human_readable_name() const { return "Garden of Madness Mode"; }
  // const char* get_description() const override { return R"(Play through the entire game in one sitting while skipping all the stuff in between. Nothing but gameplay in this mode.)"; };
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
  std::unique_ptr<FunctionHook> m_hook1;
};
#endif
