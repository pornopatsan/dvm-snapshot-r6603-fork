#ifndef _TRACE_C_
#define _TRACE_C_
/***************/    /*E0000*/

/********************************************\
* Install trace and built in debugging tools *
\********************************************/    /*E0001*/

void dvm_TraceInit(void)
{
   int     i, j, k, m, n, q, gztrace = 0;
   char    PrecisionString[4];
   DvmType    il;

   /* Allocate and fill in buffer to check memory
      requests and memory free */    /*E0002*/

   if(SaveAllocMem)
   {
      mac_calloc(AllocBuffer, s_ALLOCMEM *, AllocBufSize,
                 sizeof(s_ALLOCMEM), 1);
      if(AllocBuffer == NULL)
         eprintf(__FILE__,__LINE__,
                 "*** RTS err 016.000: no memory for allocation "
                 "memory buffer\n");

      for(i=0; i < AllocBufSize; i++)
      {
         AllocBuffer[i].ptr = NULL;
         AllocBuffer[i].size = 0;
      }
   }

   /* Installation of   pointer value checking when
      memory is requested */    /*E0003*/

   MinPtr = dvm_max(MinPtr, dvm_CodeFinalAddr+1);

   /* Installation of command memory checking */    /*E0004*/

   if(dvm_CodeStartAddr == 0 || dvm_CodeFinalAddr == 0 ||
      dvm_CodeStartAddr > dvm_CodeFinalAddr)
   {  EveryEventCheckCodeMem = 0;
      EveryTraceCheckCodeMem = 0;
   }

   if(EveryEventCheckCodeMem || EveryTraceCheckCodeMem)
   {  if(dvm_CodeCheckSum == 0)
         dvm_CodeCheckSum =
         dvm_CheckSum((unsigned short *)dvm_CodeStartAddr,
                      (unsigned short *)dvm_CodeFinalAddr);
      dvm_CodeCheckMem();
   }

   /* Installation of defined memory segment checking */    /*E0005*/

   if(dvm_StartAddr == 0 || dvm_FinalAddr == 0 ||
      dvm_StartAddr > dvm_FinalAddr)
   {  EveryEventCheckMem = 0;
      EveryTraceCheckMem = 0;
   }

   if(EveryEventCheckMem || EveryTraceCheckMem)
   {  if(dvm_ControlTotal == 0)
        dvm_ControlTotal=dvm_CheckSum((unsigned short *)dvm_StartAddr,
                                      (unsigned short *)dvm_FinalAddr);
      dvm_CheckMem();
   }

   /* Check belonging to other DVM objects */    /*E0006*/

#ifndef _DVM_IOPROC_

   if(IAmIOProcess == 0)
      TstObject = (byte)(TstObject | EnableDynControl);
   else
      TstObject = 0;

#else
   TstObject = 0;
#endif

   /* */    /*E0007*/

   if(MPS_CurrentProc == MPS_MasterProc && IAmIOProcess == 0)
   {
      for(i=0,j=0; j < 3; i++)
      {
         SYSTEM(sprintf, (DVM_String, "%d", i))
         SYSTEM(strcat, (DVM_String, "scan.dvm"))

         SYSTEM_RET(q, remove, (DVM_String))

         if(q != 0)
            j++;       /* */    /*E0008*/
         else
            j = 0;
      }
   }

   /**********************\
   *  Trace installation  *
   \**********************/    /*E0009*/

   events();  /* define event names */    /*E0010*/

   /* Delete old files  with trace */    /*E0011*/

   if(MPS_CurrentProc == MPS_MasterProc && DelSysTrace)
   {
      for(i=0,j=0,k=0,m=0,n=0; i < MaxProcNumber; i++)
      {
         if(IAmIOProcess)
         {
            SYSTEM(sprintf, (DVM_String, "%s%d&%d.%s", TracePath, i,
                             ProcNumberList[i], TraceFileExt))
         }
         else
         {
            SYSTEM(sprintf, (DVM_String, "%s%d_%d.%s", TracePath, i,
                             ProcNumberList[i], TraceFileExt))
         }

         if(j < 3)
         {
            SYSTEM_RET(q, remove, (DVM_String))

            if(q != 0)
               j++;  /* */    /*E0012*/
            else
               j = 0;
         }

         if(k < 3)
         {
            SYSTEM(strcat, (DVM_String, ".gz"))

            SYSTEM_RET(q, remove, (DVM_String))

            if(q != 0)
               k++;  /* */    /*E0013*/
            else
               k = 0;
         }

         if(IAmIOProcess)
         {
            SYSTEM(sprintf, (DVM_String, "%s%d&%d.%s", TracePath, i,
                             ProcNumberList[i], TraceBufferExt))
         }
         else
         {
            SYSTEM(sprintf, (DVM_String, "%s%d_%d.%s", TracePath, i,
                             ProcNumberList[i], TraceBufferExt))
         }

         if(m < 3)
         {
            SYSTEM_RET(q, remove, (DVM_String))

            if(q != 0)
               m++;  /* */    /*E0014*/
            else
               m = 0;
         }

         if(n < 3)
         {
            SYSTEM(strcat, (DVM_String, ".gz"))

            SYSTEM_RET(q, remove, (DVM_String))

            if(q != 0)
               n++;  /* */    /*E0015*/
            else
               n = 0;
         }

         if(j >= 3 && k >= 3 && m >= 3 && n >= 3)
            break;  /* */    /*E0016*/
      }
   }

   mps_Barrier(); /* to protect the trace files during opening
                     from deletion by another processor */    /*E0017*/

   /* Check if current  processor is traced */    /*E0018*/

   IsTraceProcList = (byte)(IsTraceProcList && IAmIOProcess == 0);

   if(IsTraceProcList)
   {
      /* List of traced processor internal numbers is on */    /*E0019*/

      for(i=0; i < ProcCount; i++)
          TraceProcNumber[i] = 0;
   }
   else
   {
      /* List of traced processor numbers is off */    /*E0020*/

      for(i=0; i < ProcCount; i++)
          TraceProcNumber[i] = 1;
   }

   TraceProcNumber[0] = 1; /* main processor is always traced */    /*E0021*/

   if(IsTraceProcList)
   {
     for(i=0; i < MaxProcNumber && TraceProcList[i] != -1; i++)
     {
        if(TraceProcList[i] >= MaxProcNumber || TraceProcList[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 016.009: invalid TraceProcList array "
           "(TraceProcList[%d]=%d)\n", i, TraceProcList[i]);

        TraceProcNumber[TraceProcList[i]] = 1;
     }
   }

   tprintf_time = DisableTraceTime; /* flag on subtraction of trace time
                                       from the tracing output time */    /*E0022*/

   if(TraceProcNumber[MPS_CurrentProc] == 0)
   {
      if(IAmIOProcess == 0)
         Is_DVM_TRACE = 0;  /* current processor is not traced */    /*E0023*/
      else
         Is_IO_TRACE = 0;
   }

   /* */    /*E0024*/

   if(MaxTraceFileSize <= 0)
      MaxTraceFileSize = DVMTYPE_MAX;   /*(int)(((word)(-1))>>1);*/    /*E0025*/

   if(MaxCommTraceFileSize <= 0)
      MaxCommTraceFileSize = DVMTYPE_MAX;  /*(int)(((word)(-1))>>1);*/    /*E0026*/

   MaxCommTraceFileSize = MaxCommTraceFileSize / ProcCount;
   il = dvm_min(MaxCommTraceFileSize, MaxTraceFileSize);

   if(il <= 0)
      MaxTraceFileSize =
      dvm_max(MaxCommTraceFileSize, MaxTraceFileSize);
   else
      MaxTraceFileSize = il;

   /* ----------------------------------------------------- */    /*E0027*/

   if(IAmIOProcess == 0)
      i = Is_DVM_TRACE;
   else
      i = Is_IO_TRACE;

   if(i)
   {
     /* Form array of necessary trace events */    /*E0028*/

      if(IsDisableTraceEvents)
      {
         for(i=0; i < MaxEventNumber; i++)
         {
           if(DisableTraceEvents[i] == -1)
              break; /* end of event list */    /*E0029*/

           if(DisableTraceEvents[i] < -1 ||
              DisableTraceEvents[i] > MaxEventNumber-1 ||
              (DisableTraceEvents[i] > -1 && DisableTraceEvents[i] < 4))
               epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 016.001: invalid DisableTraceEvents "
                     "array\n(DisableTraceEvents[%d]=%d)\n",
                     i, DisableTraceEvents[i]);

           IsEvent[DisableTraceEvents[i]] = 0; /* trace off */    /*E0030*/
         }
      }

      if(IsFullTraceEvents)
      {
         for(i=0; i < MaxEventNumber; i++)
         {
           if(FullTraceEvents[i] == -1)
              break; /* list of events traced in
                        extended mode is finished */    /*E0031*/

           if(FullTraceEvents[i] < -1 ||
              FullTraceEvents[i] > MaxEventNumber-1)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 016.002: invalid FullTraceEvents array\n"
                  "(FullTraceEvents[%d]=%d)\n", i, FullTraceEvents[i]);

           if(IsEvent[FullTraceEvents[i]])
              IsEvent[FullTraceEvents[i]] = 2; /* extended trace off */    /*E0032*/
         }
      }

      /* Install time precision in trace */    /*E0033*/

      if(TimePrecision < 1  ||  TimePrecision > 12)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 016.003: invalid Trace Time Precision (%d)\n",
          TimePrecision);

      SYSTEM(sprintf,(PrecisionString,"%d",TimePrecision))
      SYSTEM(strcat,(TracePrintFormat1,PrecisionString))
      SYSTEM(strcat,(TracePrintFormat2,PrecisionString))

      if(CurrentTimeTrace)
      {
         SYSTEM(strcat,(TracePrintFormat1, "lf %."))
         SYSTEM(strcat,(TracePrintFormat2, "lf %."))

         SYSTEM(strcat,(TracePrintFormat1,PrecisionString))
         SYSTEM(strcat,(TracePrintFormat2,PrecisionString))
      }

      SYSTEM(strcat,(TracePrintFormat1, "lf LINE=%-6.ld FILE=%s\n"))
      SYSTEM(strcat,(TracePrintFormat2, "lf LINE=%-6.ld FILE=%s\n"))

      /* check edges of busy memory blocks */    /*E0034*/

      if(SaveAllocMem == 0)
      {
         EveryEventCheckBound=0;
         EveryTraceCheckBound=0;
      }

      mac_malloc(dvm_blank, char *, MaxTraceStrLen, 1);

      if(dvm_blank == NULL)
         eprintf(__FILE__,__LINE__,
                 "*** RTS err 016.004: no memory for trace "
                 "event underline\n");
      for(i=0; i < MaxTraceStrLen; i++)
          dvm_blank[i] = ' ';

      /* */    /*E0035*/

      if(TraceCompressLevel == 0)
      {
         if(CompressLevel >= 0)
            TraceCompressLevel = CompressLevel;
      }

      /* Install trace output into buffer */    /*E0036*/

      if(BufferTrace != 0 && TraceBufLength != 0)
      {
         #ifndef _DVM_LLIO_
            LowDumpLevel = 0;
         #endif

         if(TraceBufLength < MaxTraceStrLen)
            epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 016.005: Trace Buffer Length < "
                     "Max Trace String Length\n");

         mac_malloc(TraceBufPtr, char *,
                    TraceBufLength + 2*MaxTraceStrLen, 1);

         if(TraceBufPtr == NULL)
            eprintf(__FILE__,__LINE__,
             "*** RTS err 016.006: no memory for system trace buffer\n");
      }

      /* Install trace output into file */    /*E0037*/

      if(FileTrace)
      {
         #ifdef _DVM_MPI_

         if(MPI_TraceFileNameNumb)
         {
            if(dvm_OneProcSign)
            {
               SYSTEM(sprintf, (TraceFileName, "%s%d_%d.%s",
                                TracePath, dvm_OneProcNum,
                                dvm_OneProcNum, TraceFileExt))
            }
            else
            {
               SYSTEM(sprintf, (TraceFileName, "%s%d_%d.%s",
                                TracePath, MPS_CurrentProcIdent,
                                MPS_CurrentProcIdent, TraceFileExt))
            }
         }
         else

         #endif
         {
            if(IAmIOProcess)
            {
               if(dvm_OneProcSign)
               {
                  SYSTEM(sprintf, (TraceFileName, "%s%d&%d.%s",
                                   TracePath, dvm_OneProcNum,
                                   dvm_OneProcNum, TraceFileExt))
               }
               else
               {
                  SYSTEM(sprintf, (TraceFileName, "%s%d&%d.%s",
                                   TracePath, MPS_CurrentProc,
                                   CurrentProcNumber, TraceFileExt))
               }
            }
            else
            {
               if(dvm_OneProcSign)
               {
                  SYSTEM(sprintf, (TraceFileName, "%s%d_%d.%s",
                                   TracePath, dvm_OneProcNum,
                                   dvm_OneProcNum, TraceFileExt))
               }
               else
               {
                  SYSTEM(sprintf, (TraceFileName, "%s%d_%d.%s",
                                   TracePath, MPS_CurrentProc,
                                   CurrentProcNumber, TraceFileExt))
               }
            }
         }

         if(IAmIOProcess == 0)
         {
            SYSTEM_RET(i, strncmp, (TraceFileExt, "ptr", 3))

            if(i == 0)
               TraceFlush = 0; /* */    /*E0038*/
         }

         /* */    /*E0039*/

         gztrace = 0;

         #ifdef _DVM_ZLIB_

         if(TraceCompressLevel >= 0)
         {
            gztrace = 1;  /* */    /*E0040*/

            SYSTEM(strcat, (TraceFileName, ".gz"))

            if(TraceCompressFlush < 0)
            {
               TraceFlush = 0;  /* */    /*E0041*/
               TraceCompressFlush = 0;
            }

            if(TraceCompressFlush == 0)
               TraceCompressFlush = Z_SYNC_FLUSH; /* */    /*E0042*/
            else
               TraceCompressFlush = Z_FULL_FLUSH;

            switch(TraceCompressLevel)
            {
               case 1:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb1"))
                        break;

               case 2:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb2"))
                        break;

               case 3:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb3"))
                        break;

               case 4:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb4"))
                        break;

               case 5:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb5"))
                        break;

               case 6:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb6"))
                        break;

               case 7:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb7"))
                        break;

               case 8:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb8"))
                        break;

               case 9:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb9"))
                        break;

               case 0:
               default: SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                   (TraceFileName, "wb"))
                        break;
            }

            if(gzDVM_TRACE_FILE == NULL)
            {
               if(FatTraceNoOpen == 1)
                  eprintf(__FILE__,__LINE__,
                          "*** RTS err 016.007: can't open system "
                          "trace out file <%s>\n",
                          TraceFileName);
               pprintf(3,"*** RTS warning 016.007: can't open DVM "
                         "trace out file <%s>\n", TraceFileName);
               FileTrace = 0;
            }
         }

         #endif

         if(gztrace == 0)
         {
            SYSTEM_RET(DVM_TRACE_FILE, fopen, (TraceFileName,
                                               OPENMODE(w)))

            if(DVM_TRACE_FILE == NULL)
            {
               if(FatTraceNoOpen == 1)
                  eprintf(__FILE__,__LINE__,
                          "*** RTS err 016.007: can't open system "
                          "trace out file <%s>\n",
                          TraceFileName);
               pprintf(3,"*** RTS warning 016.007: can't open DVM "
                         "trace out file <%s>\n", TraceFileName);
               FileTrace = 0;
            }
            else
            {
               /* */    /*E0043*/

               #ifdef _UNIX_
                  SYSTEM_RET(TRACE_FILE_HANDLE, fileno, (DVM_TRACE_FILE))
               #endif
            }
         }
      }

      /* Install trace beffering
         made by operating system */    /*E0044*/

      if(gztrace == 0)
      {
         if(FileTrace && SetTraceBuf)
         {
            mac_malloc(TraceOSBuffer, char *, BUFSIZ, 1);

            if(TraceOSBuffer == NULL)
               eprintf(__FILE__,__LINE__,
               "*** RTS err 016.008: no memory for OS trace buffer\n");

            SYSTEM(setbuf, (DVM_TRACE_FILE, TraceOSBuffer) )
         }
         else
            TraceFlush = 0;
      }

      RTL_TRACE  = (byte)(!BlockTrace);
      ALL_TRACE  = (byte)(RTL_TRACE && Is_ALL_TRACE);
      DEB_TRACE  = (byte)(RTL_TRACE && Is_DEB_TRACE);
      STAT_TRACE = (byte)(RTL_TRACE && Is_STAT_TRACE);

      IsTraceInit=1;     /* trace is initialized */    /*E0045*/
   }

   if(IAmIOProcess == 0)
      i = Is_DVM_TRACE;
   else
      i = Is_IO_TRACE;

   if(i == 0 || FullTrace == 0)
   {
      mappl_Trace   = 0;
      dopl_Trace    = 0;
      distr_Trace   = 0;
      align_Trace   = 0;
      dacopy_Trace  = 0;
      OutIndexTrace = 0;
      RedVarTrace   = 0;
      diter_Trace   = 0;
      drmbuf_Trace  = 0;
   }

   /* Install calculation time execution  of
      parallel loop iteration groups */    /*E0046*/

   PLTimeMeasure = (byte)(PLTimeTrace && RTL_TRACE);

   return;
}


