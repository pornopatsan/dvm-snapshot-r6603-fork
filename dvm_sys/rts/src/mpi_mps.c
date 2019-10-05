#ifndef _MPI_MPS_C_
#define _MPI_MPS_C_
/*****************/    /*E0000*/

extern int        MPI_ProcCount;
extern int        MPI_CurrentProcIdent;
extern int       *MPI_NumberList;
extern MPI_Comm   DVM_COMM_WORLD;



void  __callstd  dvmcom_(void  *DVMCommPtr)
{
  DVMFTimeStart(call_dvmcom_);

  if(RTL_TRACE)
     dvm_trace(call_dvmcom_,"DVMCommPtr=%lx;\n", (ulng)DVMCommPtr);

  SYSTEM(MPI_Comm_dup, (DVM_VMS->PS_MPI_COMM, (MPI_Comm *)DVMCommPtr))

  if(RTL_TRACE)
     dvm_trace(ret_dvmcom_," \n");

  DVMFTimeFinish(ret_dvmcom_);

  (DVM_RET);
  return;
}



int  mps_Send(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ 
  if(MPIInfoPrint && StatOff == 0)
  {  if(UserSumFlag != 0)
     {  MPISendByteCount  += ByteCount;
        MPIMsgCount++;
        MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, ByteCount);
        MinMPIMsgLen = dvm_min(MinMPIMsgLen, ByteCount);
     }
     else
     {  MPISendByteCount0 += ByteCount;
        MPIMsgCount0++;
        MaxMPIMsgLen0 = dvm_max(MaxMPIMsgLen0, ByteCount);
        MinMPIMsgLen0 = dvm_min(MinMPIMsgLen0, ByteCount);
     }
  }

  SYSTEM(MPI_Send, (BufPtr, ByteCount, MPI_BYTE,
                    ProcIdent, msg_SynchroSendRecv,
                    DVM_COMM_WORLD))
  return  1;
}



int  mps_Recv(int  ProcIdent, void  *BufPtr, int  ByteCount)
{
  SYSTEM(MPI_Recv, (BufPtr, ByteCount, MPI_BYTE,
                    ProcIdent, msg_SynchroSendRecv,
                    DVM_COMM_WORLD, &GMPI_Status))
  return  1;
}



int  mps_Sendnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ byte  IssendSign = 0;
  int   rc;

  if(MPIInfoPrint && StatOff == 0)
  {  if(UserSumFlag != 0)
     {  MPISendByteCount  += ByteCount;
        MPIMsgCount++;
        MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, ByteCount);
        MinMPIMsgLen = dvm_min(MinMPIMsgLen, ByteCount);
     }
     else
     {  MPISendByteCount0 += ByteCount;
        MPIMsgCount0++;
        MaxMPIMsgLen0 = dvm_max(MaxMPIMsgLen0, ByteCount);
        MinMPIMsgLen0 = dvm_min(MinMPIMsgLen0, ByteCount);
     }
  }

  if(MPI_Issend_sign) /* if function MPI_Issend
                         may be used */    /*E0001*/
  {  if(ByteCount < IssendMsgLength)
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           ProcIdent,
                           Tag, DVM_COMM_WORLD, &RTL_ReqPtr->MPSFlag))
     else
     {  SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                            ProcIdent,
                            Tag, DVM_COMM_WORLD, &RTL_ReqPtr->MPSFlag))
        IssendSign = 1;
     }
  }
  else
  {  SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                        ProcIdent,
                        Tag, DVM_COMM_WORLD, &RTL_ReqPtr->MPSFlag))
  }

  if(RTL_TRACE && MPI_RequestTrace)
  {  if(IssendSign)
        tprintf("*** MPI_Issend: RTL_RequestPtr=%lx\n",
                (ulng)RTL_ReqPtr);
     else
        tprintf("*** MPI_Isend: RTL_RequestPtr=%lx\n",
                (ulng)RTL_ReqPtr);
  }

  /* Save pointer to exchange flag in buffer */    /*E0002*/

  if(dopl_MPI_Test && MsgSchedule == 0 && RequestCount < ReqBufSize)
  {  RequestBuffer[RequestCount] = RTL_ReqPtr;
     RequestCount++;
  }

  if(MPITestAfterSend == 1 || MPITestAfterSend == 3)
     SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  return  1;
}



