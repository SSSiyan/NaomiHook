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

    HrMenuTask* get_HrMenuTask() {
        static uintptr_t hrMenuTaskPtr = g_framework->get_module().as<uintptr_t>() + 0x8447D4;
        if (HrMenuTask* hrMenuTask = (HrMenuTask*)*(uintptr_t*)hrMenuTaskPtr)
            return hrMenuTask;
		return NULL;
    }

    // Set stage
    void SetStage(const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10) {
        uintptr_t setStageAddress = (g_framework->get_module().as<uintptr_t>() + 0x3FD690);
        setStageFunc setStage = (setStageFunc)setStageAddress;
        if (CBgCtrl* cBgCtrl = get_CBgCtrl()) {
            setStage(cBgCtrl, _StgName, _StgAdd, _Arg1, _Arg2, inBossInfoDisp, inFadeType, inSetVolRate, inPause, a10);
        }
        if (HrMenuTask* HrMenuTask = nmh_sdk::get_HrMenuTask()){
            if (HrMenuTask->m_pHsMenu) {  // if in a shop or motel
                HrMenuTask->m_pHsMenu->m_Task = HS_CLASS_TASK_END;   // unload it
            }
        }
    }

    // Play player motion
    void PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate) {
        uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
        mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
        if (mHRPc* mHRPc = get_mHRPc()) {
            playMotion(mHRPc, inMotNo, inLoop, inStartFrame, inOverWrite, inInterpolate);
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
        uintptr_t checkCanAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D47B0);
        mCheckCanAttackFunc checkCanAttack = (mCheckCanAttackFunc)checkCanAttackAddress;
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
}
