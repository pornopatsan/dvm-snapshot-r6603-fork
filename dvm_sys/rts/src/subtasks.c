#ifndef _SUBTASKS_C_
#define _SUBTASKS_C_
/******************/    /*E0000*/

/*******************************************\
* Functions to arrange parallel subtasks   * 
\*******************************************/    /*E0001*/


DvmType  __callstd  mapam_(AMRef  *AMRefPtr, PSRef  *PSRefPtr)
{ SysHandle      *VMSHandlePtr, *AMHandlePtr;
  s_VMS          *VMS, *PVMS, *OldVMS;
  s_AMS          *AMS, *PAMS, *CAMS;
  int             i, CrtEnvInd, CurrEnvInd;
  s_AMVIEW       *AMV;

#ifdef  _DVM_MPI_
  int             j, k, m, n;
  int            *ProcList;
  SysHandle      *VProc;
  double          time;
#endif

  StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0002*/
  DVMFTimeStart(call_mapam_);

  if(RTL_TRACE)
     dvm_trace(call_mapam_,
               "AMRefPtr=%lx; AMRef=%lx; PSRefPtr=%lx; PSRef=%lx;\n",
               (uLLng)AMRefPtr, *AMRefPtr, (uLLng)PSRefPtr, *PSRefPtr);
  if(TstObject)
  {  if(TstDVMObj(PSRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 065.000: wrong call mapam_\n"
                 "(the processor system is not a DVM object; "
                 "PSRef=%lx)\n", *PSRefPtr);
  }

  VMSHandlePtr = (SysHandle *)*PSRefPtr;

  if(VMSHandlePtr->Type != sht_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 065.001: wrong call mapam_\n"
           "(the object is not a processor system; PSRef=%lx)\n",
           *PSRefPtr);

  VMS = (s_VMS *)VMSHandlePtr->pP;

  if(TstObject)
  {  if(TstDVMObj(AMRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 065.003: wrong call mapam_\n"
                 "(the abstract machine is not a DVM object; "
                 "AMRef=%lx)\n", *AMRefPtr);
  }

  AMHandlePtr = (SysHandle *)*AMRefPtr;

  if(AMHandlePtr->Type != sht_AMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 065.004: wrong call mapam_\n"
          "(the object is not an abstract machine; AMRef=%lx)\n",
          *AMRefPtr);

  AMS = (s_AMS *)AMHandlePtr->pP;

  CAMS = (s_AMS *)CurrAMHandlePtr->pP;  /* current abstract machine */    /*E0003*/

  /* Chech if the given machine is a children of the current machine */    /*E0004*/

  NotDescendant(i, CAMS, AMS)

  if(i || AMS == CAMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 065.005: wrong call mapam_ "
              "(the given abstract machine is not a descendant "
              "of the current abstract machine; "
              "AMRef=%lx; CurrentAMRef=%lx)\n",
              (uLLng)AMHandlePtr, (uLLng)CAMS->HandlePtr);

  /* Check if representation,  which the given abstract machine
     belongs to in the current subtask, has been created  */    /*E0005*/
  
  CurrEnvInd = gEnvColl->Count - 1; /* current context index */    /*E0006*/
  CrtEnvInd  = AMS->CrtEnvInd;      /* index of context where both representation 
                                       and abstract machine have been created*/    /*E0007*/
  if(CrtEnvInd != CurrEnvInd)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 065.006: wrong call mapam_\n"
       "(the given abstract machine was not created "
       "by the current subtask;\n"
       "AMRef=%lx; AMEnvIndex=%d; CurrentEnvIndex=%d)\n",
       (uLLng)AMHandlePtr, CrtEnvInd, CurrEnvInd);

  AMV  = AMS->ParentAMView;             /* representation of
                                           parent abstract machine */    /*E0008*/
  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent abstract 
                                           machine */    /*E0009*/
  PVMS = PAMS->VMS;                     /* parent processor 
                                           system */    /*E0010*/

  /* Check if the parent abstract machine has been mapped */    /*E0011*/

  if(PVMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 065.007: wrong call mapam_ "
              "(the parental AM has not been mapped; "
              "AMRef=%lx; ParentAMViewRef=%lx; ParentAMRef=%lx)\n",
              (uLLng)AMHandlePtr, (uLLng)AMV->HandlePtr,
              (uLLng)PAMS->HandlePtr);

  OldVMS = AMS->VMS;

  if(OldVMS)
  {  /* Abstract machine has already been mapped */    /*E0012*/

     if(AMS->ParentAMView) /* is there parent AM representation? */    /*E0013*/
     {  if(AMS->ParentAMView->VMS) /* if AM belongs to representation
                                      mapped by distr_ function */    /*E0014*/
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 065.009: wrong call mapam_ "
                    "(the representation of the parental AM has "
                    "already been mapped by the function distr_; "
                    "AMRef=%lx; AMViewRef=%lx)\n",
                    *AMRefPtr, (uLLng)(AMS->ParentAMView->HandlePtr));

        if(AMS->SubSystem.Count) /* are there any children of
                                    the given AM */    /*E0015*/
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 065.010: wrong call mapam_\n"
                    "(the given abstract machine has descendants; "
                    "AMRef=%lx)\n", (uLLng)AMHandlePtr);
     }

     coll_Delete(&OldVMS->AMSColl, AMS); /* delete machines mapped on 
                                            the old processor system 
                                            from the list */    /*E0016*/
  }

  AMS->VMS = VMS; /* processor system on which
                     machine is mapped */    /*E0017*/
  coll_Insert(&VMS->AMSColl, AMS); /* to the list of machine
                                      mapped on processor system */    /*E0018*/

  AMS->IsMapAM = 1; /* */    /*E0019*/
  AMS->map_DVM_LINE = DVM_LINE[0]; /* */    /*E0020*/
  AMS->map_DVM_FILE = DVM_FILE[0]; /* */    /*E0021*/

  /* */    /*E0022*/

