#ifndef _TRACER_DVMDBG_C_
#define _TRACER_DVMDBG_C_
/***********************/

#ifdef _MPI_PROF_TRAN_


void __callstd  tracer_dvm_exit_(int  rc)
{
/*printf("dvm_OneProcNum=%d\n", dvm_OneProcNum);*/
   dvm_exit(rc);

   return;
}


void __callstd  tracer_clock_res_(double  clock_res)
{
   TracerClockRes = clock_res;

   return;
}

/*
 * MPI-awareness support functions
 * */

void __callstd tracer_send_(int cpu_to, int tag, DvmType comm, t_tracer_time tracer_time, int comp_err)
{
    if ( EnableTrace && dvm_OneProcSign && (mode_COMPARETRACE == TraceOptions.TraceMode) )
    {
        MSG_INFO* pMsgInfo = NULL;

        DBG_ASSERT(__FILE__, __LINE__, tracer_time != 0);

        if ( !Trace.ErrorIsFixed && ((TraceCompareErrors.ErrCount > 0) || comp_err ) )
        {
            Trace.ErrorIsFixed = 1;

            pMsgInfo = table_GetNew(MSG_INFO, &Trace.tMessages);
            pMsgInfo->rec_type = msg_ERR;
            pMsgInfo->msg_time = tracer_time;

            dvm_AllocArray(byte, dvm_OneProcCount, Trace.pMSGSentAlready);
            memset(Trace.pMSGSentAlready, 0, dvm_OneProcCount);
        }

        if ( !Trace.ErrorIsFixed  || (Trace.ErrorIsFixed && !Trace.pMSGSentAlready[cpu_to])  )
        {
            pMsgInfo = table_GetNew(MSG_INFO, &Trace.tMessages);
            pMsgInfo->rec_type = msg_SEND;
            pMsgInfo->cpu_num  = cpu_to;
            pMsgInfo->msg_tag  = tag;
            pMsgInfo->msg_comm = comm;
            pMsgInfo->msg_time = tracer_time;

            /* we should stop all other sends to cpu_to CPU from now */
            if ( Trace.ErrorIsFixed )
            {
                Trace.pMSGSentAlready[cpu_to] = 1;
            }
        }
    }
}

/*
 * this function stores receiving messages history until error_time
 */
void __callstd tracer_recv_(int cpu_from, int tag, DvmType comm, t_tracer_time tracer_time, int comp_err )
{
    if ( EnableTrace && dvm_OneProcSign && (mode_COMPARETRACE == TraceOptions.TraceMode) )
    {
        MSG_INFO* pMsgInfo = NULL;

        DBG_ASSERT(__FILE__, __LINE__, tracer_time != 0);

        if ( !Trace.ErrorIsFixed )
        {
            pMsgInfo = table_GetNew(MSG_INFO, &Trace.tMessages);

            if ( (TraceCompareErrors.ErrCount > 0) || comp_err )
            {
                Trace.ErrorIsFixed = 1;

                pMsgInfo->rec_type = msg_ERR;
                pMsgInfo->msg_time = tracer_time;
            }
            else
            {
                pMsgInfo->rec_type = msg_RECV;
                pMsgInfo->cpu_num  = cpu_from;
                pMsgInfo->msg_tag  = tag;
                pMsgInfo->msg_comm = comm;
                pMsgInfo->msg_time = tracer_time;
            }
        }
    }
}

void msg_trc_wrt_proc(byte* pMsg, FILE* fp, char* FileName)
{
    if (fwrite(pMsg, sizeof(struct tag_MSG_INFO), 1, fp) < 1)
    {
        pprintf(3, "*** RTS err: Message file <%s> write error. Process terminated.\n", FileName);
        fflush (stderr);
        exit (-1);
    }
}

/*void err_trc_wrt_proc(byte* pErr, FILE* fp, char* FileName)
{
    /* not all errorrs should be written !!! */

  /*  if (fwrite(pErr, sizeof(struct tag_MSG_INFO), 1, fp) < 1)
    {
        pprintf(3, "*** RTS err: Message file <%s> write error. Process terminated.\n", FileName);
        fflush (stderr);
        exit (-1);
    }
}   */

