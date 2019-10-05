// CommCost.cpp: implementation of the CommCost class.
//
//////////////////////////////////////////////////////////////////////
#include <assert.h>

#include "CommCost.h"
#include "Vm.h"
#include "Ps.h"

//====
#include "stdio.h"
#include "Interval.h"
#include "LoopBlock.h"
//extern int ShdWid[10];
#include "ModelStructs.h"
extern	_ParLoopInfo ParLoopInfo;
int ShdWid[10];
//=***

using namespace std;

extern VM*	rootVM;			// pointer to root VM
extern VM * currentVM;
extern ofstream prot; 
extern _DArrayFlag * DAF_tmp;


//====
void calculate();

double	TStart, TByte;
LoopBlock** ProcBlock;
long *s,*n,x,y,z,LoopSZ;
int *p,*dmax,k,*conv_beg,*conv_end,add,first;
double time_c,time_x,call_time;
int mode=0 ;  //now mode=0 (no print)    //was mode==0 (only global), 1(global+approach), 2(only approach), 3(global+aproach+no_print)
int full_mode=0; //full_mode==0(as old dvm), 1(max_rank-pipeline calc(full search the best)) 2(different order of cycles for find the best in full search)

int *pip,mult,*mult_is,*mm,rank_mas,max_rank=4; //максимальный ранк моделируемого конвейера
int *ord; //порядок запуска циклов
int invers[10]; // 10 par loop inside each other
//=***

#if defined (_MSC_VER) || (defined (__GNUG__) && (__GNUC__  < 3))
template <class T>

T max(T a, T b)
{
	return a >= b ? a : b;
}
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CommCost::CommCost()
{ 	vm = 0;
	lvector v(0);
	transfer = Dim2Array(0, v);
}

CommCost::~CommCost()
{

}

CommCost::CommCost(VM *Avm)
{
	lvector v(rootVM->GetLSize(), 0);

	vm = Avm;
	assert(vm != 0);

#ifdef P_DEBUG
//	prot << *vm << endl;
#endif
	transfer = Dim2Array(rootVM->GetLSize(), v); // инициализируется нулями

}

void CommCost::SaveSubChannels(ClustInfo *clust)
{
	int i;

	for(i=0; i<clust->channel_time.size(); i++)
		tmp_channels.push_back(clust->channel_time[i]);

	for(i=0; i<clust->SubCluster.size(); i++)
		SaveSubChannels(&clust->SubCluster[i]);
}

void CommCost::RestoreSubChannels(ClustInfo *clust)
{
	int i;

	for(i=0; i<clust->SubCluster.size(); i++)
		RestoreSubChannels(&clust->SubCluster[i]);

	for(i=0; i<clust->channel_time.size(); i++)
	{
		if(tmp_channels.size()<=0) break; //impossible
		clust->channel_time[i]=tmp_channels[tmp_channels.size()-1];
		tmp_channels.pop_back();
	}
}

void CommCost::CommSend(float send_proc_time, long from_id, long to_id, long bytes)
{
	int i;
	double m;
	ClustInfo *CommClust;

//	printf("Wanna send from %d to %d\n",from_id, to_id);
	// must set time of begin and end of Communication

	CommClust=CurrentCluster->GetCommClust(CurrentCluster->map(from_id),CurrentCluster->map(to_id));

//	printf("CurrentCluster.name=%s\n",CurrentCluster->name.c_str());

	if(CommClust!=NULL)
	{
//		printf("ClustComm.name=%s\n",CommClust->name.c_str());

		if(CommClust->channel_time.size()!=0)
			m=CommClust->channel_time[0];

		for(i=1;i<CommClust->channel_time.size();i++)
			if(m>CommClust->channel_time[i])
				m=CommClust->channel_time[i];

		// i = stores id of min
		for(i=0;i<CommClust->channel_time.size();i++)
			if(m==CommClust->channel_time[i])
				break;

		if(i>=CommClust->channel_time.size())
			ClustError(5);

		// поступаем так - посылаем, через самый не заполненный канал. 

		// В принципе иногда более выгодно разбить на параллельные сообщения
		// или занимать минимально не заполненный для этого процессора, то есть чтобы этот проц не ждал.
		// короче говоря, вариантов много.


		if(send_proc_time<m)
			BeginTime=m;
		else
			BeginTime=send_proc_time;

		WaitStart=BeginTime-send_proc_time;
		EndTime=BeginTime + CommClust->TStart + bytes*CommClust->TByte;
		CommClust->channel_time[i] = EndTime;
	}
	else
		ClustError(5);

//	printf("ss %f send %f   %f .. %f\n",send_proc_time, WaitStart, BeginTime, EndTime);
}

void CommCost::Update(DArray * oldDA, DArray * newDA)
{
	long	p1, 
			p2, 
			size;
	Block	b1, 
			b2, 
			bi;
	long	i, 
			j;
	bool transferIs = false;
	vector<long> transferInf((long) vm->Rank(), 0);
	vector<long> SI(2,0L);

	long daRank = oldDA->Rank();
	long amRank = oldDA->AM_Dis->Rank();
	long vmRank = oldDA->AM_Dis->VM_Dis->Rank();
	bool replAxisIs = false;
	long amAxis;
	long num;

/*
	for(i=0; i<MPSProcCount(); i++)
	{
		b1 = Block(oldDA, i,1);
		printf("BLOCK %d:%d %d:%d %d:%d \n",b1.GetLower(0), b1.GetUpper(0),b1.GetLower(1), b1.GetUpper(1),b1.GetLower(2), b1.GetUpper(2));
	}
	for(i=0; i<MPSProcCount(); i++)
	{
		b1 = Block(newDA, i,1);
		printf("NEW BLOCK %d:%d %d:%d %d:%d \n",b1.GetLower(0), b1.GetUpper(0),b1.GetLower(1), b1.GetUpper(1),b1.GetLower(2), b1.GetUpper(2));
	}
*/


	//====
//	printf("Start  %d \n",daRank);	
	
	for(j=0; j<MPSProcCount(); j++)
	{ 
		b1 = Block(oldDA, j,1);
		b2 = Block(newDA, j,1);

		if(b1.empty() && !b2.empty() || b2.empty() && !b1.empty())
			break;


		for(i=0; i<daRank; i++)
		{
			if(b1.GetLower(i)!=b2.GetLower(i) || b1.GetUpper(i)!=b2.GetUpper(i)) break;
		}

		if(i!=daRank) break;
	}
	if(j==MPSProcCount()) //для всех процессоров блоки нового и старого массивов совпадают, => ничего не надо пересылать
	{ //printf("NOTHING to send!!!!!!!!!!!!!!!!!!\n");
		return;
	}

//	printf("Start  %d ok \n",daRank);	

//=***

	vector<char> replAxis(vmRank, 0);

	if (oldDA->Repl) { 
		if (oldDA->AM_Dis == newDA->AM_Dis)
			return;
		for (i = 0; i < vm->Rank(); i++) {
			if (oldDA->AM_Dis->FillArr[i] > newDA->AM_Dis->FillArr[i]) {
				transferIs = true;
			}
			transferInf[i] = oldDA->AM_Dis->FillArr[i] - newDA->AM_Dis->FillArr[i];
		}
		
		if (!transferIs)
			return;

//		printf("GGGOOOOD\n");
//grig		b1 = Block(oldDA, 0);
		b1 = Block(oldDA, 0,1);
//\grig
		// для 1-мерного случая
		if (vm->Rank() == 1) { p1=0;
			for ( p2 = 0; p2 < vm->GetSize(1) - oldDA->AM_Dis->FillArr[0]; p2++) {
//grig				b2 = Block(newDA, p2);
				b2 = Block(newDA, p2,1);
		//\grig
				bi = b1 ^ b2;
				size = bi.GetBlockSize();
				transfer[p1][p2] += size * oldDA->TypeSize; 
			}
			return;
		}

		// для 2-мерного случая
		if (transferInf[0] > 0 && transferInf[1] > 0) {
			// a)
			for (i = 0; i < vm->GetSize(2) - oldDA->AM_Dis->FillArr[1]; i++)
				for (j = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0]; 
					j < vm->GetSize(1) - newDA->AM_Dis->FillArr[0]; j++) {

					SI[0] = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0] - 1;
					SI[1] = i;
					p1 = vm->GetLI(SI);

					SI[0] = j;
					SI[1] = i;
					p2 = vm->GetLI(SI);

//grig					b2 = Block(newDA, p2);
				b2 = Block(newDA, p2,1);
//\grig
					bi = b1 ^ b2;
					size = bi.GetBlockSize();
					transfer[p1][p2] += size * oldDA->TypeSize; 
				}

			// b)
			for ( i = 0; i < vm->GetSize(1) - oldDA->AM_Dis->FillArr[0]; i++)
				for (j = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1]; 
					j < vm->GetSize(2) - newDA->AM_Dis->FillArr[1]; j++) {

					SI[0] = i;
					SI[1] = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1] - 1;
					p1 = vm->GetLI(SI);

					SI[0] = i;
					SI[1] = j;
					p2 = vm->GetLI(SI);
//grig				b2 = Block(newDA, p2);
					b2 = Block(newDA, p2,1);