#ifdef  _DVM_MPI_

  if(VMS->Is_MPI_COMM == 0 && DVM_VMS->Is_MPI_COMM != 0)
  {
     VMS->Is_MPI_COMM = 1;
     i = (int)VMS->ProcCount;
     j = sizeof(int) * i;

     mac_malloc(ProcList, int *, j, 0);

     VProc = VMS->VProc;

     for(k=0; k < i; k++)
         ProcList[k] = (int)VProc[k].lP;

     if(RTL_TRACE && MPI_MapAMTrace && TstTraceEvent(call_mapam_))
     {  time = dvm_time();
     }

     MPI_Group_incl(DVM_VMS->ps_mpi_group, i, ProcList,
                    &VMS->ps_mpi_group);
     MPI_Comm_create(DVM_VMS->PS_MPI_COMM, VMS->ps_mpi_group,
                     &VMS->PS_MPI_COMM);

     if(RTL_TRACE && MPI_MapAMTrace && TstTraceEvent(call_mapam_))
     {  time = dvm_time() - time;
        tprintf("\n");
        tprintf("*** MPI_Comm Creating ***\n");
        tprintf("&MPI_Comm=%lx; &MPI_Group=%lx; ProcNumber=%d; "
                "time=%lf;\n",
                (uLLng)&VMS->PS_MPI_COMM, (uLLng)&VMS->ps_mpi_group, i,
                time);

        j = (int)ceil( (double)i / 10.); /* */    /*E0023*/
        tprintf("MPI_Group:");

        for(k=0,m=0,i--; k < j; k++)
        {  for(n=0; n < 10; n++,m++)
           {  tprintf(" %4d", ProcList[m]);

              if(m == i)
                 break;
           }
 
           tprintf("\n");
        } 
     }
     
     mac_free(&ProcList);
  }
  else
  {  if(RTL_TRACE && MPI_MapAMTrace && TstTraceEvent(call_mapam_))
     {  i = (int)VMS->ProcCount;

        tprintf("\n");
        tprintf("*** MPI_Comm Information ***\n");
        tprintf("&MPI_Comm=%lx; &MPI_Group=%lx; ProcNumber=%d; "
                "time=0.0;\n",
                (uLLng)&VMS->PS_MPI_COMM, (uLLng)&VMS->ps_mpi_group, i);

        j = (int)ceil( (double)i / 10.); /* */    /*E0024*/
        VProc = VMS->VProc;

        tprintf("MPI_Group:");

        for(k=0,m=0,i--; k < j; k++)
        {  for(n=0; n < 10; n++,m++)
           {  tprintf(" %4ld", VProc[m].lP);

              if(m == i)
                 break;
           }
 
           tprintf(" \n");
           tprintf("          ");
        } 
     }
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_mapam_," \n");

  StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0025*/
  DVMFTimeFinish(ret_mapam_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  runam_(AMRef  *AMRefPtr)
{ SysHandle      *AMHandlePtr, *ParentAMHandlePtr = NULL;
  DvmType            Res = 0;      /* the task not started */    /*E0026*/
  s_AMS          *AMS;
  s_VMS          *VMS;
  s_ENVIRONMENT  *NewEnv;

  StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0027*/
  DVMFTimeStart(call_runam_);

  if(RTL_TRACE)
     dvm_trace(call_runam_,"AMRefPtr=%lx; AMRef=%lx;\n",
                           (uLLng)AMRefPtr, *AMRefPtr);

  if(TstObject)
  {  if(TstDVMObj(AMRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 065.020: wrong call runam_ "
            "(the abstract machine is not a DVM object; "
            "AMRef=%lx)\n", *AMRefPtr);
  }

  AMHandlePtr = (SysHandle *)*AMRefPtr;

  if(AMHandlePtr->Type != sht_AMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 065.021: wrong call runam_\n"
         "(the object is not an abstract machine; AMRef=%lx)\n",
         *AMRefPtr);

  AMS = (s_AMS *)AMHandlePtr->pP;
  VMS = AMS->VMS;

  if(VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 065.022: wrong call runam_ "
            "(the subtask does not exist; AMRef=%lx)\n", *AMRefPtr);

  if(AMS->ParentAMView != NULL)
     ParentAMHandlePtr = AMS->ParentAMView->AMHandlePtr;

  if(ParentAMHandlePtr != CurrAMHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 065.023: wrong call runam_ "
           "(the subtask AM is not a daughter AM; "
           "AMRef=%lx; ParentAMRef=%lx # CurrentAMRef=%lx)\n",
           *AMRefPtr, (uLLng)ParentAMHandlePtr, (uLLng)CurrAMHandlePtr);

  if(VMS->HasCurrent)
  {  /* Current processor belongs to processor system
        of started subtask */    /*E0028*/ 

     NewEnv = env_Init(AMHandlePtr);    /* create new current
                                           context */    /*E0029*/
     coll_Insert(gEnvColl, NewEnv);

     AMS->EnvInd = gEnvColl->Count - 1; /* new current context
                                           index */    /*E0030*/
     AMHandlePtr->EnvInd = gEnvColl->Count - 1;

     /* Initialization of variables determining 
        the number of processors executing the current thread */    /*E0031*/

     OldEnvProcCount      = CurrEnvProcCount;
     NewEnvProcCount      = VMS->ProcCount;
     NewEnv->EnvProcCount = VMS->ProcCount;

     OldAMHandlePtr       = CurrAMHandlePtr;
     NewAMHandlePtr       = AMS->HandlePtr;

     Res = 1;  /* task is running */    /*E0032*/
  }

  if(_SysInfoPrint && SubTasksTimePrint)
  {
     AMS->RunTime = dvm_time();   /* */    /*E0033*/
  
     AMS->run_DVM_LINE = DVM_LINE[0]; /* */    /*E0034*/
     AMS->run_DVM_FILE = DVM_FILE[0]; /* */    /*E0035*/
  }

  if(RTL_TRACE)
     dvm_trace(ret_runam_,"Run=%ld;\n", Res);

  StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0036*/
  DVMFTimeFinish(ret_runam_);

  if(Res)
  {  CurrEnvProcCount    = NewEnvProcCount;
     CurrEnvProcCount_m1 = CurrEnvProcCount - 1;
     d1_CurrEnvProcCount = 1./CurrEnvProcCount;

     CurrAMHandlePtr     = NewAMHandlePtr;

     /* Inialization of variables determining
        the current processor system */    /*E0037*/

     DVM_VMS         = VMS;
     DVM_MasterProc  = VMS->MasterProc;
     DVM_IOProc      = VMS->IOProc;
     DVM_CentralProc = VMS->CentralProc;
     DVM_ProcCount   = VMS->ProcCount;
  }

  return  (DVM_RET, Res);
}



DvmType  __callstd  stopam_(void)
{ s_ENVIRONMENT  *Env;
  s_AMS          *AMS;
  s_VMS          *VMS;

  DVMFTimeStart(call_stopam_);

  /* Restore variables determining 
        the number of processors executing the current thread */    /*E0038*/

  CurrEnvProcCount    = OldEnvProcCount;
  NewEnvProcCount     = CurrEnvProcCount;
  CurrEnvProcCount_m1 = CurrEnvProcCount - 1;
  d1_CurrEnvProcCount = 1./CurrEnvProcCount;

  CurrAMHandlePtr = OldAMHandlePtr;

  /* Restore variables determining
     the current processor system */    /*E0039*/

  DVM_VMS = ((s_AMS *)CurrAMHandlePtr->pP)->VMS;

  DVM_MasterProc  = DVM_VMS->MasterProc;
  DVM_IOProc      = DVM_VMS->IOProc;
  DVM_CentralProc = DVM_VMS->CentralProc;
  DVM_ProcCount   = DVM_VMS->ProcCount;

  if(RTL_TRACE)
     dvm_trace(call_stopam_," \n");

  if(gEnvColl->Count == 1)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 065.030: wrong call stopam_ "
            "(the current subtask is the initial task)\n");

  Env = genv_GetCurrEnv();

  if(Env->ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 065.031: wrong call stopam_ "
            "(the current context is a parallel loop)\n");

  if(_SysInfoPrint && SubTasksTimePrint)
  {
     AMS = (s_AMS *)(Env->AMHandlePtr->pP); /* */    /*E0040*/
     AMS->ExecTime += dvm_time() - AMS->RunTime; /* */    /*E0041*/
     AMS->stop_DVM_LINE = DVM_LINE[0]; /* */    /*E0042*/
     AMS->stop_DVM_FILE = DVM_FILE[0]; /* */    /*E0043*/
  }

  /* Restore previous context */    /*E0044*/

  coll_Free(gEnvColl, Env);
  Env = genv_GetEnvironment(gEnvColl->Count - 2);

  if(Env)
  {  OldEnvProcCount = Env->EnvProcCount;
     OldAMHandlePtr  = Env->AMHandlePtr;
  }

  /* Restore variables determining
     the current processor system */    /*E0045*/

  Env = genv_GetCurrEnv();/* new (restored) current context */    /*E0046*/

  AMS = (s_AMS *)(Env->AMHandlePtr->pP); /* abstract machine of
                                            new context */    /*E0047*/
  VMS = AMS->VMS; /* processor system of new context */    /*E0048*/

  DVM_VMS = VMS;
  DVM_MasterProc = VMS->MasterProc;
  DVM_IOProc = VMS->IOProc;
  DVM_CentralProc = VMS->CentralProc;
  DVM_ProcCount = VMS->ProcCount;

  if(RTL_TRACE)
     dvm_trace(ret_stopam_," \n");

  DVMFTimeFinish(ret_stopam_);
  return  (DVM_RET, 0);
}


#endif   /*  _SUBTASKS_C_  */    /*E0049*/
