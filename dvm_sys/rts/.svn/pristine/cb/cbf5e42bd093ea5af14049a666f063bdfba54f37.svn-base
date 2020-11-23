#ifndef _V_TURCH_C_
#define _V_TURCH_C_
/*****************/    /*E0000*/

#ifdef _DVM_MPI_

#ifndef _DVM_IOPROC_

int  __callstd  dvmifpc_(void)
{
  #ifdef _WIN_MPI_
     return  1;
  #else
     return  0;
  #endif
}



int  __callstd  dvmifstoreexist_(void)
{ FILE     *f = NULL;
  int       RC = 1;


  f = fopen("/store/dvm_tmp", OPENMODE(r));

  if(f == NULL)
  {  f = fopen("/store/dvm_tmp", OPENMODE(w));

     if(f != NULL)
        fclose(f);
     else
        RC = 0;
  }
  else
     fclose(f);

  return  RC;
}



void  __callstd  dvmremove_(char  *FileNamePtr, int  FileNameLength)
{ int    i, n;
  char  *filename;

  n = 512;

  mac_malloc(filename, char *, n+1, 0);

  for(i=0; i < n; i++)
      filename[i] = FileNamePtr[i];

  filename[n] = 0;

  remove(filename);

  mac_free((void **)&filename);

  return;
}



int  __callstd  dvmgetprocessorname_(int   *ProcNumberPtr,
                                     char  *ProcName,
                                     int   StringLength)
{ int    i, n;
  char  *CharPtr;

  i = ProcIdentList[*ProcNumberPtr];
  CharPtr = MPS_ProcNameList[ApplProcessNumber[i]];
  n = strlen(CharPtr);

  for(i=0; i < n; i++)
      ProcName[i] = CharPtr[i];

  ProcName[n] = 0;

  return  n;
}



void  __callstd  dvmcommrank_(int  *MyRankPtr)
{
  *MyRankPtr = (int)MPS_CurrentProc;
  return;
}



void  __callstd  dvmcommsize_(int  *MySizePtr)
{
  *MySizePtr = (int)ProcCount;
  return;
}



void  __callstd  dvmgetach_(AddrType  *AddrPtr, char  *Ptr, int  Len)
{
  *AddrPtr = getach_(Ptr, Len);
  return;
}



void  __callstd  dvmgetaf_(AddrType  *AddrPtr, float  *Ptr)
{
  *AddrPtr = getaf_(Ptr);
  return;
}



void  __callstd  dvmsend_(void  *sendbufPtr, int  *countPtr,
                          int  *TypePtr, int  *destPtr, int  *tagPtr)
{ int             count, dest, tag, size, rc, MsgLen;
  void           *sendbuf;
  char           *CharPtr;

  DVMMTimeStart(call_dvmsend_);

  sendbuf = sendbufPtr;
  CharPtr = (char *)sendbuf;
  count   = *countPtr;
  dest    = *destPtr;
  tag     = *tagPtr;

  switch(*TypePtr)
  { 
     case rt_CHAR:      size = sizeof(char);
                        break;
     case rt_INT:       size = sizeof(int);
                        break;
     case rt_LONG:      size = sizeof(long);
                        break;
     case rt_LLONG:     size = sizeof(long long);
                        break;
     case rt_FLOAT:     size = sizeof(float);
                        break;
     case rt_DOUBLE:    size = sizeof(double);
                        break;
  }

  if(RTL_TRACE)
     dvm_trace(call_dvmsend_,"buf=%lx; count=%d; size=%d; "
                             "procnum=%d; tag=%d;\n",
                              (uLLng)sendbuf, count, size, dest, tag);
  rc = count * size;

  if(RTL_TRACE)
     trc_PrintBuffer((char *)sendbuf, rc, call_dvmsend_);

  if(IsSynchr)
  {  CharPtr += rc;
     SYSTEM(memcpy,
     (CharPtr, dvm_time_ptr, sizeof(double)) ) 
     MsgLen = rc + sizeof(double) + SizeDelta[rc & Msk3];
  }
  else
     MsgLen = rc + SizeDelta[rc & Msk3];

  if(MPIInfoPrint && StatOff == 0)
  {
     MPISendByteNumber  += MsgLen;

     MPISendByteCount  += MsgLen;
     MPIMsgCount++;
     MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, MsgLen);
     MinMPIMsgLen = dvm_min(MinMPIMsgLen, MsgLen);
  }

  MPI_Send(sendbuf, MsgLen, MPI_BYTE, ProcIdentList[dest],
           tag, DVM_COMM_WORLD);

  if(RTL_TRACE)
     dvm_trace(ret_dvmsend_," \n");

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



