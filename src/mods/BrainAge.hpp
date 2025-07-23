#pragma once
#include "Mod.hpp"
#include "sdk/ReClass.hpp"
class BrainAge : public Mod {
public:
  BrainAge() = default;
  
  ModCategory get_category() { return ModCategory::SYSTEM; };

  static bool imguiPopout;

  // void custom_imgui_window();

  // mod name string for config
  std::string get_mod_name() const override { return "BrainAge"; }
  std::string get_human_readable_name() const { return "Brain Age"; }
  const char* get_description() const override { return R"(View info about Brain Age 3)"; };

  // called by m_mods->init() you'd want to override this
  std::optional<std::string> on_initialize() override;

  // Override this things if you want to store values in the config file
  //void on_config_load(const utility::Config& cfg) override;
  //void on_config_save(utility::Config& cfg) override;

  // on_frame() is called every frame regardless whether the gui shows up.
  void on_frame() override;
  // on_draw_ui() is called only when the gui shows up
  // you are in the imgui window here.
  void on_draw_ui() override;
  // on_draw_debug_ui() is called when debug window shows up
  //void on_draw_debug_ui() override;
private:
	// std::unique_ptr<FunctionHook> m_hook1;
	// std::unique_ptr<Patch> m_patch;
};

struct WeaponAnimations {
    const char* name;
    std::vector<std::pair<const char*, int>> animations;
};

