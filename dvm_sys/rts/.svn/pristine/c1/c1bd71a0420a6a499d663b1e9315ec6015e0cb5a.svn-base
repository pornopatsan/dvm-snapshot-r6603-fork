#ifndef _STDIO_C_
#define _STDIO_C_
/***************/    /*E0000*/

/***********************************************************************\
*  I/O functions of ANSI standard or realized on base of ANSI standard  * 
\***********************************************************************/    /*E0001*/

/*    ANSI    */    /*E0002*/

void  dvm_clearerr(DVMFILE  *stream)
{ byte         IsClear = 0;

  #ifdef _RTS_ZLIB_

  gzFile       file;
  gz_stream   *s;

  #endif

  DVMFTimeStart(call_dvm_clearerr);

  if(RTL_TRACE)
     dvm_trace(call_dvm_clearerr, "Stream=%lx;\n", (uLLng)stream); 

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsClear = 1; /* */    /*E0003*/

     #ifdef _RTS_ZLIB_

     file     = stream->zlibFile;
     s        = (gz_stream *)file;

     s->z_err = Z_OK;
     s->z_eof = 0;

     SYSTEM(clearerr, (s->file))

     #endif
  }

  #endif

  if(IsClear == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM(clearerr,(stream->File))
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_clearerr," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_clearerr);
  return;
}



char  OpenCloseBuf[MaxApplMesSize]; /* */    /*E0004*/


#define CLOSEGZFILE\
/* */    /*E0005*/\
i = sizeof(gzFile) + sizeof(int);\
i = (i/sizeof(int)+1)*sizeof(int); /* */    /*E0006*/\
CharPtr = OpenCloseBuf;\
IntPtr     = (int *)CharPtr;\
*IntPtr    = 3;\
CharPtr   += sizeof(int);\
gzFilePtr  = (gzFile *)CharPtr;\
*gzFilePtr = stream->zlibFile;\
/* */    /*E0007*/\
iom_Sendnowait1(MyIOProc, IntPtr, i, &Req, msg_IOInit);\
iom_Waitrequest1(&Req);\
/* */    /*E0008*/\
iom_Recvnowait1(MyIOProc, ResArr, sizeof(int), &Req, msg_IOProcess);\
iom_Waitrequest1(&Req);\



#define CLOSEFILE\
/* */    /*E0009*/\
i = sizeof(FILE *) + sizeof(int);\
i = (i/sizeof(int)+1)*sizeof(int); /* */    /*E0010*/\
CharPtr = OpenCloseBuf;\
IntPtr       = (int *)CharPtr;\
*IntPtr      = 4;\
CharPtr     += sizeof(int);\
FilePtrPtr   = (FILE **)CharPtr;\
*FilePtrPtr  = stream->File;\
/* */    /*E0011*/\
iom_Sendnowait1(MyIOProc, IntPtr, i, &Req, msg_IOInit);\
iom_Waitrequest1(&Req);\
/* */    /*E0012*/\
iom_Recvnowait1(MyIOProc, ResArr, sizeof(int), &Req, msg_IOProcess);\
iom_Waitrequest1(&Req);\



int   dvm_fclose(DVMFILE  *stream)
{ int           ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte          IsClose = 0;
  FILE        **FilePtrPtr;
  int           i;
  int          *IntPtr;
  char         *CharPtr;
  MPS_Request   Req;

#ifdef _DVM_ZLIB_
  gzFile       *gzFilePtr;
#endif

  DVMFTimeStart(call_dvm_fclose);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fclose,"Stream=%lx;\n", (uLLng)stream);

#ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsClose = 1;  /* */    /*E0013*/

     /* */    /*E0014*/

     if(DVM_IOProc == MPS_CurrentProc)
     {  if(stream->ScanFile)
        {  SYSTEM(fclose, (stream->ScanFile))
           SYSTEM(sprintf, (DVM_String, "%ld", stream->ScanFileID))
           SYSTEM(strcat, (DVM_String, "scan.dvm"))
           SYSTEM(remove, (DVM_String))
           SYSTEM_RET(stream->ScanFile, fopen, (DVM_String, "w"))
           SYSTEM(fclose, (stream->ScanFile))
        }
     }

     if(IsDVMInit && (DVMInputPar || stream->W))
     {
        /* */    /*E0015*/

        if(stream->LocIOType && stream->ParIOType &&
           stream->zlibFile != NULL)
        {  /* */    /*E0016*/

           CLOSEGZFILE
        }
        else
        {  /* */    /*E0017*/

           if(stream->LocIOType == 0)
           {  /* */    /*E0018*/

              if(DVM_IOProc == MPS_CurrentProc &&
                 stream->zlibFile != NULL)
              {
                 if(stream->ParIOType)
                 {  /* */    /*E0019*/

                    CLOSEGZFILE
                 }
                 else
                 {  /* */    /*E0020*/

                    SYSTEM_RET(ResArr[0], gzclose, (stream->zlibFile))
                 }
              }

              ResArr[0] = 0;  /* */    /*E0021*/

              ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int),
                                        DVM_IOProc, NULL) );
           }
           else
           {  /* */    /*E0022*/

              if(stream->zlibFile != NULL)
                 SYSTEM_RET(ResArr[0], gzclose, (stream->zlibFile))

              ResArr[0] = 0;  /* */    /*E0023*/
           }
        }
     }
     else
     {
     #ifdef  _DVM_MPI_   
        if(IsInit && (DVMInputPar || stream->W)) /* */    /*E0024*/
     #else
        if(IsSlaveRun && (DVMInputPar || stream->W)) /* */    /*E0025*/
     #endif
        {
           /* */    /*E0026*/

           if(stream->LocIOType && stream->ParIOType &&
              stream->zlibFile != NULL)
           {  /* */    /*E0027*/

              CLOSEGZFILE
           }
           else
           {
              if(stream->LocIOType == 0)
              {  /* */    /*E0028*/

                 /* */    /*E0029*/

                 if(CurrentProcIdent == MasterProcIdent &&
                    stream->zlibFile != NULL)
                 {
                    if(stream->ParIOType)
                    {  /* */    /*E0030*/

                       CLOSEGZFILE
                    }
                    else
                    {  /* */    /*E0031*/

                       SYSTEM_RET(ResArr[0], gzclose, (stream->zlibFile))
                    }
                 }

                 ResArr[0] = 0;  /* */    /*E0032*/

                 ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
              }
              else
              {  /* */    /*E0033*/

                 if(stream->zlibFile != NULL)
                    SYSTEM_RET(ResArr[0], gzclose, (stream->zlibFile))

                 ResArr[0] = 0;  /* */    /*E0034*/
              }
           } 
        }
        else
        {
           /* */    /*E0035*/

           if(stream->LocIOType && stream->ParIOType &&
              stream->zlibFile != NULL)
           {  /* */    /*E0036*/

              CLOSEGZFILE
           }
           else
           {  /* */    /*E0037*/

              if(stream->zlibFile != NULL)
              { 
                 if(stream->ParIOType)
                 {  /* */    /*E0038*/

                    CLOSEGZFILE
                 }
                 else
                 {  /* */    /*E0039*/

                    SYSTEM_RET(ResArr[0], gzclose, (stream->zlibFile))
                 }
              }

              ResArr[0] = 0;  /* */    /*E0040*/
           }
        }
     }
  }

#endif

  if(IsClose == 0)
  {  if(IsDVMInit && (DVMInputPar || stream->W))
     {
        /* */    /*E0041*/

        if(stream != NULL && stream->LocIOType && stream->ParIOType &&
           stream->File != NULL)
        {  /* */    /*E0042*/

           CLOSEFILE
        }
        else
        {
           /* */    /*E0043*/

           if(stream->LocIOType == 0)
           {  /* */    /*E0044*/

              if(DVM_IOProc == MPS_CurrentProc && stream != NULL &&
                 stream->File != NULL)
              {
                 if(stream->ParIOType)
                 {  /* */    /*E0045*/

                    CLOSEFILE
                 }
                 else
                 {  /* */    /*E0046*/

                    SYSTEM_RET(ResArr[0], fclose, (stream->File))
                 }
              }

              ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int),
                                        DVM_IOProc, NULL) );
           }
           else
           {  /* */    /*E0047*/

              if(stream != NULL && stream->File != NULL)
                 SYSTEM_RET(ResArr[0], fclose, (stream->File)) 
           }
        }
     }
     else
     {
     #ifdef  _DVM_MPI_   
        if(IsInit && (DVMInputPar || stream->W)) /* if MPS has been initialized */    /*E0048*/
     #else
        if(IsSlaveRun && (DVMInputPar || stream->W)) /* if offspring tasks are running */    /*E0049*/
     #endif
        {
           /* */    /*E0050*/

           if(stream != NULL && stream->LocIOType && stream->ParIOType &&
              stream->File != NULL)
           {  /* */    /*E0051*/

              CLOSEFILE
           }
           else
           {  /* */    /*E0052*/

              if(stream->LocIOType == 0)
              {  /* */    /*E0053*/

                 if(CurrentProcIdent == MasterProcIdent &&
                    stream != NULL && stream->File != NULL)
                 {
                    if(stream->ParIOType)
                    {  /* */    /*E0054*/

                       CLOSEFILE
                    }
                    else
                    {  /* */    /*E0055*/

                       SYSTEM_RET(ResArr[0], fclose, (stream->File))
                    }
                 }

                 ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
              }
              else
              {  /* */    /*E0056*/

                 if(stream != NULL && stream->File != NULL)
                    SYSTEM_RET(ResArr[0], fclose, (stream->File))
              }
           }
        }
        else
        {
           /* */    /*E0057*/

           if(stream != NULL && stream->LocIOType && stream->ParIOType &&
              stream->File != NULL)
           {  /* */    /*E0058*/

              CLOSEFILE
           }
           else
           {  /* */    /*E0059*/

              if(stream != NULL && stream->File != NULL)
              {
                 if(stream->ParIOType)
                 {  /* */    /*E0060*/

                    CLOSEFILE
                 }
                 else
                 {  /* */    /*E0061*/

                    SYSTEM_RET(ResArr[0], fclose, (stream->File)) 
                 }
              }
           }
        }
     }
  }

  if(ResArr[0] == 0)
  {  dvm_FreeStruct(stream);
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fclose,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_fclose);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_feof(DVMFILE *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsEof = 0;

  DVMFTimeStart(call_dvm_feof);

  if(RTL_TRACE)
     dvm_trace(call_dvm_feof, "Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsEof = 1;  /* */    /*E0062*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzeof, (stream->zlibFile))
  }

  #endif

  if(IsEof == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], feof, (stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_feof,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_feof);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_ferror(DVMFILE *stream)
{ int            ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte           IsError = 0;
  const char    *ErrPtr = NULL;
  int            Err = 0;

  DVMFTimeStart(call_dvm_ferror);

  if(RTL_TRACE)
     dvm_trace(call_dvm_ferror, "Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsError = 1;  /* */    /*E0063*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ErrPtr, gzerror, (stream->zlibFile, &Err))

     if(ErrPtr == NULL || ErrPtr[0] == '\x00')
        ResArr[0] = 0;  /* */    /*E0064*/
     else
     {  if(Err == Z_ERRNO)
           ResArr[0] = 1;  /* */    /*E0065*/
        else
           ResArr[0] = 2;  /* */    /*E0066*/
     }
  }

  #endif

  if(IsError == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], ferror, (stream->File))

     if(ResArr[0] != 0)
        ResArr[0] = 1; 
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_ferror,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_ferror);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_fflush(DVMFILE *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsFlush = 0;

  #ifdef _UNIX_
     int    handle;
  #endif

  DVMFTimeStart(call_dvm_fflush);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fflush, "Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsFlush = 1;  /* */    /*E0067*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
     {  SYSTEM_RET(ResArr[0], gzflush, (stream->zlibFile, stream->flush))

        #ifdef _UNIX_
           SYSTEM(sync, ())
        #endif
     }

     ResArr[0] = 0;   /* */    /*E0068*/
  }

  #endif

  if(IsFlush == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
     {  SYSTEM_RET(ResArr[0], fflush, (stream->File))

        #ifdef _UNIX_
           SYSTEM_RET(handle, fileno, (stream->File))
           SYSTEM(fsync, (handle))
        #endif
     }
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fflush,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_fflush);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_fgetc(DVMFILE *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsGetc = 0;

  DVMFTimeStart(call_dvm_fgetc);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fgetc,"Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsGetc = 1; /* */    /*E0069*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzgetc, (stream->zlibFile))
  }
   
  #endif

  if(IsGetc == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], fgetc, (stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fgetc, "Sym=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_fgetc);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_fgetpos(DVMFILE  *stream, fpos_t  *pos)
{ int        ResArr[2+sizeof(double)/sizeof(int)] = {EBADF};
  fpos_t    *_pos;

  DVMFTimeStart(call_dvm_fgetpos);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fgetpos,"Stream=%lx;\n", (uLLng)stream);

  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
     SYSTEM_RET(ResArr[0], fgetpos, (stream->File, pos))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if((IsSynchr && UserSumFlag) || (sizeof(fpos_t) & Msk3))
  {  mac_malloc(_pos, fpos_t *, sizeof(fpos_t), 0);
     if(MPS_CurrentProc == DVM_IOProc)
        SYSTEM(memcpy, (_pos, pos, sizeof(fpos_t)) )

     ( RTL_CALL, rtl_BroadCast(_pos, 1, sizeof(fpos_t), DVM_IOProc,
                               NULL) );

     if(MPS_CurrentProc != DVM_IOProc)
        SYSTEM(memcpy, (pos, _pos, sizeof(fpos_t)) )
     mac_free(&_pos);
  }
  else
     ( RTL_CALL, rtl_BroadCast(pos, 1, sizeof(fpos_t), DVM_IOProc,
                               NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fgetpos,"Res=%d; Pos=%ld;\n", ResArr[0], *pos);

  DVMFTimeFinish(ret_dvm_fgetpos);
  return  (DVM_RET, ResArr[0]);
}



char  *dvm_fgets(char *string, int n, DVMFILE *stream)
{ char  *ResArr[2+sizeof(double)/sizeof(char *)] ={NULL};
  int    StrLenArr[2+sizeof(double)/sizeof(int)] = {0};
  char  *_string;
  byte   IsGets = 0;

  DVMFTimeStart(call_dvm_fgets);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fgets,"Stream=%lx; N=%d;\n", (uLLng)stream, n);

  if(IsDVMInit && (DVMInputPar || stream->W))
  {
     #ifdef _DVM_ZLIB_

     if(stream->zip)
     {  IsGets = 1;  /* */    /*E0070*/

        if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        {  SYSTEM_RET(ResArr[0], gzgets, (stream->zlibFile, string, n))

           if(ResArr[0] != NULL)
           {  SYSTEM_RET(StrLenArr[0], strlen, (string))
              StrLenArr[0]++;
           }
        }
     }

     #endif

     if(IsGets == 0)
     {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        {  SYSTEM_RET(ResArr[0], fgets, (string, n, stream->File))

           if(ResArr[0] != NULL)
           {  SYSTEM_RET(StrLenArr[0], strlen, (string))
              StrLenArr[0]++;
           }
        }
     }
   
     ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(char *), DVM_IOProc,
                               NULL) );

     if(ResArr[0] != NULL)
     {  ( RTL_CALL, rtl_BroadCast(StrLenArr, 1, sizeof(int),
                                  DVM_IOProc, NULL) );

        if(StrLenArr[0] != 0)
        {  if((IsSynchr && UserSumFlag) || (StrLenArr[0] & Msk3))
           {  mac_malloc(_string, char *, StrLenArr[0], 0);

              if(MPS_CurrentProc == DVM_IOProc)
                 SYSTEM(memcpy, (_string, string, StrLenArr[0]) )

              ( RTL_CALL, rtl_BroadCast(_string, 1, StrLenArr[0],
                                        DVM_IOProc, NULL) );

              if(MPS_CurrentProc != DVM_IOProc)
                 SYSTEM(memcpy, (string, _string, StrLenArr[0]) )
              mac_free(&_string);
           }
           else
              ( RTL_CALL, rtl_BroadCast(string, 1, StrLenArr[0],
                                        DVM_IOProc, NULL) );
        }
     }
  }
  else
  { 
  #ifdef  _DVM_MPI_
     if(IsInit && (DVMInputPar || stream->W)) /* if MPS has been initialized */    /*E0071*/
  #else
     if(IsSlaveRun && (DVMInputPar || stream->W)) /* if offspring tasks are running */    /*E0072*/
  #endif
     {
        #ifdef _DVM_ZLIB_

        if(stream->zip)
        {  IsGets = 1;  /* */    /*E0073*/

           if(CurrentProcIdent == MasterProcIdent &&
              stream->zlibFile != NULL)
           {  SYSTEM_RET(ResArr[0], gzgets, (stream->zlibFile,
                                             string, n))

              if(ResArr[0] != NULL)
              {  SYSTEM_RET(StrLenArr[0], strlen, (string))
                 StrLenArr[0]++;
              }
           }
        }

        #endif

        if(IsGets == 0)
        {  if(CurrentProcIdent == MasterProcIdent &&
              stream->File != NULL)
           {  SYSTEM_RET(ResArr[0], fgets, (string, n, stream->File))

              if(ResArr[0] != NULL)
              {  SYSTEM_RET(StrLenArr[0], strlen, (string))
                 StrLenArr[0]++;
              }
           }
        }
      
        ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(char *)) );

        if(ResArr[0] != NULL)
        {  ( RTL_CALL, mps_Bcast(StrLenArr, 1, sizeof(int)) );

           if(StrLenArr[0] != 0)
              ( RTL_CALL, mps_Bcast(string, 1, StrLenArr[0]) );
        }
     }
     else
     {
        #ifdef _DVM_ZLIB_

        if(stream->zip)
        {  IsGets = 1;  /* */    /*E0074*/

           if(stream->zlibFile != NULL)
              SYSTEM_RET(ResArr[0], gzgets, (stream->zlibFile,
                                             string, n))
        }

        #endif

        if(IsGets == 0)
        {  if(stream->File != NULL)
              SYSTEM_RET(ResArr[0], fgets, (string, n, stream->File))
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fgets,"Res=%lx; String=%s\n",
                              (uLLng)ResArr[0], string);

  DVMFTimeFinish(ret_dvm_fgets);
  return  (DVM_RET, ResArr[0]);
}



