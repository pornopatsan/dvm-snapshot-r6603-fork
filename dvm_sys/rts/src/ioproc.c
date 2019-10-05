#ifndef _IOPROC_C_
#define _IOPROC_C_
/****************/    /*E0000*/

/* */    /*E0001*/

#ifdef _DVM_MPI_

#ifndef _MPI_STUBS_
#include <mpi.h>
#else
#include "mpi_stubs.h"
#endif

#ifdef _DVM_IOPROC_

  char        **ApplProcBuf  = NULL;
  MPS_Request  *ApplReqArray = NULL;
  MPI_Status    IOStatus;
  int           CurrentApplProc; /* */    /*E0002*/
  int           ApplMsgLen;      /* */    /*E0003*/
  char         *CurrentApplBuf;  /* */    /*E0004*/
  int           ApplMsgCode;     /* */    /*E0005*/

void  dvm_ioproc(void)
{
  int             i, j, k, m, Index;
  char           *CharPtr = NULL, *Buf1 = NULL, *Buf2 = NULL;
  int             ApplExitCount = 0;
  int            *IntPtr;
  DvmType           *LongPtr;
  MPS_Request     Req;
  FILE           *RCArr[2+sizeof(double)/sizeof(FILE *)];
  int             ResArr[2+sizeof(double)/sizeof(int)] = {EOF};
  FILE           *FilePtr;
  DvmType            LinSize, InitDelta, Delta, LinCount, LinInd, Par,
                  Q, QQQ, Remainder, shift, Buf1Size = 0, Buf2Size = 0;
  double          tm;

#ifdef _DVM_ZLIB_
  gzFile         gzRCArr[2+sizeof(double)/sizeof(gzFile)];
  gzFile         zlibFile;
#endif

  /* */    /*E0006*/

  mac_malloc(ApplProcBuf, char **, sizeof(char *)*ApplProcessCount, 0);

  for(i=0; i < ApplProcessCount; i++)
  {  mac_malloc(ApplProcBuf[i], char *, MaxApplMesSize, 0);
  }

  mac_malloc(ApplReqArray, MPS_Request *,
             sizeof(MPS_Request)*ApplProcessCount, 0);

  if(RTL_TRACE)
     tprintf("*** Start of input/output process ***\n");

  /* */    /*E0007*/

/* ?????????
  for(i=0; i < ApplProcessCount; i++)
      iom_Recvnowait1(i, (void *)ApplProcBuf[i], MaxApplMesSize,
                      &ApplReqArray[i], msg_IOInit);
????????? */    /*E0008*/

  /* */    /*E0009*/

waitm: ;

/* ?????????
  iom_Waitany1(ApplProcessCount, ApplReqArray, &Index, &IOStatus);
????????? */    /*E0010*/

  iom_Recvany1((void *)ApplProcBuf[0], MaxApplMesSize, msg_IOInit,
               &IOStatus);
    
  Index = IOStatus.MPI_SOURCE;   /* */    /*E0011*/

  if(IOProcessSign[Index])
     goto  waitm;  /* */    /*E0012*/

  CurrentApplProc = ApplIOProcessNumber[Index]; /* */    /*E0013*/

/* ---------
  iom_Recvnowait1(MyApplProc, (void *)ApplProcBuf[0],
                  MaxApplMesSize, &Req, msg_IOInit);
  iom_Waitrequest1(&Req);
  IOStatus = GMPI_Status;*/    /*E0014*/

/* ---------
  iom_Recv1(MyApplProc, (void *)ApplProcBuf[0],
            MaxApplMesSize, msg_IOInit, &IOStatus);*/    /*E0015*/

  CurrentApplProc = MyApplProc; /* */    /*E0016*/

  for(i=0; i < MaxApplMesSize; i++)
      ApplProcBuf[CurrentApplProc][i] = ApplProcBuf[0][i];

  /* */    /*E0017*/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0018*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0019*/

  for(j=0,ApplMsgLen=0; j < 10; j++)
  {  MPI_Get_count(&IOStatus, MPI_BYTE, &ApplMsgLen);

     if(ApplMsgLen != 0)
        break;
  }

  if(j == 10)
     eprintf(__FILE__,__LINE__, "*** RTS fatal err: "
                                "MPI_Get_count rc = %d\n", ApplMsgLen);

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0020*/
if(RTL_TRACE)
   tprintf("!!! MPI_GetCount time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0021*/

  CurrentApplBuf = ApplProcBuf[CurrentApplProc];   /* */    /*E0022*/
  IntPtr = (int *)CurrentApplBuf;
  ApplMsgCode = *IntPtr;

  if(RTL_TRACE)
     tprintf("*** Control message:\n"
             "CurrentApplProc=%d ApplMsgLen=%d ApplMsgCode=%d\n",
             CurrentApplProc, ApplMsgLen, ApplMsgCode);

  switch(ApplMsgCode)
  {  case   0:  /* */    /*E0023*/

                if(RTL_TRACE)
                   tprintf("*** Finish of applied process %d ***\n",
                           CurrentApplProc);
                ApplExitCount++;

                /* */    /*E0024*/

                if(CurrentApplProc == MyApplProc)
                {  ServerProcTime = dvm_time() - IOProcTime;

                   iom_Sendnowait1(MyApplProc, (void *)&ServerProcTime,
                                   sizeof(double), &Req, msg_IOInit);
                   iom_Waitrequest1(&Req);
                }

                break;

     case   1:  /* */    /*E0025*/

                #ifdef _DVM_ZLIB_

                CurrentApplBuf += 4*sizeof(int);
                SYSTEM_RET(j, strlen, (CurrentApplBuf))
                CharPtr = CurrentApplBuf + j + 1;

                SYSTEM_RET(gzRCArr[0], gzopen, (CurrentApplBuf, CharPtr))

                if(gzRCArr[0] == NULL && IntPtr[2] == 0 &&
                   IntPtr[3] != 0)
                {  /* */    /*E0026*/

                   CurrentApplBuf[IntPtr[1]] = '\x00';
                   SYSTEM_RET(gzRCArr[0], gzopen, (CurrentApplBuf,
                                                   CharPtr))
                }

                if(RTL_TRACE)
                   tprintf("*** gzopen: file=%s type=%s gzFile=%lx\n",
                           CurrentApplBuf, CharPtr, (uLLng)gzRCArr[0]);

                /* */    /*E0027*/
                      
                iom_Sendnowait1(CurrentApplProc, gzRCArr, sizeof(gzFile),
                                &Req, msg_IOProcess);
                iom_Waitrequest1(&Req);

                /* */    /*E0028*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0029*/

                #endif

                break;

     case   2:  /* */    /*E0030*/

                CurrentApplBuf += sizeof(int);
                SYSTEM_RET(j, strlen, (CurrentApplBuf))
                CharPtr = CurrentApplBuf + j + 1;

                SYSTEM_RET(RCArr[0], fopen, (CurrentApplBuf, CharPtr))

                if(RTL_TRACE)
                   tprintf("*** fopen: file=%s type=%s, (FILE *)=%lx\n",
                           CurrentApplBuf, CharPtr, (uLLng)RCArr[0]);

                /* */    /*E0031*/
                      
                iom_Sendnowait1(CurrentApplProc, RCArr, sizeof(FILE *),
                                &Req, msg_IOProcess);
                iom_Waitrequest1(&Req);

                /* */    /*E0032*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0033*/
                break;

     case   3:  /* */    /*E0034*/

                #ifdef _DVM_ZLIB_

                CurrentApplBuf += sizeof(int);
                zlibFile = *((gzFile *)CurrentApplBuf);
                SYSTEM_RET(ResArr[0], gzclose, (zlibFile))
                ResArr[0] = 0;  /* */    /*E0035*/

                if(RTL_TRACE)
                   tprintf("*** gzclose: gzFile=%lx\n", zlibFile);

                /* */    /*E0036*/
                      
                iom_Sendnowait1(CurrentApplProc, ResArr, sizeof(int),
                                &Req, msg_IOProcess);
                iom_Waitrequest1(&Req);

                /* */    /*E0037*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0038*/

                #endif

                break; 

     case   4:  /* */    /*E0039*/

                CurrentApplBuf += sizeof(int);
                FilePtr = *((FILE **)CurrentApplBuf);

                SYSTEM_RET(ResArr[0], fclose, (FilePtr))

                if(RTL_TRACE)
                   tprintf("*** fclose: (File *)=%lx\n", FilePtr);

                /* */    /*E0040*/
                      
                iom_Sendnowait1(CurrentApplProc, ResArr, sizeof(int),
                                &Req, msg_IOProcess);
                iom_Waitrequest1(&Req);

                /* */    /*E0041*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0042*/

                break;

     case    5: /* */    /*E0043*/

     case    6: /* */    /*E0044*/

                CurrentApplBuf += sizeof(int);

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 5)
                {
                   zlibFile = *((gzFile *)CurrentApplBuf);
                   CurrentApplBuf += sizeof(gzFile);
                }
                else

                #endif
                {
                   FilePtr = *((FILE **)CurrentApplBuf);
                   CurrentApplBuf += sizeof(FILE *);
                }

                LongPtr = (DvmType *)CurrentApplBuf;

                InitDelta = LongPtr[0];  /* */    /*E0045*/
                LinCount  = LongPtr[1];  /* */    /*E0046*/
                LinSize   = LongPtr[2];  /* */    /*E0047*/
                Delta     = LongPtr[3];  /* */    /*E0048*/
                LinInd    = LongPtr[4];  /* */    /*E0049*/
                Remainder = LongPtr[5];  /* */    /*E0050*/ 
                Q         = LongPtr[6];  /* */    /*E0051*/
                QQQ       = LongPtr[7];  /* */    /*E0052*/
                Par       = LongPtr[8];  /* */    /*E0053*/

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 5)
                {
                   if(RTL_TRACE)
                      tprintf("*** secread: (gzFile)=%lx InitDelta=%ld "
                              "LinCount=%ld LinSize=%ld Delta=%ld\n"
                              "LinInd=%ld Remainder=%ld Q=%ld QQQ=%ld "
                              "Par=%ld\n", (uLLng)zlibFile, InitDelta,
                              LinCount, LinSize, Delta, LinInd,
                              Remainder, Q, QQQ, Par);
                }
                else

                #endif
                {
                   if(RTL_TRACE)
                      tprintf("*** secread: (File *)=%lx InitDelta=%ld "
                              "LinCount=%ld LinSize=%ld Delta=%ld\n"
                              "LinInd=%ld Remainder=%ld Q=%ld QQQ=%ld "
                              "Par=%ld\n", (uLLng)FilePtr, InitDelta,
                              LinCount, LinSize, Delta, LinInd,
                              Remainder, Q, QQQ, Par);
                }

                if(QQQ == 1)
                {  /* */    /*E0054*/

                   /* */    /*E0055*/

                   if(Buf1 == NULL || Buf1Size < Par)
                   {  if(Buf1 != NULL)
                      {  mac_free((void **)&Buf1);
                      }

                      mac_malloc(Buf1, char *, Par, 0);
                      Buf1Size = Par;
                   }

                   /* */    /*E0056*/

                   #ifdef _DVM_ZLIB_

                   if(ApplMsgCode == 5)
                   {
                      if(SleepCount == 0)
                         SYSTEM_RET(k, gzseek, (zlibFile,
                                                (z_off_t)InitDelta,
                                                SEEK_SET))
                      
                      #ifdef _UNIX_

                      if(SleepCount != 0)
                         sleep(SleepCount);
                      else

                      #endif

                         SYSTEM_RET(k, gzread, (zlibFile, (voidp)Buf1,
                                                (unsigned)Par))
                   }
                   else
   
                   #endif
                   {
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0057*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0058*/

                      if(SleepCount == 0)
                         SYSTEM_RET(k, fseek, (FilePtr, InitDelta,
                                               SEEK_SET))
                      #ifdef _UNIX_

                      if(SleepCount != 0)
                         sleep(SleepCount);
                      else

                      #endif

                         SYSTEM_RET(k, fread, (Buf1, 1, Par, FilePtr))
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0059*/
if(RTL_TRACE)
   tprintf("!!! fseek,fread time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0060*/

                   }

                   /* */    /*E0061*/

                   iom_Sendnowait1(CurrentApplProc, Buf1, Par, &Req,
                                   msg_IOProcess);
                   iom_Waitrequest1(&Req);

                   /* */    /*E0062*/

                   if(FreeIOBuf)
                   {  mac_free((void **)&Buf1);
                      Buf1Size = 0;
                   }
                }
                else
                {  /* */    /*E0063*/

                   /* */    /*E0064*/

                   if(Buf1 == NULL || Buf1Size < Par)
                   {  if(Buf1 != NULL)
                      {  mac_free((void **)&Buf1);
                      }

                      mac_malloc(Buf1, char *, Par, 0);
                      Buf1Size = Par;
                   }

                   if(Buf2 == NULL || Buf2Size < Par)
                   {  if(Buf2 != NULL)
                      {  mac_free((void **)&Buf2);
                      }

                      mac_malloc(Buf2, char *, Par, 0);
                      Buf2Size = Par;
                   }

                   /* */    /*E0065*/

                   #ifdef _DVM_ZLIB_

                   if(ApplMsgCode == 5)
                   {
                      if(SleepCount == 0) 
                         SYSTEM_RET(k, gzseek, (zlibFile,
                                                (z_off_t)InitDelta,
                                                SEEK_SET))
                   }
                   else
   
                   #endif
                   {
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0066*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0067*/

                      if(SleepCount == 0)
                         SYSTEM_RET(k, fseek, (FilePtr, InitDelta,
                                               SEEK_SET))

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0068*/
if(RTL_TRACE)
   tprintf("!!! init fseek time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0069*/
                   }

                   m = 0;  /* */    /*E0070*/

                   for(i=0; i < LinCount; i++)    /* */    /*E0071*/
                   {
                      for(j=0; j < LinInd; j++)   /* */    /*E0072*/
                      {
                         #ifdef _DVM_ZLIB_

                         if(ApplMsgCode == 5)
                         {
                            #ifdef _UNIX_

                            if(SleepCount != 0)
                               sleep(SleepCount);
                            else

                            #endif
                               SYSTEM_RET(k, gzread, (zlibFile,
                                                      (voidp)Buf1,
                                                      (unsigned)Par))
                         
                         }
                         else
   
                         #endif
                         {
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0073*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0074*/
                            #ifdef _UNIX_

                            if(SleepCount != 0)
                               sleep(SleepCount);
                            else

                            #endif
                               SYSTEM_RET(k, fread, (Buf1, 1, Par,
                                                     FilePtr))
                         
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0075*/
if(RTL_TRACE)
   tprintf("!!! fread time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0076*/
                         }

                         if(m != 0)
                            iom_Waitrequest1(&Req); /* */    /*E0077*/
                         m = 1;

                         /* */    /*E0078*/

                         CharPtr = Buf1;
                         Buf1    = Buf2;
                         Buf2    = CharPtr;

                         iom_Sendnowait1(CurrentApplProc, Buf2, Par,
                                         &Req, msg_IOProcess);
                      }

                      if(Remainder)
                      {
                         #ifdef _DVM_ZLIB_

                         if(ApplMsgCode == 5)
                         {
                            #ifdef _UNIX_

                            if(SleepCount != 0)
                               sleep(SleepCount);
                            else

                            #endif
                               SYSTEM_RET(k, gzread,
                                          (zlibFile, (voidp)Buf1,
                                          (unsigned)Remainder))
                         
                         }
                         else
   
                         #endif
                         {
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0079*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0080*/
                            #ifdef _UNIX_

                            if(SleepCount != 0)
                               sleep(SleepCount);
                            else

                            #endif
                               SYSTEM_RET(k, fread, (Buf1, 1, Remainder,
                                                     FilePtr))

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0081*/
if(RTL_TRACE)
   tprintf("!!! fread time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0082*/
                         }

                         if(m != 0)
                            iom_Waitrequest1(&Req); /* */    /*E0083*/
                         m = 1;

                         /* */    /*E0084*/

                         CharPtr = Buf1;
                         Buf1    = Buf2;
                         Buf2    = CharPtr;

                         iom_Sendnowait1(CurrentApplProc, Buf2,
                                         Remainder, &Req, msg_IOProcess);
                      }

                      /* */    /*E0085*/

                      if(Delta != 0 && i < LinCount-1)
                      { 
                         #ifdef _DVM_ZLIB_

                         if(ApplMsgCode == 5)
                         {
                            if(SleepCount == 0)
                               SYSTEM_RET(k, gzseek, (zlibFile,
                                                      (z_off_t)Delta,
                                                      SEEK_CUR))
                         }
                         else
   
                         #endif
                         {
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0086*/
if(RTL_TRACE)
   tm = dvm_time();
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0087*/

                            if(SleepCount == 0)
                               SYSTEM_RET(k, fseek, (FilePtr, Delta,
                                                     SEEK_CUR))

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0088*/
if(RTL_TRACE)
   tprintf("!!! fseek time = %lf\n", dvm_time() - tm);
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0089*/
                         }
                      }
                   }

                   iom_Waitrequest1(&Req); /* */    /*E0090*/

                   /* */    /*E0091*/

                   if(FreeIOBuf)
                   {  mac_free((void **)&Buf1);
                      mac_free((void **)&Buf2);
                      Buf1Size = 0;
                      Buf2Size = 0;
                   }
                }

                /* */    /*E0092*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0093*/

                break;

     case    7: /* */    /*E0094*/

     case    8: /* */    /*E0095*/

                CurrentApplBuf += sizeof(int);

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 7)
                {
                   zlibFile = *((gzFile *)CurrentApplBuf);
                   CurrentApplBuf += sizeof(gzFile);
                }
                else

                #endif
                {
                   FilePtr = *((FILE **)CurrentApplBuf);
                   CurrentApplBuf += sizeof(FILE *);
                }

                LongPtr = (DvmType *)CurrentApplBuf;

                InitDelta = LongPtr[0];  /* */    /*E0096*/
                LinCount  = LongPtr[1];  /* */    /*E0097*/
                LinSize   = LongPtr[2];  /* */    /*E0098*/
                Delta     = LongPtr[3];  /* */    /*E0099*/
                LinInd    = LongPtr[4];  /* */    /*E0100*/
                Remainder = LongPtr[5];  /* */    /*E0101*/
                Q         = LongPtr[6];  /* */    /*E0102*/
                QQQ       = LongPtr[7];  /* */    /*E0103*/
                Par       = LongPtr[8];  /* */    /*E0104*/

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 7)
                {
                   if(RTL_TRACE)
                      tprintf("*** secwrite: (gzFile)=%lx InitDelta=%ld "
                              "LinCount=%ld LinSize=%ld Delta=%ld\n"
                              "LinInd=%ld Remainder=%ld Q=%ld QQQ=%ld "
                              "Par=%ld\n", (uLLng)zlibFile, InitDelta,
                              LinCount, LinSize, Delta, LinInd,
                              Remainder, Q, QQQ, Par);
                }
                else

                #endif
                {
                   if(RTL_TRACE)
                      tprintf("*** secwrite: (File *)=%lx InitDelta=%ld "
                              "LinCount=%ld LinSize=%ld Delta=%ld\n"
                              "LinInd=%ld Remainder=%ld Q=%ld QQQ=%ld "
                              "Par=%ld\n", (uLLng)FilePtr, InitDelta,
                              LinCount, LinSize, Delta, LinInd,
                              Remainder, Q, QQQ, Par);
                }

                if(QQQ == 1)
                {  /* */    /*E0105*/

                   /* */    /*E0106*/

                   if(Buf1 == NULL || Buf1Size < Par)
                   {  if(Buf1 != NULL)
                      {  mac_free((void **)&Buf1);
                      }

                      mac_malloc(Buf1, char *, Par, 0);
                      Buf1Size = Par;
                   }

                   /* */    /*E0107*/

                   iom_Recvnowait1(CurrentApplProc, Buf1, Par, &Req,
                                   msg_IOProcess);
                   iom_Waitrequest1(&Req);

                   /* */    /*E0108*/

                   #ifdef _DVM_ZLIB_

                   if(ApplMsgCode == 7)
                   {
                      SYSTEM_RET(k, gzseek, (zlibFile,
                                             (z_off_t)InitDelta,
                                             SEEK_SET))
                      SYSTEM_RET(k, gzwrite, (zlibFile, (voidp)Buf1,
                                              (unsigned)Par))
                   }
                   else
   
                   #endif
                   {
                      SYSTEM_RET(k, fseek, (FilePtr, InitDelta,
                                 SEEK_SET))
                      SYSTEM_RET(k, fwrite, (Buf1, 1, Par, FilePtr))
                   }

                   /* */    /*E0109*/

                   if(FreeIOBuf)
                   {  mac_free((void **)&Buf1);
                      Buf1Size = 0;
                   }
                }
                else
                {  /* */    /*E0110*/

                   /* */    /*E0111*/

                   if(Buf1 == NULL || Buf1Size < Par)
                   {  if(Buf1 != NULL)
                      {  mac_free((void **)&Buf1);
                      }

                      mac_malloc(Buf1, char *, Par, 0);
                      Buf1Size = Par;
                   }

                   if(Buf2 == NULL || Buf2Size < Par)
                   {  if(Buf2 != NULL)
                      {  mac_free((void **)&Buf2);
                      }

                      mac_malloc(Buf2, char *, Par, 0);
                      Buf2Size = Par;
                   }

                   /* */    /*E0112*/

                   #ifdef _DVM_ZLIB_

                   if(ApplMsgCode == 7)
                   {
                      SYSTEM_RET(k, gzseek, (zlibFile,
                                             (z_off_t)InitDelta,
                                             SEEK_SET))
                   }
                   else
   
                   #endif
                   {
                      SYSTEM_RET(k, fseek, (FilePtr, InitDelta,
                                            SEEK_SET))
                   }

                   m = 0;  /* */    /*E0113*/

                   for(i=0; i < LinCount; i++)    /* */    /*E0114*/
                   {
                      for(j=0; j < LinInd; j++)   /* */    /*E0115*/
                      {
                         iom_Recvnowait1(CurrentApplProc, Buf2, Par,
                                         &Req, msg_IOProcess);
                         if(m != 0)
                         {
                            #ifdef _DVM_ZLIB_

                            if(ApplMsgCode == 7)
                            {
                               SYSTEM_RET(k, gzwrite, (zlibFile,
                                                       (voidp)Buf1,
                                                       (unsigned)m))
                               if(shift != 0)
                                  SYSTEM_RET(k, gzseek, (zlibFile,
                                                         (z_off_t)shift,
                                                         SEEK_CUR))
                            }
                            else
   
                            #endif
                            {
                               SYSTEM_RET(k, fwrite, (Buf1, 1, m,
                                                      FilePtr))
                               if(shift != 0)
                                  SYSTEM_RET(k, fseek, (FilePtr, shift,
                                                        SEEK_CUR))
                            }
                         }

                         iom_Waitrequest1(&Req);

                         /* */    /*E0116*/

                         CharPtr = Buf1;
                         Buf1    = Buf2;
                         Buf2    = CharPtr;

                         m = (int)Par;       /* */    /*E0117*/
                         shift = 0;          /* */    /*E0118*/
                      }

                      if(Remainder)
                      {
                         iom_Recvnowait1(CurrentApplProc, Buf2,
                                         Remainder, &Req, msg_IOProcess);
                         if(m != 0)
                         {
                            #ifdef _DVM_ZLIB_

                            if(ApplMsgCode == 7)
                            {
                               SYSTEM_RET(k, gzwrite, (zlibFile,
                                                       (voidp)Buf1,
                                                       (unsigned)m))
                               if(shift != 0)
                                  SYSTEM_RET(k, gzseek, (zlibFile,
                                                         (z_off_t)shift,
                                                         SEEK_CUR))
                            }
                            else
   
                            #endif
                            {
                               SYSTEM_RET(k, fwrite, (Buf1, 1, m,
                                                      FilePtr))
                               if(shift != 0)
                                  SYSTEM_RET(k, fseek, (FilePtr, shift,
                                                        SEEK_CUR))
                            }
                         }

                         iom_Waitrequest1(&Req);

                         /* */    /*E0119*/

                         CharPtr = Buf1;
                         Buf1    = Buf2;
                         Buf2    = CharPtr;

                         m = (int)Remainder; /* */    /*E0120*/
                         shift = 0;          /* */    /*E0121*/
                      }

                      /* */    /*E0122*/

                      shift = Delta;    /* */    /*E0123*/
                   }

                   /* */    /*E0124*/

                   #ifdef _DVM_ZLIB_

                   if(ApplMsgCode == 7)
                   {
                      SYSTEM_RET(k, gzwrite, (zlibFile, (voidp)Buf1,
                                              (unsigned)m))
                   }
                   else
   
                   #endif
                   {
                      SYSTEM_RET(k, fwrite, (Buf1, 1, m, FilePtr))
                   }

                   /* */    /*E0125*/

                   if(FreeIOBuf)
                   {  mac_free((void **)&Buf1);
                      mac_free((void **)&Buf2);
                      Buf1Size = 0;
                      Buf2Size = 0;
                   }
                }

                /* */    /*E0126*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0127*/

                break;

     case    9: /* */    /*E0128*/

     case   10: /* */    /*E0129*/

                CurrentApplBuf += sizeof(int);

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 9)
                {
                   zlibFile = *((gzFile *)CurrentApplBuf);
                   CurrentApplBuf += sizeof(gzFile);
                   IntPtr = (int *)CurrentApplBuf;
                }
                else

                #endif
                {
                   FilePtr = *((FILE **)CurrentApplBuf);
                   CurrentApplBuf += sizeof(FILE *);
                }

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 9)
                {
                   if(RTL_TRACE)
                      tprintf("*** secsync: (gzFile)=%lx\n",
                              (uLLng)zlibFile);
                }
                else

                #endif
                {
                   if(RTL_TRACE)
                      tprintf("*** secsync: (File *)=%lx\n",
                              (uLLng)FilePtr);
                }

                #ifdef _DVM_ZLIB_

                if(ApplMsgCode == 9)
                {
                   SYSTEM(gzflush, (zlibFile, *IntPtr))

                   #ifdef _UNIX_
                      SYSTEM(sync, ())
                   #endif
                }
                else
   
                #endif
                {
                   SYSTEM(fflush, (FilePtr))

                   #ifdef _UNIX_
                      SYSTEM_RET(i, fileno, (FilePtr))
                      SYSTEM(fsync, (i))
                   #endif
                }

                /* */    /*E0130*/

/* ?????????
                iom_Recvnowait1(CurrentApplProc,
                                (void *)ApplProcBuf[CurrentApplProc],
                                MaxApplMesSize,
                                &ApplReqArray[CurrentApplProc],
                                msg_IOInit);
*/    /*E0131*/

                break;
     default:
                break;
  }  

  if(ApplExitCount != ApplProcessCount)
     goto  waitm;  /* */    /*E0132*/

  /* */    /*E0133*/

  /* */    /*E0134*/

  for(i=0; i < ApplProcessCount; i++)
  {  mac_free(((void **)&ApplProcBuf[i]));
  }

  mac_free((void **)&ApplProcBuf);
  mac_free((void **)&ApplReqArray);

  if(RTL_TRACE)
     tprintf("*** End of input/output process ***\n"
             "IOProcTime=%lf T_Recv1=%lf T_Recvany1=%lf\n"
             "T_Sendnowait1=%lf T_Recvnowait1=%lf "
             "T_Waitrequest1=%lf T_Waitany1=%lf\n",
             dvm_time()-IOProcTime, T_iom_Recv1, T_iom_Recvany1,
             T_iom_Sendnowait1, T_iom_Recvnowait1, T_iom_Waitrequest1,
             T_iom_Waitany1);

   RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

   if(1 /*CallDbgCond*/    /*E0135*/ /*EnableTrace && dvm_OneProcSign*/    /*E0136*/)
      SYSTEM(MPI_Finalize, ())
   else
     dvm_exit(0);

