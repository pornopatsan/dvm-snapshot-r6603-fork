#ifndef _INPUTPAR_C_
#define _INPUTPAR_C_
/******************/    /*E0000*/

/***************************************************\
*  Function inputs parameters from file *FileName.  *
*  Return  0, on successful  input or error code    * 
*  ( > 0 )  in case of erroneous parameters.        *  
*  In the last case corresponding message displays. * 
\***************************************************/    /*E0001*/

int  InputPar(char  *FileName, s_PARAMETERKEY  s_Key[],
              byte  FileOpenErr, byte  ParamErr)
{
#ifdef _STRUCT_STAT_
  struct stat    FileInfo;
#endif

  DvmType           FileSize = 50000;
  byte           SAVE_RTL_TRACE;
  DVMFILE       *fpar = NULL;
  char          *buf, *str1, *str2, *addr;
  DvmType           err = 0;
  DvmType           strl, i, j, k, l, m, n, ip, ind, vind;
  double         tmpdbl, op1, op2 = 1.1;
  float          tmpflt;
  int            tmpint;
  DvmType           tmplng;
  char          *KeyPtr, *BufPtr;
  DvmType           Buf4Beg, Buf4End;
  char           Buf1End, bufsave;
  DvmType           ind1, ind2, index, indmax;
  byte           CondTrue = TRUE;
  DvmType           KeyIndex; /* key word index value */    /*E0002*/

  InputParCheckSum = 0;  /* checksum of parameter file */    /*E0003*/

/************************************************\
* Open file with parameters and  allocate memory *
\************************************************/    /*E0004*/

#ifdef _STRUCT_STAT_

   for(i=1; i <= ParFileOpenCount; i++)
   {  tmpint = (RTL_CALL, dvm_stat(FileName, &FileInfo));

      #ifndef _i860_
         tmpint = tmpint || !(FileInfo.st_mode & S_IFREG);
      #endif

      if(tmpint)
      {
         if(i < ParFileOpenCount)
         {  /* Number of attempts to inquire  file size is not exhausted */    /*E0005*/

            op1 = 1.1;
      
            for(j=0; j < 1000000; j++)
                op1 /= op2; /* temporary delay */    /*E0006*/
            continue; /* to the next attempt to inquire file size*/    /*E0007*/
         }

         if(FileOpenErr)
         {  SAVE_RTL_TRACE = RTL_TRACE;

            if(IAmIOProcess == 0)
               RTL_TRACE = Is_DVM_TRACE;
            else
               RTL_TRACE = Is_IO_TRACE;

            if(FileOpenErr == 2)
               eprintf(__FILE__,__LINE__,
               "*** RTS err 012.000: parameter file <%s> "
               "does not exist\n", FileName);

            pprintf(3,"*** RTS warning 012.001: parameter file <%s> "
                      "does not exist\n", FileName);

            RTL_TRACE = SAVE_RTL_TRACE;
         }

         return 2;  /* parameter file does not exist */    /*E0008*/
      }

      break;   /* parameter file exists */    /*E0009*/
   }

   FileSize = FileInfo.st_size; /* parameter file size */    /*E0010*/

#endif   /* _STRUCT_STAT_ */    /*E0011*/
      
  for(i=0; i <= ParFileOpenCount; i++)
  {   fpar = ( RTL_CALL, dvm_fopen(FileName, OPENMODE(r)) );

      if(fpar == NULL)
      {
         if(i < ParFileOpenCount)
         {  /* Number of attempts to open file is not exhausted*/    /*E0012*/

            op1 = 1.1;
      
            for(j=0; j < 1000000; j++)
                op1 /= op2; /* temporary delay */    /*E0013*/
            continue; /*  to the next attempt to open the file */    /*E0014*/
         }

         if(FileOpenErr)
         {  SAVE_RTL_TRACE = RTL_TRACE;

            if(IAmIOProcess == 0) 
               RTL_TRACE = Is_DVM_TRACE;
            else
               RTL_TRACE = Is_IO_TRACE;

            if(FileOpenErr == 2)
               eprintf(__FILE__,__LINE__,
               "*** RTS err 012.002: can't open "
               "parameter file <%s>\n", FileName);

            pprintf(3,"*** RTS warning 012.003: can't open "
                      "parameter file <%s>\n", FileName);

            RTL_TRACE = SAVE_RTL_TRACE;
         }

         return 1; /* failed to open parameter file */    /*E0015*/
      }

      break; /* parameter file is open */    /*E0016*/
   }

   if(InputParPrint && _SysInfoPrint && IAmIOProcess == 0 &&
      DVM_TraceOff == 0)
   { if(_SysInfoStdErr)
        ( RTL_CALL, dvm_void_fprintf(DVMSTDERR,
                    "Input parameter file <%s>\n", FileName) );

     if(_SysInfoStdOut)
        ( RTL_CALL, dvm_void_printf("Input parameter file <%s>\n",
                                    FileName) );
     if(_SysInfoFile)
        ( RTL_CALL, dvm_void_fprintf(&DVMSysInfo,
                    "Input parameter file <%s>\n", FileName) );
   } 

   if(MPI_TraceRoutine == 0 || DVM_TraceOff == 0)
      tprintf("Input parameter file <%s>\n", FileName);

   for(tmplng=FileSize+1; ; tmplng += FileSize)
   {  mac_malloc(buf, char *, tmplng, 1);

      if(buf == NULL )
      {  ( RTL_CALL, dvm_fclose(fpar) );

         if(IAmIOProcess == 0)
            RTL_TRACE = Is_DVM_TRACE;
         else
            RTL_TRACE = Is_IO_TRACE;

         epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                 "*** RTS fatal err 012.004: no "
                 "memory for parameter buffer (parameter file %s)\n",
                 FileName);
      }

/*****************************\
*  Read file with parameters  *
\*****************************/    /*E0017*/

      strl = ( RTL_CALL, dvm_fread(buf, 1, (int)tmplng, fpar) );

      if(strl < tmplng)
         break;
      mac_free(&buf);

      ( RTL_CALL , dvm_rewind(fpar) );
   }

