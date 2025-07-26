#include "BrainAge.hpp"
#if 1
bool BrainAge::imguiPopout = false;
float BrainAge::custom_anim_speed = 1.0f;
static bool isPlayingAnimPlaylist = false;
uintptr_t BrainAge::anim_speed_jmp_ret = NULL;

template<typename T>
bool getBit(T flags, int bit) {
    return (flags & (1 << bit)) != 0;
}

template<typename T>
void setBit(T& flags, int bit, bool value) {
    if (value) {
        flags |= (1 << bit);  // Set the bit
    } else {
        flags &= ~(1 << bit); // Clear the bit
    }
}

void BrainAge::Stuff() {
if (ImGui::CollapsingHeader("Anim Player")) {
    static std::unordered_map<std::string, std::vector<AnimationEntry>> animationPlaylists;
    static size_t currentIndex = 0;
    static float timeSinceLast = 0.0f;
    static float lastTime = 0.0f;
    float fontSize = ImGui::GetFontSize();
    static std::string currentWeapon = "Blood Berry";
    
    if (ImGui::BeginTabBar("##weapon_tabs")) {
        for (const auto& weaponData : weaponAnimationData) {
            if (ImGui::BeginTabItem(weaponData.name)) {
                currentWeapon = weaponData.name;
                auto& playlist = animationPlaylists[currentWeapon];
                ImGui::Text("Animation Playlist:");
                
                for (auto it = playlist.begin(); it != playlist.end(); ) {
                    int i = std::distance(playlist.begin(), it);
                    ImGui::Text("%d: ID %d", i + 1, it->id);
                    ImGui::SameLine(fontSize * 3.0f);
                    
                    bool shouldRemove = ImGui::SmallButton(("Remove##" + std::to_string(i)).c_str());
                    ImGui::SameLine();

                    ImGui::Text("Delay:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(fontSize * 4.0f);
                    ImGui::SliderFloat(("##DelaySliderFloat" + std::to_string(i)).c_str(), &it->delay, 0.01f, 2.0f, "%.1f");
                    ImGui::SameLine();
                    ImGui::Text("Speed:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(fontSize * 4.0f);
                    ImGui::SliderFloat(("##SpeedSliderFloat" + std::to_string(i)).c_str(), &it->speed, 0.01f, 10.0f, "%.1f");
                    
                    if (shouldRemove) {
                        it = playlist.erase(it);
                    } else {
                        ++it;
                    }
                }

                if (ImGui::Button("Play All")) {
                    if (!playlist.empty()) {
                        isPlayingAnimPlaylist = true;
                        currentIndex = 0;
                        timeSinceLast = playlist[currentIndex].delay;
                        lastTime = ImGui::GetTime();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear Playlist")) {
                    playlist.clear();
                }

                ImGui::Separator();
                for (const auto& anim : weaponData.animations) {
                    std::string buttonLabel = std::string(anim.first) + " (" + std::to_string(anim.second) + ")";
                    if (ImGui::Button(buttonLabel.c_str())) {
                        // new entry                                                 delay, speed
                        animationPlaylists[weaponData.name].emplace_back(anim.second, 0.2f, 1.0f);
                    }
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    
    float currentTime = ImGui::GetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if (isPlayingAnimPlaylist && currentIndex < animationPlaylists[currentWeapon].size()) {
        timeSinceLast += deltaTime;
        if (timeSinceLast >= animationPlaylists[currentWeapon][currentIndex].delay) {
            timeSinceLast = 0.0f;
            if (auto* player = nmh_sdk::get_mHRPc()) {
                const auto& entry = animationPlaylists[currentWeapon][currentIndex];
                custom_anim_speed = entry.speed;
                nmh_sdk::PlayMotion((pcMotion)entry.id, 0, 0, 0, entry.speed);
            }
            ++currentIndex;
            if (currentIndex >= animationPlaylists[currentWeapon].size()) {
                isPlayingAnimPlaylist = false;
                currentIndex = 0;
            }
        }
    }
}
    if (ImGui::CollapsingHeader("New thing 1")) {

    }

    if (ImGui::CollapsingHeader("New thing 2")) {

    }

    static bool disableCamBeingAutoSet = false;
    if (ImGui::Checkbox("Disable the game setting cam mode", &disableCamBeingAutoSet)) {
        BrainAge::toggleCam(disableCamBeingAutoSet);
    }
}

void BrainAge::on_draw_ui() {
    ImGui::Checkbox("Brain Age Popout", &imguiPopout);
    if (!imguiPopout)
        Stuff();
}

//void BrainAge::custom_imgui_window() {}

// during load
//void BrainAge::on_config_load(const utility::Config &cfg) {}
// during save
//void BrainAge::on_config_save(utility::Config &cfg) {}
// do something every frame
void BrainAge::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("imguiPopout", &imguiPopout);
        Stuff();
        ImGui::End();
    }
}

void BrainAge::toggleCam(bool enable) {
    if (enable) {
        // stops cam being set
        install_patch_offset(0x3EC616, m_patch1, "\x90\x90\x90\x90\x90\x90", 6); // 
    }
    else {
        install_patch_offset(0x3EC616, m_patch1, "\x89\x0D\xA0\xA4\x7E\x01", 6); // mov [nmh.gHrCamera],ecx
    }
}

// clang-format off
naked void detour_anim_speed() { 
    __asm {
        cmp byte ptr [isPlayingAnimPlaylist], 0
        je originalcode

        movss xmm0, [BrainAge::custom_anim_speed]
        jmp retcode

        originalcode:
        mulss xmm0, [edi+0x00000194]
        retcode:
        jmp dword ptr [BrainAge::anim_speed_jmp_ret]
    }
}
// clang-format on

std::optional<std::string> BrainAge::on_initialize() {
    if (!install_hook_offset(0x403CA7, anim_speed_hook, &detour_anim_speed, &BrainAge::anim_speed_jmp_ret, 8)) {
        spdlog::error("Failed to init anim_speed mod\n");
        return "Failed to init anim_speed mod";
    }

    return Mod::on_initialize();
}

// will show up in debug window, dump ImGui widgets you want here
//void BrainAge::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
