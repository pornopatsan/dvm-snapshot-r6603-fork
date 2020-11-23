#ifndef _LOWIO_C_
#define _LOWIO_C_
/***************/    /*E0000*/


#ifdef _STRUCT_STAT_

int dvm_stat(const char *filename, struct stat *buffer)
{ int           ResArr[2+sizeof(double)/sizeof(int)]={-1};
  struct stat  *_buffer;

  DVMFTimeStart(call_dvm_stat);

  if(RTL_TRACE)
     dvm_trace(call_dvm_stat, "FileName=%s;\n", filename);

  if(IsDVMInit && DVMInputPar)
  {  if(DVM_IOProc == MPS_CurrentProc)
        SYSTEM_RET(ResArr[0], stat, (filename, buffer))

     ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );

     if((IsSynchr && UserSumFlag) || (sizeof(struct stat) & Msk3))                
     {  mac_malloc(_buffer, struct stat *, sizeof(struct stat), 0);
        if(MPS_CurrentProc == DVM_IOProc)
           SYSTEM(memcpy, (_buffer, buffer, sizeof(struct stat)) )

        ( RTL_CALL, rtl_BroadCast(_buffer, 1, sizeof(struct stat),
                                  DVM_IOProc, NULL) );

        if(MPS_CurrentProc != DVM_IOProc)
           SYSTEM(memcpy, (buffer, _buffer, sizeof(struct stat)) )
        mac_free(&_buffer);
     }
     else
        ( RTL_CALL, rtl_BroadCast(buffer, 1, sizeof(struct stat),
                                  DVM_IOProc, NULL) );
  }
  else
  {
  #ifdef _DVM_MPI_
    if(IsInit && DVMInputPar) /* if MPS has been initialized */    /*E0001*/
  #else
    if(IsSlaveRun && DVMInputPar) /* if slaves run */    /*E0002*/
  #endif
    { if(CurrentProcIdent == MasterProcIdent)
         SYSTEM_RET(ResArr[0],stat, (filename,buffer))

      ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
      ( RTL_CALL, mps_Bcast(buffer, 1, sizeof(struct stat)) );
    }
    else
       SYSTEM_RET(ResArr[0], stat, (filename, buffer))
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_stat,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_stat);
  return  (DVM_RET, ResArr[0]);
}

#endif    /*  _STRUCT_STAT_  */    /*E0003*/


/****************************\
* I/O functions of low level * 
\****************************/    /*E0004*/

#ifdef _DVM_LLIO_

