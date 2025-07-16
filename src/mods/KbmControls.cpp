#include "KbmControls.hpp"
#include "utility/Patch.hpp"
#include <map>
#include "imgui_internal.h"

static KbmControls* g_kbm { nullptr };
static bool g_block_inputs { false };

typedef void(__fastcall *PADSetHideMouseCursor_ptr)(bool s, bool lock);
static PADSetHideMouseCursor_ptr PADSetHideMouseCursor_ee { nullptr };

struct MouseRaw {
    int x = 0;
    int y = 0;
    int wheel = 0;
};

static MouseRaw g_mouser{};

class hrTaskPtrStatic
{
public:
    class hrTask *hrtsk; //0x0000
}; //Size: 0x0004

// TODO(): siyan fixed the offsets but im too lazy to change it all now
class hrTask
{
public:
    char pad_0000[104]; //0x0000
    void *shootyGame; //0x0068
    char pad_006C[44]; //0x006C
    uint32_t state; //0x0098
}; //Size: 0x009C

hrTaskPtrStatic* g_hrtask_ptr = nullptr;

#if 0
inline void mouse_set_visible(BOOL visible)
{


    ShowCursor(FALSE);

    CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
    if (!GetCursorInfo(&info))
    {
        throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetCursorInfo");
    }

    const bool isvisible = (info.flags & CURSOR_SHOWING) != 0;

    if (isvisible != visible)
    {
        ShowCursor(FALSE);
        auto& io = ImGui::GetIO();
        io.MouseDrawCursor = visible;
    }
    else {
        ShowCursor(TRUE);
    }
}

inline void mouse_center(HWND window) {
    RECT window_rect;
    GetWindowRect(window, &window_rect);

    int centerX = (window_rect.left + window_rect.right)  / 2;
    int centerY = (window_rect.top  + window_rect.bottom) / 2;

    SetCursorPos(centerX, centerY);
}
#endif
inline void mouse_capture(HWND window) {
    RECT window_rect;

    GetClientRect(window, &window_rect);
    ClientToScreen(window, (POINT*)&window_rect.left);
    ClientToScreen(window, (POINT*)&window_rect.right);
    ClipCursor(&window_rect);
}

inline void mouse_release() {
    ClipCursor(NULL);
}

static void toggle_mouse(HWND window, bool capturing) {
    if (capturing) {
        mouse_capture(window);
    }
    else {
        mouse_release();
    }
}

#if 0
static void m_camera_rot_lr(float a1) noexcept {
    auto battle = nmh_sdk::get_mHRBattle();
    auto mouse = g_framework->m_mouse.get();
    auto state = mouse->GetState();

    static constexpr float g_hsens = 0.01f;
    static constexpr float g_ysens = 0.5f;

    const float angle = (float)state.x * g_hsens + battle->mCamStatus.angle;
    battle->mCamStatus.angle = angle;
    
    const float angle_x = (float)state.y * g_ysens + battle->mCamStatus.angleX;
    battle->mCamStatus.angleX = angle_x;

    battle->mCamStatus.inputMove = 1; // idk what this does
    nmh_sdk::get
}

static void m_camera_rot_ud() noexcept {
    return;
    auto battle = nmh_sdk::get_mHRBattle();
    auto mouse = g_framework->m_mouse.get();
    auto state = mouse->GetState();
    const float g_hsens = 0.06f;
    const float angle = (float)state.y * g_hsens + battle->mCamStatus.angleX;
    battle->mCamStatus.angleX = angle;
    //battle->mCamStatus.angleX = glm::sin((ImGui::GetTime() * 0.5) + 0.5);
    battle->mCamStatus.inputMove = 1;
}


#endif

struct Action {
    const char* name{};
    union {
        uint32_t code {};
        float axis; // TODO() uhh do we need it?
    };
    void (*act)(KPADEXStatus * exst) {nullptr};
    using AFptr = decltype(act);
};

