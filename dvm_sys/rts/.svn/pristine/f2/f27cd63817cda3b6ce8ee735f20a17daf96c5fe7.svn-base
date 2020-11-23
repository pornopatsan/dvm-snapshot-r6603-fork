#ifndef _PSS_C_
#define _PSS_C_
/*************/    /*E0000*/

/**************************************\
* Functions to built processor systems * 
\**************************************/    /*E0001*/

PSRef  __callstd getps_(AMRef  *AMRefPtr)

/*
       Requesting processor system.
       ----------------------------

*AMRefPtr - reference to the abstract machine. 

The function returns a reference to the processor system
the specified abstract machine is mapped onto.
*/    /*E0002*/

{ PSRef        Res;
  SysHandle   *AMHandlePtr;
  s_AMS       *AMS;

  if(AMRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0003*/
  else
     StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0004*/

  DVMFTimeStart(call_getps_);

  if(RTL_TRACE)
  {  if(AMRefPtr == NULL || *AMRefPtr == 0)
        dvm_trace(call_getps_,"AMRefPtr=NULL; AMRef=0;\n");
     else
        dvm_trace(call_getps_,"AMRefPtr=%lx; AMRef=%lx;\n",
                              (uLLng)AMRefPtr, *AMRefPtr);
  }

  if(AMRefPtr == NULL || *AMRefPtr == 0)
     AMHandlePtr = CurrAMHandlePtr;      /* current AM */    /*E0005*/
  else
  {  if(*AMRefPtr == -1)
        AMHandlePtr = &InitAMSHandle;    /* initial AM */    /*E0006*/
     else
     {  if(TstObject)
        {  if(!TstDVMObj(AMRefPtr))
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.000: wrong call getps_ "
                "(the abstract machine is not a DVM object; "
                "AMRef=%lx)\n", *AMRefPtr);
        }

        AMHandlePtr=(SysHandle *)(*AMRefPtr);

        if(AMHandlePtr->Type != sht_AMS)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.001: wrong call getps_\n"
                "(the object is not an abstract machine; AMRef=%lx)\n",
                *AMRefPtr);
     }
  }

  AMS = (s_AMS *)AMHandlePtr->pP;

  if(AMS->VMS != NULL)
     Res = (PSRef)(AMS->VMS->HandlePtr);
  else
     Res = 0;

  if(RTL_TRACE)
     dvm_trace(ret_getps_,"PSRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0007*/
  DVMFTimeFinish(ret_getps_);
  return  (DVM_RET, Res);
}



