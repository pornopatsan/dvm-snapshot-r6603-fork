
#ifndef _TRC_CMP_C_
#define _TRC_CMP_C_
/*****************/

/*********************************************************************/

void trc_cmp_beginstruct(char *File, UDvmType Line, DvmType No, byte Type, byte Rank, DvmType *Init, DvmType *Last, DvmType *Step)
{
    STRUCT_BEGIN *Loop;
    ANY_RECORD *Rec;
    DvmType TraceRecord;
    int nCanTrace, i;
    STRUCT_INFO* pCnfgInfo;
    char buf[MAX_NUMBER_LENGTH];

    Trace.CurPreWriteRecord = -1;

    pCnfgInfo = trc_InfoFindForCurrentLevel(No, Type, Rank, File, Line);
    trc_InfoSetup(pCnfgInfo, Init, Last, Step);

    nCanTrace = trc_InfoCanTrace(pCnfgInfo, trc_STRUCTBEG);

    if( nCanTrace )
    {
        if( Trace.pCurCnfgInfo && !Trace.IterFlash )
            if (!trc_cmp_iteration_flash())
            {
                /* error_CmpTraceExt( Trace.CurStruct, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_STRUCT ); */
                Trace.pCurCnfgInfo = pCnfgInfo;
                return ;
            }
    }

    Trace.pCurCnfgInfo = pCnfgInfo;

    if ( Trace.CurStruct != -1 && Trace.CurIter == -1 )
    {
        /* beginning of a loop of absent iteration */
        return ;
    }

    if( nCanTrace )
    {
        if ( TraceOptions.SaveArrayFilename[0] != 0 && TraceOptions.SaveArrayID[0] != 0 && Type && Trace.FinishPoint
                && !Trace.ArrWasSaved && num_cmp(&Trace.CurPoint, Trace.FinishPoint) == 0 && Trace.Mode == -1)
        {
            save_array_with_ID( TraceOptions.SaveArrayID, to_string(Trace.FinishPoint, buf) );
        }

        if( Trace.CurTraceRecord < table_Count(&Trace.tTrace) )
        {
            Rec = table_At( ANY_RECORD, &Trace.tTrace, Trace.CurTraceRecord );

            if( Rec->RecordType != trc_STRUCTBEG )
            {
                TraceRecord = trc_cmp_forward( Trace.CurTraceRecord, trc_STRUCTBEG );
                if( TraceRecord == -1 )
                {
                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_STRUCT);
                    return;
                }

                Loop = table_At(STRUCT_BEGIN, &Trace.tTrace, TraceRecord );

                if( ( Loop->pCnfgInfo->No != No ) ||
                    ( Loop->pCnfgInfo->Rank != Rank ) ||
                    ( Loop->pCnfgInfo->Type != Type ))
                {
                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_STRUCT);
                    return;
                }
                /*  Extra variables in the trace ! */
                for ( i=Trace.CurTraceRecord; i<TraceRecord; i++ )
                {
                    error_CmpTraceExt(i, File, Line, ERR_CMP_NO_INFO);
                }
                Trace.CurTraceRecord = TraceRecord; /* correct comparator state */
            }

            Loop = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurTraceRecord);

            if( ( Loop->pCnfgInfo->No != No ) ||
                ( Loop->pCnfgInfo->Rank != Rank ) ||
                ( Loop->pCnfgInfo->Type != Type ))
            {
                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_STRUCT);
                return;
            }

            Trace.CurStruct = Trace.CurTraceRecord;
            Trace.CurTraceRecord ++;
            Trace.CurIter = -1;
            Trace.IterFlash = 0;

            if ( Loop->pChunkSet && Init && Last && Step )
            { /* check if the iteration limits are correct */
                byte Err = 0;

                if ( Loop->pChunkSet[0].Chunks && Loop->pChunkSet[0].Size > 0 )
                {
                    for( i = 0; i < Rank; i++ )
                    {
                        if ( Loop->pChunkSet[0].Chunks[0].Set[i].Lower != Init[i] ||
                             Loop->pChunkSet[0].Chunks[0].Set[i].Upper != Last[i] ||
                             Loop->pChunkSet[0].Chunks[0].Set[i].Step  != Step[i] )
                        {
                            Err = 1;
                            break;
                        }
                    }
                }
                else Err = 1;

                if ( Err )
                {
                    /* blocks or iteration sets are different */
                    epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** CMPTRACE ***: Initial set of iterations is different for loop %d."
                                "Program aborted.\n", No );
                    /* unreachable code */
                    exit(-1);
                }
            }

            if( Trace.pCurCnfgInfo != Loop->pCnfgInfo )
            {
                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_CURSTRUCT);
            }
        }
        else
            error_CmpTraceExt(-1, File, Line, ERR_CMP_NO_STRUCT);
    }
}