struct InputMap {
    struct ActionPair {
        ModKey::Ptr first;
        Action second;
    };
    //std::map<ModKey::Ptr, Action> actions;
    std::vector<ActionPair> actions;

    std::string cfg_name(const char* name) {
        std::string cfg_entry { name };

        for (char& c : cfg_entry) {
            if (c == ' ') {
                c = '_';
            }
            else {
                c = std::tolower(c);
            }
        }
        return "kbm_bind_" + cfg_entry;
    }

    void input_map(const char* name, ImGuiKey default_value, Action::AFptr act) {
        actions.emplace_back(ModKey::create(cfg_name(name), default_value), Action{.name = name, .act = act});
    }
    void input_map(const char* name, ImGuiKey default_value, uint32_t code) {
        actions.emplace_back(ModKey::create(cfg_name(name), default_value), Action{.name = name, .code = code});
    }
};

InputMap g_input_map {};


static float linear_map(float edge0, float edge1, float x) {
    // Scale, and clamp x to 0..1 range
    x = ((x - edge0) / (edge1 - edge0));

    return glm::clamp(x, 0.0f, 1.0f); // Simply return x instead of applying the cubic polynomial
}


static glm::vec2 g_mouse_delta {};
static glm::vec2 g_mouse_delta_cam {};
static PAD_UNI* g_ppad {nullptr};
KPADEXStatus* g_exstatus {nullptr};

void shooty_game_fix(KPADEXStatus* ext) {
    if (!g_hrtask_ptr) {
        return;
    }
    if (!g_hrtask_ptr->hrtsk) {
        return;
    }
    if (auto hrtask = g_hrtask_ptr->hrtsk) {
        // shooty game is 27
        if (hrtask->state == 27) {
            ext->cl.lstick.x += ext->cl.rstick.x * 2.0f;
            ext->cl.lstick.y += ext->cl.rstick.y * 2.0f;
        }
    }
}

void __cdecl ghm_pad_prWiiPadSamplingCallback_(void* status, void* context) {

    uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
    if(!g_kbm->m_hooks) { return; }
    g_kbm->m_hooks->g_hook_ghm_pad_pr_wii_pad_sampling_callback->get_original<decltype(ghm_pad_prWiiPadSamplingCallback_)>()(status, context);
    if(g_block_inputs) { return; }

    auto ppad = g_ppad;
    KPADEXStatus* exstatus = &ppad->WiiStatusBuffer[0][0].ex_status;
    g_exstatus = exstatus;

    glm::vec2 mouse { (float)g_mouser.x, (float)g_mouser.y };
    mouse *= g_kbm->m_base_mouse_sens->value();

    g_mouse_delta.x = mouse.x;
    g_mouse_delta.y = mouse.y;

    g_mouse_delta_cam = g_mouse_delta;

    //append_ui_log(fmt::format("g_mouse_delta=({},{})", g_mouse_delta.x, g_mouse_delta.y));

    const float range = g_kbm->m_mouse_range->value();
    exstatus->cl.rstick.x += (linear_map(-range, range, mouse.x) - 0.5f) * 2.0f;
    exstatus->cl.rstick.y += (linear_map(-range, range, mouse.y) - 0.5f) * -2.0f;
    exstatus->fs.stick = exstatus->cl.rstick;

    for (auto& action : g_input_map.actions) {
        if (action.first->is_key_down()) {
            auto act = action.second.act;
            if (act) {
                act(exstatus);
            }
            else {
                exstatus->cl.hold |= action.second.code;
            }
        }
    }
    // append_ui_log(fmt::format("cl_rstick=({},{})", exstatus->cl.rstick.x, exstatus->cl.rstick.y));

    // i am not a clever man
    shooty_game_fix(exstatus);

    g_mouse_delta = glm::vec2(0.0);
    g_mouser.x = 0;
    g_mouser.y = 0;
}