/********************************************************\
* Formatted trace output at the address of argument list *
\********************************************************/    /*E0047*/

int   vtprintf(int  prefix, char  *format, va_list  arg_ptr)
{
   int    j, n = 0, j1, n1, gztrace = 0;
   DvmType   i;
   char   s;

   if(RTL_TRACE == 0 || IsTraceInit == 0)
      return 0;           /* trace is off or not installed */    /*E0048*/

   /* Out trace to screen */    /*E0049*/

   if(ScreenTrace && _SysInfoPrint && IAmIOProcess == 0)
   {  if(_SysInfoStdErr || (prefix > 1 && NoBlStdErr))
      {  if((prefix & 0x1) && ProcCount > 1)
            SYSTEM_RET(n,fprintf,(stderr,
                       "%d(%d): ", MPS_CurrentProc, CurrentProcNumber))
         SYSTEM_RET(n, vfprintf, (stderr,format,arg_ptr))

         /* Trace vatiables output in stderr stream */    /*E0050*/

         if(TraceVarAddr)
         {  switch(TraceVarType)
            {  case 1: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, *(int *)TraceVarAddr,
                       *(int *)TraceVarAddr))
                       break;
               case 2: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                       TraceVarAddr, *(long *)TraceVarAddr,
                       *(long*)TraceVarAddr))
                       break;
               case 3: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                       TraceVarAddr, *(float *)TraceVarAddr))
                       break;
               case 4: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                       TraceVarAddr, *(double *)TraceVarAddr))
                       break;
               case 5: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, (int)(*(char *)TraceVarAddr),
                       (int)(*(char *)TraceVarAddr)))
                       break;
               case 6: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, (int)(*(short *)TraceVarAddr),
                       (int)(*(short *)TraceVarAddr)))
                       break;
               case 7: SYSTEM(fprintf, (stderr,
                       "TraceVarAddr=%lx; TraceVarValue=%lld(%lx);\n",
                       TraceVarAddr, *(long long *)TraceVarAddr,
                       *(long long*)TraceVarAddr))
                       break;                       
            }
         }
      }

      if(_SysInfoStdOut || (prefix > 1 && (StdOutFile || StdOutFile)))
      {  if((prefix & 0x1) && ProcCount > 1)
            SYSTEM_RET(n, fprintf, (stdout, "%d(%d): ",
                                    MPS_CurrentProc, CurrentProcNumber))
         SYSTEM_RET(n, vfprintf, (stdout, format, arg_ptr))

         /* Trace vatiables output in stdout stream */    /*E0051*/

         if(TraceVarAddr)
         { switch(TraceVarType)
           { case 1: SYSTEM(fprintf, (stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                     TraceVarAddr, *(int *)TraceVarAddr,
                     *(int *)TraceVarAddr))
                     break;
             case 2: SYSTEM(fprintf,(stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                     TraceVarAddr, *(long *)TraceVarAddr,
                     *(long *)TraceVarAddr))
                     break;
             case 3: SYSTEM(fprintf, (stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                     TraceVarAddr, *(float *)TraceVarAddr))
                     break;
             case 4: SYSTEM(fprintf, (stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                     TraceVarAddr, *(double *)TraceVarAddr))
                     break;
             case 5: SYSTEM(fprintf, (stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                     TraceVarAddr, (int)(*(char *)TraceVarAddr),
                     (int)(*(char *)TraceVarAddr)))
                     break;
             case 6: SYSTEM(fprintf, (stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                     TraceVarAddr, (int)(*(short *)TraceVarAddr),
                     (int)(*(short *)TraceVarAddr)))
                     break;
             case 7: SYSTEM(fprintf,(stdout,
                     "TraceVarAddr=%lx; TraceVarValue=%lld(%lx);\n",
                     TraceVarAddr, *(long long *)TraceVarAddr,
                     *(long long *)TraceVarAddr))
                     break;                    
           }
         }
      }

      if(_SysInfoFile)
      {  if((prefix & 0x1) && ProcCount > 1)
            SYSTEM_RET(n,fprintf,(SysInfo,
                       "%d(%d): ",MPS_CurrentProc,CurrentProcNumber))
         SYSTEM_RET(n,vfprintf,(SysInfo,format,arg_ptr))

         /* Trace vatiables output in SysInfo stream */    /*E0052*/

         if(TraceVarAddr)
         {  switch(TraceVarType)
            {  case 1: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, *(int *)TraceVarAddr,
                       *(int *)TraceVarAddr))
                       break;
               case 2: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                       TraceVarAddr, *(long *)TraceVarAddr,
                       *(long *)TraceVarAddr))
                       break;
               case 3: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                       TraceVarAddr, *(float *)TraceVarAddr))
                       break;
               case 4: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                       TraceVarAddr, *(double *)TraceVarAddr))
                       break;
               case 5: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, (int)(*(char *)TraceVarAddr),
                       (int)(*(char *)TraceVarAddr)))
                       break;
               case 6: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                       TraceVarAddr, (int)(*(short *)TraceVarAddr),
                       (int)(*(short *)TraceVarAddr)))
                       break;
               case 7: SYSTEM(fprintf, (SysInfo,
                       "TraceVarAddr=%lx; TraceVarValue=%lld(%lx);\n",
                       TraceVarAddr, *(long long*)TraceVarAddr,
                       *(long long*)TraceVarAddr))
                       break;                       
            }
         }
      }
   }

   /* Out trace to file */    /*E0053*/

   #ifdef _DVM_ZLIB_
      j = (FileTrace &&
           (gzDVM_TRACE_FILE != NULL || DVM_TRACE_FILE != NULL));
   #else
      j = (FileTrace && DVM_TRACE_FILE != NULL);
   #endif


   if(j)
   {  /* */    /*E0054*/

      j = 1;  /* */    /*E0055*/

      if(CurrTraceFileSize >= MaxTraceFileSize)
      {  /* */    /*E0056*/

         if(TraceFileOverflowReg == 0)
         {  /* */    /*E0057*/

            CurrTraceFileSize = 0;

            gztrace = 0;

            #ifdef _DVM_ZLIB_

            if(TraceCompressLevel >= 0)
            {  gztrace = 1; /* */    /*E0058*/

               SYSTEM(gzclose, (gzDVM_TRACE_FILE))

               SYSTEM(remove, (TraceFileName))

               switch(TraceCompressLevel)
               {  case 1:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb1"))
                           break;
                  case 2:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb2"))
                           break;
                  case 3:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb3"))
                           break;
                  case 4:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb4"))
                           break;
                  case 5:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb5"))
                           break;
                  case 6:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb6"))
                           break;
                  case 7:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb7"))
                           break;
                  case 8:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb8"))
                           break;
                  case 9:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb9"))
                           break;
                  case 0:
                  default: SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                      (TraceFileName, "wb"))
                           break;
               }

               if(gzDVM_TRACE_FILE == NULL)
                  eprintf(__FILE__,__LINE__,
                          "*** RTS err 016.007: can't open system "
                          "trace out file <%s>\n", TraceFileName);
            }

            #endif

            if(gztrace == 0)
            {  SYSTEM(fclose, (DVM_TRACE_FILE))

               SYSTEM(remove, (TraceFileName))
               SYSTEM_RET(DVM_TRACE_FILE, fopen, (TraceFileName,
                                                  OPENMODE(w)))

               if(DVM_TRACE_FILE == NULL)
                  eprintf(__FILE__,__LINE__,
                          "*** RTS err 016.007: can't open system "
                          "trace out file <%s>\n", TraceFileName);

               #ifdef _UNIX_
                  SYSTEM_RET(TRACE_FILE_HANDLE, fileno, (DVM_TRACE_FILE))
               #endif
            }
         }
         else
         {  if(TraceFileOverflowReg == 1)
               j = 0; /* */    /*E0059*/
            else
            { /* */    /*E0060*/

              FileTrace = 0;

              gztrace = 0;

              #ifdef _DVM_ZLIB_

              if(TraceCompressLevel >= 0)
              {  gztrace = 1; /* */    /*E0061*/

                 SYSTEM(gzclose, (gzDVM_TRACE_FILE))
              }

              #endif

              if(gztrace == 0)
                 SYSTEM(fclose, (DVM_TRACE_FILE))

              rtl_printf("*** RTS err 240.010: "
                         "size of system trace out file %s > %ld\n",
                         TraceFileName, MaxTraceFileSize);
              mps_exit(1);
              exit(1);
            }
         }
      }

      n = 0;

      if(j != 0)
      { j = FileTraceShift * DVM_LEVEL; /* indent  for next level of
                                         embedded function calls */    /*E0062*/
        if(j < 0)
           j = 0;
        if(j > 0 && FShiftFlag != NULL)
        {  s = dvm_blank[j];
           dvm_blank[j] = '\x00';

           gztrace = 0;

           #ifdef _DVM_ZLIB_

           if(TraceCompressLevel >= 0)
           {  gztrace = 1; /* */    /*E0063*/

              SYS_CALL(gzprintf);
              CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE, "%s",
                                            dvm_blank);
              SYS_RET;
           }

           #endif

           if(gztrace == 0)
           {  SYS_CALL(fprintf);
              CurrTraceFileSize += fprintf(DVM_TRACE_FILE, "%s",
                                           dvm_blank);
              SYS_RET;
           }

           dvm_blank[j] = s;
        }

        gztrace = 0;

        #ifdef _DVM_ZLIB_

        if(TraceCompressLevel >= 0)
        {  gztrace = 1; /* */    /*E0064*/

           SYS_CALL(gzvfprintf);
           n += gzvfprintf(gzDVM_TRACE_FILE, format, arg_ptr);
           SYS_RET;
        }

        #endif

        if(gztrace == 0)
        {  SYS_CALL(vfprintf);
           n += vfprintf(DVM_TRACE_FILE, format, arg_ptr);
           SYS_RET;
        }

        CurrTraceFileSize += n;

        SYSTEM_RET(FShiftFlag, strchr, (format,'\n'))

        /* Trace vatiables output into file */    /*E0065*/

        if(TraceVarAddr && FShiftFlag != NULL)
        {  if(j > 0)
           {  s = dvm_blank[j];
              dvm_blank[j] = '\x00';

              gztrace = 0;

              #ifdef _DVM_ZLIB_

              if(TraceCompressLevel >= 0)
              {  gztrace = 1; /* */    /*E0066*/

                 SYS_CALL(gzprintf);
                 CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE, "%s",
                                               dvm_blank);
                 SYS_RET;
              }

              #endif

              if(gztrace == 0)
              {  SYS_CALL(fprintf);
                 CurrTraceFileSize += fprintf(DVM_TRACE_FILE, "%s",
                                              dvm_blank);
                 SYS_RET;
              }

              dvm_blank[j] = s;
           }


           gztrace = 0;

           #ifdef _DVM_ZLIB_

           if(TraceCompressLevel >= 0)
           {  gztrace = 1; /* */    /*E0067*/

              switch(TraceVarType)
              {  case 1:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, *(int *)TraceVarAddr,
                         *(int *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 2:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                         TraceVarAddr, *(long *)TraceVarAddr,
                         *(long *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 3:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                         TraceVarAddr, *(float *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 4:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                         TraceVarAddr, *(double *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 5:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, (int)(*(char *)TraceVarAddr),
                         (int)(*(char *)TraceVarAddr));
                         SYS_RET;

                         break;
                 case 6:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, (int)(*(short *)TraceVarAddr),
                         (int)(*(short *)TraceVarAddr));
                         SYS_RET;

                         break;
                 case 7:
                         SYS_CALL(gzprintf);
                         CurrTraceFileSize += gzprintf(gzDVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%lld(%lx);\n",
                         TraceVarAddr, *(long long*)TraceVarAddr,
                         *(long long*)TraceVarAddr);
                         SYS_RET;

                         break;                         
              }
           }

           #endif

           if(gztrace ==0)
           {  switch(TraceVarType)
              {  case 1:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, *(int *)TraceVarAddr,
                         *(int *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 2:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                         TraceVarAddr, *(long *)TraceVarAddr,
                         *(long *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 3:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                         TraceVarAddr, *(float *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 4:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                         TraceVarAddr, *(double *)TraceVarAddr);
                         SYS_RET;

                         break;
                 case 5:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, (int)(*(char *)TraceVarAddr),
                         (int)(*(char *)TraceVarAddr));
                         SYS_RET;

                         break;
                 case 6:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                         TraceVarAddr, (int)(*(short *)TraceVarAddr),
                         (int)(*(short *)TraceVarAddr));
                         SYS_RET;

                         break;
                 case 7:
                         SYS_CALL(fprintf);
                         CurrTraceFileSize += fprintf(DVM_TRACE_FILE,
                         "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                         TraceVarAddr, *(long long*)TraceVarAddr,
                         *(long long*)TraceVarAddr);
                         SYS_RET;

                         break;                         
              }
           }
        }

        if(TraceFlush)
        {
           gztrace = 0;

           #ifdef _DVM_ZLIB_

           if(TraceCompressLevel >= 0)
           {  gztrace = 1; /* */    /*E0068*/

              SYSTEM(gzflush, (gzDVM_TRACE_FILE, TraceCompressFlush))

              #ifdef _UNIX_
                 SYSTEM(sync, ())
              #endif

              if(TraceFlush > 1)
              {  SYSTEM(gzclose, (gzDVM_TRACE_FILE))

                 switch(TraceCompressLevel)
                 {  case 1:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab1"))
                             break;
                    case 2:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab2"))
                             break;
                    case 3:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab3"))
                             break;
                    case 4:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab4"))
                             break;
                    case 5:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab5"))
                             break;
                    case 6:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab6"))
                             break;
                    case 7:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab7"))
                             break;
                    case 8:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab8"))
                             break;
                    case 9:  SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab9"))
                             break;
                    case 0:
                    default: SYSTEM_RET(gzDVM_TRACE_FILE, gzopen,
                                        (TraceFileName, "ab"))
                             break;
                 }
              }
           }

           #endif

           if(gztrace == 0)
           {  SYSTEM(fflush, (DVM_TRACE_FILE))

              #ifdef _UNIX_
                 SYSTEM(fsync, (TRACE_FILE_HANDLE))
              #endif

              if(TraceFlush > 1)
              {  SYSTEM(fclose, (DVM_TRACE_FILE))
                 SYSTEM_RET(DVM_TRACE_FILE, fopen, (TraceFileName,
                                                    OPENMODE(a)))
                 #ifdef _UNIX_
                    SYSTEM_RET(TRACE_FILE_HANDLE, fileno,
                               (DVM_TRACE_FILE))
                 #endif
              }
           }
        }
      }
   }

   /* Out trace to buffer */    /*E0069*/

   if(BufferTrace == 0 || TraceBufLength == 0 || TraceBufPtr == NULL)
      return n;
   if(FullBufferStop && TraceBufFullArr[0])
      return n;

   j = BufferTraceShift * DVM_LEVEL *
       BShiftFlag;        /* indent  for next level of
                             embedded function calls */    /*E0070*/
   if(j < 0)
      j = 0;

   if(j >= MaxTraceStrLen)
   {  rtl_printf("*** RTS fatal err 240.000: wrong trace level\n");
      mps_exit(1);
      exit(1);
   }

   if(j > 0)
   {  s = dvm_blank[j];
      dvm_blank[j] = '\x00';

      SYSTEM(sprintf,
            ((char *)(TraceBufPtr+TraceBufCountArr[0]),"%s",dvm_blank))
      dvm_blank[j] = s;
   }

   SYS_CALL(vsprintf);
   n = 1 + j + vsprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j),
                        format,arg_ptr);
   SYS_RET;

   if(n > MaxTraceStrLen)
   { rtl_printf("*** RTS fatal err 240.001: trace string length > %d\n",
                MaxTraceStrLen);
     mps_exit(1);
     exit(1);
   }

   TraceBufCountArr[0] += n;

   BShiftFlag = (byte)( TraceBufPtr[TraceBufCountArr[0]-2]=='\n' );

   if(BShiftFlag)
   {  TraceBufPtr[TraceBufCountArr[0]-2]='\r';
      TraceBufPtr[TraceBufCountArr[0]-1]='\n';
   }
   else
      TraceBufPtr[TraceBufCountArr[0]-1]=' ';

   if( (i=(TraceBufCountArr[0]-TraceBufLength)) >= 0)
   {  TraceBufCountArr[0] = (word)i;
      TraceBufFullArr[0] = 1;

      for(   ; i > 0 ;i--)
          TraceBufPtr[i-1] = TraceBufPtr[TraceBufLength+i-1];
   }

   if(TraceVarAddr == 0 || BShiftFlag == 0)
      return  n-j;

   /* Trace vatiables output in buffer */    /*E0071*/

   j1 = BufferTraceShift * DVM_LEVEL; /* shift on function call
                                         nesting level */    /*E0072*/
   if(j1 < 0)
      j1 = 0;

   if(j1 >= MaxTraceStrLen)
   {  rtl_printf("*** RTS fatal err 240.000: wrong trace level\n");
      mps_exit(1);
      exit(1);
   }

   if(j1 > 0)
   {  s = dvm_blank[j1];
      dvm_blank[j1] = '\x00';

      SYSTEM(sprintf,
            ((char *)(TraceBufPtr+TraceBufCountArr[0]),"%s",dvm_blank))
      dvm_blank[j1] = s;
   }

   SYS_CALL(sprintf);

   switch(TraceVarType)
   { case 1:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                  TraceVarAddr, *(int *)TraceVarAddr,
                  *(int *)TraceVarAddr);
             break;
     case 2:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%ld(%lx);\n",
                  TraceVarAddr, *(long *)TraceVarAddr,
                  *(long *)TraceVarAddr);
             break;
     case 3:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%f;\n",
                  TraceVarAddr, *(float *)TraceVarAddr);
             break;
     case 4:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%lf;\n",
                  TraceVarAddr, *(double *)TraceVarAddr);
             break;
     case 5:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                  TraceVarAddr, (int)(*(char *)TraceVarAddr),
                  (int)(*(char *)TraceVarAddr));
             break;
     case 6:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%d(%x);\n",
                  TraceVarAddr, (int)(*(short *)TraceVarAddr),
                  (int)(*(short *)TraceVarAddr));
             break;
     case 7:
             n1 = 1 + j1 +
                  sprintf((char *)(TraceBufPtr+TraceBufCountArr[0]+j1),
                  "TraceVarAddr=%lx; TraceVarValue=%lld(%lx);\n",
                  TraceVarAddr, *(long long*)TraceVarAddr,
                  *(long long*)TraceVarAddr);
             break;             
   }

   SYS_RET;

   if(n1 > MaxTraceStrLen)
   { rtl_printf("*** RTS fatal err 240.001: trace string length > %d\n",
                MaxTraceStrLen);
     mps_exit(1);
     exit(1);
   }

   TraceBufCountArr[0] += n1;

   TraceBufPtr[TraceBufCountArr[0]-2]='\r';
   TraceBufPtr[TraceBufCountArr[0]-1]='\n';

   if( (i=(TraceBufCountArr[0]-TraceBufLength)) >= 0)
   {  TraceBufCountArr[0] = (word)i;
      TraceBufFullArr[0] = 1;

      for(   ; i > 0 ;i--)
          TraceBufPtr[i-1] = TraceBufPtr[TraceBufLength+i-1];
   }

   return  n-j;
}