void  __callstd  dvmrecv_(void  *recvbufPtr, int  *countPtr,
                          int  *TypePtr, int  *sourcePtr,
                          int  *tagPtr)
{ int             count, source, tag, size, ByteCount, MsgLen;
  void           *recvbuf;
  char           *CharPtr; 

  DVMMTimeStart(call_dvmrecv_);

  recvbuf = recvbufPtr;
  CharPtr = (char *)recvbuf;
  count   = *countPtr;
  source  = *sourcePtr;
  tag     = *tagPtr;

  switch(*TypePtr)
  { 
     case rt_CHAR:      size = sizeof(char);
                        break;
     case rt_INT:       size = sizeof(int);
                        break;
     case rt_LONG:      size = sizeof(long);
                        break;
     case rt_LLONG:     size = sizeof(long long);
                        break;
     case rt_FLOAT:     size = sizeof(float);
                        break;
     case rt_DOUBLE:    size = sizeof(double);
                        break;
  }

  if(RTL_TRACE)
     dvm_trace(call_dvmrecv_,"buf=%lx; count=%d; size=%d; "
                             "procnum=%d; tag=%d;\n",
                              (uLLng)recvbuf, count, size, source, tag);

  ByteCount = count * size;

  if(IsSynchr)
     MsgLen = ByteCount + sizeof(double) + SizeDelta[ByteCount & Msk3];
  else
     MsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  MPI_Recv(recvbuf, MsgLen, MPI_BYTE, ProcIdentList[source],
           tag, DVM_COMM_WORLD, &GMPI_Status);

  if(RTL_TRACE)
     trc_PrintBuffer((char *)recvbuf, ByteCount, call_dvmrecv_); 
  
  if(IsSynchr)
  {  CharPtr += ByteCount;
     SYSTEM(memcpy, (synchr_time_ptr, CharPtr, sizeof(double)))
     Curr_synchr_time = dvm_abs(Curr_synchr_time - Curr_dvm_time);
  }
  else
     Curr_synchr_time = 0.;
 
  if(RTL_TRACE)
     dvm_trace(ret_dvmrecv_," \n");

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



void  __callstd  dvmsendrecv_(void  *sendbufPtr,
                              int  *sendcountPtr, int  *SendTypePtr,
                              int  *destPtr,
                              void  *recvbufPtr,
                              int  *recvcountPtr, int  *RecvTypePtr,
                              int  *sourcePtr, int  *tagPtr)
{ int             sendcount, recvcount, dest, source, tag,
                  sendsize, recvsize, rc, ByteCount, MsgLen, recvMsgLen;
  void           *sendbuf, *recvbuf;
  char           *CharPtr, *recvCharPtr;

  DVMMTimeStart(call_dvmsendrecv_);

  sendbuf     = sendbufPtr;
  CharPtr     = (char *)sendbuf;
  sendcount   = *sendcountPtr;
  dest        = *destPtr;
  recvbuf     = recvbufPtr;
  recvCharPtr = (char *)recvbuf;
  recvcount   = *recvcountPtr;
  source      = *sourcePtr;
  tag         = *tagPtr;

  switch(*SendTypePtr)
  { 
     case rt_CHAR:      sendsize = sizeof(char);
                        break;
     case rt_INT:       sendsize = sizeof(int);
                        break;
     case rt_LONG:      sendsize = sizeof(long);
                        break;
     case rt_LLONG:     sendsize = sizeof(long long);
                        break;
     case rt_FLOAT:     sendsize = sizeof(float);
                        break;
     case rt_DOUBLE:    sendsize = sizeof(double);
                        break;
  }

  switch(*RecvTypePtr)
  { 
     case rt_CHAR:      recvsize = sizeof(char);
                        break;
     case rt_INT:       recvsize = sizeof(int);
                        break;
     case rt_LONG:      recvsize = sizeof(long);
                        break;
     case rt_LLONG:     recvsize = sizeof(long long);
                        break;
     case rt_FLOAT:     recvsize = sizeof(float);
                        break;
     case rt_DOUBLE:    recvsize = sizeof(double);
                        break;
  }

  if(RTL_TRACE)
     dvm_trace(call_dvmsendrecv_,
     "sendbuf=%lx; sendcount=%d; sendsize=%d; "
     "sendproc=%d; tag=%d;\n"
     "recvbuf=%lx; recvcount=%d; recvsize=%d; recvproc=%d;\n",
     (uLLng)sendbuf, sendcount, sendsize, dest, tag,
     (uLLng)recvbuf, recvcount, recvsize, source);

  rc = sendcount * sendsize;

  if(RTL_TRACE)
     trc_PrintBuffer((char *)sendbuf, rc, call_dvmsendrecv_); 

  if(IsSynchr)
  {  CharPtr += rc;
     SYSTEM(memcpy,
     (CharPtr, dvm_time_ptr, sizeof(double)) ) 
     MsgLen = rc + sizeof(double) + SizeDelta[rc & Msk3];
  }
  else
     MsgLen = rc + SizeDelta[rc & Msk3];

  if(MPIInfoPrint && StatOff == 0)
  {
     MPISendByteNumber  += MsgLen;

     MPISendByteCount  += MsgLen;
     MPIMsgCount++;
     MaxMPIMsgLen = dvm_max(MaxMPIMsgLen, MsgLen);
     MinMPIMsgLen = dvm_min(MinMPIMsgLen, MsgLen);
  }

  ByteCount = recvcount * recvsize;

  if(IsSynchr)
     recvMsgLen = ByteCount + sizeof(double) +
                  SizeDelta[ByteCount & Msk3];
  else
     recvMsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  MPI_Sendrecv(sendbuf, MsgLen, MPI_BYTE, ProcIdentList[dest], tag,
               recvbuf, recvMsgLen, MPI_BYTE, ProcIdentList[source],
               tag, DVM_COMM_WORLD, &GMPI_Status);

  if(RTL_TRACE)
     trc_PrintBuffer((char *)recvbuf, ByteCount, call_dvmsendrecv_); 

  if(IsSynchr)
  {  recvCharPtr += ByteCount;
     SYSTEM(memcpy, (synchr_time_ptr, recvCharPtr, sizeof(double)))
     Curr_synchr_time = dvm_abs(Curr_synchr_time - Curr_dvm_time);
  }
  else
     Curr_synchr_time = 0.;
 
  if(RTL_TRACE)
     dvm_trace(ret_dvmsendrecv_," \n");

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



void  __callstd  dvmiprobe_(int  *destPtr, int  *tagPtr,
                            int  *flagPtr)
{ int   dest, tag, flag;

  DVMMTimeStart(call_dvmiprobe_);

  dest = *destPtr;
  tag  = *tagPtr;

  if(RTL_TRACE)
     dvm_trace(call_dvmiprobe_, "dest=%d; tag=%d; flagPtr=%lx;\n",
                                dest, tag, (uLLng)flagPtr);

  MPI_Iprobe(dest, tag, DVM_COMM_WORLD, &flag, &GMPI_Status);

  *flagPtr = flag;

  if(RTL_TRACE)
     dvm_trace(ret_dvmiprobe_,"flag=%d;\n", flag);

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



void  __callstd  dvmreduce_(void  *sendbufPtr, void *recvbufPtr,
                            int  *countPtr, int  *RedTypePtr,
                            int  *OpTypePtr, int  *rootPtr)
{ int             count, root;
  MPI_Datatype    datatype;
  MPI_Op          op;
  void           *sendbuf, *recvbuf;

  if(StrtRedSynchr)
     (RTL_CALL, bsynch_()); 

  DVMFTimeStart(call_dvmreduce_);

  sendbuf = sendbufPtr;
  recvbuf = recvbufPtr;

  count = *countPtr;
  root  = *rootPtr;

  if(RTL_TRACE)
     dvm_trace(call_dvmreduce_,
     "sendbuf=%lx; recvbuf=%lx; count=%d; type=%d; op=%d; root=%d;\n",
     (uLLng)sendbuf, (uLLng)recvbuf, count, *RedTypePtr, *OpTypePtr, root);

  DVMMTimeStart(call_MPI_Allreduce);

  switch(*RedTypePtr)
  { 
     case rt_CHAR:      datatype = MPI_BYTE;
                        break;
     case rt_INT:       datatype = MPI_INT;
                        break;
     case rt_LONG:      datatype = MPI_LONG;
                        break;
     case rt_LLONG:     datatype = MPI_LONG_LONG;
                        break;
     case rt_FLOAT:     datatype = MPI_FLOAT;
                        break;
     case rt_DOUBLE:    datatype = MPI_DOUBLE;
                        break;
  }

  switch(*OpTypePtr)
  {
     case rf_SUM:     op = MPI_SUM;
                      break;
     case rf_MULT:    op = MPI_PROD;
                      break;
     case rf_MAX:     op = MPI_MAX;
                      break;
     case rf_MIN:     op = MPI_MIN;
                      break;
     case rf_AND:     op = MPI_BAND;
                      break;
     case rf_OR:      op = MPI_BOR;
                      break;
     case rf_XOR:     op = MPI_BXOR;
                      break;
  }

  if(MPIInfoPrint && StatOff == 0)
     MPI_AllreduceTime -= dvm_time();

  MPI_Reduce(sendbuf, recvbuf, count, datatype, op,
             ProcIdentList[root], DVM_COMM_WORLD);

  if(MPIInfoPrint && StatOff == 0)
     MPI_AllreduceTime += dvm_time();

  DVMMTimeFinish;

  if(RTL_TRACE)
     dvm_trace(ret_dvmreduce_," \n");

  DVMFTimeFinish(ret_dvmreduce_);

  (DVM_RET);
  return;
}



int  __callstd  dvmbarrier_(void)
{
  bsynch_();
  return  0;
}



int  __callstd  dvminit_(void)
{
    DvmType  Par = 0;
  int   argc = 1;
  char *name = "tu";

  #ifdef _NT_MPI_
     MPI_Init(NULL, NULL);
  #endif

  if(MPI_ProfInitSign)
     return  0;    /* */    /*E0001*/

  rtl_init(Par, argc, &name);

  return  0;
}



int  __callstd  dvmexit_(void)
{
    DvmType  Par = 0;

  lexit_(&Par);
  return  0;
}


/* ---------------------------------------------- */    /*E0002*/


DvmType      c_getwrpar = 0;
int       t_getwrpar = 1;     
double    T_getwrpar = 0.;    
int       p_getwrpar = 1;     
double    getwrparTime = 0.;

void  __callstd  getwrpar_(int *irankPtr, int *p)
{

  if(*p == 0)
  { 
     c_getwrpar++;
     getwrparTime = T_getwrpar;

     if(_SysInfoPrint && UserTimePrint && t_getwrpar)
        T_getwrpar -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_getwrpar)
        tprintf("call getwrpar: irank=%d\n", *irankPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_getwrpar)
        T_getwrpar += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_getwrpar)
        tprintf("ret getwrpar:  time=%lf\n", T_getwrpar-getwrparTime);
  }

  return;
}


DvmType      c_filldisk = 0;
int       t_filldisk = 1;     
double    T_filldisk = 0.;    
int       p_filldisk = 1;     
double    filldiskTime = 0.;

void  __callstd  filldisk_(int *nlinesPtr, double *twPtr, int *p)
{

  if(*p == 0)
  {  
     c_filldisk++;
     filldiskTime = T_filldisk;

     if(_SysInfoPrint && UserTimePrint && t_filldisk)
        T_filldisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_filldisk)
        tprintf("call filldisk: nlines=%d tw=%lf\n", *nlinesPtr, *twPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_filldisk)
        T_filldisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_filldisk)
        tprintf("ret filldisk:  time=%lf\n", T_filldisk-filldiskTime);
  }

  return;
}


