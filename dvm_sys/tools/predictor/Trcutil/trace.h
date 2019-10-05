#ifndef _TRACE_H_
#define _TRACE_H_

#include <fstream>
#include <string>
using namespace std;

enum EKeywords
{
    N_FULL = 0,
    N_MODIFY,
    N_MINIMAL,
    N_NONE,
    N_MODE,
    N_EMPTYITER,
    N_SLOOP,
    N_PLOOP,
    N_TASKREGION,
    N_ITERATION,
    N_PRE_WRITE,
    N_POST_WRITE,
    N_R_PRE_WRITE,
    N_R_POST_WRITE,
    N_R_READ,
    N_REDUCT,
    N_READ,
    N_SKIP,
    N_END_LOOP,
    N_END_HEADER,
    N_ARRAY,
    N_MULTIDIM_ARRAY,
    N_DEF_ARR_STEP,
    N_DEF_ITER_STEP,
    N_UNKNOWN
};

extern const char *g_rgKeywords[];

void findMatches(const CLevelList& vDefLevel, ifstream& in);
void compare(const CLevelList& vLevel1, const CLevelList& vLevel2, long lLine);
void printBad(long lLine);

#endif //_TRACE_H_