buf[strl] = '\x00';

/* Checksum calculation */    /*E0018*/

if(StdStart && ParFileCheckSum)
   for(i=0; i < strl; i++)
       InputParCheckSum += buf[i];

/* Substitute  reserved symbols  for blanks */    /*E0019*/

SYS_CALL(strchr);
addr = buf;
while((addr=strchr(addr,'\r')) != NULL)
      *addr = ' ';
addr = buf;
while((addr=strchr(addr,'\n')) != NULL)
      *addr = ' ';
SYS_RET;


( RTL_CALL, dvm_fclose(fpar) );

/*
if(strl == 0) 
{  pprintf(3,"*** RTS warning 012.005: parameter "
             "file <%s> is empty\n", FileName);
   mac_free(&buf);
   return 0;
}
*/    /*E0020*/  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0021*/

/* Message on file reading completion */    /*E0022*/

if(EndReadParPrint && _SysInfoPrint && IAmIOProcess == 0)
{ if(_SysInfoStdErr)
    ( RTL_CALL, dvm_void_fprintf(DVMSTDERR,
                "End of reading parameter file <%s>\n",FileName) );

  if(_SysInfoStdOut)
    ( RTL_CALL, dvm_void_printf("End of reading parameter file <%s>\n",
                                FileName) );
  if(_SysInfoFile)
    ( RTL_CALL, dvm_void_fprintf(&DVMSysInfo,
                "End of reading parameter file <%s>\n",FileName) );
}

/*******************\
* Comments deleting *
\*******************/    /*E0023*/  

str1 = buf;