DvmType      c_wrline = 0;
int       t_wrline = 1;     
double    T_wrline = 0.;    
int       p_wrline = 1;     
double    wrlineTime = 0.;

void  __callstd  wrline_(int *ibrecPtr, int *p)
{

  if(*p == 0)
  {  
     c_wrline++;
     wrlineTime = T_wrline;

     if(_SysInfoPrint && UserTimePrint && t_wrline)
        T_wrline -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_wrline)
        tprintf("call wrline: ibrec=%d\n", *ibrecPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_wrline)
        T_wrline += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_wrline)
        tprintf("ret wrline:  time=%lf\n", T_wrline-wrlineTime);
  }

  return;
}


DvmType      c_tsttst = 0;
int       t_tsttst = 1;     
double    T_tsttst = 0.;    
int       p_tsttst = 1;     
double    tsttstTime = 0.;

void  __callstd  tsttst_(int *nlinesPtr, int *ncalcPtr, int *nrepPtr,
                         double *tdPtr, double *tcPtr, int *p)
{

  if(*p == 0)
  {  
     c_tsttst++;
     tsttstTime = T_tsttst;

     if(_SysInfoPrint && UserTimePrint && t_tsttst)
        T_tsttst -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_tsttst)
        tprintf("call tsttst: nlines=%d ncalc=%d "
                 "nrep=%d td=%lf tc=%lf\n",
                 *nlinesPtr, *ncalcPtr, *nrepPtr, *tdPtr, *tcPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_tsttst)
        T_tsttst += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_tsttst)
        tprintf("ret tsttst:  time=%lf\n", T_tsttst-tsttstTime);
  }

  return;
}