/******************************************************\
* Formatted output of trace according to argument list *
\******************************************************/    /*E0073*/

int  tprintf(char  *format, ...)
{  va_list  argptr;
   int      n;

   if(RTL_TRACE == 0 || IsTraceInit == 0)
      return  0; /* trace is off or not installed */    /*E0074*/

   if(tprintf_time)
      trace_time = dvm_time();

   va_start(argptr, format);
   n = vtprintf(1, format, argptr);
   va_end(argptr);

   if(tprintf_time)
      sum_trace_time += (dvm_time() - trace_time);

   return  n;
}


/************************************************************\
*    Formatted output of trace into screen or into files     *
*  with prefix equal  to processor number or without prefix  *
\************************************************************/    /*E0075*/

int pprintf(int prefix,char *format,...)
{  va_list  argptr;
   int      n = 0;

   va_start(argptr,format);

   if(_SysInfoPrint || prefix > 1)
   {  /* This is an error messages or
         information messages are permitted */    /*E0076*/

      if(!(ScreenTrace && _SysInfoPrint) || !RTL_TRACE || !IsTraceInit)
      {  if((_SysInfoStdErr && prefix < 2) ||
            (prefix > 1 && NoBlStdErr))
         { if((prefix & 0x1) && ProcCount > 1)
           {  if(IAmIOProcess)
                 SYSTEM_RET(n, fprintf, (stderr,"%d[%d]: ",
                                         MPS_CurrentProc,
                                         CurrentProcNumber))
               else
                 SYSTEM_RET(n, fprintf, (stderr,"%d(%d): ",
                                         MPS_CurrentProc,
                                         CurrentProcNumber))
           }

           if((prefix & 0x1) || (MPS_CurrentProc == DVM_MasterProc))
           {  SYS_CALL(vfprintf);
              n += vfprintf(stderr,format,argptr);
              SYS_RET;
           }
         }

         if( (_SysInfoStdOut && prefix < 2) ||
             (prefix > 1 && (StdOutFile || StdErrFile)) )
         { if((prefix & 0x1) && ProcCount > 1)
           {  if(IAmIOProcess)
                 SYSTEM_RET(n, fprintf,
                            (stdout, "%d[%d]: ",
                            MPS_CurrentProc, CurrentProcNumber))
              else
                 SYSTEM_RET(n, fprintf,
                            (stdout, "%d(%d): ",
                            MPS_CurrentProc, CurrentProcNumber))
           }

           if((prefix & 0x1) || (MPS_CurrentProc == DVM_MasterProc))
           {  SYS_CALL(vfprintf);
              n += vfprintf(stdout, format, argptr);
              SYS_RET;
           }
         }

         if(_SysInfoFile && IAmIOProcess == 0)
         { if((prefix & 0x1) && ProcCount > 1)
              SYSTEM_RET(n,fprintf,(SysInfo,"%d(%d): ",MPS_CurrentProc,
                                            CurrentProcNumber))
           if((prefix & 0x1) || (MPS_CurrentProc == DVM_MasterProc))
           {  SYS_CALL(vfprintf);
              n += vfprintf(SysInfo,format,argptr);
              SYS_RET;
           }
         }
      }
   }

   if(RTL_TRACE && IsTraceInit)
   {  if(DisableTraceTime)
         trace_time = dvm_time();

      n = vtprintf(prefix, format, argptr);

      if(DisableTraceTime)
         sum_trace_time += (dvm_time() - trace_time);
   }

   va_end(argptr);

   return  n;
}


