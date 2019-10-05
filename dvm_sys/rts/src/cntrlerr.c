#ifndef _CNTRLERR_C_
#define _CNTRLERR_C_
/******************/

#ifdef _UNIX_
    #include <unistd.h>
#else
    #include <io.h>
    #include <sys/locking.h>
    #include <share.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void cntx_Init(void)
{
    dvm_CONTEXT* pLevel = NULL;

    table_Init(&gContext, 10, sizeof(dvm_CONTEXT), NULL);

    pLevel = table_GetNew(dvm_CONTEXT, &gContext);
    pLevel->No = -1;
    pLevel->Rank = 0;
    pLevel->Type = 0;
    pLevel->ItersInit = 0;

    table_Init(&gPreSaveVars, 3, sizeof(PRESAVE_VARIABLE), NULL);
}

void cntx_Done(void)
{
    table_Done(&gContext);
    table_Done(&gPreSaveVars);

    dynmem_printstatistics();
}

void cntx_LevelInit(int No, byte Rank, byte Type, DvmType* pInit, DvmType* pLast, DvmType* pStep)
{
    dvm_CONTEXT* pLevel = NULL;
    int i;

    pLevel = table_GetNew(dvm_CONTEXT, &gContext);
    pLevel->No = No;
    pLevel->Rank = Rank;
    pLevel->Type = Type;
    pLevel->ItersInit = 0;

    if (pInit != NULL && pLast != NULL && pStep != NULL && Rank > 1)
    {
        for (i = 0; i < Rank; i++)
        {
            if (pInit[i] > pLast[i])
            {
                DvmType lv;

                pLevel->Limits[i].Lower = pLast[i];
                pLevel->Limits[i].Upper = pInit[i];
                pLevel->Limits[i].Step = -pStep[i];

                lv = (pInit[i] - pLast[i]) % (-pStep[i]);
                if (lv != 0)
                    pLevel->Limits[i].Lower += (pLevel->Limits[i].Step - lv);
            }
            else
            {
                pLevel->Limits[i].Lower = pInit[i];
                pLevel->Limits[i].Upper = pLast[i];
                pLevel->Limits[i].Step = pStep[i];
            }
        }
    }
    else
    {
        memset(pLevel->Limits, 0, sizeof(pLevel->Limits));
    }
}

void cntx_LevelDone(void)
{
    table_RemoveFrom(&gContext, table_Count(&gContext) - 2);
}

void cntx_SetIters(AddrType index[], DvmType IndexTypes[])
{
    int i;
    dvm_CONTEXT* pCntx = cntx_CurrentLevel();

    if (pCntx != NULL)
    {
        if (IndexTypes != NULL)
        {
            for (i = 0; i < pCntx->Rank; i++)
            {
                switch (IndexTypes[i])
                {
                    case 0 :
                        pCntx->Iters[i] = PT_LONG(index[i]);
                        break;
                    case 1 :
                        pCntx->Iters[i] = PT_INT(index[i]);
                        break;
                    case 2 :
                        pCntx->Iters[i] = PT_SHORT(index[i]);
                        break;
                    case 3 :
                        pCntx->Iters[i] = PT_CHAR(index[i]);
                        break;
                }
            }
        }
        else
        {
            for (i = 0; i < pCntx->Rank; i++)
                pCntx->Iters[i] = PT_LONG(index[i]);
        }

        pCntx->ItersInit = 1;
    }
}

dvm_CONTEXT* cntx_CurrentLevel(void)
{
    DvmType lCount = table_Count(&gContext);

    if (lCount == 0)
        return NULL;

    return table_At(dvm_CONTEXT, &gContext, lCount - 1);
}

dvm_CONTEXT* cntx_GetLevel(DvmType No)
{
    DvmType lCount = table_Count(&gContext);

    if (No >= lCount || No < 0)
        return NULL;

    return table_At(dvm_CONTEXT, &gContext, No);
}

DvmType cntx_LevelCount(void)
{
    return table_Count(&gContext);
}

DvmType cntx_GetParallelDepth(void)
{
    dvm_CONTEXT* pCntx = NULL;
    DvmType lRes = 0;
    DvmType i;

    for (i = table_Count(&gContext) - 1; i > 0 ; i--)
    {
        pCntx = table_At(dvm_CONTEXT, &gContext, i);
        if (pCntx->Rank > 0 && pCntx->Type != 0)
            lRes++;
    }

    return lRes;
}

DvmType cntx_GetInitParallelDepth(void)
{
    dvm_CONTEXT* pCntx = NULL;
    DvmType lRes = 0;
    DvmType i;

    for (i = table_Count(&gContext) - 1; i > 0 ; i--)
    {
        pCntx = table_At(dvm_CONTEXT, &gContext, i);
        if (pCntx->Rank > 0 && pCntx->ItersInit != 0 && pCntx->Type != 0)
            lRes++;
    }

    return lRes;
}

int cntx_IsInitParLoop(void)
{
    dvm_CONTEXT* pCntx = NULL;
    int nRes = 0;
    DvmType i;

    for (i = table_Count( &gContext ) - 1; i > 0 ; i--)
    {
        pCntx = table_At(dvm_CONTEXT, &gContext, i);
        if (pCntx->Rank > 0 && pCntx->Type != 0)
        {
            nRes = (pCntx->ItersInit == 0);
            break;
        }
    }

    return nRes;
}

DvmType cntx_GetAbsoluteParIter(void)
{
    dvm_CONTEXT* pCntx = NULL;
    DvmType lIndex = -1l;
    DvmType lMult;
    DvmType i;

    for (i = cntx_LevelCount() - 1; i >= 0; i--)
    {
        if (cntx_GetLevel(i)->Type != 0)
        {
            pCntx = cntx_GetLevel(i);
            break;
        }
    }

    if (pCntx != NULL && pCntx->ItersInit)
    {
        if (pCntx->Rank > 1)
        {
            lMult = 1l;
            lIndex = pCntx->Iters[pCntx->Rank - 1];

            for (i = pCntx->Rank - 2; i >= 0; i--)
            {
                lMult *= pCntx->Limits[i+1].Upper - pCntx->Limits[i+1].Lower + 1;
                lIndex += lMult * pCntx->Iters[i];
            }
        }
        else
            lIndex = pCntx->Iters[0];
    }

    return lIndex;
}

dvm_CONTEXT* cntx_GetParallelLevel(void)
{
    DvmType i;

    for (i = cntx_LevelCount() - 1; i >= 0; i--)
    {
        if (cntx_GetLevel(i)->Type != 0)
        {
            return cntx_GetLevel(i);
        }
    }

    return NULL;
}

int cntx_IsParallelLevel(void)
{
    return cntx_GetParallelLevel() != NULL;
}

