#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class NMHFixes : public Mod {
public:
  NMHFixes() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };
  static bool disableCrashDumps;
  static bool rankUpCrashFix;
  static uintptr_t rank_up_crash_fix_jmp_ret;
  static uintptr_t rank_up_crash_fix_jmp_je;

  void disableCrashDumpsToggle(bool enabled);
  void deepRankUpCrashFixToggle(bool enabled);

  // mod name string for config
  std::string get_mod_name() const override { return "NMHFixes"; }
  std::string get_human_readable_name() const { return "System Fixes"; }
  //const char* get_description() const override { return R"(System Fixes)"; };
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
	// std::unique_ptr<FunctionHook> m_hook;
	std::unique_ptr<Patch> disable_crash_dumps_patch;
	// std::unique_ptr<Patch> deep_rank_up_crash_fix_patch1, deep_rank_up_crash_fix_patch2;
	std::unique_ptr<FunctionHook> rank_up_crash_fix_hook1;
};
