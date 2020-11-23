#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <fstream>
#include <stdlib.h>

#include "ModelStructs.h"
#include "FuncCall.h"
#include "CallInfoStructs.h"
#include "Interval.h"
#include "Ps.h"
//====
#include <stdio.h>
//=***


#ifndef _UNIX_
#define max(a,b) ((a<b)?b:a)
#define min(a,b) ((a<b)?a:b)
#endif

using namespace std;

extern ofstream prot;

extern _DArrayInfo	*	GetDArrayByIndex(long ID);


//grig
extern long_vect MinSizesOfAM; // для автоматического поиска
//\grig


long			rootAM_ID = 0;
extern long			currentAM_ID = 0;		// current AM ID 
vector<long>	stackAM;


_PSInfo			*PSInfo			= NULL;
_AMInfo			*AMInfo			= NULL;
_AMViewInfo		*AMViews		= NULL;

int _PSInfo::count		= 0;
int _AMInfo::count		= 0;
int _AMViewInfo::count	= 0;

//------------------------------------ PS ------------------------------------------------

int GetPSIndex(long ID)
{ 
	int i;
// printf("Get PS ID=%d\n",ID);

	for (i = PSInfo->size() - 1; (i >= 0)  && PSInfo[i].ID!=ID; i--);
    return i;
}

_PSInfo*      GetPSByIndex(long ID)
{ 
	int i = GetPSIndex(ID);
    return (i >= 0) ? &PSInfo[i] : NULL;
}

_PSInfo*      AddPS(long ID)
{
	_PSInfo* tmp;
    int curr_size = PSInfo->size();

    PSInfo = (_PSInfo*) realloc(PSInfo,(curr_size + 1) * sizeof(_PSInfo));
	assert(PSInfo != NULL);
	++*PSInfo;
    tmp=&PSInfo[curr_size];
    tmp->ID=ID;
    return tmp;
}

void DelPS(long ID)
{
	int idx=GetPSIndex(ID);
    int curr_size = PSInfo->size();
    int i;

    if (idx<0) 
		return;
    for(i=idx+1; i<curr_size; i++) {
		PSInfo[i-1]=PSInfo[i];
	}

    PSInfo=(_PSInfo*)realloc(PSInfo,(curr_size - 1) * sizeof(_PSInfo));
	assert((PSInfo != NULL) || (curr_size == 1));
	--*PSInfo;
}

//------------------------------------ AM ------------------------------------------------

int GetAMIndex(long ID)
{ 
	int i;
    for (i = AMInfo->size() - 1; (i >= 0)  && AMInfo[i].ID!=ID; i--);
    return i;
}

_AMInfo*      GetAMByIndex(long ID)
{
	int i=GetAMIndex(ID);
    return (i>=0) ? &AMInfo[i] : NULL;
}

_AMInfo*      AddAM(long ID)
{
	_AMInfo* tmp;
    int curr_size = AMInfo->size();

    AMInfo = (_AMInfo*) realloc (AMInfo,(curr_size+1)*sizeof(_AMInfo));
	assert(AMInfo != NULL);
	++*AMInfo;
    tmp=&AMInfo[curr_size];
    tmp->ID = ID;
	tmp->PS_ID = 0;

    return tmp;
}

void DelAM(long ID)
{
	int idx=GetAMIndex(ID);
    int curr_size = AMInfo->size();
    int i;

    if (idx<0) 
		return;
    for(i=idx+1; i<curr_size; i++) {
		AMInfo[i-1]=AMInfo[i];
	}
    AMInfo=(_AMInfo*)realloc(AMInfo,(curr_size-1)*sizeof(_AMInfo));
	assert((AMInfo != NULL) || (curr_size == 1));
	--*AMInfo;
}

//---------------------------------- AMView ----------------------------------------------

int GetAMViewIndex(long ID)
{
	int i;
    for (i = AMViews->size() - 1; (i >= 0)  && AMViews[i].ID!=ID; i--);
    return i;
}

_AMViewInfo*  GetAMViewByIndex(long ID)
{
	int i=GetAMViewIndex(ID);
    return (i>=0) ? &AMViews[i] : NULL;
}

_AMViewInfo*  AddAMView(long ID)
{
	_AMViewInfo* tmp;
    int curr_size = AMViews->size();

    AMViews=(_AMViewInfo*)realloc(AMViews,(curr_size+1)*sizeof(_AMViewInfo));
	assert(AMViews != NULL);
	++*AMViews;
    tmp=&AMViews[curr_size];
    tmp->ID=ID;

    return tmp;
}