#else

   dvm_exit(0);

#endif

   SYSTEM(exit, (0))
}

#endif

#endif


/* */    /*E0137*/

#ifdef _DVM_MPI_

void  ioprocess_set(int  argc, char  *argv[])
{ int      i, j, n;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* */    /*E0138*/

  n = argc - 1;

  for(; i < n; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "resetiop", 8))

     if(j == 0)
     {  /* */    /*E0139*/

        IOProcess = 0;
     }

     SYSTEM_RET(j, strncmp, (&argv[i][1], "setiop", 6))
     if(j == 0)
     {  /* */    /*E0140*/

        IOProcess = 1;
     }
  }

  return;
}
 
#endif


/* */    /*E0141*/

#ifdef _DVM_MPI_

int  io_Sendnowait(void *buf, int count, int size, int procnum,
                   RTL_Request *RTL_ReqPtr)
{
  int           rc, MsgLen, MsgNumber, Remainder, i, MsgLength;
  char         *CharPtr = (char *)buf;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;
  void         *MsgBuf;
  byte          Part = 1;

  char         *CharPtr0, *CharPtr1, *CharPtr2, *CharPtr3, *CharPtr4;
  int           j, k, m, n, CompressLen;
  byte          IsCompress = 0;

#if defined(_DVM_ZLIB_)

  Bytef   *gzBuf;
  uLongf   gzLen;
  int      MsgCompressLev;

#endif

  DVMMTimeStart(call_io_Sendnowait);

  if(RTL_TRACE)
     dvm_trace(call_io_Sendnowait,
               "buf=%lx; count=%d; size=%d; req=%lx; procnum=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum);

  NoWaitCount++;  /* */    /*E0142*/

  rc = count * size;

  MsgLen = rc + SizeDelta[rc & Msk3];

  RTL_ReqPtr->SendRecvTime = -1.; /* */    /*E0143*/

  if(MPIInfoPrint && StatOff == 0)
  {  if(UserSumFlag != 0)
        MPISendByteNumber  += MsgLen;
     else
        MPISendByteNumber0 += MsgLen;
  }

  /* */    /*E0144*/

  RTL_ReqPtr->CompressBuf = NULL;  /* */    /*E0145*/
  RTL_ReqPtr->ResCompressIndex = -1; 
  MsgBuf = buf;

  CompressLen = MsgLen;   /* */    /*E0146*/

  if(MsgDVMCompress != 0 && MsgLen > MinMsgCompressLength &&
     UserSumFlag != 0 && (rc % sizeof(double)) == 0)
  {  /* */    /*E0147*/

     /* */    /*E0148*/

     if(!( ( MsgSchedule && MsgLen > MaxMsgLength ) ||
           ( MsgPartReg > 1 &&
             MaxMsgLength > 0 && MsgLen > MaxMsgLength ) ) ||
             MsgCompressWithMsgPart != 0)
     {
        j = MsgLen / sizeof(double);  /* */    /*E0149*/

        k = 1;
        CompressLen = 1 + sizeof(double);

        if(MsgDVMCompress < 2)
        {  /* */    /*E0150*/

           n = sizeof(double) - 1;

           if(InversByteOrder)
              CharPtr1 = (char *)buf + 1;
           else
              CharPtr1 = (char *)buf;
        }
        else
        {  /* */    /*E0151*/

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
           {  /* */    /*E0152*/

              if(MsgDVMCompress > 1)
                 CompressLen += 2;
              else
                 CompressLen++;

              CharPtr1 += sizeof(double);
              k++;
           }
           else
           {  /* */    /*E0153*/

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
  {  /* */    /*E0154*/

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( MsgPartReg > 1 &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength ))
     {
        /* */    /*E0155*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0156*/

           if(IsCompress)
           {
              /* */    /*E0157*/

              j = MsgLen / sizeof(double);  /* */    /*E0158*/

              RTL_ReqPtr->ResCompressIndex =
              compress_malloc(&MsgBuf, CompressLen);

              MPIMsgNumber++;

              CharPtr2 = (char *)MsgBuf;
              CharPtr3 = CharPtr2 + 1;
              CharPtr4 = CharPtr3 + sizeof(double);
              k = 1;

              if(MsgDVMCompress < 2)
              {  /* */    /*E0159*/

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
              {  /* */    /*E0160*/

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
                 {  /* */    /*E0161*/

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
                 {  /* */    /*E0162*/

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
           {  /* */    /*E0163*/

              /* */    /*E0164*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0165*/

              i = iom_Sendnowait(procnum, (void *)&MsgLen,
                                 sizeof(int), RTL_ReqPtr, msg_IOProcess);

              /* */    /*E0166*/

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
                         "*** RTS err 210.002: iom_Sendnowait rc = %d\n",
                         i);

              iom_Waitrequest(RTL_ReqPtr);
           }
           else
              Part = 0; /* */    /*E0167*/

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0168*/
           if(IsCompress)
              RTL_ReqPtr->CompressBuf = (char *)MsgBuf; 
        }
     }
     else
     {  /* */    /*E0169*/

        if(IsCompress)
        {
           /* */    /*E0170*/

           j = MsgLen / sizeof(double);  /* */    /*E0171*/

           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, CompressLen);

           MPIMsgNumber++;

           CharPtr2 = (char *)MsgBuf;
           CharPtr3 = CharPtr2 + 1;
           CharPtr4 = CharPtr3 + sizeof(double);
           k = 1;

           if(MsgDVMCompress < 2)
           {  /* */    /*E0172*/

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
           {  /* */    /*E0173*/

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
              {  /* */    /*E0174*/

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
              {  /* */    /*E0175*/

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
        RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0176*/
        if(IsCompress)
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #if defined(_DVM_ZLIB_)

  if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
     (MsgDVMCompress == 0 || (rc % sizeof(double)) != 0) &&
     MsgLen > MinMsgCompressLength && UserSumFlag != 0)
  {  /* */    /*E0177*/

     MsgCompressLev = MsgCompressLevel;  /* */    /*E0178*/

     if(MsgCompressLev == 0)
     {  /* */    /*E0179*/

        if(CompressLevel == 0)
           MsgCompressLev = Z_DEFAULT_COMPRESSION;
        else
           MsgCompressLev = CompressLevel;
     }

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( MsgPartReg > 1 &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength ))
     {
        /* */    /*E0180*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0181*/

           /* */    /*E0182*/

           i = MsgLen + MsgLen/50 + 12;

           RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

           MPIMsgNumber++;

           gzBuf = (Bytef *)MsgBuf;
           gzLen = (uLongf)i;

           SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                       (uLong)MsgLen, MsgCompressLev))

           if(i != Z_OK)
           {  /* */    /*E0183*/

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
           {  /* */    /*E0184*/

              /* */    /*E0185*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0186*/

              i = iom_Sendnowait(procnum, (void *)&MsgLen,
                                 sizeof(int), RTL_ReqPtr, msg_IOProcess);

              /* */    /*E0187*/

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
                         "*** RTS err 210.002: iom_Sendnowait rc = %d\n",
                         i);

              iom_Waitrequest(RTL_ReqPtr);
           }
           else
              Part = 0; /* */    /*E0188*/

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0189*/
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf; 
        }
     }
     else
     {  /* */    /*E0190*/

        /* */    /*E0191*/

        i = MsgLen + MsgLen/50 + 12;

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, i);

        MPIMsgNumber++;

        gzBuf = (Bytef *)MsgBuf;
        gzLen = (uLongf)i;

        SYSTEM_RET(i, dvm_compress, (gzBuf, &gzLen, (Bytef *)buf,
                                    (uLong)MsgLen, MsgCompressLev))

        if(i != Z_OK)
        {  /* */    /*E0192*/

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
        RTL_ReqPtr->IsCompressSize = 1;  /* */    /*E0193*/
        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  RTL_ReqPtr->SendSign = 1;         /* */    /*E0194*/
  RTL_ReqPtr->ProcNumber = procnum; /* */    /*E0195*/
  RTL_ReqPtr->BufAddr = (char *)buf;/* */    /*E0196*/
  RTL_ReqPtr->BufLength = rc;       /* */    /*E0197*/
  RTL_ReqPtr->FlagNumber = 0;       /* */    /*E0198*/
  RTL_ReqPtr->MPSFlagArray = NULL;  /* */    /*E0199*/
  RTL_ReqPtr->EndExchange = NULL;   /* */    /*E0200*/

  RTL_ReqPtr->MsgLength = 0; /* */    /*E0201*/
  RTL_ReqPtr->Remainder = 0; /* */    /*E0202*/
  RTL_ReqPtr->Init = 0;      /* */    /*E0203*/
  RTL_ReqPtr->Last = -1;     /* */    /*E0204*/
  RTL_ReqPtr->Chan = -1;     /* */    /*E0205*/
  RTL_ReqPtr->tag  = msg_IOProcess;/* */    /*E0206*/
  RTL_ReqPtr->CurrOper = CurrOper; /* */    /*E0207*/

  if(RTL_TRACE)
     trc_PrintBuffer((char *)buf, rc,
                     call_io_Sendnowait);   /* */    /*E0208*/

  if(MsgSchedule && UserSumFlag)
  {  /* */    /*E0209*/

     if(MsgLen > MaxMsgLength && Part != 0)
     {  /* */    /*E0210*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0211*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0212*/
        MsgLength = MaxMsgLength;          /* */    /*E0213*/
        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0214*/

        dvm_AllocArray(MPS_Request, RTL_ReqPtr->FlagNumber,
                       RTL_ReqPtr->MPSFlagArray);

        RTL_ReqPtr->MsgLength = MsgLength;
        RTL_ReqPtr->Remainder = Remainder;
     }
     else
     {  /* */    /*E0215*/

        RTL_ReqPtr->FlagNumber = 1;
        RTL_ReqPtr->MsgLength = MsgLen;
        RTL_ReqPtr->Remainder = 0;

        dvm_AllocArray(MPS_Request, 1, RTL_ReqPtr->MPSFlagArray);

        MsgNumber = 1;
        MsgLength = MsgLen;
        Remainder = 0;
     }

     coll_Insert(&SendReqColl, RTL_ReqPtr); /* */    /*E0216*/
     NewMsgNumber++;      /* */    /*E0217*/

     rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeff);

     if(RTL_TRACE && MsgScheduleTrace &&
        TstTraceEvent(call_io_Sendnowait))
        tprintf("*** MsgScheduleTrace. Sendnowait:\n"
            "MsgNumber=%d MsgLength=%d Remainder=%d Init=%d Last=%d\n",
            MsgNumber, MsgLength, Remainder,
            RTL_ReqPtr->Init, RTL_ReqPtr->Last);
  }
  else
  {  /* */    /*E0218*/

     if(MsgPartReg > 1 &&
        MaxMsgLength > 0 && UserSumFlag && MsgLen > MaxMsgLength &&
        Part != 0)
     {  /* */    /*E0219*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0220*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0221*/
        MsgLength = MaxMsgLength;          /* */    /*E0222*/

        if(MaxMsgParts)
        {  /* */    /*E0223*/

           if(Remainder)
              rc = MsgNumber + 1;
           else
              rc = MsgNumber;

           if(rc > MaxMsgParts)
           {  /* */    /*E0224*/

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
                 {  /* */    /*E0225*/

                    MsgLength += (4-rc); /* */    /*E0226*/
                    MsgNumber = MsgLen / MsgLength; /* */    /*E0227*/
                    Remainder = MsgLen % MsgLength; /* */    /*E0228*/
                 }
              }
           }
        }

        if(RTL_TRACE && MsgPartitionTrace &&
           TstTraceEvent(call_io_Sendnowait))
           tprintf("*** MsgPartitionTrace. Sendnowait: "
                   "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                   MsgNumber, MsgLength, Remainder);

        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0229*/

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
              CommTime = dvm_time();   /* */    /*E0230*/

           rc = iom_Sendnowait1(procnum, CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                                msg_IOProcess);

           /* */    /*E0231*/

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

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: iom_Sendnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(Remainder)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();  /* */    /*E0232*/

           rc = iom_Sendnowait1(procnum, CharPtr,
                                Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                                msg_IOProcess);

           /* */    /*E0233*/

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

           if(rc < 0)
              eprintf(__FILE__,__LINE__,
                      "*** RTS err 210.002: iom_Sendnowait1 rc = %d\n",
                      rc);
        }
     }
     else
     {  /* */    /*E0234*/

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* */    /*E0235*/

        rc = iom_Sendnowait(procnum, MsgBuf, MsgLen, RTL_ReqPtr,
                            msg_IOProcess);

        /* */    /*E0236*/

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

        if(rc < 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 210.002: iom_Sendnowait rc = %d\n", rc);
     }
  }

  for(tl=0; tl < SendDelay; tl++) /* */    /*E0237*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_io_Sendnowait,"rc=%d; req=%lx;\n",
                                 rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



int  io_Recvnowait(void *buf, int count, int size, int procnum,
                   RTL_Request *RTL_ReqPtr)
{
  int           rc, ByteCount, MsgLen, MsgNumber, Remainder, i,
                MsgLength;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;
  char         *CharPtr;
  void         *MsgBuf; 
  byte          Part = 1;

#if defined(_DVM_ZLIB_)

  int  MsgCompressLev;

#endif

  DVMMTimeStart(call_io_Recvnowait);

  if(RTL_TRACE)
     dvm_trace(call_io_Recvnowait,
               "buf=%lx; count=%d; size=%d; req=%lx; procnum=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum);

  NoWaitCount++;  /* */    /*E0238*/

  ByteCount = count * size;

  RTL_ReqPtr->SendRecvTime = Curr_dvm_time; /* */    /*E0239*/

  MsgLen = ByteCount + SizeDelta[ByteCount & Msk3];

  /* */    /*E0240*/

  RTL_ReqPtr->CompressBuf = NULL;  /* */    /*E0241*/
  RTL_ReqPtr->ResCompressIndex = -1;
  MsgBuf = buf;

  if(MsgDVMCompress != 0 && MsgLen > MinMsgCompressLength &&
     UserSumFlag != 0 && (ByteCount % sizeof(double)) == 0)
  {  /* */    /*E0242*/

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( MsgPartReg > 1 &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength ))
     {
        /* */    /*E0243*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0244*/

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0245*/

              /* */    /*E0246*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0247*/

              rc = iom_Recvnowait(procnum, (void *)&MsgLen, sizeof(int),
                                  RTL_ReqPtr, msg_IOProcess);

              /* */    /*E0248*/

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
                         "*** RTS err 210.003: iom_Recvnowait rc = %d\n",
                         rc);

              iom_Waitrequest(RTL_ReqPtr);

              RTL_ReqPtr->IsCompressSize = 1; /* */    /*E0249*/
           }
           else
           {  Part = 0; /* */    /*E0250*/
              RTL_ReqPtr->IsCompressSize = 0; /* */    /*E0251*/
           }

           RTL_ReqPtr->CompressMsgLen = MsgLen;
           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, MsgLen);
           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
        }
     }
     else
     {  /* */    /*E0252*/

        /* */    /*E0253*/

        MsgLen += MsgLen / sizeof(double); /* */    /*E0254*/
        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 0;  /* */    /*E0255*/

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, MsgLen);

        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #if defined(_DVM_ZLIB_)

  if(MsgCompressLevel >= 0 && MsgCompressLevel < 10 &&
     (MsgDVMCompress == 0 || (ByteCount % sizeof(double)) != 0) &&
     MsgLen > MinMsgCompressLength && UserSumFlag != 0)
  {  /* */    /*E0256*/

     MsgCompressLev = MsgCompressLevel;  /* */    /*E0257*/

     if(MsgCompressLev == 0)
     {  /* */    /*E0258*/

        if(CompressLevel == 0)
           MsgCompressLev = Z_DEFAULT_COMPRESSION;
        else
           MsgCompressLev = CompressLevel;
     }

     if(( MsgSchedule && MsgLen > MaxMsgLength ) ||
        ( MsgPartReg > 1 &&
          MaxMsgLength > 0 && MsgLen > MaxMsgLength ))
     {
        /* */    /*E0259*/

        if(MsgCompressWithMsgPart != 0)
        {  /* */    /*E0260*/

           if(MsgCompressWithMsgPart > 0)
           {  /* */    /*E0261*/

              /* */    /*E0262*/

              if(_SendRecvTime && StatOff == 0)
                 CommTime = dvm_time();   /* */    /*E0263*/

              rc = iom_Recvnowait(procnum, (void *)&MsgLen, sizeof(int),
                                  RTL_ReqPtr, msg_IOProcess);

              /* */    /*E0264*/

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
                         "*** RTS err 210.003: iom_Recvnowait rc = %d\n",
                         rc);

              iom_Waitrequest(RTL_ReqPtr);

              RTL_ReqPtr->IsCompressSize = 1; /* */    /*E0265*/
           }
           else
           {  Part = 0; /* */    /*E0266*/
              RTL_ReqPtr->IsCompressSize = 0; /* */    /*E0267*/
           }

           RTL_ReqPtr->CompressMsgLen = MsgLen;

           RTL_ReqPtr->ResCompressIndex =
           compress_malloc(&MsgBuf, MsgLen);

           RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
        }
     }
     else
     {  /* */    /*E0268*/

        /* */    /*E0269*/

        MsgLen                     = MsgLen + MsgLen/50 + 12;
        RTL_ReqPtr->CompressMsgLen = MsgLen;
        RTL_ReqPtr->IsCompressSize = 0;  /* */    /*E0270*/

        RTL_ReqPtr->ResCompressIndex = compress_malloc(&MsgBuf, MsgLen);

        RTL_ReqPtr->CompressBuf = (char *)MsgBuf;
     }
  }

  #endif

  RTL_ReqPtr->SendSign = 0;         /* */    /*E0271*/
  RTL_ReqPtr->ProcNumber = procnum; /* */    /*E0272*/
  RTL_ReqPtr->BufAddr = (char *)buf;/* */    /*E0273*/
  RTL_ReqPtr->BufLength = ByteCount;/* */    /*E0274*/
  RTL_ReqPtr->FlagNumber = 0;       /* */    /*E0275*/
  RTL_ReqPtr->MPSFlagArray = NULL;  /* */    /*E0276*/
  RTL_ReqPtr->EndExchange = NULL;   /* */    /*E0277*/

  RTL_ReqPtr->MsgLength = 0; /* */    /*E0278*/
  RTL_ReqPtr->Remainder = 0; /* */    /*E0279*/
  RTL_ReqPtr->Init = 0;      /* */    /*E0280*/
  RTL_ReqPtr->Last = -1;     /* */    /*E0281*/
  RTL_ReqPtr->Chan = -1;     /* */    /*E0282*/
  RTL_ReqPtr->tag  = msg_IOProcess;/* */    /*E0283*/
  RTL_ReqPtr->CurrOper = CurrOper; /* */    /*E0284*/

  if(MsgSchedule && UserSumFlag)
  {  /* */    /*E0285*/

     if(MsgLen > MaxMsgLength && Part != 0)
     {  /* */    /*E0286*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0287*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0288*/
        MsgLength = MaxMsgLength;          /* */    /*E0289*/
        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0290*/

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
     {  /* */    /*E0291*/

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
        TstTraceEvent(call_io_Recvnowait))
        tprintf("*** MsgScheduleTrace. Recvnowait:\n"
             "MsgNumber=%d MsgLength=%d Remainder=%d Init=0 Last=%d\n",
             MsgNumber, MsgLength, Remainder, RTL_ReqPtr->Last);

     CharPtr = (char *)MsgBuf;

     for(i=0; i < MsgNumber; i++)
     {
        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* */    /*E0292*/

        rc = iom_Recvnowait1(procnum, CharPtr,
                             MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                             msg_IOProcess);

        /* */    /*E0293*/

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
                   "*** RTS err 210.003: iom_Recvnowait1 rc = %d\n",
                   rc);
        CharPtr += MsgLength;
     }

     if(Remainder)
     {
        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* */    /*E0294*/

        rc = iom_Recvnowait1(procnum, CharPtr,
                             Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                             msg_IOProcess);

        /* */    /*E0295*/

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
                   "*** RTS err 210.003: iom_Recvnowait1 rc = %d\n",
                   rc);
     }
  }
  else
  {  /* */    /*E0296*/

     if(MsgPartReg > 1 &&
        MaxMsgLength > 0 && UserSumFlag && MsgLen > MaxMsgLength &&
        Part != 0)
     {  /* */    /*E0297*/

        MsgNumber = MsgLen / MaxMsgLength; /* */    /*E0298*/
        Remainder = MsgLen % MaxMsgLength; /* */    /*E0299*/
        MsgLength = MaxMsgLength;          /* */    /*E0300*/

        if(MaxMsgParts)
        {  /* */    /*E0301*/

           if(Remainder)
              rc = MsgNumber + 1;
           else
              rc = MsgNumber;

           if(rc > MaxMsgParts)
           {  /* */    /*E0302*/

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
                 {  /* */    /*E0303*/

                    MsgLength += (4-rc); /* */    /*E0304*/
                    MsgNumber = MsgLen / MsgLength; /* */    /*E0305*/
                    Remainder = MsgLen % MsgLength; /* */    /*E0306*/
                 }
              }
           }
        }

        if(RTL_TRACE && MsgPartitionTrace &&
           TstTraceEvent(call_io_Recvnowait))
           tprintf("*** MsgPartitionTrace. Recvnowait: "
                   "MsgNumber=%d MsgLength=%d Remainder=%d\n",
                   MsgNumber, MsgLength, Remainder);

        RTL_ReqPtr->FlagNumber = MsgNumber;

        if(Remainder)
           RTL_ReqPtr->FlagNumber++; /* */    /*E0307*/

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
              CommTime = dvm_time();   /* */    /*E0308*/

           rc = iom_Recvnowait1(procnum, CharPtr,
                                MsgLength, &RTL_ReqPtr->MPSFlagArray[i],
                                msg_IOProcess);

           /* */    /*E0309*/

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
                      "*** RTS err 210.003: iom_Recvnowait1 rc = %d\n",
                      rc);
           CharPtr += MsgLength;
        }

        if(Remainder)
        {
           if(_SendRecvTime && StatOff == 0)
              CommTime = dvm_time();   /* */    /*E0310*/

           rc = iom_Recvnowait1(procnum, CharPtr,
                                Remainder, &RTL_ReqPtr->MPSFlagArray[i],
                                msg_IOProcess);

           /* */    /*E0311*/

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
                      "*** RTS err 210.003: iom_Recvnowait1 rc = %d\n",
                      rc);
        }
     }
     else
     {  /* */    /*E0312*/

        if(_SendRecvTime && StatOff == 0)
           CommTime = dvm_time();   /* */    /*E0313*/

        rc = iom_Recvnowait(procnum, MsgBuf, MsgLen, RTL_ReqPtr,
                            msg_IOProcess);

        /* */    /*E0314*/

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
                   "*** RTS err 210.003: iom_Recvnowait rc = %d\n", rc);
     }
  }

  for(tl=0; tl < RecvDelay; tl++) /* */    /*E0315*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_io_Recvnowait,"rc=%d; req=%lx;\n",
                                   rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}



void  io_Waitrequest(RTL_Request *RTL_ReqPtr)
{
  int           i, j, k, procnum = RTL_ReqPtr->ProcNumber, tst;
  DvmType          tl;
  double        op1 = 1.1, op2 = 1.1;

#ifdef _DVM_MPI_

  char         *CharPtr0, *CharPtr1, *CharPtr2, *CharPtr3, *CharPtr4;
  int           n;

#endif

#if defined(_DVM_ZLIB_)

  uLongf        gzLen;

#endif

  DVMMTimeStart(call_io_Waitrequest);

  if(RTL_TRACE)
     dvm_trace(call_io_Waitrequest, "req=%lx; procnum=%d;\n",
                (uLLng)RTL_ReqPtr, procnum);

  NoWaitCount--;  /* */    /*E0316*/

  if(RTL_ReqPtr->BufAddr != NULL)
  {
     if(MsgSchedule && UserSumFlag)
     {  /* */    /*E0317*/

        tst = rtl_TstReqColl(0);

        if(RTL_ReqPtr->SendSign)
        {  /* */    /*E0318*/

           if(RTL_TRACE && MsgScheduleTrace &&
              TstTraceEvent(call_io_Waitrequest))
              tprintf("*** MsgScheduleTrace. Waitrequest (s): "
                      "FlagNumber=%d Init=%d Last=%d\n",
                      RTL_ReqPtr->FlagNumber,
                      RTL_ReqPtr->Init, RTL_ReqPtr->Last);

           if(RTL_ReqPtr->Init == RTL_ReqPtr->FlagNumber)
           {  /* */    /*E0319*/

              if(tst)
                 rtl_SendReqColl(ResCoeffWaitReq);
           }
           else
           {  /* */    /*E0320*/

              if(RTL_ReqPtr->Chan == -1)
              {  /* */    /*E0321*/

                 if(FreeChanNumber)
                 {  /* */    /*E0322*/

                    for(i=0; i < ParChanNumber; i++)
                    {  if(ChanRTL_ReqPtr[i] == NULL)
                          break;  /* */    /*E0323*/
                    }
                 }
                 else
                 {  /* */    /*E0324*/

                    i = -1;
                    j = INT_MAX;   /* */    /*E0325*/

                    for(k=0; k < ParChanNumber; k++)
                    {  tst = (ChanRTL_ReqPtr[k])->FlagNumber -
                             (ChanRTL_ReqPtr[k])->Init;

                       if(tst < j)
                       {  j = tst;
                          i = k;
                       }
                    }

                    rtl_FreeChan(i);  /* */    /*E0326*/
                 }

                 /* */    /*E0327*/

                 ChanRTL_ReqPtr[i] = RTL_ReqPtr;
                 RTL_ReqPtr->Chan = i;
                 FreeChanNumber--;
                 NewMsgNumber--;
              }

              /* */    /*E0328*/

              rtl_FreeChan(RTL_ReqPtr->Chan); /* */    /*E0329*/
              if(FreeChanReg == 0)
                 rtl_TstReqColl(0); /* */    /*E0330*/

              rtl_SendReqColl(ResCoeffWaitReq); /* */    /*E0331*/
           }

           dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
           RTL_ReqPtr->FlagNumber = 0;
           RTL_ReqPtr->MPSFlagArray = NULL;

           coll_Delete(&SendReqColl, RTL_ReqPtr); /* */    /*E0332*/

           if(RTL_ReqPtr->CompressBuf != NULL)
           {  /* */    /*E0333*/

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
        else
        {  /* */    /*E0334*/
 
           if(tst)
              rtl_SendReqColl(ResCoeffWaitReq);

           if(RTL_TRACE && MsgScheduleTrace &&
              TstTraceEvent(call_io_Waitrequest))
              tprintf("*** MsgScheduleTrace. Waitrequest (r): "
                      "FlagNumber=%d Init=%d Last=%d\n",
                      RTL_ReqPtr->FlagNumber,
                      RTL_ReqPtr->Init, RTL_ReqPtr->Last);

           for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
               iom_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

           dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
           dvm_FreeArray(RTL_ReqPtr->EndExchange);
           RTL_ReqPtr->FlagNumber = 0;
           RTL_ReqPtr->MPSFlagArray = NULL;
           RTL_ReqPtr->EndExchange = NULL;

           if(RTL_ReqPtr->CompressBuf != NULL)
           {  /* */    /*E0335*/

              if(RTL_ReqPtr->IsCompressSize == 0)
              {  /* */    /*E0336*/

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

              tl += SizeDelta[tl & Msk3];

              if(MsgDVMCompress != 0 && tl > MinMsgCompressLength &&
                 UserSumFlag != 0 &&
                 (RTL_ReqPtr->BufLength % sizeof(double)) == 0)
              {  /* */    /*E0337*/

                 /* */    /*E0338*/

                 if((double)RTL_ReqPtr->CompressMsgLen / (double)tl <=
                    CompressCoeff)
                 {  /* */    /*E0339*/

                    j = (int)(tl / sizeof(double));  /* */    /*E0340*/
                    if(MsgDVMCompress < 2)
                    {  /* */    /*E0341*/

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
                    {  /* */    /*E0342*/

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
                       {  /* */    /*E0343*/

                          CharPtr2 = CharPtr4;

                          k = (unsigned char)(*CharPtr2);
                          CharPtr3 = CharPtr2 + 1;
                          CharPtr4 = CharPtr3 + n;
                       }
                    }
                 }
                 else
                 {  /* */    /*E0344*/

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
              {  /* */    /*E0345*/

                 /* */    /*E0346*/

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
        }
     }
     else
     {  /* */    /*E0347*/

        if(RTL_ReqPtr->FlagNumber)
        {  /* */    /*E0348*/

           if(RTL_TRACE && MsgPartitionTrace &&
              TstTraceEvent(call_io_Waitrequest))
              tprintf("*** MsgPartitionTrace. Waitrequest: "
                      "FlagNumber=%d\n", RTL_ReqPtr->FlagNumber);

           for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
               iom_Waitrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

           dvm_FreeArray(RTL_ReqPtr->MPSFlagArray);
           dvm_FreeArray(RTL_ReqPtr->EndExchange);
           RTL_ReqPtr->FlagNumber = 0;
           RTL_ReqPtr->MPSFlagArray = NULL;
           RTL_ReqPtr->EndExchange = NULL;
        }
        else
        {
           iom_Waitrequest(RTL_ReqPtr);
        }

        if(RTL_ReqPtr->CompressBuf != NULL)
        {  /* */    /*E0349*/

           if(RTL_ReqPtr->SendSign)
           {  /* */    /*E0350*/

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
           {  /* */    /*E0351*/

              if(RTL_ReqPtr->IsCompressSize == 0)
              {  /* */    /*E0352*/

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

              tl += SizeDelta[tl & Msk3];

              if(MsgDVMCompress != 0 && tl > MinMsgCompressLength &&
                 UserSumFlag != 0 &&
                 (RTL_ReqPtr->BufLength % sizeof(double)) == 0)
              {  /* */    /*E0353*/

                 /* */    /*E0354*/

                 if((double)RTL_ReqPtr->CompressMsgLen / (double)tl <=
                    CompressCoeff)
                 {  /* */    /*E0355*/

                    j = (int)(tl / sizeof(double));  /* */    /*E0356*/
                    if(MsgDVMCompress < 2)
                    {  /* */    /*E0357*/

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
                    {  /* */    /*E0358*/

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
                       {  /* */    /*E0359*/

                          CharPtr2 = CharPtr4;

                          k = (unsigned char)(*CharPtr2);
                          CharPtr3 = CharPtr2 + 1;
                          CharPtr4 = CharPtr3 + n;
                       }
                    }
                 }
                 else
                 {  /* */    /*E0360*/

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
              {  /* */    /*E0361*/

                 /* */    /*E0362*/

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
     }

     if(RTL_TRACE)
        trc_PrintBuffer(RTL_ReqPtr->BufAddr, RTL_ReqPtr->BufLength,
                        call_io_Waitrequest); /* */    /*E0363*/
   
     RTL_ReqPtr->BufAddr = NULL;
  }

  for(tl=0; tl < WaitDelay; tl++) /* */    /*E0364*/
      op1 /= op2;

  if(RTL_TRACE)
     dvm_trace(ret_io_Waitrequest,"req=%lx;\n", (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  (DVM_RET);
  return;
}



int  io_Testrequest(RTL_Request *RTL_ReqPtr)
{
  int    rc = 1, procnum = RTL_ReqPtr->ProcNumber, tst;

  DVMMTimeStart(call_io_Testrequest);

  if(RTL_TRACE)
     dvm_trace(call_io_Testrequest, "req=%lx; procnum=%d;\n",
                (uLLng)RTL_ReqPtr, procnum);

  if(RTL_ReqPtr->BufAddr != NULL)
  {
     if(MsgSchedule && UserSumFlag)
     {  /* */    /*E0365*/

        tst = rtl_TstReqColl(0);

        if(tst)
           rtl_SendReqColl(ResCoeffTstReq);

        if(RTL_ReqPtr->SendSign)
        {  /* */    /*E0366*/

           if(RTL_TRACE && MsgScheduleTrace &&
              TstTraceEvent(call_io_Testrequest))
              tprintf("*** MsgScheduleTrace. Testrequest (s): "
                      "FlagNumber=%d Init=%d Last=%d\n",
                      RTL_ReqPtr->FlagNumber,
                      RTL_ReqPtr->Init, RTL_ReqPtr->Last);

           if(RTL_ReqPtr->Init != RTL_ReqPtr->FlagNumber)
              rc = 0;
        }
        else
        {  /* */    /*E0367*/
 
           if(RTL_TRACE && MsgScheduleTrace &&
              TstTraceEvent(call_io_Testrequest))
              tprintf("*** MsgScheduleTrace. Testrequest (r): "
                      "FlagNumber=%d Init=%d Last=%d\n",
                      RTL_ReqPtr->FlagNumber,
                      RTL_ReqPtr->Init, RTL_ReqPtr->Last);

           for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
           {  if(RTL_ReqPtr->EndExchange[tst] == 0)
              { rc = iom_Testrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

                if(rc == 0)
                   break; /* */    /*E0368*/

                RTL_ReqPtr->EndExchange[tst] = 1;
              }
           }
        }
     }
     else
     {  /* */    /*E0369*/

        if(RTL_ReqPtr->FlagNumber)
        {  /* */    /*E0370*/

           if(RTL_TRACE && MsgPartitionTrace &&
              TstTraceEvent(call_io_Testrequest))
              tprintf("*** MsgPartitionTrace. Testrequest: "
                      "FlagNumber=%d\n", RTL_ReqPtr->FlagNumber);

           for(tst=0; tst < RTL_ReqPtr->FlagNumber; tst++)
           {  if(RTL_ReqPtr->EndExchange[tst] == 0)
              { rc = iom_Testrequest1(&RTL_ReqPtr->MPSFlagArray[tst]);

                if(rc == 0)
                   break; /* */    /*E0371*/

                RTL_ReqPtr->EndExchange[tst] = 1;
              }
           }
        }
        else
           rc = iom_Testrequest(RTL_ReqPtr);
     }

     if(rc)
     {  if(RTL_TRACE && RTL_ReqPtr->CompressBuf == NULL)
           trc_PrintBuffer(RTL_ReqPtr->BufAddr, RTL_ReqPtr->BufLength,
                           call_io_Testrequest); /* */    /*E0372*/
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_io_Testrequest,"rc=%d; req=%lx;\n",
                                    rc, (uLLng)RTL_ReqPtr);

  DVMMTimeFinish;

  return  (DVM_RET, rc);
}


/* ---------------------------------------------------- */    /*E0373*/


int  iom_Sendnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  tag)
{ byte  IssendSign = 0;
  int   rc;

  if(RTL_TRACE && MPI_IORequestTrace)
  {  if(IAmIOProcess)
        tprintf("*** iom_Sendnowait: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d RTL_ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)RTL_ReqPtr, tag);
     else
        tprintf("*** iom_Sendnowait: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d RTL_ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)RTL_ReqPtr, tag);
  }

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

  if(MPI_Issend_sign) /* */    /*E0374*/
  {  if(ByteCount < IssendMsgLength)
     {  if(IAmIOProcess)     
           SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                              ApplProcessNumber[ProcIdent],
                              tag, MPI_COMM_WORLD_1,
                              &RTL_ReqPtr->MPSFlag))
         else
           SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                              IOProcessNumber[ProcIdent], tag,
                              MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
     }
     else
     {  if(IAmIOProcess)
           SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                               ApplProcessNumber[ProcIdent],
                               tag, MPI_COMM_WORLD_1,
                               &RTL_ReqPtr->MPSFlag))
        else
           SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                               IOProcessNumber[ProcIdent], tag,
                               MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
        IssendSign = 1;
     }
  }
  else
  {  if(IAmIOProcess)
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           ApplProcessNumber[ProcIdent], tag,
                           MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
     else
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           IOProcessNumber[ProcIdent], tag,
                           MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
  }

  if(RTL_TRACE && MPI_IORequestTrace)
  {  if(IssendSign)
        tprintf("*** MPI_Issend: RTL_ReqPtr=%lx\n",
                (uLLng)RTL_ReqPtr);
     else
        tprintf("*** MPI_Isend: RTL_ReqPtr=%lx\n",
                (uLLng)RTL_ReqPtr);
  }

  /* */    /*E0375*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MsgSchedule == 0 && RequestCount < ReqBufSize)
  {  RequestBuffer[RequestCount] = RTL_ReqPtr;
     RequestCount++;
  }

  if(MPITestAfterSend > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  return  1;
}



int  iom_Sendnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  tag)
{ byte  IssendSign = 0;
  int   rc;

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Sendnowait1 = dvm_time();

  if(RTL_TRACE && MPI_IORequestTrace)
  {
     if(IAmIOProcess)
        tprintf("*** iom_Sendnowait1: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);
     else
        tprintf("*** iom_Sendnowait1: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);
  }

  if(MPIInfoPrint && StatOff == 0)
  {
     if(UserSumFlag != 0)
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

  if(MPI_Issend_sign) /* */    /*E0376*/
  {  if(ByteCount < IssendMsgLength)
     {  if(IAmIOProcess)
           SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                              ApplProcessNumber[ProcIdent],
                              tag, MPI_COMM_WORLD_1, ReqPtr))
        else
           SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                              IOProcessNumber[ProcIdent], tag,
                              MPI_COMM_WORLD_1, ReqPtr))
     }
     else
     {  if(IAmIOProcess)
           SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                               ApplProcessNumber[ProcIdent],
                               tag, MPI_COMM_WORLD_1, ReqPtr))
        else 
           SYSTEM(MPI_Issend, (BufPtr, ByteCount, MPI_BYTE,
                               IOProcessNumber[ProcIdent], tag,
                               MPI_COMM_WORLD_1, ReqPtr))
        IssendSign = 1;
     }
  }
  else
  {
     if(IAmIOProcess)
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           ApplProcessNumber[ProcIdent], tag,
                           MPI_COMM_WORLD_1, ReqPtr))
     else
        SYSTEM(MPI_Isend, (BufPtr, ByteCount, MPI_BYTE,
                           IOProcessNumber[ProcIdent], tag,
                           MPI_COMM_WORLD_1, ReqPtr))
  }

  if(RTL_TRACE && MPI_IORequestTrace)
  {  if(IssendSign)
        tprintf("*** MPI_Issend: ReqPtr=%lx\n", (uLLng)ReqPtr);
     else
        tprintf("*** MPI_Isend: ReqPtr=%lx\n", (uLLng)ReqPtr);
  }

  /* */    /*E0377*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MsgSchedule == 0 && MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterSend > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Sendnowait1  = dvm_time() - t_iom_Sendnowait1;
     T_iom_Sendnowait1 += t_iom_Sendnowait1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Sendnowait1=%lf IOProcTime=%lf\n",
                 t_iom_Sendnowait1, dvm_time()-IOProcTime);
       
  }

  return  1;
}



int  iom_Sendinit1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                   MPS_Request  *ReqPtr, int  tag)
{ byte  IssendSign = 0;
  int   rc;      

  if(RTL_TRACE && MPI_IORequestTrace)
  {  if(IAmIOProcess)
        tprintf("*** iom_Sendinit1: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);
     else
        tprintf("*** iom_Sendinit1: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);
  }

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

  if(MPI_Issend_sign) /* */    /*E0378*/
  {  if(ByteCount < IssendMsgLength)
     {  if(IAmIOProcess)
           SYSTEM(MPI_Send_init, (BufPtr, ByteCount, MPI_BYTE,
                                  ApplProcessNumber[ProcIdent],
                                  tag, MPI_COMM_WORLD_1, ReqPtr))
        else
           SYSTEM(MPI_Send_init, (BufPtr, ByteCount, MPI_BYTE,
                                  IOProcessNumber[ProcIdent],
                                  tag, MPI_COMM_WORLD_1, ReqPtr))
     }
     else
     {  if(IAmIOProcess)
           SYSTEM(MPI_Ssend_init, (BufPtr, ByteCount, MPI_BYTE,
                                   ApplProcessNumber[ProcIdent],
                                   tag, MPI_COMM_WORLD_1,
                                   ReqPtr))
        else 
           SYSTEM(MPI_Ssend_init, (BufPtr, ByteCount, MPI_BYTE,
                                   IOProcessNumber[ProcIdent],
                                   tag, MPI_COMM_WORLD_1, ReqPtr))
        IssendSign = 1;
     }
  }
  else
  {
     if(IAmIOProcess)
        SYSTEM(MPI_Send_init, (BufPtr, ByteCount, MPI_BYTE,
                               ApplProcessNumber[ProcIdent],
                               tag, MPI_COMM_WORLD_1, ReqPtr))
     else
        SYSTEM(MPI_Send_init, (BufPtr, ByteCount, MPI_BYTE,
                               IOProcessNumber[ProcIdent], tag,
                               MPI_COMM_WORLD_1, ReqPtr))
  }

  if(RTL_TRACE && MPI_IORequestTrace)
  {  if(IssendSign)
        tprintf("*** MPI_Issend: ReqPtr=%lx\n", (uLLng)ReqPtr);
     else
        tprintf("*** MPI_Isend: ReqPtr=%lx\n", (uLLng)ReqPtr);
  }

  /* */    /*E0379*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MsgSchedule == 0 && MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterSend > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  return  1;
}



int  iom_Recvnowait(int  ProcIdent, void  *BufPtr, int  ByteCount,
                    RTL_Request  *RTL_ReqPtr, int  tag)
{ int   rc;

  if(IAmIOProcess)
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvnowait: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d RTL_ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)RTL_ReqPtr, tag);

     SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                        ApplProcessNumber[ProcIdent], tag,
                        MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
  }
  else
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvnowait: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d RTL_ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)RTL_ReqPtr, tag);

     SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                        IOProcessNumber[ProcIdent], tag,
                        MPI_COMM_WORLD_1, &RTL_ReqPtr->MPSFlag))
  }

  /* */    /*E0380*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     RequestCount < ReqBufSize)
  {  RequestBuffer[RequestCount] = RTL_ReqPtr;
     RequestCount++;
  }

  if(MPITestAfterRecv > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  return  1;
}



int  iom_Recvnowait1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                     MPS_Request  *ReqPtr, int  tag)
{ int   rc;
 
  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Recvnowait1 = dvm_time();

  if(IAmIOProcess)
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvnowait1: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);

     SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                        ApplProcessNumber[ProcIdent], tag,
                        MPI_COMM_WORLD_1, ReqPtr))
  }
  else
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvnowait1: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);

     SYSTEM(MPI_Irecv, (BufPtr, ByteCount, MPI_BYTE,
                        IOProcessNumber[ProcIdent], tag,
                        MPI_COMM_WORLD_1, ReqPtr))
  }

  /* */    /*E0381*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterRecv > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Recvnowait1  = dvm_time() - t_iom_Recvnowait1;
     T_iom_Recvnowait1 += t_iom_Recvnowait1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Recvnowait1=%lf IOProcTime=%lf\n",
                t_iom_Recvnowait1, dvm_time()-IOProcTime);
  }

  return  1;
}



int  iom_Recvinit1(int  ProcIdent, void  *BufPtr, int  ByteCount,
                   MPS_Request  *ReqPtr, int  tag)
{ int   rc;

  if(IAmIOProcess)
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvinit1: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);

     SYSTEM(MPI_Recv_init, (BufPtr, ByteCount, MPI_BYTE,
                            ApplProcessNumber[ProcIdent], tag,
                            MPI_COMM_WORLD_1, ReqPtr))
  }
  else
  {  if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recvinit1: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d ReqPtr=%lx tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent], (uLLng)BufPtr,
                ByteCount, (uLLng)ReqPtr, tag);

     SYSTEM(MPI_Recv_init, (BufPtr, ByteCount, MPI_BYTE,
                            IOProcessNumber[ProcIdent], tag,
                            MPI_COMM_WORLD_1, ReqPtr))
  }

  /* */    /*E0382*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  if(MPITestAfterRecv > 1 && tag != msg_IOInit)
     SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  return  1;
}



int  iom_Recvany1(void  *BufPtr, int  ByteCount, int  tag,
                  MPI_Status  *StatusPtr)
{ int   rc = 0;

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Recvany1 = dvm_time();

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Recvany1: BufPtr=%lx ByteCount=%d tag=%d\n",
             (uLLng)BufPtr, ByteCount, tag);

  SYSTEM(MPI_Recv, (BufPtr, ByteCount, MPI_BYTE, MPI_ANY_SOURCE, tag,
                    MPI_COMM_WORLD_1, StatusPtr))

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Recvany1  = dvm_time() - t_iom_Recvany1;
     T_iom_Recvany1 += t_iom_Recvany1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Recvany1=%lf IOProcTime=%lf\n",
                t_iom_Recvany1, dvm_time()-IOProcTime);
  }

  return  1;
}



int  iom_Recv1(int  ProcIdent, void  *BufPtr, int  ByteCount, int  tag,
               MPI_Status  *StatusPtr)
{ int   rc = 0;

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Recv1 = dvm_time();

  if(IAmIOProcess)
  {
     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recv1: Proc=%d ApplProc=%d BufPtr=%lx "
                "ByteCount=%d tag=%d\n",
                ProcIdent, ApplProcessNumber[ProcIdent],
                (uLLng)BufPtr, ByteCount, tag);
  }
  else
  {
     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("*** iom_Recv1: Proc=%d IOProc=%d BufPtr=%lx "
                "ByteCount=%d tag=%d\n",
                ProcIdent, IOProcessNumber[ProcIdent],
                (uLLng)BufPtr, ByteCount, tag);
  } 

  if(IAmIOProcess)
     SYSTEM(MPI_Recv, (BufPtr, ByteCount, MPI_BYTE,
                       ApplProcessNumber[ProcIdent], tag,
                       MPI_COMM_WORLD_1, StatusPtr))
  else
     SYSTEM(MPI_Recv, (BufPtr, ByteCount, MPI_BYTE,
                       IOProcessNumber[ProcIdent], tag,
                       MPI_COMM_WORLD_1, StatusPtr))

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Recv1  = dvm_time() - t_iom_Recv1;
     T_iom_Recv1 += t_iom_Recv1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Recv1=%lf IOProcTime=%lf\n",
                t_iom_Recv1, dvm_time()-IOProcTime);
  }

  return  1;
}



int  iom_Start1(MPS_Request  *ReqPtr)
{
  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Start1: ReqPtr=%lx\n", (uLLng)ReqPtr);

  SYSTEM(MPI_Start, (ReqPtr))

  /* */    /*E0383*/

  if(IAmIOProcess == 0 && SaveIOFlag && dopl_MPI_Test &&
     MPS_RequestCount < MPS_ReqBufSize)
  {  MPS_RequestBuffer[MPS_RequestCount] = ReqPtr;
     MPS_RequestCount++;
  }

  return  1;
}



int  iom_Startall1(int  RequestCount, MPS_Request  *ReqArray)
{
  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Startall1: RequestCount=%d, ReqArray=%lx\n",
             RequestCount, (uLLng)ReqArray);

  SYSTEM(MPI_Startall, (RequestCount, ReqArray))

  return  1;
}



void  iom_Waitrequest(RTL_Request  *RTL_ReqPtr)
{ int  i;

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Waitrequest: RTL_ReqPtr=%lx;\n",
             (uLLng)RTL_ReqPtr);

  SYSTEM(MPI_Wait, (&RTL_ReqPtr->MPSFlag, &RTL_ReqPtr->Status))

  /* */    /*E0384*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  for(i=0; i < RequestCount; i++)
     {  if(RequestBuffer[i] == RTL_ReqPtr)
           break;
     }

     if(i < RequestCount)
     {  /* */    /*E0385*/

        for(i++; i < RequestCount; i++)
            RequestBuffer[i-1] = RequestBuffer[i];
        RequestCount--;
     }
  }

  return;
}



void  iom_Waitrequest1(MPS_Request  *ReqPtr)
{ int  i;

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Waitrequest1 = dvm_time();

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Waitrequest1: ReqPtr=%lx\n", (uLLng)ReqPtr);

  SYSTEM(MPI_Wait, (ReqPtr, &GMPI_Status))

  /* */    /*E0386*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  for(i=0; i < MPS_RequestCount; i++)
     {  if(MPS_RequestBuffer[i] == ReqPtr)
           break;
     }

     if(i < MPS_RequestCount)
     {  /* */    /*E0387*/

        for(i++; i < MPS_RequestCount; i++)
            MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
        MPS_RequestCount--;
     }
  }

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Waitrequest1  = dvm_time() - t_iom_Waitrequest1;
     T_iom_Waitrequest1 += t_iom_Waitrequest1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Waitrequest1=%lf IOProcTime=%lf\n",
                t_iom_Waitrequest1, dvm_time()-IOProcTime);
  }

  return;
}