#define OPENGZFILE\
/* */    /*E0075*/\
SYSTEM_RET(j, strlen, (DVM_String))\
SYSTEM_RET(k, strlen, (tp))\
k += j + 4*sizeof(int) + 2;\
k = (k/sizeof(int)+1)*sizeof(int); /* */    /*E0076*/\
CharPtr = OpenCloseBuf;\
IntPtr = (int *)CharPtr;\
*IntPtr = 1;\
IntPtr[1] = i;\
IntPtr[2] = Res->W;\
IntPtr[3] = Add_gz;\
CharPtr += 4*sizeof(int);\
SYSTEM(strcpy, (CharPtr, DVM_String))\
CharPtr += j + 1;\
SYSTEM(strcpy, (CharPtr, tp))\
/* */    /*E0077*/\
iom_Sendnowait1(MyIOProc, IntPtr, k, &Req, msg_IOInit);\
iom_Waitrequest1(&Req);\
/* */    /*E0078*/\
iom_Recvnowait1(MyIOProc, gzRCArr, sizeof(gzFile), &Req, msg_IOProcess);\
iom_Waitrequest1(&Req);\



#define OPENFILE\
/* */    /*E0079*/\
SYSTEM_RET(j, strlen, (filename))\
SYSTEM_RET(k, strlen, (tp))\
k += j + sizeof(int) + 2;\
k = (k/sizeof(int)+1)*sizeof(int); /* */    /*E0080*/\
CharPtr = OpenCloseBuf;\
IntPtr = (int *)CharPtr;\
*IntPtr = 2;\
CharPtr += sizeof(int);\
SYSTEM(strcpy, (CharPtr, filename))\
CharPtr += j + 1;\
SYSTEM(strcpy, (CharPtr, tp))\
/* */    /*E0081*/\
iom_Sendnowait1(MyIOProc, IntPtr, k, &Req, msg_IOInit);\
iom_Waitrequest1(&Req);\
/* */    /*E0082*/\
iom_Recvnowait1(MyIOProc, RCArr, sizeof(FILE *), &Req, msg_IOProcess);\
iom_Waitrequest1(&Req);\



DVMFILE  *dvm_fopen(const char *filename, const char *type)
{ DVMFILE     *Res = NULL;
  FILE        *RCArr[2+sizeof(double)/sizeof(FILE *)];
  byte         IsOpen = 0, IsMinus = 0;
  int          i, j, k, Add_gz = 0;
  char         tp[16];
  char        *CharPtr;
  int         *IntPtr; 
  MPS_Request  Req;

#ifdef _DVM_ZLIB_
  gzFile       gzRCArr[2+sizeof(double)/sizeof(gzFile)];
#endif

  DVMFTimeStart(call_dvm_fopen);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fopen,"Filename=%s; Type=%s;\n",
                              filename, type);

  dvm_AllocStruct(DVMFILE, Res);
  Res->FileID = FilesCount; /* */    /*E0083*/
  FilesCount++;

  Res->ScanFile = NULL;  /* */    /*E0084*/

  if(strpbrk(type, "wa+") != NULL)
     Res->W = 1;   /* */    /*E0085*/
  else
     Res->W = 0;

  Res->ParIOType = 0; /* */    /*E0086*/
  Res->LocIOType = 0; /* */    /*E0087*/

  SYSTEM(strcpy, (tp, type))

  /* */    /*E0088*/

  SYSTEM_RET(CharPtr, strchr, (tp, 's'))

  if(CharPtr != NULL)
  {  /* */    /*E0089*/

     *CharPtr     = *(CharPtr+1);
     *(CharPtr+1) = *(CharPtr+2);
     *(CharPtr+2) = *(CharPtr+3);
     *(CharPtr+3) = *(CharPtr+4);
     *(CharPtr+4) = *(CharPtr+5);
     *(CharPtr+5) = *(CharPtr+6);
     *(CharPtr+6) = *(CharPtr+7);
     *(CharPtr+7) = *(CharPtr+8);
     *(CharPtr+8) = *(CharPtr+9);

     Res->ParIOType = 1; /* */    /*E0090*/
  }

  SYSTEM_RET(CharPtr, strchr, (tp, 'S'))

  if(CharPtr != NULL)
  {  /* */    /*E0091*/

     *CharPtr     = *(CharPtr+1);
     *(CharPtr+1) = *(CharPtr+2);
     *(CharPtr+2) = *(CharPtr+3);
     *(CharPtr+3) = *(CharPtr+4);
     *(CharPtr+4) = *(CharPtr+5);
     *(CharPtr+5) = *(CharPtr+6);
     *(CharPtr+6) = *(CharPtr+7);
     *(CharPtr+7) = *(CharPtr+8);
     *(CharPtr+8) = *(CharPtr+9);

     Res->ParIOType = 1; /* */    /*E0092*/
  }

  /* */    /*E0093*/

  SYSTEM_RET(CharPtr, strchr, (tp, 'l'))

  if(CharPtr != NULL)
  {  /* */    /*E0094*/

     *CharPtr     = *(CharPtr+1);
     *(CharPtr+1) = *(CharPtr+2);
     *(CharPtr+2) = *(CharPtr+3);
     *(CharPtr+3) = *(CharPtr+4);
     *(CharPtr+4) = *(CharPtr+5);
     *(CharPtr+5) = *(CharPtr+6);
     *(CharPtr+6) = *(CharPtr+7);
     *(CharPtr+7) = *(CharPtr+8);
     *(CharPtr+8) = *(CharPtr+9);

     Res->LocIOType = 1; /* */    /*E0095*/
  }

  SYSTEM_RET(CharPtr, strchr, (tp, 'L'))

  if(CharPtr != NULL)
  {  /* */    /*E0096*/

     *CharPtr     = *(CharPtr+1);
     *(CharPtr+1) = *(CharPtr+2);
     *(CharPtr+2) = *(CharPtr+3);
     *(CharPtr+3) = *(CharPtr+4);
     *(CharPtr+4) = *(CharPtr+5);
     *(CharPtr+5) = *(CharPtr+6);
     *(CharPtr+6) = *(CharPtr+7);
     *(CharPtr+7) = *(CharPtr+8);
     *(CharPtr+8) = *(CharPtr+9);

     Res->LocIOType = 1; /* */    /*E0097*/
  }

  /* */    /*E0098*/

  SYSTEM_RET(j, strncmp, (filename, "/store", 6))

  if(j == 0)
     Res->LocIOType = 1; /* */    /*E0099*/

  /* --------------------------------------------- */    /*E0100*/

  if(IOProcess == 0)
     Res->ParIOType = 0; /* */    /*E0101*/

  /* --------------------------------------------- */    /*E0102*/

  SYSTEM_RET(CharPtr, strchr, (tp, '-'))

  if(CharPtr != NULL)
  {  /* */    /*E0103*/

     *CharPtr     = *(CharPtr+1);
     *(CharPtr+1) = *(CharPtr+2);
     *(CharPtr+2) = *(CharPtr+3);
     *(CharPtr+3) = *(CharPtr+4);
     *(CharPtr+4) = *(CharPtr+5);
     *(CharPtr+5) = *(CharPtr+6);
     *(CharPtr+6) = *(CharPtr+7);
     *(CharPtr+7) = *(CharPtr+8);
     *(CharPtr+8) = *(CharPtr+9);

     IsMinus = 1;
  }

  SYSTEM_RET(i , strlen, (tp))

#ifdef _DVM_ZLIB_

  if( i > 1 && ( isdigit(tp[i-1]) || isdigit(tp[i-2]) ) )
  {  IsOpen = 1;   /* */    /*E0104*/

     if(CompressFlush == 0)
        Res->flush = Z_SYNC_FLUSH;  /* */    /*E0105*/
     else
        Res->flush = Z_FULL_FLUSH;

     while((CharPtr = strchr(tp, 't')) != NULL) /* */    /*E0106*/
           *CharPtr = 'b';

     /* */    /*E0107*/

     if(strchr(tp, 'b') == NULL)
     {  if(isdigit(tp[i-2]))
        {  tp[i+1] = tp[i];
           tp[i]   = tp[i-1];
           tp[i-1] = tp[i-2];
           tp[i-2] = 'b';
        }
        else
        {  tp[i+1] = tp[i];
           tp[i]   = tp[i-1];
           tp[i-1] = 'b';
        }

        i++;
     }

     if((CompressLevel != 0 || IsMinus) && Res->W)
     {
        if(CompressLevel > 0 && IsMinus == 0)
        {  /* */    /*E0108*/

           if(tp[i-1] == '0')
           {  SYSTEM(sprintf, (DVM_String, "%d", CompressLevel))
              tp[i-1] = DVM_String[0];
           }

           if(tp[i-2] == '0')
           {  SYSTEM(sprintf, (DVM_String, "%d", CompressLevel))
              tp[i-2] = DVM_String[0];
           }
        }
        else
        {  /* */    /*E0109*/

           if(isdigit(tp[i-1]))
              tp[i-1] = '0';

           if(isdigit(tp[i-2]))
              tp[i-2] = '0';
        }
     }
     else
     {  /* */    /*E0110*/

        if(tp[i-1] == '0')
           tp[i-1] = '\x00';

        if(tp[i-2] == '0')
        {  tp[i-2] = tp[i-1]; 
           tp[i-1] = '\x00';
        }
     }

     SYSTEM(strcpy, (DVM_String, filename))
     SYSTEM_RET(i, strlen, (DVM_String))

     if(i < 4)
        Add_gz = 1; /* */    /*E0111*/
     else
        SYSTEM_RET(Add_gz, strcmp, (&DVM_String[i-3], ".gz"))

     if(Add_gz != 0)
        SYSTEM(strcat, (DVM_String, ".gz"))/* */    /*E0112*/

     SYSTEM(strncpy, (Res->Type, tp, 15)) /* */    /*E0113*/
     Res->Type[15] = '\x00';

     if(IsDVMInit && (DVMInputPar || Res->W))
     {
        /* */    /*E0114*/

        if(Res->ParIOType && Res->LocIOType)
        {  /* */    /*E0115*/

           OPENGZFILE
        }
        else
        {  /* */    /*E0116*/

           if(Res->LocIOType == 0)
           {  /* */    /*E0117*/

              if(DVM_IOProc == MPS_CurrentProc)
              {  if(Res->ParIOType)
                 {  /* */    /*E0118*/

                    OPENGZFILE
                 }
                 else
                 {  /* */    /*E0119*/

                    SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))

                    if(gzRCArr[0] == NULL && Res->W == 0 && Add_gz != 0)
                    {  /* */    /*E0120*/

                       DVM_String[i] = '\x00';
                       SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))
                    }
                 }
              }

              ( RTL_CALL, rtl_BroadCast(gzRCArr, 1, sizeof(gzFile),
                                        DVM_IOProc, NULL) );
           }
           else
           {  /* */    /*E0121*/

              SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))

              if(gzRCArr[0] == NULL && Res->W == 0 && Add_gz != 0)
              {  /* */    /*E0122*/

                 DVM_String[i] = '\x00';
                 SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))
              }
           }
        } 
     }
     else
     {
      #ifdef  _DVM_MPI_
        if(IsInit && (DVMInputPar || Res->W)) /* */    /*E0123*/
      #else
        if(IsSlaveRun && (DVMInputPar || Res->W)) /* */    /*E0124*/
      #endif
        {
           /* */    /*E0125*/

           if(Res->ParIOType && Res->LocIOType)
           {  /* */    /*E0126*/

              OPENGZFILE
           }
           else
           {  /* */    /*E0127*/

              if(Res->LocIOType == 0)
              {  /* */    /*E0128*/

                 if(CurrentProcIdent == MasterProcIdent)
                 {
                    if(Res->ParIOType)
                    {  /* */    /*E0129*/

                       OPENGZFILE
                    }
                    else
                    {  /* */    /*E0130*/

                       SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))

                       if(gzRCArr[0] == NULL && Res->W == 0 &&
                          Add_gz != 0)
                       {  /* */    /*E0131*/
      
                          DVM_String[i] = '\x00';
                          SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String,
                                                          tp))
                       }
                    }

                 }

                 ( RTL_CALL, mps_Bcast(gzRCArr, 1, sizeof(gzFile)) );
              }
              else
              {  /* */    /*E0132*/

                 SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))

                 if(gzRCArr[0] == NULL && Res->W == 0 && Add_gz != 0)
                 {  /* */    /*E0133*/

                    DVM_String[i] = '\x00';
                    SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))
                 }
              }
           }
        }
        else
        {
           /* */    /*E0134*/

           if(Res->ParIOType && Res->LocIOType)
           {  /* */    /*E0135*/

              OPENGZFILE
           }
           else
           {  /* */    /*E0136*/

              if(Res->ParIOType)
              {  /* */    /*E0137*/

                 OPENGZFILE
              }
              else
              {  /* */    /*E0138*/

                 SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))

                 if(gzRCArr[0] == NULL && Res->W == 0 && Add_gz != 0)
                 {  /* */    /*E0139*/

                    DVM_String[i] = '\x00';
                    SYSTEM_RET(gzRCArr[0], gzopen, (DVM_String, tp))
                 }
              }
           }
        }
     }

     Res->zlibFile = gzRCArr[0];
     Res->zip  = 1;         /* */    /*E0140*/


     if(Res->zlibFile == NULL)
     {  dvm_FreeStruct(Res);
        Res = NULL;
        FilesCount--;
     }
  }
  
