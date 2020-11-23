#ifndef _PARLOOP_C_
#define _PARLOOP_C_

#include "system.typ"

/*****************/    /*E0000*/

/*************************************\
* Functions to describe parallel loop *
\*************************************/    /*E0001*/

LoopRef  __callstd crtpl_(DvmType *RankPtr)

/*
         Creating parallel loop.
         -----------------------

*RankPtr - rank of the parallel loop.

The function crtpl_ creates the parallel loop.
The function returns reference to the created object.
*/    /*E0002*/

{ LoopRef         Res;
  SysHandle      *LoopHandlePtr;
  s_PARLOOP      *PL;
  s_ENVIRONMENT  *NewEnv, *Env;
  int             i, Rank;

  DVMFTimeStart(call_crtpl_);

  Rank = (int)*RankPtr;

  if(RTL_TRACE)
     dvm_trace(call_crtpl_,"Rank=%d;\n", Rank);

  if(Rank > MAXARRAYDIM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.000: wrong call crtpl_ "
              "(loop rank=%d > %d)\n", Rank, (int)MAXARRAYDIM);

  if(Rank <= 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.001: wrong call crtpl_ "
              "(loop rank=%d <= 0)\n", Rank);

  dvm_AllocStruct(s_PARLOOP, PL);

  Env = genv_GetCurrEnv();
  NewEnv = env_Init(Env->AMHandlePtr);

  PL->CrtEnvInd = gEnvColl->Count - 1; /* context index in which
                                          parallel loop is created */    /*E0003*/
  coll_Insert(gEnvColl, NewEnv);

  PL->EnvInd = gEnvColl->Count - 1;    /* current context index */    /*E0004*/

  PL->HasAlnLoc     = FALSE;
  PL->HasLocal      = FALSE;
  PL->Empty         = FALSE;
  PL->Rank          = Rank;
  PL->Align         = NULL;
  PL->AMView        = NULL;
  PL->MapList       = NULL;
  PL->AlnLoc        = NULL;
  PL->Local         = NULL;
  PL->Set           = NULL;
  PL->IterFlag      = ITER_NORMAL;
  PL->AddBnd        = 0;
  PL->CurrBlock     = 0;
  PL->IsInIter      = 0;
  PL->IsWaitShd     = 0;
  PL->BGRef         = 0;
  PL->PLQ           = NULL;
  PL->DoQuantum     = 0;
  PL->ret_dopl_time = 0.;
  PL->WaitRedSign   = 0;
  PL->QDimSign      = 0; /* */    /*E0005*/

  PL->QDimNumber    = 0; /* */    /*E0006*/
  PL->AcrType1      = 0; /* scheme ACROSS <sendsh_ - recvsh_>
                            will not be executed  */    /*E0007*/
  PL->AcrType2      = 0; /* scheme ACROSS <sendsa_ - recvla_>
                            will not be executed */    /*E0008*/

  PL->IsAcrType1Wait = 0; /* no waiting recvsh_ */    /*E0009*/
  PL->IsAcrType2Wait = 0; /* no waiting recvla_ */    /*E0010*/

  /* Pointers to edge groups for scheme ACROSS */    /*E0011*/

  PL->AcrShadowGroup1Ref = 0;
  PL->AcrShadowGroup2Ref = 0;
  PL->OldShadowGroup1Ref = 0;
  PL->NewShadowGroup1Ref = 0;
  PL->OldShadowGroup2Ref = 0;
  PL->NewShadowGroup2Ref = 0;

  PL->PipeLineSign   = 0; /* ACROSS loop is executed without pipeline */    /*E0012*/
  PL->IsPipeLineInit = 0; /* pipeline has not been initialized */    /*E0013*/
  PL->AcrossGroup    = 0; /* number of groups to partition quanted dimension
                             for ACROSS scheme execution */    /*E0014*/
  PL->AcrossQNumber  = 0; /* number of portion  the quanted dimension of
                             the local part is to be partitioned in
                             for ACROSS scheme execution */    /*E0015*/
  PL->Tc             = 0.; /* one iteration execution time */    /*E0016*/
  PL->SetTcSign      = 0;  /* one iteration execution time is not to be measured */    /*E0017*/

  for(i=0; i < Rank; i++)
  {  PL->LowShdWidth[i]  = 0;
     PL->HighShdWidth[i] = 0;
     PL->LowShd[i]  = 0;
     PL->HighShd[i] = 0;
     PL->InitIndex[i]    = 0;
     PL->Invers[i]       = 0;
     PL->PipeLineAxis1[i] = 0;
     PL->PipeLineAxis2[i] = 0;
  }
  for (i = 0; i < MAXARRAYDIM; i++)
      PL->PLAxis[i] = 0;

  NewEnv->ParLoop = PL; /* new context - parallel loop */    /*E0018*/

  NewEnv->EnvProcCount = Env->EnvProcCount;/* number of processors
                                          performing thread of context */    /*E0019*/
  OldEnvProcCount = CurrEnvProcCount; /* number of processors
                                         performing current thread */    /*E0020*/
  OldAMHandlePtr  = CurrAMHandlePtr;  /* current abstract machine */    /*E0021*/

  dvm_AllocStruct(SysHandle, LoopHandlePtr);

  *LoopHandlePtr = sysh_Build(sht_ParLoop, PL->EnvInd,
                              PL->CrtEnvInd, 0, PL);
  PL->HandlePtr = LoopHandlePtr; /* pointer to own Handle */    /*E0022*/

  Res = (LoopRef)LoopHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_crtpl_,"LoopRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0023*/
  DVMFTimeFinish(ret_crtpl_);
  return  (DVM_RET, Res);
}



DvmType  __callstd mappl_(LoopRef *LoopRefPtr, PatternRef *PatternRefPtr,
                          DvmType AxisArray[], DvmType CoeffArray[],
                          DvmType ConstArray[], AddrType LoopVarAddrArray[],
                          DvmType LoopVarTypeArray[],
                          DvmType InInitIndexArray[], DvmType InLastIndexArray[],
                          DvmType InStepArray[], DvmType OutInitIndexArray[],
                          DvmType OutLastIndexArray[], DvmType OutStepArray[])

/*
*LoopRefPtr       - reference to the parallel loop.
*PatternRefPtr	  - reference to the pattern
                    of the parallel loop mapping.
AxisArray	  - AxisArray[j] is a dimension number of the parallel
                    loop (that is the number of the index variable) used
                    in linear alignment rule for the pattern (j+1)th
                    dimension.
CoeffArray	  - CoeffArray[j] is a coefficient for the parallel loop
                    index variable used in linear alignment rule for the
                    pattern (j+1)-th dimension.
ConstArray	  - ConstArray[j] is a constant used in the linear
                    alignment rule for the pattern (j+1)th dimension.
LoopVarAddrArray  - LoopVarAddrArray[i] is an address (casted to
                    'AddrType') of the index variable of the parallel
                    loop (i+1)-th dimension.
LoopVarTypeArray  - LoopVarTypeArray[i] is a type of the index variable
                    of the loop (i+1)-th dimension.
InInitIndexArray  - input array,InInitIndexArray[i] is an initial value
                    for the index variable of the parallel loop (i+1)-th
                    dimension.
InLastIndexArray  - input array,InLastIndexArray[i] is a last value for
                    the index variable of the parallel loop (i+1)-th
                    dimension;.
InStepArray	  - input array,InStepArray[i] is a step value for
                    the index variable of the parallel loop (i+1)-th
                    dimension.
OutInitIndexArray - output array,OutInitIndexArray[i] is  calculated
                    initial value for the index variable of the parallel
                    loop (i+1)-th dimension.
OutLastIndexArray - output array,OutLastIndexArray[i] is calculated last
                    value for the index variable of the parallel loop
                    (i+1)-th dimension.
OutStepArray	  - output array,OutStepArray[i] is calculated step
                    value for the index variable of the parallel loop
                    (i+1)-th dimension.

The function mappl_ creates regular mapping of the parallel loop onto
the abstract machine representation,according to the defined mapping
rules and the descriptions of the loop dimensions.
The function returns non-zero value if mapped parallel loop has a local
part on the current processor,otherwise - returns zero.
*/    /*E0024*/

