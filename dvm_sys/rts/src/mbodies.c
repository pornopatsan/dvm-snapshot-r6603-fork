
#ifndef _MBODIES_C_
#define _MBODIES_C_

#include <stdlib.h>

int divide_surface (s_PARLOOP *PL, LOOP_INFO* loop);
int divide_nodes (s_PARLOOP *PL, LOOP_INFO* loop);
int divide_compare (s_PARLOOP *PL, LOOP_INFO* loop);

void __callstd  dvtr_ (int *VTR, int *conv_mode)
{
    if ( VTR != NULL && conv_mode != NULL)
    {
        Trace.convMode = *conv_mode;

        if ( Trace.convMode == 0 )
        {
            int i;

            mac_malloc(Trace.tmpPL, s_PARLOOP *, sizeof(s_PARLOOP), 0);
            dvm_AllocArray(s_LOOPMAPINFO, MAXARRAYDIM, Trace.tmpPL->MapList);
            dvm_AllocArray(s_REGULARSET, MAXARRAYDIM, Trace.tmpPL->Local);
            for(i=0; i < MAXARRAYDIM; i++)
                Trace.tmpPL->InitIndex[i] = 0;
            Trace.tmpPL->HasLocal = 0;
        }
        else
            DBG_ASSERT(__FILE__, __LINE__, Trace.convMode == 1);

        switch ( TraceOptions.IterTraceMode )
        {
              case 0:   *VTR = 0; Trace.vtr = NULL; break;
              case -1:  TraceOptions.IterTraceMode = 1; /* proceed to case 1: */
              case 1:
                        if ( TraceOptions.LocIterWidth <= 0 && TraceOptions.RepIterWidth <= 0  )
                        {
                            *VTR = 0; Trace.vtr = NULL;
                        }
                        else
                        {
                            *VTR = 1; Trace.vtr = VTR;
                            if ( EnableTrace && (TraceOptions.TraceMode == mode_COMPARETRACE) )
                                 divide_set = divide_compare;
                            else divide_set = divide_nodes;
                        }
                        break;
              case 2:
                        if ( TraceOptions.LocIterWidth <= 0 && TraceOptions.RepIterWidth <= 0  )
                        {
                            *VTR = 0; Trace.vtr = NULL;
                        }
                        else
                        {
                            *VTR = 1; Trace.vtr = VTR;
                            if ( EnableTrace && (TraceOptions.TraceMode == mode_COMPARETRACE) )
                                 divide_set = divide_compare;
                            else divide_set = divide_surface;
                        }
                        break;
              case 3:  *VTR = 1; Trace.vtr = NULL; break;

              case 4:  /* special temporary case !!!! */
                        if ( TraceOptions.LocIterWidth <= 0 && TraceOptions.RepIterWidth <= 0  )
                        {
                            *VTR = 0; Trace.vtr = NULL;
                        }
                        else
                        {
                            *VTR = 1; Trace.vtr = VTR;
                            divide_set = divide_surface;
                        }
                        break;
              default:
                        EnableTrace = 0;
                        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                "*** RTS err: CMPTRACE: Invalid value: TraceOptions.IterTraceMode = %d.\n", TraceOptions.IterTraceMode);
        }

        if ( *VTR && (TraceOptions.StartPoint[0] != 0 || TraceOptions.FinishPoint[0] != 0) )
        { /* Should not run VTR mode with number limitation (incompatible) */
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** RTS err: CMPTRACE: Can not run VTR mode with number limitations. Choose only one method.\n");
        }
    }
    else
    {
        EnableTrace = 0;
        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                "*** RTS err: CMPTRACE: Invalid call to dvtr_: NULL argument.\n");
    }

    return ;
}


DvmType  __callstd  drgar_(DvmType  *plRank, DvmType  *plTypePtr, DvmType  *pHandle,
                        DvmType  *pSize, int flag, char  *szOperand,
                        DvmType  lOperLength)
{
    if ( flag == 1 )
    {
        /* We must inititalize the array with zeros here */
    }

    return drarr_(plRank, plTypePtr, pHandle, pSize, szOperand, lOperLength);
}

DvmType __callstd dosl_(DvmType *No, DvmType* Init, DvmType* Last, DvmType* Step)
{
    DvmType l1, l2, s;
    LOOP_INFO *loop = NULL;

#ifdef DOSL_TRACE

    char buffer[999];


    sprintf(buffer, "\ncall dosl_, loop %ld\n", *No);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

    sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));


#endif

    if ( !isEmpty(&Trace.sLoopsInfo) )
    {
        loop = stackLast(&Trace.sLoopsInfo);

        while ( loop->No != *No )
        {   /* the loop has been exited using goto... we need to correct this !! */
            UDvmType tmp = loop->Line;

            pprintf(3, "*** RTS err: CMPTRACE: Incorrect DOSL call for loop %ld (File=%s, Line=%ld) instead of "
                       "loop %ld (Line=%ld).\n", *No, DVM_FILE[0], DVM_LINE[0], loop->No, loop->Line);

            DvmType res=-1; lexit_(&res);  // dead code after here

            DBG_ASSERT(__FILE__, __LINE__, 0);

            //DBG_ASSERT(__FILE__, __LINE__, !isEmpty (&Trace.sLoopsInfo) ); this is performed in stackLast function
            dendl_(&loop->No, &tmp); /* necessary stack changes will be automatically produced */
            loop = stackLast(&Trace.sLoopsInfo);
        }
        //DBG_ASSERT(__FILE__, __LINE__, loop->No == *No );

        if ( (Trace.vtr == NULL) || (Trace.sLoopsInfo.top - 1 != Trace.ctrlIndex) ||
                 (TraceOptions.RepIterWidth == -1) || !Trace.EnableLoopsPartitioning ||
                 (TraceOptions.SeqLdivParContextOnly && !loop->Propagate) )
                /* the loop is NOT controlled by VTR */
        {

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", !loop->Init, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

            if ( !loop->Init  )
            {
                loop->Init = 1;    /* next time - finish the loop */
                return 1;
            }
            else
                return 0;
        }

        /* the loop is controlled by VTR */

        if ( !loop->Init )
        {
            loop->DoplRes.Set[0].Lower = *Init;

            if ( TraceOptions.RepIterWidth == 0 )
            {
                loop->Init = 3;
                *Trace.vtr = 0;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 1, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

                return 1;
            }

            if ( 2*TraceOptions.RepIterWidth*dvm_abs(*Step) >= dvm_abs(*Last - *Init) + 1 )
            {
                loop->Init = 3;
                *Trace.vtr = 1;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 1, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

                return 1;
            }

            loop->DoplRes.Set[0].Upper = *Last;

            if ( *Init <= *Last && *Step > 0 )
            {
                /* the left hand side of loop is being created */

                loop->LocHandle = 0; /* the loop is not inversed */

                /* no changes to the Init parameter */
                *Last = *Init + *Step*(TraceOptions.RepIterWidth - 1);
            }
            else
            {
               /* the right hand side of loop is being created */

                if ( *Init >= *Last && *Step < 0 )
                {
                    loop->LocHandle = 1; /* the loop is inversed */

                    /* no changes to the Init parameter */
                    *Last = *Init + *Step*(TraceOptions.RepIterWidth - 1);
                }
                else
                {
                    /* dummy loop: the first iteration is greater than the last one */
                    *Trace.vtr = 1;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 0, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

                    return 0;
                }
            }

            loop->Init = 1;
            *Trace.vtr = 1;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 1, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

            return 1;
        }
        else
        {
            if ( loop->Init == 3 )
            {
                *Trace.vtr = 1;
                /* Restore the iteration limits to their original values (just in case).
                   *Last contains the correct value already */
                *Init = loop->DoplRes.Set[0].Lower;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 0, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif
                return 0;
            }

            if ( loop->Init == 1 )
            {
                l1 = loop->DoplRes.Set[0].Lower;

                *Init = l1 + *Step * TraceOptions.RepIterWidth;

                *Last = l1 + *Step*( (loop->DoplRes.Set[0].Upper - l1) / *Step - TraceOptions.RepIterWidth );

                loop->Init++;
                *Trace.vtr = 0;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 1, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

                return 1;
            }

            if ( loop->Init == 2 )
            {
                s = loop->DoplRes.Set[0].Lower;

                l1 = s + *Step * TraceOptions.RepIterWidth;

                l2 = s + *Step*( (loop->DoplRes.Set[0].Upper - s) / *Step - TraceOptions.RepIterWidth + 1);

                *Init = (loop->LocHandle == 0)?(dvm_max(l1, l2)):(dvm_min(l1, l2));
                *Last = loop->DoplRes.Set[0].Upper;

                loop->Init++;
                *Trace.vtr = 1;

#ifdef DOSL_TRACE
            sprintf(buffer, "\nret  dosl_ = %ld, loop %ld, SRCLine=%ld\n", 1, *No, __LINE__);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));

            sprintf(buffer, "\tInit = %ld, Last = %ld, Step = %ld\n", *Init, *Last, *Step);
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

                return 1;
            }

            DBG_ASSERT(__FILE__, __LINE__, 0 );
        }
    }

    /* this code must be unreachable */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}

