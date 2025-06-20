#ifndef NDEBUG
#include <spdlog/sinks/stdout_color_sinks.h>
#else
#include <spdlog/sinks/basic_file_sink.h>
#endif

#include <imgui/imgui.h>

// ours with XInput removed
#include "fw-imgui/imgui_impl_win32.h"
#include "fw-imgui/imgui_impl_dx11.h"

#include "utility/Module.hpp"

#include "Mods.hpp"

#include "LicenseStrings.hpp"
#include "ModFramework.hpp"

#include "Config.hpp"
#include "GuiFunctions.hpp"
#include "Fonts.hpp"
#include "mods/DisableMouse.hpp"
#include "ImageViewer.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unique_ptr<ModFramework> g_framework{};

UINT g_bb_width{};
UINT g_bb_height{};
//static glm::vec2 g_window_dims{};
typedef void(__fastcall *PADSetHideMouseCursor_ptr)(bool s, bool lock);
static PADSetHideMouseCursor_ptr PADSetHideMouseCursor_ee { nullptr };

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

inline void mouse_capture(HWND window) {
    RECT window_rect;

    GetClientRect(window, &window_rect);
    ClientToScreen(window, (POINT*)&window_rect.left);
    ClientToScreen(window, (POINT*)&window_rect.right);
    ClipCursor(&window_rect);
    mouse_set_visible(FALSE);
}

inline void mouse_release() {
    ClipCursor(NULL);
    mouse_set_visible(TRUE);
}

static void toggle_mouse(HWND window, bool capturing) {
    if (capturing) {
        mouse_capture(window);
    }
    else {
        mouse_release();
    }
}
#endif

ModFramework::ModFramework()
    : m_game_module{ GetModuleHandle(0) },
#ifndef NDEBUG
    m_logger{ spdlog::stdout_color_mt("ModFramework") }
#else
    m_logger{ spdlog::basic_logger_mt("ModFramework", LOG_FILENAME, true) }
#endif // !NDEBUG
{
    spdlog::set_default_logger(m_logger);
    spdlog::flush_on(spdlog::level::info);
    spdlog::info(LOG_ENTRY);

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    m_d3d11_hook = std::make_unique<D3D11Hook>();
    m_d3d11_hook->on_present([this](D3D11Hook& hook) { on_frame(); });
    m_d3d11_hook->on_resize_buffers([this](D3D11Hook& hook) { on_reset(); });
    m_d3d11_hook->after_resize_buffers([this](D3D11Hook& hook) { after_reset(); });

    m_valid = m_d3d11_hook->hook();

    m_our_imgui_ctx = std::make_unique<OurImGuiContext>();

    if (m_valid) {
        spdlog::info("Hooked D3D11");
    }
    uintptr_t offset = (uintptr_t)((uintptr_t)m_game_module + (ptrdiff_t)0x1BECC0);
    PADSetHideMouseCursor_ee = (PADSetHideMouseCursor_ptr)offset;
}

ModFramework::~ModFramework() {

}

void ModFramework::on_frame() {

    if (!m_initialized) {
        if (!initialize()) {
            spdlog::error("Failed to initialize ModFramework");
            return;
        }

        spdlog::info("ModFramework initialized");
        m_initialized = true;
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_error.empty() && m_game_data_initialized) {
        m_mods->on_frame();
        m_mods->on_draw_custom_imgui_window();
    }

    draw_ui();

    ImGui::EndFrame();
    ImGui::Render();

    ID3D11DeviceContext* context = nullptr;
    m_d3d11_hook->get_device()->GetImmediateContext(&context);

    context->OMSetRenderTargets(1, &m_main_render_target_view, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // toggle mouse capture
    if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt)) {
        m_capture_mouse = !m_capture_mouse;
    }

    if (m_capture_mouse != m_capture_mouse_old) {
        PADSetHideMouseCursor_ee(m_capture_mouse, m_capture_mouse);

        m_capture_mouse_old = m_capture_mouse;
    }
}

void ModFramework::on_reset() {
    spdlog::info("Reset!");
    if(!m_initialized) { return; }

    cleanup_render_target();
#if 1
    //ImGui_ImplDX11_InvalidateDeviceObjects();

    // Crashes if we don't release it at this point.
    //ImGui_ImplDX11_CreateDeviceObjects();
#endif
    ImGui_ImplDX11_InvalidateDeviceObjects();

    m_mods->on_d3d11_reset();
}

