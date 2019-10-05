
#ifndef _TRC_PUT_C_
#define _TRC_PUT_C_
/*****************/

void trc_put_beginstruct(char *File, UDvmType Line, DvmType No, byte Type, byte Rank, DvmType *Init, DvmType *Last, DvmType *Step)
{
    /* type: 0 - SL; 1 - PL; 2 - TR */

    STRUCT_BEGIN loop;
    FILE *pDst = NULL;
    int nCanTrace, i;
    char buf[MAX_NUMBER_LENGTH];
    s_PARLOOP    *PL = NULL;

    if( TraceOptions.SaveThroughExec &&
        ( TraceOptions.TraceMode == mode_CONFIG_WRITETRACE || TraceOptions.TraceMode == mode_WRITETRACE )
      )
    {
        pDst = Trace.TrcFileHandle;
    }

    loop.RecordType = trc_STRUCTBEG;
    SYSTEM(strncpy, (loop.File, File, MaxSourceFileName));
    loop.File[MaxSourceFileName] = 0;

    loop.Line = Line;
    loop.num = NULL;
    loop.pChunkSet = NULL;
    loop.iCurCPU = loop.iCurCPUMaxSize = -1;
    loop.LastRec = -1;

    loop.pCnfgInfo = trc_InfoFindForCurrentLevel(No, Type, Rank, loop.File, Line);
    trc_InfoSetup(loop.pCnfgInfo, Init, Last, Step);

    /* check the correctness of numeration constraints */
    if ( Type == 1 && Trace.CurPoint.selector == 0 && TraceOptions.TraceMode != mode_COMPARETRACE)
    { /* this is parallel loop which is not in the task region */
        NUMBER cur_end = Trace.CurPoint;

        ploop_end( &cur_end ); /* create the auxiliary number for the end of current loop */

        if  ( Trace.StartPoint && Trace.StartPoint->selector && (num_cmp(&Trace.CurPoint, Trace.StartPoint) < 0) &&
              (num_cmp(Trace.StartPoint, &cur_end) < 0) )
        {
            char tmp1[MAX_NUMBER_LENGTH], tmp2[MAX_NUMBER_LENGTH];

            pprintf(3, "*** RTS warning: CMPTRACE: Start point %s implies task region but actual construction is parallel loop;"
             " starting verbose trace from the point %s.\n", to_string(Trace.StartPoint, tmp1),
             to_string(&Trace.CurPoint, tmp2));

            *(Trace.StartPoint) = Trace.CurPoint;
        }

        if  ( Trace.FinishPoint && Trace.FinishPoint->selector && (num_cmp(&Trace.CurPoint, Trace.FinishPoint) < 0) &&
              (num_cmp(Trace.FinishPoint, &cur_end) < 0) )
        {
            char tmp1[MAX_NUMBER_LENGTH], tmp2[MAX_NUMBER_LENGTH];

            pprintf(3, "*** RTS warning: CMPTRACE: Finish point %s implies task region but actual construction is parallel loop;"
             " finishing verbose trace at the point %s.\n", to_string(Trace.FinishPoint, tmp1),
             to_string(&cur_end, tmp2));

            *(Trace.FinishPoint) = cur_end;
        }
    }

    nCanTrace = (TraceOptions.TraceMode == mode_COMPARETRACE || trc_InfoCanTrace(loop.pCnfgInfo, trc_STRUCTBEG));
    if( nCanTrace )
    {
        if( !TraceOptions.WriteEmptyIter && Trace.pCurCnfgInfo && !Trace.IterFlash )
            trc_put_iteration_flash();
    }

    Trace.pCurCnfgInfo = loop.pCnfgInfo;

    if( nCanTrace )
    {
        Trace.IterFlash = 0;

        if ( Type == 1 && Trace.IterControl && (TraceOptions.Iloc_left != -1 || TraceOptions.Iloc_right != -1) )
        {   /* TraceOptions.TraceMode != mode_COMPARETRACE automatically because of Trace.IterControl */
            /* save local indexes information */
            STRUCT_INFO   *pInfo = Trace.pCurCnfgInfo;
            DvmType          *InitIndexArray;
            DvmType          *LastIndexArray;
            DvmType          *StepArray;
            int           i;

            if ( DVM_VMS->ProcCount == 1 )
            {
                /* the case for sequential run */
                for( i = 0; i < pInfo->Rank; i++ )
                {
                    pInfo->CurLocal[i].Step = pInfo->Current[i].Step;
                    pInfo->CurLocal[i].Upper = pInfo->Current[i].Upper;
                    pInfo->CurLocal[i].Lower = pInfo->Current[i].Lower;
                }
            }
            else
            {
                mac_malloc(InitIndexArray, DvmType *, pInfo->Rank * sizeof(DvmType), 0);
                mac_malloc(LastIndexArray, DvmType *, pInfo->Rank * sizeof(DvmType), 0);
                mac_malloc(StepArray, DvmType *, pInfo->Rank * sizeof(DvmType), 0);

                if ( pllind_(InitIndexArray, LastIndexArray, StepArray) )
                { /* current loop has local part on this processor */
                    for( i = 0; i < pInfo->Rank; i++ )
                    {
                        if ( InitIndexArray[i] > LastIndexArray[i] )
                        {
                            pInfo->CurLocal[i].Step = -StepArray[i];
                            pInfo->CurLocal[i].Upper = InitIndexArray[i];
                            pInfo->CurLocal[i].Lower = LastIndexArray[i];
                        }
                        else
                        {
                            pInfo->CurLocal[i].Step = StepArray[i];
                            pInfo->CurLocal[i].Lower = InitIndexArray[i];
                            pInfo->CurLocal[i].Upper = LastIndexArray[i];
                        }
                    }
                }
                else
                { /* no local part */
                    for( i = 0; i < pInfo->Rank; i++ )
                    {
                        pInfo->CurLocal[i].Lower = pInfo->CurLocal[i].Upper = pInfo->CurLocal[i].Step = MAXLONG;
                    }
                }

                mac_free(&InitIndexArray);
                mac_free(&LastIndexArray);
                mac_free(&StepArray);
            }
        }

        if ( TraceOptions.TraceMode != mode_COMPARETRACE )
        {
            if ( Type )  /* is it parallel context ? */
            {
                /* saving current dynamic point value */
                mac_malloc(loop.num, NUMBER *, sizeof(NUMBER), 0);
                dvm_memcopy(loop.num, &Trace.CurPoint, sizeof(NUMBER));

                if ( ((TraceOptions.TraceLevel == level_CHECKSUM) || (TraceOptions.CalcChecksums == 1)) &&
                        (TraceHeaderRead == 0 || TraceOptions.TrapArraysAnyway) )
                {
                    if ( Type == 1 && Trace.CurArrList.collect == 1 )  /* parallel loop in task region */
                    {
                        /* save task region's arrays info and create new one for current loop */
                        Trace.AuxArrList.body = Trace.CurArrList.body;
                        Trace.AuxArrList.collect = 1;
                        Trace.CurArrList.body = NULL;
                    }

                    Trace.CurArrList.collect = 1;
                }

                if ( Type == 1 && Trace.vtr && *(Trace.vtr) &&
                        TraceOptions.TraceMode != mode_CONFIG )
                {
                    /* save iteration limits for verification purposes */
                    DBG_ASSERT(__FILE__, __LINE__, Init && Last && Step );

                    mac_malloc(loop.pChunkSet, CHUNKSET*, sizeof(CHUNKSET), 0);
                    loop.iCurCPU = 0;

                    if ( Trace.convMode == 0 )
                    {  /* source program is compiled with mbodies and -s option */
                        loop.iCurCPUMaxSize = loop.pChunkSet[0].Size = 2;
                    }
                    else
                    {
                        DBG_ASSERT(__FILE__, __LINE__, Trace.convMode == 1);

                        PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

                        if(PL == NULL)
                            epprintf(MultiProcErrReg1, __FILE__, __LINE__,
                                "*** RTS err: CMPTRACE: current context is not a parallel loop.\n");

                        if(PL->AMView == NULL && PL->Empty == 0)
                            epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: current parallel loop has not been mapped\n");

                        DBG_ASSERT(__FILE__, __LINE__, Rank == PL->Rank);

                        if(PL->Local && PL->HasLocal)
                            loop.iCurCPUMaxSize = loop.pChunkSet[0].Size = 2;
                        else
                            loop.iCurCPUMaxSize = loop.pChunkSet[0].Size = 1;
                    }

                    mac_malloc(loop.pChunkSet[0].Chunks, ITERBLOCK *,
                                sizeof(ITERBLOCK)*loop.pChunkSet[0].Size, 0);

                    loop.pChunkSet[0].Chunks[0].Rank = Rank;

                    for( i = 0; i < Rank; i++ )
                    {
                        loop.pChunkSet[0].Chunks[0].Set[i].Step  = Step[i];
                        loop.pChunkSet[0].Chunks[0].Set[i].Lower = Init[i];
                        loop.pChunkSet[0].Chunks[0].Set[i].Upper = Last[i];
                    }

                    if(Trace.convMode == 1 && PL->Local && PL->HasLocal)
                    {
                        loop.pChunkSet[0].Chunks[1].Rank = Rank;

                        for(i=0; i < Rank; i++)
                        {
                            if(PL->Invers[i])
                            {
                                loop.pChunkSet[0].Chunks[1].Set[i].Lower = PL->Local[i].Upper + PL->InitIndex[i];
                                loop.pChunkSet[0].Chunks[1].Set[i].Upper = PL->Local[i].Lower + PL->InitIndex[i];
                                loop.pChunkSet[0].Chunks[1].Set[i].Step  = -PL->Local[i].Step;
                            }
                            else
                            {
                                loop.pChunkSet[0].Chunks[1].Set[i].Lower = PL->Local[i].Lower + PL->InitIndex[i];
                                loop.pChunkSet[0].Chunks[1].Set[i].Upper = PL->Local[i].Upper + PL->InitIndex[i];
                                loop.pChunkSet[0].Chunks[1].Set[i].Step  = PL->Local[i].Step;
                            }
                        }
                    }

                    if ( Trace.convMode == 0 )
                    {
                         loop.pChunkSet[0].Chunks[1] = loop.pChunkSet[0].Chunks[0];
                    }
                }
            }

            if ( TraceOptions.SaveArrayFilename[0] != 0 && TraceOptions.SaveArrayID[0] != 0 && Trace.pCurCnfgInfo->Type
                    && Trace.FinishPoint && !Trace.ArrWasSaved && num_cmp(&Trace.CurPoint, Trace.FinishPoint) == 0)
            {
                save_array_with_ID( TraceOptions.SaveArrayID, to_string(Trace.FinishPoint, buf) );
            }

            if( TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE )
            {
                loop.pCnfgInfo->StrCount++;
                loop.pCnfgInfo->Bytes += trc_wrt_beginstruct( pDst, &loop );
            }
        }

        if( TraceOptions.TraceMode == mode_COMPARETRACE ||
            ( !TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG ) )
        {
            loop.Parent = Trace.CurIter;
            loop.Line_num = Trace.StrCntr;
            loop.CPU_Num = Trace.CPU_Num;
            Trace.CurIter = -1;
            Trace.CurStruct = table_Put( &Trace.tTrace, &loop );
        }
    }

    Trace.Level ++;
}