/*************************************************************\
*     Formatted output of trace into screen or into files     *
*  with prefix equal  to processor number and stop execution  *
\*************************************************************/    /*E0077*/

void  eprintf(char  *FileName, int  Line, char  *format,...)
{  va_list argptr;
   int offset;

   va_start(argptr,format);

   Trace.TerminationStatus = 1; /* abnormal termination. Save error string to be writen in protocol */    /*E0078*/
   offset  = sprintf(error_message,"%d[%d]: ", MPS_CurrentProc, CurrentProcNumber);
   offset += vsprintf(error_message+offset, format, argptr);
   offset += sprintf(error_message+offset,
                    "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
                    MPS_CurrentProc, CurrentProcNumber,
                    DVM_FILE[0], DVM_LINE[0]);
   sprintf(error_message+offset,
                    "%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
                    MPS_CurrentProc, CurrentProcNumber,
                    FileName, Line);

   if((ScreenTrace && _SysInfoPrint) == 0 || RTL_TRACE == 0 ||
      IsTraceInit == 0)
   {  if(NoBlStdErr)
      {  if(IAmIOProcess)
         {  SYSTEM(fprintf,
                   (stderr,"%d[%d]: ",MPS_CurrentProc,CurrentProcNumber))
            SYSTEM(vfprintf,(stderr, format, argptr))
            SYSTEM(fprintf,(stderr, "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
                   MPS_CurrentProc, CurrentProcNumber, DVM_FILE[0],
                   DVM_LINE[0]))
            SYSTEM(fprintf,(stderr,"%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
                   MPS_CurrentProc, CurrentProcNumber, FileName,Line))
         }
         else
         {  SYSTEM(fprintf,
                   (stderr,"%d(%d): ",MPS_CurrentProc,CurrentProcNumber))
            SYSTEM(vfprintf,(stderr, format, argptr))
            SYSTEM(fprintf,(stderr, "%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
                   MPS_CurrentProc, CurrentProcNumber, DVM_FILE[0],
                   DVM_LINE[0]))
            SYSTEM(fprintf,(stderr,"%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
                   MPS_CurrentProc, CurrentProcNumber, FileName,Line))
         }
      }

      if(StdOutFile || StdErrFile)
      { if(IAmIOProcess)
        {  SYSTEM(fprintf,(stdout,
           "%d[%d]: ", MPS_CurrentProc, CurrentProcNumber))
           SYSTEM(vfprintf, (stdout, format, argptr))
           SYSTEM(fprintf,(stdout,
           "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
           MPS_CurrentProc, CurrentProcNumber, DVM_FILE[0], DVM_LINE[0]))
           SYSTEM(fprintf,(stdout,
           "%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
           MPS_CurrentProc, CurrentProcNumber, FileName, Line))
        }
        else
        {  SYSTEM(fprintf,(stdout,
           "%d(%d): ", MPS_CurrentProc, CurrentProcNumber))
           SYSTEM(vfprintf, (stdout, format, argptr))
           SYSTEM(fprintf,(stdout,
           "%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
           MPS_CurrentProc, CurrentProcNumber, DVM_FILE[0], DVM_LINE[0]))
           SYSTEM(fprintf,(stdout,
           "%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
           MPS_CurrentProc, CurrentProcNumber, FileName, Line))
        }
      }

      if(_SysInfoFile && IAmIOProcess == 0)
      {  SYSTEM(fprintf,
                (SysInfo,"%d(%d): ",MPS_CurrentProc,CurrentProcNumber))
         SYSTEM(vfprintf,(SysInfo,format,argptr))
         SYSTEM(fprintf,(SysInfo,"%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
             MPS_CurrentProc,CurrentProcNumber,DVM_FILE[0],DVM_LINE[0]))
         SYSTEM(fprintf,(SysInfo,"%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
             MPS_CurrentProc,CurrentProcNumber,FileName,Line))
      }
   }

   va_end(argptr);

   va_start(argptr, format);
   vtprintf(1, format, argptr);
   va_end(argptr);

   tprintf("USRFILE=%s;  USRLINE=%ld;\n", DVM_FILE[0], DVM_LINE[0]);
   tprintf("SYSFILE=%s;  SYSLINE=%d;\n",FileName,Line);

   RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

     if(1 /*CallDbgCond*/    /*E0079*/ /*EnableTrace && dvm_OneProcSign*/    /*E0080*/)
        SYSTEM(MPI_Finalize, ())
     else
        dvm_exit(1);

#else

     dvm_exit(1);

#endif

}


/*****************************************************************\
* Formatted output onto screen and into trace (prefix is equal to *
*       processor number) or no output and stop execution         *
\*****************************************************************/    /*e0081*/

void epprintf(int prefix, char *FileName, int Line, char *format,...)
{  va_list argptr;
   int offset;

   Trace.TerminationStatus = 1; /* abnormal termination. Save error string to be writen in protocol */    /*E0082*/
   offset  = sprintf(error_message,"%d[%d]: ", MPS_CurrentProc, CurrentProcNumber);

   va_start(argptr,format);
   offset += vsprintf(error_message+offset, format, argptr);
   va_end(argptr);

   offset += sprintf(error_message+offset,
                    "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
                    MPS_CurrentProc, CurrentProcNumber,
                    DVM_FILE[0], DVM_LINE[0]);
   sprintf(error_message+offset,
                    "%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
                    MPS_CurrentProc, CurrentProcNumber,
                    FileName, Line);

   if((ScreenTrace && _SysInfoPrint) == 0 || RTL_TRACE == 0 ||
      IsTraceInit == 0)
   {  if(NoBlStdErr)
      {  if(prefix && ProcCount > 1)
         {
            va_start(argptr,format);
            if(IAmIOProcess)
            {  SYSTEM(fprintf,
                      (stderr,"%d[%d]: ", MPS_CurrentProc,
                                          CurrentProcNumber))
               SYSTEM(vfprintf,(stderr, format, argptr))
               SYSTEM(fprintf, (stderr,
                                "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
                                MPS_CurrentProc, CurrentProcNumber,
                                DVM_FILE[0], DVM_LINE[0]))
               SYSTEM(fprintf, (stderr,
                                "%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
                                MPS_CurrentProc, CurrentProcNumber,
                                FileName, Line))
            }
            else
            {  SYSTEM(fprintf,
                      (stderr, "%d(%d): ", MPS_CurrentProc,
                                           CurrentProcNumber))
               SYSTEM(vfprintf, (stderr, format, argptr))
               SYSTEM(fprintf, (stderr,
                                "%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
                                MPS_CurrentProc, CurrentProcNumber,
                                DVM_FILE[0], DVM_LINE[0]))
               SYSTEM(fprintf, (stderr,
                                "%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
                                MPS_CurrentProc, CurrentProcNumber,
                                FileName, Line))
            }
            va_end(argptr);
         }
         else
         {  if(MPS_CurrentProc == DVM_MasterProc)
            {
               va_start(argptr,format);
               SYSTEM(vfprintf, (stderr, format, argptr))
               va_end(argptr);
               SYSTEM(fprintf,(stderr, "USRFILE=%s;  USRLINE=%ld;\n",
                               DVM_FILE[0], DVM_LINE[0]))
               SYSTEM(fprintf,(stderr, "SYSFILE=%s;  SYSLINE=%d;\n",
                               FileName, Line))
            }
         }
      }

      if(StdOutFile || StdErrFile)
      { if(prefix && ProcCount > 1)
        {
          va_start(argptr,format);
          if(IAmIOProcess)
          {  SYSTEM(fprintf, (stdout,
             "%d[%d]: ", MPS_CurrentProc, CurrentProcNumber))
             SYSTEM(vfprintf, (stdout, format, argptr))
             SYSTEM(fprintf, (stdout,
                              "%d[%d]: USRFILE=%s;  USRLINE=%ld;\n",
                              MPS_CurrentProc, CurrentProcNumber,
                              DVM_FILE[0], DVM_LINE[0]))
             SYSTEM(fprintf, (stdout,
                              "%d[%d]: SYSFILE=%s;  SYSLINE=%d;\n",
                              MPS_CurrentProc, CurrentProcNumber,
                              FileName, Line))
          }
          else
          {  SYSTEM(fprintf, (stdout,
             "%d(%d): ", MPS_CurrentProc, CurrentProcNumber))
             SYSTEM(vfprintf, (stdout, format, argptr))
             SYSTEM(fprintf, (stdout,
                              "%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
                              MPS_CurrentProc, CurrentProcNumber,
                              DVM_FILE[0], DVM_LINE[0]))
             SYSTEM(fprintf, (stdout,
                              "%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
                              MPS_CurrentProc, CurrentProcNumber,
                              FileName, Line))
          }
          va_end(argptr);
        }
        else
        { if(MPS_CurrentProc == DVM_MasterProc)
          {
            va_start(argptr,format);
            SYSTEM(vfprintf, (stdout, format, argptr))
            va_end(argptr);
            SYSTEM(fprintf, (stdout, "USRFILE=%s;  USRLINE=%ld;\n",
                             DVM_FILE[0], DVM_LINE[0]))
            SYSTEM(fprintf, (stdout,
                             "SYSFILE=%s;  SYSLINE=%d;\n",FileName,Line))
          }
        }
      }

      if(_SysInfoFile && IAmIOProcess == 0)
      {  if(prefix && ProcCount > 1)
         {  SYSTEM(fprintf,
                (SysInfo,"%d(%d): ",MPS_CurrentProc,CurrentProcNumber))
            va_start(argptr,format);
            SYSTEM(vfprintf,(SysInfo,format,argptr))
            va_end(argptr);
            SYSTEM(fprintf,
                (SysInfo,"%d(%d): USRFILE=%s;  USRLINE=%ld;\n",
                 MPS_CurrentProc,CurrentProcNumber,
                 DVM_FILE[0],DVM_LINE[0]))
            SYSTEM(fprintf,
                 (SysInfo,"%d(%d): SYSFILE=%s;  SYSLINE=%d;\n",
                  MPS_CurrentProc,CurrentProcNumber,FileName,Line))
         }
         else
         {  if(MPS_CurrentProc == DVM_MasterProc)
            {
               va_start(argptr,format);
               SYSTEM(vfprintf,(SysInfo,format,argptr))
               va_end(argptr);
               SYSTEM(fprintf,(SysInfo,"USRFILE=%s;  USRLINE=%ld;\n",
                               DVM_FILE[0],DVM_LINE[0]))
               SYSTEM(fprintf,(SysInfo,"SYSFILE=%s;  SYSLINE=%d;\n",
                               FileName,Line))
            }
         }
      }
   }

   va_start(argptr, format);
   vtprintf(1, format, argptr);
   va_end(argptr);

   tprintf("USRFILE=%s;  USRLINE=%ld;\n", DVM_FILE[0], DVM_LINE[0]);
   tprintf("SYSFILE=%s;  SYSLINE=%d;\n", FileName, Line);

   RTS_Call_MPI = 1;

   if (EnableTrace)
	Trace.TerminationStatus = 1; /* abnormal termination. Save error string to be writen in protocol */

#ifdef _MPI_PROF_TRAN_

   if(1 /*CallDbgCond*/    /*E0083*/ /*EnableTrace && dvm_OneProcSign*/    /*E0084*/)
      SYSTEM(MPI_Finalize, ())
   else
      dvm_exit(1);

#else

   dvm_exit(1);

#endif

}


