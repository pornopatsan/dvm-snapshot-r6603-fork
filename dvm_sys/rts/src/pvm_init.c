#ifndef _PVM_INIT_C_
#define _PVM_INIT_C_
/******************/    /*E0000*/ 

#include "pvm3.h"

/************************\
* PVM RTL INIT procedure *
\************************/    /*E0001*/

long  rtl_init(long  InitParam, int  argc, char  *argv[])
{
  int   i;

  dvmclock = 1. / CLOCKS_PER_SEC;

  DVM_TIME1       = dvm_time();
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

  /* Check checksum of command memory */    /*E0002*/

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


  /* Initialize  PVM  library */    /*E0003*/

  SYSTEM_RET(CurrentProcIdent, pvm_mytid, ()) /* polling current subtask
                                               identifier */    /*E0004*/
  SYSTEM_RET(MasterProcIdent, pvm_parent, ()) /* polling main subtask 
                                               identifier */    /*E0005*/
  if(MasterProcIdent == PvmNoParent)
     MasterProcIdent = CurrentProcIdent;

  IsInit = 1;  /* MPS has been initialized */    /*E0006*/

  GetCurrentParName(argc, argv); /* input of current file name current.par
                                    from command line */    /*E0007*/
  GetActPar(argc, argv);   /* input flags of attachment of folders
                              and files with parameters from 
                              command line */    /*E0008*/
  GetDeactPar(argc, argv); /* input flags of detachment of folders
                              and files with parameters from
                              command line */    /*E0009*/
  if(DeactCurrentPar)
     GetInitialPS(argc, argv); /* input initial processor system
                                  dimension size from command line */    /*E0010*/

  GetFopenCount(argc, argv); /* input number of additional attempts
                                opening parameter files
                                from command line  */    /*E0011*/

  GetTraceFileName(argc, argv); /* input left hand side of new file
                                   names with user program trace and
                                   name of file with standard
                                   trace */    /*E0012*/

  dvm_InpCurrentPar(); /* input initial parameters */    /*E0013*/

  /* Form processor identifier array */    /*E0014*/

  ProcIdentList[0] = MasterProcIdent;

  if(ProcCount > 1)
  {
     if(CurrentProcIdent == MasterProcIdent)
     {
  	ProcIdentList[1] = 0;
        SYSTEM_RET(i, pvm_spawn, (argv[1], (char **)0, 0, "",
                                  ProcCount-1, &ProcIdentList[1]))
        if(i != ProcCount-1)
           eprintf(__FILE__,__LINE__,
                   "*** RTS fatal err 002.000: pvm_spawn "
                   "rc = %d\n", i);

        for (i = 1; i < ProcCount; i++)
        {
           char aba;
           int hhh;

           hhh = pvm_recv(ProcIdentList[i], 5);
           pvm_upkbyte(&aba, 1, 1);

           SYSTEM(spvm_send,(ProcIdentList, ProcCount, sizeof(long),
                             ProcIdentList[i]))
        }
     }
     else
     {
        char bbb;

        pvm_initsend(PvmDataDefault);
        bbb = 's';
        pvm_pkbyte(&bbb, 1, 1);
        pvm_send(MasterProcIdent, 5);

        SYSTEM(spvm_recv,(ProcIdentList, ProcCount, sizeof(long),
                          MasterProcIdent))
     }
  }

  dvm_CorrOut(); /* define and correct parameters for
                    information message output */    /*E0015*/

  /* Form internal numbers of current and main subtasks */    /*E0016*/

  for(MPS_CurrentProc=0; MPS_CurrentProc < ProcCount; MPS_CurrentProc++)
     if(ProcIdentList[MPS_CurrentProc] == CurrentProcIdent)
        break;

  if(MPS_CurrentProc == ProcCount)
     eprintf(__FILE__,__LINE__,
      "*** RTS fatal err 002.001: invalid  Current Proc Ident (%ld)\n",
      CurrentProcIdent);

  CurrentProcNumber = ProcNumberList[MPS_CurrentProc]; /* external number of
                                                          current processor */    /*E0017*/

  IsSlaveRun = 1;  /* offspring subtasks are running */    /*E0018*/

  dvm_InpSysPar();     /* input system parameters */    /*E0019*/

  dvm_SysInit();       /* install support system */    /*E0020*/

  dvm_TraceInit(); /*  initialize trace and 
                       built in debugging tools */    /*E0021*/

  if(VersStartPrint && _SysInfoPrint)
     PrintVers(VersFullStartPrint, 1); /* print version into screen */    /*E0022*/

  PrintVers(1, 0);                     /* print version into trace */    /*E0023*/

  if(RTL_TRACE)
     ( RTL_CALL, dvm_trace(DVM_Trace_Start,"\n"), DVM_RET );

  CheckRendezvousInit();    /* fix that only one exchange between 
                               two processors is allowed */    /*E0024*/

  /* Set count of received messages */    /*E0025*/

  MessageCount[16] = 1;

  ( RTL_CALL, dvm_Init(InitParam) ); /* DVMLIB initialization */    /*E0026*/

  UserStartTime = dvm_time();

  UserSumFlag = 1;          /* summarize time 
                               of user program execution */    /*E0027*/

  return 0;                 /* return to the user program */    /*E0028*/ 
}


#endif /* _PVM_INIT_C_ */    /*E0029*/