int  mps_Sendnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ byte  IssendSign = 0;
  int   rc;

  if(MPIInfoPrint && StatOff == 0)
  {  if(UserSumFlag != 0)
     {  MPISendByteCount  += ByteCount;
        MPIMsgCount++;
        MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, ByteCount);
        MinMPIMsgLen = dvm_min(MinMPIMsgLen, ByteCount);
     }
     else
     {  MPISendByteCount0 += ByteCount;
        MPIMsgCount0++;
        MaxMPIMsgLen0 = dvm_max(MaxMPIMsgLen0, ByteCount);
        MinMPIMsgLen0 = dvm_min(MinMPIMsgLen0, ByteCount);
     }
  }

  if(MPI_Issend_sign) /* */    /*E0003*/
  {  if(ByteCount < IssendMsgLength)
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           ProcIdent,
                           Tag, DVM_COMM_WORLD, ReqPtr))
     else
     {  SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                            ProcIdent,
                            Tag, DVM_COMM_WORLD, ReqPtr))
        IssendSign = 1;
     }
  }
  else
  {  SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                        ProcIdent,
                        Tag, DVM_COMM_WORLD, ReqPtr))
  }

  if(RTL_TRACE && MPI_RequestTrace)
  {  if(IssendSign)
        tprintf("*** MPI_Issend: MPI_RequestPtr=%lx\n", (ulng)ReqPtr);
     else
        tprintf("*** MPI_Isend: MPI_RequestPtr=%lx\n", (ulng)ReqPtr);
  }

  /* */    /*E0004*/

  if(dopl_MPI_Test && MsgSchedule == 0 &&
     MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterSend == 1 || MPITestAfterSend == 3)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  return  1;
}



int  mps_Recvnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ int   rc;

  SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                     ProcIdent,
                     Tag, DVM_COMM_WORLD, &RTL_ReqPtr->MPSFlag))

  if(RTL_TRACE && MPI_RequestTrace)
     tprintf("*** MPI_Irecv: RTL_RequestPtr=%lx\n", (ulng)RTL_ReqPtr);

  /* Save pointer to exchange flag in buffer */    /*E0005*/

  if(dopl_MPI_Test && RequestCount < ReqBufSize)
  {  RequestBuffer[RequestCount] = RTL_ReqPtr;
     RequestCount++;
  }

  if(MPITestAfterRecv == 1 || MPITestAfterRecv == 3)
     SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  return  1;
}



int  mps_Recvnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ int   rc;

  SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                     ProcIdent,
                     Tag, DVM_COMM_WORLD, ReqPtr))

  if(RTL_TRACE && MPI_RequestTrace)
     tprintf("*** MPI_Irecv: MPI_RequestPtr=%lx\n", (ulng)ReqPtr);

  /* */    /*E0006*/

  if(dopl_MPI_Test && MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterRecv == 1 || MPITestAfterRecv == 3)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  return  1;
}



void  mps_Waitrequest(RTL_Request  *RTL_ReqPtr)
{ int  i;

  SYSTEM(MPI_Wait, (&RTL_ReqPtr->MPSFlag, &RTL_ReqPtr->Status))

  if(RTL_TRACE && MPI_RequestTrace)
     tprintf("*** MPI_Wait: RTL_RequestPtr=%lx;\n", (ulng)RTL_ReqPtr);

  /* Delete pointer to exchange flag from buffer */    /*E0007*/

  for(i=0; i < RequestCount; i++)
  {  if(RequestBuffer[i] == RTL_ReqPtr)
        break;
  }

  if(i < RequestCount)
  {  /* Pointer to exchange flag has been found */    /*E0008*/

     for(i++; i < RequestCount; i++)
         RequestBuffer[i-1] = RequestBuffer[i];
     RequestCount--;
  }

  return;
}



void  mps_Waitrequest1(MPS_Request  *ReqPtr)
{ int  i;

  SYSTEM(MPI_Wait, (ReqPtr, &GMPI_Status))

  if(RTL_TRACE && MPI_RequestTrace)
     tprintf("*** MPI_Wait: MPI_RequestPtr=%lx\n", (ulng)ReqPtr);

  /* */    /*E0009*/

  for(i=0; i < MPS_RequestCount; i++)
  {  if(MPS_RequestBuffer[i] == ReqPtr)
        break;
  }

  if(i < MPS_RequestCount)
  {  /* */    /*E0010*/

     for(i++; i < MPS_RequestCount; i++)
         MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
     MPS_RequestCount--;
  }

  return;
}



int  mps_Testrequest(RTL_Request  *RTL_ReqPtr)
{ int  rc, i;

  SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  /* Delete pointer to exchange flag from buffer, if exchange is finished */    /*E0011*/

  if(rc)
  {  for(i=0; i < RequestCount; i++)
     {  if(RequestBuffer[i] == RTL_ReqPtr)
           break;
     }

     if(i < RequestCount)
     {  /* Pointer to exchange flag has been found */    /*E0012*/

        for(i++; i < RequestCount; i++)
            RequestBuffer[i-1] = RequestBuffer[i];
        RequestCount--;
     }
  }

  return  rc;
}