static void MOVE2_SetCameraYAngleRate(float a1) noexcept {

    if(!g_kbm->m_hooks) {return;}
    HrCamera* camera = nmh_sdk::get_HrCamera();
    mHRPc* pc = nmh_sdk::get_mHRPc();
    assert(camera != NULL);
    assert(pc != NULL);

    if (camera->MAIN.Mode != HRCAMERA_MODE::HRCAMERA_MODE_MOVE2) {
        return;
    }

    // NOTE(): hack so deathblowjobs wont have camera spazz out when waggling mouse
    if(pc->mPcStatus.finishMotNo > 0) {
        return;
    }
#if 0
    auto mouse = g_framework->m_mouse->GetState();
    if (mouse.positionMode != DirectX::Mouse::MODE_RELATIVE) {
        return;
    }
#endif

    //float angle = glm::smoothstep(-200.0f, 200.0f, (float)state.x * 0.5f);
    //ImVec2 mouse = ImGui::GetIO().MouseDelta;

    const float sensitivity = g_kbm->m_cams_mouse_sens->value();
    float angle = - g_mouse_delta_cam.x * sensitivity * 0.04f;
    float angle_y = g_mouse_delta_cam.y * sensitivity * 0.05f;

    if (glm::length(g_mouse_delta_cam) <= 0.001f) {
        angle = g_exstatus->cl.rstick.x * sensitivity;
        angle_y = g_exstatus->cl.rstick.y * sensitivity;
    }

#if 0
    glm::vec2 mouse { (float)g_mouse_delta.x, (float)g_mouse_delta.y };
    angle = linear_map(-200.0f, 200.0f, (float)mouse.x * -0.2f);
    angle = angle - 0.5f;
    angle *= 2.0f;

    //const float angle_y = glm::clamp((float)state.y * 0.04f, -glm::pi<float>() * 0.4f, glm::pi<float>() * 0.4f);
    //float angle_y = glm::smoothstep(-200.0f, 200.0f, (float)state.y * 0.5f);
    angle_y = linear_map(-200.0f, 200.0f, (float)mouse.y * 0.5f);
    angle_y = angle_y - 0.5f;
    angle_y *= 2.0f;
#endif


    // TODO(deep): fix negative accel
    camera->MAIN.mov2.CamAngle      += angle;//glm::clamp(camera->m.mov2.CamAngle + angle, -glm::pi<float>(), glm::pi<float>());
    camera->MAIN.mov2.CamYAngleRate = glm::clamp(camera->MAIN.mov2.CamYAngleRate + angle_y, -0.09f, glm::pi<float>() * 0.6f);


    g_mouser.x = 0;
    g_mouser.y = 0;
    g_mouse_delta_cam.x = 0;
    g_mouse_delta_cam.y = 0;
}

// clang-format off
naked void detour_MOVE2_SetCameraYAngleRate() {
    __asm {
        sub esp, 4
        movss [esp], xmm1
        push eax
        mov eax, [esp+4]
        call MOVE2_SetCameraYAngleRate
        pop eax
        add esp,4
        ret
    }
}


#if 0
static uintptr_t jmpback_ghm_gamepad_callback { 0x009F7E75 }; // TODO(): offset!1!!!
naked void detour_ghm_gamepad_callback() {
    __asm {
        mov ecx,[edi+174h]
        pushad
        push ecx
        call fucking_buttons_callback_holy_shit
        pop ecx
        popad
        jmp DWORD PTR [jmpback_ghm_gamepad_callback]
    }
}
#endif

#if 0
naked void detour_camera_rot_lr() {
    __asm {
        ;sub esp, 4
        ;movss [esp], xmm1
        ;push eax
        ;mov eax, [esp+4]
        call m_camera_rot_lr
        ;pop eax
        ;add esp, 4
        ret
    }
}
naked void detour_camera_rot_ud() {
    __asm {
        call m_camera_rot_ud
        ret
    }
}
#endif

