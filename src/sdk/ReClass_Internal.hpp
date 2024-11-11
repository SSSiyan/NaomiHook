#pragma once
// Created with ReClass.NET 1.1 by KN4CK3R

enum dpadDirection : int8_t
{
  DPAD_LEFT = 1,
  DPAD_RIGHT = 2,
  DPAD_DOWN = 4,
  DPAD_UP = 8,
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

enum enEqReadProc
{
  eEqFileNew = 0,
  eEqFileOpen = 1,
  eEqRead = 2,
  eEqReadWait = 3,
  eEqWait1Frame = 4,
  eEqReadMax = 5,
};

enum enCharaCondition
{
  eGood = 0,
  eDamaging = 1,
  eDying = 2,
  eDead = 3,
  eBossDead = 4,
  eConditionInit = 5,
  eConditionMax = 6,
};

enum enCharaType
{
  eCharaTypeNone = 0,
  eCharaTypePC = 1,
  eCharaTypeSLV = 2,
  eCharaTypeNPC = 3,
  eCharaTypeNPCNotMove = 4,
  eCharaTypeOBJ = 5,
  eCharaTypeOBJNotMove = 6,
  eCharaTypeZKPipe = 7,
  eCharaTypeTGR = 8,
  eCharaTypeTYG = 9,
  eCharaTypeJST = 10,
  eCharaTypeSFF = 11,
  eCharaTypeEFL = 12,
  eCharaTypeOBJ700 = 13,
  eCharaTypeMGE = 14,
  eCharaTypeENX = 15,
  eCharaTypeMAM = 16,
  eCharaTypeZKClone = 17,
  eCharaTypeTET = 18,
  eCharaTypeGEN = 19,
  eCharaTypeTKL = 20,
  eCharaTypeZKKatana = 21,
  eCharaTypeZKAxe = 22,
  eCharaTypeZKBeamKatanaA = 23,
  eCharaTypeZKBeamKatanaB = 24,
  eCharaTypeZKHandGun = 25,
  eCharaTypeZKSubMachineGun = 26,
  eCharaTypeZKShotGun = 27,
  eCharaTypeZKBall = 28,
  eCharaTypeOBJ013_0 = 29,
  eCharaTypeOBJ013_1 = 30,
  eCharaTypeOBJ013_2 = 31,
  eCharaTypeOBJ630 = 32,
  eCharaTypeOBJ631 = 33,
  eCharaTypeOBJ632 = 34,
  eCharaTypeMAM_Jr = 35,
  eCharaTypeHML = 36,
  eCharaTypeNZN = 37,
  eCharaTypeZakoBike = 38,
  eCharaTypeOBJ161 = 39,
  eCharaTypeOBJ162 = 40,
  eCharaTypeSCP = 41,
  eCharaTypeOBJ164 = 42,
  eCharaTypeZKNone = 43,
  eCharaTypeDAG = 44,
  eCharaTypeOBJ169 = 45,
  eCharaTypeZKHammer = 46,
  eCharaTypeZKKakuzai = 47,
  eCharaTypeZKBat = 48,
  eCharaTypeZKSchop = 49,
  eCharaTypeZKSignboard = 50,
  eCharaTypeZKSword = 51,
  eCharaTypeZKKaeinbin = 52,
  eCharaTypeOBJ180 = 53,
  eCharaTypeOBJ181 = 54,
  eCharaTypeOBJ182 = 55,
  eCharaTypeETG = 56,
  eCharaTypeSCT = 57,
  eCharaTypeOBJ955 = 58,
  eCharaTypeSLV2 = 59,
  eCharaTypeWEL = 60,
  eCharaTypeSLV3 = 61,
  eCharaTypeOBJ095 = 62,
  eCharaTypeOBJ431 = 63,
  eCharaTypeOBJ629 = 64,
  eCharaTypeOBJ183 = 65,
  eCharaTypeSubMissionNpc = 500,
  eCharaTypeOBJ021 = 501,
  eCharaTypeSMJ = 502,
  eCharaTypeOBJ008 = 503,
  eCharaTypeOBJ009 = 504,
  eCharaTypeOBJ010 = 505,
  eCharaTypeOBJ011 = 506,
  eCharaTypeOBJ012 = 507,
  eCharaTypeOBJ106 = 508,
  eCharaTypeOBJ107 = 509,
  eCharaTypeOBJ108 = 510,
  eCharaTypeOBJ019 = 511,
  eCharaTypeOBJ435 = 512,
  eCharaTypeOBJ065_1 = 513,
  eCharaTypeOBJ065_2 = 514,
  eCharaTypeOBJ065_3 = 515,
  eCharaTypeSML = 516,
  eCharaTypeOBJ156_1 = 517,
  eCharaTypeOBJ156_2 = 518,
  eCharaTypeOBJ155 = 519,
  eCharaTypeOBJ159 = 520,
  eCharaTypeOBJ158 = 521,
  eCharaTypeSMQ = 522,
  eCharaTypeOBJ_GAS = 523,
  eCharaTypeSM_CAT = 524,
};

enum enPopReqType {
  ePopTypeDistance = 0,
  ePopTypeAlways = 1,
  ePopTypeOrder = 2,
  ePopTypeMax = 3,
};

enum pcItem {
    // Swords
    BLOOD_BERRY = 0,
    TSUBAKI_MK3 = 1,
    TSUBAKI_MK1 = 2,
    TSUBAKI_MK2 = 3,
    SWORD4 = 4,
    SWORD5 = 5,
    SWORD6 = 6,
    SWORD7 = 7,
    SWORD8 = 8,
    SWORD9 = 9,
    SWORD10 = 10,
    SWORD11 = 11,
    SWORD12 = 12,
    SWORD13 = 13,
    SWORD14 = 14,
    SWORD15 = 15,
    SWORD16 = 16,
    SWORD17 = 17,
    SWORD18 = 18,
    SWORD19 = 19,
    SWORD20 = 20,
    SWORD21 = 21,
    SWORD22 = 22,
    SWORD23 = 23,
    SWORD24 = 24,
    SWORD25 = 25,
    SWORD26 = 26,
    SWORD27 = 27,
    SWORD28 = 28,
    SWORD29 = 29,
    SWORD30 = 30,
    SWORD31 = 31,
    SWORD32 = 32,
    SWORD33 = 33,
    SWORD34 = 34,
    SWORD35 = 35,
    SWORD36 = 36,
    SWORD37 = 37,
    SWORD38 = 38,
    SWORD39 = 39,
    SWORD40 = 40,
    SWORD41 = 41,
    SWORD42 = 42,
    SWORD43 = 43,
    SWORD44 = 44,
    SWORD45 = 45,
    SWORD46 = 46,
    SWORD47 = 47,
    SWORD48 = 48,
    SWORD49 = 49,
    SWORD50 = 50,
    SWORD51 = 51,
    SWORD52 = 52,
    SWORD53 = 53,
    SWORD54 = 54,
    SWORD55 = 55,
    SWORD56 = 56,
    SWORD57 = 57,
    SWORD58 = 58,
    SWORD59 = 59,
    SWORD60 = 60,
    SWORD61 = 61,
    SWORD62 = 62,
    SWORD63 = 63,
    SWORD64 = 64,
    SWORD65 = 65,
    SWORD66 = 66,
    SWORD67 = 67,
    SWORD68 = 68,
    SWORD69 = 69,
    SWORD70 = 70,
    SWORD71 = 71,
    SWORD72 = 72,
    SWORD73 = 73,
    SWORD74 = 74,
    SWORD75 = 75,
    SWORD76 = 76,
    SWORD77 = 77,
    SWORD78 = 78,
    SWORD79 = 79,
    SWORD80 = 80,
    SWORD81 = 81,
    SWORD82 = 82,
    SWORD83 = 83,
    SWORD84 = 84,
    SWORD85 = 85,
    SWORD86 = 86,
    SWORD87 = 87,
    SWORD88 = 88,
    SWORD89 = 89,
    SWORD90 = 90,
    SWORD91 = 91,
    SWORD92 = 92,
    SWORD93 = 93,
    SWORD94 = 94,
    SWORD95 = 95,
    SWORD96 = 96,
    SWORD97 = 97,
    SWORD98 = 98,
    SWORD99 = 99,

    // Glasses
    GLASSES0 = 100,
    GLASSES1 = 101,
    GLASSES2 = 102,
    GLASSES3 = 103,
    GLASSES4 = 104,
    GLASSES5 = 105,
    GLASSES6 = 106,
    GLASSES7 = 107,
    GLASSES8 = 108,
    GLASSES9 = 109,
    GLASSES10 = 110,
    GLASSES11 = 111,
    GLASSES12 = 112,
    GLASSES13 = 113,
    GLASSES14 = 114,
    GLASSES15 = 115,
    GLASSES16 = 116,
    GLASSES17 = 117,
    GLASSES18 = 118,
    GLASSES19 = 119,
    GLASSES20 = 120,
    GLASSES21 = 121,
    GLASSES22 = 122,
    GLASSES23 = 123,
    GLASSES24 = 124,
    GLASSES25 = 125,
    GLASSES26 = 126,
    GLASSES27 = 127,
    GLASSES28 = 128,
    GLASSES29 = 129,
    GLASSES30 = 130,
    GLASSES31 = 131,
    GLASSES32 = 132,
    GLASSES33 = 133,
    GLASSES34 = 134,
    GLASSES35 = 135,
    GLASSES36 = 136,
    GLASSES37 = 137,
    GLASSES38 = 138,
    GLASSES39 = 139,
    GLASSES40 = 140,
    GLASSES41 = 141,
    GLASSES42 = 142,
    GLASSES43 = 143,
    GLASSES44 = 144,
    GLASSES45 = 145,
    GLASSES46 = 146,
    GLASSES47 = 147,
    GLASSES48 = 148,
    GLASSES49 = 149,
    GLASSES50 = 150,
    GLASSES51 = 151,
    GLASSES52 = 152,
    GLASSES53 = 153,
    GLASSES54 = 154,
    GLASSES55 = 155,
    GLASSES56 = 156,
    GLASSES57 = 157,
    GLASSES58 = 158,
    GLASSES59 = 159,
    GLASSES60 = 160,
    GLASSES61 = 161,
    GLASSES62 = 162,
    GLASSES63 = 163,
    GLASSES64 = 164,
    GLASSES65 = 165,
    GLASSES66 = 166,
    GLASSES67 = 167,
    GLASSES68 = 168,
    GLASSES69 = 169,
    GLASSES70 = 170,
    GLASSES71 = 171,
    GLASSES72 = 172,
    GLASSES73 = 173,
    GLASSES74 = 174,
    GLASSES75 = 175,
    GLASSES76 = 176,
    GLASSES77 = 177,
    GLASSES78 = 178,
    GLASSES79 = 179,
    GLASSES80 = 180,
    GLASSES81 = 181,
    GLASSES82 = 182,
    GLASSES83 = 183,
    GLASSES84 = 184,
    GLASSES85 = 185,
    GLASSES86 = 186,
    GLASSES87 = 187,
    GLASSES88 = 188,
    GLASSES89 = 189,
    GLASSES90 = 190,
    GLASSES91 = 191,
    GLASSES92 = 192,
    GLASSES93 = 193,
    GLASSES94 = 194,
    GLASSES95 = 195,
    GLASSES96 = 196,
    GLASSES97 = 197,
    GLASSES98 = 198,
    GLASSES99 = 199,

    // Jackets
    JACKET0 = 200,
    JACKET1 = 201,
    JACKET2 = 202,
    JACKET3 = 203,
    JACKET4 = 204,
    JACKET5 = 205,
    JACKET6 = 206,
    JACKET7 = 207,
    JACKET8 = 208,
    JACKET9 = 209,
    JACKET10 = 210,
    JACKET11 = 211,
    JACKET12 = 212,
    JACKET13 = 213,
    JACKET14 = 214,
    JACKET15 = 215,
    JACKET16 = 216,
    JACKET17 = 217,
    JACKET18 = 218,
    JACKET19 = 219,
    JACKET20 = 220,
    JACKET21 = 221,
    JACKET22 = 222,
    JACKET23 = 223,
    JACKET24 = 224,
    JACKET25 = 225,
    JACKET26 = 226,
    JACKET27 = 227,
    JACKET28 = 228,
    JACKET29 = 229,
    JACKET30 = 230,
    JACKET31 = 231,
    JACKET32 = 232,
    JACKET33 = 233,
    JACKET34 = 234,
    JACKET35 = 235,
    JACKET36 = 236,
    JACKET37 = 237,
    JACKET38 = 238,
    JACKET39 = 239,
    JACKET40 = 240,
    JACKET41 = 241,
    JACKET42 = 242,
    JACKET43 = 243,
    JACKET44 = 244,
    JACKET45 = 245,
    JACKET46 = 246,
    JACKET47 = 247,
    JACKET48 = 248,
    JACKET49 = 249,
    JACKET50 = 250,
    JACKET51 = 251,
    JACKET52 = 252,
    JACKET53 = 253,
    JACKET54 = 254,
    JACKET55 = 255,
    JACKET56 = 256,
    JACKET57 = 257,
    JACKET58 = 258,
    JACKET59 = 259,
    JACKET60 = 260,
    JACKET61 = 261,
    JACKET62 = 262,
    JACKET63 = 263,
    JACKET64 = 264,
    JACKET65 = 265,
    JACKET66 = 266,
    JACKET67 = 267,
    JACKET68 = 268,
    JACKET69 = 269,
    JACKET70 = 270,
    JACKET71 = 271,
    JACKET72 = 272,
    JACKET73 = 273,
    JACKET74 = 274,
    JACKET75 = 275,
    JACKET76 = 276,
    JACKET77 = 277,
    JACKET78 = 278,
    JACKET79 = 279,
    JACKET80 = 280,
    JACKET81 = 281,
    JACKET82 = 282,
    JACKET83 = 283,
    JACKET84 = 284,
    JACKET85 = 285,
    JACKET86 = 286,
    JACKET87 = 287,
    JACKET88 = 288,
    JACKET89 = 289,
    JACKET90 = 290,
    JACKET91 = 291,
    JACKET92 = 292,
    JACKET93 = 293,
    JACKET94 = 294,
    JACKET95 = 295,
    JACKET96 = 296,
    JACKET97 = 297,
    JACKET98 = 298,
    JACKET99 = 299,

    // Shoes
    SHOES0 = 300,
    SHOES1 = 301,
    SHOES2 = 302,
    SHOES3 = 303,
    SHOES4 = 304,
    SHOES5 = 305,
    SHOES6 = 306,
    SHOES7 = 307,
    SHOES8 = 308,
    SHOES9 = 309,
    SHOES10 = 310,
    SHOES11 = 311,
    SHOES12 = 312,
    SHOES13 = 313,
    SHOES14 = 314,
    SHOES15 = 315,
    SHOES16 = 316,
    SHOES17 = 317,
    SHOES18 = 318,
    SHOES19 = 319,
    SHOES20 = 320,
    SHOES21 = 321,
    SHOES22 = 322,
    SHOES23 = 323,
    SHOES24 = 324,
    SHOES25 = 325,
    SHOES26 = 326,
    SHOES27 = 327,
    SHOES28 = 328,
    SHOES29 = 329,
    SHOES30 = 330,
    SHOES31 = 331,
    SHOES32 = 332,
    SHOES33 = 333,
    SHOES34 = 334,
    SHOES35 = 335,
    SHOES36 = 336,
    SHOES37 = 337,
    SHOES38 = 338,
    SHOES39 = 339,
    SHOES40 = 340,
    SHOES41 = 341,
    SHOES42 = 342,
    SHOES43 = 343,
    SHOES44 = 344,
    SHOES45 = 345,
    SHOES46 = 346,
    SHOES47 = 347,
    SHOES48 = 348,
    SHOES49 = 349,
    SHOES50 = 350,
    SHOES51 = 351,
    SHOES52 = 352,
    SHOES53 = 353,
    SHOES54 = 354,
    SHOES55 = 355,
    SHOES56 = 356,
    SHOES57 = 357,
    SHOES58 = 358,
    SHOES59 = 359,
    SHOES60 = 360,
    SHOES61 = 361,
    SHOES62 = 362,
    SHOES63 = 363,
    SHOES64 = 364,
    SHOES65 = 365,
    SHOES66 = 366,
    SHOES67 = 367,
    SHOES68 = 368,
    SHOES69 = 369,
    SHOES70 = 370,
    SHOES71 = 371,
    SHOES72 = 372,
    SHOES73 = 373,
    SHOES74 = 374,
    SHOES75 = 375,
    SHOES76 = 376,
    SHOES77 = 377,
    SHOES78 = 378,
    SHOES79 = 379,
    SHOES80 = 380,
    SHOES81 = 381,
    SHOES82 = 382,
    SHOES83 = 383,
    SHOES84 = 384,
    SHOES85 = 385,
    SHOES86 = 386,
    SHOES87 = 387,
    SHOES88 = 388,
    SHOES89 = 389,
    SHOES90 = 390,
    SHOES91 = 391,
    SHOES92 = 392,
    SHOES93 = 393,
    SHOES94 = 394,
    SHOES95 = 395,
    SHOES96 = 396,
    SHOES97 = 397,
    SHOES98 = 398,
    SHOES99 = 399,

