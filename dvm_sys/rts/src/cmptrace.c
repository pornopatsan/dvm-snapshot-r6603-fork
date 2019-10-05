
#ifndef _CMPTRACE_C_
#define _CMPTRACE_C_

/******************/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#ifdef _UNIX_
    #include <signal.h>
    #include <sys/resource.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

/******************************************\
*  Functions to process trace information  *
\******************************************/

/*********************************************************************/

#ifdef _UNIX_
void sig_handler(int signum, siginfo_t *siginfo, void *context)
{
    char    buf[MAX_NUMBER_LENGTH];
    static char first=1;
    DvmType    res = -signum;

    pprintf(3, "*** RTS err: Caught signal %d at point %s, file %s, line %ld.\n",
            signum, to_string(&Trace.CurPoint, buf), DVM_FILE[0], DVM_LINE[0]);

    if ( first )
    {
        first = 0;

        if ( EnableTrace )
            error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_TR_TERM_BY_SIGNAL, signum, buf);

        lexit_(&res);
        /* no code will be executed after lexit_. */
    }
}
#else
DvmType WINAPI myUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    char    buf[MAX_NUMBER_LENGTH];
    static char first=1;
    DvmType    res = -222;

    pprintf(3, "*** RTS err: Unhandled exception at point %s, file %s, line %ld.\n",
            to_string(&Trace.CurPoint, buf), DVM_FILE[0], DVM_LINE[0]);

    if ( first )
    {
        first = 0;

        if ( EnableTrace )
            error_CmpTraceExt( -1, DVM_FILE[0], DVM_LINE[0], ERR_TR_TERM_BY_SIGNAL, -222, buf);
        lexit_(&res);
        /* no code will be executed after lexit_. */
    }

    /* stop exception processing */
    return NULL;
}
#endif

void cmptrace_Init(void)
{
    int size;
    char FileName[MaxPathSize + 1];
    ANY_RECORD* pErrRecord;
    char fName[] = "0.!!!";

#ifdef _SIGACTION_
#ifdef _UNIX_
	struct sigaction new_action;
    struct rlimit rl;

    if (TraceOptions.SetCoreSizeMax )
    {
        /* Obtain the current core file limit */
        getrlimit (RLIMIT_CORE, &rl);

        /* Set a core file limit of maximum allowed */
        rl.rlim_cur = rl.rlim_max;
        setrlimit (RLIMIT_CORE, &rl);
    }

    if ( !dvm_OneProcSign || (mode_COMPARETRACE != TraceOptions.TraceMode))
    {
        /* signal handling */
        new_action.sa_sigaction = sig_handler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = SA_SIGINFO /*| SA_ONESHOT*/;

        if(sigaction(SIGSEGV, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGSEGV);
        }

        if(sigaction(SIGHUP, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGHUP);
        }

        if(sigaction(SIGINT, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGINT);
        }

        if(sigaction(SIGQUIT, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGQUIT);
        }

        if(sigaction(SIGILL, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGILL);
        }

        if(sigaction(SIGABRT, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGABRT);
        }

        if(sigaction(SIGFPE, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGFPE);
        }

        /*if(sigaction(SIGKILL, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGKILL);
        } */

        if(sigaction(SIGINT, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGINT);
        }

        if(sigaction(SIGPIPE, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGPIPE);
        }

        if(sigaction(SIGALRM, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGALRM);
        }

        if(sigaction(SIGTERM, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGTERM);
        }

    /*    if(sigaction(SIGUSR1, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGUSR1);
        }*/

        if(sigaction(SIGUSR2, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGUSR2);
        }

        if(sigaction(SIGTRAP, &new_action, NULL) == -1)
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Can not set signal %d handler, sigaction() failed.\n", SIGTRAP);
        }
    }
    /* SIGSEGV SIGHUP SIGINT SIGQUIT SIGILL SIGABRT SIGFPE SIGKILL SIGINT SIGPIPE SIGALRM SIGTERM SIGUSR1 SIGUSR2 SIGTRAP */
#else
    //Trace.previousFilter =
      //  SetUnhandledExceptionFilter(myUnhandledExceptionFilter);
    if ( !dvm_OneProcSign  || (mode_COMPARETRACE != TraceOptions.TraceMode))
    {
        SetErrorMode(0);
        SetUnhandledExceptionFilter(NULL);
    }
#endif
#endif

    Trace.vtr = NULL;
    Trace.ctrlIndex = -1;
    mkStack(&Trace.sLoopsInfo, 10);
    Trace.convMode = -1;
    Trace.tmpPL    = NULL;
    Trace.EnableDbgTracingCtrl = 0;
    Trace.EnableLoopsPartitioning = 1;
    Trace.CovInfo = NULL;
    Trace.CodeCovWritten = 0;
    num_init( &(Trace.CurPoint) );
    Trace.TerminationStatus = 0;

    if (EnableCodeCoverage)
    {
        mac_malloc(Trace.CovInfo, COVERAGE_INFO *, sizeof(COVERAGE_INFO), 0);

        Trace.CovInfo->LastUsedFile = 0;

        for (size=0; size < MaxSourceFileCount; size++)
        {
            Trace.CovInfo->FileNames[size][0] = 0;
            Trace.CovInfo->AccessInfoSizes[size] = 0;
            Trace.CovInfo->LinesAccessInfo[size] = NULL;
        }
    }

#if defined(DOSL_TRACE) || !defined(NO_DOPL_DOPLMB_TRACE)
    *((char *)(&fName[0])) += (dvm_OneProcSign?dvm_OneProcNum:MPS_CurrentProc) ;
    SYSTEM_RET(Trace.DoplMBFileHandle, fopen, (fName, OPENMODE(w)))