/********************************************************\
*  Output trace for event with number equal to 'number'  *
\********************************************************/    /*E0085*/

void  dvm_trace(int number, char *format,...)
{  char     EventNumber[5];
   char    *txt_ptr, s;
   int      i, event_number;
   va_list  argptr;
   double   dt, ct;

   if(RTL_TRACE == 0 || IsTraceInit == 0)
      return;             /* trace is off or not installed */    /*E0086*/

   if(DisableTraceTime)
   {  trace_time = dvm_time();
      tprintf_time = 0;
   }

   if(number > MaxEventNumber)
      event_number = 0;
   else
      event_number = number;

   if(DVM_LEVEL > MaxTraceLevel || IsEvent[event_number] == 0 ||
      DVM_LEVEL > MaxEventLevel[event_number])
   {  if(EveryTraceCheckCodeMem)
         dvm_CodeCheckMem();
      if(EveryTraceCheckMem)
         dvm_CheckMem();
      if(EveryTraceCheckBound)
         check_buf_bound();
   }
   else
   {  if(number > MaxEventNumber)
      {  SYSTEM(sprintf,(EventNumber,"%d",number))
         txt_ptr = EventNumber;
      }
      else
         txt_ptr = EventName[number];

      va_start(argptr,format);

      dt = delta_for_dvm_trace(event_number); /* time elapsed from
                                                 previous event */    /*E0087*/

      if(_CurrentTimeTrace)
         ct = DVM_TIME2 - Init_dvm_time; /* current system time */    /*E0088*/
      else
         ct = 0.;

      if(KeyWordName)
      {  if(CurrentTimeTrace)
            i = tprintf(TracePrintFormat1, txt_ptr, dt, ct,
                        DVM_LINE[DVM_LEVEL], DVM_FILE[DVM_LEVEL]);
         else
            i = tprintf(TracePrintFormat1, txt_ptr, dt,
                        DVM_LINE[DVM_LEVEL], DVM_FILE[DVM_LEVEL]);
      }
      else
      {  if(CurrentTimeTrace)
            i = tprintf(TracePrintFormat2, txt_ptr, dt, ct,
                        DVM_LINE[DVM_LEVEL], DVM_FILE[DVM_LEVEL]);
         else
            i = tprintf(TracePrintFormat2, txt_ptr, dt,
                        DVM_LINE[DVM_LEVEL], DVM_FILE[DVM_LEVEL]);
      }

      if(IsEvent[event_number] > 1 || FullTrace)
      {  if(PreUnderLine)
         {  s = PreUnderLining[i-2];
            PreUnderLining[i-2] = '\x00';
            tprintf("%s\n", PreUnderLining);
            PreUnderLining[i-2] = s;
         }

         if(event_number != Event_MeasureStart &&
            event_number != Event_MeasureFinish)
            vtprintf(1, format, argptr);
         else
         {  if(event_number == Event_MeasureStart)
               tprintf("Index=%d;\n", MeasureIndex);
            else
               tprintf("Index=%d;  Time=%lf;  TraceTime=%lf;\n",
                       MeasureIndex+1, MeasureStartTime[MeasureIndex+1],
                       MeasureTraceTime[MeasureIndex+1]);
         }

         if(PostUnderLine)
         {  s = PostUnderLining[i-2];
            PostUnderLining[i-2] = '\x00';
            tprintf("%s\n", PostUnderLining);
            PostUnderLining[i-2] = s;
         }
      }

      va_end(argptr);

      if(EveryEventCheckCodeMem || EveryTraceCheckCodeMem)
         dvm_CodeCheckMem();
      if(EveryEventCheckMem || EveryTraceCheckMem)
         dvm_CheckMem();
      if(EveryEventCheckBound || EveryTraceCheckBound)
         check_buf_bound();
   }

   if(DisableTraceTime)
   {  sum_trace_time += (dvm_time() - trace_time);
      tprintf_time = 1;
   }

   return;
}


/*******************************************\
*     Support function for  dvm_trace.      *
* Do everything needed with time and output *
*       time elapsed from last event        *
\*******************************************/    /*E0089*/

double delta_for_dvm_trace(int event_number)
{ double  delta;

  if(Is_Curr_dvm_time)
  {  DVM_TIME2 = Curr_dvm_time;
     Is_Curr_dvm_time = 0;
  }
  else
  {  DVM_TIME2 = dvm_time();
  }

  delta = DVM_TIME2-DVM_TIME1-sum_trace_time; /* time elapsed from
                                                 last event */    /*E0090*/
  if(delta <= 0.)
  {  sum_trace_time = -delta - 0.000001;
     delta = 0.000001;
  }
  else
     sum_trace_time = 0.;

  SystemDeltaSum += delta;       /* to support system
                                    life time */    /*E0091*/
  if(UserSumFlag)
     UserDeltaSum += delta; /* to the time of user
                               program execution */    /*E0092*/
  DVM_TIME1 = DVM_TIME2;

  if(MeasureIndex > -1)
     MeasureTraceTime[MeasureIndex] += delta;

  if(event_number == Event_MeasureStart)
  {  /* MeasureStart */    /*E0093*/
     MeasureIndex++;
     if(MeasureIndex > MaxMeasureIndex)
        eprintf(__FILE__,__LINE__,
             "*** RTS fatal err: Measure Index > Max Measure Index\n");
     MeasureStartTime[MeasureIndex] = DVM_TIME2;
     MeasureTraceTime[MeasureIndex] = 0.0;
  }

  if(event_number == Event_MeasureFinish)
  {  /* MeasureFinish */    /*E0094*/
     if(MeasureIndex < 0)
        eprintf(__FILE__,__LINE__,
                "*** RTS fatal err: Measure Index < 0\n");
     if(MeasureIndex > 0)
        MeasureTraceTime[MeasureIndex-1] +=
        MeasureTraceTime[MeasureIndex];
     MeasureStartTime[MeasureIndex] = DVM_TIME2 -
                                      MeasureStartTime[MeasureIndex];
     MeasureIndex--;
  }

  return  delta;
}


/******************************************************************\
* Compare checksum of domain given in parameters with standard one *
\******************************************************************/    /*E0095*/

void dvm_CheckMem(void)
{  unsigned short S;

   if((S=dvm_CheckSum((unsigned short *)dvm_StartAddr,
                      (unsigned short *)dvm_FinalAddr)) !=
                      dvm_ControlTotal)
   {  pprintf(3,"*** RTS err 250.000: invalid CheckSum\n");
      eprintf(__FILE__,__LINE__,
           "dvm_CheckSum=%lx new_CheckSum=%x\n", dvm_ControlTotal, S);
   }

   return;
}


/*******************************************************\
* Compare checksum of command memory  with standard one *
\*******************************************************/    /*E0096*/

void dvm_CodeCheckMem(void)
{  unsigned short S;

   if((S=dvm_CheckSum((unsigned short *)dvm_CodeStartAddr,
                      (unsigned short *)dvm_CodeFinalAddr)) !=
                      dvm_CodeCheckSum)
   {  pprintf(3,"*** RTS err 250.001: invalid code CheckSum\n");
      pprintf(3,"dvm_CodeStartAddr=%lx  dvm_CodeFinalAddr=%lx\n",
                dvm_CodeStartAddr,dvm_CodeFinalAddr);
      eprintf(__FILE__,__LINE__,
              "dvm_CodeCheckSum=%lx  new_CodeCheckSum=%x\n",
              dvm_CodeCheckSum, (int)S);
   }

   return;
}


/************************************\
* Count checksum of the given domain *
\************************************/    /*E0097*/

unsigned short dvm_CheckSum(unsigned short *First,unsigned short *Final)
{  unsigned short   *MemPtr;
   uLLng              Res = 0;

   Final = (unsigned short *)((uLLng)Final & 0xfffffffeL);

   for(MemPtr=First; MemPtr <= Final; MemPtr++)
   {  Res += *MemPtr;
      Res = (Res >> 16) + (Res & 0xffffL);
   }

   return (unsigned short)Res;
}


/*****************************************\
* Print final information, count checksum *
*     close trace file, stop tracing      *
\*****************************************/    /*E0098*/