//\grig

					bi = b1 ^ b2;
					size = bi.GetBlockSize();
					transfer[p1][p2] += size * oldDA->TypeSize; 
				}

			// c)
			SI[0] = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0] - 1;
			SI[1] = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1] - 1;
			p1 = vm->GetLI(SI);

			for (i = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0]; 
				i < vm->GetSize(1) - newDA->AM_Dis->FillArr[0]; i++)

				for (j = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1]; 
					j < vm->GetSize(2) - newDA->AM_Dis->FillArr[1]; j++) {

					SI[0] = i;
					SI[1] = j;
					p2 = vm->GetLI(SI);

//grig					b2 = Block(newDA, p2);
						b2 = Block(newDA, p2,1);
//\grig

					bi = b1 ^ b2;
					size = bi.GetBlockSize();
					transfer[p1][p2] += size * oldDA->TypeSize; 
				}
		} else {
			if (transferInf[0] > 0) {
				for (i = 0; i < vm->GetSize(2) - newDA->AM_Dis->FillArr[1]; i++)
					for (j = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0]; 
						j < vm->GetSize(1) - newDA->AM_Dis->FillArr[0]; j++) {

						SI[0] = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0] - 1;
						SI[1] = i;
						p1 = vm->GetLI(SI);

						SI[0] = j;
						SI[1] = i;
						p2 = vm->GetLI(SI);
//grig					b2 = Block(newDA, p2);
					b2 = Block(newDA, p2,1);
//\grig
						bi = b1 ^ b2;
						size = bi.GetBlockSize();
						transfer[p1][p2] += size * oldDA->TypeSize; 
					}
			} else {
				for ( i = 0; i < vm->GetSize(1) - newDA->AM_Dis->FillArr[0]; i++)
					for (j = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1]; 
						j < vm->GetSize(2) - newDA->AM_Dis->FillArr[1]; j++) {

						SI[0] = i;
						SI[1] = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1] - 1;
						p1 = vm->GetLI(SI);

						SI[0] = i;
						SI[1] = j;
						p2 = vm->GetLI(SI);
					
//grig						b2 = Block(newDA, p2);
							b2 = Block(newDA, p2,1);
//\grig

						bi = b1 ^ b2;
						size = bi.GetBlockSize();
						transfer[p1][p2] += size * oldDA->TypeSize; 
					}
			}
		}
		return;
	}


	for (i = 0; i < vmRank; i++)
	{
		switch (oldDA->AM_Dis->DistRule[amRank + i].Attr)
		{
		case map_REPLICATE :
			replAxis[i] = 1;
			replAxisIs = true;
			break;
		case map_NORMVMAXIS :
			amAxis = oldDA->AM_Dis->DistRule[amRank + i].Axis;
			switch (oldDA->AlignRule[daRank + amAxis - 1].Attr)
			{
			case align_REPLICATE :
				replAxis[i] = 1;
				replAxisIs = true;
				break;
			case align_BOUNDREPL :
				replAxis[i] = 2; // Здесь нужна дополнительная информация - номера процессоров в этом измерении на которые DArray размножен
				replAxisIs = true;
			}
			break;
		}
	}
//			printf("UPDATE GOOD\n");

	// для 2-мерного случая(считаю, что BOUNDREPL для 1-мерной машины нет, 
	// для 2-мерной пока тоже) следовательно REPLICATE только по одному какому-то измерению
	if (replAxisIs) {
		if (replAxis[0]) {
			// размножен по 1-му измерению
//			printf("UPDATE 1DIM\n");

			for (j = 0; j < vm->GetSize(2); j++) {
//grig				b1 = Block(oldDA, vm->GetSpecLI(0, 2, j));
						b1 = Block(oldDA, vm->GetSpecLI(0, 2, j),1);
//\grig

				for (p2 = 0; p2 < vm->GetLSize(); p2++) {
//grig					b2 = Block(newDA, p2);
			b2 = Block(newDA, p2,1);
	//\grig
					bi = b1 ^ b2;
					num = vm->GetNumInDim(p2, 1);
					printf("Num=%d Block=%d-%d %d-%d %d-%d \n",num,b1.GetLower(0),b1.GetUpper(0),b1.GetLower(1),b1.GetUpper(1),b1.GetLower(2),b1.GetUpper(2));
					if (num > (vm->GetSize(1) - oldDA->AM_Dis->FillArr[0] - 1))
						SI[0] = vm->GetSize(1) - oldDA->AM_Dis->FillArr[0] - 1;
					else
						SI[0] = num;
					SI[1] = j;

					printf("j=%d p2=%d SI=[%d %d]\n",j,p2,SI[0],SI[1]);
					p1 = vm->GetLI(SI);
					if (p1 != p2)
						transfer[p1][p2] += bi.GetBlockSize() * oldDA->TypeSize;
				}
			}
		} else {
			for (j = 0; j < vm->GetSize(1); j++) {
//grig				b1 = Block(oldDA, vm->GetSpecLI(0, 1, j));
				b1 = Block(oldDA, vm->GetSpecLI(0, 1, j),1);
//\grig

				for (p2 = 0; p2 < vm->GetLSize(); p2++) {
//grig
//				b2 = Block(newDA, p2);
					b2 = Block(newDA, p2,1);

//\grig
					bi = b1 ^ b2;
					num = vm->GetNumInDim(p2, 2);
					if (num > (vm->GetSize(2) - oldDA->AM_Dis->FillArr[1] - 1))
						SI[1] = vm->GetSize(2) - oldDA->AM_Dis->FillArr[1] - 1;
					else
						SI[1] = num;
					SI[0] = j;					
					p1 = vm->GetLI(SI);
					if (p1 != p2)
						transfer[p1][p2] += bi.GetBlockSize() * oldDA->TypeSize;
				}
			}
		}
		return;
	}

	// случай когда массив до пере... ни как не размножен ни по одному из измерений
	// (в общем виде)
	for (p1 = 0; p1 < vm->GetLSize(); p1++) {
//grig		b1 = Block(oldDA, p1);
		b1 = Block(oldDA, p1,1);

//\grig

		for (p2 = 0; p2 < vm->GetLSize(); p2++) {
			if (p1 != p2) {
//grig				b2 = Block(newDA, p2);
					b2 = Block(newDA, p2,1);
//\grig

				bi = b1 ^ b2;
				size = bi.GetBlockSize();
				transfer[p1][p2] += size * oldDA->TypeSize;  // update
			}
		}
	}
}

CommCost & CommCost::operator =(const CommCost & cc)
{
    this->transfer = cc.transfer;
	this->vm = cc.vm;
	return *this;
}

double CommCost::GetCost()
{
	bool flag=0;
	double	cost = 0.0, min_start, max_end;
	long	p1, 
			p2;
	long	Distance, 
			maxDistance = 0;	// текущее и максимальное растояние
								// (длина минимального пути между процессорами) 
								// между процессорами
	long	Byte, 
			maxByte = 0;		// -//- число пересылаемых байтов
	long	s;					// размер пересылаемой по конвейеру порции данных(в байтах)
	long	k;					// число порций
	long	e;					// остаток
	int		c = 0;				// 0 - если остаток = 0, 1 - иначе

	long	LSize = rootVM->GetLSize();
//	assert(vm != NULL);
//	double	TStart = vm->getTStart();
//	double	TByte = vm->getTByte();

if(0||mode)
{
	printf("TRANSFER\n");
	for (p1 = 0; p1 < LSize; p1++) 
	{ for (p2 = 0; p2 < LSize; p2++) 
			printf("%d\t",transfer[p1][p2]);
		printf("\n");
	}
}

	for (p1 = 0; p1 < LSize; p1++)
		for (p2 = 0; p2 < LSize; p2++) 
			if ((p1 != p2) && (transfer[p1][p2] != 0))
			{
				CommSend(p1,p2,transfer[p1][p2]);
//				printf("ss[%d->%d] %f send %f   %f .. %f  pure_comm=%f\n",p1,p2,CurrProcTime(currentVM->map(p1))/*CurrInterval->GetProcPred(currentVM->map(p1),_Execution_time)*/, WaitStart, BeginTime, EndTime, EndTime-BeginTime);

				if(!flag)
				{
					flag=1;
					min_start=BeginTime-WaitStart;
					max_end=EndTime;
				}
				else
				{
					if(BeginTime<min_start)
						min_start=BeginTime-WaitStart;
					if(EndTime>max_end)
						max_end=EndTime;
				}
//				cost+= EndTime - BeginTime + WaitStart;

				if(DAF_tmp!=NULL) 
				{
					if(	CurrProcTime(currentVM->map(p1)) + DAF_tmp->ProcessTimeStamp[p1] < EndTime)
						DAF_tmp->ProcessTimeStamp[p1] = EndTime - CurrProcTime(currentVM->map(p1));

					if(	CurrProcTime(currentVM->map(p2)) + DAF_tmp->ProcessTimeStamp[p2] < EndTime)
						DAF_tmp->ProcessTimeStamp[p2] = EndTime - CurrProcTime(currentVM->map(p2));

//					DAF_tmp->ProcessTimeStamp[p1]+=EndTime - BeginTime + WaitStart;
//					DAF_tmp->ProcessTimeStamp[p2]+=EndTime - BeginTime + WaitStart;
				}
			}


	cost=max_end-min_start;

	if(DAF_tmp!=NULL) 
		DAF_tmp->time_start=min_start;

//	printf("Cost =%f\n",cost);

	return cost;
}


//===========================================================================
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)

#define mbeg(i,j) ((j<rank_mas)?ProcBlock[i]->LSDim[j].Lower:0)
#define mend(i,j) ((j<rank_mas)?ProcBlock[i]->LSDim[j].Upper:0)
#define mstep(i,j) ((j<rank_mas)?ProcBlock[i]->LSDim[j].Step:1)
#define msize(i,j) ((j<rank_mas)?(ProcBlock[i]->LSDim[j].Upper - ProcBlock[i]->LSDim[j].Lower + 1) / ProcBlock[i]->LSDim[j].Step:1)

#define bsize(i,j,z) ((msize(i,j)%z)?msize(i,j)/z+1:msize(i,j)/z)

#define for_calc(n,k,beg,end) for(n[k]=invers[k]?end:beg; invers[k]?n[k]>=beg:n[k]<=end; invers[k]?n[k]--:n[k]++)

#define ShdWid(k) ((!invers[k])?ParLoopInfo.SGnew->BoundGroup_Obj->dimInfo[k].LeftBSize : (ParLoopInfo.across_SGR!=0?ParLoopInfo.SG->BoundGroup_Obj->dimInfo[k].RightBSize:1))
#define PreShdWid(k) (invers[k]?ParLoopInfo.SGnew->BoundGroup_Obj->dimInfo[k].LeftBSize : (ParLoopInfo.across_SGR!=0?ParLoopInfo.SG->BoundGroup_Obj->dimInfo[k].RightBSize:1))