    // Jeans
    JEANS0 = 400,
    JEANS1 = 401,
    JEANS2 = 402,
    JEANS3 = 403,
    JEANS4 = 404,
    JEANS5 = 405,
    JEANS6 = 406,
    JEANS7 = 407,
    JEANS8 = 408,
    JEANS9 = 409,
    JEANS10 = 410,
    JEANS11 = 411,
    JEANS12 = 412,
    JEANS13 = 413,
    JEANS14 = 414,
    JEANS15 = 415,
    JEANS16 = 416,
    JEANS17 = 417,
    JEANS18 = 418,
    JEANS19 = 419,
    JEANS20 = 420,
    JEANS21 = 421,
    JEANS22 = 422,
    JEANS23 = 423,
    JEANS24 = 424,
    JEANS25 = 425,
    JEANS26 = 426,
    JEANS27 = 427,
    JEANS28 = 428,
    JEANS29 = 429,
    JEANS30 = 430,
    JEANS31 = 431,
    JEANS32 = 432,
    JEANS33 = 433,
    JEANS34 = 434,
    JEANS35 = 435,
    JEANS36 = 436,
    JEANS37 = 437,
    JEANS38 = 438,
    JEANS39 = 439,
    JEANS40 = 440,
    JEANS41 = 441,
    JEANS42 = 442,
    JEANS43 = 443,
    JEANS44 = 444,
    JEANS45 = 445,
    JEANS46 = 446,
    JEANS47 = 447,
    JEANS48 = 448,
    JEANS49 = 449,
    JEANS50 = 450,
    JEANS51 = 451,
    JEANS52 = 452,
    JEANS53 = 453,
    JEANS54 = 454,
    JEANS55 = 455,
    JEANS56 = 456,
    JEANS57 = 457,
    JEANS58 = 458,
    JEANS59 = 459,
    JEANS60 = 460,
    JEANS61 = 461,
    JEANS62 = 462,
    JEANS63 = 463,
    JEANS64 = 464,
    JEANS65 = 465,
    JEANS66 = 466,
    JEANS67 = 467,
    JEANS68 = 468,
    JEANS69 = 469,
    JEANS70 = 470,
    JEANS71 = 471,
    JEANS72 = 472,
    JEANS73 = 473,
    JEANS74 = 474,
    JEANS75 = 475,
    JEANS76 = 476,
    JEANS77 = 477,
    JEANS78 = 478,
    JEANS79 = 479,
    JEANS80 = 480,
    JEANS81 = 481,
    JEANS82 = 482,
    JEANS83 = 483,
    JEANS84 = 484,
    JEANS85 = 485,
    JEANS86 = 486,
    JEANS87 = 487,
    JEANS88 = 488,
    JEANS89 = 489,
    JEANS90 = 490,
    JEANS91 = 491,
    JEANS92 = 492,
    JEANS93 = 493,
    JEANS94 = 494,
    JEANS95 = 495,
    JEANS96 = 496,
    JEANS97 = 497,
    JEANS98 = 498,
    JEANS99 = 499,

    // Belts
    BELT0 = 500,
    BELT1 = 501,
    BELT2 = 502,
    BELT3 = 503,
    BELT4 = 504,
    BELT5 = 505,
    BELT6 = 506,
    BELT7 = 507,
    BELT8 = 508,
    BELT9 = 509,
    BELT10 = 510,
    BELT11 = 511,
    BELT12 = 512,
    BELT13 = 513,
    BELT14 = 514,
    BELT15 = 515,
    BELT16 = 516,
    BELT17 = 517,
    BELT18 = 518,
    BELT19 = 519,
    BELT20 = 520,
    BELT21 = 521,
    BELT22 = 522,
    BELT23 = 523,
    BELT24 = 524,
    BELT25 = 525,
    BELT26 = 526,
    BELT27 = 527,
    BELT28 = 528,
    BELT29 = 529,
    BELT30 = 530,
    BELT31 = 531,
    BELT32 = 532,
    BELT33 = 533,
    BELT34 = 534,
    BELT35 = 535,
    BELT36 = 536,
    BELT37 = 537,
    BELT38 = 538,
    BELT39 = 539,
    BELT40 = 540,
    BELT41 = 541,
    BELT42 = 542,
    BELT43 = 543,
    BELT44 = 544,
    BELT45 = 545,
    BELT46 = 546,
    BELT47 = 547,
    BELT48 = 548,
    BELT49 = 549,
    BELT50 = 550,
    BELT51 = 551,
    BELT52 = 552,
    BELT53 = 553,
    BELT54 = 554,
    BELT55 = 555,
    BELT56 = 556,
    BELT57 = 557,
    BELT58 = 558,
    BELT59 = 559,
    BELT60 = 560,
    BELT61 = 561,
    BELT62 = 562,
    BELT63 = 563,
    BELT64 = 564,
    BELT65 = 565,
    BELT66 = 566,
    BELT67 = 567,
    BELT68 = 568,
    BELT69 = 569,
    BELT70 = 570,
    BELT71 = 571,
    BELT72 = 572,
    BELT73 = 573,
    BELT74 = 574,
    BELT75 = 575,
    BELT76 = 576,
    BELT77 = 577,
    BELT78 = 578,
    BELT79 = 579,
    BELT80 = 580,
    BELT81 = 581,
    BELT82 = 582,
    BELT83 = 583,
    BELT84 = 584,
    BELT85 = 585,
    BELT86 = 586,
    BELT87 = 587,
    BELT88 = 588,
    BELT89 = 589,
    BELT90 = 590,
    BELT91 = 591,
    BELT92 = 592,
    BELT93 = 593,
    BELT94 = 594,
    BELT95 = 595,
    BELT96 = 596,
    BELT97 = 597,
    BELT98 = 598,
    BELT99 = 599,

    // Shirts
    SHIRT0 = 600,
    SHIRT1 = 601,
    SHIRT2 = 602,
    SHIRT3 = 603,
    SHIRT4 = 604,
    SHIRT5 = 605,
    SHIRT6 = 606,
    SHIRT7 = 607,
    SHIRT8 = 608,
    SHIRT9 = 609,
    SHIRT10 = 610,
    SHIRT11 = 611,
    SHIRT12 = 612,
    SHIRT13 = 613,
    SHIRT14 = 614,
    SHIRT15 = 615,
    SHIRT16 = 616,
    SHIRT17 = 617,
    SHIRT18 = 618,
    SHIRT19 = 619,
    SHIRT20 = 620,
    SHIRT21 = 621,
    SHIRT22 = 622,
    SHIRT23 = 623,
    SHIRT24 = 624,
    SHIRT25 = 625,
    SHIRT26 = 626,
    SHIRT27 = 627,
    SHIRT28 = 628,
    SHIRT29 = 629,
    SHIRT30 = 630,
    SHIRT31 = 631,
    SHIRT32 = 632,
    SHIRT33 = 633,
    SHIRT34 = 634,
    SHIRT35 = 635,
    SHIRT36 = 636,
    SHIRT37 = 637,
    SHIRT38 = 638,
    SHIRT39 = 639,
    SHIRT40 = 640,
    SHIRT41 = 641,
    SHIRT42 = 642,
    SHIRT43 = 643,
    SHIRT44 = 644,
    SHIRT45 = 645,
    SHIRT46 = 646,
    SHIRT47 = 647,
    SHIRT48 = 648,
    SHIRT49 = 649,
    SHIRT50 = 650,
    SHIRT51 = 651,
    SHIRT52 = 652,
    SHIRT53 = 653,
    SHIRT54 = 654,
    SHIRT55 = 655,
    SHIRT56 = 656,
    SHIRT57 = 657,
    SHIRT58 = 658,
    SHIRT59 = 659,
    SHIRT60 = 660,
    SHIRT61 = 661,
    SHIRT62 = 662,
    SHIRT63 = 663,
    SHIRT64 = 664,
    SHIRT65 = 665,
    SHIRT66 = 666,
    SHIRT67 = 667,
    SHIRT68 = 668,
    SHIRT69 = 669,
    SHIRT70 = 670,
    SHIRT71 = 671,
    SHIRT72 = 672,
    SHIRT73 = 673,
    SHIRT74 = 674,
    SHIRT75 = 675,
    SHIRT76 = 676,
    SHIRT77 = 677,
    SHIRT78 = 678,
    SHIRT79 = 679,
    SHIRT80 = 680,
    SHIRT81 = 681,
    SHIRT82 = 682,
    SHIRT83 = 683,
    SHIRT84 = 684,
    SHIRT85 = 685,
    SHIRT86 = 686,
    SHIRT87 = 687,
    SHIRT88 = 688,
    SHIRT89 = 689,
    SHIRT90 = 690,
    SHIRT91 = 691,
    SHIRT92 = 692,
    SHIRT93 = 693,
    SHIRT94 = 694,
    SHIRT95 = 695,
    SHIRT96 = 696,
    SHIRT97 = 697,
    SHIRT98 = 698,
    SHIRT99 = 699,
    SHIRT100 = 700,
    SHIRT101 = 701,
    SHIRT102 = 702,
    SHIRT103 = 703,
    SHIRT104 = 704,
    SHIRT105 = 705,
    SHIRT106 = 706,
    SHIRT107 = 707,
    SHIRT108 = 708,
    SHIRT109 = 709,
    SHIRT110 = 710,
    SHIRT111 = 711,
    SHIRT112 = 712,
    SHIRT113 = 713,
    SHIRT114 = 714,
    SHIRT115 = 715,
    SHIRT116 = 716,
    SHIRT117 = 717,
    SHIRT118 = 718,
    SHIRT119 = 719,
    SHIRT120 = 720,
    SHIRT121 = 721,
    SHIRT122 = 722,
    SHIRT123 = 723,
    SHIRT124 = 724,
    SHIRT125 = 725,
    SHIRT126 = 726,
    SHIRT127 = 727,
    SHIRT128 = 728,
    SHIRT129 = 729,
    SHIRT130 = 730,
    SHIRT131 = 731,
    SHIRT132 = 732,
    SHIRT133 = 733,
    SHIRT134 = 734,
    SHIRT135 = 735,
    SHIRT136 = 736,
    SHIRT137 = 737,
    SHIRT138 = 738,
    SHIRT139 = 739,
    SHIRT140 = 740,
    SHIRT141 = 741,
    SHIRT142 = 742,
    SHIRT143 = 743,
    SHIRT144 = 744,
    SHIRT145 = 745,
    SHIRT146 = 746,
    SHIRT147 = 747,
    SHIRT148 = 748,
    SHIRT149 = 749,
    SHIRT150 = 750,
    SHIRT151 = 751,
    SHIRT152 = 752,
    SHIRT153 = 753,
    SHIRT154 = 754,
    SHIRT155 = 755,
    SHIRT156 = 756,
    SHIRT157 = 757,
    SHIRT158 = 758,
    SHIRT159 = 759,
    SHIRT160 = 760,
    SHIRT161 = 761,
    SHIRT162 = 762,
    SHIRT163 = 763,
    SHIRT164 = 764,
    SHIRT165 = 765,
    SHIRT166 = 766,
    SHIRT167 = 767,
    SHIRT168 = 768,
    SHIRT169 = 769,
    SHIRT170 = 770,
    SHIRT171 = 771,
    SHIRT172 = 772,
    SHIRT173 = 773,
    SHIRT174 = 774,
    SHIRT175 = 775,
    SHIRT176 = 776,
    SHIRT177 = 777,
    SHIRT178 = 778,
    SHIRT179 = 779,
    SHIRT180 = 780,
    SHIRT181 = 781,
    SHIRT182 = 782,
    SHIRT183 = 783,
    SHIRT184 = 784,
    SHIRT185 = 785,
    SHIRT186 = 786,
    SHIRT187 = 787,
    SHIRT188 = 788,
    SHIRT189 = 789,
    SHIRT190 = 790,
    SHIRT191 = 791,
    SHIRT192 = 792,
    SHIRT193 = 793,
    SHIRT194 = 794,
    SHIRT195 = 795,
    SHIRT196 = 796,
    SHIRT197 = 797,
    SHIRT198 = 798,
    SHIRT199 = 799,
};


enum pcMotion
{
  ePcMtIdlLp = 0,
  ePcMtIdl0 = 1,
  ePcMtIdl1 = 2,
  ePcMtBtIdl0 = 3,
  ePcMtBtIdl1 = 4,
  ePcMtBtDSFnsh = 5,
  ePcMtWlkLp = 6,
  ePcMtWlkEd = 7,
  ePcMtWlkEdL = 8,
  ePcMtWlkEdR = 9,
  ePcMtWlkTrn = 10,
  ePcMtRnLp = 11,
  ePcMtRnEd = 12,
  ePcMtRnTrn = 13,
  ePcMtDshLp = 14,
  ePcMtDshEd = 15,
  ePcMtTrdSt = 16,
  ePcMtTrdLp = 17,
  ePcMtTrdEd = 18,
  ePcMtPhnIdlLp = 19,
  ePcMtPhnWlkLp = 20,
  ePcMtElShckIdlLp = 21,
  ePcMtElShckRnLp = 22,
  ePcMtAtkEdWstR = 23,
  ePcMtBtDefRunR = 24,
  ePcMtBtDefRunL = 25,
  ePcMtStpF = 26, // Roll Forward
  ePcMtRollB = 27, // Roll Back
  ePcMtAvdR = 28, // Darkstep Right
  ePcMtAvdL = 29, // Darkstep Left
  ePcMtRollR = 30,
  ePcMtRollL = 31,
  ePcMtAvdFStrt = 32,
  ePcMtAvdFAr = 33,
  ePcMtAvdFEd = 34,
  ePcMtStairsUp = 35,
  ePcMtStairsDown = 36,
  ePcMtBtChgWlkF = 37,
  ePcMtBtChgWlkB = 38,
  ePcMtBtChgWlkL = 39,
  ePcMtBtChgWlkR = 40,
  ePcMtTbLpGd = 41,
  ePcMtTbLp = 42,
  ePcMtTbLpBd = 43,
  ePcMtBtryChrgSt = 44, // Start Charging
  ePcMtBtryChrgLp = 45, // Continue Charging
  ePcMtBtryChrgEnd = 46,
  ePcMtBtryChrgQkLp = 47,
  ePcMtGrdDfltLp = 48,
  ePcMtGrdDfltLpD = 49,
  ePcMtGrdDfltDmg = 50,
  ePcMtGrdDflt2RU = 51,
  ePcMtGrdDflt2R = 52,
  ePcMtGrdDflt2RD = 53,
  ePcMtGrdDflt2LU = 54,
  ePcMtGrdDflt2L = 55,
  ePcMtGrdDflt2LD = 56,
  ePcMtGrdRULp = 57,
  ePcMtGrdRUDmg = 58,
  ePcMtGrdRU2R = 59,
  ePcMtGrdRU2RD = 60,
  ePcMtGrdRU2LU = 61,
  ePcMtGrdRU2L = 62,
  ePcMtGrdRU2LD = 63,
  ePcMtGrdRLp = 64,
  ePcMtGrdRDmg = 65,
  ePcMtGrdR2RU = 66,
  ePcMtGrdR2RD = 67,
  ePcMtGrdR2LU = 68,
  ePcMtGrdR2L = 69,
  ePcMtGrdR2LD = 70,
  ePcMtGrdRDLp = 71,
  ePcMtGrdRDDmg = 72,
  ePcMtGrdRD2RU = 73,
  ePcMtGrdRD2R = 74,
  ePcMtGrdRD2LU = 75,
  ePcMtGrdRD2L = 76,
  ePcMtGrdRD2LD = 77,
  ePcMtGrdLULp = 78,
  ePcMtGrdLUDmg = 79,
  ePcMtGrdLU2RU = 80,
  ePcMtGrdLU2R = 81,
  ePcMtGrdLU2RD = 82,
  ePcMtGrdLU2L = 83,
  ePcMtGrdLU2LD = 84,
  ePcMtGrdLLp = 85,
  ePcMtGrdLDmg = 86,
  ePcMtGrdL2RU = 87,
  ePcMtGrdL2R = 88,
  ePcMtGrdL2RD = 89,
  ePcMtGrdL2LU = 90,
  ePcMtGrdL2LD = 91,
  ePcMtGrdLDLp = 92,
  ePcMtGrdLDDmg = 93,
  ePcMtGrdLD2RU = 94,
  ePcMtGrdLD2R = 95,
  ePcMtGrdLD2RD = 96,
  ePcMtGrdLD2LU = 97,
  ePcMtGrdLD2L = 98,
  ePcMtGrdRngR = 99,
  ePcMtGrdRngL = 100,
  ePcMtGrdRngBck = 101,
  ePcMtDmgSmlR = 102,
  ePcMtDmgMidR = 103,
  ePcMtDmgSmlL = 104,
  ePcMtDmgMidL = 105,
  ePcMtDmgSmlB = 106,
  ePcMtDmgMidB = 107,
  ePcMtDmgSmlF = 108,
  ePcMtDmgMidF = 109,
  ePcMtDmgNBR = 110,
  ePcMtDmgNBRLnd = 111,
  ePcMtDmgNBL = 112,
  ePcMtDmgNBLLnd = 113,
  ePcMtDmgNBB = 114,
  ePcMtDmgNBBLnd = 115,
  ePcMtDmgNBF = 116,
  ePcMtDmgNBFLnd = 117,
  ePcMtDwnB = 118,
  ePcMtDwnBBtn = 119,
  ePcMtDwnB2Stnd = 120,
  ePcMtDmgDwnB = 121,
  ePcMtDwnF = 122,
  ePcMtDwnFBtn = 123,
  ePcMtDwnF2Stnd = 124,
  ePcMtDmgDwnF = 125,
  ePcMtHjkr = 126,
  ePcMtTbOt1 = 127,
  ePcMtDdStd = 128,
  ePcMtDdAomk = 129,
  ePcMtDdUtsbs = 130,
  ePcMtLSFly = 131,
  ePcMtLSRtn = 132,
  ePcMtBtAtkFghtCmbA = 133,
  ePcMtBtAtkFghtCmbB = 134,
  ePcMtBtAtkFghtCmbC = 135,
  ePcMtBtAtkFghtCmbD = 136,
  ePcMtBtAtkFghtCmbE = 137,
  ePcMtBtAtkFghtCmbF = 138,
  ePcMtBtAtkFghtDsh = 139,
  ePcMtBtAtkFghtRrgSbt = 140,
  ePcMtBtAtkFghtRHKck = 141,
  ePcMtBtAtkFghtRLKck = 142,
  ePcMtBtAtkFghtTbgr = 143,
  ePcMtBtAtkFghtRlt = 144,
  ePcMtBtAtkFghtNlKck = 145,
  ePcMtBtAtkFghtRrgElbw = 146,
  ePcMtBtAtkFghtTrKick = 147,
  ePcMtBtAtkFghtLHk = 148,
  ePcMtBtAtkFghtLUpr = 149,
  ePcMtBtAtkFghtRrgSbt2 = 150,
  ePcMtBtAtkFghtDsprx = 151,
  ePcMtBtAtkFghtBbstr = 152,
  ePcMtBtAtkFghtDdt = 153,
  ePcMtBtAtkFghtPdrvr = 154,
  ePcMtBtAtkFghtFstnr = 155,
  ePcMtBtAtkFghtPbmb = 156,
  ePcMtBtAtkFght53 = 157,
  ePcMtBtAtkFghtKbrd = 158,
  ePcMtBtAtkFghtJmsprx = 159,
  ePcMtBtAtkFghtTgsprx = 160,
  ePcMtBtAtkFghtSldr = 161,
  ePcMtBtAtkFghtFnck = 162,
  ePcMtBtAtkFghtCpchd = 163,
  ePcMtBtAtkFghtRvsams = 164,
  ePcMtBtAtkFghtKnnk = 165,
  ePcMtBtAtkFghtWwrst = 166,
  ePcMtBtChgStrt = 167,
  ePcMtBtChgLp = 168,
  ePcMtBtAtkChg = 169,
  ePcMtBtAtkChgUp = 170,
  ePcMtBtAtkNrmlDsh = 171,
  ePcMtBtCtch = 172,
  ePcMtBtCtchMss = 173,
  ePcMtBtLSSonic = 174,
  ePcMtBtLSBombSt = 175,
  ePcMtBtLSBombLp = 176,
  ePcMtBtLSBombEd = 177,
  ePcMtBtDSIdl = 178,
  ePcMtBtDSRn = 179,
  ePcMtBtDSAtkR = 180,
  ePcMtBtDSAtkL = 181,
  ePcMtBtDSAtkB = 182,
  ePcMtBtDSAtkFDwn = 183,
  ePcMtBtDSAtkBDwn = 184,
  ePcMtBtDSAtkMdR = 185,
  ePcMtBtDSAtkMdL = 186,
  ePcMtBtDSAtkMdB = 187,
  ePcMtBtDSAtkMdFDwn = 188,
  ePcMtBtDSAtkMdBDwn = 189,
  ePcMtBtDSAtkLgSt = 190,
  ePcMtBtDSAtkLgLp = 191,
  ePcMtBtDSAtkLgEd = 192,
  ePcMtBtAtkDwnB = 193,
  ePcMtBtDashAtk = 194,
  ePcMtBtOkiAtk = 195,
  ePcMtBtJmpDwnSt = 196,
  ePcMtBtJmpDwnAr = 197,
  ePcMtBtJmpDwnAt = 198,
  ePcMtBtPullBK = 199,
  ePcMtStdDoorOpen = 200,
  ePcMtStdBoxKick = 201,
  ePcMtStdPickUp = 202,
  ePcMtStdPickUpOk = 203,
  ePcMtStdPickUpMiss = 204,
  ePcMtMax = 205,
  ePcMtBt01IdlU = 206,
  ePcMtBt01IdlM = 207,
  ePcMtBt01IdlD = 208,
  ePcMtBt01WlkUF = 209,
  ePcMtBt01WlkMF = 210,
  ePcMtBt01WlkBF = 211,
  ePcMtBt01WlkUB = 212,
  ePcMtBt01WlkMB = 213,
  ePcMtBt01WlkBB = 214,
  ePcMtBt01WlkUL = 215,
  ePcMtBt01WlkML = 216,
  ePcMtBt01WlkBL = 217,
  ePcMtBt01WlkUR = 218,
  ePcMtBt01WlkMR = 219,
  ePcMtBt01WlkBR = 220,
  ePcMtBt01Rn = 221,
  ePcMtBt01RnEd = 222,
  ePcMtBtAtk01CmbUpA = 223,
  ePcMtBtAtk01CmbUpB = 224,
  ePcMtBtAtk01CmbUpC = 225,
  ePcMtBtAtk01CmbUpD = 226,
  ePcMtBtAtk01CmbUpFnsh = 227,
  ePcMtBtAtk01CmbMidA = 228,
  ePcMtBtAtk01CmbMidB = 229,
  ePcMtBtAtk01CmbMidC = 230,
  ePcMtBtAtk01CmbMidD = 231,
  ePcMtBtAtk01CmbMidE = 232,
  ePcMtBtAtk01CmbMidF = 233,
  ePcMtBtAtk01CmbMidG = 234,
  ePcMtBtAtk01CmbMidH = 235,
  ePcMtBtAtk01CmbMidFnsh = 236,
  ePcMtBtAtk01CmbBtmA = 237,
  ePcMtBtAtk01CmbBtmB = 238,
  ePcMtBtAtk01CmbBtmC = 239,
  ePcMtBtAtk01CmbBtmD = 240,
  ePcMtBtAtk01CmbBtmE = 241,
  ePcMtBtAtk01CmbBtmF = 242,
  ePcMtBtAtk01CmbBtmG = 243,
  ePcMtBtAtk01CmbBtmFnsh = 244,
  ePcMtBtAtk01CmbR2LFnsh = 245,
  ePcMtBtAtk01Rng = 246,
  ePcMtBtAtk01Dwn = 247,
  ePcMtBtAtk01Tb = 248,
  ePcMtBattou01 = 249,
  ePcMtBattou01Lp = 250,
  ePcMtLSBattou01 = 251,
  ePcMtLSNoutou01 = 252,
  ePcMtBtAtk01Max = 253,
  ePcMtBt02IdlU = 254,
  ePcMtBt02IdlM = 255,
  ePcMtBt02IdlD = 256,
  ePcMtBt02WlkUF = 257,
  ePcMtBt02WlkMF = 258,
  ePcMtBt02WlkBF = 259,
  ePcMtBt02WlkUB = 260,
  ePcMtBt02WlkMB = 261,
  ePcMtBt02WlkBB = 262,
  ePcMtBt02WlkUL = 263,
  ePcMtBt02WlkML = 264,
  ePcMtBt02WlkBL = 265,
  ePcMtBt02WlkUR = 266,
  ePcMtBt02WlkMR = 267,
  ePcMtBt02WlkBR = 268,
  ePcMtBt02Rn = 269,
  ePcMtBt02RnEd = 270,
  ePcMtBtAtk02CmbUpA = 271,
  ePcMtBtAtk02CmbUpB = 272,
  ePcMtBtAtk02CmbUpC = 273,
  ePcMtBtAtk02CmbUpD = 274,
  ePcMtBtAtk02CmbUpE = 275,
  ePcMtBtAtk02CmbUpF = 276,
  ePcMtBtAtk02CmbUpG = 277,
  ePcMtBtAtk02CmbUpFnsh = 278,
  ePcMtBtAtk02CmbBtmA = 279,
  ePcMtBtAtk02CmbBtmB = 280,
  ePcMtBtAtk02CmbBtmC = 281,
  ePcMtBtAtk02CmbBtmD = 282,
  ePcMtBtAtk02CmbBtmE = 283,
  ePcMtBtAtk02CmbBtmF = 284,
  ePcMtBtAtk02CmbBtmG = 285,
  ePcMtBtAtk02CmbBtmH = 286,
  ePcMtBtAtk02CmbBtmFnsh = 287,
  ePcMtBtAtk02CmbL2RFnsh = 288,
  ePcMtBtAtk02CmbD2UFnsh = 289,
  ePcMtBtAtk02RngCmbA = 290,
  ePcMtBtAtk02RngCmbB = 291,
  ePcMtBtAtk02RngCmbC = 292,
  ePcMtBtAtk02Dwn = 293,
  ePcMtBtAtk02Tb = 294,
  ePcMtBattou02 = 295,
  ePcMtBattou02Lp = 296,
  ePcMtBattou02Ed = 297,
  ePcMtLSBattou021 = 298,
  ePcMtLSBattou022 = 299,
  ePcMtLSNoutou02 = 300,
  ePcMtBtAtk02Max = 301,
  ePcMtBt03IdlU = 302,
  ePcMtBt03IdlM = 303,
  ePcMtBt03IdlD = 304,
  ePcMtBt03WlkUF = 305,
  ePcMtBt03WlkMF = 306,
  ePcMtBt03WlkBF = 307,
  ePcMtBt03WlkUB = 308,
  ePcMtBt03WlkMB = 309,
  ePcMtBt03WlkBB = 310,
  ePcMtBt03WlkUL = 311,
  ePcMtBt03WlkML = 312,
  ePcMtBt03WlkBL = 313,
  ePcMtBt03WlkUR = 314,
  ePcMtBt03WlkMR = 315,
  ePcMtBt03WlkBR = 316,
  ePcMtBt03Rn = 317,
  ePcMtBt03RnEd = 318,
  ePcMtBtAtk03CmbUpA = 319,
  ePcMtBtAtk03CmbUpB = 320,
  ePcMtBtAtk03CmbUpC = 321,
  ePcMtBtAtk03CmbUpD = 322,
  ePcMtBtAtk03CmbUpE = 323,
  ePcMtBtAtk03CmbUpFnsh = 324,
  ePcMtBtAtk03CmbMidA = 325,
  ePcMtBtAtk03CmbMidB = 326,
  ePcMtBtAtk03CmbMidC = 327,
  ePcMtBtAtk03CmbMidD = 328,
  ePcMtBtAtk03CmbMidE = 329,
  ePcMtBtAtk03CmbMidF = 330,
  ePcMtBtAtk03CmbMidG = 331,
  ePcMtBtAtk03CmbMidH = 332,
  ePcMtBtAtk03CmbMidFnsh = 333,
  ePcMtBtAtk03CmbBtmA = 334,
  ePcMtBtAtk03CmbBtmB = 335,
  ePcMtBtAtk03CmbBtmC = 336,
  ePcMtBtAtk03CmbBtmD = 337,
  ePcMtBtAtk03CmbBtmFnsh = 338,
  ePcMtBtAtk03CmbL2RFnsh = 339,
  ePcMtBtAtk03Rng = 340,
  ePcMtBtAtk03Dwn = 341,
  ePcMtBtAtk03Tb = 342,
  ePcMtBattou03 = 343,
  ePcMtBattou03Lp = 344,
  ePcMtBattou03Ed = 345,
  ePcMtLSBattou03 = 346,
  ePcMtLSNoutou03 = 347,
  ePcMtBtAtk03Max = 348,
  ePcMtBt04IdlU = 349,
  ePcMtBt04IdlM = 350,
  ePcMtBt04IdlD = 351,
  ePcMtBt04WlkUF = 352,
  ePcMtBt04WlkMF = 353,
  ePcMtBt04WlkBF = 354,
  ePcMtBt04WlkUB = 355,
  ePcMtBt04WlkMB = 356,
  ePcMtBt04WlkBB = 357,
  ePcMtBt04WlkUL = 358,
  ePcMtBt04WlkML = 359,
  ePcMtBt04WlkBL = 360,
  ePcMtBt04WlkUR = 361,
  ePcMtBt04WlkMR = 362,
  ePcMtBt04WlkBR = 363,
  ePcMtBt04Rn = 364,
  ePcMtBt04RnEd = 365,
  ePcMtBtAtk04CmbUpA = 366,
  ePcMtBtAtk04CmbUpB = 367,
  ePcMtBtAtk04CmbUpC = 368,
  ePcMtBtAtk04CmbUpD = 369,
  ePcMtBtAtk04CmbUpFnsh = 370,
  ePcMtBtAtk04CmbMidA = 371,
  ePcMtBtAtk04CmbMidB = 372,
  ePcMtBtAtk04CmbMidC = 373,
  ePcMtBtAtk04CmbMidD = 374,
  ePcMtBtAtk04CmbMidE = 375,
  ePcMtBtAtk04CmbMidF = 376,
  ePcMtBtAtk04CmbMidG = 377,
  ePcMtBtAtk04CmbMidFnsh = 378,
  ePcMtBtAtk04CmbBtmA = 379,
  ePcMtBtAtk04CmbBtmB = 380,
  ePcMtBtAtk04CmbBtmC = 381,
  ePcMtBtAtk04CmbBtmD = 382,
  ePcMtBtAtk04CmbBtmE = 383,
  ePcMtBtAtk04CmbBtmFnsh = 384,
  ePcMtBtAtk04CmbR2LFnsh = 385,
  ePcMtBtAtk04Rng = 386,
  ePcMtBtAtk04Dwn = 387,
  ePcMtBtAtk04Tb = 388,
  ePcMtBattou04 = 389,
  ePcMtBattou04Lp = 390,
  ePcMtBattou04Ed = 391,
  ePcMtLSBattou041 = 392,
  ePcMtLSBattou042 = 393,
  ePcMtLSNoutou04 = 394,
  ePcWepMtUpFnsh04 = 395,
  ePcWepMt04MidF = 396,
  ePcWepMt04MidG = 397,
  ePcWepMt04MidFnsh = 398,
  ePcWepMt04BtmE = 399,
  ePcWepMt04Rng = 400,
  ePcWepMt04Dwn = 401,
  ePcMtBtAtk04Max = 402,
};

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
  /* 0x0018 */ enum pcItem id;
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
public:
  /* 0x0000 */ int Padding_0; // edited, was long
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

struct TFAnmObjF32_3
{
    struct GETA
    {
      /* 0x0000 */ float Value;
      /* 0x0004 */ float Wait;
      /* 0x0008 */ float LockWait;
      /* 0x000c */ float InSlope;
      /* 0x0010 */ float OutSlope;
    }; /* size: 0x0014 */

