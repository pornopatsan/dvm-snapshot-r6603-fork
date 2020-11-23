#ifndef _GETPAR_C_
#define _GETPAR_C_
/****************/    /*E0000*/

/**************************\
* Input initial parameters *
\**************************/    /*E0001*/

void dvm_InpCurrentPar(void)
{
  int       i, j;
  DvmType      temp;
                                               
  /* Initialization of DMV input/output structures */    /*E0002*/

  DVMSTDIN->FileID  = FilesCount;
  DVMSTDIN->File    = stdin;
  FilesCount++;
  DVMSTDOUT->FileID = FilesCount;
  DVMSTDOUT->File   = stdout;
  FilesCount++;
  DVMSTDERR->FileID = FilesCount; 
  DVMSTDERR->File   = stderr;
  FilesCount++;

  #ifdef _DVM_STDAUX_

     DVMSTDAUX->FileID = FilesCount; 
     DVMSTDAUX->File   = stdaux; 
     FilesCount++;

  #endif

  #ifdef _DVM_STDPRN_

     DVMSTDPRN->FileID = FilesCount; 
     DVMSTDPRN->File   = stdprn; 
     FilesCount++;

  #endif

  /* Preliminary filling in 
     support system arrays  */    /*E0003*/

  for(i=0; i < ProcCount; i++)
      TraceProcList[i] = i;        /* */    /*E0004*/
  TraceProcList[ProcCount] = -1;

  for(i=0; i < MaxProcNumber; i++)
      ProcNumberList[i]  = i;  /* */    /*E0005*/
  ProcNumberList[MaxProcNumber] = 0;

  for(i=0; i < MaxProcNumber; i++)
      ProcWeightList[i]  = 1.;  /* */    /*E0006*/

  for(i=0; i < ProcCount; i++)
  {
     CoordWeightList1[i] = 1.;
     CoordWeightList2[i] = 1.;
     CoordWeightList3[i] = 1.;
     CoordWeightList4[i] = 1.;
  }

  CoordWeightList1[ProcCount] = 0.;
  CoordWeightList2[ProcCount] = 0.;
  CoordWeightList3[ProcCount] = 0.;
  CoordWeightList4[ProcCount] = 0.;

  /* Memory request for tracing and statistic arrays 
              and preliminary filling in             */    /*E0007*/
  
  for(i=0; i <= MaxEventNumber; i++)
  {
     DisableTraceEvents[i] = -1;
     FullTraceEvents[i] = -1;
     IsEvent[i] = 1;
     IsStat[i] = 0;
     MaxEventLevel[i] = 127;
  }

  IsEvent[0] = 2;
  IsEvent[1] = 2;
  IsEvent[2] = 2;
  IsEvent[3] = 2;

  for(i=0; i < MaxVMRank; i++)
      PLGroupNumber[i] = 0;

  groups();    /* define event group names */    /*E0008*/
  intergrp();  /* define for every event its group number
                  that determine event processing in current interval */    /*E0009*/
  statevnt();  /* define for every event its byte
                  that determine event processing by statistics program */    /*E0010*/

  /* Define names and types of current run parameters */    /*E0011*/

  for(i=0; i <= CurrentParNumber; i++)
      CurrentPar[i][0]='\x00';

  if(DeactCurrentPar == 0)
  {
     /* File current.par is on */    /*E0012*/

     MaxParNumber(CurrentParNumber); /* max number of current
                                        run parameters */    /*E0013*/
     aParameter(CurrentPar,char [MaxParFileName+1]); /* current run
                                                     parameter defined
                                                     as alphabetic
                                                     string */    /*E0014*/

     /* Input of current run parameters */    /*E0015*/

     INPUT_PAR(CurrentParName, 2, 2);

     MaxParNumber(0);

     /* Define sizes of initial virtual machine */    /*E0016*/

     if(IAmIOProcess == 0)
     {
        /* */    /*E0017*/

        for(j=0,i=0; i < CurrentParNumber; i++)
        {
           if(CurrentPar[i][0] == '\x00')
              continue;

           SYSTEM_RET(temp, isdigit, ((unsigned int)CurrentPar[i][0]))

           if(temp == 0)
              continue;

           SYSTEM_RET(VMSSize[j], atol, (CurrentPar[i]))

           if(VMSSize[j] < 1 )
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 014.000: invalid current parameter\n"
                       "(CurrentPar[%d]=%s)\n", i, CurrentPar[i]);
           j++;
           VMSSize[j] = 0;

           if(j == MaxVMRank)
              break; 
        }
     }
     else
     {
        /* */    /*E0018*/

        VMSSize[0] = ProcCount; /* */    /*E0019*/
        VMSSize[1] = 1;
        VMSSize[2] = 1;
        VMSSize[3] = 1;
     }
  }

  /* Count matrix dimension and number of processor in MPS */    /*E0020*/

  ProcCount = 1;

  for(VMSRank=0; VMSSize[VMSRank] != 0; VMSRank++)
      ProcCount *= VMSSize[VMSRank];

  if(VMSRank == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 014.001: initial PS rank = 0\n");

  if(dvm_OneProcSign)
  {
     for(i=0; VMSSize[i] != 0; i++)
         VMSSize[i] = 1;

     VMSRank = i;

     ProcCount = 1;
  }

  return;
}


/*************************************\
* Input parameters for support system *
\*************************************/    /*E0021*/

void  dvm_InpSysPar(void)
{
   int    i, temp;
   char  *CharPtr, *SaveParFileExt;
   char   DirExt[128];
 
   mps_Barrier(); /* it is necessary for correct deleting current.par
                    ( after syspar processing) */    /*E0022*/

   ( RTL_CALL, mps_Bcast(&DeactBaseDir, 1, sizeof(int)) );
   ( RTL_CALL, mps_Bcast(&DeactUserPar, 1, sizeof(int)) );

   for(i=0; i < CurrentParNumber; i++)
   {
      if(CurrentPar[i][0] == '\x00')
         continue;

      SYSTEM_RET(temp, isdigit, ((unsigned int)CurrentPar[i][0]))

      if(temp)
         continue;

      switch(CurrentPar[i][0])
      {
         case '-': switch(CurrentPar[i][1])
                   {   
                      case 'd':
                      case 'D': 
                                if(CurrentPar[i][2] != 'e' ||
                                   CurrentPar[i][3] != 'a') /*is it
                                                              -deact  */    /*E0023*/
                                   ParFileExt = ".rel";
                                break;
                      case 'r':
                      case 'R':
                                if(CurrentPar[i][2] != 'e' ||
                                   CurrentPar[i][3] != 's') /* */    /*E0024*/
                                ParFileExt = ".deb";
                                break;
                      case 'f': if(CurrentPar[i][2] == 'w')
                                   FileOpenErrReg = 0;
                                if(CurrentPar[i][2] == 'e' &&
                                   FileOpenErrReg == 2)
                                   FileOpenErrReg = 1;
                                   break;
                      case 'p': if(CurrentPar[i][2] == 'w')
                                   ParamErrReg = 0;
                                if(CurrentPar[i][2] == 'e' &&
                                   ParamErrReg == 2)
                                   ParamErrReg = 1;
                                   break;
                      case 'i': break; /* parameter -itr[<path>]*/    /*E0025*/
                      case 'c': break; /* parameter -cp[<new name>]*/    /*E0026*/
                      case 'a': break; /* parameter -act*/    /*E0027*/
                      case 'o': break; /* parameter -opf[the namber of attempts 
                                          to open parameter file]*/    /*E0028*/
                      case 't': break; /* parameter -tfn[<file name>]*/    /*E0029*/

                      default:  epprintf(MultiProcErrReg1,
                                         __FILE__,__LINE__,
                             "*** RTS err 014.000: invalid current "
                             "parameter\n(CurrentPar[%d]=%s)\n",
                             i, CurrentPar[i]);
                   }

                   break;

         case '+': switch(CurrentPar[i][1])
                   {  
                      case 'o':
                      case 'O':         /* stdout */    /*E0030*/
                      case 'e':
                      case 'E':         /* stderr */    /*E0031*/
                      case 'i':
                      case 'I': break;
                      case 'd':
                      case 'D':
                                ParFileExt = ".deb";
                                break;
                      case 'r':
                      case 'R':
                                ParFileExt = ".rel";
                                break;
                      case 'f': if(CurrentPar[i][2] == 'w')
                                   FileOpenErrReg = 1;
                                if(CurrentPar[i][2] == 'e')
                                   FileOpenErrReg = 2;
                                   break;
                      case 'p': if(CurrentPar[i][2] == 'w')
                                   ParamErrReg = 1;
                                if(CurrentPar[i][2] == 'e')
                                   ParamErrReg = 2;
                                   break;

                      default:  epprintf(MultiProcErrReg1,
                                         __FILE__,__LINE__,
                             "*** RTS err 014.000: invalid current "
                             "parameter\n(CurrentPar[%d]=%s)\n",
                             i, CurrentPar[i]);
                   }

                   break;

         case 'd':
         case 'D':
                   if(CurrentPar[i][1] == '\x00')
                   {
                      ParFileExt = ".deb";
                      break;
                   }

         case 'r':
         case 'R':
                   if(CurrentPar[i][1] == '\x00')
                   {
                      ParFileExt = ".rel";
                      break;
                   }

         default:  if(CurrentPar[i][0] == '.' &&
                      CurrentPar[i][1] != '.')
                      ParFileExt = &CurrentPar[i][0];
                   else
                   {  SYSTEM_RET(temp, strlen, (CurrentPar[i]))
                      if(CurrentPar[i][temp-1] == '\\' ||
                         CurrentPar[i][temp-1] == '/')
                         dvm_InpFromDir(CurrentPar[i]);
                      else
                      {  if(i > 0 && CurrentPar[i-1][0] == '-')
                         {
                            SYSTEM_RET(temp, strcmp,
                                       (&CurrentPar[i-1][1], "itr"))
                            if(temp == 0)
                               break;
                            SYSTEM_RET(temp, strcmp,
                                       (&CurrentPar[i-1][1], "cp"))
                            if(temp == 0)
                               break;
                            SYSTEM_RET(temp, strcmp,
                                       (&CurrentPar[i-1][1], "deact"))
                            if(temp == 0)
                               break;
                            SYSTEM_RET(temp, strcmp,
                                       (&CurrentPar[i-1][1], "act"))
                            if(temp == 0)
                               break;
                         }
                        
                         SYSTEM_RET(CharPtr, strrchr,
                                    (CurrentPar[i], '.'))
                         if(CharPtr == NULL ||
                            (CharPtr[-1] != '/' && CharPtr[-1] != '\\'))
                            dvm_InpFromFile(CurrentPar[i]);
                         else
                         {  SaveParFileExt = ParFileExt;
                            SYSTEM(strcpy, (DirExt, CharPtr))
                            ParFileExt = DirExt;
                            CharPtr[0] = '\x00';
                            dvm_InpFromDir(CurrentPar[i]);
                            ParFileExt = SaveParFileExt;
                         }
                      }
                   }

                   break; 
      }
   }

   /* Restore SysInfoPrint parameter that has been
              input from base directory            */    /*E0032*/

   #ifndef _INIT_INFO_
      if(InpSysInfoPrint == 0)
         _SysInfoPrint = SaveSysInfoPrint;
   #endif

   return;
}

/*********************************\
* Input parameters from directory *
\*********************************/    /*E0033*/
                        
