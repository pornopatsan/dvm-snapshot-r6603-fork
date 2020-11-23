#ifndef _PVM_MPS_C_
#define _PVM_MPS_C_
/*****************/    /*E0000*/


void  **bufs;
int    *lens;
int    *procs;
int     flreq=0;



int spvm_send(void *buf, int count, int size, int procid)
{ int  rc;

  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkbyte,(buf, count*size, 1))
  SYSTEM_RET(rc,pvm_send,(procid, 10))
  SYSTEM(pvm_recv,(procid, 20))
  SYSTEM(pvm_upkint,(&rc, 1, 1))

  if(rc < 0)
     eprintf(__FILE__,__LINE__,
             "*** RTS err 212.000: pvm_send rc = %d\n", rc);
 
  return  rc;
}



int spvm_recv(void *buf, int count, int size, int procid)
{ int  rc;

  SYSTEM_RET(rc,pvm_recv,(procid, 10))
  if(rc < 0)
     eprintf(__FILE__,__LINE__,
             "*** RTS err 212.001: pvm_recv rc = %d\n", rc);

  SYSTEM(pvm_upkbyte,(buf, count*size, 1))
  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkint,(&rc, 1, 1))

  SYSTEM_RET(rc,pvm_send,(procid, 20))
  if(rc < 0)
     eprintf(__FILE__,__LINE__,
             "*** RTS err 212.002: pvm_send rc = %d\n", rc);

  return  rc;
}



int mps_Send(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ int  rc;

  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkbyte,(BufPtr, ByteCount, 1))
  SYSTEM_RET(rc,pvm_send,(ProcIdent, 11))
  SYSTEM(pvm_recv,(ProcIdent, 21))
  SYSTEM(pvm_upkint,(&rc, 1, 1))

  return  rc;
}



int mps_Recv(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ int  rc;

  SYSTEM_RET(rc,pvm_recv,(ProcIdent, 11))
  SYSTEM(pvm_upkbyte,(BufPtr, ByteCount, 1))
  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkint,(&rc, 1, 1))
  SYSTEM_RET(rc,pvm_send,(ProcIdent, 21))

  return  rc;
}



int  make_req()
{  int  i;

   if(flreq == 1)
      return 0;

   flreq = 1;

   mac_malloc(bufs, void **, 10*ProcCount*sizeof(void *), 0);
   mac_malloc(lens, int *, 10*ProcCount*sizeof(int), 0);
   mac_malloc(procs, int *, 10*ProcCount*sizeof(int), 0);

   for(i=0; i < 10*ProcCount; i++)
   {  bufs[i] = NULL;
      lens[i] = 0;
      procs[i] = 0;
   }
  
   return 1;
}



int mps_Sendnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                   RTL_Request  *RTL_ReqPtr, int  Tag)
{ int  rc, i;

  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkbyte,(BufPtr, ByteCount, 1))
  SYSTEM_RET(rc,pvm_send,(ProcIdent, 1))

  make_req();

  for(i=0; i < 10*ProcCount; i++)
      if(bufs[i] == NULL)
         break;

  if(i == 10*ProcCount)
     rc = -333; 

  bufs[i] = (void *)0xffffffff;
  lens[i] = ByteCount;
  procs[i] = ProcIdent;
  RTL_ReqPtr->MPSFlag = i;

  return  rc;
}



int mps_Sendnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    MPS_Request  *ReqPtr, int  Tag)
{ int  rc, i;

  SYSTEM(pvm_initsend,(PvmDataDefault))
  SYSTEM(pvm_pkbyte,(BufPtr, ByteCount, 1))
  SYSTEM_RET(rc,pvm_send,(ProcIdent, 1))

  make_req();

  for(i=0; i < 10*ProcCount; i++)
      if(bufs[i] == NULL)
         break;

  if(i == 10*ProcCount)
     rc = -333; 

  bufs[i] = (void *)0xffffffff;
  lens[i] = ByteCount;
  procs[i] = ProcIdent;
  *ReqPtr = i;

  return  rc;
}


	
int mps_Recvnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                   RTL_Request  *RTL_ReqPtr, int  Tag)
{ int  rc = 1, i;

  make_req();

  for(i=0; i < 10*ProcCount; i++)
      if(bufs[i] == NULL)
         break;

  if(i == 10*ProcCount)
     rc = -333;

  bufs[i] = BufPtr;
  lens[i] = ByteCount;
  procs[i] = ProcIdent;
  RTL_ReqPtr->MPSFlag = i;

  return  rc;
}