while(CondTrue)
{  SYSTEM_RET(str1, strstr, (str1, "/*"))

   if(str1 == NULL)
      break;   /* no more comments */    /*E0024*/

   SYSTEM_RET(str2, strstr, (str1+2, "*/"))

   if(str2 == NULL)
   {  mac_free(&buf);

      if(IAmIOProcess == 0)
         RTL_TRACE = Is_DVM_TRACE;
      else
         RTL_TRACE = Is_IO_TRACE;

      epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 012.006: no end of comments "
               "(parameter file <%s>)\n", FileName);
   }

   while(str1 != str2+2)
   {  *str1 = ' ';
      str1++;
   }
}

/*******************************************************************\
*  Locate key words and parameters  and put them to given address.  *
*  Distribute indexes in buffer buf:                                *
*                                                                   *
*    i   kl   m       n        j                                    *
*    key = parameter           ;                                    *
*       =             ,                                             *
*                     ;                                             *
\*******************************************************************/    /*E0025*/ 

SYSTEM_RET(i, strspn, (buf, " "))

while(i < strl && err == 0)
{  SYSTEM_RET(tmpint, strcspn, (&buf[i], ";~"))
   if((j = i + tmpint) == i)
   {  err = 7;
      break;
   }

   SYSTEM_RET(tmpint, strcspn, (&buf[i], " =["))

   k = i + tmpint;

   if(k >= j || k == i)
   {  err = 8;
      break;
   }

   /* Search for the key word in array of structure with parameters */    /*E0026*/

   if (MinParSym >= sizeof(DvmType))
   {  Buf1End = buf[k - 2];
      Buf4Beg = *(DvmType *)&buf[i];
      Buf4End = *(DvmType *)&buf[k - sizeof(DvmType)];
      ind1 = k - i - 2;
      ind2 = ind1 - (sizeof(DvmType)-2);

      for(ip=0; s_Key[ip].bsize != 0; ip++)
      { indmax = s_Key[ip].NameLen;

        if(indmax != tmpint)
           continue; /* */    /*E0027*/

        KeyPtr = s_Key[ip].pName;

        if(Buf1End != *(KeyPtr + ind1))
           continue;

        if (*(DvmType *)(KeyPtr + ind2) != Buf4End)
           continue;

        if (*(DvmType *)KeyPtr != Buf4Beg)
           continue;

        BufPtr = buf + i;

        for(index=0; index < indmax; index++,BufPtr++,KeyPtr++)
        {  if(*BufPtr == *KeyPtr)
              continue;

           break;
        }

        if(index == indmax)
           break;
      }
   }
   else
   {  if(MinParSym > 1)
      {  Buf1End = buf[k-2];
         ind1 = k - i - 2;

         for(ip=0; s_Key[ip].bsize != 0; ip++)
         { indmax = s_Key[ip].NameLen;

           if(indmax != tmpint)
              continue; /* */    /*E0028*/

           KeyPtr = s_Key[ip].pName;

           if(Buf1End != *(KeyPtr + ind1))
              continue;

           BufPtr = buf + i;

           for(index=0; index < indmax; index++,BufPtr++,KeyPtr++)
           {  if(*BufPtr == *KeyPtr)
                 continue;

              break;
           }

           if(index == indmax)
              break;
         }
      }
      else
      {  for(ip=0; s_Key[ip].bsize != 0; ip++)
         { indmax = s_Key[ip].NameLen;

           if(indmax != tmpint)
              continue; /* */    /*E0029*/

           KeyPtr = s_Key[ip].pName;
           BufPtr = buf + i;

           for(index=0; index < indmax; index++,BufPtr++,KeyPtr++)
           {  if(*BufPtr == *KeyPtr)
                 continue;

              break;
           }

           if(index == indmax)
              break;
         }
      }
   }


   if(s_Key[ip].bsize == 0)
   {  /* Key was not found */    /*E0030*/

      if(ParamErr && buf[i] != '#')
      {  if(ParamErr == 2)
         {  err = 12;
            break;
         }

         SAVE_RTL_TRACE = RTL_TRACE;

         if(IAmIOProcess == 0)
            RTL_TRACE = Is_DVM_TRACE;
         else
            RTL_TRACE = Is_IO_TRACE;

         pprintf(2+MultiProcErrReg1,
                 "*** RTS warning 012.007: invalid key word "
                 "(parameter file <%s>)\n", FileName);

         bufsave = buf[j];
         buf[j] = '\x00';

         pprintf(2+MultiProcErrReg1, "%s\n", (char *)&buf[i]);

         buf[j] = bufsave;
         RTL_TRACE = SAVE_RTL_TRACE;
      }

      if(j >= strl-1)
         break;

      SYSTEM_RET(tmpint, strspn, (&buf[j+1], " "))
      i = j + 1 + tmpint;
      continue;
   }

   /* Calculate index if it has been defined */    /*E0031*/

   KeyIndex = 0;
   SYSTEM_RET(tmpint, strcspn, (&buf[k], "=["))
   l = k + tmpint;

   if(buf[l] == '[' && l < j)
   {  SYSTEM_RET(KeyIndex, atoi, (&buf[l+1]))
      SYSTEM_RET(tmpint, strcspn, (&buf[l], "=]"))

      l += tmpint;

      if(buf[l] != ']' || l >= j)
      {  err = 6;
         break;
      }

      if(KeyIndex < 0 || KeyIndex >= (int)s_Key[ip].isize)
      {  err = 5;
         break;
      }
   }

   /* --------------------------------- */    /*E0032*/

   if(s_Key[ip].pType == 'c')
   {  SYSTEM_RET(tmpint, strcspn, (&buf[k], "="))

      if((l = k+tmpint) >= j)
      {  err = 18;
         break;
      }

      m = l + 1;
      n = j;
   }
   else
   {  SYSTEM_RET(tmpint, strcspn, (&buf[k], "="))

      if((l = k+tmpint) >= j-1)
      {  err = 9;
	 break;
      }

      SYSTEM_RET(tmpint, strspn, (&buf[l+1], " "))

      if((m = l+1+tmpint) >= j)
      {  err = 10;
         break;
      }

      SYSTEM_RET(tmpint, strcspn, (&buf[m], " ;~,"))

      if((n = m+tmpint) == m)
      {  err = 11;
	 break;
      }
   }

   for(vind=KeyIndex;     ;vind++)
   {  if((char *)s_Key[ip].pValue != NULL)
      {  addr = (char *)&buf[n];

	 if(s_Key[ip].pType == 'd')
         {  SYSTEM_RET( tmpdbl, strtod, (&buf[m], &addr))
            *((double *)s_Key[ip].pValue + vind) = tmpdbl;
         }

         if(errno == ERANGE)
	 {  err = 13;
	    break;
	 }

         if(s_Key[ip].pType == 'f')
         {  SYSTEM_RET(tmpflt, (float)atof, (&buf[m]))
            *((float *)s_Key[ip].pValue + vind) = tmpflt;
	 }

	 if(s_Key[ip].pType == 'i')
	 {  SYSTEM_RET(tmpint, atoi, (&buf[m]))
            *((int *)s_Key[ip].pValue + vind) = tmpint;
	 }

	 if(s_Key[ip].pType == 'b')
         {  SYSTEM_RET(tmpint, atoi, (&buf[m]))
	    *((byte *)s_Key[ip].pValue + vind) = (byte)tmpint;
	 }

         addr = (char *)&buf[n];

         if(s_Key[ip].pType == 'l')
         {  SYSTEM_RET(tmplng, strtol, (&buf[m], &addr, 0))
            *((long *)s_Key[ip].pValue + vind) = tmplng;
	 }

	 if(errno == ERANGE)
         {  err = 14;
	    break;
	 }

         addr = (char *)&buf[n];

	 if(s_Key[ip].pType == 'u')
	 {  sscanf(buf + m, UDTFMT, &tmplng);
            *((DvmType *)s_Key[ip].pValue + vind) = tmplng;
	 }

	 if(s_Key[ip].pType == 'D')
	 {  sscanf(buf + m, DTFMT, &tmplng);
            *((DvmType *)s_Key[ip].pValue + vind) = tmplng;
	 }

	 if(errno == ERANGE)
         {  err = 17;
	    break;
	 }

	 if(s_Key[ip].pType == 'c')
	 {
            if(s_Key[ip].esize <= 1)
            {  err = 15;
	       break;
	    }

	    if(n-m > s_Key[ip].esize-1)
            {  err = 16;
	       break;
	    }

	    for(ind=m,vind*=s_Key[ip].tsize; ind < n; ind++,vind++)
                ((char *)s_Key[ip].pValue)[vind] = buf[ind];

            ((char *)s_Key[ip].pValue)[vind] = '\x00';
         }
      }

      if(s_Key[ip].pType == 'c')
         break;

      SYSTEM_RET(tmpint, strspn, (&buf[n], " "))

      if((m = n+tmpint) >= j)
         break;

      if(vind >= s_Key[ip].esize-1)
      {  err = 19;
	 break;
      }

      if(buf[m] == ',')
      {  SYSTEM_RET(tmpint, strspn, (&buf[m+1], " "))
         m = m + 1 + tmpint;
      }

      if(m >= j)
      {  err = 20;
	 break;
      }

      SYSTEM_RET(tmpint, strcspn, (&buf[m], " ;~,"))

      if((n = m+tmpint) == m)
      {  err = 21;
	 break;
      }
   }

   if(j >= strl-1 || err)
      break;

   SYSTEM_RET(tmpint, strspn, (&buf[j+1], " "))
   i = j + 1 + tmpint;
}

