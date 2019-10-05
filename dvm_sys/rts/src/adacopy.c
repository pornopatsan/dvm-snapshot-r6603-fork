#ifndef _ADACOPY_C_
#define _ADACOPY_C_
/*****************/    /*E0000*/

/**********************************************************\
* Functions for asynchronous copuing of distributed arrays *
\**********************************************************/    /*E0001*/

DvmType __callstd aarrcp_(DvmType FromArrayHeader[], DvmType FromInitIndexArray[],
                          DvmType FromLastIndexArray[],DvmType FromStepArray[],
                          DvmType ToArrayHeader[], DvmType ToInitIndexArray[],
                          DvmType ToLastIndexArray[], DvmType ToStepArray[],
                          DvmType *CopyRegimPtr, AddrType *CopyFlagPtr)

/*        
     Asynchronous copying of distributed arrays.
     -------------------------------------------

FromArrayHeader    - header of read distributed array.
FromInitIndexArray - array: i- element contains initial index value for
                     (i+1)- dimension of read array.
FromLastIndexArray - array: i- element contains final index value for
                     (i+1)- dimension of read array.
FromStepArray      - array: i- element contains index step for  
                     (i+1)- dimension of read array.
ToArrayHeader      - header of written distributed array.
ToInitIndexArray   - array: j- element contains initial index value for
                     (j+1)- dimension of written array.
ToLastIndexArray   - array: j- element contains final index value for  
                     (j+1)- dimension of written array.
ToStepArray        - array: j- element contains index step for  
                     (j+1)- dimension of written array.
*CopyRegimPtr      - copy mode.
CopyFlagPtr        - the pointer to the complete operation flag.

Function returns account of copied elements.
*/    /*E0002*/

