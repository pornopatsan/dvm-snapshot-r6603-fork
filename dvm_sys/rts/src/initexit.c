#ifndef _INITEXIT_C_
#define _INITEXIT_C_
/******************/    /*E0000*/

/***********************************\
* Initialization of  support system *
\***********************************/    /*E0001*/

#ifdef  _DVM_MPI_

  extern MPI_Comm    DVM_COMM_WORLD;

#endif


  void dvm_Init(DvmType  InitParam)
{
  int              i, j, elm, LinInd;
  s_ENVIRONMENT   *Env;
  DVMFILE         *fpar;

#ifndef _DVM_IOPROC_
  char             FileName[128];
#endif

  PSRef            InitPSRef;
  DvmType             VProcCount, GenBlock = 0;


  if(RTL_TRACE)
     dvm_trace(call_dvm_Init, "InitParam=%lx\n", InitParam);

  if(dvm_OneProcSign)
     Is_DVM_STAT = 0;

  if(IAmIOProcess)
  {
     _SysInfoPrint   = 0;
     IsUserPS        = 0;
     TstObject       = 0;
     Is_DVM_STAT     = 0;
     TimeExpendPrint = -1;
     SendRecvTime    = 0;
  }

  /* */    /*E0002*/

  i = sizeof(DvmType);

  if(i > sizeof(double))
     GlobalBasePtr = (void *)&GlobalLong;
  else
     GlobalBasePtr = (void *)&GlobalDouble;

  /* Create current context */    /*E0003*/

  dvm_AllocStruct(s_COLLECTION, gEnvColl);
  *gEnvColl = coll_Init(EnvCount, EnvCount, env_Done);
  Env = env_Init(&InitAMSHandle);
  coll_Insert(gEnvColl, Env);


  Env->EnvProcCount   = ProcCount;    /* number of processors performing
                                         the current thread */    /*E0004*/ 
  OldEnvProcCount     = ProcCount;
  CurrEnvProcCount    = ProcCount;
  CurrEnvProcCount_m1 = ProcCount - 1;
  d1_CurrEnvProcCount = 1./ProcCount;
  NewEnvProcCount     = ProcCount;

  /* Create the array CoordWeight1 with unit
     processor coordinate weights for setpsw_ */    /*E0005*/

  LinInd = ProcCount + MAXARRAYDIM;
  dvm_AllocArray(double, LinInd, CoordWeight1);

  for(i=0; i < LinInd; i++)
      CoordWeight1[i] = 1.;

  /* Create current processor system */    /*E0006*/

  dvm_AllocArray(SysHandle, ProcCount, gVirtProcs);

  for(i=0; i < ProcCount; i++) /* build array of SysHandle's
                                  processors in initial processor
                                  system */    /*E0007*/
      gVirtProcs[i] = sysh_Build(sht_VProc, -1, -1, i, NULL);

  gInitialVMS = (SysHandle *)
                ( RTL_CALL, CreateVMS(gVirtProcs, VMSRank, VMSSize, 1) );

  InitPSRef = (PSRef)gInitialVMS;     /* reference to initial processor
                                         system */    /*E0008*/
  gInitialVMS->CrtEnvInd = -1;        /* index of context creation */    /*E0009*/
  gInitialVMS->EnvInd    = -1;
  MPS_VMS = (s_VMS *)gInitialVMS->pP; /* initial processor 
                                         system */    /*E0010*/

  /* */    /*E0011*/

  for(i=0; i < VMSRank; i++)
      MPS_VMS->InitIndex[i] = 0;

  /* ------------------------------------------------------- */    /*E0012*/

  MPS_VMS->CrtEnvInd = -1;            /* index of context creation */    /*E0013*/
  MPS_VMS->EnvInd =    -1;

  coll_Insert(&MPS_VMS->AMSColl, &InitAMS); /* into the list of 
                                               abstract machines mapped
                                               on the current processor
                                               system */    /*E0014*/
  DVM_VMS = MPS_VMS; /* current processor system */    /*E0015*/

  MPS_IOProc = MPS_VMS->IOProc; /* internal number of 
                                   input/output processor in
                                   initial processor system */    /*E0016*/
  MPS_CentralProc = MPS_VMS->CentralProc; /* internal number of
                                             central processor in
                                             initial processor
                                             system */    /*E0017*/

  DVM_MasterProc = MPS_VMS->MasterProc; /* internal number of
                                           main processor in 
                                           current processor system */    /*E0018*/
  DVM_IOProc = MPS_VMS->IOProc; /* internal number of input/output
                                   processor in current
                                   processor system */    /*E0019*/
  DVM_CentralProc = MPS_VMS->CentralProc; /* internal numebr of
                                             central processor
                                             in current 
                                             processor system */    /*E0020*/
  DVM_ProcCount = MPS_VMS->ProcCount; /* number of processors in
                                         current processor system */    /*E0021*/

  /* Arrange space of user program processor system */    /*E0022*/

  if(IsUserPS)
     MPS_VMS->TrueSpace = space_Init(VMSRank, UserPS);

  /* */    /*E0023*/

#ifdef  _DVM_MPI_

  DVM_VMS->Is_MPI_COMM  = 1;
  MPI_Comm_dup(DVM_COMM_WORLD, &DVM_VMS->PS_MPI_COMM);
  MPI_Comm_group(DVM_COMM_WORLD, &DVM_VMS->ps_mpi_group);

#endif

  /* Create current processor system */    /*E0024*/

  InitAMSHandle = sysh_Build(sht_AMS, 0, -1, 0, &InitAMS);

  InitAMS.EnvInd    = 0;              /* current context index */    /*E0025*/
  InitAMS.CrtEnvInd = -1;             /* current context index */    /*E0026*/
  InitAMS.HandlePtr = &InitAMSHandle; /* pointer to own Handle */    /*E0027*/
  InitAMS.TreeIndex = 0;              /* distance from the root
                                         of abstract machine tree */    /*E0028*/
  InitAMS.ParentAMView = NULL;        /* no representation of 
                                         parent machine */    /*E0029*/
  InitAMS.VMS = MPS_VMS; /* processor system on which the current
                            abstarct machine is mapped */    /*E0030*/
  InitAMS.SubSystem = coll_Init(AMSAMVCount, AMSAMVCount, NULL);

  /* Previous, current and next abstarct machines */    /*E0031*/

  OldAMHandlePtr  = &InitAMSHandle;
  CurrAMHandlePtr = &InitAMSHandle;
  NewAMHandlePtr  = &InitAMSHandle;

  if(TstObject)
     InsDVMObj((ObjectRef)&InitAMSHandle);

  /* processor performance measuring */    /*E0032*/

  if(IAmIOProcess == 0)
  {
     sysprintf(__FILE__,__LINE__, "call GetProcPower\n");
     GetProcPower();
     sysprintf(__FILE__,__LINE__, "ret GetProcPower\n");
  }

  /* Form for each dimension of the initial processor system
      the array of processor coordinate weights and the array
        of summed previous processor coordinate weights        */    /*E0033*/

  for(i=0; i < VMSRank; i++)
  {
     MPS_VMS->CoordWeight[i] = NULL;
     MPS_VMS->PrevSumCoordWeight[i] = NULL;
  }

  ( RTL_CALL, setpsw_(&InitPSRef, NULL, CoordWeightList,
                      NULL) );

  /* Initialize set of started reduction groups */    /*E0034*/

  dvm_AllocStruct(s_COLLECTION, gRedGroupColl);
  *gRedGroupColl = coll_Init(StrtRedGrpCount, StrtRedGrpCount, NULL);

  AlignAddition = AlignMemoryAddition; /* */    /*E0035*/

  MinParSym = 1;  /* min number of characters in parameter name 
                     for user program */    /*E0036*/

  IsDVMInit = 1;  /* DVM LIB has been initialized */    /*E0037*/

#ifdef _DVM_MPI_

  /* */    /*E0038*/

  if(IOProcess)
  {
     /* */    /*E0039*/

     if(ApplProcessCount != IOProcessCount)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS fatal err: applied process number (%d) is not"
                 " equal to input/out process number (%d)\n",
                 ApplProcessCount, IOProcessCount);

     mac_malloc(ApplNumber, int *, ApplProcessCount*sizeof(int), 0);
     mac_malloc(IONumber, int *, IOProcessCount*sizeof(int), 0);

     for(i=0; i < ApplProcessCount; i++)
         ApplNumber[i] = i;         /* */    /*E0040*/
     for(i=0; i < IOProcessCount; i++)
         IONumber[i] = i;           /* */    /*E0041*/

     MyApplProc = CurrentProcIdent;  /* */    /*E0042*/
     MyIOProc = CurrentProcIdent;    /* */    /*E0043*/
  }

  /* */    /*E0044*/

  if(SysProcessNamePrint && _SysInfoPrint && IAmIOProcess == 0)
  {
     if(MPS_CurrentProc == MPS_MasterProc)
     {
        rtl_iprintf("Applied processes number = %-4d  "
                    "Input/output processes number = %-d\n\n",
                    ApplProcessCount, IOProcessCount);

        rtl_iprintf("Applied:                         "
                    "Input/output:\n");   
        rtl_iprintf("    N     IN     NAME            "
                    "  N  IN     NAME\n");   

        LinInd = dvm_max(ApplProcessCount, IOProcessCount);

        for(i=0; i < LinInd; i++)
        {
           if(i < ApplProcessCount && i < IOProcessCount)
           {
              rtl_iprintf("%4d_%-4d %-4d   %-15s%4d  %-4d   %-s\n",
              i, ProcIdentList[i], ApplProcessNumber[ProcIdentList[i]],
              MPS_ProcNameList[ApplProcessNumber[ProcIdentList[i]]],
              i, IOProcessNumber[i],
              MPS_ProcNameList[IOProcessNumber[i]]);
           }
           else
           {
              if(i < ApplProcessCount)
              {
                 rtl_iprintf("%4d_%-4d %-4d   %-16s%-s\n",
                 i, ProcIdentList[i],
                 ApplProcessNumber[ProcIdentList[i]],
                 MPS_ProcNameList[ApplProcessNumber[ProcIdentList[i]]],
                 "  -");
              }
              else
              {
                 rtl_iprintf("                 "
                             "                "
                             "%4d %-4d   %-s\n",
                 i, IOProcessNumber[i],
                 MPS_ProcNameList[IOProcessNumber[i]]);
              }
           }
        }

        rtl_iprintf(" \n");
     }
  }

  if(SysProcessNameTrace)
  {
     tprintf("Applied processes number = %-4d  "
             "Input/output processes number = %-d\n\n",
             ApplProcessCount, IOProcessCount);

     tprintf("Applied:                         "
             "Input/output:\n");   
     tprintf("    N     IN     NAME            "
             "  N  IN     NAME\n");   

     LinInd = dvm_max(ApplProcessCount, IOProcessCount);

     for(i=0; i < LinInd; i++)
     {
        if(i < ApplProcessCount && i < IOProcessCount)
        {
           tprintf("%4d_%-4d %-4d   %-15s%4d  %-4d   %-s\n",
           i, ProcIdentList[i], ApplProcessNumber[ProcIdentList[i]],
           MPS_ProcNameList[ApplProcessNumber[ProcIdentList[i]]],
           i, IOProcessNumber[i],
           MPS_ProcNameList[IOProcessNumber[i]]);
        }
        else
        {
           if(i < ApplProcessCount)
           {
              tprintf("%4d_%-4d %-4d   %-16s%-s\n",
              i, ProcIdentList[i],
              ApplProcessNumber[ProcIdentList[i]],
              MPS_ProcNameList[ApplProcessNumber[ProcIdentList[i]]],
              "  -");
           }
           else
           {
              tprintf("                 "
                      "               "
                      "%4d %-4d   %-s\n",
              i, IOProcessNumber[i],
              MPS_ProcNameList[IOProcessNumber[i]]);
           }
        }
     }

     tprintf(" \n");
  }