void DelAMView(long ID)
{ 
	int idx=GetAMViewIndex(ID);
    int curr_size = AMViews->size();
    int i;

    if (idx<0) 
		return;
    delete AMViews[idx].AMView_Obj;
    for(i=idx+1; i<curr_size; i++) {
		AMViews[i-1]=AMViews[i];
    }
    AMViews=(_AMViewInfo*)realloc(AMViews,(curr_size-1)*sizeof(_AMViewInfo));
	assert((AMViews != NULL) || (curr_size == 1));
	--*AMViews;
}

//------------------------------------------------------------------------------

void FuncCall::crtps()
{

#ifdef nodef
	vector<long>	lb;
	int				AMType;
	double			ATStart;
	double			ATByte;
	double			AProcPower;

//	ps->hardwarePS(AMType, ATStart, ATByte, AProcPower);
#endif
	
	vector<long>	ASizeArray;

	crtps_Info *	params = (crtps_Info*) call_params;
	unsigned sz = params->InitIndexArray.size(); 
	
	ASizeArray.resize(sz);
	for (int i=0; i < sz; i++) 
		ASizeArray[i] = params->LastIndexArray[i] - params->InitIndexArray[i] + 1;

	VM * vm = new VM(params->InitIndexArray, ASizeArray, currentVM);
	_PSInfo		*	psi = AddPS(params->PSRef);
	psi->VM_Obj = vm;

//	prot << *vm << endl;
}

void FuncCall::psview()
{
#ifdef nodef
	vector<long>	lb;
	vector<long>	ASizeArray;
	int				AMType;
	double			ATStart;
	double			ATByte;
	double			AProcPower;
	DoubleVector    AvProcPower;

	ps->hardwarePS(AMType, ATStart, ATByte, AProcPower,AvProcPower);
#endif

	psview_Info *	params = (psview_Info*) call_params;
	VM * vm = new VM(params->SizeArray, currentVM);


	*currentVM = *vm; //====// может поможет разгрести путанницу в количестве процессоров во вложенном vm
	//====// просто потом надо будет менять currentVM обратно на родительский

	_PSInfo		*	psi = AddPS(params->PSRef);
	psi->VM_Obj = vm;

//	prot << *vm << endl;

}

void FuncCall::getps()
{
	static bool root = true;
	_AMInfo* am;
	_PSInfo* ps;
	getps_Info* params = (getps_Info*) call_params;

//printf("try\n");
	if ((params->AMRef == 0) && (currentPS_ID == 0)) {
		// get current PS
		if (currentAM_ID != 0) {
			am = GetAMByIndex(currentAM_ID);
//printf("try 11 %d %d %d\n",am,params,currentAM_ID);
			am->PS_ID = params->PSRef; 
//printf("try 1111111111\n");
			ps = AddPS(params->PSRef);
			ps->VM_Obj = currentVM;
			currentPS_ID = params->PSRef;
			if (root) {
				root = false;
				rootAM_ID = currentAM_ID;
			}
		} else {
			ps = AddPS(params->PSRef);
			currentPS_ID = params->PSRef;
		}
	} else if ((params->AMRef != 0) && (currentAM_ID == params->AMRef)) {
		// get current PS ID
		am = GetAMByIndex(currentAM_ID);
		assert(am != NULL);
		am->PS_ID = params->PSRef; 
		ps = GetPSByIndex(params->PSRef);
		if (ps == NULL) {
			ps = AddPS(params->PSRef);
			assert(currentVM != NULL);
			ps->VM_Obj = currentVM;
		}
		if (root) {
			root = false;
			rootAM_ID = currentAM_ID;
			currentPS_ID = params->PSRef;
//			stackAM.push_back(rootAM_ID);
		}
	} else if (params->AMRef == 0x7fffffff) {
		// get root PS
	}
//printf("try done\n");

}

void FuncCall::delps()
{
	delps_Info* params = (delps_Info*) call_params;
    DelPS(params->ID);
}