void  dvm_InpFromDir(char  *Dir)
{ char      FileName[MaxParFileName+1];
  int       NotStdSign, rc;

  if(DeactBaseDir && FirstParDir == NULL)
  {  /* input of base parameters set 
        (first directory with parameters) is blocked  */    /*E0034*/

     FirstParDir = Dir;
     ParamErrReg = 0;
     dvm_CorrOut(); /* correct information message
                       output parameters */    /*E0035*/

     /* Delete file current.par */    /*E0036*/

/*
     if(DelCurrentPar == 1)
     {  if(MPS_CurrentProc == MPS_MasterProc && IAmIOProcess == 0)
           SYSTEM(remove, (CurrentParName))
     }
*/    /*e0037*/

     return;
  }

  if(DeactUserPar && FirstParDir)
  {  /* Input of directories and files with parameters
        for correction is blocked */    /*E0038*/

     dvm_CorrOut(); /* correct information message
                       output parameters */    /*E0039*/
     return;
  }
   
  SYSTEM_RET(NotStdSign, strcmp, (ParFileExt, ".rel"))

  /***********************************\
  * Input checksum of parameter files *
  \***********************************/    /*E0040*/

  if(FirstParDir == NULL && !NotStdSign)
  {  /* Define names and types of parameters of the file
          containing checksum of files with parameters   */    /*E0041*/

     MaxParNumber(10); /* max number of parameters */    /*E0042*/

     Parameter(SystemStdCheckSum,uLLng);
     Parameter(SysTraceStdCheckSum,uLLng);
     Parameter(DebugStdCheckSum,uLLng);
     Parameter(TrcEventStdCheckSum,uLLng);
     Parameter(TrcDynControlStdCheckSum,uLLng);
     Parameter(StatistStdCheckSum,uLLng);

     Parameter(ParFileCheckSum,byte); /* attribute: check checksum of
                                         files containing parameters */    /*E0043*/

     /* Input parameters with checksum of files containing parameters */    /*E0044*/

     SYSTEM(strcpy, (FileName, Dir))
     SYSTEM(strcat, (FileName, "checksum.par"))

     SYSTEM(strcpy, (FirstCheckSumFile, FileName))/* Full name of the first 
                                                     file checksum.par  */    /*E0045*/

     INPUT_PAR(FileName, 0, ParamErrReg);

     MaxParNumber(0);
  }
 
  /***************************************\
  * Input basic support system parameters *
  \***************************************/    /*E0046*/

  MaxParNumber(SysParNumber); /* max number of parameters 
                                 for support system */    /*E0047*/

  /* Define names and types of parameters 
             for support system           */    /*E0048*/
                                     
  Parameter(SYSTEM_VERS,int);     /* the number of file version */    /*E0049*/

  SysParSet();                    /* definition of parameters */    /*E0050*/

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "syspar"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standart start */    /*E0051*/

  if(FirstParDir == NULL)
     SystemCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct output parameters for 
                    information messages */    /*E0052*/

  /* Delete file current.par */    /*E0053*/

/*
  if(DelCurrentPar == 1)
  {  if(MPS_CurrentProc == MPS_MasterProc && IAmIOProcess == 0)
        SYSTEM(remove, (CurrentParName))
  }
*/    /*e0054*/

  /* Check the version number */    /*E0055*/
 
  if(ParamErrReg)
  {  if(SYSTEM_VERS < SYSTEM_VERS_MIN || SYSTEM_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 014.002: invalid file version %.4d\n"
                   "(parameter file <%s>; right value %.4d - %.4d)\n",
                    SYSTEM_VERS, FileName, SYSTEM_VERS_MIN, DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                   "*** RTS warning 014.003: wrong file version %.4d\n"
                   "(parameter file <%s>; right value %.4d - %.4d)\n",
                   SYSTEM_VERS, FileName, SYSTEM_VERS_MIN, DVM_VERS);
     }
  }


  /**************************************************\
  * Input of parameters for built in debugging tools *
  \**************************************************/    /*E0056*/ 

  MaxParNumber(DebugParNumber); /* max number of parameters for
                                   built in debugging tools */    /*E0057*/

  /* Define names and types of built in debugging tool parameters */    /*E0058*/

  Parameter(DEBUG_VERS,int);     /* the number of file version */    /*E0059*/

  DebugParSet();                 /* definition of parameters */    /*E0060*/

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "sysdebug"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standard start */    /*E0061*/

  if(FirstParDir == NULL)
     DebugCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct parameters for output
                    of information messages */    /*E0062*/

  /* Check version number */    /*E0063*/
 
  if(ParamErrReg)
  {  if(DEBUG_VERS < DEBUG_VERS_MIN || DEBUG_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 014.002: wrong file version %.4d\n"
                   "(parameter file <%s>; right value %.4d - %.4d)\n",
                    DEBUG_VERS,FileName,DEBUG_VERS_MIN,DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                  "*** RTS warning 014.003: wrong file version %.4d\n"
                  "(parameter file <%s>; right value %.4d - %.4d)\n",
                  DEBUG_VERS,FileName,DEBUG_VERS_MIN,DVM_VERS);
     }
  }


  /********************************\ 
  * Input of trace mode parameters *
  \********************************/    /*E0064*/ 

  MaxParNumber(TraceParNumber); /* max number of trace parameters */    /*E0065*/

  /* Define names and types of trace parameters */    /*E0066*/

  Parameter(SYSTRACE_VERS,int);  /* the number of file version */    /*E0067*/

  TraceParSet();                 /* definition of parameters */    /*E0068*/

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "systrace"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standard start */    /*E0069*/

  if(FirstParDir == NULL)
     SysTraceCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct parameters 
                    for output of information messages */    /*E0070*/

  /* Check the version number */    /*E0071*/
 
  if(ParamErrReg)
  {  if(SYSTRACE_VERS < SYSTRACE_VERS_MIN || SYSTRACE_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 014.002: wrong file version %.4d\n"
                  "(parameter file <%s>; right value %.4d - %.4d)\n",
                  SYSTRACE_VERS,FileName,SYSTRACE_VERS_MIN,DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                 "*** RTS warning 014.003: wrong file version %.4d\n"
                 "(parameter file <%s>; right value %.4d - %.4d)\n",
                 SYSTRACE_VERS,FileName,SYSTRACE_VERS_MIN,DVM_VERS);
     }
  }

  /*********************************\
  * Input of trace event parameters *
  \*********************************/    /*E0072*/ 

  MaxParNumber(EventParNumber); /* max number of trace event parameters */    /*E0073*/

  /*   Define names and types 
     of  trace event parameters */    /*E0074*/

  Parameter(TRCEVENT_VERS,int);   /* the file version number */    /*E0075*/

  EventParSet();                  /* definision of parameters */    /*E0076*/

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "trcevent"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standard start */    /*E0077*/

  if(FirstParDir == NULL)
     TrcEventCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct parameters 
                    for output of information messages */    /*E0078*/

  /* Check version number */    /*E0079*/
 
  if(ParamErrReg)
  {  if(TRCEVENT_VERS < TRCEVENT_VERS_MIN || TRCEVENT_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 014.002: wrong file version %.4d\n"
                  "(parameter file <%s>; right value %.4d - %.4d)\n",
            TRCEVENT_VERS,FileName,TRCEVENT_VERS_MIN,DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                 "*** RTS warning 014.003: wrong file version %.4d\n"
                 "(parameter file <%s>; right value %.4d - %.4d)\n",
            TRCEVENT_VERS,FileName,TRCEVENT_VERS_MIN,DVM_VERS);
     }
  }

  /**********************************\
  * Input dynamic control parameters *
  \**********************************/    /*E0080*/

  MaxParNumber(DynControlParNumber); /* max number of 
                                        dynamic control parameters */    /*E0081*/ 
 
  /* Define names and types of dynamic control parameters */    /*E0082*/
 
  Parameter(CMPTRACE_VERS,int);     /* the number of file version */    /*E0083*/

  DynControlParSet();               /* definition of parameters */    /*E0084*/ 

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "usrdebug"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standard start */    /*E0085*/

  if(FirstParDir == NULL)
     TrcDynControlCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct parameters 
                    for output of information messages */    /*E0086*/

  /* Check version number */    /*E0087*/
 
  if(ParamErrReg)
  {  if(CMPTRACE_VERS < CMPTRACE_VERS_MIN || CMPTRACE_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 014.002: wrong file version %.4d\n"
                  "(parameter file <%s>; right value %.4d - %.4d)\n",
                    CMPTRACE_VERS,FileName,CMPTRACE_VERS_MIN,DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                "*** RTS warning 014.003: wrong file version %.4d\n"
                "(parameter file <%s>; right value %.4d - %.4d)\n",
                CMPTRACE_VERS,FileName,CMPTRACE_VERS_MIN,DVM_VERS);
     }
  }

  /*************************************\ 
  * Input of statistics mode parameters *
  \*************************************/    /*E0088*/

  MaxParNumber(StatistParNumber); /* max number of 
                                     statistics parameters */    /*E0089*/

  /* Define names and types of statistics parameters */    /*E0090*/

  Parameter(STATIST_VERS,int);     /* the number of file version */    /*E0091*/

  StatistParSet();                 /* definision of parameters */    /*E0092*/

  SYSTEM(strcpy, (FileName, Dir))
  SYSTEM(strcat, (FileName, "statist"))
  SYSTEM(strcat, (FileName, ParFileExt))

  rc = INPUT_PAR(FileName, FileOpenErrReg, ParamErrReg);

  if((!FirstParDir && (NotStdSign || rc)) || (FirstParDir && !rc))
     StdStart = 0;  /* turn off standard statr */    /*E0093*/

  if(FirstParDir == NULL)
     StatistCheckSum = InputParCheckSum;

  MaxParNumber(0);

  dvm_CorrOut(); /* correct papameters for output
                    of information messages */    /*E0094*/

  CheckGrpName(FileName);/* check group name for time printing */    /*E0095*/ 

  /* Check version number */    /*E0096*/
 
  if(ParamErrReg)
  {  if(STATIST_VERS < STATIST_VERS_MIN || STATIST_VERS > DVM_VERS)
     {  if(ParamErrReg == 2)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 014.002: wrong file version %.4d\n"
                 "(parameter file <%s>; right value %.4d - %.4d)\n",
                 STATIST_VERS,FileName,STATIST_VERS_MIN,DVM_VERS);
        else
           pprintf(2+MultiProcErrReg1,
                "*** RTS warning 014.003: wrong file version %.4d\n"
                "(parameter file <%s>; right value %.4d - %.4d)\n",
                STATIST_VERS,FileName,STATIST_VERS_MIN,DVM_VERS);
     }
  }

  if(FirstParDir == NULL)
     FirstParDir = Dir;   /* pointer to the first
                             directory with parameters */    /*E0097*/
  return;
}


/****************************\
* Input parameters from file *
\****************************/    /*E0098*/

void  dvm_InpFromFile(char  *File)
{ int  rc;

  if(DeactUserPar)
  {  /* Input of directories and files with parameters
        for correction is blocked */    /*E0099*/

     dvm_CorrOut(); /* correct information message
                       output parameters */    /*E0100*/
     return;
  }

  MaxParNumber(SysParNumber
               +DebugParNumber
               +TraceParNumber
               +EventParNumber
               +DynControlParNumber
               +StatistParNumber   ); /* max number of parameters */    /*E0101*/

  /* Define names and types of parameters */    /*E0102*/

  /* ----------  syspar  ---------- */    /*E0103*/

  SysParSet();

  /* ----------  sysdebug  ---------- */    /*E0104*/

  DebugParSet();

  /* ----------  systrace  ---------- */    /*E0105*/

  TraceParSet();

  /* ----------  trcevent  ---------- */    /*E0106*/

  EventParSet();

  /* ----------  usrdebug  ---------- */    /*E0107*/

  DynControlParSet();

  /* ----------  statist  ---------- */    /*E0108*/

  StatistParSet();

  rc = INPUT_PAR(File, FileOpenErrReg, ParamErrReg);

  if(!rc)
     StdStart = 0;  /* turn off standard start */    /*E0109*/

  MaxParNumber(0);

  dvm_CorrOut(); /* correct parameters for output of
                    information messages */    /*E0110*/

  CheckGrpName(File); /* check group name for time printing */    /*E0111*/ 

  return;
}


/************************************\
* Functions of parameters definition *
\************************************/    /*E0112*/