#endif

  if(IsOpen == 0)
  {  if(i > 1)
     {  if(isdigit(tp[i-1]))
           tp[i-1] = '\x00';

        if(isdigit(tp[i-2]))
           tp[i-2] = '\x00';
     }
   
     if(IsDVMInit && (DVMInputPar || Res->W))
     {
        /* */    /*E0141*/

        if(Res->ParIOType && Res->LocIOType)
        {  /* */    /*E0142*/

           OPENFILE
        }
        else
        {
           if(Res->LocIOType == 0)
           {  /* */    /*E0143*/

              /* */    /*E0144*/

              if(DVM_IOProc == MPS_CurrentProc)
              {  if(Res->ParIOType)
                 {  /* */    /*E0145*/

                    OPENFILE
                 }
                 else
                 {  /* */    /*E0146*/

                    SYSTEM_RET(RCArr[0], fopen, (filename, tp))
                 }
              }
   
              ( RTL_CALL, rtl_BroadCast(RCArr, 1, sizeof(FILE *),
                                        DVM_IOProc, NULL) );
           }
           else
           {  /* */    /*E0147*/

              SYSTEM_RET(RCArr[0], fopen, (filename, tp))
           }
        }
     }
     else
     {
      #ifdef  _DVM_MPI_
        if(IsInit && (DVMInputPar || Res->W)) /* if MPS has been initialized */    /*E0148*/
      #else
        if(IsSlaveRun && (DVMInputPar || Res->W)) /* if offspring tasks are running */    /*E0149*/
      #endif
        {
           /* */    /*E0150*/

           if(Res->ParIOType && Res->LocIOType)
           {  /* */    /*E0151*/

              OPENFILE
           }
           else
           {
              if(Res->LocIOType == 0)
              {  /* */    /*E0152*/

                 /* */    /*E0153*/

                 if(CurrentProcIdent == MasterProcIdent)
                 {  if(Res->ParIOType)
                    {  /* */    /*E0154*/

                       OPENFILE
                    }
                    else
                    {  /* */    /*E0155*/

                       SYSTEM_RET(RCArr[0], fopen, (filename, tp))
                    }
                 }

                 ( RTL_CALL, mps_Bcast(RCArr, 1, sizeof(FILE *)) );
              }
              else
              {  /* */    /*E0156*/

                 SYSTEM_RET(RCArr[0], fopen, (filename, tp))
              }
           }
        }
        else
        {
           /* */    /*E0157*/

           if(Res->ParIOType && Res->LocIOType)
           {  /* */    /*E0158*/

              OPENFILE
           }
           else
           {  /* */    /*E0159*/

              if(Res->ParIOType)
              {  /* */    /*E0160*/

                 OPENFILE
              }
              else
              {  /* */    /*E0161*/

                 SYSTEM_RET(RCArr[0], fopen, (filename, tp))
              }
           } 
        }
     }

     Res->File = RCArr[0];
     Res->zip  = 0;         /* */    /*E0162*/

     if(Res->File == NULL)
     {  dvm_FreeStruct(Res);
        Res = NULL;
        FilesCount--;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fopen,"Res=%lx;\n", (uLLng)Res);

  DVMFTimeFinish(ret_dvm_fopen);
  return  (DVM_RET, Res);
}



void  dvm_void_fprintf(DVMFILE *stream, const char *format ,...)
{ va_list  arglist;

  DVMFTimeStart(call_dvm_void_fprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_void_fprintf,"Stream=%lx; Format=%s\n",
                                      (uLLng)stream, format);

  va_start(arglist, format);
  rtl_vfprintf(stream, format, arglist);
  va_end(arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_void_fprintf, " \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_void_fprintf);
  return;
}



int  dvm_fprintf(DVMFILE *stream, const char *format ,...)
{ va_list  arglist;
  int      Res;

  DVMFTimeStart(call_dvm_fprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fprintf,"Stream=%lx; Format=%s\n",
                                 (uLLng)stream, format);

  va_start(arglist, format);
  Res = rtl_rc_vfprintf(stream, format, arglist);
  va_end(arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fprintf,"Res=%d;\n", Res);

  DVMFTimeFinish(ret_dvm_fprintf);
  return  (DVM_RET, Res);
}



int  dvm_fputc(int c, DVMFILE *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsPutc = 0;

  DVMFTimeStart(call_dvm_fputc);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fputc,"Stream=%lx; Sym=%c;\n",
                               (uLLng)stream, (char)c);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsPutc = 1; /* */    /*E0163*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzputc, (stream->zlibFile, c))
  }


  #endif

  if(IsPutc == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], fputc, (c, stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fputc,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_fputc);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_fputs(const char *string, DVMFILE *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsPuts = 0;

  DVMFTimeStart(call_dvm_fputs);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fputs,"Stream=%lx; String=%s;\n",
                               (uLLng)stream, string);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsPuts = 1; /* */    /*E0164*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzputs, (stream->zlibFile, string))
  }

  #endif

  if(IsPuts == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], fputs, (string, stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fputs,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_fputs);
  return  (DVM_RET, ResArr[0]);
}



int dvm_fread(void *buffer, size_t size, size_t count, DVMFILE *stream)
{ int             ResArr[2+sizeof(double)/sizeof(int)] = {0};
  DvmType            LongCount;
  void           *_buffer;
  SysHandle      *ArrayHandlePtr = NULL;
  s_DISARRAY     *DArr;
  s_AMVIEW       *AMV;
  int             i;
  byte            IsRead = 0;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_fread);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(IsDVMInit)
     ArrayHandlePtr = TstDVMArray(buffer);

  if(IsDVMInit && ArrayHandlePtr != NULL)
  { /* buffer - pointer to the header of distributed array */    /*E0165*/

    if(RTL_TRACE)
       dvm_trace(call_dvm_fread,
           "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
           "Size=%d; Count=%d; Stream=%lx;\n",
           ArrayHandlePtr->HeaderPtr, (uLLng)ArrayHandlePtr,
           size, count, (uLLng)stream);
  
    DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
    AMV  = DArr->AMView; /* representation on which 
                            array is aligned */    /*E0166*/

    if(AMV == NULL)      /* if array has been mapped */    /*E0167*/
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 101.000: wrong call dvm_fread\n"
           "(the array has not been aligned with an abstract machine "
           "representation;\nArrayHeader[0]=%lx)\n",
           (uLLng)ArrayHandlePtr);

    /* Check if processor system on which the array is mapped 
       is a subsystem of a current processor system */    /*E0168*/

    NotSubsystem(i, DVM_VMS, AMV->VMS)

    if(i)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 101.001: wrong call dvm_fread\n"
            "(the array PS is not a subsystem of the current PS;\n"
            "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
            (uLLng)ArrayHandlePtr, (uLLng)AMV->VMS->HandlePtr,
            (uLLng)DVM_VMS->HandlePtr);

    ResArr[0] = ((s_DISARRAY *)ArrayHandlePtr->pP)->TLen;
    LongCount = (count*size)/ResArr[0];
    ResArr[0] = (int)(RTL_CALL, dvm_dfread((DvmType *)buffer, LongCount,
                                            stream) );
  }
  else
  { /* buffer - pointer to the header of ordinary array */    /*E0169*/

    if(RTL_TRACE)
       dvm_trace(call_dvm_fread,
                 "Buffer=%lx; Size=%d; Count=%d; Stream=%lx;\n",
                  (uLLng)buffer, size, count, (uLLng)stream);

    if(IsDVMInit && (DVMInputPar || stream->W))
    {
      IsRead = 0;

      #ifdef _DVM_ZLIB_

      if(stream->zip)
      {  IsRead = 1;  /* */    /*E0170*/
         ResArr[0] = (int)(size*count);

         if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
            SYSTEM_RET(ResArr[0], gzread, (stream->zlibFile,
                                           (voidp)buffer,
                                           (unsigned)(size*count)))
            if(ResArr[0] > 0)
               ResArr[0] /= size;
      }

      #endif

      if(IsRead == 0)
      {  ResArr[0] = (int)count;

         if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
            SYSTEM_RET(ResArr[0], fread, (buffer, size, count,
                                          stream->File))
      }

      if(dvm_void_fread == 0)
         ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                                   NULL) );
      if(ResArr[0] != 0)
      {
         #ifndef _DVM_IOPROC_

         if(EnableDynControl)
         {   /* Array initialization */    /*E0171*/

             DvmType lElmSize = size;
             DvmType lArrSize = count;

             ( RTL_CALL, dreada_((AddrType*)(&buffer), &lElmSize, &lArrSize) );
         }

         #endif

         LongCount = ResArr[0] * size;

         if((IsSynchr && UserSumFlag) || (LongCount & Msk3))
         {  mac_malloc(_buffer, void *, LongCount, 0);

            if(MPS_CurrentProc == DVM_IOProc)
               SYSTEM(memcpy, (_buffer, buffer, LongCount) )

            ( RTL_CALL, rtl_BroadCast(_buffer, ResArr[0], size,
                                      DVM_IOProc, NULL) );

            if(MPS_CurrentProc != DVM_IOProc)
               SYSTEM(memcpy, (buffer, _buffer, LongCount) )

            mac_free(&_buffer);
         }
         else
            ( RTL_CALL, rtl_BroadCast(buffer, ResArr[0], size,
                                      DVM_IOProc, NULL) );
      }
    }
    else
    {
    #ifdef _DVM_MPI_
      if(IsInit && (DVMInputPar || stream->W)) /* if MPS has been initialized */    /*E0172*/
    #else
      if(IsSlaveRun && (DVMInputPar || stream->W)) /* if offspring tasks are running */    /*E0173*/
    #endif
      {
        IsRead = 0;

        #ifdef _DVM_ZLIB_

        if(stream->zip)
        {  IsRead = 1;  /* */    /*E0174*/
        
           if(CurrentProcIdent == MasterProcIdent &&
              stream->zlibFile != NULL)
              SYSTEM_RET(ResArr[0], gzread, (stream->zlibFile,
                                             (voidp)buffer,
                                             (unsigned)(size*count)))
              if(ResArr[0] > 0)
                 ResArr[0] /= size;
        }

        #endif

        if(IsRead == 0)
        {  if(CurrentProcIdent == MasterProcIdent &&
              stream->File != NULL)
              SYSTEM_RET(ResArr[0], fread, (buffer, size, count,
                                            stream->File))
        }

        ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );

        if(ResArr[0] != 0)
           ( RTL_CALL, mps_Bcast(buffer, ResArr[0], size) );
      }
      else
      {
        IsRead = 0;

        #ifdef _DVM_ZLIB_

        if(stream->zip)
        {  IsRead = 1;  /* */    /*E0175*/
        
           if(stream->zlibFile != NULL)
              SYSTEM_RET(ResArr[0], gzread, (stream->zlibFile,
                                             (voidp)buffer,
                                             (unsigned)(size*count)))
           if(ResArr[0] > 0)
              ResArr[0] /= size;
        }

        #endif

        if(IsRead == 0)
        {  if(stream->File != NULL)
              SYSTEM_RET(ResArr[0], fread, (buffer, size, count,
                                            stream->File))
        }
      }
    }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fread,"Res=%d;\n", ResArr[0]);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_fread);
  StatOff = s_StatOff;
  return  (DVM_RET, ResArr[0]);
}



DVMFILE  *dvm_freopen(const char*  filename, const char  *type,
                      DVMFILE *stream)
{ FILE     *ResArr[2+sizeof(double)/sizeof(FILE *)] = {NULL};
  DVMFILE  *DVMRes = NULL;   

  DVMFTimeStart(call_dvm_freopen);

  if(RTL_TRACE)
     dvm_trace(call_dvm_freopen,"Filename=%s; Type=%s; Stream=%lx;\n",
                                 filename,type,(uLLng)stream);

  if((DVM_IOProc == MPS_CurrentProc) AND (stream != NULL) AND
     (stream->File != NULL))
     SYSTEM_RET(ResArr[0], freopen, (filename, type, stream->File))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(FILE *), DVM_IOProc,
                            NULL) );

  if(ResArr[0] != NULL)
  {
     DVMRes = stream;
     stream->File = ResArr[0];
     stream->FileID = FilesCount;
     FilesCount++;

    if(strpbrk(type, "wa+") != NULL)
       stream->W = 1;   /* */    /*E0176*/
    else
       stream->W = 0;
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_freopen,"Res=%lx;\n", (uLLng)DVMRes);

  DVMFTimeFinish(ret_dvm_freopen);
  return  (DVM_RET, DVMRes);
}



int  dvm_fscanf(DVMFILE  *stream, const char  *format, ...)
{
  int       Res;
  byte      s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_fscanf);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_fscanf,"Stream=%lx; Format=%s\n",
                                (uLLng)stream, format);

  va_start(arg_list_scan1, format);
  Res = rtl_vfscanf(stream, format);
  va_end(arg_list_scan1);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fscanf,"Res=%d;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_fscanf);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}



int  dvm_fseek(DVMFILE *stream, DvmType offset, int origin)
{ int     ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte    IsSeek = 0;
  byte    s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_fseek);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_fseek, "Stream=%lx; Offset=%ld; Origin=%d;\n",
                               (uLLng)stream, offset, origin);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsSeek = 1;  /* */    /*E0177*/

     ResArr[0] = -1;

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
     {
        SYSTEM_RET(ResArr[0], gzseek, (stream->zlibFile,
                                       (z_off_t)offset, origin))
        if(ResArr[0] >= 0)
           ResArr[0] = 0;
     }
  }

  #endif

  if(IsSeek == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], fseek, (stream->File, offset, origin))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fseek, "Res=%d;\n", ResArr[0]);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_fseek);
  StatOff = s_StatOff;
  return  (DVM_RET, ResArr[0]);
}



int  dvm_fsetpos(DVMFILE  *stream, const fpos_t  *pos)
{ int       ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_fsetpos);

  if(RTL_TRACE)
     dvm_trace(call_dvm_fsetpos,"Stream=%lx; Pos=%ld;\n",
                                 (uLLng)stream, *pos);

  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
     SYSTEM_RET(ResArr[0], fsetpos, (stream->File, pos))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fsetpos,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_fsetpos);
  return  (DVM_RET, ResArr[0]);
}



DvmType  dvm_ftell(DVMFILE *stream)
{
    DvmType    ResArr[2 + sizeof(double) / sizeof(DvmType)] = { -1L };
  byte    IsTell = 0;

  DVMFTimeStart(call_dvm_ftell);

  if(RTL_TRACE)
     dvm_trace(call_dvm_ftell,"Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsTell = 1;   /* */    /*E0178*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gztell, (stream->zlibFile))
  }

  #endif

  if(IsTell == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], ftell, (stream->File))
  }

  (RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(DvmType), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_ftell,"Res=%ld;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_ftell);
  return  (DVM_RET, ResArr[0]);
}


#ifndef _DVM_IOPROC_

