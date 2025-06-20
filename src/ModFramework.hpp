#pragma once

#include <spdlog/spdlog.h>

class Mods;
struct OurImGuiContext;
struct ID3D11Device;

struct MouseRaw {
    int x = 0;
    int y = 0;
    int wheel = 0;
};

#include "D3D11Hook.hpp"
#include "WindowsMessageHook.hpp"
#include "utility/Address.hpp"
#include <array>

// Global facilitator
class ModFramework {
public:
    ModFramework();
    virtual ~ModFramework();

    bool is_valid() const {
        return m_valid;
    }

    const auto& get_mods() const {
        return m_mods;
    }

    const auto& get_keyboard_state() const {
        return m_last_keys;
    }

    Address get_module() const {
        return GetModuleHandle(NULL);
    }

    bool is_ready() const {
        return m_game_data_initialized;
    }

    D3D11Hook* d3d11() const {
        return m_d3d11_hook.get();
    }

    void on_frame();
    void on_reset();
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);

    void save_config();

    void reset_mouse();
public:
    MouseRaw mouser;
    bool m_capture_mouse { false };
    bool m_capture_mouse_old { false };
    
    float window_size_x{};
    float window_size_y{};

private:
    void draw_ui();
    void draw_about();

    bool initialize();
    void create_render_target();
    void cleanup_render_target();

    bool m_first_frame{ true };
    bool m_valid{ false };
    bool m_initialized{ false };
    bool m_draw_ui{ false };
    std::atomic<bool> m_game_data_initialized{ false };

    HWND m_wnd{ 0 };
    HMODULE m_game_module{ 0 };
    uint8_t m_menu_key{ VK_DELETE };

    std::array<uint8_t, 256> m_last_keys{ 0 };
    std::unique_ptr<D3D11Hook> m_d3d11_hook{};
    std::unique_ptr<WindowsMessageHook> m_windows_message_hook;
    std::unique_ptr<OurImGuiContext> m_our_imgui_ctx;
    std::shared_ptr<spdlog::logger> m_logger;

    std::string m_error{ "" };

    // Game-specific stuff
    std::unique_ptr<Mods> m_mods;

    ID3D11RenderTargetView* m_main_render_target_view{ nullptr };
};

extern std::unique_ptr<ModFramework> g_framework;