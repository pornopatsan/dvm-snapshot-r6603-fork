#include <fstream>
#include <assert.h>

#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"
#include "Vm.h"

using namespace std;

extern ofstream prot;
extern _PSInfo*      GetPSByIndex(long ID);

extern _ParLoopInfo ParLoopInfo;
extern _DArrayFlag * DAF_tmp;
_DArrayInfo*  GetDArrayByIndex(long ID);

_RemAccessInfo	*RemAccess		= NULL;

int _RemAccessInfo::count	= 0;

//-------------------------------- RemAccess --------------------------------------------

int GetRemAccessIndex(long ID)
{
	int i;
    for (i = RemAccess->size() - 1; (i >= 0)  && RemAccess[i].ID!=ID; i--);
    return i;
}

_RemAccessInfo*  GetRemAccessByIndex(long ID)
{
	int i = GetRemAccessIndex(ID);
    return (i>=0) ? &RemAccess[i] : NULL;
}

_RemAccessInfo*  AddRemAccess(long ID)
{
	_RemAccessInfo* tmp;
    int curr_size = RemAccess->size();

    RemAccess=(_RemAccessInfo*)realloc(RemAccess,(curr_size+1)*sizeof(_RemAccessInfo));
	assert(RemAccess != NULL);
	++*RemAccess;
    tmp=&RemAccess[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelRemAccess(long ID)
{
	int idx=GetRemAccessIndex(ID);
    int curr_size = RemAccess->size();
    int i;

    if (idx<0) 
		return;
    delete RemAccess[idx].RemAccess_Obj;
    for (i=idx+1; i<curr_size; i++) {
		RemAccess[i-1]=RemAccess[i];
    }
    RemAccess=(_RemAccessInfo*)realloc(RemAccess,(curr_size-1)*sizeof(_RemAccessInfo));
	assert((RemAccess != NULL) || (curr_size == 1));
	--*RemAccess;
	
}

//--------------------------------------------------------------------------------------------------

void FuncCall::crtrbp()
{
	crtrbp_Info* params=(crtrbp_Info*) call_params;
    _RemAccessInfo* tmp=AddRemAccess(params->ID);
	if (params->PSRef == 0)
		tmp->RemAccess_Obj = new RemAccessBuf(currentVM);
	else {
		_PSInfo*    ps = GetPSByIndex(params->PSRef);
		tmp->RemAccess_Obj = new RemAccessBuf(ps->VM_Obj);
	}
}

void FuncCall::crtrbl()
{
	int i,j;
	CommCost *remCost = new CommCost(currentVM);	
	vector<long> FromInitIndexArray;
	vector<long> FromLastIndexArray;
	vector<long> FromStepIndexArray;
	vector<long> proc_indexes;
	vector <LS> blockIni;

	crtrbl_Info* params=(crtrbl_Info*) call_params;
    _RemAccessInfo* tmp=AddRemAccess(params->BufferHeader);
    _DArrayInfo* ArrFrom=GetDArrayByIndex(params->RemArrayHeader);

//	printf("Create Remote block %x %x\n",params->RemArrayHeader, params->LoopRef);

	LoopBlock** ProcBlock=(LoopBlock**)calloc(MPSProcCount(),sizeof(LoopBlock*));
	assert(ProcBlock != NULL);

	proc_indexes.resize(0);
	//построение витков на каждом процессоре
	for(i=0;i<MPSProcCount();i++)  
	{
		ProcBlock[i]=new LoopBlock(ParLoopInfo.ParLoop_Obj, i,1);
		if(!ProcBlock[i]->empty())
			proc_indexes.push_back(i);
	}
/*
	for(i=0;i<MPSProcCount();i++)  
	{
		printf("Proc[%d] Block=",i);
		for(j=0;j<ProcBlock[i]->LSDim.size();j++)
			printf(" %d..%d",ProcBlock[i]->LSDim[j].Lower,ProcBlock[i]->LSDim[j].Upper);
		printf("\n");
	}

	printf("Array size=");
	for(j=0;j<ArrFrom->DArray_Obj->Rank();j++)
		printf(" %d",ArrFrom->DArray_Obj->GetSize(j+1));
	printf("\n");
*/
	FromInitIndexArray.resize(params->AxisArray.size());
	FromLastIndexArray.resize(params->AxisArray.size());
	FromStepIndexArray.resize(params->AxisArray.size());
	for(j=0;j<params->AxisArray.size();j++)
	{
		if(params->AxisArray[j]==-1)
		{
			FromInitIndexArray[j]=0;
			FromLastIndexArray[j]=ArrFrom->DArray_Obj->GetSize(j+1)-1;
			FromStepIndexArray[j]=1;
		}
		else
		{
			if(params->CoeffArray[j]==0)
			{
				FromInitIndexArray[j]=params->ConstArray[j];
				FromLastIndexArray[j]=params->ConstArray[j];
				FromStepIndexArray[j]=1;
			}
			else
			{
				//dont know yet
			}


		}
	}
/*
	printf("Remote block =");
	for(j=0;j<params->AxisArray.size();j++)
		printf(" %d..%d(st=%d)",FromInitIndexArray[j],FromLastIndexArray[j],FromStepIndexArray[j]);
	printf("  Transfer to procs =");
	for(j=0;j<proc_indexes.size();j++)
		printf(" %d",proc_indexes[j]);
	printf("\n");
*/
	for(j=0;j<FromInitIndexArray.size();j++)
		blockIni.push_back(LS(FromInitIndexArray[j], FromLastIndexArray[j], FromStepIndexArray[j]));
	Block RemBlock(blockIni);
	
	for(i=0;i<proc_indexes.size();i++)
		remCost->CopyUpdateDistr(ArrFrom->DArray_Obj, RemBlock, proc_indexes[i]);

	DAF_tmp = new (_DArrayFlag);
	DAF_tmp->ProcessTimeStamp=(double *)malloc(MPSProcCount()*sizeof(double));
	for (i=0; i < MPSProcCount(); i++)  
		DAF_tmp->ProcessTimeStamp[currentVM->map(i)]=0;

	remCost->GetCost();

	tmp->StartRemoteTimes=(double *)malloc(MPSProcCount()*sizeof(double));
	tmp->EndRemoteTimes=(double *)malloc(MPSProcCount()*sizeof(double));
	for (i=0; i < MPSProcCount(); i++) 
	{
		tmp->EndRemoteTimes[currentVM->map(i)] = DAF_tmp->ProcessTimeStamp[currentVM->map(i)]; // time of communication
//		printf("loadrb[%d]=%f\n",i,tmp->EndRemoteTimes[currentVM->map(i)]);
	}


	free(DAF_tmp->ProcessTimeStamp);
	delete DAF_tmp;
	DAF_tmp=NULL;

	++CurrInterval->num_op_remote; // почему-то так сделано в Анализаторе производительности

}

void FuncCall::loadrb()
{
	loadrb_Info* params=(loadrb_Info*) call_params;
    _RemAccessInfo* tmp=GetRemAccessByIndex(params->ID);
	int i;

	for (i=0; i < MPSProcCount(); i++) 
	{
		tmp->StartRemoteTimes[currentVM->map(i)] = CurrProcTime(currentVM->map(i)); // time of START of communication
		tmp->EndRemoteTimes[currentVM->map(i)] += CurrProcTime(currentVM->map(i)); // time of END of communication
	}

	++CurrInterval->num_op_remote;
}

void FuncCall::waitrb()
{
	waitrb_Info* params=(waitrb_Info*) call_params;
    _RemAccessInfo* tmp=GetRemAccessByIndex(params->ID);
	double curr_pt;
	int i;

	for (i=0; i < MPSProcCount(); i++) 
	{
		curr_pt=CurrProcTime(currentVM->map(i));
//		printf("St=%f curr_pt=%f End=%f\n",tmp->StartRemoteTimes[currentVM->map(i)], curr_pt, tmp->EndRemoteTimes[currentVM->map(i)]);
		if(curr_pt < tmp->EndRemoteTimes[currentVM->map(i)]) 
		{
			AddTime(__Remote_access, currentVM->map(i), tmp->EndRemoteTimes[currentVM->map(i)] - curr_pt);
			AddTime(__Remote_overlap, currentVM->map(i), curr_pt - tmp->StartRemoteTimes[currentVM->map(i)]);
		}
		else
		{
//			printf("RRRR %f %f\n",tmp->EndRemoteTimes[currentVM->map(i)] , tmp->StartRemoteTimes[currentVM->map(i)]);
			AddTime(__Remote_overlap, currentVM->map(i), tmp->EndRemoteTimes[currentVM->map(i)] - tmp->StartRemoteTimes[currentVM->map(i)]);
		}

	}
	++CurrInterval->num_op_remote; // почему-то так сделано в Анализаторе производительности
}

void FuncCall::RemAccessTime()
{
    switch(func_id) {
        case crtrbp_ :
          crtrbp();
          break;
        case crtrbl_ :
          crtrbl();
          break;
        case loadrb_ :
          loadrb();
          break;
        case waitrb_ :
          waitrb();
          break;
      }

	RegularTime();  
}

