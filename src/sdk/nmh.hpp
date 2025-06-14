#pragma once
#include "ReClass.hpp"
#include "ModFramework.hpp"
namespace nmh_sdk {
	mHRPc* get_mHRPc();
	mHRBattle* get_mHRBattle();
	CBgCtrl* get_CBgCtrl();
	HrMenuTask* get_HrMenuTask();
	DolphinApp* get_DolphinApp();
	HrGameTask* get_HrGameTask();

	typedef bool(__thiscall* setStageFunc)(CBgCtrl* CBgCtrl, const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	typedef bool(__thiscall* mSetVisibleFunc)(mHRPc* mHRPc, bool inVisible); // 0x3D6D90
	typedef bool(__thiscall* mPlayMotionFunc)(mHRPc* mHRPc, pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate); // 0x402CF0 // ida says __userpurge
	typedef char(__thiscall* mSetEquipFunc)(mHRPc* mHRPc, pcItem inID, bool inPowUp); // 0x3E2240
	typedef char(__thiscall* mSetEquipMk3Func)(mHRPc* mHRPc); // 0x3E29C0 
	typedef bool(__thiscall* mCheckCanAttackFunc)(mHRPc* mHRPc); // 0x3D47B0 
	typedef bool(__thiscall* mCheckGuardMotionFunc)(mHRPc* mHRPc, bool inDefGuardNoChk); // 0x3E26C0 
	typedef bool(__thiscall* mCheckHajikareFunc)(mHRPc* mHRPc); // 0x3D4BA0 
	typedef bool(__thiscall* mCheckTsubazeringFunc)(mHRPc* mHRPc, int unkn); // 0x3DFFC0 
	typedef bool(__thiscall* mCheckSideStepFunc)(mHRPc* mHRPc, int unkn); // call this with -1 // 0x3E2730
	typedef  int(__thiscall* mSetInitNpcDatFunc)(mHRBattle* mHRBattle, int inResNo, enCharaType inChType, int inRepop, const Vec* inPos, const Vec* inRot, enPopReqType inPopType, bool inDisEnableCollision);
	typedef void(__thiscall* mStart777Func)(mHRPc* mHRPc);
	typedef void(__thiscall* mStartBarFunc)(mHRPc* mHRPc, bool inDontCallStencil, int inTick);
	typedef void(__thiscall* mStartBellFunc)(mHRPc* mHRPc);
	typedef void(__thiscall* mStartHopperFunc)(mHRPc* mHRPc);
	typedef void(__thiscall* mStartCherryFunc)(mHRPc* mHRPc);
	typedef char(__fastcall* mGetScreenPosFunc)(const Vec* Src, Vec* Dest);
	void SetStage(const char* _StgName, int _StageAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	void SetVisible(bool inVisible);
	bool PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate);
	void SetEquip(pcItem inID, bool inPowUp);
	void SetMk3Equip();
	bool CheckCanAttack();
	bool CheckGuardMotion(bool inDefGuardNoChk);
	bool CheckHajikare();
	bool CheckTsubazering(int unkn);
	bool CheckSideStep(int unkn);
	int setInitNpcDat(int inResNo, enCharaType inChType, int inRepop, const Vec* inPos, const Vec* inRot, enPopReqType inPopType, bool inDisEnableCollision);
	bool Start777();
	bool StartBar(bool inDontCallStencil, int inTick);
	bool StartBell();
	bool StartHopper();
	bool StartCherry();
	bool GetScreenPos(Vec* Src, Vec* Dest);
};