{ SysHandle     *FromArrayHandlePtr, *ToArrayHandlePtr;
  s_DISARRAY    *FromDArr, *ToDArr;
  int            FromRank, ToRank, i, j, n, RC;
  DvmType           OutFromInitIndex[MAXARRAYDIM],
                 OutFromLastIndex[MAXARRAYDIM],
                 OutFromStep[MAXARRAYDIM],
                 OutToInitIndex[MAXARRAYDIM],
                 OutToLastIndex[MAXARRAYDIM],
                 OutToStep[MAXARRAYDIM],
                 Weight[MAXARRAYDIM];
  DvmType           Res, FromBSize, ToBSize;
  s_BLOCK        ReadBlock, WriteBlock;
  s_AMVIEW      *FromAMV, *ToAMV;
  byte           Step = 1;

  if(ADACopySynchr)
     (RTL_CALL, bsynch_()); /* */    /*E0003*/

  DVMFTimeStart(call_aarrcp_);

  /* forward to the next element of message tag circle 
     tag_DACopy for the current processor system */    /*E0004*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0005*/

  *CopyFlagPtr = (AddrType)NULL;

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 096.000: wrong call aarrcp_\n"
              "(FromArray and ToArray are not distributed arrays;\n"
              "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
              (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     /* copy distributed array -> memory */    /*E0006*/

     FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;
     FromRank = FromDArr->Space.Rank;
     FromAMV = FromDArr->AMView;

     if(RTL_TRACE)
     {  dvm_trace(call_aarrcp_,
                "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                "ToBufferPtr=%lx; "
                "CopyRegim=%ld; CopyFlagPtr=%lx;\n",
                (uLLng)FromArrayHeader, FromArrayHeader[0],
                (uLLng)ToArrayHeader, *CopyRegimPtr, (uLLng)CopyFlagPtr);

        if(TstTraceEvent(call_aarrcp_))
        {  for(i=0; i < FromRank; i++)
               tprintf("FromInitIndexArray[%d]=%ld; ",
                       i,FromInitIndexArray[i]);
           tprintf(" \n"); 
           for(i=0; i < FromRank; i++)
               tprintf("FromLastIndexArray[%d]=%ld; ",
                       i,FromLastIndexArray[i]);
           tprintf(" \n"); 
           for(i=0; i < FromRank; i++)
               tprintf("     FromStepArray[%d]=%ld; ",
                       i,FromStepArray[i]);
           tprintf(" \n"); 
           tprintf(" \n");

           /* local part of read array is output in trace */    /*E0007*/

           if(dacopy_Trace)
           {  if(FromDArr->HasLocal)
              {  for(i=0; i < FromRank; i++)
                     tprintf("FromLocal[%d]: Lower=%ld Upper=%ld\n",
                             i, FromDArr->Block.Set[i].Lower,
                                FromDArr->Block.Set[i].Upper);
              }
              else
                 tprintf("No FromLocal\n");

              tprintf(" \n");
              tprintf("FromRepl=%d FromPartRepl=%d FromEvery=%d\n\n",
                      (int)FromDArr->Repl, (int)FromDArr->PartRepl,
                      (int)FromDArr->Every);
           }
        }
     }

     if(FromAMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 096.004: wrong call aarrcp_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     Res = GetIndexArray(FromArrayHandlePtr, FromInitIndexArray,
                         FromLastIndexArray, FromStepArray,
                         OutFromInitIndex, OutFromLastIndex,
                         OutFromStep, 0);
     if(Res == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 096.001: wrong call aarrcp_\n"
                 "(invalid from index or from step; "
                 "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     if(EnableDynControl)
        dyn_DisArrTestVal(FromArrayHandlePtr, OutFromInitIndex,
                          OutFromLastIndex, OutFromStep);

     NotSubsystem(i, DVM_VMS, FromAMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 096.006: wrong call aarrcp_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)FromAMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

     for(i=0; i < FromRank; i++)
         if(OutFromStep[i] != 1)
            break;

     if(i == FromRank)
     {  /* step on all dimensions of read array - 1 */    /*E0008*/

        ReadBlock = block_InitFromArr((byte)FromRank,
                                       OutFromInitIndex,
                                       OutFromLastIndex,
                                       OutFromStep);
        if(*CopyRegimPtr)
        {  if(*CopyRegimPtr < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 096.002: wrong call aarrcp_\n"
                       "(CopyRegim=%ld < 0)\n", *CopyRegimPtr);

           if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = IOGetBlockRepl((char *)ToArrayHeader,FromDArr,
                                   &ReadBlock);
           else
              Res = AIOGetBlock((char *)ToArrayHeader, FromDArr,
                                &ReadBlock, CopyFlagPtr);
        }
        else
        {  if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = GetBlockRepl((char *)ToArrayHeader,FromDArr,
                                 &ReadBlock);
           else
              Res = AGetBlock((char *)ToArrayHeader,FromDArr,
                              &ReadBlock, CopyFlagPtr);
        }
     }
     else
     {  /* step on dimensions of read array is not always equal to 1*/    /*E0009*/

        ReadBlock = block_InitFromArr((byte)FromRank,
                                       OutFromInitIndex,
                                       OutFromLastIndex,
                                       OutFromStep);
        if(*CopyRegimPtr)
        {  if(*CopyRegimPtr < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 096.002: wrong call aarrcp_\n"
                       "(CopyRegim=%ld < 0)\n", *CopyRegimPtr);

           if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = IOGetBlockRepl((char *)ToArrayHeader,FromDArr,
                                   &ReadBlock);
           else
              Res = AIOGetBlock((char *)ToArrayHeader, FromDArr,
                                &ReadBlock, CopyFlagPtr);
        }
        else
        {  if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = GetBlockRepl((char *)ToArrayHeader,FromDArr,
                                 &ReadBlock);
           else
              Res = AGetBlock((char *)ToArrayHeader,FromDArr,
                              &ReadBlock, CopyFlagPtr);
        }
     }
  }
  else
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
     {  /* copy memory -> distributed array */    /*E0010*/

        ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;
        ToRank = ToDArr->Space.Rank;
        ToAMV = ToDArr->AMView;

        if(RTL_TRACE)
        {  dvm_trace(call_aarrcp_,
                   "FromBufferPtr=%lx; ToArrayHeader=%lx; "
                   "ToArrayHandlePtr=%lx; "
                   "CopyRegim=%ld; CopyFlagPtr=%lx;\n",
                   (uLLng)FromArrayHeader, (uLLng)ToArrayHeader,
                   ToArrayHeader[0], *CopyRegimPtr, (uLLng)CopyFlagPtr);
           if(TstTraceEvent(call_aarrcp_))
           {  for(i=0; i < ToRank; i++)
                  tprintf("ToInitIndexArray[%d]=%ld; ",
                          i,ToInitIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < ToRank; i++)
                  tprintf("ToLastIndexArray[%d]=%ld; ",
                          i,ToLastIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < ToRank; i++)
                  tprintf("     ToStepArray[%d]=%ld; ",
                          i,ToStepArray[i]);
              tprintf(" \n"); 
              tprintf(" \n");

              /* local part of written array 
                 is output in trace*/    /*E0011*/

              if(dacopy_Trace)
              {  if(ToDArr->HasLocal)
                 {  for(i=0; i < ToRank; i++)
                        tprintf("ToLocal[%d]: Lower=%ld Upper=%ld\n",
                                i, ToDArr->Block.Set[i].Lower,
                                   ToDArr->Block.Set[i].Upper);
                 }
                 else
                    tprintf("No ToLocal\n");

                 tprintf(" \n");
                 tprintf("ToRepl=%d ToPartRepl=%d ToEvery=%d\n\n",
                         (int)ToDArr->Repl, (int)ToDArr->PartRepl,
                         (int)ToDArr->Every);
              }
           }
        }

        if(ToAMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 096.005: wrong call aarrcp_\n"
                 "(ToArray has not been aligned; "
                 "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        Res = GetIndexArray(ToArrayHandlePtr, ToInitIndexArray,
                            ToLastIndexArray, ToStepArray,
                            OutToInitIndex, OutToLastIndex, OutToStep,
                            0);
        if(Res == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 096.003: wrong call aarrcp_\n"
                    "(invalid to index or to step; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        if(EnableDynControl)
           dyn_DisArrSetVal(ToArrayHandlePtr, OutToInitIndex,
                            OutToLastIndex, OutToStep);

        NotSubsystem(i, DVM_VMS, ToAMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 096.007: wrong call aarrcp_\n"
             "(the ToArray PS is not a subsystem of "
             "the current PS;\n"
             "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
             "CurrentPSRef=%lx)\n",
             ToArrayHeader[0], (uLLng)ToAMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

        for(j=0; j < ToRank; j++)
            if(OutToStep[j] != 1)
               break;

        if(j == ToRank)
        {  /* step on all dimensions of 
                 written array - 1 */    /*E0012*/

           WriteBlock = block_InitFromArr((byte)ToRank,
                                          OutToInitIndex,
                                          OutToLastIndex,
                                          OutToStep);
           if(*CopyRegimPtr)
           {  if(*CopyRegimPtr < 0)
                 Res = FillBlock((char *)FromArrayHeader,ToDArr,
                                 &WriteBlock);  /* fill in */    /*E0013*/
              else
              {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                    Res = AIOPutBlockRepl((char *)FromArrayHeader,
                                          ToDArr, &WriteBlock,
                                          CopyFlagPtr);
                 else
                    Res = AIOPutBlock((char *)FromArrayHeader,
                                      ToDArr, &WriteBlock,
                                      CopyFlagPtr);
              }
           }
           else
           {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                 Res = PutBlockRepl((char *)FromArrayHeader,
                                    ToDArr, &WriteBlock);
              else
                 Res = PutBlock((char *)FromArrayHeader, ToDArr,
                                &WriteBlock);
           }
        }
        else
        {  /* steps on dimensions of written array 
                 are not always equal to 1*/    /*E0014*/

           WriteBlock = block_InitFromArr((byte)ToRank,
                                          OutToInitIndex,
                                          OutToLastIndex,
                                          OutToStep);
           if(*CopyRegimPtr)
           {  if(*CopyRegimPtr < 0)
                 Res = FillBlock((char *)FromArrayHeader,ToDArr,
                                 &WriteBlock);  /* fill in  */    /*E0015*/
              else
              {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                    Res = AIOPutBlockRepl((char *)FromArrayHeader,
                                          ToDArr, &WriteBlock,
                                          CopyFlagPtr);
                 else
                    Res = AIOPutBlock((char *)FromArrayHeader,
                                      ToDArr, &WriteBlock,
                                      CopyFlagPtr);
              }
           }
           else
           {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                 Res = PutBlockRepl((char *)FromArrayHeader,
                                    ToDArr, &WriteBlock);
              else
                 Res = PutBlock((char *)FromArrayHeader, ToDArr,
                                &WriteBlock);
           }
        }
     }
     else
     {  /* copy distributed array-> distributed array */    /*E0016*/

        FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;
        FromRank = FromDArr->Space.Rank;
        FromAMV = FromDArr->AMView;
        ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;
        ToRank = ToDArr->Space.Rank;
        ToAMV = ToDArr->AMView;

        if(RTL_TRACE)
        {  dvm_trace(call_aarrcp_,
                     "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                     "ToArrayHeader=%lx; ToArrayHandlePtr=%lx; "
                     "CopyRegim=%ld; CopyFlagPtr=%lx;\n",
                     (uLLng)FromArrayHeader, FromArrayHeader[0],
                     (uLLng)ToArrayHeader, ToArrayHeader[0],
                     *CopyRegimPtr, (uLLng)CopyFlagPtr);

           if(TstTraceEvent(call_aarrcp_))
           {  for(i=0; i < FromRank; i++)
                  tprintf("FromInitIndexArray[%d]=%ld; ",
                          i, FromInitIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < FromRank; i++)
                  tprintf("FromLastIndexArray[%d]=%ld; ",
                          i, FromLastIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < FromRank; i++)
                  tprintf("     FromStepArray[%d]=%ld; ",
                          i, FromStepArray[i]);
              tprintf(" \n");

              for(i=0; i < ToRank; i++)
                  tprintf("  ToInitIndexArray[%d]=%ld; ",
                          i, ToInitIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < ToRank; i++)
                  tprintf("  ToLastIndexArray[%d]=%ld; ",
                          i, ToLastIndexArray[i]);
              tprintf(" \n"); 
              for(i=0; i < ToRank; i++)
                  tprintf("       ToStepArray[%d]=%ld; ",
                          i, ToStepArray[i]);
              tprintf(" \n"); 
              tprintf(" \n");

              /* local parts of copied arrays 
                 is output in trace*/    /*E0017*/

              if(dacopy_Trace)
              {  if(FromDArr->HasLocal)
                 {  for(i=0; i < FromRank; i++)
                        tprintf("FromLocal[%d]: Lower=%ld Upper=%ld\n",
                                i, FromDArr->Block.Set[i].Lower,
                                   FromDArr->Block.Set[i].Upper);
                 }
                 else
                    tprintf("No FromLocal\n");

                 tprintf(" \n");

                 tprintf("FromRepl=%d FromPartRepl=%d FromEvery=%d\n\n",
                         (int)FromDArr->Repl, (int)FromDArr->PartRepl,
                         (int)FromDArr->Every);

                 if(ToDArr->HasLocal)
                 {  for(i=0; i < ToRank; i++)
                        tprintf("  ToLocal[%d]: Lower=%ld Upper=%ld\n",
                                i, ToDArr->Block.Set[i].Lower,
                                   ToDArr->Block.Set[i].Upper);
                 }
                 else
                    tprintf("No ToLocal\n");

                 tprintf(" \n");

                 tprintf("  ToRepl=%d   ToPartRepl=%d   ToEvery=%d\n\n",
                         (int)ToDArr->Repl, (int)ToDArr->PartRepl,
                         (int)ToDArr->Every);
              }
           }
        }

        if(ToAMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 096.005: wrong call aarrcp_\n"
                 "(ToArray has not been aligned; "
                 "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        if(FromAMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 096.004: wrong call aarrcp_\n"
                 "(FromArray has not been aligned; "
                 "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        FromBSize = GetIndexArray(FromArrayHandlePtr, FromInitIndexArray,
                                  FromLastIndexArray, FromStepArray,
                                  OutFromInitIndex, OutFromLastIndex,
                                  OutFromStep, 0);
        if(FromBSize == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 096.001: wrong call aarrcp_\n"
                    "(invalid from index or from step; "
                    "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        ToBSize = GetIndexArray(ToArrayHandlePtr, ToInitIndexArray,
                                ToLastIndexArray, ToStepArray,
                                OutToInitIndex, OutToLastIndex,
                                OutToStep, 0);

        if(ToBSize == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 096.003: wrong call aarrcp_\n"
                    "(invalid to index or to step; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        Res = dvm_min(FromBSize, ToBSize);

        if(EnableDynControl)
        {  dyn_DisArrTestVal(FromArrayHandlePtr, OutFromInitIndex,
                             OutFromLastIndex, OutFromStep);
           dyn_DisArrSetVal(ToArrayHandlePtr, OutToInitIndex,
                            OutToLastIndex, OutToStep);
        }

        NotSubsystem(i, DVM_VMS, ToAMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 096.007: wrong call aarrcp_\n"
             "(the ToArray PS is not a subsystem of "
             "the current PS;\n"
             "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
             "CurrentPSRef=%lx)\n",
             ToArrayHeader[0], (uLLng)ToAMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

        NotSubsystem(i, DVM_VMS, FromAMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 096.006: wrong call aarrcp_\n"
             "(the FromArray PS is not a subsystem of "
             "the current PS;\n"
             "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; "
             "CurrentPSRef=%lx)\n",
             FromArrayHeader[0], (uLLng)FromAMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

        if(FromDArr->HasLocal || ToDArr->HasLocal)
        {  ReadBlock = block_InitFromArr((byte)FromRank,
                                         OutFromInitIndex,
                                         OutFromLastIndex,
                                         OutFromStep);

           WriteBlock = block_InitFromArr((byte)ToRank,
                                          OutToInitIndex,
                                          OutToLastIndex,
                                          OutToStep);

           if(FromBSize > ToBSize)
           {  /* Correct read block */    /*E0018*/

              block_GetWeight(&ReadBlock, Weight, 1);
              index_GetSI(&ReadBlock, Weight, ToBSize-1,
                          OutFromLastIndex, Step);
              ReadBlock = block_InitFromArr((byte)FromRank,
                                            OutFromInitIndex,
                                            OutFromLastIndex,
                                            OutFromStep);
           }

           if(ToBSize > FromBSize)
           {  /* correct written block */    /*E0019*/

              block_GetWeight(&WriteBlock, Weight, 1);
              index_GetSI(&WriteBlock, Weight, FromBSize-1,
                          OutToLastIndex, Step);
              WriteBlock = block_InitFromArr((byte)ToRank,
                                             OutToInitIndex,
                                             OutToLastIndex,
                                             OutToStep);
           }

           NotSubsystem(n, FromAMV->VMS, ToAMV->VMS)

           for(i=0; i < FromRank; i++)
               if(OutFromStep[i] != 1)
                  break;

           for(j=0; j < ToRank; j++)
               if(OutToStep[j] != 1)
                  break;

           if(i == FromRank && j == ToRank)
           {  /* steps on all dimensions of the both arrays - 1*/    /*E0020*/
           
              if(FromDArr->Repl)
              {  CopyBlockRepl1(FromDArr, &ReadBlock, ToDArr,
                                &WriteBlock);
                 if(n)
                    ACopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                &WriteBlock, CopyFlagPtr);
                 else
                 {  if(A_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       ACopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                   &WriteBlock, CopyFlagPtr);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_aarrcp_))
                    tprintf("*** aarrcp_: CopyBlockRepl1 branch "
                            "(n=%d)\n", n);
              }
              else
              {  RC = AttemptLocCopy1(FromDArr, &ReadBlock, ToDArr,
                                      &WriteBlock);
                 if(n || RC)
                    ACopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                &WriteBlock, CopyFlagPtr);
                 else
                 {  if(A_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       ACopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                   &WriteBlock, CopyFlagPtr);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_aarrcp_))
                    tprintf("*** aarrcp_: AttemptLocCopy1 branch "
                            "(n=%d  RC=%d)\n", n, RC);
              }
           }
           else
           {  /* steps on dimensions of arrays are not always equal to 1*/    /*E0021*/

              if(FromDArr->Repl)
              {  CopyBlockRepl(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock);
                 if(n)
                    ACopyBlock(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock, CopyFlagPtr);

                 else
                 {  if(A_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       ACopyBlock(FromDArr, &ReadBlock, ToDArr,
                                  &WriteBlock, CopyFlagPtr);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_aarrcp_))
                    tprintf("*** aarrcp_: CopyBlockRepl branch "
                            "(n=%d)\n", n);
              }
              else
              {  RC = AttemptLocCopy(FromDArr, &ReadBlock, ToDArr,
                                     &WriteBlock);
                 if(n || RC)
                    ACopyBlock(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock, CopyFlagPtr);

                 else
                 {  if(A_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       ACopyBlock(FromDArr, &ReadBlock, ToDArr,
                                  &WriteBlock, CopyFlagPtr);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_aarrcp_))
                    tprintf("*** aarrcp_: AttemptLocCopy branch "
                            "(n=%d  RC=%d)\n", n, RC);
              }
           }
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_aarrcp_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_aarrcp_);
  return  (DVM_RET, Res);
}


/* ----------------------------------------------------- */    /*E0022*/


DvmType  AGetBlock(char *BufferPtr, s_DISARRAY *DArr,
                s_BLOCK *ReadBlockPtr, AddrType *CopyFlagPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, DVM_VMSize, i, j, k, Proc;
  RTL_Request   *RecvReq = NULL, *SendReq = NULL;
  int           *RecvFlag = NULL, *SendFlag = NULL;
  s_BLOCK       *RecvBlock = NULL, *wLocalBlock = NULL;
  s_BLOCK        ReadLocalBlock, wBlock;
  int            SendBSize, RecvBSize;
  void          *SendBuf = NULL, **RecvBuf = NULL;
  byte           Step; 
  s_COPYCONT    *CopyCont;
  s_AGetBlock   *GetBlock;
  DvmType           tlong;

  Step = GetStepSign(ReadBlockPtr);

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;
  DVM_VMSize = DVM_VMS->ProcCount;

  dvm_AllocArray(RTL_Request, DVM_VMSize, RecvReq);
  dvm_AllocArray(int, DVM_VMSize, RecvFlag);
  dvm_AllocArray(void *, DVM_VMSize, RecvBuf);
  dvm_AllocArray(s_BLOCK, DVM_VMSize, RecvBlock);
  dvm_AllocArray(RTL_Request, DVM_VMSize, SendReq);
  dvm_AllocArray(int, DVM_VMSize, SendFlag);

  /* Structures to continue */    /*E0023*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_AGetBlock, GetBlock);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)GetBlock;

  /* ------------------------- */    /*E0024*/

  for(i=0; i < DVM_VMSize; i++)
  {  RecvFlag[i] = 0;
     SendFlag[i] = 0;
  }

  if(DArr->HasLocal && block_Intersect(&ReadLocalBlock, ReadBlockPtr,
                                       &DArr->Block, ReadBlockPtr,
                                       Step))
  {  /* Local part of read block is not empty */    /*E0025*/

     block_GetSize(tlong, &ReadLocalBlock, Step)
     SendBSize = (int)(tlong * DArr->TLen);
     mac_malloc(SendBuf, void *, SendBSize, 0);
     CopyBlockToMem((char *)SendBuf, &ReadLocalBlock, DArr, Step);
     CopySubmemToMem(BufferPtr, ReadBlockPtr, SendBuf, &ReadLocalBlock,
                     DArr->TLen, Step);

     /* Pass local part of read block to other processors */    /*E0026*/

     for(i=0; i < DVM_VMSize; i++)
     {  Proc = (int)DVM_VMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;

        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0027*/

        if( (j = IsProcInVMS(Proc, AMS->VMS)) >= 0 )
        {
           if(DArr->IsVMSBlock[j] == 0)
           {  wLocalBlock = GetSpaceLB4Proc(j, AMS, &DArr->Space,
                                            DArr->Align, &ReadLocalBlock,
                                            &DArr->VMSLocalBlock[j]);
              DArr->VMSBlock[j] = wLocalBlock;
              DArr->IsVMSBlock[j] = 1;
           }
           else
           {  wLocalBlock = DArr->VMSBlock[j];

              if(wLocalBlock != NULL)
              {  for(k=0; k < ReadLocalBlock.Rank; k++)
                     wLocalBlock->Set[k].Step =
                     ReadLocalBlock.Set[k].Step;
              }
           }
        }
        else
           wLocalBlock = NULL;

        if(wLocalBlock == NULL ||
           block_Intersect(&wBlock, &ReadLocalBlock, wLocalBlock,
                           &ReadLocalBlock, Step) == 0)
        {  ( RTL_CALL, rtl_Sendnowait(SendBuf, 1, SendBSize, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_DA_Mem) );
           SendFlag[i] = 1;
        }
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffDACopy);
     }
  }

  /* Receiving missing parts of read block from other processors*/    /*E0028*/

  for(i=0; i < VMSize; i++)
  {  Proc = (int)VMS->VProc[i].lP;

     if(Proc == MPS_CurrentProc)
        continue;

     if(DArr->IsVMSBlock[i] == 0)
     {  wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                      DArr->Align, ReadBlockPtr,
                                      &DArr->VMSLocalBlock[i]);
        DArr->VMSBlock[i] = wLocalBlock;
        DArr->IsVMSBlock[i] = 1;
     }
     else
     {  wLocalBlock = DArr->VMSBlock[i];

        if(wLocalBlock != NULL)
        {  for(k=0; k < ReadBlockPtr->Rank; k++)
               wLocalBlock->Set[k].Step = ReadBlockPtr->Set[k].Step;
        }
     }

     if(wLocalBlock && block_Intersect(&RecvBlock[i], ReadBlockPtr,
                                       wLocalBlock, ReadBlockPtr, Step))
     {  if(DArr->HasLocal == 0 ||
           block_Intersect(&wBlock, &RecvBlock[i], &DArr->Block,
                           &RecvBlock[i], Step) == 0)
        {  block_GetSize(tlong, &RecvBlock[i], Step)
           RecvBSize = (int)(tlong * DArr->TLen);
	   mac_malloc(RecvBuf[i], void *, RecvBSize, 0);
	   RecvFlag[i] = 1;
	   ( RTL_CALL, rtl_Recvnowait(RecvBuf[i], 1, RecvBSize, Proc,
				      DVM_VMS->tag_DACopy,
                                      &RecvReq[i], 1) );
	}
     }
  }

  CopyCont->Oper = sendrecv_AGetBlock; /* code of continue operation */    /*E0029*/

  /* Save information for asynchronous continuation */    /*E0030*/

  GetBlock->VMSize    = VMSize;
  GetBlock->SendFlag  = SendFlag;
  GetBlock->RecvFlag  = RecvFlag;
  GetBlock->SendReq   = SendReq;
  GetBlock->RecvReq   = RecvReq;
  GetBlock->SendBuf   = SendBuf;
  GetBlock->RecvBuf   = RecvBuf;
  GetBlock->RecvBlock = RecvBlock;
  GetBlock->DArr      = DArr;
  GetBlock->Step      = Step;
  GetBlock->BufferPtr = BufferPtr;

  GetBlock->ReadBlock = block_Copy(ReadBlockPtr);

  block_GetSize(tlong, ReadBlockPtr, Step)
  return   tlong;
}



DvmType  AIOGetBlock(char *BufferPtr, s_DISARRAY *DArr,
                  s_BLOCK *ReadBlockPtr, AddrType *CopyFlagPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, k, Proc;
  RTL_Request   *RecvReq = NULL, *SendReq = NULL;
  int           *RecvFlag = NULL, *SendFlag = NULL;
  s_BLOCK        ReadLocalBlock, wBlock;
  s_BLOCK       *RecvBlock = NULL, *wLocalBlock = NULL;
  DvmType           ReadBSize, SendBSize, RecvBSize;
  void          *ReadBuf = NULL, *SendBuf = NULL, **RecvBuf = NULL;
  byte           Step;
  s_COPYCONT    *CopyCont;
  s_AGetBlock   *GetBlock;

  Step = GetStepSign(ReadBlockPtr);

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, RecvReq);
  dvm_AllocArray(RTL_Request, 1, SendReq);
  dvm_AllocArray(int, VMSize, RecvFlag);
  dvm_AllocArray(int, 1, SendFlag);
  dvm_AllocArray(void *, VMSize, RecvBuf);
  dvm_AllocArray(s_BLOCK, VMSize, RecvBlock);


  /* Stuctures for continuation */    /*E0031*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_AGetBlock, GetBlock);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)GetBlock;

  /* ------------------------- */    /*E0032*/

  for(i=0; i < VMSize; i++)
      RecvFlag[i]=0;

  SendFlag[0] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0033*/

     if(DArr->HasLocal && block_Intersect(&ReadLocalBlock,ReadBlockPtr,
                                        &DArr->Block,ReadBlockPtr,Step))
     {  /* Local part of read processor is not empty */    /*E0034*/

        block_GetSize(ReadBSize, &ReadLocalBlock, Step)
        ReadBSize *= DArr->TLen;
        mac_malloc(ReadBuf, void *, ReadBSize, 0);
        CopyBlockToMem((char *)ReadBuf, &ReadLocalBlock, DArr, Step);
        CopySubmemToMem(BufferPtr, ReadBlockPtr, ReadBuf,
                        &ReadLocalBlock, DArr->TLen, Step);
        mac_free(&ReadBuf);
     }

     for(i=0; i < VMSize; i++)
     {  Proc = (int)VMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;

        if(DArr->IsVMSBlock[i] == 0)
        {  wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                         DArr->Align, ReadBlockPtr,
                                         &DArr->VMSLocalBlock[i]);
           DArr->VMSBlock[i] = wLocalBlock;
           DArr->IsVMSBlock[i] = 1;
        }
        else
        {  wLocalBlock = DArr->VMSBlock[i];

           if(wLocalBlock != NULL)
           {  for(k=0; k < ReadBlockPtr->Rank; k++)
                  wLocalBlock->Set[k].Step = ReadBlockPtr->Set[k].Step;
           }
        }

        if(wLocalBlock && block_Intersect(&RecvBlock[i], ReadBlockPtr,
                                          wLocalBlock, ReadBlockPtr,
                                          Step))
        {  if(DArr->HasLocal == 0 ||
              block_Intersect(&wBlock, &RecvBlock[i], &DArr->Block,
                              &RecvBlock[i], Step) == 0)
           {  block_GetSize(RecvBSize, &RecvBlock[i], Step)
              RecvBSize *= DArr->TLen;
              mac_malloc(RecvBuf[i], void *, RecvBSize, 0);
              RecvFlag[i] = 1;
              ( RTL_CALL, rtl_Recvnowait(RecvBuf[i], 1, (int)RecvBSize,
                                         Proc, DVM_VMS->tag_DACopy,
                                         &RecvReq[i], 1) );
           }
        }
     }

     CopyCont->Oper = recv_AIOGetBlock; /* code of continue operation */    /*E0035*/
  } 
  else
  {  /* Current processor is not I/O processor */    /*E0036*/

     if(DArr->HasLocal && block_Intersect(&ReadLocalBlock, ReadBlockPtr,
                                          &DArr->Block, ReadBlockPtr,
                                          Step))
     {  /* Local part of read block is not empty */    /*E0037*/

        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0038*/

        if( (i = IsProcInVMS(DVM_IOProc, AMS->VMS)) >= 0 )
        {
           if(DArr->IsVMSBlock[i] == 0)
           {  wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                            DArr->Align, &ReadLocalBlock,
                                            &DArr->VMSLocalBlock[i]);
              DArr->VMSBlock[i] = wLocalBlock;
              DArr->IsVMSBlock[i] = 1;
           }
           else
           {  wLocalBlock = DArr->VMSBlock[i];

              if(wLocalBlock != NULL)
              {  for(k=0; k < ReadLocalBlock.Rank; k++)
                     wLocalBlock->Set[k].Step =
                     ReadLocalBlock.Set[k].Step;
              }
           }
        }
        else
           wLocalBlock = NULL;

        if(wLocalBlock == NULL ||
           block_Intersect(&wBlock, &ReadLocalBlock,
                           wLocalBlock, &ReadLocalBlock, Step) == 0)
        {  block_GetSize(SendBSize, &ReadLocalBlock, Step)
           SendBSize *= DArr->TLen;
           mac_malloc(SendBuf, void *, SendBSize, 0);
           CopyBlockToMem((char *)SendBuf, &ReadLocalBlock, DArr, Step);
           SendFlag[0] = 1;
           ( RTL_CALL, rtl_Sendnowait(SendBuf, 1, (int)SendBSize,
	                              DVM_IOProc, DVM_VMS->tag_DACopy,
                                      SendReq, a_DA_IOMem) );
        }
     }

     CopyCont->Oper = send_AIOGetBlock; /* code of continue operation */    /*E0039*/
  }

  /* Save information for asynchronouse continuation */    /*E0040*/

  GetBlock->VMSize    = VMSize;
  GetBlock->SendFlag  = SendFlag;
  GetBlock->RecvFlag  = RecvFlag;
  GetBlock->SendReq   = SendReq;
  GetBlock->RecvReq   = RecvReq;
  GetBlock->SendBuf   = SendBuf;
  GetBlock->RecvBuf   = RecvBuf;
  GetBlock->RecvBlock = RecvBlock;
  GetBlock->DArr      = DArr;
  GetBlock->Step      = Step;
  GetBlock->BufferPtr = BufferPtr;

  GetBlock->ReadBlock = block_Copy(ReadBlockPtr);

  block_GetSize(RecvBSize, ReadBlockPtr, Step)
  return  RecvBSize;
}



