#include "AnimPlayer.hpp"
#if 1
bool AnimPlayer::imguiPopout = false;
float AnimPlayer::custom_anim_speed = 1.0f;
static bool isPlayingAnimPlaylist = false;
uintptr_t AnimPlayer::anim_speed_jmp_ret = NULL;

static std::unordered_map<std::string, std::vector<AnimationEntry>> animationPlaylists;
static size_t currentIndex = 0;
static float timeSinceLast = 0.0f;
static float lastTime = 0.0f;
static int lastMotionNo = -1;
static float motionCheckDelay = 0.0f;
static std::string currentWeapon = "Berry";

void AnimPlayer::Stuff() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        float fontSize = ImGui::GetFontSize();
        ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
        // ImGui::Text("Current Index: %i", currentIndex);
        // ImGui::Text("Current Motion No: %i", currentMotion);
        if (ImGui::BeginTabBar("##weapon_tabs")) {
            for (const auto& weaponData : weaponAnimationData) {
                if (ImGui::BeginTabItem(weaponData.name)) {
                    currentWeapon = weaponData.name;
                    //                                                       60% of the avaiable width, 60% of the available height
                    ImGui::BeginChild("##anim_buttons", ImVec2(contentRegionAvail.x * 0.6f, contentRegionAvail.y * 0.6f));
                    for (const auto& anim : weaponData.animations) {
                        std::string buttonLabel = std::string(anim.first) + " (" + std::to_string(anim.second) + ")";
                        if (ImGui::Button(buttonLabel.c_str())) {
                            // new entry                                                 delay, speed
                            animationPlaylists[weaponData.name].emplace_back(anim.second, 0.2f, 1.0f);
                        }
                    }
                    ImGui::EndChild();
                    ImGui::Separator();
                    float sliderWidth = contentRegionAvail.x * 0.2f;
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
                        ImGui::SetNextItemWidth(sliderWidth);
                        ImGui::SliderFloat(("##DelaySliderFloat" + std::to_string(i)).c_str(), &it->delay, 0.01f, 2.0f, "%.1f");
                        ImGui::SameLine();
                        ImGui::Text("Speed:");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(sliderWidth);
                        ImGui::SliderFloat(("##SpeedSliderFloat" + std::to_string(i)).c_str(), &it->speed, 0.01f, 10.0f, "%.1f");
                        if (shouldRemove) {
                            it = playlist.erase(it);
                        }
                        else {
                            ++it;
                        }
                    }
                    if (ImGui::Button("Play All")) {
                        if (!playlist.empty()) {
                            isPlayingAnimPlaylist = true;
                            currentIndex = 0;
                            timeSinceLast = 0.0f;
                            lastTime = ImGui::GetTime();
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Clear Playlist")) {
                        playlist.clear();
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
}

void AnimPlayer::on_draw_ui() {
    ImGui::Checkbox("View In Popout Window", &imguiPopout);
    if (!imguiPopout)
        Stuff();
}

//void AnimPlayer::custom_imgui_window() {}

// during load
//void AnimPlayer::on_config_load(const utility::Config &cfg) {}
// during save
//void AnimPlayer::on_config_save(utility::Config &cfg) {}
// do something every frame

void AnimPlayer::anim_player() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (player) {
        float currentTime = ImGui::GetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        timeSinceLast += deltaTime;
        if (isPlayingAnimPlaylist && currentIndex < animationPlaylists[currentWeapon].size()) {
            if (timeSinceLast >= animationPlaylists[currentWeapon][currentIndex].delay) {
                timeSinceLast = 0.0f;
                if (auto* player = nmh_sdk::get_mHRPc()) {
                    const auto& entry = animationPlaylists[currentWeapon][currentIndex];
                    custom_anim_speed = entry.speed;
                    nmh_sdk::PlayMotion((pcMotion)entry.id, 0, 0, 1, 0.1f);
                    lastMotionNo = entry.id;
                }
                ++currentIndex;

                if (currentIndex >= animationPlaylists[currentWeapon].size()) {
                    motionCheckDelay = 0.1f;
                }
            }
        }

        if (isPlayingAnimPlaylist && currentIndex >= animationPlaylists[currentWeapon].size()) {
            motionCheckDelay -= deltaTime;
            if (motionCheckDelay <= 0.0f) {
                int currentMotion = player->mCharaStatus.motionNo;
                if (currentMotion != lastMotionNo) {
                    isPlayingAnimPlaylist = false;
                    currentIndex = 0;
                    lastMotionNo = -1;
                }
            }
        }
    }
}

void AnimPlayer::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("Animation Player", &imguiPopout);
        Stuff();
        ImGui::End();
    }
    anim_player();
}

// clang-format off
naked void detour_anim_speed() { 
    __asm {
        cmp byte ptr [isPlayingAnimPlaylist], 0
        je originalcode

        movss xmm0, [AnimPlayer::custom_anim_speed]
        jmp retcode

        originalcode:
        mulss xmm0, [edi+0x00000194]
        retcode:
        jmp dword ptr [AnimPlayer::anim_speed_jmp_ret]
    }
}
// clang-format on

std::optional<std::string> AnimPlayer::on_initialize() {
    if (!install_hook_offset(0x403CA7, anim_speed_hook, &detour_anim_speed, &AnimPlayer::anim_speed_jmp_ret, 8)) {
        spdlog::error("Failed to init anim_speed mod\n");
        return "Failed to init anim_speed mod";
    }

    return Mod::on_initialize();
}

// will show up in debug window, dump ImGui widgets you want here
//void AnimPlayer::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
