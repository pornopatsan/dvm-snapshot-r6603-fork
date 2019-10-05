#ifndef _MSGPAS_C_
#define _MSGPAS_C_
/****************/    /*E0000*/


int  rtl_Send(void *buf, int count, int size, int procnum)
{ int    rc, MsgLen, MsgNumber, Remainder, MsgLength;
  char  *CharPtr = (char *)buf;

  DVMMTimeStart(call_rtl_Send);
 
  if(RTL_TRACE)
     dvm_trace(call_rtl_Send,"buf=%lx; count=%d; size=%d; "
                             "procnum=%d(%d); procid=%d;\n",
                              (uLLng)buf, count, size, procnum,
                              ProcNumberList[procnum],
                              ProcIdentList[procnum]);

  rc = count * size;

  SendRendezvous(procnum);          /* check that only one exchange between 
                                       two processors is allowed  */    /*E0001*/

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, rc, call_rtl_Send); /* print incoming message */    /*e0002*/

  if(IsSynchr && UserSumFlag)
  {  CharPtr += rc;
     SYSTEM(memcpy,
     (CharPtr, dvm_time_ptr, sizeof(double)) ) /* copy time of the beginning
                                                  of exchange */    /*E0003*/
     MsgLen = rc + sizeof(double) + SizeDelta[rc & Msk3];
  }
  else
     MsgLen = rc + SizeDelta[rc & Msk3];

  #ifdef _DVM_MPI_

    if(MPIInfoPrint && StatOff == 0)
    {  if(UserSumFlag != 0)
          MPISendByteNumber  += MsgLen;
       else
          MPISendByteNumber0 += MsgLen;
    }

  #endif

  if(MsgPartReg > 1 && MaxMsgLength > 0 && UserSumFlag &&
     MsgLen > MaxMsgLength && CheckRendezvous == 0)
  {  /* */    /*E0004*/

     MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0005*/
     Remainder = MsgLen % MaxMsgLength; /* */    /*E0006*/
     MsgLength = MaxMsgLength;          /* */    /*E0007*/

     if(MaxMsgParts)
     {  /* */    /*E0008*/

        if(Remainder)
           rc = MsgNumber + 1;
        else
           rc = MsgNumber;

        if(rc > MaxMsgParts)
        {  /* */    /*E0009*/

           MsgNumber = MaxMsgParts;
           MsgLength = MsgLen / MaxMsgParts;
           Remainder = MsgLen % MaxMsgParts;

           if(Remainder)
           {  MsgNumber--;
              Remainder += MsgLength;
           }

           if(Msk3)
           {  rc = MsgLength % 4;

              if(rc)
              {  /* */    /*E0010*/

                 MsgLength += (4-rc); /* */    /*E0011*/
                 MsgNumber = MsgLen / MsgLength; /* */    /*E0012*/
                 Remainder = MsgLen % MsgLength; /* */    /*E0013*/
              }
           }
        }
     }

     if(RTL_TRACE && MsgPartitionTrace && TstTraceEvent(call_rtl_Send))
        tprintf("*** MsgPartitionTrace. Send: "
                "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                MsgNumber, MsgLength, Remainder);

     CharPtr = (char *)buf;

     for(; MsgNumber > 0; MsgNumber--)
     {  rc = mps_Send(ProcIdentList[procnum], CharPtr, MsgLength);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.000: mps_Send rc = %d\n", rc);
        CharPtr += MsgLength;
     }

     if(Remainder)
     {  rc = mps_Send(ProcIdentList[procnum], CharPtr, Remainder);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.000: mps_Send rc = %d\n", rc);
     }
  }
  else
  {  /* */    /*E0014*/

     rc = mps_Send(ProcIdentList[procnum], buf, MsgLen);

     if(rc < 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 210.000: mps_Send rc = %d\n", rc);
  }
 
  if(RTL_TRACE)
     dvm_trace(ret_rtl_Send,"rc=%d;\n", rc);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  rtl_Recv(void *buf, int count, int size, int procnum)
{ int    rc, ByteCount, MsgLen, MsgNumber, Remainder, MsgLength;
  char  *CharPtr = (char *)buf, *CharPtr1;

  DVMMTimeStart(call_rtl_Recv);

  if(RTL_TRACE)
     dvm_trace(call_rtl_Recv,"buf=%lx; count=%d; size=%d; "
                             "procnum=%d(%d); procid=%d;\n",
                              (uLLng)buf, count, size, procnum,
                              ProcNumberList[procnum],
                              ProcIdentList[procnum]);

  ByteCount = count * size;

  RecvRendezvous(procnum);  /* check that only one exchange between 
                               two processors is allowed */    /*E0015*/

  if(IsSynchr && UserSumFlag)
     MsgLen = ByteCount + sizeof(double) + SizeDelta[ByteCount & Msk3];
  else
     MsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  if(MsgPartReg > 1 && MaxMsgLength > 0 && UserSumFlag &&
     MsgLen > MaxMsgLength && CheckRendezvous == 0)
  {  /* */    /*E0016*/

     MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0017*/
     Remainder = MsgLen % MaxMsgLength; /* */    /*E0018*/
     MsgLength = MaxMsgLength;          /* */    /*E0019*/

     if(MaxMsgParts)
     {  /* */    /*E0020*/

        if(Remainder)
           rc = MsgNumber + 1;
        else
           rc = MsgNumber;

        if(rc > MaxMsgParts)
        {  /* */    /*E0021*/

           MsgNumber = MaxMsgParts;
           MsgLength = MsgLen / MaxMsgParts;
           Remainder = MsgLen % MaxMsgParts;

           if(Remainder)
           {  MsgNumber--;
              Remainder += MsgLength;
           }

           if(Msk3)
           {  rc = MsgLength % 4;

              if(rc)
              {  /* */    /*E0022*/

                 MsgLength += (4-rc); /* */    /*E0023*/
                 MsgNumber = MsgLen / MsgLength; /* */    /*E0024*/
                 Remainder = MsgLen % MsgLength; /* */    /*E0025*/
              }
           }
        }
     }

     if(RTL_TRACE && MsgPartitionTrace && TstTraceEvent(call_rtl_Recv))
        tprintf("*** MsgPartitionTrace. Recv: "
                "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                MsgNumber, MsgLength, Remainder);

     CharPtr1 = (char *)buf;

     for(; MsgNumber > 0; MsgNumber--)
     {  rc = mps_Recv(ProcIdentList[procnum], CharPtr1, MsgLength);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.001: mps_Recv rc = %d\n", rc);
        CharPtr1 += MsgLength;
     }

     if(Remainder)
     {  rc = mps_Recv(ProcIdentList[procnum], CharPtr1, Remainder);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.001: mps_Recv rc = %d\n", rc);
     }
  }
  else
  {  /* */    /*E0026*/

     rc = mps_Recv(ProcIdentList[procnum], buf, MsgLen);

     if(rc < 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 210.001: mps_Recv rc = %d\n", rc);
  }

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, ByteCount,
                     call_rtl_Recv); /* Print received array */    /*e0027*/

  /* Calculate dissynchronization time */    /*E0028*/

  if(IsSynchr && UserSumFlag)
  {  CharPtr += ByteCount;
     SYSTEM(memcpy, (synchr_time_ptr, CharPtr, sizeof(double)))
     Curr_synchr_time = dvm_abs(Curr_synchr_time - Curr_dvm_time);
  }
  else
     Curr_synchr_time = 0.;
 
  if(RTL_TRACE)
     dvm_trace(ret_rtl_Recv,"rc=%d;\n", rc);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  rtl_Sendnowait(void *buf, int count, int size, int procnum,
                    int tag, RTL_Request *RTL_ReqPtr, int MsgPartition)
{ int           rc, MsgLen, MsgNumber, Remainder, i, MsgLength;
  char         *CharPtr = (char *)buf;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;
  void         *MsgBuf;
  byte          Part = 1;

#ifdef _DVM_MPI_

  char   *CharPtr0, *CharPtr1, *CharPtr2, *CharPtr3, *CharPtr4;
  int     j, k, m, n, CompressLen;
  byte    IsCompress = 0;

#endif

#if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  Bytef   *gzBuf;
  uLongf   gzLen;
  int      MsgCompressLev;

#endif

  DVMMTimeStart(call_rtl_Sendnowait);

  if(RTL_TRACE)
     dvm_trace(call_rtl_Sendnowait,
               "buf=%lx; count=%d; size=%d; req=%lx; procnum=%d(%d); "
               "procid=%d; tag=%d; MsgPartition=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
               ProcNumberList[procnum], ProcIdentList[procnum], tag,
               MsgPartition);

  NoWaitCount++;  /* */    /*E0029*/

  rc = count * size;

  SendRendezvous(procnum);  /* check that only one exchange between 
                               two processors is allowed */    /*E0030*/

  RTL_ReqPtr->SendRecvTime = -1.; /* negative time of the beginning
                                     of exchange - rtl_Sendnowait sign */    /*E0031*/

  if(IsSynchr && UserSumFlag)
  {  CharPtr += rc;
     SYSTEM(memcpy,
     (CharPtr, dvm_time_ptr, sizeof(double)) ) /* copy the  time of the beginning
                                                  of exchange */    /*E0032*/
     MsgLen = rc + sizeof(double) + SizeDelta[rc & Msk3];
  }
  else
     MsgLen = rc + SizeDelta[rc & Msk3];

  #ifdef _DVM_MPI_

    if(MPIInfoPrint && StatOff == 0)
    {  if(UserSumFlag != 0)
          MPISendByteNumber  += MsgLen;
       else
          MPISendByteNumber0 += MsgLen;
    }

  #endif

  /* */    /*E0033*/

  RTL_ReqPtr->CompressBuf = NULL;  /* */    /*E0034*/
  RTL_ReqPtr->ResCompressIndex = -1; 
  MsgBuf = buf;

  #ifdef _DVM_MPI_

  CompressLen = MsgLen;   /* */    /*E0035*/

  if(MsgDVMCompress != 0 && MsgLen > MinMsgCompressLength &&
     UserSumFlag != 0 && (rc % sizeof(double)) == 0)
  {  /* */    /*E0036*/

     /* */    /*E0037*/

     if(!( ( MsgSchedule && MsgLen > MaxMsgLength ) ||
           ( ((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
             MaxMsgLength > 0 && MsgLen > MaxMsgLength &&
             CheckRendezvous == 0 ) ) || MsgCompressWithMsgPart != 0)
     {
        j = MsgLen / sizeof(double);  /* */    /*E0038*/

        k = 1;
        CompressLen = 1 + sizeof(double);

        if(MsgDVMCompress < 2)
        {  /* */    /*E0039*/

           n = sizeof(double) - 1;

           if(InversByteOrder)
              CharPtr1 = (char *)buf + 1;
           else
              CharPtr1 = (char *)buf;
        }
        else
        {  /* */    /*E0040*/

           n = sizeof(double) - 2;

           
           if(InversByteOrder)
              CharPtr1 = (char *)buf + 2;
           else
              CharPtr1 = (char *)buf;
        }

        CharPtr3 = CharPtr1;
        CharPtr1 += sizeof(double);

        for(i=1; i < j; i++)
        {  for(m=0; m < n; m++)
               if(CharPtr1[m] != CharPtr3[m])
                  break;

           if(m == n && k < 255)
           {  /* */    /*E0041*/

              if(MsgDVMCompress > 1)
                 CompressLen += 2;
              else
                 CompressLen++;

              CharPtr1 += sizeof(double);
              k++;
           }
           else
           {  /* */    /*E0042*/

              k = 1;

              CharPtr3 = CharPtr1;
              CharPtr1 += sizeof(double);
              CompressLen += 1 + sizeof(double);
           }
        }

        CompressLen += SizeDelta[CompressLen & Msk3];
     }
  }

  IsCompress = (byte)((double)CompressLen / (double)MsgLen <=
                      CompressCoeff);

  if(MsgDVMCompress != 0 && MsgLen > MinMsgCompressLength &&
     UserSumFlag != 0 && (rc % sizeof(double)) == 0)
  {  /* */    /*E0043*/

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( ((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength &&
          CheckRendezvous == 0 ))
     {
        /* */    /*E0044*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0045*/

           if(IsCompress)
           {
              /* */    /*E0046*/

              j = MsgLen / sizeof(double);  /* */    /*E0047*/

              RTL_ReqPtr->ResCompressIndex =
              compress_malloc(&MsgBuf, CompressLen);

              MPIMsgNumber++;

              CharPtr2 = (char *)MsgBuf;
              CharPtr3 = CharPtr2 + 1;
              CharPtr4 = CharPtr3 + sizeof(double);
              k = 1;

              if(MsgDVMCompress < 2)
              {  /* */    /*E0048*/

                 n = sizeof(double) - 1;

                 if(InversByteOrder)
                 {  CharPtr0 = (char *)buf;
                    CharPtr1 = CharPtr0 + 1;
                 } 
                 else
                 {  CharPtr1 = (char *)buf;
                    CharPtr0 = CharPtr1 + n;
                 }
              }
              else
              {  /* */    /*E0049*/

                 n = sizeof(double) - 2;

                 if(InversByteOrder)
                 {  CharPtr0 = (char *)buf;
                    CharPtr1 = CharPtr0 + 2;
                 }
                 else
                 {  CharPtr1 = (char *)buf;
                    CharPtr0 = CharPtr1 + n;
                 }
              }

              SYSTEM(memcpy, (CharPtr3, CharPtr1, n))
              CharPtr3[n] = *CharPtr0;

              if(MsgDVMCompress > 1)
                 CharPtr3[n+1] = CharPtr0[1];

              CharPtr0 += sizeof(double);
              CharPtr1 += sizeof(double);

              for(i=1; i < j; i++)
              {  for(m=0; m < n; m++)
                     if(CharPtr1[m] != CharPtr3[m])
                        break;

                 if(m == n && k < 255)
                 {  /* */    /*E0050*/

                    *CharPtr4 = *CharPtr0;
                    CharPtr4++;

                    if(MsgDVMCompress > 1)
                    {  *CharPtr4 = CharPtr0[1];
                       CharPtr4++;
                    }

                    CharPtr0 += sizeof(double);
                    CharPtr1 += sizeof(double);
                    k++;
                 }
                 else
                 {  /* */    /*E0051*/

                    *CharPtr2 = (byte)k;
                    CharPtr2 = CharPtr4;
                    CharPtr3 = CharPtr2 + 1;
                    CharPtr4 = CharPtr3 + sizeof(double);
                    k = 1;

                    SYSTEM(memcpy, (CharPtr3, CharPtr1, n))
                    CharPtr3[n] = *CharPtr0;

                    if(MsgDVMCompress > 1)
                       CharPtr3[n+1] = CharPtr0[1];

                    CharPtr0 += sizeof(double);
                    CharPtr1 += sizeof(double);
                 }
              }

              *CharPtr2 = (byte)k;
           }

           if(RTL_TRACE && MsgCompressTrace)
           {  tprintf("*** MsgDVMCompress: branch=1; MsgLen=%d; "
                      "CompressLen=%d; CompressCode=%d;\n",
                      MsgLen, CompressLen, MsgDVMCompress);

              tprintf("    CompressWithPart=%d\n",
                      MsgCompressWithMsgPart);
           }

           if(IsCompress)
              MsgLen = CompressLen;

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0052*/

              /* */    /*E0053*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0054*/

              i = mps_Sendnowait(ProcIdentList[procnum], (void *)&MsgLen,
                                 sizeof(int), RTL_ReqPtr, tag);

              /* */    /*E0055*/

              if(_SendRecvTime && StatOff == 0)
              {  CommTime        = dvm_time() - CommTime;
                 SendCallTime   += CommTime;
                 MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
                 MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
                 SendCallCount++;

                 if(RTL_STAT)
                 {  SendRecvTimesPtr = (s_SendRecvTimes *)
                    &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                    SendRecvTimesPtr->SendCallTime    += CommTime;
                    SendRecvTimesPtr->MinSendCallTime  =
                    dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                    SendRecvTimesPtr->MaxSendCallTime  =
                    dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                    SendRecvTimesPtr->SendCallCount++;
                 }
              }

              if(i < 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 210.002: mps_Sendnowait rc = %d\n",
                         i);

              mps_Waitrequest(RTL_ReqPtr);
           }
           else
              Part = 0; /* */    /*E0056*/

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0057*/
           if(IsCompress)
              RTL_ReqPtr->CompressBuf = (char *)MsgBuf; 
        }
     }
     else
     {  /* */    /*E0058*/

        if(IsCompress)
        {
           /* */    /*E0059*/

           j = MsgLen / sizeof(double);  /* */    /*E0060*/

           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, CompressLen);

           MPIMsgNumber++;

           CharPtr2 = (char *)MsgBuf;
           CharPtr3 = CharPtr2 + 1;
           CharPtr4 = CharPtr3 + sizeof(double);
           k = 1;

           if(MsgDVMCompress < 2)
           {  /* */    /*E0061*/

              n = sizeof(double) - 1;

              if(InversByteOrder)
              {  CharPtr0 = (char *)buf;
                 CharPtr1 = CharPtr0 + 1;
              }
              else
              {  CharPtr1 = (char *)buf;
                 CharPtr0 = CharPtr1 + n;
              }
           }
           else
           {  /* */    /*E0062*/

              n = sizeof(double) - 2;

              if(InversByteOrder)
              {  CharPtr0 = (char *)buf;
                 CharPtr1 = CharPtr0 + 2;
              }
              else
              {  CharPtr1 = (char *)buf;
                 CharPtr0 = CharPtr1 + n;
              }
           }

           SYSTEM(memcpy, (CharPtr3, CharPtr1, n))
           CharPtr3[n] = *CharPtr0;

           if(MsgDVMCompress > 1)
              CharPtr3[n+1] = CharPtr0[1];

           CharPtr0 += sizeof(double);
           CharPtr1 += sizeof(double);

           for(i=1; i < j; i++)
           {  for(m=0; m < n; m++)
                  if(CharPtr1[m] != CharPtr3[m])
                     break;

              if(m == n && k < 255)
              {  /* */    /*E0063*/

                 *CharPtr4 = *CharPtr0;
                 CharPtr4++;

                 if(MsgDVMCompress > 1)
                 {  *CharPtr4 = CharPtr0[1];
                    CharPtr4++;
                 }

                 CharPtr0 += sizeof(double);
                 CharPtr1 += sizeof(double);
                 k++;
              }
              else
              {  /* */    /*E0064*/

                 *CharPtr2 = (byte)k;
                 CharPtr2 = CharPtr4;
                 CharPtr3 = CharPtr2 + 1;
                 CharPtr4 = CharPtr3 + sizeof(double);
                 k = 1;

                 SYSTEM(memcpy, (CharPtr3, CharPtr1, n))
                 CharPtr3[n] = *CharPtr0;

                 if(MsgDVMCompress > 1)
                    CharPtr3[n+1] = CharPtr0[1];

                 CharPtr0 += sizeof(double);
                 CharPtr1 += sizeof(double);
              }
           }

           *CharPtr2 = (byte)k;
        }

        if(RTL_TRACE && MsgCompressTrace)
           tprintf("*** MsgDVMCompress: branch=2; MsgLen=%d; "
                   "CompressLen=%d; CompressCode=%d;\n",
                   MsgLen, CompressLen, MsgDVMCompress);

        if(IsCompress)
           MsgLen = CompressLen;

        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0065*/
        if(IsCompress)
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  #if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
     (MsgDVMCompress == 0 || (rc % sizeof(double)) != 0) &&
     MsgLen > MinMsgCompressLength && UserSumFlag != 0)
  {  /* */    /*E0066*/

     MsgCompressLev = MsgCompressLevel;  /* */    /*E0067*/

     if(MsgCompressLev == 0)
     {  /* */    /*E0068*/

        if(CompressLevel == 0)
           MsgCompressLev = Z_DEFAULT_COMPRESSION;
        else
           MsgCompressLev = CompressLevel;
     }

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( ((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength &&
          CheckRendezvous == 0 ))
     {
        /* */    /*E0069*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0070*/

           /* */    /*E0071*/

           i = MsgLen + MsgLen/50 + 12;

           RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

           MPIMsgNumber++;

           gzBuf = (Bytef *)MsgBuf;
           gzLen = (uLongf)i;

           SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                       (uLong)MsgLen, MsgCompressLev))
/*           SYSTEM_RET(i, compress2, (gzBuf, &gzLen, (Bytef *)buf,
                                       (uLong)MsgLen, MsgCompressLev))*/    /*E0072*/

           if(i != Z_OK)
           {  /* */    /*E0073*/

              if(ZLIB_Warning)
                 pprintf(2+MultiProcErrReg2,
                         "*** RTS warning 210.010: compress2 rc = %d\n",
                         i);

              mac_free(&MsgBuf);

              i = RTL_ReqPtr->ResCompressIndex;
              RTL_ReqPtr->ResCompressIndex = -1;

              if(i >= 0)
              {  CompressBuf[i]     = NULL;
                 CompressBufSize[i] = 0;
                 FreeCompressBuf[i] = 1;
              }

              i = MsgLen + MsgLen + 12;

              RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

              MPIMsgNumber++;

              gzBuf = (Bytef *)MsgBuf;
              gzLen = (uLongf)i;

              SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                          (uLong)MsgLen, MsgCompressLev))
/*              SYSTEM_RET(i, compress2, (gzBuf, &gzLen, (Bytef *)buf,
                                          (uLong)MsgLen, MsgCompressLev))*/    /*E0074*/
              if(i != Z_OK)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 210.010: compress2 rc = %d\n", i);
           }
               
           if(RTL_TRACE && MsgCompressTrace)
           {  tprintf("*** MsgCompress2: branch=1; rc=%d; MsgLen=%d; "
                      "gzLen=%ld; Level=%d;\n",
                      i, MsgLen, (DvmType)gzLen, MsgCompressLev);

              tprintf("    CompressWithPart=%d\n",
                      MsgCompressWithMsgPart);
           }

           MsgLen = (int)gzLen;

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0075*/

              /* */    /*E0076*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0077*/

              i = mps_Sendnowait(ProcIdentList[procnum], (void *)&MsgLen,
                                 sizeof(int), RTL_ReqPtr, tag);

              /* */    /*E0078*/

              if(_SendRecvTime && StatOff == 0)
              {  CommTime        = dvm_time() - CommTime;
                 SendCallTime   += CommTime;
                 MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
                 MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
                 SendCallCount++;

                 if(RTL_STAT)
                 {  SendRecvTimesPtr = (s_SendRecvTimes *)
                    &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                    SendRecvTimesPtr->SendCallTime    += CommTime;
                    SendRecvTimesPtr->MinSendCallTime  =
                    dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                    SendRecvTimesPtr->MaxSendCallTime  =
                    dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                    SendRecvTimesPtr->SendCallCount++;
                 }
              }

              if(i < 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 210.002: mps_Sendnowait rc = %d\n",
                         i);

              mps_Waitrequest(RTL_ReqPtr);
           }
           else
              Part = 0; /* */    /*E0079*/

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0080*/
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf; 
        }
     }
     else
     {  /* */    /*E0081*/

        /* */    /*E0082*/

        i = MsgLen + MsgLen/50 + 12;

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

        MPIMsgNumber++;

        gzBuf = (Bytef *)MsgBuf;
        gzLen = (uLongf)i;

        SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                    (uLong)MsgLen, MsgCompressLev))
/*        SYSTEM_RET(i, compress2, (gzBuf, &gzLen, (Bytef *)buf,
                                    (uLong)MsgLen, MsgCompressLev))*/    /*E0083*/

        if(i != Z_OK)
        {  /* */    /*E0084*/

           if(ZLIB_Warning)
              pprintf(2+MultiProcErrReg2,
                      "*** RTS warning 210.010: compress2 rc = %d\n", i);

           mac_free(&MsgBuf);

           i = RTL_ReqPtr->ResCompressIndex;
           RTL_ReqPtr->ResCompressIndex = -1;

           if(i >= 0)
           {  CompressBuf[i]     = NULL;
              CompressBufSize[i] = 0;
              FreeCompressBuf[i] = 1;
           }

           i = MsgLen + MsgLen + 12;

           RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

           MPIMsgNumber++;

           gzBuf = (Bytef *)MsgBuf;
           gzLen = (uLongf)i;

           SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                       (uLong)MsgLen, MsgCompressLev))
