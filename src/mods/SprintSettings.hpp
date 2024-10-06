#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"

class SprintSettings : public Mod {
public:
  SprintSettings() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };
  static bool battleSprint;
  static uintptr_t jmp_ret1;
  static uintptr_t l3Addr;
  static uintptr_t jneAddr;
  static uintptr_t closeQuartersAddr;

  static uintptr_t jmp_ret2;
  static float sprintSpeed;
  static float battleSprintSpeed;

  static uintptr_t jmp_ret3;

  // mod name string for config
  std::string get_mod_name() const override { return "SprintSettings"; }
  std::string get_human_readable_name() const { return "Sprint Settings"; }
  const char* get_description() const override { return R"(Sprint Settings)"; };

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
	std::unique_ptr<FunctionHook> m_hook1, m_hook2, m_hook3;
	// std::unique_ptr<Patch> m_patch;
};