void __callstd tracer_proc_term_(t_tracer_time tracer_time, int comp_err)
{
    FILE*    MsgFile;
    char     FileName[MaxPathSize + 1];
    MSG_INFO msg_fin = {msg_FIN, 0, 0, 0};

    if ( EnableTrace && dvm_OneProcSign && (mode_COMPARETRACE == TraceOptions.TraceMode) )
    {
        SYSTEM(sprintf, (FileName, "%s%s.%d.msg.trd",
                TraceOptions.TracePath, dvm_argv[0], dvm_OneProcNum ));
        SYSTEM(remove, (FileName));
        SYSTEM_RET(MsgFile, fopen, (FileName, "wb"));

        if ( !MsgFile )
        {
            pprintf(3, "*** RTS err: Can't open trace file <%s> for writing\n", FileName);
        }
        else
        {
            table_Iterator(&Trace.tMessages, msg_trc_wrt_proc, MsgFile, FileName);
            if (  !Trace.ErrorIsFixed && ((TraceCompareErrors.ErrCount > 0) || comp_err ) )
            {
                Trace.ErrorIsFixed = 1;

                msg_fin.rec_type = msg_ERR;
                msg_fin.msg_time = tracer_time;
                msg_trc_wrt_proc((byte *) (&msg_fin), MsgFile, FileName);
                msg_fin.rec_type = msg_FIN;
            }
            msg_fin.msg_time = tracer_time; /* just to make classification algorithm work faster */
            msg_trc_wrt_proc((byte *)(&msg_fin), MsgFile, FileName);

            /* write errors of current CPU to this file */
            // table_Iterator(&TraceCompareErrors.tErrors, err_trc_wrt_proc, MsgFile, FileName);

            SYSTEM(fclose, (MsgFile))
        }
    }
}

void do_matching( TABLE* pSendTable, UDvmType send_startidx, UDvmType send_finidx,
                  TABLE* pRecvTable, UDvmType recv_startidx, UDvmType recv_finidx,
                                     int req_tag, DvmType req_comm)
{
    MSG_SEND*       pmsg_send;
    MSG_RECV*       pmsg_recv;
    int             i;
    UDvmType   cur_snd_si, cur_rcv_si;
    byte            send_found, recv_found;

    cur_snd_si = send_startidx;
    cur_rcv_si = recv_startidx;

    do
    {
        send_found = 0;
        for ( i = cur_snd_si; i < send_finidx; i++ )
        {
            pmsg_send = table_At(MSG_SEND, pSendTable, i);
            DBG_ASSERT(__FILE__, __LINE__, pmsg_send->snd_time != 0);

            if ( (pmsg_send->msg_tag != req_tag) || (pmsg_send->msg_comm != req_comm) )
                continue;

            send_found = 1;
            cur_snd_si = i+1;
            break;
        }

        if ( !send_found  )
            /* end matching */
            break;

        recv_found = 0;
        for ( i = cur_rcv_si; i < recv_finidx; i++ )
        {
            pmsg_recv = table_At(MSG_RECV, pRecvTable, i);
            DBG_ASSERT(__FILE__, __LINE__, pmsg_recv->rcv_time != 0);

            if ( (pmsg_recv->msg_tag != req_tag) || (pmsg_recv->msg_comm != req_comm) )
                continue;

            recv_found = 1;
            cur_rcv_si = i+1;
            break;
        }

        if ( !recv_found  )
            /* end matching */
            break;

       /* send_found && recv_found */
        pmsg_send->rcv_time = pmsg_recv->rcv_time;

    } while ( 1 );
}

/*
 * This function must be called only from ONE CPU. It generates final errors times data, stored in Trace structure.
 */

