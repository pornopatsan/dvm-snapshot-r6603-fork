
#ifndef _TRC_READ_C_
#define _TRC_READ_C_
/******************/

/*********************************************************************/

UDvmType trc_rd_header(DVMFILE *hf, char* szFileName)
{
    char *pnt, *last, mode[256];
    STRUCT_INFO *Loop;
    dvm_ARRAY_INFO* pArr;
    short Key, Code;
    DvmType Rank, No, ParentNo, Lower, Upper, Step, Level, Line, Type;
    int Len;
    DvmType i;
    UDvmType StrCntr = 0, Count;

    Trace.ErrCode = SUCCESS;
    Count = trc_rd_gets(hf);
    if (Count == 0)
        return 0;

    do
    {
        StrCntr += Count;

        Key = trc_rd_search_key(trc_rdwrt_buff);
        switch (Key)
        {
            case N_FILE :
            {
                /* this is the case when the header is empty, but contains coverage info */
                return 0;
            }
            case N_TASK_NAME :
            case N_WORK_DIR :
            case N_USER_HOST :
            case N_ARCH :
            case N_OS :
            {
                break;                  // do not parse these records, no sense
            }
            case N_DEB_VERSION :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_DEB_VERSION]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=@", mode);
                if (pnt)
                {
                    if (strcmp(DEBUGGER_VERSION, mode))
                        pprintf(3, "*** RTS warning: READTRACE: Current debugger version differ from the one in the trace.\n");
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_TIME :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_TIME]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=@", mode);
                if (pnt)
                {
                    SYSTEM(strncpy, (Trace.TraceTime, mode, 25));
                    Trace.TraceTime[24] = 0;
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_MODE :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_MODE]));
                pnt = trc_rd_split( trc_rdwrt_buff + Len, "=&", &Code);
                if (pnt)
                {
                    switch (Code)
                    {
                        case N_NONE : TraceOptions.TraceLevel = level_NONE; break;
                        case N_MINIMAL : TraceOptions.TraceLevel = level_MINIMAL; break;
                        case N_MODIFY : TraceOptions.TraceLevel = level_MODIFY; break;
                        case N_FULL : TraceOptions.TraceLevel = level_FULL; break;
                        case N_CHECKSUM : TraceOptions.TraceLevel = level_CHECKSUM; break;
                        default :
                            TraceOptions.TraceLevel = level_NONE;
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                    }
                }
                else
                    error_CmpTrace( szFileName, StrCntr, ERR_RD_SYNTAX );
                break;
            }
            case N_CPUCOUNT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_CPUCOUNT]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    Trace.TraceCPUCount = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_EMPTYITER :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_EMPTYITER]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.WriteEmptyIter = (byte)(0 != Type);
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_MULTIDIM_ARRAY :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_MULTIDIM_ARRAY]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.MultidimensionalArrays = (byte)(0 != Type);
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
                                           }
            case N_DEF_ARR_STEP :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_DEF_ARR_STEP]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.DefaultArrayStep = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_DEF_ITER_STEP :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_DEF_ITER_STEP]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.DefaultIterStep = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_STARTPOINT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_STARTPOINT]));

                if ( *(trc_rdwrt_buff + Len + 1) == 0 )
                {
                    Trace.StartPoint = NULL;
                }
                else
                {
                    if ( !Trace.StartPoint )
                        mac_malloc(Trace.StartPoint, NUMBER *, sizeof(NUMBER), 0);

                    if ( !parse_number(trc_rdwrt_buff + Len + 1, Trace.StartPoint) )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                }
                break;
            }
            case N_FINISHPOINT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_FINISHPOINT]));

                if ( *(trc_rdwrt_buff + Len + 1) == 0 )
                {
                    Trace.FinishPoint = NULL;
                }
                else
                {
                    if ( !Trace.FinishPoint )
                        mac_malloc(Trace.FinishPoint, NUMBER *, sizeof(NUMBER), 0);

                    if ( !parse_number(trc_rdwrt_buff + Len + 1, Trace.FinishPoint) )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                }
                break;
            }
            case N_IG_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IG_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Ig_left = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IG_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IG_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Ig_right = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ILOC_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_ILOC_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Iloc_left = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ILOC_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_ILOC_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Iloc_right = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IREP_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IREP_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Irep_left = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IREP_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IREP_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.Irep_right = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_LOCITERWIDTH :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_LOCITERWIDTH]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.LocIterWidth = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_REPITERWIDTH :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_REPITERWIDTH]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.RepIterWidth = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_CALC_CHECKSUM :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_CALC_CHECKSUM]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                    TraceOptions.CalcChecksums = (int)Type;
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ARRAY :
            {
                int dummy;
                /* ARR: <name> (<Elem Type>) [<Rank>] {<File>, <Line>, <No>} = Level, (dim:min,max,step), ... */

                Type = 0; Code = -1;
                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ));
                pnt = trc_rdwrt_buff + Len;
                pnt = trc_rd_split( pnt, ":@(!)[!]{@,!,!,!}=&", trc_filebuff1, &Type, &Rank, trc_filebuff, &Line, &i, &dummy, &Code);
                if (NULL != pnt)
                {
                    pArr = trc_ArrayInfoNew(trc_filebuff, trc_filebuff1, Line, NULL, 0, Type, (byte)Rank, &No);
                    pArr->iNumber = (int)i;

                    switch (Code)
                    {
                        case N_NONE   : pArr->eTraceLevel = level_NONE; break;
                        case N_MINIMAL : pArr->eTraceLevel = level_MINIMAL; break;
                        case N_MODIFY : pArr->eTraceLevel = level_MODIFY; break;
                        case N_FULL   : pArr->eTraceLevel = level_FULL; break;
                        default : pArr->eTraceLevel = level_DEFAULT; break;
                    }
                    while (pnt && *pnt)
                    {
                        Lower = Upper = Step = MAXLONG;
                        last = pnt;
                        pnt = trc_rd_split( pnt, ",(!:%,%,%)", &Level, &Lower, &Upper, &Step);
                        if( pnt && pnt != last && (byte)Level < MAXARRAYDIM)
                        {
                            pArr->rgLimit[(size_t)Level].Lower = Lower;
                            pArr->rgLimit[(size_t)Level].Upper = Upper;
                            pArr->rgLimit[(size_t)Level].Step = Step;
                        }
                        else
                        {
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                            break;
                        }
                    }
                }
                else
                {
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                }
                break;
            }
            case N_SLOOP :
            case N_PLOOP :
            case N_TASKREGION :
            {
                /* L: <No>( <Parent> ) [<Rank>] (lower, upper, step)()...(); {<File>, <Line>} = Level, (dim:min,max,step), ... */

                No = 0; ParentNo = -1; Code = -1;
                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ));
                pnt = trc_rdwrt_buff + Len;
                pnt = trc_rd_split( pnt, ":!(%)[!]{@,!}=&", &No, &ParentNo, &Rank, trc_filebuff, &Line, &Code );
                if (NULL != pnt)
                {
                    if (ParentNo >= 0)
                    {
                        if (NULL == Trace.pCurCnfgInfo || Trace.pCurCnfgInfo->No != ParentNo)
                        {
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                            break;
                        }
                    }

                    Loop = trc_InfoNew(Trace.pCurCnfgInfo);
                    Loop->No = (short)No;
                    Loop->Rank = (byte)Rank;
                    switch (Key)
                    {
                        case N_SLOOP : Loop->Type = 0; break;
                        case N_PLOOP : Loop->Type = 1; break;
                        case N_TASKREGION : Loop->Type = 2; break;
                        default : Loop->Type = 0;
                    }
                    SYSTEM(strncpy, (Loop->File, trc_filebuff, MaxSourceFileName));
                    Loop->File[MaxSourceFileName] = 0;
                    Loop->Line = Line;

                    switch( Code )
                    {
                        case N_NONE   : Loop->TraceLevel = level_NONE; break;
                        case N_MINIMAL : Loop->TraceLevel = level_MINIMAL; break;
                        case N_MODIFY : Loop->TraceLevel = level_MODIFY; break;
                        case N_FULL   : Loop->TraceLevel = level_FULL; break;
                        default : Loop->TraceLevel = level_DEFAULT; break;
                    }
                    while( pnt && *pnt )
                    {
                        Lower = Upper = Step = MAXLONG;
                        last = pnt;
                        pnt = trc_rd_split( pnt, ",(!:%,%,%)", &Level, &Lower, &Upper, &Step );
                        if( pnt && pnt != last && (byte)Level < Loop->Rank )
                        {
                            Loop->Limit[ (size_t)Level ].Lower = Lower;
                            Loop->Limit[ (size_t)Level ].Upper = Upper;
                            Loop->Limit[ (size_t)Level ].Step = Step;
                        }
                        else
                        {
                            error_CmpTrace( szFileName, StrCntr, ERR_RD_STRUCT );
                            break;
                        }
                    }
                    Trace.pCurCnfgInfo = Loop;
                }
                else
                {
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                }
                break;
            }
            case N_END_LOOP :
            {
                if (NULL == Trace.pCurCnfgInfo)
                {
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                }
                else
                {
                    last = trc_rdwrt_buff + Len;
                    pnt = trc_rd_split( trc_rdwrt_buff + Len, ":!", &No);
                    if ( !pnt || pnt == last)
                    {
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                        Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
                        break;
                    }

                    last = pnt;
                    pnt = trc_rd_split( pnt, "(@,@,!,!,@)", trc_filebuff, trc_filebuff1, &Line, &No, mode );

                    if (NULL != pnt && pnt != last)
                    {
                        if(Trace.pCurCnfgInfo->Type == 0 || (TraceOptions.TraceLevel != level_CHECKSUM && TraceOptions.CalcChecksums == 0))
                        {
                            error_CmpTrace( szFileName, StrCntr, ERR_RD_STRUCT );
                            Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
                            break;
                        }
                        else
                        {
                            while (NULL != pnt && pnt != last)
                            {
                                /*- Find the appropriate array in the arrays table -*/
                                for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
                                {
                                    pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

                                    if (((DvmType)pArr->ulLine) == Line && ((DvmType)(pArr->iNumber)) == No && !strcmp(pArr->szOperand, trc_filebuff)
                                            && !strFileCmp(pArr->szFile, trc_filebuff1))
                                    {
                                        if (strlen(mode) == 2 && mode[0]=='r' && mode[1]=='w')
                                                updateListN( &(Trace.pCurCnfgInfo->ArrList), i, 3);
                                        else
                                        {
                                            if (strlen(mode) != 1) error_CmpTrace( szFileName, StrCntr, ERR_RD_SYNTAX);
                                            if (mode[0]=='w')
                                                updateListN( &(Trace.pCurCnfgInfo->ArrList), i, 2);
                                            else
                                            {
                                                if (mode[0]!='r') error_CmpTrace( szFileName, StrCntr, ERR_RD_SYNTAX);
                                                updateListN( &(Trace.pCurCnfgInfo->ArrList), i, 1);
                                            }
                                        }
                                        break;
                                    }
                                }

                                if (i == -1)
                                    error_CmpTrace(szFileName, StrCntr, ERR_RD_UNDEFINED);

                                last = pnt;
                                pnt = trc_rd_split( pnt, "(@,@,!,!,@)", trc_filebuff, trc_filebuff1, &Line, &No, mode );
                            }
                        }
                    }
                    if (NULL == pnt)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                    Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
                }
                break;
            }
            case N_END_HEADER :
            {
                TraceHeaderRead = 1;

                if (NULL != Trace.pCurCnfgInfo)
                {
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
                }
                return StrCntr;
            }
            default : error_CmpTrace( szFileName, StrCntr, ERR_RD_UNDEF_KEY);
        }
    }
    while (Trace.ErrCode == SUCCESS && (Count = trc_rd_gets(hf)) != 0);

    if (SUCCESS == Trace.ErrCode && NULL != Trace.pCurCnfgInfo)
    {
        error_CmpTrace(szFileName, StrCntr, ERR_RD_STRUCT);
    }

    /* The header was completely read.
       Now all header inconsistencies will be treated as an error
    */
    TraceHeaderRead = 1;

    return StrCntr;
}

