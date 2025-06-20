#include "KbmControls.hpp"
#include "utility/Patch.hpp"
#include <map>

static KbmControls* g_kbm { nullptr };

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

struct Vec2 {
    float x,y;
};

struct KPADEXStatus
{
  struct
  {
    /* 0x0000 */ struct Vec2 stick;
    /* 0x0008 */ struct Vec acc;
    /* 0x0014 */ float acc_value;
    /* 0x0018 */ float acc_speed;
  } /* size: 0x001c */ fs;
  struct
  {
    /* 0x001c */ unsigned int hold;
    /* 0x0020 */ unsigned int trig;
    /* 0x0024 */ unsigned int release;
    /* 0x0028 */ struct Vec2 lstick;
    /* 0x0030 */ struct Vec2 rstick;
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
    std::map<ModKey::Ptr, Action> actions;

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
        actions.emplace(ModKey::create(cfg_name(name), default_value), Action{.name = name, .act = act});
    }
    void input_map(const char* name, ImGuiKey default_value, uint32_t code) {
        actions.emplace(ModKey::create(cfg_name(name), default_value), Action{.name = name, .code = code});
    }
};

InputMap g_input_map {};


static float linear_map(float edge0, float edge1, float x) {
    // Scale, and clamp x to 0..1 range
    x = ((x - edge0) / (edge1 - edge0));

    return glm::clamp(x, 0.0f, 1.0f); // Simply return x instead of applying the cubic polynomial
}

static std::unique_ptr<FunctionHook> g_hook_ghm_pad_pr_wii_pad_sampling_callback;

static glm::vec2 g_mouse_delta {};
static PAD_UNI* g_ppad {nullptr};

void __cdecl ghm_pad_prWiiPadSamplingCallback_(void* status, void* context) {

    uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
    g_hook_ghm_pad_pr_wii_pad_sampling_callback->get_original<decltype(ghm_pad_prWiiPadSamplingCallback_)>()(status, context);

    auto ppad = g_ppad;
    KPADEXStatus* exstatus = &ppad->WiiStatusBuffer[0][0].ex_status;

#if 0
    if(ImGui::IsKeyDown(ImGuiKey_W)) {
        exstatus->cl.lstick.y = 1.0f;
    }
    if(ImGui::IsKeyDown(ImGuiKey_S)) {
        exstatus->cl.lstick.y = -1.0f;
    }
    if(ImGui::IsKeyDown(ImGuiKey_A)) {
        exstatus->cl.lstick.x = -1.0f;
    }
    if(ImGui::IsKeyDown(ImGuiKey_D)) {
        exstatus->cl.lstick.x = 1.0f;
    }
#endif
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

#if 0
    ImVec2 mouse = ImGui::GetIO().MouseDelta;
#else
    auto mouser = g_framework->mouser;
    glm::vec2 mouse { mouser.x, mouser.y };
    mouse *= g_kbm->m_base_mouse_sens->value();
#endif
    g_mouse_delta.x = (float)mouse.x;
    g_mouse_delta.y = (float)mouse.y;

    const float range = g_kbm->m_mouse_range->value();
    exstatus->cl.rstick.x = (linear_map(-range, range, mouse.x) - 0.5f) * 2.0f;
    exstatus->cl.rstick.y = (linear_map(-range, range, mouse.y) - 0.5f) * 2.0f;

#if 0
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        exstatus->cl.hold |= 32;
    }
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        exstatus->cl.hold |= 8;
    }
#endif
}



