#ifndef _IDACCESS_C_
#define _IDACCESS_C_
/******************/    /*E0000*/

/************************************************\
* Functions of non-regular access to remote data *
\************************************************/    /*E0001*/

DvmType  __callstd  crtibl_(DvmType  RemArrayHeader[], DvmType  BufferHeader[],
                            void *BasePtr, DvmType *StaticSignPtr,
                            LoopRef *LoopRefPtr, DvmType  MEHeader[],
                            DvmType  ConstArray[])
{ SysHandle     *RemArrayHandlePtr, *LoopHandlePtr, *MEHandlePtr;
  s_DISARRAY    *DArr, *BArr, *ME;
  s_AMVIEW      *AMV, *MEAMV;
  s_PARLOOP     *PL;
  int            i, j, k, AR, AMR;
  ArrayMapRef    MapRef;
  s_ARRAYMAP    *Map;
  DvmType           StaticSign = 0, TLen, ExtHdrSign = 1, BR = 2, Res = 0;
  s_ALIGN       *Align;
  DvmType           SizeArray[2], ShdWidthArray[2];
  s_IDBUF       *IdBuf;

  DVMFTimeStart(call_crtibl_);

  if(RTL_TRACE)
     dvm_trace(call_crtibl_,
        "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; BufferHeader=%lx;\n"
        "BasePtr=%lx; StaticSign=%ld; LoopRefPtr=%lx; LoopRef=%lx;\n"
        "MEHeader=%lx; MEHandlePtr=%lx;\n",
        (uLLng)RemArrayHeader, RemArrayHeader[0], (uLLng)BufferHeader,
        (uLLng)BasePtr, *StaticSignPtr, (uLLng)LoopRefPtr, *LoopRefPtr,
        (uLLng)MEHeader, MEHeader[0]);

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.000: wrong call crtibl_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  RemArrayHandlePtr = TstDVMArray((void *)RemArrayHeader);

  if(RemArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 140.001: wrong call crtibl_\n"
        "(the object is not a remote distributed array;\n"
        "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  DArr = (s_DISARRAY *)RemArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtibl_))
     {  for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
     }
  }

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.002: wrong call crtibl_\n"
              "(the remote array has not been aligned; "
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);
 
  /* Check if processor system, in which the remote array
     is mapped, is a subsystem of current processor system */    /*E0002*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 140.003: wrong call crtibl_\n"
       "(the remote array PS is not a subsystem of the current PS;\n"
       "RemArrayHeader[0]=%lx; RemArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       RemArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Check if remote array has exactly one distributed dimension */    /*E0003*/

  for(i=0,j=0; i < AR; i++)
  {  if(DArr->Align[i].Attr == align_NORMAL)
     {  j++;    /* number of remote array distributed dimensions */    /*E0004*/
        k = i;  /* remote array distributed dimension number */    /*E0005*/
     }
  }

  if(j != 1)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.004: wrong call crtibl_\n"
              "(distributed dimension number of the remote array "
              "is not equal to 1;\n"
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  /* Control specified ConsyArray array */    /*E0006*/

  for(i=0; i < AR; i++)
  {  if(i == k)
        continue;  /* distributed dimension is not checked */    /*E0007*/

     if(ConstArray[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 140.010: wrong call crtibl_\n"
                 "(ConstArray[%d]=%ld < 0)\n", i, ConstArray[i]);
     if(ConstArray[i] >= DArr->Space.Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 140.011: wrong call crtibl_\n"
                 "(ConstArray[%d]=%ld >= %ld)\n",
                 i, ConstArray[i], DArr->Space.Size[i]);
  }

  /* Control specified parallel loop */    /*E0008*/

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.020: wrong call crtibl_\n"
              "(the object is not a parallel loop; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 140.021: wrong call crtibl_\n"
                "(the current context is not the parallel loop; "
                "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.022: wrong call crtibl_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(PL->Rank != 1) 
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.023: wrong call crtibl_\n"
              "(rank of the parallel loop is not equal to 1; "
              "LoopRef=%lx; PLRank=%d;)\n", *LoopRefPtr, PL->Rank);

  /* Control specified index matrix */    /*E0009*/

  MEHandlePtr = TstDVMArray((void *)MEHeader);

  if(MEHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.030: wrong call crtibl_\n"
              "(the index matrix is not a distributed array; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);

  ME = (s_DISARRAY *)MEHandlePtr->pP;

  if(ME->Space.Rank != 2)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.031: wrong call crtibl_\n"
              "(rank of the index matrix is not equal to 2; "
              "MEHeader[0]=%lx; MERank=%d;)\n",
              MEHeader[0], (int)ME->Space.Rank);

  MEAMV = ME->AMView;

  if(MEAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.032: wrong call crtibl_\n"
              "(the index matrix has not been aligned; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);
 
  /* Check if processor system, in which the index matrix
     is mapped, is a subsystem of current processor system */    /*E0010*/

  NotSubsystem(i, DVM_VMS, MEAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 140.033: wrong call crtibl_\n"
         "(the index matrix PS is not a subsystem of the current PS;\n"
         "MEHeader[0]=%lx; MEPSRef=%lx; CurrentPSRef=%lx;)\n",
         MEHeader[0], (uLLng)MEAMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

  /* Checking: first dimension of the index
           matrix must be distributed       */    /*E0011*/

  if(ME->Align[0].Attr != align_NORMAL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.034: wrong call crtibl_\n"
              "(distribution rule of first dimension of the "
              "index matrix is not NORMAL;\nMEHeader[0]=%lx)\n",
              MEHeader[0]);

  /* Checking: second dimension of the index
            matrix must be replicated        */    /*E0012*/

  if(ME->Align[1].Attr != align_COLLAPSE)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.035: wrong call crtibl_\n"
              "(distribution rule of second dimension of the "
              "index matrix is not REPLICATE;\nMEHeader[0]=%lx)\n",
              MEHeader[0]);

  /* Check if index matrix edge widths are equal to 0 */    /*E0013*/

  for(i=0; i < 2; i++)
  {  if(ME->InitLowShdWidth[i] != 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 140.036: wrong call crtibl_\n"
                 "(LowShadowWidth[%d]=%d is not equal to 0;\n"
                 "MEHeader[0]=%lx)\n",
                 i, ME->InitLowShdWidth[i], MEHeader[0]);
     if(ME->InitHighShdWidth[i] != 0) 
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 140.037: wrong call crtibl_\n"
                 "(HighShadowWidth[%d]=%d is not equal to 0;\n"
                 "MEHeader[0]=%lx)\n",
                 i, ME->InitHighShdWidth[i], MEHeader[0]);
  }

  /* Check index matrix element type */    /*E0014*/

  if(ME->TLen != sizeof(int))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.038: wrong call crtibl_\n"
              "(type of index matrix element is not integer; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);

  /* Interrogate parallel loop mapping */    /*E0015*/

  MapRef = ( RTL_CALL, plmap_(LoopRefPtr, &StaticSign) );

  if(MapRef)
  {  /* Mapping created (loop is not empty) */    /*E0016*/

     Map = (s_ARRAYMAP *)((SysHandle *)MapRef)->pP;

     /* Control of distribution rule of the parallel loop */    /*E0017*/

     if(Map->Align[0].Attr != align_NORMAL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.040: wrong call crtibl_\n"
              "(distribution rule of the parallel loop is not NORMAL; "
              "LoopRef=%lx)\n", *LoopRefPtr);

     /* Control of parallel loop indexes */    /*E0018*/

     if(PL->Set[0].Lower < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.041: wrong call crtibl_\n"
              "(loop init index = %ld < 0; LoopRef=%lx)\n",
              PL->Set[0].Lower, *LoopRefPtr);

     if(PL->Set[0].Upper < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.042: wrong call crtibl_\n"
              "(loop last index = %ld < 0; LoopRef=%lx)\n",
              PL->Set[0].Upper, *LoopRefPtr);

     if(PL->Set[0].Lower >= ME->Space.Size[0])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.043: wrong call crtibl_\n"
              "(loop init index = %ld >= %ld; LoopRef=%lx)\n",
              PL->Set[0].Lower, ME->Space.Size[0], *LoopRefPtr);

     if(PL->Set[0].Upper >= ME->Space.Size[0])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.044: wrong call crtibl_\n"
              "(loop last index = %ld >= %ld; LoopRef=%lx)\n",
              PL->Set[0].Upper, ME->Space.Size[0], *LoopRefPtr);

     /* Check if first dimension mapping of index matrix 
        and one dimensional parallel loop mapping are the same */    /*E0019*/

     if(Map->AMView != MEAMV)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 140.050: wrong call crtibl_\n"
                 "(AM view of the index matrix is not equal "
                 "to AM view of the parallel loop;\n"
                 "MEAMViewRef=%lx; PLAMViewRef=%lx;)\n",
                 (uLLng)MEAMV->HandlePtr, (uLLng)Map->AMView->HandlePtr);

     if(ME->Align[0].TAxis != Map->Align[0].TAxis ||
        ME->Align[0].A != Map->Align[0].A || 
        ME->Align[0].Bound != Map->Align[0].Bound   )
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 140.051: wrong call crtibl_\n"
              "(distribution rule of the index matrix is not equal to "
              "distribution rule of the parallel loop;\n"
              "MEHeader[0]=%lx; LoopRef=%lx;)\n",
              MEHeader[0], *LoopRefPtr);

     AMR = Map->AMViewRank; /* dimension of AM representation */    /*E0020*/

     /* Creation of array-buffer mapping */    /*E0021*/

     dvm_AllocArray(s_ALIGN, AMR+2, Align);

     for(i=1; i <= AMR; i++)
         Align[i+1] = Map->Align[i];

     Align[0] = Map->Align[0];

     Align[1].Attr  = align_COLLAPSE;
     Align[1].Axis  = 2;
     Align[1].TAxis = 0;
     Align[1].A     = 0;
     Align[1].B     = 0;
     Align[1].Bound = 0;
                    
     dvm_FreeArray(Map->Align);
     Map->Align = Align;       /* rules of mapping of array-buffer
                                  on AM loop representation */    /*E0022*/
     Map->ArrayRank = 2;       /* dimension of array-buffer */    /*E0023*/

     /* Create array-buffer */    /*E0024*/

     TLen = DArr->TLen;

     BufferHeader[4] = PL->Set[0].Lower;
     BufferHeader[5] = 1;

     SizeArray[0]    = PL->Set[0].Size;
     SizeArray[1]    = ME->Space.Size[1] - 1;

     ShdWidthArray[0] = 0;
     ShdWidthArray[1] = 0;

     ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &BR,
                        &TLen, SizeArray, StaticSignPtr, &StaticSign,
                        ShdWidthArray, ShdWidthArray) );

     /* Array-buffer mapping */    /*E0025*/

     Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
     ( RTL_CALL, delarm_(&MapRef) );

     /* Initialize staructure with info for buffer loading */    /*E0026*/

     dvm_AllocStruct(s_IDBUF, IdBuf);

     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     BArr->IdBuf = IdBuf;

     IdBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                array */    /*E0027*/
     IdBuf->MEHandlePtr = MEHandlePtr; /* reference to index
                                          matrix Handle */    /*E0028*/
     IdBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0029*/
     IdBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0030*/
     IdBuf->LoadAMHandlePtr = NULL; /* reference to Handle of AM
                                       started the buffer loading */    /*E0031*/
     IdBuf->LoadEnvInd  = 0;    /* index of context started
                                   the buffer loading */    /*E0032*/
     IdBuf->IBG          = NULL; /* reference to buffer group,
                                    to which the buffer belongs */    /*E0033*/

     /* Information left by StartLoadBuffer function
        for WaitLoadBuffer function */    /*E0034*/

     IdBuf->MEReq        = NULL;
     IdBuf->DASendBuf    = NULL; 
     IdBuf->DALocalBlock = NULL; 
     IdBuf->DARecvBuf    = NULL; 
     IdBuf->DARecvReq    = NULL; 
     IdBuf->DARecvSize   = NULL; 

     /* Save replicated dimension coordinates of remote array */    /*E0035*/

     for(i=0; i < AR; i++)
         IdBuf->ConstArray[i] = ConstArray[i];

     IdBuf->DistrAxis = k; /* number of distributed dimension
                              of remote array - 1 */    /*E0036*/
  }
  else
  {  /* Mapping not created (empty loop) */    /*E0037*/

     BufferHeader[0] = 0;

     /* Registaration of remote access buffer header */    /*E0038*/

     if(DACount >= MaxDACount)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 140.060: wrong call crtibl_\n"
            "(DistrArray Count = Max DistrArray Count(%d))\n",
            MaxDACount);

     DAHeaderAddr[DACount] = BufferHeader;
     DACount++;
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtibl_,"BufferHandlePtr=%lx; IsLocal=%ld\n",
                           BufferHeader[0], Res);

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0039*/
  DVMFTimeFinish(ret_crtibl_);
  return  (DVM_RET, Res);
}