int mps_Recvnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    MPS_Request  *ReqPtr, int  Tag)
{ int  rc = 1, i;

  make_req();

  for(i=0; i < 10*ProcCount; i++)
      if(bufs[i] == NULL)
         break;

  if(i == 10*ProcCount)
     rc = -333;

  bufs[i] = BufPtr;
  lens[i] = ByteCount;
  procs[i] = ProcIdent;
  *ReqPtr = i;

  return  rc;
}



void  mps_Waitrequest(RTL_Request  *RTL_ReqPtr)
{ int  nb, len, tag, ttid;

  if(bufs[RTL_ReqPtr->MPSFlag] != NULL)
  {  if(bufs[RTL_ReqPtr->MPSFlag] != (void *)0xffffffff)
     {  /* wait end of recv */    /*E0001*/

        SYSTEM_RET(nb,pvm_recv,(procs[RTL_ReqPtr->MPSFlag], 1))
        SYSTEM(pvm_bufinfo,(nb, &len, &tag, &ttid))
        SYSTEM(pvm_upkbyte,(bufs[RTL_ReqPtr->MPSFlag],
               lens[RTL_ReqPtr->MPSFlag], 1))
        SYSTEM(pvm_initsend,(PvmDataDefault))
        SYSTEM(pvm_pkint,(&len, 1, 1))
        SYSTEM_RET(nb,pvm_send,(procs[RTL_ReqPtr->MPSFlag], 2))
     }
     else
     {  /* wait end of send */    /*E0002*/

        SYSTEM_RET(nb,pvm_recv,(procs[RTL_ReqPtr->MPSFlag], 2))
        SYSTEM(pvm_bufinfo,(nb, &len, &tag, &ttid))
        SYSTEM(pvm_upkint,(&nb, 1, 1))
     }

     bufs[RTL_ReqPtr->MPSFlag] = NULL;
     RTL_ReqPtr->MPSFlag = 0;
  }

  return;
}



void mps_Waitrequest1(MPS_Request  *ReqPtr)
{ int   nb, len, tag, ttid;

  if(bufs[*ReqPtr] != NULL)
  {  if(bufs[*ReqPtr] != (void *)0xffffffff)
     {  /* */    /*E0003*/

        SYSTEM_RET(nb,pvm_recv,(procs[*ReqPtr], 1))
        SYSTEM(pvm_bufinfo,(nb, &len, &tag, &ttid))
        SYSTEM(pvm_upkbyte,(bufs[*ReqPtr], lens[*ReqPtr], 1))
        SYSTEM(pvm_initsend,(PvmDataDefault))
        SYSTEM(pvm_pkint,(&len, 1, 1))
        SYSTEM_RET(nb,pvm_send,(procs[*ReqPtr], 2))
     }
     else
     {  /* */    /*E0004*/

        SYSTEM_RET(nb,pvm_recv,(procs[*ReqPtr], 2))
        SYSTEM(pvm_bufinfo,(nb, &len, &tag, &ttid))
        SYSTEM(pvm_upkint,(&nb, 1, 1))
     }

     bufs[*ReqPtr] = NULL;
     *ReqPtr = 0;
  }

  return;
}



