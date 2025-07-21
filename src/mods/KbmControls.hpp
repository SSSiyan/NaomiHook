#pragma once

#include "Mod.hpp"
#include "sdk/ReClass.hpp"

struct RAIIHooks {
    RAIIHooks();

    std::unique_ptr<FunctionHook> g_hook_ghm_pad_pr_wii_pad_sampling_callback;
    Patch::Ptr g_hr_cam_angle_horizontal_patch;
    std::unique_ptr<FunctionHook> m_camera_rot_lr;
};

#pragma region GHM_RECLASS
struct Vec2Our {
    float x,y;
};

struct KPADEXStatus
{
  struct
  {
    /* 0x0000 */ struct Vec2Our stick;
    /* 0x0008 */ struct Vec acc;
    /* 0x0014 */ float acc_value;
    /* 0x0018 */ float acc_speed;
  } /* size: 0x001c */ fs;
  struct
  {
    /* 0x001c */ unsigned int hold;
    /* 0x0020 */ unsigned int trig;
    /* 0x0024 */ unsigned int release;
    /* 0x0028 */ struct Vec2Our lstick;
    /* 0x0030 */ struct Vec2Our rstick;
    /* 0x0038 */ float ltrigger;
    /* 0x003c */ float rtrigger;
  } /* size: 0x0024 */ cl;
}; /* size: 0x0040 */

struct KPADStatus
{
  /* 0x0000 */ unsigned int hold;
  /* 0x0004 */ unsigned int trig;
  /* 0x0008 */ unsigned int release;
  /* 0x000c */ struct Vec acc;
  /* 0x0018 */ float acc_value;
  /* 0x001c */ float acc_speed;
  /* 0x0020 */ unsigned char dev_type;
  /* 0x0021 */ char wpad_err;
  /* 0x0022 */ char dpd_valid_fg;
  /* 0x0023 */ unsigned char data_format;
  /* 0x0024 */ struct KPADEXStatus ex_status;
}; /* size: 0x0064 */

struct GHM_PAD_BUTTON
{
  /* 0x0000 */ bool Input;
  /* 0x0001 */ bool RevEdgeInput;
  /* 0x0002 */ bool EdgeInput;
  /* 0x0003 */ bool EdgeRepeatInput;
  /* 0x0004 */ unsigned int RepeatPopCounter;
  /* 0x0008 */ unsigned int RepeatCounter;
}; /* size: 0x000c */

enum GHMGC_PAD_BATTERY_LEVEL
{
  GHMGC_PAD_BATTERY_LEVEL_CRITICAL = 0,
  GHMGC_PAD_BATTERY_LEVEL_LOW = 1,
  GHMGC_PAD_BATTERY_LEVEL_MEDIUM = 2,
  GHMGC_PAD_BATTERY_LEVEL_HIGH = 3,
  GHMGC_PAD_BATTERY_LEVEL_MAX = 4,
  GHMGC_PAD_BATTERY_LEVEL_DEVICE_NONE = 5,
};

enum GHMGC_PAD_CONNECT
{
  GHMGC_PAD_CONNECT_READY = 0,
  GHMGC_PAD_CONNECT_NOT_FOUND = 253,
  GHMGC_PAD_CONNECT_WII_CORE = 0,
  GHMGC_PAD_CONNECT_WII_FREESTYLE = 1,
  GHMGC_PAD_CONNECT_WII_CLASSIC = 2,
  GHMGC_PAD_CONNECT_WII_MPLS = 3,
  GHMGC_PAD_CONNECT_WII_MPLS_FREESTYLE = 4,
  GHMGC_PAD_CONNECT_WII_MPLS_CLASSIC = 5,
  GHMGC_PAD_CONNECT_WII_UNKNOWN = 255,
  GHMGC_PAD_CONNECT_WII_NOT_FOUND = 253,
  GHMGC_PAD_CONNECT_WII_NOT_SUPPORTED = 252,
  GHMGC_PAD_CONNECT_WII_FUTURE = 251,
};

