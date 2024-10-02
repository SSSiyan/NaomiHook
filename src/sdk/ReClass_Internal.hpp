#pragma once
// Created with ReClass.NET 1.1 by KN4CK3R

enum enCharaInitProc
{
  eCharaInitProcPrepareForReadRsl = 0,
  eCharaInitProcReadRsl = 1,
  eCharaInitProcReadRslWait = 2,
  eCharaInitProcPrepareForRsl = 3,
  eCharaInitProcFinish = 4,
  eCharaInitProcMax = 5,
};

enum enEasyDemoProc
{
  eEasyDemoInit = 0,
  eEasyDemoSetPause = 1,
  eEasyDemoPause = 2,
  eEasyDemoPlay = 3,
  eEasyDemoTerm = 4,
  eEasyDemoMax = 5,
};

enum enShakeDir
{
  eShakeUp2Down = 0,
  eShakeDown2Up = 1,
  eShakeLeft2Right = 2,
  eShakeRight2Left = 3,
  eShakeThrust = 4,
  eShakePull = 5,
  eShakeNone = 6,
};

enum enThrowKind
{
  enThrowUU = 0,
  enThrowDD = 1,
  enThrowLL = 2,
  enThrowRR = 3,
  enThrowUD = 4,
  enThrowDU = 5,
  enThrowLR = 6,
  enThrowRL = 7,
  enThrowRU = 8,
  enThrowDR = 9,
  enThrowRoRo = 10,
  enThrowUL = 11,
  enThrowKindMax = 12,
};

enum enPcInputMode
{
  ePcInputIdle = 0,
  ePcInputCamera = 1,
  ePcInputMove = 2,
  ePcInputBattleMove = 3,
  ePcInputBattleIdle = 4,
  ePcInputBike = 5,
  ePcInputMenu = 6,
  ePcInputDamage = 7,
  ePcInputDigHole = 8,
  ePcInputMax = 9,
};

struct Vec
{
  /* 0x0000 */ float x;
  /* 0x0004 */ float y;
  /* 0x0008 */ float z;
}; /* size: 0x000c */

struct stRovicofInfo
{
  /* 0x0000 */ bool questOffer;
  /* 0x0001 */ bool questClear;
  /* 0x0002 */ bool getPos[49];
  /* 0x0033 */ char Padding_1096;
  /* 0x0034 */ struct Vec pos[49];
}; /* size: 0x0280 */

class ghmGcFile {};
class TGan {};

struct stEqItem
{
  /* 0x0000 */ enum enEqReadProc readProc;
  /* 0x0004 */ class ghmGcFile* file;
  /* 0x0008 */ void* pDat[2];
  /* 0x0010 */ class TGmf* pGmf;
  /* 0x0014 */ class TGmf* pGmfTmp;
  /* 0x0018 */ int id;
  /* 0x001c */ bool reverseDisp;
  /* 0x001d */ char Padding_1095[3];
  /* 0x0020 */ int nowBufIndex;
}; /* size: 0x0024 */

struct stWepInfo
{
  /* 0x0000 */ int id;
  /* 0x0004 */ short battery;
  /* 0x0006 */ short batteryMax;
  /* 0x0008 */ float power;
  /* 0x000c */ bool cmbExtend;
  /* 0x000d */ char __PADDING__[3];
}; /* size: 0x0010 */

struct stAtkMot{
  /* 0x0000 */ class ghmGcFile* pFile;
  /* 0x0004 */ enum enCharaInitProc motReadProc;
  /* 0x0008 */ void* pData;
  /* 0x000c */ class TGan** pGan;
  /* 0x0010 */ short motionNumMax;
  /* 0x0012 */ char __PADDING__[2];
}; /* size: 0x0014 */

struct stEq
{
  /* 0x0000 */ int id;
}; /* size: 0x0004 */

struct stItem
{
  /* 0x0000 */ int id;
  /* 0x0004 */ char num;
  /* 0x0005 */ bool use;
  /* 0x0006 */ char __PADDING__[2];
}; /* size: 0x0008 */

class WAnimF
{
  /* 0x0000 */ long Padding_20;
  /* 0x0004 */ float mCurValue;
  /* 0x0008 */ float mDstValue;
  /* 0x000c */ float mSrcValue;
  /* 0x0010 */ unsigned int mCount;
  /* 0x0014 */ unsigned int mTime;
  /* 0x0018 */ int mIpType;
  /* 0x001c */ float mMotionRate;
}; /* size: 0x0020 */