void trc_put_endstruct( char *File, UDvmType Line, DvmType No, UDvmType BegLine )
{
    ITERATION     *Iter;
    STRUCT_BEGIN  *LoopBeg;
    STRUCT_END    LoopEnd;
    STRUCT_INFO   *pInfo;
    dvm_ARRAY_INFO* pArr;
    DvmType          i, j;
    int           currCs=0;
    FILE          *pDst = NULL;
    char buf[MAX_NUMBER_LENGTH];

    pInfo = Trace.pCurCnfgInfo;

    if (NULL == pInfo)
    {
        EnableTrace = 0;
        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
        "*** RTS err: CMPTRACE: Abnormal loop exit has been detected for loop number %d. "
        "File: %s, Begin Line: %ld, End Line: %ld.\n",
        No, File, BegLine, Line);

        return;
    }

    if (TraceOptions.SaveThroughExec &&
        (TraceOptions.TraceMode == mode_CONFIG_WRITETRACE ||
         TraceOptions.TraceMode == mode_WRITETRACE))
    {
        pDst = Trace.TrcFileHandle;
    }

    Trace.Level--;

    LoopEnd.RecordType = trc_STRUCTEND;
    SYSTEM(strncpy, (LoopEnd.File, File, MaxSourceFileName));
    LoopEnd.File[MaxSourceFileName] = 0;

    LoopEnd.Line = Line;
    LoopEnd.pCnfgInfo = pInfo;
    LoopEnd.csSize = 0;
    LoopEnd.checksums = NULL;
    LoopEnd.num = NULL;

    for (i = 0; i < pInfo->Rank; i++)
    {
        pInfo->CurIter[i] = MAXLONG;
    }

    if ( Trace.IterCtrlInfo == pInfo ) /* pInfo already != NULL */
    {
        Trace.IterCtrlInfo = NULL;
        pCmpOperations->Variable = trc_put_variable;
    }

    if (TraceOptions.TraceMode == mode_COMPARETRACE ||
        trc_InfoCanTrace(pInfo, trc_STRUCTEND))
    {
        if ( TraceOptions.TraceMode != mode_COMPARETRACE )
        {
            if ( pInfo->Type )  /* is it parallel context ? */
            {
                /* saving current dynamic point value */
                mac_malloc(LoopEnd.num, NUMBER *, sizeof(NUMBER), 0);
                dvm_memcopy(LoopEnd.num, &Trace.CurPoint, sizeof(NUMBER));

                if ( (TraceOptions.TraceLevel == level_CHECKSUM) || (TraceOptions.CalcChecksums == 1) )
                {
                    if ( TraceHeaderRead == 0 || TraceOptions.TrapArraysAnyway )
                    { /* condition in which we use gathered information from Trace.CurArrList */
                      /* exclude reduction variables from arrays list */
                        list_remove_reduction(&(Trace.CurArrList));
                    }

                    if ( TraceHeaderRead == 0 )
                    { /* update trace header information */
                        lists_uucopy( &(pInfo->ArrList), &(Trace.CurArrList) ); /* copy unique with update */
                    }

                    if ( TraceHeaderRead == 0 || TraceOptions.TrapArraysAnyway )
                    { /* config checksums using trapped information */
                        LoopEnd.csSize = list_build_checksums( &(Trace.CurArrList), &(LoopEnd.checksums), 1 );
                    }
                    else
                    { /* config checksums using loaded header information */
                        LoopEnd.csSize = list_build_checksums( &(pInfo->ArrList), &(LoopEnd.checksums), 0 );
                    }

                    /* calculate checksums anyway */
                    cs_compute(LoopEnd.checksums, LoopEnd.csSize);

                    for (j = 0; j<LoopEnd.csSize; j++)
                    {
                        if (LoopEnd.checksums[j].errCode == 0)
                        {
                            pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, LoopEnd.checksums[j].lArrNo);

                            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_FAILED_CS, pArr->szOperand,
                                               pArr->szFile, pArr->ulLine, pArr->iNumber, to_string(&Trace.CurPoint, buf));
                        }
                        else
                        {
                            if (LoopEnd.checksums[j].errCode == 2)
                            {
                                pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, LoopEnd.checksums[j].lArrNo);

                                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_DIFF_REPL_ARR, pArr->szOperand,
                                                    pArr->szFile, pArr->ulLine, pArr->iNumber, to_string(&Trace.CurPoint, buf));
                            }
                        }
                    }

                    /* clean list structure */
                    list_clean(&(Trace.CurArrList));

                    if ( Trace.AuxArrList.collect )  /* it's the end of parallel loop in task region */
                    {
                        Trace.CurArrList = Trace.AuxArrList;
                        list_init( &Trace.AuxArrList );
                    }

                }   /* In the compare trace mode the reader module attaches computed checksums to the loop */
            }

            if ( TraceOptions.SaveArrayFilename[0] != 0 && TraceOptions.SaveArrayID[0] != 0 && Trace.pCurCnfgInfo->Type
                    && Trace.FinishPoint && !Trace.ArrWasSaved && num_cmp(&Trace.CurPoint, Trace.FinishPoint) == 0)
            {
                save_array_with_ID( TraceOptions.SaveArrayID, to_string(Trace.FinishPoint, buf) );
            }

            if ( TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE )
            {
                pInfo->StrCount += 1 + LoopEnd.csSize;
                pInfo->Bytes += trc_wrt_endstruct( pDst, &LoopEnd );
            }
        }
        /* in the compare trace mode restrictions are not checked */

        if (TraceOptions.TraceMode == mode_COMPARETRACE ||
            (!TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG))
        {
            if (Trace.CurStruct == -1)
            {
                /*_asm{int 2};
                cmptrace_Write();*/
                error_CmpTraceExt(-1, File, Line, ERR_TR_NO_CURSTRUCT );
                return;
            }

            LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);
            if ((LoopBeg->Line != BegLine) ||
                (LoopBeg->pCnfgInfo->No != No))
            {
                error_CmpTraceExt(-1, LoopBeg->File, LoopBeg->Line, ERR_CMP_OUT_STRUCT);
                return;
            };

            LoopEnd.Parent = LoopBeg->Parent;

            Trace.CurIter = LoopBeg->Parent;
            if (Trace.CurIter != -1)
            {
                Iter = table_At( ITERATION, &Trace.tTrace, Trace.CurIter );
                Trace.CurStruct = Iter->Parent;
            }
            else
            {
                Trace.CurStruct = -1;
            }
            LoopEnd.Line_num = Trace.StrCntr;
            LoopEnd.CPU_Num = Trace.CPU_Num;
            LoopBeg->LastRec = table_Put( &Trace.tTrace, &LoopEnd );
        }
        Trace.IterFlash = 1;
    }

    Trace.pCurCnfgInfo = pInfo->pParent;
}

