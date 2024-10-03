#include "PlayerTracker.hpp"
#if 1

std::optional<std::string> PlayerTracker::on_initialize() {
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

void PlayerTracker::on_draw_ui() {
    if (mHRBattle* mediator = nmh_sdk::get_mediator()){
        if (ImGui::CollapsingHeader("mHRBattle")) {
            ImGui::InputInt("mTotalNpcNum", &mediator->mTotalNpcNum);
            ImGui::InputInt("mTotalKillNum", &mediator->mTotalKillNum);
            ImGui::InputInt("mKillNum", &mediator->mKillNum);
            ImGui::InputFloat("mNpcAttackRate", &mediator->mNpcAttackRate);

            bool battlePause = getBit(mediator->mFlag, 0);
            if (ImGui::Checkbox("battlePause", &battlePause)) setBit(mediator->mFlag, 0, battlePause);

            bool tutoRun = getBit(mediator->tutoRun, 1);
            if (ImGui::Checkbox("tutoRun", &tutoRun)) setBit(mediator->mFlag, 1, tutoRun);

            bool dispStatusDisEnable = getBit(mediator->dispStatusDisEnable, 2);
            if (ImGui::Checkbox("dispStatusDisEnable", &dispStatusDisEnable)) setBit(mediator->mFlag, 2, dispStatusDisEnable);

            bool chargeDamage = getBit(mediator->chargeDamage, 3);
            if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(mediator->mFlag, 3, chargeDamage);
        }
        if (ImGui::CollapsingHeader("HrScreenStatus")) {
            if (mediator->mBtEffect.pScreenStatus) {
                uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mediator->mBtEffect.pScreenStatus->Padding_0[0]);
                ImGui::Text("Base Address: 0x%08X", baseAddress);
                uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&mediator->mBtEffect.pScreenStatus->m_StatusProcFlag);
                ImGui::Text("Target Address: 0x%08X", targetAddress);
                uintptr_t offsetDifference = targetAddress - baseAddress;
                ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                ImGui::Text("Param Section");
                ImGui::InputInt3("Sound IDs", mediator->mBtEffect.pScreenStatus->m_SoundID);
                ImGui::InputInt("SprLch Sound ID", &mediator->mBtEffect.pScreenStatus->m_SprLchSoundID);
                ImGui::InputScalar("Herose", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_Herose);
                ImGui::SliderFloat("HP", &mediator->mBtEffect.pScreenStatus->m_HP, 0.0f, 100.0f); // Assuming ranges
                ImGui::SliderFloat("Battery", &mediator->mBtEffect.pScreenStatus->m_Battery, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &mediator->mBtEffect.pScreenStatus->m_Stamina, 0.0f, 100.0f);
                ImGui::SliderFloat2("Tension", mediator->mBtEffect.pScreenStatus->m_Tension, 0.0f, 100.0f);
                ImGui::InputInt2("Money", mediator->mBtEffect.pScreenStatus->m_Money);
                ImGui::InputInt("Money Comp Sound Wait", &mediator->mBtEffect.pScreenStatus->m_MoneyCompSoundWait);
                ImGui::InputInt("Flag Anim Counter", &mediator->mBtEffect.pScreenStatus->m_FlagAnimCounter);
                ImGui::InputInt("Gear Anim Counter", &mediator->mBtEffect.pScreenStatus->m_GearAnimCounter);
                ImGui::InputInt2("Gear Anim Frame", mediator->mBtEffect.pScreenStatus->m_GearAnimFrame);
                ImGui::InputInt("Gear Fade Count", &mediator->mBtEffect.pScreenStatus->m_GearFadeCnt);
                ImGui::InputInt("Gear Max Anim Frame", &mediator->mBtEffect.pScreenStatus->m_GearMaxAnimFrame);
                ImGui::InputScalar("Gear Rand Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_GearRandCounter[0]);
                ImGui::InputScalar("Gear Rand Counter 2", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_GearRandCounter[1]);
                ImGui::InputScalar("Gear Rand Flag", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_GearRandFlag);
                ImGui::InputScalar("Gear", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_Gear);
                ImGui::InputScalar("Counters", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_Counter);
                ImGui::InputScalar("Money Appear Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_MoneyAppearCounter);
                ImGui::SliderFloat("Money Speed", &mediator->mBtEffect.pScreenStatus->m_MoneySpeed, 0.0f, 100.0f);
                ImGui::SliderFloat("Money Delta Speed", &mediator->mBtEffect.pScreenStatus->m_MoneyDeltaSpeed, -100.0f, 100.0f);
                ImGui::InputScalar("Delay Counters", ImGuiDataType_S16, mediator->mBtEffect.pScreenStatus->m_DelayCounter);
                ImGui::InputInt("Global Counter", &mediator->mBtEffect.pScreenStatus->m_GlobalCounter);
                ImGui::InputScalar("Counter Tiger Goal", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_Counter_TigerGoal);
                ImGui::InputScalar("Change Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_ChangeCounter);
                ImGui::InputScalar("Hp Anim Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_HpAnimCounter);
                ImGui::SliderFloat("Hp Scale", &mediator->mBtEffect.pScreenStatus->m_HpScale, 0.0f, 10.0f);
                ImGui::InputScalar("Bike In/Out Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_BikeInOutCounter);
                ImGui::Checkbox("Touring2 Natural Flag", &mediator->mBtEffect.pScreenStatus->m_Touring2NaturalFlag);
                ImGui::Checkbox("Battle2 Natural Flag", &mediator->mBtEffect.pScreenStatus->m_Battle2NaturalFlag);
                ImGui::Checkbox("Natural2 Battle Flag", &mediator->mBtEffect.pScreenStatus->m_Natural2BattleFlag);
                ImGui::InputInt("Load Mes Stat", &mediator->mBtEffect.pScreenStatus->m_LoadMesStat);
                ImGui::InputScalar("Log W", ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_Log_W);
                ImGui::InputScalar("Log Tenmetu Now Progress", ImGuiDataType_S8, mediator->mBtEffect.pScreenStatus->m_Log_tenmetuNowProgress);
                ImGui::InputScalar("Log Alpha", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_Log_alpha);
                ImGui::InputScalar("Combo Num", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_ComboNum[0]);
                ImGui::InputScalar("Combo Num 2", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_ComboNum[1]);
                ImGui::InputInt("Combo Counter", &mediator->mBtEffect.pScreenStatus->m_ComboCounter);
                ImGui::InputInt("Combo Delay Counter", &mediator->mBtEffect.pScreenStatus->m_ComboDelayCounter);
                ImGui::InputScalar("Slot Dan", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_SlotDan);
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(fmt::format("Slot Deme [{}]", i).c_str(), ImGuiDataType_S8, mediator->mBtEffect.pScreenStatus->m_SlotDeme[i]);
                    ImGui::InputScalar(fmt::format("Slot Deme Counter [{}]", i).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_SlotDemeCounter[i]);
                }
                ImGui::InputScalar("Slot Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_SlotCounter);
                ImGui::Checkbox("Play Zoro Sound", &mediator->mBtEffect.pScreenStatus->m_PlayZoroSound);
                ImGui::Checkbox("Leach Hazure", &mediator->mBtEffect.pScreenStatus->m_LeachHazure);
                ImGui::Checkbox("Play Cherry Zoro", &mediator->mBtEffect.pScreenStatus->m_PlayCherryZoro);
                ImGui::Checkbox("Play Leach Sound", &mediator->mBtEffect.pScreenStatus->m_PlayLeachSound);
                ImGui::InputInt("Slot Zorome Enshutu Counter", &mediator->mBtEffect.pScreenStatus->m_SlotZoromeEnshutuCounter);
                ImGui::Checkbox("Slot Zorome Enshutu Flag", &mediator->mBtEffect.pScreenStatus->m_SlotZoromeEnshutuFlag);
                ImGui::Checkbox("Slot SP Attack Start Flag", &mediator->mBtEffect.pScreenStatus->m_SlotSPAttackStartFlag);
                ImGui::InputScalar("Slot Zorome Alpha", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_SlotZoromeAlpha);
                ImGui::InputScalar("Slot Zorome Flash Alpha", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_SlotZoromeFlashAlpha);
                ImGui::InputScalar("Slot Zoro Back Alpha", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_SlotZoroBackAlpha);
                ImGui::InputScalar("Slot Zorome ZOROME", ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_SlotZoromeZOROME);

                ImGui::Checkbox("Zorome Sorotta", &mediator->mBtEffect.pScreenStatus->m_bZoromeSorotta);
                ImGui::InputInt("Slot Zoro Back Cnt", &mediator->mBtEffect.pScreenStatus->m_SlotZoroBackCnt);
                ImGui::InputInt("Slot Zoro Leave Wait Cnt", &mediator->mBtEffect.pScreenStatus->m_SlotZoroLeaveWaitCnt);
                ImGui::Checkbox("Slot Zorome Sorotta Start To End Flag", &mediator->mBtEffect.pScreenStatus->m_SlotZoromeStartToEndFlag);
                ImGui::Checkbox("BlueHeartFlag", &mediator->mBtEffect.pScreenStatus->m_BlueHeartFlag);
                bool lmode_seven = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 0);
                if (ImGui::Checkbox("lmode_seven", &lmode_seven)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 0, lmode_seven);
                bool lmode_melon = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 1);
                if (ImGui::Checkbox("lmode_melon", &lmode_melon)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 1, lmode_melon);
                bool lmode_bel = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 2);
                if (ImGui::Checkbox("lmode_bel", &lmode_bel)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 2, lmode_bel);
                bool lmode_cherry = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 3);
                if (ImGui::Checkbox("lmode_cherry", &lmode_cherry)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 3, lmode_cherry);
                bool lmode_ber = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 4);
                if (ImGui::Checkbox("lmode_ber", &lmode_ber)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 4, lmode_ber);
                bool lmode_hopperman = getBit(mediator->mBtEffect.pScreenStatus->flaglmode, 5);
                if (ImGui::Checkbox("lmode_hopperman", &lmode_hopperman)) setBit(mediator->mBtEffect.pScreenStatus->flaglmode, 5, lmode_hopperman);
                ImGui::InputInt("m_SMMesPosX", &mediator->mBtEffect.pScreenStatus->m_SMMesPosX);
                ImGui::InputInt("m_SMMesPosY", &mediator->mBtEffect.pScreenStatus->m_SMMesPosY);
                ImGui::InputScalar("m_TigerProc", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_TigerProc);
                ImGui::InputScalar("m_TigerCounter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_TigerCounter);
                ImGui::InputScalar("m_TigerTexId", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_TigerTexId);
                ImGui::InputScalar("m_FireTexId", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_FireTexId);
                ImGui::InputFloat("m_TigerXpos", &mediator->mBtEffect.pScreenStatus->m_TigerXpos);
                ImGui::InputInt("m_WalkStartTick", &mediator->mBtEffect.pScreenStatus->m_WalkStartTick);
                ImGui::InputScalar("m_TigerGoalLength", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_TigerGoalLength);
                ImGui::Checkbox("m_TigerSPReturnFlag", &mediator->mBtEffect.pScreenStatus->m_TigerSPReturnFlag);
                ImGui::Text("Union 1");
                bool drawbpber = getBit(mediator->mBtEffect.pScreenStatus->flag, 0);
                if (ImGui::Checkbox("drawbpber", &drawbpber)) setBit(mediator->mBtEffect.pScreenStatus->flag, 0, drawbpber);
                bool drawhber = getBit(mediator->mBtEffect.pScreenStatus->flag, 1);
                if (ImGui::Checkbox("drawhber", &drawhber)) setBit(mediator->mBtEffect.pScreenStatus->flag, 1, drawhber);
                bool drawbattery = getBit(mediator->mBtEffect.pScreenStatus->flag, 2);
                if (ImGui::Checkbox("drawbattery", &drawbattery)) setBit(mediator->mBtEffect.pScreenStatus->flag, 2, drawbattery);
                bool drawmoney = getBit(mediator->mBtEffect.pScreenStatus->flag, 3);
                if (ImGui::Checkbox("drawmoney", &drawmoney)) setBit(mediator->mBtEffect.pScreenStatus->flag, 3, drawmoney);
                bool drawtension = getBit(mediator->mBtEffect.pScreenStatus->flag, 4);
                if (ImGui::Checkbox("drawtension", &drawtension)) setBit(mediator->mBtEffect.pScreenStatus->flag, 4, drawtension);
                bool drawkamae = getBit(mediator->mBtEffect.pScreenStatus->flag, 5);
                if (ImGui::Checkbox("drawkamae", &drawkamae)) setBit(mediator->mBtEffect.pScreenStatus->flag, 5, drawkamae);
                bool drawspeedmeter = getBit(mediator->mBtEffect.pScreenStatus->flag, 6);
                if (ImGui::Checkbox("drawspeedmeter", &drawspeedmeter)) setBit(mediator->mBtEffect.pScreenStatus->flag, 6, drawspeedmeter);
                bool drawmap = getBit(mediator->mBtEffect.pScreenStatus->flag, 7);
                if (ImGui::Checkbox("drawmap", &drawmap)) setBit(mediator->mBtEffect.pScreenStatus->flag, 7, drawmap);
                bool drawsilvia = getBit(mediator->mBtEffect.pScreenStatus->flag, 8);
                if (ImGui::Checkbox("drawsilvia", &drawsilvia)) setBit(mediator->mBtEffect.pScreenStatus->flag, 8, drawsilvia);
                bool drawlmode = getBit(mediator->mBtEffect.pScreenStatus->flag, 9);
                if (ImGui::Checkbox("drawlmode", &drawlmode)) setBit(mediator->mBtEffect.pScreenStatus->flag, 9, drawlmode);
                bool drawkeyguide = getBit(mediator->mBtEffect.pScreenStatus->flag, 10);
                if (ImGui::Checkbox("drawkeyguide", &drawkeyguide)) setBit(mediator->mBtEffect.pScreenStatus->flag, 10, drawkeyguide);
                bool drawcheckpo = getBit(mediator->mBtEffect.pScreenStatus->flag, 11);
                if (ImGui::Checkbox("drawcheckpo", &drawcheckpo)) setBit(mediator->mBtEffect.pScreenStatus->flag, 11, drawcheckpo);
                ImGui::Text("Union 2");
                bool reqlmodedel = getBit(mediator->mBtEffect.pScreenStatus->flag2, 0);
                if (ImGui::Checkbox("reqlmodedel", &reqlmodedel)) setBit(mediator->mBtEffect.pScreenStatus->flag2, 0, reqlmodedel);
                bool tigerdirect = getBit(mediator->mBtEffect.pScreenStatus->flag2, 1);
                if (ImGui::Checkbox("tigerdirect", &tigerdirect)) setBit(mediator->mBtEffect.pScreenStatus->flag2, 1, tigerdirect);
                bool tigerfire = getBit(mediator->mBtEffect.pScreenStatus->flag2, 2);
                if (ImGui::Checkbox("tigerfire", &tigerfire)) setBit(mediator->mBtEffect.pScreenStatus->flag2, 2, tigerfire);
                ImGui::Text("Union 3");
                bool checkpo_Checkpoint = getBit(mediator->mBtEffect.pScreenStatus->flag4, 0);
                if (ImGui::Checkbox("checkpo_Checkpoint", &checkpo_Checkpoint)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 0, checkpo_Checkpoint);

                bool checkpo_Standby = getBit(mediator->mBtEffect.pScreenStatus->flag4, 1);
                if (ImGui::Checkbox("checkpo_Standby", &checkpo_Standby)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 1, checkpo_Standby);

                bool checkpo_Zenmetu = getBit(mediator->mBtEffect.pScreenStatus->flag4, 2);
                if (ImGui::Checkbox("checkpo_Zenmetu", &checkpo_Zenmetu)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 2, checkpo_Zenmetu);

                bool checkpo_CantRide = getBit(mediator->mBtEffect.pScreenStatus->flag4, 3);
                if (ImGui::Checkbox("checkpo_CantRide", &checkpo_CantRide)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 3, checkpo_CantRide);

                bool checkpo_Phone = getBit(mediator->mBtEffect.pScreenStatus->flag4, 4);
                if (ImGui::Checkbox("checkpo_Phone", &checkpo_Phone)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 4, checkpo_Phone);

                bool checkpo_NewMask = getBit(mediator->mBtEffect.pScreenStatus->flag4, 5);
                if (ImGui::Checkbox("checkpo_NewMask", &checkpo_NewMask)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 5, checkpo_NewMask);

                bool checkpo_Senrihin = getBit(mediator->mBtEffect.pScreenStatus->flag4, 6);
                if (ImGui::Checkbox("checkpo_Senrihin", &checkpo_Senrihin)) setBit(mediator->mBtEffect.pScreenStatus->flag4, 6, checkpo_Senrihin);

                ImGui::InputScalar("m_BatteyArram", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_BatteyArram);
                ImGui::InputInt("m_AlarmSound", &mediator->mBtEffect.pScreenStatus->m_AlarmSound);
                ImGui::InputInt("m_DrawType", &mediator->mBtEffect.pScreenStatus->m_DrawType);
                ImGui::InputInt("m_DrawTypeOld", &mediator->mBtEffect.pScreenStatus->m_DrawTypeOld);
                ImGui::Checkbox("m_ChargeMaxFlag", &mediator->mBtEffect.pScreenStatus->m_ChargeMaxFlag);
                ImGui::Checkbox("m_ChargeMaxFlag2", &mediator->mBtEffect.pScreenStatus->m_ChargeMaxFlag2);
                ImGui::Checkbox("m_ChargeSEStopFlag", &mediator->mBtEffect.pScreenStatus->m_ChargeSEStopFlag);
                ImGui::Checkbox("m_BusFightFlag", &mediator->mBtEffect.pScreenStatus->m_BusFightFlag);
                ImGui::InputInt("m_ChangeMinimapIndex", &mediator->mBtEffect.pScreenStatus->m_ChangeMinimapIndex);
                ImGui::Checkbox("m_Flag_BatterySE", &mediator->mBtEffect.pScreenStatus->m_Flag_BatterySE);
                ImGui::Checkbox("m_Flag_StatusDataLoad", &mediator->mBtEffect.pScreenStatus->m_Flag_StatusDataLoad);
                ImGui::Checkbox("m_Flag_StatusNewGame", &mediator->mBtEffect.pScreenStatus->m_Flag_StatusNewGame);
                ImGui::Checkbox("m_StatusProcFlag", &mediator->mBtEffect.pScreenStatus->m_StatusProcFlag);
                ImGui::Checkbox("m_fPC_BatteryCharge[0]", &mediator->mBtEffect.pScreenStatus->m_fPC_BatteryCharge[0]);
                ImGui::Checkbox("m_fPC_BatteryCharge[1]", &mediator->mBtEffect.pScreenStatus->m_fPC_BatteryCharge[1]);
            }
        }
        if (ImGui::CollapsingHeader("HrInGameMenu")) {
            if (mediator->mBtEffect.pScreenStatus->m_pInGameMenu) {
                uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mediator->mBtEffect.pScreenStatus->m_pInGameMenu->Padding_1054[0]);
                ImGui::Text("Base Address: 0x%08X", baseAddress);
                uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&mediator->mBtEffect.pScreenStatus->m_pInGameMenu->pad_3c78[0]);
                ImGui::Text("Target Address: 0x%08X", targetAddress);
                uintptr_t offsetDifference = targetAddress - baseAddress;
                ImGui::Text("Offset difference: 0x%08X", offsetDifference);

                ImGui::InputScalar("File Count", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_fileCount);
                ImGui::InputScalar("Handle", ImGuiDataType_U32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Handl);
                ImGui::InputScalar("DEMO Jimaku Handle", ImGuiDataType_U32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_DEMOJimaku_Handl);
                ImGui::InputScalar("Menu Mode", ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuMode);
                ImGui::InputScalar("Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Counter);
                ImGui::InputScalar("Start Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartCounter);
                ImGui::InputScalar("Start Anim Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartAnimCounter);
                ImGui::InputScalar("Line Anim Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineAnimCounter);
                ImGui::InputScalar("Title Line Anim Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_TitleLineAnimCounter);
                ImGui::SliderFloat("Info Scroll Counter", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_InfoScrollCounter, 0.0f, 100.0f);
                ImGui::InputScalar("Leave Menu Fade Count", ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_LeaveMenuFadeCnt);
                for (int i = 0; i < 6; i++) {
                    ImGui::InputScalar(("Select Menu Word Num " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_SelectMenuWordNum[i]);
                }
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Push Cross Btn Count " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_PushCrossBtnCnt[i]);
                }
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Pushing Cross Btn Flag " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_PusshingCrossBtnFlag[i]);
                }
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 4; j++) {
                        ImGui::InputScalar(("Pull Cross Btn Stat " + std::to_string(i) + "," + std::to_string(j)).c_str(), ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_PullCrossBtnStat[i][j]);
                    }
                }
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Pull Cross Btn Flag " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_PullCrossBtnFlag[i]);
                }
                for (int i = 0; i < 2; i++) {
                    ImGui::InputScalar(("Sec Temp " + std::to_string(i)).c_str(), ImGuiDataType_S64, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_SecTemp[i]);
                }
                ImGui::Checkbox("Start WB Flag", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartWBFlag);
                ImGui::Checkbox("Title WB Flag", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_TitleWBFlag);
                for (int i = 0; i < 4; i++) {
                    ImGui::Checkbox(("Load Yes No WB Flag " + std::to_string(i)).c_str(), &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_LoadYesNoWBFlag[i]);
                }
                ImGui::InputScalar("Line Mode", ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineMode);
                ImGui::InputScalar("Line SubJob Mode", ImGuiDataType_S32, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineSubJobMode);
                ImGui::Checkbox("Draw V Line Flag", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_DrawVLineFlag);
                ImGui::Checkbox("Draw H Line Flag", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_DrawHLineFlag);
                ImGui::Checkbox("Debug Draw Flag", &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_DebugDrawFlag);
                for (int i = 0; i < 2; i++) {
                    ImGui::InputScalar(("Select Menu " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_SelectMenu[i]);
                }
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Stat Wariai " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_StatWariai[i]);
                }
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Before Stat Wariai " + std::to_string(i)).c_str(), ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Before_StatWariai[i]);
                }
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Wariai Sa " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_WariaiSa[i]);
                }
                bool freturnmain = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 0);
                if (ImGui::Checkbox("Return Main", &freturnmain)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 0, freturnmain);
                bool fendmenu = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 1);
                if (ImGui::Checkbox("End Menu", &fendmenu)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 1, fendmenu);
                bool fdrawgraph = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 2);
                if (ImGui::Checkbox("Draw Graph", &fdrawgraph)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 2, fdrawgraph);
                bool fmainstartend = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 3);
                if (ImGui::Checkbox("Main Start End", &fmainstartend)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 3, fmainstartend);
                bool fmainmenu = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4);
                if (ImGui::Checkbox("Main Menu", &fmainmenu)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4, fmainmenu);
                bool fsubmissionend = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5);
                if (ImGui::Checkbox("Submission End", &fsubmissionend)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5, fsubmissionend);
                bool fFadeOutExit = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6);
                if (ImGui::Checkbox("Fade Out Exit", &fFadeOutExit)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6, fFadeOutExit);
                ImGui::InputScalar("Menu Info Counter", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuInfoCounter);
                ImGui::InputScalar("Menu Info Length", ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuInfoLenght);
                for (int i = 0; i < 7; i++) {
                    ImGui::InputScalar(("Menu Icon Shift X " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuIconShift_X[i]);
                }
                for (int i = 0; i < 6; i++) {
                    ImGui::InputScalar(("Menu Icon Shift Counter " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuIconShift_COUNTER[i]);
                }
                if (ImGui::TreeNode("Dot Circle XY Grid")) {
                    for (int i = 0; i < 23; i++) {
                        for (int j = 0; j < 23; j++) {
                            for (int k = 0; k < 2; k++) {
                                ImGui::InputScalar(("Grid[" + std::to_string(i) + "][" + std::to_string(j) + "][" + std::to_string(k) + "]").c_str(), ImGuiDataType_S8, &mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_DotCircleXY_Grid[i][j][k]);
                            }
                        }
                    }
                    ImGui::TreePop();
                }
                bool m_Jimaku = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimaku, 0);
                if (ImGui::Checkbox("Jimaku", &m_Jimaku)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 0, m_Jimaku);
                bool m_JimakuTMP = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_JimakuTMP, 1);
                if (ImGui::Checkbox("Jimaku TMP", &m_JimakuTMP)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 1, m_JimakuTMP);
                bool m_MotionFlag = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MotionFlag, 2);
                if (ImGui::Checkbox("Motion Flag", &m_MotionFlag)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 2, m_MotionFlag);
                bool m_MotionFlagTMP = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_MotionFlagTMP, 3);
                if (ImGui::Checkbox("Motion Flag TMP", &m_MotionFlagTMP)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 3, m_MotionFlagTMP);
                bool m_CamAxisXFlag = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4);
                if (ImGui::Checkbox("Cam Axis X Flag", &m_CamAxisXFlag)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4, m_CamAxisXFlag);
                bool m_CamAxisXFlagTMP = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5);
                if (ImGui::Checkbox("Cam Axis X Flag TMP", &m_CamAxisXFlagTMP)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5, m_CamAxisXFlagTMP);
                bool m_CamAxisYFlag = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6);
                if (ImGui::Checkbox("Cam Axis Y Flag", &m_CamAxisYFlag)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6, m_CamAxisYFlag);
                bool m_CamAxisYFlagTMP = getBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 7);
                if (ImGui::Checkbox("Cam Axis Y Flag TMP", &m_CamAxisYFlagTMP)) setBit(mediator->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 7, m_CamAxisYFlagTMP);
            }
        }

        if (mHRPc* player = nmh_sdk::get_local_player()) {
            if (ImGui::CollapsingHeader("mHRPc")) {
                uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->pad_00);
                ImGui::Text("Base Address: 0x%08X", baseAddress);
                uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mOperate);
                ImGui::Text("Target Address: 0x%08X", targetAddress);
                uintptr_t offsetDifference = targetAddress - baseAddress;
                ImGui::Text("Offset difference: 0x%08X", offsetDifference);

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
            }
            if (ImGui::CollapsingHeader("mHRPc mCharaStatus")) {
                uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->mCharaStatus.resNo);
                ImGui::Text("Base Address: 0x%08X", baseAddress);
                uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mCharaStatus.flag2);
                ImGui::Text("Target Address: 0x%08X", targetAddress);
                uintptr_t offsetDifference = targetAddress - baseAddress;
                ImGui::Text("Offset difference: 0x%08X", offsetDifference);

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
                bool visible = getBit(player->mCharaStatus.flag, 0);
                if (ImGui::Checkbox("visible", &visible)) setBit(player->mCharaStatus.flag, 0, visible);
                bool visibleWep = getBit(player->mCharaStatus.flag, 1);
                if (ImGui::Checkbox("visibleWep", &visibleWep)) setBit(player->mCharaStatus.flag, 1, visibleWep);
                bool visibleWepEffect = getBit(player->mCharaStatus.flag, 2);
                if (ImGui::Checkbox("visibleWepEffect", &visibleWepEffect)) setBit(player->mCharaStatus.flag, 2, visibleWepEffect);
                bool visibleDismember = getBit(player->mCharaStatus.flag, 3);
                if (ImGui::Checkbox("visibleDismember", &visibleDismember)) setBit(player->mCharaStatus.flag, 3, visibleDismember);
                bool visibleDist = getBit(player->mCharaStatus.flag, 4);
                if (ImGui::Checkbox("visibleDist", &visibleDist)) setBit(player->mCharaStatus.flag, 4, visibleDist);
                bool lockOn = getBit(player->mCharaStatus.flag, 5);
                if (ImGui::Checkbox("lockOn", &lockOn)) setBit(player->mCharaStatus.flag, 5, lockOn);
                bool hitChara = getBit(player->mCharaStatus.flag, 6);
                if (ImGui::Checkbox("hitChara", &hitChara)) setBit(player->mCharaStatus.flag, 6, hitChara);
                bool hitAttack = getBit(player->mCharaStatus.flag, 7);
                if (ImGui::Checkbox("hitAttack", &hitAttack)) setBit(player->mCharaStatus.flag, 7, hitAttack);
                bool hitStage = getBit(player->mCharaStatus.flag, 8);
                if (ImGui::Checkbox("hitStage", &hitStage)) setBit(player->mCharaStatus.flag, 8, hitStage);
                bool downShockWave = getBit(player->mCharaStatus.flag, 9);
                if (ImGui::Checkbox("downShockWave", &downShockWave)) setBit(player->mCharaStatus.flag, 9, downShockWave);
                bool hitStageDisEnable = getBit(player->mCharaStatus.flag, 10);
                if (ImGui::Checkbox("hitStageDisEnable", &hitStageDisEnable)) setBit(player->mCharaStatus.flag, 10, hitStageDisEnable);
                bool hitStageDisEnableReq = getBit(player->mCharaStatus.flag, 11);
                if (ImGui::Checkbox("hitStageDisEnableReq", &hitStageDisEnableReq)) setBit(player->mCharaStatus.flag, 11, hitStageDisEnableReq);
                bool hitOidashiDisEnable = getBit(player->mCharaStatus.flag, 12);
                if (ImGui::Checkbox("hitOidashiDisEnable", &hitOidashiDisEnable)) setBit(player->mCharaStatus.flag, 12, hitOidashiDisEnable);
                bool operateDisEnable = getBit(player->mCharaStatus.flag, 13);
                if (ImGui::Checkbox("operateDisEnable", &operateDisEnable)) setBit(player->mCharaStatus.flag, 13, operateDisEnable);
                bool drawPriority = getBit(player->mCharaStatus.flag, 14);
                if (ImGui::Checkbox("drawPriority", &drawPriority)) setBit(player->mCharaStatus.flag, 14, drawPriority);
                bool charaPause = getBit(player->mCharaStatus.flag, 15);
                if (ImGui::Checkbox("charaPause", &charaPause)) setBit(player->mCharaStatus.flag, 15, charaPause);
                bool initPlayMotion = getBit(player->mCharaStatus.flag, 16);
                if (ImGui::Checkbox("initPlayMotion", &initPlayMotion)) setBit(player->mCharaStatus.flag, 16, initPlayMotion);
                bool slowBlow = getBit(player->mCharaStatus.flag, 17);
                if (ImGui::Checkbox("slowBlow", &slowBlow)) setBit(player->mCharaStatus.flag, 17, slowBlow);
                bool dontCountKill = getBit(player->mCharaStatus.flag, 18);
                if (ImGui::Checkbox("dontCountKill", &dontCountKill)) setBit(player->mCharaStatus.flag, 18, dontCountKill);
                bool dontSubRepop = getBit(player->mCharaStatus.flag, 19);
                if (ImGui::Checkbox("dontSubRepop", &dontSubRepop)) setBit(player->mCharaStatus.flag, 19, dontSubRepop);
                bool dontStandUp = getBit(player->mCharaStatus.flag, 20);
                if (ImGui::Checkbox("dontStandUp", &dontStandUp)) setBit(player->mCharaStatus.flag, 20, dontStandUp);
                bool lowDamage = getBit(player->mCharaStatus.flag, 21);
                if (ImGui::Checkbox("lowDamage", &lowDamage)) setBit(player->mCharaStatus.flag, 21, lowDamage);
                bool dontHitStageWall = getBit(player->mCharaStatus.flag, 22);
                if (ImGui::Checkbox("dontHitStageWall", &dontHitStageWall)) setBit(player->mCharaStatus.flag, 22, dontHitStageWall);
                bool ikari = getBit(player->mCharaStatus.flag, 23);
                if (ImGui::Checkbox("ikari", &ikari)) setBit(player->mCharaStatus.flag, 23, ikari);
                bool motSpeedControl = getBit(player->mCharaStatus.flag, 24);
                if (ImGui::Checkbox("motSpeedControl", &motSpeedControl)) setBit(player->mCharaStatus.flag, 24, motSpeedControl);
                bool bossDeadFlag = getBit(player->mCharaStatus.flag, 25);
                if (ImGui::Checkbox("bossDeadFlag", &bossDeadFlag)) setBit(player->mCharaStatus.flag, 25, bossDeadFlag);
                bool successThrow = getBit(player->mCharaStatus.flag, 26);
                if (ImGui::Checkbox("successThrow", &successThrow)) setBit(player->mCharaStatus.flag, 26, successThrow);
                bool loseTsubazeri = getBit(player->mCharaStatus.flag, 27);
                if (ImGui::Checkbox("loseTsubazeri", &loseTsubazeri)) setBit(player->mCharaStatus.flag, 27, loseTsubazeri);
                bool chargeDamage = getBit(player->mCharaStatus.flag, 28);
                if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(player->mCharaStatus.flag, 28, chargeDamage);
                bool motionProcessDisEnable = getBit(player->mCharaStatus.flag, 29);
                if (ImGui::Checkbox("motionProcessDisEnable", &motionProcessDisEnable)) setBit(player->mCharaStatus.flag, 29, motionProcessDisEnable);
                bool downWaitStart = getBit(player->mCharaStatus.flag, 30);
                if (ImGui::Checkbox("downWaitStart", &downWaitStart)) setBit(player->mCharaStatus.flag, 30, downWaitStart);
                bool miniMapRender = getBit(player->mCharaStatus.flag, 31);
                if (ImGui::Checkbox("miniMapRender", &miniMapRender)) setBit(player->mCharaStatus.flag, 31, miniMapRender);
                bool normalClip = getBit(player->mCharaStatus.flag2, 0);
                if (ImGui::Checkbox("normalClip", &normalClip)) setBit(player->mCharaStatus.flag2, 0, normalClip);
                bool jpnDead = getBit(player->mCharaStatus.flag2, 1);
                if (ImGui::Checkbox("jpnDead", &jpnDead)) setBit(player->mCharaStatus.flag2, 1, jpnDead);
            }
            if (ImGui::CollapsingHeader("mHRPc mCharaStatus.dmgInfo")) {
                ImGui::SliderFloat("Damage", &player->mCharaStatus.dmgInfo.dmg, 0.0f, 100.0f);
                ImGui::InputInt("Damage Motion", &player->mCharaStatus.dmgInfo.dmgMot);
                ImGui::InputInt("Guard Motion", &player->mCharaStatus.dmgInfo.grdMot);
                ImGui::InputInt("Attack Motion", &player->mCharaStatus.dmgInfo.atkMot);
                ImGui::SliderFloat("Damage Direction", &player->mCharaStatus.dmgInfo.dmgDirec, -360.0f, 360.0f);
                ImGui::SliderFloat("Knockback Distance", &player->mCharaStatus.dmgInfo.nockBackDst, 0.0f, 100.0f);
                ImGui::SliderFloat("Upper Power", &player->mCharaStatus.dmgInfo.upperPow, 0.0f, 100.0f);
                ImGui::SliderFloat("Upper Position Y", &player->mCharaStatus.dmgInfo.upperPosY, -50.0f, 50.0f);
                ImGui::SliderFloat("Gravity", &player->mCharaStatus.dmgInfo.grav, 0.0f, 20.0f);
                ImGui::Checkbox("Upper", &player->mCharaStatus.dmgInfo.upper);
                ImGui::InputInt("Tick", &player->mCharaStatus.dmgInfo.tick);
                ImGui::SliderFloat("Air Blow Position X", &player->mCharaStatus.dmgInfo.m_AirBlowPos.x, -100.0f, 100.0f);
                ImGui::SliderFloat("Air Blow Position Y", &player->mCharaStatus.dmgInfo.m_AirBlowPos.y, -100.0f, 100.0f);
                ImGui::SliderFloat("Air Blow Position Z", &player->mCharaStatus.dmgInfo.m_AirBlowPos.z, -100.0f, 100.0f);
                ImGui::SliderFloat("Air Blow Power", &player->mCharaStatus.dmgInfo.m_AirBlowPower, 0.0f, 100.0f);
                ImGui::Checkbox("Air Flag", &player->mCharaStatus.dmgInfo.m_AirFlag);
                ImGui::SliderFloat("Gravity Acceleration", &player->mCharaStatus.dmgInfo.m_GravAccele, 0.0f, 20.0f);
                ImGui::InputScalar("Piyo Request", ImGuiDataType_S8, &player->mCharaStatus.dmgInfo.m_PiyoRequest);
                ImGui::SliderFloat("Stored Damage", &player->mCharaStatus.dmgInfo.storeDamage, 0.0f, 100.0f);
                ImGui::SliderFloat("Stored Damage Distance", &player->mCharaStatus.dmgInfo.storeDamageDst, 0.0f, 100.0f);
                ImGui::InputInt("Restore Damage Tick", &player->mCharaStatus.dmgInfo.restoreDamegeTick);
                ImGui::InputInt("Restore Damage Basic Tick", &player->mCharaStatus.dmgInfo.restoreDamegeBasicTick);
                ImGui::InputScalar("Bike Dead Request", ImGuiDataType_S8, &player->mCharaStatus.dmgInfo.m_BikeDeadRequest);
            }
            if (ImGui::CollapsingHeader("mHRPc stPcStatus")) {
                if (ImGui::CollapsingHeader("Weapon Info")) {
                    for (int i = 0; i < 16; i++) {
                        ImGui::Text("Weapon Info %d", i);
                        ImGui::InputInt(("Weapon ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].id);
                        ImGui::InputScalar(("Battery ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].battery);
                        ImGui::InputScalar(("Battery Max ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].batteryMax);
                        ImGui::SliderFloat(("Power ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].power, 0.0f, 100.0f);
                        ImGui::Checkbox(("Combo Extend ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].cmbExtend);
                    }
                }
    
                if (ImGui::CollapsingHeader("Locker Items")) {
                    for (int i = 0; i < 200; i++) {
                        ImGui::Text("Locker Item %d", i);
                        ImGui::InputInt(("Locker Item ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.locker[i].id);
                    }
                }

                if (ImGui::CollapsingHeader("Inventory Items")) {
                    for (int i = 0; i < 300; i++) {
                        ImGui::Text("Inventory Item %d", i);
                        ImGui::InputInt(("Item ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].id);
                        ImGui::InputScalar(("Item Quantity ##" + std::to_string(i)).c_str(), ImGuiDataType_S8, &player->mPcStatus.item[i].num);
                        ImGui::Checkbox(("Item Use ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].use);
                    }
                }

                ImGui::SliderFloat("Strength", &player->mPcStatus.strength, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &player->mPcStatus.stammina, 0.0f, 100.0f);
                ImGui::SliderFloat("Vitality", &player->mPcStatus.vitality, 0.0f, 100.0f);
                ImGui::InputInt("Battery Tick", &player->mPcStatus.batteryTick);
                ImGui::InputInt("Attack 360 Tick", &player->mPcStatus.attack360Tick);
                ImGui::SliderFloat("Heroes Meter", &player->mPcStatus.heroesMeter, 0.0f, 1000.0f);
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
                ImGui::Checkbox("Just Guard", &player->mPcStatus.justGuard);
                ImGui::Checkbox("Just Attack", &player->mPcStatus.justAttack);
                ImGui::InputInt("Just Input Tick", &player->mPcStatus.justInputTick);
                ImGui::InputInt("Just Atk Input Start Tick", &player->mPcStatus.justAtkInputStartTick);
                ImGui::InputInt("Just Atk Input End Tick", &player->mPcStatus.justAtkInputEndTick);
                ImGui::Checkbox("Success Input Finish", &player->mPcStatus.successInputFinish);
                ImGui::Checkbox("Finish Reset Slow", &player->mPcStatus.finishResetSlow);
                ImGui::Checkbox("Finish Slow", &player->mPcStatus.finishSlow);
                ImGui::Checkbox("Finish Se Play", &player->mPcStatus.finishSePlay);
                ImGui::InputInt("Finish Slow Tick", &player->mPcStatus.finishSlowTick);
                ImGui::Checkbox("Get 777", &player->mPcStatus.get777);
                ImGui::Checkbox("Play Shake Input", &player->mPcStatus.playShakeInput);
                ImGui::InputInt("Play Shake Input Wait", &player->mPcStatus.playShakeInputWait);
                ImGui::Checkbox("Ban Slot Cry", &player->mPcStatus.banSlotCry);
                ImGui::Checkbox("Ban Slot Hop", &player->mPcStatus.banSlotHop);
                ImGui::Checkbox("Ban Slot Bel", &player->mPcStatus.banSlotBel);
                ImGui::Checkbox("Ban Slot Bar", &player->mPcStatus.banSlotBar);
                ImGui::Checkbox("Ban Slot 777", &player->mPcStatus.banSlot777);
                ImGui::Checkbox("Weapon Stick", &player->mPcStatus.wepStick);
                ImGui::InputFloat3("Pad Y Offset", (float*)&player->mPcStatus.padYOffset);
                ImGui::SliderFloat("Rot Y Offset", &player->mPcStatus.rotYOffset, -360.0f, 360.0f);
                ImGui::Checkbox("Tsuba Se 0", &player->mPcStatus.tsubaSe0);
                ImGui::Checkbox("Tsuba Se 1", &player->mPcStatus.tsubaSe1);
                ImGui::Checkbox("Tsuba Se 2", &player->mPcStatus.tsubaSe2);
                ImGui::Checkbox("Dead Submission", &player->mPcStatus.deadSubmission);
                ImGui::Checkbox("Call Motion Process", &player->mPcStatus.callMotionProcess);
                ImGui::InputInt("Dead Boss Num", (int*)&player->mPcStatus.deadBossNum);
                ImGui::InputInt("Tel Ramble Tick", &player->mPcStatus.telRambleTick);
                ImGui::InputInt("Tel Ramble Wait Tick", &player->mPcStatus.telRambleWaitTick);
                ImGui::Checkbox("Tel Ramble", &player->mPcStatus.telRamble);
                ImGui::Checkbox("Cherry", &player->mPcStatus.cherry);
                ImGui::Checkbox("Hit Normal Dash Attack", &player->mPcStatus.hitNormalDashAttack);
                ImGui::Checkbox("Just Avoid", &player->mPcStatus.justAvoid);
                ImGui::Checkbox("Don't Sub Battery", &player->mPcStatus.dontSubBattey);
                ImGui::Checkbox("Auto Sub Battery", &player->mPcStatus.autoSubBattey);
                ImGui::Checkbox("Init Camera", &player->mPcStatus.initCamera);
                ImGui::Checkbox("Data Load", &player->mPcStatus.dataLoad);
                ImGui::Checkbox("Don't Restore Motion", &player->mPcStatus.dontRestoreMotion);
                ImGui::Checkbox("Bike Clash 2 Battou", &player->mPcStatus.bikeClash2battou);
                ImGui::Checkbox("Win Tsubazeri", &player->mPcStatus.winTsubazeri);
                ImGui::Checkbox("Just Guard Dis Enable", &player->mPcStatus.justGuardDisEnable);
                ImGui::Checkbox("Dash Atk Dis Enable", &player->mPcStatus.dashAtkDisEnable);
                ImGui::Checkbox("Just Escape Dis Enable", &player->mPcStatus.justEscapeDisEnable);
                ImGui::InputInt("Ikasama Slot", &player->mPcStatus.ikasamaSlot);
                ImGui::InputInt("Fire Man Tick", &player->mPcStatus.fireManTick);
            }
            if (ImGui::CollapsingHeader("mHRPc mPcSaveData")) {
                ImGui::SliderFloat("Max HP", &player->mPcSaveData.maxHp, 0.0f, 1000.0f);
                ImGui::SliderFloat("HP", &player->mPcSaveData.hp, 0.0f, player->mPcSaveData.maxHp);
                ImGui::SliderFloat("Strength", &player->mPcSaveData.strength, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &player->mPcSaveData.stammina, 0.0f, 100.0f);
                ImGui::SliderFloat("Vitality", &player->mPcSaveData.vitality, 0.0f, 100.0f);
                ImGui::InputInt("Money", &player->mPcSaveData.money);
                ImGui::InputInt("Dark Side Use Count", &player->mPcSaveData.darkSideUseNum);
                ImGui::InputScalar("Kill Combo", ImGuiDataType_S8, &player->mPcSaveData.killCmb);
                ImGui::SliderFloat("Bike Position X", &player->mPcSaveData.bikePos.x, -1000.0f, 1000.0f);
                ImGui::SliderFloat("Bike Position Y", &player->mPcSaveData.bikePos.y, -1000.0f, 1000.0f);
                ImGui::SliderFloat("Bike Position Z", &player->mPcSaveData.bikePos.z, -1000.0f, 1000.0f);
                ImGui::SliderFloat("Bike Rotation", &player->mPcSaveData.bikeRot, -360.0f, 360.0f);
                ImGui::Checkbox("Dig Tool", &player->mPcSaveData.digTool);
                for (int i = 0; i < 16; i++) {
                    ImGui::Checkbox(std::string("Skill Catch ").append(std::to_string(i)).c_str(), &player->mPcSaveData.skillCatch[i]);
                }
                for (int i = 0; i < 7; i++) {
                    ImGui::Checkbox(std::string("Skill K7 ").append(std::to_string(i)).c_str(), &player->mPcSaveData.skillK7[i]);
                }
                ImGui::InputScalar("Max Combo", ImGuiDataType_S8, &player->mPcSaveData.maxCmb);
                ImGui::SliderFloat("Received Damage", &player->mPcSaveData.receiveDmg, 0.0f, 1000.0f);
                ImGui::InputInt("Total Kill Count", &player->mPcSaveData.totalKillNum);
                ImGui::InputScalar("Bomb Stock Count", ImGuiDataType_S8, &player->mPcSaveData.bomStockNum);
                ImGui::Checkbox("Lost Bike", &player->mPcSaveData.lostBike);
                ImGui::Checkbox("Display Map", &player->mPcSaveData.dispMap);
                ImGui::InputScalar("Bike Sight", ImGuiDataType_S8, &player->mPcSaveData.bikeSight);
                ImGui::InputInt("Clear Count", (int*)&player->mPcSaveData.clearNum);
            }

            if (ImGui::CollapsingHeader("mpLockOnNpc")) {
                if (player->mpLockOnNpc) {
                    ImGui::InputFloat("mDistFromPc", &player->mpLockOnNpc->mDistFromPc);
                    ImGui::InputFloat("mDirecYFromPc", &player->mpLockOnNpc->mDirecYFromPc);
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
                    bool visible = getBit(player->mpLockOnNpc->mStatus.flag, 0);
                    if (ImGui::Checkbox("visible", &visible)) setBit(player->mpLockOnNpc->mStatus.flag, 0, visible);
                    bool visibleWep = getBit(player->mpLockOnNpc->mStatus.flag, 1);
                    if (ImGui::Checkbox("visibleWep", &visibleWep)) setBit(player->mpLockOnNpc->mStatus.flag, 1, visibleWep);
                    bool visibleWepEffect = getBit(player->mpLockOnNpc->mStatus.flag, 2);
                    if (ImGui::Checkbox("visibleWepEffect", &visibleWepEffect)) setBit(player->mpLockOnNpc->mStatus.flag, 2, visibleWepEffect);
                    bool visibleDismember = getBit(player->mpLockOnNpc->mStatus.flag, 3);
                    if (ImGui::Checkbox("visibleDismember", &visibleDismember)) setBit(player->mpLockOnNpc->mStatus.flag, 3, visibleDismember);
                    bool visibleDist = getBit(player->mpLockOnNpc->mStatus.flag, 4);
                    if (ImGui::Checkbox("visibleDist", &visibleDist)) setBit(player->mpLockOnNpc->mStatus.flag, 4, visibleDist);
                    bool lockOn = getBit(player->mpLockOnNpc->mStatus.flag, 5);
                    if (ImGui::Checkbox("lockOn", &lockOn)) setBit(player->mpLockOnNpc->mStatus.flag, 5, lockOn);
                    bool hitChara = getBit(player->mpLockOnNpc->mStatus.flag, 6);
                    if (ImGui::Checkbox("hitChara", &hitChara)) setBit(player->mpLockOnNpc->mStatus.flag, 6, hitChara);
                    bool hitAttack = getBit(player->mpLockOnNpc->mStatus.flag, 7);
                    if (ImGui::Checkbox("hitAttack", &hitAttack)) setBit(player->mpLockOnNpc->mStatus.flag, 7, hitAttack);
                    bool hitStage = getBit(player->mpLockOnNpc->mStatus.flag, 8);
                    if (ImGui::Checkbox("hitStage", &hitStage)) setBit(player->mpLockOnNpc->mStatus.flag, 8, hitStage);
                    bool downShockWave = getBit(player->mpLockOnNpc->mStatus.flag, 9);
                    if (ImGui::Checkbox("downShockWave", &downShockWave)) setBit(player->mpLockOnNpc->mStatus.flag, 9, downShockWave);
                    bool hitStageDisEnable = getBit(player->mpLockOnNpc->mStatus.flag, 10);
                    if (ImGui::Checkbox("hitStageDisEnable", &hitStageDisEnable)) setBit(player->mpLockOnNpc->mStatus.flag, 10, hitStageDisEnable);
                    bool hitStageDisEnableReq = getBit(player->mpLockOnNpc->mStatus.flag, 11);
                    if (ImGui::Checkbox("hitStageDisEnableReq", &hitStageDisEnableReq)) setBit(player->mpLockOnNpc->mStatus.flag, 11, hitStageDisEnableReq);
                    bool hitOidashiDisEnable = getBit(player->mpLockOnNpc->mStatus.flag, 12);
                    if (ImGui::Checkbox("hitOidashiDisEnable", &hitOidashiDisEnable)) setBit(player->mpLockOnNpc->mStatus.flag, 12, hitOidashiDisEnable);
                    bool operateDisEnable = getBit(player->mpLockOnNpc->mStatus.flag, 13);
                    if (ImGui::Checkbox("operateDisEnable", &operateDisEnable)) setBit(player->mpLockOnNpc->mStatus.flag, 13, operateDisEnable);
                    bool drawPriority = getBit(player->mpLockOnNpc->mStatus.flag, 14);
                    if (ImGui::Checkbox("drawPriority", &drawPriority)) setBit(player->mpLockOnNpc->mStatus.flag, 14, drawPriority);
                    bool charaPause = getBit(player->mpLockOnNpc->mStatus.flag, 15);
                    if (ImGui::Checkbox("charaPause", &charaPause)) setBit(player->mpLockOnNpc->mStatus.flag, 15, charaPause);
                    bool initPlayMotion = getBit(player->mpLockOnNpc->mStatus.flag, 16);
                    if (ImGui::Checkbox("initPlayMotion", &initPlayMotion)) setBit(player->mpLockOnNpc->mStatus.flag, 16, initPlayMotion);
                    bool slowBlow = getBit(player->mpLockOnNpc->mStatus.flag, 17);
                    if (ImGui::Checkbox("slowBlow", &slowBlow)) setBit(player->mpLockOnNpc->mStatus.flag, 17, slowBlow);
                    bool dontCountKill = getBit(player->mpLockOnNpc->mStatus.flag, 18);
                    if (ImGui::Checkbox("dontCountKill", &dontCountKill)) setBit(player->mpLockOnNpc->mStatus.flag, 18, dontCountKill);
                    bool dontSubRepop = getBit(player->mpLockOnNpc->mStatus.flag, 19);
                    if (ImGui::Checkbox("dontSubRepop", &dontSubRepop)) setBit(player->mpLockOnNpc->mStatus.flag, 19, dontSubRepop);
                    bool dontStandUp = getBit(player->mpLockOnNpc->mStatus.flag, 20);
                    if (ImGui::Checkbox("dontStandUp", &dontStandUp)) setBit(player->mpLockOnNpc->mStatus.flag, 20, dontStandUp);
                    bool lowDamage = getBit(player->mpLockOnNpc->mStatus.flag, 21);
                    if (ImGui::Checkbox("lowDamage", &lowDamage)) setBit(player->mpLockOnNpc->mStatus.flag, 21, lowDamage);
                    bool dontHitStageWall = getBit(player->mpLockOnNpc->mStatus.flag, 22);
                    if (ImGui::Checkbox("dontHitStageWall", &dontHitStageWall)) setBit(player->mpLockOnNpc->mStatus.flag, 22, dontHitStageWall);
                    bool ikari = getBit(player->mpLockOnNpc->mStatus.flag, 23);
                    if (ImGui::Checkbox("ikari", &ikari)) setBit(player->mpLockOnNpc->mStatus.flag, 23, ikari);
                    bool motSpeedControl = getBit(player->mpLockOnNpc->mStatus.flag, 24);
                    if (ImGui::Checkbox("motSpeedControl", &motSpeedControl)) setBit(player->mpLockOnNpc->mStatus.flag, 24, motSpeedControl);
                    bool bossDeadFlag = getBit(player->mpLockOnNpc->mStatus.flag, 25);
                    if (ImGui::Checkbox("bossDeadFlag", &bossDeadFlag)) setBit(player->mpLockOnNpc->mStatus.flag, 25, bossDeadFlag);
                    bool successThrow = getBit(player->mpLockOnNpc->mStatus.flag, 26);
                    if (ImGui::Checkbox("successThrow", &successThrow)) setBit(player->mpLockOnNpc->mStatus.flag, 26, successThrow);
                    bool loseTsubazeri = getBit(player->mpLockOnNpc->mStatus.flag, 27);
                    if (ImGui::Checkbox("loseTsubazeri", &loseTsubazeri)) setBit(player->mpLockOnNpc->mStatus.flag, 27, loseTsubazeri);
                    bool chargeDamage = getBit(player->mpLockOnNpc->mStatus.flag, 28);
                    if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(player->mpLockOnNpc->mStatus.flag, 28, chargeDamage);
                    bool motionProcessDisEnable = getBit(player->mpLockOnNpc->mStatus.flag, 29);
                    if (ImGui::Checkbox("motionProcessDisEnable", &motionProcessDisEnable)) setBit(player->mpLockOnNpc->mStatus.flag, 29, motionProcessDisEnable);
                    bool downWaitStart = getBit(player->mpLockOnNpc->mStatus.flag, 30);
                    if (ImGui::Checkbox("downWaitStart", &downWaitStart)) setBit(player->mpLockOnNpc->mStatus.flag, 30, downWaitStart);
                    bool miniMapRender = getBit(player->mpLockOnNpc->mStatus.flag, 31);
                    if (ImGui::Checkbox("miniMapRender", &miniMapRender)) setBit(player->mpLockOnNpc->mStatus.flag, 31, miniMapRender);
                    bool normalClip = getBit(player->mpLockOnNpc->mStatus.flag2, 0);
                    if (ImGui::Checkbox("normalClip", &normalClip)) setBit(player->mpLockOnNpc->mStatus.flag2, 0, normalClip);
                    bool jpnDead = getBit(player->mpLockOnNpc->mStatus.flag2, 1);
                    if (ImGui::Checkbox("jpnDead", &jpnDead)) setBit(player->mpLockOnNpc->mStatus.flag2, 1, jpnDead);
                }
            }
            if (ImGui::CollapsingHeader("mpLockOnNpc mStatus.dmgInfo")) {
                if (player->mpLockOnNpc) {
                    ImGui::SliderFloat("Damage", &player->mpLockOnNpc->mStatus.dmgInfo.dmg, 0.0f, 100.0f);
                    ImGui::InputInt("Damage Motion", &player->mpLockOnNpc->mStatus.dmgInfo.dmgMot);
                    ImGui::InputInt("Guard Motion", &player->mpLockOnNpc->mStatus.dmgInfo.grdMot);
                    ImGui::InputInt("Attack Motion", &player->mpLockOnNpc->mStatus.dmgInfo.atkMot);
                    ImGui::SliderFloat("Damage Direction", &player->mpLockOnNpc->mStatus.dmgInfo.dmgDirec, -360.0f, 360.0f);
                    ImGui::SliderFloat("Knockback Distance", &player->mpLockOnNpc->mStatus.dmgInfo.nockBackDst, 0.0f, 100.0f);
                    ImGui::SliderFloat("Upper Power", &player->mpLockOnNpc->mStatus.dmgInfo.upperPow, 0.0f, 100.0f);
                    ImGui::SliderFloat("Upper Position Y", &player->mpLockOnNpc->mStatus.dmgInfo.upperPosY, -50.0f, 50.0f);
                    ImGui::SliderFloat("Gravity", &player->mpLockOnNpc->mStatus.dmgInfo.grav, 0.0f, 20.0f);
                    ImGui::Checkbox("Upper", &player->mpLockOnNpc->mStatus.dmgInfo.upper);
                    ImGui::InputInt("Tick", &player->mpLockOnNpc->mStatus.dmgInfo.tick);
                    ImGui::SliderFloat("Air Blow Position X", &player->mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.x, -100.0f, 100.0f);
                    ImGui::SliderFloat("Air Blow Position Y", &player->mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.y, -100.0f, 100.0f);
                    ImGui::SliderFloat("Air Blow Position Z", &player->mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.z, -100.0f, 100.0f);
                    ImGui::SliderFloat("Air Blow Power", &player->mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPower, 0.0f, 100.0f);
                    ImGui::Checkbox("Air Flag", &player->mpLockOnNpc->mStatus.dmgInfo.m_AirFlag);
                    ImGui::SliderFloat("Gravity Acceleration", &player->mpLockOnNpc->mStatus.dmgInfo.m_GravAccele, 0.0f, 20.0f);
                    ImGui::InputScalar("Piyo Request", ImGuiDataType_S8, &player->mpLockOnNpc->mStatus.dmgInfo.m_PiyoRequest);
                    ImGui::SliderFloat("Stored Damage", &player->mpLockOnNpc->mStatus.dmgInfo.storeDamage, 0.0f, 100.0f);
                    ImGui::SliderFloat("Stored Damage Distance", &player->mpLockOnNpc->mStatus.dmgInfo.storeDamageDst, 0.0f, 100.0f);
                    ImGui::InputInt("Restore Damage Tick", &player->mpLockOnNpc->mStatus.dmgInfo.restoreDamegeTick);
                    ImGui::InputInt("Restore Damage Basic Tick", &player->mpLockOnNpc->mStatus.dmgInfo.restoreDamegeBasicTick);
                    ImGui::InputScalar("Bike Dead Request", ImGuiDataType_S8, &player->mpLockOnNpc->mStatus.dmgInfo.m_BikeDeadRequest);
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
