#include "leak_detector.h"

#include "acc_data.h"

// global data for ACC files

bool READ = false;
bool WRITE = true;
bool dontGenConvertXY = false;
bool oneCase = false;
int ACROSS_MOD_IN_KERNEL = 0;
int DVM_DEBUG_LVL = 0;
const int rtTypes[] = { rt_INT, rt_LLONG };

std::set<std::string> intrinsicF;
std::set<std::string> intrinsicDoubleT;
std::set<std::string> intrinsicFloatT;
std::set<std::string> intrinsicInt4T;

std::map<char, const char*> SpecialSymbols;
std::vector <SgFunctionCallExp* > RTC_FCall;
std::vector<SgExpression* > RTC_FArgs;
std::vector<SgFunctionCallExp* > RTC_FKernelArgs;
std::vector<SgSymbol*> newVars;
std::stack<SgStatement*> CopyOfBody;

const char *funcDvmhConvXYname = "dvmh_convert_XY";
Loop *currentLoop = NULL;
unsigned countKernels = 2;

int number_of_loop_line = 0; // for TRACE in acc_f2c.cpp
SgSymbol *s_indexType_int = NULL, *s_indexType_long = NULL, *s_indexType_llong = NULL;
SgType *indexType_int = NULL, *indexType_long = NULL, *indexType_llong = NULL;

const char *declaration_cmnt;
int loc_el_num;
SgStatement *cur_in_mod, *cur_in_kernel;
SgStatement *dvm_parallel_dir, *loop_body;
SgStatement *kernel_st;
SgExpression *private_list, *uses_list, *kernel_index_var_list, *formal_red_grid_list;
SgSymbol *kernel_symb, *s_overall_blocks;
SgType *t_dim3;
SgSymbol *s_threadidx, *s_blockidx, *s_blockdim, *s_griddim, *s_blocks_k;

//------ C ----------
SgStatement *block_C, *block_C_Cuda, *info_block;
SgSymbol *s_DvmhLoopRef, *s_cudaStream, *s_cmplx, *s_dcmplx;