struct stPcStatus
{
  /* 0x0000 */ struct stAtkMot atkMot;
  /* 0x0014 */ struct stEqItem equip[7];
  /* 0x0110 */ struct stWepInfo wepInfo[16];
  /* 0x0210 */ struct stEq locker[200];
  /* 0x0530 */ struct stItem item[300];
  /* 0x0e90 */ int itemMax;
  /* 0x0e94 */ float strength;
  /* 0x0e98 */ float stammina;
  /* 0x0e9c */ float vitality;
  /* 0x0ea0 */ int batteryTick;
  /* 0x0ea4 */ int attack360Tick;
  /* 0x0ea8 */ float heroesMeter;
  /* 0x0eac */ int heroesMeterTick;
  /* 0x0eb0 */ int darkSideUseNum;
  /* 0x0eb4 */ int darkSideKey;
  /* 0x0eb8 */ int darkSideKeyTick;
  /* 0x0ebc */ int darkSideNum;
  /* 0x0ec0 */ int darkSideMotion;
  /* 0x0ec4 */ struct Vec darkSideTrvPos;
  /* 0x0ed0 */ bool darkSideEndWait;
  /* 0x0ed1 */ bool darkSideAutoStart;
  /* 0x0ed2 */ bool darkSideKeyGuideDisp;
  /* 0x0ed3 */ bool darkSideSuccessInput;
  /* 0x0ed4 */ bool rapidFinishEnd;
  /* 0x0ed5 */ char Padding_1097[3];
  /* 0x0ed8 */ int lightSideChargeTick;
  /* 0x0edc */ int rapidFinishTick;
  /* 0x0ee0 */ int fastActionTick;
  /* 0x0ee4 */ int shinkuTick;
  /* 0x0ee8 */ int bulletTick;
  /* 0x0eec */ int money;
  /* 0x0ef0 */ int moneyMax;
  /* 0x0ef4 */ int tsubaTick;
  /* 0x0ef8 */ int tsubaTickTotal;
  /* 0x0efc */ float tsubaRate;
  /* 0x0f00 */ unsigned char tsubaNum;
  /* 0x0f01 */ unsigned char tsubaRotNum;
  /* 0x0f02 */ char Padding_1098[2];
  /* 0x0f04 */ class WAnimF tsubaWait;
  /* 0x0f24 */ class WAnimF tsubaOsare;
  /* 0x0f44 */ bool tsubaOsareFlag;
  /* 0x0f45 */ unsigned char chargeShake;
  /* 0x0f46 */ char killCmb;
  /* 0x0f47 */ unsigned char cmb;
  /* 0x0f48 */ enum enPcCmbKind cmbKind;
  /* 0x0f4c */ enum enPcCmbKind cmbKindOld;
  /* 0x0f50 */ enum enPcPose pose;
  /* 0x0f54 */ int catchTick;
  /* 0x0f58 */ float beforeRotY;
  /* 0x0f5c */ float beforeCamRotY;
  /* 0x0f60 */ enum enEasyDemoProc easyDemoProc;
  /* 0x0f64 */ enum enShakeDir needShakeDirLast;
  /* 0x0f68 */ enum enShakeDir needShakeDir;
  /* 0x0f6c */ enum enShakeDir shakeDir;
  /* 0x0f70 */ enum enThrowKind throwKind;
  /* 0x0f74 */ bool throw2nd;
  /* 0x0f75 */ bool areaChecksAButton;
  /* 0x0f76 */ char finishNpcIndex;
  /* 0x0f77 */ char Padding_1099;
  /* 0x0f78 */ float oldNpcAtkRate;
  /* 0x0f7c */ int finishMotNo;
  /* 0x0f80 */ int fghMotNo;
  /* 0x0f84 */ int gachaNum;
  /* 0x0f88 */ int gachaTick;
  /* 0x0f8c */ struct Vec beforePos4Camera;
  /* 0x0f98 */ class WAnimF poseLiner;
  /* 0x0fb8 */ struct Vec bikePos;
  /* 0x0fc4 */ float bikeRot;
  /* 0x0fc8 */ struct stRovicofInfo rvcf;
  /* 0x1248 */ bool digTool;
  /* 0x1249 */ bool rapidFinish;
  /* 0x124a */ bool alreadyHitRapidFinish;
  /* 0x124b */ bool cmbFlag;
  /* 0x124c */ int stepKey;
  /* 0x1250 */ bool stepFlag;
  /* 0x1251 */ bool fghFlag;
  /* 0x1252 */ bool rngFlag;
  /* 0x1253 */ bool battouDemo;
  /* 0x1254 */ int battouDemoProc;
  /* 0x1258 */ bool battouDemoPauseMode;
  /* 0x1259 */ bool noutouDemo;
  /* 0x125a */ bool eqWep;
  /* 0x125b */ bool eqWepLaser;
  /* 0x125c */ bool lInput;
  /* 0x125d */ bool rInput;
  /* 0x125e */ bool lockOnOder;
  /* 0x125f */ bool throwModeOnly;
  /* 0x1260 */ bool slashModeOnly;
  /* 0x1261 */ bool catchModeDisEnable;
  /* 0x1262 */ bool fightTameDisEnable;
  /* 0x1263 */ bool slashTameDisEnable;
  /* 0x1264 */ bool tsubaDisEnable;
  /* 0x1265 */ bool downAttackDisEnable;
  /* 0x1266 */ bool digDisEnable;
  /* 0x1267 */ bool digDisEnable4SM;
  /* 0x1268 */ bool rangeGuard;
  /* 0x1269 */ bool deadMotionPlay;
  /* 0x126a */ bool telephoneWalk;
  /* 0x126b */ bool electroShockWalk;
  /* 0x126c */ bool hugWalk;
  /* 0x126d */ bool eventWalk;
  /* 0x126e */ bool camOperateOld;
  /* 0x126f */ bool alwaysEmptyBattery;
  /* 0x1270 */ bool cantChargeBattery;
  /* 0x1271 */ bool cantDispLaserEffect;
  /* 0x1272 */ bool lastComboCancel;
  /* 0x1273 */ bool hizageriHit;
  /* 0x1274 */ bool backHomingCamera;
  /* 0x1275 */ bool wepChangeUnVisible;
  /* 0x1276 */ bool finishBeforeAttackAlreadyHit;
  /* 0x1277 */ bool warpMove;
  /* 0x1278 */ bool dontSemitrans;
  /* 0x1279 */ bool dontSemitransControl;
  /* 0x127a */ bool cantCallBike;
  /* 0x127b */ bool callBike;
  /* 0x127c */ bool atkHitAbsolute;
  /* 0x127d */ bool lostBike;
  /* 0x127e */ bool bikeVisible;
  /* 0x127f */ bool skillCatch[16];
  /* 0x128f */ bool skillK7[7];
  /* 0x1296 */ unsigned char maxCmb;
  /* 0x1297 */ char Padding_1100;
  /* 0x1298 */ int swingCount;
  /* 0x129c */ float padRotY;
  /* 0x12a0 */ class WAnimF dashProjection;
  /* 0x12c0 */ int hajikareTick;
  /* 0x12c4 */ int guardTick;
  /* 0x12c8 */ float receiveDmg;
  /* 0x12cc */ bool puppetMode;
  /* 0x12cd */ bool useWeaponEffect;
  /* 0x12ce */ bool todomePrepareMode;
  /* 0x12cf */ bool throwPrepareMode;
  /* 0x12d0 */ int throwInputResult;
  /* 0x12d4 */ float moneyUpRate;
  /* 0x12d8 */ float jumpDownAttackDist;
  /* 0x12dc */ unsigned char bomStockNum;
  /* 0x12dd */ bool cameraVReverseControl;
  /* 0x12de */ bool cameraYReverseControl;
  /* 0x12df */ bool dispMap;
  /* 0x12e0 */ unsigned char bikeSight;
  /* 0x12e1 */ bool dontChangeBikeCamera;
  /* 0x12e2 */ unsigned char shadowDepth;
  /* 0x12e3 */ char Padding_1101;
  /* 0x12e4 */ int idlingTick;
  /* 0x12e8 */ int joyuuLightNo;
  /* 0x12ec */ int joyuuLightDisableNo[4];
  /* 0x12fc */ int joyuuLightDisableNoNum;
  /* 0x1300 */ unsigned int clearNum;
  /* 0x1304 */ unsigned char rouletteHitRate;
  /* 0x1305 */ bool finishBonus;
  /* 0x1306 */ bool justGuard;
  /* 0x1307 */ bool justAttack;
  /* 0x1308 */ int justInputTick;
  /* 0x130c */ int justAtkInputStartTick;
  /* 0x1310 */ int justAtkInputEndTick;
  /* 0x1314 */ bool successInputFinish;
  /* 0x1315 */ bool finishResetSlow;
  /* 0x1316 */ bool finishSlow;
  /* 0x1317 */ bool finishSePlay;
  /* 0x1318 */ int finishSlowTick;
  /* 0x131c */ bool get777;
  /* 0x131d */ bool playShakeInput;
  /* 0x131e */ char Padding_1102[2];
  /* 0x1320 */ int playShakeInputWait;
  /* 0x1324 */ class WAnimF semiTransRate;
  /* 0x1344 */ bool banSlotCry;
  /* 0x1345 */ bool banSlotHop;
  /* 0x1346 */ bool banSlotBel;
  /* 0x1347 */ bool banSlotBar;
  /* 0x1348 */ bool banSlot777;
  /* 0x1349 */ bool wepStick;
  /* 0x134a */ char Padding_1103[2];
  /* 0x134c */ struct Vec padYOffset;
  /* 0x1358 */ float rotYOffset;
  /* 0x135c */ bool tsubaSe0;
  /* 0x135d */ bool tsubaSe1;
  /* 0x135e */ bool tsubaSe2;
  /* 0x135f */ bool deadSubmission;
  /* 0x1360 */ bool callMotionProcess;
  /* 0x1361 */ char Padding_1104[3];
  /* 0x1364 */ unsigned int deadBossNum;
  /* 0x1368 */ int telRambleTick;
  /* 0x136c */ int telRambleWaitTick;
  /* 0x1370 */ bool telRamble;
  /* 0x1371 */ bool cherry;
  /* 0x1372 */ bool hitNormalDashAttack;
  /* 0x1373 */ bool justAvoid;
  /* 0x1374 */ bool dontSubBattey;
  /* 0x1375 */ bool autoSubBattey;
  /* 0x1376 */ bool initCamera;
  /* 0x1377 */ bool dataLoad;
  /* 0x1378 */ bool dontRestoreMotion;
  /* 0x1379 */ bool bikeClash2battou;
  /* 0x137a */ bool winTsubazeri;
  /* 0x137b */ bool justGuardDisEnable;
  /* 0x137c */ bool dashAtkDisEnable;
  /* 0x137d */ bool justEscapeDisEnable;
  /* 0x137e */ char Padding_1105[2];
  /* 0x1380 */ int ikasamaSlot;
  /* 0x1384 */ int fireManTick;
}; /* size: 0x1388 */