#ifdef no_def
/*
long  setpsw(long  PSRefPtr, long  AMViewRefPtr,
                        double  CoordWeightArray[])
{ 
  //SysHandle      *VMSHandlePtr, *AMVHandlePtr;
  //s_VMS          *VMS;
  //AMVIEW       *AMV;
	_PSInfo ps;	
	VM* vm;
	long PSRank;
	AMView * amv;

  long             Rank, i, j, LinInd, elm, VMSize;
  double          PrevSum, Power;
  double         *SumCoordWeight[MAXARRAYDIM];
  double          MinWeight[MAXARRAYDIM]; 
  s_BLOCK         CurrBlock, InitBlock;
  long            Index[MAXARRAYDIM+1];
  double         *CoordWeightPtr = NULL, *CWPtr = NULL;
  s_AMS          *PAMS;

  

	if (PSRefPtr == NULL) {
		// current VM
		vm = currentVM;
	} else {
		ps = GetPSByIndex(PSRefPtr);
		vm = ps->VM_Obj;
	}

	_AMViewInfo* AMV_Info=GetAMViewByIndex(AMViewRefPtr);

	PSRank = vm->Rank();

	// запись в трасссу параметров выхова функции 
	// PSref,AMViewRef


  if(AMViewRefPtr == NULL) 
  {  //      Set coordinate weigths for all abstract machine representations

  }
  else
  {  // Set coordinate weigths for the given representation
 
     // Check if the parent abstract machine is mapped 

     if(PSRefPtr == NULL)
     { 
     }
     else
     {  
     }
  }
 
  Rank = vm->Rank();

  if(CoordWeightArray != NULL && CoordWeightArray[0] == -1.)
  {  
     // Set unit processor weights and their coordinates 
     if(amv)
     {  // Set unit processor weights for the given
        //       abstract machine representation      

        for(i=0; i < Rank; i++)
        {  //VMSize = VMS->Space.Size[i];

           
           //for(j=0; j < VMSize; j++)
           //{  AMV->CoordWeight[i][j] = 1.;
             // AMV->PrevSumCoordWeight[i][j] = (double)j;
           //}
        }
     }
     else
     {  //   Set unit coordinate weights for all
        //   representations ( for processor system) 

        for(i=0; i < Rank; i++)
        {  
           //VMSize = (int)VMS->Space.Size[i];
           for(j=0; j < VMSize; j++)
           { // VMS->CoordWeight[i][j] = 1.;
             // VMS->PrevSumCoordWeight[i][j] = (double)j;
           }
        }
     }

// вывод весов в трассу
	// иначе

      if(CoordWeightArray == NULL || CoordWeightArray[0] == 0. ||
        IsUserPS)
     {  // Set initial processor coordinate weights 

        //if(VMS == MPS_VMS && IsUserPS == 0)
//           CWPtr = CoordWeightList; // coordinate weights, defined
                                    //   for initial processor system
                                    //   at the start 
  //      else
    //       CWPtr = CoordWeight1; //unit coordinate weights for the 
                                   // processor systet that is not 
                                   // initial
     }
     else
        CWPtr = CoordWeightArray;

     // Check and norm processor coordinate weight array 

     for(i=0,LinInd=0,elm=0; i < Rank; i++)
     {   VMSize = VMS->Space.Size[i];
         MinWeight[i] = 1.e7;   // minimal weight in i+1-th dimension 
         elm += VMSize;         // sum of sizes of all dimensions

         for(j=0; j < VMSize; j++,LinInd++)
         { 
           MinWeight[i] = dvm_min(MinWeight[i], CWPtr[LinInd]);
        }
     }

     for(i=0,LinInd=0; i < Rank; i++)
     {   VMSize = VMS->Space.Size[i];

         for(j=0; j < VMSize; j++,LinInd++)
             CoordWeightPtr[LinInd] = CWPtr[LinInd] / MinWeight[i];
     }

     

     for(i=0; i < Rank; i++)
     {  VMSize = VMS->Space.Size[i];

        dvm_AllocArray(double, VMSize, SumCoordWeight[i]);

        for(j=0; j < VMSize; j++)
           SumCoordWeight[i][j] = 0.;
     }

     InitBlock = block_InitFromSpace(&VMS->Space);
     CurrBlock = block_Copy(&InitBlock);
     VMSize = VMS->ProcCount;

     switch(Rank)
     {  case 1:

        for(LinInd=0; LinInd < VMSize; LinInd++)
        { // spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);

           PrevSum = ProcWeightArray[LinInd];

           for(i=0; i < Rank; i++)
               SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;

        case 2:

        for(LinInd=0; LinInd < VMSize; LinInd++)
        {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);

           PrevSum = sqrt(ProcWeightArray[LinInd]);

           for(i=0; i < Rank; i++)
               SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;

        default:

        Power = 1./(double)Rank;

        for(LinInd=0; LinInd < VMSize; LinInd++)
        {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);

           PrevSum = pow(ProcWeightArray[LinInd], Power);

           for(i=0; i < Rank; i++)
               SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;
     }

     // Form processor coordinate weights and array
     //  of integral preceding  processor coordinate
     //        weights for all array dimensions       

     if(AMV)
     {  // Set processor coordinate weights 
        //       for given representation    

        AMV->WeightVMS = VMS; // processor system for which
                              //   coordinate weights are settung
        
        for(i=0,LinInd=0; i < Rank; i++)
        {  VMSize = VMS->Space.Size[i];
           Power = (double)VMS->ProcCount / (double)VMSize;
                                           
           for(j=0,PrevSum=0.; j < VMSize; j++,LinInd++)
           {  AMV->CoordWeight[i][j] = CoordWeightPtr[LinInd] *
              (SumCoordWeight[i][j] / Power);
              AMV->PrevSumCoordWeight[i][j] = PrevSum;
              PrevSum += AMV->CoordWeight[i][j];
           }
        }
     }
     else
     {  //    Set processor coordinate weights for
        //   all representations ( for processor system) 

        for(i=0; i < Rank; i++)
        {  dvm_FreeArray(VMS->CoordWeight[i]);
           dvm_FreeArray(VMS->PrevSumCoordWeight[i]);
           dvm_AllocArray(double, VMS->Space.Size[i],
                          VMS->CoordWeight[i]);
           dvm_AllocArray(double, VMS->Space.Size[i],
                          VMS->PrevSumCoordWeight[i]);
        }

        for(i=0,LinInd=0; i < Rank; i++)
        {  VMSize = VMS->Space.Size[i];
           Power = (double)VMS->ProcCount / (double)VMSize;

           for(j=0,PrevSum=0.; j < VMSize; j++,LinInd++)
           {  VMS->CoordWeight[i][j] = CoordWeightPtr[LinInd] *
              (SumCoordWeight[i][j] / Power);
              VMS->PrevSumCoordWeight[i][j] = PrevSum;
              PrevSum += VMS->CoordWeight[i][j];
           }
        }
     }

     for(i=0; i < Rank; i++)
         dvm_FreeArray(SumCoordWeight[i]);

     if(RTL_TRACE)
     {  if(TstTraceEvent(call_setpsw_))
        {  for(i=0,LinInd=0; i < Rank; i++)
           {  tprintf("CoordWeight[%d]= ", i);

              VMSize = VMS->Space.Size[i];

              for(j=0,elm=0; j < VMSize; j++,elm++,LinInd++)
              {  if(elm == 5)
                 {  elm = 0;
                    tprintf(" \n                ");
                 }

                 if(AMV)
                    tprintf("%4.2lf(%4.2lf) ", AMV->CoordWeight[i][j],
                                               CoordWeightPtr[LinInd]);
                 else
                    tprintf("%4.2lf(%4.2lf) ", VMS->CoordWeight[i][j],
                                               CoordWeightPtr[LinInd]);
              }

              tprintf(" \n");
           }
        }
     }
  

  dvm_FreeArray(CoordWeightPtr);

  if(RTL_TRACE)
     dvm_trace(ret_setpsw_," \n");

  DVMFTimeFinish(ret_setpsw_);
  return  (DVM_RET, 0);
  
}
*/
#endif

