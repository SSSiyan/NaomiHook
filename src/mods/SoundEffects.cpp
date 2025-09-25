#include "SoundEffects.hpp"
#if 1
#include <random>

bool SoundEffects::disable_death_screams = false;
bool SoundEffects::custom_death_screams  = false;
uintptr_t SoundEffects::jmp_ret1         = NULL;
uintptr_t SoundEffects::ghm_snd_prPrep   = NULL;
int SoundEffects::temp_sound_id          = 0;

// -------- Runtime-only state (kept simple; no new Config API usage) ---------
static int s_custom_scream_id       = 0x680; // editable at runtime
static bool s_randomize_custom_pool = false; // checkbox to use custom pool
static bool s_rng_seeded            = false; // PRNG seed guard
static std::mt19937 s_rng;                   // PRNG

// Customizable random-pick pool (UI-editable, persisted as individual ints)
static int s_pool_count = 7; // how many entries in use
static int s_pool[16]   = {  // hard cap to avoid vectors/serialization
    744, 780, 799, 800, 805, 806, 1691, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void SoundEffects::ToggleDeathScreamExtras(bool enable) {
    if (enable) {
        install_patch_offset(0x42DF28, m_patch1, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghmGcSound::PrepPlay
        install_patch_offset(0x42DF45, m_patch2, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghm_snd_prPrep
        install_patch_offset(0x42DFCA, m_patch3, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghmGcSound::PrepPlay
    } else {
        m_patch1.reset();
        m_patch2.reset();
        m_patch3.reset();
    }
}

static inline int pick_from_pool() {
    if (!s_rng_seeded) {
        std::random_device rd;
        s_rng.seed(rd());
        s_rng_seeded = true;
    }
    int n = (s_pool_count <= 0) ? 0 : s_pool_count;
    if (n <= 0) {
        return s_custom_scream_id; // fallback
    }
    std::uniform_int_distribution<int> dist(0, n - 1);
    int idx = dist(s_rng);
    return s_pool[idx];
}

void SoundEffects::GetCustomScreamSound(int /*soundID*/) {
    // If the random-pool checkbox is on, pick from list; otherwise use the typed-in ID
    if (s_randomize_custom_pool) {
        temp_sound_id = pick_from_pool();
    } else {
        temp_sound_id = s_custom_scream_id;
    }
}

// clang-format off
naked void detour1() { // in nmh.ghmGcSoundTiming::Process
    __asm {
        //
            cmp byte ptr [SoundEffects::disable_death_screams], 1
            je noScreams
            cmp byte ptr [SoundEffects::custom_death_screams], 1
            je customScreams
            jmp originalcode

        noScreams:
            cmp edx, 0x682 // ahh
            je retcode
            cmp edx, 0x6a7 // less annoying ahh
            je retcode
            cmp edx, 0x6c5 // ahh
            je retcode
            jmp originalcode

        customScreams:
            cmp edx, 0x682 // ahh
            je newSound
            cmp edx, 0x6a7 // less annoying ahh
            je retcode
            cmp edx, 0x6c5 // ahh
            je retcode
            jmp originalcode

        newSound:
            pushad
            push edx
            call dword ptr [SoundEffects::GetCustomScreamSound]
            popad
            mov edx, [SoundEffects::temp_sound_id]

        originalcode:
            call dword ptr [SoundEffects::ghm_snd_prPrep]
        retcode:
            jmp dword ptr [SoundEffects::jmp_ret1]
    }
}
// clang-format on

std::optional<std::string> SoundEffects::on_initialize() {
    SoundEffects::ghm_snd_prPrep = g_framework->get_module().as<uintptr_t>() + 0x5F1B90;
    if (!install_hook_offset(0x6013C5, m_hook1, &detour1, &SoundEffects::jmp_ret1, 5)) {
        spdlog::error("Failed to init SoundEffects mod\n");
        return "Failed to init SoundEffects mod";
    }

    return Mod::on_initialize();
}

void SoundEffects::on_draw_ui() {
    if (ImGui::Checkbox("Disable Death Screams", &disable_death_screams)) {
        custom_death_screams = false;
        ToggleDeathScreamExtras(disable_death_screams);
    }
    if (ImGui::Checkbox("Custom Death Screams", &custom_death_screams)) {
        disable_death_screams = false;
        ToggleDeathScreamExtras(custom_death_screams);
    }

    ImGui::Separator();
    ImGui::Text("Custom scream controls");

    ImGui::Checkbox("Random from custom list", &s_randomize_custom_pool);

    ImGui::BeginDisabled(s_randomize_custom_pool);
    ImGui::InputInt("Custom Scream ID", &s_custom_scream_id);
    ImGui::EndDisabled();

    // Pool editor (kept minimal and ASCII-safe; no tables needed)
    if (ImGui::TreeNodeEx("Custom List (used when random is on)", ImGuiTreeNodeFlags_SpanAvailWidth)) {
        // Count controls
        if (ImGui::SmallButton("+")) {
            if (s_pool_count < (int)(sizeof(s_pool) / sizeof(s_pool[0])))
                s_pool_count++;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("-")) {
            if (s_pool_count > 0)
                s_pool_count--;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Reset Defaults")) {
            const int def[7] = {744, 780, 799, 800, 805, 806, 1691};
            s_pool_count     = 7;
            for (int i = 0; i < s_pool_count; ++i)
                s_pool[i] = def[i];
        }
        ImGui::Text("Count: %d (max %d)", s_pool_count, (int)(sizeof(s_pool) / sizeof(s_pool[0])));

        // Entries
        for (int i = 0; i < s_pool_count; ++i) {
            ImGui::InputInt((std::string("ID[") + std::to_string(i) + "]").c_str(), &s_pool[i]);
        }
        ImGui::TreePop();
    }
}

// during load
void SoundEffects::on_config_load(const utility::Config& cfg) {
    disable_death_screams = cfg.get<bool>("disable_death_screams").value_or(false);
    custom_death_screams  = cfg.get<bool>("custom_death_screams").value_or(false);
    if (disable_death_screams || custom_death_screams) {
        ToggleDeathScreamExtras(true);
    }

    s_custom_scream_id      = cfg.get<int>("custom_scream_id").value_or(s_custom_scream_id);
    s_randomize_custom_pool = cfg.get<bool>("randomize_custom_pool").value_or(false);

    // Pool persistence (flat ints, no vector)
    s_pool_count = cfg.get<int>("scream_pool_count").value_or(s_pool_count);
    if (s_pool_count < 0)
        s_pool_count = 0;
    int cap = (int)(sizeof(s_pool) / sizeof(s_pool[0]));
    if (s_pool_count > cap)
        s_pool_count = cap;

    for (int i = 0; i < s_pool_count; ++i) {
        char key[64];
        snprintf(key, sizeof(key), "scream_pool_%d", i);
        s_pool[i] = cfg.get<int>(key).value_or(s_pool[i]);
    }
}

// during save
void SoundEffects::on_config_save(utility::Config& cfg) {
    cfg.set<bool>("disable_death_screams", disable_death_screams);
    cfg.set<bool>("custom_death_screams", custom_death_screams);

    cfg.set<int>("custom_scream_id", s_custom_scream_id);
    cfg.set<bool>("randomize_custom_pool", s_randomize_custom_pool);

    // Pool persistence (flat ints)
    cfg.set<int>("scream_pool_count", s_pool_count);
    for (int i = 0; i < s_pool_count; ++i) {
        char key[64];
        snprintf(key, sizeof(key), "scream_pool_%d", i);
        cfg.set<int>(key, s_pool[i]);
    }
}

// do something every frame
// void SoundEffects::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
// void SoundEffects::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