void trc_rd_trace( DVMFILE *hf, char* szFileName, UDvmType StrBase )
{
    UDvmType Line;
    UDvmType Count;
    char *pnt, smode[10], *last;
    VALUE Value;
    short Key;
    int Len, i, end_trace_exist = 0;
    byte LineRead = 0;
    STRUCT_BEGIN *LoopBeg = NULL;
    STRUCT_END   *LoopEnd = NULL;
    STRUCT_INFO  *pInfo = NULL;
    ANY_RECORD   *pAny  = NULL;
    dvm_ARRAY_INFO *pArr = NULL;
    byte printed = 0;

    if ( ! EnableTrace )
        return ;

    Trace.StrCntr = StrBase;
    Trace.CPU_Num = dvm_OneProcSign?dvm_OneProcNum:0;

    Count = trc_rd_gets( hf );
    if( Count == 0 )
    {
        error_CmpTrace( szFileName, 0, ERR_RD_EMPTY );
        return;
    }

    Trace.TraceRecordBase = StrBase + Count - 1;

    do
    {
        if (!LineRead) Trace.StrCntr += Count;
        else LineRead = 0;

        Key = trc_rd_search_key( trc_rdwrt_buff );
        switch( Key )
        {
            case N_MODE :
            {
                short  Code = -1, WrtEmpty = -1;
                SYSTEM_RET( Len, strlen, (KeyWords[N_MODE]) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, "=&,&", &Code, &WrtEmpty );
                if( pnt )
                {
                    switch( Code )
                    {
                        case N_NONE : TraceOptions.TraceLevel = level_NONE; break;
                        case N_MINIMAL : TraceOptions.TraceLevel = level_MINIMAL; break;
                        case N_MODIFY : TraceOptions.TraceLevel = level_MODIFY; break;
                        case N_FULL : TraceOptions.TraceLevel = level_FULL; break;
                        case N_CHECKSUM : TraceOptions.TraceLevel = level_CHECKSUM; break;
                        default :
                            TraceOptions.TraceLevel = level_NONE;
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                    }
                    if( WrtEmpty == N_EMPTYITER )
                        TraceOptions.WriteEmptyIter = 1;
                }
                else
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                break;
            }
            case N_SLOOP:
            case N_PLOOP:
            case N_TASKREGION:
            {
                DvmType rank, no, parnt = -1;
                byte Type;
                ITERBLOCK block;

                /* L: <no>(<parent>) [<rank>] = <level>, (iter), ...; { <file>, <line> } */
                SYSTEM_RET( Len, strlen, ( KeyWords[Key] ) );

                if ( Key == N_PLOOP && (strrchr(trc_rdwrt_buff, '(') != strchr(trc_rdwrt_buff, '(')) )
                { /* we must read the loop iterations set and Local part information */
                    pnt = trc_rd_split( trc_rdwrt_buff + Len, ":!(%)[!]", &no, &parnt, &rank);
                    if( pnt )
                    {
                        if( parnt != ( Trace.pCurCnfgInfo == NULL ? -1 : Trace.pCurCnfgInfo->No ) )
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }

                        /* we save iteration limits in the local block in order not to inquire
                         * a lot of memory in case of absence of local part on this CPU */
                        block.Rank = rank;
                        block.vtr  = 255;
                        for (i=0; i<rank; i++)
                        {
                            last = pnt;
                            pnt = trc_rd_split(last, "(!,!,!)", &block.Set[i].Lower,
                                    &block.Set[i].Upper, &block.Set[i].Step );
                            if ( !pnt || last == pnt )
                            {
                                error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                                break;
                            }
                        }
                        if ( i < rank ) break;

                        pnt = trc_rd_split( pnt, ";{@,!}", trc_filebuff, &Line );
                        if( pnt )
                        {
                            trc_put_beginstruct( trc_filebuff, Line, (short)no, (byte)1,
                                                    (byte)rank, NULL, NULL, NULL);
                        }
                        else
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }

                        /* now we need to read LOCAL record and attach
                         * information to the loopbeg structure */

                        LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

                        DBG_ASSERT(__FILE__, __LINE__, LoopBeg->pChunkSet == NULL);

                        Count = trc_rd_gets( hf );
                        if (Count == 0)
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }
                        Trace.StrCntr += Count;

                        pnt = trc_rd_split(trc_rdwrt_buff, "LOCAL:");
                        if ( !pnt || (pnt == trc_rdwrt_buff) )
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }

                        /* how much CPU is expected  */
                        DBG_ASSERT(__FILE__, __LINE__, Trace.TraceCPUCount >= 1);

                        mac_malloc(LoopBeg->pChunkSet, CHUNKSET*, sizeof(CHUNKSET)*Trace.TraceCPUCount, 0);
                        /* clear memory for 'undreloading' checking */
                        memset(LoopBeg->pChunkSet, 0, sizeof(CHUNKSET)*Trace.TraceCPUCount);

                        LoopBeg->iCurCPU = 0;

                        last = trc_rd_split(pnt, "NONE");

                        if ( !last || (pnt == last) )
                        {   /* there is local part of the loop on this processor */

                            /* number of blocks for nodes mode + 2 */
                            /* cannot do more exact forecast */
                            LoopBeg->iCurCPUMaxSize = ((int)1 << (rank + 1)) + 1;
                            LoopBeg->pChunkSet[0].Size = 2;

                            mac_malloc(LoopBeg->pChunkSet[0].Chunks, ITERBLOCK *,
                                        sizeof(ITERBLOCK)*LoopBeg->iCurCPUMaxSize, 0);

                            LoopBeg->pChunkSet[0].Chunks[0] = block;

                            LoopBeg->pChunkSet[0].Chunks[1].Rank = rank;
                            LoopBeg->pChunkSet[0].Chunks[1].vtr  = 255;
                            for (i=0; i<rank; i++)
                            {
                                last = pnt;
                                pnt = trc_rd_split(last, "(!,!,!)",
                                        &LoopBeg->pChunkSet[0].Chunks[1].Set[i].Lower,
                                        &LoopBeg->pChunkSet[0].Chunks[1].Set[i].Upper,
                                        &LoopBeg->pChunkSet[0].Chunks[1].Set[i].Step );
                                if ( !pnt || pnt == last )
                                {
                                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                                    LoopBeg->pChunkSet[0].Size--;
                                    break;
                                }
                            }
                            if ( i < rank ) break;
                        }
                        else
                        {   /* there is no local part of the loop on this processor */

                            LoopBeg->iCurCPUMaxSize = LoopBeg->pChunkSet[0].Size = 1;

                            mac_malloc(LoopBeg->pChunkSet[0].Chunks, ITERBLOCK *, sizeof(ITERBLOCK), 0);

                            /* store global iteration set */
                            LoopBeg->pChunkSet[0].Chunks[0] = block;
                        }
                    }
                    else
                    {
                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                        break;
                    }
                }
                else
                {
                    pnt = trc_rd_split( trc_rdwrt_buff + Len, ":!(%)[!];{@,!}", &no, &parnt, &rank, trc_filebuff, &Line );
                    if( pnt )
                    {
                        if( parnt != ( Trace.pCurCnfgInfo == NULL ? -1 : Trace.pCurCnfgInfo->No ) )
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }

                        switch (Key)
                        {
                            case N_SLOOP : Type = (byte)0; break;
                            case N_PLOOP : Type = (byte)1; break;
                            case N_TASKREGION : Type = (byte)2; break;
                            default : Type = 0;
                        }
                        trc_put_beginstruct( trc_filebuff, Line, (short)no, Type, (byte)rank, NULL, NULL, NULL);
                    }
                    else
                    {
                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                    }
                }
                break;
            }
            case N_CHUNK :
            {
                DvmType tmp;

                if ( Trace.pCurCnfgInfo != NULL && Trace.CurStruct != -1 )
                {
                    SYSTEM_RET( Len, strlen, (KeyWords[N_CHUNK]) );
                    pnt = trc_rdwrt_buff + Len + 1; /* 1 is for ':' character */

                    /* investigate if there is enough memory in current construct */
                    LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

                    DBG_ASSERT(__FILE__, __LINE__, LoopBeg->pChunkSet != NULL
                                                   && LoopBeg->pChunkSet[0].Size > 1
                                                   && LoopBeg->pChunkSet[0].Chunks != NULL);

                    if ( LoopBeg->pChunkSet[0].Size == LoopBeg->iCurCPUMaxSize )
                    {   /* need to reallocate memory */
                        LoopBeg->iCurCPUMaxSize += (((int)1 << (Trace.pCurCnfgInfo->Rank + 1)) - 1);
                        LoopBeg->pChunkSet[0].Chunks = (ITERBLOCK *) realloc(LoopBeg->pChunkSet[0].Chunks, sizeof(ITERBLOCK)*LoopBeg->iCurCPUMaxSize);

                        if ( LoopBeg->pChunkSet[0].Chunks == NULL )
                            epprintf(MultiProcErrReg1, __FILE__,__LINE__, "*** RTS err: No memory.\n");
                    }

                    for (i=0; i<Trace.pCurCnfgInfo->Rank; i++)
                    {
                        last = pnt;
                        pnt = trc_rd_split(last, "(!,!,!)",
                                &LoopBeg->pChunkSet[0].Chunks[LoopBeg->pChunkSet[0].Size].Set[i].Lower,
                                &LoopBeg->pChunkSet[0].Chunks[LoopBeg->pChunkSet[0].Size].Set[i].Upper,
                                &LoopBeg->pChunkSet[0].Chunks[LoopBeg->pChunkSet[0].Size].Set[i].Step );

                        LoopBeg->pChunkSet[0].Chunks[LoopBeg->pChunkSet[0].Size].Rank =
                                Trace.pCurCnfgInfo->Rank;

                        if ( !pnt || pnt == last )
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }
                    }
                    if ( i < Trace.pCurCnfgInfo->Rank ) break;

                    last = pnt;
                    pnt = trc_rd_split(last, "VTR=!", &tmp);
                    if ( !pnt || pnt == last )
                    {
                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                        break;
                    }

                    LoopBeg->pChunkSet[0].Chunks[LoopBeg->pChunkSet[0].Size].vtr = (byte) tmp;
                    LoopBeg->pChunkSet[0].Size++;
                }
                else
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }

                break;
            }
            case N_ITERATION:
            {
                DvmType LI, Index[MAXARRAYDIM];
                byte Rank = 1;

                /* IT: <abs_no>, (iter) */

                SYSTEM_RET( Len, strlen, (KeyWords[N_ITERATION]) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":!,(!", &LI, &Index[0] );
                if( pnt != NULL )
                {
                    while( pnt && *pnt != ')' && Rank < MAXARRAYDIM )
                    {
                        pnt = trc_rd_split( pnt, ",!", &Index[ Rank++ ] );
                    }
                    if( pnt != NULL )
                    {
                        trc_put_iteration_flash_par( Rank, Index, LI );
                    }
                    else
                    {
                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                    }
                }
                else
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }
                break;
            }
            case N_END_LOOP:
            {
                DvmType No, k = 5, i;     /* initial value of k is the default
                                        * size of array, which is allocated for
                                        * read checksums (magic) */
                const short k1 = 5;     /* quantity of elements to be added */
                double checksum;

                /* EL: <no>; { <file>, <line> }
                 *     CS(<name>, <file>, <line>, <number>, <access_type>)=<checksum>
                 *     . . .
                 *     CS(<name>, <file>, <line>, <number>, <access_type>)=<checksum>
                 * or
                 *     CS(<name>, <file>, <line>, <number>, <access_type>) FAILED
                 */

                SYSTEM_RET( Len, strlen, (KeyWords[N_END_LOOP]) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":!;{@,!}", &No, trc_filebuff, &Line );

                if( pnt != NULL )
                {
                    LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);
                    pInfo = Trace.pCurCnfgInfo;

                    trc_put_endstruct( trc_filebuff, Line, No, Trace.pCurCnfgInfo ? Trace.pCurCnfgInfo->Line : -1 );

                    if (TraceOptions.TraceLevel == level_CHECKSUM || TraceOptions.CalcChecksums == 1)
                    {                   /* but CSs might absent */

                        LoopEnd = table_At(STRUCT_END, &Trace.tTrace, LoopBeg->LastRec);
                                        /* we have retrieved the place for checksums */

                        Count = trc_rd_gets( hf );
                        Trace.StrCntr += Count;
                        if (Count == 0) break;

                        if (trc_rdwrt_buff[0]=='C' && trc_rdwrt_buff[1]=='S')
                        {
                            last = trc_rdwrt_buff+2;
                            pnt = trc_rd_split( last, "(@,@,!,!,@)", trc_filebuff, trc_filebuff1, &Line, &No, smode);

                            if (NULL != pnt && pnt != last && pInfo->Type)
                            {
                                mac_calloc(LoopEnd->checksums, CHECKSUM *, k, sizeof(CHECKSUM), 0);

                                while (NULL != pnt && pnt != last)
                                {
                                    /*- Find the appropriate array in the arrays table -*/

                                    for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
                                    {
                                        pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

                                        if (((UDvmType)pArr->ulLine) == Line && ((DvmType)(pArr->iNumber)) == No && !strcmp(pArr->szOperand, trc_filebuff)
                                                && !strFileCmp(pArr->szFile, trc_filebuff1))
                                        {
                                            if (LoopEnd->csSize == k)
                                            {
                                                /*- we need to reallocate the memory -*/
                                                k += k1;
                                                mac_realloc(LoopEnd->checksums, CHECKSUM *, LoopEnd->checksums, k*sizeof(CHECKSUM), 0);
                                                memset(LoopEnd->checksums+k-k1, 0, (k-k1)*sizeof(CHECKSUM));
                                            }

                                            LoopEnd->checksums[LoopEnd->csSize].pInfo = pArr;
                                            LoopEnd->checksums[LoopEnd->csSize].lArrNo = i;

                                            last = pnt;
                                            pnt = trc_rd_split(last, "=\"#d\"", &checksum);

                                            if (NULL == pnt) error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_SYNTAX);
                                            else if (last == pnt)
                                                    {
                                                        LoopEnd->checksums[LoopEnd->csSize].errCode = 0;
                                                        if (!printed)
                                                        {
                                                            error_Message( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, szFileName, Trace.StrCntr, "*** CMPTRACE *** : ", ErrString[ERR_RD_FAILED_CS], Trace.CPU_Num, (UDvmType) Trace.StrCntr  );
                                                            printed = 1;
                                                        }
                                                    }
                                                 else
                                                 {
                                                      LoopEnd->checksums[LoopEnd->csSize].errCode = 1;
                                                      LoopEnd->checksums[LoopEnd->csSize].sum = checksum;
                                                 }

                                            if (strlen(smode) == 2)
                                                LoopEnd->checksums[LoopEnd->csSize].accType = 3;
                                            else
                                            {
                                                if (strlen(smode) != 1) error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX);

                                                if (smode[0]=='w')
                                                    LoopEnd->checksums[LoopEnd->csSize].accType = 2;
                                                else
                                                {
                                                    if (smode[0]!='r') error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX);

                                                    LoopEnd->checksums[LoopEnd->csSize].accType = 1;
                                                }
                                            }
                                            LoopEnd->csSize++;
                                            break;
                                        }
                                    }

                                    if (i == -1)
                                        error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_UNDEFINED);

                                    Count = trc_rd_gets( hf );
                                    Trace.StrCntr += Count;
                                    if (Count == 0) break;

                                    if (trc_rdwrt_buff[0]!='C' || trc_rdwrt_buff[1]!='S') break;
                                    last = trc_rdwrt_buff+2;
                                    pnt = trc_rd_split( last, "(@,@,!,!,@)", trc_filebuff, trc_filebuff1, &Line, &No, smode);
                                }
                            }
                            if (Count != 0)
                            {
                                if (NULL == pnt || !pInfo->Type)
                                    error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_SYNTAX);
                                else            /* last == pnt, mostly */
                                    LineRead = 1;  /* equals to unread */
                            }
                        }
                        else LineRead = 1;
                    }
                }
                else
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }
                break;
            }
            case N_SKIP :
            {
                /* SKP: { <file>, <line> } */

                SYSTEM_RET( Len, strlen, (KeyWords[N_SKIP]) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":{@,!}", trc_filebuff, &Line );
                if( pnt != NULL )
                {
                    trc_put_skip( trc_filebuff, Line );
                }
                else
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }
                break;
            }
            case N_READ:
            case N_R_READ:
            case N_POST_WRITE:
            case N_R_POST_WRITE:
            {
                DvmType Type;
                int Success = 0;

                /* K: [<type>] <oper> = <value>; { <file>, <line> } */

                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":[!]@=", &Type, trc_filebuff1 );

                if( pnt )
                {
                    switch( Type )
                    {
                    case rt_INT :
                    case rt_LOGICAL :
                        pnt = trc_rd_split(pnt, "#i;", &Value);
                        break;
                    case rt_LONG :
                        pnt = trc_rd_split(pnt, "#l;", &Value);
                        break;
                    case rt_LLONG:
                        pnt = trc_rd_split(pnt, "#L;", &Value);
                        break;
                    case rt_FLOAT :
                        pnt = trc_rd_split(pnt, "#f;", &Value);
                        break;
                    case rt_DOUBLE :
                        pnt = trc_rd_split(pnt, "#d;", &Value);
                        break;
                    case rt_FLOAT_COMPLEX :
                        pnt = trc_rd_split(pnt, "(#f,#f);", &(Value._complex_float[0]),
                            &(Value._complex_float[1]));
                        break;
                    case rt_DOUBLE_COMPLEX :
                        pnt = trc_rd_split(pnt, "(#d,#d);", &(Value._complex_double[0]),
                            &(Value._complex_double[1]));
                        break;
                    }
                    if (pnt)
                    {
                        pnt = trc_rd_split( pnt, "{@,!}", trc_filebuff, &Line );
                        if( pnt )
                        {
                            switch( Key )
                            {
                                case N_READ :
                                case N_R_READ :
                                    pCmpOperations->Variable(trc_filebuff, Line, trc_filebuff1,
                                        trc_READVAR, (short)Type, &Value,
                                        (byte)(Key == N_R_READ), NULL);
                                    break;
                                case N_POST_WRITE :
                                case N_R_POST_WRITE :
                                    pCmpOperations->Variable(trc_filebuff, Line, trc_filebuff1,
                                        trc_POSTWRITEVAR, (short)Type, &Value,
                                        (byte)(Key == N_R_POST_WRITE), NULL);
                                    break;
                            }
                            Success = 1;
                        }
                    }
                }
                if( Success == 0 )
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX);
                }

                break;
            }
            case N_REDUCT:
            {
                DvmType Type;
                int Success = 0;

                /* K: [<type>] <value>; { <file>, <line> } */

                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":[!]", &Type );

                if( pnt )
                {
                    switch( Type )
                    {
                    case rt_INT :
                    case rt_LOGICAL :
                        pnt = trc_rd_split(pnt, "#i;", &Value);
                        break;
                    case rt_LONG :
                        pnt = trc_rd_split(pnt, "#l;", &Value);
                        break;
                    case rt_LLONG:
                        pnt = trc_rd_split(pnt, "#L;", &Value);
                        break;
                    case rt_FLOAT :
                        pnt = trc_rd_split(pnt, "#f;", &Value);
                        break;
                    case rt_DOUBLE :
                        pnt = trc_rd_split(pnt, "#d;", &Value);
                        break;
                    case rt_FLOAT_COMPLEX :
                        pnt = trc_rd_split(pnt, "(#f,#f);", &(Value._complex_float[0]),
                            &(Value._complex_float[1]));
                        break;
                    case rt_DOUBLE_COMPLEX :
                        pnt = trc_rd_split(pnt, "(#d,#d);", &(Value._complex_double[0]),
                            &(Value._complex_double[1]));
                        break;
                    }
                    if(pnt)
                    {
                        pnt = trc_rd_split( pnt, "{@,!}", trc_filebuff, &Line );
                        if( pnt )
                        {
                            pCmpOperations->Variable(trc_filebuff, Line, "",
                                trc_REDUCTVAR, (short)Type, &Value, 1, NULL);
                            Success = 1;
                        }
                    }
                }
                if( Success == 0 )
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }

                break;
            }
            case N_PRE_WRITE:
            case N_R_PRE_WRITE:
            {
                DvmType     Type;

                /* K: [<type>] <operand>; { <file>, <line> } */

                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":[!]@;{@,!}", &Type, trc_filebuff, trc_filebuff, &Line );
                if (pnt)
                {
                    pCmpOperations->Variable(trc_filebuff, Line, "",
                        trc_PREWRITEVAR, (short)Type, NULL,
                        (byte)(Key == N_R_PRE_WRITE), NULL);
                }
                else
                {
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );
                }

                break;
            }
            case N_END_TRACE:
                end_trace_exist = 1;
                break;
            default : error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_UNDEF_KEY );
        }
    }
    while( Trace.ErrCode == SUCCESS && !end_trace_exist && ( LineRead || (( Count = trc_rd_gets( hf ) ) != 0 )));

    if ( EnableTrace && Trace.ErrCode == SUCCESS && !end_trace_exist )
    {
        error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_RD_TRACE_INCOMPLETE, "0.trd" );
    }
}