void setpsw(long psr,long amvr,double CoordWeightArray[])
{
    _PSInfo *	ps;
	VM      *	vm;
	int PSRank;
	long i,j,jmax,st=0;

	double *calc_weight;
	
	// получить информацию о PS и AMView
	if (psr == 0) {
		// current VM
		vm = currentVM;
	} else {
		ps = GetPSByIndex(psr);
		vm = ps->VM_Obj;
	}

	double Multiplier;// для нормировки процессорных производительностей
	Multiplier=100000.0;
	for(i=0;i<vm->getProcCount();i++)
	{
		if(vm->getProcPower(i) < Multiplier)// ищем минимум
			Multiplier=vm->getProcPower(i);
		//printf("%f \n",vm->getProcPower(i));
	}

//	for(i=0;i<vm->getProcCount();i++)
//	{
//		printf("%f \n",vm->getProcPower(i)/Multiplier);
//	}
	


    PSRank = vm->Rank();
	int LinSize=0;//число элементов во всех измерениях

	for(i=0;i<PSRank;i++)  
	 	 LinSize+=vm->GetSize(i+1);

	//for(i=0;i<LinSize;i++)
	//	printf("debug!!! in CoordWeightarray[%d]=%f\n",i,CoordWeightArray[i]);
	
	// создать массив вычислительных весов
	calc_weight= new double[LinSize];

	for(i=0;i<PSRank;i++)  // norm weights in any directions
	{
 	// нормировать массив 
	double min_weight=1.e7;
	jmax=vm->GetSize(i+1);

	for(j=st;j<st+jmax;j++)
	 min_weight=min_weight >= CoordWeightArray[j] ? CoordWeightArray[j] : min_weight;
		//MIN(min_weight,CoordWeightArray[i]);

	  for(j=st;j<st+jmax;j++)
	  {
		  calc_weight[j]=CoordWeightArray[j]/min_weight;
	//	  printf("temporary calc_weight (after norm)  calc_weight[%d]=%f\n",j,calc_weight[j]);
	  }
	  st+=jmax;
	}

	  _AMViewInfo*	amvinfo = GetAMViewByIndex(amvr);  //надо ли ?
 //_____________________________
	  
	  long* temp_mult;
	  temp_mult = new long[PSRank];

	  for(i=0;i<PSRank;i++)
	  {
		
		   temp_mult[i]=vm->getProcCount()/vm->GetSize(i+1);
	  }


	  //считаем сумму 
	  st=0;
	  double * sumweight;
	  sumweight=new double[LinSize];
	  for(i=0;i<LinSize;i++)
		  sumweight[i]=0.0;

	  long IndIndim;
	  double Power=1.0/(double)PSRank;

	  for(i=0;i<PSRank;i++)//по всем измерениям
	  {
		  jmax=vm->GetSize(i+1);
		  //IndIndim - индекс в измерении PS // = Pi
	//	  printf("Counting weights for %d dimension ...\n",i);

		  for(j=st,IndIndim=0;j<jmax+st;j++,IndIndim++) // для всех элементов данного измерения
		  {
			//  printf(" finding weights for %d element %d dimension\n",j-st,i);
			  for(int indproc=0;indproc<vm->getProcCount();indproc++)
			  {				  
				  std::vector<long> SI;
				  vm->GetSI(indproc,SI);
				  if(SI[i]==IndIndim)
				  {
				   // add to summ
				//	printf("Add to summ info about %d %d %d\n",SI[0],SI[1],SI[2]);
					double tmp=(vm->getProcPower(indproc)/Multiplier);
					sumweight[j]+=pow((vm->getProcPower(indproc)/Multiplier),Power);
				//	printf("procpower[%d][%d][%d] = %f\n",SI[0],SI[1],SI[2],vm->getProcPower(indproc));
				  }
			  }
			  sumweight[j]=sumweight[j]/temp_mult[i];
		  }		  
	//	  printf("     st= %d\n",st);
		  st+=jmax;
	  }


	  std::vector<double> aaa;
	  for(i=0;i<LinSize;i++)
	  {
//		  printf("sumweight[%d]=%f  calc_weight[%d]=%f\n",i,sumweight[i],i,calc_weight[i]);
		  aaa.push_back(sumweight[i]*calc_weight[i]);
//		  printf("weight_opt[%d]=%f\n",i,aaa[i]);
	  }
// установить веса для данного шаблона
//	vm->weights=WeightClass(psr,aaa);
amvinfo->AMView_Obj->weightEl.SetWeights(psr,aaa);

	  
}