#endif

  /* Print initialization parameters into screen */    /*E0045*/

  if(SysParPrint && _SysInfoPrint)
  {
     if( MPS_CurrentProc == MPS_MasterProc )
     {
       rtl_iprintf(" ProcCount=%ld;      PSRank=%d;      PSSize=",
                   ProcCount, (int)VMSRank);
       for(i=0; i < VMSRank; i++)
           rtl_iprintf("%  ld", VMSSize[i]);

       rtl_iprintf(";\n");

       if(IsUserPS)
       {
          for(i=0,VProcCount=1; i < VMSRank; i++)
              VProcCount *= MPS_VMS->TrueSpace.Size[i];

          rtl_iprintf("VProcCount=%ld;     VPSRank=%d;     VPSSize=",
                      VProcCount, (int)VMSRank);
          for(i=0; i < VMSRank; i++)
              rtl_iprintf("%  ld", MPS_VMS->TrueSpace.Size[i]);

          rtl_iprintf(";\n");
       }
       else
          rtl_iprintf("VProcCount=0;\n");

       rtl_iprintf("MasterProc=%d(%d);   IOProc=%d(%d);   "
                   "CentralProc=%d(%d);   MeanProcTime=%lf;\n",
                   MPS_MasterProc, ProcNumberList[MPS_MasterProc],
                   MPS_IOProc, ProcNumberList[MPS_IOProc],
                   MPS_CentralProc, ProcNumberList[MPS_CentralProc],
                   MeanProcPower);
       rtl_iprintf(" \n");
     } 
  }

  if(ProcListPrint && _SysInfoPrint)
  {
     if(MPS_CurrentProc == MPS_MasterProc)
     {
        rtl_iprintf("IntProcNumber  ExtProcNumber  ProcIdent"
                    "  ProcWeight\n");

        for(i=0; i < ProcCount; i++)
            rtl_iprintf("    %3d            %3d          %3d    "
                        "    %4.2lf\n", i, ProcNumberList[i],
                        ProcIdentList[i], ProcWeightArray[i]);

        rtl_iprintf(" \n");
     } 
  }

  if(WeightListPrint && _SysInfoPrint)
  {
     if(MPS_CurrentProc == MPS_MasterProc)
     {
        for(i=0,LinInd=0; i < MPS_VMS->Space.Rank; i++)
        {
          rtl_iprintf("CoordWeight[%d]= ", i);

          for(j=0,elm=0; j < MPS_VMS->Space.Size[i]; j++,elm++,LinInd++)
          {
            if(elm == 5)
            {  elm = 0;
               rtl_iprintf(" \n                ");
            }

            rtl_iprintf("%4.2lf(%4.2lf) ", MPS_VMS->CoordWeight[i][j],
                                           CoordWeightList[LinInd]);
          }

          rtl_iprintf(" \n");
        }

        rtl_iprintf(" \n");
     }
  }

  if(MsgSchedulePrint && _SysInfoPrint)
  {
     if(MPS_CurrentProc == MPS_MasterProc)
     {
        rtl_iprintf("MaxMsgLength  MsgBufLength  MsgBuf1Length  "
                    "MaxMsgSendNumber  ChanNumber  Dupl\n");
        rtl_iprintf(" %10d    %10d    %10d       %10d    "
                    "    %4d       %1d\n",
                    MaxMsgLength, MsgBufLength, MsgBuf1Length,
                    MaxMsgSendNumber, ParChanNumber, (int)DuplChanSign);
        rtl_iprintf("                                              "
                    "%10d\n", MaxMsg1SendNumber);
        rtl_iprintf(" \n");
     } 
  }

  /* Print initialization parameters in trace */    /*E0046*/

  if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
  {
     tprintf(" ProcCount=%ld;      PSRank=%d;      PSSize=",
             ProcCount, (int)VMSRank);

     for(i=0; i < VMSRank; i++)
         tprintf("%  ld", VMSSize[i]);

     tprintf(";\n");

     if(IsUserPS)
     {
        for(i=0,VProcCount=1; i < VMSRank; i++)
            VProcCount *= MPS_VMS->TrueSpace.Size[i];

        tprintf("VProcCount=%ld;     VPSRank=%d;     VPSSize=",
                VProcCount, (int)VMSRank);
        for(i=0; i < VMSRank; i++)
            tprintf("%  ld", MPS_VMS->TrueSpace.Size[i]);

        tprintf(";\n");
     }
     else
        tprintf("VProcCount=0;\n");

     tprintf("MasterProc=%d(%d);   IOProc=%d(%d);   CentralProc=%d(%d);"
             "   MeanProcTime=%lf;\n",
              MPS_MasterProc, ProcNumberList[MPS_MasterProc],
              MPS_IOProc, ProcNumberList[MPS_IOProc],
              MPS_CentralProc, ProcNumberList[MPS_CentralProc],
              MeanProcPower);
     tprintf(" \n");

     tprintf("IntProcNumber  ExtProcNumber  ProcIdent"
             "  ProcWeight\n");

     for(i=0; i < ProcCount; i++)
         tprintf("    %3d            %3d          %3d    "
                 "    %4.2lf\n", i, ProcNumberList[i], ProcIdentList[i],
                                 ProcWeightArray[i]);
     tprintf(" \n");


     for(i=0,LinInd=0; i < MPS_VMS->Space.Rank; i++)
     {
        tprintf("CoordWeight[%d]= ", i);

        for(j=0,elm=0; j < MPS_VMS->Space.Size[i]; j++, elm++, LinInd++)
        {
           if(elm == 5)
           {
              elm = 0;
              tprintf(" \n                ");
           }

           tprintf("%4.2lf(%4.2lf) ", MPS_VMS->CoordWeight[i][j],
                                      CoordWeightList[LinInd]);
        }

        tprintf(" \n");
     }

     tprintf(" \n");

     tprintf("MaxMsgLength  MsgBufLength  MsgBuf1Length  "
             "MaxMsgSendNumber  ChanNumber  Dupl\n");
     tprintf(" %10d    %10d    %10d       %10d    "
             "    %4d       %1d\n",
             MaxMsgLength, MsgBufLength, MsgBuf1Length,
             MaxMsgSendNumber, ParChanNumber, (int)DuplChanSign);
     tprintf("                                              %10d\n",
             MaxMsg1SendNumber);

     tprintf(" \n");
  }

  /* Information about start parameters output into screen */    /*E0047*/

  i = 1;

  if(ParFileCheckSum)
  {
     if(SystemCheckSum != SystemStdCheckSum)
        i = 0;
     if(SysTraceCheckSum != SysTraceStdCheckSum)
        i = 0;
     if(DebugCheckSum != DebugStdCheckSum)
        i = 0;
     if(TrcEventCheckSum != TrcEventStdCheckSum)
        i = 0;
     if(TrcDynControlCheckSum != TrcDynControlStdCheckSum)
        i = 0;
     if(StatistCheckSum != StatistStdCheckSum)
        i = 0;
  }

  if(ParamRunPrint && _SysInfoPrint)
  {
     if( MPS_CurrentProc == MPS_MasterProc )
     {
        switch(MPS_TYPE)
        {  case EMP_MPS_TYPE: rtl_iprintf("MPS=EMP;  ");
                              break;
           case GNS_MPS_TYPE: rtl_iprintf("MPS=GNS;  ");
                              break;
           case ROU_MPS_TYPE: rtl_iprintf("MPS=ROU;  ");
                              break;
           case MPI_MPS_TYPE: rtl_iprintf("MPS=MPI;  ");
                              break;
           case PVM_MPS_TYPE: rtl_iprintf("MPS=PVM;  ");
                              break;
        }

        if(i && StdStart)
           rtl_iprintf("Standard run;\n");
        else
           rtl_iprintf("Non-standard run;\n");
        rtl_iprintf(" \n");
     }
  }
             
  /* Information about start parameters output into trace */    /*E0048*/

  if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
  {
     switch(MPS_TYPE)
     {
        case EMP_MPS_TYPE: tprintf("MPS=EMP;  ");
                           break;
        case GNS_MPS_TYPE: tprintf("MPS=GNS;  ");
                           break;
        case ROU_MPS_TYPE: tprintf("MPS=ROU;  ");
                           break;
        case MPI_MPS_TYPE: tprintf("MPS=MPI;  ");
                           break;
        case PVM_MPS_TYPE: tprintf("MPS=PVM;  ");
                           break;
     }
                               
     if(i && StdStart)
        tprintf("Standard run;\n");
     else
        tprintf("Non-standard run;\n");

     tprintf(" \n");
  }

  /* Create file with checksums of parameter files, if it does not exist */    /*E0049*/

  if(FirstCheckSumFile[0] != '\x00' && IAmIOProcess == 0)
  {
     fpar=( RTL_CALL, dvm_fopen(FirstCheckSumFile, OPENMODE(r)) );

     if(fpar == NULL)
     {
        fpar=( RTL_CALL, dvm_fopen(FirstCheckSumFile, OPENMODE(w)) );

        if(fpar)
        {
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "ParFileCheckSum=%d;\n",1) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "SystemStdCheckSum=%lu;\n",
                                        SystemCheckSum) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "SysTraceStdCheckSum=%lu;\n",
                                        SysTraceCheckSum) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "DebugStdCheckSum=%lu;\n",
                                        DebugCheckSum) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "TrcEventStdCheckSum=%lu;\n",
                                        TrcEventCheckSum) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                     "TrcDynControlStdCheckSum=%lu;\n",
                                      TrcDynControlCheckSum) );
          ( RTL_CALL, dvm_void_fprintf(fpar,
                                       "StatistStdCheckSum=%lu;\n",
                                        StatistCheckSum) );
          ( RTL_CALL, dvm_fclose(fpar) );
        }
     }
     else
        ( RTL_CALL, dvm_fclose(fpar) );
  }

#ifndef _DVM_IOPROC_

  /* initialization of  dynamic control tools */    /*E0050*/

  if(0x1 & InitParam)
  {
     EnableDynControl = 0;
     EnableTrace = 0;
  }

  if(MPS_CurrentProc == MPS_MasterProc && IAmIOProcess == 0 &&
     DelUsrTrace && EnableDynControl == 0 && EnableTrace == 0)
  {
     /* Delete old files with user tarcing
        error messages from debugging tools */    /*E0051*/

     for(i=0; i < MaxProcNumber; i++)
     { SYSTEM(sprintf,(FileName,"%s%s%d.%s",
                       TraceOptions.TracePath, TraceOptions.OutputTracePrefix, i, TraceOptions.Ext))
       SYSTEM_RET(j, remove, (FileName))

       if(j != 0)
          break;   /* no more files */    /*E0052*/
     }

     for(i=0; i < MaxProcNumber; i++)
     { SYSTEM(sprintf,(FileName,"%s%s%d.%s",
                       TraceOptions.TracePath, TraceOptions.FileLoopInfo, i, TraceOptions.Ext))
       SYSTEM_RET(j, remove, (FileName))

       if(j != 0)
          break;
     }


     if ( TraceOptionsTraceFile[0] != 0 )
     {
        SYSTEM(sprintf, (FileName, "%s%s",
                        TraceOptions.TracePath, TraceOptionsTraceFile))
        SYSTEM(remove, (FileName))
        SYSTEM(remove, (TraceOptionsTraceFile))
     }

     SYSTEM(sprintf, (FileName, "%s%s",
                      TraceOptions.TracePath, TraceOptions.ErrorFile))
     SYSTEM(remove, (FileName))
     SYSTEM(sprintf, (FileName, "%s%s",
                      TraceOptions.TracePath, DebugOptions.ErrorFile))
     SYSTEM(remove, (FileName))

     SYSTEM(remove, (TraceOptions.FileLoopInfo))
     SYSTEM(remove, (TraceOptions.ErrorFile))
     SYSTEM(remove, (DebugOptions.ErrorFile))
  }

#endif

  if(IAmIOProcess == 0)
     mps_Barrier(); /* when files are opened
                    another processor cannot delete them */    /*E0053*/

#ifndef _DVM_IOPROC_

  if(IAmIOProcess == 0)
  {
     sysprintf(__FILE__,__LINE__, "call cntx_Init\n");
     cntx_Init();
     sysprintf(__FILE__,__LINE__, "ret cntx_Init\n");

     sysprintf(__FILE__,__LINE__, "call dyn_Init\n");
     dyn_Init();
     sysprintf(__FILE__,__LINE__, "ret dyn_Init\n");

     sysprintf(__FILE__,__LINE__, "call cmptrace_Init\n");
     cmptrace_Init();
     sysprintf(__FILE__,__LINE__, "ret cmptrace_Init\n");
  }