DvmType      c_initwaitdisk = 0;
int       t_initwaitdisk = 1;     
double    T_initwaitdisk = 0.;    
int       p_initwaitdisk = 1;     
double    initwaitdiskTime = 0.;

void  __callstd  initwaitdisk_(int *p)
{

  if(*p == 0)
  {  
     c_initwaitdisk++;
     initwaitdiskTime = T_initwaitdisk;

     if(_SysInfoPrint && UserTimePrint && t_initwaitdisk)
        T_initwaitdisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_initwaitdisk)
        tprintf("call initwaitdisk:\n");

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_initwaitdisk)
        T_initwaitdisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_initwaitdisk)
        tprintf("ret initwaitdisk:  time=%lf\n",
                T_initwaitdisk-initwaitdiskTime);
  }

  return;
}


DvmType      c_waitdisk = 0;
int       t_waitdisk = 1;     
double    T_waitdisk = 0.;    
int       p_waitdisk = 1;     
double    waitdiskTime = 0.;

void  __callstd  waitdisk_(int *p)
{

  if(*p == 0)
  {  
     c_waitdisk++;
     waitdiskTime = T_waitdisk;

     if(_SysInfoPrint && UserTimePrint && t_waitdisk)
        T_waitdisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_waitdisk)
        tprintf("call waitdisk:\n");

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_waitdisk)
        T_waitdisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_waitdisk)
        tprintf("ret waitdisk:  time=%lf\n", T_waitdisk-waitdiskTime);
  }

  return;
}


