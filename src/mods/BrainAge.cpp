#include "BrainAge.hpp"
#if 1
bool BrainAge::imguiPopout = false;

std::optional<std::string> BrainAge::on_initialize() {
    return Mod::on_initialize();
}

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

void Stuff() {
    if (ImGui::CollapsingHeader("Anim Player")) {
        // Simple Animation Playlist UI (No Timeline or Popouts)
        static std::unordered_map<std::string, std::vector<std::pair<int, float>>> animationPlaylists;
        // static float defaultAnimationDelay = 0.2f;

        static bool isPlaying = false;
        static size_t currentIndex = 0;
        static float timeSinceLast = 0.0f;
        static float lastTime = 0.0f;
        static std::string currentWeapon = "Blood Berry";

        if (ImGui::BeginTabBar("##weapon_tabs")) {
            // I put weaponAnimationData in the hpp so this file is more easily scrolled
            for (const auto& weaponData : weaponAnimationData) {
                if (ImGui::BeginTabItem(weaponData.name)) {
                    currentWeapon = weaponData.name;
                    auto& playlist = animationPlaylists[currentWeapon];

                    ImGui::Text("Animation Playlist:");
                    for (int i = 0; i < playlist.size(); ++i) {
                        ImGui::Text("%d: ID %d", i + 1, playlist[i].first);
                        ImGui::SameLine();
                        if (ImGui::SmallButton(("Remove##" + std::to_string(i)).c_str()))
                            playlist.erase(playlist.begin() + i);
                        ImGui::SameLine();

                        // Wider delay input box (180px)
                        ImGui::SetNextItemWidth(180);
                        ImGui::InputFloat(("Delay##" + std::to_string(i)).c_str(), &playlist[i].second, 0.01f);
                    }

                    // Removed obsolete global delay input here

                    if (ImGui::Button("Play All")) {
                        if (!playlist.empty()) {
                            isPlaying = true;
                            currentIndex = 0;
                            timeSinceLast = playlist[currentIndex].second;
                            lastTime = ImGui::GetTime();
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Clear Playlist")) {
                        playlist.clear();
                    }

                    ImGui::Separator();

                    for (const auto& anim : weaponData.animations) {
                        if (ImGui::Button(anim.first)) {
                            // Use a default delay of 0.2f per animation here if you want,
                            // or set your own per-animation delay manually after adding
                            animationPlaylists[weaponData.name].emplace_back(anim.second, 0.2f);
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

        if (isPlaying && currentIndex < animationPlaylists[currentWeapon].size()) {
            timeSinceLast += deltaTime;
            if (timeSinceLast >= animationPlaylists[currentWeapon][currentIndex].second) {
                timeSinceLast = 0.0f;
                if (auto* player = nmh_sdk::get_mHRPc()) {
                    nmh_sdk::PlayMotion((pcMotion)animationPlaylists[currentWeapon][currentIndex].first, 0, 0, 0, 0.1f);
                }
                ++currentIndex;
                if (currentIndex >= animationPlaylists[currentWeapon].size()) {
                    isPlaying = false;
                    currentIndex = 0;
                }
            }
        }
    }
    if (ImGui::CollapsingHeader("New thing 1")) {

    }
    if (ImGui::CollapsingHeader("New thing 2")) {

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
// will show up in debug window, dump ImGui widgets you want here
//void BrainAge::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