void  genblk_(long psr, long am, double * AxisWeightAddr[], long AxisCount,
                         long *DoubleSignPtr)
{ 
	_PSInfo *	ps;
	VM      *	vm;

	long        cwaDim = 0, jmax;
	double     *cwa;
	int         i, j, k, PSRank;
	int        *IntPtr;
	double     *DoublePtr, 
			   *CWPtr;

	if (psr == 0) {
		// current VM
		vm = currentVM;
	} else {
		ps = GetPSByIndex(psr);
		vm = ps->VM_Obj;
	}


	  PSRank = vm->Rank();

	for(i=0; i < PSRank; i++)
		cwaDim += vm->GetSize(i+1);

	cwa = new double[cwaDim];

	if (*DoubleSignPtr) {
		for (i = 0,k = 0; i < AxisCount; i++) {
			jmax = vm->GetSize(i+1);
			DoublePtr   = (double *)AxisWeightAddr[i];

			if(DoublePtr) {
				for(j=0; j < jmax; j++,k++)
					cwa[k] = DoublePtr[j];
			} else {
				for(j=0; j < jmax; j++,k++)
				cwa[k] = 1.0;
				/*CWPtr[k];*/
			}
		}
	} else {
		printf("ERROR: genblk in this version of Predictor not support DoubleSign=0 \n");
		exit(0);
	
	}

  for( ; k < cwaDim; k++)
      cwa[k] = 1.0;
	  //CWPtr[k];

//  for(i=0;i<cwaDim;i++)
//	  printf("cwa[%d]=%f\n",i,cwa[i]);
  setpsw(psr, am, cwa);
}




