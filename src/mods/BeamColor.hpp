#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class BeamColor : public Mod {
public:
  BeamColor() = default;

  ModCategory get_category() { return ModCategory::COSMETICS; };

  // mod name string for config
  std::string get_mod_name() const override { return "BeamColor"; }
  // mod name for ui
  std::string get_human_readable_name() const override { return "Beam Color"; }
  // mod description
  const char* get_description() const override { return R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean tempor felis odio, vel volutpat urna viverra id. Mauris luctus vestibulum odio, eu ullamcorper arcu. Cras elementum leo magna, quis aliquam libero posuere id. Maecenas laoreet lobortis elit, sit amet feugiat dui pretium eu. Praesent nunc orci, efficitur quis bibendum et, ultrices a felis. Praesent sagittis posuere finibus. Sed quis magna urna.
Cras commodo justo sem, ut dapibus augue rhoncus nec. Morbi at scelerisque elit. Nulla ullamcorper nunc non dolor tempus mattis. Aenean at suscipit felis. Pellentesque odio tellus, sollicitudin in pulvinar id, iaculis at augue. Sed quis dui neque. Nulla tempor eros at ullamcorper blandit. Curabitur ut elementum magna. Interdum et malesuada fames ac ante ipsum primis in faucibus. Sed orci orci, porta vel ante quis, euismod porttitor nisl. Nulla tristique, mi ut sagittis suscipit, tortor dolor pharetra nunc, sed malesuada velit elit ac sapien. Sed at pulvinar velit. )"; };
  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  //void on_config_load(const utility::Config& cfg) override;
  //void on_config_save(utility::Config& cfg) override;


  //void on_frame() override;
  void on_draw_ui() override;
  //void on_draw_debug_ui() override;
private:
  // std::unique_ptr<FunctionHook> m_function_hook;
};