struct GXTexObj
{
  /* 0x0000 */ class GXTexture* pTex;
}; /* size: 0x0004 */

struct tagGHMR_TEX
{
  /* 0x0000 */ bool TMEFlag;
  /* 0x0001 */ bool AlphaFlag;
  /* 0x0002 */ bool ImageBufferFreeFlag;
  /* 0x0003 */ char Padding_39;
  /* 0x0004 */ void* FileImage;
  /* 0x0008 */ void* ImageBuffer;
  /* 0x000c */ unsigned short Width;
  /* 0x000e */ unsigned short Height;
  /* 0x0010 */ struct GXTexObj TexObj;
  /* 0x0014 */ enum GHMR_TEXADDRESS AddressMode;
  /* 0x0018 */ enum GHMR_TEXFILTER FilterMode;
  /* 0x001c */ enum _GXTexFmt GetScreenImageFormat;
  /* 0x0020 */ int ImageSize;
  /* 0x0024 */ enum GHMR_TEXDOT_ASPECT Aspect;
}; /* size: 0x0028 */

struct stPcSaveData
{
  /* 0x0000 */ float maxHp;
  /* 0x0004 */ float hp;
  /* 0x0008 */ struct stWepInfo wepInfo[16];
  /* 0x0108 */ struct stEqItem equip[7];
  /* 0x0204 */ struct stEq locker[200];
  /* 0x0524 */ struct stItem item[300];
  /* 0x0e84 */ float strength;
  /* 0x0e88 */ float stammina;
  /* 0x0e8c */ float vitality;
  /* 0x0e90 */ int money;
  /* 0x0e94 */ int darkSideUseNum;
  /* 0x0e98 */ char killCmb;
  /* 0x0e99 */ char Padding_1106[3];
  /* 0x0e9c */ struct Vec bikePos;
  /* 0x0ea8 */ float bikeRot;
  /* 0x0eac */ struct stRovicofInfo rvcf;
  /* 0x112c */ bool digTool;
  /* 0x112d */ bool skillCatch[16];
  /* 0x113d */ bool skillK7[7];
  /* 0x1144 */ unsigned char maxCmb;
  /* 0x1145 */ char Padding_1107[3];
  /* 0x1148 */ float receiveDmg;
  /* 0x114c */ int totalKillNum;
  /* 0x1150 */ unsigned char bomStockNum;
  /* 0x1151 */ bool lostBike;
  /* 0x1152 */ bool dispMap;
  /* 0x1153 */ unsigned char bikeSight;
  /* 0x1154 */ unsigned int clearNum;
}; /* size: 0x1158 */

struct stCamData
{
  /* 0x0000 */ float vdir;
  /* 0x0004 */ float ydir;
  /* 0x0008 */ enum enCameraLengthType lenType;
}; /* size: 0x000c */

