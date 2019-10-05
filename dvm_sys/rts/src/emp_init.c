#ifndef _EMP_INIT_C_
#define _EMP_INIT_C_
/******************/    /*E0000*/ 

/**************************\
* EMPTY RTL INIT procedure *
\**************************/    /*E0001*/

long rtl_init(long  InitParam, int  argc, char  *argv[])
/*
     Initialization in C program.
     ----------------------------
 
InitParamPtr - parameter of Run-Time Library initialization.
argc         - a number of string parameters in command line.
argv         - an array contained pointers to string parameters
               in command line.

The function initializes Run-Time Library internal structures according
to modes of interprocessor exchanges, statistic and trace accumulation,
and so on defined in configuration files.
Using zero as parameter implies the default initialization.
The function returns zero. 
*/    /*E0002*/
{
  int i;
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

  /* checksum of command memory */    /*E0003*/

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

  IsInit=1; /* attribute: MPS has been initialized */    /*E0004*/   

  GetCurrentParName(argc, argv); /* current file name ( current.par) is input 
                                    from the command line */    /*E0005*/

  GetActPar(argc, argv);   /* flags of using directories and files
                              with parameters are input 
                                    from the command line */    /*E0006*/
  GetDeactPar(argc, argv); /* flags of non using directories and files
                              with parameters are input 
                                    from the command line */    /*E0007*/

  if(DeactCurrentPar)
     GetInitialPS(argc, argv); /* sizes of initial processor system 
                                  are input from the command line */    /*E0008*/

  GetFopenCount(argc, argv); /* number of additional attempts 
                                to open parameter files 
                                is input from the command line */    /*E0009*/

  GetTraceFileName(argc, argv); /* left part of new file names  
                                for tracing user program 
                                name of file with standard trace  
                                are input from the command line */    /*E0010*/

  dvm_InpCurrentPar(); /* input initial parameters */    /*E0011*/

  dvm_CorrOut(); /* define and correct parameters 
                    for information message output */    /*E0012*/

  if(ProcCount != 1) 
     eprintf(__FILE__,__LINE__,
             "*** RTS err: wrong ProcCount (=%ld)\n",ProcCount);

  /* Form array of processor identifiers */    /*E0013*/

  ProcIdentList[0] = 0;
  IsSlaveRun = 1;    /* attribute: offspring tasks are running */    /*E0014*/

  dvm_InpSysPar();     /* input system parameters */    /*E0015*/

  dvm_SysInit();       /* install support system */    /*E0016*/

  dvm_TraceInit();   /* initialize trace and
                        built in debugging tools */    /*E0017*/

  if(VersStartPrint && _SysInfoPrint)
     PrintVers(VersFullStartPrint,1);  /* print version on screen */    /*E0018*/

  PrintVers(1,0);                      /* print version on trace */    /*E0019*/

  if(RTL_TRACE)
     ( RTL_CALL, dvm_trace(DVM_Trace_Start," \n"), DVM_RET );

  CheckRendezvousInit();      /* fix that only one exchange between 
                                 two processors is allowed */    /*E0020*/

  /* Set count of received messages */    /*E0021*/

  MessageCount[16] = 1;

  ( RTL_CALL, dvm_Init(InitParam) ); /* initialization of DVMLIB */    /*E0022*/

  UserStartTime = dvm_time(); 

  UserSumFlag = 1;            /* attribute: to summarize time
                                 of user program execution */    /*E0023*/

  return 0;                   /* return to user program */    /*E0024*/
}


#endif /* _EMP_INIT_C_ */    /*E0025*/

