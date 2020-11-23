#pragma once

#include "dvmh_types.h"
#pragma GCC visibility push(default)
#include <dvmlib.h>
#pragma GCC visibility pop

#define MAXARRAYDIM 7
#define MaxParFileName 128

#include "system.typ"

#define map_BLOCK 1
#define map_COLLAPSE 2
#define map_REPLICATE 3
#define map_NORMVMAXIS 4
#define map_CONSTANT 5

#define align_REPLICATE 1
#define align_CONSTANT 3
#define align_BOUNDREPL 4
#define sht_DisArray 1
#define sht_AMView 3
#define sht_VMS 4
#define sht_ParLoop 6

extern "C" s_VMS *DVM_VMS;
extern "C" double ProcWeightArray[];
extern "C" DvmType *DAHeaderAddr[];
extern "C" int DACount;
extern "C" unsigned char DisArrayFill;
extern "C" byte FortranFlag;
extern "C" byte Minus;
extern "C" s_COLLECTION *gEnvColl;

extern "C" unsigned char trace_Dump(int, unsigned int, int);
extern "C" unsigned int TraceBufCountArr[];
extern "C" int TraceBufFullArr[];
extern "C" int MPS_CurrentProc;

extern "C" byte AllowRedisRealnBypass;