static std::vector<WeaponAnimations> weaponAnimationData = {
    {"Blood Berry",
        {
            {"ePcMtBt01IdlU", 206},
            {"ePcMtBt01IdlM", 207},
            {"ePcMtBt01IdlD", 208},
            {"ePcMtBt01WlkUF", 209},
            {"ePcMtBt01WlkMF", 210},
            {"ePcMtBt01WlkBF", 211},
            {"ePcMtBt01WlkUB", 212},
            {"ePcMtBt01WlkMB", 213},
            {"ePcMtBt01WlkBB", 214},
            {"ePcMtBt01WlkUL", 215},
            {"ePcMtBt01WlkML", 216},
            {"ePcMtBt01WlkBL", 217},
            {"ePcMtBt01WlkUR", 218},
            {"ePcMtBt01WlkMR", 219},
            {"ePcMtBt01WlkBR", 220},
            {"ePcMtBt01Rn", 221},
            {"ePcMtBt01RnEd", 222},
            {"ePcMtBtAtk01CmbUpA", 223},
            {"ePcMtBtAtk01CmbUpB", 224},
            {"ePcMtBtAtk01CmbUpC", 225},
            {"ePcMtBtAtk01CmbUpD", 226},
            {"ePcMtBtAtk01CmbUpFnsh", 227},
            {"ePcMtBtAtk01CmbMidA", 228},
            {"ePcMtBtAtk01CmbMidB", 229},
            {"ePcMtBtAtk01CmbMidC", 230},
            {"ePcMtBtAtk01CmbMidD", 231},
            {"ePcMtBtAtk01CmbMidE", 232},
            {"ePcMtBtAtk01CmbMidF", 233},
            {"ePcMtBtAtk01CmbMidG", 234},
            {"ePcMtBtAtk01CmbMidH", 235},
            {"ePcMtBtAtk01CmbMidFnsh", 236},
            {"ePcMtBtAtk01CmbBtmA", 237},
            {"ePcMtBtAtk01CmbBtmB", 238},
            {"ePcMtBtAtk01CmbBtmC", 239},
            {"ePcMtBtAtk01CmbBtmD", 240},
            {"ePcMtBtAtk01CmbBtmE", 241},
            {"ePcMtBtAtk01CmbBtmF", 242},
            {"ePcMtBtAtk01CmbBtmG", 243},
            {"ePcMtBtAtk01CmbBtmFnsh", 244},
            {"ePcMtBtAtk01CmbR2LFnsh", 245},
            {"ePcMtBtAtk01Rng", 246},
            {"ePcMtBtAtk01Dwn", 247},
            {"ePcMtBtAtk01Tb", 248},
            {"ePcMtBattou01", 249},
            {"ePcMtBattou01Lp", 250},
        }
    },
    {"Tsubaki Mk-I",
        {
            {"ePcMtBt03IdlU", 302},
            {"ePcMtBt03IdlM", 303},
            {"ePcMtBt03IdlD", 304},
            {"ePcMtBt03WlkUF", 305},
            {"ePcMtBt03WlkMF", 306},
            {"ePcMtBt03WlkBF", 307},
            {"ePcMtBt03WlkUB", 308},
            {"ePcMtBt03WlkMB", 309},
            {"ePcMtBt03WlkBB", 310},
            {"ePcMtBt03WlkUL", 311},
            {"ePcMtBt03WlkML", 312},
            {"ePcMtBt03WlkBL", 313},
            {"ePcMtBt03WlkUR", 314},
            {"ePcMtBt03WlkMR", 315},
            {"ePcMtBt03WlkBR", 316},
            {"ePcMtBt03Rn", 317},
            {"ePcMtBt03RnEd", 318},
            {"ePcMtBtAtk03CmbUpA", 319},
            {"ePcMtBtAtk03CmbUpB", 320},
            {"ePcMtBtAtk03CmbUpC", 321},
            {"ePcMtBtAtk03CmbUpD", 322},
            {"ePcMtBtAtk03CmbUpE", 323},
            {"ePcMtBtAtk03CmbUpFnsh", 324},
            {"ePcMtBtAtk03CmbMidA", 325},
            {"ePcMtBtAtk03CmbMidB", 326},
            {"ePcMtBtAtk03CmbMidC", 327},
            {"ePcMtBtAtk03CmbMidD", 328},
            {"ePcMtBtAtk03CmbMidE", 329},
            {"ePcMtBtAtk03CmbMidF", 330},
            {"ePcMtBtAtk03CmbMidG", 331},
            {"ePcMtBtAtk03CmbMidH", 332},
            {"ePcMtBtAtk03CmbMidFnsh", 333},
            {"ePcMtBtAtk03CmbBtmA", 334},
            {"ePcMtBtAtk03CmbBtmB", 335},
            {"ePcMtBtAtk03CmbBtmC", 336},
            {"ePcMtBtAtk03CmbBtmD", 337},
            {"ePcMtBtAtk03CmbBtmFnsh", 338},
            {"ePcMtBtAtk03CmbL2RFnsh", 339},
            {"ePcMtBtAtk03Rng", 340},
            {"ePcMtBtAtk03Dwn", 341},
            {"ePcMtBtAtk03Tb", 342},
            {"ePcMtBattou03", 343},
            {"ePcMtBattou03Lp", 344},
            {"ePcMtBattou03Ed", 345},
            {"ePcMtLSBattou03", 346},
            {"ePcMtLSNoutou03", 347},
        }
    },
    {"Tsubaki Mk-II",
        {
            {"ePcMtBt04IdlU", 349},
            {"ePcMtBt04IdlM", 350},
            {"ePcMtBt04IdlD", 351},
            {"ePcMtBt04WlkUF", 352},
            {"ePcMtBt04WlkMF", 353},
            {"ePcMtBt04WlkBF", 354},
            {"ePcMtBt04WlkUB", 355},
            {"ePcMtBt04WlkMB", 356},
            {"ePcMtBt04WlkBB", 357},
            {"ePcMtBt04WlkUL", 358},
            {"ePcMtBt04WlkML", 359},
            {"ePcMtBt04WlkBL", 360},
            {"ePcMtBt04WlkUR", 361},
            {"ePcMtBt04WlkMR", 362},
            {"ePcMtBt04WlkBR", 363},
            {"ePcMtBt04Rn", 364},
            {"ePcMtBt04RnEd", 365},
            {"ePcMtBtAtk04CmbUpA", 366},
            {"ePcMtBtAtk04CmbUpB", 367},
            {"ePcMtBtAtk04CmbUpC", 368},
            {"ePcMtBtAtk04CmbUpD", 369},
            {"ePcMtBtAtk04CmbUpFnsh", 370},
            {"ePcMtBtAtk04CmbMidA", 371},
            {"ePcMtBtAtk04CmbMidB", 372},
            {"ePcMtBtAtk04CmbMidC", 373},
            {"ePcMtBtAtk04CmbMidD", 374},
            {"ePcMtBtAtk04CmbMidE", 375},
            {"ePcMtBtAtk04CmbMidF", 376},
            {"ePcMtBtAtk04CmbMidG", 377},
            {"ePcMtBtAtk04CmbMidFnsh", 378},
            {"ePcMtBtAtk04CmbBtmA", 379},
            {"ePcMtBtAtk04CmbBtmB", 380},
            {"ePcMtBtAtk04CmbBtmC", 381},
            {"ePcMtBtAtk04CmbBtmD", 382},
            {"ePcMtBtAtk04CmbBtmE", 383},
            {"ePcMtBtAtk04CmbBtmFnsh", 384},
            {"ePcMtBtAtk04CmbR2LFnsh", 385},
            {"ePcMtBtAtk04Rng", 386},
            {"ePcMtBtAtk04Dwn", 387},
            {"ePcMtBtAtk04Tb", 388},
            {"ePcMtBattou04", 389},
            {"ePcMtBattou04Lp", 390},
            {"ePcMtBattou04Ed", 391},
            {"ePcMtLSBattou041", 392},
            {"ePcMtLSBattou042", 393},
            {"ePcMtLSNoutou04", 394},
        }
    },
    {"Tsubaki Mk-III",
        {
            {"ePcMtBt02IdlU", 254},
            {"ePcMtBt02IdlM", 255},
            {"ePcMtBt02IdlD", 256},
            {"ePcMtBt02WlkUF", 257},
            {"ePcMtBt02WlkMF", 258},
            {"ePcMtBt02WlkBF", 259},
            {"ePcMtBt02WlkUB", 260},
            {"ePcMtBt02WlkMB", 261},
            {"ePcMtBt02WlkBB", 262},
            {"ePcMtBt02WlkUL", 263},
            {"ePcMtBt02WlkML", 264},
            {"ePcMtBt02WlkBL", 265},
            {"ePcMtBt02WlkUR", 266},
            {"ePcMtBt02WlkMR", 267},
            {"ePcMtBt02WlkBR", 268},
            {"ePcMtBt02Rn", 269},
            {"ePcMtBt02RnEd", 270},
            {"ePcMtBtAtk02CmbUpA", 271},
            {"ePcMtBtAtk02CmbUpB", 272},
            {"ePcMtBtAtk02CmbUpC", 273},
            {"ePcMtBtAtk02CmbUpD", 274},
            {"ePcMtBtAtk02CmbUpE", 275},
            {"ePcMtBtAtk02CmbUpF", 276},
            {"ePcMtBtAtk02CmbUpG", 277},
            {"ePcMtBtAtk02CmbUpFnsh", 278},
            {"ePcMtBtAtk02CmbBtmA", 279},
            {"ePcMtBtAtk02CmbBtmB", 280},
            {"ePcMtBtAtk02CmbBtmC", 281},
            {"ePcMtBtAtk02CmbBtmD", 282},
            {"ePcMtBtAtk02CmbBtmE", 283},
            {"ePcMtBtAtk02CmbBtmF", 284},
            {"ePcMtBtAtk02CmbBtmG", 285},
            {"ePcMtBtAtk02CmbBtmH", 286},
            {"ePcMtBtAtk02CmbBtmFnsh", 287},
            {"ePcMtBtAtk02CmbL2RFnsh", 288},
            {"ePcMtBtAtk02CmbD2UFnsh", 289},
            {"ePcMtBtAtk02RngCmbA", 290},
            {"ePcMtBtAtk02RngCmbB", 291},
            {"ePcMtBtAtk02RngCmbC", 292},
            {"ePcMtBtAtk02Dwn", 293},
            {"ePcMtBtAtk02Tb", 294},
            {"ePcMtBattou02", 295},
            {"ePcMtBattou02Lp", 296},
            {"ePcMtBattou02Ed", 297},
            {"ePcMtLSBattou021", 298},
            {"ePcMtLSBattou022", 299},
            {"ePcMtLSNoutou02", 300},
        }
    }
};