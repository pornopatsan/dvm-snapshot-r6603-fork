#include <stdlib.h>
#include <assert.h>

#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"
#include "LoopBlock.h"
//====
#include <stdio.h>
extern long TraceProcNum;
//=***

extern _ShadowInfo	*	GetShadowByIndex(long ID);
extern void				DelShadow(long ID);
extern _AMViewInfo	*	GetAMViewByIndex(long ID);
extern _DArrayInfo	*	GetDArrayByIndex(long ID);
extern _ShdGrpInfo	*	GetShdGroupByIndex(long ID);

_ParLoopInfo ParLoopInfo;
int _ParLoopInfo::count = 0;

//grig
LoopBlock * prevLoopBlock=NULL;
//\grig

void FuncCall::crtpl()
{
	crtpl_Info* params=(crtpl_Info*) call_params;
    ParLoopInfo.ID=params->ID;
    ParLoopInfo.Rank=params->Rank;
    ParLoopInfo.AlignType=0;
	ParLoopInfo.PatternType=0;
    ParLoopInfo.PatternID=0;
	ParLoopInfo.exfrst = false;
	ParLoopInfo.imlast = false;
	ParLoopInfo.across = false;
#ifdef nodef
    if(ParLoopInfo.AxisArray)
      free(ParLoopInfo.AxisArray);
    ParLoopInfo.AxisArray=(long*)calloc(ParLoopInfo.Rank,sizeof(long));
	assert(ParLoopInfo.AxisArray != NULL);
#endif
    ParLoopInfo.ParLoop_Obj = new ParLoop(ParLoopInfo.Rank);
}

void FuncCall::endpl()
{
	int i;
	double curr_pt;
	_ShadowInfo* SHD;

	if 	(ParLoopInfo.imlast) {
		SHD=GetShadowByIndex(ParLoopInfo.imlast_SGR);
		for (i=0; i<MPSProcCount(); i++) {

			curr_pt = CurrProcTime(currentVM->map(i));
			if(curr_pt < SHD->time_end) {
				printf("Overlap = %f (%f -%f )\n", curr_pt - SHD->time_start, curr_pt, SHD->time_start);

				AddTime(__Shadow_overlap,currentVM->map(i), (curr_pt - SHD->time_start));
				AddTime(__Wait_shadow,currentVM->map(i), (SHD->time_end - curr_pt));
			} else {
				AddTime(__Shadow_overlap,currentVM->map(i),	(curr_pt - SHD->time_start));
			}
		}
		ParLoopInfo.imlast = false;

		DelShadow(ParLoopInfo.imlast_SGR/*params->ID*/);

	} 
	else if (ParLoopInfo.across) {
	/* не нужно, потому что если исползуется across, то используется синхронное обновления гранями 
	/* (как на в входе (на первой итерации (мне кажется там нельзя задать асинхронный режим))
	/* так и при вычиcлении цикла across), поэтому на выходе из цикла не требуется считать время Shadow.
		SHD=GetShadowByIndex(ParLoopInfo.across_SGR);
		for (i=0; i<MPSProcCount(); i++) {

			curr_pt = CurrProcTime(currentVM->map(i));
			if(curr_pt < SHD->time_end) 
			{
				printf("Overlap across= %f (%f - %f )\n", curr_pt - SHD->time_start, curr_pt, SHD->time_start);

				AddTime(__Shadow_overlap,currentVM->map(i),	(curr_pt - SHD->time_start));
				AddTime(__Wait_shadow,currentVM->map(i), (SHD->time_end - curr_pt));
			} else {

//====
//printf("SHD %f %f\n",curr_pt, SHD->time_start);
//was				AddTime(__Shadow_overlap,currentVM->map(i),	(curr_pt - SHD->time_start));
//=*** 
			}
		}
		*/
	
		ParLoopInfo.across = false;

		if(ParLoopInfo.across_SGR)
			DelShadow(ParLoopInfo.across_SGR);

	}
	delete ParLoopInfo.ParLoop_Obj;
	ParLoopInfo.ParLoop_Obj=NULL;

	//grig
	if(prevLoopBlock!=NULL)
	{
		delete prevLoopBlock;
		prevLoopBlock=NULL;
	}
	//\grig
}