/* This function will not be executed with number limitations mechanism ON
 * (denied).
 * */

void trc_put_chunk(s_PARLOOP *PL)
{
    int           i;
    UDvmType Size;
    FILE          *pDst = NULL;
    CHUNK         Chunk;

    Chunk.RecordType = (byte) trc_CHUNK;
    Chunk.block.vtr  = (byte) *Trace.vtr;
    Chunk.block.Rank = PL->Rank;

    for( i=0; i < PL->Rank; i++ )
    {
        /* write iterations set "as is" */
        Chunk.block.Set[i].Lower = *(PL->MapList[i].InitIndexPtr);
        Chunk.block.Set[i].Upper = *(PL->MapList[i].LastIndexPtr);
        Chunk.block.Set[i].Step  = *(PL->MapList[i].StepPtr);
    }

    if( TraceOptions.SaveThroughExec &&
        ( TraceOptions.TraceMode == mode_CONFIG_WRITETRACE || TraceOptions.TraceMode == mode_WRITETRACE )
    )
    {
        pDst = Trace.TrcFileHandle;
    }

    if(  TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE )
    {
        Size = trc_wrt_chunk( pDst, &Chunk );

        if( Trace.pCurCnfgInfo )
        {
            Trace.pCurCnfgInfo->Bytes += Size;
            Trace.pCurCnfgInfo->StrCount++;
        }
        else
            DBG_ASSERT(__FILE__, __LINE__, 0)
    }

    if( !TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG
            && TraceOptions.TraceLevel != level_NONE )
    {

        Chunk.Line_num = Trace.StrCntr;
        Chunk.CPU_Num = Trace.CPU_Num;
        table_Put( &Trace.tTrace, &Chunk );
    }
}

