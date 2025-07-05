#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class Tony : public Mod {
public:
  Tony() = default;
  
  ModCategory get_category() { return ModCategory::COSMETICS; };
  static bool mod_enabled;
  static uintptr_t jmp_ret1;
  static uintptr_t jmp_ret2;
  static uintptr_t jmp_ret3;
  static uintptr_t jmp_ret4;
  static uintptr_t jmp_ret5;

  // mod name string for config
  std::string get_mod_name() const override { return "Tony"; }
  std::string get_human_readable_name() const { return "Arcade Scoring"; }
  const char* get_description() const override { return R"(Enable NaomiHook's custom scoring feed and keep track of how much your attacks are earning you! Each attack you land is displayed on the left-hand side of the screen with its name and LB$ value. The money earned from your kill is also shown as a Kill Reward.)"; };

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  void on_config_load(const utility::Config& cfg) override;
  void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	// std::unique_ptr<FunctionHook> m_hook1;   
	// std::unique_ptr<FunctionHook> m_hook2;
	std::unique_ptr<FunctionHook> m_hook3;
	std::unique_ptr<FunctionHook> m_hook4;
	std::unique_ptr<FunctionHook> m_hook5;
	// std::unique_ptr<Patch> m_patch;
};

static const char* MoveNames[] = {
  "ePcMtIdlLp", // 0,
  "ePcMtIdl0", // 1,
  "ePcMtIdl1", // 2,
  "ePcMtBtIdl0", // 3,
  "ePcMtBtIdl1", // 4,
  "ePcMtBtDSFnsh", // 5,
  "ePcMtWlkLp", // 6,
  "ePcMtWlkEd", // 7,
  "ePcMtWlkEdL", // 8,
  "ePcMtWlkEdR", // 9,
  "ePcMtWlkTrn", // 10,
  "ePcMtRnLp", // 11,
  "ePcMtRnEd", // 12,
  "ePcMtRnTrn", // 13,
  "ePcMtDshLp", // 14,
  "ePcMtDshEd", // 15,
  "ePcMtTrdSt", // 16,
  "ePcMtTrdLp", // 17,
  "ePcMtTrdEd", // 18,
  "ePcMtPhnIdlLp", // 19,
  "ePcMtPhnWlkLp", // 20,
  "ePcMtElShckIdlLp", // 21,
  "ePcMtElShckRnLp", // 22,
  "ePcMtAtkEdWstR", // 23,
  "ePcMtBtDefRunR", // 24,
  "ePcMtBtDefRunL", // 25,
  "ePcMtStpF", // 26, // Roll Forward
  "ePcMtRollB", // 27, // Roll Back
  "ePcMtAvdR", // 28, // Darkstep Right
  "ePcMtAvdL", // 29, // Darkstep Left
  "ePcMtRollR", // 30,
  "ePcMtRollL", // 31,
  "ePcMtAvdFStrt", // 32,
  "ePcMtAvdFAr", // 33,
  "ePcMtAvdFEd", // 34,
  "ePcMtStairsUp", // 35,
  "ePcMtStairsDown", // 36,
  "ePcMtBtChgWlkF", // 37,
  "ePcMtBtChgWlkB", // 38,
  "ePcMtBtChgWlkL", // 39,
  "ePcMtBtChgWlkR", // 40,
  "ePcMtTbLpGd", // 41,
  "ePcMtTbLp", // 42,
  "ePcMtTbLpBd", // 43,
  "ePcMtBtryChrgSt", // 44, // Start Charging
  "ePcMtBtryChrgLp", // 45, // Continue Charging
  "ePcMtBtryChrgEnd", // 46,
  "ePcMtBtryChrgQkLp", // 47,
  "ePcMtGrdDfltLp", // 48,
  "ePcMtGrdDfltLpD", // 49,
  "ePcMtGrdDfltDmg", // 50,
  "ePcMtGrdDflt2RU", // 51,
  "ePcMtGrdDflt2R", // 52,
  "ePcMtGrdDflt2RD", // 53,
  "ePcMtGrdDflt2LU", // 54,
  "ePcMtGrdDflt2L", // 55,
  "ePcMtGrdDflt2LD", // 56,
  "ePcMtGrdRULp", // 57,
  "ePcMtGrdRUDmg", // 58,
  "ePcMtGrdRU2R", // 59,
  "ePcMtGrdRU2RD", // 60,
  "ePcMtGrdRU2LU", // 61,
  "ePcMtGrdRU2L", // 62,
  "ePcMtGrdRU2LD", // 63,
  "ePcMtGrdRLp", // 64,
  "ePcMtGrdRDmg", // 65,
  "ePcMtGrdR2RU", // 66,
  "ePcMtGrdR2RD", // 67,
  "ePcMtGrdR2LU", // 68,
  "ePcMtGrdR2L", // 69,
  "ePcMtGrdR2LD", // 70,
  "ePcMtGrdRDLp", // 71,
  "ePcMtGrdRDDmg", // 72,
  "ePcMtGrdRD2RU", // 73,
  "ePcMtGrdRD2R", // 74,
  "ePcMtGrdRD2LU", // 75,
  "ePcMtGrdRD2L", // 76,
  "ePcMtGrdRD2LD", // 77,
  "ePcMtGrdLULp", // 78,
  "ePcMtGrdLUDmg", // 79,
  "ePcMtGrdLU2RU", // 80,
  "ePcMtGrdLU2R", // 81,
  "ePcMtGrdLU2RD", // 82,
  "ePcMtGrdLU2L", // 83,
  "ePcMtGrdLU2LD", // 84,
  "ePcMtGrdLLp", // 85,
  "ePcMtGrdLDmg", // 86,
  "ePcMtGrdL2RU", // 87,
  "ePcMtGrdL2R", // 88,
  "ePcMtGrdL2RD", // 89,
  "ePcMtGrdL2LU", // 90,
  "ePcMtGrdL2LD", // 91,
  "ePcMtGrdLDLp", // 92,
  "ePcMtGrdLDDmg", // 93,
  "ePcMtGrdLD2RU", // 94,
  "ePcMtGrdLD2R", // 95,
  "ePcMtGrdLD2RD", // 96,
  "ePcMtGrdLD2LU", // 97,
  "ePcMtGrdLD2L", // 98,
  "ePcMtGrdRngR", // 99,
  "ePcMtGrdRngL", // 100,
  "ePcMtGrdRngBck", // 101,
  "ePcMtDmgSmlR", // 102,
  "ePcMtDmgMidR", // 103,
  "ePcMtDmgSmlL", // 104,
  "ePcMtDmgMidL", // 105,
  "ePcMtDmgSmlB", // 106,
  "ePcMtDmgMidB", // 107,
  "ePcMtDmgSmlF", // 108,
  "ePcMtDmgMidF", // 109,
  "ePcMtDmgNBR", // 110,
  "ePcMtDmgNBRLnd", // 111,
  "ePcMtDmgNBL", // 112,
  "ePcMtDmgNBLLnd", // 113,
  "ePcMtDmgNBB", // 114,
  "ePcMtDmgNBBLnd", // 115,
  "ePcMtDmgNBF", // 116,
  "ePcMtDmgNBFLnd", // 117,
  "ePcMtDwnB", // 118,
  "ePcMtDwnBBtn", // 119,
  "ePcMtDwnB2Stnd", // 120,
  "ePcMtDmgDwnB", // 121,
  "ePcMtDwnF", // 122,
  "ePcMtDwnFBtn", // 123,
  "ePcMtDwnF2Stnd", // 124,
  "ePcMtDmgDwnF", // 125,
  "ePcMtHjkr", // 126,
  "ePcMtTbOt1", // 127,
  "ePcMtDdStd", // 128,
  "ePcMtDdAomk", // 129,
  "ePcMtDdUtsbs", // 130,
  "ePcMtLSFly", // 131,
  "ePcMtLSRtn", // 132,
  "BEAT Combo",						    // 133, ePcMtBtAtkFghtCmbA
  "BEAT Combo",						    // 134, ePcMtBtAtkFghtCmbB
  "BEAT Combo",						    // 135, ePcMtBtAtkFghtCmbC
  "BEAT Combo",						    // 136, ePcMtBtAtkFghtCmbD
  "BEAT Combo",						    // 137, ePcMtBtAtkFghtCmbE
  "BEAT Combo",						    // 138, ePcMtBtAtkFghtCmbF
  "While-Running Left Hook",			// 139, ePcMtBtAtkFghtDsh
  "Spinning Hook Kick",				    // 140, ePcMtBtAtkFghtRrgSbt
  "Roundhouse Kick",					// 141, ePcMtBtAtkFghtRHKck
  "Ankle Kick",						    // 142, ePcMtBtAtkFghtRLKck
  "Hiza Geri Knee Kick",				// 143, ePcMtBtAtkFghtTbgr
  "ePcMtBtAtkFghtRlt",				    // 144, ePcMtBtAtkFghtRlt Unused Black Screen Punch
  "Overhead Kick",						// 145, ePcMtBtAtkFghtNlKck
  "Right Elbow",						// 146, ePcMtBtAtkFghtRrgElbw
  "Spinning Back Kick",				    // 147, ePcMtBtAtkFghtTrKick
  "Left Hook",							// 148, ePcMtBtAtkFghtLHk
  "Left Uppercut",						// 149, ePcMtBtAtkFghtLUpr
  "Jumping Spinning Back Kick",			// 150, ePcMtBtAtkFghtRrgSbt2
  "Dragon Suplex",						// 151, ePcMtBtAtkFghtDsprx
  "Brain Buster",						// 152, ePcMtBtAtkFghtBbstr
  "DDT ",								// 153, ePcMtBtAtkFghtDdt
  "Tombstone Piledriver",				// 154, ePcMtBtAtkFghtPdrvr
  "Hurricanrana",						// 155, ePcMtBtAtkFghtFstnr
  "Powerbomb",							// 156, ePcMtBtAtkFghtPbmb
  "Brain Buster Slam",					// 157, ePcMtBtAtkFght53
  "Quebradora Con Giro",				// 158, ePcMtBtAtkFghtKbrd
  "German Suplex",						// 159, ePcMtBtAtkFghtJmsprx
  "Tiger Suplex",						// 160, ePcMtBtAtkFghtTgsprx
  "Belly to Belly",					    // 161, ePcMtBtAtkFghtSldr
  "Front Neck Chancery Drop",			// 162, ePcMtBtAtkFghtFnck
  "Captured",							// 163, ePcMtBtAtkFghtCpchd
  "Reverse Armsault",					// 164, ePcMtBtAtkFghtRvsams
  "Double Arm Suplex",					// 165, ePcMtBtAtkFghtKnnk
  "Double Wrist Suplex",				// 166, ePcMtBtAtkFghtWwrst
  "ePcMtBtChgStrt", // 167,
  "ePcMtBtChgLp", // 168,
  "Mid Charged Slash",				    // 169, ePcMtBtAtkChg
  "High Charged Slash",				    // 170, ePcMtBtAtkChgUp
  "Running Slash",						// 171, ePcMtBtAtkNrmlDsh
  "Throw Bonus!",						// 172, ePcMtBtCtch Wrestling Start
  "ePcMtBtCtchMss", // 173,
  "Blueberry Cheese Brownie",			// 174, ePcMtBtLSSonic
  "ePcMtBtLSBombSt", // 175,
  "ePcMtBtLSBombLp", // 176,
  "ePcMtBtLSBombEd", // 177,
  "ePcMtBtDSIdl", // 178,
  "ePcMtBtDSRn", // 179,
  "DARKSIDE KILL", // 180, ePcMtBtDSAtkR
  "DARKSIDE KILL", // 181, ePcMtBtDSAtkL
  "DARKSIDE KILL", // 182, ePcMtBtDSAtkB
  "DARKSIDE KILL", // 183, ePcMtBtDSAtkFDwn
  "DARKSIDE KILL", // 184, ePcMtBtDSAtkBDwn
  "DARKSIDE KILL", // 185, ePcMtBtDSAtkMdR Rapid Slash
  "DARKSIDE KILL", // 186, ePcMtBtDSAtkMdL Stinger
  "DARKSIDE KILL", // 187, ePcMtBtDSAtkMdB
  "DARKSIDE KILL", // 188, ePcMtBtDSAtkMdFDwn
  "DARKSIDE KILL", // 189, ePcMtBtDSAtkMdBDwn
  "ePcMtBtDSAtkLgSt", // 190, ePcMtBtDSAtkLgSt
  "ePcMtBtDSAtkLgLp", // 191, ePcMtBtDSAtkLgLp
  "ePcMtBtDSAtkLgEd", // 192, ePcMtBtDSAtkLgEd
  "ePcMtBtAtkDwnB", // 193,
  "Jumping Slash",						 // 194, ePcMtBtDashAtk
  "Reversal!",							 // 195, ePcMtBtOkiAtk
  "ePcMtBtJmpDwnSt", // 196,
  "ePcMtBtJmpDwnAr", // 197,
  "ePcMtBtJmpDwnAt", // 198,
  "ePcMtBtPullBK", // 199,
  "ePcMtStdDoorOpen", // 200,
  "ePcMtStdBoxKick", // 201,
  "ePcMtStdPickUp", // 202,
  "ePcMtStdPickUpOk", // 203,
  "ePcMtStdPickUpMiss", // 204,
  "NICE THROW!", // 205, Boss Throw?
  "ePcMtBt01IdlU", // 206,
  "ePcMtBt01IdlM", // 207,
  "ePcMtBt01IdlD", // 208,
  "ePcMtBt01WlkUF", // 209,
  "ePcMtBt01WlkMF", // 210,
  "ePcMtBt01WlkBF", // 211,
  "ePcMtBt01WlkUB", // 212,
  "ePcMtBt01WlkMB", // 213,
  "ePcMtBt01WlkBB", // 214,
  "ePcMtBt01WlkUL", // 215,
  "ePcMtBt01WlkML", // 216,
  "ePcMtBt01WlkBL", // 217,
  "ePcMtBt01WlkUR", // 218,
  "ePcMtBt01WlkMR", // 219,
  "ePcMtBt01WlkBR", // 220,
  "ePcMtBt01Rn", // 221,
  "ePcMtBt01RnEd", // 222,
  "[Blood Berry] High Slash",               // 223, ePcMtBtAtk01CmbUpA
  "[Blood Berry] High Slash",			     // 224, ePcMtBtAtk01CmbUpB
  "[Blood Berry] High Slash",               // 225, ePcMtBtAtk01CmbUpC
  "[Blood Berry] High Slash",               // 226, ePcMtBtAtk01CmbUpD
  "[Blood Berry] DEATH BLOW",			     // 227, ePcMtBtAtk01CmbUpFnsh
  "[Blood Berry] Mid Slash",                // 228, ePcMtBtAtk01CmbMidA
  "[Blood Berry] Mid Slash",                // 229, ePcMtBtAtk01CmbMidB
  "[Blood Berry] Mid Slash",                // 230, ePcMtBtAtk01CmbMidC
  "[Blood Berry] Mid Slash",                // 231, ePcMtBtAtk01CmbMidD
  "[Blood Berry] Mid Slash",                // 232, ePcMtBtAtk01CmbMidE
  "[Blood Berry] Mid Slash",                // 233, ePcMtBtAtk01CmbMidF
  "[Blood Berry] Mid Slash",                // 234, ePcMtBtAtk01CmbMidG
  "[Blood Berry] Mid Slash",                // 235, ePcMtBtAtk01CmbMidH
  "[Blood Berry] DEATH BLOW",			     // 236, ePcMtBtAtk01CmbMidFnsh
  "[Blood Berry] Low Slash",			     // 237, ePcMtBtAtk01CmbBtmA
  "[Blood Berry] Low Slash",			     // 238, ePcMtBtAtk01CmbBtmB
  "[Blood Berry] Low Slash",			     // 239, ePcMtBtAtk01CmbBtmC
  "[Blood Berry] Low Slash",			     // 240, ePcMtBtAtk01CmbBtmD
  "[Blood Berry] Low Slash",			     // 241, ePcMtBtAtk01CmbBtmE
  "[Blood Berry] Low Slash",			     // 242, ePcMtBtAtk01CmbBtmF
  "[Blood Berry] Low Slash",			     // 243, ePcMtBtAtk01CmbBtmG
  "[Blood Berry] DEATH BLOW",			     // 244, ePcMtBtAtk01CmbBtmFnsh
  "[Blood Berry] DEATH BLOW",			     // 245, ePcMtBtAtk01CmbR2LFnsh
  "[Blood Berry] Low Charged Slash",		 // 246, ePcMtBtAtk01Rng
  "[Blood Berry] Execution",				 // 247, ePcMtBtAtk01Dwn
  "ePcMtBtAtk01Tb", // 248,
  "ePcMtBattou01", // 249,
  "ePcMtBattou01Lp", // 250,
  "ePcMtLSBattou01", // 251,
  "ePcMtLSNoutou01", // 252,
  "ePcMtBtAtk01Max", // 253,
  "ePcMtBt02IdlU", // 254,
  "ePcMtBt02IdlM", // 255,
  "ePcMtBt02IdlD", // 256,
  "ePcMtBt02WlkUF", // 257,
  "ePcMtBt02WlkMF", // 258,
  "ePcMtBt02WlkBF", // 259,
  "ePcMtBt02WlkUB", // 260,
  "ePcMtBt02WlkMB", // 261,
  "ePcMtBt02WlkBB", // 262,
  "ePcMtBt02WlkUL", // 263,
  "ePcMtBt02WlkML", // 264,
  "ePcMtBt02WlkBL", // 265,
  "ePcMtBt02WlkUR", // 266,
  "ePcMtBt02WlkMR", // 267,
  "ePcMtBt02WlkBR", // 268,
  "ePcMtBt02Rn", // 269,
  "ePcMtBt02RnEd", // 270,
  "[MK-III] High Slash",			 // 271, ePcMtBtAtk02CmbUpA
  "[MK-III] High Slash",			 // 272, ePcMtBtAtk02CmbUpB
  "[MK-III] High Slash",			 // 273, ePcMtBtAtk02CmbUpC
  "[MK-III] High Slash",			 // 274, ePcMtBtAtk02CmbUpD
  "[MK-III] High Slash",			 // 275, ePcMtBtAtk02CmbUpE
  "[MK-III] High Slash",			 // 276, ePcMtBtAtk02CmbUpF
  "[MK-III] High Slash",			 // 277, ePcMtBtAtk02CmbUpG
  "[MK-III] DEATH BLOW",			 // 278, ePcMtBtAtk02CmbUpFnsh AniObi Spin
  "[MK-III] Low Slash",			 // 279, ePcMtBtAtk02CmbBtmA
  "[MK-III] Low Slash",			 // 280, ePcMtBtAtk02CmbBtmB
  "[MK-III] Low Slash",			 // 281, ePcMtBtAtk02CmbBtmC
  "[MK-III] Low Slash",			 // 282, ePcMtBtAtk02CmbBtmD
  "[MK-III] Low Slash",			 // 283, ePcMtBtAtk02CmbBtmE
  "[MK-III] Low Slash",			 // 284, ePcMtBtAtk02CmbBtmF
  "[MK-III] Low Slash",			 // 285, ePcMtBtAtk02CmbBtmG
  "[MK-III] Low Slash",			 // 286, ePcMtBtAtk02CmbBtmH
  "[MK-III] DEATH BLOW",			 // 287, ePcMtBtAtk02CmbBtmFnsh
  "[MK-III] DEATH BLOW",			 // 288, ePcMtBtAtk02CmbL2RFnsh
  "[MK-III] DEATH BLOW",			 // 289, ePcMtBtAtk02CmbD2UFnsh
  "[MK-III] Low Charge Combo A",    // 290,ePcMtBtAtk02RngCmbA
  "[MK-III] Low Charge Combo B",    // 291,ePcMtBtAtk02RngCmbB
  "[MK-III] Low Charge Combo C",    // 292,ePcMtBtAtk02RngCmbC
  "[MK-III] Execution",		     // 293, ePcMtBtAtk02Dwn
  "ePcMtBtAtk02Tb", // 294,
  "ePcMtBattou02", // 295,
  "ePcMtBattou02Lp", // 296,
  "ePcMtBattou02Ed", // 297,
  "ePcMtLSBattou021", // 298,
  "ePcMtLSBattou022", // 299,
  "ePcMtLSNoutou02", // 300,
  "ePcMtBtAtk02Max", // 301,
  "ePcMtBt03IdlU", // 302,
  "ePcMtBt03IdlM", // 303,
  "ePcMtBt03IdlD", // 304,
  "ePcMtBt03WlkUF", // 305,
  "ePcMtBt03WlkMF", // 306,
  "ePcMtBt03WlkBF", // 307,
  "ePcMtBt03WlkUB", // 308,
  "ePcMtBt03WlkMB", // 309,
  "ePcMtBt03WlkBB", // 310,
  "ePcMtBt03WlkUL", // 311,
  "ePcMtBt03WlkML", // 312,
  "ePcMtBt03WlkBL", // 313,
  "ePcMtBt03WlkUR", // 314,
  "ePcMtBt03WlkMR", // 315,
  "ePcMtBt03WlkBR", // 316,
  "ePcMtBt03Rn",    // 317,
  "ePcMtBt03RnEd",  // 318,
  "[MK-I] High Slash",			   // 319, ePcMtBtAtk03CmbUpA
  "[MK-I] High Slash",			   // 320, ePcMtBtAtk03CmbUpB
  "[MK-I] High Slash",			   // 321, ePcMtBtAtk03CmbUpC
  "[MK-I] High Slash",			   // 322, ePcMtBtAtk03CmbUpD
  "[MK-I] High Slash",			   // 323, ePcMtBtAtk03CmbUpE
  "[MK-I] DEATH BLOW",			   // 324,
  "[MK-I] Mid Slash",			   // 325, ePcMtBtAtk03CmbMidA
  "[MK-I] Mid Slash",			   // 326, ePcMtBtAtk03CmbMidB
  "[MK-I] Mid Slash",			   // 327, ePcMtBtAtk03CmbMidC
  "[MK-I] Mid Slash",			   // 328, ePcMtBtAtk03CmbMidD
  "[MK-I] Mid Slash",			   // 329, ePcMtBtAtk03CmbMidE
  "[MK-I] Mid Slash",			   // 330, ePcMtBtAtk03CmbMidF
  "[MK-I] Mid Slash",			   // 331, ePcMtBtAtk03CmbMidG
  "[MK-I] Mid Slash",			   // 332, ePcMtBtAtk03CmbMidH
  "[MK-I] DEATH BLOW",			   // 333, ePcMtBtAtk03CmbMidFnsh
  "[MK-I] Low Slash",			   // 334, ePcMtBtAtk03CmbBtmA
  "[MK-I] Low Slash",			   // 335,
  "[MK-I] Low Slash",			   // 336, ePcMtBtAtk03CmbBtmC
  "[MK-I] Low Slash",			   // 337, ePcMtBtAtk03CmbBtmD
  "[MK-I] DEATH BLOW",			   // 338, ePcMtBtAtk03CmbBtmFnsh
  "[MK-I] DEATH BLOW",			   // 339, ePcMtBtAtk03CmbL2RFnsh
  "[MK-I] Low Charged Slash",      // 340, ePcMtBtAtk03Rng
  "[MK-I] Execution",              // 341, ePcMtBtAtk03Dwn
  "ePcMtBtAtk03Tb", // 342,
  "ePcMtBattou03", // 343,
  "ePcMtBattou03Lp", // 344,
  "ePcMtBattou03Ed", // 345,
  "ePcMtLSBattou03", // 346,
  "ePcMtLSNoutou03", // 347,
  "ePcMtBtAtk03Max", // 348,
  "ePcMtBt04IdlU", // 349,
  "ePcMtBt04IdlM", // 350,
  "ePcMtBt04IdlD", // 351,
  "ePcMtBt04WlkUF", // 352,
  "ePcMtBt04WlkMF", // 353,
  "ePcMtBt04WlkBF", // 354,
  "ePcMtBt04WlkUB", // 355,
  "ePcMtBt04WlkMB", // 356,
  "ePcMtBt04WlkBB", // 357,
  "ePcMtBt04WlkUL", // 358,
  "ePcMtBt04WlkML", // 359,
  "ePcMtBt04WlkBL", // 360,
  "ePcMtBt04WlkUR", // 361,
  "ePcMtBt04WlkMR", // 362,
  "ePcMtBt04WlkBR", // 363,
  "ePcMtBt04Rn", // 364,
  "ePcMtBt04RnEd", // 365,
  "[MK-II] High Slash",			 // 366, ePcMtBtAtk04CmbUpA
  "[MK-II] High Slash",			 // 367, ePcMtBtAtk04CmbUpB
  "[MK-II] High Slash",			 // 368, ePcMtBtAtk04CmbUpC
  "[MK-II] High Slash",			 // 369, ePcMtBtAtk04CmbUpD
  "[MK-II] DEATH BLOW",			 // 370, ePcMtBtAtk04CmbUpFnsh
  "[MK-II] Mid Slash",			     // 371, ePcMtBtAtk04CmbMidA
  "[MK-II] Mid Slash",			     // 372, ePcMtBtAtk04CmbMidB
  "[MK-II] Mid Slash",			     // 373, ePcMtBtAtk04CmbMidC
  "[MK-II] Mid Slash",			     // 374, ePcMtBtAtk04CmbMidD
  "[MK-II] Mid Slash",			     // 375, ePcMtBtAtk04CmbMidE
  "[MK-II] Mid Slash",			     // 376, ePcMtBtAtk04CmbMidF
  "[MK-II] Mid Slash",			     // 377, ePcMtBtAtk04CmbMidG
  "[MK-II] DEATH BLOW",			 // 378, ePcMtBtAtk04CmbMidFnsh
  "[MK-II] Low Slash",			     // 379, ePcMtBtAtk04CmbBtmA
  "[MK-II] Low Slash",			     // 380, ePcMtBtAtk04CmbBtmB
  "[MK-II] Low Slash",			     // 381, ePcMtBtAtk04CmbBtmC
  "[MK-II] Low Slash",			     // 382, ePcMtBtAtk04CmbBtmD
  "[MK-II] Low Slash",			     // 383, ePcMtBtAtk04CmbBtmE
  "[MK-II] DEATH BLOW",			 // 384, ePcMtBtAtk04CmbBtmFnsh
  "[MK-II] DEATH BLOW",			 // 385, ePcMtBtAtk04CmbR2LFnsh
  "[MK-II] Low Charged Slash",      // 386, ePcMtBtAtk04Rng
  "[MK-II] Execution",			     // 387, ePcMtBtAtk04Dwn
  "ePcMtBtAtk04Tb", // 388,
  "ePcMtBattou04", // 389,
  "ePcMtBattou04Lp", // 390,
  "ePcMtBattou04Ed", // 391,
  "ePcMtLSBattou041", // 392,
  "ePcMtLSBattou042", // 393,
  "ePcMtLSNoutou04", // 394,
  "ePcWepMtUpFnsh04", // 395,
  "ePcWepMt04MidF", // 396,
  "ePcWepMt04MidG", // 397,
  "ePcWepMt04MidFnsh", // 398,
  "ePcWepMt04BtmE", // 399,
  "ePcWepMt04Rng", // 400,
  "ePcWepMt04Dwn", // 401,
  "ePcMtBtAtk04Max", // 402,
};