void 	genbld(long PSRef, long AMViewRef, double **AxisWeightAddr, long AddrNumber)
{ 
	long  DoubleSign = 1;

//grig

	_PSInfo *	ps;
	VM      *	vm;
//получить описание PS
    if (PSRef == 0) {
		// current VM
		vm = currentVM;
	} else {
		ps = GetPSByIndex(PSRef);
		vm = ps->VM_Obj;
	}
	
	std::vector<double> temp_weights;
	double *ArrPtr1;
	int Sum=0;
	int i,j;

	for(i=0;i<AddrNumber;i++)// i - номер измерения PS
	{
		ArrPtr1=*(AxisWeightAddr+i);
		if(ArrPtr1){
		j=vm->GetSize(i+1);
		//создать вектор весов для всех измерений PS(Vm)
        for(j=0;j<vm->GetSize(i+1);j++)
			 temp_weights.push_back(*(ArrPtr1+j));
		}
		else
		{
		j=vm->GetSize(i+1);

        for(j=0;j<vm->GetSize(i+1);j++)
			 temp_weights.push_back(1.0);
		}

	}

//	for(i=0;i<temp_weights.size();i++)
//		printf("temp_weights[%d]=%f\n",i,temp_weights[i]);
	genblk_(PSRef, AMViewRef, AxisWeightAddr, AddrNumber, &DoubleSign);
}

