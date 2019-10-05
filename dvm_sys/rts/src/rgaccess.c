#ifndef _RGACCESS_C_
#define _RGACCESS_C_
/******************/    /*E0000*/

/********************************************\
* Functions of regular access to remote data *
\********************************************/    /*E0001*/

DvmType  __callstd  crtrbl_(DvmType  RemArrayHeader[], DvmType  BufferHeader[],
                           void *BasePtr, DvmType *StaticSignPtr,
                           LoopRef *LoopRefPtr, DvmType  AxisArray[],
                           DvmType  CoeffArray[], DvmType  ConstArray[])
{ SysHandle     *RemArrayHandlePtr, *LoopHandlePtr;
  s_DISARRAY    *DArr, *BArr;
  s_AMVIEW      *AMV;
  s_PARLOOP     *PL;
  int            i, j, k, AR, LR, BR = 0, NormCount = 0, ReplCount = 0,
                 AMR, ALSize;
  ArrayMapRef    MapRef;
  s_ARRAYMAP    *Map;
  DvmType           StaticSign = 0, ai, TLen, ExtHdrSign = 1, Res = 0;
  byte           TstArray[MAXARRAYDIM];
  s_ALIGN       *Align;
  DvmType           SizeArray[MAXARRAYDIM], ShdWidthArray[MAXARRAYDIM];
  s_REGBUF      *RegBuf;

  DVMFTimeStart(call_crtrbl_);

  if(RTL_TRACE)
     dvm_trace(call_crtrbl_,
        "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; BufferHeader=%lx; "
        "BasePtr=%lx; StaticSign=%ld; LoopRefPtr=%lx; LoopRef=%lx;\n",
        (uLLng)RemArrayHeader, RemArrayHeader[0], (uLLng)BufferHeader,
        (uLLng)BasePtr, *StaticSignPtr, (uLLng)LoopRefPtr, *LoopRefPtr);

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.000: wrong call crtrbl_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  RemArrayHandlePtr = TstDVMArray((void *)RemArrayHeader);

  if(RemArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.001: wrong call crtrbl_\n"
              "(the object is not a remote distributed array;\n"
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  DArr = (s_DISARRAY *)RemArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtrbl_))
     {  for(i=0; i < AR; i++)
            tprintf(" AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.002: wrong call crtrbl_\n"
              "(the remote array has not been aligned; "
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);
 
  /* Check if processor system, in which the remote array
     is mapped, is a subsystem of current processor system */    /*E0002*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 130.003: wrong call crtrbl_\n"
       "(the remote array PS is not a subsystem of the current PS;\n"
       "RemArrayHeader[0]=%lx; RemArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       RemArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Control specified parallel loop */    /*E0003*/

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 130.004: wrong call crtrbl_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 130.005: wrong call crtrbl_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;
  LR = PL->Rank;

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.006: wrong call crtrbl_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  /* Interrogate parallel loop mapping */    /*E0004*/

  MapRef = ( RTL_CALL, plmap_(LoopRefPtr, &StaticSign) );

  if(MapRef)
  {  /* Mapping created (loop is not empty) */    /*E0005*/

     Map = (s_ARRAYMAP *)((SysHandle *)MapRef)->pP;

     /* Control of specified parameters and
        array-buffer dimension calculation  */    /*E0006*/

     for(i=0; i < AR; i++)
     {  if(AxisArray[i] < -1)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 130.010: wrong call crtrbl_\n"
                    "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);

        if(AxisArray[i] > LR)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 130.011: wrong call crtrbl_\n"
                    "(AxisArray[%d]=%ld > %d; LoopRef=%lx)\n",
                    i, AxisArray[i], LR, *LoopRefPtr);

        if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           { NormCount++;        /* number of linear selection rules */    /*E0007*/
             BR++;               /* dimension of array-buffer */    /*E0008*/

             if(AxisArray[i] == 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                         "*** RTS err 130.012: wrong call crtrbl_\n"
                         "(AxisArray[%d]=0)\n", i);

             ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
                  ConstArray[i];

             if(ai < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 130.013: wrong call crtrbl_\n"
                   "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                   " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                   i, CoeffArray[i], AxisArray[i]-1,
                   PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                   *LoopRefPtr);

             if(ai >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 130.014: wrong call crtrbl_\n"
                    "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                    " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                    i, CoeffArray[i], AxisArray[i]-1,
                    PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                    DArr->Space.Size[i], *LoopRefPtr);

             ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
                  ConstArray[i];

             if(ai < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 130.015: wrong call crtrbl_\n"
                   "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                   " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                   i, CoeffArray[i], AxisArray[i]-1,
                   PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                   *LoopRefPtr);

             if(ai >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 130.016: wrong call crtrbl_\n"
                    "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                    " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                    i, CoeffArray[i], AxisArray[i]-1,
                    PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                    DArr->Space.Size[i], *LoopRefPtr);
           }
           else
           { if(ConstArray[i] < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 130.017: wrong call crtrbl_\n"
                       "(ConstArray[%d]=%ld < 0)\n",
                       i, ConstArray[i]);
             if(ConstArray[i] >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 130.018: wrong call crtrbl_\n"
                       "(ConstArray[%d]=%ld >= %ld)\n",
                       i, ConstArray[i], DArr->Space.Size[i]);
           }
        }
        else
        {  ReplCount++;        /* number of replicated dimensions
                                  of remote array */    /*E0009*/
           BR++;               /* dimension of array-buffer */    /*E0010*/
        }
     }

     for(i=0; i < LR; i++)
         TstArray[i] = 0;

     for(i=0; i < AR; i++)
     {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
           continue;

        if(TstArray[AxisArray[i]-1])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 130.019: wrong call crtrbl_\n"
                   "(AxisArray[%d]=AxisArray[%d]=%ld)\n",
                   (int)(TstArray[AxisArray[i]-1]-1), i, AxisArray[i]);

        TstArray[AxisArray[i]-1] = (byte)(i+1);
     }

     if(BR)
     {  /* Dimension of array-buffer is not zero */    /*E0011*/

        AMR    = Map->AMViewRank;  /* dimension of AM representation */    /*E0012*/
        ALSize = AMR + BR;         /* size of converted mapping */    /*E0013*/

        dvm_AllocArray(s_ALIGN, ALSize, Align);

        /* Conversion of parallel loop mapping */    /*E0014*/

        for(i=0; i < BR; i++)
        {  Align[i].Attr  = align_COLLAPSE;
           Align[i].Axis  = (byte)(i+1);
           Align[i].TAxis = 0;
           Align[i].A     = 0;
           Align[i].B     = 0;
           Align[i].Bound = 0;
        }

        for(i=0; i < AMR; i++)
        {  k = i + BR;
           Align[k] = Map->Align[i+LR];

           if(Align[k].Attr == align_NORMTAXIS)
           {  Align[k].Attr  = align_REPLICATE;
              Align[k].Axis  = 0;
              Align[k].TAxis = (byte)(i+1);
              Align[k].A     = 0;
              Align[k].B     = 0;
              Align[k].Bound = 0;
           }
        }

        for(i=0,j=0; i < AR; i++)
        {  if(AxisArray[i] >= 0)
           {  if(CoeffArray[i] != 0)
              { /* Linear selection rule */    /*E0015*/

                if(Map->Align[AxisArray[i]-1].Attr == align_NORMAL)
                {  Align[j].Attr  = align_NORMAL;
                   Align[j].Axis  = (byte)(j+1);
                   Align[j].TAxis = Map->Align[AxisArray[i]-1].TAxis;
                   Align[j].A     = Map->Align[AxisArray[i]-1].A;
                   Align[j].B     = Map->Align[AxisArray[i]-1].B;
                   Align[j].Bound = 0;

                   k = Align[j].TAxis - 1 + BR;

                   Align[k].Attr  = align_NORMTAXIS;
                   Align[k].Axis  = (byte)(j+1);
                   Align[k].TAxis = Align[j].TAxis;
                   Align[k].A     = Align[j].A;
                   Align[k].B     = Align[j].B;
                   Align[k].Bound = 0;
                }

                j++;  /* current dimension number of array-buffer */    /*E0016*/
              }
           }
           else
              j++;    /* Callapse array-buffer dimension */    /*E0017*/
        }

        dvm_FreeArray(Map->Align);
        Map->Align = Align;       /* rules of mapping of array-buffer
                                     on AM loop representation */    /*E0018*/
        Map->ArrayRank = (byte)BR;

        /* Create array-buffer */    /*E0019*/

        ai = BR;
        TLen = DArr->TLen;

        for(i=0,j=0,k=BR+2; i < AR; i++)
        {  if(AxisArray[i] >= 0)
           {  if(CoeffArray[i] != 0)
              { /* Linear selection rule */    /*E0020*/

                SizeArray[j]    = PL->Set[AxisArray[i]-1].Size;
                BufferHeader[BR + 2 + (BR - 1 - j)] = PL->Set[AxisArray[i]-1].Lower;
                j++;  /* current number of array-buffer dimension */    /*E0021*/
                k++;
              }
           }
           else
           {  SizeArray[j]    = DArr->Space.Size[i];
              BufferHeader[BR + 2 + (BR - 1 - j)] = DArr->ExtHdrSign ? RemArrayHeader[AR + 2 + (AR - 1 - i)] : 0;
              j++;    /* current number of array-buffer dimension */    /*E0022*/
              k++;
           }
        }

        for(i=0; i < BR; i++)
            ShdWidthArray[i] = 0;

        ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &ai,
                           &TLen, SizeArray, StaticSignPtr, &StaticSign,
                           ShdWidthArray, ShdWidthArray) );
        BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
        BArr->RegBufSign = 1;

        /* Array-buffer mapping */    /*E0023*/

        Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
        ( RTL_CALL, delarm_(&MapRef) );

        /* Initialize staructure with info for buffer loading */    /*E0024*/

        dvm_AllocStruct(s_REGBUF, RegBuf);

        BArr->RegBuf = RegBuf;

        RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                    array */    /*E0025*/
        RegBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0026*/
        RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0027*/
        RegBuf->CopyFlag    = 0;  /* flag of asynchronous copying
                                     during the buffer loading */    /*E0028*/
        RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle of AM
                                           started the buffer loading */    /*E0029*/
        RegBuf->LoadEnvInd  = 0;    /* index of context started
                                       the buffer loading */    /*E0030*/
        RegBuf->RBG         = NULL; /* reference to buffer group,
                                       to which the buffer belongs */    /*E0031*/
        RegBuf->crtrbp_sign = 0;    /* flag of buffer creation
                                       by crtrbp_ function */    /*E0032*/

        for(i=0; i < AR; i++)
        {  if(AxisArray[i] >= 0)
           {  if(CoeffArray[i] != 0)
              { /* Linear selection rule */    /*E0033*/

                if(CoeffArray[i] > 0)
                {  RegBuf->InitIndex[i] =
                   CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
                   ConstArray[i];
                   RegBuf->LastIndex[i] =
                   CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
                   ConstArray[i];
                   RegBuf->Step[i] = CoeffArray[i];
                }
                else
                {  RegBuf->InitIndex[i] =
                   CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
                   ConstArray[i];
                   RegBuf->LastIndex[i] =
                   CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
                   ConstArray[i];
                   RegBuf->Step[i] = -CoeffArray[i];
                }
              }
              else
              {  /* Constant selection rule */    /*E0034*/

                 RegBuf->InitIndex[i] = ConstArray[i];
                 RegBuf->LastIndex[i] = ConstArray[i];
                 RegBuf->Step[i]      = 1;
              }
           }
           else
           {  /* Free dimension of remote array */    /*E0035*/

              RegBuf->InitIndex[i] = 0;
              RegBuf->LastIndex[i] = DArr->Space.Size[i] - 1;
              RegBuf->Step[i]      = 1;
           }
        }
     }
     else
     {  /* Dimension of array-buffer is zero */    /*E0036*/

        AMR    = Map->AMViewRank;  /* dimension of AM representation */    /*E0037*/
        ALSize = AMR + 1;          /* size of converted mapping */    /*E0038*/

        dvm_AllocArray(s_ALIGN, ALSize, Align);

        /* Conversion of parallel loop mapping */    /*E0039*/

        Align[0].Attr  = align_COLLAPSE;
        Align[0].Axis  = 1;
        Align[0].TAxis = 0;
        Align[0].A     = 0;
        Align[0].B     = 0;
        Align[0].Bound = 0;

        for(i=0; i < AMR; i++)
        {  k = i + 1;
           Align[k] = Map->Align[i+LR];

           if(Align[k].Attr == align_NORMTAXIS)
           {  Align[k].Attr  = align_REPLICATE;
              Align[k].Axis  = 0;
              Align[k].TAxis = (byte)(i+1);
              Align[k].A     = 0;
              Align[k].B     = 0;
              Align[k].Bound = 0;
           }
        }

        dvm_FreeArray(Map->Align);
        Map->Align = Align;       /* rules of array-buffer mapping
                                     on loop AM representation */    /*E0040*/
        Map->ArrayRank = 1;

        /* Create one dimensional array-buffer wtih only one element */    /*E0041*/

        ai = 1;
        TLen = DArr->TLen;
        SizeArray[0] = 1;
        BufferHeader[3] = 0;
        ShdWidthArray[0] = 0;

        ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &ai,
                           &TLen, SizeArray, StaticSignPtr, &StaticSign,
                           ShdWidthArray, ShdWidthArray) );
        BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
        BArr->RegBufSign = 1;

        /* Array-buffer mapping */    /*E0042*/

        Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
        ( RTL_CALL, delarm_(&MapRef) );

        /* Initialize structure with info for buffer loading */    /*E0043*/

        dvm_AllocStruct(s_REGBUF, RegBuf);

        BArr->RegBuf = RegBuf;

        RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                    array */    /*E0044*/
        RegBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0045*/
        RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0046*/
        RegBuf->CopyFlag    = 0;  /* flag os asynchronous copying
                                     during buffer loading */    /*E0047*/
        RegBuf->LoadAMHandlePtr = NULL; /* reference to AM Handle
                                           started buffer loading */    /*E0048*/
        RegBuf->LoadEnvInd  = 0;  /* index of context started
                                     buffer loading */    /*E0049*/ 
        RegBuf->RBG       = NULL; /* reference to buffer group
                                     to which the buffer belongs */    /*E0050*/
        RegBuf->crtrbp_sign = 0;  /* flag of buffer creation
                                     by the function crtrbp_ */    /*E0051*/

        for(i=0; i < AR; i++)
        {  /* ALl the seletion rules are constants */    /*E0052*/

           RegBuf->InitIndex[i] = ConstArray[i];
           RegBuf->LastIndex[i] = ConstArray[i];
           RegBuf->Step[i]      = 1;
        }
     }
  }
  else
  {  /* Mapping not created (empty loop) */    /*E0053*/

     BufferHeader[0] = 0;

     /* Registaration of remote access buffer header */    /*E0054*/

     if(DACount >= MaxDACount)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 130.029: wrong call crtrbl_\n"
            "(DistrArray Count = Max DistrArray Count(%d))\n",
            MaxDACount);

     DAHeaderAddr[DACount] = BufferHeader;
     DACount++;
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtrbl_,"BufferHandlePtr=%lx; IsLocal=%ld\n",
                           BufferHeader[0], Res);

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0055*/
  DVMFTimeFinish(ret_crtrbl_);
  return  (DVM_RET, Res);
}



