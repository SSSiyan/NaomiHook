#include "nmh.hpp"

namespace nmh_sdk {
	mHRPc* get_local_player() {
		uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		static mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		if (gpBattle->mpPc)
			return gpBattle->mpPc;
		return NULL;
	}
	mHRBattle* get_mediator() {
		uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		static mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		if (gpBattle)
			return (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		 return NULL;
	}
}