DvmType  AIOPutBlock(char *BufferPtr, s_DISARRAY *DArr,
                  s_BLOCK *WriteBlockPtr, AddrType *CopyFlagPtr)
{ s_AMVIEW        *AMS;
  s_VMS           *VMS;
  int              VMSize, i, k, Proc;
  RTL_Request     *SendReq = NULL, *RecvReq = NULL;
  int             *SendFlag = NULL, *RecvFlag = NULL;
  void           **SendBuf = NULL, *WriteBuf = NULL, *RecvBuf = NULL;
  s_BLOCK         *SendBlock = NULL, *wLocalBlock = NULL;
  s_BLOCK          WriteLocalBlock;
  DvmType             WriteBSize, SendBSize, RecvBSize;
  byte             Step;
  s_COPYCONT      *CopyCont;
  s_AIOPutBlock   *PutBlock;

  Step = GetStepSign(WriteBlockPtr);

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(RTL_Request, 1, RecvReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  dvm_AllocArray(int, 1, RecvFlag);
  dvm_AllocArray(void *, VMSize, SendBuf);
  dvm_AllocArray(s_BLOCK, VMSize, SendBlock);

  /* Structures for continuation */    /*E0041*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_AIOPutBlock, PutBlock);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)PutBlock;

  /* ------------------------- */    /*E0042*/

  for(i=0; i < VMSize; i++)
      SendFlag[i] = 0;

  RecvFlag[0] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0043*/

     for(i=0; i < VMSize; i++)
     {  Proc = (int)VMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;

        if(DArr->IsVMSBlock[i] == 0)
        {  wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                         DArr->Align, WriteBlockPtr,
                                         &DArr->VMSLocalBlock[i]);
           DArr->VMSBlock[i] = wLocalBlock;
           DArr->IsVMSBlock[i] = 1;
        }
        else
        {  wLocalBlock = DArr->VMSBlock[i];

           if(wLocalBlock != NULL)
           {  for(k=0; k < WriteBlockPtr->Rank; k++)
                  wLocalBlock->Set[k].Step = WriteBlockPtr->Set[k].Step;
           }
        }

        if(wLocalBlock && block_Intersect(&SendBlock[i], WriteBlockPtr,
                                          wLocalBlock, WriteBlockPtr,
                                          Step))
        {  block_GetSize(SendBSize, &SendBlock[i], Step)
           SendBSize *= DArr->TLen;
           mac_malloc(SendBuf[i], void *, SendBSize, 0);
           CopyMemToSubmem(SendBuf[i], &SendBlock[i], BufferPtr,
			   WriteBlockPtr, DArr->TLen, Step);
           SendFlag[i] = 1;
           ( RTL_CALL, rtl_Sendnowait(SendBuf[i], 1, (int)SendBSize,
                                      Proc, DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_IOMem_DA) );
        }
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffDACopy);
     }

     if(DArr->HasLocal && block_Intersect(&WriteLocalBlock,
                                          WriteBlockPtr, &DArr->Block,
                                          WriteBlockPtr, Step))
     { /* Local part of written block is not empty */    /*E0044*/

       block_GetSize(WriteBSize, &WriteLocalBlock, Step)
       WriteBSize *= DArr->TLen;
       mac_malloc(WriteBuf, void *, WriteBSize, 0);
       CopyMemToSubmem(WriteBuf, &WriteLocalBlock, BufferPtr,
                       WriteBlockPtr, DArr->TLen,Step);
       CopyMemToBlock(DArr, (char *)WriteBuf, &WriteLocalBlock, Step);
       mac_free(&WriteBuf);
     }

     CopyCont->Oper = send_AIOPutBlock; /* code of continue operation */    /*E0045*/
  }
  else
  {  /* Current processor is not I/O processor */    /*E0046*/

     if(DArr->HasLocal && block_Intersect(&WriteLocalBlock,
                                          WriteBlockPtr,
                                          &DArr->Block,
                                          WriteBlockPtr, Step))
     {  /* Local part of written block is not empty */    /*E0047*/

        block_GetSize(RecvBSize, &WriteLocalBlock, Step)
        RecvBSize *= DArr->TLen;
        mac_malloc(RecvBuf, void *, RecvBSize, 0);
        RecvFlag[0] = 1; 
        ( RTL_CALL, rtl_Recvnowait(RecvBuf, 1, (int)RecvBSize,
                                   DVM_IOProc, DVM_VMS->tag_DACopy,
                                   RecvReq, 1) );
     }

     CopyCont->Oper = recv_AIOPutBlock; /* code of continue operation */    /*E0048*/
  }

  /* Save information for asynchronous continuation */    /*E0049*/

  PutBlock->VMSize    = VMSize;
  PutBlock->SendFlag  = SendFlag;
  PutBlock->RecvFlag  = RecvFlag;
  PutBlock->SendReq   = SendReq;
  PutBlock->RecvReq   = RecvReq;
  PutBlock->SendBuf   = SendBuf;
  PutBlock->RecvBuf   = RecvBuf;
  PutBlock->DArr      = DArr;
  PutBlock->Step      = Step;

  PutBlock->WriteLocalBlock = block_Copy(&WriteLocalBlock);

  dvm_FreeArray(SendBlock);

  block_GetSize(RecvBSize, WriteBlockPtr, Step)
  return  RecvBSize;
}



DvmType  AIOPutBlockRepl(char *BufferPtr, s_DISARRAY *DArr,
                      s_BLOCK *WriteBlockPtr, AddrType *CopyFlagPtr)
{ s_AMVIEW           *AMS;
  s_VMS              *VMS;
  int                 VMSize, i, Proc, BSize;
  DvmType                Res;
  RTL_Request        *SendReq = NULL, *RecvReq = NULL;
  byte                Step, RecvSign;
  char               *_BufferPtr = NULL;
  s_COPYCONT         *CopyCont;
  s_AIOPutBlockRepl  *PutBlockRepl;

  Step   = GetStepSign(WriteBlockPtr);
  
  AMS    = DArr->AMView;
  VMS    = AMS->VMS;
  VMSize = VMS->ProcCount;
  block_GetSize(Res, WriteBlockPtr, Step)
  BSize  = (int)( Res * (DArr->TLen) );

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(RTL_Request, 1, RecvReq);

  /* Structures for continuation */    /*E0050*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_AIOPutBlockRepl, PutBlockRepl);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)PutBlockRepl;

  /* ------------------------- */    /*E0051*/

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0052*/

     if((IsSynchr && UserSumFlag) || (BSize & Msk3))
     {  mac_malloc(_BufferPtr, char *, BSize, 0);
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, BSize))

        for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;
           if(Proc == MPS_CurrentProc)
              continue;
           ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, (int)BSize, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_IOMem_DARepl) );
        }
     }
     else
     {  for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;
           if(Proc == MPS_CurrentProc)
              continue;
           ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, (int)BSize, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_IOMem_DARepl) );
        }
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffDACopy);
     }

     CopyMemToBlock(DArr, BufferPtr, WriteBlockPtr, Step);

     CopyCont->Oper = send_AIOPutBlockRepl; /* code of continue operation */    /*E0053*/
  }
  else
  {  /* Current processor is not I/O processor */    /*E0054*/

     if((IsSynchr && UserSumFlag) || (BSize & Msk3))
     {  RecvSign = 1;
        mac_malloc(_BufferPtr, char *, BSize, 0);
        ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, BSize, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, RecvReq, 1) );
     }
     else
     {  RecvSign = 0;
        ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, BSize, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, RecvReq, 1) );
     }

     CopyCont->Oper = recv_AIOPutBlockRepl; /* code of continue operation */    /*E0055*/
  }

  /* Save information for asynchronous continuation */    /*E0056*/

  PutBlockRepl->VMSize     = VMSize;
  PutBlockRepl->BSize      = BSize;
  PutBlockRepl->VMS        = VMS;
  PutBlockRepl->SendReq    = SendReq;
  PutBlockRepl->RecvReq    = RecvReq;
  PutBlockRepl->Step       = Step;
  PutBlockRepl->RecvSign   = RecvSign;
  PutBlockRepl->BufferPtr  = BufferPtr;
  PutBlockRepl->_BufferPtr = _BufferPtr;
  PutBlockRepl->DArr       = DArr;

  PutBlockRepl->WriteBlock = block_Copy(WriteBlockPtr);

  return Res;
}



