
#ifndef _TRC_MERGE_C_
#define _TRC_MERGE_C_
/******************/

/*********************************************************************/

UDvmType trc_mrg_header(DVMFILE *hf, char* szFileName)
{
    char *pnt, *last, mode[256];
    STRUCT_INFO *Loop;
    dvm_ARRAY_INFO* pArr;
    short Key, Code;
    DvmType Rank, No, ParentNo, Line, Type;
    int Len;
    DvmType i, j;
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
                    SYSTEM_RET(i, strcmp, (Trace.TraceTime, mode))
                    if( i != 0 )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
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
                        case N_NONE : i = (TraceOptions.TraceLevel != level_NONE)? 1 : 0; break;
                        case N_MINIMAL : i = (TraceOptions.TraceLevel != level_MINIMAL)? 1 : 0; break;
                        case N_MODIFY : i = (TraceOptions.TraceLevel != level_MODIFY)? 1 : 0; break;
                        case N_FULL : i = (TraceOptions.TraceLevel != level_FULL)? 1 : 0; break;
                        case N_CHECKSUM : i = (TraceOptions.TraceLevel != level_CHECKSUM)? 1 : 0; break;
                        default :
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                    }
                    if ( i != 0 )
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
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
                {
                    if (Trace.TraceCPUCount != Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_EMPTYITER :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_EMPTYITER]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.WriteEmptyIter != (byte)(0 != Type))
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_MULTIDIM_ARRAY :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_MULTIDIM_ARRAY]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.MultidimensionalArrays != (byte)(0 != Type))
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_DEF_ARR_STEP :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_DEF_ARR_STEP]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.DefaultArrayStep != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_DEF_ITER_STEP :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_DEF_ITER_STEP]));
                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.DefaultIterStep != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_STARTPOINT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_STARTPOINT]));

                if ( *(trc_rdwrt_buff + Len + 1) == 0 )
                {
                    if (Trace.StartPoint != NULL )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                {
                    NUMBER num;

                    if ( Trace.StartPoint == NULL )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                    else
                    {
                        if ( !parse_number(trc_rdwrt_buff + Len + 1, &num) )
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                        else
                            if ( !num_cmp(&num, Trace.StartPoint) )
                                error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                    }
                }
                break;
            }
            case N_FINISHPOINT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_FINISHPOINT]));

                if ( *(trc_rdwrt_buff + Len + 1) == 0 )
                {
                    if (Trace.FinishPoint != NULL)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                {
                    NUMBER num;

                    if ( Trace.FinishPoint  == NULL )
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                    else
                    {
                        if ( !parse_number(trc_rdwrt_buff + Len + 1, &num) )
                            error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                        else
                            if ( !num_cmp(&num, Trace.FinishPoint) )
                                error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                    }
                }
                break;
            }
            case N_IG_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IG_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Ig_left != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IG_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IG_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Ig_right != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ILOC_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_ILOC_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Iloc_left != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ILOC_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_ILOC_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Iloc_right != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IREP_LEFT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IREP_LEFT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Irep_left != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_IREP_RIGHT :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_IREP_RIGHT]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.Irep_right != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_LOCITERWIDTH :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_LOCITERWIDTH]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.LocIterWidth != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_REPITERWIDTH :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_REPITERWIDTH]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.RepIterWidth != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_CALC_CHECKSUM :
            {
                SYSTEM_RET(Len, strlen, (KeyWords[N_CALC_CHECKSUM]));

                pnt = trc_rd_split(trc_rdwrt_buff + Len, "=!", &Type);
                if (pnt)
                {
                    if (TraceOptions.CalcChecksums != (int)Type)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_TRACEF_MISMATCH);
                }
                else
                    error_CmpTrace(szFileName, StrCntr, ERR_RD_SYNTAX);
                break;
            }
            case N_ARRAY :
            {
                /* ARR: <name> (<Elem Type>) [<Rank>] {<File>, <Line>, <No>} = Level, (dim:min,max,step), ... */
                int dummy;

                Type = 0; Code = -1;
                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ));
                pnt = trc_rdwrt_buff + Len;
                pnt = trc_rd_split( pnt, ":@(!)[!]{@,!,!,!}=&", trc_filebuff1, &Type, &Rank, trc_filebuff, &Line, &i, &dummy, &Code);
                if (NULL != pnt)
                {
                    /*- Find the appropriate array in the arrays table -*/
                    for (j = table_Count(&Trace.tArrays)-1; j>=0; j--)
                    {
                        pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, j);

                        if (((DvmType)pArr->ulLine) == Line && ((DvmType)(pArr->iNumber)) == i &&
                                pArr->bRank == ((byte)Rank) && /*pArr->lElemType == Type &&*/
                                !strcmp(pArr->szOperand, trc_filebuff1) && !strFileCmp(pArr->szFile, trc_filebuff) )
                            /* match found */
                            break;
                    }

                    if (j == -1)
                        error_CmpTrace(szFileName, StrCntr, ERR_RD_UNDEFINED);
                    /* other options are not read because they are treated obsolete */
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
                /* L: <No>( <Parent> ) [<Rank>] {<File>, <Line>} = Level, (dim:min,max,step), ... */

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
                    switch (Key)
                    {
                        case N_SLOOP : Type = 0; break;
                        case N_PLOOP : Type = 1; break;
                        case N_TASKREGION : Type = 2; break;
                        default : Type = 0;
                    }
                    Loop = trc_InfoFindForCurrentLevel(No, Type, Rank, trc_filebuff, Line);
                    /* if there is no such struct in the memory a new struct will be created */

                    switch( Code )
                    {   /* let's leave it because we don't know here if the struct is new or not */
                        case N_NONE   : Loop->TraceLevel = level_NONE; break;
                        case N_MINIMAL : Loop->TraceLevel = level_MINIMAL; break;
                        case N_MODIFY : Loop->TraceLevel = level_MODIFY; break;
                        case N_FULL   : Loop->TraceLevel = level_FULL; break;
                        default : Loop->TraceLevel = level_DEFAULT; break;
                    }
                    /* other options are not read because they are treated obsolete */
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
                        if(Trace.pCurCnfgInfo->Type == 0 || (TraceOptions.TraceLevel != level_CHECKSUM && TraceOptions.CalcChecksums == 0) )
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

    TraceHeaderRead = 1;

    return StrCntr;
}