void  SysParSet(void)
{
  Parameter(IsUserPS,byte);           /* flag: user program
                                         processor system */    /*E0113*/
  Parameter(UserPS, DvmType);             /* sizes of user program
                                         processor system
                                         ( virtual processor system) */    /*E0114*/
  Parameter(ProcListSign,byte);       /* use list of external
                                         processor numbers */    /*E0115*/
  Parameter(ProcNumberList,int);      /* list of external
                                         processor numbers */    /*E0116*/
  Parameter(CoordWeightSign,byte);    /* use list of 
                                         processor coordinate weights */    /*E0117*/

  Parameter(MaxCoordWeight,double);    /* */ 

  Parameter(CoordWeightList1,double); /* list of processor coordinate
                                         weights of the 1-st dimension
                                         of the initial processor 
                                         system */    /*E0118*/
  Parameter(CoordWeightList2,double); /* list of processor coordinate 
                                         weights of the 2-nd dimension
                                         of the initial processor 
                                         system */    /*E0119*/
  Parameter(CoordWeightList3,double); /* list of processor coordinate 
                                         weights of the 3-rd dimension
                                         of the initial processor 
                                         system */    /*E0120*/
  Parameter(CoordWeightList4,double); /* list of processor coordinate 
                                         weights of the 4-th dimension
                                         of the initial processor 
                                         system */    /*E0121*/
  Parameter(RouterKillPrint,byte);    /* print killed
                                         subtasks numbers when
                                         working with ROUTER */    /*E0122*/
  Parameter(SysParPrint,byte);        /* print parameters of initialization 
                                         when execution starts */    /*E0123*/
  Parameter(VersStartPrint,byte);     /* print version 
                                         when execution starts */    /*E0124*/
  Parameter(VersFullStartPrint,byte); /* print version in detail 
                                         when execution starts */    /*E0125*/ 
  Parameter(ProcListPrint,byte);      /* print processor table 
                                         when execution starts */    /*E0126*/
  Parameter(WeightListPrint,byte);    /* print processor coordinate weights 
                                         when execution starts */    /*E0127*/
  Parameter(MsgSchedulePrint,byte);   /* */    /*E0128*/
  Parameter(ParamRunPrint,byte);      /* output at screen 
                                         run parameters 
                                         when execution starts */    /*E0129*/
  Parameter(AcrossInfoPrint,byte);    /* output information of 
                                         ACROSS scheme execution */    /*E0130*/
  Parameter(MPIReducePrint,byte);     /* */    /*E0131*/
  Parameter(MPIBcastPrint,byte);      /* */    /*E0132*/
  Parameter(MPIBarrierPrint,byte);    /* */    /*E0133*/
  Parameter(MPIInfoPrint,byte);       /* */    /*E0134*/
  Parameter(PPMeasurePrint,byte);     /* */    /*E0135*/
  Parameter(VersFinishPrint,byte);    /* print version 
                                         when execution ends */    /*E0136*/
  Parameter(VersFullFinishPrint,byte);/* print version in detail 
                                         when execution ends */    /*E0137*/
  Parameter(EndProgMemoryPrint,byte); /* output information on
                                         non freed memory by the 
                                         work completion */    /*E0138*/  
  Parameter(EndProgObjectPrint,byte); /* output information on 
                                         non freed DVM-objects by the
                                         work completion */    /*E0139*/
  Parameter(EndProgCheckSumPrint,byte);/* output information on 
                                          control sums of the memory
                                          regions under control
                                          by the work completion */    /*E0140*/
  Parameter(SysProcessNamePrint,byte); /* */    /*E0141*/
  Parameter(SubTasksTimePrint,byte);   /* */    /*E0142*/
  Parameter(UserTimePrint,byte);       /* */    /*E0143*/
  Parameter(FreeObjects,byte);         /* kill non static DVM-objects
                                         by the work completion */    /*E0144*/
  Parameter(S_MPIAlltoall,byte);  /* */    /*E0145*/
  Parameter(A_MPIAlltoall,byte);  /* */    /*E0146*/
  Parameter(CG_MPIAlltoall,byte);/* */    /*E0147*/
  Parameter(AlltoallWithMsgSchedule,byte); /* */    /*E0148*/
  Parameter(MPIReduce,byte);      /* */    /*E0149*/
  Parameter(MPIBcast,byte);       /* */    /*E0150*/
  Parameter(MPIBarrier,byte);     /* */    /*E0151*/
  Parameter(MPIGather,byte);      /* */    /*E0152*/
  Parameter(strtac_FreeBuf,byte); /* */    /*E0153*/
  Parameter(consda_FreeBuf,byte); /* */    /*E0154*/
  Parameter(dopl_WaitRD,byte);    /* flag on  outstripping execution of 
                                         reductions in function dopl _ */    /*E0155*/
  Parameter(InPLQNumber,int);     /* number of parts
                                     internal part of parallel loop is 
                                     divided in */    /*E0156*/
  Parameter(AcrossGroupNumber,int); /* number of parts, the quanted loop dimension
                                       to be partioned
                                       during conveiorization of ACROSS  */    /*E0157*/
  Parameter(AcrossQuantumReg,byte); /* */    /*E0158*/
  Parameter(ASynchrPipeLine,byte);  /* flag of asynchronous ACROSS 
                                       pipeline scheme */    /*E0159*/
  Parameter(CoilTime,double);       /* time of execution of one ACROSS 
                                       loop iteration */    /*E0160*/
  Parameter(dopl_MPI_Test,byte);  /* flag: call MPI_Test function
                                     while dopl_ function execution */    /*E0161*/
  Parameter(dopl_MPI_Test_Count,int); /* */    /*E0162*/
  Parameter(ReqBufSize,int);      /* size of buffer to save
                                     exchange flag pointers when
                                     dopl_MPI_Test=1 */    /*E0163*/
  Parameter(MPS_ReqBufSize,int);  /* */    /*E0164*/
  Parameter(MPI_Issend_sign,int); /* MPI_Issend may be used with MPI system.
                                     if the value = 0 MPI_Isend function
                                     may be used only */    /*E0165*/
  Parameter(IssendMsgLength, DvmType); /* if message lenth is more
                                      than the parameter value
                                      MPI_Issend function will be used 
                                      instead of MPI_Isend function */    /*E0166*/
  Parameter(MsgSchedule,int);   /* */    /*E0167*/
  Parameter(MsgPartReg,int);    /* */    /*E0168*/
  Parameter(MaxMsgLength,int);  /* */    /*E0169*/
  Parameter(MaxMsgParts,int);   /* */    /*E0170*/
  Parameter(MsgExchangeScheme,int); /* */    /*E0171*/
  Parameter(MsgPairNumber,int);    /* */    /*E0172*/
  Parameter(setelw_precision,double); /* precision of processor coordinate weights
                                         calculated by setelw_ function */    /*E0173*/

  /* ----------------------------------------------------- */    /*E0174*/

  Parameter(OneProcSign, int);      /* */    /*E0175*/

  Parameter(PPMeasureCount, DvmType);   /* number of loops during processor
                                       performance  measuring */    /*E0176*/
  Parameter(ProcWeightSign,byte); /* use list of processor
                                     performance weights */    /*E0177*/
  Parameter(ProcWeightList,double);/* list of processor performance
                                      weights */    /*E0178*/
  Parameter(StdOutToFile,byte);   /* flag of redirection of stout to file */    /*E0179*/
  Parameter(StdOutFileName,char); /* file name for sdtout stream
                                     redirection */    /*E0180*/
  Parameter(StdErrToFile,byte);   /* flag of redirection of stderr to file */    /*E0181*/
  Parameter(StdErrFileName,char); /* file name for sdterr stream
                                     redirection */    /*E0182*/
  Parameter(DelStdStream,byte);   /* flag of deletion of old files
                                     with standard streams */    /*E0183*/ 
  Parameter(MultiProcErrReg,byte);/* byte of output messages of errors
                                     on multiprocessor system */    /*E0184*/
  Parameter(SysInfoPrint,byte);   /* general flag on
                                     output information messages of 
                                     support system */    /*E0185*/
  Parameter(SysInfoStdOut,byte);  /* output information messages 
                                     into stdout */    /*E0186*/
  Parameter(SysInfoStdErr,byte);  /* output information messages 
                                      into stderr */    /*E0187*/
  Parameter(SysInfoFile,byte);    /* output information messages 
                                     in file */    /*E0188*/
  Parameter(SysInfoSepFile,byte); /* save information messages 
                                     in separate files for all processors */    /*E0189*/
  Parameter(SysInfoFileName,char);/* file name for
                                     information messages */    /*E0190*/
  Parameter(FatInfoNoOpen,byte);  /* stop execution in case of failed openning file
                                     for information messages */    /*E0191*/
  Parameter(DelSysInfo,byte);     /* flag of deletion of old files with
                                     information messages */    /*E0192*/
  Parameter(DelCurrentPar,byte);  /* */    /*E0193*/
  Parameter(InputParPrint,byte);  /* print when input of each
                                     file with parameters starts */    /*E0194*/
  Parameter(EndReadParPrint,byte);/* print when input of each
                                     file with parameters ends */    /*E0195*/
  Parameter(DVMInputPar,byte);    /* input parameters 
                                     by DVM functions */    /*E0196*/
  Parameter(CheckRendezvous,byte);/* check that only one exchange
                                     between two processors is allowed */    /*E0197*/
  Parameter(Msk3,int);            /* mask for message length */    /*E0198*/
  Parameter(RendErrorPrint,byte); /* output message mode: 
                                     CheckRendezvous=1 */    /*E0199*/
  Parameter(MaxMeasureIndex,int); /* maximal index of 
                                     time measuring */    /*E0200*/

  /* */    /*E0201*/

  Parameter(MsgBuf1Length,int);    /* */    /*E0202*/
  Parameter(MsgBufLength,int);     /* */    /*E0203*/
  Parameter(DuplChanSign,byte);    /* */    /*E0204*/
  Parameter(ParChanNumber,int);    /* */    /*E0205*/
  Parameter(MaxMsgSendNumber,int); /* */    /*E0206*/
  Parameter(MaxMsg1SendNumber,int);/* */    /*E0207*/
  Parameter(ResCoeff,double);       /* */    /*E0208*/
  Parameter(ResCoeffDoPL,double);   /* */    /*E0209*/
  Parameter(ResCoeffTstReq,double); /* */    /*E0210*/
  Parameter(ResCoeffWaitReq,double);/* */    /*E0211*/
  Parameter(ResCoeffDACopy,double); /* */    /*E0212*/
  Parameter(ResCoeffElmCopy,double);/* */    /*E0213*/
  Parameter(ResCoeffRedNonCentral,double); /* */    /*E0214*/
  Parameter(ResCoeffRedCentral,double);    /* */    /*E0215*/
  Parameter(ResCoeffLoadIB,double); /* */    /*E0216*/
  Parameter(ResCoeffLoadIG,double); /* */    /*E0217*/
  Parameter(ResCoeffAcross,double); /* */    /*E0218*/
  Parameter(ResCoeffShdSend,double);/* */    /*E0219*/
  Parameter(ResCoeffInSend,double); /* */    /*E0220*/
  Parameter(ResCoeffLoadRB,double); /* */    /*E0221*/
  Parameter(ResCoeffLoadBG,double); /* */    /*E0222*/

  Parameter(MsgWaitReg,byte);       /* */    /*E0223*/
  Parameter(FreeChanReg,int);       /* */    /*E0224*/

  /* */    /*E0225*/

  Parameter(AlignMemoryAddition,int);
  Parameter(AlignMemoryDelta,int);
  Parameter(AlignMemoryCircle,int);

  /* ------------------------------------------------- */    /*E0226*/

  Parameter(CompressLevel,int); /* */    /*E0227*/
  Parameter(CompressFlush,byte); /* */    /*E0228*/

  /* ------------------------------------------------- */    /*E0229*/

  Parameter(TimeEqualizationCount,int); /* */    /*E0230*/
  Parameter(MPI_Wtime_Sign,byte);    /* */    /*E0231*/
  Parameter(UTime_Sign,byte);        /* */    /*E0232*/

  /* */    /*E0233*/

  Parameter(ShgSave,byte);  /* */    /*E0234*/
  Parameter(RgSave,byte);   /* */    /*E0235*/

  /* ------------------------------------------------- */    /*E0236*/

  Parameter(BoundAddition,int);  /* */    /*E0237*/

  /* */    /*E0238*/

  Parameter(MsgCompressLevel,int); /* */    /*E0239*/
  Parameter(MsgCompressStrategy,int); /* */    /*E0240*/ 
  Parameter(MsgDVMCompress,int);   /* */    /*E0241*/
  Parameter(CompressCoeff,float);  /* */    /*E0242*/
  Parameter(MinMsgCompressLength,int); /* */    /*E0243*/
  Parameter(MsgCompressWithMsgPart,int); /* */    /*E0244*/
  Parameter(ZLIB_Warning,byte); /* */    /*E0245*/
  Parameter(AlltoallCompress,byte); /* */    /*E0246*/
  Parameter(GatherCompress,byte);   /* */    /*E0247*/
  Parameter(BcastCompress,byte);    /* */    /*E0248*/

  /* */    /*E0249*/

  Parameter(DAReadPlane,byte);   /* */    /*E0250*/
  Parameter(DAWritePlane,byte);  /* */    /*E0251*/
  Parameter(DAVoidRead,byte);    /* */    /*E0252*/
  Parameter(DAVoidWrite,byte);   /* */    /*E0253*/

  /* */    /*E0254*/

  Parameter(dvm_void_scan,byte);   /* */    /*E0255*/
  Parameter(dvm_void_fread,byte);  /* */    /*E0256*/
  Parameter(dvm_void_fwrite,byte); /* */    /*E0257*/
  Parameter(MinIOMsgSize, DvmType);    /* */    /*E0258*/
  Parameter(MaxIOMsgSize, DvmType);    /* */    /*E0259*/
  Parameter(FreeIOBuf,byte);     /* */    /*E0260*/
  Parameter(MPITestAfterSend,int); /* */    /*E0261*/
  Parameter(MPITestAfterRecv,int); /* */    /*E0262*/
  Parameter(SaveIOFlag,byte);      /* */    /*E0263*/
  Parameter(PrtSign,byte);         /* */    /*E0264*/
  Parameter(IOProcPrt,int);        /* */    /*E0265*/
  Parameter(ApplProcPrt,int);      /* */    /*E0266*/
  Parameter(SleepCount,int);       /* */    /*E0267*/

  /* */    /*E0268*/

  Parameter(StrtRedSynchr,byte); /* */    /*E0269*/
  Parameter(StrtShdSynchr,byte); /* */    /*E0270*/
  Parameter(DACopySynchr,byte);  /* */    /*E0271*/
  Parameter(ADACopySynchr,byte); /* */    /*E0272*/

  return;
}