void trc_cmp_endstruct(char *File, UDvmType Line, DvmType No, UDvmType BegLine)
{
    STRUCT_END   *LoopEnd;
    STRUCT_BEGIN *LoopBeg;
    dvm_ARRAY_INFO* pArr;
    ITERATION  *Iter;
    ANY_RECORD *Rec;
    char       buf[MAX_NUMBER_LENGTH];

    if (NULL == Trace.pCurCnfgInfo)
    {
        EnableTrace = 0;

        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
        "*** RTS err: CMPTRACE: Abnormal loop exit has been detected for loop number %d. "
        "File: %s, Begin Line: %ld, End Line: %ld.\n",
        No, File, BegLine, Line);

        return;
    }

    Trace.CurPreWriteRecord = -1;

    if ( Trace.IterCtrlInfo == Trace.pCurCnfgInfo )
    {
        Trace.IterCtrlInfo = NULL;
        pCmpOperations->Variable = trc_cmp_variable;
    }

    if( trc_InfoCanTrace(Trace.pCurCnfgInfo, trc_STRUCTEND) )
    {
        if( Trace.CurStruct == -1 )
        {
            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_CURSTRUCT);
            Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
            return;
        }

        LoopBeg = table_At( STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct );
        if( Trace.pCurCnfgInfo != LoopBeg->pCnfgInfo )
        {
            /* situation of missed iteration, skip the end of loop silently */
            /*error_CmpTraceExt( Trace.CurTraceRecord, File, Line, ERR_CMP_NO_CURSTRUCT);*/
            Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
            return ;
        }

        if ( LoopBeg->LastRec == -1 )
        {
            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_TR_NO_CURSTRUCT);
            Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
            return ;
        }

        Rec = table_At( ANY_RECORD, &Trace.tTrace, LoopBeg->LastRec );
        if( Rec->RecordType != trc_STRUCTEND )
        {
            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_CURSTRUCT);
            Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
            return ;
        }
        else
        {
            LoopEnd = (STRUCT_END *)Rec;

            if( ( LoopEnd->pCnfgInfo->Line != BegLine ) ||
                ( LoopEnd->pCnfgInfo->No != No )
            )
            {
                error_CmpTraceExt(LoopBeg->LastRec, LoopEnd->File, LoopEnd->Line, ERR_CMP_OUT_STRUCT);
                Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
                return ;
            }
        }

        Trace.CurTraceRecord = LoopBeg->LastRec ;

        if ((TraceOptions.TraceLevel == level_CHECKSUM || TraceOptions.CalcChecksums == 1)
                 && Trace.pCurCnfgInfo->Type && Trace.Mode == -1)
        {                       /* creating structures and comparing checksums for current parallel context
                                 * independently on presence of access to arrays on current CPU */
            CHECKSUM *css;
            int i, currCs = 0;

            mac_calloc(css, CHECKSUM *, LoopEnd->csSize, sizeof(CHECKSUM), 0);

            for (i=0; i < LoopEnd->csSize; i++)
                if (LoopEnd->checksums[i].errCode == 1)
                {

                    pArr = table_At(dvm_ARRAY_INFO, &(Trace.tArrays), LoopEnd->checksums[i].lArrNo);
                    if (pArr->pAddr)
                    {
                        css[currCs].pInfo = pArr;
                        css[currCs].lArrNo = LoopEnd->checksums[i].lArrNo;
                        css[currCs].accType = LoopEnd->checksums[i].accType;
                        currCs++;
                    }
                }

            /* the number and contents of checksum structures should be the same on all CPUs
               because it was read from the trace */
            if ( currCs > 0 )
                cs_compute(css, currCs);

            currCs = 0;

            for (i = 0; i<LoopEnd->csSize; i++)
                if (LoopEnd->checksums[i].errCode == 1)
                {
                    pArr = table_At(dvm_ARRAY_INFO, &Trace.tArrays, LoopEnd->checksums[i].lArrNo);

                    if (css[currCs].errCode == 1)
                    {
                        if ( TraceOptions.EnableNANChecks )
                        {
                             if (dbg_isNAN(&(css[currCs].sum), rt_DOUBLE ))
                             {
                                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NAN_VALUE);
                             }
                        }

                        if (trc_CompareValue( (VALUE *)(&(LoopEnd->checksums[i].sum)), &(css[currCs].sum) , rt_DOUBLE, 0 ) == 0 )
                        {
                          /* "Different checksums for array (\"%s\", \"%s\", %ld, %ld, \"%s\"): %.*G != %.*G at the point %s" */
                            char tmp[MAXDEBSTRINGLENGTH], tmp2[MAX_NUMBER_LENGTH];
                            int  sz;

                            to_string(&Trace.CurPoint, tmp);
                            sz = strlen(tmp);

                            if ( pArr->correctCSPoint == NULL )
                                sprintf(tmp + sz, "; previous correct point is absent");
                            else
                                sprintf(tmp + sz, "; previous correct point is %s", to_string(pArr->correctCSPoint, tmp2));

                            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_DIFF_CS_VALUES,
                                pArr->szOperand, pArr->szFile, pArr->ulLine, pArr->iNumber,
                                (LoopEnd->checksums[i].accType==1)?"r":((LoopEnd->checksums[i].accType==2)?"w":"rw"),
                                pArr->bIsDistr,
                                Trace.DoublePrecision, LoopEnd->checksums[i].sum, Trace.DoublePrecision, css[currCs].sum,
                                tmp );

                            if ( TraceOptions.SaveArrayFilename[0] != 0 && !Trace.ArrWasSaved && pArr->pAddr != NULL )
                                /* save array elements to the file */
                                save_array( pArr, to_string(&Trace.CurPoint, buf) );
                        }
                        else
                        {
                            /* store this point as correct for this array */

                            if ( pArr->correctCSPoint == NULL )
                                mac_malloc(pArr->correctCSPoint, NUMBER *, sizeof(NUMBER), 0);

                            dvm_memcopy(pArr->correctCSPoint, &Trace.CurPoint, sizeof(NUMBER));
                        }
                    }
                    else
                    {
                        if ( css[currCs].errCode == 0 )
                        {
                            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_FAILED_CS, pArr->szOperand,
                                               pArr->szFile, pArr->ulLine, pArr->iNumber, to_string(&Trace.CurPoint, buf));
                        }
                        else
                        {
                            if ( css[currCs].errCode == 2 )
                            {
                                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_DIFF_REPL_ARR, pArr->szOperand,
                                                    pArr->szFile, pArr->ulLine, pArr->iNumber, to_string(&Trace.CurPoint, buf));
                            }
                            else
                                DBG_ASSERT(__FILE__, __LINE__, 0 );
                        }
                    }

                    currCs++;
                }

            mac_free(&css);
        }

        if ( TraceOptions.SaveArrayFilename[0] != 0 && TraceOptions.SaveArrayID[0] != 0 && Trace.pCurCnfgInfo->Type
                && Trace.FinishPoint && !Trace.ArrWasSaved && num_cmp(&Trace.CurPoint, Trace.FinishPoint) == 0 && Trace.Mode == -1)
        {
            save_array_with_ID( TraceOptions.SaveArrayID, to_string(Trace.FinishPoint, buf) );
        }

        Trace.CurIter = LoopBeg->Parent;
        if( Trace.CurIter != -1 )
        {
            Iter = table_At( ITERATION, &Trace.tTrace, Trace.CurIter );
            Trace.CurStruct = Iter->Parent;
        }
        else
        {
            Trace.CurStruct = -1;
        }

        Trace.IterFlash = 1;
        Trace.CurTraceRecord ++;
    }

    Trace.pCurCnfgInfo = Trace.pCurCnfgInfo->pParent;
}