// clang-format on

RAIIHooks::RAIIHooks() {
    uintptr_t base = g_framework->get_module().as<uintptr_t>();

    static constexpr ptrdiff_t pad_sampling_cb_offset = 0x5F7CD0;

    g_hook_ghm_pad_pr_wii_pad_sampling_callback =
        std::make_unique<FunctionHook>(base + pad_sampling_cb_offset, &ghm_pad_prWiiPadSamplingCallback_); // TODO: offset
    if (!g_hook_ghm_pad_pr_wii_pad_sampling_callback->create()) {
        throw "Failed to hook ghm_pad_pr_wii_pad_sampling_callback()";
    }

    static constexpr ptrdiff_t hr_cam_angle_hor_offset = 0x3B4D34;
    g_hr_cam_angle_horizontal_patch                    = Patch::create_nop(base + hr_cam_angle_hor_offset, 8);

    static constexpr ptrdiff_t set_camera_y_angle_offset = 0x3EA110;
    m_camera_rot_lr = std::make_unique<FunctionHook>(base + set_camera_y_angle_offset, &detour_MOVE2_SetCameraYAngleRate);
    if (!m_camera_rot_lr->create()) {
        throw "Failed to hook camera rotate lr";
    }
}

void KbmControls::register_raw_input_mouse(HWND handle) {
    if (!m_mod_enabled->value() || m_ri_registered) {
        return;
    }
    // Registering raw input devices
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short)0x02)
#endif

    // We're configuring just one RAWINPUTDEVICE, the mouse,
    // so it's a single-element array (a pointer).
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags     = RIDEV_INPUTSINK;
    rid[0].hwndTarget  = handle;
    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
// End of resgistering.
    m_ri_registered = true;
}

bool KbmControls::window_proc_handler(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    static bool& draw_ui = g_framework->m_draw_ui;
    if (draw_ui || this == nullptr) {
        return false;
    }
    // TODO(): can sometimes land here without being fully initialized yet
    if (!m_mod_enabled->value()) {
        return false;
    }
    if (message == WM_KILLFOCUS) {
        m_capture_mouse = false;
        m_capture_mouse_old = true;
        g_block_inputs = true;
        PADSetHideMouseCursor_ee(m_capture_mouse, m_capture_mouse);
        toggle_mouse(m_wnd, m_capture_mouse);
    }
    if (message == WM_SETFOCUS) {
        m_capture_mouse = true;
        m_capture_mouse_old = false;
        g_block_inputs = false;
    }
    if (message == WM_INPUT) {
        if(g_block_inputs) { return false; }
        size_t size = sizeof(RAWINPUT);
        static RAWINPUT raw[sizeof(RAWINPUT)];
        GetRawInputData((HRAWINPUT)l_param, RID_INPUT, raw, &size, sizeof(RAWINPUTHEADER));
        if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {

            if (raw->header.dwType == RIM_TYPEMOUSE) {
                g_mouser.x = raw->data.mouse.lLastX;
                g_mouser.y = raw->data.mouse.lLastY;

                if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
                    g_mouser.wheel = (*(short*)&raw->data.mouse.usButtonData) / WHEEL_DELTA;
            }
        }
        return true;
    }
    return false;
}

//void KbmControls::custom_imgui_window(){}

