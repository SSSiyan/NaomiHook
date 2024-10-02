#include "PlayerTracker.hpp"
#if 1

std::optional<std::string> PlayerTracker::on_initialize() {
    return Mod::on_initialize();
}

void PlayerTracker::on_draw_ui() {
    if (mHRBattle* mediator = nmh_sdk::get_mediator()){
        if (ImGui::CollapsingHeader("mHRBattle")) {
            ImGui::InputInt("mBtlInitProc", (int*)&mediator->mBtlInitProc);
            ImGui::InputInt("mBtlFrameProc", (int*)&mediator->mBtlFrameProc);
            ImGui::InputInt("mpPc", (int*)&mediator->mpPc);
            ImGui::InputInt("mTotalNpcNum", &mediator->mTotalNpcNum);
            ImGui::InputInt("mTotalKillNum", &mediator->mTotalKillNum);
            ImGui::InputInt("mKillNum", &mediator->mKillNum);
            ImGui::InputFloat("mNpcAttackRate", &mediator->mNpcAttackRate);

            ImGui::Checkbox("battlePause", (bool*)&mediator->mFlag);
            ImGui::Checkbox("tutoRun", (bool*)&mediator->mFlag + 1);
            ImGui::Checkbox("dispStatusDisEnable", (bool*)&mediator->mFlag + 2);
            ImGui::Checkbox("chargeDamage", (bool*)&mediator->mFlag + 3);
        }
        if (mHRPc* player = nmh_sdk::get_local_player()) {
            if (ImGui::CollapsingHeader("mHRPc")) {
                ImGui::Checkbox("mPauseAll", &player->mPauseAll);
                ImGui::Checkbox("mPauseNpc", &player->mPauseNpc);
                ImGui::Checkbox("mOperate", &player->mOperate);
                ImGui::Checkbox("mOnlyMove", &player->mOnlyMove);
                ImGui::Checkbox("mMotSpdAdj", &player->mMotSpdAdj);
                ImGui::Checkbox("mDead", &player->mDead);
                ImGui::Checkbox("mDeadPause", &player->mDeadPause);
                ImGui::Checkbox("mCameraOperate", &player->mCameraOperate);
                ImGui::Checkbox("mBattouDemoRequest", &player->mBattouDemoRequest);
                ImGui::Checkbox("mStageChangeInitEnd", &player->mStageChangeInitEnd);
                ImGui::Checkbox("mStageChangeTermEnd", &player->mStageChangeTermEnd);
                ImGui::Checkbox("mStageChangeMuteki", &player->mStageChangeMuteki);
                ImGui::Checkbox("mBanStatusScreen", &player->mBanStatusScreen);

                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->pad_00);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mOperate);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                // ImGui::Text("Target: 0x299e");

                ImGui::InputInt("resNo", &player->mCharaStatus.resNo);
                ImGui::InputInt("datNo", &player->mCharaStatus.datNo);
                ImGui::InputInt("charaType", (int*)&player->mCharaStatus.charaType);
                ImGui::InputScalar("zakoWepType", ImGuiDataType_S16, &player->mCharaStatus.zakoWepType);
                ImGui::InputFloat("maxHp", &player->mCharaStatus.maxHp);
                ImGui::InputFloat("hp", &player->mCharaStatus.hp);
                ImGui::InputScalar("money", ImGuiDataType_S16, &player->mCharaStatus.money);
                ImGui::InputScalar("dropMoney", ImGuiDataType_S16, &player->mCharaStatus.dropMoney);
                ImGui::InputFloat("tension", &player->mCharaStatus.tension);
                ImGui::InputFloat("hajikiCount", &player->mCharaStatus.hajikiCount);
                ImGui::Checkbox("alwaysCheckHitColl", &player->mCharaStatus.alwaysCheckHitColl);
                ImGui::InputFloat3("pos", &player->mCharaStatus.pos.x);
                ImGui::InputFloat3("beforePos", &player->mCharaStatus.beforePos.x);
                ImGui::InputFloat3("navelPos", &player->mCharaStatus.navelPos.x);
                ImGui::InputFloat3("brainPos", &player->mCharaStatus.brainPos.x);
                ImGui::InputFloat3("rot", &player->mCharaStatus.rot.x);
                ImGui::InputFloat3("movRot", &player->mCharaStatus.movRot.x);
                ImGui::InputFloat4("qRot", &player->mCharaStatus.qRot.x);
                ImGui::InputFloat("movSpd", &player->mCharaStatus.movSpd);
                ImGui::InputFloat("hitSize", &player->mCharaStatus.hitSize);
                ImGui::InputFloat("height", &player->mCharaStatus.height);
                ImGui::InputFloat("upperPosY", &player->mCharaStatus.upperPosY);
                ImGui::InputInt("enterPattern", &player->mCharaStatus.enterPattern);
                ImGui::InputFloat("reactionDist", &player->mCharaStatus.reactionDist);
                ImGui::InputInt("itemNo", &player->mCharaStatus.itemNo);
                ImGui::InputInt("mAiDefBasic", &player->mCharaStatus.mAiDefBasic);
                ImGui::InputInt("mAiDefCurrent", &player->mCharaStatus.mAiDefCurrent);
                ImGui::InputInt("mAiDefTick", &player->mCharaStatus.mAiDefTick);
                ImGui::InputInt("mAiDefBaseTick", &player->mCharaStatus.mAiDefBaseTick);
                ImGui::InputFloat("mAiDamageCount", &player->mCharaStatus.mAiDamageCount);
                ImGui::InputInt("tsubazeriNum", &player->mCharaStatus.tsubazeriNum);
                ImGui::InputInt("DamageAcceptFrame", &player->mCharaStatus.DamageAcceptFrame);
                ImGui::InputInt("motionNo", &player->mCharaStatus.motionNo);
                ImGui::InputScalar("motionBrendNum", ImGuiDataType_S8, &player->mCharaStatus.motionBrendNum);
                ImGui::InputFloat("motSpd", &player->mCharaStatus.motSpd);
                ImGui::InputInt("condition", (int*)&player->mCharaStatus.condition);
                ImGui::InputInt("footHitResultObj", (int*)&player->mCharaStatus.footHitResultObj);
                ImGui::InputInt("footHitResult", (int*)&player->mCharaStatus.footHitResult);
                ImGui::InputInt("footHitResultObj2", (int*)&player->mCharaStatus.footHitResultObj2);
                ImGui::InputInt("hitWallMaterial", (int*)&player->mCharaStatus.hitWallMaterial);
                ImGui::InputInt("dmgInfo", (int*)&player->mCharaStatus.dmgInfo);
                ImGui::InputFloat3("mYukaNormal", &player->mCharaStatus.mYukaNormal.x);
                ImGui::InputInt("movTick", &player->mCharaStatus.movTick);
                ImGui::InputScalar("hitNum", ImGuiDataType_S16, &player->mCharaStatus.hitNum);
                ImGui::InputInt("mAiAtkType", &player->mCharaStatus.mAiAtkType);
                ImGui::InputInt("mAiAtkClass", &player->mCharaStatus.mAiAtkClass);
                for (int i = 0; i < 10; ++i) {
                    ImGui::InputScalar(("AtkAi[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &player->mCharaStatus.AtkAi[i]);
                }
                ImGui::InputInt("renderSkipCounter", &player->mCharaStatus.renderSkipCounter);
                ImGui::InputFloat("renderSkipMotSpd", &player->mCharaStatus.renderSkipMotSpd);
                ImGui::InputInt("frameStop", &player->mCharaStatus.frameStop);
                ImGui::Checkbox("visible", (bool*)&player->mCharaStatus.flag[0]);
                ImGui::Checkbox("visibleWep", (bool*)&player->mCharaStatus.flag[0] + 1);
                ImGui::Checkbox("visibleWepEffect", (bool*)&player->mCharaStatus.flag[0] + 2);
                ImGui::Checkbox("visibleDismember", (bool*)&player->mCharaStatus.flag[0] + 3);
                ImGui::Checkbox("visibleDist", (bool*)&player->mCharaStatus.flag[0] + 4);
                ImGui::Checkbox("lockOn", (bool*)&player->mCharaStatus.flag[0] + 5);
            }
            if (ImGui::CollapsingHeader("mHRPc stPcStatus")) {
                ImGui::SliderFloat("Strength", &player->mPcStatus.strength, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &player->mPcStatus.stammina, 0.0f, 100.0f);
                ImGui::SliderFloat("Vitality", &player->mPcStatus.vitality, 0.0f, 100.0f);
                ImGui::InputInt("Battery Tick", &player->mPcStatus.batteryTick);
                ImGui::InputInt("Attack 360 Tick", &player->mPcStatus.attack360Tick);
                ImGui::SliderFloat("Heroes Meter", &player->mPcStatus.heroesMeter, 0.0f, 100.0f);
                ImGui::InputInt("Heroes Meter Tick", &player->mPcStatus.heroesMeterTick);
                ImGui::InputInt("Dark Side Use Num", &player->mPcStatus.darkSideUseNum);
                ImGui::InputInt("Dark Side Key", &player->mPcStatus.darkSideKey);
                ImGui::InputInt("Dark Side Key Tick", &player->mPcStatus.darkSideKeyTick);
                ImGui::InputInt("Dark Side Num", &player->mPcStatus.darkSideNum);
                ImGui::InputInt("Dark Side Motion", &player->mPcStatus.darkSideMotion);
                ImGui::Checkbox("Dark Side End Wait", &player->mPcStatus.darkSideEndWait);
                ImGui::Checkbox("Dark Side Auto Start", &player->mPcStatus.darkSideAutoStart);
                ImGui::Checkbox("Dark Side Key Guide Disp", &player->mPcStatus.darkSideKeyGuideDisp);
                ImGui::Checkbox("Dark Side Success Input", &player->mPcStatus.darkSideSuccessInput);
                ImGui::Checkbox("Rapid Finish End", &player->mPcStatus.rapidFinishEnd);
                ImGui::InputInt("Money", &player->mPcStatus.money);
                ImGui::InputInt("Max Money", &player->mPcStatus.moneyMax);
                ImGui::SliderFloat("Tsuba Rate", &player->mPcStatus.tsubaRate, 0.0f, 1.0f);
                ImGui::Checkbox("Tsuba Osare Flag", &player->mPcStatus.tsubaOsareFlag);
                ImGui::InputInt("Finish Motion No", &player->mPcStatus.finishMotNo);
                ImGui::InputInt("Gacha Tick", &player->mPcStatus.gachaTick);
                ImGui::Checkbox("Dig Tool", &player->mPcStatus.digTool);
                ImGui::Checkbox("Rapid Finish", &player->mPcStatus.rapidFinish);
                ImGui::Checkbox("Already Hit Rapid Finish", &player->mPcStatus.alreadyHitRapidFinish);
                ImGui::Checkbox("CMB Flag", &player->mPcStatus.cmbFlag);
                ImGui::Checkbox("Bike Visible", &player->mPcStatus.bikeVisible);
                ImGui::Checkbox("Puppet Mode", &player->mPcStatus.puppetMode);
                ImGui::Checkbox("Use Weapon Effect", &player->mPcStatus.useWeaponEffect);
                ImGui::SliderFloat("Money Up Rate", &player->mPcStatus.moneyUpRate, 0.0f, 100.0f);
                ImGui::SliderFloat("Jump Down Attack Dist", &player->mPcStatus.jumpDownAttackDist, 0.0f, 100.0f);
                ImGui::InputInt("Idling Tick", &player->mPcStatus.idlingTick);
                ImGui::Checkbox("Finish Bonus", &player->mPcStatus.finishBonus);
            }
            if (ImGui::CollapsingHeader("mpLockOnNpc")) {
                if (player->mpLockOnNpc) {
                    ImGui::InputFloat("mDistFromPc", &player->mpLockOnNpc->mDistFromPc);
                    ImGui::InputFloat("mDirecYFromPc", &player->mpLockOnNpc->mDirecYFromPc);
                    ImGui::InputInt("mInitProc", (int*)&player->mpLockOnNpc->mInitProc);
                }
            }
            if (ImGui::CollapsingHeader("mpLockOnNpc mStatus")) {
                if (player->mpLockOnNpc) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->mpLockOnNpc->mZakoWepMotionNumMax[0]);
                // uintptr_t statusAddress = reinterpret_cast<uintptr_t>(&player->mpLockOnNpc->mDirecYFromPc);
                // uintptr_t offsetDifference = statusAddress - baseAddress;
                // ImGui::Text("Offset target: 0x304 | Offset: 0x%08X", offsetDifference);

                ImGui::InputInt("resNo", &player->mpLockOnNpc->mStatus.resNo);
                ImGui::InputInt("datNo", &player->mpLockOnNpc->mStatus.datNo);
                ImGui::InputInt("charaType", (int*)&player->mpLockOnNpc->mStatus.charaType);
                ImGui::InputScalar("zakoWepType", ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.zakoWepType);
                ImGui::InputFloat("maxHp", &player->mpLockOnNpc->mStatus.maxHp);
                ImGui::InputFloat("hp", &player->mpLockOnNpc->mStatus.hp);
                ImGui::InputScalar("money", ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.money);
                ImGui::InputScalar("dropMoney", ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.dropMoney);
                ImGui::InputFloat("tension", &player->mpLockOnNpc->mStatus.tension);
                ImGui::InputFloat("hajikiCount", &player->mpLockOnNpc->mStatus.hajikiCount);
                ImGui::Checkbox("alwaysCheckHitColl", &player->mpLockOnNpc->mStatus.alwaysCheckHitColl);
                ImGui::InputFloat3("pos", &player->mpLockOnNpc->mStatus.pos.x);
                ImGui::InputFloat3("beforePos", &player->mpLockOnNpc->mStatus.beforePos.x);
                ImGui::InputFloat3("navelPos", &player->mpLockOnNpc->mStatus.navelPos.x);
                ImGui::InputFloat3("brainPos", &player->mpLockOnNpc->mStatus.brainPos.x);
                ImGui::InputFloat3("rot", &player->mpLockOnNpc->mStatus.rot.x);
                ImGui::InputFloat3("movRot", &player->mpLockOnNpc->mStatus.movRot.x);
                ImGui::InputFloat4("qRot", &player->mpLockOnNpc->mStatus.qRot.x);
                ImGui::InputFloat("movSpd", &player->mpLockOnNpc->mStatus.movSpd);
                ImGui::InputFloat("hitSize", &player->mpLockOnNpc->mStatus.hitSize);
                ImGui::InputFloat("height", &player->mpLockOnNpc->mStatus.height);
                ImGui::InputFloat("upperPosY", &player->mpLockOnNpc->mStatus.upperPosY);
                ImGui::InputInt("enterPattern", &player->mpLockOnNpc->mStatus.enterPattern);
                ImGui::InputFloat("reactionDist", &player->mpLockOnNpc->mStatus.reactionDist);
                ImGui::InputInt("itemNo", &player->mpLockOnNpc->mStatus.itemNo);
                ImGui::InputInt("mAiDefBasic", &player->mpLockOnNpc->mStatus.mAiDefBasic);
                ImGui::InputInt("mAiDefCurrent", &player->mpLockOnNpc->mStatus.mAiDefCurrent);
                ImGui::InputInt("mAiDefTick", &player->mpLockOnNpc->mStatus.mAiDefTick);
                ImGui::InputInt("mAiDefBaseTick", &player->mpLockOnNpc->mStatus.mAiDefBaseTick);
                ImGui::InputFloat("mAiDamageCount", &player->mpLockOnNpc->mStatus.mAiDamageCount);
                ImGui::InputInt("tsubazeriNum", &player->mpLockOnNpc->mStatus.tsubazeriNum);
                ImGui::InputInt("DamageAcceptFrame", &player->mpLockOnNpc->mStatus.DamageAcceptFrame);
                ImGui::InputInt("motionNo", &player->mpLockOnNpc->mStatus.motionNo);
                ImGui::InputScalar("motionBrendNum", ImGuiDataType_S8, &player->mpLockOnNpc->mStatus.motionBrendNum);
                ImGui::InputFloat("motSpd", &player->mpLockOnNpc->mStatus.motSpd);
                ImGui::InputInt("condition", (int*)&player->mpLockOnNpc->mStatus.condition);
                ImGui::InputInt("footHitResultObj", (int*)&player->mpLockOnNpc->mStatus.footHitResultObj);
                ImGui::InputInt("footHitResult", (int*)&player->mpLockOnNpc->mStatus.footHitResult);
                ImGui::InputInt("footHitResultObj2", (int*)&player->mpLockOnNpc->mStatus.footHitResultObj2);
                ImGui::InputInt("hitWallMaterial", (int*)&player->mpLockOnNpc->mStatus.hitWallMaterial);
                ImGui::InputInt("dmgInfo", (int*)&player->mpLockOnNpc->mStatus.dmgInfo);
                ImGui::InputFloat3("mYukaNormal", &player->mpLockOnNpc->mStatus.mYukaNormal.x);
                ImGui::InputInt("movTick", &player->mpLockOnNpc->mStatus.movTick);
                ImGui::InputScalar("hitNum", ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.hitNum);
                ImGui::InputInt("mAiAtkType", &player->mpLockOnNpc->mStatus.mAiAtkType);
                ImGui::InputInt("mAiAtkClass", &player->mpLockOnNpc->mStatus.mAiAtkClass);
                for (int i = 0; i < 10; ++i) {
                    ImGui::InputScalar(("AtkAi[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.AtkAi[i]);
                }
                ImGui::InputInt("renderSkipCounter", &player->mpLockOnNpc->mStatus.renderSkipCounter);
                ImGui::InputFloat("renderSkipMotSpd", &player->mpLockOnNpc->mStatus.renderSkipMotSpd);
                ImGui::InputInt("frameStop", &player->mpLockOnNpc->mStatus.frameStop);
                ImGui::Checkbox("visible", (bool*)&player->mpLockOnNpc->mStatus.flag[0]);
                ImGui::Checkbox("visibleWep", (bool*)&player->mpLockOnNpc->mStatus.flag[0] + 1);
                ImGui::Checkbox("visibleWepEffect", (bool*)&player->mpLockOnNpc->mStatus.flag[0] + 2);
                ImGui::Checkbox("visibleDismember", (bool*)&player->mpLockOnNpc->mStatus.flag[0] + 3);
                ImGui::Checkbox("visibleDist", (bool*)&player->mpLockOnNpc->mStatus.flag[0] + 4);
                ImGui::Checkbox("lockOn", (bool*)&player->mpLockOnNpc->mStatus.flag[0] + 5);
                }
            }
        }
    }
}

// during load
//void PlayerTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void PlayerTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
//void PlayerTracker::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void PlayerTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