#ifndef NO_DOPL_DOPLMB_TRACE
#define RETURN(par)\
{\
    if ( PL->HasLocal && PL->Local )\
    {\
            sprintf(buffer, "\nret  doplmb_ = %ld, loop %ld, SRCLine=%ld, VTR=%d\n", par, *No, __LINE__, Trace.vtr?*Trace.vtr:-1);\
            SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));\
\
            for( i=0; i < PL->Rank; i++ )\
            {\
                sprintf(buffer, "\tloop[%d] Init = %ld, Last = %ld, Step = %ld\n", i, *(PL->MapList[i].InitIndexPtr), *(PL->MapList[i].LastIndexPtr), *(PL->MapList[i].StepPtr));\
                SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));\
            }\
    }\
    else SYSTEM(fputs, ("\nret  doplmb_: NO LOCAL PART\n", Trace.DoplMBFileHandle));\
}
#else
#define RETURN(par) ;
#endif

DvmType __callstd doplmb_(LoopRef  *LoopRefPtr, DvmType *No)
{
    DvmType lRes;
    int i;
    LOOP_INFO *loop = NULL;
    DvmType sz;
    byte inversed=0;
    s_PARLOOP      *PL;
    static DvmType SIZE = 0;
    DvmType t_sz;
#ifndef NO_DOPL_DOPLMB_TRACE
    char buffer[999];
#endif

    /* incorrect comparison may happen otherwise */
    if ( (Trace.EnableDbgTracingCtrl && !EnableTrace) || !Trace.EnableLoopsPartitioning )
         return dopl_(LoopRefPtr);

#ifndef NO_DOPL_DOPLMB_TRACE
    sprintf(buffer, "\ncall doplmb_, loop %ld, VTR = %d\n", *No, Trace.vtr?*Trace.vtr:-1);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

    PL = (s_PARLOOP *)((SysHandle *) *LoopRefPtr )->pP; /* the correctness is already checked ???*/

    if ( !isEmpty(&Trace.sLoopsInfo) )
    {
        loop = stackLast(&Trace.sLoopsInfo);

        while ( loop->No != *No )
        {   /* the loop has been exited using goto... we need to correct this !! */
            UDvmType tmp = loop->Line;

            pprintf(3, "*** RTS err: CMPTRACE: Incorrect DOPLMB call for loop %ld (File=%s, Line=%ld) instead of "
                       "loop %ld (Line=%ld).\n", *No, DVM_FILE[0], DVM_LINE[0], loop->No, loop->Line);
            DBG_ASSERT(__FILE__, __LINE__, 0);

            //DBG_ASSERT(__FILE__, __LINE__, !isEmpty (&Trace.sLoopsInfo) ); this is performed in stackLast function
            dendl_(&loop->No, &tmp); /* necessary stack changes will be automatically produced */
            loop = stackLast(&Trace.sLoopsInfo);
        }
        //DBG_ASSERT(__FILE__, __LINE__, loop->No == *No );

        if ( (Trace.vtr == NULL) || (Trace.sLoopsInfo.top - 1 != Trace.ctrlIndex) || (TraceOptions.LocIterWidth == -1) )
        {
            lRes = dopl_(LoopRefPtr);
            RETURN( lRes )
            return lRes;
            /* do not create any chunk record because this loop is not gonna be traced */
        }


        if ( loop->HasItems )
        {
            if ( divide_set(PL, loop) != 0 )
            {
                RETURN(1)

                /* SIZE checking feature */
#ifndef NO_SIZE_CHECK
                t_sz = 1;
                for( i=0; i < PL->Rank && t_sz > 0; i++ )
                {
                        if ( PL->Invers[i] )
                        {
                            t_sz *= *(PL->MapList[i].InitIndexPtr) - *(PL->MapList[i].LastIndexPtr) + 1;
                        }
                        else
                        {
                            t_sz *= *(PL->MapList[i].LastIndexPtr) - *(PL->MapList[i].InitIndexPtr) + 1;
                        }
                }

                DBG_ASSERT(__FILE__, __LINE__, t_sz > 0 );

                SIZE -= t_sz;

                DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) >= 0 );
#endif
               /* generate chunk record */
               if ( EnableTrace && TraceOptions.TraceMode != mode_COMPARETRACE )
               /* we should not save anything in compare mode */
                   trc_put_chunk(PL); /* all correctness checks are inside the function */

#ifdef DONT_TRACE_NECESSARY_CHECK
               if ( EnableTrace && TraceOptions.TraceMode == mode_COMPARETRACE
                    && !*Trace.vtr ) /* do not trace (vtr == off) condition */
               {
                   DvmType index[MAXARRAYDIM];
                   dont_trace_necessary_check(0, index, PL);
               }
#endif
               return 1;
            }
            DBG_ASSERT(__FILE__, __LINE__, loop->HasItems == 0 );
        }

        while ( 1 )
        {

/* SIZE must be equal zero since the last loop entrance */
#ifndef NO_SIZE_CHECK
DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) == 0 );
#endif
            lRes = dopl_(LoopRefPtr);

            if ( lRes == 0 )
            {
                /* finish the work on this loop */
                if (loop->Init)
                {
                     loop->Init = 0;
                     if (loop->LocParts)
                         dvm_FreeArray(loop->LocParts);
                }

                *Trace.vtr = 1;

                RETURN(0)

                return 0;
            }