    struct tagMAIN {
        /* 0x0000 */ struct _WFAnmObjHeader** ppAnmData;
        /* 0x0004 */ enum TFANMOBJDATATYPE DataType;
        /* 0x0008 */ struct Vec* pData;
        /* 0x000c */ struct Vec NowValue;
        union
        {
            struct /* bitfield */
            {
                /* 0x0018 */ unsigned int Idle : 1; /* bit position: 0 */
                /* 0x0018 */ unsigned int Pause : 1; /* bit position: 1 */
                /* 0x0018 */ unsigned int Interpolate : 1; /* bit position: 2 */
                /* 0x0018 */ unsigned int Reverse : 1; /* bit position: 3 */
                /* 0x0018 */ unsigned int Init : 1; /* bit position: 4 */
                /* 0x0018 */ unsigned int NoUpdate : 1; /* bit position: 5 */
                /* 0x0018 */ unsigned int FlagPad : 26; /* bit position: 6 */
            }; /* bitfield */
            /* 0x0018 */ unsigned int Flag;
        }; /* size: 0x0004 */
        /* 0x001c */ unsigned char NextType;
        /* 0x001d */ unsigned char PrevType;
        /* 0x001e */ unsigned short KeyCount;
        /* 0x0020 */ unsigned short KeyCountMax;
        /* 0x0022 */ unsigned short KeyListCount;
        /* 0x0024 */ float Tick;
        /* 0x0028 */ float NextWait;
        /* 0x002c */ float PrevWait;
        /* 0x0030 */ void* pNextKey[3];
        /* 0x003c */ void* pPrevKey[3];
        /* 0x0048 */ float NextValue[3];
        /* 0x0054 */ float PrevValue[3];
        /* 0x0060 */ float NextSlope[3];
        /* 0x006c */ float PrevSlope[3];
        /* 0x0078 */ struct TFAnmObjF32_3::GETA NextGeta[3];
        /* 0x00b4 */ struct TFAnmObjF32_3::GETA PrevGeta[3];
        /* 0x00f0 */ struct _WFAnmObjKeyListHeader* pNowKeyListHeader[3];
        /* 0x00fc */ struct TFAnmObjF32_3::GETA NowKeyGeta[3];
        /* 0x0138 */ unsigned int NowKeyTypeSize;
        /* 0x013c */ void* pNowKey[3];
    };
}; /* size: 0x0148 */

struct TGmfDrawCmpParam
{
  /* 0x0000 */ unsigned char NotUseLightMask;
  /* 0x0001 */ unsigned char NotUseLightMask_Toon;
  /* 0x0002 */ unsigned char UseAmbLightIndex;
  union
  {
    /* 0x0003 */ unsigned char Flag;
    struct /* bitfield */
    {
      /* 0x0003 */ unsigned char UseColor : 1; /* bit position: 0 */
      /* 0x0003 */ unsigned char FogCut : 1; /* bit position: 1 */
      /* 0x0003 */ unsigned char BackClip : 1; /* bit position: 2 */
      /* 0x0003 */ unsigned char Padding : 5; /* bit position: 3 */
    }; /* bitfield */
  }; /* size: 0x0001 */
}; /* size: 0x0004 */

struct TGmfNode
{
    struct tagMAIN {
        /* 0x0000 */ class TGmf* pGmf;
        /* 0x0004 */ struct _WGmfNodeSpec* pSpec;
        /* 0x0008 */ class TGmfNode* pBaseAttachTarget;
        /* 0x000c */ class TGmf* pAttachModel;
        /* 0x0010 */ struct TGmfNodeStripList* pStripList;
        /* 0x0014 */ unsigned int StripListNum;
        /* 0x0018 */ struct TGmfNodeBlendSet* pBlendSet;
        /* 0x001c */ class TGmfNode* pParent;
        /* 0x0020 */ class TGmfNode* pChild;
        /* 0x0024 */ class TGmfNode* pPrev;
        /* 0x0028 */ class TGmfNode* pNext;
        /* 0x002c */ class TGmfNode* pBlendMatrixNodeNext;
        /* 0x0030 */ class TGmfNode* pBlendMatrixNodePrev;
        /* 0x0034 */ bool Visible;
        /* 0x0035 */ bool Hide;
        /* 0x0036 */ bool VC;
        /* 0x0037 */ bool Big;
        /* 0x0038 */ struct Vec Position;
        /* 0x0044 */ struct Vec Rotation;
        /* 0x0050 */ struct Vec Scale;
        /* 0x005c */ struct TGmfDrawCmpParam CmpP;
        /* 0x0060 */ unsigned int MulColor;
        /* 0x0064 */ bool SetupMatrixFlag;
        /* 0x0065 */ char Padding_10[3];
        /* 0x0068 */ enum TGMF_MATRIX MatrixType;
        /* 0x006c */ float LocalWorldMatrix[4][3];
        union
        {
            /* 0x009c */ float OrigLocalWorldMatrix[4][3];
            /* 0x009c */ float OrigWorldLocalMatrix[4][3];
        }; /* size: 0x0030 */
        /* 0x00cc */ int MotionNum;
        /* 0x00d0 */ struct TGmfNodeMotion* Motion[2];
        /* 0x00d4 */ struct TGmfNodeShape* pShape;
        /* 0x00d8 */ enum TGMF_LIGHTTYPE LightType;
        /* 0x00dc */ struct TGMFLIGHT* pLightParam;
    }; /* size: 0x00e0 */
};

class TFAnm {
public:
    struct tagMAIN
    {
      /* 0x0000 */ class TGmfNode::tagMAIN* pGmfNode;
      /* 0x0004 */ class TFAnmObjF32_3::tagMAIN* pAnmObj3;
      /* 0x0008 */ int AnmObj3Num;
      /* 0x000c */ class TFAnmObj* pAnmObj;
      /* 0x0010 */ int AnmObjNum;
      /* 0x0014 */ float Rate;
      union
      {
        struct /* bitfield */
        {
          /* 0x0018 */ unsigned int Idle : 1; /* bit position: 0 */
          /* 0x0018 */ unsigned int Pause : 1; /* bit position: 1 */
          /* 0x0018 */ unsigned int Loop : 1; /* bit position: 2 */
          /* 0x0018 */ unsigned int PlayStart : 1; /* bit position: 3 */
          /* 0x0018 */ unsigned int NoUpdate : 1; /* bit position: 4 */
          /* 0x0018 */ unsigned int Rewind : 1; /* bit position: 5 */
        }; /* bitfield */
        /* 0x0018 */ unsigned int Flag;
      }; /* size: 0x0004 */
      /* 0x001c */ float PlayTick;
      /* 0x0020 */ float StartTick;
      /* 0x0024 */ float EndTick;
    }; /* size: 0x0028 */
};

class TGmf {
public:
    struct tagMOVMOTION
    {
      /* 0x0000 */ class TGanPlay* pGanPlay;
      /* 0x0004 */ bool ValidPrevPos;
      /* 0x0005 */ char Padding_34[3];
      /* 0x0008 */ struct Vec NowPos;
      /* 0x0014 */ struct Vec PrevPos;
    }; /* size: 0x0020 */

