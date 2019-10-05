#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <cmath>
#include <cstring>
#include <climits>

#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <string>
#include <iterator>
#include <algorithm>

#include "dvm.h"
#include "acc_across_analyzer.h"

extern bool READ;
extern bool WRITE;
extern bool dontGenConvertXY;
extern bool oneCase;
extern int ACROSS_MOD_IN_KERNEL;
extern int DVM_DEBUG_LVL;
extern const int rtTypes[];

extern std::set<std::string> intrinsicF;
extern std::set<std::string> intrinsicDoubleT;
extern std::set<std::string> intrinsicFloatT;
extern std::set<std::string> intrinsicInt4T;

extern std::map<char, const char*> SpecialSymbols;
extern std::vector <SgFunctionCallExp* > RTC_FCall;
extern std::vector<SgExpression* > RTC_FArgs;
extern std::vector<SgFunctionCallExp* > RTC_FKernelArgs;
extern std::vector<SgSymbol*> newVars;

extern const char *funcDvmhConvXYname;
extern int number_of_loop_line;
extern std::stack<SgStatement*> CopyOfBody;
extern Loop *currentLoop;
extern unsigned countKernels;

extern SgType *indexType_int, *indexType_long, *indexType_llong;
extern SgSymbol *s_indexType_int, *s_indexType_long, *s_indexType_llong;

extern const char *declaration_cmnt;
extern int loc_el_num;
extern SgStatement *cur_in_mod, *cur_in_kernel;
extern SgStatement *dvm_parallel_dir, *loop_body;
extern SgStatement *kernel_st;
extern SgExpression *private_list, *uses_list, *kernel_index_var_list, *formal_red_grid_list;
extern struct local_part_list *lpart_list;
extern SgSymbol *kernel_symb, *s_overall_blocks;

extern SgType *t_dim3;
extern SgSymbol *s_threadidx, *s_blockidx, *s_blockdim, *s_griddim, *s_blocks_k;

//------ C ----------
extern SgStatement *block_C, *block_C_Cuda, *info_block;
extern SgSymbol *s_DvmhLoopRef, *s_cudaStream, *s_cmplx, *s_dcmplx;

enum ACROSS_ANALYZE { RIGHT, LEFT, RESTORE, ADD, ACROSS_TYPE, NON_ACROSS_TYPE };
enum rt_TYPES { rt_INT, rt_LONG, rt_LLONG };
enum { _NUL_, _READ_, _WRITE_, _READ_WRITE_ };
enum VAR_TYPES { EMPTY, INTENT_IN, INTENT_OUT, INTENT_INOUT, INTENT_LOCAL, INTENT_INLOCAL };
enum { ZERO_, HANDLER_TYPE_PARALLEL, HANDLER_TYPE_MASTER };
enum RED_TYPES { red_NULL, red_SUM, red_PROD, red_MAX, red_MIN, red_AND, red_OR, 
                 red_NEQ, red_EQ, red_MAXL, red_MINL, red_SUM_N, red_PROD_N, 
                 red_MAX_N, red_MIN_N, red_AND_N, red_OR_N, red_NEQ_N, red_EQ_N };