int  dvm_fwrite(const void *buffer, size_t size, size_t count,
                DVMFILE *stream)
{ int             ResArr[2+sizeof(double)/sizeof(int)] = {0};
  DvmType            LongCount;
  SysHandle      *ArrayHandlePtr;
  s_DISARRAY     *DArr;
  s_AMVIEW       *AMV;
  int             i;
  byte            IsWrite = 0;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_fwrite);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  ArrayHandlePtr = TstDVMArray((void *)buffer);

  if(ArrayHandlePtr != NULL)
  { /* buffer - pointer to the header of distributed array */    /*E0179*/

    if(RTL_TRACE)
       dvm_trace(call_dvm_fwrite,
           "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
           "Size=%d; Count=%d; Stream=%lx;\n",
           ArrayHandlePtr->HeaderPtr, (uLLng)ArrayHandlePtr,
           size, count, (uLLng)stream);
  
    DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
    AMV  = DArr->AMView; /* representation on which 
                            array is aligned */    /*E0180*/

    if(AMV == NULL)      /* if array has been mapped */    /*E0181*/
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 101.010: wrong call dvm_fwrite\n"
           "(the array has not been aligned with an abstract machine "
           "representation;\nArrayHeader[0]=%lx)\n",
           (uLLng)ArrayHandlePtr);
 
    /* Check if processor system on which the array is mapped 
       is a subsystem of a current processor system */    /*E0182*/

    NotSubsystem(i, DVM_VMS, AMV->VMS)

    if(i)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 101.011: wrong call dvm_fwrite\n"
            "(the array PS is not a subsystem of the current PS;\n"
            "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
            (uLLng)ArrayHandlePtr, (uLLng)AMV->VMS->HandlePtr,
            (uLLng)DVM_VMS->HandlePtr);

    ResArr[0] = ((s_DISARRAY *)ArrayHandlePtr->pP)->TLen;
    LongCount = (count*size)/ResArr[0];
    ResArr[0] = (int)(RTL_CALL, dvm_dfwrite((DvmType *)buffer, LongCount, stream) );
  }
  else
  { /* buffer - pointer to the header of ordinary array */    /*E0183*/

    if(RTL_TRACE)
       dvm_trace(call_dvm_fwrite,
                 "Buffer=%lx; Size=%d; Count=%d; Stream=%lx;\n",
                  (uLLng)buffer,size,count,(uLLng)stream);

    IsWrite = 0;

    #ifdef _DVM_ZLIB_

    if(stream->zip)
    {  IsWrite = 1;  /* */    /*E0184*/
       ResArr[0] = (int)(size*count);

       if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
          SYSTEM_RET(ResArr[0], gzwrite, (stream->zlibFile,
                                          (voidp)buffer,
                                          (unsigned)(size*count)))
       if(ResArr[0] > 0)
          ResArr[0] /= size;
    }

    #endif

    if(IsWrite == 0)
    {  ResArr[0] = (int)count;

       if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
          SYSTEM_RET(ResArr[0], fwrite, (buffer, size, count,
                                         stream->File))
    }

    if(dvm_void_fwrite == 0)
       ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                                 NULL) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_fwrite,"Res=%d;\n", ResArr[0]);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_fwrite);
  StatOff = s_StatOff;
  return  (DVM_RET, ResArr[0]);
}

#endif


int  dvm_getc(DVMFILE  *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsGetc = 0;

  DVMFTimeStart(call_dvm_getc);

  if(RTL_TRACE)
     dvm_trace(call_dvm_getc, "Stream=%lx;\n", (uLLng)stream);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsGetc = 1; /* */    /*E0185*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzgetc, (stream->zlibFile))
  }

  #endif

  if(IsGetc == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], getc, (stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_getc, "Sym=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_getc);
  return  (DVM_RET, ResArr[0]);
}



int dvm_getchar(void)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_getchar);

  if(RTL_TRACE)
     dvm_trace(call_dvm_getchar," \n");

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(ResArr[0],getchar,()) 

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_getchar,"Sym=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_getchar);
  return  (DVM_RET, ResArr[0]);
}



char *dvm_gets(char *buffer)
{ char  *ResArr[2+sizeof(double)/sizeof(char *)] ={NULL};
  int    StrLenArr[2+sizeof(double)/sizeof(int)] = {0};
  char  *_buffer;
  int    n = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0186*/

  DVMFTimeStart(call_dvm_gets);

  if(RTL_TRACE)
     dvm_trace(call_dvm_gets," \n");

  if(DVM_IOProc == MPS_CurrentProc)
  {

     SYSTEM_RET(ResArr[0], fgets, (buffer, n, stdin))

     if(ResArr[0] != NULL)
     {  SYSTEM_RET(StrLenArr[0], strlen, (buffer))
        StrLenArr[0]++;
     }
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(char *), DVM_IOProc,
                            NULL) );

  if(ResArr[0] != NULL)
  {  ( RTL_CALL, rtl_BroadCast(StrLenArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );

     if(StrLenArr[0] != 0)
     {  if((IsSynchr && UserSumFlag) || (StrLenArr[0] & Msk3))
        {  mac_malloc(_buffer, char *, StrLenArr[0], 0);
           if(MPS_CurrentProc == DVM_IOProc)
              SYSTEM(memcpy, (_buffer, buffer, StrLenArr[0]) )

           ( RTL_CALL, rtl_BroadCast(_buffer, 1, StrLenArr[0],
                                     DVM_IOProc, NULL) );

           if(MPS_CurrentProc != DVM_IOProc)
              SYSTEM(memcpy, (buffer, _buffer, StrLenArr[0]) )
           mac_free(&_buffer);
        }
        else
           ( RTL_CALL, rtl_BroadCast(buffer, 1, StrLenArr[0],
                                     DVM_IOProc, NULL) );
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_gets,"Res=%lx; String=%s\n",
                             (uLLng)ResArr[0], buffer);

  DVMFTimeFinish(ret_dvm_gets);
  return  (DVM_RET, ResArr[0]);
}



void dvm_void_printf(const char *format,...)
{ va_list arglist;

  DVMFTimeStart(call_dvm_void_printf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_void_printf,"Format=%s\n", format); 
 
  va_start(arglist, format);
  rtl_vfprintf(NULL, format, arglist);
  va_end(arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_void_printf, " \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_void_printf);
  return;
}



int dvm_printf(const char *format,...)
{ va_list arglist;
  int     Res;

  DVMFTimeStart(call_dvm_printf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_printf,"Format=%s\n", format); 
 
  va_start(arglist, format);
  Res = rtl_rc_vfprintf(NULL, format, arglist);
  va_end(arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_printf,"Res=%d;\n", Res);

  DVMFTimeFinish(ret_dvm_printf);
  return  (DVM_RET, Res);
}



int  dvm_putc(int  c, DVMFILE  *stream)
{ int    ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  byte   IsPutc = 0;

  DVMFTimeStart(call_dvm_putc);

  if(RTL_TRACE)
     dvm_trace(call_dvm_putc,"Stream=%lx; Sym=%c;\n",
                              (uLLng)stream, (char)c);

  #ifdef _DVM_ZLIB_

  if(stream->zip)
  {  IsPutc = 1; /* */    /*E0187*/

     if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
        SYSTEM_RET(ResArr[0], gzputc, (stream->zlibFile, c))
  }

  #endif

  if(IsPutc == 0)
  {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
        SYSTEM_RET(ResArr[0], putc, (c, stream->File))
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_putc,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_putc);
  return  (DVM_RET, ResArr[0]);
}



int dvm_putchar(int c)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_putchar);

  if(RTL_TRACE)
     dvm_trace(call_dvm_putchar,"Sym=%c;\n", (char)c);

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(ResArr[0],putchar, (c)) 

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_putchar,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_putchar);
  return  (DVM_RET, ResArr[0]);
}



int dvm_puts(const char *string)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_puts);

  if(RTL_TRACE)
     dvm_trace(call_dvm_puts,"String=%s;\n", string);

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(ResArr[0],puts, (string)) 

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_puts,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_puts);
  return  (DVM_RET, ResArr[0]);
}



void  dvm_rewind(DVMFILE  *stream)
{ byte  IsRewind = 0;

  DVMFTimeStart(call_dvm_rewind);

  if(RTL_TRACE)
     dvm_trace(call_dvm_rewind, "Stream=%lx;\n", (uLLng)stream); 

  if(IsDVMInit && (DVMInputPar || stream->W))
  {
     #ifdef _DVM_ZLIB_

     if(stream->zip)
     {  IsRewind = 1;  /* */    /*E0188*/

        if(DVM_IOProc == MPS_CurrentProc && stream->zlibFile != NULL)
           SYSTEM(gzrewind, (stream->zlibFile))
     }

     #endif

     if(IsRewind == 0)
     {  if(DVM_IOProc == MPS_CurrentProc && stream->File != NULL)
           SYSTEM(rewind, (stream->File))
     }
  }
  else
  {  
    #ifdef _DVM_MPI_
      if(IsInit && (DVMInputPar || stream->W)) /* if MPS has been initialized */    /*E0189*/
    #else
      if(IsSlaveRun && (DVMInputPar || stream->W)) /* if offspring tasks are running */    /*E0190*/
    #endif
      {
         #ifdef _DVM_ZLIB_

         if(stream->zip)
         {  IsRewind = 1;  /* */    /*E0191*/

            if(CurrentProcIdent == MasterProcIdent &&
               stream->zlibFile != NULL)
               SYSTEM(gzrewind, (stream->zlibFile))
         }

         #endif

         if(IsRewind == 0)
         {  if(CurrentProcIdent == MasterProcIdent &&
               stream->File != NULL)
               SYSTEM(rewind, (stream->File))
         }
      }
      else 
      {
         #ifdef _DVM_ZLIB_

         if(stream->zip)
         {  IsRewind = 1;  /* */    /*E0192*/

            if(stream->zlibFile != NULL)
               SYSTEM(gzrewind, (stream->zlibFile))
         }

         #endif

         if(IsRewind == 0)
         {  if(stream->File != NULL)
               SYSTEM(rewind, (stream->File))
         }
      }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_rewind, " \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_rewind);
  return;
}



int  dvm_scanf(const char  *format, ...)
{
  int       Res;
  byte      s_RTL_TRACE, s_StatOff;


  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_scanf);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_scanf,"Format=%s\n", format);

  va_start(arg_list_scan1, format);
  Res = rtl_vfscanf(NULL, format);
  va_end(arg_list_scan1);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_scanf,"Res=%d;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_scanf);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}



void dvm_setbuf(DVMFILE *stream,char *buffer)
{
  DVMFTimeStart(call_dvm_setbuf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_setbuf,"Stream=%lx;\n", (uLLng)stream); 

  if( (DVM_IOProc == MPS_CurrentProc) AND (stream != NULL) AND
      (stream->File != NULL) )
      SYSTEM(setbuf,(stream->File,buffer))

  if(RTL_TRACE)
     dvm_trace(ret_dvm_setbuf," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_setbuf);
  return;
}



int  dvm_setvbuf(DVMFILE *stream, char *buffer, int type, int size)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_setvbuf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_setvbuf,"Stream=%lx; Type=%d; Size=%d;\n",
                                 (uLLng)stream,type,size); 

  if( (DVM_IOProc == MPS_CurrentProc) AND (stream != NULL) AND
      (stream->File != NULL) )
      SYSTEM_RET(ResArr[0],setvbuf, (stream->File, buffer, type, size))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_setvbuf,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_setvbuf);
  return  (DVM_RET, ResArr[0]);
}



DVMFILE *dvm_tmpfile(void)
{ DVMFILE *Res=NULL;
  FILE    *RCArr[2+sizeof(double)/sizeof(FILE *)];

  DVMFTimeStart(call_dvm_tmpfile);

  if(RTL_TRACE)
     dvm_trace(call_dvm_tmpfile," \n");

  dvm_AllocStruct(DVMFILE, Res);
  Res->FileID = FilesCount;
  FilesCount++;

  if(DVM_IOProc == MPS_CurrentProc)
     SYSTEM_RET(RCArr[0],tmpfile,())

  ( RTL_CALL, rtl_BroadCast(RCArr, 1, sizeof(FILE *), DVM_IOProc,
                            NULL) );

  Res->File = RCArr[0];

  if (Res->File == NULL)
  {  dvm_FreeStruct(Res);
     Res = NULL;
     FilesCount--;
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_tmpfile,"Res=%lx;\n", (uLLng)Res);

  DVMFTimeFinish(ret_dvm_tmpfile);
  return  (DVM_RET, Res);
}



int dvm_ungetc(int c,DVMFILE *stream)
{ int  ResArr[2+sizeof(double)/sizeof(int)] = {EOF};

  DVMFTimeStart(call_dvm_ungetc);

  if(RTL_TRACE)
     dvm_trace(call_dvm_ungetc,"Stream=%lx; Sym=%c;\n",
                                (uLLng)stream,(char)c);

  if((DVM_IOProc == MPS_CurrentProc) AND (stream != NULL) AND
     (stream->File !=NULL))
     SYSTEM_RET(ResArr[0],ungetc, (c, stream->File)) 

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_ungetc,"Res=%c;\n", (char)ResArr[0]);

  DVMFTimeFinish(ret_dvm_ungetc);
  return  (DVM_RET, ResArr[0]);
}



void dvm_void_vfprintf(DVMFILE *stream,const char *format,
                       va_list arglist)
{ 
  DVMFTimeStart(call_dvm_void_vfprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_void_vfprintf,"Stream=%lx; Format=%s\n",
                                       (uLLng)stream,format);

  rtl_vfprintf(stream, format, arglist);
 
  if(RTL_TRACE)
     dvm_trace(ret_dvm_void_vfprintf," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_void_vfprintf);
  return;
}



int dvm_vfprintf(DVMFILE *stream, const char *format, va_list arglist)
{ int Res;

  DVMFTimeStart(call_dvm_vfprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_vfprintf,"Stream=%lx; Format=%s\n",
                                  (uLLng)stream, format);

  Res = rtl_rc_vfprintf(stream, format, arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_vfprintf,"Res=%d;\n", Res); 

  DVMFTimeFinish(ret_dvm_vfprintf);
  return  (DVM_RET, Res);
}



void dvm_void_vprintf(const char *format,va_list arglist)
{ 
  DVMFTimeStart(call_dvm_void_vprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_void_vprintf,"Format=%s\n", format); 

  rtl_vfprintf(NULL, format, arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_void_vprintf," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_dvm_void_vprintf);
  return;
}



int dvm_vprintf(const char *format,va_list arglist)
{ int Res;

  DVMFTimeStart(call_dvm_vprintf);

  if(RTL_TRACE)
     dvm_trace(call_dvm_vprintf,"Format=%s\n", format); 

  Res = rtl_rc_vfprintf(NULL, format, arglist);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_vprintf,"Res=%d;\n", Res);

  DVMFTimeFinish(ret_dvm_vprintf);
  return  (DVM_RET, Res);
}



/*    TURBO-C    */    /*E0193*/

#ifdef  _DVM_VSCANF_

int  dvm_vscanf(const char  *format, va_list  arg_ptr)
{ int       Res;
  byte      s_RTL_TRACE, s_StatOff; 

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_vscanf);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_vscanf,"Format=%s\n", format);

  arg_list_scan1 = arg_ptr;

  Res = rtl_vfscanf(NULL, format);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_vscanf,"Res=%d;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_vscanf);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}



int dvm_vfscanf(DVMFILE  *stream, const char  *format, va_list  arg_ptr)
{ int Res;
  byte      s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvm_vfscanf);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_vfscanf,"Stream=%lx; Format=%s\n",
                                 (uLLng)stream, format);

  arg_list_scan1 = arg_ptr;

  Res = rtl_vfscanf(stream, format);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_vfscanf,"Res=%d;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvm_vfscanf);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}

#endif



/***********************************************\
* Folders and files operations of ANSI standard *
\***********************************************/    /*E0194*/

int dvm_remove(const char *filename)
{ int  ResArr[2+sizeof(double)/sizeof(int)];

  DVMFTimeStart(call_dvm_remove);

  if(RTL_TRACE)
     dvm_trace(call_dvm_remove,"FileName=%s\n", filename);

  if(MPS_CurrentProc == DVM_IOProc)
     SYSTEM_RET(ResArr[0],remove, (filename))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_remove,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_remove);
  return  (DVM_RET, ResArr[0]);
}