DvmType      c_freedisk = 0;
int       t_freedisk = 1;     
double    T_freedisk = 0.;    
int       p_freedisk = 1;     
double    freediskTime = 0.;

void  __callstd  freedisk_(int *p)
{

  if(*p == 0)
  {  
     c_freedisk++;
     freediskTime = T_freedisk;

     if(_SysInfoPrint && UserTimePrint && t_freedisk)
        T_freedisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_freedisk)
        tprintf("call freedisk:\n");

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_freedisk)
        T_freedisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_freedisk)
        tprintf("ret freedisk:  time=%lf\n", T_freedisk-freediskTime);
  }

  return;
}


DvmType      c_finaldisk = 0;
int       t_finaldisk = 1;     
double    T_finaldisk = 0.;    
int       p_finaldisk = 1;     
double    finaldiskTime = 0.;

void  __callstd  finaldisk_(int *p)
{

  if(*p == 0)
  {  
     c_finaldisk++;
     finaldiskTime = T_finaldisk;

     if(_SysInfoPrint && UserTimePrint && t_finaldisk)
        T_finaldisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_finaldisk)
        tprintf("call finaldisk:\n");

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_finaldisk)
        T_finaldisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_finaldisk)
        tprintf("ret finaldisk:  time=%lf\n", T_finaldisk-finaldiskTime);
  }

  return;
}