void FuncCall::setelw()
{
	int			i,
				j,
				k,
				cw,
				jmax,
				st,
				AxisSize;
	_PSInfo *	ps;
	VM      *	vm;
	double	*	DoublePtr1;
	double      Wsum, 
				Wmax, 
				Wlow, 
				Whigh, 
				W, 
				Wpre, 
				Wcur; 
	const double setelw_precision=0.001;
	// precision of calculations of processor coordinate weights by setelw_ function

	double **	AxisWeightAddr = NULL;
	setelw_Info* params = (setelw_Info*) call_params;
	long AddrNumber = params->AddrNumber;

	if (params->PSRef == 0) {
		// current VM
		vm = currentVM;
	} else {
		ps = GetPSByIndex(params->PSRef);
		vm = ps->VM_Obj;
	}
	assert(vm != NULL);

	if (AddrNumber) {
		// Memory request for parameters of genbld_function
		AxisWeightAddr = new double *[AddrNumber];

		for (i = 0; i < AddrNumber; i++) {
			
			if (params->WeightNumber[i] != 0 && 
				params->WeightNumber[i] >= (long) vm->GetSize(i+1)) {

				AxisWeightAddr[i] = new double[vm->GetSize(i+1)];
			} else

				AxisWeightAddr[i] = NULL;
		}

		// Find calculated coordinate weights
		// for each dimension of processor system 
		// to provide uniform processor loading

		for (i = 0, st = 0; i < AddrNumber; i++) {
			// Solve optimisation task for (i+1) dimension
			jmax=0;

			if(AxisWeightAddr[i]) {
				// Calculate summary and maximal dimension loading weights

				// jmax - number of loading coordinate weights
				jmax = params->WeightNumber[i] + st;

				AxisSize = (int) vm->GetSize(i + 1);
				// size of dimension of processor system
				DoublePtr1 = (double *) AxisWeightAddr[i];
//				DoublePtr2 = (double *) LoadWeightAddr[i];

				for (j = st, Wsum=0., Wmax=0.; j < jmax; j++) {
//					cout<<"loadweight"<<"["<<j<<"]="<<params->LoadWeight[j]<<"\n";
					Wsum += params->LoadWeight[j];
#ifdef _UNIX_
					Wmax = max(Wmax, params->LoadWeight[j]);
#else
					Wmax = max(Wmax, params->LoadWeight[j]);
#endif
				}

				// Calculate Whigh (maximal calculated weight)

				Wlow = Wsum / (double) AxisSize;    // initial low edge
#ifdef _UNIX_
				Whigh = min(Wsum, Wlow+Wmax);		// initial high edge
#else
				Whigh = min(Wsum, Wlow+Wmax);		// initial high edge
#endif

				while ((Whigh - Wlow) > setelw_precision) {

					// if it is necessary precision
					W = (Whigh + Wlow) * 0.5; // new low or high edge

					// Check if there is a distribution with
					// maximal calculated processor weight equal to W

					for (j = st,k = 0, Wmax=0.; j < jmax; j++) {
						Wcur = params->LoadWeight[j];

						// j-th loading coordinate weight can't keep within dimension
						if (Wcur > W)
							break; 

						Wpre = Wmax + Wcur;

						if (Wpre <= W)
							// calculate weight of k-coordinate do not exceed maximum value
							Wmax = Wpre; 
						else {
							// To the next dimension coordinate
							k++;
							// loading coordinate weights cannot keep within dimension
							if (k == AxisSize)
							break;
							Wmax = Wcur;
						}
					}	// for

					if(j == jmax)
						Whigh = W; // there is a dimention with high edge W
					else
						Wlow = W;  // there is no  dimention with high edge W
				}	// while

				// Count calculated processor coordinate weights

				for(j = 0; j < AxisSize; j++)
					DoublePtr1[j] = params->LoadWeight[j];
				/*
				for(j = 0; j < AxisSize; j++)
					DoublePtr1[j] = 0.;

				for (j = st, k=0, cw = 0, Wmax=0.; j < jmax; j++) {
					Wcur = params->LoadWeight[j];
					Wpre = Wmax + Wcur;

					if(Wpre <= Whigh) {
						Wmax = Wpre; // Demanded weight of coordinate is not reached
						cw++;
					} else {
						// Demanded weight of k-coordinate is reached
						DoublePtr1[k] = cw; // calculated weight of k-coordinate
						k++;       // to the next coordinate
						if(k == AxisSize)
							break;  // out of processor system dimension limits

						Wmax = Wcur;
						cw = 1;
					}
				}	// for

				if (k < AxisSize)
					DoublePtr1[k] = cw; // calculated weight of the last coordinate

				for (k++; k < AxisSize; k++)
					DoublePtr1[k] = 3.4E-38; // weight of dimentions with lack of loading weights
				*/
			}	//if
			st += jmax;
		}	// for
	}	// if

	// Define calculated processor coordinate weights


	genbld(params->PSRef, params->AMViewRef, AxisWeightAddr, AddrNumber);

	  // Free memory of genbld_ parameters

	if (AddrNumber) {
		for(i=0; i < AddrNumber; i++)
			if (AxisWeightAddr[i])
				delete AxisWeightAddr[i];
		delete AxisWeightAddr;
	}
}

void FuncCall::getam()
{
	_AMInfo* am;
	_PSInfo* ps;
	getam_Info* params = (getam_Info*) call_params;



    if (GetAMIndex(params->ID) < 0) 
		AddAM(params->ID);
	if (currentAM_ID == 0) {
		currentAM_ID = params->ID;			// current AM ID 
		if (currentPS_ID != 0) {
			ps = GetPSByIndex(currentPS_ID);
			am = GetAMByIndex(currentAM_ID);
			am->PS_ID = currentPS_ID; 
			ps->VM_Obj = currentVM;
		}
	}
}

void FuncCall::getamr()
{
	getamr_Info* params = (getamr_Info*) call_params;

    if (GetAMIndex(params->AMRef) < 0) 
		AddAM(params->AMRef);
}

void FuncCall::crtamv()
{ 
	crtamv_Info* params = (crtamv_Info*) call_params;
    _AMViewInfo* tmp = AddAMView(params->ID);
    tmp->AM_ID=params->AM_ID;




    tmp->AMView_Obj = new AMView(params->SizeArray);


}

void FuncCall::delamv()
{
	delamv_Info* params = (delamv_Info*) call_params;
    DelAMView(params->ID);
}

void FuncCall::mapam()
{
	mapam_Info* params = (mapam_Info*) call_params;
	_AMInfo*	am = GetAMByIndex(params->AMRef);
	assert(am != NULL);
	am->PS_ID = params->PSRef;
}