void ModFramework::after_reset() {
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    auto swap_chain = m_d3d11_hook->get_swap_chain();
    if (!swap_chain) {
        return;
    }
    swap_chain->GetDesc(&swapChainDesc);
    g_bb_width  = swapChainDesc.BufferDesc.Width;
    g_bb_height = swapChainDesc.BufferDesc.Height;

    create_render_target();
    ImGui_ImplDX11_CreateDeviceObjects();
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)g_bb_width, (float)g_bb_height);
}

bool ModFramework::on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    if (!m_initialized) {
        return true;
    }

#if 0
    if (message == WM_SIZE) {
        g_window_dims.x = (float)LOWORD(l_param);
        g_window_dims.y = (float)HIWORD(l_param);
        spdlog::info("WM_SIZE: {}, {}", g_window_dims.x, g_window_dims.y);
    }
#endif

    // TODO(): hotkey crap from dmc4hook?
    if (message == WM_KEYDOWN && w_param == VK_DELETE) {
        m_draw_ui = !m_draw_ui;
        DisableMouse::gui_open = m_draw_ui;
        m_capture_mouse = !m_draw_ui;
        //mouse_set_visible(m_draw_ui);
    }


    if (ImGui_ImplWin32_WndProcHandler(wnd, message, w_param, l_param) != 0) { // if (m_draw_ui && didn't work and stops us interacting with debug windows
        // If the user is interacting with the UI we block the message from going to the game.
        auto& io = ImGui::GetIO();

        if (io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput) {
            return false;
        }
    }

    if (message == WM_INPUT) {
        size_t size = sizeof(RAWINPUT);
        static RAWINPUT raw[sizeof(RAWINPUT)];
        GetRawInputData((HRAWINPUT)l_param, RID_INPUT, raw, &size, sizeof(RAWINPUTHEADER));

        if (raw->header.dwType == RIM_TYPEMOUSE) {
            mouser.x = raw->data.mouse.lLastX;
            mouser.y = raw->data.mouse.lLastY;

            if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
                mouser.wheel = (*(short*)&raw->data.mouse.usButtonData) / WHEEL_DELTA;
        }
    }

    return true;
}

void ModFramework::save_config() {
    spdlog::info("Saving config to file");

    utility::Config cfg{};

    for (auto& mod : m_mods->m_mods) {
        mod->on_config_save(cfg);
    }

    if (!cfg.save(CONFIG_FILENAME)) {
        spdlog::info("Failed to save config");
        return;
    }

    spdlog::info("Saved config");
}

void ModFramework::reset_mouse() {
    memset(&mouser, 0, sizeof(MouseRaw));
}

void ModFramework::draw_ui() {
#ifndef _NDEBUG
    auto& io = ImGui::GetIO();
    //ImGui::Text("Resolution: %f, %f; backbuffer: %d, %d", io.DisplaySize.x, io.DisplaySize.y, g_bb_width, g_bb_height);
#endif // !_NDEBUG

    if (!m_draw_ui) {
        return;
    }
    m_our_imgui_ctx->wflags = m_draw_ui;

#if 0
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_::ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_::ImGuiCond_Once);

    ImGui::Begin(PROJECT_NAME, &m_draw_ui);

    ImGui::Text("Menu Key: Delete");

    draw_about();
#endif
    if (m_error.empty() && m_game_data_initialized) {
        gui::imgui_main_window_proc  (m_our_imgui_ctx.get(), m_mods.get(), &m_draw_ui);
        gui::imgui_left_window_proc  (m_our_imgui_ctx.get(), m_mods.get());
        gui::imgui_bottom_window_proc(m_our_imgui_ctx.get(), m_mods.get());
    }
    else if (!m_game_data_initialized) {
        //ImGui::TextWrapped("ModFramework is currently initializing...");
    }
    else if(!m_error.empty()) {
        //ImGui::TextWrapped("ModFramework error: %s", m_error.c_str());
    }
#if 0
    imview_draw();
#endif

#if 0
    ImGui::End();
#endif
}

