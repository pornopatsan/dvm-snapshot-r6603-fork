#ifndef _EMP_MPS_C_
#define _EMP_MPS_C_
/*****************/    /*E0000*/

int  mps_Send(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Send "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount);
  return  1;
}



int  mps_Recv(int  ProcIdent, void  *BufPtr, int  ByteCount)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Recv "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount);
  return  1;
}



int  mps_Sendnowait(int  ProcIdent, void  *BufPtr,  int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Sendnowait "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; "
          "RTL_ReqPtr=%lx; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, (ulng)RTL_ReqPtr, Tag);
  return  1;
}



int  mps_Sendnowait1(int  ProcIdent, void  *BufPtr,  int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Sendnowait1 "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; "
          "ReqPtr=%lx; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, (ulng)ReqPtr, Tag);
  return  1;
}



int  mps_Recvnowait(int  ProcIdent, void  *BufPtr,  int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Recvnowait "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; "
          "RTL_ReqPtr=%lx; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, (ulng)RTL_ReqPtr, Tag);
  return  1;
}



int  mps_Recvnowait1(int  ProcIdent, void  *BufPtr,  int  ByteCount,
                     MPS_Request  *ReqPtr, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Recvnowait1 "
          "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; "
          "ReqPtr=%lx; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, (ulng)ReqPtr, Tag);
  return  1;
}



void  mps_Waitrequest(RTL_Request  *RTL_ReqPtr)
{
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Waitrequest "
          "(MPS=EMP)\nRTL_ReqPtr=%lx;\n", (ulng)RTL_ReqPtr);
}



void mps_Waitrequest1(MPS_Request  *ReqPtr)
{
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Waitrequest1 "
          "(MPS=EMP)\nReqPtr=%lx;\n", (ulng)ReqPtr);
}



int   mps_Testrequest(RTL_Request  *RTL_ReqPtr)
{
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Testrequest "
          "(MPS=EMP)\nReqPtr=%lx;\n", (ulng)RTL_ReqPtr);
  return  1;
}



int  mps_Testrequest1(MPS_Request  *ReqPtr)
{
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_Testrequest1 "
          "(MPS=EMP)\nReqPtr=%lx;\n", (ulng)ReqPtr);
  return  1;
}



int  mps_SendA(int  ProcIdent, void  *BufPtr,  int  ByteCount, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_SendA "
         "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, Tag);
  return  1;
}



int  mps_RecvA(int  ProcIdent, void  *BufPtr,  int  ByteCount, int  Tag)
{ 
  eprintf(__FILE__,__LINE__,"*** RTS err: wrong call mps_RecvA "
         "(MPS=EMP)\nProcIdent=%d; BufPtr=%lx; ByteCount=%d; Tag=%d;\n",
          ProcIdent, (ulng)BufPtr, ByteCount, Tag);
  return  1;
}



void  mps_exit(int rc)
{
  SYSTEM(exit, (rc))
  return;
}



void mps_Bcast(void *buf,int count,int size)
{ 
  if(RTL_TRACE)
     dvm_trace(call_mps_Bcast,"buf=%lx; count=%d; size=%d;\n",
                               (ulng)buf,count,size);


  if(RTL_TRACE)
     dvm_trace(ret_mps_Bcast,"\n");

  (DVM_RET);

  return;
}



void mps_Barrier(void)
{ 
  if(RTL_TRACE)
     dvm_trace(call_mps_Barrier," \n");


  if(RTL_TRACE)
     dvm_trace(ret_mps_Barrier," \n");

  (DVM_RET);

  return;
}


#endif /* _EMP_MPS_C_ */    /*E0001*/

