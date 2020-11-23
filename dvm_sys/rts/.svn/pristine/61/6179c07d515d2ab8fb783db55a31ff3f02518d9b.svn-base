#ifndef _DISTRIB_C_
#define _DISTRIB_C_

#include "system.typ"

/*****************/    /*E0000*/

/**************************************\
* Functions to map an abstract machine *
\**************************************/    /*E0001*/

DvmType  __callstd distr_(AMViewRef  *AMViewRefPtr, PSRef  *PSRefPtr,
                          DvmType  *ParamCountPtr, DvmType  AxisArray[],
                          DvmType  DistrParamArray[])

/*
      Mapping  abstract machine representation.
      -----------------------------------------

*AMViewRefPtr   - reference to the representation of the parental
                  abstract machine.
*PSRefPtr       - reference to the processor system, which defines
                  the structure of the distributed resources.
*ParamCountPtr	- the number of parameters defined in arrays AxisArray
                  and DistrParamArray.
AxisArray	- AxisArray[j] is a dimension number of the abstract
                  machine representation used in mapping rule for
                  processor system (j+1)-th dimension. 
DistrParamArray	- DistrParamArray[j] is a mapping rule parameter
                  for processor system (j+1)th dimension
                  (DistrParamArray[j] is a nonnegative integer). 

The function distr_ distributes resources of the parental abstract
machine among child abstract machines, and the pointer AMViewPtr
defines the representation containing these abstract machines.
The function returns non-zero value if mapped representation
has a local part on the current processor, otherwise returns zero.
*/    /*E0002*/