DvmType      c_skipopenclosedisk = 0;
int       t_skipopenclosedisk = 1;     
double    T_skipopenclosedisk = 0.;    
int       p_skipopenclosedisk = 1;     
double    skipopenclosediskTime = 0.;

void  __callstd  skipopenclosedisk_(int *num_skip_send_by_destPtr,
                                    int *p)
{

  if(*p == 0)
  {  
     c_skipopenclosedisk++;
     skipopenclosediskTime = T_skipopenclosedisk;

     if(_SysInfoPrint && UserTimePrint && t_skipopenclosedisk)
        T_skipopenclosedisk -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_skipopenclosedisk)
        tprintf("call skipopenclosedisk: num_skip_send_by_dest=%d\n",
                *num_skip_send_by_destPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_skipopenclosedisk)
        T_skipopenclosedisk += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_skipopenclosedisk)
        tprintf("ret skipopenclosedisk:  time=%lf\n",
                T_skipopenclosedisk-skipopenclosediskTime);
  }

  return;
}


DvmType      c_outoutinfotime = 0;
int       t_outoutinfotime = 1;     
double    T_outoutinfotime = 0.;    
int       p_outoutinfotime = 1;     
double    outoutinfotimeTime = 0.;

void  __callstd  outoutinfotime_(float *timPtr, int *p)
{

  if(*p == 0)
  {  
     c_outoutinfotime++;
     outoutinfotimeTime = T_outoutinfotime;

     if(_SysInfoPrint && UserTimePrint && t_outoutinfotime)
        T_outoutinfotime -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_outoutinfotime)
        tprintf("call outoutinfotime: tim=%f\n", *timPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_outoutinfotime)
        T_outoutinfotime += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_outoutinfotime)
        tprintf("ret outoutinfotime:  time=%lf\n",
                T_outoutinfotime-outoutinfotimeTime);
  }

  return;
}


DvmType      c_rdline = 0;
int       t_rdline = 1;     
double    T_rdline = 0.;    
int       p_rdline = 1;     
double    rdlineTime = 0.;