std::optional<std::string> KbmControls::on_initialize() {
    g_kbm = this;
    uintptr_t base = g_framework->get_module().as<uintptr_t>();

    uintptr_t offset = (uintptr_t)(base + (ptrdiff_t)0x1BECC0);
    PADSetHideMouseCursor_ee = (PADSetHideMouseCursor_ptr)offset;

    auto ptr = (base+0x849D10);
    g_ppad = (PAD_UNI*)ptr;

    g_hrtask_ptr = (hrTaskPtrStatic*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x841414);

    // NOTE(deep): order matters unfortunately
    g_input_map.input_map("Forward", ImGuiKey_W, 
        [](KPADEXStatus* ext) { 
            ext->cl.lstick.y = 1.0f;  /* ext->cl.hold = KEY_DPAD_UP; */ 
            
            // check bike
            if (mHRPc* pc = nmh_sdk::get_mHRPc()) {
                if (pc->mInputMode == 5) {
                    ext->cl.hold |= KEY_CROSS;
                }
            }
        });
    g_input_map.input_map("Back", ImGuiKey_S, 
        [](KPADEXStatus* ext) { 
            ext->cl.lstick.y = -1.0f; /* ext->cl.hold = KEY_DPAD_DOWN; */

            // check bike
            if (mHRPc* pc = nmh_sdk::get_mHRPc()) {
                if (pc->mInputMode == 5) {
                    ext->cl.hold |= KEY_CIRCLE;
                }
            }
        });
    g_input_map.input_map("Left", ImGuiKey_A, 
        [](KPADEXStatus* ext) { ext->cl.lstick.x = -1.0f; /* ext->cl.hold = KEY_DPAD_LEFT; */});
    g_input_map.input_map("Right", ImGuiKey_D, 
        [](KPADEXStatus* ext) { ext->cl.lstick.x = 1.0f;  /* ext->cl.hold = KEY_DPAD_RIGHT; */});

    g_input_map.input_map("Accept", ImGuiKey_Enter,     KEY_START);
    g_input_map.input_map("Cancel", ImGuiKey_Backspace, KEY_SELECT);

    g_input_map.input_map("Skip Scene", ImGuiKey_Backspace, KEY_SELECT);
    g_input_map.input_map("Open Menu",  ImGuiKey_Tab,       [](KPADEXStatus* ext) { 
        ext->cl.hold |= KEY_SELECT;
        ext->cl.hold |= KEY_START;
        static uint32_t* ass3 = (uint32_t*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x849D14);//(uint32_t*)(0x00C49D14);
        *ass3 |= 0x10;
    });

    g_input_map.input_map("Target Lock On", ImGuiKey_LeftShift, [](KPADEXStatus* ext) { 
        ext->cl.hold |= KEY_LT; ext->cl.ltrigger = 255.0f; 

        mHRPc* pc = nmh_sdk::get_mHRPc();
        // TODO(): dont want to bother cheking each ugly case, seems to fix deathblows and tsubazerings
        bool null_rstick = nmh_sdk::CheckCanAttack();//!nmh_sdk::CheckTsubazering(-1);

        //if (!nmh_sdk::CheckTsubazering(-1) /* clashing */ ) {
        if(!g_kbm) {return;}
        if (g_kbm->m_block_lockon->value() == true) {
            if (null_rstick && (!nmh_sdk::CheckTsubazering(-1))) {
                ext->cl.rstick.x = 0;
                ext->cl.rstick.y = 0;
            }
        }
        static unsigned int* ass = (unsigned int*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x8761D0);
        *ass |= 128;
        static float* ass1 = (float*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x8761EC);
        *ass1 = 255.0f;
        static uint32_t* ass3 = (uint32_t*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x849D14);//(uint32_t*)(0x00C49D14);
        *ass3 |= 0x2000;
    } );

    g_input_map.input_map("Low Slash Mouse", ImGuiKey_MouseLeft,   [](KPADEXStatus* ext) { 
        // NOTE(): hack to stop bomb spam in shmup :(
        if (!g_hrtask_ptr) {
            return;
        }
        if (!g_hrtask_ptr->hrtsk) {
            return;
        }
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) { return; }
        }
        ext->cl.hold |= KEY_SQUARE; 
    });
    g_input_map.input_map("High Slash Mouse", ImGuiKey_MouseRight, [](KPADEXStatus* ext) { ext->cl.hold |= KEY_TRIANGLE; });

    g_input_map.input_map("Low Slash Keyboard",    ImGuiKey_C, [](KPADEXStatus* ext) { ext->cl.hold |= KEY_SQUARE; });
    g_input_map.input_map("High Slash Keyboard",   ImGuiKey_V, [](KPADEXStatus* ext) { ext->cl.hold |= KEY_TRIANGLE; });

    g_input_map.input_map("High Melee Attack", ImGuiKey_Q, [](KPADEXStatus* ext) { ext->cl.hold |= KEY_CIRCLE; });
    g_input_map.input_map("Low Melee Attack",  ImGuiKey_E, [](KPADEXStatus* ext) { ext->cl.hold |= KEY_CROSS; });

    // WARNING(deep): must be after lock on bind
    g_input_map.input_map("Emergency Evade", ImGuiKey_Space, [](KPADEXStatus* ext) { 
        if (ext->cl.hold & KEY_LT) { // is lockon down?
            ext->cl.rstick = ext->cl.lstick;
        }
            if (mHRPc* pc = nmh_sdk::get_mHRPc()) {
                if (pc->mInputMode == 5) {
                    ext->cl.hold |= KEY_RT;
                    ext->cl.rtrigger = 255.0f;

                }
            }
    });

    g_input_map.input_map("Battery Charge Start", ImGuiKey_R, KEY_LB);
    g_input_map.input_map("Camera Reset", ImGuiKey_LeftAlt, KEY_RB);

