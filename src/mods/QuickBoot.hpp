#pragma once
#if 1
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class QuickBoot : public Mod {
public:
  QuickBoot() = default;
  static bool mod_enabled;

  static const char* originalBootStage;
  static int newBootStageIndex;

  static std::vector<const char*> stage_names; // Just names for asm pushes
  static std::vector<std::string> stage_display_names; // Names + descriptions
  static std::vector<const char*> stage_display_names_cstr; // ImGui acceptable names + descriptions

  ModCategory get_category() { return ModCategory::STAGE; };

  // mod name string for config
  std::string get_mod_name() const override { return "QuickBoot"; }
  std::string get_human_readable_name() const { return "Quick Boot"; }
  const char* get_description() const override { return R"(Load into the stage of your choice on boot rather than the default Motel.
This skips the intro toilet sequences and loads your last checkpoint. Disable this when you are starting a new game!)"; };

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
  // std::unique_ptr<FunctionHook> m_hook1;
};
#endif