    struct tagMOTION
    {
      /* 0x0000 */ bool Valid[2];
      /* 0x0002 */ char Padding_35[2];
      /* 0x0004 */ float BlendWeight;
      /* 0x0008 */ bool Blend;
      /* 0x0009 */ char Padding_36[3];
      /* 0x000c */ float BlendRate;
      /* 0x0010 */ float BlendSpeed;
      /* 0x0014 */ int Active;
      /* 0x0018 */ int MotionType2State;
      /* 0x001c */ bool MotionAttachFlag;
      /* 0x001d */ bool PlayMotionFlag;
      /* 0x001e */ char Padding_37[2];
      /* 0x0020 */ float PlayMotionTick;
      /* 0x0024 */ float MotionRate;
      /* 0x0028 */ class TGan* pMotion[2];
      /* 0x0030 */ int TimingSoundHandle[2];
      /* 0x0038 */ bool Loop;
      /* 0x0039 */ bool LoopFlag2[2];
      /* 0x003b */ char Padding_38;
      /* 0x003c */ float LoopMotionTime2[2];
      /* 0x0044 */ bool** pSetupMatrixFlag[2];
      /* 0x004c */ int SetupMatrixFlagNum[2];
      /* 0x0054 */ class TFAnm MotionType3Anm[2];
      /* 0x00a4 */ struct TGmf::tagMOVMOTION MOVInfo[2];
    }; /* size: 0x00e4 */

    struct tagMAIN
    {
      /* 0x0000 */ class TGmf* pGlobalListPrev;
      /* 0x0004 */ class TGmf* pGlobalListNext;
      /* 0x0008 */ void* pDeleteCallback[16] /* function */;
      /* 0x0048 */ void* pDeleteCallbackData[16];
      /* 0x0088 */ int DeleteCallbackNum;
      /* 0x008c */ bool ProcessBlock;
      /* 0x008d */ char Padding_40[3];
      /* 0x0090 */ enum TGMF_STATE State;
      /* 0x0094 */ struct TGmfNodeStripList* pNodeStripList;
      /* 0x0098 */ unsigned int NodeStripListNum;
      /* 0x009c */ unsigned int UseHeap;
      /* 0x00a0 */ int MaterialNum;
      /* 0x00a4 */ bool BaseAttach;
      /* 0x00a5 */ char Padding_41[3];
      /* 0x00a8 */ class TGmf* pBaseAttachTarget;
      /* 0x00ac */ class TGmf* pBaseAttachModel[8];
      /* 0x00cc */ int BaseAttachModelNum;
      /* 0x00d0 */ class TGmfNode* pAttachTarget;
      /* 0x00d4 */ struct _WGmfSpec* pSpec;
      /* 0x00d8 */ struct _WGmfNodeSpec* pTopNodeSpec;
      /* 0x00dc */ struct Vec RootPosition;
      /* 0x00e8 */ struct Vec RootRotation;
      /* 0x00f4 */ struct Vec RootScale;
      /* 0x0100 */ float RootMatrix[4][3];
      /* 0x0130 */ bool ValidRootMatrix;
      /* 0x0131 */ char Padding_42[3];
      /* 0x0134 */ float LocalWorldMatrix[4][3];
      /* 0x0164 */ bool SetupMatrixFlag;
      /* 0x0165 */ char Padding_43[3];
      /* 0x0168 */ enum TGMF_MATRIX MatrixType;
      /* 0x016c */ class TGmfNode* pTopNode;
      /* 0x0170 */ struct TGmfMaterial* pTopMaterial;
      /* 0x0174 */ struct TGmfTexture* pTopTexture;
      /* 0x0178 */ int VertFormat;
      /* 0x017c */ bool AlphaBlend;
      /* 0x017d */ char Padding_44[3];
      /* 0x0180 */ int Alpha;
      /* 0x0184 */ enum GHMR_BLEND BlendMode;
      /* 0x0188 */ bool EnableDestAlpha;
      /* 0x0189 */ char Padding_45[3];
      /* 0x018c */ int DestAlpha;
      /* 0x0190 */ unsigned int LightRegisterMap;
      /* 0x0194 */ int MaxAttachMotionNum;
      /* 0x0198 */ struct TGmf::tagMOTION Motion[4];
      /* 0x0528 */ bool EnableMotionSound;
      /* 0x0529 */ char Padding_46[3];
      /* 0x052c */ class TGmfNode* pCharLightNode[4];
      /* 0x053c */ class TGmfNode* pAmbientLightNode[4];
      /* 0x054c */ class TGmfNode* pDirLightNode[4];
      /* 0x055c */ class TGmfNode* pPointLightNode[4];
      /* 0x056c */ class TGmfNode* pSpotLightNode[4];
      /* 0x057c */ struct TGMFLIGHT* pLightParam;
      /* 0x0580 */ class TGmfNode* pNodeBuffer;
      /* 0x0584 */ int NodeBufferAddress;
      /* 0x0588 */ class TGmfNode** pStripNodeBuffer;
      /* 0x058c */ int StripNodeBufferAddress;
      /* 0x0590 */ struct TGmfNodeMotion* pNodeMotionBuffer[2];
      /* 0x0594 */ int NodeMotionBufferAddress;
      /* 0x0598 */ struct TGmfNodeBlendSet* pNodeBlendSetBuffer;
      /* 0x059c */ int NodeBlendSetAddress;
      /* 0x05a0 */ struct TGmfNodeBlend* pNodeBlendBuffer;
      /* 0x05a4 */ int NodeBlendAddress;
      /* 0x05a8 */ struct TGmfNodeBlendWeight* pNodeBlendWeightBuffer;
      /* 0x05ac */ int NodeBlendWeightAddress;
      /* 0x05b0 */ unsigned char* pShapeBuffer;
      /* 0x05b4 */ int ShapeAddress;
      /* 0x05b8 */ float BoundSphereSize;
      /* 0x05bc */ bool NoClip;
      /* 0x05bd */ bool ReverseCulling;
      /* 0x05be */ bool DrawType2;
      /* 0x05bf */ bool UseBlendMatrix;
      /* 0x05c0 */ class TGmfNode* pBlendMatrixNodeFirst;
      /* 0x05c4 */ class TGmfNode* pBlendMatrixNodeLast;
      /* 0x05c8 */ bool UseMotionType3;
      /* 0x05c9 */ char Padding_47[3];
      /* 0x05cc */ struct TFANMOBJINFO* pMotType3AnmObjInfo;
      /* 0x05d0 */ int MotType3AnmObjInfoNum;
      /* 0x05d4 */ bool** pMotType3SetupMatrixFlag;
      /* 0x05d8 */ int MotType3SetupMatrixFlagNum;
      /* 0x05dc */ struct WGanFAnmSpec* pMotType3FAnmSpec;
      /* 0x05e0 */ bool UseMotionType2;
      /* 0x05e1 */ bool UseBlendWeightNormalize;
      /* 0x05e2 */ bool UseShadowDraw;
      /* 0x05e3 */ bool UseTexShadow;
      /* 0x05e4 */ struct tagGHMR_TEX TexShadowTex;
      /* 0x060c */ class TGmfNode* pTexShadowBaseNode;
      /* 0x0610 */ class TGmf* pTexShadowModel;
      /* 0x0614 */ bool ValidEasyShadowYPosition;
      /* 0x0615 */ char Padding_48[3];
      /* 0x0618 */ float EasyShadowYPosition;
      /* 0x061c */ float EasyShadowScaleXZ;
      /* 0x0620 */ bool ValidEasyShadowYukaNormal;
      /* 0x0621 */ char Padding_49[3];
      /* 0x0624 */ struct Vec EasyShadowYukaNormal;
      /* 0x0630 */ class TGmf* pTexShadowGmfNext;
      /* 0x0634 */ class TGmf* pTexShadowGmfPrev;
      /* 0x0638 */ bool TexShadowSetupDraw;
      /* 0x0639 */ bool EasyShadowDrawOnly;
      /* 0x063a */ char Padding_50[2];
      /* 0x063c */ struct tagGHMR_TEX* pGradationTexture;
      /* 0x0640 */ struct Vec TexShadowCameraPosition;
      /* 0x064c */ struct Vec TexShadowCameraTarget;
      /* 0x0658 */ float TexShadowAreaHeight;
      /* 0x065c */ float TexShadowAreaWidth;
      /* 0x0660 */ int TexShadowPixelWidth;
      /* 0x0664 */ int TexShadowPixelHeight;
      /* 0x0668 */ short MotionSoundSeqLocalValue[8];
      /* 0x0678 */ unsigned short MotionSoundSeqLocalValueValid;
      /* 0x067a */ unsigned short MotionSoundSeqLocalValueValidNum;
      /* 0x067c */ class TGmf* pLinkTexShadowGmf[16];
      /* 0x06bc */ int LinkTexShadowGmfNum;
      /* 0x06c0 */ class TGmf* pLinkTexShadowTarget;
      /* 0x06c4 */ float LinkTexShadowGmfDrawArea[4];
    }; /* size: 0x06d4 */
};

struct TGmfNodeMotion
{
  /* 0x0000 */ bool Valid;
  /* 0x0001 */ bool Play;
  /* 0x0002 */ char Padding_8[2];
  /* 0x0004 */ struct Vec Pos;
  /* 0x0010 */ struct Vec Rot;
  /* 0x001c */ struct Vec Scale;
  /* 0x0028 */ class TFAnm::tagMAIN Anm;
}; /* size: 0x0050 */

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

class rPrimBase
{
  /* 0x0000 */ int Padding_597; // edited, was long
  union
  {
    struct /* bitfield */
    {
      /* 0x0004 */ unsigned int m_bInit : 1; /* bit position: 0 */
      /* 0x0004 */ unsigned int m_bActive : 1; /* bit position: 1 */
      /* 0x0004 */ unsigned int m_bVisible : 1; /* bit position: 2 */
      /* 0x0004 */ unsigned int m_bPause : 1; /* bit position: 3 */
      /* 0x0004 */ unsigned int m_bColorAnime : 1; /* bit position: 4 */
      /* 0x0004 */ unsigned int m_bRot : 1; /* bit position: 5 */
      /* 0x0004 */ unsigned int m_bBuillBoard : 1; /* bit position: 6 */
      /* 0x0004 */ unsigned int m_bTransform : 1; /* bit position: 7 */
      /* 0x0004 */ unsigned int m_bClip : 1; /* bit position: 8 */
      /* 0x0004 */ unsigned int m_pad : 23; /* bit position: 9 */
    }; /* bitfield */
    /* 0x0004 */ unsigned int m_BitFlag;
  }; /* size: 0x0004 */
  /* 0x0008 */ class rPrimBase* m_pPrev;
  /* 0x000c */ class rPrimBase* m_pNext;
}; /* size: 0x0010 */

class rPrimUnific
{
  /* 0x0000 */ int Padding_247; // edited, was long
  /* 0x0004 */ class rPrimBase* m_pListTop;
  /* 0x0008 */ class rPrimBase* m_pListEnd;
  /* 0x000c */ enum GHMR_PROJECT m_ProjectMode;
  /* 0x0010 */ enum GHMR_PRIMTYPE m_PrimType;
  /* 0x0014 */ unsigned int m_PrimNum;
  /* 0x0018 */ struct tagGHMR_TEX* m_pTex;
  union
  {
    struct /* bitfield */
    {
      /* 0x001c */ unsigned int m_bInit : 1; /* bit position: 0 */
      /* 0x001c */ unsigned int m_bVisible : 1; /* bit position: 1 */
      /* 0x001c */ unsigned int m_bActive : 1; /* bit position: 2 */
      /* 0x001c */ unsigned int m_bPause : 1; /* bit position: 3 */
      /* 0x001c */ unsigned int m_bZTest : 1; /* bit position: 4 */
      /* 0x001c */ unsigned int m_bZWrite : 1; /* bit position: 5 */
      /* 0x001c */ unsigned int m_pad : 26; /* bit position: 6 */
    }; /* bitfield */
    /* 0x001c */ unsigned int m_BitFlag;
  }; /* size: 0x0004 */
}; /* size: 0x0020 */

struct rColor
{
  /* 0x0000 */ float r;
  /* 0x0004 */ float g;
  /* 0x0008 */ float b;
  /* 0x000c */ float a;
}; /* size: 0x0010 */

struct rST
{
  /* 0x0000 */ float s;
  /* 0x0004 */ float t;
}; /* size: 0x0008 */

struct rColorAnime
{
  /* 0x0000 */ struct rColor Add;
  /* 0x0010 */ struct rColor Target;
  /* 0x0020 */ unsigned int Count;
  /* 0x0024 */ unsigned int Frame;
}; /* size: 0x0028 */

class rQuad
{
  /* 0x0000 */ int Padding_173[4]; // edited, was long
  /* 0x0010 */ struct Vec m_RootPosition;
  /* 0x001c */ struct Vec m_LocalPosition[4];
  /* 0x004c */ struct rColor m_Color;
  /* 0x005c */ struct rST m_ST[4];
  /* 0x007c */ struct rColorAnime m_ColorAnime;
  /* 0x00a4 */ struct Vec m_RotAngle;
}; /* size: 0x00b0 */

class EffectCloseContest
{
public:
  struct Common
  {
      /* 0x0000 */ unsigned char LoadState;
      /* 0x0001 */ char Padding_1153[3];
      /* 0x0004 */ struct tagGHMR_TEX Tex;
  }; /* size: 0x002c */
  /* 0x0000 */ char Padding_0[0x50]; // edited
  union
  {
    struct /* bitfield */
    {
      /* 0x0050 */ unsigned char m_bState : 3; /* bit position: 0 */
      /* 0x0050 */ unsigned char m_bInit : 1; /* bit position: 3 */
      /* 0x0050 */ unsigned char m_bActive : 1; /* bit position: 4 */
      /* 0x0050 */ unsigned char m_bVisible : 1; /* bit position: 5 */
      /* 0x0050 */ unsigned char m_bPause : 1; /* bit position: 6 */
      /* 0x0050 */ unsigned char m_bPad : 1; /* bit position: 7 */
    }; /* bitfield */
    struct
    {
      /* 0x0050 */ unsigned char m_Flag;
      /* 0x0051 */ char Padding_1155[3];
      /* 0x0054 */ class rPrimUnific m_QuadUni;
      /* 0x0074 */ class rQuad m_Quad;
      /* 0x0124 */ bool m_QuadScale;
      /* 0x0125 */ char Padding_1156[3];
      /* 0x0128 */ float m_SetCount;
      /* 0x012c */ float m_SetRate;
      /* 0x0130 */ float m_TimeCount;
      /* 0x0134 */ int m_TimeFrame;
      /* 0x0138 */ float m_TimeRate;
    }; /* size: 0x00e6 */
  }; /* size: 0x00e6 */
  union                                                   
  {                                                       
    /* 0x0000 */ struct EffectCloseContest::Common Uni;   
    /* 0x0000 */ char** ParamName /* zero-length array */;
    /* 0x0000 */ int* Param /* zero-length array */;      
  }; /* size: 0x002c */                                   
}; /* size: 0x013c */

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
  /* 0x017c */ enum pcMotion motionNo;
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
    /* 0x02fc */ unsigned short flag;  // 0x2fc
                 char pad_2fe[2];
                 unsigned short flag2; // 0x300
                 char pad_302[2];
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

class HrBattleIcon
{
public: // edited
  // union                                                   // edited
  // {                                                       // edited
    /* 0x0000 */ //int mBattleIconTodomeWait;                // edited
    /* 0x0000 */ //struct HrBattleIcon::UNION uni;           // edited
    /* 0x0000 */ //char** ParamName /* zero-length array */; // edited
    struct
    {
      /* 0x0000 */ //int* Param /* zero-length array */;
      /* 0x0001 */ char Padding_175[0x50];
      /* 0x0050 */ enum m_IconStat D_BICON_STAT;
      /* 0x0054 */ enum m_YoyakuIcon m_YoyakuIcon;
      /* 0x0058 */ float m_LockOnDist;
      /* 0x005c */ float m_LockOnCircleSize;
      /* 0x0060 */ float m_BIcon_Block_Size;
      /* 0x0064 */ unsigned char m_blockcolor[3];
      /* 0x0067 */ unsigned char m_ColorType;
      /* 0x0068 */ enum E_CON_ACT_ID m_ConActID[2];
      /* 0x0070 */ short m_ConActCount[2];
      /* 0x0074 */ short m_Counter;
      /* 0x0076 */ short m_DelCounter;
      /* 0x0078 */ int m_Arrow_AppearCnt;
      /* 0x007c */ int m_Rotate_AnimCnt;
      /* 0x0080 */ struct Vec m_TmpJstVec;
      /* 0x008c */ short m_PyokoCnt;
      /* 0x008e */ short m_PyokoFlag;
      /* 0x0090 */ float m_PyokoRatio;
      /* 0x0094 */ unsigned char m_PyokoAlpha;
      /* 0x0095 */ bool m_DrawHitCmbFlag;
      /* 0x0096 */ char Padding_176[2];
      /* 0x0098 */ float m_TsubaRatio[36];
      /* 0x0128 */ struct Vec* m_pPosition[2];
      /* 0x0130 */ class TGmfNode* m_pNode;
      /* 0x0134 */ unsigned char m_Color[3][2];
      /* 0x013a */ short m_HpBerCounter;
      /* 0x013c */ short m_LineCounter;
      /* 0x013e */ char m_MeterCounter;
      /* 0x013f */ char Padding_177;
      /* 0x0140 */ float m_Tension[2];
      /* 0x0148 */ float m_TMeterRevPos[2];
      /* 0x0150 */ short m_GetMoney[2];
      /* 0x0154 */ short m_HitNum[2];
      /* 0x0158 */ float m_HP[3];
      /* 0x0164 */ unsigned char m_HPBaseAlpha;
      /* 0x0165 */ unsigned char m_HPVirtAlpha;
      /* 0x0166 */ short m_BottanCounter[4];
      /* 0x016e */ short m_TergetIconCount;
      /* 0x0170 */ enum D_TODOME_DIRECT m_Direct;
      /* 0x0174 */ int m_Angle;
      /* 0x0178 */ int m_Soundid;
      union
      {
        struct /* bitfield */
        {
          /* 0x017c */ unsigned short fdrawbase : 1; /* bit position: 0 */
          /* 0x017c */ unsigned short fdrawmoney : 1; /* bit position: 1 */
          /* 0x017c */ unsigned short fdrawtension : 1; /* bit position: 2 */
          /* 0x017c */ unsigned short fdrawheart : 1; /* bit position: 3 */
          /* 0x017c */ unsigned short fdrawterget : 1; /* bit position: 4 */
          /* 0x017c */ unsigned short fdrawhp : 1; /* bit position: 5 */
          /* 0x017c */ unsigned short fdraw_word : 4; /* bit position: 6 */
          /* 0x017c */ unsigned short fhptype : 2; /* bit position: 10 */
          /* 0x017c */ unsigned short pad : 4; /* bit position: 12 */
        }; /* bitfield */
        /* 0x017c */ unsigned short flag;
      }; /* size: 0x0002 */
    }; /* size: 0x012c */
  //}; /* size: 0x0b7c */ // edited
}; /* size: 0x0180 */

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
               struct stCharaStatus mCharaStatus; // edited // size = 0x304
  /* 0x0314 */ char pad_304[0xC];
  /* 0x0320 */ class TGmf::tagMAIN* tagMain;
               char pad_324[0xD4]; // edited
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

struct stBike
{
  /* 0x0000 */ class TGmf* pGmf;
  /* 0x0004 */ class TGan** pGan;
  /* 0x0008 */ enum enBikeProc process;
  /* 0x000c */ enum enBikeFallProc fallProcess;
  /* 0x0010 */ int motionNo;
  /* 0x0014 */ int motionNumMax;
  /* 0x0018 */ int motionBrendNum;
  /* 0x001c */ int restoreMotionNo;
  /* 0x0020 */ float spd;
  /* 0x0024 */ float posYF;
  /* 0x0028 */ float posYB;
  /* 0x002c */ struct Vec pos;
  /* 0x0038 */ struct Vec beforePos;
  /* 0x0044 */ struct Vec beforePosF;
  /* 0x0050 */ struct Vec beforePosB;
  /* 0x005c */ struct Vec moveTempPos;
  /* 0x0068 */ struct Vec moveTempPosF;
  /* 0x0074 */ struct Vec moveTempPosB;
  /* 0x0080 */ struct Vec rot;
  /* 0x008c */ struct Vec oldRot;
  /* 0x0098 */ float oldPosY;
  /* 0x009c */ float oldPosYF;
  /* 0x00a0 */ float oldPosYB;
  /* 0x00a4 */ float adjustRotY;
  /* 0x00a8 */ bool hitCheck;
  /* 0x00a9 */ bool clash;
  /* 0x00aa */ bool hitWall;
  /* 0x00ab */ bool hitWall4Spin;
  /* 0x00ac */ bool alwaysCheckHitColl;
  /* 0x00ad */ char Padding_1117[3];
  /* 0x00b0 */ int hp;
  /* 0x00b4 */ int bgmHdl;
  /* 0x00b8 */ int seHndlEngine;
  /* 0x00bc */ int seHndlEngineNeutral;
  /* 0x00c0 */ int seHndlBreak;
  /* 0x00c4 */ int seHndlQBreak;
  /* 0x00c8 */ int seHndlAir;
  /* 0x00cc */ int seHndlNitro;
  /* 0x00d0 */ float seVolEngine;
  /* 0x00d4 */ float seVolEngineNeutral;
  /* 0x00d8 */ float seVolBreak;
  /* 0x00dc */ float seVolQBreak;
  /* 0x00e0 */ float seVolAir;
  /* 0x00e4 */ float seVolNitro;
  /* 0x00e8 */ int command;
  /* 0x00ec */ float acc;
  /* 0x00f0 */ bool useNitro;
  /* 0x00f1 */ char Padding_1118[3];
  /* 0x00f4 */ float nitro;
  /* 0x00f8 */ int nitroStopTick;
  /* 0x00fc */ float turnVal;
  /* 0x0100 */ float rimTurnVal;
  /* 0x0104 */ float qBreakRotY;
  /* 0x0108 */ enum enBikeSpinDir spinDir;
  /* 0x010c */ float spinY;
  /* 0x0110 */ bool wiry;
  /* 0x0111 */ bool clashMySelf;
  /* 0x0112 */ bool battle;
  /* 0x0113 */ bool rideOnStart;
  /* 0x0114 */ bool initHitJudge;
  /* 0x0115 */ bool pushAcceling;
  /* 0x0116 */ bool pushBreaking;
  /* 0x0117 */ bool cantRideOn;
  /* 0x0118 */ bool cantGetOff;
  /* 0x0119 */ bool cantHandling;
  /* 0x011a */ bool dispCantGetOff;
  /* 0x011b */ bool crash2Stand;
  /* 0x011c */ bool changeVolEngine;
  /* 0x011d */ bool changeVolEngineIdle;
  /* 0x011e */ char Padding_1119[2];
  /* 0x0120 */ int startWait;
  /* 0x0124 */ float rateTbl[3];
  /* 0x0130 */ class WAnimF inputRelay;
  /* 0x0150 */ class ghmGcCollObjCapsule hitColl;
  /* 0x01b0 */ class ghmGcCollObjCapsule rideColl;
  /* 0x0210 */ class ghmGcCollObjCapsule hitStgColl;
  /* 0x0270 */ class ghmGcCollObjCapsule hitStgCollF;
  /* 0x02d0 */ class ghmGcCollObjCapsule hitStgCollB;
  /* 0x0330 */ class ghmGcCollObj* footHitResultObj;
  /* 0x0334 */ class ghmGcCollObjHitResult* footHitResult;
  /* 0x0338 */ class ghmGcCollObjHitResultObj footHitResultObj2;
  /* 0x0384 */ class EventAreaCircle* pEvIcn;
  /* 0x0388 */ struct tagGHMR_TEX texSpeed;
  /* 0x03b0 */ class WAnimF cameraLength;
  /* 0x03d0 */ class WAnimF cameraAngle;
  /* 0x03f0 */ float rotZ;
  /* 0x03f4 */ bool forceHitStage;
  /* 0x03f5 */ bool crashFlag;
  /* 0x03f6 */ char Padding_1120[2];
  /* 0x03f8 */ int damageCount;
  /* 0x03fc */ class WAnimF prj;
  /* 0x041c */ class WAnimF modelAlpha;
}; /* size: 0x043c */

struct stBikeEffect
{
  /* 0x0000 */ class EffectDriftMark* pDriftMark;
  /* 0x0004 */ class EffectKeepSmoke* pKeepSmoke;
  /* 0x0008 */ int smoking;
  /* 0x000c */ struct Vec posTireOldL;
  /* 0x0018 */ struct Vec posTireOldR;
  /* 0x0024 */ class EffectFixFire* pFixFire[4];
}; /* size: 0x0034 */

class hPath
{
  /* 0x0000 */ long Padding_1121;
  /* 0x0004 */ char* mName;
  /* 0x0008 */ int mPointNum;
  /* 0x000c */ struct Vec* mPointList;
  /* 0x0010 */ class hPath* mpPrev;
  /* 0x0014 */ class hPath* mpNext;
  /* 0x0018 */ class hPathManager* mpParentManager;
  /* 0x001c */ unsigned short mObjCount;
  /* 0x001e */ char Padding_1122[2];
  /* 0x0020 */ int mEventNum;
  /* 0x0024 */ class hPathEvent* mEventTop;
  /* 0x0028 */ class hPathEvent* mEventEnd;
  /* 0x002c */ float mPathLength;
  union
  {
    struct /* bitfield */
    {
      /* 0x0030 */ unsigned short mbReady : 1; /* bit position: 0 */
      /* 0x0030 */ unsigned short mbMemAlloc : 1; /* bit position: 1 */
      /* 0x0030 */ unsigned short mbLock : 1; /* bit position: 2 */
      /* 0x0030 */ unsigned short mbVisible : 1; /* bit position: 3 */
      /* 0x0030 */ unsigned short mbCross : 1; /* bit position: 4 */
      /* 0x0030 */ unsigned short mbCrossPoint : 1; /* bit position: 5 */
      /* 0x0030 */ unsigned short mbLinear : 1; /* bit position: 6 */
      /* 0x0030 */ unsigned short mbPad : 9; /* bit position: 7 */
    }; /* bitfield */
    /* 0x0030 */ unsigned short mFlag;
  }; /* size: 0x0002 */
}; /* size: 0x0034 */

struct HLINK
{
  /* 0x0000 */ short pathindex;
  /* 0x0002 */ short point;
}; /* size: 0x0004 */

class hPathWalk
{
  /* 0x0000 */ struct Vec mPos;
  /* 0x000c */ struct Vec mDir;
  /* 0x0018 */ struct Vec mPointNow;
  /* 0x0024 */ struct Vec mPointNext;
  /* 0x0030 */ int mPoint;
  /* 0x0034 */ float mNowPoint;
  /* 0x0038 */ float mIpPoint;
  /* 0x003c */ float mSpeed;
  /* 0x0040 */ float mMargin;
  /* 0x0044 */ int mWaitTime;
  /* 0x0048 */ int mWaitTimeCnt;
  /* 0x004c */ class hPath* mWalkLine;
  /* 0x0050 */ struct HLINK* mpNextLink;
  /* 0x0054 */ class hPath mInterpolate;
  /* 0x0088 */ struct HLINK mVirtualLink;
  /* 0x008c */ class hPathScroll* mpScroll;
  union
  {
    struct /* bitfield */
    {
      /* 0x0090 */ unsigned short mMode : 4; /* bit position: 0 */
      /* 0x0090 */ unsigned short mState : 4; /* bit position: 4 */
      /* 0x0090 */ unsigned short mbRev : 1; /* bit position: 8 */
      /* 0x0090 */ unsigned short mbInterpolate : 1; /* bit position: 9 */
      /* 0x0090 */ unsigned short mbVisible : 1; /* bit position: 10 */
      /* 0x0090 */ unsigned short mbPathChange : 1; /* bit position: 11 */
      /* 0x0090 */ unsigned short mbTail : 1; /* bit position: 12 */
      /* 0x0090 */ unsigned short mbPad : 3; /* bit position: 13 */
    }; /* bitfield */
    /* 0x0090 */ unsigned short mFlag;
  }; /* size: 0x0002 */
}; /* size: 0x0094 */

class mHRBike
{
public:
  /* 0x0000 */ struct stBike mBike;
  /* 0x043c */ struct stBikeEffect mBikeEffect;
  /* 0x0470 */ bool mhitStage;
  /* 0x0471 */ char Padding_1123[3];
  /* 0x0474 */ float mWryRate;
  /* 0x0478 */ float mBankRate;
  /* 0x047c */ struct Vec mDamegeDir;
  /* 0x0488 */ float mDamegeRotY;
  /* 0x048c */ float mAccel;
  /* 0x0490 */ struct Vec mPassRot;
  /* 0x049c */ int mFireSEID;
  /* 0x04a0 */ float mHitWait;
  /* 0x04a4 */ bool mRotLock;
  /* 0x04a5 */ bool mBkAtkOk;
  /* 0x04a6 */ char Padding_1124[2];
  /* 0x04a8 */ class hPathWalk mBkPathWalk;
  /* 0x053c */ class hPath mVirtualBkPath;
}; /* size: 0x0570 */


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
    /* 0x1f5f0 */ unsigned short mFlag; // edited
                  char padding_1f5f2[2]; // edited
  }; /* size: 0x0008 */
  /* 0x1f5f8 */ class WAnimF mBikeRotY;
}; /* size: 0x1f618 */