/* calculate new SIZE */
#ifndef NO_SIZE_CHECK
            SIZE = 1;

            for( i=0; i < PL->Rank && SIZE > 0; i++ )
            {
                    if ( PL->Invers[i] )
                    {
                        SIZE *= *(PL->MapList[i].InitIndexPtr) - *(PL->MapList[i].LastIndexPtr) + 1;
                    }
                    else
                    {
                        SIZE *= *(PL->MapList[i].LastIndexPtr) - *(PL->MapList[i].InitIndexPtr) + 1;
                    }
            }
            if ( SIZE < 0 ) SIZE = 0;
#endif

            i = divide_set(PL, loop);
            if ( i == 0 )
            {
#ifndef NO_SIZE_CHECK
            DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) == 0 );
#endif
                continue; /* it is possible that dopl_ return 1, but the number of iterations to execute is zero */
            }

#ifndef NO_SIZE_CHECK
            DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) > 0);
#endif

            RETURN(1)

            /* SIZE checking feature */
#ifndef NO_SIZE_CHECK
            t_sz = 1;
            for( i=0; i < PL->Rank && t_sz > 0; i++ )
            {
                    if ( PL->Invers[i] )
                    {
                         t_sz *= *(PL->MapList[i].InitIndexPtr) - *(PL->MapList[i].LastIndexPtr) + 1;
                    }
                    else
                    {
                         t_sz *= *(PL->MapList[i].LastIndexPtr) - *(PL->MapList[i].InitIndexPtr) + 1;
                    }
            }

            DBG_ASSERT(__FILE__, __LINE__, t_sz > 0);
            SIZE -= t_sz;
            DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) >= 0 );
#endif

            /* generate chunk record */
            if ( EnableTrace && TraceOptions.TraceMode != mode_COMPARETRACE )
            /* we should not save anything in compare mode */
                trc_put_chunk(PL); /* all correctness checks are inside the function */

#ifdef DONT_TRACE_NECESSARY_CHECK
               if ( EnableTrace && TraceOptions.TraceMode == mode_COMPARETRACE
                    && !*Trace.vtr ) /* do not trace (vtr == off) condition */
               {
                   DvmType index[MAXARRAYDIM];
                   dont_trace_necessary_check(0, index, PL);
               }
#endif

            return 1;
        }
    }

    /* this code must be unreachable */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}


/*
 * Before returning 0 should set HasItems and LocHandle to zero
 *
 * Before returning 1 should set VTR to the required value
 *
 * */