void  DebugParSet(void)
{
  Parameter(TstObject,byte);     /* check if it is DVM-object */    /*E0273*/
  Parameter(DisArrayFill,byte);  /* flag: filling distributed array
                                    during distribution and redistribution */    /*E0274*/
  Parameter(FillCode,byte);      /* sequence of bytes for filling  
                                    initialized elements of 
                                    distributed arrays */    /*E0275*/
  Parameter(WaitDelay, DvmType);     /* value of delay of rtl_Waitrequest
                                    function execution */    /*E0276*/
  Parameter(RecvDelay, DvmType);     /* value of delay of rtl_Recvnowait
                                    function execution*/    /*E0277*/
  Parameter(SendDelay, DvmType);     /* value of delay of rtl_Sendnowait
                                    function execution */    /*E0278*/
  Parameter(SaveAllocMem,byte);  /* saving request
                                    in structure array */    /*E0279*/
  Parameter(CheckFreeMem,byte);  /* check edges of 
                                    all saved requests */    /*E0280*/
  Parameter(AllocBufSize,int);   /* size of structure array for
                                    saving memory requests */    /*E0281*/
  Parameter(BoundSize,byte);     /* size of low and high edges for
                                    requested memory in bytes */    /*E0282*/
  Parameter(BoundCode,byte);     /* fill code for low and high edges */    /*E0283*/
  Parameter(CheckPtr,byte);      /* check pointer values
                                    during memory requests and returns */    /*E0284*/
  Parameter(MinPtr,uLLng);        /* min value of the pointer for
                                    checking memory requests */    /*E0285*/
  Parameter(MaxPtr,uLLng);        /* max value of the pointer for
                                    checking memory requests */    /*E0286*/

  /* */    /*E0287*/

  Parameter(MPI_MsgTest,int);   /* */    /*E0288*/

  Parameter(MPI_TestCount,int); /* */    /*E0289*/
  Parameter(MPI_TestSize,int);  /* */    /*E0290*/

  /* */    /*E0291*/

  Parameter(MPI_TraceRoutine,int); /* */    /*E0292*/
  Parameter(DVM_TraceOff,int);   /* */    /*E0293*/
  Parameter(MPI_TraceLevel,int); /* */    /*E0294*/
  Parameter(MPI_TraceReg,int);   /* */    /*E0295*/
  Parameter(MPI_TraceTimeReg,byte); /* */    /*E0296*/
  Parameter(MPI_SlashOut,byte);  /* */    /*E0297*/
  Parameter(MPI_TraceAll,byte);  /* */    /*E0298*/
  Parameter(MPI_TraceTime,byte); /* */    /*E0299*/
  Parameter(MPI_TraceFileLine,byte); /* */    /*E0300*/
  Parameter(MPI_TraceMsgChecksum,byte); /* */    /*E0301*/
  Parameter(MPI_DynAnalyzer,int);/* */    /*E0302*/
  Parameter(MPI_TestTraceCount,int); /* */    /*E0303*/
  Parameter(MPI_TraceBufSize, DvmType);  /* */    /*E0304*/
  Parameter(MPI_TraceFileSize, DvmType); /* */    /*E0305*/
  Parameter(MPI_TotalTraceFileSize, DvmType);/* */    /*E0306*/
  Parameter(MPI_DebugMsgChecksum,byte); /* */    /*E0307*/
  Parameter(MPI_DebugBufChecksum,byte); /* */    /*E0308*/

  return;
}



void  TraceParSet(void)
{
  Parameter(Is_DVM_TRACE,byte);  /* trace on/trace off */    /*E0309*/
  Parameter(Is_IO_TRACE,byte);   /* */    /*E0310*/
  Parameter(Is_ALL_TRACE,byte);  /* trace all functions
                                    (except calls for statistics and
                                    debugger) */    /*E0311*/
  Parameter(Is_DEB_TRACE,byte);  /* trace all functions for  
                                    debugger call */    /*E0312*/
  Parameter(Is_STAT_TRACE,byte); /* trace functions for
                                    statistics calls */    /*E0313*/
  Parameter(Is_IOFun_TRACE,byte);/* */    /*E0314*/
  Parameter(UserCallTrace,byte); /* */    /*E0315*/
  Parameter(DisableTraceTime,byte);/* subtract trace time
                                      from the time output by tarcing */    /*E0316*/
  Parameter(BlockTrace,byte);    /* stop trace till 
                                    function tron_ */    /*E0317*/
  Parameter(MaxTraceLevel,byte); /* maximal depth of trace 
                                    for embedded functions */    /*E0318*/
  Parameter(TraceClosePrint,byte);  /* print message at the end of 
                                       trace dumping */    /*E0319*/
  Parameter(IsTraceProcList,byte);  /* flag of turned on list
                                       TraceProcList */    /*E0320*/
  Parameter(TraceProcList,int);     /* array of internal 
                                       trace processor numbers */    /*E0321*/
  Parameter(TraceBufLength, DvmType);   /* trace buffer length in bytes */    /*E0322*/
  Parameter(BufferTrace,byte);      /* save  trace in buffer */    /*E0323*/
  Parameter(FullBufferStop,byte);   /* stop  tracing 
                                       if the buffer is full */    /*E0324*/
  Parameter(BufferTraceUnLoad,byte);/* unload trace buffer into file 
                                       at the end of execution */    /*E0325*/
  Parameter(ScreenTrace,byte);      /* trace output to the screen */    /*E0326*/
  Parameter(FileTrace,byte);        /* save trace into  files */    /*E0327*/
  Parameter(MaxTraceFileSize, DvmType); /* */    /*E0328*/
  Parameter(MaxCommTraceFileSize, DvmType); /* */    /*E0329*/
  Parameter(TraceFileOverflowReg,byte); /* */    /*E0330*/ 
  Parameter(FullTrace,byte);        /* flag on  detailed trace mode  */    /*E0331*/
  Parameter(KeyWordName,byte);      /* print " NAME = " 
                                       before each event name */    /*E0332*/
  Parameter(SetTraceBuf,byte);      /* trace with buffering by
                                       operating system */    /*E0333*/
  Parameter(TraceFlush,byte);       /* dump output stream 
                                       for each trace event */    /*E0334*/
  Parameter(FatTraceNoOpen,byte);   /* stop execution 
                                       in case of  unsuccessful 
                                       trace file opening */    /*E0335*/
  Parameter(PreUnderLine,byte);     /* underline header of each event */    /*E0336*/
  Parameter(PostUnderLine,byte);    /* underline  the end of 
                                       each event */    /*E0337*/
  Parameter(BufferTraceShift,byte); /* indent for next level of embedded function calls  
                                       while tracing in buffer */    /*E0338*/
  Parameter(FileTraceShift,byte);   /* indent for next level of embedded function calls
                                       while tracing in files */    /*E0339*/ 
  Parameter(TracePath,char);        /* path for trace files */    /*E0340*/
  Parameter(TraceFileExt,char);     /* file extension for 
                                       tracing into files */    /*E0341*/
  Parameter(TraceBufferExt,char);   /* file extension for 
                                       dumping trace buffers into files */    /*E0342*/
  Parameter(MPI_TraceFileNameNumb,byte);/* */    /*E0343*/
  Parameter(DelSysTrace,byte);      /* delete old trace files */    /*E0344*/
  Parameter(PreUnderLining,char);   /* string for underlining 
                                       header of each event */    /*E0345*/
  Parameter(PostUnderLining,char);  /* string to be print 
                                       at the end of each event */    /*E0346*/
  Parameter(CurrentTimeTrace,byte); /* flag of trace of  
                                       current system tine*/    /*E0347*/
  Parameter(TimePrecision,byte);    /* number of digits after decimal point 
                                       for time representation in trace */    /*E0348*/

  Parameter(LowDumpLevel,byte); /* trace output using 
                                   low level functions */    /*E0349*/
  Parameter(mappl_Trace,byte);  /* additional information output mode
                                   while tracing mappl_ function:
                                   0 - do not output 
                                   1 - output index values 
                                   2 - output all additional information*/    /*E0350*/
  Parameter(dopl_Trace,byte);          /* additional information output 
                                          while tracing the dopl_   function */    /*E0351*/ 
  Parameter(dopl_dyn_GetLocalBlock,byte);/* test call of function
                                            dyn_GetLocalBlock during
                                            the tracing of the function dopl_ */    /*E0352*/
  Parameter(distr_Trace,byte);         /* additional information output 
                                          while tracing the distr_ function */    /*E0353*/
  Parameter(align_Trace,byte);         /* additional information output 
                                          while tracing the align_ function */    /*E0354*/
  Parameter(dacopy_Trace,byte);        /* additional information output 
                                          while tracing arrcpy_ and aarrcp_
                                          functions */    /*E0355*/
  Parameter(OutIndexTrace,byte);       /* print output indexes 
                                          in function GetIndexArray */    /*E0356*/
  Parameter(RedVarTrace,int);          /* print reduction variable 
                                          in function saverg_,saverv_,
                                          strtrd_ and waitrd_ */    /*E0357*/
  Parameter(diter_Trace,byte);         /* output detail information
                                          while tracing function diter_ */    /*E0358*/
  Parameter(drmbuf_Trace,byte);        /* output detail information
                                          while tracing function drmbuf_ */    /*E0359*/
  Parameter(dyn_GetLocalBlock_Trace,byte);/* trace teh function
                                             dyn_GetLocalBlock under
                                             detail tarcing regime */    /*E0360*/
  Parameter(PrintBufferByteCount,int); /* number of the bytes in buffer 
                                          printed before Send
                                          and after Receive */    /*E0361*/
  Parameter(TraceVarAddr,uLLng);        /* address of tracing variable;
                                          for null address the variable tracing
                                          is not preformed */    /*E0362*/
  Parameter(TraceVarType,byte);        /* type of tracing variable:
                                          1 - int,
                                          2 - long,
                                          3 - float,
                                          4 - double,
                                          5 - char,
                                          6 - short,
                                          7 - long long.
                                          */    /*E0363*/
  Parameter(LoadWeightTrace,byte);     /* flad: trace processor loading 
                                          weights
                                          when function setelw_ is called */    /*E0364*/
  Parameter(WeightArrayTrace,byte);    /* flag of trace of coordinate loading weights
                                          at the end of function
                                          gettar_ */    /*E0365*/
  Parameter(AcrossTrace,byte);   /* flag of tracing ACROSS scheme execution
                                    in across_ and dopl_ functions */    /*E0366*/
  Parameter(MsgPartitionTrace,byte);/* */    /*E0367*/
  Parameter(MsgScheduleTrace,int);  /* */    /*E0368*/
  Parameter(MPI_AlltoallTrace,byte);/* */    /*E0369*/
  Parameter(MPI_ReduceTrace,byte); /* */    /*E0370*/
  Parameter(DAConsistTrace,byte);  /* */    /*E0371*/
  Parameter(MPI_MapAMTrace,byte);  /* */    /*E0372*/
  Parameter(CrtPSTrace,byte);      /* */    /*E0373*/
  Parameter(MsgCompressTrace,byte);/* */    /*E0374*/
  Parameter(SysProcessNameTrace,byte); /* */    /*E0375*/
  Parameter(MPI_RequestTrace,byte); /* */    /*E0376*/
  Parameter(MPI_IORequestTrace,byte);/* */    /*E0377*/
  Parameter(dvm_StartAddr, DvmType); /* first address of checked memory */    /*E0378*/
  Parameter(dvm_FinalAddr, DvmType); /* last address of checked memory */    /*E0379*/
  Parameter(EveryEventCheckMem,byte);    /* check defined memory 
                                            every trace event */    /*E0380*/
  Parameter(EveryTraceCheckMem,byte);    /* check defined memory 
                                            every trace entry */    /*E0381*/
  Parameter(EveryEventCheckCodeMem,byte);/* check command memory 
                                            every trace event */    /*E0382*/
  Parameter(EveryTraceCheckCodeMem,byte);/* check command memory 
                                            every trace entry */    /*E0383*/     
  Parameter(EveryEventCheckBound,byte);  /* check edges of 
                                            occupied memory blocks 
                                            every trace event */    /*E0384*/
  Parameter(EveryTraceCheckBound,byte);  /* check edges of 
                                            occupied memory blocks 
                                            every trace entry */    /*E0385*/

/* */    /*E0386*/

  Parameter(TraceCompressLevel,int); /* */    /*E0387*/
  Parameter(TraceCompressFlush,int); /* */    /*E0388*/

  return;
}