//internal functions
double calc_comm(long i) 
{ long k,m; double res; long *save_n;

  save_n=(long *)malloc(max_rank*sizeof(long));
	if (i==max_rank) 
	{ for(k=1,m=ShdWid[n[0]];k<max_rank;k++)
			m*=n[k];
	  return TStart+m*TByte;
	}
  else 
	{ for(k=i;k<max_rank;k++)
	    save_n[k]=n[k];

	  m=n[i];
		n[i]=s[m];
		res=(dmax[m]/s[m])*calc_comm(i+1);
	  
		//restore n[i+1..]
		for(k=i+1;k<max_rank;k++)
	    n[k]=save_n[k];
		n[i]=dmax[m]%s[m];
		if(n[i]) res+=calc_comm(i+1);
		return res;
	}
}

void calculate_all_pipes()
{ int old_add=add;  
	int k,*n,*first_pipe;
	int *old_conv_beg ,*old_conv_end,*old_Shd_Wid;
	n=(int *)malloc(max_rank*sizeof(int));
	first_pipe=(int *)malloc(max_rank*sizeof(int));
	old_conv_beg=(int *)malloc(max_rank*sizeof(int));
	old_conv_end=(int *)malloc(max_rank*sizeof(int));
	old_Shd_Wid=(int *)malloc(max_rank*sizeof(int));

	add=old_add;

	for(k=0; k<max_rank;k++)
	{ old_conv_beg[k]=conv_beg[k]; 
		old_conv_end[k]=conv_end[k];
		old_Shd_Wid[k]=ShdWid[k];
		first_pipe[k]=1;
	}
		

	for(n[0]=0; n[0]<(mult_is[0]?p[0]:1); n[0]++)
	for(n[1]=0; n[1]<(mult_is[1]?p[1]:1); n[1]++)
	for(n[2]=0; n[2]<(mult_is[2]?p[2]:1); n[2]++)
	for(n[3]=0; n[3]<(mult_is[3]?p[3]:1); n[3]++)
	{ for(k=0; k<max_rank;k++)
			if(mult_is[k]) 
			{ conv_beg[k]=n[k]; 
				conv_end[k]=n[k];
				if(mult_is[k]==1 && first_pipe[k]==0 && add==1) 
					add=2;
			}
			else 
			{ conv_beg[k]=old_conv_beg[k]; 
				conv_end[k]=old_conv_end[k];
			}

		if(add==2 && mode) printf("Insuff.par.usr..."); 
		if(mode) printf("Pipe[%d %d %d %d] using procs %d-%d %d-%d %d-%d %d-%d\n",n[0],n[1],n[2],n[3],conv_beg[0],conv_end[0],conv_beg[1],conv_end[1],conv_beg[2],conv_end[2],conv_beg[3],conv_end[3]);
	  for(k=0;k<max_rank;k++)  
			pip[k]=conv_end[k]-conv_beg[k]+1;
//		if(mode) printf("Konv procs=%d %d %d %d\n",pip[0],pip[1],pip[2],pip[3]);

		calculate();

		for(k=0; k<max_rank;k++)
			ShdWid[k]=old_Shd_Wid[k];

		for(k=0; k<max_rank;k++)
			if(mult_is[k]==1 && first_pipe[k]==1) first_pipe[k]=0;
	}
	if(mode) printf("\n");

}