void  iom_Waitany1(int  RequestCount, MPS_Request  *ReqArray,
                   int  *IndexPtr, MPI_Status  *StatusPtr)
{ int           i, Index;
  MPS_Request  *ReqPtr;

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
     t_iom_Waitany1 = dvm_time();

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Waitany1: RequestCount=%d  ReqArray=%lx\n",
             RequestCount, (uLLng)ReqArray);

  SYSTEM(MPI_Waitany, (RequestCount, ReqArray, IndexPtr, StatusPtr))

  Index = *IndexPtr;  

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Waitany1: Index=%d\n", Index);

  /* */    /*E0388*/

  if(IAmIOProcess == 0 && SaveIOFlag && Index >= 0)
  {  ReqPtr = &ReqArray[Index];   /* */    /*E0389*/

     for(i=0; i < MPS_RequestCount; i++)
     {  if(MPS_RequestBuffer[i] == ReqPtr)
           break;
     }

     if(i < MPS_RequestCount)
     {  /* */    /*E0390*/

        for(i++; i < MPS_RequestCount; i++)
            MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
        MPS_RequestCount--;
     }
  }

  if((RTL_TRACE && MPI_IORequestTrace) || (MPIInfoPrint && StatOff == 0))
  {  t_iom_Waitany1  = dvm_time() - t_iom_Waitany1;
     T_iom_Waitany1 += t_iom_Waitany1;

     if(RTL_TRACE && MPI_IORequestTrace)
        tprintf("t_iom_Waitany1=%lf IOProcTime=%lf\n",
                t_iom_Waitany1, dvm_time()-IOProcTime);
  }

  return;
}