UDvmType trc_rd_gets( DVMFILE *hf )
{
    char *pnt, *tmp;
    UDvmType Read = 0;

    *trc_rdwrt_buff = 0;
    if( Trace.ErrCode != SUCCESS ) return 0;
    do
    {
        tmp = (RTL_CALL, dvm_fgets( trc_rdwrt_buff, sizeof(trc_rdwrt_buff), hf ));
        if( !tmp ) return 0;

        trc_rdwrt_buff[sizeof(trc_rdwrt_buff)-1] = 0;
        SYSTEM_RET( pnt, strchr, ( trc_rdwrt_buff, '\n' ) ); /* search for code 13 in order to work correctly
                                                                with Windows traces on Linux systems */
        if( pnt != 0 ) *pnt = 0;

        /* Symbol # is used as comment symbol, everything after it is ignored */

        SYSTEM_RET( pnt, strchr, ( trc_rdwrt_buff, '#' ) );
        if( pnt != 0 )
        {
            if ( *(pnt+1)== 'N' && *(pnt+2)== 'A' && *(pnt+3)== 'N' )
            { /* Not A Number was read from the trace */
                epprintf(MultiProcErrReg2,__FILE__,__LINE__,"*** RTS error: READTRACE: NAN value found in the trace.\n");
                //*pnt=*(pnt+1)=*(pnt+2)=*(pnt+3)=*(pnt+4)='0';
            }
                // make error here
            *pnt = 0;
        }

        if( *trc_rdwrt_buff )
        {
            /* delete unnecessary separators */

            pnt = tmp = trc_rdwrt_buff;
            while( *tmp )
            {
                while( ( *tmp == ' ' ) || ( *tmp == '\t' ) ) tmp++;
                *(pnt++) = *(tmp++);
            }
            *pnt = 0;
        }
        Read++;
    }
    while( *trc_rdwrt_buff == 0 );

    return Read;
}