/*           SYSTEM_RET(i, compress2, (gzBuf, &gzLen, (Bytef *)buf,
                                       (uLong)MsgLen, MsgCompressLev))*/    /*E0085*/
           if(i != Z_OK)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.010: compress2 rc = %d\n", i);
        }

        if(RTL_TRACE && MsgCompressTrace)
           tprintf("*** MsgCompress2: branch=2; rc=%d; MsgLen=%d; "
                   "gzLen=%ld; Level=%d;\n",
                   i, MsgLen, (DvmType)gzLen, MsgCompressLev);

        MsgLen = (int)gzLen;

        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0086*/
        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  RTL_ReqPtr->SendSign = 1;         /* */    /*E0087*/
  RTL_ReqPtr->ProcNumber = procnum; /* */    /*E0088*/
  RTL_ReqPtr->BufAddr = (char *)buf;/* */    /*E0089*/
  RTL_ReqPtr->BufLength = rc;       /* */    /*E0090*/
  RTL_ReqPtr->FlagNumber = 0;       /* */    /*E0091*/
  RTL_ReqPtr->MPSFlagArray = NULL;  /* */    /*E0092*/
  RTL_ReqPtr->EndExchange = NULL;   /* */    /*E0093*/

  RTL_ReqPtr->MsgLength = 0; /* */    /*E0094*/
  RTL_ReqPtr->Remainder = 0; /* */    /*E0095*/
  RTL_ReqPtr->Init = 0;      /* */    /*E0096*/
  RTL_ReqPtr->Last = -1;     /* */    /*E0097*/
  RTL_ReqPtr->Chan = -1;     /* */    /*E0098*/
  RTL_ReqPtr->tag  = tag;    /* */    /*E0099*/
  RTL_ReqPtr->CurrOper = CurrOper; /* */    /*E0100*/

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, rc,
                     call_rtl_Sendnowait);   /* Print sent array */    /*E0101*/

  if(MsgSchedule && UserSumFlag)
  {  /* */    /*E0102*/

     if(MsgLen > MaxMsgLength && Part != 0)
     {  /* */    /*E0103*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0104*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0105*/
        MsgLength = MaxMsgLength;          /* */    /*E0106*/
        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0107*/

        dvm_AllocArray(MPS_Request, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->MPSFlagArray);

        RTL_ReqPtr->MsgLength = MsgLength;
        RTL_ReqPtr->Remainder = Remainder;
     }
     else
     {  /* */    /*E0108*/

        RTL_ReqPtr->FlagNumber = 1;
        RTL_ReqPtr->MsgLength = MsgLen;
        RTL_ReqPtr->Remainder = 0;

        dvm_AllocArray(MPS_Request, 1, RTL_ReqPtr->MPSFlagArray);

        MsgNumber = 1;
        MsgLength = MsgLen;
        Remainder = 0;
     }

     coll_Insert(&SendReqColl, RTL_ReqPtr); /* */    /*E0109*/
     NewMsgNumber++;      /* */    /*E0110*/

     if(MsgPartition == 0)
     {  /* */    /*E0111*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeff);
     }

     if(MsgPartition == s_DA_IOMem || MsgPartition == s_DA_EX_DA ||
        MsgPartition == s_DA1_EX_DA1 || MsgPartition == s_Elm_IOMem)
     {  /* */    /*E0112*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     if((MsgPartition == a_DA_EX_DA || MsgPartition == a_DA1_EX_DA1) &&
        DVM_LEVEL < 2)
     {  /* */    /*E0113*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     if(MsgPartition == a_DA_IOMem && DVM_LEVEL < 2)
     {  /* */    /*E0114*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffDACopy);
     }

     if(MsgPartition == a_Elm_IOMem && DVM_LEVEL < 2)
     {  /* */    /*E0115*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffElmCopy);
     }

     if(MsgPartition == RedNonCentral)
     {  /* */    /*E0116*/

        rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffRedNonCentral);
     }

     if(RTL_TRACE && MsgScheduleTrace &&
        TstTraceEvent(call_rtl_Sendnowait))
        tprintf("*** MsgScheduleTrace. Sendnowait:\n"
            "MsgNumber=%d MsgLength=%d Remainder=%d Init=%d Last=%d\n",
            MsgNumber, MsgLength, Remainder,
            RTL_ReqPtr->Init, RTL_ReqPtr->Last);
  }
  else
  {  /* */    /*E0117*/

     if(((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
        MaxMsgLength > 0 && UserSumFlag && MsgLen > MaxMsgLength &&
        CheckRendezvous == 0 && Part != 0)
     {  /* */    /*E0118*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0119*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0120*/
        MsgLength = MaxMsgLength;          /* */    /*E0121*/

        if(MaxMsgParts)
        {  /* */    /*E0122*/

           if(Remainder)
              rc = MsgNumber + 1;
           else
              rc = MsgNumber;

           if(rc > MaxMsgParts)
           {  /* */    /*E0123*/

              MsgNumber = MaxMsgParts;
              MsgLength = MsgLen / MaxMsgParts;
              Remainder = MsgLen % MaxMsgParts;

              if(Remainder)
              {  MsgNumber--;
                 Remainder += MsgLength;
              }

              if(Msk3)
              {  rc = MsgLength % 4;

                 if(rc)
                 {  /* */    /*E0124*/

                    MsgLength += (4-rc); /* */    /*E0125*/
                    MsgNumber = MsgLen / MsgLength; /* */    /*E0126*/
                    Remainder = MsgLen % MsgLength; /* */    /*E0127*/
                 }
              }
           }
        }

        if(RTL_TRACE && MsgPartitionTrace &&
           TstTraceEvent(call_rtl_Sendnowait))
           tprintf("*** MsgPartitionTrace. Sendnowait: "
                   "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                   MsgNumber, MsgLength, Remainder);

        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0128*/

        dvm_AllocArray(MPS_Request, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->MPSFlagArray);
        dvm_AllocArray(byte, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->EndExchange);

        for(i=0; i < RTL_ReqPtr->FlagNumber; i++)
            RTL_ReqPtr->EndExchange[i] = 0;

        CharPtr = (char *)MsgBuf;

        for(i=0; i < MsgNumber; i++)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();   /* for statistics */    /*E0129*/

           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                                tag);

           /* For statistics. rtl_Sendnowait. */    /*E0130*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              SendCallTime   += CommTime;
              MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
              MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
              SendCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->SendCallTime    += CommTime;
                 SendRecvTimesPtr->MinSendCallTime  =
                 dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                 SendRecvTimesPtr->MaxSendCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                 SendRecvTimesPtr->SendCallCount++;
              }
           }

           /* -------------- */    /*E0131*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(Remainder)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();  /* for statistics */    /*E0132*/

           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                                tag);

           /* For statistics. rtl_Sendnowait. */    /*E0133*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              SendCallTime   += CommTime;
              MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
              MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
              SendCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->SendCallTime    += CommTime;
                 SendRecvTimesPtr->MinSendCallTime  =
                 dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                 SendRecvTimesPtr->MaxSendCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                 SendRecvTimesPtr->SendCallCount++;
              }
           }

           /* -------------- */    /*E0134*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                      rc);
        }
     }
     else
     {  /* */    /*E0135*/

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0136*/

        rc = mps_Sendnowait(ProcIdentList[procnum], MsgBuf, MsgLen,
                            RTL_ReqPtr, tag);

        /* For statistics. rtl_Sendnowait. */    /*E0137*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           SendCallTime   += CommTime;
           MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
           MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
           SendCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->SendCallTime    += CommTime;
              SendRecvTimesPtr->MinSendCallTime  =
              dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
              SendRecvTimesPtr->MaxSendCallTime  =
              dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
              SendRecvTimesPtr->SendCallCount++;
           }
        }

        /* -------------- */    /*E0138*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.002: mps_Sendnowait rc = %d\n", rc);
     }
  }

  if(CheckRendezvous)
     SendRendezvousArray[procnum] = RTL_ReqPtr;/* processor with internal number procnum 
                                                 is busy */    /*E0139*/

  for(tl=0; tl < SendDelay; tl++) /* loop of special delay */    /*E0140*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_rtl_Sendnowait,"rc=%d; req=%lx;\n",
                                   rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  rtl_Recvnowait(void *buf, int count, int size, int procnum,
                    int tag, RTL_Request *RTL_ReqPtr, int MsgPartition)
{ int           rc, ByteCount, MsgLen, MsgNumber, Remainder, i,
                MsgLength;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;
  char         *CharPtr;
  void         *MsgBuf; 
  byte          Part = 1;

#if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  int  MsgCompressLev;

#endif

  DVMMTimeStart(call_rtl_Recvnowait);

  if(RTL_TRACE)
     dvm_trace(call_rtl_Recvnowait,
               "buf=%lx; count=%d; size=%d; req=%lx; procnum=%d(%d); "
               "procid=%ld; tag=%d; MsgPartition=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
               ProcNumberList[procnum], ProcIdentList[procnum], tag,
               MsgPartition);

  NoWaitCount++;  /* */    /*E0141*/

  ByteCount = count * size;

  RecvRendezvous(procnum);  /* check that only one exchange between 
                               two processors is allowed */    /*E0142*/

  RTL_ReqPtr->SendRecvTime = Curr_dvm_time; /* time of the beginning
                                               of receiving */    /*E0143*/

  if(IsSynchr && UserSumFlag)
     MsgLen = ByteCount + sizeof(double) + SizeDelta[ByteCount & Msk3];
  else
     MsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  /* */    /*E0144*/

  RTL_ReqPtr->CompressBuf = NULL;  /* */    /*E0145*/
  RTL_ReqPtr->ResCompressIndex = -1;
  MsgBuf = buf;

  #ifdef _DVM_MPI_

  if(MsgDVMCompress != 0 && MsgLen > MinMsgCompressLength &&
     UserSumFlag != 0 && (ByteCount % sizeof(double)) == 0)
  {  /* */    /*E0146*/

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( ((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength &&
          CheckRendezvous == 0 ))
     {
        /* */    /*E0147*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0148*/

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0149*/

              /* */    /*E0150*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0151*/

              rc = mps_Recvnowait(ProcIdentList[procnum],
                                  (void *)&MsgLen, sizeof(int),
                                  RTL_ReqPtr, tag);

              /* */    /*E0152*/

              if(_SendRecvTime && StatOff == 0)
              {  CommTime        = dvm_time() - CommTime;
                 RecvCallTime   += CommTime;
                 MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
                 MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
                 RecvCallCount++;

                 if(RTL_STAT)
                 {  SendRecvTimesPtr = (s_SendRecvTimes *)
                    &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                    SendRecvTimesPtr->RecvCallTime    += CommTime;
                    SendRecvTimesPtr->MinRecvCallTime  =
                    dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
                    SendRecvTimesPtr->MaxRecvCallTime  =
                    dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
                    SendRecvTimesPtr->RecvCallCount++;
                 }
              }

              if(rc < 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 210.003: mps_Recvnowait rc = %d\n",
                         rc);

              mps_Waitrequest(RTL_ReqPtr);

              RTL_ReqPtr->IsCompressSize = 1; /* */    /*E0153*/
           }
           else
           {  Part = 0; /* */    /*E0154*/
              RTL_ReqPtr->IsCompressSize = 0; /* */    /*E0155*/
           }

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, MsgLen);
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
        }
     }
     else
     {  /* */    /*E0156*/

        /* */    /*E0157*/

        MsgLen += MsgLen / sizeof(double); /* */    /*E0158*/
        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 0;  /* */    /*E0159*/

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, MsgLen);

        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  #if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
     (MsgDVMCompress == 0 || (ByteCount % sizeof(double)) != 0) &&
     MsgLen > MinMsgCompressLength && UserSumFlag != 0)
  {  /* */    /*E0160*/

     MsgCompressLev = MsgCompressLevel;  /* */    /*E0161*/

     if(MsgCompressLev == 0)
     {  /* */    /*E0162*/

        if(CompressLevel == 0)
           MsgCompressLev = Z_DEFAULT_COMPRESSION;
        else
           MsgCompressLev = CompressLevel;
     }

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( ((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength &&
          CheckRendezvous == 0 ))
     {
        /* */    /*E0163*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0164*/

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0165*/

              /* */    /*E0166*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0167*/

              rc = mps_Recvnowait(ProcIdentList[procnum],
                                  (void *)&MsgLen, sizeof(int),
                                  RTL_ReqPtr, tag);

              /* */    /*E0168*/

              if(_SendRecvTime && StatOff == 0)
              {  CommTime        = dvm_time() - CommTime;
                 RecvCallTime   += CommTime;
                 MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
                 MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
                 RecvCallCount++;

                 if(RTL_STAT)
                 {  SendRecvTimesPtr = (s_SendRecvTimes *)
                    &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                    SendRecvTimesPtr->RecvCallTime    += CommTime;
                    SendRecvTimesPtr->MinRecvCallTime  =
                    dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
                    SendRecvTimesPtr->MaxRecvCallTime  =
                    dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
                    SendRecvTimesPtr->RecvCallCount++;
                 }
              }

              if(rc < 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 210.003: mps_Recvnowait rc = %d\n",
                         rc);

              mps_Waitrequest(RTL_ReqPtr);

              RTL_ReqPtr->IsCompressSize = 1; /* */    /*E0169*/
           }
           else
           {  Part = 0; /* */    /*E0170*/
              RTL_ReqPtr->IsCompressSize = 0; /* */    /*E0171*/
           }

           RTL_ReqPtr->CompressMsgLen = MsgLen;

           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, MsgLen);

           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
        }
     }
     else
     {  /* */    /*E0172*/

        /* */    /*E0173*/

        MsgLen                     = MsgLen + MsgLen/50 + 12;
        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 0;  /* */    /*E0174*/

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, MsgLen);

        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  RTL_ReqPtr->SendSign = 0;         /* */    /*E0175*/
  RTL_ReqPtr->ProcNumber = procnum; /* */    /*E0176*/
  RTL_ReqPtr->BufAddr = (char *)buf;/* */    /*E0177*/
  RTL_ReqPtr->BufLength = ByteCount;/* */    /*E0178*/
  RTL_ReqPtr->FlagNumber = 0;       /* */    /*E0179*/
  RTL_ReqPtr->MPSFlagArray = NULL;  /* */    /*E0180*/
  RTL_ReqPtr->EndExchange = NULL;   /* */    /*E0181*/

  RTL_ReqPtr->MsgLength = 0; /* */    /*E0182*/
  RTL_ReqPtr->Remainder = 0; /* */    /*E0183*/
  RTL_ReqPtr->Init = 0;      /* */    /*E0184*/
  RTL_ReqPtr->Last = -1;     /* */    /*E0185*/
  RTL_ReqPtr->Chan = -1;     /* */    /*E0186*/
  RTL_ReqPtr->tag  = tag;    /* */    /*E0187*/ 
  RTL_ReqPtr->CurrOper = CurrOper; /* */    /*E0188*/

  if(MsgSchedule && UserSumFlag)
  {  /* */    /*E0189*/

     if(MsgLen > MaxMsgLength && Part != 0)
     {  /* */    /*E0190*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0191*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0192*/
        MsgLength = MaxMsgLength;          /* */    /*E0193*/
        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0194*/

        dvm_AllocArray(MPS_Request, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->MPSFlagArray);
        dvm_AllocArray(byte, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->EndExchange);

        for(i=0; i < RTL_ReqPtr->FlagNumber; i++)
            RTL_ReqPtr->EndExchange[i] = 0;

        RTL_ReqPtr->MsgLength = MsgLength;
        RTL_ReqPtr->Remainder = Remainder;
     }
     else
     {  /* */    /*E0195*/

        RTL_ReqPtr->FlagNumber = 1;
        RTL_ReqPtr->MsgLength = MsgLen;
        RTL_ReqPtr->Remainder = 0;

        dvm_AllocArray(MPS_Request, 1, RTL_ReqPtr->MPSFlagArray);
        dvm_AllocArray(byte, 1, RTL_ReqPtr->EndExchange);

        RTL_ReqPtr->EndExchange[0] = 0;

        MsgNumber = 1;
        MsgLength = MsgLen;
        Remainder = 0;
     }

     RTL_ReqPtr->Init = 0;
     RTL_ReqPtr->Last = RTL_ReqPtr->FlagNumber - 1;

     if(RTL_TRACE && MsgScheduleTrace &&
        TstTraceEvent(call_rtl_Recvnowait))
        tprintf("*** MsgScheduleTrace. Recvnowait:\n"
             "MsgNumber=%d MsgLength=%d Remainder=%d Init=0 Last=%d\n",
             MsgNumber, MsgLength, Remainder, RTL_ReqPtr->Last);

     CharPtr = (char *)MsgBuf;

     for(i=0; i < MsgNumber; i++)
     {
        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0196*/

        rc = mps_Recvnowait1(ProcIdentList[procnum], CharPtr,
                             MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                             tag);

        /* For statistics. rtl_Recvnowait. */    /*E0197*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           RecvCallTime   += CommTime;
           MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
           MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
           RecvCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->RecvCallTime    += CommTime;
              SendRecvTimesPtr->MinRecvCallTime  =
              dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
              SendRecvTimesPtr->MaxRecvCallTime  =
              dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
              SendRecvTimesPtr->RecvCallCount++;
           }
        }

        /* -------------- */    /*E0198*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.003: mps_Recvnowait1 rc = %d\n",
                   rc);
        CharPtr += MsgLength;
     }

     if(Remainder)
     {
        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0199*/

        rc = mps_Recvnowait1(ProcIdentList[procnum], CharPtr,
                             Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                             tag);

        /* For statistics. rtl_Recvnowait. */    /*E0200*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           RecvCallTime   += CommTime;
           MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
           MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
           RecvCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->RecvCallTime    += CommTime;
              SendRecvTimesPtr->MinRecvCallTime  =
              dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
              SendRecvTimesPtr->MaxRecvCallTime  =
              dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
              SendRecvTimesPtr->RecvCallCount++;
           }
        }

        /* -------------- */    /*E0201*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.003: mps_Recvnowait1 rc = %d\n",
                   rc);
     }
  }
  else
  {  /* */    /*E0202*/

     if(((MsgPartReg && MsgPartition > 0) || MsgPartReg > 1) &&
        MaxMsgLength > 0 && UserSumFlag && MsgLen > MaxMsgLength &&
        CheckRendezvous == 0 && Part != 0)
     {  /* */    /*E0203*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0204*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0205*/
        MsgLength = MaxMsgLength;          /* */    /*E0206*/

        if(MaxMsgParts)
        {  /* */    /*E0207*/

           if(Remainder)
              rc = MsgNumber + 1;
           else
              rc = MsgNumber;

           if(rc > MaxMsgParts)
           {  /* */    /*E0208*/

              MsgNumber = MaxMsgParts;
              MsgLength = MsgLen / MaxMsgParts;
              Remainder = MsgLen % MaxMsgParts;

              if(Remainder)
              {  MsgNumber--;
                 Remainder += MsgLength;
              }

              if(Msk3)
              {  rc = MsgLength % 4;

                 if(rc)
                 {  /* */    /*E0209*/

                    MsgLength += (4-rc); /* */    /*E0210*/
                    MsgNumber = MsgLen / MsgLength; /* */    /*E0211*/
                    Remainder = MsgLen % MsgLength; /* */    /*E0212*/
                 }
              }
           }
        }

        if(RTL_TRACE && MsgPartitionTrace &&
           TstTraceEvent(call_rtl_Recvnowait))
           tprintf("*** MsgPartitionTrace. Recvnowait: "
                   "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                   MsgNumber, MsgLength, Remainder);

        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0213*/

        dvm_AllocArray(MPS_Request, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->MPSFlagArray);
        dvm_AllocArray(byte, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->EndExchange);

        for(i=0; i < RTL_ReqPtr->FlagNumber; i++)
            RTL_ReqPtr->EndExchange[i] = 0;

        CharPtr = (char *)MsgBuf;

        for(i=0; i < MsgNumber; i++)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();   /* for statistics */    /*E0214*/

           rc = mps_Recvnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                                tag);

           /* For statistics. rtl_Recvnowait. */    /*E0215*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              RecvCallTime   += CommTime;
              MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
              MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
              RecvCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->RecvCallTime    += CommTime;
                 SendRecvTimesPtr->MinRecvCallTime  =
                 dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
                 SendRecvTimesPtr->MaxRecvCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
                 SendRecvTimesPtr->RecvCallCount++;
              }
           }

           /* -------------- */    /*E0216*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.003: mps_Recvnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(Remainder)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();   /* for statistics */    /*E0217*/

           rc = mps_Recvnowait1(ProcIdentList[procnum], CharPtr,
                                Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                                tag);

           /* For statistics. rtl_Recvnowait. */    /*E0218*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              RecvCallTime   += CommTime;
              MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
              MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
              RecvCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->RecvCallTime    += CommTime;
                 SendRecvTimesPtr->MinRecvCallTime  =
                 dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
                 SendRecvTimesPtr->MaxRecvCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
                 SendRecvTimesPtr->RecvCallCount++;
              }
           }

           /* -------------- */    /*E0219*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.003: mps_Recvnowait1 rc = %d\n",
                      rc);
        }
     }
     else
     {  /* */    /*E0220*/

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0221*/

        rc = mps_Recvnowait(ProcIdentList[procnum], MsgBuf, MsgLen,
                            RTL_ReqPtr, tag);

        /* For statistics. rtl_Recvnowait. */    /*E0222*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           RecvCallTime   += CommTime;
           MinRecvCallTime = dvm_min(MinRecvCallTime, CommTime);
           MaxRecvCallTime = dvm_max(MaxRecvCallTime, CommTime);
           RecvCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->RecvCallTime    += CommTime;
              SendRecvTimesPtr->MinRecvCallTime  =
              dvm_min(SendRecvTimesPtr->MinRecvCallTime, CommTime);
              SendRecvTimesPtr->MaxRecvCallTime  =
              dvm_max(SendRecvTimesPtr->MaxRecvCallTime, CommTime);
              SendRecvTimesPtr->RecvCallCount++;
           }
        }

        /* -------------- */    /*E0223*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.003: mps_Recvnowait rc = %d\n", rc);
     }
  }

  if(CheckRendezvous)
     RecvRendezvousArray[procnum] = RTL_ReqPtr;/* processor with internal
                                                 number procnum is busy */    /*E0224*/

  for(tl=0; tl < RecvDelay; tl++) /* loop of special delay */    /*E0225*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_rtl_Recvnowait,"rc=%d; req=%lx;\n",
                                   rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



void  rtl_Waitrequest(RTL_Request *RTL_ReqPtr)
{ int           i, j, k, procnum = RTL_ReqPtr->ProcNumber, tst;
  char         *CharPtr;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;

#ifdef _DVM_MPI_

  char   *CharPtr0, *CharPtr1, *CharPtr2, *CharPtr3, *CharPtr4;
  int     n;

#endif

#if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

  uLongf        gzLen;

#endif

  DVMMTimeStart(call_rtl_Waitrequest);

  if(RTL_TRACE)
     dvm_trace(call_rtl_Waitrequest,
               "req=%lx; procnum=%d(%d); procid=%d;\n",
                (uLLng)RTL_ReqPtr, procnum, ProcNumberList[procnum],
                ProcIdentList[procnum]);

  NoWaitCount--;  /* */    /*E0226*/

  Curr_synchr_time = 0.;

  if(RTL_ReqPtr->BufAddr != NULL)
  {  TstFreeProc(tst, procnum, RTL_ReqPtr);

     if(tst)
     {
        if(MsgSchedule && UserSumFlag)
        {  /* */    /*E0227*/

           tst = rtl_TstReqColl(0);

           if(RTL_ReqPtr->SendSign)
           {  /* */    /*E0228*/

              if(RTL_TRACE && MsgScheduleTrace &&
                 TstTraceEvent(call_rtl_Waitrequest))
                 tprintf("*** MsgScheduleTrace. Waitrequest (s): "
                         "FlagNumber=%d Init=%d Last=%d\n",
                         RTL_ReqPtr->FlagNumber,
                         RTL_ReqPtr->Init, RTL_ReqPtr->Last);

              if(RTL_ReqPtr->Init == RTL_ReqPtr->FlagNumber)
              {  /* */    /*E0229*/

                 if(tst)
                    rtl_SendReqColl(ResCoeffWaitReq);
              }
              else
              {  /* */    /*E0230*/

                 if(RTL_ReqPtr->Chan == -1)
                 {  /* */    /*E0231*/

                    if(FreeChanNumber)
                    {  /* */    /*E0232*/

                       for(i=0; i < ParChanNumber; i++)
                       {  if(ChanRTL_ReqPtr[i] == NULL)
                             break;  /* */    /*E0233*/
                       }
                    }
                    else
                    {  /* */    /*E0234*/

                       i = -1;
                       j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0235*/

                       for(k=0; k < ParChanNumber; k++)
                       {  tst = (ChanRTL_ReqPtr[k])->FlagNumber -
                                (ChanRTL_ReqPtr[k])->Init;

                          if(tst < j)
                          {  j = tst;
                             i = k;
                          }
                       }

                       rtl_FreeChan(i);  /* */    /*E0236*/
                    }

                    /* */    /*E0237*/

                    ChanRTL_ReqPtr[i] = RTL_ReqPtr;
                    RTL_ReqPtr->Chan = i;
                    FreeChanNumber--;
                    NewMsgNumber--;
                 }

                 /* */    /*E0238*/

                 rtl_FreeChan(RTL_ReqPtr->Chan); /* */    /*E0239*/
                 if(FreeChanReg == 0)
                    rtl_TstReqColl(0); /* */    /*E0240*/

                 rtl_SendReqColl(ResCoeffWaitReq); /* */    /*E0241*/
              }

              dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
              RTL_ReqPtr->FlagNumber = 0;
              RTL_ReqPtr->MPSFlagArray = NULL;

              coll_Delete(&SendReqColl, RTL_ReqPtr); /* */    /*E0242*/

              #ifdef _DVM_MPI_

              if(RTL_ReqPtr->CompressBuf != NULL)
              {  /* */    /*E0243*/

                 i = RTL_ReqPtr->ResCompressIndex; 
                 RTL_ReqPtr->ResCompressIndex = -1; 

                 if(i >= 0)
                 {  RTL_ReqPtr->CompressBuf = NULL;
                    FreeCompressBuf[i] = 1;
                 }
                 else
                 {  mac_free(&RTL_ReqPtr->CompressBuf);
                 }
              }

              #endif
           }
           else
           {  /* */    /*E0244*/
 
              if(tst)
                 rtl_SendReqColl(ResCoeffWaitReq);

              if(RTL_TRACE && MsgScheduleTrace &&
                 TstTraceEvent(call_rtl_Waitrequest))
                 tprintf("*** MsgScheduleTrace. Waitrequest (r): "
                         "FlagNumber=%d Init=%d Last=%d\n",
                         RTL_ReqPtr->FlagNumber,
                         RTL_ReqPtr->Init, RTL_ReqPtr->Last);

              for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
                  mps_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

              dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
              dvm_FreeArray(RTL_ReqPtr->EndExchange);
              RTL_ReqPtr->FlagNumber = 0;
              RTL_ReqPtr->MPSFlagArray = NULL;
              RTL_ReqPtr->EndExchange = NULL;

              #ifdef _DVM_MPI_

              if(RTL_ReqPtr->CompressBuf != NULL)
              {  /* */    /*E0245*/

                 if(RTL_ReqPtr->IsCompressSize == 0)
                 {  /* */    /*E0246*/

                    for(i=0,RTL_ReqPtr->CompressMsgLen=0; i < 10; i++)
                    {
                       SYSTEM(MPI_Get_count,
                              (&RTL_ReqPtr->Status, MPI_BYTE,
                               &RTL_ReqPtr->CompressMsgLen))

                       if(RTL_ReqPtr->CompressMsgLen != 0)
                          break;
                    }

                    if(i == 10)
                       eprintf(__FILE__,__LINE__,
                               "*** RTS fatal err: "
                               "MPI_Get_count rc = %d\n",
                               RTL_ReqPtr->CompressMsgLen);
                    RTL_ReqPtr->IsCompressSize = 1;
                 }

                 tl = RTL_ReqPtr->BufLength;

                 if(IsSynchr && UserSumFlag)
                    tl += sizeof(double) + SizeDelta[tl & Msk3];
                 else
                    tl += SizeDelta[tl & Msk3];

                 if(MsgDVMCompress != 0 && tl > MinMsgCompressLength &&
                    UserSumFlag != 0 &&
                    (RTL_ReqPtr->BufLength % sizeof(double)) == 0)
                 {  /* */    /*E0247*/

                    /* */    /*E0248*/

                    if((double)RTL_ReqPtr->CompressMsgLen / (double)tl <=
                       CompressCoeff)
                    {  /* */    /*E0249*/

                       j = (int)(tl / sizeof(double));  /* */    /*E0250*/
                       if(MsgDVMCompress < 2)
                       {  /* */    /*E0251*/

                          n = sizeof(double) - 1;

                          if(InversByteOrder)
                          {  CharPtr0 = RTL_ReqPtr->BufAddr;
                             CharPtr1 = CharPtr0 + 1;
                          }
                          else
                          {  CharPtr1 = RTL_ReqPtr->BufAddr;
                             CharPtr0 = CharPtr1 + n;
                          }
                       }
                       else
                       {  /* */    /*E0252*/

                          n = sizeof(double) - 2;

                          if(InversByteOrder)
                          {  CharPtr0 = RTL_ReqPtr->BufAddr;
                             CharPtr1 = CharPtr0 + 2;
                          } 
                          else
                          {  CharPtr1 = RTL_ReqPtr->BufAddr;
                             CharPtr0 = CharPtr1 + n;
                          }
                       }

                       CharPtr2 = RTL_ReqPtr->CompressBuf;
                       k = (unsigned char)(*CharPtr2);
                       CharPtr3 = CharPtr2 + 1;
                       CharPtr4 = CharPtr3 + n;

                       for(i=0; i < j; i++)
                       {  SYSTEM(memcpy, (CharPtr1, CharPtr3, n))

                          *CharPtr0 = *CharPtr4;
                          CharPtr4++;

                          if(MsgDVMCompress > 1)
                          {  CharPtr0[1] = *CharPtr4;
                             CharPtr4++;
                          }

                          CharPtr0 += sizeof(double);
                          CharPtr1 += sizeof(double);
                      
                          k--;

                          if(k == 0)
                          {  /* */    /*E0253*/

                             CharPtr2 = CharPtr4;

                             k = (unsigned char)(*CharPtr2);
                             CharPtr3 = CharPtr2 + 1;
                             CharPtr4 = CharPtr3 + n;
                          }
                       }
                    }
                    else
                    {  /* */    /*E0254*/

                       CharPtr0 = RTL_ReqPtr->BufAddr;
                       CharPtr2 = RTL_ReqPtr->CompressBuf;

                       SYSTEM(memcpy, (CharPtr0, CharPtr2,
                              RTL_ReqPtr->CompressMsgLen))
                    }

                    if(RTL_TRACE && MsgCompressTrace)
                       tprintf("*** MsgDVMUncompress: branch=1; "
                               "MsgLen=%ld; CompressLen=%d; "
                               "CompressCode=%d;\n",
                               tl, RTL_ReqPtr->CompressMsgLen,
                               MsgDVMCompress);
                 }

                 #ifdef _DVM_ZLIB_

                 if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
                    (MsgDVMCompress == 0 ||
                     (RTL_ReqPtr->BufLength % sizeof(double)) != 0) &&
                    tl > MinMsgCompressLength && UserSumFlag != 0)
                 {  /* */    /*E0255*/

                    /* */    /*E0256*/

                    gzLen = (uLongf)tl;

                    SYSTEM_RET(i, uncompress,
                               ((Bytef *)RTL_ReqPtr->BufAddr, &gzLen,
                                (Bytef *)RTL_ReqPtr->CompressBuf,
                                (uLong)RTL_ReqPtr->CompressMsgLen));

                    if(i != Z_OK)
                       eprintf(__FILE__,__LINE__,
                            "*** RTS err 210.011: uncompress rc = %d\n",
                            i);

                    if(RTL_TRACE && MsgCompressTrace)
                       tprintf("*** MsgUncompress: branch=1; rc=%d; "
                               "MsgLen=%ld; gzLen=%d;\n",
                               i, (DvmType)gzLen,
                               RTL_ReqPtr->CompressMsgLen);
                 }

                 #endif

                 i = RTL_ReqPtr->ResCompressIndex; 
                 RTL_ReqPtr->ResCompressIndex = -1; 

                 if(i >= 0)
                 {  RTL_ReqPtr->CompressBuf = NULL;
                    FreeCompressBuf[i] = 1;
                 }
                 else
                 {  mac_free(&RTL_ReqPtr->CompressBuf);
                 }
              }

              #endif
           }
        }
        else
        {  /* */    /*E0257*/

           if(RTL_ReqPtr->FlagNumber)
           {  /* */    /*E0258*/

              if(RTL_TRACE && MsgPartitionTrace &&
                 TstTraceEvent(call_rtl_Waitrequest))
                 tprintf("*** MsgPartitionTrace. Waitrequest: "
                         "FlagNumber=%d\n", RTL_ReqPtr->FlagNumber);

              for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
                  mps_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

              dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
              dvm_FreeArray(RTL_ReqPtr->EndExchange);
              RTL_ReqPtr->FlagNumber = 0;
              RTL_ReqPtr->MPSFlagArray = NULL;
              RTL_ReqPtr->EndExchange = NULL;
           }
           else
           {
              mps_Waitrequest(RTL_ReqPtr);
           }

           #ifdef _DVM_MPI_

           if(RTL_ReqPtr->CompressBuf != NULL)
           {  /* */    /*E0259*/

              if(RTL_ReqPtr->SendSign)
              {  /* */    /*E0260*/

                 i = RTL_ReqPtr->ResCompressIndex; 
                 RTL_ReqPtr->ResCompressIndex = -1; 

                 if(i >= 0)
                 {  RTL_ReqPtr->CompressBuf = NULL;
                    FreeCompressBuf[i] = 1;
                 }
                 else
                 {  mac_free(&RTL_ReqPtr->CompressBuf);
                 }
              }
              else
              {  /* */    /*E0261*/

                 if(RTL_ReqPtr->IsCompressSize == 0)
                 {  /* */    /*E0262*/

                    for(i=0,RTL_ReqPtr->CompressMsgLen=0; i < 10; i++)
                    {
                       SYSTEM(MPI_Get_count,
                              (&RTL_ReqPtr->Status, MPI_BYTE,
                               &RTL_ReqPtr->CompressMsgLen))

                       if(RTL_ReqPtr->CompressMsgLen != 0)
                          break;
                    }

                    if(i == 10)
                       eprintf(__FILE__,__LINE__,
                               "*** RTS fatal err: "
                               "MPI_Get_count rc = %d\n",
                               RTL_ReqPtr->CompressMsgLen);
                    RTL_ReqPtr->IsCompressSize = 1;
                 }

                 tl = RTL_ReqPtr->BufLength;

                 if(IsSynchr && UserSumFlag)
                    tl += sizeof(double) + SizeDelta[tl & Msk3];
                 else
                    tl += SizeDelta[tl & Msk3];

                 if(MsgDVMCompress != 0 && tl > MinMsgCompressLength &&
                    UserSumFlag != 0 &&
                    (RTL_ReqPtr->BufLength % sizeof(double)) == 0)
                 {  /* */    /*E0263*/

                    /* */    /*E0264*/

                    if((double)RTL_ReqPtr->CompressMsgLen / (double)tl <=
                       CompressCoeff)
                    {  /* */    /*E0265*/

                       j = (int)(tl / sizeof(double));  /* */    /*E0266*/
                       if(MsgDVMCompress < 2)
                       {  /* */    /*E0267*/

                          n = sizeof(double) - 1;

                          if(InversByteOrder)
                          {  CharPtr0 = RTL_ReqPtr->BufAddr;
                             CharPtr1 = CharPtr0 + 1;
                          }
                          else
                          {  CharPtr1 = RTL_ReqPtr->BufAddr;
                             CharPtr0 = CharPtr1 + n;
                          } 
                       }
                       else
                       {  /* */    /*E0268*/

                          n = sizeof(double) - 2;

                          if(InversByteOrder)
                          {  CharPtr0 = RTL_ReqPtr->BufAddr;
                             CharPtr1 = CharPtr0 + 2;
                          }
                          else
                          {  CharPtr1 = RTL_ReqPtr->BufAddr;
                             CharPtr0 = CharPtr1 + n;
                          }
                       }

                       CharPtr2 = RTL_ReqPtr->CompressBuf;
                       k = (unsigned char)(*CharPtr2);
                       CharPtr3 = CharPtr2 + 1;
                       CharPtr4 = CharPtr3 + n;

                       for(i=0; i < j; i++)
                       {  SYSTEM(memcpy, (CharPtr1, CharPtr3, n))

                          *CharPtr0 = *CharPtr4;
                          CharPtr4++;

                          if(MsgDVMCompress > 1)
                          {  CharPtr0[1] = *CharPtr4;
                             CharPtr4++;
                          }

                          CharPtr0 += sizeof(double);
                          CharPtr1 += sizeof(double);
                      
                          k--;

                          if(k == 0)
                          {  /* */    /*E0269*/

                             CharPtr2 = CharPtr4;

                             k = (unsigned char)(*CharPtr2);
                             CharPtr3 = CharPtr2 + 1;
                             CharPtr4 = CharPtr3 + n;
                          }
                       }
                    }
                    else
                    {  /* */    /*E0270*/

                       CharPtr0 = RTL_ReqPtr->BufAddr;
                       CharPtr2 = RTL_ReqPtr->CompressBuf;

                       SYSTEM(memcpy, (CharPtr0, CharPtr2,
                              RTL_ReqPtr->CompressMsgLen))
                    }

                    if(RTL_TRACE && MsgCompressTrace)
                       tprintf("*** MsgDVMUncompress: branch=2; "
                               "MsgLen=%ld; CompressLen=%d; "
                               "CompressCode=%d;\n",
                               tl, RTL_ReqPtr->CompressMsgLen,
                               MsgDVMCompress);
                 }

                 #ifdef _DVM_ZLIB_

                 if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
                    (MsgDVMCompress == 0 ||
                     (RTL_ReqPtr->BufLength % sizeof(double)) != 0) &&
                    tl > MinMsgCompressLength && UserSumFlag != 0)
                 {  /* */    /*E0271*/

                    /* */    /*E0272*/

                    gzLen = (uLongf)tl;

                    SYSTEM_RET(i, uncompress,
                               ((Bytef *)RTL_ReqPtr->BufAddr, &gzLen,
                                (Bytef *)RTL_ReqPtr->CompressBuf,
                                (uLong)RTL_ReqPtr->CompressMsgLen));

                    if(i != Z_OK)
                       eprintf(__FILE__,__LINE__,
                               "*** RTS err 210.011: uncompress rc = %d\n",
                               i);

                    if(RTL_TRACE && MsgCompressTrace)
                       tprintf("*** MsgUncompress: branch=2; rc=%d; "
                               "MsgLen=%ld; gzLen=%d;\n",
                               i, (DvmType)gzLen, RTL_ReqPtr->CompressMsgLen);
                 }

                 #endif

                 i = RTL_ReqPtr->ResCompressIndex; 
                 RTL_ReqPtr->ResCompressIndex = -1; 

                 if(i >= 0)
                 {  RTL_ReqPtr->CompressBuf = NULL;
                    FreeCompressBuf[i] = 1;
                 }
                 else
                 {  mac_free(&RTL_ReqPtr->CompressBuf);
                 }
              }
           } 

           #endif
        }

        if(RTL_TRACE)
           trc_PrintBuffer(RTL_ReqPtr->BufAddr, RTL_ReqPtr->BufLength,
                           call_rtl_Waitrequest); /* Print received array */    /*E0273*/
   
        FreeProc(procnum, RTL_ReqPtr); /* processor with internal
                                          number procnum is free */    /*E0274*/

        /* Calculate dissynchronization time */    /*E0275*/

        if(IsSynchr && UserSumFlag && RTL_ReqPtr->SendRecvTime > 0.f)
        {  CharPtr = RTL_ReqPtr->BufAddr;
           CharPtr += RTL_ReqPtr->BufLength;
           SYSTEM(memcpy, (synchr_time_ptr, CharPtr, sizeof(double)))
           if(Curr_synchr_time < RTL_ReqPtr->SendRecvTime)
   /*           Curr_synchr_time = RTL_ReqPtr->SendRecvTime -
                                   Curr_synchr_time;  */    /*E0276*/
              Curr_synchr_time = 0.;
           else
           {  if(Curr_synchr_time > Curr_dvm_time)
                 Curr_synchr_time -= Curr_dvm_time;
              else
                 Curr_synchr_time = 0.;
           }
        }

        RTL_ReqPtr->BufAddr = NULL;
     }
  }

  for(tl=0; tl < WaitDelay; tl++) /* loop of special delay */    /*E0277*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_rtl_Waitrequest,"req=%lx;\n", (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



int  rtl_Testrequest(RTL_Request *RTL_ReqPtr)
{ int    rc = 1, procnum = RTL_ReqPtr->ProcNumber, tst;

  DVMMTimeStart(call_rtl_Testrequest);

  if(RTL_TRACE)
     dvm_trace(call_rtl_Testrequest,
               "req=%lx; procnum=%d(%d); procid=%d;\n",
                (uLLng)RTL_ReqPtr, procnum, ProcNumberList[procnum],
                ProcIdentList[procnum]);

  if(RTL_ReqPtr->BufAddr != NULL)
  {  TstFreeProc(tst, procnum, RTL_ReqPtr);

     if(tst)
     {  if(MsgSchedule && UserSumFlag)
        {  /* */    /*E0278*/

           tst = rtl_TstReqColl(0);

           if(tst)
              rtl_SendReqColl(ResCoeffTstReq);

           if(RTL_ReqPtr->SendSign)
           {  /* */    /*E0279*/

              if(RTL_TRACE && MsgScheduleTrace &&
                 TstTraceEvent(call_rtl_Testrequest))
                 tprintf("*** MsgScheduleTrace. Testrequest (s): "
                         "FlagNumber=%d Init=%d Last=%d\n",
                         RTL_ReqPtr->FlagNumber,
                         RTL_ReqPtr->Init, RTL_ReqPtr->Last);

              if(RTL_ReqPtr->Init != RTL_ReqPtr->FlagNumber)
                 rc = 0;
           }
           else
           {  /* */    /*E0280*/
 
              if(RTL_TRACE && MsgScheduleTrace &&
                 TstTraceEvent(call_rtl_Testrequest))
                 tprintf("*** MsgScheduleTrace. Testrequest (r): "
                         "FlagNumber=%d Init=%d Last=%d\n",
                         RTL_ReqPtr->FlagNumber,
                         RTL_ReqPtr->Init, RTL_ReqPtr->Last);

              for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
              {  if(RTL_ReqPtr->EndExchange[tst] == 0)
                 { rc = mps_Testrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

                   if(rc == 0)
                      break; /* */    /*E0281*/

                   RTL_ReqPtr->EndExchange[tst] = 1;
                 }
              }
           }
        }
        else
        {  /* */    /*E0282*/

           if(RTL_ReqPtr->FlagNumber)
           {  /* */    /*E0283*/

              if(RTL_TRACE && MsgPartitionTrace &&
                 TstTraceEvent(call_rtl_Testrequest))
                 tprintf("*** MsgPartitionTrace. Testrequest: "
                         "FlagNumber=%d\n", RTL_ReqPtr->FlagNumber);

              for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
              {  if(RTL_ReqPtr->EndExchange[tst] == 0)
                 { rc = mps_Testrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

                   if(rc == 0)
                      break; /* */    /*E0284*/

                   RTL_ReqPtr->EndExchange[tst] = 1;
                 }
              }
           }
           else
              rc = mps_Testrequest(RTL_ReqPtr);
        }

        if(rc)
        {  if(RTL_TRACE && RTL_ReqPtr->CompressBuf == NULL)
              trc_PrintBuffer(RTL_ReqPtr->BufAddr, RTL_ReqPtr->BufLength,
                              call_rtl_Testrequest); /* Print received array */    /*E0285*/
           FreeProc(procnum, RTL_ReqPtr); /* processor with internal
                                             number procnum is free */    /*E0286*/
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_rtl_Testrequest,"rc=%d; req=%lx;\n",
                                    rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  rtl_SendA(void *buf, int count, int size, int procnum, int tag)
{ int    rc, MsgLen, MsgNumber, Remainder, MsgLength;
  char  *CharPtr = (char *)buf;

  DVMMTimeStart(call_rtl_SendA);

  if(RTL_TRACE)
     dvm_trace(call_rtl_SendA,"buf=%lx; count=%d; size=%d; "
                              "procnum=%d(%d);"
                              " procid=%d; tag=%d;\n",
                               (uLLng)buf, count, size, procnum,
                               ProcNumberList[procnum],
                               ProcIdentList[procnum], tag);

  rc = count * size;

  SendRendezvous(procnum);  /* check that only one exchange between 
                               two processors is allowed */    /*E0287*/

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, rc,
                     call_rtl_SendA);    /* Print sent array */    /*E0288*/

  if(IsSynchr && UserSumFlag)
  {  CharPtr += rc;
     SYSTEM(memcpy,
     (CharPtr, dvm_time_ptr, sizeof(double)) ) /* copy the time
                                                  of the beginning of exchange */    /*E0289*/
     MsgLen = rc + sizeof(double) + SizeDelta[rc & Msk3];
  }
  else
     MsgLen = rc + SizeDelta[rc & Msk3];

  #ifdef _DVM_MPI_

    if(MPIInfoPrint && StatOff == 0)
    {  if(UserSumFlag != 0)
          MPISendByteNumber  += MsgLen;
       else
          MPISendByteNumber0 += MsgLen;
    }

  #endif

  if(MsgPartReg > 1 && MaxMsgLength > 0 && UserSumFlag &&
     MsgLen > MaxMsgLength && CheckRendezvous == 0)
  {  /* */    /*E0290*/

     MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0291*/
     Remainder = MsgLen % MaxMsgLength; /* */    /*E0292*/
     MsgLength = MaxMsgLength;          /* */    /*E0293*/

     if(MaxMsgParts)
     {  /* */    /*E0294*/

        if(Remainder)
           rc = MsgNumber + 1;
        else
           rc = MsgNumber;

        if(rc > MaxMsgParts)
        {  /* */    /*E0295*/

           MsgNumber = MaxMsgParts;
           MsgLength = MsgLen / MaxMsgParts;
           Remainder = MsgLen % MaxMsgParts;

           if(Remainder)
           {  MsgNumber--;
              Remainder += MsgLength;
           }

           if(Msk3)
           {  rc = MsgLength % 4;

              if(rc)
              {  /* */    /*E0296*/

                 MsgLength += (4-rc); /* */    /*E0297*/
                 MsgNumber = MsgLen / MsgLength; /* */    /*E0298*/
                 Remainder = MsgLen % MsgLength; /* */    /*E0299*/
              }
           }
        }
     }

     if(RTL_TRACE && MsgPartitionTrace && TstTraceEvent(call_rtl_SendA))
        tprintf("*** MsgPartitionTrace. SendA: "
                "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                MsgNumber, MsgLength, Remainder);

     CharPtr = (char *)buf;

     for(; MsgNumber > 0; MsgNumber--)
     {  rc = mps_SendA(ProcIdentList[procnum], CharPtr, MsgLength, tag);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.004: mps_SendA rc = %d\n", rc);
        CharPtr += MsgLength;
     }

     if(Remainder)
     {  rc = mps_SendA(ProcIdentList[procnum], CharPtr, Remainder, tag);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.004: mps_SendA rc = %d\n", rc);
     }
  }
  else
  {  /* */    /*E0300*/

     rc = mps_SendA(ProcIdentList[procnum], buf, MsgLen, tag);

     if(rc < 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 210.004: mps_SendA rc = %d\n", rc);
  }

  if(RTL_TRACE)
     dvm_trace(ret_rtl_SendA,"rc=%d;\n", rc);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  rtl_RecvA(void *buf, int count, int size, int procnum, int tag)
{ int    rc, ByteCount, MsgLen, MsgNumber, Remainder, MsgLength;
  char  *CharPtr = (char *)buf, *CharPtr1;

  DVMMTimeStart(call_rtl_RecvA);

  if(RTL_TRACE)
     dvm_trace(call_rtl_RecvA,"buf=%lx; count=%d; size=%d; "
                              "procnum=%d(%d);"
                              " procid=%d; tag=%d;\n",
                              (uLLng)buf, count, size, procnum,
                              ProcNumberList[procnum],
                              ProcIdentList[procnum], tag);

  ByteCount = count * size;

  RecvRendezvous(procnum);  /* check that only one exchange between 
                               two processors is allowed */    /*E0301*/

  if(IsSynchr && UserSumFlag)
     MsgLen = ByteCount + sizeof(double) + SizeDelta[ByteCount & Msk3];
  else
     MsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  if(MsgPartReg > 1 && MaxMsgLength > 0 && UserSumFlag &&
     MsgLen > MaxMsgLength && CheckRendezvous == 0)
  {  /* */    /*E0302*/

     MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0303*/
     Remainder = MsgLen % MaxMsgLength; /* */    /*E0304*/
     MsgLength = MaxMsgLength;          /* */    /*E0305*/

     if(MaxMsgParts)
     {  /* */    /*E0306*/

        if(Remainder)
           rc = MsgNumber + 1;
        else
           rc = MsgNumber;

        if(rc > MaxMsgParts)
        {  /* */    /*E0307*/

           MsgNumber = MaxMsgParts;
           MsgLength = MsgLen / MaxMsgParts;
           Remainder = MsgLen % MaxMsgParts;

           if(Remainder)
           {  MsgNumber--;
              Remainder += MsgLength;
           }

           if(Msk3)
           {  rc = MsgLength % 4;

              if(rc)
              {  /* */    /*E0308*/

                 MsgLength += (4-rc); /* */    /*E0309*/
                 MsgNumber = MsgLen / MsgLength; /* */    /*E0310*/
                 Remainder = MsgLen % MsgLength; /* */    /*E0311*/
              }
           }
        }
     }

     if(RTL_TRACE && MsgPartitionTrace && TstTraceEvent(call_rtl_RecvA))
        tprintf("*** MsgPartitionTrace. RecvA: "
                "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                MsgNumber, MsgLength, Remainder);

     CharPtr1 = (char *)buf;

     for(; MsgNumber > 0; MsgNumber--)
     {  rc = mps_RecvA(ProcIdentList[procnum], CharPtr1, MsgLength, tag);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.005: mps_RecvA rc = %d\n", rc);
        CharPtr1 += MsgLength;
     }

     if(Remainder)
     {  rc = mps_RecvA(ProcIdentList[procnum], CharPtr1, Remainder, tag);

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.005: mps_RecvA rc = %d\n", rc);
     }
  }
  else
  {  /* */    /*E0312*/

     rc = mps_RecvA(ProcIdentList[procnum], buf, MsgLen, tag);

     if(rc < 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 210.005: mps_RecvA rc = %d\n", rc);
  }

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, ByteCount,
                     call_rtl_RecvA);  /* Print received array */    /*E0313*/

  /* Calculate dissynchronization time */    /*E0314*/

  if(IsSynchr && UserSumFlag)
  {  CharPtr += ByteCount;
     SYSTEM(memcpy, (synchr_time_ptr, CharPtr, sizeof(double)))
     Curr_synchr_time -= Curr_dvm_time;
     if(Curr_synchr_time < 0.)
        Curr_synchr_time = 0.;
  }
  else
     Curr_synchr_time = 0.;

  if(RTL_TRACE)
     dvm_trace(ret_rtl_RecvA,"rc=%d;\n", rc);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



void  rtl_BroadCast(void *buf, int count, int size, int SenderProcNum,
                    PSRef *PSRefPtr)
{ int           i;
  RTL_Request  *Req;
  RTL_Request   InReq;
  s_VMS        *VMS;
  SysHandle    *VMSHandlePtr;

  DVMFTimeStart(call_rtl_BroadCast);

  if(RTL_TRACE)
  {  if(PSRefPtr == NULL)
        dvm_trace(call_rtl_BroadCast,
                  "Buf=%lx; Count=%d; Size=%d; ProcNumb=%d(%d); "
                  "ProcId=%ld; PSRefPtr=NULL; PSRef=0\n",
                  (uLLng)buf, count, size, SenderProcNum,
                  ProcNumberList[SenderProcNum],
                  ProcIdentList[SenderProcNum]);
     else 
        dvm_trace(call_rtl_BroadCast,
                  "Buf=%lx; Count=%d; Size=%d; ProcNumb=%d(%d); "
                  "ProcId=%ld; PSRefPtr=%lx; PSRef=%lx\n",
                  (uLLng)buf, count, size, SenderProcNum,
                  ProcNumberList[SenderProcNum],
                  ProcIdentList[SenderProcNum],
                  (uLLng)PSRefPtr, *PSRefPtr);
  }

  if(PSRefPtr == NULL || *PSRefPtr == 0)
     VMS = DVM_VMS;
  else
  {  if(TstObject)
     {  if(TstDVMObj(PSRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 219.000: wrong call rtl_BroadCast\n"
             "(the processor system is not a DVM object; "
             "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr = (SysHandle *)*PSRefPtr;

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.001: wrong call rtl_BroadCast\n"
                 "(the object is not a processor system); PSRef=%lx\n",
                 *PSRefPtr);

     VMS = (s_VMS *)VMSHandlePtr->pP;
  }

  /* Check if all processors of the given processor system
              are in the current processor system          */    /*E0315*/

  NotSubsystem(i, DVM_VMS, VMS)
   
  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 219.002: wrong call rtl_BroadCast\n"
              "(the given PS is not a subsystem of the current PS;\n"
              "PSRef=%lx; CurrentPSRef=%lx)\n",
              *PSRefPtr, (uLLng)DVM_VMS->HandlePtr);

  /* Check if the given processor belongs to 
          the current processor system       */    /*E0316*/

  if(IsProcInVMS(SenderProcNum, DVM_VMS) < 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 219.003: wrong call rtl_BroadCast\n"
           "(the given processr is not a member of the current PS;\n"
           "SenderProcNum=%d; CurrentPSRef=%lx)\n",
           SenderProcNum, (uLLng)DVM_VMS->HandlePtr);

  i = 1;

#ifdef _DVM_MPI_

  if(MPIBcast)
  {
     if(VMS->Is_MPI_COMM != 0)
     {
        i = IsProcInVMS(SenderProcNum, VMS);

        if(MPIInfoPrint && StatOff == 0)
           MPI_BcastTime -= dvm_time();

        SYSTEM(MPI_Bcast, (buf, count*size, MPI_BYTE, i, VMS->PS_MPI_COMM))

        if(MPIInfoPrint && StatOff == 0)
           MPI_BcastTime += dvm_time();

        i = 0;
     }
     else
        i = 2;
  }
  else
     i = 1;

  if(RTL_TRACE && TstTraceEvent(call_rtl_BroadCast))
     tprintf("*** DoNotMPIBcast = %d\n", i);

  if(MPIBcastPrint && _SysInfoPrint && MPS_CurrentProc == DVM_IOProc)
     rtl_iprintf("\n*** RTS: DoNotMPIBcast = %d\n", i);

#endif

  if(i != 0)
  {  
    /* Forward to the next element of message tag circle tag_BroadCast 
     for the current processor system */    /*E0317*/

    DVM_VMS->tag_BroadCast++;

    if((DVM_VMS->tag_BroadCast - (msg_BroadCast)) >= TagCount)
       DVM_VMS->tag_BroadCast = msg_BroadCast;

    /* ------------------------------------------------ */    /*E0318*/

    if(MPS_CurrentProc == SenderProcNum)
    {
       if(VMS->ProcCount > 1)
       {
          dvm_AllocArray(RTL_Request, VMS->ProcCount, Req);

          for(i=0; i < VMS->ProcCount; i++)
              if(VMS->VProc[i].lP != SenderProcNum)
                ( RTL_CALL, rtl_Sendnowait(buf, count, size,
                                           (int)VMS->VProc[i].lP,
                                           DVM_VMS->tag_BroadCast,
                                           &Req[i], BCastSend) );

          if(MsgSchedule && UserSumFlag)
          {
             rtl_TstReqColl(0);
             rtl_SendReqColl(0.);
          }

          for(i=0; i < VMS->ProcCount; i++)
              if(VMS->VProc[i].lP != SenderProcNum)
                ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

          dvm_FreeArray(Req);     
       }
    }
    else
    {
       if(VMS->HasCurrent)
       {
          ( RTL_CALL, rtl_Recvnowait(buf, count, size, SenderProcNum,
                                     DVM_VMS->tag_BroadCast, &InReq,
                                     0) );
          ( RTL_CALL, rtl_Waitrequest(&InReq) );
       }
    }
  }

  if(RTL_TRACE)
     dvm_trace(ret_rtl_BroadCast," \n");

  DVMFTimeFinish(ret_rtl_BroadCast);

  (DVM_RET);
  return;
}



DvmType  __callstd  bcast_(AddrType  *ArrayAddrPtr, DvmType  *ElmTypePtr,
                          DvmType  *ElmNumberPtr, DvmType  *SenderProcPtr,
                          PSRef  *PSRefPtr)
{ int      Count, Size, SenderProcNum, Length;
  char    *Mem, *MemPtr;


  switch(*ElmTypePtr)
  { case  1: Size = sizeof(int);
             break;
    case  2: Size = sizeof(DvmType);
             break;
    case  3: Size = sizeof(float);
             break;
    case  4: Size = sizeof(double);
             break;
    default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 219.006: wrong call bcast_\n"
                      "(invalid element type %ld)\n", *ElmTypePtr);
  }

  Count = (int)*ElmNumberPtr;

  if(Count <= 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 219.007: wrong call bcast_\n"
              "(invalid element number %d)\n", Count);

  Mem = (char *)*ArrayAddrPtr;
  SenderProcNum = (int)*SenderProcPtr;
  Length = Count * Size;

  if((IsSynchr && UserSumFlag) || (Length & Msk3))
  {  mac_malloc(MemPtr, char *, Length, 0);

     if(MPS_CurrentProc == SenderProcNum)
        SYSTEM(memcpy, (MemPtr, Mem, Length))

     ( RTL_CALL, rtl_BroadCast(MemPtr, Count, Size, SenderProcNum,
                               PSRefPtr) );

     if(MPS_CurrentProc != SenderProcNum)
        SYSTEM(memcpy, (Mem, MemPtr, Length))

     mac_free((void **)&MemPtr); 
  }
  else
     ( RTL_CALL, rtl_BroadCast(Mem, Count, Size, SenderProcNum,
                               PSRefPtr) );

  return  0;
}



DvmType  __callstd  bsynch_(void)
{
  int            i;
  RTL_Request   *Req;
  RTL_Request    InReq;
  int            BufArr[2+sizeof(double)/sizeof(int)];

  DVMFTimeStart(call_bsynch_);

  if(RTL_TRACE)
     dvm_trace(call_bsynch_," \n");

  i = 1;

#ifdef _DVM_MPI_

  if(MPIBarrier)
  {
     if(DVM_VMS->Is_MPI_COMM != 0)
     {
        i = 0;
        if(MPIInfoPrint && StatOff == 0)
           MPI_BarrierTime -= dvm_time();

        MPI_Barrier(DVM_VMS->PS_MPI_COMM);

        if(MPIInfoPrint && StatOff == 0)
           MPI_BarrierTime += dvm_time();
     }
     else
        i = 2;
  }
  else
     i = 1;

  if(RTL_TRACE && TstTraceEvent(call_bsynch_))
     tprintf("*** DoNotMPIBarrier = %d\n", i);
  if(MPIBarrierPrint && _SysInfoPrint && MPS_CurrentProc == DVM_IOProc)
     rtl_iprintf("\n*** RTS: DoNotMPIBarrier = %d\n", i);

#endif

  if(i != 0)
  {
    /* Forward to the next element of message tag circle tag_BroadCast 
     for the current processor system */    /*E0319*/

    DVM_VMS->tag_BroadCast++;

    if((DVM_VMS->tag_BroadCast - (msg_BroadCast)) >= TagCount)
       DVM_VMS->tag_BroadCast = msg_BroadCast;

    /* ------------------------------------------------ */    /*E0320*/

    if(MPS_CurrentProc == DVM_CentralProc)
    {
       if(DVM_ProcCount > 1)
       {
          dvm_AllocArray(RTL_Request, DVM_ProcCount, Req);

          for(i=0; i < DVM_ProcCount; i++)
              if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                ( RTL_CALL, rtl_Recvnowait(BufArr, 1, sizeof(int),
                                           (int)DVM_VMS->VProc[i].lP,
                                            DVM_VMS->tag_BroadCast,
                                            &Req[i], 0) );

          for(i=0; i < DVM_ProcCount; i++)
              if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
                 ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

          dvm_FreeArray(Req);     
       }
    }
    else
    {
       ( RTL_CALL, rtl_Sendnowait(BufArr, 1, sizeof(int),
                                  DVM_CentralProc,
                                  DVM_VMS->tag_BroadCast, &InReq, 0) );
       ( RTL_CALL, rtl_Waitrequest(&InReq) );
    }

    ( RTL_CALL, rtl_BroadCast(BufArr, 1, sizeof(int), DVM_CentralProc,
                              NULL) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_bsynch_," \n");

  DVMFTimeFinish(ret_bsynch_);

  return  (DVM_RET, 0);
}



DvmType  __callstd  tsynch_(void)
{
  int      i, j;
  double  *TimeArray, *MesTimeArray;
  double   CurrTime[2], AnsTime[2];

#ifdef _WIN_MPI_
   TimeEqualizationCount = 1;
#endif

  if(TimeEqualizationCount < 1)
     TimeEqualizationCount = 1;

  if(RTL_TRACE)
     dvm_trace(call_tsynch_,"TimeEqualizationCount = %d\n",
                             TimeEqualizationCount);

  dvm_TimeDelta = 0.;

  if(MPS_CurrentProc == DVM_CentralProc)
  {
     mac_calloc(TimeArray, double *, DVM_ProcCount, sizeof(double), 0);
     mac_calloc(MesTimeArray, double *, DVM_ProcCount,
                sizeof(double), 0);

     for(i=0; i < DVM_ProcCount; i++)
     {
         TimeArray[i] = 0.;
         MesTimeArray[i] = 1.e+300;
     }

     for(i=0; i < DVM_ProcCount; i++)
         if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
            ( RTL_CALL, rtl_Recv(AnsTime, 1, sizeof(double),
                                 (int)DVM_VMS->VProc[i].lP) );

     for(i=0; i < TimeEqualizationCount; i++)
     {
        for(j=0; j < DVM_ProcCount; j++)
        {
           if(DVM_VMS->VProc[j].lP == DVM_CentralProc)
              continue;

           CurrTime[0] = dvm_time();

           ( RTL_CALL, rtl_Send(CurrTime, 1, sizeof(double),
                                (int)DVM_VMS->VProc[j].lP) );
           ( RTL_CALL, rtl_Recv(AnsTime, 1, sizeof(double), 
                                (int)DVM_VMS->VProc[j].lP) );
           CurrTime[1] = dvm_time();
           AnsTime[1] = CurrTime[1] - CurrTime[0];
           AnsTime[0] = (CurrTime[0] + CurrTime[1])/2. - AnsTime[0];

           if(AnsTime[1] < MesTimeArray[j])
           {
              MesTimeArray[j] = AnsTime[1];
              TimeArray[j] = AnsTime[0];
           } 
        }
     }

     for(i=0; i < DVM_ProcCount; i++)
     {
        CurrTime[0] = TimeArray[i];

        if(DVM_VMS->VProc[i].lP != DVM_CentralProc)
           ( RTL_CALL, rtl_Send(CurrTime, 1, sizeof(double),
                                (int)DVM_VMS->VProc[i].lP) );
     }

     mac_free((void **)&TimeArray);
     mac_free((void **)&MesTimeArray);
  }
  else
  {
     ( RTL_CALL, rtl_Send(AnsTime, 1, sizeof(double),
                          DVM_CentralProc) );

     for(i=0; i < TimeEqualizationCount; i++)
     {
        ( RTL_CALL, rtl_Recv(AnsTime, 1, sizeof(double),
                             DVM_CentralProc) );
        AnsTime[0] = dvm_time();
        ( RTL_CALL, rtl_Send(AnsTime, 1, sizeof(double),
                             DVM_CentralProc) );
     }

     ( RTL_CALL, rtl_Recv(AnsTime, 1, sizeof(double), DVM_CentralProc) );

     dvm_TimeDelta = AnsTime[0];
  }

  if(RTL_TRACE)
     dvm_trace(ret_tsynch_," \n");

  return  (DVM_RET, 0);
}


/****************************************************************\
* Check that only one exchange between two processors is allowed *
\****************************************************************/    /*E0321*/

/* Installation */    /*E0322*/

void CheckRendezvousInit(void)
{ int i;

  if(CheckRendezvous)
  {  dvm_AllocArray(RTL_Request *, ProcCount, SendRendezvousArray);
     dvm_AllocArray(RTL_Request *, ProcCount, RecvRendezvousArray);

     for(i=0; i < ProcCount; i++)
     {  SendRendezvousArray[i] = NULL;
        RecvRendezvousArray[i] = NULL;
     }
  }

  return;
}


/******************************************************\
* Print array in byte form before sending or receiving *
\******************************************************/    /*E0323*/

void  trc_PrintBuffer(char  *buffer, DvmType  length, int  EventNumber)
{ int   nb;
  DvmType  NB;

  if(RTL_TRACE == 0)
     return;  /* trace is off */    /*E0324*/

  if(PrintBufferByteCount == 0 || IsEvent[EventNumber] == 0)
     return;  /* array is not to be output
                 or trace is off */    /*E0325*/

  if(FullTrace == 0 && IsEvent[EventNumber] == 1)
     return;  /* short trace mode */    /*E0326*/

  if(DVM_LEVEL > MaxTraceLevel || DVM_LEVEL > MaxEventLevel[EventNumber])
     return;  /* current level of function call nesting is more than
                 maximal trace level */    /*E0327*/

  NB=dvm_min(PrintBufferByteCount,length);
  tprintf("Buffer= ");

  for(nb=0; nb < NB; nb++)
      tprintf("%2.2x ",(byte)buffer[nb]);
  tprintf("\n\n");
}


/* */    /*E0328*/

int  rtl_TstReqColl(int  DoPLSign)
{ int            i, j, Res = 0, Init, Last, rc,
                 SaveCurrOper, LastCurrOper;
  byte           IsDVMMTimeStart = 0;
  RTL_Request   *RTL_ReqPtr;

  if(RTL_TRACE && MsgScheduleTrace)
     tprintf("*** call rtl_TstReqColl: DoPLSign=%d\n"
             "MaxMsgSendNumber=%d "
             "FreeChanNumber=%d NewMsgNumber=%d\n",
             DoPLSign, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber);
            
  if(FreeChanNumber == ParChanNumber)
     return  Res;

  SaveCurrOper = CurrOper; /* */    /*E0329*/
  LastCurrOper = CurrOper; /* */    /*E0330*/

  for(i=0; i < ParChanNumber; i++)
  {  /* */    /*E0331*/

     if(i == DeactChan)
        continue;  /* */    /*E0332*/

     RTL_ReqPtr = ChanRTL_ReqPtr[i];

     if(RTL_ReqPtr == NULL)
        continue;  /* */    /*E0333*/

     Init = RTL_ReqPtr->Init;
     Last = RTL_ReqPtr->Last;

     if(Init > Last)
        continue;   /* */    /*E0334*/

     #ifdef _F_TIME_

        if(RTL_ReqPtr->CurrOper != LastCurrOper)
        {  LastCurrOper = RTL_ReqPtr->CurrOper; /* */    /*E0335*/
           SetHostOper(LastCurrOper)
        }

        if(IsDVMMTimeStart == 0 &&
           StatGrpStack[DVMCallLevel].GrpNumber != MsgPasGrp)
        {  DVMMTimeStart(call_rtl_Testrequest); /* */    /*E0336*/
           IsDVMMTimeStart = 1;
        }

     #endif

     for(j=Init; j <= Last; j++)
     {  /* */    /*E0337*/

        rc = mps_Testrequest1(&RTL_ReqPtr->MPSFlagArray[j]);

        #ifdef  _DVM_MPI_
           if(rc == 0)
           {  /* */    /*E0338*/

              if(DoPLSign && dopl_MPI_Test)
              {  /* */    /*E0339*/

                 MPI_Status   Status;
                 int          MPI_RC, MaxLast;
      
                 MaxLast  = dvm_min(Last-j, dopl_MPI_Test_Count);
                 MaxLast += j;

                 for(rc=j+1; rc <=MaxLast; rc++)
                     SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlagArray[rc],
                            &MPI_RC, &Status))
              }

              break;
           }
        #else
           if(rc == 0)
              break;  /* */    /*E0340*/
        #endif

        Res = 1; /* */    /*E0341*/

        mps_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[j]);

        /* */    /*E0342*/

        MaxMsgSendNumber++;
        ChanMsgSendNumber[i]++;
     }

     if(RTL_TRACE && MsgScheduleTrace > 1 && RTL_ReqPtr->Init != j)
        tprintf("*** rtl_TstReqColl: DoPLSign=%d\n"
           "Chan=%d ChanMsgSendNumber=%d ReqPtr=%lx "
           "Proc=%d Init=%d Last=%d\n",
           DoPLSign, i, ChanMsgSendNumber[i], (uLLng)RTL_ReqPtr,
           RTL_ReqPtr->ProcNumber, j, RTL_ReqPtr->Last);

     RTL_ReqPtr->Init = j; /* */    /*E0343*/

     if(j == RTL_ReqPtr->FlagNumber)
     {  /* */    /*E0344*/

        RTL_ReqPtr->Chan = -1; /* */    /*E0345*/
        ChanRTL_ReqPtr[i] = NULL;   /* */    /*E0346*/
        FreeChanNumber++;       /* */    /*E0347*/
     }
  }

  if(RTL_TRACE && MsgScheduleTrace)
     tprintf("*** ret rtl_TstReqColl: DoPLSign=%d\n"
             "MaxMsgSendNumber=%d "
             "FreeChanNumber=%d NewMsgNumber=%d\n",
             DoPLSign, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber);

  #ifdef _F_TIME_

     if(IsDVMMTimeStart)
     {  DVMMTimeFinish;
     }

     if(LastCurrOper != SaveCurrOper)
        SetHostOper(SaveCurrOper) /* */    /*E0348*/
  #endif

  return  Res;
}



int  rtl_SendReqColl(double  Coeff)
{ int            i, j, k, Count, Res = 0, MaxMsgParts, ChanNumber,
                 MsgChanNumber, MaxChanMsg, InitPart, LastPart, procnum,
                 MsgLength, tag, rc, SaveCurrOper, LastCurrOper;
  void         **List;
  RTL_Request   *RTL_ReqPtr;
  byte           CondTrue = 1, IsDVMMTimeStart = 0;
  char          *CharPtr;

  Count = SendReqColl.Count;
  List  = SendReqColl.List;

  if(RTL_TRACE && MsgScheduleTrace)
     tprintf("*** call rtl_SendReqColl: Coeff=%lf\n"
        "MaxMsgSendNumber=%d "
        "FreeChanNumber=%d NewMsgNumber=%d MsgNumber=%d\n",
        Coeff, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber, Count);

  /* */    /*E0349*/

  if(FreeChanNumber && NewMsgNumber)
  {  /* */    /*E0350*/

     for(i=0; i < Count; i++)
     {  RTL_ReqPtr = (RTL_Request *)List[i];

        if(RTL_ReqPtr->Last == -1 && RTL_ReqPtr->Chan == -1)
        {  /* */    /*E0351*/

           for(j=0; j < ParChanNumber; j++)
           {  if(ChanRTL_ReqPtr[j] == NULL)
                 continue;  /* */    /*E0352*/

              if((ChanRTL_ReqPtr[j])->ProcNumber ==
                 RTL_ReqPtr->ProcNumber)
                 break;  /* */    /*E0353*/
           }

           if(j != ParChanNumber)
              continue; /* */    /*E0354*/

           for(j=0; j < ParChanNumber; j++)
           {  if(ChanRTL_ReqPtr[j] == NULL)
                 break;  /* */    /*E0355*/
           }

           if(j == ParChanNumber)
              break; /* */    /*E0356*/

           ChanRTL_ReqPtr[j] = RTL_ReqPtr;
           RTL_ReqPtr->Chan = j;
           FreeChanNumber--;
           NewMsgNumber--;

           if(RTL_TRACE && MsgScheduleTrace > 1)
              tprintf("*** New Msg rtl_SendReqColl: Coeff=%lf\n"
                 "MaxMsgSendNumber=%d "
                 "FreeChanNumber=%d NewMsgNumber=%d MsgNumber=%d\n"
                 "Chan=%d ChanMsgSendNumber=%d ReqPtr=%lx "
                 "Proc=%d Init=%d Last=%d\n",
                 Coeff, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber,
                 Count, j, ChanMsgSendNumber[j], (uLLng)RTL_ReqPtr,
                 RTL_ReqPtr->ProcNumber, RTL_ReqPtr->Init,
                 RTL_ReqPtr->Last);

           if(FreeChanNumber == 0 || NewMsgNumber == 0)
              break;
        }
     }
  }

  if(MaxMsgSendNumber < 1)
     return  Res;  /* */    /*E0357*/
 
  SaveCurrOper = CurrOper; /* */    /*E0358*/
  LastCurrOper = CurrOper; /* */    /*E0359*/

  /* */    /*E0360*/

  MaxMsgParts = (int)(InitMaxMsgSendNumber * Coeff);
  MaxMsgParts = dvm_min(MaxMsgParts, MaxMsgSendNumber);

  if(MaxMsgParts < 1)
     MaxMsgParts = MaxMsgSendNumber; /* */    /*E0361*/

  /* */    /*E0362*/

  while(MaxMsgParts > 0)
  {  /* */    /*E0363*/

     ChanNumber = 0;    /* */    /*E0364*/
     MsgChanNumber = 0; /* */    /*E0365*/
     
     for(i=0; i < ParChanNumber; i++)
     {  PlanChanList[i] = 0; /* */    /*E0366*/

        RTL_ReqPtr = ChanRTL_ReqPtr[i];

        if(RTL_ReqPtr == NULL)
           continue;  /* */    /*E0367*/

        if(i == DeactChan)
           continue;  /* */    /*E0368*/

        j = RTL_ReqPtr->FlagNumber - RTL_ReqPtr->Last
            - 1; /* */    /*E0369*/

        if(j <= 0)
           continue;  /* */    /*E0370*/

        j = dvm_min(j, ChanMsgSendNumber[i]);

        if(j <= 0)
           continue;  /* */    /*E0371*/

        PlanChanList[i] = j;    /* */    /*E0372*/
        MsgChanList[i]  = RTL_ReqPtr->Last - RTL_ReqPtr->Init
                          + 1;  /* */    /*E0373*/
        MsgChanNumber += MsgChanList[i];        
        ChanNumber++;
     }

     if(ChanNumber == 0)
        break;  /* */    /*E0374*/

     /* */    /*E0375*/

     while(CondTrue)
     {  j = MaxMsgParts / ChanNumber; /* */    /*E0376*/
        if(j > 0)
           break;          /* */    /*E0377*/

        MaxChanMsg = 0;  /* */    /*E0378*/
        Count = -1;      /* */    /*E0379*/

        for(i=0; i < ParChanNumber; i++)
        {  if(PlanChanList[i] == 0)
              continue;  /* */    /*E0380*/
           if(MsgChanList[i] > MaxChanMsg)
           {  MaxChanMsg = MsgChanList[i];
              Count = i;
           }
        }

        if(Count < 0)
           break;   /* */    /*E0381*/

        /* */    /*E0382*/

        ChanNumber--;
        MsgChanNumber -= MaxChanMsg;
        PlanChanList[Count] = 0;

        if(ChanNumber == 0)
           break;   /* */    /*E0383*/
     }

     if(j <= 0)
        break; /* */    /*E0384*/

     j = (MsgChanNumber + MaxMsgParts) /
         ChanNumber;    /* */    /*E0385*/

     /* */    /*E0386*/

     for(i=0; i < ParChanNumber; i++)
     {  if(PlanChanList[i] == 0)
           continue;  /* */    /*E0387*/

        Count = j - MsgChanList[i];

        if(Count <= 0)
           continue;  /* */    /*E0388*/

        /* */    /*E0389*/

        RTL_ReqPtr = ChanRTL_ReqPtr[i];

        #ifdef _F_TIME_

        if(RTL_ReqPtr->CurrOper != LastCurrOper)
        {  LastCurrOper = RTL_ReqPtr->CurrOper; /* */    /*E0390*/
           SetHostOper(LastCurrOper)
        }

        if(IsDVMMTimeStart == 0 &&
           StatGrpStack[DVMCallLevel].GrpNumber != MsgPasGrp)
        {  DVMMTimeStart(call_rtl_Sendnowait); /* */    /*E0391*/
           IsDVMMTimeStart = 1;
        }

        #endif

        Count = dvm_min(Count, PlanChanList[i]); /* */    /*E0392*/
        Res += Count; /* */    /*E0393*/
        MaxMsgSendNumber -= Count; /* */    /*E0394*/
        MaxMsgParts -= Count;      /* */    /*E0395*/
        ChanMsgSendNumber[i] -= Count; /* */    /*E0396*/

        if(RTL_ReqPtr->CompressBuf == NULL)
           CharPtr   = RTL_ReqPtr->BufAddr;
        else
           CharPtr   = RTL_ReqPtr->CompressBuf;
           
        InitPart  = RTL_ReqPtr->Last + 1;
        LastPart  = RTL_ReqPtr->Last + Count;
        procnum   = RTL_ReqPtr->ProcNumber;
        MsgLength = RTL_ReqPtr->MsgLength;
        tag       = RTL_ReqPtr->tag;

        CharPtr += (InitPart * MsgLength);

        for(k=InitPart; k < LastPart; k++)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();   /* for statistics */    /*E0397*/

           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[k],
                                tag);

           /* For statistics. rtl_SendReqColl. */    /*E0398*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              SendCallTime   += CommTime;
              MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
              MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
              SendCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->SendCallTime    += CommTime;
                 SendRecvTimesPtr->MinSendCallTime  =
                 dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                 SendRecvTimesPtr->MaxSendCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                 SendRecvTimesPtr->SendCallCount++;
              }
           }

           /* -------------- */    /*E0399*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0400*/

        if((LastPart+1) == RTL_ReqPtr->FlagNumber &&
           RTL_ReqPtr->Remainder != 0)
           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                RTL_ReqPtr->Remainder,
                                &RTL_ReqPtr->MPSFlagArray[k], tag);
        else
           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[k],
                                tag);

        /* For statistics. rtl_SendReqColl. */    /*E0401*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           SendCallTime   += CommTime;
           MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
           MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
           SendCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->SendCallTime    += CommTime;
              SendRecvTimesPtr->MinSendCallTime  =
              dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
              SendRecvTimesPtr->MaxSendCallTime  =
              dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
              SendRecvTimesPtr->SendCallCount++;
           }
        }

        /* -------------- */    /*E0402*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                   rc);

        RTL_ReqPtr->Last = LastPart;

        if(RTL_TRACE && MsgScheduleTrace > 1)
           tprintf("*** Chan Msg rtl_SendReqColl: Coeff=%lf\n"
              "MaxMsgSendNumber=%d "
              "FreeChanNumber=%d NewMsgNumber=%d MsgNumber=%d\n"
              "Chan=%d ChanMsgSendNumber=%d ReqPtr=%lx "
              "Proc=%d Init=%d Last=%d\n",
              Coeff, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber,
              Count, i, ChanMsgSendNumber[i], (uLLng)RTL_ReqPtr,
              RTL_ReqPtr->ProcNumber, RTL_ReqPtr->Init,
              RTL_ReqPtr->Last);
     }
  }

  if(RTL_TRACE && MsgScheduleTrace)
     tprintf("*** ret rtl_SendReqColl: Coeff=%lf\n"
        "MaxMsgSendNumber=%d "
        "FreeChanNumber=%d NewMsgNumber=%d MsgNumber=%d\n",
        Coeff, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber, Count);

  #ifdef _F_TIME_

     if(IsDVMMTimeStart)
     {  DVMMTimeFinish;
     }

     if(LastCurrOper != SaveCurrOper)
        SetHostOper(SaveCurrOper) /* */    /*E0403*/
  #endif

  return  Res;
}



void  rtl_FreeChan(int  Chan)
{ RTL_Request   *RTL_ReqPtr;
  int            procnum, MsgLength, tag, Count, InitPart, LastPart,
                 k, rc, SaveCurrOper;
  char          *CharPtr;
  byte           IsDVMMTimeStart = 0; 

  RTL_ReqPtr = ChanRTL_ReqPtr[Chan];

  if(RTL_TRACE && MsgScheduleTrace)
  {  if(RTL_ReqPtr == NULL) 
        tprintf("*** call_ret rtl_FreeChan: Chan=%d\n"
           "MaxMsgSendNumber=%d FreeChanNumber=%d "
           "NewMsgNumber=%d ReqPtr=NULL\n",
           Chan, MaxMsgSendNumber, FreeChanNumber, NewMsgNumber);
     else
        tprintf("*** call rtl_FreeChan: Chan=%d\n"
           "MaxMsgSendNumber=%d ChanMsgSendNumber=%d "
           "FreeChanNumber=%d NewMsgNumber=%d\n"
           "ReqPtr=%lx Proc=%d Init=%d Last=%d\n",
           Chan, MaxMsgSendNumber, ChanMsgSendNumber[Chan],
           FreeChanNumber, NewMsgNumber, (uLLng)RTL_ReqPtr,
           RTL_ReqPtr->ProcNumber, RTL_ReqPtr->Init, RTL_ReqPtr->Last);
  }

  if(RTL_ReqPtr == NULL)
     return;  /* */    /*E0404*/

  SaveCurrOper = CurrOper; /* */    /*E0405*/

  #ifdef _F_TIME_

  SetHostOper(RTL_ReqPtr->CurrOper)

  if(StatGrpStack[DVMCallLevel].GrpNumber != MsgPasGrp)
  {  DVMMTimeStart(call_rtl_Waitrequest); /* */    /*E0406*/
     IsDVMMTimeStart = 1;
  }

  #endif

  procnum   = RTL_ReqPtr->ProcNumber;
  MsgLength = RTL_ReqPtr->MsgLength;
  tag       = RTL_ReqPtr->tag;

  DeactChan = Chan; /* */    /*E0407*/

  /* */    /*E0408*/

  while(RTL_ReqPtr->FlagNumber != RTL_ReqPtr->Init)
  {  Count = RTL_ReqPtr->FlagNumber - RTL_ReqPtr->Last
             - 1;  /* */    /*E0409*/

     Count = dvm_min(Count, ChanMsgSendNumber[Chan]);

     if(Count > 0)
     {  /* */    /*E0410*/

        MaxMsgSendNumber -= Count; /* */    /*E0411*/
        ChanMsgSendNumber[Chan] -= Count; /* */    /*E0412*/
        if(RTL_ReqPtr->CompressBuf == NULL)
           CharPtr   = RTL_ReqPtr->BufAddr;
        else
           CharPtr   = RTL_ReqPtr->CompressBuf;

        InitPart  = RTL_ReqPtr->Last + 1;
        LastPart  = RTL_ReqPtr->Last + Count;

        CharPtr += (InitPart * MsgLength);
   
        for(k=InitPart; k < LastPart; k++)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();

           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[k],
                                tag);

           /* For statistics. rtl_SendFreeChan. */    /*E0413*/

           if(_SendRecvTime && StatOff == 0)
           {  CommTime        = dvm_time() - CommTime;
              SendCallTime   += CommTime;
              MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
              MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
              SendCallCount++;

              if(RTL_STAT)
              {  SendRecvTimesPtr = (s_SendRecvTimes *)
                 &CurrInterPtr[StatGrpCountM1][StatGrpCount];

                 SendRecvTimesPtr->SendCallTime    += CommTime;
                 SendRecvTimesPtr->MinSendCallTime  =
                 dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
                 SendRecvTimesPtr->MaxSendCallTime  =
                 dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
                 SendRecvTimesPtr->SendCallCount++;
              }
           }

           /* -------------- */    /*E0414*/

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* for statistics */    /*E0415*/

        if((LastPart+1) == RTL_ReqPtr->FlagNumber &&
           RTL_ReqPtr->Remainder != 0)
           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                RTL_ReqPtr->Remainder,
                                &RTL_ReqPtr->MPSFlagArray[k], tag);
        else
           rc = mps_Sendnowait1(ProcIdentList[procnum], CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[k],
                                tag);

        /* For statistics. rtl_SendFreeChan. */    /*E0416*/

        if(_SendRecvTime && StatOff == 0)
        {  CommTime        = dvm_time() - CommTime;
           SendCallTime   += CommTime;
           MinSendCallTime = dvm_min(MinSendCallTime, CommTime);
           MaxSendCallTime = dvm_max(MaxSendCallTime, CommTime);
           SendCallCount++;

           if(RTL_STAT)
           {  SendRecvTimesPtr = (s_SendRecvTimes *)
              &CurrInterPtr[StatGrpCountM1][StatGrpCount];

              SendRecvTimesPtr->SendCallTime    += CommTime;
              SendRecvTimesPtr->MinSendCallTime  =
              dvm_min(SendRecvTimesPtr->MinSendCallTime, CommTime);
              SendRecvTimesPtr->MaxSendCallTime  =
              dvm_max(SendRecvTimesPtr->MaxSendCallTime, CommTime);
              SendRecvTimesPtr->SendCallCount++;
           }
        }

        /* -------------- */    /*E0417*/

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.002: mps_Sendnowait1 rc = %d\n",
                   rc);

        RTL_ReqPtr->Last = LastPart;
     }

     /* */    /*E0418*/

     if(MsgWaitReg)
     { while(mps_Testrequest1(
             &RTL_ReqPtr->MPSFlagArray[RTL_ReqPtr->Init])
             == 0)
             continue;
     }

     mps_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[RTL_ReqPtr->Init]);

     MaxMsgSendNumber++;
     ChanMsgSendNumber[Chan]++;
     RTL_ReqPtr->Init++;

     if(FreeChanReg)
     {  rc = rtl_TstReqColl(0); /* */    /*E0419*/

        if(FreeChanReg > 1 && rc)
           rtl_SendReqColl(ResCoeffWaitReq); /* */    /*E0420*/
     }
  }

  RTL_ReqPtr->Chan = -1;        /* */    /*E0421*/
  ChanRTL_ReqPtr[Chan] = NULL;  /* */    /*E0422*/
  FreeChanNumber++;             /* */    /*E0423*/

  DeactChan = -1; /* */    /*E0424*/

  if(RTL_TRACE && MsgScheduleTrace)
     tprintf("*** ret rtl_FreeChan: Chan=%d\n"
            "MaxMsgSendNumber=%d ChanMsgSendNumber=%d "
            "FreeChanNumber=%d NewMsgNumber=%d\n"
            "ReqPtr=%lx Proc=%d Init=%d Last=%d\n",
            Chan, MaxMsgSendNumber, ChanMsgSendNumber[Chan],
            FreeChanNumber, NewMsgNumber, (uLLng)RTL_ReqPtr,
            RTL_ReqPtr->ProcNumber, RTL_ReqPtr->Init, RTL_ReqPtr->Last);

  #ifdef _F_TIME_

     if(IsDVMMTimeStart)
     {  DVMMTimeFinish;
     }

     SetHostOper(SaveCurrOper) /* */    /*E0425*/
  #endif

  return;
}