#endif

    /* -------------------------------------------- */

    if (EnableDynControl || EnableTrace)
    {
        /* TraceOptions.ErrorFile is now task-dependent */
           SYSTEM(sprintf, (TraceOptions.ErrorFile, "%s.err.trd", dvm_argv[0] ));
    }

    if (EnableTrace)  // when run with dynctrl will not remove unnecessary files
    {
        Trace.EnableDbgTracingCtrl = 1;
        Trace.MatchedEvents = 0;
        Trace.TraceMarker   = 1;
        Trace.CurCPUNum = dvm_OneProcSign?dvm_OneProcNum:MPS_CurrentProc;
        Trace.RealCPUCount  = dvm_OneProcSign?dvm_OneProcCount:MPS_ProcCount;
        TraceOptions.MaxErrors /= Trace.RealCPUCount;

        // remove old protocols && extra-CPU protocols && all error protocols
        if ( Trace.CurCPUNum == 0 ) /* io_proc CPU is zero CPU */
        {
            char FileName2[128];
            int i = 0, res = 0;

            while (res == 0)
            {
                SYSTEM(sprintf, (FileName2, "%s%s.%d.prot.trd", TraceOptions.TracePath,
                            dvm_argv[0], i));
                i++;
                SYSTEM_RET(res, remove, (FileName2))
            }

            SYSTEM(sprintf, (FileName2, "%s%s.finerr.trd", TraceOptions.TracePath, dvm_argv[0]));
            SYSTEM_RET(res, remove, (FileName2))

            SYSTEM(sprintf, (FileName2, "%s%s.finprot.trd", TraceOptions.TracePath, dvm_argv[0]));
            SYSTEM_RET(res, remove, (FileName2))

            // err.trd file is removed later in this function
        }

        if ( TraceOptions.TraceMode == mode_WRITETRACE || TraceOptions.TraceMode == mode_CONFIG_WRITETRACE )
        {
            struct tm *newtime;
            time_t aclock;

            /* generate an instance ID (use current time) */
            time( &aclock );   // Get time in seconds
            newtime = localtime( &aclock );   // Convert time to struct tm form

            /* store local time as a string */
            SYSTEM(strncpy, (Trace.TraceTime, asctime( newtime ), 25));
            Trace.TraceTime[24]=0;

            if ( MPS_ProcCount > 1 )
                ( RTL_CALL, mps_Bcast(Trace.TraceTime, 25, 1) );
        }

        table_Init( &DelayTrace, 10, sizeof( struct tag_DELAY_TRACE ), trc_DelayTraceDestruct );
        vartable_Init(&ReductVarTable, TraceOptions.ReductVarTableSize, TraceOptions.ReductHashIndexSize, TraceOptions.ReductHashTableSize, GlobalHashFunc);
        error_Init( &TraceCompareErrors, TraceOptions.MaxErrors );

        size = 0;
        size =  dvm_max( size, sizeof( ITERATION ) );
        size =  dvm_max( size, sizeof( STRUCT_BEGIN ) );
        size =  dvm_max( size, sizeof( STRUCT_END ) );
        size =  dvm_max( size, sizeof( VARIABLE ) );
        size =  dvm_max( size, sizeof( SKIP ) );
        size =  dvm_max( size, sizeof( CHUNK ) );

        table_Init(&Trace.tTrace, TraceOptions.TableTraceSize, size, NULL);
        table_Init(&Trace.tStructs, 10, sizeof(struct tag_STRUCT_INFO), trc_InfoDone);

        hash2_Init(&Trace.hIters, TraceOptions.HashIterIndex, TraceOptions.HashIterSize, StandartHashCalc);

        table_Init(&Trace.tArrays, 10, sizeof(struct tag_ARRAY_INFO), NULL);
        hash1_Init(&Trace.hArrayPointers, TraceOptions.ReductHashIndexSize, TraceOptions.ReductHashTableSize, GlobalHashFunc);
        Trace.lNextArray = 0;

#ifdef _MPI_PROF_TRAN_
        Trace.ErrTimes = NULL;
#endif

        if (mode_COMPARETRACE == TraceOptions.TraceMode)
        {
            table_Init(&Trace.tVarErrEntries, 30, sizeof(struct tag_ERR_INFO), NULL);
#ifdef _MPI_PROF_TRAN_
            if ( dvm_OneProcSign  )
            {
                table_Init(&Trace.tMessages, 150, sizeof(struct tag_MSG_INFO), NULL);
            }
#endif
        }
        else
        {
            Trace.tVarErrEntries.IsInit = 0;
        }
        pErrRecord = table_GetNew(ANY_RECORD, &Trace.tTrace);
        pErrRecord->RecordType = (byte)trc_ERROR;

        Trace.pCurCnfgInfo = NULL;
        Trace.CurStruct = -1;
        Trace.CurIter = -1;
        Trace.Level = 0;
        Trace.TrcFileHandle = NULL;

        Trace.Bytes = 0;
        Trace.StrCount = 0;
        Trace.Iters = 0;
        Trace.StartPoint = NULL;
        Trace.FinishPoint = NULL;
        Trace.IterControl = 0;
        Trace.IterCtrlInfo = NULL;
        Trace.ArrWasSaved = 0;
        Trace.inParLoop = 0;
        Trace.ErrorIsFixed = 0;

        /* internal state of trace merger */
        Trace.Mode = -1;
        table_Init(&Trace.tSkipped, 10, size, NULL);
        hash1_Init(&Trace.hSkippedPointers, TraceOptions.ReductHashIndexSize, TraceOptions.ReductHashTableSize, GlobalHashFunc);

        if ( TraceOptions.StrictCompare )
        {
            trc_CompareValue = trc_CompareValueExact;
        }
        else
        {
            if ( TraceOptions.ExpIsAbsolute )
                    trc_CompareValue = trc_CompareValueAbsolute;
            else
                    trc_CompareValue = trc_CompareValueRelative;
        }

        list_init( &(Trace.CurArrList) );
        list_init( &(Trace.AuxArrList) );

        Trace.DoublePrecision = (unsigned) ceil(1 + DBL_MANT_DIG * log10(2.));
        Trace.FloatPrecision  = (unsigned) ceil(1 + FLT_MANT_DIG * log10(2.));

        if ( TraceOptions.StartPoint[0] != 0 )
        {
            mac_malloc(Trace.StartPoint, NUMBER *, sizeof(NUMBER), 0);
            if ( !parse_number(TraceOptions.StartPoint, Trace.StartPoint) )
            {
                    EnableTrace = 0;
                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: Invalid start point value: TraceOptions."
                                "StartPoint = '%s'.\n", TraceOptions.StartPoint);
            }
        }

        if ( TraceOptions.FinishPoint[0] != 0 )
        {
            mac_malloc(Trace.FinishPoint, NUMBER *, sizeof(NUMBER), 0);
            if ( !parse_number(TraceOptions.FinishPoint, Trace.FinishPoint) )
            {
                    EnableTrace = 0;
                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: Invalid finish point value: TraceOptions."
                                "FinishPoint = '%s'.\n", TraceOptions.FinishPoint);
            }
        }

        switch (TraceOptions.TraceLevel)
        {
            case level_CHECKSUM :
            case level_FULL :
            case level_MODIFY :
            case level_MINIMAL :
            case level_NONE :
                break;
            default :
                TraceOptions.TraceLevel = level_NONE;
        }

        Trace.ErrCode = SUCCESS;

        if(TraceOptions.AppendErrorFile == 0)
        {
           SYSTEM(sprintf, (FileName, "%s%s", TraceOptions.TracePath, TraceOptions.ErrorFile ));
           SYSTEM(remove, (FileName));
        }

        TraceInit = 1;

        pCmpOperations = &putTable;
        cmptrace_Read();

        if ( Trace.StartPoint && Trace.FinishPoint )
        {
            int k = num_cmp(Trace.StartPoint, Trace.FinishPoint);

            if ( k >= 0 )
            {
                /* start point is not less than finish point */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Start point is not less than finish point.\n");
            }

            if ( k == -2 )
            {
                /* start and finish points are incomparable values */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Start and finish points are incomparable values.\n");
            }
        }

        if ( TraceOptions.RelCompareMin <= 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.RelCompareMin value.\n");
        }

        if ( TraceOptions.DisableRedArrays != 0 )
        {
                /* issue a warning */
                pprintf(3, "*** RTS warning: TraceOptions.DisableRedArrays is activated. False errors can be found.\n");
        }

        if ( TraceOptions.SRCLocCompareMode > 3 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.SRCLocCompareMode value.\n");
        }

        if ( TraceOptions.Ig_left != -1 && TraceOptions.Ig_left < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Ig_left value.\n");
        }

        if ( TraceOptions.Ig_right != -1 && TraceOptions.Ig_right < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Ig_right value.\n");
        }

        if ( TraceOptions.Iloc_left != -1 && TraceOptions.Iloc_left < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Iloc_left value.\n");
        }

        if ( TraceOptions.Iloc_right != -1 && TraceOptions.Iloc_right < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Iloc_right value.\n");
        }

        if ( TraceOptions.Irep_left != -1 && TraceOptions.Irep_left < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Irep_left value.\n");
        }

        if ( TraceOptions.Irep_right != -1 && TraceOptions.Irep_right < 0 )
        {
                /* incorrect value */
                EnableTrace = 0;
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                         "*** RTS err: CMPTRACE: Incorrect parameter TraceOptions.Irep_right value.\n");
        }

        /* for futher removal */
        if ( TraceOptions.Irep_right != -1 )
        {
            pprintf(3, "*** RTS warning: CMPTRACE: Parameter TraceOptions.Irep_right is reserved for further versions;"
                        " it is switched off for this run.\n");
            TraceOptions.Irep_right = -1;
        }

        if ( TraceOptions.Ig_left != -1 || TraceOptions.Ig_right != -1 || TraceOptions.Iloc_left != -1 ||
             TraceOptions.Iloc_right != -1 || TraceOptions.Irep_left != -1 || TraceOptions.Irep_right != -1 )
        {
             Trace.IterControl = 1;
        }

        if(TraceOptions.TraceLevel == level_CHECKSUM)
        {
            TraceOptions.drarr=1;
            TraceOptions.CalcChecksums=0;
            DisArrayFill=1;
        }

        if ( TraceOptions.CalcChecksums == 1 )
        {
            TraceOptions.drarr=1;
            DisArrayFill=1;
        }

        if (TraceOptions.ChecksumMode >= 3)
        {
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                        "*** RTS err: CMPTRACE: Incorrect parameter value: TraceOptions.ChecksumMode = %d.\n",
                        TraceOptions.ChecksumMode);
        }

        if (TraceOptions.ChecksumMode == 0)
        {
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                        "*** RTS err: CMPTRACE: TraceOptions.ChecksumMode = 0 is not supported yet.\n");
        }

        if ( TraceOptions.ChecksumDisarrOnly > 1 )
        {
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                        "*** RTS err: CMPTRACE: Incorrect parameter value: TraceOptions.ChecksumDisarrOnly = %d.\n",
                        TraceOptions.ChecksumDisarrOnly);
        }

        if ( TraceOptions.ChecksumDisarrOnly == 0 )
        {
            pprintf(3, "*** RTS warning: CMPTRACE: With TraceOptions.ChecksumDisarrOnly = 0 "
             "differences in local checksums of replicated arrays are not checked yet.\n");
        }

        if ( TraceOptions.SeqLdivParContextOnly > 1 )
        {
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                        "*** RTS err: CMPTRACE: Incorrect parameter value: TraceOptions.SeqLdivParContextOnly = %d.\n",
                        TraceOptions.SeqLdivParContextOnly);
        }

        if (TraceOptions.TrapArraysAnyway > 1)
        {
                    EnableTrace = 0;
			        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: Incorrect parameter value: TraceOptions.TrapArraysAnyway = %d.\n",
                                TraceOptions.TrapArraysAnyway);
        }

        if (TraceOptions.TrapArraysAnyway != 0 && mode_COMPARETRACE == TraceOptions.TraceMode)
            TraceOptions.TrapArraysAnyway = 0;

        if (TraceOptions.MultidimensionalArrays)
            trc_ArrayCanTrace = trc_ArrayCanTraceMD;
        else
            trc_ArrayCanTrace = trc_ArrayCanTraceSD;

        if (TraceOptions.DefaultArrayStep < 1)
            TraceOptions.DefaultArrayStep = 1;

        if (TraceOptions.DefaultIterStep < 1)
            TraceOptions.DefaultIterStep = 1;

        if (mode_COMPARETRACE == TraceOptions.TraceMode)
        {
            pCmpOperations = &cmpTable;
        }
        else
            pCmpOperations = &putTable;

        if( ( TraceOptions.TraceMode == mode_WRITETRACE ||
              TraceOptions.TraceMode == mode_CONFIG_WRITETRACE
            ) && TraceOptions.SaveThroughExec != 0 )
        {
            SYSTEM(sprintf, (FileName, "%s%s%d.%s",
                TraceOptions.TracePath, TraceOptions.OutputTracePrefix,
                Trace.CurCPUNum, TraceOptions.Ext));
            SYSTEM(remove, (FileName));
            SYSTEM_RET(Trace.TrcFileHandle, fopen, (FileName, OPENMODE(w)));
            CurrCmpTraceFileSize = 0;

            if( Trace.TrcFileHandle == NULL )
            {
                TraceOptions.SaveThroughExec = 0;
            }
            else
            {
                CurrCmpTraceFileSize += trc_wrt_header(&Trace.tStructs, 0, Trace.TrcFileHandle, 0);
            }
        }
    }
}