#endif

  if(MPS_CurrentProc == MPS_MasterProc && DelStatist &&
     IAmIOProcess == 0)
  {
     SYSTEM(remove, (StatFileName))  /* delete old
                                       statistics file */    /*E0054*/
     SYSTEM(strcpy, (DVM_String, StatFileName))
     SYSTEM(strcat, (DVM_String, ".gz"))
     SYSTEM(remove, (DVM_String))
     SYSTEM(strcat, (DVM_String, "+"))
     SYSTEM(remove, (DVM_String))
  }

  /* Synchronization of all processors */    /*E0055*/

  if(IAmIOProcess == 0)
  {
     sysprintf(__FILE__,__LINE__, "call tsynch\n");
     ( RTL_CALL, tsynch_() );     /* */    /*E0056*/
     sysprintf(__FILE__,__LINE__, "ret tsynch\n");
  }

  Curr_dvm_time = dvm_time();    /* current system time */    /*E0057*/
  Init_dvm_time = Curr_dvm_time; /* initial system time */    /*E0058*/
  DVM_TIME1 += dvm_TimeDelta;    /* */    /*E0059*/
  SystemStartTime += dvm_TimeDelta;

  /* Measuring Tm, Tirecv and Twrecv times 
     for pipeline ACROSS scheme execution */    /*E0060*/

#ifndef _DVM_IOPROC_

  if(IAmIOProcess == 0)
  {
     sysprintf(__FILE__,__LINE__, "call GetAcrossTimes\n");
     GetAcrossTimes();
     sysprintf(__FILE__,__LINE__, "ret GetGetAcrossTimes\n");

     #ifdef _F_TIME_
        sysprintf(__FILE__,__LINE__, "call StatInit\n");

        StatInit();      /* initialization of  statistics */    /*E0061*/

        sysprintf(__FILE__,__LINE__, "ret StatInit\n");
     #else
        Is_DVM_STAT = 0;
     #endif
  }

#else

  Is_DVM_STAT = 0;

#endif

  /* */    /*E0062*/

  if(SendRecvTime == 0)
     SendRecvTimePrint = 0;

  _SendRecvTime = SendRecvTime;

  /* fill in global task interval */    /*E0063*/

  for(i=0; i < StatGrpCount; i++)
      for(j=0; j < StatGrpCount; j++)
      {
          CurrInterPtr[i][j].CallCount   = 0.;
          CurrInterPtr[i][j].ProductTime = 0.;
          CurrInterPtr[i][j].LostTime    = 0.;
          TaskInter[i][j].CallCount      = 0.;
          TaskInter[i][j].ProductTime    = 0.;
          TaskInter[i][j].LostTime       = 0.;
          DebugInter[i][j].CallCount     = 0.;
          DebugInter[i][j].ProductTime   = 0.;
          DebugInter[i][j].LostTime      = 0.;
      }
 
  if(_SendRecvTime && RTL_STAT)
  {
     SendRecvTimesPtr =
     (s_SendRecvTimes *)&CurrInterPtr[StatGrpCountM1][StatGrpCount];

     SendRecvTimesPtr->SendCallTime    = 0.;
     SendRecvTimesPtr->MinSendCallTime = 1.e10;
     SendRecvTimesPtr->MaxSendCallTime = 0.;
     SendRecvTimesPtr->SendCallCount   = 0;
     SendRecvTimesPtr->RecvCallTime    = 0.;
     SendRecvTimesPtr->MinRecvCallTime = 1.e10;
     SendRecvTimesPtr->MaxRecvCallTime = 0.;
     SendRecvTimesPtr->RecvCallCount   = 0;
  }

  /* ------------------------------------ */    /*E0064*/

  #ifdef _DVM_MPI_

    if(_SysInfoPrint == 0)
       MPIInfoPrint = 0;
  #else
       MPIInfoPrint = 0;
  #endif


  /*      Initialization of variables for
     macros DVMFTimeStart and DVMFTimeFinish */    /*E0065*/

  if(_SysInfoPrint == 0)
     TimeExpendPrint = -1;

  IsExpend = (byte)((TimeExpendPrint > 1 && _SysInfoPrint) || RTL_STAT);
  IsVariation = (byte)(RTL_STAT && IsTimeVariation);
  IsSynchr = (byte)(IsSynchrTime && IsExpend);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_Init," \n");

  _CurrentTimeTrace = 1; /* start tracing
                            current system time */    /*E0066*/

  (DVM_RET);

  /* Initialization of operation group stack */    /*E0067*/

  if(IAmIOProcess == 0)
  {
     StatGrpStack[0].GrpNumber   = UserGrp;
     StatGrpStack[0].ProductTime = 0.;
     StatGrpStack[0].LostTime    = 0.;
     StatGrpStack[0].dvm_time    = dvm_time();

     /* Dump standard streams into files */    /*E0068*/

     SYSTEM(fflush,(stdout))
     SYSTEM(fflush,(stderr))

     #ifdef _UNIX_
        SYSTEM(sync, ())
     #endif
  }

  return;
}


/*********************************\
* Processor performance measuring *
\*********************************/    /*E0069*/

void  CallCoil(int     int1,    int     int2,
               DvmType    long1,   DvmType    long2,
               float   float1,  float   float2,
               double  double1, double  double2,
               int    *int3,    int    *int4,    int     *int5,
               DvmType  *long3, DvmType   *long4, DvmType    *long5,
               float  *float3,  float  *float4,  float   *float5,
               double *double3, double *double4, double  *double5)
{
     /* Addition */    /*E0070*/

     *int3    = int1    + int2;
     *long3   = long1   + long2;
     *float3  = float1  + float2;
     *double3 = double1 + double2;

     /* Multiplication */    /*E0071*/

     *int4    = int1    * int2;
     *long4   = long1   * long2;
     *float4  = float1  * float2;
     *double4 = double1 * double2;

     /* Division */    /*E0072*/

     *int5    = int1    / int2;
     *long5   = long1   / long2;
     *float5  = float1  / float2;
     *double5 = double1 / double2;

     return;
}
 
 

void  GetProcPower(void)
{ int           i;
  double        time, MinWP;
  DvmType       lng;
  RTL_Request  *Req;
  RTL_Request   InReq;
  int           int1=123,          int2=567;
  DvmType       long1 = 1234, long2 = 4321;
  float         float1=0.1234f,    float2=54321.0f;
  double        double1=7654321.0, double2=0.1234567;
  int           int3,              int4,              int5;
  DvmType       long3, long4, long5;
  float         float3,            float4,            float5;
  double        double3,           double4,           double5;

  /* Current processor performance measuring */    /*E0073*/

  mps_Barrier(); /* */    /*E0074*/

  if(PPMeasureCount >= 0)
  {
     if(ArithmLoopCount > 0)
        PPMeasureCount = ArithmLoopCount; /* */    /*E0075*/ 
  }
  else
     PPMeasureCount *= -1;

  time = dvm_time();

#ifdef  _WIN_MPI_
  PPMeasureCount = 10 /* 1000000 */    /*E0076*/ ;
#endif

/*
  if(MPS_CurrentProc != 0)
  {
     PPMeasureCount *= 3;
     tprintf("++++++ PPMeasureCount=%d\n", (int)PPMeasureCount);
  }
*/    /*E0077*/

  for(lng=0; lng < PPMeasureCount; lng++)
  {
     CallCoil( int1,      int2,
               long1,     long2,
               float1,    float2,
               double1,   double2,
              &int3,     &int4,     &int5,
              &long3,    &long4,    &long5,
              &float3,   &float4,   &float5,
              &double3,  &double4,  &double5);
  }

  time = dvm_time() - time + 0.000001;

/*
  if(MPS_CurrentProc == 0 || MPS_CurrentProc == 1)
     time *= 2;
*/    /*E0078*/

  if(PPMeasurePrint && _SysInfoPrint)
     rtl_iprintf("ProcPowerMeasuring:  Proc=%d  PPMeasureCount=%ld  "
                 "PPMeasureTime=%lf\n",
                 MPS_CurrentProc, PPMeasureCount, time);

  if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
  {
     if(RTL_TRACE)
        tprintf("ProcPowerMeasuring:  PPMeasureCount=%ld  "
                "PPMeasureTime=%lf\n", PPMeasureCount, time);
  }

  if(MPS_CurrentProc == MPS_CentralProc)
  {
     /* Current processor is central one */    /*E0079*/

     ProcPowerArray[MPS_CurrentProc] = time;

     /* Receiving times from all non central processors */    /*E0080*/

     dvm_AllocArray(RTL_Request, ProcCount, Req);

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Recvnowait((void *)&ProcPowerArray[i], 1,
                                   sizeof(double), i,
                                   msg_ProcPowerMeasure, &Req[i], 0) );
     }

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Waitrequest(&Req[i]) );
     }

     /* Send processor performance array
         to all non central processor */    /*E0081*/

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Sendnowait((void *)ProcPowerArray, ProcCount,
                                   sizeof(double), i,
                                   msg_ProcPowerMeasure, &Req[i], 0) );
     }

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Waitrequest(&Req[i]) );
     }

     dvm_FreeArray(Req);
  }
  else
  {
     /* Current processor is not a central */    /*E0082*/

     ( RTL_CALL, rtl_Sendnowait((void *)&time, 1, sizeof(double),
                                MPS_CentralProc, msg_ProcPowerMeasure,
                                &InReq, 0) );
     ( RTL_CALL, rtl_Waitrequest(&InReq) );
     
     ( RTL_CALL, rtl_Recvnowait((void *)ProcPowerArray, ProcCount,
                                sizeof(double), MPS_CentralProc,
                                msg_ProcPowerMeasure, &InReq, 0) );
     ( RTL_CALL, rtl_Waitrequest(&InReq) );
  }

  /* Calculation of average processor performance */    /*E0083*/

  for(i=0; i < ProcCount; i++)
      MeanProcPower += ProcPowerArray[i];

  MeanProcPower /= (double)ProcCount;

  /* Change processor performance weigth array */    /*E0084*/

  if(ProcWeightSign == 0)
  {  for(i=0; i < ProcCount; i++)
         ProcWeightArray[i] = 1. / ProcPowerArray[i];

     /* */    /*E0085*/
       
     MinWP = 1.e7;  /* */    /*E0086*/

     for(i=0; i < ProcCount; i++)
     {  MinWP = (double)(dvm_min(MinWP, ProcWeightArray[i]));
     }

     for(i=0; i < ProcCount; i++)
         ProcWeightArray[i] /= MinWP;
  }

  return;
}


/********************************************\
*  Measuring of Tm, Tirecv and Twrecv times *
*  for pipeline ACROSS scheme execution     *
\********************************************/    /*E0087*/