#if 0
    g_input_map.input_map("Up Arrow",    ImGuiKey_UpArrow,    KEY_DPAD_UP);
    g_input_map.input_map("Down Arrow",  ImGuiKey_DownArrow,  KEY_DPAD_DOWN);
    g_input_map.input_map("Left Arrow",  ImGuiKey_LeftArrow,  KEY_DPAD_LEFT);
    g_input_map.input_map("Right Arrow", ImGuiKey_RightArrow, KEY_DPAD_RIGHT);
#endif


    // resident evil 4 pc 2007 reference
    g_input_map.input_map("Camera Up", ImGuiKey_Keypad8, [](KPADEXStatus* ext) {
        ext->cl.rstick.y = 1.0f;
    });
    g_input_map.input_map("Camera Down", ImGuiKey_Keypad5, [](KPADEXStatus* ext) {
        ext->cl.rstick.y = -1.0f;
    });
    g_input_map.input_map("Camera Left", ImGuiKey_Keypad4, [](KPADEXStatus* ext) {
        ext->cl.rstick.x = -1.0f;
    });
    g_input_map.input_map("Camera Right", ImGuiKey_Keypad6, [](KPADEXStatus* ext) {
        ext->cl.rstick.x = 1.0f;
        //ext->cl.rstick.x = -1.0f;
    });

    g_input_map.input_map("Switch to Blood Berry", ImGuiKey_1,   [](KPADEXStatus* ext){ 
        uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
        if (dPadInputsAddr) {
            int8_t* dPadInput = (int8_t*)dPadInputsAddr;
            *dPadInput = DPAD_UP;
        }
    });
    g_input_map.input_map("Switch to Tsubaki MK1", ImGuiKey_2, [](KPADEXStatus* ext){ 
        uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
        if (dPadInputsAddr) {
            int8_t* dPadInput = (int8_t*)dPadInputsAddr;
            *dPadInput = DPAD_LEFT;
        }
        });
    g_input_map.input_map("Switch to Tsubaki MK2",  ImGuiKey_3, [](KPADEXStatus* ext){ 
        uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
        if (dPadInputsAddr) {
            int8_t* dPadInput = (int8_t*)dPadInputsAddr;
            *dPadInput = DPAD_DOWN;
        }
    });
    g_input_map.input_map("Switch to Tsubaki MK3",  ImGuiKey_4, [](KPADEXStatus* ext){ 
        uintptr_t dPadInputsAddr = (g_framework->get_module().as<uintptr_t>() + 0x849D14);
        if (dPadInputsAddr) {
            int8_t* dPadInput = (int8_t*)dPadInputsAddr;
            *dPadInput = DPAD_RIGHT;
        }
    });

    g_input_map.input_map("Call Bike",    ImGuiKey_B, [](KPADEXStatus* ext) {
        ext->cl.hold |= KEY_SQUARE; 
        static uint32_t* ass3 = (uint32_t*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x849D14);//(uint32_t*)(0x00C49D14);
        *ass3 |= 0x200;
    });

    g_input_map.input_map("Combat Sprint", ImGuiKey_LeftCtrl, [](KPADEXStatus* ext) {
        ext->cl.hold |= KEY_L3; 
        int* l3_address = (int*)(g_framework->get_module().as<uintptr_t>() + (ptrdiff_t)0x849D16);
        *l3_address = 1;
    });

    g_input_map.input_map("Shmup Shoot Keyboard", ImGuiKey_Space, [](KPADEXStatus* ext) {
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) {
                ext->cl.hold |= KEY_CIRCLE;
            }
        }
    });
    g_input_map.input_map("Shmup Sword Keyboard", ImGuiKey_LeftShift, [](KPADEXStatus* ext) { 
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) {
                ext->cl.hold |= KEY_CROSS;
            }
        }
    });

    g_input_map.input_map("Shmup Shoot Mouse", ImGuiKey_MouseLeft, [](KPADEXStatus* ext) { 
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) {
                ext->cl.hold |= KEY_CIRCLE;
            }
        }
    });

    g_input_map.input_map("Shmup Sword Mouse", ImGuiKey_MouseRight, [](KPADEXStatus* ext) { 
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) {
                ext->cl.hold |= KEY_CROSS;
            }
        }
    });

    g_input_map.input_map("Shmup Bomb", ImGuiKey_Tab, [](KPADEXStatus* ext) { 
        if (auto hrtask = g_hrtask_ptr->hrtsk) {
            if (hrtask->state == 27) {
                ext->cl.hold |= KEY_SQUARE;
            }
        }
    });
    

    return Mod::on_initialize();
}