void  EventParSet(void)
{
  Parameter(IsDisableTraceEvents,byte); /* flag on DisableTraceEvents */    /*E0389*/
  Parameter(IsFullTraceEvents,byte); /* flag on FullTraceEvents */    /*E0390*/
  Parameter(DisableTraceEvents,int); /* array of numbers of events
                                        to be traced */    /*E0391*/
  Parameter(FullTraceEvents,int);    /* array of traced event numbers
                                        in extended mode */    /*E0392*/
  Parameter(IsEvent,byte);        /* event is included */    /*E0393*/
  Parameter(MaxEventLevel,byte);  /* maximal depth of trace 
                                     for embedded functions */    /*E0394*/

  return;
}



void  StatistParSet(void)
{
  Parameter(Is_DVM_STAT,byte);     /* statistics on/statistics off */    /*E0395*/
  Parameter(Is_IO_STAT,byte);      /* */    /*E0396*/
  Parameter(StatBufLength, DvmType);   /* ststistics buffer length
                                      for one processor */    /*E0397*/
  Parameter(StatFileName,char);    /* filename for statistics */    /*E0398*/
  Parameter(DelStatist,byte);      /* delete ald files with
                                      statistics */    /*E0399*/ 
  Parameter(TimeExpendPrint,int);  /* print
                                      task execution time:
                                      0   - not print;
                                      1   - short print;
                                      > 1 - detail print. */    /*E0400*/
  Parameter(StatGrpName,char);     /* group name for printing
                                      task execution time */    /*E0401*/
  Parameter(CallCountPrint,byte);  /* function call
                                      statistics output */    /*E0402*/
  Parameter(IsSynchrTime,byte);    /* calculate time of 
                                      real dissynchronization */    /*E0403*/
  Parameter(IsTimeVariation,byte); /* calculate time of 
                                      potential dissynchronization and
                                      time variation */    /*E0404*/
  Parameter(MaxIntervalLevel,int); /* max level of 
                                      nested intervals */    /*E0405*/
  Parameter(IntervalBarrier,byte); /* */    /*E0406*/
  Parameter(IntermediateIntervalsLevel,int); /* */    /*E0407*/
  Parameter(DVMExitSynchr,byte);   /* */    /*E0408*/

  /* Parameters for calculation of execution time
        of iteration groups in parallel loops     */    /*E0409*/

  Parameter(PLGroupNumber,int); /* for each dimension of processor 
                                   system number of groups of
                                   iteration coordinates */    /*E0410*/
  Parameter(PLTimeTrace,int);   /* output trace of
                                   information on execution time
                                   of iteration groups in parallel loops */    /*E0411*/

  /* */    /*E0412*/

  Parameter(WriteStat,byte);         /* */    /*E0413*/
  Parameter(WriteStatByParts,byte);  /* */    /*E0414*/
  Parameter(WriteStatByFwrite,byte); /* */    /*E0415*/

  /* ------------------------------------------------- */    /*E0416*/

  Parameter(SendRecvTime,byte); /* */    /*E0417*/
  Parameter(SendRecvTimePrint,byte); /* */    /*E0418*/
  /* */    /*E0419*/

  Parameter(StatCompressLevel,int); /* */    /*E0420*/
  Parameter(StatCompressScheme,int); /* */    /*E0421*/

  return;
}


/******************************************************\
* Install support system according to input parameters *
\******************************************************/    /*E0422*/