char* cntx_FormatLevelString(char* Str)
{
    char *Pnt = Str;
    DvmType i, lCount;
    int j, l;
    dvm_CONTEXT* pCntx = NULL;

    lCount = cntx_LevelCount();
    if (lCount > 1)
    {
        for (i = 1; i < lCount; i++)
        {
            pCntx = cntx_GetLevel(i);
            if (i != 1)
            {
                SYSTEM(strcpy, (Pnt, ", "));
                Pnt += 2;
            }
            if (pCntx->Type == 2)
            {
                SYSTEM_RET(l, sprintf, (Pnt, "TaskRegion( No(%d)", pCntx->No));
            }
            else
            {
                SYSTEM_RET(l, sprintf, (Pnt, "Loop( No(%d)", pCntx->No));
            }
            Pnt += l;
            if (pCntx->ItersInit)
            {
                for (j = 0; j < pCntx->Rank; j++)
                {
                    if (j == 0)
                    {
                        if (pCntx->Type == 2)
                        {
                            SYSTEM_RET(l, sprintf, (Pnt, ", Task(%ld", pCntx->Iters[j]));
                        }
                        else
                        {
                            SYSTEM_RET(l, sprintf, (Pnt, ", Iter(%ld", pCntx->Iters[j]));
                        }
                    }
                    else
                    {
                        SYSTEM_RET(l, sprintf, (Pnt, ",%ld", pCntx->Iters[j]));
                    }
                    Pnt += l;
                }
                *(Pnt++) = ')';
            }
            SYSTEM(strcpy, (Pnt, " )"));
            Pnt += 2;
        }
        SYSTEM(strcpy, (Pnt, ". " ));
        Pnt += 2;
    }
    else
    {
        SYSTEM(strcpy,( Pnt, "Sequential branch. "));
        SYSTEM_RET(l,strlen,(Pnt));
        Pnt += l;
    }

    return Pnt;
}


void error_Init( ERRORTABLE *errTable, int MaxErrors )
{
    table_Init( &(errTable->tErrors), MaxErrors + 1, sizeof(ERROR_RECORD), NULL );
    errTable->MaxErrors = MaxErrors;
    errTable->ErrCount = 0;
}

void error_Done( ERRORTABLE *errTable )
{
    table_Done( &(errTable->tErrors) );
    errTable->MaxErrors = 0;
}

ERROR_RECORD *error_Put(ERRORTABLE *errTable, char *File, UDvmType Line, char *Context, char *Message, int StructNo, DvmType CntxNo, int trccpu, UDvmType trcline)
{
    ERROR_RECORD *pErr;

    pErr = table_GetNew(ERROR_RECORD, &(errTable->tErrors));

    pErr->Count = 1;
    SYSTEM(strncpy, (pErr->File, File, MAX_ERR_FILENAME));
    pErr->File[MAX_ERR_FILENAME] = 0;

    SYSTEM(strncpy, (pErr->Context, Context, MAX_ERR_CONTEXT));
    pErr->Context[MAX_ERR_CONTEXT] = 0;

    SYSTEM(strncpy, (pErr->Message, Message, MAX_ERR_MESSAGE));
    pErr->Message[MAX_ERR_MESSAGE] = 0;

    pErr->Line = Line;
    pErr->StructNo = StructNo;
    pErr->CntxNo  = CntxNo;
    pErr->TrcCPU  = trccpu;
    pErr->TrcLine = trcline;
    pErr->RealCPU = Trace.CurCPUNum;
    pErr->CPUList = NULL;

    pErr->ErrTime = dvm_get_time();
#ifdef _MPI_PROF_TRAN_
    pErr->Primary = 0;
#endif

    return pErr;
}

ERROR_RECORD *error_Find(ERRORTABLE *errTable, char *File, UDvmType Line, char *Message, int StructNo, DvmType CntxNo)
{
    ERROR_RECORD *pErr;
    int CmpRes;
    DvmType i, Count = table_Count( &(errTable->tErrors) );

    for ( i = 0; i < Count; i++ )
    {
        pErr = table_At( ERROR_RECORD, &(errTable->tErrors), i );
        if( pErr->Line != Line || pErr->StructNo != StructNo || pErr->CntxNo != CntxNo )
            continue;

        SYSTEM_RET(CmpRes, strcmp,(pErr->File,File));
        if( CmpRes )
            continue;

        SYSTEM_RET(CmpRes, strcmp,(pErr->Message,Message));
        if( CmpRes )
            continue;

        return pErr;
    }

    return NULL;
}

ERR_INFO *errInfo_Find(TABLE *errInfoTable, char *File, UDvmType Line, char *Operand, DvmType vtype)
{
    ERR_INFO *pErrInfo;
    int CmpRes;
    DvmType i, Count = table_Count( errInfoTable );

    for ( i = 0; i < Count; i++ )
    {
        pErrInfo = table_At( ERR_INFO, errInfoTable, i );
        if( pErrInfo->Line != Line || pErrInfo->vtype != vtype )
            continue;

        SYSTEM_RET(CmpRes, strcmp,(pErrInfo->File, File));
        if( CmpRes )
            continue;

        SYSTEM_RET(CmpRes, strcmp,(pErrInfo->Operand, Operand));
        if( CmpRes )
            continue;

        return pErrInfo;
    }

    return NULL;
}

byte error_Message(char *To, ERRORTABLE *errTable, char *File, UDvmType Line, char *Context, char *Message, int trccpu, UDvmType trcline)
{
    ERROR_RECORD *pErr;
    int LoopNo;
    DvmType CntxNo;
    byte Res = 1;

    LoopNo = cntx_CurrentLevel()->No;
    CntxNo = cntx_LevelCount() - 1;

    pErr = error_Find( errTable, File, Line, Message, LoopNo, CntxNo );
    if( pErr )
    {
        pErr->Count++;
    }
    else
    {
        errTable->ErrCount++;
        pErr = error_Put( errTable, File, Line, Context, Message, LoopNo, CntxNo, trccpu, trcline );
        Res = error_Print( To, pErr );
    }

    return Res;
}

byte error_Print( char *To, ERROR_RECORD *pErr )
{
    FILE *ErrOut = stderr;
    byte Res = 1;

    if( To != NULL )
    {
        ErrOut = error_OpenFile( To );
        if( !ErrOut )
        {
            ErrOut = stderr;
            Res = 0;
        }
    }

    error_PrintRecord( ErrOut, pErr );

    if( ErrOut != stderr )
        SYSTEM( fclose, ( ErrOut ));

    return Res;
}