void calculate()
{ int i,j,d,k;
  float cur,cur_beg;
	float **comm;
  long *prev,*b,*post;

//mode=0;
		
//ord[0]=1; ord[1]=0; ord[2]=2; ord[3]=3; 
//s[0]=1; s[1]=11; s[2]=1; s[3]=1;

  prev=(long *)malloc(max_rank*sizeof(long));
  post=(long *)malloc(max_rank*sizeof(long));
  n=(long *)malloc(max_rank*sizeof(long));
  b=(long *)malloc(max_rank*sizeof(long));
	
	comm=(float **)malloc(MPSProcCount()*sizeof(float *));
	for(i=0;i<MPSProcCount();i++)
    if(ProcBlock[i]->GetRank())
		  comm[i]=(float *)malloc(max_rank*sizeof(float));

//printf("Step %d %d %d\n",mstep(0,0),mstep(0,1),mstep(0,2));
//printf("Invers %d %d %d\n",invers[0],invers[1],invers[2]);

	if(rank_mas>=2) //was 3
	{
		float *****a,m,mwait,*com;
		float **prev_comm, sz[4][4][4][4]; //max_rank=4; хранит размеры блоков текущего процессора
		float beg,step,last;
		int ind_beg; 
		double time_beg;
		double last_real_comm, real_comm;

		int pip_ord[4]; //для корректного вычисления prev и post

		com=(float *)malloc(MPSProcCount()*sizeof(float));


		prev_comm=(float **)malloc(max_rank*sizeof(float *));
		for(i=0;i<rank_mas;i++)
		  prev_comm[i]=(float *)malloc(rank_mas*sizeof(float));

		//почему 4: 1-2-3=начало а по 2-3 можно спрогнозировать конец=4
		a=(float *****)malloc(MPSProcCount()*sizeof(float ****));
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				a[i]=(float ****)malloc(4*sizeof(float ***));
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
					a[i][n[0]]=(float ***)malloc(4*sizeof(float **));
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
				for(n[1]=0;n[1]<4;n[1]++)
					a[i][n[0]][n[1]]=(float **)malloc(4*sizeof(float *));
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
				for(n[1]=0;n[1]<4;n[1]++)
				for(n[2]=0;n[2]<4;n[2]++)
					a[i][n[0]][n[1]][n[2]]=(float *)malloc(4*sizeof(float));

//		printf("ms %d %d %d \n",bsize(0,0,s[0]),bsize(0,1,s[1]),bsize(0,2,s[2]));
//	printf("ALL=%d\n",LoopSZ);

		//опять лишь предварительный обсчет, точнее печать
	if(call_time/LoopSZ>0.0000000001 && mode) printf("Send/Exec=%.f\n",(TStart+TByte)/(call_time/LoopSZ));

		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<min(4,bsize(i,ord[0],s[ord[0]]));n[0]++)
				for(n[1]=0;n[1]<min(4,bsize(i,ord[1],s[ord[1]]));n[1]++)
				for(n[2]=0;n[2]<min(4,bsize(i,ord[2],s[ord[2]]));n[2]++)
				for(n[3]=0;n[3]<min(4,bsize(i,ord[3],s[ord[3]]));n[3]++)
				{ a[i][n[0]][n[1]][n[2]][n[3]]=1;
				  for(j=0;j<4;j++)
						if(n[j]<min(3,bsize(i,ord[j],s[ord[j]])-1)) a[i][n[0]][n[1]][n[2]][n[3]]*=s[j];
							else a[i][n[0]][n[1]][n[2]][n[3]]*=(msize(i,j)%s[j])?msize(i,j)%s[j]:s[j];
//			  printf("%d-%d-%d-%d-%d=%.f\n",i,n[0],n[1],n[2],n[3],a[i][n[0]][n[1]][n[2]][n[3]]);
				}

		for(i=0;i<MPSProcCount();i++) 
		{ com[i]=0;
			if(ProcBlock[i]->GetRank())
				for(k=0;k<max_rank;k++) 
					comm[i][k]=0;
		}


		//==== предварительный обсчет
		
	for(i=0,time_c=0;i<max_rank;i++)
	{	for(k=0;k<max_rank;k++)
			n[k]=ord[(i+k)%max_rank];
		for(k=1,m=pip[n[0]]-1;k<max_rank;k++)
		  m*=pip[n[k]];

		if(m) time_c+=m*calc_comm(1);
	}
  if(mode) printf("Communication::time_c=%.10f\n",time_c);

	for(i=0,m=0;i<MPSProcCount();i++)
    if(ProcBlock[i]->GetRank()) m++;




//printf("Time_comm=%f\n",time_c);
//	printf("wanna insuff %f ->> %f - %f  comm=%f\n",call_time, cur*call_time/LoopSZ, call_time/m, time_c);
	if(add)
	{
//		AddMPSTime(__Wait_shadow, time_c/m);
//		AddMPSTime(__CPU_time_usr, call_time/LoopSZ);
//		AddMPSTime(__CPU_time_usr, call_time/(m*mult));
//		AddMPSTime(__Insuff_parall_sys, (cur*call_time/LoopSZ-call_time/m)/mult-time_c/m);
//		AddMPSTime(__CPU_time_sys, cur*call_time/LoopSZ-call_time/m);
	}


		//=*** возможно это не точно но примерно верно рассчитывается объем вычислений и передач, а далее очень точно вычисляется когда закончится общее выполнение


		double real_wait;

		ind_beg=-1;
		for_calc(n,0,conv_beg[0],conv_end[0]) //n[0] пробегает 1 измерение процов конвейера в порядке для вычисления блоков конвейера
		for_calc(n,1,conv_beg[1],conv_end[1]) //n[1] пробегает 2 измерение процов конвейера ...
		for_calc(n,2,conv_beg[2],conv_end[2]) //n[2] пробегает 3 измерение процов конвейера ...
		for_calc(n,3,conv_beg[3],conv_end[3]) //n[3] пробегает 4 измерение процов конвейера ...
		{
			real_wait=0;

			//only comment	i=n[0]*p[1]*p[2]+n[1]*p[2]+n[2]; only comment
			for(k=0,i=0;k<rank_mas;k++)
			{
			  for(j=k+1,d=1;j<rank_mas;j++)
 				  d*=p[j];
				i+=n[k]*d;
			}

			if(add)
			{
				for(j=0,cur=1;j<max_rank;j++)
					cur*=msize(i,j);

//				printf("proc[%d] cpu+=%f\n",i,cur*call_time/LoopSZ);
//				AddTime(__CPU_time_usr, i, cur*call_time/LoopSZ/curr);
//				if(add==2) AddTime(__Insuff_parall_usr, i, cur*call_time/LoopSZ);

			}


		  if(ProcBlock[i]->GetRank()) 
		  {
				for(k=0;k<rank_mas;k++)
				{ 
					for(j=k+1,d=1;j<rank_mas;j++)
						d*=p[j];
				//надо prev == -1 если нет пред. процессора для него по этому измерению, кот. надо ждать
					if(invers[k]) 
						if(n[k]!=conv_end[k]/*pip[k]-1*/ && i+d<MPSProcCount() && ProcBlock[i+d]->GetRank()) prev[k]=i+d;
						else prev[k]=-1;
					else 
						if(n[k]!=conv_beg[k]/*0*/ && i-d>=0 && ProcBlock[i-d]->GetRank()) prev[k]=i-d;
						else prev[k]=-1;

					if(!invers[k]) 
						if(n[k]!=conv_end[k]/*pip[k]-1*/ && i+d<MPSProcCount() && ProcBlock[i+d]->GetRank()) post[k]=i+d;
						else post[k]=-1;
					else 
						if(n[k]!=conv_beg[k]/*0*/ && i-d>=0 && ProcBlock[i-d]->GetRank()) post[k]=i-d;
						else post[k]=-1;
				}
				

				if(ind_beg==-1)
				{ 
//					int tmp_shd_wid[4];

					for(k=0;k<max_rank;k++)
					{
//						tmp_shd_wid[k]=ShdWid[k];
//						ShdWid[k]=0;
						pip_ord[k]=-1;
					}

					for(k=0,d=1;k<rank_mas;k++)
					{
						if(post[k]!=-1)	
							for(j=0;j<rank_mas;j++)
								if(mbeg(i,j)!=mbeg(post[k],j) || mend(i,j)!=mend(post[k],j))
								{ //ShdWid[k]=tmp_shd_wid[j];
									pip_ord[k]=j;
								}
					}
				}
			if(mode) {printf("PIPE ORDER [ProcDim -> ArrDim]"); {for(k=0;k<max_rank;k++)if(pip_ord[k]!=-1)printf("  %d -> %d   ",k,pip_ord[k]);}	printf("\n"); }

			{ int old_prev[4],old_post[4];
				for(k=0;k<max_rank;k++)
				{
					old_prev[k]=prev[k];
					old_post[k]=post[k];
					prev[k]=-1;
					post[k]=-1;
				}
				for(k=0;k<max_rank;k++)
				{
					if(pip_ord[k]!=-1)
					{ prev[pip_ord[k]]=old_prev[k];
						post[pip_ord[k]]=old_post[k];
					}
				}

			}

			if(mode) printf("WasPrev %2d= %2d %2d %2d\n",i,prev[0],prev[1],prev[2]);
			if(mode) printf("WasPost %2d= %2d %2d %2d\n",i,post[0],post[1],post[2]);
			if(mode) printf("ShdWid  %2d= %2d %2d %2d\n",i,ShdWid[0],ShdWid[1],ShdWid[2]);

			if(ind_beg==-1) {ind_beg=i; time_beg=CurrProcTime(ind_beg);}

			/*
			for(k=0;k<4;k++)
				if(mm[k]==1) 
				{
					for(j=3;j>k;j--)
					{
						prev[j]=prev[j-1];
						post[j]=post[j-1];
					}
					prev[k]=-1;
					post[k]=-1;
				}
			if(mode) printf("WillPrev %2d= %2d %2d %2d\n",i,prev[0],prev[1],prev[2]);
			if(mode) printf("WillPost %2d= %2d %2d %2d\n",i,post[0],post[1],post[2]);
*/
//			printf("LZ=%f\n",call_time/LoopSZ*1000000);


//printf("proc[%d] time=%f beg=%f cur_beg=%.f\n",i,CurrProcTime(i),time_beg,(CurrProcTime(i)-time_beg)*LoopSZ/call_time);


//				for(j=0;j<max_rank;j++)  prev_size[j]=0;

//				cur=0; //так было
				cur=(CurrProcTime(i)-time_beg)*LoopSZ/call_time; //возможно и отрицательный но потом после первой передачи все встанет на свои места
				cur_beg=cur;
				real_comm=0; last_real_comm=0;
				d=0;
				// макрос 'aа' заменяет (с+1)-тый индекс в выражении a[prev][b[0]][b[1]][b[2]][b[3]] на индекс 'p'
				#define a_change(prev,c,p) ((c==0)?a[prev][p][b[1]][b[2]][b[3]]:(c==1)?a[prev][b[0]][p][b[2]][b[3]]:(c==2)?a[prev][b[0]][b[1]][p][b[3]]:a[prev][b[0]][b[1]][b[2]][p])
				#define a_b(i) a[i][b[0]][b[1]][b[2]][b[3]]

//				printf("MIN %d %d %d %d\n",min(3,bsize(i,ord[0],s[ord[0]])-1),min(3,bsize(i,ord[1],s[ord[1]])-1),min(3,bsize(i,ord[2],s[ord[2]])-1),min(3,bsize(i,ord[3],s[ord[3]])-1));
//				printf("Invers %d %d %d %d\n",invers[ord[0]],invers[ord[1]],invers[ord[2]],invers[ord[3]]);
//				printf("ord %d %d %d %d\n",ord[0],ord[1],ord[2],ord[3]);
				// ord[0]==0 && ord[1]==1 && ord[2]==2  ~~~~~~~~     z,x,y ~ 0 1 2 
				for_calc(b,ord[3],0,min(3,bsize(i,ord[3],s[ord[3]])-1)) //b[ord[3]] пробегает блоки вдоль своего измерения в i-том проце
				for_calc(b,ord[2],0,min(3,bsize(i,ord[2],s[ord[2]])-1)) //b[ord[2]] пробегает блоки ...
				for_calc(b,ord[0],0,min(3,bsize(i,ord[0],s[ord[0]])-1)) //b[ord[0]] пробегает блоки ...
				for_calc(b,ord[1],0,min(3,bsize(i,ord[1],s[ord[1]])-1)) //b[ord[1]] пробегает блоки ...
				{ m=cur; mwait=m;
					d=0; //чтобы пройти дальше если невозможно сделать ускоренный пробег
//					if(mode) printf("B=%d %d %d %d\n",b[ord[1]],b[ord[0]],b[ord[2]],b[ord[3]]);

					//ускоренный пробег
				  if(1) //всегда делать [но вообще можно его отлючить и сравнить результаты (с ним & без него)]
				  for(k=0;k<rank_mas;k++) 
					{ 
						if(!invers[ord[k]] && b[ord[k]]==3 || invers[ord[k]] && b[ord[k]]==0 && (bsize(i,ord[k],s[ord[k]])-1>3)) 
						{ 
							b[ord[k]]=invers[ord[k]]?2:1;
							beg=a_b(i); // время завершения вычисления блока 1
							b[ord[k]]=invers[ord[k]]?1:2;
							step=a_b(i)-beg;				// промежуток времени между временами завершения вычислений соседних блоков
							last=sz[b[0]][b[1]][b[2]][b[3]]; 
							b[ord[k]]=invers[ord[k]]?0:3; 

//							if(mode) printf("prev=%.f sz=%.f Beg %.f step=%.f * %d\n",last,a_b(i),beg,step,(bsize(i,ord[k],s[ord[k]])-2));

							sz[b[0]][b[1]][b[2]][b[3]]=a_b(i);
							a_b(i)+=beg+step*(bsize(i,ord[k],s[ord[k]])-2)-last; // расчет времени последнего блока в этом измерении
							cur=a_b(i);

						  for(j=0;j<rank_mas;j++) 
							{ //прогнозируем загрузку коммуникационных каналов

							  // нужно еще скоординировать ее с реальными каналами???!!!
							  // в принципе она есть, но только не тогда когда каналы перегружены.

								comm[i][j]+=(bsize(i,ord[k],s[ord[k]])-3)*prev_comm[ord[k]][j];
//							  printf("comm=== %.f increased by %.f\n",comm[i][j],(bsize(i,ord[k],s[ord[k]])-3)*prev_comm[ord[k]][j]);
								prev_comm[ord[k]][j]=0; //чтобы повышать один раз
							}

							real_comm+=(bsize(i,ord[k],s[ord[k]])-3)*last_real_comm;
//							printf("[last=%f] * %d = [real_comm=%f]\n",last_real_comm,bsize(i,ord[k],s[ord[k]])-3,real_comm);

							
							if(mode) printf("%d-%d-%d-%d-%d==%.f\n",i,b[0],b[1],b[2],b[3],a_b(i));
							d=-1;
							break; //чтобы один раз вычислял этот блок
						}
					}
					//если был сделан ускоренный пробег, то не надо вычислять этот блок еще раз
					if(d<0) continue;
			 
					last_real_comm=0;
					//comm - коммуникационый канал на вход и отвечает за то когда он закончит передачу данных
				  for(k=0;k<rank_mas;k++) 
					{
						CommCost cc;
//						if(mode) printf("Invers=%d Prev[%d]=%d  b[k]=%d\n",invers[k],k,prev[k],b[k]);
						if(!invers[k] && b[k]==0 && prev[k]!=-1) 
						{ 
							double trans;
//так было							trans=TStart+(a_b(i)/min(s[k],msize(i,k))*ShdWid[k])*TByte;
							if(add==0) cc.SaveChannels();
//							printf("Predddd %.0f  sec=%f + %f or %f \n",a_b(prev[k]),a_b(prev[k])*call_time/LoopSZ,  CurrProcTime(0),  CurrProcTime(i)); //was not i but 0
							cc.CommSend(a_b(prev[k])*call_time/LoopSZ + CurrProcTime(0), prev[k], i, (a_b(i)/min(s[k],msize(i,k)))*ShdWid[k] );
							trans=cc.EndTime - cc.BeginTime;// + cc.WaitStart;
							if(add==0) cc.RestoreChannels();

							if(mode) printf("WaitStart=%f\n",cc.WaitStart);
							real_wait+=max(0,cc.EndTime-CurrProcTime(0) - m*call_time/LoopSZ);
							if(mode) printf("Real_wait %f - %f = %f\n",cc.EndTime-CurrProcTime(0), m *call_time/LoopSZ, max(0,cc.EndTime-CurrProcTime(0) - m *call_time/LoopSZ) );

							comm[i][k]=max(comm[i][k],a_change(prev[k],k,min(3,bsize(i,ord[k],s[ord[k]])-1)))+(trans)*LoopSZ/call_time;
								last_real_comm=max(last_real_comm, trans);
						}

						if( invers[k] && b[k]==min(3,bsize(i,ord[k],s[ord[k]])-1) && prev[k]!=-1) 
						{ 
							double trans;
//так было							trans=TStart+(a_b(i)/min(s[k],msize(i,k))*ShdWid[k])*TByte;
							if(add==0) cc.SaveChannels();
							cc.CommSend(a_b(prev[k])*call_time/LoopSZ + CurrProcTime(0), prev[k], i, (a_b(i)/min(s[k],msize(i,k)))*ShdWid[k] );
							trans=cc.EndTime - cc.BeginTime;// + cc.WaitStart;
							if(add==0) cc.RestoreChannels();

							if(mode) printf("WaitStart=%f\n",cc.WaitStart);
							real_wait+=max(0,cc.EndTime-CurrProcTime(0) - m*call_time/LoopSZ); //хотя лучше было бы взять тот процессор с которого должо все начаться, а не нулевой... хотя это очень близко из-за малого разброса времен.
							if(mode) printf("Real_wait %f - %f = %f\n",cc.EndTime-CurrProcTime(0), m *call_time/LoopSZ, max(0,cc.EndTime-CurrProcTime(0) - m *call_time/LoopSZ) );

							comm[i][k]=max(comm[i][k],a_change(prev[k],k,0))+(trans)*LoopSZ/call_time;
								last_real_comm=max(last_real_comm, trans);
						}

/*						//чтобы и отправитель ждал конца передачи данных надо сделать следующее
						if( invers[k] && b[k]==0 && post[k]!=-1		||		!invers[k] && b[k]==min(3,bsize(i,ord[k],s[ord[k]])-1) && post[k]!=-1) 
						{ 	last_real_comm=max(last_real_comm, (TStart+(0*a_b(i)/min(s[k],msize(i,k))*ShdWid[k])*TByte));
						}

*/
					if(mode) printf("Proc[%d] last_real_comm=%f\n",i,last_real_comm);
						
						//у prev_comm есть направление [j] вдоль которого он ищет блоки с номерами 1 и 2
						//для каждого направления он должен посмотреть, как изменялись коммуникационные каналы [k]
						for(j=0;j<max_rank;j++)
							if(!invers[ord[j]] && b[ord[j]]==1 || invers[ord[j]] && b[ord[j]]==2)
								prev_comm[ord[j]][k]=comm[i][k];
						for(j=0;j<max_rank;j++)
							if(!invers[ord[j]] && b[ord[j]]==2 || invers[ord[j]] && b[ord[j]]==1)
							  prev_comm[ord[j]][k]=comm[i][k]-prev_comm[ord[j]][k];
					}

				
					for(k=0;k<rank_mas;k++) 
					{
//						printf("[k=%d] m=%.f comm=%.f\n",k,m,comm[i][k]);
						m=max(m,comm[i][k]);
					}
					mwait=m-mwait;
					com[i]+=mwait*call_time/LoopSZ;
					real_comm+=last_real_comm;
					if(mwait>0) real_comm+=mwait*call_time/LoopSZ;

					//mwait и com[i] делают асинхронную передачу данных (совмещенную с вычислениями - и как результат учитывается только первая передача а остальные совмещаются)
					//last_real_comm и real_comm делает синхронную (не совмещенную с вычислениями - и как результат учитываются все передачи)

			//		if(mwait>=0) printf("%d-%d-%d-%d wait[%d]=%.f [%f sec]  last_real_comm=%f [sync=idle||insuf.sys]=%f\n",b[0],b[1],b[2],b[3],i,mwait,com[i],last_real_comm,com[i]-last_real_comm);

					if(add && com[i]>last_real_comm) 
					{ 
						AddTime(__CPU_time_sys,i,com[i]-last_real_comm);
						AddTime(__Insuff_parall_sys,i,com[i]-last_real_comm);
						com[i]=last_real_comm;
					}

					sz[b[0]][b[1]][b[2]][b[3]]=a_b(i);
					a_b(i)+=m;
					cur=a_b(i);
				  if(mode) printf("%d-%d-%d-%d-%d=%.f\n",i,b[0],b[1],b[2],b[3],cur);
					if(mode) printf("Proc[%d] last_real_comm=%f\n",i,last_real_comm);
				}
			  
//				printf("proc %d-%d-%d-%d done_time=%f\n",n[0],n[1],n[2],n[3],cur*call_time/LoopSZ);

				if(add)
				{
//				AddTime(__Synchronize,i,time_beg+cur*call_time/LoopSZ);

					if(mode) printf("proc[%d] real_wait=%f    wait_shad+= %f (async) || %f (sync)\n", i, real_wait, com[i], real_comm);
//					if(1||mode) printf("proc[%d] wait_shad+= %f\n",i,real_comm);

					AddTime(__Wait_shadow,i,real_wait); //wait channels

					AddTime(__Wait_shadow,i,com[i]);//async
//					AddTime(__Wait_shadow,i,real_comm);//sync

					//					printf("wait[%d]=%f sec   cur_beg=%.f cur=%.f  raznica=%f\n",i,com[i],cur_beg,cur,(cur-cur_beg)*call_time/LoopSZ);
//					AddTime(__CPU_time_usr, i, (cur-cur_beg)*call_time/LoopSZ - com[i]); //call_time/(m*mult)
				}

				/*
				if(mode && com[i]>0.000001)
				{ printf("proc(%d) wait %2.10f sec from [ ",i,com[i]);
				  for(k=0;k<rank_mas;k++)
						if(prev[k]!=-1) printf("%d ",prev[k]);
					printf("]\n");
				}
				*/
		  }
		}

			time_x=cur*call_time/LoopSZ;

//printf("Total=%f Idle=%f\n",cur*call_time/LoopSZ*m,cur*call_time/LoopSZ*m-call_time);

//	printf("Result %d-%d-%d=%d (time=%5.15f)\n",x,y,z,cur,cur*call_time/LoopSZ);
//  printf("cur=%d call=%5.15f LoopSZ=%d\n",cur,call_time,LoopSZ);
//  printf("Result %d-%d-%d=%d (time=%f)\n",x,y,z,cur,cur*call_time/LoopSZ);
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
				for(n[1]=0;n[1]<4;n[1]++)
				for(n[2]=0;n[2]<4;n[2]++)
					free(a[i][n[0]][n[1]][n[2]]);
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
				for(n[1]=0;n[1]<4;n[1]++)
					free(a[i][n[0]][n[1]]);
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				for(n[0]=0;n[0]<4;n[0]++)
					free(a[i][n[0]]);
		for(i=0;i<MPSProcCount();i++)
      if(ProcBlock[i]->GetRank())
				free(a[i]);
    free(a); 
	}

	for(i=0;i<MPSProcCount();i++)
    if(ProcBlock[i]->GetRank())
		  free(comm[i]);
	free(comm);
	free(prev);
	free(n);
	free(b);