class HsMessage
{
  /* 0x0000 */ long Padding_1049[8];
  /* 0x0020 */ unsigned int m_MesHndl[2];
  /* 0x0028 */ unsigned int m_ItemAtt;
  /* 0x002c */ unsigned int m_Item_Endian_No;
  /* 0x0030 */ int m_Price;
  /* 0x0034 */ unsigned int m_Disp_Flag_No;
  /* 0x0038 */ enum HS_MES_TYPE m_MesType;
  /* 0x003c */ enum HS_MES_CONDITION m_Condition;
  /* 0x0040 */ unsigned int m_Genre_No;
}; /* size: 0x0044 */

class HrMenuCurBox
{
  /* 0x0000 */ float m_BoxPosX;
  /* 0x0004 */ float m_BoxPosY;
  /* 0x0008 */ float m_BoxWidth;
  /* 0x000c */ float m_BoxHeight;
  /* 0x0010 */ unsigned int m_TriCurColor;
  /* 0x0014 */ unsigned int m_BoxColor;
  /* 0x0018 */ unsigned char m_Alpha;
  /* 0x0019 */ bool m_selectingFlag;
  /* 0x001a */ bool m_settingFlag;
  /* 0x001b */ char Padding_1050;
  /* 0x001c */ int m_ColorCnt;
  /* 0x0020 */ int m_FadeCnt;
}; /* size: 0x0024 */