void trc_cmp_iteration(DvmType* index)
{
    int i;

    if( Trace.pCurCnfgInfo )
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
                    pCmpOperations->Variable = trc_cmp_variable;
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

        for( i = 0; i < Trace.pCurCnfgInfo->Rank; i++ )
        {
            Trace.pCurCnfgInfo->CurIter[i] = index[i];
        }
    }
}


byte trc_cmp_iteration_flash(void)
{
    ITERATION *Iter;
    STRUCT_BEGIN *Loop;
    DvmType LI;

    if( Trace.pCurCnfgInfo == NULL )
    {
        error_CmpTraceExt(Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_CURSTRUCT);
        Trace.CurIter = -1;
        return 0;
    }

    Trace.CurPreWriteRecord = -1;

    if( !trc_InfoCanTrace(Trace.pCurCnfgInfo, trc_ITER) )
        return 1;

    if( Trace.CurStruct == -1 )
    {
        error_CmpTraceExt(Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_CURSTRUCT);
        Trace.CurIter = -1;
        return 0;
    }

    Trace.IterFlash = 1;

   if ( Trace.Mode == -1 )
        LI = trc_CalcLI();
    else
    {
        DBG_ASSERT(__FILE__, __LINE__, Trace.Mode == 0);
        LI = Trace.LI;
        Trace.LI = -1;
    }

    Loop = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);

    if( Trace.pCurCnfgInfo != Loop->pCnfgInfo )
    {
        /* error_CmpTraceExt( Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_ITER, LI );*/
        /* iteration of a loop, nested in absent iteration, no diagnostics */
        Trace.CurIter = -1;
        return 0;
    }

    Trace.CurIter = hash2_Find(&(Trace.hIters), Trace.CurStruct, LI);
    if( Trace.CurIter == -1 )
    {
        error_CmpTraceExt(Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_ITER, LI );
        return 0;
    }

    Iter = table_At( ITERATION, &Trace.tTrace, Trace.CurIter );
    if( Iter->Checked )
    {
        error_CmpTraceExt(Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_CMP_DUAL_ITER, LI );
    }
    else
    {
        if ( Trace.Mode == -1 ) Iter->Checked = 1;
    }

    Trace.CurTraceRecord = Trace.CurIter + 1;

    return 1;
}