void  ACopyBlock(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                 s_DISARRAY  *ToDArr,  s_BLOCK  *ToBlockPtr,
                 AddrType *CopyFlagPtr)
{ s_AMVIEW       *ReadAMS, *WriteAMS;
  s_VMS          *ReadVMS, *WriteVMS;
  DvmType            ReadVMSize, WriteVMSize;
  RTL_Request    *ReadReq = NULL, *WriteReq = NULL;
  void          **ReadBuf = NULL, **WriteBuf = NULL;
  int            *IsReadInter = NULL, *IsWriteInter = NULL,
                 *WriteVMInReadVM = NULL;
  s_BLOCK        *ReadLocalBlock = NULL, *WriteLocalBlock = NULL,
                 *ToLocalBlock = NULL;
  int            *ReadBSize = NULL, *WriteBSize = NULL;
  DvmType            Proc, LinInd, MyMinLinInd, MyMaxLinInd,
                  MinLinInd, MaxLinInd;
  int             i, j, k, n, m, p, q, ToRank, FromRank,
                  FromOnlyAxis, ToOnlyAxis;
  DvmType            ReadWeight[MAXARRAYDIM], WriteWeight[MAXARRAYDIM];
  s_BLOCK        *CurrReadBlock, *CurrWriteBlock;
  s_BLOCK         wReadBlock, wWriteBlock, FromLocalBlock;
  DvmType            ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  char          **WriteElmPtr = NULL, *ReadElmPtr = NULL,
                 *CharPtr1, *CharPtr2;
  byte            FromStep, ToStep, EquSign = 0, ExchangeScheme,
                  Alltoall = 0;
  s_COPYCONT     *CopyCont;
  s_ACopyBlock   *CopyBlock;
  byte           *IsVMSBlock;
  s_BLOCK       **VMSBlock;
  SysHandle      *VProc;
  int             Save_a_DA = a_DA_NE_DA;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *sdispls, *rdispls;
  int             r;
#endif

  /* Structures for continuation */    /*E0057*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ACopyBlock, CopyBlock);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)CopyBlock;

  /* ------------------------- */    /*E0058*/

  FromStep = GetStepSign(FromBlockPtr);
  ToStep   = GetStepSign(ToBlockPtr);

  ToRank   = ToBlockPtr->Rank;
  FromRank = FromBlockPtr->Rank;

  ReadAMS    = FromDArr->AMView;
  ReadVMS    = ReadAMS->VMS;
  ReadVMSize = ReadVMS->ProcCount;

  WriteAMS    = ToDArr->AMView;
  WriteVMS    = WriteAMS->VMS;
  WriteVMSize = WriteVMS->ProcCount;

  /* */    /*E0059*/

#ifdef  _DVM_MPI_

  if(A_MPIAlltoall && ReadVMS == DVM_VMS && WriteVMS == DVM_VMS &&
     (FromDArr->Every && ToDArr->Every) && DVM_VMS->Is_MPI_COMM != 0
     /*&& NoWaitCount == 0*/    /*E0060*/)
     Alltoall = 1;

#endif

  /* */    /*E0061*/

  ExchangeScheme = (byte)(MsgExchangeScheme == 2 &&
                          ReadVMS == WriteVMS && ReadVMS->HasCurrent &&
                          Alltoall == 0);

  /* ---------------------------------------- */    /*E0062*/

  dvm_AllocArray(RTL_Request, ReadVMSize, ReadReq);
  dvm_AllocArray(void *, ReadVMSize, ReadBuf);
  dvm_AllocArray(int, ReadVMSize, IsReadInter);
  dvm_AllocArray(s_BLOCK, ReadVMSize, ReadLocalBlock);
  dvm_AllocArray(int, ReadVMSize, ReadBSize);

  /* Create array of blocks of From array local parts 
     for processor system the From array is mapped on */    /*E0063*/

  VMSBlock = FromDArr->VMSBlock;
  IsVMSBlock = FromDArr->IsVMSBlock; 

  for(i=0; i < ReadVMSize; i++)
  {  if(IsVMSBlock[i] == 0)
     {  VMSBlock[i] = GetSpaceLB4Proc(i, ReadAMS, &FromDArr->Space,
                                      FromDArr->Align, FromBlockPtr,
                                      &FromDArr->VMSLocalBlock[i]);
        IsVMSBlock[i] = 1;
     }
     else
     {  if(VMSBlock[i] != NULL)
           for(j=0; j < FromRank; j++)
               VMSBlock[i]->Set[j].Step = FromBlockPtr->Set[j].Step;
     }
  }

  /* ------------------------------------------------- */    /*E0064*/

  for(i=0; i < ReadVMSize; i++)
  {  ReadBSize[i] = 0;

     if(VMSBlock[i])
        IsReadInter[i] = block_Intersect(&ReadLocalBlock[i],
                                         FromBlockPtr, VMSBlock[i],
                                         FromBlockPtr, FromStep);
     else
        IsReadInter[i] = 0;
  }

  dvm_AllocArray(RTL_Request, WriteVMSize, WriteReq);
  dvm_AllocArray(void *, WriteVMSize, WriteBuf);
  dvm_AllocArray(int, WriteVMSize, IsWriteInter);
  dvm_AllocArray(s_BLOCK, WriteVMSize, WriteLocalBlock);
  dvm_AllocArray(int, WriteVMSize, WriteBSize);

  /* Create array of blocks of To array local parts 
     for processor system the To array is mapped on */    /*E0065*/

  VMSBlock   = ToDArr->VMSBlock;
  IsVMSBlock = ToDArr->IsVMSBlock; 

  for(i=0; i < WriteVMSize; i++)
  {  if(IsVMSBlock[i] == 0)
     {  VMSBlock[i] = GetSpaceLB4Proc(i, WriteAMS, &ToDArr->Space,
                                      ToDArr->Align, ToBlockPtr,
                                      &ToDArr->VMSLocalBlock[i]);
        IsVMSBlock[i] = 1;
     }
     else
     {  if(VMSBlock[i] != NULL)
           for(j=0; j < ToRank; j++)
               VMSBlock[i]->Set[j].Step = ToBlockPtr->Set[j].Step;
     }
  }

  /* ------------------------------------------------ */    /*E0066*/

  for(i=0; i < WriteVMSize; i++)
  {  WriteBSize[i] = 0;

     if(VMSBlock[i])
       IsWriteInter[i] = block_Intersect(&WriteLocalBlock[i],
                                         ToBlockPtr, VMSBlock[i],
                                         ToBlockPtr, ToStep);
     else
       IsWriteInter[i] = 0;
  }

  /* */    /*E0067*/

  /* */    /*E0068*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);
  ToOnlyAxis   = GetOnlyAxis(ToBlockPtr);

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0069*/
     /* */    /*E0070*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
        tprintf("*** ACopyBlock: Axis -> Axis; Recv.\n");

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0071*/

        CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = CurrWriteBlock->Set[i].Lower;
        index_GetLI(MyMinLinInd, ToBlockPtr, WriteIndex, ToStep)

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = CurrWriteBlock->Set[i].Upper;
        index_GetLI(MyMaxLinInd, ToBlockPtr, WriteIndex, ToStep)

        /* */    /*E0072*/

        for(i=0; i < ReadVMSize; i++)
        {  Proc = ReadVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsReadInter[i] == 0)
              continue; /* */    /*E0073*/

           for(j=0; j < FromRank; j++)
               ReadIndex[j] = ReadLocalBlock[i].Set[j].Lower;
           index_GetLI(LinInd, FromBlockPtr, ReadIndex, FromStep)

           MinLinInd = dvm_max(LinInd, MyMinLinInd);

           for(j=0; j < FromRank; j++)
               ReadIndex[j] = ReadLocalBlock[i].Set[j].Upper;
           index_GetLI(LinInd, FromBlockPtr, ReadIndex, FromStep)

           MaxLinInd = dvm_min(LinInd, MyMaxLinInd);

           if(MinLinInd > MaxLinInd)
              continue;

           ReadBSize[i] = (int)
           ((MaxLinInd - MinLinInd + 1) * FromDArr->TLen);

           mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0074*/

              ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                         ReadBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &ReadReq[i], 1) );
           }
        } 
     }
  }
  else
  {
    /* Check if read block and written one are the same */    /*E0075*/

    if(FromRank == ToRank)
    {  for(i=0; i < ToRank; i++)
           if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size ||
              FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step   )
              break;
       if(i == ToRank)
          EquSign = 1;
    }

    if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
       tprintf("*** ACopyBlock: EquSign=%d\n", (int)EquSign);


    if(EquSign)
    {  dvm_AllocArray(s_BLOCK, ReadVMSize, ToLocalBlock);
    }
    else
    {  dvm_AllocArray(char *, WriteVMSize, WriteElmPtr);
       dvm_AllocArray(int, WriteVMSize, WriteVMInReadVM);

       VProc = WriteVMS->VProc;

       if(WriteVMS == ReadVMS)
       {  for(i=0; i < WriteVMSize; i++)
              WriteVMInReadVM[i] = (int)VProc[i].lP;
       }
       else
       {  for(i=0; i < WriteVMSize; i++)
              WriteVMInReadVM[i] = IsProcInVMS(VProc[i].lP, ReadVMS);
       }

       block_GetWeight(FromBlockPtr, ReadWeight, FromStep);
       block_GetWeight(ToBlockPtr, WriteWeight, ToStep);
    } 

    if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
    {  /* Written block overlap  the local part of current processor */    /*E0076*/

       CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];
       wWriteBlock = block_Copy(CurrWriteBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0077*/

          for(i=0; i < ToRank; i++)
          {  wWriteBlock.Set[i].Lower -= ToBlockPtr->Set[i].Lower;
             wWriteBlock.Set[i].Upper -= ToBlockPtr->Set[i].Lower;
          }

          for(i=0; i < ReadVMSize; i++)
          {  Proc = ReadVMS->VProc[i].lP;
             if(Proc == MPS_CurrentProc)
                continue;
             if(IsReadInter[i] == 0)
                continue;

             for(j=0; j < FromRank; j++)
             {  ReadLocalBlock[i].Set[j].Lower -=
                FromBlockPtr->Set[j].Lower;
                ReadLocalBlock[i].Set[j].Upper -=
                FromBlockPtr->Set[j].Lower;
             }

             k = block_Intersect(&ToLocalBlock[i], &wWriteBlock,
                                 &ReadLocalBlock[i], ToBlockPtr,
                                 EquSign);

             /*for(j=0; j < FromRank; j++)
             {  ReadLocalBlock[i].Set[j].Lower +=
                FromBlockPtr->Set[j].Lower;
                ReadLocalBlock[i].Set[j].Upper +=
                FromBlockPtr->Set[j].Lower;
             }*/    /*E0078*/

             if(k == 0)
                continue;

             block_GetSize(ReadBSize[i], &ToLocalBlock[i], EquSign)
             ReadBSize[i] *= FromDArr->TLen;

             for(j=0; j < ToRank; j++)
             {  ToLocalBlock[i].Set[j].Lower += ToBlockPtr->Set[j].Lower;
                ToLocalBlock[i].Set[j].Upper += ToBlockPtr->Set[j].Lower;
             }

             mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0079*/

                Proc = ReadVMS->VProc[i].lP;
                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0080*/

          block_GetSize(LinInd, CurrWriteBlock, ToStep)
          n = (int)LinInd; /* size of local part of written array */    /*E0081*/

          for(j=0; j < n; j++)
          {  index_FromBlock(WriteIndex, &wWriteBlock, CurrWriteBlock,
                             ToStep)
             index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)
             index_GetSI(FromBlockPtr, ReadWeight, LinInd, ReadIndex,
                         FromStep)

             if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
             {  IsElmOfBlock(k, &ReadLocalBlock[ReadVMS->CurrentProc],
                             ReadIndex)
                if(k)
                   continue;
             }

             for(i=0; i < ReadVMSize; i++)
             {  Proc = ReadVMS->VProc[i].lP;
                if(Proc == MPS_CurrentProc)
                   continue;

                if(IsReadInter[i] == 0)
                   continue;

                IsElmOfBlock(k, &ReadLocalBlock[i], ReadIndex)

                if(k)
                   ReadBSize[i] += FromDArr->TLen;
             }
          }

          for(i=0; i < ReadVMSize; i++)
          {  if(ReadBSize[i] == 0)
                continue;

             mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0082*/

                Proc = ReadVMS->VProc[i].lP;
                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
    }
  }

  /* */    /*E0083*/

  /* */    /*E0084*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0085*/
     /* */    /*E0086*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
        tprintf("*** ACopyBlock: Axis -> Axis; Send.\n");

     if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
     {  /* */    /*E0087*/

        CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = CurrReadBlock->Set[i].Lower;
        index_GetLI(MyMinLinInd, FromBlockPtr, ReadIndex, FromStep)

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = CurrReadBlock->Set[i].Upper;
        index_GetLI(MyMaxLinInd, FromBlockPtr, ReadIndex, FromStep)

        /* */    /*E0088*/

        for(i=0; i < WriteVMSize; i++)
        {  Proc = WriteVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsWriteInter[i] == 0)
              continue; /* */    /*E0089*/

           for(j=0; j < ToRank; j++)
               WriteIndex[j] = WriteLocalBlock[i].Set[j].Lower;
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)

           MinLinInd = dvm_max(LinInd, MyMinLinInd);

           for(j=0; j < ToRank; j++)
               WriteIndex[j] = WriteLocalBlock[i].Set[j].Upper;
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)

           MaxLinInd = dvm_min(LinInd, MyMaxLinInd);

           if(MinLinInd > MaxLinInd)
              continue;

           k = (int)(MaxLinInd - MinLinInd + 1); /* */    /*E0090*/
           n = ToDArr->TLen;
           m = k * n;
           WriteBSize[i] = m;   /* */    /*E0091*/

           mac_malloc(ReadElmPtr, char *, m, 0);

           WriteBuf[i] = (void *)ReadElmPtr;  /* */    /*E0092*/

           for(j=0; j < FromRank; j++)
               ReadIndex[j] = FromBlockPtr->Set[j].Lower;

           ReadIndex[FromOnlyAxis] +=
           (MinLinInd * FromBlockPtr->Set[FromOnlyAxis].Step);
        
           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0093*/

           ReadIndex[FromOnlyAxis] +=
           FromBlockPtr->Set[FromOnlyAxis].Step;

           LocElmAddr(CharPtr2, FromDArr, ReadIndex) /* */    /*E0094*/
           j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0095*/
           for(p=0; p < k; p++)
           {  CharPtr2 = CharPtr1;

              for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                  *ReadElmPtr = *CharPtr2;

              CharPtr1 += j;
           }

           Save_a_DA = a_DA_Axis_DA;

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0096*/

              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         m, (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], a_DA_Axis_DA) );
           }
        } 

        if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
           DVM_LEVEL == 0 && Alltoall == 0)
        {  rtl_TstReqColl(0);
           rtl_SendReqColl(ResCoeffDACopy);
        }
     }
  }
  else
  { if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
    {  /* Read block overlap the local part of current processor */    /*E0097*/

       CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];
       wReadBlock = block_Copy(CurrReadBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal*/    /*E0098*/

          for(i=0; i < FromRank; i++)
          {  wReadBlock.Set[i].Lower -= FromBlockPtr->Set[i].Lower;
             wReadBlock.Set[i].Upper -= FromBlockPtr->Set[i].Lower;
          }

          for(i=0; i < WriteVMSize; i++)
          {  Proc = WriteVMS->VProc[i].lP;

             if(Proc == MPS_CurrentProc)
                continue;
             if(IsWriteInter[i] == 0)
                continue;

             for(j=0; j < ToRank; j++)
             {  WriteLocalBlock[i].Set[j].Lower -=
                ToBlockPtr->Set[j].Lower;
                WriteLocalBlock[i].Set[j].Upper -=
                ToBlockPtr->Set[j].Lower;
             }

             k = block_Intersect(&FromLocalBlock, &wReadBlock,
                                 &WriteLocalBlock[i], FromBlockPtr,
                                 EquSign);

             /*for(j=0; j < ToRank; j++)
             {  WriteLocalBlock[i].Set[j].Lower +=
                ToBlockPtr->Set[j].Lower;
                WriteLocalBlock[i].Set[j].Upper +=
                ToBlockPtr->Set[j].Lower;
             }*/    /*E0099*/

             if(k == 0)
                continue;

             block_GetSize(WriteBSize[i], &FromLocalBlock, EquSign)
             WriteBSize[i] *= ToDArr->TLen;

             for(j=0; j < FromRank; j++)
             {  FromLocalBlock.Set[j].Lower +=
                FromBlockPtr->Set[j].Lower;
                FromLocalBlock.Set[j].Upper +=
                FromBlockPtr->Set[j].Lower;
             }

             mac_malloc(WriteBuf[i], void *, WriteBSize[i], 0);
             ReadElmPtr = (char *)WriteBuf[i];

             wWriteBlock = block_Copy(&FromLocalBlock);

             if(FromLocalBlock.Set[ToRank-1].Step == 1)
             {  /* Step on main dimension is equal to 1 */    /*E0100*/

                LinInd = FromLocalBlock.Set[FromRank-1].Size;
                n = (int)(LinInd * FromDArr->TLen);
                k = (int)(WriteBSize[i] / n);

                for(j=0; j < k; j++)
                {  index_FromBlock1S(ReadIndex, &wWriteBlock,
                                     &FromLocalBlock)
                   GetLocElm1(FromDArr, ReadIndex, ReadElmPtr, LinInd)
                   ReadElmPtr += n;
                }
             }
             else
             {  /* Step on main dimension is not equal to 1 */    /*E0101*/

                k = (int)(WriteBSize[i] / FromDArr->TLen);

                for(j=0; j < k; j++)
                {  index_FromBlock(ReadIndex, &wWriteBlock,
                                   &FromLocalBlock, EquSign)
                   GetLocElm(FromDArr, ReadIndex, ReadElmPtr)
                   ReadElmPtr += FromDArr->TLen;
                }
             }
 
             Save_a_DA = a_DA_EQ_DA;

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0102*/

                ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                           WriteBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &WriteReq[i], a_DA_EQ_DA) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0103*/

          block_GetSize(LinInd, CurrReadBlock, FromStep)
          n = (int)LinInd; /* size of local part of read array */    /*E0104*/

          for(j=0; j < n; j++)
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock,
                             FromStep)
             index_GetLI(LinInd, FromBlockPtr, ReadIndex, FromStep)
             index_GetSI(ToBlockPtr, WriteWeight, LinInd, WriteIndex,
                         ToStep)

             for(i=0; i < WriteVMSize; i++)
             {  Proc = WriteVMS->VProc[i].lP;

                if(Proc == MPS_CurrentProc)
                   continue;

                if(IsWriteInter[i] == 0)
                   continue;

                if( (m = WriteVMInReadVM[i]) >= 0 )
                {   if(IsReadInter[m])
                    {  IsElmOfBlock(k, &ReadLocalBlock[m], ReadIndex)
                       if(k)
                          continue;
                    }
                }

                IsElmOfBlock(k, &WriteLocalBlock[i], WriteIndex)

                if(k)
                   WriteBSize[i] += FromDArr->TLen;
             }
          }

          for(i=0; i < WriteVMSize; i++)
          {  if(WriteBSize[i] == 0)
                continue;
             mac_malloc(WriteBuf[i], void *, WriteBSize[i], 0);
             WriteElmPtr[i] = (char *)WriteBuf[i];
          }

          wReadBlock = block_Copy(CurrReadBlock);

          for(j=0; j < n; j++)
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock,
                             FromStep)
             index_GetLI(LinInd, FromBlockPtr, ReadIndex, FromStep)
             index_GetSI(ToBlockPtr, WriteWeight, LinInd, WriteIndex,
                         ToStep)

             for(i=0; i < WriteVMSize; i++)
             {  if(WriteBSize[i] == 0)
                   continue;

                if( (m = WriteVMInReadVM[i]) >= 0 )
                {    if(IsReadInter[m])
                     {  IsElmOfBlock(k, &ReadLocalBlock[m], ReadIndex)
                        if(k)
                           continue;
                     }
                } 

                IsElmOfBlock(k, &WriteLocalBlock[i], WriteIndex)
                if(k == 0)
                   continue;

                GetLocElm(FromDArr, ReadIndex, WriteElmPtr[i])
                WriteElmPtr[i] += FromDArr->TLen;
             }
          }

          Save_a_DA = a_DA_NE_DA;

          if(ExchangeScheme == 0 && Alltoall == 0)
          {  /* */    /*E0105*/

             for(i=0; i < WriteVMSize; i++)
             { if(WriteBSize[i])
                  ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                             WriteBSize[i],
                                             (int)WriteVMS->VProc[i].lP,
                                             DVM_VMS->tag_DACopy,
                                             &WriteReq[i], a_DA_NE_DA) );
             }
          }
       }

       if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
          DVM_LEVEL == 0 && Alltoall == 0)
       {  rtl_TstReqColl(0);
          rtl_SendReqColl(ResCoeffDACopy);
       }
    }
  }

  /* */    /*E0106*/

  if(ExchangeScheme)
  {  m = ReadVMSize % 2;
     n = (int)(ReadVMSize - 1 + m);
     k = ReadVMS->CurrentProc;

     j = 0;
     p = 0;

     while(p < n)
     {  p = dvm_min(j+MsgPairNumber, n);

        for(q=j; j < p; j++)
        {  i = (j + j - k + n)%n;

           if(m == 0)
           {  if(k == n)
              {  i = j;
              }
              else
              {  if(k == j)
                 {  i = n;
                 }
              }
           }
           else
           {  if(k == j)
                 continue;
           }

           if(k == i)
              continue;

           Proc = ReadVMS->VProc[i].lP;

           if(ReadBSize[i] != 0)
              ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                         ReadBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &ReadReq[i], 1) );
           if(WriteBSize[i] != 0)
              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         WriteBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], s_DA_EX_DA) );
        }

        for(j=q; j < p; j++)
        {  i = (j + j - k + n)%n;

           if(m == 0)
           {  if(k == n)
              {  i = j;
              }
              else
              {  if(k == j)
                 {  i = n;
                 }
              }
           }
           else
           {  if(k == j)
                 continue;
           }

           if(k == i)
              continue;

           if(WriteBSize[i] != 0)
              ( RTL_CALL, rtl_Waitrequest(&WriteReq[i]) );
           if(ReadBSize[i] != 0)
              (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));
        } 
     }
  }

  /* */    /*E0107*/