DvmType  __callstd  crtrba_(DvmType  RemArrayHeader[], DvmType  BufferHeader[],
                            void *BasePtr, DvmType *StaticSignPtr,
                            DvmType  LocArrayHeader[], DvmType  AxisArray[],
                            DvmType  CoeffArray[], DvmType  ConstArray[])
{ SysHandle     *RemArrayHandlePtr, *LocArrayHandlePtr;
  s_DISARRAY    *RemDArr, *LocDArr, *BArr;
  s_AMVIEW      *RemAMV, *LocAMV;
  int            i, j, k, RemAR, LocAR, BR = 0, NormCount = 0,
                 ReplCount = 0, AMR, ALSize;
  ArrayMapRef    MapRef;
  s_ARRAYMAP    *Map;
  DvmType           StaticSign = 0, ai, TLen, ExtHdrSign = 1, Res = 0;
  byte           TstArray[MAXARRAYDIM];
  s_ALIGN       *Align;
  DvmType           SizeArray[MAXARRAYDIM], ShdWidthArray[MAXARRAYDIM];
  s_REGBUF      *RegBuf;

  DVMFTimeStart(call_crtrba_);

  if(RTL_TRACE)
     dvm_trace(call_crtrba_,
        "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; BufferHeader=%lx; "
        "BasePtr=%lx; StaticSign=%ld; "
        "LocArrayHeader=%lx; LocArrayHandlePtr=%lx;\n",
        (uLLng)RemArrayHeader, RemArrayHeader[0], (uLLng)BufferHeader,
        (uLLng)BasePtr, *StaticSignPtr,
        (uLLng)LocArrayHeader, LocArrayHeader[0]);

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.030: wrong call crtrba_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  RemArrayHandlePtr = TstDVMArray((void *)RemArrayHeader);

  if(RemArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 130.031: wrong call crtrba_\n"
        "(the object is not a remote distributed array;\n"
        "RemArrayHeader[0]=%lx)\n",
        RemArrayHeader[0]);

  RemDArr = (s_DISARRAY *)RemArrayHandlePtr->pP;
  RemAR   = RemDArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtrba_))
     {  for(i=0; i < RemAR; i++)
            tprintf(" AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < RemAR; i++)
            tprintf("CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < RemAR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  RemAMV  = RemDArr->AMView;

  if(RemAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.032: wrong call crtrba_\n"
              "(the remote array has not been aligned; "
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);
 
  /* Check if the processor system, to which the remote array
      is mapped, is a subsystem of current processor system   */    /*E0056*/

  NotSubsystem(i, DVM_VMS, RemAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 130.033: wrong call crtrba_\n"
       "(the remote array PS is not a subsystem of the current PS;\n"
       "RemArrayHeader[0]=%lx; RemArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       RemArrayHeader[0], (uLLng)RemAMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Control of specified local distribute array */    /*E0057*/

  LocArrayHandlePtr = TstDVMArray((void *)LocArrayHeader);

  if(LocArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 130.035: wrong call crtrba_\n"
        "(the object is not a local distributed array;\n"
        "LocArrayHeader[0]=%lx)\n",
        LocArrayHeader[0]);

  LocDArr = (s_DISARRAY *)LocArrayHandlePtr->pP;
  LocAR   = LocDArr->Space.Rank;

  LocAMV  = LocDArr->AMView;

  if(LocAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.036: wrong call crtrba_\n"
              "(the local array has not been aligned; "
              "LocArrayHeader[0]=%lx)\n", LocArrayHeader[0]);
 
  /*   Check if the processor system, to which the local array
        is mapped, is a subsystem of current processor system */    /*E0058*/

  NotSubsystem(i, DVM_VMS, LocAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 130.037: wrong call crtrba_\n"
       "(the local array PS is not a subsystem of the current PS;\n"
       "LocArrayHeader[0]=%lx; LocArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       LocArrayHeader[0], (uLLng)LocAMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Interrogation of local distributed array mapping */    /*E0059*/

  MapRef = ( RTL_CALL, arrmap_(LocArrayHeader, &StaticSign) );

  Map = (s_ARRAYMAP *)((SysHandle *)MapRef)->pP;

  /*     Contraol of specified parameters
     and calculation of array-buffer dimension */    /*E0060*/

  for(i=0; i < RemAR; i++)
  {  if(AxisArray[i] < -1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 130.040: wrong call crtrba_\n"
                 "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);

     if(AxisArray[i] > LocAR)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 130.041: wrong call crtrba_\n"
                 "(AxisArray[%d]=%ld > %d; LocArrayHeader[0]=%lx)\n",
                 i, AxisArray[i], LocAR, LocArrayHeader[0]);

     if(AxisArray[i] >= 0)
     {  if(ConstArray[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.042: wrong call crtrba_\n"
              "(ConstArray[%d]=%ld < 0)\n", i, ConstArray[i]);

        if(ConstArray[i] >= RemDArr->Space.Size[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 130.043: wrong call crtrba_\n"
             "(ConstArray[%d]=%ld >= %ld; RemArrayHeader[0]=%lx)\n",
             i, ConstArray[i], RemDArr->Space.Size[i],
             RemArrayHeader[0]);

        if(CoeffArray[i] != 0)
        { NormCount++;        /* number of linear selection rules */    /*E0061*/
          BR++;               /* array-buffer dimension */    /*E0062*/

          if(AxisArray[i] == 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 130.044: wrong call crtrba_\n"
                      "(AxisArray[%d]=0)\n", i);

          ai = CoeffArray[i] * (LocDArr->Space.Size[AxisArray[i]-1] - 1)
               + ConstArray[i];

          if(ai < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 130.045: wrong call crtrba_\n"
                "( (CoeffArray[%d]=%ld) * (LocArrayLastIndex[%ld]=%ld)"
                " + (ConstArray[%d]=%ld) < 0;\n"
                "LocArrayHeader[0]=%lx )\n",
                i, CoeffArray[i], AxisArray[i]-1,
                LocDArr->Space.Size[AxisArray[i]-1]-1, i, ConstArray[i],
                LocArrayHeader[0]);

          if(ai >= RemDArr->Space.Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 130.046: wrong call crtrba_\n"
                "( (CoeffArray[%d]=%ld) * (LocArrayLastIndex[%ld]=%ld)"
                " + (ConstArray[%d]=%ld) >= %ld;\n"
                "LocArrayHeader[0]=%lx )\n",
                i, CoeffArray[i], AxisArray[i]-1,
                LocDArr->Space.Size[AxisArray[i]-1]-1, i, ConstArray[i],
                RemDArr->Space.Size[i], LocArrayHeader[0]);
        }
     }
     else
     {  ReplCount++;        /* number of replicated dimensions
                               of remote array */    /*E0063*/
        BR++;               /* array-buffer dimension */    /*E0064*/
     }
  }

  for(i=0; i < LocAR; i++)
      TstArray[i] = 0;

  for(i=0; i < RemAR; i++)
  {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
        continue;

     if(TstArray[AxisArray[i]-1])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 130.047: wrong call crtrba_\n"
                "(AxisArray[%d]=AxisArray[%d]=%ld)\n",
                (int)(TstArray[AxisArray[i]-1]-1), i, AxisArray[i]);

     TstArray[AxisArray[i]-1] = (byte)(i+1);
  }

  if(BR)
  {  /* Array-buffer dimension is not zero */    /*E0065*/

     AMR    = Map->AMViewRank;  /* AM representation dimension */    /*E0066*/
     ALSize = AMR + BR;         /* mapping dimension */    /*E0067*/

     dvm_AllocArray(s_ALIGN, ALSize, Align);

     /*  Conversion of local distributed array mapping */    /*E0068*/

     for(i=0; i < BR; i++)
     {  Align[i].Attr  = align_COLLAPSE;
        Align[i].Axis  = (byte)(i+1);
        Align[i].TAxis = 0;
        Align[i].A     = 0;
        Align[i].B     = 0;
        Align[i].Bound = 0;
     }

     for(i=0; i < AMR; i++)
     {  k = i + BR;
        Align[k] = Map->Align[i+LocAR];

        if(Align[k].Attr == align_NORMTAXIS)
        {  Align[k].Attr  = align_REPLICATE;
           Align[k].Axis  = 0;
           Align[k].TAxis = (byte)(i+1);
           Align[k].A     = 0;
           Align[k].B     = 0;
           Align[k].Bound = 0;
        }
     }

     for(i=0,j=0; i < RemAR; i++)
     {  if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           { /* Linear selection rule */    /*E0069*/

             if(Map->Align[AxisArray[i]-1].Attr == align_NORMAL)
             {  Align[j].Attr  = align_NORMAL;
                Align[j].Axis  = (byte)(j+1);
                Align[j].TAxis = Map->Align[AxisArray[i]-1].TAxis;
                Align[j].A     = Map->Align[AxisArray[i]-1].A;
                Align[j].B     = Map->Align[AxisArray[i]-1].B;
                Align[j].Bound = 0;

                k = Align[j].TAxis - 1 + BR;

                Align[k].Attr  = align_NORMTAXIS;
                Align[k].Axis  = (byte)(j+1);
                Align[k].TAxis = Align[j].TAxis;
                Align[k].A     = Align[j].A;
                Align[k].B     = Align[j].B;
                Align[k].Bound = 0;
             }

             j++;  /* current number of array-buffer dimension */    /*E0070*/
           }
        }
        else
           j++;    /* Collapse array-buffer dimension */    /*E0071*/
     }

     dvm_FreeArray(Map->Align);
     Map->Align = Align;       /* rules of array-buffer mapping
                                  on AM representation for local
                                  distributed array */    /*E0072*/
     Map->ArrayRank = (byte)BR;

     /* Create array-buffer */    /*E0073*/

     ai = BR;
     TLen = RemDArr->TLen;

     for(i=0,j=0,k=BR+2; i < RemAR; i++)
     {  if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           { /* Linear selection rule */    /*E0074*/

             SizeArray[j]    = LocDArr->Space.Size[AxisArray[i]-1];
             BufferHeader[BR + 2 + (BR - 1 - j)] = LocDArr->ExtHdrSign ? LocArrayHeader[LocAR + 2 + (LocAR - 1 - (AxisArray[i] - 1))] : 0;
             j++;  /* current number of array-buffer dimension */    /*E0075*/
             k++;
           }
        }
        else
        {  SizeArray[j]    = RemDArr->Space.Size[i];
           BufferHeader[BR + 2 + (BR - 1 - j)] = RemDArr->ExtHdrSign ? RemArrayHeader[RemAR + 2 + (RemAR - 1 - i)] : 0;
           j++;    /* current number og array-buffer dimension */    /*E0076*/
           k++;
        }
     }

     for(i=0; i < BR; i++)
         ShdWidthArray[i] = 0;

     ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &ai,
                        &TLen, SizeArray, StaticSignPtr, &StaticSign,
                        ShdWidthArray, ShdWidthArray) );
     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     BArr->RegBufSign = 1;

     /* Array-buffer mapping */    /*E0077*/

     Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
     ( RTL_CALL, delarm_(&MapRef) );

     /* Initialization of structure with info for buffer loading */    /*E0078*/

     dvm_AllocStruct(s_REGBUF, RegBuf);

     BArr->RegBuf = RegBuf;

     RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                 array */    /*E0079*/
     RegBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0080*/
     RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0081*/
     RegBuf->CopyFlag    = 0;  /* flag of asynchronous copying 
                                  during buffer loading */    /*E0082*/
     RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                        started buffer loading */    /*E0083*/
     RegBuf->LoadEnvInd  = 0;  /* index of context started
                                  buffer loading */    /*E0084*/
     RegBuf->RBG         = NULL; /* reference to buffer group
                                    to which the buffer belongs */    /*E0085*/
     RegBuf->crtrbp_sign = 0;    /* flag buffer creation
                                    by function crtrbp_ */    /*E0086*/

     for(i=0; i < RemAR; i++)
     {  if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           { /* Linear selection rule */    /*E0087*/

             if(CoeffArray[i] > 0)
             { RegBuf->InitIndex[i] = ConstArray[i];
               RegBuf->LastIndex[i] =
               CoeffArray[i] * (LocDArr->Space.Size[AxisArray[i]-1] - 1)
               + ConstArray[i];
               RegBuf->Step[i] = CoeffArray[i];
             }
             else
             { RegBuf->InitIndex[i] =
               CoeffArray[i] * (LocDArr->Space.Size[AxisArray[i]-1] - 1)
               + ConstArray[i];
               RegBuf->LastIndex[i] = ConstArray[i];
               RegBuf->Step[i] = -CoeffArray[i];
             }
           }
           else
           {  /* Constant selection rule */    /*E0088*/

              RegBuf->InitIndex[i] = ConstArray[i];
              RegBuf->LastIndex[i] = ConstArray[i];
              RegBuf->Step[i]      = 1;
           }
        }
        else
        {  /* Free dimension of remote array */    /*E0089*/

           RegBuf->InitIndex[i] = 0;
           RegBuf->LastIndex[i] = RemDArr->Space.Size[i] - 1;
           RegBuf->Step[i]      = 1;
        }
     }
  }
  else
  {  /* Dimension of array-buffer */    /*E0090*/

     AMR    = Map->AMViewRank;  /* dimension of AM representation */    /*E0091*/
     ALSize = AMR + 1;          /* dimension of converted mapping */    /*E0092*/

     dvm_AllocArray(s_ALIGN, ALSize, Align);

     /* Conversion of local distributed array mapping */    /*E0093*/

     Align[0].Attr  = align_COLLAPSE;
     Align[0].Axis  = 1;
     Align[0].TAxis = 0;
     Align[0].A     = 0;
     Align[0].B     = 0;
     Align[0].Bound = 0;

     for(i=0; i < AMR; i++)
     {  k = i + 1;
        Align[k] = Map->Align[i+LocAR];

        if(Align[k].Attr == align_NORMTAXIS)
        {  Align[k].Attr  = align_REPLICATE;
           Align[k].Axis  = 0;
           Align[k].TAxis = (byte)(i+1);
           Align[k].A     = 0;
           Align[k].B     = 0;
           Align[k].Bound = 0;
        }
     }

     dvm_FreeArray(Map->Align);
     Map->Align = Align;       /* rules of array-buffer mapping
                                  on AM representation of local
                                  distributed array */    /*E0094*/
     Map->ArrayRank = 1;

     /* Create one dimensional array-buffer with only one element */    /*E0095*/

     ai = 1;
     TLen = RemDArr->TLen;
     SizeArray[0] = 1;
     BufferHeader[3] = 0;
     ShdWidthArray[0] = 0;

     ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &ai,
                        &TLen, SizeArray, StaticSignPtr, &StaticSign,
                        ShdWidthArray, ShdWidthArray) );
     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     BArr->RegBufSign = 1;

     /* Conversion of array-buffer */    /*E0096*/

     Res = ( RTL_CALL, malign_(BufferHeader, NULL, &MapRef) );
     ( RTL_CALL, delarm_(&MapRef) );

     /* Initialization of structure with info for buffer loading */    /*E0097*/

     dvm_AllocStruct(s_REGBUF, RegBuf);

     BArr->RegBuf = RegBuf;

     RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                 array */    /*E0098*/
     RegBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0099*/
     RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0100*/
     RegBuf->CopyFlag    = 0;  /* flag of asynchronous copying
                                  during buffer loading */    /*E0101*/
     RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                        started buffer loading */    /*E0102*/
     RegBuf->LoadEnvInd  = 0;  /* index of context started
                                  buffer loading */    /*E0103*/
     RegBuf->RBG       = NULL; /* reference to buffer group
                                  to which the buffer belongs */    /*E0104*/
     RegBuf->crtrbp_sign = 0;  /* flag of buffer creation
                                  by function crtrbp_ */    /*E0105*/

     for(i=0; i < RemAR; i++)
     {  /* All selection rules are constant */    /*E0106*/

        RegBuf->InitIndex[i] = ConstArray[i];
        RegBuf->LastIndex[i] = ConstArray[i];
        RegBuf->Step[i]      = 1;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtrba_,"BufferHandlePtr=%lx; IsLocal=%ld\n",
                           BufferHeader[0], Res);

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0107*/
  DVMFTimeFinish(ret_crtrba_);
  return  (DVM_RET, Res);
}



