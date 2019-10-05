#include <stdlib.h>
#include <assert.h>
#include <iostream>
using namespace std;


#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"

bool SynchCopy;//====//

extern _AMViewInfo	*	GetAMViewByIndex(long ID);
_DArrayFlag * DAF_tmp;

_DArrayInfo	*	DArrays		= NULL;
int				_DArrayInfo::count	= 0;

_DArrayFlag *   dArrayFlags = NULL;
int				_DArrayFlag::count = 0;

//---------------------------------- DArray ----------------------------------------------

int GetDArrayIndex(long ID)
{
	int i;
    for (i = DArrays->size() - 1; (i >= 0)  && DArrays[i].ID!=ID; i--);
    return i;
}

_DArrayInfo*  GetDArrayByIndex(long ID)
{
	int i = GetDArrayIndex(ID);
	return (i >=0) ? &DArrays[i] : NULL;
}

bool ResetDArrayKey(long OldKey, long NewKey)
{
	int i=GetDArrayIndex(OldKey);
	if (i >= 0) {
		DArrays[i].ID = NewKey;
		return true;
	} else
		return false;

}

_DArrayInfo*  AddDArray(long ID)
{
	_DArrayInfo* tmp;
    int curr_size = DArrays->size();

    DArrays=(_DArrayInfo*)realloc(DArrays,(curr_size+1)*sizeof(_DArrayInfo));
	assert(DArrays != NULL);
	++*DArrays;
    tmp=&DArrays[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelDArray(long ID)
{
	int idx=GetDArrayIndex(ID);
    int curr_size = DArrays->size();
    int i;

    if(idx<0) return;
    delete DArrays[idx].DArray_Obj;
    for(i=idx+1; i<curr_size; i++)
		DArrays[i-1]=DArrays[i];
    DArrays=(_DArrayInfo*)realloc(DArrays,(curr_size-1)*sizeof(_DArrayInfo));
	assert((DArrays != NULL) || (curr_size == 1));
	--*DArrays;
}

//---------------------------------- DArrayFlag --------------------------------------------

int GetDArrayFlagIndex(long ID)
{
	int i;
    for (i = dArrayFlags->size() - 1; (i >= 0)  && dArrayFlags[i].ID!=ID; i--);
    return i;
}

_DArrayFlag*  GetDArrayFlagByIndex(long ID)
{ 
	int i = GetDArrayFlagIndex(ID);
    return (i>=0) ? &dArrayFlags[i] : NULL;
}

_DArrayFlag*  AddDArrayFlag(long ID)
{
	_DArrayFlag* tmp;
    int curr_size = dArrayFlags->size();

    dArrayFlags = (_DArrayFlag*) realloc(dArrayFlags,(curr_size+1)*sizeof(_DArrayFlag));
	assert(dArrayFlags != NULL);
	++*dArrayFlags;
    tmp = &dArrayFlags[curr_size];
	tmp->ProcessTimeStamp = new double[rootProcCount];
	tmp->time_start = 0.0;
	tmp->time_end = 0.0;
    tmp->ID=ID;

    return tmp;
}

void DelDArrayFlag(long ID)
{
	int idx=GetDArrayFlagIndex(ID);
    int curr_size = dArrayFlags->size();
    int i;

    if (idx<0) 
		return;

	delete dArrayFlags[idx].ProcessTimeStamp;

    for(i=idx+1; i<curr_size; i++)
       dArrayFlags[i-1] = dArrayFlags[i];
    dArrayFlags = (_DArrayFlag*) realloc(dArrayFlags,(curr_size-1)*sizeof(_DArrayFlag));
	assert((dArrayFlags != NULL) || (curr_size == 1));
	--*dArrayFlags;
}

//------------------------------- Modelling functions DArray --------------------------

void FuncCall::crtda()
{ 
	crtda_Info* params = (crtda_Info*) call_params;
    _DArrayInfo* tmp=AddDArray(params->ArrayHeader);
    tmp->AlignType=0;
    tmp->DArray_Obj=new DArray(params->SizeArray,
		params->LowShdWidthArray, params->HiShdWidthArray, params->TypeSize);
}

void FuncCall::align() 
{
	align_Info* params=(align_Info*) call_params;
//    _DArrayInfo* ArrInfo=GetDArrayByIndex(params->ID);

    _DArrayInfo* ArrInfo=GetDArrayByIndex(params->ArrayHeader);
	assert(ArrInfo != NULL);

	if (params->PatternType == 1) {
		// AMView
		_AMViewInfo* AMV_Info=GetAMViewByIndex(params->PatternRef);
        ArrInfo->AlignType=1;
        ArrInfo->DArray_Obj->AlnDA(AMV_Info->AMView_Obj, params->AxisArray,
                                   params->CoeffArray, params->ConstArray);
	} else if (params->PatternType == 2) {
		// DisArray
		_DArrayInfo* DA_Info=GetDArrayByIndex(params->PatternRefPtr);
        ArrInfo->AlignType=2;
        ArrInfo->DArray_Obj->AlnDA(DA_Info->DArray_Obj, params->AxisArray,
                                   params->CoeffArray, params->ConstArray);
	}
}


void FuncCall::delda()
{
	delda_Info* params=(delda_Info*) call_params;
    DelDArray(params->ID);
}

void FuncCall::realn() 
{
    double		f_time;
	realn_Info* params = (realn_Info*) call_params;

    _DArrayInfo* ArrInfo=GetDArrayByIndex(params->ArrayHeader);
	assert(ArrInfo != NULL);

	if (params->PatternType == 1) {
		// AMView
		_AMViewInfo* AMV_Info=GetAMViewByIndex(params->PatternRef);
        ArrInfo->AlignType=1;
        f_time=ArrInfo->DArray_Obj->RAlnDA(AMV_Info->AMView_Obj, params->AxisArray,
                                           params->CoeffArray, params->ConstArray, 
                                           params->NewSign);
	} else if (params->PatternType == 2) {
		// DisArray
		_DArrayInfo* DA_Info=GetDArrayByIndex(params->PatternRefPtr);
        ArrInfo->AlignType=2;
		f_time=ArrInfo->DArray_Obj->RAlnDA(DA_Info->DArray_Obj, params->AxisArray,
                                           params->CoeffArray, params->ConstArray, 
                                           params->NewSign);
	} else {
		return;
	}
//	printf("Realign lasted %f sec\n",f_time);

    MPSSynchronize(__Redistribute);
    AddMPSTime(__Redistribute, f_time);
	++CurrInterval->num_op_redist;

}

void FuncCall::arrcpy()
{
	double f_time = 0.0;
	int i,j;
	vector<LS> blockIni;
	bool returned=false;


		SynchCopy=true;
	_DArrayInfo* DA_From	= NULL;
	_DArrayInfo* DA_To		= NULL;
	arrcpy_Info* params		= (arrcpy_Info*) call_params;

	if (params->FromBufferPtr == 0)
		DA_From=GetDArrayByIndex(params->FromArrayHeader);
	if (params->ToBufferPtr == 0)
		DA_To=GetDArrayByIndex(params->ToArrayHeader);

    if (DA_From == NULL && DA_To == NULL)
		return;
    if (DA_From != NULL && DA_To != NULL) {
//cout << DA_From->DArray_Obj->Rank() << ' ' << DA_From->ID << endl;
		// Distributed -> Distributed
        f_time=ArrayCopy(DA_From->DArray_Obj, params->FromInitIndexArray, 
                         params->FromLastIndexArray, params->FromStepArray,
                         DA_To->DArray_Obj, params->ToInitIndexArray, 
                         params->ToLastIndexArray, params->ToStepArray);

	} else if (DA_From == NULL && DA_To != NULL) {
		// Replicated -> Distributed
		returned=true;
	} else if (DA_From != NULL && DA_To == NULL) {
		// Distributed -> Replicated
        f_time=ArrayCopy(DA_From->DArray_Obj, params->FromInitIndexArray, 
	                     params->FromLastIndexArray, params->FromStepArray,
						 params->CopyRegim);

	}



	// добавляем времена на присваивания переменных при копировании (для каждого процессора оно свое)
	if(DA_To != NULL)
	{
		for(i=0;i<params->ToStepArray.size();i++)
			if(params->ToStepArray[i]>0)
				blockIni.push_back(LS(params->ToInitIndexArray[i], params->ToLastIndexArray[i], params->ToStepArray[i]));
			else
				blockIni.push_back(LS(params->ToLastIndexArray[i], params->ToInitIndexArray[i], -params->ToStepArray[i]));
		
		Block writeBlock(blockIni);

		for(i=0;i<MPSProcCount();i++)
		{	Block locBlock(DA_To->DArray_Obj, i, 1);
			Block writeLocBlock = locBlock ^ writeBlock;

//			printf("To Repl = %d  BlockSZ=%d Loc=%d\n", DA_To->DArray_Obj->Repl,writeBlock.GetBlockSize(),locBlock.GetBlockSize());

			if(writeBlock.GetBlockSize()!=0)
				AddTime(__CPU_time_usr, currentVM->map(i), vcall_time[i] * writeLocBlock.GetBlockSize()/writeBlock.GetBlockSize());

			if(writeBlock.GetBlockSize()==0 && DA_To->DArray_Obj->Repl) //я заметил только такой случай, т.е. в трассу попадает -1 индекс, что подразумевает весь массив
				AddTime(__CPU_time_usr, currentVM->map(i), vcall_time[i]);
		}

		if(DA_To->DArray_Obj->Repl) //приводит к дублированию присваиваний на всех остальных процессорах
		{
			for(i=0,j=0;i<MPSProcCount();i++)
			{
				Block locBlock(DA_To->DArray_Obj, i, 1);

				if(j!=0)
					AddTime(__Insuff_parall_usr, currentVM->map(i), vcall_time[i]);

				if(locBlock.GetBlockSize()>0)
					j++;
			}
		}

	}
	else if(DA_From != NULL)
	{
		for(i=0;i<params->FromStepArray.size();i++)
			if(params->FromStepArray[i]>0)
				blockIni.push_back(LS(params->FromInitIndexArray[i], params->FromLastIndexArray[i], params->FromStepArray[i]));
			else
				blockIni.push_back(LS(params->FromLastIndexArray[i], params->FromInitIndexArray[i], -params->FromStepArray[i]));
	
		Block writeBlock(blockIni);

		for(i=0;i<MPSProcCount();i++)
		{ Block locBlock(DA_From->DArray_Obj, i, 1);
			Block writeLocBlock = locBlock ^ writeBlock;

			if(writeBlock.GetBlockSize()!=0)
				AddTime(__CPU_time_usr, currentVM->map(i), vcall_time[i] * writeLocBlock.GetBlockSize()/writeBlock.GetBlockSize());
		}

	}

	if(returned) return;


	MPSSynchronize(__Remote_access);
	AddMPSTime(__Remote_access, f_time);
	++CurrInterval->num_op_remote;
}

void FuncCall::aarrcp()
{
    double f_time = 0.0;
	_DArrayInfo* DA_From	= NULL;
	_DArrayInfo* DA_To		= NULL;
	arrcpy_Info* params		= (arrcpy_Info*) call_params;
	_DArrayFlag* DA_Flags  = AddDArrayFlag(params->CopyFlagPtr);
	
	DA_Flags->ProcessTimeStamp=(double *)malloc(MPSProcCount()*sizeof(double));
	DAF_tmp =  DA_Flags; //====// для передачи его в CommCost::CopyUpdateDistr

	int i;
	for (i=0; i < MPSProcCount(); i++)  
		DAF_tmp->ProcessTimeStamp[currentVM->map(i)]=0;


	if (params->FromBufferPtr == 0)
		DA_From=GetDArrayByIndex(params->FromArrayHeader);
	if (params->ToBufferPtr == 0)
		DA_To=GetDArrayByIndex(params->ToArrayHeader);

    if (DA_From == NULL && DA_To == NULL)
		f_time = 0.0;
    if (DA_From != NULL && DA_To != NULL) {
//cout << DA_From->DArray_Obj->Rank() << ' ' << DA_From->ID << endl;
		// Distributed -> Distributed
        f_time=ArrayCopy(DA_From->DArray_Obj, params->FromInitIndexArray, 
                         params->FromLastIndexArray, params->FromStepArray,
                         DA_To->DArray_Obj, params->ToInitIndexArray, 
                         params->ToLastIndexArray, params->ToStepArray);
	} else if (DA_From == NULL && DA_To != NULL) {
		// Replicated -> Distributed
		f_time = 0.0;
	} else if (DA_From != NULL && DA_To == NULL) {
		// Distributed -> Replicated
        f_time=ArrayCopy(DA_From->DArray_Obj, params->FromInitIndexArray, 
	                     params->FromLastIndexArray, params->FromStepArray,
						 params->CopyRegim);
	}
	DAF_tmp =  NULL; //====//

//	MPSSynchronize(__Remote_access); //aarrcp - не синхронное копирование
	DA_Flags-> time_start = CurrProcTime(0);
	DA_Flags-> time_end = DA_Flags-> time_start + f_time;
	
//	printf("DAF STart=%f DAF End=%f\n",DA_Flags->time_start, DA_Flags->time_end);
	++CurrInterval->num_op_remote;


//printf("F_time=%f\n",f_time);
//	AddMPSTime(__Remote_access, f_time);
}

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)

void FuncCall::waitcp()
{
    int i;
    double curr_pt;

	waitcp_Info* params=(waitcp_Info*) call_params;
    _DArrayFlag * DAF=GetDArrayFlagByIndex(params->CopyFlagPtr);

//	assert(DAF != NULL);
	if (DAF == NULL)
		return;

	for (i=0; i < MPSProcCount(); i++) {

//		printf("TIME stamp[%d]=%f\n",currentVM->map(i),DAF->ProcessTimeStamp[currentVM->map(i)]);
		curr_pt = CurrProcTime(currentVM->map(i));
//		printf("Curr_pt[%d]=%f\n",i,curr_pt);
//		printf("WAIT[%d] Copy [%f]-[%f]=[%f]\n",currentVM->map(i),curr_pt,DAF->time_end,max(DAF->time_end-curr_pt,0));
		if(DAF->ProcessTimeStamp[currentVM->map(i)]>0 && curr_pt < DAF->time_start + DAF->ProcessTimeStamp[currentVM->map(i)]) 
		{
//printf("proc[%d].waitcp\n",currentVM->map(i));
			AddTime(__Remote_access,currentVM->map(i), max(DAF->time_start + DAF->ProcessTimeStamp[currentVM->map(i)] - curr_pt, DAF->ProcessTimeStamp[currentVM->map(i)] ));
			if(curr_pt < DAF->time_start) AddTimeSynchronize(__Synchronize, currentVM->map(i), DAF->time_start - curr_pt);

		} else {
//			if(DAF->ProcessTimeStamp[currentVM->map(i)]>0.00000001) printf("Overlap %f %f\n",curr_pt, DAF->ProcessTimeStamp[currentVM->map(i)]);
//			AddTime(__Remote_overlap,currentVM->map(i), DAF->ProcessTimeStamp[currentVM->map(i)]);
		}
	}

//	printf("Wait cp done****************************************\n");
	DelDArrayFlag(params->CopyFlagPtr);


	++CurrInterval->num_op_remote; //почему-то так в Анализаторе эффективности

}

// Main function

void FuncCall::DArrayTime()
{
    switch (func_id) {
        case crtda_ :
          crtda();
          break;
        case align_ :
          align();
          break;
        case realn_ :
          realn();
          break;
        case delda_ :
          delda();
          break;
        case aarrcp_ :
          aarrcp();
          break;
//====
        case arrcpy_ :
          arrcpy();
          break;
//=***

        case waitcp_ :
          waitcp();
          break;
        default :
          RegularTime(); 
	}
}