void  trace_Done(void)
{
    DvmType     count1, count2;
   int      i, gztrace = 0;
   char     buf[128];

   /* Output statistics of function calls */    /*E0099*/

   if(CallCountPrint && _SysInfoPrint)
   {
      pprintf(1, " \n");
      pprintf(1, "Event  Counts:\n\n");

      if(CallCountPrint > 1)
         for(i=0; i < MaxEventNumber; i++)
         {
             if(EventCount0[i] == 0.)
                pprintf(1, "%-32s  =  %ld\n",
                           EventName[i], EventCount[i]);
             else
                pprintf(1, "%-32s  =  %ld(%ld)\n",
                           EventName[i], EventCount[i],
                           EventCount[i]+EventCount0[i]);
         }
      else
      {
         for(i=0; i < MaxEventNumber; i++)
         {
            if(EventCount[i] != 0 || EventCount0[i] != 0)
            {
               if(EventCount0[i] == 0.)
                  pprintf(1, "%-32s  =  %ld\n",
                             EventName[i], EventCount[i]);
               else
                  pprintf(1, "%-32s  =  %ld(%ld)\n",
                             EventName[i], EventCount[i],
                             EventCount[i]+EventCount0[i]);
            }
         }
      }

      pprintf(1," \n");
   }

   /* Print version number */    /*E0100*/

   if((VersFinishPrint || VersFullFinishPrint) && _SysInfoPrint)
      PrintVers(VersFullFinishPrint, 1);

   if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
      PrintVers(1, 0);

   /* Print number of allocated memory blocks and
      total size of allocated memory */    /*E0101*/

   if(SaveAllocMem && AllocBuffer != NULL)
   {
      check_count(&count1, &count2);

      if(EndProgMemoryPrint && _SysInfoPrint)
         pprintf(1,"Allocate Counter=%ld; Allocate Memory=%ld;\n",
                   count1,count2);
      else
      {
         if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
            tprintf("Allocate Counter=%ld; Allocate Memory=%ld;\n",
                    count1,count2);
      }
   }

   /* Print not free distributed arrays count */    /*E0102*/

   if(EndProgObjectPrint && _SysInfoPrint)
   {
      pprintf(1, "DACount=%8d; DVMObjCount=%8d;\n",
                  DACount, DVMObjCount);

      for(i=0; i < dvm_max(DACount,DVMObjCount); i++)
      {
         if(i < DACount)
            SYSTEM(sprintf,(buf,"        %8lx",(uLLng)DAHeaderAddr[i]))
         else
            SYSTEM(sprintf,(buf,"                "))

         if(i < DVMObjCount)
            SYSTEM(sprintf,(&buf[16],"              %8lx",DVMObjRef[i]))
         else
            SYSTEM(sprintf,(&buf[16],"                      "))

         pprintf(1,"%s\n", buf);
      }
   }
   else
   {
      if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
      {
         tprintf("DACount=%8d; DVMObjCount=%8d;\n",
                 DACount, DVMObjCount);

         for(i=0; i < dvm_max(DACount,DVMObjCount); i++)
         {
            if(i < DACount)
               tprintf("        %8lx",(uLLng)DAHeaderAddr[i]);
            else
               tprintf("                ");

            if(i < DVMObjCount)
               tprintf("              %8lx\n",DVMObjRef[i]);
            else
               tprintf("                      \n");
         }
      }
   }

   EndProgrammCheckSum();  /* check checksums
                              at the end of execution */    /*E0103*/

   /* Print times */    /*E0104*/

   SystemFinishTime = dvm_time();
   UserFinishTime = dvm_max(UserFinishTime - UserStartTime, 0.0);


   if(TimeExpendPrint >= 0 && _SysInfoPrint)
   {
      if(IAmIOProcess == 0)
         i = Is_DVM_TRACE;
      else
         i = Is_IO_TRACE;

      if(i)
      {
            pprintf(1,"    TaskTime=%lf;   TaskTraceTime=%lf;\n",
                      UserFinishTime, UserDeltaSum);

         #ifdef _F_TIME_

            if(TimeExpendPrint > 0)
               pprintf(1,"FunctionTime=%lf;\n", DVMFTime);

         #endif

            pprintf(1,"  SystemTime=%lf; SystemTraceTime=%lf;\n",
                    SystemFinishTime - SystemStartTime, SystemDeltaSum);
      }
      else
      {
            pprintf(1,"    TaskTime=%lf;   SystemTime=%lf;\n",
                    UserFinishTime, SystemFinishTime - SystemStartTime);

         #ifdef _F_TIME_

            if(TimeExpendPrint > 0)
               pprintf(1,"FunctionTime=%lf;\n", DVMFTime);

         #endif
      }

      #ifdef _DVM_MPI_

      if(IAmIOProcess == 0 && IOProcessCount != 0)
         pprintf(1,"  ServerTime=%lf;\n", ServerProcTime);

      #endif
   }
   else
   {
      if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
      {
            tprintf("    TaskTime=%lf;   TaskTraceTime=%lf;\n",
                    UserFinishTime, UserDeltaSum);

         #ifdef _F_TIME_
            if(TimeExpendPrint > 0)
               tprintf("FunctionTime=%lf;\n", DVMFTime);

         #endif

            tprintf("  SystemTime=%lf; SystemTraceTime=%lf;\n",
                    SystemFinishTime - SystemStartTime, SystemDeltaSum);

         #ifdef _DVM_MPI_

         if(IAmIOProcess == 0 && IOProcessCount != 0)
            tprintf("  ServerTime=%lf;\n", ServerProcTime);

         #endif
      }
      else
         tprintf("\nTaskTime=%lf;\n", UserFinishTime);
   }

   /* */    /*E0105*/

   if(_SendRecvTime && SendRecvTimePrint && _SysInfoPrint)
   {
      if(SendCallCount > 0)
         CommTime = SendCallTime / SendCallCount;
      else
         CommTime = 0.;

      pprintf(1,
      "SendTime   MeanSendTime MaxSendTime MinSendTime SendCall\n");
      pprintf(1,
      "%-10lf %-10lf   %-10lf  %-10lf  %-d\n",
      SendCallTime, CommTime, MaxSendCallTime, MinSendCallTime,
      SendCallCount);

      if(RecvCallCount > 0)
         CommTime = RecvCallTime / RecvCallCount;
      else
         CommTime = 0.;

      pprintf(1,
      "RecvTime   MeanRecvTime MaxRecvTime MinRecvTime RecvCall\n");
      pprintf(1,
      "%-10lf %-10lf   %-10lf  %-10lf  %-d\n",
      RecvCallTime, CommTime, MaxRecvCallTime, MinRecvCallTime,
      RecvCallCount);
   }

   /* ----------------------------------------------------------- */    /*E0106*/

   #ifdef _F_TIME_

   if(TimeExpendPrint > 1 && _SysInfoPrint)
   {
      int     j;
      double  ProductTime, LostTime, CallCount, SynchrTime,
              TotalProduct, TotalLost, TotalCall, TotalSynchr;

      pprintf(1," \n");

      if(TimeExpendPrint < 4)
      {
         pprintf(1,
         "GROUP NAME     CALL COUNT   PRODUCT TIME   LOST TIME   "
         "SYNCHR TIME\n");

         for(j=0,TotalProduct=0,TotalLost=0,TotalCall=0,TotalSynchr=0;
             j < StatGrpCount; j++)
         {
            for(i=0,ProductTime=0,LostTime=0,CallCount=0,SynchrTime=0;
                i < StatGrpCount; i++)
            {
               if(TimeExpendPrint == 3)
               {
                  if(j == MsgPasGrp)
                     SynchrTime += TaskInter[i][j].ProductTime;
                  else
                     ProductTime += TaskInter[i][j].ProductTime;
                  LostTime    += TaskInter[i][j].LostTime;
                  CallCount   += TaskInter[i][j].CallCount;
               }
               else
               {
                  if(i == MsgPasGrp)
                     SynchrTime += TaskInter[j][i].ProductTime;
                  else
                     ProductTime += TaskInter[j][i].ProductTime;

                  LostTime    += TaskInter[j][i].LostTime;
                  CallCount   += TaskInter[j][i].CallCount;
               }
            }

            pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf    %9.2lf\n",
                 GrpName[j],CallCount,ProductTime,LostTime,SynchrTime);

            TotalProduct += ProductTime;
            TotalLost    += LostTime;
            TotalCall    += CallCount;
            TotalSynchr  += SynchrTime;
         }

         pprintf(1," \n");
         pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf    %9.2lf\n",
                 "TOTAL",TotalCall,TotalProduct,TotalLost,TotalSynchr);
         pprintf(1," \n");
      }
      else
      {
         pprintf(1,"%s:\n\n", StatGrpName);
         pprintf(1,
         "GROUP NAME     CALL COUNT   PRODUCT TIME   LOST TIME   "
         "SYNCHR TIME\n");

         for(i=0,TotalProduct=0,TotalLost=0,TotalCall=0,TotalSynchr=0;
             i < StatGrpCount; i++)
         {
            if(TimeExpendPrint == 4)
            {
               TotalLost += TaskInter[i][StatGrpNumber].LostTime;
               TotalCall += TaskInter[i][StatGrpNumber].CallCount;

               if(StatGrpNumber == MsgPasGrp)
               {
                 TotalSynchr+=TaskInter[i][StatGrpNumber].ProductTime;
                 pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf"
                           "    %9.2lf\n",
                         GrpName[i],
                         TaskInter[i][StatGrpNumber].CallCount,
                         0.f,
                         TaskInter[i][StatGrpNumber].LostTime,
                         TaskInter[i][StatGrpNumber].ProductTime);
               }
               else
               {
                 TotalProduct+=TaskInter[i][StatGrpNumber].ProductTime;
                 pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf"
                           "    %9.2lf\n",
                         GrpName[i],
                         TaskInter[i][StatGrpNumber].CallCount,
                         TaskInter[i][StatGrpNumber].ProductTime,
                         TaskInter[i][StatGrpNumber].LostTime,
                         0.f);
               }
            }
            else
            {
               TotalLost    += TaskInter[StatGrpNumber][i].LostTime;
               TotalCall    += TaskInter[StatGrpNumber][i].CallCount;

               if(i == MsgPasGrp)
               {
                 TotalSynchr+=TaskInter[StatGrpNumber][i].ProductTime;
                 pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf"
                           "    %9.2lf\n",
                         GrpName[i],
                         TaskInter[StatGrpNumber][i].CallCount,
                         0.f,
                         TaskInter[StatGrpNumber][i].LostTime,
                         TaskInter[StatGrpNumber][i].ProductTime);
               }
               else
               {
                 TotalProduct+=TaskInter[StatGrpNumber][i].ProductTime;
                 pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf"
                           "    %9.2lf\n",
                         GrpName[i],
                         TaskInter[StatGrpNumber][i].CallCount,
                         TaskInter[StatGrpNumber][i].ProductTime,
                         TaskInter[StatGrpNumber][i].LostTime,
                         0.f);
               }
            }
         }

         pprintf(1," \n");
         pprintf(1,"%-15s%9.2lf    %9.2lf     %9.2lf    %9.2lf\n",
                "TOTAL",TotalCall, TotalProduct, TotalLost, TotalSynchr);
         pprintf(1," \n");
      }
   }

   #endif

   /* ---------------------------- */    /*E0107*/

   if(RTL_TRACE)
      ( RTL_CALL, dvm_trace(DVM_Trace_Finish," \n"), DVM_RET );

   /* One more dump of standatd streams in files */    /*E0108*/

   if(IAmIOProcess == 0)
   {
      SYSTEM(fflush,(stdout))
      SYSTEM(fflush,(stderr))

      #ifdef _UNIX_
         SYSTEM(sync, ())
      #endif
   }

   gztrace = 0;

   #ifdef _DVM_ZLIB_

   if(TraceCompressLevel >= 0)
   {
      gztrace = 1; /* */    /*E0109*/

      if(gzDVM_TRACE_FILE != NULL)
         SYSTEM(gzclose, (gzDVM_TRACE_FILE)) /* */    /*E0110*/
   }

   #endif

   if(gztrace == 0)
   {
      if(DVM_TRACE_FILE != NULL)
         SYSTEM(fclose, (DVM_TRACE_FILE)) /* close trace file */    /*E0111*/
   }

   RTL_TRACE = 0;                      /* trace KAPUT */    /*E0112*/

   return;
}


/****************************\
* Dump trace saved in buffer *
\****************************/    /*E0113*/

byte  trace_Dump(int  ProcNumber, word  TrcBufCount, int  TrcBufFull)
{
   int      DumpHandle, Res, gztrace;
   FILE    *DumpFile;
   char     TraceFileName[128];
   char    *StringPtr;
   DvmType     ByteCount = 0;
   double   TimeCount;

#ifdef _DVM_ZLIB_
   gzFile   gzDumpFile;
#endif


   if(IsTraceInit           &&
      BufferTraceUnLoad     &&
      BufferTrace           &&
      TraceBufLength        &&
      TraceBufPtr != NULL)

   {
      TimeCount = dvm_time();

      #ifdef _DVM_MPI_

      if(MPI_TraceFileNameNumb)
      {
         if(dvm_OneProcSign)
         {
            SYSTEM(sprintf,(TraceFileName, "%s%d_%d.%s",
                            TracePath, dvm_OneProcNum,
                            dvm_OneProcNum, TraceBufferExt))
         }
         else
         {
            SYSTEM(sprintf,(TraceFileName, "%s%d_%d.%s",
                            TracePath, MPS_CurrentProcIdent,
                            MPS_CurrentProcIdent, TraceBufferExt))
         }
      }
      else

      #endif
      {

         if(IAmIOProcess)
         {
            if(dvm_OneProcSign)
            {
               SYSTEM(sprintf,(TraceFileName, "%s%d&%d.%s",
                               TracePath, dvm_OneProcNum,
                               dvm_OneProcNum, TraceBufferExt))
            }
            else
            {
               SYSTEM(sprintf,(TraceFileName, "%s%d&%d.%s",
                               TracePath, ProcNumber,
                               ProcNumberList[ProcNumber], TraceBufferExt))
            }
         }
         else
         {
            if(dvm_OneProcSign)
            {
               SYSTEM(sprintf,(TraceFileName, "%s%d_%d.%s",
                               TracePath, dvm_OneProcNum,
                               dvm_OneProcNum, TraceBufferExt))
            }
            else
            {
               SYSTEM(sprintf,(TraceFileName, "%s%d_%d.%s",
                               TracePath, ProcNumber,
                               ProcNumberList[ProcNumber], TraceBufferExt))
            }
         }
      }

      gztrace = 0;

      #ifdef _DVM_ZLIB_

      if(TraceCompressLevel >= 0)
      {
         gztrace = 1; /* */    /*E0114*/

         Res = gzDFOpen(TraceFileName, &gzDumpFile);/* */    /*E0115*/
      }

      #endif

      if(gztrace == 0)
         Res = DFOpen(TraceFileName, &DumpHandle,
                      &DumpFile);    /* open file
                                                     to dump trace */    /*E0116*/
      if(Res == 0)
         return 0;  /* file has not been open */    /*E0117*/

      if(TrcBufFull)
      {
         TraceBufPtr[(word)TraceBufLength] = '\x00';

         SYSTEM_RET(StringPtr,strstr,(&TraceBufPtr[TrcBufCount],"\r\n"))

         if(StringPtr != NULL)
         {
            ByteCount = &TraceBufPtr[(word)TraceBufLength]-StringPtr-2;

            gztrace = 0;

            #ifdef _DVM_ZLIB_

            if(TraceCompressLevel >= 0)
            {
               gztrace = 1; /* */    /*E0118*/

               Res = gzDFWrite(StringPtr+2, ByteCount, gzDumpFile,
                               TraceFileName);
            }

            #endif

            if(gztrace == 0)
               Res = DFWrite(StringPtr+2, ByteCount, DumpHandle,
                             DumpFile, TraceFileName);

            if(Res == 0)
               return 0;  /* writing in the file failed */    /*E0119*/
	 }
      }

      ByteCount += TrcBufCount;

      gztrace = 0;

      #ifdef _DVM_ZLIB_

      if(TraceCompressLevel >= 0)
      {
         gztrace = 1; /* */    /*E0120*/

         Res = gzDFWrite(TraceBufPtr, TrcBufCount, gzDumpFile,
                         TraceFileName);
      }

      #endif

      if(gztrace == 0)
         Res = DFWrite(TraceBufPtr, TrcBufCount, DumpHandle, DumpFile,
                       TraceFileName);
      if(Res == 0)
         return 0;  /* writing in the file failed */    /*E0121*/

      gztrace = 0;

      #ifdef _DVM_ZLIB_

      if(TraceCompressLevel >= 0)
      {
         gztrace = 1; /* */    /*E0122*/

         SYSTEM(gzclose, (gzDumpFile))
      }

      #endif

      if(gztrace == 0)
         DFClose(DumpHandle, DumpFile);  /* close file
                                        with dumped trace */    /*E0123*/

      if(TraceClosePrint && _SysInfoPrint)
	 rtl_printf("the trace out file <%s> has been closed; "
                    "size=%ld; time=%lf;\n",
	             TraceFileName, ByteCount, dvm_time() - TimeCount);
   }

   return  1;
}


