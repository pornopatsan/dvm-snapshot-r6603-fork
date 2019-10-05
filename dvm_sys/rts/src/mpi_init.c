#ifndef _MPI_INIT_C_
#define _MPI_INIT_C_
/******************/    /*E0000*/

void   setMPInumbers(int argc, char **argv);
void   mpi_msg_test(void);

static	int	Reorder = 0;
static	int	P_Excl = 0;
static	int	excl_procs = 0;
static	int	DVMShow = 0; 
#ifdef _WIN_MPI_
  static  DvmType  MesLoopCount = 10000000;
#else
  static  DvmType  MesLoopCount = 40000000;
#endif

int              MPI_ProcCount = 1;
int             *MPI_NumberList = NULL;
static double   *ProcTimes = NULL;

/* used statist.c */    /*E0001*/

double	         ProcTime, ResTime;
char             ProcName[MPI_MAX_PROCESSOR_NAME+1];
int              ProcNameLen;

/* SetAffinity */

/*#include <sched.h>
#include <omp.h>
void SetAffinity (int rank) {
    int MPI_PROCESSES_PER_NODE = omp_get_num_procs ()/omp_get_max_threads ();
#pragma omp parallel
  {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    int cpu = (rank% MPI_PROCESSES_PER_NODE)*omp_get_num_threads() + omp_get_thread_num ();
    CPU_SET(cpu,&mask);
    sched_setaffinity ((pid_t)0, sizeof(cpu_set_t),&mask);
  }
}
*/
/************************\
* MPI RTL INIT procedure *
\************************/    /*E0002*/

