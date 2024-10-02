#include "nmh.hpp"

namespace nmh_sdk {
	mHRBattle* get_mediator() {
		static uintptr_t gpBattlePtr = g_framework->get_module().as<uintptr_t>() + 0x843584;
		mHRBattle* gpBattle = (mHRBattle*)*(uintptr_t*)gpBattlePtr;
		if (gpBattle)
			return gpBattle;
		 return NULL;
	}
	mHRPc* get_local_player() {
		static mHRBattle* gpBattle = get_mediator();
		if (gpBattle && gpBattle->mpPc)
			return gpBattle->mpPc;
		return NULL;
	}
}