void trc_put_iteration(DvmType* index)
{
    int i;

    if (Trace.pCurCnfgInfo)
    {
        Trace.CurIter = -1;

        if ( Trace.IterControl )
        {
            if ( Trace.IterCtrlInfo )
            {
                if ( Trace.IterCtrlInfo == Trace.pCurCnfgInfo )
                {
                    if ( out_of_bounds( index ) )
                            return ;

                    Trace.IterCtrlInfo = NULL;
                    pCmpOperations->Variable = trc_put_variable;
                }
                else
                        return ;
            }
            else
            {
                if ( Trace.pCurCnfgInfo->Type != 2 && out_of_bounds( index ) )
                {
                    Trace.IterCtrlInfo = Trace.pCurCnfgInfo;
                    pCmpOperations->Variable = dummy_var;

                    return ;
                }
                /* else - continue tracing */
            }
        }

        Trace.IterFlash = 0;

        for (i = 0; i < Trace.pCurCnfgInfo->Rank; i++)
            Trace.pCurCnfgInfo->CurIter[i] = index[i];

        if (TraceOptions.WriteEmptyIter)
            trc_put_iteration_flash_par(Trace.pCurCnfgInfo->Rank, NULL,
                (Trace.pCurCnfgInfo->Rank > 1) ? trc_CalcLI() : Trace.pCurCnfgInfo->CurIter[0]);
    }
}