void error_PrintRecord( FILE *stream, ERROR_RECORD *pErr )
{
    static char Buff[ MAX_ERR_FILENAME + MAX_ERR_CONTEXT + MAX_ERR_MESSAGE ];
    static char Count2[45]; /* "Primary      Count(12345)" */
    char        *Pnt2;
#ifdef _MPI_PROF_TRAN_
    int         l2;
#endif

    int l;
    char *Pnt, *tmp;

    #ifdef _UNIX_
       int handle;
    #endif

    if ( stream == NULL )
            return;

    Pnt = Buff;
    if( (Trace.RealCPUCount > 1) && (stream != stderr) )
    {
        SYSTEM_RET( l, sprintf, ( Pnt, "(%d)", pErr->RealCPU ) );
        Pnt += l;
    }
    SYSTEM_RET( l, sprintf, ( Pnt, "%s\n\t\t", pErr->Context ) );
    Pnt += l;

    SYSTEM_RET(tmp,strstr,(pErr->Message,"%"))  // hack

    if ( tmp != NULL )
    {
        if ( pErr->CPUList )
        {
            SYSTEM( fputs, ( Buff, stream ) );
            Pnt = Buff;
        }

        Pnt2 = Count2;

#ifdef _MPI_PROF_TRAN_
        if ( dvm_OneProcSign )
        {                              /* may be it should be commented some time but leave it for debugging */
            SYSTEM_RET( l2, sprintf, ( Pnt2, "ErrTime(%lu)   ", pErr->ErrTime ) );
            Pnt2 += l2;

            if ( pErr->Primary == 1 )
            {
                SYSTEM_RET(l2, sprintf, (Pnt2, "Primary      "));
                Pnt2 += l2;
            }
            else
                if ( pErr->Primary == -1 )
                {
                    SYSTEM_RET(l2, sprintf, (Pnt2, "p.Secondary  "));
                    Pnt2 += l2;
                }
        }
#endif

        if( pErr->Count > 1 )
        {
            sprintf (Pnt2, "Count(%d)", pErr->Count);
        }
        else
            *Pnt2 = 0; /* empty string */

        if ( pErr->CPUList )
        {
            char *tmp = writeCPUListString(pErr->CPUList, 0);

            if ( (strlen(Count2)!=0) || (strlen(tmp)!=0) )
                strcpy(tmp+strlen(tmp), "\n\t\t");  // add newline transition if there are symbols

            SYSTEM_RET( l, fprintf, ( stream, pErr->Message, Count2, tmp,
                                        pErr->File, pErr->Line ) );
        }
        else
        {
            SYSTEM_RET( l, sprintf, ( Pnt, pErr->Message, Count2, (strlen(Count2)!=0)?"\n\t\t":"",
                                        pErr->File, pErr->Line ) );
            Pnt += l;
        }
    }
    else
    {
        SYSTEM_RET( l, sprintf, ( Pnt, "%s", pErr->Message ) );
        Pnt += l;

        if( pErr->Count > 1 )
        {
            SYSTEM_RET( l, sprintf, ( Pnt,  "\n\t\tCount(%d)  ", pErr->Count) );
            Pnt += l;
        }

        if ( pErr->CPUList )
        {
            /* there are other CPUs in the list */
            SYSTEM( fputs, ( Buff, stream ) );
            Pnt = Buff;

            SYSTEM( fputs, ( writeCPUListString(pErr->CPUList, 0), stream ) );
        }

        if( pErr->File[0] != 0 )
        {
            SYSTEM_RET( l, sprintf, ( Pnt, "\n\t\tFile: %s  Line: %lu", pErr->File, pErr->Line ) );
            Pnt += l;
        }

 /*       if( (pErr->TrcLine > 0) || (pErr->TrcCPU  >= 0) )
        {
            SYSTEM_RET( l, sprintf, ( Pnt, "\n\t\tTrcRec=%*lu\n\t\tTrcCPU=%*d", 10, pErr->TrcLine, 10, pErr->TrcCPU ) );
            Pnt += l;
        }*/  // disabled as extra - information
    }

    *Pnt = '\n'; Pnt ++; *Pnt = 0;

    if(stream == stderr)
        pprintf(3, "%s", Buff);
    else
    {
        SYSTEM( fputs, ( Buff, stream ) );
    }

    SYSTEM( fflush, (stream) );

    #ifdef _UNIX_
        SYSTEM_RET(handle, fileno, (stream))
        SYSTEM(fsync, (handle))
    #endif
}

/*
 * Name = Either NULL or short filename
 * */
FILE *error_OpenFile( char *Name )
{
    static char ErrFileName[MaxParFileName + MaxPathSize + 1] = "";
    FILE *Res = stdout;
    int i;

    if( Name != NULL )
    {
        if ( TraceOptions.TracePath != NULL )
            SYSTEM(sprintf, (ErrFileName, "%s%s", TraceOptions.TracePath, Name ));

        for( i = 0, Res = NULL; (i < 100) && !Res; i++ )
        {
            SYSTEM_RET( Res, fopen, ( ErrFileName, OPENMODE(a) ) );
        }
        if( Res == NULL )
        {
            pprintf(3, "*** RTS err: Can't open error-file <%s>\n", Name);
        }
    }

    return Res;
}