DvmType  __callstd  crtib_(DvmType  RemArrayHeader[], DvmType  BufferHeader[],
                           void *BasePtr, DvmType *StaticSignPtr,
                           DvmType  MEHeader[], DvmType  ConstArray[])
{ SysHandle     *RemArrayHandlePtr, *MEHandlePtr;
  s_DISARRAY    *DArr, *BArr, *ME;
  s_AMVIEW      *AMV, *MEAMV;
  int            i, j, k, AR;
  ArrayMapRef    MapRef;
  DvmType           StaticSign = 0, TLen, ExtHdrSign = 1, BR = 2, Res = 0;
  DvmType           SizeArray[2], ShdWidthArray[2];
  s_IDBUF       *IdBuf;

  DVMFTimeStart(call_crtib_);

  if(RTL_TRACE)
     dvm_trace(call_crtib_,
        "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; BufferHeader=%lx;\n"
        "BasePtr=%lx; StaticSign=%ld; MEHeader=%lx; MEHandlePtr=%lx;\n",
        (uLLng)RemArrayHeader, RemArrayHeader[0], (uLLng)BufferHeader,
        (uLLng)BasePtr, *StaticSignPtr, (uLLng)MEHeader, MEHeader[0]);

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.000: wrong call crtib_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  RemArrayHandlePtr = TstDVMArray((void *)RemArrayHeader);

  if(RemArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 141.001: wrong call crtib_\n"
        "(the object is not a remote distributed array;\n"
        "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  DArr = (s_DISARRAY *)RemArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtib_))
     {  for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
     }
  }

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.002: wrong call crtib_\n"
              "(the remote array has not been aligned; "
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);
 
  /* Check if processor system the remote array is mapped on
     is a subsystem of the current processor system */    /*E0040*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 141.003: wrong call crtib_\n"
       "(the remote array PS is not a subsystem of the current PS;\n"
       "RemArrayHeader[0]=%lx; RemArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       RemArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Checking: remote array must exactly have one
     distributed dimention */    /*E0041*/

  for(i=0,j=0; i < AR; i++)
  {  if(DArr->Align[i].Attr == align_NORMAL)
     {  j++;    /* number of remote array distributed dimensions */    /*E0042*/
        k = i;  /* remote array distributed dimension number */    /*E0043*/
     }
  }

  if(j != 1)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.004: wrong call crtib_\n"
              "(distributed dimension number of the remote array "
              "is not equal to 1;\n"
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  /* Check ConstArray array */    /*E0044*/

  for(i=0; i < AR; i++)
  {  if(i == k)
        continue;  /* distributed dimension is not to be checked */    /*E0045*/

     if(ConstArray[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 141.010: wrong call crtib_\n"
                 "(ConstArray[%d]=%ld < 0)\n", i, ConstArray[i]);
     if(ConstArray[i] >= DArr->Space.Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 141.011: wrong call crtib_\n"
                 "(ConstArray[%d]=%ld >= %ld)\n",
                 i, ConstArray[i], DArr->Space.Size[i]);
  }

  /* Check index matrix */    /*E0046*/

  MEHandlePtr = TstDVMArray((void *)MEHeader);

  if(MEHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.030: wrong call crtib_\n"
              "(the index matrix is not a distributed array; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);

  ME = (s_DISARRAY *)MEHandlePtr->pP;

  if(ME->Space.Rank != 2)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.031: wrong call crtib_\n"
              "(rank of the index matrix is not equal to 2; "
              "MEHeader[0]=%lx; MERank=%d;)\n",
              MEHeader[0], (int)ME->Space.Rank);

  MEAMV = ME->AMView;

  if(MEAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.032: wrong call crtib_\n"
              "(the index matrix has not been aligned; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);
 
  /* Check if processor system the index matrix is mapped on
     is a subsystem of the current processor system */    /*E0047*/

  NotSubsystem(i, DVM_VMS, MEAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 141.033: wrong call crtib_\n"
         "(the index matrix PS is not a subsystem of the current PS;\n"
         "MEHeader[0]=%lx; MEPSRef=%lx; CurrentPSRef=%lx;)\n",
         MEHeader[0], (uLLng)MEAMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

  /* Checking: 1-st dimension of index matrix
     must be distributed */    /*E0048*/

  if(ME->Align[0].Attr != align_NORMAL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.034: wrong call crtib_\n"
              "(distribution rule of first dimension of the "
              "index matrix is not NORMAL;\nMEHeader[0]=%lx)\n",
              MEHeader[0]);

  /* Checking: 2-nd dimension of index matrix
     must be replicated */    /*E0049*/

  if(ME->Align[1].Attr != align_COLLAPSE)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.035: wrong call crtib_\n"
              "(distribution rule of second dimension of the "
              "index matrix is not REPLICATE;\nMEHeader[0]=%lx)\n",
              MEHeader[0]);

  /* Checking: index matrix edge wigths must be equal to 0 */    /*E0050*/

  for(i=0; i < 2; i++)
  {  if(ME->InitLowShdWidth[i] != 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 141.036: wrong call crtib_\n"
                 "(LowShadowWidth[%d]=%d is not equal to 0;\n"
                 "MEHeader[0]=%lx)\n",
                 i, ME->InitLowShdWidth[i], MEHeader[0]);
     if(ME->InitHighShdWidth[i] != 0) 
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 141.037: wrong call crtib_\n"
                 "(HighShadowWidth[%d]=%d is not equal to 0;\n"
                 "MEHeader[0]=%lx)\n",
                 i, ME->InitHighShdWidth[i], MEHeader[0]);
  }

  /* Check index matrix element types */    /*E0051*/

  if(ME->TLen != sizeof(int))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.038: wrong call crtib_\n"
              "(type of index matrix element is not integer; "
              "MEHeader[0]=%lx)\n", MEHeader[0]);

  /* interrogation of index matrix map */    /*E0052*/

  MapRef = ( RTL_CALL, arrmap_(MEHeader, &StaticSign) );

  /* Create array-buffer */    /*E0053*/

  TLen = DArr->TLen;

  BufferHeader[4] = 0;
  BufferHeader[5] = 1;

  SizeArray[0]    = ME->Space.Size[0];
  SizeArray[1]    = ME->Space.Size[1] - 1;

  ShdWidthArray[0] = 0;
  ShdWidthArray[1] = 0;

  ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &BR,
                     &TLen, SizeArray, StaticSignPtr, &StaticSign,
                     ShdWidthArray, ShdWidthArray) );

  /* Mapping array-buffer */    /*E0054*/

  Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
  ( RTL_CALL, delarm_(&MapRef) );

  /* Initialize structure with information of buffer loading */    /*E0055*/

  dvm_AllocStruct(s_IDBUF, IdBuf);

  BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
  BArr->IdBuf = IdBuf;

  IdBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to
                                             remote array */    /*E0056*/
  IdBuf->MEHandlePtr = MEHandlePtr; /* reference to Handle of
                                       index matrix */    /*E0057*/
  IdBuf->IsLoad      = 0;  /* flag:buffer is loaded */    /*E0058*/
  IdBuf->LoadSign    = 0;  /* flag:buffer is being loaded*/    /*E0059*/
  IdBuf->LoadAMHandlePtr = NULL; /* reference to Handle of AM
                                    which started buffer loading */    /*E0060*/
  IdBuf->LoadEnvInd  = 0;     /* index of context
                                 which started buffer loading */    /*E0061*/
  IdBuf->IBG          = NULL; /* reference to buffer group
                                 the buffer is belong to */    /*E0062*/

  /* Information left by StartLoadBuffer function
        for WaitLoadBuffer function */    /*E0063*/

  IdBuf->MEReq        = NULL;
  IdBuf->DASendBuf    = NULL; 
  IdBuf->DALocalBlock = NULL; 
  IdBuf->DARecvBuf    = NULL; 
  IdBuf->DARecvReq    = NULL; 
  IdBuf->DARecvSize   = NULL; 

  /* Save replicated dimension coordinates of remote array */    /*E0064*/

  for(i=0; i < AR; i++)
      IdBuf->ConstArray[i] = ConstArray[i];

  IdBuf->DistrAxis = k; /* number of distributed dimension
                              of remote array - 1 */    /*E0065*/

  if(RTL_TRACE)
     dvm_trace(ret_crtib_,"BufferHandlePtr=%lx; IsLocal=%ld\n",
                          BufferHeader[0], Res);

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0066*/
  DVMFTimeFinish(ret_crtib_);
  return  (DVM_RET, Res);
}