void trc_cmp_variable(char *File, UDvmType Line, char* Operand, enum_TraceType iType,
                      DvmType vType, void *Value, byte Reduct, void* pArrBase)
{
    VARIABLE *Var, *Var2;
    DvmType TraceRecord;
    int DiffRec, DiffTyp = 0;
    byte IterFound = 1;
    int nCheck;

    if ( !trc_InfoCanTrace(Trace.pCurCnfgInfo, iType) || TraceOptions.TraceLevel == level_CHECKSUM )
        return;

    if ( ( Operand[0] == 0 ) && Reduct )      /* add diagnostics for reduction variables */
        Operand = "<reduct.var>";

    nCheck = trc_ArrayCanTrace(Value, pArrBase, iType);
    if (nCheck < 0)
        error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_ARRAY_OUTOFBOUND, Operand);
    if (!nCheck)
        return;

    if( Trace.pCurCnfgInfo && !Trace.IterFlash )
    {
        if( !trc_cmp_iteration_flash() )
        {
            /* silently skip inforamtion about skipped variables */
            /* error_CmpTraceExt( Trace.CurStruct, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_INFO );*/
            return ;
        }
    }

    if( Trace.CurTraceRecord < table_Count( &Trace.tTrace ) )
    {
        Var = table_At( VARIABLE, &Trace.tTrace, Trace.CurTraceRecord );

        switch( Var->RecordType )
        {
            case trc_PREWRITEVAR :
            case trc_POSTWRITEVAR :
            case trc_READVAR :
            case trc_REDUCTVAR :
            case trc_SKIP :
                break;
            default :
                if ( Trace.CurStruct != -1 && Trace.CurIter == -1 )
                {
                    /* variable from absent iteration */
                }
                else
                     error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_INFO );
                return ;
        }

        if ( Var->RecordType == trc_SKIP )
        {
            if ( Trace.Mode == 0 )
            {
                /* trace accumulation compare mode, must save the data to special table */
                Var2 = table_GetNew(VARIABLE, &Trace.tSkipped);
                Var2->RecordType = (byte)iType;
                Var2->vType = vType;
                Var2->Reduct = Reduct;
                if (trc_PREWRITEVAR != iType)
                    trc_StoreValue(&(Var2->val), Value, vType);

                SYSTEM(strncpy, (Var2->Operand, Operand, MaxOperand));
                Var2->Operand[MaxOperand] = 0;

                SYSTEM(strncpy, (Var2->File, File, MaxSourceFileName));
                Var2->File[MaxSourceFileName] = 0;

                Var2->Line = Line;

                if ( hash1_Find(&Trace.hSkippedPointers, Trace.CurTraceRecord) == -1 )
                        /* it's the first skipped record */
                    hash1_Insert(&Trace.hSkippedPointers, Trace.CurTraceRecord, table_Count(&Trace.tSkipped)-1);

                return;
            }
            else /* real traces compare situation */
            {
                DvmType val;

                DBG_ASSERT(__FILE__, __LINE__, Trace.Mode == -1);

                val = hash1_Find(&Trace.hSkippedPointers, Trace.CurTraceRecord);

                if ( val != -1 ) /* there are some additional records stored in special table */
                {
                    Var2 = table_At( VARIABLE, &Trace.tSkipped, val );

                    if ( ( Var2->vType == vType ) && ( Var2->RecordType == iType ) &&
                              ( Var2->Reduct == Reduct) && (Var2->Line == Line) )
                    {
                        /* Remove k previous useless records in the trace and
                           put skipped records there */

                        /* identify the value of k */
                        ANY_RECORD *Rec = NULL;
                        DvmType CurTraceRecord = val + 1, i;

                        while( CurTraceRecord < table_Count( &Trace.tSkipped ) )
                        {
                            Rec = table_At( ANY_RECORD, &Trace.tSkipped, CurTraceRecord );

                            if( Rec->RecordType == trc_SKIP )
                                break;

                            CurTraceRecord++;
                        }

                        DBG_ASSERT(__FILE__, __LINE__, Rec != NULL && Rec->RecordType == trc_SKIP);

                        /* exclude the link from the hash */
                        hash1_Remove(&Trace.hSkippedPointers, Trace.CurTraceRecord);

                        /* k == CurTraceRecord - val; */

                        for ( i=val; i<CurTraceRecord; i++)
                            memcpy( table_At( char, &Trace.tTrace, Trace.CurTraceRecord - CurTraceRecord + i),
                                    table_At( char, &Trace.tSkipped, i ),
                                    Trace.tSkipped.ElemSize );

                        /* update Trace.CurTraceRecord value */
                        Trace.CurTraceRecord -= CurTraceRecord - val;
                        Var = Var2;
                    }
                }
            }
        }

        if( Trace.CurPreWriteRecord != -1 && Trace.ReductExprType == 1 )
        {
            if( iType == trc_POSTWRITEVAR && Reduct )
            {
                /* If you are here, the variable has been written.
                   Proceeding further as usual  */

                Trace.CurPreWriteRecord = -1;
                Trace.ReductExprType = 0;
            }
            return;
        }

        DiffRec = ( Var->RecordType != iType ) || ( Var->Reduct != Reduct );

        if ( !DiffRec && (Var->vType != vType) )
        {
            if ( (Var->vType == rt_INT) && (vType == rt_LONG) )
                DiffTyp = 1;
            else
                if ( (Var->vType == rt_LONG) && (vType == rt_INT) )
                    DiffTyp = 2;
                else
                    DiffRec = 1; /* really different types, DiffTyp == 0  */
                // TODO: rt_LLONG
        }

        /* Processing different variants of trace discrepancy  */

        if( DiffRec )
        {
            /* expression of type:if(red < any) red = any
               it is possible that the variable has not been written*/
            /* Record is in program and is absent in trace */
            /* Start of writing variable */

            if( iType == trc_PREWRITEVAR && Reduct && Trace.CurPreWriteRecord == -1 )
            {
                Trace.CurPreWriteRecord = Trace.CurTraceRecord;
                Trace.ReductExprType = 1;
                return;
            }

            /* expression of type: red = max(any, red).
               it is possible that some variablse have not been read */
            /* All operators are ignored till completion
               of reduction variable writting */

            if( iType == trc_POSTWRITEVAR && Trace.CurPreWriteRecord != -1 && Trace.ReductExprType == 0)
            {
                TraceRecord = trc_cmp_forward( Trace.CurTraceRecord + 1, trc_POSTWRITEVAR );
                if( TraceRecord != -1 )
                {
                    Var = table_At( VARIABLE, &Trace.tTrace, TraceRecord );
                    DiffRec = ( Var->RecordType != iType ) || ( Var->Reduct != Reduct );

                    if ( !DiffRec && (Var->vType != vType) )
                    {
                        if ( (Var->vType == rt_INT) && (vType == rt_LONG) )
                            DiffTyp = 1;
                        else
                            if ( (Var->vType == rt_LONG) && (vType == rt_INT) )
                                DiffTyp = 2;
                            else
                                DiffRec = 1; /* really different types, DiffTyp == 0  */
                            // TODO: rt_LLONG
                    }

                    if( !DiffRec )
                        Trace.CurTraceRecord = TraceRecord;
                }
                Trace.CurPreWriteRecord = -1;
            }

            /* expression of type:if(red < any) red = any.
               it is possible that the variable has not been written */
            /* Record is in trace and is absent in the program */
            /* Advancing forward till the completion of writing */

            if( Var->RecordType == trc_PREWRITEVAR && Var->Reduct && Trace.CurPreWriteRecord == -1 )
            {
                TraceRecord = trc_cmp_forward( Trace.CurTraceRecord + 1, trc_POSTWRITEVAR );
                if( TraceRecord != -1 )
                {
                    Var = table_At( VARIABLE, &Trace.tTrace, TraceRecord + 1);
                    DiffRec = ( Var->RecordType != iType ) || ( Var->Reduct != Reduct );

                    if ( !DiffRec && (Var->vType != vType) )
                    {
                        if ( (Var->vType == rt_INT) && (vType == rt_LONG) )
                            DiffTyp = 1;
                        else
                            if ( (Var->vType == rt_LONG) && (vType == rt_INT) )
                                DiffTyp = 2;
                            else
                                DiffRec = 1; /* really different types, DiffTyp == 0  */
                            // TODO: rt_LLONG
                    }

                    if( !DiffRec )
                        Trace.CurTraceRecord = TraceRecord + 1;
                }
            }
        }

        if( DiffRec == 0 )
        {
            if( iType == trc_PREWRITEVAR && Reduct )
            {
                Trace.CurPreWriteRecord = Trace.CurTraceRecord;
                Trace.ReductExprType = 0;
            }

            if( ( !Reduct && iType != trc_PREWRITEVAR ) || iType == trc_REDUCTVAR )
            {
                if ( TraceOptions.EnableNANChecks && (vType == rt_FLOAT || vType == rt_DOUBLE) )
                {
                    if ( dbg_isNAN(Value, vType) )
                    {
                        error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NAN_VALUE);
                    }
                }

                /* check for different names */
                if( (Var->Line != Line) || (strNameCmp(Var->Operand, Operand))
                                        || (strFileCmp(Var->File, File)) )
                {
                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_INFO );
                    return ;
                }

                if( trc_CompareValue( &(Var->val), Value, vType, DiffTyp ) == 0 )
                {
                    nCheck = 1; /* Error status (just to avoid GOTO) */

                    if ( iType == trc_REDUCTVAR && TraceOptions.StrictCompare &&
                            TraceOptions.SubstRedResults && Trace.Mode == -1 )
                    {
                        int diff;

                        if ( TraceOptions.ExpIsAbsolute )
                            diff = trc_CompareValueAbsolute(&(Var->val), Value, vType, DiffTyp);
                        else
                            diff = trc_CompareValueRelative(&(Var->val), Value, vType, DiffTyp);

                        if ( diff != 0 )
                        { /* weak compare OK, substitute reduct. value */
                            trc_SubstituteRedVar(Value, Var, vType);
                            Trace.MatchedEvents++; /* let's count it as matched */
                            nCheck = 0;
                        } /* else continue - error notification */
                    }

                    if ( nCheck )
                    {
                        if ( !DiffTyp )
                        {
                            nCheck = 0;
                            switch( vType )
                            {
                                case rt_INT :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_INT_VAL : ERR_CMP_DIFF_INT_VAL),
                                        Operand, Var->val._int, *(int *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_LOGICAL :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_BOOL_VAL : ERR_CMP_DIFF_BOOL_VAL),
                                        Operand, Var->val._int, *(int *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_LONG :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_LONG_VAL : ERR_CMP_DIFF_LONG_VAL),
                                        Operand, Var->val._long, *(long *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_LLONG:
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_LLONG_VAL : ERR_CMP_DIFF_LLONG_VAL),
                                        Operand, Var->val._longlong, *(long long *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_FLOAT :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_FLOAT_VAL : ERR_CMP_DIFF_FLOAT_VAL),
                                        Operand, Trace.FloatPrecision, Var->val._float, Trace.FloatPrecision, *(float *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_DOUBLE :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_DBL_VAL : ERR_CMP_DIFF_DBL_VAL),
                                        Operand, Trace.DoublePrecision, Var->val._double, Trace.DoublePrecision, *(double *)Value );
                                    nCheck = 1;
                                    break;
                                case rt_FLOAT_COMPLEX :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ?
                                            ERR_CMP_DIFF_REDUCT_COMPLEX_FLOAT_VAL : ERR_CMP_DIFF_COMPLEX_FLOAT_VAL),
                                        Operand, Trace.FloatPrecision, Var->val._complex_float[0],
                                        Trace.FloatPrecision, Var->val._complex_float[1],
                                        Trace.FloatPrecision, *(float *)Value,
                                        Trace.FloatPrecision, *(((float *)Value) + 1));
                                    nCheck = 1;
                                    break;
                                case rt_DOUBLE_COMPLEX :
                                    error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                        (iType == trc_REDUCTVAR ?
                                            ERR_CMP_DIFF_REDUCT_COMPLEX_DBL_VAL : ERR_CMP_DIFF_COMPLEX_DBL_VAL),
                                        Operand, Trace.DoublePrecision, Var->val._complex_double[0],
                                        Trace.DoublePrecision, Var->val._complex_double[1],
                                        Trace.DoublePrecision, *(double *)Value,
                                        Trace.DoublePrecision, *(((double *)Value) + 1));
                                    nCheck = 1;
                                    break;
                            }

                            if ( nCheck && Trace.Mode == -1 )
                            {
                                ERR_INFO *pErrInfo;
                                dvm_ARRAY_INFO* pInfo = NULL;
                                DvmType lArrayNo;
                                int  old = 0;
                                double tmp1, tmp2;

                                nCheck = 0;

                                /* analyze & update tables with diff information */
                                if ( pArrBase != NULL ) /* check if it is array */
                                {
                                    lArrayNo = hash1_Find(&Trace.hArrayPointers, pArrBase);
                                    if (-1 != lArrayNo)
                                    {
                                        nCheck = 1;
                                        pInfo = table_At(dvm_ARRAY_INFO, &Trace.tArrays, lArrayNo);
                                        DBG_ASSERT(__FILE__, __LINE__, pInfo->tErrEntries.IsInit);

                                        for (DiffTyp = table_Count(&(pInfo->tErrEntries))-1; DiffTyp>=0; DiffTyp--)
                                        {
                                            pErrInfo = table_At(ERR_INFO, &(pInfo->tErrEntries), DiffTyp);

                                            if ( (pErrInfo->Line == Line) && (!strFileCmp(pErrInfo->File, File)) )
                                            /* the place for updating is found */
                                            {
                                                old = 1;
                                                break;
                                            }
                                        }

                                        if ( !old )
                                        {   /* create new entry in the table */
                                            pErrInfo = table_GetNew(ERR_INFO, &(pInfo->tErrEntries));
                                            memset(pErrInfo, 0, sizeof(struct tag_ERR_INFO));
                                            /* pErrInfo->Operand[0] = 0; */
                                        }

                                    }
                                }

                                if ( !nCheck )
                                {   /* old == 0 */
                                    char *paren;

                                    /* do not distinguish different array accesses */
                                    paren = strchr(Operand, '(');

                                    if ( paren != NULL )
                                         *paren=0;

                                    paren = strchr(Operand, '[');

                                    if ( paren != NULL )
                                         *paren=0;
                                    /* --------------------------------------------*/
                                    for (DiffTyp = table_Count(&Trace.tVarErrEntries)-1; DiffTyp>=0; DiffTyp--)
                                    {
                                        pErrInfo = table_At(ERR_INFO, &Trace.tVarErrEntries, DiffTyp);

                                        if ( (pErrInfo->Line == Line) && (!strcmp(pErrInfo->Operand, Operand))
                                                && (!strFileCmp(pErrInfo->File, File)) )
                                        /* the place for updating is found */
                                        {
                                            old = 1;
                                            break;
                                        }
                                    }

                                    if ( !old )
                                    {   /* create new entry in the table */
                                        pErrInfo = table_GetNew(ERR_INFO, &Trace.tVarErrEntries);
                                        memset(pErrInfo, 0, sizeof(struct tag_ERR_INFO));
                                        strncpy(pErrInfo->Operand, Operand, MaxOperand+1);
                                        pErrInfo->Operand[MaxOperand] = 0;
                                    }
                                }

                                if ( !old )
                                {   /* first time got error at this file/line/operand */
                                    /* integer fields are already initialized with zero */
                                    pErrInfo->Line = Line;
                                    strncpy(pErrInfo->File, File, MaxSourceFileName + 1);
                                    pErrInfo->File[MaxSourceFileName] = 0;
                                    pErrInfo->vtype = vType;

                                    /* initialize double fields */
                                    pErrInfo->maxRelAcc  = pErrInfo->maxRelAccAbs  = 0; /* in case of incompatible types */
                                    pErrInfo->maxAbsAcc  = pErrInfo->maxAbsAccRel  = 0;
                                    pErrInfo->maxLeapRel = pErrInfo->maxLeapRelAbs = 0;
                                    pErrInfo->maxLeapAbs = pErrInfo->maxLeapAbsRel = 0;

                                    switch( vType )
                                    {
                                        case rt_INT :
                                        case rt_LOGICAL :
                                            tmp1 = *((int *)Value);
                                            tmp2 = Var->val._int;
                                            pErrInfo->maxRelAcc = REAL_REL_VAL(double, tmp2, &tmp1);
                                            pErrInfo->maxAbsAcc = REAL_ABS_VAL(double, tmp2, &tmp1);
                                            break;
                                        case rt_FLOAT :
                                            pErrInfo->maxRelAcc = REAL_REL_VAL(float, Var->val._float, (float *)Value);
                                            pErrInfo->maxAbsAcc = REAL_ABS_VAL(float, Var->val._float, (float *)Value);
                                            break;
                                        case rt_DOUBLE :
                                            pErrInfo->maxRelAcc = REAL_REL_VAL(double, Var->val._double, (double *)Value);
                                            pErrInfo->maxAbsAcc = REAL_ABS_VAL(double, Var->val._double, (double *)Value);
                                            break;
                                        case rt_FLOAT_COMPLEX :
                                            pErrInfo->maxRelAcc = dvm_max(
                                                    REAL_REL_VAL(float, Var->val._complex_float[0], (float *)Value),
                                                    REAL_REL_VAL(float, Var->val._complex_float[1], ((float *)Value)+1) );
                                            pErrInfo->maxAbsAcc = dvm_max(
                                                    REAL_ABS_VAL(float, Var->val._complex_float[0], (float *)Value),
                                                    REAL_ABS_VAL(float, Var->val._complex_float[1], ((float *)Value)+1) );
                                        case rt_DOUBLE_COMPLEX :
                                            pErrInfo->maxRelAcc = dvm_max(
                                                    REAL_REL_VAL(double, Var->val._complex_double[0], (double *)Value),
                                                    REAL_REL_VAL(double, Var->val._complex_double[1], ((double *)Value)+1) );
                                            pErrInfo->maxAbsAcc = dvm_max(
                                                    REAL_ABS_VAL(double, Var->val._complex_double[0], (double *)Value),
                                                    REAL_ABS_VAL(double, Var->val._complex_double[1], ((double *)Value)+1) );
                                    }

                                    pErrInfo->firstHitValT = Var->val;
                                    trc_StoreValue(&(pErrInfo->firstHitValE), Value, vType);
                                    pErrInfo->firstHitRel  = pErrInfo->maxRelAcc;
                                    pErrInfo->firstHitAbs  = pErrInfo->maxAbsAcc;
                                    pErrInfo->firstHitLoc  = Var->Line_num;
                                    pErrInfo->firstHitLocC = Var->CPU_Num;
                                    pErrInfo->HitCount++;

                                    if ( pErrInfo->maxRelAcc > TraceOptions.Exp )
                                    {
                                        /* fill in max Acc. fields */
                                         pErrInfo->maxRelAccAbs = pErrInfo->maxAbsAcc;
                                         pErrInfo->maxRelAccLoc = Var->Line_num;
                                         pErrInfo->maxRelAccLocC= Var->CPU_Num;
                                         pErrInfo->maxRelAccValT= Var->val;
                                         trc_StoreValue(&(pErrInfo->maxRelAccValE), Value, vType);

                                        /* fill in Leap fields */
                                        /* pErrInfo->maxLeapRel    = ;
                                         pErrInfo->maxLeapRelAbs = ;*/
                                         pErrInfo->maxLeapRLoc = Var->Line_num;
                                         pErrInfo->maxLeapRLocC= Var->CPU_Num;
                                         pErrInfo->LeapsRCount++;
                                         pErrInfo->maxLeapRelValT = Var->val;
                                         trc_StoreValue(&(pErrInfo->maxLeapRelValE), Value, vType);
                                    }

                                    if ( pErrInfo->maxAbsAcc > TraceOptions.Exp )
                                    {
                                        /* fill in max Acc. fields */
                                         pErrInfo->maxAbsAccRel = pErrInfo->maxRelAcc;
                                         pErrInfo->maxAbsAccLoc = Var->Line_num;
                                         pErrInfo->maxAbsAccLocC= Var->CPU_Num;
                                         pErrInfo->maxAbsAccValT= Var->val;
                                         trc_StoreValue(&(pErrInfo->maxAbsAccValE), Value, vType);

                                        /* fill in Leap fields */
                                        /* pErrInfo->maxLeapAbs  = ;
                                         pErrInfo->maxLeapAbsRel = ; */
                                         pErrInfo->maxLeapALoc = Var->Line_num;
                                         pErrInfo->maxLeapALocC= Var->CPU_Num;
                                         pErrInfo->LeapsACount++;
                                         pErrInfo->maxLeapAbsValT = Var->val;
                                         trc_StoreValue(&(pErrInfo->maxLeapAbsValE), Value, vType);
                                    }
                                }
                                else
                                {   /* not first time difference */
                                    double maxRelAcc, maxAbsAcc, leap;

                                    switch( vType )
                                    {
                                        case rt_INT :
                                        case rt_LOGICAL :
                                            tmp1 = *((int *)Value);
                                            tmp2 = Var->val._int;
                                            maxRelAcc = REAL_REL_VAL(double, tmp2, &tmp1);
                                            maxAbsAcc = REAL_ABS_VAL(double, tmp2, &tmp1);
                                            break;
                                        case rt_FLOAT :
                                            maxRelAcc = REAL_REL_VAL(float, Var->val._float, (float *)Value);
                                            maxAbsAcc = REAL_ABS_VAL(float, Var->val._float, (float *)Value);
                                            break;
                                        case rt_DOUBLE :
                                            maxRelAcc = REAL_REL_VAL(double, Var->val._double, (double *)Value);
                                            maxAbsAcc = REAL_ABS_VAL(double, Var->val._double, (double *)Value);
                                            break;
                                        case rt_FLOAT_COMPLEX :
                                            maxRelAcc = dvm_max(
                                                    REAL_REL_VAL(float, Var->val._complex_float[0], (float *)Value),
                                                    REAL_REL_VAL(float, Var->val._complex_float[1], ((float *)Value)+1) );
                                            maxAbsAcc = dvm_max(
                                                    REAL_ABS_VAL(float, Var->val._complex_float[0], (float *)Value),
                                                    REAL_ABS_VAL(float, Var->val._complex_float[1], ((float *)Value)+1) );
                                        case rt_DOUBLE_COMPLEX :
                                            maxRelAcc = dvm_max(
                                                    REAL_REL_VAL(double, Var->val._complex_double[0], (double *)Value),
                                                    REAL_REL_VAL(double, Var->val._complex_double[1], ((double *)Value)+1) );
                                            maxAbsAcc = dvm_max(
                                                    REAL_ABS_VAL(double, Var->val._complex_double[0], (double *)Value),
                                                    REAL_ABS_VAL(double, Var->val._complex_double[1], ((double *)Value)+1) );
                                    }

                                    pErrInfo->HitCount++;

                                    if ( maxRelAcc > TraceOptions.Exp )
                                    {
                                         if ( pErrInfo->LeapsRCount == 0 )
                                         {   /* first leap, fill in Leap fields */
                                             /* do not change zero leap value */
                                             /* pErrInfo->maxLeapRel    = ;
                                             pErrInfo->maxLeapRelAbs = ;*/
                                             pErrInfo->maxLeapRLoc = Var->Line_num;
                                             pErrInfo->maxLeapRLocC= Var->CPU_Num;
                                             pErrInfo->LeapsRCount++;
                                             pErrInfo->maxLeapRelValT = Var->val;
                                             trc_StoreValue(&(pErrInfo->maxLeapRelValE), Value, vType);

                                             /* fill in max Acc. fields for the first time */
                                             pErrInfo->maxRelAcc    = maxRelAcc;
                                             pErrInfo->maxRelAccAbs = maxAbsAcc;
                                             pErrInfo->maxRelAccLoc = Var->Line_num;
                                             pErrInfo->maxRelAccLocC= Var->CPU_Num;
                                             pErrInfo->maxRelAccValT   = Var->val;
                                             trc_StoreValue(&(pErrInfo->maxRelAccValE), Value, vType);
                                         }
                                         else
                                         {
                                             if ( maxRelAcc > pErrInfo->maxRelAcc )
                                             {
                                                  /* this is not first hit */
                                                  /* check if it is leap */
                                                  leap = maxRelAcc - pErrInfo->maxRelAcc;  // dvm_abs is not necessary
                                                  if ( leap > pErrInfo->maxLeapRel )
                                                  {
                                                       pErrInfo->maxLeapRel    = leap;
                                                       pErrInfo->maxLeapRelAbs = dvm_abs(maxAbsAcc - pErrInfo->maxAbsAcc);
                                                       pErrInfo->maxLeapRLoc = Var->Line_num;
                                                       pErrInfo->maxLeapRLocC= Var->CPU_Num;
                                                       pErrInfo->LeapsRCount++;
                                                       pErrInfo->maxLeapRelValT = Var->val;
                                                       trc_StoreValue(&(pErrInfo->maxLeapRelValE), Value, vType);
                                                  }

                                                  /* substitute max. value */
                                                  pErrInfo->maxRelAcc    = maxRelAcc;
                                                  pErrInfo->maxRelAccAbs = maxAbsAcc;
                                                  pErrInfo->maxRelAccLoc = Var->Line_num;
                                                  pErrInfo->maxRelAccLocC= Var->CPU_Num;
                                                  pErrInfo->maxRelAccValT   = Var->val;
                                                  trc_StoreValue(&(pErrInfo->maxRelAccValE), Value, vType);
                                             }
                                         }
                                    }

                                    if ( maxAbsAcc > TraceOptions.Exp )
                                    {
                                         if ( pErrInfo->LeapsACount == 0 )
                                         {   /* first leap, fill in Leap fields */
                                             /* do not change zero leap value */
                                             /* pErrInfo->maxLeapAbs    = ;
                                             pErrInfo->maxLeapAbsAbs = ;*/
                                             pErrInfo->maxLeapALoc = Var->Line_num;
                                             pErrInfo->maxLeapALocC= Var->CPU_Num;
                                             pErrInfo->LeapsACount++;
                                             pErrInfo->maxLeapAbsValT = Var->val;
                                             trc_StoreValue(&(pErrInfo->maxLeapAbsValE), Value, vType);

                                             /* fill in max Acc. fields for the first time */
                                             pErrInfo->maxAbsAcc    = maxAbsAcc;
                                             pErrInfo->maxAbsAccRel = maxRelAcc;
                                             pErrInfo->maxAbsAccLoc = Var->Line_num;
                                             pErrInfo->maxAbsAccLocC= Var->CPU_Num;
                                             pErrInfo->maxAbsAccValT   = Var->val;
                                             trc_StoreValue(&(pErrInfo->maxAbsAccValE), Value, vType);
                                         }
                                         else
                                         {
                                             if ( maxAbsAcc > pErrInfo->maxAbsAcc )
                                             {
                                                  /* this is not first hit */
                                                  /* check if it is leap */
                                                  leap = maxAbsAcc - pErrInfo->maxAbsAcc; // dvm_abs is not necessary
                                                  if ( leap > pErrInfo->maxLeapAbs )
                                                  {
                                                       pErrInfo->maxLeapAbs    = leap;
                                                       pErrInfo->maxLeapAbsRel = dvm_abs(maxRelAcc - pErrInfo->maxRelAcc);
                                                       pErrInfo->maxLeapALoc = Var->Line_num;
                                                       pErrInfo->maxLeapALocC= Var->CPU_Num;
                                                       pErrInfo->LeapsACount++;
                                                       pErrInfo->maxLeapAbsValT = Var->val;
                                                       trc_StoreValue(&(pErrInfo->maxLeapAbsValE), Value, vType);
                                                  }

                                                  /* substitute max. value */
                                                  pErrInfo->maxAbsAcc    = maxAbsAcc;
                                                  pErrInfo->maxAbsAccRel = maxRelAcc;
                                                  pErrInfo->maxAbsAccLoc = Var->Line_num;
                                                  pErrInfo->maxAbsAccLocC= Var->CPU_Num;
                                                  pErrInfo->maxAbsAccValT= Var->val;
                                                  trc_StoreValue(&(pErrInfo->maxAbsAccValE), Value, vType);
                                             }
                                         }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if ( DiffTyp == 1 )
                                 /* int & long */
                                error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                    (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_LONG_VAL : ERR_CMP_DIFF_LONG_VAL),
                                    Operand, (DvmType)Var->val._int, *(DvmType *)Value );
                            else /* long & int */
                                error_CmpTraceExt(Trace.CurTraceRecord, File, Line,
                                    (iType == trc_REDUCTVAR ? ERR_CMP_DIFF_REDUCT_LONG_VAL : ERR_CMP_DIFF_LONG_VAL),
                                    Operand, Var->val._long, (DvmType)*((int *)Value) );
                            pprintf(3, "*** RTS warning: CMPTRACE: DIFFERENT TYPES COMPARISON IS NOT IMPLEMENTED IN THE PROTOCOL!");
                        }
                    }
                }
                else
                {
                    Trace.MatchedEvents++;

                    if ( !TraceOptions.StrictCompare && Trace.Mode == -1 ) {
                        if ( TraceOptions.SubstRedResults && iType == trc_REDUCTVAR ) {
                            trc_SubstituteRedVar(Value, Var, vType);
                        } else if ( TraceOptions.SubstAllResults && iType == trc_POSTWRITEVAR ) {
                            trc_SubstituteCommonVar(Value, Var, vType);
                        }
                    }
                }
            }
        }
        else if( Trace.CurPreWriteRecord == -1 )
        {
            error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_INFO );
        }

        if( iType == trc_POSTWRITEVAR )
            Trace.CurPreWriteRecord = -1;

        if( DiffRec == 0 || Trace.CurPreWriteRecord == -1 )
            Trace.CurTraceRecord++;
    }
    else
    {
        error_CmpTraceExt(-1, File, Line, ERR_CMP_NO_INFO );
    }
}