void errInfo_PrintAll(char *FileName, ERR_INFO **ppErrInfo, int EntriesCount, byte finalprot )
{
    FILE *ProtFile;
    int i;

    SYSTEM(remove, (FileName));
    SYSTEM_RET(ProtFile, fopen, (FileName, OPENMODE(w)));

    if ( !ProtFile )
    {
        pprintf(3, "*** RTS err: Can't open protocol file <%s> for writing\n", FileName);
        return ;
    }

    SYSTEM(fprintf, ( ProtFile, "\nDVM debugger version %s\n" \
                        "==============================================\n\n",
                        DEBUGGER_VERSION) )

    //SYSTEM(fprintf, ( ProtFile, "Source trace name = %s.\n\n",  ) )

    SYSTEM(fprintf, ( ProtFile, "TraceOptions.Exp = %.*lG\n", Trace.DoublePrecision, TraceOptions.Exp ) )
    SYSTEM(fprintf, ( ProtFile, "TraceOptions.ExpIsAbsolute = %d\n", TraceOptions.ExpIsAbsolute ) )
    SYSTEM(fprintf, ( ProtFile, "TraceOptions.SRCLocCompareMode = %d\n", TraceOptions.SRCLocCompareMode ) )

    if ( !finalprot  )
    {
        SYSTEM(fprintf, ( ProtFile, "CPU number = %d\n", Trace.CurCPUNum ) )

        if ( ! EnableTrace )
            SYSTEM(fprintf, ( ProtFile, "\nABNORMAL termination: %s\n", error_message ))

        if( TraceCompareErrors.ErrCount > 0 )
        {
            DvmType last = table_GetLastAccessed(&Trace.tTrace);

            if ( TraceCompareErrors.ErrCount >= TraceOptions.MaxErrors)
            {
                SYSTEM(fprintf, ( ProtFile, "Found %lu matched events and >= %d errors (%s%s.err.trd).\n\n",
                            Trace.MatchedEvents, TraceOptions.MaxErrors, TraceOptions.TracePath, dvm_argv[0]) )

            }
            else
            {
                SYSTEM(fprintf, ( ProtFile, "Found %lu matched events and %d errors (%s%s.err.trd).\n\n",
                            Trace.MatchedEvents, TraceCompareErrors.ErrCount, TraceOptions.TracePath, dvm_argv[0]) )
            }

            if ( (last != -1) && (last != (table_Count(&Trace.tTrace)-1) ) )
            {
                SYSTEM(fprintf, ( ProtFile, "*** Warning: debugger did not reach the end of trace:\n" ))
                SYSTEM(fprintf, ( ProtFile, "last accessed internal trace record is %ld of total records %ld.\n\n",
                                  last, table_Count(&Trace.tTrace) ) )
            }
        }
    }

    /* sort array elements by appropriate leaps */
    qsort(ppErrInfo, EntriesCount, sizeof(ERR_INFO*), &errinfo_compare);

    SYSTEM(fprintf, ( ProtFile, "----------------------------------------------------------------------------------------\n" \
                    ) )

    if (EntriesCount > 0)
    {
        /* output final records */
        for (i=0; i < EntriesCount; i++)
        {
        SYSTEM(fprintf, ( ProtFile, "\n\t\tVar. name = %s, File = %s, Line = %lu.\n\n", \
                        (*(ppErrInfo + i))->Operand,(*(ppErrInfo + i))->File, (*(ppErrInfo + i))->Line ) )

        if ( !finalprot  )
        {
            SYSTEM(fprintf, ( ProtFile, "Hit count          %lu\n", (*(ppErrInfo + i))->HitCount ) )
        }
        else
        {
            SYSTEM(fprintf, ( ProtFile, "Hit count          Sum   =%*lu %s\n", 10, (*(ppErrInfo + i))->HitCount,    writeCPUListString((*(ppErrInfo + i))->CPULists, 0) ) )

            if (Trace.RealCPUCount > 1)
            {
                SYSTEM(fprintf, ( ProtFile, "Hit count          Max   =%*lu %s\n", 10, (*(ppErrInfo + i))->MaxHitCount, writeCPUListString((*(ppErrInfo + i))->CPULists, 1) ) )
            }
        }

        if ( !finalprot  )
        {

        SYSTEM(fprintf, ( ProtFile, "First dif          TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->firstHitLocC,
                        10, "",
                        (*(ppErrInfo + i))->firstHitLoc ))
        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  dif_r=%- .*lE\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->firstHitValT), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->firstHitRel ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  dif_a=%- .*lE\n\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->firstHitValE), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->firstHitAbs ))
        }
        else
        {
        SYSTEM(fprintf, ( ProtFile, "Max first dif      TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->firstHitLocC,
                        10, "",
                        (*(ppErrInfo + i))->firstHitLoc ))

        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  dif_r=%- .*lE%s\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->firstHitValT), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->firstHitRel, TraceOptions.ExpIsAbsolute?"":writeCPUListString((*(ppErrInfo + i))->CPULists, 2) ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  dif_a=%- .*lE%s\n\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->firstHitValE), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->firstHitAbs, TraceOptions.ExpIsAbsolute?writeCPUListString((*(ppErrInfo + i))->CPULists, 2):"" ))
        }

        SYSTEM(fprintf, ( ProtFile, "Max relative diff  TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->maxRelAccLocC,
                        10, "",
                        (*(ppErrInfo + i))->maxRelAccLoc ))
        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  dif_r=%- .*lE%s\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxRelAccValT), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxRelAcc, finalprot?writeCPUListString((*(ppErrInfo + i))->CPULists, 3):"" ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  dif_a=%- .*lE\n\n",
                        Trace.DoublePrecision+5, trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxRelAccValE), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxRelAccAbs ))

        SYSTEM(fprintf, ( ProtFile, "Max relative leap  TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->maxLeapRLocC,
                        10, "",
                        (*(ppErrInfo + i))->maxLeapRLoc ))
        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  leapr=%- .*lE%s\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxLeapRelValT), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxLeapRel, finalprot?writeCPUListString((*(ppErrInfo + i))->CPULists, 4):"" ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  leapa=%- .*lE\n\n",
                        Trace.DoublePrecision+5, trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxLeapRelValE), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxLeapRelAbs ))

        SYSTEM(fprintf, ( ProtFile, "Max absolute diff  TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->maxAbsAccLocC,
                        10, "",
                        (*(ppErrInfo + i))->maxAbsAccLoc ))
        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  dif_r=%- .*lE\n",
                        Trace.DoublePrecision+5, trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxAbsAccValT), (*(ppErrInfo + i))->vtype),\
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxAbsAccRel ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  dif_a=%- .*lE%s\n\n",
                        Trace.DoublePrecision+5, trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxAbsAccValE), (*(ppErrInfo + i))->vtype),\
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxAbsAcc, finalprot?writeCPUListString((*(ppErrInfo + i))->CPULists, 5):"" ))

        SYSTEM(fprintf, ( ProtFile, "Max absolute leap  TraceFile(%d.trd)%-*s   TrcRec= %lu\n",
                        (*(ppErrInfo + i))->maxLeapALocC,
                        10, "", (*(ppErrInfo + i))->maxLeapALoc ))
        SYSTEM(fprintf, ( ProtFile, "                   val1=%-*s  leapr=%- .*lE\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxLeapAbsValT), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxLeapAbsRel ))
        SYSTEM(fprintf, ( ProtFile, "                   val2=%-*s  leapa=%- .*lE%s\n\n",
                        Trace.DoublePrecision+5,
                        trc_SprintfValue(FileName, &((*(ppErrInfo + i))->maxLeapAbsValE), (*(ppErrInfo + i))->vtype),
                        Trace.DoublePrecision, (*(ppErrInfo + i))->maxLeapAbs, finalprot?writeCPUListString((*(ppErrInfo + i))->CPULists, 6):"" ))

        SYSTEM(fprintf, ( ProtFile, "----------------------------------------------------------------------------------------\n" \
                        ) )
        }
    }
    else
    {
        if ( !finalprot  )
            SYSTEM(fprintf, ( ProtFile, "Found %lu matched events; %d errors.\n", Trace.MatchedEvents, TraceCompareErrors.ErrCount ) )
        else
            SYSTEM(fprintf, ( ProtFile, "%d errors.\n", TraceCompareErrors.ErrCount ) )
    }

    SYSTEM(fprintf, ( ProtFile, "\n==============================================\n" ))
    SYSTEM(fclose,  ( ProtFile ));
}

/*
 * File "To" should be removed before this call if necessary
 * */
void error_PrintAll( char *To, ERRORTABLE *errTable, byte timesort )
{
    ERROR_RECORD *pErr, **pErr2;
    FILE *ErrOut = stderr;
    DvmType i, Count = table_Count( &(errTable->tErrors) );

    if( To != NULL )
    {
        ErrOut = error_OpenFile( To );
        if( !ErrOut )
        {
            ErrOut = stderr;
        }
    }

    if(ErrOut == stderr)
       pprintf(3, "*** Total errors: %d; Limit per CPU: %d\n\n",
                  errTable->ErrCount, errTable->MaxErrors);
    else
    {
        if ( ! timesort )
        {
            SYSTEM( fprintf, ( ErrOut, "\n*** Processor %d: total errors: %d; Limit per CPU: %d\n\n",
                                Trace.CurCPUNum, errTable->ErrCount, errTable->MaxErrors ) )
        }
        else
        {
            SYSTEM( fprintf, ( ErrOut, "\n*** Total errors: %d; Limit per CPU: %d\n\n",
                                errTable->ErrCount, errTable->MaxErrors ) )
        }
    }

    if ( !timesort )
    {
        for ( i = 0; i < Count; i++ )
        {
            pErr = table_At( ERROR_RECORD, &(errTable->tErrors), i );
            error_PrintRecord( ErrOut, pErr );
        }
    }
    else
    {
        mac_malloc(pErr2, ERROR_RECORD **, Count*sizeof(ERROR_RECORD *), 0);
        /* no error handling is required */
        for ( i = 0; i < Count; i++ )
        {
            *(pErr2+i) = table_At( ERROR_RECORD, &(errTable->tErrors), i );
        }

        stablesort(pErr2, Count, sizeof(ERR_INFO*), &err_record_compare);

        for ( i = 0; i < Count; i++ )
        {
            error_PrintRecord( ErrOut, *(pErr2+i) );
        }
        mac_free( &pErr2 );
    }

    if( ErrOut != stderr )
        SYSTEM( fclose, ( ErrOut ));
}