void  dvm_SysInit(void)
{ int       i, j, MaxSlaveNumber, LinInd;
  char     *CharPtr;
  double    MinWP;
  byte      MaxMsgLengthSign = 0; /* */    /*E0423*/

  /* Delete file current.par */    /*E0424*/

  if(DelCurrentPar == 1 && IAmIOProcess == 0)
  {  if(MPS_CurrentProc == MPS_MasterProc)
        SYSTEM(remove, (CurrentParName))
  }

  if(SysInfo == 0 && MPS_CurrentProc == MPS_MasterProc &&
     IAmIOProcess == 0)
  {  if(DelSysInfo)
        SYSTEM(remove,(SysInfoFileName))/* Delete old file
                                           with information massages */    /*E0425*/

     /* Delete all temporary files
        with information messages  */    /*E0426*/

     for(i=0; i < MaxProcNumber; i++)
     {  GetSysInfoName(i, InfoFileName);

        SYSTEM_RET(j, remove, (InfoFileName))

        if(j != 0)
           break;  /* no more files */    /*E0427*/
     }
  }

  if(IAmIOProcess == 0)
     mps_Barrier(); /* when one processor opens the file
                   with information messages 
                   another processor cannot delete it */    /*E0428*/

  if(IAmIOProcess)
     SysInfoFile = 0;

  /*     Turn off output of information messages into file, if it
     coinsides with the file, in which the stdout stream is redirected */    /*E0429*/

  if(_SysInfoStdOut && SysInfoFile)
  {  /* Information messages directed into the stdout and file */    /*E0430*/

     if(StdOutFile)
     {  SYSTEM_RET(i, strcmp, (CurrStdOutFileName, SysInfoFileName));
     }
     else
        i = 1;

     if(i == 0)
        SysInfoFile = 0;
  }
          
  /*     Turn off output of information messages into file, if it
     coinsides with the file, in which the stderr stream is redirected */    /*E0431*/

  if(_SysInfoStdErr && SysInfoFile)
  {  /* Information messages directed into the stderr and file */    /*E0432*/

     if(StdErrFile)
     {  SYSTEM_RET(i, strcmp, (CurrStdErrFileName, SysInfoFileName));
     }
     else
        i = 1;

     if(i == 0)
        SysInfoFile = 0;
  }
          
  /* Install output of information messages in file */    /*E0433*/

  if(SysInfoFile && SysInfo == 0)
  {  if(SysInfoSepFile)
     {  GetSysInfoName(MPS_CurrentProc, InfoFileName);
        CharPtr = InfoFileName;
     }
     else
        CharPtr = SysInfoFileName;

     SYSTEM_RET(SysInfo, fopen, (CharPtr, OPENMODE(w)))

     DVMSysInfo.File = SysInfo;
     DVMSysInfo.FileID = FilesCount;
     FilesCount++;

     if(SysInfo == NULL)
     {  if(FatInfoNoOpen) 
           eprintf(__FILE__,__LINE__,
                 "*** RTS err 014.004: can't open SysInfo file <%s>\n",
                 CharPtr);
        pprintf(3,
             "*** RTS warning 014.005: can't open SysInfo file <%s>\n",
             CharPtr);
        SysInfoFile = 0;
        FilesCount--;
     }
     else
        _SysInfoFile = 1; /* result flag on
                             output  information messages in file */    /*E0434*/
  }
          
  /* Forming  and checking array of external processor numbers */    /*E0435*/

  ProcNumberList[0]             = 0;
  ProcNumberList[MaxProcNumber] = 0;

  if(ProcListSign == 0 || IAmIOProcess)
     for(i=0; i < MaxProcNumber; i++)
         ProcNumberList[i] = i;

  j = (int)(ProcCount - 1);

  for(MaxSlaveNumber = 0;
      MaxSlaveNumber < j && ProcNumberList[MaxSlaveNumber+1] != 0;
      MaxSlaveNumber++)
      continue;

  if(MaxSlaveNumber < j)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 014.006: length of ProcNumberList (%d) "
          "< processor count(%d)\n",
          MaxSlaveNumber+1, ProcCount);

  for(i=1; i < MaxSlaveNumber; i++)
      for(j=i+1; j < MaxSlaveNumber; j++)
          if(ProcNumberList[j] == ProcNumberList[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 014.007: invalid ProcNumberList\n"
                  "(ProcNumberList[%d] = ProcNumberList[%d] = %d)\n",
                  i, j, ProcNumberList[i]);

  /* Forming  and checking array of  processor coordinate weights */    /*E0436*/

  if(CoordWeightSign == 0 || IAmIOProcess)
  {  /*            Set unit coordinate weights when
        the list of processor coordinate weights is turned off */    /*E0437*/

     j = ProcCount + MAXARRAYDIM;

     for(i=0; i < j; i++)
         CoordWeightList[i] = 1.;

     CoordWeightList[j] = 0.;
  }
  else
  {  /*       Check and norm arrays
        with processor coordinate weights */    /*E0438*/

     MinWP = 1.e7; /* minimal weight in dimension 1 */    /*E0439*/

     j = (int)VMSSize[0];

     for(i=0; i < j; i++)
     { if(CoordWeightList1[i] <= 0.)
          epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 014.008: invalid CoordWeightList1\n"
              "(CoordWeightList1[%d]=%lf)\n", i, CoordWeightList1[i]);

       MinWP = dvm_min(MinWP, CoordWeightList1[i]);
     }

     for(i=0; i < j; i++)
         CoordWeightList1[i] /= MinWP;

     if(VMSRank > 1)
     {
        MinWP = 1.e7; /* minimal weight in dimension 2 */    /*E0440*/

        j = (int)VMSSize[1];

        for(i=0; i < j; i++)
        { if(CoordWeightList2[i] <= 0.)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 014.009: invalid CoordWeightList2\n"
               "(CoordWeightList2[%d]=%lf)\n", i, CoordWeightList2[i]);

          MinWP = dvm_min(MinWP, CoordWeightList2[i]);
        }

        for(i=0; i < j; i++)
            CoordWeightList2[i] /= MinWP;
     }

     if(VMSRank > 2)
     {
        MinWP = 1.e7; /* minimal weight in dimension 3 */    /*E0441*/

        j = (int)VMSSize[2];

        for(i=0; i < j; i++)
        {
          if(CoordWeightList3[i] <= 0.)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 014.010: invalid CoordWeightList3\n"
               "(CoordWeightList3[%d]=%lf)\n", i, CoordWeightList3[i]);

          MinWP = dvm_min(MinWP, CoordWeightList3[i]);
        }

        for(i=0; i < j; i++)
            CoordWeightList3[i] /= MinWP;
     }

     if(VMSRank > 3)
     {
        MinWP = 1.e7; /* minimal weight in dimension 4 */    /*E0442*/

        j = (int)VMSSize[3];

        for(i=0; i < j; i++)
        {
          if(CoordWeightList4[i] <= 0.)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 014.011: invalid CoordWeightList4\n"
               "(CoordWeightList4[%d]=%lf)\n", i, CoordWeightList4[i]);

          MinWP = dvm_min(MinWP, CoordWeightList4[i]);
        }

        for(i=0; i < j; i++)
            CoordWeightList4[i] /= MinWP;
     }

     j = (int)VMSSize[0];

     for(i=0,LinInd=0; i < j; i++,LinInd++)
         CoordWeightList[LinInd] = CoordWeightList1[i];

     if(VMSRank > 1)
     {  j = (int)VMSSize[1];

        for(i=0; i < j; i++,LinInd++)
            CoordWeightList[LinInd] = CoordWeightList2[i];
     }

     if(VMSRank > 2)
     {  j = (int)VMSSize[2];

        for(i=0; i < j; i++,LinInd++)
            CoordWeightList[LinInd] = CoordWeightList3[i];
     }

     if(VMSRank > 3)
     {  j = (int)VMSSize[3];

        for(i=0; i < j; i++,LinInd++)
            CoordWeightList[LinInd] = CoordWeightList4[i];
     }
  }

  /* Forming  and checking array of 
      processor performance weights  */    /*E0443*/

  if(ProcWeightSign == 0 || IAmIOProcess)
  {  for(i=0; i < MaxProcNumber; i++)
         ProcWeightList[i] = 1.;
  }
  else
  {  for(i=0; i < MaxProcNumber; i++)
     {  if(ProcWeightList[i] <= 0.)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 014.020: invalid ProcWeightList\n"
                  "(ProcWeightList[%d]=%lf)\n", i, ProcWeightList[i]);
     }
  }

  MinWP = 1.e7;  /* minimal weight of performance */    /*E0444*/

  for(i=0; i < ProcCount; i++)
  {  ProcWeightArray[i] = ProcWeightList[ProcNumberList[i]];
     MinWP = dvm_min(MinWP, ProcWeightArray[i]);
  }

  for(i=0; i < ProcCount; i++)
      ProcWeightArray[i] /= MinWP;

  /* Installation of time measuring tools */    /*E0445*/

  if(IAmIOProcess != 0)
     MaxMeasureIndex = 1;

  if(MaxMeasureIndex < 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 014.030: invalid MaxMeasureIndex (%d)\n",
              MaxMeasureIndex);

  mac_calloc(MeasureStartTime, double *, MaxMeasureIndex+1,
             sizeof(double), 1);

  if(MeasureStartTime == NULL)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
       "*** RTS err 014.031: no memory for Measure Start Time Array\n");

  mac_calloc(MeasureTraceTime, double *, MaxMeasureIndex+1,
             sizeof(double), 1);

  if(MeasureTraceTime == NULL)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
       "*** RTS err 014.032: no memory for Measure Trace Time Array\n");

  /* Check parameters for calculation of execution time
           of iteration groups in parallel loops        */    /*E0446*/

  for(i=0; i < MaxVMRank; i++)
  {  if(PLGroupNumber[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 014.035: invalid PLGroupNumber\n"
                 "(PLGroupNumber[%d]=%d)\n", i, PLGroupNumber[i]);
  }

  /* ------------------------------------------------ */    /*E0447*/

  #ifndef  _DVM_MPI_

     dopl_MPI_Test = 0; /* flag off: call MPI_Test function 
                           while dopl_ function execution
                           ( if MPS is not MPI ) */    /*E0448*/
     ReqBufSize     = 0;
     MPS_ReqBufSize = 0;

  #else
     if(IAmIOProcess)
        dopl_MPI_Test = 0;

     if(dopl_MPI_Test)
     {  /* Initialisation of the buffer to
             save exchange flag pointers   */    /*E0449*/

        #ifdef _WIN_MPI_

        /* */    /*E0450*/

          ReqBufSize     = 500;
          MPS_ReqBufSize = 500;

        #endif

        dvm_AllocArray(RTL_Request *, ReqBufSize, RequestBuffer);
        dvm_AllocArray(MPS_Request *, MPS_ReqBufSize, MPS_RequestBuffer);
     }
     else
     {  ReqBufSize     = 0;
        MPS_ReqBufSize = 0;
     }

  #endif

  if(InPLQNumber < 2)
     InPLQNumber = 2; /* minimal number of parts internal part
                         of parallel loop can be divaded in */    /*E0451*/

  /* Check parameters of user program processor system */    /*E0452*/
  
  if(IAmIOProcess)
     IsUserPS = 0;

  if(IsUserPS)
  {  /* Dimension of user program 
            processor system      */    /*E0453*/

     for(i=0; i <= MaxVMRank; i++)
     { if(UserPS[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 014.038: invalid UserPS\n"
                    "(UserPS[%d]=%ld)\n", i, UserPS[i]);
        if(UserPS[i] == 0)
           break;
     }

     if(i != VMSRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 014.039: invalid user PS rank\n"
                 "user PS rank (%d) # initial PS Rank (%d)\n",
                 i, (int)VMSRank);

     /* Check if the user program processor system
           is equal to initial processor system    */    /*E0454*/

     for(i=0; i < VMSRank; i++)
     {  if(UserPS[i] != VMSSize[i])
           break;
     }

     if(i == VMSRank)
        IsUserPS = 0; /* user program processor system: flag off */    /*E0455*/
  }

  /* */    /*E0456*/

  #ifdef  _i860_ROU_
     MsgSchedule = 0;
  #endif

  if(ProcCount == 1 || CheckRendezvous)
     MsgSchedule = 0;

  if(MsgSchedule)
  {  MaxMsgParts = 0;

     SendReqColl = coll_Init(SendReqCount, SendReqCount, NULL);

     if(MsgBuf1Length <= 0)
        MsgBuf1Length = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0457*/

     if(MsgBufLength <= 0)
        MsgBufLength = INT_MAX;    /*(int)(((word)(-1)) >> 1);*/    /*E0458*/

     MsgBuf1Length = dvm_min(MsgBuf1Length, MsgBufLength);

     MaxMsgLength = dvm_min(MaxMsgLength, MsgBuf1Length);
     MaxMsgLength = dvm_min(MaxMsgLength, MsgBufLength);

     if(MaxMsgLength <= 0)
        MaxMsgLength = MsgBuf1Length;
     else
        MaxMsgLengthSign = 1; /* */    /*E0459*/

     if(Msk3)
     {  MaxMsgLength = MaxMsgLength >> 2;
        MaxMsgLength = MaxMsgLength << 2;

        if(MaxMsgLength == 0)
           MaxMsgLength = 4;
     }

     if(MaxMsgSendNumber <= 0)
        MaxMsgSendNumber = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0460*/

     if(MaxMsgLengthSign)
        i = MsgBufLength / MaxMsgLength;
     else
        i = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0461*/

     MaxMsgSendNumber = dvm_min(MaxMsgSendNumber, i);/* */    /*E0462*/

     InitMaxMsgSendNumber = MaxMsgSendNumber; /* */    /*E0463*/

     if(ParChanNumber <= 0)
        ParChanNumber = ProcCount - 1;

     ParChanNumber = dvm_min(ParChanNumber, MaxMsgSendNumber);
     ParChanNumber = dvm_max(ParChanNumber, 1);

     dvm_AllocArray(int, ParChanNumber, ChanMsgSendNumber);
     dvm_AllocArray(RTL_Request *, ParChanNumber, ChanRTL_ReqPtr);
     dvm_AllocArray(int, ParChanNumber, PlanChanList);
     dvm_AllocArray(int, ParChanNumber, MsgChanList);

     if(MaxMsgLengthSign)
        j = MsgBuf1Length / MaxMsgLength;
     else
        j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0464*/

     if(MaxMsg1SendNumber <= 0)
        MaxMsg1SendNumber = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0465*/

     #ifdef  _DVM_ROU_
        MaxMsg1SendNumber = dvm_min(MaxMsg1SendNumber, 15);
     #endif

     MaxMsg1SendNumber = dvm_min(j, MaxMsg1SendNumber);

     for(i=0; i < ParChanNumber; i++)
     {  ChanMsgSendNumber[i] = MaxMsg1SendNumber;
        ChanRTL_ReqPtr[i] = NULL;
     }

     FreeChanNumber = ParChanNumber; /* */    /*E0466*/
     NewMsgNumber   = 0;   /* */    /*E0467*/

     /* */    /*E0468*/

     if(ResCoeff <= 0. || ResCoeff > 1.)
        ResCoeff = 1.;
     if(ResCoeffDoPL <= 0. || ResCoeffDoPL > 1.)
        ResCoeffDoPL = 1.;
     if(ResCoeffTstReq <= 0. || ResCoeffTstReq > 1.)
        ResCoeffTstReq = 1.;
     if(ResCoeffWaitReq <= 0. || ResCoeffWaitReq > 1.)
        ResCoeffWaitReq = 1.;
     if(ResCoeffDACopy <= 0. || ResCoeffDACopy > 1.)
        ResCoeffDACopy = 1.;
  }

  /* */    /*E0469*/

  if(MsgPartReg < 0)
     MsgPartReg = 0;
  if(MaxMsgLength < 0)
     MaxMsgLength = 0;
  if(MaxMsgParts < 0)
     MaxMsgParts = 0;

  if(Msk3)
  {  if(MaxMsgLength && MsgSchedule == 0)
     {  MaxMsgLength = MaxMsgLength >> 2;
        MaxMsgLength = MaxMsgLength << 2;

        if(MaxMsgLength == 0)
           MaxMsgLength = 4;
     }
  }

  #ifdef  _DVM_ROU_
     if(MaxMsgParts == 0)
        MaxMsgParts = 4;
     else
        MaxMsgParts = dvm_min(MaxMsgParts, 4);

     if(MaxMsgParts == 3)
        MaxMsgParts = 2;
  #endif

  if(CheckRendezvous || MaxMsgParts == 1)
  {  MsgPartReg = 0;

     if(MsgSchedule == 0)
        MaxMsgLength = 0;

     MaxMsgParts = 0;
  }

  /* */    /*E0470*/

  if(MsgPairNumber <= 0)
     MsgPairNumber = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0471*/

  /* */    /*E0472*/

  i = 1;
  CharPtr = (char *)&i;

  if(CharPtr[0] == 1)
     InversByteOrder = 1;
  else  
     InversByteOrder = 0;

  /* */    /*E0473*/

  for(i=0; i < CompressBufNumber; i++)
  {  FreeCompressBuf[i] = 1;  /* */    /*E0474*/
     CompressBuf[i]     = NULL;
     CompressBufSize[i] = 0;
  }

  if(CompressCoeff < 0.0f)
     CompressCoeff = 0.0f;
  if(CompressCoeff >= 1.0f)
     CompressCoeff = 0.999999f;

  return;
}
      
/**************************************************\
* Form in OutFileName file name for processor with *
* number ProcNumber to output information messages *
\**************************************************/    /*e0475*/

void  GetSysInfoName(int  ProcNumber, char  *OutFileName)
{ char  *NamePtr, *CharPtr;

  NamePtr = SysInfoFileName;

  #ifdef _UNIX_
     SYSTEM_RET(CharPtr, strrchr, (SysInfoFileName, '/'))
  #else
     SYSTEM_RET(CharPtr, strrchr, (SysInfoFileName, '\\'))
  #endif

  if(CharPtr)
     NamePtr = CharPtr+1;

  for(CharPtr=SysInfoFileName; CharPtr != NamePtr;
                               CharPtr++,OutFileName++)
      *OutFileName = *CharPtr;
  SYSTEM(sprintf, (OutFileName, "%d.out", ProcNumber))
  return;
}


/********************************************************\
* Correct parameters for output of  information messages *
\********************************************************/    /*E0476*/

void  dvm_CorrOut(void)
{ int     i, j, all;
  FILE   *F;
  char   *CharPtr, *ExtPtr;
  char    FileName[256];

  /* Set flags on multiprocessor
      output of error messages   */    /*E0477*/

  switch(MultiProcErrReg)
  {  case 0:  MultiProcErrReg1 = 0;
              MultiProcErrReg2 = 0;
              break;
     case 1:  MultiProcErrReg1 = 0;
              MultiProcErrReg2 = 1;
              break;
     case 2:  MultiProcErrReg1 = 1;
              MultiProcErrReg2 = 1;
              break;
  }

  if(IAmIOProcess)
     return;
       
  /* Change direction of  standard streams in files and define
     characteristics of output streams for information messages */    /*E0478*/

  for(i=0; i < CurrentParNumber; i++)
  {  if(CurrentPar[i][0] != '+')
        continue;

     switch(CurrentPar[i][1])
     {
        case 'o':
        case 'O': if(IsStdOutFile)
                     break;      /* stdout has already changed direction */    /*E0479*/

                  IsStdOutFile = 1; /* flag: stdout was redirected */    /*E0480*/

                  SYSTEM_RET(ExtPtr, strchr, (&CurrentPar[i][1], '.'))
                  if(ExtPtr)   /* if extension is  defined */    /*E0481*/
                  {  *ExtPtr = '\x00';
                     ExtPtr++;
                  }
                  else
                     ExtPtr = "sto";

                  if(CurrentPar[i][2] == '*')
                  {  all = 1; /* stdout was redirected
                                 on every processor */    /*E0482*/

                     if(CurrentPar[i][3] == '\x00')
                     {  SYSTEM_RET(j, sprintf,
                                   (FileName, "%d.sto",
                                    MPS_CurrentProc))
                     }
                     else
                     {  SYSTEM_RET(j, sprintf,
                                   (FileName, "%s%d.%s",
                                    &CurrentPar[i][3], MPS_CurrentProc,
                                    ExtPtr))
                     }

                     FileName[j] = '\x00';
                     CharPtr = FileName;
                  }
                  else
                  {  if(CurrentPar[i][2] != '+')
                     {  all = 0; /* stdout was redirected
                                    on processor with number
                                    MPS_MasterProc */    /*E0483*/

                        if(CurrentPar[i][2] == '\x00')
                           CharPtr = "stdout";
                        else
                        {  SYSTEM_RET(j, sprintf,
                                      (FileName, "%s.%s",
                                       &CurrentPar[i][2], ExtPtr))
                           FileName[j] = '\x00';
                           CharPtr = FileName;
                        }
                     }
                     else
                     {  all = 1; /* stdout was redirected
                                    on every processor */    /*E0484*/

                        if(CurrentPar[i][3] == '\x00')
                           CharPtr = "stdout";
                        else
                        {  SYSTEM_RET(j, sprintf,
                                      (FileName, "%s.%s",
                                       &CurrentPar[i][3], ExtPtr))
                           FileName[j] = '\x00';
                           CharPtr = FileName;
                        }
                     }
                  }

                  if((MPS_CurrentProc == MPS_MasterProc || all) &&
                     DelStdStream)
                     SYSTEM(remove, (CharPtr))/* delete old files
                                                 with stdout stream */    /*E0485*/

                  mps_Barrier(); /* when file is open with flow 
                                    another processor cannot delete it */    /*E0486*/

                  SYSTEM(strcpy, (CurrStdOutFileName, CharPtr))

                  if(MPS_CurrentProc == MPS_MasterProc || all)
                  {  SYSTEM_RET(F,freopen,(CharPtr,OPENMODE(w),stdout));

                     if(F == NULL)
                        eprintf(__FILE__,__LINE__,
                                "*** RTS err 014.040: can not "
                                "open stdout file <%s>\n", CharPtr);
                     StdOutFile = 1;
                  }

                  break;

        case 'e':
        case 'E': if(IsStdErrFile)
                     break;      /* stderr has already changed direction */    /*E0487*/

                  IsStdErrFile = 1; /* flag: stderr  was redirected */    /*E0488*/

                  SYSTEM_RET(ExtPtr, strchr, (&CurrentPar[i][1], '.'))
                  if(ExtPtr)   /* if extension is  defined */    /*E0489*/
                  {  *ExtPtr = '\x00';
                     ExtPtr++;
                  }
                  else
                     ExtPtr = "ste";

                  if(CurrentPar[i][2] == '*')
                  {  all = 1; /* stderr was redirected
                                 on every processor */    /*E0490*/

                     if(CurrentPar[i][3] == '\x00')
                     {  SYSTEM_RET(j, sprintf,
                                   (FileName, "%d.ste",
                                    MPS_CurrentProc))
                     }
                     else
                     {  SYSTEM_RET(j, sprintf,
                                   (FileName, "%s%d.%s",
                                    &CurrentPar[i][3], MPS_CurrentProc,
                                    ExtPtr))
                     }

                     FileName[j] = '\x00';
                     CharPtr = FileName;
                  }
                  else
                  {  if(CurrentPar[i][2] != '+')
                     {  all = 0; /* stderr was redirected
                                    on processor with number
                                    MPS_MasterProc */    /*E0491*/

                        if(CurrentPar[i][2] == '\x00')
                           CharPtr = "stderr";
                        else
                        {  SYSTEM_RET(j, sprintf,
                                      (FileName, "%s.%s",
                                       &CurrentPar[i][2], ExtPtr))
                           FileName[j] = '\x00';
                           CharPtr = FileName;
                        }
                     }
                     else
                     {  all = 1; /* stderr was redirected
                                    on every processor */    /*E0492*/

                        if(CurrentPar[i][3] == '\x00')
                           CharPtr = "stderr";
                        else
                        {  SYSTEM_RET(j, sprintf,
                                      (FileName, "%s.%s",
                                       &CurrentPar[i][3], ExtPtr))
                           FileName[j] = '\x00';
                           CharPtr = FileName;
                        }
                     }
                  }

                  if((MPS_CurrentProc == MPS_MasterProc || all) &&
                     DelStdStream)
                     SYSTEM(remove, (CharPtr))/* delete old file 
                                                 with stderr stream */    /*E0493*/

                  mps_Barrier(); /* when file is open with flow 
                                    another processor cannot delete it */    /*E0494*/

                  SYSTEM(strcpy, (CurrStdErrFileName,CharPtr))

                  if(MPS_CurrentProc == MPS_MasterProc || all)
                  {  SYSTEM_RET(F,freopen,(CharPtr,OPENMODE(w),stderr));

                     if(F == NULL)
                        eprintf(__FILE__,__LINE__,
                                "*** RTS err 014.041: can not "
                                "open stderr file <%s>\n", CharPtr);
                     StdErrFile = 1;
                  }

                  break;

        case 'i':
        case 'I': SysInfoStdOut = 0;
                  SysInfoStdErr = 0;
/*                  SysInfoFile = 0;*/    /*E0495*/

                  for(j=2; j < 5 && CurrentPar[i][j] != '\x00'; j++)
                  {  switch(CurrentPar[i][j])
                     {  case 'o': SysInfoStdOut = 1;
                                  break;
                        case 'e': SysInfoStdErr = 1;
                                  break;
                        case 'f': SysInfoFile = 1;
                                  break;
                     }
                  }

                  break;
     }
  }

  /*      Redirection  of standard output 
     streams with the syspar.* file parameters */    /*E0496*/

  if(IsStdOutFile == 0 && StdOutToFile) /* if the stdout stream is not redirected yet */    /*E0497*/
  {  IsStdOutFile = 1; /* flag: stdout  was redirected */    /*E0498*/

     SYSTEM_RET(ExtPtr, strchr, (StdOutFileName, '.'))
     if(ExtPtr)   /* if extension is  defined */    /*E0499*/
     {  *ExtPtr = '\x00';
        ExtPtr++;
     }
     else
        ExtPtr = "sto";

     if(StdOutFileName[0] == '*')
     {  all = 1; /* stdout was redirected on every processor */    /*E0500*/

        if(StdOutFileName[1] == '\x00')
        {  SYSTEM_RET(j, sprintf, (FileName, "%d.sto", MPS_CurrentProc))
        }
        else
        {  SYSTEM_RET(j, sprintf,
                      (FileName, "%s%d.%s",
                       &StdOutFileName[1], MPS_CurrentProc, ExtPtr))
        }

        FileName[j] = '\x00';
        CharPtr = FileName;
     }
     else
     {  if(StdOutFileName[0] != '+')
        {  all = 0; /* stdout was redirected on processor with number
                                    MPS_MasterProc */    /*E0501*/

           if(StdOutFileName[0] == '\x00')
              CharPtr = "stdout";
           else
           {  SYSTEM_RET(j, sprintf,
                         (FileName, "%s.%s", &StdOutFileName[0], ExtPtr))
              FileName[j] = '\x00';
              CharPtr = FileName;
           }
        }
        else
        {  all = 1; /* stdout was redirected on every processor */    /*E0502*/

           if(StdOutFileName[1] == '\x00')
              CharPtr = "stdout";
           else
           {  SYSTEM_RET(j, sprintf,
                         (FileName, "%s.%s", &StdOutFileName[1], ExtPtr))
              FileName[j] = '\x00';
              CharPtr = FileName;
           }
        }
     }

     if((MPS_CurrentProc == MPS_MasterProc || all) &&
        DelStdStream)
        SYSTEM(remove, (CharPtr)) /* delete old file 
                                     with the stdout stream */    /*E0503*/

     mps_Barrier(); /* when file is open with flow 
                       another processor cannot delete it */    /*E0504*/

     SYSTEM(strcpy, (CurrStdOutFileName,CharPtr))

     if(MPS_CurrentProc == MPS_MasterProc || all)
     {  SYSTEM_RET(F,freopen,(CharPtr,OPENMODE(w),stdout));

        if(F == NULL)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 014.040: can not "
                   "open stdout file <%s>\n", CharPtr);
        StdOutFile = 1;
     }
  }

  if(IsStdErrFile == 0 && StdErrToFile) /* if the stderr stream is not redirected yet */    /*E0505*/
  {  IsStdErrFile = 1; /* flag: stderr  was redirected */    /*E0506*/

     SYSTEM_RET(ExtPtr, strchr, (StdErrFileName, '.'))
     if(ExtPtr)   /* if extension is  defined */    /*E0507*/
     {  *ExtPtr = '\x00';
        ExtPtr++;
     }
     else
        ExtPtr = "ste";

     if(StdErrFileName[0] == '*')
     {  all = 1; /* stderr is redirected on every processor */    /*E0508*/

        if(StdErrFileName[1] == '\x00')
        {  SYSTEM_RET(j, sprintf,
                      (FileName, "%d.ste", MPS_CurrentProc))
        }
        else
        {  SYSTEM_RET(j, sprintf,
                      (FileName, "%s%d.%s",
                       &StdErrFileName[1], MPS_CurrentProc, ExtPtr))
        }

        FileName[j] = '\x00';
        CharPtr = FileName;
     }
     else
     {  if(StdErrFileName[0] != '+')
        {  all = 0; /* stderr was redirected on processor with number
                                    MPS_MasterProc */    /*E0509*/

           if(StdErrFileName[0] == '\x00')
              CharPtr = "stderr";
           else
           {  SYSTEM_RET(j, sprintf,
                         (FileName, "%s.%s", &StdErrFileName[0], ExtPtr))
              FileName[j] = '\x00';
              CharPtr = FileName;
           }
        }
        else
        {  all = 1; /* stderr is redirected on every processor */    /*E0510*/

           if(StdErrFileName[1] == '\x00')
              CharPtr = "stderr";
           else
           {  SYSTEM_RET(j, sprintf,
                         (FileName, "%s.%s", &StdErrFileName[1], ExtPtr))
              FileName[j] = '\x00';
              CharPtr = FileName;
           }
        }
     }

     if((MPS_CurrentProc == MPS_MasterProc || all) &&
        DelStdStream)
        SYSTEM(remove, (CharPtr)) /* delete old file
                                     with the stderr stream */    /*E0511*/

     mps_Barrier(); /* when file is open with flow 
                       another processor cannot delete it */    /*E0512*/

     SYSTEM(strcpy, (CurrStdErrFileName,CharPtr))

     if(MPS_CurrentProc == MPS_MasterProc || all)
     {  SYSTEM_RET(F, freopen, (CharPtr,OPENMODE(w), stderr))

        if(F == NULL)
           eprintf(__FILE__,__LINE__,
                   "*** RTS err 014.041: can not "
                   "open stderr file <%s>\n", CharPtr);
        StdErrFile = 1;
     }
  }

  /* ------------------------------------------------- */    /*E0513*/

  _SysInfoStdOut = SysInfoStdOut;
  _SysInfoStdErr = SysInfoStdErr;

  if(SysInfoStdErr && SysInfoStdOut)
  {
    /* Information messages to be output into both streams */    /*E0514*/

    if(StdErrFile && StdOutFile)
    {
       SYSTEM_RET(i, strcmp, (CurrStdOutFileName,CurrStdErrFileName));
    }
    else
       i = 1;

    if((!StdErrFile && !StdOutFile) || (StdErrFile && StdOutFile && !i))
       #ifdef _INFO_STDERR_
          _SysInfoStdOut = 0; /* turn off flag on output of information
                                 messages into stdout */    /*E0515*/
       #else
          _SysInfoStdErr = 0; /* turn off flag on output of information
                                 messages into stderr */    /*E0516*/
       #endif
  }

  /* Block the output of error messages into the stderr stream
      if stdout and stderr are redirected into the same file   */    /*E0517*/

  if(StdOutFile && StdErrFile)
  {
     SYSTEM_RET(i, strcmp, (CurrStdOutFileName, CurrStdErrFileName));

     if(i == 0)
        NoBlStdErr = 0;
  }

  /* Process input parameter SysInfoPrint */    /*E0518*/

  if(SysInfoPrint != 255)
  {
     InpSysInfoPrint = 1;  /* flag:
                              SysInfoPrint parameter has been input */    /*E0519*/
     if(FirstParDir == NULL)
     {
        /* SysInfoPrint parameter has been input from base directory */    /*E0520*/

        #ifndef _INIT_INFO_

          SaveSysInfoPrint = SysInfoPrint; /* save SysInfoPrint */    /*E0521*/
          InpSysInfoPrint = 0;        /* flag:
                                         SysInfoPrint parameter has not
                                         been input */    /*E0522*/
        #else

          _SysInfoPrint = SysInfoPrint;

        #endif
     }
     else
        _SysInfoPrint = SysInfoPrint;
  }

  SysInfoPrint = 255; /* if SysInfoPrint parameter be input */    /*E0523*/
   
  return;
}