#ifdef  _DVM_MPI_

  /* */    /*E0108*/

  if(Alltoall)
  {
     if((MsgSchedule || MsgPartReg) && AlltoallWithMsgSchedule &&
        MaxMsgLength > 0)
     {
        r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0109*/

        for(i=0,k=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

           r = dvm_min(r, WriteBSize[i]);
           k = 1;
        }

        for(i=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              continue;

           r = dvm_min(r, ReadBSize[i]);
           k = 1;
        }
       
        if(k == 1 && r > MaxMsgLength)
        {  /* */    /*E0110*/

           Alltoall = 0;

           for(i=0; i < ReadVMSize; i++)
           {  if(ReadBSize[i] == 0)
                 continue;

                 Proc = ReadVMS->VProc[i].lP;

                 ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                            ReadBSize[i], (int)Proc,
                                            DVM_VMS->tag_DACopy,
                                            &ReadReq[i], 1) );
           }

           for(i=0; i < WriteVMSize; i++)
           {  if(WriteBSize[i] == 0)
                 continue;

                 Proc = WriteVMS->VProc[i].lP;

                 ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                            WriteBSize[i], (int)Proc,
                                            DVM_VMS->tag_DACopy,
                                            &WriteReq[i], Save_a_DA) );
           }

           if(MsgSchedule && UserSumFlag  && DVM_LEVEL == 0)
           {  /* */    /*E0111*/

              rtl_TstReqColl(0);
              rtl_SendReqColl(ResCoeffDACopy);
           }
        }
     }
  }

  if(Alltoall)
  {
     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0112*/

     /* */    /*E0113*/

     Proc = DVMTYPE_MAX;    /*E0114*/
     j = -1;

     for(i=0; i < WriteVMSize; i++)
     {  if(WriteBSize[i] == 0)
           continue;  /* */    /*E0115*/

        LinInd = (DvmType)WriteBuf[i];

        if(LinInd < Proc)
        {  j = i;
           Proc = LinInd;
        }
     }

     if(j < 0)
        sendbuf = (void *)&AlltoallMem;
     else
        sendbuf = WriteBuf[j];

     /* */    /*E0116*/

     MinLinInd = DVMTYPE_MAX;    /*E0117*/
     k = -1;

     for(i=0; i < ReadVMSize; i++)
     {  if(ReadBSize[i] == 0)
           continue;  /* */    /*E0118*/

        LinInd = (DvmType)ReadBuf[i];

        if(LinInd < MinLinInd)
        {  k = i;
           MinLinInd = LinInd;
        }
     }

     if(k < 0)
        recvbuf = (void *)&AlltoallMem;
     else
        recvbuf = ReadBuf[k];

     /* */    /*E0119*/

     dvm_AllocArray(int, WriteVMSize, sdispls);
     dvm_AllocArray(int, ReadVMSize,  rdispls);

     r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0120*/

     for(m=0; m < WriteVMSize; m++)
     {  if(WriteBSize[m] == 0)
           sdispls[m] = 0;
        else
        {
            LinInd = (DvmType)WriteBuf[m] - Proc;

           if(LinInd >= r)
              break;    /* */    /*E0121*/
           sdispls[m] = (int)LinInd;
        }
     }

     for(n=0; n < ReadVMSize; n++)
     {  if(ReadBSize[n] == 0)
           rdispls[n] = 0;
        else
        {
            LinInd = (DvmType)ReadBuf[n] - MinLinInd;

           if(LinInd >= r)
              break;    /* */    /*E0122*/
           rdispls[n] = (int)LinInd;
        }
     }

     if(m < WriteVMSize)
     {  /* */    /*E0123*/

        for(i=0,p=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              sdispls[i] = 0;
           else
           {  sdispls[i] = p;
              p += WriteBSize[i];
           }
        }

        mac_malloc(sendbuf, void *, p, 0);

        /* */    /*E0124*/

        CharPtr1 = (char *)sendbuf;

        for(i=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

          SYSTEM(memcpy, (CharPtr1, (char *)WriteBuf[i], WriteBSize[i]))
          CharPtr1 += WriteBSize[i];
        }
     }

     if(n < ReadVMSize)
     {  /* */    /*E0125*/

        for(i=0,q=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              rdispls[i] = 0;
           else
           {  rdispls[i] = q;
              q += ReadBSize[i];
           }
        }

        mac_malloc(recvbuf, void *, q, 0);
     }

     /* */    /*E0126*/

     if(RTL_TRACE && MPI_AlltoallTrace && TstTraceEvent(call_aarrcp_))
     {  if(m < WriteVMSize || n < ReadVMSize)
        {  if(m < WriteVMSize && n < ReadVMSize)
              tprintf("*** ACopyBlock: AllToAll-branch\n");
           else
           {  if(m < WriteVMSize)
                 tprintf("*** ACopyBlock: AllToAll-branch "
                         "(recv_fast)\n");
              else
                 tprintf("*** ACopyBlock: AllToAll-branch "
                         "(send_fast)\n");
           } 
        }
        else
           tprintf("*** ACopyBlock: AllToAll-branch (super_fast)\n");
     }

     /* */    /*E0127*/

     if(MPIInfoPrint && StatOff == 0)
        MPI_AlltoallvTime -= dvm_time();

     MPI_Alltoallv(sendbuf, WriteBSize, sdispls, MPI_CHAR,
                   recvbuf, ReadBSize,  rdispls, MPI_CHAR,
                   DVM_VMS->PS_MPI_COMM);

     if(MPIInfoPrint && StatOff == 0)
        MPI_AlltoallvTime += dvm_time();

     if(m < WriteVMSize)
     {  mac_free(&sendbuf);
     }

     if(n < ReadVMSize)
     {  /* */    /*E0128*/

        CharPtr1 = (char *)recvbuf;

        for(i=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              continue;

          SYSTEM(memcpy, ((char *)ReadBuf[i], CharPtr1, ReadBSize[i]))
          CharPtr1 += ReadBSize[i];
        }

        mac_free(&recvbuf);
     }

     dvm_FreeArray(sdispls);
     dvm_FreeArray(rdispls);

     DVMMTimeFinish;        /* */    /*E0129*/
  }

#endif

  /* --------------------------------------------- */    /*E0130*/

  dvm_FreeArray(WriteVMInReadVM);
  dvm_FreeArray(WriteElmPtr);

  CopyCont->Oper = sendrecv_ACopyBlock; /* code of continue operation */    /*E0131*/

  /* Save information for asynchronous continuation */    /*E0132*/

  CopyBlock->WriteVMSize      = WriteVMSize;
  CopyBlock->ReadVMSize       = ReadVMSize;
  CopyBlock->WriteBSize       = WriteBSize;
  CopyBlock->ReadBSize        = ReadBSize;
  CopyBlock->ReadVMS          = ReadVMS;
  CopyBlock->WriteVMS         = WriteVMS;
  CopyBlock->WriteReq         = WriteReq;
  CopyBlock->ReadReq          = ReadReq;
  CopyBlock->WriteBuf         = WriteBuf;
  CopyBlock->ReadBuf          = ReadBuf;
  CopyBlock->FromStep         = FromStep;
  CopyBlock->ToStep           = ToStep;
  CopyBlock->WriteLocalBlock  = WriteLocalBlock;
  CopyBlock->ReadLocalBlock   = ReadLocalBlock;
  CopyBlock->ToDArr           = ToDArr;
  CopyBlock->IsWriteInter     = IsWriteInter;
  CopyBlock->IsReadInter      = IsReadInter;
  CopyBlock->EquSign          = EquSign;
  CopyBlock->FromOnlyAxis     = FromOnlyAxis;
  CopyBlock->ToOnlyAxis       = ToOnlyAxis;
  CopyBlock->CurrWriteBlock   = CurrWriteBlock;
  CopyBlock->FromSuperFast    = 0;
  CopyBlock->ToSuperFast      = 0;

  CopyBlock->FromBlock = block_Copy(FromBlockPtr);
  CopyBlock->ToBlock   = block_Copy(ToBlockPtr);

  CopyBlock->ToLocalBlock     = ToLocalBlock;

  for(i=0; i < MAXARRAYDIM; i++)
      CopyBlock->ReadWeight[i] = ReadWeight[i];

  CopyBlock->ExchangeScheme = ExchangeScheme;
  CopyBlock->Alltoall       = Alltoall;

  return;
}