void cmptrace_ReInit(void)
{
    Trace.ErrCode = SUCCESS;

    Trace.pCurCnfgInfo = NULL;
    Trace.CurStruct = -1;
    Trace.CurIter = -1;
    Trace.CurTraceRecord = 1;
    Trace.CurPreWriteRecord = -1;
    Trace.ReductExprType = 0;

    Trace.lNextArray = 0;
    Trace.Mode = -1;
}

void cmptrace_Done(void)
{
    LOOP_INFO *loop = NULL;

    if (TraceInit)
        error_CmpTracePrintAll();

    if (EnableTrace && NULL != Trace.pCurCnfgInfo)
    {

        // commented out 20.04.08 for errorless finish of programs with STOP inside of a loop
//        pprintf(3, "*** RTS err: CMPTRACE: No loop exit has been detected for loop %ld at program finish time. "
//            "File: %s, Begin Line: %ld.\n",
//            Trace.pCurCnfgInfo->No, Trace.pCurCnfgInfo->File, Trace.pCurCnfgInfo->Line);

        // not usable code on 20.04.08. May be removed or may be useful some time later
//        while (EnableTrace && NULL != Trace.pCurCnfgInfo)
//        {
//            pprintf(3, "*** RTS err: CMPTRACE: No loop exit has been detected for loop %d. "
//                "File: %s, Begin Line: %ld.\n",
//                Trace.pCurCnfgInfo->No, Trace.pCurCnfgInfo->File, Trace.pCurCnfgInfo->Line);

//            if ( Trace.vtr && !isEmpty(&Trace.sLoopsInfo))
//            {
//                loop = stackLast(&Trace.sLoopsInfo);
//                /* the loop has been exited using goto... we need to correct this !! */
//                tmp = loop->Line;
//                //DBG_ASSERT(__FILE__, __LINE__, !isEmpty (&Trace.sLoopsInfo) ); this is performed in stackLast function
//                dendl_(&loop->No, &tmp); /* necessary stack changes will be automatically produced */
//            }
//            else
//                EnableTrace = 0; /* can not fix trace saving in this case */
//        }
    }

    if (EnableTrace)
    {
        if (DynDebugPrintStatistics && MPS_CurrentProc == DVM_IOProc &&
            DbgInfoPrint && _SysInfoPrint)
        {
            pprintf(0, "*** Trace statistics ***\n" );
            pprintf(0, "Trace records : %lu\n", (UDvmType)table_Count(&Trace.tTrace));
            pprintf(0, "Trace element size : %lu byte(s)\n", (UDvmType)Trace.tTrace.ElemSize);
            pprintf(0, "Trace size in memory : %lu byte(s)\n", (UDvmType)(Trace.tTrace.ElemSize * table_Count(&Trace.tTrace)));
            pprintf(0, "*** Iteration hash-table ***\n" );
            hash_PrintStatistics(&Trace.hIters);
            pprintf(0, "*** Array hash-table ***\n" );
            hash_PrintStatistics(&Trace.hArrayPointers);
        }

        cmptrace_Write(TRACE_DONE);
    }

    if ( EnableCodeCoverage && !Trace.CodeCovWritten )
    {
        FILE *hf;
        char FileName[128];

        SYSTEM(sprintf,(FileName, "%s%s%d.%s", TraceOptions.TracePath,
                        TraceOptions.FileLoopInfo, Trace.CurCPUNum, TraceOptions.Ext));
        SYSTEM(remove, (FileName));

        SYSTEM_RET(hf, fopen, (FileName, OPENMODE(w)));

        if (hf != NULL)
        {
            trc_wrt_codecoverage(hf);
            SYSTEM(fclose, (hf));
        }
        else
        {
            pprintf(3, "*** RTS err: CMPTRACE: Can't open file %s for writing\n", FileName);
        }
    }

    if (((EnableTrace && mode_COMPARETRACE == TraceOptions.TraceMode) || EnableDynControl)
          && (!(EnableTrace && mode_COMPARETRACE == TraceOptions.TraceMode) || (TraceCompareErrors.ErrCount == 0 && !Trace.TerminationStatus))
          && (!EnableDynControl || (DynControlErrors.ErrCount == 0)))
        pprintf(3, "*** DVM debugger: No errors detected\n");

    if (TraceInit)
    {
        table_Done(&DelayTrace);
        vartable_Done(&ReductVarTable);
        error_Done(&TraceCompareErrors);
        table_Done(&Trace.tStructs);
        hash_Done(&Trace.hArrayPointers);
        table_Done(&Trace.tArrays);
        hash_Done(&Trace.hIters);
        table_Done(&Trace.tTrace);
        stack_destroy(&Trace.sLoopsInfo);

        if (mode_COMPARETRACE == TraceOptions.TraceMode)
        {
            table_Done(&Trace.tVarErrEntries);
        }

        if (Trace.TrcFileHandle != NULL)
        {
            if( EnableTrace && ( TraceOptions.TraceMode == mode_WRITETRACE ||
                  TraceOptions.TraceMode == mode_CONFIG_WRITETRACE
                ) && TraceOptions.SaveThroughExec != 0 )
                trc_wrt_end(Trace.TrcFileHandle);

            SYSTEM(fclose, (Trace.TrcFileHandle));
            Trace.TrcFileHandle = NULL;
        }
    }

    if ( Trace.CovInfo )
        coverage_Done();

    EnableTrace = EnableCodeCoverage = TraceInit = 0;

#if  !defined(NO_DOPL_DOPLMB_TRACE) || defined (DOSL_TRACE)
    fflush(Trace.DoplMBFileHandle);
    SYSTEM(fclose, (Trace.DoplMBFileHandle))
#endif
}


