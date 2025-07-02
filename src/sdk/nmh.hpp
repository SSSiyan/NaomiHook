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
    HrCamera* get_HrCamera();
    HrScreenFilter* get_HrScreenFilter();

	typedef bool(__thiscall* setStageFunc)(CBgCtrl* CBgCtrl, const char* _StgName, int _StgAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	void SetStage(const char* _StgName, int _StageAdd, int _Arg1, int _Arg2, bool inBossInfoDisp, int inFadeType, __int64 inSetVolRate, bool inPause, unsigned int a10);
	typedef bool(__thiscall* mSetVisibleFunc)(mHRPc* mHRPc, bool inVisible); // 0x3D6D90
	void SetVisible(bool inVisible);
	typedef bool(__thiscall* mPlayMotionFunc)(mHRPc* mHRPc, pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate); // 0x402CF0 // ida says __userpurge
	bool PlayMotion(pcMotion inMotNo, bool inLoop, int inStartFrame, bool inOverWrite, float inInterpolate);
	typedef char(__thiscall* mSetEquipFunc)(mHRPc* mHRPc, pcItem inID, bool inPowUp); // 0x3E2240
	void SetEquip(pcItem inID, bool inPowUp);
	typedef char(__thiscall* mSetEquipMk3Func)(mHRPc* mHRPc); // 0x3E29C0 
	void SetMk3Equip();
	typedef bool(__thiscall* mCheckCanAttackFunc)(mHRPc* mHRPc); // 0x3D47B0 
	bool CheckCanAttack();
	typedef bool(__thiscall* mCheckGuardMotionFunc)(mHRPc* mHRPc, bool inDefGuardNoChk); // 0x3E26C0 
	bool CheckGuardMotion(bool inDefGuardNoChk);
	typedef bool(__thiscall* mCheckHajikareFunc)(mHRPc* mHRPc); // 0x3D4BA0 
	bool CheckHajikare();
	typedef bool(__thiscall* mCheckTsubazeringFunc)(mHRPc* mHRPc, int unkn); // 0x3DFFC0 
	bool CheckTsubazering(int unkn);
	typedef bool(__thiscall* mCheckSideStepFunc)(mHRPc* mHRPc, int unkn); // call this with -1 // 0x3E2730
	bool CheckSideStep(int unkn);
	typedef  int(__thiscall* mSetInitNpcDatFunc)(mHRBattle* mHRBattle, int inResNo, enCharaType inChType, int inRepop, const Vec* inPos, const Vec* inRot, enPopReqType inPopType, bool inDisEnableCollision);
	int setInitNpcDat(int inResNo, enCharaType inChType, int inRepop, const Vec* inPos, const Vec* inRot, enPopReqType inPopType, bool inDisEnableCollision);
	typedef void(__thiscall* mStart777Func)(mHRPc* mHRPc);
	bool Start777();
	typedef void(__thiscall* mStartBarFunc)(mHRPc* mHRPc, bool inDontCallStencil, int inTick);
	bool StartBar(bool inDontCallStencil, int inTick);
	typedef void(__thiscall* mStartBellFunc)(mHRPc* mHRPc);
	bool StartBell();
	typedef void(__thiscall* mStartHopperFunc)(mHRPc* mHRPc);
	bool StartHopper();
	typedef void(__thiscall* mStartCherryFunc)(mHRPc* mHRPc);
	bool StartCherry();
	typedef char(__fastcall* mGetScreenPosFunc)(const Vec* Src, Vec* Dest);
	bool GetScreenPos(Vec* Src, Vec* Dest);
	typedef bool(__thiscall* mCheckThrowAttackFunc)(mHRPc* mHRPc, int a2);
	bool CheckThrowAttack();
	typedef uint32_t(__thiscall* mGetLaserColorFunc)(mHRPc* mHRPc);
	uint32_t GetLaserColor();
	// typedef void(__userpurge* mSetLightReflecteFunc)(mHRPc* mHRPc, float strength, const Vec* inPos, unsigned int inCol, int a5);
	void SetLightReflect(mHRPc* player, float strength, Vec* inPos, int inCol, int unkn);
};