/****************************\
*  Output error diagnostics  * 
\****************************/    /*E0033*/

switch(err)
{
  case 5 : pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.008: invalid key word index (%ld) "
                   "(parameter file <%s>)\n", KeyIndex, FileName);
           break;
  case 6:  pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.009: no ']' (parametr "
                   "file <%s>)\n", FileName);
           break;
  case 7:  pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.010: no key word or ';~' "
                   "(parameter file <%s>)\n", FileName);
	   break;
  case 8:  pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.011: no key word or ' =[' "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 9:  pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.012: no '=' (parametr "
                   "file <%s>)\n", FileName);
           break;
  case 10: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.013: no parameter "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 11: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.013: no parameter "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 12: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.014: invalid key word "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 13: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.015: invalid double "
                   "parameter (parameter file <%s>)\n", FileName);
           break;
  case 14: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.016: invalid long parameter "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 15: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.017: invalid max length of "
                   "char parameter (parameter file <%s>)\n",
                   FileName);
           break;
  case 16: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.018: length of char "
                   "parameter > %ld (parameter file <%s>)\n",
                   s_Key[ip].esize-1, FileName);
           break;
  case 17: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.019: invalid unsigned long "
                   "parameter (parameter file <%s>)\n", FileName);
           break;
  case 18: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.012: no '=' "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 19: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.020: no ';' or '~' "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 20: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.013: no parameter "
                   "(parameter file <%s>)\n", FileName);
           break;
  case 21: pprintf(2+MultiProcErrReg1,
                   "*** RTS err 012.013: no parameter "
                   "(parameter file <%s>)\n", FileName);
           break;
}

if(err)
{
   if(j-i > 80)
      j = i + 80;

   buf[j+1] = '\x00';

   pprintf(2+MultiProcErrReg1, "%s\n", (char *)&buf[i]);

   mac_free(&buf);

   RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

   if(1 /*CallDbgCond*/    /*E0034*/ /*EnableTrace && dvm_OneProcSign*/    /*E0035*/)
      SYSTEM(MPI_Finalize, ())
   else
      dvm_exit(err);

#else

   dvm_exit(err);

#endif

}

mac_free(&buf);

return (err);

}


#endif /* _INPUTPAR_C_ */    /*E0036*/