void error_DynControl( int code, ... )
{
    static char Buff[MAX_ERR_CONTEXT];
    static char Msg[MAX_ERR_MESSAGE];
    char *Pnt;
    int l;
    va_list ap;

    if( code > ERR_LAST ) code = ERR_LAST;

    switch( code )
    {
        case ERR_DYN_WRITE_RO :
            if( !DebugOptions.CheckVarReadOnly )
                return;
            break;
        case ERR_DYN_PRIV_NOTINIT :
            if( !DebugOptions.CheckVarInitialization )
                return;
            break;
        case ERR_DYN_DISARR_NOTINIT :
            if( !DebugOptions.CheckDisArrInitialization )
                return;
            break;
        case ERR_DYN_ARED_NOCOMPLETE :
        case ERR_DYN_REDUCT_WAIT_BSTART :
        case ERR_DYN_REDUCT_START_WITHOUT_WAIT :
        case ERR_DYN_REDUCT_NOT_STARTED :
            if( !DebugOptions.CheckReductionAccess )
                return;
            break;
        case ERR_DYN_DATA_DEPEND :
            if( !DebugOptions.CheckDataDependence )
                return;
            break;
        case ERR_DYN_BOUND_RENEW_NOCOMPLETE :
            if( !DebugOptions.CheckDisArrEdgeExchange )
                return;
            break;
        case ERR_DYN_WRITE_REMOTEBUFF :
            if( !DebugOptions.CheckRemoteBufferAccess )
                return;
            break;
        case ERR_DYN_SEQ_WRITEARRAY :
        case ERR_DYN_SEQ_READARRAY :
            if( !DebugOptions.CheckDisArrSequentialAccess )
                return;
            break;
        case ERR_DYN_DISARR_LIMIT :
            if( !DebugOptions.CheckDisArrLimits )
                return;
            break;
        case ERR_DYN_NONLOCAL_ACCESS :
        case ERR_DYN_WRITE_IN_BOUND :
            if( !DebugOptions.CheckDisArrLocalElm )
                return;
            break;
    }

    SYSTEM(strcpy,( Buff, " *** DYNCONTROL *** : " ));
    SYSTEM_RET(l,strlen,(Buff));
    Pnt = Buff + l;
    Pnt = cntx_FormatLevelString( Pnt );

    va_start( ap, code );
    SYSTEM( vsprintf, ( Msg, ErrString[code], ap ) );
    va_end( ap );

    if( error_Message( DebugOptions.ErrorToScreen ? NULL : DebugOptions.ErrorFile, &DynControlErrors, DVM_FILE[0], DVM_LINE[0], Buff, Msg, -1, 0 ) == 0 )
        DebugOptions.ErrorToScreen = 1;

    if( DynControlErrors.ErrCount >= DynControlErrors.MaxErrors )
    {
        EnableDynControl = 0;
    }
}

void error_DynControlPrintAll(void)
{
    if( DynControlErrors.ErrCount > 0 )
    {
        pprintf(3, "*** Dynamic control found %d error(s) ***\n", DynControlErrors.ErrCount );
        error_PrintAll( DebugOptions.ErrorToScreen ? NULL : DebugOptions.ErrorFile, &DynControlErrors, 0 );
    }
}

void error_CmpTrace(char *File, UDvmType Line, int code)
{
    char Msg[MAX_ERR_MESSAGE];

    SYSTEM( sprintf, ( Msg, "%s", ErrString[code]) );

    if ( ErrFatality[code]  )
        SYSTEM( sprintf, ( Msg+strlen(Msg), " (fatal)") );

    /* this function is supposed to be used exclusively for output read trace (fatal) errors */
    if( code > ERR_LAST ) code = ERR_LAST;
    Trace.ErrCode = code;

    if( error_Message( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, File, Line, " *** READTRACE *** : ", Msg, Trace.CPU_Num, Line  ) == 0 )
        TraceOptions.ErrorToScreen = 1;

    EnableTrace = 0;

    Trace.TerminationStatus = 1; /* abnormal termination. Save error string to be writen in protocol */
    sprintf(error_message, "*** READTRACE *** : %s, File=%s, Line=%lu", ErrString[code], File, Line);
}

/*
 * File and Line arguments in this function have different semantics
 */
void error_CmpTraceExt(DvmType RecordNo, char *File, UDvmType Line, int code, ...)
{
    va_list ap;
    char *Pnt;
    int l;
    ANY_RECORD  *pAny  = NULL;
    static char Buff[MAX_ERR_CONTEXT];
    static char Msg[MAX_ERR_MESSAGE];

    if ( TraceCompareErrors.ErrCount < TraceCompareErrors.MaxErrors )
    {
        if( code > ERR_LAST ) code = ERR_LAST;
        Trace.ErrCode = code;

        if( RecordNo <= 0 )
        {
            RecordNo = table_GetLastAccessed(&Trace.tTrace) + 1;  // next record after the record that was accessed last
        }

        if (( RecordNo >= table_Count(&Trace.tTrace) ) || (RecordNo <= 0) )
        {
            if ( table_Count(&Trace.tTrace) > 0 )
                /* erroneous error location, should correct :)*/
                pAny = table_GetBack(ANY_RECORD, &Trace.tTrace);
        }
        else
            pAny = table_At(ANY_RECORD, &Trace.tTrace, RecordNo);

        SYSTEM_RET(l,sprintf,( Buff, ": TrcFile(%s%s%d.%s); TrcRec(%lu); ", TraceOptions.TracePath,
                     TraceOptions.InputTracePrefix, pAny?pAny->CPU_Num:-1, pAny?TraceOptions.Ext:"-", pAny?pAny->Line_num:-1 ));

        SYSTEM_RET(l,strlen,(Buff));
        Pnt = Buff + l;

        Pnt = cntx_FormatLevelString( Pnt );

        va_start( ap, code );
        SYSTEM( vsprintf, ( Msg, ErrString[code], ap ) );
        va_end( ap );

        if ( ErrFatality[code]  )
        {
            SYSTEM_RET(l,sprintf,( Msg+strlen(Msg), " (fatal)"));
        }

        if( error_Message( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, File, Line, Buff, Msg, pAny?(pAny->CPU_Num):(-1), pAny?(pAny->Line_num):0 ) == 0 )
            TraceOptions.ErrorToScreen = 1;

        if( Trace.Mode == 0 )
        {
            EnableTrace = 0;
            return;
        }
    }
                        // save trace and disable debugger if error is fatal
    if ( ErrFatality[code] )
    {
        cmptrace_Done();
        if (Trace.convMode == -1)
            pprintf(3, "*** DVM debugger warning: Fatal error was detected, debugger was disabled.\n");
        else
        {
            pprintf(3, "*** DVM debugger: Fatal error was detected in -dbif* instrumentation mode, program was terminated.\n");
            exit (-1);
        }
    }
}