//mode=0;
	return;
}


//==== procedure =========================================================================
void CommCost::Across(double call_timeArg, long LoopSZArg, LoopBlock** ProcBlockArg,int type_size)
{
  int i,j,i0;
	int rank,r;
	int *dim;
  int ind[10];
	vector<long> pp;

	ProcBlock=ProcBlockArg;
	call_time=call_timeArg; 
	LoopSZ=LoopSZArg;

//	TStart = vm->getTStart();
//	TByte = vm->getTByte()*type_size;

	this->SaveChannels();
	this->CommSend(0,1,1);
	TStart = this->EndTime - this->BeginTime;
	this->CommSend(0,1,2);
	this->RestoreChannels();

	//эти используются только для предварительных обсчетов и только чтобы не вводить сложность там, где предварительный обсчет будет уже не нужен.
	TByte = this->EndTime - this->BeginTime - TStart;
	TStart -= TByte;

//	printf("TStart=%.10f TByte=%.10f\n",TStart,TByte);
	  
	pp=vm->getSizeArray();
	rank=pp.size();

// печать конфигурации процессорной решетки
	if(mode)
	{
   	printf("VM %d",pp[0]);
		for(i=1;i<rank;i++)
			printf("x%d",pp[i]);
		printf("\n");
	}

//calc invers
  for(i=0;i<MPSProcCount();i++)  
  for(j=0;j<ProcBlock[i]->GetRank();j++)
		if(ProcBlock[i]->LSDim[j].Step<0) {invers[j]=1;ProcBlock[i]->LSDim[j].Step=-ProcBlock[i]->LSDim[j].Step;}
		else invers[j]=0;

//	printf("Inverse %d %d %d\n",invers[0],invers[1],invers[2]);
	  

//calc rank_mas 
  for(i=0,rank_mas=0;i<MPSProcCount();i++)  
  { r=ProcBlock[i]->GetRank();
 		if(rank_mas<r) rank_mas=r;
  }

  	//correct rank if other dims equals 1
	for(i=k=rank_mas;i<rank;i++)
		if(pp[i]!=1) k=i;
	rank=k;


  if(mode) printf("rank=%d rank_mas=%d\n",rank,rank_mas);
  if(rank>rank_mas) {if(mode) printf("Too many dimensions %d>%d\n",rank,rank_mas); exit(1);}
  if(rank_mas>max_rank) {if(mode) printf("Cannot model ACROSS pipeline because of Array rank=%d>%d\n",rank_mas,max_rank); return;}

	s=(long *)malloc(max_rank*sizeof(long));
  p=(int *)malloc(max_rank*sizeof(int));
  dmax=(int *)malloc(max_rank*sizeof(int));
	dim=(int *)malloc(max_rank*sizeof(int));
  conv_beg=(int *)malloc(max_rank*sizeof(int));
  conv_end=(int *)malloc(max_rank*sizeof(int));

  mult_is=(int *)malloc(max_rank*sizeof(int));
	pip=(int *)malloc(max_rank*sizeof(int));
	ord=(int *)malloc(max_rank*sizeof(int));

	for(i=rank;i<max_rank;i++)
		ShdWid[i]=0;
	for(i=0;i<rank;i++)
		ShdWid[i]=ShdWid(i);

	if(mode) printf("COMM ACROSS %5.10f ShdWid=%d %d %d %d\n",call_time,ShdWid[0],ShdWid[1],ShdWid[2],ShdWid[3]);

	for(k=0;k<rank;k++)
		p[k]=pp[k];
//по другим измерения решетка процов имеет ширину 1
	for(k=rank;k<max_rank;k++)
		p[k]=1;

//печать блоков каждого процессора
  if(mode)
  for(i=0;i<MPSProcCount();i++)  
  { 
    r=ProcBlock[i]->GetRank();
 		for(k=0;k<rank_mas;k++)
			ind[k]=i;
		//only comment   i=ind[0]*p[2]*p[1]+ind[1]*p[2]+ind[2]; only comment
		for(k=rank_mas-1;k>=0;k--)
		{
			ind[k]=ind[k]%p[k];
			for(x=0;x<k;x++) 
				ind[x]=ind[x]/p[k];
		}

		printf("[%d",ind[0]);
		for(k=1;k<max_rank;k++)
			if(k>=rank_mas) printf(",0");
			else printf(",%d",ind[k]);
		printf("]%2d. ",i);

    for(j=0;j<r;j++)
			printf("%d:%d:%d ",mbeg(i,j),mend(i,j),mstep(i,j));
		printf("\n");
  }

  for(i=0;i<max_rank;i++) {dmax[i]=1; dim[i]=1;}

  for(j=0;j<MPSProcCount();j++)  
    if(ProcBlock[j]->GetRank()) break;
  first=j; 
  //first proc with data

//calc dmax(max_dim_size)
  for(k=0;k<max_rank;k++)
	for(x=j,dmax[k]=msize(j,k),i=j+1;i<MPSProcCount();i++)  
		if(ProcBlock[i]->GetRank()&& (msize(i,k)>msize(x,k))) 
		{ x=i;
			dmax[k]=msize(i,k);
		}

//calc mult (number of parallelizing pipelines) 
//эта часть помогает вычислить mult в дальнейшем
  if(rank_mas>=2)
  { mult=1; 
    for(k=0;k<max_rank;k++)
			mult_is[k]=0;

	//only comment i=i0*p[2]*p[1]+j0*p[2]+e0; only comment
    for(i=0;i<MPSProcCount();i++)  
      if(ProcBlock[i]->GetRank()) break;
	//i=first proc with data


    // Надо (x=0  i0=p[2]*p[1])   (x=1   i0=p[2])    (x=2   i0=1)
    // это смещения для вычисления соседних процов вдоль какого-то измерения
    for(x=0;x<max_rank;x++)
		{ for(k=x+1,i0=1;k<rank_mas;k++)
					i0=i0*p[k];

			if(p[x]>1 && i+i0<MPSProcCount())
			{
				for(k=0,r=1;k<rank_mas;k++) 
					if (mbeg(i+i0,k)!=mbeg(i,k) || mend(i+i0,k)!=mend(i,k)) 
					{ if(ShdWid[k]!=0) {r=0;break;} 
						else r=2;
					}
					// r==0 (соседние процы по одному измерению имеют разные блоки)    r==1 (одинаковые блоки)
					if(r) { mult*=p[x]; mult_is[x]=r;}
			}
		}
  }

  if(mode) printf("MULT %d %d %d %d = %d\n",mult_is[0],mult_is[1],mult_is[2],mult_is[3],mult);


//	for(i=0;i<rank_mas;i++)
//		if(ShdWid[i]==0) { mult_is[i]=1;}

//  if(mode) printf("MULT + ZeroWidth %d %d %d %d\n",mult_is[0],mult_is[1],mult_is[2],mult_is[3]);

  j=first;
// mm[i]==0 (по i-тому измерению есть конвейер)    ==1 (конвейера нет)
	mm=(int *)malloc(max_rank*sizeof(int));
	for(k=0;k<max_rank;k++)
	  mm[k]=1;

  for(i=0;i<MPSProcCount();i++)  
		{ r=ProcBlock[i]->GetRank();
			for(k=0;k<r;k++)
				if(mbeg(i,k)!=mbeg(j,k) || mend(i,k)!=mend(j,k)) mm[k]=0;
		}


//	for(i=0;i<rank_mas;i++)
//		if(ShdWid[i]==0) { mm[i]=1;}

  if(mode) printf("MM = %d %d %d %d\n",mm[0],mm[1],mm[2],mm[3]);
//	for(i=0;i<max_rank;i++)
//		mult_is[i]=(i<rank_mas)?1:0; //за пределами размерности массива считаем 0 (не размноженными) так по тем измерениям 1 процессор, иначе раньше вылетает с ошибкой пользователя
/*
	for(i=0,j=0;i<rank_mas;i++)
		{
			if(mm[i]==0) 
			{ if(ShdWid[i]==0 && mult_is[j]==0) {mult*=p[j]; mult_is[j++]=2;}
				else mult_is[j++]=0;
			}
			if(mm[i]==1) 
			{ mult_is[j++]=1;}
			
		}
	*/


  // mult_is == 1 (размноженный массив, => insuff.par.usr)			mult_is == 2 (shdwid==0, => параллельные конвейеры)	
  if(mode) printf("MULT %d %d %d %d\n",mult_is[0],mult_is[1],mult_is[2],mult_is[3]);

//вычисляем какие процы участвуют в конвейере! (процы [от i1 до i2][от j1 до j2] )
  i=first;
//conv_beg[0,1,2] ~ i
  for(k=0;k<max_rank;k++)
    conv_beg[k]=i;
  for(k=max_rank-1;k>=0;k--)
  {
    conv_beg[k]=conv_beg[k]%p[k];
    for(x=0;x<k;x++) 
      conv_beg[x]=conv_beg[x]/p[k];
  }

  for(k=0;k<max_rank;k++)
		conv_end[k]=conv_beg[k];

  for(x=0;x<rank_mas;x++)
  { for(k=x+1,i0=1;k<rank_mas;k++)
      i0=i0*p[k];
   
 		for(j=1;j<p[x] && i+i0<MPSProcCount() && ProcBlock[i+i0]->GetRank();j++,i+=i0)
		{ 
			for(k=0,r=1;k<rank_mas;k++) 
  			if (mbeg(i+i0,k)!=mbeg(i,k) || mend(i+i0,k)!=mend(i,k)) {r=0;break;}
			// r==0 (соседние процы по одному измерению имеют разные блоки)    r==1 (одинаковые блоки)
			if(r==0) { conv_end[x]++;}
		}
  }

////вычисление процессоров по которым есть конвейер и заносится в массив 'pip' 
  for(i=0;i<max_rank;i++)  
    pip[i]=conv_end[i]-conv_beg[i]+1;
  if(mode) printf("Konv procs=%d %d %d %d\n",pip[0],pip[1],pip[2],pip[3]);


  if(mode) 
  { printf("We got %d KONV ",mult);
    for(k=0;k<max_rank;k++) 
			if(k>=rank_mas) printf("%d=%d ",conv_beg[k],conv_end[k]);
			else printf("%d:%d ",conv_beg[k],conv_end[k]);
		printf("\n");
  }

  //вычисляем размеры массива который надо выполнить в конвейере
  //only comment   i=ind[0]*p[2]*p[1]+ind[1]*p[2]+ind[2]; only comment
  for(k=0;k<max_rank;k++)
  { for(x=k+1,y=1;x<max_rank;x++)
      y*=p[x];
    dim[k]=mend(conv_end[k]*y,k)-mbeg(conv_beg[k]*y,k)+1;
  }
  if(mode) printf("Dim %d %d %d %d\n",dim[0],dim[1],dim[2],dim[3]);


//!!!!!!!!!!! пока еще не рассмотрен случай когда rank>3
// и когда есть >=2 параллельных конвейера (одинаковые)

//подсчет времени коммуникаций и времени работы процов
  if(rank_mas==1) //одномерный массив, => нет конвейера
  { if(mode) printf("No pipeline\n");
		double a,b,curr_pt;
		k=0;
		a=call_time;
		b=0; //(p[0]-1)*(TStart+ShdWid[0]*TByte);
		for(i=1;i<p[0];i++)
		{
			curr_pt = CurrProcTime(currentVM->map(i));
			this->CommSend(i-1,i,ShdWid[0]);
			//AddTime(__Insuff_parall_usr, currentVM->map(i), i*call_time/p[0]);
			if(curr_pt < this->EndTime)
			{
				AddTime(__Wait_shadow, currentVM->map(i), this->EndTime - curr_pt);

				if(curr_pt > this->BeginTime)
					AddTime(__Shadow_overlap, currentVM->map(i), curr_pt - this->BeginTime);

				//не учитывается WaitStart - ну вроде и не надо
			}

		}
		if(mode) printf("exec=%f comm=%f\n",a,b);
		/*
		for(i=0;i<p[0];i++)
		{
			AddTime(__CPU_time_usr, currentVM->map(i), call_time/p[0]);
		}
		*/
  }
  else 
  { //pipeline in old dvm
		long M,N,P,W,Q;

		double DD,shad;

	// M-конвейеризуемые   N-НЕконв.(квантуемые)   P-кол-во процов для конвейера
		for(i=0,M=1,N=1,P=1;i<rank_mas;i++)
			if(mm[i] || ShdWid[i]==0) {N*=dim[i];P*=pip[i];}
			else { M*=dim[i]; P*=pip[i];}
//			if(mm[i] || ShdWid[i]==0) N*=dim[i];
//			else {M*=dim[i];P*=pip[i];}


		for(i=0,DD=1;i<rank_mas;i++)
			DD*=dmax[i];
		for(i=0,W=0,shad=0;i<rank_mas;i++)
			if(mm[i]==0) shad+=DD/dmax[i]*ShdWid[i]; // есть конвейер по этому измерению
		W=ceil(shad/N);
  
		if(mode) printf("M=%d N=%d W=%d P=%d\n",M,N,W,P);
		if(mode) printf("W=%d (Shad=%5.0f)\n",W,shad);

		double Tc=call_time/LoopSZ;
	//	double Tm=0.000023/500;
	//	double T0wrecv=0.000001;
	//	double Tirecv=0.000001;
	//	double Tisend=0.000001;
		double Tm=3*W*TByte/M; //TByte/300
		int TLen=sizeof(double);
		double T0wrecv=TStart; //используется предварительный его расчет, но он весьма неточен и точного быть не может, потому что нет единого
		double Tirecv=T0wrecv;
		double Tisend=T0wrecv;
		double tt1,tt2;

		if(0)
		{ printf("W=%f  Tc=%f  Nq=%d  M=%d  Tc*M=%f  Tm*W*TLen=%f\n",shad/N,Tc,N,M,Tc*M,Tm*W*TLen);
			printf("(Tc*M)/(Tm*W*TLen)=%f  P*Tm*W*TLen=%f\n",(Tc*M)/(Tm*W*TLen),P*Tm*W*TLen);
			printf("P*(Tirecv+Twrecv)=%f Tc*M+P*Tm*W*TLen=%f\n",P*(Tirecv+T0wrecv),Tc*M+P*Tm*W*TLen);
		}

		if(mode)
		{ //printf("pip=%d %d %d \n",pip[0],pip[1],pip[2]);
			if(W*Tm>0.0000000001) printf("Greater? P=%d or %7.0f\n",P,(M*Tc)/(W*Tm*TLen));
		}
		if(W*Tm>0.0000000001 && P>(M*Tc)/(W*Tm*TLen))
		{ if (T0wrecv>0) Q=sqrt( N*( P*(Tc*M + P*Tm*W*TLen) - 2*Tc*M ) / (P*(Tirecv + T0wrecv)) );
			else Q=-1;
			if (Q>N) Q=N;
			if (Q<1) Q=1;
	//	  tt1 = (Tirecv + T0wrecv)*Q + (N*( P*(Tc*M + P*Tm*W) - 2*Tc*M ) / P )/Q;
	//	  tt2 = Tirecv*P + Tisend*(P - 1) + T0wrecv + N*Tm*W;
			tt1 = (N*(P-2)*Tc*M)/(P*Q);
			tt2 = Tirecv*P + Tisend*(P - 1) + T0wrecv + (Tirecv + T0wrecv)*Q + N*Tm*W*TLen + N*P*Tm*W*TLen/Q;
		}
		else 
		{ if (T0wrecv>0) Q=sqrt( N*(P - 1)*(M*Tc + P*Tm*W*TLen)  / (P*(Tirecv + T0wrecv)) );
			else Q=-1;
			if (Q>N) Q=N;
			if (Q<1) Q=1;
	//	  tt1 = (Tirecv + T0wrecv)*Q + (N*(P - 1)*(Tc*M + P*Tm*W)/P)/Q;
	//	  tt2 = Tirecv*P + Tisend*(P - 1) + T0wrecv + Tc*M*N/P;
			tt1 = (N*(P - 1)*Tc*M)/(P*Q) + Tc*M*N/P;
			tt2 = Tirecv*P + Tisend*(P - 1) + T0wrecv + (Tirecv + T0wrecv)*Q + N*(P-1)*Tm*W*TLen/Q;
		}


//		Q=20;
		if(mode) printf("Q=%d time=%5.10f+%5.10f=%5.10f\n",Q,tt1,tt2,tt1+tt2);

//		{ 
//			long *kk;
//		kk=(long*)malloc(max_rank*sizeof(long));
			for(i=0;i<rank_mas;i++)
				s[i]=dmax[i];

			for(i=rank_mas;i<max_rank;i++)
				s[i]=1;


		// Квантуем по многим измерениям
			for(i=0,N=min(Q,N);i<rank_mas;i++)
			{
//				printf("mm[%d]=%d\n",i,mm[i]);

				if(mm[i] || ShdWid[i]==0) 
				{
					if(N>=dim[i])
					{ s[i]=1;
						if(mode) printf("QQ=%d ",dim[i]);
					}
					else 
					{ s[i]=dim[i]/N;
						if(mode) printf("QQ=%d ",N);
						break;
					}
					N/=dim[i];
				}
			}


//			x=kk[0]; y=kk[1]; z=kk[2];
//			free(kk);
//		}

//	printf("Shd=%d %d %d\n",ShdWid[0],ShdWid[1],ShdWid[2]);

		if(mode) printf("Konv Steps [%d,%d,%d,%d]\n",s[0],s[1],s[2],s[3]);
	//	x=dmax[0];y=1;z=242;
		for(k=0;k<max_rank;k++) // standart order of doing cycles
		  ord[k]=k;

		if(full_mode==0) add=1; //записывает времена в интервал
		calculate_all_pipes();

		if(mode) printf("Time_c=%5.10f time_x=%5.10f\n",time_c,time_x);
//end of pipeline in old dvm

    if (full_mode) // full_mode - searching better than pipe in old dvm
		{ double ttt=1000000000;
			long *best,*ord_best,count,*st;//st=step in full_search

			best=(long *)malloc(max_rank*sizeof(long));
			ord_best=(long *)malloc(max_rank*sizeof(long));
			st=(long *)malloc(max_rank*sizeof(long));
			add=0; // не записывает в интервал, а только ищет наилучший

//			if(full_mode==2) for(i=1;i<max_rank;i++) k*=i;
	//		else k=1;
			//k=сколько надо разных порядков обработать
//			printf("Orders=%d\n",k);

			//лучший результат и порядок по схеме старого dvm конвейера
			for(i=0;i<max_rank;i++)
			{ best[i]=s[i];
			  ord_best[i]=i; 
			}

			ttt=time_x;
			count=0;
			for(ord[0]=0;ord[0]<max_rank;ord[0]++)
			for(ord[1]=0;ord[1]<max_rank;ord[1]++)
			for(ord[2]=0;ord[2]<max_rank;ord[2]++)
			for(ord[3]=0;ord[3]<max_rank;ord[3]++)
			{ if(ord[0]+ord[1]+ord[2]+ord[3]!=6) continue;
				i=ord[0]*ord[1]+ord[2]*ord[3];
				if(i!=6 && i!=3 && i!=2) continue;
				//здесь они уже все разные и занимают все числа [от 0 до max_rank-1]
				//need to check if you increase max_rank!!!
				for(k=0,i=0;k<max_rank;k++)
					if(dmax[ord[k]]==1 && ord[k]!=k) i=1; 
				if(i) continue; //оставить только один порядок по этому измерению

				printf("Ord %d %d %d %d\n",ord[0],ord[1],ord[2],ord[3]);

				//вычисление шага сетки
				for(i=0;i<max_rank;i++)
					st[i]=dmax[ord[i]]/10+1; //размер сетки == 10 по каждому измерению

				if(rank_mas>=2) //full_search
				{ 
//				  printf("Step %d %d %d %d\n",st[0],st[1],st[2],st[3]);
					for(s[0]=1;s[0]<dmax[ord[0]] || s[0]<dmax[ord[0]]+st[0] && (s[0]=dmax[ord[0]]);s[0]+=st[0])
					for(s[1]=1;s[1]<dmax[ord[1]] || s[1]<dmax[ord[1]]+st[1] && (s[1]=dmax[ord[1]]);s[1]+=st[1])
					for(s[2]=1;s[2]<dmax[ord[2]] || s[2]<dmax[ord[2]]+st[2] && (s[2]=dmax[ord[2]]);s[2]+=st[2])
					for(s[3]=1;s[3]<dmax[ord[3]] || s[3]<dmax[ord[3]]+st[3] && (s[3]=dmax[ord[3]]);s[3]+=st[3])
					{ calculate_all_pipes();
						if(time_x<ttt) 
						{ ttt=time_x;
							for(k=0;k<max_rank;k++)
							{
								best[ord[k]]=s[k];
								ord_best[k]=ord[k];
							}
							if(mode) printf("BETTER %d %d %d %d [comm=%.10f] time=%.10f\n",s[ord[0]],s[ord[1]],s[ord[2]],s[ord[3]],time_c,time_x);
						}
						count++;
					}
				}
				if(full_mode==1) {ord[0]=ord[1]=ord[2]=ord[3]=max_rank;} 
			}

			for(k=0;k<max_rank;k++)
			{ ord[k]=ord_best[k];
				s[k]=best[ord[k]];
			}
			if(mode) printf("BEST[%d %d %d %d] %d %d %d %d = %5.10f [tried %d variants]\n",ord[0],ord[1],ord[2],ord[3],s[0],s[1],s[2],s[3],ttt,count);
			add=1; //запись в интервал
			calculate_all_pipes();
		}
  }

	free(pip);
	free(mult_is);
  free(dmax);
	free(conv_beg);
	free(conv_end);
	free(s);
	return;
}
//=**************************************************************************************