int  mps_Testrequest1(MPS_Request  *ReqPtr)
{ int  i, rc;

  SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  /* */    /*E0013*/

  if(rc)
  {  for(i=0; i < MPS_RequestCount; i++)
     {  if(MPS_RequestBuffer[i] == ReqPtr)
           break;
     }

     if(i < MPS_RequestCount)
     {  /* */    /*E0014*/

        for(i++; i < MPS_RequestCount; i++)
            MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
        MPS_RequestCount--;
     }
  }

  return  rc;
}



int  mps_SendA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{ 
  if(MPIInfoPrint && StatOff == 0)
  {  if(UserSumFlag != 0)
     {  MPISendByteCount  += ByteCount;
        MPIMsgCount++;
        MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, ByteCount);
        MinMPIMsgLen = dvm_min(MinMPIMsgLen, ByteCount);
     }
     else
     {  MPISendByteCount0 += ByteCount;
        MPIMsgCount0++;
        MaxMPIMsgLen0 = dvm_max(MaxMPIMsgLen0, ByteCount);
        MinMPIMsgLen0 = dvm_min(MinMPIMsgLen0, ByteCount);
     }
  }

  SYSTEM(MPI_Bsend, (BufPtr, ByteCount, MPI_BYTE,
                     ProcIdent,
                     Tag, DVM_COMM_WORLD))
  return  1;
}



int  mps_RecvA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{
  SYSTEM(MPI_Recv, (BufPtr, ByteCount, MPI_BYTE,
                    ProcIdent,
                    Tag, DVM_COMM_WORLD, &GMPI_Status))
  return  1;
}



void  mps_exit(int rc)
{
  int           i;

  MPI_BotsulaProf = 0;  /* */    /*E0015*/
  MPI_BotsulaDeb = 0;   /* */    /*E0016*/
  RTS_Call_MPI = 1;     /* */    /*E0017*/

  /* */    /*E0018*/

  if(ExclProcWait && MPI_CurrentProcIdent == 0 && IAmIOProcess == 0)
  {
     for(i=ProcCount; i < MPI_ProcCount; i++)
     {
        SYSTEM(MPI_Send, (&i, 1, MPI_INT, MPI_NumberList[i],
                          msg_SynchroSendRecv, APPL_COMM_WORLD))
     }
  }

  RTS_Call_MPI = 1;

#if !defined(_MPI_PROF_EXT_) && defined(_DVM_MPI_PROF_)
    SYSTEM(PMPI_Finalize, ())
#else
    SYSTEM(MPI_Finalize, ())
#endif

  SYSTEM(exit, (rc))
}



void  mps_Bcast(void *buf, int count, int size)
{
  char         *_buf = (char *)buf;
  int           ByteCount, alloc_buf = 0;

  if(RTL_TRACE)
     dvm_trace(call_mps_Bcast, "buf=%lx; count=%d; size=%d;\n",
                               (ulng)buf, count, size);

  if(IsInit)       /* if MPS has been initialized */    /*E0019*/
  {
     ByteCount = count * size;
     alloc_buf = (int)(ByteCount & Msk3);

     if(alloc_buf != 0)
     {
        mac_malloc(_buf, char *,
                   ByteCount + SizeDelta[alloc_buf], 0);
     }

     if(alloc_buf)
        SYSTEM(memcpy, (_buf, buf, ByteCount))

     if(MPIInfoPrint && StatOff == 0 &&
        CurrentProcIdent == MasterProcIdent)
     {
        if(UserSumFlag != 0)
           MPISendByteCount  += ByteCount + SizeDelta[alloc_buf];
        else
           MPISendByteCount0 += ByteCount + SizeDelta[alloc_buf];
     }
     if(MPIInfoPrint && StatOff == 0)
        MPI_BcastTime -= dvm_time();

     SYSTEM(MPI_Bcast, (_buf, ByteCount + SizeDelta[alloc_buf],
                        MPI_BYTE, MasterProcIdent, DVM_COMM_WORLD))

     if(MPIInfoPrint && StatOff == 0)
        MPI_BcastTime += dvm_time();

     if(alloc_buf)
     {
        SYSTEM(memcpy, (buf, _buf, ByteCount))
        mac_free(&_buf);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_mps_Bcast," \n");

  (DVM_RET);

  return;
}



void  mps_Barrier(void)
{
  if(RTL_TRACE)
     dvm_trace(call_mps_Barrier," \n");

  if(MPIInfoPrint && StatOff == 0)
     MPI_BarrierTime -= dvm_time();

  if(IsInit)         /* if MPS has been initialized */    /*E0020*/
     SYSTEM(MPI_Barrier, (DVM_COMM_WORLD))

  if(MPIInfoPrint && StatOff == 0)
     MPI_BarrierTime += dvm_time();

  if(RTL_TRACE)
     dvm_trace(ret_mps_Barrier," \n");

  (DVM_RET);

  return;
}


#endif /* _MPI_MPS_C_ */    /*E0021*/