struct GHM_PAD_PAD
{
  /* 0x0000 */ bool Valid;
  /* 0x0001 */ char Padding_1873[3];
  /* 0x0004 */ enum GHMGC_PAD_CONNECT Connect;
  /* 0x0008 */ enum GHMGC_PAD_BATTERY_LEVEL Battery;
  /* 0x000c */ struct GHM_PAD_BUTTON Button[34];
  /* 0x01a4 */ float LStickX;
  /* 0x01a8 */ float LStickY;
  /* 0x01ac */ float RStickX;
  /* 0x01b0 */ float RStickY;
  /* 0x01b4 */ float LTrigger;
  /* 0x01b8 */ float RTrigger;
  /* 0x01bc */ bool VibrationFlag;
  /* 0x01bd */ char Padding_1874[3];
  /* 0x01c0 */ int VibrationTime;
}; /* size: 0x01c4 */

struct PAD_UNI
{
public:
    int32_t N000008AB; //0x0000
    class KPADStatus WiiStatusBuffer[32][1]; //0x0004
}; //Size: 0x0C84
#pragma endregion

constexpr auto KEY_DPAD_UP    = 1 << 0;
constexpr auto KEY_DPAD_LEFT  = 1 << 1;
constexpr auto KEY_RT         = 1 << 2;
constexpr auto KEY_SQUARE     = 1 << 3;
constexpr auto KEY_CROSS      = 1 << 4;
constexpr auto KEY_TRIANGLE   = 1 << 5;
constexpr auto KEY_CIRCLE     = 1 << 6;
constexpr auto KEY_LT         = 1 << 7;
constexpr auto KEY_UNK0       = 1 << 8;
constexpr auto KEY_RB         = 1 << 9;
constexpr auto KEY_SELECT     = 1 << 10;
constexpr auto KEY_UNK1       = 1 << 11;
constexpr auto KEY_START      = 1 << 12;
constexpr auto KEY_LB         = 1 << 13;
constexpr auto KEY_DPAD_DOWN  = 1 << 14;
constexpr auto KEY_DPAD_RIGHT = 1 << 15;
constexpr auto KEY_L3         = 1 << 16;
constexpr auto KEY_R3         = 1 << 17;

class KbmControls : public Mod {
public:
  KbmControls() = default;
  
  void register_raw_input_mouse(HWND handle);
  bool window_proc_handler(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);

  ModCategory get_category() { return ModCategory::GAMEPLAY; };

  // mod name string for config
  std::string get_mod_name() const override { return "KbmControls"; }
  std::string get_human_readable_name() const { return "Keyboard and Mouse Controls"; }
  const char* get_description() const override { return R"(Experimental KBM controls. Has some limitations, and certain NaomiHook settings aren't fully compatible with our KBM.)"; };


  // void custom_imgui_window();

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
  
  const ModToggle::Ptr m_mod_enabled            { ModToggle::create(generate_name("mode_enable")) };
  const ModToggle::Ptr m_block_lockon           { ModToggle::create(generate_name("block_right_stick_lockon")) };
  const ModFloat::Ptr  m_mouse_range            { ModFloat::create(generate_name("mouse_range"), 200.0f) };
  const ModFloat::Ptr  m_base_mouse_sens        { ModFloat::create(generate_name("mouse_base_sensitivity"), 3.0f) };
  const ModFloat::Ptr  m_cams_mouse_sens        { ModFloat::create(generate_name("mouse_cams_sensitivity"), 0.04f) };

  HWND m_wnd               { NULL  };
  bool m_capture_mouse     { false };
  bool m_capture_mouse_old { false };
  bool m_ri_registered     { false };

  ValueList mod_vars {
      *m_mod_enabled,
      *m_mouse_range,
      *m_cams_mouse_sens,
      *m_base_mouse_sens
  };

    std::unique_ptr<RAIIHooks> m_hooks;
private:
  // std::unique_ptr<FunctionHook> m_hook;
};
extern int8_t g_kbm_stance;