DvmType  rtl_init(DvmType  InitParam, int  argc, char  *argv[])
{
  MPI_Status    statadd;
  double        MaxTime, MidTime, MinTime;
  int           i, j, k, ProcMax, ProcMin, retval;

  int          *MPS_ProcNameLen = NULL, *displs = NULL; 
  char         *CharPtr;

/* InitParam = 2; */    /*E0003*/

  dvmclock = 1. / CLOCKS_PER_SEC;

  dvm_OneProcSign = InitParam & '\x2';

  dvm_InitParam = InitParam  & '\x1';
  InitParam = dvm_InitParam;

  dvm_argc = argc;
  mac_malloc(dvm_argv, char **, (dvm_argc + 1) * sizeof(char *), 1);
  for (i = 0; i < dvm_argc; i++) {
     mac_malloc(dvm_argv[i], char *, strlen(argv[i]) + 1, 1);
     strcpy(dvm_argv[i], argv[i]);
  }
  dvm_argv[dvm_argc] = 0;

  /* Check checksum of command memory */    /*e0004*/

  if(CodeCheckSumPrint)
     SYSTEM(fprintf, ( stderr, "CodeStartAddr=%lx;  CodeFinalAddr=%lx;  "
                               "CodeCheckSum=%lx;\n",
            dvm_CodeStartAddr, dvm_CodeFinalAddr, dvm_CodeCheckSum ))

  if(dvm_CodeStartAddr && dvm_CodeFinalAddr &&
     dvm_CodeStartAddr <= dvm_CodeFinalAddr)
  {
    if(dvm_CodeCheckSum == 0)
       dvm_CodeCheckSum =
       dvm_CheckSum((unsigned short *)dvm_CodeStartAddr,
                    (unsigned short *)dvm_CodeFinalAddr);

     dvm_CodeCheckMem();
  }

  /*******************************\
  * Initialization of MPI library *
  \*******************************/    /*e0005*/

  RTS_Call_MPI = 1;  /* */    /*E0006*/

  SYSTEM(MPI_Initialized, (&i))

  if(MPI_ProfInitSign == 0 && i == 0)
     SYSTEM_RET(retval, MPI_Init, (&dvm_argc, &dvm_argv)) /* initialize MPI system */    /*e0007*/

  if(dvm_OneProcSign)
  {
     SYSTEM(MPI_Comm_dup, (MPI_COMM_SELF, &MPI_COMM_WORLD_1))
  }
  else
  {
     SYSTEM(MPI_Comm_dup, (MPI_COMM_WORLD, &MPI_COMM_WORLD_1))
  }

  SYSTEM(MPI_Comm_rank, (MPI_COMM_WORLD,
                         &dvm_OneProcNum))       /* my place in MPI system */    /*e0008*/
  SYSTEM(MPI_Comm_rank, (MPI_COMM_WORLD_1,
                         &MPS_CurrentProcIdent))
  //SetAffinity(dvm_OneProcNum);
  /* Initialization of initial system trace */    /*e0009*/

  systrace_set(dvm_argc, dvm_argv, dvm_OneProcNum);

  SYSTEM(MPI_Comm_size, (MPI_COMM_WORLD_1,
                         &MPS_ProcCount)) /* size of MPI system */    /*e0010*/
  SYSTEM(MPI_Comm_size, (MPI_COMM_WORLD,
                         &dvm_OneProcCount))

  SYSTEM(MPI_Comm_dup, (MPI_COMM_WORLD_1, &DVM_COMM_WORLD))
  SYSTEM(MPI_Comm_dup, (MPI_COMM_WORLD_1, &APPL_COMM_WORLD))
  SYSTEM(MPI_Comm_dup, (MPI_COMM_WORLD_1, &IO_COMM_WORLD))

  ProcCount            = MPS_ProcCount;
  MPI_CurrentProcIdent = MPS_CurrentProcIdent;
  CurrentProcIdent     = MPI_CurrentProcIdent;
  MPS_CurrentProc      = CurrentProcIdent;
  MasterProcIdent      = 0;

  if(MPS_ProcCount == 1)
  {
     MinMPIMsgLen  = 0;
     MinMPIMsgLen0 = 0;
  }

  sysprintf(__FILE__,__LINE__, "MPS_Comm_size: MPS_ProcCount=%d\n",
                                MPS_ProcCount);

  /* */    /*E0011*/

  ioprocess_set(dvm_argc, dvm_argv);  /* */    /*E0012*/

  /* */    /*E0013*/

  SYSTEM(MPI_Bcast, ((void *)&IOProcess, 1, MPI_INT, 0, MPI_COMM_WORLD_1))

  mac_malloc(IOProcessSign, int *, MPS_ProcCount*sizeof(int), 0);

  /* */    /*E0014*/

  SYSTEM(MPI_Allgather, ((void *)&IAmIOProcess, 1, MPI_INT,
                         (void *)IOProcessSign, 1, MPI_INT,
                         MPI_COMM_WORLD_1))

  /* */    /*E0015*/

  mac_malloc(ApplProcessNumber, int *, MPS_ProcCount*sizeof(int), 0);
  mac_malloc(IOProcessNumber, int *, MPS_ProcCount*sizeof(int), 0);
  mac_malloc(ApplIOProcessNumber, int *, MPS_ProcCount*sizeof(int), 0);

  ApplProcessCount = 0; /* */    /*E0016*/
  IOProcessCount   = 0; /* */    /*E0017*/

  for(i=0; i < MPS_ProcCount; i++)
  {
     if(IOProcessSign[i] == 0)
     {
        /* */    /*E0018*/

        ApplProcessNumber[ApplProcessCount] = i;
        ApplIOProcessNumber[i] = ApplProcessCount;
        ApplProcessCount++;
     }
     else
     {
        /* */    /*E0019*/

        IOProcessNumber[IOProcessCount] = i;
        ApplIOProcessNumber[i] = IOProcessCount;
        IOProcessCount++;
     }
  }

  if(ApplProcessCount == 0)
  {
     if(MPS_CurrentProcIdent == 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err (mpi_init): applied process count = 0\n");

     RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

     if(1 /*CallDbgCond*/    /*E0020*/ /*EnableTrace && dvm_OneProcSign*/    /*E0021*/)
        SYSTEM(MPI_Finalize, ())
     else
        dvm_exit(1);

#else

     dvm_exit(1);

#endif

  }

  if(IOProcess == 0 && IOProcessCount != 0)
  {
     /* */    /*E0022*/

     if(MPS_CurrentProcIdent == 0)
        SYSTEM(fprintf,
               (stderr, "*** RTS warning (mpi_init): input/output "
                        "process count is not zero\n"))
  }

  /* */    /*E0023*/

  #ifdef _WIN_MPI_
     SYSTEM_RET(ProcNameLen, sprintf, (ProcName, "%d",
                MPS_CurrentProcIdent))
  #else
     SYSTEM(MPI_Get_processor_name, (ProcName, &ProcNameLen))
  #endif

  if(ProcNameLen > MPI_MAX_PROCESSOR_NAME)
  {
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
           "*** RTS fatal err (mpi_init): proc name is too long (%d)\n", ProcNameLen);
  }

  ProcName[ProcNameLen] = 0;

  mac_malloc(MPS_ProcNameList, char **, MPS_ProcCount*sizeof(char *), 0);
  mac_malloc(MPS_ProcNameLen, int *, MPS_ProcCount*sizeof(int), 0);

  i = ((ProcNameLen+1)/sizeof(int) + 1) * sizeof(int); /* */    /*E0024*/
  SYSTEM(MPI_Allgather,
         ((void *)&i, 1, MPI_INT, (void *)MPS_ProcNameLen,
          1, MPI_INT, MPI_COMM_WORLD_1))  /* */    /*E0025*/

  mac_malloc(displs, int *, MPS_ProcCount*sizeof(int), 0);

  j = 0;  /* */    /*E0026*/

  for(k=0; k < MPS_ProcCount; k++)
  {
     displs[k] = j;
     j += MPS_ProcNameLen[k];
  }
   
  /* */    /*E0027*/

  mac_malloc(MPS_ProcName, char *, j, 0);

  SYSTEM(MPI_Allgatherv,
         ((void *)ProcName, i, MPI_BYTE, (void *)MPS_ProcName,
          MPS_ProcNameLen, displs, MPI_BYTE, MPI_COMM_WORLD_1))

  CharPtr = MPS_ProcName;

  for(k=0; k < MPS_ProcCount; k++)
  {
     MPS_ProcNameList[k] = CharPtr;
     CharPtr += MPS_ProcNameLen[k];     
  }

  mac_free((void **)&MPS_ProcNameLen);
  mac_free((void **)&displs);

  /* ------------------------------------------------ */    /*E0028*/

  /* */    /*E0029*/

  SYSTEM(MPI_Comm_free, (&DVM_COMM_WORLD))
  SYSTEM(MPI_Comm_free, (&APPL_COMM_WORLD))
  SYSTEM(MPI_Comm_free, (&IO_COMM_WORLD))

  SYSTEM(MPI_Comm_group, (MPI_COMM_WORLD_1, &mpi_gr))

  SYSTEM(MPI_Group_incl, (mpi_gr, ApplProcessCount, ApplProcessNumber,
                          &appl_gr))
  SYSTEM(MPI_Comm_create, (MPI_COMM_WORLD_1, appl_gr, &APPL_COMM_WORLD))

  if(IOProcessCount != 0)
  {
     SYSTEM(MPI_Group_incl, (mpi_gr, IOProcessCount, IOProcessNumber,
                             &io_gr))
     SYSTEM(MPI_Comm_create, (MPI_COMM_WORLD_1, io_gr, &IO_COMM_WORLD))
  }

  /* */    /*E0030*/

  if(IAmIOProcess)
     MPI_ProcCount  = IOProcessCount;
  else
     MPI_ProcCount  = ApplProcessCount;

  mac_malloc(MPI_NumberList, int *, MPS_ProcCount*sizeof(int), 0);

  for(i=0; i < MPI_ProcCount; i++)
      MPI_NumberList[i] = i;

  if(IAmIOProcess)
  {
     SYSTEM(MPI_Comm_rank, (IO_COMM_WORLD,
                            &MPI_CurrentProcIdent)) /* */    /*E0031*/

     /* */    /*E0032*/

     SYSTEM(MPI_Comm_dup, (IO_COMM_WORLD, &DVM_COMM_WORLD))
     SYSTEM(MPI_Comm_group, (DVM_COMM_WORLD, &dvm_gr))
  }
  else
  {
     SYSTEM(MPI_Comm_rank, (APPL_COMM_WORLD,
                            &MPI_CurrentProcIdent)) /* */    /*E0033*/

     /* */    /*E0034*/

     setMPInumbers(dvm_argc, dvm_argv);

     /* */    /*E0035*/

     SYSTEM(MPI_Group_incl, (appl_gr, ApplProcessCount, MPI_NumberList,
                             &dvm_gr))
     SYSTEM(MPI_Comm_create, (APPL_COMM_WORLD, dvm_gr, &DVM_COMM_WORLD))
  }

  SYSTEM(MPI_Comm_rank, (DVM_COMM_WORLD,
                         &MPI_CurrentProcIdent)) /* */    /*E0036*/

  /* ------------------------------------------ */    /*E0037*/

  CurrentProcIdent = MPI_CurrentProcIdent;
  MasterProcIdent  = 0;
  ProcCount        = MPI_ProcCount;        /* let it be like that */    /*e0038*/
  MPS_CurrentProc  = CurrentProcIdent;     /* internal number of
                                              current subtask */    /*e0039*/

  /* Form array of processor identifiers */    /*e0040*/

  for(i=0; i < ProcCount; i++)
      ProcIdentList[i] = i;

  IsInit = 1;       /* MPS has been initialized */    /*e0041*/

  /* ------------------------------------------------- */    /*e0042*/

  DVM_TIME1       = dvm_time();
  SystemStartTime = DVM_TIME1;

  GetCurrentParName(dvm_argc, dvm_argv); /* current file name current.par
                                    is input from the command line */    /*e0043*/

  ( RTL_CALL, mps_Bcast(CurrentParName, 256, 1) );

  GetActPar(dvm_argc, dvm_argv);   /* flags 0f swiching on directories and
                              parameter files
                              are input from the command line */    /*e0044*/
  GetDeactPar(dvm_argc, dvm_argv); /* flags 0f swiching off directories and
                              parameter files
                              are input from the command line */    /*e0045*/

  ( RTL_CALL, mps_Bcast(&DeactCurrentPar, 1, sizeof(int)) );

  if(DeactCurrentPar)
     GetInitialPS(dvm_argc, dvm_argv); /* initial processor system sizes 
                                  are input from the command line  */    /*e0046*/

  (RTL_CALL, mps_Bcast(VMSSize, MaxVMRank, sizeof(DvmType)));

  GetFopenCount(dvm_argc, dvm_argv); /* number af additional attempts 
                                to open parameter files 
                                is input from the command line */    /*e0047*/

  ( RTL_CALL, mps_Bcast(&ParFileOpenCount, 1, sizeof(int)) );

  GetTraceFileName(dvm_argc, dvm_argv); /* left part of new user program trace file name
                                   and standatd trace file name
                                   are input from the command line */    /*e0048*/
  ( RTL_CALL, mps_Bcast(TraceOptions.OutputTracePrefix, MaxParFileName+1, 1) );
  ( RTL_CALL, mps_Bcast(TraceOptionsTraceFile, MaxPathSize + 1, 1) );

  sysprintf(__FILE__,__LINE__, "call dvm_InpCurrentPar\n");

  dvm_InpCurrentPar(); /* input initial parameters */    /*e0049*/

  sysprintf(__FILE__,__LINE__, "ret dvm_InpCurrentPar\n");

  dvm_CorrOut(); /* define and correct parameters for
                    information message output */    /*e0050*/

  if(IAmIOProcess == 0)
  {
     if(ProcCount > MPI_ProcCount - excl_procs)
        eprintf(__FILE__,__LINE__,
                "*** RTS fatal err 001.004: Proc Count(=%ld) < "
                "MPI Proc Count(=%d)\n",
                ProcCount, MPI_ProcCount-excl_procs);

     if(ProcCount < MPI_ProcCount)
     {
        SYSTEM(MPI_Comm_free, (&DVM_COMM_WORLD))

        SYSTEM(MPI_Group_incl, (appl_gr, ProcCount, MPI_NumberList,
                                &dvm_gr))
        SYSTEM(MPI_Comm_create, (APPL_COMM_WORLD, dvm_gr,
                                 &DVM_COMM_WORLD))
     }
  }

  if(CurrentProcIdent == 0 && IAmIOProcess == 0)
  {
    MidTime = 0.0;
    MinTime = ProcTimes[MPI_NumberList[0]];
    ProcMin = 0;
    MaxTime = ProcTimes[MPI_NumberList[0]];
    ProcMax = 0;

    for(i=0; i < ProcCount; i++)
    {
       if(MaxTime < ProcTimes[MPI_NumberList[i]])
       {  MaxTime = ProcTimes[MPI_NumberList[i]];
          ProcMax = i;
       }

       if(MinTime > ProcTimes[MPI_NumberList[i]])
       {  MinTime = ProcTimes[MPI_NumberList[i]];
          ProcMin = i;
       }

       MidTime += ProcTimes[MPI_NumberList[i]];
    }

    MidTime /= ProcCount;

    if((Reorder) || (P_Excl) || (DVMShow))
       SYSTEM(printf,
              ("*** RTS (mpi_init): MinTime=%f(%d), MidTime=%f(%ld), "
              "MaxTime=%f(%d)\n",
              MinTime, ProcMin, MidTime, ProcCount, MaxTime, ProcMax))

//printf("*** RTS (mpi_init): MinTime=%f(%d), MidTime=%f(%ld), "
//              "MaxTime=%f(%d)\n",
//              MinTime, ProcMin, MidTime, ProcCount, MaxTime, ProcMax);




  }

  mac_free((void **)&ProcTimes);   /* !!!!!!!!!!!!!! */    /*E0051*/

  ExclProcWait = 1;  /* */    /*E0052*/

  if(MPI_CurrentProcIdent >= ProcCount && IAmIOProcess == 0)
  {
     /* */    /*E0053*/

     SYSTEM(MPI_Recv, (&i, 1, MPI_INT, MPI_NumberList[0],
                       msg_SynchroSendRecv, APPL_COMM_WORLD, &statadd))

     mps_exit(0);
  }

  sysprintf(__FILE__,__LINE__, "call dvm_InpSysPar\n");

  dvm_InpSysPar();     /* input system parameters */    /*e0054*/

  sysprintf(__FILE__,__LINE__, "ret dvm_InpSysPar\n");

  DVM_Prof_Init1(); /* */    /*E0055*/

  sysprintf(__FILE__,__LINE__, "call dvm_SysInit\n");

  dvm_SysInit();       /* install support system */    /*e0056*/

  sysprintf(__FILE__,__LINE__, "ret dvm_SysInit\n");

  /* Change processor identifier array */    /*e0057*/

  for(i=0; i < ProcCount; i++)
      ProcIdentList[i] = ProcNumberList[i];

  ProcIdentList[0] = MasterProcIdent;

  /* Create internal numbers of the current and main subtasks */    /*e0058*/

  MPS_MasterProc = 0;

  for(MPS_CurrentProc=0; MPS_CurrentProc < ProcCount; MPS_CurrentProc++)
      if(ProcIdentList[MPS_CurrentProc] == CurrentProcIdent)
         break;

  if(MPS_CurrentProc == ProcCount)
     eprintf(__FILE__,__LINE__,
        "*** RTS fatal err 001.005: invalid Current Proc Ident (%ld)\n",
        CurrentProcIdent);

  CurrentProcNumber = ProcNumberList[MPS_CurrentProc]; /* external number of
                                                          current processor */    /*e0059*/

  IsSlaveRun = 1;   /* attribute: offspring tasks are running */    /*e0060*/

  sysprintf(__FILE__,__LINE__, "call dvm_TraceInit\n");

  dvm_TraceInit();   /* initialize trace and built in 
                        debugging tools  */    /*e0061*/

  sysprintf(__FILE__,__LINE__, "ret dvm_TraceInit\n");

  if(VersStartPrint && _SysInfoPrint)
     PrintVers(VersFullStartPrint, 1); /* print version into screen */    /*e0062*/

  PrintVers(1, 0);                     /* print version on trace */    /*e0063*/

  if(RTL_TRACE)
     ( RTL_CALL, dvm_trace(DVM_Trace_Start,"\n"), DVM_RET );

  CheckRendezvousInit();     /* fix that only one exchange between 
                                two processors is allowed  */    /*e0064*/

  /* Set count of received messages */    /*e0065*/

  MessageCount[16] = 1;

  sysprintf(__FILE__,__LINE__, "call dvm_Init\n");

  ( RTL_CALL, dvm_Init(InitParam) );  /* initialization of DVMLIB */    /*e0066*/

  sysprintf(__FILE__,__LINE__, "ret dvm_Init\n");

  if(IAmIOProcess == 0)
     mpi_msg_test(); /* */    /*E0067*/

  UserStartTime = dvm_time();

  UserSumFlag = 1;                /* attribute: to summarize time 
                                of user program execution */    /*e0068*/
  IOProcTime = dvm_time();        /* */    /*E0069*/

#ifdef _DVM_IOPROC_

   #ifdef _UNIX_
      if(PrtSign == 2 || PrtSign == 3)
      {
         if(IOProcPrt != 0)
         {
            SYSTEM_RET(i, nice, (IOProcPrt))

            if(RTL_TRACE)
               tprintf("nice: Prt=%d RC=%d\n", IOProcPrt, i);
         }
      }
   #endif

     DVM_Prof_Init2();         /* */    /*E0070*/

     ( RTL_CALL, dvm_ioproc() );  /* */    /*E0071*/

#endif

#ifdef _UNIX_

  if(PrtSign == 1 || PrtSign == 3)
  {
     if(ApplProcPrt != 0)
     {
        SYSTEM_RET(i, nice, (ApplProcPrt))

        if(RTL_TRACE)
           tprintf("nice: Prt=%d RC=%d\n", ApplProcPrt, i);
     } 
  }

#endif

     DVM_Prof_Init2();         /* */    /*E0072*/
     RTS_Call_MPI = 0;         /* */    /*E0073*/
     return  (int)retval;      /* return to user program */    /*e0074*/
}