/****************************************************\
* Check group name and its number  for time printing *
\****************************************************/    /*E0524*/

void  CheckGrpName(char *FileName)
{
  int  i, ErrSign;

  for(i=0; i < StatGrpCount; i++)
  {
     SYSTEM_RET(ErrSign, strcmp, (StatGrpName, GrpName[i]))

     if(!ErrSign)
        break;        /* group is found */    /*E0525*/
  }

  if(i == StatGrpCount)
  {
     pprintf(2+MultiProcErrReg1,
             "*** RTS err 014.050: invalid statistics group name <%s> "
             "(parameter file <%s>)\n\n",StatGrpName, FileName);
     pprintf(2+MultiProcErrReg1,"StatGrpName:\n\n");

     for(i=0; i < StatGrpCount; i++)
         pprintf(2+MultiProcErrReg1,"%s  ",GrpName[i]);

     pprintf(2+MultiProcErrReg1," \n");

     RTS_Call_MPI = 1;

#ifdef _MPI_PROF_TRAN_

     if(1 /*CallDbgCond*/    /*E0526*/ /*EnableTrace && dvm_OneProcSign*/    /*E0527*/)
        SYSTEM(MPI_Finalize, ())
     else
        dvm_exit(1);

#else

     dvm_exit(1);

#endif

  }

  StatGrpNumber = i;  /* group number */    /*E0528*/ 

  return; 
}


