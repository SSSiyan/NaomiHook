#include "nmh.hpp"

namespace nmh_sdk {
    // Get battle, contains player and enemies
	mHRBattle* get_mHRBattle() {
		static uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		if (mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr)
			return gpBattle;
		 return NULL;
	}

    // Get player character
	mHRPc* get_mHRPc() {
		mHRBattle* gpBattle = get_mHRBattle();
		if (gpBattle && gpBattle->mpPc)
			return gpBattle->mpPc;
		return NULL;
	}

    // Get stage
	CBgCtrl* get_CBgCtrl() {
		static uintptr_t CBgCtrlPtr = g_framework->get_module().as<uintptr_t>() + 0x8446F0;
		if (CBgCtrl* cBgCtrl = (CBgCtrl*)*(uintptr_t*)CBgCtrlPtr)
			return cBgCtrl;
		return NULL;
	}

    HrGameTask* get_HrGameTask() {
        static uintptr_t HrGameTaskPtr = g_framework->get_module().as<uintptr_t>() + 0x841414;
        if (HrGameTask* hrGameTask = (HrGameTask*)*(uintptr_t*)HrGameTaskPtr)
			return hrGameTask;
		return NULL;
    }

    HrCamera* get_HrCamera()
    {
        static HrCamera* HrCameraPtr = (HrCamera*)(g_framework->get_module().as<uintptr_t>() + 0x82A4A0);
        return HrCameraPtr;
    }

    HrScreenFilter* get_HrScreenFilter()
    {
        static HrScreenFilter* HrScreenFilterPtr = (HrScreenFilter*)(g_framework->get_module().as<uintptr_t>() + 0x85E440);
        return HrScreenFilterPtr;
    }

    HrMenuTask* get_HrMenuTask() {
        static uintptr_t hrMenuTaskPtr = g_framework->get_module().as<uintptr_t>() + 0x8447D4;
        if (HrMenuTask* hrMenuTask = (HrMenuTask*)*(uintptr_t*)hrMenuTaskPtr)
            return hrMenuTask;
		return NULL;
    }

    DolphinApp* get_DolphinApp() {
        static uintptr_t dolphinAppPtr = g_framework->get_module().as<uintptr_t>() + 0x83DDE0;
        if (DolphinApp* dolphinApp = (DolphinApp*)*(uintptr_t*)dolphinAppPtr)
            return dolphinApp;
		return NULL;
    }

    // Set stage
    void SetStage(const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10) {
        uintptr_t setStageAddress = (g_framework->get_module().as<uintptr_t>() + 0x3FD690);
        setStageFunc setStage = (setStageFunc)setStageAddress;
        // jump to stage
        if (CBgCtrl* cBgCtrl = get_CBgCtrl()) {
            setStage(cBgCtrl, _StgName, _StgAdd, _Arg1, _Arg2, inBossInfoDisp, inFadeType, inSetVolRate, inPause, a10);
        }
        // if in a shop or motel, unload it
        if (HrMenuTask* HrMenuTask = nmh_sdk::get_HrMenuTask()){
            if (HrMenuTask->m_pHsMenu) {
                HrMenuTask->m_pHsMenu->m_Task = HS_CLASS_TASK_END;
            }
        }
        // set player visible in case cutscenes have set you invisible
        SetVisible(true);
    }