int  dvm_rename(const char *oldname, const char *newname)
{ int  ResArr[2+sizeof(double)/sizeof(int)];

  DVMFTimeStart(call_dvm_rename);

  if(RTL_TRACE)
     dvm_trace(call_dvm_rename,"OldName=%s  NewName=%s\n",
                                oldname, newname);

  if(MPS_CurrentProc == DVM_IOProc)
     SYSTEM_RET(ResArr[0],rename, (oldname, newname))

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  if(RTL_TRACE)
     dvm_trace(ret_dvm_rename,"Res=%d;\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_rename);
  return  (DVM_RET, ResArr[0]);
}



char *dvm_tmpnam(char *filename)
{ char  *ResArr[2+sizeof(double)/sizeof(char *)] = {NULL};
  int    StrLenArr[2+sizeof(double)/sizeof(int)] = {0};
  char  *_filename;

  DVMFTimeStart(call_dvm_tmpnam);

  if(RTL_TRACE)
     dvm_trace(call_dvm_tmpnam," \n");

  if(DVM_IOProc == MPS_CurrentProc)
  {  SYSTEM_RET(ResArr[0],tmpnam, (filename))
     if(ResArr[0] != NULL)
     {  SYSTEM_RET(StrLenArr[0],strlen, (filename))
        StrLenArr[0]++;
     }
  }

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(char *), DVM_IOProc,
                            NULL) );
  if(ResArr[0] != NULL)
  {  ( RTL_CALL, rtl_BroadCast(StrLenArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );

     if(StrLenArr[0] != 0)
     {  if((IsSynchr && UserSumFlag) || (StrLenArr[0] & Msk3))
        {  mac_malloc(_filename, char *, StrLenArr[0], 0);
           if(MPS_CurrentProc == DVM_IOProc)
              SYSTEM(memcpy, (_filename, filename, StrLenArr[0]) )

           ( RTL_CALL, rtl_BroadCast(_filename, 1, StrLenArr[0],
                                     DVM_IOProc, NULL) );

           if(MPS_CurrentProc != DVM_IOProc)
              SYSTEM(memcpy, (filename, _filename, StrLenArr[0]) )
           mac_free(&_filename);
        }
        else
           ( RTL_CALL, rtl_BroadCast(filename, 1, StrLenArr[0],
                                     DVM_IOProc, NULL) );
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvm_tmpnam,"Res=%lx; FileName=%s\n",
                               (uLLng)ResArr[0], filename);

  DVMFTimeFinish(ret_dvm_tmpnam);
  return  (DVM_RET, ResArr[0]);
}



/*******************\
* Support functions *
\*******************/    /*E0195*/

void rtl_vfprintf(DVMFILE *stream, const char *format, va_list arglist)
{ FILE    *StreamPtr;
  byte     IsPrint = 0;

  if(stream != NULL)
  {
     #ifdef _DVM_ZLIB_

     if(stream->zip)
     {  IsPrint = 1;  /* */    /*E0196*/

        gz_vfprintf(stream->zlibFile, format, arglist);
     }

     #endif

     if(IsPrint != 0)
        return;  /* */    /*E0197*/

     StreamPtr = stream->File;
  }
  else
     StreamPtr = stdout;

  if(StreamPtr == NULL)
     StreamPtr = stdout;

  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        SYSTEM(vfprintf,(StreamPtr, format, arglist))
  }
  else
  { if(CurrentProcIdent == MasterProcIdent)
       SYSTEM(vfprintf,(StreamPtr, format, arglist))
  }

  return;
}



int  rtl_rc_vfprintf(DVMFILE *stream, const char *format,
                     va_list arglist)
{ FILE     *StreamPtr;
  int       ResArr[2+sizeof(double)/sizeof(int)];
  byte      IsPrint = 0;

  if(stream != NULL)
  {
     #ifdef _DVM_ZLIB_

     if(stream->zip)
     {  IsPrint = 1;  /* */    /*E0198*/

        ResArr[0] = gz_rc_vfprintf(stream->zlibFile, format, arglist);
     }

     #endif

     if(IsPrint != 0)
        return  ResArr[0]; /* */    /*E0199*/

     StreamPtr = stream->File;
  }
  else
     StreamPtr = stdout;

  if(StreamPtr == NULL)
     StreamPtr = stdout;

  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        SYSTEM_RET(ResArr[0], vfprintf, (StreamPtr, format, arglist))

     ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );
  }
  else
  {  if(CurrentProcIdent == MasterProcIdent)
        SYSTEM_RET(ResArr[0], vfprintf, (StreamPtr, format, arglist))

     ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
  } 

  return  ResArr[0];
}
     


#ifdef _DVM_ZLIB_

#ifndef Z_PRINTF_BUFSIZE
# define Z_PRINTF_BUFSIZE 4096
#endif

static char dvm_gzBuf[Z_PRINTF_BUFSIZE];

int  gzvfprintf(gzFile  file, const char  *format, va_list  arglist)
{
  int     len;

  len = vsprintf(dvm_gzBuf, format, arglist);

  /* Some *sprintf don't return the nb of bytes written */    /*e0200*/

  /*len = strlen(dvm_gzBuf);*/    /*e0201*/

  if(len >= (Z_PRINTF_BUFSIZE-1))
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err 100.020: wrong call gzvfprintf "
             "(overflowing of vsprintf buffer dvm_gzBuf)\n");


  if(len <= 0)
     return  0;

  /* gzputs(file, dvm_gzBuf); */    /*e0202*/

  gzwrite(file, dvm_gzBuf, (unsigned)len);

  return  len;
}



void  gz_vfprintf(gzFile  file, const char  *format, va_list  arglist)
{
  int     len;

  len = vsprintf(dvm_gzBuf, format, arglist);

  /* Some *sprintf don't return the nb of bytes written */    /*e0203*/

  /*len = strlen(dvm_gzBuf);*/    /*e0204*/

  if(len >= (Z_PRINTF_BUFSIZE-1))
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err 100.021: wrong call gz_vfprintf "
             "(overflowing of vsprintf buffer dvm_gzBuf)\n");

  if(len <= 0)
     return;

/*
  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        gzputs(file, dvm_gzBuf);
  }
  else
  {  if(CurrentProcIdent == MasterProcIdent)
        gzputs(file, dvm_gzBuf);
  }
*/    /*e0205*/

  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        gzwrite(file, dvm_gzBuf, (unsigned)len);
  }
  else
  {  if(CurrentProcIdent == MasterProcIdent)
        gzwrite(file, dvm_gzBuf, (unsigned)len);
  }

  return;
}



int  gz_rc_vfprintf(gzFile  file, const char  *format, va_list  arglist)
{
  int     len;
  int     ResArr[2+sizeof(double)/sizeof(int)];

  len = vsprintf(dvm_gzBuf, format, arglist);

  /* Some *sprintf don't return the nb of bytes written */    /*e0206*/

  /*len = strlen(dvm_gzBuf);*/    /*e0207*/

  if(len >= (Z_PRINTF_BUFSIZE-1))
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err 100.022: wrong call gz_rc_vfprintf "
             "(overflowing of vsprintf buffer dvm_gzBuf)\n");

  if(len <= 0)
     return  0;

/*
  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        ResArr[0] = gzputs(file, dvm_gzBuf);

     ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );
  }
  else
  {  if(CurrentProcIdent == MasterProcIdent)
        ResArr[0] = gzputs(file, dvm_gzBuf);

     ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
  }
*/    /*e0208*/

  if(IsDVMInit)
  {  if(DVM_IOProc == MPS_CurrentProc)
        ResArr[0] = gzwrite(file, dvm_gzBuf, (unsigned)len);

     ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc,
                               NULL) );
  }
  else
  {  if(CurrentProcIdent == MasterProcIdent)
        ResArr[0] = gzwrite(file, dvm_gzBuf, (unsigned)len);

     ( RTL_CALL, mps_Bcast(ResArr, 1, sizeof(int)) );
  } 

  return  ResArr[0];
}

#endif


/* */    /*E0209*/


int  dvm_gzsetparams(DVMFILE *stream, int  level, int  strategy)
{ int   ResArr[2+sizeof(double)/sizeof(int)] = {0};
  int   LEVEL = level;

  DVMFTimeStart(call_dvm_gzsetparams);

  if(RTL_TRACE)
     dvm_trace(call_dvm_gzsetparams,
               "Stream=%lx; level=%d; strategy=%d;\n",
               (uLLng)stream, level, strategy);

  #ifdef _DVM_ZLIB_

  if(level == -1 || CompressLevel == -1)
     LEVEL = 0;
  else
  {  if(level == 0)
     {  if(CompressLevel == 0)
           LEVEL = Z_DEFAULT_COMPRESSION;
        else
           LEVEL = CompressLevel;
     }
  }

  if(DVM_IOProc == MPS_CurrentProc && stream->zip)
     ResArr[0] = gzsetparams(stream->zlibFile, LEVEL, strategy);

  ( RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(int), DVM_IOProc, NULL) );

  #endif

  if(RTL_TRACE)
     dvm_trace(ret_dvm_gzsetparams, "RC=%d\n", ResArr[0]);

  DVMFTimeFinish(ret_dvm_gzsetparams);
  return  (DVM_RET, ResArr[0]);
}



void  dvm_gzflush(DVMFILE *stream, int  flush)
{
  DVMFTimeStart(call_dvm_gzflush);

  if(RTL_TRACE)
     dvm_trace(call_dvm_gzflush,
               "Stream=%lx; flush=%d;\n", (uLLng)stream, flush);

  #ifdef _DVM_ZLIB_

  if(stream != NULL)
  {  if(flush == 0)
        stream->flush = Z_SYNC_FLUSH;  /* */    /*E0210*/
     else
        stream->flush = Z_FULL_FLUSH;
  }

  #endif

  if(RTL_TRACE)
     dvm_trace(ret_dvm_gzflush, " \n");

  DVMFTimeFinish(ret_dvm_gzflush);
  return;
}


/* ----------------------------------------------- */    /*E0211*/


int  rtl_vfscanf(DVMFILE *stream, const char *formusr)
{
  char  *format, *formsave;
  /* number of input variables 
                            stored in memory */    /*E0212*/
  int    VarNumberArr[2+sizeof(double)/sizeof(int)] = {0};

  int    SymNumber = 0; /* number of characters
                           read from input stream */    /*E0213*/
  FILE  *StreamPtr;     /* input stream pointer */    /*E0214*/
  char  *ControlPtr;    /* current % pointer */    /*E0215*/
  char   SaveSym;
  int    FormatPos;     /* position of current format chacacter
                           relative to ControlPtr  */    /*E0216*/
  int    ResScan1;
  char  *ControlSym = "*FNhldDoOxXiIuUeEgGfcs[n";
  char  *FormatSym = "dDoOxXiIuUeEgGfcs]n";
  int    res1, res2 = 0;
  byte   CondTrue = TRUE;

  #ifdef _DVM_ZLIB_
     char  *buf;
  #endif

  #define  buf_size  50000  /* */    /*E0217*/

  SYSTEM_RET(res1, strlen, (formusr))
  res1++;
  mac_malloc(formsave, char *, res1, 1);
  format = formsave;

  if(format == NULL)
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err 100.000: wrong call rtl_vfscanf "
             "(no memory for format string)\n");

  SYSTEM(strcpy, (format, formusr))

  if(stream != NULL)
  {
     #ifdef _DVM_ZLIB_

     if(stream->zip)
     {  res2 = 1;   /* */    /*E0218*/
        if(MPS_CurrentProc == DVM_IOProc)
        {  if(stream->ScanFile == NULL)
           {  /* */    /*E0219*/

              mac_malloc(buf, char *, buf_size, 0);

              SYSTEM(sprintf, (DVM_String, "%ld", ScanFilesCount))
              SYSTEM(strcat, (DVM_String, "scan.dvm"))
              SYSTEM_RET(stream->ScanFile, fopen, (DVM_String,
                                                   OPENMODE(w)))

              if(stream->ScanFile == NULL)
                 eprintf(__FILE__,__LINE__,
                   "*** RTS fatal err 100.001: wrong call rtl_vfscanf\n"
                   "(can not open temporary file <%s> "
                   "for scan function; type=w)\n", DVM_String);

              stream->ScanFileID = ScanFilesCount;

              while(CondTrue)
              {  /* */    /*E0220*/

                  SYSTEM_RET(ResScan1, gzread,
                             (stream->zlibFile, (voidp)buf,
                              (unsigned)(buf_size-1)))
                 if(ResScan1 <= 0)
                    break;  /* */    /*E0221*/
 
                 SYSTEM(fwrite, (buf, 1, ResScan1, stream->ScanFile))

                 if(ResScan1 < (buf_size-1))
                    break;  /* */    /*E0222*/ 
              }

              /* */    /*E0223*/

              SYSTEM(fclose, (stream->ScanFile))

              SYSTEM_RET(ResScan1, strlen, (stream->Type))

              if(stream->Type[ResScan1-1] == 'f' ||
                 stream->Type[ResScan1-1] == 'h')
                 stream->Type[ResScan1-1] = '\x00';

              SYSTEM_RET(ResScan1, strlen, (stream->Type))

              if( isdigit(stream->Type[ResScan1-1]) )
                  stream->Type[ResScan1-1] = '\x00';

              SYSTEM_RET(stream->ScanFile, fopen, (DVM_String,
                                                   stream->Type))
              if(stream->ScanFile == NULL)
                 eprintf(__FILE__,__LINE__,
                   "*** RTS fatal err 100.001: wrong call rtl_vfscanf\n"
                   "(can not open temporary file <%s> "
                   "for scan function; type=%s)\n",
                   DVM_String, stream->Type);

              ScanFilesCount++;
           }

           StreamPtr = stream->ScanFile;
        }
     }

     #endif

     if(res2 == 0)
        StreamPtr = stream->File;
  }
  else
     StreamPtr = stdin;

  if(StreamPtr == NULL)
     StreamPtr = stdin;

  ControlPtr = format;

  while(CondTrue)
  {  SYSTEM_RET(ControlPtr, strchr, (ControlPtr, '%'))

     if(ControlPtr == NULL)
     {  if(format[0] != '\x00')
        {  int dummy;
           if(MPS_CurrentProc == DVM_IOProc) 
              SYSTEM(fscanf, (StreamPtr, format, &dummy))
        }

        break;
     }

     SYSTEM_RET(res1, strcspn, (ControlPtr, ControlSym))
     SYSTEM_RET(res2, isdigit, (ControlPtr[1]))

     if(res1 != 1 && res2 == 0)
     {  /* first after % character that is
           neither digit no control character */    /*E0224*/

        ++ControlPtr;
        continue;
     }
     else
     {  /* first after % character that is
           digit or control character */    /*E0225*/

        SYSTEM_RET(FormatPos, strcspn, (ControlPtr, FormatSym))

        if(FormatPos < 0 || ControlPtr[FormatPos] == '\x00')
           break; /* there is no format character */    /*E0226*/

        SaveSym = ControlPtr[FormatPos+1];
        ControlPtr[FormatPos+1] = '\x00';
        ResScan1 = dvm_scan1(StreamPtr, format, &SymNumber, ControlPtr,
                             FormatPos);
        ControlPtr[FormatPos+1] = SaveSym;

        if(ResScan1 < 0)
        {  VarNumberArr[0] = ResScan1;
           break;   /* end of file is reached or
                       format is too long */    /*E0227*/
        }

        VarNumberArr[0] += ResScan1;
        format = &ControlPtr[FormatPos+1];
        ControlPtr = format;
        continue;
     }
  }

  if(dvm_void_scan == 0)
     ( RTL_CALL, rtl_BroadCast(VarNumberArr, 1, sizeof(int),
                               DVM_IOProc, NULL) );
  else
     VarNumberArr[0] = 0;
     
  mac_free(&formsave);

  return  VarNumberArr[0];
}



int dvm_scan1(FILE *stream,char *format1, int *SymNumberPtr,
              char *ControlPtr, int FormatPos)