void CommCost::BoundUpdate(DArray *daPtr, vector<DimBound> & dimInfo, bool IsCorner)
{
	long	p1, 
			p2;
	long	bound_size, 
			coner_size;
	Block	b;
	long	sizeNoConerDim;

	for (p1 = 0; p1 < vm->GetLSize(); p1++) {

		// Блок массива daPtr, распределенный на процессор p1
//grig		b = Block(daPtr, p1);	
			b = Block(daPtr, p1,1);	
//\grig

		if (b.empty())
			continue;

		vector<DimBound>::iterator first = dimInfo.begin();
		vector<DimBound>::iterator last = dimInfo.end();

		// определение требуемых пересылок для правой и левой границы
		while (first != last) {
			if (b.IsLeft(first->arrDim, 0)) {  
				// есть сосед слева
				if (first->RightBSize > 0) {

					bound_size = b.GetBlockSizeMult(first->arrDim) * first->RightBSize;
					p2 = vm->GetSpecLI(p1, first->vmDim, - first->dir);
					// += так как Update
					if(p2>=0 && p2<vm->GetLSize()) //====//
						transfer[vm->map(p1)][vm->map(p2)] += bound_size * daPtr->TypeSize;
				}
			
			}

			if (b.IsRight(first->arrDim, daPtr->GetSize(first->arrDim) - 1)) {
				// есть сосед справа
				if (first->LeftBSize >0) {

					bound_size = b.GetBlockSizeMult(first->arrDim) * first->LeftBSize;
					p2 = vm->GetSpecLI(p1, first->vmDim, first->dir);

					if(p2>=0 && p2<vm->GetLSize()) //====//
						transfer[vm->map(p1)][vm->map(p2)] += bound_size * daPtr->TypeSize;
				}
			}
			first++;
		}

		// определение необходимых пересылок "угловых" элементов
		// Внимание (только для 2-х мерного случая)!!!
		// Случай включения в границы массива "угловых" элементов 
		// рассмотрен в 2-х мерном варианте(> 2 пока не учитываю)

		if (IsCorner) {

			sizeNoConerDim = b.GetBlockSizeMult2(dimInfo[0].arrDim, dimInfo[1].arrDim);
			if (b.IsLeft(dimInfo[0].arrDim, 0) && b.IsLeft(dimInfo[1].arrDim, 0)) {
				if (dimInfo[0].RightBSize > 0 && dimInfo[1].RightBSize > 0) {

					coner_size = sizeNoConerDim * dimInfo[0].RightBSize * dimInfo[1].RightBSize;
					p2 = vm->GetSpecLI(p1, dimInfo[0].vmDim, - dimInfo[0].dir);
					p2 = vm->GetSpecLI(p2, dimInfo[1].vmDim, - dimInfo[1].dir);
					transfer[vm->map(p1)][vm->map(p2)] += coner_size * daPtr ->TypeSize;
				}
			}

			if (b.IsLeft(dimInfo[0].arrDim, 0) && b.IsRight(dimInfo[1].arrDim, daPtr->GetSize(dimInfo[1].arrDim) - 1)) {
				if (dimInfo[0].RightBSize > 0 && dimInfo[1].LeftBSize > 0) {

					coner_size = sizeNoConerDim * dimInfo[0].RightBSize * dimInfo[1].LeftBSize;
					p2 = vm->GetSpecLI(p1, dimInfo[0].vmDim, - dimInfo[0].dir);
					p2 = vm->GetSpecLI(p2, dimInfo[1].vmDim, dimInfo[1].dir);
					transfer[vm->map(p1)][vm->map(p2)] += coner_size * daPtr ->TypeSize;
				}
			}
			
			if (b.IsRight(dimInfo[0].arrDim, daPtr->GetSize(dimInfo[0].arrDim) - 1) && b.IsLeft(dimInfo[1].arrDim, 0)) {
				if (dimInfo[0].LeftBSize > 0 && dimInfo[1].RightBSize > 0) {

					coner_size = sizeNoConerDim * dimInfo[0].LeftBSize * dimInfo[1].RightBSize;
					p2 = vm->GetSpecLI(p1, dimInfo[0].vmDim, dimInfo[0].dir);
					p2 = vm->GetSpecLI(p2, dimInfo[1].vmDim, - dimInfo[1].dir);
					transfer[vm->map(p1)][vm->map(p2)] += coner_size * daPtr ->TypeSize;
				}
			}
			
			if (b.IsRight(dimInfo[0].arrDim, daPtr->GetSize(dimInfo[0].arrDim) - 1) && b.IsRight(dimInfo[1].arrDim, daPtr->GetSize(dimInfo[1].arrDim) - 1)) {
				if (dimInfo[0].LeftBSize > 0 && dimInfo[1].LeftBSize > 0) {

					coner_size = sizeNoConerDim * dimInfo[0].LeftBSize * dimInfo[1].LeftBSize;
					p2 = vm->GetSpecLI(p1, dimInfo[0].vmDim, dimInfo[0].dir);
					p2 = vm->GetSpecLI(p2, dimInfo[1].vmDim, dimInfo[1].dir);
					transfer[vm->map(p1)][vm->map(p2)] += coner_size * daPtr ->TypeSize;
				}
			}
		}
	}

//	prot << endl;
	#ifdef _TIME_TRACE_
	// потом убрать
	prot << "CommCost::BoundUpdate: transfer" << endl;
	long	pp1,
			pp2;
	for (pp1 = 0; pp1 < rootVM->GetLSize(); pp1++) {
		for (pp2 = 0; pp2 < rootVM->GetLSize(); pp2++) {
			if (transfer[pp1][pp2] != 0)
				prot << '[' << pp1 << ','  << pp2 << "] = " << transfer[pp1][pp2] << "; ";
		}
		prot << endl;
	}
	// потом убрать
	#endif
}


