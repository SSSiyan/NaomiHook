#include "PlayerTracker.hpp"
#include "GuiFunctions.hpp" // for g_framework
#include "ClothesSwitcher.hpp"
#include "Tony.hpp" // for MoveNames
#if 1
bool PlayerTracker::imguiPopout = false;

static uintptr_t gPcCommonTable = NULL;
static float* OverheadKickAddr = NULL;

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

void DrawPlayerStats() {
    if (ImGui::TreeNodeEx("Useful", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            if (ImGui::TreeNodeEx("Player##UsefulTreeNode", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::SliderFloat("Player HP##Useful", &player->mCharaStatus.hp, 0.0f, player->mCharaStatus.maxHp);
                ImGui::InputInt("mInputMode##Useful", (int*)&player->mInputMode, 1);
                ImGui::InputInt("motionNo##Useful", (int*)&player->mCharaStatus.motionNo, 1);
                ImGui::Text("%s", MoveNames[player->mCharaStatus.motionNo]);
                ImGui::InputScalar("Roulette Hit Rate##Useful", ImGuiDataType_S8, &player->mPcStatus.rouletteHitRate);
                ImGui::InputInt("Ikasama Slot##Useful", &player->mPcStatus.ikasamaSlot);
                ImGui::Checkbox("mOperate##Useful", &player->mOperate);
                ImGui::Checkbox("mCameraOperate##Useful", &player->mCameraOperate);
                ImGui::Checkbox("mDead##Useful", &player->mDead);
                ImGui::Checkbox("BaseAttach##Useful", &player->tagMain->BaseAttach);
                ImGui::InputInt("renderSkipCounter##Useful", &player->mCharaStatus.renderSkipCounter);
                bool hitStageDisEnable = getBit(player->mCharaStatus.flag, 10);
                if (ImGui::Checkbox("hitStageDisEnable##Useful", &hitStageDisEnable)) setBit(player->mCharaStatus.flag, 10, hitStageDisEnable);
                ImGui::InputFloat3("pos##Useful", &player->mCharaStatus.pos.x);
                ImGui::TreePop();
            }
            if (mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle()) {
                if (ImGui::TreeNodeEx("Enemy##UsefulTreeNode", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("AttackRate##Useful", &mHRBattle->mNpcAttackRate, 1.0f, 10.0f, "%.1f");
                    if (player->mpLockOnNpc) {
                        ImGui::SliderFloat("Target HP##Useful", &player->mpLockOnNpc->mStatus.hp, 0.0f, player->mpLockOnNpc->mStatus.maxHp);
                    }
                    else {
                        ImGui::Text("No Lock On Target");
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Call Motion IDs / Equip IDs", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            static pcMotion motionID = pcMotion::ePcMtBtLSSonic;
            ImGui::InputInt("Motion ID", (int*)&motionID);
            float itemWidth = ImGui::CalcItemWidth();
            if (ImGui::Button("Play Move", ImVec2(itemWidth, NULL))) {
                nmh_sdk::PlayMotion(motionID, 0, 0, 0, 0.1f);
            }

            static pcItem equipID = SHIRT1;
            ImGui::InputInt("Equip ID", (int*)&equipID);
            if (ImGui::Button("Set Equip", ImVec2(itemWidth, NULL))) {
                nmh_sdk::SetEquip(equipID, false);
            }

            ImGui::SeparatorText("Observed Attack Motion");
            ImGui::Text("Current atkMot: %d", player->mCharaStatus.dmgInfo.atkMot);

        }
        ImGui::TreePop();
    }

    /*if (mHRPc* player = nmh_sdk::get_mHRPc()) {
        if (ImGui::TreeNodeEx("Cancel timings", ImGuiTreeNodeFlags_DrawLinesFull)) {
            // MEMORY_BASIC_INFORMATION mbi;
            // VirtualQuery((uintptr_t*)gPcCommonTable, &mbi, sizeof(mbi));
            // int protectionFlag = 0;
            // if (mbi.Protect & PAGE_READONLY) protectionFlag = 1;
            // else if (mbi.Protect & PAGE_READWRITE) protectionFlag = 2;
            // else if (mbi.Protect & PAGE_EXECUTE_READ) protectionFlag = 3;
            // else if (mbi.Protect & PAGE_EXECUTE_READWRITE) protectionFlag = 4;
            // ImGui::Combo("Memory Protection", &protectionFlag, "Unknown\0Read-Only\0Read-Write\0Execute-Read\0Execute-Read-Write\0");

            // float* HookKickAddr = (float*)(gPcCommonTable + 0x1314);
            // float* HighBeatAddr = (float*)(gPcCommonTable + 0x141C);
            // float* JumpingSlashAddr = (float*)(gPcCommonTable + 0x1600);
            
            // float* RoundhouseKickAddr = (float*)(gPcCommonTable + 0x1340);
            // float* HighChargeAddr = (float*)(gPcCommonTable + 0x1524);

            // DWORD oldProtect1, oldProtect2, oldProtect3, oldProtect4, oldProtect5, oldProtect6;

            // VirtualProtect(HookKickAddr, sizeof(float), PAGE_READWRITE, &oldProtect1);
            // VirtualProtect(HighBeatAddr, sizeof(float), PAGE_READWRITE, &oldProtect2);
            // VirtualProtect(JumpingSlashAddr, sizeof(float), PAGE_READWRITE, &oldProtect3);
            // VirtualProtect(OverheadKickAddr, sizeof(float), PAGE_READWRITE, &oldProtect4);
            // VirtualProtect(RoundhouseKickAddr, sizeof(float), PAGE_READWRITE, &oldProtect5);
            // VirtualProtect(HighChargeAddr, sizeof(float), PAGE_READWRITE, &oldProtect6);

            // ImGui::SliderFloat("360 Hook Kick", HookKickAddr, 0.0f, 999.0f, "%.0f");
            // ImGui::SliderFloat("High Beat Hook Kick", HighBeatAddr, 0.0f, 999.0f, "%.0f");
            // ImGui::SliderFloat("Jumping Slash", JumpingSlashAddr, 0.0f, 999.0f, "%.0f");
            // ImGui::SliderFloat("Overhead Kick", OverheadKickAddr, 0.0f, 999.0f, "%.0f");
            // help_marker("Default 999");
            // ImGui::SliderFloat("Roundhouse Kick", RoundhouseKickAddr, 0.0f, 999.0f, "%.0f");
            // ImGui::SliderFloat("High Charge", HighChargeAddr, 0.0f, 999.0f, "%.0f");

            // VirtualProtect(HookKickAddr, sizeof(float), oldProtect1, &oldProtect1);
            // VirtualProtect(HighBeatAddr, sizeof(float), oldProtect2, &oldProtect2);
            // VirtualProtect(JumpingSlashAddr, sizeof(float), oldProtect3, &oldProtect3);
            // VirtualProtect(OverheadKickAddr, sizeof(float), oldProtect4, &oldProtect4);
            // VirtualProtect(RoundhouseKickAddr, sizeof(float), oldProtect5, &oldProtect5);
            // VirtualProtect(HighChargeAddr, sizeof(float), oldProtect6, &oldProtect6);
        }
    }*/

if (ImGui::TreeNodeEx("HrGameTask", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (HrGameTask* hrGameTask = nmh_sdk::get_HrGameTask()) {

            ImGui::InputScalar("mLoadSts", ImGuiDataType_S32, &hrGameTask->mLoadSts);
            ImGui::Text("mpRankingUp: %p", hrGameTask->mpRankingUp);
            ImGui::InputInt("mDebugScenarioID", &hrGameTask->mDebugScenarioID);
            if (ImGui::Button("Advance Scenario")) {
                hrGameTask->mDebugScenarioID++;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Scenario")) {
                hrGameTask->mDebugScenarioID = 0;
            }

            ImGui::InputScalar("mDebugScenarioInputKeta", ImGuiDataType_S8, &hrGameTask->mDebugScenarioInputKeta);
            ImGui::InputScalar("mDebugScenarioMenu", ImGuiDataType_S8, &hrGameTask->mDebugScenarioMenu);
            ImGui::InputScalar("mDebugLogoSkip", ImGuiDataType_S8, &hrGameTask->mDebugLogoSkip);
            if (ImGui::Button("Skip Logo")) {
                hrGameTask->mDebugLogoSkip = 1;
            }
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

            ImGui::Checkbox("mInitStageLoad", &hrGameTask->mInitStageLoad);
            ImGui::Checkbox("mOpeningMovieRequest", &hrGameTask->mOpeningMovieRequest);
            ImGui::Checkbox("mNewGameRequest", &hrGameTask->mNewGameRequest);
            if (ImGui::Button("Request New Game")) {
                hrGameTask->mNewGameRequest = true;
            }

            ImGui::Checkbox("mGotoTitleRequest", &hrGameTask->mGotoTitleRequest);
            if (ImGui::Button("Request Title Screen")) {
                hrGameTask->mGotoTitleRequest = true;
            }

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
            if (ImGui::TreeNodeEx("mp_SaveData", ImGuiTreeNodeFlags_DrawLinesFull)) {
                if (hrGameTask->mp_SaveData) {
                    ImGui::InputInt("mp_SaveData->t_MainScenarioID", &hrGameTask->mp_SaveData->t_MainScenarioID);
                } else
                    ImGui::Text("mp_SaveData is nullptr");

                if (hrGameTask->mp_CheckPoint) {
                    ImGui::InputInt("mp_CheckPoint->t_MainScenarioID", &hrGameTask->mp_CheckPoint->t_MainScenarioID);
                } else
                    ImGui::Text("mp_CheckPoint is nullptr");

                if (hrGameTask->mp_HikitugiSaveData) {
                    ImGui::InputInt("mp_HikitugiSaveData->t_MainScenarioID", &hrGameTask->mp_HikitugiSaveData->t_MainScenarioID);
                } else
                    ImGui::Text("mp_HikitugiSaveData is nullptr");

                ImGui::TreePop();
            }

            if (ImGui::Button("Enable All SmRunflags")) {
                for (int i = 0; i < 60; ++i)
                    hrGameTask->m_SmRunflag[i] = 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All SmRunflags")) {
                for (int i = 0; i < 60; ++i)
                    hrGameTask->m_SmRunflag[i] = 0;
            }

            for (int i = 0; i < 60; ++i) {
                ImGui::InputInt(("mSmRunflag[" + std::to_string(i) + "]").c_str(), &hrGameTask->m_SmRunflag[i]);
            }

            ImGui::Separator();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("m_pHrMenuTask", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
        ImGui::TreePop();
    }   
    if (ImGui::TreeNodeEx("HrMenuTask->hsSourceBase", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
        ImGui::TreePop();
    }
    if (CBgCtrl* CBgCtrl = nmh_sdk::get_CBgCtrl()) {
        if (ImGui::TreeNodeEx("CBgCtrl", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
            help_marker("Displays the different states for Bishop when delivering the Schpeltiger. 3, 4, 5, 6, 9, 10, 13.");
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
            help_marker("ID for Stage ambiance");
            ImGui::Checkbox("m_bChanged", &CBgCtrl->m_bChanged);
            ImGui::InputInt("m_Status", (int*)&CBgCtrl->m_Status);
            ImGui::InputInt("m_LDstatus", (int*)&CBgCtrl->m_LDstatus);
            ImGui::Checkbox("m_RequestChangeStage", &CBgCtrl->m_RequestChangeStage);
            ImGui::Checkbox("m_DisEnableLoadingEffect", &CBgCtrl->m_DisEnableLoadingEffect);
            ImGui::Checkbox("m_BgRenderOK", &CBgCtrl->m_BgRenderOK);
            ImGui::InputInt("mReleaseWaitCount", &CBgCtrl->mReleaseWaitCount);
            ImGui::TreePop();
        }
    }
    if (mHRBattle* mHRBattle = nmh_sdk::get_mHRBattle()) {
        if (ImGui::TreeNodeEx("mHRBattle", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputInt("mBtlInitProc", (int*)&mHRBattle->mBtlInitProc);
            ImGui::InputInt("mBtlFrameProc", (int*)&mHRBattle->mBtlFrameProc);
            ImGui::Text("struct mCamStatus Address: 0x%08X", ((uintptr_t)(&mHRBattle->mCamStatus)));
            ImGui::InputInt("mTotalNpcNum", &mHRBattle->mTotalNpcNum); 
            ImGui::InputInt("mTotalKillNum", &mHRBattle->mTotalKillNum);
            ImGui::InputInt("mKillNum", &mHRBattle->mKillNum);
            ImGui::InputFloat("mNpcAttackRate", &mHRBattle->mNpcAttackRate, 1.0f, 10.0f, "%.1f");
            bool battlePause = getBit(mHRBattle->mFlag, 0);
            if (ImGui::Checkbox("battlePause", &battlePause)) setBit(mHRBattle->mFlag, 0, battlePause);
            bool tutoRun = getBit(mHRBattle->tutoRun, 1);
            if (ImGui::Checkbox("tutoRun", &tutoRun)) setBit(mHRBattle->mFlag, 1, tutoRun);
            bool dispStatusDisEnable = getBit(mHRBattle->dispStatusDisEnable, 2);
            if (ImGui::Checkbox("dispStatusDisEnable", &dispStatusDisEnable)) setBit(mHRBattle->mFlag, 2, dispStatusDisEnable);
            bool chargeDamage = getBit(mHRBattle->chargeDamage, 3);
            if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(mHRBattle->mFlag, 3, chargeDamage);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("mHRBattle stBtEffect", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputInt("slowMotTotalTick", &mHRBattle->mBtEffect.slowMotTotalTick);
            ImGui::InputInt("slowMotTick", &mHRBattle->mBtEffect.slowMotTick);
            ImGui::InputInt("bulletSlowTick", &mHRBattle->mBtEffect.bulletSlowTick);
            ImGui::InputInt("bossBreakSlowTick", &mHRBattle->mBtEffect.bossBreakSlowTick);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("mHRBattle HrScreenStatus", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                    ImGui::InputScalar(("Slot Deme [" + std::to_string(i) + "]").c_str(), ImGuiDataType_S8, mHRBattle->mBtEffect.pScreenStatus->m_SlotDeme[i]);
                    ImGui::InputScalar(("Slot Deme Counter [" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &mHRBattle->mBtEffect.pScreenStatus->m_SlotDemeCounter[i]);

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
                help_marker("Flag for turning the HP heart white after a scorpion has poisoned you.");
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
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("mHRBattle HrScreenStatus HrInGameMenu", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                help_marker("Toggle the vertical line in the pause menu");
                ImGui::Checkbox("Draw H Line Flag", &mHRBattle->mBtEffect.pScreenStatus->m_pInGameMenu->m_DrawHLineFlag);
                help_marker("Toggle the horizontal line in the pause menu");
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
                if (ImGui::TreeNodeEx("Dot Circle XY Grid", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
            ImGui::TreePop();
        }
        if (mHRPc* player = nmh_sdk::get_mHRPc()) {
            if (ImGui::TreeNodeEx("mHRPc", ImGuiTreeNodeFlags_DrawLinesFull)) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->pad_00);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mOperate);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                ImGui::InputScalar("mEscapeActionInit", ImGuiDataType_S32, &player->mEscapeActionInit);
                ImGui::InputScalar("mEscapeOnButton", ImGuiDataType_S32, &player->mEscapeOnButton);
                ImGui::Text("mpBike: %p", player->mpBike);
                ImGui::InputInt("mInputMode", (int*)&player->mInputMode, 1);
                help_marker("Displays and controls current playmode. 0 = Idle, 1 = FPS, 3 = Walking, 4 = Battle, 5 = Bike");
                ImGui::InputInt("mInputModeOld", (int*)&player->mInputModeOld, 1);
                ImGui::InputInt("mInputModeBefore", (int*)&player->mInputModeBefore, 1);
                ImGui::Checkbox("mPauseAll", &player->mPauseAll);
                ImGui::Checkbox("mPauseNpc", &player->mPauseNpc);
                ImGui::Checkbox("mOperate", &player->mOperate);
                help_marker("Tick to give control over Travis");
                ImGui::Checkbox("mOnlyMove", &player->mOnlyMove);
                ImGui::Checkbox("mMotSpdAdj", &player->mMotSpdAdj);
                ImGui::Checkbox("mDead", &player->mDead);
                help_marker("Kill Travis");
                ImGui::Checkbox("mDeadPause", &player->mDeadPause);
                help_marker("Enables the pause used when dying.");
                ImGui::Checkbox("mCameraOperate", &player->mCameraOperate);
                help_marker("Ticks when camera control is taken away.");
                ImGui::Checkbox("mBattouDemoRequest", &player->mBattouDemoRequest);
                ImGui::Checkbox("mStageChangeInitEnd", &player->mStageChangeInitEnd);
                ImGui::Checkbox("mStageChangeTermEnd", &player->mStageChangeTermEnd);
                ImGui::Checkbox("mStageChangeMuteki", &player->mStageChangeMuteki);
                ImGui::Checkbox("mBanStatusScreen", &player->mBanStatusScreen);
                ImGui::Checkbox("mIkasamaTsuba", &player->mIkasamaTsuba);
                static bool freezeAtkPauseTime = false;

                ImGui::InputInt("mAtkPauseTime", &player->mAtkPauseTime);

                // Add "Freeze" button on same line
                ImGui::SameLine();
                if (ImGui::SmallButton("Freeze = 1")) {
                    freezeAtkPauseTime = !freezeAtkPauseTime;
                }

                // Apply freeze behavior
                if (freezeAtkPauseTime) {
                    player->mAtkPauseTime = 1;
                }
                ImGui::InputInt("mStopRenderFrameNum", &player->mStopRenderFrameNum);
                ImGui::Checkbox("mSavehitOidashiDisEnable", &player->mSavehitOidashiDisEnable);
                ImGui::InputInt("mSlowMotionSytemMotNo", &player->mSlowMotionSytemMotNo);
                ImGui::InputInt("mSlowMotionSytemButton", &player->mSlowMotionSytemButton);
                ImGui::InputInt("mRuleBit", (int*)&player->mRuleBit);
                ImGui::InputInt("mDisEnableBtlPoseCalc", &player->mDisEnableBtlPoseCalc);
                ImGui::InputScalar("mTamePush", ImGuiDataType_U8, &player->mTamePush);
                ImGui::InputScalar("mTameMax", ImGuiDataType_U8, &player->mTameMax);
                ImGui::Checkbox("mTameSe", &player->mTameSe);
                ImGui::InputInt("mDigAction", &player->mDigAction);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPC tagMAIN", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPc tagMAIN tagMOTION", ImGuiTreeNodeFlags_DrawLinesFull)) {
                for (int i = 0; i < 4; ++i) {
                    if (&player->tagMain->Motion[i]) {
                        if (ImGui::TreeNodeEx(("Motion " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                                if (ImGui::TreeNodeEx(("MOVInfo[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
                                    // ImGui::Text("pGanPlay: %p", player->tagMain->Motion[i].MOVInfo[j].pGanPlay);
                                    ImGui::Checkbox(("ValidPrevPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].ValidPrevPos);
                                    ImGui::InputFloat3(("NowPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].NowPos.x);
                                    ImGui::InputFloat3(("PrevPos[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), &player->tagMain->Motion[i].MOVInfo[j].PrevPos.x);
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPc mCharaStatus", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                ImGui::SliderFloat("hp", &player->mCharaStatus.hp, 0.0f, player->mCharaStatus.maxHp);
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
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPc mCharaStatus.dmgInfo", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::SliderFloat("Damage", &player->mCharaStatus.dmgInfo.dmg, 0.0f, 100.0f);
                ImGui::InputInt("Damage Motion", &player->mCharaStatus.dmgInfo.dmgMot);
                help_marker("Displays MotionID for current damage animation");
                ImGui::InputInt("Guard Motion", &player->mCharaStatus.dmgInfo.grdMot);
                help_marker("Displays MotionID for current guard animation");
                ImGui::Text("Attack Motion: %d", player->mCharaStatus.dmgInfo.atkMot);
                help_marker("MotionID of incoming attack and this changes when hit or guarded by it.");
                ImGui::SliderFloat("Damage Direction", &player->mCharaStatus.dmgInfo.dmgDirec, -360.0f, 360.0f);
                help_marker("Displays the direction that the object or pawn is pushed towards when attacked.");
                ImGui::SliderFloat("Knockback Distance", &player->mCharaStatus.dmgInfo.nockBackDst, 0.0f, 100.0f);
                help_marker("Displays the distance that an object or pawn is pushed when attacked.");
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
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPc stPcStatus", ImGuiTreeNodeFlags_DrawLinesFull)) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&player->mPcStatus.atkMot);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->mPcStatus.skillCatch);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                if (ImGui::TreeNodeEx("Weapon Info", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 16; i++) {
                        ImGui::Text("Weapon Info %d", i);
                        ImGui::Text(clothing_items[player->mPcStatus.wepInfo[i].id].name);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].id);
                        ImGui::InputScalar(("Battery ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].battery);
                        ImGui::InputScalar(("Battery Max ##" + std::to_string(i)).c_str(), ImGuiDataType_S16, &player->mPcStatus.wepInfo[i].batteryMax);
                        ImGui::SliderFloat(("Power ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].power, 0.0f, 100.0f);
                        help_marker("Strength of the Beam Katana. This strength value is independent from Travis' own strength.");
                        ImGui::Checkbox(("Combo Extend ##" + std::to_string(i)).c_str(), &player->mPcStatus.wepInfo[i].cmbExtend);
                        help_marker("Ticks when the Beam Katana has had its Slash Combos extended through the Gym dumbell exercise.");
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Equipped Items", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 7; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("readProc ##" + std::to_string(i)).c_str(), (int*)&player->mPcStatus.equip[i].readProc);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), (int*)&player->mPcStatus.equip[i].id);
                        ImGui::Checkbox(("reverseDisp ##" + std::to_string(i)).c_str(), &player->mPcStatus.equip[i].reverseDisp);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Locker Items", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 200; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.locker[i].id);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Inventory Items", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    for (int i = 0; i < 300; i++) {
                        ImGui::Text("Item %d", i);
                        ImGui::InputInt(("ID ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].id);
                        ImGui::InputScalar(("Quantity ##" + std::to_string(i)).c_str(), ImGuiDataType_S8, &player->mPcStatus.item[i].num);
                        ImGui::Checkbox(("Use ##" + std::to_string(i)).c_str(), &player->mPcStatus.item[i].use);
                    }
                    ImGui::TreePop();
                }
                ImGui::SliderFloat("Strength", &player->mPcStatus.strength, 0.0f, 100.0f);
                ImGui::SliderFloat("Stamina", &player->mPcStatus.stammina, 0.0f, 1.0f);
                ImGui::SliderFloat("Vitality", &player->mPcStatus.vitality, 0.0f, 100.0f);
                ImGui::InputInt("Battery Tick", &player->mPcStatus.batteryTick);
                ImGui::InputInt("Attack 360 Tick", &player->mPcStatus.attack360Tick);
                ImGui::SliderFloat("Heroes Meter", &player->mPcStatus.heroesMeter, 0.0f, 1000.0f);
                ImGui::InputInt("Heroes Meter Tick", &player->mPcStatus.heroesMeterTick);
                help_marker("Tick for Cranberry Chocolate Sundae Darkside");
                ImGui::InputInt("Dark Side Use Num", &player->mPcStatus.darkSideUseNum);
                ImGui::InputInt("Dark Side Key", &player->mPcStatus.darkSideKey);
                ImGui::InputInt("Dark Side Key Tick", &player->mPcStatus.darkSideKeyTick);
                ImGui::InputInt("Dark Side Num", &player->mPcStatus.darkSideNum);
                ImGui::InputInt("Dark Side Motion", &player->mPcStatus.darkSideMotion);
                ImGui::InputFloat3("Dark Side Travel Position", (float*)&player->mPcStatus.darkSideTrvPos);
                ImGui::Checkbox("Dark Side End Wait", &player->mPcStatus.darkSideEndWait);
                ImGui::Checkbox("Dark Side Auto Start", &player->mPcStatus.darkSideAutoStart);
                ImGui::Checkbox("Dark Side Key Guide Display", &player->mPcStatus.darkSideKeyGuideDisp);
                ImGui::Checkbox("Dark Side Success Input", &player->mPcStatus.darkSideSuccessInput);
                ImGui::Checkbox("Rapid Finish End", &player->mPcStatus.rapidFinishEnd);
                ImGui::InputInt("Light Side Charge Tick", &player->mPcStatus.lightSideChargeTick);
                help_marker("Tick for Beam Katana charge effects");
                ImGui::InputInt("Rapid Finish Tick", &player->mPcStatus.rapidFinishTick);
                help_marker("Tick for unused Rapid Finish Melon Darkside");
                ImGui::InputInt("Fast Action Tick", &player->mPcStatus.fastActionTick);
                help_marker("Tick for Grasshopper Darkside");
                ImGui::InputInt("Shinku Tick", &player->mPcStatus.shinkuTick);
                help_marker("Tick for Blueberry Cheese Brownie");
                ImGui::InputInt("Bullet Tick", &player->mPcStatus.bulletTick);
                help_marker("Tick for Cherries");
                ImGui::InputInt("Money", &player->mPcStatus.money);
                help_marker("The money you currently have in your wallet.");
                ImGui::InputInt("Money Max", &player->mPcStatus.moneyMax);
                help_marker("The maximum amount of money allowed in your wallet.");
                ImGui::InputInt("Tsuba Tick", &player->mPcStatus.tsubaTick);
                ImGui::InputInt("Tsuba Tick Total", &player->mPcStatus.tsubaTickTotal);
                ImGui::InputFloat("Tsuba Rate", &player->mPcStatus.tsubaRate);
                ImGui::InputScalar("Tsuba Number", ImGuiDataType_S8, &player->mPcStatus.tsubaNum);
                ImGui::InputScalar("Tsuba Rotation Number", ImGuiDataType_S8, &player->mPcStatus.tsubaRotNum);
                ImGui::Text("Tsuba Wait Animation");
                ImGui::InputFloat("Tsuba Wait Rate", &player->mPcStatus.tsubaWait.mMotionRate);
                ImGui::Text("Tsuba Osare Animation");
                ImGui::InputFloat("Tsuba Osare Rate", &player->mPcStatus.tsubaOsare.mMotionRate);
                ImGui::Checkbox("Tsuba Osare Flag", &player->mPcStatus.tsubaOsareFlag);
                ImGui::InputScalar("chargeShake", ImGuiDataType_U8, &player->mPcStatus.chargeShake);
                ImGui::InputScalar("killCmb", ImGuiDataType_S8, &player->mPcStatus.killCmb);
                ImGui::InputScalar("cmb", ImGuiDataType_U8, &player->mPcStatus.cmb);
                help_marker("Displays how far you are into the current Beam Katana combo");
                ImGui::InputScalar("cmbKind", ImGuiDataType_S32, &player->mPcStatus.cmbKind);
                ImGui::InputScalar("cmbKindOld", ImGuiDataType_S32, &player->mPcStatus.cmbKindOld);
                ImGui::InputScalar("pose", ImGuiDataType_S32, &player->mPcStatus.pose);
                help_marker("Displays High / Mid / Low stance");
                ImGui::InputScalar("catchTick", ImGuiDataType_S32, &player->mPcStatus.catchTick);
                help_marker("Displays Wrestling throw tick");
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
                help_marker("Death Blow animation number");
                ImGui::InputInt("Fight Motion Number", &player->mPcStatus.fghMotNo);
                ImGui::InputInt("Gacha Number", &player->mPcStatus.gachaNum);
                ImGui::InputInt("Gacha Tick", &player->mPcStatus.gachaTick);
                ImGui::InputFloat3("Before Position for Camera", (float*)&player->mPcStatus.beforePos4Camera);
                ImGui::Text("Pose Liner Animation");
                ImGui::InputFloat("Pose Liner Rate", &player->mPcStatus.poseLiner.mMotionRate);
                ImGui::InputFloat3("Bike Position", (float*)&player->mPcStatus.bikePos);
                ImGui::InputFloat("Bike Rotation", &player->mPcStatus.bikeRot);
                ImGui::Checkbox("Dig Tool", &player->mPcStatus.digTool);
                help_marker("Toggles the Dig Tool. Will only show up when the katana model is reloaded.");
                ImGui::Checkbox("Rapid Finish", &player->mPcStatus.rapidFinish);
                ImGui::Checkbox("Already Hit Rapid Finish", &player->mPcStatus.alreadyHitRapidFinish);
                ImGui::Checkbox("Combo Flag", &player->mPcStatus.cmbFlag);
                help_marker("Ticks when performing a Beam Katana combo");
                ImGui::InputInt("Step Key", &player->mPcStatus.stepKey);
                ImGui::Checkbox("Step Flag", &player->mPcStatus.stepFlag);
                help_marker("Roll buffer");
                ImGui::Checkbox("Fight Flag", &player->mPcStatus.fghFlag);
                help_marker("Beat Attack buffer");
                ImGui::Checkbox("Range Flag", &player->mPcStatus.rngFlag);
                help_marker("Jumping Slash buffer");
                ImGui::Checkbox("Battou Demo", &player->mPcStatus.battouDemo);
                help_marker("Ticks when the pre battle demo plays for unsheathing Beam Katana");
                ImGui::InputInt("Battou Demo Proc", &player->mPcStatus.battouDemoProc);
                ImGui::Checkbox("Battou Demo Pause Mode", &player->mPcStatus.battouDemoPauseMode);
                ImGui::Checkbox("Noutou Demo", &player->mPcStatus.noutouDemo);
                help_marker("Unused tick for when Travis is sheathing a Beam Katana");
                ImGui::Checkbox("Equip Weapon", &player->mPcStatus.eqWep);
                ImGui::Checkbox("Equip Weapon Laser", &player->mPcStatus.eqWepLaser);
                ImGui::Checkbox("Left Input", &player->mPcStatus.lInput);
                ImGui::Checkbox("Right Input", &player->mPcStatus.rInput);
                ImGui::Checkbox("Lock On Order", &player->mPcStatus.lockOnOder);
                ImGui::Checkbox("Throw Mode Only", &player->mPcStatus.throwModeOnly);
                help_marker("Disable everything but Wrestling moves");
                ImGui::Checkbox("Slash Mode Only", &player->mPcStatus.slashModeOnly);
                help_marker("Disable everything but Beam Katana attacks");
                ImGui::Checkbox("Catch Mode Disable", &player->mPcStatus.catchModeDisEnable);
                help_marker("Disable Wrestling Throws");
                ImGui::Checkbox("Fight Tame Disable", &player->mPcStatus.fightTameDisEnable);
                help_marker("Disable Beat Attack charges");
                ImGui::Checkbox("Slash Tame Disable", &player->mPcStatus.slashTameDisEnable);
                help_marker("Disable Beam Katana Attack charges");
                ImGui::Checkbox("Tsuba Disable", &player->mPcStatus.tsubaDisEnable);
                help_marker("Disable Clashes");
                ImGui::Checkbox("Down Attack Disable", &player->mPcStatus.downAttackDisEnable);
                help_marker("Disable Knockdown Executions");
                ImGui::Checkbox("Dig Disable", &player->mPcStatus.digDisEnable);
                help_marker("Disable digging in the overworld");
                ImGui::Checkbox("Dig Disable 4 SM", &player->mPcStatus.digDisEnable4SM);
                help_marker("Disable digging in the Mine Sweeper side job");
                ImGui::Checkbox("Range Guard", &player->mPcStatus.rangeGuard);
                help_marker("Ticks when guarding bullets");
                ImGui::Checkbox("Dead Motion Play", &player->mPcStatus.deadMotionPlay);
                ImGui::Checkbox("Telephone Walk", &player->mPcStatus.telephoneWalk);
                help_marker("Toggles the phone walk animation");
                ImGui::Checkbox("Electro Shock Walk", &player->mPcStatus.electroShockWalk);
                help_marker("Toggles the electrified animation");
                ImGui::Checkbox("Hug Walk", &player->mPcStatus.hugWalk);
                help_marker("Toggles the Holly Summers body carry animation. Very likely to crash when used where it shouldn't be.");
                ImGui::Checkbox("Event Walk", &player->mPcStatus.eventWalk);
                help_marker("Toggles the walk animation when exiting or entering interiors");
                ImGui::Checkbox("Camera Operate Old", &player->mPcStatus.camOperateOld);
                ImGui::Checkbox("Always Empty Battery", &player->mPcStatus.alwaysEmptyBattery);
                ImGui::Checkbox("Cant Charge Battery", &player->mPcStatus.cantChargeBattery);
                help_marker("Disables battery charging");
                ImGui::Checkbox("Cant Display Laser Effect", &player->mPcStatus.cantDispLaserEffect);
                ImGui::Checkbox("Last Combo Cancel", &player->mPcStatus.lastComboCancel);
                help_marker("Black screen Beat Attacks");
                ImGui::Checkbox("Hizageri Hit", &player->mPcStatus.hizageriHit);
                help_marker("Knee kick Beat Attack");
                ImGui::Checkbox("Back Homing Camera", &player->mPcStatus.backHomingCamera);
                ImGui::Checkbox("Weapon Change Invisible", &player->mPcStatus.wepChangeUnVisible);
                ImGui::Checkbox("Finish Before Attack Already Hit", &player->mPcStatus.finishBeforeAttackAlreadyHit);
                ImGui::Checkbox("Warp Move", &player->mPcStatus.warpMove);
                ImGui::Checkbox("Dont Semitrans", &player->mPcStatus.dontSemitrans);
                ImGui::Checkbox("Dont Semitrans Control", &player->mPcStatus.dontSemitransControl);
                ImGui::Checkbox("Cant Call Bike", &player->mPcStatus.cantCallBike);
                ImGui::Checkbox("Call Bike", &player->mPcStatus.callBike);
                ImGui::Checkbox("Attack Hit Absolute", &player->mPcStatus.atkHitAbsolute);
                ImGui::Checkbox("Lost Bike", &player->mPcStatus.lostBike);
                ImGui::Checkbox("Bike Visible", &player->mPcStatus.bikeVisible);
                for (int i = 0; i < 16; i++) {
                    ImGui::Checkbox(("Skill Catch " + std::to_string(i)).c_str(), &player->mPcStatus.skillCatch[i]);
                    help_marker(wrestlingNames[i]);
                }
                for (int i = 0; i < 7; i++) {
                    ImGui::Checkbox(("Skill K7 " + std::to_string(i)).c_str(), &player->mPcStatus.skillK7[i]);
                    help_marker(k7Names[i]);
                }
                ImGui::InputScalar("Max Combo", ImGuiDataType_S8, &player->mPcStatus.maxCmb);
                help_marker("Current killstreak count");
                ImGui::InputInt("Swing Count", &player->mPcStatus.swingCount);
                help_marker("Displays number of times you've swung Beam Katanas");
                ImGui::InputFloat("Pad Rotation Y", &player->mPcStatus.padRotY);
                ImGui::Text("Dash Projection Animation");
                ImGui::InputFloat("Dash Projection Rate", &player->mPcStatus.dashProjection.mMotionRate);
                ImGui::InputInt("Hajikare Tick", &player->mPcStatus.hajikareTick);
                help_marker("Tick that displays the duration of Travis' stagger when an enemy repels his block string");
                ImGui::InputInt("Guard Tick", &player->mPcStatus.guardTick);
                help_marker("Tick that displays the duration of block stun");
                ImGui::InputFloat("Received Damage", &player->mPcStatus.receiveDmg);
                ImGui::Checkbox("Puppet Mode", &player->mPcStatus.puppetMode);
                ImGui::Checkbox("Use Weapon Effect", &player->mPcStatus.useWeaponEffect);
                ImGui::Checkbox("Todome Prepare Mode", &player->mPcStatus.todomePrepareMode);
                help_marker("Ticks when entering Deathblow");
                ImGui::Checkbox("Throw Prepare Mode", &player->mPcStatus.throwPrepareMode);
                help_marker("Ticks when entering Wrestling move");
                ImGui::InputInt("Throw Input Result", &player->mPcStatus.throwInputResult);
                ImGui::InputFloat("Money Up Rate", &player->mPcStatus.moneyUpRate);
                help_marker("Increases from 1 to 90 when performing a strong Deathblow");
                ImGui::InputFloat("Jump Down Attack Distance", &player->mPcStatus.jumpDownAttackDist);
                help_marker("Displays the distance for Memory of White skill");
                ImGui::InputScalar("Bomb Stock Number", ImGuiDataType_S8, &player->mPcStatus.bomStockNum);
                ImGui::Checkbox("Camera Vertical Reverse Control", &player->mPcStatus.cameraVReverseControl);
                ImGui::Checkbox("Camera Y Reverse Control", &player->mPcStatus.cameraYReverseControl);
                ImGui::Checkbox("Display Map", &player->mPcStatus.dispMap);
                ImGui::InputScalar("Bike Sight", ImGuiDataType_S8, &player->mPcStatus.bikeSight);
                ImGui::Checkbox("Dont Change Bike Camera", &player->mPcStatus.dontChangeBikeCamera);
                ImGui::InputScalar("Shadow Depth", ImGuiDataType_S8, &player->mPcStatus.shadowDepth);
                ImGui::InputInt("Idling Tick", &player->mPcStatus.idlingTick);
                help_marker("Tick for displaying when Travis begins an Idle action");
                ImGui::InputInt("Joyuu Light Number", &player->mPcStatus.joyuuLightNo);
                for (int i = 0; i < 4; i++) {
                    ImGui::InputInt(("Joyuu Light Disable Number " + std::to_string(i)).c_str(), &player->mPcStatus.joyuuLightDisableNo[i]);
                }
                ImGui::InputInt("Joyuu Light Disable Number Count", &player->mPcStatus.joyuuLightDisableNoNum);
                ImGui::InputInt("Clear Number", (int*)&player->mPcStatus.clearNum);
                ImGui::InputScalar("Roulette Hit Rate", ImGuiDataType_S8, &player->mPcStatus.rouletteHitRate);
                help_marker("Chances of winning slots are increased with each successful wrestling move performed. The higher the number, the greater your chances are of winning slots. Max is 100 and will reset when ranking up.");
                ImGui::Checkbox("Finish Bonus", &player->mPcStatus.finishBonus);
                help_marker("Ticks when performing a second directional input during a Death Blow");
                ImGui::Checkbox("Just Guard", &player->mPcStatus.justGuard);
                help_marker("Ticks when a Parry is performed");
                ImGui::Checkbox("Just Attack", &player->mPcStatus.justAttack);
                help_marker("Parry reprisal");
                ImGui::InputInt("Just Input Tick", &player->mPcStatus.justInputTick);
                help_marker("Parry window length");
                ImGui::InputInt("Just Atk Input Start Tick", &player->mPcStatus.justAtkInputStartTick);
                ImGui::InputInt("Just Atk Input End Tick", &player->mPcStatus.justAtkInputEndTick);
                ImGui::Checkbox("Success Input Finish", &player->mPcStatus.successInputFinish);
                ImGui::Checkbox("Finish Reset Slow", &player->mPcStatus.finishResetSlow);
                ImGui::Checkbox("Finish Slow", &player->mPcStatus.finishSlow);
                ImGui::Checkbox("Finish Se Play", &player->mPcStatus.finishSePlay);
                ImGui::InputInt("Finish Slow Tick", &player->mPcStatus.finishSlowTick);
                help_marker("Tick for the slow motion during Death Blows");
                ImGui::Checkbox("Get 777", &player->mPcStatus.get777);
                ImGui::Checkbox("Play Shake Input", &player->mPcStatus.playShakeInput);
                ImGui::InputInt("Play Shake Input Wait", &player->mPcStatus.playShakeInputWait);
                ImGui::Checkbox("Ban Slot Cry", &player->mPcStatus.banSlotCry);
                help_marker("Disables rolling Cherries");
                ImGui::Checkbox("Ban Slot Hop", &player->mPcStatus.banSlotHop);
                help_marker("Disables rolling Strawberry on the Shortcake");
                ImGui::Checkbox("Ban Slot Bel", &player->mPcStatus.banSlotBel);
                help_marker("Disables rolling Blueberry Cheese Brownie");
                ImGui::Checkbox("Ban Slot Bar", &player->mPcStatus.banSlotBar);
                help_marker("Disables rolling Cranberry Chocolate Sundae");
                ImGui::Checkbox("Ban Slot 777", &player->mPcStatus.banSlot777);
                help_marker("Disables rolling Anarchy in the Galaxy");
                ImGui::Checkbox("Weapon Stick", &player->mPcStatus.wepStick);
                help_marker("Harvey stick gimmick. Only enables the SFX.");
                ImGui::InputFloat3("Pad Y Offset", (float*)&player->mPcStatus.padYOffset);
                ImGui::SliderFloat("Rot Y Offset", &player->mPcStatus.rotYOffset, -360.0f, 360.0f);
                ImGui::Checkbox("Tsuba Se 0", &player->mPcStatus.tsubaSe0);
                help_marker("Go!");
                ImGui::Checkbox("Tsuba Se 1", &player->mPcStatus.tsubaSe1);
                help_marker("Go!!");
                ImGui::Checkbox("Tsuba Se 2", &player->mPcStatus.tsubaSe2);
                help_marker("Go!!!");
                ImGui::Checkbox("Dead Submission", &player->mPcStatus.deadSubmission);
                ImGui::Checkbox("Call Motion Process", &player->mPcStatus.callMotionProcess);
                ImGui::InputInt("Dead Boss Num", (int*)&player->mPcStatus.deadBossNum);
                ImGui::InputInt("Tel Ramble Tick", &player->mPcStatus.telRambleTick);
                ImGui::InputInt("Tel Ramble Wait Tick", &player->mPcStatus.telRambleWaitTick);
                ImGui::Checkbox("Tel Ramble", &player->mPcStatus.telRamble);
                ImGui::Checkbox("Cherry", &player->mPcStatus.cherry);
                ImGui::Checkbox("Hit Normal Dash Attack", &player->mPcStatus.hitNormalDashAttack);
                help_marker("Ticks when hitting Running Slash");
                ImGui::Checkbox("Just Avoid", &player->mPcStatus.justAvoid);
                help_marker("Ticks when performing a black screen Darkstep");
                ImGui::Checkbox("Don't Sub Battery", &player->mPcStatus.dontSubBattey);
                ImGui::Checkbox("Auto Sub Battery", &player->mPcStatus.autoSubBattey);
                ImGui::Checkbox("Init Camera", &player->mPcStatus.initCamera);
                ImGui::Checkbox("Data Load", &player->mPcStatus.dataLoad);
                ImGui::Checkbox("Don't Restore Motion", &player->mPcStatus.dontRestoreMotion);
                ImGui::Checkbox("Bike Clash 2 Battou", &player->mPcStatus.bikeClash2battou);
                ImGui::Checkbox("Win Tsubazeri", &player->mPcStatus.winTsubazeri);
                help_marker("Ticks when winning a sword to sword clash");
                ImGui::Checkbox("Just Guard Dis Enable", &player->mPcStatus.justGuardDisEnable);
                help_marker("Disables Parry");
                ImGui::Checkbox("Dash Atk Dis Enable", &player->mPcStatus.dashAtkDisEnable);
                help_marker("Disables Running Slash");
                ImGui::Checkbox("Just Escape Dis Enable", &player->mPcStatus.justEscapeDisEnable);
                help_marker("Disables the Right Stick Darkstep");
                ImGui::InputInt("Ikasama Slot", &player->mPcStatus.ikasamaSlot);
                ImGui::InputInt("Fire Man Tick", &player->mPcStatus.fireManTick);
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRPc mPcSaveData", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::SliderFloat("Max HP", &player->mPcSaveData.maxHp, 0.0f, 1000.0f);
                ImGui::SliderFloat("HP", &player->mPcSaveData.hp, 0.0f, player->mPcSaveData.maxHp, "%.1f");
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
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("mHRBike", ImGuiTreeNodeFlags_DrawLinesFull)) {
                if (player->mpBike) {
                    if (ImGui::TreeNodeEx("mBike", ImGuiTreeNodeFlags_DrawLinesFull)) {
                        ImGui::Text("pGmf: %p", player->mpBike->mBike.pGmf);
                        ImGui::Text("pGan: %p", player->mpBike->mBike.pGan);
                        ImGui::Combo("process", (int*)&player->mpBike->mBike.process, "Process 0\0Process 1\0Process 2\0Process 3\0");
                        ImGui::Combo("fallProcess", (int*)&player->mpBike->mBike.fallProcess, "Fall 0\0Fall 1\0Fall 2\0Fall 3\0");
                        ImGui::InputInt("motionNo", &player->mpBike->mBike.motionNo);
                        help_marker("Displays the MotionID for Travis' current animation on the bike.");
                        ImGui::InputInt("motionNumMax", &player->mpBike->mBike.motionNumMax);
                        ImGui::InputInt("motionBrendNum", &player->mpBike->mBike.motionBrendNum);
                        ImGui::InputInt("restoreMotionNo", &player->mpBike->mBike.restoreMotionNo);
                        ImGui::InputFloat("spd", &player->mpBike->mBike.spd);
                        help_marker("Displays current bike speed");
                        ImGui::InputFloat("posYF", &player->mpBike->mBike.posYF);
                        ImGui::InputFloat("posYB", &player->mpBike->mBike.posYB);
                        ImGui::InputFloat3("pos", &player->mpBike->mBike.pos.x);
                        ImGui::InputFloat3("beforePos", &player->mpBike->mBike.beforePos.x);
                        ImGui::InputFloat3("beforePosF", &player->mpBike->mBike.beforePosF.x);
                        ImGui::InputFloat3("beforePosB", &player->mpBike->mBike.beforePosB.x);
                        ImGui::InputFloat3("moveTempPos", &player->mpBike->mBike.moveTempPos.x);
                        ImGui::InputFloat3("moveTempPosF", &player->mpBike->mBike.moveTempPosF.x);
                        ImGui::InputFloat3("moveTempPosB", &player->mpBike->mBike.moveTempPosB.x);
                        ImGui::InputFloat3("rot", &player->mpBike->mBike.rot.x);
                        ImGui::InputFloat3("oldRot", &player->mpBike->mBike.oldRot.x);
                        ImGui::InputFloat("oldPosY", &player->mpBike->mBike.oldPosY);
                        ImGui::InputFloat("oldPosYF", &player->mpBike->mBike.oldPosYF);
                        ImGui::InputFloat("oldPosYB", &player->mpBike->mBike.oldPosYB);
                        ImGui::InputFloat("adjustRotY", &player->mpBike->mBike.adjustRotY);
                        ImGui::Checkbox("hitCheck", &player->mpBike->mBike.hitCheck);
                        ImGui::Checkbox("clash", &player->mpBike->mBike.clash);
                        ImGui::Checkbox("hitWall", &player->mpBike->mBike.hitWall);
                        ImGui::Checkbox("hitWall4Spin", &player->mpBike->mBike.hitWall4Spin);
                        ImGui::Checkbox("alwaysCheckHitColl", &player->mpBike->mBike.alwaysCheckHitColl);
                        ImGui::InputInt("hp", &player->mpBike->mBike.hp);
                        ImGui::InputInt("bgmHdl", &player->mpBike->mBike.bgmHdl);
                        ImGui::InputInt("seHndlEngine", &player->mpBike->mBike.seHndlEngine);
                        ImGui::InputInt("seHndlEngineNeutral", &player->mpBike->mBike.seHndlEngineNeutral);
                        ImGui::InputInt("seHndlBreak", &player->mpBike->mBike.seHndlBreak);
                        ImGui::InputInt("seHndlQBreak", &player->mpBike->mBike.seHndlQBreak);
                        ImGui::InputInt("seHndlAir", &player->mpBike->mBike.seHndlAir);
                        ImGui::InputInt("seHndlNitro", &player->mpBike->mBike.seHndlNitro);
                        ImGui::InputFloat("seVolEngine", &player->mpBike->mBike.seVolEngine);
                        ImGui::InputFloat("seVolEngineNeutral", &player->mpBike->mBike.seVolEngineNeutral);
                        ImGui::InputFloat("seVolBreak", &player->mpBike->mBike.seVolBreak);
                        ImGui::InputFloat("seVolQBreak", &player->mpBike->mBike.seVolQBreak);
                        ImGui::InputFloat("seVolAir", &player->mpBike->mBike.seVolAir);
                        ImGui::InputFloat("seVolNitro", &player->mpBike->mBike.seVolNitro);
                        ImGui::InputInt("command", &player->mpBike->mBike.command);
                        ImGui::InputFloat("acc", &player->mpBike->mBike.acc);
                        ImGui::Checkbox("useNitro", &player->mpBike->mBike.useNitro);
                        ImGui::InputFloat("nitro", &player->mpBike->mBike.nitro);
                        ImGui::InputInt("nitroStopTick", &player->mpBike->mBike.nitroStopTick);
                        ImGui::InputFloat("turnVal", &player->mpBike->mBike.turnVal);
                        ImGui::InputFloat("rimTurnVal", &player->mpBike->mBike.rimTurnVal);
                        ImGui::InputFloat("qBreakRotY", &player->mpBike->mBike.qBreakRotY);
                        ImGui::Combo("spinDir", (int*)&player->mpBike->mBike.spinDir, "Spin 0\0Spin 1\0Spin 2\0Spin 3\0");
                        ImGui::InputFloat("spinY", &player->mpBike->mBike.spinY);
                        ImGui::Checkbox("wiry", &player->mpBike->mBike.wiry);
                        help_marker("Ticks when performing a Wheelie Jump.");
                        ImGui::Checkbox("clashMySelf", &player->mpBike->mBike.clashMySelf);
                        ImGui::Checkbox("battle", &player->mpBike->mBike.battle);
                        help_marker("Ticks when in battle on the Schpeltiger");
                        ImGui::Checkbox("rideOnStart", &player->mpBike->mBike.rideOnStart);
                        ImGui::Checkbox("initHitJudge", &player->mpBike->mBike.initHitJudge);
                        ImGui::Checkbox("pushAcceling", &player->mpBike->mBike.pushAcceling);
                        help_marker("Ticks when acceclerating");
                        ImGui::Checkbox("pushBreaking", &player->mpBike->mBike.pushBreaking);
                        help_marker("Ticks when braking.");
                        ImGui::Checkbox("cantRideOn", &player->mpBike->mBike.cantRideOn);
                        help_marker("Tick to prevent mounting the bike");
                        ImGui::Checkbox("cantGetOff", &player->mpBike->mBike.cantGetOff);
                        help_marker("Tick to prevent dismounting the bike");
                        ImGui::Checkbox("cantHandling", &player->mpBike->mBike.cantHandling);
                        help_marker("Tick to prevent steering");
                        ImGui::Checkbox("dispCantGetOff", &player->mpBike->mBike.dispCantGetOff);
                        help_marker("Tick to display the speech bubble indicating that dismounting is prohibited.");
                        ImGui::Checkbox("crash2Stand", &player->mpBike->mBike.crash2Stand);
                        ImGui::Checkbox("changeVolEngine", &player->mpBike->mBike.changeVolEngine);
                        ImGui::Checkbox("changeVolEngineIdle", &player->mpBike->mBike.changeVolEngineIdle);
                        ImGui::InputInt("startWait", &player->mpBike->mBike.startWait);
                        for (int i = 0; i < 3; ++i) {
                            ImGui::InputFloat(("rateTbl[" + std::to_string(i) + "]").c_str(), &player->mpBike->mBike.rateTbl[i]);
                        }
                        ImGui::Text("inputRelay: %p", player->mpBike->mBike.inputRelay);
                        ImGui::Text("hitColl: %p", player->mpBike->mBike.hitColl);
                        ImGui::Text("rideColl: %p", player->mpBike->mBike.rideColl);
                        ImGui::Text("hitStgColl: %p", player->mpBike->mBike.hitStgColl);
                        ImGui::Text("hitStgCollF: %p", player->mpBike->mBike.hitStgCollF);
                        ImGui::Text("hitStgCollB: %p", player->mpBike->mBike.hitStgCollB);
                        ImGui::Text("footHitResultObj: %p", player->mpBike->mBike.footHitResultObj);
                        ImGui::Text("footHitResult: %p", player->mpBike->mBike.footHitResult);
                        ImGui::Text("footHitResultObj2: %p", player->mpBike->mBike.footHitResultObj2);
                        ImGui::Text("pEvIcn: %p", player->mpBike->mBike.pEvIcn);
                        ImGui::Text("texSpeed: %p", player->mpBike->mBike.texSpeed);
                        ImGui::Text("cameraLength: %p", player->mpBike->mBike.cameraLength);
                        ImGui::Text("cameraAngle: %p", player->mpBike->mBike.cameraAngle);
                        ImGui::InputFloat("rotZ", &player->mpBike->mBike.rotZ);
                        ImGui::Checkbox("forceHitStage", &player->mpBike->mBike.forceHitStage);
                        ImGui::Checkbox("crashFlag", &player->mpBike->mBike.crashFlag);
                        ImGui::InputInt("damageCount", &player->mpBike->mBike.damageCount);
                        ImGui::Text("prj: %p", player->mpBike->mBike.prj);
                        ImGui::Text("modelAlpha: %p", player->mpBike->mBike.modelAlpha);
                        ImGui::TreePop();
                    }
                    /*if (ImGui::TreeNodeEx("mBikeEffect", ImGuiTreeNodeFlags_DrawLinesFull)) {
                        ImGui::Text("Contents of stBikeEffect struct");
                        ImGui::TreePop();
                    }*/
                    ImGui::Checkbox("mhitStage", &player->mpBike->mhitStage);
                    ImGui::InputFloat("mWryRate", &player->mpBike->mWryRate);
                    help_marker("Value that determines the height of a Wheelie Jump.");
                    ImGui::InputFloat("mBankRate", &player->mpBike->mBankRate);
                    ImGui::InputFloat3("mDamegeDir", &player->mpBike->mDamegeDir.x);
                    ImGui::InputFloat("mDamegeRotY", &player->mpBike->mDamegeRotY);
                    ImGui::InputFloat("mAccel", &player->mpBike->mAccel);
                    ImGui::InputFloat3("mPassRot", &player->mpBike->mPassRot.x);
                    ImGui::InputInt("mFireSEID", &player->mpBike->mFireSEID);
                    ImGui::InputFloat("mHitWait", &player->mpBike->mHitWait);
                    ImGui::Checkbox("mRotLock", &player->mpBike->mRotLock);
                    ImGui::Checkbox("mBkAtkOk", &player->mpBike->mBkAtkOk);
                    /*if (ImGui::TreeNodeEx("mBkPathWalk", ImGuiTreeNodeFlags_DrawLinesFull)) {
                        ImGui::Text("Contents of hPathWalk class");
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNodeEx("mVirtualBkPath", ImGuiTreeNodeFlags_DrawLinesFull)) {
                        ImGui::Text("Contents of hPath class");
                        ImGui::TreePop();
                    }*/
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("hHRPc mPcEffect", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::Checkbox("Laser Track Fade", &player->mPcEffect.laserTrackFade);
                ImGui::InputFloat("Laser Depth", &player->mPcEffect.laserDepth);
                ImGui::InputInt("Laser Wait Tick", &player->mPcEffect.laserWaitTick);
                if (ImGui::TreeNodeEx("EffectCloseContest", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    if (player->mPcEffect.pCloseContest) {
                        bool state = getBit(player->mPcEffect.pCloseContest->m_Flag, 0);
                        if (ImGui::Checkbox("State", &state)) setBit(player->mPcEffect.pCloseContest->m_Flag, 0, state);
                        bool init = getBit(player->mPcEffect.pCloseContest->m_Flag, 3);
                        if (ImGui::Checkbox("Initialized", &init)) setBit(player->mPcEffect.pCloseContest->m_Flag, 3, init);
                        bool active = getBit(player->mPcEffect.pCloseContest->m_Flag, 4);
                        if (ImGui::Checkbox("Active", &active)) setBit(player->mPcEffect.pCloseContest->m_Flag, 4, active);
                        bool visible = getBit(player->mPcEffect.pCloseContest->m_Flag, 5);
                        if (ImGui::Checkbox("Visible", &visible)) setBit(player->mPcEffect.pCloseContest->m_Flag, 5, visible);
                        bool pause = getBit(player->mPcEffect.pCloseContest->m_Flag, 6);
                        if (ImGui::Checkbox("Paused", &pause)) setBit(player->mPcEffect.pCloseContest->m_Flag, 6, pause);
                        if (ImGui::TreeNodeEx("Common", ImGuiTreeNodeFlags_DrawLinesFull)) {
                            ImGui::InputInt("Load State", (int*)(&player->mPcEffect.pCloseContest->Uni.LoadState));
                            ImGui::Text("Texture: SoonTM");
                            ImGui::TreePop();
                        }
                        ImGui::Checkbox("Quad Scale", &player->mPcEffect.pCloseContest->m_QuadScale);
                        ImGui::InputFloat("Set Count", &player->mPcEffect.pCloseContest->m_SetCount);
                        ImGui::InputFloat("Set Rate", &player->mPcEffect.pCloseContest->m_SetRate);
                        ImGui::InputFloat("Time Count", &player->mPcEffect.pCloseContest->m_TimeCount);
                        ImGui::InputInt("Time Frame", &player->mPcEffect.pCloseContest->m_TimeFrame);
                        ImGui::InputFloat("Time Rate", &player->mPcEffect.pCloseContest->m_TimeRate);
                        ImGui::Text("Quad Unified Class: SoonTM");
                        ImGui::Text("Quad Class: SoonTM");
                    }
                    ImGui::TreePop();
                }
                ImGui::Text("Speed Blur Effect: SoonTM");
                ImGui::InputInt("Speed Blur Tick", &player->mPcEffect.speedBlurTick);
                ImGui::InputFloat("Speed Blur Alpha", &player->mPcEffect.speedBlurAlpha);
                ImGui::InputFloat("Speed Blur Scale", &player->mPcEffect.speedBlurScale);
                ImGui::InputInt("Speed Blur Repeat", &player->mPcEffect.speedBlurRepeat);
                if (ImGui::TreeNodeEx("Speed Blur Repeat Number", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("Current Value ## Speed Blur Repeat Number", &player->mPcEffect.speedBlurRepeatNum.mCurValue);
                    ImGui::InputFloat("Destination Value ## Speed Blur Repeat Number", &player->mPcEffect.speedBlurRepeatNum.mDstValue);
                    ImGui::InputFloat("Source Value ## Speed Blur Repeat Number", &player->mPcEffect.speedBlurRepeatNum.mSrcValue);
                    ImGui::InputInt("Count ## Speed Blur Repeat Number", (int*)(&player->mPcEffect.speedBlurRepeatNum.mCount));
                    ImGui::InputInt("Time ## Speed Blur Repeat Number", (int*)(&player->mPcEffect.speedBlurRepeatNum.mTime));
                    ImGui::InputInt("Interpolation Type ## Speed Blur Repeat Number", &player->mPcEffect.speedBlurRepeatNum.mIpType);
                    ImGui::InputFloat("Motion Rate ## Speed Blur Repeat Number", &player->mPcEffect.speedBlurRepeatNum.mMotionRate);
                    ImGui::TreePop();
                }
                ImGui::InputFloat3("Position Hit Slash", (float*)(&player->mPcEffect.posHitSlash));
                ImGui::InputFloat3("Position Hit Slash Old", (float*)(&player->mPcEffect.posHitSlashOld));
                ImGui::Text("Throw Sabel Effect: SoonTM");
                ImGui::InputInt("Light Reflect Tick", &player->mPcEffect.lightReflecteTick);
                ImGui::Text("Concentration Line Effect: SoonTM");
                if (ImGui::TreeNodeEx("Dark Value", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("Current ## Dark Value", &player->mPcEffect.darkValue.mCurValue);
                    ImGui::InputFloat("Destination ## Dark Value", &player->mPcEffect.darkValue.mDstValue);
                    ImGui::InputFloat("Source ## Dark Value", &player->mPcEffect.darkValue.mSrcValue);
                    ImGui::InputInt("Count ## Dark Value", (int*)(&player->mPcEffect.darkValue.mCount));
                    ImGui::InputInt("Time ## Dark Value", (int*)(&player->mPcEffect.darkValue.mTime));
                    ImGui::InputInt("Interpolation Type ## Dark Value", &player->mPcEffect.darkValue.mIpType);
                    ImGui::InputFloat("Motion Rate ## Dark Value", &player->mPcEffect.darkValue.mMotionRate);
                    ImGui::TreePop();
                }
                for (int i = 0; i < 3; i++) {
                    ImGui::Text("Shinku Wave Effect %d: SoonTM", i);
                }
                for (int i = 0; i < 3; i++) {
                    ImGui::Text("Shinku Wave Task Check %d: SoonTM", i);
                }
                ImGui::Text("Model Fire Effect: SoonTM");
                if (ImGui::TreeNodeEx("Model Fire Value", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("Current ## Model Fire", &player->mPcEffect.modelFireValue.mCurValue);
                    ImGui::InputFloat("Destination ## Model Fire", &player->mPcEffect.modelFireValue.mDstValue);
                    ImGui::InputFloat("Source ## Model Fire", &player->mPcEffect.modelFireValue.mSrcValue);
                    ImGui::InputInt("Count ## Model Fire", (int*)(&player->mPcEffect.modelFireValue.mCount));
                    ImGui::InputInt("Time ## Model Fire", (int*)(&player->mPcEffect.modelFireValue.mTime));
                    ImGui::InputInt("Interpolation Type ## Model Fire", &player->mPcEffect.modelFireValue.mIpType);
                    ImGui::InputFloat("Motion Rate ## Model Fire", &player->mPcEffect.modelFireValue.mMotionRate);
                    ImGui::TreePop();
                }
                ImGui::InputInt("Bomb Tick", &player->mPcEffect.bombTick);
                ImGui::Text("Bomb Effect: SoonTM");
                ImGui::Text("Bomb Task Check: SoonTM");
                ImGui::Text("Charge Effect: SoonTM");
                for (int i = 0; i < 2; i++) {
                    ImGui::Text("Laser LR Effect %d: SoonTM", i);
                }
                for (int i = 0; i < 5; i++) {
                    ImGui::Text("Laser Effect %d: SoonTM", i);
                }
                ImGui::Text("Dry Ice Effect: SoonTM");
                ImGui::Text("Metal Elect Effect: SoonTM");
                ImGui::Text("Warp Locus Effect: SoonTM");
                ImGui::Text("Shinku Cursor Texture: SoonTM");

                if (ImGui::TreeNodeEx("Hit Attack Projection", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("Current Value ## Hit Attack Projection", &player->mPcEffect.hitAttackProjection.mCurValue);
                    ImGui::InputFloat("Destination Value ## Hit Attack Projection", &player->mPcEffect.hitAttackProjection.mDstValue);
                    ImGui::InputFloat("Source Value ## Hit Attack Projection", &player->mPcEffect.hitAttackProjection.mSrcValue);
                    ImGui::InputInt("Count ## Hit Attack Projection", (int*)(&player->mPcEffect.hitAttackProjection.mCount));
                    ImGui::InputInt("Time ## Hit Attack Projection", (int*)(&player->mPcEffect.hitAttackProjection.mTime));
                    ImGui::InputInt("Interpolation Type ## Hit Attack Projection", &player->mPcEffect.hitAttackProjection.mIpType);
                    ImGui::InputFloat("Motion Rate ## Hit Attack Projection", &player->mPcEffect.hitAttackProjection.mMotionRate);
                    ImGui::TreePop();
                }
                ImGui::Text("Monokuro Effect: SoonTM");
                ImGui::Text("Slot Noise Effect: SoonTM");
                if (ImGui::TreeNodeEx("Fast Action Blur", ImGuiTreeNodeFlags_DrawLinesFull)) {
                    ImGui::InputFloat("Current ## Fast Action Blur", &player->mPcEffect.fastActionBlur.mCurValue);
                    ImGui::InputFloat("Destination ## Fast Action Blur", &player->mPcEffect.fastActionBlur.mDstValue);
                    ImGui::InputFloat("Source ## Fast Action Blur", &player->mPcEffect.fastActionBlur.mSrcValue);
                    ImGui::InputInt("Count ## Fast Action Blur", (int*)(&player->mPcEffect.fastActionBlur.mCount));
                    ImGui::InputInt("Time ## Fast Action Blur", (int*)(&player->mPcEffect.fastActionBlur.mTime));
                    ImGui::InputInt("Interpolation Type ## Fast Action Blur", &player->mPcEffect.fastActionBlur.mIpType);
                    ImGui::InputFloat("Motion Rate ## Fast Action Blur", &player->mPcEffect.fastActionBlur.mMotionRate);
                    ImGui::TreePop();
                }
                ImGui::Text("Lock On Effect 2: SoonTM");
                ImGui::TreePop();
            }
        }
    }
    if (ImGui::TreeNodeEx("HrCamera", ImGuiTreeNodeFlags_DrawLinesFull)) {
        HrCamera* hrCamera = nmh_sdk::get_HrCamera();
        if (ImGui::TreeNodeEx("MOVE2", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Pc Pos", &hrCamera->MAIN.mov2.PcPos.x);
            ImGui::InputFloat("Pc Angle", &hrCamera->MAIN.mov2.PcAngle);
            ImGui::InputFloat("Cam Angle", &hrCamera->MAIN.mov2.CamAngle);
            ImGui::InputFloat("Cam Targ Length", &hrCamera->MAIN.mov2.CamTargLength);
            ImGui::InputFloat("Cam Y Angle Rate", &hrCamera->MAIN.mov2.CamYAngleRate);
            ImGui::InputFloat3("Abs Cam Pos", &hrCamera->MAIN.mov2.AbsCamPos.x);
            ImGui::InputFloat3("Abs Targ Pos", &hrCamera->MAIN.mov2.AbsTargPos.x);
            ImGui::InputFloat("Pc Look Rate", &hrCamera->MAIN.mov2.PcLookRate);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("BATTLE2", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Player Pos", &hrCamera->MAIN.bat2.PPos.x);
            ImGui::InputFloat3("Player Pos Offset", &hrCamera->MAIN.bat2.PPosOffset.x);
            ImGui::InputFloat3("Enemy Pos", &hrCamera->MAIN.bat2.EPos.x);
            ImGui::Checkbox("Debug Mode", &hrCamera->MAIN.bat2.DebugMode);
            ImGui::InputFloat("Length", &hrCamera->MAIN.bat2.DebugInfo.Length);
            ImGui::InputFloat3("CameraPos", &hrCamera->MAIN.bat2.DebugInfo.CameraPos.x);
            ImGui::InputFloat3("TargetPos", &hrCamera->MAIN.bat2.DebugInfo.TargetPos.x);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("MOTION", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputScalar("Gan Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGan);
            ImGui::InputScalar("GanPlay Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGanPlay);
            ImGui::InputScalar("GanPlayNode Pointer", ImGuiDataType_U32, &hrCamera->MAIN.motion.pGanPlayNode);
            ImGui::InputFloat3("Translate", &hrCamera->MAIN.motion.Translate.x);
            ImGui::InputFloat("Rotate Y", &hrCamera->MAIN.motion.RotateY);
            ImGui::InputFloat("Fov", &hrCamera->MAIN.motion.Fov);
            ImGui::InputFloat("Roll", &hrCamera->MAIN.motion.Roll);
            ImGui::InputFloat("Motion Rate Time", &hrCamera->MAIN.motion.MotionRateTime);
            ImGui::Checkbox("Valid Fov", &hrCamera->MAIN.motion.ValidFov);
            ImGui::Checkbox("Valid Roll", &hrCamera->MAIN.motion.ValidRoll);
            ImGui::Checkbox("Pause", &hrCamera->MAIN.motion.pause);
            ImGui::Checkbox("Coll", &hrCamera->MAIN.motion.coll);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("NORMAL", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Camera Pos", &hrCamera->MAIN.nrm.CPos.x);
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.nrm.TPos.x);
            ImGui::Checkbox("Valid Fov", &hrCamera->MAIN.nrm.ValidFov);
            ImGui::InputFloat("Fov", &hrCamera->MAIN.nrm.Fov);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("HOMING", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.homing.T_Pos.x);
            ImGui::InputFloat3("Camera Pos", &hrCamera->MAIN.homing.C_Pos.x);
            ImGui::InputFloat("Max Length", &hrCamera->MAIN.homing.C_T_MaxLen);
            ImGui::InputFloat("Limit Length", &hrCamera->MAIN.homing.C_T_LimitLen);
            ImGui::InputFloat("Min Length", &hrCamera->MAIN.homing.C_T_MinLen);
            ImGui::InputFloat("Order Length", &hrCamera->MAIN.homing.C_T_OrderLen);
            ImGui::InputFloat("Angle", &hrCamera->MAIN.homing.C_T_Angle);
            ImGui::InputFloat("Add Y", &hrCamera->MAIN.homing.T_PosAddY);
            ImGui::InputInt("Max Over Count", &hrCamera->MAIN.homing.MaxOverCount);
            ImGui::Checkbox("Setup", &hrCamera->MAIN.homing.Setup);
            ImGui::Checkbox("Move Flag", &hrCamera->MAIN.homing.MoveFlag);
            for (int i = 0; i < 20; i++) {
                ImGui::InputFloat3(("T_PosLog[" + std::to_string(i) + "]").c_str(), &hrCamera->MAIN.homing.T_PosLog[i].x);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("FREE", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Camera-Target Pos", &hrCamera->MAIN.free.C_T_Pos.x);
            ImGui::InputFloat("Target VDir", &hrCamera->MAIN.free.T_VDir);
            ImGui::InputFloat("VDir", &hrCamera->MAIN.free.VDir);
            ImGui::InputFloat("Target YDir", &hrCamera->MAIN.free.T_YDir);
            ImGui::InputFloat("YDir", &hrCamera->MAIN.free.YDir);
            ImGui::TreePop();
        }
        if (ImGui::TreeNodeEx("MOVE", ImGuiTreeNodeFlags_DrawLinesFull)) {
            ImGui::InputFloat3("Player Pos", &hrCamera->MAIN.mov.P_Pos.x);
            ImGui::Checkbox("Player Pos Valid", &hrCamera->MAIN.mov.P_PosValid);
            ImGui::Checkbox("Coll Valid", &hrCamera->MAIN.mov.CollValid);
            ImGui::InputFloat3("Target Pos", &hrCamera->MAIN.mov.T_Pos.x);
            ImGui::InputFloat("Add Y", &hrCamera->MAIN.mov.T_PosAddY);
            ImGui::InputFloat3("Target Dir", &hrCamera->MAIN.mov.T_Dir.x);
            ImGui::InputFloat("Target Rot Y", &hrCamera->MAIN.mov.T_RotY);
            ImGui::TreePop();
        }
        ImGui::Combo("Mode", (int*)&hrCamera->MAIN.Mode, "HRCAMERA_MODE_HOMING\0HRCAMERA_MODE_MOTION\0HRCAMERA_MODE_FREE\0HRCAMERA_MODE_MOVE\0HRCAMERA_MODE_BATTLE\0HRCAMERA_MODE_IDLE\0HRCAMERA_MODE_NORMAL\0HRCAMERA_MODE_BATTLE2\0HRCAMERA_MODE_MOVE2\0");
        // uint32_t cameraPosAddr = (uint32_t)(uintptr_t)&hrCamera->MAIN.Pos.x;
        // ImGui::InputScalar("Camera Position Addr", ImGuiDataType_U64, &cameraPosAddr, nullptr, nullptr, "%8X", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat3("Position", &hrCamera->MAIN.Pos.x);
        ImGui::InputFloat3("Target", &hrCamera->MAIN.Targ.x);
        ImGui::InputFloat("Twist Angle", &hrCamera->MAIN.TwistAngle);
        ImGui::InputInt("Frame Counter", &hrCamera->MAIN.FrameCounter);
        ImGui::Checkbox("Always", &hrCamera->MAIN.Always);
        ImGui::Checkbox("Change", &hrCamera->MAIN.Change);
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("HrMotel", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (nmh_sdk::get_HrMenuTask()) {
            if (HrMotel* hrMotel = (HrMotel*)nmh_sdk::get_HrMenuTask()->m_pHsMenu) {
                ImGui::InputInt("m_HrMotelState", (int*)&hrMotel->m_HrMotelState);
                for (int i = 0; i < 2; ++i) {
                    ImGui::InputInt(("m_pNeked_Trv_Model[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_pNeked_Trv_Model[i]);
                }
                ImGui::InputInt("m_pD_Pants_Model", (int*)&hrMotel->m_pD_Pants_Model);
                ImGui::InputInt("m_pStg_Model", (int*)&hrMotel->m_pStg_Model);
                ImGui::InputInt("m_pFan_Model", (int*)&hrMotel->m_pFan_Model);
                ImGui::InputInt("m_pCat_Model", (int*)&hrMotel->m_pCat_Model);
                ImGui::InputInt("m_Etc_Model", (int*)&hrMotel->m_Etc_Model);
                for (int i = 0; i < 3; ++i) {
                    ImGui::InputInt(("m_Trv_Motion_TOILET[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_TOILET[i]);
                    ImGui::InputInt(("m_Trv_Motion_TV[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_TV[i]);
                    ImGui::InputInt(("m_Trv_Motion_FRIDGE[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_FRIDGE[i]);
                    ImGui::InputInt(("m_Trv_Motion_MAP[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_MAP[i]);
                }
                for (int i = 0; i < 4; ++i) {
                    ImGui::InputInt(("m_Trv_Motion_CAT[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_CAT[i]);
                    ImGui::InputInt(("m_Trv_Motion_CLOSET[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_CLOSET[i]);
                    ImGui::InputInt(("m_Trv_Motion_DRAWER[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Trv_Motion_DRAWER[i]);
                    ImGui::InputInt(("m_Fan_Motion[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Fan_Motion[i]);

                }
                for (int i = 0; i < 7; ++i) {
                    ImGui::InputInt(("m_Jara_Motion[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Jara_Motion[i]);
                }
                ImGui::InputInt("m_Trv_Motion_Collec", (int*)&hrMotel->m_Trv_Motion_Collec);
                ImGui::InputInt("m_Trv_Motion_EXIT", (int*)&hrMotel->m_Trv_Motion_EXIT);
                ImGui::InputInt("m_CameraMotion_EXIT", (int*)&hrMotel->m_CameraMotion_EXIT);
                for (int i = 0; i < 9; ++i) {
                    ImGui::InputInt(("m_BG_Box[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_BG_Box[i]);
                    ImGui::InputInt(("m_HsNextBox[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_HsNextBox[i]);
                    ImGui::InputInt(("m_HsBoxMoveAry[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_HsBoxMoveAry[i]);
                }
                ImGui::InputInt("m_CursorBox", (int*)&hrMotel->m_CursorBox);
                ImGui::InputInt("m_BG_Tex", (int*)&hrMotel->m_BG_Tex);
                for (int i = 0; i < 2; ++i) {
                    ImGui::InputInt(("m_Arrow_Line[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Arrow_Line[i]);
                    ImGui::InputInt(("m_BG_Line_Y[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_BG_Line_Y[i]);
    
                }
                for (int i = 0; i < 6; ++i) {
                    ImGui::InputInt(("m_BG_Tri[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_BG_Tri[i]);
                    ImGui::InputInt(("m_Guide_Tex[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->m_Guide_Tex[i]);
                }
                ImGui::InputInt("m_HsNextBg", (int*)&hrMotel->m_HsNextBg);
                ImGui::InputFloat2("m_HsBgMoveAry", (float*)&hrMotel->m_HsBgMoveAry);
                ImGui::InputInt("m_HsNextCur", (int*)&hrMotel->m_HsNextCur);
                ImGui::InputFloat2("m_HsCurMoveAry", (float*)&hrMotel->m_HsCurMoveAry);
                ImGui::InputInt("m_pHsMenuFontMes", (int*)&hrMotel->m_pHsMenuFontMes);
                ImGui::InputInt("m_pNextMenuFontMes", (int*)&hrMotel->m_pNextMenuFontMes);
                ImGui::InputInt("m_pMenuFontMoveAry", (int*)&hrMotel->m_pMenuFontMoveAry);
                ImGui::InputInt("m_pAllSysMes", (int*)&hrMotel->m_pAllSysMes);
                ImGui::Checkbox("m_Toilet_SE_f", &hrMotel->m_Toilet_SE_f);
                ImGui::Checkbox("m_MiniDemo_f", &hrMotel->m_MiniDemo_f);
                ImGui::Checkbox("m_JaraMove_f", &hrMotel->m_JaraMove_f);
                ImGui::Checkbox("m_Cat_Alpha_f", &hrMotel->m_Cat_Alpha_f);
                ImGui::Checkbox("m_Cat_Load_f", &hrMotel->m_Cat_Load_f);
                ImGui::Checkbox("m_PV_OuickBack_f", &hrMotel->m_PV_OuickBack_f);
                ImGui::Checkbox("m_Load_Back_f", &hrMotel->m_Load_Back_f);

                ImGui::InputInt("m_Befor_Box_No", &hrMotel->m_Befor_Box_No);
                ImGui::InputInt("m_After_Box_No", &hrMotel->m_After_Box_No);

                ImGui::Checkbox("m_SaveClassLock_f", &hrMotel->m_SaveClassLock_f);
                ImGui::Checkbox("m_Shadow_Closet_f", &hrMotel->m_Shadow_Closet_f);
                ImGui::Checkbox("m_BedStart_f", &hrMotel->m_BedStart_f);
                ImGui::Checkbox("m_CatControl_f", &hrMotel->m_CatControl_f);

                ImGui::InputInt("m_Guide_Font", (int*)&hrMotel->m_Guide_Font);
                for (int i = 0; i < 2; ++i) {
                    ImGui::InputInt(("Interactions[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->Interactions[i]);
                    ImGui::InputInt(("InteractionCounts[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->InteractionCounts[i]);
                }
                for (int i = 0; i < 9; ++i) {
                    ImGui::InputInt(("livingRoomInteractions[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->livingRoomInteractions[i]);
                }
                for (int i = 0; i < 7; ++i) {
                    ImGui::InputInt(("bedRoomInteractions[" + std::to_string(i) + "]").c_str(), (int*)&hrMotel->bedRoomInteractions[i]);
                }
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("HrMotel HrTV", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (nmh_sdk::get_HrMenuTask()) {
            if (HrMotel* hrMotel = (HrMotel*)nmh_sdk::get_HrMenuTask()->m_pHsMenu) {
                // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&hrMotel->Padding_1345);
                // ImGui::Text("Base Address: 0x%08X", baseAddress);
                // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&hrMotel->m_pHsTV); // m_pHsTV);
                // ImGui::Text("Target Address: 0x%08X", targetAddress);
                // uintptr_t offsetDifference = targetAddress - baseAddress;
                // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
                if (HrTV* hrTv = hrMotel->m_pHsTV) {
                    ImGui::InputScalar("Tv Resource Pointer", ImGuiDataType_U32, &hrTv->m_Tv_Res);
                    ImGui::InputScalar("Video Message Demo Pointer", ImGuiDataType_U32, &hrTv->m_pVideoMesDemo);
                    ImGui::InputText("In Demo Label", hrTv->m_InDemoLabel, sizeof(hrTv->m_InDemoLabel));
                    ImGui::InputScalar("All Video Pointer", ImGuiDataType_U32, &hrTv->m_pAllVideo);
                    ImGui::InputScalar("Displayed Video Number", ImGuiDataType_U32, &hrTv->m_DispVideoNum);
                    ImGui::Checkbox("BGM Volume Off", &hrTv->m_BGM_Vol_Off_f);
                    for (int i = 0; i < 6; i++) {
                        if (ImGui::TreeNodeEx(("Item Video Box " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
                            ImGui::InputFloat("Width", &hrTv->m_Item_Video_Box[i].m_w);
                            ImGui::InputFloat("Height", &hrTv->m_Item_Video_Box[i].m_h);
                            ImGui::InputFloat("Line", &hrTv->m_Item_Video_Box[i].m_Line);
                            {
                                unsigned char r = (hrTv->m_Item_Video_Box[i].m_LineColor >> 24) & 0xFF;
                                unsigned char g = (hrTv->m_Item_Video_Box[i].m_LineColor >> 16) & 0xFF;
                                unsigned char b = (hrTv->m_Item_Video_Box[i].m_LineColor >> 8) & 0xFF;
                                unsigned char a = hrTv->m_Item_Video_Box[i].m_LineColor & 0xFF;
                                float color[4] = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
                                if (ImGui::ColorEdit4("Line Color", color)) {
                                    r = static_cast<unsigned char>(color[0] * 255.0f); // Alpha (Blue)
                                    g = static_cast<unsigned char>(color[1] * 255.0f); // Red (Alpha)
                                    b = static_cast<unsigned char>(color[2] * 255.0f); // Blue (Green)
                                    a = static_cast<unsigned char>(color[3] * 255.0f); // Green (Red)
                                    hrTv->m_Item_Video_Box[i].m_LineColor = (r << 24) | (g << 16) | (b << 8) | a;
                                }
                            }
                            ImGui::InputScalar("Alpha Point", ImGuiDataType_U32, &hrTv->m_Item_Video_Box[i].m_Alpha_Point);
                            ImGui::InputScalar("Line Alpha Type", ImGuiDataType_U32, &hrTv->m_Item_Video_Box[i].m_Line_Alpha_Type);
                            ImGui::InputInt("Alpha Num", &hrTv->m_Item_Video_Box[i].m_Line_Alpha_Num);
                            ImGui::InputInt("Alpha Max Num", &hrTv->m_Item_Video_Box[i].m_Line_Alpha_Max_Num);
                            ImGui::InputInt("Alpha 1F Num", &hrTv->m_Item_Video_Box[i].m_Line_Alpha_1F_Num);
                            ImGui::TreePop();
                        }
                    }
                    for (int i = 0; i < 6; i++) {
                        if (ImGui::TreeNodeEx(("Next Video Pos " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
                            ImGui::InputFloat("X Position", &hrTv->m_HsNextVideo[i].m_x);
                            ImGui::InputFloat("Y Position", &hrTv->m_HsNextVideo[i].m_y);
                            ImGui::TreePop();
                        }
                    }
                    for (int i = 0; i < 6; i++) {
                        ImGui::InputFloat2(("Video Move Array " + std::to_string(i)).c_str(), &hrTv->m_HsVideoMoveAry[i].x);
                    }
                    for (int i = 0; i < 6; i++) {
                        if (ImGui::TreeNodeEx(("Video Tri " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
                            ImGui::InputFloat("Width", &hrTv->m_Video_Tri[i].m_w);
                            ImGui::InputFloat("Height", &hrTv->m_Video_Tri[i].m_h);
                            ImGui::InputFloat("Line", &hrTv->m_Video_Tri[i].m_Line);
                            ImGui::ColorEdit4("Line Color", reinterpret_cast<float*>(&hrTv->m_Video_Tri[i].m_LineColor));
                            ImGui::InputScalar("Alpha Point", ImGuiDataType_U32, &hrTv->m_Video_Tri[i].m_Alpha_Point);
                            ImGui::InputScalar("Line Alpha Type", ImGuiDataType_U32, &hrTv->m_Video_Tri[i].m_Line_Alpha_Type);
                            ImGui::InputInt("Alpha Num", &hrTv->m_Video_Tri[i].m_Line_Alpha_Num);
                            ImGui::InputInt("Alpha Max Num", &hrTv->m_Video_Tri[i].m_Line_Alpha_Max_Num);
                            ImGui::InputInt("Alpha 1F Num", &hrTv->m_Video_Tri[i].m_Line_Alpha_1F_Num);
                            ImGui::TreePop();
                        }
                    }
                    ImGui::InputScalar("Command Motion State", ImGuiDataType_U32, &hrTv->m_Command_Motion_State);
                    ImGui::InputScalar("Command Motion Task", ImGuiDataType_U32, &hrTv->m_Command_Motion_Task);
                    ImGui::Checkbox("Films Visible", &hrTv->m_Films_Visible_f);
                    ImGui::Checkbox("Display Reflected Light", &hrTv->m_Disp_ReflectedLight_f);
                    ImGui::Checkbox("PV Flag", &hrTv->m_PV_f);
                    ImGui::InputScalar("Message Handle", ImGuiDataType_U32, &hrTv->mMessHndle);
                }
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("HrMotel HrCat", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (nmh_sdk::get_HrMenuTask()) {
            if (HrMotel* hrMotel = (HrMotel*)nmh_sdk::get_HrMenuTask()->m_pHsMenu) {
                if (HrCat* hrCat = hrMotel->m_pCat) {
                    if (ImGui::TreeNodeEx("HrCat", ImGuiTreeNodeFlags_DrawLinesFull)) {
                        ImGui::InputScalar("m_HsCatState", ImGuiDataType_S32, &hrCat->m_HsCatState);
                        ImGui::InputScalar("m_HsCatState", ImGuiDataType_S32, &hrCat->m_HsCatState);
                        ImGui::Text("m_Cat_Res: %p", hrCat->m_Cat_Res);  // Pointer
                        ImGui::Text("m_Etc_Model: %p", hrCat->m_Etc_Model);  // Pointer
                        ImGui::InputScalar("m_FlowType", ImGuiDataType_S32, &hrCat->m_FlowType);
                        for (int i = 0; i < 7; ++i) {
                            if (ImGui::TreeNodeEx(("m_Trv_Motion[" + std::to_string(i) + "]").c_str(), ImGuiTreeNodeFlags_DrawLinesFull)) {
                                ImGui::Checkbox(("m_Trv_Motion[" + std::to_string(i) + "].Loop").c_str(), &hrCat->m_Trv_Motion[i].m_Loop_f);
                                ImGui::InputFloat(("m_Trv_Motion[" + std::to_string(i) + "].Start Motion Frame").c_str(), &hrCat->m_Trv_Motion[i].m_StartMotionframe);
                                ImGui::TreePop();
                            }
                        }
                        ImGui::InputScalar("m_Command_Motion_State", ImGuiDataType_S32, &hrCat->m_Command_Motion_State);
                        ImGui::InputScalar("m_Command_Motion_Task", ImGuiDataType_S32, &hrCat->m_Command_Motion_Task);
                        ImGui::Checkbox("m_KeyLock_f", &hrCat->m_KeyLock_f);
                        ImGui::InputInt("m_Motel_Cat_Orders", &hrCat->m_Motel_Cat_Orders);
                        ImGui::InputFloat("m_Cntrol_ShakeSpd", &hrCat->m_Cntrol_ShakeSpd);
                        ImGui::Checkbox("m_Guide_Disp_f", &hrCat->m_Guide_Disp_f);
                        ImGui::Checkbox("m_Cont_Start_f", &hrCat->m_Cont_Start_f);
                        ImGui::InputFloat("m_Alpha_Num", &hrCat->m_Alpha_Num);
                        ImGui::Checkbox("m_Alpha_Type", &hrCat->m_Alpha_Type);

                        ImGui::InputFloat3("m_Plus_Pos", &hrCat->m_Plus_Pos.x);

                        ImGui::Checkbox("m_Cat_Neck_Start_f", &hrCat->m_Cat_Neck_Start_f);
                        ImGui::Checkbox("m_Cat_Jara_Swing_f", &hrCat->m_Cat_Jara_Swing_f);
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::TreePop();
    }


    if (ImGui::TreeNodeEx("HrScreenFilter", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (HrScreenFilter* filter = nmh_sdk::get_HrScreenFilter()) {

            auto& u = filter->uni;

            ImGui::Text("InitFlag:");
            ImGui::SameLine();
            ImGui::InputInt("##InitFlag", &u.InitFlag);

            ImGui::Separator();

            // tagGHMR_TEX Full
            ImGui::Text("Full:");
            ImGui::Indent();
            ImGui::InputScalar("Width##Full", ImGuiDataType_U16, &u.Full.Width);
            ImGui::InputScalar("Height##Full", ImGuiDataType_U16, &u.Full.Height);
            // that didnt work, idk what i was expecting
            // ImGui::Image(u.Full.ImageBuffer, ImVec2(256.0f, 256.0f));
            ImGui::Unindent();

            ImGui::Separator();

            // tagGHMR_TEX Div2
            ImGui::Text("Div2:");
            ImGui::Indent();
            ImGui::InputScalar("Width##Div2", ImGuiDataType_U16, &u.Div2.Width);
            ImGui::InputScalar("Height##Div2", ImGuiDataType_U16, &u.Div2.Height);
            // ImGui::Image(u.Div2.ImageBuffer, ImVec2(128.0f, 128.0f));
            ImGui::Unindent();

            ImGui::Separator();

            // Noise[3]
            for (int i = 0; i < 3; ++i) {
                ImGui::Text("Noise[%d]:", i);
                ImGui::Indent();
                ImGui::InputScalar(("Width##Noise" + std::to_string(i)).c_str(), ImGuiDataType_U16, &u.Noise[i].Width);
                ImGui::InputScalar(("Height##Noise" + std::to_string(i)).c_str(), ImGuiDataType_U16, &u.Noise[i].Height);
                ImGui::Unindent();
            }

            ImGui::Separator();

            // Param[8][2]
            for (int i = 0; i < 8; ++i) {
                ImGui::PushID(i);
                ImGui::Text("Param[%d]", i);
                ImGui::SameLine();
                ImGui::InputInt2("##Param", u.Param[i]);
                ImGui::PopID();
            }

            ImGui::Separator();

            ImGui::InputInt("AnimCounter", &u.AnimCounter);

            ImGui::Text("CaptureOnly:");
            ImGui::SameLine();
            ImGui::InputInt("##CaptureOnly", &u.CaptureOnly);

            ImGui::Separator();

            ImGui::InputInt("NoiseLineNum", &u.NoiseLineNum);
            ImGui::InputInt("NoiseRefreshWait", &u.NoiseRefreshWait);
            ImGui::InputInt("NoiseRefreshCounter", &u.NoiseRefreshCounter);
            ImGui::InputFloat("NoiseMax", &u.NoiseMax, 0.01f, 0.1f, "%.3f");
            ImGui::InputFloat("NoiseDownSpeed", &u.NoiseDownSpeed, 0.01f, 0.1f, "%.3f");

            ImGui::Separator();

            if (ImGui::TreeNodeEx("NoiseZure[0..480]", ImGuiTreeNodeFlags_DrawLinesFull)) {
                for (int i = 0; i < 480; ++i) {
                    ImGui::PushID(i);
                    ImGui::InputFloat(("##NoiseZure" + std::to_string(i)).c_str(), &u.NoiseZure[i], 0.01f, 0.1f, "%.3f");
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

void PlayerTracker::on_draw_ui() {
    ImGui::PushFont(g_framework->get_our_imgui_ctx()->main_font, 24.0f * (ImGui::GetIO().DisplaySize.y / 1080.0f));
    ImGui::Checkbox("Player Stats Popout", &imguiPopout);
    if (!imguiPopout)
        DrawPlayerStats();
    ImGui::PopFont();
}

//void PlayerTracker::custom_imgui_window() {}

// during load
//void PlayerTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void PlayerTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
void PlayerTracker::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("Player Stats", &imguiPopout);
        DrawPlayerStats();
        ImGui::End();
    }
}
// will show up in debug window, dump ImGui widgets you want here
//void PlayerTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
