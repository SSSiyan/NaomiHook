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
        if (ImGui::CollapsingHeader("Player -> mSnd", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputInt("SE Handle Lightsavor Ready", &player->mSnd.seHdlLightsavorReady);
            ImGui::InputFloat("SE Volume Lightsavor Ready", &player->mSnd.seVolLightsavorReady);
            ImGui::Checkbox("SE Don't Play Again LS Ready", &player->mSnd.seDontPlaySeAgainLSReady);
            ImGui::InputInt("SE Handle Slow", &player->mSnd.seHdlSlow);
            ImGui::InputFloat("Old BGM Volume", &player->mSnd.oldBgmVol);
            ImGui::InputInt("Old BGM Handle", &player->mSnd.oldBgmHdl);
            ImGui::InputInt("Now BGM Handle", &player->mSnd.nowBgmHdl);
            ImGui::InputInt("SE Read Process", (int*)(&player->mSnd.seReadProc));

            if (ImGui::CollapsingHeader("Pitch BGM")) {
                ImGui::Indent();
                ImGui::InputFloat("Current Value ## Pitch BGM", &player->mSnd.pitchBgm.mCurValue);
                ImGui::InputFloat("Destination Value ## Pitch BGM", &player->mSnd.pitchBgm.mDstValue);
                ImGui::InputFloat("Source Value ## Pitch BGM", &player->mSnd.pitchBgm.mSrcValue);
                ImGui::InputInt("Count ## Pitch BGM", (int*)(&player->mSnd.pitchBgm.mCount));
                ImGui::InputInt("Time ## Pitch BGM", (int*)(&player->mSnd.pitchBgm.mTime));
                ImGui::InputInt("Interpolation Type ## Pitch BGM", &player->mSnd.pitchBgm.mIpType);
                ImGui::InputFloat("Motion Rate ## Pitch BGM", &player->mSnd.pitchBgm.mMotionRate);
                ImGui::Unindent();
            }

            ImGui::InputInt("Old Finish BGM Volume", (int*)(&player->mSnd.oldFinishBgmVol));
            ImGui::InputInt("Old Finish SE Volume", (int*)(&player->mSnd.oldFinishSeVol));
            ImGui::InputInt("SE Handle Charge", &player->mSnd.seHdlCharge);

            if (ImGui::CollapsingHeader("Pitch Charge")) {
                ImGui::Indent();
                ImGui::InputFloat("Current Value ## Pitch Charge", &player->mSnd.pitchCharge.mCurValue);
                ImGui::InputFloat("Destination Value ## Pitch Charge", &player->mSnd.pitchCharge.mDstValue);
                ImGui::InputFloat("Source Value ## Pitch Charge", &player->mSnd.pitchCharge.mSrcValue);
                ImGui::InputInt("Count ## Pitch Charge", (int*)(&player->mSnd.pitchCharge.mCount));
                ImGui::InputInt("Time ## Pitch Charge", (int*)(&player->mSnd.pitchCharge.mTime));
                ImGui::InputInt("Interpolation Type ## Pitch Charge", &player->mSnd.pitchCharge.mIpType);
                ImGui::InputFloat("Motion Rate ## Pitch Charge", &player->mSnd.pitchCharge.mMotionRate);
                ImGui::Unindent();
            }

            ImGui::InputInt("SE Handle Charge Max", &player->mSnd.seHdlChargeMax);

            if (ImGui::CollapsingHeader("Pitch Charge Max")) {
                ImGui::Indent();
                ImGui::InputFloat("Current Value ## Pitch Charge Max", &player->mSnd.pitchChargeMax.mCurValue);
                ImGui::InputFloat("Destination Value ## Pitch Charge Max", &player->mSnd.pitchChargeMax.mDstValue);
                ImGui::InputFloat("Source Value ## Pitch Charge Max", &player->mSnd.pitchChargeMax.mSrcValue);
                ImGui::InputInt("Count ## Pitch Charge Max", (int*)(&player->mSnd.pitchChargeMax.mCount));
                ImGui::InputInt("Time ## Pitch Charge Max", (int*)(&player->mSnd.pitchChargeMax.mTime));
                ImGui::InputInt("Interpolation Type ## Pitch Charge Max", &player->mSnd.pitchChargeMax.mIpType);
                ImGui::InputFloat("Motion Rate ## Pitch Charge Max", &player->mSnd.pitchChargeMax.mMotionRate);
                ImGui::Unindent();
            }

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