void __callstd tracer_task_term_(t_tracer_time tracer_time)
{
    char            FileName[MaxPathSize + 1];
    TABLE*          MsgTables;
    TABLE           RecvTable, SendTable;
    FILE*           MsgFile;
    MSG_INFO        msg_rec;
    MSG_SEND*       pmsg_send, *pmsg_send2;
    MSG_RECV*       pmsg_recv;
    int             i, j, k, t;
    t_tracer_time*  ErrTimes;    /* error times for each cpu */
    ERR_TIME*       ErrTimes2;   /* -----------------------  */
    UDvmType*  FromToOffs;
    DvmType*           FromToCount;
    UDvmType*  RecvOffs;
    UDvmType*  RecvCount;
    byte            fin, for_exit;
    static byte     print_no_recv = 1, once_called = 0;

    printf("!@#$%^\n");

    //_asm{int 3};

    if ( !EnableTrace || !dvm_OneProcSign || (mode_COMPARETRACE != TraceOptions.TraceMode) )
        return ;

    if ( once_called ) /* check if this function is executed more than once (wrong!, for debugging) */
    {
        pprintf(3, "*** RTS err: tracer_task_term is called more than once for current CPU. Program is aborted.\n");
        exit(-1);
    }

    once_called = 1;

    /* create dvm_OneProcCount tables to store messages from cpu trace */
    dvm_AllocArray(TABLE, dvm_OneProcCount, MsgTables);

    dvm_AllocArray(t_tracer_time, dvm_OneProcCount, ErrTimes);
    dvm_AllocArray(ERR_TIME,      dvm_OneProcCount, ErrTimes2);
    dvm_AllocArray(UDvmType, dvm_OneProcCount, RecvOffs);
    dvm_AllocArray(UDvmType, dvm_OneProcCount, RecvCount);
    dvm_AllocArray(UDvmType, dvm_OneProcCount*dvm_OneProcCount, FromToOffs);
    dvm_AllocArray(DvmType,          dvm_OneProcCount*dvm_OneProcCount, FromToCount);

    table_Init(&RecvTable, 500, sizeof(struct tag_MSG_RECV), NULL);
    table_Init(&SendTable, 500, sizeof(struct tag_MSG_SEND), NULL);

    for ( i=0; i < dvm_OneProcCount; i++ )
    {
        SYSTEM(sprintf, (FileName, "%s%s.%d.msg.trd",
                TraceOptions.TracePath, dvm_argv[0], i ));
        SYSTEM_RET(MsgFile, fopen, (FileName, "rb"));

        for ( j=0; j < dvm_OneProcCount; j++ )
        {
            table_Init(&MsgTables[j], 100, sizeof(struct tag_MSG_SEND), NULL);
        }

        ErrTimes[i]  = 0;
        RecvCount[i] = 0;
        fin = 0;

        /* read trace data and put messages from different CPUs to different tables */
        while( !feof(MsgFile) && ! fin )
        {
            if ( fread(&msg_rec, sizeof(struct tag_MSG_INFO), 1, MsgFile) < 1 )
            {
                pprintf(3, "*** RTS err: Message file <%s> read error. Errors classification failed.\n", FileName);
                fflush (stderr);
                return ;
            }

            if ( msg_rec.rec_type == msg_FIN )
            {
                fin = 1;
                if ( ErrTimes[i] == 0 )
                {
                     ErrTimes[i] = msg_rec.msg_time + 1; /* just to make classification work faster */
                }
                break;
            }

            if ( msg_rec.rec_type == msg_ERR )
            {
                DBG_ASSERT(__FILE__, __LINE__, ErrTimes[i] == 0);      /* integrity check */

                ErrTimes[i] = msg_rec.msg_time;
                continue;
            }

            if ( msg_rec.rec_type == msg_SEND )
            {
                pmsg_send = table_GetNew(MSG_SEND, &MsgTables[msg_rec.cpu_num]);
                memcpy(pmsg_send, &msg_rec, sizeof(struct tag_MSG_INFO) );
                pmsg_send->rcv_time = 0;
            }

            if ( msg_rec.rec_type == msg_RECV )
            {
                pmsg_recv = table_GetNew(MSG_RECV, &RecvTable);
                memcpy(pmsg_recv, &msg_rec, sizeof(struct tag_MSG_INFO) );
                pmsg_recv->receiver_cpu_num = i;
                RecvCount[i]++;
            }
        }

        RecvOffs[i] = ( (i==0)?0:(RecvOffs[i-1]+RecvCount[i-1]) );

        if ( ! fin )
        {
            pprintf(3, "*** RTS err: Message file <%s> is incomplete. Errors classification cancelled.\n", FileName);
            fflush (stderr);
            return ;
        }

        SYSTEM(fclose, (MsgFile))
        SYSTEM(remove, (FileName));

        /* copy temporary send tables to final sorted-by-CPUs table */
        for ( j=0; j < dvm_OneProcCount; j++ )
        {
            DvmType tbl_sz = table_Count(&MsgTables[j]);

            t = i*dvm_OneProcCount + j;

            /* save indexes */
            FromToOffs[t]  = ( (t==0)?0:(FromToOffs[t-1]+FromToCount[t-1]) );
            FromToCount[t] = tbl_sz;

            for (k = 0; k < tbl_sz; k++)
            {
                pmsg_send = table_GetNew(MSG_SEND, &SendTable);
                memcpy(pmsg_send, table_At(MSG_SEND, &MsgTables[j], k), sizeof(struct tag_MSG_SEND) );
            }

            table_Done(&MsgTables[j]);
        }
    }

    dvm_FreeArray(MsgTables);

                                            /* make final errors classification */
    fin = 0;

    while ( !fin )
    {
        for ( i=0; i < dvm_OneProcCount; i++ )
        {
            ErrTimes2[i].err_time = ErrTimes[i];
            ErrTimes2[i].cpu_num  = i;
        }

        /* 1. sort CPUs by ErrTimes */
        stablesort(ErrTimes2, dvm_OneProcCount, sizeof(ERR_TIME), &errtime_compare);

        /* 2. remove useless messages in a loop and correct err_times of other CPUs if necessary */
        for_exit = 0;

        for ( i=0; i < dvm_OneProcCount; i++ )
        {
            for ( j=0; j < dvm_OneProcCount; j++ )
            {
                t = ErrTimes2[i].cpu_num * dvm_OneProcCount + j;

                if ( FromToCount[t] > 0 )
                {
                    do
                    {
                        pmsg_send = table_At(MSG_SEND, &SendTable, FromToOffs[t]+FromToCount[t]-1); /* get the last item */
                        DBG_ASSERT(__FILE__, __LINE__, pmsg_send->snd_time != 0);

                        if (pmsg_send->snd_time < ErrTimes2[i].err_time)
                            break; /* last send was before err_time of this CPU */

                        if ( pmsg_send->snd_time < ErrTimes[j] )  /* ErrTimes[j] may be lowered? */
                        {
                            /* check if we can miss this search and lower ErrTimes[j] more? */
                            if ( FromToCount[t] > 1 )
                            {
                                pmsg_send2 = table_At(MSG_SEND, &SendTable, FromToOffs[t]+FromToCount[t]- 2 /*!*/);
                                DBG_ASSERT(__FILE__, __LINE__, pmsg_send2->snd_time != 0);

                                /* there are other "useful" messages? A little optimization .. */
                                if ( (pmsg_send2->snd_time >= ErrTimes2[i].err_time) &&
                                      (pmsg_send2->msg_tag == pmsg_send->msg_tag) &&
                                      (pmsg_send2->msg_comm == pmsg_send->msg_comm) )
                                {
                                     FromToCount[t]--;
                                     continue;
                                }
                            }

                            if ( !pmsg_send->rcv_time )
                            {
                                /* need matching facilities to get exact error time for CPU j */
                                do_matching( &SendTable, FromToOffs[t], FromToOffs[t]+FromToCount[t],
                                             &RecvTable, RecvOffs[j],   RecvOffs[j]+RecvCount[j],
                                             pmsg_send->msg_tag, pmsg_send->msg_comm);
                            }

                            if ( !pmsg_send->rcv_time )
                            {
/*  */                          if ( 1 /*print_no_recv */ )
                                {
                                    pprintf(3, "*** RTS warn: There was no matching receive found.\n");
                                    fflush (stderr);
                                    print_no_recv  = 0;
                                }
                                FromToCount[t]--;
                                continue;
                            }

                            if ( ErrTimes[j] > pmsg_send->rcv_time )
                            {
                                ErrTimes[j] = pmsg_send->rcv_time;
                                for_exit = 1; /* implement fast restoration to the required CPU ? */
                            }
                        }

                        FromToCount[t]--;

                    } while ( FromToCount[t] > 0 );
                }
            }

            if ( for_exit )
                break;
        }

        fin = !for_exit && (i == dvm_OneProcCount) && (j == dvm_OneProcCount);
    }

    table_Done(&SendTable);
    table_Done(&RecvTable);

    dvm_FreeArray(ErrTimes2);
    dvm_FreeArray(RecvOffs);
    dvm_FreeArray(RecvCount);
    dvm_FreeArray(FromToOffs);
    dvm_FreeArray(FromToCount);

    /* final ErrTimes are defined */
    Trace.ErrTimes = ErrTimes; /* no memory free operation */
    /* dvm_??Array(ErrTimes); */

    return ;
}



#endif  /* _MPI_PROF_TRAN_ */


#endif  /* _TRACER_DVMDBG_C_ */