void CommCost::CopyUpdate(DArray * FromArray, Block & readBlock)
{
	long	p1, 
			p2, 
			size;
//====
if(mode) printf("My CopyUpdate\n");
//=***

	for (p1 = 0; p1 < vm->GetLSize(); p1++) {

		Block locBlock(FromArray, p1,1);
//printf("LocBlock^readBlock: ");
		Block bi = locBlock ^ readBlock;
//printf("Loc^readDone\n ");
//printf("loc[%d].0=%d-%d\n",p1,locBlock.GetLower(0),locBlock.GetUpper(0));
//printf("loc[%d].1=%d-%d\n",p1,locBlock.GetLower(1),locBlock.GetUpper(1));
//printf("read.0=%d-%d\n",readBlock.GetLower(0),readBlock.GetUpper(0));
//printf("read.1=%d-%d\n",readBlock.GetLower(1),readBlock.GetUpper(1));
//printf("bi.0=%d-%d\n",bi.GetLower(0),bi.GetUpper(0));
//printf("bi.1=%d-%d\n",bi.GetLower(1),bi.GetUpper(1));
//printf("*********************************************\n");

//		printf("locBlock=%x, readBlock=%x, bi=%x\n", &locBlock, &readBlock, &bi);

		if (!locBlock.empty() && !bi.empty()) {
			// можно оставить только последнее условие         
			// локальная часть читаемого массива не пуста

			size = bi.GetBlockSize();
				
			for (p2 = 0; p2 < vm->GetLSize(); p2++) {
				if (p1 != p2) {
					Block locBlock1(FromArray, p2,1);
//					printf("locBlock1=%x\n", &locBlock1);
//printf("LocBlock1^bi: ");
					if (locBlock1.empty() || (locBlock1 ^ bi).empty()) {
						// последнее условие можно только оставить
						// нет этой части читаемого массива на данном процессоре
						transfer[p1][p2] += size * FromArray->TypeSize;  // update
if(mode) printf("Transfer[%d][%d]+=%d*%d\n",p1,p2,FromArray->TypeSize,size);

					}
				}
			}
		}
	p2 = p1;
	}
}