void  ACopyBlock1(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                  s_DISARRAY  *ToDArr,  s_BLOCK  *ToBlockPtr,
                 AddrType *CopyFlagPtr)
{ s_AMVIEW       *ReadAMS, *WriteAMS;
  s_VMS          *ReadVMS, *WriteVMS;
  DvmType            ReadVMSize, WriteVMSize;
  RTL_Request    *ReadReq = NULL, *WriteReq = NULL;
  void          **ReadBuf = NULL, **WriteBuf = NULL;
  int            *IsReadInter = NULL, *IsWriteInter = NULL,
                 *WriteVMInReadVM = NULL;
  s_BLOCK        *ReadLocalBlock = NULL, *WriteLocalBlock = NULL,
                 *ToLocalBlock = NULL;
  int            *ReadBSize = NULL, *WriteBSize = NULL;
  DvmType            Proc, LinInd, MyMinLinInd, MyMaxLinInd,
                  MinLinInd, MaxLinInd;
  int             i, j, k, n, m, p, q, r, ToRank, FromRank,
                  FromOnlyAxis, ToOnlyAxis;
  DvmType            ReadWeight[MAXARRAYDIM], WriteWeight[MAXARRAYDIM];
  s_BLOCK        *CurrReadBlock, *CurrWriteBlock;
  s_BLOCK         wReadBlock, wWriteBlock, FromLocalBlock;
  DvmType            ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  char          **WriteElmPtr = NULL, *ReadElmPtr = NULL,
                  *CharPtr1, *CharPtr2;
  byte            Step = 0, EquSign = 0, ExchangeScheme,
                  FromSuperFast = 0, ToSuperFast = 0, Alltoall = 0;
  s_COPYCONT     *CopyCont;
  s_ACopyBlock   *CopyBlock;
  byte           *IsVMSBlock;
  s_BLOCK       **VMSBlock;
  SysHandle      *VProc;

  int             CGSign = 0;
  int             FromVMAxis = 0, ToVMAxis = 0;
  byte           *FromVM = NULL, *ToVM = NULL;
  int             Save_a_DA = a_DA1_NE_DA1;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *sdispls, *rdispls;
#endif


  /* Structures for continuation */    /*E0133*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ACopyBlock, CopyBlock);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)CopyBlock;

  /* ------------------------- */    /*E0134*/

  ToRank   = ToBlockPtr->Rank;
  FromRank = FromBlockPtr->Rank;

  ReadAMS    = FromDArr->AMView;
  ReadVMS    = ReadAMS->VMS;
  ReadVMSize = ReadVMS->ProcCount;

  WriteAMS    = ToDArr->AMView;
  WriteVMS    = WriteAMS->VMS;
  WriteVMSize = WriteVMS->ProcCount;

  dvm_AllocArray(byte, ReadVMSize, FromVM);
  dvm_AllocArray(byte, WriteVMSize, ToVM);

  /* */    /*E0135*/

#ifdef  _DVM_MPI_

  if(A_MPIAlltoall && ReadVMS == DVM_VMS && WriteVMS == DVM_VMS &&
     (FromDArr->Every && ToDArr->Every) && DVM_VMS->Is_MPI_COMM != 0
     /*&& NoWaitCount == 0*/    /*E0136*/)
     Alltoall = 1;

#endif

  /* */    /*E0137*/

  if(ReadVMS == WriteVMS && ReadVMS->Space.Rank == 2)
  {  /* */    /*E0138*/

     if(FromRank == 1 && ToRank == 1 &&
        (FromDArr->HasLocal || ToDArr->HasLocal))
     {  /* */    /*E0139*/

        if(FromBlockPtr->Set[0].Lower == 0 &&
           ToBlockPtr->Set[0].Lower == 0)
        {  /* */    /*E0140*/

           if(FromDArr->DAAxis[0] != 0)
              FromVMAxis = 1;

           if(FromDArr->DAAxis[1] != 0)
              FromVMAxis = 2;

           if(ToDArr->DAAxis[0] != 0)
              ToVMAxis = 1;

           if(ToDArr->DAAxis[1] != 0)
              ToVMAxis = 2;

           if(FromVMAxis != 0 && ToVMAxis != 0)
           {  /* */    /*E0141*/

              if(FromVMAxis != ToVMAxis)
              {  /* */    /*E0142*/

                 CGSign = 1; /* */    /*E0143*/
              }
              else
              {  /* */    /*E0144*/
   
                 CGSign = -1; /* */    /*E0145*/
              }

              for(i=0; i < ReadVMSize; i++)
              {  FromVM[i] = 0;
                 ToVM[i] = 0;
              }
 
              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_aarrcp_))
                 tprintf("*** ACopyBlock1: cg-branch; CGSign=%d\n",
                         CGSign);
           }
        }
     }
  }

  /* */    /*E0146*/

  if(CGSign && CG_MPIAlltoall == 0)
     Alltoall = 0;

  /* */    /*E0147*/

  ExchangeScheme = (byte)(MsgExchangeScheme == 2 &&
                          ReadVMS == WriteVMS && ReadVMS->HasCurrent &&
                          CGSign == 0 && Alltoall == 0);

  /* ---------------------------------------- */    /*E0148*/

  dvm_AllocArray(RTL_Request, ReadVMSize, ReadReq);
  dvm_AllocArray(void *, ReadVMSize, ReadBuf);
  dvm_AllocArray(int, ReadVMSize, IsReadInter);
  dvm_AllocArray(s_BLOCK, ReadVMSize, ReadLocalBlock);
  dvm_AllocArray(int, ReadVMSize, ReadBSize);

  /* Create array of blocks of From array local parts 
     for processor system the From array is mapped on */    /*E0149*/

  VMSBlock   = FromDArr->VMSBlock;
  IsVMSBlock = FromDArr->IsVMSBlock;

  if(CGSign != 0)
  {  /* */    /*E0150*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(FromVMAxis == 2)
     {  /* */    /*E0151*/

        for(i=0; i < k; i++)
        {  if(IsVMSBlock[i] == 0)
           {  VMSBlock[i] = GetSpaceLB4Proc(i, ReadAMS, &FromDArr->Space,
                                            FromDArr->Align,
                                            FromBlockPtr,
                                            &FromDArr->VMSLocalBlock[i]);
              IsVMSBlock[i] = 1;
           }
           else
           {  if(VMSBlock[i] != NULL)
                 for(r=0; r < FromRank; r++)
                     VMSBlock[i]->Set[r].Step = 1;
           }

           q = i + k*j;
           
           for(p=i+k; p < q; p+=k)
           {
              if(IsVMSBlock[p] == 0)
              {  if(VMSBlock[i] != NULL)
                 {  VMSBlock[p] = &FromDArr->VMSLocalBlock[p];

                    VMSBlock[p]->Rank = 1;
                    VMSBlock[p]->Set[0] = VMSBlock[i]->Set[0];
                 }
                 else
                    VMSBlock[p] = NULL; 
    
                 IsVMSBlock[p] = 1;
              }
              else
              {  if(VMSBlock[p] != NULL)
                    for(r=0; r < FromRank; r++)
                        VMSBlock[p]->Set[r].Step = 1;
              }
           }
        }
     }
     else
     {  /* */    /*E0152*/

        n = j*k;

        for(i=0; i < n; i+=k)
        {  if(IsVMSBlock[i] == 0)
           {  VMSBlock[i] = GetSpaceLB4Proc(i, ReadAMS, &FromDArr->Space,
                                            FromDArr->Align,
                                            FromBlockPtr,
                                            &FromDArr->VMSLocalBlock[i]);
              IsVMSBlock[i] = 1;
           }
           else
           {  if(VMSBlock[i] != NULL)
                 for(r=0; r < FromRank; r++)
                     VMSBlock[i]->Set[r].Step = 1;
           }

           q = i + k;
           
           for(p=i+1; p < q; p++)
           {  if(IsVMSBlock[p] == 0)
              {  if(VMSBlock[i] != NULL)
                 {  VMSBlock[p] = &FromDArr->VMSLocalBlock[p];

                    VMSBlock[p]->Rank = 1;
                    VMSBlock[p]->Set[0] = VMSBlock[i]->Set[0];
                 }
                 else
                    VMSBlock[p] = NULL; 
    
                 IsVMSBlock[p] = 1;
              }
              else
              {  if(VMSBlock[p] != NULL)
                    for(r=0; r < FromRank; r++)
                        VMSBlock[p]->Set[r].Step = 1;
              }
           }
        }
     }
  }
  else
  {  for(i=0; i < ReadVMSize; i++)
     {  FromVM[i] = 1;

        if(IsVMSBlock[i] == 0)
        {  VMSBlock[i] = GetSpaceLB4Proc(i, ReadAMS, &FromDArr->Space,
                                         FromDArr->Align, FromBlockPtr,
                                         &FromDArr->VMSLocalBlock[i]);
           IsVMSBlock[i] = 1;
        }
        else
        {  if(VMSBlock[i] != NULL)
              for(r=0; r < FromRank; r++)
                  VMSBlock[i]->Set[r].Step = 1;
        }
     }
  }

  /* ------------------------------------------------- */    /*E0153*/

  dvm_AllocArray(RTL_Request, WriteVMSize, WriteReq);
  dvm_AllocArray(void *, WriteVMSize, WriteBuf);
  dvm_AllocArray(int, WriteVMSize, IsWriteInter);
  dvm_AllocArray(s_BLOCK, WriteVMSize, WriteLocalBlock);
  dvm_AllocArray(int, WriteVMSize, WriteBSize);

  /* Create array of blocks of To array local parts 
     for processor system the To array is mapped on */    /*E0154*/

  VMSBlock   = ToDArr->VMSBlock;
  IsVMSBlock = ToDArr->IsVMSBlock; 

  if(CGSign != 0)
  {  /* */    /*E0155*/

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(ToVMAxis == 2)
     {  /* */    /*E0156*/

        for(i=0; i < k; i++)
        {  if(IsVMSBlock[i] == 0)
           {  VMSBlock[i] = GetSpaceLB4Proc(i, WriteAMS, &ToDArr->Space,
                                            ToDArr->Align, ToBlockPtr,
                                            &ToDArr->VMSLocalBlock[i]);
              IsVMSBlock[i] = 1;
           }
           else
           {  if(VMSBlock[i] != NULL)
                 for(r=0; r < ToRank; r++)
                     VMSBlock[i]->Set[r].Step = 1;
           }

           q = i + k*j;
           
           for(p=i+k; p < q; p+=k)
           {  if(IsVMSBlock[p] == 0)
              {  if(VMSBlock[i] != NULL)
                 {  VMSBlock[p] = &ToDArr->VMSLocalBlock[p];

                    VMSBlock[p]->Rank = 1;
                    VMSBlock[p]->Set[0] = VMSBlock[i]->Set[0];
                 }
                 else
                    VMSBlock[p] = NULL; 
    
                 IsVMSBlock[p] = 1;
              }
              else
              {  if(VMSBlock[p] != NULL)
                    for(r=0; r < ToRank; r++)
                        VMSBlock[p]->Set[r].Step = 1;
              }
           }
        }
     }
     else
     {  /* */    /*E0157*/

        n = j*k;

        for(i=0; i < n; i+=k)
        {  if(IsVMSBlock[i] == 0)
           {  VMSBlock[i] = GetSpaceLB4Proc(i, WriteAMS, &ToDArr->Space,
                                            ToDArr->Align, ToBlockPtr,
                                            &ToDArr->VMSLocalBlock[i]);
              IsVMSBlock[i] = 1;
           }
           else
           {  if(VMSBlock[i] != NULL)
                 for(r=0; r < ToRank; r++)
                     VMSBlock[i]->Set[r].Step = 1;
           }

           q = i + k;
           
           for(p=i+1; p < q; p++)
           {  if(IsVMSBlock[p] == 0)
              {  if(VMSBlock[i] != NULL)
                 {  VMSBlock[p] = &ToDArr->VMSLocalBlock[p];

                    VMSBlock[p]->Rank = 1;
                    VMSBlock[p]->Set[0] = VMSBlock[i]->Set[0];
                 }
                 else
                    VMSBlock[p] = NULL; 
    
                 IsVMSBlock[p] = 1;
              }
              else
              {  if(VMSBlock[p] != NULL)
                    for(r=0; r < ToRank; r++)
                        VMSBlock[p]->Set[r].Step = 1;
              }
           }
        }
     }
  }
  else
  {  for(i=0; i < WriteVMSize; i++)
     {  ToVM[i] = 1;

        if(IsVMSBlock[i] == 0)
        {  VMSBlock[i] = GetSpaceLB4Proc(i, WriteAMS, &ToDArr->Space,
                                         ToDArr->Align, ToBlockPtr,
                                         &ToDArr->VMSLocalBlock[i]);
           IsVMSBlock[i] = 1;
        }
        else
        {  if(VMSBlock[i] != NULL)
              for(r=0; r < ToRank; r++)
                  VMSBlock[i]->Set[r].Step = 1;
        }
     }
  }

  /* ------------------------------------------------ */    /*E0158*/

  if(CGSign != 0)
  {  /* */    /*E0159*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(ToDArr->HasLocal)
     {  /* */    /*E0160*/

        VMSBlock   = FromDArr->VMSBlock;

        m = ToDArr->Block.Set[0].Lower;
        n = ToDArr->Block.Set[0].Upper;

        if(FromVMAxis == 2)
        {  /* */    /*E0161*/

           for(i=0; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = i;    /* */    /*E0162*/
                 break;
              }
           }

           for( ; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = i;    /* */    /*E0163*/
                 break;
              }
           }

           if(CGSign > 0)
              r = (int)(ReadVMS->CVP[2] % j) * k;
           else
              r = (int)ReadVMS->CVP[1] * k;

           m = q+r;

           for(i=p+r; i <= m; i++)  /* */    /*E0164*/
               FromVM[i] = 1;
        }
        else
        {  /* */    /*E0165*/

           for(r=0, i=0; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = r;    /* */    /*E0166*/
                 break;
              }
           }

           for( ; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = r;    /* */    /*E0167*/
                 break;
              } 
           }

           if(CGSign > 0) 
              r = (int)(ReadVMS->CVP[1] % k);
           else
              r = (int)ReadVMS->CVP[2];

           m = q*k + r;

           for(i=p*k+r; i <= m; i+=k)  /* */    /*E0168*/
               FromVM[i] = 1;
        }
     }

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(FromDArr->HasLocal)
     {  /* */    /*E0169*/

        VMSBlock   = ToDArr->VMSBlock;

        m = FromDArr->Block.Set[0].Lower;
        n = FromDArr->Block.Set[0].Upper;

        if(ToVMAxis == 2)
        {  /* */    /*E0170*/

           for(i=0; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = i;    /* */    /*E0171*/
                 break;
              }
           }

           for( ; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = i;    /* */    /*E0172*/
                 break;
              }
           }

           if(CGSign > 0)
           {  r = (int)ceil((double)j/(double)k); /* */    /*E0173*/
              m = (int)WriteVMS->CVP[2] + k*r;

              for( ; p <= q; p++)
              {  for(n=(int)WriteVMS->CVP[2]; n < m; n += k)
                 {  if(n >= j)
                       break;

                    i = p + n*k;  /* */    /*E0174*/
                    ToVM[i] = 1;
                 }
              }
           }
           else
           {  r = (int)ReadVMS->CVP[1] * k;
              m = q + r;
               
              for(i=p+r ; i <= m; i++)  /* */    /*E0175*/
                  ToVM[i] = 1;
           }
        }
        else
        {  /* */    /*E0176*/

           for(r=0, i=0; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = r;    /* */    /*E0177*/
                 break;
              }
           }

           for( ; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = r;    /* */    /*E0178*/
                 break;
              }
           }

           if(CGSign > 0)
           {  r = (int)ceil((double)k/(double)j); /* */    /*E0179*/
              m = (int)WriteVMS->CVP[1] + j*r;

              for( ; p <= q; p++)
              {  r = p*k;

                 for(n=(int)WriteVMS->CVP[1]; n < m; n += j)
                 {  if(n >= k)
                       break;

                    i = r + n;    /* */    /*E0180*/
                    ToVM[i] = 1;
                 }
              }
           }
           else
           {  r = (int)ReadVMS->CVP[2];
              m = q*k + r;

              for(i=p*k+r; i <= m; i+=k) /* */    /*E0181*/
                  ToVM[i] = 1;
           }
        }
     }
  }

  /* */    /*E0182*/

  VMSBlock   = FromDArr->VMSBlock;

  if(CGSign != 0)
  {  /* */    /*E0183*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(FromVMAxis == 2)
     {  /* */    /*E0184*/

        for(i=0; i < k; i++)
        {  ReadBSize[i] = 0;

           if(VMSBlock[i])
              IsReadInter[i] = BlockIntersect(&ReadLocalBlock[i],
                                              FromBlockPtr, VMSBlock[i]);
           else
              IsReadInter[i] = 0;

           r = IsReadInter[i];
           q = i + k*j;
           
           for(p=i+k; p < q; p+=k)
           {  ReadBSize[p] = 0;
              IsReadInter[p] = r;

              if(r)
              {  ReadLocalBlock[p].Rank = 1;
                 ReadLocalBlock[p].Set[0] = ReadLocalBlock[i].Set[0];
              }
           }
        }
     }
     else
     {  /* */    /*E0185*/

        n = j*k;

        for(i=0; i < n; i+=k)
        {  ReadBSize[i] = 0;

           if(VMSBlock[i])
              IsReadInter[i] = BlockIntersect(&ReadLocalBlock[i],
                                              FromBlockPtr, VMSBlock[i]);
           else
              IsReadInter[i] = 0;

           r = IsReadInter[i];
           q = i + k;
           
           for(p=i+1; p < q; p++)
           {  ReadBSize[p] = 0;
              IsReadInter[p] = r;

              if(r)
              {  ReadLocalBlock[p].Rank = 1;
                 ReadLocalBlock[p].Set[0] = ReadLocalBlock[i].Set[0];
              }
           }
        }
     }
  }
  else
  {  for(i=0; i < ReadVMSize; i++)
     {  ReadBSize[i] = 0;

        if(VMSBlock[i])
           IsReadInter[i] = BlockIntersect(&ReadLocalBlock[i],
                                           FromBlockPtr, VMSBlock[i]);
        else
           IsReadInter[i] = 0;
     }
  }

  /* */    /*E0186*/

  VMSBlock   = ToDArr->VMSBlock;

  if(CGSign != 0)
  {  /* */    /*E0187*/

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(ToVMAxis == 2)
     {  /* */    /*E0188*/

        for(i=0; i < k; i++)
        {  WriteBSize[i] = 0;

           if(VMSBlock[i])
              IsWriteInter[i] = BlockIntersect(&WriteLocalBlock[i],
                                               ToBlockPtr, VMSBlock[i]);
           else
              IsWriteInter[i] = 0;

           r = IsWriteInter[i];
           q = i + k*j;
           
           for(p=i+k; p < q; p+=k)
           {  WriteBSize[p] = 0;
              IsWriteInter[p] = r;

              if(r)
              {  WriteLocalBlock[p].Rank = 1;
                 WriteLocalBlock[p].Set[0] = WriteLocalBlock[i].Set[0];
              }
           }
        }
     }
     else
     {  /* */    /*E0189*/

        n = j*k;

        for(i=0; i < n; i+=k)
        {  WriteBSize[i] = 0;

           if(VMSBlock[i])
              IsWriteInter[i] = BlockIntersect(&WriteLocalBlock[i],
                                               ToBlockPtr, VMSBlock[i]);
           else
              IsWriteInter[i] = 0;

           r = IsWriteInter[i];
           q = i + k;
           
           for(p=i+1; p < q; p++)
           {  WriteBSize[p] = 0;
              IsWriteInter[p] = r;

              if(r)
              {  WriteLocalBlock[p].Rank = 1;
                 WriteLocalBlock[p].Set[0] = WriteLocalBlock[i].Set[0];
              }
           }
        }
     }
  }
  else
  {  for(i=0; i < WriteVMSize; i++)
     {  WriteBSize[i] = 0;

        if(VMSBlock[i])
           IsWriteInter[i] = BlockIntersect(&WriteLocalBlock[i],
                                            ToBlockPtr, VMSBlock[i]);
        else
          IsWriteInter[i] = 0;
     }
  }

  /* */    /*E0190*/

  /* */    /*E0191*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);
  ToOnlyAxis   = GetOnlyAxis(ToBlockPtr);

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0192*/
     /* */    /*E0193*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
        tprintf("*** ACopyBlock1: Axis -> Axis; Recv.\n");

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0194*/

        CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];

        MyMinLinInd = CurrWriteBlock->Set[ToOnlyAxis].Lower -
                      ToBlockPtr->Set[ToOnlyAxis].Lower;
        MyMaxLinInd = CurrWriteBlock->Set[ToOnlyAxis].Upper -
                      ToBlockPtr->Set[ToOnlyAxis].Lower;

        if(ToOnlyAxis == (ToRank-1) && ToDArr->TLen > 3 && IsSynchr == 0)
        {  ToSuperFast = 1;

           for(j=0; j < ToRank; j++)
               WriteIndex[j] = CurrWriteBlock->Set[j].Lower;
        }

        /* */    /*E0195*/

        for(i=0; i < ReadVMSize; i++)
        {  if(FromVM[i] == 0)
              continue;

           Proc = ReadVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsReadInter[i] == 0) 
              continue; /* */    /*E0196*/

           LinInd = ReadLocalBlock[i].Set[FromOnlyAxis].Lower -
                    FromBlockPtr->Set[FromOnlyAxis].Lower;

           MinLinInd = dvm_max(LinInd, MyMinLinInd);

           LinInd = ReadLocalBlock[i].Set[FromOnlyAxis].Upper -
                    FromBlockPtr->Set[FromOnlyAxis].Lower;

           MaxLinInd = dvm_min(LinInd, MyMaxLinInd);

           if(MinLinInd > MaxLinInd)
              continue;

           ReadBSize[i] = (int)
           ((MaxLinInd - MinLinInd + 1) * FromDArr->TLen);

           if(ToSuperFast == 0)
           {  /* */    /*E0197*/

              mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);
           }
           else
           {  /* */    /*E0198*/

              WriteIndex[ToOnlyAxis] =
              ToBlockPtr->Set[ToOnlyAxis].Lower + MinLinInd;
        
              LocElmAddr(CharPtr1, ToDArr, WriteIndex) /* */    /*E0199*/
              ReadBuf[i] = (void *)CharPtr1;
           }

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0200*/

              ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                         ReadBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &ReadReq[i], 1) );
           }
        } 
     }
  }
  else
  {
    /* Check if read block and written one are the same*/    /*E0201*/

    if(FromRank == ToRank)
    {  for(i=0; i < ToRank; i++)
           if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
       if(i == ToRank)
          EquSign = 1;
    }

    if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
       tprintf("*** ACopyBlock1: EquSign=%d\n", (int)EquSign);
  
    if(EquSign)
    {  dvm_AllocArray(s_BLOCK, ReadVMSize, ToLocalBlock);
    }
    else
    {  dvm_AllocArray(char *, WriteVMSize, WriteElmPtr);
       dvm_AllocArray(int, WriteVMSize, WriteVMInReadVM);

       VProc = WriteVMS->VProc;

       if(WriteVMS == ReadVMS)
       {  for(i=0; i < WriteVMSize; i++)
              WriteVMInReadVM[i] = (int)VProc[i].lP;
       }
       else
       {  for(i=0; i < WriteVMSize; i++)
              WriteVMInReadVM[i] = IsProcInVMS(VProc[i].lP, ReadVMS);
       }

       block_GetWeight(FromBlockPtr, ReadWeight, Step);
       block_GetWeight(ToBlockPtr, WriteWeight, Step);
    } 

    if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
    {  /* Written block overlap  the local part of current processor */    /*E0202*/

       CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];
       wWriteBlock = block_Copy(CurrWriteBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0203*/

          for(i=0; i < ToRank; i++)
          {  wWriteBlock.Set[i].Lower -= ToBlockPtr->Set[i].Lower;
             wWriteBlock.Set[i].Upper -= ToBlockPtr->Set[i].Lower;
          }

          for(i=0; i < ReadVMSize; i++)
          {  Proc = ReadVMS->VProc[i].lP;
             if(Proc == MPS_CurrentProc)
                continue;
             if(IsReadInter[i] == 0)
                continue;

             for(j=0; j < FromRank; j++)
             {  ReadLocalBlock[i].Set[j].Lower -=
                FromBlockPtr->Set[j].Lower;
                ReadLocalBlock[i].Set[j].Upper -=
                FromBlockPtr->Set[j].Lower;
             }

             k = BlockIntersect(&ToLocalBlock[i], &wWriteBlock,
                                &ReadLocalBlock[i]);

             /*for(j=0; j < FromRank; j++)
             {  ReadLocalBlock[i].Set[j].Lower +=
                FromBlockPtr->Set[j].Lower;
                ReadLocalBlock[i].Set[j].Upper +=
                FromBlockPtr->Set[j].Lower;
             }*/    /*E0204*/

             if(k == 0)
                continue;

             block_GetSize(ReadBSize[i], &ToLocalBlock[i], Step)
             ReadBSize[i] *= FromDArr->TLen;

             for(j=0; j < ToRank; j++)
             {  ToLocalBlock[i].Set[j].Lower += ToBlockPtr->Set[j].Lower;
                ToLocalBlock[i].Set[j].Upper += ToBlockPtr->Set[j].Lower;
             }

             mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0205*/

                Proc = ReadVMS->VProc[i].lP;
                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0206*/

          block_GetSize(LinInd, CurrWriteBlock, Step)
          n = (int)LinInd; /* size of local part of written array */    /*E0207*/

          for(j=0; j < n; j++)
          {  index_FromBlock(WriteIndex, &wWriteBlock, CurrWriteBlock,
                             Step)
             index_GetLI(LinInd, ToBlockPtr, WriteIndex, Step)
             index_GetSI(FromBlockPtr, ReadWeight, LinInd, ReadIndex,
                         Step)

             if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
             {  IsElmOfBlock(k, &ReadLocalBlock[ReadVMS->CurrentProc],
                             ReadIndex)
                if(k)
                   continue;
             }

             for(i=0; i < ReadVMSize; i++)
             {  Proc = ReadVMS->VProc[i].lP;
                if(Proc == MPS_CurrentProc)
                   continue;

                if(IsReadInter[i] == 0)
                   continue;

                IsElmOfBlock(k, &ReadLocalBlock[i], ReadIndex)
                if(k)
                   ReadBSize[i] += FromDArr->TLen;
             }
          }

          for(i=0; i < ReadVMSize; i++)
          {  if(ReadBSize[i] == 0)
                continue;

             mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0208*/

                Proc = ReadVMS->VProc[i].lP;
                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
    }
  }

  /* */    /*E0209*/

  /* */    /*E0210*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0211*/
     /* */    /*E0212*/

     if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
     {  /* */    /*E0213*/

        CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];

        MyMinLinInd = CurrReadBlock->Set[FromOnlyAxis].Lower -
                      FromBlockPtr->Set[FromOnlyAxis].Lower;
        MyMaxLinInd = CurrReadBlock->Set[FromOnlyAxis].Upper -
                      FromBlockPtr->Set[FromOnlyAxis].Lower;

        n = ToDArr->TLen;

        for(j=0; j < FromRank; j++)
            ReadIndex[j] = CurrReadBlock->Set[j].Lower;

        if(FromOnlyAxis == (FromRank-1) && FromDArr->TLen > 3 &&
           IsSynchr == 0)
           FromSuperFast = 1;

        /* */    /*E0214*/

        for(i=0; i < WriteVMSize; i++)
        {  if(ToVM[i] == 0)
              continue;

           Proc = WriteVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsWriteInter[i] == 0)
              continue; /* */    /*E0215*/

           LinInd = WriteLocalBlock[i].Set[ToOnlyAxis].Lower -
                    ToBlockPtr->Set[ToOnlyAxis].Lower;

           MinLinInd = dvm_max(LinInd, MyMinLinInd);

           LinInd = WriteLocalBlock[i].Set[ToOnlyAxis].Upper -
                    ToBlockPtr->Set[ToOnlyAxis].Lower;

           MaxLinInd = dvm_min(LinInd, MyMaxLinInd);

           if(MinLinInd > MaxLinInd)
              continue;

           k = (int)(MaxLinInd - MinLinInd + 1); /* */    /*E0216*/
           m = k * n;
           WriteBSize[i] = m;    /* */    /*E0217*/

           ReadIndex[FromOnlyAxis] =
           FromBlockPtr->Set[FromOnlyAxis].Lower + MinLinInd;

           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0218*/

           if(FromSuperFast)
           {  /* */    /*E0219*/

              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_aarrcp_))
                 tprintf("*** ACopyBlock1: Axis -> Axis; "
                         "Send (super fast)\n");

              WriteBuf[i] = (void *)CharPtr1;
           }
           else
           { mac_malloc(ReadElmPtr, char *, m, 0); /* */    /*E0220*/

             WriteBuf[i] = (void *)ReadElmPtr; /* */    /*E0221*/

             if(FromOnlyAxis == (FromRank-1))
             {  if(RTL_TRACE && dacopy_Trace &&
                   TstTraceEvent(call_aarrcp_))
                   tprintf("*** ACopyBlock1: Axis -> Axis; "
                           "Send (fast)\n");

                SYSTEM(memcpy, (ReadElmPtr, CharPtr1, m))
             }
             else
             {  if(RTL_TRACE && dacopy_Trace &&
                   TstTraceEvent(call_aarrcp_))
                   tprintf("*** ACopyBlock1: Axis -> Axis; Send.\n");

                ReadIndex[FromOnlyAxis]++;

                LocElmAddr(CharPtr2, FromDArr,
                           ReadIndex) /* */    /*E0222*/
                           j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0223*/
                for(p=0; p < k; p++)
                {  CharPtr2 = CharPtr1;

                   for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                       *ReadElmPtr = *CharPtr2;

                   CharPtr1 += j;
                }
             }
           }

           Save_a_DA = a_DA1_Axis_DA1;

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0224*/

              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         m, (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], a_DA1_Axis_DA1) );
           }
        } 

        if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
           DVM_LEVEL == 0 && Alltoall == 0)
        {  rtl_TstReqColl(0);
           rtl_SendReqColl(ResCoeffDACopy);
        }
     }
  }
  else
  { if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
    {  /* Read block overlap the local part of current processor */    /*E0225*/

       CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];
       wReadBlock = block_Copy(CurrReadBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0226*/

          for(i=0; i < FromRank; i++)
          {  wReadBlock.Set[i].Lower -= FromBlockPtr->Set[i].Lower;
             wReadBlock.Set[i].Upper -= FromBlockPtr->Set[i].Lower;
          }

          for(i=0; i < WriteVMSize; i++)
          {  Proc = WriteVMS->VProc[i].lP;
             if(Proc == MPS_CurrentProc)
                continue;
             if(IsWriteInter[i] == 0)
                continue;

             for(j=0; j < ToRank; j++)
             {  WriteLocalBlock[i].Set[j].Lower -=
                ToBlockPtr->Set[j].Lower;
                WriteLocalBlock[i].Set[j].Upper -=
                ToBlockPtr->Set[j].Lower;
             }

             k = BlockIntersect(&FromLocalBlock, &wReadBlock,
                                &WriteLocalBlock[i]);

             /*for(j=0; j < ToRank; j++)
             {  WriteLocalBlock[i].Set[j].Lower +=
                ToBlockPtr->Set[j].Lower;
                WriteLocalBlock[i].Set[j].Upper +=
                ToBlockPtr->Set[j].Lower;
             }*/    /*E0227*/

             if(k == 0)
                continue;

             block_GetSize(WriteBSize[i], &FromLocalBlock, Step)
             WriteBSize[i] *= ToDArr->TLen;

             for(j=0; j < FromRank; j++)
             {  FromLocalBlock.Set[j].Lower +=
                FromBlockPtr->Set[j].Lower;
                FromLocalBlock.Set[j].Upper +=
                FromBlockPtr->Set[j].Lower;
             }

             mac_malloc(WriteBuf[i], void *, WriteBSize[i], 0);
             ReadElmPtr = (char *)WriteBuf[i];

             wWriteBlock = block_Copy(&FromLocalBlock);

             LinInd = FromLocalBlock.Set[FromRank-1].Size;
             n = (int)(LinInd * FromDArr->TLen);
             k = (int)(WriteBSize[i] / n);

             for(j=0; j < k; j++)
             {  index_FromBlock1(ReadIndex, &wWriteBlock,
                                 &FromLocalBlock)
                GetLocElm1(FromDArr, ReadIndex, ReadElmPtr, LinInd)
                ReadElmPtr += n;
             }

             Save_a_DA = a_DA1_EQ_DA1;

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0228*/

                ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                           WriteBSize[i],
                                           (int)WriteVMS->VProc[i].lP,
                                           DVM_VMS->tag_DACopy,
                                           &WriteReq[i], a_DA1_EQ_DA1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal*/    /*E0229*/

          block_GetSize(LinInd, CurrReadBlock, Step)
          n = (int)LinInd; /* size of local part of read array */    /*E0230*/

          for(j=0; j < n; j++)
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock, Step)
             index_GetLI(LinInd, FromBlockPtr, ReadIndex, Step)
             index_GetSI(ToBlockPtr, WriteWeight, LinInd, WriteIndex,
             Step)

             for(i=0; i < WriteVMSize; i++)
             {  Proc = WriteVMS->VProc[i].lP;

                if(Proc == MPS_CurrentProc)
                   continue;

                if(IsWriteInter[i] == 0)
                   continue;

                if( (m = WriteVMInReadVM[i]) >= 0 )
                {   if(IsReadInter[m])
                    {  IsElmOfBlock(k, &ReadLocalBlock[m], ReadIndex)
                       if(k)
                          continue;
                    }
                }

                IsElmOfBlock(k, &WriteLocalBlock[i], WriteIndex)

                if(k)
                   WriteBSize[i] += FromDArr->TLen;
             }
          }

          for(i=0; i < WriteVMSize; i++)
          {  if(WriteBSize[i] == 0)
                continue;
             mac_malloc(WriteBuf[i], void *, WriteBSize[i], 0);
             WriteElmPtr[i] = (char *)WriteBuf[i];
          }

          wReadBlock = block_Copy(CurrReadBlock);

          for(j=0; j < n; j++)
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock, Step)
             index_GetLI(LinInd, FromBlockPtr, ReadIndex, Step)
             index_GetSI(ToBlockPtr, WriteWeight, LinInd, WriteIndex,
                         Step)

             for(i=0; i < WriteVMSize; i++)
             {  if(WriteBSize[i] == 0)
                   continue;

                if( (m = WriteVMInReadVM[i]) >= 0 )
                {    if(IsReadInter[m])
                     {  IsElmOfBlock(k, &ReadLocalBlock[m], ReadIndex)
                        if(k)
                           continue;
                     }
                } 

                IsElmOfBlock(k, &WriteLocalBlock[i], WriteIndex)
                if(k == 0)
                   continue;

                GetLocElm(FromDArr, ReadIndex, WriteElmPtr[i])
                WriteElmPtr[i] += FromDArr->TLen;
             }
          }

          Save_a_DA = a_DA1_NE_DA1;

          if(ExchangeScheme == 0 && Alltoall == 0)
          {  /* */    /*E0231*/

             for(i=0; i < WriteVMSize; i++)
             { if(WriteBSize[i])
                  ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                             WriteBSize[i],
                                             (int)WriteVMS->VProc[i].lP,
                                             DVM_VMS->tag_DACopy,
                                             &WriteReq[i],
                                             a_DA1_NE_DA1) );
             }
          }
       }

       if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
          DVM_LEVEL == 0 && Alltoall == 0)
       {  rtl_TstReqColl(0);
          rtl_SendReqColl(ResCoeffDACopy);
       }
    }
  }

  /* */    /*E0232*/

  if(ExchangeScheme)
  {  m = ReadVMSize % 2;
     n = (int)(ReadVMSize - 1 + m);
     k = ReadVMS->CurrentProc;

     j = 0;
     p = 0;

     while(p < n)
     {  p = dvm_min(j+MsgPairNumber, n);

        for(q=j; j < p; j++)
        {  i = (j + j - k + n)%n;

           if(m == 0)
           {  if(k == n)
              {  i = j;
              }
              else
              {  if(k == j)
                 {  i = n;
                 }
              }
           }
           else
           {  if(k == j)
                 continue;
           }

           if(k == i)
              continue;

           Proc = ReadVMS->VProc[i].lP;

           if(ReadBSize[i] != 0)
              ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                         ReadBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &ReadReq[i], 1) );
           if(WriteBSize[i] != 0)
              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         WriteBSize[i], (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], s_DA_EX_DA) );
        }

        for(j=q; j < p; j++)
        {  i = (j + j - k + n)%n;

           if(m == 0)
           {  if(k == n)
              {  i = j;
              }
              else
              {  if(k == j)
                 {  i = n;
                 }
              }
           }
           else
           {  if(k == j)
                 continue;
           }

           if(k == i)
              continue;

           if(WriteBSize[i] != 0)
              ( RTL_CALL, rtl_Waitrequest(&WriteReq[i]) );
           if(ReadBSize[i] != 0)
              (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));
        } 
     }
  }

  /* */    /*E0233*/