/*****************************************\
* Open file to dump trace saved in buffer *
\*****************************************/    /*E0124*/

byte  DFOpen(char  *FileName, int  *DumpHandlePtr, FILE  **DumpFilePtr)
{
  int DumpHandle = *DumpHandlePtr;

#ifdef _DVM_LLIO_

  if(LowDumpLevel)
  {
     #ifdef _i860_
        SYSTEM_RET(*DumpHandlePtr, open,
                   (FileName, O_CREAT | O_RDWR | O_BINARY))
     #else
        #ifdef _UNIX_
           SYSTEM_RET(*DumpHandlePtr, open,
                      (FileName, O_CREAT | O_RDWR,
                       S_IREAD | S_IWRITE))
        #else
           SYSTEM_RET(*DumpHandlePtr, open,
                      (FileName, O_CREAT | O_RDWR | O_BINARY,
                       S_IREAD | S_IWRITE))
        #endif
     #endif

     if(*DumpHandlePtr == -1)
     {  rtl_printf(
        "*** RTS err 022.000: can't open trace out file <%s>\n",
        FileName);
        return 0;
     }
  }
  else

#endif

  {
     SYSTEM_RET(*DumpFilePtr, fopen, (FileName,"wb"))

     if(*DumpFilePtr == NULL)
     { rtl_printf(
       "*** RTS err 022.000: can't open trace out file <%s>\n",
       FileName);

       return 0;
     }
  }

  return  1;
}


#ifdef _DVM_ZLIB_

byte  gzDFOpen(char  *FileName, gzFile  *gzDumpFilePtr)
{
  SYSTEM(strcat, (FileName, ".gz"))

  switch(TraceCompressLevel)
  {
     case 1:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb1"))
              break;
     case 2:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb2"))
              break;
     case 3:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb3"))
              break;
     case 4:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb4"))
              break;
     case 5:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb5"))
              break;
     case 6:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb6"))
              break;
     case 7:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb7"))
              break;
     case 8:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb8"))
              break;
     case 9:  SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb9"))
              break;
     case 0:
     default: SYSTEM_RET(*gzDumpFilePtr, gzopen, (FileName, "wb"))
              break;
  }

  if(*gzDumpFilePtr == NULL)
  {
    rtl_printf("*** RTS err 022.000: can't open trace out file <%s>\n",
               FileName);

    return 0;
  }

  return  1;
}

#endif


/***************************************\
* Close file with trace saved in buffer *
\***************************************/    /*e0125*/

void  DFClose(int DumpHandle,FILE *DumpFile)
{
  int  w = DumpHandle;

#ifdef _DVM_LLIO_
  if(LowDumpLevel)
     SYSTEM(close,(DumpHandle))
  else
#endif
     SYSTEM(fclose,(DumpFile))
  return;
}


/**********************************\
* Write byte array into trace file *
\**********************************/    /*E0126*/

byte  DFWrite(char  *ArrayPtr, DvmType  Count, int  DumpHandle,
              FILE  *DumpFile, char  *FileName)
{ int  MaxInt, rc = 0, nobj, remainder, w = DumpHandle;

  if(Count > 0)
  {  MaxInt = INT_MAX;   /*(int)(((word)(-1))>>1);*/    /*E0127*/

#ifdef _DVM_LLIO_
     if(LowDumpLevel)
     {  for( ; Count > MaxInt; Count -= MaxInt, ArrayPtr += MaxInt)
        {  SYSTEM_RET(rc, write, (DumpHandle, ArrayPtr, MaxInt))

           if(rc == -1)
           {  rtl_printf("*** RTS err 022.001: can't write on trace "
                         "out file <%s>\n",FileName);
              return 0;
           }
        }

	SYSTEM_RET(rc, write, (DumpHandle, ArrayPtr, (word)Count))

        if(rc == -1)
        {  rtl_printf("*** RTS err 022.001: can't write on trace "
                      "out file <%s>\n", FileName);
           return 0;
        }
     }
     else
#endif
     {  nobj = (int)(Count/MaxInt);

        if(nobj)
	{  SYSTEM_RET(rc, fwrite, (ArrayPtr, MaxInt, nobj, DumpFile))

	   if(rc < nobj)
	   {  rtl_printf("*** RTS err 022.001: can't write on trace "
                         "out file <%s>\n", FileName);
              return 0;
	   }
	}

        remainder = (int)(Count%MaxInt);

	if(remainder)
	{  SYSTEM_RET(rc, fwrite,
                      (ArrayPtr + MaxInt*nobj, remainder, 1, DumpFile))

	   if(rc < 1)
	   {  rtl_printf("*** RTS err 022.001: can't write on trace "
                         "out file <%s>\n", FileName);
              return 0;
	   }
	}
     }
  }

  return  1;
}


#ifdef _DVM_ZLIB_

byte  gzDFWrite(char  *ArrayPtr, DvmType  Count, gzFile  gzDumpFile,
                char  *FileName)
{ int   MaxInt, rc = 0, nobj, remainder;

  if(Count > 0)
  {  MaxInt = INT_MAX;   /*(int)(((word)(-1))>>1);*/    /*E0128*/

     nobj = (int)(Count / MaxInt);

     if(nobj)
     {  SYSTEM_RET(rc, gzwrite, ( gzDumpFile, (voidp)ArrayPtr,
                                  (unsigned)(MaxInt*nobj) ))

        if(rc < nobj)
	{  rtl_printf("*** RTS err 022.001: can't write on trace "
                      "out file <%s>\n", FileName);
           return 0;
	}
     }

     remainder = (int)(Count%MaxInt);

     if(remainder)
     {  SYSTEM_RET(rc, gzwrite,
                   (gzDumpFile, (voidp)(ArrayPtr + MaxInt*nobj),
                    (unsigned)remainder))

        if(rc < 1)
        {  rtl_printf("*** RTS err 022.001: can't write on trace "
                      "out file <%s>\n", FileName);
           return 0;
        }
     }
  }

  return  1;
}

#endif


/****************************************\
* Check checksum at the end of execution *
\****************************************/    /*E0129*/

void EndProgrammCheckSum(void)
{
  if((dvm_CodeStartAddr!=0) && (dvm_CodeFinalAddr!=0) &&
     (dvm_CodeStartAddr<=dvm_CodeFinalAddr) && (dvm_CodeCheckSum!=0))
  {  if(dvm_CheckSum((unsigned short *)dvm_CodeStartAddr,
                     (unsigned short *)dvm_CodeFinalAddr) ==
        dvm_CodeCheckSum)
        {  if(EndProgCheckSumPrint && _SysInfoPrint)
              pprintf(1,"End of programm: Code Check Sum is rigth\n");
           else
              tprintf("End of programm: Code Check Sum is rigth\n");
	}
     else
	pprintf(3,"*** RTS err 022.002: End of programm: Code Check "
		  "Sum is wrong\n");
  }

  if((dvm_StartAddr!=0) && (dvm_FinalAddr!=0) &&
     (dvm_StartAddr<=dvm_FinalAddr) && (dvm_ControlTotal!=0))
  {  if(dvm_CheckSum((unsigned short *)dvm_StartAddr,
		     (unsigned short *)dvm_FinalAddr) ==
	dvm_ControlTotal)
	{  if(EndProgCheckSumPrint && _SysInfoPrint)
	      pprintf(1,"End of programm: Control Total is rigth\n");
	else
	      tprintf("End of programm: Control Total is rigth\n");
	}
     else
	pprintf(3,"*** RTS err 022.003: End of programm: "
		  "Control Total is wrong\n");
  }

  return;
}


/************************************************************\
* Check if it is necessary to print detailed information for *
*               the tevent number EventNumber                *
\************************************************************/    /*E0130*/

int TstTraceEvent(int EventNumber)
{
  if(RTL_TRACE == 0)
     return 0; /* trace off */    /*E0131*/

  if(IsEvent[EventNumber] == 0)
     return 0; /* event off */    /*E0132*/

  if(DVM_LEVEL > MaxTraceLevel || DVM_LEVEL > MaxEventLevel[EventNumber])
     return 0; /* current level of function call nesting is more than
                  maximum trace level */    /*E0133*/

  return (FullTrace || IsEvent[EventNumber] > 1);
}


/**********************************************************\
* Functions to communicate to support system file name and *
*      string number of the point in user program          *
\**********************************************************/    /*E0134*/

DvmType  __callstd  fname_(char  *FileNamePtr, int  FileNameLength)
{

  DVM_FILE[0] = FileNamePtr;
  return  FileNameLength;
}



DvmType  __callstd  lnumb_(DvmType  *LineNumberPtr)
{
  DVM_LINE[0] = *LineNumberPtr;
  return  0;
}



DvmType  __callstd  dvmlf_(DvmType  *LineNumberPtr, char  *FileNamePtr, int  FileNameLength)
{
  DVM_FILE[0] = FileNamePtr;
  DVM_LINE[0] = *LineNumberPtr;
  return  FileNameLength;
}


/**************************\
*  Trace on/off functions  *
\**************************/    /*E0135*/

void   __callstd tron_(void)
{
  DVMFTimeStart(call_tron_);

  if(IAmIOProcess == 0)
  {  if(Is_DVM_TRACE)
        RTL_TRACE = 1;
  }
  else
  {  if(Is_IO_TRACE)
        RTL_TRACE = 1;
  }

  ALL_TRACE  = (byte)(RTL_TRACE && Is_ALL_TRACE);
  DEB_TRACE  = (byte)(RTL_TRACE && Is_DEB_TRACE);
  STAT_TRACE = (byte)(RTL_TRACE && Is_STAT_TRACE);
  PLTimeMeasure = (byte)(PLTimeTrace && RTL_TRACE);

  if(RTL_TRACE)
     dvm_trace(call_tron_," \n");

  (DVM_RET);

  DVMFTimeFinish(call_tron_);
  return;
}



void   __callstd troff_(void)
{
  DVMFTimeStart(call_troff_);

  if(RTL_TRACE)
     dvm_trace(call_troff_," \n");

  RTL_TRACE  = 0;
  ALL_TRACE  = 0;
  DEB_TRACE  = 0;
  STAT_TRACE = 0;
  PLTimeMeasure = 0;

  (DVM_RET);

  DVMFTimeFinish(call_troff_);
  return;
}


/***************************************************\
*  tuning of initial system tracing and             *
*  redirection of stdout and stderr flows into file *
\***************************************************/    /*E0136*/

void  systrace_set(int  argc, char  *argv[], int  IntProcNumber)
{
  int      i, j;
  char     FileName[256];
  FILE    *F;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0137*/

  for(; i < argc; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "itr", 3))

     if(j == 0)
     {
        /* parameter defining initial system tracing found */    /*E0138*/

        InitSysTrace = 1;   /* flag of started initial system
                               tracing */    /*E0139*/
        IsStdOutFile = 1;   /* flag: there was stdout redirection */    /*E0140*/
        IsStdErrFile = 1;   /* falg: there was stderr redirection */    /*E0141*/
        StdOutFile = 1;     /* flag: stdout redirected */    /*E0142*/
        StdErrFile = 1;     /* flag: stderr redirected */    /*E0143*/

        if(i < (argc-1) && argv[i+1][0] != Minus)
        {
           /* Folder specified */    /*E0144*/

           SYSTEM_RET(j, sprintf, (FileName, "%s%d.dvm",
                                   argv[i+1], IntProcNumber))
        }
        else
        {
           SYSTEM_RET(j, sprintf, (FileName, "%d.dvm", IntProcNumber))
        }

        FileName[j] = '\x00';

        SYSTEM(remove, (FileName)) /* deletion of old file */    /*E0145*/

        SYSTEM(strcpy, (CurrStdOutFileName, FileName))
        SYSTEM(strcpy, (CurrStdErrFileName, FileName))

        SYSTEM_RET(F, freopen, (FileName, OPENMODE(w), stdout))

        if(F == NULL)
           SYSTEM(fprintf, (stderr, "can not open stdout file <%s>\n",
                            FileName))

        SYSTEM_RET(F, freopen, (FileName, OPENMODE(w), stderr))

        if(F == NULL)
           SYSTEM(fprintf, (stderr, "can not open stderr file <%s>\n",
                            FileName))

        /* Input into stdout flow of command line */    /*E0146*/

        SYSTEM(fprintf, (stdout, "argc = %d\n\n", argc))

        for(j=0; j < argc; j++)
            SYSTEM(fprintf, (stdout, "argv[%d] = %s\n", j, argv[j]))

        SYSTEM(fprintf, (stdout, "\n"))

        SYSTEM(fflush,(stdout))
        SYSTEM(fflush,(stderr))

        #ifdef _UNIX_
           SYSTEM(sync, ())
        #endif

        break;
     }
  }

  return;
}


