#include "SoundTracker.hpp"
#if 1

std::optional<std::string> SoundTracker::on_initialize() {
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

void SoundTracker::on_draw_ui() {
    if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        ImGui::Text("Player -> mSnd:");

        if (ImGui::CollapsingHeader("Lightsaber Ready SE")) {
            ImGui::InputInt("SE Handle", &player->mSnd.seHdlLightsavorReady);
            ImGui::InputFloat("SE Volume", &player->mSnd.seVolLightsavorReady);
            ImGui::Checkbox("Don't Replay", &player->mSnd.seDontPlaySeAgainLSReady);
            if (ImGui::Button("Reset LS Ready Volume")) {
                player->mSnd.seVolLightsavorReady = 1.0f;
            }
        }

        if (ImGui::CollapsingHeader("Slow SE + BGM")) {
            ImGui::InputInt("SE Handle Slow", &player->mSnd.seHdlSlow);
            ImGui::InputFloat("Old BGM Volume", &player->mSnd.oldBgmVol);
            ImGui::InputInt("Old BGM Handle", &player->mSnd.oldBgmHdl);
            ImGui::InputInt("Now BGM Handle", &player->mSnd.nowBgmHdl);
            if (ImGui::Button("Mute BGM")) {
                player->mSnd.oldBgmVol = 0.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button("Restore BGM")) {
                player->mSnd.oldBgmVol = 1.0f;
            }
        }

        ImGui::InputInt("SE Read Process", (int*)(&player->mSnd.seReadProc));

        if (ImGui::TreeNodeEx("Pitch BGM", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat("Current", &player->mSnd.pitchBgm.mCurValue);
            ImGui::InputFloat("Destination", &player->mSnd.pitchBgm.mDstValue);
            ImGui::InputFloat("Source", &player->mSnd.pitchBgm.mSrcValue);
            ImGui::InputInt("Count", (int*)(&player->mSnd.pitchBgm.mCount));
            ImGui::InputInt("Time", (int*)(&player->mSnd.pitchBgm.mTime));
            ImGui::InputInt("Interpolation Type", &player->mSnd.pitchBgm.mIpType);
            ImGui::InputFloat("Motion Rate", &player->mSnd.pitchBgm.mMotionRate);
            if (ImGui::Button("Reset Pitch BGM")) {
                player->mSnd.pitchBgm.mCurValue = 1.0f;
                player->mSnd.pitchBgm.mDstValue = 1.0f;
                player->mSnd.pitchBgm.mSrcValue = 1.0f;
                player->mSnd.pitchBgm.mCount    = 0;
            }
            ImGui::TreePop();
        }

        ImGui::InputInt("Old Finish BGM Volume", (int*)(&player->mSnd.oldFinishBgmVol));
        ImGui::InputInt("Old Finish SE Volume", (int*)(&player->mSnd.oldFinishSeVol));

        if (ImGui::CollapsingHeader("Charge SE")) {
            ImGui::InputInt("SE Handle Charge", &player->mSnd.seHdlCharge);
            if (ImGui::TreeNodeEx("Pitch Charge", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::InputFloat("Current", &player->mSnd.pitchCharge.mCurValue);
                ImGui::InputFloat("Destination", &player->mSnd.pitchCharge.mDstValue);
                ImGui::InputFloat("Source", &player->mSnd.pitchCharge.mSrcValue);
                ImGui::InputInt("Count", (int*)(&player->mSnd.pitchCharge.mCount));
                ImGui::InputInt("Time", (int*)(&player->mSnd.pitchCharge.mTime));
                ImGui::InputInt("Interpolation Type", &player->mSnd.pitchCharge.mIpType);
                ImGui::InputFloat("Motion Rate", &player->mSnd.pitchCharge.mMotionRate);
                if (ImGui::Button("Reset Pitch Charge")) {
                    player->mSnd.pitchCharge.mCurValue = 1.0f;
                    player->mSnd.pitchCharge.mDstValue = 1.0f;
                    player->mSnd.pitchCharge.mSrcValue = 1.0f;
                    player->mSnd.pitchCharge.mCount    = 0;
                }
                ImGui::TreePop();
            }

            ImGui::InputInt("SE Handle Charge Max", &player->mSnd.seHdlChargeMax);

            if (ImGui::TreeNodeEx("Pitch Charge Max", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::InputFloat("Current", &player->mSnd.pitchChargeMax.mCurValue);
                ImGui::InputFloat("Destination", &player->mSnd.pitchChargeMax.mDstValue);
                ImGui::InputFloat("Source", &player->mSnd.pitchChargeMax.mSrcValue);
                ImGui::InputInt("Count", (int*)(&player->mSnd.pitchChargeMax.mCount));
                ImGui::InputInt("Time", (int*)(&player->mSnd.pitchChargeMax.mTime));
                ImGui::InputInt("Interpolation Type", &player->mSnd.pitchChargeMax.mIpType);
                ImGui::InputFloat("Motion Rate", &player->mSnd.pitchChargeMax.mMotionRate);
                if (ImGui::Button("Reset Pitch Charge Max")) {
                    player->mSnd.pitchChargeMax.mCurValue = 1.0f;
                    player->mSnd.pitchChargeMax.mDstValue = 1.0f;
                    player->mSnd.pitchChargeMax.mSrcValue = 1.0f;
                    player->mSnd.pitchChargeMax.mCount    = 0;
                }
                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("Misc Sound Handles")) {
            ImGui::InputInt("SE Handle Alarm", &player->mSnd.seHdlAlarm);
            ImGui::InputInt("SE Handle Tel", &player->mSnd.seHdlTel);
            ImGui::InputInt("SE Handle DS Breath", &player->mSnd.seHdlDsBreath);
            ImGui::InputInt("SE Handle Tsubazeri", &player->mSnd.seHdlTsubazeri);
        }
    }
}

// during load
//void SoundTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void SoundTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
//void SoundTracker::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void SoundTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