void  __callstd  rdline_(int *ibrecPtr, int *p)
{

  if(*p == 0)
  {  
     c_rdline++;
     rdlineTime = T_rdline;

     if(_SysInfoPrint && UserTimePrint && t_rdline)
        T_rdline -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_rdline)
        tprintf("call rdline: ibrec=%d\n", *ibrecPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_rdline)
        T_rdline += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_rdline)
        tprintf("ret rdline:  time=%lf\n", T_rdline-rdlineTime);
  }

  return;
}



DvmType      c_waitline = 0;
int       t_waitline = 1;     
double    T_waitline = 0.;    
int       p_waitline = 1;     
double    waitlineTime = 0.;

void  __callstd  waitline_(int *p)
{

  if(*p == 0)
  {  
     c_waitline++;
     waitlineTime = T_waitline;

     if(_SysInfoPrint && UserTimePrint && t_waitline)
        T_waitline -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_waitline)
        tprintf("call waitline:\n");

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_waitline)
        T_waitline += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_waitline)
        tprintf("ret waitline:  time=%lf\n", T_waitline-waitlineTime);
  }

  return;
}



DvmType      c_calccalc = 0;
int       t_calccalc = 1;     
double    T_calccalc = 0.;    
int       p_calccalc = 1;     
double    calccalcTime = 0.;

void  __callstd  calccalc_(int *mPtr, int *ncalcPtr, int *p)
{

  if(*p == 0)
  {  
     c_calccalc++;
     calccalcTime = T_calccalc;

     if(_SysInfoPrint && UserTimePrint && t_calccalc)
        T_calccalc -= dvm_time();

     if(RTL_TRACE && UserCallTrace && p_calccalc)
        tprintf("call calccalc: m=%d ncalc=%d\n", *mPtr, *ncalcPtr);

     (RTL_CALL);        
  }
  else
  {  
     if(_SysInfoPrint && UserTimePrint && t_calccalc)
        T_calccalc += dvm_time();

     (DVM_RET);

     if(RTL_TRACE && UserCallTrace && p_calccalc)
        tprintf("ret calccalc:  time=%lf\n", T_calccalc-calccalcTime);
  }

  return;
}


/* ---------------------------------------- */    /*E0003*/