int divide_surface (s_PARLOOP *PL, LOOP_INFO* loop)
{
    s_BLOCK        block;
    int            i, Counter, dim, cur_no, found;
    DvmType           Lower, Step, Lower1, Lower2;

    if ( !loop->Init )
    {
        /* we need to save the Local part information in the Global Iterations format in the internal structures
         * in blocks ordered by the calculations direction (ACROSS scheme must work) */
        s_REGULARSET LocalInGlobalInd[MAXARRAYDIM];

        loop->LocHandle = 0;

        dvm_AllocArray(s_BLOCK, PL->Rank*2 + 1, loop->LocParts);

        DBG_ASSERT(__FILE__, __LINE__, PL->Local && PL->HasLocal );

        /* translate loop local part into global coordinates */
        for(i=0; i < PL->Rank; i++)
        {
            LocalInGlobalInd[i].Lower = PL->Local[i].Lower + PL->InitIndex[i];
            LocalInGlobalInd[i].Upper = PL->Local[i].Upper + PL->InitIndex[i];
            LocalInGlobalInd[i].Step =  PL->Local[i].Step;

            Lower = LocalInGlobalInd[i].Upper-LocalInGlobalInd[i].Lower+1;
            Step  = dvm_abs(LocalInGlobalInd[i].Step);

            if ( 2*TraceOptions.LocIterWidth*Step >= Lower )
            {
                *Trace.vtr = 1;
                dvm_FreeArray(loop->LocParts);

                if ( Lower <= 0 ) /* empty loop */
                    return 0;

                /* we need to identify if there are any elements in the local part */
                /* in order not to save in the trace dummy chunks */
                for (Counter=i+1; Counter < PL->Rank; Counter++)
                {
                    if ( PL->Local[Counter].Upper - PL->Local[Counter].Lower < 0 )
                        return 0;
                }

                /* now we need to know if the dopl_ result is empty */
                for( Counter=0; Counter < PL->Rank; Counter++ )
                {
                    if ( PL->Invers[Counter] )
                    {
                        if (*(PL->MapList[Counter].InitIndexPtr) -
                                *(PL->MapList[Counter].LastIndexPtr) + 1 <= 0)
                                return 0;
                    }
                    else
                    {
                        if (*(PL->MapList[Counter].LastIndexPtr) <
                                *(PL->MapList[Counter].InitIndexPtr) )
                                return 0;
                    }
                }

                return 1;
                /* HasItems is equal zero here, that is why we will not get into infinite loop here
                 * there is no need to care of the restoring the initial iteration borders because it is the
                 * first call to divide_set for the loop */
            }
        }

        Counter = cur_no = 0;

        while ( Counter < 2*PL->Rank )
        {
            loop->LocParts[cur_no].Rank = PL->Rank;

            if ( Counter == PL->Rank && Counter == cur_no )
            {   /* the half of blocks is created */
                /* the internal part of the loop is being created */

                for(i=0; i < PL->Rank; i++)
                {
                    Step = loop->LocParts[cur_no].Set[i].Step = LocalInGlobalInd[i].Step;
                    Lower = LocalInGlobalInd[i].Lower;

                    loop->LocParts[cur_no].Set[i].Lower = Lower + Step * TraceOptions.LocIterWidth;
                    loop->LocParts[cur_no].Set[i].Upper =
                            Lower + Step * ( (LocalInGlobalInd[i].Upper - Lower) / Step - TraceOptions.LocIterWidth);

                    loop->LocParts[cur_no].Set[i].Size =
                            loop->LocParts[cur_no].Set[i].Upper - loop->LocParts[cur_no].Set[i].Lower + 1;

                    DBG_ASSERT(__FILE__, __LINE__, loop->LocParts[cur_no].Set[i].Size > 0);
                }

                cur_no++;
                continue;  /* do not update the Counter, continue to build structures */
            }

            /* set the order of parts of loop */
            if ( Counter < PL->Rank )
                dim = Counter;
            else
                dim = 2*PL->Rank - 1 - Counter;


            /* all dims < dim are processed with limitations (central part like) */

            for(i=0; i < dim; i++)
            {
                Step = loop->LocParts[cur_no].Set[i].Step = LocalInGlobalInd[i].Step;

                if ( !PL->Invers[i] )
                {
                    Lower = LocalInGlobalInd[i].Lower;

                    loop->LocParts[cur_no].Set[i].Lower = Lower + Step * TraceOptions.LocIterWidth;
                    loop->LocParts[cur_no].Set[i].Upper =
                        Lower + Step * ( (LocalInGlobalInd[i].Upper - Lower) / Step - TraceOptions.LocIterWidth);
                }
                else
                {
                    loop->LocParts[cur_no].Set[i].Lower = LocalInGlobalInd[i].Upper -
                        Step * ( (LocalInGlobalInd[i].Upper - LocalInGlobalInd[i].Lower) / Step - TraceOptions.LocIterWidth);

                    loop->LocParts[cur_no].Set[i].Upper = LocalInGlobalInd[i].Upper - Step * TraceOptions.LocIterWidth;
                }

                loop->LocParts[cur_no].Set[i].Size =
                    loop->LocParts[cur_no].Set[i].Upper - loop->LocParts[cur_no].Set[i].Lower + 1;

            }

            /* all dims >= dim+1 are processed fully */

            for(i=dim+1; i < PL->Rank; i++)
            {
                loop->LocParts[cur_no].Set[i].Step = LocalInGlobalInd[i].Step;
                loop->LocParts[cur_no].Set[i].Lower = LocalInGlobalInd[i].Lower;
                loop->LocParts[cur_no].Set[i].Upper = LocalInGlobalInd[i].Upper;
                loop->LocParts[cur_no].Set[i].Size =
                    loop->LocParts[cur_no].Set[i].Upper - loop->LocParts[cur_no].Set[i].Lower + 1;
            }

            /* process the dimension-parameter */

            Step = loop->LocParts[cur_no].Set[dim].Step = LocalInGlobalInd[dim].Step;

            if ( Counter < PL->Rank )
            {
                if (PL->Invers[dim])
                {
                    loop->LocParts[cur_no].Set[dim].Lower = LocalInGlobalInd[dim].Upper - Step*(TraceOptions.LocIterWidth - 1);
                    loop->LocParts[cur_no].Set[dim].Upper = LocalInGlobalInd[dim].Upper;
                }
                else
                {
                    /* the left hand side of loop is being created */
                    loop->LocParts[cur_no].Set[dim].Lower = LocalInGlobalInd[dim].Lower;
                    loop->LocParts[cur_no].Set[dim].Upper = LocalInGlobalInd[dim].Lower + Step*(TraceOptions.LocIterWidth - 1);
                }
            }
            else
            {
                if (PL->Invers[dim])
                {
                    loop->LocParts[cur_no].Set[dim].Lower = LocalInGlobalInd[dim].Lower;

                    Lower1 = LocalInGlobalInd[dim].Upper - Step * TraceOptions.LocIterWidth;

                    Lower2 = LocalInGlobalInd[dim].Upper - Step * ( (LocalInGlobalInd[dim].Upper -
                                 LocalInGlobalInd[dim].Lower) / Step - TraceOptions.LocIterWidth + 1);

                    loop->LocParts[cur_no].Set[dim].Upper = dvm_min(Lower1, Lower2);
                }
                else
                {
                    Lower = LocalInGlobalInd[dim].Lower;
                    Lower1 = Lower + Step * TraceOptions.LocIterWidth;
                    Lower2 = Lower + Step * ( (LocalInGlobalInd[dim].Upper-Lower)
                            / Step - TraceOptions.LocIterWidth + 1);
                    loop->LocParts[cur_no].Set[dim].Lower = dvm_max(Lower1, Lower2);
                    loop->LocParts[cur_no].Set[dim].Upper = LocalInGlobalInd[dim].Upper;
                }
            }

            loop->LocParts[cur_no].Set[dim].Size =
                loop->LocParts[cur_no].Set[dim].Upper - loop->LocParts[cur_no].Set[dim].Lower + 1;

            cur_no++;
            Counter++;
        }

        loop->Init = 1;
    }

    if ( loop->LocHandle == 0 )
    {
         /* it's the first time we run this function for a portion from dopl_
          * set up the data structures.
          */
        loop->DoplRes.Rank = PL->Rank;
        loop->BordersSetBack = 0;

        for(i=0; i < PL->Rank; i++)
        {
            if ( ! PL->Invers[i] )
            {
                if ( *(PL->MapList[i].InitIndexPtr) > *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is not inverted, but the last iteration limit is less than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }

                loop->DoplRes.Set[i].Lower = *(PL->MapList[i].InitIndexPtr);
                loop->DoplRes.Set[i].Upper = *(PL->MapList[i].LastIndexPtr);
            }
            else
            {
                if ( *(PL->MapList[i].InitIndexPtr) < *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is inverted, but the last iteration limit is greater than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }

                loop->DoplRes.Set[i].Lower = *(PL->MapList[i].LastIndexPtr);
                loop->DoplRes.Set[i].Upper = *(PL->MapList[i].InitIndexPtr);
            }

            loop->DoplRes.Set[i].Step = dvm_abs(*(PL->MapList[i].StepPtr)); /* dvm_abs - just in case */
            loop->DoplRes.Set[i].Size = loop->DoplRes.Set[i].Upper - loop->DoplRes.Set[i].Lower + 1;
        }

        loop->HasItems = 1;
    }

    found = 0;

    while ( (loop->LocHandle <= 2*PL->Rank) && !found )
    {
        Counter = block_Intersect(&block, &(loop->LocParts[loop->LocHandle]), &(loop->DoplRes), &(loop->DoplRes), 1);
                                                                /* may try to set 0 as the last parameter some day */
        if ( Counter )
        {
            for( i=0; i < PL->Rank; i++)
            {
                if ( PL->Invers[i] )
                {
                    *(PL->MapList[i].InitIndexPtr) = block.Set[i].Upper;
                    *(PL->MapList[i].LastIndexPtr) = block.Set[i].Lower;
                }
                else
                {
                    *(PL->MapList[i].InitIndexPtr) = block.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = block.Set[i].Upper;
                }
            }

            found = 1;
        }

        loop->LocHandle++;
    }

    if ( found )
    {
        if ( loop->LocHandle == PL->Rank + 1 )
            *Trace.vtr = 0;
        else
            *Trace.vtr = 1;

        loop->BordersSetBack = 1;
        return 1;
    }

    if ( loop->LocHandle > 2*PL->Rank ) /* nothing found */
    {
        loop->HasItems = 0;
        loop->LocHandle = 0;

        if ( loop->BordersSetBack )
        {
            /* restore the information into iterations borders, because dopl_ don't change some sometimes ... */
            for( i=0; i < PL->Rank; i++ )
            {
                if ( PL->Invers[i] )
                {
                    *(PL->MapList[i].InitIndexPtr) = loop->DoplRes.Set[i].Upper;
                    *(PL->MapList[i].LastIndexPtr) = loop->DoplRes.Set[i].Lower;
                }
                else
                {
                    *(PL->MapList[i].InitIndexPtr) = loop->DoplRes.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = loop->DoplRes.Set[i].Upper;
                }
            }
        }

        return 0;
    }

    /* unreachable code */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}