/************************************************************\
* Current file name (current.par) is input from command line *
\************************************************************/    /*E0529*/

void  GetCurrentParName(int  argc, char  *argv[])
{
  int      i, j;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0530*/

  for(; i < argc; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "cp", 2))

     if(j == 0)
     { 
        /* Parameter with current file name current.par is found */    /*E0531*/

        if(i == (argc-1) || argv[i+1][0] == Minus)
        {
           /* -cp - last parameter or next parameter which is not a name.
                              current.par is off                          */    /*E0532*/

           DeactBaseDir    = 1; /* input of base parameter set  is off */    /*E0533*/
           DeactUserPar    = 1; /* input of directories adn files with parameters for 
                                   correction if off */    /*E0534*/
           DeactCurrentPar = 1; /* input of the file current.par is off */    /*E0535*/
        }
        else
        {
           /* Copy current file name current.par */    /*E0536*/

           SYSTEM(strcpy, (CurrentParName, &argv[i+1][0]))
        }

        break;
     }
  }

  return;
}


/**********************************************\
* Input flags of directory and parameter files *
*     switching-off from the command line      *
\**********************************************/    /*E0537*/

void  GetDeactPar(int  argc, char  *argv[])
{
  int      i, j, n;

  if ((i = GetDVMPar(argc, argv)) < 0)
  {
	  // default: dvm -deact cp
	  DeactBaseDir = 1; /* base parameter set is off */    /*E0544*/
	  DeactUserPar = 1; /* directories  and parameter files
						for correction are off */    /*E0545*/
	  DeactCurrentPar = 1; /* file current.par is off */    /*E0546*/

	  DeactBaseDir = 1; /* base parameter set is off */    /*E0542*/
	  DeactUserPar = 1; /* directories  and parameter files
						for correction are off */    /*E0543*/
	  return;     /* parameter "dvm" has not been found */    /*E0538*/
  }
     

  n = argc - 1;

  for(; i < n; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "deact", 5))

     if(j == 0)
     {  /* Parameter defining switching-off (input is blocked)
                  of directories and parameter files           */    /*E0539*/

        if(argv[i+1][0] == 's')
           DeactBaseDir = 1; /* base parameter set is off */    /*E0540*/
        if(argv[i+1][0] == 'u')
           DeactUserPar = 1; /* directories  and parameter files
                                for correction are off */    /*E0541*/
        if(argv[i+1][0] == 'p')
        {  DeactBaseDir = 1; /* base parameter set is off */    /*E0542*/
           DeactUserPar = 1; /* directories  and parameter files
                                for correction are off */    /*E0543*/
        }

        if(argv[i+1][0] == 'c')
        {  DeactBaseDir = 1; /* base parameter set is off */    /*E0544*/
           DeactUserPar = 1; /* directories  and parameter files
                                for correction are off */    /*E0545*/
           DeactCurrentPar = 1; /* file current.par is off */    /*E0546*/
        }

        continue;
     }
  }

  return;
}


/**********************************************\
* Input flags of directory and parameter files *
*     switching-off from the command line      *
\**********************************************/    /*E0547*/

void  GetActPar(int  argc, char  *argv[])
{
  int      i, j, n;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0548*/

  n = argc - 1;

  for(; i < n; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "act", 3))

     if(j == 0)
     {  /* Parameter defining directory and parameter files
           switching-on is found*/    /*E0549*/

        if(argv[i+1][0] == 's')
           DeactBaseDir = 0; /* base parameter set is on */    /*E0550*/
        if(argv[i+1][0] == 'u')
           DeactUserPar = 0; /* directories  and parameter files
                                for correction are on */    /*E0551*/
        if(argv[i+1][0] == 'p')
        {  DeactBaseDir = 0; /* base parameter set is on */    /*E0552*/
           DeactUserPar = 0; /* directories  and parameter files
                                for correction are on*/    /*E0553*/
        }

        if(argv[i+1][0] == 'c')
        {
           DeactCurrentPar = 0; /* file current.par is on */    /*E0554*/

           if(argv[i+1][1] == 's')
              DeactBaseDir = 0; /* base parameter set is on */    /*E0555*/
           if(argv[i+1][1] == 'u')
              DeactUserPar = 0; /* directories  and parameter files
                                   for correction are on*/    /*E0556*/
           if(argv[i+1][1] == 'p')
           {  DeactBaseDir = 0;
              DeactUserPar = 0;
           }
        }

        continue;
     }
  }

  return;
}


/******************************************************************\
*  Size of initial processor system is input from command line     *
\******************************************************************/    /*E0557*/

void  GetInitialPS(int  argc, char  *argv[])
{
  int      i, j, k, PSRank = -1;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0558*/

  if(IAmIOProcess == 0)
  {
     for(; i < argc; i++)
     {
        SYSTEM_RET(j, isdigit, (argv[i][0]))

        if(j)
        {
           /* First command line parameter beginning with digit is fouind */    /*E0559*/

           for(j=0; (i < argc && j < MaxVMRank); i++,j++)
           {
              SYSTEM_RET(k, isdigit, (argv[i][0]))

              if(k)
              {
                 /* Parameter is beginning with digit*/    /*E0560*/

                 SYSTEM_RET(VMSSize[j], atol, (argv[i]))

                 if(VMSSize[j] < 1)
                    break;  /* dimension size is equal to zero -
                            end of the list of dimension sizes */    /*E0561*/
                 VMSSize[j+1] = 0;
                 PSRank = j+1;
                 continue;
              }
              else
                 break;
           }

           break;
        }
     }
  }

  if(PSRank < 0 && MPS_TYPE == MPI_MPS_TYPE)
  {
     /* Initial processor system sizes are not found
        in the command line during MPI execution */    /*E0562*/

     PSRank     = 3;
     VMSSize[0] = ProcCount; /* number of processors */    /*E0563*/
     VMSSize[1] = 1;
     VMSSize[2] = 1;
     VMSSize[3] = 0;
  }

  if(dvm_OneProcSign)
  {
     for(i=0; VMSSize[i] != 0; i++)
         VMSSize[i] = 1;

     PSRank = i;

     ProcCount = 1;
  }

  return;
}


/*****************************************************************\
*    Number of additional attempts to open parameter file and     *
* standard user program trace file is input from the command line *
\*****************************************************************/    /*E0564*/

void  GetFopenCount(int  argc, char  *argv[])
{ int      i, j;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0565*/

  for(; i < argc; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "opf", 3))

     if(j != 0)
        continue;

     /* Parameter defining the number of additional attempts 
                  to open parameter file is found            */    /*E0566*/

     SYSTEM_RET(j, isdigit, (argv[i][4]))

     if(j == 0)
        continue; /* non digit follows "-opf" */    /*E0567*/

     /* digit follows "-opf" */    /*E0568*/

     j = -1;
        
     SYSTEM_RET(j, atoi, (&argv[i][4]))
     if(j < 0)
        continue; /* wrong number of additional attempts is defining */    /*E0569*/

     ParFileOpenCount = j+1; /* the number of attempts 
                                to open parameter */    /*E0570*/
  }

  return;
}


/************************************************\
*  Standard user program trace file name and     *
*     additives to new trace file names          *
*      are input from the command line           *
\************************************************/    /*E0571*/

void  GetTraceFileName(int  argc, char  *argv[])
{ int      i, j;

  if( (i = GetDVMPar(argc, argv)) < 0)
     return;     /* parameter "dvm" has not been found */    /*E0572*/

  for(; i < argc; i++)
  {
     if(argv[i][0] != Minus)
        continue;

     SYSTEM_RET(j, strncmp, (&argv[i][1], "tfn", 3))

     if(j != 0)
        continue;

     /* Parameter defining user program trace file name is found */    /*E0573*/

     if(argv[i][4] == '+')
     {  /* Left part of new trace file names is defined */    /*E0574*/

        SYSTEM(strcpy, (TraceOptions.OutputTracePrefix, &argv[i][5]))
     }
     else
     {  /* Standard file name is defined for comparison of traces */    /*e0575*/

        SYSTEM(strcpy, (TraceOptionsTraceFile, &argv[i][4]))
     }
  }

  return;
}


/*************************************************************\
*          Search parameter "dvm" in command line             *
*  Return values: >= 0 - index of the found parameter;        *
                <  0 - parameter "dvm" has not been found.    *
\*************************************************************/    /*E0576*/

int  GetDVMPar(int  argc, char  *argv[])
{ int      i, j;

  for(i=0; i < argc; i++)
  {  SYSTEM_RET(j, strncmp, (argv[i], "dvm", 3))

     #ifdef  _WIN_MPI_
        break;
     #else
        if(j == 0)
           break;
     #endif
  }

  if(i == argc)
     return  -1;
  return  i;
}


#endif   /* _GETPAR_C_ */    /*E0577*/