void cmptrace_Read(void)
{
    DVMFILE* hf;
    FILE *f;
    UDvmType StrCount;
    char FileName[200];
    int i, k;

    Trace.ErrCode = SUCCESS;

    switch( TraceOptions.TraceMode )
    {
        case mode_COMPARETRACE :

            if ( dvm_OneProcSign )
            {
                SYSTEM(sprintf, (FileName, "%s%s%d.%s",
                    TraceOptions.TracePath, TraceOptions.InputTracePrefix, dvm_OneProcNum,
                    TraceOptions.Ext))
            }
            else
            {
                if ( TraceOptionsTraceFile[0] == 0 )
                        /* use input trace prefix */
                    SYSTEM(sprintf, (FileName, "%s%s0.%s",
                        TraceOptions.TracePath, TraceOptions.InputTracePrefix,
                        TraceOptions.Ext))
                else
                        /* the filename is specified in the command-line */
                    SYSTEM(sprintf,(FileName, "%s%s", TraceOptions.TracePath,
                                                    TraceOptionsTraceFile ));
            }
            for(i=0; i <= ParFileOpenCount; i++)
            {  hf = (RTL_CALL, dvm_fopen( FileName, OPENMODE(r) ));

               if( hf != NULL )
               {
                   StrCount = trc_rd_header( hf, FileName );
                   trc_rd_trace( hf, FileName, StrCount );
                   (RTL_CALL, dvm_fclose(hf));
                   cmptrace_ReInit();
               }
               else
               {
                   if(i < ParFileOpenCount)
                   {  /* the number of attempts to open the file
                        is not exhausted */

                      /* waiting for next attempt */
                      SLEEP(1);
                      continue; /* to the next attempt to open file */
                   }

                   epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                            "*** RTS err: CMPTRACE: Can't open "
                            "trace file %s for reading\n", FileName );
                   return ;
               }

               break;
            }

            if ( Trace.TraceCPUCount > 1 )
            {
                if ( TraceOptionsTraceFile[0] != 0 )
                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: Please specify TraceOptions.InputTracePrefix "
                                "instead of command-line trace filename argument\n");

                /* load other traces into memory */
                for ( k=1; k < Trace.TraceCPUCount; k++ )
                {
                    SYSTEM(sprintf, (FileName, "%s%s%d.%s", TraceOptions.TracePath,
                            TraceOptions.InputTracePrefix, k, TraceOptions.Ext));

                    for(i=0; i <= ParFileOpenCount; i++)
                    {
                        hf = (RTL_CALL, dvm_fopen( FileName, OPENMODE(r) ));

                        if( hf != NULL )
                        {
                            TraceHeaderRead = 0; /* in order trc_InfoFindForCurrentLevel() does not diagnose
                                                  * an error when additional constructs are found in the
                                                  * header of other traces */
                            StrCount = trc_mrg_header( hf, FileName );

                            if ( !EnableTrace )
                                    return ;  /* stop the debugger if there were read errors */

                            Trace.CPU_Num = k;
                            trc_mrg_trace( hf, FileName, StrCount );

                            if ( !EnableTrace )
                                    return ;  /* stop the debugger if there were read errors */

                            (RTL_CALL, dvm_fclose(hf));
                            cmptrace_ReInit();
                        }
                        else
                        {
                            if(i < ParFileOpenCount)
                            {  /* Number of attempts to open file is not exhausted */    /*E0005*/
                                double op1, op2 = 1.1;
                                DvmType   j;

                                op1 = 1.1;

                                for(j=0; j < 1000000; j++)
                                    op1 /= op2; /* temporary delay */    /*E0006*/
                                continue; /* to the next attempt to inquire file size*/    /*E0007*/
                            }
                            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                        "*** RTS err: CMPTRACE: Can't open "
                                        "trace file %s for reading\n", FileName );
                            return ;
                        }
                        break;
                    }
                }

                if ( TraceOptions.MrgTracePrint )
                {
                    if ( Trace.CurCPUNum == 0 ) /* io_proc CPU is zero CPU */
                    {
                        SYSTEM(sprintf, (FileName, "%s%s.mrg.trd", TraceOptions.TracePath, dvm_argv[0]));
                        SYSTEM(remove, (FileName));

                        SYSTEM_RET(f, fopen, (FileName, OPENMODE(w)));
                        pprintf(3, "*** DVM debugger: Merged trace writing started\n");

                        trc_wrt_header( &Trace.tStructs, 0, f, 0 );
                        trc_wrt_trace( f, TRACE_DONE );
                        pprintf(3, "*** DVM debugger: Merged trace writing finished\n");

                        SYSTEM(fclose,(f));
                        if ( TraceOptions.MrgTracePrint == 2 )
                            pprintf(3, "*** DVM debugger: User program finished after merged trace generation. No real execution\n");
                    }

                    if ( TraceOptions.MrgTracePrint == 2 )
                    {
                        DvmType res = 0;
                        EnableTrace=0;  // suppress "No errors found" output
                        lexit_(&res);   // assume there must be some sync not to stop writing trace process
                    }
                }
            }
            break;

        case mode_WRITETRACE :
        case mode_CONFIG_WRITETRACE :
        case mode_CONFIG :
            SYSTEM(sprintf,( FileName, "%s%s0.%s", TraceOptions.TracePath,
                             TraceOptions.FileLoopInfo, TraceOptions.Ext ));
            hf = (RTL_CALL, dvm_fopen( FileName, OPENMODE(r) ));
            if( hf != NULL )
            {
                trc_rd_header( hf, FileName );
                (RTL_CALL, dvm_fclose(hf));
                if(DbgInfoPrint && _SysInfoPrint)
                   pprintf(1, "Trace accumulation: file %s is used\n", FileName );
            }
            else
            {
                int StrNo;

                if( TraceOptions.SaveThroughExec != 0  &&
                    ( TraceOptions.TraceMode == mode_WRITETRACE || TraceOptions.TraceMode == mode_CONFIG_WRITETRACE ) )
                {
                    EnableTrace = 0;
                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** RTS err: CMPTRACE: You can't specify TraceOptions.SaveThroughExec mode"
                    " if trace configuration file does not exist.\n" );

                    /*TraceOptions.SaveThroughExec = 0;*/
                }

                if(DbgInfoPrint && _SysInfoPrint)
                {
                    switch( TraceOptions.TraceLevel )
                    {
                        case level_MINIMAL : StrNo = N_MINIMAL; break;
                        case level_MODIFY : StrNo = N_MODIFY; break;
                        case level_FULL : StrNo = N_FULL; break;
                        default : StrNo = N_NONE; break;
                    }

                    pprintf(1, "Trace accumulation: level %s\n",
                              KeyWords[StrNo] );
                }
            }
            Trace.ErrCode = SUCCESS;
            break;
    }
}

void cmptrace_Write(TRACE_WRITE_REASON reason)
{
    FILE *hf;
    char FileName[128];

    if (TraceOptions.TraceMode == mode_CONFIG || TraceOptions.TraceMode == mode_CONFIG_WRITETRACE)
    { /* MPS_CurrentProc == TraceOptions.WrtHeaderProc is not checked */

        SYSTEM(sprintf,(FileName, "%s%s%d.%s", TraceOptions.TracePath,
                        TraceOptions.FileLoopInfo, Trace.CurCPUNum, TraceOptions.Ext));
        SYSTEM(remove, (FileName));
        SYSTEM_RET(hf, fopen, (FileName, OPENMODE(w)));

        if (hf != NULL)
        {
            trc_wrt_header(&Trace.tStructs, 0, hf, 1);

            if ( EnableCodeCoverage )
                trc_wrt_codecoverage(hf);

            SYSTEM(fclose, (hf));
        }
        else
        {
            pprintf(3, "*** RTS err: CMPTRACE: Can't open file %s for writing\n", FileName);
        }
        Trace.ErrCode = SUCCESS;
    }

    if ((TraceOptions.TraceMode == mode_CONFIG_WRITETRACE || TraceOptions.TraceMode == mode_WRITETRACE) &&
        TraceOptions.SaveThroughExec == 0)
    {
        if ( dvm_OneProcSign )
        {
            SYSTEM(sprintf, (FileName, "%s%s%d.%s", TraceOptions.TracePath,
                            TraceOptions.OutputTracePrefix, dvm_OneProcNum,
                            TraceOptions.Ext));
        }
        else
        {
            SYSTEM(sprintf, (FileName, "%s%s%d.%s", TraceOptions.TracePath,
                            TraceOptions.OutputTracePrefix, MPS_CurrentProc,
                            TraceOptions.Ext));
            if(MPS_CurrentProc == DVM_IOProc) // remove all extra-CPU traces
            {
                char FileName2[128];
                int i = MPS_VMS->ProcCount, res = 0;

                while ( res == 0 )
                {
                    SYSTEM(sprintf, (FileName2, "%s%s%d.%s", TraceOptions.TracePath,
                                TraceOptions.OutputTracePrefix, i, TraceOptions.Ext));
                    SYSTEM_RET(res, remove, (FileName2))
                    i++;
                }
            }
        }

        SYSTEM(remove, (FileName));

        SYSTEM_RET(hf, fopen, (FileName, OPENMODE(w)));
        CurrCmpTraceFileSize = 0;

        if (reason == TRACE_DYN_MEMORY_LIMIT) {
            pprintf(3, "*** DVM debugger: Incomplete trace writing started\n");
        } else {
            pprintf(3, "*** DVM debugger: Trace writing started\n");
        }

        if( hf != NULL )
        {
            CurrCmpTraceFileSize += trc_wrt_header( &Trace.tStructs, 0, hf, 0 );
            trc_wrt_trace( hf, reason );
            SYSTEM(fclose,(hf));
        }
        else
        {
            pprintf(3, "*** RTS err: CMPTRACE: Can't open file %s for writing\n", FileName);
        }

        pprintf(3, "*** DVM debugger: Trace writing finished\n");

        Trace.ErrCode = SUCCESS;
    }
}

/*********************************************************************/

STRUCT_INFO* trc_InfoNew(STRUCT_INFO* pParent)
{
    STRUCT_INFO* pRes;
    short i;

    if( pParent != NULL )
    {
        pRes = table_GetNew(STRUCT_INFO, &(pParent->tChildren));
    }
    else
    {
        pRes = table_GetNew(STRUCT_INFO, &(Trace.tStructs));
    }
    pRes->Rank = MAXARRAYDIM;
    pRes->TraceLevel = level_DEFAULT;
    pRes->pParent = pParent;
    pRes->TracedIterNum = 0;

    for( i = 0; i < pRes->Rank; i++ )
    {
        pRes->Limit[i].Lower = pRes->Current[i].Lower = pRes->Common[i].Lower = pRes->CurLocal[i].Lower = MAXLONG;
        pRes->Limit[i].Upper = pRes->Current[i].Upper = pRes->Common[i].Upper = pRes->CurLocal[i].Upper = MAXLONG;
        pRes->Limit[i].Step = pRes->Current[i].Step = pRes->Common[i].Step = pRes->CurLocal[i].Step  = MAXLONG;
        pRes->CurIter[i] = MAXLONG;
    }
    table_Init(&(pRes->tChildren), 10, sizeof(struct tag_STRUCT_INFO), trc_InfoDone);

    list_init( &(pRes->ArrList) );

    return pRes;
}

void trc_InfoDone(STRUCT_INFO *pInfo)
{
    table_Done(&(pInfo->tChildren));
    list_clean(&(pInfo->ArrList));
}

