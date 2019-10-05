#ifndef _GNS_MPS_C_
#define _GNS_MPS_C_
/*****************/    /*E0000*/


/* Additional variables for gns_receivenw */    /*E0001*/

int       ReceiveNB;
TASKID    ReceiveProcid;

/* -------------------------------------------- */    /*E0002*/



int  mps_Send(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ int  rc;

  SYSTEM_RET(rc, gns_send, (ProcIdent, BufPtr, ByteCount))
  return  rc;
}



int  mps_Recv(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ int     rc;
  TASKID  source=0;

  SYSTEM_RET(rc, gns_receive, (ProcIdent, &source, BufPtr, ByteCount))
  return  rc;
}



int  mps_Sendnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ int  rc;

  SYSTEM_RET(rc, gns_sendnw, (ProcIdent, &RTL_ReqPtr->MPSFlag, BufPtr,
                              ByteCount))
  return  rc;
}



int  mps_Sendnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ int  rc;

  SYSTEM_RET(rc, gns_sendnw, (ProcIdent, ReqPtr, BufPtr, ByteCount))
  return  rc;
}



int  mps_Recvnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ int  rc;

  SYSTEM_RET(rc, gns_receivenw, (ProcIdent, &ReceiveProcid,
                                 &RTL_ReqPtr->MPSFlag, &ReceiveNB,
                                 BufPtr, ByteCount))
  return  rc;
}



int  mps_Recvnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ int  rc;

  SYSTEM_RET(rc, gns_receivenw, (ProcIdent, &ReceiveProcid,
                                 ReqPtr, &ReceiveNB, BufPtr,
                                 ByteCount))
  return  rc;
}



void  mps_Waitrequest(RTL_Request  *RTL_ReqPtr)
{
  SYSTEM(gns_msgdone, (&RTL_ReqPtr->MPSFlag))
}



void  mps_Waitrequest1(MPS_Request  *ReqPtr)
{ 
  SYSTEM(gns_msgdone, (ReqPtr))
}



int  mps_Testrequest(RTL_Request  *RTL_ReqPtr)
{ int  rc;

  SYSTEM_RET(rc, gns_testflag, (&RTL_ReqPtr->MPSFlag))
  return  rc;
}



int  mps_Testrequest1(MPS_Request  *ReqPtr)
{ int    rc;

  SYSTEM_RET(rc, gns_testflag, (ReqPtr))
  return  rc;
}



int  mps_SendA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{ int  rc, count = 100;

  while(TRUE)
  {  SYSTEM_RET(rc, gns_senda, (ProcIdent, Tag, BufPtr, ByteCount))

     if(rc >= 0)
        break;

     #ifdef _i860_GNS_
        if(count == 0)
        { pprintf(3,"*** RTS warning 213.000: gns_senda rc = %d\n", rc);
          count = 100;
        }
        count--;

        SYSTEM(sleep,(0.1))
     #else
        eprintf(__FILE__,__LINE__,
                "*** RTS err 213.000: gns_senda rc = %d\n", rc);
     #endif
  } 

  return  rc;
}



int  mps_RecvA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{ int     rc;
  TASKID  source=0; 

  SYSTEM_RET(rc, gns_receivea, (ProcIdent, &source, Tag,
                                BufPtr, ByteCount))
  return  rc;
}



void  mps_exit(int rc)
{
  SYSTEM(gns_exit,(rc))
  SYSTEM(exit,(rc))
  return;
}



void  mps_Bcast(void *buf, int count, int size)
{ int           i, nb;
  MPS_Request  *Req;
  MPS_Request   InReq;
  char         *_buf = (char *)buf;
  byte          alloc_buf = 0;

  if(RTL_TRACE)
     dvm_trace(call_mps_Bcast,"buf=%lx; count=%d; size=%d;\n",
                               (ulng)buf, count, size);

  if(IsSlaveRun)
  {  if((count*size) & Msk3)
     {  alloc_buf = 1;
        mac_malloc(_buf, char *,
                   count*size + SizeDelta[(count*size)&Msk3], 0);
     }

     if(CurrentProcIdent == MasterProcIdent)
     {  if(ProcCount > 1)
        {  if(alloc_buf)
              SYSTEM(memcpy, (_buf, buf, count*size))

           dvm_AllocArray(MPS_Request, ProcCount, Req);
 
           for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
               {  SYSTEM_RET(nb,gns_sendnw,(ProcIdentList[i], &Req[i],
                             _buf, count*size +
                                   SizeDelta[(count*size)&Msk3]))
                  if(nb < 0)
                     eprintf(__FILE__,__LINE__,
                      "*** RTS err 213.001: gns_sendnw rc = %d\n", nb);
               }
 
           for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
                  SYSTEM(gns_msgdone, (&Req[i]))
 
           dvm_FreeArray(Req);     
        }
     }
     else
     {  SYSTEM(gns_receivenw, (MasterProcIdent, (TASKID *)&ReceiveProcid,
                               &InReq, (int *)&ReceiveNB, _buf,
                               count*size +
                               SizeDelta[(count*size)&Msk3]))
        SYSTEM(gns_msgdone, (&InReq))

        if(alloc_buf)
           SYSTEM(memcpy, (buf, _buf, count*size))
     }

     if(alloc_buf)
        mac_free(&_buf);
  }

  if(RTL_TRACE)
     dvm_trace(ret_mps_Bcast," \n");

  (DVM_RET);

  return;
}



void  mps_Barrier(void)
{ int           i, nb;
  MPS_Request  *Req;
  MPS_Request   InReq;
  int           buf;

  if(RTL_TRACE)
     dvm_trace(call_mps_Barrier,"\n");

  if(IsSlaveRun)
  {  if(CurrentProcIdent == MasterProcIdent)
     {  if(ProcCount > 1)
        {  dvm_AllocArray(MPS_Request, ProcCount, Req);
   
           for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
                  SYSTEM(gns_receivenw, (ProcIdentList[i],
                         (TASKID *)&ReceiveProcid, &Req[i],
                         (int *)&ReceiveNB, (char *)&buf, sizeof(int)))
   
           for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
                  SYSTEM(gns_msgdone, (&Req[i]))
   
           dvm_FreeArray(Req);     
        }
     }
     else
     {  SYSTEM_RET(nb, gns_sendnw, (MasterProcIdent, &InReq,
                                    (char *)&buf, sizeof(int)))
        if(nb < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 213.002: gns_sendnw rc = %d\n", nb);

        SYSTEM(gns_msgdone, (&InReq))
     }
   
     ( RTL_CALL, mps_Bcast(&buf, 1, sizeof(int)) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_mps_Barrier, "\n");

  (DVM_RET);

  return;
}


#endif /* _GNS_MPS_C_ */    /*E0003*/