void trc_cmp_skip(char *File, UDvmType Line)
{
    ANY_RECORD*     Rec;
    SKIP*           Skip2;
    DvmType            TraceRecord;

    if( !trc_InfoCanTrace(Trace.pCurCnfgInfo, trc_SKIP) )
        return;

    if (Trace.pCurCnfgInfo && !Trace.IterFlash)
    {
        if( !trc_cmp_iteration_flash() )
        {
            /* "skip" from absent iteration */
            /* error_CmpTraceExt( Trace.CurStruct, DVM_FILE[0], DVM_LINE[0], ERR_CMP_NO_SKIP ); */
            return ;
        }
    }

    if( Trace.CurTraceRecord < table_Count(&Trace.tTrace) )
    {
        Rec = table_At( ANY_RECORD, &Trace.tTrace, Trace.CurTraceRecord );

        if( Rec->RecordType != trc_SKIP )
        {
            TraceRecord = -1;
            if( Rec->RecordType == trc_PREWRITEVAR )
                TraceRecord = trc_cmp_forward( Trace.CurTraceRecord, trc_SKIP );

            if( TraceRecord == -1 )
            {
                error_CmpTraceExt(Trace.CurTraceRecord, File, Line, ERR_CMP_NO_TRACE );
                return;
            }
            Trace.CurTraceRecord = TraceRecord;
        }

        if ( (Trace.Mode == 0) /* trace accumulation compare mode */
            && (hash1_Find(&Trace.hSkippedPointers, Trace.CurTraceRecord) != -1) )
                /* it is the last record of skipped data */
        {
            Rec = table_GetBack(ANY_RECORD, &Trace.tSkipped );

            if ( Rec->RecordType != trc_SKIP ) /* there is no written SKIP record */
            {
                Skip2 = table_GetNew(SKIP, &Trace.tSkipped);
                Skip2->RecordType = trc_SKIP;
            }
        }

        // Skip = table_At( SKIP, &Trace.tTrace, Trace.CurTraceRecord );

        Trace.CurTraceRecord ++;
    }
    else
        error_CmpTraceExt(-1, File, Line, ERR_CMP_NO_TRACE );
}

DvmType trc_cmp_forward(DvmType CurTraceRecord, enum_TraceType iType)
{
    ANY_RECORD *Rec;

    while( CurTraceRecord < table_Count( &Trace.tTrace ) )
    {
        Rec = table_At( ANY_RECORD, &Trace.tTrace, CurTraceRecord );

        if( Rec->RecordType == iType )
            return CurTraceRecord;

        switch( Rec->RecordType )
        {
            case trc_PREWRITEVAR :
            case trc_POSTWRITEVAR :
            case trc_READVAR :
            case trc_REDUCTVAR :
                break;
            default : return -1;
        }

        CurTraceRecord++;
    }

    return -1;
}

#endif /* _TRC_CMP_C_ */