short trc_rd_search_key(char *str)
{
    short No = -1;
    short i;
    char *pnt;

    for( i = 0; i < N_EMPTY; i++ )
    {
        SYSTEM_RET( pnt, strstr, ( str, KeyWords[i] ) );
        if( ( pnt != 0 ) && ( pnt == str ) )
        {
            int k = strlen(KeyWords[i]);
            int s = strlen(str);

            if ( s != k )
                // s > k, sure
                if ( (str[k] != ':') && (str[k] != '=') )
                    continue ;

            No = i;
            break;
        }
    }

    return No;
}

#define isfloatdigit(c) (isdigit(c)||(c)=='-'||(c)=='+'||(c)=='.'||(c)=='E'||(c)=='e')

/************************************************
 * Special characters used in format string     *
 * & - key word (short)                         *
 * % - not obligatory integer (long)            *
 * ! - obligatory integer (long)                *
 * # - obligatory real number (double)          *
 *     i - int                                  *
 *     l - long                                 *
 *     L - long long                            *
 *     f - float                                *
 *     d - double                               *
 * @ - quoted string                            *
 ************************************************/
char *trc_rd_split( char *Str, char *Format, ... )
{
    char *pStr, *pFmt;
    void *arg;
    va_list ap;
    short Count = 0, Err = 0;

    va_start( ap, Format );
    pStr = Str; pFmt = Format;

    while( *pFmt && !Err )
    {
        switch( *pFmt )
        {
            case '&' :
            {
                short No;
                int Len;
                No = trc_rd_search_key( pStr );
                pFmt++;
                arg = va_arg( ap, void * );
                if( No != -1 )
                {
                    *(short *)arg = No;
                    SYSTEM_RET( Len, strlen, ( KeyWords[ No ] ) );
                    pStr = pStr + Len;
                    Count++;
                }
                else
                {
                    if( *pFmt && *pFmt != *pStr )
                        Err = 1;
                }
                break;
            }
            case '%' :
            {
                pFmt++;
                arg = va_arg( ap, void * );
                if( isdigit( *pStr ) || *pStr == '-' )
                {
                    *(DvmType *)arg = atol( pStr );
                    while( isdigit( *pStr ) || *pStr == '-' )
                        pStr++;
                    Count++;
                }
                else
                {
                    if( *pFmt && *pFmt != *pStr )
                        Err = 1;
                }
                break;
            }
            case '!' :
            {
                arg = va_arg( ap, void * );
                if( isdigit( *pStr ) || *pStr == '-' )
                {
                    *(DvmType *)arg = atol( pStr );
                    pFmt++;
                    while( isdigit( *pStr ) || *pStr == '-' )
                        pStr++;
                    Count++;
                }
                else
                    Err = 1;
                break;
            }
            case '#' :
            {
                arg = va_arg( ap, void * );
                if( isfloatdigit( *pStr ) )
                {
                    pFmt++;
                    switch( *pFmt )
                    {
                        case 'i' : *((int *)arg )= atoi( pStr ); break;
                        case 'l' : *((long *)arg )= atol( pStr ); break;
                        case 'L': sscanf(pStr, "%lld", (long long *)arg); break;
                        case 'f' : *((float *)arg )= (float)atof( pStr ); break;
                        case 'd' : *((double *)arg )= atof( pStr ); break;
                        default: Err = 1;
                    }
                    pFmt++;
                    while( isfloatdigit( *pStr ) )
                        pStr++;
                    Count++;
                }
                else
                    Err = 1;
                break;
            }
            case '@' :
            {
                char *p;
                p = va_arg( ap, char * );
                pFmt++;
                if ('\"' == *pStr)
                {
                    for (pStr++; ('\"' != *pStr) && (0 != *pStr); pStr++, p++)
                        *p = *pStr;
                    if ('\"' == *pStr)
                        pStr++;
                    else
                        Err = 1;
                }
                else
                {
                    Err = 1;
                }
                *p = 0;
                break;
            }
            default :
            {
                if( *pFmt == *pStr )
                {
                    pFmt++; pStr++;
                    Count++;
                }
                else
                    Err = 1;
            }
        }
    }

    va_end(ap);

    if( Err && Count > 0 ) return NULL;
    return pStr;
}

#undef isfloatdigit


#endif /* _TRC_READ_C_ */
