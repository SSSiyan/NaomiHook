#include "Mods.hpp"
#include "Config.hpp"
#include <spdlog/spdlog.h>
#include "utility/Hash.hpp"

//#inlcude "YourMod.hpp"
#include "mods/SimpleMod.hpp"
#include "mods/BeamColor.hpp"
#include "mods/HpColor.hpp"
#include "mods/DisableCrashDumps.hpp"
#include "mods/AcceptPadInputsTabbedOut.hpp"
#include "mods/ForceFOV.hpp"

#define ADD_MOD(name)                                  \
    do {                                               \
        m_mods.emplace_back(std::make_unique<name>()); \
        m_hash.emplace_back(#name##_hash);             \
    } while (0)

static Mods* g_mods {nullptr};

Mods::Mods()
{
    g_mods = this;

    ADD_MOD(SimpleMod);
    ADD_MOD(BeamColor);
    ADD_MOD(HpColor);
    ADD_MOD(DisableCrashDumps);
    ADD_MOD(AcceptPadInputsTabbedOut);
    ADD_MOD(ForceFOV);
    //m_mods.emplace_back(std::make_unique<YourMod>());

#ifdef DEVELOPER
    m_mods.emplace_back(std::make_unique<DeveloperTools>());
#endif
}

std::optional<std::string> Mods::on_initialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_initialize()", mod->get_mod_name().data());

        if (auto e = mod->on_initialize(); e != std::nullopt) {
            spdlog::info("{:s}::on_initialize() has failed: {:s}", mod->get_mod_name().data(), *e);
            return e;
        }
    }

    utility::Config cfg{ CONFIG_FILENAME };

    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_config_load()", mod->get_mod_name().data());
        mod->on_config_load(cfg);
    }

    return std::nullopt;
}

void Mods::on_frame() const {
    for (auto& mod : m_mods) {
        mod->on_frame();
    }
}

void Mods::on_draw_debug_ui() const {
	for (auto& mod : m_mods) {
		mod->on_draw_debug_ui();
	}
}

void Mods::on_config_save() {

    spdlog::info("Saving config to file");

    utility::Config cfg{};

    for (auto& mod : m_mods) {
        spdlog::info("%s::onConfigSave()\n", mod->get_mod_name().data());
        mod->on_config_save(cfg);

#if 0
        auto& hks = mod->m_hotkeys;
        if (hks.empty()) { continue; }
        for (const auto& key : hks) {
            key->on_config_save(cfg);
        }
#endif
}

#if 0
    ModFramework* framework = g_framework.get();
    framework->get_menu_key_struct()->on_config_save(cfg);
    framework->get_menu_button_struct()->on_config_save(cfg);
#endif

    if (!cfg.save(CONFIG_FILENAME)) {
        spdlog::info("Failed to save config");
        return;
    }

    spdlog::info("Saved config");
}


#if 0

void Mods::on_draw_ui() const {
    for (auto& mod : m_mods) {
        mod->on_draw_ui();
    }
}
#else

void Mods::on_draw_ui(uint32_t hash) {
    auto itr = std::find(m_hash.begin(), m_hash.end(), hash);
    if (itr != m_hash.cend()) {
        m_mods[std::distance(m_hash.begin(), itr)]->on_draw_ui();
    }
    else {
        spdlog::error("Mods::onDrawUI not found in hashindex!");
        __debugbreak();
    }
}
#endif