/* ---------------------------------------------------------- */    /*E0067*/


DvmType  __callstd loadib_(DvmType   BufferHeader[], DvmType  *RenewSignPtr)
{ int           i;
  s_DISARRAY   *BArr, *DArr;
  s_IDBUF      *IdBuf;
  DvmType          CopyRegim = 0;
  s_AMVIEW     *AMV;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0068*/
  DVMFTimeStart(call_loadib_);

  /* Forward to the next elements of message tag circle tag_IdAccess 
     for the current processor system */    /*E0069*/

  DVM_VMS->tag_IdAccess++;

  if((DVM_VMS->tag_IdAccess - (msg_IdAccess)) >= TagCount)
     DVM_VMS->tag_IdAccess = msg_IdAccess;

  /* ----------------------------------------------- */    /*E0070*/

  if(RTL_TRACE)
     dvm_trace(call_loadib_,
         "BufferHeader=%lx; BufferHandlePtr=%lx; RenewSign=%ld;\n",
         (uLLng)BufferHeader, BufferHeader[0], *RenewSignPtr);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 143.000: wrong call loadib_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0071*/

     if(TstDVMArray(BufferHeader) == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.000: wrong call loadib_\n"
           "(the object is not a distributed array;\n"
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     IdBuf = BArr->IdBuf;

     if(IdBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.001: wrong call loadib_\n"
           "(the distributed array is not a buffer; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     if(IdBuf->IBG && IdBuf->IBG->LoadSign == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.002: wrong call loadib_\n"
           "(the buffer is a member of the indirect access group;\n"
           "BufferHeader[0]=%lx; IndirectAccessGroupRef=%lx)\n",
           BufferHeader[0], (uLLng)IdBuf->IBG->HandlePtr);

     if(IdBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.003: wrong call loadib_\n"
           "(the buffer loading has already been started; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     AMV  = BArr->AMView;
 
     /* Check if processor system, on which array-buffer
        is mapped, subsystem of current processor system */    /*E0072*/

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.004: wrong call loadib_\n"
           "(the buffer PS is not a subsystem of the current PS;\n"
           "BufferHeader[0]=%lx; BufferPSRef=%lx; CurrentPSRef=%lx)\n",
           BufferHeader[0], (uLLng)AMV->VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     DArr = (s_DISARRAY *)IdBuf->DAHandlePtr->pP;
     AMV  = DArr->AMView;
 
     /* Check if processor system, on which remote array
        is mapped, subsystem of current processor system */    /*E0073*/

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.005: wrong call loadib_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)IdBuf->DAHandlePtr, (uLLng)AMV->VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     IdBuf->LoadSign = 1;                      /* flag: buffer
                                                  is loading */    /*E0074*/
     IdBuf->LoadAMHandlePtr = CurrAMHandlePtr; /* reference to Handle AM
                                                  started loading */    /*E0075*/
     IdBuf->LoadEnvInd  = gEnvColl->Count - 1; /* index of context
                                                  started buffer
                                                  loading */    /*E0076*/ 

     if(IdBuf->IsLoad == 0 || *RenewSignPtr)
        ( RTL_CALL, StartLoadBuffer(BArr) );/* buffer loading start */    /*E0077*/
  }

  if(RTL_TRACE)
     dvm_trace(ret_loadib_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0078*/
  DVMFTimeFinish(ret_loadib_);
  return  (DVM_RET, 0);
}



DvmType  __callstd waitib_(DvmType   BufferHeader[])
{ int           i;
  s_DISARRAY   *BArr;
  s_IDBUF      *IdBuf;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0079*/
  DVMFTimeStart(call_waitib_);

  if(RTL_TRACE)
     dvm_trace(call_waitib_,"BufferHeader=%lx; BufferHandlePtr=%lx;\n",
                            (uLLng)BufferHeader, BufferHeader[0]);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 143.020: wrong call waitib_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0080*/

     if(TstDVMArray(BufferHeader) == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.020: wrong call waitib_\n"
           "(the object is not a distributed array;\n"
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     IdBuf = BArr->IdBuf;

     if(IdBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.022: wrong call waitib_\n"
           "(the distributed array is not a buffer; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     i = gEnvColl->Count - 1;       /* current context index */    /*E0081*/

     if(IdBuf->LoadAMHandlePtr != CurrAMHandlePtr)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 143.023: wrong call waitib_\n"
            "(the buffer loading was not started by the "
            "current subtask;\nBufferHeader[0]=%lx; LoadEnvIndex=%d; "
            "CurrentEnvIndex=%d)\n",
            BufferHeader[0], IdBuf->LoadEnvInd, i);

     if(IdBuf->LoadSign == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 143.024: wrong call waitib_\n"
           "(the buffer loading has not been started; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     /* Waiting completion of buffer loading */    /*E0082*/

     ( RTL_CALL, WaitLoadBuffer(BArr) );

     IdBuf->LoadSign = 0;           /* turn off current
                                       buffer loading */    /*E0083*/
     IdBuf->IsLoad   = 1;           /* flag: buffer loaded */    /*E0084*/
     IdBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                       started buffer loading */    /*E0085*/
     IdBuf->LoadEnvInd = 0;         /* index of context started
                                       buffer loading */    /*E0086*/
  }

  if(RTL_TRACE)
     dvm_trace(ret_waitib_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0087*/
  DVMFTimeFinish(ret_waitib_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delib_(DvmType  BufferHeader[])
{ int             i;
  SysHandle      *BHandlePtr;
  s_DISARRAY     *BArr;
  void           *CurrAM;
  s_IDBUFGROUP   *IBG;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0088*/
  DVMFTimeStart(call_delib_);

  if(RTL_TRACE)
     dvm_trace(call_delib_,"BufferHeader=%lx; BufferHandlePtr=%lx;\n",
                           (uLLng)BufferHeader, BufferHeader[0]);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 141.040: wrong call delib_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0089*/

     BHandlePtr = TstDVMArray(BufferHeader);

     if(BHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 141.040: wrong call delib_\n"
           "(the object is not a distributed array;\n"
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)BHandlePtr->pP;

     if(BArr->IdBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 141.042: wrong call delib_\n"
           "(the distributed array is not a buffer; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);
  
     /* Check if buffer is created in current subtask */    /*E0090*/
      
     i      = gEnvColl->Count - 1;     /* current context index */    /*E0091*/
     CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0092*/

     if(BHandlePtr->CrtAMHandlePtr != CurrAM)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 141.043: wrong call delib_\n"
           "(the buffer was not created by the current subtask;\n"
           "BufferHeader[0]=%lx; BufferEnvIndex=%d; "
           "CurrentEnvIndex=%d)\n",
           BufferHeader[0], BHandlePtr->CrtEnvInd, i);

     if(BArr->IdBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 141.044: wrong call delib_\n"
           "(the buffer loading has not been completed; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     IBG = BArr->IdBuf->IBG;

     if(IBG)
     {  if(IBG->LoadSign)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 141.045: wrong call delib_\n"
               "(the group loading has not been completed;\n"
               "BufferHeader[0]=%lx; IndirectAccessGroupRef=%lx)\n",
               BufferHeader[0], (uLLng)IBG->HandlePtr);
     }

     ( RTL_CALL, delda_(BufferHeader) );
  }
  else
  {  /* Cancel empty buffer */    /*E0093*/

     for(i=0; i < DACount; i++)
     {  if(DAHeaderAddr[i] == BufferHeader)
        {  for( ; i < DACount; i++)
               DAHeaderAddr[i] = DAHeaderAddr[i+1];
           DACount--;
           break;
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_delib_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0094*/
  DVMFTimeFinish(ret_delib_);
  return  (DVM_RET, 0);
}


/* ---------------------------------------------------- */    /*E0095*/

            
IndirectAccessGroupRef __callstd crtig_(DvmType  *StaticSignPtr, DvmType  *DelBufSignPtr)
{ SysHandle               *IBGHandlePtr;
  s_IDBUFGROUP            *IBG;
  IndirectAccessGroupRef   Res;

  DVMFTimeStart(call_crtig_);

  if(RTL_TRACE)
     dvm_trace(call_crtig_,"StaticSign=%ld; DelBufSign=%ld;\n",
                           *StaticSignPtr, *DelBufSignPtr);

  dvm_AllocStruct(s_IDBUFGROUP, IBG);
  dvm_AllocStruct(SysHandle, IBGHandlePtr);

  IBG->Static          = (byte)*StaticSignPtr;
  IBG->IB              = coll_Init(IdBufGrpCount, IdBufGrpCount, NULL);
  IBG->DelBuf          = (byte)*DelBufSignPtr; /* flag of buffer cancelling
                                                  together with group */    /*E0096*/ 
  IBG->IsLoad          = 0;    /* group is not loaded */    /*E0097*/
  IBG->LoadSign        = 0;    /* group loading not started */    /*E0098*/
  IBG->LoadAMHandlePtr = NULL; /* reference to Handle AM started
                                  group loading */    /*E0099*/
  IBG->LoadEnvInd      = 0;    /* index of context in which
                                  group loading started */    /*E0100*/ 

  *IBGHandlePtr  = genv_InsertObject(sht_IdBufGroup, IBG);
  IBG->HandlePtr = IBGHandlePtr; /* pointer to own Handle */    /*E0101*/

  if(TstObject)
     InsDVMObj((ObjectRef)IBGHandlePtr);

  Res = (IndirectAccessGroupRef)IBGHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_crtig_,"IndirectAccessGroupRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0102*/
  DVMFTimeFinish(ret_crtig_);
  return  (DVM_RET, Res);
}



DvmType __callstd insib_(IndirectAccessGroupRef *IndirectAccessGroupRefPtr, DvmType  BufferHeader[])
{ SysHandle       *IBGHandlePtr, *BHandlePtr;
  s_IDBUFGROUP    *IBG;
  s_IDBUF         *IdBuf;
  int              i;
  void            *CurrAM;
  s_DISARRAY      *BArr;

  StatObjectRef = (ObjectRef)BufferHeader[0];  /* for statistics */    /*E0103*/
  DVMFTimeStart(call_insib_);

  if(RTL_TRACE)
     dvm_trace(call_insib_,
         "IndirectAccessGroupRefPtr=%lx; IndirectAccessGroupRef=%lx; "
         "BufferHeader=%lx; BufferHeader[0]=%lx\n",
         (uLLng)IndirectAccessGroupRefPtr, *IndirectAccessGroupRefPtr,
         (uLLng)BufferHeader, BufferHeader[0]);

  IBGHandlePtr = (SysHandle *)*IndirectAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(IndirectAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.010: wrong call insib_\n"
            "(the indirect access group is not a DVM object; "
            "IndirectAccessGroupRef=%lx)\n",*IndirectAccessGroupRefPtr);
  }

  if(IBGHandlePtr->Type != sht_IdBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 144.011: wrong call insib_\n"
          "(the object is not an indirect access group;\n"
          "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  IBG = (s_IDBUFGROUP *)IBGHandlePtr->pP;
  
  /* Check if buffer group is created in current subtask */    /*E0104*/
      
  i      = gEnvColl->Count - 1;     /* current context index */    /*E0105*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0106*/

  if(IBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 144.012: wrong call insib_\n"
        "(the indirect access group was not created by the "
        "current subtask;\n"
        "IndirectAccessGroupRef=%lx; IndirectAccessGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *IndirectAccessGroupRefPtr, IBGHandlePtr->CrtEnvInd, i);

  /* Check if group loading is finished */    /*E0107*/

  if(IBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.013: wrong call insib_\n"
           "(the group loading has not been completed; "
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  /* Control of buffer included into group */    /*E0108*/

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 144.014: wrong call insib_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0109*/

     BHandlePtr = TstDVMArray(BufferHeader);

     if(BHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 144.014: wrong call insib_\n"
                 "(the object is not a distributed array;\n"
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)BHandlePtr->pP;
     IdBuf = BArr->IdBuf;

     if(IdBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.016: wrong call insib_\n"
           "(the distributed array is not a buffer; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);
  
     /* Check if buffer is created in current subtask */    /*E0110*/
      
     i      = gEnvColl->Count - 1;     /* current context index */    /*E0111*/
     CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0112*/

     if(BHandlePtr->CrtAMHandlePtr != CurrAM)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.017: wrong call insib_\n"
           "(the buffer was not created by the current subtask;\n"
           "BufferHeader[0]=%lx; BufferEnvIndex=%d; "
           "CurrentEnvIndex=%d)\n",
           BufferHeader[0], BHandlePtr->CrtEnvInd, i);

     /* Check if buffer is included into some other group */    /*E0113*/

     if(IdBuf->IBG && IdBuf->IBG != IBG)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.018: wrong call insib_\n"
            "(the buffer has already been inserted "
            "in the indirect access group;\n"
            "BufferHeader[0]=%lx; IndirectAccessGroupRef=%lx)\n",
            BufferHeader[0], (uLLng)IdBuf->IBG->HandlePtr);

     /* Check if buffer loading is finished */    /*E0114*/

     if(IdBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.019: wrong call insib_\n"
            "(the buffer loading has not been completed; "
            "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     if(IdBuf->IBG == NULL)
     {  /* Buffer is not included into group */    /*E0115*/

        coll_Insert(&IBG->IB, BArr); /* add into list of buffers
                                        included into group */    /*E0116*/
           
        IdBuf->IBG = IBG;            /* fix buffer group for buffer */    /*E0117*/
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_insib_," \n");

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for
                                                            statistics */    /*E0118*/
  DVMFTimeFinish(ret_insib_);
  return  (DVM_RET, 0);
}



DvmType __callstd loadig_(IndirectAccessGroupRef *IndirectAccessGroupRefPtr, DvmType  *RenewSignPtr)
{ int             i, NotSubSys;
  SysHandle      *IBGHandlePtr;
  s_IDBUFGROUP   *IBG;
  s_DISARRAY     *BArr, *DArr;
  s_IDBUF        *IdBuf;
  s_VMS          *VMS;
  DvmType           *BufferHeader;
  DvmType            RenewSign = 1;

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for
                                                            statistics */    /*E0119*/
  DVMFTimeStart(call_loadig_);

  if(RTL_TRACE)
     dvm_trace(call_loadig_,
         "IndirectAccessGroupRefPtr=%lx; IndirectAccessGroupRef=%lx; "
         "RenewSign=%d\n",
         (uLLng)IndirectAccessGroupRefPtr, *IndirectAccessGroupRefPtr,
         *RenewSignPtr);

  IBGHandlePtr = (SysHandle *)*IndirectAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(IndirectAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.030: wrong call loadig_\n"
            "(the indirect access group is not a DVM object; "
            "IndirectAccessGroupRef=%lx)\n",*IndirectAccessGroupRefPtr);
  }

  if(IBGHandlePtr->Type != sht_IdBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.031: wrong call loadig_\n"
           "(the object is not an indirect access group;\n"
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  IBG = (s_IDBUFGROUP *)IBGHandlePtr->pP;

  /* Check if all buffer group and its remote arrays
     mapped on subsystems of current processor system */    /*E0120*/
  
  for(i=0; i < IBG->IB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &IBG->IB, i);

     VMS = BArr->AMView->VMS;          /* processor system
                                          on which buffer mapped */    /*E0121*/

     NotSubsystem(NotSubSys, DVM_VMS, VMS)

     if(NotSubSys)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.032: wrong call loadig_\n"
           "(the buffer PS is not a subsystem of the current PS;\n"
           "BufferHeader[0]=%lx; BufferPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)BArr->HandlePtr, (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     IdBuf = BArr->IdBuf;
     DArr = (s_DISARRAY *)IdBuf->DAHandlePtr->pP;
     VMS  = DArr->AMView->VMS;         /* processor system
                                          on which array mapped */    /*E0122*/
 
     NotSubsystem(NotSubSys, DVM_VMS, VMS)

     if(NotSubSys)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.033: wrong call loadig_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)IdBuf->DAHandlePtr, (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);
  }

  /* Check if group loading is started */    /*E0123*/

  if(IBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.034: wrong call loadig_\n"
           "(the group loading has already been started; "
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  /* Start group loading */    /*E0124*/

  IBG->LoadSign = 1;                      /* flag: group is
                                             loading */    /*E0125*/
  IBG->LoadAMHandlePtr = CurrAMHandlePtr; /* reference to Handle AM
                                             started loading */    /*E0126*/
  IBG->LoadEnvInd  = gEnvColl->Count - 1; /* index of context
                                             started group loading */    /*E0127*/ 

  if(IBG->IsLoad == 0 || *RenewSignPtr)
  {  /* Start group buffer loading */    /*E0128*/

     for(i=0; i < IBG->IB.Count; i++)
     {  BArr = coll_At(s_DISARRAY *, &IBG->IB, i);
        BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;
        ( RTL_CALL, loadib_(BufferHeader, &RenewSign) );
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffLoadIG);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_loadig_,"\n");

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for
                                                            statistics */    /*E0129*/
  DVMFTimeFinish(ret_loadig_);
  return  (DVM_RET, 0);
}



DvmType __callstd waitig_(IndirectAccessGroupRef *IndirectAccessGroupRefPtr)
{ SysHandle       *IBGHandlePtr;
  s_IDBUFGROUP    *IBG;
  int              i;
  s_DISARRAY      *BArr;
  DvmType            *BufferHeader;

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for
                                                            statistics */    /*E0130*/
  DVMFTimeStart(call_waitig_);

  if(RTL_TRACE)
     dvm_trace(call_waitig_,
         "IndirectAccessGroupRefPtr=%lx; IndirectAccessGroupRef=%lx;\n",
         (uLLng)IndirectAccessGroupRefPtr, *IndirectAccessGroupRefPtr);

  IBGHandlePtr = (SysHandle *)*IndirectAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(IndirectAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.040: wrong call waitig_\n"
            "(the indirect access group is not a DVM object; "
            "IndirectAccessGroupRef=%lx)\n",*IndirectAccessGroupRefPtr);
  }

  if(IBGHandlePtr->Type != sht_IdBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.041: wrong call waitig_\n"
           "(the object is not an indirect access group;\n"
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  IBG = (s_IDBUFGROUP *)IBGHandlePtr->pP;

  i = gEnvColl->Count - 1;       /* current context index */    /*E0131*/

  if(IBG->LoadAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 144.042: wrong call waitig_\n"
         "(the group loading was not started by the "
         "current subtask;\nIndirectAccessGroupRef=%lx; "
         "LoadEnvIndex=%d; CurrentEnvIndex=%d)\n",
         *IndirectAccessGroupRefPtr, IBG->LoadEnvInd, i);

  if(IBG->LoadSign == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 144.043: wrong call waitig_\n"
        "(the group loading has not been started; "
        "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  /* Waiting completion of all group buffer loading */    /*E0132*/

  for(i=0; i < IBG->IB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &IBG->IB, i);
     BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;
     ( RTL_CALL, waitib_(BufferHeader) );
  }

  IBG->LoadSign = 0;           /* turn off flag of current
                                  group buffer loading */    /*E0133*/
  IBG->IsLoad   = 1;           /* flag: group loaded */    /*E0134*/
  IBG->LoadAMHandlePtr = NULL; /* reference to Handle AM started
                                  buffer group loading */    /*E0135*/
  IBG->LoadEnvInd = 0;         /* index of context started 
                                  buffer group loading */    /*E0136*/

  if(RTL_TRACE)
     dvm_trace(ret_waitig_,"\n");

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for 
                                                            statistics */    /*E0137*/
  DVMFTimeFinish(ret_waitig_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delig_(IndirectAccessGroupRef *IndirectAccessGroupRefPtr)
{ SysHandle       *IBGHandlePtr;
  s_IDBUFGROUP    *IBG;
  int              i;
  void            *CurrAM;

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for 
                                                            statistics */    /*E0138*/
  DVMFTimeStart(call_delig_);

  if(RTL_TRACE)
     dvm_trace(call_delig_,
         "IndirectAccessGroupRefPtr=%lx; IndirectAccessGroupRef=%lx;\n",
         (uLLng)IndirectAccessGroupRefPtr, *IndirectAccessGroupRefPtr);

  IBGHandlePtr = (SysHandle *)*IndirectAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(IndirectAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 144.050: wrong call delig_\n"
            "(the indirect access group is not a DVM object; "
            "IndirectAccessGroupRef=%lx)\n",*IndirectAccessGroupRefPtr);
  }

  if(IBGHandlePtr->Type != sht_IdBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.051: wrong call delig_\n"
           "(the object is not an indirect access group;\n"
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  IBG = (s_IDBUFGROUP *)IBGHandlePtr->pP;
  
  /* Check if buffer group is created in current subtask */    /*E0139*/
      
  i      = gEnvColl->Count - 1;     /* current context index */    /*E0140*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0141*/

  if(IBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 144.052: wrong call delig_\n"
        "(the indirect access group was not created by the "
        "current subtask;\n"
        "IndirectAccessGroupRef=%lx; IndirectAccessGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *IndirectAccessGroupRefPtr, IBGHandlePtr->CrtEnvInd, i);

  /* Check if group loading is finished */    /*E0142*/

  if(IBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 144.053: wrong call delig_\n"
           "(the group loading has not been completed; "
           "IndirectAccessGroupRef=%lx)\n", *IndirectAccessGroupRefPtr);

  ( RTL_CALL, delobj_(IndirectAccessGroupRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delig_,"\n");

  StatObjectRef = (ObjectRef)*IndirectAccessGroupRefPtr; /* for
                                                            statistics */    /*E0143*/
  DVMFTimeFinish(ret_delig_);
  return  (DVM_RET, 0);
}


/* ---------------------------------------------- */    /*E0144*/


void   IdBufGroup_Done(s_IDBUFGROUP  *IBG)
{ int                     i;
  s_DISARRAY              *BArr;
  s_IDBUF                 *IdBuf;
  IndirectAccessGroupRef   IdBufGrpRef;
  DvmType                 *BufferHeader;

  if(RTL_TRACE)
     dvm_trace(call_IdBufGroup_Done, "IndirectAccessGroupRef=%lx;\n",
                                     (uLLng)IBG->HandlePtr);

  if(IBG->LoadSign)
  {  IdBufGrpRef = (IndirectAccessGroupRef)IBG->HandlePtr;
     ( RTL_CALL, waitig_(&IdBufGrpRef) );
  }

  /* Cancel buffers included into group */    /*E0145*/

  for(i=0; i < IBG->IB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &IBG->IB, i);
     IdBuf = BArr->IdBuf;
     IdBuf->IBG = NULL;   /* turn off reference to cancelled group */    /*E0146*/
     BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;

     if(IdBuf->LoadSign)
        ( RTL_CALL, waitib_(BufferHeader) );

     if(IBG->DelBuf)
     {  if(DelObjFlag)   /* working from explicit group cancelation */    /*E0147*/
           ( RTL_CALL, delib_(BufferHeader) );
        else             /* implicit buffer group cancelation */    /*E0148*/
        {  if(BArr->Static == 0)
              ( RTL_CALL, delib_(BufferHeader) );
        }
     }
  }

  dvm_FreeArray(IBG->IB.List);

  if(TstObject)
     DelDVMObj((ObjectRef)IBG->HandlePtr);

  /* Cancel own Handle */    /*E0149*/

  IBG->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(IBG->HandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_IdBufGroup_Done, " \n");

  (DVM_RET);

  return;
}


/* ----------------------------------------------- */    /*E0150*/


void  StartLoadBuffer(s_DISARRAY  *BArr)
{ s_IDBUF       *IdBuf;
  s_DISARRAY    *ME, *DArr;
  int            i, j, k, DistrAxis, MELower0, MEUpper0, MESize1,
                 MESize, DAProcCount, Proc, DALower, DAUpper,
                 MECount, MECoord, Size, BProcCount, TLen;
  DvmType           LI;
  uLLng          *BHeader, *MEHeader;
  int           *MEIntPtr, *MEIntPtr0, *MEIntPtr1, *DARecvSize;
  s_AMVIEW      *DAAMV, *BAMV;
  s_VMS         *DAVMS, *BVMS;
  RTL_Request   *MESendReq, *DARecvReq, *MEReq;
  s_BLOCK      **DALocalBlock = NULL, *DABlock = NULL,
               **MELocalBlock = NULL, *MEBlock = NULL;
  s_BLOCK       *LocalBlock = NULL;
  char          *CharPtr;
  void         **DARecvBuf, **MERecvBuf, **DASendBuf;

  if(RTL_TRACE)
     dvm_trace(call_StartLoadBuffer,
               "BufferHandlePtr=%lx;\n", (uLLng)BArr->HandlePtr);

  TLen = BArr->TLen;

  IdBuf = BArr->IdBuf;  /* structure with information of the buffer */    /*E0151*/

  ME   = (s_DISARRAY *)IdBuf->MEHandlePtr->pP;  /* index matrix
                                                   descriptor */    /*E0152*/
  DArr = (s_DISARRAY *)IdBuf->DAHandlePtr->pP;  /* remote array
                                                   descriptor */    /*E0153*/
  BHeader  = (uLLng *)BArr->HandlePtr->HeaderPtr; /* buffer header */    /*E0154*/
  MEHeader = (uLLng *)ME->HandlePtr->HeaderPtr;   /* index matrix header */    /*E0155*/
  MESize1 = (int)ME->Space.Size[1]; /* size of 2-nd dimension (replicated)
                                       of index matrix */    /*E0156*/
  DistrAxis = IdBuf->DistrAxis;     /* number of distributed dimension
                                       of remote array -1 */    /*E0157*/

  /******************************************************\
  * Processor that contains local part of the buffer    *
  \******************************************************/    /*E0158*/

  if(BArr->HasLocal)
  {  /* local part of the buffer is in the current processor */    /*E0159*/

     /* Calculate coordinates and size of local part of index matrix 
        corresponding to the buffer local part */    /*E0160*/

     MELower0 = (int)(BArr->Block.Set[0].Lower + BHeader[4]);
     MEUpper0 = (int)(BArr->Block.Set[0].Upper + BHeader[4]);
     MESize   = (MEUpper0 - MELower0 + 1) * MESize1;

     /* Calculate address of local part of index matrix 
        corresponding to the buffer local part */    /*E0161*/

     LI = MEHeader[1] * MELower0;

     MEIntPtr  = (int *)ME->BasePtr;
     MEIntPtr += (DvmType)(LI + MEHeader[2]);

     /* Start sending index matrix local part 
        to all processors of the processor system
        the remote array is mapped on*/    /*E0162*/

     DAAMV = DArr->AMView; /* AM representation
                              the remote array is mapped on */    /*E0163*/
     DAVMS = DAAMV->VMS;   /* processor system
                              the remote array is mapped on */    /*E0164*/
     DAProcCount = (int)DAVMS->ProcCount;

     dvm_AllocArray(RTL_Request, DAProcCount, MESendReq);
     dvm_AllocArray(s_BLOCK *, DAProcCount, DALocalBlock);
     dvm_AllocArray(s_BLOCK, DAProcCount, DABlock);

     for(i=0; i < DAProcCount; i++)
     {  DALocalBlock[i] = NULL;
        Proc = (int)DAVMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;  /* skip the current processor */    /*E0165*/

        DALocalBlock[i] = GetSpaceLB4Proc(i, DAAMV, &DArr->Space,
                                          DArr->Align, NULL,
                                          &DABlock[i]);
        if(DALocalBlock[i])
           ( RTL_CALL, rtl_Sendnowait((void *)MEIntPtr, MESize,
                                      sizeof(int), Proc,
                                      DVM_VMS->tag_IdAccess,
                                      &MESendReq[i], MESend) );
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     /* Rewrite remote array elements of the current processor 
        to  buffer  */    /*E0166*/

     if(DArr->HasLocal)
     {  /* Remote array local part 
          is in the current processor  */    /*E0167*/

        DALower = (int)DArr->Block.Set[DistrAxis].Lower;
        DAUpper = (int)DArr->Block.Set[DistrAxis].Upper;

        for(i=MELower0,MEIntPtr0=MEIntPtr; i <= MEUpper0;
            i++, MEIntPtr0 += MESize1)
        {  MECount = *MEIntPtr0; /* current number of required coordinates */    /*E0168*/

           for(j=0,MEIntPtr1=MEIntPtr0+1; j < MECount; j++,MEIntPtr1++)
           {  MECoord = *MEIntPtr1; /* the current required coordinate */    /*E0169*/

              if(MECoord < DALower || MECoord > DAUpper)
                 continue; /* the current required coordinate is 
                             out of bounds of remote array local part */    /*E0170*/

              /* Fix required coordinate
                 in the vector of required element */    /*E0171*/

              IdBuf->ConstArray[DistrAxis] = MECoord;

              /* Calculate address of required element in the buffer */    /*E0172*/

              LI = (DvmType)(j + BHeader[1] * (i - BHeader[4]));

              CharPtr  = (char *)BArr->BasePtr;
              CharPtr += (DvmType)(TLen * (LI + BHeader[2]));

              /* Write element in buffer */    /*E0173*/

              GetLocElm(DArr, IdBuf->ConstArray, CharPtr)
           }
        }
     }

     /* Calculate lenghts of messages with remote elements
        which will be received from processors
        that contain remote array local parts */    /*E0174*/

     dvm_AllocArray(int, DAProcCount, DARecvSize);

     for(i=0; i < DAProcCount; i++)
     {  DARecvSize[i] = 0;

        LocalBlock = DALocalBlock[i];

        if(LocalBlock == NULL)
           continue; /* i-th processor does not
                        contain remote array local parts*/    /*E0175*/

        DALower = (int)LocalBlock->Set[DistrAxis].Lower;
        DAUpper = (int)LocalBlock->Set[DistrAxis].Upper;
        Size = 0;

        for(j=MELower0,MEIntPtr0=MEIntPtr; j <= MEUpper0;
            j++, MEIntPtr0 += MESize1)
        {  MECount = *MEIntPtr0; /* current number of required coordinates */    /*E0176*/

           for(k=0,MEIntPtr1=MEIntPtr0+1; k < MECount; k++,MEIntPtr1++)
           {  MECoord = *MEIntPtr1;  /* the current required coordinate */    /*E0177*/

              if(MECoord >= DALower && MECoord <= DAUpper)
                 Size++;
           }
        }

        DARecvSize[i] = Size;
     }

     /* End of index matrix local part sending*/    /*E0178*/

     for(i=0; i < DAProcCount; i++)
     {  if(DALocalBlock[i])
           ( RTL_CALL, rtl_Waitrequest(&MESendReq[i]) );
     }

     dvm_FreeArray(MESendReq);

     /* Free remote array local part blocks
        for processors that will not send remote elements */    /*E0179*/

     for(i=0; i < DAProcCount; i++)
     {  if(DALocalBlock[i] != NULL && DARecvSize[i] == 0)
           DALocalBlock[i] = NULL;
     }

     /* Start receiving messages with remote elements */    /*E0180*/

     dvm_AllocArray(RTL_Request, DAProcCount, DARecvReq);
     dvm_AllocArray(void *, DAProcCount, DARecvBuf);

     for(i=0; i < DAProcCount; i++)
     {  DARecvBuf[i] = NULL;

        if(DARecvSize[i] == 0)
           continue;  /* there will no be remote elements from
                         i-th processor*/    /*E0181*/

        mac_malloc(DARecvBuf[i], void *, DARecvSize[i]*TLen, 0);

        Proc = (int)DAVMS->VProc[i].lP;
        ( RTL_CALL, rtl_Recvnowait(DARecvBuf[i], DARecvSize[i],
                                   TLen, Proc, DVM_VMS->tag_IdAccess,
                                   &DARecvReq[i], 0) );
     }

     /* Save information for WaitLoadBuffer  */    /*E0182*/

     IdBuf->DALocalBlock = DALocalBlock;
     IdBuf->DABlock      = DABlock;
     IdBuf->DARecvBuf    = DARecvBuf;
     IdBuf->DARecvReq    = DARecvReq;
     IdBuf->DARecvSize   = DARecvSize;

  }

  /******************************************************\
  * Processor that  contains local part of remote array *
  \******************************************************/    /*E0183*/

  if(DArr->HasLocal)
  {  /* Remote array has local part in the current processor */    /*E0184*/

     BAMV = BArr->AMView; /* AM representation
                              the buffer is mapped on */    /*E0185*/
     BVMS = BAMV->VMS;    /* processor system
                              the buffer is mapped on */    /*E0186*/
     BProcCount = (int)BVMS->ProcCount;

     dvm_AllocArray(RTL_Request, BProcCount, MEReq);
     dvm_AllocArray(s_BLOCK *, BProcCount, MELocalBlock);
     dvm_AllocArray(s_BLOCK, BProcCount, MEBlock);
     dvm_AllocArray(void *, BProcCount, MERecvBuf);
     dvm_AllocArray(void *, BProcCount, DASendBuf);

     /* determine buffer local parts for
       all processor from the processor system
       the buffer is mapped on */    /*E0187*/

     for(i=0; i < BProcCount; i++)
     {  MELocalBlock[i] = NULL;

        Proc = (int)BVMS->VProc[i].lP;
        if(Proc == MPS_CurrentProc)
           continue;

        MELocalBlock[i] = GetSpaceLB4Proc(i, BAMV, &BArr->Space,
                                          BArr->Align, NULL,
                                          &MEBlock[i]);
     }

     /* Transform buffer local parts into 
        corresponding index matrix local parts */    /*E0188*/

     for(i=0; i < BProcCount; i++)
     {  if(MELocalBlock[i] == NULL)
           continue; /* i-th processor does not
                        contain buffer local parts*/    /*E0189*/

        (MELocalBlock[i])->Set[0].Lower += BHeader[4];
        (MELocalBlock[i])->Set[0].Upper += BHeader[4];
        (MELocalBlock[i])->Set[1].Upper++;
        (MELocalBlock[i])->Set[1].Size++;
     }

     /* Start receiving index matrix localparts */    /*E0190*/

     for(i=0; i < BProcCount; i++)
     {  MERecvBuf[i] = NULL;

        if(MELocalBlock[i] == NULL)
           continue; /* i-th processor does not
                        contain buffer local parts*/    /*E0191*/

        Size = (int)( (MELocalBlock[i])->Set[0].Size *
                      (MELocalBlock[i])->Set[1].Size );
        mac_malloc(MERecvBuf[i], void *, Size*sizeof(int), 0);
        Proc = (int)BVMS->VProc[i].lP;

        ( RTL_CALL, rtl_Recvnowait(MERecvBuf[i], Size, sizeof(int),
                                   Proc, DVM_VMS->tag_IdAccess,
                                   &MEReq[i], 0) );
     }

     /* Process received local parts of index matrix */    /*E0192*/

     DALower = (int)DArr->Block.Set[DistrAxis].Lower;
     DAUpper = (int)DArr->Block.Set[DistrAxis].Upper;

     for(i=0; i < BProcCount; i++)
     {  DASendBuf[i] = NULL;

        if(MELocalBlock[i] == NULL)
           continue; /* i-th processor does not
                        contain buffer local parts*/    /*E0193*/
 
        ( RTL_CALL, rtl_Waitrequest(&MEReq[i]) );

        /* Calculate lenghts of messages with remote elements */    /*E0194*/

        MEUpper0 = (int)(MELocalBlock[i])->Set[0].Size;
        MEIntPtr = (int *)MERecvBuf[i];
        Size = 0;

        for(j=0,MEIntPtr0=MEIntPtr; j < MEUpper0;
            j++, MEIntPtr0 += MESize1)
        {  MECount = *MEIntPtr0; /* current number of required coordinates */    /*E0195*/

           for(k=0,MEIntPtr1=MEIntPtr0+1; k < MECount; k++,MEIntPtr1++)
           {  MECoord = *MEIntPtr1; /* the current required coordinate */    /*E0196*/

              if(MECoord >= DALower && MECoord <= DAUpper)
                 Size++;
           }
        }

        if(Size)
        {  /* lenght of message with remote elements is not equal to 0
              (there are required elements at the current processor)*/    /*E0197*/
          
           mac_malloc(DASendBuf[i], void *, Size*TLen, 0);

           /* Rewrite remote elements into message buffer */    /*E0198*/

           CharPtr = (char *)DASendBuf[i];

           for(j=0,MEIntPtr0=MEIntPtr; j < MEUpper0;
               j++, MEIntPtr0 += MESize1)
           {  MECount = *MEIntPtr0; /* number of required coordinates */    /*E0199*/

              for(k=0,MEIntPtr1=MEIntPtr0+1; k < MECount;
                  k++,MEIntPtr1++)
              {  MECoord = *MEIntPtr1; /* required coordinate */    /*E0200*/

                 if(MECoord < DALower || MECoord > DAUpper)
                    continue; /* required coordinate is out of bounds of
                                 remote array local part */    /*E0201*/

                 /* Fix required coordinate in the vector
                    of required element */    /*E0202*/

                 IdBuf->ConstArray[DistrAxis] = MECoord;

                 /* Write element into the message buffer */    /*E0203*/

                 GetLocElm(DArr, IdBuf->ConstArray, CharPtr)

                 CharPtr += TLen; /* pointer to the next element
                                     in message buffer */    /*E0204*/
              }
           }

           /* Start sending message with remote elements */    /*E0205*/

           Proc = (int)BVMS->VProc[i].lP;
           ( RTL_CALL, rtl_Sendnowait(DASendBuf[i], Size, TLen,
                                      Proc, DVM_VMS->tag_IdAccess,
                                      &MEReq[i], IDSend) );
        }

        mac_free(&MERecvBuf[i]);
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL < 2)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffLoadIB);
     }

     dvm_FreeArray(MELocalBlock);
     dvm_FreeArray(MEBlock);
     dvm_FreeArray(MERecvBuf);

     /* Save information for WaitLoadBuffer */    /*E0206*/

     IdBuf->MEReq = MEReq;
     IdBuf->DASendBuf = DASendBuf;
  }

  if(RTL_TRACE)
     dvm_trace(ret_StartLoadBuffer, " \n");

  (DVM_RET);

  return;
}



void  WaitLoadBuffer(s_DISARRAY  *BArr)
{ s_IDBUF       *IdBuf;
  s_DISARRAY    *ME, *DArr;
  int            i, j, k, m, MELower0, MEUpper0, MESize1, DAProcCount,
                 DALower, DAUpper, MECount, MECoord, BProcCount, TLen,
                 DistrAxis;
  DvmType           LI;
  uLLng          *BHeader, *MEHeader;
  int           *MEIntPtr, *MEIntPtr0, *MEIntPtr1;
  s_AMVIEW      *DAAMV, *BAMV;
  s_VMS         *DAVMS, *BVMS;
  s_BLOCK       *LocalBlock;
  char          *CharPtr, *RemPtr;

  if(RTL_TRACE)
     dvm_trace(call_WaitLoadBuffer,
               "BufferHandlePtr=%lx;\n", (uLLng)BArr->HandlePtr);

  TLen = BArr->TLen;

  IdBuf = BArr->IdBuf;  /* structure with information of the buffer */    /*E0207*/

  ME   = (s_DISARRAY *)IdBuf->MEHandlePtr->pP;  /* index matrix
                                                    descriptor */    /*E0208*/
  DArr = (s_DISARRAY *)IdBuf->DAHandlePtr->pP;  /* remote array
                                                   descriptor */    /*E0209*/
  BHeader  = (uLLng *)BArr->HandlePtr->HeaderPtr; /* buffer header */    /*E0210*/
  MEHeader = (uLLng *)ME->HandlePtr->HeaderPtr;   /* index matrix 
                                                    header */    /*E0211*/
  MESize1 = (int)ME->Space.Size[1]; /* size of the 2-nd dimension (replicated)
                                       of the index matrix */    /*E0212*/
  DistrAxis = IdBuf->DistrAxis;     /* number of distributed dimension of
                                       remote array -1 */    /*E0213*/

  /******************************************************\
  * Processor that  contains local part of the buffer   *
  \******************************************************/    /*E0214*/

  if(BArr->HasLocal)
  {  /* Buffer has local part in the current processor */    /*E0215*/

     /* Calculate coordinates  of local part of index matrix 
        corresponding to the buffer local part */    /*E0216*/

     MELower0 = (int)(BArr->Block.Set[0].Lower + BHeader[4]);
     MEUpper0 = (int)(BArr->Block.Set[0].Upper + BHeader[4]);

     /* Calculate address of local part of index matrix 
        corresponding to the buffer local part */    /*E0217*/

     LI = (DvmType)(MEHeader[1] * MELower0);

     MEIntPtr  = (int *)ME->BasePtr;
     MEIntPtr += (DvmType)(LI + MEHeader[2]);

     DAAMV = DArr->AMView; /* AM representation
                              the remote array is mapped on */    /*E0218*/
     DAVMS = DAAMV->VMS;   /* processor system
                              the remote array is mapped on */    /*E0219*/
     DAProcCount = (int)DAVMS->ProcCount;

     /* Process received remote elements */    /*E0220*/

     for(i=0; i < DAProcCount; i++)
     {  if(IdBuf->DARecvBuf[i] == 0)
           continue; /* there will not be remote elements from i-th processor */    /*E0221*/

        ( RTL_CALL, rtl_Waitrequest(&IdBuf->DARecvReq[i]) );

        RemPtr = (char *)IdBuf->DARecvBuf[i]; /* pointer to the current
                                                 remote element
                                                 in the message buffer */    /*E0222*/

        LocalBlock = IdBuf->DALocalBlock[i];

        DALower = (int)LocalBlock->Set[DistrAxis].Lower;
        DAUpper = (int)LocalBlock->Set[DistrAxis].Upper;

        for(j=MELower0,MEIntPtr0=MEIntPtr; j <= MEUpper0;
            j++, MEIntPtr0 += MESize1)
        {  MECount = *MEIntPtr0; /* current number of required coordinates */    /*E0223*/

           for(k=0,MEIntPtr1=MEIntPtr0+1; k < MECount; k++,MEIntPtr1++)
           {  MECoord = *MEIntPtr1; /* the current required coordinate */    /*E0224*/

              if(MECoord < DALower || MECoord > DAUpper)
                 continue; /* the current required coordinate is 
                             out of bounds of remote array local part */    /*E0225*/

              /* Calculate address of required element in the buffer 
                 of remote elements */    /*E0226*/

              LI = (DvmType)(k + BHeader[1] * (j - BHeader[4]));

              CharPtr  = (char *)BArr->BasePtr;
              CharPtr += (DvmType)(TLen * (LI + BHeader[2]));

              /* Rewrite element in remote element buffer 
                 from the message buffer */    /*E0227*/

              for(m=0; m < TLen; m++,RemPtr++,CharPtr++)
                  *CharPtr = *RemPtr;
           }

           dvm_FreeStruct(IdBuf->DALocalBlock[i]);
           mac_free(&IdBuf->DARecvBuf[i]);
        }
     }

     /* Free memory */    /*E0228*/

     dvm_FreeArray(IdBuf->DALocalBlock);
     dvm_FreeArray(IdBuf->DARecvBuf);
     dvm_FreeArray(IdBuf->DARecvReq);
     dvm_FreeArray(IdBuf->DARecvSize);
  }

  /******************************************************\
  * Processor that  contains local part of remote array *
  \******************************************************/    /*E0229*/

  if(DArr->HasLocal)
  {  /* Remote array has local part in the current processor */    /*E0230*/

     BAMV = BArr->AMView; /* AM representation
                              the buffer is mapped on */    /*E0231*/
     BVMS = BAMV->VMS;    /* processor system
                              the buffer is mapped on */    /*E0232*/
     BProcCount = (int)BVMS->ProcCount;

     /* End of sending remote elements */    /*E0233*/

     for(i=0; i < BProcCount; i++)
     { if(IdBuf->DASendBuf[i] == 0)
          continue; /* remote elements was not been sent to
                       i-th processor */    /*E0234*/
       ( RTL_CALL, rtl_Waitrequest(&IdBuf->MEReq[i]) );
       mac_free(&IdBuf->DASendBuf[i]);
     }

     dvm_FreeArray(IdBuf->MEReq);
     dvm_FreeArray(IdBuf->DASendBuf);
  }

  if(RTL_TRACE)
     dvm_trace(ret_WaitLoadBuffer, " \n");

  (DVM_RET);

  return;
}


#endif  /*  _IDACCESS_C_  */    /*E0235*/