/*
 * Before returning 0 should set HasItems and LocHandle to zero
 *
 * Before returning 1 should set VTR to the required value
 *
 * */
int divide_nodes (s_PARLOOP *PL, LOOP_INFO* loop)
{
    s_BLOCK        block;
    int            i, j, p, counter, found;
    DvmType           Lower, Step, Lower1, Lower2;

    if ( !loop->Init )
    {
        /* we need to save the Local part information in the Global Iterations format in the internal structures
         * in blocks ordered by the calculations direction (ACROSS scheme must work) */
        s_REGULARSET LocalInGlobalInd[MAXARRAYDIM];

        loop->LocHandle = 0;
        loop->BlCount = ((int)1 << (PL->Rank + 1)) - 1;

        dvm_AllocArray(s_BLOCK, loop->BlCount, loop->LocParts);

        DBG_ASSERT(__FILE__, __LINE__, PL->Local && PL->HasLocal );

        /* translate loop local part into global coordinates */
        for(i=0; i < PL->Rank; i++)
        {
            LocalInGlobalInd[i].Lower = PL->Local[i].Lower + PL->InitIndex[i];
            LocalInGlobalInd[i].Upper = PL->Local[i].Upper + PL->InitIndex[i];
            LocalInGlobalInd[i].Step =  PL->Local[i].Step;

            if ( LocalInGlobalInd[i].Upper-LocalInGlobalInd[i].Lower+1 <= 0 )
            {  /* empty dimension and local part. Sometimes happen */
                *Trace.vtr = 1;
                dvm_FreeArray(loop->LocParts);

                return 0;
                /* HasItems is equal zero here, that is why we will not get into infinite loop here
                 * there is no need to care of the restoring the initial iteration borders because it is the
                 * first call to divide_set for the loop */
            }
        }

        p = loop->BlCount;

        for(i=0; i < p; i++)
            loop->LocParts[i].Rank = PL->Rank;

        for(i=0; i < PL->Rank; i++)
        {
            p >>= 1; j = 0;

            while ( j < loop->BlCount )
            {
                /* lower part */
                for ( counter=0; counter<p; counter++,j++ )
                {
                    Step = loop->LocParts[j].Set[i].Step = LocalInGlobalInd[i].Step;

                    if (PL->Invers[i])
                    {
                        loop->LocParts[j].Set[i].Lower = LocalInGlobalInd[i].Upper - Step*(TraceOptions.LocIterWidth - 1);
                        loop->LocParts[j].Set[i].Upper = LocalInGlobalInd[i].Upper;
                    }
                    else
                    {
                        /* the left hand side of loop is being created */
                        loop->LocParts[j].Set[i].Lower = LocalInGlobalInd[i].Lower;
                        loop->LocParts[j].Set[i].Upper = LocalInGlobalInd[i].Lower + Step*(TraceOptions.LocIterWidth - 1);
                    }

                    loop->LocParts[j].Set[i].Size =
                        loop->LocParts[j].Set[i].Upper - loop->LocParts[j].Set[i].Lower + 1;
                }

                /* middle part */
                Step = loop->LocParts[j].Set[i].Step = LocalInGlobalInd[i].Step;
                Lower = LocalInGlobalInd[i].Lower;

                loop->LocParts[j].Set[i].Lower = Lower + Step * TraceOptions.LocIterWidth;
                loop->LocParts[j].Set[i].Upper =
                        Lower + Step * ( (LocalInGlobalInd[i].Upper - Lower) / Step - TraceOptions.LocIterWidth);

                loop->LocParts[j].Set[i].Size =
                        loop->LocParts[j].Set[i].Upper - loop->LocParts[j].Set[i].Lower + 1;

                j++;

                /* upper part */
                for ( counter=0; counter<p; counter++,j++ )
                {
                    Step = loop->LocParts[j].Set[i].Step = LocalInGlobalInd[i].Step;

                    if (PL->Invers[i])
                    {
                        loop->LocParts[j].Set[i].Lower = LocalInGlobalInd[i].Lower;

                        Lower1 = LocalInGlobalInd[i].Upper - Step * TraceOptions.LocIterWidth;

                        Lower2 = LocalInGlobalInd[i].Upper - Step * ( (LocalInGlobalInd[i].Upper -
                                    LocalInGlobalInd[i].Lower) / Step - TraceOptions.LocIterWidth + 1);

                        loop->LocParts[j].Set[i].Upper = dvm_min(Lower1, Lower2);
                    }
                    else
                    {
                        Lower = LocalInGlobalInd[i].Lower;
                        Lower1 = Lower + Step * TraceOptions.LocIterWidth;
                        Lower2 = Lower + Step * ( (LocalInGlobalInd[i].Upper-Lower)
                                / Step - TraceOptions.LocIterWidth + 1);
                        loop->LocParts[j].Set[i].Lower = dvm_max(Lower1, Lower2);
                        loop->LocParts[j].Set[i].Upper = LocalInGlobalInd[i].Upper;
                    }

                    loop->LocParts[j].Set[i].Size =
                        loop->LocParts[j].Set[i].Upper - loop->LocParts[j].Set[i].Lower + 1;
                }

                /* process fully this block */
                if ( j < loop->BlCount )
                {
                    loop->LocParts[j].Set[i].Step = LocalInGlobalInd[i].Step;
                    loop->LocParts[j].Set[i].Lower = LocalInGlobalInd[i].Lower;
                    loop->LocParts[j].Set[i].Upper = LocalInGlobalInd[i].Upper;
                    loop->LocParts[j].Set[i].Size =
                        loop->LocParts[j].Set[i].Upper - loop->LocParts[j].Set[i].Lower + 1;
                    j++;
                }
            }
        }
        loop->Init = 1;
    }

    if ( loop->LocHandle == 0 )
    {
         /* it's the first time we run this function for a portion from dopl_
          * set up the data structures.
          */
        loop->DoplRes.Rank = PL->Rank;
        loop->BordersSetBack = 0;

        for(i=0; i < PL->Rank; i++)
        {
            if ( ! PL->Invers[i] )
            {
                if ( *(PL->MapList[i].InitIndexPtr) > *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is not inverted, but the last iteration limit is less than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }

                loop->DoplRes.Set[i].Lower = *(PL->MapList[i].InitIndexPtr);
                loop->DoplRes.Set[i].Upper = *(PL->MapList[i].LastIndexPtr);
            }
            else
            {
                if ( *(PL->MapList[i].InitIndexPtr) < *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is inverted, but the last iteration limit is greater than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }

                loop->DoplRes.Set[i].Lower = *(PL->MapList[i].LastIndexPtr);
                loop->DoplRes.Set[i].Upper = *(PL->MapList[i].InitIndexPtr);
            }

            loop->DoplRes.Set[i].Step = dvm_abs(*(PL->MapList[i].StepPtr)); /* dvm_abs - just in case */
            loop->DoplRes.Set[i].Size = loop->DoplRes.Set[i].Upper - loop->DoplRes.Set[i].Lower + 1;
        }

        loop->HasItems = 1;
    }

    found = 0;

    while ( (loop->LocHandle < loop->BlCount) && !found )
    {
        p = block_Intersect(&block, &(loop->LocParts[loop->LocHandle]), &(loop->DoplRes), &(loop->DoplRes), 1);
                                                                /* may try to set 0 as the last parameter some day */
        if ( p )
        {
            for( i=0; i < PL->Rank; i++)
            {
                if ( PL->Invers[i] )
                {
                    *(PL->MapList[i].InitIndexPtr) = block.Set[i].Upper;
                    *(PL->MapList[i].LastIndexPtr) = block.Set[i].Lower;
                }
                else
                {
                    *(PL->MapList[i].InitIndexPtr) = block.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = block.Set[i].Upper;
                }
            }

            found = 1;
        }

        loop->LocHandle++;
    }

    if ( found )
    {
        if ( loop->LocHandle % 2 )
            *Trace.vtr = 1;   /* the VTR value is assigned correctly because the
                               * parity has already been changed for the next LocHandle */
        else
            *Trace.vtr = 0;

        loop->BordersSetBack = 1;
        return 1;
    }

    if ( loop->LocHandle == loop->BlCount ) /* nothing found */
    {
        loop->HasItems = 0;
        loop->LocHandle = 0;

        if ( loop->BordersSetBack )
        {
            /* restore the information into iterations borders, because dopl_ don't change some sometimes ... */
            for( i=0; i < PL->Rank; i++ )
            {
                if ( PL->Invers[i] )
                {
                    *(PL->MapList[i].InitIndexPtr) = loop->DoplRes.Set[i].Upper;
                    *(PL->MapList[i].LastIndexPtr) = loop->DoplRes.Set[i].Lower;
                }
                else
                {
                    *(PL->MapList[i].InitIndexPtr) = loop->DoplRes.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = loop->DoplRes.Set[i].Upper;
                }
            }
        }

        return 0;
    }

    /* unreachable code */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}