void trc_mrg_trace( DVMFILE *hf, char* szFileName, UDvmType StrBase )
{
    UDvmType Line;
    UDvmType Count;
    char *pnt, smode[10], *last;
    VALUE Value;
    short Key;
    int Len, i, j, end_trace_exist = 0;;
    byte LineRead = 0;
    STRUCT_BEGIN *LoopBeg = NULL;
    STRUCT_END   *LoopEnd = NULL;
    STRUCT_INFO  *pInfo = NULL;
    ANY_RECORD   *pAny  = NULL;
    dvm_ARRAY_INFO *pArr = NULL;
    byte printed = 0, found;
    DvmType tmpTraceRecord;
    STRUCT_INFO  *tmpStructInfo;

    if ( ! EnableTrace )
        return ;

    Trace.StrCntr = StrBase;

    Count = trc_rd_gets( hf );
    if( Count == 0 )
    {
        error_CmpTrace( szFileName, 0, ERR_RD_EMPTY );
        return;
    }

    Trace.TraceRecordBase = StrBase + Count - 1;
    Trace.Mode = 0;

    do
    {
        if (!LineRead) Trace.StrCntr += Count;
        else LineRead = 0;

        Key = trc_rd_search_key( trc_rdwrt_buff );
        switch( Key )
        {
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
                            if ( Trace.Mode == 0 )
                                trc_cmp_beginstruct( trc_filebuff, Line, (short)no, (byte)1,
                                                        (byte)rank, NULL, NULL, NULL);
                            else
                            {
                                trc_put_beginstruct( trc_filebuff, Line, (short)no, (byte)1,
                                                        (byte)rank, NULL, NULL, NULL);
                            }
                        }
                        else
                        {
                            error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                            break;
                        }

                        /* now we need to read LOCAL record and attach/compare
                         * information to the loopbeg structure */

                        LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

                        DBG_ASSERT(__FILE__, __LINE__,
                                (LoopBeg->pChunkSet == NULL)&&(Trace.Mode == 1) ||
                                (LoopBeg->pChunkSet != NULL)&&(Trace.Mode == 0) );

                        if ( Trace.Mode == 1 ) /* put (add) mode */
                        {
                            /* how much CPU is expected  */
                            DBG_ASSERT(__FILE__, __LINE__, Trace.TraceCPUCount >= 1);

                            /* LoopBeg->pChunkSet == NULL already assured */
                            mac_malloc(LoopBeg->pChunkSet, CHUNKSET*, sizeof(CHUNKSET)*Trace.TraceCPUCount, 0);
                            /* clear memory for 'underloading' checking */
                            memset(LoopBeg->pChunkSet, 0, sizeof(CHUNKSET)*Trace.TraceCPUCount);

                            LoopBeg->iCurCPU = 0;
                        }

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

                        last = trc_rd_split(pnt, "NONE");

                        if ( !last || (pnt == last) )
                        {   /* there is local part of the loop on this processor */

                            if ( Trace.Mode == 0 )
                            {   /* compare mode */

                                if ( !LoopBeg->pChunkSet || !LoopBeg->pChunkSet[0].Chunks ||
                                     iters_Compare(&block, &LoopBeg->pChunkSet[0].Chunks[0]) )
                                {
                                    /* blocks or iteration sets are different */
                                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                                "*** READTRACE ***: Initial set of iterations is different for loop %d. "
                                                "Program aborted.\n", no );
                                }

                                LoopBeg->iCurCPU++;
                                DBG_ASSERT(__FILE__, __LINE__, LoopBeg->iCurCPU < Trace.TraceCPUCount);

                                /* number of blocks for nodes mode + 2 */
                                /* cannot do more exact forecast */
                                LoopBeg->iCurCPUMaxSize = ((int)1 << (rank + 1)) + 1;
                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size = 2;

                                mac_malloc(LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks, ITERBLOCK *,
                                            sizeof(ITERBLOCK)*LoopBeg->iCurCPUMaxSize, 0);

                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[0] = block;

                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[1].Rank = rank;
                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[1].vtr = 255;
                                for (i=0; i<rank; i++)
                                {
                                    last = pnt;
                                    pnt = trc_rd_split(last, "(!,!,!)",
                                            &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[1].Set[i].Lower,
                                            &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[1].Set[i].Upper,
                                            &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[1].Set[i].Step );
                                    if ( !pnt || last == pnt )
                                    {
                                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                                        LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size --;
                                        break;
                                    }
                                }
                                if ( i < rank ) break;
                            }
                            else
                            {   /* put (add), create new */
                                DBG_ASSERT(__FILE__, __LINE__, Trace.Mode == 1);

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
                                    if ( !pnt || last == pnt )
                                    {
                                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                                        LoopBeg->pChunkSet[0].Size --;
                                        break;
                                    }
                                }
                                if ( i < rank ) break;
                            }
                        }
                        else
                        {   /* there is no local part of the loop on this processor */

                            if ( Trace.Mode == 0 )
                            {   /* compare */

                                if ( !LoopBeg->pChunkSet || !LoopBeg->pChunkSet[0].Chunks ||
                                     iters_Compare(&block, &LoopBeg->pChunkSet[0].Chunks[0]) )
                                {
                                    /* blocks or iteration sets are different */
                                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                                "*** READTRACE ***: Initial set of iterations is different for loop %d."
                                                "Program aborted.\n", no );
                                }

                                LoopBeg->iCurCPU++;

                                DBG_ASSERT(__FILE__, __LINE__, LoopBeg->iCurCPU < Trace.TraceCPUCount);

                                /* store this information for debugging purposes */
                                LoopBeg->iCurCPUMaxSize = LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size = 1;
                                mac_malloc(LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks, ITERBLOCK *, sizeof(ITERBLOCK), 0);

                                /* store global iteration set */
                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[0] = block;

                                /*LoopBeg->iCurCPUMaxSize = LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size = -1;
                                LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks = NULL;*/
                                /* exit */
                            }
                            else
                            {   /* put (add) */
                                DBG_ASSERT(__FILE__, __LINE__, Trace.Mode == 1);

                                LoopBeg->iCurCPUMaxSize = LoopBeg->pChunkSet[0].Size = 1;
                                mac_malloc(LoopBeg->pChunkSet[0].Chunks, ITERBLOCK *, sizeof(ITERBLOCK), 0);

                                /* store global iteration set */
                                LoopBeg->pChunkSet[0].Chunks[0] = block;
                            }
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

                        if ( Trace.Mode == 0 )
                            trc_cmp_beginstruct( trc_filebuff, Line, (short)no, Type, (byte)rank, NULL, NULL, NULL);
                        else
                        {
                            trc_put_beginstruct( trc_filebuff, Line, (short)no, Type, (byte)rank, NULL, NULL, NULL);
                        }
                    }
                    else
                    {
                        error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                    }
                }
                break;
            }
            case N_CHUNK:
            {
                DvmType tmp;

                if ( Trace.pCurCnfgInfo != NULL && Trace.CurStruct != -1 )
                {
                    SYSTEM_RET( Len, strlen, (KeyWords[N_CHUNK]) );
                    pnt = trc_rdwrt_buff + Len + 1; /* 1 is for ':' character */

                    /* investigate if there is enough memory in current construct */
                    LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

                    DBG_ASSERT(__FILE__, __LINE__, LoopBeg->pChunkSet != NULL
                                                   && LoopBeg->iCurCPU >= 0
                                                   && LoopBeg->iCurCPU < Trace.TraceCPUCount
                                                   && LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size > 1
                                                   && LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks != NULL );

                    if ( LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size == LoopBeg->iCurCPUMaxSize )
                    {   /* need to reallocate memory */
                        LoopBeg->iCurCPUMaxSize += (((int)1 << (Trace.pCurCnfgInfo->Rank + 1)) - 1);
                        LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks = (ITERBLOCK *) realloc(LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks, sizeof(ITERBLOCK)*LoopBeg->iCurCPUMaxSize);

                        if ( LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks == NULL )
                            epprintf(MultiProcErrReg1, __FILE__,__LINE__, "*** RTS err: No memory.\n");
                    }

                    for (i=0; i<Trace.pCurCnfgInfo->Rank; i++)
                    {
                        last = pnt;
                        pnt = trc_rd_split(last, "(!,!,!)",
                                &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size].Set[i].Lower,
                                &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size].Set[i].Upper,
                                &LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size].Set[i].Step );

                        LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size].Rank =
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

                    LoopBeg->pChunkSet[LoopBeg->iCurCPU].Chunks[LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size].vtr = (byte) tmp;
                    LoopBeg->pChunkSet[LoopBeg->iCurCPU].Size++;
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
                        if( hash2_Find(&(Trace.hIters), Trace.CurStruct, LI) == -1 )
                            /* was not traced */
                        {
                            if ( Trace.Mode == 0 ) /* compare mode */
                            {   /* we need to switch to add mode */
                                Trace.Mode = 1;
                                tmpTraceRecord = Trace.CurTraceRecord;
                                tmpStructInfo = Trace.pCurCnfgInfo;
                                Trace.CurTraceRecord = table_Count(&Trace.tTrace) - 1;
                            }
                            /* else leave the mode the same */

                            trc_put_iteration_flash_par( Rank, Index, LI );
                        }
                        else
                            /* was traced: it is a duplicate */
                        {
                            if ( Trace.Mode == 1 ) /* add mode */
                            {   /* we need to switch to compare mode */
                                Trace.Mode = 0;
                                Trace.CurTraceRecord = tmpTraceRecord;
                            }
                            /* else leave the mode the same */

                            trc_cmp_iteration(Index);
                            Trace.LI = LI; /* important for correct iterations fetching */
                            trc_cmp_iteration_flash(); /* force the iteration comparison */
                        }
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
                    if ( Trace.Mode == 1 ) /* add mode */
                    {
                        if ( tmpStructInfo == Trace.pCurCnfgInfo )
                        {   /* it is the end of the correct loop */
                            /* we need to switch to compare mode */
                            Trace.Mode = 0;
                            Trace.CurTraceRecord = tmpTraceRecord;
                        } /* else do nothing */
                    }

                    LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);
                    pInfo = Trace.pCurCnfgInfo;

                    if ( Trace.Mode == 1 ) /* add mode */
                    {
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
                                                                error_Message( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, szFileName, Trace.StrCntr, "*** CMPTRACE *** : ", ErrString[ERR_RD_FAILED_CS], Trace.CPU_Num, (UDvmType) Trace.StrCntr );
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
                    {   /* compare mode */
                        trc_cmp_endstruct( trc_filebuff, Line, No, Trace.pCurCnfgInfo ? Trace.pCurCnfgInfo->Line : -1 );

                        if (TraceOptions.TraceLevel == level_CHECKSUM || TraceOptions.CalcChecksums == 1)
                        {                   /* verify checksums values */

                            if ( LoopBeg->LastRec == -1 )
                            {
                                error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_STRUCT );
                                break;
                            }

                            LoopEnd = table_At(STRUCT_END, &Trace.tTrace, LoopBeg->LastRec);

                            Count = trc_rd_gets( hf );
                            Trace.StrCntr += Count;
                            if (Count == 0) break;

                            if (trc_rdwrt_buff[0]=='C' && trc_rdwrt_buff[1]=='S')
                            {
                                last = trc_rdwrt_buff+2;
                                pnt = trc_rd_split( last, "(@,@,!,!,@)", trc_filebuff, trc_filebuff1, &Line, &No, smode);

                                if (NULL != pnt && pnt != last && pInfo->Type)
                                {
                                    while (NULL != pnt && pnt != last)
                                    {
                                        /* Find the appropriate CS record attached to the loop */
                                        /* Additional checksums can be added !!!!! */

                                        found=0; /* array search */
                                        for (i = table_Count(&Trace.tArrays)-1; i>=0; i--)
                                        {
                                            pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, i);

                                            if (((UDvmType)pArr->ulLine) == Line && ((DvmType)(pArr->iNumber)) == No && !strcmp(pArr->szOperand, trc_filebuff)
                                                    && !strFileCmp(pArr->szFile, trc_filebuff1))
                                            {
                                                found = 1;
                                                break;
                                            }
                                        }

                                        if ( found )
                                        {
                                            found = 0;
                                            for ( j=0; j<LoopEnd->csSize; j++ )
                                            {
                                                if ( LoopEnd->checksums[j].pInfo == pArr )
                                                {
                                                    found = 1;
                                                    break;
                                                }
                                            }

                                            last = pnt;
                                            pnt  = trc_rd_split(last, "=\"#d\"", &checksum);

                                            if ( !found )
                                            {
                                                DBG_ASSERT(__FILE__, __LINE__, 0);

                                                /* need to add checksum to the list */
                                                mac_realloc(LoopEnd->checksums, CHECKSUM *, LoopEnd->checksums, (LoopEnd->csSize + 1)*sizeof(CHECKSUM), 0);
                                                LoopEnd->checksums[LoopEnd->csSize].pInfo  = pArr;
                                                LoopEnd->checksums[LoopEnd->csSize].lArrNo = i;
                                                LoopEnd->checksums[LoopEnd->csSize].pAddr  = NULL;

                                                if (strlen(smode) == 2 && smode[0]=='r' && smode[1]=='w')
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
                                            }

                                            if (NULL == pnt) error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_SYNTAX);
                                            else if (last == pnt)
                                                    {
                                                        if (!printed)
                                                        {
                                                            error_Message( TraceOptions.ErrorToScreen ? NULL : TraceOptions.ErrorFile, &TraceCompareErrors, szFileName, Trace.StrCntr, "*** CMPTRACE *** : ", ErrString[ERR_RD_FAILED_CS], Trace.CPU_Num, (UDvmType) Trace.StrCntr );
                                                            printed = 1;
                                                        }

                                                        if (found)
                                                        {    /* compare checksum values and continue reading input file */
                                                             /* don't check smode correctness */
                                                             if (LoopEnd->checksums[j].errCode != 0)
                                                                 error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_TRACEF_MISMATCH);
                                                        }
                                                        else
                                                             LoopEnd->checksums[LoopEnd->csSize-1].errCode = 0;
                                                    }
                                                    else
                                                    {
                                                        if (found)
                                                        {
                                                            if ( LoopEnd->checksums[j].sum != checksum )
                                                                error_CmpTrace(szFileName, Trace.StrCntr, ERR_RD_TRACEF_MISMATCH);
                                                        }
                                                        else
                                                            LoopEnd->checksums[LoopEnd->csSize-1].sum = checksum;
                                                    }
                                        }
                                        else
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

                         /* fin CS section */
                        }
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
                    if ( Trace.Mode == 0 ) /* compare mode */
                        trc_cmp_skip( trc_filebuff, Line );
                    else                   /* add mode */
                    {
                        trc_put_skip( trc_filebuff, Line );
                    }
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
                                        if ( Trace.Mode == 0 ) /* compare mode */
                                            trc_cmp_variable(trc_filebuff, Line,  trc_filebuff1,
                                                trc_READVAR, (short)Type, &Value,
                                                (byte)(Key == N_R_READ), NULL);
                                        else                   /* add mode */
                                        {
                                            trc_put_variable(trc_filebuff, Line,  trc_filebuff1,
                                                trc_READVAR, (short)Type, &Value,
                                                (byte)(Key == N_R_READ), NULL);
                                        }
                                    break;
                                case N_POST_WRITE :
                                case N_R_POST_WRITE :
                                    if ( Trace.Mode == 0 ) /* compare mode */
                                        trc_cmp_variable(trc_filebuff, Line,  trc_filebuff1,
                                            trc_POSTWRITEVAR, (short)Type, &Value,
                                            (byte)(Key == N_R_POST_WRITE), NULL);
                                    else                   /* add mode */
                                    {
                                        trc_put_variable(trc_filebuff, Line,  trc_filebuff1,
                                            trc_POSTWRITEVAR, (short)Type, &Value,
                                            (byte)(Key == N_R_POST_WRITE), NULL);
                                    }
                                    break;
                            }
                            Success = 1;
                        }
                    }
                }
                if( Success == 0 )
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX);

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
                            if ( Trace.Mode == 0 ) /* compare mode */
                                trc_cmp_variable(trc_filebuff, Line, "",
                                    trc_REDUCTVAR, (short)Type, &Value, 1, NULL);
                            else                   /* add mode */
                            {
                                trc_put_variable(trc_filebuff, Line, "",
                                    trc_REDUCTVAR, (short)Type, &Value, 1, NULL);
                            }
                            Success = 1;
                        }
                    }
                }
                if( Success == 0 )
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );

                break;
            }
            case N_PRE_WRITE:
            case N_R_PRE_WRITE:
            {
                DvmType Type;

                /* K: [<type>] <operand>; { <file>, <line> } */

                SYSTEM_RET( Len, strlen, ( KeyWords[ Key ] ) );
                pnt = trc_rd_split( trc_rdwrt_buff + Len, ":[!]@;{@,!}", &Type, trc_filebuff, trc_filebuff, &Line );
                if (pnt)
                {
                    if ( Trace.Mode == 0 ) /* compare mode */
                        trc_cmp_variable(trc_filebuff, Line, "",
                            trc_PREWRITEVAR, (short)Type, NULL,
                            (byte)(Key == N_R_PRE_WRITE), NULL);
                    else                   /* add mode */
                    {
                        trc_put_variable(trc_filebuff, Line, "",
                            trc_PREWRITEVAR, (short)Type, NULL,
                            (byte)(Key == N_R_PRE_WRITE), NULL);
                    }
                }
                else
                    error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_SYNTAX );

                break;
            }
            case N_END_TRACE:
                end_trace_exist = 1;
                break;
            default : error_CmpTrace( szFileName, Trace.StrCntr, ERR_RD_UNDEF_KEY );
        }
    }
    while( Trace.ErrCode == SUCCESS && !end_trace_exist && ( LineRead || (( Count = trc_rd_gets( hf ) ) != 0 )) && EnableTrace );

    if ( EnableTrace == 0 )
    {
        /* interrupt the program execution: input multiprocessor trace was not loaded correctly */
        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** READTRACE *** : There were some errors during loading multiprocessor trace \"%s\". "
                    "Program aborted.\n", szFileName );
    }

    if ( EnableTrace && Trace.ErrCode == SUCCESS && !end_trace_exist )
    {
        error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_RD_TRACE_INCOMPLETE, szFileName );
    }
}

#endif /* _TRC_MERGE_C_ */