void  iom_Waitsome1(int  InRequestCount, MPS_Request  *ReqArray,
                    int  *OutRequestCountPtr, int  *IndexArray,
                    MPI_Status  *StatusArray)
{ int           i, j, OutRequestCount;
  MPS_Request  *ReqPtr;

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Waitsome1: InRequestCount=%d  ReqArray=%lx\n",
             InRequestCount, (uLLng)ReqArray);

  SYSTEM(MPI_Waitsome, (InRequestCount, ReqArray, OutRequestCountPtr,
                        IndexArray, StatusArray))

  OutRequestCount = *OutRequestCountPtr;  

  if(RTL_TRACE && MPI_IORequestTrace)
  {  tprintf("*** iom_Waitsome1: OutRequestCount=%d\n", OutRequestCount);

     for(i=0; i < OutRequestCount; i++)
         tprintf("*** iom_Waitsome1: IndexArray[%d]=%d\n",
                 i, IndexArray[i]);
  }

  /* */    /*E0391*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  for(j=0; j < OutRequestCount; j++)
     {  ReqPtr = &ReqArray[IndexArray[j]]; /* */    /*E0392*/

        for(i=0; i < MPS_RequestCount; i++)
        {  if(MPS_RequestBuffer[i] == ReqPtr)
              break;
        }

        if(i < MPS_RequestCount)
        {  /* */    /*E0393*/

           for(i++; i < MPS_RequestCount; i++)
               MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
           MPS_RequestCount--;
        }
     }
  }

  return;
}