/* */    /*E0075*/

void   setMPInumbers(int argc, char **argv)
{
  FILE    *fh;                                 
  char    *p;
  int      i, j;
  char     optch[20];
  int      iTime;
  double   PTime = 1.0;
  char     Buf[257];

  if(MPI_CurrentProcIdent == 0)
  {
     for(i=0; i < argc; i++)
     {
        SYSTEM(sprintf, (optch, "%cptime", Minus))

        if(strcmp(argv[i], optch) == 0)
        {
           if(((i+1) < argc) && isdigit((unsigned int)*argv[i+1]))
           {
              iTime = atoi(argv[i+1]);
              PTime = (double)iTime;
           }
        }

        SYSTEM(sprintf, (optch, "%cperf", Minus))

        if(strcmp(argv[i], optch) == 0)
        {
           if(((i+1) < argc) && isdigit((unsigned int)*argv[i+1]))
           {
              P_Excl = atoi(argv[i+1]);
           }
        }
//printf("argv[%d]=%s\n",i,argv[i]);
        SYSTEM(sprintf, (optch, "%carloopsize", Minus))

        if(strcmp(argv[i], optch) == 0)
        {
           if(((i+1) < argc) && isdigit((unsigned int)*argv[i+1]))
           {
              MesLoopCount = atoi(argv[i+1]);
           }
        }

        SYSTEM(sprintf, (optch, "%creord", Minus))

        if(strcmp(argv[i], optch) ==0 )
            Reorder = 1;

        SYSTEM(sprintf, (optch, "%cdvmshow", Minus))

        if(strcmp(argv[i], optch) ==0 )
            DVMShow = 1;
     }
  }

  SYSTEM(MPI_Bcast, (&PTime, 1, MPI_DOUBLE, 0, APPL_COMM_WORLD))
  SYSTEM(MPI_Bcast, (&P_Excl, 1, MPI_INT, 0, APPL_COMM_WORLD))
  SYSTEM(MPI_Bcast, (&Reorder, 1, MPI_INT, 0, APPL_COMM_WORLD))
  SYSTEM(MPI_Bcast, (&MesLoopCount, 1, MPI_LONG, 0, APPL_COMM_WORLD))
  SYSTEM(MPI_Bcast, (&DVMShow, 1, MPI_INT, 0, APPL_COMM_WORLD))

if (DVMShow && (MPI_CurrentProcIdent == 0))
    SYSTEM(printf, ("*** RTS (mpi_init):Processors performance measuring... (Loop count=%ld)\n",MesLoopCount))
  SYSTEM(MPI_Barrier, (APPL_COMM_WORLD))

  ProcTime = MPI_Wtime();

  {
    DvmType lng;
    int           int1=123,          int2=567;
    DvmType          long1 = 1234, long2 = 4321;
    float         float1=0.1234f,    float2=54321.0f;
    double        double1=7654321.0, double2=0.1234567;
    int           int3,              int4,              int5;
    DvmType          long3, long4, long5;
    float         float3,            float4,            float5;
    double        double3,           double4,           double5;
    int ArLoopCount=0;
//    MesLoopCount=PPMeasureCount;

    /* Current processor performance measuring */    /*E0076*/
    /* MesLoopCount = 1000000; */    /*E0077*/

//printf("MesLoop=%d\n",MesLoopCount);
    for(lng=0; lng < MesLoopCount; lng++)
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

    /*** LOOP ***/    /*e0078*/

    ProcTime = MPI_Wtime() - ProcTime + 0.000001;
/*
  #ifdef _WIN_MPI_
    ProcTime *= 1000.0;
  #endif
*/
  SYSTEM(MPI_Barrier, (APPL_COMM_WORLD))
if (DVMShow && (MPI_CurrentProcIdent == 0))
 
    SYSTEM(printf, ("*** RTS (mpi_init):Processors performance measuring done\n"))


/* b_alex
    SYSTEM(MPI_Allreduce, (&ProcTime, &ResTime, 1, MPI_DOUBLE, MPI_MIN,
                           APPL_COMM_WORLD))

    ArLoopCount = (int)(ResTime > PTime ? 1 : PTime/ResTime);
e_alex */    
    /*SYSTEM(printf, ("*** RTS (mpi_init): ArLoopCount=%d, "
                      "ProcTime=%f, ResTime=%lf, PTime=%lf\n",
                      ArLoopCount, ProcTime, ResTime, PTime))*/    /*e0079*/
/* b_alex

    ArithmLoopCount = (ArLoopCount+1) * MesLoopCount;

    if(ArLoopCount > 1)
    {
       SYSTEM(MPI_Barrier, (APPL_COMM_WORLD))

       ProcTime = MPI_Wtime();

       for(lng=0; lng < ArithmLoopCount; lng++)
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

  

       ProcTime = MPI_Wtime() - ProcTime + 0.000001;

       #ifdef _WIN_MPI_
          ProcTime *= 1000.0;
       #endif

    }
e_alex */    

  }

  /*SYSTEM(printf, ("*** RTS (mpi_init): %d: ProcTime=%lf\n",
                    MPI_CurrentProcIdent, ProcTime))*/    /*e0081*/

  if(ApplProcessCount != 0)
  {
     mac_malloc(ProcTimes, double *, ApplProcessCount*sizeof(double), 0);
  }

  SYSTEM(MPI_Allgather,
         (&ProcTime, 1, MPI_DOUBLE, ProcTimes, 1, MPI_DOUBLE,
          APPL_COMM_WORLD))

  if(MPI_CurrentProcIdent == 0)
  {
     /* excluding processors */    /*E0082*/

     SYSTEM_RET(fh, fopen, ("exclude_hosts", "r"))

     if(fh != NULL)
     {
        while(feof(fh) == 0)
        {
           p = Buf;

           if(fgets(p, 256, fh) == NULL)
              continue;

           if((p[strlen(p)-1] == '\n') || (p[strlen(p)-1] == '\r'))
              p[strlen(p)-1] = 0;

           if((p[strlen(p)-1] == '\n') || (p[strlen(p)-1] == '\r'))
              p[strlen(p)-1] = 0;

           if(p[0] == 0)
              continue;

           for(i=0; i < ApplProcessCount; i++)
           {
              if(strcmp(p, MPS_ProcNameList[ApplProcessNumber[i]]) == 0)
              {
                 MPS_ProcNameList[ApplProcessNumber[i]][0] = 0;
                 excl_procs++;

                 SYSTEM(printf,
                        ("*** RTS (mpi_init): excluded proc %d <%s> by "
                         "<exclude_hosts> with power time %lf\n",
                         i, p, ProcTimes[i]))
                 break;
              }
           }
        }

        SYSTEM(fclose, (fh))
     }

     if(P_Excl > 0)
     {
        int      ProcMax = 0;
        double   MaxTimeP;
        char     PName[256];
        int      ttt = P_Excl;

        while(ttt > 0)
        {
           MaxTimeP = -1.0;

           for(i=0; i < ApplProcessCount; i++)
           {
              if(MPS_ProcNameList[ApplProcessNumber[i]][0] == 0)
                 continue;

              if(ProcTimes[i] >= MaxTimeP)
              {
                 ProcMax = i;
                 MaxTimeP = ProcTimes[i];
              }
           }

           if(MaxTimeP == -1.0)
           {
              eprintf(__FILE__,__LINE__,
                    "*** RTS fatal err (mpi_init): ProcCount < 0 after "
                    "excluding\n");
           }

           SYSTEM(sprintf,
                  (PName, "%s",
                   MPS_ProcNameList[ApplProcessNumber[ProcMax]]))

           for(i=0; i < ApplProcessCount; i++)
           {
              if(MPS_ProcNameList[ApplProcessNumber[i]][0] == 0)
                 continue;

              if(strcmp(PName, MPS_ProcNameList[ApplProcessNumber[i]]) == 0)
              {
                 ttt--;

                 SYSTEM(printf,
                        ("*** RTS (mpi_init): excluded proc %d <%s> "
                         "with power time %lf\n",
                         i, MPS_ProcNameList[ApplProcessNumber[i]],
                         ProcTimes[i]))

                 MPS_ProcNameList[ApplProcessNumber[i]][0] = 0;
                 excl_procs++;

                 if(ttt == 0)
                    break;
              }
           }
        }
     }

     j = 0;

     for(i=0; i < ApplProcessCount; i++)
     {
        if(MPS_ProcNameList[ApplProcessNumber[i]][0] == 0)
           continue;

        MPI_NumberList[j++] = i;
     }

     for(i=0; i < ApplProcessCount; i++)
     {
        if(MPS_ProcNameList[ApplProcessNumber[i]][0] != 0)
           continue;

        MPI_NumberList[j++] = i;
     }

     /*
     if(MPI_CurrentProcIdent == 0)
        for(i=0; i < ApplProcessCount; i++)
            SYSTEM(printf, ("*** RTS (mpi_init): %d;  %d %lf \n",
                            MPI_NumberList[i], i, ProcTimes[i]))
     */    /*e0083*/

     /* reordering processors */    /*E0084*/

     j = 0;

     SYSTEM_RET(fh, fopen, ("mpi_hosts", "r"))

     if(fh != NULL)
     {
        while(feof(fh) == 0)
        {
           p = Buf;

           if(fgets(p, 256, fh) == NULL)
              continue;

           if((p[strlen(p)-1] == '\n') || (p[strlen(p)-1] == '\r'))
              p[strlen(p)-1] = 0;

           if((p[strlen(p)-1] == '\n') || (p[strlen(p)-1] == '\r'))
              p[strlen(p)-1] = 0;

           if(p[0] == 0)
              continue;

           for(i=0; i < ApplProcessCount; i++)
           {
              if(strcmp(p, MPS_ProcNameList[ApplProcessNumber[i]]) == 0)
              {
                 MPS_ProcNameList[ApplProcessNumber[i]][0] = 0;
                 break;
              }
           }

           if(i == ApplProcessCount)
              continue;

           MPI_NumberList[j++] = i;

           SYSTEM(printf,
                  ("*** RTS (mpi_init): %d: proc from <mpi_hosts> <%s> "
                   "with power time %lf\n", j-1, p, ProcTimes[i]))

           if(j == (ApplProcessCount - excl_procs))
              break;
        }

        fclose(fh);

        for(i=0; i < ApplProcessCount; i++)
        {
           if(MPS_ProcNameList[ApplProcessNumber[i]][0] != 0)
           {
             MPI_NumberList[j++] = i;

             SYSTEM(printf,
                    ("*** RTS (mpi_init): %d: other proc name=<%s> with "
                     "power time %lf\n",
                     j-1, MPS_ProcNameList[ApplProcessNumber[i]],
                     ProcTimes[i]))
           }
        }
     }
     else
     {
        if(Reorder)
        {
           int   k = 0;

           i = 0;

           while( i != (ApplProcessCount - excl_procs - 1))
           {
              for(i=k; i < (ApplProcessCount - excl_procs - 1); i++)
              {
                 if(ProcTimes[MPI_NumberList[i]] <=
                    ProcTimes[MPI_NumberList[i+1]])
                    continue;

                 {  int lll;

                    lll = MPI_NumberList[i];
                    MPI_NumberList[i] = MPI_NumberList[i+1];
                    MPI_NumberList[i+1]=lll;

                    if(i != 0)
                    {
                       k = i-1;
                       break;
                    }
                 }
              }
           }
        }

        for(i=0; i < (ApplProcessCount - excl_procs); i++)
        {
           if(MPS_ProcNameList[ApplProcessNumber[MPI_NumberList[i]]][0]
              != 0)
           {
             if(Reorder)
                SYSTEM(printf,
                ("*** RTS (mpi_init): %d: proc name=<%s>\n", i,
                 MPS_ProcNameList[ApplProcessNumber[MPI_NumberList[i]]]))
           }
        }
     }
  }

  SYSTEM(MPI_Bcast, (MPI_NumberList, ApplProcessCount, MPI_INT, 0,
                     APPL_COMM_WORLD))

  for(i=0; i < ApplProcessCount; i++)
  {
     if(MPI_NumberList[i] != MPI_CurrentProcIdent)
        continue;

     MPI_CurrentProcIdent = i;
     break;
  }

  if(i == ApplProcessCount)
  {
     eprintf(__FILE__,__LINE__,
             "*** RTS err (mpi_init): %d: system error "
             "in <setMPInumbers>\n",
             MPI_CurrentProcIdent);
  }
}