class HrInGameMenu
{
public: // edited
  // union // edited
  // { // edited
    /* 0x0000 */ // struct HrInGameMenu::UNION uni; // edited
    /* 0x0000 */ // char** ParamName /* zero-length array */; // edited
    struct
    {
      ///* 0x0000 */ int* Param /* zero-length array */; // edited
      /* 0x0001 */ char Padding_1054[0x50]; // edited
      /* 0x0050 */ class HrSave* m_pHrSave;
      /* 0x0054 */ short m_fileCount;
      /* 0x0056 */ char Padding_1055[2];
      /* 0x0058 */ class ghmGcFile* m_fp;
      /* 0x005c */ void* m_pRsl;
      /* 0x0060 */ unsigned int m_Handl;
      /* 0x0064 */ unsigned int m_DEMOJimaku_Handl;
      /* 0x0068 */ struct tagGHMR_TEX m_Line_Tex[2];
      /* 0x00b8 */ void* m_ItemRsl;
      /* 0x00bc */ void* m_WeaponRsl;
      /* 0x00c0 */ int m_MenuMode;
      /* 0x00c4 */ short m_Counter;
      /* 0x00c6 */ short m_StartCounter;
      /* 0x00c8 */ short m_StartAnimCounter;
      /* 0x00ca */ short m_LineAnimCounter;
      /* 0x00cc */ short m_TitleLineAnimCounter;
      /* 0x00ce */ char Padding_1056[2];
      /* 0x00d0 */ float m_InfoScrollCounter;
      /* 0x00d4 */ int m_LeaveMenuFadeCnt;
      /* 0x00d8 */ class HsMessage m_WeaponInfoMes;
      /* 0x011c */ int m_SelectMenuWordNum[6];
      /* 0x0134 */ short m_PushCrossBtnCnt[4];
      /* 0x013c */ int m_PusshingCrossBtnFlag[4];
      /* 0x014c */ int m_PullCrossBtnStat[2][4];
      /* 0x016c */ int m_PullCrossBtnFlag[4];
      /* 0x017c */ class HrMenuCurBox m_CurBox[7];
      /* 0x0278 */ class HrMenuCurBox m_CurBoxConfig[2][4];
      /* 0x0398 */ const int layoutOffset;
      /* 0x039c */ long Padding_1057;
      /* 0x03a0 */ __int64 m_SecTemp[2];
      /* 0x03b0 */ bool m_StartWBFlag;
      /* 0x03b1 */ bool m_TitleWBFlag;
      /* 0x03b2 */ bool m_LoadYesNoWBFlag[4];
      /* 0x03b6 */ char Padding_1058[2];
      /* 0x03b8 */ char pad_03b8[0x40];// struct HrInGameMenu::WORD_BOX_KANRI m_WB_EXPLAIN[4];          // edited
      /* 0x03f8 */ char pad_03f8[0x40];// struct HrInGameMenu::WORD_BOX_KANRI m_WB_TIMER[4];            // edited
      /* 0x0438 */ char pad_0438[0x10];// struct HrInGameMenu::WORD_BOX_KANRI m_WB_TITLE;               // edited
      /* 0x0448 */ char pad_0448[0x10];// struct HrInGameMenu::WORD_BOX_KANRI m_WB_OTHER_LOAD;          // edited
      /* 0x0458 */ char pad_0458[0x40];// struct HrInGameMenu::WORD_BOX_KANRI m_WB_OTHER_LOAD_YESNO[4]; // edited
      /* 0x0498 */ char pad_0498[0x1A0];// struct HrInGameMenu::WORD_BOX_KANRI m_WB[26];                // edited
      /* 0x0638 */ int m_LineMode;
      /* 0x063c */ int m_LineSubJobMode;
      /* 0x0640 */ bool m_DrawVLineFlag;
      /* 0x0641 */ bool m_DrawHLineFlag;
      /* 0x0642 */ bool m_DebugDrawFlag;
      /* 0x0643 */ char Padding_1059;
      /* 0x0644 */ char pad_644[0xF00]; // struct HrInGameMenu::RANDH_LINE RandHLine[12];  // edited
      /* 0x1544 */ char pad_1544[0x2154];// struct HrInGameMenu::RANDV_LINE RandVLine[27]; // edited
      /* 0x3698 */ short m_SelectMenu[2];
      /* 0x369c */ short m_StatWariai[3];
      /* 0x36a2 */ char m_Before_StatWariai[3];
      /* 0x36a5 */ char Padding_1060;
      /* 0x36a6 */ short m_WariaiSa[3];
      union
      {
        struct /* bitfield */
        {
          /* 0x36ac */ unsigned char freturnmain : 1; /* bit position: 0 */
          /* 0x36ac */ unsigned char fendmenu : 1; /* bit position: 1 */
          /* 0x36ac */ unsigned char fdrawgraph : 1; /* bit position: 2 */
          /* 0x36ac */ unsigned char fmainstartend : 1; /* bit position: 3 */
          /* 0x36ac */ unsigned char fmainmenu : 1; /* bit position: 4 */
          /* 0x36ac */ unsigned char fsubmissionend : 1; /* bit position: 5 */
          /* 0x36ac */ unsigned char fFadeOutExit : 1; /* bit position: 6 */
          /* 0x36ac */ unsigned char pad1 : 1; /* bit position: 7 */
        }; /* bitfield */
        /* 0x36ac */ unsigned char m_Mainflag;
      }; /* size: 0x0001 */
      /* 0x36ad */ // char Padding_1061; // edited
      /* 0x36ae */ short m_MenuInfoCounter;
      /* 0x36b0 */ short m_MenuInfoLenght;
      /* 0x36b2 */ short m_MenuIconShift_X[7];
      /* 0x36c0 */ short m_MenuIconShift_COUNTER[6];
      /* 0x36cc */ char m_DotCircleXY_Grid[23][23][2];
      struct /* bitfield */
      {
        /* 0x3aee */ unsigned char m_Jimaku : 1; /* bit position: 0 */
        /* 0x3aee */ unsigned char m_JimakuTMP : 1; /* bit position: 1 */
        /* 0x3aee */ unsigned char m_MotionFlag : 1; /* bit position: 2 */
        /* 0x3aee */ unsigned char m_MotionFlagTMP : 1; /* bit position: 3 */
        /* 0x3aee */ unsigned char m_CamAxisXFlag : 1; /* bit position: 4 */
        /* 0x3aee */ unsigned char m_CamAxisXFlagTMP : 1; /* bit position: 5 */
        /* 0x3aee */ unsigned char m_CamAxisYFlag : 1; /* bit position: 6 */
        /* 0x3aee */ unsigned char m_CamAxisYFlagTMP : 1; /* bit position: 7 */
      }; /* bitfield */
      /* 0x3aee */ unsigned char m_Jimakuflag;
      /* 0x3aef */ bool m_MotionFlagNowOld[2];
      /* 0x3af1 */ char Padding_1062[3];
      /* 0x3af4 */ struct tagGHMR_TEX m_ItemImage;
      /* 0x3b1c */ short m_SelectItemId[2];
      /* 0x3b20 */ short m_ItemKeyPos[2];
      /* 0x3b24 */ short m_ItemKeyStat[2];
      /* 0x3b28 */ short m_ItemCursorCounter;
      /* 0x3b2a */ short m_LoadItemID;
      /* 0x3b2c */ short m_ItemWordNum[2];
      /* 0x3b30 */ short m_ScrollCounter;
      /* 0x3b32 */ short m_UseBoxCounter[2];
      /* 0x3b36 */ short m_ItemImageCounter;
      union
      {
        struct /* bitfield */
        {
          /* 0x3b38 */ unsigned char fitemload : 2; /* bit position: 0 */
          /* 0x3b38 */ unsigned char floadimage : 1; /* bit position: 2 */
          /* 0x3b38 */ unsigned char pad2 : 5; /* bit position: 3 */
        }; /* bitfield */
        /* 0x3b38 */ unsigned char m_Itemflag;
      }; /* size: 0x0001 */
      /* 0x3b39 */ char Padding_1063;
      /* 0x3b3a */ short m_OptionKeyStat[2];
      /* 0x3b3e */ short m_TempVolume[2];
      /* 0x3b42 */ short m_Volume_Real[2];
      /* 0x3b46 */ short m_Volume_Wariai[2];
      /* 0x3b4a */ short m_Volume_WariaiTMP[2];
      /* 0x3b4e */ short m_BikeTypeCounter;
      /* 0x3b50 */ bool m_OptionSettingFlag;
      /* 0x3b51 */ bool m_OptionSettingFlagOld;
      union
      {
        struct /* bitfield */
        {
          /* 0x3b52 */ unsigned char fste_mono : 2; /* bit position: 0 */
          /* 0x3b52 */ unsigned char fsoundtype : 2; /* bit position: 2 */
          /* 0x3b52 */ unsigned char pad3 : 4; /* bit position: 4 */
        }; /* bitfield */
        /* 0x3b52 */ unsigned char m_Optionflag;
      }; /* size: 0x0001 */
      /* 0x3b53 */ char Padding_1064;
      /* 0x3b54 */ class HrMap* m_pMap;
      /* 0x3b58 */ char m_MapScale;
      /* 0x3b59 */ char Padding_1065;
      /* 0x3b5a */ short m_MapInfoCounter;
      /* 0x3b5c */ char m_SelectWeapon;
      /* 0x3b5d */ char Padding_1066[3];
      /* 0x3b60 */ int m_WeaponWordLength;
      /* 0x3b64 */ char m_SelectSubjob[2];
      /* 0x3b66 */ char m_SubjobMode;
      /* 0x3b67 */ char m_WB_SubjobMode;
      /* 0x3b68 */ char m_InfoScrollVec;
      /* 0x3b69 */ char m_CancelAnimCounter;
      /* 0x3b6a */ char m_CancelCursol[2];
      /* 0x3b6c */ char m_CancelMode;
      /* 0x3b6d */ char Padding_1067[3];
      /* 0x3b70 */ unsigned int m_NonMissionMesHandl;
      /* 0x3b74 */ short m_CursorAnimCounter;
      /* 0x3b76 */ short m_SubjobCounter;
      /* 0x3b78 */ bool m_SubMissionFlag;
      /* 0x3b79 */ bool m_SMCantdrawFlag;
      /* 0x3b7a */ char Padding_1068[2];
      /* 0x3b7c */ int m_SMCantdrawCnt;
      /* 0x3b80 */ bool freturnSubJob;
      /* 0x3b81 */ char Padding_1069[3];
      /* 0x3b84 */ unsigned int m_SMC_YN_WordColor[2];
      /* 0x3b8c */ unsigned int m_SMC_YN_WakuColor[2];
      /* 0x3b94 */ char m_SelectLovikov[2];
      /* 0x3b96 */ char m_LovikovMode;
      /* 0x3b97 */ char Padding_1070;
      /* 0x3b98 */ short m_LovikovCounter;
      /* 0x3b9a */ char m_SelectOther[2];
      /* 0x3b9c */ char m_SelectOtherLoad[2];
      /* 0x3b9e */ char m_OtherMode;
      /* 0x3b9f */ char m_DemoStatus;
      /* 0x3ba0 */ bool m_NewGameFlag;
      /* 0x3ba1 */ char Padding_1071[3];
      /* 0x3ba4 */ unsigned int m_NewGameYesNoColor[2];
      /* 0x3bac */ char m_SelecNewGameYesNo[2];
      /* 0x3bae */ bool m_NewGameYesNoSelectFlag;
      /* 0x3baf */ unsigned char m_selectedOptionQuitGame;
      /* 0x3bb0 */ bool m_QuitGameYesNoSelectFlag;
      /* 0x3bb1 */ char m_SelectLoadData[2];
      /* 0x3bb3 */ char m_SelectLoadYesNo[2];
      /* 0x3bb5 */ unsigned char fcheckload;
      /* 0x3bb6 */ unsigned char m_YesNoWakuAlpha[2];
      /* 0x3bb8 */ unsigned int m_YesNoWakuColor[2];
      /* 0x3bc0 */ unsigned int m_YesNoWakuInnerColor[2];
      /* 0x3bc8 */ short m_OtherCounter;
      /* 0x3bca */ short m_LoadEndChkFlag[2];
      /* 0x3bce */ bool m_LoadEndFlag;
      /* 0x3bcf */ bool m_OtherLoadMode;
      /* 0x3bd0 */ bool freturnOther;
      /* 0x3bd1 */ bool m_DataLoadFlag;
      /* 0x3bd2 */ char Padding_1072[2];
      /* 0x3bd4 */ unsigned int m_LoadDataColor[5];
      /* 0x3be8 */ unsigned int m_LoadYesNoColor[2];
      /* 0x3bf0 */ unsigned char m_Alpha;
      /* 0x3bf1 */ char Padding_1073[3];
      /* 0x3bf4 */ struct tagHRTASKCHECK m_CheckStaffRoll;
      /* 0x3bfc */ int m_Soundid[2];
      /* 0x3c04 */ class ghmGcFont* m_pMessFont;
      /* 0x3c08 */ int m_Language;
      /* 0x3c0c */ int mainMenuSelectedMouseInteraction;
      /* 0x3c10 */ char pad_3c10[0xC]; // class MouseDeltaTracker mouseDeltaTracker; // edited
      /* 0x3c1c */ const int MouseInteractionsCount[11];
      /* 0x3c48 */ class NMH1MouseInteractionObject* MouseInteractions[11];
      /* 0x3c74 */ long Padding_1074;
      /* 0x3c78 */ char pad_3c78[0x1DCE]; // class NMH1MouseInteractionObject miMain[8];                 // edited
      /* 0x4138 */ // class NMH1MouseInteractionObject miMap[3];                  // edited
      /* 0x4300 */ // class NMH1MouseInteractionObject miRanking[1];              // edited
      /* 0x4398 */ // class NMH1MouseInteractionObject miSide_Job[5];             // edited
      /* 0x4690 */ // class NMH1MouseInteractionObject miSide_Job_WorkDetails[1]; // edited
      /* 0x4728 */ // class NMH1MouseInteractionObject miSide_Job_Cancel_1[4];    // edited
      /* 0x4988 */ // class NMH1MouseInteractionObject miSide_Job_Cancel_2[4];    // edited
      /* 0x4be8 */ // class NMH1MouseInteractionObject miConfig[8];               // edited
      /* 0x50a8 */ // class NMH1MouseInteractionObject miData[8];                 // edited
      /* 0x5568 */ // class NMH1MouseInteractionObject miData_Load[9];            // edited
    }; /* size: 0x5a46 */
  // }; /* size: 0x5a46 */
}; /* size: 0x5ac0 */


class HrScreenStatus
{
public: // edited
  /* 0x0000 */ //long Padding_1031[20]; // edited
  /* 0x0050 */ //enum D_CLASS_STAT m_Stat; // edited
  //union { // edited
    /* 0x0000 */ // struct HrScreenStatus::UNION uni; // edited
    /* 0x0000 */ // char** ParamName /* zero-length array */; // edited
    struct
    {
      /* 0x0000 */ //int* Param /* zero-length array */;
      /* 0x0001 */ char Padding_0[0x54]; // edited
      /* 0x0054 */ /*class HrMap m_Map;*/ char padding_54[0x3a5dc];
      /* 0x3a630 */ class HrMissionResult* m_pMissionResult;
      /* 0x3a634 */ struct tagHRTASKCHECK m_CheckMissionResult;
      /* 0x3a63c */ class HrInGameMenu* m_pInGameMenu;
      /* 0x3a640 */ struct tagHRTASKCHECK m_CheckInGameMenu;
      /* 0x3a648 */ class HrKeyGuide* m_pKeyGuide;
      /* 0x3a64c */ struct tagHRTASKCHECK m_CheckKeyGuide;
      /* 0x3a654 */ int m_SoundID[3];
      /* 0x3a660 */ int m_SprLchSoundID;
      /* 0x3a664 */ short m_Herose;
      /* 0x3a666 */ char Padding_1033[2];
      /* 0x3a668 */ float m_HP;
      /* 0x3a66c */ float m_Battery;
      /* 0x3a670 */ float m_Stamina;
      /* 0x3a674 */ float m_Tension[2];
      /* 0x3a67c */ int m_Money[2];
      /* 0x3a684 */ int m_MoneyCompSoundWait;
      /* 0x3a688 */ int m_FlagAnimCounter;
      /* 0x3a68c */ int m_GearAnimCounter;
      /* 0x3a690 */ int m_GearAnimFrame[2];
      /* 0x3a698 */ int m_GearFadeCnt;
      /* 0x3a69c */ int m_GearMaxAnimFrame;
      /* 0x3a6a0 */ short m_GearRandCounter[2];
      /* 0x3a6a4 */ short m_GearRandFlag;
      /* 0x3a6a6 */ short m_Gear;
      /* 0x3a6a8 */ short m_Counter[18];
      /* 0x3a6cc */ short m_MoneyAppearCounter;
      /* 0x3a6ce */ char Padding_1034[2];
      /* 0x3a6d0 */ float m_MoneySpeed;
      /* 0x3a6d4 */ float m_MoneyDeltaSpeed;
      /* 0x3a6d8 */ short m_DelayCounter[18];
      /* 0x3a6fc */ int m_GlobalCounter;
      /* 0x3a700 */ short m_Counter_TigerGoal;
      /* 0x3a702 */ short m_ChangeCounter;
      /* 0x3a704 */ short m_HpAnimCounter;
      /* 0x3a706 */ char Padding_1035[2];
      /* 0x3a708 */ float m_HpScale;
      /* 0x3a70c */ short m_BikeInOutCounter;
      /* 0x3a70e */ bool m_Touring2NaturalFlag;
      /* 0x3a70f */ bool m_Battle2NaturalFlag;
      /* 0x3a710 */ bool m_Natural2BattleFlag;
      /* 0x3a711 */ char Padding_1036[3];
      /* 0x3a714 */ int m_LoadMesStat; // edited
      /* 0x3a718 */ void* m_pMesRSL;
      /* 0x3a71c */ int m_LogId[2][10];
      /* 0x3a76c */ int m_LogCounter[10];
      /* 0x3a794 */ short m_LogLineCounter;
      /* 0x3a796 */ short m_Log_W[3];
      /* 0x3a79c */ bool m_Log_tenmetuFlag[10];
      /* 0x3a7a6 */ char m_Log_tenmetuCnt[32][10];
      /* 0x3a8e6 */ char m_Log_tenmetuTmp[32][10];
      /* 0x3aa26 */ char m_Log_tenmetuNowProgress[10];
      /* 0x3aa30 */ unsigned char m_Log_alpha[10];
      /* 0x3aa3a */ char m_ComboNum[2];
      /* 0x3aa3c */ int m_ComboCounter;
      /* 0x3aa40 */ int m_ComboDelayCounter;
      /* 0x3aa44 */ char m_SlotDan;
      /* 0x3aa45 */ char m_SlotDeme[3][3];
      /* 0x3aa4e */ short m_SlotDemeCounter[3][3];
      /* 0x3aa60 */ short m_SlotCounter;
      /* 0x3aa62 */ bool m_PlayZoroSound;
      /* 0x3aa63 */ bool m_LeachHazure;
      /* 0x3aa64 */ bool m_PlayCherryZoro;
      /* 0x3aa65 */ bool m_PlayLeachSound;
      /* 0x3aa66 */ char Padding_1037[2];
      /* 0x3aa68 */ int m_SlotZoromeEnshutuCounter;
      /* 0x3aa6c */ bool m_SlotZoromeEnshutuFlag;
      /* 0x3aa6d */ bool m_SlotSPAttackStartFlag;
      /* 0x3aa6e */ unsigned char m_SlotZoromeAlpha;
      /* 0x3aa6f */ unsigned char m_SlotZoromeFlashAlpha;
      /* 0x3aa70 */ unsigned char m_SlotZoroBackAlpha;
      /* 0x3aa71 */ char Padding_1038;
      /* 0x3aa72 */ short m_SlotZoromeZOROME;
      /* 0x3aa74 */ bool m_bZoromeSorotta;
      /* 0x3aa75 */ char Padding_1039[3];
      /* 0x3aa78 */ int m_SlotZoroBackCnt;
      /* 0x3aa7c */ int m_SlotZoroLeaveWaitCnt;
      /* 0x3aa80 */ bool m_SlotZoromeStartToEndFlag;
      /* 0x3aa81 */ char Padding_1040[3];
      /* 0x3aa84 */ char pad_3aa84[0x10][27]; // struct HrScreenStatus::HEART_BIT m_DefHeartBit[27]; // edited
      /* 0x3ac34 */ char pad_3ac34[0x10][27]; // struct HrScreenStatus::HEART_BIT m_AmpHeartBit[27]; // edited
      /* 0x3ade4 */ bool m_BlueHeartFlag;
      /* 0x3ade5 */ char Padding_1041;
      union
      {
        struct /* bitfield */
        {
          /* 0x3ade6 */ unsigned short lmode_seven : 1; /* bit position: 0 */
          /* 0x3ade6 */ unsigned short lmode_melon : 1; /* bit position: 1 */
          /* 0x3ade6 */ unsigned short lmode_bel : 1; /* bit position: 2 */
          /* 0x3ade6 */ unsigned short lmode_cherry : 1; /* bit position: 3 */
          /* 0x3ade6 */ unsigned short lmode_ber : 1; /* bit position: 4 */
          /* 0x3ade6 */ unsigned short lmode_hopperman : 1; /* bit position: 5 */
          /* 0x3ade6 */ unsigned short pad3 : 10; /* bit position: 6 */
        }; /* bitfield */
        /* 0x3ade6 */ unsigned short flaglmode;
      }; /* size: 0x0002 */
      /* 0x3ade8 */ unsigned char m_KeyGuideButtonID[8];
      /* 0x3adf0 */ int m_SMMesPosX;
      /* 0x3adf4 */ int m_SMMesPosY;
      /* 0x3adf8 */ short m_TigerProc;
      /* 0x3adfa */ short m_TigerCounter;
      /* 0x3adfc */ short m_TigerTexId;
      /* 0x3adfe */ short m_FireTexId;
      /* 0x3ae00 */ float m_TigerXpos;
      /* 0x3ae04 */ int m_WalkStartTick;
      /* 0x3ae08 */ short m_TigerGoalLength;
      /* 0x3ae0a */ bool m_TigerSPReturnFlag;
      /* 0x3ae0b */ char Padding_1042;
      union
      {
        struct /* bitfield */
        {
          /* 0x3ae0c */ unsigned short drawbpber : 1; /* bit position: 0 */
          /* 0x3ae0c */ unsigned short drawhber : 1; /* bit position: 1 */
          /* 0x3ae0c */ unsigned short drawbattery : 1; /* bit position: 2 */
          /* 0x3ae0c */ unsigned short drawmoney : 1; /* bit position: 3 */
          /* 0x3ae0c */ unsigned short drawtension : 1; /* bit position: 4 */
          /* 0x3ae0c */ unsigned short drawkamae : 1; /* bit position: 5 */
          /* 0x3ae0c */ unsigned short drawspeedmeter : 1; /* bit position: 6 */
          /* 0x3ae0c */ unsigned short drawmap : 1; /* bit position: 7 */
          /* 0x3ae0c */ unsigned short drawsilvia : 1; /* bit position: 8 */
          /* 0x3ae0c */ unsigned short drawlmode : 1; /* bit position: 9 */
          /* 0x3ae0c */ unsigned short drawkeyguide : 1; /* bit position: 10 */
          /* 0x3ae0c */ unsigned short drawcheckpo : 1; /* bit position: 11 */
          /* 0x3ae0c */ unsigned short pad : 4; /* bit position: 12 */
        }; /* bitfield */
        /* 0x3ae0c */ unsigned short flag;
      }; /* size: 0x0002 */
      union
      {
        struct /* bitfield */
        {
          /* 0x3ae0e */ unsigned short reqlmodedel : 1; /* bit position: 0 */
          /* 0x3ae0e */ unsigned short tigerdirect : 1; /* bit position: 1 */
          /* 0x3ae0e */ unsigned short tigerfire : 2; /* bit position: 2 */
          /* 0x3ae0e */ unsigned short pad2 : 12; /* bit position: 4 */
        }; /* bitfield */
        /* 0x3ae0e */ unsigned short flag2;
      }; /* size: 0x0002 */
      union
      {
        struct
        {
          struct /* bitfield */
          {
            /* 0x3ae10 */ unsigned char checkpo_Checkpoint : 1; /* bit position: 0 */
            /* 0x3ae10 */ unsigned char checkpo_Standby : 1; /* bit position: 1 */
            /* 0x3ae10 */ unsigned char checkpo_Zenmetu : 1; /* bit position: 2 */
            /* 0x3ae10 */ unsigned char checkpo_CantRide : 1; /* bit position: 3 */
            /* 0x3ae10 */ unsigned char checkpo_Phone : 1; /* bit position: 4 */
            /* 0x3ae10 */ unsigned char checkpo_NewMask : 1; /* bit position: 5 */
            /* 0x3ae10 */ unsigned char checkpo_Senrihin : 1; /* bit position: 6 */
          }; /* bitfield */
          /* 0x3ae11 */ // unsigned short pad4; /* bit position: 0 */
        }; /* size: 0x0002 */
        /* 0x3ae10 */ unsigned short flag4;
      }; /* size: 0x0002 */
      /* 0x3ae12 */ short m_BatteyArram;
      /* 0x3ae14 */ int m_AlarmSound;
      /* 0x3ae18 */ int m_DrawType; // HrScreenStatus::STAT_DRAW_TYPE m_DrawType; // edited
      /* 0x3ae1c */ int m_DrawTypeOld; // HrScreenStatus::STAT_DRAW_TYPE m_DrawTypeOld; // edited
      /* 0x3ae20 */ bool m_ChargeMaxFlag;
      /* 0x3ae21 */ bool m_ChargeMaxFlag2;
      /* 0x3ae22 */ bool m_ChargeSEStopFlag;
      /* 0x3ae23 */ bool m_BusFightFlag;
      /* 0x3ae24 */ int m_ChangeMinimapIndex;
      /* 0x3ae28 */ bool m_Flag_BatterySE;
      /* 0x3ae29 */ bool m_Flag_StatusDataLoad;
      /* 0x3ae2a */ bool m_Flag_StatusNewGame;
      /* 0x3ae2b */ bool m_StatusProcFlag;
      /* 0x3ae2c */ bool m_fPC_BatteryCharge[2];
    }; /* size: 0x3adc7 */
  //}; /* size: 0x3adc7 */ // edited
}; /* size: 0x3ae30 */

