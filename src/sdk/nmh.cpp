#include "nmh.hpp"

namespace nmh_sdk {
	mHRBattle* get_mHRBattle() {
		static uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		if (gpBattle)
			return gpBattle;
		 return NULL;
	}
	mHRPc* get_mHRPc() {
		static mHRBattle* gpBattle = get_mHRBattle();
		if (gpBattle && gpBattle->mpPc)
			return gpBattle->mpPc;
		return NULL;
	}
	CBgCtrl* get_CBgCtrl() {
		static uintptr_t CBgCtrlPtr = g_framework->get_module().as<uintptr_t>() + 0x8446F0;
		CBgCtrl* cBgCtrl = (CBgCtrl*)*(uintptr_t*)CBgCtrlPtr;
		if (cBgCtrl)
			return cBgCtrl;
		return NULL;
	}

    uintptr_t* GetPlayerPtr(void){
        uintptr_t gpBattleAddr = g_framework->get_module().as<uintptr_t>() + 0x843584;
        if (uintptr_t* gpBattle = (uintptr_t*)gpBattleAddr) {
            if (uintptr_t* gpBattleBase = *(uintptr_t**)gpBattle) {
                uintptr_t* mHRPcAddr = (uintptr_t*)((char*)gpBattleBase + 0x164); // add as bytes
                uintptr_t* mHRPc = *(uintptr_t**)mHRPcAddr;
                return mHRPc;
            }
        }
        return NULL;
    }

    void PlayMotion(int inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate) {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t playMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x402CF0);
        mPlayMotionFunc playMotion = (mPlayMotionFunc)playMotionAddress;
        if (mHRPc) {
            playMotion(mHRPc, inMotNo, inLoop, inStartFrame, inOverWrite, inInterpolate);
        }
    }

    void SetEquip(int inID, bool inPowUp) {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t setEquipAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2240);
        mSetEquipFunc setEquip = (mSetEquipFunc)setEquipAddress;
        if (mHRPc) {
            setEquip(mHRPc, inID, inPowUp);
        }
    }

    void SetMk3Equip() {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t setEquipMk3Address = (g_framework->get_module().as<uintptr_t>() + 0x3E29C0);
        mSetEquipMk3Func setEquipMk3 = (mSetEquipMk3Func)setEquipMk3Address;
        if (mHRPc) {
            setEquipMk3(mHRPc);
        }
    }

    bool CheckCanAttack() {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t checkCanAttackAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D47B0);
        mCheckCanAttackFunc checkCanAttack = (mCheckCanAttackFunc)checkCanAttackAddress;
        if (mHRPc) {
            return checkCanAttack(mHRPc);
        }
        return false; // player ptr is invalid, treat as if we can't attack
    }

    bool CheckGuardMotion(bool inDefGuardNoChk) {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t checkGuardMotionAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E26C0);
        mCheckGuardMotionFunc checkGuardMotion = (mCheckGuardMotionFunc)checkGuardMotionAddress;
        if (mHRPc) {
            return checkGuardMotion(mHRPc, inDefGuardNoChk);
        }
        return true; // player ptr is invalid, treat as if we're guarding
    }

    bool CheckHajikare() {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t checkHajikareAddress = (g_framework->get_module().as<uintptr_t>() + 0x3D4BA0);
        mCheckHajikareFunc checkHajikare = (mCheckHajikareFunc)checkHajikareAddress;
        if (mHRPc) {
            return checkHajikare(mHRPc);
        }
        return true; // player ptr is invalid, treat as if we're taking a hit
    }

    bool CheckTsubazering() {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t checkTsubazeringAddress = (g_framework->get_module().as<uintptr_t>() + 0x3DFFC0);
        mCheckTsubazeringFunc checkTsubazering = (mCheckTsubazeringFunc)checkTsubazeringAddress;
        if (mHRPc) {
            return checkTsubazering(mHRPc, -1);
        }
        return true; // player ptr is invalid, treat as if we're clashing
    }

    bool CheckSideStep(int unkn) {
        uintptr_t* mHRPc = GetPlayerPtr();
        uintptr_t checkSideStepAddress = (g_framework->get_module().as<uintptr_t>() + 0x3E2730);
        mCheckSideStepFunc checkSideStep = (mCheckSideStepFunc)checkSideStepAddress;
        if (mHRPc) {
            return checkSideStep(mHRPc, unkn);
        }
        return true; // player ptr is invalid, treat as if we're dodging
    }
}