int trc_InfoCanTrace(STRUCT_INFO *pInfo, int nRecType)
{
    DvmType CurIter;
    int i;
    enum_TraceLevel TraceLevel;
    DvmType Step;

    TraceLevel = (enum_TraceLevel)
    (pInfo != NULL ? pInfo->RealLevel : TraceOptions.TraceLevel);

    if ( (pInfo != NULL && pInfo->bSkipExecution) || Trace.IterCtrlInfo )
        return 0;

    switch ( TraceLevel )
    {
        case  level_CHECKSUM:    /* Significant performance slowdown !!! */
            if ( nRecType == trc_PREWRITEVAR || nRecType == trc_REDUCTVAR || nRecType == trc_SKIP
                    || (Trace.inParLoop && (nRecType == trc_STRUCTBEG || nRecType == trc_STRUCTEND)) )
                return 0;
            break;

        case  level_MINIMAL:
            if (nRecType == trc_READVAR || nRecType == trc_PREWRITEVAR || nRecType == trc_POSTWRITEVAR )
                return 0;
            break;

        case  level_MODIFY:
            if (nRecType == trc_READVAR )
                return 0;
            break;

        case  level_NONE:
            return 0;
    }

    if ( /*nRecType != trc_STRUCTBEG && nRecType != trc_STRUCTEND && nRecType != trc_ITER &&*/
            (
              (Trace.StartPoint && num_cmp(&Trace.CurPoint, Trace.StartPoint) < 0) ||
              (Trace.FinishPoint && ((i = num_cmp(&Trace.CurPoint, Trace.FinishPoint)) >= 0 || i == -2) )
            )
       )
        return 0;

    if (TraceLevel != level_CHECKSUM && pInfo != NULL && nRecType != trc_STRUCTBEG && nRecType != trc_STRUCTEND)
    {
        for (i = 0; i < pInfo->Rank; i++)
        {
            CurIter = pInfo->CurIter[i];

            if ((pInfo->Limit[i].Lower != MAXLONG && CurIter < pInfo->Limit[i].Lower) ||
                (pInfo->Limit[i].Upper != MAXLONG && CurIter > pInfo->Limit[i].Upper))
            {
                return 0;
            }

            Step = pInfo->Limit[i].Step != MAXLONG ? pInfo->Limit[i].Step : (DvmType)TraceOptions.DefaultIterStep;
            if (Step != 1 && Step != 0)
            {
                if (pInfo->Limit[i].Lower != MAXLONG)
                    CurIter -= pInfo->Limit[i].Lower;

                if (CurIter % Step != 0)
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void trc_InfoSetup(STRUCT_INFO *pInfo, DvmType *Init, DvmType *Last, DvmType *Step)
{
    short i;
    DvmType lv;
    DvmType CurIter;
    DvmType CurStep;

    pInfo->bSkipExecution = (byte)(pInfo->pParent != NULL ? pInfo->pParent->bSkipExecution : 0);
    pInfo->TracedIterNum = 0;

    if( pInfo->TraceLevel == level_DEFAULT )
    {
        pInfo->RealLevel = (enum_TraceLevel)
        (pInfo->pParent != NULL ? pInfo->pParent->RealLevel : TraceOptions.TraceLevel);

    }
    else
    {
        pInfo->RealLevel = pInfo->TraceLevel;
    }

    if (pInfo->RealLevel != level_NONE && pInfo->pParent != NULL && !pInfo->bSkipExecution)
    {
        for (i = 0; i < pInfo->pParent->Rank; i++)
        {
            CurIter = pInfo->pParent->CurIter[i];

            if ((pInfo->pParent->Limit[i].Lower != MAXLONG && CurIter < pInfo->pParent->Limit[i].Lower) ||
                (pInfo->pParent->Limit[i].Upper != MAXLONG && CurIter > pInfo->pParent->Limit[i].Upper))
            {
                pInfo->bSkipExecution = 1;
                break;
            }

            CurStep = pInfo->pParent->Limit[i].Step != MAXLONG ?
                pInfo->pParent->Limit[i].Step : (DvmType)TraceOptions.DefaultIterStep;

            if (CurStep != 1 && CurStep != 0)
            {
                if (pInfo->pParent->Limit[i].Lower != MAXLONG)
                    CurIter -= pInfo->pParent->Limit[i].Lower;

                if (CurIter % CurStep != 0)
                {
                    pInfo->bSkipExecution = 1;
                    break;
                }
            }
        }
    }

    for( i = 0; i < pInfo->Rank; i++ )
    {
        pInfo->CurIter[i] = MAXLONG;
        pInfo->Current[i].Lower = pInfo->Current[i].Upper = pInfo->Current[i].Step  = 1;
    }

    if( Init && Last && Step )
    {
        for( i = 0; i < pInfo->Rank; i++ )
        {
            if( Init[i] > Last[i] )
            {
                pInfo->Current[i].Step = -Step[i];
                pInfo->Current[i].Upper = Init[i];
                pInfo->Current[i].Lower = Last[i];

                lv = (Init[i] - Last[i]) % pInfo->Current[i].Step;
                if(lv)
                    pInfo->Current[i].Lower += (pInfo->Current[i].Step - lv);
            }
            else
            {
                pInfo->Current[i].Step = Step[i];
                pInfo->Current[i].Lower = Init[i];
                pInfo->Current[i].Upper = Last[i];
            }

            if( pInfo->Common[i].Lower == MAXLONG || pInfo->Common[i].Lower > pInfo->Current[i].Lower )
            {
                pInfo->Common[i].Lower = pInfo->Current[i].Lower;
            }
            if( pInfo->Common[i].Upper == MAXLONG || pInfo->Common[i].Upper < pInfo->Current[i].Upper )
            {
                pInfo->Common[i].Upper = pInfo->Current[i].Upper;
            }
            if( pInfo->Common[i].Step == MAXLONG || pInfo->Common[i].Step != pInfo->Current[i].Step )
            {
                pInfo->Common[i].Step = pInfo->Current[i].Step > 0 ? 1 : -1;
            }
        }
    }
}



void  trc_ArrayRegister(byte  bRank, DvmType  *pSize, DvmType  lType,
                        char  *szFile, uLLng  ulLine, char  *szOperand,
                        void  *pArrBase, byte  bIsDistr)
{
    dvm_ARRAY_INFO  *pInfo = NULL;
    int              nCmpRes, i;
    DvmType             lArrayNo = Trace.lNextArray;

    if(NULL == pArrBase)
       return;

    if(0 != TraceHeaderRead && TraceOptions.drarr)
    {
        if (lArrayNo < table_Count(&(Trace.tArrays)))
        {
            pInfo = table_At(dvm_ARRAY_INFO, &(Trace.tArrays), lArrayNo);

            SYSTEM_RET(nCmpRes, strnFileCmp, (pInfo->szFile, szFile,
                       MaxSourceFileName));

            if(0 == nCmpRes && bRank == pInfo->bRank)
            {
                if ( ulLine == pInfo->ulLine || TraceOptions.SRCLocCompareMode >= 2 )
                {
                    pInfo->ulLine = ulLine; /* just in case - different lines mapping */

                    if ( lType == pInfo->lElemType )
                    {
                        ++Trace.lNextArray;
                    }
                    else
                    {
                        if ( (lType == rt_INT || lType == rt_LONG || lType == rt_LLONG ) &&
                                (pInfo->lElemType == rt_INT || pInfo->lElemType == rt_LONG || pInfo->lElemType == rt_LLONG) )
                        {
                            pInfo->lElemType = lType; /*     DANGEROUS !!!!!!!   */
                            ++Trace.lNextArray;
                        }
                    }
                }
                else
                    pInfo = NULL;
            }
            else
                pInfo = NULL;
        }

        if(NULL == pInfo)
        {
            /* The read header does not match the current file.
               Disable debugger. */

            EnableTrace = 0;

            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** RTS err: CMPTRACE: The trace configuration file "
            "does not match the current program. "
            "Array mismatch or absent. File: %s, Line: %ld.\n",
            szFile, ulLine);
        }

        pInfo->pAddr = pArrBase;
        pInfo->bIsDistr = bIsDistr;
    }
    else
    {
        pInfo = trc_ArrayInfoNew(szFile, szOperand, ulLine, pArrBase,
                                 bIsDistr, lType, bRank, &lArrayNo);
    }

    pInfo->lLineSize = 1;

    for(i = 0; i < bRank; ++i)
    {
        pInfo->rgSize[i] = pSize[i];
        pInfo->lLineSize *= pSize[i];
    }

    /* Insert into hash-table using array address as key */

    hash1_Insert(&Trace.hArrayPointers, pArrBase, lArrayNo);
}



void trc_ArrayRegisterRemoteBuffer(SysHandle* pSrcHandle, void* pBuffAddr,
                                   void* pBuffBaseAddr, DvmType* pIndex)
{
    dvm_ARRAY_INFO* pInfo = NULL;
    dvm_ARRAY_INFO* pSrcInfo = NULL;
    DvmType lSrcNo = -1;
    DvmType lBufferNo = -1;
    int i;

    lSrcNo = hash1_Find(&Trace.hArrayPointers, pSrcHandle);
    if (-1 != lSrcNo)
    {
        pSrcInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, lSrcNo);

        pInfo = trc_ArrayInfoNew("", "", 0, pBuffBaseAddr, 0,
            pSrcInfo->lElemType, pSrcInfo->bRank, &lBufferNo);
        pInfo->bIsRemoteBuffer = 1;
        pInfo->bSourceNo = lSrcNo;
        dvm_ArrayCopy(DvmType, pInfo->rgRBIndexMap, pIndex, pSrcInfo->bRank);
        pInfo->lLineSize = 1;

        for (i = 0; i < pSrcInfo->bRank; ++i)
        {
            if (-1 == pIndex[i])
                pInfo->rgSize[i] = pSrcInfo->rgSize[i];
            else
                pInfo->rgSize[i] = 1;

            pInfo->lLineSize *= pInfo->rgSize[i];
        }
    }

    /* Insert into hash-table using buffer address as key */

    hash1_Insert(&Trace.hArrayPointers, pBuffAddr, lBufferNo);
}



void trc_ArrayReRegister(SysHandle* pOldHandle, SysHandle* pNewHandle)
{
    dvm_ARRAY_INFO* pInfo = NULL;
    DvmType lArrayNo;

    lArrayNo = hash1_Find(&Trace.hArrayPointers, pOldHandle);
    if (-1 != lArrayNo)
    {
        pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, lArrayNo);
        hash1_Remove(&Trace.hArrayPointers, pOldHandle);

        pInfo->pAddr = pNewHandle;
        hash1_Insert(&Trace.hArrayPointers, pNewHandle, lArrayNo);
    }
}



int trc_ArrayCanTraceMD(void* pAddr, void* pArrBase, enum_TraceType iType)
{
    dvm_ARRAY_INFO* pInfo = NULL;
    dvm_ARRAY_INFO* pSrcInfo = NULL;
    s_DISARRAY* pDisArray = NULL;
    enum_TraceLevel TraceLevel;
    DvmType lIndex;
    DvmType lArrayNo;
    DvmType rgSIndex[MAXARRAYDIM];
    int i;
    DvmType Step;

    if ((TraceOptions.TraceLevel == level_CHECKSUM) ||
        (TraceOptions.CalcChecksums == 1)) return 1; /* Limitations won't work with checksums calculation */

    /* The scalar variable can point to remote buffer */

    if (NULL == pArrBase)
        lArrayNo = hash1_Find(&Trace.hArrayPointers, pAddr);
    else
        lArrayNo = hash1_Find(&Trace.hArrayPointers, pArrBase);

    if (-1 != lArrayNo)
    {
        pSrcInfo = pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, lArrayNo);
        if (pInfo->bIsRemoteBuffer)
        {
            pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, pInfo->bSourceNo);
            if (NULL == pInfo)
                return 1;
        }
        else if (NULL == pArrBase)
            return 1;

        TraceLevel = (enum_TraceLevel)
        ((level_DEFAULT == pInfo->eTraceLevel) ? TraceOptions.TraceLevel : pInfo->eTraceLevel);

        switch (TraceLevel)
        {
        case level_NONE:
        case level_MINIMAL:
            return 0;
        case level_MODIFY:
            if (trc_READVAR == iType)
                return 0;
            break;
        case level_FULL:
            break;
        default:
            return 0;
        }

        if (pSrcInfo->bIsRemoteBuffer)
        {
            lIndex = (DvmType)(((char*)pAddr - (char*)pSrcInfo->pAddr) / pInfo->nElemSize);
            if (lIndex >= pSrcInfo->lLineSize)
                return -1;

            trc_CalcRemoteSI(lIndex, pSrcInfo->bRank, pSrcInfo->rgSize,
                pSrcInfo->rgRBIndexMap, rgSIndex);
        }
        else
        {
            if (pInfo->bIsDistr)
            {
                pDisArray = (s_DISARRAY*)(((SysHandle*)pArrBase)->pP);
                lIndex = (DvmType)(((char*)pAddr - (char*)(pDisArray->ArrBlock.ALoc.Ptr)) / pInfo->nElemSize);
                if (lIndex >= pInfo->lLineSize)
                    return -1;

                trc_CalcPrimarySI(lIndex, &(pDisArray->ArrBlock.Block), rgSIndex);
            }
            else
            {
                lIndex = (DvmType)(((char*)pAddr - (char*)pArrBase) / pInfo->nElemSize);
                if (lIndex >= pInfo->lLineSize)
                    return -1;

                if (pInfo->bRank > 1)
                    trc_CalcSI(lIndex, pInfo->bRank, pInfo->rgSize, rgSIndex);
                else
                    rgSIndex[0] = lIndex;
            }
        }

        for (i = 0; i < pInfo->bRank; ++i)
        {
            if ((pInfo->rgLimit[i].Lower != MAXLONG && rgSIndex[i] < pInfo->rgLimit[i].Lower) ||
                (pInfo->rgLimit[i].Upper != MAXLONG && rgSIndex[i] > pInfo->rgLimit[i].Upper))
            {
                return 0;
            }

            Step = pInfo->rgLimit[i].Step != MAXLONG ? pInfo->rgLimit[i].Step : (DvmType)TraceOptions.DefaultArrayStep;
            if (Step != 0 && Step != 1)
            {
                if (pInfo->rgLimit[i].Lower != MAXLONG)
                    rgSIndex[i] -= pInfo->rgLimit[i].Lower;

                if (rgSIndex[i] % Step != 0)
                {
                    return 0;
                }
            }
        }

        switch (iType)
        {
        case trc_READVAR :
            ++(pInfo->ulRead);
            break;
        case trc_POSTWRITEVAR :
            ++(pInfo->ulWrite);
            break;
        }
    }

    return 1;
}