void ModFramework::draw_about() {
    if (!ImGui::CollapsingHeader("About")) {
        return;
    }

    ImGui::TreePush("About");

    ImGui::Text("Inspired by the REFramework.");
    ImGui::Text("https://github.com/praydog/RE2-Mod-Framework");

    if (ImGui::CollapsingHeader("Licenses")) {
        ImGui::TreePush("Licenses");

        if (ImGui::CollapsingHeader("glm")) {
            ImGui::TextWrapped(license::glm);
        }

        if (ImGui::CollapsingHeader("imgui")) {
            ImGui::TextWrapped(license::imgui);
        }

        if (ImGui::CollapsingHeader("minhook")) {
            ImGui::TextWrapped(license::minhook);
        }

        if (ImGui::CollapsingHeader("spdlog")) {
            ImGui::TextWrapped(license::spdlog);
        }

        ImGui::TreePop();
    }

    ImGui::TreePop();
}

bool ModFramework::initialize() {
    if (m_initialized) {
        return true;
    }

    spdlog::info("Attempting to initialize");

    auto device = m_d3d11_hook->get_device();
    auto swap_chain = m_d3d11_hook->get_swap_chain();

    // Wait.
    if (device == nullptr || swap_chain == nullptr) {
        spdlog::info("Device or SwapChain null. DirectX 12 may be in use. A crash may occur.");
        return false;
    }

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);

    DXGI_SWAP_CHAIN_DESC swap_desc{};
    swap_chain->GetDesc(&swap_desc);

    m_wnd = swap_desc.OutputWindow;

    RECT window_rect{};
    if (!GetWindowRect(m_wnd, &window_rect)) {
        spdlog::error("Failed to GetWindowRect({}, {});\n", (void*)m_wnd, (void*)&window_rect);
    }
    window_size_x = window_rect.right - window_rect.left;
    window_size_y = window_rect.bottom - window_rect.top;

    // Explicitly call destructor first
    m_windows_message_hook.reset();
    m_windows_message_hook = std::make_unique<WindowsMessageHook>(m_wnd);
    m_windows_message_hook->on_message = [this](auto wnd, auto msg, auto wParam, auto lParam) {
        return on_message(wnd, msg, wParam, lParam);
    };

    // Registering raw input devices
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short) 0x02)
#endif

    // We're configuring just one RAWINPUTDEVICE, the mouse,
    // so it's a single-element array (a pointer).
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = m_wnd;
    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
    // End of resgistering.

    spdlog::info("Creating render target");

    create_render_target();

    spdlog::info("Window Handle: {0:x}", (uintptr_t)m_wnd);
    spdlog::info("Initializing ImGui");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    spdlog::info("Initializing ImGui Win32");

    if (!ImGui_ImplWin32_Init(m_wnd)) {
        spdlog::error("Failed to initialize ImGui.");
        return false;
    }

    spdlog::info("Initializing ImGui D3D11");

    if (!ImGui_ImplDX11_Init(device, context)) {
        spdlog::error("Failed to initialize ImGui.");
        return false;
    }

    UINT dpi = GetDpiForWindow(m_wnd);

    gui::dark_theme(dpi);

    auto& io = ImGui::GetIO();

    load_fonts(m_our_imgui_ctx.get(), io, dpi);

#if 0
    imview_init();
#endif


    if (m_first_frame) {
        m_first_frame = false;

        spdlog::info("Starting game data initialization thread");

        // Game specific initialization stuff
        std::thread init_thread([this]() {
            m_mods = std::make_unique<Mods>();

            auto e = m_mods->on_initialize();

            if (e) {
                if (e->empty()) {
                    m_error = "An unknown error has occurred.";
                }
                else {
                    m_error = *e;
                }
            }

            m_game_data_initialized = true;
        });

        init_thread.detach();
    }


    return true;
}

void ModFramework::create_render_target() {

    ID3D11Texture2D* back_buffer{ nullptr };
    if (m_d3d11_hook->get_swap_chain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer) == S_OK) {
        m_d3d11_hook->get_device()->CreateRenderTargetView(back_buffer, NULL, &m_main_render_target_view);
        back_buffer->Release();
    }
}

void ModFramework::cleanup_render_target() {
    if (m_main_render_target_view != nullptr) {
        m_main_render_target_view->Release();
        m_main_render_target_view = nullptr;
    }
}