{ int    VarType;        /* input variable type */    /*E0228*/
  int    Res = 0;        /* value returned by  function dvm_scan1 */    /*E0229*/
  int    SymNumber = 0;  /* number of chacacters input 
                            through one dvm_scan1  call */    /*E0230*/
  char   format2[256];   /* format for 
                            one variable input */    /*E0231*/
  char  *ResPtr;
  int    ResInt;
  /* number of characters input for [],s,c  */    /*E0232*/
  int    WidthArr[2+sizeof(double)/sizeof(int)] = {0};
  int    i;

  char            *Ptr0;
  short           *Ptr1;
  int             *Ptr2;
  DvmType         *Ptr3;
  float           *Ptr4;
  double          *Ptr5;

  char            *_CharPtr;
  int              wPtr[3*sizeof(double)/sizeof(int)];

  SYSTEM_RET(ResInt, strlen, (format1))

  if(ResInt > 252)
     return EOF-1;    /* too long format */    /*E0233*/

  SYSTEM(strcpy, (format2, format1))
  SYSTEM(strcat, (format2, "%n"))

  switch(ControlPtr[FormatPos])
  {  case 'd': VarType=2;  break;
     case 'D': VarType=3;  break;
     case 'o': VarType=2;  break;
     case 'O': VarType=3;  break;
     case 'x': VarType=2;  break;
     case 'X': VarType=3;  break;
     case 'i': VarType=2;  break;
     case 'I': VarType=3;  break;
     case 'u': VarType=2;  break;
     case 'U': VarType=3;  break;
     case 'e': VarType=4;  break;
     case 'E': VarType=4;  break;
     case 'g': VarType=4;  break;
     case 'G': VarType=4;  break;
     case 'f': VarType=4;  break;
     case 'c': VarType=0;  break;
     case 'n': VarType=2;  break;
     case 's': VarType=0;  break;
     case ']': VarType=0;  break;
  }

  SYSTEM_RET(ResPtr, strchr, (ControlPtr, 'h'))

  if(ResPtr != NULL)  switch(ControlPtr[FormatPos])
                      {  /*      int -> short      */    /*E0234*/
                         case 'd': VarType=1; break;
                         case 'o': VarType=1; break;
                         case 'x': VarType=1; break;
                         case 'i': VarType=1; break;
                         case 'u': VarType=1; break;
                      }

  SYSTEM_RET(ResPtr, strchr, (ControlPtr, 'l'))
  if(ResPtr != NULL)  switch(ControlPtr[FormatPos])
                      {  /* int -> long , float -> double */    /*E0235*/
                         case 'd': VarType=3;  break;
                         case 'o': VarType=3;  break;
                         case 'x': VarType=3;  break;
                         case 'i': VarType=3;  break;
                         case 'u': VarType=3;  break;
                         case 'e': VarType=5;  break;
                         case 'E': VarType=5;  break;
                         case 'g': VarType=5;  break;
                         case 'G': VarType=5;  break;
                         case 'f': VarType=5;  break;
                      }
   
  
  switch(VarType)
  {  case 0:  Ptr0 = va_arg(arg_list_scan1, char   *);
                            break;
     case 1:  Ptr1 = va_arg(arg_list_scan1, short  *);
                            break;
     case 2:  Ptr2 = va_arg(arg_list_scan1, int    *);
                            break;
     case 3:  Ptr3 = va_arg(arg_list_scan1, DvmType   *);
                            break;
     case 4:  Ptr4 = va_arg(arg_list_scan1, float  *);
                            break;
     case 5:  Ptr5 = va_arg(arg_list_scan1, double *);
                            break;
  }

  if(MPS_CurrentProc == DVM_IOProc)
  {  switch(VarType)
     {  case 0:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (char   *)Ptr0, (int *)&SymNumber))
                 break;
        case 1:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (short  *)Ptr1, (int *)&SymNumber))
                 break;
        case 2:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (int    *)Ptr2, (int *)&SymNumber))
                 break;
        case 3:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (DvmType   *)Ptr3, (int *)&SymNumber))
                 break;
        case 4:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (float  *)Ptr4, (int *)&SymNumber))
                 break;
        case 5:  SYSTEM_RET(Res,fscanf, (stream, format2,
                            (double *)Ptr5, (int *)&SymNumber))
                 break;
     }

     if(Res >= 0)
        *SymNumberPtr += SymNumber;
  }

  if(Res >= 0 && ControlPtr[1] != '*')
  {  if(ControlPtr[FormatPos] == 'c')
     {  for(i=0; i < FormatPos; i++)
        {  SYSTEM_RET(ResInt, isdigit, (ControlPtr[i]))
           if(ResInt)
              WidthArr[0] = WidthArr[0]*10 + ControlPtr[i] - '0';
        }
        if(WidthArr[0] == 0)
           WidthArr[0] = 1;
     }

     if(ControlPtr[FormatPos] == 'n')
        *(Ptr2) = *SymNumberPtr;

     if(ControlPtr[FormatPos]=='s' || ControlPtr[FormatPos]==']')
     {  SYSTEM_RET(WidthArr[0], strlen, (Ptr0))
        WidthArr[0]++;
     }

     #ifndef _DVM_IOPROC_

     if(EnableDynControl)
     {   /* Scalar variable initialization */    /*E0236*/

         switch (VarType)
         {
            case 0 : ( RTL_CALL, dread_((AddrType*)&Ptr1) );
                     break;                                 /* char or
                                                               string */    /*E0237*/
            case 1 : ( RTL_CALL, dread_((AddrType*)&Ptr1) );
                     break;                                 /* short */    /*E0238*/
            case 2 : ( RTL_CALL, dread_((AddrType*)&Ptr2) );
                     break;                                 /* int */    /*E0239*/
            case 3 : ( RTL_CALL, dread_((AddrType*)&Ptr3) );
                     break;                                 /* long */    /*E0240*/
            case 4 : ( RTL_CALL, dread_((AddrType*)&Ptr4) );
                     break;                                 /* float */    /*E0241*/
            case 5 : ( RTL_CALL, dread_((AddrType*)&Ptr5) );
                     break;                                 /* double */    /*E0242*/
         }
     }

     #endif

     switch(VarType)
     { case 0: ( RTL_CALL, rtl_BroadCast(WidthArr, 1, sizeof(int),
                                         DVM_IOProc, NULL) );

               if((IsSynchr && UserSumFlag) || (WidthArr[0] & Msk3))
               {  mac_malloc(_CharPtr, char *, WidthArr[0], 0);

                  if(MPS_CurrentProc == DVM_IOProc)
                     SYSTEM(memcpy, (_CharPtr, Ptr0, WidthArr[0]) )

                  ( RTL_CALL, rtl_BroadCast(_CharPtr, 1, WidthArr[0],
                                            DVM_IOProc, NULL) );

                  if(MPS_CurrentProc != DVM_IOProc)
                     SYSTEM(memcpy, (Ptr0, _CharPtr, WidthArr[0]) )

                  mac_free(&_CharPtr);
               }
               else
                  ( RTL_CALL, rtl_BroadCast(Ptr0, 1, WidthArr[0],
                                            DVM_IOProc, NULL) );

               break;

       case 1: if(MPS_CurrentProc == DVM_IOProc)
                  SYSTEM(memcpy, (wPtr, (char *)Ptr1, sizeof(short)) )

               ( RTL_CALL, rtl_BroadCast(wPtr, 1, sizeof(int),
                                         DVM_IOProc, NULL) );

               if(MPS_CurrentProc != DVM_IOProc)
                  SYSTEM(memcpy, ((char *)Ptr1, wPtr, sizeof(short)) )
               break;

       case 2: if(MPS_CurrentProc == DVM_IOProc)
                  SYSTEM(memcpy, (wPtr, (char *)Ptr2, sizeof(int)) )

               ( RTL_CALL, rtl_BroadCast(wPtr, 1, sizeof(int),
                                         DVM_IOProc, NULL) );

               if(MPS_CurrentProc != DVM_IOProc)
                  SYSTEM(memcpy, ((char *)Ptr2, wPtr, sizeof(int)) )
               break;

       case 3: if(MPS_CurrentProc == DVM_IOProc)
                  SYSTEM(memcpy, (wPtr, (char *)Ptr3, sizeof(DvmType)) )

                  (RTL_CALL, rtl_BroadCast(wPtr, 1, sizeof(DvmType),
                                         DVM_IOProc, NULL) );

               if(MPS_CurrentProc != DVM_IOProc)
                  SYSTEM(memcpy, ((char *)Ptr3, wPtr, sizeof(DvmType)) )
               break;

       case 4: if(MPS_CurrentProc == DVM_IOProc)
                  SYSTEM(memcpy, (wPtr, (char *)Ptr4, sizeof(float)) )

               ( RTL_CALL, rtl_BroadCast(wPtr, 1, sizeof(float),
                                         DVM_IOProc, NULL) );

               if(MPS_CurrentProc != DVM_IOProc)
                  SYSTEM(memcpy, ((char *)Ptr4, wPtr, sizeof(float)) )
               break;

       case 5: if(MPS_CurrentProc == DVM_IOProc)
                  SYSTEM(memcpy, (wPtr, (char *)Ptr5, sizeof(double)) )

               ( RTL_CALL, rtl_BroadCast(wPtr, 1, sizeof(double),
                                         DVM_IOProc, NULL) );

               if(MPS_CurrentProc != DVM_IOProc)
                  SYSTEM(memcpy, ((char *)Ptr5, wPtr, sizeof(double)) )
               break;
     }
  }

  return Res;
}


/* */    /*E0243*/

void  __callstd  dvmfopen_(AddrType  *FileNameAddrPtr,
                           AddrType  *RegAddrPtr,
                           AddrType  *DVMFileAddrPtr)
{
  *DVMFileAddrPtr = (AddrType)dvm_fopen((char *)*FileNameAddrPtr,
                                        (char *)*RegAddrPtr);
  return;
}



void  __callstd  dvmfclose_(AddrType  *DVMFileAddrPtr)
{
  dvm_fclose((DVMFILE *)*DVMFileAddrPtr);
  return;
}



#ifdef _DVM_MPI_


void  __callstd  dvmsync_(AddrType  *DVMFileAddrPtr)
{
  byte            s_RTL_TRACE, s_StatOff;
  DVMFILE        *DVMStream;
  int             i;
  int            *IntPtr;
  char           *Mes;
  FILE          **FilePtrPtr;
  MPS_Request     Req;

  #ifdef _UNIX_
     int    handle;
  #endif

#ifdef _DVM_ZLIB_
  gzFile         *gzFilePtr;
  int            *FlushPtr;
#endif

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvmsync_);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  DVMStream = (DVMFILE *)*DVMFileAddrPtr;

  if(RTL_TRACE)
     dvm_trace(call_dvmsync_, "DVMStream=%lx;\n", (uLLng)DVMStream);

  if(DVMStream == NULL)
  {  /* */    /*E0244*/

     SYSTEM(fflush, (NULL))

     #ifdef _UNIX_
        SYSTEM(sync, ())
     #endif
  }
  else
  {

     if(DVMStream->ParIOType == 0 && DVMStream->LocIOType != 0)
     {  /* */    /*E0245*/

        #ifdef _DVM_ZLIB_
 
        if(DVMStream->zip)
        {
           SYSTEM(gzflush, (DVMStream->zlibFile, DVMStream->flush))

           #ifdef _UNIX_
              SYSTEM(sync, ())
           #endif
        }
        else
  
        #endif
        {
           SYSTEM(fflush, (DVMStream->File))

           #ifdef _UNIX_
              SYSTEM_RET(handle, fileno, (DVMStream->File))
              SYSTEM(fsync, (handle))
           #endif
        } 
     }


     if(DVMStream->ParIOType != 0 && DVMStream->LocIOType != 0)
     {  /* */    /*E0246*/

        /* */    /*E0247*/

        #ifdef _DVM_ZLIB_
 
        if(DVMStream->zip)
           i = sizeof(int) + sizeof(gzFile) + sizeof(int);
        else
  
        #endif
           i = sizeof(int) + sizeof(FILE *);

        mac_malloc(Mes, char *, i, 0);

        IntPtr = (int *)Mes;
        Mes += sizeof(int);

        #ifdef _DVM_ZLIB_
 
        if(DVMStream->zip)
        {
           *IntPtr = 9;
           gzFilePtr = (gzFile *)Mes;
           *gzFilePtr = DVMStream->zlibFile;
           Mes += sizeof(gzFile);
           FlushPtr = (int *)Mes;
           *FlushPtr = DVMStream->flush;
        }
        else
  
        #endif
        {
           *IntPtr = 10;
           FilePtrPtr = (FILE **)Mes;
           *FilePtrPtr = DVMStream->File;
        }

        iom_Sendnowait1(MyIOProc, IntPtr, i, &Req, msg_IOInit);
        iom_Waitrequest1(&Req);

        mac_free((void **)&IntPtr);
     }


     if(DVMStream->ParIOType == 0 && DVMStream->LocIOType == 0)
     {  /* */    /*E0248*/

        if(DVM_IOProc == MPS_CurrentProc)
        {
           #ifdef _DVM_ZLIB_
 
           if(DVMStream->zip)
           {
              SYSTEM(gzflush, (DVMStream->zlibFile, DVMStream->flush))

              #ifdef _UNIX_
                 SYSTEM(sync, ())
              #endif
           }
           else
  
           #endif
           {
              SYSTEM(fflush, (DVMStream->File))

              #ifdef _UNIX_
                 SYSTEM_RET(handle, fileno, (DVMStream->File))
                 SYSTEM(fsync, (handle))
              #endif
           }
        } 
     }


     if(DVMStream->ParIOType != 0 && DVMStream->LocIOType == 0)
     {  /* */    /*E0249*/


     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvmsync_," \n");

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvmsync_);

  (DVM_RET);
  return;
}



