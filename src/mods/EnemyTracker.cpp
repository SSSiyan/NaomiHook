#include "EnemyTracker.hpp"
#if 1
static bool imguiPopout = false;

std::optional<std::string> EnemyTracker::on_initialize() {
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

void DrawEnemyStats() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    static mHRChara* mpLockOnNpc = NULL;

    static int currentSelectedEnemy = 0;
    static bool useCurrentSelectedEnemySlider = false;
    ImGui::Checkbox("useCurrentSelectedEnemySlider", &useCurrentSelectedEnemySlider);
    if (useCurrentSelectedEnemySlider) {
        ImGui::SliderInt("currentSelectedEnemy", &currentSelectedEnemy, 0, 30);
        mpLockOnNpc = nmh_sdk::get_mHRBattle()->mpNpc[currentSelectedEnemy];
        if (ImGui::Button("Find Locked On Enemy In List")) {
            if (player->mpLockOnNpc) {
                for (uint32_t i = 0; i < 30; ++i) {
                    if (nmh_sdk::get_mHRBattle()->mpNpc[i] && nmh_sdk::get_mHRBattle()->mpNpc[i] == player->mpLockOnNpc) {
                        currentSelectedEnemy = i;
                        break;
                    }
                }
            }
        }
    }
    else {
        static bool saveCurrentLockedOnEnemy = false;
        if (ImGui::Checkbox("Save Currently Locked On Enemy", &saveCurrentLockedOnEnemy)) {
            mpLockOnNpc = player->mpLockOnNpc; // update on checking the box
        }
        if (!saveCurrentLockedOnEnemy) {
            mpLockOnNpc = player->mpLockOnNpc; // update every tick
        }
    }

    if (ImGui::CollapsingHeader("mpLockOnNpc")) {
        if (mpLockOnNpc) {
            ImGui::InputFloat("mDistFromPc", &mpLockOnNpc->mDistFromPc);
            ImGui::InputFloat("mDirecYFromPc", &mpLockOnNpc->mDirecYFromPc);
        }
    }
    if (ImGui::CollapsingHeader("mpLockOnNpc mStatus")) {
        if (mpLockOnNpc) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mZakoWepMotionNumMax[0]);
            // uintptr_t statusAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mDirecYFromPc);
            // uintptr_t offsetDifference = statusAddress - baseAddress;
            // ImGui::Text("Offset target: 0x304 | Offset: 0x%08X", offsetDifference);

            ImGui::InputInt("resNo", &mpLockOnNpc->mStatus.resNo);
            ImGui::InputInt("datNo", &mpLockOnNpc->mStatus.datNo);
            ImGui::InputInt("charaType", (int*)&mpLockOnNpc->mStatus.charaType);
            ImGui::InputScalar("zakoWepType", ImGuiDataType_S16, &mpLockOnNpc->mStatus.zakoWepType);
            ImGui::InputFloat("maxHp", &mpLockOnNpc->mStatus.maxHp);
            ImGui::SliderFloat("hp", &mpLockOnNpc->mStatus.hp, 0.0f, mpLockOnNpc->mStatus.maxHp);
            ImGui::InputScalar("money", ImGuiDataType_S16, &mpLockOnNpc->mStatus.money);
            ImGui::InputScalar("dropMoney", ImGuiDataType_S16, &mpLockOnNpc->mStatus.dropMoney);
            ImGui::InputFloat("tension", &mpLockOnNpc->mStatus.tension);
            ImGui::InputFloat("hajikiCount", &mpLockOnNpc->mStatus.hajikiCount);
            ImGui::Checkbox("alwaysCheckHitColl", &mpLockOnNpc->mStatus.alwaysCheckHitColl);
            ImGui::InputFloat3("pos", &mpLockOnNpc->mStatus.pos.x);
            ImGui::InputFloat3("beforePos", &mpLockOnNpc->mStatus.beforePos.x);
            ImGui::InputFloat3("navelPos", &mpLockOnNpc->mStatus.navelPos.x);
            ImGui::InputFloat3("brainPos", &mpLockOnNpc->mStatus.brainPos.x);
            ImGui::InputFloat3("rot", &mpLockOnNpc->mStatus.rot.x);
            ImGui::InputFloat3("movRot", &mpLockOnNpc->mStatus.movRot.x);
            ImGui::InputFloat4("qRot", &mpLockOnNpc->mStatus.qRot.x);
            ImGui::InputFloat("movSpd", &mpLockOnNpc->mStatus.movSpd);
            ImGui::InputFloat("hitSize", &mpLockOnNpc->mStatus.hitSize);
            ImGui::InputFloat("height", &mpLockOnNpc->mStatus.height);
            ImGui::InputFloat("upperPosY", &mpLockOnNpc->mStatus.upperPosY);
            ImGui::InputInt("enterPattern", &mpLockOnNpc->mStatus.enterPattern);
            ImGui::InputFloat("reactionDist", &mpLockOnNpc->mStatus.reactionDist);
            ImGui::InputInt("itemNo", &mpLockOnNpc->mStatus.itemNo);
            ImGui::InputInt("mAiDefBasic", &mpLockOnNpc->mStatus.mAiDefBasic);
            ImGui::InputInt("mAiDefCurrent", &mpLockOnNpc->mStatus.mAiDefCurrent);
            ImGui::InputInt("mAiDefTick", &mpLockOnNpc->mStatus.mAiDefTick);
            ImGui::InputInt("mAiDefBaseTick", &mpLockOnNpc->mStatus.mAiDefBaseTick);
            ImGui::InputFloat("mAiDamageCount", &mpLockOnNpc->mStatus.mAiDamageCount);
            ImGui::InputInt("tsubazeriNum", &mpLockOnNpc->mStatus.tsubazeriNum);
            ImGui::InputInt("DamageAcceptFrame", &mpLockOnNpc->mStatus.DamageAcceptFrame);
            ImGui::InputInt("motionNo", (int*)&mpLockOnNpc->mStatus.motionNo);
            ImGui::InputScalar("motionBrendNum", ImGuiDataType_S8, &mpLockOnNpc->mStatus.motionBrendNum);
            ImGui::InputFloat("motSpd", &mpLockOnNpc->mStatus.motSpd);
            ImGui::InputScalar("condition", ImGuiDataType_S8, &mpLockOnNpc->mStatus.condition);
            ImGui::InputFloat3("mYukaNormal", &mpLockOnNpc->mStatus.mYukaNormal.x);
            ImGui::InputInt("movTick", &mpLockOnNpc->mStatus.movTick);
            ImGui::InputScalar("hitNum", ImGuiDataType_S16, &mpLockOnNpc->mStatus.hitNum);
            ImGui::InputInt("mAiAtkType", &mpLockOnNpc->mStatus.mAiAtkType);
            ImGui::InputInt("mAiAtkClass", &mpLockOnNpc->mStatus.mAiAtkClass);
            for (int i = 0; i < 10; ++i) {
                ImGui::InputScalar(("AtkAi[" + std::to_string(i) + "]").c_str(), ImGuiDataType_S16, &mpLockOnNpc->mStatus.AtkAi[i]);
            }
            ImGui::Separator();
            ImGui::InputInt("renderSkipCounter", &mpLockOnNpc->mStatus.renderSkipCounter);
            ImGui::InputFloat("renderSkipMotSpd", &mpLockOnNpc->mStatus.renderSkipMotSpd);
            ImGui::InputInt("frameStop", &mpLockOnNpc->mStatus.frameStop);
            bool visible = getBit(mpLockOnNpc->mStatus.flag, 0);
            if (ImGui::Checkbox("visible", &visible)) setBit(mpLockOnNpc->mStatus.flag, 0, visible);
            bool visibleWep = getBit(mpLockOnNpc->mStatus.flag, 1);
            if (ImGui::Checkbox("visibleWep", &visibleWep)) setBit(mpLockOnNpc->mStatus.flag, 1, visibleWep);
            bool visibleWepEffect = getBit(mpLockOnNpc->mStatus.flag, 2);
            if (ImGui::Checkbox("visibleWepEffect", &visibleWepEffect)) setBit(mpLockOnNpc->mStatus.flag, 2, visibleWepEffect);
            bool visibleDismember = getBit(mpLockOnNpc->mStatus.flag, 3);
            if (ImGui::Checkbox("visibleDismember", &visibleDismember)) setBit(mpLockOnNpc->mStatus.flag, 3, visibleDismember);
            bool visibleDist = getBit(mpLockOnNpc->mStatus.flag, 4);
            if (ImGui::Checkbox("visibleDist", &visibleDist)) setBit(mpLockOnNpc->mStatus.flag, 4, visibleDist);
            bool lockOn = getBit(mpLockOnNpc->mStatus.flag, 5);
            if (ImGui::Checkbox("lockOn", &lockOn)) setBit(mpLockOnNpc->mStatus.flag, 5, lockOn);
            bool hitChara = getBit(mpLockOnNpc->mStatus.flag, 6);
            if (ImGui::Checkbox("hitChara", &hitChara)) setBit(mpLockOnNpc->mStatus.flag, 6, hitChara);
            bool hitAttack = getBit(mpLockOnNpc->mStatus.flag, 7);
            if (ImGui::Checkbox("hitAttack", &hitAttack)) setBit(mpLockOnNpc->mStatus.flag, 7, hitAttack);
            bool hitStage = getBit(mpLockOnNpc->mStatus.flag, 8);
            if (ImGui::Checkbox("hitStage", &hitStage)) setBit(mpLockOnNpc->mStatus.flag, 8, hitStage);
            bool downShockWave = getBit(mpLockOnNpc->mStatus.flag, 9);
            if (ImGui::Checkbox("downShockWave", &downShockWave)) setBit(mpLockOnNpc->mStatus.flag, 9, downShockWave);
            bool hitStageDisEnable = getBit(mpLockOnNpc->mStatus.flag, 10);
            if (ImGui::Checkbox("hitStageDisEnable", &hitStageDisEnable)) setBit(mpLockOnNpc->mStatus.flag, 10, hitStageDisEnable);
            bool hitStageDisEnableReq = getBit(mpLockOnNpc->mStatus.flag, 11);
            if (ImGui::Checkbox("hitStageDisEnableReq", &hitStageDisEnableReq)) setBit(mpLockOnNpc->mStatus.flag, 11, hitStageDisEnableReq);
            bool hitOidashiDisEnable = getBit(mpLockOnNpc->mStatus.flag, 12);
            if (ImGui::Checkbox("hitOidashiDisEnable", &hitOidashiDisEnable)) setBit(mpLockOnNpc->mStatus.flag, 12, hitOidashiDisEnable);
            bool operateDisEnable = getBit(mpLockOnNpc->mStatus.flag, 13);
            if (ImGui::Checkbox("operateDisEnable", &operateDisEnable)) setBit(mpLockOnNpc->mStatus.flag, 13, operateDisEnable);
            bool drawPriority = getBit(mpLockOnNpc->mStatus.flag, 14);
            if (ImGui::Checkbox("drawPriority", &drawPriority)) setBit(mpLockOnNpc->mStatus.flag, 14, drawPriority);
            bool charaPause = getBit(mpLockOnNpc->mStatus.flag, 15);
            if (ImGui::Checkbox("charaPause", &charaPause)) setBit(mpLockOnNpc->mStatus.flag, 15, charaPause);
            bool initPlayMotion = getBit(mpLockOnNpc->mStatus.flag, 16);
            if (ImGui::Checkbox("initPlayMotion", &initPlayMotion)) setBit(mpLockOnNpc->mStatus.flag, 16, initPlayMotion);
            bool slowBlow = getBit(mpLockOnNpc->mStatus.flag, 17);
            if (ImGui::Checkbox("slowBlow", &slowBlow)) setBit(mpLockOnNpc->mStatus.flag, 17, slowBlow);
            bool dontCountKill = getBit(mpLockOnNpc->mStatus.flag, 18);
            if (ImGui::Checkbox("dontCountKill", &dontCountKill)) setBit(mpLockOnNpc->mStatus.flag, 18, dontCountKill);
            bool dontSubRepop = getBit(mpLockOnNpc->mStatus.flag, 19);
            if (ImGui::Checkbox("dontSubRepop", &dontSubRepop)) setBit(mpLockOnNpc->mStatus.flag, 19, dontSubRepop);
            bool dontStandUp = getBit(mpLockOnNpc->mStatus.flag, 20);
            if (ImGui::Checkbox("dontStandUp", &dontStandUp)) setBit(mpLockOnNpc->mStatus.flag, 20, dontStandUp);
            bool lowDamage = getBit(mpLockOnNpc->mStatus.flag, 21);
            if (ImGui::Checkbox("lowDamage", &lowDamage)) setBit(mpLockOnNpc->mStatus.flag, 21, lowDamage);
            bool dontHitStageWall = getBit(mpLockOnNpc->mStatus.flag, 22);
            if (ImGui::Checkbox("dontHitStageWall", &dontHitStageWall)) setBit(mpLockOnNpc->mStatus.flag, 22, dontHitStageWall);
            bool ikari = getBit(mpLockOnNpc->mStatus.flag, 23);
            if (ImGui::Checkbox("ikari", &ikari)) setBit(mpLockOnNpc->mStatus.flag, 23, ikari);
            bool motSpeedControl = getBit(mpLockOnNpc->mStatus.flag, 24);
            if (ImGui::Checkbox("motSpeedControl", &motSpeedControl)) setBit(mpLockOnNpc->mStatus.flag, 24, motSpeedControl);
            bool bossDeadFlag = getBit(mpLockOnNpc->mStatus.flag, 25);
            if (ImGui::Checkbox("bossDeadFlag", &bossDeadFlag)) setBit(mpLockOnNpc->mStatus.flag, 25, bossDeadFlag);
            bool successThrow = getBit(mpLockOnNpc->mStatus.flag, 26);
            if (ImGui::Checkbox("successThrow", &successThrow)) setBit(mpLockOnNpc->mStatus.flag, 26, successThrow);
            bool loseTsubazeri = getBit(mpLockOnNpc->mStatus.flag, 27);
            if (ImGui::Checkbox("loseTsubazeri", &loseTsubazeri)) setBit(mpLockOnNpc->mStatus.flag, 27, loseTsubazeri);
            bool chargeDamage = getBit(mpLockOnNpc->mStatus.flag, 28);
            if (ImGui::Checkbox("chargeDamage", &chargeDamage)) setBit(mpLockOnNpc->mStatus.flag, 28, chargeDamage);
            bool motionProcessDisEnable = getBit(mpLockOnNpc->mStatus.flag, 29);
            if (ImGui::Checkbox("motionProcessDisEnable", &motionProcessDisEnable)) setBit(mpLockOnNpc->mStatus.flag, 29, motionProcessDisEnable);
            bool downWaitStart = getBit(mpLockOnNpc->mStatus.flag, 30);
            if (ImGui::Checkbox("downWaitStart", &downWaitStart)) setBit(mpLockOnNpc->mStatus.flag, 30, downWaitStart);
            bool miniMapRender = getBit(mpLockOnNpc->mStatus.flag, 31);
            if (ImGui::Checkbox("miniMapRender", &miniMapRender)) setBit(mpLockOnNpc->mStatus.flag, 31, miniMapRender);
            bool normalClip = getBit(mpLockOnNpc->mStatus.flag2, 0);
            if (ImGui::Checkbox("normalClip", &normalClip)) setBit(mpLockOnNpc->mStatus.flag2, 0, normalClip);
            bool jpnDead = getBit(mpLockOnNpc->mStatus.flag2, 1);
            if (ImGui::Checkbox("jpnDead", &jpnDead)) setBit(mpLockOnNpc->mStatus.flag2, 1, jpnDead);
        }
    }
    static const char* HRZAKOWarningText = "if locked on enemy isn't a zako, you're on your own";
    if (ImGui::CollapsingHeader("mpLockOnNpc HRZAKO")) {
        help_marker(HRZAKOWarningText);
        HRZAKO* hrZako = (HRZAKO*)mpLockOnNpc;
        if (hrZako) {
            // For HRZAKO class
            ImGui::Checkbox("Landing Smoke", &hrZako->mRandingSmoke);
            help_marker("Smoke that emits when an enemy spawns.");
            ImGui::Checkbox("Start Attack", &hrZako->mStartAttack);
            ImGui::Checkbox("First Voice", &hrZako->mFirstVoice);
            help_marker("The initial quip said by an enemy when spawning.");
            ImGui::InputInt("Idle Motion", &hrZako->mIdleMot);
            help_marker("MotionID for enemy idle.");
            ImGui::InputInt("Standby Tick", &hrZako->mStandbyTick);
            ImGui::InputInt("Shot Number", &hrZako->mShotNum);
            help_marker("Displays the current shot number of the burst being fired. This is different each time a gunman attacks.");
            ImGui::InputInt("Shot Number Max", &hrZako->mShotNumMax);
            help_marker("Total number of bullets that will be fired for the current burst.");
            ImGui::InputInt("Reload Number", &hrZako->mReloadNum);
            help_marker("Maximum ammo capacity of enemy gun clip/magazine.");
            ImGui::InputInt("Ball Number", &hrZako->mBallNum);
            ImGui::InputInt("Ball Number Max", &hrZako->mBallNumMax);
            if (ImGui::CollapsingHeader("ZakoAi")) {
                ImGui::Combo("Mode", (int*)&hrZako->mAi.mMode, "Init\0Wait\0DarkSide\0");
                ImGui::InputInt("Recast Count", &hrZako->mAi.mRecastCnt);
                ImGui::InputInt("Attack Cast Count", &hrZako->mAi.mAtkCastCnt);
                ImGui::InputInt("Run Count", &hrZako->mAi.mRunCount);
                ImGui::InputInt("Asibumi Count", &hrZako->mAi.mAsibumiCount);
                ImGui::InputFloat3("Asibumi Start Pos", &hrZako->mAi.mAsibumiStartPos.x);
                ImGui::InputFloat("Asibumi Length", &hrZako->mAi.mAsibumiLength);
                ImGui::InputInt("Restore Direction Count", &hrZako->mAi.mRestoreDirecCount);
                ImGui::InputFloat("Boid Target Direction", &hrZako->mAi.mBoidTargetDirec);
                ImGui::InputFloat("Boid Now Direction", &hrZako->mAi.mBoidNowDirec);
                ImGui::Checkbox("Zako Boss", &hrZako->mAi.mZakoBoss);
                ImGui::InputInt("Dark Side Type", &hrZako->mAi.mDarkSideType);
                ImGui::InputInt("Dark Side Proc", &hrZako->mAi.mDarkSideProc);
                ImGui::InputInt("Dark Side Frame", &hrZako->mAi.mDarkSideFrame);
                ImGui::Checkbox("Down Attack Move", &hrZako->mAi.mDownAtkMove);
                ImGui::Checkbox("Long Attack Move", &hrZako->mAi.mLongAtkMove);
                help_marker("Tick to proc the ZAKO's special move. Usually their unblockable attack.");
                ImGui::InputInt("Long Attack Wait Count", &hrZako->mAi.mLongAtkWaitCount);
                ImGui::InputInt("Mawari Kinshi Count", &hrZako->mAi.mMawariKinshiCount);
                ImGui::Checkbox("Endurance", &hrZako->mAi.mEndurance);
                help_marker("Endurance is the mechanic that gives enemies extra resistance to attacks and immunity to Death Blow insta-kills. While only applied to specific enemies by default, this can be ticked on any Zako type.");
                ImGui::InputFloat("Mawarikomi Direction", &hrZako->mAi.mMawarikomiDirec);
                ImGui::Checkbox("Back Step Check", &hrZako->mAi.mBackStepChk);
                ImGui::Separator();
            }
            ImGui::InputFloat3("Boid Position", &hrZako->mBoidPos.x);
            ImGui::InputFloat("Walk Speed", &hrZako->mWalkSpeed);
            ImGui::InputFloat("Run Speed Rate", &hrZako->mRunSpeedRate);
            ImGui::InputScalar("Bit Move Update", ImGuiDataType_U8, &hrZako->mBitMoveUpDate);
            ImGui::InputFloat("Target Direction", &hrZako->mTargetDirec);
            ImGui::InputFloat("Now Direction Y", &hrZako->mNowDirecY);
            ImGui::InputInt("Basic Motion ID", &hrZako->mBasicMotionID);
            ImGui::InputInt("Damage Wait Tick", &hrZako->mDamageWaitTick);
            ImGui::Checkbox("Quick Stand Up", &hrZako->mQuickStandUp);
            ImGui::Checkbox("Down SE", &hrZako->mDownSE);
            ImGui::InputInt("Piyori Tick", &hrZako->mPiyoriTick);
            ImGui::InputInt("Lose Sight Tick", &hrZako->mLoseSightTick);
            ImGui::Checkbox("Damage Voice", &hrZako->mDmgVoice);
            ImGui::InputInt("Dead Model Type", &hrZako->mDeadModelType);
            ImGui::InputFloat("Piyori Store Damage", &hrZako->mPiyoriStoreDamage);
            ImGui::InputInt("Piyori Store Tick", &hrZako->mPiyoriStoreTick);
            ImGui::Checkbox("Already Hit Down Attack", &hrZako->mAlreadyHitDownAttack);
            if (ImGui::TreeNode("Union Fields")) {
                ImGui::InputInt("En Count Voice Tick", &hrZako->mEnCountVoiceTick);
                ImGui::Checkbox("AI Kyousei Up Guard", &hrZako->mAiKyouseiUpGuard);
                ImGui::Checkbox("AI Kyousei Down Guard", &hrZako->mAiKyouseiDownGuard);
                ImGui::Checkbox("AI Kyousei Rash", &hrZako->mAiKyouseiRash);
                ImGui::Checkbox("AI Event Run Mode", &hrZako->mAiEventRunMode);
                ImGui::InputFloat("AI Event Run Direction", &hrZako->mAiEventRunDirec);
                ImGui::Checkbox("AI Ikasama Throw Down", &hrZako->mAiIkasamaThrowDown);
                ImGui::Checkbox("AI Kyousei Guard Only", &hrZako->mAiKyouseiGuardOnly);
                ImGui::Checkbox("Money Far Too Little", &hrZako->mMoneyFarTooLittle);
                ImGui::Separator();
                ImGui::TreePop();
            }
        }
    }
    else
        help_marker(HRZAKOWarningText);
    static const char* HRJSTWarningText = "if locked on enemy isn't Destroybong, you're on your own";
    if (ImGui::CollapsingHeader("mpLockOnNpc HRJST")) {
        help_marker(HRJSTWarningText);
        HRJST* hrJst = (HRJST*)mpLockOnNpc;
        if (hrJst) {
            ImGui::InputInt("Action Mode", &hrJst->m_ActionMode);
            ImGui::InputFloat("Action Time", &hrJst->m_ActionTime);
            ImGui::InputFloat("Rand Time", &hrJst->m_RandTime);
            ImGui::InputFloat("Attack Wait", &hrJst->m_AttackWait);
            ImGui::InputInt("Motion Frame", &hrJst->m_MotionFrame);
            ImGui::InputFloat3("Move Vec", &hrJst->m_MoveVec.x);
            ImGui::InputFloat3("Tackle Move", &hrJst->m_TackleMove.x);
            ImGui::InputFloat3("Brain Pos", &hrJst->m_BrainPos.x);
            ImGui::Checkbox("Jump Enable", (bool*)&hrJst->m_JumpEnable);
            ImGui::Checkbox("Jump Flag", (bool*)&hrJst->m_JumpFlag);
            ImGui::Checkbox("Elevator", (bool*)&hrJst->m_Elevator);
            ImGui::InputInt("Pc Elect SEID", &hrJst->m_PcElectSEID);
            ImGui::InputInt("Eye Beam SEID", &hrJst->m_EyeBeamSEID);
            ImGui::InputInt("Beam Hit SEID", &hrJst->m_BeamHitSEID);
            ImGui::InputInt("Jst Beam SEID", &hrJst->m_JstBeamSEID);
            ImGui::InputInt("Demo Cut", &hrJst->m_DemoCut);

            ImGui::Checkbox("Demo", (bool*)&hrJst->m_Demo);
            ImGui::InputInt("Demo Count", &hrJst->m_DemoCount);
            for (int i = 0; i < 4; i++) {
                ImGui::Checkbox(("Lift[" + std::to_string(i) + "]").c_str(), (bool*)&hrJst->m_Lift[i]);
            }
            for (int i = 0; i < 3; i++) {
                ImGui::InputInt(("Rand Lift[" + std::to_string(i) + "]").c_str(), &hrJst->m_RandLift[i]);
            }
            ImGui::InputInt("Before Pc Pose", (int*)&hrJst->m_BeforePcPose);
            ImGui::InputInt("Camera End Wait", (int*)&hrJst->m_CameraEndWait);
            ImGui::Checkbox("Bef Guard", (bool*)&hrJst->m_BefGuard);
            ImGui::InputFloat3("Pc Position", &hrJst->m_PcPosition.x);
            ImGui::InputFloat3("Pc Navel", &hrJst->m_PcNavel.x);
            ImGui::InputFloat3("Pc Direction", &hrJst->m_PcDirection.x);
            ImGui::InputFloat("Pc Distance", &hrJst->m_PcDistance);
            ImGui::InputFloat("Pc Rand Time", &hrJst->m_PcRandTime);
            ImGui::Checkbox("Pc Share", (bool*)&hrJst->m_PcShare);
            ImGui::InputInt("Pc Motion No", &hrJst->m_PcMotionNo);
            ImGui::InputFloat("Hit Wait", &hrJst->m_HitWait);
            ImGui::InputInt("Sound ID", &hrJst->m_SoundID);
            ImGui::InputScalar("Right Eye Beam", ImGuiDataType_U32, &hrJst->m_pRightEyeBeam);
            ImGui::InputScalar("Left Eye Beam", ImGuiDataType_U32, &hrJst->m_pLeftEyeBeam);
            ImGui::InputScalar("HadoKen", ImGuiDataType_U32, &hrJst->m_pHadoKen);
            ImGui::InputScalar("Final Flash", ImGuiDataType_U32, &hrJst->m_pFinalFlash);
            for (int i = 0; i < 4; i++) {
                ImGui::InputScalar(("Lamp[" + std::to_string(i) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_pLamp[i]);
            }
            ImGui::InputScalar("Flash Blur", ImGuiDataType_U32, &hrJst->m_FlashBlur);
            ImGui::InputScalar("Elect", ImGuiDataType_U32, &hrJst->m_pElect);
            ImGui::InputScalar("Paund", ImGuiDataType_U32, &hrJst->m_pPaund);
            ImGui::InputInt("Right Eye Check", (int*)&hrJst->m_RightEyeCheck);
            ImGui::InputInt("Left Eye Check", (int*)&hrJst->m_LeftEyeCheck);
            ImGui::InputInt("Hadoken Check", (int*)&hrJst->m_HadokenCheck);
            ImGui::InputInt("Final Flash Check", (int*)&hrJst->m_FinalFlashCheck);
            ImGui::InputInt("Paund Check", (int*)&hrJst->m_PaundCheck);
            for (int i = 0; i < 4; i++) {
                ImGui::InputScalar(("Line Uni[" + std::to_string(i) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_LineUni[i]);
            }
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 4; j++) {
                    ImGui::InputScalar(("Line[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_Line[i][j]);
                }
            }
            ImGui::InputScalar("Demo Camera", ImGuiDataType_U32, &hrJst->m_DemoCamera);
            ImGui::InputScalar("Container A", ImGuiDataType_U32, &hrJst->m_ContainerA);
            ImGui::InputScalar("Container B", ImGuiDataType_U32, &hrJst->m_ContainerB);
            ImGui::InputScalar("Light", ImGuiDataType_U32, &hrJst->m_Light);
            ImGui::InputScalar("Container A Up", ImGuiDataType_U32, &hrJst->m_ContainerAUp);
            ImGui::InputScalar("Container B Up", ImGuiDataType_U32, &hrJst->m_ContainerBUp);
            ImGui::InputScalar("Container A Loop", ImGuiDataType_U32, &hrJst->m_ContainerALoop);
            ImGui::InputScalar("Container B Loop", ImGuiDataType_U32, &hrJst->m_ContainerBLoop);
            ImGui::InputScalar("Container A Down", ImGuiDataType_U32, &hrJst->m_ContainerADown);
            ImGui::InputScalar("Container B Down", ImGuiDataType_U32, &hrJst->m_ContainerBDown);
            ImGui::InputScalar("Light Up", ImGuiDataType_U32, &hrJst->m_LightUp);
            ImGui::InputScalar("Light Loop", ImGuiDataType_U32, &hrJst->m_LightLoop);
            ImGui::InputScalar("Light Down", ImGuiDataType_U32, &hrJst->m_LightDown);
        }
    }
    if (ImGui::CollapsingHeader("mpLockOnNpc mEffect.pBattleIcon")) {
        if (mpLockOnNpc && mpLockOnNpc->mEffect.pBattleIcon) {
            uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mEffect.pBattleIcon->Padding_175);
            uintptr_t statusAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mEffect.pBattleIcon->m_DrawHitCmbFlag);
            uintptr_t offsetDifference = statusAddress - baseAddress;
            ImGui::Text("Offset: 0x%08X", offsetDifference);

            ImGui::InputScalar("Icon Stat", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->D_BICON_STAT);
            ImGui::InputScalar("Yoyaku Icon", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_YoyakuIcon);
            ImGui::InputScalar("Lock On Distance", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_LockOnDist);
            ImGui::InputScalar("Lock On Circle Size", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_LockOnCircleSize);
            ImGui::InputScalar("Block Size", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_BIcon_Block_Size);
            ImGui::InputScalarN("Block Color", ImGuiDataType_U8, mpLockOnNpc->mEffect.pBattleIcon->m_blockcolor, 3);
            ImGui::InputScalar("Color Type", ImGuiDataType_U8, &mpLockOnNpc->mEffect.pBattleIcon->m_ColorType);
            ImGui::InputScalarN("Con Act ID", ImGuiDataType_S32, mpLockOnNpc->mEffect.pBattleIcon->m_ConActID, 2);
            ImGui::InputScalarN("Con Act Count", ImGuiDataType_S16, mpLockOnNpc->mEffect.pBattleIcon->m_ConActCount, 2);
            ImGui::InputScalar("Counter", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_Counter);
            ImGui::InputScalar("Del Counter", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_DelCounter);
            help_marker("Setting this value begins the timer for how long the lock on blocks should spin.");
            ImGui::InputScalar("Arrow Appear Count", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_Arrow_AppearCnt);
            ImGui::InputScalar("Rotate Anim Count", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_Rotate_AnimCnt);
            ImGui::InputScalar("Tmp Jst Vec X", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.x);
            ImGui::InputScalar("Tmp Jst Vec Y", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.y);
            ImGui::InputScalar("Tmp Jst Vec Z", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_TmpJstVec.z);
            ImGui::InputScalar("Pyoko Count", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_PyokoCnt);
            ImGui::InputScalar("Pyoko Flag", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_PyokoFlag);
            ImGui::InputScalar("Pyoko Ratio", ImGuiDataType_Float, &mpLockOnNpc->mEffect.pBattleIcon->m_PyokoRatio);
            ImGui::InputScalar("Pyoko Alpha", ImGuiDataType_U8, &mpLockOnNpc->mEffect.pBattleIcon->m_PyokoAlpha);
            ImGui::Checkbox("Draw Hit Combo Flag", (bool*)&mpLockOnNpc->mEffect.pBattleIcon->m_DrawHitCmbFlag);
            for (int i = 0; i < 36; ++i) {
                ImGui::InputFloat(fmt::format("Tsuba Ratio [{}]", i).c_str(), &mpLockOnNpc->mEffect.pBattleIcon->m_TsubaRatio[i]);
            }
            for (int i = 0; i < 2; ++i) {
                ImGui::Text("Position %d: %p", i, mpLockOnNpc->mEffect.pBattleIcon->m_pPosition[i]);
            }
            ImGui::Text("Node: %p", mpLockOnNpc->mEffect.pBattleIcon->m_pNode);
            for (int i = 0; i < 3; ++i) {
                ImGui::InputScalarN(fmt::format("Color [{}]", i).c_str(), ImGuiDataType_U8, mpLockOnNpc->mEffect.pBattleIcon->m_Color[i], 2);
            }
            ImGui::InputScalar("HP Bar Counter", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_HpBerCounter);
            ImGui::InputScalar("Line Counter", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_LineCounter);
            ImGui::InputScalar("Meter Counter", ImGuiDataType_S8, &mpLockOnNpc->mEffect.pBattleIcon->m_MeterCounter);
            ImGui::InputScalarN("Tension", ImGuiDataType_Float, mpLockOnNpc->mEffect.pBattleIcon->m_Tension, 2);
            ImGui::InputScalarN("T Meter Rev Position", ImGuiDataType_Float, mpLockOnNpc->mEffect.pBattleIcon->m_TMeterRevPos, 2);
            ImGui::InputScalarN("Get Money", ImGuiDataType_S16, mpLockOnNpc->mEffect.pBattleIcon->m_GetMoney, 2);
            ImGui::InputScalarN("Hit Number", ImGuiDataType_S16, mpLockOnNpc->mEffect.pBattleIcon->m_HitNum, 2);
            ImGui::InputScalarN("HP", ImGuiDataType_Float, mpLockOnNpc->mEffect.pBattleIcon->m_HP, 3);
            ImGui::InputScalar("HP Base Alpha", ImGuiDataType_U8, &mpLockOnNpc->mEffect.pBattleIcon->m_HPBaseAlpha);
            ImGui::InputScalar("HP Virt Alpha", ImGuiDataType_U8, &mpLockOnNpc->mEffect.pBattleIcon->m_HPVirtAlpha);
            ImGui::InputScalarN("Button Counter", ImGuiDataType_S16, mpLockOnNpc->mEffect.pBattleIcon->m_BottanCounter, 4);
            ImGui::InputScalar("Target Icon Count", ImGuiDataType_S16, &mpLockOnNpc->mEffect.pBattleIcon->m_TergetIconCount);
            ImGui::InputScalar("Direct", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_Direct);
            ImGui::InputScalar("Angle", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_Angle);
            ImGui::InputScalar("Sound ID", ImGuiDataType_S32, &mpLockOnNpc->mEffect.pBattleIcon->m_Soundid);
            bool fdrawbase = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 0);
            if (ImGui::Checkbox("Draw Base", &fdrawbase)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 0, fdrawbase);
            bool fdrawmoney = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 1);
            if (ImGui::Checkbox("Draw Money", &fdrawmoney)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 1, fdrawmoney);
            bool fdrawtension = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 2);
            if (ImGui::Checkbox("Draw Tension", &fdrawtension)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 2, fdrawtension);
            bool fdrawheart = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 3);
            if (ImGui::Checkbox("Draw Heart", &fdrawheart)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 3, fdrawheart);
            bool fdrawterget = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 4);
            if (ImGui::Checkbox("Draw Target", &fdrawterget)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 4, fdrawterget);
            bool fdrawhp = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 5);
            if (ImGui::Checkbox("Draw HP", &fdrawhp)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 5, fdrawhp);
            bool fdraw_word = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 6);
            if (ImGui::Checkbox("Draw Word", &fdraw_word)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 6, fdraw_word);
            bool fhptype = getBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 10);
            if (ImGui::Checkbox("HP Type", &fhptype)) setBit(mpLockOnNpc->mEffect.pBattleIcon->flag, 10, fhptype);
        }
    }
    if (ImGui::CollapsingHeader("mpLockOnNpc mStatus.dmgInfo")) {
        if (mpLockOnNpc) {
            ImGui::SliderFloat("Damage", &mpLockOnNpc->mStatus.dmgInfo.dmg, 0.0f, 100.0f);
            ImGui::InputInt("Damage Motion", &mpLockOnNpc->mStatus.dmgInfo.dmgMot);
            ImGui::InputInt("Guard Motion", &mpLockOnNpc->mStatus.dmgInfo.grdMot);
            ImGui::InputInt("Attack Motion", &mpLockOnNpc->mStatus.dmgInfo.atkMot);
            ImGui::SliderFloat("Damage Direction", &mpLockOnNpc->mStatus.dmgInfo.dmgDirec, -360.0f, 360.0f);
            ImGui::SliderFloat("Knockback Distance", &mpLockOnNpc->mStatus.dmgInfo.nockBackDst, 0.0f, 100.0f);
            ImGui::SliderFloat("Upper Power", &mpLockOnNpc->mStatus.dmgInfo.upperPow, 0.0f, 100.0f);
            ImGui::SliderFloat("Upper Position Y", &mpLockOnNpc->mStatus.dmgInfo.upperPosY, -50.0f, 50.0f);
            ImGui::SliderFloat("Gravity", &mpLockOnNpc->mStatus.dmgInfo.grav, 0.0f, 20.0f);
            ImGui::Checkbox("Upper", &mpLockOnNpc->mStatus.dmgInfo.upper);
            ImGui::InputInt("Tick", &mpLockOnNpc->mStatus.dmgInfo.tick);
            ImGui::SliderFloat("Air Blow Position X", &mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.x, -100.0f, 100.0f);
            ImGui::SliderFloat("Air Blow Position Y", &mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.y, -100.0f, 100.0f);
            ImGui::SliderFloat("Air Blow Position Z", &mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPos.z, -100.0f, 100.0f);
            ImGui::SliderFloat("Air Blow Power", &mpLockOnNpc->mStatus.dmgInfo.m_AirBlowPower, 0.0f, 100.0f);
            ImGui::Checkbox("Air Flag", &mpLockOnNpc->mStatus.dmgInfo.m_AirFlag);
            ImGui::SliderFloat("Gravity Acceleration", &mpLockOnNpc->mStatus.dmgInfo.m_GravAccele, 0.0f, 20.0f);
            ImGui::InputScalar("Piyo Request", ImGuiDataType_S8, &mpLockOnNpc->mStatus.dmgInfo.m_PiyoRequest);
            ImGui::SliderFloat("Stored Damage", &mpLockOnNpc->mStatus.dmgInfo.storeDamage, 0.0f, 100.0f);
            ImGui::SliderFloat("Stored Damage Distance", &mpLockOnNpc->mStatus.dmgInfo.storeDamageDst, 0.0f, 100.0f);
            ImGui::InputInt("Restore Damage Tick", &mpLockOnNpc->mStatus.dmgInfo.restoreDamegeTick);
            ImGui::InputInt("Restore Damage Basic Tick", &mpLockOnNpc->mStatus.dmgInfo.restoreDamegeBasicTick);
            ImGui::InputScalar("Bike Dead Request", ImGuiDataType_S8, &mpLockOnNpc->mStatus.dmgInfo.m_BikeDeadRequest);
        }
    }
}

void EnemyTracker::on_draw_ui() {
    ImGui::Separator();
    ImGui::Text("Enemy");
    ImGui::Checkbox("Enemy Stats Popout", &imguiPopout);
    if (!imguiPopout)
        DrawEnemyStats();
}

void EnemyTracker::custom_imgui_window() {
    if (imguiPopout) {
        ImGui::Begin("Enemy Stats", &imguiPopout);
        DrawEnemyStats();
        ImGui::End();
    }
}

// during load
//void EnemyTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void EnemyTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
//void EnemyTracker::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void EnemyTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