void FuncCall::mappl()
{
	mappl_Info* params = (mappl_Info*) call_params;

	if (params->PatternType == 1) {
		// AMView
		ParLoopInfo.PatternType=1;
		ParLoopInfo.AlignType=1;
		ParLoopInfo.PatternID=params->PatternRef;
		_AMViewInfo* AMV_Info=GetAMViewByIndex(params->PatternRef);
        ParLoopInfo.ParLoop_Obj->MapPL(AMV_Info->AMView_Obj, params->AxisArray, 
			params->CoeffArray, params->ConstArray, params->InInitIndexArray,
				params->InLastIndexArray, params->InStepArray);
	} else if (params->PatternType == 2) {
		// DisArray
		ParLoopInfo.PatternType=2;
		ParLoopInfo.AlignType=2;
		ParLoopInfo.PatternID=params->PatternRefPtr;
		_DArrayInfo* DA_Info=GetDArrayByIndex(params->PatternRefPtr);
        ParLoopInfo.ParLoop_Obj->MapPL(DA_Info->DArray_Obj, params->AxisArray, 
			params->CoeffArray, params->ConstArray, params->InInitIndexArray, 
				params->InLastIndexArray, params->InStepArray);
	} else {
		return;
	}
    ParLoopInfo.AxisArray = params->AxisArray;

}

