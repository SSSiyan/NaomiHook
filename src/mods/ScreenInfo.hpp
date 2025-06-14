#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"

#pragma region reclass
// Created with ReClass.NET 1.2 by KN4CK3R

class ScreenInfoGame
{
public:
    uint32_t refcount; //0x0004
    char pad_0008[8]; //0x0008
    uint32_t screen_left; //0x0010
    uint32_t screen_top; //0x0014
    uint32_t screen_right; //0x0018
    uint32_t screen_bottom; //0x001C

    virtual void Function0();
}; //Size: 0x0020
#pragma endregion

class ScreenInfo : public Mod {
public:
  ScreenInfo() = default;
  
  ModCategory get_category() { return ModCategory::GAMEPLAY; };

  // mod name string for config
  std::string get_mod_name() const override { return "ScreenInfo"; }
  std::string get_human_readable_name() const { return "ScreenInfo"; }
  const char* get_description() const override { return R"(ScreenInfo Description)"; };

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  static ScreenInfoGame* get_screen_info();

};