int trc_ArrayCanTraceSD(void* pAddr, void* pArrBase, enum_TraceType iType)
{
    dvm_ARRAY_INFO* pInfo = NULL;
    dvm_ARRAY_INFO* pSrcInfo = NULL;
    s_DISARRAY* pDisArray = NULL;
    enum_TraceLevel TraceLevel;
    DvmType lIndex;
    DvmType lArrayNo;
    DvmType rgSIndex[MAXARRAYDIM];
    DvmType Step;

    if ((TraceOptions.TraceLevel == level_CHECKSUM) ||
        (TraceOptions.CalcChecksums == 1)) return 1; /* Limitations won't work with checksums calculation */

    /* The scalar variable can point to remote buffer */

    if (NULL == pArrBase)
        lArrayNo = hash1_Find(&Trace.hArrayPointers, pAddr);
    else
        lArrayNo = hash1_Find(&Trace.hArrayPointers, pArrBase);

    if (-1 != lArrayNo)
    {
        pSrcInfo = pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, lArrayNo);
        if (pInfo->bIsRemoteBuffer)
        {
            pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, pInfo->bSourceNo);
            if (NULL == pInfo)
                return 1;
        }

        TraceLevel = (enum_TraceLevel)
        ((level_DEFAULT == pInfo->eTraceLevel) ? TraceOptions.TraceLevel : pInfo->eTraceLevel);

        switch (TraceLevel)
        {
        case level_NONE:
        case level_MINIMAL:
            return 0;
        case level_MODIFY:
            if (trc_READVAR == iType)
                return 0;
            break;
        case level_FULL:
            break;
        default:
            return 0;
        }

        if (pSrcInfo->bIsRemoteBuffer)
        {
            lIndex = (DvmType)(((char*)pAddr - (char*)pSrcInfo->pAddr) / pInfo->nElemSize);
            if (lIndex >= pSrcInfo->lLineSize)
                return -1;

            trc_CalcRemoteSI(lIndex, pSrcInfo->bRank, pSrcInfo->rgSize,
                pSrcInfo->rgRBIndexMap, rgSIndex);
            lIndex = trc_CalcArrayLI(pInfo->bRank, pInfo->rgSize, rgSIndex);
        }
        else
        {
            if (pInfo->bIsDistr)
            {
                pDisArray = (s_DISARRAY*)(((SysHandle*)pArrBase)->pP);
                lIndex = (DvmType)(((char*)pAddr - (char*)(pDisArray->ArrBlock.ALoc.Ptr)) / pInfo->nElemSize);
                trc_CalcPrimarySI(lIndex, &(pDisArray->ArrBlock.Block), rgSIndex);
                lIndex = trc_CalcArrayLI(pInfo->bRank, pInfo->rgSize, rgSIndex);
            }
            else
            {
                lIndex = (DvmType)(((char*)pAddr - (char*)pArrBase) / pInfo->nElemSize);
            }

            if (lIndex >= pInfo->lLineSize)
                return -1;
        }

        if ((pInfo->rgLimit[0].Lower != MAXLONG && lIndex < pInfo->rgLimit[0].Lower) ||
            (pInfo->rgLimit[0].Upper != MAXLONG && lIndex > pInfo->rgLimit[0].Upper))
        {
            return 0;
        }

        Step = pInfo->rgLimit[0].Step != MAXLONG ? pInfo->rgLimit[0].Step : (DvmType)TraceOptions.DefaultArrayStep;
        if (Step != 0 && Step != 1)
        {
            if (pInfo->rgLimit[0].Lower != MAXLONG)
                lIndex -= pInfo->rgLimit[0].Lower;

            if (lIndex % Step != 0)
            {
                return 0;
            }
        }

        switch (iType)
        {
        case trc_READVAR :
            ++(pInfo->ulRead);
            break;
        case trc_POSTWRITEVAR :
            ++(pInfo->ulWrite);
            break;
        }
    }

    return 1;
}