int  iom_Testrequest(RTL_Request  *RTL_ReqPtr)
{ int  rc, i;

  SYSTEM(MPI_Test, (&RTL_ReqPtr->MPSFlag, &rc, &RTL_ReqPtr->Status))

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Testrequest: RTL_ReqPtr=%lx; rc=%d;\n",
             (uLLng)RTL_ReqPtr, rc);

  /* */    /*E0394*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  if(rc)
     {  for(i=0; i < RequestCount; i++)
        {  if(RequestBuffer[i] == RTL_ReqPtr)
              break;
        }

        if(i < RequestCount)
        {  /* */    /*E0395*/

           for(i++; i < RequestCount; i++)
               RequestBuffer[i-1] = RequestBuffer[i];
           RequestCount--;
        }
     }
  }

  return  rc;
}



int  iom_Testrequest1(MPS_Request  *ReqPtr)
{ int  i, rc;

  SYSTEM(MPI_Test, (ReqPtr, &rc, &GMPI_Status))

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Testrequest1: ReqPtr=%lx; rc=%d;\n",
             (uLLng)ReqPtr, rc);

  /* */    /*E0396*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  if(rc)
     {  for(i=0; i < MPS_RequestCount; i++)
        {  if(MPS_RequestBuffer[i] == ReqPtr)
              break;
        }

        if(i < MPS_RequestCount)
        {  /* */    /*E0397*/

           for(i++; i < MPS_RequestCount; i++)
               MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
           MPS_RequestCount--;
        }
     }
  }

  return  rc;
}