struct GXColor
{
  union
  {
    struct
    {
      /* 0x0000 */ unsigned char r;
      /* 0x0001 */ unsigned char g;
      /* 0x0002 */ unsigned char b;
      /* 0x0003 */ unsigned char a;
    }; /* size: 0x0004 */
    /* 0x0000 */ unsigned int PackedValue;
  }; /* size: 0x0004 */
}; /* size: 0x0004 */

class CBgCtrl
{
public:
enum CBGCTRL_STATUS
{
  CBGCTRL_STATUS_NO_DATA = 0,
  CBGCTRL_STATUS_LOAD = 1,
  CBGCTRL_STATUS_INIT = 2,
  CBGCTRL_STATUS_IDLE = 3,
  CBGCTRL_STATUS_RELEASESETUP = 4,
  CBGCTRL_STATUS_NPCRELEASE = 5,
  CBGCTRL_STATUS_RELEASE = 6,
  CBGCTRL_STATUS_SNDGROUPRELEASE = 7,
  CBGCTRL_STATUS_TESTDEMO = 8,
  CBGCTRL_STATUS_DEMOSETUP = 9,
  CBGCTRL_STATUS_DEMOPLAY = 10,
  CBGCTRL_STATUS_BOSSRESULTWAIT = 11,
  CBGCTRL_STATUS_STAFF1WAIT = 12,
  CBGCTRL_STATUS_STAFF2WAIT = 13,
  CBGCTRL_STATUS_SHOOTINGWAIT = 14,
  CBGCTRL_STATUS_CHANGESTAGE = 15,
  CBGCTRL_STATUS_NPCRSLLOAD = 16,
  CBGCTRL_STATUS_MAX = 17,
};
enum CBGCTRL_LOADSTATUS
{
  CBGCTRL_LOADSTATUS_SETUP = 0,
  CBGCTRL_LOADSTATUS_LOADING = 1,
  CBGCTRL_LOADSTATUS_SCR_POOL = 2,
  CBGCTRL_LOADSTATUS_SUBMISSIONSTATICSOUND_SET = 3,
  CBGCTRL_LOADSTATUS_SUBMISSIONSTATICSOUND_POOL = 4,
  CBGCTRL_LOADSTATUS_RESULTSOUND_SET = 5,
  CBGCTRL_LOADSTATUS_RESULTSOUND_POOL = 6,
  CBGCTRL_LOADSTATUS_SUBMISSIONSOUND_SET = 7,
  CBGCTRL_LOADSTATUS_SUBMISSIONSOUND_POOL = 8,
  CBGCTRL_LOADSTATUS_SOUND_SET = 9,
  CBGCTRL_LOADSTATUS_SOUND_POOL = 10,
  CBGCTRL_LOADSTATUS_SUB_SOUND_SET = 11,
  CBGCTRL_LOADSTATUS_SUB_SOUND_POOL = 12,
  CBGCTRL_LOADSTATUS_ENB_SOUND_SET = 13,
  CBGCTRL_LOADSTATUS_ENB_SOUND_POOL = 14,
  CBGCTRL_LOADSTATUS_SOUND_SETOK = 15,
  CBGCTRL_LOADSTATUS_READY = 16,
  CBGCTRL_LOADSTATUS_REQUEST = 17,
  CBGCTRL_LOADSTATUS_NPCLOADREQUEST = 18,
  CBGCTRL_LOADSTATUS_NPCLOADING = 19,
  CBGCTRL_LOADSTATUS_MAX = 20,
};

  /* 0x0000 */ int Padding_1372; // edited
  /* 0x0004 */ class ghmGcFile* m_pFile;
  union
  {
    struct /* bitfield */
    {
      /* 0x0008 */ unsigned int m_DISABLEUPDATE : 1; /* bit position: 0 */
      /* 0x0008 */ unsigned int m_VISIBLE : 1; /* bit position: 1 */
      /* 0x0008 */ unsigned int m_NPCRSLREAD : 1; /* bit position: 2 */
      /* 0x0008 */ unsigned int pad : 29; /* bit position: 3 */
    }; /* bitfield */
    /* 0x0008 */ unsigned int m_Flag;
  }; /* size: 0x0004 */
  /* 0x000c */ unsigned int m_BgScrHndl;
  /* 0x0010 */ unsigned int m_MesHndl;
  /* 0x0014 */ unsigned int m_SpecialStgIdx;
  /* 0x0018 */ int m_CurrentBgX;
  /* 0x001c */ int m_CurrentBgY;
  /* 0x0020 */ class TGmf* mpAirGmf;
  /* 0x0024 */ class TGmf* mpReceiveShadowGmf;
  /* 0x0028 */ class HrStageDraw* mp_StageDraw;
  /* 0x002c */ struct Vec m_CamPos;
  /* 0x0038 */ bool m_ResultTex;
  /* 0x0039 */ char Padding_1373[3];
  /* 0x003c */ unsigned int m_ResultTexAlpha;
  /* 0x0040 */ char pad_0x40[0xa30]; // class ghmGcFont m_ResultFont; // size = 0x0a30 // edited
  /* 0x0a70 */ unsigned int m_ResultFontColor[2];
  /* 0x0a78 */ int m_ResultFontTenmetuNum;
  /* 0x0a7c */ int m_ResultFontDispTick;
  /* 0x0a80 */ int m_ResultFontDispMaxTick;
  /* 0x0a84 */ int m_ResultCmnTick;
  /* 0x0a88 */ class WAnimF m_ResultPauseTileAlpha;
  /* 0x0aa8 */ short m_DarkSideModeColor;
  /* 0x0aaa */ bool m_DarkStageReq;
  /* 0x0aab */ char Padding_1374;
  /* 0x0aac */ struct STAGE_INFO* m_Stage_Info;
  /* 0x0ab0 */ char m_NowStageName[32];
  /* 0x0ad0 */ char m_RequestStageName[32];
  /* 0x0af0 */ char m_NpcRslName[32];
  /* 0x0b10 */ class ghmResGroup* m_pRsl;
  /* 0x0b14 */ void* m_pRslData;
  /* 0x0b18 */ bool m_IsPressData;
  /* 0x0b19 */ char Padding_1375[3];
  /* 0x0b1c */ class ghmResGroup* mpNpcRsl;
  /* 0x0b20 */ void* mpNpcRslData;
  /* 0x0b24 */ bool m_NpcVisible;
  /* 0x0b25 */ char Padding_1376[3];
  /* 0x0b28 */ int mPathCounter;
  /* 0x0b2c */ class hPathManager* mp_CPathManager;
  /* 0x0b30 */ class hPathManager* mp_MPathManager;
  /* 0x0b34 */ class hNPCManager* mp_NpcManager;
  /* 0x0b38 */ class hNpcInvisible* mp_NpcPlayer;
  /* 0x0b3c */ class hNpcBike* mp_NpcBike;
  /* 0x0b40 */ class hNpcCat* mp_NpcCat;
  /* 0x0b44 */ class hNpcObjMan* mp_NpcObjMan;
  /* 0x0b48 */ class TGmf* mp_NpcPhone;
  /* 0x0b4c */ int m_CallBikeState;
  /* 0x0b50 */ int m_CallBikeCnt;
  /* 0x0b54 */ float m_CallBikeDist;
  /* 0x0b58 */ int m_CallBikeSndHandle;
  /* 0x0b5c */ int m_CallBikeSndHandle2;
  /* 0x0b60 */ int m_CallBikeTailEndNum;
  /* 0x0b64 */ bool m_BikeHitCarFlag;
  /* 0x0b65 */ bool m_RankingUpGo;
  /* 0x0b66 */ bool m_BossResultGo;
  /* 0x0b67 */ char Padding_1377;
  /* 0x0b68 */ int m_BossResultAfterDemoNo;
  /* 0x0b6c */ bool m_StaffRoll1Go;
  /* 0x0b6d */ bool m_StaffRoll2Go;
  /* 0x0b6e */ bool m_ShootingGo;
  /* 0x0b6f */ char Padding_1378;
  /* 0x0b70 */ int m_TutoResultProc;
  /* 0x0b74 */ int m_TutoResultType;
  /* 0x0b78 */ enum _GXFogType m_FogType;
  /* 0x0b7c */ struct GXColor m_FogColor;
  /* 0x0b80 */ float m_FogNear;
  /* 0x0b84 */ float m_FogFar;
  /* 0x0b88 */ unsigned int m_DenCount;
  /* 0x0b8c */ bool m_EventPaintData;
  /* 0x0b8d */ char Padding_1379[0x3];
  /* 0x0b90 */ char pad_0xb90[0x6C];// class WGcl m_CamGcl; // size = 0x6C // edited
  /* 0x0bfc */ char pad_bfc[0x30];//class ghmGcCollObjHitResult m_CamColResult; // edited
  /* 0x0c2c */ char pad_c2c[0x30];//class ghmGcCollObjHitResult m_CamColResultBak; // edited
  /* 0x0c5c */ struct WGclMaterialSpec* mp_Material;
  /* 0x0c60 */ int m_CamStatus;
  /* 0x0c64 */ int m_CamStatusBak;
  /* 0x0c68 */ float m_CamFov;
  /* 0x0c6c */ int m_CamWait;
  /* 0x0c70 */ bool m_Charge;
  /* 0x0c71 */ char Padding_1380[3];
  /* 0x0c74 */ struct Vec m_CamHOKAN_c;
  /* 0x0c80 */ struct Vec m_CamHOKAN_t;
  /* 0x0c8c */ float m_CamHOKANcount;
  /* 0x0c90 */ int m_LocID;
  /* 0x0c94 */ enum HRCAMERA_MODE m_CamModeBack;
  /* 0x0c98 */ int m_ScriptArg;
  /* 0x0c9c */ int m_ScriptMainScenarioID;
  /* 0x0ca0 */ int m_ScriptSubMissionID;
  /* 0x0ca4 */ int m_SubMissionSndGroupID;
  /* 0x0ca8 */ int m_StageSndGroupID;
  /* 0x0cac */ int m_StageSubSndGroupID;
  /* 0x0cb0 */ int m_StageGayaID;
  /* 0x0cb4 */ bool m_bChanged;
  /* 0x0cb5 */ char Padding_1381[3];
  /* 0x0cb8 */ enum CBgCtrl::CBGCTRL_STATUS m_Status;
  /* 0x0cbc */ enum CBgCtrl::CBGCTRL_LOADSTATUS m_LDstatus;
  /* 0x0cc0 */ bool m_RequestChangeStage;
  /* 0x0cc1 */ bool m_DisEnableLoadingEffect;
  /* 0x0cc2 */ bool m_BgRenderOK;
  /* 0x0cc3 */ char Padding_1382;
  /* 0x0cc4 */ int mReleaseWaitCount;
}; /* size: 0x0cc8 */

struct LOADING
{
  /* 0x0000 */ bool mFadeOnly;
  /* 0x0001 */ bool mVisible;
  /* 0x0002 */ unsigned char mRunType;
}; /* size: 0x0003 */

struct GAMESAVERESOURCE
{
  /* 0x0000 */ class ghmGcFile* file;
  /* 0x0004 */ void* pData;
  /* 0x0008 */ class ghmResGroup* pRsl;
}; /* size: 0x000c */

class WAnimLinearF
{
  /* 0x0000 */ int Padding_119; // edited
  /* 0x0004 */ float mCurValue;
  /* 0x0008 */ float mDstValue;
  /* 0x000c */ float mSrcValue;
  /* 0x0010 */ unsigned int mCount;
  /* 0x0014 */ unsigned int mTime;
}; /* size: 0x0018 */

class HsMoveBase
{
  /* 0x0000 */ int m_Move_Cnt;
  /* 0x0004 */ bool m_Move_f;
  /* 0x0005 */ char Padding_912[3];
  /* 0x0008 */ int m_HiSpd_Cnt;
  /* 0x000c */ bool m_HiSpd_f;
  /* 0x000d */ char Padding_913[3];
  /* 0x0010 */ enum HS_INPUT_VEC m_InputVec[2];
}; /* size: 0x0018 */

class NMH1MouseInteractionObject
{
    char pad_00[0x98]; // edited
}; /* size: 0x0098 */

class MouseDeltaTracker
{
  /* 0x0000 */ int _oldMousePointerPosition[2]; // edited
  /* 0x0008 */ float _sqrMouseDelta;
}; /* size: 0x000c */

enum HS_CLASS_TASK
{
  HS_CLASS_TASK_NON = -1,
  HS_CLASS_TASK_INIT = 0,
  HS_CLASS_TASK_LODING = 1,
  HS_CLASS_TASK_IDLE = 2,
  HS_CLASS_TASK_RELEASE = 3,
  HS_CLASS_TASK_END = 4,
};

class hsSourceBase
{
public:
    enum MouseInteractionType
    {
      None = 0,
      Main = 1,
      Map = 2,
      Ranking = 3,
      Side_Job = 4,
      Side_Job_WorkDetails = 5,
      Side_Job_Cancel_1 = 6,
      Side_Job_Cancel_2 = 7,
      Config = 8,
      Data = 9,
      Data_Load = 10,
      Num = 11,
    };
  /* 0x0000 */ long Padding_1229[2];
  /* 0x0008 */ bool m_Access_f;
  /* 0x0009 */ char Padding_1230[3];
  /* 0x000c */ unsigned int m_Access_No;
  /* 0x0010 */ enum HS_CLASS_TASK m_Task;
  /* 0x0014 */ enum HS_PROC_LOAD_STATE m_hsLoadState;
  /* 0x0018 */ enum HS_FILE_LOAD_STATE m_hsfileState;
  /* 0x001c */ unsigned short m_SaveSound_No;
  /* 0x001e */ char Padding_1231[2];
  /* 0x0020 */ float m_StartMotion_frame_Num;
  /* 0x0024 */ enum HS_MODEL_ALPHA_TYPE m_Model_Alpha_Type;
  /* 0x0028 */ int m_Model_Alpha_Num;
  /* 0x002c */ int m_Alpha_1F_Num;
  /* 0x0030 */ int m_Info_x;
  /* 0x0034 */ float m_InfoLength;
  /* 0x0038 */ float m_Most_ItemName_Length;
  /* 0x003c */ int m_CurNum[2];
  /* 0x0044 */ class HsMoveBase m_HsMoveBase;
  /* 0x005c */ unsigned int m_Cnt;
  /* 0x0060 */ unsigned int m_Cnt2;
  /* 0x0064 */ class TGmf* m_pGmf;
  /* 0x0068 */ bool updatesMouseSelf;
  /* 0x0069 */ char Padding_1232[3];
  /* 0x006c */ enum MouseInteractionType chosenMouseInteraction;
  /* 0x0070 */ class NMH1MouseInteractionObject* const MouseInteractions[12];
  /* 0x00a0 */ class NMH1MouseInteractionObject* const SubMenuInteractions[12];
  /* 0x00d0 */ const int MouseInteractionCounts[12];
  /* 0x0100 */ const int SubMenuMouseInteractionCounts[12];
  /* 0x0130 */ class NMH1MouseInteractionObject mi_Cat[1];
  /* 0x01c8 */ class NMH1MouseInteractionObject mi_Closet[11];
  /* 0x0850 */ class NMH1MouseInteractionObject mi_Drawer[8];
  /* 0x0d10 */ class NMH1MouseInteractionObject mi_TV[10];
  /* 0x1300 */ class NMH1MouseInteractionObject mi_Naomi[7];
  /* 0x1728 */ class NMH1MouseInteractionObject mi_Gym[6];
  /* 0x1ab8 */ class NMH1MouseInteractionObject mi_VideoStore[6];
  /* 0x1e48 */ class NMH1MouseInteractionObject mi_Lovikov[6];
  /* 0x21d8 */ class NMH1MouseInteractionObject mi_Closet_Sub[15];
  /* 0x2ac0 */ class NMH1MouseInteractionObject mi_Naomi_Sub[7];
  /* 0x2ee8 */ class NMH1MouseInteractionObject mi_Gym_Sub[3];
  /* 0x30b0 */ class NMH1MouseInteractionObject mi_VideoStore_Sub[4];
  /* 0x3310 */ class NMH1MouseInteractionObject mi_Lovikov_Sub[7];
  /* 0x3738 */ class MouseDeltaTracker mouseDeltaTracker;
}; /* size: 0x3748 */