/*
 * Before returning 0 should set HasItems and LocHandle to zero
 *
 * Before returning 1 should set VTR to the required value
 *
 * */
int divide_compare (s_PARLOOP *PL, LOOP_INFO* loop)
{
    STRUCT_BEGIN   *LoopBeg;
    ITERBLOCK      iblock;
    int            i, j, k, found;

    /* TraceMode == mode_COMPARETRACE due to selected function */
    DBG_ASSERT(__FILE__, __LINE__, TraceOptions.TraceMode == mode_COMPARETRACE );

    if ( EnableTrace == 0 || Trace.CurStruct == -1 )
    {   /* debugging is switched off */

        EnableTrace = 0;
        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                "*** RTS err: CMPTRACE: Debugging was disabled, can not access trace data.\n");

        /* unreachable code */
        /* do not execute "debug" version of the loop body */
        // *Trace.vtr = 0; /* need advanced mechanism, this approach will not work */
        return 1;
    }

    if ( !loop->Init )
    {
        /* we need to save the Local part information in the Global Iterations format in the internal structures
         * in blocks ordered by the calculations direction (ACROSS scheme must work) */
        ITERBLOCK LocalInGlobalInd;
        SORTPAIR  *chunks;
        DvmType      mult[MAXARRAYDIM];
        byte      need_correct;
        int       cur_idx;

        DBG_ASSERT(__FILE__, __LINE__, PL->Local && PL->HasLocal );

        /* translate loop local part into global coordinates */
        LocalInGlobalInd.Rank = PL->Rank;
        for(i=0; i < PL->Rank; i++)
        {
            if ( PL->Local[i].Upper - PL->Local[i].Lower + 1 <= 0 )
            { /* nothing to do !!! */
                DBG_ASSERT(__FILE__, __LINE__, 0);
                *Trace.vtr = 1;
                return 0;
                /* HasItems is equal zero here, that is why we will not get into infinite loop here
                 * there is no need to care of the restoring the initial iteration borders because it is the
                 * first call to divide_set for the loop */
            }
            if(PL->Invers[i])
            {
                LocalInGlobalInd.Set[i].Lower =  PL->Local[i].Upper + PL->InitIndex[i];
                LocalInGlobalInd.Set[i].Upper =  PL->Local[i].Lower + PL->InitIndex[i];
                LocalInGlobalInd.Set[i].Step  = -PL->Local[i].Step;
            }
            else
            {
                LocalInGlobalInd.Set[i].Lower = PL->Local[i].Lower + PL->InitIndex[i];
                LocalInGlobalInd.Set[i].Upper = PL->Local[i].Upper + PL->InitIndex[i];
                LocalInGlobalInd.Set[i].Step  = PL->Local[i].Step;
            }
        }

        LoopBeg = table_At(STRUCT_BEGIN, &Trace.tTrace, Trace.CurStruct);
        if ( LoopBeg->pChunkSet == NULL  )
        {
            if ( EnableTrace )
            {
                error_CmpTraceExt(Trace.CurTraceRecord, DVM_FILE[0], DVM_LINE[0], ERR_RD_STRUCT);
                epprintf(MultiProcErrReg1, __FILE__,__LINE__,
                "*** RTS err: doplmb_: bad trace (TraceRecord = %lu,  TraceCPU = %d): no CHUNKS information in reference trace"
                " for loop %d!\n", LoopBeg->Line_num, LoopBeg->CPU_Num, loop->No);
            }
            else
            {
                epprintf(MultiProcErrReg1, __FILE__,__LINE__,
                "*** RTS err: doplmb_: bad trace: no CHUNKS information in reference trace"
                " for loop %d!\n", loop->No);
            }
        }

        /* select only those local blocks which intersect current local part */
        loop->BlCount = 0;
        for(i=0; i <= LoopBeg->iCurCPU; i++) /* attention "<=" */
        {
            if ( LoopBeg->pChunkSet[i].Size != 0 )
            {
                if ( (LoopBeg->pChunkSet[i].Size == 1) || /* empty local part */
                        !iters_intersect_correct(&LoopBeg->pChunkSet[i].Chunks[1], &LocalInGlobalInd, &need_correct) )
                { /* if there is no intersection then free the memory */
                    mac_free(&LoopBeg->pChunkSet[i].Chunks);
                    LoopBeg->pChunkSet[i].Chunks = NULL;
                }
                else
                { /* otherwise blocks were intersected and if necessary the borders were corrected */
                    if ( need_correct ) /* but it may be also necessary to correct the chunks */
                    {   /* Go through chunks and correct them if necessary */
                        DBG_ASSERT(__FILE__, __LINE__, LoopBeg->pChunkSet[i].Size > 2);
                        for(j=2; j < LoopBeg->pChunkSet[i].Size; j++)
                        {
                            iters_intersect_correct(&LoopBeg->pChunkSet[i].Chunks[j], &LoopBeg->pChunkSet[i].Chunks[1], &need_correct);
                            /* do not use return value of this function; */
                            /* useless blocks were marked with Rank = 0 */
                        }
                    }
                    loop->BlCount += LoopBeg->pChunkSet[i].Size - 2; /* not exact size */
                }
            } /* LoopBeg->pChunkSet[i].Size == 0 when this loop is found only on some CPUs */
            else
                DBG_ASSERT(__FILE__, __LINE__, 0);
        }

        if ( loop->BlCount <= 0 )
        {
            EnableTrace = 0;
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                    "*** RTS err: CMPTRACE: Trace error: no iterations to execute.\n");
            /* unreachable code */
            exit(-1);
        }

        /* investigate if any of Local part blocks are intersected */
        for(i=0; i <= LoopBeg->iCurCPU; i++) /* attention "<=" */
            if ( LoopBeg->pChunkSet[i].Chunks != NULL )
                for(j=i+1; j <= LoopBeg->iCurCPU; j++)
                    if ( LoopBeg->pChunkSet[j].Chunks &&
                         iters_intersect_test(&LoopBeg->pChunkSet[i].Chunks[1], &LoopBeg->pChunkSet[j].Chunks[1]) )
                    {   /* there is nonempty intersection of blocks */
                        if ( chunksets_Compare(&LoopBeg->pChunkSet[i], &LoopBeg->pChunkSet[j]) )
                        { /* replicated information, free the memory */
                            mac_free(&LoopBeg->pChunkSet[j].Chunks);
                            LoopBeg->pChunkSet[j].Chunks = NULL;
                            loop->BlCount -= LoopBeg->pChunkSet[j].Size - 2;
                        }
                        else
                        {
                            EnableTrace = 0;
                            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                                    "*** RTS err: CMPTRACE: Non-empty intersection of local blocks is not supported in this version.\n");
                            /* unreachable code */
                            exit(-1);
                        }
                    }

        /* Fine: no intersections */
        /* Now build the structure to sort itersets for ACROSS scheme correct work */

        dvm_AllocArray(SORTPAIR, loop->BlCount, chunks);

        /* find not-empty-localpart processor to fetch iterlimits */
        for(j=0; j <= LoopBeg->iCurCPU; j++) /* attention "<=" */
            if ( LoopBeg->pChunkSet[j].Chunks != NULL )
                break;

        DBG_ASSERT(__FILE__, __LINE__, j <= LoopBeg->iCurCPU );

        /* Prepare multipliers for effective LI calculation */
        mult[PL->Rank-1] = 1;
        for(i = PL->Rank - 2; i >= 0 ; i--)
        {
            if( LoopBeg->pChunkSet[j].Chunks[0].Set[i+1].Step < 0 )
                mult[i] = LoopBeg->pChunkSet[j].Chunks[0].Set[i+1].Lower -
                          LoopBeg->pChunkSet[j].Chunks[0].Set[i+1].Upper + 1;
            else
                mult[i] = LoopBeg->pChunkSet[j].Chunks[0].Set[i+1].Upper -
                          LoopBeg->pChunkSet[j].Chunks[0].Set[i+1].Lower + 1;
            mult[i] *= mult[i+1];
        }

        cur_idx = 0;
        for(i=0; i <= LoopBeg->iCurCPU; i++) /* attention "<=" */
            if ( LoopBeg->pChunkSet[i].Chunks ) /* this local part intersects with current Local */
            {
                for(j=2; j < LoopBeg->pChunkSet[i].Size; j++)
                    if ( LoopBeg->pChunkSet[i].Chunks[j].Rank != 0 )
                    {
                        /* generate good-for-ACROSS LI */
                        chunks[cur_idx].LI = 0;

                        for (k=PL->Rank-1; k >= 0 ; k--)
                            if( LoopBeg->pChunkSet[i].Chunks[j].Set[k].Step < 0 )
                                chunks[cur_idx].LI +=
                                    mult[k] * ( LoopBeg->pChunkSet[i].Chunks[0].Set[k].Lower -
                                                LoopBeg->pChunkSet[i].Chunks[j].Set[k].Lower  );
                            else
                                chunks[cur_idx].LI +=
                                    mult[k] * LoopBeg->pChunkSet[i].Chunks[j].Set[k].Lower;

                        chunks[cur_idx].iblock = &LoopBeg->pChunkSet[i].Chunks[j];
                        cur_idx++;
                    }
            }

        DBG_ASSERT(__FILE__, __LINE__, cur_idx > 0);

        loop->BlCount = cur_idx;

        /* Sort by LI */
        qsort(chunks, cur_idx, sizeof(SORTPAIR), &sortpair_compare);

        loop->LocParts  = (s_BLOCK *) chunks;
        loop->LocHandle = 0;
        loop->Init = 1;
    }

    if ( loop->LocHandle == 0 )
    {
         /* it's the first time we run this function for a portion from dopl_
          * set up the data structures.
          */
        loop->DoplRes.Rank = PL->Rank;
        loop->BordersSetBack = 0;

        for(i=0; i < PL->Rank; i++)
        {
            if ( ! PL->Invers[i] )
            {
                if ( *(PL->MapList[i].InitIndexPtr) > *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is not inverted, but the last iteration limit is less than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }
            }
            else
                if ( *(PL->MapList[i].InitIndexPtr) < *(PL->MapList[i].LastIndexPtr) )
                { /* dimension is inverted, but the last iteration limit is greater than initial */
                    loop->HasItems = 0;
                    return 0; /* LocHandle is equal zero */
                    /* there is no need to care of restoring the initial iteration limits because it
                     * is the first call to divide_set for the portion from dopl_*/
                }
            /* do not translate coordinates in lower < upper form */
            loop->DoplRes.Set[i].Lower = *(PL->MapList[i].InitIndexPtr);
            loop->DoplRes.Set[i].Upper = *(PL->MapList[i].LastIndexPtr);
            loop->DoplRes.Set[i].Step  = *(PL->MapList[i].StepPtr);
            //loop->DoplRes.Set[i].Size  = loop->DoplRes.Set[i].Upper - loop->DoplRes.Set[i].Lower + 1;
        }

        loop->HasItems = 1;
    }

    found = 0;

    while ( (loop->LocHandle < loop->BlCount) && !found )
    {
        k = it_bl_intersect_res(&iblock, ((SORTPAIR *)loop->LocParts)[loop->LocHandle].iblock, &(loop->DoplRes));

        if ( k )
        {
            for( i=0; i < PL->Rank; i++)
            {
                    *(PL->MapList[i].InitIndexPtr) = iblock.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = iblock.Set[i].Upper;
            }

            found = 1;
        }

        loop->LocHandle++;
    }

    if ( found )
    {
        *Trace.vtr = ((SORTPAIR *)loop->LocParts)[loop->LocHandle-1].iblock->vtr;

        loop->BordersSetBack = 1;
        return 1;
    }

    if ( loop->LocHandle == loop->BlCount ) /* nothing found */
    {
        loop->HasItems = 0;
        loop->LocHandle = 0;

        if ( loop->BordersSetBack )
        {
            /* restore the information into iterations borders, because dopl_ don't change some sometimes ... */
            for( i=0; i < PL->Rank; i++ )
            {
                    *(PL->MapList[i].InitIndexPtr) = loop->DoplRes.Set[i].Lower;
                    *(PL->MapList[i].LastIndexPtr) = loop->DoplRes.Set[i].Upper;
            }
        }

        return 0;
    }

    /* unreachable code */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}

