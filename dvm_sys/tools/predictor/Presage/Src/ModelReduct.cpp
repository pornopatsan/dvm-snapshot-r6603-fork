#include <fstream>
#include <assert.h>

#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"
#include "Vm.h"

using namespace std;

extern ofstream prot;

extern _DArrayInfo	*	GetDArrayByIndex(long ID);
extern _AMViewInfo	*	GetAMViewByIndex(long ID);
extern _ParLoopInfo ParLoopInfo;

_RedVarInfo		*RedVars		= NULL;
_RedGrpInfo		*RedGroups		= NULL;
_ReductInfo		*Reductions		= NULL;

int _RedVarInfo::count	= 0;
int _RedGrpInfo::count	= 0;
int _ReductInfo::count	= 0;

//---------------------------------- RedVar ----------------------------------------------

int GetRedVarIndex(long ID)
{
	int i;
    for (i = RedVars->size() - 1; (i >= 0)  && RedVars[i].ID!=ID; i--);
    return i;
}

_RedVarInfo*  GetRedVarByIndex(long ID)
{
	int i = GetRedVarIndex(ID);
    return (i>=0) ? &RedVars[i] : NULL;
}

_RedVarInfo*  AddRedVar(long ID)
{
	_RedVarInfo* tmp;
    int curr_size = RedVars->size();

    RedVars=(_RedVarInfo*)realloc(RedVars,(curr_size+1)*sizeof(_RedVarInfo));
	assert(RedVars != NULL);
	++*RedVars;
    tmp=&RedVars[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelRedVar(long ID)
{
	int idx=GetRedVarIndex(ID);
    int curr_size = RedVars->size();
    int i;

    if (idx<0) 
		return;
    delete RedVars[idx].RedVar_Obj;
    for(i=idx+1; i<curr_size; i++)
      { RedVars[i-1]=RedVars[i];
      }
    RedVars=(_RedVarInfo*)realloc(RedVars,(curr_size-1)*sizeof(_RedVarInfo));
	assert((RedVars != NULL) || (curr_size == 1));
	--*RedVars;
	
}

//---------------------------------- RedGroup --------------------------------------------

int GetRedGroupIndex(long ID)
{
	int i;
    for (i = RedGroups->size() - 1; (i >= 0)  && RedGroups[i].ID!=ID; i--);
    return i;
}

_RedGrpInfo*  GetRedGroupByIndex(long ID)
{
	int i=GetRedGroupIndex(ID);
    return (i>=0) ? &RedGroups[i] : NULL;
}

_RedGrpInfo*  AddRedGroup(long ID)
{ 
	_RedGrpInfo* tmp;
    int curr_size = RedGroups->size();

    RedGroups=(_RedGrpInfo*)realloc(RedGroups,(curr_size+1)*sizeof(_RedGrpInfo));
	assert(RedGroups != NULL);
	++*RedGroups;
    tmp=&RedGroups[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelRedGroup(long ID)
{ 
	int idx=GetRedGroupIndex(ID);
    int curr_size = RedGroups->size();
    int i;

    if(idx<0) return;
    delete RedGroups[idx].RedGroup_Obj;
    for(i=idx+1; i<curr_size; i++) {
		RedGroups[i-1]=RedGroups[i];
    }
    RedGroups=(_RedGrpInfo*)realloc(RedGroups,(curr_size-1)*sizeof(_RedGrpInfo));
	assert((RedGroups != NULL) || (curr_size == 1));
	--*RedGroups;
}

//---------------------------------- Reduct --------------------------------------------

int GetReductIndex(long ID)
{
	int i;
    for (i = Reductions->size() - 1; (i >= 0)  && Reductions[i].ID!=ID; i--);
    return i;
}

_ReductInfo*  GetReductByIndex(long ID)
{
	int i=GetReductIndex(ID);
    return (i>=0) ? &Reductions[i] : NULL;
}

_ReductInfo*  AddReduct(long ID)
{
	_ReductInfo* tmp;
    int curr_size = Reductions->size();

    Reductions=(_ReductInfo*)realloc(Reductions,(curr_size+1)*sizeof(_ReductInfo));
	assert(Reductions != NULL);
	++*Reductions;
    tmp=&Reductions[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelReduct(long ID)
{
	int idx=GetReductIndex(ID);
    int curr_size = Reductions->size();
    int i;

    if(idx<0) return;
    for(i=idx+1; i<curr_size; i++)
      { Reductions[i-1]=Reductions[i];
      };
    Reductions=(_ReductInfo*)realloc(Reductions, (curr_size-1)*sizeof(_ReductInfo));
	assert((Reductions != NULL) || (curr_size == 1));
	--*Reductions;
}

//--------------------------------------------------------------------------------------------------

void FuncCall::crtrg()
{
	crtrg_Info* params=(crtrg_Info*) call_params;
    _RedGrpInfo* tmp=AddRedGroup(params->ID);
    tmp->RedGroup_Obj=new RedGroup(currentVM);
}

void FuncCall::crtred()
{
	crtred_Info* params=(crtred_Info*) call_params;
    _RedVarInfo* tmp=AddRedVar(params->ID);
    int RedElmSize=0;

    switch(params->RedArrayType) {
        case 1 :
          RedElmSize=sizeof(int); break;
        case 2:
          RedElmSize=sizeof(long); break;
        case 3:
          RedElmSize=sizeof(float); break;
        case 4:
          RedElmSize=sizeof(double); break;
      };
    tmp->RedVar_Obj=new RedVar(RedElmSize, params->RedArrayLength, params->LocElmLength);
}

void FuncCall::insred()
{
	insred_Info* params=(insred_Info*) call_params;
    _RedVarInfo* RV=GetRedVarByIndex(params->RV_ID);
    _RedGrpInfo* RG=GetRedGroupByIndex(params->RG_ID);

    RG->RedGroup_Obj->AddRV(RV->RedVar_Obj);
}

void FuncCall::delred()
{
	delred_Info* params=(delred_Info*) call_params;
    DelRedVar(params->ID);
}

void FuncCall::delrg()
{
	delrg_Info* params=(delrg_Info*) call_params;
    DelRedGroup(params->ID);
}

void FuncCall::strtrd()
{
	strtrd_Info* params=(strtrd_Info*) call_params;
    _ReductInfo* RED=AddReduct(params->ID);
//		printf("Reduction sync\n");

    MPSSynchronize(__Wait_reduct);

////    MPSSynchronize(__Synchronize);
    RED->time_start = CurrProcTime(0); // центральный - нулевой
	++CurrInterval->num_op_reduct;
}

void FuncCall::waitrd()
{
	int i; 
	double curr_pt;
	double red_time;
	_AMViewInfo* AM;
	_DArrayInfo* DA;

    waitrd_Info* params=(waitrd_Info*) call_params;
    _ReductInfo* RED=GetReductByIndex(params->ID);
    _RedGrpInfo* RG=GetRedGroupByIndex(params->ID);

	if (ParLoopInfo.PatternType == 1) {
		// AMView
		AM = GetAMViewByIndex(ParLoopInfo.PatternID);
		red_time = RG->RedGroup_Obj->StartR(AM->AMView_Obj, ParLoopInfo.Rank, 
                                             ParLoopInfo.AxisArray);
	} else if (ParLoopInfo.PatternType == 2) {
		// DisArray
		DA = GetDArrayByIndex(ParLoopInfo.PatternID);
		red_time = RG->RedGroup_Obj->StartR(DA->DArray_Obj, ParLoopInfo.Rank, 
                                             ParLoopInfo.AxisArray);
	}

    RED->time_end=red_time;//RED->time_start+red_time;

    for (i=0; i<MPSProcCount(); i++) 
		{
//			printf("red %d   curr=%f  red_end=%f\n",i,CurrProcTime(currentVM->map(i)),RED->time_end);
        curr_pt = CurrProcTime(currentVM->map(i));
			if(curr_pt < RED->time_end) {
				AddTime(__Reduct_overlap, currentVM->map(i), (curr_pt - RED->time_start));
				AddTime(__Wait_reduct,currentVM->map(i), (RED->time_end - curr_pt));
			} else {
//				AddTime(__Reduct_overlap,currentVM->map(i), (RED->time_end - RED->time_start));
				AddTime(__Reduct_overlap, currentVM->map(i), (curr_pt - RED->time_start));
			}
	}
     
    DelReduct(params->ID);
}

void FuncCall::ReductTime()
{
    switch(func_id) {
        case crtrg_ :
          crtrg();
          break;
        case crtred_ :
          crtred();
          break;
        case insred_ :
          insred();
          break;
        case delred_ :
          delred();
          break;
        case delrg_ :
          delrg();
          break;
        case strtrd_ :
          strtrd();
          break;
        case waitrd_ :
          waitrd();
          break;
      }

	RegularTime();  
}