PSRef  __callstd  crtps_(PSRef  *PSRefPtr, DvmType  InitIndexArray[],
                         DvmType  LastIndexArray[], DvmType  *StaticSignPtr)
{ PSRef       Res;
  SysHandle  *VMSHandlePtr, *VProc;
  s_VMS      *VMS, *NewVMS;
  int         Rank, i, NewInd;
  DvmType        PSSize, LinInd, GenBlock = 0;
  DvmType       *CurrIndexArray, *SizeArray, *InitIndex, *LastIndex;
  s_SPACE    *VMSSpace;

  if(PSRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0008*/
  else
     StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0009*/

  DVMFTimeStart(call_crtps_);

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL || *PSRefPtr == 0)
        dvm_trace(call_crtps_,
                  "PSRefPtr=NULL; PSRef=0; StaticSign=%ld;\n",
                  *StaticSignPtr);
     else
        dvm_trace(call_crtps_,
                  "PSRefPtr=%lx; PSRef=%lx; StaticSign=%ld;\n",
                  (uLLng)PSRefPtr, *PSRefPtr, *StaticSignPtr);
  }

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(TstDVMObj(PSRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.030: wrong call crtps_ "
             "(the processor system is not a DVM object; "
             "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 032.031: wrong call crtps_\n"
               "(the object is not a processor system; PSRef=%lx)\n",
               *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;

     /* Check if all processosr of the processor system
            belong to the current processor system      */    /*E0010*/

     NotSubsystem(i, DVM_VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.032: wrong call crtps_ "
             "(the initial PS is not a subsystem of the current PS; "
             "PSRef=%lx; CurrentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);
  }

  Rank = VMS->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtps_))
     {  for(i=0; i < Rank; i++)
            tprintf("InitIndexArray[%d]=%ld; ", i, InitIndexArray[i]);
        tprintf(" \n");

        for(i=0; i < Rank; i++)
            tprintf("LastIndexArray[%d]=%ld; ", i, LastIndexArray[i]);
        tprintf(" \n");

        tprintf(" \n");
     }
  }

  /* Check correctness of initial and final index values */    /*E0011*/

  if(DVM_LEVEL)
     VMSSpace = &VMS->Space; /* support system call
                                crtps_ function */    /*E0012*/
  else
     VMSSpace = &VMS->TrueSpace; /* user program call
                                    ctrps_ function */    /*E0013*/

  for(i=0; i < Rank; i++)
  {  if(InitIndexArray[i] >= VMSSpace->Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.035: wrong call crtps_ "
                 "(InitIndexArray[%d]=%ld >= %ld; PSRef=%lx)\n",
                 i, InitIndexArray[i], VMSSpace->Size[i],
                 (uLLng)VMSHandlePtr);
  }

  for(i=0; i < Rank; i++)
  {  if(InitIndexArray[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.036: wrong call crtps_ "
                 "(InitIndexArray[%d]=%ld < 0; PSRef=%lx)\n",
                 i, InitIndexArray[i], (uLLng)VMSHandlePtr);
  }

  for(i=0; i < Rank; i++)
  {  if(LastIndexArray[i] >= VMSSpace->Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.037: wrong call crtps_ "
                 "(LastIndexArray[%d]=%ld >= %ld; PSRef=%lx)\n",
                 i, LastIndexArray[i], VMSSpace->Size[i],
                 (uLLng)VMSHandlePtr);
  }

  for(i=0; i < Rank; i++)
  {  if(LastIndexArray[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.038: wrong call crtps_ "
                 "(LastIndexArray[%d]=%ld < 0; PSRef=%lx)\n",
                 i, LastIndexArray[i], (uLLng)VMSHandlePtr);
  }

  for(i=0; i < Rank; i++)
  {  if(InitIndexArray[i] > LastIndexArray[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.039: wrong call crtps_ "
                 "(InitIndexArray[%d]=%ld > LastIndexArray[%d]=%ld; "
                 "PSRef=%lx)\n",
                 i, InitIndexArray[i], i, LastIndexArray[i],
                 (uLLng)VMSHandlePtr);
  }

  /* Create arrays of initial and final
     indexes of created processor system */    /*E0014*/

  dvm_AllocArray(DvmType, Rank, InitIndex);
  dvm_AllocArray(DvmType, Rank, LastIndex);

  if(IsUserPS)
  {  /* Work with user program processor system */    /*E0015*/

     for(i=0; i < Rank; i++)
     {  InitIndex[i] = 0;
        LastIndex[i] = VMS->Space.Size[i] - 1;
     }
  }
  else
  {  /* Work in ordinary mode */    /*E0016*/

     for(i=0; i < Rank; i++)
     {  InitIndex[i] = InitIndexArray[i];
        LastIndex[i] = LastIndexArray[i];
     }
  }

  /* Create array of created processor system sizes
       and array of its processor element Handles   */    /*E0017*/

  dvm_AllocArray(DvmType, Rank + 1, CurrIndexArray);
  CurrIndexArray[0] = Rank;

  dvm_AllocArray(DvmType, Rank, SizeArray);

  PSSize = 1;  /* the number of elements of created processor system */    /*E0018*/

  for(i=0; i < Rank; i++)
  {  SizeArray[i] = LastIndex[i] - InitIndex[i] + 1;
     PSSize *= SizeArray[i];
     CurrIndexArray[i+1] = InitIndex[i];
  }

  dvm_AllocArray(SysHandle, PSSize, VProc);

  for(NewInd=0; ; NewInd++)
  {  LinInd = space_GetLI(&VMS->Space, CurrIndexArray);
     VProc[NewInd] = VMS->VProc[LinInd];
     VProc[NewInd].EnvInd = gEnvColl->Count-1; /* index of context
                                                  where processor has 
                                                  been created */    /*E0019*/
     VProc[NewInd].CrtEnvInd = gEnvColl->Count-1; /* index of context
                                                     where processor has 
                                                     been created */    /*E0020*/

     for(i=Rank-1; i >= 0; i--)
     {  if(CurrIndexArray[i+1] < LastIndex[i])
           break;
     }

     if(i < 0)
        break;
     else
     {  CurrIndexArray[i+1]++;
        for(i++; i < Rank; i++)
            CurrIndexArray[i+1] = InitIndex[i];
     }
  }

  dvm_FreeArray(CurrIndexArray);

  Res = ( RTL_CALL, CreateVMS(VProc, (byte)Rank, SizeArray, (byte)*StaticSignPtr) );

  NewVMS = (s_VMS *)((SysHandle *)Res)->pP;

  /* */    /*E0021*/

  for(i=0; i < Rank; i++)
      NewVMS->InitIndex[i] = InitIndex[i];

  /* ------------------------------------------------------- */    /*E0022*/

  NewVMS->PHandlePtr = VMS->HandlePtr;  /* reference to Handle of
                                           parent processor system */    /*E0023*/
  coll_Insert(&VMS->SubSystem, NewVMS); /* to the list of subsystems of
                                           the parent processor system */    /*E0024*/
  NewVMS->TreeIndex = VMS->TreeIndex+1; /* distance to the 
                                           processor system tree root */    /*E0025*/

  /* Form sizes of user program processor system */    /*E0026*/

  for(i=0; i < Rank; i++)
      SizeArray[i] = LastIndexArray[i] - InitIndexArray[i] + 1;
  NewVMS->TrueSpace = space_Init((byte)Rank, SizeArray);

  dvm_FreeArray(SizeArray);
  dvm_FreeArray(InitIndex);
  dvm_FreeArray(LastIndex);

  /*      Form processor coordinate weigdht array and 
     processor coordinate summary outstripping weight array 
         for each dimension of created processor system     */    /*E0027*/

  for(i=0; i < Rank; i++)
  {  NewVMS->CoordWeight[i] = NULL;
     NewVMS->PrevSumCoordWeight[i] = NULL;
  }

  ( RTL_CALL, setpsw_(&Res, NULL, NULL, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_crtps_,"PSRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0028*/
  DVMFTimeFinish(ret_crtps_);
  return  (DVM_RET, Res);
}



PSRef  __callstd  psview_(PSRef  *PSRefPtr, DvmType  *RankPtr,
                          DvmType  SizeArray[], DvmType  *StaticSignPtr)
{ PSRef       Res;
  SysHandle  *VMSHandlePtr, *VProc;
  s_VMS      *VMS, *NewVMS;
  int         Rank, NewRank, i;
  DvmType        PSSize, GenBlock = 0;
  DvmType       *Size;

  if(PSRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0029*/
  else
     StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0030*/

  DVMFTimeStart(call_psview_);

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL || *PSRefPtr == 0)
       dvm_trace(call_psview_,
                 "PSRefPtr=NULL; PSRef=0; Rank=%ld; StaticSign=%ld;\n",
                 *RankPtr, *StaticSignPtr);
     else
       dvm_trace(call_psview_,
                "PSRefPtr=%lx; PSRef=%lx; Rank=%ld; StaticSign=%ld;\n",
                (uLLng)PSRefPtr, *PSRefPtr, *RankPtr, *StaticSignPtr);
  }

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(!TstDVMObj(PSRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 032.050: wrong call psview_ "
                    "(the processor system is not a DVM object;"
                    " PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 032.051: wrong call psview_\n"
               "(the object is not a processor system; PSRef=%lx)\n",
               *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;

     /* Check if all processosr of the processor system
            belong to the current processor system      */    /*E0031*/

     NotSubsystem(i, DVM_VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.052: wrong call psview_ "
             "(the initial PS is not a subsystem of the current PS; "
             "PSRef=%lx; CurrentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);
  }

  Rank = VMS->Space.Rank;
  NewRank = *RankPtr;

  if(NewRank <= 0 || NewRank > MAXARRAYDIM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.055: wrong call psview_ "
              "(rank of the new PS = %d); PSRef=%lx\n",
              NewRank, (uLLng)VMSHandlePtr);
                                          
  if(RTL_TRACE)
  {  if(TstTraceEvent(call_psview_))
     {  for(i=0; i < NewRank; i++)
            tprintf("SizeArray[%d]=%ld; ", i, SizeArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  /* Create array of sizes of created processor system */    /*E0032*/

  dvm_AllocArray(DvmType, NewRank, Size);

  if(IsUserPS)
  {  /* Work with user program processor system */    /*E0033*/

     PSSize = VMS->ProcCount / NewRank; /* size of each dimension 
                                           of created system */    /*E0034*/
     if(PSSize == 0)
        PSSize = 1;

     for(i=0; i < NewRank; i++)
         Size[i] = PSSize;

     PSSize *= NewRank;
     Size[0] += VMS->ProcCount - PSSize;
  }
  else
  {  /* Work in ordinary mode */    /*E0035*/

     for(i=0; i < NewRank; i++)
         Size[i] = SizeArray[i];
  }

  /* Check correctness of created processor system sizes */    /*E0036*/

  PSSize = 1;  /* the number of processors in  
                  newly created processor system */    /*E0037*/

  for(i=0; i < NewRank; i++)
  {  if(SizeArray[i] <= 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.056: wrong call psview_ "
                 "(SizeArray[%d]=%ld <= 0; PSRef=%lx)\n",
                 i, SizeArray[i], (uLLng)VMSHandlePtr);

     PSSize *= Size[i];
  }

  if(PSSize != VMS->ProcCount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.057: wrong call psview_ "
              "(New PSSize=%ld # Old PSSize=%ld; PSRef=%lx)\n",
              PSSize, VMS->ProcCount, (uLLng)VMSHandlePtr);

  /* Create new processor system */    /*E0038*/

  dvm_AllocArray(SysHandle, PSSize, VProc);

  for(i=0; i < PSSize; i++)
      VProc[i] = VMS->VProc[i];

  Res = ( RTL_CALL, CreateVMS(VProc, (byte)NewRank, Size, (byte)*StaticSignPtr) );

  NewVMS = (s_VMS *)((SysHandle *)Res)->pP;

  /* */    /*E0039*/

  for(i=0; i < NewRank; i++)
      NewVMS->InitIndex[i] = 0;

  /* ------------------------------------------------------- */    /*E0040*/

  NewVMS->PHandlePtr = VMS->HandlePtr;  /* reference to Handle of
                                           parent processor system */    /*E0041*/
  coll_Insert(&VMS->SubSystem, NewVMS); /* to the list of subsystems of
                                           the parent processor system */    /*E0042*/
  NewVMS->TreeIndex = VMS->TreeIndex+1; /* distance to the processor
                                           system tree root */    /*E0043*/

  /* Form sizes of user program processor system */    /*E0044*/

  NewVMS->TrueSpace = space_Init((byte)NewRank, SizeArray);
  dvm_FreeArray(Size);

  /*      Form processor coordinate weigdht array and 
     processor coordinate summary outstripping weight array 
         for each dimension of created processor system     */    /*E0045*/

  for(i=0; i < NewRank; i++)
  {  NewVMS->CoordWeight[i] = NULL;
     NewVMS->PrevSumCoordWeight[i] = NULL;
  }

  ( RTL_CALL, setpsw_(&Res, NULL, NULL, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_psview_,"PSRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0046*/
  DVMFTimeFinish(ret_psview_);
  return  (DVM_RET, Res);
}



#ifndef _DVM_IOPROC_

DvmType  __callstd  delps_(PSRef  *PSRefPtr)
{ SysHandle  *VMSHandlePtr;

  StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0047*/
  DVMFTimeStart(call_delps_);

  if(RTL_TRACE)
     dvm_trace(call_delps_,"PSRefPtr=%lx; PSRef=%lx;\n",
                           (uLLng)PSRefPtr, *PSRefPtr);

  if(TstObject)
  {  if(!TstDVMObj(PSRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.070: wrong call delps_\n"
             "(the processor system is not a DVM object;"
             " PSRef=%lx)\n", *PSRefPtr);
  }

  VMSHandlePtr = (SysHandle *)*PSRefPtr;

  if(VMSHandlePtr->Type != sht_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.071: wrong call delps_\n"
              "(the object is not a processor system; PSRef=%lx)\n",
              *PSRefPtr);

  ( RTL_CALL, delobj_(PSRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delps_," \n");

  StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0048*/
  DVMFTimeFinish(ret_delps_);
  return  (DVM_RET, 0);
}

#endif


/* ----------------------------------------------------- */    /*E0049*/
 
DvmType  __callstd setpsw_(PSRef  *PSRefPtr, AMViewRef  *AMViewRefPtr, double  CoordWeightArray[], DvmType  *GenBlockSignPtr)
{ SysHandle      *VMSHandlePtr, *AMVHandlePtr;
  s_VMS          *VMS;
  s_AMVIEW       *AMV;
  int             Rank, i, j, LinInd, elm, VMSize;
  double          PrevSum, Power;
  double         *SumCoordWeight[MAXARRAYDIM];
  double          MinWeight[MAXARRAYDIM]; 
  s_BLOCK         CurrBlock, InitBlock;
  DvmType            Index[MAXARRAYDIM + 1];
  byte            WSignArray[MAXARRAYDIM];
  double         *CoordWeightPtr = NULL, *CWPtr = NULL, *DPtr1, *DPtr2;
  s_AMS          *PAMS;
  DvmType            tlong;
  DvmType           *LPtr1, *LPtr2;

  if(PSRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0050*/
  else
     StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0051*/

  DVMFTimeStart(call_setpsw_);

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
           dvm_trace(call_setpsw_,
                     "PSRefPtr=NULL; PSRef=0; AMViewRefPtr=NULL; "
                     "AMViewRef=0; IntWeightSign=%lx;\n",
                     GenBlockSignPtr);
        else
           dvm_trace(call_setpsw_,
                     "PSRefPtr=NULL; PSRef=0; "
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "IntWeightSign=%lx;\n", (uLLng)AMViewRefPtr,
                     *AMViewRefPtr, GenBlockSignPtr);
     }
     else
     {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
           dvm_trace(call_setpsw_,
                     "PSRefPtr=%lx; PSRef=%lx; "
                     "AMViewRefPtr=NULL; AMViewRef=0; "
                     "IntWeightSign=%lx;\n", (uLLng)PSRefPtr,
                     *PSRefPtr, GenBlockSignPtr);
        else
           dvm_trace(call_setpsw_,
                     "PSRefPtr=%lx; PSRef=%lx; "
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "IntWeightSign=%lx;\n", (uLLng)PSRefPtr, *PSRefPtr,
                     (uLLng)AMViewRefPtr, *AMViewRefPtr,
                     GenBlockSignPtr);
     }
  }

  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
  {  /*      Set coordinate weigths for 
        all abstract machine representations */    /*E0052*/

     AMV = NULL;

     if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  VMS = DVM_VMS;
        VMSHandlePtr = VMS->HandlePtr;
     }
     else
     {  if(TstObject)
        {  if(TstDVMObj(PSRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.010: wrong call setpsw_\n"
                "(the processor system is not a DVM object; "
                "PSRef=%lx)\n", *PSRefPtr);
        }

        VMSHandlePtr = (SysHandle *)*PSRefPtr;

        if(VMSHandlePtr->Type != sht_VMS)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.011: wrong call setpsw_\n"
                "(the object is not a processor system; PSRef=%lx)\n",
                *PSRefPtr);

        VMS = (s_VMS *)VMSHandlePtr->pP;
     }
  }
  else
  {  /* Set coordinate weigths for the given representation */    /*E0053*/

     if(TstObject)
     {  if(TstDVMObj(AMViewRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.015: wrong call setpsw_\n"
             "(the representation is not a DVM object; "
             "AMViewRef=%lx)\n", *AMViewRefPtr);
     }

     AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

     if(AMVHandlePtr->Type != sht_AMView)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 032.016: wrong call setpsw_\n"
           "(the object is not an abctract machine representation; "
           "AMViewRef=%lx)\n",
           *AMViewRefPtr);

     AMV = (s_AMVIEW *)AMVHandlePtr->pP;

     PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent abstract machine */    /*E0054*/

     /* Check if the parent abstract machine is mapped */    /*E0055*/

     if(PAMS->VMS == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.017: wrong call setpsw_\n"
                 "(the parental AM has not been mapped; "
                 "AMViewRef=%lx; ParentAMRef=%lx)\n",
                 (uLLng)AMVHandlePtr, (uLLng)PAMS->HandlePtr);

     if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  VMS = DVM_VMS;
        VMSHandlePtr = VMS->HandlePtr;
     }
     else
     {  if(TstObject)
        {  if(TstDVMObj(PSRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.010: wrong call setpsw_\n"
                "(the processor system is not a DVM object; "
                "PSRef=%lx)\n", *PSRefPtr);
        }

        VMSHandlePtr = (SysHandle *)*PSRefPtr;

        if(VMSHandlePtr->Type != sht_VMS)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.011: wrong call setpsw_\n"
                "(the object is not a processor system; PSRef=%lx)\n",
                *PSRefPtr);

        VMS = (s_VMS *)VMSHandlePtr->pP;
     }

     /* Check if all processors of the given processor
        system belong  to the parent processor system  */    /*E0056*/

     NotSubsystem(i, PAMS->VMS, VMS)
   
     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 032.012: wrong call setpsw_\n"
             "(the given PS is not a subsystem of the parental PS;\n"
             "PSRef=%lx; ParentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)(PAMS->VMS->HandlePtr));
  }

  /* Check if all processors of the given processor
     system belong to the current processor system  */    /*E0057*/

  NotSubsystem(i, DVM_VMS, VMS)
   
  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.013: wrong call setpsw_\n"
              "(the given PS is not a subsystem of the current PS;\n"
              "PSRef=%lx; CurrentPSRef=%lx)\n",
              *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);

  Rank = VMS->Space.Rank;

  if(CoordWeightArray != NULL && CoordWeightArray[0] == -1.)
  {  /* Set unit processor weights and their coordinates */    /*E0058*/

     if(AMV)
     {  /* Set unit processor weights for the given
               abstract machine representation      */    /*E0059*/

        AMV->WeightVMS = VMS; /* reference to processor system
                                 for which weight are setting */    /*E0060*/

        for(i=0; i < MAXARRAYDIM; i++)
        {  dvm_FreeArray(AMV->CoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumCoordWeight[i]);
           dvm_FreeArray(AMV->GenBlockCoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumGenBlockCoordWeight[i]);
        }

        for(i=0; i < Rank; i++)
        {  VMSize = (int)VMS->Space.Size[i];

           dvm_AllocArray(double, VMSize, AMV->CoordWeight[i]);
           dvm_AllocArray(double, VMSize, AMV->PrevSumCoordWeight[i]);

           DPtr1 = AMV->CoordWeight[i];
           DPtr2 = AMV->PrevSumCoordWeight[i];

           for(j=0; j < VMSize; j++)
           {  DPtr1[j] = 1.;
              DPtr2[j] = (double)j;
           }
        }
     }
     else
     {  /*   Set unit coordinate weights for all
           representations ( for processor system) */    /*E0061*/

        for(i=0; i < Rank; i++)
        {  PtrToVoidPtr = (void **)&VMS->CoordWeight[i];
           mac_free(PtrToVoidPtr);
           PtrToVoidPtr = (void **)&VMS->PrevSumCoordWeight[i];
           mac_free(PtrToVoidPtr);

           VMSize = (int)VMS->Space.Size[i];

           dvm_AllocArray(double, VMSize, VMS->CoordWeight[i]);
           dvm_AllocArray(double, VMSize, VMS->PrevSumCoordWeight[i]);

           DPtr1 = VMS->CoordWeight[i];
           DPtr2 = VMS->PrevSumCoordWeight[i];

           for(j=0; j < VMSize; j++)
           {  DPtr1[j] = 1.;
              DPtr2[j] = (double)j;
           }
        }
     }

     if(RTL_TRACE)
     {  if(TstTraceEvent(call_setpsw_))
        {  for(i=0; i < Rank; i++)
           {  tprintf("CoordWeight[%d]= ", i);

              VMSize = (int)VMS->Space.Size[i];

              for(j=0,elm=0; j < VMSize; j++,elm++)
              {  if(elm == 5)
                 {  elm = 0;
                    tprintf(" \n                ");
                 }

                 if(AMV)
                    tprintf("%4.2lf(%4.2lf) ",
                            AMV->CoordWeight[i][j], 1.);
                 else
                    tprintf("%4.2lf(%4.2lf) ",
                            VMS->CoordWeight[i][j], 1.);
              }
              tprintf(" \n");
           }
        }
     }
  }
  else
  {  if(CoordWeightArray == NULL || CoordWeightArray[0] == 0. ||
        IsUserPS)
     {  /* Set initial processor coordinate weights */    /*E0062*/
        if(VMS == MPS_VMS && IsUserPS == 0)
           CWPtr = CoordWeightList; /* coordinate weights, defined
                                       for initial processor system
                                       at the start */    /*E0063*/
        else
           CWPtr = CoordWeight1; /* unit coordinate weights for the 
                                    processor systet that is not 
                                    initial */    /*E0064*/
     }
     else
        CWPtr = CoordWeightArray;

     /* Check and norm processor coordinate weight array */    /*E0065*/

     for(i=0,LinInd=0,elm=0; i < Rank; i++)
     {   VMSize = (int)VMS->Space.Size[i];
         MinWeight[i] = 1.e7;   /* minimal weight in i+1-th dimension */    /*E0066*/
         elm += VMSize;         /* sum of sizes of all dimensions */    /*E0067*/

         for(j=0; j < VMSize; j++,LinInd++)
         { if(CWPtr[LinInd] < 0.)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.019: wrong call setpsw_\n"
                 "(invalid CoordWeightArray[%d]=%lf < 0); PSRef=%lx\n",
                 LinInd, CWPtr[LinInd], *PSRefPtr);
           if(CWPtr[LinInd] > 0.)
              MinWeight[i] = dvm_min(MinWeight[i], CWPtr[LinInd]);
        }
     }

     dvm_AllocArray(double, elm, CoordWeightPtr);

     for(i=0,LinInd=0; i < Rank; i++)
     {   VMSize = (int)VMS->Space.Size[i];
         Power  = 1./MinWeight[i];

         for(j=0; j < VMSize; j++,LinInd++)
             CoordWeightPtr[LinInd] = CWPtr[LinInd] * Power;
     }

     /* ----------------------------------------- */    /*E0068*/

     for(i=0; i < Rank; i++)
     {  WSignArray[i] = 0; /* */    /*E0069*/
        VMSize = (int)VMS->Space.Size[i];
        dvm_AllocArray(double, VMSize, SumCoordWeight[i]);
     }

     /* */    /*E0070*/

     InitBlock = block_InitFromSpace(&VMS->Space);
     CurrBlock = block_Copy(&InitBlock);
     VMSize    = (int)VMS->ProcCount;

     for(LinInd=0; LinInd < VMSize; LinInd++)
     {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);
        Power = ProcWeightArray[LinInd];  /* */    /*E0071*/

        if(LinInd == 0)               /* */    /*E0072*/
        {  for(i=0; i < Rank; i++)
               MinWeight[i] = Power;
           continue;                  /* */    /*E0073*/
        }

        /* */    /*E0074*/

        for(i=Rank-1,j=0; i >= 0; i--)
        {  if(Index[i+1] != 0)
           {  j++;         /* */    /*E0075*/

              if(j == 1) 
                 elm = i;  /* */    /*E0076*/
           }
        }

        if(WSignArray[elm] == 0)
        {  if(dvm_min(MinWeight[elm], Power) /
              dvm_max(MinWeight[elm], Power) > 0.85)
           {  /* */    /*E0077*/

                 MinWeight[elm] = (MinWeight[elm] + Power) * 0.5;
           }
           else 
              WSignArray[elm] = 1;    /* */    /*E0078*/
        }

        for(i=Rank-1; i >= elm; i--)
            MinWeight[i] = Power;
     }
     
     for(i=0; i < Rank; i++)
     {  VMSize = (int)VMS->Space.Size[i];
        DPtr1 = SumCoordWeight[i];

        if(WSignArray[i] == 0)
        {  for(j=0; j < VMSize; j++)
               DPtr1[j] = 1.;
        }
        else
        {  for(j=0; j < VMSize; j++)
               DPtr1[j] = 0.;
        }
     }

     for(i=0,j=0; i < Rank; i++)
         j += WSignArray[i];  /* */    /*E0079*/

     InitBlock = block_InitFromSpace(&VMS->Space);
     CurrBlock = block_Copy(&InitBlock);
     VMSize    = (int)VMS->ProcCount;

     switch(j)
     {  case 0:

        break;

        case 1:

        for(LinInd=0; LinInd < VMSize; LinInd++)
        {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);
           PrevSum = ProcWeightArray[LinInd];

           for(i=0; i < Rank; i++)
               if(WSignArray[i] != 0)
                  SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;

        case 2:

        for(LinInd=0; LinInd < VMSize; LinInd++)
        {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);
           PrevSum = sqrt(ProcWeightArray[LinInd]);

           for(i=0; i < Rank; i++)
               if(WSignArray[i] != 0)
                  SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;

        default:

        Power = 1./(double)j;

        for(LinInd=0; LinInd < VMSize; LinInd++)
        {  spind_FromBlock(Index, &CurrBlock, &InitBlock, 0);
           PrevSum = pow(ProcWeightArray[LinInd], Power);

           for(i=0; i < Rank; i++)
               if(WSignArray[i] != 0)
                  SumCoordWeight[i][Index[i+1]] += PrevSum;
        }

        break;
     }

     /* */    /*E0080*/

     PrevSum = 1.e7;

     for(i=0,LinInd=0; i < Rank; i++)
     {  VMSize = (int)VMS->Space.Size[i];
        DPtr1 = SumCoordWeight[i];

        if(WSignArray[i] != 0)
        {  Power = (double)VMSize / (double)VMS->ProcCount;

           for(j=0; j < VMSize; j++,LinInd++)
           {  DPtr1[j] *= (CoordWeightPtr[LinInd] * Power);
              PrevSum   = dvm_min(PrevSum, DPtr1[j]);
           }
        }
        else
        {  for(j=0; j < VMSize; j++,LinInd++)
           {  DPtr1[j] *= CoordWeightPtr[LinInd];
if (DPtr1[j])
              PrevSum   = dvm_min(PrevSum, DPtr1[j]);
           }
        }
     }
     PrevSum = 1. / PrevSum;

     for(i=0; i < Rank; i++)
     {  VMSize = (int)VMS->Space.Size[i];
        DPtr1 = SumCoordWeight[i];

        for(j=0; j < VMSize; j++)
            DPtr1[j] *= PrevSum;
     }

     /* Form processor coordinate weights and array
        of integral preceding  processor coordinate
             weights for all array dimensions       */    /*E0081*/

     if(AMV)
     {  /* Set processor coordinate weights 
               for given representation     */    /*E0082*/
if( AMV->setCW==1) {

        AMV->disWeightVMS = AMV->WeightVMS; 
	AMV->WeightVMS=VMS; 
        for(i=0; i < MAXARRAYDIM; i++)
        {  AMV->disCoordWeight[i]=AMV->CoordWeight[i];
           AMV->disPrevSumCoordWeight[i]=AMV->PrevSumCoordWeight[i];
           AMV->disGenBlockCoordWeight[i]=AMV->GenBlockCoordWeight[i];
           AMV->disPrevSumGenBlockCoordWeight[i]=AMV->PrevSumGenBlockCoordWeight[i];
           AMV->CoordWeight[i]=NULL;
           AMV->PrevSumCoordWeight[i]=NULL;
           AMV->GenBlockCoordWeight[i]=NULL;
           AMV->PrevSumGenBlockCoordWeight[i]=NULL;

        }
        AMV->setCW=2;
} else {
        AMV->WeightVMS = VMS; /* processor system for which
                                 coordinate weights are settung */    /*E0083*/

        for(i=0; i < MAXARRAYDIM; i++)
        {  dvm_FreeArray(AMV->CoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumCoordWeight[i]);
           dvm_FreeArray(AMV->GenBlockCoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumGenBlockCoordWeight[i]);
        }
}


/*        AMV->WeightVMS = VMS; 
                              

        for(i=0; i < MAXARRAYDIM; i++)
        {  dvm_FreeArray(AMV->CoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumCoordWeight[i]);
           dvm_FreeArray(AMV->GenBlockCoordWeight[i]);
           dvm_FreeArray(AMV->PrevSumGenBlockCoordWeight[i]);
        }
*/
/**/    if(GenBlockSignPtr)
        {  /* Execution of "hard" GenBlock */    /*E0084*/

           for(i=0; i < MAXARRAYDIM; i++)
               AMV->Div[i] = 1;  /* */    /*E0085*/

           for(i=0,LinInd=0; i < Rank; i++)
           {  VMSize = (int)VMS->Space.Size[i];
            if(GenBlockSignPtr[i]==0)
                { LinInd += VMSize; continue; }                                                                                      
              dvm_AllocArray(DvmType, VMSize, AMV->GenBlockCoordWeight[i]);
              dvm_AllocArray(DvmType, VMSize,
                             AMV->PrevSumGenBlockCoordWeight[i]);

              LPtr1 = AMV->GenBlockCoordWeight[i];
              LPtr2 = AMV->PrevSumGenBlockCoordWeight[i];

              for(j=0,tlong=0; j < VMSize; j++,LinInd++)
              {
                  LPtr1[j] = (DvmType)CoordWeightArray[LinInd];
                 LPtr2[j] = tlong;
                 tlong += LPtr1[j];
              }
           }
        }

        for(i=0; i < Rank; i++)
        {  dvm_AllocArray(double, VMS->Space.Size[i],
                          AMV->CoordWeight[i]);
           dvm_AllocArray(double, VMS->Space.Size[i],
                          AMV->PrevSumCoordWeight[i]);
        }

        for(i=0; i < Rank; i++)
        {  VMSize = (int)VMS->Space.Size[i];
           DPtr1 = AMV->CoordWeight[i];
           DPtr2 = SumCoordWeight[i];

           for(j=0,PrevSum=0.; j < VMSize; j++)
           {  DPtr1[j] = DPtr2[j];
              AMV->PrevSumCoordWeight[i][j] = PrevSum;
              PrevSum += DPtr1[j];
           }
        }
     }
     else
     {  /*    Set processor coordinate weights for
           all representations ( for processor system) */    /*E0086*/

        for(i=0; i < Rank; i++)
        {  dvm_FreeArray(VMS->CoordWeight[i]);
           dvm_FreeArray(VMS->PrevSumCoordWeight[i]);
           dvm_AllocArray(double, VMS->Space.Size[i],
                          VMS->CoordWeight[i]);
           dvm_AllocArray(double, VMS->Space.Size[i],
                          VMS->PrevSumCoordWeight[i]);
        }

        for(i=0; i < Rank; i++)
        {  VMSize = (int)VMS->Space.Size[i];
           DPtr1 = VMS->CoordWeight[i];
           DPtr2 = SumCoordWeight[i];

           for(j=0,PrevSum=0.; j < VMSize; j++)
           {  DPtr1[j] = DPtr2[j];
              VMS->PrevSumCoordWeight[i][j] = PrevSum;
              PrevSum += DPtr1[j];
           }
        }
     }

     for(i=0; i < Rank; i++)
     {  dvm_FreeArray(SumCoordWeight[i]);
     }

     if(RTL_TRACE)
     {  if(TstTraceEvent(call_setpsw_))
        {  for(i=0,LinInd=0; i < Rank; i++)
           {  tprintf("CoordWeight[%d]= ", i);

              VMSize = (int)VMS->Space.Size[i];

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
  }

  dvm_FreeArray(CoordWeightPtr);

  if(RTL_TRACE)
     dvm_trace(ret_setpsw_," \n");

  DVMFTimeFinish(ret_setpsw_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  genblk_(PSRef *PSRefPtr, AMViewRef *AMViewRefPtr,
                         AddrType AxisWeightAddr[], DvmType *AxisCountPtr,
                         DvmType *DoubleSignPtr)
{
    DvmType        cwaDim = 0, jmax, AxisCount, GenBlock = 0, *GenBlockPtr = NULL;
  double     *cwa;
  int         i, j, k, PSRank;
  SysHandle  *VMSHandlePtr;
  s_VMS      *VMS;
  int        *IntPtr;
  double     *DoublePtr, *CWPtr;

  if(PSRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0087*/
  else
     StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0088*/

  DVMFTimeStart(call_genblk_);

  if(AxisCountPtr == NULL)
     AxisCount = 0;
  else
     AxisCount = *AxisCountPtr;

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
           dvm_trace(call_genblk_,
                     "PSRefPtr=NULL; PSRef=0; "
                     "AMViewRefPtr=NULL; AMViewRef=0; "
                     "AxisCount=%ld; DoubleSign=%ld\n",
                     AxisCount, *DoubleSignPtr);
        else
           dvm_trace(call_genblk_,
                     "PSRefPtr=NULL; PSRef=0; "
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "AxisCount=%ld; DoubleSign=%ld\n",
                     (uLLng)AMViewRefPtr, *AMViewRefPtr,
                     AxisCount, *DoubleSignPtr);
     }
     else
     {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
           dvm_trace(call_genblk_,
                     "PSRefPtr=%lx; PSRef=%lx; "
                     "AMViewRefPtr=NULL; AMViewRef=0; "
                     "AxisCount=%ld; DoubleSign=%ld\n",
                     (uLLng)PSRefPtr, *PSRefPtr,
                     AxisCount, *DoubleSignPtr);
        else
           dvm_trace(call_genblk_,
                     "PSRefPtr=%lx; PSRef=%lx; "
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "AxisCount=%ld; DoubleSign=%ld\n",
                     (uLLng)PSRefPtr, *PSRefPtr,
                     (uLLng)AMViewRefPtr, *AMViewRefPtr,
                     AxisCount, *DoubleSignPtr);
     }
  }

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(TstDVMObj(PSRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 032.020: wrong call genblk_\n"
                "(the processor system is not a DVM object; "
                "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.021: wrong call genblk_\n"
                 "(the object is not a processor system; PSRef=%lx)\n",
                 *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;
  }

  PSRank = VMS->Space.Rank;

  if(VMS == MPS_VMS)
     CWPtr = CoordWeightList; /* coordinate weights,
                                 defined for initial processor system
                                 at the start */    /*E0089*/
  else
     CWPtr = CoordWeight1; /* unit coordinate weights for the 
                              processor systet that is not initial */    /*E0090*/

  if(AxisCount < 0 || AxisCount > PSRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 032.022: wrong call genblk_\n"
        "(invalid AxisCount: AxisCount=%ld; PSRef=%lx; PSRank=%d)\n",
        AxisCount, (uLLng)VMSHandlePtr, PSRank);

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_genblk_))
     {  if(*DoubleSignPtr)
        {  for(i=0; i < AxisCount; i++)
           {  jmax = VMS->TrueSpace.Size[i];
              DoublePtr   = (double *)AxisWeightAddr[i];

              if(DoublePtr)
              {  for(j=0; j < jmax; j++)
                     tprintf("AxisWeightAddr[%d][%d] = %lf\n",
                             i, j, DoublePtr[j]);
              }
              else
                 tprintf("AxisWeightAddr[%d] = 0.\n", i);
           }
        }
        else
        {  for(i=0; i < AxisCount; i++)
           {  jmax = VMS->TrueSpace.Size[i];
              IntPtr   = (int *)AxisWeightAddr[i];

              if(IntPtr)
              {  for(j=0; j < jmax; j++)
                     tprintf("AxisWeightAddr[%d][%d] = %d\n",
                             i, j, IntPtr[j]);
              }
              else
                 tprintf("AxisWeightAddr[%d] = 0\n", i);
           }
        }
     }
  }

  for(i=0; i < PSRank; i++)
      cwaDim += VMS->TrueSpace.Size[i];

  mac_malloc(cwa, double *, cwaDim*sizeof(double), 0);

  if(*DoubleSignPtr)
  {  for(i=0,k=0; i < AxisCount; i++)
     {  jmax = VMS->TrueSpace.Size[i];
        DoublePtr   = (double *)AxisWeightAddr[i];

        if(DoublePtr)
        {  for(j=0; j < jmax; j++,k++)
               cwa[k] = DoublePtr[j];
        }
        else
        {  for(j=0; j < jmax; j++,k++)
               cwa[k] = CWPtr[k];
        }
     }
  }
  else
  {  /*GenBlock = 1; flag of execution of "hard" GenBlock*/    /*E0091*/
      mac_malloc(GenBlockPtr, DvmType *, AxisCount*sizeof(DvmType), 0);

     for(i=0,k=0; i < AxisCount; i++)
     {  jmax = VMS->TrueSpace.Size[i];
        IntPtr   = (int *)AxisWeightAddr[i];


        if(IntPtr)
        {  for(j=0; j < jmax; j++,k++)
               cwa[k] = IntPtr[j];
GenBlockPtr[i]=1;
        }
        else
        {  for(j=0; j < jmax; j++,k++)
               cwa[k] = CWPtr[k];
GenBlockPtr[i]=0;
        }
     }
  }

  for( ; k < cwaDim; k++)
      cwa[k] = CWPtr[k];

  ( RTL_CALL, setpsw_(PSRefPtr, AMViewRefPtr, cwa, GenBlockPtr) );

  mac_free((void **)&cwa);
  if (GenBlockPtr) mac_free((DvmType **)&GenBlockPtr);
  if(RTL_TRACE)
     dvm_trace(ret_genblk_," \n");

  DVMFTimeFinish(ret_genblk_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  genbli_(PSRef *PSRefPtr, AMViewRef *AMViewRefPtr,
                            AddrType AxisWeightAddr[], DvmType *AxisCountPtr)
{
    DvmType  DoubleSign = 0;

  return  genblk_(PSRefPtr, AMViewRefPtr, AxisWeightAddr, AxisCountPtr, &DoubleSign);
}



DvmType  __callstd  genbld_(PSRef *PSRefPtr, AMViewRef *AMViewRefPtr,
                            AddrType AxisWeightAddr[], DvmType *AxisCountPtr)
{
    DvmType  DoubleSign = 1;

  return  genblk_(PSRefPtr, AMViewRefPtr, AxisWeightAddr, AxisCountPtr, &DoubleSign);
}



DvmType  __callstd  blkdiv_(AMViewRef *AMViewRefPtr, DvmType  AMVAxisDiv[], DvmType *AMVAxisCountPtr)
{ SysHandle   *AMVHandlePtr;
  s_AMVIEW    *AMV;
  int          AMVAxisCount, AMVRank, i;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* */    /*E0092*/

  DVMFTimeStart(call_blkdiv_);

  if(AMVAxisCountPtr == NULL)
     AMVAxisCount = 0;
  else
     AMVAxisCount = (int)*AMVAxisCountPtr;

  if(TstObject)
  {  if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.025: wrong call blkdiv_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.026: wrong call blkdiv_\n"
              "(the object is not an abctract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  AMVRank = AMV->Space.Rank;

  if(AMVAxisCount < 0 || AMVAxisCount > AMVRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.027: wrong call blkdiv_\n"
              "(invalid AMVAxisCount: AMVAxisCount=%d; "
              "AMViewRef=%lx; AMViewRank=%d)\n",
              AMVAxisCount, (uLLng)AMVHandlePtr, AMVRank);

  if(RTL_TRACE)
  {  dvm_trace(call_blkdiv_,
               "AMViewRefPtr=%lx; AMViewRef=%lx; AMVAxisCount=%d\n",
               (uLLng)AMViewRefPtr, *AMViewRefPtr, AMVAxisCount);

     if(TstTraceEvent(call_blkdiv_))
     {  for(i=0; i < AMVAxisCount; i++)
            tprintf("AMVAxisDiv[%d]=%ld ", i, AMVAxisDiv[i]);
        tprintf(" \n");
      }
  }

  for(i=0; i < AMVAxisCount; i++)
      if(AMVAxisDiv[i] <= 0)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 032.028: wrong call blkdiv_\n"
                  "(invalid AMVAxisDiv[%d]=%ld)\n", i, AMVAxisDiv[i]);

  for(i=0; i < AMVAxisCount; i++)
      if(AMV->Space.Size[i] % AMVAxisDiv[i])
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 032.029: wrong call blkdiv_\n"
                  "(size of the %d-th dimension of the abstract "
                  "machine representation is not divisible by %ld;\n"
                  "AMViewRef=%lx; AMVAxisSize[%d]=%ld)\n", i+1,
                  AMVAxisDiv[i], *AMViewRefPtr, i, AMV->Space.Size[i]);

  if(AMVAxisCount != 0)
  {
     for(i=0; i < MAXARRAYDIM; i++)
         AMV->Div[i] = 1;

     for(i=0; i < AMVAxisCount; i++)
         AMV->Div[i] = AMVAxisDiv[i];

     AMV->DivReset = 0;
  }
  else
     AMV->DivReset = 1;

  if(RTL_TRACE)
     dvm_trace(ret_blkdiv_," \n");

  DVMFTimeFinish(ret_blkdiv_);
  return  (DVM_RET, 0);
}



DvmType  __callstd setelw_(PSRef *PSRefPtr, AMViewRef *AMViewRefPtr,
                           AddrType LoadWeightAddr[], DvmType WeightNumber[],
                           DvmType *AddrNumberPtr)
{ SysHandle      *VMSHandlePtr, *AMVHandlePtr;
  s_VMS          *VMS;
  s_AMVIEW       *AMV;
  int             i, j, k, cw;
  s_AMS          *PAMS;
  DvmType            AddrNumber, jmax, AxisSize;
  double         *DoublePtr1, *DoublePtr2;
  double          Wsum, Wmax, Wlow, Whigh, W, Wpre, Wcur; 
  AddrType       *AxisWeightAddr = NULL;

  if(PSRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0093*/
  else
     StatObjectRef = (ObjectRef)*PSRefPtr; /* for statistics */    /*E0094*/

  DVMFTimeStart(call_setelw_);

  if(AddrNumberPtr == NULL)
     AddrNumber = 0;
  else
     AddrNumber = *AddrNumberPtr;

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL || *PSRefPtr == 0)
        dvm_trace(call_setelw_,
                  "PSRefPtr=NULL; PSRef=0; "
                  "AMViewRefPtr=%lx; AMViewRef=%lx;\nAddrNumber=%ld;\n",
                  (uLLng)AMViewRefPtr, *AMViewRefPtr, AddrNumber);
     else
        dvm_trace(call_setelw_,
                  "PSRefPtr=%lx; PSRef=%lx; "
                  "AMViewRefPtr=%lx; AMViewRef=%lx;\nAddrNumber=%ld;\n",
                  (uLLng)PSRefPtr, *PSRefPtr,
                  (uLLng)AMViewRefPtr, *AMViewRefPtr, AddrNumber);
  }

  if(TstObject)
  {  if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.085: wrong call setelw_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.086: wrong call setelw_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;

  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent abstract machine */    /*E0095*/

  /* Check if the parent abstract machine is mapped */    /*E0096*/

  if(PAMS->VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.088: wrong call setelw_\n"
              "(the parental AM has not been mapped; "
              "AMViewRef=%lx; ParentAMRef=%lx)\n",
              (uLLng)AMVHandlePtr, (uLLng)PAMS->HandlePtr);

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(!TstDVMObj(PSRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 032.080: wrong call setelw_\n"
                    "(the processor system is not a DVM object; "
                    "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 032.081: wrong call setelw_\n"
                 "(the object is not a processor system; PSRef=%lx)\n",
                 *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;
  }

  /* Check if all processors of given processor system
           belong to the parent processor system       */    /*E0097*/

  NotSubsystem(i, PAMS->VMS, VMS)
   
  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.082: wrong call setelw_\n"
              "(the given PS is not a subsystem of the parental PS;\n"
              "PSRef=%lx; ParentPSRef=%lx)\n",
              *PSRefPtr, (uLLng)(PAMS->VMS->HandlePtr));

  /* Check if all processors of given processor system
          belong to the current processor system       */    /*E0098*/

  NotSubsystem(i, DVM_VMS, VMS)
   
  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 032.083: wrong call setelw_\n"
              "(the given PS is not a subsystem of the current PS;\n"
              "PSRef=%lx; CurrentPSRef=%lx)\n",
              *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);

  k = VMS->Space.Rank; /* dimension of the given processor system */    /*E0099*/

  if(AddrNumber < 0 || AddrNumber > k)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 032.090: wrong call setelw_\n"
        "(invalid AddrNumber: AddrNumber=%ld; PSRef=%lx; PSRank=%d)\n",
        AddrNumber, (uLLng)VMSHandlePtr, k);

  /* Check correctness of load coordinate weights */    /*E0100*/

  for(i=0; i < AddrNumber; i++)
  {  DoublePtr1 = (double *)LoadWeightAddr[i];

     if(DoublePtr1)
     {  jmax = WeightNumber[i];

        if(jmax < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 032.091: wrong call setelw_\n"
                    "(invalid WeightNumber[%d]=%ld < 0\n", i, jmax);

        for(j=0; j < jmax; j++)
        {  if(DoublePtr1[j] < 0.)

           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 032.092: wrong call setelw_\n"
                    "(invalid LoadWeightAddr[%d][%d]=%lf < 0\n",
                    i, j, DoublePtr1[j]);
           if(DoublePtr1[j] >  MaxCoordWeight)

           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 032.092: wrong call setelw_\n"
                    "(invalid LoadWeightAddr[%d][%d]=%.2lf > MaxCoordWeight=%.2lf\n",
                    i, j, DoublePtr1[j],MaxCoordWeight);
           
        }
     }
  }

  /* Output load coordinate weights in trace */    /*E0101*/

  if(RTL_TRACE)
  {  if(LoadWeightTrace && TstTraceEvent(call_setelw_))
     {  for(i=0; i < AddrNumber; i++)
        {  DoublePtr1 = (double *)LoadWeightAddr[i];

           if(DoublePtr1)
           {  jmax = WeightNumber[i];
              tprintf("WeightNumber[%d] = %ld;\n", i, jmax);

              for(j=0,k=0; j < jmax; j++,k++)
              {  if(k == 2)
                 {  tprintf(" \n");
                    k = 0;
                 }

                 tprintf("  LoadWeight[%d] = %4.2lf;", j, DoublePtr1[j]);
              }

              tprintf(" \n");
           }
           else
              tprintf("  LoadWeightAddr[%d] = 0;\n", i);
        }
     }
  }

  if(AddrNumber)
  {  /* Memory request for parameters of genbld_function */    /*E0102*/

     mac_malloc(AxisWeightAddr, AddrType *,
                AddrNumber*sizeof(AddrType), 0);

     for(i=0; i < AddrNumber; i++)
     {  if(LoadWeightAddr[i] &&
           WeightNumber[i] >= VMS->TrueSpace.Size[i])
        {  mac_malloc(DoublePtr1, double *,
                      VMS->TrueSpace.Size[i]*sizeof(double), 0);
           AxisWeightAddr[i] = (AddrType)DoublePtr1;
        }
        else
           AxisWeightAddr[i] = 0;
     }

     /*   Find calculated coordinate weights
        for each dimension of processor system 
         to provide uniform processor loading  */    /*E0103*/

     for(i=0; i < AddrNumber; i++)
     {  /* Solve optimisation task for (i+1) dimension */    /*E0104*/
        if(AxisWeightAddr[i])
        {  /* Calculate summary and maximal dimension loading weights */    /*E0105*/

           jmax = WeightNumber[i]; /* number of loading coordinate weights */    /*E0106*/
           AxisSize = (int)VMS->TrueSpace.Size[i]; /* size of dimension of
                                                      processor system */    /*E0107*/
           DoublePtr1 = (double *)AxisWeightAddr[i];
           DoublePtr2 = (double *)LoadWeightAddr[i];

           /* --------------------------------------------------- */    /*E0108*/

/* MMMM
           if(jmax == AxisSize)
           { 

              for(j=0; j < jmax; j++)
                  DoublePtr1[j] = DoublePtr2[j];

              continue; 
           }
MMMM
*/
           /* --------------------------------------------------- */    /*E0111*/

           for(j=0,Wsum=0.,Wmax=0.; j < jmax; j++)
           {  Wsum += DoublePtr2[j];
              Wmax = dvm_max(Wmax, DoublePtr2[j]);
           }

           /* Calculate Whigh (maximal calculated weight) */    /*E0112*/

           Wlow = Wsum/(double)AxisSize;     /* initial low edge */    /*E0113*/
           Whigh = dvm_min(Wsum, Wlow+Wmax); /* initial high edge */    /*E0114*/

           while((Whigh - Wlow) > setelw_precision) /* if it is 
                                                       necessary precision*/    /*E0115*/
           {  W = (Whigh + Wlow) * 0.5; /* new low or high edge */    /*E0116*/

              /*     Check if there is a distribution with
                 maximal calculated processor weight equal to W */    /*E0117*/

              for(j=0,k=0,Wmax=0.; j < jmax; j++)
              {  Wcur = DoublePtr2[j];

                 if(Wcur > W)
                    break; /* j-th loading coordinate weight
                              cannot keep within dimension */    /*E0118*/

                 Wpre = Wmax + Wcur;

                 if(Wpre <= W)
                    Wmax = Wpre; /* calculate weight of k-coordinate
                                    do not exceed maximum value */    /*E0119*/
                 else
                 {  /* To the next dimension coordinate */    /*E0120*/

                    k++;

                    if(k == AxisSize)
                       break; /* loading coordinate weights
                                 cannot keep within dimension*/    /*E0121*/

                    Wmax = Wcur;
                 }
              }

              if(j == jmax)
                 Whigh = W; /* there is a dimention with
                               high edge W */    /*E0122*/
              else
                 Wlow = W;  /* there is no  dimention with
                               high edge W */    /*E0123*/
           }
           /* Count calculated processor coordinate weights */    /*E0124*/

           for(j=0; j < AxisSize; j++)
               DoublePtr1[j] = 0.;

           for(j=0,k=0,cw=0,Wmax=0.; j < jmax; j++)
           {  Wcur = DoublePtr2[j];
              Wpre = Wmax + Wcur;

              if(Wpre <= Whigh)
              {  Wmax = Wpre; /* Demanded weight of coordinate is not reached*/    /*E0125*/
                 cw++;
              }
              else
              {  /* Demanded weight of k-coordinate is reached */    /*E0126*/

                 DoublePtr1[k] = cw; /* calculated weight 
                                        of k-coordinate */    /*E0127*/

                 k++;       /* to the next coordinate */    /*E0128*/
                 if(k == AxisSize)
                    break;  /* out of processor system dimension limits */    /*E0129*/

                 Wmax = Wcur;
                 cw = 1;
              }
           }

           if(k < AxisSize)
              DoublePtr1[k] = cw; /* calculated weight of 
                                     the last coordinate */    /*E0130*/

           for(k++; k < AxisSize; k++)
               DoublePtr1[k] = cw;      /* weight of dimentions
                                           with lack of loading weights */    /*E0131*/
        }
     }
  }

  /* Define calculated processor coordinate weights */    /*E0132*/

  ( RTL_CALL, genbld_(PSRefPtr, AMViewRefPtr,
                      AxisWeightAddr, &AddrNumber) );

  /* Free memory of genbld_ parameters */    /*E0133*/

  if(AddrNumber)
  {  for(i=0; i < AddrNumber; i++)
     {  if(AxisWeightAddr[i])
        {  DoublePtr1 = (double *)AxisWeightAddr[i];
           mac_free((void **)&DoublePtr1);
        } 
     }

     mac_free((void **)&AxisWeightAddr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_setelw_," \n");

  DVMFTimeFinish(ret_setelw_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------------- */    /*E0134*/


PSRef  CreateVMS(SysHandle *VPDescArray, byte Rank, DvmType SizeArray[],
                 byte Static)
{ SysHandle     *VMHandlePtr;
  s_VMS         *VMS;
  int            i;
  int            PSSize;
  PSRef          Res;

  if(RTL_TRACE)
  {  dvm_trace(call_CreateVMS, "Rank=%d\n", (int)Rank);

     if(TstTraceEvent(call_CreateVMS))
     {  for(i=0; i < Rank; i++)
            tprintf("SizeArray[%d]=%ld;\n", i, SizeArray[i]);
     }
  }

  dvm_AllocStruct(s_VMS, VMS);

  VMS->Space = space_Init(Rank, SizeArray);
  VMS->TrueSpace = VMS->Space; /* real sizes of
                                  processor system
                                  ( for fixed number of processors) */    /*E0135*/ 
  PSSize     = (int)space_GetSize(&VMS->Space);

  VMS->VProc = VPDescArray;

  VMS->HasCurrent = FALSE;
  VMS->CVP = NULL;

  for(i=0; i < PSSize; i++)
  {  if(VPDescArray[i].lP == MPS_CurrentProc)
     { VMS->HasCurrent = TRUE; /* flag: current processor belongs to 
                                  the given processor system */    /*E0136*/
       VMS->CurrentProc = i;   /* current processor number in 
                                  the given processor system */    /*E0137*/
       VMS->CVP = spind_Init(Rank);
       space_GetSI(&VMS->Space, i, &VMS->CVP); /* internal coordinates of 
                                                  the current processor */    /*E0138*/
     }
  }

  VMS->SubSystem    = coll_Init(VMSVMSCount, VMSVMSCount, NULL);
  VMS->AMVColl      = coll_Init(VMSAMVCount, VMSAMVCount, NULL);
  VMS->AMSColl      = coll_Init(VMSAMSCount, VMSAMVCount, NULL);

  VMS->RedSubSystem = coll_Init(VMSVMSCount, VMSVMSCount, NULL);
  VMS->ResBuf = NULL;    /* */    /*E0139*/
  VMS->RemBuf = NULL;    /* */    /*E0140*/
  VMS->RemBufSize = 0;   /* */    /*E0141*/
  VMS->FreeRemBuf = 1;   /* */    /*E0142*/

  VMS->PHandlePtr = NULL;/* reference to parent processor system */    /*E0143*/
  VMS->TreeIndex  = 0;   /* distance from processor system tree root */    /*E0144*/
  VMS->Static = Static;  /* flag of static processor system */    /*E0145*/

  VMS->MasterProc  = (int)VMS->VProc[0].lP; /* main processor */    /*E0146*/
  VMS->IOProc      = (int)VMS->VProc[0].lP; /* I/O processor */    /*E0147*/
  VMS->VMSCentralProc = (int)GetCentralProc(VMS); /* central processor number
                                                     in the given processor 
                                                     system */    /*E0148*/
  VMS->CentralProc =
  (int)VMS->VProc[VMS->VMSCentralProc].lP; /* central processor */    /*E0149*/
  VMS->ProcCount = PSSize; /* the number of elements in processor system */    /*E0150*/

  VMS->Is_MPI_COMM  = 0; /* */    /*E0151*/

  dvm_AllocStruct(SysHandle, VMHandlePtr);

  *VMHandlePtr = genv_InsertObject(sht_VMS, VMS);
  VMS->HandlePtr = VMHandlePtr; /* pointer to own Handle */    /*E0152*/

  /* Initialization of message tags 
     for created processor system */    /*E0153*/

  VMS->tag_common           = msg_common;
  VMS->tag_gettar_          = msg_gettar_;
  VMS->tag_BroadCast        = msg_BroadCast;
  VMS->tag_BoundsBuffer     = msg_BoundsBuffer;
  VMS->tag_DACopy           = msg_DACopy;
  VMS->tag_RedVar           = msg_RedVar;
  VMS->tag_IdAccess         = msg_IdAccess;
  VMS->tag_across           = msg_across;
  VMS->tag_ProcPowerMeasure = msg_ProcPowerMeasure;

  if(TstObject)
     InsDVMObj((ObjectRef)VMHandlePtr);

  Res = (PSRef)VMHandlePtr;

  /* */    /*E0154*/

  if(RTL_TRACE && CrtPSTrace)
  {  tprintf("ProcCount=%ld  HasCurrent=%d  CentralProc=%d  IOProc=%d\n",
             VMS->ProcCount, (int)VMS->HasCurrent, VMS->CentralProc,
             VMS->IOProc); 
  }

  if(RTL_TRACE)
     dvm_trace(ret_CreateVMS,"PSRef=%lx;\n", Res);

  return  (DVM_RET, Res);
}



s_VMS  *GetCurrentVMS(void)
{ s_AMS      *AMS;

  AMS = (s_AMS *)CurrAMHandlePtr->pP;

  return  AMS->VMS;
}



DvmType   GetCentralProc(s_VMS  *VMS)
{
    DvmType         Si[MAXARRAYDIM + 1];
  int          i, Rank;
  DvmType         Res;

  Rank  = VMS->Space.Rank;
  Si[0] = Rank;

  /* Geometric centre */    /*E0155*/

  for(i=0; i < Rank; i++)
      Si[i+1] = VMS->Space.Size[i] / 2;

  Res = space_GetLI(&VMS->Space, Si);

  return  Res;
}


/***********************************************************\
* Request of internal number in specified processor system  *
* for processor with internal global number IntNumberProc.  *
*          Return -1, if the specified processor            *
*       is not from the specified prosessor system          *
\***********************************************************/    /*E0156*/

int  IsProcInVMS(DvmType  IntNumberProc, s_VMS  *VMS)
{ SysHandle  *VProc;
  int         i, Init, Last;

  VProc = VMS->VProc;
  Init  = 0;
  Last  = (VMS->ProcCount - 1);

  while((i = Last - Init) > 0)
  {
     i = i/2 + Init;

     if(IntNumberProc >= VProc[Init].lP &&
        IntNumberProc <= VProc[i].lP)
     {  Last = i;
        continue;
     }

     i++;

     if(IntNumberProc >= VProc[i].lP &&
        IntNumberProc <= VProc[Last].lP)
     {  Init = i;
        continue;
     }

     break;
  }

  if(IntNumberProc == VProc[Init].lP)
     return  Init;

  return  -1;
}



#ifndef _DVM_IOPROC_

void   vms_Done(s_VMS  *VMS)
{ s_AMVIEW    *wAMV;
  s_VMS       *wVMS, *PVMS;
  AMViewRef    AMVRef;
  PSRef        VMRef;
  int          i;
  s_AMS       *AMS;

  if(RTL_TRACE)
     dvm_trace(call_vms_Done, "PSRef=%lx;\n", (uLLng)VMS->HandlePtr);

  dvm_FreeArray(VMS->VProc);

  /* Cancel all abstract machine mappings 
        on cancelled processor system     */    /*E0157*/

  for(i=0; i < VMS->AMSColl.Count; i++)
  {  AMS = coll_At(s_AMS *, &VMS->AMSColl, i);

     if(AMS->VMS == VMS)
        AMS->VMS = NULL;
  }

  dvm_FreeArray(VMS->AMSColl.List);

  VMS->AMSColl.Count    = 0;
  VMS->AMSColl.Reserv   = 0;
  VMS->AMSColl.CountInc = 0;

  /* Delete all abstract machine representations 
        mapped on cancelled processor system     */    /*E0158*/

  while(VMS->AMVColl.Count)
  {  wAMV = coll_At(s_AMVIEW *, &VMS->AMVColl, VMS->AMVColl.Count-1);

     wAMV->CrtEnvInd = gEnvColl->Count - 1;
     wAMV->HandlePtr->CrtEnvInd = gEnvColl->Count - 1;
     wAMV->HandlePtr->CrtAMHandlePtr = CurrAMHandlePtr;

     AMVRef = (AMViewRef)wAMV->HandlePtr;

     ( RTL_CALL, delamv_(&AMVRef) );
  } 

  coll_Done(&VMS->AMVColl);

  /* Delete parent processor system from subsystem list */    /*E0159*/

  if(VMS->PHandlePtr)
  {  PVMS = (s_VMS *)(VMS->PHandlePtr->pP);
     coll_Delete(&PVMS->SubSystem, VMS);
     coll_Delete(&PVMS->RedSubSystem, VMS);
  }

  /* Cancel all subsystems of cancelled processor system */    /*E0160*/

  while(VMS->SubSystem.Count)
  {  wVMS = coll_At(s_VMS *, &VMS->SubSystem, VMS->SubSystem.Count-1);
     VMRef = (PSRef)wVMS->HandlePtr;

     ( RTL_CALL, delps_(&VMRef) );
  } 

  coll_Done(&VMS->SubSystem);
  coll_Done(&VMS->RedSubSystem);
  mac_free(&VMS->ResBuf);

  mac_free(&VMS->RemBuf);
  VMS->RemBufSize = 0;
  VMS->FreeRemBuf = 1;

  /* ------------------------------------------------ */    /*E0161*/

  if(VMS->HasCurrent)
     spind_Done(&VMS->CVP);

  for(i=0; i < VMS->Space.Rank; i++)
  {  dvm_FreeArray(VMS->CoordWeight[i]);
     dvm_FreeArray(VMS->PrevSumCoordWeight[i]);
  }

  if(TstObject)
     DelDVMObj((ObjectRef)VMS->HandlePtr);

  VMS->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(VMS->HandlePtr);

  /* */    /*E0162*/

#ifdef  _DVM_MPI_

  if(VMS->Is_MPI_COMM)
  {  MPI_Group_free(&VMS->ps_mpi_group);
     MPI_Comm_free(&VMS->PS_MPI_COMM);
     VMS->Is_MPI_COMM = 0;

     if(RTL_TRACE && MPI_ReduceTrace)
     {  tprintf(" \n");
        tprintf("*** MPI_Comm Deleting ***\n");
        tprintf("&MPI_Comm=%lx; &MPI_Group=%lx;\n",
                (uLLng)&VMS->PS_MPI_COMM, (uLLng)&VMS->ps_mpi_group);
     }
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_vms_Done," \n");

  (DVM_RET);

  return;
}

#endif


/* -------------------------------------------------------------- */    /*E0163*/


/******************************************************************\
* Function returns internal processor number if a coordinate of    *
* current processor on prescribed dimension is increased by one.   *
*   If there is no such processor in the prescribed processor      *
* system then it returns -1.                                       *
\******************************************************************/    /*E0164*/

int  GetNextProc(s_VMS  *VMS, int  PSDim)
{ int        Res = -1;
  s_SPACE   *VMSpace;

  VMSpace = &VMS->Space;

  if(VMS->CVP[PSDim] == (VMSpace->Size[PSDim-1] - 1))
     return  Res; /* next processor does not exist */    /*E0165*/

  VMS->CVP[PSDim]++;
  Res = VMS->VProc[ space_GetLI(VMSpace, VMS->CVP) ].lP;
  VMS->CVP[PSDim]--;

  return  Res;
}


/******************************************************************\
* Function returns internal processor number if a coordinate of    *
* current processor on presribed dimension is decreased by one.    *
* If there is no such processor in the prescribed processor        *
* system then it returns -1.                                       *
\******************************************************************/    /*E0166*/

int  GetPrevProc(s_VMS  *VMS, int  PSDim)
{ int        Res = -1;
  s_SPACE   *VMSpace;

  VMSpace = &VMS->Space;

  if(VMS->CVP[PSDim] == 0)
     return  Res; /* there is no next processor */    /*E0167*/

  VMS->CVP[PSDim]--;
  Res = VMS->VProc[ space_GetLI(VMSpace, VMS->CVP) ].lP;
  VMS->CVP[PSDim]++;

  return  Res;
}


#endif   /*  _PSS_C_  */    /*E0168*/