struct stPcSndData
{
  /* 0x0000 */ int seHdlLightsavorReady;
  /* 0x0004 */ float seVolLightsavorReady;
  /* 0x0008 */ bool seDontPlaySeAgainLSReady;
  /* 0x0009 */ char Padding_1108[3];
  /* 0x000c */ int seHdlSlow;
  /* 0x0010 */ float oldBgmVol;
  /* 0x0014 */ int oldBgmHdl;
  /* 0x0018 */ int nowBgmHdl;
  /* 0x001c */ enum enSeReadProc seReadProc;
  /* 0x0020 */ class WAnimF pitchBgm;
  /* 0x0040 */ unsigned int oldFinishBgmVol;
  /* 0x0044 */ unsigned int oldFinishSeVol;
  /* 0x0048 */ int seHdlCharge;
  /* 0x004c */ class WAnimF pitchCharge;
  /* 0x006c */ int seHdlChargeMax;
  /* 0x0070 */ class WAnimF pitchChargeMax;
  /* 0x0090 */ int seHdlAlarm;
  /* 0x0094 */ int seHdlTel;
  /* 0x0098 */ int seHdlDsBreath;
  /* 0x009c */ int seHdlTsubazeri;
}; /* size: 0x00a0 */

struct tagHRTASKCHECK
{
  /* 0x0000 */ class HrTask* Task;
  /* 0x0004 */ unsigned int MagicNumber;
}; /* size: 0x0008 */

struct stPcEffect
{
  /* 0x0000 */ bool laserTrackFade;
  /* 0x0001 */ char Padding_1109[3];
  /* 0x0004 */ float laserDepth;
  /* 0x0008 */ int laserWaitTick;
  /* 0x000c */ class EffectCloseContest* pCloseContest;
  /* 0x0010 */ class EffectSpeedBlur* pSpeedBlur;
  /* 0x0014 */ int speedBlurTick;
  /* 0x0018 */ float speedBlurAlpha;
  /* 0x001c */ float speedBlurScale;
  /* 0x0020 */ int speedBlurRepeat;
  /* 0x0024 */ class WAnimF speedBlurRepeatNum;
  /* 0x0044 */ struct Vec posHitSlash;
  /* 0x0050 */ struct Vec posHitSlashOld;
  /* 0x005c */ class EffectThrowSever* pThrowSabel;
  /* 0x0060 */ int lightReflecteTick;
  /* 0x0064 */ class EffectConcentLine* pConLine;
  /* 0x0068 */ class WAnimF darkValue;
  /* 0x0088 */ class EffectVacuumWave* pShinkuWave[3];
  /* 0x0094 */ struct tagHRTASKCHECK taskCheckShinkuWave[3];
  /* 0x00ac */ class EffectModelFire* pModelFire;
  /* 0x00b0 */ class WAnimF modelFireValue;
  /* 0x00d0 */ int bombTick;
  /* 0x00d4 */ class EffectDevilExp* pBomb;
  /* 0x00d8 */ struct tagHRTASKCHECK taskCheckBomb;
  /* 0x00e0 */ class EffectAccumulate* pCharge;
  /* 0x00e4 */ class EffectSwordLaser* pLaserLR[2];
  /* 0x00ec */ class EffectSwordLaser* pLaser[5];
  /* 0x0100 */ class EffectDryIce* pDryIce;
  /* 0x0104 */ class EffectMetalElect* pMetalElect;
  /* 0x0108 */ class EffectSlashTrack* pWarpLocus;
  /* 0x010c */ struct tagGHMR_TEX texShinkuCursor;
  /* 0x0134 */ class WAnimF hitAttackProjection;
  /* 0x0154 */ class EffectMonokuro* pMono;
  /* 0x0158 */ class EffectSlotNoise* pNoise;
  /* 0x015c */ class WAnimF fastActionBlur;
  /* 0x017c */ class EffectLockOn2* pLockOn2;
}; /* size: 0x0180 */

struct stHugWalk
{
  /* 0x0000 */ class TGmf* pGmf;
  /* 0x0004 */ class TGan* pGan[6];
  /* 0x001c */ int motNo;
}; /* size: 0x0020 */

class HROBJDummy
{
    char pad_00[0x3f8]; // edited
}; /* size: 0x03f8 */

class ghmTriangle
{
  /* 0x0000 */ struct Vec mPoint[3];
}; /* size: 0x0024 */

class HrOverLap {}; /* size: 0x0038 */

struct stFade
{
  /* 0x0000 */ enum enScreenChangeKind kind;
  /* 0x0004 */ bool requestStart;
  /* 0x0005 */ bool requestEnd;
  /* 0x0006 */ char Padding_1110[2];
  /* 0x0008 */ class HrOverLap* pOverLap;
}; /* size: 0x000c */

union uniSMflag
{
  union
  {
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short smBaceActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short smBaseMotionmode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short smBacepad : 8; /* bit position: 8 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm02ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm02MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm02NearGomi : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm02AKeyGuide : 1; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm02pad : 6; /* bit position: 10 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm05ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm05MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm05HaveCocoNuts : 3; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm05InCocoNutsArea : 1; /* bit position: 11 */
      /* 0x0000 */ unsigned short sm05DamegeReq : 1; /* bit position: 12 */
      /* 0x0000 */ unsigned short sm05PutReq : 1; /* bit position: 13 */
      /* 0x0000 */ unsigned short sm05AKeyGuide : 1; /* bit position: 14 */
      /* 0x0000 */ unsigned short sm05pad : 1; /* bit position: 15 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm07ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm07MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm07RL : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm07pad : 7; /* bit position: 9 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm09ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm09MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm09NearWall : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm09AKeyGuide : 1; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm09pad : 6; /* bit position: 10 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm10ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm10MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm10MissileFlag : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm10pad : 5; /* bit position: 9 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm11ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm11MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm11DamegeReq : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm11Poisoned : 2; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm11NearSasori : 1; /* bit position: 11 */
      /* 0x0000 */ unsigned short sm11AKeyGuide : 1; /* bit position: 12 */
      /* 0x0000 */ unsigned short sm11pad : 3; /* bit position: 13 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm12ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm12MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm12SikanMode : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm12pad : 7; /* bit position: 9 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm13ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm13MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm13CarStop : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm13Petrol : 2; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm13MeterState : 3; /* bit position: 11 */
      /* 0x0000 */ unsigned short sm13DamegeReq : 1; /* bit position: 14 */
      /* 0x0000 */ unsigned short sm13WepAttach : 1; /* bit position: 15 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm14ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm14MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm14DamegeReq : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm14WepAttach : 1; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm14AKeyGuide : 1; /* bit position: 10 */
      /* 0x0000 */ unsigned short sm14pad : 5; /* bit position: 11 */
    }; /* bitfield */
    struct /* bitfield */
    {
      /* 0x0000 */ unsigned short sm24ActivSubMission : 2; /* bit position: 0 */
      /* 0x0000 */ unsigned short sm24MotionMode : 6; /* bit position: 2 */
      /* 0x0000 */ unsigned short sm24Touch : 1; /* bit position: 8 */
      /* 0x0000 */ unsigned short sm24WepAttach : 1; /* bit position: 9 */
      /* 0x0000 */ unsigned short sm24Catch : 2; /* bit position: 10 */
      /* 0x0000 */ unsigned short sm24Cancel : 1; /* bit position: 12 */
      /* 0x0000 */ unsigned short sm24pad : 3; /* bit position: 13 */
    }; /* bitfield */
    /* 0x0000 */ unsigned short msubmissionflag;
  }; /* size: 0x0002 */
}; /* size: 0x0002 */