void error_CmpTracePrintAll(void)
{
    char           FileName[MaxParFileName + MaxPathSize + 1];
    int            i, j, CurElem = 0;
    FILE           *binf;
    int            binfd;
    dvm_ARRAY_INFO *pArr;
    DvmType           EntriesCount = 0;
    DvmType           Count1, Count2;
    ERROR_RECORD   ErrRec;
    ERROR_RECORD   *pErr;
    ERR_INFO       ErrInfo;
    ERR_INFO       *pErrInfo;
    ERR_INFO       **ppErrInfo = NULL;
#ifdef _UNIX_
    struct flock l;
#endif

    if( TraceCompareErrors.ErrCount > 0 )
    {
        pprintf(3, "*** Found %d error(s) ***\n", TraceCompareErrors.ErrCount );
        error_PrintAll( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, 0 );
    }

    if ( TraceOptions.TraceMode == 3 )
    {
        /* generate protocol filename */
        SYSTEM(sprintf, (FileName, "%s%s.%d.prot.trd",
                TraceOptions.TracePath, dvm_argv[0],
                Trace.CurCPUNum ));

        /* count memory size for final protocol table */
        for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
        {
            pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);
            EntriesCount += table_Count(&(pArr->tErrEntries));
        }
        EntriesCount += table_Count(&Trace.tVarErrEntries);

        /* allocate memory for pointers to sort them */
        mac_malloc(ppErrInfo, ERR_INFO**, EntriesCount*sizeof(ERR_INFO *), 0);

        /* save pointers in array */
        for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
        {
            pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

            for (j = table_Count(&(pArr->tErrEntries))-1; j>=0; j--)
            {
                *(ppErrInfo+CurElem) = table_At(ERR_INFO, &(pArr->tErrEntries), j);
                strcpy((*(ppErrInfo+CurElem))->Operand, pArr->szOperand);
                CurElem++;
            }
        }
        for (j = table_Count(&Trace.tVarErrEntries)-1; j>=0; j--)
        {
            *(ppErrInfo+CurElem) = table_At(ERR_INFO, &Trace.tVarErrEntries, j);
            CurElem++;
        }

        DBG_ASSERT(__FILE__, __LINE__, EntriesCount == CurElem);

        errInfo_PrintAll(FileName, ppErrInfo, EntriesCount, 0 );

        /* create binary files with errors and leaps information for further reading by IO CPU */
        /* do not create this file on IO CPU */

        if ( Trace.CurCPUNum == 0 )
                /* read files branch */
        {
            /* merge leaps into one table for further merge with ones read form files */
            for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
            {
                pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

                for (j = table_Count(&(pArr->tErrEntries))-1; j>=0; j--)
                {
                    table_Put( &(Trace.tVarErrEntries), table_At(ERR_INFO, &(pArr->tErrEntries), j) );
                    /* Operand field is already filled in */
                }
                /* free the memory of the tables pArr->tErrEntries */
                table_Done(&(pArr->tErrEntries));
            }

            /* initialize CPULists in order if no leaps will be found on other CPUs
             * => otherwise these fields will not be initialized */
            if ( Trace.RealCPUCount > 1 )
            {
                for ( i = table_Count(&(Trace.tVarErrEntries))-1; i>=0; i-- )
                {
                    pErrInfo = table_At( ERR_INFO, &(Trace.tVarErrEntries), i );

                    DBG_ASSERT(__FILE__, __LINE__, !pErrInfo->MaxHitCount );

                    pErrInfo->MaxHitCount = pErrInfo->HitCount;
                    update_CPUlist(&(pErrInfo->CPULists), 0, 1, 0);
                    update_CPUlist(&(pErrInfo->CPULists), 1, 1, 0);
                    update_CPUlist(&(pErrInfo->CPULists), 2, 1, 0);
                    update_CPUlist(&(pErrInfo->CPULists), 3, 1, 0);       /* add ioproc to all lists */
                    update_CPUlist(&(pErrInfo->CPULists), 4, 1, 0);
                    update_CPUlist(&(pErrInfo->CPULists), 5, 1, 0);
                    update_CPUlist(&(pErrInfo->CPULists), 6, 1, 0);
                }
            }

            /* classify errors form IO CPU on primary and secondary */

#ifdef _MPI_PROF_TRAN_
            if ( dvm_OneProcSign )
            {
                DBG_ASSERT(__FILE__, __LINE__, Trace.ErrTimes); /* NEED TO BE CORRECTED !!! */

                Count1 = table_Count(&(TraceCompareErrors.tErrors));

                if ( Count1 > 0 )
                {
                    pErr = table_At( ERROR_RECORD, &(TraceCompareErrors.tErrors), 0 );

                    /* !!! '<' is possible if DVM error is got earlier then MPI message passing occured */

                    if ( pErr->ErrTime <= Trace.ErrTimes[0] )
                    {
                        /* -1 - pot. secondary, 1 - primary */
                        pErr->Primary = 1;
                    }
                    else
                        pErr->Primary = -1;

                    for ( i=Count1-1; i>0; i-- ) /* strict > 0 is necessary */
                    {
                        table_At( ERROR_RECORD, &(TraceCompareErrors.tErrors), i )->Primary = -1;
                    }
                }
            }
#endif

            /* read files and merge data loop */
            for ( j = 1; j < Trace.RealCPUCount; j++ ) /* skip io_proc CPU */
            {
                SYSTEM(sprintf, (FileName, "%s%s.%d.bin.trd",
                        TraceOptions.TracePath, dvm_argv[0], j ));

                for( i = 0, binf = NULL; (i < 100) && !binf; i++ ) /* 100 attempts to open the file with 1 sec delay */
                {
                    SYSTEM_RET( binf, fopen, ( FileName, "rb" ) );

                    /* in win32 we'll lock here until the file is closed by the writer process */

                    if( binf == NULL )
                        SLEEP(1);
                }

                if( binf == NULL )
                {
                    pprintf(3, "*** RTS err: Can't open binary file <%s>. Some errors and leaps information may be lost.\n", FileName);
                    continue ;
                }

                for( i = 0; (i < 100) && feof(binf); i++ ) /* 100 attempts to wait for inforation */
                     SLEEP(1);

                if( feof(binf) )
                {
                    pprintf(3, "*** RTS err: Information in binary file <%s> did not come. Some errors and leaps information may be lost.\n", FileName);
                    continue ;
                }
#ifdef _UNIX_
                else
                {
                    l.l_type = F_RDLCK;
                    l.l_whence = SEEK_SET;
                    l.l_start = 0;
                    l.l_len = 0;
                    if ( fcntl(fileno(binf),F_SETLKW,&l) == -1 )
                    {
                        SYSTEM(fclose,  ( binf ));
                        pprintf(3, "*** RTS err: Waiting on lock for binary file <%s> error. Some errors and leaps information may be lost.\n", FileName);
                        continue ;
                    }
                }
#endif

                /* read and merge information from the source file to generate final errors and leaps files */
                if ( feof(binf) || ( fread(&Count1, sizeof(DvmType), 1, binf) < 1 ) ||
                                    ( fread(&Count2, sizeof(DvmType), 1, binf) < 1 ) )
                {
                    pprintf(3, "*** RTS err: Binary file <%s> read error. Process terminated.\n", FileName);
                    exit (-1);
                }

                for ( i=0; i < Count1; i++)
                {
                    if (feof(binf) || ( fread(&ErrRec, sizeof(ERROR_RECORD), 1, binf) < 1 ) )
                    {
                        pprintf(3, "*** RTS err: Binary file <%s> read error. Process terminated.\n", FileName);
                        exit (-1);
                    }

                    DBG_ASSERT(__FILE__, __LINE__, j == ErrRec.RealCPU);

#ifdef _MPI_PROF_TRAN_
                    if ( dvm_OneProcSign )
                    {
                        if ( i == 0 ) /* first error on CPU i */
                        {
                            /* !!! '<' is possible if DVM error is got earlier
                             * then MPI message passing occured */

                            if ( ErrRec.ErrTime <= Trace.ErrTimes[j] )
                            {
                                /* -1 - pot. secondary, 1 - primary */
                                ErrRec.Primary = 1;
                            }
                            else
                                ErrRec.Primary = -1;
                        }
                        else
                            ErrRec.Primary = -1; /* anyway secondary */
                    }
#endif

                    pErr = error_Find( &TraceCompareErrors, ErrRec.File, ErrRec.Line, ErrRec.Message, ErrRec.StructNo, ErrRec.CntxNo );
                    if( pErr )
                    {
                        pErr->Count += ErrRec.Count;  /* does not increment TraceCompareErrors.ErrCount */

#ifdef _MPI_PROF_TRAN_
                        if ( dvm_OneProcSign )
                        {
                        if ( (ErrRec.Primary*pErr->Primary) > 0 ) /* both are primary or secondary */
                        {
#endif
                        if ( ErrRec.ErrTime < pErr->ErrTime )
                        {   /* change context information to the earliest error */

                            SYSTEM(strncpy, (pErr->Context, ErrRec.Context, MAX_ERR_CONTEXT));
                            pErr->Context[MAX_ERR_CONTEXT] = 0;
                            pErr->TrcLine = ErrRec.TrcLine;
                            pErr->TrcCPU  = ErrRec.TrcCPU;

                            update_CPUlist(&(pErr->CPUList), 0, 0, pErr->RealCPU);

                            pErr->RealCPU = ErrRec.RealCPU;    /*  == j  */
                            pErr->ErrTime = ErrRec.ErrTime;
                        }
                        else
                        {
                            /* save cpu list */
                            update_CPUlist(&(pErr->CPUList), 0, 0, j);
                        }
#ifdef _MPI_PROF_TRAN_
                        }
                        else
                        {
                            if ( pErr->Primary < 0 )  /* this is a secondary error */
                            {   /* change context information to primary error */

                                SYSTEM(strncpy, (pErr->Context, ErrRec.Context, MAX_ERR_CONTEXT));
                                pErr->Context[MAX_ERR_CONTEXT] = 0;
                                pErr->TrcLine = ErrRec.TrcLine;
                                pErr->TrcCPU  = ErrRec.TrcCPU;

                                update_CPUlist(&(pErr->CPUList), 0, 0, pErr->RealCPU);

                                pErr->RealCPU = ErrRec.RealCPU;  /*  == j  */
                                pErr->ErrTime = ErrRec.ErrTime;
                                pErr->Primary = 1;
                            }
                            else
                            {
                                /* save cpu list */
                                update_CPUlist(&(pErr->CPUList), 0, 0, j);
                            }
                        }
                        } /* dvm_OneProcSign */
#endif
                    }
                    else
                    {
                        table_Put( &(TraceCompareErrors.tErrors), &ErrRec );
                        TraceCompareErrors.ErrCount ++;
                    }
                }

                /* read leaps from file */
                for ( i=0; i < Count2; i++)
                {
                    if (feof(binf) || ( fread(&ErrInfo, sizeof(ERR_INFO), 1, binf) < 1 ) )
                    {
                        pprintf(3, "*** RTS err: Binary file <%s> read error. Process terminated.\n", FileName);
                        exit (-1);
                    }

                    pErrInfo = errInfo_Find( &(Trace.tVarErrEntries), ErrInfo.File, ErrInfo.Line, ErrInfo.Operand, ErrInfo.vtype );

                    if( pErrInfo )
                    {
                        pErrInfo->HitCount += ErrInfo.HitCount; /* summary No of counts */

                        /* update hit count */
                        update_CPUlist(&(pErrInfo->CPULists), 0, 1, j);

                        if ( ErrInfo.HitCount > pErrInfo->MaxHitCount )
                        {    /* clean & update max hit count */
                             clean_update_CPUlist(&(pErrInfo->CPULists), 1, j);
                             pErrInfo->MaxHitCount = ErrInfo.HitCount;
                        }
                        else
                            if ( ErrInfo.HitCount == pErrInfo->MaxHitCount )
                            {   /* update max hit count */
                                update_CPUlist(&(pErrInfo->CPULists), 1, 1, j);
                            }
                            /* else do nothing */

                        /* first dif maximum discrepancy dispatcher */
                        if ( !TraceOptions.ExpIsAbsolute )
                        {
                            if ( ErrInfo.firstHitRel > pErrInfo->firstHitRel )
                            {
                                clean_update_CPUlist(&(pErrInfo->CPULists), 2, j);
                                pErrInfo->firstHitRel  = ErrInfo.firstHitRel;
                                pErrInfo->firstHitAbs  = ErrInfo.firstHitAbs;
                                pErrInfo->firstHitLoc  = ErrInfo.firstHitLoc;
                                pErrInfo->firstHitLocC = ErrInfo.firstHitLocC;
                                pErrInfo->firstHitValT = ErrInfo.firstHitValT;
                                pErrInfo->firstHitValE = ErrInfo.firstHitValE;
                            }
                            else
                                if ( ErrInfo.firstHitRel == pErrInfo->firstHitRel )
                                {
                                    update_CPUlist(&(pErrInfo->CPULists), 2, 1, j);
                                }
                                /* else do nothing */
                        }
                        else
                        {
                            if ( ErrInfo.firstHitAbs > pErrInfo->firstHitAbs )
                            {
                                clean_update_CPUlist(&(pErrInfo->CPULists), 2, j);
                                pErrInfo->firstHitRel  = ErrInfo.firstHitRel;
                                pErrInfo->firstHitAbs  = ErrInfo.firstHitAbs;
                                pErrInfo->firstHitLoc  = ErrInfo.firstHitLoc;
                                pErrInfo->firstHitLocC = ErrInfo.firstHitLocC;
                                pErrInfo->firstHitValT = ErrInfo.firstHitValT;
                                pErrInfo->firstHitValE = ErrInfo.firstHitValE;
                            }
                            else
                                if ( ErrInfo.firstHitAbs == pErrInfo->firstHitAbs )
                                {
                                    update_CPUlist(&(pErrInfo->CPULists), 2, 1, j);
                                }
                                /* else do nothing */
                        }

                        /* maximum relative discrepancy dispatcher */
                        if ( ErrInfo.maxRelAcc > pErrInfo->maxRelAcc )
                        {
                            clean_update_CPUlist(&(pErrInfo->CPULists), 3, j);
                            pErrInfo->maxRelAcc    = ErrInfo.maxRelAcc;
                            pErrInfo->maxRelAccAbs = ErrInfo.maxRelAccAbs;
                            pErrInfo->maxRelAccLoc = ErrInfo.maxRelAccLoc;
                            pErrInfo->maxRelAccLocC= ErrInfo.maxRelAccLocC;
                            pErrInfo->maxRelAccValT= ErrInfo.maxRelAccValT;
                            pErrInfo->maxRelAccValE= ErrInfo.maxRelAccValE;
                        }
                        else
                            if ( ErrInfo.maxRelAcc == pErrInfo->maxRelAcc )
                            {
                                update_CPUlist(&(pErrInfo->CPULists), 3, 1, j);
                            }

                        /* maximum relative leap dispatcher */
                        pErrInfo->LeapsRCount += ErrInfo.LeapsRCount;
                        if ( ErrInfo.maxLeapRel > pErrInfo->maxLeapRel )
                        {
                            clean_update_CPUlist(&(pErrInfo->CPULists), 4, j);
                            pErrInfo->maxLeapRel    = ErrInfo.maxLeapRel;
                            pErrInfo->maxLeapRelAbs = ErrInfo.maxLeapRelAbs;
                            pErrInfo->maxLeapRLoc   = ErrInfo.maxLeapRLoc;
                            pErrInfo->maxLeapRLocC  = ErrInfo.maxLeapRLocC;
                            pErrInfo->maxLeapRelValT= ErrInfo.maxLeapRelValT;
                            pErrInfo->maxLeapRelValE= ErrInfo.maxLeapRelValE;
                        }
                        else
                            if ( ErrInfo.maxLeapRel == pErrInfo->maxLeapRel )
                            {
                                update_CPUlist(&(pErrInfo->CPULists), 4, 1, j);
                            }

                        /* maximum absolute discrepancy dispatcher */
                        if ( ErrInfo.maxAbsAcc > pErrInfo->maxAbsAcc )
                        {
                            clean_update_CPUlist(&(pErrInfo->CPULists), 5, j);
                            pErrInfo->maxAbsAcc    = ErrInfo.maxAbsAcc;
                            pErrInfo->maxAbsAccRel = ErrInfo.maxAbsAccRel;
                            pErrInfo->maxAbsAccLoc = ErrInfo.maxAbsAccLoc;
                            pErrInfo->maxAbsAccLocC= ErrInfo.maxAbsAccLocC;
                            pErrInfo->maxAbsAccValT= ErrInfo.maxAbsAccValT;
                            pErrInfo->maxAbsAccValE= ErrInfo.maxAbsAccValE;
                        }
                        else
                            if ( ErrInfo.maxAbsAcc == pErrInfo->maxAbsAcc )
                            {
                                update_CPUlist(&(pErrInfo->CPULists), 5, 1, j);
                            }

                        /* maximum absolute leap dispatcher */
                        pErrInfo->LeapsACount += ErrInfo.LeapsACount;
                        if ( ErrInfo.maxLeapAbs > pErrInfo->maxLeapAbs )
                        {
                            clean_update_CPUlist(&(pErrInfo->CPULists), 6, j);
                            pErrInfo->maxLeapAbs    = ErrInfo.maxLeapAbs;
                            pErrInfo->maxLeapAbsRel = ErrInfo.maxLeapAbsRel;
                            pErrInfo->maxLeapALoc   = ErrInfo.maxLeapALoc;
                            pErrInfo->maxLeapALocC  = ErrInfo.maxLeapALocC;
                            pErrInfo->maxLeapAbsValT= ErrInfo.maxLeapAbsValT;
                            pErrInfo->maxLeapAbsValE= ErrInfo.maxLeapAbsValE;
                        }
                        else
                            if ( ErrInfo.maxLeapAbs == pErrInfo->maxLeapAbs )
                            {
                                update_CPUlist(&(pErrInfo->CPULists), 6, 1, j);
                            }
                    }
                    else
                    {
                        DBG_ASSERT(__FILE__, __LINE__, !ErrInfo.MaxHitCount );
                        ErrInfo.MaxHitCount = ErrInfo.HitCount;
                        update_CPUlist(&(ErrInfo.CPULists), 0, 1, j);  /* save cpu_num into hit count list */
                        update_CPUlist(&(ErrInfo.CPULists), 1, 1, j);  /* save cpu_num into max hit count list */
                        update_CPUlist(&(ErrInfo.CPULists), 2, 1, j);  /* save cpu_num into * list */
                        update_CPUlist(&(ErrInfo.CPULists), 3, 1, j);  /* save cpu_num into * list */
                        update_CPUlist(&(ErrInfo.CPULists), 4, 1, j);  /* save cpu_num into * list */
                        update_CPUlist(&(ErrInfo.CPULists), 5, 1, j);  /* save cpu_num into * list */
                        update_CPUlist(&(ErrInfo.CPULists), 6, 1, j);  /* save cpu_num into * list */
                        table_Put( &(Trace.tVarErrEntries), &ErrInfo );
                    }
                }

                /* close and remove unnecessary binary files */
                SYSTEM(fclose, (binf))
                SYSTEM(remove, (FileName));
            }

            /* now we should write final errors and leaps protocols to files */

            SYSTEM(sprintf, (FileName, "%s.finerr.trd", dvm_argv[0]));

            error_PrintAll(FileName, &TraceCompareErrors, 1);

            /* now write final leaps protocol */
            SYSTEM(sprintf, (FileName, "%s%s.finprot.trd", TraceOptions.TracePath, dvm_argv[0]));

            Count1 = table_Count(&(Trace.tVarErrEntries));

            DBG_ASSERT(__FILE__, __LINE__, Count1 >= EntriesCount );

            mac_realloc(ppErrInfo, ERR_INFO **, ppErrInfo, Count1*sizeof(ERR_INFO *), 0);

            /* fill in newely allocated entries */
            Count2 = Count1-EntriesCount;
            for ( i=0; i < Count2; i++)
            {
                 *(ppErrInfo+EntriesCount+i) = table_At(ERR_INFO, &Trace.tVarErrEntries, EntriesCount+i);
            }

            errInfo_PrintAll(FileName, ppErrInfo, Count1, 1 );

            /* table_Done for Trace.tVarErrEntries will be called in calling function */
        }
        else
        {      /* save files branch */
            Count1 = table_Count( &(TraceCompareErrors.tErrors) );

            SYSTEM(sprintf, (FileName, "%s%s.%d.bin.trd",
                    TraceOptions.TracePath, dvm_argv[0], Trace.CurCPUNum ));
            SYSTEM(remove, (FileName));

#ifdef WIN32
            SYSTEM_RET(binfd, _sopen, (FileName, _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYRD, _S_IWRITE  ));
#else
            SYSTEM_RET(binfd, open,   (FileName, O_CREAT | O_WRONLY, 0666 ));
            l.l_type = F_WRLCK;
            l.l_whence = SEEK_SET;
            l.l_start = 0;
            l.l_len = 0;
            if ( (binfd != -1) && (fcntl(binfd,F_SETLKW,&l)==-1) )
            {
                close(binfd);
                binfd = -1;
            }
#endif
            if( binfd == -1 )
            {
                pprintf(3, "*** RTS err: Can't open binary file <%s> for writing.\n", FileName);
                return ;
            }

            /* save summary information - number of errors and number of leaps */
            if ((write(binfd, &Count1, sizeof(DvmType)) < sizeof(DvmType)) ||
                (write(binfd, &EntriesCount, sizeof(DvmType)) < sizeof(DvmType))
                 )
            {
                pprintf(3, "*** RTS err: Binary file <%s> write error.\n", FileName);
                return ;
            }

            for ( i = 0; i < Count1; i++ )
            {
                pErr = table_At( ERROR_RECORD, &(TraceCompareErrors.tErrors), i );
                if ( write(binfd, pErr, sizeof(ERROR_RECORD)) < sizeof(ERROR_RECORD) )
                {
                    pprintf(3, "*** RTS err: Binary file <%s> write error.\n", FileName);
                    return ;
                }
            }

            for ( i = 0; i < EntriesCount; i++ )
            {
                if ( write(binfd, *(ppErrInfo+i), sizeof(ERR_INFO)) < sizeof(ERR_INFO) )
                {
                    pprintf(3, "*** RTS err: Binary file <%s> write error.\n", FileName);
                    return ;
                }
            }

            SYSTEM( close, ( binfd ));
            mac_free(&ppErrInfo);

            /* free leaps tables here ... skip it to make program finish faster */
            /* the work is finished */
        }
    }
}


#endif  /* _CNTRLERR_C_ */