struct WEAPONCOUNT
{
    /* 0x0000 */ int m_WeaponCounter;
    /* 0x0004 */ int m_ID;
}; /* size: 0x0008 */

class HrMenuTask
{
public:
  /* 0x0000 */ class HrMenuTask* m_pHrMenuTask;
  /* 0x0004 */ char Padding_1228[0x4C];
  /* 0x0050 */ class hsSourceBase* m_pHsMenu;
  /* 0x0054 */ class HrEmployment* m_pdEmployment;
  /* 0x0058 */ class HrK_Entertainment* m_pdK_Entertainment;
  /* 0x005c */ struct WEAPONCOUNT m_Weapon;
  /* 0x0064 */ int m_Dag_Counter;
  /* 0x0068 */ bool m_Dag_LockFlag;
}; /* size: 0x006c */

struct HRSAVEDATA_SAVEHEADER
{
  /* 0x0000 */ unsigned char t_SaveBit;
  /* 0x0001 */ unsigned char t_SaveDataVer;
  /* 0x0002 */ char t_Header[4];
}; /* size: 0x0006 */

struct HRSAVEDATA_EVENTFLAG
{
  /* 0x0000 */ unsigned char t_Flag[128];
}; /* size: 0x0080 */

struct HRSAVEDATA_SM
{
  /* 0x0000 */ int mResultNum;
  /* 0x0004 */ char mTopRanking;
  /* 0x0005 */ char mNew;
  union
  {
    struct /* bitfield */
    {
      /* 0x0006 */ unsigned char mVisible : 1; /* bit position: 0 */
      /* 0x0006 */ unsigned char mAccept : 1; /* bit position: 1 */
    }; /* bitfield */
    /* 0x0006 */ unsigned char mFlag;
  }; /* size: 0x0001 */
  /* 0x0007 */ char Padding_1226;
  /* 0x0008 */ int mPopTime;
}; /* size: 0x000c */

struct HRSTG_SCORE
{
  /* 0x0000 */ int t_Score;
  /* 0x0004 */ char t_Name[4];
}; /* size: 0x0008 */

struct HRSAVEDATA_STG
{
  /* 0x0000 */ struct HRSTG_SCORE t_Score[10];
}; /* size: 0x0050 */

class hBjObjectManager
{
  /* 0x0000 */ long Padding_1215;
  /* 0x0004 */ class hBjObject* mTop;
  /* 0x0008 */ class hBjObject* mEnd;
  /* 0x000c */ unsigned int mTexNum;
  /* 0x0010 */ struct tagGHMR_TEX* mpTex;
  /* 0x0014 */ class ghmResGroup* mpRes;
}; /* size: 0x0018 */

struct bjStar
{
  /* 0x0000 */ struct Vec mPos;
  /* 0x000c */ float mSpeed;
  /* 0x0010 */ float mSize;
  /* 0x0014 */ unsigned char mR;
  /* 0x0015 */ unsigned char mG;
  /* 0x0016 */ unsigned char mB;
  /* 0x0017 */ unsigned char mA;
}; /* size: 0x0018 */

class hBjStage
{
  /* 0x0000 */ long Padding_1216;
  /* 0x0004 */ class hBjObjectManager* mManager;
  /* 0x0008 */ class ghmResGroup* mpRes;
  /* 0x000c */ struct tagGHMR_TEX* mpTex;
  /* 0x0010 */ class hBjSys* mpSys;
  /* 0x0014 */ int mStageID;
  /* 0x0018 */ int mState;
  /* 0x001c */ int mCount;
  /* 0x0020 */ int mTableCount;
  /* 0x0024 */ int mWarnCount;
  /* 0x0028 */ int mWarnState;
  /* 0x002c */ int* mpBGMHandle;
  /* 0x0030 */ int mTableCnt;
  /* 0x0034 */ int mEnemyOutCount;
  /* 0x0038 */ int mEnemyDeadCount;
  /* 0x003c */ struct bjStar mBgStar[50];
}; /* size: 0x04ec */

class hBjSys
{
  union
  {
    /* 0x0000 */ int mGlovalScore;
    /* 0x0000 */ int mGlovalBomb;
    struct
    {
      /* 0x0000 */ unsigned int mGlovalColor;
      /* 0x0004 */ int mState;
      /* 0x0008 */ int mSubState;
      /* 0x000c */ int mCounter;
      /* 0x0010 */ int mSubCounter;
    }; /* size: 0x0014 */
    /* 0x0000 */ int mScore;
    /* 0x0000 */ int mHighScore;
    /* 0x0000 */ int mPlayerNum;
    /* 0x0000 */ int mBombGauge;
    /* 0x0000 */ int mBombGaugeMax;
    struct
    {
      /* 0x0000 */ int mBombGaugeFlash;
      /* 0x0004 */ long Padding_1217[4];
      /* 0x0014 */ int mCursor;
      /* 0x0018 */ int mFadeCount;
      /* 0x001c */ int mFadeDest;
      /* 0x0020 */ int mFadeFlag;
    }; /* size: 0x0014 */
    /* 0x0000 */ bool mGameClearFlag;
    /* 0x0000 */ bool mBossDeadFlag;
    /* 0x0000 */ int mGameClearState;
    /* 0x0000 */ int mGameOverState;
    struct
    {
      /* 0x0000 */ int mContinueCount;
      /* 0x0004 */ long Padding_1218[8];
      /* 0x0024 */ int mGameClearCnt;
      /* 0x0028 */ int mDeadWaitCnt;
      /* 0x002c */ int mGameMode;
      /* 0x0030 */ class ghmResGroup* pResource;
      /* 0x0034 */ class hBjObjectManager mObjManager;
      /* 0x004c */ class hBjPlayer* mPlayer;
      /* 0x0050 */ class hBjStage mStage;
      /* 0x053c */ unsigned int mGanNum;
      /* 0x0540 */ class TGan** mpGan;
    }; /* size: 0x0524 */
    struct
    {
      /* 0x0000 */ struct tagGHMR_TEX* mTexture;
      /* 0x0004 */ long Padding_1219[336];
      /* 0x0544 */ unsigned int mTexNum;
    }; /* size: 0x0008 */
    /* 0x0000 */ bool mbPause;
    /* 0x0000 */ bool mbGameOver;
  }; /* size: 0x0524 */
  /* 0x0524 */ long Padding_1220[9];
  /* 0x0548 */ int mBGMHandle;
  /* 0x054c */ struct HRSAVEDATA_STG mHiScore;
  /* 0x059c */ int mChangeNum;
  /* 0x05a0 */ int mSelNum;
  /* 0x05a4 */ int mPadDataNum;
  /* 0x05a8 */ int mPadDataCnt;
  /* 0x05ac */ struct BjPadData* mpPadData;
  /* 0x05b0 */ void* mpKey;
}; /* size: 0x05b4 */

struct tagHRSAVEDATA
{
  /* 0x0000 */ struct HRSAVEDATA_SAVEHEADER t_Header;
  /* 0x0006 */ char Padding_1227[2];
  /* 0x0008 */ int t_MainScenarioID;
  /* 0x000c */ int t_StageID;
  /* 0x0010 */ __int64 t_Time;
  /* 0x0018 */ __int64 t_ScenarioTime;
  /* 0x0020 */ struct Vec t_SavePosition;
  /* 0x002c */ struct HRSAVEDATA_EVENTFLAG t_Event;
  /* 0x00ac */ struct stPcSaveData t_PcData;
  /* 0x1204 */ struct HRSAVEDATA_SM t_SMResult[56];
  /* 0x14a4 */ int t_KenterTixNum;
  /* 0x14a8 */ int t_GetTopRankingNum;
  /* 0x14ac */ int t_SEVol;
  /* 0x14b0 */ int t_BGMVol;
  /* 0x14b4 */ int t_Difficult;
  /* 0x14b8 */ struct HRSAVEDATA_STG t_StgData;
  /* 0x1508 */ int t_Subtitles;
  /* 0x150c */ int t_dummy;
  /* 0x1510 */ int t_CamYAxis;
  /* 0x1514 */ int t_CamXAxis;
  /* 0x1518 */ int t_Trainig_B_Num;
  /* 0x151c */ short t_Trainig_S_Num;
  /* 0x151e */ short t_VisitedToilets;
  /* 0x1520 */ int t_KaihatutyuuWeaponCounter;
  /* 0x1524 */ int t_KaihatutyuuWeaponID;
}; /* size: 0x1528 */

class HrGameTask
{
public:
    enum HR_LOAD_STS
    {
        HR_LOAD_NONE = 0,
        HR_LOAD_NEW = 1,
        HR_LOAD_CONTINUE = 2,
        HR_LOAD_CHECKPOINT = 3,
        HR_LOAD_RETURNHOME = 4,
        HR_LOAD_DEBUG = 5,
        HR_LOAD_ALLCLEAR = 6,
        HR_LOAD_MAX = 7,
    };

    enum HRGAME_ID
    {
      HR_INIT_ID = 0,
      HR_OPMOVIE_ID = 1,
      HR_TITLE_ID = 2,
      HR_LOAD_ID = 3,
      HR_OPTION_ID = 4,
      HR_GAME_ID = 5,
      HR_SUBMENU_ID = 6,
      HR_GAMEOVER_ID = 7,
      HR_ENDING_ID = 8,
      HR_STAFFROLL_ID = 9,
      HR_MAX = 10,
    };

    enum HR_PROCESS_STS
    {
      HR_PROCESS_INIT = 0,
      HR_PROCESS_INITWAIT = 1,
      HR_PROCESS_SCENARIOSELECT = 2,
      HR_PROCESS_SCENARIOSCRIPTLOADING = 3,
      HR_PROCESS_INITSTAGE_LOADING = 4,
      HR_PROCESS_TITLEPROCESS = 5,
      HR_PROCESS_TITLETERM = 6,
      HR_PROCESS_INIT_NEW_OR_LOAD = 7,
      HR_PROCESS_INGAMEINITSTAGE_LOADING = 8,
      HR_PROCESS_INGAMEINITSTAGE_LOADINGWAIT = 9,
      HR_PROCESS_IDLE = 10,
      HR_PROCESS_IDLE2 = 11,
      HR_PROCESS_GAMEOVER_WAIT = 12,
      HR_PROCESS_GAMEOVER = 13,
      HR_PROCESS_GAMEOVER_LOADSTAGE = 14,
      HR_PROCESS_GAMEOVER_ENDWAIT = 15,
      HR_PROCESS_BOSSRESULT_SETUP = 16,
      HR_PROCESS_BOSSRESULT_TO_DEMO = 17,
      HR_PROCESS_BOSSRESULT_TO_DEMO_WAIT = 18,
      HR_PROCESS_RANKUP_SETUP = 19,
      HR_PROCESS_RANKUP = 20,
      HR_PROCESS_RANKUP_TO_STAGECHANGE = 21,
      HR_PROCESS_STAFF1_SETUP = 22,
      HR_PROCESS_STAFF1 = 23,
      HR_PROCESS_STAFF2_SETUP = 24,
      HR_PROCESS_STAFF2 = 25,
      HR_PROCESS_SHOOTING_SETUP = 26,
      HR_PROCESS_SHOOTING = 27,
      HR_PROCESS_LAST_LOGO_DISP_SETUP = 28,
      HR_PROCESS_LAST_LOGO_DISP = 29,
      HR_PROCESS_LAST_LOGO_DISP_TERM = 30,
      HR_PROCESS_FADEOUT = 31,
      HR_PROCESS_STAGECHANGE = 32,
      HR_PROCESS_RELEASE = 33,
      HR_PROCESS_END = 34,
      HR_PROCESS_EXIT = 35,
      HR_PROCESS_LOADSUBMISSION = 36,
      HR_PROCESS_LOADSUBMISSIONREQUEST = 37,
      HR_PROCESS_LOADSUBMISSIONREQUEST_WAIT = 38,
      HR_PROCESS_LOADSUBMISSIONREQUEST_RELEASEWAIT = 39,
      HR_PROCESS_ENDSAVE_SETUP = 40,
      HR_PROCESS_MAX = 41,
    };

  /* 0x0000 */ char Padding_972[0x50];
  /* 0x0050 */ enum HrGameTask::HR_LOAD_STS mLoadSts;
  /* 0x0054 */ class HrRankingUp* mpRankingUp;
  /* 0x0058 */ int mDebugScenarioID;
  /* 0x005c */ char mDebugScenarioInputKeta;
  /* 0x005d */ char mDebugScenarioMenu;
  /* 0x005e */ char mDebugLogoSkip;
  /* 0x005f */ char Padding_973;
  /* 0x0060 */ class CDEBUGINFO* m_pDebug;
  /* 0x0064 */ int mDebugSubMissionID;
  /* 0x0068 */ char mDebugSubMissionInputKeta;
  /* 0x0069 */ struct LOADING mLoading;
  /* 0x006c */ class HrBossResult* mpBossResult;
  /* 0x0070 */ class HrStaffRoll* mStaffRoll;
  /* 0x0074 */ class HrStaffRoll2* mStaffRoll2;
  /* 0x0078 */ class HStgTask* mpShooting;
  /* 0x007c */ int mLastLogoSndGroupID;
  /* 0x0080 */ int mLastSaveSndGroupID;
  /* 0x0084 */ int TerminateCnt;
  // union // edited
  // {
  //   /* 0x0000 */ __int64 m_TotalStartPlayTime;
  //   /* 0x0000 */ __int64 m_StartPlayTime;
  //   /* 0x0000 */ __int64 m_ScenarioPlayTime;
  //   /* 0x0000 */ bool mNewGameRequestForTitle;
  //   struct
  //   {
      /* 0x0000 */ //bool mAllClearHikitugi;
      /* 0x0001 */ //char Padding_974[0x3];
      /* 0x0088 */ bool mInitStageLoad;
      /* 0x0089 */ bool mOpeningMovieRequest;
      /* 0x008a */ bool mNewGameRequest;
      /* 0x008b */ bool mGotoTitleRequest;
      /* 0x008c */ int mGameExit;
      /* 0x0090 */ int mGameExitProcess;
      /* 0x0094 */ bool mAllClearDataLoadFlag;
      /* 0x0095 */ bool mMainScenarioRun;
      /* 0x0096 */ bool mSubMissionRun;
      /* 0x0097 */ char Padding_975;
      /* 0x0098 */ int mSetSubMissionID;
      /* 0x009c */ int mGameLevel;
      /* 0x00a0 */ bool mHomeButtonDisEnable;
      /* 0x00a1 */ char Padding_976[3];
      /* 0x00a4 */ enum HrGameTask::HRGAME_ID m_Process_id;
      /* 0x00a8 */ enum HrGameTask::HR_PROCESS_STS m_Pro_Sts;
      /* 0x00ac */ struct tagHRSAVEDATA* mp_SaveData;
      /* 0x00b0 */ struct tagHRSAVEDATA* mp_CheckPoint;
      /* 0x00b4 */ struct tagHRSAVEDATA* mp_HikitugiSaveData;
      /* 0x00b8 */ int m_SmRunflag[60];
      /* 0x01a8 */ class HrScriptTask* mpGameScrTask;
      /* 0x01ac */ class HrDemo* m_pDemo;
      /* 0x01b0 */ class HrMenuTask* m_pMenu;
      /* 0x01b4 */ class HrGameOver* mpGameOver;
      /* 0x01b8 */ bool mBossResultRequest;
      /* 0x01b9 */ bool mRankingUpRequest;
      /* 0x01ba */ char Padding_977[2];
      /* 0x01bc */ int mStageChangeFadeType;
      /* 0x01c0 */ bool mStaffRollRequest;
      /* 0x01c1 */ bool mStaffRoll2Request;
      /* 0x01c2 */ bool mShootingRequest;
      /* 0x01c3 */ bool mLastLogoRequest;
      /* 0x01c4 */ bool mLastSaveRequest;
      /* 0x01c5 */ char Padding_978[3];
      /* 0x01c8 */ int mLastSaveProcess;
      /* 0x01cc */ struct GAMESAVERESOURCE mLastSaveResource;
      /* 0x01d8 */ class HrSave* mpLastSave;
      /* 0x01dc */ int mLastLogoProcess;
      /* 0x01e0 */ struct tagGHMR_TEX mLastLogoTex[4];
      /* 0x0280 */ int mLastLogoTexDispNum;
      /* 0x0284 */ class WAnimLinearF mLastLogoAlpha;
      /* 0x029c */ int mLastLogoDispTick;
      /* 0x02a0 */ class WAnimLinearF mLastLogoMoveX;
      /* 0x02b8 */ int mGameSaveProcess;
      /* 0x02bc */ struct GAMESAVERESOURCE mGameSaveResource;
      /* 0x02c8 */ class HrSave* mpGameSave;
      /* 0x02cc */ int mGameSaveSetUpWait;
      /* 0x02d0 */ int mGameSaveMode;
      /* 0x02d4 */ class TGmfNode* mpGameSavePcNode;
      /* 0x02d8 */ bool mGameSaveCopyLightFadeOutReq;
      /* 0x02d9 */ char Padding_979[3];
      /* 0x02dc */ int mLoadingWallType;
      /* 0x02e0 */ int mBossResultAfterDemoNo;
      /* 0x02e4 */ bool m_STG_Feed_f;
      /* 0x02e5 */ char Padding_980[3];
      /* 0x02e8 */ class HrSubGameTask* m_pSubTask;
    //}; /* size: 0x0256 */ // edited
    /* 0x0000 */ char mScenarioScriptFileName[64];
    /* 0x0000 */ char mSubMissionScriptFileName[64];
    /* 0x0000 */ int mRefreshCounter;
  //}; /* size: 0x0256 */ // edited
}; /* size: 0x02ec */