void trc_put_iteration_flash_par(byte Rank, DvmType* Index, DvmType LI)
{
    short i;
    ITERATION Iter;
    STRUCT_BEGIN *Loop;
    UDvmType Size;
    FILE *pDst = NULL;

    if (TraceOptions.SaveThroughExec &&
        (TraceOptions.TraceMode == mode_CONFIG_WRITETRACE ||
         TraceOptions.TraceMode == mode_WRITETRACE))
    {
        pDst = Trace.TrcFileHandle;
    }

    if (Trace.pCurCnfgInfo == NULL)
    {
        error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_TR_NO_CURSTRUCT);
        return;
    }

    Iter.Rank = Rank;
    Iter.LI = LI;

    if (NULL != Index)
    {
        dvm_ArrayCopy(DvmType, Iter.Index, Index, Rank);
        for (i = 0; i < Trace.pCurCnfgInfo->Rank; i++)
        {
            Trace.pCurCnfgInfo->CurIter[i] = Index[i];
        }
    }
    else
    {
        dvm_ArrayCopy(DvmType, Iter.Index, Trace.pCurCnfgInfo->CurIter, Rank);
    }

    if (TraceOptions.TraceMode == mode_COMPARETRACE ||
        trc_InfoCanTrace(Trace.pCurCnfgInfo, trc_ITER))
    {
        Trace.IterFlash = 1;

        if (TraceOptions.TraceMode != mode_COMPARETRACE &&
            (TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE))
        {
            Size = trc_wrt_iter( pDst, &Iter );

            if (NULL != Trace.pCurCnfgInfo)
            {
                Trace.pCurCnfgInfo->StrCount++;
                Trace.pCurCnfgInfo->Bytes += Size;
                Trace.pCurCnfgInfo->Iters++;
            }
        }

        if (TraceOptions.TraceMode == mode_COMPARETRACE ||
            (!TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG))
        {
            if (-1 == Trace.CurStruct)
            {
                error_CmpTraceExt(-1, DVM_FILE[0], DVM_LINE[0], ERR_TR_NO_CURSTRUCT);
                return;
            }

            Loop = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

            Iter.RecordType = trc_ITER;
            Iter.Parent = Trace.CurStruct;
            Iter.Checked = 0;

            Iter.Line_num = Trace.StrCntr;
            Iter.CPU_Num  = Trace.CPU_Num;
            Trace.CurIter = table_Put(&Trace.tTrace, &Iter);

            // if (TraceOptions.TraceMode == mode_COMPARETRACE)
            hash2_Insert(&(Trace.hIters), Trace.CurStruct, Iter.LI, Trace.CurIter);
        }
    }
}