void  __callstd  dvmprinttime_(void)
{  int  i = 0;

   if(_SysInfoPrint && UserTimePrint &&
      (UserTimePrint > 1 || MPS_CurrentProc == MPS_IOProc))
      i = 1;

   if(i)
   {
      rtl_iprintf(" \n");

      if(t_getwrpar)
         rtl_iprintf("c_getwrpar          = %-10ld   "
                     "T_getwrpar          = %-lf\n",
                     c_getwrpar, T_getwrpar);
      else
         rtl_iprintf("c_getwrpar          = %-10ld\n", c_getwrpar);

      if(t_filldisk)
         rtl_iprintf("c_filldisk          = %-10ld   "
                     "T_filldisk          = %-lf\n",
                     c_filldisk, T_filldisk);
      else
         rtl_iprintf("c_filldisk          = %-10ld\n", c_filldisk);

      if(t_wrline)
         rtl_iprintf("c_wrline            = %-10ld   "
                     "T_wrline            = %-lf\n",
                     c_wrline, T_wrline);
      else
         rtl_iprintf("c_wrline            = %-10ld\n", c_wrline);

      if(t_tsttst)
         rtl_iprintf("c_tsttst            = %-10ld   "
                     "T_tsttst            = %-lf\n",
                     c_tsttst, T_tsttst);
      else
         rtl_iprintf("c_tsttst            = %-10ld\n", c_tsttst);

      if(t_initwaitdisk)
         rtl_iprintf("c_initwaitdisk      = %-10ld   "
                     "T_initwaitdisk      = %-lf\n",
                     c_initwaitdisk, T_initwaitdisk);
      else
         rtl_iprintf("c_initwaitdisk      = %-10ld\n", c_initwaitdisk);

      if(t_waitdisk)
         rtl_iprintf("c_waitdisk          = %-10ld   "
                     "T_waitdisk          = %-lf\n",
                     c_waitdisk, T_waitdisk);
      else
         rtl_iprintf("c_waitdisk          = %-10ld\n", c_waitdisk);

      if(t_skipopenclosedisk)
         rtl_iprintf("c_skipopenclosedisk = %-10ld   "
                     "T_skipopenclosedisk = %-lf\n",
                     c_skipopenclosedisk, T_skipopenclosedisk);
      else
         rtl_iprintf("c_skipopenclosedisk = %-10ld\n", c_skipopenclosedisk);

      if(t_freedisk)
         rtl_iprintf("c_freedisk          = %-10ld   "
                     "T_freedisk          = %-lf\n",
                     c_freedisk, T_freedisk);
      else
         rtl_iprintf("c_freedisk          = %-10ld\n", c_freedisk);

      if(t_finaldisk)
         rtl_iprintf("c_finaldisk         = %-10ld   "
                     "T_finaldisk         = %-lf\n",
                     c_finaldisk, T_finaldisk);
      else
         rtl_iprintf("c_finaldisk         = %-10ld\n", c_finaldisk);

      if(t_outoutinfotime)
         rtl_iprintf("c_outoutinfotime    = %-10ld   "
                     "T_outoutinfotime    = %-lf\n",
                     c_outoutinfotime, T_outoutinfotime);
      else
         rtl_iprintf("c_outoutinfotime    = %-10ld\n", c_outoutinfotime);

      if(t_rdline)
         rtl_iprintf("c_rdline            = %-10ld   "
                     "T_rdline            = %-lf\n",
                     c_rdline, T_rdline);
      else
         rtl_iprintf("c_rdline            = %-10ld\n", c_rdline);

      if(t_waitline)
         rtl_iprintf("c_waitline          = %-10ld   "
                     "T_waitline          = %-lf\n",
                     c_waitline, T_waitline);
      else
         rtl_iprintf("c_waitline          = %-10ld\n", c_waitline);

      if(t_calccalc)
         rtl_iprintf("c_calccalc          = %-10ld   "
                     "T_calccalc          = %-lf\n",
                     c_calccalc, T_calccalc);
      else
         rtl_iprintf("c_calccalc          = %-10ld\n", c_calccalc);

      rtl_iprintf(" \n");
   }

   c_getwrpar = 0;     
   T_getwrpar = 0.;    
   getwrparTime = 0.;

   c_filldisk = 0;     
   T_filldisk = 0.;    
   filldiskTime = 0.;

   c_wrline = 0;     
   T_wrline = 0.;    
   wrlineTime = 0.;

   c_tsttst = 0;     
   T_tsttst = 0.;    
   tsttstTime = 0.;

   c_initwaitdisk = 0;     
   T_initwaitdisk = 0.;    
   initwaitdiskTime = 0.;

   c_waitdisk = 0;     
   T_waitdisk = 0.;    
   waitdiskTime = 0.;

   c_freedisk = 0;     
   T_freedisk = 0.;    
   freediskTime = 0.;

   c_finaldisk = 0;     
   T_finaldisk = 0.;    
   finaldiskTime = 0.;

   c_skipopenclosedisk = 0;     
   T_skipopenclosedisk = 0.;    
   skipopenclosediskTime = 0.;

   c_outoutinfotime = 0;     
   T_outoutinfotime = 0.;    
   outoutinfotimeTime = 0.;

   c_rdline = 0;     
   T_rdline = 0.;    
   rdlineTime = 0.;

   c_waitline = 0;     
   T_waitline = 0.;    
   waitlineTime = 0.;

   c_calccalc = 0;     
   T_calccalc = 0.;    
   calccalcTime = 0.;

   return;
}

#endif

#endif


#endif   /*  _V_TURCH_C_  */    /*E0004*/