{ s_AMVIEW       *AMV;
  SysHandle      *AMVHandlePtr, *VMSHandlePtr;
  s_VMS          *VMS;
  int             VMR, AMR, i, j, MapArrSize, ALSize, VMSize, Size;
  DvmType        *AxisArr;
  s_MAP          *MapArray;
  s_ALIGN        *AlList;
  s_BLOCK        *Local, **LocBlockPtr = NULL, Block, *LocBlock = NULL;
  DvmType        MinPar;
  byte            TstArray[MAXARRAYDIM], IsConst = 0;
  double          DisPar;
  s_AMS          *PAMS, *CAMS;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0003*/
  DVMFTimeStart(call_distr_);

  if(RTL_TRACE)
  { if(TstTraceEvent(call_distr_) == 0)
       dvm_trace(call_distr_," \n");
    else
    {
       if(PSRefPtr == NULL || *PSRefPtr == 0)
          dvm_trace(call_distr_,
                    "AMViewRefPtr=%lx; AMViewRef=%lx; PSRefPtr=NULL; "
                    "PSRef=0; ParamCount=%ld;\n",
                    (uLLng)AMViewRefPtr, *AMViewRefPtr, *ParamCountPtr);
       else
          dvm_trace(call_distr_,
                    "AMViewRefPtr=%lx; AMViewRef=%lx; PSRefPtr=%lx; "
                    "PSRef=%lx; ParamCount=%ld;\n",
                    (uLLng)AMViewRefPtr, *AMViewRefPtr, (uLLng)PSRefPtr,
                    *PSRefPtr, *ParamCountPtr);

       for(i=0; i < *ParamCountPtr; i++)
           tprintf("      AxisArray[%d]=%ld; ", i, AxisArray[i]);
       tprintf(" \n");
       for(i=0; i < *ParamCountPtr; i++)
           tprintf("DistrParamArray[%d]=%ld; ", i, DistrParamArray[i]);
       tprintf(" \n");
       tprintf(" \n");
    }
  }

  if(TstObject)
  {
     if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.000: wrong call distr_\n"
             "(the abstract machine representation "
             "is not a DVM object; "
             "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 036.001: wrong call distr_\n"
            "(the object is not an abstract machine representation; "
            "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV  = (s_AMVIEW *)AMVHandlePtr->pP;

  /* Check if the representation has already be mapped */    /*E0004*/

  if(AMV->VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 036.002: wrong call distr_\n"
              "(the representation has already been mapped; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  /* Check if any abstract machine of representation has already be mapped */    /*E0005*/

  for(i=0; i < AMV->AMSColl.Count; i++)
  {
      PAMS = coll_At(s_AMS *, &AMV->AMSColl, i);

      if(PAMS->VMS)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 036.003: wrong call distr_\n"
              "(the daughter abstract machine of the representation "
              "has already been mapped;\nAMViewRef=%lx; "
              "DaughterAMRef=%lx)\n",
              *AMViewRefPtr, (uLLng)PAMS->HandlePtr);
  }

  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent abstract machine */    /*E0006*/
  CAMS = (s_AMS *)CurrAMHandlePtr->pP;  /* current abstract macine */    /*E0007*/

  /* Check if the parent abstract machine has already be mapped */    /*E0008*/

  if(PAMS->VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 036.014: wrong call distr_\n"
              "(the parental AM has not been mapped; "
              "AMViewRef=%lx; ParentAMRef=%lx)\n",
              *AMViewRefPtr, (uLLng)PAMS->HandlePtr);

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {
     VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {
     if(TstObject)
     {  if(!TstDVMObj(PSRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 036.010: wrong call distr_\n"
                    "(the processor system is not a DVM object; "
                    "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.011: wrong call distr_\n"
                 "(the object is not a processor system; PSRef=%lx)\n",
                 *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;
  }

  /* Check if all processors of the processor system
          belong to the parent processor system      */    /*E0009*/

  NotSubsystem(i, PAMS->VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.012: wrong call distr_\n"
             "(the given PS is not a subsystem of the parental PS;\n"
             "PSRef=%lx; ParentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)(PAMS->VMS->HandlePtr));

  /* Check if all processors of the processor system
         belong to the current processor system      */    /*E0010*/

  NotSubsystem(i, DVM_VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.013: wrong call distr_\n"
             "(the given PS is not a subsystem of the current PS;\n"
             "PSRef=%lx; CurrentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);

  VMR = VMS->Space.Rank;
  AMR = AMV->Space.Rank;

for(i=0; i < MAXARRAYDIM; i++)
    AMV->disDiv[i] = AMV->Div[i];

  if(AMV->DivReset)
  {
     AMV->DivReset = 0;

     for(i=0; i < MAXARRAYDIM; i++)
         AMV->Div[i] = 1;
  }
AMV->setCW=1;

  MapArrSize = VMR + AMR;

  if(*ParamCountPtr < 0 || *ParamCountPtr > VMR)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 036.020: wrong call distr_\n"
        "(invalid ParamCount: ParamCount=%ld; PSRef=%lx; PSRank=%d)\n",
        *ParamCountPtr, (uLLng)VMSHandlePtr, VMR);

  dvm_AllocArray(DvmType, VMR, AxisArr);

  for(i=0; i < *ParamCountPtr; i++)
      AxisArr[i] = AxisArray[i];

  for(   ; i < VMR; i++)
      AxisArr[i] = 0;

  for (i = 0; i < MAXARRAYDIM; i++)
      AMV->AMVAxis[i] = 0;
  for (i = 0; i < VMR; i++)
      AMV->AMVAxis[i] = AxisArr[i];

  /* Check correctness of given parameters */    /*E0011*/

  for(i=0; i < VMR; i++)
  {  j = (int)AxisArr[i] - 1;

     if(AxisArr[i] < -1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.021: wrong call distr_\n"
                 "(AxisArray[%d]=%ld < -1; AMViewRef=%lx)\n",
                 i, AxisArr[i], *AMViewRefPtr);

     if(AxisArr[i] > AMR)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.022: wrong call distr_\n"
                 "(AxisArray[%d]=%ld > %d; AMViewRef=%lx)\n",
                 i, AxisArr[i], AMR, *AMViewRefPtr);

     if(AxisArr[i] != 0)
     {
        if(DistrParamArray[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 036.023: wrong call distr_\n"
                    "(DistrParamArray[%d]=%ld < 0; AMViewRef=%lx; "
                    "PSRef=%lx)\n",
                    i, DistrParamArray[i],
                    *AMViewRefPtr, (uLLng)VMSHandlePtr);
     }

     Size = (int)VMS->Space.Size[i] - 1;

     if(AxisArr[i] == -1)
     {
        if(DistrParamArray[i] > Size)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 036.024: wrong call distr_\n"
                    "(DistrParamArray[%d]=%ld >= %d; "
                    "AMViewRef=%lx; PSRef=%lx)\n",
                    i, DistrParamArray[i], Size+1,
                    *AMViewRefPtr, (uLLng)VMSHandlePtr);
     }

     if(AxisArr[i] > 0)
     {
        if(DistrParamArray[i] > 0)
        {
           if(VMS == AMV->WeightVMS)
              MinPar = (DvmType)((double)AMV->Space.Size[j] /
                              (AMV->PrevSumCoordWeight[i][Size] +
                               AMV->CoordWeight[i][Size]));
           else
              MinPar = (DvmType)((double)AMV->Space.Size[j] /
                              (VMS->PrevSumCoordWeight[i][Size] +
                               VMS->CoordWeight[i][Size]));

           if(DistrParamArray[i] < MinPar)
              epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                       "*** RTS err 036.025: wrong call distr_\n"
                       "(DistrParamArray[%d]=%ld < %ld; "
                       "AMViewRef=%lx; PSRef=%lx)\n",
                       i, DistrParamArray[i], MinPar,
                       *AMViewRefPtr, (uLLng)VMSHandlePtr);
        }
     }

     if(AxisArr[i] > 0 && VMS == AMV->WeightVMS &&
        AMV->GenBlockCoordWeight[i])
     {
        /* On (i+1)-th dimension of the processor system 
           GenBlock is "hard" */    /*E0012*/

        if( AMV->Space.Size[j] !=
            (AMV->PrevSumGenBlockCoordWeight[i][Size] +
             AMV->GenBlockCoordWeight[i][Size]) )
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** RTS err 036.027: wrong call distr_\n"
                    "(invalid size %ld of %ld-th dimension of the "
                    "abstract machine representation;\n"
                    "right value must be %ld;  AMViewRef=%lx)\n",
                    AMV->Space.Size[j], AxisArr[i],
                    (AMV->PrevSumGenBlockCoordWeight[i][Size] +
                     AMV->GenBlockCoordWeight[i][Size]), *AMViewRefPtr);
//MMM

     } else { 
if (AMV->CoordWeight[i]) {
      { int k; double elm;
        for (k=0,elm=0; k< Size+1 ; k++) {
    	   if (elm < 0) { AMV->PrevSumCoordWeight[i][k] += elm; continue;}
           elm = AMV->Space.Size[j] - AMV->PrevSumCoordWeight[i][k] -
                 AMV->CoordWeight[i][k];
           if (elm == 0) break;
	   if (elm < 0) AMV->CoordWeight[i][k] += elm;
        }
      }
}
     }
//MMM
  }

  for(i=0; i < AMR; i++)
      TstArray[i] = 0;

  for(i=0; i < VMR; i++)
  {
     j = (int)AxisArr[i] - 1;

     if(AxisArr[i] <= 0)
        continue;

     if(TstArray[j])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.026: wrong call distr_\n"
                 "(AxisArray[%d]=AxisArray[%d]=%ld; "
                 "AMViewRef=%lx; PSRef=%lx)\n",
                 (int)(TstArray[j]-1), i, AxisArr[i],
                 *AMViewRefPtr, (uLLng)VMSHandlePtr);

     TstArray[j] = (byte)(i+1);
  }

 /**********************\
 * Execute distribution *
 \**********************/    /*E0013*/

  dvm_AllocArray(s_MAP, MapArrSize, MapArray);

  /* Preliminary fill in first and second parts of MapArray */    /*E0014*/

  for(i=0; i < AMR; i++)
  {
     MapArray[i].Attr = map_COLLAPSE;
     MapArray[i].Axis = (byte)(i+1);
     MapArray[i].PAxis = 0;
     MapArray[i].DisPar = 0.;
  }

  for(i=AMR; i < MapArrSize; i++)
  {
     MapArray[i].Attr = map_NORMVMAXIS;
     MapArray[i].Axis = 0;
     MapArray[i].PAxis = (byte)(i-AMR+1);
     MapArray[i].DisPar = 0.;
  }

  /* Fill in MapArray  in accordance with given parameters */    /*E0015*/

  for(i=0; i < VMR; i++)
  {
     j = (int)AxisArr[i] - 1;

     if(AxisArr[i] == 0)
        MapArray[AMR+i].Attr = map_REPLICATE;
     
     if(AxisArr[i] == -1)
     {
        MapArray[AMR+i].Attr = map_CONSTANT;
        MapArray[AMR+i].DisPar = DistrParamArray[i];
        IsConst = 1;
     }

     if(AxisArr[i] > 0)
     {
        MapArray[AMR+i].Axis = (byte)AxisArr[i];
 	MapArray[j].Attr = map_BLOCK;
	MapArray[j].PAxis = (byte)(i+1);

        /* Block size */    /*E0016*/

        if(DistrParamArray[i] > 0)
           DisPar = dvm_min(DistrParamArray[i],
                            AMV->Space.Size[j]);
        else
        {
           Size = (int)VMS->Space.Size[i] - 1;

           if(VMS == AMV->WeightVMS)
              DisPar = (double)(AMV->Space.Size[j])/
                       (AMV->PrevSumCoordWeight[i][Size] +
                        AMV->CoordWeight[i][Size]);
           else
              DisPar = (double)(AMV->Space.Size[j])/
                       (VMS->PrevSumCoordWeight[i][Size] +
                        VMS->CoordWeight[i][Size]);
           if(AMV->Div[j] != 1)
           {
              if(DisPar < AMV->Div[j])
                 DisPar = AMV->Div[j];
           }
        }

        if(DisPar < 1.)
           DisPar = 1.;

        MapArray[j].DisPar = DisPar;
        MapArray[AMR+i].DisPar = DisPar;
     }
  }

  /* Fix distribution in tables */    /*E0017*/

  AMV->VMS = VMS;
  AMV->DISTMAP = MapArray;

  coll_Insert(&VMS->AMVColl, AMV); /* to the list of representations
                                      mapped on the processor system */    /*E0018*/

  /* Create local part of representation */    /*E0019*/

  ALSize = 2*AMR;

  dvm_AllocArray(s_ALIGN, ALSize, AlList);

  /* Form two parts of AlList for identical alignment */    /*E0020*/

  for(i=0; i < AMR; i++)
  {  AlList[i].Attr  = align_NORMAL;
     AlList[i].Axis  = (byte)(i+1);
     AlList[i].TAxis = (byte)(i+1);
     AlList[i].A     = 1;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  for(i=AMR; i < ALSize; i++)
  {  AlList[i].Attr  = align_NORMTAXIS;
     AlList[i].Axis  = (byte)(i-AMR+1);
     AlList[i].TAxis = (byte)(i-AMR+1);
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  /* Calculate local part */    /*E0021*/

  Local = NULL;

  if(VMS->HasCurrent)
     Local = GetSpaceLB4Proc(VMS->CurrentProc, AMV,
                             &AMV->Space, AlList, NULL, &Block);

  if(Local)
  {  AMV->HasLocal = TRUE;
     AMV->Local    = block_Copy(Local);
  }

  /* Form an attribute of completely replicated representation */    /*E0022*/

  VMSize = VMS->ProcCount;

  if(VMSize == 1)
     AMV->Repl = 1; /* replication along all dimensions
                       of the processor system */    /*E0023*/
  else
  {  for(i=0; i < VMR; i++)
         if(AxisArr[i])
            break;

     if(i == VMR)
        AMV->Repl = 1; /* replication along all dimensions
                          of the processor system */    /*E0024*/
  }

  /* Setting the flag of partially replicated representation */    /*E0025*/

  if(AMV->Repl == 0)
  {  for(i=0; i < VMR; i++)
         if(AxisArr[i] == 0)
            break;

     if(i != VMR)
        AMV->PartRepl = 1; /* the representation is replicated by some 
                              dimension of the processor system */    /*E0026*/
  }

  /* Form an attribute, that there is at least one element
              of representation on each processor          */    /*E0027*/

  if(AMV->Repl)
     AMV->Every = 1;
  else
  {  if(AMV->AMSColl.Count)
     {  dvm_AllocArray(s_BLOCK *, VMSize, LocBlockPtr);
        dvm_AllocArray(s_BLOCK, VMSize, LocBlock);

        for(i=0; i < VMSize; i++)
            LocBlockPtr[i] = GetSpaceLB4Proc(i, AMV, &AMV->Space, AlList,
                                             NULL, &LocBlock[i]);

        for(i=0; i < VMSize; i++)
            if(LocBlockPtr[i] == NULL)
               break;

        if(i == VMSize)
           AMV->Every = 1; /* there is an element of representation 
                              on each processor */    /*E0028*/
     }
     else
     {  Local = GetSpaceLB4Proc(VMSize-1, AMV, &AMV->Space, AlList,
                                NULL, &Block);
        if(Local && (IsConst == 0 || VMR == 1))
           AMV->Every = 1; /* there is an element of representation 
                              on each processor */    /*E0029*/
     }
  }

  CrtVMSForAMS(AMV, LocBlockPtr, AlList);/* create subtasks for 
                                            the representation local part 
                                            and for all interrogated machines */    /*E0030*/
  if(LocBlockPtr)
  {  dvm_FreeArray(LocBlockPtr);
     dvm_FreeArray(LocBlock);
  }

  dvm_FreeArray(AxisArr);
  dvm_FreeArray(AlList);

  /* Print result tables */    /*E0031*/

  if(RTL_TRACE)
  {  if(distr_Trace && TstTraceEvent(call_distr_))
     {  for(i=0; i < MapArrSize; i++)
            tprintf("DistMap[%d]: Attr=%d Axis=%d "
                    "PAxis=%d DisPar=%lf\n",
                    i, (int)MapArray[i].Attr, (int)MapArray[i].Axis,
                    (int)MapArray[i].PAxis, MapArray[i].DisPar);
        tprintf(" \n");

        if(AMV->HasLocal)
        {  for(i=0; i < AMR; i++) 
               tprintf("Local[%d]: Lower=%ld Upper=%ld "
                       "Size=%ld Step=%ld\n",
                       i, AMV->Local.Set[i].Lower,
                       AMV->Local.Set[i].Upper,
                       AMV->Local.Set[i].Size, AMV->Local.Set[i].Step);
          tprintf(" \n");
        }

        tprintf("Repl=%d PartRepl=%d Every=%d\n",
                (int)AMV->Repl, (int)AMV->PartRepl, (int)AMV->Every);
        tprintf(" \n");
     }
  }

  /* ------------------------------ */    /*E0032*/
       
  if(RTL_TRACE)
     dvm_trace(ret_distr_,"IsLocal=%d;\n",(int)AMV->HasLocal);

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0033*/
  DVMFTimeFinish(ret_distr_);
  return  (DVM_RET, (DvmType)AMV->HasLocal);
}



DvmType  __callstd redis_(AMViewRef *AMViewRefPtr, PSRef *PSRefPtr,
                          DvmType *ParamCountPtr, DvmType AxisArray[],
                          DvmType DistrParamArray[], DvmType *NewSignPtr)

/*
      Remapping abstract machine representation.
      ------------------------------------------

*AMViewRefPtr 	- reference to the parental abstract machine
                  representation, which mapping has to be redistributed.
*PSRefPtr	- reference to the processor system, which defines
                  resource structure of a new distribution.
*ParamCountPtr	- the number of parameters, defined in arrays
                  AxisArray and DistrParamArray.
AxisArray	- AxisArray[j] is a dimension number of the abstract
                  machine representation used in the mapping rule for
                  processor system (j+1)th dimension. 
DistrParamArray	- DistrParamArray[j] is a mapping rule parameter
                  for parallel system (j+1)th dimension 
                  (DistrParamArray[j] is a nonnegative integer). 
*NewSignPtr	- the flag that defines whether to save contents
                  of realigned arrays or not.

The function redis_ cancels the resource distribution of the parental
abstract machine previously defined for child abstract machines from
*AMViewRefPtr representation by function distr_. The function defines
new distribution of this representation according to specified
parameters. The function returns non-zero value if remapped
representation has a local part on the current processor, otherwise
returns zero.
*/    /*E0034*/

{ SysHandle      *ArrayHandlePtr, *AMVHandlePtr, *NewAMVHandlePtr,
                 *VMSHandlePtr;
  int             i;
  s_DISARRAY     *DArr, *wDArr;
  s_AMVIEW       *AMV, *NewAMV;
  AMViewRef       AMVRef, NewAMVRef;
  AMRef           AMRefr;
  DvmType         AMVRank, AMVStatic, StaticMap = 0, Res = 0;
  DvmType         *ArrayHeader;
  ArrayMapRef     ArrMapRef;
  byte            SaveReDistr;
  s_AMS          *CAMS, *PAMS, *AMS;
  s_VMS          *VMS;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0035*/
  DVMFTimeStart(call_redis_);

  ArrayHandlePtr = TstDVMArray((DvmType *)AMViewRefPtr);

  if(RTL_TRACE)
  { if(!TstTraceEvent(call_redis_))
       dvm_trace(call_redis_," \n");
    else
    {  if(ArrayHandlePtr)
       {  if(PSRefPtr == NULL || *PSRefPtr == 0)
             dvm_trace(call_redis_,
                 "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                 "PSRefPtr=NULL; PSRef=0; "
                 "ParamCount=%ld; NewSign=%ld;\n",
                 (uLLng)AMViewRefPtr, *AMViewRefPtr,
                 *ParamCountPtr,*NewSignPtr);
          else
             dvm_trace(call_redis_,
                 "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                 "PSRefPtr=%lx; PSRef=%lx; "
                 "ParamCount=%ld; NewSign=%ld;\n",
                 (uLLng)AMViewRefPtr, *AMViewRefPtr,
                 (uLLng)PSRefPtr, *PSRefPtr,
                 *ParamCountPtr, *NewSignPtr);
       }
       else
       {  if(PSRefPtr == NULL || *PSRefPtr == 0)
             dvm_trace(call_redis_,
                 "AMViewRefPtr=%lx; AMViewRef=%lx; "
                 "PSRefPtr=NULL; PSRef=0; "
                 "ParamCount=%ld; NewSign=%ld;\n",
                 (uLLng)AMViewRefPtr, *AMViewRefPtr,
                 *ParamCountPtr, *NewSignPtr);
          else
             dvm_trace(call_redis_,
                 "AMViewRefPtr=%lx; AMViewRef=%lx; "
                 "PSRefPtr=%lx; PSRef=%lx; "
                 "ParamCount=%ld; NewSign=%ld;\n",
                 (uLLng)AMViewRefPtr, *AMViewRefPtr,
                 (uLLng)PSRefPtr, *PSRefPtr,
                 *ParamCountPtr, *NewSignPtr);
       }

       for(i=0; i < *ParamCountPtr; i++)
           tprintf("      AxisArray[%d]=%ld; ",i,AxisArray[i]);
       tprintf(" \n");
       for(i=0; i < *ParamCountPtr; i++)
           tprintf("DistrParamArray[%d]=%ld; ",i,DistrParamArray[i]);
       tprintf(" \n");
       tprintf(" \n");
    }
  }

  /* skip redistribute if launched only on 1 proc */
  if ((DVM_VMS->ProcCount == 1)&&(AllowRedisRealnBypass))
  {
    if(RTL_TRACE)
        dvm_trace(ret_redis_,"IsLocal=%ld;\n", (DvmType)1);

    StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */
    DVMFTimeFinish(ret_redis_);

    return  (DVM_RET, (DvmType)1);
  }

  if(ArrayHandlePtr)
  {  /* Representation is defined indirectly through array */    /*E0036*/

     DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

     if( !(DArr->ReDistr & 0x1) )
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 036.050: wrong call redis_\n"
                  "(ReDistrPar=%d; ArrayHeader[0]=%lx)\n",
                  (int)DArr->ReDistr, (uLLng)ArrayHandlePtr);

     AMV = DArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.051: wrong call redis_\n"
                 "(the array has not been aligned;\n"
                 "ArrayHeader[0]=%lx)\n", (uLLng)ArrayHandlePtr);

     AMVHandlePtr = AMV->HandlePtr;
  }
  else
  {  /* Representation is directly defined */    /*E0037*/

     if(TstObject)
     {  if(!TstDVMObj(AMViewRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.060: wrong call redis_\n"
             "(the abstract machine representation "
             "is not a DVM object;\n"
             "AMViewRef=%lx)\n", *AMViewRefPtr);
     }

     AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

     if(AMVHandlePtr->Type != sht_AMView)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 036.061: wrong call redis_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

     AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  }

  if(AMV->DivReset)
  {
     AMV->DivReset = 0;

     for(i=0; i < MAXARRAYDIM; i++)
         AMV->Div[i] = 1;
  }

  if(AMV->VMS == NULL) /* if remapped representation
                          has already be mapped */    /*E0038*/
     Res = ( RTL_CALL, distr_(AMViewRefPtr, PSRefPtr, ParamCountPtr,
                              AxisArray, DistrParamArray) );
  else
  {  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent abstract machine */    /*E0039*/
     CAMS = (s_AMS *)CurrAMHandlePtr->pP;  /* current abstract machine */    /*E0040*/

     /* Check if the representation elements have children */    /*E0041*/

     for(i=0; i < AMV->AMSColl.Count; i++)
     {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

        if(AMS->SubSystem.Count)/* whether the abstract
                                   machine is a list */    /*E0042*/
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 036.065: wrong call redis_\n"
                    "(the abstract machine of the representation "
                    "has descendants; AMViewRef=%lx; AMRef=%lx)\n",
                    *AMViewRefPtr, (uLLng)AMS->HandlePtr);
     }
  
     if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  VMS = DVM_VMS;
        VMSHandlePtr = VMS->HandlePtr;
     }
     else
     {  if(TstObject)
        {  if(!TstDVMObj(PSRefPtr))
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 036.066: wrong call redis_\n"
                       "(the processor system is not a DVM object; "
                       "PSRef=%lx)\n", *PSRefPtr);
        }

        VMSHandlePtr = (SysHandle *)*PSRefPtr;

        if(VMSHandlePtr->Type != sht_VMS)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 036.067: wrong call redis_\n"
                 "(the object is not a processor system; PSRef=%lx)\n",
                 *PSRefPtr);

        VMS = (s_VMS *)VMSHandlePtr->pP;
     }

     /* Check if all specified processor system processors
                  are in parent processor system           */    /*E0043*/

     NotSubsystem(i, PAMS->VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.068: wrong call redis_\n"
             "(the given PS is not a subsystem of the parental PS; "
             "PSRef=%lx; ParentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)(PAMS->VMS->HandlePtr));

     /* Check if all specified processor system processors
                 are in  current processor system          */    /*E0044*/

     NotSubsystem(i, DVM_VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 036.069: wrong call redis_\n"
             "(the given PS is not a subsystem of the current PS; "
             "PSRef=%lx; CurrentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);

     /* Create a new representation of an abstract machine */    /*E0045*/

     AMVRef    = (AMViewRef)AMVHandlePtr;
     AMRefr    = (AMRef)AMV->AMHandlePtr; /* reference to the parent
                                             abstract machine */    /*E0046*/
     AMVRank   = AMV->Space.Rank;
     AMVStatic = AMV->Static;

     NewAMVRef = ( RTL_CALL, crtamv_(&AMRefr, &AMVRank, AMV->Space.Size, &AMVStatic) );

     NewAMVHandlePtr = (SysHandle *)NewAMVRef;
     NewAMV          = (s_AMVIEW *)NewAMVHandlePtr->pP;

     /* Transfer of the processor system coordinate weights */    /*E0047*/

if(AMV->setCW==1) {
     NewAMV->WeightVMS = NULL;
     for(i=0; i < MAXARRAYDIM; i++)
     {
        NewAMV->CoordWeight[i] = NULL;
        NewAMV->PrevSumCoordWeight[i] = NULL;
        NewAMV->GenBlockCoordWeight[i] = NULL;
        NewAMV->PrevSumGenBlockCoordWeight[i] = NULL;  
     }
}
if (AMV->setCW==2) {
     NewAMV->WeightVMS = AMV->WeightVMS;
     AMV->WeightVMS = AMV->disWeightVMS;
     AMV->disWeightVMS = NULL;
     for(i=0; i < MAXARRAYDIM; i++)
     {
        NewAMV->CoordWeight[i] = AMV->CoordWeight[i];
        NewAMV->PrevSumCoordWeight[i] = AMV->PrevSumCoordWeight[i];
        NewAMV->GenBlockCoordWeight[i] = AMV->GenBlockCoordWeight[i];
        NewAMV->PrevSumGenBlockCoordWeight[i] = AMV->PrevSumGenBlockCoordWeight[i];  
        AMV->CoordWeight[i] = AMV->disCoordWeight[i];
        AMV->PrevSumCoordWeight[i] = AMV->disPrevSumCoordWeight[i];
        AMV->GenBlockCoordWeight[i] = AMV->disGenBlockCoordWeight[i];
        AMV->PrevSumGenBlockCoordWeight[i] = AMV->disPrevSumGenBlockCoordWeight[i];
        AMV->disCoordWeight[i] = NULL;
        AMV->disPrevSumCoordWeight[i] = NULL;
        AMV->disGenBlockCoordWeight[i] = NULL;
        AMV->disPrevSumGenBlockCoordWeight[i] = NULL;
     }
}

     for(i=0; i < MAXARRAYDIM; i++)
     {
        NewAMV->Div[i] = AMV->Div[i];
        AMV->Div[i] = AMV->disDiv[i];
     }
/*
     NewAMV->WeightVMS = AMV->WeightVMS;
     AMV->WeightVMS = NULL;

     for(i=0; i < MAXARRAYDIM; i++)
     {
        NewAMV->CoordWeight[i] = AMV->CoordWeight[i];
        NewAMV->PrevSumCoordWeight[i] = AMV->PrevSumCoordWeight[i];
        NewAMV->GenBlockCoordWeight[i] = AMV->GenBlockCoordWeight[i];
        NewAMV->PrevSumGenBlockCoordWeight[i] =
        AMV->PrevSumGenBlockCoordWeight[i];
        AMV->CoordWeight[i] = NULL;
        AMV->PrevSumCoordWeight[i] = NULL;
        AMV->GenBlockCoordWeight[i] = NULL;
        AMV->PrevSumGenBlockCoordWeight[i] = NULL;
        NewAMV->Div[i] = AMV->Div[i];
     }
*/

     /* Transfer of all interrogated abstract machines
              from old to the new representation       */    /*E0048*/

     NewAMV->AMSColl = AMV->AMSColl;
     AMV->AMSColl    = coll_Init(AMVAMSCount, AMVAMSCount, NULL);

     for(i=0; i < NewAMV->AMSColl.Count; i++)
     {
        AMS = coll_At(s_AMS *, &NewAMV->AMSColl, i);
        AMS->ParentAMView = NewAMV;
        AMS->VMS = NULL;
     }

     /* Mapping new representation */    /*E0049*/

     Res = ( RTL_CALL, distr_(&NewAMVRef, PSRefPtr, ParamCountPtr,
                              AxisArray, DistrParamArray) );

     /* Redistribute all arrays of an old representation */    /*E0050*/

     while(AMV->ArrColl.Count)
     {  wDArr = coll_At(s_DISARRAY *, &AMV->ArrColl,
                        AMV->ArrColl.Count - 1);
        ArrayHeader = (DvmType *)wDArr->HandlePtr->HeaderPtr;
        SaveReDistr = wDArr->ReDistr;
        wDArr->ReDistr = 3;

        ArrMapRef = ( RTL_CALL, arrmap_(ArrayHeader, &StaticMap) );
        ( RTL_CALL, mrealn_(ArrayHeader, &NewAMVRef, &ArrMapRef, NewSignPtr) );
        ( RTL_CALL, delarm_(&ArrMapRef) );

        wDArr = (s_DISARRAY *)((SysHandle *)ArrayHeader[0])->pP;
        wDArr->ReDistr = SaveReDistr;
     }

     /* Delete an old representation and save Handle */    /*E0051*/

     AMV->HandlePtr = NULL;    /* to save Handle */    /*E0052*/

     ( RTL_CALL, delamv_(&AMVRef) );

     /* Form an old  Handle as a  Handle of new representation */    /*E0053*/

     *AMVHandlePtr     = *NewAMVHandlePtr;
     NewAMV->HandlePtr = AMVHandlePtr;

     /* Delete new Handle */    /*E0054*/

     if(TstObject)
        DelDVMObj((ObjectRef)NewAMVHandlePtr);

     dvm_FreeStruct(NewAMVHandlePtr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_redis_,"IsLocal=%ld;\n", Res);

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0055*/
  DVMFTimeFinish(ret_redis_);
  return  (DVM_RET, Res);
}


/* ----------------------------------------------------------- */    /*E0056*/


DvmType  __callstd setgrn_(AMViewRef  *AMViewRefPtr, DvmType *AxisPtr, DvmType *GroupNumberPtr)
{ SysHandle      *AMVHandlePtr;
  int             Axis, GroupNumber, i, Rank;
  s_AMVIEW       *AMV;
  double         *dptr;
  void          **PtrToVoidPtr;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0057*/
  DVMFTimeStart(call_setgrn_);

  if(RTL_TRACE)
     dvm_trace(call_setgrn_,"AMViewRefPtr=%lx; AMViewRef=%lx; "
                            "Axis=%ld; GroupNumber=%ld;\n",
                             (uLLng)AMViewRefPtr, *AMViewRefPtr,
                             *AxisPtr, *GroupNumberPtr);

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 037.000: wrong call setgrn_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr=(SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.001: wrong call setgrn_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV  = (s_AMVIEW *)AMVHandlePtr->pP;

  Axis = (int)*AxisPtr;
  GroupNumber = (int)*GroupNumberPtr;
  Rank = AMV->Space.Rank;

  if(Axis < 1 || Axis > Rank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.002: wrong call setgrn_\n"
              "(invalid Axis parameter: Axis=%d)\n", Axis);

  if(GroupNumber < 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.003: wrong call setgrn_\n"
              "(invalid GroupNumber parameter: GroupNumber=%d)\n",
              GroupNumber);
  Axis--;

  if(GroupNumber > 0)
  {  AMV->TimeMeasure = 1;
     AMV->GroupNumber[Axis] = GroupNumber;
     AMV->Is_gettar[Axis] = 0;
     PtrToVoidPtr = (void **)&AMV->GroupWeightArray[Axis];
     mac_free(PtrToVoidPtr);
     dvm_AllocArray(double, GroupNumber, AMV->GroupWeightArray[Axis]);

     dptr = AMV->GroupWeightArray[Axis];

     for(i=0; i < GroupNumber; i++)
         dptr[i] = 0.;          
  }
  else
  {  AMV->TimeMeasure = 0; /* preliminary: do not  measuring 
                              for the given AM representation */    /*E0058*/
     AMV->GroupNumber[Axis] = 0;
     AMV->Is_gettar[Axis] = 0;
     dvm_FreeArray(AMV->GroupWeightArray[Axis]);

     /* Restore flag of measuring 
        for the given AM representation */    /*E0059*/

     for(i=0; i < Rank; i++)
     {  if(AMV->GroupNumber[i])
           AMV->TimeMeasure = 1;
     } 
  }

  if(RTL_TRACE)
     dvm_trace(ret_setgrn_," \n");

  DVMFTimeFinish(ret_setgrn_);
  return  (DVM_RET, 0);
}



DvmType  __callstd getwar_(double  WeightArray[], AMViewRef  *AMViewRefPtr, DvmType  *AxisPtr)
{
    DvmType Res;

  Res = gettar_(WeightArray, AMViewRefPtr, AxisPtr);
  rsttar_(AMViewRefPtr, AxisPtr);
  
  return  Res;
}



DvmType  __callstd gettar_(double  WeightArray[], AMViewRef  *AMViewRefPtr, DvmType *AxisPtr)
{ SysHandle      *AMVHandlePtr;
  DvmType        Res = 0;
  int             Axis, GroupNumber, i, j, CW;
  s_AMVIEW       *AMV;
  RTL_Request    *Req;
  int           **Header;
  double        **Weight;
  double         *dptr0, *dptr1, *dptr2;
  int            *Hdr;
  RTL_Request     RTL_Req;


  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0060*/
  DVMFTimeStart(call_gettar_);

  if(RTL_TRACE)
     dvm_trace(call_gettar_,"WeightArray=%lx; AMViewRefPtr=%lx; "
                            "AMViewRef=%lx; Axis=%ld;\n",
                            (uLLng)WeightArray, (uLLng)AMViewRefPtr,
                            *AMViewRefPtr, *AxisPtr);

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 037.010: wrong call gettar_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr=(SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.011: wrong call gettar_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV  = (s_AMVIEW *)AMVHandlePtr->pP;

  Axis = (int)*AxisPtr;

  if(Axis < 1 || Axis > AMV->Space.Rank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.012: wrong call gettar_\n"
              "(invalid Axis parameter: Axis=%d)\n", Axis);
  Axis--;
  GroupNumber = AMV->GroupNumber[Axis];

  /* Forward to the next element of message tag circle tag_gettar_
     for the current processor system */    /*E0061*/

  DVM_VMS->tag_gettar_++;

  if((DVM_VMS->tag_gettar_ - (msg_gettar_)) >= TagCount)
     DVM_VMS->tag_gettar_ = msg_gettar_;

  /* ------------------------------------------------ */    /*E0062*/

  if(GroupNumber)
  {  Res = GroupNumber; /* length of array with weights is returned */    /*E0063*/

     dptr0 = AMV->GroupWeightArray[Axis];

     if(AMV->Is_gettar[Axis] == 0)
     {  AMV->Is_gettar[Axis] = 1; /* flag: measuring results were
                                     scanned */    /*E0064*/

        if(MPS_CurrentProc == DVM_CentralProc)
        {  /* Current processor is central processor */    /*E0065*/

           /* Receive messages with ranges */    /*E0066*/

           dvm_AllocArray(RTL_Request, DVM_ProcCount, Req);
           dvm_AllocArray(int *, DVM_ProcCount, Header);

           for(i=0; i < DVM_ProcCount; i++)
           {  dvm_AllocArray(int, 2, Header[i]);

              if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                 ( RTL_CALL, rtl_Recvnowait(Header[i], 2, sizeof(int),
                                            (int)DVM_VMS->VProc[i].lP,
                                             DVM_VMS->tag_gettar_,
                                             &Req[i], 0) );
           }

           for(i=0; i < DVM_ProcCount; i++)
               if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                  ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

           /* Receive messages with weight array fragments */    /*E0067*/

           dvm_AllocArray(double *, DVM_ProcCount, Weight);

           for(i=0; i < DVM_ProcCount; i++)
           {  if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
              {  CW = Header[i][1] - Header[i][0] + 1;
                 dvm_AllocArray(double, CW, Weight[i]);

                 ( RTL_CALL, rtl_Recvnowait(Weight[i],CW,sizeof(double),
                                            (int)DVM_VMS->VProc[i].lP,
                                             DVM_VMS->tag_gettar_,
                                             &Req[i], 0) );
              }
           }

           for(i=0; i < DVM_ProcCount; i++)
               if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                  ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

           /* add received fragments in the array
              of central (current) processor weights */    /*E0068*/

           for(i=0; i < DVM_ProcCount; i++)
           {  if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
              {  CW = Header[i][1] - Header[i][0] + 1;
                 dptr1 = Weight[i];
                 dptr2 = dptr0 + Header[i][0];

                 for(j=0; j < CW; j++, dptr1++, dptr2++)
                     *dptr2 += *dptr1;
              }
           }

           /* Send result weight array */    /*E0069*/

           for(i=0; i < DVM_ProcCount; i++)
           {  if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                 ( RTL_CALL, rtl_Sendnowait(dptr0, GroupNumber,
                                            sizeof(double),
                                            (int)DVM_VMS->VProc[i].lP,
                                             DVM_VMS->tag_gettar_,
                                             &Req[i], gettar_Send) );
           }
         
           if(MsgSchedule && UserSumFlag)
           {  rtl_TstReqColl(0);
              rtl_SendReqColl(0.);
           }

           for(i=0; i < DVM_ProcCount; i++)
               if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                  ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

           /* Free memory */    /*E0070*/

           for(i=0; i < DVM_ProcCount; i++)
           {  dvm_FreeArray(Header[i]);

              if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
              {  dvm_FreeArray(Weight[i]);
              }
           }

           dvm_FreeArray(Header);
           dvm_FreeArray(Weight);
           dvm_FreeArray(Req);
        }
        else
        {  /* Current processor is non-central processor */    /*E0071*/

           /* Define array local part */    /*E0072*/

           dvm_AllocArray(int, 2, Hdr);

           for(i=0; i < GroupNumber; i++)
               if(dptr0[i] != 0.)
                  break; /* first non zero element of weight array
                            is found */    /*E0073*/

           if(i == GroupNumber)
           {  /* All elements of weight array are equal to zero */    /*E0074*/

              Hdr[0] = 0;
              Hdr[1] = 0;
           }
           else
           {  Hdr[0] = i;

              for(i=GroupNumber-1; i > -1; i--)
                  if(dptr0[i] != 0.)
                     break; /* last non zero element of weight array
                               is found */    /*E0075*/
              Hdr[1] = i;
           }

           /* Send index range of weight array local part
              to the central processor */    /*E0076*/

           ( RTL_CALL, rtl_Sendnowait(Hdr,2,sizeof(int),DVM_CentralProc,
                                      DVM_VMS->tag_gettar_,
                                      &RTL_Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&RTL_Req) );

           /* Send  weight array local part
              to the central processor */    /*E0077*/

           dptr1 = dptr0 + Hdr[0];
           CW = Hdr[1] - Hdr[0] + 1;

           ( RTL_CALL, rtl_Sendnowait(dptr1, CW, sizeof(double),
                                      DVM_CentralProc,
                                      DVM_VMS->tag_gettar_, &RTL_Req,
                                      0) );
           ( RTL_CALL, rtl_Waitrequest(&RTL_Req) );

           /* Receive result weight array from 
              the central processor */    /*E0078*/

           ( RTL_CALL, rtl_Recvnowait(dptr0,GroupNumber,sizeof(double),
                                      DVM_CentralProc,
                                      DVM_VMS->tag_gettar_, &RTL_Req,
                                      0) );
           ( RTL_CALL, rtl_Waitrequest(&RTL_Req) );

           dvm_FreeArray(Hdr);
        }
     }

     /* Rewrite weight array into the user program */    /*E0079*/

     for(i=0; i < GroupNumber; i++, dptr0++)
         WeightArray[i] = *dptr0;
  }

  /* Array of load weight coordinates is output in trace */    /*E0080*/

  if(RTL_TRACE)
  {  if(WeightArrayTrace && TstTraceEvent(ret_gettar_) && Res)
     {  for(i=0,j=0; i < Res; i++,j++)
        {  if(j == 2)
           {  tprintf(" \n");
              j = 0;
           }

           tprintf(" WeightArray[%d]=%lf;", i, WeightArray[i]);
        }

        tprintf(" \n");
     }
  }
   
  if(RTL_TRACE)
     dvm_trace(ret_gettar_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_gettar_);
  return  (DVM_RET, Res);
}



DvmType  __callstd rsttar_(AMViewRef  *AMViewRefPtr, DvmType  *AxisPtr)
{ SysHandle      *AMVHandlePtr;
  int             Axis, i, Rank, Init, Last;
  s_AMVIEW       *AMV;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0081*/
  DVMFTimeStart(call_rsttar_);

  if(RTL_TRACE)
  {  if(AxisPtr == NULL)
        dvm_trace(call_rsttar_,
                  "AMViewRefPtr=%lx; AMViewRef=%lx; Axis=0;\n",
                  (uLLng)AMViewRefPtr, *AMViewRefPtr);
     else
        dvm_trace(call_rsttar_,
                  "AMViewRefPtr=%lx; AMViewRef=%lx; Axis=%ld;\n",
                  (uLLng)AMViewRefPtr, *AMViewRefPtr, *AxisPtr);
  }

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 037.020: wrong call rsttar_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr=(SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.021: wrong call rsttar_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV  = (s_AMVIEW *)AMVHandlePtr->pP;

  if(AxisPtr == NULL)
     Axis = 0;  /* measuring for all dimentions */    /*E0082*/
  else
     Axis = (int)*AxisPtr;

  Rank = AMV->Space.Rank;

  if(Axis < 0 || Axis > Rank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 037.022: wrong call rsttar_\n"
              "(invalid Axis parameter: Axis=%d)\n", Axis);

  if(Axis)
  {  Init = Axis - 1;
     Last = Init;
  }
  else
  {  Init = 0;
     Last = Rank - 1;
  }

  AMV->TimeMeasure = 0; /* preliminary: do not measuring for
                           the given AM representation  */    /*E0083*/

  for(i=Init; i <= Last; i++)
  {  AMV->GroupNumber[i] = 0;
     AMV->Is_gettar[i] = 0;
     dvm_FreeArray(AMV->GroupWeightArray[i]);
  }

  /* Restore flag of measuring for the given AM representation */    /*E0084*/

  for(i=0; i < Rank; i++)
  {  if(AMV->GroupNumber[i])
        AMV->TimeMeasure = 1;
  } 

  if(RTL_TRACE)
     dvm_trace(ret_rsttar_," \n");

  DVMFTimeFinish(ret_rsttar_);
  return  (DVM_RET, 0);
}


/* ----------------------------------------------------------- */    /*E0085*/


/****************************************************************************\
* Creation of subtask for the local part of the specified representation     *
* and processor systems for all its interrogated abstract machines           *
\****************************************************************************/    /*E0086*/

void  CrtVMSForAMS(s_AMVIEW  *AMV, s_BLOCK  **LocBlockPtr,
                   s_ALIGN  *AlList)
{ s_AMS       *PAMS, *AMS, *wAMS;
  s_VMS       *VMS;
  PSRef        VMRef, NewVMRef;
  int          i, j, k, VMRank, AMVRank, MapArrSize, ALSize;
  s_MAP       *MapArray;
  DvmType        *SizeArray, *Lower, *Upper;
  DvmType         Static;
  SysHandle   *NewAMHandlePtr;
  s_BLOCK    **LocBlockArray = NULL, *LocBlock = NULL;
  s_ALIGN     *AlignList;

  PAMS       = (s_AMS *)AMV->AMHandlePtr->pP; /* descriptor of parent
                                                 abstract machine */    /*E0087*/
  Static     = AMV->Static;
  VMS        = AMV->VMS;         /* processror system in which
                                    the representation mapped */    /*E0088*/

  VMRef     = (PSRef)VMS->HandlePtr; /* reference to processor system
                                        which subsystem is to be created */    /*E0089*/

  VMRank     = VMS->Space.Rank;  /* dimension of processor system */    /*E0090*/
  AMVRank    = AMV->Space.Rank;  /* dimension of representation */    /*E0091*/
  MapArray   = AMV->DISTMAP;     /* representation map */    /*E0092*/
  MapArrSize = VMRank + AMVRank; /* size of representation map */    /*E0093*/
  
  dvm_AllocArray(DvmType, MapArrSize, SizeArray);
  dvm_AllocArray(DvmType, MapArrSize, Lower);
  dvm_AllocArray(DvmType, MapArrSize, Upper);

  /* Find dimensions of processor systems under creation */    /*E0094*/

  for(i=0; i < VMRank; i++)
  {  if(MapArray[i+AMVRank].Attr == map_REPLICATE)
        SizeArray[i] = VMS->Space.Size[i]; /* if in (i+1)-dimension
                                              there is replication */    /*E0095*/
     else
        SizeArray[i] = 1;
  }

  /**************************************************************\
  * Creation of processor system for the local part of specified * 
  * repsresentation and teh corresponding abstract machine       *
  \**************************************************************/    /*E0096*/

  if(AMV->HasLocal)
  {  /* Representation with local part on current processor */    /*E0097*/

     /*        Find for each dimension 
        initial value of processor coordinate */    /*E0098*/

     for(i=0; i < VMRank; i++)
     {  if(MapArray[i+AMVRank].Attr == map_REPLICATE)
           Lower[i] = 0;
        else
           Lower[i] = VMS->CVP[i+1];
     }

     /* Creation of processor system */    /*E0099*/

     for(i=0; i < VMRank; i++)
         Upper[i] = Lower[i] + SizeArray[i] - 1;

     NewVMRef = (RTL_CALL, crtps_(&VMRef, Lower, Upper, &Static) );

     /* Compute minimal linear index of abstract 
        machine in local part of representation  */    /*E0100*/

     Lower[0] = AMVRank;

     for(i=0; i < AMVRank; i++)
         Lower[i+1] = AMV->Local.Set[i].Lower;

     AMV->LinAMInd = space_GetLI(&AMV->Space, Lower);

     /*     Search abstract machine with computed linear index  
        among interrogated abstract machines in the representation */    /*E0101*/

     for(i=0; i < AMV->AMSColl.Count; i++)
     {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);
        if(AMS->HandlePtr->lP == AMV->LinAMInd)
           break;    /* machine with compute index was interrogated */    /*E0102*/
     }

     AMV->LocAMInd = i; /* index of local abstract machine
                           in the list of interrogated abstarct machines */    /*E0103*/

     if(i == AMV->AMSColl.Count)
     {  /* Abstract machine with computed index not found */    /*E0104*/

        /* Creation of abstarct machine for
           the local part of representation */    /*E0105*/

        dvm_AllocStruct(SysHandle, NewAMHandlePtr);
        dvm_AllocStruct(s_AMS, AMS);

        *NewAMHandlePtr = sysh_Build(sht_AMS, gEnvColl->Count - 1,
                                     AMV->CrtEnvInd, AMV->LinAMInd,
                                     AMS);
        AMS->EnvInd = NewAMHandlePtr->EnvInd; /* index of current
                                                 context */    /*E0106*/
        AMS->CrtEnvInd = NewAMHandlePtr->CrtEnvInd; /* index of context,
                                                       in which the object
                                                       created */    /*E0107*/
        AMS->HandlePtr = NewAMHandlePtr; /* pointer to own
                                            Handle */    /*E0108*/
        AMS->ParentAMView = AMV; /* reference to representation
                                    of parent abstract machine */    /*E0109*/
        AMS->SubSystem = coll_Init(AMSAMVCount, AMSAMVCount, NULL);
        coll_Insert(&AMV->AMSColl, AMS); /* to the list of created
                                            abstract machines */    /*E0110*/
        AMS->TreeIndex = PAMS->TreeIndex + 1; /* distance to the root
                                                 of abstract machine 
                                                 tree */    /*E0111*/
        if(TstObject)
           InsDVMObj((ObjectRef)NewAMHandlePtr); /* register new
                                                    abstract machine */    /*E0112*/
     }
     
     /* Map abstract machine on a new processor system */    /*E0113*/

     AMS->VMS = (s_VMS *)((SysHandle *)NewVMRef)->pP;
     coll_Insert(&AMS->VMS->AMSColl, AMS);/* into the list of abstract machines
                                             mapped into processor system */    /*E0114*/
  }

  /****************************************************\
  * Creation of processor systems for all interrogated *
  * abstract machines of the representation            *
  \****************************************************/    /*E0115*/

  if(AMV->AMSColl.Count > 1)
  {  if(AlList == NULL)
     {  /* Map of equivalent alignment not given in parameters */    /*E0116*/

        ALSize = 2 * AMVRank;

        dvm_AllocArray(s_ALIGN, ALSize, AlignList);

        /* Form two parts of AlignList
            for equivalent alignment   */    /*E0117*/

        for(i=0; i < AMVRank; i++)
        {  AlignList[i].Attr  = align_NORMAL;
           AlignList[i].Axis  = (byte)(i+1);
           AlignList[i].TAxis = (byte)(i+1);
           AlignList[i].A     = 1;
           AlignList[i].B     = 0;
           AlignList[i].Bound = 0;
        }

        for(i=AMVRank; i < ALSize; i++)
        {  AlignList[i].Attr  = align_NORMTAXIS;
           AlignList[i].Axis  = (byte)(i-AMVRank+1);
           AlignList[i].TAxis = (byte)(i-AMVRank+1);
           AlignList[i].A     = 0;
           AlignList[i].B     = 0;
           AlignList[i].Bound = 0;
        }
     }
     else
        AlignList = AlList;

     if(LocBlockPtr == NULL)
     {  /* Local blocks of processors not given in parameters */    /*E0118*/

        dvm_AllocArray(s_BLOCK *, VMS->ProcCount, LocBlockArray);
        dvm_AllocArray(s_BLOCK, VMS->ProcCount, LocBlock);

        for(i=0; i < VMS->ProcCount; i++)
            LocBlockArray[i] = GetSpaceLB4Proc(i, AMV, &AMV->Space,
                                               AlignList, NULL,
                                               &LocBlock[i]);
     }
     else
        LocBlockArray = LocBlockPtr;

     for(i=0; i < AMV->AMSColl.Count; i++) /* loop in interrogated
                                              abstract machines    */    /*E0119*/
     {  wAMS = coll_At(s_AMS *, &AMV->AMSColl, i);

        if(wAMS->VMS)
           continue; /* interrogated abstract machine already mapped */    /*E0120*/
        
        Lower[0] = AMVRank;
        space_GetSI(&AMV->Space, wAMS->HandlePtr->lP, &Lower);

        /* Search of processor, on which the next 
           interrogated abstarct machine is mapped */    /*E0121*/

        for(j=0; j < VMS->ProcCount; j++)  /* loop in processors */    /*E0122*/
        {  if(LocBlockArray[j] != NULL)
              IsElmOfBlock(k, LocBlockArray[j], &Lower[1])
           else
              k = 0;

           if(k)
           {  /* i-th abstract machine mapped on j-th processor */    /*E0123*/

              /*      Find for each dimension
                 initial processor ccordinate value */    /*E0124*/

              Upper[0] = VMRank;
              space_GetSI(&VMS->Space, j, &Upper);

              for(k=0; k < VMRank; k++)
              {  if(MapArray[k+AMVRank].Attr == map_REPLICATE)
                    Lower[k] = 0;
                 else
                    Lower[k] = Upper[k+1];
              }

              /* Creation of processor system
                  for i-th abstarct machine   */    /*E0125*/

              for(k=0; k < VMRank; k++)
                  Upper[k] = Lower[k] + SizeArray[k] - 1;

              NewVMRef = ( RTL_CALL, crtps_(&VMRef, Lower, Upper, &Static) );

              /* Mapping i-th abstarct machine 
                   on a new processor system   */    /*E0126*/

              wAMS->VMS = (s_VMS *)((SysHandle *)NewVMRef)->pP;
              coll_Insert(&(wAMS->VMS->AMSColl),wAMS);/* into the list
                                                         of machines
                                                         mapped on processor
                                                         system */    /*E0127*/
              break;   /* to the next abstract machine */    /*E0128*/
           }
        }
     }

     /* Free memory */    /*E0129*/

     if(AlList == NULL)
        dvm_FreeArray(AlignList);

     if(LocBlockPtr == NULL)
     {  dvm_FreeArray(LocBlockArray);
        dvm_FreeArray(LocBlock);
     }
  }

  dvm_FreeArray(SizeArray);
  dvm_FreeArray(Lower);
  dvm_FreeArray(Upper);

  return;
}


#endif   /*  _DISTRIB_C_  */    /*E0130*/