void  GetAcrossTimes(void)
{ int             i;
  double          TimeMes[5];
  double         *Mes;
  double          time;
  RTL_Request    *Req;
  RTL_Request     RecvReq, SendReq;
  double        **TMes3 = NULL;

  if(ProcCount == 1 || AcrossGroupNumber != 0)
  {  if(RTL_TRACE && AcrossTrace)
        tprintf("Tm=%lf  Tirecv=%lf  Twrecv=%lf\n", Tm, Tirecv, Twrecv);
     return;
  }

  dvm_AllocArray(double, TimeMesLen, Mes);

  if(MPS_CurrentProc)
  {  /* Internal current processor number is not equal to 0 */    /*E0088*/

     /* Initialization of measuring message receiving 
        and get Tirecv time */    /*E0089*/

     time = dvm_time();
     ( RTL_CALL, rtl_Recvnowait(Mes, TimeMesLen,
                                sizeof(double), MPS_CurrentProc-1,
                                msg_ProcPowerMeasure, &RecvReq, 0) );
     Tirecv = dvm_time() - time + 0.000001;

     /* Send message that permit measuring message sending */    /*E0090*/

     ( RTL_CALL, rtl_Sendnowait(TimeMes, 1, sizeof(int),
                                MPS_CurrentProc-1, msg_ProcPowerMeasure,
                                &SendReq, 0) );

     ( RTL_CALL, rtl_Waitrequest(&SendReq) );

     /* Measuring message receiving ang get Tm and Twrecv times */    /*E0091*/

     ( RTL_CALL, rtl_Waitrequest(&RecvReq) );

     time = dvm_time();
     Tm = time - Mes[0] + 0.000001;

     if(RTL_TRACE && AcrossTrace)
        tprintf("time=%lf  Mes[0]=%lf  Tm=%lf\n", time, Mes[0], Tm);

     Tm /= TimeMesLen * sizeof(double);

     if(Tm <= 0.)
        Tm = 0.000000000001;

     if(RTL_TRACE && AcrossTrace)
        tprintf("Tm=%lf\n", Tm);

     time = dvm_time();
     ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
     Twrecv = dvm_time() - time + 0.000001;
  }

  if(MPS_CurrentProc != ProcCount-1)
  {  /* the current processor is not last one */    /*E0092*/

     /* Receive message that permit measuring message sending 
        and send the measuring message*/    /*E0093*/

     ( RTL_CALL, rtl_Recvnowait(TimeMes, 1, sizeof(int),
                                MPS_CurrentProc+1, msg_ProcPowerMeasure,
                                &RecvReq, 0) );

     ( RTL_CALL, rtl_Waitrequest(&RecvReq) );

     Mes[0] = dvm_time();

     ( RTL_CALL, rtl_Sendnowait(Mes, TimeMesLen,
                                sizeof(double), MPS_CurrentProc+1,
                                msg_ProcPowerMeasure, &SendReq, 0) );
     ( RTL_CALL, rtl_Waitrequest(&SendReq) );
  }

  dvm_FreeArray(Mes);

  /* Get final Tm, Tirecv and Twrecv times */    /*E0094*/

  if(MPS_CurrentProc == MPS_CentralProc)
  {  /* the current processor is central */    /*E0095*/

     /* Receive times from all non-central processors */    /*E0096*/

     dvm_AllocArray(RTL_Request, ProcCount, Req);
     dvm_AllocArray(double *, ProcCount, TMes3);

     for(i=0; i < ProcCount; i++)
     {  dvm_AllocArray(double, 3, TMes3[i]);
     }

     TMes3[MPS_CurrentProc][0] = Tm;
     TMes3[MPS_CurrentProc][1] = Tirecv;
     TMes3[MPS_CurrentProc][2] = Twrecv;

     for(i=1; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Recvnowait(TMes3[i], 3,
                                   sizeof(double), i,
                                   msg_ProcPowerMeasure, &Req[i], 0) );
     }

     for(i=1; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Waitrequest(&Req[i]) );
     }

     /* Calculate average times and send them to all non-central processors */    /*E0097*/

     Tm = 0.;
     Tirecv = 0.;
     Twrecv = 0.;

     for(i=1; i < ProcCount; i++)
     {  Tm += TMes3[i][0];
        Tirecv += TMes3[i][1];
        Twrecv += TMes3[i][2];
     }

     Tm /= ProcCount - 1;
     Tirecv /= ProcCount - 1;
     Twrecv /= ProcCount - 1;

     TimeMes[0] = Tm;
     TimeMes[1] = Tirecv;
     TimeMes[2] = Twrecv;

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Sendnowait(TimeMes, 3,
                                   sizeof(double), i,
                                   msg_ProcPowerMeasure, &Req[i], 0) );
     }

     for(i=0; i < ProcCount; i++)
     {  if(i == MPS_CurrentProc)
           continue;

        ( RTL_CALL, rtl_Waitrequest(&Req[i]) );
     }

     for(i=0; i < ProcCount; i++)
     {  dvm_FreeArray(TMes3[i]);
     }

     dvm_FreeArray(Req);
     dvm_FreeArray(TMes3);
  }
  else
  {  /* the current processor is not central */    /*E0098*/

     if(MPS_CurrentProc)
     {  /* current processor number is not equal to 0 */    /*E0099*/

        TimeMes[0] = Tm;
        TimeMes[1] = Tirecv;
        TimeMes[2] = Twrecv;

        ( RTL_CALL, rtl_Sendnowait(TimeMes, 3, sizeof(double),
                                   MPS_CentralProc,
                                   msg_ProcPowerMeasure, &SendReq, 0) );
        ( RTL_CALL, rtl_Waitrequest(&SendReq) );
     }
     
     ( RTL_CALL, rtl_Recvnowait(TimeMes, 3, sizeof(double),
                                MPS_CentralProc,
                                msg_ProcPowerMeasure, &RecvReq, 0) );
     ( RTL_CALL, rtl_Waitrequest(&RecvReq) );

     Tm = TimeMes[0];
     Tirecv = TimeMes[1];
     Twrecv = TimeMes[2];
  }

  if(RTL_TRACE && AcrossTrace)
     tprintf("Tm=%lf  Tirecv=%lf  Twrecv=%lf\n", Tm, Tirecv, Twrecv);

  return;
}


/*******************************\
* Initialization of  statistics *
\*******************************/    /*E0100*/

#ifndef _DVM_IOPROC_

void  StatInit(void)
{
  AMRef      amref;
  PSRef      psref;
  AMViewRef  mvref;
  DvmType       VMRank, ExtHdrSign = 0, GenBlock = 0;
  int        i;
  DvmType       Static = 1, ReDistr = 0, TypeSize;
  DvmType       AMDim[MAXARRAYDIM], Axis[MAXARRAYDIM], All0[MAXARRAYDIM],
             All1[MAXARRAYDIM], InitIndex[MAXARRAYDIM],
             LastIndex[MAXARRAYDIM];
  double     CoordWeightArray[1] = {-1.}; 

  if(Is_DVM_STAT && StatBufLength)
  {
     amref = ( RTL_CALL, getam_() );
     psref = ( RTL_CALL, getps_(&amref) );
     VMRank = ( RTL_CALL, getrnk_(&psref) );

     ( RTL_CALL, setpsw_(&psref, NULL, CoordWeightArray,
                         NULL) ); /* Set processor weights  1 
                                          and coordinates */    /*E0101*/
     for(i=0; i < VMRank; i++)
     { 
        AMDim[i] = MPS_VMS->Space.Size[i];

        Axis[i] = i+1;
        All0[i] = 0;
        All1[i] = 1;
     }

     AMDim[VMRank-1] *= StatBufLength;

     mvref = ( RTL_CALL, crtamv_(&amref, &VMRank, AMDim, &Static) );
     ( RTL_CALL, distr_(&mvref, &psref, &VMRank, Axis, All0) );

     TypeSize = sizeof(char);

     for(i=0; i < 2*MAXARRAYDIM+2; i++)
         StatArrHeader[i] = 1;

     ( RTL_CALL, crtda_(StatArrHeader, &ExtHdrSign, NULL, &VMRank, &TypeSize, AMDim,
                        &Static, &ReDistr, All0, All0) );
     ( RTL_CALL, align_(StatArrHeader, &mvref, Axis, All1, All0) );
     ( RTL_CALL, locind_(StatArrHeader, InitIndex, LastIndex) );

     StatBufPtr = ( RTL_CALL, GetLocElmAddr(StatArrHeader, InitIndex) );

     /* Restore processor coordinate weights 
     and array of summary previous processor coordinate weights 
     for all array dimensions */    /*E0102*/

     ( RTL_CALL, setpsw_(NULL, NULL, CoordWeightList, NULL) );

     /* */    /*E0103*/

     #ifdef _DVM_ZLIB_

     if(StatCompressLevel >= 0)
     {
        if(StatCompressScheme == 0)
        {
           switch(StatCompressLevel)
           {
              case 1:  SYSTEM(strcpy, (StatOpenReg, "wb1"))
                       break;
              case 2:  SYSTEM(strcpy, (StatOpenReg, "wb2"))
                       break;
              case 3:  SYSTEM(strcpy, (StatOpenReg, "wb3"))
                       break;
              case 4:  SYSTEM(strcpy, (StatOpenReg, "wb4"))
                       break;
              case 5:  SYSTEM(strcpy, (StatOpenReg, "wb5"))
                       break;
              case 6:  SYSTEM(strcpy, (StatOpenReg, "wb6"))
                       break;
              case 7:  SYSTEM(strcpy, (StatOpenReg, "wb7"))
                       break;
              case 8:  SYSTEM(strcpy, (StatOpenReg, "wb8"))
                       break;
              case 9:  SYSTEM(strcpy, (StatOpenReg, "wb9"))
                       break;
              case 0:
              default: SYSTEM(strcpy, (StatOpenReg, "wb0"))
                       break;
           }
        }
        else
        {
           SYSTEM(strcpy, (StatOpenReg, "wb"))
           SYSTEM(strcat, (StatFileName, ".gz+"))
        }
     }

     #endif

     /* */    /*E0104*/

     MISize  = (uLLng)&TaskInter[StatGrpCountM1][StatGrpCount] -
               (uLLng)&TaskInter[0][0];
     MISize += 3*sizeof(double) + sizeof(s_SendRecvTimes);

     /* ----------------------------------------------- */    /*E0105*/

     RTL_STAT = 1;  /* statistics on */    /*E0106*/

     stat_init();   /* substantial statistics installation */    /*E0107*/
  }

  return;
}

#endif


/******************************\
* Print support system version * 
\******************************/    /*E0108*/

void PrintVers(byte Reg, byte Screen)
{
  if(Screen && IAmIOProcess == 0)
  { if(CurrentProcIdent == MasterProcIdent)
       rtl_iprintf("\nRTS VERSION = %.4d\n\n", DVM_VERS);
  }
  else
    tprintf("\nRTS VERSION = %.4d\n\n", DVM_VERS);

  if(Reg)
  {  /* detailed print mode */    /*E0109*/

     if(Screen && IAmIOProcess == 0)
     { if(CurrentProcIdent == MasterProcIdent)
       {
         rtl_iprintf("syspar   file version = %.4d (min vers = %.4d)\n",
                     SYSTEM_VERS, SYSTEM_VERS_MIN);
         rtl_iprintf("sysdebug file version = %.4d (min vers = %.4d)\n",
                     DEBUG_VERS, DEBUG_VERS_MIN);
         rtl_iprintf("systrace file version = %.4d (min vers = %.4d)\n",
                     SYSTRACE_VERS, SYSTRACE_VERS_MIN);
         rtl_iprintf("trcevent file version = %.4d (min vers = %.4d)\n",
                     TRCEVENT_VERS, TRCEVENT_VERS_MIN);
         rtl_iprintf("usrdebug file version = %.4d (min vers = %.4d)\n",
                     CMPTRACE_VERS, CMPTRACE_VERS_MIN);
         rtl_iprintf("statist  file version = %.4d "
                     "(min vers = %.4d)\n\n",
                     STATIST_VERS, STATIST_VERS_MIN);
       }
     }
     else
     {
        if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
        {  tprintf("syspar   file version = %.4d (min vers = %.4d)\n",
                   SYSTEM_VERS, SYSTEM_VERS_MIN);
           tprintf("sysdebug file version = %.4d (min vers = %.4d)\n",
                   DEBUG_VERS, DEBUG_VERS_MIN);
           tprintf("systrace file version = %.4d (min vers = %.4d)\n",
                   SYSTRACE_VERS, SYSTRACE_VERS_MIN);
           tprintf("trcevent file version = %.4d (min vers = %.4d)\n",
                   TRCEVENT_VERS, TRCEVENT_VERS_MIN);
           tprintf("usrdebug file version = %.4d (min vers = %.4d)\n",
                   CMPTRACE_VERS, CMPTRACE_VERS_MIN);
           tprintf("statist  file version = %.4d (min vers = %.4d)\n\n",
                   STATIST_VERS, STATIST_VERS_MIN);
        }
     }
  }

  return;
}


/*****************************\
* Support system  termination *  
\*****************************/    /*E0110*/

DvmType  __callstd lexit_(DvmType *UserResPtr)
/*
*UserResPtr - value returned by user program. 

The function completes correctly the execution of Run-Time Library.
The function does not return control.
*/    /*E0111*/
{
  int  flag = 0;

  RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

  if(1 /*CallDbgCond*/    /*E0112*/ /*EnableTrace && dvm_OneProcSign*/    /*E0113*/)
  {
/*
     PMPI_Initialized(&flag);

     if(flag == 0)
        return  *UserResPtr;
*/    /*E0114*/

     SYSTEM(MPI_Finalize, ())
  }
  else
  {
     dvm_exit((int)*UserResPtr);
  }

#else

   dvm_exit((int)*UserResPtr);

#endif

  SYSTEM(exit, ((int)*UserResPtr))
  return *UserResPtr;
}
  

/**************************************\
* System termination of support system *  
\**************************************/    /*E0115*/