#ifdef  _DVM_MPI_

  /* */    /*E0234*/

  if(Alltoall)
  {
     if((MsgSchedule || MsgPartReg) && AlltoallWithMsgSchedule &&
        MaxMsgLength > 0)
     {
        j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0235*/

        for(i=0,k=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

           j = dvm_min(j, WriteBSize[i]);
           k = 1; 
        }
       
        for(i=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              continue;

           j = dvm_min(j, ReadBSize[i]);
           k = 1;
        }

        if(k == 1 && j > MaxMsgLength)
        {  /* */    /*E0236*/

           Alltoall = 0;

           for(i=0; i < ReadVMSize; i++)
           {  if(ReadBSize[i] == 0)
                 continue;

                 Proc = ReadVMS->VProc[i].lP;

                 ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                            ReadBSize[i], (int)Proc,
                                            DVM_VMS->tag_DACopy,
                                            &ReadReq[i], 1) );
           }

           for(i=0; i < WriteVMSize; i++)
           {  if(WriteBSize[i] == 0)
                 continue;

                 Proc = WriteVMS->VProc[i].lP;

                 ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                            WriteBSize[i], (int)Proc,
                                            DVM_VMS->tag_DACopy,
                                            &WriteReq[i], Save_a_DA) );
           }

           if(MsgSchedule && UserSumFlag  && DVM_LEVEL == 0)
           {  /* */    /*E0237*/

              rtl_TstReqColl(0);
              rtl_SendReqColl(ResCoeffDACopy);
           }
        }
     }
  }

  if(Alltoall)
  {
     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0238*/

     /* */    /*E0239*/

     Proc = DVMTYPE_MAX;    /*E0240*/
     j = -1;

     for(i=0; i < WriteVMSize; i++)
     {  if(WriteBSize[i] == 0)
           continue;  /* */    /*E0241*/

        LinInd = (DvmType)WriteBuf[i];

        if(LinInd < Proc)
        {  j = i;
           Proc = LinInd;
        }
     }

     if(j < 0)
        sendbuf = (void *)&AlltoallMem;
     else
        sendbuf = WriteBuf[j];

     /* */    /*E0242*/

     MinLinInd = DVMTYPE_MAX;    /*E0243*/
     k = -1;

     for(i=0; i < ReadVMSize; i++)
     {  if(ReadBSize[i] == 0)
           continue;  /* */    /*E0244*/

        LinInd = (DvmType)ReadBuf[i];

        if(LinInd < MinLinInd)
        {  k = i;
           MinLinInd = LinInd;
        }
     }

     if(k < 0)
        recvbuf = (void *)&AlltoallMem;
     else
        recvbuf = ReadBuf[k];

     /* */    /*E0245*/

     dvm_AllocArray(int, WriteVMSize, sdispls);
     dvm_AllocArray(int, ReadVMSize,  rdispls);

     r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0246*/

     for(m=0; m < WriteVMSize; m++)
     {  if(WriteBSize[m] == 0)
           sdispls[m] = 0;
        else
        {
            LinInd = (DvmType)WriteBuf[m] - Proc;

           if(LinInd >= r)
              break;    /* */    /*E0247*/
           sdispls[m] = (int)LinInd;
        }
     }

     for(n=0; n < ReadVMSize; n++)
     {  if(ReadBSize[n] == 0)
           rdispls[n] = 0;
        else
        {
            LinInd = (DvmType)ReadBuf[n] - MinLinInd;

           if(LinInd >= r)
              break;    /* */    /*E0248*/
           rdispls[n] = (int)LinInd;
        }
     }

     if(m < WriteVMSize)
     {  /* */    /*E0249*/

        for(i=0,p=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              sdispls[i] = 0;
           else
           {  sdispls[i] = p;
              p += WriteBSize[i];
           }
        }

        mac_malloc(sendbuf, void *, p, 0);

        /* */    /*E0250*/

        CharPtr1 = (char *)sendbuf;

        for(i=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

          SYSTEM(memcpy, (CharPtr1, (char *)WriteBuf[i], WriteBSize[i]))
          CharPtr1 += WriteBSize[i];
        }
     }

     if(n < ReadVMSize)
     {  /* */    /*E0251*/

        for(i=0,q=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              rdispls[i] = 0;
           else
           {  rdispls[i] = q;
              q += ReadBSize[i];
           }
        }

        mac_malloc(recvbuf, void *, q, 0);
     }

     /* */    /*E0252*/

     if(RTL_TRACE && MPI_AlltoallTrace && TstTraceEvent(call_aarrcp_))
     {  if(m < WriteVMSize || n < ReadVMSize)
        {  if(m < WriteVMSize && n < ReadVMSize)
              tprintf("*** ACopyBlock1: AllToAll-branch\n");
           else
           {  if(m < WriteVMSize)
                 tprintf("*** ACopyBlock1: AllToAll-branch "
                         "(recv_fast)\n");
              else
                 tprintf("*** ACopyBlock1: AllToAll-branch "
                         "(send_fast)\n");
           } 
        }
        else
           tprintf("*** ACopyBlock1: AllToAll-branch (super_fast)\n");
     }

     /* */    /*E0253*/

     if(MPIInfoPrint && StatOff == 0)
        MPI_AlltoallvTime -= dvm_time();

     MPI_Alltoallv(sendbuf, WriteBSize, sdispls, MPI_CHAR,
                   recvbuf, ReadBSize,  rdispls, MPI_CHAR,
                   DVM_VMS->PS_MPI_COMM);

     if(MPIInfoPrint && StatOff == 0)
        MPI_AlltoallvTime += dvm_time();

     if(m < WriteVMSize)
     {  mac_free(&sendbuf);
     }

     if(n < ReadVMSize)
     {  /* */    /*E0254*/

        CharPtr1 = (char *)recvbuf;

        for(i=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              continue;

          SYSTEM(memcpy, ((char *)ReadBuf[i], CharPtr1, ReadBSize[i]))
          CharPtr1 += ReadBSize[i];
        }

        mac_free(&recvbuf);
     }

     dvm_FreeArray(sdispls);
     dvm_FreeArray(rdispls);

     DVMMTimeFinish;    /* */    /*E0255*/
  }