void KbmControls::on_frame() {

    if (!m_mod_enabled->value()) {
        return;
    }

    static bool& draw_ui = g_framework->m_draw_ui;

    if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
        static uintptr_t baseAddress = g_framework->get_module().as<uintptr_t>();
        static HrCamera* hrCamera    = reinterpret_cast<HrCamera*>(baseAddress + 0x82A4A0);
        auto mode                    = mHRPc->mInputMode;
        int camMode                  = hrCamera->MAIN.Mode;
        if (mode == ePcInputMenu) {
            m_capture_mouse = false;
        }
        if (mHRPc->mOperate && camMode != HRCAMERA_MODE_MOTION) {
            m_capture_mouse = true;
        }

        m_capture_mouse &= !draw_ui;
    }

    if (m_capture_mouse != m_capture_mouse_old) {
        PADSetHideMouseCursor_ee(m_capture_mouse, m_capture_mouse);
        toggle_mouse(m_wnd, m_capture_mouse);

        m_capture_mouse_old = m_capture_mouse;
        g_block_inputs      = !m_capture_mouse;
        // NOTE(): try to stop imgui from screwing with mouse cursor
        if(m_capture_mouse) {
            // if we capture set this config flag bit
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }
        else {
            // if we dont want to capture mouse let it screw with it i dont care
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        }
    }

    /*auto ppad = g_ppad;
    KPADEXStatus* exstatus = &ppad->WiiStatusBuffer[0][0].ex_status;
    ImGui::Begin("gamepad viewer", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    {
        ImGui::InputFloat2("lstick", (float*)&exstatus->cl.lstick);
        ImGui::InputFloat2("rstick", (float*)&exstatus->cl.rstick);

        ImGui::InputScalar("hold", ImGuiDataType_U32, &exstatus->cl.hold);
        ImGui::InputScalar("trig", ImGuiDataType_U32, &exstatus->cl.trig);
        ImGui::InputScalar("release", ImGuiDataType_U32, &exstatus->cl.release);

        ImGui::InputFloat("ltrigger", &exstatus->cl.ltrigger);
        ImGui::InputFloat("rtrigger", &exstatus->cl.rtrigger);

    }
    ImGui::End();*/
}