void  __callstd  dvmsecread_(AddrType  *DVMFileAddrPtr,
                             DvmType  *ElmTypePtr,
                             DvmType  *RankPtr, DvmType  SizeArray[],
                             DvmType  InitIndexArray[],
                             DvmType  LastIndexArray[],
                             AddrType  *BufAddrPtr, DvmType  *ParPtr,
                             AddrType  *FlagAddrPtr)
{
  byte            s_RTL_TRACE, s_StatOff;
  int             Rank, i, j, k, size, Type;
  int            *IntPtr;
  char           *Buf;
  DVMFILE        *DVMStream;
  FILE          **FilePtrPtr;
  DvmType            InitIndex[MAXARRAYDIM], LastIndex[MAXARRAYDIM],
                  Size[MAXARRAYDIM];
  DvmType            LinSize, InitDelta, Delta, LinCount, coeff, LinInd,
                  Par, Q, QQQ, Remainder;
  char           *Mes, *Flag = NULL, **FlagCharPtr;
  MPS_Request   **FlagSendPtr, **FlagRecvPtr;
  MPS_Request    *ReqSend, *ReqRecv;
  DvmType           *LongPtr;

#ifdef _DVM_ZLIB_
  gzFile         *gzFilePtr;
#endif

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvmsecread_);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  Type      = (int)*ElmTypePtr;
  Rank      = (int)*RankPtr;
  Buf       = (char *)*BufAddrPtr;
  DVMStream = (DVMFILE *)*DVMFileAddrPtr;
  Par       = *ParPtr;

  if(RTL_TRACE)
  {
     dvm_trace(call_dvmsecread_,
               "DVMStream=%lx; Type=%d; Rank=%d; Buf=%lx; Par=%ld;\n",
               (uLLng)DVMStream, Type, Rank, (uLLng)Buf, Par);

     if(TstTraceEvent(call_dvmsecread_))
     {  for(i=0; i < Rank; i++)
            tprintf("     SizeArray[%d]=%ld; ",
                     i, SizeArray[i]);
        tprintf(" \n"); 
        for(i=0; i < Rank; i++)
            tprintf("InitIndexArray[%d]=%ld; ",
                    i, InitIndexArray[i]);
        tprintf(" \n"); 
        for(i=0; i < Rank; i++)
            tprintf("LastIndexArray[%d]=%ld; ",
                    i, LastIndexArray[i]);
        tprintf(" \n"); 
        tprintf(" \n");
     }
  }

  /* */    /*E0250*/

  if(Rank > MAXARRAYDIM)
  {
     if(DVMStream->LocIOType != 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 111.000: wrong call dvmsecread_ "
                "(Array Rank=%d > %d)\n", Rank, (int)MAXARRAYDIM);
     else
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 111.000: wrong call dvmsecread_ "
                 "(Array Rank=%d > %d)\n", Rank, (int)MAXARRAYDIM);
  }

  if(Rank <= 0)
  {
     if(DVMStream->LocIOType != 0)
        eprintf(__FILE__,__LINE__,
                 "*** RTS err 111.001: wrong call dvmsecread_ "
                 "(Array Rank=%d)\n", Rank);
     else
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 111.001: wrong call dvmsecread_ "
                 "(Array Rank=%d)\n", Rank);
  }

  /* */    /*E0251*/

  for(i=0; i < Rank; i++)
      Size[i] = SizeArray[i];

  if(Size[0] < 1)
     Size[0] = DVMTYPE_MAX;

  for(i=0; i < Rank; i++)
  {  if(InitIndexArray[i] < 0)
     {  InitIndex[i] = 0;
        LastIndex[i] = Size[i] - 1;
     }
     else
     {  InitIndex[i] = InitIndexArray[i];
        LastIndex[i] = LastIndexArray[i];
     }
  }

  /* */    /*E0252*/

  for(i=0; i < Rank; i++)
  {  if(Size[i] <= 0)
     {  /* */    /*E0253*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.002: wrong call dvmsecread_ "
                   "(SizeArray[%d]=%ld)\n", i, Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.002: wrong call dvmsecread_ "
                    "(SizeArray[%d]=%ld)\n", i, Size[i]);
     }

     if(InitIndex[i] >= Size[i])
     {  /* */    /*E0254*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.003: wrong call dvmsecread_ "
                   "(InitIndex[%d]=%ld >= %ld)\n",
                   i, InitIndex[i], Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.003: wrong call dvmsecread_ "
                    "(InitIndex[%d]=%ld >= %ld)\n",
                    i, InitIndex[i], Size[i]);
     }

     if(InitIndex[i] < 0)
     {  /* */    /*E0255*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.004: wrong call dvmsecread_ "
                   "(InitIndex[%d]=%ld < 0)\n", i, InitIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.004: wrong call dvmsecread_ "
                    "(InitIndex[%d]=%ld < 0)\n", i, InitIndex[i]);
     }

     if(LastIndex[i] >= Size[i])
     {  /* */    /*E0256*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.005: wrong call dvmsecread_ "
                   "(LastIndex[%d]=%ld >= %ld)\n",
                   i, LastIndex[i], Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.005: wrong call dvmsecread_ "
                    "(LastIndex[%d]=%ld >= %ld)\n",
                    i, LastIndex[i], Size[i]);
     }

     if(LastIndex[i] < 0)
     {  /* */    /*E0257*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.006: wrong call dvmsecread_ "
                   "(LastIndex[%d]=%ld < 0)\n", i, LastIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.006: wrong call dvmsecread_ "
                    "(LastIndex[%d]=%ld < 0)\n", i, LastIndex[i]);
     }


     if(InitIndex[i] > LastIndex[i])
     {  /* */    /*E0258*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.007: wrong call dvmsecread_ "
                   "(InitIndex[%d]=%ld > LastIndex[%d]=%ld)\n",
                   i, InitIndex[i], i, LastIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.007: wrong call dvmsecread_ "
                    "(InitIndex[%d]=%ld > LastIndex[%d]=%ld)\n",
                    i, InitIndex[i], i, LastIndex[i]);
     }
  }

  /* */    /*E0259*/

  for(i=Rank-1,LinSize=1; i >= 0; i--)
  {  LinSize *= LastIndex[i] - InitIndex[i] + 1;

     if(InitIndex[i] > 0 || LastIndex[i] < Size[i] - 1)
        break;
  }

  if(i < 0)
  {  /* */    /*E0260*/

     InitDelta = 0;  /* */    /*E0261*/
     Delta = 0;      /* */    /*E0262*/
     LinCount = 1;   /* */    /*E0263*/
  }
  else
  {  /* */    /*E0264*/

     /* */    /*E0265*/

     i--;
     j = i;

     for(LinCount=1; i >= 0; i--)
         LinCount *= LastIndex[i] - InitIndex[i] + 1;

     /* */    /*E0266*/

     for(i=Rank-1,coeff=1,InitDelta=0; i >= 0; i--)
     {  InitDelta += InitIndex[i] * coeff;
        coeff     *= Size[i];
     }

     /* */    /*E0267*/

     if(LinCount == 1)
        Delta = 0;
     else
     {  for( ; j >= 0; j--)
            if(LastIndex[j] - InitIndex[j] + 1 > 1)
               break;

        InitIndex[j]++;

        for(i=Rank-1,coeff=1,LinInd=0; i >= 0; i--)
        {  LinInd += InitIndex[i] * coeff;
           coeff  *= Size[i];
        }

        Delta = LinInd - InitDelta - LinSize;
        InitIndex[j]--; 
     }
  }

  /* */    /*E0268*/

  switch(Type)
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
     default:  
              if(DVMStream->LocIOType != 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 111.009: wrong call dvmsecread_ "
                         "(invalid type %d)\n", Type);
              else
                 epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                          "*** RTS err 111.009: wrong call dvmsecread_ "
                          "(invalid type %d)\n", Type);
  }

  /* */    /*E0269*/

  Delta *= size;
  InitDelta *= size;
  LinSize *= size;

  if(DVMStream->ParIOType == 0 && DVMStream->LocIOType != 0)
  {  /* */    /*E0270*/

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
     {
        /* */    /*E0271*/

        if(SleepCount == 0)
           SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                  (z_off_t)InitDelta, SEEK_SET))

        for(i=0; i < LinCount; i++)
        {
           #ifdef _UNIX_

           if(SleepCount != 0)
              sleep(SleepCount);
           else

           #endif

             SYSTEM_RET(k, gzread, (DVMStream->zlibFile,
                                    (voidp)Buf, (unsigned)LinSize))
           Buf += LinSize;

           if(Delta != 0 && i < LinCount-1 && SleepCount == 0)
              SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                     (z_off_t)Delta, SEEK_CUR))
        }
     }
     else
  
     #endif
     {
        /* */    /*E0272*/

        if(SleepCount == 0)
           SYSTEM_RET(k, fseek, (DVMStream->File, InitDelta, SEEK_SET))

        for(i=0; i < LinCount; i++)
        {
           #ifdef _UNIX_

           if(SleepCount != 0)
              sleep(SleepCount);
           else

           #endif

              SYSTEM_RET(k, fread, (Buf, 1, LinSize, DVMStream->File))

           Buf += LinSize;

           if(Delta != 0 && i < LinCount-1 && SleepCount == 0)
              SYSTEM_RET(k, fseek, (DVMStream->File, Delta, SEEK_CUR))
        }
     }

     *FlagAddrPtr = (AddrType)Flag;
  }

  if(DVMStream->ParIOType != 0 && DVMStream->LocIOType != 0)
  {  /* */    /*E0273*/

     /* */    /*E0274*/

     if(Par < 1)
        Par = MaxIOMsgSize;     /* */    /*E0275*/
     else
        Par *= size;

     if(Par <= 0)
        Par = INT_MAX;

     Par = dvm_max(Par, MinIOMsgSize);
     Par = (Par/size + 1) * size;
     Par = dvm_min(Par, LinSize);  /* */    /*E0276*/

     /* */    /*E0277*/

     LinInd = LinSize / Par;       /* */    /*E0278*/
     Remainder = LinSize % Par;    /* */    /*E0279*/

     if(Remainder == 0)
        Q = LinInd;       /* */    /*E0280*/
     else
        Q = LinInd + 1; 

     QQQ = Q * LinCount;  /* */    /*E0281*/

     /* */    /*E0282*/

     i = sizeof(int) + 2 * sizeof(MPS_Request *) + sizeof(char *);

     mac_malloc(Flag, char *, i, 0);
     *FlagAddrPtr = (AddrType)Flag;

     IntPtr = (int *)Flag;
     *IntPtr = 0;  /* */    /*E0283*/
     Flag += sizeof(int);
     FlagSendPtr = (MPS_Request **)Flag; 

     /* */    /*E0284*/

     mac_malloc(ReqSend, MPS_Request *, sizeof(MPS_Request), 0);

           *FlagSendPtr = ReqSend;  /* */    /*E0285*/
           Flag += sizeof(MPS_Request *);
           FlagCharPtr = (char **)Flag;

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
         i = sizeof(int)+sizeof(gzFile)+9 * sizeof(DvmType);
     else
  
     #endif
         i = sizeof(int)+sizeof(FILE *)+9 * sizeof(DvmType);

     mac_malloc(Mes, char *, i, 0);

           *FlagCharPtr = Mes;      /* */    /*E0286*/
           Flag += sizeof(char *);
           FlagRecvPtr = (MPS_Request **)Flag; 

     IntPtr = (int *)Mes;
     Mes += sizeof(int);

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
     {
        *IntPtr = 5;
        gzFilePtr = (gzFile *)Mes;
        *gzFilePtr = DVMStream->zlibFile;
        Mes += sizeof(gzFile);
     }
     else
  
     #endif
     {
        *IntPtr = 6;
        FilePtrPtr = (FILE **)Mes;
        *FilePtrPtr = DVMStream->File;
        Mes += sizeof(FILE *);
     }

     LongPtr = (DvmType *)Mes;
     LongPtr[0] = InitDelta;     /* */    /*E0287*/
     LongPtr[1] = LinCount;      /* */    /*E0288*/
     LongPtr[2] = LinSize;       /* */    /*E0289*/
     LongPtr[3] = Delta;         /* */    /*E0290*/
     LongPtr[4] = LinInd;        /* */    /*E0291*/
     LongPtr[5] = Remainder;     /* */    /*E0292*/ 
     LongPtr[6] = Q;             /* */    /*E0293*/
     LongPtr[7] = QQQ;           /* */    /*E0294*/
     LongPtr[8] = Par;           /* */    /*E0295*/

     iom_Sendnowait1(MyIOProc, IntPtr, i, *FlagSendPtr, msg_IOInit);
     
     /* */    /*E0296*/

     j = (int)(QQQ * sizeof(MPS_Request));
     mac_malloc(ReqRecv, MPS_Request *, j, 0);

           *FlagRecvPtr = ReqRecv;  /* */    /*E0297*/

     for(i=0,k=0; i < LinCount; i++)    /* */    /*E0298*/
     {
        for(j=0; j < LinInd; j++)   /* */    /*E0299*/
        {  iom_Recvnowait1(MyIOProc, Buf, (int)Par, &ReqRecv[k],
                           msg_IOProcess);
           Buf += Par;
           k++;
        }

        if(Remainder)               /* */    /*E0300*/
        {  iom_Recvnowait1(MyIOProc, Buf, (int)Remainder, &ReqRecv[k],
                           msg_IOProcess);
           Buf += Remainder;
           k++;
        }
     }
  }


  if(DVMStream->ParIOType == 0 && DVMStream->LocIOType == 0)
  {  /* */    /*E0301*/

     Mes = Buf;

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
     {
        if(DVM_IOProc == MPS_CurrentProc)
        {
           /* */    /*E0302*/

           if(SleepCount == 0)
              SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                     (z_off_t)InitDelta, SEEK_SET))

           for(i=0; i < LinCount; i++)
           {
              #ifdef _UNIX_

              if(SleepCount != 0)
                 sleep(SleepCount);
              else

              #endif

                 SYSTEM_RET(k, gzread, (DVMStream->zlibFile,
                                        (voidp)Buf, (unsigned)LinSize))
              Buf += LinSize;

              if(Delta != 0 && i < LinCount-1 && SleepCount == 0)
                 SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                        (z_off_t)Delta, SEEK_CUR))
           }
        }
     }
     else
  
     #endif
     {
        if(DVM_IOProc == MPS_CurrentProc)
        {
           /* */    /*E0303*/

           if(SleepCount == 0)
              SYSTEM_RET(k, fseek, (DVMStream->File, InitDelta,
                                    SEEK_SET))

           for(i=0; i < LinCount; i++)
           {
              #ifdef _UNIX_

              if(SleepCount != 0)
                 sleep(SleepCount);
              else

              #endif

                 SYSTEM_RET(k, fread, (Buf, 1, LinSize, DVMStream->File))

              Buf += LinSize;

              if(Delta != 0 && i < LinCount-1 && SleepCount == 0)
                 SYSTEM_RET(k, fseek, (DVMStream->File, Delta, SEEK_CUR))
           }
        }
     }

     LinInd = LinCount * LinSize;

     if((IsSynchr && UserSumFlag) || (LinInd & Msk3))
     {  mac_malloc(Buf, char *, LinInd, 0);
 
        if(MPS_CurrentProc == DVM_IOProc)
           SYSTEM(memcpy, (Buf, Mes, LinInd) )
 
        ( RTL_CALL, rtl_BroadCast(Buf, LinCount, LinSize,
                                  DVM_IOProc, NULL) );
 
        if(MPS_CurrentProc != DVM_IOProc)
           SYSTEM(memcpy, (Mes, Buf, LinInd) )
 
        mac_free((void **)&Buf);
     }
     else
        ( RTL_CALL, rtl_BroadCast(Mes, LinCount, LinSize,
                                  DVM_IOProc, NULL) );

     *FlagAddrPtr = (AddrType)Flag;
  }


  if(DVMStream->ParIOType != 0 && DVMStream->LocIOType == 0)
  {  /* */    /*E0304*/

     *FlagAddrPtr = (AddrType)Flag;
  }

 
  if(RTL_TRACE)
     dvm_trace(ret_dvmsecread_,"Flag=%lx;\n", (uLLng)*FlagAddrPtr);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvmsecread_);

  (DVM_RET);
  return;
}