struct Quaternion
{
  /* 0x0000 */ float x;
  /* 0x0004 */ float y;
  /* 0x0008 */ float z;
  /* 0x000c */ float w;
}; /* size: 0x0010 */

class ghmListObj
{
  /* 0x0000 */ long Padding_9;
  /* 0x0004 */ class ghmListObj* mpPrev;
  /* 0x0008 */ class ghmListObj* mpNext;
  /* 0x000c */ int mPriority;
}; /* size: 0x0010 */

class ghmPlane
{
  /* 0x0000 */ struct Vec mNormal;
  /* 0x000c */ float mDist;
}; /* size: 0x0010 */

class ghmGcCollObjHitResultObj
{
  /* 0x0000 */ class ghmPlane mPlane;
  /* 0x0010 */ class ghmTriangle mTriangle;
  /* 0x0034 */ struct Vec mPoint;
  /* 0x0040 */ float mRate;
  /* 0x0044 */ unsigned int mAttr;
  /* 0x0048 */ class ghmGcCollObj* mpObj;
}; /* size: 0x004c */

struct stVec
{
  /* 0x0000 */ class WAnimF x;
  /* 0x0020 */ class WAnimF y;
  /* 0x0040 */ class WAnimF z;
}; /* size: 0x0060 */

struct stDamageInfo
{
  /* 0x0000 */ float dmg;
  /* 0x0004 */ int dmgMot;
  /* 0x0008 */ int grdMot;
  /* 0x000c */ int atkMot;
  /* 0x0010 */ float dmgDirec;
  /* 0x0014 */ float nockBackDst;
  /* 0x0018 */ float upperPow;
  /* 0x001c */ float upperPosY;
  /* 0x0020 */ float grav;
  /* 0x0024 */ bool upper;
  /* 0x0025 */ char Padding_22[3];
  /* 0x0028 */ int tick;
  /* 0x002c */ class WAnimF fade;
  /* 0x004c */ struct Vec m_AirBlowPos;
  /* 0x0058 */ float m_AirBlowPower;
  /* 0x005c */ bool m_AirFlag;
  /* 0x005d */ char Padding_23[3];
  /* 0x0060 */ float m_GravAccele;
  /* 0x0064 */ unsigned char m_PiyoRequest;
  /* 0x0065 */ char Padding_24[3];
  /* 0x0068 */ float storeDamage;
  /* 0x006c */ float storeDamageDst;
  /* 0x0070 */ int restoreDamegeTick;
  /* 0x0074 */ int restoreDamegeBasicTick;
  /* 0x0078 */ unsigned char m_BikeDeadRequest;
  /* 0x0079 */ char __PADDING__[3];
}; /* size: 0x007c */

class ghmRectangle
{
  /* 0x0000 */ struct Vec mCenter;
  /* 0x000c */ struct Vec mAxis[2];
  /* 0x0024 */ float mExtent[2];
}; /* size: 0x002c */

class ghmLozenge
{
  /* 0x0000 */ class ghmRectangle mRectangle;
  /* 0x002c */ float mRadius;
}; /* size: 0x0030 */

class ghmSegment
{
  /* 0x0000 */ struct Vec mCenter;
  /* 0x000c */ struct Vec mDir;
  /* 0x0018 */ float mExtent;
}; /* size: 0x001c */

class ghmCapsule
{
  /* 0x0000 */ class ghmSegment mAxis;
  /* 0x001c */ float mRadius;
}; /* size: 0x0020 */

class ghmGcCollObjCapsule
{
  /* 0x0000 */ class ghmLozenge mMoveShape;
  /* 0x0030 */ long Padding_152[4];
  /* 0x0040 */ class ghmCapsule mShape;
}; /* size: 0x0060 */