#endif

  /* --------------------------------------------- */    /*E0256*/

  dvm_FreeArray(WriteVMInReadVM);
  dvm_FreeArray(WriteElmPtr);

  CopyCont->Oper = sendrecv_ACopyBlock1; /* code of continue operation */    /*E0257*/

  /* Save information for asynchronous continuation */    /*E0258*/

  CopyBlock->WriteVMSize      = WriteVMSize;
  CopyBlock->ReadVMSize       = ReadVMSize;
  CopyBlock->WriteBSize       = WriteBSize;
  CopyBlock->ReadBSize        = ReadBSize;
  CopyBlock->WriteVMS         = WriteVMS;
  CopyBlock->ReadVMS          = ReadVMS;
  CopyBlock->WriteReq         = WriteReq;
  CopyBlock->ReadReq          = ReadReq;
  CopyBlock->WriteBuf         = WriteBuf;
  CopyBlock->ReadBuf          = ReadBuf;
  CopyBlock->FromStep         = 0;
  CopyBlock->ToStep           = 0;
  CopyBlock->WriteLocalBlock  = WriteLocalBlock;
  CopyBlock->ReadLocalBlock   = ReadLocalBlock;
  CopyBlock->ToDArr           = ToDArr;
  CopyBlock->IsWriteInter     = IsWriteInter;
  CopyBlock->IsReadInter      = IsReadInter;
  CopyBlock->EquSign          = EquSign;
  CopyBlock->FromOnlyAxis     = FromOnlyAxis;
  CopyBlock->ToOnlyAxis       = ToOnlyAxis;
  CopyBlock->CurrWriteBlock   = CurrWriteBlock;
  CopyBlock->FromSuperFast    = FromSuperFast;
  CopyBlock->ToSuperFast      = ToSuperFast;

  CopyBlock->FromBlock = block_Copy(FromBlockPtr);
  CopyBlock->ToBlock   = block_Copy(ToBlockPtr);

  CopyBlock->ToLocalBlock     = ToLocalBlock;

  for(i=0; i < MAXARRAYDIM; i++)
      CopyBlock->ReadWeight[i] = ReadWeight[i];

  CopyBlock->ExchangeScheme = ExchangeScheme;
  CopyBlock->Alltoall       = Alltoall;

  CopyBlock->ToVM   = ToVM;
  CopyBlock->FromVM = FromVM;

  return;
}


#endif   /* _ADACOPY_C_ */    /*E0259*/