/******************************************************\
*  Function of input for initial system tracing        *
\******************************************************/    /*E0147*/

void  sysprintf(char  *FileName, int  Line, char  *format, ...)
{ va_list  argptr;

  if(InitSysTrace == 0)
     return;

  SYSTEM(fprintf, (stdout, "\n*** InitSysTrace:   FILE=%s  LINE=%d\n",
                           FileName, Line))
  va_start(argptr, format);
  SYSTEM(vfprintf, (stdout, format, argptr))
  va_end(argptr);

  SYSTEM(fprintf, (stdout, "\n"))

  SYSTEM(fflush,(stdout))
  SYSTEM(fflush,(stderr))

  #ifdef _UNIX_
     SYSTEM(sync, ())
  #endif

  return;
}


/* */    /*E0148*/

#ifdef _DVM_MPI_

void  DVM_Prof_Init1(void)  /* */    /*E0149*/
{
   int          i;
   commInfo    *commInfoPtr1, *commInfoPtr2;

   if(MPI_ProfInitSign == 0)
   {
      MPI_TraceRoutine = 0;
      MPI_DynAnalyzer = 0;
   }

   #ifndef _DVM_MPI2_

      MPI_TraceRoutine = 0;

   #endif

   #ifndef _DVM_MPI2_

      MPI_DynAnalyzer = 0;

   #endif

   if(MPI_TraceRoutine || MPI_DynAnalyzer)
   {
      dopl_MPI_Test = 0;
      MPITestAfterSend = 0;
      MPITestAfterRecv = 0;

      /* */    /*E0150*/

      dvm_AllocStruct(s_COLLECTION, RequestColl);
      dvm_AllocStruct(s_COLLECTION, ReqStructColl);
      *RequestColl   = coll_Init(100, 100, NULL);
      *ReqStructColl = coll_Init(100, 100, NULL);

      /* */    /*E0151*/

      dvm_AllocStruct(s_COLLECTION, CommColl);
      dvm_AllocStruct(s_COLLECTION, CommStructColl);
      *CommColl       = coll_Init(50, 50, NULL);
      *CommStructColl = coll_Init(50, 50, NULL);

      /* */    /*E0152*/

      dvm_AllocStruct(commInfo, commInfoPtr1);
      coll_Insert(CommStructColl, commInfoPtr1);
      coll_Insert(CommColl, MPI_COMM_WORLD_1);

      commInfoPtr1->proc   = MPS_CurrentProcIdent;
      commInfoPtr1->pcount = MPS_ProcCount;
      mac_malloc(commInfoPtr1->plist, int *, MPS_ProcCount*sizeof(int), 0);

      for(i=0; i < MPS_ProcCount; i++)
          commInfoPtr1->plist[i] = i;

      /* */    /*E0153*/

      dvm_AllocStruct(commInfo, commInfoPtr2);
      coll_Insert(CommStructColl, commInfoPtr2);
      coll_Insert(CommColl, DVM_COMM_WORLD);

      commInfoPtr2->proc   = MPI_CurrentProcIdent;
      commInfoPtr2->pcount = (int)ProcCount;
      mac_malloc(commInfoPtr2->plist, int *, ProcCount*sizeof(int), 0);

      if(IAmIOProcess)
         for(i=0; i < ProcCount; i++)
             commInfoPtr2->plist[i] = IOProcessNumber[i];
      else
         for(i=0; i < ProcCount; i++)
             commInfoPtr2->plist[i] = ApplProcessNumber[i];
   }

   if(MPI_TraceRoutine == 0)
      return;

   /* */    /*E0154*/

   if(MPI_TraceReg >= 0)
   {
      /* */    /*E0155*/

      Is_DVM_TRACE = 1;    /* */    /*E0156*/
      Is_IO_TRACE = 1;     /* */    /*E0157*/
      IsTraceProcList = 0; /* */    /*E0158*/
      BlockTrace = 0;      /* */    /*E0159*/

      /* */    /*E0160*/

      TraceBufLength       = MPI_TraceBufSize;
      MaxTraceFileSize     = MPI_TraceFileSize;
      MaxCommTraceFileSize = MPI_TotalTraceFileSize;

      if(MPI_TraceLevel != 0 && IOProcessCount != 0)
         MPI_TraceFileNameNumb = 1; /* */    /*E0161*/

      if(DVM_TraceOff)
      {
         /* */    /*E0162*/

         Is_ALL_TRACE = 0;
         Is_DEB_TRACE = 0;
         Is_STAT_TRACE = 0;
         Is_IOFun_TRACE = 0;
         UserCallTrace = 0;

         BufferTraceShift=0;
         FileTraceShift=0;

         TraceFileOverflowReg = 2;

         mappl_Trace = 0;
         dopl_Trace = 0;
         distr_Trace = 0;
         align_Trace = 0;
         dacopy_Trace = 0;
         OutIndexTrace = 0;
         RedVarTrace = 0;
         diter_Trace = 0;
         drmbuf_Trace = 0;
         LoadWeightTrace = 0;
         WeightArrayTrace = 0;
         AcrossTrace = 0;
         MsgPartitionTrace = 0;
         MsgScheduleTrace = 0;
         MPI_AlltoallTrace = 0;
         MPI_ReduceTrace = 0;
         DAConsistTrace = 0;
         MPI_MapAMTrace = 0;
         CrtPSTrace = 0;
         MsgCompressTrace = 0;
         MPI_RequestTrace = 0;
         MPI_IORequestTrace = 0;

         PrintBufferByteCount = 0;

         TraceVarAddr = 0;

         dopl_dyn_GetLocalBlock = 0;
         dyn_GetLocalBlock_Trace = 0;

         for(i=0; i < MaxEventNumber; i++)
             IsEvent[i] = 0;

         SysProcessNameTrace = 0;
      }
   }

   return;
}



void  DVM_Prof_Init2(void)  /* */    /*E0163*/
{
   int         i, j;
   commInfo   *commInfoPtr;

   if(MPI_TraceRoutine && MPI_ProfInitSign && RTL_TRACE)
      MPI_BotsulaProf = 1;
   if(MPI_DynAnalyzer && MPI_ProfInitSign)
      MPI_BotsulaDeb = 1;

   if(MPI_BotsulaProf == 0)
   {
      MPI_TraceMsgChecksum = 0;

   }

   if(MPI_BotsulaDeb == 0)
   {
      MPI_DebugMsgChecksum = 0;
      MPI_DebugBufChecksum = 0;
   }

   if(MPI_BotsulaProf || MPI_BotsulaDeb)
   {
      /* */    /*E0164*/

      dvm_AllocStruct(commInfo, commInfoPtr);
      coll_Insert(CommStructColl, commInfoPtr);
      coll_Insert(CommColl, DVM_VMS->PS_MPI_COMM);

      commInfoPtr->proc   = MPI_CurrentProcIdent;
      commInfoPtr->pcount = (int)ProcCount;
      mac_malloc(commInfoPtr->plist, int *, ProcCount*sizeof(int), 0);

      if(IAmIOProcess)
         for(i=0; i < ProcCount; i++)
             commInfoPtr->plist[i] = IOProcessNumber[i];
      else
         for(i=0; i < ProcCount; i++)
             commInfoPtr->plist[i] = ApplProcessNumber[i];

      if(MPI_BotsulaProf)
      {
         /* */    /*E0165*/

         tprintf("$MPI_TraceReg=%d$\n", (int)MPI_TraceReg);
         tprintf("$MPI_TraceTime=%d$\n", (int)MPI_TraceTime);
         tprintf("$MPI_TraceTimeReg=%d$\n", (int)MPI_TraceTimeReg);
         tprintf("$MPI_TraceFileLine=%d$\n", (int)MPI_TraceFileLine);
         tprintf("$MPI_TraceAll=%d$\n", (int)MPI_TraceAll);
         tprintf("$MPI_SlashOut=%d$\n", (int)MPI_SlashOut);
         tprintf("$MPI_TraceMsgChecksum=%d$\n",
                 (int)MPI_TraceMsgChecksum);
         tprintf("$MPI_DebugMsgChecksum=%d$\n",
                 (int)MPI_DebugMsgChecksum);
         tprintf("$MPI_DebugBufChecksum=%d$\n",
                 (int)MPI_DebugBufChecksum);

         /* */    /*E0166*/

         tprintf("$MPS_ProcCount=%d\tMPS_CurrentProcIdent=%d\t"
                 "IAmIOProcess=%d\n"
                 "ProcCount=%ld\tMPI_CurrentProcIdent=%d\n"
                 "MPI_COMM_WORLD=%u\tDVM_COMM_WORLD=%u\t"
                 "PS_MPI_COMM=%u\n",
                 MPS_ProcCount, MPS_CurrentProcIdent, IAmIOProcess,
                 ProcCount, MPI_CurrentProcIdent,
                 MPI_COMM_WORLD_1, DVM_COMM_WORLD, DVM_VMS->PS_MPI_COMM);

         tprintf("plist=\n");

         if(IAmIOProcess)
         {
            for(i=0,j=0; i < ProcCount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               if(i < ProcCount - 1)
                  tprintf("%d,", IOProcessNumber[i]);
               else
                  tprintf("%d", IOProcessNumber[i]);

               j++;
           }
         }
         else
         {
            for(i=0,j=0; i < ProcCount; i++)
            {
               if(j == ListCount)
               {  j = 0;
                  tprintf("\n");
               }

               if(i < ProcCount - 1)
                  tprintf("%d,", ApplProcessNumber[i]);
               else
                  tprintf("%d", ApplProcessNumber[i]);

               j++;
            }
         }

         tprintf("$\n");

         /* */    /*E0167*/

         /* C */    /*E0168*/

         #ifdef _DVM_MPI2_

         tprintf("$C-Type: MPI_CHAR=%d\tMPI_UNSIGNED_CHAR=%d\t"
                 "MPI_BYTE=%d\tMPI_SHORT=%d\n"
                 "MPI_UNSIGNED_SHORT=%d\tMPI_INT=%d\tMPI_UNSIGNED=%d\t"
                 "MPI_LONG=%d\n"
                 "MPI_UNSIGNED_LONG=%d\tMPI_FLOAT=%d\tMPI_DOUBLE=%d\t"
                 "MPI_LONG_DOUBLE=%d\n"
                 "MPI_LONG_LONG_INT=%d\tMPI_PACKED=%d\tMPI_LB=%d\t"
                 "MPI_UB=%d\n"
                 "MPI_FLOAT_INT=%d\tMPI_DOUBLE_INT=%d\tMPI_LONG_INT=%d\t"
                 "MPI_SHORT_INT=%d\n"
                 "MPI_2INT=%d\tMPI_LONG_DOUBLE_INT=%d$\n",
                 MPI_CHAR, MPI_UNSIGNED_CHAR, MPI_BYTE, MPI_SHORT,
                 MPI_UNSIGNED_SHORT, MPI_INT, MPI_UNSIGNED, MPI_LONG,
                 MPI_UNSIGNED_LONG, MPI_FLOAT, MPI_DOUBLE,
                 MPI_LONG_DOUBLE,
                 MPI_LONG_LONG_INT, MPI_PACKED, MPI_LB, MPI_UB,
                 MPI_FLOAT_INT, MPI_DOUBLE_INT, MPI_LONG_INT,
                 MPI_SHORT_INT, MPI_2INT, MPI_LONG_DOUBLE_INT);

         /* Forntran */    /*E0169*/

         tprintf("$F-Type: MPI_COMPLEX=%d\tMPI_DOUBLE_COMPLEX=%d\t"
                 "MPI_LOGICAL=%d\tMPI_REAL=%d\n"
                 "MPI_DOUBLE_PRECISION=%d\tMPI_INTEGER=%d\t"
                 "MPI_2INTEGER=%d\n"
                 "MPI_2COMPLEX=%d\tMPI_2DOUBLE_COMPLEX=%d\t"
                 "MPI_2REAL=%d\tMPI_2DOUBLE_PRECISION=%d\n"
                 "MPI_CHARACTER=%d$\n",
                 MPI_COMPLEX, MPI_DOUBLE_COMPLEX, MPI_LOGICAL, MPI_REAL,
                 MPI_DOUBLE_PRECISION, MPI_INTEGER, MPI_2INTEGER,
                 MPI_2COMPLEX, MPI_2DOUBLE_COMPLEX, MPI_2REAL,
                 MPI_2DOUBLE_PRECISION, MPI_CHARACTER);

         #endif

         /* */    /*E0170*/

         tprintf(" \n");
      }
   }

   return;
}

#endif


#endif /* _TRACE_C_ */    /*E0171*/
