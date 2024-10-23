#pragma once

#include "Mod.hpp"

class Mods {
public:
    Mods();
    virtual ~Mods() {}

    std::optional<std::string> on_initialize() const;

    void on_frame() const;
    void on_draw_ui(uint32_t hash);
    void on_draw_custom_imgui_window();
    void on_draw_debug_ui() const;
    void on_config_save();

    std::vector<std::unique_ptr<Mod>> m_mods;

    std::vector<uint32_t> m_hash;
};