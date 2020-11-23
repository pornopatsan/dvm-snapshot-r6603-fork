#ifndef _BOUNDS_C_
#define _BOUNDS_C_
/****************/    /*E0000*/

/***********************************************\
* Renewal of shadow edges of distributed arrays * 
\***********************************************/    /*E0001*/

ShadowGroupRef  __callstd crtshg_(DvmType  *StaticSignPtr)

/*
                Creating shadow edge group.
                ---------------------------

*StaticSignPtr - the flag of the static shadow edge group creation.

The function crtshg_ creates empty shadow edge group
(that is group, that does not contain any shadow edge).
The function returns pointer to the created group.
*/    /*E0002*/

{ s_BOUNDGROUP    *BG;
  SysHandle       *GroupHandlePtr;
  ShadowGroupRef   Res;
  int              i, j; 
 
  if(ShgSave)
  {  /* */    /*E0003*/

     /* */    /*E0004*/

     for(i=0; i < ShgCount; i++)
     {  if(DVM_LINE[DVM_LEVEL] != ShgLine[i])
           continue;
        SYSTEM_RET(j, strcmp, (DVM_FILE[DVM_LEVEL], ShgFile[i]))
        if(j == 0)
           break;
     }

     if(i < ShgCount)
     {  Res = (ShadowGroupRef)ShgRef[i];
        BG = (s_BOUNDGROUP *)((SysHandle *)Res)->pP;
        BG->SaveSign = 1;  /* */    /*E0005*/
        return  (DVM_RET, Res);
     }
  }

  /* */    /*E0006*/

  DVMFTimeStart(call_crtshg_);

  if(RTL_TRACE)
     dvm_trace(call_crtshg_,"StaticSign=%ld;\n", *StaticSignPtr);

  dvm_AllocStruct(s_BOUNDGROUP, BG);
 
  /* Lists of all arrays included in the group */    /*E0007*/

  BG->ArrayColl       = coll_Init(BGArrCount, BGArrCount, NULL);
  BG->NewArrayColl    = coll_Init(BGArrCount, BGArrCount, NULL);
  BG->NewShdWidthColl = coll_Init(BGArrCount, BGArrCount, NULL);

  BG->BufPtr   = NULL; /* pointer to Handle of the first edge buffer */    /*E0008*/
  BG->Static   = (byte)*StaticSignPtr; /* static group */    /*E0009*/
  BG->IsStrt   = 0; /* edge exchange (any) not started */    /*E0010*/
  BG->IsStrtsh = 0; /* edge exchange not started */    /*E0011*/
  BG->IsRecvsh = 0; /* receiving imported elements not started */    /*E0012*/
  BG->IsSendsh = 0; /* sending exported elements not started */    /*E0013*/
  BG->IsRecvla = 0; /* local part element receiving is
                       not started */    /*E0014*/
  BG->IsSendsa = 0; /* edge element sending is 
                       not started */    /*E0015*/
  BG->ShdAMHandlePtr = NULL; /* pointer to Handle of AM which starts 
                                edge exchange operation */    /*E0016*/
  BG->ShdEnvInd      = 0;    /* index of context which starts 
                                edge exchange operation*/    /*E0017*/
  BG->ResetSign      = 0;    /* */    /*E0018*/

  dvm_AllocStruct(SysHandle, GroupHandlePtr);

  *GroupHandlePtr = genv_InsertObject(sht_BoundsGroup, BG);
  BG->HandlePtr = GroupHandlePtr; /* pointer to own Handle */    /*E0019*/

  if(TstObject)
     InsDVMObj((ObjectRef)GroupHandlePtr);

  Res = (ShadowGroupRef)GroupHandlePtr;

  BG->SaveSign = 0;  /* */    /*E0020*/

  if(ShgSave)
  {  /* */    /*E0021*/

     if(ShgCount < (ShgNumber-1))
     {  /* */    /*E0022*/

        ShgLine[ShgCount] = DVM_LINE[DVM_LEVEL];
        ShgFile[ShgCount] = DVM_FILE[DVM_LEVEL];
        ShgRef[ShgCount]  = (uLLng)Res;
        ShgCount++;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtshg_,"ShadowGroupRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0023*/
  DVMFTimeFinish(ret_crtshg_);
  return  (DVM_RET, Res);
}



DvmType __callstd inssh_(ShadowGroupRef *ShadowGroupRefPtr,
                        DvmType ArrayHeader[], DvmType LowShdWidthArray[],
                        DvmType HiShdWidthArray[], DvmType *FullShdSignPtr)

/*
     Including shadow edge in the group.
     -----------------------------------

*ShadowGroupRefPtr - reference to the shadow edge group.
ArrayHeader        - the header of the distributed array.
LowShdWidthArray   - LowShdWidthArray[i] is the width of the low
                     shadow edge of the (i+1)th dimension of the array.
HiShdWidthArray    - HiShdWidthArray[i] is the width of the high
                     shadow edge of the (i+1)th dimension of the array.
*FullShdSignPtr	   - the flag of the inclusion of the all elements
                     to the shadow edge.

Including distributed array shadow edge in the group means only
registration of this shadow edge as a member of shadow edge group.
The function returns zero.
*/    /*E0024*/

{ SysHandle     *BoundHandlePtr, *ArrayHandlePtr;
  s_DISARRAY    *ArrayDescPtr;
  int            i, j, Rank, temp, CurrEnvInd;
  s_AMVIEW      *AMV;
  void          *CurrAM;
  s_BOUNDGROUP  *BGPtr, *BG;
  s_SHDWIDTH    *ShdWidth;

  BoundHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.000: wrong call inssh_\n"
                 "(the shadow group is not a DVM object; "
                 "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BoundHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.001: wrong call inssh_\n"
            "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
            *ShadowGroupRefPtr);

  BGPtr = (s_BOUNDGROUP *)BoundHandlePtr->pP;

  if(BGPtr->SaveSign)
     return  (DVM_RET, 0);  /* */    /*E0025*/

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0026*/
  DVMFTimeStart(call_inssh_);

  if(RTL_TRACE)
     dvm_trace(call_inssh_,
             "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx; "
             "ArrayHeader=%lx; ArrayHandlePtr=%lx; FullShdSign=%ld;\n",
             (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr,
             (uLLng)ArrayHeader, ArrayHeader[0], *FullShdSignPtr);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.002: wrong call inssh_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0027*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0028*/

  if(BoundHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 080.003: wrong call inssh_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BoundHandlePtr->CrtEnvInd, CurrEnvInd);
         
  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV = ArrayDescPtr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.005: wrong call inssh_ "
              "(the array has not been aligned;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 080.006: wrong call inssh_\n"
             "(the array PS is not a subsystem of the current PS;\n"
             "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
             ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);
  
  /* Check if the edge group started */    /*E0029*/
                                  
  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.007: wrong call inssh_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.008: wrong call inssh_\n"
              "(the import receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.009: wrong call inssh_\n"
              "(the export sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.010: wrong call inssh_\n"
              "(the local receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.011: wrong call inssh_\n"
              "(the boundary sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  BGPtr->IsStrt = 0;  /* */    /*E0030*/

  Rank = ArrayDescPtr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_inssh_))
     {  for(i=0; i < Rank; i++)
            tprintf("LowShdWidthArray[%d]=%ld; ",i,LowShdWidthArray[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
           tprintf(" HiShdWidthArray[%d]=%ld; ",i,HiShdWidthArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  CurrEnvInd = ArrayDescPtr->BG.Count; /* */    /*E0031*/

  for(j=0; j < CurrEnvInd; j++)
  {  BG = coll_At(s_BOUNDGROUP *, &ArrayDescPtr->BG, j); /* current
                                                            group */    /*E0032*/
     if(BG != BGPtr)
        continue; /* the current group is not the group
                     the array to be included in */    /*E0033*/

     ShdWidth = coll_At(s_SHDWIDTH *, &ArrayDescPtr->ResShdWidthColl, j);

     for(i=0; i < Rank; i++)
     {  if(LowShdWidthArray[i] == -1)
           temp = (ShdWidth->ResLowShdWidth[i] ==
                   ArrayDescPtr->InitLowShdWidth[i]);
        else
           temp = (ShdWidth->ResLowShdWidth[i] == LowShdWidthArray[i]);

        if(temp == 0)
           break;

        if(HiShdWidthArray[i] == -1)
           temp = (ShdWidth->ResHighShdWidth[i] ==
                   ArrayDescPtr->InitHighShdWidth[i]);
        else
           temp = (ShdWidth->ResHighShdWidth[i] == HiShdWidthArray[i]);

        if(temp == 0)
           break;

        if(ShdWidth->ShdSign[i] != 7)
           break;

        if(ShdWidth->InitDimIndex[i] != 0)
           break;

        if(ShdWidth->DimWidth[i] != ArrayDescPtr->Space.Size[i])
           break;

        if(ShdWidth->InitLowShdIndex[i] !=
           (ArrayDescPtr->InitLowShdWidth[i] -
            ShdWidth->ResLowShdWidth[i]))
           break;          

        if(ShdWidth->InitHiShdIndex[i] != 0)
           break;
     }

     if(i != Rank)
        continue;

     if(*FullShdSignPtr)
        i = Rank;
     else
        i = 1;

     if(i != ShdWidth->MaxShdCount)
        continue;

     break;
  }

  if(j == CurrEnvInd)
  {  /* Array was not included in this group */    /*E0034*/

     dvm_AllocStruct(s_SHDWIDTH, ShdWidth);
     ShdWidth->UseSign = 0;  /* */    /*E0035*/

     if(*FullShdSignPtr)
        ShdWidth->MaxShdCount = Rank;
     else
        ShdWidth->MaxShdCount = 1;

     for(i=0; i < Rank; i++)
     {  /* Standard values to narrowing 
           elementary shadow parallelepipeds */    /*E0036*/

        ShdWidth->InitDimIndex[i]    = 0;
        ShdWidth->DimWidth[i]        = ArrayDescPtr->Space.Size[i];
        ShdWidth->InitHiShdIndex[i]  = 0;

        /* ------------------------------------- */    /*E0037*/ 

        if(LowShdWidthArray[i] < -1 ||
           LowShdWidthArray[i] > ArrayDescPtr->InitLowShdWidth[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.014: wrong call inssh_\n"
                    "(invalid LowShdWidthArray[%d]=%ld)\n",
                    i, LowShdWidthArray[i]);
        if(LowShdWidthArray[i] == -1)
           ShdWidth->ResLowShdWidth[i] =
           ArrayDescPtr->InitLowShdWidth[i];
        else
           ShdWidth->ResLowShdWidth[i] = (int)LowShdWidthArray[i];

        ShdWidth->InitLowShdIndex[i] = ArrayDescPtr->InitLowShdWidth[i] -
                                       ShdWidth->ResLowShdWidth[i];

        if(HiShdWidthArray[i] < -1 ||
           HiShdWidthArray[i] > ArrayDescPtr->InitHighShdWidth[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.015: wrong call inssh_\n"
                    "(invalid HiShdWidthArray[%d]=%ld)\n",
                    i, HiShdWidthArray[i]);
        if(HiShdWidthArray[i] == -1)
           ShdWidth->ResHighShdWidth[i] =
           ArrayDescPtr->InitHighShdWidth[i];
        else
           ShdWidth->ResHighShdWidth[i] = (int)HiShdWidthArray[i];

        ShdWidth->ShdSign[i] = 7;
     }

     coll_Insert(&ArrayDescPtr->BG, BGPtr); /* to the list of edge groups
                                               in which the array was included */    /*E0038*/

     /* To the list of resulting edge widths */    /*E0039*/

     coll_Insert(&ArrayDescPtr->ResShdWidthColl, ShdWidth);

     /* To the lists of all arrays included in the group */    /*E0040*/

     coll_Insert(&BGPtr->NewArrayColl, ArrayDescPtr);
     coll_Insert(&BGPtr->NewShdWidthColl, ShdWidth);
     coll_Insert(&BGPtr->ArrayColl, ArrayDescPtr);

     if(EnableDynControl)
        dyn_DisArrDefineShadow(ArrayDescPtr, BGPtr, ShdWidth);
  }
 
  if(RTL_TRACE)
     dvm_trace(ret_inssh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0041*/
  DVMFTimeFinish(ret_inssh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd insshd_(ShadowGroupRef  *ShadowGroupRefPtr,
                           DvmType  ArrayHeader[],DvmType  LowShdWidthArray[],
                           DvmType  HiShdWidthArray[], DvmType  *MaxShdCountPtr,
                           DvmType  ShdSignArray[])

/*
      Support system allows to consider any set of primitive shadow
      parallelepipeds as a set for edge exchange using below function
      insshd_ ,that includes edges of distributed array into edge group.

*ShadowGroupRefPtr - reference to an edge group.
ArrayHeader        - header of a distributed array.
LowShdWidthArray   - array: i - element is a width of  low edge 
                                for (i+1)- array dimension.
HiShdWidthArray    - array: i - element is a width of  high  edge 
                                for (i+1)- array dimension.
*MaxShdCountPtr    - max number of array dimentions, which can form
                     edges (positive integer) .
ShdSignArray       - array: i - element contains sign of  (i+1)- array
                                dimension possibility to form edges.  
The function returns zero.
*/    /*E0042*/

{ SysHandle     *BoundHandlePtr, *ArrayHandlePtr;
  s_DISARRAY    *ArrayDescPtr;
  int            i, j, Rank, temp, CurrEnvInd;
  byte           ZeroSign;
  s_AMVIEW      *AMV;
  void          *CurrAM; 
  s_BOUNDGROUP  *BGPtr, *BG;
  s_SHDWIDTH    *ShdWidth;

  BoundHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.020: wrong call insshd_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BoundHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.021: wrong call insshd_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BoundHandlePtr->pP;

  if(BGPtr->SaveSign)
     return  (DVM_RET, 0);  /* */    /*E0043*/

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0044*/
  DVMFTimeStart(call_insshd_);

  if(RTL_TRACE)
     dvm_trace(call_insshd_,
             "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx; "
             "ArrayHeader=%lx; ArrayHandlePtr=%lx; MaxShdCount=%ld;\n",
             (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr,
             (uLLng)ArrayHeader, ArrayHeader[0], *MaxShdCountPtr);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.022: wrong call insshd_\n"
            "(the object is not a distributed array; "
            "ArrayHeader[0]=%lx)\n",
            ArrayHeader[0]);

  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0045*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0046*/

  if(BoundHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 080.023: wrong call insshd_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BoundHandlePtr->CrtEnvInd, CurrEnvInd);
         
  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV = ArrayDescPtr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.024: wrong call insshd_ "
              "(the array has not been aligned;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 080.025: wrong call insshd_\n"
             "(the array PS is not a subsystem of the current PS;\n"
             "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
             ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

  /* Check if the edge group started */    /*E0047*/
                                  
  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.026: wrong call insshd_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.027: wrong call insshd_\n"
              "(the import receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.028: wrong call insshd_\n"
              "(the export sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.029: wrong call insshd_\n"
              "(the local receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.030: wrong call insshd_\n"
              "(the boundary sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  BGPtr->IsStrt = 0;  /* */    /*E0048*/

  Rank = ArrayDescPtr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_insshd_))
     {  for(i=0; i < Rank; i++)
           tprintf("LowShdWidthArray[%d]=%ld; ",i,LowShdWidthArray[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
           tprintf(" HiShdWidthArray[%d]=%ld; ",i,HiShdWidthArray[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
           tprintf("    ShdSignArray[%d]=%ld; ",i,ShdSignArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  CurrEnvInd = ArrayDescPtr->BG.Count; /* */    /*E0049*/

  for(j=0; j < CurrEnvInd; j++)
  {  BG = coll_At(s_BOUNDGROUP *, &ArrayDescPtr->BG, j); /* current
                                                            group */    /*E0050*/
     if(BG != BGPtr)
        continue; /* the current group is not the group
                     the array to be included in */    /*E0051*/

     ShdWidth = coll_At(s_SHDWIDTH *, &ArrayDescPtr->ResShdWidthColl, j);

     for(i=0; i < Rank; i++)
     {  if(LowShdWidthArray[i] == -1)
           temp = (ShdWidth->ResLowShdWidth[i] ==
                   ArrayDescPtr->InitLowShdWidth[i]);
        else
           temp = (ShdWidth->ResLowShdWidth[i] == LowShdWidthArray[i]);

        if(temp == 0)
           break;

        if(HiShdWidthArray[i] == -1)
           temp = (ShdWidth->ResHighShdWidth[i] ==
                   ArrayDescPtr->InitHighShdWidth[i]);
        else
           temp = (ShdWidth->ResHighShdWidth[i] == HiShdWidthArray[i]);

        if(temp == 0)
           break;

        temp = (ShdWidth->ShdSign[i] == ShdSignArray[i]);

        if(temp == 0)
           break;

        if(ShdWidth->InitDimIndex[i] != 0)
           break;

        if(ShdWidth->DimWidth[i] != ArrayDescPtr->Space.Size[i])
           break;

        if(ShdWidth->InitLowShdIndex[i] !=
          (ArrayDescPtr->InitLowShdWidth[i] -
           ShdWidth->ResLowShdWidth[i]))
           break;

        if(ShdWidth->InitHiShdIndex[i] != 0)
           break;
     }

     if(i != Rank)
        continue;

     temp = (ShdWidth->MaxShdCount == *MaxShdCountPtr);

     if(temp == 0)
        continue;

     break;
  }

  if(j == CurrEnvInd)
  {  /* Array was not included in this group */    /*E0052*/

     dvm_AllocStruct(s_SHDWIDTH, ShdWidth);
     ShdWidth->UseSign = 0;  /* */    /*E0053*/

     ShdWidth->MaxShdCount = (byte)*MaxShdCountPtr;

     if(*MaxShdCountPtr < 1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.031: wrong call insshd_\n"
                 "(invalid MaxShdCount=%ld)\n", *MaxShdCountPtr);

     ZeroSign = 1;

     for(i=0; i < Rank; i++)
     {  /* Standard values for narrowing 
           elementary shadow parallelepipeds */    /*E0054*/

        ShdWidth->InitDimIndex[i]    = 0;
        ShdWidth->DimWidth[i]        = ArrayDescPtr->Space.Size[i];
        ShdWidth->InitHiShdIndex[i]  = 0;

        /* ------------------------------------- */    /*E0055*/

        if(LowShdWidthArray[i] < -1 ||
           LowShdWidthArray[i] > ArrayDescPtr->InitLowShdWidth[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.034: wrong call insshd_\n"
                    "(invalid LowShdWidthArray[%d]=%ld)\n",
                    i, LowShdWidthArray[i]);

        if(LowShdWidthArray[i] == -1)
           ShdWidth->ResLowShdWidth[i] =
           ArrayDescPtr->InitLowShdWidth[i];
        else
           ShdWidth->ResLowShdWidth[i] = (int)LowShdWidthArray[i];

        ShdWidth->InitLowShdIndex[i] = ArrayDescPtr->InitLowShdWidth[i] -
                                       ShdWidth->ResLowShdWidth[i];

        if(HiShdWidthArray[i] < -1 ||
           HiShdWidthArray[i] > ArrayDescPtr->InitHighShdWidth[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.035: wrong call insshd_\n"
                    "(invalid HiShdWidthArray[%d]=%ld)\n",
                    i, HiShdWidthArray[i]);

        if(HiShdWidthArray[i] == -1)
           ShdWidth->ResHighShdWidth[i] =
           ArrayDescPtr->InitHighShdWidth[i];
        else
           ShdWidth->ResHighShdWidth[i] = (int)HiShdWidthArray[i];

        ShdWidth->ShdSign[i] = (byte)ShdSignArray[i];

        if(ShdSignArray[i] < 1 || ShdSignArray[i] > 7)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.032: wrong call insshd_ "
                    "(invalid ShdSignArray[%d]=%ld)\n",
                    i, ShdSignArray[i]);

        if(ShdSignArray[i] > 1)
           ZeroSign = 0;
     }

     if(ZeroSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 080.033: wrong call insshd_ "
               "(the boundary of the distributed array is empty)\n");

     coll_Insert(&ArrayDescPtr->BG, BGPtr); /* to the list of edge groups
                                               in which the array was included */    /*E0056*/

     /* To the list of resulting edge widths */    /*E0057*/

     coll_Insert(&ArrayDescPtr->ResShdWidthColl, ShdWidth);

     /* To the lists of all arrays included in the group */    /*E0058*/

     coll_Insert(&BGPtr->NewArrayColl, ArrayDescPtr);
     coll_Insert(&BGPtr->NewShdWidthColl, ShdWidth);
     coll_Insert(&BGPtr->ArrayColl, ArrayDescPtr);

     if(EnableDynControl)
        dyn_DisArrDefineShadow(ArrayDescPtr, BGPtr, ShdWidth);
  }
 
  if(RTL_TRACE)
     dvm_trace(ret_insshd_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0059*/
  DVMFTimeFinish(ret_insshd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  incsh_(ShadowGroupRef  *ShadowGroupRefPtr,
                           DvmType ArrayHeader[],
                           DvmType InitDimIndex[], DvmType LastDimIndex[],
                           DvmType InitLowShdIndex[], DvmType LastLowShdIndex[],
                           DvmType InitHiShdIndex[], DvmType LastHiShdIndex[],
                           DvmType *FullShdSignPtr)
{ SysHandle     *BoundHandlePtr, *ArrayHandlePtr;
  s_DISARRAY    *ArrayDescPtr;
  int            i, j, Rank, temp, CurrEnvInd;
  s_AMVIEW      *AMV;
  void          *CurrAM;
  s_BOUNDGROUP  *BGPtr, *BG;
  s_SHDWIDTH    *ShdWidth;

  BoundHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.200: wrong call incsh_\n"
                 "(the shadow group is not a DVM object; "
                 "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BoundHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.201: wrong call incsh_\n"
            "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
            *ShadowGroupRefPtr);

  BGPtr = (s_BOUNDGROUP *)BoundHandlePtr->pP;

  if(BGPtr->SaveSign)
     return  (DVM_RET, 0);  /* */    /*E0060*/

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0061*/
  DVMFTimeStart(call_incsh_);

  if(RTL_TRACE)
     dvm_trace(call_incsh_,
             "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx; "
             "ArrayHeader=%lx; ArrayHandlePtr=%lx; FullShdSign=%ld;\n",
             (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr,
             (uLLng)ArrayHeader, ArrayHeader[0], *FullShdSignPtr);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.202: wrong call incsh_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              ArrayHeader[0]);

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0062*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0063*/

  if(BoundHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 080.203: wrong call incsh_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BoundHandlePtr->CrtEnvInd, CurrEnvInd);
         
  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV = ArrayDescPtr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.205: wrong call incsh_ "
              "(the array has not been aligned;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 080.206: wrong call incsh_\n"
             "(the array PS is not a subsystem of the current PS;\n"
             "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
             ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);
  
  /* Check if the edge group has been started */    /*E0064*/
                                  
  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.207: wrong call incsh_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.208: wrong call incsh_\n"
              "(the import receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.209: wrong call incsh_\n"
              "(the export sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.210: wrong call incsh_\n"
              "(the local receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.211: wrong call incsh_\n"
              "(the boundary sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  BGPtr->IsStrt = 0;  /* */    /*E0065*/

  Rank = ArrayDescPtr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_incsh_))
     {  for(i=0; i < Rank; i++)
            tprintf("   InitDimIndex[%d]=%ld; ",i,InitDimIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("   LastDimIndex[%d]=%ld; ",i,LastDimIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("InitLowShdIndex[%d]=%ld; ",i,InitLowShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("LastLowShdIndex[%d]=%ld; ",i,LastLowShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf(" InitHiShdIndex[%d]=%ld; ",i,InitHiShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf(" LastHiShdIndex[%d]=%ld; ",i,LastHiShdIndex[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  CurrEnvInd = ArrayDescPtr->BG.Count; /* */    /*E0066*/

  for(j=0; j < CurrEnvInd; j++)
  {  BG = coll_At(s_BOUNDGROUP *, &ArrayDescPtr->BG, j); /* current
                                                            group */    /*E0067*/
     if(BG != BGPtr)
        continue; /* the current group is not the group
                     the array to be included in */    /*E0068*/

     ShdWidth = coll_At(s_SHDWIDTH *, &ArrayDescPtr->ResShdWidthColl, j);

     for(i=0; i < Rank; i++)
     {  if(InitDimIndex[i] == -1)
        {  /* There is no global restriction on (i+1)-th array dimension */    /*E0069*/

           if(ShdWidth->InitDimIndex[i] != 0)
              break; 

           if(ShdWidth->DimWidth[i] != ArrayDescPtr->Space.Size[i])
              break;
        }
        else
        {  /* global restriction on (i+1)-th array dimension 
              is defined */    /*E0070*/

           if(InitDimIndex[i] != ShdWidth->InitDimIndex[i])
              break;
           if((LastDimIndex[i]-InitDimIndex[i]+1) !=
              ShdWidth->DimWidth[i])
              break;
        }

        if(InitLowShdIndex[i] == -1)
        {  /* width of the low edge of (i+1)- th dimension 
              is defined in LastLowShdIndex[i] */    /*E0071*/

           if(LastLowShdIndex[i] == -1)
              temp = (ShdWidth->ResLowShdWidth[i] ==
                      ArrayDescPtr->InitLowShdWidth[i]);
           else
              temp = (ShdWidth->ResLowShdWidth[i] == LastLowShdIndex[i]);

           if(temp == 0)
              break;

           if(ShdWidth->InitLowShdIndex[i] !=
              (ArrayDescPtr->InitLowShdWidth[i] -
               ShdWidth->ResLowShdWidth[i]))
               break;
        }
        else
        {  /* initial and final relative index values determining
              the low edge of (i+1)-th dimension are defined in
              InitLowShdIndex[i] and LastLowShdIndex[i] */    /*E0072*/

           if(InitLowShdIndex[i] != ShdWidth->InitLowShdIndex[i])
              break;

           if((LastLowShdIndex[i]-InitLowShdIndex[i]+1) !=
              ShdWidth->ResLowShdWidth[i])
              break; 
        }

        if(InitHiShdIndex[i] == -1)
        {  /* width of the high edge of (i+1)- th dimension 
              is defined in LastHiShdIndex[i] */    /*E0073*/

           if(ShdWidth->InitHiShdIndex[i] != 0)
              break;

           if(LastHiShdIndex[i] == -1)
              temp = (ShdWidth->ResHighShdWidth[i] ==
                      ArrayDescPtr->InitHighShdWidth[i]);
           else
              temp = (ShdWidth->ResHighShdWidth[i] == LastHiShdIndex[i]);

           if(temp == 0)
              break;
        }
        else
        {  /* initial and final relative index values determining
              the high edge of (i+1)-th dimension are defined in
              InitHiShdIndex[i] and LastHiShdIndex[i] */    /*E0074*/

           if(InitHiShdIndex[i] != ShdWidth->InitHiShdIndex[i])
              break;

           if((LastHiShdIndex[i]-InitHiShdIndex[i]+1) !=
              ShdWidth->ResHighShdWidth[i])
              break;
        }
     }

     if(i != Rank)
        continue;

     if(*FullShdSignPtr)
        i = Rank;
     else
        i = 1;

     if(i != ShdWidth->MaxShdCount)
        continue;

     break;
  }

  if(j == CurrEnvInd)
  {  /* Array has not been included in the group */    /*E0075*/

     dvm_AllocStruct(s_SHDWIDTH, ShdWidth);
     ShdWidth->UseSign = 0;  /* */    /*E0076*/

     if(*FullShdSignPtr)
        ShdWidth->MaxShdCount = Rank;
     else
        ShdWidth->MaxShdCount = 1;

     for(i=0; i < Rank; i++)
     {  /* processing InitDimIndex and LastDimIndex arrays */    /*E0077*/

        if(InitDimIndex[i] == -1)
        {  /* There is no global restriction on (i+1)-th array dimension */    /*E0078*/

           ShdWidth->InitDimIndex[i] = 0;
           ShdWidth->DimWidth[i]     = ArrayDescPtr->Space.Size[i];
        }
        else
        {  /* global restriction on (i+1)-th array dimension 
              is defined */    /*E0079*/

           if(InitDimIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.214: wrong call incsh_\n"
                       "(invalid InitDimIndex[%d]=%ld)\n",
                       i, InitDimIndex[i]);

           if(InitDimIndex[i] > LastDimIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.215: wrong call incsh_\n"
                    "(InitDimIndex[%d]=%ld > LastDimIndex[%d]=%ld)\n",
                    i, InitDimIndex[i], i, LastDimIndex[i]);

           if(LastDimIndex[i] >= ArrayDescPtr->Space.Size[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.216: wrong call incsh_\n"
                       "(invalid LastDimIndex[%d]=%ld >= %ld)\n",
                       i, LastDimIndex[i], ArrayDescPtr->Space.Size[i]);

           ShdWidth->InitDimIndex[i] = InitDimIndex[i];
           ShdWidth->DimWidth[i] = LastDimIndex[i]-InitDimIndex[i]+1;
        }

        /* processing InitLowShdIndex and LastLowShdIndex arrays*/    /*E0080*/

        if(InitLowShdIndex[i] == -1)
        {  /* width of the low edge of (i+1)- th dimension 
              is defined in LastLowShdIndex[i] */    /*E0081*/

           if(LastLowShdIndex[i] < -1 ||
              LastLowShdIndex[i] > ArrayDescPtr->InitLowShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.220: wrong call incsh_\n"
                       "(invalid LastLowShdIndex[%d]=%ld)\n",
                       i, LastLowShdIndex[i]);

           if(LastLowShdIndex[i] == -1)
              ShdWidth->ResLowShdWidth[i] =
              ArrayDescPtr->InitLowShdWidth[i];
           else
              ShdWidth->ResLowShdWidth[i] = (int)LastLowShdIndex[i];

           ShdWidth->InitLowShdIndex[i] =
           ArrayDescPtr->InitLowShdWidth[i] -
           ShdWidth->ResLowShdWidth[i];
        }
        else
        {  /* initial and final relative index values determining
              the low edge of (i+1)-th dimension are defined in
              InitLowShdIndex[i] and LastLowShdIndex[i] */    /*E0082*/

           if(InitLowShdIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.225: wrong call incsh_\n"
                       "(invalid InitLowShdIndex[%d]=%ld)\n",
                       i, InitLowShdIndex[i]);

           if(InitLowShdIndex[i] > LastLowShdIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.226: wrong call incsh_\n"
                 "(InitLowShdIndex[%d]=%ld > LastLowShdIndex[%d]=%ld)\n",
                 i, InitLowShdIndex[i], i, LastLowShdIndex[i]);

           if(LastLowShdIndex[i] >= ArrayDescPtr->InitLowShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 080.227: wrong call incsh_\n"
                   "(invalid LastLowShdIndex[%d]=%ld >= %d)\n",
                   i, LastLowShdIndex[i],
                   ArrayDescPtr->InitLowShdWidth[i]);

           ShdWidth->InitLowShdIndex[i] = (int)InitLowShdIndex[i];
           ShdWidth->ResLowShdWidth[i]  = (int)(LastLowShdIndex[i] -
                                                InitLowShdIndex[i] + 1);
        }

        /* processing InitHiShdIndex and LastHiShdIndex arrays*/    /*E0083*/

        if(InitHiShdIndex[i] == -1)
        {  /* width of the high edge of (i+1)- th dimension 
              is defined in LastHiShdIndex[i] */    /*E0084*/

           ShdWidth->InitHiShdIndex[i] = 0;

           if(LastHiShdIndex[i] < -1 ||
              LastHiShdIndex[i] > ArrayDescPtr->InitHighShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.230: wrong call incsh_\n"
                       "(invalid LastHiShdIndex[%d]=%ld)\n",
                       i, LastHiShdIndex[i]);

           if(LastHiShdIndex[i] == -1)
              ShdWidth->ResHighShdWidth[i] =
              ArrayDescPtr->InitHighShdWidth[i];
           else
              ShdWidth->ResHighShdWidth[i] = (int)LastHiShdIndex[i];
        }
        else
        {  /* initial and final relative index values determining
              the high edge of (i+1)-th dimension are defined in
              InitHiShdIndex[i] and LastHiShdIndex[i] */    /*E0085*/

           if(InitHiShdIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.235: wrong call incsh_\n"
                       "(invalid InitHiShdIndex[%d]=%ld)\n",
                       i, InitHiShdIndex[i]);

           if(InitHiShdIndex[i] > LastHiShdIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.236: wrong call incsh_\n"
                 "(InitHiShdIndex[%d]=%ld > LastHiShdIndex[%d]=%ld)\n",
                 i, InitHiShdIndex[i], i, LastHiShdIndex[i]);

           if(LastHiShdIndex[i] >= ArrayDescPtr->InitHighShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 080.237: wrong call incsh_\n"
                   "(invalid LastHiShdIndex[%d]=%ld >= %d)\n",
                   i, LastHiShdIndex[i],
                   ArrayDescPtr->InitHighShdWidth[i]);

           ShdWidth->InitHiShdIndex[i] = (int)InitHiShdIndex[i];
           ShdWidth->ResHighShdWidth[i] = (int)(LastHiShdIndex[i] -
                                                InitHiShdIndex[i] + 1);
        }

        ShdWidth->ShdSign[i] = 7;
     }

     coll_Insert(&ArrayDescPtr->BG, BGPtr); /* to list of edge groups 
                                               the array is included in */    /*E0086*/

     /* To the list of result edge wigths */    /*E0087*/

     coll_Insert(&ArrayDescPtr->ResShdWidthColl, ShdWidth);

     /* To the lists of all and new arrays included in the group */    /*E0088*/

     coll_Insert(&BGPtr->NewArrayColl, ArrayDescPtr);
     coll_Insert(&BGPtr->NewShdWidthColl, ShdWidth);
     coll_Insert(&BGPtr->ArrayColl, ArrayDescPtr);

     if(EnableDynControl)
        dyn_DisArrDefineShadow(ArrayDescPtr, BGPtr, ShdWidth);
  }
 
  if(RTL_TRACE)
     dvm_trace(ret_incsh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0089*/
  DVMFTimeFinish(ret_incsh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  incshd_(ShadowGroupRef  *ShadowGroupRefPtr,
                            DvmType ArrayHeader[],
                            DvmType InitDimIndex[], DvmType LastDimIndex[],
                            DvmType InitLowShdIndex[], DvmType LastLowShdIndex[],
                            DvmType InitHiShdIndex[], DvmType LastHiShdIndex[],
                            DvmType *MaxShdCountPtr, DvmType ShdSignArray[])
{ SysHandle     *BoundHandlePtr, *ArrayHandlePtr;
  s_DISARRAY    *ArrayDescPtr;
  int            i, j, Rank, temp, CurrEnvInd;
  byte           ZeroSign;
  s_AMVIEW      *AMV;
  void          *CurrAM; 
  s_BOUNDGROUP  *BGPtr, *BG;
  s_SHDWIDTH    *ShdWidth;

  BoundHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.250: wrong call incshd_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BoundHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.251: wrong call incshd_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BoundHandlePtr->pP;

  if(BGPtr->SaveSign)
     return  (DVM_RET, 0);  /* */    /*E0090*/

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0091*/
  DVMFTimeStart(call_incshd_);

  if(RTL_TRACE)
     dvm_trace(call_incshd_,
             "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx; "
             "ArrayHeader=%lx; ArrayHandlePtr=%lx; MaxShdCount=%ld;\n",
             (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr,
             (uLLng)ArrayHeader, ArrayHeader[0], *MaxShdCountPtr);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.252: wrong call incshd_\n"
            "(the object is not a distributed array; "
            "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;

  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0092*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM  */    /*E0093*/

  if(BoundHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 080.253: wrong call incshd_\n"
          "(the shadow group was not created by the current subtask;\n"
          "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
          "CurrentEnvIndex=%d)\n",
          *ShadowGroupRefPtr, BoundHandlePtr->CrtEnvInd, CurrEnvInd);
         
  ArrayDescPtr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV = ArrayDescPtr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.254: wrong call incshd_ "
              "(the array has not been aligned;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 080.255: wrong call incshd_\n"
             "(the array PS is not a subsystem of the current PS;\n"
             "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
             ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

  /* Check if the edge group has been started */    /*E0094*/
                                  
  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.256: wrong call incshd_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.257: wrong call incshd_\n"
              "(the import receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.258: wrong call incshd_\n"
              "(the export sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.259: wrong call incshd_\n"
              "(the local receiving has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.260: wrong call incshd_\n"
              "(the boundary sending has not been completed;\n"
              "ArrayHeader[0]=%lx; ShadowGroupRef=%lx)\n",
              ArrayHeader[0], *ShadowGroupRefPtr);

  BGPtr->IsStrt = 0;  /* */    /*E0095*/

  Rank = ArrayDescPtr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_incshd_))
     {  for(i=0; i < Rank; i++)
            tprintf("   InitDimIndex[%d]=%ld; ",i,InitDimIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("   LastDimIndex[%d]=%ld; ",i,LastDimIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("InitLowShdIndex[%d]=%ld; ",i,InitLowShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("LastLowShdIndex[%d]=%ld; ",i,LastLowShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf(" InitHiShdIndex[%d]=%ld; ",i,InitHiShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf(" LastHiShdIndex[%d]=%ld; ",i,LastHiShdIndex[i]);
        tprintf(" \n");
        for(i=0; i < Rank; i++)
            tprintf("   ShdSignArray[%d]=%ld; ",i,ShdSignArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  CurrEnvInd = ArrayDescPtr->BG.Count; /* */    /*E0096*/

  for(j=0; j < CurrEnvInd; j++)
  {  BG = coll_At(s_BOUNDGROUP *, &ArrayDescPtr->BG, j); /* current
                                                            group */    /*E0097*/
     if(BG != BGPtr)
        continue; /* the current group is not the group
                     the array to be included in */    /*E0098*/

     ShdWidth = coll_At(s_SHDWIDTH *, &ArrayDescPtr->ResShdWidthColl, j);

     for(i=0; i < Rank; i++)
     {
        if(InitDimIndex[i] == -1)
        {  /* There is no global restriction on (i+1)-th array dimension */    /*E0099*/

           if(ShdWidth->InitDimIndex[i] != 0)
              break;

           if(ShdWidth->DimWidth[i] != ArrayDescPtr->Space.Size[i])
              break;
        }
        else
        {  /* global restriction on (i+1)-th array dimension 
              is defined */    /*E0100*/

           if(InitDimIndex[i] != ShdWidth->InitDimIndex[i])
              break;

           if((LastDimIndex[i]-InitDimIndex[i]+1) !=
               ShdWidth->DimWidth[i])
               break;
        }

        if(InitLowShdIndex[i] == -1)
        {  /* width of the low edge of (i+1)- th dimension 
              is defined in LastLowShdIndex[i] */    /*E0101*/

           if(LastLowShdIndex[i] == -1)
              temp = (ShdWidth->ResLowShdWidth[i] ==
                      ArrayDescPtr->InitLowShdWidth[i]);
           else
              temp = (ShdWidth->ResLowShdWidth[i] == LastLowShdIndex[i]);

           if(temp == 0)
              break;

           if(ShdWidth->InitLowShdIndex[i] !=
              (ArrayDescPtr->InitLowShdWidth[i] -
               ShdWidth->ResLowShdWidth[i]))
              break; 
        }
        else
        {  /* initial and final relative index values determining
              the low edge of (i+1)-th dimension are defined in
              InitLowShdIndex[i] and LastLowShdIndex[i] */    /*E0102*/

           if(InitLowShdIndex[i] != ShdWidth->InitLowShdIndex[i])
              break;

           if((LastLowShdIndex[i]-InitLowShdIndex[i]+1) !=
              ShdWidth->ResLowShdWidth[i])
              break;
        }

        if(InitHiShdIndex[i] == -1)
        {  /* width of the high edge of (i+1)- th dimension 
              is defined in LastHiShdIndex[i] */    /*E0103*/

           if(ShdWidth->InitHiShdIndex[i] != 0)
              break;

           if(LastHiShdIndex[i] == -1)
              temp = (ShdWidth->ResHighShdWidth[i] ==
                      ArrayDescPtr->InitHighShdWidth[i]);
           else
              temp = (ShdWidth->ResHighShdWidth[i] == LastHiShdIndex[i]);

           if(temp == 0)
              break;
        }
        else
        {  /* initial and final relative index values determining
              the high edge of (i+1)-th dimension are defined in
              InitHiShdIndex[i] and LastHiShdIndex[i] */    /*E0104*/

           if(InitHiShdIndex[i] != ShdWidth->InitHiShdIndex[i])
              break;

           if((LastHiShdIndex[i]-InitHiShdIndex[i]+1) !=
              ShdWidth->ResHighShdWidth[i])
              break;
        }

        temp = (ShdWidth->ShdSign[i] == ShdSignArray[i]);

        if(temp == 0)
           break;
     }

     if(i != Rank)
        continue;

     temp = (ShdWidth->MaxShdCount == *MaxShdCountPtr);

     if(temp == 0)
        continue;

     break;
  }

  if(j == CurrEnvInd)
  {  /* The array has not been included in the group */    /*E0105*/

     dvm_AllocStruct(s_SHDWIDTH, ShdWidth);
     ShdWidth->UseSign = 0;  /* */    /*E0106*/

     ShdWidth->MaxShdCount = (byte)*MaxShdCountPtr;

     if(*MaxShdCountPtr < 1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.261: wrong call incshd_\n"
                 "(invalid MaxShdCount=%ld)\n", *MaxShdCountPtr);

     ZeroSign = 1;

     for(i=0; i < Rank; i++)
     {  /* processing InitDimIndex and LastDimIndex arrays*/    /*E0107*/

        if(InitDimIndex[i] == -1)
        {  /* There is no global restriction on (i+1)-th array dimension */    /*E0108*/

           ShdWidth->InitDimIndex[i] = 0;
           ShdWidth->DimWidth[i]     = ArrayDescPtr->Space.Size[i];
        }
        else
        {  /* global restriction on (i+1)-th array dimension 
              is defined */    /*E0109*/

           if(InitDimIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.264: wrong call incshd_\n"
                       "(invalid InitDimIndex[%d]=%ld)\n",
                       i, InitDimIndex[i]);

           if(InitDimIndex[i] > LastDimIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.265: wrong call incshd_\n"
                    "(InitDimIndex[%d]=%ld > LastDimIndex[%d]=%ld)\n",
                    i, InitDimIndex[i], i, LastDimIndex[i]);

           if(LastDimIndex[i] >= ArrayDescPtr->Space.Size[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.266: wrong call incshd_\n"
                       "(invalid LastDimIndex[%d]=%ld >= %ld)\n",
                       i, LastDimIndex[i], ArrayDescPtr->Space.Size[i]);

           ShdWidth->InitDimIndex[i] = InitDimIndex[i];
           ShdWidth->DimWidth[i] = LastDimIndex[i]-InitDimIndex[i]+1;
        }

        /* processing InitLowShdIndex and LastLowShdIndex arrays*/    /*E0110*/

        if(InitLowShdIndex[i] == -1)
        {  /* width of the low edge of (i+1)- th dimension 
              is defined in LastLowShdIndex[i] */    /*E0111*/

           if(LastLowShdIndex[i] < -1 ||
              LastLowShdIndex[i] > ArrayDescPtr->InitLowShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.270: wrong call incshd_\n"
                       "(invalid LastLowShdIndex[%d]=%ld)\n",
                       i, LastLowShdIndex[i]);

           if(LastLowShdIndex[i] == -1)
              ShdWidth->ResLowShdWidth[i] =
              ArrayDescPtr->InitLowShdWidth[i];
           else
              ShdWidth->ResLowShdWidth[i] = (int)LastLowShdIndex[i];

           ShdWidth->InitLowShdIndex[i] =
           ArrayDescPtr->InitLowShdWidth[i] -
           ShdWidth->ResLowShdWidth[i];
        }
        else
        {  /* initial and final relative index values determining
              the low edge of (i+1)-th dimension are defined in
              InitLowShdIndex[i] and LastLowShdIndex[i] */    /*E0112*/

           if(InitLowShdIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.275: wrong call incshd_\n"
                       "(invalid InitLowShdIndex[%d]=%ld)\n",
                       i, InitLowShdIndex[i]);

           if(InitLowShdIndex[i] > LastLowShdIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.276: wrong call incshd_\n"
                 "(InitLowShdIndex[%d]=%ld > LastLowShdIndex[%d]=%ld)\n",
                 i, InitLowShdIndex[i], i, LastLowShdIndex[i]);

           if(LastLowShdIndex[i] >= ArrayDescPtr->InitLowShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 080.277: wrong call incshd_\n"
                   "(invalid LastLowShdIndex[%d]=%ld >= %d)\n",
                   i, LastLowShdIndex[i],
                   ArrayDescPtr->InitLowShdWidth[i]);

           ShdWidth->InitLowShdIndex[i] = (int)InitLowShdIndex[i];
           ShdWidth->ResLowShdWidth[i]  = (int)(LastLowShdIndex[i] -
                                                InitLowShdIndex[i] + 1);
        }

        /* processing InitHiShdIndex and LastHiShdIndex arrays*/    /*E0113*/

        if(InitHiShdIndex[i] == -1)
        {  /* width of the high edge of (i+1)- th dimension 
              is defined in LastHiShdIndex[i] */    /*E0114*/

           ShdWidth->InitHiShdIndex[i] = 0;

           if(LastHiShdIndex[i] < -1 ||
              LastHiShdIndex[i] > ArrayDescPtr->InitHighShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.280: wrong call incshd_\n"
                       "(invalid LastHiShdIndex[%d]=%ld)\n",
                       i, LastHiShdIndex[i]);

           if(LastHiShdIndex[i] == -1)
              ShdWidth->ResHighShdWidth[i] =
              ArrayDescPtr->InitHighShdWidth[i];
           else
              ShdWidth->ResHighShdWidth[i] = (int)LastHiShdIndex[i];
        }
        else
        {  /* initial and final relative index values determining
              the high edge of (i+1)-th dimension are defined in
              InitHiShdIndex[i] and LastHiShdIndex[i] */    /*E0115*/

           if(InitHiShdIndex[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 080.285: wrong call incshd_\n"
                       "(invalid InitHiShdIndex[%d]=%ld)\n",
                       i, InitHiShdIndex[i]);

           if(InitHiShdIndex[i] > LastHiShdIndex[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.286: wrong call incshd_\n"
                 "(InitHiShdIndex[%d]=%ld > LastHiShdIndex[%d]=%ld)\n",
                 i, InitHiShdIndex[i], i, LastHiShdIndex[i]);

           if(LastHiShdIndex[i] >= ArrayDescPtr->InitHighShdWidth[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 080.287: wrong call incshd_\n"
                   "(invalid LastHiShdIndex[%d]=%ld >= %d)\n",
                   i, LastHiShdIndex[i],
                   ArrayDescPtr->InitHighShdWidth[i]);

           ShdWidth->InitHiShdIndex[i] = (int)InitHiShdIndex[i];
           ShdWidth->ResHighShdWidth[i] = (int)(LastHiShdIndex[i] -
                                                InitHiShdIndex[i] + 1);
        }

        ShdWidth->ShdSign[i] = (byte)ShdSignArray[i];

        if(ShdSignArray[i] < 1 || ShdSignArray[i] > 7)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 080.290: wrong call incshd_ "
                    "(invalid ShdSignArray[%d]=%ld)\n",
                    i, ShdSignArray[i]);

        if(ShdSignArray[i] > 1)
           ZeroSign = 0;
     }

     if(ZeroSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 080.291: wrong call incshd_ "
               "(the boundary of the distributed array is empty)\n");

     coll_Insert(&ArrayDescPtr->BG, BGPtr); /* to the list of edge groups
                                               the array is included in */    /*E0116*/

     /* To the list of resulting edge wigths */    /*E0117*/

     coll_Insert(&ArrayDescPtr->ResShdWidthColl, ShdWidth);

     /* To the lists of all and new arrays included in the group */    /*E0118*/

     coll_Insert(&BGPtr->NewArrayColl, ArrayDescPtr);
     coll_Insert(&BGPtr->NewShdWidthColl, ShdWidth);
     coll_Insert(&BGPtr->ArrayColl, ArrayDescPtr);

     if(EnableDynControl)
        dyn_DisArrDefineShadow(ArrayDescPtr, BGPtr, ShdWidth);
  }
 
  if(RTL_TRACE)
     dvm_trace(ret_incshd_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0119*/
  DVMFTimeFinish(ret_incshd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd strtsh_(ShadowGroupRef  *ShadowGroupRefPtr)

/*
      Starting shadow edge group renewing.
      ------------------------------------

*ShadowGroupRefPtr - reference to the shadow edge group.

The function initializes the system renewing buffer
(if the shadow edge group is renewed at first time)
and starts the shadow edge renewing operation for all
shadow edges registered by the function inssh_.
The function returns zero.
*/    /*E0120*/

{ SysHandle     *BGHandlePtr, *TstHandlePtr;
  s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  s_BOUNDBUF    *BBuf;
  int            i; 

  if(StrtShdSynchr)
     (RTL_CALL, bsynch_()); /* */    /*E0121*/

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0122*/
  DVMFTimeStart(call_strtsh_);

  if(RTL_TRACE)
     dvm_trace(call_strtsh_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.040: wrong call strtsh_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.041: wrong call strtsh_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.042: wrong call strtsh_\n"
              "(the shadow edges exchange has already been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.043: wrong call strtsh_\n"
              "(the import receiving has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.044: wrong call strtsh_\n"
              "(the export sending has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.045: wrong call strtsh_\n"
              "(the local receiving has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.046: wrong call strtsh_\n"
              "(the boundary sending has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  /* Check if all group arrays are mapped on the processor systems
     included in the current processor system */    /*E0123*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
  {  /* */    /*E0124*/

     TstHandlePtr = CheckShadowArrayVMS(BGHandlePtr);

     if(TstHandlePtr)
     {  DA  = (s_DISARRAY *)TstHandlePtr->pP;
        VMS = DA->AMView->VMS;

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.047: wrong call strtsh_\n"
                 "(the array PS is not a subsystem of the current PS;\n"
                 "ShadowGroupRef=%lx; ArrayHeader[0]=%lx;\n"
                 "ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
                 *ShadowGroupRefPtr, (uLLng)TstHandlePtr,
                 (uLLng)VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  BGPtr->IsStrtsh = 1;   /* edge exchange is executed now */    /*E0125*/
  BGPtr->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle of AM 
                                              which started the operation */    /*E0126*/
  BGPtr->ShdEnvInd = gEnvColl->Count - 1;  /* index of context
                                              which started the operation */    /*E0127*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
     CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers for 
                                    the whole edge group */    /*E0128*/

  /* Forward to the next element of message tag circle tag_BoundsBuffer
     for the current processor system */    /*E0129*/

  DVM_VMS->tag_BoundsBuffer++;

  if((DVM_VMS->tag_BoundsBuffer - (msg_BoundsBuffer)) >= TagCount)
     DVM_VMS->tag_BoundsBuffer = msg_BoundsBuffer;

  /* */    /*E0130*/

  i = 0;

  #ifdef _DVM_MPI_

  if(MsgDVMCompress != 0 && UserSumFlag != 0 &&
     MsgCompressWithMsgPart > 0)
  {
     if(MsgSchedule ||
        (MsgPartReg && MaxMsgLength > 0 && CheckRendezvous == 0))
        i = 1;  /* */    /*E0131*/
  }

  #endif

  #if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  if(MsgCompressLevel >= 0 && UserSumFlag != 0 &&
     MsgCompressWithMsgPart > 0)
  {
     if(MsgSchedule ||
        (MsgPartReg && MaxMsgLength > 0 && CheckRendezvous == 0))
        i = 1;  /* */    /*E0132*/
  }

  #endif

  /* Start shadow edge renewal of all edge buffers */    /*E0133*/

  TstHandlePtr = BGPtr->BufPtr;

  while(TstHandlePtr)
  {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;

     if(i != 0)
     {  bbuf_Send(BBuf);         /* rewrite edges in buffer
                                 and start sending */    /*E0134*/
        bbuf_Receive(BBuf);      /* initialisation of edge receiving */    /*E0135*/
     }
     else
     {  bbuf_Receive(BBuf);      /* */    /*E0136*/
        bbuf_Send(BBuf);         /* */    /*E0137*/
     }

     TstHandlePtr =
     (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next
                                                    edge buffer */    /*E0138*/
  }

  if(MsgSchedule && UserSumFlag)
  {  rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffShdSend);
  }

  BGPtr->IsStrt = 1;     /* flag: edge exchange
                            has been started */    /*E0139*/

  if(RTL_TRACE)
     dvm_trace(ret_strtsh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0140*/
  DVMFTimeFinish(ret_strtsh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd recvsh_(ShadowGroupRef  *ShadowGroupRefPtr)
{ SysHandle     *BGHandlePtr, *TstHandlePtr;
  s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  s_BOUNDBUF    *BBuf;
  int            i;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0141*/
  DVMFTimeStart(call_recvsh_);

  if(RTL_TRACE)
     dvm_trace(call_recvsh_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.050: wrong call recvsh_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.051: wrong call recvsh_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.052: wrong call recvsh_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.053: wrong call recvsh_\n"
              "(the import receiving has already been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.054: wrong call recvsh_\n"
              "(the boundary sending has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  i = gEnvColl->Count - 1;       /* current context index */    /*E0142*/

  if(BGPtr->ShdAMHandlePtr && BGPtr->ShdAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.056: wrong call recvsh_\n"
       "(export sending or local receiving "
       "has already been started by an "
       "other subtask;\nShadowGroupRef=%lx; SendEnvIndex=%d; "
       "CurrentEnvIndex=%d)\n",
       *ShadowGroupRefPtr, BGPtr->ShdEnvInd, i);

  /* Check if all group arrays are mapped on the processor systems
     included in the current processor system */    /*E0143*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
  {  /* */    /*E0144*/

     TstHandlePtr = CheckShadowArrayVMS(BGHandlePtr);

     if(TstHandlePtr)
     {  DA  = (s_DISARRAY *)TstHandlePtr->pP;
        VMS = DA->AMView->VMS;

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.057: wrong call recvsh_\n"
                 "(the array PS is not a subsystem of the current PS;\n"
                 "ShadowGroupRef=%lx; ArrayHeader[0]=%lx;\n"
                 "ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
                 *ShadowGroupRefPtr, (uLLng)TstHandlePtr,
                 (uLLng)VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  BGPtr->IsRecvsh = 1; /* imported elements are received now */    /*E0145*/
  BGPtr->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle of AM 
                                              which started the operation */    /*E0146*/
  BGPtr->ShdEnvInd = gEnvColl->Count - 1;  /* index of context
                                              which started the operation */    /*E0147*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
     CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers for 
                                    the whole edge group */    /*E0148*/

  /* Start imported element receiving for all edge buffers */    /*E0149*/

  TstHandlePtr = BGPtr->BufPtr;

  while(TstHandlePtr)
  {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
     bbuf_Receive(BBuf);  /* initialisation of edge receiving */    /*E0150*/

     if(EnableDynControl)
        for(i = 0; i < BBuf->ArrCount; i++)
            dyn_DisArrAcross(BBuf->ArrList[i], 1);

     TstHandlePtr =
     (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next edge buffer */    /*E0151*/
  }

  BGPtr->IsStrt = 1;     /* flag: an edge exchange has been started */    /*E0152*/

  if(RTL_TRACE)
     dvm_trace(ret_recvsh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0153*/
  DVMFTimeFinish(ret_recvsh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd sendsh_(ShadowGroupRef  *ShadowGroupRefPtr)
{ SysHandle     *BGHandlePtr, *TstHandlePtr;
  s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  s_BOUNDBUF    *BBuf;
  int            i;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0154*/
  DVMFTimeStart(call_sendsh_);

  if(RTL_TRACE)
     dvm_trace(call_sendsh_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.060: wrong call sendsh_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.061: wrong call sendsh_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.062: wrong call sendsh_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.063: wrong call sendsh_\n"
              "(the export sending has already been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.064: wrong call sendsh_\n"
              "(the local receiving has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  i = gEnvColl->Count - 1;       /* current context index */    /*E0155*/

  if(BGPtr->ShdAMHandlePtr && BGPtr->ShdAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.066: wrong call sendsh_\n"
       "(import receiving or boundary sending "
       "has already been started by an "
       "other subtask;\nShadowGroupRef=%lx; RecvEnvIndex=%d; "
       "CurrentEnvIndex=%d)\n",
       *ShadowGroupRefPtr, BGPtr->ShdEnvInd, i);

  /* Check if all group arrays are mapped on the processor systems
     included in the current processor system */    /*E0156*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
  {  /* */    /*E0157*/

     TstHandlePtr = CheckShadowArrayVMS(BGHandlePtr);

     if(TstHandlePtr)
     {  DA  = (s_DISARRAY *)TstHandlePtr->pP;
        VMS = DA->AMView->VMS;

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.067: wrong call sendsh_\n"
                 "(the array PS is not a subsystem of the current PS;\n"
                 "ShadowGroupRef=%lx; ArrayHeader[0]=%lx;\n"
                 "ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
                 *ShadowGroupRefPtr, (uLLng)TstHandlePtr,
                 (uLLng)VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  BGPtr->IsSendsh = 1; /* exported elements are sent now */    /*E0158*/
  BGPtr->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle of AM 
                                              which started the operation */    /*E0159*/
  BGPtr->ShdEnvInd = gEnvColl->Count - 1;  /* index of context
                                              which started the operation */    /*E0160*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
     CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers for 
                                    the whole edge group */    /*E0161*/

  /* Start exported element sending for all edge buffers */    /*E0162*/

  TstHandlePtr = BGPtr->BufPtr;

  while(TstHandlePtr)
  {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
     bbuf_Send(BBuf);         /* rewrite edges in buffer 
                                 and start sending */    /*E0163*/

     TstHandlePtr =
     (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next edge buffer */    /*E0164*/ 
  }

  if(MsgSchedule && UserSumFlag)
  {  rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffShdSend);
  }

  BGPtr->IsStrt = 1;     /* flag: an edge exchange has been started */    /*E0165*/

  if(RTL_TRACE)
     dvm_trace(ret_sendsh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0166*/
  DVMFTimeFinish(ret_sendsh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd sendsa_(ShadowGroupRef  *ShadowGroupRefPtr)
{ SysHandle     *BGHandlePtr, *TstHandlePtr;
  s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  s_BOUNDBUF    *BBuf;
  int            i;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0167*/
  DVMFTimeStart(call_sendsa_);

  if(RTL_TRACE)
     dvm_trace(call_sendsa_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.150: wrong call sendsa_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.151: wrong call sendsa_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.152: wrong call sendsa_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.153: wrong call sendsa_\n"
              "(the boundary sending has already been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.154: wrong call sendsa_\n"
              "(the import receiving has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  i = gEnvColl->Count - 1;       /* current context index */    /*E0168*/

  if(BGPtr->ShdAMHandlePtr && BGPtr->ShdAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.156: wrong call sendsa_\n"
       "(export sending or local receiving "
       "has already been started by an "
       "other subtask;\nShadowGroupRef=%lx; SendEnvIndex=%d; "
       "CurrentEnvIndex=%d)\n",
       *ShadowGroupRefPtr, BGPtr->ShdEnvInd, i);

  /* Check if all group elements are mapped on the processor systems 
     with elements from the current processor system */    /*E0169*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
  {  /* */    /*E0170*/

     TstHandlePtr = CheckShadowArrayVMS(BGHandlePtr);

     if(TstHandlePtr)
     {  DA  = (s_DISARRAY *)TstHandlePtr->pP;
        VMS = DA->AMView->VMS;

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.157: wrong call sendsa_\n"
                 "(the array PS is not a subsystem of the current PS;\n"
                 "ShadowGroupRef=%lx; ArrayHeader[0]=%lx;\n"
                 "ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
                 *ShadowGroupRefPtr, (uLLng)TstHandlePtr,
                 (uLLng)VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  BGPtr->IsSendsa = 1; /* flag: edge element sending is executing now */    /*E0171*/
  BGPtr->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle AM
                                              where the operation has been started */    /*E0172*/
  BGPtr->ShdEnvInd = gEnvColl->Count - 1;  /* index of context 
                                              where the operation has been started */    /*E0173*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
     CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers 
                                    for the whole edge group  */    /*E0174*/

  /* Start edge element sending for all edge buffers */    /*E0175*/

  TstHandlePtr = BGPtr->BufPtr;

  while(TstHandlePtr)
  {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
     bbuf_Sendsa(BBuf);  /* rewrite edges in the buffer and
                            initialize  it sending to the neighbouring processors */    /*E0176*/

     TstHandlePtr =
     (SysHandle *)TstHandlePtr->NextHandlePtr; /* to next
                                                    edge buffer*/    /*E0177*/
  }

  if(MsgSchedule && UserSumFlag)
  {  rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffInSend);
  }

  BGPtr->IsStrt = 1;   /* flag: edge exchange (any) has already
                          been started */    /*E0178*/

  if(RTL_TRACE)
     dvm_trace(ret_sendsa_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0179*/
  DVMFTimeFinish(ret_sendsa_);
  return  (DVM_RET, 0);
}



DvmType  __callstd recvla_(ShadowGroupRef  *ShadowGroupRefPtr)
{ SysHandle     *BGHandlePtr, *TstHandlePtr;
  s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  s_BOUNDBUF    *BBuf;
  int            i;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0180*/
  DVMFTimeStart(call_recvla_);

  if(RTL_TRACE)
     dvm_trace(call_recvla_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.160: wrong call recvla_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.161: wrong call recvla_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BGPtr=(s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->IsStrtsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.162: wrong call recvla_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsRecvla)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.163: wrong call recvla_\n"
              "(the local receiving has already been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  if(BGPtr->IsSendsh)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.164: wrong call recvla_\n"
              "(the export sending has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  i = gEnvColl->Count - 1;       /* current context index */    /*E0181*/

  if(BGPtr->ShdAMHandlePtr && BGPtr->ShdAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.166: wrong call recvla_\n"
       "(import receiving or boundary sending "
       "has already been started by an "
       "other subtask;\nShadowGroupRef=%lx; RecvEnvIndex=%d; "
       "CurrentEnvIndex=%d)\n",
       *ShadowGroupRefPtr, BGPtr->ShdEnvInd, i);

  /* Check if all group elements are mapped on the processor systems 
     with elements from the current processor system */    /*E0182*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
  {  /* */    /*E0183*/

     TstHandlePtr = CheckShadowArrayVMS(BGHandlePtr);

     if(TstHandlePtr)
     {  DA  = (s_DISARRAY *)TstHandlePtr->pP;
        VMS = DA->AMView->VMS;

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.167: wrong call recvla_\n"
                 "(the array PS is not a subsystem of the current PS;\n"
                 "ShadowGroupRef=%lx; ArrayHeader[0]=%lx;\n"
                 "ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
                 *ShadowGroupRefPtr, (uLLng)TstHandlePtr,
                 (uLLng)VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  BGPtr->IsRecvla = 1; /* flag: edge element receiving is executing now */    /*E0184*/
  BGPtr->ShdAMHandlePtr = CurrAMHandlePtr; /* pointer to Handle AM
                                              where the operation has been started */    /*E0185*/
  BGPtr->ShdEnvInd = gEnvColl->Count - 1;  /* index of context 
                                              where the operation has been started */    /*E0186*/

  if(BGPtr->SaveSign == 0 && BGPtr->IsStrt == 0)
     CrtShdGrpBuffers(BGHandlePtr); /* create edge buffers 
                                    for the whole edge group */    /*E0187*/

  /* Start edge element receiving for all edge buffers */    /*E0188*/

  TstHandlePtr = BGPtr->BufPtr;

  while(TstHandlePtr)
  {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
     bbuf_Recvla(BBuf);  /* initialization of local elements receiving 
                            in Out buffer */    /*E0189*/

     if(EnableDynControl)
        for(i = 0; i < BBuf->ArrCount; i++)
            dyn_DisArrAcross(BBuf->ArrList[i], 2);

     TstHandlePtr =
     (SysHandle *)TstHandlePtr->NextHandlePtr; /* to next
                                                    edge buffer */    /*E0190*/
  }

  BGPtr->IsStrt = 1;   /* flag: edge exchange (any) has already
                          been started */    /*E0191*/

  if(RTL_TRACE)
     dvm_trace(ret_recvla_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0192*/
  DVMFTimeFinish(ret_recvla_);
  return  (DVM_RET, 0);
}



DvmType   __callstd  waitsh_(ShadowGroupRef  *ShadowGroupRefPtr)

/*
      Waiting for completion of shadow edge group renewing.
      -----------------------------------------------------

*ShadowGroupRefPtr - reference to the shadow edge group.
The function returns zero.
*/    /*E0193*/

{ SysHandle     *BGHandlePtr, *CurrHandlePtr;
  s_BOUNDGROUP  *BG;
  s_BOUNDBUF    *BBuf;
  int            i;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0194*/
  DVMFTimeStart(call_waitsh_);

  if(RTL_TRACE)
     dvm_trace(call_waitsh_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr = (SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 080.070: wrong call waitsh_\n"
                 "(the shadow group is not a DVM object; "
                 "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.071: wrong call waitsh_\n"
            "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
            *ShadowGroupRefPtr);

  BG = (s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BG->IsStrtsh == 0 && BG->IsRecvsh == 0 && BG->IsSendsh == 0 &&
     BG->IsRecvla == 0 && BG->IsSendsa == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.072: wrong call waitsh_\n"
              "(the shadow edges exchange has not been started;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  i = gEnvColl->Count - 1;       /* current context index */    /*E0195*/

  if(BG->ShdAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.073: wrong call waitsh_\n"
       "(the shadow edges exchange was not started by the "
       "current subtask;\nShadowGroupRef=%lx; ShdEnvIndex=%d; "
       "CurrentEnvIndex=%d)\n",
       *ShadowGroupRefPtr, BG->ShdEnvInd, i);

  CurrHandlePtr = BG->BufPtr;

  while(CurrHandlePtr)
  {  BBuf = (s_BOUNDBUF *)CurrHandlePtr->pP;

     if(BG->IsStrtsh || BG->IsSendsh) /* is edge exchange or 
                                         exported element sending
                                         executed now ? */    /*E0196*/
        bbuf_WSendComplete(BBuf);

     if(BG->IsStrtsh || BG->IsRecvsh) /* is edge exchange or 
                                         imported element receiving
                                         executed now ? */    /*E0197*/
        bbuf_WRecvComplete(BBuf);

     if(BG->IsSendsa) /* has edge elements sending already been started */    /*E0198*/
        bbuf_WSendsaComplete(BBuf);

     if(BG->IsRecvla) /* has local elements receiving already been started */    /*E0199*/
        bbuf_WRecvlaComplete(BBuf);

     if(EnableDynControl)
        for(i=0; i < BBuf->ArrCount; i++)
            dyn_DisArrCompleteShadows(BBuf->ArrList[i], BG);

     CurrHandlePtr =
     (SysHandle *)CurrHandlePtr->NextHandlePtr; /* to the next edge buffer */    /*E0200*/ 
  }

  /* edge exchange operations are not execited now */    /*E0201*/

  BG->IsStrtsh = 0;
  BG->IsRecvsh = 0;
  BG->IsSendsh = 0;
  BG->IsRecvla = 0;
  BG->IsSendsa = 0;

  BG->ShdAMHandlePtr = NULL; /* pointer to Handle of AM 
                                which started the edge exchange operation */    /*E0202*/
  BG->ShdEnvInd      = 0;    /* index of context which started
                                the edge exchange operation  */    /*E0203*/

  if(RTL_TRACE)
     dvm_trace(ret_waitsh_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0204*/
  DVMFTimeFinish(ret_waitsh_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delshg_(ShadowGroupRef  *ShadowGroupRefPtr)

/*
      Deleting shadow edge group.
      ---------------------------

*ShadowGroupRefPtr - reference to the shadow edge group.

The function deletes the shadow edge group created by function crtshg_.
The function returns zero.
*/    /*E0205*/

{ SysHandle     *BGHandlePtr;
  s_BOUNDGROUP  *BG;
  int            i;
  void          *CurrAM;

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;
  BG = (s_BOUNDGROUP *)BGHandlePtr->pP;

  if(ShgSave)
     return (DVM_RET, 0); /* */    /*E0206*/

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0207*/
  DVMFTimeStart(call_delshg_);

  if(RTL_TRACE)
     dvm_trace(call_delshg_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.080: wrong call delshg_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.081: wrong call delshg_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BG = (s_BOUNDGROUP *)BGHandlePtr->pP;

  /* Check if an edge group has been created in the current subtask */    /*E0208*/
      
  i      = gEnvColl->Count - 1;     /* current context index */    /*E0209*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0210*/

  if(BGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 080.082: wrong call delshg_\n"
           "(the shadow group was not created by the current subtask;\n"
           "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
           "CurrentEnvIndex=%d)\n",
           *ShadowGroupRefPtr, BGHandlePtr->CrtEnvInd, i);

  if(BG->IsStrtsh || BG->IsRecvsh || BG->IsSendsh ||
     BG->IsRecvla || BG->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.083: wrong call delshg_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  ( RTL_CALL, delobj_(ShadowGroupRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delshg_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* for statistics */    /*E0211*/
  DVMFTimeFinish(ret_delshg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd rstshg_(ShadowGroupRef  *ShadowGroupRefPtr)
{ SysHandle     *BGHandlePtr, *NewBGHandlePtr;
  s_BOUNDGROUP  *BG;
  int            i;
  void          *CurrAM;
  DvmType           StaticSign;

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;
  BG = (s_BOUNDGROUP *)BGHandlePtr->pP;

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* */    /*E0212*/
  DVMFTimeStart(call_rstshg_);

  if(RTL_TRACE)
     dvm_trace(call_rstshg_,
               "ShadowGroupRefPtr=%lx; ShadowGroupRef=%lx;\n",
               (uLLng)ShadowGroupRefPtr, *ShadowGroupRefPtr);

  BGHandlePtr=(SysHandle *)*ShadowGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ShadowGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 080.085: wrong call rstshg_\n"
            "(the shadow group is not a DVM object; "
            "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);
  }

  if(BGHandlePtr->Type != sht_BoundsGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 080.086: wrong call rstshg_\n"
       "(the object is not a shadow group; ShadowGroupRef=%lx)\n",
       *ShadowGroupRefPtr);

  BG = (s_BOUNDGROUP *)BGHandlePtr->pP;

  /* */    /*E0213*/
      
  i      = gEnvColl->Count - 1;     /* */    /*E0214*/
  CurrAM = (void *)CurrAMHandlePtr; /* */    /*E0215*/

  if(BGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 080.087: wrong call rstshg_\n"
           "(the shadow group was not created by the current subtask;\n"
           "ShadowGroupRef=%lx; ShadowGroupEnvIndex=%d; "
           "CurrentEnvIndex=%d)\n",
           *ShadowGroupRefPtr, BGHandlePtr->CrtEnvInd, i);

  if(BG->IsStrtsh || BG->IsRecvsh || BG->IsSendsh ||
     BG->IsRecvla || BG->IsSendsa)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 080.088: wrong call rstshg_\n"
              "(the shadow edges exchange has not been completed;\n"
              "ShadowGroupRef=%lx)\n", *ShadowGroupRefPtr);

  StaticSign = BG->Static;
  BG->ResetSign = 1;  /* */    /*E0216*/

  ( RTL_CALL, delobj_(ShadowGroupRefPtr) );

  NewBGHandlePtr = (SysHandle *)( RTL_CALL, crtshg_(&StaticSign) );

  BG = (s_BOUNDGROUP *)NewBGHandlePtr->pP;
  BGHandlePtr->pP = NewBGHandlePtr->pP;
  BG->HandlePtr = BGHandlePtr;

  NewBGHandlePtr->Type = sht_NULL;
  dvm_FreeStruct(NewBGHandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_rstshg_," \n");

  StatObjectRef = (ObjectRef)*ShadowGroupRefPtr; /* */    /*E0217*/
  DVMFTimeFinish(ret_rstshg_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------ */    /*E0218*/


void  CrtShdGrpBuffers(SysHandle  *BGHandlePtr)
{ s_BOUNDGROUP  *BGPtr;
  SysHandle     *CurrHandlePtr, *BufHandlePtr;
  int            i, j;
  s_BOUNDBUF    *BBuf;
  s_DISARRAY    *DA, *wDA;
  s_COLLECTION   wArrayColl, wShdWidthColl;
  s_SHDWIDTH    *ShdWidth, *wShdWidth;
  byte           CountSign = 0;

  BGPtr = (s_BOUNDGROUP *)BGHandlePtr->pP;

  if(BGPtr->NewArrayColl.Count)
  {  CountSign = 1;
     wArrayColl    = coll_Init(BGArrCount, BGArrCount, NULL);
     wShdWidthColl = coll_Init(BGArrCount, BGArrCount, NULL);
  }

  while(BGPtr->NewArrayColl.Count)
  { /* There are non processed arrays in the group still */    /*E0219*/

    DA            = coll_At(s_DISARRAY *, &BGPtr->NewArrayColl, 0);
    ShdWidth      = coll_At(s_SHDWIDTH *, &BGPtr->NewShdWidthColl, 0);
    CurrHandlePtr = DA->HandlePtr;

    /* Create the list of non processed arrays
       equivalent to the current non processed array */    /*E0220*/

    for(i=1; i < BGPtr->NewArrayColl.Count; i++)
    {  wDA       = coll_At(s_DISARRAY *, &BGPtr->NewArrayColl, i);
       wShdWidth = coll_At(s_SHDWIDTH *, &BGPtr->NewShdWidthColl, i);

       if( IsArrayEqu(CurrHandlePtr, wDA->HandlePtr, 1, 1,
           ShdWidth, wShdWidth) )
       {   coll_Insert(&wArrayColl, wDA);
           coll_Insert(&wShdWidthColl, wShdWidth);
       }
    }

    /* Create edge buffer */    /*E0221*/

    BufHandlePtr = ( RTL_CALL, CreateBoundBuffer((DvmType *)CurrHandlePtr->HeaderPtr, wArrayColl.Count+1) );

    /* Attach all arrays equivalent to the current non processed 
       array to the buffer */    /*E0222*/

    BBuf = (s_BOUNDBUF *)BufHandlePtr->pP;

    for(i=0; i < wArrayColl.Count; i++)
    {  if(BBuf->ArrCount >= BBuf->Count)
          epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS fatal err 080.090: "
                   "overflow of the shadow buffer counter\n");

       wDA = coll_At(s_DISARRAY *, &wArrayColl, i);

       BBuf->ArrCount ++;
       BBuf->ArrList[BBuf->ArrCount-1] = wDA;
    }

    /* Delete  current non processed array and arrays equivalent to it
       from the list of arrays newly included in group */    /*E0223*/

    coll_AtDelete(&BGPtr->NewArrayColl, 0);
    coll_AtDelete(&BGPtr->NewShdWidthColl, 0);

    for(i=0; i < wArrayColl.Count; i++)
    {  wShdWidth = coll_At(s_SHDWIDTH *, &wShdWidthColl, i);
       j = coll_IndexOf(&BGPtr->NewShdWidthColl, wShdWidth);

       coll_AtDelete(&BGPtr->NewArrayColl, j);
       coll_AtDelete(&BGPtr->NewShdWidthColl, j);
    }

    wArrayColl.Count    = 0;
    wShdWidthColl.Count = 0;

    /* Attach buffer to the group of edges */    /*E0224*/

    BufHandlePtr->NextHandlePtr = BGPtr->BufPtr;
    BGPtr->BufPtr = BufHandlePtr;

    /* Initialize structures for shadow edge renewal */    /*E0225*/
    
    shd_init(ShdWidth, BufHandlePtr);
  }

  if(CountSign)
  {  dvm_FreeArray(wArrayColl.List);
     dvm_FreeArray(wShdWidthColl.List);
  }

  return;
}


/* ----------------------------------------- */    /*E0226*/


SysHandle  *CreateBoundBuffer(DvmType  ArrayHeader[], int  Count)
{ SysHandle   *BoundHandlePtr;
  s_BOUNDBUF  *BB;
  s_DISARRAY  *DA;
  int          i, Rank;
  byte        *DimWidthSignPtr;
 
  if(RTL_TRACE)
     dvm_trace(call_CreateBoundBuffer,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; Count=%d;\n",
               (uLLng)ArrayHeader, ArrayHeader[0], Count);

  DA = (s_DISARRAY *)((SysHandle *)ArrayHeader[0])->pP; 

  dvm_AllocStruct(s_BOUNDBUF, BB);

  Rank = DA->Space.Rank;

  BB->EnvInd        = gEnvColl->Count-1;
  BB->CrtEnvInd     = gEnvColl->Count-1;
  BB->Count         = Count;
  BB->DoWaitIn      = FALSE;
  BB->DoWaitOut     = FALSE;
  BB->ArrCount      = 1;
  BB->Rank          = Rank;
  BB->ShdCount      = 0;
  BB->ShdInfo       = NULL;
  BB->DimBlock.Rank = 0;

  DimWidthSignPtr = BB->DimWidthSign;

  for(i=0; i < Rank; i++)
      DimWidthSignPtr[i] = 0;


  dvm_AllocArray(s_DISARRAY *, Count, BB->ArrList);
  BB->ArrList[0] = DA;

  dvm_AllocStruct(SysHandle, BoundHandlePtr);
  *BoundHandlePtr = sysh_Build(sht_BoundsBuffer, gEnvColl->Count-1,
                               gEnvColl->Count-1, 0, BB);

  BB->HandlePtr = BoundHandlePtr; /* pointer to own Handle */    /*E0227*/

  if(RTL_TRACE)
     dvm_trace(ret_CreateBoundBuffer,"BoundBufferRef=%lx;\n",
                                      (uLLng)BoundHandlePtr);

  return  (DVM_RET, BoundHandlePtr);
}


/* ------------------------------------------- */    /*E0228*/


/* BOUNDBUF  */    /*E0229*/


void   bbuf_WSendComplete(s_BOUNDBUF  *BBuf)
{ int            i;
  s_SHADOW      *SB;
 
  if(BBuf->DoWaitOut)
  {  for(i=0; i < BBuf->ShdCount; i++)
     {  SB = &BBuf->ShdInfo[i];

        if(SB->IsShdOut && SB->ReqOut.BufLength)
        {  if(RTL_TRACE)
             ( RTL_CALL, shd_Waitrequest(&SB->ReqOut) );
           else
             ( RTL_CALL, rtl_Waitrequest(&SB->ReqOut) );
        }
     }
  }

  BBuf->DoWaitOut = FALSE;
}



void   bbuf_WRecvComplete(s_BOUNDBUF  *BBuf)
{ int            i, ArN;
  s_SHADOW      *SB;
  DvmType           Size;
  char          *wPtr;
  byte           Step = 0;
 
  if(BBuf->DoWaitIn)
  {  for(i=0; i < BBuf->ShdCount; i++)
     {  SB = &BBuf->ShdInfo[i];

        if(SB->IsShdIn)
        { if(RTL_TRACE)
             ( RTL_CALL, shd_Waitrequest(&SB->ReqIn) );
          else
             ( RTL_CALL, rtl_Waitrequest(&SB->ReqIn) );

           block_GetSize(Size, &SB->BlockIn, Step)
           Size *= SB->TLen;
           wPtr = (char *)SB->BufIn;

           for(ArN=0; ArN < BBuf->ArrCount; ArN++)
           {  CopyMem2Block(&(BBuf->ArrList[ArN]->ArrBlock),
                            wPtr, &SB->BlockIn);
              wPtr += (int)Size;
           }
        }
     }
  }

  BBuf->DoWaitIn = FALSE;
}



void   bbuf_WSendsaComplete(s_BOUNDBUF  *BBuf)
{ int            i;
  s_SHADOW      *SB;
 
  if(BBuf->DoWaitIn)
  {  for(i=0; i < BBuf->ShdCount; i++)
     {  SB = &BBuf->ShdInfo[i];

        if(SB->IsShdIn && SB->ReqIn.BufLength)
        {  if(RTL_TRACE)
             ( RTL_CALL, shd_Waitrequest(&SB->ReqIn) );
           else
             ( RTL_CALL, rtl_Waitrequest(&SB->ReqIn) );
        }
     }
  }

  BBuf->DoWaitIn = FALSE;
}



void   bbuf_WRecvlaComplete(s_BOUNDBUF  *BBuf)
{ int            i, ArN;
  s_SHADOW      *SB;
  DvmType           Size;
  char          *wPtr;
  byte           Step = 0;
 
  if(BBuf->DoWaitOut)
  {  for(i=0; i < BBuf->ShdCount; i++)
     {  SB = &BBuf->ShdInfo[i];

        if(SB->IsShdOut)
        { if(RTL_TRACE)
             ( RTL_CALL, shd_Waitrequest(&SB->ReqOut) );
          else
             ( RTL_CALL, rtl_Waitrequest(&SB->ReqOut) );

           block_GetSize(Size, &SB->BlockOut, Step)
           Size *= SB->TLen;
           wPtr = (char *)SB->BufOut;

           for(ArN=0; ArN < BBuf->ArrCount; ArN++)
           {  CopyMem2Block(&(BBuf->ArrList[ArN]->ArrBlock),
                            wPtr, &SB->BlockOut);
              wPtr += (int)Size;
           }
        }
     }
  }

  BBuf->DoWaitOut = FALSE;
}



void   bbuf_Send(s_BOUNDBUF  *BBuf)
{ int            i, ArN;
  char          *wPtr;
  DvmType           Size;
  s_SHADOW      *SB;
  byte           Step = 0;

  for(i=0; i < BBuf->ShdCount; i++)
  {  SB = &BBuf->ShdInfo[i];

     if(SB->IsShdOut)
     {  /* Rewrite edges in transfer buffer */    /*E0230*/

        block_GetSize(Size, &SB->BlockOut, Step)
        Size *= SB->TLen;
        wPtr = (char *)SB->BufOut;

        for(ArN=0; ArN < BBuf->ArrCount; ArN++)
        {  CopyBlock2Mem(wPtr, &SB->BlockOut,
                         &(BBuf->ArrList[ArN]->ArrBlock));
           wPtr += (int)Size;
        }

        /* initialization of edge exchange */    /*E0231*/

        Size *= BBuf->ArrCount;
        wPtr = (char *)SB->BufOut;

        if(RTL_TRACE)
           ( RTL_CALL, shd_Sendnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqOut, ShdSend) );
        else 
           ( RTL_CALL, rtl_Sendnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqOut, ShdSend) );
     }
  }

  BBuf->DoWaitOut = TRUE;
}



void   bbuf_Receive(s_BOUNDBUF  *BBuf)
{ int            i;
  char          *wPtr;
  DvmType           Size;
  s_SHADOW      *SB;
  byte           Step = 0;

  for(i=0; i < BBuf->ShdCount; i++)
  {  SB = &BBuf->ShdInfo[i];

     if(SB->IsShdIn)
     {  block_GetSize(Size, &SB->BlockIn, Step)
        Size *= (BBuf->ArrCount * SB->TLen);
        wPtr  = (char *)SB->BufIn;

        if(RTL_TRACE)
           ( RTL_CALL, shd_Recvnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqIn) );
        else
           ( RTL_CALL, rtl_Recvnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqIn, 0) );
     }
  }

  BBuf->DoWaitIn = TRUE;
}



void   bbuf_Sendsa(s_BOUNDBUF  *BBuf)
{ int            i, ArN;
  char          *wPtr;
  DvmType           Size;
  s_SHADOW      *SB;
  byte           Step = 0;

  for(i=0; i < BBuf->ShdCount; i++)
  {  SB = &BBuf->ShdInfo[i];

     if(SB->IsShdIn)
     {  /* Rewrite edges in sending buffer */    /*E0232*/

        block_GetSize(Size, &SB->BlockIn, Step)
        Size *= SB->TLen;
        wPtr = (char *)SB->BufIn;

        for(ArN=0; ArN < BBuf->ArrCount; ArN++)
        {  CopyBlock2Mem(wPtr, &SB->BlockIn,
                         &(BBuf->ArrList[ArN]->ArrBlock));
           wPtr += (int)Size;
        }

        /* initialization of edge sending */    /*E0233*/

        Size *= BBuf->ArrCount;
        wPtr = (char *)SB->BufIn;

        if(RTL_TRACE)
           ( RTL_CALL, shd_Sendnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqIn, InSend) );
        else 
           ( RTL_CALL, rtl_Sendnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqIn, InSend) );
     }
  }

  BBuf->DoWaitIn = TRUE;
}



void   bbuf_Recvla(s_BOUNDBUF  *BBuf)
{ int            i;
  char          *wPtr;
  DvmType           Size;
  s_SHADOW      *SB;
  byte           Step = 0;

  for(i=0; i < BBuf->ShdCount; i++)
  {  SB = &BBuf->ShdInfo[i];

     if(SB->IsShdOut)
     {  block_GetSize(Size, &SB->BlockOut, Step)
        Size *= (BBuf->ArrCount * SB->TLen);
        wPtr  = (char *)SB->BufOut;

        if(RTL_TRACE)
           ( RTL_CALL, shd_Recvnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqOut) );
        else
           ( RTL_CALL, rtl_Recvnowait(wPtr, 1, (int)Size, (int)SB->Proc,
                                      DVM_VMS->tag_BoundsBuffer,
                                      &SB->ReqOut, 0) );
     }
  }

  BBuf->DoWaitOut = TRUE;
}


/*********************************************************\
* Edge exchange for pipeline ACROSS scheme execution     *
\*********************************************************/    /*E0234*/

void  InitAcrossRecv(s_PARLOOP  *PL)
{
    DvmType            Size;
    DvmType            DimLower[MAXARRAYDIM], DimSize[MAXARRAYDIM],
                  MaxLower[MAXARRAYDIM], tlong[MAXARRAYDIM],
                  PLSize[MAXARRAYDIM];
  s_BOUNDGROUP   *BG;
  s_BOUNDBUF     *BBuf;
  SysHandle      *TstHandlePtr;
  int             i, j, DDim;
  char           *wPtr;
  s_SHADOW       *SB;
  byte            Step = 0, DimWidthSign[MAXARRAYDIM];
  s_REGULARSET   *Set;
  s_BLOCK        *DimBlockPtr;

  for(j=0; j < PL->QDimNumber; j++)  /* */    /*E0235*/
  {
     DDim = PL->DDim[j]; /* number of array dimension
                      for extracting from shadow edge - 1 */    /*E0236*/

     if(PL->NextInit[j] > PL->NextLast[j])
     {  PL->RecvInit[j] = PL->NextLast[j] - PL->LowIndex[j];
        PL->RecvLast[j] = PL->NextInit[j] - PL->LowIndex[j];
     }
     else
     {  PL->RecvInit[j] = PL->NextInit[j] - PL->LowIndex[j];
        PL->RecvLast[j] = PL->NextLast[j] - PL->LowIndex[j];
     }

     PLSize[j] = PL->RecvLast[j] - PL->RecvInit[j] + 1;

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_dopl_))
        tprintf("***1 InitAcrossRecv: NQ=%d QDim=%d LowIndex=%ld "
                "RecvInit=%ld RecvLast=%ld DDim=%d\n",
                j, PL->QDim[j], PL->LowIndex[j], PL->RecvInit[j],
                PL->RecvLast[j], DDim);
  }

  if(PL->AcrType1)
  {  /* scheme <sendsh_ - recvsh_> is executing */    /*E0237*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup1Ref)->pP;
     BG->IsRecvsh = 1; /* flag: receiving imported elements */    /*E0238*/

     /* Start of receiving imported elements for all edge buffers */    /*E0239*/

     TstHandlePtr = BG->BufPtr;

     while(TstHandlePtr)
     {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
        DimBlockPtr = &BBuf->DimBlock;

        for(j=0; j < PL->QDimNumber; j++) /* */    /*E0240*/
        {
           DDim = PL->DDim[j]; /* */    /*E0241*/
           DimWidthSign[j] = BBuf->DimWidthSign[DDim];

           if(DimWidthSign[j])
           {  /* Edge of quanted dimension is not complete */    /*E0242*/

              DimLower[j] = DimBlockPtr->Set[DDim].Lower;
              DimSize[j] = DimLower[j] + DimBlockPtr->Set[DDim].Size;
              MaxLower[j] = dvm_max(DimLower[j], PL->RecvInit[j]);
              DimSize[j] = dvm_min(DimSize[j], (PL->RecvLast[j] + 1)) -
                           MaxLower[j];
              tlong[j] = MaxLower[j] + DimSize[j] - 1;
           }
        }

        /* Initialization of edge receiving */    /*E0243*/

        for(i=0; i < BBuf->ShdCount; i++)
        {  SB = &BBuf->ShdInfo[i];
           SB->ReqIn.BufLength = 0;

           if(SB->IsShdIn)
           {  Set = SB->BlockIn.Set;

              for(j=0; j < PL->QDimNumber; j++) /* */    /*E0244*/
              {
                 DDim = PL->DDim[j]; /* */    /*E0245*/
                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_dopl_))
                    tprintf("***2 InitAcrossRecv: AcrType1. NQ=%d "
                            "Lower=%ld Upper=%ld\n",
                            j, Set[DDim].Lower, Set[DDim].Upper);

                 if(DimWidthSign[j])
                 {  /* Edge is not complete */    /*E0246*/

                    if(DimSize[j] > 0)
                    {  SB->ReqIn.BufLength = 1;

                       Set[DDim].Lower = MaxLower[j];
                       Set[DDim].Upper = tlong[j];
                       Set[DDim].Size  = DimSize[j];
                    }
                 }
                 else
                 {  /* Edge is complete */    /*E0247*/

                    SB->ReqIn.BufLength = 1;

                    Set[DDim].Lower = PL->RecvInit[j];
                    Set[DDim].Upper = PL->RecvLast[j];
                    Set[DDim].Size  = PLSize[j];
                 }
              }

              if(SB->ReqIn.BufLength)
              {  block_GetSize(Size, &SB->BlockIn, Step)
                 Size *= (BBuf->ArrCount * SB->TLen);
                 wPtr  = (char *)SB->BufIn;

                 for(j=0; j < PL->QDimNumber; j++) /* */    /*E0248*/
                 {
                    DDim = PL->DDim[j]; /* */    /*E0249*/
                    if(RTL_TRACE && AcrossTrace &&
                       TstTraceEvent(call_dopl_))
                       tprintf("***3 InitAcrossRecv: AcrType1. NQ=%d "
                               "Lower=%ld Upper=%ld Size=%ld\n",
                               j, Set[DDim].Lower, Set[DDim].Upper,
                               Size);
                 }

                 if(RTL_TRACE)
                    ( RTL_CALL, shd_Recvnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqIn) );
                 else
                    ( RTL_CALL, rtl_Recvnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqIn, 0) );
              }
           }
        }

        BBuf->DoWaitIn = TRUE;

        TstHandlePtr =
        (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0250*/
     }
  }

  if(PL->AcrType2)
  {  /* scheme <sendsa_ - recvla_> is executing */    /*E0251*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup2Ref)->pP;
     BG->IsRecvla = 1; /* flag: receiving local elements */    /*E0252*/

     /* Start of receiving local elements for all edge buffers */    /*E0253*/

     TstHandlePtr = BG->BufPtr;

     while(TstHandlePtr)
     {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
        DimBlockPtr = &BBuf->DimBlock;

        for(j=0; j < PL->QDimNumber; j++) /* */    /*E0254*/
        {
           DDim = PL->DDim[j]; /* */    /*E0255*/
           DimWidthSign[j] = BBuf->DimWidthSign[DDim];

           if(DimWidthSign[j])
           {  /* Edge of quanted dimension is not complete */    /*E0256*/

              DimLower[j] = DimBlockPtr->Set[DDim].Lower;
              DimSize[j] = DimLower[j] + DimBlockPtr->Set[DDim].Size;
              MaxLower[j] = dvm_max(DimLower[j], PL->RecvInit[j]);
              DimSize[j] = dvm_min(DimSize[j], (PL->RecvLast[j] + 1)) -
                           MaxLower[j];
              tlong[j] = MaxLower[j] + DimSize[j] - 1;
           }
        }

        /* Initialization of local elements receiving into Out buffer */    /*E0257*/

        for(i=0; i < BBuf->ShdCount; i++)
        {  SB = &BBuf->ShdInfo[i];
           SB->ReqOut.BufLength = 0;

           if(SB->IsShdOut)
           {  Set = SB->BlockOut.Set;

              for(j=0; j < PL->QDimNumber; j++) /* */    /*E0258*/
              {
                 DDim = PL->DDim[j]; /* */    /*E0259*/
                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_dopl_))
                    tprintf("***4 InitAcrossRecv: AcrType2. NQ=%d "
                            "Lower=%ld Upper=%ld\n",
                            j, Set[DDim].Lower, Set[DDim].Upper);

                 if(DimWidthSign[j])
                 {  /* Edge is not complete */    /*E0260*/

                    if(DimSize[j] > 0)
                    {  SB->ReqOut.BufLength = 1;

                       Set[DDim].Lower = MaxLower[j];
                       Set[DDim].Upper = tlong[j];
                       Set[DDim].Size  = DimSize[j];
                    }
                 }
                 else
                 {  /* Edge is complete */    /*E0261*/

                    SB->ReqOut.BufLength = 1;

                    Set[DDim].Lower = PL->RecvInit[j];
                    Set[DDim].Upper = PL->RecvLast[j];
                    Set[DDim].Size  = PLSize[j];
                 }
              }

              if(SB->ReqOut.BufLength)
              {  block_GetSize(Size, &SB->BlockOut, Step)
                 Size *= (BBuf->ArrCount * SB->TLen);
                 wPtr  = (char *)SB->BufOut;

                 for(j=0; j < PL->QDimNumber; j++) /* */    /*E0262*/
                 {
                    DDim = PL->DDim[j]; /* */    /*E0263*/
                    if(RTL_TRACE && AcrossTrace &&
                       TstTraceEvent(call_dopl_))
                       tprintf("***5 InitAcrossRecv: AcrType2. NQ=%d "
                               "Lower=%ld Upper=%ld Size=%ld\n",
                               j, Set[DDim].Lower, Set[DDim].Upper,
                               Size);
                 }

                 if(RTL_TRACE)
                    ( RTL_CALL, shd_Recvnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqOut) );
                 else
                    ( RTL_CALL, rtl_Recvnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqOut, 0) );
              }
           }
        }

        BBuf->DoWaitOut = TRUE;

        TstHandlePtr =
        (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0264*/
     }
  }

  return;
}



void  WaitAcrossRecv(s_PARLOOP  *PL)
{ s_BOUNDGROUP   *BG;
  s_BOUNDBUF     *BBuf;
  SysHandle      *CurrHandlePtr;
  int             i, ArN;
  s_SHADOW       *SB;
  DvmType            Size;
  char           *wPtr;
  byte            Step = 0;

  if(PL->AcrType1)
  {  /* scheme <sendsh_ - recvsh_> is executing */    /*E0265*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup1Ref)->pP;
     CurrHandlePtr = BG->BufPtr;

     while(CurrHandlePtr)
     {  BBuf = (s_BOUNDBUF *)CurrHandlePtr->pP;

        if(BG->IsRecvsh) /* if receiving improted elements
                            has been started */    /*E0266*/
        {  if(BBuf->DoWaitIn)
           {  for(i=0; i < BBuf->ShdCount; i++)
              {  SB = &BBuf->ShdInfo[i];

                 if(SB->IsShdIn && SB->ReqIn.BufLength)
                 {  if(RTL_TRACE)
                       ( RTL_CALL, shd_Waitrequest(&SB->ReqIn) );
                    else
                       ( RTL_CALL, rtl_Waitrequest(&SB->ReqIn) );

                    block_GetSize(Size, &SB->BlockIn, Step)
                    Size *= SB->TLen;
                    wPtr = (char *)SB->BufIn;

                    for(ArN=0; ArN < BBuf->ArrCount; ArN++)
                    {  CopyMem2Block(&(BBuf->ArrList[ArN]->ArrBlock),
                                     wPtr, &SB->BlockIn);
                       wPtr += (int)Size;
                    }
                 }
              }
           }

           BBuf->DoWaitIn = FALSE;
        }

        CurrHandlePtr =
        (SysHandle *)CurrHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0267*/
     }

     BG->IsRecvsh = 0; /* flag off: (receiving imported elements) */    /*E0268*/
  }

  if(PL->AcrType2)
  {  /* scheme <sendsa_ - recvla_> is executing */    /*E0269*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup2Ref)->pP;
     CurrHandlePtr = BG->BufPtr;

     while(CurrHandlePtr)
     {  BBuf = (s_BOUNDBUF *)CurrHandlePtr->pP;

        if(BG->IsRecvla) /* if receiving local elements
                            has been started */    /*E0270*/
        {  if(BBuf->DoWaitOut)
           {  for(i=0; i < BBuf->ShdCount; i++)
              {  SB = &BBuf->ShdInfo[i];

                 if(SB->IsShdOut && SB->ReqOut.BufLength)
                 {  if(RTL_TRACE)
                       ( RTL_CALL, shd_Waitrequest(&SB->ReqOut) );
                    else
                       ( RTL_CALL, rtl_Waitrequest(&SB->ReqOut) );

                    block_GetSize(Size, &SB->BlockOut, Step)
                    Size *= SB->TLen;
                    wPtr = (char *)SB->BufOut;

                    for(ArN=0; ArN < BBuf->ArrCount; ArN++)
                    {  CopyMem2Block(&(BBuf->ArrList[ArN]->ArrBlock),
                                     wPtr, &SB->BlockOut);
                       wPtr += (int)Size;
                    }
                 }
              }
           }

           BBuf->DoWaitOut = FALSE;
        }

        CurrHandlePtr =
        (SysHandle *)CurrHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0271*/
     }

     BG->IsRecvla = 0; /* flag off: (receiving local elements) */    /*E0272*/
  }

  return;
}



void  InitAcrossSend(s_PARLOOP  *PL)
{ s_BOUNDGROUP   *BG;
  s_BOUNDBUF     *BBuf;
  SysHandle      *TstHandlePtr;
  int             i, j, ArN, DDim;
  char           *wPtr;
  DvmType            Size;
  DvmType            DimLower[MAXARRAYDIM], DimSize[MAXARRAYDIM],
                  MaxLower[MAXARRAYDIM], tlong[MAXARRAYDIM],
                  PLSize[MAXARRAYDIM];
  s_SHADOW       *SB;
  byte            Step = 0, DimWidthSign[MAXARRAYDIM];
  s_REGULARSET   *Set;
  s_BLOCK        *DimBlockPtr;

  for(j=0; j < PL->QDimNumber; j++)  /* */    /*E0273*/
  {
     DDim = PL->DDim[j]; /* number of array dimension
                      for extracting from shadow edge - 1 */    /*E0274*/
      
     if(PL->CurrInit[j] > PL->CurrLast[j])
     {  PL->SendInit[j] = PL->CurrLast[j] - PL->LowIndex[j];
        PL->SendLast[j] = PL->CurrInit[j] - PL->LowIndex[j];
     }
     else
     {  PL->SendInit[j] = PL->CurrInit[j] - PL->LowIndex[j];
        PL->SendLast[j] = PL->CurrLast[j] - PL->LowIndex[j];
     }

     PLSize[j] = PL->SendLast[j] - PL->SendInit[j] + 1;

     if(RTL_TRACE && AcrossTrace && TstTraceEvent(call_dopl_))
        tprintf("***1 InitAcrossSend: NQ=%d QDim=%d PLLowIndex=%ld "
                "SendInit=%ld SendLast=%ld DDim=%d\n",
                j, PL->QDim[j], PL->LowIndex[j], PL->SendInit[j],
                PL->SendLast[j], DDim);
  }

  if(PL->AcrType1)
  {  /* scheme <sendsh_ - recvsh_> is executing */    /*E0275*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup1Ref)->pP;
     BG->IsSendsh = 1;   /* flag: sending exported elements */    /*E0276*/

     /* Start of sending exported elements for all edge buffers */    /*E0277*/

     TstHandlePtr = BG->BufPtr;

     while(TstHandlePtr)
     {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
        DimBlockPtr = &BBuf->DimBlock;

        for(j=0; j < PL->QDimNumber; j++) /* */    /*E0278*/
        {
           DDim = PL->DDim[j]; /* */    /*E0279*/
           DimWidthSign[j] = BBuf->DimWidthSign[DDim];

           if(DimWidthSign[j])
           {  /* Edge of quanted dimension is not complete */    /*E0280*/

              DimLower[j] = DimBlockPtr->Set[DDim].Lower;
              DimSize[j] = DimLower[j] + DimBlockPtr->Set[DDim].Size;
              MaxLower[j] = dvm_max(DimLower[j], PL->SendInit[j]);
              DimSize[j] = dvm_min(DimSize[j], (PL->SendLast[j] + 1)) -
                           MaxLower[j];
              tlong[j] = MaxLower[j] + DimSize[j] - 1;
           }
        }

        /* Rewrite edges into buffer and initialize sending */    /*E0281*/

        for(i=0; i < BBuf->ShdCount; i++)
        {  SB = &BBuf->ShdInfo[i];
           SB->ReqOut.BufLength = 0;

           if(SB->IsShdOut)
           {  /* Rewrite edges into the sending buffer */    /*E0282*/

              Set = SB->BlockOut.Set;

              for(j=0; j < PL->QDimNumber; j++) /* */    /*E0283*/
              {
                 DDim = PL->DDim[j]; /* */    /*E0284*/
                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_dopl_))
                    tprintf("***2 InitAcrossSend: AcrType1. NQ=%d "
                            "Lower=%ld Upper=%ld\n",
                            j, Set[DDim].Lower, Set[DDim].Upper);

                 if(DimWidthSign[j])
                 {  /* Edge is not complete */    /*E0285*/

                    if(DimSize[j] > 0)
                    {  SB->ReqOut.BufLength = 1;

                       Set[DDim].Lower = MaxLower[j];
                       Set[DDim].Upper = tlong[j];
                       Set[DDim].Size  = DimSize[j];
                    }
                 }
                 else
                 {  /* Edge is complete */    /*E0286*/

                    SB->ReqOut.BufLength = 1;

                    Set[DDim].Lower = PL->SendInit[j];
                    Set[DDim].Upper = PL->SendLast[j];
                    Set[DDim].Size  = PLSize[j];
                 }
              }

              if(SB->ReqOut.BufLength)
              {  block_GetSize(Size, &SB->BlockOut, Step)
                 Size *= SB->TLen;
                 wPtr = (char *)SB->BufOut;

                 for(ArN=0; ArN < BBuf->ArrCount; ArN++)
                 {  CopyBlock2Mem(wPtr, &SB->BlockOut,
                                  &(BBuf->ArrList[ArN]->ArrBlock));
                    wPtr += (int)Size;
                 }

                 /* Initialization of edge sending */    /*E0287*/

                 Size *= BBuf->ArrCount;
                 wPtr = (char *)SB->BufOut;

                 for(j=0; j < PL->QDimNumber; j++) /* */    /*E0288*/
                 {
                    DDim = PL->DDim[j]; /* */    /*E0289*/

                    if(RTL_TRACE && AcrossTrace &&
                       TstTraceEvent(call_dopl_))
                       tprintf("***3 InitAcrossSend: AcrType1. NQ=%d "
                               "Lower=%ld Upper=%ld Size=%ld\n",
                               j, Set[DDim].Lower, Set[DDim].Upper,
                               Size);
                 }

                 if(RTL_TRACE)
                    ( RTL_CALL, shd_Sendnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqOut, AcrossOut) );
                 else 
                    ( RTL_CALL, rtl_Sendnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqOut, AcrossOut) );
              }
           }
        }

        BBuf->DoWaitOut = TRUE;

        TstHandlePtr =
        (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0290*/
     }
  }

  if(PL->AcrType2)
  {  /* scheme <sendsa_ - recvla_> is executing */    /*E0291*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup2Ref)->pP;
     BG->IsSendsa = 1;  /* flag: sending edge elements */    /*E0292*/

     /* Start of sending edge elements for all edge buffers */    /*E0293*/

     TstHandlePtr = BG->BufPtr;

     while(TstHandlePtr)
     {  BBuf = (s_BOUNDBUF *)TstHandlePtr->pP;
        DimBlockPtr = &BBuf->DimBlock;

        for(j=0; j < PL->QDimNumber; j++) /* */    /*E0294*/
        {
           DDim = PL->DDim[j]; /* */    /*E0295*/
           DimWidthSign[j] = BBuf->DimWidthSign[DDim];

           if(DimWidthSign[j])
           {  /* Edge of quanted dimension is not complete */    /*E0296*/

              DimLower[j] = DimBlockPtr->Set[DDim].Lower;
              DimSize[j] = DimLower[j] + DimBlockPtr->Set[DDim].Size;
              MaxLower[j] = dvm_max(DimLower[j], PL->SendInit[j]);
              DimSize[j] = dvm_min(DimSize[j], (PL->SendLast[j] + 1)) -
                           MaxLower[j];
              tlong[j] = MaxLower[j] + DimSize[j] - 1;
           }
        }

        /* Rewrite edges into In buffer and initialize sending to
           neighboring processors */    /*E0297*/

        for(i=0; i < BBuf->ShdCount; i++)
        {  SB = &BBuf->ShdInfo[i];
           SB->ReqIn.BufLength = 0;

           if(SB->IsShdIn)
           {  /* Rewrite edges into the sending buffer */    /*E0298*/

              Set = SB->BlockIn.Set;

              for(j=0; j < PL->QDimNumber; j++) /* */    /*E0299*/
              {
                 DDim = PL->DDim[j]; /* */    /*E0300*/

                 if(RTL_TRACE && AcrossTrace &&
                    TstTraceEvent(call_dopl_))
                    tprintf("***4 InitAcrossSend: AcrType2. NQ=%d "
                            "Lower=%ld Upper=%ld\n",
                            j, Set[DDim].Lower, Set[DDim].Upper);
       

                 if(DimWidthSign[j])
                 {  /* Edge is not complete */    /*E0301*/

                    if(DimSize[j] > 0)
                    {  SB->ReqIn.BufLength = 1;

                       Set[DDim].Lower = MaxLower[j];
                       Set[DDim].Upper = tlong[j];
                       Set[DDim].Size  = DimSize[j];
                    }
                 }
                 else
                 {  /* Edge is complete */    /*E0302*/

                    SB->ReqIn.BufLength = 1;

                    Set[DDim].Lower = PL->SendInit[j];
                    Set[DDim].Upper = PL->SendLast[j];
                    Set[DDim].Size  = PLSize[j];
                 }
              }

              if(SB->ReqIn.BufLength)
              {  block_GetSize(Size, &SB->BlockIn, Step)
                 Size *= SB->TLen;
                 wPtr = (char *)SB->BufIn;

                 for(ArN=0; ArN < BBuf->ArrCount; ArN++)
                 {  CopyBlock2Mem(wPtr, &SB->BlockIn,
                                  &(BBuf->ArrList[ArN]->ArrBlock));
                    wPtr += (int)Size;
                 }

                 /* Initialization of edge sending */    /*E0303*/

                 Size *= BBuf->ArrCount;
                 wPtr = (char *)SB->BufIn;

                 for(j=0; j < PL->QDimNumber; j++) /* */    /*E0304*/
                 {
                    DDim = PL->DDim[j]; /* */    /*E0305*/
                    if(RTL_TRACE && AcrossTrace &&
                       TstTraceEvent(call_dopl_))
                       tprintf("***5 InitAcrossSend: AcrType2. NQ=%d "
                               "Lower=%ld Upper=%ld Size=%ld\n",
                               j, Set[DDim].Lower, Set[DDim].Upper,
                               Size);
                 }

                 if(RTL_TRACE)
                    ( RTL_CALL, shd_Sendnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqIn, AcrossIn) );
                 else 
                    ( RTL_CALL, rtl_Sendnowait(wPtr, 1, (int)Size,
                                               (int)SB->Proc,
                                               DVM_VMS->tag_BoundsBuffer,
                                               &SB->ReqIn, AcrossIn) );
              }
           }
        }

        BBuf->DoWaitIn = TRUE;

        TstHandlePtr =
        (SysHandle *)TstHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0306*/
     }
  }

  if(MsgSchedule && UserSumFlag)
  {  rtl_TstReqColl(0);

     if(ASynchrPipeLine == 0) 
        rtl_SendReqColl(1.0);
     else
     {  if(PL->AcrType1 && PL->AcrType2)
           rtl_SendReqColl(ResCoeffAcross);
        else
           rtl_SendReqColl(0.);
     }
  }

  return;
}



void  WaitAcrossSend(s_PARLOOP  *PL)
{ s_BOUNDGROUP   *BG;
  s_BOUNDBUF     *BBuf;
  SysHandle      *CurrHandlePtr;

  if(PL->AcrType1)
  {  /* scheme <sendsh_ - recvsh_> is executing */    /*E0307*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup1Ref)->pP;
     CurrHandlePtr = BG->BufPtr;

     while(CurrHandlePtr)
     {  BBuf = (s_BOUNDBUF *)CurrHandlePtr->pP;

        if(BG->IsSendsh) /* if sending exproted elements
                            has been started */    /*E0308*/
           bbuf_WSendComplete(BBuf);

        CurrHandlePtr =
        (SysHandle *)CurrHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0309*/
     }

     BG->IsSendsh = 0;   /* flag off: (sending exported elements) */    /*E0310*/
  }

  if(PL->AcrType2)
  {  /* scheme <sendsa_ - recvla_> is executing */    /*E0311*/

     BG = (s_BOUNDGROUP *)((SysHandle *)PL->NewShadowGroup2Ref)->pP;
     CurrHandlePtr = BG->BufPtr;

     while(CurrHandlePtr)
     {  BBuf = (s_BOUNDBUF *)CurrHandlePtr->pP;

        if(BG->IsSendsa) /* if sending edge elements
                            has been started */    /*E0312*/
           bbuf_WSendsaComplete(BBuf);

        CurrHandlePtr =
        (SysHandle *)CurrHandlePtr->NextHandlePtr; /* to the next
                                                       edge buffer */    /*E0313*/
     }

     BG->IsSendsa = 0;   /* flag off: (sending edge elements) */    /*E0314*/
  }

  return;
}


/* ----------------------------------------------------- */    /*E0315*/


void bgroup_Done(s_BOUNDGROUP *BGPtr)
{ SysHandle       *CurrHandlePtr, *NextPtr;
  s_BOUNDBUF      *DescPtr;
  ShadowGroupRef   ShdGrpRef;
  s_DISARRAY      *DArr;
  int              i, j;
  s_SHDWIDTH      *ShdWidth;

  if(ShgSave)
  { (DVM_RET);
    return;    /* */    /*E0316*/
  }

  if(RTL_TRACE)
     dvm_trace(call_bgroup_Done,"ShadowGroupRef=%lx;\n",
                                 (uLLng)BGPtr->HandlePtr);

  if(EnableDynControl)
  {
     dyn_DisArrShadowGroupDeleted(BGPtr);
  }

  /* Completion of edge exchange or of receiving (sending)
     imported (exported) elements */    /*E0317*/

  if(BGPtr->IsStrtsh || BGPtr->IsSendsh || BGPtr->IsRecvsh ||
     BGPtr->IsSendsa || BGPtr->IsRecvla)
  {  ShdGrpRef = (ShadowGroupRef)BGPtr->HandlePtr;
     ( RTL_CALL, waitsh_(&ShdGrpRef) );
  }

  /* Delete all edge subgroups */    /*E0318*/

  CurrHandlePtr = BGPtr->BufPtr;

  while(CurrHandlePtr)
  {  NextPtr = (SysHandle *)CurrHandlePtr->NextHandlePtr;
     DescPtr = (s_BOUNDBUF *)CurrHandlePtr->pP;
     ( RTL_CALL, bbuf_Done(DescPtr) );
     dvm_FreeStruct(DescPtr);
     CurrHandlePtr = NextPtr;
  }

  /* Delete all arrays from deleted edge group */    /*E0319*/

  for(i=0; i < BGPtr->ArrayColl.Count; i++)
  {  DArr = coll_At(s_DISARRAY *, &BGPtr->ArrayColl, i);

     while( (j = coll_IndexOf(&DArr->BG, BGPtr)) >= 0)
     {  coll_AtDelete(&DArr->BG, j);

        ShdWidth = coll_At(s_SHDWIDTH *, &DArr->ResShdWidthColl, j);
        dvm_FreeStruct(ShdWidth);
     coll_AtDelete(&DArr->ResShdWidthColl, j);
     }
  }

  dvm_FreeArray(BGPtr->ArrayColl.List);
  dvm_FreeArray(BGPtr->NewArrayColl.List);
  dvm_FreeArray(BGPtr->NewShdWidthColl.List);

  if(TstObject)
     DelDVMObj((ObjectRef)BGPtr->HandlePtr);

  /* */    /*E0320*/

  if(BGPtr->ResetSign == 0)
  {  /* */    /*E0321*/

     BGPtr->HandlePtr->Type = sht_NULL;
     dvm_FreeStruct(BGPtr->HandlePtr);
  }

  BGPtr->ResetSign = 0;
  
  if(RTL_TRACE)
     dvm_trace(ret_bgroup_Done," \n");

  (DVM_RET);
  return;
}



void bbuf_Done(s_BOUNDBUF *BBuf)
{ int          i;
  s_SHADOW    *SB;

  if(RTL_TRACE)
     dvm_trace(call_bbuf_Done,"BoundBufferRef=%lx;\n",
                               (uLLng)BBuf->HandlePtr);

  dvm_FreeArray(BBuf->ArrList);

  for(i=0; i < BBuf->ShdCount; i++)
  {  SB = &BBuf->ShdInfo[i];

     mac_free(&SB->BufIn);
     mac_free(&SB->BufOut);
  }

  dvm_FreeArray(BBuf->ShdInfo); 

  BBuf->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(BBuf->HandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_bbuf_Done," \n");

  (DVM_RET);

  return;
}

/***********************************************\
* Initialize structures for shadow edge renewal *
\***********************************************/    /*E0322*/

void  shd_init(s_SHDWIDTH  *ShdWidth, SysHandle  *BoundBufferPtr)
{ s_BOUNDBUF   *BB;
  s_DISARRAY   *DA;
  int           VMDim[MAXARRAYDIM], ArDir[MAXARRAYDIM];
  int           AxisArray[MAXARRAYDIM];
  int           i, j, ShdSign, Rank, tint;
  s_VMS        *VMS;
  s_SPACE      *VMSpace, *ASpace;
  DvmType         *ProcSI;
  int           AxisVector1, AxisVector2, FinVector1, FinVector2;
  int           AxisCount;
  DvmType          BSize, tlong;
  byte          Step = 0, DimWidthSign = 0;
  byte         *DimWidthSignPtr;
  s_BLOCK      *DimBlockPtr;
  s_REGULARSET *Set;
  s_SHADOW     *ShdInfo;

  BB  = (s_BOUNDBUF *)BoundBufferPtr->pP;
  DA  = BB->ArrList[0];

  if(DA->HasLocal == 0)
     return;

  Rank = DA->Space.Rank;

  for(i=0,j=0; i < Rank; i++)
      if((VMDim[i] = GetArMapDim(DA, i+1, &ArDir[i])) > 0)
         j = 1;

  if(j == 0)
     return;

  VMS     = DA->AMView->VMS; /* processor system on which
                                the array is mapped */    /*E0323*/
  VMSpace = &VMS->Space;
  ASpace  = &DA->Space;

  dvm_AllocArray(DvmType, (VMS->CVP)[0] + 1, ProcSI);
  FinVector1 = 1 << Rank;
  dvm_AllocArray(s_SHADOW, FinVector1*FinVector1, BB->ShdInfo);

  /* Create block each dimension of which consists of
     allowable coordinate segments */    /*E0324*/

  DimBlockPtr = &BB->DimBlock;
  DimWidthSignPtr = BB->DimWidthSign;

  for(i=0; i < Rank; i++)
  {  if(ShdWidth->InitDimIndex[i] != 0 ||
        ShdWidth->DimWidth[i] != ASpace->Size[i])
     {  DimWidthSign = 1;
        DimWidthSignPtr[i] = 1;
     }
  }

  if(DimWidthSign)
  {  DimBlockPtr->Rank = (byte)Rank;
     Set = DimBlockPtr->Set;

     for(i=0; i < Rank; i++)
     {  BSize = ShdWidth->DimWidth[i];
        tlong = ShdWidth->InitDimIndex[i];

        Set[i].Lower = tlong;
        Set[i].Upper = tlong + BSize - 1;
        Set[i].Size  = BSize;
        Set[i].Step  = 1;
     }
  }

  /* Loop in vectors of dimensions involved */    /*E0325*/

  for(AxisVector1=1; AxisVector1 < FinVector1; AxisVector1++)
  {  AxisCount = 0;

     for(i=0,j=Rank-1; i < Rank; i++,j--)
     {   ShdSign = ShdWidth->ShdSign[i];

         if( (AxisVector1 >> j) & 0x1 )
         {  if(ShdSign == 1)
               break; /* dimension should not take part
                         in assembling of edges */    /*E0326*/
            AxisArray[AxisCount] = i;
            AxisCount++;
         }
         else
         {  if((ShdSign & 0x1) == 0)
               break; /* the dimension cannot be changed
                         in the frame of the local part */    /*E0327*/
         }
     }

     if(i < Rank)
        continue; /* some dimension of current vector should not take
                     part in assembling of edges */    /*E0328*/
     if(AxisCount > ShdWidth->MaxShdCount)
        continue; /* number of  dimensions taking part in assembling
                     of edges exceeds maximum */    /*E0329*/

     FinVector2 = 1 << AxisCount;

     /* Loop in vectors of edge dimensions of  
        current vector of dimensions involved */    /*E0330*/

     for(AxisVector2=0; AxisVector2 < FinVector2; AxisVector2++)
     {  ShdInfo = &BB->ShdInfo[BB->ShdCount];

        ShdInfo->IsShdIn  = 1;
        ShdInfo->IsShdOut = 1;
        ShdInfo->BlockIn  = block_Copy(&DA->Block);
        ShdInfo->BlockOut = block_Copy(&DA->Block);
        ShdInfo->BufIn    = NULL;
        ShdInfo->BufOut   = NULL;
        ShdInfo->TLen     = DA->TLen;

        dvm_memcopy(ProcSI, VMS->CVP, sizeof(DvmType)*((VMS->CVP)[0] + 1));

        /* Loop in current vector of edge dimensions */    /*E0331*/
        
        for(i=0; i < AxisCount; i++)
        {  j = AxisArray[i];  /* dimension number minus 1 */    /*E0332*/
           ShdSign = (ShdWidth->ShdSign[j]) >> 1;

           if( (AxisVector2 >> (AxisCount-i-1)) & 0x1 )
           {  /* Edge of high end in j+1 dimension is needed */    /*E0333*/

              if(DA->Block.Set[j].Upper+1 >= ASpace->Size[j])
                 break;  /* no right neighbor */    /*E0334*/

              if(VMDim[j] > 0)
                 ProcSI[VMDim[j]] = VMS->CVP[VMDim[j]] + ArDir[j];

              tint = ShdWidth->ResHighShdWidth[j];

              if(tint > 0 && (ShdSign == 2 || ShdSign == 3))
              {  /* Edge of high end in j+1 dimension exists */    /*E0335*/

                 BSize = DA->Block.Set[j].Upper +
                         ShdWidth->InitHiShdIndex[j];

                 Set = ShdInfo->BlockIn.Set;

                 Set[j].Lower = BSize + 1;
                 Set[j].Upper = BSize + tint;
                 Set[j].Size  = tint;
                 Set[j].Step  = 1;
              }
              else
              {  /* No edge of high end in j+1 dimension */    /*E0336*/

                 ShdInfo->IsShdIn = 0;
              }

              tint = ShdWidth->ResLowShdWidth[j];

              if(tint > 0 && (ShdSign == 1 || ShdSign == 3))
              {  /* Edge of low end in j+1 dimension exists */    /*E0337*/

                 BSize = DA->Block.Set[j].Upper -
                         DA->InitLowShdWidth[j] +
                         ShdWidth->InitLowShdIndex[j];

                 Set = ShdInfo->BlockOut.Set;

                 Set[j].Lower = BSize + 1;
                 Set[j].Upper = BSize + tint;
                 Set[j].Size  = tint;
                 Set[j].Step  = 1;
              }
              else
              {  /* No edge of low end in j+1 dimension */    /*E0338*/

                 ShdInfo->IsShdOut = 0;
              } 
           } 
           else
           {  /* Edge of low end in j+1 dimension  is needed */    /*E0339*/

              if(DA->Block.Set[j].Lower <= 0)
                 break;   /* no left neighbor */    /*E0340*/

              if(VMDim[j] > 0)
                 ProcSI[VMDim[j]] = VMS->CVP[VMDim[j]] - ArDir[j];

              tint = ShdWidth->ResHighShdWidth[j];

              if(tint > 0 && (ShdSign == 2 || ShdSign == 3))
              {  /* Edge of high end in j+1 dimension exists */    /*E0341*/

                 BSize = DA->Block.Set[j].Lower +
                         ShdWidth->InitHiShdIndex[j];

                 Set = ShdInfo->BlockOut.Set;

                 Set[j].Lower = BSize;
                 Set[j].Upper = BSize + tint - 1;
                 Set[j].Size  = tint;
                 Set[j].Step  = 1;
              }
              else
              {  /* No edge of high end in j+1 dimension */    /*E0342*/

                 ShdInfo->IsShdOut = 0;
              }

              tint = ShdWidth->ResLowShdWidth[j];

              if(tint > 0 && (ShdSign == 1 || ShdSign == 3))
              {  /* Edge of low end in j+1 dimension exists */    /*E0343*/

                 BSize = DA->Block.Set[j].Lower -
                         DA->InitLowShdWidth[j] +
                         ShdWidth->InitLowShdIndex[j];

                 Set = ShdInfo->BlockIn.Set;

                 Set[j].Lower = BSize;
                 Set[j].Upper = BSize + tint - 1;
                 Set[j].Size  = tint;
                 Set[j].Step  = 1;
              }
              else
              {  /* No edge of low end in j+1 dimension */    /*E0344*/

                 ShdInfo->IsShdIn = 0;
              }
           } 
        } 

        if(i < AxisCount)
           continue; /* there is no new edge component */    /*E0345*/

        /* Check intersection with the block
           of allowable coordinate segments */    /*E0346*/

        if(DimWidthSign)
        {  if(ShdInfo->IsShdIn)
           {  i = BlockIntersect(&ShdInfo->BlockIn, &ShdInfo->BlockIn,
                                 DimBlockPtr);
              if(i == 0)
                 ShdInfo->IsShdIn = 0;
           }

           if(ShdInfo->IsShdOut)
           {  i = BlockIntersect(&ShdInfo->BlockOut, &ShdInfo->BlockOut,
                                 DimBlockPtr);
              if(i == 0)
                 ShdInfo->IsShdOut = 0;
           }
        }

        /* -------------------------------- */    /*E0347*/

        if(ShdInfo->IsShdIn == 0 && ShdInfo->IsShdOut == 0)
           continue;

        ShdInfo->Proc = VMS->VProc[ space_GetLI(VMSpace, ProcSI) ].lP;

        if(ShdInfo->IsShdIn)
        {  block_GetSize(BSize, &ShdInfo->BlockIn, Step)
           BSize *= (DA->TLen * BB->Count);
           mac_malloc(ShdInfo->BufIn, void *, BSize, 0);
        }

        if(ShdInfo->IsShdOut)
        { block_GetSize(BSize, &ShdInfo->BlockOut, Step)
          BSize *= (DA->TLen * BB->Count);
          mac_malloc(ShdInfo->BufOut, void *, BSize, 0);
        }
        
        BB->ShdCount++;
     }
  }

  dvm_FreeArray(ProcSI);

  return;
}
                 

/****************************************************************\
*   Function returns the dimension number of virtual subsystem   *
*        on which the given array dim ension is mapped.          *
*     If the array dimension is replicated in all directions     *
*         of virtual processor matrix then 0 is returned.        *
*                                                                *
*     The variable Dir is assigned with 1 or -1 depending        *
*        on the direction of array dimension splitting.          *
\****************************************************************/    /*E0348*/

int GetArMapDim(s_DISARRAY *Ar, int ArDim, int *Dir)
{ int          Res = 0;
  s_ALIGN     *Align;
  s_MAP       *Map;
  s_AMVIEW    *AMV;
  int          AMSDim;

  Align = &Ar->Align[ArDim-1];

  if(Align->Attr == align_NORMAL)
  {  AMSDim = Align->TAxis;
     AMV    = Ar->AMView;
     *Dir   = dvm_sign(Align->A);
     Map    = &AMV->DISTMAP[AMSDim-1];

     if(Map->Attr == map_BLOCK)
        Res = Map->PAxis;
  }

  return  Res;
}


/****************************************************************\
*   Function returns the dimension number of virtual subsystem   *
*        on which the given array dim ension is mapped.          *
*     If the array dimension is replicated in all directions     *
*         of virtual processor matrix then 0 is returned.        *
\****************************************************************/    /*E0349*/

int GetDAMapDim(s_DISARRAY *Ar, int ArDim)
{ int          Res = 0;
  s_ALIGN     *Align;
  s_MAP       *Map;
  s_AMVIEW    *AMV;
  int          AMSDim;

  Align = &Ar->Align[ArDim-1];

  if(Align->Attr == align_NORMAL)
  {  AMSDim = Align->TAxis;
     AMV    = Ar->AMView;
     Map    = &AMV->DISTMAP[AMSDim-1];

     if(Map->Attr == map_BLOCK)
        Res = Map->PAxis;
  }

  return  Res;
}


/***************************************************************\
* Check if all arrays from the edge group are mapped on         *
* processor systems, each element of which belongs to           *
* the current processor system.                                 *
* Returns pointer to the Handle of the first array              *
* not sutisfying this condition or NULL.                        *  
\***************************************************************/    /*E0350*/

SysHandle  *CheckShadowArrayVMS(SysHandle  *BGHandlePtr)
{ s_BOUNDGROUP  *BGPtr;
  s_DISARRAY    *DA;
  s_VMS         *VMS;
  int            i, Not = 0;

  BGPtr = (s_BOUNDGROUP *)BGHandlePtr->pP;

  for(i=0; i < BGPtr->ArrayColl.Count; i++)
  {  DA = coll_At(s_DISARRAY *, &BGPtr->ArrayColl, i);
     VMS = DA->AMView->VMS;  /* processor system on which 
                                the array is mapped*/    /*E0351*/
     NotSubsystem(Not, DVM_VMS, VMS)
     if(Not)
        break;
  }

  if(Not)
     return  DA->HandlePtr;
  else
     return  NULL;
}


/*************************************************\
* Check if edges of all arrays from               *
* the given edge group consist of shadow edges only*
*                                                 *
* Return value:          1 - yes;                 *
*                        0 - no.                  *
\*************************************************/    /*E0352*/

int  CheckShdEdge(s_BOUNDGROUP  *BG)
{ int            i, j, k, ArrayCount, BGCount, Rank, AxisCount, Res = 1;
  s_DISARRAY    *DArr;
  s_SHDWIDTH    *ShdWidth;
  s_BOUNDGROUP  *CurrBG;

  s_COLLECTION  *ColPtr;

  AxisCount = 0; /* */    /*E0353*/

  ArrayCount = BG->ArrayColl.Count; /* number of arrays in edge group */    /*E0354*/

  for(i=0; i < ArrayCount; i++)
  {  ColPtr = &BG->ArrayColl;
     DArr = coll_At(s_DISARRAY *, ColPtr, i);

     BGCount = DArr->BG.Count; /* number of groups
                                  the current array is included in */    /*E0355*/

     for(k=0; k < BGCount; k++)
     {  ColPtr = &DArr->ResShdWidthColl;
        ShdWidth = coll_At(s_SHDWIDTH *, ColPtr, k);

        if(ShdWidth->UseSign)
           continue;           /* */    /*E0356*/

        ShdWidth->UseSign = 1; /* */    /*E0357*/

        ColPtr = &DArr->BG;
        CurrBG = coll_At(s_BOUNDGROUP *, ColPtr, k);

        if(CurrBG != BG)
           continue;  /* the given group is not current */    /*E0358*/

        if(ShdWidth->MaxShdCount < 2)
           continue; /* array edge consists of 
                        shadow edges only */    /*E0359*/

        Rank = DArr->Space.Rank;

        AxisCount = 0;

        for(j=0; j < Rank; j++)
        {  switch(ShdWidth->ShdSign[j])
           {  case 2:
              case 3:  if(ShdWidth->ResLowShdWidth[j])
                          AxisCount++;
                       break;
              case 4:
              case 5:  if(ShdWidth->ResHighShdWidth[j])
                          AxisCount++;
                       break;
              case 6:
              case 7:  if(ShdWidth->ResLowShdWidth[j] ||
                          ShdWidth->ResHighShdWidth[j])
                          AxisCount++;
                       break;
              default: break;
           }
        }

        if(AxisCount > 1)
           break;  /* array edge consists not only of 
                        shadow edges  */    /*E0360*/
     }

     if(AxisCount > 1)
     {  Res = 0;
        break;  /* array edge consists not only of 
                        shadow edges  */    /*E0361*/
     }
  }

  /* */    /*E0362*/

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

  return  Res;
}


/* */    /*E0363*/

int  CheckBGAMView(s_BOUNDGROUP  *BG, s_AMVIEW  *AMView)
{ int          i, ArrayCount;
  s_DISARRAY  *DArr;

  ArrayCount = BG->ArrayColl.Count; /* */    /*E0364*/

  for(i=0; i < ArrayCount; i++)
  {  DArr = coll_At(s_DISARRAY *, &BG->ArrayColl, i);

     if(DArr->AMView != AMView)
        break;
  }
 
  return  (int)(i == ArrayCount);
}


/* */    /*E0365*/

int  GetBGAxis(s_BOUNDGROUP  *BG, int  *AxisArray, int  *OutDirArray)
{ int             i, j, k, n, ArrayCount, BGCount, Rank, AxisCount = 0,
                  AMVAxis;
  s_DISARRAY     *DArr;
  s_SHDWIDTH     *ShdWidth;
  s_ALIGN        *Align;
  s_BOUNDGROUP   *CurrBG;
  s_COLLECTION   *ColPtr;

  ArrayCount = BG->ArrayColl.Count; /* */    /*E0366*/

  for(i=0; i < ArrayCount; i++)
  {  ColPtr = &BG->ArrayColl;
     DArr = coll_At(s_DISARRAY *, ColPtr, i);

     Rank = DArr->Space.Rank;
     Align = DArr->Align;

     BGCount = DArr->BG.Count; /* */    /*E0367*/

     for(k=0; k < BGCount; k++)
     {  ColPtr = &DArr->ResShdWidthColl;
        ShdWidth = coll_At(s_SHDWIDTH *, ColPtr, k);

        if(ShdWidth->UseSign)
           continue;  /* */    /*E0368*/

        ShdWidth->UseSign = 1; /* */    /*E0369*/

        ColPtr = &DArr->BG;
        CurrBG = coll_At(s_BOUNDGROUP *, ColPtr, k);

        if(CurrBG != BG)
           continue;  /* */    /*E0370*/

        for(j=0; j < Rank; j++)
        {  if(Align[j].Attr != align_NORMAL)
              continue; /* */    /*E0371*/

           AMVAxis = Align[j].TAxis;

           switch(ShdWidth->ShdSign[j])
           {
              case 2:
              case 3:  if(ShdWidth->ResLowShdWidth[j] == 0)
                          continue; /* */    /*E0372*/

                       for(n=0; n < AxisCount; n++)
                           if(AxisArray[n] == AMVAxis &&
                              OutDirArray[n] == -1)
                              break;

                       if(n == AxisCount)
                       {  AxisArray[n] = AMVAxis;
                          OutDirArray[n] = -1;
                          AxisCount++;
                       }

                       continue; 

              case 4:
              case 5:  if(ShdWidth->ResHighShdWidth[j] == 0)
                          continue; /* */    /*E0373*/

                       for(n=0; n < AxisCount; n++)
                           if(AxisArray[n] == AMVAxis &&
                              OutDirArray[n] == 1)
                              break;

                       if(n == AxisCount)
                       {  AxisArray[n] = AMVAxis;
                          OutDirArray[n] = 1;
                          AxisCount++;
                       }

                       continue; 

              case 6:
              case 7:  if(ShdWidth->ResLowShdWidth[j])
                       {  /* */    /*E0374*/

                          for(n=0; n < AxisCount; n++)
                              if(AxisArray[n] == AMVAxis &&
                                 OutDirArray[n] == -1)
                                 break;

                          if(n == AxisCount)
                          {  AxisArray[n] = AMVAxis;
                             OutDirArray[n] = -1;
                             AxisCount++;
                          }
                       }

                       if(ShdWidth->ResHighShdWidth[j])
                       {  /* */    /*E0375*/

                          for(n=0; n < AxisCount; n++)
                              if(AxisArray[n] == AMVAxis &&
                                 OutDirArray[n] == 1)
                                 break;

                          if(n == AxisCount)
                          {  AxisArray[n] = AMVAxis;
                             OutDirArray[n] = 1;
                             AxisCount++;
                          }
                       }

                       continue;

              default: continue;
           }
        }
     }
  }

  /* */    /*E0376*/

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

  return  AxisCount;
}



/****************************************************************\
*  Check if the edge of each array from the edge group           *
* is unidirectional shadow edge for all arrays                   *
* and if the edge direction is mapped on the same dimension of   *
* abstract machine representation for all arrays                 *
*                                                                * 
* Return values:         AM representation dimension number,     *
*                        for which pipeline is possible          *
*                        or 0,                                   *
* *OutDir - direction (-1 or 1) of exceeding the bounds          *
* of the local part                                              *
*           or 0,                                                *
* *BGAMView - pointer to AM representation or NULL               *
\****************************************************************/    /*E0377*/

int  CheckSingleShdEdge(s_BOUNDGROUP  *BG, s_AMVIEW  **BGAMView,
                        int  *OutDir)
{ int             i, j, k, ArrayCount, BGCount, Rank, AxisCount = 0,
                  OutAxis = 0, AMVAxis = 0;
  s_DISARRAY     *DArr;
  s_SHDWIDTH     *ShdWidth;
  s_ALIGN        *Align;
  s_AMVIEW       *AMView = NULL;
  s_BOUNDGROUP   *CurrBG;
  s_COLLECTION   *ColPtr;

  ArrayCount = BG->ArrayColl.Count; /* number of arrays in edge group */    /*E0378*/

  for(i=0; i < ArrayCount; i++)
  {  ColPtr = &BG->ArrayColl;
     DArr = coll_At(s_DISARRAY *, ColPtr, i);

     BGCount = DArr->BG.Count; /* number of groups
                                  the current array is included in */    /*E0379*/

     for(k=0; k < BGCount; k++)
     {  ColPtr = &DArr->ResShdWidthColl;
        ShdWidth = coll_At(s_SHDWIDTH *, ColPtr, k);

        if(ShdWidth->UseSign)
           continue;           /* */    /*E0380*/

        ShdWidth->UseSign = 1; /* */    /*E0381*/

        ColPtr = &DArr->BG;
        CurrBG = coll_At(s_BOUNDGROUP *, ColPtr, k);

        if(CurrBG != BG)
           continue;  /* the given group is not current */    /*E0382*/ 

        Rank = DArr->Space.Rank;
        Align = DArr->Align;

        for(j=0; j < Rank; j++)
        {  if(Align[j].Attr != align_NORMAL)
              continue; /* array dimention is not distributed */    /*E0383*/

           switch(ShdWidth->ShdSign[j])
           {  case 2:
              case 3:  if(ShdWidth->ResLowShdWidth[j] == 0)
                          continue; /* no case of local part bounds exceeding  */    /*E0384*/
                       if(AMVAxis)
                       {  /* local part bounds exceeding took place */    /*E0385*/

                          if(DArr->AMView != AMView ||
                             Align[j].TAxis != AMVAxis || OutAxis != -1)
                             AxisCount++; /* number of different cases of
                                             local part bounds exceeding */    /*E0386*/
                       }
                       else
                       {  /* First case of local part bounds exceeding */    /*E0387*/

                          AMView = DArr->AMView;
                          AMVAxis = Align[j].TAxis;
                          OutAxis = -1;
                          AxisCount++; /* number of different cases of 
                                             local part bounds exceeding */    /*E0388*/
                       }

                       break; 

              case 4:
              case 5:  if(ShdWidth->ResHighShdWidth[j] == 0)
                          continue; /* no case of local part bounds exceeding*/    /*E0389*/
                       if(AMVAxis)
                       {  /* local part bounds exceeding took place */    /*E0390*/

                          if(DArr->AMView != AMView ||
                             Align[j].TAxis != AMVAxis || OutAxis != 1)
                             AxisCount++; /*  number of different cases of 
                                             local part bounds exceeding */    /*E0391*/
                       }
                       else
                       {  /* First case of local part bounds exceeding */    /*E0392*/

                          AMView = DArr->AMView;
                          AMVAxis = Align[j].TAxis;
                          OutAxis = 1;
                          AxisCount++; /* number of different cases of 
                                          local part bounds exceeding */    /*E0393*/
                       }

                       break;

              case 6:
              case 7:  if(ShdWidth->ResLowShdWidth[j])
                       {  /* First local part overrunning */    /*E0394*/

                          if(AMVAxis)
                          {  /* local part bounds exceeding took place */    /*E0395*/

                             if(DArr->AMView != AMView ||
                                Align[j].TAxis != AMVAxis ||
                                OutAxis != -1)
                                AxisCount++; /* number of different cases of 
                                               local part bounds exceeding */    /*E0396*/
                          }
                          else
                          {  /* First local part bounds exceeding */    /*E0397*/

                             AMView = DArr->AMView;
                             AMVAxis = Align[j].TAxis;
                             OutAxis = -1;
                             AxisCount++; /* number of different cases of 
                                          local part bounds exceeding */    /*E0398*/
                          }
                       }

                       if(ShdWidth->ResHighShdWidth[j])
                       {  /* High edge bounds exceeding took place */    /*E0399*/

                          if(AMVAxis)
                          {  /* local part bounds exceeding took place */    /*E0400*/

                             if(DArr->AMView != AMView ||
                                Align[j].TAxis != AMVAxis ||
                                OutAxis != 1)
                                AxisCount++; /* number of different cases of 
                                                local part bounds exceeding */    /*E0401*/
                          }
                          else
                          {  /* First local part bounds exceeding */    /*E0402*/

                             AMView = DArr->AMView;
                             AMVAxis = Align[j].TAxis;
                             OutAxis = 1;
                             AxisCount++; /* number of different cases of 
                                             local part bounds exceeding */    /*E0403*/
                          }
                       }

                       break;

              default: break;
           }

           if(AxisCount > 1)
              break;
        }

        if(AxisCount > 1)
           break;
     }

     if(AxisCount > 1)
        break;
  }

  /* */    /*E0404*/

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

  *BGAMView = AMView;  /* pointer to AM representation */    /*E0405*/
  *OutDir   = OutAxis; /* direction 
                          of the local part bounds exceeding */    /*E0406*/

  if(AxisCount > 1)
     return  0;
  else
     return  AMVAxis;
}


/* --------------------------------------------------- */    /*E0407*/


int  shd_Sendnowait(void *buf, int count, int size, int procnum,
                   int tag, RTL_Request *RTL_ReqPtr, int MsgPartition)
{ int     rc;

  if(RTL_TRACE)
     dvm_trace(call_shd_Sendnowait,"buf=%lx; count=%d; size=%d; "
               "req=%lx; procnum=%d(%d); procid=%d; tag=%d;\n",
                (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
                ProcNumberList[procnum], ProcIdentList[procnum], tag);

  rc = ( RTL_CALL, rtl_Sendnowait(buf, count, size, procnum,
                                  tag, RTL_ReqPtr, MsgPartition) );

  if(RTL_TRACE)
     dvm_trace(ret_shd_Sendnowait,"rc=%d; req=%lx; MsgPartition=%d;\n",
                                   rc, (uLLng)RTL_ReqPtr, MsgPartition);

  return  (DVM_RET, rc);
}



int  shd_Recvnowait(void *buf, int count, int size, int procnum,
                   int tag, RTL_Request *RTL_ReqPtr)
{ int rc;

  if(RTL_TRACE)
     dvm_trace(call_shd_Recvnowait,"buf=%lx; count=%d; size=%d; "
               "req=%lx; procnum=%d(%d); procid=%ld; tag=%d;\n",
                (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
                ProcNumberList[procnum], ProcIdentList[procnum],tag);

  rc = ( RTL_CALL, rtl_Recvnowait(buf, count, size, procnum,
                                  tag, RTL_ReqPtr, 0) );

  if(RTL_TRACE)
     dvm_trace(ret_shd_Recvnowait,"rc=%d; req=%lx;\n",
                                   rc, (uLLng)RTL_ReqPtr);

  return  (DVM_RET, rc);
}


                       
void  shd_Waitrequest(RTL_Request *RTL_ReqPtr)
{ int  procnum;

  if(RTL_TRACE)
  {  procnum = RTL_ReqPtr->ProcNumber;

     dvm_trace(call_shd_Waitrequest,
               "req=%lx; procnum=%d(%d); procid=%d;\n",
               (uLLng)RTL_ReqPtr, procnum, ProcNumberList[procnum],
               ProcIdentList[procnum]);
  }

  ( RTL_CALL, rtl_Waitrequest(RTL_ReqPtr) );
  
  if(RTL_TRACE)
     dvm_trace(ret_shd_Waitrequest,"req=%lx;\n", (uLLng)RTL_ReqPtr);

  (DVM_RET);

  return;
}


#endif /* _BOUNDS_C_ */    /*E0408*/