void trc_put_iteration_flash(void)
{
    if (Trace.pCurCnfgInfo != NULL)
    {
        trc_put_iteration_flash_par((byte)(Trace.pCurCnfgInfo->Rank), NULL, trc_CalcLI());
    }
}


void trc_put_variable(char* File, UDvmType Line, char* Operand, enum_TraceType iType,
                      DvmType Type, void* Value, byte Reduct, void* pArrBase)
{
    UDvmType Size;
    VARIABLE    Var;
    FILE *pDst = NULL;
    int nCheck;

    if ( ( Operand[0] == 0 ) && Reduct )      /* add diagnostics for reduction variables */
        Operand = "<reduct.var>";

    /* While reading trace from file trace restrictions are not checked */
    if (TraceOptions.TraceMode != mode_COMPARETRACE)
    {
        if (!trc_InfoCanTrace(Trace.pCurCnfgInfo, iType))
            return;

        if ( (iType != trc_PREWRITEVAR) && ((TraceOptions.TraceLevel == level_CHECKSUM) || (TraceOptions.CalcChecksums == 1)) )
        {
            if ( pArrBase != NULL && Trace.CurArrList.collect )
            {
                /*if ( TraceOptions.ChecksumDisarrOnly )
                {  relaxed tstDVMarray() function
                    ArrayHeader = (DvmType *)pArrBase;  /* pointer to header
                    for(nCheck=0; nCheck < DACount; nCheck++)
                        if(DAHeaderAddr[nCheck] == ArrayHeader)
                            break;
                    if(nCheck < DACount)
                        updateListA(&(Trace.CurArrList), pArrBase, (byte)((iType == trc_POSTWRITEVAR)?2:1));
                }
                else*/
                if ( !TraceOptions.ChecksumDisarrOnly || Trace.isDistr )
                {
                    updateListA(&(Trace.CurArrList), pArrBase, (byte)((iType == trc_POSTWRITEVAR)?2:1));
                }
            }

            if ( TraceOptions.TraceLevel == level_CHECKSUM )
                return;
        }

        nCheck = trc_ArrayCanTrace(Value, pArrBase, iType);
        if (nCheck < 0)
            error_CmpTraceExt(-1, File, Line, ERR_CMP_ARRAY_OUTOFBOUND, Operand);
        if (!nCheck)
            return;
    }

    if (!TraceOptions.WriteEmptyIter && Trace.pCurCnfgInfo && !Trace.IterFlash)
        trc_put_iteration_flash();

    if( TraceOptions.SaveThroughExec &&
        ( TraceOptions.TraceMode == mode_CONFIG_WRITETRACE || TraceOptions.TraceMode == mode_WRITETRACE )
      )
    {
        pDst = Trace.TrcFileHandle;
    }

    Var.RecordType = (byte)iType;
    Var.vType = Type;
    Var.Reduct = Reduct;
    if (trc_PREWRITEVAR != iType)
    {
        if ( TraceOptions.EnableNANChecks && (Type == rt_FLOAT || Type == rt_DOUBLE) )
        {
            if ( dbg_isNAN(Value, Type) )
            {
                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NAN_VALUE);
            }
        }

        trc_StoreValue(&(Var.val), Value, Type);
    }

    SYSTEM(strncpy, (Var.Operand, Operand, MaxOperand));
    Var.Operand[MaxOperand] = 0;

    SYSTEM(strncpy, (Var.File, File, MaxSourceFileName));
    Var.File[MaxSourceFileName] = 0;

    Var.Line = Line;

    if( TraceOptions.TraceMode != mode_COMPARETRACE &&
        ( TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE ) )
    {
        switch( iType )
        {
            case trc_READVAR : Size = trc_wrt_readvar( pDst, &Var ); break;
            case trc_PREWRITEVAR : Size = trc_wrt_prewritevar( pDst, &Var ); break;
            case trc_POSTWRITEVAR : Size = trc_wrt_postwritevar( pDst, &Var ); break;
            case trc_REDUCTVAR : Size = trc_wrt_reductvar( pDst, &Var ); break;
            default: Size = 0;
        }

        if( Trace.pCurCnfgInfo )
        {
            Trace.pCurCnfgInfo->Bytes += Size;
            Trace.pCurCnfgInfo->StrCount++;
        }
        else
        {
            Trace.Bytes += Size;
            Trace.StrCount++;
        }
    }

    if( TraceOptions.TraceMode == mode_COMPARETRACE ||
        ( !TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG ) )
    {
        Var.Line_num = Trace.StrCntr;
        Var.CPU_Num  = Trace.CPU_Num;
        table_Put( &Trace.tTrace, &Var );
    }
}