void  __callstd  dvmsecwrite_(AddrType  *DVMFileAddrPtr,
                              DvmType  *ElmTypePtr,
                              DvmType  *RankPtr, DvmType  SizeArray[],
                              DvmType  InitIndexArray[],
                              DvmType  LastIndexArray[],
                              AddrType  *BufAddrPtr, DvmType  *ParPtr,
                              AddrType  *FlagAddrPtr)
{
  byte            s_RTL_TRACE, s_StatOff;
  int             Rank, i, j, k, size, Type;
  int            *IntPtr;
  char           *Buf;
  DVMFILE        *DVMStream;
  FILE          **FilePtrPtr;
  DvmType            InitIndex[MAXARRAYDIM], LastIndex[MAXARRAYDIM],
                  Size[MAXARRAYDIM];
  DvmType            LinSize, InitDelta, Delta, LinCount, coeff, LinInd,
                  Par, Q, QQQ, Remainder;
  char           *Mes, *Flag = NULL, **FlagCharPtr;
  MPS_Request   **FlagSendPtr, **FlagRecvPtr;
  MPS_Request    *ReqSend, *ReqSecSend;
  DvmType           *LongPtr;

#ifdef _DVM_ZLIB_
  gzFile         *gzFilePtr;
#endif

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvmsecwrite_);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  Type      = (int)*ElmTypePtr;
  Rank      = (int)*RankPtr;
  Buf       = (char *)*BufAddrPtr;
  DVMStream = (DVMFILE *)*DVMFileAddrPtr;
  Par       = *ParPtr;

  if(RTL_TRACE)
  {
     dvm_trace(call_dvmsecwrite_,
               "DVMStream=%lx; Type=%d; Rank=%d; Buf=%lx; Par=%ld;\n",
               (uLLng)DVMStream, Type, Rank, (uLLng)Buf, Par);

     if(TstTraceEvent(call_dvmsecwrite_))
     {  for(i=0; i < Rank; i++)
            tprintf("     SizeArray[%d]=%ld; ",
                     i, SizeArray[i]);
        tprintf(" \n"); 
        for(i=0; i < Rank; i++)
            tprintf("InitIndexArray[%d]=%ld; ",
                    i, InitIndexArray[i]);
        tprintf(" \n"); 
        for(i=0; i < Rank; i++)
            tprintf("LastIndexArray[%d]=%ld; ",
                    i, LastIndexArray[i]);
        tprintf(" \n"); 
        tprintf(" \n");
     }
  }

  /* */    /*E0305*/

  if(Rank > MAXARRAYDIM)
  {
     if(DVMStream->LocIOType != 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS err 111.050: wrong call dvmsecwrite_ "
                "(Array Rank=%d > %d)\n", Rank, (int)MAXARRAYDIM);
     else
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 111.050: wrong call dvmsecwrite_ "
                 "(Array Rank=%d > %d)\n", Rank, (int)MAXARRAYDIM);
  }

  if(Rank <= 0)
  {
     if(DVMStream->LocIOType != 0)
        eprintf(__FILE__,__LINE__,
                 "*** RTS err 111.051: wrong call dvmsecwrite_ "
                 "(Array Rank=%d)\n", Rank);
     else
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 111.051: wrong call dvmsecwrite_ "
                 "(Array Rank=%d)\n", Rank);
  }

  /* */    /*E0306*/

  for(i=0; i < Rank; i++)
      Size[i] = SizeArray[i];

  if(Size[0] < 1)
     Size[0] = DVMTYPE_MAX;

  for(i=0; i < Rank; i++)
  {  if(InitIndexArray[i] < 0)
     {  InitIndex[i] = 0;
        LastIndex[i] = Size[i] - 1;
     }
     else
     {  InitIndex[i] = InitIndexArray[i];
        LastIndex[i] = LastIndexArray[i];
     }
  }

  /* */    /*E0307*/

  for(i=0; i < Rank; i++)
  {  if(Size[i] <= 0)
     {  /* */    /*E0308*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.052: wrong call dvmsecwrite_ "
                   "(SizeArray[%d]=%ld)\n", i, Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.052: wrong call dvmsecwrite_ "
                    "(SizeArray[%d]=%ld)\n", i, Size[i]);
     }

     if(InitIndex[i] >= Size[i])
     {  /* */    /*E0309*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.053: wrong call dvmsecwrite_ "
                   "(InitIndex[%d]=%ld >= %ld)\n",
                   i, InitIndex[i], Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.053: wrong call dvmsecwrite_ "
                    "(InitIndex[%d]=%ld >= %ld)\n",
                    i, InitIndex[i], Size[i]);
     }

     if(InitIndex[i] < 0)
     {  /* */    /*E0310*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.054: wrong call dvmsecwrite_ "
                   "(InitIndex[%d]=%ld < 0)\n", i, InitIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.054: wrong call dvmsecwrite_ "
                    "(InitIndex[%d]=%ld < 0)\n", i, InitIndex[i]);
     }

     if(LastIndex[i] >= Size[i])
     {  /* */    /*E0311*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.055: wrong call dvmsecwrite_ "
                   "(LastIndex[%d]=%ld >= %ld)\n",
                   i, LastIndex[i], Size[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.055: wrong call dvmsecwrite_ "
                    "(LastIndex[%d]=%ld >= %ld)\n",
                    i, LastIndex[i], Size[i]);
     }

     if(LastIndex[i] < 0)
     {  /* */    /*E0312*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.056: wrong call dvmsecwrite_ "
                   "(LastIndex[%d]=%ld < 0)\n", i, LastIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.056: wrong call dvmsecwrite_ "
                    "(LastIndex[%d]=%ld < 0)\n", i, LastIndex[i]);
     }


     if(InitIndex[i] > LastIndex[i])
     {  /* */    /*E0313*/

        if(DVMStream->LocIOType != 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 111.057: wrong call dvmsecwrite_ "
                   "(InitIndex[%d]=%ld > LastIndex[%d]=%ld)\n",
                   i, InitIndex[i], i, LastIndex[i]);
        else
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 111.057: wrong call dvmsecwrite_ "
                    "(InitIndex[%d]=%ld > LastIndex[%d]=%ld)\n",
                    i, InitIndex[i], i, LastIndex[i]);
     }
  }

  /* */    /*E0314*/

  for(i=Rank-1,LinSize=1; i >= 0; i--)
  {  LinSize *= LastIndex[i] - InitIndex[i] + 1;

     if(InitIndex[i] > 0 || LastIndex[i] < Size[i] - 1)
        break;
  }

  if(i < 0)
  {  /* */    /*E0315*/

     InitDelta = 0;  /* */    /*E0316*/
     Delta = 0;      /* */    /*E0317*/
     LinCount = 1;   /* */    /*E0318*/
  }
  else
  {  /* */    /*E0319*/

     /* */    /*E0320*/

     i--;
     j = i;

     for(LinCount=1; i >= 0; i--)
         LinCount *= LastIndex[i] - InitIndex[i] + 1;

     /* */    /*E0321*/

     for(i=Rank-1,coeff=1,InitDelta=0; i >= 0; i--)
     {  InitDelta += InitIndex[i] * coeff;
        coeff     *= Size[i];
     }

     /* */    /*E0322*/

     if(LinCount == 1)
        Delta = 0;
     else
     {  for( ; j >= 0; j--)
            if(LastIndex[j] - InitIndex[j] + 1 > 1)
               break;

        InitIndex[j]++;

        for(i=Rank-1,coeff=1,LinInd=0; i >= 0; i--)
        {  LinInd += InitIndex[i] * coeff;
           coeff  *= Size[i];
        }

        Delta = LinInd - InitDelta - LinSize;
        InitIndex[j]--; 
     }
  }

  /* */    /*E0323*/

  switch(Type)
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
     default:
              if(DVMStream->LocIOType != 0)
                 eprintf(__FILE__,__LINE__,
                         "*** RTS err 111.059: wrong call dvmsecwrite_ "
                         "(invalid type %d)\n", Type);
              else
                 epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                          "*** RTS err 111.059: wrong call dvmsecwrite_ "
                          "(invalid type %d)\n", Type);
  }

  /* */    /*E0324*/

  Delta *= size;
  InitDelta *= size;
  LinSize *= size;

  if(DVMStream->ParIOType == 0 && DVMStream->LocIOType != 0)
  {  /* */    /*E0325*/

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
     {
        /* */    /*E0326*/

        SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                               (z_off_t)InitDelta, SEEK_SET))

        for(i=0; i < LinCount; i++)
        {
           SYSTEM_RET(k, gzwrite, (DVMStream->zlibFile, (voidp)Buf,
                                   (unsigned)LinSize))
           Buf += LinSize;

           if(Delta != 0 && i < LinCount-1)
              SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                     (z_off_t)Delta, SEEK_CUR))
        }
     }
     else
  
     #endif
     {
        /* */    /*E0327*/

        SYSTEM_RET(k, fseek, (DVMStream->File, InitDelta, SEEK_SET))

        for(i=0; i < LinCount; i++)
        {  SYSTEM_RET(k, fwrite, (Buf, 1, LinSize, DVMStream->File))

           Buf += LinSize;

           if(Delta != 0 && i < LinCount-1)
              SYSTEM_RET(k, fseek, (DVMStream->File, Delta, SEEK_CUR))
        }
     } 

     *FlagAddrPtr = (AddrType)Flag;
  }


  if(DVMStream->ParIOType != 0 && DVMStream->LocIOType != 0)
  {  /* */    /*E0328*/

     /* */    /*E0329*/

     if(Par < 1)
        Par = MaxIOMsgSize;     /* */    /*E0330*/
     else
        Par *= size;

     if(Par <= 0)
        Par = INT_MAX;

     Par = dvm_max(Par, MinIOMsgSize);
     Par = (Par/size + 1) * size;
     Par = dvm_min(Par, LinSize);  /* */    /*E0331*/

     /* */    /*E0332*/

     LinInd = LinSize / Par;       /* */    /*E0333*/
     Remainder = LinSize % Par;    /* */    /*E0334*/

     if(Remainder == 0)
        Q = LinInd;       /* */    /*E0335*/
     else
        Q = LinInd + 1; 

     QQQ = Q * LinCount;  /* */    /*E0336*/

     /* */    /*E0337*/

     i = sizeof(int) + 2 * sizeof(MPS_Request *) + sizeof(char *);

     mac_malloc(Flag, char *, i, 0);
     *FlagAddrPtr = (AddrType)Flag;

     IntPtr = (int *)Flag;
     *IntPtr = 1;  /* */    /*E0338*/
     Flag += sizeof(int);
     FlagSendPtr = (MPS_Request **)Flag; 

     /* */    /*E0339*/

     mac_malloc(ReqSend, MPS_Request *, sizeof(MPS_Request), 0);

           *FlagSendPtr = ReqSend;  /* */    /*E0340*/
           Flag += sizeof(MPS_Request *);
           FlagCharPtr = (char **)Flag;

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
         i = sizeof(int)+sizeof(gzFile)+9 * sizeof(DvmType);
     else
  
     #endif
         i = sizeof(int)+sizeof(FILE *)+9 * sizeof(DvmType);

     mac_malloc(Mes, char *, i, 0);

           *FlagCharPtr = Mes;      /* */    /*E0341*/
           Flag += sizeof(char *);
           FlagRecvPtr = (MPS_Request **)Flag; 

     IntPtr = (int *)Mes;
     Mes += sizeof(int);

     #ifdef _DVM_ZLIB_
 
     if(DVMStream->zip)
     {
        *IntPtr = 7;
        gzFilePtr = (gzFile *)Mes;
        *gzFilePtr = DVMStream->zlibFile;
        Mes += sizeof(gzFile);
     }
     else
  
     #endif
     {
        *IntPtr = 8;
        FilePtrPtr = (FILE **)Mes;
        *FilePtrPtr = DVMStream->File;
        Mes += sizeof(FILE *);
     }

     LongPtr = (DvmType *)Mes;
     LongPtr[0] = InitDelta;     /* */    /*E0342*/
     LongPtr[1] = LinCount;      /* */    /*E0343*/
     LongPtr[2] = LinSize;       /* */    /*E0344*/
     LongPtr[3] = Delta;         /* */    /*E0345*/
     LongPtr[4] = LinInd;        /* */    /*E0346*/
     LongPtr[5] = Remainder;     /* */    /*E0347*/
     LongPtr[6] = Q;             /* */    /*E0348*/
     LongPtr[7] = QQQ;           /* */    /*E0349*/
     LongPtr[8] = Par;           /* */    /*E0350*/

     iom_Sendnowait1(MyIOProc, IntPtr, i, *FlagSendPtr, msg_IOInit);
     
     /* */    /*E0351*/

     j = (int)(QQQ * sizeof(MPS_Request));
     mac_malloc(ReqSecSend, MPS_Request *, j, 0);

           *FlagRecvPtr = ReqSecSend;  /* */    /*E0352*/

     for(i=0,k=0; i < LinCount; i++)    /* */    /*E0353*/
     {
        for(j=0; j < LinInd; j++)   /* */    /*E0354*/
        {  iom_Sendnowait1(MyIOProc, Buf, (int)Par, &ReqSecSend[k],
                           msg_IOProcess);
           Buf += Par;
           k++;
        }

        if(Remainder)               /* */    /*E0355*/
        {  iom_Sendnowait1(MyIOProc, Buf, (int)Remainder,
                           &ReqSecSend[k], msg_IOProcess);
           Buf += Remainder;
           k++;
        }
     }
  }


  if(DVMStream->ParIOType == 0 && DVMStream->LocIOType == 0)
  {  /* */    /*E0356*/

     if(DVM_IOProc == MPS_CurrentProc)
     {

        #ifdef _DVM_ZLIB_
 
        if(DVMStream->zip)
        {
           /* */    /*E0357*/

           SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                  (z_off_t)InitDelta, SEEK_SET))

           for(i=0; i < LinCount; i++)
           {
              SYSTEM_RET(k, gzwrite, (DVMStream->zlibFile, (voidp)Buf,
                                      (unsigned)LinSize))
              Buf += LinSize;

              if(Delta != 0 && i < LinCount-1)
                 SYSTEM_RET(k, gzseek, (DVMStream->zlibFile,
                                        (z_off_t)Delta, SEEK_CUR))
           }
        }
        else
  
        #endif
        {
           /* */    /*E0358*/

           SYSTEM_RET(k, fseek, (DVMStream->File, InitDelta, SEEK_SET))

           for(i=0; i < LinCount; i++)
           {  SYSTEM_RET(k, fwrite, (Buf, 1, LinSize, DVMStream->File))

              Buf += LinSize;

              if(Delta != 0 && i < LinCount-1)
                 SYSTEM_RET(k, fseek, (DVMStream->File, Delta, SEEK_CUR))
           }
        }
     } 

     *FlagAddrPtr = (AddrType)Flag;
  }


  if(DVMStream->ParIOType != 0 && DVMStream->LocIOType == 0)
  {  /* */    /*E0359*/

     *FlagAddrPtr = (AddrType)Flag;
  }

  if(RTL_TRACE)
     dvm_trace(ret_dvmsecwrite_,"Flag=%lx;\n", (uLLng)*FlagAddrPtr);

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvmsecwrite_);

  (DVM_RET);
  return;
}



void  __callstd  dvmsecwait_(AddrType  *FlagAddrPtr)
{
  byte            s_RTL_TRACE, s_StatOff;
  char           *Flag, *Mes;
  MPS_Request    *ReqPtr;
  char          **CharPtrPtr;
  DvmType           *LongPtr;
  DvmType            QQQ;
  int            *IntPtr;
  int             i;
  
  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  DVMFTimeStart(call_dvmsecwait_);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  Flag = (char *)*FlagAddrPtr;

  if(Flag != NULL)
  {
     IntPtr = (int *)Flag;

     if(RTL_TRACE)
     {
        if(*IntPtr == 0)
           dvm_trace(call_dvmsecwait_, "Read. Flag=%lx;\n", (uLLng)Flag);
        else
           dvm_trace(call_dvmsecwait_, "Write. Flag=%lx;\n", (uLLng)Flag);
     }

     /* */    /*E0360*/

     Flag += sizeof(int);
     ReqPtr = *((MPS_Request **)Flag);
     iom_Waitrequest1(ReqPtr);
     mac_free((void **)&ReqPtr); /* */    /*E0361*/

     /* */    /*E0362*/

     Flag += sizeof(MPS_Request *);
     CharPtrPtr = (char **)Flag;
     Mes = *CharPtrPtr;
     Mes += sizeof(int) + sizeof(FILE *);
     LongPtr = (DvmType *)Mes;
     QQQ = LongPtr[7];  /* */    /*E0363*/

     mac_free((void **)CharPtrPtr); /* */    /*E0364*/
     Flag += sizeof(char *);
     ReqPtr = *((MPS_Request **)Flag);

     for(i=0; i < QQQ; i++)
         iom_Waitrequest1(&ReqPtr[i]);

     mac_free((void **)Flag);  /* */    /*E0365*/

     mac_free((void **)&IntPtr);  /* */    /*E0366*/
  }
  else
  {
     if(RTL_TRACE)
        dvm_trace(call_dvmsecwait_, "Flag=%lx;\n", (uLLng)Flag);
  }

  *FlagAddrPtr = 0;

  if(RTL_TRACE)
     dvm_trace(ret_dvmsecwait_," \n");

  RTL_TRACE = s_RTL_TRACE;

  DVMFTimeFinish(ret_dvmsecwait_);

  (DVM_RET);
  return;
}

#endif


#endif  /* _STDIO_C_ */    /*E0367*/