DvmType __callstd doplmbseq_(DvmType *No, DvmType *Rank, DvmType Init[], DvmType Last[], DvmType Step[])
{
    DvmType lRes;
    int i;
    LOOP_INFO *loop = NULL;
    s_PARLOOP      *PL;
    DvmType sz;
    static DvmType SIZE = 0;
    DvmType t_sz;

#ifndef NO_DOPL_DOPLMB_TRACE
    char buffer[999];
    sprintf(buffer, "\ncall doplmb_, loop %ld, VTR = %d\n", *No, Trace.vtr?*Trace.vtr:-1);
    SYSTEM(fputs, (buffer, Trace.DoplMBFileHandle));
#endif

    PL = Trace.tmpPL;

    if ( !isEmpty(&Trace.sLoopsInfo) )
    {
        loop = stackLast(&Trace.sLoopsInfo);

        while ( loop->No != *No )
        {   /* the loop has been exited using goto... we need to correct this !! */
            UDvmType tmp = loop->Line;

            pprintf(3, "*** RTS err: CMPTRACE: Incorrect DOPLMBSEQ call for loop %ld (File=%s, Line=%ld) instead of "
                       "loop %ld (Line=%ld).\n", *No, DVM_FILE[0], DVM_LINE[0], loop->No, loop->Line);
            DBG_ASSERT(__FILE__, __LINE__, 0);

            //DBG_ASSERT(__FILE__, __LINE__, !isEmpty (&Trace.sLoopsInfo) ); this is performed in stackLast function
            dendl_(&loop->No, &tmp); /* necessary stack changes will be automatically produced */
            loop = stackLast(&Trace.sLoopsInfo);
        }
        //DBG_ASSERT(__FILE__, __LINE__, loop->No == *No );

        if ( (Trace.vtr == NULL) || (Trace.sLoopsInfo.top - 1 != Trace.ctrlIndex) || (TraceOptions.LocIterWidth == -1) )
        {
            if ( !loop->Init  )
            {
                loop->Init = 1;    /* next time - finish the loop */
                RETURN( 1 )
                return 1;
            }
            else
            {
                RETURN( 0 )
                return 0;
            }
            /* do not create any chunk record because this loop is not gonna be traced */
        }

        if ( !loop->HasItems ) /* the loop is not yet initialized, initialize. */
        {
/* SIZE must be equal zero since the last loop entrance */
#ifndef NO_SIZE_CHECK
DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) == 0 );
#endif

            PL->Rank = (int) *Rank;
            PL->HasLocal = 1;
            DBG_ASSERT(__FILE__, __LINE__, PL->MapList && PL->Local);

            for(i=0; i < PL->Rank; i++)
            {
                if(Init[i] > Last[i])
                {
                    if(Step[i] >= 0)
                        return 0; /* nothing to do */

                    PL->Invers[i] = 1;

                    PL->Local[i].Step = -Step[i];
                    PL->Local[i].Upper = Init[i];
                    PL->Local[i].Lower = Last[i];

                    lRes = (Init[i] - Last[i]) % PL->Local[i].Step;
                    if (lRes)
                        PL->Local[i].Lower += (PL->Local[i].Step - lRes);
                }
                else
                {
                    if(Step[i] <= 0)
                    {
                        if(Init[i] < Last[i])
                            return 0; /* nothing to do */

                        /* Init[i] == Last[i], Step[i] <=0 */
                        PL->Invers[i] = 1;

                        PL->Local[i].Step = -Step[i];
                        PL->Local[i].Upper = Init[i];
                        PL->Local[i].Lower = Last[i];
                    }
                    else
                    {   /* Step[i] >= 0, Init[i] <= Last[i] */
                        PL->Invers[i] = 0;
                        PL->Local[i].Step = Step[i];
                        PL->Local[i].Lower = Init[i];
                        PL->Local[i].Upper = Last[i];
                    }
                }

                PL->MapList[i].InitIndexPtr = &Init[i];
                PL->MapList[i].LastIndexPtr = &Last[i];
                PL->MapList[i].StepPtr      = &Step[i];
            }

/* calculate new SIZE */
#ifndef NO_SIZE_CHECK
            SIZE = 1;

            for( i=0; i < PL->Rank && SIZE > 0; i++ )
            {
                    if ( PL->Invers[i] )
                    {
                        SIZE *= *(PL->MapList[i].InitIndexPtr) - *(PL->MapList[i].LastIndexPtr) + 1;
                    }
                    else
                    {
                        SIZE *= *(PL->MapList[i].LastIndexPtr) - *(PL->MapList[i].InitIndexPtr) + 1;
                    }
            }
            if ( SIZE < 0 ) SIZE = 0;
#endif
        }

        if ( divide_set(PL, loop) != 0 )
        {
            /* SIZE checking feature */
#ifndef NO_SIZE_CHECK
            t_sz = 1;
            for( i=0; i < PL->Rank && t_sz > 0; i++ )
            {
                    if ( PL->Invers[i] )
                    {
                        t_sz *= *(PL->MapList[i].InitIndexPtr) - *(PL->MapList[i].LastIndexPtr) + 1;
                    }
                    else
                    {
                        t_sz *= *(PL->MapList[i].LastIndexPtr) - *(PL->MapList[i].InitIndexPtr) + 1;
                    }
            }

            DBG_ASSERT(__FILE__, __LINE__, t_sz > 0 );

            SIZE -= t_sz;

            DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) >= 0 );
#endif
            /* generate chunk record */
            if ( EnableTrace && TraceOptions.TraceMode != mode_COMPARETRACE )
            /* we should not save anything in compare mode */
                trc_put_chunk(PL); /* all correctness checks are inside the function */

#ifdef DONT_TRACE_NECESSARY_CHECK
            if ( EnableTrace && TraceOptions.TraceMode == mode_COMPARETRACE
                && !*Trace.vtr ) /* do not trace (vtr == off) condition */
            {
                DvmType index[MAXARRAYDIM];
                dont_trace_necessary_check(0, index, PL);
            }
#endif
            RETURN(1)
            return 1;
        }
        else
        {   /* check if the size is correct */
#ifndef NO_SIZE_CHECK
        DBG_ASSERT(__FILE__, __LINE__, (sz=SIZE) == 0 );
#endif
            RETURN(0)
            PL->HasLocal = 0;
            return 0;
        }
    }

    /* this code must be unreachable */
    DBG_ASSERT(__FILE__, __LINE__, 0 );
    return 0;
}

#endif /* _MBODIES_C_ */
