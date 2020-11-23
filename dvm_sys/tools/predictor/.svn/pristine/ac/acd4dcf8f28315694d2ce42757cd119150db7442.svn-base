#include <assert.h>
#include <fstream>

//	#include "Event.h"
#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"

using namespace std;

extern ofstream prot;

extern _DArrayInfo	*	GetDArrayByIndex(long ID);

extern	_ParLoopInfo ParLoopInfo;

_ShadowInfo		*Shadows		= NULL;
int _ShadowInfo::count	= 0;

_ShdGrpInfo		*ShdGroups		= NULL;
int _ShdGrpInfo::count	= 0;
//====
int type_size;
//=***


//---------------------------------- Shadow --------------------------------------------

int GetShadowIndex(long ID)
{
	int i;
    for (i = Shadows->size() - 1; (i >= 0)  && Shadows[i].ID!=ID; i--);
    return i;
}

_ShadowInfo*  GetShadowByIndex(long ID)
{
	int i=GetShadowIndex(ID);
    return (i >= 0) ? &Shadows[i] : NULL;
}

_ShadowInfo*  AddShadow(long ID)
{
	_ShadowInfo* tmp;
    int curr_size = Shadows->size();

    Shadows=(_ShadowInfo*)realloc(Shadows,(curr_size+1)*sizeof(_ShadowInfo));
	assert(Shadows != NULL);
	++*Shadows;
    tmp=&Shadows[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelShadow(long ID)
{
	int idx=GetShadowIndex(ID);
    int curr_size = Shadows->size();
    int i;
    if(idx<0) return;
    for(i=idx+1; i<curr_size; i++) {
        Shadows[i-1]=Shadows[i];
    }
    Shadows=(_ShadowInfo*)realloc(Shadows,(curr_size-1)*sizeof(_ShadowInfo));
	assert((Shadows != NULL) || (curr_size == 1));
	--*Shadows;
}

//---------------------------------- ShdGroup --------------------------------------------

int GetShdGroupIndex(long ID)
{
	int i;
    for (i = ShdGroups->size() - 1; (i >= 0)  && ShdGroups[i].ID!=ID; i--);
    return i;
}

_ShdGrpInfo*  GetShdGroupByIndex(long ID)
{ 
	int i=GetShdGroupIndex(ID);
    return (i>=0) ? &ShdGroups[i] : NULL;
}

_ShdGrpInfo*  AddShdGroup(long ID)
{
	_ShdGrpInfo* tmp;
    int curr_size = ShdGroups->size();

    ShdGroups=(_ShdGrpInfo*)realloc(ShdGroups,(curr_size+1)*sizeof(_ShdGrpInfo));
	assert(ShdGroups != NULL);
	++*ShdGroups;
    tmp=&ShdGroups[curr_size];
	tmp->ProcessTimeStamp = new double[rootProcCount];
    tmp->ID=ID;

    return tmp;
}

void DelShdGroup(long ID)
{
	int idx=GetShdGroupIndex(ID);
    int curr_size = ShdGroups->size();
    int i;

    if (idx<0) 
		return;

    delete ShdGroups[idx].BoundGroup_Obj;
	delete ShdGroups[idx].ProcessTimeStamp;

    for(i=idx+1; i<curr_size; i++)
       ShdGroups[i-1]=ShdGroups[i];
    ShdGroups=(_ShdGrpInfo*)realloc(ShdGroups,(curr_size-1)*sizeof(_ShdGrpInfo));
	assert((ShdGroups != NULL) || (curr_size == 1));
	--*ShdGroups;
}


//-------------------------------------------------------------------------------

void FuncCall::crtshg()
{
	crtshg_Info* params = (crtshg_Info*) call_params;
    _ShdGrpInfo* tmp = AddShdGroup(params->ShadowGroupRef);		// ShadowGroupRef
//	std::cout << tmp << std::endl;
    tmp->BoundGroup_Obj = new BoundGroup();
}

void FuncCall::inssh()
{
	inssh_Info* params  = (inssh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ShadowGroupRef);
    _DArrayInfo* DA=GetDArrayByIndex(params->ArrayHeader);		// DA_ID
//====
type_size=DA->DArray_Obj->TypeSize;
//=***

    SG->BoundGroup_Obj->AddBound(DA->DArray_Obj,params->LowShdWidthArray,
                                 params->HiShdWidthArray, params->FullShdSign);
}

void FuncCall::insshd()
{
	inssh_Info* params  = (inssh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ShadowGroupRef);
    _DArrayInfo* DA=GetDArrayByIndex(params->ArrayHeader);		// DA_ID
//====
type_size=DA->DArray_Obj->TypeSize;
//=***
    SG->BoundGroup_Obj->AddBound(DA->DArray_Obj,params->LowShdWidthArray,
                                 params->HiShdWidthArray, 0);	// 0 - EVA ???
}

static void setShdWidth(inssh_Info* params, _DArrayInfo* DA) 
{
	int i;
	unsigned arrayRank = params->InitLowShdIndex.size();

	params->HiShdWidthArray.resize(arrayRank);
	params->LowShdWidthArray.resize(arrayRank);

	for (i = 0; i < arrayRank; i++) {

		// Low shadow
		if (params->InitLowShdIndex[i] == -1) {
			if (params->LastLowShdIndex[i] == -1) {
				params->LowShdWidthArray[i] = DA->DArray_Obj->LowShdWidthArray[i];
			} else {
				params->LowShdWidthArray[i] = params->LastLowShdIndex[i];
			}
		} else {
			params->LowShdWidthArray[i] = params->LastLowShdIndex[i] -
				params->InitLowShdIndex[i] + 1;
		}

		// High shadow
		if (params->InitHiShdIndex[i] == -1) {
			if (params->LastHiShdIndex[i] == -1) {
				params->HiShdWidthArray[i] = DA->DArray_Obj->HiShdWidthArray[i];
			} else {
				params->HiShdWidthArray[i] = params->LastHiShdIndex[i];
			}
		} else {
			params->HiShdWidthArray[i] = params->LastHiShdIndex[i] -
				params->InitHiShdIndex[i] + 1;
		}
//		prot << "LowShdWidthArray[" << i << "] = " << params->LowShdWidthArray[i] << "  ";
//		prot << "HiShdWidthArray[" << i << "] = " << params->HiShdWidthArray[i] << endl;
	}
}

void FuncCall::incsh()
{
	inssh_Info* params  = (inssh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ShadowGroupRef);
    _DArrayInfo* DA=GetDArrayByIndex(params->ArrayHeader);		// DA_ID

	setShdWidth(params, DA);

    SG->BoundGroup_Obj->AddBound(DA->DArray_Obj,params->LowShdWidthArray,
                                 params->HiShdWidthArray, params->FullShdSign);
}

void FuncCall::incshd()
{
	inssh_Info* params  = (inssh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ShadowGroupRef);
    _DArrayInfo* DA=GetDArrayByIndex(params->ArrayHeader);		// DA_ID

	setShdWidth(params, DA);

    SG->BoundGroup_Obj->AddBound(DA->DArray_Obj,params->LowShdWidthArray,
                                 params->HiShdWidthArray, 0);	// 0 - EVA ???
}

void FuncCall::delshg()
{
	delshg_Info* params=(delshg_Info*) call_params;
    DelShdGroup(params->ID);
}


void FuncCall::strtsh()
{
	strtsh_Info* params=(strtsh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ID);
    _ShadowInfo* SHD=AddShadow(params->ID);
    double shd_time=SG->BoundGroup_Obj->StartB();

//		printf("Start shadow\n");
    MPSSynchronize(__Wait_shadow);
//		printf("Start shadow end %f\n",shd_time);

    SHD->time_start=CurrProcTime(0);
    SHD->time_end=SHD->time_start+(shd_time/MPSProcCount());
	++CurrInterval->num_op_shadow;
}

void FuncCall::waitsh()
{
	waitsh_Info* params=(waitsh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ID);
    _ShadowInfo* SHD=GetShadowByIndex(params->ID);

//	assert(SHD != NULL);
	if (SHD == NULL) {
//		cout << "Pipeline recvsh/sendsh is not implemented." << endl;
//		prot << "Pipeline recvsh/sendsh is not implemented." << endl;
		exit(0);
	}

    int i; 
    double curr_pt;

	for (i=0; i<MPSProcCount(); i++) {

		curr_pt = CurrProcTime(currentVM->map(i));
		if(curr_pt < SHD->time_end) {

//			printf("Start WAIT[%d] shadow %f-%f=%f or %f \n",i,SHD->time_start, SHD->time_end, SHD->time_end-SHD->time_start, curr_pt);
			AddTime(__Shadow_overlap,currentVM->map(i), curr_pt - SHD->time_start);
			AddTime(__Wait_shadow,currentVM->map(i), SHD->time_end - curr_pt);
		} else {
//			AddTime(__Shadow_overlap,currentVM->map(i), SHD->time_end - SHD->time_start);

			AddTime(__Shadow_overlap,currentVM->map(i), curr_pt - SHD->time_start);
		}
	}
//		printf("END WAIT shadow\n");
    DelShadow(params->ID);
}

void FuncCall::exfrst()
{
	exfrst_Info * params = (exfrst_Info *) call_params;
	assert(ParLoopInfo.ID = params->ID);
//	ParLoopInfo.exfrst = true;
//	ParLoopInfo.exfrst_SGR = params->ShadowGroupRef;

    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ShadowGroupRef);
    _ShadowInfo* SHD=AddShadow(params->ShadowGroupRef);
    double shd_time=SG->BoundGroup_Obj->StartB();

    MPSSynchronize(__Wait_shadow);
    SHD->time_start = CurrProcTime(0);
//	printf("Shd+=%f\n",shd_time);

    SHD->time_end = SHD->time_start+shd_time;
	++CurrInterval->num_op_shadow;
}


void FuncCall::imlast()
{
	imlast_Info * params = (imlast_Info *) call_params;
	assert(ParLoopInfo.ID = params->ID);
	ParLoopInfo.imlast = true;
	ParLoopInfo.imlast_SGR = params->ShadowGroupRef;
}

void FuncCall::sendsh()
{
//	prot << "sendsh" << endl;
	++CurrInterval->num_op_shadow;
}

void FuncCall::recvsh()
{
//	prot << "recvsh" << endl;

#ifdef nodef
	int i;
	recvsh_Info* params=(recvsh_Info*) call_params;
    _ShdGrpInfo* SG=GetShdGroupByIndex(params->ID);
	assert(SG != NULL);
    _ShadowInfo* SHD=AddShadow(params->ID);
	assert(SHD != NULL);

    char dimBound = SG->BoundGroup_Obj->getDimBound();
    int vmDim = SG->BoundGroup_Obj->getVmDimension();
    int vmDimSize = currentVM->GetSize(vmDim);
	int vmDimMult = currentVM->GetMult(vmDim);
    double shd_time=SG->BoundGroup_Obj->StartB();
	double shd_time1 = 0.0;

	// clean ProcessTimeStamp
	for (i = 0; i < rootProcCount; i++)
		SG->ProcessTimeStamp[i] = 0.0;

	if (vmDimSize >= 2) {
		if (dimBound == 'L') {
			for (i = vmDimSize - 2; i >= 0; i--) {
				// Left bound
				shd_time1 += shd_time;
				SG->ProcessTimeStamp[currentVM->map(i)] = shd_time1;
			}
		} else if (dimBound == 'R') {
			for (i = 1; i < vmDimSize; i++) {
				// Left bound
				shd_time1 += shd_time;
				SG->ProcessTimeStamp[currentVM->map(i)] = shd_time1;
			}
		} else {
		}
	}

//    SHD->time_start=CurrProcTime(0);
//    SHD->time_end=SHD->time_start+shd_time;
#endif
}

void FuncCall::across()
{

	across_Info * params = (across_Info *) call_params;
//====
    _ShdGrpInfo* SGNEW=GetShdGroupByIndex(params->NewShadowGroupRef);
	ParLoopInfo.ParLoop_Obj->Across(SGNEW->BoundGroup_Obj->GetBoundCost(),type_size);
//=***

/*ch
	if (params->OldShadowGroupRef == 0) {
		++CurrInterval->num_op_shadow;

		printf("Bad news\n");
		return;
	}
*/

//    _ShdGrpInfo* SG=GetShdGroupByIndex(params->OldShadowGroupRef);
//====
	ParLoopInfo.SGnew=GetShdGroupByIndex(params->NewShadowGroupRef);

	if(params->OldShadowGroupRef!=0)
		ParLoopInfo.SG=GetShdGroupByIndex(params->OldShadowGroupRef);
	else
		ParLoopInfo.SG=0;

//	ParLoopInfo.across=true;
//printf("PARLoopInfo %d\n",ParLoopInfo.ParLoop_Obj->GetLoopSize());

//was    _ShdGrpInfo* SG=GetShdGroupByIndex(params->OldShadowGroupRef);

    double shd_time=0;
//was   double shd_time=SG->BoundGroup_Obj->StartB();
//printf("shadow time = %f\n",shd_time);
//=***

    MPSSynchronize(__Wait_shadow);
	/*ch
	if(params->OldShadowGroupRef != 0) //ch
	{
	    _ShadowInfo* SHD=AddShadow(params->OldShadowGroupRef);
		SHD->time_start = CurrProcTime(0);
		SHD->time_end = SHD->time_start+shd_time;
	}
	*/
	++CurrInterval->num_op_shadow;
	ParLoopInfo.across = true;
	if(params->OldShadowGroupRef != 0) //ch
		ParLoopInfo.across_SGR = params->OldShadowGroupRef;
	else
		ParLoopInfo.across_SGR = 0;

}

void FuncCall::ShadowTime()
{
    switch(func_id) {
        case crtshg_ :
			crtshg();
			break;
        case inssh_ :
			inssh();
			break;
        case insshd_ :
			insshd();
			break;
        case incsh_ :
			incsh();
			break;
        case incshd_ :
			incshd();
			break;
        case delshg_ :
			delshg();
			break;
        case strtsh_ :
			strtsh();
			break;
        case waitsh_ :
			waitsh();
			break;
		case  exfrst_:
			exfrst();
			break;
		case  imlast_:
			imlast();
			break;
		case  sendsh_:
			sendsh();
			break;
		case  recvsh_:
			recvsh();
			break;
		case  across_:
			across();
			break;
      }

	// calculate times

	RegularTime();  
}