{ s_DISARRAY      *TempDArr = NULL, *DA;
  s_SPACE         *TSpace, *AMVSpace, PLSpace;
  int              LR, TR, ALSize, i, j, PLAxis, TAxis, NotSubSys,
                   GroupNumber;
  s_AMVIEW        *TempAMV=NULL;
  SysHandle       *TempHandlePtr, *LoopHandlePtr, *ArrayHandlePtr=NULL;
  s_PARLOOP       *PL;
  s_ALIGN         *TAlign = NULL, *AlList, *ResList;
  byte             TstArray[MAXARRAYDIM];
  DvmType             lv, LowPoint, HiPoint;
  s_BLOCK         *Local, Block;
  s_ALIGN          aAl, tAl;
  DvmType             InInit[MAXARRAYDIM], InLast[MAXARRAYDIM],
                   InStep[MAXARRAYDIM];
  s_ENVIRONMENT   *Env;
  s_AMS           *AMS;
  byte             CondTrue = TRUE;
  s_BOUNDGROUP    *BGPtr, *BG;
  s_SHDWIDTH      *ShdWidth;

  double           tdouble;
  s_PLQUANTUM     *PLQ;
  s_MAP           *Map;

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0025*/
  DVMFTimeStart(call_mappl_);

  Env = genv_GetCurrEnv();  /* to store number of processors
                               executing current thread */    /*E0026*/

  if(TstObject)
  {  if(!TstDVMObj(PatternRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.010: wrong call mappl_\n"
                 "(the pattern is not a DVM object; "
                 "PatternRef=%lx)\n", *PatternRefPtr);
  }

  TempHandlePtr = (SysHandle *)*PatternRefPtr;

  switch(TempHandlePtr->Type)
  {  case sht_DisArray:

     if(RTL_TRACE)
        dvm_trace(call_mappl_,
                  "LoopRefPtr=%lx; LoopRef=%lx; "
                  "PatternRefPtr=%lx; PatternRef=%lx (DisArray);\n",
                  (uLLng)LoopRefPtr, *LoopRefPtr,
                  (uLLng)PatternRefPtr, *PatternRefPtr);

     TempDArr = (s_DISARRAY *)TempHandlePtr->pP;
     TempAMV  = TempDArr->AMView;
     TAlign   = TempDArr->Align;
     TSpace   = &(TempDArr->Space);
     break;

     case sht_AMView:

     if(RTL_TRACE)
        dvm_trace(call_mappl_,
                  "LoopRefPtr=%lx; LoopRef=%lx; "
                  "PatternRefPtr=%lx; PatternRef=%lx (AMView);\n",
                  (uLLng)LoopRefPtr, *LoopRefPtr,
                  (uLLng)PatternRefPtr, *PatternRefPtr);

     TempAMV = (s_AMVIEW *)TempHandlePtr->pP;
     TSpace  = &(TempAMV->Space);
     break;

     default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 060.011: wrong call mappl_\n"
                       "(the object is not a mapping pattern; "
                       "PatternRef=%lx)\n",
                       *PatternRefPtr);
  }

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.013: wrong call mappl_\n"
              "(the object is not a parallel loop; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 060.012: wrong call mappl_\n"
                "(the current context is not the parallel loop; "
                "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  /* Check if parallel loop and mapping pattern are mapped */    /*E0027*/

  if(PL->AMView || PL->Empty)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.014: wrong call mappl_\n"
              "(the parallel loop has already been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(TempAMV == NULL || TempAMV->VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.015: wrong call mappl_\n"
              "(the pattern has not been mapped; "
              "PatternRef=%lx)\n", *PatternRefPtr);

  LR     = PL->Rank;
  TR     = TSpace->Rank;
  ALSize = LR +TR;

  if( EnableDynControl && ( !Trace.vtr || (Trace.vtr && *Trace.vtr) ))
  {
      for( i = 0; i < LR; i++ )
      {    dyn_DefinePrivate( (void *)LoopVarAddrArray[i], (byte)0 );
      }
  }

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_mappl_))
     {  for(i=0; i < TR; i++)
            tprintf("       AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
            tprintf("      CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
            tprintf("      ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("LoopVarAddrArray[%d]=%lx; ",
                     i, LoopVarAddrArray[i]);
        tprintf(" \n");

        if(LoopVarTypeArray)
        {  for(i=0; i < LR; i++)
               tprintf("LoopVarTypeArray[%d]=%lx; ",
                       i, LoopVarTypeArray[i]);
        }
        else
           tprintf("LoopVarTypeArray = NULL;\n");

        tprintf(" \n");

        for(i=0; i < LR; i++)
            tprintf("InInitIndexArray[%d]=%ld; ",
                     i, InInitIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("InLastIndexArray[%d]=%ld; ",
                     i, InLastIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("     InStepArray[%d]=%ld; ", i, InStepArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  for(i=0; i < LR; i++)
  {  OutInitIndexArray[i] = 0;
     OutLastIndexArray[i] = 0;
     OutStepArray[i] = 0;
  }

  /* Check correctness of given parameters */    /*E0028*/

  if(LoopVarTypeArray) /* whether array of parallel loop
                          index variable types is given */    /*E0029*/
  {  for(i=0; i < LR; i++)
     {  if(LoopVarTypeArray[i] < 0 || LoopVarTypeArray[i] > 3)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.019: wrong call mappl_\n"
                 "(invalid LoopVarTypeArray[%d]=%ld; LoopRef=%lx)\n",
                 i, LoopVarTypeArray[i], *LoopRefPtr);
     }
  }

  for(i=0; i < LR; i++)
  {  if(InInitIndexArray[i] > InLastIndexArray[i])
     {  if(InStepArray[i] >= 0)
        {  OutInitIndexArray[i] = 2;
           OutLastIndexArray[i] = 1;
           OutStepArray[i] = 1;
           break;
        }

        PL->Invers[i] = 1;
        InStep[i] = -InStepArray[i];
        InLast[i] = InInitIndexArray[i];
        InInit[i] = InLastIndexArray[i];

        lv = (InInitIndexArray[i] - InLastIndexArray[i]) % InStep[i];
        if(lv)
           InInit[i] += lv;
     }
     else
     {  if(InStepArray[i] <= 0)
        {  OutInitIndexArray[i] = 1;
           OutLastIndexArray[i] = 2;
           OutStepArray[i] = -1;
           break;
        }

        PL->Invers[i] = 0;
        InStep[i] = InStepArray[i];
        InInit[i] = InInitIndexArray[i];
        InLast[i] = InLastIndexArray[i];
     }
  }

  /* Check if pattern is mapped on current
       processor system or its subsystem   */    /*E0030*/

  NotSubsystem(NotSubSys, DVM_VMS, TempAMV->VMS)

  if(i < LR || NotSubSys)
  {
     if(NotSubSys)
     {  for(i=0; i < LR; i++)
        {  OutInitIndexArray[i] = 9;
           OutLastIndexArray[i] = 8;
           OutStepArray[i] = 1;
        }
     }

     if(RTL_TRACE)
     {  dvm_trace(ret_mappl_,"IsLocal=0000000;\n");

        if(mappl_Trace && TstTraceEvent(ret_mappl_))
        {  for(i=0; i < LR; i++)
               tprintf("OutInitIndexArray[%d]=%ld; ",
                        i,OutInitIndexArray[i]);
           tprintf(" \n");
           for(i=0; i < LR; i++)
               tprintf("OutLastIndexArray[%d]=%ld; ",
                        i,OutLastIndexArray[i]);
           tprintf(" \n");
           for(i=0; i < LR; i++)
               tprintf("     OutStepArray[%d]=%ld; ",
                        i,OutStepArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     PL->Empty = 1; /* flag of special exit
                       from mappl_: empty loop */    /*E0031*/

     NewEnvProcCount   = CurrEnvProcCount;
     Env->EnvProcCount = CurrEnvProcCount;

     NewAMHandlePtr    = CurrAMHandlePtr;
     Env->AMHandlePtr  = CurrAMHandlePtr;

     StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0032*/
     DVMFTimeFinish(ret_mappl_);
     return  (DVM_RET, 0);
  }

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] < -1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.020: wrong call mappl_\n"
                 "(AxisArray[%d]=%ld < -1; LoopRef=%lx)\n",
                 i, AxisArray[i], *LoopRefPtr);

     if(AxisArray[i] > LR)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.021: wrong call mappl_\n"
                 "(AxisArray[%d]=%ld > %d; LoopRef=%lx)\n",
                 i, AxisArray[i], LR, *LoopRefPtr);

     if(AxisArray[i] >= 0)
     {  if(CoeffArray[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 060.022: wrong call mappl_\n"
                    "(CoeffArray[%d]=%ld < 0; LoopRef=%lx)\n",
                    i, CoeffArray[i], *LoopRefPtr);

        if(CoeffArray[i] != 0)
        { if(AxisArray[i] == 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 060.023: wrong call mappl_\n"
                      "(AxisArray[%d]=0; LoopRef=%lx)\n",
                      i, *LoopRefPtr);

          lv = CoeffArray[i] * InInitIndexArray[AxisArray[i]-1] +
               ConstArray[i];

          if(lv < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 060.024: wrong call mappl_\n"
                "( (CoeffArray[%d]=%ld) * (InInitIndexArray[%ld]=%ld)"
                " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                i, CoeffArray[i], AxisArray[i]-1,
                InInitIndexArray[AxisArray[i]-1], i, ConstArray[i],
                *LoopRefPtr);

          if(lv >= TSpace->Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.025: wrong call mappl_\n"
                 "( (CoeffArray[%d]=%ld) * (InInitIndexArray[%ld]=%ld)"
                 " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                 i, CoeffArray[i], AxisArray[i]-1,
                 InInitIndexArray[AxisArray[i]-1], i, ConstArray[i],
                 TSpace->Size[i], *LoopRefPtr);

          lv = CoeffArray[i] * InLastIndexArray[AxisArray[i]-1] +
               ConstArray[i];

          if(lv < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 060.026: wrong call mappl_\n"
                "( (CoeffArray[%d]=%ld) * (InLastIndexArray[%ld]=%ld)"
                " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                i, CoeffArray[i], AxisArray[i]-1,
                InLastIndexArray[AxisArray[i]-1], i, ConstArray[i],
                *LoopRefPtr);

          if(lv >= TSpace->Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.027: wrong call mappl_\n"
                 "( (CoeffArray[%d]=%ld) * (InLastIndexArray[%ld]=%ld)"
                 " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                 i, CoeffArray[i], AxisArray[i]-1,
                 InLastIndexArray[AxisArray[i]-1], i, ConstArray[i],
                 TSpace->Size[i], *LoopRefPtr);
        }
        else
        { if(ConstArray[i] < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 060.028: wrong call mappl_\n"
                    "(ConstArray[%d]=%ld < 0; LoopRef=%lx)\n",
                    i, ConstArray[i], *LoopRefPtr);
          if(ConstArray[i] >= TSpace->Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 060.029: wrong call mappl_\n"
                    "(ConstArray[%d]=%ld >= %ld; LoopRef=%lx)\n",
                    i, ConstArray[i], TSpace->Size[i], *LoopRefPtr);
        }
     }
  }

  for(i=0; i < LR; i++)
      TstArray[i] = 0;

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
        continue;

     if(TstArray[AxisArray[i]-1])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 060.030: wrong call mappl_\n"
                "(AxisArray[%d]=AxisArray[%d]=%ld; LoopRef=%lx)\n",
                (int)(TstArray[AxisArray[i]-1]-1), i, AxisArray[i],
                *LoopRefPtr);

     TstArray[AxisArray[i]-1] = (byte)(i+1);
  }

  /* Save pointer to representation pattern if the pattern is array
     and save array dimension number for every loop dimension */    /*E0033*/

  PL->TempDArr = TempDArr;

  if(TempDArr)
  {  if(PL->AddBnd)
        PL->IterFlag = ITER_NORMAL;

     for(i=0; i < LR; i++)
         PL->DArrAxis[i] = 0;

     for(i=0; i < TR; i++)
         if(AxisArray[i] > 0)
            PL->DArrAxis[AxisArray[i]-1] = i+1;
  }
  else
     PL->AddBnd = 0;

  /***********************\
  * Mapping parallel loop *
  \***********************/    /*E0034*/

  /* Save initial values of indexes */    /*E0035*/

  for(i=0; i < LR; i++)
      PL->InitIndex[i] = InInit[i];

  dvm_AllocArray(s_ALIGN,ALSize,AlList);

  /* Preliminary  filling in first and second parts of AlList */    /*E0036*/

  for(i=0; i < LR; i++)
  {  AlList[i].Attr  = align_COLLAPSE;
     AlList[i].Axis  = (byte)(i+1);
     AlList[i].TAxis = 0;
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  for(i=LR; i < ALSize; i++)
  {  AlList[i].Attr  = align_NORMTAXIS;
     AlList[i].Axis  = 0;
     AlList[i].TAxis = (byte)(i-LR+1);
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  /* Form AlList in accordance with given parameters */    /*E0037*/

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] == -1)
     {  AlList[i+LR].Attr  = align_REPLICATE;
        AlList[i+LR].Axis  = 0;
	AlList[i+LR].TAxis = (byte)(i+1);
        AlList[i+LR].A     = 0;
        AlList[i+LR].B     = 0;
     }
     else
     {  if(CoeffArray[i]==0)
        {  AlList[i+LR].Attr  = align_CONSTANT;
           AlList[i+LR].Axis  = 0;
           AlList[i+LR].TAxis = (byte)(i+1);
           AlList[i+LR].A     = 0;
           AlList[i+LR].B     = ConstArray[i];
        }
        else
        {  AlList[i+LR].Axis = (byte)AxisArray[i];
	   AlList[i+LR].A = CoeffArray[i];
           AlList[i+LR].B = ConstArray[i]+CoeffArray[i]*
                            InInit[AxisArray[i]-1];

	   AlList[AxisArray[i]-1].Attr = align_NORMAL;
	   AlList[AxisArray[i]-1].Axis = (byte)AxisArray[i];
	   AlList[AxisArray[i]-1].TAxis = (byte)(i+1);
	   AlList[AxisArray[i]-1].A = CoeffArray[i];
           AlList[AxisArray[i]-1].B = ConstArray[i]+CoeffArray[i]*
                                           InInit[AxisArray[i]-1];
        }
     }
  }

  /* Perform superposition of mappings */    /*E0038*/

  AMVSpace = &(TempAMV->Space);
  ALSize = LR + AMVSpace->Rank;

  if(TAlign)
  {  dvm_AllocArray(s_ALIGN, ALSize, ResList);

     for(i=0; i < AMVSpace->Rank; i++)
         ResList[i+LR] = TAlign[i+TR];

     for(i = 0; i < LR; i++)
     {  aAl = AlList[i];

        if(aAl.Attr == align_NORMAL)
        {  tAl = TAlign[aAl.TAxis - 1];

           switch (tAl.Attr)
           {  case align_NORMAL   : aAl.TAxis = tAl.TAxis;
                                    aAl.A *= tAl.A;
                                    aAl.B = aAl.B * tAl.A + tAl.B;
                                    ResList[i] = aAl;
                                    ResList[LR+aAl.TAxis-1].Axis =
                                    (byte)(i+1);
                                    ResList[LR+aAl.TAxis-1].A = aAl.A;
                                    ResList[LR+aAl.TAxis-1].B = aAl.B;
                                    break;
              case align_COLLAPSE : aAl.TAxis = 0;
                                    aAl.Attr  = align_COLLAPSE;
                                    ResList[i] = aAl;
                                    break;
           }
        }
        else
           ResList[i] = aAl;  /* if COLLAPSE */    /*E0039*/
     }

     for(i=0; i < TR; i++)
     {  aAl = AlList[i+LR];

        switch (aAl.Attr)
        { case align_CONSTANT : tAl = TAlign[aAl.TAxis-1];
                                if (tAl.Attr == align_NORMAL)
                                {  aAl.TAxis = tAl.TAxis;
                                   aAl.B = tAl.A*aAl.B + tAl.B;
				   ResList[LR+tAl.TAxis-1] = aAl;
				}
				break;
	  case align_REPLICATE: tAl = TAlign[aAl.TAxis-1];
				if (tAl.Attr == align_NORMAL)
				{ aAl.TAxis = tAl.TAxis;

                                  if(tAl.A != 1 || tAl.B != 0)
                                  { aAl.Attr  = align_BOUNDREPL;
                                    aAl.A     = tAl.A;
                                    aAl.B     = tAl.B;
                                    aAl.Bound =
                                    TSpace->Size[tAl.TAxis-1];
                                  }

                                  ResList[LR+tAl.TAxis-1] = aAl;
				}
                                break;
       }
     }

     dvm_FreeArray(AlList);
     AlList = ResList;
  }

  /* Print resulting map */    /*E0040*/

  if(RTL_TRACE)
  {  if(mappl_Trace > 1 && TstTraceEvent(ret_mappl_))
     {  for(i=0; i < ALSize; i++)
            tprintf("LoopMap[%d]: Attr=%d Axis=%d TAxis=%d "
                    "A=%ld B=%ld Bound=%ld\n",
                    i,(int)AlList[i].Attr,(int)AlList[i].Axis,
                    (int)AlList[i].TAxis,AlList[i].A,AlList[i].B,
                    AlList[i].Bound);
        tprintf(" \n");
     }
  }

  /* Form resulting mapping */    /*E0041*/

  PL->AMView = TempAMV;

  dvm_AllocArray(s_ALIGN, ALSize, PL->Align);
  dvm_ArrayCopy(s_ALIGN, PL->Align, AlList, ALSize);

  dvm_AllocArray(s_LOOPMAPINFO, LR, PL->MapList);
  dvm_AllocArray(s_REGULARSET,  LR, PL->Set);
  dvm_AllocArray(s_REGULARSET,  LR, PL->Local);
  dvm_AllocArray(s_REGULARSET,  LR, PL->AlnLoc);

  for(i=0; i < LR; i++)
  {  if(PL->Invers[i])
     {  OutInitIndexArray[i] = 1;
        OutLastIndexArray[i] = 2;
     }
     else
     {  OutInitIndexArray[i] = 2;
        OutLastIndexArray[i] = 1;
     }

     OutStepArray[i] = InStep[i];

     PL->MapList[i].InitIndexPtr = &OutInitIndexArray[i];
     PL->MapList[i].LastIndexPtr = &OutLastIndexArray[i];
     PL->MapList[i].StepPtr      = &OutStepArray[i];
     PL->MapList[i].LoopVarAddr  = (void *)LoopVarAddrArray[i];

     if(LoopVarTypeArray)
        PL->MapList[i].LoopVarType  = (byte)LoopVarTypeArray[i];
     else
        PL->MapList[i].LoopVarType  = 1;   /* by default - type int */    /*E0042*/

     PL->Set[i].Lower = InInit[i];
     PL->Set[i].Upper = InLast[i];
     PL->Set[i].Size  = InLast[i] - InInit[i] + 1;
     PL->Set[i].Step  = InStep[i];

     PL->AlnLoc[i].Lower = -1;
     PL->AlnLoc[i].Upper = -2;
     PL->AlnLoc[i].Size  = 0;
     PL->AlnLoc[i].Step  = InStep[i];

     PL->Local[i].Lower = -1;
     PL->Local[i].Upper = -2;
     PL->Local[i].Size  = 0;
     PL->Local[i].Step  = InStep[i];
  }

  PLSpace.Rank = (byte)LR;

  for(i=0; i < LR; i++)
      PLSpace.Size[i] = PL->Set[i].Size;

  /* */    /*E0043*/

  TSpace = &TempAMV->VMS->Space;

  j = TSpace->Rank;    /* */    /*E0044*/
  for(i=0; i < j; i++)
      PL->PLAxis[i] = 0;

  for(i=0; i < LR; i++)
  {  ResList = &AlList[i];

     if(ResList->Attr == align_NORMAL)
     {  Map = &TempAMV->DISTMAP[ResList->TAxis - 1];

        if(Map->Attr == map_BLOCK)
           PL->PLAxis[Map->PAxis - 1] = i+1;
     }
  }

  if(mappl_Trace && TstTraceEvent(ret_mappl_))
     for(i=0; i < j; i++)
         tprintf("PL->PLAxis[%d]=%d\n", i, PL->PLAxis[i]);

  /* */    /*E0045*/

  NewEnvProcCount = 1;

  for(i=0; i < j; i++)
      if(PL->PLAxis[i] == 0)
         NewEnvProcCount *= TSpace->Size[i]; /* */    /*E0046*/

  /* -------------------------------------------------------------- */    /*E0047*/

  Local = NULL;

  if(TempAMV->VMS->HasCurrent)
     Local = GetSpaceLB4Proc(TempAMV->VMS->CurrentProc, TempAMV,
                             &PLSpace, AlList, NULL, &Block);

  if(Local == NULL)
  {  /* Loop without local part */    /*E0048*/

     NewEnvProcCount   = CurrEnvProcCount;
     Env->EnvProcCount = CurrEnvProcCount;

     NewAMHandlePtr    = CurrAMHandlePtr;
     Env->AMHandlePtr  = CurrAMHandlePtr;
  }
  else
  {  /* Loop with local part */    /*E0049*/

     Env->EnvProcCount = NewEnvProcCount; /* new number of processors
                                             performing current thread */    /*E0050*/

     AMS = coll_At(s_AMS *, &TempAMV->AMSColl, TempAMV->LocAMInd);
     NewAMHandlePtr   = AMS->HandlePtr; /* new abstract machine */    /*E0051*/
     Env->AMHandlePtr = NewAMHandlePtr;

     PL->HasAlnLoc = TRUE;

     /* extend the loop local part by edges of distributed array
        the loop is mapped on */    /*E0052*/

     if(PL->AddBnd)
     {  for (i = 0; i < LR; i++)
        {  j = PL->DArrAxis[i]-1; /* array dimension number - 1 */    /*E0053*/

           if(j < 0)
              continue;

           if(PL->AddBnd == 1)
           {  if(Local->Set[i].Lower - TempDArr->InitLowShdWidth[j] >= 0)
              {  Local->Set[i].Lower -= TempDArr->InitLowShdWidth[j];
                 Local->Set[i].Size += TempDArr->InitLowShdWidth[j];
              }

              if(Local->Set[i].Upper + TempDArr->InitHighShdWidth[j] <
                 PLSpace.Size[i])
              {  Local->Set[i].Upper += TempDArr->InitHighShdWidth[j];
                 Local->Set[i].Size += TempDArr->InitHighShdWidth[j];
              }
           }
           else
           {  if(Local->Set[i].Lower - PL->LowShd[j] >= 0)
              {  Local->Set[i].Lower -= PL->LowShd[j];
                 Local->Set[i].Size += PL->LowShd[j];
              }

              if(Local->Set[i].Upper + PL->HighShd[j] <
                 PLSpace.Size[i])
              {  Local->Set[i].Upper += PL->HighShd[j];
                 Local->Set[i].Size += PL->HighShd[j];
              }
           }
        }
     }

     /* Form local part not using step */    /*E0054*/

     for(i=0; i < LR; i++)
     {  PL->AlnLoc[i].Lower = Local->Set[i].Lower;
        PL->AlnLoc[i].Upper = Local->Set[i].Upper;
        PL->AlnLoc[i].Size  =
        Local->Set[i].Upper - Local->Set[i].Lower + 1;
        PL->AlnLoc[i].Step  = Local->Set[i].Step;
     }

     /* Calculate local part using step */    /*E0055*/

     for(i=0; i < LR; i++)
     {  lv = InStep[i];
        OutInitIndexArray[i] = lv * (DvmType)ceil((double)Local->Set[i].Lower / (double)lv );
        OutLastIndexArray[i] = (Local->Set[i].Upper / lv) * lv;

        if(OutInitIndexArray[i] > OutLastIndexArray[i])
           break;

        PL->Local[i].Lower = OutInitIndexArray[i];
        PL->Local[i].Upper = OutLastIndexArray[i];
        PL->Local[i].Size  =
        OutLastIndexArray[i] - OutInitIndexArray[i] + 1;
        PL->Local[i].Step  = OutStepArray[i];
     }

     if(i == LR)
     {  PL->HasLocal = TRUE;

        /* Correct output index values using
                 initial index values        */    /*E0056*/

        for(i=0; i < LR; i++)
        {  OutInitIndexArray[i] += InInit[i];
           OutLastIndexArray[i] += InInit[i];
        }

        /* Correct output index values  and step for
                    inverse loop execution           */    /*E0057*/

        for(i=0; i < LR; i++)
        {  if(PL->Invers[i])
           {  lv = OutInitIndexArray[i];
              OutInitIndexArray[i] = OutLastIndexArray[i];
              OutLastIndexArray[i] = lv;
              OutStepArray[i] *= -1;
           }
        }
     }
     else
     {  for(i=0; i < LR; i++)
        {  OutInitIndexArray[i] = -1;
           OutLastIndexArray[i] = -2;
           PL->Local[i] = rset_Build(-1, -2, OutStepArray[i]);
        }
     }
  }

  dvm_FreeArray(AlList);

  /***********************************************\
  * Form edge widths for parallel loop dimensions *
  \***********************************************/    /*E0058*/

  if(PL->IterFlag != ITER_NORMAL && PL->HasLocal)
  {  BGPtr = (s_BOUNDGROUP *)((SysHandle *)PL->BGRef)->pP;

     /*     Loop in edge group arrays aligned by representations
        equivalent to the representation in which the loop is mapped */    /*E0059*/

     for(j=0; j < BGPtr->ArrayColl.Count; j++)
     {  DA = coll_At(s_DISARRAY *,&BGPtr->ArrayColl,j);/* current array
                                                          of the group */    /*E0060*/

        GroupNumber = DA->BG.Count;

        for(NotSubSys=0; NotSubSys < GroupNumber; NotSubSys++)
        { BG = coll_At(s_BOUNDGROUP *, &DA->BG, NotSubSys);/* current
                                                              array group */    /*E0061*/
          if(BG != BGPtr)
             continue; /* current array edge group and
                          loop edge group are not the same */    /*E0062*/

          ShdWidth = coll_At(s_SHDWIDTH *, &DA->ResShdWidthColl,
                             NotSubSys);

          if(CondTrue /*IsAMViewEqu(DA->AMView, TempAMV)*/    /*E0063*/)
          {  /* Loop in array dimensions */    /*E0064*/

             ALSize = DA->Space.Rank;

             for(i=0; i < ALSize; i++)
             { if(DA->Align[i].Attr != align_NORMAL)
                  continue;

               TAxis = DA->Align[i].TAxis + LR - 1;

               if(PL->Align[TAxis].Attr != align_NORMTAXIS)
                  continue;

               PLAxis = PL->Align[TAxis].Axis - 1; /* number of loop
                                                    dimension minus 1 */    /*E0065*/

               PL->LowShdWidth[PLAxis] = (int)
               dvm_max( PL->LowShdWidth[PLAxis],
                        dvm_abs((DA->InitLowShdWidth[i] -
                                 ShdWidth->InitLowShdIndex[i]) *
                                DA->Align[i].A) );
               PL->HighShdWidth[PLAxis]= (int)
               dvm_max( PL->HighShdWidth[PLAxis],
                        dvm_abs((ShdWidth->ResHighShdWidth[i] +
                                 ShdWidth->InitHiShdIndex[i]) *
                                DA->Align[i].A) );
             }
          }
          else
          {  /*    In edge group connected to the loop there is
              representation non equivalent to the representation
                     on which the parallel loop in mapped         */    /*E0066*/

             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 060.040: wrong call mappl_\n"
                     "(LoopRef=%lx; ShadowGroupRef=%lx;\n"
                     "LoopAMViewRef=%lx not equ ShdGrpAMViewRef=%lx)\n",
                     *LoopRefPtr, PL->BGRef, (uLLng)TempAMV->HandlePtr,
                     (uLLng)DA->AMView->HandlePtr);
          }
        }
     }

     /* Calculate edge widths for parallel loop
               using it representation          */    /*E0067*/

     for(i=0; i < LR; i++)
     {  if(PL->Align[i].Attr != align_NORMAL || PL->Align[i].A == 0)
           continue;

        TAxis = PL->Align[i].TAxis - 1;

        LowPoint =
        (TempAMV->Local.Set[TAxis].Lower + PL->HighShdWidth[i] - 1 -
         PL->Align[i].B) / PL->Align[i].A;

        HiPoint =
        (TempAMV->Local.Set[TAxis].Upper - PL->LowShdWidth[i] + 1 -
         PL->Align[i].B) / PL->Align[i].A;

        LowPoint = dvm_min(LowPoint, PL->Local[i].Upper);
        HiPoint  = dvm_max(HiPoint, PL->Local[i].Lower);

        PL->HighShdWidth[i] =
        (int)(LowPoint - PL->Local[i].Lower + 1);
        PL->LowShdWidth[i] =
        (int)(PL->Local[i].Upper - HiPoint + 1);

        if(PL->HighShdWidth[i] < 0)
           PL->HighShdWidth[i] = 0;
        if(PL->LowShdWidth[i] < 0)
           PL->LowShdWidth[i] = 0;
     }
  }

  /******************************************\
  *  Prepare the tools to calculate time of  *
  *      execution of iteration quants       *
  \******************************************/    /*E0068*/

  if((PLTimeMeasure || TempAMV->TimeMeasure) && PL->HasLocal)
  {  dvm_AllocStruct(s_PLQUANTUM, PL->PLQ);
     PLQ = PL->PLQ;

     /* Search loop dimensions distributed on quant
            dimensions of its processor system      */    /*E0069*/

     for(i=LR-1,j=0; i >= 0; i--)
     {  PLQ->QSign[i] = 0;         /* non quant loop dimension */    /*E0070*/
        PLQ->PLGroupNumber[i] = 0; /* loop  dimension is
                                      not divided into groups */    /*E0071*/

        if(PL->Align[i].Attr != align_NORMAL)
           continue;   /* loop dimension is not distributed */    /*E0072*/
        TAxis = PL->Align[i].TAxis - 1; /* number of representation
                                           dimension AM minus 1    */    /*E0073*/
        if(TempAMV->DISTMAP[TAxis].Attr != map_BLOCK)
           continue;   /* representation dimension AM not distributed */    /*E0074*/

        GroupNumber = TempAMV->GroupNumber[TAxis]; /* number of partitioning
                                                      groups for AM
                                                      representation */    /*E0075*/
        if(PLTimeMeasure)
        {  TAxis =
           TempAMV->DISTMAP[TAxis].PAxis - 1; /* number of dimension of
                                                 processor system minus 1 */    /*E0076*/
           GroupNumber = dvm_max(GroupNumber, PLGroupNumber[TAxis]);
        }

        if(GroupNumber > 0)
        {  /* Loop dimension is quanted */    /*E0077*/

           j++;               /* number of quant loop dimensions */    /*E0078*/
           PLQ->QSign[i] = 1; /* flag: quant loop dimension */    /*E0079*/
           PLQ->PLGroupNumber[i] = GroupNumber; /* number of
                                                   partition groups */    /*E0080*/
        }
     }

     if(j == 0)
     {  /* No quant loop dimension */    /*E0081*/

        dvm_FreeStruct(PL->PLQ);
     }
     else
     {  /* At least one quant loop dimension found */    /*E0082*/

        if(MeasurePL == NULL)
           MeasurePL = PL;/* pointer to loop descriptor
                             which iteration execution time should be measured */    /*E0083*/

        SYSTEM(strncpy, (PLQ->DVM_FILE, DVM_FILE[0], MaxParFileName))
        PLQ->DVM_FILE[MaxParFileName] = '\x00';
        PLQ->DVM_LINE = DVM_LINE[0];
        PLQ->PLRank = LR;
        PLQ->QCount = 0;  /* number of occupied elements of QTime array */    /*E0084*/

        for(j=0; j < LR; j++)
        {  PLQ->InitIndex[j] = PL->Set[j].Lower;
           PLQ->LastIndex[j] = PL->Set[j].Upper;
           PLQ->PLStep[j]    = PL->Set[j].Step;
           PLQ->Invers[j]    = PL->Invers[j];
           PLQ->QSize[j]     = 0; /* null quant value for
                                     non quant loop dimension */    /*E0085*/
        }

        /* Calculation of quant values for quant loop dimensions */    /*E0086*/

        for(i=0; i < LR; i++)
        {  if(PLQ->QSign[i] == 0)
              continue;  /* non quant loop dimension */    /*E0087*/

           tdouble = ceil( (double)(PL->Set[i].Size * PL->Set[i].Step) /
                           (double)PLQ->PLGroupNumber[i] );
           PLQ->QSize[i] = (DvmType)((double)PLQ->PLStep[i] * ceil(tdouble /
                                   (double)PLQ->PLStep[i]) );
        }

        /* Calculation of total number of quants
            for all portions of loop iterations  */    /*E0088*/

        for(j=0,lv=1; j < LR; j++)
        {  if(PLQ->QSign[j])
               lv *= (DvmType)ceil((double)PL->AlnLoc[j].Size / (double)PLQ->QSize[j] );
        }

        PLQ->QNumber = lv; /* number of quanys for internal domain */    /*E0089*/

        for(j=0; j < LR; j++)
        {  if(PLQ->QSign[j])
           { tdouble = ceil( (double)PL->LowShdWidth[j] /
                             (double)PLQ->QSize[j] ) +
                       ceil( (double)PL->HighShdWidth[j] /
                             (double)PLQ->QSize[j] );
             tdouble *= ceil( (double)lv /
                              ceil( (double)PL->AlnLoc[j].Size /
                                    (double)PLQ->QSize[j] ) );
           }
           else
             tdouble = lv + lv;

           PLQ->QNumber += (DvmType)tdouble; /* + number of quants for upper
                                             and lower limits in
                                             loop dimension j+1 */    /*E0090*/
        }

        /*  Memory allocation for saving
           iteration quant execution time */    /*E0091*/

        PLQ->QNumber++;
        dvm_AllocArray(s_QTIME, PLQ->QNumber, PLQ->QTime);
     }
  }

  /* -------------------------------------------------- */    /*E0092*/

  if(RTL_TRACE)
  {  dvm_trace(ret_mappl_,"IsLocal=%d;\n",(int)PL->HasLocal);

     if(mappl_Trace && TstTraceEvent(ret_mappl_))
     {  for(i=0; i < LR; i++)
            tprintf("OutInitIndexArray[%d]=%ld; ",
                    i, OutInitIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("OutLastIndexArray[%d]=%ld; ",
                    i, OutLastIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("     OutStepArray[%d]=%ld; ",i,OutStepArray[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("      LowHsdWidth[%d]=%ld; ",
            i, (DvmType)PL->LowShdWidth[i]);
        tprintf(" \n");
        for(i=0; i < LR; i++)
            tprintf("     HighShdWidth[%d]=%ld; ",
            i, (DvmType)PL->HighShdWidth[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0093*/
  DVMFTimeFinish(ret_mappl_);
  return  (DVM_RET, (DvmType)PL->HasLocal);
}



ArrayMapRef __callstd plmap_(LoopRef  *LoopRefPtr, DvmType  *StaticSignPtr)
/*
            Getting map of a parallel loop.
            -------------------------------

*LoopRefPtr    - reference to the parallel loop.
*StaticSignPtr - sign of static map creation.

Function plmap_  creates an object (map), describing mapping
of the parallel loop onto an  abstract machine representation
and returns reference to the created object.
*/    /*E0094*/

{ ArrayMapRef     Res;
  SysHandle      *LoopHandlePtr, *MapHandlePtr = NULL;
  s_AMVIEW       *AMV;
  s_PARLOOP      *PL;
  s_ARRAYMAP     *Map;
  int             ALSize;

  StatObjectRef = (ObjectRef)*LoopRefPtr;   /* for statistics */    /*E0095*/
  DVMFTimeStart(call_plmap_);

  if(RTL_TRACE)
     dvm_trace(call_plmap_,
               "LoopRefPtr=%lx; LoopRef=%lx; StaticSign=%ld;\n",
               (uLLng)LoopRefPtr, *LoopRefPtr, *StaticSignPtr);

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.051: wrong call plmap_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 060.050: wrong call plmap_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.052: wrong call plmap_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(PL->AMView && PL->Align)
  {  /* Non empty parallel loop */    /*E0096*/

     dvm_AllocStruct(SysHandle, MapHandlePtr);
     dvm_AllocStruct(s_ARRAYMAP, Map);

     Map->Static    = (byte)*StaticSignPtr;
     Map->ArrayRank = (byte)PL->Rank;

     AMV             = PL->AMView;
     Map->AMView     = AMV;
     Map->AMViewRank = AMV->Space.Rank;

     ALSize          = PL->Rank + AMV->Space.Rank;

     dvm_AllocArray(s_ALIGN, ALSize, Map->Align);
     dvm_ArrayCopy(s_ALIGN, Map->Align, PL->Align, ALSize);

     *MapHandlePtr  = genv_InsertObject(sht_ArrayMap, Map);
     Map->HandlePtr = MapHandlePtr;  /* pointer to own Handle */    /*E0097*/
     if(TstObject)
        InsDVMObj((ObjectRef)MapHandlePtr);
  }

  Res = (ArrayMapRef)MapHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_plmap_,"ArrayMapRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res;   /* for statistics */    /*E0098*/
  DVMFTimeFinish(ret_plmap_);
  return  (DVM_RET, Res);
}



DvmType  __callstd exfrst_(LoopRef  *LoopRefPtr,
                        ShadowGroupRef  *ShadowGroupRefPtr)

/*
      Reordering parallel loop execution.
      -----------------------------------

*LoopRefPtr        - reference to the parallel loop.
*ShadowGroupRefPtr - reference to the shadow edge group,which will be
                     renewed after computing the exported elements from
                     the local parts of the distributed arrays.
The function returns zero.
*/    /*E0099*/

{ SysHandle   *LoopHandlePtr, *BGHandlePtr;
  s_PARLOOP   *PL;
  int          CurrEnvInd;
  void        *CurrAM;

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0100*/
  DVMFTimeStart(call_exfrst_);

  if(RTL_TRACE)
     dvm_trace(call_exfrst_,
               "LoopRefPtr=%lx; LoopRef=%lx; "
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)LoopRefPtr, *LoopRefPtr,
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.061: wrong call exfrst_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 060.060: wrong call exfrst_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  if(PL->AMView || PL->Empty)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.062: wrong call exfrst_\n"
              "(the parallel loop has already been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  BGHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(ShadowGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.065: wrong call exfrst_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 060.066: wrong call exfrst_\n"
       "(the object is not a shadow group; "
       "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0101*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0102*/

  if(BGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 060.067: wrong call exfrst_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BGHandlePtr->CrtEnvInd, CurrEnvInd);

  PL->IterFlag = ITER_BOUNDS_FIRST;
  PL->BGRef    = *ShadowGroupRefPtr;

  if(RTL_TRACE)
     dvm_trace(ret_exfrst_," \n");

  DVMFTimeFinish(ret_exfrst_);
  return  (DVM_RET, 0);
}



DvmType  __callstd imlast_(LoopRef  *LoopRefPtr,
                        ShadowGroupRef  *ShadowGroupRefPtr)

/*
      Reordering parallel loop execution.
      -----------------------------------

*LoopRefPtr        - reference to the parallel loop.
*ShadowGroupRefPtr - referenceto the shadow edge group,which renewing
                     completion the Run-Time Library awaits after the
                     computation of the internal points of the local
                     parts of the distributed arrays.

The function sets the following order of the parallel loop iterations:

1. Internal points of the local parts of the distributed arrays have
   been computed (without using  values of imported elements);
2. Run-Time Library awaits the completion of the shadow edge renewing;
3. The other elements of the local parts of the distributed arrays have
   been computed (with using values of imported elements).

The function returns zero.
*/    /*E0103*/

{ SysHandle   *LoopHandlePtr, *BGHandlePtr;
  s_PARLOOP   *PL;
  int          CurrEnvInd;
  void        *CurrAM;

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0104*/
  DVMFTimeStart(call_imlast_);

  if(RTL_TRACE)
     dvm_trace(call_imlast_,
               "LoopRefPtr=%lx; LoopRef=%lx; "
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)LoopRefPtr, *LoopRefPtr,
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.071: wrong call imlast_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 060.070: wrong call imlast_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  if(PL->AMView || PL->Empty)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.072: wrong call imlast_\n"
              "(the parallel loop has already been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  BGHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(ShadowGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.075: wrong call imlast_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 060.076: wrong call imlast_\n"
       "(the object is not a shadow group; "
       "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0105*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0106*/

  if(BGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 060.077: wrong call imlast_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BGHandlePtr->CrtEnvInd, CurrEnvInd);

  PL->IterFlag = ITER_BOUNDS_LAST;
  PL->BGRef    = *ShadowGroupRefPtr;

  if(RTL_TRACE)
     dvm_trace(ret_imlast_," \n");

  DVMFTimeFinish(ret_imlast_);
  return  (DVM_RET, 0);
}



DvmType  __callstd addbnd_(void)
{ s_PARLOOP   *PL;

  DVMFTimeStart(call_addbnd_);

  if(RTL_TRACE)
     dvm_trace(call_addbnd_, " \n");

  /* Check the current parallel loop */    /*E0107*/

  PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

  if(PL == NULL)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.200: wrong call addbnd_\n"
              "(the current context is not a parallel loop)\n");

  if(PL->AMView || PL->Empty)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.201: wrong call addbnd_\n"
              "(the current parallel loop has already been mapped)\n");

  if(PL->IterFlag == ITER_BOUNDS_FIRST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.210: wrong call addbnd_\n"
         "(type of the current parallel loop is ITER_BOUNDS_FIRST)\n");

  if(PL->IterFlag == ITER_BOUNDS_LAST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.211: wrong call addbnd_\n"
         "(type of the current parallel loop is ITER_BOUNDS_LAST)\n");

  PL->AddBnd = 1;

  if(RTL_TRACE)
     dvm_trace(ret_addbnd_," \n");

  DVMFTimeFinish(ret_addbnd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  addshd_(DvmType  ArrayHeader[], DvmType  LowShdWidthArray[], DvmType HiShdWidthArray[])
{ s_PARLOOP      *PL;
  SysHandle      *ArrayHandlePtr;
  s_DISARRAY     *ArrayDescPtr;
  int             i, Rank;
  s_AMVIEW       *AMV;

  DVMFTimeStart(call_addshd_);

  if(RTL_TRACE)
     dvm_trace(call_addshd_, "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                             (uLLng)ArrayHeader, ArrayHeader[0]);

  /* */    /*E0108*/

  PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

  if(PL == NULL)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.300: wrong call addshd_\n"
              "(the current context is not a parallel loop)\n");

  if(PL->AMView || PL->Empty)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.301: wrong call addshd_\n"
              "(the current parallel loop has already been mapped)\n");

  if(PL->IterFlag == ITER_BOUNDS_FIRST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.310: wrong call addshd_\n"
         "(type of the current parallel loop is ITER_BOUNDS_FIRST)\n");

  if(PL->IterFlag == ITER_BOUNDS_LAST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.311: wrong call addshd_\n"
         "(type of the current parallel loop is ITER_BOUNDS_LAST)\n");

  /* */    /*E0109*/

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.320: wrong call addshd_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV = ArrayDescPtr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.322: wrong call addshd_ "
              "(the array has not been aligned;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  Rank = ArrayDescPtr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_addshd_))
     {  for(i=0; i < Rank; i++)
            tprintf("LowShdWidthArray[%d]=%ld; ",i,LowShdWidthArray[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf(" HiShdWidthArray[%d]=%ld; ",i,HiShdWidthArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  for(i=0; i < Rank; i++)
  {
     if(LowShdWidthArray[i] < -1 ||
        LowShdWidthArray[i] > ArrayDescPtr->InitLowShdWidth[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.330: wrong call addshd_\n"
                 "(invalid LowShdWidthArray[%d]=%ld)\n",
                 i, LowShdWidthArray[i]);

     if(LowShdWidthArray[i] == -1)
        PL->LowShd[i] =
        ArrayDescPtr->InitLowShdWidth[i];
     else
        PL->LowShd[i] = (int)LowShdWidthArray[i];

     if(HiShdWidthArray[i] < -1 ||
        HiShdWidthArray[i] > ArrayDescPtr->InitHighShdWidth[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.332: wrong call addshd_\n"
                 "(invalid HiShdWidthArray[%d]=%ld)\n",
                 i, HiShdWidthArray[i]);

     if(HiShdWidthArray[i] == -1)
        PL->HighShd[i] =
        ArrayDescPtr->InitHighShdWidth[i];
     else
        PL->HighShd[i] = (int)HiShdWidthArray[i];
  }

  PL->AddBnd = 2;

  if(RTL_TRACE)
     dvm_trace(ret_addshd_," \n");

  DVMFTimeFinish(ret_addshd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  setba_(void)
{
  DVMFTimeStart(call_setba_);

  if(RTL_TRACE)
     dvm_trace(call_setba_, " \n");

  BoundAddition = 0;

  if(RTL_TRACE)
     dvm_trace(ret_setba_," \n");

  DVMFTimeFinish(ret_setba_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  rstba_(void)
{
  DVMFTimeStart(call_rstba_);

  if(RTL_TRACE)
     dvm_trace(call_rstba_, " \n");

  BoundAddition = -1;

  if(RTL_TRACE)
     dvm_trace(ret_rstba_," \n");

  DVMFTimeFinish(ret_rstba_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  doacr_(DvmType            *AcrossTypePtr,
                           ShadowGroupRef  *AcrShadowGroupRefPtr,
                           double          *PipeLineParPtr)
{ s_PARLOOP        *PL;
  byte              AcrType, CondTrue = 1;
  ShadowGroupRef    OldShadowGroupRef, NewShadowGroupRef;
  int               i, j, k, m, PLRank, DARank, ArrayCount, BGCount;
  DvmType              ErrPipeLine = 0,
                    NewMaxShdCount = 0, OldMaxShdCount = 0;
  s_BOUNDGROUP     *BG, *CurrBG;
  SysHandle        *AcrBGHandlePtr;
  void             *CurrAM;
  s_DISARRAY       *DArr;
  s_SHDWIDTH       *ShdWidth;
  int               PLAxis[MAXARRAYDIM];
  byte              ShdSign[MAXARRAYDIM];
  byte             *BytePtr;
  int              *IntPtr1, *IntPtr2;
  s_COLLECTION     *ColPtr;

  DvmType              NewInitDimIndex[MAXARRAYDIM],
                    NewLastDimIndex[MAXARRAYDIM],
                    NewInitLowShdIndex[MAXARRAYDIM],
                    NewLastLowShdIndex[MAXARRAYDIM],
                    NewInitHiShdIndex[MAXARRAYDIM],
                    NewLastHiShdIndex[MAXARRAYDIM],
                    NewShdSignArray[MAXARRAYDIM];

  DvmType              OldInitDimIndex[MAXARRAYDIM],
                    OldLastDimIndex[MAXARRAYDIM],
                    OldInitLowShdIndex[MAXARRAYDIM],
                    OldLastLowShdIndex[MAXARRAYDIM],
                    OldInitHiShdIndex[MAXARRAYDIM],
                    OldLastHiShdIndex[MAXARRAYDIM],
                    OldShdSignArray[MAXARRAYDIM];

  DVMFTimeStart(call_doacr_);

  AcrType = (byte)*AcrossTypePtr;

  if(RTL_TRACE)
     dvm_trace(call_doacr_,
               "AcrossType=%d; "
               "AcrShadowGroupRefPtr=%lx; AcrShadowGroupRef=%lx; "
               "PipeLinePar=%lf;\n", (int)AcrType,
               (uLLng)AcrShadowGroupRefPtr, *AcrShadowGroupRefPtr,
               *PipeLineParPtr);

  /* Check the current parallel loop */    /*E0110*/

  PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

  if(PL == NULL)  /* */    /*E0111*/
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.150: wrong call doacr_\n"
              "(the current context is not a parallel loop)\n");

  if(PL->Empty)               /* */    /*E0112*/
     epprintf(MultiProcErrReg2, __FILE__, __LINE__,
              "*** RTS err 060.151: wrong call doacr_\n"
              "(the current parallel loop is empty)\n");

  if(PL->AMView == NULL)      /* */    /*E0113*/
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.152: wrong call doacr_\n"
              "(the current parallel loop has not been mapped)\n");

  if(PL->TempDArr == NULL)    /* */    /*E0114*/
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.153: wrong call doacr_\n"
              "(the current parallel loop has not been mapped with"
              " a distributed array)\n");

  /* Check edge group, defined by *AcrShadowGroupRefPtr reference */    /*E0115*/

  if(TstObject)
  {  if(!TstDVMObj(AcrShadowGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.165: wrong call doacr_\n"
                 "(the across shadow group is not a DVM object; "
                 "AcrShadowGroupRef=%lx)\n", *AcrShadowGroupRefPtr);
  }

  AcrBGHandlePtr = (SysHandle *)*AcrShadowGroupRefPtr;

  if(AcrBGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.166: wrong call doacr_\n"
              "(the object is not a shadow group; "
              "AcrShadowGroupRef=%lx)\n", *AcrShadowGroupRefPtr);

  i      = gEnvColl->Count - 1; /* current context index */    /*E0116*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0117*/

  if(AcrBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.167: wrong call doacr_\n"
              "(the across shadow group was not created by the "
              "current subtask;\n"
              "AcrShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
              "CurrentEnvIndex=%d)\n",
              *AcrShadowGroupRefPtr, AcrBGHandlePtr->CrtEnvInd, i);

  BG = (s_BOUNDGROUP *)AcrBGHandlePtr->pP;

  /* */    /*E0118*/

  OldShadowGroupRef = ( RTL_CALL, crtshg_(&ErrPipeLine) );
  NewShadowGroupRef = ( RTL_CALL, crtshg_(&ErrPipeLine) );

  PLRank      = PL->Rank;

  /* -------------------------------------------------------------- */    /*E0119*/

  if(AcrType != 0)
  {  /* */    /*E0120*/

     if(PL->AcrShadowGroup2Ref != 0)
        epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                 "*** RTS err 060.170: wrong call doacr_\n"
                 "(across scheme <sendsa_ - recvla_> has "
                 "already been initialized)\n");

     PL->AcrShadowGroup2Ref = *AcrShadowGroupRefPtr;
  }
  else
  {  /* */    /*E0121*/

     if(PL->AcrShadowGroup1Ref != 0)
        epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                 "*** RTS err 060.190: wrong call doacr_\n"
                 "(across scheme <sendsh_ - recvsh_> has "
                 "already been initialized)\n");

     PL->AcrShadowGroup1Ref = *AcrShadowGroupRefPtr;
  }

  ArrayCount = BG->ArrayColl.Count; /* */    /*E0122*/
  for(i=0; i < ArrayCount; i++)     /* */    /*E0123*/
  {  ColPtr = &BG->ArrayColl;
     DArr = coll_At(s_DISARRAY *, ColPtr, i);

     /* */    /*E0124*/

     DARank = DArr->Space.Rank;

     for(j=0; j < DARank; j++)
        PLAxis[j] = 0;      /* */    /*E0125*/

     for(j=0; j < PLRank; j++)
     {  k = PL->DArrAxis[j] - 1;

        if(k < 0)
           continue;        /* */    /*E0126*/
        PLAxis[k] = j + 1;  /* */    /*E0127*/
     }

     BGCount = DArr->BG.Count;   /* */    /*E0128*/
     for(k=0; k < BGCount; k++)  /* */    /*E0129*/
     {  ColPtr = &DArr->ResShdWidthColl;
        ShdWidth = coll_At(s_SHDWIDTH *, ColPtr, k);

        if(ShdWidth->UseSign)
           continue;           /* */    /*E0130*/

        ShdWidth->UseSign = 1; /* */    /*E0131*/

        ColPtr = &DArr->BG;
        CurrBG = coll_At(s_BOUNDGROUP *, ColPtr, k);

        if(CurrBG != BG)
           continue;  /* */    /*E0132*/

        BytePtr = ShdWidth->ShdSign;
        IntPtr1 = ShdWidth->ResLowShdWidth;
        IntPtr2 = ShdWidth->ResHighShdWidth;

        /* */    /*E0133*/

        for(j=0, ErrPipeLine=0, NewMaxShdCount = 0, OldMaxShdCount = 0;
            j < DARank; j++)
        {  ShdSign[j]            = 0x1;

           NewInitDimIndex[j]    = 0;
           NewLastDimIndex[j]    = 0;
           NewInitLowShdIndex[j] = 0;
           NewLastLowShdIndex[j] = 0;
           NewInitHiShdIndex[j]  = 0;
           NewLastHiShdIndex[j]  = 0;
           NewShdSignArray[j]    = 0;

           OldInitDimIndex[j]    = 0;
           OldLastDimIndex[j]    = 0;
           OldInitLowShdIndex[j] = 0;
           OldLastLowShdIndex[j] = 0;
           OldInitHiShdIndex[j]  = 0;
           OldLastHiShdIndex[j]  = 0;
           OldShdSignArray[j]    = 0;
        }

        while(CondTrue)
        {  for(j=0; j < DARank; j++)
               if(ShdSign[j] != 0x4)
                  break;

           if(j == DARank)
              break;  /* */    /*E0134*/

           /* */    /*E0135*/

           if(ShdSign[j] == 0x1)
              ErrPipeLine++;   /* */    /*E0136*/
           ErrPipeLine -= j;

           ShdSign[j] <<= 1;

           for(j--; j >= 0; j--)
               ShdSign[j] = 0x1;

           if(ErrPipeLine > ShdWidth->MaxShdCount)
              continue;  /* */    /*E0137*/

           /* */    /*E0138*/

           for(j=0; j < DARank; j++)
           {  if((BytePtr[j] & ShdSign[j]) == 0x0)
                 break; /* */    /*E0139*/

              if(ShdSign[j] == 0x2 && IntPtr1[j] < 1)
                 break; /* */    /*E0140*/
              if(ShdSign[j] == 0x4 && IntPtr2[j] < 1)
                 break; /* */    /*E0141*/
           }

           if(j != DARank)
              continue; /* */    /*E0142*/

           /* */    /*E0143*/

           for(j=0; j < PLRank; j++)
           {  m = PL->DArrAxis[j] - 1;

              if(m < 0)
                 continue; /* */    /*E0144*/
              if(ShdSign[m] != 0x1)
                 break;    /* */    /*E0145*/
           }

           /* */    /*E0146*/

           if( ( AcrType == 0 &&
                 ( (PL->Invers[j] == 0 && ShdSign[m] == 0x2) ||
                   (PL->Invers[j] != 0 && ShdSign[m] == 0x4) ) ) ||
               ( AcrType != 0 &&
                 ( (PL->Invers[j] == 0 && ShdSign[m] == 0x4) ||
                   (PL->Invers[j] != 0 && ShdSign[m] == 0x2) ) ) )
           {  /* */    /*E0147*/

              /* */    /*E0148*/

              for(j=0; j < DARank; j++)
                  if(PLAxis[j] == 0 && ShdSign[j] != 0x1)
                     break;
              if(j != DARank)
                 continue; /* */    /*E0149*/

              for(j=0; j < DARank; j++)
              {  NewInitDimIndex[j] = ShdWidth->InitDimIndex[j];
                 NewLastDimIndex[j] = NewInitDimIndex[j] +
                                      ShdWidth->DimWidth[j] - 1;
                 if(ShdSign[j] == 0x2)
                 {  NewInitLowShdIndex[j] = ShdWidth->InitLowShdIndex[j];
                    NewLastLowShdIndex[j] = NewInitLowShdIndex[j] +
                                            IntPtr1[j] - 1;
                 }

                 if(ShdSign[j] == 0x4)
                 {  NewInitHiShdIndex[j] = ShdWidth->InitHiShdIndex[j];
                    NewLastHiShdIndex[j] = NewInitHiShdIndex[j] +
                                           IntPtr2[j] - 1;
                 }

                 NewShdSignArray[j] |= ShdSign[j];
                 NewMaxShdCount = ShdWidth->MaxShdCount;
              }
           }
           else
           {  /* */    /*E0150*/

              for(j=0; j < DARank; j++)
              {  OldInitDimIndex[j] = ShdWidth->InitDimIndex[j];
                 OldLastDimIndex[j] = OldInitDimIndex[j] +
                                      ShdWidth->DimWidth[j] - 1;
                 if(ShdSign[j] == 0x2)
                 {  OldInitLowShdIndex[j] = ShdWidth->InitLowShdIndex[j];
                    OldLastLowShdIndex[j] = OldInitLowShdIndex[j] +
                                            IntPtr1[j] - 1;
                 }

                 if(ShdSign[j] == 0x4)
                 {  OldInitHiShdIndex[j] = ShdWidth->InitHiShdIndex[j];
                    OldLastHiShdIndex[j] = OldInitHiShdIndex[j] +
                                           IntPtr2[j] - 1;
                 }

                 OldShdSignArray[j] |= ShdSign[j];
                 OldMaxShdCount = ShdWidth->MaxShdCount;
              }
           }
        }

        if(NewMaxShdCount != 0)    /* */    /*E0151*/
           ( RTL_CALL, incshd_(&NewShadowGroupRef,
                               (DvmType *)DArr->HandlePtr->HeaderPtr,
                               NewInitDimIndex, NewLastDimIndex,
                               NewInitLowShdIndex, NewLastLowShdIndex,
                               NewInitHiShdIndex, NewLastHiShdIndex,
                               &NewMaxShdCount, NewShdSignArray) );

        if(OldMaxShdCount != 0)    /* */    /*E0152*/
           ( RTL_CALL, incshd_(&OldShadowGroupRef,
                               (DvmType *)DArr->HandlePtr->HeaderPtr,
                               OldInitDimIndex, OldLastDimIndex,
                               OldInitLowShdIndex, OldLastLowShdIndex,
                               OldInitHiShdIndex, OldLastHiShdIndex,
                               &OldMaxShdCount, OldShdSignArray) );

        continue;  /* */    /*E0153*/
     }
  }

  /* */    /*E0154*/

  for(i=0; i < ArrayCount; i++)
  {  ColPtr = &BG->ArrayColl;
     DArr = coll_At(s_DISARRAY *, ColPtr, i);

     BGCount = DArr->BG.Count;

     for(k=0; k < BGCount; k++)
     {  ColPtr = &DArr->ResShdWidthColl;
        ShdWidth = coll_At(s_SHDWIDTH *, ColPtr, k);
        ShdWidth->UseSign = 0;
     }
  }

  ErrPipeLine = ( RTL_CALL, across_(AcrossTypePtr, &OldShadowGroupRef,
                                    &NewShadowGroupRef,
                                    PipeLineParPtr) );
  if(RTL_TRACE)
     dvm_trace(ret_doacr_,"ErrPipeLine=%ld\n", ErrPipeLine);

  DVMFTimeFinish(ret_doacr_);
  return  (DVM_RET, ErrPipeLine);
}



DvmType  __callstd  across_(DvmType            *AcrossTypePtr,
                         ShadowGroupRef  *OldShadowGroupRefPtr,
                         ShadowGroupRef  *NewShadowGroupRefPtr,
                         double          *PipeLineParPtr        )
{ s_PARLOOP        *PL;
  byte              AcrType, CondTrue = 1;
  ShadowGroupRef    OldShadowGroupRef = 0;
  int               i, j, k, CondPipeLine = 0, CurrEnvInd, AxisCount,
                    OutDir, Rank, PipeLineAMVAxis, ErrPipeLine = 0;
  DvmType              tlong, coil;
  double            DStep, DSize, tdouble, PipeLinePar, Tc;
  s_REGULARSET     *Local, *Global;
  s_BOUNDGROUP     *OldBG, *NewBG;
  SysHandle        *NewBGHandlePtr;
  void             *CurrAM;
  s_ALIGN          *Align;
  s_DISARRAY       *TempDArr;
  int               VMAxis[MAXARRAYDIM];
  int               PipeLineAMVAxisArray[2*MAXARRAYDIM],
                    OutDirArray[2*MAXARRAYDIM];
  s_VMS            *VMS;
  s_SPACE          *VMSpace;

  DVMFTimeStart(call_across_);

  AcrType = (byte)*AcrossTypePtr;
  PipeLinePar = *PipeLineParPtr;

  if(PipeLinePar <= 0.)
     PipeLinePar *= MeanProcPower; /* conversion of
                                      iteration execution time
                                      to ordinary one */    /*E0155*/

  if(RTL_TRACE)
  {  if(OldShadowGroupRefPtr == NULL || *OldShadowGroupRefPtr == 0)
        dvm_trace(call_across_,
                  "AcrossType=%d; "
                  "OldShadowGroupRef=0; "
                  "NewShadowGroupRefPtr=%lx; NewShadowGroupRef=%lx; "
                  "PipeLinePar=%lf;\n", (int)AcrType,
                  (uLLng)NewShadowGroupRefPtr, *NewShadowGroupRefPtr,
                  PipeLinePar);
     else
     {  if (NewShadowGroupRefPtr == NULL || *NewShadowGroupRefPtr == 0)
           dvm_trace(call_across_,
                     "AcrossType=%d; "
                     "OldShadowGroupRefPtr=%lx; OldShadowGroupRef=%lx; "
                     "NewShadowGroupRef=0; "
                     "PipeLinePar=%lf;\n", (int)AcrType,
                     (uLLng)OldShadowGroupRefPtr, *OldShadowGroupRefPtr,
                     PipeLinePar);
        else
           dvm_trace(call_across_,
                     "AcrossType=%d; "
                     "OldShadowGroupRefPtr=%lx; OldShadowGroupRef=%lx; "
                     "NewShadowGroupRefPtr=%lx; NewShadowGroupRef=%lx; "
                     "PipeLinePar=%lf;\n", (int)AcrType,
                     (uLLng)OldShadowGroupRefPtr, *OldShadowGroupRefPtr,
                     (uLLng)NewShadowGroupRefPtr, *NewShadowGroupRefPtr,
                     PipeLinePar);
     }
  }

  if(OldShadowGroupRefPtr != NULL && *OldShadowGroupRefPtr != 0)
  {  OldShadowGroupRef = *OldShadowGroupRefPtr;
     OldBG = (s_BOUNDGROUP *)((SysHandle *)OldShadowGroupRef)->pP;

     if(OldBG->ArrayColl.Count == 0)
        OldShadowGroupRef = 0;
  }

  stat_event_flag = 1; /* flag: edge exchange functions are executed
                          from the user (for statistics) */    /*E0162*/

  if(OldShadowGroupRef)
  {  /*  Ahead edge exchange is needed to provide
        non renewed edge elements for calculations */    /*E0163*/

     SetHostOper(StartShdGrp)
     ( RTL_CALL, strtsh_(OldShadowGroupRefPtr) );
     SetHostOper(WaitShdGrp)
     ( RTL_CALL, waitsh_(OldShadowGroupRefPtr) );
     SetHostOper(ShdGrp)
  }

  if (NewShadowGroupRefPtr == NULL || *NewShadowGroupRefPtr == 0)
  {  stat_event_flag = 0; /* flag of edge exchange function execution
                             from the user is off */    /*E0391*/

     if(RTL_TRACE)
        dvm_trace(ret_across_," \n");

     DVMFTimeFinish(ret_across_);
     return  (DVM_RET, (DvmType)1);
  }

  /* Check the current parallel loop */    /*E0156*/

  PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

  if(PL == NULL)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.100: wrong call across_\n"
              "(the current context is not a parallel loop)\n");

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 060.101: wrong call across_\n"
              "(the current parallel loop has not been mapped)\n");

  if(PL->IterFlag == ITER_BOUNDS_FIRST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.110: wrong call across_\n"
         "(type of the current parallel loop is ITER_BOUNDS_FIRST)\n");

  if(PL->IterFlag == ITER_BOUNDS_LAST)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
         "*** RTS err 060.111: wrong call across_\n"
         "(type of the current parallel loop is ITER_BOUNDS_LAST)\n");

  PL->PipeLineParPtr = PipeLineParPtr; /* save parameter address
                                          for possible writing
                                          measured iteration execution time */    /*E0157*/

  /* Check edge group, defined by *NewShadowGroupRefPtr reference */    /*E0158*/

  if(TstObject)
  {  if(!TstDVMObj(NewShadowGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.115: wrong call across_\n"
                 "(the new shadow group is not a DVM object; "
                 "NewShadowGroupRef=%lx)\n", *NewShadowGroupRefPtr);
  }

  NewBGHandlePtr = (SysHandle *)*NewShadowGroupRefPtr;
  NewBG          = (s_BOUNDGROUP *)NewBGHandlePtr->pP;

  if(NewBGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.116: wrong call across_\n"
              "(the object is not a shadow group; "
              "NewShadowGroupRef=%lx)\n", *NewShadowGroupRefPtr);

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0159*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM*/    /*E0160*/

  if(NewBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.117: wrong call across_\n"
              "(the new shadow group was not created by the "
              "current subtask;\n"
              "NewShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
              "CurrentEnvIndex=%d)\n",
              *NewShadowGroupRefPtr, NewBGHandlePtr->CrtEnvInd,
              CurrEnvInd);

  if(NewBG->ArrayColl.Count == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.118: wrong call across_\n"
              "(the new shadow group is empty; "
              "NewShadowGroupRef=%lx)\n", *NewShadowGroupRefPtr);

  VMS         = PL->AMView->VMS;
  VMSpace     = &VMS->Space;
  Rank        = PL->Rank;
  Align       = PL->Align;
  PL->QDim[0] = -1;

  /* -------------------------------------------------------------- */    /*E0161*/

  if(AcrType != 0)
  {  if(PL->AcrType2)
        epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                 "*** RTS err 060.120: wrong call across_\n"
                 "(across scheme <sendsa_ - recvla_> has "
                 "already been initialized)\n");

     PL->AcrType2 = 1; /* scheme ACROSS <sendsa_ - recvla_>
                          will be executed */    /*E0164*/

     if(OldShadowGroupRefPtr != NULL)
        PL->OldShadowGroup2Ref = *OldShadowGroupRefPtr;
     else
        PL->OldShadowGroup2Ref = 0;

     PL->NewShadowGroup2Ref = *NewShadowGroupRefPtr;

     /* Check if  <sendsa_ - recvla_> scheme can be pipelined */    /*E0165*/

     while(CondTrue)
     {
        if(VMS->ProcCount == 1)
        {  ErrPipeLine = 9;
           break; /* */    /*E0166*/
        }

        if(PL->Empty)
        {  ErrPipeLine = 10;
           break; /* empty loop is executed without pipelining */    /*E0167*/
        }

        if(PL->AcrType1 && PL->PipeLineSign == 0)
        {  ErrPipeLine = 20;
           break; /* <sendsh_ - recvsh_> scheme has already been
                     initialized without pipelining */    /*E0168*/
        }

        if(AcrossGroupNumber == -1)
        {  ErrPipeLine = 30;
           break; /* <sendsa_ - recvla_> scheme is initialized
                     with blocked pipeline */    /*E0169*/
        }

        /* */    /*E0170*/

        for(i=0; i < Rank; i++)
        {  VMAxis[i] = 0;

           if(Align[i].Attr != align_NORMAL)
              continue;  /* */    /*E0171*/

           VMAxis[i] = GetPLMapDim(PL, i+1); /* */    /*E0172*/
           if(VMAxis[i] == 0)
              continue;  /* */    /*E0173*/

           if( ( PL->Set[i].Size / PL->Set[i].Step ) <
               VMSpace->Size[VMAxis[i]-1] )
           {  ErrPipeLine = 40;
              break; /* */    /*E0174*/
           }
        }

        if(i != Rank)
           break; /* */    /*E0175*/

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(i=0; i < Rank; i++)
               tprintf("VMAxis[%d] = %d\n", i, VMAxis[i]);

        /* ------------------------------------------------ */    /*E0176*/

        if(AcrossGroupNumber == 0 && PipeLinePar <= 0.)
        {  /* Time of loop iteration execution is defined in both parameters */    /*E0177*/

           Tc = dvm_max(CoilTime, -PipeLinePar);

           if(PL->Tc != 0. && Tc != 0. && PL->Tc != Tc)
           {  ErrPipeLine = 50;
              break; /* time of loop iteration execution and
                        time defined for <sendsh_ - recvsh_> scheme
                        are not the same */    /*E0178*/
           }

           if(Tc == 0.)
           {  /* Time of iteration execution if not equal to zero */    /*E0179*/

              for(i=0; i < TcCount; i++)
              {  if(DVM_LINE[0] != TcLine[i])
                    continue;
                 SYSTEM_RET(CurrEnvInd, strcmp, (DVM_FILE[0], TcFile[i]))
                 if(CurrEnvInd == 0)
                    break;
              }

              if(i == TcCount)
              {  /* Time of loop iteration execution
                           has not been found        */    /*E0180*/

                 if(TcCount == (TcNumber-1) ||
                    (TcPL != NULL && TcPL != PL))
                 {  ErrPipeLine = 60;
                    break; /* no  space to save
                              time of loop iteration execution */    /*E0181*/
                 }
                 else
                 {  TcLine[TcCount] = DVM_LINE[0];
                    TcFile[TcCount] = DVM_FILE[0];
                    TcTime[TcCount] = 0.000000001;
                    TcCount++;
                    PL->SetTcSign = 1; /* flag: measuring time of
                                          loop iteration execution */    /*E0182*/
                    TcPL = PL; /* pointer to loop descriptor
                                  which iteration execution time
                                  is to be measured */    /*E0183*/
                 }
              }
              else
              {  if(TcTime[i] < 0.)
                 {  ErrPipeLine = 70;
                    break; /* */    /*E0184*/
                 }

                 PL->Tc = TcTime[i];
              }
           }
           else
           {  /* Iteration execution time is not equal to zero */    /*E0185*/

              PL->Tc = Tc;
           }
        }
        else
        {  /* Number of groups of quanted loop dimension partitioning
                       is defined in one parameter at least           */    /*E0186*/

           if(AcrossGroupNumber > 0 && PipeLinePar > 0.)
           {  PipeLinePar = (double)
                            dvm_max(AcrossGroupNumber, PipeLinePar);
           }
           else
           {  if(AcrossGroupNumber > 0)
                 PipeLinePar = (double)AcrossGroupNumber;
           }

           if(PipeLinePar < 2.)
           {  ErrPipeLine = 80;
              break; /* number of groups of quanted dimension
                        partitioning < 2 */    /*E0187*/
           }

           if(PL->AcrossGroup && PL->AcrossGroup != (int)PipeLinePar)
           {  ErrPipeLine = 90;
              break; /* number of groups of quanted dimension
                        partitioning and nubmer of groups
                        for <sendsh_ - recvsh_> scheme
                        are not the same */    /*E0188*/
           }

           PL->AcrossGroup = (int)PipeLinePar;
        }

        if(PLTimeMeasure || PL->AMView->TimeMeasure)
        {  ErrPipeLine = 110;
           break; /* loop iterations are divided in groups
                     for time execution measuring */    /*E0189*/
        }

        /* Check if edge group defined by OldShadowGroupRef
                    consists of shadow edges only           */    /*E0190*/

        if(OldShadowGroupRef)
        {  if(CheckShdEdge(OldBG) == 0)
           {  ErrPipeLine = 120;
              break; /* the group consists not only of shadow edges */    /*E0191*/
           }
        }

        /* */    /*E0192*/

        if(CheckShdEdge(NewBG) == 0)
        {  ErrPipeLine = 130;
           break; /* */    /*E0193*/
        }

        /* */    /*E0194*/

        if(CheckBGAMView(NewBG, PL->AMView) == 0)
        {  ErrPipeLine = 140;
           break; /* */    /*E0195*/
        }

        /* */    /*E0196*/

        AxisCount = GetBGAxis(NewBG, PipeLineAMVAxisArray, OutDirArray);

        if(AxisCount <= 0)
        {  ErrPipeLine = 150;
           break; /* */    /*E0197*/
        }

        for(j=0,k=0; j < AxisCount; j++)
        {  PipeLineAMVAxis = PipeLineAMVAxisArray[j];
           OutDir = OutDirArray[j];

           /*     Choose pipelined current parallel loop dimension
              (PipeLineAMVAxis - number of AM representation dimension
                       the loop dimension is to be mapped on)          */    /*E0198*/

           for(i=0; i < Rank; i++)
           {  if(Align[i].Attr != align_NORMAL)
                 continue;
              if(Align[i].TAxis != PipeLineAMVAxis)
                 continue;
              if(PL->Invers[i] && OutDir > 0)
                 continue; /* loop step is negative,
                              but direction of <sendsa_ - recvla_>
                              ACROSS scheme shadow edge is positive */    /*E0199*/
              if(PL->Invers[i] == 0 && OutDir < 0)
                 continue; /* loop step is positive
                              but direction of <sendsa_ - recvla_>
                              ACROSS scheme shadow edge is negative */    /*E0200*/
              break;
           }

           if(i == Rank)
              continue; /* loop dimension for pipelining
                           has not been found */    /*E0201*/

           if(VMAxis[i] == 0)
              continue; /* chosen pipelining dimension
                           is not block distributed
                           on the processor system dimension */    /*E0202*/

           if(VMSpace->Size[VMAxis[i]-1] == 1)
              continue; /* */    /*E0203*/

           k = 1;  /* */    /*E0204*/

           PL->PipeLineAxis2[i] = 1;
        }

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(i=0; i < Rank; i++)
               tprintf("PipeLineAxis2[%d]=%d\n",
                       i, (int)PL->PipeLineAxis1[i]);

        if(k == 0)
        {  ErrPipeLine = 160;
           break; /* */    /*E0205*/
        }

        /* Search dimension for pipelining */    /*E0206*/

        /*
        if(PL->HasLocal == 0)
        {  if(PL->SetTcSign == 0)
              epprintf(MultiProcErrReg2, __FILE__, __LINE__,
                       "*** RTS err 060.121: wrong call across_\n"
                       "(the current parallel loop has not been "
                       "mapped onto the current processor)\n");
           ErrPipeLine = 32;
           break;
        }
        */

        if(PL->TempDArr == NULL)
        {  ErrPipeLine = 170;
           break; /* loop is mapped on non distributed array
                     but on AM representation */    /*E0208*/
        }

        Local = PL->Local;
        Global = PL->Set;

        for(j=0,i=-1,tlong=-1; j < Rank; j++)
        {  if(VMAxis[j] != 0)
              continue;  /* */    /*E0209*/

           if(PL->PipeLineAxis2[j] || PL->PipeLineAxis1[j])
              continue;  /* */    /*E0210*/

           /* */    /*E0211*/

           DStep = (double)Global[j].Step;
           DSize = (double)Global[j].Size;

           coil = (DvmType)ceil(DSize / DStep);

           if(coil < 2)
              continue;  /* */    /*E0212*/
           if(PL->DArrAxis[j] == 0)
              continue; /* */    /*E0213*/

           if(AcrossQuantumReg != 0)
           {  /* */    /*E0214*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0215*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("ReplPLQDim=%d  Size=%ld\n", j, coil);
              }
           }
           else
           {  /* */    /*E0216*/

              if(coil > tlong)
              {  tlong = coil;
                 i = j;
              }
           }
        }

        if(AcrossQuantumReg == 0)
        {  /* */    /*E0217*/

           PL->QDim[0] = i; /* */    /*E0218*/

           if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
              tprintf("ReplPLQDim=%d  Size=%ld\n", i, tlong);
        }

        for(j=0,i=-1,tlong=-1; j < Rank; j++)
        {  if(VMAxis[j] == 0)
              continue;  /* */    /*E0219*/

           if(VMSpace->Size[VMAxis[j]-1] > 1)
              continue; /* */    /*E0203*/

           if(PL->PipeLineAxis2[j] || PL->PipeLineAxis1[j])
              continue;  /* */    /*E0220*/

           /* */    /*E0221*/

           DStep = (double)Global[j].Step;
           DSize = (double)Global[j].Size;

           coil = (DvmType)ceil(DSize / DStep);

           if(coil < 2)
              continue;  /* */    /*E0222*/
           if(PL->DArrAxis[j] == 0)
              continue; /* */    /*E0223*/

           if(AcrossQuantumReg != 0)
           {  /* */    /*E0224*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0225*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("DistrPLQDim=%d  Size=%ld\n", j, coil);
              }
           }
           else
           {  /* */    /*E0226*/

              if(coil > tlong)
              {  tlong = coil;
                 i = j;
              }
           }
        }

        if(AcrossQuantumReg == 0)
        {  /* */    /*E0227*/

           if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
              tprintf("DistrPLQDim=%d  Size=%ld\n", i, tlong);

           j = dvm_min(PL->QDim[0], i);
           k = dvm_max(PL->QDim[0], i);

           if(k == -1)
           {  /* */    /*E0228*/

              for(j=0,i=-1,tlong=-1; j < Rank; j++)
              {  if(PL->PipeLineAxis1[j] == 0 &&
                    PL->PipeLineAxis2[j] == 0)
                    continue;  /* */    /*E0229*/

                 if(VMAxis[j] == 0)
                    continue;  /* */    /*E0230*/

                 if(VMSpace->Size[VMAxis[j]-1] > 1)
                    continue;  /* */    /*E0231*/

                 /* */    /*E0232*/

                 DStep = (double)Global[j].Step;
                 DSize = (double)Global[j].Size;

                 coil = (DvmType)ceil(DSize / DStep);

                 if(coil < 2)
                    continue;  /* */    /*E0233*/
                 if(coil > tlong)
                 {  tlong = coil;
                    i = j;
                 }
              }

              if(i == -1)
              {  ErrPipeLine = 180;
                 break; /* */    /*E0234*/
              }
              else
              {  j = i;
                 k = i;
                 PL->QDim[0] = i;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("AcrossPLQDim=%d  Size=%ld\n", i, tlong);
              }
           }

           if(j >= 0)
           {  /* */    /*E0235*/

              DStep = (double)Global[PL->QDim[0]].Step;
              DSize = (double)Global[PL->QDim[0]].Size;
              coil = (DvmType)ceil(DSize / DStep);

              DStep = (double)Global[i].Step;
              DSize = (double)Global[i].Size;
              tlong = (DvmType)ceil(DSize / DStep);

              if(coil == tlong)
              {  /* */    /*E0236*/

                 PL->QDim[0] = j;
                 i = j;
              }
              else
              {  /* */    /*E0237*/

                 if(coil > tlong)
                    i = PL->QDim[0];
                 else
                    PL->QDim[0] = i;
              }
           }
           else
           {  /* */    /*E0238*/

              PL->QDim[0] = k;
              i = k;
           }

           PL->QDimNumber = 1; /* */    /*E0239*/
        }
        else
        {  /* */    /*E0240*/

           for(j=0; j < Rank; j++)
           {  if(PL->PipeLineAxis1[j] == 0 && PL->PipeLineAxis2[j] == 0)
                 continue;  /* */    /*E0241*/

              if(VMAxis[j] == 0)
                 continue;  /* */    /*E0242*/

              if(VMSpace->Size[VMAxis[j]-1] > 1)
                 continue;  /* */    /*E0243*/

              /* */    /*E0244*/

              DStep = (double)Global[j].Step;
              DSize = (double)Global[j].Size;

              coil = (DvmType)ceil(DSize / DStep);

              if(coil < 2)
                 continue;  /* */    /*E0245*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0246*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("AcrossPLQDim=%d  Size=%ld\n", j, coil);
              }
           }

           if(PL->QDimNumber == 0)
           {  ErrPipeLine = 180;
              break; /* */    /*E0247*/
           }
        }

        tlong = 1;

        for(j=0; j < PL->QDimNumber; j++)
        {  DStep = (double)Global[PL->QDim[j]].Step;
           DSize = (double)Global[PL->QDim[j]].Size;
           tlong *= (DvmType)ceil(DSize / DStep);
        }

        for(j=0; j < PL->QDimNumber; j++)
            PL->DDim[j] =
            PL->DArrAxis[PL->QDim[j]] - 1; /* */    /*E0248*/
        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(j=0; j < PL->QDimNumber; j++)
               tprintf("QDim=%d DDim=%d\n", PL->QDim[j], PL->DDim[j]);

        if(PL->SetTcSign)
        {  ErrPipeLine = 32;  /* the number is fixed */    /*E0249*/
           break; /* it is necessary to measure
                     one  iteration execution time */    /*E0250*/
        }

        /* Extract initial loop index value for its quanted dimension
                       from the distributed array header              */    /*E0251*/

        TempDArr = PL->TempDArr;

        if(TempDArr->ExtHdrSign)
        {  /* The loop was mapped on the array with extended header */    /*E0252*/

           k = 2*TempDArr->Space.Rank + 1;

           for(j=0; j < PL->QDimNumber; j++)
               PL->LowIndex[j] =
               ((DvmType *)TempDArr->HandlePtr->HeaderPtr)[k - PL->DDim[j]];
        }
        else
        {  for(j=0; j < PL->QDimNumber; j++)
               PL->LowIndex[j] = 0;
        }

        if(PL->Tc == 0. && PL->AcrossGroup != 0)
        {  /* Number of groups of partitioned
               quanted dimension is defined   */    /*E0253*/

           if(AcrossQuantumReg == 0)
           {  /* */    /*E0254*/

              /* Calculate quant value for
                     quanted dimention     */    /*E0255*/

              tdouble = ceil( (double)PL->Set[i].Size /
                              (double)PL->AcrossGroup );
              tdouble = DStep * ceil(tdouble/DStep);

              /* Calculate number of quants for
                   the internal local domain    */    /*E0256*/

              PL->AcrossQNumber = (int)ceil(DSize/tdouble);
           }
           else
           {  /* */    /*E0257*/

              /* */    /*E0258*/

              coil = 1;

              for(j=0; j < PL->QDimNumber; j++)
              {  k = PL->QDim[j];
                 coil *= PL->Set[k].Size;
              }

              tdouble = ceil( (double)coil / (double)PL->AcrossGroup );

              /* */    /*E0259*/

              coil = 1;

              for(j=0; j < PL->QDimNumber; j++)
              {  k = PL->QDim[j];
                 coil *= Global[k].Size;
              }

              PL->AcrossQNumber = (int)ceil( (double)coil / tdouble );
           }
        }
        else
        {  /* One iteration execution time is defined */    /*E0260*/
           /* Calculate PL->AcrossQNumber */    /*E0261*/

           CrtShdGrpBuffers(NewBGHandlePtr); /* create edge buffers
                                                for the edge group */    /*E0262*/
           GetAcrossQNumber(PL, tlong, PL->PipeLineAxis2, VMAxis, NewBG);
        }

        /*   Number of portions the internal
           part of the loop is to be divided in */    /*E0263*/

        coil = dvm_min(PL->AcrossQNumber, tlong);

        if(coil < 2)
        {  ErrPipeLine = 190;
           break; /* */    /*E0264*/
        }

        for(j=0; j < PL->QDimNumber; j++)
        {  if(coil < 2)
              break;  /* */    /*E0265*/

           k = PL->QDim[j];
           DStep = (double)Global[k].Step;
           DSize = (double)Global[k].Size;

           tlong = (DvmType)ceil(DSize / DStep); /* */    /*E0266*/

           PL->StdPLQCount[j] = dvm_min(tlong, coil); /* */    /*E0267*/

           /* Number of points in standard dimention portion */    /*E0268*/

           PL->StdPLQSize[j] = tlong / PL->StdPLQCount[j];

           /* */    /*E0269*/

           coil = (DvmType)(ceil((double)coil /
                               (double)PL->StdPLQCount[j] ));
        }

        PL->QDimNumber = j; /* */    /*E0270*/

        /* Output information in the stream of information messages */    /*E0271*/

        if(AcrossInfoPrint && _SysInfoPrint &&
           MPS_CurrentProc == DVM_IOProc)
        {  if(PL->Tc == 0. && PL->AcrossGroup != 0)
           {  /* Number of quanted dimension
                 partition group is defined  */    /*E0272*/

              rtl_iprintf("\n*** RTS: FILE=%s LINE=%d\n",
                          DVM_FILE[0], DVM_LINE[0]);
           }
           else
           {  /* One iteration execution time is defined */    /*E0273*/

              rtl_iprintf("\n*** RTS: FILE=%s LINE=%d "
                          "MeanProcTime=%le CoilTime=%le\n",
                          DVM_FILE[0], DVM_LINE[0],
                          MeanProcPower, PL->Tc/MeanProcPower);
           }

           for(j=0; j < PL->QDimNumber; j++)
           {  k = PL->QDim[j];
              rtl_iprintf("AcrossGroupNumber[%d]=%ld  "
                          "AcrossGroupSize[%d]=%ld\n",
                          k, PL->StdPLQCount[j], k, PL->StdPLQSize[j]);
           }
        }

        /* ------------------------------------------------- */    /*E0274*/

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(j=0; j < PL->QDimNumber; j++)
           {  k = PL->QDim[j];
              tprintf("StdPLQCount[%d]=%ld  StdPLQSize[%d]=%ld\n",
                      k, PL->StdPLQCount[j], k, PL->StdPLQSize[j]);
           }

        CondPipeLine = 1;
        break;
     }

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
        tprintf("CondPipeLine=%d  ErrPipeLine=%d\n",
                 CondPipeLine, ErrPipeLine);

     if(CondPipeLine)
     {  PL->PipeLineSign   = 1; /* conditions for <sendsa_ - recvla_>
                                   scheme pipelining are satisfied */    /*E0275*/
     }
     else
     {  /* Conditions for <sendsa_ - recvla_> scheme
                 pipelining are not satisfied        */    /*E0276*/

        SetHostOper(StartShdGrp)

        if(PL->PipeLineSign)
           ( RTL_CALL, recvsh_(&PL->NewShadowGroup1Ref) );

        PL->PipeLineSign   = 0;

        if(ErrPipeLine != 32)
        {  if(PL->SetTcSign)
           {  PL->SetTcSign = 0;
              PL->Tc = 0.;
              TcPL = NULL;
              TcCount--;
           }
        }

        ( RTL_CALL, recvla_(NewShadowGroupRefPtr) );
        SetHostOper(ShdGrp)
     }
  }
  else
  {  if(PL->AcrType1)
        epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                 "*** RTS err 060.140: wrong call across_\n"
                 "(across scheme <sendsh_ - recvsh_> has "
                 "already been initialized)\n");

     PL->AcrType1 = 1; /* scheme ACROSS <sendsh_ - recvsh_>
                          will be executed */    /*E0277*/

     if(OldShadowGroupRefPtr != NULL)
        PL->OldShadowGroup1Ref = *OldShadowGroupRefPtr;
     else
        PL->OldShadowGroup1Ref = 0;

     PL->NewShadowGroup1Ref = *NewShadowGroupRefPtr;

     /* Check if it is possible to use pipeline
            for <sendsh_ - recvsh_> scheme      */    /*E0278*/

     while(CondTrue)
     {
        if(VMS->ProcCount == 1)
        {  ErrPipeLine = 9;
           break; /* */    /*E0279*/
        }

        if(PL->Empty)
        {  ErrPipeLine = 10;
           break; /* empty loop is executed without pipelining */    /*E0280*/
        }

        if(PL->AcrType2 && PL->PipeLineSign == 0)
        {  ErrPipeLine = 20;
           break; /* <sendsa_ - recvla_> scheme has already been
                     initialized without pipelining */    /*E0281*/
        }

        if(AcrossGroupNumber == -1)
        {  ErrPipeLine = 30;
           break; /* <sendsh_ - recvsh_> scheme is
                     initialized with blocked pipelining */    /*E0282*/
        }

        /* */    /*E0283*/

        for(i=0; i < Rank; i++)
        {  VMAxis[i] = 0;

           if(Align[i].Attr != align_NORMAL)
              continue;  /* */    /*E0284*/

           VMAxis[i] = GetPLMapDim(PL, i+1); /* */    /*E0285*/
           if(VMAxis[i] == 0)
              continue;  /* */    /*E0286*/

           if( ( PL->Set[i].Size / PL->Set[i].Step ) <
               VMSpace->Size[VMAxis[i]-1] )
           {  ErrPipeLine = 40;
              break; /* */    /*E0287*/
           }
        }

        if(i != Rank)
           break; /* */    /*E0288*/

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(i=0; i < Rank; i++)
               tprintf("VMAxis[%d] = %d\n", i, VMAxis[i]);

        /* ------------------------------------------------ */    /*E0289*/

        if(AcrossGroupNumber == 0 && PipeLinePar <= 0.)
        {  /* Time of iteration execution is defined in both parameters */    /*E0290*/

           Tc = dvm_max(CoilTime, -PipeLinePar);

           if(PL->Tc != 0. && Tc != 0. && PL->Tc != Tc)
           {  ErrPipeLine = 50;
              break; /* time of iteration execution
                        and time defined for <sendsa_ - recvla_>
                        scheme are not the same */    /*E0291*/
           }

           if(Tc == 0.)
           {  /* Iteration execution time is equal to 0 */    /*E0292*/

              for(i=0; i < TcCount; i++)
              {  if(DVM_LINE[0] != TcLine[i])
                    continue;
                 SYSTEM_RET(CurrEnvInd, strcmp, (DVM_FILE[0], TcFile[i]))
                 if(CurrEnvInd == 0)
                    break;
              }

              if(i == TcCount)
              {  /* Time of loop iteration execution
                           has not been found        */    /*E0293*/

                 if(TcCount == (TcNumber-1) ||
                    (TcPL != NULL && TcPL != PL))
                 {  ErrPipeLine = 60;
                    break; /* there is no space to save
                              loop iteration execution time */    /*E0294*/
                 }
                 else
                 {  TcLine[TcCount] = DVM_LINE[0];
                    TcFile[TcCount] = DVM_FILE[0];
                    TcTime[TcCount] = 0.000000001;
                    TcCount++;
                    PL->SetTcSign = 1; /* flag: measuring loop
                                          iteration execution time */    /*E0295*/
                    TcPL = PL; /* reference to loop descriptor
                                  which iteration execution time
                                  is to be measured */    /*E0296*/
                 }
              }
              else
              {  if(TcTime[i] < 0.)
                 {  ErrPipeLine = 70;
                    break; /* */    /*E0297*/
                 }

                 PL->Tc = TcTime[i];
              }
           }
           else
           {  /* Iteration execution time is not equal to 0 */    /*E0298*/

              PL->Tc = Tc;
           }
        }
        else
        {  /* Number of groups of quanted loop dimension partition
                      is defined in one parameter at least         */    /*E0299*/

           if(AcrossGroupNumber > 0 && PipeLinePar > 0.)
           {  PipeLinePar = (double)
                            dvm_max(AcrossGroupNumber, PipeLinePar);
           }
           else
           {  if(AcrossGroupNumber > 0)
                 PipeLinePar = (double)AcrossGroupNumber;
           }

           if(PipeLinePar < 2.)
           {  ErrPipeLine = 80;
              break; /* number of groups of quanted
                        dimension partition < 2 */    /*E0300*/
           }

           if(PL->AcrossGroup && PL->AcrossGroup != (int)PipeLinePar)
           {  ErrPipeLine = 90;
              break; /* number of groups of quanted dimension partition
                        and nubmer of groups for <sendsa_ - recvla_>
                        scheme are not the same */    /*E0301*/
           }

           PL->AcrossGroup = (int)PipeLinePar;
        }

        if(PLTimeMeasure || PL->AMView->TimeMeasure)
        {  ErrPipeLine = 110;
           break; /* loop iterations are divided in groups
                     for time execution measuring */    /*E0302*/
        }

        /* Check if edge group defined by OldShadowGroupRef
                    consists of shadow edges only           */    /*E0303*/

        if(OldShadowGroupRef)
        {  if(CheckShdEdge(OldBG) == 0)
           {  ErrPipeLine = 120;
              break; /* the group consists not only of shadow edges */    /*E0304*/
           }
        }

        /* */    /*E0305*/

        if(CheckShdEdge(NewBG) == 0)
        {  ErrPipeLine = 130;
           break; /* */    /*E0306*/
        }

        /* */    /*E0307*/

        if(CheckBGAMView(NewBG, PL->AMView) == 0)
        {  ErrPipeLine = 140;
           break; /* */    /*E0308*/
        }

        /* */    /*E0309*/

        AxisCount = GetBGAxis(NewBG, PipeLineAMVAxisArray, OutDirArray);

        if(AxisCount <= 0)
        {  ErrPipeLine = 150;
           break; /* */    /*E0310*/
        }

        for(j=0,k=0; j < AxisCount; j++)
        {  PipeLineAMVAxis = PipeLineAMVAxisArray[j];
           OutDir = OutDirArray[j];

           /*     Choose pipelined current parallel loop dimension
              (PipeLineAMVAxis - number of AM representation dimension
                      the loop dimension is to be mapped on)           */    /*E0311*/

           for(i=0; i < Rank; i++)
           {  if(Align[i].Attr != align_NORMAL)
                 continue;
              if(Align[i].TAxis != PipeLineAMVAxis)
                 continue;
              if(PL->Invers[i] && OutDir < 0)
                 continue; /* loop step is negative, and
                              direction of <sendsh_ - recvsh_> ACROSS
                              scheme shadow edge is negative */    /*E0312*/
              if(PL->Invers[i] == 0 && OutDir > 0)
                 continue; /* loop step is positive and
                              direction of <sendsh_ - recvsh_> ACROSS
                              scheme shadow edge is positive */    /*E0313*/
              break;
           }

           if(i == Rank)
              continue; /* loop dimension for pipelining
                           has not been found */    /*E0314*/

           if(VMAxis[i] == 0)
              continue; /* chosen pipelined dimension
                           is not block distributed
                           on the processor system dimension */    /*E0315*/

           if(VMSpace->Size[VMAxis[i]-1] == 1)
              continue; /* */    /*E0316*/

           k = 1;  /* */    /*E0317*/

           PL->PipeLineAxis1[i] = 1;
        }

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(i=0; i < Rank; i++)
               tprintf("PipeLineAxis1[%d]=%d\n",
                       i, (int)PL->PipeLineAxis1[i]);

        if(k == 0)
        {  ErrPipeLine = 160;
           break; /* */    /*E0318*/
        }

        /* Search dimension for pipelining */    /*E0319*/

/*
        if(PL->HasLocal == 0)
        {  if(PL->SetTcSign == 0)
              epprintf(MultiProcErrReg2, __FILE__, __LINE__,
                       "*** RTS err 060.141: wrong call across_\n"
                       "(the current parallel loop has not been "
                       "mapped onto the current processor)\n");
           ErrPipeLine = 32;
           break;
        }
*/

        if(PL->TempDArr == NULL)
        {  ErrPipeLine = 170;
           break; /* loop is not mapped on distributed array
                     but on AM representation */    /*E0321*/
        }

        Local = PL->Local;
        Global = PL->Set;

        for(j=0,i=-1,tlong=-1; j < Rank; j++)
        {  if(VMAxis[j] != 0)
              continue;  /* */    /*E0322*/

           if(PL->PipeLineAxis2[j] || PL->PipeLineAxis1[j])
              continue;  /* */    /*E0323*/

           /* */    /*E0324*/

           DStep = (double)Global[j].Step;
           DSize = (double)Global[j].Size;

           coil = (DvmType)ceil(DSize / DStep);

           if(coil < 2)
              continue;  /* */    /*E0325*/
           if(PL->DArrAxis[j] == 0)
              continue; /* */    /*E0326*/

           if(AcrossQuantumReg != 0)
           {  /* */    /*E0327*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0328*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("ReplPLQDim=%d  Size=%ld\n", j, coil);
              }
           }
           else
           {  /* */    /*E0329*/

              if(coil > tlong)
              {  tlong = coil;
                 i = j;
              }
           }
        }

        if(AcrossQuantumReg == 0)
        {  /* */    /*E0330*/

           PL->QDim[0] = i; /* */    /*E0331*/

           if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
              tprintf("ReplPLQDim=%d  Size=%ld\n", i, tlong);
        }

        for(j=0,i=-1,tlong=-1; j < Rank; j++)
        {  if(VMAxis[j] == 0)
              continue;  /* */    /*E0332*/

           if(VMSpace->Size[VMAxis[j]-1] > 1)
              continue; /* */    /*E0203*/

           if(PL->PipeLineAxis1[j] || PL->PipeLineAxis2[j])
              continue;  /* */    /*E0333*/

           /* */    /*E0334*/

           DStep = (double)Global[j].Step;
           DSize = (double)Global[j].Size;

           coil = (DvmType)ceil(DSize / DStep);

           if(coil < 2)
              continue;  /* */    /*E0335*/
           if(PL->DArrAxis[j] == 0)
              continue; /* */    /*E0336*/

           if(AcrossQuantumReg != 0)
           {  /* */    /*E0337*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0338*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("DistrPLQDim=%d  Size=%ld\n", j, coil);
              }
           }
           else
           {  /* */    /*E0339*/

              if(coil > tlong)
              {  tlong = coil;
                 i = j;
              }
           }
        }

        if(AcrossQuantumReg == 0)
        {  /* */    /*E0340*/

           if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
              tprintf("DistrPLQDim=%d  Size=%ld\n", i, tlong);

           j = dvm_min(PL->QDim[0], i);
           k = dvm_max(PL->QDim[0], i);

           if(k == -1)
           {  /* */    /*E0341*/

              for(j=0,i=-1,tlong=-1; j < Rank; j++)
              {  if(PL->PipeLineAxis1[j] == 0 &&
                    PL->PipeLineAxis2[j] == 0)
                    continue;  /* */    /*E0342*/

                 if(VMAxis[j] == 0)
                    continue;  /* */    /*E0343*/

                 if(VMSpace->Size[VMAxis[j]-1] > 1)
                    continue;  /* */    /*E0344*/

                 /* */    /*E0345*/

                 DStep = (double)Global[j].Step;
                 DSize = (double)Global[j].Size;

                 coil = (DvmType)ceil(DSize / DStep);

                 if(coil < 2)
                    continue;  /* */    /*E0346*/
                 if(coil > tlong)
                 {  tlong = coil;
                    i = j;
                 }
              }

              if(i == -1)
              {  ErrPipeLine = 180;
                 break; /* */    /*E0347*/
              }
              else
              {  j = i;
                 k = i;
                 PL->QDim[0] = i;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("AcrossPLQDim=%d  Size=%ld\n", i, tlong);
              }
           }

           if(j >= 0)
           {  /* */    /*E0348*/

              DStep = (double)Global[PL->QDim[0]].Step;
              DSize = (double)Global[PL->QDim[0]].Size;
              coil = (DvmType)ceil(DSize / DStep);

              DStep = (double)Global[i].Step;
              DSize = (double)Global[i].Size;
              tlong = (DvmType)ceil(DSize / DStep);

              if(coil == tlong)
              {  /* */    /*E0349*/

                 PL->QDim[0] = j;
                 i = j;
              }
              else
              {  /* */    /*E0350*/

                 if(coil > tlong)
                    i = PL->QDim[0];
                 else
                    PL->QDim[0] = i;
              }
           }
           else
           {  /* */    /*E0351*/

              PL->QDim[0] = k;
              i = k;
           }

           PL->QDimNumber = 1; /* */    /*E0352*/
        }
        else
        {  /* */    /*E0353*/

           for(j=0; j < Rank; j++)
           {  if(PL->PipeLineAxis1[j] == 0 && PL->PipeLineAxis2[j] == 0)
                 continue;  /* */    /*E0354*/

              if(VMAxis[j] == 0)
                 continue;  /* */    /*E0355*/

              if(VMSpace->Size[VMAxis[j]-1] > 1)
                 continue;  /* */    /*E0356*/

              /* */    /*E0357*/

              DStep = (double)Global[j].Step;
              DSize = (double)Global[j].Size;

              coil = (DvmType)ceil(DSize / DStep);

              if(coil < 2)
                 continue;  /* */    /*E0358*/

              for(k=0; k < PL->QDimNumber; k++)
                  if(PL->QDim[k] == j)
                     break;  /* */    /*E0359*/
              if(k == PL->QDimNumber)
              {  PL->QDim[PL->QDimNumber] = j;
                 PL->QDimNumber++;

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_across_))
                    tprintf("AcrossPLQDim=%d  Size=%ld\n", j, coil);
              }
           }

           if(PL->QDimNumber == 0)
           {  ErrPipeLine = 180;
              break; /* */    /*E0360*/
           }
        }

        tlong = 1;

        for(j=0; j < PL->QDimNumber; j++)
        {  DStep = (double)Global[PL->QDim[j]].Step;
           DSize = (double)Global[PL->QDim[j]].Size;
           tlong *= (DvmType)ceil(DSize / DStep);
        }

        for(j=0; j < PL->QDimNumber; j++)
            PL->DDim[j] =
            PL->DArrAxis[PL->QDim[j]] - 1; /* */    /*E0361*/
        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(j=0; j < PL->QDimNumber; j++)
               tprintf("QDim=%d DDim=%d\n", PL->QDim[j], PL->DDim[j]);

        if(PL->SetTcSign)
        {  ErrPipeLine = 32;  /* the number is fixed */    /*E0362*/
           break; /* it is necessary to measure
                     one  iteration execution time */    /*E0363*/
        }

        /* Extract initial loop index value for its quanted
             dimension from the distributed array header    */    /*E0364*/

        TempDArr = PL->TempDArr;

        if(TempDArr->ExtHdrSign)
        {  /* The loop was mapped on the array with extended header */    /*E0365*/

           k = 2*TempDArr->Space.Rank + 1;

           for(j=0; j < PL->QDimNumber; j++)
               PL->LowIndex[j] =
               ((DvmType *)TempDArr->HandlePtr->HeaderPtr)[k - PL->DDim[j]];
        }
        else
        {  for(j=0; j < PL->QDimNumber; j++)
               PL->LowIndex[j] = 0;
        }

        if(PL->Tc == 0. && PL->AcrossGroup != 0)
        {  /* Number of groups of partitioned
               quanted dimension is defined   */    /*E0366*/

           if(AcrossQuantumReg == 0)
           {  /* */    /*E0367*/

              /* Calculate quant value for
                     quanted dimension     */    /*E0368*/

              tdouble = ceil( (double)PL->Set[i].Size /
                              (double)PL->AcrossGroup );
              tdouble = DStep * ceil(tdouble/DStep);

              /* Calculate number of quants for
                   the internal local domain    */    /*E0369*/

              PL->AcrossQNumber = (int)ceil(DSize/tdouble);
           }
           else
           {  /* */    /*E0370*/

              /* */    /*E0371*/

              coil = 1;

              for(j=0; j < PL->QDimNumber; j++)
              {  k = PL->QDim[j];
                 coil *= PL->Set[k].Size;
              }

              tdouble = ceil( (double)coil / (double)PL->AcrossGroup );

              /* */    /*E0372*/

              coil = 1;

              for(j=0; j < PL->QDimNumber; j++)
              {  k = PL->QDim[j];
                 coil *= Global[k].Size;
              }

              PL->AcrossQNumber = (int)ceil( (double)coil / tdouble );
           }
        }
        else
        {  /* One iteration execution time is defined */    /*E0373*/
           /* Calculate PL->AcrossQNumber */    /*E0374*/

           CrtShdGrpBuffers(NewBGHandlePtr); /* create edge buffers
                                                for the edge group */    /*E0375*/
           GetAcrossQNumber(PL, tlong, PL->PipeLineAxis1, VMAxis, NewBG);

        }

        /* Number of portions the internal part
             of the loop is to be divided in    */    /*E0376*/

        coil = dvm_min(PL->AcrossQNumber, tlong);

        if(coil < 2)
        {  ErrPipeLine = 190;
           break; /* */    /*E0377*/
        }

        for(j=0; j < PL->QDimNumber; j++)
        {  if(coil < 2)
              break;  /* */    /*E0378*/

           k = PL->QDim[j];
           DStep = (double)Global[k].Step;
           DSize = (double)Global[k].Size;

           tlong = (DvmType)ceil(DSize / DStep); /* */    /*E0379*/

           PL->StdPLQCount[j] = dvm_min(tlong, coil); /* */    /*E0380*/

           /* */    /*E0381*/

           PL->StdPLQSize[j] = tlong / PL->StdPLQCount[j];

           /* */    /*E0382*/

           coil = (DvmType)(ceil((double)coil /
                               (double)PL->StdPLQCount[j] ));
        }


/*
if(PL->Tc != 0. || PL->AcrossGroup == 0)
{

tprintf("++++++1 j=%d QDimNumber=%d AcrossQNumber=%d\n",
        j, PL->QDimNumber, PL->AcrossQNumber);
   if(j != PL->QDimNumber)
   {
        tlong = 1;

        for(k=0; k < j; k++)
        {  DStep = (double)Local[PL->QDim[k]].Step;
           DSize = (double)Local[PL->QDim[k]].Size;
           tlong *= (DvmType)ceil(DSize / DStep);
        }

        PL->QDimNumber = j;
        GetAcrossQNumber(PL, tlong, PL->PipeLineAxis1, VMAxis, NewBG);
tprintf("++++++2 AcrossQNumber=%d\n", PL->AcrossQNumber);

   }


}
*/    /*E0383*/

        PL->QDimNumber = j; /* */    /*E0384*/

        /* Output information in the stream of information messages */    /*E0385*/

        if(AcrossInfoPrint && _SysInfoPrint &&
           MPS_CurrentProc == DVM_IOProc)
        {  if(PL->Tc == 0. && PL->AcrossGroup != 0)
           {  /* Number of quanted dimension
                 partition group is defined  */    /*E0386*/

              rtl_iprintf("\n*** RTS: FILE=%s LINE=%d\n",
                          DVM_FILE[0], DVM_LINE[0]);
           }
           else
           {  /* One iteration execution time is defined */    /*E0387*/

              rtl_iprintf("\n*** RTS: FILE=%s LINE=%d "
                          "MeanProcTime=%le CoilTime=%le\n",
                          DVM_FILE[0], DVM_LINE[0],
                          MeanProcPower, PL->Tc/MeanProcPower);
           }

           for(j=0; j < PL->QDimNumber; j++)
           {  k = PL->QDim[j];
              rtl_iprintf("AcrossGroupNumber[%d]=%ld  "
                          "AcrossGroupSize[%d]=%ld\n",
                          k, PL->StdPLQCount[j], k, PL->StdPLQSize[j]);
           }
        }

        /* ------------------------------------------------- */    /*E0388*/

        if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
           for(j=0; j < PL->QDimNumber; j++)
           {  k = PL->QDim[j];
              tprintf("StdPLQCount[%d]=%ld  StdPLQSize[%d]=%ld\n",
                      k, PL->StdPLQCount[j], k, PL->StdPLQSize[j]);
           }

        CondPipeLine = 1;
        break;
     }

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
        tprintf("CondPipeLine=%d  ErrPipeLine=%d\n",
                 CondPipeLine, ErrPipeLine);

     if(CondPipeLine)
     {  PL->PipeLineSign = 1; /* conditions for <sendsh_ - recvsh_>
                                 scheme pipelining are satisfied */    /*E0389*/
     }
     else
     {  /* Conditions for <sendsh_ - recvsh_> scheme
                 pipelining are not satisfied        */    /*E0390*/

        SetHostOper(StartShdGrp)

        if(PL->PipeLineSign)
           ( RTL_CALL, recvla_(&PL->NewShadowGroup2Ref) );

        PL->PipeLineSign   = 0;

        if(ErrPipeLine != 32)
        {  if(PL->SetTcSign)
           {  PL->SetTcSign = 0;
              PL->Tc = 0.;
              TcPL = NULL;
              TcCount--;
           }
        }

        ( RTL_CALL, recvsh_(NewShadowGroupRefPtr) );
        SetHostOper(ShdGrp)
     }
  }

  stat_event_flag = 0; /* flag of edge exchange function execution
                          from the user is off */    /*E0391*/

  if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
  {  if(PL->QDimNumber == 0 || PL->QDim[0] == -1)
        tprintf("QDim[0]=-1\n");
     else
        for(j=0; j < PL->QDimNumber; j++)
            tprintf("QDim[%d]=%d\n", j, PL->QDim[j]);
  }

  if(RTL_TRACE)
     dvm_trace(ret_across_,"ErrPipeLine=%d\n", ErrPipeLine);

  DVMFTimeFinish(ret_across_);
  return  (DVM_RET, (DvmType)ErrPipeLine);
}



DvmType __callstd dopl_(LoopRef  *LoopRefPtr)

/*
     Inquiry of continuation of parallel loop execution.
     ---------------------------------------------------

*LoopRefPtr - reference to the parallel loop.

The function allows to determine the completion of the execution of all
the parallel loop parts, on which the loop have been divided by the
functions exfrst_ or imlast_.

The function returns the following values:

0 - the execution of all parts of the parallel loop is completed;
1 - the execution of the parallel loop has to be continued.
*/    /*E0392*/

{ SysHandle      *LoopHandlePtr, *BGHandlePtr;
  DvmType            IntProc, Init, Last, tlong;
  s_PARLOOP      *PL;
  int             i, j, k, m, MsgCount;
  s_VMS          *VMS;
  s_REDGROUP     *RG;
  s_REDVAR       *RV;
  s_AMVIEW       *AMV;
  s_AMS          *AMS;
  s_PLQUANTUM    *PLQ;
  double          call_dopl_time;
  DvmType           *LongPtr1, *LongPtr2;
  byte            StopIter, InterSign;
  s_BOUNDGROUP   *BG;


#ifndef NO_DOPL_DOPLMB_TRACE

    char buffer[999];

    PL = (s_PARLOOP *)((SysHandle *) *LoopRefPtr )->pP;
    if ( PL->HasLocal && PL->Local )
    {
        sprintf(buffer, "\n   call dopl_\n");
        SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

        for( i=0; i < PL->Rank; i++ )
        {
            sprintf(buffer, "\tloop[%d] Init = %d, Last = %d, Step = %ld\n", i, *(PL->MapList[i].InitIndexPtr), *(PL->MapList[i].LastIndexPtr), *(PL->MapList[i].StepPtr));
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
        }
    }
    else SYSTEM(fputs, ("call dopl: NO LOCAL PART\n", Trace.DoplMBFileHandle));

#endif


  if(TcPL || MeasurePL || PLTimeMeasure)
     call_dopl_time = dvm_time(); /* for execution time
                                     calculation of iteration group */    /*E0393*/

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0394*/
  DVMFTimeStart(call_dopl_);

  CurrEnvProcCount    = OldEnvProcCount;
  CurrEnvProcCount_m1 = CurrEnvProcCount - 1;
  d1_CurrEnvProcCount = 1./CurrEnvProcCount;

  CurrAMHandlePtr = OldAMHandlePtr;

  /* Restore variables characteristic
       for current processor system   */    /*E0395*/

  DVM_VMS = ((s_AMS *)CurrAMHandlePtr->pP)->VMS;

  DVM_MasterProc  = DVM_VMS->MasterProc;
  DVM_IOProc      = DVM_VMS->IOProc;
  DVM_CentralProc = DVM_VMS->CentralProc;
  DVM_ProcCount   = DVM_VMS->ProcCount;

  /* Initial settings */    /*E0396*/

  MsgCount  = 0;
  StopIter  = 0;
  InterSign = 0;

  if(RTL_TRACE)
     dvm_trace(call_dopl_,"LoopRefPtr=%lx; LoopRef=%lx;\n",
                          (uLLng)LoopRefPtr, *LoopRefPtr);

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 060.081: wrong call dopl_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 060.080: wrong call dopl_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.082: wrong call dopl_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  /*       Addition to the loop execution time
     which iteration execution time is to be measured */    /*E0397*/

  if(PL->SetTcSign && PL->ret_dopl_time != 0.)
     PL->Tc += call_dopl_time - PL->ret_dopl_time;

  stat_event_flag = 1; /* flag: edge exchange functions are executed
                          (for statistics) */    /*E0398*/

  /* Wait for imported or local elements to execute ACROSS scheme
              without pipelining or create buffer groups
              to execute ACROSS scheme with pipelining            */    /*E0399*/

  if(PL->AcrType1)
  {  /* scheme <sendsh_ - recvsh_> is executing */    /*E0400*/

     if(PL->PipeLineSign == 0)
     {  /* <sendsh_ - recvsh_> scheme was
           initialized without pipelining */    /*E0401*/

        if(PL->IsAcrType1Wait == 0)
        {  /* No waiting for recvsh_ */    /*E0402*/

           PL->IsAcrType1Wait = 1;
           SetHostOper(WaitShdGrp)
           ( RTL_CALL, waitsh_(&PL->NewShadowGroup1Ref) );
           SetHostOper(DoPLGrp)
        }
     }
     else
     {  /* <sendsh_ - recvsh_> scheme was
            initialized with pipelining   */    /*E0403*/

        if(PL->IsPipeLineInit == 0)
        {  /* Pipeline has not been initialized yet */    /*E0404*/

           PL->IsPipeLineInit = 1;

           BGHandlePtr = (SysHandle *)PL->NewShadowGroup1Ref;
           BG          = (s_BOUNDGROUP *)BGHandlePtr->pP;

           BG->IsStrt = 1;  /* flag: some edge exchange
                               has already been started */    /*E0405*/
           BG->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle AM,
                                                    which started the
                                                    operation */    /*E0406*/
           BG->ShdEnvInd = gEnvColl->Count - 1; /* index of context,
                                                   which started the
                                                   operation */    /*E0407*/

           CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers
                                             for edge group */    /*E0408*/
        }
     }
  }

  if(PL->AcrType2)
  {  /* Scheme <sendsa_ - recvla_> is executing */    /*E0409*/

     if(PL->PipeLineSign == 0)
     {  /* <sendsa_ - recvla_> scheme was
           initialized without pipelining */    /*E0410*/

        if(PL->IsAcrType2Wait == 0)
        { /* No waiting  for recvla_ */    /*E0411*/

          PL->IsAcrType2Wait = 1;
          SetHostOper(WaitShdGrp)
          ( RTL_CALL, waitsh_(&PL->NewShadowGroup2Ref) );
          SetHostOper(DoPLGrp)
        }
     }
     else
     {  /* <sendsa_ - recvla_> scheme was
           initialized with pipelining */    /*E0412*/

        if(PL->IsPipeLineInit == 0)
        {  /* Pipeline has not been initialized yet */    /*E0413*/

           PL->IsPipeLineInit = 1;

           BGHandlePtr = (SysHandle *)PL->NewShadowGroup2Ref;
           BG          = (s_BOUNDGROUP *)BGHandlePtr->pP;

           BG->IsStrt = 1;  /* flag: some edge exchange
                               has already been started */    /*E0414*/
           BG->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle AM,
                                                    which started
                                                    the operation */    /*E0415*/
           BG->ShdEnvInd = gEnvColl->Count - 1; /* index of context,
                                                   which started
                                                   the operation */    /*E0416*/

           CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers
                                             for edge group */    /*E0417*/
        }
     }
  }

  /* Reduction execution for the started reduction groups */    /*E0418*/

  if(PL->WaitRedSign)
  {
     PL->WaitRedSign = 0; /* flag on outstripping asynchronous
                             reduction execution is off */    /*E0419*/

     SetHostOper(WaitRedGrp)

     while(MsgCount != MessageCount[16])
     {  MsgCount = MessageCount[16];

        for(j=0; j < gRedGroupColl->Count; j++)
        {  RG = coll_At(s_REDGROUP *, gRedGroupColl, j);

           VMS = RG->VMS; /* reduction group processor sysytem */    /*E0420*/

           if(VMS->CentralProc != MPS_CurrentProc)
              continue; /* the current processor is not central for
                           the group processor sysytem */    /*E0421*/

           NotSubsystem(i, DVM_VMS, VMS)

           if(i)
              continue; /* the group processor system is not
                           a subsystem of the current processor system */    /*E0422*/

           #ifdef _DVM_GNS_
              if(RG->MessageCount == MsgCount)
                 continue;   /* no new messages */    /*E0423*/
              RG->MessageCount = MsgCount;
           #endif

           for(i=0; i < VMS->ProcCount; i++)
           {  if(RG->Flag[i])
                 continue;  /* the completion of waiting for the
                               end of receiving by the central */    /*E0424*/
              if(RTL_TRACE)
              {  if( ( RTL_CALL, red_Testrequest(&RG->Req[i]) ) == 0)
                    break;
              }
              else
              {  if( ( RTL_CALL, rtl_Testrequest(&RG->Req[i]) ) == 0)
                    break;
              }

              RG->Flag[i] = 1;  /* flag that waiting for the end of
                                   receiving by the central
                                   is completed */    /*E0425*/
              if(RTL_TRACE)
                 ( RTL_CALL, red_Waitrequest(&RG->Req[i]) );
              else
                 ( RTL_CALL, rtl_Waitrequest(&RG->Req[i]) );
           }

           if(i == VMS->ProcCount) /* if all massages for the group
                                      have been received */    /*E0426*/
           {  coll_AtDelete(gRedGroupColl, j); /* delete from
                                                  started group list */    /*E0427*/
              for(i=0; i < VMS->ProcCount; i++)
              {  if(VMS->VProc[i].lP != MPS_CurrentProc)
                 {  IntProc = VMS->VProc[i].lP; /* internal number of
                                                 the current processor */    /*E0428*/
                    for(k=0; k < RG->RV.Count; k++)
                    {  RV = coll_At(s_REDVAR *, &RG->RV, k);

                       if(RV->AMView)
                       {  /* Reduction for subtasks */    /*E0429*/

                          AMV = RV->AMView;

                          for(m=0; m < AMV->AMSColl.Count; m++)
                          { AMS = coll_At(s_AMS *, &AMV->AMSColl, m);

                            if(AMS->VMS == NULL)
                               continue; /* abstract machine
                                            has not been mapped */    /*E0430*/

                            if(AMS->VMS->CentralProc != IntProc)
                               continue; /* the current processor is not
                                            central for the current
                                            subtask */    /*E0431*/

                            CalculateRedVars(&RG->RV,
                              RG->NoWaitBufferPtr[i],
                              RG->NoWaitBufferPtr[VMS->VMSCentralProc],
                              k);
                          }
                       }
                       else
                       {  /* Reduction on processor system */    /*E0432*/

                          CalculateRedVars(&RG->RV,
                            RG->NoWaitBufferPtr[i],
                            RG->NoWaitBufferPtr[VMS->VMSCentralProc],
                            k);
                       }
                    }
                 }
              }

              CopyRedVars2Buffer(&RG->RV, RG->NoWaitBufferPtr[0]);

              for(i=0; i < VMS->ProcCount; i++)
              {  if(VMS->VProc[i].lP != MPS_CurrentProc)
                 {  if(RTL_TRACE)
                      (RTL_CALL, red_Sendnowait(RG->NoWaitBufferPtr[0],
                                                1, RG->BlockSize,
                                                (int)VMS->VProc[i].lP,
                                                DVM_VMS->tag_RedVar,
                                                &RG->Req[i],
                                                RedCentral));
                    else
                      (RTL_CALL, rtl_Sendnowait(RG->NoWaitBufferPtr[0],
                                                1, RG->BlockSize,
                                                (int)VMS->VProc[i].lP,
                                                DVM_VMS->tag_RedVar,
                                                &RG->Req[i],
                                                RedCentral));
                 }
              }

              if(MsgSchedule && UserSumFlag)
              {  rtl_TstReqColl(0);
                 rtl_SendReqColl(ResCoeffRedCentral);
              }
           }
           else
           {  /* A Testrequest returns bad code */    /*E0433*/

           }
        }
     }

     SetHostOper(DoPLGrp)
  }

  /* */    /*E0434*/

  if(MsgSchedule)
  {  i = rtl_TstReqColl(1);

     if(i)
        rtl_SendReqColl(ResCoeffDoPL);
  }

  /* MPI_Test function execution */    /*E0435*/

  #ifdef  _DVM_MPI_

     if(dopl_MPI_Test)
     {  MPI_Status  Status;

        for(i=0; i < RequestCount; i++)
        {  SYSTEM(MPI_Test, (&RequestBuffer[i]->MPSFlag, &j,
                             &RequestBuffer[i]->Status) )

           if(j)
           {  /* */    /*E0436*/

              if(RequestBuffer[i]->CompressBuf != NULL &&
                 RequestBuffer[i]->SendSign == 0 &&
                 RequestBuffer[i]->IsCompressSize == 0)
              {
                 SYSTEM(MPI_Get_count,
                        (&RequestBuffer[i]->Status, MPI_BYTE,
                         &RequestBuffer[i]->CompressMsgLen))
                 RequestBuffer[i]->IsCompressSize = 1;
              }

              /* Delete pointer to exchange flag from
              buffer if exchange has been finished */    /*E0437*/

              for(k=i+1; k < RequestCount; k++)
                  RequestBuffer[k-1] = RequestBuffer[k];
              RequestCount--;
              i--;
           }
        }

        m = dvm_min(MPS_RequestCount, dopl_MPI_Test_Count);

        for(i=0; i < m; i++)
        {  SYSTEM(MPI_Test, (MPS_RequestBuffer[i], &j, &Status) )

           /* */    /*E0438*/

           if(j)
           {  for(k=i+1; k < MPS_RequestCount; k++)
                  MPS_RequestBuffer[k-1] = MPS_RequestBuffer[k];
              MPS_RequestCount--;
              m = dvm_min(MPS_RequestCount, dopl_MPI_Test_Count);
              i--;
           }
        }
     }

  #endif

  /* ---------------------------------------------------- */    /*E0439*/

  if(PL->HasLocal == 0)
  {  if(PL->IterFlag == ITER_BOUNDS_FIRST)
     {  SetHostOper(StartShdGrp)
        ( RTL_CALL, strtsh_(&PL->BGRef) );
        SetHostOper(DoPLGrp)
     }

     if(PL->IterFlag == ITER_BOUNDS_LAST)
     {  SetHostOper(WaitShdGrp)
        ( RTL_CALL, waitsh_(&PL->BGRef) );
        SetHostOper(DoPLGrp)
     }

     StopIter = 1;
  }
  else
  {  if(PL->DoQuantum)
     {  PLQ = PL->PLQ;

        /* Save the time of iteration quant execution */    /*E0440*/

        IntProc = PLQ->QCount;
        PLQ->QTime[IntProc].Time = call_dopl_time - PL->ret_dopl_time;
        IntProc++;
        PLQ->QCount = IntProc;

        if(IntProc == PLQ->QNumber)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** RTS fatal err 060.085: "
                    "overflow of the time measure counter\n"
                    "(LoopRef=%lx; Time measure counter = %ld)\n",
                    *LoopRefPtr, IntProc);

        /* Continue execution of iteration group
              with calculation of its quants     */    /*E0441*/

        for(i=PL->Rank-1; i >= 0; i--)
        {  if(PLQ->QSign[i] == 0)
              continue;  /* skip non quant dimensioon */    /*E0442*/

           Last = *(PL->MapList[i].LastIndexPtr);

           if(PL->Invers[i])
           {  /* Quant dimension with negative step */    /*E0443*/

              if(Last <= PLQ->SLastIndex[i])
                 continue; /* the current quant dimension is exhausted,
                              to the next loop dimension */    /*E0444*/

              Init = Last - 1;
              *(PL->MapList[i].InitIndexPtr) = Init;

              Last = PLQ->SLastIndex[i];

              if(Last <= Init - PLQ->QSize[i])
                 *(PL->MapList[i].LastIndexPtr) =
                 Init - PLQ->QSize[i] + 1;
              else
                 *(PL->MapList[i].LastIndexPtr) = Last;
           }
           else
           {  /* Quant dimension with positive step */    /*E0445*/

              if(Last >= PLQ->SLastIndex[i])
                 continue; /* the current quant dimension is exhausted,
                              to the next loop dimension */    /*E0446*/

              Init = Last + 1;
              *(PL->MapList[i].InitIndexPtr) = Init;

              Last = PLQ->SLastIndex[i];

              if(Last >= Init + PLQ->QSize[i])
                 *(PL->MapList[i].LastIndexPtr) =
                 Init + PLQ->QSize[i] - 1;
              else
                 *(PL->MapList[i].LastIndexPtr) = Last;
           }
           break;
        }

        if(i < 0)
           PL->DoQuantum = 0; /* all quants of iteration group has been
                                 execited */    /*E0447*/
        else
        {  /* Continue execution of iteration group quants */    /*E0448*/

           for(i++; i < PL->Rank; i++)
           {  /*  Back to the initial state of all dimensions with
                 numbers greater than current quant dimension number */    /*E0449*/

              if(PLQ->QSign[i] == 0)
                 continue;  /* skip non quant dimension */    /*E0450*/

              Init = PLQ->SInitIndex[i];
              Last = PLQ->SLastIndex[i];
              *(PL->MapList[i].InitIndexPtr) = Init;
              *(PL->MapList[i].LastIndexPtr) = Last;

              if(PL->Invers[i])
              {  if(Last <= Init - PLQ->QSize[i])
                    *(PL->MapList[i].LastIndexPtr) =
                    Init - PLQ->QSize[i] + 1;
              }
              else
              {  if(Last >= Init + PLQ->QSize[i])
                    *(PL->MapList[i].LastIndexPtr) =
                    Init + PLQ->QSize[i] - 1;
              }
           }

           /* Save parameters of the current iteration group */    /*E0451*/

           LongPtr1 = &(PLQ->QTime[PLQ->QCount].InitIndex[0]);
           LongPtr2 = &(PLQ->QTime[PLQ->QCount].LastIndex[0]);

           for(m=0; m < PL->Rank; m++,LongPtr1++,LongPtr2++)
           {  *LongPtr1 = *(PL->MapList[m].InitIndexPtr);
              *LongPtr2 = *(PL->MapList[m].LastIndexPtr);
           }
        }
     }

     if(PL->QDimSign)
     {  /* Execution of current part of internal part of the loop */    /*E0452*/

        if(PL->PipeLineSign)
        {  /* ACROSS scheme with pipeline is executed */    /*E0453*/

           if(PL->FirstQuantum)     /* if the first portion of iteration
                                       has been executed */    /*E0454*/
              PL->FirstQuantum = 0;
           else
           {  if(ASynchrPipeLine)
              {  SetHostOper(WaitShdGrp)
                 WaitAcrossSend(PL);
              }
           }

           SetHostOper(StartShdGrp)
           InitAcrossSend(PL);

           if(ASynchrPipeLine == 0)
           {  SetHostOper(WaitShdGrp)
              WaitAcrossSend(PL);
              SetHostOper(StartShdGrp)
              InitAcrossRecv(PL);
           }

           SetHostOper(WaitShdGrp)
           WaitAcrossRecv(PL);
           SetHostOper(DoPLGrp)

           for(j=0; j < PL->QDimNumber; j++) /* */    /*E0455*/
           {
              i = PL->QDim[j];     /* */    /*E0456*/
              PL->CurrInit[j] = PL->NextInit[j];
              PL->CurrLast[j] = PL->NextLast[j];
              *(PL->MapList[i].InitIndexPtr) = PL->CurrInit[j];
              *(PL->MapList[i].LastIndexPtr) = PL->CurrLast[j];
           }

           /* */    /*E0457*/

           for(j=PL->QDimNumber-1; j >= 0; j--)
               if(PL->StdPLQCount[j] != 0)
                  break;  /* */    /*E0458*/

           if(j >= 0)
           {  /* If the last portion of internal
                 part iterations is executed ?   */    /*E0459*/

              if(dopl_WaitRD && gRedGroupColl->Count)
                 PL->WaitRedSign = 1; /* attempt to execute reduction */    /*E0460*/

              i = PL->QDim[j]; /* */    /*E0461*/
              tlong = *(PL->MapList[i].StepPtr);

              PL->NextInit[j] = PL->CurrLast[j] + tlong;

              if(PL->StdPLQCount[j] > 1)
              {  /* */    /*E0462*/

                 PL->NextLast[j] = PL->NextInit[j] +
                                   (PL->StdPLQSize[j]-1)*tlong;
              }
              else
              {  /* */    /*E0463*/

                 PL->NextLast[j] = PL->QLastIndex[j];
              }

              PL->StdPLQCount[j]--;

              /* */    /*E0464*/

              for(j++; j < PL->QDimNumber; j++)
              {
                 i = PL->QDim[j];     /* */    /*E0465*/
                 PL->NextInit[j] = PL->QInitIndex[j];
                 tlong = *(PL->MapList[i].StepPtr);

                 PL->NextLast[j] = PL->NextInit[j] +
                                   (PL->StdPLQSize[j]-1)*tlong;
                 PL->StdPLQCount[j] = PL->StdPLQNumb[j];
              }

              if(ASynchrPipeLine)
              {  SetHostOper(StartShdGrp)
                 InitAcrossRecv(PL);
                 SetHostOper(DoPLGrp)
              }
           }
           else
           {  /* If the last portion of internal
                 part iterations is executed ?   */    /*E0466*/

              PL->QDimSign = 0;
           }
        }
        else
        {  /* */    /*E0467*/

           i = PL->QDim[0]; /* number of quantum dimension minus 1 */    /*E0468*/
           PL->StdPLQCount[0]--;
           tlong = *(PL->MapList[i].StepPtr);

           Init = *(PL->MapList[i].LastIndexPtr) + tlong;

           if(PL->StdPLQCount[0])
           {  /* Execution of non last part of internal
                      part iterations of the loop       */    /*E0469*/

              *(PL->MapList[i].InitIndexPtr) = Init;
              *(PL->MapList[i].LastIndexPtr) =
              Init + (PL->StdPLQSize[0] - 1) * tlong;

              if(dopl_WaitRD && gRedGroupColl->Count)
                 PL->WaitRedSign = 1; /* attempt to execute reduction */    /*E0470*/
           }
           else
           {  /* Execution of last part of internal
                    part iterations of the loop     */    /*E0471*/

              PL->QDimSign = 0;

              *(PL->MapList[i].InitIndexPtr) = Init;
              *(PL->MapList[i].LastIndexPtr) = PL->QLastIndex[0];

              if(dopl_WaitRD > 1 && gRedGroupColl->Count &&
                 PL->IterFlag == ITER_BOUNDS_LAST)
                 PL->WaitRedSign = 1; /* attempt to execute reduction */    /*E0472*/
           }
        }
     }
     else
     {  if(PL->DoQuantum == 0)
        {  /* Prepare iteration group for execution */    /*E0473*/

           if(PL->IterFlag == ITER_NORMAL)
           {  StopIter = PL->IsInIter;
              PL->IsInIter = 1;
              InterSign = (byte)!StopIter; /* flag: the internal part
                                              is ready */    /*E0474*/
           }
           else
           {  if(PL->IterFlag == ITER_BOUNDS_FIRST)
              {  if(PL->CurrBlock < PL->Rank*2)
                 {  shd_iter(PL);

                    if(dopl_WaitRD > 1 && gRedGroupColl->Count &&
                       PL->IterFlag == PL->Rank*2)
                       PL->WaitRedSign = 1;
                 }
                 else
                 {  StopIter = PL->IsInIter;

                    if(StopIter == 0)
                    {  SetHostOper(StartShdGrp)
                       ( RTL_CALL, strtsh_(&PL->BGRef) );
                       SetHostOper(DoPLGrp)
                       in_iter(PL);
                       InterSign = 1; /* flag: the internal
                                         part is ready */    /*E0475*/
                    }
                 }
              }
              else
              {  if(PL->IsInIter)
                 {  if(PL->IsWaitShd == 0)
                    {  PL->IsWaitShd = 1;
                       SetHostOper(WaitShdGrp)
                       ( RTL_CALL, waitsh_(&PL->BGRef) );
                       SetHostOper(DoPLGrp)
                    }

                    if(PL->CurrBlock < PL->Rank*2)
                       shd_iter(PL);
                    else
                       StopIter = 1;
                 }
                 else
                 {  in_iter(PL);
                    InterSign = 1; /* flag: the internal
                                      part is ready */    /*E0476*/
                 }
              }
           }

           if(PL->PLQ && StopIter == 0)
           {  /* Prepare iteration quant for execition
                     with calculation of the time      */    /*E0477*/

              PLQ = PL->PLQ;

              for(i=0; i < PL->Rank; i++)
              {  /* Save iteration group parameters */    /*E0478*/

                 Init = *(PL->MapList[i].InitIndexPtr);
                 Last = *(PL->MapList[i].LastIndexPtr);
                 PLQ->SInitIndex[i] = Init;
                 PLQ->SLastIndex[i] = Last;

                 if(PL->Invers[i])
                 {  if(Last > Init)
                       break;
                 }
                 else
                 {  if(Init > Last)
                       break;
                 }
              }

              if(i == PL->Rank)
              {  /* Iteration group is not empty */    /*E0479*/

                 PL->DoQuantum = 1;

                 for(j=0; j < PL->Rank; j++)
                 {  if(PLQ->QSign[j] == 0)
                       continue;  /* skip non quant
                                     dimension */    /*E0480*/

                    Init = PLQ->SInitIndex[j];
                    Last = PLQ->SLastIndex[j];

                    if(PL->Invers[j])
                    {  if(Last <= Init - PLQ->QSize[j])
                          *(PL->MapList[j].LastIndexPtr) =
                          Init - PLQ->QSize[j] + 1;
                    }
                    else
                    {  if(Last >= Init + PLQ->QSize[j])
                          *(PL->MapList[j].LastIndexPtr) =
                          Init + PLQ->QSize[j] - 1;
                    }
                 }

                 /* Save parameters of the first iteration quant */    /*E0481*/

                 LongPtr1 = &(PLQ->QTime[PLQ->QCount].InitIndex[0]);
                 LongPtr2 = &(PLQ->QTime[PLQ->QCount].LastIndex[0]);

                 for(m=0; m < PL->Rank; m++,LongPtr1++,LongPtr2++)
                 {  *LongPtr1 = *(PL->MapList[m].InitIndexPtr);
                    *LongPtr2 = *(PL->MapList[m].LastIndexPtr);
                 }
              }
           }
           else
           {  if(InterSign && PL->PipeLineSign)
              {  /*  Divide internal loop part in portions for
                    pipeline loop execution using ACROSS scheme */    /*E0482*/

                 PL->QDimSign = 1; /* flag: internal loop part
                                      is divided in portions */    /*E0483*/
                 PL->FirstQuantum = 1; /* flag of execution of
                                          the first portion */    /*E0484*/
                 if(dopl_WaitRD && gRedGroupColl->Count)
                    PL->WaitRedSign = 1; /* flag: reduction execution */    /*E0485*/

                 for(j=0; j < PL->QDimNumber; j++) /* */    /*E0486*/
                 {
                    i = PL->QDim[j];     /* number of quanted dimension -1 */    /*E0487*/

                    PL->NextInit[j] = *(PL->MapList[i].InitIndexPtr);

                    PL->QInitIndex[j] = PL->NextInit[j]; /* */    /*E0488*/
                    tlong = *(PL->MapList[i].LastIndexPtr);

                    PL->QLastIndex[j] = tlong; /* save final index value
                                                  for continuation */    /*E0489*/
                    PL->StdPLQNumb[j] = PL->StdPLQCount[j];/* */    /*E0490*/

                    tlong = *(PL->MapList[i].StepPtr);

                    PL->NextLast[j] = PL->NextInit[j] +
                                      (PL->StdPLQSize[j]-1)*tlong;
                    *(PL->MapList[i].LastIndexPtr) = PL->NextLast[j];

                    PL->CurrInit[j] = PL->NextInit[j];
                    PL->CurrLast[j] = PL->NextLast[j];
                 }

                 /* */    /*E0491*/

                 SetHostOper(StartShdGrp)
                 InitAcrossRecv(PL);
                 SetHostOper(WaitShdGrp)
                 WaitAcrossRecv(PL);

                 /* */    /*E0492*/

                 j = PL->QDimNumber - 1;

                 i = PL->QDim[j];   /* */    /*E0493*/
                 tlong = *(PL->MapList[i].StepPtr);

                 PL->StdPLQCount[j]--; /* */    /*E0494*/

                 PL->NextInit[j] = PL->CurrLast[j] + tlong;

                 if(PL->StdPLQCount[j] == 1)
                 {  /* Next portion is the last one */    /*E0495*/

                    PL->NextLast[j] = PL->QLastIndex[j];
                 }
                 else
                 {  /* Next portion is not the last one */    /*E0496*/

                    PL->NextLast[j] = PL->NextInit[j] +
                                      (PL->StdPLQSize[j] - 1) * tlong;
                 }

                 /* */    /*E0497*/

                 if(ASynchrPipeLine)
                 {  SetHostOper(StartShdGrp)
                    InitAcrossRecv(PL);
                 }

                 PL->StdPLQCount[j]--; /* */    /*E0498*/

                 SetHostOper(DoPLGrp)
              }
              else
              {  /* Divide internal loop in parts for outstrip
                     execution of asynchronous reduction and
                             call MPI_Test function            */    /*E0499*/

                 if(InterSign && ( (dopl_WaitRD && gRedGroupColl->Count)
                    || (dopl_MPI_Test && (RequestCount ||
                                          MPS_RequestCount))
                    || (MsgSchedule && (PL->IterFlag != ITER_NORMAL ||
                     FreeChanNumber < ParChanNumber || NewMsgNumber)) ))
                 {  /* Check if the loop internal part
                        is empty, save its parameters  */    /*E0500*/

                    for(i=0; i < PL->Rank; i++)
                    {  Init = *(PL->MapList[i].InitIndexPtr);
                       Last = *(PL->MapList[i].LastIndexPtr);

                       if(PL->Invers[i])
                       {  if(Last > Init)
                             break;
                       }
                       else
                       {  if(Init > Last)
                             break;
                       }
                    }

                    if(i == PL->Rank)
                    {  /* Internal part of the loop is not empty */    /*E0501*/

                       for(i=0; i < PL->Rank; i++)
                       {  Init = *(PL->MapList[i].InitIndexPtr);
                          Last = *(PL->MapList[i].LastIndexPtr);

                          /* The number of executable points
                                   of i+1-th dimension       */    /*E0502*/

                          tlong =
                              (DvmType)ceil((double)(dvm_abs(Last - Init) + 1) /
                                     (double)PL->Set[i].Step);
                          if(tlong < 2)
                             continue; /* only one iteration
                                          is executed in dimension */    /*E0503*/

                          /* Number of perts internal
                                loop is divided in    */    /*E0504*/

                          if(InPLQNumber > tlong)
                             PL->StdPLQCount[0] = tlong;
                          else
                             PL->StdPLQCount[0] = InPLQNumber;

                          PL->QDimSign = 1; /* flag: internal loop
                                            has been diveded in parts */    /*E0505*/
                          PL->QDim[0] = i;  /* the number of divided
                                               dimension minus 1 */    /*E0506*/
                          PL->QLastIndex[0] = Last; /* save the final
                                                    value of the index
                                                    for continuation */    /*E0507*/
                          if(dopl_WaitRD && gRedGroupColl->Count)
                             PL->WaitRedSign = 1; /* flag: execute
                                                     reduction */    /*E0508*/

                          /* The number of points in standard
                                    dimension portion         */    /*E0509*/

                          PL->StdPLQSize[0] = tlong / PL->StdPLQCount[0];

                          PL->StdPLQCount[0]--; /* portion counter */    /*E0510*/

                          *(PL->MapList[i].LastIndexPtr) = Init +
                          (PL->StdPLQSize[0] - 1) *
                          *(PL->MapList[i].StepPtr);

                          break;
                       }
                    }
                 }
              }
           }
        }
     }

     if(RTL_TRACE)
     {  if(dopl_Trace && StopIter == 0  && TstTraceEvent(call_dopl_))
        {  for(i=0; i < PL->Rank; i++)
           {  if(PL->MapList[i].InitIndexPtr != NULL)
                 tprintf("Dim%d: Lower=%ld Upper=%ld Step=%ld\n",
                          i,*(PL->MapList[i].InitIndexPtr),
                          *(PL->MapList[i].LastIndexPtr),
                          *(PL->MapList[i].StepPtr));

              if(PL->MapList[i].LoopVarAddr)
              {  switch(PL->MapList[i].LoopVarType)
                 {  case 0 :
                    PT_LONG(PL->MapList[i].LoopVarAddr) =
                    *(PL->MapList[i].InitIndexPtr);
                    break;

                    case 1 :
                    PT_INT(PL->MapList[i].LoopVarAddr) =
                    (int)(*(PL->MapList[i].InitIndexPtr));
                    break;

                    case 2 :
                    PT_SHORT(PL->MapList[i].LoopVarAddr) =
                    (short)(*(PL->MapList[i].InitIndexPtr));
                    break;

                    case 3 :
                    PT_CHAR(PL->MapList[i].LoopVarAddr) =
                    (char)(*(PL->MapList[i].InitIndexPtr));
                    break;
                 }
              }
              else
                 dopl_dyn_GetLocalBlock = 0;
           }

           if(dopl_dyn_GetLocalBlock)
           {  s_DISARRAY  *DA;
              s_BLOCK      DABlock;

              for(i=0; i < PL->AMView->ArrColl.Count; i++)
              {  DA = coll_At(s_DISARRAY *, &PL->AMView->ArrColl, i);
                 (RTL_CALL, dyn_GetLocalBlock(&DABlock, DA, PL));
              }
           }
        }
     }
  }

  /* Completion of ACROSS schemes */    /*E0511*/

  if(StopIter && (PL->AcrType1 || PL->AcrType2))
  {  if(PL->AcrType1 && PL->PipeLineSign == 0)
     {  /* <sendsh_ - recvsh_> scheme has been
             initialized without pipelining    */    /*E0512*/

        SetHostOper(StartShdGrp)
        ( RTL_CALL, sendsh_(&PL->NewShadowGroup1Ref) );
     }

     if(PL->AcrType2 && PL->PipeLineSign == 0)
     {  /* <sendsa_ - recvla_> scheme has been
             initialized without pipelining    */    /*E0513*/

        SetHostOper(StartShdGrp)
        ( RTL_CALL, sendsa_(&PL->NewShadowGroup2Ref) );
     }

     if(PL->PipeLineSign)
     {  /* ACROSS schemes have been initialized without pipelining */    /*E0514*/

        if(ASynchrPipeLine)
        {  SetHostOper(WaitShdGrp)
           WaitAcrossSend(PL);
        }

        SetHostOper(StartShdGrp)
        InitAcrossSend(PL);
     }

     if(PL->AcrType1 && PL->PipeLineSign == 0)
     {  /* <sendsh_ - recvsh_> scheme has been
             initialized without pipelining    */    /*E0515*/

        SetHostOper(WaitShdGrp)
        ( RTL_CALL, waitsh_(&PL->NewShadowGroup1Ref) );
     }

     if(PL->AcrType2 && PL->PipeLineSign == 0)
     {  /* <sendsa_ - recvla_> scheme has been
             initialized without pipelining    */    /*E0516*/

        SetHostOper(WaitShdGrp)
        ( RTL_CALL, waitsh_(&PL->NewShadowGroup2Ref) );
     }

     if(PL->PipeLineSign)
     {  SetHostOper(WaitShdGrp)
        WaitAcrossSend(PL); /* ACROSS schemes have been initialized
                               with pipelining */    /*E0517*/
     }

     SetHostOper(DoPLGrp)

     if(PL->AcrType1)
     {  if(PL->OldShadowGroup1Ref)
           ( RTL_CALL, delobj_(&PL->OldShadowGroup1Ref) );
        ( RTL_CALL, delobj_(&PL->NewShadowGroup1Ref) );

        if(PL->AcrShadowGroup1Ref)
           ( RTL_CALL, delobj_(&PL->AcrShadowGroup1Ref) );
     }

     if(PL->AcrType2)
     {  if(PL->OldShadowGroup2Ref)
           ( RTL_CALL, delobj_(&PL->OldShadowGroup2Ref) );
        ( RTL_CALL, delobj_(&PL->NewShadowGroup2Ref) );

        if(PL->AcrShadowGroup2Ref)
           ( RTL_CALL, delobj_(&PL->AcrShadowGroup2Ref) );
     }
  }

  stat_event_flag = 0; /* flag off edge exchange function execution
                          by user */    /*E0518*/

  if(RTL_TRACE)
     dvm_trace(ret_dopl_,"DoPL=%d;\n", (int)!StopIter);

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0519*/
  DVMFTimeFinish(ret_dopl_);

  if(StopIter == 0)
  {  CurrEnvProcCount = NewEnvProcCount;
     CurrEnvProcCount_m1 = CurrEnvProcCount - 1;
     d1_CurrEnvProcCount = 1./CurrEnvProcCount;

     CurrAMHandlePtr = NewAMHandlePtr;

     /* Initialize variables characteristic
          to the current processor system   */    /*E0520*/

     DVM_VMS = ((s_AMS *)CurrAMHandlePtr->pP)->VMS;

     DVM_MasterProc  = DVM_VMS->MasterProc;
     DVM_IOProc      = DVM_VMS->IOProc;
     DVM_CentralProc = DVM_VMS->CentralProc;
     DVM_ProcCount   = DVM_VMS->ProcCount;
  }

  if(PL->SetTcSign || PL->DoQuantum)
     PL->ret_dopl_time = dvm_time(); /* to change the time of
                                        iteration quant execution */    /*E0521*/

#ifndef NO_DOPL_DOPLMB_TRACE
    if ( PL->HasLocal && PL->Local )
    {
            sprintf(buffer, "\n   ret  dopl = %ld, SRCLine=%ld\n", (DvmType)!StopIter, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            for( i=0; i < PL->Rank; i++ )
            {
                sprintf(buffer, "\tloop[%d] Init = %ld, Last = %ld, Step = %ld\n", i, *(PL->MapList[i].InitIndexPtr), *(PL->MapList[i].LastIndexPtr), *(PL->MapList[i].StepPtr));
                SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
            }
    }
    else SYSTEM(fputs, ("call dopl: NO LOCAL PART\n", Trace.DoplMBFileHandle));
#endif


    return  (DVM_RET, (DvmType)!StopIter);
}



DvmType  __callstd endpl_(LoopRef  *LoopRefPtr)

/*
      Terminating parallel loop.
      --------------------------

*LoopRefPtr - reference to the parallel loop.

The function endpl_ completes the parallel loop execution and force
merger of the parallel branches to the parental one.
The function returns zero.
*/    /*E0522*/

{ SysHandle       *LoopHandlePtr;
  s_PARLOOP       *PL;
  s_ENVIRONMENT   *Env;
  s_PLQUANTUM     *PLQ;
  s_QTIME         *QTime;
  int              i, j, k, Count, Rank, TAxis, GroupNumber, A, B,
                   PLInit, Init, Last, InitGrp, LastGrp, InitGroup,
                   LastGroup;
  s_AMVIEW        *AMV;
  double          *dptr;
  double           GroupSize, InitAMIndex, LastAMIndex, CoordTime,
                   GroupTime, InitCoord, LastCoord;
  s_VMS           *VMS;
  PSRef            VMRef;
  double           Tc[2];
  RTL_Request     *Req;
  RTL_Request      InReq;

  StatObjectRef = (ObjectRef)*LoopRefPtr; /* for statistics */    /*E0523*/
  DVMFTimeStart(call_endpl_);

  CurrEnvProcCount = OldEnvProcCount;
  NewEnvProcCount  = CurrEnvProcCount;
  CurrEnvProcCount_m1 = CurrEnvProcCount - 1;
  d1_CurrEnvProcCount = 1./CurrEnvProcCount;

  CurrAMHandlePtr = OldAMHandlePtr;

  /* Restore variables characteristic
     to the current processor system  */    /*E0524*/

  DVM_VMS = ((s_AMS *)CurrAMHandlePtr->pP)->VMS;

  DVM_MasterProc  = DVM_VMS->MasterProc;
  DVM_IOProc      = DVM_VMS->IOProc;
  DVM_CentralProc = DVM_VMS->CentralProc;
  DVM_ProcCount   = DVM_VMS->ProcCount;

  if(RTL_TRACE)
     dvm_trace(call_endpl_,"LoopRefPtr=%lx; LoopRef=%lx;\n",
                           (uLLng)LoopRefPtr, *LoopRefPtr);

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 060.091: wrong call endpl_\n"
              "(the object is not a parallel loop; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  Env = genv_GetCurrEnv();
  PL = Env->ParLoop;

  if(TstObject)
  {  if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 060.090: wrong call endpl_ "
                 "(the current context is not the parallel loop; "
                 "LoopRef=%lx)\n", *LoopRefPtr);
  }

  /* Calculate and broadcast one iteration execution time */    /*E0525*/

  if(PL->SetTcSign)
  {  SetHostOper(ShdGrp)

     TcPL = NULL; /* loop which iteration execution time
                     was to be measured has completed */    /*E0526*/

     VMS = PL->AMView->VMS;
     VMRef = (PSRef)VMS->HandlePtr;

     if(PL->HasLocal)
     {  for(i=0; i < PL->Rank; i++)
            PL->Tc /= ceil((double)PL->Local[i].Size /
                           (double)PL->Local[i].Step);
        Tc[0] = PL->Tc;
     }
     else
     {  PL->Tc = 0.;
        Tc[0] = -1.;
     }

     if(MPS_CurrentProc == VMS->CentralProc)
     {  /* The current processor is a central processor
                     of loop processor system           */    /*E0527*/

        dvm_AllocArray(RTL_Request, VMS->ProcCount, Req);
        k = (int)(VMS->ProcCount + VMS->ProcCount);
        dvm_AllocArray(double, k, dptr);

        /* Receive loop iteration execution time
              from all non central processors    */    /*E0528*/

        for(i=0; i < VMS->ProcCount; i++)
        {  if(VMS->VProc[i].lP == MPS_CurrentProc)
              continue;

           k = i + i;
           ( RTL_CALL, rtl_Recvnowait(&dptr[k], 1, sizeof(double),
                                      (int)VMS->VProc[i].lP,
                                      msg_common, &Req[i], 0) );
        }

        for(i=0; i < VMS->ProcCount; i++)
        {  if(VMS->VProc[i].lP != MPS_CurrentProc)
              ( RTL_CALL, rtl_Waitrequest(&Req[i]) );
           else
           {  k = i + i;
              dptr[k] = Tc[0];
           }
        }

        /* Calculate average time of iteration execution */    /*E0529*/

        j = 0; /* number of processors on which
                  at least one iteration has been executed */    /*E0530*/
        Tc[0] = 0.; /* average time of iteration execution */    /*E0531*/

        for(i=0; i < VMS->ProcCount; i++)
        {  k = i + i;

           if(dptr[k] < 0.)
           {  Tc[0] = -10.;
              break;
           }

           j++;
           Tc[0] += dptr[k];
        }

        if(j > 0)
           Tc[0] /= (double)j;

        dvm_FreeArray(Req);
        dvm_FreeArray(dptr);
     }
     else
     {  /* The current processor is not a central processor
                      of loop processor system              */    /*E0532*/
        if(VMS->HasCurrent)
        {  ( RTL_CALL, rtl_Sendnowait(Tc, 1, sizeof(double),
                                      VMS->CentralProc, msg_common,
                                      &InReq, 0) );
           ( RTL_CALL, rtl_Waitrequest(&InReq) );
        }
     }

     /* Send and receive average iteration execution time */    /*E0533*/

     ( RTL_CALL, rtl_BroadCast(Tc, 1, sizeof(double),
                               VMS->CentralProc, &VMRef) );
     TcTime[TcCount-1] = Tc[0]; /* fix iteration execution time */    /*E0534*/

     *(PL->PipeLineParPtr) = Tc[0]/MeanProcPower; /* write iteration
                                                     execution time
                                                     in user program */    /*E0535*/

     SetHostOper(MapPLGrp)
  }

  /* Output information of loop iteration quant execution time */    /*E0536*/

  if(RTL_TRACE && PLTimeTrace && PL->PLQ)
  {  PLQ = PL->PLQ;
     Rank = PLQ->PLRank;
     Count = (int)PLQ->QCount;

     if(PLTimeTrace < 2)
     {  /* Output information of iteration quant execution time */    /*E0537*/

        tprintf("PL time measuring: UserFile=%s; UserLine=%ld; "
                "PLRank=%d;\n",
                PLQ->DVM_FILE, PLQ->DVM_LINE, PLQ->PLRank);

        for(i=0; i < Rank; i++)
        {  if(PLQ->Invers[i])
              tprintf("PLDim=%d; Init=%ld; Last=%ld; Step=%ld; "
                      "GrpNumber=%d; QSign=%d; QSize=%ld;\n",
                      i, PLQ->LastIndex[i], PLQ->InitIndex[i],
                      -PLQ->PLStep[i], PLQ->PLGroupNumber[i],
                      (int)PLQ->QSign[i], PLQ->QSize[i]);
           else
              tprintf("PLDim=%d; Init=%ld; Last=%ld; Step=%ld; "
                      "GrpNumber=%d; QSign=%d; QSize=%ld;\n",
                      i, PLQ->InitIndex[i], PLQ->LastIndex[i],
                      PLQ->PLStep[i], PLQ->PLGroupNumber[i],
                      (int)PLQ->QSign[i], PLQ->QSize[i]);
        }

        tprintf(" \n");

        for(i=0; i < Count; i++)
        {  QTime = &PLQ->QTime[i];

           for(j=0; j < Rank; j++)
               tprintf("d=%d; i=%-5ld; l=%-5ld; ",
                       j, QTime->InitIndex[j], QTime->LastIndex[j]);
           tprintf("t=%lf;\n", QTime->Time);
        }

        tprintf(" \n");
     }
     else
     {  /* Output of coordinate group weights for each dimension
                    of abstract machine representation           */    /*E0538*/

        AMV = PL->AMView; /* AM representatin
                             the loop is mapped on */    /*E0539*/

        tprintf("PL time measuring: UserFile=%s; UserLine=%ld; "
                "PLRank=%d; AMViewRef=%lx;\n",
                PLQ->DVM_FILE, PLQ->DVM_LINE, PLQ->PLRank,
                (uLLng)AMV->HandlePtr);

        tprintf(" \n");

        for(i=0; i < Rank; i++)
        {  if(PLQ->Invers[i])
              tprintf("PLDim=%d; Init=%ld; Last=%ld; Step=%ld; ",
                      i, PLQ->LastIndex[i], PLQ->InitIndex[i],
                      -PLQ->PLStep[i]);
           else
              tprintf("PLDim=%d; Init=%ld; Last=%ld; Step=%ld; ",
                      i, PLQ->InitIndex[i], PLQ->LastIndex[i],
                      PLQ->PLStep[i]);

           if(PLQ->QSign[i] == 0)
           {  /* Loop dimension has not been quanted */    /*E0540*/

              tprintf("GrpNumber=0;\n");
              tprintf(" \n");
              continue;
           }
           else
           {  TAxis = PL->Align[i].TAxis - 1; /* AM representation
                                                 dimension number - 1 */    /*E0541*/
              j = AMV->DISTMAP[TAxis].PAxis - 1; /* processor system
                                                    dimension
                                                    number - 1 */    /*E0542*/
              GroupNumber = PLGroupNumber[j]; /* number of partition
                                                 group of processor
                                                 system dimension */    /*E0543*/
              if(GroupNumber == 0)
              {  /* No quanting for processor system dimension */    /*E0544*/

                 tprintf("GrpNumber=0;\n");
                 tprintf(" \n");
                 continue;
              }
              else
              {  tprintf("GrpNumber=%d; QSize=%ld;\n",
                         GroupNumber, PLQ->QSize[i]);
                 tprintf("AMDim=%d; AMDimSize=%ld; ",
                         TAxis, AMV->Space.Size[TAxis]);

                 /*    Distribution of loop iteration quant
                    execution times over groups of (TAxis+1)-th
                          dimention of AM erpresentation        */    /*E0545*/

                 dvm_AllocArray(double, GroupNumber, dptr);

                 for(j=0; j < GroupNumber; j++)
                     dptr[j] = 0.;

                 A = (int)PL->Align[i].A;
                 B = (int)PL->Align[i].B;

                 GroupSize = (double)AMV->Space.Size[TAxis] /
                             (double)GroupNumber; /* number of
                                                     "coordinates"
                                                     in one group */    /*E0546*/
                 if(GroupSize < 1.)
                    GroupSize = 1.;

                 PLInit = (int)PL->InitIndex[i]; /* initial value
                                                    of loop index */    /*E0547*/

                 InitGrp = INT_MAX;   /*(int)(((word)(-1))>>1);*/    /*E0548*/
                 LastGrp = 0;

                 for(j=0; j < Count; j++)
                 {  QTime = &PLQ->QTime[j];

                    /* Initial and final value of iteration
                       quant index not taking into account
                            loop index initial value        */    /*E0549*/

                    Init = (int)QTime->InitIndex[i] - PLInit;
                    Last = (int)QTime->LastIndex[i] - PLInit;

                    /* Initial and final value of coordinate
                          for AM representation dimension    */    /*E0550*/

                    InitAMIndex = (double)(Init*A + B);
                    LastAMIndex = (double)(Last*A + B);

                    /*  First and last group numbers
                       for AM representation dimension */    /*E0551*/

                    InitGroup = (int)(InitAMIndex / GroupSize);
                    LastGroup = (int)(LastAMIndex / GroupSize);

                    InitGroup = dvm_min(InitGroup, GroupNumber-1);
                    LastGroup = dvm_min(LastGroup, GroupNumber-1);

                    if(InitGroup == LastGroup)
                      dptr[InitGroup] += QTime->Time; /* quant time
                                                         -> in group */    /*E0552*/
                    else
                    { /* First  and last groups are not the same */    /*E0553*/

                      /* Time of one coordinate of AM representation
                                    and time of one group            */    /*E0554*/

                      CoordTime = QTime->Time /
                                  (LastAMIndex - InitAMIndex + 1.);
                      GroupTime = CoordTime * GroupSize;

                      /* Add time of one group
                          to each inner group  */    /*E0555*/

                      for(k=InitGroup+1; k < LastGroup; k++)
                          dptr[k] += GroupTime;

                      /* Additives to first and last groups */    /*E0556*/

                      InitCoord = InitGroup * GroupSize; /* coordinate
                                                            of the first
                                                            group */    /*E0557*/
                      LastCoord = LastGroup * GroupSize; /* coordinate
                                                            of the last
                                                            group */    /*E0558*/
                      dptr[InitGroup] +=
                      CoordTime * (GroupSize - InitAMIndex + InitCoord);
                      dptr[LastGroup] +=
                      CoordTime * (LastAMIndex - LastCoord);
                    }

                    InitGrp = dvm_min(InitGrp, InitGroup);
                    LastGrp = dvm_max(LastGrp, LastGroup);
                 }

                 /* Input of local part of weight array in trace */    /*E0559*/

                 tprintf("InitGrp=%d; LastGrp=%d;\n", InitGrp, LastGrp);

                 for(j=InitGrp,k=0; j <= LastGrp; j++,k++)
                 {  if(k == 4)
                    {  tprintf(" \n");
                       k = 0;
                    }

                    tprintf(" Time[%d]=%lf;", j, dptr[j]);
                 }

                 tprintf(" \n");
                 tprintf(" \n");

                 dvm_FreeArray(dptr);
              }
           }
        }
     }
  }

  /* Loop iteration quant execution time distribution
              over AM representation groups           */    /*E0560*/

  AMV = PL->AMView; /* AM representation the loop is mapped on */    /*E0561*/

  if(PL->PLQ && AMV->TimeMeasure)
  {  PLQ = PL->PLQ;
     Rank = PLQ->PLRank;
     Count = (int)PLQ->QCount;

     for(i=0; i < Rank; i++)
     {  if(PLQ->QSign[i] == 0)
           continue;   /* loop dimension has not been quanted */    /*E0562*/

        TAxis = PL->Align[i].TAxis - 1; /* AM representation
                                           dimension number - 1 */    /*E0563*/
        if(AMV->GroupNumber[TAxis] == 0)
           continue;   /* no task for measuring on (Taxis+1)th
                          dimension of AM representation */    /*E0564*/

        AMV->Is_gettar[TAxis] = 0; /* flag: no result of
                                      measurement query */    /*E0565*/

        A = (int)PL->Align[i].A;
        B = (int)PL->Align[i].B;
        GroupNumber = AMV->GroupNumber[TAxis];
        GroupSize = (double)AMV->Space.Size[TAxis] /
                    (double)GroupNumber; /* number of "coodinates"
                                            in one group */    /*E0566*/
        if(GroupSize < 1.)
           GroupSize = 1.;

        dptr = AMV->GroupWeightArray[TAxis]; /* pointer to arraay with
                                              time weights of groups */    /*E0567*/
        PLInit = (int)PL->InitIndex[i]; /* initial value of
                                           loop counter */    /*E0568*/

        for(j=0; j < Count; j++)
        {  QTime = &PLQ->QTime[j];

           /*    First and last values of quant iteration index
              not taking into account initial value of loop index */    /*E0569*/

           Init = (int)QTime->InitIndex[i] - PLInit;
           Last = (int)QTime->LastIndex[i] - PLInit;

           /* First and last values of coordinate
              for measuring of AM representation  */    /*E0570*/

           InitAMIndex = (double)(Init*A + B);
           LastAMIndex = (double)(Last*A + B);

           /*  First and last numbers of groups
              for measuring of AM representation */    /*E0571*/

           InitGroup = (int)(InitAMIndex / GroupSize);
           LastGroup = (int)(LastAMIndex / GroupSize);

           InitGroup = dvm_min(InitGroup, GroupNumber-1);
           LastGroup = dvm_min(LastGroup, GroupNumber-1);

           if(InitGroup == LastGroup)
              dptr[InitGroup] += QTime->Time; /* Quant time
                                                 -> into group */    /*E0572*/
           else
           {  /* First and last groups are the same */    /*E0573*/

              /*    Time corresponding to one "coordinate" of AM
                 representation and time corresponding to one group */    /*E0574*/

              CoordTime = QTime->Time /
                          (LastAMIndex - InitAMIndex + 1.);
              GroupTime = CoordTime * GroupSize;

              /* Supplement to every internal group
                  time corresponding to one group   */    /*E0575*/

              for(k=InitGroup+1; k < LastGroup; k++)
                  dptr[k] += GroupTime;

              /* Supplements to the first ans last groups */    /*E0576*/

              InitCoord = InitGroup * GroupSize; /* coordinate of the
                                                    first group */    /*E0577*/
              LastCoord = LastGroup * GroupSize; /* last group
                                                    coordinate */    /*E0578*/
              dptr[InitGroup] +=
              CoordTime * (GroupSize - InitAMIndex + InitCoord);
              dptr[LastGroup] +=
              CoordTime * (LastAMIndex - LastCoord);
           }
        }
     }
  }

  /* --------------------------------------------------------- */    /*E0579*/

  coll_Free(gEnvColl, Env);
  Env = genv_GetEnvironment(gEnvColl->Count-2);

  if(Env)
  {  OldEnvProcCount = Env->EnvProcCount;
     OldAMHandlePtr  = Env->AMHandlePtr;
  }

  if(RTL_TRACE)
     dvm_trace(ret_endpl_," \n");

  DVMFTimeFinish(ret_endpl_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------ */    /*E0580*/


void  shd_iter(s_PARLOOP  *PL)
{
    DvmType          Lower, Step, Lower1, Lower2;
   int           i, Dim;

   Dim = PL->CurrBlock >> 1;

   /* Internal domain */    /*E0581*/

   for(i=0; i < Dim; i++)
   { Step = PL->Local[i].Step;
     Lower = PL->Local[i].Lower;

     *(PL->MapList[i].InitIndexPtr) =
         Lower + Step*(DvmType)ceil((double)PL->HighShdWidth[i] / (double)Step);
     *(PL->MapList[i].LastIndexPtr) =
         Lower + Step*(DvmType)ceil((double)(PL->Local[i].Upper - Lower + 1 -
                              PL->LowShdWidth[i])/(double)Step ) - Step;
   }

   /* Internal domain + exported domain */    /*E0582*/

   for(i=Dim+1; i < PL->Rank; i++)
   {  *(PL->MapList[i].InitIndexPtr) = PL->Local[i].Lower;
      *(PL->MapList[i].LastIndexPtr) = PL->Local[i].Upper;
   }

   /* Exported domain */    /*E0583*/

   if(PL->CurrBlock & 0x1)
   {  /* High export */    /*E0584*/

      Lower = PL->Local[Dim].Lower;
      Step = PL->Local[Dim].Step;
      Lower1 = Lower + Step * (DvmType)ceil((double)PL->HighShdWidth[Dim] /
                                         (double)Step);
      Lower2 = Lower + Step * (DvmType)ceil((double)(PL->Local[Dim].Upper -
                                          Lower+1-PL->LowShdWidth[Dim])/
                                          (double)Step );
      *(PL->MapList[Dim].InitIndexPtr) = dvm_max(Lower1, Lower2);
      *(PL->MapList[Dim].LastIndexPtr) = PL->Local[Dim].Upper;
   }
   else
   { /* Low export */    /*E0585*/

     Lower = PL->Local[Dim].Lower;
     Step = PL->Local[Dim].Step;

     *(PL->MapList[Dim].InitIndexPtr) = Lower;
     *(PL->MapList[Dim].LastIndexPtr) = Lower +
         Step*(DvmType)ceil((double)PL->HighShdWidth[Dim] / (double)Step) - Step;
   }

   PL->CurrBlock++;

   /* Correct loop parameters according to initial index values */    /*E0586*/

   for(i=0; i < PL->Rank; i++)
   {  *(PL->MapList[i].InitIndexPtr) += PL->InitIndex[i];
      *(PL->MapList[i].LastIndexPtr) += PL->InitIndex[i];
   }

   /* Correct loop parameters for it inverse execution */    /*E0587*/

   for(i=0; i < PL->Rank; i++)
   {  if(PL->Invers[i])
      { Step = *(PL->MapList[i].InitIndexPtr);
        *(PL->MapList[i].InitIndexPtr) = *(PL->MapList[i].LastIndexPtr);
        *(PL->MapList[i].LastIndexPtr) = Step;
      }
   }

   return;
}



void  in_iter(s_PARLOOP  *PL)
{  int           i;
   DvmType          Lower, Step;

   for(i=0; i < PL->Rank; i++)
   { Step = PL->Local[i].Step;
     Lower = PL->Local[i].Lower;

     *(PL->MapList[i].InitIndexPtr) =
         Lower + Step*(DvmType)ceil((double)PL->HighShdWidth[i] / (double)Step);
     *(PL->MapList[i].LastIndexPtr) =
         Lower + Step*(DvmType)ceil((double)(PL->Local[i].Upper - Lower + 1 -
                              PL->LowShdWidth[i])/(double)Step ) - Step;
   }

   PL->IsInIter = 1;

   /* Correct loop parameters according to initial index values */    /*E0588*/

   for(i=0; i < PL->Rank; i++)
   {  *(PL->MapList[i].InitIndexPtr) += PL->InitIndex[i];
      *(PL->MapList[i].LastIndexPtr) += PL->InitIndex[i];
   }

   /* Correct loop parameters for it inverse execution */    /*E0589*/

   for(i=0; i < PL->Rank; i++)
   {  if(PL->Invers[i])
      { Step = *(PL->MapList[i].InitIndexPtr);
        *(PL->MapList[i].InitIndexPtr) = *(PL->MapList[i].LastIndexPtr);
        *(PL->MapList[i].LastIndexPtr) = Step;
      }
   }

   return;
}



void parloop_Done(s_PARLOOP *PL)
{
  if(RTL_TRACE)
     dvm_trace(call_parloop_Done,"LoopRef=%lx;\n",
                                 (uLLng)PL->HandlePtr);

  if(PL == MeasurePL)
     MeasurePL = NULL; /* loop, for which iteration time execution
                          is to be measured, finished */    /*E0590*/

  /*   Free structures with information of
     completed loop iteration group execution */    /*E0591*/

  if(PL->PLQ)
  {  dvm_FreeArray(PL->PLQ->QTime);
     dvm_FreeStruct(PL->PLQ);
  }

  dvm_FreeArray(PL->MapList);
  dvm_FreeArray(PL->AlnLoc);
  dvm_FreeArray(PL->Local);
  dvm_FreeArray(PL->Set);
  dvm_FreeArray(PL->Align);

  /* Free own Handle */    /*E0592*/

  PL->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(PL->HandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_parloop_Done," \n");

  (DVM_RET);

  return;
}


/* For dynamical control */    /*E0593*/

/*****************************************************\
*   Function to calculate set of DA array elements    *
*   corresponding to current iteration of PL loop.    *
*                                                     *
* Return value : = 0 - DABlock is calculated;         *
*                > 0 - current loop iteration has no  *
*                      corresponding array element.   *
*  Return value > 0 are defined in system.def:        *
*                                                     *
*  AMViewEqu_Err         =  1;                        *
*  HasLocal_Err          =  2;                        *
*  align_NORMTAXIS_Err   =  3;                        *
*  distrib_NORMTAXIS_Err =  4.                        *
\*****************************************************/    /*E0594*/

byte  dyn_GetLocalBlock(s_BLOCK *DABlock, s_DISARRAY *DA, s_PARLOOP *PL)
{   int       i, j, k, AMRank, PLRank, DARank, DAAxis, CollapseRank;
    DvmType      PLVector[MAXARRAYDIM];
    DvmType      Coord;
    s_ALIGN  *PLAlign, *DAAlign;
    byte      RC = 0, Tr;

    Tr = (byte)(RTL_TRACE && dyn_GetLocalBlock_Trace &&
                TstTraceEvent(call_dyn_GetLocalBlock));

    if(Tr)
       dvm_trace(call_dyn_GetLocalBlock,
                 "ArrayHandlePtr=%lx; LoopRef=%lx;\n",
                 (uLLng)DA->HandlePtr, (uLLng)PL->HandlePtr);

    if(PL->HasLocal && DA->HasLocal)
    {  *DABlock = block_Copy(&DA->Block);
       DARank = DA->Space.Rank;

       for(i=0,CollapseRank=0; i < DA->AMView->Space.Rank; i++)
       {  if(DA->AMView->DISTMAP[i].Attr == map_COLLAPSE)
             CollapseRank++;
       }

       if(CollapseRank != DA->AMView->Space.Rank)
       {  if(PL->AMView != DA->AMView)
          {  RC = AMViewEqu_Err; /* representations of loop and
                                    array are not equivalent */    /*E0595*/
          }
          else
          {  AMRank = PL->AMView->Space.Rank;
             PLRank = PL->Rank;

             PLAlign = PL->Align;
             DAAlign = DA->Align;

             for(i=0; i < PLRank; i++)
             {  switch (PL->MapList[i].LoopVarType)
                {  case 0 :
                        PLVector[i] =
                        PT_LONG(PL->MapList[i].LoopVarAddr) -
                        PL->InitIndex[i];
                        break;
                   case 1 :
                        PLVector[i] =
                        PT_INT(PL->MapList[i].LoopVarAddr) -
                        PL->InitIndex[i];
                        break;
                   case 2 :
                        PLVector[i] =
                        PT_SHORT(PL->MapList[i].LoopVarAddr) -
                        PL->InitIndex[i];
                        break;
                   case 3 :
                        PLVector[i] =
                        PT_CHAR(PL->MapList[i].LoopVarAddr) -
                        PL->InitIndex[i];
                        break;
                 }
             }

             if(Tr)
             {  for(i=0; i < PL->Rank; i++)
                {  tprintf("i%d=%ld; ",
                           i, PLVector[i]+PL->InitIndex[i]);
                }
                tprintf(" \n");
             }

             for(i=0,j=PLRank,k=DARank; i < AMRank; i++,j++,k++)
             {  if(PLAlign[j].Attr == align_NORMTAXIS &&
                   DAAlign[k].Attr == align_NORMTAXIS &&
                   DA->AMView->DISTMAP[i].Attr != map_COLLAPSE)
                {  Coord = PLVector[PLAlign[j].Axis-1] * PLAlign[j].A
                           + PLAlign[j].B - DAAlign[k].B;

                   if(Coord % DAAlign[k].A)
                   {  RC = align_NORMTAXIS_Err; /* loop element
                                                   has not got
                                                   in array element */    /*E0596*/
                      break;
                   }

                   Coord /= DAAlign[k].A;
                   DAAxis = DAAlign[k].Axis - 1;

                   if(Coord < DABlock->Set[DAAxis].Lower ||
                      Coord > DABlock->Set[DAAxis].Upper)
                   {  RC = distrib_NORMTAXIS_Err; /* array element
                                                     has not got in
                                                     local part */    /*E0597*/
                      break;
                   }

                   DABlock->Set[DAAxis] = rset_Build(Coord, Coord, 1);
                }
             }
          }
       }
    }
    else
       RC = HasLocal_Err; /* loop or array is not mapped
                             or has no local part */    /*E0598*/

    if(Tr)
    {  if(RC == 0)
       {  for(i=0; i < DARank; i++)
              tprintf("da%d: l=%ld; u=%ld;  ",
                      i, DABlock->Set[i].Lower, DABlock->Set[i].Upper);
          tprintf(" \n");
       }

       dvm_trace(ret_dyn_GetLocalBlock,"RC=%d\n", (int)RC);
    }

    return  (DVM_RET, RC);
}


/* -------------------------------------------------------------- */    /*E0599*/


/********************************************************************\
* FuncTion returns number of processor system dimension on which     *
* the dimension of prescribed parallel loop is mapped on.  If        *
* the dimension of the parallel loop is multiplicated then returns 0 *
\********************************************************************/    /*E0600*/

int  GetPLMapDim(s_PARLOOP *PL, int PLDim)
{ int          Res = 0;
  s_ALIGN     *Align;
  s_MAP       *Map;
  s_AMVIEW    *AMV;
  int          AMSDim;

  Align = &PL->Align[PLDim-1];

  if(Align->Attr == align_NORMAL)
  {  AMSDim = Align->TAxis;
     AMV    = PL->AMView;
     Map    = &AMV->DISTMAP[AMSDim-1];

     if(Map->Attr == map_BLOCK)
        Res = Map->PAxis;
  }

  return  Res;
}


/*************************************************\
* Calculate number of groups local part of        *
* loop quanted dimension is to be divided in      *
* for ACROSS scheme pipeline execution.           *
* Number of groups is saved in PL->AcrossQNumber. *
\*************************************************/    /*E0601*/

void  GetAcrossQNumber(s_PARLOOP  *PL, DvmType  N, byte  *PLAxisArray,
                       int  *VMAxisArray, s_BOUNDGROUP  *BG)
{ s_VMS        *VMS;
  int           i, j, Rank, AxisM1;
  double        M, P, TcM, W = 0.0, TmW, TcM_TmW, PTmW, PTirecv_Twrecv,
                TcM_PTmW;
  SysHandle    *CurrHandlePtr;
  s_BOUNDBUF   *BB;
  s_SHADOW     *ShdInfo;
  DvmType          BSize, Nq;
  byte          Step = 0;
  s_SPACE      *Space;

  Nq = N/* * 10000*/    /*E0602*/;  /* */    /*E0603*/

  VMS   = PL->AMView->VMS;
  Space = &VMS->Space;
  Rank  = PL->Rank;
  BSize = 0;

  /* */    /*E0604*/

  for(i=0; i < Rank; i++)
  {  if(PLAxisArray[i] == 0)
        continue;  /* */    /*E0605*/

     if(VMAxisArray[i] == 0)
        continue;  /* */    /*E0606*/

     if(Space->Size[VMAxisArray[i]-1] > BSize)
     {  BSize = Space->Size[VMAxisArray[i]-1];
        AxisM1 = i;
     }
  }

  P = (double)BSize; /* size of processor system dimension
                        on which pipelined parallel loop dimension
                        is block distributed */    /*E0607*/

  M = ceil( (double)(PL->Set[AxisM1].Size) /
            (double)(PL->Set[AxisM1].Step)   ); /* size of pipelined
                                                   dimension */    /*E0608*/

  /* */    /*E0609*/

  for(i=0; i < Rank; i++)
  {  if(i == AxisM1)
        continue;     /* */    /*E0610*/

     for(j=0; j < PL->QDimNumber; j++)
         if(i == PL->QDim[j])
            break;

     if(j != PL->QDimNumber)
        continue;  /* */    /*E0611*/

     M *= ceil( (double)PL->Set[i].Size /
                (double)PL->Set[i].Step   );

     if(PL->Align[i].Attr != align_NORMAL)
        continue;   /* */    /*E0612*/

     if(VMAxisArray[i] == 0)
        continue;   /* */    /*E0613*/

     M /= (double)(Space->Size[VMAxisArray[i]-1]);
  }

  /* Number of points in edge group not including quanted dimension */    /*E0614*/

  CurrHandlePtr = BG->BufPtr;

  while(CurrHandlePtr)
  {  BB = (s_BOUNDBUF *)CurrHandlePtr->pP;
     ShdInfo = BB->ShdInfo;

     if(ShdInfo->IsShdIn)
     {  block_GetSize(BSize, &ShdInfo->BlockIn, Step)
        W += (double)(BSize * BB->Count);
     }
     else
     {  block_GetSize(BSize, &ShdInfo->BlockOut, Step)
        W += (double)(BSize * BB->Count);
     }
     /* Assumed that input and output group have the same size */

     CurrHandlePtr = (SysHandle *)CurrHandlePtr->NextHandlePtr;
  }

  W /= (double)Nq;

  /* ----------------------------------------------------------------- */    /*E0615*/

  TcM = PL->Tc * M;
  TmW = Tm * W * (double)PL->TempDArr->TLen;
  TcM_TmW = TcM / TmW;
  PTmW = P * TmW;
  PTirecv_Twrecv = P*(Tirecv + Twrecv);
  TcM_PTmW = TcM + PTmW;

  if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
     tprintf("W=%lf  Tc=%lf  Nq=%ld  M=%lf  Tc*M=%lf  Tm*W*TLen=%lf\n"
             "(Tc*M)/(Tm*W*Tlen)=%lf  P*Tm*W*TLen=%lf\n"
             "P*(Tirecv+Twrecv)=%lf  Tc*M+P*Tm*W*TLen=%lf\n",
             W, PL->Tc, Nq, M, TcM, TmW,
             TcM_TmW, PTmW,
             PTirecv_Twrecv, TcM_PTmW);

  if(P > TcM_TmW)
  {  /* Not fully loaded pipeline */    /*E0616*/

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
        tprintf("*** UnderLoad PipeLine\n");

     PL->AcrossQNumber = (int)
     ceil(sqrt((((double)Nq)*(P*TcM_PTmW - TcM - TcM)) /
               PTirecv_Twrecv));
  }
  else
  {  /* Overloaded or balanced pipeline */    /*E0617*/

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
        tprintf("*** OverLoad PipeLine\n");

     PL->AcrossQNumber = (int)
     ceil(sqrt((((double)Nq)*(P-1)*TcM_PTmW) / PTirecv_Twrecv));
  }

/*  PL->AcrossQNumber *= 23;*/    /*E0618*/

  if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_across_))
     tprintf("PL->AcrossQNumber=%d\n", PL->AcrossQNumber);

  return;
}


#endif   /*  _PARLOOP_C_  */    /*E0619*/