void KbmControls::on_draw_ui() {
    if (m_mod_enabled->draw("Enable Keyboard and Mouse Support")) {
        if (m_mod_enabled->value() && !m_hooks) {
            m_hooks = std::make_unique<RAIIHooks>();
            register_raw_input_mouse(m_wnd);
        }
        else {
            m_hooks.reset();
        }
    }
    m_block_lockon->draw("Disable Mouse Roll");

    m_base_mouse_sens->draw("Base mouse sensitivity");
    m_cams_mouse_sens->draw("Camera senisitivity");

    m_mouse_range->draw("QTE mouse range"); ImGui::SameLine();
    help_marker("Adjust if sword clashes or deathblows dont register well when waggling the mouse\nWe cannot account for all possible mouse configs/sensitivity/poll rates ;_;");

    if (ImGui::CollapsingHeader("Bindings")) {

        for (auto& keys : g_input_map.actions) {
            ImGuiKey value = (ImGuiKey)keys.first->value();
            keys.first->draw(keys.second.name); ImGui::SameLine();
            if (ImGui::IsNamedKeyOrMod(value) && value) {
                //auto col = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
                //ImGui::PushStyleColor(ImGuiCol_Text, col);
                ImGui::Text(": %s", ImGui::GetKeyName(value));
                //ImGui::PopStyleColor();
            }
            else {
                ImGui::Text(": UNBOUND");
            }
        }
    }

#if 0
    KPADStatus* pad_status = (KPADStatus*)0x00C76190;

    ImGui::InputInt("hold", (int*)&pad_status->hold);
    ImGui::InputInt("trig", (int*)&pad_status->trig);
    ImGui::InputInt("release", (int*)&pad_status->release);

    ImGui::InputFloat("acc_value", &pad_status->acc_value);
    ImGui::InputFloat("acc_speed", &pad_status->acc_speed);

    ImGui::Text("dev_type: %d", pad_status->dev_type);
    for (size_t i = 0; i < 34; i++) {
        if (ImGui::TreeNode((void*)(intptr_t)i, "button %d", i)) {
            auto& button = g_pad->Button[i];

            ImGui::Checkbox("Input", &button.Input);
            ImGui::Checkbox("RevEdgeInput", &button.RevEdgeInput);
            ImGui::Checkbox("EdgeInput", &button.EdgeInput);
            ImGui::Checkbox("EdgeRepeatInput", &button.EdgeRepeatInput);
            ImGui::InputScalar("RepeatPopCounter", ImGuiDataType_U32, &button.RepeatPopCounter);
            ImGui::InputScalar("RepeatCounter", ImGuiDataType_U32, &button.RepeatCounter);

            ImGui::TreePop();
        }
    }
#endif
}

// during load
void KbmControls::on_config_load(const utility::Config &cfg) {
    for (IModValue& option : mod_vars) {
        option.config_load(cfg);
    }
    for (auto& bind : g_input_map.actions) {
        bind.first->config_load(cfg);
    }
    if (m_mod_enabled->value()) {
        m_hooks = std::make_unique<RAIIHooks>();
    }
}

// during save
void KbmControls::on_config_save(utility::Config &cfg) {
    for (IModValue& option : mod_vars) {
        option.config_save(cfg);
    }
    for (auto& bind : g_input_map.actions) {
        bind.first->config_save(cfg);
    }
}

// will show up in debug window, dump ImGui widgets you want here
//void KbmControls::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
//void KbmControls::on_draw_ui() {}
