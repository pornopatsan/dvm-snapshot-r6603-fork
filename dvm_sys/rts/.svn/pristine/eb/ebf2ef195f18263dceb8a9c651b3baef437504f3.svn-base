#ifndef _GNS_INIT_C_
#define _GNS_INIT_C_
/******************/    /*E0000*/ 

/************************\
* GNS RTL INIT procedure *
\************************/    /*E0001*/


#ifdef _DVM_GNS_
   void  gns_NMessage(int *); /* getting a number of received messages
                                 for optimisation of reduction */    /*e0002*/
#endif


long  rtl_init(long  InitParam, int  argc, char  *argv[])
/*
      Initialization in C program.
      ---------------------------- 
 
InitParamPtr - parameter of Run-Time Library initialization.
argc         - a number of string parameters in command line.
argv         - an array contained pointers to string parameters
               in command line.

The function initializes Run-Time Library internal structures
according to modes of interprocessor exchanges, statistic and
trace accumulation, and so on defined in configuration files.
Using zero as parameter implies the default initialization.
The function returns zero. 
*/    /*E0003*/
{
  int    i;

  dvmclock = 1. / CLOCKS_PER_SEC;

  DVM_TIME1 = dvm_time();
  SystemStartTime = DVM_TIME1;

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

  /* check checksum of command memory */    /*E0004*/

  if(CodeCheckSumPrint)
     SYSTEM(fprintf,(stderr,"CodeStartAddr=%lx;  CodeFinalAddr=%lx;  "
                            "CodeCheckSum=%lx;\n",
            dvm_CodeStartAddr,dvm_CodeFinalAddr,dvm_CodeCheckSum))

  if(dvm_CodeStartAddr && dvm_CodeFinalAddr &&
     dvm_CodeStartAddr <= dvm_CodeFinalAddr)
  {
     if(dvm_CodeCheckSum == 0)
        dvm_CodeCheckSum =
        dvm_CheckSum((unsigned short *)dvm_CodeStartAddr,
                     (unsigned short *)dvm_CodeFinalAddr);
     dvm_CodeCheckMem();
  }

  /* Initialization of GNS library */    /*E0005*/

  SYSTEM(gns_init,(0,"DVM"))

  SYSTEM_RET(CurrentProcIdent,gns_mytaskid,()) /* polling  of current
                                                  subtask  identifier */    /*E0006*/

  /* Initialization of initial system trace */    /*E0007*/

#ifndef _MVS_1000_16_
  argc = 0; /* ROUTER does not support command line !!! */    /*E0008*/
#endif
  systrace_set(argc, argv, CurrentProcIdent);

  SYSTEM_RET(MasterProcIdent,gns_master,()) /* polling of 
                                               main subtask identifier */    /*E0009*/

  sysprintf(__FILE__,__LINE__, "gns_master: MasterProcIdent=%d\n",
                                MasterProcIdent);

  IsInit = 1;     /* MPS has been initialized */    /*E0010*/

  GetCurrentParName(argc, argv); /* current file name current.par
                                    is input from the command line */    /*E0011*/
  GetActPar(argc, argv);   /* flags of swiching on directories and 
                              parameter files is input from the command line */    /*E0012*/
  GetDeactPar(argc, argv); /* flags of swiching off directories and 
                              parameter files is input from the command line */    /*E0013*/
  if(DeactCurrentPar)
     GetInitialPS(argc, argv); /* sizes of initial processor system 
                                  are input from the command line  */    /*E0014*/

  GetFopenCount(argc, argv); /* number of additional attempts to
                                open parameter file 
                                is input from the command line */    /*E0015*/

  GetTraceFileName(argc, argv); /* left part of new user program trace file name 
                                   and standard trace file name
                                   are input from the command line */    /*E0016*/

  sysprintf(__FILE__,__LINE__, "call dvm_InpCurrentPar\n");

  dvm_InpCurrentPar(); /* input initial parameters */    /*E0017*/

  sysprintf(__FILE__,__LINE__, "ret dvm_InpCurrentPar\n");

  /* Form array of processors identifiers */    /*E0018*/

  ProcIdentList[0] = MasterProcIdent;

  if(ProcCount > 1)
  {
     if(CurrentProcIdent == MasterProcIdent)
     {
        SYSTEM_RET(i, gns_newtask, ("DVM", ProcCount-1,
                                    &ProcNumberList[1],
                                    &ProcIdentList[1]))
        if(i != ProcCount-1)
           eprintf(__FILE__,__LINE__,
                   "*** RTS fatal err 003.000: gns_newtask rc = %d\n",
                   i);

        for(i=1; i < ProcCount; i++)
            SYSTEM(gns_send,(ProcIdentList[i], (char *)ProcIdentList,
                             ProcCount*sizeof(int)))
     }
     else
        SYSTEM(gns_receive,(MasterProcIdent, &i, (char *)ProcIdentList,
                            ProcCount*sizeof(int)))
  }

  dvm_CorrOut(); /* define and correct parameters for
                    information message output */    /*E0019*/

  /* Form internal numbers for current and main subtasks */    /*E0020*/

  MPS_MasterProc = 0;

  for(MPS_CurrentProc=0; MPS_CurrentProc < ProcCount; MPS_CurrentProc++)
      if(ProcIdentList[MPS_CurrentProc] == CurrentProcIdent)
         break;

  if(MPS_CurrentProc == ProcCount)
     eprintf(__FILE__,__LINE__,
       "*** RTS fatal err 003.001: invalid Current Proc Ident (%ld)\n",
       CurrentProcIdent);

  CurrentProcNumber = ProcNumberList[MPS_CurrentProc];/* external number 
                                                         of current processor */    /*E0021*/
  IsSlaveRun = 1; /* offspring subtasks are running */    /*E0022*/

  sysprintf(__FILE__,__LINE__, "call dvm_InpSysPar\n");

  dvm_InpSysPar();     /* input system parameters */    /*E0023*/

  sysprintf(__FILE__,__LINE__, "ret dvm_InpSysPar\n");

  sysprintf(__FILE__,__LINE__, "call dvm_SysInit\n");

  dvm_SysInit();       /* install support system */    /*E0024*/

  sysprintf(__FILE__,__LINE__, "ret dvm_SysInit\n");

  sysprintf(__FILE__,__LINE__, "call dvm_TraceInit\n");

  dvm_TraceInit();   /* initialize trace and 
                        built in debugging tools */    /*E0025*/

  sysprintf(__FILE__,__LINE__, "ret dvm_TraceInit\n");

  if(VersStartPrint && _SysInfoPrint)
     PrintVers(VersFullStartPrint, 1); /* print version into screen */    /*E0026*/

  PrintVers(1, 0);                     /* print version in trace */    /*E0027*/

  if(RTL_TRACE)
     ( RTL_CALL, dvm_trace(DVM_Trace_Start," \n"), DVM_RET );

  CheckRendezvousInit(); /* install that only one exchange 
                            between two processors is allowed */    /*E0028*/

  /* Set count of received messages */    /*E0029*/

  #ifdef _DVM_GNS_
     MessageCount[16] = 0;
     SYSTEM(gns_NMessage,(&MessageCount[16]))
  #else
     MessageCount[16] = 1;
  #endif

  sysprintf(__FILE__,__LINE__, "call dvm_Init\n");

  ( RTL_CALL, dvm_Init(InitParam) ); /* initialization of DVMLIB */    /*E0030*/

  sysprintf(__FILE__,__LINE__, "ret dvm_Init\n");

  UserStartTime = dvm_time();

  UserSumFlag = 1;             /* sum up time of user program */    /*E0031*/

  return 0;              /* return to the user program */    /*E0032*/ 
}


#endif /* _GNS_INIT_C_ */    /*E0033*/