struct stCharaStatus
{
  /* 0x0000 */ int resNo;
  /* 0x0004 */ int datNo;
  /* 0x0008 */ enum enCharaType charaType;
  /* 0x000c */ short zakoWepType;
  /* 0x000e */ char Padding_225[2];
  /* 0x0010 */ float maxHp;
  /* 0x0014 */ float hp;
  /* 0x0018 */ short money;
  /* 0x001a */ short dropMoney;
  /* 0x001c */ float tension;
  /* 0x0020 */ float hajikiCount;
  /* 0x0024 */ bool alwaysCheckHitColl;
  /* 0x0025 */ char Padding_226[3];
  /* 0x0028 */ struct Vec pos;
  /* 0x0034 */ struct Vec beforePos;
  /* 0x0040 */ struct Vec navelPos;
  /* 0x004c */ struct Vec brainPos;
  /* 0x0058 */ struct Vec rot;
  /* 0x0064 */ struct Vec movRot;
  /* 0x0070 */ struct Quaternion qRot;
  /* 0x0080 */ float movSpd;
  /* 0x0084 */ float hitSize;
  /* 0x0088 */ float height;
  /* 0x008c */ float upperPosY;
  /* 0x0090 */ struct stVec aPos;
  /* 0x00f0 */ struct stVec aRot;
  /* 0x0150 */ int enterPattern;
  /* 0x0154 */ float reactionDist;
  /* 0x0158 */ int itemNo;
  /* 0x015c */ int mAiDefBasic;
  /* 0x0160 */ int mAiDefCurrent;
  /* 0x0164 */ int mAiDefTick;
  /* 0x0168 */ int mAiDefBaseTick;
  /* 0x016c */ float mAiDamageCount;
  /* 0x0170 */ char* pScriptProc;
  /* 0x0174 */ int tsubazeriNum;
  /* 0x0178 */ int DamageAcceptFrame;
  /* 0x017c */ int motionNo;
  /* 0x0180 */ char motionBrendNum;
  /* 0x0181 */ char Padding_227[3];
  /* 0x0184 */ float motSpd;
  /* 0x0188 */ enum enCharaCondition condition;
  /* 0x018c */ class ghmGcCollObj* footHitResultObj;
  /* 0x0190 */ class ghmGcCollObjHitResult* footHitResult;
  /* 0x0194 */ class ghmGcCollObjHitResultObj footHitResultObj2;
  /* 0x01e0 */ struct WGclMaterialSpec* hitWallMaterial;
  /* 0x01e4 */ struct stDamageInfo dmgInfo;
  /* 0x0260 */ struct Vec mYukaNormal;
  /* 0x026c */ int movTick;
  /* 0x0270 */ short hitNum;
  /* 0x0272 */ char Padding_228[2];
  /* 0x0274 */ class ghmGcCollObjCapsule hitColl;
  /* 0x02d4 */ int mAiAtkType;
  /* 0x02d8 */ int mAiAtkClass;
  /* 0x02dc */ short AtkAi[10];
  /* 0x02f0 */ int renderSkipCounter;
  /* 0x02f4 */ float renderSkipMotSpd;
  /* 0x02f8 */ int frameStop;
  union
  {
    struct
    {
      struct /* bitfield */
      {
        /* 0x02fc */ unsigned int visible : 1; /* bit position: 0 */
        /* 0x02fc */ unsigned int visibleWep : 1; /* bit position: 1 */
        /* 0x02fc */ unsigned int visibleWepEffect : 1; /* bit position: 2 */
        /* 0x02fc */ unsigned int visibleDismember : 1; /* bit position: 3 */
        /* 0x02fc */ unsigned int visibleDist : 1; /* bit position: 4 */
        /* 0x02fc */ unsigned int lockOn : 1; /* bit position: 5 */
        /* 0x02fc */ unsigned int hitChara : 1; /* bit position: 6 */
        /* 0x02fc */ unsigned int hitAttack : 1; /* bit position: 7 */
        /* 0x02fc */ unsigned int hitStage : 1; /* bit position: 8 */
        /* 0x02fc */ unsigned int downShockWave : 1; /* bit position: 9 */
        /* 0x02fc */ unsigned int hitStageDisEnable : 1; /* bit position: 10 */
        /* 0x02fc */ unsigned int hitStageDisEnableReq : 1; /* bit position: 11 */
        /* 0x02fc */ unsigned int hitOidashiDisEnable : 1; /* bit position: 12 */
        /* 0x02fc */ unsigned int operateDisEnable : 1; /* bit position: 13 */
        /* 0x02fc */ unsigned int drawPriority : 1; /* bit position: 14 */
        /* 0x02fc */ unsigned int charaPause : 1; /* bit position: 15 */
        /* 0x02fc */ unsigned int initPlayMotion : 1; /* bit position: 16 */
        /* 0x02fc */ unsigned int slowBlow : 1; /* bit position: 17 */
        /* 0x02fc */ unsigned int dontCountKill : 1; /* bit position: 18 */
        /* 0x02fc */ unsigned int dontSubRepop : 1; /* bit position: 19 */
        /* 0x02fc */ unsigned int dontStandUp : 1; /* bit position: 20 */
        /* 0x02fc */ unsigned int lowDamage : 1; /* bit position: 21 */
        /* 0x02fc */ unsigned int dontHitStageWall : 1; /* bit position: 22 */
        /* 0x02fc */ unsigned int ikari : 1; /* bit position: 23 */
        /* 0x02fc */ unsigned int motSpeedControl : 1; /* bit position: 24 */
        /* 0x02fc */ unsigned int bossDeadFlag : 1; /* bit position: 25 */
        /* 0x02fc */ unsigned int successThrow : 1; /* bit position: 26 */
        /* 0x02fc */ unsigned int loseTsubazeri : 1; /* bit position: 27 */
        /* 0x02fc */ unsigned int chargeDamage : 1; /* bit position: 28 */
        /* 0x02fc */ unsigned int motionProcessDisEnable : 1; /* bit position: 29 */
        /* 0x02fc */ unsigned int downWaitStart : 1; /* bit position: 30 */
        /* 0x02fc */ unsigned int miniMapRender : 1; /* bit position: 31 */
      }; /* bitfield */
      struct /* bitfield */
      {
        /* 0x0300 */ unsigned int normalClip : 1; /* bit position: 0 */
        /* 0x0300 */ unsigned int jpnDead : 1; /* bit position: 1 */
      }; /* bitfield */
    }; /* size: 0x0008 */
    /* 0x02fc */ unsigned int flag[2];
  }; /* size: 0x0008 */
}; /* size: 0x0304 */


class stCharaFileData
{
  /* 0x0000 */ class ghmGcFile* file;
  /* 0x0004 */ void* pData;
  /* 0x0008 */ class ghmResGroup* pRsl;
  /* 0x000c */ class TGmf* pGmf;
  /* 0x0010 */ class TGmf* pGmfDead;
  /* 0x0014 */ class TGmf* pGmfWepR;
  /* 0x0018 */ class TGmf* pGmfWepL;
  /* 0x001c */ class TGmf* pGmfShadow;
  /* 0x0020 */ class TGmf* pGmfWepShadow;
  /* 0x0024 */ class TGmf* pGmfEasyShadow;
  /* 0x0028 */ class TGmf* pGmfDeadEasyShadow;
  /* 0x002c */ short motionNumMax;
  /* 0x002e */ bool readyDelete;
  /* 0x002f */ char Padding_229;
  /* 0x0030 */ int sndFileHandle;
  /* 0x0034 */ struct HRCHARAVOICE* pVoice;
  /* 0x0038 */ class TGan** pGan;
}; /* size: 0x003c */

struct stBloodEffect
{
  /* 0x0000 */ struct tagHRTASKCHECK chkDat;
  /* 0x0008 */ class TGmfNode* pNode;
  /* 0x000c */ int tick;
}; /* size: 0x0010 */

