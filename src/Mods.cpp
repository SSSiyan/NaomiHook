#include "Mods.hpp"
#include "Config.hpp"
#include <spdlog/spdlog.h>
#include "utility/Hash.hpp"

//#inlcude "YourMod.hpp"
// #include "mods/SimpleMod.hpp"
// #include "mods/BeamColor.hpp"
// #include "mods/HpColor.hpp"
#include "mods/PlayerTracker.hpp"
#include "mods/EnemyTracker.hpp"
#include "mods/DisableCrashDumps.hpp"
#include "mods/AcceptPadInputsTabbedOut.hpp"
#include "mods/WeaponSwitcher.hpp"
#include "mods/ForceFOV.hpp"
#include "mods/StageWarp.hpp"
#include "mods/ClothesSwitcher.hpp"
#include "mods/LockOnSettings.hpp"
#include "mods/HitstopSettings.hpp"
#include "mods/ScreenshakeSettings.hpp"
#include "mods/RollForward.hpp"
#include "mods/SprintSettings.hpp"
#include "mods/SwordColours.hpp"
#include "mods/ThrowBattery.hpp"
#include "mods/DodgeBattery.hpp"
#include "mods/HPHitNumber.hpp"
#include "mods/ChargeSubsBattery.hpp"
#include "mods/LockOnDisablesThrows.hpp"
#include "mods/DebugText.hpp"
#include "mods/Invincibility.hpp"
#include "mods/RollRotation.hpp"
#include "mods/SwapIdleStances.hpp"
#include "mods/ReprisalSwap.hpp"
#include "mods/DisableMouse.hpp"
#include "mods/SoundTracker.hpp"
#include "mods/QuickBoot.hpp"
#include "mods/ArcadeMode.hpp"
#include "mods/StanceControl.hpp"

#define ADD_MOD(name)                                  \
    do {                                               \
        m_mods.emplace_back(std::make_unique<name>()); \
        m_hash.emplace_back(#name##_hash);             \
    } while (0)

static Mods* g_mods {nullptr};

Mods::Mods()
{
    g_mods = this;

    // ADD_MOD(SimpleMod);
    // ADD_MOD(BeamColor);
    // ADD_MOD(HpColor);
    ADD_MOD(PlayerTracker); // keep first
    ADD_MOD(EnemyTracker); // keep second
    ADD_MOD(DisableCrashDumps);
    ADD_MOD(AcceptPadInputsTabbedOut);
    ADD_MOD(WeaponSwitcher);
    ADD_MOD(ForceFOV);
    ADD_MOD(StageWarp);
    ADD_MOD(ClothesSwitcher);
    ADD_MOD(LockOnSettings);
    ADD_MOD(LockOnDisablesThrows);
    ADD_MOD(HitstopSettings);
    ADD_MOD(ScreenshakeSettings);
    ADD_MOD(RollForward);
    ADD_MOD(SprintSettings);
    ADD_MOD(SwordColours);
    ADD_MOD(ThrowBattery);
    ADD_MOD(DodgeBattery);
    ADD_MOD(HPHitNumber);
    ADD_MOD(ChargeSubsBattery);
    ADD_MOD(DebugText);
    ADD_MOD(Invincibility);
    ADD_MOD(RollRotation);
    ADD_MOD(SwapIdleStances);
    ADD_MOD(ReprisalSwap);
    ADD_MOD(DisableMouse);
    ADD_MOD(SoundTracker);
    ADD_MOD(QuickBoot);
    ADD_MOD(ArcadeMode);
    ADD_MOD(StanceControl);
    //m_mods.emplace_back(std::make_unique<YourMod>());

#ifdef DEVELOPER
    m_mods.emplace_back(std::make_unique<DeveloperTools>());
#endif
}

std::optional<std::string> Mods::on_initialize() const {
    for (auto& mod : m_mods) {
        spdlog::info("{:s}::on_initialize()", mod->get_mod_name().data());

        if (auto e = mod->on_initialize(); e != std::nullopt) {
            spdlog::error("{:s}::on_initialize() has failed: {:s}", mod->get_mod_name().data(), *e);
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

void Mods::on_draw_custom_imgui_window() {
    PlayerTracker* pt = dynamic_cast<PlayerTracker*>(m_mods[0].get());
    pt->custom_imgui_window();
    EnemyTracker* et = dynamic_cast<EnemyTracker*>(m_mods[1].get());
    et->custom_imgui_window();
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

void Mods::on_d3d11_reset()
{
    for (auto& mod : m_mods) {
        mod->on_d3d_reset();
    }
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

