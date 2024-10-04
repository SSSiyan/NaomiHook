#include "nmh.hpp"

namespace nmh_sdk {
    // Get battle, contains player and enemies
	mHRBattle* get_mHRBattle() {
		static uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		if (gpBattle)
			return gpBattle;
		 return NULL;
	}

    // Get player character
	mHRPc* get_mHRPc() {
		static mHRBattle* gpBattle = get_mHRBattle();
		if (gpBattle && gpBattle->mpPc)
			return gpBattle->mpPc;
		return NULL;
	}

    // Get stage
	CBgCtrl* get_CBgCtrl() {
		static uintptr_t CBgCtrlPtr = g_framework->get_module().as<uintptr_t>() + 0x8446F0;
		CBgCtrl* cBgCtrl = (CBgCtrl*)*(uintptr_t*)CBgCtrlPtr;
		if (cBgCtrl)
			return cBgCtrl;
		return NULL;
	}

    // Play player motion
    void PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate) {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
        mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
        if (mHRPc) {
            playMotion(mHRPc, inMotNo, inLoop, inStartFrame, inOverWrite, inInterpolate);
        }
    }

    // Set weapon or clothing
    void SetEquip(pcItem inID, bool inPowUp) {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
        mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
        if (mHRPc) {
            setEquip(mHRPc, inID, inPowUp);
        }
    }

    // Set current stage
    void SetStage(const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10) {
        CBgCtrl* cBgCtrl = get_CBgCtrl();
        uintptr_t setStageAddress = (g_framework->get_module().as<uintptr_t>() + 0x3FD690);
        setStageFunc setStage = (setStageFunc)setStageAddress;
        if (cBgCtrl) {
            setStage(cBgCtrl, _StgName, _StgAdd, _Arg1, _Arg2, inBossInfoDisp, inFadeType, inSetVolRate, inPause, a10);
        }
    }

    // Set weapon to Mk3
    void SetMk3Equip() {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t setEquipMk3Address = (g_framework->get_module().as<uintptr_t>() + 0x3E29C0);
        mSetEquipMk3Func setEquipMk3 = (mSetEquipMk3Func)setEquipMk3Address;
        if (mHRPc) {
            setEquipMk3(mHRPc);
        }
    }

    // Check if attacking
    bool CheckCanAttack() {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t checkCanAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D47B0);
        mCheckCanAttackFunc checkCanAttack = (mCheckCanAttackFunc)checkCanAttackAddress;
        if (mHRPc) {
            return checkCanAttack(mHRPc);
        }
        return false; // player ptr is invalid, treat as if we can't attack
    }

    // Check if guarding
    bool CheckGuardMotion(bool inDefGuardNoChk) {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t checkGuardMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E26C0);
        mCheckGuardMotionFunc checkGuardMotion = (mCheckGuardMotionFunc)checkGuardMotionAddress;
        if (mHRPc) {
            return checkGuardMotion(mHRPc, inDefGuardNoChk);
        }
        return true; // player ptr is invalid, treat as if we're guarding
    }

    // Check if eating a hit
    bool CheckHajikare() {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t checkHajikareAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D4BA0);
        mCheckHajikareFunc checkHajikare = (mCheckHajikareFunc)checkHajikareAddress;
        if (mHRPc) {
            return checkHajikare(mHRPc);
        }
        return true; // player ptr is invalid, treat as if we're taking a hit
    }

    // Check if clashing, call with -1
    bool CheckTsubazering(int unkn) {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t checkTsubazeringAddress = (g_framework->get_module().as<uintptr_t>() + 0x3DFFC0);
        mCheckTsubazeringFunc checkTsubazering = (mCheckTsubazeringFunc)checkTsubazeringAddress;
        if (mHRPc) {
            return checkTsubazering(mHRPc, unkn);
        }
        return true; // player ptr is invalid, treat as if we're clashing
    }

    // Check if dodging back or left or right, call with -1
    bool CheckSideStep(int unkn) {
        mHRPc* mHRPc = get_mHRPc();
        uintptr_t checkSideStepAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2730);
        mCheckSideStepFunc checkSideStep = (mCheckSideStepFunc)checkSideStepAddress;
        if (mHRPc) {
            return checkSideStep(mHRPc, unkn);
        }
        return true; // player ptr is invalid, treat as if we're dodging
    }
}