struct stCharaEffect
{
  /* 0x0000 */ class EffectSwordLaser* pLaser;
  /* 0x0004 */ class EffectSlashTrack* pSlash;
  /* 0x0008 */ class EffectNoGuard* pNoGuard;
  /* 0x000c */ class EffectPowerFlash* pPowerFlash;
  /* 0x0010 */ class HrBattleIcon* pBattleIcon;
  /* 0x0014 */ class EffectBomDead* pBomDead;
  /* 0x0018 */ class EffectDizzyStar* pDizzyStar;
  /* 0x001c */ struct stBloodEffect bldEf[6];
  /* 0x007c */ struct tagHRTASKCHECK bomDeadEf;
  /* 0x0084 */ struct tagHRTASKCHECK blackSmkChkDat;
  /* 0x008c */ float bloodRate;
  /* 0x0090 */ class EffectEnemyDamage* pDamage;
  /* 0x0094 */ struct tagHRTASKCHECK damageChkDat;
}; /* size: 0x009c */

class ghmGcCollObjHitResult
{
    enum Condition { // edited
        NO_HIT = 0,
        OVERLAPPED = 1,
        HIT = 2
    };
  union
  {
    struct
    {
      /* 0x0000 */ enum ghmGcCollObjHitResult::Condition mCondition;
      /* 0x0004 */ class ghmPlane mPlane;
      /* 0x0014 */ struct Vec mPoint;
      /* 0x0020 */ float mRate;
      /* 0x0024 */ float mMargin;
      /* 0x0028 */ class ghmGcCollObj* mpObj;
      /* 0x002c */ int mHitObjCount;
    }; /* size: 0x0030 */
    /* 0x0000 */ class ghmGcCollObjHitResultObj mObj[512];
  }; /* size: 0x9800 */
}; /* size: 0x0030 */

class mHRChara
{
public:
  // union
  // {
    /* 0x0000 */ // class ghmListObj mCharaAnchor;
    /* 0x0000 */ // int mOldCameraMode;
    /* 0x0000 */ // class TGan** mpGan;
    /* 0x0000 */ // short mZakoStdMotionNumMax;
    /* 0x0000 */ // class TGan** mpZakoWepGan[18];
    struct
    {
      /* 0x0000 */ short mZakoWepMotionNumMax[8]; // edited, mStatus was at +24
      /* 0x0010 */ struct stCharaStatus mStatus;
      /* 0x0314 */ class stCharaFileData mResource;
      /* 0x0350 */ struct stCharaEffect mEffect;
      /* 0x03ec */ enum enCharaInitProc mInitProc;
      /* 0x03f0 */ float mDistFromPc;
      /* 0x03f4 */ float mDirecYFromPc;
    }; /* size: 0x040c */
    /* 0x0000 */ // class ghmGcCollObjHitResult mHitResult; // edited out because static_assert(sizeof(mHRChara) == 38912); // bruh
  // }; /* size: 0x040c */
}; /* size: 0x03f8 */

class mHRPc
{
public: // edited
  //union
  //{
    /* 0x0000 */ // struct tagGHMR_TEX DarkFaceTex;
    /* 0x0000 */ // struct tagGHMR_TEX DarkHandTex;
    /* 0x0000 */ // float DarkRate;
    /* 0x0000 */ // bool CameraYReverseControlFlag;
    /* 0x0000 */ // bool CameraXReverseControlFlag;
  //}; /* size: 0x0028 */
  char pad_00[0x10]; // edited
               struct stCharaStatus mCharaStatus; // edited
               char pad_314[0xe4]; // edited
  /* 0x03f8 */ int mEscapeActionInit;
  /* 0x03fc */ int mEscapeOnButton;
  /* 0x0400 */ struct stPcStatus mPcStatus;
  /* 0x1788 */ struct stPcSaveData mPcSaveData;
  /* 0x28e0 */ class mHRBike* mpBike;
  /* 0x28e4 */ struct stCamData mCamData;
  /* 0x28f0 */ struct stPcSndData mSnd;
  /* 0x2990 */ enum enPcInputMode mInputMode;
  /* 0x2994 */ enum enPcInputMode mInputModeOld;
  /* 0x2998 */ enum enPcInputMode mInputModeBefore;
  /* 0x299c */ bool mPauseAll;
  /* 0x299d */ bool mPauseNpc;
  /* 0x299e */ bool mOperate;
  /* 0x299f */ bool mOnlyMove;
  /* 0x29a0 */ bool mMotSpdAdj;
  /* 0x29a1 */ bool mDead;
  /* 0x29a2 */ bool mDeadPause;
  /* 0x29a3 */ bool mCameraOperate;
  /* 0x29a4 */ bool mBattouDemoRequest;
  /* 0x29a5 */ bool mStageChangeInitEnd;
  /* 0x29a6 */ bool mStageChangeTermEnd;
  /* 0x29a7 */ bool mStageChangeMuteki;
  /* 0x29a8 */ bool mBanStatusScreen;
  /* 0x29a9 */ char Padding_1112[3];
  /* 0x29ac */ int mAtkPauseTime;
  /* 0x29b0 */ int mStopRenderFrameNum;
  /* 0x29b4 */ struct stPcEffect mPcEffect;
  /* 0x2b34 */ struct stHugWalk mPcHug;
  /* 0x2b54 */ struct Vec mLockOnPos;
  /* 0x2b60 */ class mHRChara* mpLockOnNpc;
  /* 0x2b64 */ class mHRChara* mpTsubaNpc;
  /* 0x2b68 */ class mHRChara* mpCatchNpc;
  /* 0x2b6c */ class mHRChara* mpCatchReserveNpc;
  /* 0x2b70 */ class mHRChara* mpEscapeNpc;
  /* 0x2b74 */ class mHRChara* mpInitFinishNpc;
  /* 0x2b78 */ class mHRChara* mpNextFinishNpc;
  /* 0x2b7c */ class mHRChara* mpFinishNpc[8];
  /* 0x2b9c */ class mHRChara* mpFinishReserveNpc;
  /* 0x2ba0 */ class mHRChara* mpDSTargetNpc;
  /* 0x2ba4 */ class mHRChara* mpDwnAtkNpc;
  /* 0x2ba8 */ class mHRChara* mpLastAtkNpc;
  /* 0x2bac */ class HROBJDummy mLockOnDummy;
  /* 0x2fa4 */ bool mSavehitOidashiDisEnable;
  /* 0x2fa5 */ char Padding_1113[3];
  /* 0x2fa8 */ int mSlowMotionSytemMotNo;
  /* 0x2fac */ int mSlowMotionSytemButton;
  /* 0x2fb0 */ class ghmTriangle mWepColl;
  /* 0x2fd4 */ struct stFade mFade;
  /* 0x2fe0 */ union uniSMflag mSubMissionflag;
  /* 0x2fe2 */ bool mSubMissionPcPosStoreFlag;
  /* 0x2fe3 */ char Padding_1114;
  /* 0x2fe4 */ struct Vec mSubMissionStorePos;
  /* 0x2ff0 */ struct Vec mSubMissionStoreRot;
  /* 0x2ffc */ bool mSubMissionBikePosStoreFlag;
  /* 0x2ffd */ char Padding_1115[3];
  /* 0x3000 */ struct Vec mSubMissionStoreBikePos;
  /* 0x300c */ struct Vec mSubMissionStoreBikeRot;
  /* 0x3018 */ class EventAreaCamera* mpSubMissionJumpCameraHandle;
  /* 0x301c */ unsigned int mRuleBit;
  /* 0x3020 */ int mDisEnableBtlPoseCalc;
  /* 0x3024 */ unsigned char mTamePush;
  /* 0x3025 */ unsigned char mTameMax;
  /* 0x3026 */ bool mTameSe;
  /* 0x3027 */ char Padding_1116;
  /* 0x3028 */ int mDigAction;
  /* 0x302c */ bool mIkasamaTsuba;
               char pad_302d[3];
}; /* size: 0x3030 */

