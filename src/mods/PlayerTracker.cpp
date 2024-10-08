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
    if (ImGui::CollapsingHeader("HrGameTask")) {
        if (HrGameTask* hrGameTask = nmh_sdk::get_HrGameTask()) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&hrGameTask->mAllClearHikitugi);
            // ImGui::Text("Base Address: 0x%08X", baseAddress);
            // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&hrGameTask->m_pSubTask); 
            // ImGui::Text("Target Address: 0x%08X", targetAddress);
            // uintptr_t offsetDifference = targetAddress - baseAddress;
            // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

            ImGui::InputScalar("mLoadSts", ImGuiDataType_S32, &hrGameTask->mLoadSts);
            ImGui::Text("mpRankingUp: %p", hrGameTask->mpRankingUp);
            ImGui::InputInt("mDebugScenarioID", &hrGameTask->mDebugScenarioID);
            ImGui::InputScalar("mDebugScenarioInputKeta", ImGuiDataType_S8, &hrGameTask->mDebugScenarioInputKeta);
            ImGui::InputScalar("mDebugScenarioMenu", ImGuiDataType_S8, &hrGameTask->mDebugScenarioMenu);
            ImGui::InputScalar("mDebugLogoSkip", ImGuiDataType_S8, &hrGameTask->mDebugLogoSkip);
            ImGui::Text("m_pDebug: %p", hrGameTask->m_pDebug);
            ImGui::InputInt("mDebugSubMissionID", &hrGameTask->mDebugSubMissionID);
            ImGui::InputScalar("mDebugSubMissionInputKeta", ImGuiDataType_S8, &hrGameTask->mDebugSubMissionInputKeta);
            ImGui::Text("mLoading: %p", &hrGameTask->mLoading);
            ImGui::Text("mpBossResult: %p", hrGameTask->mpBossResult);
            ImGui::Text("mStaffRoll: %p", hrGameTask->mStaffRoll);
            ImGui::Text("mStaffRoll2: %p", hrGameTask->mStaffRoll2);
            ImGui::Text("mpShooting: %p", hrGameTask->mpShooting);
            ImGui::InputInt("mLastLogoSndGroupID", &hrGameTask->mLastLogoSndGroupID);
            ImGui::InputInt("mLastSaveSndGroupID", &hrGameTask->mLastSaveSndGroupID);
            ImGui::InputInt("TerminateCnt", &hrGameTask->TerminateCnt);
            ImGui::Text("Union");
            ImGui::InputScalar("m_TotalStartPlayTime", ImGuiDataType_S64, &hrGameTask->m_TotalStartPlayTime);
            ImGui::Checkbox("mNewGameRequestForTitle", &hrGameTask->mNewGameRequestForTitle);
            ImGui::Checkbox("mAllClearHikitugi", &hrGameTask->mAllClearHikitugi);
            ImGui::Checkbox("mInitStageLoad", &hrGameTask->mInitStageLoad);
            ImGui::Checkbox("mOpeningMovieRequest", &hrGameTask->mOpeningMovieRequest);
            ImGui::Checkbox("mNewGameRequest", &hrGameTask->mNewGameRequest);
            ImGui::Checkbox("mGotoTitleRequest", &hrGameTask->mGotoTitleRequest);
            ImGui::InputInt("mGameExit", &hrGameTask->mGameExit);
            ImGui::InputInt("mGameExitProcess", &hrGameTask->mGameExitProcess);
            ImGui::Checkbox("mAllClearDataLoadFlag", &hrGameTask->mAllClearDataLoadFlag);
            ImGui::Checkbox("mMainScenarioRun", &hrGameTask->mMainScenarioRun);
            ImGui::Checkbox("mSubMissionRun", &hrGameTask->mSubMissionRun);
            ImGui::InputInt("mSetSubMissionID", &hrGameTask->mSetSubMissionID);
            ImGui::InputInt("mGameLevel", &hrGameTask->mGameLevel);
            ImGui::Checkbox("mHomeButtonDisEnable", &hrGameTask->mHomeButtonDisEnable);
            ImGui::InputScalar("m_Process_id", ImGuiDataType_S32, &hrGameTask->m_Process_id);
            ImGui::InputScalar("m_Pro_Sts", ImGuiDataType_S32, &hrGameTask->m_Pro_Sts);
            ImGui::Text("mp_SaveData: %p", hrGameTask->mp_SaveData);
            ImGui::Text("mp_CheckPoint: %p", hrGameTask->mp_CheckPoint);
            ImGui::Text("mp_HikitugiSaveData: %p", hrGameTask->mp_HikitugiSaveData);
            for (int i = 0; i < 60; ++i)
            {
                ImGui::InputInt(("mSmRunflag[" + std::to_string(i) + "]").c_str(), &hrGameTask->m_SmRunflag[i]);
            }
            ImGui::Separator();
            ImGui::Text("mpGameScrTask: %p", hrGameTask->mpGameScrTask);
            ImGui::Text("m_pDemo: %p", hrGameTask->m_pDemo);
            ImGui::Text("mpGameOver: %p", hrGameTask->mpGameOver);
            ImGui::Checkbox("mBossResultRequest", &hrGameTask->mBossResultRequest);
            ImGui::Checkbox("mRankingUpRequest", &hrGameTask->mRankingUpRequest);
            ImGui::InputInt("mStageChangeFadeType", &hrGameTask->mStageChangeFadeType);
            ImGui::Checkbox("mStaffRollRequest", &hrGameTask->mStaffRollRequest);
            ImGui::Checkbox("mStaffRoll2Request", &hrGameTask->mStaffRoll2Request);
            ImGui::Checkbox("mShootingRequest", &hrGameTask->mShootingRequest);
            ImGui::Checkbox("mLastLogoRequest", &hrGameTask->mLastLogoRequest);
            ImGui::Checkbox("mLastSaveRequest", &hrGameTask->mLastSaveRequest);
            ImGui::InputInt("mLastSaveProcess", &hrGameTask->mLastSaveProcess);
            ImGui::Text("mLastSaveResource: %p", &hrGameTask->mLastSaveResource);
            ImGui::Text("mpLastSave: %p", hrGameTask->mpLastSave);
            ImGui::InputInt("mLastLogoProcess", &hrGameTask->mLastLogoProcess);
            for (int i = 0; i < 4; ++i)
            {
                ImGui::Text("mLastLogoTex[%d]: %p", i, &hrGameTask->mLastLogoTex[i]);
            }
            ImGui::Separator();
            ImGui::InputInt("mLastLogoTexDispNum", &hrGameTask->mLastLogoTexDispNum);
            ImGui::Text("mLastLogoAlpha: %p", &hrGameTask->mLastLogoAlpha);
            ImGui::InputInt("mLastLogoDispTick", &hrGameTask->mLastLogoDispTick);
            ImGui::Text("mLastLogoMoveX: %p", &hrGameTask->mLastLogoMoveX);
            ImGui::InputInt("mGameSaveProcess", &hrGameTask->mGameSaveProcess);
            ImGui::Text("mGameSaveResource: %p", &hrGameTask->mGameSaveResource);
            ImGui::Text("mpGameSave: %p", hrGameTask->mpGameSave);
            ImGui::InputInt("mGameSaveSetUpWait", &hrGameTask->mGameSaveSetUpWait);
            ImGui::InputInt("mGameSaveMode", &hrGameTask->mGameSaveMode);
            ImGui::Text("mpGameSavePcNode: %p", hrGameTask->mpGameSavePcNode);
            ImGui::Checkbox("mGameSaveCopyLightFadeOutReq", &hrGameTask->mGameSaveCopyLightFadeOutReq);
            ImGui::InputInt("mLoadingWallType", &hrGameTask->mLoadingWallType);
            ImGui::InputInt("mBossResultAfterDemoNo", &hrGameTask->mBossResultAfterDemoNo);
            ImGui::Checkbox("m_STG_Feed_f", &hrGameTask->m_STG_Feed_f);
            ImGui::Text("m_pSubTask: %p", hrGameTask->m_pSubTask);
        }
    }

    if (ImGui::CollapsingHeader("m_pHrMenuTask")) {
        if (HrMenuTask* HrMenuTask = nmh_sdk::get_HrMenuTask()) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&HrMenuTask->m_pHrMenuTask);
            // ImGui::Text("Base Address: 0x%08X", baseAddress);
            // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&HrMenuTask->m_pHsMenu);
            // ImGui::Text("Target Address: 0x%08X", targetAddress);
            // uintptr_t offsetDifference = targetAddress - baseAddress;
            // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

            ImGui::Text("m_pHrMenuTask: %p", HrMenuTask->m_pHrMenuTask);
            ImGui::Text("m_pHsMenu: %p", HrMenuTask->m_pHsMenu);
            ImGui::Text("m_pdEmployment: %p", HrMenuTask->m_pdEmployment);
            ImGui::Text("m_pdK_Entertainment: %p", HrMenuTask->m_pdK_Entertainment);
            ImGui::Text("m_Weapon: %p", &HrMenuTask->m_Weapon);
            ImGui::InputInt("m_Dag_Counter", &HrMenuTask->m_Dag_Counter);
            ImGui::Checkbox("m_Dag_LockFlag", &HrMenuTask->m_Dag_LockFlag);
        }
    }   
    if (ImGui::CollapsingHeader("HrMenuTask->hsSourceBase")) {
        if (hsSourceBase* hsMenu = nmh_sdk::get_HrMenuTask()->m_pHsMenu) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&HrMenuTask->m_pHsMenu->Padding_1229[0]);
            // ImGui::Text("Base Address: 0x%08X", baseAddress);
            // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&HrMenuTask->m_pHsMenu->mouseDeltaTracker);
            // ImGui::Text("Target Address: 0x%08X", targetAddress);
            // uintptr_t offsetDifference = targetAddress - baseAddress;
            // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

            ImGui::Checkbox("m_Access_f", &hsMenu->m_Access_f);
            ImGui::InputScalar("m_Access_No", ImGuiDataType_U32, &hsMenu->m_Access_No);
            ImGui::InputScalar("m_Task", ImGuiDataType_S32, &hsMenu->m_Task);
            ImGui::InputScalar("m_hsLoadState", ImGuiDataType_S32, &hsMenu->m_hsLoadState);
            ImGui::InputScalar("m_hsfileState", ImGuiDataType_S32, &hsMenu->m_hsfileState);
            ImGui::InputScalar("m_SaveSound_No", ImGuiDataType_U16, &hsMenu->m_SaveSound_No);
            ImGui::InputFloat("m_StartMotion_frame_Num", &hsMenu->m_StartMotion_frame_Num);
            ImGui::InputScalar("m_Model_Alpha_Type", ImGuiDataType_S32, &hsMenu->m_Model_Alpha_Type);
            ImGui::InputInt("m_Model_Alpha_Num", &hsMenu->m_Model_Alpha_Num);
            ImGui::InputInt("m_Alpha_1F_Num", &hsMenu->m_Alpha_1F_Num);
            ImGui::InputInt("m_Info_x", &hsMenu->m_Info_x);
            ImGui::InputFloat("m_InfoLength", &hsMenu->m_InfoLength);
            ImGui::InputFloat("m_Most_ItemName_Length", &hsMenu->m_Most_ItemName_Length);
            for (int i = 0; i < 2; ++i)
            {
                ImGui::InputInt(("m_CurNum[" + std::to_string(i) + "]").c_str(), &hsMenu->m_CurNum[i]);
            }
            ImGui::Separator();
            ImGui::Text("m_HsMoveBase: %p", &hsMenu->m_HsMoveBase);
            ImGui::InputScalar("m_Cnt", ImGuiDataType_U32, &hsMenu->m_Cnt);
            ImGui::InputScalar("m_Cnt2", ImGuiDataType_U32, &hsMenu->m_Cnt2);
            ImGui::Text("m_pGmf: %p", hsMenu->m_pGmf);
            ImGui::Checkbox("updatesMouseSelf", &hsMenu->updatesMouseSelf);
            ImGui::InputScalar("chosenMouseInteraction", ImGuiDataType_S32, &hsMenu->chosenMouseInteraction);
            for (int i = 0; i < 12; ++i)
            {
                ImGui::Text("MouseInteractions[%d]: %p", i, hsMenu->MouseInteractions[i]);
            }
            for (int i = 0; i < 12; ++i)
            {
                ImGui::Text("SubMenuInteractions[%d]: %p", i, hsMenu->SubMenuInteractions[i]);
            }
            for (int i = 0; i < 12; ++i)
            {
                ImGui::InputInt(("MouseInteractionCounts[" + std::to_string(i) + "]").c_str(), (int*)&hsMenu->MouseInteractionCounts[i]);
            }
            for (int i = 0; i < 12; ++i)
            {
                ImGui::InputInt(("SubMenuMouseInteractionCounts[" + std::to_string(i) + "]").c_str(), (int*)&hsMenu->SubMenuMouseInteractionCounts[i]);
            }
            ImGui::Text("mi_Cat: %p", &hsMenu->mi_Cat);
            for (int i = 0; i < 11; ++i)
            {
                ImGui::Text("mi_Closet[%d]: %p", i, &hsMenu->mi_Closet[i]);
            }
            for (int i = 0; i < 8; ++i)
            {
                ImGui::Text("mi_Drawer[%d]: %p", i, &hsMenu->mi_Drawer[i]);
            }
            for (int i = 0; i < 10; ++i)
            {
                ImGui::Text("mi_TV[%d]: %p", i, &hsMenu->mi_TV[i]);
            }
            for (int i = 0; i < 7; ++i)
            {
                ImGui::Text("mi_Naomi[%d]: %p", i, &hsMenu->mi_Naomi[i]);
            }
            for (int i = 0; i < 6; ++i)
            {
                ImGui::Text("mi_Gym[%d]: %p", i, &hsMenu->mi_Gym[i]);
            }
            for (int i = 0; i < 6; ++i)
            {
                ImGui::Text("mi_VideoStore[%d]: %p", i, &hsMenu->mi_VideoStore[i]);
            }
            for (int i = 0; i < 6; ++i)
            {
                ImGui::Text("mi_Lovikov[%d]: %p", i, &hsMenu->mi_Lovikov[i]);
            }
            for (int i = 0; i < 15; ++i)
            {
                ImGui::Text("mi_Closet_Sub[%d]: %p", i, &hsMenu->mi_Closet_Sub[i]);
            }
            for (int i = 0; i < 7; ++i)
            {
                ImGui::Text("mi_Naomi_Sub[%d]: %p", i, &hsMenu->mi_Naomi_Sub[i]);
            }
            for (int i = 0; i < 3; ++i)
            {
                ImGui::Text("mi_Gym_Sub[%d]: %p", i, &hsMenu->mi_Gym_Sub[i]);
            }
            for (int i = 0; i < 4; ++i)
            {
                ImGui::Text("mi_VideoStore_Sub[%d]: %p", i, &hsMenu->mi_VideoStore_Sub[i]);
            }
            for (int i = 0; i < 7; ++i)
            {
                ImGui::Text("mi_Lovikov_Sub[%d]: %p", i, &hsMenu->mi_Lovikov_Sub[i]);
            }
            ImGui::Text("mouseDeltaTracker: %p", &hsMenu->mouseDeltaTracker);
        }
    }

    if (CBgCtrl* CBgCtrl = nmh_sdk::get_CBgCtrl()) {
        if (ImGui::CollapsingHeader("CBgCtrl")) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&CBgCtrl->Padding_1372);
            // ImGui::Text("Base Address: 0x%08X", baseAddress);
            // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&CBgCtrl->mReleaseWaitCount);
            // ImGui::Text("Target Address: 0x%08X", targetAddress);
            // uintptr_t offsetDifference = targetAddress - baseAddress;
            // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

            bool disableUpdate = getBit(CBgCtrl->m_Flag, 0);
            if (ImGui::Checkbox("m_DISABLEUPDATE", &disableUpdate)) setBit(CBgCtrl->m_Flag, 0, disableUpdate);
            bool visible = getBit(CBgCtrl->m_Flag, 1);
            if (ImGui::Checkbox("m_VISIBLE", &visible)) setBit(CBgCtrl->m_Flag, 1, visible);
            bool npcrslRead = getBit(CBgCtrl->m_Flag, 2);
            if (ImGui::Checkbox("m_NPCRSLREAD", &npcrslRead)) setBit(CBgCtrl->m_Flag, 2, npcrslRead);
            ImGui::InputInt("m_BgScrHndl", (int*)&CBgCtrl->m_BgScrHndl);
            ImGui::InputInt("m_MesHndl", (int*)&CBgCtrl->m_MesHndl);
            ImGui::InputInt("m_SpecialStgIdx", (int*)&CBgCtrl->m_SpecialStgIdx);
            ImGui::InputInt("m_CurrentBgX", &CBgCtrl->m_CurrentBgX);
            ImGui::InputInt("m_CurrentBgY", &CBgCtrl->m_CurrentBgY);
            ImGui::Text("mpAirGmf Address: 0x%08X", (uintptr_t)CBgCtrl->mpAirGmf);
            ImGui::Text("mpReceiveShadowGmf Address: 0x%08X", (uintptr_t)CBgCtrl->mpReceiveShadowGmf);
            ImGui::Text("mp_StageDraw Address: 0x%08X", (uintptr_t)CBgCtrl->mp_StageDraw);
            ImGui::InputFloat3("m_CamPos", (float*)&CBgCtrl->m_CamPos);
            ImGui::Checkbox("m_ResultTex", &CBgCtrl->m_ResultTex);
            ImGui::InputInt("m_ResultTexAlpha", (int*)&CBgCtrl->m_ResultTexAlpha);
            ImGui::InputInt2("m_ResultFontColor", (int*)&CBgCtrl->m_ResultFontColor);
            ImGui::InputInt("m_ResultFontTenmetuNum", &CBgCtrl->m_ResultFontTenmetuNum);
            ImGui::InputInt("m_ResultFontDispTick", &CBgCtrl->m_ResultFontDispTick);
            ImGui::InputInt("m_ResultFontDispMaxTick", &CBgCtrl->m_ResultFontDispMaxTick);
            ImGui::InputInt("m_ResultCmnTick", &CBgCtrl->m_ResultCmnTick);
            ImGui::Text("m_ResultPauseTileAlpha Address: 0x%08X", (uintptr_t)&CBgCtrl->m_ResultPauseTileAlpha);
            ImGui::InputScalar("m_DarkSideModeColor", ImGuiDataType_S16, &CBgCtrl->m_DarkSideModeColor);
            ImGui::Checkbox("m_DarkStageReq", &CBgCtrl->m_DarkStageReq);
            ImGui::Text("m_Stage_Info Address: 0x%08X", (uintptr_t)CBgCtrl->m_Stage_Info);
            ImGui::InputText("m_NowStageName", CBgCtrl->m_NowStageName, IM_ARRAYSIZE(CBgCtrl->m_NowStageName));
            ImGui::InputText("m_RequestStageName", CBgCtrl->m_RequestStageName, IM_ARRAYSIZE(CBgCtrl->m_RequestStageName));
            ImGui::InputText("m_NpcRslName", CBgCtrl->m_NpcRslName, IM_ARRAYSIZE(CBgCtrl->m_NpcRslName));
            ImGui::Text("m_pRsl Address: 0x%08X", (uintptr_t)CBgCtrl->m_pRsl);
            ImGui::Text("m_pRslData Address: 0x%08X", (uintptr_t)CBgCtrl->m_pRslData);
            ImGui::Checkbox("m_IsPressData", &CBgCtrl->m_IsPressData);
            ImGui::Text("mpNpcRsl Address: 0x%08X", (uintptr_t)CBgCtrl->mpNpcRsl);
            ImGui::Text("mpNpcRslData Address: 0x%08X", (uintptr_t)CBgCtrl->mpNpcRslData);
            ImGui::Checkbox("m_NpcVisible", &CBgCtrl->m_NpcVisible);
            ImGui::InputInt("mPathCounter", &CBgCtrl->mPathCounter);
            ImGui::Text("mp_CPathManager Address: 0x%08X", (uintptr_t)CBgCtrl->mp_CPathManager);
            ImGui::Text("mp_MPathManager Address: 0x%08X", (uintptr_t)CBgCtrl->mp_MPathManager);
            ImGui::Text("mp_NpcManager Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcManager);
            ImGui::Text("mp_NpcPlayer Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcPlayer);
            ImGui::Text("mp_NpcBike Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcBike);
            ImGui::Text("mp_NpcCat Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcCat);
            ImGui::Text("mp_NpcObjMan Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcObjMan);
            ImGui::Text("mp_NpcPhone Address: 0x%08X", (uintptr_t)CBgCtrl->mp_NpcPhone);
            ImGui::InputInt("m_CallBikeState", &CBgCtrl->m_CallBikeState);
            ImGui::InputInt("m_CallBikeCnt", &CBgCtrl->m_CallBikeCnt);
            ImGui::InputFloat("m_CallBikeDist", &CBgCtrl->m_CallBikeDist);
            ImGui::InputInt("m_CallBikeSndHandle", &CBgCtrl->m_CallBikeSndHandle);
            ImGui::InputInt("m_CallBikeSndHandle2", &CBgCtrl->m_CallBikeSndHandle2);
            ImGui::InputInt("m_CallBikeTailEndNum", &CBgCtrl->m_CallBikeTailEndNum);
            ImGui::Checkbox("m_BikeHitCarFlag", &CBgCtrl->m_BikeHitCarFlag);
            ImGui::Checkbox("m_RankingUpGo", &CBgCtrl->m_RankingUpGo);
            ImGui::Checkbox("m_BossResultGo", &CBgCtrl->m_BossResultGo);
            ImGui::InputInt("m_BossResultAfterDemoNo", &CBgCtrl->m_BossResultAfterDemoNo);
            ImGui::Checkbox("m_StaffRoll1Go", &CBgCtrl->m_StaffRoll1Go);
            ImGui::Checkbox("m_StaffRoll2Go", &CBgCtrl->m_StaffRoll2Go);
            ImGui::Checkbox("m_ShootingGo", &CBgCtrl->m_ShootingGo);
            ImGui::InputInt("m_TutoResultProc", &CBgCtrl->m_TutoResultProc);
            ImGui::InputInt("m_TutoResultType", &CBgCtrl->m_TutoResultType);
            ImGui::InputInt("m_FogType", (int*)&CBgCtrl->m_FogType);
            ImGui::ColorEdit4("m_FogColor", (float*)&CBgCtrl->m_FogColor);
            ImGui::InputFloat("m_FogNear", &CBgCtrl->m_FogNear);
            ImGui::InputFloat("m_FogFar", &CBgCtrl->m_FogFar);
            ImGui::InputInt("m_DenCount", (int*)&CBgCtrl->m_DenCount);
            ImGui::Checkbox("m_EventPaintData", &CBgCtrl->m_EventPaintData);
            ImGui::InputInt("m_CamStatus", &CBgCtrl->m_CamStatus);
            ImGui::InputInt("m_CamStatusBak", &CBgCtrl->m_CamStatusBak);
            ImGui::InputFloat("m_CamFov", &CBgCtrl->m_CamFov);
            ImGui::InputInt("m_CamWait", &CBgCtrl->m_CamWait);
            ImGui::Checkbox("m_Charge", &CBgCtrl->m_Charge);
            ImGui::InputFloat3("m_CamHOKAN_c", (float*)&CBgCtrl->m_CamHOKAN_c);
            ImGui::InputFloat3("m_CamHOKAN_t", (float*)&CBgCtrl->m_CamHOKAN_t);
            ImGui::InputFloat("m_CamHOKANcount", &CBgCtrl->m_CamHOKANcount);
            ImGui::InputInt("m_LocID", &CBgCtrl->m_LocID);
            ImGui::InputInt("m_CamModeBack", (int*)&CBgCtrl->m_CamModeBack);
            ImGui::InputInt("m_ScriptArg", &CBgCtrl->m_ScriptArg);
            ImGui::InputInt("m_ScriptMainScenarioID", &CBgCtrl->m_ScriptMainScenarioID);
            ImGui::InputInt("m_ScriptSubMissionID", &CBgCtrl->m_ScriptSubMissionID);
            ImGui::InputInt("m_SubMissionSndGroupID", &CBgCtrl->m_SubMissionSndGroupID);
            ImGui::InputInt("m_StageSndGroupID", &CBgCtrl->m_StageSndGroupID);
            ImGui::InputInt("m_StageSubSndGroupID", &CBgCtrl->m_StageSubSndGroupID);
            ImGui::InputInt("m_StageGayaID", &CBgCtrl->m_StageGayaID);
            ImGui::Checkbox("m_bChanged", &CBgCtrl->m_bChanged);
            ImGui::InputInt("m_Status", (int*)&CBgCtrl->m_Status);
            ImGui::InputInt("m_LDstatus", (int*)&CBgCtrl->m_LDstatus);
            ImGui::Checkbox("m_RequestChangeStage", &CBgCtrl->m_RequestChangeStage);
            ImGui::Checkbox("m_DisEnableLoadingEffect", &CBgCtrl->m_DisEnableLoadingEffect);
            ImGui::Checkbox("m_BgRenderOK", &CBgCtrl->m_BgRenderOK);
            ImGui::InputInt("mReleaseWaitCount", &CBgCtrl->mReleaseWaitCount);
        }
    }
    if (mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle()) {
        if (ImGui::CollapsingHeader("mHRBattle")) {
            ImGui::InputInt("mBtlInitProc", (int*)&mHRBattle->mBtlInitProc);
            ImGui::InputInt("mBtlFrameProc", (int*)&mHRBattle->mBtlFrameProc);
            ImGui::Text("struct mCamStatus Address: 0x%08X", ((uintptr_t)(&mHRBattle->mCamStatus)));
            ImGui::InputInt("mTotalNpcNum", &mHRBattle->mTotalNpcNum); 
            ImGui::InputInt("mTotalKillNum", &mHRBattle->mTotalKillNum);
            ImGui::InputInt("mKillNum", &mHRBattle->mKillNum);
            ImGui::InputFloat("mNpcAttackRate", &mHRBattle->mNpcAttackRate);
            bool battlePause = getBit(mHRBattle->mFlag, 0);
            if (ImGui::Checkbox("battlePause", &battlePause)) setBit(mHRBattle->mFlag, 0, battlePause);
            bool tutoRun = getBit(mHRBattle->tutoRun, 1);
            if (ImGui::Checkbox("tutoRun", &tutoRun)) setBit(mHRBattle->mFlag, 1, tutoRun);
            bool dispStatusDisEnable = getBit(mHRBattle->dispStatusDisEnable, 2);
            if (ImGui::Checkbox("dispStatusDisEnable", &dispStatusDisEnable)) setBit(mHRBattle->mFlag, 2, dispStatusDisEnable);
            bool chargeDamage = getBit(mHRBattle->chargeDamage, 3);
            if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(mHRBattle->mFlag, 3, chargeDamage);
        }
        if (ImGui::CollapsingHeader("mHRBattle stBtEffect")) {
            ImGui::InputInt("slowMotTotalTick", &mHRBattle->mBtEffect.slowMotTotalTick);
            ImGui::InputInt("slowMotTick", &mHRBattle->mBtEffect.slowMotTick);
            ImGui::InputInt("bulletSlowTick", &mHRBattle->mBtEffect.bulletSlowTick);
            ImGui::InputInt("bossBreakSlowTick", &mHRBattle->mBtEffect.bossBreakSlowTick);
        }
        if (ImGui::CollapsingHeader("mHRBattle HrScreenStatus")) {
            if (mHRBattle->mBtEffect.pScreenStatus) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mHRBattle->mBtEffect.pScreenStatus->Padding_0[0]);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&mHRBattle->mBtEffect.pScreenStatus->m_StatusProcFlag);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

                ImGui::InputInt3("Sound IDs", mHRBattle->mBtEffect.pScreenStatus->m_SoundID);
                ImGui::InputInt("SprLch Sound ID", &mHRBattle->mBtEffect.pScreenStatus->m_SprLchSoundID);
                ImGui::InputScalar("Herose", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_Herose);
                ImGui::SliderFloat("HP", &mHRBattle->mBtEffect.pScreenStatus->m_HP, 0.0f, 100.0f); // Assuming ranges
                ImGui::SliderFloat("Battery", &mHRBattle->mBtEffect.pScreenStatus->m_Battery, 0.0f, 1000.0f);
                ImGui::SliderFloat("Stamina", &mHRBattle->mBtEffect.pScreenStatus->m_Stamina, 0.0f, 1.0f);
                ImGui::SliderFloat2("Tension", mHRBattle->mBtEffect.pScreenStatus->m_Tension, 0.0f, 1000.0f);
                ImGui::InputInt2("Money", mHRBattle->mBtEffect.pScreenStatus->m_Money);
                ImGui::InputInt("Money Comp Sound Wait", &mHRBattle->mBtEffect.pScreenStatus->m_MoneyCompSoundWait);
                ImGui::InputInt("Flag Anim Counter", &mHRBattle->mBtEffect.pScreenStatus->m_FlagAnimCounter);
                ImGui::InputInt("Gear Anim Counter", &mHRBattle->mBtEffect.pScreenStatus->m_GearAnimCounter);
                ImGui::InputInt2("Gear Anim Frame", mHRBattle->mBtEffect.pScreenStatus->m_GearAnimFrame);
                ImGui::InputInt("Gear Fade Count", &mHRBattle->mBtEffect.pScreenStatus->m_GearFadeCnt);
                ImGui::InputInt("Gear Max Anim Frame", &mHRBattle->mBtEffect.pScreenStatus->m_GearMaxAnimFrame);
                ImGui::InputScalar("Gear Rand Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_GearRandCounter[0]);
                ImGui::InputScalar("Gear Rand Counter 2", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_GearRandCounter[1]);
                ImGui::InputScalar("Gear Rand Flag", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_GearRandFlag);
                ImGui::InputScalar("Gear", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_Gear);
                ImGui::InputScalar("Counters", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_Counter);
                ImGui::InputScalar("Money Appear Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_MoneyAppearCounter);
                ImGui::SliderFloat("Money Speed", &mHRBattle->mBtEffect.pScreenStatus->m_MoneySpeed, 0.0f, 100.0f);
                ImGui::SliderFloat("Money Delta Speed", &mHRBattle->mBtEffect.pScreenStatus->m_MoneyDeltaSpeed, -100.0f, 100.0f);
                ImGui::InputScalar("Delay Counters", ImGuiDataType_S16, mHRBattle->mBtEffect.pScreenStatus->m_DelayCounter);
                ImGui::InputInt("Global Counter", &mHRBattle->mBtEffect.pScreenStatus->m_GlobalCounter);
                ImGui::InputScalar("Counter Tiger Goal", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_Counter_TigerGoal);
                ImGui::InputScalar("Change Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_ChangeCounter);
                ImGui::InputScalar("Hp Anim Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_HpAnimCounter);
                ImGui::SliderFloat("Hp Scale", &mHRBattle->mBtEffect.pScreenStatus->m_HpScale, 0.0f, 10.0f);
                ImGui::InputScalar("Bike In/Out Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_BikeInOutCounter);
                ImGui::Checkbox("Touring2 Natural Flag", &mHRBattle->mBtEffect.pScreenStatus->m_Touring2NaturalFlag);
                ImGui::Checkbox("Battle2 Natural Flag", &mHRBattle->mBtEffect.pScreenStatus->m_Battle2NaturalFlag);
                ImGui::Checkbox("Natural2 Battle Flag", &mHRBattle->mBtEffect.pScreenStatus->m_Natural2BattleFlag);
                ImGui::InputInt("Load Mes Stat", &mHRBattle->mBtEffect.pScreenStatus->m_LoadMesStat);
                ImGui::InputScalar("Log W", ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_Log_W);
                ImGui::InputScalar("Log Tenmetu Now Progress", ImGuiDataType_S8, mHRBattle->mBtEffect.pScreenStatus->m_Log_tenmetuNowProgress);
                ImGui::InputScalar("Log Alpha", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_Log_alpha);
                ImGui::InputScalar("Combo Num", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_ComboNum[0]);
                ImGui::InputScalar("Combo Num 2", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_ComboNum[1]);
                ImGui::InputInt("Combo Counter", &mHRBattle->mBtEffect.pScreenStatus->m_ComboCounter);
                ImGui::InputInt("Combo Delay Counter", &mHRBattle->mBtEffect.pScreenStatus->m_ComboDelayCounter);
                ImGui::InputScalar("Slot Dan", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_SlotDan);
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(fmt::format("Slot Deme [{}]", i).c_str(), ImGuiDataType_S8, mHRBattle->mBtEffect.pScreenStatus->m_SlotDeme[i]);
                    ImGui::InputScalar(fmt::format("Slot Deme Counter [{}]", i).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_SlotDemeCounter[i]);
                }
                ImGui::Separator();
                ImGui::InputScalar("Slot Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_SlotCounter);
                ImGui::Checkbox("Play Zoro Sound", &mHRBattle->mBtEffect.pScreenStatus->m_PlayZoroSound);
                ImGui::Checkbox("Leach Hazure", &mHRBattle->mBtEffect.pScreenStatus->m_LeachHazure);
                ImGui::Checkbox("Play Cherry Zoro", &mHRBattle->mBtEffect.pScreenStatus->m_PlayCherryZoro);
                ImGui::Checkbox("Play Leach Sound", &mHRBattle->mBtEffect.pScreenStatus->m_PlayLeachSound);
                ImGui::InputInt("Slot Zorome Enshutu Counter", &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeEnshutuCounter);
                ImGui::Checkbox("Slot Zorome Enshutu Flag", &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeEnshutuFlag);
                ImGui::Checkbox("Slot SP Attack Start Flag", &mHRBattle->mBtEffect.pScreenStatus->m_SlotSPAttackStartFlag);
                ImGui::InputScalar("Slot Zorome Alpha", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeAlpha);
                ImGui::InputScalar("Slot Zorome Flash Alpha", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeFlashAlpha);
                ImGui::InputScalar("Slot Zoro Back Alpha", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoroBackAlpha);
                ImGui::InputScalar("Slot Zorome ZOROME", ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeZOROME);
                ImGui::Checkbox("Zorome Sorotta", &mHRBattle->mBtEffect.pScreenStatus->m_bZoromeSorotta);
                ImGui::InputInt("Slot Zoro Back Cnt", &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoroBackCnt);
                ImGui::InputInt("Slot Zoro Leave Wait Cnt", &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoroLeaveWaitCnt);
                ImGui::Checkbox("Slot Zorome Sorotta Start To End Flag", &mHRBattle->mBtEffect.pScreenStatus->m_SlotZoromeStartToEndFlag);
                ImGui::Checkbox("BlueHeartFlag", &mHRBattle->mBtEffect.pScreenStatus->m_BlueHeartFlag);
                bool lmode_seven = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 0);
                if (ImGui::Checkbox("lmode_seven", &lmode_seven)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 0, lmode_seven);
                bool lmode_melon = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 1);
                if (ImGui::Checkbox("lmode_melon", &lmode_melon)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 1, lmode_melon);
                bool lmode_bel = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 2);
                if (ImGui::Checkbox("lmode_bel", &lmode_bel)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 2, lmode_bel);
                bool lmode_cherry = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 3);
                if (ImGui::Checkbox("lmode_cherry", &lmode_cherry)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 3, lmode_cherry);
                bool lmode_ber = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 4);
                if (ImGui::Checkbox("lmode_ber", &lmode_ber)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 4, lmode_ber);
                bool lmode_hopperman = getBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 5);
                if (ImGui::Checkbox("lmode_hopperman", &lmode_hopperman)) setBit(mHRBattle->mBtEffect.pScreenStatus->flaglmode, 5, lmode_hopperman);
                ImGui::InputInt("m_SMMesPosX", &mHRBattle->mBtEffect.pScreenStatus->m_SMMesPosX);
                ImGui::InputInt("m_SMMesPosY", &mHRBattle->mBtEffect.pScreenStatus->m_SMMesPosY);
                ImGui::InputScalar("m_TigerProc", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_TigerProc);
                ImGui::InputScalar("m_TigerCounter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_TigerCounter);
                ImGui::InputScalar("m_TigerTexId", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_TigerTexId);
                ImGui::InputScalar("m_FireTexId", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_FireTexId);
                ImGui::InputFloat("m_TigerXpos", &mHRBattle->mBtEffect.pScreenStatus->m_TigerXpos);
                ImGui::InputInt("m_WalkStartTick", &mHRBattle->mBtEffect.pScreenStatus->m_WalkStartTick);
                ImGui::InputScalar("m_TigerGoalLength", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_TigerGoalLength);
                ImGui::Checkbox("m_TigerSPReturnFlag", &mHRBattle->mBtEffect.pScreenStatus->m_TigerSPReturnFlag);
                ImGui::Text("Union 1");
                bool drawbpber = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 0);
                if (ImGui::Checkbox("drawbpber", &drawbpber)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 0, drawbpber);
                bool drawhber = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 1);
                if (ImGui::Checkbox("drawhber", &drawhber)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 1, drawhber);
                bool drawbattery = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 2);
                if (ImGui::Checkbox("drawbattery", &drawbattery)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 2, drawbattery);
                bool drawmoney = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 3);
                if (ImGui::Checkbox("drawmoney", &drawmoney)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 3, drawmoney);
                bool drawtension = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 4);
                if (ImGui::Checkbox("drawtension", &drawtension)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 4, drawtension);
                bool drawkamae = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 5);
                if (ImGui::Checkbox("drawkamae", &drawkamae)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 5, drawkamae);
                bool drawspeedmeter = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 6);
                if (ImGui::Checkbox("drawspeedmeter", &drawspeedmeter)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 6, drawspeedmeter);
                bool drawmap = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 7);
                if (ImGui::Checkbox("drawmap", &drawmap)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 7, drawmap);
                bool drawsilvia = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 8);
                if (ImGui::Checkbox("drawsilvia", &drawsilvia)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 8, drawsilvia);
                bool drawlmode = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 9);
                if (ImGui::Checkbox("drawlmode", &drawlmode)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 9, drawlmode);
                bool drawkeyguide = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 10);
                if (ImGui::Checkbox("drawkeyguide", &drawkeyguide)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 10, drawkeyguide);
                bool drawcheckpo = getBit(mHRBattle->mBtEffect.pScreenStatus->flag, 11);
                if (ImGui::Checkbox("drawcheckpo", &drawcheckpo)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag, 11, drawcheckpo);
                ImGui::Text("Union 2");
                bool reqlmodedel = getBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 0);
                if (ImGui::Checkbox("reqlmodedel", &reqlmodedel)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 0, reqlmodedel);
                bool tigerdirect = getBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 1);
                if (ImGui::Checkbox("tigerdirect", &tigerdirect)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 1, tigerdirect);
                bool tigerfire = getBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 2);
                if (ImGui::Checkbox("tigerfire", &tigerfire)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag2, 2, tigerfire);
                ImGui::Text("Union 3");
                bool checkpo_Checkpoint = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 0);
                if (ImGui::Checkbox("checkpo_Checkpoint", &checkpo_Checkpoint)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 0, checkpo_Checkpoint);
                bool checkpo_Standby = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 1);
                if (ImGui::Checkbox("checkpo_Standby", &checkpo_Standby)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 1, checkpo_Standby);
                bool checkpo_Zenmetu = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 2);
                if (ImGui::Checkbox("checkpo_Zenmetu", &checkpo_Zenmetu)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 2, checkpo_Zenmetu);
                bool checkpo_CantRide = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 3);
                if (ImGui::Checkbox("checkpo_CantRide", &checkpo_CantRide)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 3, checkpo_CantRide);
                bool checkpo_Phone = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 4);
                if (ImGui::Checkbox("checkpo_Phone", &checkpo_Phone)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 4, checkpo_Phone);
                bool checkpo_NewMask = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 5);
                if (ImGui::Checkbox("checkpo_NewMask", &checkpo_NewMask)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 5, checkpo_NewMask);
                bool checkpo_Senrihin = getBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 6);
                if (ImGui::Checkbox("checkpo_Senrihin", &checkpo_Senrihin)) setBit(mHRBattle->mBtEffect.pScreenStatus->flag4, 6, checkpo_Senrihin);
                ImGui::InputScalar("m_BatteyArram", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_BatteyArram);
                ImGui::InputInt("m_AlarmSound", &mHRBattle->mBtEffect.pScreenStatus->m_AlarmSound);
                ImGui::InputInt("m_DrawType", &mHRBattle->mBtEffect.pScreenStatus->m_DrawType);
                ImGui::InputInt("m_DrawTypeOld", &mHRBattle->mBtEffect.pScreenStatus->m_DrawTypeOld);
                ImGui::Checkbox("m_ChargeMaxFlag", &mHRBattle->mBtEffect.pScreenStatus->m_ChargeMaxFlag);
                ImGui::Checkbox("m_ChargeMaxFlag2", &mHRBattle->mBtEffect.pScreenStatus->m_ChargeMaxFlag2);
                ImGui::Checkbox("m_ChargeSEStopFlag", &mHRBattle->mBtEffect.pScreenStatus->m_ChargeSEStopFlag);
                ImGui::Checkbox("m_BusFightFlag", &mHRBattle->mBtEffect.pScreenStatus->m_BusFightFlag);
                ImGui::InputInt("m_ChangeMinimapIndex", &mHRBattle->mBtEffect.pScreenStatus->m_ChangeMinimapIndex);
                ImGui::Checkbox("m_Flag_BatterySE", &mHRBattle->mBtEffect.pScreenStatus->m_Flag_BatterySE);
                ImGui::Checkbox("m_Flag_StatusDataLoad", &mHRBattle->mBtEffect.pScreenStatus->m_Flag_StatusDataLoad);
                ImGui::Checkbox("m_Flag_StatusNewGame", &mHRBattle->mBtEffect.pScreenStatus->m_Flag_StatusNewGame);
                ImGui::Checkbox("m_StatusProcFlag", &mHRBattle->mBtEffect.pScreenStatus->m_StatusProcFlag);
                ImGui::Checkbox("m_fPC_BatteryCharge[0]", &mHRBattle->mBtEffect.pScreenStatus->m_fPC_BatteryCharge[0]);
                ImGui::Checkbox("m_fPC_BatteryCharge[1]", &mHRBattle->mBtEffect.pScreenStatus->m_fPC_BatteryCharge[1]);
            }
        }
        if (ImGui::CollapsingHeader("mHRBattle HrScreenStatus HrInGameMenu")) {
            if (mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->Padding_1054[0]);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->pad_3c78[0]);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

                ImGui::InputScalar("File Count", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_fileCount);
                ImGui::InputScalar("Handle", ImGuiDataType_U32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Handl);
                ImGui::InputScalar("DEMO Jimaku Handle", ImGuiDataType_U32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DEMOJimaku_Handl);
                ImGui::InputScalar("Menu Mode", ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuMode);
                ImGui::InputScalar("Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Counter);
                ImGui::InputScalar("Start Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartCounter);
                ImGui::InputScalar("Start Anim Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartAnimCounter);
                ImGui::InputScalar("Line Anim Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineAnimCounter);
                ImGui::InputScalar("Title Line Anim Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_TitleLineAnimCounter);
                ImGui::SliderFloat("Info Scroll Counter", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_InfoScrollCounter, 0.0f, 100.0f);
                ImGui::InputScalar("Leave Menu Fade Count", ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_LeaveMenuFadeCnt);
                for (int i = 0; i < 6; i++) {
                    ImGui::InputScalar(("Select Menu Word Num " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_SelectMenuWordNum[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Push Cross Btn Count " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_PushCrossBtnCnt[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Pushing Cross Btn Flag " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_PusshingCrossBtnFlag[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 4; j++) {
                        ImGui::InputScalar(("Pull Cross Btn Stat " + std::to_string(i) + "," + std::to_string(j)).c_str(), ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_PullCrossBtnStat[i][j]);
                    }
                }
                ImGui::Separator();
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Pull Cross Btn Flag " + std::to_string(i)).c_str(), ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_PullCrossBtnFlag[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 2; i++) {
                    ImGui::InputScalar(("Sec Temp " + std::to_string(i)).c_str(), ImGuiDataType_S64, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_SecTemp[i]);
                }
                ImGui::Separator();
                ImGui::Checkbox("Start WB Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_StartWBFlag);
                ImGui::Checkbox("Title WB Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_TitleWBFlag);
                for (int i = 0; i < 4; i++) {
                    ImGui::Checkbox(("Load Yes No WB Flag " + std::to_string(i)).c_str(), &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_LoadYesNoWBFlag[i]);
                }
                ImGui::Separator();
                ImGui::InputScalar("Line Mode", ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineMode);
                ImGui::InputScalar("Line SubJob Mode", ImGuiDataType_S32, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_LineSubJobMode);
                ImGui::Checkbox("Draw V Line Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DrawVLineFlag);
                ImGui::Checkbox("Draw H Line Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DrawHLineFlag);
                ImGui::Checkbox("Debug Draw Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DebugDrawFlag);
                for (int i = 0; i < 2; i++) {
                    ImGui::InputScalar(("Select Menu " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_SelectMenu[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Stat Wariai " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_StatWariai[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Before Stat Wariai " + std::to_string(i)).c_str(), ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Before_StatWariai[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 3; i++) {
                    ImGui::InputScalar(("Wariai Sa " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_WariaiSa[i]);
                }
                ImGui::Separator();
                bool freturnmain = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 0);
                if (ImGui::Checkbox("Return Main", &freturnmain)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 0, freturnmain);
                bool fendmenu = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 1);
                if (ImGui::Checkbox("End Menu", &fendmenu)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 1, fendmenu);
                bool fdrawgraph = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 2);
                if (ImGui::Checkbox("Draw Graph", &fdrawgraph)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 2, fdrawgraph);
                bool fmainstartend = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 3);
                if (ImGui::Checkbox("Main Start End", &fmainstartend)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 3, fmainstartend);
                bool fmainmenu = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4);
                if (ImGui::Checkbox("Main Menu", &fmainmenu)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4, fmainmenu);
                bool fsubmissionend = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5);
                if (ImGui::Checkbox("Submission End", &fsubmissionend)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5, fsubmissionend);
                bool fFadeOutExit = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6);
                if (ImGui::Checkbox("Fade Out Exit", &fFadeOutExit)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6, fFadeOutExit);
                ImGui::InputScalar("Menu Info Counter", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuInfoCounter);
                ImGui::InputScalar("Menu Info Length", ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuInfoLenght);
                for (int i = 0; i < 7; i++) {
                    ImGui::InputScalar(("Menu Icon Shift X " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuIconShift_X[i]);
                }
                ImGui::Separator();
                for (int i = 0; i < 6; i++) {
                    ImGui::InputScalar(("Menu Icon Shift Counter " + std::to_string(i)).c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MenuIconShift_COUNTER[i]);
                }
                ImGui::Separator();
                if (ImGui::TreeNode("Dot Circle XY Grid")) {
                    for (int i = 0; i < 23; i++) {
                        for (int j = 0; j < 23; j++) {
                            for (int k = 0; k < 2; k++) {
                                ImGui::InputScalar(("Grid[" + std::to_string(i) + "][" + std::to_string(j) + "][" + std::to_string(k) + "]").c_str(), ImGuiDataType_S8, &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DotCircleXY_Grid[i][j][k]);
                            }
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::Separator();
                bool m_Jimaku = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimaku, 0);
                if (ImGui::Checkbox("Jimaku", &m_Jimaku)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 0, m_Jimaku);
                bool m_JimakuTMP = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_JimakuTMP, 1);
                if (ImGui::Checkbox("Jimaku TMP", &m_JimakuTMP)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 1, m_JimakuTMP);
                bool m_MotionFlag = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MotionFlag, 2);
                if (ImGui::Checkbox("Motion Flag", &m_MotionFlag)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 2, m_MotionFlag);
                bool m_MotionFlagTMP = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_MotionFlagTMP, 3);
                if (ImGui::Checkbox("Motion Flag TMP", &m_MotionFlagTMP)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Jimakuflag, 3, m_MotionFlagTMP);
                bool m_CamAxisXFlag = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4);
                if (ImGui::Checkbox("Cam Axis X Flag", &m_CamAxisXFlag)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 4, m_CamAxisXFlag);
                bool m_CamAxisXFlagTMP = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5);
                if (ImGui::Checkbox("Cam Axis X Flag TMP", &m_CamAxisXFlagTMP)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 5, m_CamAxisXFlagTMP);
                bool m_CamAxisYFlag = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6);
                if (ImGui::Checkbox("Cam Axis Y Flag", &m_CamAxisYFlag)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 6, m_CamAxisYFlag);
                bool m_CamAxisYFlagTMP = getBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 7);
                if (ImGui::Checkbox("Cam Axis Y Flag TMP", &m_CamAxisYFlagTMP)) setBit(mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_Mainflag, 7, m_CamAxisYFlagTMP);
            }
        }

        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            if (ImGui::CollapsingHeader("mHRPc")) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->pad_00);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mOperate);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                ImGui::InputScalar("mEscapeActionInit", ImGuiDataType_S32, &player->mEscapeActionInit);
                ImGui::InputScalar("mEscapeOnButton", ImGuiDataType_S32, &player->mEscapeOnButton);
                ImGui::Text("mpBike: %p", player->mpBike);
                ImGui::InputScalar("mInputMode", ImGuiDataType_S32, &player->mInputMode);
                ImGui::InputScalar("mInputModeOld", ImGuiDataType_S32, &player->mInputModeOld);
                ImGui::InputScalar("mInputModeBefore", ImGuiDataType_S32, &player->mInputModeBefore);
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
            if (ImGui::CollapsingHeader("mHRPC tagMAIN")) {
                if (player->tagMain) {
                    // ImGui::Text("pGlobalListPrev: %p", player->tagMain->pGlobalListPrev);
                    // ImGui::Text("pGlobalListNext: %p", player->tagMain->pGlobalListNext);
                    // for (int i = 0; i < 16; ++i) {
                    //     ImGui::Text(("pDeleteCallback[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pDeleteCallback[i]);
                    //     ImGui::Text(("pDeleteCallbackData[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pDeleteCallbackData[i]);
                    // }
                    ImGui::InputInt("DeleteCallbackNum", &player->tagMain->DeleteCallbackNum);
                    ImGui::Checkbox("ProcessBlock", &player->tagMain->ProcessBlock);
                    ImGui::InputInt("State", reinterpret_cast<int*>(&player->tagMain->State));
                    // ImGui::Text("pNodeStripList: %p", player->tagMain->pNodeStripList);
                    ImGui::InputInt("NodeStripListNum", (int*)&player->tagMain->NodeStripListNum);
                    ImGui::InputInt("UseHeap", (int*)&player->tagMain->UseHeap);
                    ImGui::InputInt("MaterialNum", &player->tagMain->MaterialNum);
                    ImGui::Checkbox("BaseAttach", &player->tagMain->BaseAttach);
                    // ImGui::Text("pBaseAttachTarget: %p", player->tagMain->pBaseAttachTarget);

                    // for (int i = 0; i < 8; ++i) {
                    //     ImGui::Text(("pBaseAttachModel[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pBaseAttachModel[i]);
                    // }

                    ImGui::InputInt("BaseAttachModelNum", &player->tagMain->BaseAttachModelNum);
                    // ImGui::Text("pAttachTarget: %p", player->tagMain->pAttachTarget);
                    // ImGui::Text("pSpec: %p", player->tagMain->pSpec);
                    // ImGui::Text("pTopNodeSpec: %p", player->tagMain->pTopNodeSpec);
                    ImGui::InputFloat3("RootPosition", &player->tagMain->RootPosition.x);
                    ImGui::InputFloat3("RootRotation", &player->tagMain->RootRotation.x);
                    ImGui::InputFloat3("RootScale", &player->tagMain->RootScale.x);
                    for (int row = 0; row < 4; ++row) {
                        for (int col = 0; col < 3; ++col) {
                            ImGui::InputFloat(("RootMatrix[" + std::to_string(row) + "][" + std::to_string(col) + "]").c_str(), &player->tagMain->RootMatrix[row][col]);
                        }
                    }
                    ImGui::Separator();
                    ImGui::Checkbox("ValidRootMatrix", &player->tagMain->ValidRootMatrix);
                    ImGui::InputFloat3("LocalWorldMatrix", &player->tagMain->LocalWorldMatrix[0][0]);
                    ImGui::Checkbox("SetupMatrixFlag", &player->tagMain->SetupMatrixFlag);
                    ImGui::InputInt("MatrixType", reinterpret_cast<int*>(&player->tagMain->MatrixType));
                    // ImGui::Text("pTopNode Addr: %p", player->tagMain->pTopNode);
                    // ImGui::Text("pTopMaterial Addr: %p", player->tagMain->pTopMaterial);
                    // ImGui::Text("pTopTexture Addr: %p", player->tagMain->pTopTexture);
                    ImGui::InputInt("VertFormat", &player->tagMain->VertFormat);
                    ImGui::Checkbox("AlphaBlend", &player->tagMain->AlphaBlend);
                    ImGui::InputInt("Alpha", &player->tagMain->Alpha);
                    ImGui::InputInt("BlendMode", reinterpret_cast<int*>(&player->tagMain->BlendMode));
                    ImGui::Checkbox("EnableDestAlpha", &player->tagMain->EnableDestAlpha);
                    ImGui::InputInt("DestAlpha", &player->tagMain->DestAlpha);
                    ImGui::InputInt("LightRegisterMap", (int*)&player->tagMain->LightRegisterMap);
                    ImGui::InputInt("MaxAttachMotionNum", &player->tagMain->MaxAttachMotionNum);
                    ImGui::Checkbox("EnableMotionSound", &player->tagMain->EnableMotionSound);
                    // for (int i = 0; i < 4; ++i) {
                    //     ImGui::Text(("pCharLightNode Addr[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pCharLightNode[i]);
                    //     ImGui::Text(("pAmbientLightNode Addr[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pAmbientLightNode[i]);
                    //     ImGui::Text(("pDirLightNode Addr[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pDirLightNode[i]);
                    //     ImGui::Text(("pPointLightNode Addr[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pPointLightNode[i]);
                    //     ImGui::Text(("pSpotLightNode Addr[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pSpotLightNode[i]);
                    // }
                    // ImGui::Text("pLightParam Addr: %p", player->tagMain->pLightParam);
                    // ImGui::Text("pNodeBuffer Addr: %p", player->tagMain->pNodeBuffer);
                    // ImGui::InputInt("NodeBufferAddress", &player->tagMain->NodeBufferAddress);
                    // ImGui::Text("pStripNodeBuffer Addr: %p", player->tagMain->pStripNodeBuffer);
                    // ImGui::InputInt("StripNodeBufferAddress", &player->tagMain->StripNodeBufferAddress);
                    for (int i = 0; i < 2; ++i) {
                        if (player->tagMain->pNodeMotionBuffer[i]) {
                            ImGui::Text("pNodeMotionBuffer %d", i);
                            ImGui::Checkbox(("Valid##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Valid);
                            ImGui::Checkbox(("Play##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Play);
                            ImGui::InputFloat3(("Position##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Pos.x);
                            ImGui::InputFloat3(("Rotation##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Rot.x);
                            ImGui::InputFloat3(("Scale##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Scale.x);
                            if (player->tagMain->pNodeMotionBuffer[i]->Anm.pGmfNode) {
                                ImGui::Text("pNodeMotionBuffer[i]->Anm.pGmfNode %d", i);
                                ImGui::Checkbox(("Visible##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.pGmfNode->Visible);
                                ImGui::Checkbox(("Hide##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.pGmfNode->Hide);
                                ImGui::Checkbox(("VC##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.pGmfNode->VC);
                                ImGui::Checkbox(("Big##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.pGmfNode->Big);
                            }
                            if (player->tagMain->pNodeMotionBuffer[i]->Anm.pAnmObj3) {
                                ImGui::Text(">pNodeMotionBuffer[%d]->Anm.pAnmObj3", i);
                                ImGui::InputFloat3(("AnmObj3Num##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.pAnmObj3->pData->x);
                                auto& mainData = *player->tagMain->pNodeMotionBuffer[i]->Anm.pAnmObj3;
                                ImGui::Text("tagMAIN Properties:");
                                ImGui::Text("DataType: %d", mainData.DataType);
                                ImGui::InputFloat3(("NowValue##" + std::to_string(i)).c_str(), &mainData.NowValue.x);
                                bool idle = getBit(mainData.Flag, 0);
                                if (ImGui::Checkbox(("Idle##" + std::to_string(i)).c_str(), &idle)) setBit(mainData.Flag, 0, idle);
                                bool pause = getBit(mainData.Flag, 1);
                                if (ImGui::Checkbox(("Pause##" + std::to_string(i)).c_str(), &pause)) setBit(mainData.Flag, 1, pause);
                                bool interpolate = getBit(mainData.Flag, 2);
                                if (ImGui::Checkbox(("Interpolate##" + std::to_string(i)).c_str(), &interpolate)) setBit(mainData.Flag, 2, interpolate);
                                bool reverse = getBit(mainData.Flag, 3);
                                if (ImGui::Checkbox(("Reverse##" + std::to_string(i)).c_str(), &reverse)) setBit(mainData.Flag, 3, reverse);
                                bool init = getBit(mainData.Flag, 4);
                                if (ImGui::Checkbox(("Init##" + std::to_string(i)).c_str(), &init)) setBit(mainData.Flag, 4, init);
                                bool noUpdate = getBit(mainData.Flag, 5);
                                if (ImGui::Checkbox(("NoUpdate##" + std::to_string(i)).c_str(), &noUpdate)) setBit(mainData.Flag, 5, noUpdate);
                                ImGui::InputInt(("NextType##" + std::to_string(i)).c_str(), (int*)&mainData.NextType);
                                ImGui::InputInt(("PrevType##" + std::to_string(i)).c_str(), (int*)&mainData.PrevType);
                                ImGui::InputInt(("KeyCount##" + std::to_string(i)).c_str(), (int*)&mainData.KeyCount);
                                ImGui::InputInt(("KeyCountMax##" + std::to_string(i)).c_str(), (int*)&mainData.KeyCountMax);
                                ImGui::InputInt(("KeyListCount##" + std::to_string(i)).c_str(), (int*)&mainData.KeyListCount);
                                ImGui::InputFloat(("Tick##" + std::to_string(i)).c_str(), &mainData.Tick);
                                ImGui::InputFloat(("NextWait##" + std::to_string(i)).c_str(), &mainData.NextWait);
                                ImGui::InputFloat(("PrevWait##" + std::to_string(i)).c_str(), &mainData.PrevWait);
                                for (int j = 0; j < 3; ++j) {
                                    ImGui::InputFloat(("NextValue[" + std::to_string(j) + "]##" + std::to_string(i)).c_str(), &mainData.NextValue[j]);
                                    ImGui::InputFloat(("PrevValue[" + std::to_string(j) + "]##" + std::to_string(i)).c_str(), &mainData.PrevValue[j]);
                                    ImGui::InputFloat(("NextSlope[" + std::to_string(j) + "]##" + std::to_string(i)).c_str(), &mainData.NextSlope[j]);
                                    ImGui::InputFloat(("PrevSlope[" + std::to_string(j) + "]##" + std::to_string(i)).c_str(), &mainData.PrevSlope[j]);
                                }
                                ImGui::Separator();
                                for (int j = 0; j < 3; ++j) {
                                    ImGui::Text("NextGeta[%d]:", j);
                                    ImGui::InputFloat(("NextGeta[" + std::to_string(j) + "].Value##" + std::to_string(i)).c_str(), &mainData.NextGeta[j].Value);
                                    ImGui::InputFloat(("NextGeta[" + std::to_string(j) + "].Wait##" + std::to_string(i)).c_str(), &mainData.NextGeta[j].Wait);
                                    ImGui::InputFloat(("NextGeta[" + std::to_string(j) + "].LockWait##" + std::to_string(i)).c_str(), &mainData.NextGeta[j].LockWait);
                                    ImGui::InputFloat(("NextGeta[" + std::to_string(j) + "].InSlope##" + std::to_string(i)).c_str(), &mainData.NextGeta[j].InSlope);
                                    ImGui::InputFloat(("NextGeta[" + std::to_string(j) + "].OutSlope##" + std::to_string(i)).c_str(), &mainData.NextGeta[j].OutSlope);
        
                                    ImGui::Text("PrevGeta[%d]:", j);
                                    ImGui::InputFloat(("PrevGeta[" + std::to_string(j) + "].Value##" + std::to_string(i)).c_str(), &mainData.PrevGeta[j].Value);
                                    ImGui::InputFloat(("PrevGeta[" + std::to_string(j) + "].Wait##" + std::to_string(i)).c_str(), &mainData.PrevGeta[j].Wait);
                                    ImGui::InputFloat(("PrevGeta[" + std::to_string(j) + "].LockWait##" + std::to_string(i)).c_str(), &mainData.PrevGeta[j].LockWait);
                                    ImGui::InputFloat(("PrevGeta[" + std::to_string(j) + "].InSlope##" + std::to_string(i)).c_str(), &mainData.PrevGeta[j].InSlope);
                                    ImGui::InputFloat(("PrevGeta[" + std::to_string(j) + "].OutSlope##" + std::to_string(i)).c_str(), &mainData.PrevGeta[j].OutSlope);
                                }
                                ImGui::Separator();
                                ImGui::InputInt(("NowKeyTypeSize##" + std::to_string(i)).c_str(), (int*)&mainData.NowKeyTypeSize);
                                // for (int j = 0; j < 3; ++j) {
                                //     ImGui::InputText(("NowKey[" + std::to_string(j) + "]##" + std::to_string(i)).c_str(), (char**)&mainData.pNowKey[j]);
                                // }
                                // ImGui::Separator();
                                for (int j = 0; j < 3; ++j) {
                                    ImGui::InputFloat(("NowKeyGeta[" + std::to_string(j) + "].Value##" + std::to_string(i)).c_str(), &mainData.NowKeyGeta[j].Value);
                                    ImGui::InputFloat(("NowKeyGeta[" + std::to_string(j) + "].Wait##" + std::to_string(i)).c_str(), &mainData.NowKeyGeta[j].Wait);
                                    ImGui::InputFloat(("NowKeyGeta[" + std::to_string(j) + "].LockWait##" + std::to_string(i)).c_str(), &mainData.NowKeyGeta[j].LockWait);
                                    ImGui::InputFloat(("NowKeyGeta[" + std::to_string(j) + "].InSlope##" + std::to_string(i)).c_str(), &mainData.NowKeyGeta[j].InSlope);
                                    ImGui::InputFloat(("NowKeyGeta[" + std::to_string(j) + "].OutSlope##" + std::to_string(i)).c_str(), &mainData.NowKeyGeta[j].OutSlope);
                                }
                                ImGui::Separator();
                            }
                            ImGui::InputInt(("AnmObjNum##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.AnmObjNum);
                            ImGui::InputFloat(("Rate##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.Rate);
                            ImGui::InputFloat(("PlayTick##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.PlayTick);
                            ImGui::InputFloat(("StartTick##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.StartTick);
                            ImGui::InputFloat(("EndTick##" + std::to_string(i)).c_str(), &player->tagMain->pNodeMotionBuffer[i]->Anm.EndTick);
                            bool idle = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 0);
                            if (ImGui::Checkbox(("Idle##" + std::to_string(i)).c_str(), &idle)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 0, idle);
                            bool pause = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 1);
                            if (ImGui::Checkbox(("Pause##" + std::to_string(i)).c_str(), &pause)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 1, pause);
                            bool loop = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 2);
                            if (ImGui::Checkbox(("Loop##" + std::to_string(i)).c_str(), &loop)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 2, loop);
                            bool playStart = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 3);
                            if (ImGui::Checkbox(("PlayStart##" + std::to_string(i)).c_str(), &playStart)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 3, playStart);
                            bool noUpdate = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 4);
                            if (ImGui::Checkbox(("NoUpdate##" + std::to_string(i)).c_str(), &noUpdate)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 4, noUpdate);
                            bool rewind = getBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 5);
                            if (ImGui::Checkbox(("Rewind##" + std::to_string(i)).c_str(), &rewind)) setBit(player->tagMain->pNodeMotionBuffer[i]->Anm.Flag, 5, rewind);
                        }
                    }
                    // ImGui::Text("pNodeBlendSetBuffer: %p", player->tagMain->pNodeBlendSetBuffer);
                    ImGui::InputInt("NodeBlendSetAddress", &player->tagMain->NodeBlendSetAddress);
                    // ImGui::Text("pNodeBlendBuffer: %p", player->tagMain->pNodeBlendBuffer);
                    // ImGui::InputInt("NodeBlendAddress", &player->tagMain->NodeBlendAddress);
                    // ImGui::Text("pNodeBlendWeightBuffer: %p", player->tagMain->pNodeBlendWeightBuffer);
                    ImGui::InputInt("NodeBlendWeightAddress", &player->tagMain->NodeBlendWeightAddress);
                    // ImGui::Text("pShapeBuffer: %p", player->tagMain->pShapeBuffer);
                    ImGui::InputInt("ShapeAddress", &player->tagMain->ShapeAddress);
                    ImGui::InputFloat("BoundSphereSize", &player->tagMain->BoundSphereSize);
                    ImGui::Checkbox("NoClip", &player->tagMain->NoClip);
                    ImGui::Checkbox("ReverseCulling", &player->tagMain->ReverseCulling);
                    ImGui::Checkbox("DrawType2", &player->tagMain->DrawType2);
                    ImGui::Checkbox("UseBlendMatrix", &player->tagMain->UseBlendMatrix);
                    // ImGui::Text("pBlendMatrixNodeFirst: %p", player->tagMain->pBlendMatrixNodeFirst);
                    // ImGui::Text("pBlendMatrixNodeLast: %p", player->tagMain->pBlendMatrixNodeLast);
                    ImGui::Checkbox("UseMotionType3", &player->tagMain->UseMotionType3);
                    // ImGui::Text("pMotType3AnmObjInfo: %p", player->tagMain->pMotType3AnmObjInfo);
                    ImGui::InputInt("MotType3AnmObjInfoNum", &player->tagMain->MotType3AnmObjInfoNum);
                    // if (player->tagMain->pMotType3SetupMatrixFlag) { 
                    //     bool* firstBoolPointer = *player->tagMain->pMotType3SetupMatrixFlag;
                    //     if (firstBoolPointer) {
                    //         ImGui::Checkbox("pMotType3SetupMatrixFlag", firstBoolPointer); // this is bool** im lost man
                    //     }
                    // }
                    ImGui::InputInt("MotType3SetupMatrixFlagNum", &player->tagMain->MotType3SetupMatrixFlagNum);
                    ImGui::Text("pMotType3FAnmSpec: %p", player->tagMain->pMotType3FAnmSpec);
                    ImGui::Checkbox("UseMotionType2", &player->tagMain->UseMotionType2);
                    ImGui::Checkbox("UseBlendWeightNormalize", &player->tagMain->UseBlendWeightNormalize);
                    ImGui::Checkbox("UseShadowDraw", &player->tagMain->UseShadowDraw);
                    ImGui::Checkbox("UseTexShadow", &player->tagMain->UseTexShadow);
                    // ImGui::Text("pTexShadowBaseNode: %p", player->tagMain->pTexShadowBaseNode);
                    // ImGui::Text("pTexShadowModel: %p", player->tagMain->pTexShadowModel);
                    ImGui::Checkbox("ValidEasyShadowYPosition", &player->tagMain->ValidEasyShadowYPosition);
                    ImGui::InputFloat("EasyShadowYPosition", &player->tagMain->EasyShadowYPosition);
                    ImGui::InputFloat("EasyShadowScaleXZ", &player->tagMain->EasyShadowScaleXZ);
                    ImGui::Checkbox("ValidEasyShadowYukaNormal", &player->tagMain->ValidEasyShadowYukaNormal);
                    ImGui::InputFloat3("EasyShadowYukaNormal", &player->tagMain->EasyShadowYukaNormal.x);
                    ImGui::Text("pTexShadowGmfNext: %p", player->tagMain->pTexShadowGmfNext);
                    ImGui::Text("pTexShadowGmfPrev: %p", player->tagMain->pTexShadowGmfPrev);
                    ImGui::Checkbox("TexShadowSetupDraw", &player->tagMain->TexShadowSetupDraw);
                    ImGui::Checkbox("EasyShadowDrawOnly", &player->tagMain->EasyShadowDrawOnly);
                    ImGui::Text("pGradationTexture: %p", player->tagMain->pGradationTexture);
                    ImGui::InputFloat3("TexShadowCameraPosition", &player->tagMain->TexShadowCameraPosition.x);
                    ImGui::InputFloat3("TexShadowCameraTarget", &player->tagMain->TexShadowCameraTarget.x);
                    ImGui::InputFloat("TexShadowAreaHeight", &player->tagMain->TexShadowAreaHeight);
                    ImGui::InputFloat("TexShadowAreaWidth", &player->tagMain->TexShadowAreaWidth);
                    ImGui::InputInt("TexShadowPixelWidth", &player->tagMain->TexShadowPixelWidth);
                    ImGui::InputInt("TexShadowPixelHeight", &player->tagMain->TexShadowPixelHeight);

                    // Display MotionSoundSeqLocalValue array
                    for (int i = 0; i < 8; ++i) {
                        ImGui::InputScalar(("MotionSoundSeqLocalValue[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &player->tagMain->MotionSoundSeqLocalValue[i]);
                    }
                    ImGui::Separator();
                    ImGui::InputScalar("MotionSoundSeqLocalValueValid", ImGuiDataType_U16, &player->tagMain->MotionSoundSeqLocalValueValid);
                    ImGui::InputScalar("MotionSoundSeqLocalValueValidNum", ImGuiDataType_U16, &player->tagMain->MotionSoundSeqLocalValueValidNum);
        
                    // // Display link texture shadow
                    // for (int i = 0; i < 16; ++i) {
                    //     ImGui::Text(("pLinkTexShadowGmf[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->pLinkTexShadowGmf[i]);
                    // }

                    ImGui::InputInt("LinkTexShadowGmfNum", &player->tagMain->LinkTexShadowGmfNum);
                    // ImGui::Text("pLinkTexShadowTarget: %p", player->tagMain->pLinkTexShadowTarget);
                    ImGui::InputFloat4("LinkTexShadowGmfDrawArea", player->tagMain->LinkTexShadowGmfDrawArea);
                }
            }

            if (ImGui::CollapsingHeader("mHRPc tagMain tagMOTION")) {
                for (int i = 0; i < 4; ++i) {
                    if (&player->tagMain->Motion[i]) {
                        if (ImGui::CollapsingHeader(("Motion " + std::to_string(i)).c_str())) {
                            ImGui::Checkbox(("Valid 1 [" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].Valid[0]);
                            ImGui::Checkbox(("Valid 2 [" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].Valid[1]);
                            ImGui::InputFloat(("BlendWeight[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].BlendWeight);
                            ImGui::Checkbox(("Blend[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].Blend);
                            ImGui::InputFloat(("BlendRate[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].BlendRate);
                            ImGui::InputFloat(("BlendSpeed[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].BlendSpeed);
                            ImGui::InputInt(("Active[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].Active);
                            ImGui::InputInt(("MotionType2State[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].MotionType2State);
                            ImGui::Checkbox(("MotionAttachFlag[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].MotionAttachFlag);
                            ImGui::Checkbox(("PlayMotionFlag[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].PlayMotionFlag);
                            ImGui::InputFloat(("PlayMotionTick[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].PlayMotionTick);
                            ImGui::InputFloat(("MotionRate[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].MotionRate);
                            // ImGui::Text(("pMotion[" + std::to_string(i) + "]: %p").c_str(), player->tagMain->Motion[i].pMotion);
                            for (int j = 0; j < 2; ++j) {
                                ImGui::InputInt(("TimingSoundHandle[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].TimingSoundHandle[j]);
                            }
                            ImGui::Separator();
                            ImGui::Checkbox(("Loop[" + std::to_string(i) + "]").c_str(), &player->tagMain->Motion[i].Loop);
                            for (int j = 0; j < 2; ++j) {
                                ImGui::Checkbox(("LoopFlag2[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].LoopFlag2[j]);
                                ImGui::InputFloat(("LoopMotionTime2[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].LoopMotionTime2[j]);
                                ImGui::InputInt(("SetupMatrixFlagNum[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].SetupMatrixFlagNum[j]);
                                if (ImGui::TreeNode(("MOVInfo[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str())) {
                                    // ImGui::Text("pGanPlay: %p", player->tagMain->Motion[i].MOVInfo[j].pGanPlay);
                                    ImGui::Checkbox(("ValidPrevPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].ValidPrevPos);
                                    ImGui::InputFloat3(("NowPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].NowPos.x);
                                    ImGui::InputFloat3(("PrevPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].PrevPos.x);
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::Separator();
                        }
                    }
                }
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("mHRPc mCharaStatus")) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->mCharaStatus.resNo);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mCharaStatus.flag2);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);

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
                ImGui::InputInt("motionNo", (int*)&player->mCharaStatus.motionNo);
                ImGui::InputScalar("motionBrendNum", ImGuiDataType_S8, &player->mCharaStatus.motionBrendNum);
                ImGui::InputFloat("motSpd", &player->mCharaStatus.motSpd);
                ImGui::InputScalar("condition", ImGuiDataType_S8, &player->mCharaStatus.condition);
                ImGui::InputFloat3("mYukaNormal", &player->mCharaStatus.mYukaNormal.x);
                ImGui::InputInt("movTick", &player->mCharaStatus.movTick);
                ImGui::InputScalar("hitNum", ImGuiDataType_S16, &player->mCharaStatus.hitNum);
                ImGui::InputInt("mAiAtkType", &player->mCharaStatus.mAiAtkType);
                ImGui::InputInt("mAiAtkClass", &player->mCharaStatus.mAiAtkClass);
                for (int i = 0; i < 10; ++i) {
                    ImGui::InputScalar(("AtkAi[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &player->mCharaStatus.AtkAi[i]);
                }
                ImGui::Separator();
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
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].id);
                        ImGui::InputScalar(("Battery ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].battery);
                        ImGui::InputScalar(("Battery Max ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].batteryMax);
                        ImGui::SliderFloat(("Power ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].power, 0.0f, 100.0f);
                        ImGui::Checkbox(("Combo Extend ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].cmbExtend);
                    }
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Equipped Items")) {
                    for (int i = 0; i < 7; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("readProc ##" + std::to_string(i)).c_str(), (int*)&player->mPcStatus.equip[i].readProc);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), (int*)&player->mPcStatus.equip[i].id);
                        ImGui::Checkbox(("reverseDisp ##" + std::to_string(i)).c_str(), &player->mPcStatus.equip[i].reverseDisp);
                    }
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Locker Items")) {
                    for (int i = 0; i < 200; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.locker[i].id);
                    }
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Inventory Items")) {
                    for (int i = 0; i < 300; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].id);
                        ImGui::InputScalar(("Quantity ##" + std::to_string(i)).c_str(), ImGuiDataType_S8, &player->mPcStatus.item[i].num);
                        ImGui::Checkbox(("Use ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].use);
                    }
                    ImGui::Separator();
                }
                ImGui::SliderFloat("Strength", &player->mPcStatus.strength, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &player->mPcStatus.stammina, 0.0f, 1.0f);
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
                ImGui::InputScalar("chargeShake", ImGuiDataType_U8, &player->mPcStatus.chargeShake);
                ImGui::InputScalar("killCmb", ImGuiDataType_S8, &player->mPcStatus.killCmb);
                ImGui::InputScalar("cmb", ImGuiDataType_U8, &player->mPcStatus.cmb);
                ImGui::InputScalar("cmbKind", ImGuiDataType_S32, &player->mPcStatus.cmbKind);
                ImGui::InputScalar("cmbKindOld", ImGuiDataType_S32, &player->mPcStatus.cmbKindOld);
                ImGui::InputScalar("pose", ImGuiDataType_S32, &player->mPcStatus.pose);
                ImGui::InputScalar("catchTick", ImGuiDataType_S32, &player->mPcStatus.catchTick);
                ImGui::InputScalar("beforeRotY", ImGuiDataType_Float, &player->mPcStatus.beforeRotY);
                ImGui::InputScalar("beforeCamRotY", ImGuiDataType_Float, &player->mPcStatus.beforeCamRotY);
                ImGui::InputScalar("easyDemoProc", ImGuiDataType_S32, &player->mPcStatus.easyDemoProc);
                ImGui::InputScalar("needShakeDirLast", ImGuiDataType_S32, &player->mPcStatus.needShakeDirLast);
                ImGui::InputScalar("needShakeDir", ImGuiDataType_S32, &player->mPcStatus.needShakeDir);
                ImGui::InputScalar("shakeDir", ImGuiDataType_S32, &player->mPcStatus.shakeDir);
                ImGui::InputScalar("throwKind", ImGuiDataType_S32, &player->mPcStatus.throwKind);
                ImGui::Checkbox("throw2nd", &player->mPcStatus.throw2nd);
                ImGui::Checkbox("areaChecksAButton", &player->mPcStatus.areaChecksAButton);
                ImGui::InputScalar("finishNpcIndex", ImGuiDataType_S8, &player->mPcStatus.finishNpcIndex);
                ImGui::InputScalar("oldNpcAtkRate", ImGuiDataType_Float, &player->mPcStatus.oldNpcAtkRate);
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
                ImGui::Separator();
                for (int i = 0; i < 7; i++) {
                    ImGui::Checkbox(std::string("Skill K7 ").append(std::to_string(i)).c_str(), &player->mPcSaveData.skillK7[i]);
                }
                ImGui::Separator();
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
                    ImGui::InputInt("motionNo", (int*)&player->mpLockOnNpc->mStatus.motionNo);
                    ImGui::InputScalar("motionBrendNum", ImGuiDataType_S8, &player->mpLockOnNpc->mStatus.motionBrendNum);
                    ImGui::InputFloat("motSpd", &player->mpLockOnNpc->mStatus.motSpd);
                    ImGui::InputScalar("condition", ImGuiDataType_S8, &player->mpLockOnNpc->mStatus.condition);
                    ImGui::InputFloat3("mYukaNormal", &player->mpLockOnNpc->mStatus.mYukaNormal.x);
                    ImGui::InputInt("movTick", &player->mpLockOnNpc->mStatus.movTick);
                    ImGui::InputScalar("hitNum", ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.hitNum);
                    ImGui::InputInt("mAiAtkType", &player->mpLockOnNpc->mStatus.mAiAtkType);
                    ImGui::InputInt("mAiAtkClass", &player->mpLockOnNpc->mStatus.mAiAtkClass);
                    for (int i = 0; i < 10; ++i) {
                        ImGui::InputScalar(("AtkAi[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &player->mpLockOnNpc->mStatus.AtkAi[i]);
                    }
                    ImGui::Separator();
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
            if (ImGui::CollapsingHeader("mpLockOnNpc mEffect.pBattleIcon")) {
                if (player->mpLockOnNpc) {
                    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->mpLockOnNpc->mEffect.pBattleIcon->Padding_175);
                    uintptr_t statusAddress = reinterpret_cast<uintptr_t>(&player->mpLockOnNpc->mEffect.pBattleIcon->m_DrawHitCmbFlag);
                    uintptr_t offsetDifference = statusAddress - baseAddress;
                    ImGui::Text("Offset target: 0x304 | Offset: 0x%08X", offsetDifference);

                    ImGui::InputScalar("Icon Stat", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->D_BICON_STAT);
                    ImGui::InputScalar("Yoyaku Icon", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_YoyakuIcon);
                    ImGui::InputScalar("Lock On Distance", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_LockOnDist);
                    ImGui::InputScalar("Lock On Circle Size", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_LockOnCircleSize);
                    ImGui::InputScalar("Block Size", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_BIcon_Block_Size);
                    ImGui::InputScalarN("Block Color", ImGuiDataType_U8, player->mpLockOnNpc->mEffect.pBattleIcon->m_blockcolor, 3);
                    ImGui::InputScalar("Color Type", ImGuiDataType_U8, &player->mpLockOnNpc->mEffect.pBattleIcon->m_ColorType);
                    ImGui::InputScalarN("Con Act ID", ImGuiDataType_S32, player->mpLockOnNpc->mEffect.pBattleIcon->m_ConActID, 2);
                    ImGui::InputScalarN("Con Act Count", ImGuiDataType_S16, player->mpLockOnNpc->mEffect.pBattleIcon->m_ConActCount, 2);
                    ImGui::InputScalar("Counter", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Counter);
                    ImGui::InputScalar("Del Counter", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_DelCounter);
                    ImGui::InputScalar("Arrow Appear Count", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Arrow_AppearCnt);
                    ImGui::InputScalar("Rotate Anim Count", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Rotate_AnimCnt);
                    ImGui::InputScalar("Tmp Jst Vec X", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.x);
                    ImGui::InputScalar("Tmp Jst Vec Y", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.y);
                    ImGui::InputScalar("Tmp Jst Vec Z", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.z);
                    ImGui::InputScalar("Pyoko Count", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_PyokoCnt);
                    ImGui::InputScalar("Pyoko Flag", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_PyokoFlag);
                    ImGui::InputScalar("Pyoko Ratio", ImGuiDataType_Float, &player->mpLockOnNpc->mEffect.pBattleIcon->m_PyokoRatio);
                    ImGui::InputScalar("Pyoko Alpha", ImGuiDataType_U8, &player->mpLockOnNpc->mEffect.pBattleIcon->m_PyokoAlpha);
                    ImGui::Checkbox("Draw Hit Combo Flag", (bool*)&player->mpLockOnNpc->mEffect.pBattleIcon->m_DrawHitCmbFlag);
                    ImGui::InputScalarN("Tsuba Ratio", ImGuiDataType_Float, player->mpLockOnNpc->mEffect.pBattleIcon->m_TsubaRatio, 36);
                    for (int i = 0; i < 2; ++i) {
                        ImGui::Text("Position %d: %p", i, player->mpLockOnNpc->mEffect.pBattleIcon->m_pPosition[i]);
                    }
                    ImGui::Text("Node: %p", player->mpLockOnNpc->mEffect.pBattleIcon->m_pNode);
                    for (int i = 0; i < 3; ++i) {
                        ImGui::InputScalarN(fmt::format("Color [{}]", i).c_str(), ImGuiDataType_U8, player->mpLockOnNpc->mEffect.pBattleIcon->m_Color[i], 2);
                    }
                    ImGui::InputScalar("HP Bar Counter", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_HpBerCounter);
                    ImGui::InputScalar("Line Counter", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_LineCounter);
                    ImGui::InputScalar("Meter Counter", ImGuiDataType_S8, &player->mpLockOnNpc->mEffect.pBattleIcon->m_MeterCounter);
                    ImGui::InputScalarN("Tension", ImGuiDataType_Float, player->mpLockOnNpc->mEffect.pBattleIcon->m_Tension, 2);
                    ImGui::InputScalarN("T Meter Rev Position", ImGuiDataType_Float, player->mpLockOnNpc->mEffect.pBattleIcon->m_TMeterRevPos, 2);
                    ImGui::InputScalarN("Get Money", ImGuiDataType_S16, player->mpLockOnNpc->mEffect.pBattleIcon->m_GetMoney, 2);
                    ImGui::InputScalarN("Hit Number", ImGuiDataType_S16, player->mpLockOnNpc->mEffect.pBattleIcon->m_HitNum, 2);
                    ImGui::InputScalarN("HP", ImGuiDataType_Float, player->mpLockOnNpc->mEffect.pBattleIcon->m_HP, 3);
                    ImGui::InputScalar("HP Base Alpha", ImGuiDataType_U8, &player->mpLockOnNpc->mEffect.pBattleIcon->m_HPBaseAlpha);
                    ImGui::InputScalar("HP Virt Alpha", ImGuiDataType_U8, &player->mpLockOnNpc->mEffect.pBattleIcon->m_HPVirtAlpha);
                    ImGui::InputScalarN("Button Counter", ImGuiDataType_S16, player->mpLockOnNpc->mEffect.pBattleIcon->m_BottanCounter, 4);
                    ImGui::InputScalar("Target Icon Count", ImGuiDataType_S16, &player->mpLockOnNpc->mEffect.pBattleIcon->m_TergetIconCount);
                    ImGui::InputScalar("Direct", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Direct);
                    ImGui::InputScalar("Angle", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Angle);
                    ImGui::InputScalar("Sound ID", ImGuiDataType_S32, &player->mpLockOnNpc->mEffect.pBattleIcon->m_Soundid);
                    bool fdrawbase = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 0);
                    if (ImGui::Checkbox("Draw Base", &fdrawbase)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 0, fdrawbase);
                    bool fdrawmoney = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 1);
                    if (ImGui::Checkbox("Draw Money", &fdrawmoney)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 1, fdrawmoney);
                    bool fdrawtension = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 2);
                    if (ImGui::Checkbox("Draw Tension", &fdrawtension)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 2, fdrawtension);
                    bool fdrawheart = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 3);
                    if (ImGui::Checkbox("Draw Heart", &fdrawheart)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 3, fdrawheart);
                    bool fdrawterget = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 4);
                    if (ImGui::Checkbox("Draw Target", &fdrawterget)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 4, fdrawterget);
                    bool fdrawhp = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 5);
                    if (ImGui::Checkbox("Draw HP", &fdrawhp)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 5, fdrawhp);
                    bool fdraw_word = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 6);
                    if (ImGui::Checkbox("Draw Word", &fdraw_word)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 6, fdraw_word);
                    bool fhptype = getBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 10);
                    if (ImGui::Checkbox("HP Type", &fhptype)) setBit(player->mpLockOnNpc->mEffect.pBattleIcon->flag, 10, fhptype);
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
