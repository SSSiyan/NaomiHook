#pragma once
#include "ReClass.hpp"
#include "ModFramework.hpp"
namespace nmh_sdk {
	mHRPc* get_mHRPc();
	mHRBattle* get_mHRBattle();
	CBgCtrl* get_CBgCtrl();

	typedef bool(__thiscall* mPlayMotionFunc)(mHRPc* mHRPc, pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate); // 0x402CF0 // ida says __userpurge
	typedef char(__thiscall* mSetEquipFunc)(mHRPc* mHRPc, pcItem inID, bool inPowUp); // 0x3E2240
	typedef char(__thiscall* mSetEquipMk3Func)(mHRPc* mHRPc); // 0x3E29C0 
	typedef bool(__thiscall* setStageFunc)(CBgCtrl* CBgCtrl, const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	typedef bool(__thiscall* mCheckCanAttackFunc)(mHRPc* mHRPc); // 0x3D47B0 
	typedef bool(__thiscall* mCheckGuardMotionFunc)(mHRPc* mHRPc, bool inDefGuardNoChk); // 0x3E26C0 
	typedef bool(__thiscall* mCheckHajikareFunc)(mHRPc* mHRPc); // 0x3D4BA0 
	typedef bool(__thiscall* mCheckTsubazeringFunc)(mHRPc* mHRPc, int unkn); // 0x3DFFC0 
	typedef bool(__thiscall* mCheckSideStepFunc)(mHRPc* mHRPc, int unkn); // call this with -1 // 0x3E2730

	// these are player funcs but lets skip a step and just get player inside of them
	void PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate);
	void SetEquip(pcItem inID, bool inPowUp);
	void SetStage(const char* _StgName, int _StageAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	void SetMk3Equip();
	bool CheckCanAttack();
	bool CheckGuardMotion(bool inDefGuardNoChk);
	bool CheckHajikare();
	bool CheckTsubazering(int unkn);
	bool CheckSideStep(int unkn);
};