dvm_ARRAY_INFO  *trc_ArrayInfoNew(char  *szFile, char  *szOperand,
                                  uLLng  ulLine, void  *pArrBase,
                                  byte  bIsDistr, DvmType  lType,
                                  byte  bRank, DvmType  *plNo)
{
    dvm_ARRAY_INFO *pInfo = NULL, *pInfo1 = NULL;
    int i;

    *plNo = table_GetNewNo(&Trace.tArrays);
    pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, *plNo);
    SYSTEM(strncpy, (pInfo->szFile, szFile, MaxSourceFileName));
    pInfo->szFile[MaxSourceFileName] = 0;
    SYSTEM(strncpy, (pInfo->szOperand, szOperand, MaxOperand));
    pInfo->szOperand[MaxOperand] = 0;
    pInfo->ulLine = ulLine;
    pInfo->pAddr = pArrBase;
    pInfo->bIsDistr = bIsDistr;
    pInfo->eTraceLevel = level_DEFAULT;
    pInfo->bRank = bRank;
    pInfo->lElemType = lType;
    pInfo->iNumber = 0;
    pInfo->correctCSPoint = NULL;

    pInfo->bIsRemoteBuffer = 0;
    pInfo->bSourceNo = -1;

    if (mode_COMPARETRACE == TraceOptions.TraceMode)
    {
        table_Init(&(pInfo->tErrEntries), 7, sizeof(struct tag_ERR_INFO), NULL);
    }
    else
    {
        pInfo->tErrEntries.IsInit = 0;
    }

    switch (lType)
    {
    case rt_INT :
    case rt_LOGICAL :
        pInfo->nElemSize = sizeof(int);
        break;
    case rt_LONG :
        pInfo->nElemSize = sizeof(long);
        break;
    case rt_LLONG:
        pInfo->nElemSize = sizeof(long long);
        break;
    case rt_FLOAT :
        pInfo->nElemSize = sizeof(float);
        break;
    case rt_DOUBLE :
        pInfo->nElemSize = sizeof(double);
        break;
    case rt_FLOAT_COMPLEX :
        pInfo->nElemSize = 2 * sizeof(float);
        break;
    case rt_DOUBLE_COMPLEX :
        pInfo->nElemSize = 2 * sizeof(double);
        break;
    default :
        pInfo->nElemSize = 1;
    }

    for (i = 0; i < MAXARRAYDIM; ++i)
    {
        pInfo->rgRBIndexMap[i] = -1;
        pInfo->rgSize[i] = 0;
        pInfo->rgLimit[i].Lower = MAXLONG;
        pInfo->rgLimit[i].Upper = MAXLONG;
        pInfo->rgLimit[i].Step = MAXLONG;
    }

    for (i = table_Count(&Trace.tArrays)-2; i>=0; i--)
    {
        pInfo1 = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

        if (pInfo1->ulLine == ulLine && !pInfo1->bIsRemoteBuffer)
            if (strcmp(pInfo1->szOperand, pInfo->szOperand) == 0)
            {
                pInfo->iNumber = pInfo1->iNumber+1;
                break;
            }
    }

    return pInfo;
}

STRUCT_INFO *trc_InfoFindForCurrentLevel(DvmType No, byte Type, byte Rank, char *File, UDvmType Line)
{
    STRUCT_INFO *pInfo = NULL;
    TABLE* pColl;
    int i, CmpRes;

    pColl = Trace.pCurCnfgInfo != NULL ? &(Trace.pCurCnfgInfo->tChildren) : &(Trace.tStructs);

    for( i = 0; i < table_Count(pColl); i++ )
    {
        pInfo = table_At(STRUCT_INFO, pColl, i);

        SYSTEM_RET(CmpRes, strnFileCmp, (pInfo->File, File, MaxSourceFileName));
        if( pInfo->No == No && pInfo->Rank == Rank && pInfo->Type == Type &&
            CmpRes == 0 )
        {
            if ( pInfo->Line == Line || TraceOptions.SRCLocCompareMode >= 2 )
            {
                pInfo->Line = Line; /* just in case, to report differences in the new lines form */
                break;
            }
        }
        pInfo = NULL;
    }

    if (pInfo == NULL)
    {
        /* Check loaded header status */

        if (0 != TraceHeaderRead)
        {
            /* The read header does not match the current file. Disable debugger. */

            EnableTrace = 0;

            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** RTS err: CMPTRACE: The trace configuration file does not match the current program. "
            "Loop: %ld, File: %s, Line: %ld.\n", No, File, Line);
        }

        pInfo = trc_InfoNew( Trace.pCurCnfgInfo );

        SYSTEM(strncpy, (pInfo->File, File, MaxSourceFileName));
        pInfo->File[MaxSourceFileName] = 0;
        pInfo->Line = Line;
        pInfo->No = No;
        pInfo->Rank = Rank;
        pInfo->Type = Type;
    }

    return pInfo;
}

/*********************************************************************/

void trc_StoreValue(VALUE* pValue, void* pMem, DvmType lType)
{
    switch (lType)
    {
        case rt_INT :
            pValue->_int = PT_INT(pMem);
            break;
        case rt_LOGICAL :
            pValue->_int  = (PT_INT(pMem) != 0)?1:0; /* change logical type to integer */
            break;
        case rt_LONG :
            pValue->_long = PT_LONG(pMem);
            break;
        case rt_LLONG:
            pValue->_longlong = PT_LLONG(pMem);
            break;
        case rt_FLOAT :
            pValue->_float = PT_FLOAT(pMem);
            break;
        case rt_DOUBLE :
            pValue->_double = PT_DOUBLE(pMem);
            break;
        case rt_FLOAT_COMPLEX :
            pValue->_complex_float[0] = PT_FLOAT(pMem);
            pValue->_complex_float[1] = PT_FLOAT(((float *)pMem) + 1);
            break;
        case rt_DOUBLE_COMPLEX :
            pValue->_complex_double[0] = PT_DOUBLE(pMem);
            pValue->_complex_double[1] = PT_DOUBLE(((double *)pMem) + 1);
            break;
    }
}

char *trc_SprintfValue(char *string, VALUE* pValue, DvmType lType)
{
    string[0]=0;

    switch (lType)
    {
        case rt_INT :
        case rt_LOGICAL :
            sprintf(string, "%- d", pValue->_int);
            break;
        case rt_LONG :
            sprintf(string, "%- ld", pValue->_long);
            break;
        case rt_LLONG:
            sprintf(string, "%- lld", pValue->_longlong);
            break;
        case rt_FLOAT :
            sprintf(string, "%- .*G", Trace.FloatPrecision, pValue->_float);
            break;
        case rt_DOUBLE :
            sprintf(string, "%- .*lG", Trace.DoublePrecision, pValue->_double);
            break;
        case rt_FLOAT_COMPLEX :
            sprintf(string, "(%- .*G,%- .*G)", Trace.FloatPrecision, pValue->_complex_float[0],
                    Trace.FloatPrecision, pValue->_complex_float[1]);
            break;
        case rt_DOUBLE_COMPLEX :
            sprintf(string, "(%- .*lG,%- .*lG)", Trace.DoublePrecision, pValue->_complex_double[0],
                    Trace.DoublePrecision, pValue->_complex_double[1]);
            break;
    }
    return string;
}

int trc_CompareValueAbsolute(VALUE* pValue, void* pMem, DvmType lType, int difType)
{
    int nCmpRes = 0;

    if ( !difType )
    {
        switch (lType)
        {
            case rt_INT :
                nCmpRes = INT_CMP(int, pValue->_int, pMem);
                break;
            case rt_LOGICAL :
                difType = (PT_INT(pMem) != 0)?1:0;
                nCmpRes = INT_CMP(int, pValue->_int, &difType);
                break;
            case rt_LONG :
                nCmpRes = INT_CMP(long, pValue->_long, pMem);
                break;
            case rt_LLONG:
                nCmpRes = INT_CMP(long long, pValue->_longlong, pMem);
                break;
            case rt_FLOAT :
                nCmpRes = REAL_ABS_VAL(float, pValue->_float, pMem) <= TraceOptions.Exp;
                break;
            case rt_DOUBLE :
                nCmpRes = REAL_ABS_VAL(double, pValue->_double, pMem) <= TraceOptions.Exp;
                break;
            case rt_FLOAT_COMPLEX :
                nCmpRes = (REAL_ABS_VAL(float, pValue->_complex_float[0], pMem) <= TraceOptions.Exp) &&
                    (REAL_ABS_VAL(float, pValue->_complex_float[1], ((float *)pMem) + 1) <= TraceOptions.Exp);
                break;
            case rt_DOUBLE_COMPLEX :
                nCmpRes = (REAL_ABS_VAL(double, pValue->_complex_double[0], pMem) <= TraceOptions.Exp) &&
                    (REAL_ABS_VAL(double, pValue->_complex_double[1], ((double *)pMem) + 1) <= TraceOptions.Exp);
                break;
        }
    }
    else
    {
        if ( difType == 1 )
             /* cmp int & long */
             nCmpRes = INT_CMP(long, (long)pValue->_int, pMem);
        else /* cmp long & int */
        {
            long tmp = *((int *)pMem);
            nCmpRes = INT_CMP(long, pValue->_long, &tmp);
        }
    }

    return nCmpRes;
}

