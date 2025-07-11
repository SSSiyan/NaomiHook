#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class PlayerTracker : public Mod {
public:
  PlayerTracker() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };

  static bool imguiPopout;

  // void custom_imgui_window();

  // mod name string for config
  std::string get_mod_name() const override { return "PlayerTracker"; }
  std::string get_human_readable_name() const { return "Player Tracker"; }
  const char* get_description() const override { return R"(View info about the player character)"; };

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  //void on_config_load(const utility::Config& cfg) override;
  //void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	// std::unique_ptr<FunctionHook> m_hook1;
	// std::unique_ptr<Patch> m_patch;
};

static const char* wrestlingNames[16] {
    "Dragon Suplex",            // Skill Catch 0 Default 1
    "Brain Buster",             // Skill Catch 1 Default 2
    "default##2",               // Skill Catch 2
    "default##3",               // Skill Catch 3
    "Hurricanrana",             // Skill Catch 4  VHS Vol.3
    "Power Bomb",               // Skill Catch 5  VHS Vol.4
    "Brain Buster Slam",        // Skill Catch 6  VHS Vol.2
    "Quebradora Con Giro",      // Skill Catch 7  VHS Vol.1
    "German Suplex",            // Skill Catch 8  Destroyman
    "Tiger Suplex",             // Skill Catch 9  Henry Cooldown
    "Belly To Belly",           // Skill Catch 10 Holly Summers
    "Front Neck Chancery Drop", // Skill Catch 11 Death Metal
    "Captured",                 // Skill Catch 12 Shinobu
    "Reverse Armsault",         // Skill Catch 13 Dr.Peace
    "Double Arm Suplex",        // Skill Catch 14 Harvey
    "Double Wrist Armsault",    // Skill Catch 15 Bad Girl
};

static const char* k7Names[7] {
    "Memory of Demon - Jumping Slash",       // K7 0
    "Memory of Child - Sprint",              // K7 1
    "Memory of Three - Mini Map",            // K7 2
    "Memory of Woman - Darkside Extension",  // K7 3
    "Memory of Mask - Wrestling Grab Range", // K7 4
    "Memory of Tattoo - Total Rank Bonus",   // K7 5
    "Memory of White - Jumping Down Attack", // K7 6
};