/* */    /*E0426*/

#ifdef _DVM_MPI_

int  compress_malloc(void  **MsgBuf, int  CompressLen)
{ int      i, j;
  void   **VoidPtrPtr;
  void    *VoidPtr;

  dvm_compress0++; /* */    /*E0427*/

  /* */    /*E0428*/

  for(i=0,j=-1; i < CompressBufNumber; i++)
  {  if(FreeCompressBuf[i] == 0)
        continue;                 /* */    /*E0429*/
     if(CompressBuf[i] == NULL)
        continue;                 /* */    /*E0430*/
     if(CompressBufSize[i] < CompressLen)
        continue;                 /* */    /*E0431*/

     if(j == -1)
        j = i;
     else
     {  if(CompressBufSize[i] < CompressBufSize[j])
           j = i;
     }
  }

  if(j >= 0)
  {  /* */    /*E0432*/

     dvm_compress1++; /* */    /*E0433*/

     *MsgBuf            = CompressBuf[j];
     FreeCompressBuf[j] = 0;

     return  j;
  }

  /* */    /*E0434*/

  /* */    /*E0435*/

  for(i=0; i < CompressBufNumber; i++)
  {  if(FreeCompressBuf[i] == 0)
        continue;                 /* */    /*E0436*/
     if(CompressBuf[i] == NULL)
        continue;                 /* */    /*E0437*/

     if(j == -1)
        j = i;
     else
     {  if(CompressBufSize[i] > CompressBufSize[j])
           j = i;
     }
  }

  if(j >= 0)
  {  /* */    /*E0438*/

     dvm_compress2++; /* */    /*E0439*/

     VoidPtrPtr = &CompressBuf[j];
     mac_free(VoidPtrPtr);

     mac_malloc(VoidPtr, void *, CompressLen, 0);
     *MsgBuf            = VoidPtr;
     CompressBuf[j]     = VoidPtr;
     FreeCompressBuf[j] = 0;
     CompressBufSize[j] = CompressLen;

     return  j;
  }

  /* */    /*E0440*/

  /* */    /*E0441*/

  for(i=0; i < CompressBufNumber; i++)
      if(FreeCompressBuf[i] == 1)
         break;                   /* */    /*E0442*/

  if(i == CompressBufNumber)
  {  /* */    /*E0443*/

     dvm_compress3++; /* */    /*E0444*/

     mac_malloc(VoidPtr, void *, CompressLen, 0);
     *MsgBuf = VoidPtr;

     return  -1;
  }

  /* */    /*E0445*/

  dvm_compress4++; /* */    /*E0446*/

  mac_malloc(VoidPtr, void *, CompressLen, 0);
  *MsgBuf = VoidPtr;
  CompressBuf[i]     = VoidPtr;
  FreeCompressBuf[i] = 0;
  CompressBufSize[i] = CompressLen;

  return  i;
}

#endif


/* */    /*E0447*/

#if defined(_DVM_ZLIB_) && defined(_DVM_MPI_)

int  dvm_compress(Bytef *dest, uLongf *destLen, const Bytef *source,
                  uLong sourceLen, int level)
{
    z_stream stream;
    int err, strategy;

    switch(MsgCompressStrategy)
    {  default:
       case 0:  strategy = Z_DEFAULT_STRATEGY;
                break;
       case 1:  strategy = Z_FILTERED;
                break;
       case 2:  strategy = Z_HUFFMAN_ONLY;
                break;
    }

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;

    if((uLong)stream.avail_out != *destLen)
       return  Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit2_(&stream, level, Z_DEFLATED, MAX_WBITS,
                        MAX_MEM_LEVEL, strategy, ZLIB_VERSION,
                        sizeof(z_stream));

    if(err != Z_OK)
       return err;

    err = deflate(&stream, Z_FINISH);

    if(err != Z_STREAM_END)
    {   deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }

    *destLen = stream.total_out;
    err = deflateEnd(&stream);

    return err;
}

#endif
 

#endif  /* _MSGPAS_C_ */    /*E0448*/