struct stVecA
{
  /* 0x0000 */ class WAnimF x;
  /* 0x0020 */ class WAnimF y;
  /* 0x0040 */ class WAnimF z;
}; /* size: 0x0060 */

struct stCamStatus
{
  /* 0x0000 */ float projection;
  /* 0x0004 */ float aspectRate;
  /* 0x0008 */ float angle;
  /* 0x000c */ float angleX;
  /* 0x0010 */ bool lockNavelPos;
  /* 0x0011 */ bool motionCamera;
  /* 0x0012 */ bool motionCameraNoUse;
  /* 0x0013 */ bool motionCameraAutoReturnDisEnable;
  /* 0x0014 */ struct stVecA pcPosOffset;
  /* 0x0074 */ struct stVecA enPosOffset;
  /* 0x00d4 */ struct stVecA fpsPosOffset;
  /* 0x0134 */ bool inputMove;
  /* 0x0135 */ char Padding_1091[3];
  /* 0x0138 */ struct Vec* pMotPos;
  /* 0x013c */ float* pMotY;
  /* 0x0140 */ bool dontAppearStatus;
  /* 0x0141 */ char __PADDING__[3];
}; /* size: 0x0144 */

struct stBtEffect
{
  /* 0x0000 */ class HrScreenStatus* pScreenStatus;
  /* 0x0004 */ class HrEffectShutter* pShutter;
  /* 0x0008 */ int slowMotTotalTick;
  /* 0x000c */ int slowMotTick;
  /* 0x0010 */ int bulletSlowTick;
  /* 0x0014 */ int bossBreakSlowTick;
}; /* size: 0x0018 */

struct stNpcDat
{
  /* 0x0000 */ short index;
  /* 0x0002 */ char Padding_1092[2];
  /* 0x0004 */ int resNo;
  /* 0x0008 */ int indexMpNpc;
  /* 0x000c */ enum enCharaType charaType;
  /* 0x0010 */ int repop;
  /* 0x0014 */ int repopTimer;
  /* 0x0018 */ struct Vec pos;
  /* 0x0024 */ struct Vec rot;
  /* 0x0030 */ enum enPopReqType popReqType;
  /* 0x0034 */ bool popOrder;
  /* 0x0035 */ char Padding_1093[3];
  /* 0x0038 */ int enterPattern;
  /* 0x003c */ float popDist;
  /* 0x0040 */ float reactionDist;
  /* 0x0044 */ int itemNo;
  /* 0x0048 */ int money;
  /* 0x004c */ char callScriptProcName[32];
  /* 0x006c */ bool disEnableCollision;
  /* 0x006d */ char Padding_1094[3];
  /* 0x0070 */ float initHp;
  /* 0x0074 */ int defAi;
  /* 0x0078 */ int atkAi;
  /* 0x007c */ int atkAiClass;
}; /* size: 0x0080 */

class mHRBattle
{
public: // edited
  /* 0x0000 */ enum enBattleInitProc mBtlInitProc;
  /* 0x0004 */ enum enBattleFrameProc mBtlFrameProc;
  /* 0x0008 */ struct stCamStatus mCamStatus;
  /* 0x014c */ struct stBtEffect mBtEffect;
  /* 0x0164 */ class mHRPc* mpPc;
  /* 0x0168 */ class mHRChara* mpNpc[30];
  /* 0x01e0 */ struct stNpcDat mNpcDat[1000];
  /* 0x1f5e0 */ int mTotalNpcNum;
  /* 0x1f5e4 */ int mTotalKillNum;
  /* 0x1f5e8 */ int mKillNum;
  /* 0x1f5ec */ float mNpcAttackRate;
  union
  {
    struct
    {
      struct /* bitfield */
      {
        /* 0x1f5f0 */ unsigned int battlePause : 1; /* bit position: 0 */
        /* 0x1f5f0 */ unsigned int tutoRun : 1; /* bit position: 1 */
        /* 0x1f5f0 */ unsigned int dispStatusDisEnable : 1; /* bit position: 2 */
        /* 0x1f5f0 */ unsigned int chargeDamage : 1; /* bit position: 3 */
      }; /* bitfield */
      /* 0x1f5f4 */ unsigned int padding : 29; /* bit position: 0 */
    }; /* size: 0x0008 */
    /* 0x1f5f0 */ unsigned int mFlag;
  }; /* size: 0x0008 */
  /* 0x1f5f8 */ class WAnimF mBikeRotY;
}; /* size: 0x1f618 */
