#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class ReprisalSwap : public Mod {
public:
  ReprisalSwap() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool mod_enabled;
  static bool mid_stance_enabled;
  static uintptr_t jmp_ret1;
  static uintptr_t gpPadUni;

  static uintptr_t jmp_ret2;
  static uintptr_t detour2je;

  // mod name string for config
  std::string get_mod_name() const override { return "ReprisalSwap"; }
  std::string get_human_readable_name() const { return "Parry Reprisals"; }
  //const char* get_description() const override { return R"(Adds a new parry reprisal to the High attack button that causes a soft knockdown and can initiate clashes. After a parry, press High attack.)"; };
  static const char* defaultDescription;
  static const char* hoveredDescription;
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
	std::unique_ptr<FunctionHook> m_hook1, m_hook2;
	// std::unique_ptr<Patch> m_patch;
};