//====
void CommCost::CopyUpdateDistr(DArray * FromArray, Block &readBlock, long p2)
{
	long	size, p1;
if(mode) printf("***My CopyUpdateDistr***\n");

/*
	for (p2=0; p2 < vm->GetLSize(); p2++) 
	{
			Block locBlock1(FromArray, p2,1);
			if(mode) printf("Proc[%d]=%d-%d %d-%d\n",p2,locBlock1.GetLower(0),locBlock1.GetUpper(0),locBlock1.GetLower(1),locBlock1.GetUpper(1));
//			if(mode) printf("Proc[%d]=%d-%d %d-%d\n",p2,locBlock1.GetLower(0),locBlock1.GetUpper(0),locBlock1.GetLower(1),locBlock1.GetUpper(1),locBlock1.GetLower(2),locBlock1.GetUpper(2));
	}
*/
	for (p1=0; p1 < vm->GetLSize(); p1++) 
	{
		if (p1 != p2) 
		{
			Block locBlock1(FromArray, p1,1);
		
//printf("loc[%d].0=%d-%d\n",p2,locBlock1.GetLower(0),locBlock1.GetUpper(0));
//printf("loc[%d].1=%d-%d\n",p2,locBlock1.GetLower(1),locBlock1.GetUpper(1));
//printf("read[%d].0=%d-%d\n",p2,readBlock.GetLower(0),readBlock.GetUpper(0));
//printf("read[%d].1=%d-%d\n",p2,readBlock.GetLower(1),readBlock.GetUpper(1));
			Block bi2 = locBlock1 ^ readBlock;

			size = bi2.GetBlockSize();

			if (!bi2.empty()) {
//printf("bi2.0=%d-%d\n",bi2.GetLower(0),bi2.GetUpper(0));
//printf("bi2.1=%d-%d\n",bi2.GetLower(1),bi2.GetUpper(1));
//printf("*********************************************\n");
				transfer[p1][p2] += size* FromArray->TypeSize;
if(mode) printf("Transfer[%d][%d]+=%d*%d\n",p1,p2,FromArray->TypeSize,size);
			}
//else
//  printf("bi2.EMPTY\n*********************************************\n");
		}
	}
}

long CommCost::GetLSize() 
{ return vm->GetLSize();
}


//=***