void FuncCall::dopl()
{
	int		i, j, cnt;
	double	time = 0.0, ip_time = 0.0;
	long	loop_size = ParLoopInfo.ParLoop_Obj->GetLoopSize();
	long	block_size;
	long interceptj;
//====
	int type_size, mode=0;
//=***

 /*   for(i=0;i<MPSProcCount();i++)
	{ 	printf("vcalltime[%d]=%f * Proc %d = %f\n",i,vcall_time[i],TraceProcNum,vcall_time[i]*TraceProcNum);
		vcall_time[i]*=TraceProcNum;
	}
	
//	printf("calltime=%f * Proc %d = %f\n",call_time,TraceProcNum,call_time*TraceProcNum);
	call_time*=TraceProcNum; //number of processors in trace-mode execution
*/


//	printf("DOPL %f\n",call_time);
	if(mode)
	{
		printf("DOPL      ");
		for(i=0;i<MPSProcCount(); i++)
			printf("%f ",CurrProcTime(i));
		printf("\n");
	}

	dopl_full_Info* tmp_params = (dopl_full_Info*)this->call_params;

	if (call_time==0 || loop_size==0) 	return;

	LoopBlock** ProcBlock=(LoopBlock**)calloc(MPSProcCount(),sizeof(LoopBlock*));
	assert(ProcBlock != NULL);

	//построение витков на каждом процессоре
	for(i=0;i<MPSProcCount();i++)  
		ProcBlock[i]=new LoopBlock(ParLoopInfo.ParLoop_Obj, i,1);


//	for(i=0;i<MPSProcCount();i++)  
//		printf("DOPL empty=%d proc[%d]=%d %d   %d %d\n",ProcBlock[i]->empty(),i,ProcBlock[i]->LSDim[0].Lower,ProcBlock[i]->LSDim[0].Upper,ProcBlock[i]->LSDim[1].Lower,ProcBlock[i]->LSDim[1].Upper);

//	printf("across=%d   Ret=%d\n",ParLoopInfo.across, tmp_params->ReturnVar);

	//====
	if(ParLoopInfo.across && tmp_params->ReturnVar==1) 
	{ 

//		printf("PreACROSS\n");

		#define max_rank 4
		#define ShdWid(k) ((!invers[k])?ParLoopInfo.SGnew->BoundGroup_Obj->dimInfo[k].LeftBSize:ParLoopInfo.SG->BoundGroup_Obj->dimInfo[k].RightBSize)
		#define PreShdWid(k) (invers[k]?ParLoopInfo.SGnew->BoundGroup_Obj->dimInfo[k].LeftBSize:ParLoopInfo.SG->BoundGroup_Obj->dimInfo[k].RightBSize)
		#define msize(i,j) ((j<rank_mas)?(ProcBlock[i]->LSDim[j].Upper - ProcBlock[i]->LSDim[j].Lower + 1) / ProcBlock[i]->LSDim[j].Step:1)
		std::vector<long> pp;
		int k,d,rank,j,i,rank_mas,x;
		int invers[max_rank],prev[max_rank],post[max_rank],p[max_rank],n[max_rank];
		double a,sendtime,com_time,real_sync,exectime,overlap;

		pp=currentVM->getSizeArray();
		rank=pp.size();


		for(k=0;k<rank;k++)
			p[k]=pp[k];
		//по другим измерения решетка процов имеет ширину 1
		for(k=rank;k<max_rank;k++)
			p[k]=1; 

		if(k<MPSProcCount())
			rank_mas=ProcBlock[0]->LSDim.size();
		else
			rank_mas=0; //impossible must be

//		rank_mas=rank;

		CommCost cc;

		cc.transfer.resize(MPSProcCount());
		for(i=0;i<MPSProcCount();i++)
			cc.transfer[i].resize(MPSProcCount());

		for(i=0;i<MPSProcCount();i++)
		for(j=0;j<MPSProcCount();j++)
			cc.transfer[i][j]=0;


		for(i=0;i<MPSProcCount();i++)
		{
				for(j=0;j<ParLoopInfo.Rank;j++)
					invers[j]=ParLoopInfo.ParLoop_Obj->Invers[j]; 
				
				for(k=0;k<rank_mas;k++)
					n[k]=i;
				for(k=rank_mas;k<max_rank;k++)
					n[k]=0;

				for(k=max_rank-1;k>=0;k--)
				{
					n[k]=n[k]%p[k];
					for(x=0;x<k;x++) 
						n[x]=n[x]/p[k];
				}

				for(k=0;k<rank;k++)
				{ 
						for(j=k+1,d=1;j<rank;j++)
							d*=p[j];
					//надо prev == -1 если нет пред. процессора для него по этому измерению, кот. надо ждать
						if(invers[k]) 
							if(n[k]!=p[k]-1 && i+d<MPSProcCount()) prev[k]=i+d;
							else prev[k]=-1;
						else 
							if(n[k]!=0 && i-d>=0) prev[k]=i-d;
							else prev[k]=-1;

						if(!invers[k]) 
							if(n[k]!=p[k]-1 && i+d<MPSProcCount()) post[k]=i+d;
							else post[k]=-1;
						else 
							if(n[k]!=0 && i-d>=0) post[k]=i-d;
							else post[k]=-1;
				}
//				printf("PREV %d %d\n",prev[0],prev[1],prev[2],prev[3]);
//				printf("POST %d %d\n",post[0],post[1],post[2],post[3]);

				for(k=0,a=1;k<rank;k++)
					a*=msize(i,k);

	//			for(k=0;k<rank;k++)
	//				printf("SHAD widthNEW[%d]=%d   SHAD width[%d]=%d\n",k,ShdWid(k),k,PreShdWid(k));
				type_size=ParLoopInfo.ParLoop_Obj->AcrossFlag;
			
				
				sendtime=0; com_time=0; real_sync=0; exectime=0; overlap=0;
				for(k=0;k<rank;k++)
				{ 
					if(post[k]!=-1)
					{ 
						double curr_pt;
//						cc.transfer[i][post[k]] += a/msize(i,k)*type_size;
					
						curr_pt=CurrProcTime(currentVM->map(post[k]));

						cc.CommSend(i,post[k],a/msize(i,k)*type_size);

//						printf("Curr_pt=%f  Beg_time=%f   End_time=%f\n",curr_pt, cc.BeginTime, cc.EndTime);
						if(curr_pt < cc.EndTime)
						{
							AddTime(__Wait_shadow, currentVM->map(post[k]), cc.EndTime - curr_pt);

							if(curr_pt > cc.BeginTime)
								AddTime(__Shadow_overlap, currentVM->map(post[k]), curr_pt - cc.BeginTime);

							//не учитывается WaitStart - ну вроде и не надо
						}
					}
				}
			}

//		com_time=cc.GetCost(); //убрал


			//printf("Procs[%d] comm=%f\n",i,com_time);
	//			AddMPSTime(__Shadow_synchronize,my_num,real_sync);

//		AddMPSTime(__Wait_shadow,com_time); // убрал, так как по-моему это не совсем верно??? добавлять всем процам общее время работы

	//			AddMPSTime(__Shadow_overlap,overlap);
	}

	if(ParLoopInfo.across && tmp_params->ReturnVar==0)
	{ 
		double max_time;
		type_size=ParLoopInfo.ParLoop_Obj->AcrossFlag;
		//Если обратный отсчет в цикле то Step должен быть < 0
		for(i=0;i<MPSProcCount();i++)  
		for(j=0;j<ProcBlock[i]->GetRank();j++)  
			if(ParLoopInfo.ParLoop_Obj->Invers[j]==1) ProcBlock[i]->LSDim[j].Step=-ProcBlock[i]->LSDim[j].Step;


		max_time=0;
		for(i=0;i<MPSProcCount();i++)
			max_time=(CurrProcTime(i)>max_time)?CurrProcTime(i):max_time;

		for(i=0;i<MPSProcCount();i++)
		{
//			AddTimeSynchronize(__Synchronize, i, max_time-CurrProcTime(i));

			AddTimeSynchronize(__Wait_shadow, i, max_time-CurrProcTime(i));
//			printf("Sync %f\n",max_time-CurrProcTime(i));
		}

//	  if(mode) printf("DOPL %f ACROSS LoopSZ=%d:%d %d:%d %d:%d\n",call_time,ParLoopInfo.ParLoop_Obj->LowerIndex[0],ParLoopInfo.ParLoop_Obj->HigherIndex[0],ParLoopInfo.ParLoop_Obj->LowerIndex[1],ParLoopInfo.ParLoop_Obj->HigherIndex[1],ParLoopInfo.ParLoop_Obj->LowerIndex[2],ParLoopInfo.ParLoop_Obj->HigherIndex[2]);
	  if(mode) printf("DOPL ACROSS LoopInvers=%d %d %d\n",ParLoopInfo.ParLoop_Obj->Invers[0],ParLoopInfo.ParLoop_Obj->Invers[1],ParLoopInfo.ParLoop_Obj->Invers[2]);
		ParLoopInfo.ParLoop_Obj->AcrossCost->Across(vcall_time[0], ParLoopInfo.ParLoop_Obj->GetLoopSize(),ProcBlock,type_size);

		max_time=0;
		for(i=0;i<MPSProcCount();i++)
			max_time=(CurrProcTime(i)>max_time)?CurrProcTime(i):max_time;
		for(i=0;i<MPSProcCount();i++)
		{
			AddTimeVariation(__Wait_shadow, i, max_time-CurrProcTime(i));
//			printf("time[%d]=%f max=%f TimVar=%f\n",i,CurrProcTime(i),max_time,max_time-CurrProcTime(i));
		}

		for(i=0;i<MPSProcCount();i++) 
		{
			block_size=ProcBlock[i]->GetBlockSize();
//			printf("DOPL[%d]=%d of %d\n",i,block_size,loop_size);
			if(block_size==0) 
				continue;

			time = (vcall_time[i]*((double)block_size/(double)loop_size));//commented grig /currentVM->getProcPower(i);
			AddTime(__CPU_time_usr, currentVM->map(i), time);
     
			cnt=0;
			for (j=0; j<MPSProcCount(); j++)
			{ if( (*(ProcBlock[i])) == (*(ProcBlock[j])) ) 
					cnt++;
			}

//printf("DOPL[%d] time=%f cnt=%d\n",i,time,cnt);
			if (cnt > 1) 
			{ 
				ip_time = time * (((double) cnt - 1.0) / (double) cnt);
				AddTime(__Insuff_parall_usr, currentVM->map(i), ip_time);
			}
		}
 


		for (i=0;i<MPSProcCount();i++)  
			delete ProcBlock[i];
			free(ProcBlock);

		AddMPSTime(__CPU_time_sys, vret_time);
		AddMPSTime(__Insuff_parall_sys, (ret_time  * ((double) MPSProcCount()-1.0) / (double) MPSProcCount()));
		

		if(mode)
		{
			printf("DONE DOPL ");
			for(i=0;i<MPSProcCount(); i++)
				printf("%f ",CurrProcTime(i));
			printf("\n");
		}

		return;
	}
	//else
	//{
	//=***

//grig
	LoopBlock *minipl;

	if(prevLoopBlock!=NULL)
	{	
		minipl= prevLoopBlock;
	
	// проверяем пересечение  блока minipl с локальными блоками процессоров,
	// кооректируем время выполнеения - для каждого процесссора
    for(i=0;i<MPSProcCount();i++) 
		{
        block_size=ProcBlock[i]->GetBlockSize();

		if(block_size==0) 
			continue;
    	interceptj=intersection(*minipl,*ProcBlock[i]); // число элементов в пересечении




		time= ((double)vcall_time[i])*((double)interceptj/(double)minipl->GetBlockSize());
	//\grig


	//currentVM->getProcPower()/*MPSProcPower()*/;
    AddTime(__CPU_time_usr, currentVM->map(i), time);
        
	cnt=0;
	
    for (j=0; j<MPSProcCount(); j++)
		if( (*(ProcBlock[i])) == (*(ProcBlock[j])) ) 
			cnt++;

	    if (cnt > 1) { 
            ip_time = time * (((double) cnt - 1.0) / (double) cnt);
            AddTime(__Insuff_parall_usr, currentVM->map(i), ip_time);
        }

		}	

    for (i=0;i<MPSProcCount();i++)  
		delete ProcBlock[i];
    free(ProcBlock);

    //delete minipl;

    if(tmp_params->Dim.size()!=0)
		{
			delete minipl;
			 std::vector<LoopLS> lstemp;
			//lstemp.resize(tmp_params->Dim.size();
			for(i=0;i<tmp_params->Dim.size();i++)
			{
				lstemp.push_back(LoopLS(tmp_params->Lower[i],tmp_params->Upper[i],tmp_params->Step[i]));	
			}
			// постоение блока выполняющихся на данный момент витков
			prevLoopBlock = new LoopBlock(lstemp);		
			lstemp.resize(0);
		}


		AddMPSTime(__CPU_time_sys, vret_time);
			AddMPSTime(__Insuff_parall_sys,(ret_time  * ((double) MPSProcCount()-1.0) / (double) MPSProcCount()));
		}
	
	//grig
	else
	{
		//grig
		if(tmp_params->Dim.size()!=0)
		{
			std::vector<LoopLS> lstemp;
		
			for(i=0;i<tmp_params->Dim.size();i++)
			{
				lstemp.push_back(LoopLS(tmp_params->Lower[i],tmp_params->Upper[i],tmp_params->Step[i]));	
			}
			// постоение блока выполняющихся на данный момент витков
			prevLoopBlock = new LoopBlock(lstemp);		
			lstemp.resize(0);
		}

		//\grig
		if (call_time==0 || loop_size==0) 
			return;

		LoopBlock** ProcBlock=(LoopBlock**)calloc(MPSProcCount(),sizeof(LoopBlock*));
		assert(ProcBlock != NULL);

		for(i=0;i<MPSProcCount();i++)  
			ProcBlock[i]=new LoopBlock(ParLoopInfo.ParLoop_Obj, i,1);

		
		for(i=0;i<MPSProcCount();i++) 
		{
			block_size=ProcBlock[i]->GetBlockSize();
//			printf("DOPL[%d]=%d of %d\n",i,block_size,loop_size);

			if(block_size==0) 
				continue;
			//grig
			time = (vcall_time[i]*((double)block_size/(double)loop_size));//commented grig /currentVM->getProcPower(i);
			//\grig


			//currentVM->getProcPower()/*MPSProcPower()*/;
			AddTime(__CPU_time_usr, currentVM->map(i), time);
      

			cnt=0;
			for (j=0; j<MPSProcCount(); j++)
			{ 
//				printf("i=%d j=%d  [0] %d %d    %d %d\n",i,j,ProcBlock[i]->LSDim[0].Lower,ProcBlock[i]->LSDim[0].Upper, ProcBlock[j]->LSDim[0].Lower, ProcBlock[j]->LSDim[0].Upper);
//				printf("i=%d j=%d  [1] %d %d    %d %d",i,j,ProcBlock[i]->LSDim[1].Lower,ProcBlock[i]->LSDim[1].Upper, ProcBlock[j]->LSDim[1].Lower, ProcBlock[j]->LSDim[1].Upper);
				if(*(ProcBlock[i]) == *(ProcBlock[j])) 
					cnt++;
//				printf(" cnt=%d\n",cnt);
			}

//printf("DOPL time=%f cnt=%d\n",time,cnt);

			if (cnt > 1) 
			{ 
				ip_time = time * (((double) cnt - 1.0) / (double) cnt);
				AddTime(__Insuff_parall_usr, currentVM->map(i), ip_time);
			}
		}

		for (i=0;i<MPSProcCount();i++)  
			delete ProcBlock[i];
		free(ProcBlock);

		AddMPSTime(__CPU_time_sys, vret_time);
		AddMPSTime(__Insuff_parall_sys, (ret_time  * ((double) MPSProcCount()-1.0) / (double) MPSProcCount()));

	}
		
}

void FuncCall::ParLoopTime()
{
    switch(func_id) {
        case crtpl_ :
          crtpl();
          break;
        case mappl_ :
          mappl();
          break;
        case dopl_ :
          dopl();
          break;
        case endpl_ :
          endpl();
          break;
      }

	if (func_id != dopl_)
		RegularTime();  
}