int mps_Testrequest(RTL_Request  *RTLReqPtr)
{ int  nb, rc = 1;

  if(bufs[RTL_ReqPtr->MPSFlag] != (void *)0xffffffff)
  {  /* wait end of recv */    /*E0005*/

     SYSTEM_RET(rc,pvm_nrecv,(procs[RTL_ReqPtr->MPSFlag], 1))

     if (rc > 0)
     {  SYSTEM(pvm_upkbyte,(bufs[RTL_ReqPtr->MPSFlag],
                            lens[RTL_ReqPtr->MPSFlag], 1))
        SYSTEM(pvm_initsend,(PvmDataDefault))
        SYSTEM(pvm_pkint,(&nb, 1, 1))
        SYSTEM_RET(nb,pvm_send,(procs[RTL_ReqPtr->MPSFlag], 2))

        bufs[RTL_ReqPtr->MPSFlag] = NULL;
        RTL_ReqPtr->MPSFlag = 0;
     }
  }
  else
  {  /* wait end of send */    /*E0006*/

     SYSTEM_RET(rc,pvm_nrecv,(procs[RTL_ReqPtr->MPSFlag], 2))

     if(rc > 0)
     {  SYSTEM(pvm_upkint,(&nb, 1, 1))

        bufs[RTL_ReqPtr->MPSFlag] = NULL;
        RTL_ReqPtr->MPSFlag = 0;
     }
  }

  return  rc;
}



int mps_SendA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{ int  rc;

  while(TRUE)
  {  SYSTEM(pvm_initsend,(PvmDataDefault))
     SYSTEM(pvm_pkbyte,(BufPtr, ByteCount, 1))
     SYSTEM_RET(rc,pvm_send(ProcIdent, Tag))
	
     if(rc == 0)
        break;

     pprintf(3,"*** RTS warning 212.003: pvm_send rc = %d\n", rc);
     SYSTEM(sleep,(0.1))
  } 

  return  rc;
}



int mps_RecvA(int  ProcIdent, void  *BufPtr, int  ByteCount, int  Tag)
{ int  rc;

  SYSTEM_RET(rc,pvm_recv,(ProcIdent, Tag))
  SYSTEM(pvm_upkbyte,(BufPtr, ByteCount, 1))
  return  rc;
}



void mps_exit(int rc)
{
  SYSTEM(pvm_exit,()) 
  SYSTEM(exit, (rc))
  return;
}



void  mps_Bcast(void *buf, int count, int size)
{ int  nb;
  char         *_buf = (char *)buf;
  byte          alloc_buf = 0;

  if(RTL_TRACE)
     dvm_trace(call_mps_Bcast,"buf=%lx; count=%d; size=%d;\n",
                               (ulng)buf,count,size);

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

           SYSTEM(pvm_initsend,(PvmDataDefault))
           SYSTEM(pvm_pkbyte,(_buf, count*size +
                                    SizeDelta[(count*size) & Msk3], 1))
           SYSTEM_RET(nb,pvm_mcast,(&ProcIdentList[1], ProcCount-1, 3))
        }
     }
     else
     {	SYSTEM_RET(nb,pvm_recv,(MasterProcIdent, 3))
        SYSTEM(pvm_upkbyte,(_buf, count*size +
                                  SizeDelta[(count*size) & Msk3], 1))
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



void mps_Barrier(void)
{ int          i;
  int          buf;

  if(RTL_TRACE)
     dvm_trace(call_mps_Barrier," \n");

  if(IsSlaveRun)
  {  if(CurrentProcIdent == MasterProcIdent)
     {  if(ProcCount > 1)
        {  for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
                  SYSTEM(spvm_recv,(&buf, 1, sizeof(int),
                                    ProcIdentList[i]))
   
           for(i=0; i < ProcCount; i++)
               if(ProcIdentList[i] != MasterProcIdent)
               {  buf = 'b';
                  SYSTEM(spvm_send,(&buf, 1, sizeof(int),
                                    ProcIdentList[i]))
               }
   
        }
     }
     else
     {  buf = 'a';
        SYSTEM(spvm_send,(&buf, 1, sizeof(int), ProcIdentList[i]))
        SYSTEM(spvm_recv,(&buf, 1, sizeof(int), ProcIdentList[i]))
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_mps_Barrier," \n");

  (DVM_RET);

  return;
}


#endif /* _PVM_MPS_C_ */    /*E0007*/
