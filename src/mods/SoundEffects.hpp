#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class SoundEffects : public Mod {
public:
  SoundEffects() = default;
  
  ModCategory get_category() { return ModCategory::SOUND; };
  static bool disable_death_screams;
  static bool custom_death_screams;
  static uintptr_t jmp_ret1;
  static uintptr_t ghm_snd_prPrep;

  void ToggleDeathScreamExtras(bool enable);
  void GetCustomScreamSound(int soundID);
  static int temp_sound_id;

  // mod name string for config
  std::string get_mod_name() const override { return "SoundEffects"; }
  std::string get_human_readable_name() const { return "SoundEffects"; }
  const char* get_description() const override { return R"(SoundsEffects.)"; };

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
	std::unique_ptr<Patch> m_patch1, m_patch2, m_patch3;
};