int trc_CompareValueRelative(VALUE* pValue, void* pMem, DvmType lType, int difType)
{
    int nCmpRes = 0;

    if ( !difType )
    {
        switch (lType)
        {
            case rt_INT :
                nCmpRes = INT_CMP(int, pValue->_int, pMem);
                break;
            case rt_LOGICAL :
                difType = (PT_INT(pMem) != 0)?1:0;
                nCmpRes = INT_CMP(int, pValue->_int, &difType);
                break;
            case rt_LONG :
                nCmpRes = INT_CMP(long, pValue->_long, pMem);
                break;
            case rt_LLONG:
                nCmpRes = INT_CMP(long long, pValue->_longlong, pMem);
                break;
            case rt_FLOAT :
                nCmpRes = REAL_REL_VAL(float, pValue->_float, pMem) <= TraceOptions.Exp;
                break;
            case rt_DOUBLE :
                nCmpRes = REAL_REL_VAL(double, pValue->_double, pMem) <= TraceOptions.Exp;
                break;
            case rt_FLOAT_COMPLEX :
                nCmpRes = (REAL_REL_VAL(float, pValue->_complex_float[0], pMem) <= TraceOptions.Exp ) &&
                    (REAL_REL_VAL(float, pValue->_complex_float[1], ((float *)pMem) + 1) <= TraceOptions.Exp);
                break;
            case rt_DOUBLE_COMPLEX :
                nCmpRes = (REAL_REL_VAL(double, pValue->_complex_double[0], pMem) <= TraceOptions.Exp ) &&
                    (REAL_REL_VAL(double, pValue->_complex_double[1], ((double *)pMem) + 1) <= TraceOptions.Exp);
                break;
        }
    }
    else
    {
        if ( difType == 1 )
             /* cmp int & long */
             nCmpRes = INT_CMP(long, (long)pValue->_int, pMem);
        else /* cmp long & int */
        {
            long tmp = *((int *)pMem);
            nCmpRes = INT_CMP(long, pValue->_long, &tmp);
        }
    }

    return nCmpRes;
}

/* Return values: == 0 - error, !=0 - OK */
int trc_CompareValueExact(VALUE *pValue, void *pMem, DvmType lType, int difType)
{
    int nCmpRes = 0;

    if ( !difType )
    {
        switch (lType)
        {
            case rt_INT :
                nCmpRes = INT_CMP(int, pValue->_int, pMem);
                break;
            case rt_LOGICAL :
                difType = (PT_INT(pMem) != 0)?1:0;
                nCmpRes = INT_CMP(int, pValue->_int, &difType);
                break;
            case rt_LONG :
                nCmpRes = INT_CMP(long, pValue->_long, pMem);
                break;
            case rt_LLONG:
                nCmpRes = INT_CMP(long long, pValue->_longlong, pMem);
                break;
            case rt_FLOAT :
                nCmpRes = INT_CMP(float, pValue->_float, pMem);
                break;
            case rt_DOUBLE :
                nCmpRes = INT_CMP(double, pValue->_double, pMem);
                break;
            case rt_FLOAT_COMPLEX :
                nCmpRes = INT_CMP(float, pValue->_complex_float[0], pMem) &&
                        INT_CMP(float, pValue->_complex_float[1], ((float *)pMem) + 1);
                break;
            case rt_DOUBLE_COMPLEX :
                nCmpRes = INT_CMP(double, pValue->_complex_double[0], pMem) &&
                        INT_CMP(double, pValue->_complex_double[1], ((double *)pMem) + 1);
                break;
        }
    }
    else
    {
        if ( difType == 1 )
             /* cmp int & long */
             nCmpRes = INT_CMP(long, (long)pValue->_int, pMem);
        else /* cmp long & int */
        {
            long tmp = *((int *)pMem);
            nCmpRes = INT_CMP(long, pValue->_long, &tmp);
        }
    }

    return nCmpRes;
}

void trc_SubstituteRedVar(void *Value, VARIABLE *Var, DvmType vType)
{
    VarInfo* Varinfo = NULL;

    Varinfo = vartable_FindVar(&ReductVarTable, Value);
    if ( Varinfo )
    {
        if ( Varinfo->Type == rf_SUM || Varinfo->Type == rf_MULT )
        {
            switch( vType )
            {
                case rt_FLOAT :
                    *((float *) Value) = Var->val._float;
                    break;

                case rt_DOUBLE :
                    *((double *) Value) = Var->val._double;
                    break;

                case rt_FLOAT_COMPLEX :
                    *((float *) Value) = Var->val._complex_float[0];
                    *((float *) Value + 1) = Var->val._complex_float[1];
                    break;
                case rt_DOUBLE_COMPLEX :
                    *((double *) Value) = Var->val._complex_double[0];
                    *((double *) Value + 1) = Var->val._complex_double[1];
                    break;
            }
        }
    }
    else DBG_ASSERT(__FILE__, __LINE__, 0 );
}

void trc_SubstituteCommonVar(void *Value, VARIABLE *Var, DvmType vType)
{
    if ( Var )
    {
        switch( vType )
        {
            case rt_FLOAT :
                *((float *) Value) = Var->val._float;
                break;
            case rt_DOUBLE :
                *((double *) Value) = Var->val._double;
                break;
            case rt_FLOAT_COMPLEX :
                *((float *) Value) = Var->val._complex_float[0];
                *((float *) Value + 1) = Var->val._complex_float[1];
                break;
            case rt_DOUBLE_COMPLEX :
                *((double *) Value) = Var->val._complex_double[0];
                *((double *) Value + 1) = Var->val._complex_double[1];
                break;
        }
    }
    else DBG_ASSERT(__FILE__, __LINE__, 0 );
}


void trc_DelayTraceDestruct(DELAY_TRACE *trc)
{
    pCmpOperations->Variable(trc->File, trc->Line, "", trc_REDUCTVAR, trc->Type, trc->Value, 1, NULL);
}

DvmType trc_CalcLI(void)
{
    int i;
    DvmType Mult, LI;

    Mult = 1;
    LI = Trace.pCurCnfgInfo->CurIter[Trace.pCurCnfgInfo->Rank-1];

    for (i = Trace.pCurCnfgInfo->Rank - 2; i >= 0; i--)
    {
        Mult *= Trace.pCurCnfgInfo->Current[i+1].Upper - Trace.pCurCnfgInfo->Current[i+1].Lower + 1;
        LI += Mult * Trace.pCurCnfgInfo->CurIter[i];
    }

    return LI;
}

/* Calculate multidimensional index using linear index */

void trc_CalcSI(DvmType lIndex, byte bRank, DvmType rgSize[], DvmType *pSIndex)
{
    int i;
    DvmType *ip;
    DvmType mp;

    for (i = bRank - 2, mp = 1; i >= 0; --i)
        mp *= rgSize[i + 1];

    for (ip = pSIndex, i = 0; i < bRank; ++ip, ++i)
    {
        *ip = lIndex / mp;
        lIndex -= (*ip) * mp;
        if (i + 1 < bRank)
            mp /= rgSize[i + 1];
    }
}

/* Calculate multidimensional index using linear index of local part */

void trc_CalcPrimarySI(DvmType lIndex, s_BLOCK* pLocalBlock, DvmType *pSIndex)
{
    int i;
    DvmType *ip;
    DvmType mp;

    for (i = pLocalBlock->Rank - 2, mp = 1; i >= 0; --i)
        mp *= pLocalBlock->Set[i + 1].Size;

    for (ip = pSIndex, i = 0; i < pLocalBlock->Rank; ++ip, ++i)
    {
        *ip = lIndex / mp;
        lIndex -= (*ip) * mp;
        *ip += pLocalBlock->Set[i].Lower;
        if (i + 1 < pLocalBlock->Rank)
            mp /= pLocalBlock->Set[i + 1].Size;
    }
}

/* Calculate multidimensional index for remote buffer */

void trc_CalcRemoteSI(DvmType lIndex, byte bRank, DvmType rgSize[],
                      DvmType rgIndexMap[], DvmType *pSIndex)
{
    int i;

    trc_CalcSI(lIndex, bRank, rgSize, pSIndex);
    for (i = 0; i < bRank; ++i)
        if (-1 != rgIndexMap[i])
            pSIndex[i] = rgIndexMap[i];
}

DvmType trc_CalcArrayLI(byte bRank, DvmType* pSize, DvmType* pSIndex)
{
    int i;
    DvmType lMult, lIndex;

    lMult = 1;
    lIndex = pSIndex[bRank - 1];

    for (i = bRank - 2; i >= 0; i--)
    {
        lMult *= pSize[i + 1];
        lIndex += lMult * pSIndex[i];
    }

    return lIndex;
}
#endif /* _CMPTRACE_C_ */