void FuncCall::runam()
{
	runam_Info* params = (runam_Info*) call_params;
	_AMInfo*	am = GetAMByIndex(params->ID);

	// change all current settings
	stackAM.push_back(currentAM_ID);
	currentAM_ID = params->ID;
	currentPS_ID = am->PS_ID;
	_PSInfo*	ps = GetPSByIndex(currentPS_ID);
	currentVM = ps->VM_Obj;
	assert(currentVM != NULL);
//	prot << *currentVM << endl;
}

void FuncCall::stopam()
{
	// get last AM and pop it
	currentAM_ID = stackAM[stackAM.size() - 1];
	stackAM.pop_back();

	_AMInfo*	am = GetAMByIndex(currentAM_ID);
	currentPS_ID = am->PS_ID;
	_PSInfo*	ps = GetPSByIndex(currentPS_ID);
	currentVM = ps->VM_Obj;
	assert(currentVM != NULL);

//	prot << *currentVM << endl;
}

//====
void FuncCall::blkdiv()
{
	int i;
	blkdiv_Info*		params = (blkdiv_Info*) call_params;
	_AMViewInfo*	tmp = GetAMViewByIndex(params->ID);

//	printf("We get Block div[%d] ", params->AMVAxisDiv.size());
//	if(params->AMVAxisDiv.size() > 1) printf("= %d %d", params->AMVAxisDiv[0], params->AMVAxisDiv[1]);
//	printf("\n");

	tmp->AMView_Obj->BSize.resize(params->AMVAxisDiv.size());
	for(i=0; i< params->AMVAxisDiv.size(); i++)
		tmp->AMView_Obj->BSize[i] = params->AMVAxisDiv[i];

}
//=***

void FuncCall::distr()
{
    VM*				vm;
	distr_Info*		params = (distr_Info*) call_params;
    _AMViewInfo*	tmp = GetAMViewByIndex(params->ID);
	_PSInfo*		ps = GetPSByIndex(params->PSRef);
//	prot << "ps = " << (void*) ps << endl;

	vm = (ps == NULL) ? currentVM : ps->VM_Obj;
//	prot << *vm << endl;




//grig
	int i;
	if(tmp->AMView_Obj->weightEl.GetSize()==0)
	{
//      printf("In function distr_ AMViewRef = %d  PsRef = %d Variant 1 (weights were not  setted)\n",params->ID,ps);
	  int arrSize=0;
	  for(i=0;i<vm->Rank();i++)
		  arrSize+=vm->GetSize(i+1);
	  double * arr_temp;
	  arr_temp = new double[arrSize];
	  for(i=0;i<arrSize;i++)
		  arr_temp[i]=1.0;

	  setpsw((ps==NULL) ? NULL : ps->ID ,params->ID,arr_temp);
	}
	else{
		
	}
//\grig

	tmp->AMView_Obj->DisAM(vm, params->AxisArray, params->DistrParamArray);

	
}

void FuncCall::RedisTime()
{ 
	double time = 0.;
	redis_Info* params=(redis_Info*) call_params;

	return;

	if (params->ID != 0) {
		_AMViewInfo* AMV=GetAMViewByIndex(params->ID);
		time=AMV->AMView_Obj->RDisAM(params->AxisArray, params->DistrParamArray, params->NewSign);
	} else if (params->AID != 0){
		_DArrayInfo*  DAR=GetDArrayByIndex(params->AID);
		time=DAR->DArray_Obj->RDisDA(params->AxisArray, params->DistrParamArray, params->NewSign);
	}

    MPSSynchronize(__Redistribute);
    AddMPSTime(__Redistribute, time);
	++CurrInterval->num_op_redist;

}


// Main modelling function
void FuncCall::MPS_AMTime()
{
    switch(func_id) {
        case crtps_ :
			crtps();
			break;
		case psview_ :
			psview();
			break;
		case getps_:
			getps();
			break;
		case delps_ :
			delps();
			break;
		case setelw_ :
			setelw();
			break;
        case getam_ :
			getam();
			break;
        case getamr_ :
			getamr();
			break;
        case crtamv_ :
			crtamv();
			break;
        case delamv_ :
			delamv();
			break;
		case mapam_ :
			mapam();
			break;
		case runam_ :
			runam();
			break;
		case stopam_ :
			stopam();
			break;
		case distr_ :
			distr();
			break;
		case blkdiv_ :
			blkdiv();
			break;
	//	case setpsw_ :
	//		break;

      }

	if (func_id == redis_)
          RedisTime();
	else
          RegularTime();  
}


//====
void SetCurrentAm_ID()
{ currentAM_ID = 0;
}
//=***