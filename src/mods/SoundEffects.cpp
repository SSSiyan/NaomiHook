#include "SoundEffects.hpp"
#if 1
bool SoundEffects::disable_death_screams = false;
bool SoundEffects::custom_death_screams  = false;
uintptr_t SoundEffects::jmp_ret1 = NULL;
uintptr_t SoundEffects::ghm_snd_prPrep = NULL;
int SoundEffects::temp_sound_id = 0;

void SoundEffects::ToggleDeathScreamExtras(bool enable) {
    if (enable) {
        install_patch_offset(0x42DF28, m_patch1, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghmGcSound::PrepPlay
        install_patch_offset(0x42DF45, m_patch2, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghm_snd_prPrep
        //install_patch_offset(0x42DF79, m_patch3, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghmGcSound::PrepPlay I think this was the wrong one :D
        install_patch_offset(0x42DFCA, m_patch3, "\x90\x90\x90\x90\x90", 5); // nop call nmh.ghmGcSound::PrepPlay
    }
    else {
        m_patch1.reset();
        m_patch2.reset();
        //m_patch3.reset();
        m_patch3.reset();
    }
}

void SoundEffects::GetCustomScreamSound(int soundID) {
    // throw some logic here for randomizing which death scream plays. Here's an example:
    
    /*mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { return; }
    int motionID  = player->mCharaStatus.motionNo;
    if (motionID == 20) temp_sound_id == 0x680;*/

    // for now just use a static sound
    temp_sound_id = 0x680;
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
}

// during load
void SoundEffects::on_config_load(const utility::Config &cfg) {
    disable_death_screams = cfg.get<bool>("disable_death_screams").value_or(false);
    custom_death_screams  = cfg.get<bool>("custom_death_screams").value_or(false);
    if (disable_death_screams || custom_death_screams) {
        ToggleDeathScreamExtras(true);
    }
}

// during save
void SoundEffects::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("disable_death_screams", disable_death_screams);
    cfg.set<bool>("custom_death_screams", custom_death_screams);
}

// do something every frame
//void SoundEffects::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void SoundEffects::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
