#pragma once
#include "ReClass.hpp"
#include "ModFramework.hpp"
namespace nmh_sdk {
	mHRPc* get_mHRPc();
	mHRBattle* get_mHRBattle();
	CBgCtrl* get_CBgCtrl();

	typedef bool(__thiscall* mPlayMotionFunc)(uintptr_t* playerPtr, int inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate); // 0x402CF0 // ida says __userpurge
	typedef char(__thiscall* mSetEquipFunc)(uintptr_t* playerPtr, int inID, bool inPowUp); // 0x3E2240
	typedef char(__thiscall* mSetEquipMk3Func)(uintptr_t* playerPtr); // 0x3E29C0 
	typedef bool(__thiscall* mCheckCanAttackFunc)(uintptr_t* playerPtr); // 0x3D47B0 
	typedef bool(__thiscall* mCheckGuardMotionFunc)(uintptr_t* playerPtr, bool inDefGuardNoChk); // 0x3E26C0 
	typedef bool(__thiscall* mCheckHajikareFunc)(uintptr_t* playerPtr); // 0x3D4BA0 
	typedef bool(__thiscall* mCheckTsubazeringFunc)(uintptr_t* playerPtr, int unkn); // 0x3DFFC0 
	typedef bool(__thiscall* mCheckSideStepFunc)(uintptr_t* playerPtr, int unkn); // call this with -1 // 0x3E2730

	// these are player funcs but lets skip a step and just get player inside of them
	void PlayMotion(int inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate);
	void SetEquip(int inID, bool inPowUp);
	void SetMk3Equip();
	bool CheckCanAttack();
	bool CheckGuardMotion(bool inDefGuardNoChk);
	bool CheckHajikare();
	bool CheckTsubazering();
	bool CheckSideStep(int unkn);
};