void trc_put_skip( char *File, UDvmType Line )
{
    SKIP Skip;
    UDvmType Size;
    FILE *pDst = NULL;

    if( TraceOptions.TraceMode != mode_COMPARETRACE )
        if( !trc_InfoCanTrace(Trace.pCurCnfgInfo, trc_SKIP) )
            return;

    if( TraceOptions.SaveThroughExec &&
        ( TraceOptions.TraceMode == mode_CONFIG_WRITETRACE || TraceOptions.TraceMode == mode_WRITETRACE )
      )
    {
        pDst = Trace.TrcFileHandle;
    }

    Skip.RecordType = trc_SKIP;
    SYSTEM(strncpy, (Skip.File, File, MaxSourceFileName));
    Skip.File[MaxSourceFileName] = 0;

    Skip.Line = Line;

    if( TraceOptions.TraceMode != mode_COMPARETRACE &&
        ( TraceOptions.SaveThroughExec || TraceOptions.TraceMode != mode_WRITETRACE ) )
    {
        Size = trc_wrt_skip( pDst, &Skip );

        if( Trace.pCurCnfgInfo )
        {
            Trace.pCurCnfgInfo->StrCount++;
            Trace.pCurCnfgInfo->Bytes += Size;
        }
        else
        {
            Trace.StrCount++;
            Trace.Bytes += Size;
        }
    }

    if( TraceOptions.TraceMode == mode_COMPARETRACE ||
        ( !TraceOptions.SaveThroughExec && TraceOptions.TraceMode != mode_CONFIG ) )
    {
        Skip.Line_num = Trace.StrCntr;
        Skip.CPU_Num  = Trace.CPU_Num;
        table_Put( &Trace.tTrace, &Skip );
    }
}


#endif /* _TRC_PUT_C_ */