/* */    /*E0085*/

void mpi_msg_test(void)
{
  int             i;
  MPI_Status      Status;
  char           *sptr, *rptr, *sptr0, *rptr0;
  MPI_Request    *rReq0, *sReq0, *rReq, *sReq;
  int             proc;

  if(MPI_ProcCount != 2 || MPI_MsgTest == 0)
     return;

#ifdef _WIN_MPI_

  MPI_TestCount = 2;
  MPI_TestSize  = 10;

#endif  

  mac_malloc(sptr0, char *, MPI_TestCount*MPI_TestSize, 0);
  mac_malloc(rptr0, char *, MPI_TestCount*MPI_TestSize, 0);
  mac_malloc(rReq0, MPI_Request *, MPI_TestCount*sizeof(MPI_Request), 0);
  mac_malloc(sReq0, MPI_Request *, MPI_TestCount*sizeof(MPI_Request), 0);

  sptr = sptr0;
  rptr = rptr0;

  if(MPI_CurrentProcIdent == 0)
  {
     SYSTEM(printf, ("*** RTS (mpi_init): start of mpi test ***\n"))
     SYSTEM(printf,
            ("*** RTS (mpi_init): MPI_MsgTest=%d MPI_TestCount=%d "
             "MPI_TestSize=%d\n",
             MPI_MsgTest, MPI_TestCount, MPI_TestSize))
  }

  if(MPI_CurrentProcIdent == 0)
     proc = 1;
  else
     proc = 0;

  rReq = rReq0;
  sReq = sReq0;

  for(i=0; i < MPI_TestCount; i++)
  {
     SYSTEM(MPI_Irecv, (rptr, MPI_TestSize, MPI_BYTE, proc, 1,
                        DVM_COMM_WORLD, rReq))

     rptr += MPI_TestSize;
     rReq++;
  }

  for(i=0; i < MPI_TestCount; i++)
  {
     if(MPI_MsgTest > 1)
        SYSTEM(MPI_Issend, (sptr, MPI_TestSize, MPI_BYTE, proc, 1,
                            DVM_COMM_WORLD, sReq))
     else
        SYSTEM(MPI_Isend, (sptr, MPI_TestSize, MPI_BYTE, proc, 1,
                           DVM_COMM_WORLD, sReq))

     sptr += MPI_TestSize;
     sReq++;
  }

  rReq = rReq0;
  sReq = sReq0;

  for(i=0; i < MPI_TestCount; i++)
  {
     SYSTEM(MPI_Wait, (sReq, &Status))
     sReq++;
  }

  for(i=0; i < MPI_TestCount; i++)
  {
     SYSTEM(MPI_Wait, (rReq, &Status))
     rReq++;
  }

  if(MPI_CurrentProcIdent == 0)
     SYSTEM(printf, ("*** RTS (mpi_init): end of mpi test ***\n"))

  mac_free((void **)&sptr0);
  mac_free((void **)&rptr0);
  mac_free((void **)&rReq0);
  mac_free((void **)&sReq0);

  return;
}


#endif /* _MPI_INIT_C_ */    /*E0086*/