void dvm_exit(int rc)
{
  int            i, j, j1, k, k1, n, m, l, StopLine;
  int            DumpMsgArr[2+sizeof(double)/sizeof(int)];
  byte           trace_Dump_res;
  word           TrcBufCountArr[2+sizeof(double)/sizeof(word)];
  s_AMVIEW      *AMV;
  s_AMS         *AMS;
  double        *SubTasksTimeArray, *GSubTasksTimeArray, *DoublePtr;
  int           *SubTasksLineArray, *GSubTasksLineArray, *IntPtr;
  double         MinTime, MaxTime, TotalTime, CurrTime;
  RTL_Request   *Req;
  RTL_Request    OutReq;
  s_VMS         *VMS;

/*printf("dvm_OneProcNum=%d\n", dvm_OneProcNum);*/    /*E0116*/

  #ifdef _STRUCT_STAT_
     struct stat  FileInfo;
  #endif

  int          InfoFileSize;
  char        *InfoFileBuf;

#ifndef _DVM_IOPROC_
  DVMFILE     *StatFile;
  AMRef        amref;
  PSRef        psref;
#endif

  double       CoordWeightArray[1] = {-1.};
  DvmType         FileSize = 10000, BufSize, GenBlock = 0;
  FILE        *ResSysInfo;

#ifdef _DVM_MPI_  
  MPS_Request    IOReq;
  MPS_Request   *IOReqPtr;
#endif
     
  sysprintf(__FILE__,__LINE__, "call dvm_exit\n");

  RTS_Call_MPI = 1;  /* */    /*E0117*/

  if(ExitFlag == 0)   
  {
     if(UserSumFlag)
     {
        if(DVMExitSynchr && DVM_ProcCount > 1)
           (RTL_CALL, bsynch_()); /* */    /*E0118*/

        UserSumFlag = 0;
        UserFinishTime = dvm_time();
     }

     /* */    /*E0119*/

     #ifdef _DVM_MPI_

     if(IsInit && IAmIOProcess == 0 && IOProcessCount != 0)
     {
        iom_Sendnowait1(MyIOProc, (void *)&IAmIOProcess, sizeof(int),
                        &IOReq, msg_IOInit);
        iom_Waitrequest1(&IOReq);

        iom_Recvnowait1(MyIOProc, (void *)&ServerProcTime,
                        sizeof(double), &IOReq, msg_IOInit);
        iom_Waitrequest1(&IOReq);

        dvm_AllocArray(MPS_Request, IOProcessCount, IOReqPtr);

        for(i=0; i < IOProcessCount; i++)
            if(i != MyIOProc)
               iom_Sendnowait1(i, (void *)&IAmIOProcess, sizeof(int),
                               &IOReqPtr[i], msg_IOInit);

        for(i=0; i < IOProcessCount; i++)
            if(i != MyIOProc)
               iom_Waitrequest1(&IOReqPtr[i]);

        dvm_FreeArray(IOReqPtr);
     }

     #endif

     MPI_BotsulaProf = 0;  /* */    /*E0120*/
     MPI_BotsulaDeb  = 0;  /* */    /*E0121*/

     ExitFlag = 1;

     if(RTL_TRACE)
        dvm_trace(Event_dvm_exit,"rc=%d;\n", rc);

     if(IAmIOProcess == 0)
        dvm_void_printf(" \n"); /* line feed for Fortran */    /*E0122*/

     /* */    /*E0123*/

     if(_SysInfoPrint && SubTasksTimePrint)
     {
        /* */    /*E0124*/

        for(i=0; i < InitAMS.SubSystem.Count; i++)
        {
           AMV = coll_At(s_AMVIEW *, &InitAMS.SubSystem, i);

           if(AMV->IsGetAMR == 0)
              continue;  /* */    /*E0125*/

           k  = (int)(AMV->AMSColl.Count * sizeof(double));
           k1 = (int)(AMV->AMSColl.Count * sizeof(int));

           mac_malloc(SubTasksTimeArray, double *, k, 0);
           mac_malloc(SubTasksLineArray, int *, k1, 0);

           /* */    /*E0126*/    

           for(j=0; j < AMV->AMSColl.Count; j++)
           {
              AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->IsMapAM == 0)
              {
                 /* */    /*E0127*/

                 SubTasksTimeArray[j] = 0.;
                 SubTasksLineArray[j] = 0;
              }
              else
              {
                 /* */    /*E0128*/

                 SubTasksTimeArray[j] = AMS->ExecTime;
                 SubTasksLineArray[j] = (int)AMS->stop_DVM_LINE;
              }
           }

           if(MPS_CurrentProc == MPS_IOProc)
           {  
              j  = (int)(k * ProcCount);
              j1 = (int)(k1 * ProcCount);

              mac_malloc(GSubTasksTimeArray, double *, j, 0);
              mac_malloc(GSubTasksLineArray, int *, j1, 0);

              DoublePtr = GSubTasksTimeArray +
                          MPS_IOProc * AMV->AMSColl.Count;
              IntPtr    = GSubTasksLineArray +
                          MPS_IOProc * AMV->AMSColl.Count;

              for(j=0; j < AMV->AMSColl.Count; j++)
              {
                 DoublePtr[j] = SubTasksTimeArray[j];
                 IntPtr[j] = SubTasksLineArray[j];
              }

              mac_free((void **)&SubTasksTimeArray);
              mac_free((void **)&SubTasksLineArray);

              dvm_AllocArray(RTL_Request, ProcCount, Req);

              for(j=0; j < ProcCount; j++)
              {
                 if(j == MPS_IOProc)
                    continue;

                 DoublePtr = GSubTasksTimeArray + j * AMV->AMSColl.Count;

                 ( RTL_CALL, rtl_Recvnowait((void *)DoublePtr, 1, k, j,
                                            msg_common, &Req[j], 0) );
              }

              for(j=0; j < ProcCount; j++)
              {
                 if(j == MPS_IOProc)
                    continue;

                 ( RTL_CALL, rtl_Waitrequest(&Req[j]) );
              }

              for(j=0; j < ProcCount; j++)
              {
                 if(j == MPS_IOProc)
                    continue;

                 IntPtr = GSubTasksLineArray + j * AMV->AMSColl.Count;

                 ( RTL_CALL, rtl_Recvnowait((void *)IntPtr, 1, k1, j,
                                            msg_common, &Req[j], 0) );
              }

              for(j=0; j < ProcCount; j++)
              {
                 if(j == MPS_IOProc)
                    continue;

                 ( RTL_CALL, rtl_Waitrequest(&Req[j]) );
              }

              dvm_FreeArray(Req);

              /* */    /*E0129*/

              rtl_iprintf("*** SubTasksTime. AMViewCrt: FILE=%s "
                          "LINE=%ld\n",
                          AMV->DVM_FILE, AMV->DVM_LINE);

              for(j=0; j < AMV->AMSColl.Count; j++)
              {
                 AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS->IsMapAM == 0)
                    continue;   /* */    /*E0130*/

                 /* */    /*E0131*/

                 VMS = AMS->VMS;

                 n = -1;
                 m = -1;
                 MinTime = 1.e10;
                 MaxTime = 0.;
                 TotalTime = 0.;

                 for(k=0; k < VMS->ProcCount; k++)
                 {
                    l = (int)VMS->VProc[k].lP;  /* */    /*E0132*/
                    DoublePtr = GSubTasksTimeArray +
                                l * AMV->AMSColl.Count;
                    IntPtr = GSubTasksLineArray +
                             l * AMV->AMSColl.Count;
                    CurrTime = DoublePtr[j];
                    StopLine = IntPtr[j];

                    if(CurrTime < MinTime)
                    {
                       MinTime = CurrTime;
                       n = l;
                    }

                    if(CurrTime > MaxTime)
                    {
                       MaxTime = CurrTime;
                       m = l;
                    }

                    TotalTime += CurrTime;
                 }

                 if(VMS->ProcCount == 1)
                 {
                    /* */    /*E0133*/

                    rtl_iprintf("    Task %d. FILE=%s LINE: map=%ld "
                                "run=%ld stop=%d\n         "
                                "ProcCount=%ld "
                                "Proc=%ld Time=%lf\n",
                                j, AMS->map_DVM_FILE, AMS->map_DVM_LINE,
                                AMS->run_DVM_LINE, StopLine,
                                VMS->ProcCount, VMS->VProc[0].lP,
                                TotalTime);
                 }
                 else
                 {
                    /* */    /*E0134*/

                    rtl_iprintf("    Task %d. FILE=%s LINE: map=%ld "
                                "run=%ld stop=%d\n         "
                                "ProcCount=%ld "
                                "MinTime=%lf(P=%d) MaxTime=%lf(P=%d) "
                                "MeanTime=%lf\n",
                                j, AMS->map_DVM_FILE, AMS->map_DVM_LINE,
                                AMS->run_DVM_LINE, StopLine,
                                VMS->ProcCount, MinTime, n, MaxTime, m,
                                (double)(TotalTime/VMS->ProcCount));

                    if(SubTasksTimePrint > 1 && VMS->ProcCount > 2)
                    {
                       /* */    /*E0135*/

                       for(k=0; k < VMS->ProcCount; k++)
                       {
                          l = (int)VMS->VProc[k].lP;  /* */    /*E0136*/
                          DoublePtr = GSubTasksTimeArray +
                                      l * AMV->AMSColl.Count;
                          CurrTime = DoublePtr[j];

                          rtl_iprintf("         Proc=%d Time=%lf\n",
                                      l, CurrTime);
                       }
                    }
                 }
              }

              mac_free((void **)&GSubTasksTimeArray);
              mac_free((void **)&GSubTasksLineArray);
           }
           else
           {
              ( RTL_CALL, rtl_Sendnowait((void *)SubTasksTimeArray, 1, k,
                                         MPS_IOProc, msg_common,
                                         &OutReq, 0) );
              ( RTL_CALL, rtl_Waitrequest(&OutReq) );
              mac_free((void **)&SubTasksTimeArray);

              ( RTL_CALL, rtl_Sendnowait((void *)SubTasksLineArray, 1,
                                         k1, MPS_IOProc, msg_common,
                                         &OutReq, 0) );
              ( RTL_CALL, rtl_Waitrequest(&OutReq) );
              mac_free((void **)&SubTasksLineArray);
           }
        }
     } 

     /* -------------------------------- */    /*E0137*/

     #ifdef  _DVM_MPI_

     if(MPIInfoPrint)
     {
        if(MPIInfoPrint > 1)
        {
            pprintf(1, "*** RTS: ByteCount=%ld(%ld) "
                      "without compress=%ld(%ld)\n",
                      MPISendByteCount,
                      MPISendByteCount + MPISendByteCount0,
                      MPISendByteNumber,
                      MPISendByteNumber + MPISendByteNumber0);
            pprintf(1, "*** RTS: MsgCount=%ld(%ld) "
                      "with compress=%ld(%ld)\n",
                      MPIMsgCount, MPIMsgCount + MPIMsgCount0,
                      MPIMsgNumber, MPIMsgNumber);

           if(MinMPIMsgLen < 0 || MinMPIMsgLen == DVMTYPE_MAX)
              MinMPIMsgLen = 0;

           if(MinMPIMsgLen0 < 0 || MinMPIMsgLen0 == DVMTYPE_MAX)
              MinMPIMsgLen0 = 0;
 
           pprintf(1, "*** RTS: MaxMsgLen=%ld(%ld) "
                      "MinMsgLen=%ld(%ld)\n",
                      MaxMPIMsgLen, MaxMPIMsgLen + MaxMPIMsgLen0,
                      MinMPIMsgLen, MinMPIMsgLen + MinMPIMsgLen0);
           pprintf(1, "*** RTS: compress_malloc:\n"
                      "msg0=%ld msg1=%ld msg2=%ld msg3=%ld msg4=%ld\n",
                      dvm_compress0, dvm_compress1, dvm_compress2,
                      dvm_compress3, dvm_compress4);
           pprintf(1, "*** RTS: AllreduceTime=%lf AlltoallvTime=%lf BcastTime=%lf BarrierTime=%lf GatherTime=%lf\n",
                      MPI_AllreduceTime, MPI_AlltoallvTime, MPI_BcastTime, MPI_BarrierTime, MPI_GatherTime);

           if(CurrentProcIdent == MasterProcIdent)
              pprintf(1, "*** RTS: sizeof(MPI_Comm)=%d sizeof(int)=%d "
                         "sizeof(DvmType)=%d\n"
                         "sizeof(float)=%d sizeof(double)=%d "
                         "CLOCKS_PER_SEC=%d InversByteOrder=%d\n"
                         "MPI_MAX_PROCESSOR_NAME=%d\n"
                         "INT_MAX=%d LONG_MAX="
                         DTFMT
                         "\n", 
                         sizeof(MPI_Comm), sizeof(int), sizeof(DvmType),
                         sizeof(float), sizeof(double), CLOCKS_PER_SEC,
                         (int)InversByteOrder, MPI_MAX_PROCESSOR_NAME,
                         INT_MAX, DVMTYPE_MAX);
        }
        else
        {
           if(CurrentProcIdent == MasterProcIdent)
           {
              rtl_iprintf("*** RTS: ByteCount=%ld(%ld) "
                          "without compress=%ld(%ld)\n",
                          MPISendByteCount,
                          MPISendByteCount + MPISendByteCount0,
                          MPISendByteNumber,
                          MPISendByteNumber + MPISendByteNumber0);
              rtl_iprintf("*** RTS: MsgCount=%ld(%ld) "
                          "with compress=%ld(%ld)\n",
                          MPIMsgCount, MPIMsgCount + MPIMsgCount0,
                          MPIMsgNumber, MPIMsgNumber);

              if(MinMPIMsgLen < 0 || MinMPIMsgLen == DVMTYPE_MAX)
                 MinMPIMsgLen = 0;

              if(MinMPIMsgLen0 < 0 || MinMPIMsgLen0 == DVMTYPE_MAX)
                 MinMPIMsgLen0 = 0;

              rtl_iprintf("*** RTS: MaxMsgLen=%ld(%ld) "
                          "MinMsgLen=%ld(%ld)\n",
                          MaxMPIMsgLen, MaxMPIMsgLen + MaxMPIMsgLen0,
                          MinMPIMsgLen, MinMPIMsgLen + MinMPIMsgLen0);
              rtl_iprintf("*** RTS: compress_malloc:\n"
                          "msg0=%ld msg1=%ld msg2=%ld msg3=%ld "
                          "msg4=%ld\n",
                          dvm_compress0, dvm_compress1, dvm_compress2,
                          dvm_compress3, dvm_compress4);
              rtl_iprintf("*** RTS: AllreduceTime=%lf "
                          "AlltoallvTime=%lf "
                          "BcastTime=%lf "
                          "BarrierTime=%lf "
                          "GatherTime=%lf\n",
                          MPI_AllreduceTime, MPI_AlltoallvTime, MPI_BcastTime, MPI_BarrierTime, MPI_GatherTime);

              rtl_iprintf("*** RTS: sizeof(MPI_Comm)=%d sizeof(int)=%d "
                          "sizeof(DvmType)=%d\n"
                          "sizeof(float)=%d sizeof(double)=%d "
                          "CLOCKS_PER_SEC=%d InversByteOrder=%d\n"
                          "MPI_MAX_PROCESSOR_NAME=%d\n"
                          "INT_MAX=%d LONG_MAX="
                          DTFMT
                          "\n",
                          sizeof(MPI_Comm), sizeof(int), sizeof(DvmType),
                          sizeof(float), sizeof(double), CLOCKS_PER_SEC,
                          (int)InversByteOrder, MPI_MAX_PROCESSOR_NAME,
                          INT_MAX, DVMTYPE_MAX);
           }
        }
     }

     #endif