    void SetVisible(bool inVisible) {
        uintptr_t setVisibleAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D6D90);
        mSetVisibleFunc setVisible = (mSetVisibleFunc)setVisibleAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            setVisible(mHRPc, inVisible);
        }
    }

    // Play player motion
    bool PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate) {
        uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
        mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return playMotion(mHRPc, inMotNo, inLoop, inStartFrame, inOverWrite, inInterpolate);
        }
    }

    // Set player weapon or clothing
    void SetEquip(pcItem inID, bool inPowUp) {
        uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
        mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            setEquip(mHRPc, inID, inPowUp);
        }
    }

    // Set player weapon to Mk3
    void SetMk3Equip() {
        uintptr_t setEquipMk3Address = (g_framework->get_module().as<uintptr_t>() + 0x3E29C0);
        mSetEquipMk3Func setEquipMk3 = (mSetEquipMk3Func)setEquipMk3Address;
        if (mHRPc* mHRPc = get_mHRPc()) {
            setEquipMk3(mHRPc);
        }
    }

    // Check if player is attacking
    bool CheckCanAttack() {
        static uintptr_t checkCanAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D47B0);
        static mCheckCanAttackFunc checkCanAttack = (mCheckCanAttackFunc)checkCanAttackAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkCanAttack(mHRPc);
        }
        return false; // player ptr is invalid, treat as if we can't attack
    }

    // Check if player is guarding
    bool CheckGuardMotion(bool inDefGuardNoChk) {
        uintptr_t checkGuardMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E26C0);
        mCheckGuardMotionFunc checkGuardMotion = (mCheckGuardMotionFunc)checkGuardMotionAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkGuardMotion(mHRPc, inDefGuardNoChk);
        }
        return true; // player ptr is invalid, treat as if we're guarding
    }

    // Check if player is eating a hit
    bool CheckHajikare() {
        uintptr_t checkHajikareAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D4BA0);
        mCheckHajikareFunc checkHajikare = (mCheckHajikareFunc)checkHajikareAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkHajikare(mHRPc);
        }
        return true; // player ptr is invalid, treat as if we're taking a hit
    }

    // Check if player is clashing, call with -1
    bool CheckTsubazering(int unkn) {
        uintptr_t checkTsubazeringAddress = (g_framework->get_module().as<uintptr_t>() + 0x3DFFC0);
        mCheckTsubazeringFunc checkTsubazering = (mCheckTsubazeringFunc)checkTsubazeringAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkTsubazering(mHRPc, unkn);
        }
        return true; // player ptr is invalid, treat as if we're clashing
    }

    // Check if player is dodging back or left or right, call with -1
    bool CheckSideStep(int unkn) {
        uintptr_t checkSideStepAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2730);
        mCheckSideStepFunc checkSideStep = (mCheckSideStepFunc)checkSideStepAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkSideStep(mHRPc, unkn);
        }
        return true; // player ptr is invalid, treat as if we're dodging
    }

    // Trigger Darksides
    bool Start777() {
        uintptr_t start777Address = (g_framework->get_module().as<uintptr_t>() + 0x3C1AE0);
        mStart777Func start777 = (mStart777Func)start777Address;
        if (mHRPc* mHRPc = get_mHRPc()) {
            start777(mHRPc);
            return false;
        }
        return true; // player ptr invalid
    }
    bool StartBar(bool inDontCallStencil, int inTick) {
        uintptr_t startBarAddress = (g_framework->get_module().as<uintptr_t>() + 0x3C1BD0);
        mStartBarFunc startBar = (mStartBarFunc)startBarAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            startBar(mHRPc, inDontCallStencil, inTick);
            return false;
        }
        return true; // player ptr invalid
    }
    bool StartBell() {
        uintptr_t startBellAddress = (g_framework->get_module().as<uintptr_t>() + 0x3C1EE0);
        mStartBellFunc startBell = (mStartBellFunc)startBellAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            startBell(mHRPc);
            return false;
        }
        return true; // player ptr invalid
    }
    bool StartHopper() {
        uintptr_t startHopperAddress = (g_framework->get_module().as<uintptr_t>() + 0x3C2120);
        mStartHopperFunc startHopper = (mStartHopperFunc)startHopperAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            startHopper(mHRPc);
            return false;
        }
        return true; // player ptr invalid
    }
    bool StartCherry() {
        uintptr_t startCherryAddress = (g_framework->get_module().as<uintptr_t>() + 0x3C2330);
        mStartCherryFunc startCherry = (mStartCherryFunc)startCherryAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            startCherry(mHRPc);
            return false;
        }
        return true; // player ptr invalid
    }

    // Take 3d pos, put screen space pos in Dest
    bool GetScreenPos(Vec* Src, Vec* Dest) {
        uintptr_t getScreenPosAddress = (g_framework->get_module().as<uintptr_t>() + 0x5E8560);
        mGetScreenPosFunc getScreenPos = (mGetScreenPosFunc)getScreenPosAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return getScreenPos(Src, Dest);
        }
        return true; // player ptr invalid
    }

    bool CheckThrowAttack() {
        uintptr_t checkThrowAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E24B0);
        mCheckThrowAttackFunc checkThrowAttack = (mCheckThrowAttackFunc)checkThrowAttackAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkThrowAttack(mHRPc, -1);
        }
        return true; // player ptr invalid
    }

    uint32_t GetLaserColor() {
        uintptr_t getLaserColorAddress = (g_framework->get_module().as<uintptr_t>() + 0x3BF5E0);
        mGetLaserColorFunc getLaserColor = (mGetLaserColorFunc)getLaserColorAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return getLaserColor(mHRPc);
        }
        return 0; // player ptr invalid
    }

    void PlayCamMotFromCharMot(int inMotNo, int inNum, bool inHitColl, bool inPcBasePos, bool inPcBasePosa) {
        uintptr_t playCamMotFromCharMotAddr = (g_framework->get_module().as<uintptr_t>() + 0x402340);
        mPlayCamMotFromCharMotFunc playCamMotFromCharMot = (mPlayCamMotFromCharMotFunc)playCamMotFromCharMotAddr;
        if (mHRPc* mHRPc = get_mHRPc()) {
            playCamMotFromCharMot(mHRPc, inMotNo, inNum, inHitColl, inPcBasePos, inPcBasePosa);
        }
    }

    void SetLightReflect(mHRPc* mHRPc, float strength, Vec* inPos, int inCol, int unkn) {
        uintptr_t mSetLightReflectAddress = (g_framework->get_module().as<uintptr_t>() + 0x3C52C0);
        // mSetLightReflecteFunc setLightReflect = (mSetLightReflecteFunc)mSetLightReflectAddress;
        __asm {
            mov ecx, mHRPc
            movss xmm2, strength
            push unkn
            push inCol
            push inPos
            call mSetLightReflectAddress
        }
    }

    void SubPcMoney(int inMoney) {
        uintptr_t subPcMoneyAddr = (g_framework->get_module().as<uintptr_t>() + 0x9D0E0);
        mSubPcMoneyFunc subPcMoney = (mSubPcMoneyFunc)subPcMoneyAddr;
        if (mHRPc* mHRPc = get_mHRPc()) {
            subPcMoney(mHRPc, inMoney);
        }
    }

    void AddLocker(pcItem itemID) {
        uintptr_t addLockerAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E2780);
        mAddLockerFunc addLocker = (mAddLockerFunc)addLockerAddr;
        if (mHRPc* mHRPc = get_mHRPc()) {
            addLocker(mHRPc, itemID);
        }
    }

    // returns true if you own the item
    bool CheckLocker(pcItem itemID) {
        uintptr_t checkLockerAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E2750);
        mChkLockerFunc checkLocker = (mChkLockerFunc)checkLockerAddr;
        if (mHRPc* mHRPc = get_mHRPc()) {
            return checkLocker(mHRPc, itemID);
        }
        return true; // player ptr invalid
    }

    void SetInputMode(mHRPc* mHRPc, enPcInputMode inMode) {
        uintptr_t setInputModeAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E28B0);
        mSetInputModeFunc setInputMode = (mSetInputModeFunc)setInputModeAddr;
        if (mHRPc) {
            setInputMode(mHRPc, inMode);
        }
    }

    bool CheckTrvReadEnd(mHRPc* mHRPc) {
        uintptr_t travisReadEndAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E2AA0);
        mCheckTrvReadEndFunc trvReadEnd = (mCheckTrvReadEndFunc)travisReadEndAddr;
        if (mHRPc) {
            return trvReadEnd(mHRPc);
        }
        return false;
    }

    bool CheckEquipReadEnd(mHRPc* mHRPc) {
        uintptr_t equipReadEndAddr = (g_framework->get_module().as<uintptr_t>() + 0x3DF290);
        mCheckEquipReadEndFunc equipReadEnd = (mCheckEquipReadEndFunc)equipReadEndAddr;
        if (mHRPc) {
            return equipReadEnd(mHRPc);
        }
        return false;
    }

    void PlayBattouDemo(mHRPc* mHRPc) {
        uintptr_t battouDemoAddr = (g_framework->get_module().as<uintptr_t>() + 0x3C3D70);
        mPlayBattouDemoFunc battouDemo = (mPlayBattouDemoFunc)battouDemoAddr;
        if (mHRPc) {
            battouDemo(mHRPc);
        }
    }

    void PlayNoutouDemo(mHRPc* mHRPc) {
        uintptr_t noutouDemoAddr = (g_framework->get_module().as<uintptr_t>() + 0x3C3C80);
        mPlayNoutouDemoFunc noutouDemo = (mPlayNoutouDemoFunc)noutouDemoAddr;
        if (mHRPc) {
            noutouDemo(mHRPc);
        }
    }

    void SetNoutou(mHRPc* mHRPc, bool inSkipAttach) {
        uintptr_t setNoutouAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E0B00);
        mSetNoutouFunc setNoutou = (mSetNoutouFunc)setNoutouAddr;
        if (mHRPc) {
            setNoutou(mHRPc, inSkipAttach);
        }
    }

    void SetBattou(mHRPc* mHRPc, bool inSkipAttach) { 
        uintptr_t setBattouAddr = (g_framework->get_module().as<uintptr_t>() + 0x3E0950);
        mSetBattouFunc setBattou = (mSetBattouFunc)setBattouAddr;
        if (mHRPc) {
            setBattou(mHRPc, inSkipAttach);
        }
    }

    // Spawn enemies
    int setInitNpcDat(int inResNo, enCharaType inChType, int inRepop, const Vec* inPos, const Vec* inRot, enPopReqType inPopType, bool inDisEnableCollision) {
        uintptr_t mSetInitNpcDatAddress = (g_framework->get_module().as<uintptr_t>() + 0x3B6B30);
        mSetInitNpcDatFunc mSetInitNpcDat = (mSetInitNpcDatFunc)mSetInitNpcDatAddress;
        if (mHRBattle* mHRBattle = get_mHRBattle()) {
            return mSetInitNpcDat(mHRBattle, inResNo, inChType, inRepop, inPos, inRot, inPopType, inDisEnableCollision);
        }
        return 0;
    }
}