DvmType  __callstd  crtrbp_(DvmType  RemArrayHeader[], DvmType  BufferHeader[],
                            void *BasePtr, DvmType *StaticSignPtr,
                            PSRef  *PSRefPtr, DvmType  CoordArray[])
{ SysHandle     *RemArrayHandlePtr, *VMSHandlePtr;
  s_VMS         *VMS;
  PSRef          VMRef;
  s_DISARRAY    *RemDArr, *BArr;
  s_AMVIEW      *RemAMV;
  int            i, j, k, RemAR;
  DvmType           StaticSign = 0, BR = 0, TLen, ExtHdrSign = 1, VMAR,
                 Res = 0;
  DvmType           TmpAMVSize[1] = { 1 }, AlignAxisArray[1] = { -1 };
  DvmType           SizeArray[MAXARRAYDIM], ShdWidthArray[MAXARRAYDIM];
  s_REGBUF      *RegBuf;
  AMViewRef      TmpAMVRef; 

  DVMFTimeStart(call_crtrbp_);

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL)
        dvm_trace(call_crtrbp_,
            "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; "
            "BufferHeader=%lx; "
            "BasePtr=%lx; StaticSign=%ld; PSRefPtr=NULL; PSRef=0;\n",
            (uLLng)RemArrayHeader, RemArrayHeader[0],
            (uLLng)BufferHeader, (uLLng)BasePtr, *StaticSignPtr);
     else
        dvm_trace(call_crtrbp_,
            "RemArrayHeader=%lx; RemArrayHandlePtr=%lx; "
            "BufferHeader=%lx; "
            "BasePtr=%lx; StaticSign=%ld; PSRefPtr=%lx; PSRef=%lx;\n",
            (uLLng)RemArrayHeader, RemArrayHeader[0],
            (uLLng)BufferHeader, (uLLng)BasePtr,
            *StaticSignPtr, (uLLng)PSRefPtr, *PSRefPtr);
  }

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.060: wrong call crtrbp_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  RemArrayHandlePtr = TstDVMArray((void *)RemArrayHeader);

  if(RemArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 130.061: wrong call crtrbp_\n"
        "(the object is not a remote distributed array;\n"
        "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);

  RemDArr = (s_DISARRAY *)RemArrayHandlePtr->pP;
  RemAR   = RemDArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_crtrbp_))
     {  for(i=0; i < RemAR; i++)
            tprintf("CoordArray[%d]=%ld; ", i, CoordArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  RemAMV  = RemDArr->AMView;

  if(RemAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 130.062: wrong call crtrbp_\n"
              "(the remote array has not been aligned; "
              "RemArrayHeader[0]=%lx)\n", RemArrayHeader[0]);
 
  /* Check if the processor system, on which the remote array
       is mapped, is a subsystem of current processor system  */    /*E0108*/

  NotSubsystem(i, DVM_VMS, RemAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 130.063: wrong call crtrbp_\n"
       "(the remote array PS is not a subsystem of the current PS;\n"
       "RemArrayHeader[0]=%lx; RemArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       RemArrayHeader[0], (uLLng)RemAMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Control specified processor system */    /*E0109*/

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = DVM_VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(!TstDVMObj(PSRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 130.065: wrong call crtrbp_\n"
             "(the processor system is not a DVM object; "
             "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 130.066: wrong call crtrbp_\n"
               "(the object is not a processor system; PSRef=%lx)\n",
               *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;

     /* Check if all processors of specified processor
            system are in current processor system     */    /*E0110*/

     NotSubsystem(i, DVM_VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 130.067: wrong call crtrbp_\n"
             "(the given PS is not a subsystem of the current PS;\n"
             "PSRef=%lx; CurrentPSRef=%lx)\n",
             *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);
  }

  VMAR = VMS->Space.Rank;

  /*    Control of specified parameters
     and array-buffer dimension calculation */    /*E0111*/

  for(i=0; i < RemAR; i++)
  {  if(CoordArray[i] >= RemDArr->Space.Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 130.070: wrong call crtrbp_\n"
             "(CoordArray[%d]=%ld >= %ld; RemArrayHeader[0]=%lx)\n",
             i, CoordArray[i], RemDArr->Space.Size[i],
             RemArrayHeader[0]);
      if(CoordArray[i] < 0)
         BR++;               /* array-buffer dimension */    /*E0112*/
  }

  if(BR)
  {  /* Array-buffer dimension is not zero */    /*E0113*/

     /* Create array-buffer */    /*E0114*/

     TLen = RemDArr->TLen;

     for(i=0,j=0,k=BR+2; i < RemAR; i++)
     {  if(CoordArray[i] < 0)
        {  SizeArray[j]    = RemDArr->Space.Size[i];
           BufferHeader[BR + 2 + (BR - 1 - j)] = RemDArr->ExtHdrSign ? RemArrayHeader[RemAR + 2 + (RemAR - 1 - i)] : 0;
           j++;    /* current number of array-buffer dimension */    /*E0115*/
           k++;
        }
     }

     for(i=0; i < MAXARRAYDIM; i++)
         ShdWidthArray[i] = 0;

     ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr, &BR,
                        &TLen, SizeArray, StaticSignPtr, &StaticSign,
                        ShdWidthArray, ShdWidthArray) );
     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     BArr->RegBufSign = 1;

     /* Create one dimensional representation of current AM */    /*E0116*/

     TmpAMVRef = ( RTL_CALL, crtamv_(NULL, &ExtHdrSign, TmpAMVSize, StaticSignPtr) );

     /* Mapping of created one dimensional representation on specified
            processor system (all mapping rules are replications)      */    /*E0117*/

     VMRef = (PSRef)VMSHandlePtr;

     ( RTL_CALL, distr_(&TmpAMVRef, &VMRef, &VMAR,
                        ShdWidthArray, ShdWidthArray) );

     /*  Mapping pf array-buffer on created one dimensional
        representation. The only mapping rule is replication */    /*E0118*/

     Res = ( RTL_CALL, align_(BufferHeader, &TmpAMVRef, AlignAxisArray,
                              ShdWidthArray, ShdWidthArray) );

     /* Initialize structure with info for buffer loading */    /*E0119*/

     dvm_AllocStruct(s_REGBUF, RegBuf);

     BArr->RegBuf = RegBuf;

     RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote 
                                                 array */    /*E0120*/
     RegBuf->IsLoad      = 0;  /* flag of loaded buffer */    /*E0121*/
     RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0122*/
     RegBuf->CopyFlag    = 0;  /* flag asynchronous copying
                                  during buffer loading */    /*E0123*/
     RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                        started buffer loading */    /*E0124*/
     RegBuf->LoadEnvInd  = 0;  /* index of context started
                                  buffer loading */    /*E0125*/
     RegBuf->RBG       = NULL; /* reference to buffer group
                                  to which buffer belongs */    /*E0126*/
     RegBuf->crtrbp_sign = 1;  /* flag of buffer creation
                                  by function crtrbp_ */    /*E0127*/

     for(i=0; i < RemAR; i++)
     {  if(CoordArray[i] < 0)
        {  /* Free dimension of remote array */    /*E0128*/

           RegBuf->InitIndex[i] = 0;
           RegBuf->LastIndex[i] = RemDArr->Space.Size[i] - 1;
           RegBuf->Step[i]      = 1;
        }
        else
        {  /* Constatnt selection rule */    /*E0129*/

           RegBuf->InitIndex[i] = CoordArray[i];
           RegBuf->LastIndex[i] = CoordArray[i];
           RegBuf->Step[i]      = 1;
        }
     }
  }
  else
  {  /* Array-buffer dimension is zero */    /*E0130*/

     /* Create one dimensional array-buffer with only one element */    /*E0131*/

     TLen = RemDArr->TLen;
     BufferHeader[3] = 0;

     for(i=0; i < MAXARRAYDIM; i++)
         ShdWidthArray[i] = 0;

     ( RTL_CALL, crtda_(BufferHeader, &ExtHdrSign, BasePtr,
                        &ExtHdrSign, &TLen, TmpAMVSize, StaticSignPtr,
                        &StaticSign, ShdWidthArray, ShdWidthArray) );
     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     BArr->RegBufSign = 1;

     /* Create one dimensional representation of current AM */    /*E0132*/

     TmpAMVRef = ( RTL_CALL, crtamv_(NULL, &ExtHdrSign, TmpAMVSize, StaticSignPtr) );

     /* Mapping of created one dimensional representation on specified
            processor system (all mapping rules are replications)      */    /*E0133*/

     VMRef = (PSRef)VMSHandlePtr;

     ( RTL_CALL, distr_(&TmpAMVRef, &VMRef, &VMAR,
                        ShdWidthArray, ShdWidthArray) );

     /*   Array-buffer mapping on created one dimensional
        repesentation. The only mapping rule is replication. */    /*E0134*/

     Res = ( RTL_CALL, align_(BufferHeader, &TmpAMVRef, AlignAxisArray,
                              ShdWidthArray, ShdWidthArray) );

     /* Initialize structure with info for buffer loading */    /*E0135*/

     dvm_AllocStruct(s_REGBUF, RegBuf);

     BArr->RegBuf = RegBuf;

     RegBuf->DAHandlePtr = RemArrayHandlePtr; /* reference to remote
                                                 array */    /*E0136*/
     RegBuf->IsLoad      = 0;  /* flag of laoded buffer */    /*E0137*/
     RegBuf->LoadSign    = 0;  /* flag of current buffer loading */    /*E0138*/
     RegBuf->CopyFlag    = 0;  /* flag of asynchronous copying
                                  during buffer loading */    /*E0139*/
     RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                        started buffer loading */    /*E0140*/
     RegBuf->LoadEnvInd  = 0;  /* index of context started
                                  buffer loading */    /*E0141*/
     RegBuf->RBG       = NULL; /* reference to buffer group
                                  to which the buffer belongs */    /*E0142*/
     RegBuf->crtrbp_sign = 1;  /* flag of buffer creation
                                  by function crtrbp_ */    /*E0143*/

     for(i=0; i < RemAR; i++)
     {  /* All selection rules are constants */    /*E0144*/

        RegBuf->InitIndex[i] = CoordArray[i];
        RegBuf->LastIndex[i] = CoordArray[i];
        RegBuf->Step[i]      = 1;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtrbp_,"BufferHandlePtr=%lx; IsLocal=%ld\n",
                           BufferHeader[0], Res);

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0145*/
  DVMFTimeFinish(ret_crtrbp_);
  return  (DVM_RET, Res);
}


/* ---------------------------------------------------------- */    /*E0146*/


DvmType  __callstd loadrb_(DvmType   BufferHeader[], DvmType  *RenewSignPtr)
{ int           i, BRank;
  s_DISARRAY   *BArr, *DArr;
  s_REGBUF     *RegBuf;
  DvmType          ToInitIndex[MAXARRAYDIM], ToLastIndex[MAXARRAYDIM],
                ToStep[MAXARRAYDIM];
  DvmType          CopyRegim = 0;
  s_AMVIEW     *AMV;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0147*/
  DVMFTimeStart(call_loadrb_);

  if(RTL_TRACE)
     dvm_trace(call_loadrb_,
         "BufferHeader=%lx; BufferHandlePtr=%lx; RenewSign=%ld;\n",
         (uLLng)BufferHeader, BufferHeader[0], *RenewSignPtr);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 131.000: wrong call loadrb_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0148*/

     if(TstDVMArray(BufferHeader) == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.000: wrong call loadrb_\n"
                 "(the object is not a distributed array;\n"
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     RegBuf = BArr->RegBuf;

     if(RegBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.001: wrong call loadrb_\n"
                 "(the distributed array is not a buffer; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     if(RegBuf->RBG && RegBuf->RBG->LoadSign == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.002: wrong call loadrb_\n"
                 "(the buffer is a member of the regular access group;\n"
                 "BufferHeader[0]=%lx; ReguarAccessGroupRef=%lx)\n",
                 BufferHeader[0], (uLLng)RegBuf->RBG->HandlePtr);

     if(RegBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.003: wrong call loadrb_\n"
                 "(the buffer loading has already been started; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     AMV  = BArr->AMView;
 
     /* Check if processor system, on which array-buffer
        is mapped, subsystem of current processor system */    /*E0149*/

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 131.004: wrong call loadrb_\n"
           "(the buffer PS is not a subsystem of the current PS;\n"
           "BufferHeader[0]=%lx; BufferPSRef=%lx; CurrentPSRef=%lx)\n",
           BufferHeader[0], (uLLng)AMV->VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     DArr = (s_DISARRAY *)RegBuf->DAHandlePtr->pP;
     AMV  = DArr->AMView;
 
     /* Check if processor system, on which remote array
        is mapped, subsystem of current processor system */    /*E0150*/

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 131.005: wrong call loadrb_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)RegBuf->DAHandlePtr, (uLLng)AMV->VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     RegBuf->LoadSign = 1;                      /* flag: buffer
                                                   is loading */    /*E0151*/
     RegBuf->LoadAMHandlePtr = CurrAMHandlePtr; /* reference to Handle AM
                                                   started loading */    /*E0152*/
     RegBuf->LoadEnvInd  = gEnvColl->Count - 1; /* index of context
                                                   started buffer
                                                   loading */    /*E0153*/ 

     if(RegBuf->IsLoad == 0 || *RenewSignPtr)
     {  /* Start asynchronous coping of distributed arrays */    /*E0154*/

        BRank = BArr->Space.Rank;

        for(i=0; i < BRank; i++)
        {  ToInitIndex[i] = 0;
           ToLastIndex[i] = BArr->Space.Size[i] - 1;
           ToStep[i]      = 1;
        }

        ( RTL_CALL, aarrcp_((DvmType *)RegBuf->DAHandlePtr->HeaderPtr,
                            RegBuf->InitIndex, RegBuf->LastIndex,
                            RegBuf->Step, BufferHeader, ToInitIndex,
                            ToLastIndex, ToStep, &CopyRegim,
                            &RegBuf->CopyFlag) );

        if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
        {  rtl_TstReqColl(0);
           rtl_SendReqColl(ResCoeffLoadRB);
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_loadrb_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0155*/
  DVMFTimeFinish(ret_loadrb_);
  return  (DVM_RET, 0);
}



DvmType  __callstd waitrb_(DvmType   BufferHeader[])
{ int           i;
  s_DISARRAY   *BArr;
  s_REGBUF     *RegBuf;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0156*/
  DVMFTimeStart(call_waitrb_);

  if(RTL_TRACE)
     dvm_trace(call_waitrb_,"BufferHeader=%lx; BufferHandlePtr=%lx;\n",
                            (uLLng)BufferHeader, BufferHeader[0]);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 131.020: wrong call waitrb_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0157*/

     if(TstDVMArray(BufferHeader) == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.020: wrong call waitrb_\n"
                 "(the object is not a distributed array;\n"
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)((SysHandle *)BufferHeader[0])->pP;
     RegBuf = BArr->RegBuf;

     if(RegBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.022: wrong call waitrb_\n"
                 "(the distributed array is not a buffer; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     i = gEnvColl->Count - 1;       /* current context index */    /*E0158*/

     if(RegBuf->LoadAMHandlePtr != CurrAMHandlePtr)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 131.023: wrong call waitrb_\n"
            "(the buffer loading was not started by the "
            "current subtask;\nBufferHeader[0]=%lx; LoadEnvIndex=%d; "
            "CurrentEnvIndex=%d)\n",
            BufferHeader[0], RegBuf->LoadEnvInd, i);

     if(RegBuf->LoadSign == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.024: wrong call waitrb_\n"
                 "(the buffer loading has not been started; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     /* Waiting completion of asynchronous copying */    /*E0159*/

     ( RTL_CALL, waitcp_(&RegBuf->CopyFlag) );

     RegBuf->LoadSign = 0;           /* turn off current
                                        buffer loading */    /*E0160*/
     RegBuf->IsLoad   = 1;           /* flag: buffer loaded */    /*E0161*/
     RegBuf->LoadAMHandlePtr = NULL; /* reference to Handle AM
                                        started buffer loading */    /*E0162*/
     RegBuf->LoadEnvInd = 0;         /* index of context started
                                        buffer loading */    /*E0163*/
  }

  if(RTL_TRACE)
     dvm_trace(ret_waitrb_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0164*/
  DVMFTimeFinish(ret_waitrb_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delrb_(DvmType  BufferHeader[])
{ int             i;
  SysHandle      *BHandlePtr;
  s_DISARRAY     *BArr;
  void           *CurrAM;
  s_REGBUFGROUP  *RBG;

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* fot statistics */    /*E0165*/
  DVMFTimeStart(call_delrb_);

  if(RTL_TRACE)
     dvm_trace(call_delrb_,"BufferHeader=%lx; BufferHandlePtr=%lx;\n",
                           (uLLng)BufferHeader, BufferHeader[0]);

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 131.040: wrong call delrb_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0166*/

     BHandlePtr = TstDVMArray(BufferHeader);

     if(BHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.040: wrong call delrb_\n"
                 "(the object is not a distributed array;\n"
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)BHandlePtr->pP;

     if(BArr->RegBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.042: wrong call delrb_\n"
                 "(the distributed array is not a buffer; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);
  
     /* Check if buffer is created in current subtask */    /*E0167*/
      
     i      = gEnvColl->Count - 1;     /* current context index */    /*E0168*/
     CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0169*/

     if(BHandlePtr->CrtAMHandlePtr != CurrAM)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.043: wrong call delrb_\n"
                 "(the buffer was not created by the current subtask;\n"
                 "BufferHeader[0]=%lx; BufferEnvIndex=%d; "
                 "CurrentEnvIndex=%d)\n",
                 BufferHeader[0], BHandlePtr->CrtEnvInd, i);

     if(BArr->RegBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 131.044: wrong call delrb_\n"
                 "(the buffer loading has not been completed; "
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     RBG = BArr->RegBuf->RBG;

     if(RBG)
     {  if(RBG->LoadSign)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 131.045: wrong call delrb_\n"
                    "(the group loading has not been completed;\n"
                    "BufferHeader[0]=%lx; RegularAccessGroupRef=%lx)\n",
                    BufferHeader[0], (uLLng)RBG->HandlePtr);
     }

     ( RTL_CALL, delda_(BufferHeader) );
  }
  else
  {  /* Cancel empty buffer */    /*E0170*/

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
     dvm_trace(ret_delrb_," \n");

  StatObjectRef = (ObjectRef)BufferHeader[0]; /* for statistics */    /*E0171*/
  DVMFTimeFinish(ret_delrb_);
  return  (DVM_RET, 0);
}


/* ---------------------------------------------------- */    /*E0172*/

            
RegularAccessGroupRef __callstd crtbg_(DvmType  *StaticSignPtr,
                                       DvmType  *DelBufSignPtr)
{ SysHandle               *RBGHandlePtr;
  s_REGBUFGROUP           *RBG;
  RegularAccessGroupRef    Res;

  DVMFTimeStart(call_crtbg_);

  if(RTL_TRACE)
     dvm_trace(call_crtbg_,"StaticSign=%ld; DelBufSign=%ld;\n",
                           *StaticSignPtr, *DelBufSignPtr);

  dvm_AllocStruct(s_REGBUFGROUP, RBG);
  dvm_AllocStruct(SysHandle, RBGHandlePtr);

  RBG->Static          = (byte)*StaticSignPtr;
  RBG->RB              = coll_Init(RegBufGrpCount, RegBufGrpCount, NULL);
  RBG->DelBuf          = (byte)*DelBufSignPtr; /* flag of buffer cancelling
                                                  together with group */    /*E0173*/ 
  RBG->IsLoad          = 0;    /* group is not loaded */    /*E0174*/
  RBG->LoadSign        = 0;    /* group loading not started */    /*E0175*/
  RBG->LoadAMHandlePtr = NULL; /* reference to Handle AM started
                                  group loading */    /*E0176*/
  RBG->LoadEnvInd      = 0;    /* index of context in which
                                  group loading started */    /*E0177*/ 
  RBG->ResetSign       = 0;    /* */    /*E0178*/

  *RBGHandlePtr  = genv_InsertObject(sht_RegBufGroup, RBG);
  RBG->HandlePtr = RBGHandlePtr; /* pointer to own Handle */    /*E0179*/

  if(TstObject)
     InsDVMObj((ObjectRef)RBGHandlePtr);

  Res = (RegularAccessGroupRef)RBGHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_crtbg_,"RegularAccessGroupRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0180*/
  DVMFTimeFinish(ret_crtbg_);
  return  (DVM_RET, Res);
}



DvmType __callstd insrb_(RegularAccessGroupRef  *RegularAccessGroupRefPtr,
                         DvmType  BufferHeader[])
{ SysHandle       *RBGHandlePtr, *BHandlePtr;
  s_REGBUFGROUP   *RBG;
  s_REGBUF        *RegBuf;
  int              i;
  void            *CurrAM;
  s_DISARRAY      *BArr;

  StatObjectRef = (ObjectRef)BufferHeader[0];  /* for statistics */    /*E0181*/
  DVMFTimeStart(call_insrb_);

  if(RTL_TRACE)
     dvm_trace(call_insrb_,
         "RegularAccessGroupRefPtr=%lx; RegularAccessGroupRef=%lx; "
         "BufferHeader=%lx; BufferHeader[0]=%lx\n",
         (uLLng)RegularAccessGroupRefPtr, *RegularAccessGroupRefPtr,
         (uLLng)BufferHeader, BufferHeader[0]);

  RBGHandlePtr = (SysHandle *)*RegularAccessGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RegularAccessGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.010: wrong call insrb_\n"
            "(the regular access group is not a DVM object; "
            "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);
  }

  if(RBGHandlePtr->Type != sht_RegBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 132.011: wrong call insrb_\n"
          "(the object is not a regular access group;\n"
          "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  RBG = (s_REGBUFGROUP *)RBGHandlePtr->pP;
  
  /* Check if buffer group is created in current subtask */    /*E0182*/
      
  i      = gEnvColl->Count - 1;     /* current context index */    /*E0183*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0184*/

  if(RBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 132.012: wrong call insrb_\n"
        "(the regular access group was not created by the "
        "current subtask;\n"
        "RegularAccessGroupRef=%lx; RegularAccessGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RegularAccessGroupRefPtr, RBGHandlePtr->CrtEnvInd, i);

  /* Check if group loading is finished */    /*E0185*/

  if(RBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.013: wrong call insrb_\n"
           "(the group loading has not been completed; "
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  /* Control of buffer included into group */    /*E0186*/

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == BufferHeader)
         break;

  if(i == DACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 132.014: wrong call insrb_\n"
              "(the object is not a distributed array;\n"
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  if(BufferHeader[0])
  {  /* Non empty buffer */    /*E0187*/

     BHandlePtr = TstDVMArray(BufferHeader);

     if(BHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 132.014: wrong call insrb_\n"
                 "(the object is not a distributed array;\n"
                 "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     BArr = (s_DISARRAY *)BHandlePtr->pP;
     RegBuf = BArr->RegBuf;

     if(RegBuf == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.016: wrong call insrb_\n"
           "(the distributed array is not a buffer; "
           "BufferHeader[0]=%lx)\n", BufferHeader[0]);
  
     /* Check if buffer is created in current subtask */    /*E0188*/
      
     i      = gEnvColl->Count - 1;     /* current context index */    /*E0189*/
     CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0190*/

     if(BHandlePtr->CrtAMHandlePtr != CurrAM)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.017: wrong call insrb_\n"
           "(the buffer was not created by the current subtask;\n"
           "BufferHeader[0]=%lx; BufferEnvIndex=%d; "
           "CurrentEnvIndex=%d)\n",
           BufferHeader[0], BHandlePtr->CrtEnvInd, i);

     /* Check if buffer is included into some other group */    /*E0191*/

     if(RegBuf->RBG && RegBuf->RBG != RBG)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.018: wrong call insrb_\n"
            "(the buffer has already been inserted "
            "in the regular access group;\n"
            "BufferHeader[0]=%lx; RegularAccessGroupRef=%lx)\n",
            BufferHeader[0], (uLLng)RegBuf->RBG->HandlePtr);

     /* Check if buffer loading is finished */    /*E0192*/

     if(RegBuf->LoadSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.019: wrong call insrb_\n"
            "(the buffer loading has not been completed; "
            "BufferHeader[0]=%lx)\n", BufferHeader[0]);

     if(RegBuf->RBG == NULL)
     {  /* Buffer is not included into group */    /*E0193*/

        coll_Insert(&RBG->RB, BArr); /* add into list of buffers
                                        included into group */    /*E0194*/
           
        RegBuf->RBG = RBG;           /* fix buffer group for buffer */    /*E0195*/
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_insrb_," \n");

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for
                                                           statistics */    /*E0196*/
  DVMFTimeFinish(ret_insrb_);
  return  (DVM_RET, 0);
}



DvmType __callstd loadbg_(RegularAccessGroupRef  *RegularAccessGroupRefPtr, DvmType  *RenewSignPtr)
{ int             i, NotSubSys;
  SysHandle      *RBGHandlePtr;
  s_REGBUFGROUP  *RBG;
  s_DISARRAY     *BArr, *DArr;
  s_REGBUF       *RegBuf;
  s_VMS          *VMS;
  DvmType           *BufferHeader;
  DvmType            RenewSign = 1;

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for 
                                                           statistics */    /*E0197*/
  DVMFTimeStart(call_loadbg_);

  if(RTL_TRACE)
     dvm_trace(call_loadbg_,
         "RegularAccessGroupRefPtr=%lx; RegularAccessGroupRef=%lx; "
         "RenewSign=%d\n",
         (uLLng)RegularAccessGroupRefPtr, *RegularAccessGroupRefPtr,
         *RenewSignPtr);

  RBGHandlePtr = (SysHandle *)*RegularAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(RegularAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.030: wrong call loadbg_\n"
            "(the regular access group is not a DVM object; "
            "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);
  }

  if(RBGHandlePtr->Type != sht_RegBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.031: wrong call loadbg_\n"
           "(the object is not a regular access group;\n"
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  RBG = (s_REGBUFGROUP *)RBGHandlePtr->pP;

  /* Check if all buffer group and its remote arrays
     mapped on subsystems of current processor system */    /*E0198*/
  
  for(i=0; i < RBG->RB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &RBG->RB, i);

     VMS = BArr->AMView->VMS;          /* processor system
                                          on which buffer mapped */    /*E0199*/

     NotSubsystem(NotSubSys, DVM_VMS, VMS)

     if(NotSubSys)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.032: wrong call loadbg_\n"
           "(the buffer PS is not a subsystem of the current PS;\n"
           "BufferHeader[0]=%lx; BufferPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)BArr->HandlePtr, (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     RegBuf = BArr->RegBuf;
     DArr = (s_DISARRAY *)RegBuf->DAHandlePtr->pP;
     VMS  = DArr->AMView->VMS;         /* processor system
                                          on which array mapped */    /*E0200*/
 
     NotSubsystem(NotSubSys, DVM_VMS, VMS)

     if(NotSubSys)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.033: wrong call loadbg_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)RegBuf->DAHandlePtr, (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);
  }

  /* Check if group loading is started */    /*E0201*/

  if(RBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.034: wrong call loadbg_\n"
           "(the group loading has already been started; "
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  /* Start group loading */    /*E0202*/

  RBG->LoadSign = 1;                      /* flag: group is
                                             loading */    /*E0203*/
  RBG->LoadAMHandlePtr = CurrAMHandlePtr; /* reference to Handle AM
                                             started loading */    /*E0204*/
  RBG->LoadEnvInd  = gEnvColl->Count - 1; /* index of context
                                             started group loading */    /*E0205*/ 

  if(RBG->IsLoad == 0 || *RenewSignPtr)
  {  /* Start group buffer loading */    /*E0206*/

     for(i=0; i < RBG->RB.Count; i++)
     {  BArr = coll_At(s_DISARRAY *, &RBG->RB, i);
        BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;
        ( RTL_CALL, loadrb_(BufferHeader, &RenewSign) );
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffLoadBG);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_loadbg_,"\n");

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for
                                                           statistics */    /*E0207*/
  DVMFTimeFinish(ret_loadbg_);
  return  (DVM_RET, 0);
}



DvmType __callstd waitbg_(RegularAccessGroupRef  *RegularAccessGroupRefPtr)
{ SysHandle       *RBGHandlePtr;
  s_REGBUFGROUP   *RBG;
  int              i;
  s_DISARRAY      *BArr;
  DvmType            *BufferHeader;

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for
                                                           statistics */    /*E0208*/
  DVMFTimeStart(call_waitbg_);

  if(RTL_TRACE)
     dvm_trace(call_waitbg_,
         "RegularAccessGroupRefPtr=%lx; RegularAccessGroupRef=%lx;\n",
         (uLLng)RegularAccessGroupRefPtr, *RegularAccessGroupRefPtr);

  RBGHandlePtr = (SysHandle *)*RegularAccessGroupRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(RegularAccessGroupRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.040: wrong call waitbg_\n"
            "(the regular access group is not a DVM object; "
            "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);
  }

  if(RBGHandlePtr->Type != sht_RegBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.041: wrong call waitbg_\n"
           "(the object is not a regular access group;\n"
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  RBG = (s_REGBUFGROUP *)RBGHandlePtr->pP;

  i = gEnvColl->Count - 1;       /* current context index */    /*E0209*/

  if(RBG->LoadAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 132.042: wrong call waitbg_\n"
         "(the group loading was not started by the "
         "current subtask;\nRegularAccessGroupRef=%lx; "
         "LoadEnvIndex=%d; CurrentEnvIndex=%d)\n",
         *RegularAccessGroupRefPtr, RBG->LoadEnvInd, i);

  if(RBG->LoadSign == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 132.043: wrong call waitbg_\n"
        "(the group loading has not been started; "
        "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  /* Waiting completion of all group buffer loading */    /*E0210*/

  for(i=0; i < RBG->RB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &RBG->RB, i);
     BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;
     ( RTL_CALL, waitrb_(BufferHeader) );
  }

  RBG->LoadSign = 0;           /* turn off flag of current
                                  group buffer loading */    /*E0211*/
  RBG->IsLoad   = 1;           /* flag: group loaded */    /*E0212*/
  RBG->LoadAMHandlePtr = NULL; /* reference to Handle AM started
                                  buffer group loading */    /*E0213*/
  RBG->LoadEnvInd = 0;         /* index of context started 
                                  buffer group loading */    /*E0214*/

  if(RTL_TRACE)
     dvm_trace(ret_waitbg_,"\n");

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for 
                                                           statistics */    /*E0215*/
  DVMFTimeFinish(ret_waitbg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delbg_(RegularAccessGroupRef  *RegularAccessGroupRefPtr)
{ SysHandle       *RBGHandlePtr;
  s_REGBUFGROUP   *RBG;
  int              i;
  void            *CurrAM;

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for 
                                                           statistics */    /*E0216*/
  DVMFTimeStart(call_delbg_);

  if(RTL_TRACE)
     dvm_trace(call_delbg_,
         "RegularAccessGroupRefPtr=%lx; RegularAccessGroupRef=%lx;\n",
         (uLLng)RegularAccessGroupRefPtr, *RegularAccessGroupRefPtr);

  RBGHandlePtr = (SysHandle *)*RegularAccessGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RegularAccessGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.050: wrong call delbg_\n"
            "(the regular access group is not a DVM object; "
            "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);
  }

  if(RBGHandlePtr->Type != sht_RegBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.051: wrong call delbg_\n"
           "(the object is not a regular access group;\n"
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  RBG = (s_REGBUFGROUP *)RBGHandlePtr->pP;
  
  /* Check if buffer group is created in current subtask */    /*E0217*/
      
  i      = gEnvColl->Count - 1;     /* current context index */    /*E0218*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0219*/

  if(RBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 132.052: wrong call delbg_\n"
        "(the regular access group was not created by the "
        "current subtask;\n"
        "RegularAccessGroupRef=%lx; RegularAccessGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RegularAccessGroupRefPtr, RBGHandlePtr->CrtEnvInd, i);

  /* Check if group loading is finished */    /*E0220*/

  if(RBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.053: wrong call delbg_\n"
           "(the group loading has not been completed; "
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  ( RTL_CALL, delobj_(RegularAccessGroupRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delbg_,"\n");

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* for
                                                           statistics */    /*E0221*/
  DVMFTimeFinish(ret_delbg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd rstbg_(RegularAccessGroupRef  *RegularAccessGroupRefPtr, DvmType  *DelBufSignPtr)
{ SysHandle       *RBGHandlePtr, *NewRBGHandlePtr;
  s_REGBUFGROUP   *RBG;
  int              i;
  void            *CurrAM;
  DvmType             StaticSign, DelBufSign;

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* */    /*E0222*/
  DVMFTimeStart(call_rstbg_);

  if(RTL_TRACE)
     dvm_trace(call_rstbg_,
         "RegularAccessGroupRefPtr=%lx; RegularAccessGroupRef=%lx; "
         "DelBufSign=%ld;\n",
         (uLLng)RegularAccessGroupRefPtr, *RegularAccessGroupRefPtr,
         *DelBufSignPtr);

  RBGHandlePtr = (SysHandle *)*RegularAccessGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RegularAccessGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 132.060: wrong call rstbg_\n"
            "(the regular access group is not a DVM object; "
            "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);
  }

  if(RBGHandlePtr->Type != sht_RegBufGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.061: wrong call rstbg_\n"
           "(the object is not a regular access group;\n"
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  RBG = (s_REGBUFGROUP *)RBGHandlePtr->pP;
  
  /* */    /*E0223*/
      
  i      = gEnvColl->Count - 1;     /* */    /*E0224*/
  CurrAM = (void *)CurrAMHandlePtr; /* */    /*E0225*/

  if(RBGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 132.062: wrong call rstbg_\n"
        "(the regular access group was not created by the "
        "current subtask;\n"
        "RegularAccessGroupRef=%lx; RegularAccessGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RegularAccessGroupRefPtr, RBGHandlePtr->CrtEnvInd, i);

  /* */    /*E0226*/

  if(RBG->LoadSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 132.063: wrong call rstbg_\n"
           "(the group loading has not been completed; "
           "RegularAccessGroupRef=%lx)\n", *RegularAccessGroupRefPtr);

  StaticSign     = RBG->Static;
  DelBufSign     = RBG->DelBuf;
  RBG->ResetSign = 1;  /* */    /*E0227*/

  ( RTL_CALL, delobj_(RegularAccessGroupRefPtr) );

  NewRBGHandlePtr = (SysHandle *)
                    ( RTL_CALL, crtbg_(&StaticSign, &DelBufSign) );

  RBG = (s_REGBUFGROUP *)NewRBGHandlePtr->pP;
  RBGHandlePtr->pP = NewRBGHandlePtr->pP;
  RBG->HandlePtr = RBGHandlePtr;

  NewRBGHandlePtr->Type = sht_NULL;
  dvm_FreeStruct(NewRBGHandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_rstbg_,"\n");

  StatObjectRef = (ObjectRef)*RegularAccessGroupRefPtr; /* */    /*E0228*/
  DVMFTimeFinish(ret_rstbg_);
  return  (DVM_RET, 0);
}


/* ---------------------------------------------- */    /*E0229*/


void   RegBufGroup_Done(s_REGBUFGROUP  *RBG)
{ int                     i;
  s_DISARRAY             *BArr;
  s_REGBUF               *RegBuf;
  RegularAccessGroupRef   RegBufGrpRef;
  DvmType                   *BufferHeader;

  if(RTL_TRACE)
     dvm_trace(call_RegBufGroup_Done, "RegularAccessGroupRef=%lx;\n",
                                      (uLLng)RBG->HandlePtr);

  if(RBG->LoadSign)
  {  RegBufGrpRef = (RegularAccessGroupRef)RBG->HandlePtr;
     ( RTL_CALL, waitbg_(&RegBufGrpRef) );
  }

  /* Cancel buffers included into group */    /*E0230*/

  for(i=0; i < RBG->RB.Count; i++)
  {  BArr = coll_At(s_DISARRAY *, &RBG->RB, i);
     RegBuf = BArr->RegBuf;
     RegBuf->RBG = NULL;   /* turn off reference to cancelled group */    /*E0231*/
     BufferHeader = (DvmType *)BArr->HandlePtr->HeaderPtr;

     if(RegBuf->LoadSign)
        ( RTL_CALL, waitrb_(BufferHeader) );

     if(RBG->DelBuf)
     {  if(DelObjFlag)   /* working from explicit group cancelation */    /*E0232*/
           ( RTL_CALL, delrb_(BufferHeader) );
        else             /* implicit buffer group cancelation */    /*E0233*/
        {  if(BArr->Static == 0)
              ( RTL_CALL, delrb_(BufferHeader) );
        }
     }
  }

  dvm_FreeArray(RBG->RB.List);

  if(TstObject)
     DelDVMObj((ObjectRef)RBG->HandlePtr);

  /* Cancel own Handle */    /*E0234*/

  if(RBG->ResetSign == 0)
  {  /* */    /*E0235*/

     RBG->HandlePtr->Type = sht_NULL;
     dvm_FreeStruct(RBG->HandlePtr);
  }

  RBG->ResetSign = 0;

  if(RTL_TRACE)
     dvm_trace(ret_RegBufGroup_Done, " \n");

  (DVM_RET);

  return;
}


/****************************************\
* Definition of access type to specified *
*       distributed array elements       *
\****************************************/    /*E0236*/

DvmType  __callstd  rmkind_(DvmType  ArrayHeader[], DvmType  BufferHeader[],
                           void *BasePtr, DvmType *StaticSignPtr,
                           LoopRef *LoopRefPtr, DvmType  AxisArray[],
                           DvmType  CoeffArray[], DvmType  ConstArray[],
                           DvmType  LowShdWidthArray[],
                           DvmType  HiShdWidthArray[])
{ SysHandle     *ArrayHandlePtr, *LoopHandlePtr;
  s_DISARRAY    *DArr;
  s_AMVIEW      *DAAMV, *PLAMV;
  s_PARLOOP     *PL;
  s_VMS         *PLVMS, *DAVMS;
  s_SPACE        PLSpace;
  s_BLOCK       *Local, Block;
  int            i, j, AR, LR, PLProc, DAProc, TAxis,
                 ShdCount, ShdNumber = 0;
  DvmType           lv, lv1, VMSize, Proc;
  DvmType           Res = 1;         /* local access */    /*E0237*/
  byte           TstArray[MAXARRAYDIM];
  DvmType           InitIndex[MAXARRAYDIM], LastIndex[MAXARRAYDIM];

  DVMFTimeStart(call_rmkind_);

  if(RTL_TRACE)
     dvm_trace(call_rmkind_,
        "ArrayHeader=%lx; ArrayHandlePtr=%lx; BufferHeader=%lx; "
        "BasePtr=%lx; StaticSign=%ld; LoopRefPtr=%lx; LoopRef=%lx;\n",
        (uLLng)ArrayHeader, ArrayHeader[0], (uLLng)BufferHeader,
        (uLLng)BasePtr, *StaticSignPtr, (uLLng)LoopRefPtr, *LoopRefPtr);

  if(TstDVMArray(BufferHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 133.000: wrong call rmkind_\n"
              "(BufferHeader already exists; "
              "BufferHeader[0]=%lx)\n", BufferHeader[0]);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 133.001: wrong call rmkind_\n"
        "(the object is not a distributed array;\n"
        "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_rmkind_))
     {  for(i=0; i < AR; i++)
            tprintf(" AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  DAAMV  = DArr->AMView;

  if(DAAMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 133.002: wrong call rmkind_\n"
              "(the array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);
 
  /* Check if processor system, on which specified array
      is mapped, subsystem of current processor system   */    /*E0238*/

  NotSubsystem(i, DVM_VMS, DAAMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 133.003: wrong call rmkind_\n"
       "(the array PS is not a subsystem of the current PS;\n"
       "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
       ArrayHeader[0], (uLLng)DAAMV->VMS->HandlePtr,
       (uLLng)DVM_VMS->HandlePtr);

  /* Control of specified parallel loop */    /*E0239*/

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 133.004: wrong call rmkind_\n"
            "(the object is not a parallel loop; "
            "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 133.005: wrong call rmkind_\n"
         "(the current context is not the parallel loop; "
         "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;
  LR = PL->Rank;
  PLAMV = PL->AMView;

  if(PLAMV == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 133.006: wrong call rmkind_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(PLAMV && PL->Align)
  {  /* Loop is not empty */    /*E0240*/

     /* Control of specified parameters */    /*E0241*/

     for(i=0; i < AR; i++)
     {  if(AxisArray[i] < -1)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 133.010: wrong call rmkind_\n"
                    "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);

        if(AxisArray[i] > LR)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 133.011: wrong call rmkind_\n"
                    "(AxisArray[%d]=%ld > %d; LoopRef=%lx)\n",
                    i, AxisArray[i], LR, *LoopRefPtr);

        if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           {  if(AxisArray[i] == 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                         "*** RTS err 133.012: wrong call rmkind_\n"
                         "(AxisArray[%d]=0)\n", i);

             lv = CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
                  ConstArray[i];

             if(lv < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 133.013: wrong call rmkind_\n"
                   "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                   " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                   i, CoeffArray[i], AxisArray[i]-1,
                   PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                   *LoopRefPtr);

             if(lv >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 133.014: wrong call rmkind_\n"
                    "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                    " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                    i, CoeffArray[i], AxisArray[i]-1,
                    PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                    DArr->Space.Size[i], *LoopRefPtr);

             lv = CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
                  ConstArray[i];

             if(lv < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 133.015: wrong call rmkind_\n"
                   "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                   " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                   i, CoeffArray[i], AxisArray[i]-1,
                   PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                   *LoopRefPtr);

             if(lv >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 133.016: wrong call rmkind_\n"
                    "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                    " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                    i, CoeffArray[i], AxisArray[i]-1,
                    PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                    DArr->Space.Size[i], *LoopRefPtr);
           }
           else
           { if(ConstArray[i] < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 133.017: wrong call rmkind_\n"
                       "(ConstArray[%d]=%ld < 0)\n",
                       i, ConstArray[i]);
             if(ConstArray[i] >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 133.018: wrong call rmkind_\n"
                       "(ConstArray[%d]=%ld >= %ld)\n",
                       i, ConstArray[i], DArr->Space.Size[i]);
           }
        }
     }

     for(i=0; i < LR; i++)
         TstArray[i] = 0;

     for(i=0; i < AR; i++)
     {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
           continue;

        if(TstArray[AxisArray[i]-1])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 133.019: wrong call rmkind_\n"
                   "(AxisArray[%d]=AxisArray[%d]=%ld)\n",
                   (int)(TstArray[AxisArray[i]-1]-1), i, AxisArray[i]);

        TstArray[AxisArray[i]-1] = (byte)(i+1);
     }

     PLVMS = PLAMV->VMS;
     DAVMS = DAAMV->VMS;
     VMSize = DVM_VMS->ProcCount;

     /* Loop in current processor system  processors */    /*E0242*/

     for(i=0; i < VMSize; i++)
     {  Proc = DVM_VMS->VProc[i].lP;
        PLProc = IsProcInVMS(Proc, PLVMS);

        if(PLProc < 0)
           continue;  /* current processor is not from
                         loop processor system */    /*E0243*/

        DAProc = IsProcInVMS(Proc, DAVMS);

        if(DAProc < 0)
        {  Res = 4;   /* remote access */    /*E0244*/
           break;     /* current processor is not
                         from array processor system */    /*E0245*/
        }

        /* Calculation of loop local part for current processor */    /*E0246*/

        PLSpace.Rank = (byte)LR;

        for(j=0; j < LR; j++)
            PLSpace.Size[j] = PL->Set[j].Size;

        Local = GetSpaceLB4Proc(PLProc, PLAMV, &PLSpace, PL->Align,
                                NULL, &Block);
        if(Local == NULL)
           continue;      /* no loop local part on 
                             current processor */    /*E0247*/

        for(j=0; j < LR; j++)
        {  lv = PL->Set[j].Step;
           InitIndex[j] = lv * (DvmType)ceil((double)Local->Set[j].Lower / (double)lv );
           LastIndex[j] = (Local->Set[j].Upper / lv) * lv;

           if(InitIndex[j] > LastIndex[j])
              break;
        }

        if(j < LR)
           continue;   /* initial index greater than final */    /*E0248*/

        for(j=0; j < LR; j++)
        {  InitIndex[j] += PL->Set[j].Lower;
           LastIndex[j] += PL->Set[j].Lower;
        }

        /* Calculation of array local part for current processor */    /*E0249*/

        Local = GetSpaceLB4Proc(DAProc, DAAMV, &DArr->Space,
                                DArr->Align, NULL, &Block);
        if(Local == NULL)
        {  Res = 4;       /* remote access */    /*E0250*/
           continue;      /* no array local part 
                             on current processor */    /*E0251*/
        }
        
        /* Check if requested part of array belongs to its local part */    /*E0252*/

        for(j=0,ShdCount=0; j < AR; j++)
        {  if(AxisArray[j] >= 0)
           {  if(CoeffArray[j] != 0)
              {  /* Linear selection rule */    /*E0253*/

                 lv = InitIndex[AxisArray[j]-1] * CoeffArray[j] +
                      ConstArray[j];

                 lv1 = LastIndex[AxisArray[j]-1] * CoeffArray[j] +
                       ConstArray[j];

                 if(lv < Local->Set[j].Lower  ||
                    lv > Local->Set[j].Upper  ||
                    lv1 < Local->Set[j].Lower ||
                    lv1 > Local->Set[j].Upper)
                 {  ShdCount++; /* to the number of dimensions where
                                   out of the array local part */    /*E0254*/
                    continue;   /* out of the array local part */    /*E0255*/
                 }
              }
              else
              {  /* Constant selection rule */    /*E0256*/

                 if(ConstArray[j] < Local->Set[j].Lower ||
                    ConstArray[j] > Local->Set[j].Upper   )
                 {  ShdCount++; /* to the number of dimensions where
                                   out of the array local part */    /*E0257*/
                    continue;   /* out of the array local part */    /*E0258*/
                 }
              }
           }
           else
           {  /* All array dimension is needed */    /*E0259*/

              if(DArr->Align[j].Attr == align_COLLAPSE)
                 continue;
              TAxis = DArr->Align[j].TAxis;
              if(DArr->AMView->DISTMAP[TAxis-1].Attr != map_COLLAPSE)
              {  Res = 4;    /* remote access */    /*E0260*/
                 break;
              }
           }
        }

        ShdNumber = dvm_max(ShdNumber, ShdCount);
 
        if(j == AR && ShdCount == 0)
           continue;   /* all array dimensions in the frame
                          of its loacl part */    /*E0261*/

        if(Res == 4)
           break;  /* during the check if the requested array part
                     belongs to its local part, 
                     remote access was identified */    /*E0262*/

        /* Check if requeste array part belongs to
             its local part extended with edges    */    /*E0263*/

        UserSumFlag = 0;  /* */    /*E0264*/

        AppendBounds(Local, DArr);  /* in Local - local part of
                                       array with edges */    /*E0265*/
        UserSumFlag = 1;

        for(j=0; j < AR; j++)
        {  if(AxisArray[j] >= 0)
           {  if(CoeffArray[j] != 0)
              {  /* Linear selection rule */    /*E0266*/

                 lv = InitIndex[AxisArray[j]-1] * CoeffArray[j] +
                      ConstArray[j];
                 if(lv < Local->Set[j].Lower ||
                    lv > Local->Set[j].Upper)
                    break;   /* out of array local part */    /*E0267*/

                 lv = LastIndex[AxisArray[j]-1] * CoeffArray[j] +
                      ConstArray[j];
                 if(lv < Local->Set[j].Lower ||
                    lv > Local->Set[j].Upper)
                    break;   /* out of array local part */    /*E0268*/
              }
              else
              {  /* Constant selection rule */    /*E0269*/

                 if(ConstArray[j] < Local->Set[j].Lower ||
                    ConstArray[j] > Local->Set[j].Upper   )
                    break;   /* out of array local part */    /*E0270*/
              }
           }
           else
           {  /* All array dimensions are needed */    /*E0271*/

              if(DArr->Align[j].Attr == align_COLLAPSE)
                 continue;
              TAxis = DArr->Align[j].TAxis;
              if(DArr->AMView->DISTMAP[TAxis-1].Attr != map_COLLAPSE)
              {  Res = 4;    /* remote access */    /*E0272*/
                 break;
              }
           }
        }

        if(j != AR)
        {  Res = 4;   /* remote access */    /*E0273*/
           break;
        }

        Res = 2;   /* access - edge exchange */    /*E0274*/
     }

     if(Res == 4)
       ( RTL_CALL, crtrbl_(ArrayHeader, BufferHeader, BasePtr,
                           StaticSignPtr, LoopRefPtr,
                           AxisArray, CoeffArray, ConstArray) );

     if(Res == 2)
     {  /* Check if  access is exchange of full edge */    /*E0275*/

        if(ShdNumber > 1)
           Res = 3; /* access - exchange of full edge */    /*E0276*/

        /* Rewrite edge widths into output arrays */    /*E0277*/

        for(i=0; i < AR; i++)
        {  LowShdWidthArray[i] = DArr->InitLowShdWidth[i];
           HiShdWidthArray[i]  = DArr->InitHighShdWidth[i];
        }
     }
  }

  if (EnableDynControl)
      dyn_DisArrDisableLocalCheck(ArrayHandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_rmkind_,"Res=%ld\n", Res);

  DVMFTimeFinish(ret_rmkind_);
  return  (DVM_RET, Res);
}


#endif  /*  _RGACCESS_C_  */    /*E0278*/