static void MOVE2_SetCameraYAngleRate(float a1) noexcept {

    HrCamera* camera = nmh_sdk::get_HrCamera();
    mHRPc* pc = nmh_sdk::get_mHRPc();
    assert(camera != NULL);
    assert(pc != NULL);

    if (camera->m.Mode != HRCAMERA_MODE::HRCAMERA_MODE_MOVE2) {
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
    float angle = - g_mouse_delta.x * sensitivity * 0.04f;
    float angle_y = g_mouse_delta.y * sensitivity * 0.05f;

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


    camera->m.mov2.CamAngle      += angle;//glm::clamp(camera->m.mov2.CamAngle + angle, -glm::pi<float>(), glm::pi<float>());
    camera->m.mov2.CamYAngleRate = glm::clamp(camera->m.mov2.CamYAngleRate + angle_y, -0.09f, glm::pi<float>() * 0.6f);
    g_framework->reset_mouse();
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

static Patch::Ptr g_hr_cam_angle_horizontal_patch {};
static std::unique_ptr<FunctionHook> m_stage{};

void KbmControls::custom_imgui_window()
{
    auto ppad = g_ppad;
    KPADEXStatus* exstatus = &ppad->WiiStatusBuffer[0][0].ex_status;
    ImGui::Begin("gamepad viewer");
    {

        ImGui::InputFloat2("lstick", (float*)&exstatus->cl.lstick);
        ImGui::InputFloat2("rstick", (float*)&exstatus->cl.rstick);

        ImGui::InputScalar("hold", ImGuiDataType_U32, &exstatus->cl.hold);
        ImGui::InputScalar("trig", ImGuiDataType_U32, &exstatus->cl.trig);
        ImGui::InputScalar("release", ImGuiDataType_U32, &exstatus->cl.release);

        ImGui::InputFloat("ltrigger", &exstatus->cl.ltrigger);
        ImGui::InputFloat("rtrigger", &exstatus->cl.rtrigger);

    }
    ImGui::End();

}

std::optional<std::string> KbmControls::on_initialize() {
#if 0
    g_framework->m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
#endif
    g_kbm = this;
    uintptr_t base = g_framework->get_module().as<uintptr_t>();
    auto ptr = (base+0x849D10);
    g_ppad = (PAD_UNI*)ptr;

    static constexpr ptrdiff_t pad_sampling_cb_offset = 0x5F7CD0; 

    g_hook_ghm_pad_pr_wii_pad_sampling_callback = std::make_unique<FunctionHook>(base + pad_sampling_cb_offset, &ghm_pad_prWiiPadSamplingCallback_); // TODO: offset
    if(!g_hook_ghm_pad_pr_wii_pad_sampling_callback->create()) {
        return "Failed to hook ghm_pad_pr_wii_pad_sampling_callback()";
    }

    static constexpr ptrdiff_t hr_cam_angle_hor_offset = 0x3B4D34;
    g_hr_cam_angle_horizontal_patch = Patch::create_nop(base + hr_cam_angle_hor_offset, 8);

#if 0
    m_camera_rot_lr = std::make_unique<FunctionHook>(0x007B47C0, &detour_camera_rot_lr);
    if (!m_camera_rot_lr->create()) {
        return "Failed to hook camera rotate lr";
    }

    m_camera_rot_ud = std::make_unique<FunctionHook>(0x007B4750, &detour_camera_rot_ud);
    if (!m_camera_rot_ud->create()) {
        return "Failed to hook camera rotate ud";
    }
#endif
    static constexpr ptrdiff_t set_camera_y_angle_offset = 0x3EA110;
    m_camera_rot_lr = std::make_unique<FunctionHook>(base + set_camera_y_angle_offset, &detour_MOVE2_SetCameraYAngleRate);
    if (!m_camera_rot_lr->create()) {
        return "Failed to hook camera rotate lr";
    }

    g_input_map.input_map("Forward", ImGuiKey_W, 
        [](KPADEXStatus* ext) { ext->cl.lstick.y = 1.0f;  ext->cl.hold = KEY_DPAD_UP; } );
    g_input_map.input_map("Back", ImGuiKey_S, 
        [](KPADEXStatus* ext) { ext->cl.lstick.y = -1.0f; ext->cl.hold = KEY_DPAD_DOWN; } );
    g_input_map.input_map("Left", ImGuiKey_A, 
        [](KPADEXStatus* ext) { ext->cl.lstick.x = -1.0f; ext->cl.hold = KEY_DPAD_LEFT; } );
    g_input_map.input_map("Right", ImGuiKey_D, 
        [](KPADEXStatus* ext) { ext->cl.lstick.x = 1.0f;  ext->cl.hold = KEY_DPAD_RIGHT; } );

    g_input_map.input_map("Accept", ImGuiKey_Enter,     KEY_START);
    g_input_map.input_map("Cancel", ImGuiKey_Backspace, KEY_SELECT);

    g_input_map.input_map("Skip Scene", ImGuiKey_Backspace, KEY_SELECT);
    g_input_map.input_map("Open Menu",  ImGuiKey_Tab,       KEY_START);

    g_input_map.input_map("Target Lock On", ImGuiKey_LeftShift, [](KPADEXStatus* ext) { ext->cl.hold = KEY_LT; ext->cl.ltrigger = 128.1f; } );

    g_input_map.input_map("Low Slash Mouse", ImGuiKey_MouseLeft,   [](KPADEXStatus* ext) { ext->cl.hold = KEY_SQUARE; });
    g_input_map.input_map("High Slash Mouse", ImGuiKey_MouseRight, [](KPADEXStatus* ext) { ext->cl.hold = KEY_TRIANGLE; });

    g_input_map.input_map("Low Slash Keyboard", ImGuiKey_E, [](KPADEXStatus* ext) { ext->cl.hold = KEY_SQUARE; });
    g_input_map.input_map("High Slash Mouse",   ImGuiKey_Q, [](KPADEXStatus* ext) { ext->cl.hold = KEY_TRIANGLE; });

    g_input_map.input_map("High Melee Attack", ImGuiKey_V, [](KPADEXStatus* ext) { ext->cl.hold = KEY_CIRCLE; });
    g_input_map.input_map("Low Melee Attack",  ImGuiKey_C, [](KPADEXStatus* ext) { ext->cl.hold = KEY_CROSS; });

    g_input_map.input_map("Emergency Evade", ImGuiKey_Space, [](KPADEXStatus* ext) { 
        if (ext->cl.hold & KEY_LT) { // is lockon down?
            ext->cl.rstick = ext->cl.lstick;
        }
    });

    g_input_map.input_map("Battery Charge Start", ImGuiKey_LeftCtrl, KEY_LB);
    g_input_map.input_map("Camera Reset", ImGuiKey_LeftAlt, KEY_RB);


    return Mod::on_initialize();
}

void KbmControls::on_frame() {
}

void KbmControls::on_draw_ui() {
    m_mod_enabled->draw("Enable Keyboard and Mouse Support?");

    m_base_mouse_sens->draw("Base mouse sensitivity");
    m_cams_mouse_sens->draw("Camera mouse senisitivity");

    m_mouse_range->draw("QTE mouse range");

    if (ImGui::CollapsingHeader("Bindings")) {

        for (auto& keys : g_input_map.actions) {
            keys.first->draw(keys.second.name);
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

// do something every frame
//void KbmControls::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void KbmControls::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
//void KbmControls::on_draw_ui() {}