int  dvm_close(DVMHANDLE *HandlePtr)
{ int  ResArr[2+sizeof(double)/sizeof(int)]={-1};

  DVMFTimeStart(call_dvm_close);

  if(RTL_TRACE)
     dvm_trace(call_dvm_close,"HandlePtr=%lx;\n",(uLLng)HandlePtr);

  if((DVM_IOProc == MPS_CurrentProc) AND (HandlePtr != NULL))
     SYSTEM_RET(ResArr[0],close, (HandlePtr->Handle))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(ResArr[0] == 0)
  {  dvm_FreeStruct(HandlePtr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_close,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_close);
  return  (DVM_RET, ResArr[0]);
}



#ifdef _STRUCT_STAT_

int  dvm_fstat(DVMHANDLE *HandlePtr, struct stat *buffer)
{ int            ResArr[2+sizeof(double)/sizeof(int)]={-1};
  struct stat   *_buffer;

  DVMFTimeStart(call_dvm_fstat);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fstat,"HandlePtr=%lx;\n",(uLLng)HandlePtr);

  if((DVM_IOProc == MPS_CurrentProc) AND (HandlePtr != NULL))
     SYSTEM_RET(ResArr[0],fstat, (HandlePtr->Handle, buffer))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if((IsSynchr && UserSumFlag) || (sizeof(struct stat) & Msk3))
  {  mac_malloc(_buffer, struct stat *, sizeof(struct stat), 0);
     if(MPS_CurrentProc == DVM_IOProc)
        SYSTEM(memcpy, (_buffer, buffer, sizeof(struct stat)) )

     ( RTL_CALL, rtl_BroadCast(_buffer, 1, sizeof(struct stat),
                               DVM_IOProc, NULL) );

     if(MPS_CurrentProc != DVM_IOProc)
        SYSTEM(memcpy, (buffer, _buffer, sizeof(struct stat)) )
     mac_free(&_buffer);
  }
  else
     ( RTL_CALL, rtl_BroadCast(buffer, 1, sizeof(struct stat),
                               DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fstat,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_fstat);
  return  (DVM_RET, ResArr[0]);
}

#endif    /*  _STRUCT_STAT_  */    /*E0005*/



DvmType  dvm_lseek(DVMHANDLE *HandlePtr, DvmType offset, int origin)
{
    DvmType  ResArr[2 + sizeof(double) / sizeof(DvmType)] = { -1L };

  DVMFTimeStart(call_dvm_lseek);

  if(RTL_TRACE)
     dvm_trace(call_dvm_lseek,"HandlePtr=%lx; Offset=%ld; Origin=%d;\n",
                               (uLLng)HandlePtr,offset,origin);

  if((DVM_IOProc == MPS_CurrentProc) AND (HandlePtr != NULL))
     SYSTEM_RET(ResArr[0],lseek,(HandlePtr->Handle,offset,origin))

     (RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(DvmType), DVM_IOProc,
                            NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_lseek,"Res=%ld;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_lseek);
  return  (DVM_RET, ResArr[0]);
}



DVMHANDLE  *dvm_open(const char *filename, int oflag, int pmode)
{ DVMHANDLE  *Res = NULL;
  int         RCArr[2+sizeof(double)/sizeof(int)];

  DVMFTimeStart(call_dvm_open);

  if(RTL_TRACE)
     dvm_trace(call_dvm_open,"Filename=%s; Oflag=%d; Pmode=%d;\n",
                              filename,oflag,pmode);

  dvm_AllocStruct(DVMHANDLE, Res);
  Res->FileID = FilesCount;
  FilesCount++;

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(RCArr[0],open, (filename, oflag, pmode))

  ( RTL_CALL, rtl_BroadCast(RCArr, 1, sizeof(int), DVM_IOProc, NULL) );

  Res->Handle = RCArr[0];

  if(Res->Handle == -1)
  {  dvm_FreeStruct(Res);
     Res = NULL;
     FilesCount--;
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_open,"Res=%lx;\n", (uLLng)Res);

  DVMFTimeFinish(ret_dvm_open);
  return  (DVM_RET, Res);
}



int  dvm_read(DVMHANDLE *HandlePtr, char *buffer, unsigned int count)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {0};
  char  *_buffer;

  DVMFTimeStart(call_dvm_read);

  if(RTL_TRACE)
     dvm_trace(call_dvm_read,"HandlePtr=%lx; Buffer=%lx; Count=%d;\n",
                              (uLLng)HandlePtr,(uLLng)buffer,count);

  if((DVM_IOProc == MPS_CurrentProc) AND (HandlePtr != NULL))
     SYSTEM_RET(ResArr[0],read, (HandlePtr->Handle, buffer, count))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(ResArr[0] > 0)
  {  if((IsSynchr && UserSumFlag) || (ResArr[0] & Msk3))
     {  mac_malloc(_buffer, char *, ResArr[0], 0);
        if(MPS_CurrentProc == DVM_IOProc)
           SYSTEM(memcpy, (_buffer, buffer, ResArr[0]) )

        ( RTL_CALL, rtl_BroadCast(_buffer, 1, ResArr[0], DVM_IOProc,
                                  NULL) );

        if(MPS_CurrentProc != DVM_IOProc)
           SYSTEM(memcpy, (buffer, _buffer, ResArr[0]) )
        mac_free(&_buffer);
     }
     else
        ( RTL_CALL, rtl_BroadCast(buffer, 1, ResArr[0], DVM_IOProc,
                                  NULL) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_read,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_read);
  return  (DVM_RET, ResArr[0]);
}



int dvm_write(DVMHANDLE  *HandlePtr, const void *buffer,
              unsigned int count)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {-1};

  DVMFTimeStart(call_dvm_write);

  if(RTL_TRACE)
     dvm_trace(call_dvm_write,"HandlePtr=%lx; Buffer=%lx; Count=%d;\n",
                               (uLLng)HandlePtr,(uLLng)buffer,count);

  if((DVM_IOProc == MPS_CurrentProc) AND (HandlePtr != NULL))
     SYSTEM_RET(ResArr[0], write, (HandlePtr->Handle,buffer,count))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_write,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_write);
  return  (DVM_RET, ResArr[0]);
}

#endif  /* _DVM_LLIO_ */    /*e0006*/


/********************************************\
* Operations with not ANSI folders and files * 
\********************************************/    /*E0007*/

#ifdef _ACCESS_FUN_

int dvm_access(const char *filename,int mode)
{ int  ResArr[2+sizeof(double)/sizeof(int)];

  DVMFTimeStart(call_dvm_access);

  if(RTL_TRACE)
     dvm_trace(call_dvm_access,"FileName=%s; Mode=%d;\n",filename,mode);

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(ResArr[0], access, (filename, mode))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_access,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_access);
  return  (DVM_RET, ResArr[0]);
}

#endif  /* _ACCESS_FUN_ */    /*E0008*/


#endif  /* _LOWIO_C_ */    /*E0009*/
