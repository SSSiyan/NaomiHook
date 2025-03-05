#pragma once
#if 1
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class StageWarp : public Mod {
public:
  StageWarp() = default;

  struct Stage {
	const char* name;
	int arrayItem;
	const char* info;
	const char* longerInfo;
  };

  static std::array<Stage, 78> stage_items;

  ModCategory get_category() { return ModCategory::STAGE; };

  // mod name string for config
  std::string get_mod_name() const override { return "StageWarp"; }
  std::string get_human_readable_name() const { return "Stage Warp"; }
  //const char* get_description() const override { return R"(Teleport to any stage in the game. 'Wii/Unused' contains warps to stages which aren't included with the Steam release of NMH1, so to make use of these, you'll need to have the files from the Wii version.)"; };

  void render_description() const override;

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  //void on_config_load(const utility::Config& cfg) override;
  //void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  //void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	// std::unique_ptr<FunctionHook> m_hook1;
	// std::unique_ptr<Patch> m_patch;
};
#endif
