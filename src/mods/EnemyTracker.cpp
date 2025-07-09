#include "EnemyTracker.hpp"
#include "GuiFunctions.hpp" // for g_framework
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

const char* GHMR_TEXADDRESS_NAMES[5] {
    "GHMR_TEXADDRESS_CLAMP", // = 0,
    "GHMR_TEXADDRESS_REPEAT", // = 1,
    "GHMR_TEXADDRESS_MIRROR", // = 2,
    "GHMR_TEXADDRESS_REPEAT_FORCED", // = 3,
    "GHMR_TEXADDRESS_MIRROR_FORCED", // = 4,
};
const char* GHMR_TEXFILTER_NAMES[2] {
    "GHMR_TEXFILTER_NEAR", // = 0,
    "GHMR_TEXFILTER_LINEAR", // = 1,
};

void DrawEnemyStats() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    static mHRChara* mpLockOnNpc = NULL;

    static int currentSelectedEnemy = 0;
    static bool useCurrentSelectedEnemySlider = false;
    ImGui::Checkbox("Select Enemy With Slider", &useCurrentSelectedEnemySlider);
    if (useCurrentSelectedEnemySlider) {
        ImGui::SliderInt("Current Enemy", &currentSelectedEnemy, 0, 30);
        mpLockOnNpc = nmh_sdk::get_mHRBattle()->mpNpc[currentSelectedEnemy];
        float combo_width = ImGui::CalcItemWidth();
        if (ImGui::Button("Find Locked On Enemy In List", ImVec2(combo_width, NULL))) {
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

    // attach the menu to enemies
    /*if (mpLockOnNpc) {
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        Vec playerPos = mpLockOnNpc->mStatus.pos;
        //playerPos.y += 10.0f;
        Vec screenPlayerPos{ 0.0f, 0.0f, 0.0f };
        nmh_sdk::GetScreenPos(&playerPos, &screenPlayerPos);
        float scaleX = screenSize.x / 854.0f;
        float scaleY = screenSize.y / 480.0f;
        ImVec2 scaledPos(screenPlayerPos.x * scaleX, screenPlayerPos.y * scaleY);
        ImGui::SetWindowPos(ImVec2(scaledPos.x, scaledPos.y));
    }*/

    if (ImGui::TreeNodeEx("mpLockOnNpc", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (mpLockOnNpc) {
            ImGui::InputFloat("mDistFromPc", &mpLockOnNpc->mDistFromPc);
            ImGui::InputFloat("mDirecYFromPc", &mpLockOnNpc->mDirecYFromPc);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("mpLockOnNpc mStatus", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
        ImGui::TreePop();
    }
    static const char* HRZAKOWarningText = "if locked on enemy isn't a zako, you're on your own";
    if (ImGui::TreeNodeEx("mpLockOnNpc HRZAKO", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
            if (ImGui::TreeNodeEx("ZakoAi", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                ImGui::TreePop();
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
            if (ImGui::TreeNodeEx("Union Fields", ImGuiTreeNodeFlags_DrawLinesFull)) {
                ImGui::InputInt("En Count Voice Tick", &hrZako->mEnCountVoiceTick);
                ImGui::Checkbox("AI Kyousei Up Guard", &hrZako->mAiKyouseiUpGuard);
                ImGui::Checkbox("AI Kyousei Down Guard", &hrZako->mAiKyouseiDownGuard);
                ImGui::Checkbox("AI Kyousei Rash", &hrZako->mAiKyouseiRash);
                ImGui::Checkbox("AI Event Run Mode", &hrZako->mAiEventRunMode);
                ImGui::InputFloat("AI Event Run Direction", &hrZako->mAiEventRunDirec);
                ImGui::Checkbox("AI Ikasama Throw Down", &hrZako->mAiIkasamaThrowDown);
                ImGui::Checkbox("AI Kyousei Guard Only", &hrZako->mAiKyouseiGuardOnly);
                ImGui::Checkbox("Money Far Too Little", &hrZako->mMoneyFarTooLittle);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("mpLockOnNpc mEffect.pBattleIcon", ImGuiTreeNodeFlags_DrawLinesFull)) {
        if (mpLockOnNpc && mpLockOnNpc->mEffect.pBattleIcon) {
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mEffect.pBattleIcon->Padding_175);
            // uintptr_t statusAddress = reinterpret_cast<uintptr_t>(&mpLockOnNpc->mEffect.pBattleIcon->m_DrawHitCmbFlag);
            // uintptr_t offsetDifference = statusAddress - baseAddress;
            // ImGui::Text("Offset: 0x%08X", offsetDifference);

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
                ImGui::InputFloat(("Tsuba Ratio [" + std::to_string(i) + "]").c_str(), &mpLockOnNpc->mEffect.pBattleIcon->m_TsubaRatio[i]);
            }
            for (int i = 0; i < 2; ++i) {
                ImGui::Text("Position %d: %p", i, mpLockOnNpc->mEffect.pBattleIcon->m_pPosition[i]);
            }
            ImGui::Text("Node: %p", mpLockOnNpc->mEffect.pBattleIcon->m_pNode);
            for (int i = 0; i < 3; ++i) {
                ImGui::InputScalarN(("Color [" + std::to_string(i) + "]").c_str(), ImGuiDataType_U8, mpLockOnNpc->mEffect.pBattleIcon->m_Color[i], 2);
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
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("mpLockOnNpc mStatus.dmgInfo", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
        ImGui::TreePop();
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeMAM) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRMAM (Death Metal)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRMAM* hrMam = (HRMAM*)mpLockOnNpc;
            if (hrMam) {
                ImGui::InputInt("Action Mode", &hrMam->m_ActionMode);
                ImGui::InputFloat("Action Time", &hrMam->m_ActionTime, 0.1f, 1.0f, "%.3f");
                ImGui::InputFloat("Rand Time", &hrMam->m_RandTime, 0.1f, 1.0f, "%.3f");
                ImGui::InputFloat("Attack Wait", &hrMam->m_AttackWait, 0.1f, 1.0f, "%.3f");
                ImGui::InputInt("Motion Frame", &hrMam->m_MotionFrame);
                ImGui::InputFloat3("Move Vector", &hrMam->m_MoveVec.x);
                ImGui::InputFloat3("Tackle Move", &hrMam->m_TackleMove.x);
                ImGui::InputScalar("Tubazeriai Count", ImGuiDataType_S16, &hrMam->m_TubazeriaiCount);
                ImGui::InputInt("From Start Tick", &hrMam->m_FromStartTick);
                ImGui::Combo("Before PC Pose", (int*)&hrMam->m_BeforePcPose, "ePcPoseUpper\0ePcPoseMiddle\0ePcPoseBottom\0ePcPoseMax\0");
                unsigned short& flags = hrMam->flag;
                bool fAtkTackleReq = getBit(flags, 0);
                if (ImGui::Checkbox("Atk Tackle Req", &fAtkTackleReq)) setBit(flags, 0, fAtkTackleReq);
                bool fAtkThrowReq = getBit(flags, 1);
                if (ImGui::Checkbox("Atk Throw Req", &fAtkThrowReq)) setBit(flags, 1, fAtkThrowReq);
                bool fAtkBunsinReq = getBit(flags, 2);
                if (ImGui::Checkbox("Atk Bunsin Req", &fAtkBunsinReq)) setBit(flags, 2, fAtkBunsinReq);
                bool fBackStepCount = getBit(flags, 3);
                if (ImGui::Checkbox("Back Step Count", &fBackStepCount)) setBit(flags, 3, fBackStepCount);
                bool fSetBunsinVec = getBit(flags, 4);
                if (ImGui::Checkbox("Set Bunsin Vec", &fSetBunsinVec)) setBit(flags, 4, fSetBunsinVec);
                bool fCreateBunsin = getBit(flags, 5);
                if (ImGui::Checkbox("Create Bunsin", &fCreateBunsin)) setBit(flags, 5, fCreateBunsin);
                bool fDeadOK = getBit(flags, 6);
                if (ImGui::Checkbox("Dead OK", &fDeadOK)) setBit(flags, 6, fDeadOK);
                bool fIdolRot = getBit(flags, 7);
                if (ImGui::Checkbox("Idol Rot", &fIdolRot)) setBit(flags, 7, fIdolRot);
                bool fPcThowed = getBit(flags, 8);
                if (ImGui::Checkbox("PC Thowed", &fPcThowed)) setBit(flags, 8, fPcThowed);
                bool fStartMonolog = getBit(flags, 9);
                if (ImGui::Checkbox("Start Monolog", &fStartMonolog)) setBit(flags, 9, fStartMonolog);
                ImGui::Combo("Genki", (int*)&hrMam->m_Genki, "E_MAM_STAT_100\0E_MAM_STAT_60\0E_MAM_STAT_30\0");
                ImGui::InputInt2("Bunsin ID", hrMam->m_BunsinId);
                ImGui::InputInt("Loop Sound ID", &hrMam->m_LoopSoundID);
                ImGui::InputInt("Tuba SE Wait", &hrMam->m_TubaSEWait);
                ImGui::InputScalar("Suki Counter", ImGuiDataType_S16, &hrMam->m_SukiCounter);
                for (int i = 0; i < 13; ++i) {
                    ImGui::InputFloat3(("Away Pos " + std::to_string(i)).c_str(), &hrMam->m_AwayPos[i].x);
                }
                for (int i = 0; i < 4; ++i) {
                    ImGui::InputFloat3(("Wall Pos " + std::to_string(i)).c_str(), &hrMam->m_WallPos[i].x);
                }
                ImGui::InputInt("Away Pos Index", &hrMam->m_AwayPosIndex);
                ImGui::Checkbox("First Bunsin", &hrMam->m_FirstBunsin);
                ImGui::Checkbox("Before Guard", &hrMam->m_BefGuard);
                ImGui::InputFloat3("PC Position", &hrMam->m_PcPosition.x);
                ImGui::InputFloat3("PC Navel", &hrMam->m_PcNavel.x);
                ImGui::InputFloat3("PC Direction", &hrMam->m_PcDirection.x);
                ImGui::InputFloat("PC Distance", &hrMam->m_PcDistance, 0.1f, 1.0f, "%.3f");
                ImGui::InputFloat("PC Rand Time", &hrMam->m_PcRandTime, 0.1f, 1.0f, "%.3f");
                ImGui::InputFloat("Inner", &hrMam->m_Inner, 0.1f, 1.0f, "%.3f");
                ImGui::Checkbox("PC Share", &hrMam->m_PcShare);
                ImGui::InputInt("PC Motion No", &hrMam->m_PcMotionNo);
                ImGui::InputFloat("Hit Wait", &hrMam->m_HitWait, 0.1f, 1.0f, "%.3f");
                ImGui::Combo("Before PC Pose", (int*)&hrMam->m_OldPcPose, "ePcPoseUpper\0ePcPoseMiddle\0ePcPoseBottom\0ePcPoseMax\0");
                ImGui::InputInt("Monolog ID", &hrMam->m_MonoLogID);
                ImGui::Text("Trace Texture");
                ImGui::Checkbox("mTraceTex TME Flag", &hrMam->mTraceTex.TMEFlag);
                ImGui::Checkbox("mTraceTex Alpha Flag", &hrMam->mTraceTex.AlphaFlag);
                ImGui::Checkbox("mTraceTex Image Buffer Free Flag", &hrMam->mTraceTex.ImageBufferFreeFlag);
                ImGui::InputInt("mTraceTex File Image Ptr", (int*)&hrMam->mTraceTex.FileImage);
                ImGui::InputInt("mTraceTex Image Buffer Ptr", (int*)&hrMam->mTraceTex.ImageBuffer);
                ImGui::InputScalar("mTraceTex Width", ImGuiDataType_U16, &hrMam->mTraceTex.Width);
                ImGui::InputScalar("mTraceTex Height", ImGuiDataType_U16, &hrMam->mTraceTex.Height);
                ImGui::InputInt("mTraceTex TexObj Ptr", (int*)&hrMam->mTraceTex.TexObj);
                ImGui::Combo("mTraceTex Address Mode", (int*)&hrMam->mTraceTex.AddressMode, GHMR_TEXADDRESS_NAMES, IM_ARRAYSIZE(GHMR_TEXADDRESS_NAMES));
                ImGui::Combo("mTraceTex Filter Mode", (int*)&hrMam->mTraceTex.FilterMode, GHMR_TEXFILTER_NAMES, IM_ARRAYSIZE(GHMR_TEXFILTER_NAMES));
                ImGui::InputInt("mTraceTex Get Screen Image Format Enum", (int*)&hrMam->mTraceTex.GetScreenImageFormat);
                ImGui::InputInt("mTraceTex Image Size", (int*)&hrMam->mTraceTex.ImageSize);
                ImGui::InputInt("mTraceTex Aspect Enum", (int*)&hrMam->mTraceTex.Aspect);
                ImGui::Text("Slash Texture");
                ImGui::Checkbox("m_SlashTex TME Flag", &hrMam->m_SlashTex.TMEFlag);
                ImGui::Checkbox("m_SlashTex Alpha Flag", &hrMam->m_SlashTex.AlphaFlag);
                ImGui::Checkbox("m_SlashTex Image Buffer Free Flag", &hrMam->m_SlashTex.ImageBufferFreeFlag);
                ImGui::InputInt("m_SlashTex File Image Ptr", (int*)&hrMam->m_SlashTex.FileImage);
                ImGui::InputInt("m_SlashTex Image Buffer Ptr", (int*)&hrMam->m_SlashTex.ImageBuffer);
                ImGui::InputScalar("m_SlashTex Width", ImGuiDataType_U16, &hrMam->m_SlashTex.Width);
                ImGui::InputScalar("m_SlashTex Height", ImGuiDataType_U16, &hrMam->m_SlashTex.Height);
                ImGui::InputInt("m_SlashTex TexObj Ptr", (int*)&hrMam->m_SlashTex.TexObj);
                ImGui::Combo("m_SlashTex Address Mode", (int*)&hrMam->m_SlashTex.AddressMode, GHMR_TEXADDRESS_NAMES, IM_ARRAYSIZE(GHMR_TEXADDRESS_NAMES));
                ImGui::Combo("m_SlashTex Filter Mode", (int*)&hrMam->m_SlashTex.FilterMode, GHMR_TEXFILTER_NAMES, IM_ARRAYSIZE(GHMR_TEXFILTER_NAMES));
                ImGui::InputInt("m_SlashTex Get Screen Image Format Enum", (int*)&hrMam->m_SlashTex.GetScreenImageFormat);
                ImGui::InputInt("m_SlashTex Image Size", (int*)&hrMam->m_SlashTex.ImageSize);
                ImGui::InputInt("m_SlashTex Aspect Enum", (int*)&hrMam->m_SlashTex.Aspect);
                if (hrMam->m_pSwordLaser) {
                    ImGui::Text("Sword Laser Pointer: %p", hrMam->m_pSwordLaser);
                }
                ImGui::InputInt("Monolog Handle", (int*)&hrMam->m_MonologHandle);
                if (hrMam->m_pDebugAtkCheck) {
                    ImGui::Text("Debug Attack Check Pointer: %p", hrMam->m_pDebugAtkCheck);
                }
                ImGui::InputInt("Camera Mode", &hrMam->m_CameraMode);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeMAM_Jr) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRMAMJr (Death Metal Clone)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRMAMJR* hrMamJr = (HRMAMJR*)mpLockOnNpc;
            if (hrMamJr) {
                ImGui::InputInt("m_ActionMode", &hrMamJr->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrMamJr->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrMamJr->m_RandTime);
                ImGui::InputFloat("m_AttackWait", &hrMamJr->m_AttackWait);
                ImGui::InputInt("m_MotionFrame", &hrMamJr->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrMamJr->m_MoveVec.x);
                ImGui::InputFloat3("m_TackleMove", &hrMamJr->m_TackleMove.x);
                ImGui::Checkbox("m_BackStepCount", &hrMamJr->m_BackStepCount);
                ImGui::Checkbox("m_InitAtack", &hrMamJr->m_InitAtack);
                ImGui::InputScalar("m_DeadCounter", ImGuiDataType_S16, &hrMamJr->m_DeadCounter);
                ImGui::InputFloat3("m_EnPosition", &hrMamJr->m_EnPosition.x);
                ImGui::InputFloat3("m_EnDirection", &hrMamJr->m_EnDirection.x);
                ImGui::InputFloat("m_EnDistance", &hrMamJr->m_EnDistance);
                ImGui::InputFloat3("m_PcPosition", &hrMamJr->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrMamJr->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrMamJr->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrMamJr->m_PcDistance);
                ImGui::InputFloat("m_PcRandTime", &hrMamJr->m_PcRandTime);
                ImGui::InputFloat("m_Inner", &hrMamJr->m_Inner);
                ImGui::Checkbox("m_PcShare", &hrMamJr->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrMamJr->m_PcMotionNo);
                ImGui::InputFloat("m_HitWait", &hrMamJr->m_HitWait);
                ImGui::Checkbox("mTraceTex TME Flag", &hrMamJr->mTraceTex.TMEFlag);
                ImGui::Checkbox("mTraceTex Alpha Flag", &hrMamJr->mTraceTex.AlphaFlag);
                ImGui::Checkbox("mTraceTex Image Buffer Free Flag", &hrMamJr->mTraceTex.ImageBufferFreeFlag);
                ImGui::InputInt("mTraceTex File Image Ptr", (int*)&hrMamJr->mTraceTex.FileImage);
                ImGui::InputInt("mTraceTex Image Buffer Ptr", (int*)&hrMamJr->mTraceTex.ImageBuffer);
                ImGui::InputScalar("mTraceTex Width", ImGuiDataType_U16, &hrMamJr->mTraceTex.Width);
                ImGui::InputScalar("mTraceTex Height", ImGuiDataType_U16, &hrMamJr->mTraceTex.Height);
                ImGui::InputInt("mTraceTex TexObj Ptr", (int*)&hrMamJr->mTraceTex.TexObj);
                ImGui::Combo("mTraceTex Address Mode", (int*)&hrMamJr->mTraceTex.AddressMode, GHMR_TEXADDRESS_NAMES, IM_ARRAYSIZE(GHMR_TEXADDRESS_NAMES));
                ImGui::Combo("mTraceTex Filter Mode", (int*)&hrMamJr->mTraceTex.FilterMode, GHMR_TEXFILTER_NAMES, IM_ARRAYSIZE(GHMR_TEXFILTER_NAMES));
                ImGui::InputInt("mTraceTex Get Screen Image Format Enum", (int*)&hrMamJr->mTraceTex.GetScreenImageFormat);
                ImGui::InputInt("mTraceTex Image Size Enum", (int*)&hrMamJr->mTraceTex.ImageSize);
                ImGui::InputInt("mTraceTex Aspect Enum", (int*)&hrMamJr->mTraceTex.Aspect);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeMGE) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRMGE (Dr. Peace)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRMGE* hrMge = (HRMGE*)mpLockOnNpc;
            if (hrMge) {
                ImGui::InputInt("m_ActionMode", &hrMge->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrMge->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrMge->m_RandTime);
                ImGui::InputFloat("m_AttackWait", &hrMge->m_AttackWait);
                ImGui::InputInt("m_MotionFrame", &hrMge->m_MotionFrame);
                ImGui::InputInt("m_DamageAccum", &hrMge->m_DamageAccum);
                ImGui::InputFloat("m_RotDiv", &hrMge->m_RotDiv);
                ImGui::Checkbox("m_SongMode", &hrMge->m_SongMode);
                ImGui::InputFloat3("m_MoveVec", &hrMge->m_MoveVec.x);
                ImGui::InputFloat3("m_MoundPos", &hrMge->m_MoundPos.x);
                ImGui::InputInt("m_RandKey", &hrMge->m_RandKey);
                ImGui::InputFloat3("m_BeforePos", &hrMge->m_BeforePos.x);
                ImGui::InputInt("m_WepNum", &hrMge->m_WepNum);
                ImGui::Checkbox("m_SongWait", &hrMge->m_SongWait);
                ImGui::Combo("m_BeforePcPose", (int*)&hrMge->m_BeforePcPose, "ePcPoseUpper\0ePcPoseMiddle\0ePcPoseBottom\0ePcPoseMax\0");
                ImGui::InputInt("m_BGMHandle", &hrMge->m_BGMHandle);
                ImGui::InputInt("m_SubBGMHandle", &hrMge->m_SubBGMHandle);
                ImGui::Combo("m_SetDemoNum", (int*)&hrMge->m_SetDemoNum, "DemoInit\0ShotAIn\0ShotAOut\0ShotBIn\0ShotBOut\0ShotCIn\0ShotCOut\0ShotDIn\0ShotDOut\0ShortShotIn\0");
                ImGui::Checkbox("m_FirstDemoIn", &hrMge->m_FirstDemoIn);
                ImGui::InputInt("m_FadeCount", &hrMge->m_FadeCount);
                ImGui::InputFloat("m_ReloadSpeed", &hrMge->m_ReloadSpeed);
                ImGui::InputFloat("m_BackDmgCount", &hrMge->m_BackDmgCount);
                ImGui::Checkbox("m_BefGuard", &hrMge->m_BefGuard);
                ImGui::InputFloat("m_FirtRotDamage", &hrMge->m_FirtRotDamage);
                ImGui::InputFloat3("m_PcPosition", &hrMge->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrMge->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrMge->m_PcDirection.x);
                ImGui::InputFloat3("m_PcBeforePos", &hrMge->m_PcBeforePos.x);
                ImGui::InputFloat("m_PcDistance", &hrMge->m_PcDistance);
                ImGui::InputFloat("m_PcRotY", &hrMge->m_PcRotY);
                ImGui::InputFloat("m_PcRandTime", &hrMge->m_PcRandTime);
                ImGui::InputFloat("m_HitWait", &hrMge->m_HitWait);
                ImGui::InputInt("m_Battery", &hrMge->m_Battery);
                for (int i = 0; i < 20; ++i) {
                    ImGui::InputInt(("m_pCart Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrMge->m_pCart[i]);
                    ImGui::InputInt(("m_Gun Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrMge->m_Gun[i]);
                }
                ImGui::InputInt("m_pRightGunGmf Ptr", (int*)&hrMge->m_pRightGunGmf);
                ImGui::InputInt("m_pLeftGunGmf Ptr", (int*)&hrMge->m_pLeftGunGmf);
                ImGui::InputInt("m_pMicGmf Ptr", (int*)&hrMge->m_pMicGmf);
                ImGui::InputInt("m_ShootFrame", &hrMge->m_ShootFrame);
                ImGui::Combo("m_ShootMode", (int*)&hrMge->m_ShootMode, "InitShoot\0RightShoot\0LeftShoot\0");
                ImGui::InputFloat3("m_RightGunPos", &hrMge->m_RightGunPos.x);
                ImGui::InputFloat3("m_LeftGunPos", &hrMge->m_LeftGunPos.x);
                ImGui::InputFloat3("m_RightGunCart", &hrMge->m_RightGunCart.x);
                ImGui::InputFloat3("m_LeftGunCart", &hrMge->m_LeftGunCart.x);
                ImGui::InputFloat3("m_MicPos", &hrMge->m_MicPos.x);
                for (int i = 0; i < 20; ++i) {
                    ImGui::InputInt(("m_CartCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrMge->m_CartCheck[i]);
                    ImGui::InputInt(("m_GunCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrMge->m_GunCheck[i]);
                }
                ImGui::InputInt("m_Screen Ptr", (int*)&hrMge->m_Screen);
                ImGui::InputInt("m_PrimUni Ptr", (int*)&hrMge->m_PrimUni);
                ImGui::InputInt("m_Quad Ptr", (int*)&hrMge->m_Quad);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeTGR) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRTGR (Shinobu)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRTGR* hrTgr = (HRTGR*)mpLockOnNpc;
            if (hrTgr) {
                ImGui::Checkbox("mStartAttack", &hrTgr->mStartAttack);
                ImGui::Checkbox("mAlreadyStandby", &hrTgr->mAlreadyStandby);
                ImGui::Checkbox("mGuardMode", &hrTgr->mGuardMode);
                ImGui::InputScalar("mRanbuRateOffset", ImGuiDataType_S16, &hrTgr->mRanbuRateOffset);
                ImGui::InputScalar("mRapidStepNum", ImGuiDataType_U8, &hrTgr->mRapidStepNum);
                ImGui::InputScalar("mRapidSonicNum", ImGuiDataType_U8, &hrTgr->mRapidSonicNum);
                ImGui::InputInt("mStandbyTick", &hrTgr->mStandbyTick);
                ImGui::InputFloat("mPiyoriFrame", &hrTgr->mPiyoriFrame);
                ImGui::InputInt("mGuardTick", &hrTgr->mGuardTick);
                ImGui::Checkbox("m_BefGuard", &hrTgr->m_BefGuard);
                bool mAdvAtk0 = getBit(hrTgr->mFlag, 0);
                if (ImGui::Checkbox("mAdvAtk0", &mAdvAtk0)) setBit(hrTgr->mFlag, 0, mAdvAtk0);
                bool mAdvAtk1 = getBit(hrTgr->mFlag, 1);
                if (ImGui::Checkbox("mAdvAtk1", &mAdvAtk1)) setBit(hrTgr->mFlag, 1, mAdvAtk1);
                bool mAdvAtk2 = getBit(hrTgr->mFlag, 2);
                if (ImGui::Checkbox("mAdvAtk2", &mAdvAtk2)) setBit(hrTgr->mFlag, 2, mAdvAtk2);
                bool mBlwAtk = getBit(hrTgr->mFlag, 3);
                if (ImGui::Checkbox("mBlwAtk", &mBlwAtk)) setBit(hrTgr->mFlag, 3, mBlwAtk);
                bool mBlwUsed = getBit(hrTgr->mFlag, 4);
                if (ImGui::Checkbox("mBlwUsed", &mBlwUsed)) setBit(hrTgr->mFlag, 4, mBlwUsed);
                bool mUseBlwRapid = getBit(hrTgr->mFlag, 5);
                if (ImGui::Checkbox("mUseBlwRapid", &mUseBlwRapid)) setBit(hrTgr->mFlag, 5, mUseBlwRapid);
                bool mBlwAtkRpd0 = getBit(hrTgr->mFlag, 6);
                if (ImGui::Checkbox("mBlwAtkRpd0", &mBlwAtkRpd0)) setBit(hrTgr->mFlag, 6, mBlwAtkRpd0);
                bool mBlwAtkRpd1 = getBit(hrTgr->mFlag, 7);
                if (ImGui::Checkbox("mBlwAtkRpd1", &mBlwAtkRpd1)) setBit(hrTgr->mFlag, 7, mBlwAtkRpd1);
                bool mWallTurn = getBit(hrTgr->mFlag, 8);
                if (ImGui::Checkbox("mWallTurn", &mWallTurn)) setBit(hrTgr->mFlag, 8, mWallTurn);
                ImGui::InputInt("mTraceTex Ptr", (int*)&hrTgr->mTraceTex);
                for (int i = 0; i < 8; ++i) {
                    ImGui::InputInt(("mpEfWaveBlow Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTgr->mpEfWaveBlow[i]);
                    ImGui::InputInt(("mTaskCheckWaveBlow Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTgr->mTaskCheckWaveBlow[i]);
                }
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeJST) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRJST (Destroyman)", ImGuiTreeNodeFlags_DrawLinesFull)) {
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
                ImGui::Combo("Before Pc Pose", (int*)&hrJst->m_BeforePcPose, "ePcPoseUpper\0ePcPoseMiddle\0ePcPoseBottom\0ePcPoseMax\0");
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
                ImGui::Text("Right Eye Beam", ImGuiDataType_U32, &hrJst->m_pRightEyeBeam);
                ImGui::Text("Left Eye Beam", ImGuiDataType_U32, &hrJst->m_pLeftEyeBeam);
                ImGui::Text("HadoKen", ImGuiDataType_U32, &hrJst->m_pHadoKen);
                ImGui::Text("Final Flash", ImGuiDataType_U32, &hrJst->m_pFinalFlash);
                for (int i = 0; i < 4; i++) {
                    ImGui::InputScalar(("Lamp[" + std::to_string(i) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_pLamp[i]);
                }
                ImGui::Text("Flash Blur", ImGuiDataType_U32, &hrJst->m_FlashBlur);
                ImGui::Text("Elect", ImGuiDataType_U32, &hrJst->m_pElect);
                ImGui::Text("Paund", ImGuiDataType_U32, &hrJst->m_pPaund);
                ImGui::Text("Right Eye Check", (int*)&hrJst->m_RightEyeCheck);
                ImGui::Text("Left Eye Check", (int*)&hrJst->m_LeftEyeCheck);
                ImGui::Text("Hadoken Check", (int*)&hrJst->m_HadokenCheck);
                ImGui::Text("Final Flash Check", (int*)&hrJst->m_FinalFlashCheck);
                ImGui::Text("Paund Check", (int*)&hrJst->m_PaundCheck);
                for (int i = 0; i < 4; i++) {
                    ImGui::Text(("Line Uni[" + std::to_string(i) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_LineUni[i]);
                }
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 4; j++) {
                        ImGui::Text(("Line[" + std::to_string(i) + "][" + std::to_string(j) + "]").c_str(), ImGuiDataType_U32, &hrJst->m_Line[i][j]);
                    }
                }
                ImGui::Text("Demo Camera", ImGuiDataType_U32, &hrJst->m_DemoCamera);
                ImGui::Text("Container A", ImGuiDataType_U32, &hrJst->m_ContainerA);
                ImGui::Text("Container B", ImGuiDataType_U32, &hrJst->m_ContainerB);
                ImGui::Text("Light", ImGuiDataType_U32, &hrJst->m_Light);
                ImGui::Text("Container A Up", ImGuiDataType_U32, &hrJst->m_ContainerAUp);
                ImGui::Text("Container B Up", ImGuiDataType_U32, &hrJst->m_ContainerBUp);
                ImGui::Text("Container A Loop", ImGuiDataType_U32, &hrJst->m_ContainerALoop);
                ImGui::Text("Container B Loop", ImGuiDataType_U32, &hrJst->m_ContainerBLoop);
                ImGui::Text("Container A Down", ImGuiDataType_U32, &hrJst->m_ContainerADown);
                ImGui::Text("Container B Down", ImGuiDataType_U32, &hrJst->m_ContainerBDown);
                ImGui::Text("Light Up", ImGuiDataType_U32, &hrJst->m_LightUp);
                ImGui::Text("Light Loop", ImGuiDataType_U32, &hrJst->m_LightLoop);
                ImGui::Text("Light Down", ImGuiDataType_U32, &hrJst->m_LightDown);
            }
            ImGui::TreePop();
        }
    }
    
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeSFF) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRSFF (Holly Summers)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRSFF* hrSff = (HRSFF*)mpLockOnNpc;
            if (hrSff) {
                const char* holeStateNames[] = { "Lid", "Open", "Bury" };
                for (int i = 0; i < 14; ++i) {
                    ImGui::Combo(("m_HoleState[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_HoleState[i], holeStateNames, IM_ARRAYSIZE(holeStateNames));
                }
                const char* randKeyNames[] = { "RightCon", "LeftCon", "AButton", "NonKey" };
                ImGui::Combo("m_RandKey", (int*)&hrSff->m_RandKey, randKeyNames, IM_ARRAYSIZE(randKeyNames));
                for (int i = 0; i < 14; ++i) {
                    ImGui::InputFloat3(("m_HolePos[" + std::to_string(i) + "]").c_str(), &hrSff->m_HolePos[i].x);
                }
                for (int i = 0; i < 6; ++i) {
                    ImGui::InputFloat3(("m_SafePos[" + std::to_string(i) + "]").c_str(), &hrSff->m_SafePos[i].x);
                }
                ImGui::InputInt("m_ActionMode", &hrSff->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrSff->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrSff->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrSff->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrSff->m_MoveVec.x);
                ImGui::InputInt("m_TargetHoleNum", &hrSff->m_TargetHoleNum);
                ImGui::InputInt("m_TargetSafePos", &hrSff->m_TargetSafePos);
                ImGui::Checkbox("m_ColHole", &hrSff->m_ColHole);
                for (int i = 0; i < 14; ++i) {
                    ImGui::Checkbox(("m_HoleReq[" + std::to_string(i) + "]").c_str(), &hrSff->m_HoleReq[i]);
                }
                ImGui::Checkbox("m_SmokeHit", &hrSff->m_SmokeHit);
                ImGui::Checkbox("m_Demo", &hrSff->m_Demo);
                ImGui::Checkbox("m_PcDead", &hrSff->m_PcDead);
                ImGui::Checkbox("m_PineapReq", &hrSff->m_PineapReq);
                ImGui::Checkbox("m_StoneUp", &hrSff->m_StoneUp);
                ImGui::Checkbox("m_BefGuard", &hrSff->m_BefGuard);
                ImGui::Checkbox("m_PcShare", &hrSff->m_PcShare);
                ImGui::InputFloat3("m_PcPosition", &hrSff->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrSff->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrSff->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrSff->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrSff->m_HitWait);
                ImGui::InputInt("m_PcMotionNo", &hrSff->m_PcMotionNo);
                ImGui::InputFloat("m_PcRandTime", &hrSff->m_PcRandTime);
                ImGui::InputFloat("m_PineapleTime", &hrSff->m_PineapleTime);
                ImGui::InputInt("m_HoleDownNum", &hrSff->m_HoleDownNum);
                ImGui::InputFloat3("m_BeforePos", &hrSff->m_BeforePos.x);
                for (int i = 0; i < 10; ++i) {
                    ImGui::InputInt(("m_pMacross Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pMacross[i]);
                }
                for (int i = 0; i < 4; ++i) {
                    ImGui::InputInt(("m_pPineaple Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pPineaple[i]);
                }
                ImGui::InputInt("m_pSmoke Ptr", (int*)&hrSff->m_pSmoke);
                ImGui::InputInt("m_pKnife Ptr", (int*)&hrSff->m_pKnife);
                for (int i = 0; i < 28; ++i) {
                    ImGui::InputInt(("m_pPcSmoke Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pPcSmoke[i]);
                }
                for (int i = 0; i < 10; ++i) {
                    ImGui::InputInt(("m_pMacrossCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pMacrossCheck[i]);
                }
                for (int i = 0; i < 4; ++i) {
                    ImGui::InputInt(("m_pPineapleCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pPineapleCheck[i]);
                }
                ImGui::InputInt("m_pSmokeCheck Ptr", (int*)&hrSff->m_pSmokeCheck);
                ImGui::InputInt("m_pKnifeCheck Ptr", (int*)&hrSff->m_pKnifeCheck);
                for (int i = 0; i < 28; ++i) {
                    ImGui::InputInt(("m_pPcSmokeCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrSff->m_pPcSmokeCheck[i]);
                }
                ImGui::InputInt("m_pPineapleGmf Ptr", (int*)&hrSff->m_pPineapleGmf);
                ImGui::InputInt("m_pMineGmf Ptr", (int*)&hrSff->m_pMineGmf);
                ImGui::Checkbox("m_MineVisible", &hrSff->m_MineVisible);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeTET) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRTET (Harvey)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRTET* hrTet = (HRTET*)mpLockOnNpc;
            if (hrTet) {
                ImGui::Combo("m_SetDemoNum", (int*)&hrTet->m_SetDemoNum, "DemoInit\0ShowIn\0ShortShowIn\0ShowTrue\0ShowFalse\0");
                ImGui::Combo("m_RandKey", (int*)&hrTet->m_RandKey, "RightCon\0LeftCon\0AButton\0");
                ImGui::Checkbox("ItemMagic.Active", &hrTet->m_ItemMagic.Active);
                ImGui::InputFloat3("ItemMagic.Pos", &hrTet->m_ItemMagic.Pos.x);
                ImGui::InputInt("m_ActionMode", &hrTet->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrTet->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrTet->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrTet->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrTet->m_MoveVec.x);
                ImGui::InputFloat3("m_BeforePos", &hrTet->m_BeforePos.x);
                ImGui::InputFloat("m_BeforeRotY", &hrTet->m_BeforeRotY);
                ImGui::InputFloat("m_ShowHitCount", &hrTet->m_ShowHitCount);
                ImGui::InputInt("m_AmbCount", &hrTet->m_AmbCount);
                ImGui::Checkbox("m_BossVisible", &hrTet->m_BossVisible);
                ImGui::Checkbox("m_PigionEnable", &hrTet->m_PigionEnable);
                ImGui::Checkbox("m_FirstShow", &hrTet->m_FirstShow);
                ImGui::Checkbox("m_BossPause", &hrTet->m_BossPause);
                ImGui::InputFloat3("m_BeforeAcc", &hrTet->m_BeforeAcc.x);
                ImGui::InputInt("m_FadeFrame", &hrTet->m_FadeFrame);
                ImGui::InputInt("m_ShowTimeCount", &hrTet->m_ShowTimeCount);
                ImGui::InputInt("m_ReverseWait", &hrTet->m_ReverseWait);
                ImGui::InputInt("m_BaseBGMHandle", &hrTet->m_BaseBGMHandle);
                ImGui::InputInt("m_SpBGMHandle", &hrTet->m_SpBGMHandle);
                ImGui::Checkbox("m_BefGuard", &hrTet->m_BefGuard);
                ImGui::Checkbox("m_FirstShowTime", &hrTet->m_FirstShowTime);
                ImGui::InputFloat3("m_PcPosition", &hrTet->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrTet->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrTet->m_PcDirection.x);
                ImGui::InputFloat3("m_PcBeforePos", &hrTet->m_PcBeforePos.x);
                ImGui::InputFloat("m_PcBeforeRotY", &hrTet->m_PcBeforeRotY);
                ImGui::InputFloat("m_PcDistance", &hrTet->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrTet->m_HitWait);
                ImGui::Checkbox("m_PcShare", &hrTet->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrTet->m_PcMotionNo);
                ImGui::InputFloat("m_PcRandTime", &hrTet->m_PcRandTime);
                ImGui::InputInt("m_LoopSEID", &hrTet->m_LoopSEID);
                ImGui::InputInt("m_pBoxGmf Ptr", (int*)&hrTet->m_pBoxGmf);
                ImGui::InputInt("m_BoxMotionNo", &hrTet->m_BoxMotionNo);
                ImGui::InputInt("m_ShakeCount", &hrTet->m_ShakeCount);
                ImGui::InputFloat("m_ShakeAcc", &hrTet->m_ShakeAcc);
                ImGui::InputInt("m_pAssistAGmf Ptr", (int*)&hrTet->m_pAssistAGmf);
                ImGui::InputInt("m_pAssistBGmf Ptr", (int*)&hrTet->m_pAssistBGmf);
                ImGui::InputFloat3("m_AssistAPos", &hrTet->m_AssistAPos.x);
                ImGui::InputFloat3("m_AssistBPos", &hrTet->m_AssistBPos.x);
                ImGui::InputFloat3("m_AssistARot", &hrTet->m_AssistARot.x);
                ImGui::InputFloat3("m_AssistBRot", &hrTet->m_AssistBRot.x);
                ImGui::InputInt("m_pLeftArmsGmf Ptr", (int*)&hrTet->m_pLeftArmsGmf);
                ImGui::InputInt("m_pRightArmsGmf Ptr", (int*)&hrTet->m_pRightArmsGmf);
                ImGui::Checkbox("m_RArmVisible", &hrTet->m_RArmVisible);
                ImGui::Checkbox("m_LArmVisible", &hrTet->m_LArmVisible);
                ImGui::Checkbox("m_AssistAVisible", &hrTet->m_AssistAVisible);
                ImGui::Checkbox("m_AssistBVisible", &hrTet->m_AssistBVisible);
                ImGui::Checkbox("m_BoxVisible", &hrTet->m_BoxVisible);
                ImGui::Checkbox("m_ArmVisible", &hrTet->m_ArmVisible);
                for (int i = 0; i < 4; ++i) {
                    ImGui::InputInt(("m_Light Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTet->m_Light[i]);
                }
                ImGui::Checkbox("m_FadeFlag", &hrTet->m_FadeFlag);
                ImGui::InputInt("m_pStageMotion Ptr", (int*)&hrTet->m_pStageMotion);
                ImGui::InputInt("m_RSlash Ptr", (int*)&hrTet->m_RSlash);
                ImGui::InputInt("m_LSlash Ptr", (int*)&hrTet->m_LSlash);
                ImGui::InputInt("m_EnemySpot Ptr", (int*)&hrTet->m_EnemySpot);
                ImGui::InputInt("m_PcSpot Ptr", (int*)&hrTet->m_PcSpot);
                ImGui::InputInt("m_AsiASpot Ptr", (int*)&hrTet->m_AsiASpot);
                ImGui::InputInt("m_AsiBSpot Ptr", (int*)&hrTet->m_AsiBSpot);
                ImGui::InputInt("m_ViewReverse Ptr", (int*)&hrTet->m_ViewReverse);
                ImGui::InputInt("m_SwordR Ptr", (int*)&hrTet->m_SwordR);
                ImGui::InputInt("m_SwordB Ptr", (int*)&hrTet->m_SwordB);
                ImGui::InputInt("m_pStar Ptr", (int*)&hrTet->m_pStar);
                ImGui::InputInt("m_pHeart Ptr", (int*)&hrTet->m_pHeart);
                for (int i = 0; i < 6; ++i) {
                    ImGui::InputInt(("m_Pigeon Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTet->m_Pigeon[i]);
                }
                for (int i = 0; i < 6; ++i) {
                    ImGui::InputInt(("m_PigeonCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTet->m_PigeonCheck[i]);
                }
                ImGui::InputInt("m_pAmbGmf Ptr", (int*)&hrTet->m_pAmbGmf);
                ImGui::Checkbox("m_AmbVisible Ptr", &hrTet->m_AmbVisible);
                ImGui::InputInt("m_pSlvGmf Ptr", (int*)&hrTet->m_pSlvGmf);
                for (int i = 0; i < 2; ++i) {
                    ImGui::InputInt(("m_pSlvMotion Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTet->m_pSlvMotion[i]);
                }
                ImGui::InputFloat("m_SlvRandTime", &hrTet->m_SlvRandTime);
                ImGui::InputInt("m_pTopSpin Ptr", (int*)&hrTet->m_pTopSpin);
                ImGui::InputInt("m_pSmoke Ptr", (int*)&hrTet->m_pSmoke);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeEFL) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HREFL (Speedbuster)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HREFL* hrEfl = (HREFL*)mpLockOnNpc;
            if (hrEfl) {
                ImGui::InputInt("m_ActionMode", &hrEfl->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrEfl->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrEfl->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrEfl->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrEfl->m_MoveVec.x);
                ImGui::InputFloat("m_AttackWait", &hrEfl->m_AttackWait);
                ImGui::Checkbox("m_BusterHit", &hrEfl->m_BusterHit);
                ImGui::Checkbox("m_MiniDemo", &hrEfl->m_MiniDemo);
                ImGui::Checkbox("m_HitFirst", &hrEfl->m_HitFirst);
                ImGui::Checkbox("m_BusterBreak", &hrEfl->m_BusterBreak);
                ImGui::InputFloat("m_KeyCount", &hrEfl->m_KeyCount);
                ImGui::Checkbox("m_PcDead", &hrEfl->m_PcDead);
                ImGui::Checkbox("m_EffectHit", &hrEfl->m_EffectHit);
                ImGui::InputInt("m_TsubaSE", &hrEfl->m_TsubaSE);
                ImGui::InputFloat3("m_PcPosition", &hrEfl->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrEfl->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrEfl->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrEfl->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrEfl->m_HitWait);
                ImGui::Checkbox("m_PcShare", &hrEfl->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrEfl->m_PcMotionNo);
                ImGui::InputInt("m_pGmf Ptr", (int*)&hrEfl->m_pGmf);
                ImGui::InputInt("m_pBuster Ptr", (int*)&hrEfl->m_pBuster);
                ImGui::InputFloat3("m_BusterPos", &hrEfl->m_BusterPos.x);
                ImGui::InputFloat("m_BusterRot", &hrEfl->m_BusterRot);
                ImGui::Checkbox("m_BusterFlag", &hrEfl->m_BusterFlag);
                ImGui::InputInt("m_pBusterCheck Ptr", (int*)&hrEfl->m_pBusterCheck);
                ImGui::InputInt("m_Close Ptr", (int*)&hrEfl->m_Close);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeTYG) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRTYG (Bad Girl)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRTYG* hrTyg = (HRTYG*)mpLockOnNpc;
            if (hrTyg) {
                ImGui::InputInt("m_ActionMode", &hrTyg->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrTyg->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrTyg->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrTyg->m_MotionFrame);
                ImGui::InputInt("m_EnemyPopCount", &hrTyg->m_EnemyPopCount);
                ImGui::InputInt("m_EnemyPopMax", &hrTyg->m_EnemyPopMax);
                ImGui::InputInt("m_EnemyPopNum", &hrTyg->m_EnemyPopNum);
                ImGui::InputInt("m_EnemyKillNum", &hrTyg->m_EnemyKillNum);
                ImGui::InputInt("m_NewPopIndex", &hrTyg->m_NewPopIndex);
                ImGui::InputFloat("m_ClonePopFrame", &hrTyg->m_ClonePopFrame);
                ImGui::InputInt("m_CloneRetNum", &hrTyg->m_CloneRetNum);
                ImGui::InputInt("m_CloneRetMax", &hrTyg->m_CloneRetMax);
                ImGui::InputInt("m_OldPopFrame", &hrTyg->m_OldPopFrame);
                ImGui::InputInt("m_DrinkCount", &hrTyg->m_DrinkCount);
                ImGui::Checkbox("m_DrinkCamera", &hrTyg->m_DrinkCamera);
                ImGui::Checkbox("m_BattleClone", &hrTyg->m_BattleClone);
                ImGui::InputFloat3("m_MoveVec", &hrTyg->m_MoveVec.x);
                ImGui::InputFloat3("m_BatPos", &hrTyg->m_BatPos.x);
                ImGui::InputFloat3("m_EfPos", &hrTyg->m_EfPos.x);
                ImGui::InputInt("m_SlashTex Ptr", (int*)&hrTyg->m_SlashTex);
                ImGui::Checkbox("m_RetCamera", &hrTyg->m_RetCamera);
                ImGui::Checkbox("m_SwingCamera", &hrTyg->m_SwingCamera);
                ImGui::InputInt("m_BeforePcPose", (int*)&hrTyg->m_BeforePcPose);
                ImGui::InputInt("m_DemoNum", (int*)&hrTyg->m_DemoNum);
                ImGui::Checkbox("m_BatFireFlag", &hrTyg->m_BatFireFlag);
                ImGui::InputFloat("m_LampCount", &hrTyg->m_LampCount);
                ImGui::Checkbox("m_ZakoTsuba", &hrTyg->m_ZakoTsuba);
                ImGui::Checkbox("m_BefGuard", &hrTyg->m_BefGuard);
                ImGui::Checkbox("m_JumpShakeEffect", &hrTyg->m_JumpShakeEffect);
                ImGui::Checkbox("m_CryFirst", &hrTyg->m_CryFirst);
                ImGui::InputInt("m_SEReqCount", &hrTyg->m_SEReqCount);
                ImGui::Checkbox("m_FirstFireAtk", &hrTyg->m_FirstFireAtk);
                ImGui::InputFloat3("m_PcPosition", &hrTyg->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrTyg->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrTyg->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrTyg->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrTyg->m_HitWait);
                ImGui::Checkbox("m_PcShare", &hrTyg->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrTyg->m_PcMotionNo);
                ImGui::InputInt("m_RefrigeGmf Ptr", (int*)&hrTyg->m_RefrigeGmf);
                ImGui::InputInt("m_RefrigeGan Ptr", (int*)&hrTyg->m_RefrigeGan);
                ImGui::InputFloat3("m_RefrigePos", &hrTyg->m_RefrigePos.x);
                ImGui::InputInt("m_CanGmf Ptr", (int*)&hrTyg->m_CanGmf);
                ImGui::InputInt("m_CanCloseGan Ptr", (int*)&hrTyg->m_CanCloseGan);
                ImGui::InputInt("m_CanOpenGan Ptr", (int*)&hrTyg->m_CanOpenGan);
                ImGui::InputInt("m_pBatFire Ptr", (int*)&hrTyg->m_pBatFire);
                ImGui::InputInt("m_ShockWave Ptr", (int*)&hrTyg->m_ShockWave);
                ImGui::InputInt("m_WaveCheck Ptr", (int*)&hrTyg->m_WaveCheck);
                ImGui::InputInt("m_Close Ptr", (int*)&hrTyg->m_Close);
                ImGui::InputInt("m_Belt Ptr", (int*)&hrTyg->m_Belt);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeGEN) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRGEN (Jeane)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRGEN* hrGen = (HRGEN*)mpLockOnNpc;
            if (hrGen) {
                ImGui::InputInt("m_ActionMode", &hrGen->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrGen->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrGen->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrGen->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrGen->m_MoveVec.x);
                ImGui::InputInt("m_ComboCount", &hrGen->m_ComboCount);
                ImGui::InputInt("m_AvoidCount", &hrGen->m_AvoidCount);
                ImGui::InputFloat("m_AddRot", &hrGen->m_AddRot);
                ImGui::InputFloat("m_RotLength", &hrGen->m_RotLength);
                ImGui::Checkbox("m_Demo", &hrGen->m_Demo);
                ImGui::InputInt("m_Stage", &hrGen->m_Stage);
                ImGui::InputInt("m_DemoNum", (int*)&hrGen->m_DemoNum);
                ImGui::InputInt("m_BGMHandle", &hrGen->m_BGMHandle);
                ImGui::InputInt("m_GuardModeCount", &hrGen->m_GuardModeCount);
                ImGui::InputFloat("m_Fov", &hrGen->m_Fov);
                ImGui::InputFloat("m_Aspect", &hrGen->m_Aspect);
                ImGui::InputFloat("m_Near", &hrGen->m_Near);
                ImGui::InputFloat("m_Far", &hrGen->m_Far);
                ImGui::InputFloat3("m_PcPosition", &hrGen->m_PcPosition.x);
                ImGui::InputFloat3("m_PcNavel", &hrGen->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrGen->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrGen->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrGen->m_HitWait);
                ImGui::Checkbox("m_PcShare", &hrGen->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrGen->m_PcMotionNo);
                ImGui::InputFloat("m_PcRandTime", &hrGen->m_PcRandTime);
                ImGui::InputInt("m_pSlash Ptr[0]", (int*)&hrGen->m_pSlash[0]);
                ImGui::InputInt("m_pSlash Ptr[1]", (int*)&hrGen->m_pSlash[1]);
                ImGui::InputInt("m_DragonModel Ptr", (int*)&hrGen->m_DragonModel);
                ImGui::InputFloat3("m_RHandPos", &hrGen->m_RHandPos.x);
                ImGui::InputFloat3("m_LHandPos", &hrGen->m_LHandPos.x);
                ImGui::InputFloat3("m_RFootPos", &hrGen->m_RFootPos.x);
                ImGui::InputFloat3("m_LFootPos", &hrGen->m_LFootPos.x);
            }
            ImGui::TreePop();
        }
    }
    if (mpLockOnNpc && mpLockOnNpc->mStatus.charaType == eCharaTypeTKL) {
        if (ImGui::TreeNodeEx("mpLockOnNpc HRTKL (Henry)", ImGuiTreeNodeFlags_DrawLinesFull)) {
            HRTKL* hrTkl = (HRTKL*)mpLockOnNpc;
            // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(&hrTkl->Padding_1612[0]);
            // ImGui::Text("Base Address: 0x%08X", baseAddress);
            // uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&hrTkl->m_SlashPos);
            // ImGui::Text("Target Address: 0x%08X", targetAddress);
            // uintptr_t offsetDifference = targetAddress - baseAddress;
            // ImGui::Text("Offset difference: 0x%08X", offsetDifference);
            if (hrTkl) {
                ImGui::InputInt("m_ActionMode", &hrTkl->m_ActionMode);
                ImGui::InputFloat("m_ActionTime", &hrTkl->m_ActionTime);
                ImGui::InputFloat("m_RandTime", &hrTkl->m_RandTime);
                ImGui::InputInt("m_MotionFrame", &hrTkl->m_MotionFrame);
                ImGui::InputFloat3("m_MoveVec", &hrTkl->m_MoveVec.x);
                ImGui::InputInt("m_ComboMode", (int*)&hrTkl->m_ComboMode);
                ImGui::InputInt("m_BeforePcPose", (int*)&hrTkl->m_BeforePcPose);
                ImGui::Checkbox("m_BefGuard", &hrTkl->m_BefGuard);
                for (int i = 0; i < 5; ++i) {
                    ImGui::InputInt(("m_pCatGmf Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTkl->m_pCatGmf[i]);
                    ImGui::InputInt(("m_pCatGan Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTkl->m_pCatGan[i]);

                }
                ImGui::InputFloat3("m_PcPosition", &hrTkl->m_PcPosition.x);
                ImGui::InputFloat3("m_PcFrontPos", &hrTkl->m_PcFrontPos.x);
                ImGui::InputFloat3("m_PcBackPos", &hrTkl->m_PcBackPos.x);
                ImGui::InputFloat3("m_PcRightPos", &hrTkl->m_PcRightPos.x);
                ImGui::InputFloat3("m_PcLeftPos", &hrTkl->m_PcLeftPos.x);
                ImGui::InputFloat3("m_PcNavel", &hrTkl->m_PcNavel.x);
                ImGui::InputFloat3("m_PcDirection", &hrTkl->m_PcDirection.x);
                ImGui::InputFloat("m_PcDistance", &hrTkl->m_PcDistance);
                ImGui::InputFloat("m_HitWait", &hrTkl->m_HitWait);
                ImGui::Checkbox("m_PcShare", &hrTkl->m_PcShare);
                ImGui::InputInt("m_PcMotionNo", &hrTkl->m_PcMotionNo);
                ImGui::InputFloat("m_PcRandTime", &hrTkl->m_PcRandTime);
                for (int i = 0; i < 5; ++i) {
                    ImGui::InputInt(("m_pPlasma Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTkl->m_pPlasma[i]);
                    ImGui::InputInt(("m_PlasmaCheck Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTkl->m_PlasmaCheck[i]);

                }
                ImGui::InputFloat3("m_SlashPos", &hrTkl->m_SlashPos.x);
                ImGui::InputFloat3("m_OldSlashPos", &hrTkl->m_OldSlashPos.x);
                ImGui::InputInt("m_pObjAGmf Ptr", (int*)&hrTkl->m_pObjAGmf);
                ImGui::InputInt("m_pObjBGmf Ptr", (int*)&hrTkl->m_pObjBGmf);
                ImGui::InputInt("m_pObjAMotion Ptr", (int*)&hrTkl->m_pObjAMotion);
                ImGui::InputInt("m_pObjBMotion Ptr", (int*)&hrTkl->m_pObjBMotion);
                for (int i = 0; i < 5; ++i) {
                    ImGui::InputInt(("m_pSwordEff Ptr[" + std::to_string(i) + "]").c_str(), (int*)&hrTkl->m_pSwordEff[i]);
                }
            }
            ImGui::TreePop();
        }
    }
}

void EnemyTracker::on_draw_ui() {
    ImGui::Checkbox("Enemy Stats Popout", &imguiPopout);
    if (!imguiPopout)
        DrawEnemyStats();
}

//void EnemyTracker::custom_imgui_window() {}

// during load
//void EnemyTracker::on_config_load(const utility::Config &cfg) {}
// during save
//void EnemyTracker::on_config_save(utility::Config &cfg) {}
// do something every frame
void EnemyTracker::on_frame() {
    if (imguiPopout) {
        ImGui::Begin("Enemy Stats", &imguiPopout);
        DrawEnemyStats();
        ImGui::End();
    }
}
// will show up in debug window, dump ImGui widgets you want here
//void EnemyTracker::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