/*
#if defined(_DVM_ZLIB_) && defined(_WIN_MPI_)

{
double buf[10000];
Bytef   gzBuf[sizeof(double)*20000];
uLongf   gzLen;
int      MsgCompressLev, m, n, CompressLen;
byte    *CharPtr1, *CharPtr2, *CharPtr3;

k = 10000;

for(i=0; i < k; i++)
    buf[i] = 1.7E-30;


for(i=0,j=0; i < k; i++,j++)
{
    CharPtr1 = (byte *)&buf[i];
    CharPtr2 = (byte *)&i;

    CharPtr1[0] = CharPtr2[0];
    CharPtr1[1] = CharPtr2[1];
}

MsgCompressLev = MsgCompressLevel;

if(MsgCompressLev == 0)
{
   if(CompressLevel == 0)
      MsgCompressLev = Z_DEFAULT_COMPRESSION;
   else
      MsgCompressLev = CompressLevel;
}

gzLen = (uLongf)((sizeof(double)*k) + (sizeof(double)*k)/50 + 12);

SYSTEM_RET(i, compress2, (gzBuf, &gzLen, (Bytef *)buf,
                          (uLong)(sizeof(double)*k), MsgCompressLev))

tprintf("++++++ MsgCompress2: rc=%d; MsgLen=%d; "
                      "gzLen=%ld; Level=%d;\n",
                      i, (sizeof(double)*k), (DvmType)gzLen, MsgCompressLev);



     if(MsgDVMCompress != 0)
     {
        j = k;

        k = 1;
        CompressLen = 1 + sizeof(double);

        if(MsgDVMCompress < 2)
        {
           n = sizeof(double) - 1;

           if(InversByteOrder)
              CharPtr1 = (byte *)buf + 1;
           else
              CharPtr1 = (byte *)buf;
        }
        else
        {
           n = sizeof(double) - 2;

           if(InversByteOrder)
              CharPtr1 = (byte *)buf + 2;
           else
              CharPtr1 = (byte *)buf;
        }

        CharPtr3 = CharPtr1;
        CharPtr1 += sizeof(double);

        for(i=1; i < j; i++)
        {
           for(m=0; m < n; m++)
               if(CharPtr1[m] != CharPtr3[m])
                  break;

           if(m == n && k < 255)
           {
              if(MsgDVMCompress > 1)
                 CompressLen += 2;
              else
                 CompressLen++;

              CharPtr1 += sizeof(double);
              k++;
           }
           else
           {
              k = 1;

              CharPtr3 = CharPtr1;
              CharPtr1 += sizeof(double);
              CompressLen += 1 + sizeof(double);
           }
        }

        CompressLen += SizeDelta[CompressLen & Msk3];



tprintf("++++++ DVMCompress: MsgLen=%d; "
                      "CompressLen=%d;\n",
                      (sizeof(double)*k), CompressLen);

     }
}

#endif
*/    /*E0138*/



     /* Dump standard streams into files */    /*E0139*/

     if(IAmIOProcess == 0)
     {
        SYSTEM(fflush,(stdout))
        SYSTEM(fflush,(stderr))

        #ifdef _UNIX_
           SYSTEM(sync, ())
        #endif
     }

     if(IsDVMInit)
     { 
        /* Return context to the initial subproblem
               and free non static DVM-objects      */    /*E0140*/

        ( RTL_CALL, dvm_Done() );

        /* User task times into current interval */    /*E0141*/

        if(DVMCallLevel == 0 && IsExpend)
        {
           Double1 = (dvm_time() - StatGrpStack[0].dvm_time) *
                     d1_CurrEnvProcCount;
           Double2 = CurrEnvProcCount_m1 * Double1;

           if(TimeExpendPrint > 1 && _SysInfoPrint)
           {
              TaskInter[UserGrp][UserGrp].ProductTime +=
              Double1 + StatGrpStack[0].ProductTime; 
              TaskInter[UserGrp][UserGrp].LostTime +=
              Double2 + StatGrpStack[0].LostTime;
           }
           if(RTL_STAT)
           {
              CurrInterPtr[UserGrp][UserGrp].ProductTime +=
              Double1 + StatGrpStack[0].ProductTime; 
              CurrInterPtr[UserGrp][UserGrp].LostTime +=
              Double2 + StatGrpStack[0].LostTime;
           }
        }

        /* --------------------------------------------------- */    /*E0142*/

        #ifndef _DVM_IOPROC_

        if(RTL_STAT && StatBufLength)
        {
           stat_done();  /* */    /*E0143*/

           /* Dump statistics in file */    /*E0144*/
       
           /* Set processor weights 1
               and their coordinates  */    /*E0145*/

           amref = ( RTL_CALL, getam_() );
           psref = ( RTL_CALL, getps_(&amref) );

           ( RTL_CALL, setpsw_(&psref, NULL, CoordWeightArray,
                               NULL) );

           i = CompressLevel;  /* */    /*E0146*/

           if(CompressLevel == -1)
              CompressLevel = 0; /* */    /*E0147*/

           StatFile = ( RTL_CALL, dvm_fopen(StatFileName,StatOpenReg) );

           CompressLevel = i;  /* */    /*E0148*/

           if(StatFile)
           {
              if(WriteStatByFwrite)
                 ( RTL_CALL, dvm_dfwrite(StatArrHeader, 0, StatFile) );
              else
                 WriteStatToFile(StatFile);
 
              ( RTL_CALL, dvm_fclose(StatFile) );

              /* */    /*E0149*/

              #ifdef _DVM_ZLIB_

              if(StatCompressLevel >= 0 && StatCompressScheme < 0 &&
                 MPS_CurrentProc == MPS_IOProc)
              {
                 char   *buf, *buf1, *CharPtr;
                 int     stblen, proc_number, len, len1;
                 uLongf  gzlen;

                 StatFile->File = fopen(StatFileName, "rb");

                 if(StatFile->File)
                 {
                    mac_malloc(buf, char *, 200000, 0);
                    mac_malloc(buf1, char *, 200000, 0);

                    CharPtr = buf;

                    i = fread(CharPtr, 1, 200000, StatFile->File);

                    stblen = atoi(CharPtr);
                    CharPtr += strlen(CharPtr)+1; /* */    /*E0150*/
                    printf("stblen=%d\n", stblen);

                    proc_number = atoi(CharPtr);
                    CharPtr += strlen(CharPtr)+1; /* */    /*E0151*/

                    for(i=0; i < proc_number; i++)
                    {
                       len = atoi(CharPtr);  /* */    /*E0152*/

                       CharPtr += strlen(CharPtr)+1; /* */    /*E0153*/
                       gzlen = 200000;

                       SYSTEM_RET(len1, uncompress,
                                  ((Bytef *)buf1, &gzlen,
                                   (Bytef *)CharPtr, (uLong)len));

                       len1 = (int)gzlen; /* */    /*E0154*/

                       buf1[len1] = '\x00';
                       printf("buf1=%s", buf1);

                       CharPtr += len;
                    }

                    mac_free(&buf);
                    mac_free(&buf1);
                 }
              }

              #endif 
           }

           ( RTL_CALL, delda_(StatArrHeader) );
        }

        if(IAmIOProcess == 0)
        {
           dyn_Done();  /* termination of 
                        dynamic control tools */    /*E0155*/
           cmptrace_Done();
           cntx_Done();
        }

        #endif
     }

     /* */    /*E0156*/

     if(DelCurrentPar == 2)
     {
        if(MPS_CurrentProc == MPS_MasterProc && IAmIOProcess == 0)
           SYSTEM(remove, (CurrentParName))
     }

     /* */    /*E0157*/

     if(MPS_CurrentProc == MPS_IOProc && IAmIOProcess == 0)
     {
        for(i=0,j=0; j < 3; i++)
        {
           SYSTEM(sprintf, (DVM_String, "%d", i))
           SYSTEM(strcat, (DVM_String, "scan.dvm"))

           SYSTEM_RET(k, remove, (DVM_String))

           if(k != 0)
              j++;       /* */    /*E0158*/
           else
              j = 0;
        }
     }

     /* Print final results, close trace file,
                dump trace buffer             */    /*E0159*/

     if(MPS_CurrentProc == MPS_MasterProc)
     {
        trace_Done();        /* print final results and
                                   close trace file */    /*E0160*/

        for(i=1; i < ProcCount; i++)
        {
           ( RTL_CALL, rtl_Send(DumpMsgArr, 1, sizeof(int), i) );
           ( RTL_CALL, rtl_Recv(DumpMsgArr, 1, sizeof(int), i) );
        }
     }
     else
     {
        ( RTL_CALL, rtl_Recv(DumpMsgArr, 1, sizeof(int),
                             MPS_MasterProc) );
        trace_Done();        /* print final results and
                                   close trace file */    /*E0161*/
        ( RTL_CALL, rtl_Send(DumpMsgArr, 1, sizeof(int),
                             MPS_MasterProc) );
     }


     if(IsTraceInit)
     {
        if(MPS_CurrentProc == MPS_MasterProc)
        {
           /* Dump trace buffer */    /*E0162*/

           trace_Dump_res =
           trace_Dump(MPS_CurrentProc, TraceBufCountArr[0],
                                       TraceBufFullArr[0]);

           if(BufferTraceUnLoad     &&
              BufferTrace           &&
              TraceBufLength        &&
              TraceBufPtr!=NULL     &&
              IsSlaveRun               )
           {
              for(i=1; i < ProcCount; i++)
              {
                if(TraceProcNumber[i] == 0)
                   continue;

                DumpMsgArr[0] = trace_Dump_res;
                ( RTL_CALL, rtl_Send(DumpMsgArr, 1, sizeof(int), i) );
                ( RTL_CALL, rtl_Recv(TrcBufCountArr, 1,
                                     sizeof(word), i) );
                if(trace_Dump_res)
                {
                  ( RTL_CALL, rtl_Recv(DumpMsgArr,1,
                                       sizeof(int), i) );/* TrcBufFull */    /*E0163*/
                  ( RTL_CALL, rtl_Recv(TraceBufPtr,(int)TraceBufLength,
                                       1, i) );
                  trace_Dump(i, TrcBufCountArr[0], DumpMsgArr[0]);
                }
              }
           }
           else
           {
              for(i=1; i < ProcCount; i++)
              {
                 if(TraceProcNumber[i] == 0)
                    continue;

                 ( RTL_CALL, rtl_Send(DumpMsgArr, 1, sizeof(int), i) );
                 ( RTL_CALL, rtl_Recv(DumpMsgArr, 1, sizeof(int), i) );
              }
           }
        }
        else
        {
           if(BufferTraceUnLoad     &&
              BufferTrace           &&
              TraceBufLength        &&
              TraceBufPtr!=NULL     &&
              IsSlaveRun               )
           {
              ( RTL_CALL, rtl_Recv(DumpMsgArr,1,sizeof(int),
                                   MPS_MasterProc) );

              /* Dump trace buffer */    /*E0164*/

              ( RTL_CALL, rtl_Send(TraceBufCountArr,1,sizeof(word),
                                   MPS_MasterProc) );
              if(DumpMsgArr[0])
              { ( RTL_CALL, rtl_Send(TraceBufFullArr,1,sizeof(int),
                                     MPS_MasterProc) );
                ( RTL_CALL, rtl_Send(TraceBufPtr,(int)TraceBufLength,1,
                                     MPS_MasterProc) );
              }
           }
           else
           {
              ( RTL_CALL, rtl_Recv(DumpMsgArr,1,sizeof(int),
                                   MPS_MasterProc) );

              /* Dump trace buffer */    /*E0165*/

              trace_Dump(MPS_CurrentProc, TraceBufCountArr[0],
                         TraceBufFullArr[0]);
              ( RTL_CALL, rtl_Send(DumpMsgArr,1,sizeof(int),
                                   MPS_MasterProc) );
           }
        }
     }

     
     /* Close file with information messages */    /*E0166*/

     if(_SysInfoFile && SysInfoFile && SysInfo && IAmIOProcess == 0)
     {
        SYSTEM(fclose,(SysInfo))
        SysInfoFile = 0;
        _SysInfoFile = 0;
        SysInfo = NULL;
        DVMSysInfo.File = NULL;

        /* Create common file  with information messages */    /*E0167*/

        if(SysInfoSepFile)
        {
           if(MPS_CurrentProc == MPS_MasterProc)
           {
              /* Current processor in the main */    /*E0168*/

              for(i=1; i < ProcCount; i++)
                  ( RTL_CALL, rtl_Recv(DumpMsgArr,1,sizeof(int),i) );

              /*    Open resulting file 
                 with information messages */    /*E0169*/

              SYSTEM_RET(ResSysInfo,fopen,(SysInfoFileName,OPENMODE(w)))

              if(ResSysInfo == NULL)
              {
                 SYSTEM(fprintf,(stderr,
                                 "*** RTS err 020.000: can not open "
                                 "System Info File <%s>\n",
                                 SysInfoFileName))
              }
              else
              {
                 /* Fail to open the resulting file */    /*E0170*/

                 for(i=0; i < ProcCount; i++)
                 {
                    /* Loop in processors */    /*E0171*/

                    GetSysInfoName(i,InfoFileName); /* get temporary
                                                       file name */    /*E0172*/

                    /* Request size of the next temporary file */    /*E0173*/

                    #ifdef _STRUCT_STAT_                     
                       #ifdef _i860_
                          if(stat(InfoFileName, &FileInfo) == 0)
                       #else
                          if(stat(InfoFileName, &FileInfo) == 0 &&
                             (FileInfo.st_mode & S_IFREG))
                       #endif
                             FileSize = FileInfo.st_size; 
                    #endif   /* _STRUCT_STAT_ */    /*E0174*/

                    if(FileSize > 0)
                    {
                       /* Open temporary file */    /*E0175*/
                 
                       SYSTEM_RET(SysInfo, fopen,
                                  (InfoFileName,OPENMODE(r)))

                       if(SysInfo == NULL)
                       {
                          SYSTEM(fprintf,(stderr,
                                 "*** RTS err 020.000: can not open "
                                 "System Info File <%s>\n",
                                 SysInfoFileName))
                       }
                       else
                       {
                          /* Fail to open temporary file */    /*E0176*/

                          for(BufSize=FileSize+1; ; BufSize += FileSize)
                          {
                             mac_malloc(InfoFileBuf, char *, BufSize, 1);

                             if(InfoFileBuf)
                             {
                                /* Memory allocated  for temporary file.
                                            Read temporary file          */    /*E0177*/
            
                                SYSTEM_RET(InfoFileSize,fread,
                                           (InfoFileBuf,1,BufSize,
                                            SysInfo))
                                if(InfoFileSize < BufSize)
                                   break; /* all temporary file 
                                             read */    /*E0178*/

                                mac_free(&InfoFileBuf);

                                SYSTEM(rewind, (SysInfo))
                             }  
                             else
                             {
                                SYSTEM(fprintf,(stderr,
                                "*** RTS err 020.001: no memory for "
                                "System Info File <%s>\n",InfoFileName))
                                break;
                             }
                          }

                          SYSTEM(fclose, (SysInfo))

                          if(InfoFileBuf)
                          {
                             /* Fail to read temporary file */    /*E0179*/
                             
                             if(InfoFileSize > 0)
                                SYSTEM(fwrite,(InfoFileBuf,1,
                                       InfoFileSize,ResSysInfo))
                             SYSTEM(remove, (InfoFileName))
                             mac_free(&InfoFileBuf);

                          }
                       }
                    }
                    else
                    {
                       SYSTEM(remove, (InfoFileName))
                    }
                 } 

                 SYSTEM(fclose, (ResSysInfo))/* close resulting 
                                                file */    /*E0180*/
              }
           } 
           else
           {
              /* Current processor is not main */    /*E0181*/

              ( RTL_CALL, rtl_Send(DumpMsgArr,1,sizeof(int),
                                   MPS_MasterProc) );
           }
        } 
     }
  }

  MPI_BotsulaProf = 0;  /* */    /*E0182*/
  MPI_BotsulaDeb = 0;   /* */    /*E0183*/