int  iom_Testany1(int  RequestCount, MPS_Request  *ReqArray,
                  int  *IndexPtr, int  *rc, MPI_Status  *StatusPtr)
{ int           i, Index, Res = 0;
  MPS_Request  *ReqPtr;

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Testany1: RequestCount=%d  ReqArray=%lx\n",
             RequestCount, (uLLng)ReqArray);

  SYSTEM(MPI_Testany, (RequestCount, ReqArray, IndexPtr, rc, StatusPtr))

  Index = *IndexPtr;
  Res = *rc;

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Testany1: Index=%d rc=%d\n", Index, Res);

  /* */    /*E0398*/

  if(IAmIOProcess == 0 && SaveIOFlag && Index >= 0 && Res != 0)
  {  ReqPtr = &ReqArray[Index];   /* */    /*E0399*/

     for(i=0; i < MPS_RequestCount; i++)
     {  if(MPS_RequestBuffer[i] == ReqPtr)
           break;
     }

     if(i < MPS_RequestCount)
     {  /* */    /*E0400*/

        for(i++; i < MPS_RequestCount; i++)
            MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
        MPS_RequestCount--;
     }
  }

  return  Res;
}



int  iom_Testsome1(int  InRequestCount, MPS_Request  *ReqArray,
                   int  *OutRequestCountPtr, int  *IndexArray,
                   MPI_Status  *StatusArray)
{ int           i, j, OutRequestCount;
  MPS_Request  *ReqPtr;

  if(RTL_TRACE && MPI_IORequestTrace)
     tprintf("*** iom_Testsome1: InRequestCount=%d  ReqArray=%lx\n",
             InRequestCount, (uLLng)ReqArray);

  SYSTEM(MPI_Testsome, (InRequestCount, ReqArray, OutRequestCountPtr,
                        IndexArray, StatusArray))
  OutRequestCount = *OutRequestCountPtr;  

  if(RTL_TRACE && MPI_IORequestTrace)
  {  tprintf("*** iom_Testsome1: OutRequestCount=%d\n", OutRequestCount);

     for(i=0; i < OutRequestCount; i++)
         tprintf("*** iom_Testsome1: IndexArray[%d]=%d\n",
                 i, IndexArray[i]);
  }

  /* */    /*E0401*/

  if(IAmIOProcess == 0 && SaveIOFlag)
  {  for(j=0; j < OutRequestCount; j++)
     {  ReqPtr = &ReqArray[IndexArray[j]]; /* */    /*E0402*/

        for(i=0; i < MPS_RequestCount; i++)
        {  if(MPS_RequestBuffer[i] == ReqPtr)
              break;
        }

        if(i < MPS_RequestCount)
        {  /* */    /*E0403*/

           for(i++; i < MPS_RequestCount; i++)
               MPS_RequestBuffer[i-1] = MPS_RequestBuffer[i];
           MPS_RequestCount--;
        }
     }
  }

  return  OutRequestCount;
}

#endif


#endif  /* _IOPROC_C_ */    /*E0404*/