#ifdef _MPI_PROF_TRAN_

  if(1 /*CallDbgCond*/    /*E0184*/ /*EnableTrace && dvm_OneProcSign*/    /*E0185*/)
     return;
  else
  {
     if(IsInit)
        mps_exit(rc);

     SYSTEM(exit, (rc))
  }

#else

  if(IsInit)
     mps_exit(rc);

  SYSTEM(exit, (rc))

#endif

}
                          

/*************************************************\
* Termination of internal tools of support system *
\*************************************************/    /*E0186*/

void dvm_Done(void)
{
  s_ENVIRONMENT  *Env;

#ifndef _DVM_IOPROC_
  LoopRef         PLoopRef;
  s_REDGROUP     *RG;
  RedGroupRef     RedGrpRef;
#endif

  int             i;
  void          **List;
  RTL_Request    *RTL_ReqPtr;

  #ifdef  _DVM_MPI_
     MPI_Status   Status;
  #endif

  if(RTL_TRACE)
     dvm_trace(call_dvm_Done," \n");

  /* Wait reduction completion 
     for all started reduction groups */    /*E0187*/

#ifndef _DVM_IOPROC_

  if(IAmIOProcess == 0)
  {
     for(i=0; i < gRedGroupColl->Count; i++)
     {
        RG = coll_At(s_REDGROUP *, gRedGroupColl, i);

        if(RG->StrtFlag)
        {
           RedGrpRef = (RedGroupRef)RG->HandlePtr;

           pprintf(2+MultiProcErrReg2,
                   "*** RTS warning 020.100: the reduction had not been "
                   "comleted;\nRedGroupRef=%lx\n", (uLLng)RedGrpRef);

           ( RTL_CALL, waitrd_(&RedGrpRef) );
        }
     }
  }

#endif

  /* ------------------------------------- */    /*E0188*/

#ifndef _DVM_IOPROC_  

  if(FreeObjects && IAmIOProcess == 0) /* whether DVM-objects are to be deleted
                      by the work completion */    /*E0189*/
  {
     /* Delete all contexts except the initial one */    /*E0190*/

     while(gEnvColl->Count > 1)
     {
        Env = genv_GetCurrEnv();

        if(Env->ParLoop)
        {
           /* Next context - parallel loop */    /*E0191*/

           PLoopRef = (LoopRef)(Env->ParLoop->HandlePtr);
           ( RTL_CALL, endpl_(&PLoopRef) );
        }
        else
           /* Next context - subtask */    /*E0192*/

           ( RTL_CALL, stopam_() );
     }

     /* Delete initial context with all its objects
              and create new initial context        */    /*E0193*/

     coll_AtFree(gEnvColl, 0);

     /*coll_DeleteAll(gEnvColl);*/    /*E0194*/

     Env = env_Init(&InitAMSHandle);
     coll_Insert(gEnvColl, Env);

     Env->EnvProcCount = ProcCount;

     OldEnvProcCount     = ProcCount;
     CurrEnvProcCount    = ProcCount;
     NewEnvProcCount     = ProcCount;
     CurrEnvProcCount_m1 = ProcCount - 1;
     d1_CurrEnvProcCount = 1./ProcCount;

     DVM_VMS         = MPS_VMS;
     DVM_MasterProc  = MPS_VMS->MasterProc;
     DVM_IOProc      = MPS_VMS->IOProc;
     DVM_CentralProc = MPS_VMS->CentralProc;
     DVM_ProcCount   = MPS_VMS->ProcCount;

     OldAMHandlePtr  = &InitAMSHandle;
     CurrAMHandlePtr = &InitAMSHandle;
     NewAMHandlePtr  = &InitAMSHandle;
  }
  else

#endif

  {
     /* Delete all contexts without object deletion
              and create new initial contetxt       */    /*E0195*/

     coll_DeleteAll(gEnvColl);
     Env = env_Init(&InitAMSHandle);
     coll_Insert(gEnvColl, Env);

     Env->EnvProcCount = ProcCount;

     OldEnvProcCount     = ProcCount;
     CurrEnvProcCount    = ProcCount;
     NewEnvProcCount     = ProcCount;
     CurrEnvProcCount_m1 = ProcCount - 1;
     d1_CurrEnvProcCount = 1./ProcCount;

     DVM_VMS         = MPS_VMS;
     DVM_MasterProc  = MPS_VMS->MasterProc;
     DVM_IOProc      = MPS_VMS->IOProc;
     DVM_CentralProc = MPS_VMS->CentralProc;
     DVM_ProcCount   = MPS_VMS->ProcCount;

     OldAMHandlePtr  = &InitAMSHandle;
     CurrAMHandlePtr = &InitAMSHandle;
     NewAMHandlePtr  = &InitAMSHandle;
  }

  #ifdef  _DVM_MPI_

  /* Completion of all exchanges for MPI */    /*E0196*/

  if(IAmIOProcess == 0)
  {
     for(i=0; i < RequestCount; i++)
     {
        if(MsgSchedule == 0)
           pprintf(2+MultiProcErrReg2,
                   "*** RTS warning 020.101: MPI_Irecv "
                   "(MPI_Isend, MPI_Issend) had not been comleted;\n"
                   "RTL_ReqPtr=%lx\n", (uLLng)RequestBuffer[i]);
        else
           pprintf(2+MultiProcErrReg2,
                   "*** RTS warning 020.102: MPI_Irecv "
                   "had not been comleted (MsgSchedule=1);\n"
                   "RTL_ReqPtr=%lx\n", (uLLng)RequestBuffer[i]);

        SYSTEM(MPI_Wait, (&RequestBuffer[i]->MPSFlag,
                          &RequestBuffer[i]->Status))

     }

     for(i=0; i < MPS_RequestCount; i++)
     {
        if(MsgSchedule == 0)
           pprintf(2+MultiProcErrReg2,
                   "*** RTS warning 020.101: MPI_Irecv "
                   "(MPI_Isend, MPI_Issend) had not been comleted;\n"
                   "MPI_ReqPtr=%lx\n", (uLLng)MPS_RequestBuffer[i]);
        else
           pprintf(2+MultiProcErrReg2,
                   "*** RTS warning 020.102: MPI_Irecv "
                   "had not been comleted (MsgSchedule=1);\n"
                   "MPI_ReqPtr=%lx\n", (uLLng)MPS_RequestBuffer[i]);

        SYSTEM(MPI_Wait, (MPS_RequestBuffer[i], &Status))
     }
  }

  RequestCount = 0;
  MPS_RequestCount = 0;

  #endif

  /* */    /*E0197*/

  if(MsgSchedule && IAmIOProcess == 0)
  {
     List  = SendReqColl.List;

     while(SendReqColl.Count)
     {
        RTL_ReqPtr = (RTL_Request *)List[0];

        pprintf(2+MultiProcErrReg2,
                "*** RTS warning 020.103: RTL_Sendnowait "
                "had not been comleted (MsgSchedule=1);\n"
                "RTL_ReqPtr=%lx\n", (uLLng)RTL_ReqPtr);

        ( RTL_CALL, rtl_Waitrequest(RTL_ReqPtr) );
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_Done, " \n" );

  (DVM_RET);

  return;
}


/* */    /*E0198*/

void  WriteStatToFile(DVMFILE  *StatFile)
{ int           i, j, StatBufLen;
DvmType          BufLen;
  char         *Buf, *CharPtr;
  RTL_Request  *Req;
  RTL_Request   InReq;
  byte          IsWrite = 0;

#ifdef _DVM_ZLIB_
  uLongf       *Len;
  Bytef       **BufProc;
  Bytef        *gzBuf;
  uLongf        gzLen = 0;
  RTL_Request   gzInReq;
  int           StatCompressLev;
#endif

  StatBufLen = (int)StatBufLength;

  i = 0;

  #ifdef _DVM_ZLIB_

  if(StatCompressLevel >= 0 && StatCompressScheme != 0)
     i = 1;  /* */    /*E0199*/

  #endif

  if(i == 0)
  {
     /* */    /*E0200*/

     if(MPS_CurrentProc == MPS_IOProc)
     { 
        /* */    /*E0201*/

        BufLen = ProcCount * StatBufLen;
        mac_malloc(Buf, char *, BufLen, 1);

        if(Buf == NULL)
        {
           /* */    /*E0202*/

           if(WriteStatByParts == 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS fatal err: wrong call WriteStatToFile "
                      "(no memory, WriteStatByParts = 0)\n");

           mac_malloc(Buf, char *, StatBufLen, 0);

           for(i=0; i < ProcCount; i++)
           {
              if(i == MPS_CurrentProc)
              {  
                 if(WriteStat)
                 {
                    IsWrite = 0;
                 
                    #ifdef _DVM_ZLIB_

                    if(StatFile->zip)
                    {  IsWrite = 1;
                 
                       SYSTEM( gzwrite, (StatFile->zlibFile,
                                         (voidp)StatBufPtr,
                                         (unsigned)StatBufLen) )
                    }

                    #endif

                    if(IsWrite == 0)
                       SYSTEM( fwrite, (StatBufPtr, (size_t)StatBufLen,
                                        1, StatFile->File) )
                 }
              }
              else
              {  ( RTL_CALL, rtl_Recvnowait(Buf, StatBufLen, 1, i,
                                            msg_common, &InReq, 0) );
                 ( RTL_CALL, rtl_Waitrequest(&InReq) );

                 if(WriteStat)
                 {
                    IsWrite = 0;
                 
                    #ifdef _DVM_ZLIB_

                    if(StatFile->zip)
                    {  IsWrite = 1;
                 
                       SYSTEM( gzwrite, (StatFile->zlibFile, (voidp)Buf,
                                         (unsigned)StatBufLen) )
                    }

                    #endif

                    if(IsWrite == 0)
                       SYSTEM( fwrite, (Buf, (size_t)StatBufLen, 1,
                                        StatFile->File) )
                 }
              }
           }

           mac_free(&Buf);
        }
        else
        {  /* */    /*E0203*/

           dvm_AllocArray(RTL_Request, ProcCount, Req);

           CharPtr = Buf;

           for(i=0; i < ProcCount; i++, CharPtr += StatBufLen)
           {  if(i == MPS_CurrentProc)
              {  for(j=0; j < StatBufLen; j++)
                     CharPtr[j] = StatBufPtr[j];
              }
              else
              {  ( RTL_CALL, rtl_Recvnowait(CharPtr, StatBufLen, 1, i,
                                            msg_common, &Req[i], 0) );
              }
           }

           for(i=0; i < ProcCount; i++)
               if(i != MPS_CurrentProc)
                  ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

           dvm_FreeArray(Req);

           if(WriteStat)
           {
              IsWrite = 0;
                 
              #ifdef _DVM_ZLIB_

              if(StatFile->zip)
              {  IsWrite = 1;
                 
                 SYSTEM( gzwrite, (StatFile->zlibFile, (voidp)Buf,
                                   (unsigned)BufLen) )
              }

              #endif

              if(IsWrite == 0)
                 SYSTEM( fwrite, (Buf, (size_t)BufLen, 1,
                                  StatFile->File) )
           } 

           mac_free(&Buf);
        }
     }
     else
     {  /* */    /*E0204*/

        ( RTL_CALL, rtl_Sendnowait(StatBufPtr, StatBufLen, 1,
                                   MPS_IOProc, msg_common, &InReq, 0) );
        ( RTL_CALL, rtl_Waitrequest(&InReq) );
     }
  }
  else
  {  /* */    /*E0205*/

     #ifdef _DVM_ZLIB_

     StatCompressLev = StatCompressLevel;

     if(StatCompressLev == 0)
     {
        /* */    /*E0206*/

        if(CompressLevel == 0)
           StatCompressLev = Z_DEFAULT_COMPRESSION;
        else
           StatCompressLev = CompressLevel;
     }
                      
     if(MPS_CurrentProc == MPS_IOProc)
     {
        /* */    /*E0207*/

        dvm_AllocArray(RTL_Request, ProcCount, Req);
        dvm_AllocArray(uLongf, ProcCount, Len);
        dvm_AllocArray(Bytef *, ProcCount, BufProc);

        /* */    /*E0208*/

        for(i=0; i < ProcCount; i++)
        {  if(i == MPS_CurrentProc)
              continue;

           ( RTL_CALL, rtl_Recvnowait(&Len[i], sizeof(uLongf), 1, i,
                                      msg_common, &Req[i], 0) );
        }

        /* */    /*E0209*/

        j = StatBufLen + StatBufLen/50 + 12;
        Len[MPS_CurrentProc] = j;
        j += 6 + 12;      /* */    /*E0210*/

        mac_malloc(BufProc[MPS_CurrentProc], Bytef *, j, 0);

        if(StatCompressScheme < 0)  /* */    /*E0211*/
        {  sprintf(StatBufPtr, "Proc=%d  aaabbbcccddd\n",
                   MPS_CurrentProc);
           StatBufLen = strlen(StatBufPtr);
        }

        CharPtr  = (char *)BufProc[MPS_CurrentProc];

        SYSTEM(sprintf, (CharPtr, "%ld", StatBufLength))
        SYSTEM_RET(j, strlen, (CharPtr))
        CharPtr += j + 1;

        SYSTEM(sprintf, (CharPtr, "%ld", ProcCount))
        SYSTEM_RET(i, strlen, (CharPtr))
        CharPtr += i + 1;

        j += i + 1;

        gzBuf = (Bytef *)(CharPtr + 11);

        SYSTEM_RET(i, compress2, (gzBuf, &Len[MPS_CurrentProc],
                                  (Bytef *)StatBufPtr,
                                  (uLong)StatBufLen, StatCompressLev))

        if(i != Z_OK)
        {
           /* */    /*E0212*/

           if(ZLIB_Warning)
              pprintf(2+MultiProcErrReg2,
                      "*** RTS warning: wrong call WriteStatToFile "
                      "(compress2 rc = %d)\n", i);

           mac_free(&BufProc[MPS_CurrentProc]);

           j = StatBufLen + StatBufLen + 12;
           Len[MPS_CurrentProc] = j;
           j += 6 + 12;      /* */    /*E0213*/

           mac_malloc(BufProc[MPS_CurrentProc], Bytef *, j, 0);

           if(StatCompressScheme < 0)  /* */    /*E0214*/
           {  sprintf(StatBufPtr, "Proc=%d  aaabbbcccddd\n",
                      MPS_CurrentProc);
              StatBufLen = strlen(StatBufPtr);
           }

           CharPtr  = (char *)BufProc[MPS_CurrentProc];

           SYSTEM(sprintf, (CharPtr, "%ld", StatBufLength))
           SYSTEM_RET(j, strlen, (CharPtr))
           CharPtr += j + 1;

           SYSTEM(sprintf, (CharPtr, "%ld", ProcCount))
           SYSTEM_RET(i, strlen, (CharPtr))
           CharPtr += i + 1;

           j += i + 1;

           gzBuf    = (Bytef *)(CharPtr + 11);

           SYSTEM_RET(i, compress2, (gzBuf, &Len[MPS_CurrentProc],
                                     (Bytef *)StatBufPtr,
                                     (uLong)StatBufLen, StatCompressLev))
           if(i != Z_OK)
              pprintf(2+MultiProcErrReg2,
                      "*** RTS err: wrong call WriteStatToFile "
                      "(compress2 rc = %d)\n", i);
        }

        SYSTEM(sprintf, (CharPtr, "%.10d", (int)Len[MPS_CurrentProc]))

        j += 1 + 11 + (int)Len[MPS_CurrentProc];

        if(WriteStat)
           SYSTEM(fwrite, (BufProc[MPS_CurrentProc], j, 1,
                           StatFile->File) )

        /* */    /*E0215*/

        for(i=0; i < ProcCount; i++)
        {
           if(i == MPS_CurrentProc)
              continue;

           /* */    /*E0216*/

           (RTL_CALL, rtl_Waitrequest(&Req[i]));

           j = (int)Len[i] + 11;

           mac_malloc(BufProc[i], Bytef *, j, 0);

           j -= 11;     /* */    /*E0217*/

           CharPtr = (char *)BufProc[i];

           SYSTEM(sprintf, (CharPtr, "%.10d", j))

           CharPtr += 11;
           gzBuf = (Bytef *)CharPtr;

           ( RTL_CALL, rtl_Recvnowait(gzBuf, j, 1, i, msg_common,
                                         &Req[i], 0) );
        }

        /* */    /*E0218*/

        for(i=0; i < ProcCount; i++)
        {
           if(i == MPS_CurrentProc)
              continue;

           rtl_Waitrequest(&Req[i]);

           j = (int)Len[i] + 11;

           if(WriteStat)
              SYSTEM( fwrite, (BufProc[i], j, 1, StatFile->File) )
        }

        /* */    /*E0219*/

        for(i=0; i < ProcCount; i++)
        {  mac_free(&BufProc[i]);
        } 
 
        dvm_FreeArray(Req);
        dvm_FreeArray(Len);
        dvm_FreeArray(BufProc);
     }
     else
     {
        /* */    /*E0220*/

        j = StatBufLen + StatBufLen/50 + 12; /* */    /*E0221*/
        gzLen = j;

        mac_malloc(gzBuf, Bytef *, j, 0);

        if(StatCompressScheme < 0)  /* */    /*E0222*/
        {  sprintf(StatBufPtr, "Proc=%d  aaabbbcccddd\n",
                   MPS_CurrentProc);
           StatBufLen = strlen(StatBufPtr);
        }

        SYSTEM_RET(j, compress2, (gzBuf, &gzLen, (Bytef *)StatBufPtr,
                                  (uLong)StatBufLen, StatCompressLev))

        if(j != Z_OK)
        {  /* */    /*E0223*/

           if(ZLIB_Warning)
              pprintf(2+MultiProcErrReg2,
                      "*** RTS warning: wrong call WriteStatToFile "
                      "(compress2 rc = %d)\n", j);

           mac_free(&gzBuf);

           j = StatBufLen + StatBufLen + 12; /* */    /*E0224*/
           gzLen = j;

           mac_malloc(gzBuf, Bytef *, j, 0);

           if(StatCompressScheme < 0)  /* */    /*E0225*/
           {  sprintf(StatBufPtr, "Proc=%d  aaabbbcccddd\n",
                      MPS_CurrentProc);
              StatBufLen = strlen(StatBufPtr);
           }

           SYSTEM_RET(j, compress2, (gzBuf, &gzLen, (Bytef *)StatBufPtr,
                                     (uLong)StatBufLen, StatCompressLev))

           if(j != Z_OK)
              pprintf(2+MultiProcErrReg2,
                      "*** RTS err: wrong call WriteStatToFile "
                      "(compress2 rc = %d)\n", j);
        }

        ( RTL_CALL, rtl_Sendnowait(&gzLen, sizeof(uLongf), 1,
                                   MPS_IOProc, msg_common, &InReq, 0) );
        ( RTL_CALL, rtl_Sendnowait(gzBuf, gzLen, 1, MPS_IOProc,
                                   msg_common, &gzInReq, 0) );
        ( RTL_CALL, rtl_Waitrequest(&InReq) );
        ( RTL_CALL, rtl_Waitrequest(&gzInReq) );

        mac_free(&gzBuf);
     }

     #endif

  } 

  return;
}


#endif  /*  _INITEXIT_C_  */    /*E0226*/
