#ifndef _DACOPY_C_
#define _DACOPY_C_
/****************/    /*E0000*/

/**************************************\ 
* Functions to copy distributed arrays *
\**************************************/    /*E0001*/

DvmType __callstd arrcpy_(DvmType FromArrayHeader[], DvmType FromInitIndexArray[],
                          DvmType FromLastIndexArray[],DvmType FromStepArray[],
                          DvmType ToArrayHeader[], DvmType ToInitIndexArray[],
                          DvmType ToLastIndexArray[], DvmType ToStepArray[],
                          DvmType *CopyRegimPtr)

/*        
     Copy distributed arrays.
     ------------------------

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

  if(DACopySynchr)
     (RTL_CALL, bsynch_()); /* */    /*E0003*/

  DVMFTimeStart(call_arrcpy_);

  /* Forward to the next element of message tag circle tag_DACopy
    for the current processor system */    /*E0004*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0005*/

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 094.000: wrong call arrcpy_\n"
              "(FromArray and ToArray are not distributed arrays;\n"
              "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
              (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     /* copy distributed array -> memory */    /*E0006*/

     FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;
     FromRank = FromDArr->Space.Rank;
     FromAMV = FromDArr->AMView;

     if(RTL_TRACE)
     {  dvm_trace(call_arrcpy_,
                  "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                  "ToBufferPtr=%lx; CopyRegim=%ld;\n",
                  (uLLng)FromArrayHeader, FromArrayHeader[0],
                  (uLLng)ToArrayHeader, *CopyRegimPtr);

        if(TstTraceEvent(call_arrcpy_))
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
              "*** RTS err 094.004: wrong call arrcpy_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     Res = GetIndexArray(FromArrayHandlePtr, FromInitIndexArray,
                         FromLastIndexArray, FromStepArray,
                         OutFromInitIndex, OutFromLastIndex,
                         OutFromStep, 0);
     if(Res == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 094.001: wrong call arrcpy_\n"
                 "(invalid from index or from step; "
                 "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     if(EnableDynControl)
        dyn_DisArrTestVal(FromArrayHandlePtr, OutFromInitIndex, OutFromLastIndex, OutFromStep);
     NotSubsystem(i, DVM_VMS, FromAMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 094.006: wrong call arrcpy_\n"
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
                       "*** RTS err 094.002: wrong call arrcpy_\n"
                       "(CopyRegim=%ld < 0)\n", *CopyRegimPtr);

           if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = IOGetBlockRepl((char *)ToArrayHeader,FromDArr,
                                   &ReadBlock);
           else
              Res = IOGetBlock((char *)ToArrayHeader, FromDArr,
                               &ReadBlock);
        }
        else
        {  if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = GetBlockRepl((char *)ToArrayHeader,FromDArr,
                                 &ReadBlock);
           else
              Res = GetBlock((char *)ToArrayHeader, FromDArr,
                             &ReadBlock);
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
                       "*** RTS err 094.002: wrong call arrcpy_\n"
                       "(CopyRegim=%ld < 0)\n", *CopyRegimPtr);

           if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = IOGetBlockRepl((char *)ToArrayHeader,FromDArr,
                                   &ReadBlock);
           else
              Res = IOGetBlock((char *)ToArrayHeader, FromDArr,
                               &ReadBlock);
        }
        else
        {  if(FromDArr->Repl && FromAMV->VMS == DVM_VMS)
              Res = GetBlockRepl((char *)ToArrayHeader,FromDArr,
                                 &ReadBlock);
           else
              Res = GetBlock((char *)ToArrayHeader, FromDArr,
                             &ReadBlock);
        }
     }
  }
  else
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
     {  /* Š®¯¨à®¢ ­¨¥ €ŒŸ’œ -> €‘…„…‹ð›‰ Œ€‘‘ˆ‚ */    /*E0010*/

        ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;
        ToRank = ToDArr->Space.Rank;
        ToAMV = ToDArr->AMView;

        if(RTL_TRACE)
        {  dvm_trace(call_arrcpy_,
                     "FromBufferPtr=%lx; ToArrayHeader=%lx; "
                     "ToArrayHandlePtr=%lx; CopyRegim=%ld;\n",
                     (uLLng)FromArrayHeader, (uLLng)ToArrayHeader,
                     ToArrayHeader[0], *CopyRegimPtr);

           if(TstTraceEvent(call_arrcpy_))
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
                 "*** RTS err 094.005: wrong call arrcpy_\n"
                 "(ToArray has not been aligned; "
                 "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        Res = GetIndexArray(ToArrayHandlePtr, ToInitIndexArray,
                            ToLastIndexArray, ToStepArray,
                            OutToInitIndex, OutToLastIndex, OutToStep,
                            0);
        if(Res == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 094.003: wrong call arrcpy_\n"
                    "(invalid to index or to step; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        if(EnableDynControl)
           dyn_DisArrSetVal(ToArrayHandlePtr, OutToInitIndex,
                            OutToLastIndex, OutToStep);

        NotSubsystem(i, DVM_VMS, ToAMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 094.007: wrong call arrcpy_\n"
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
                                 &WriteBlock);  /* filling in */    /*E0013*/
              else
              {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                    Res = IOPutBlockRepl((char *)FromArrayHeader,
                                         ToDArr, &WriteBlock);
                 else
                    Res = IOPutBlock((char *)FromArrayHeader,
                                     ToDArr, &WriteBlock);
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
                                 &WriteBlock);  /* filling in */    /*E0015*/
              else
              {  if(ToDArr->Repl && ToAMV->VMS == DVM_VMS)
                    Res = IOPutBlockRepl((char *)FromArrayHeader,
                                         ToDArr, &WriteBlock);
                 else
                    Res = IOPutBlock((char *)FromArrayHeader,
                                     ToDArr, &WriteBlock);
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
        {  dvm_trace(call_arrcpy_,
                     "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                     "ToArrayHeader=%lx; ToArrayHandlePtr=%lx; "
                     "CopyRegim=%ld;\n",
                     (uLLng)FromArrayHeader, FromArrayHeader[0],
                     (uLLng)ToArrayHeader, ToArrayHeader[0],
                     *CopyRegimPtr);

           if(TstTraceEvent(call_arrcpy_))
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
                 "*** RTS err 094.005: wrong call arrcpy_\n"
                 "(ToArray has not been aligned; "
                 "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        if(FromAMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 094.004: wrong call arrcpy_\n"
                 "(FromArray has not been aligned; "
                 "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        FromBSize = GetIndexArray(FromArrayHandlePtr, FromInitIndexArray,
                                  FromLastIndexArray, FromStepArray,
                                  OutFromInitIndex, OutFromLastIndex,
                                  OutFromStep, 0);
        if(FromBSize == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 094.001: wrong call arrcpy_\n"
                    "(invalid from index or from step; "
                    "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        ToBSize = GetIndexArray(ToArrayHandlePtr, ToInitIndexArray,
                                ToLastIndexArray, ToStepArray,
                                OutToInitIndex, OutToLastIndex,
                                OutToStep, 0);

        if(ToBSize == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 094.003: wrong call arrcpy_\n"
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
             "*** RTS err 094.007: wrong call arrcpy_\n"
             "(the ToArray PS is not a subsystem of "
             "the current PS;\n"
             "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
             "CurrentPSRef=%lx)\n",
             ToArrayHeader[0], (uLLng)ToAMV->VMS->HandlePtr,
             (uLLng)DVM_VMS->HandlePtr);

        NotSubsystem(i, DVM_VMS, FromAMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 094.006: wrong call arrcpy_\n"
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
                    CopyBlock1(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock);
                 else
                 {  if(S_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       CopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                  &WriteBlock);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_arrcpy_))
                    tprintf("*** arrcpy_: CopyBlockRepl1 branch "
                            "(n=%d)\n", n);
              } 
              else
              {  RC = AttemptLocCopy1(FromDArr, &ReadBlock,
                                      ToDArr, &WriteBlock);
                 if(n || RC)
                    CopyBlock1(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock);
                 else
                 {  if(S_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       CopyBlock1(FromDArr, &ReadBlock, ToDArr,
                                  &WriteBlock);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_arrcpy_))
                    tprintf("*** arrcpy_: AttemptLocCopy1 branch "
                            "(n=%d  RC=%d)\n", n, RC);
              }
           }
           else
           {  /* steps on dimensions of arrays are not always equal to 1*/    /*E0021*/

              if(FromDArr->Repl)
              {  CopyBlockRepl(FromDArr, &ReadBlock, ToDArr,
                               &WriteBlock);
                 if(n)
                    CopyBlock(FromDArr, &ReadBlock, ToDArr,
                              &WriteBlock);
                 else
                 {  if(S_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       CopyBlock(FromDArr, &ReadBlock, ToDArr,
                                 &WriteBlock);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_arrcpy_))
                    tprintf("*** arrcpy_: CopyBlockRepl branch "
                            "(n=%d)\n", n);
              } 
              else
              {  RC = AttemptLocCopy(FromDArr, &ReadBlock,
                                     ToDArr, &WriteBlock);
                 if(n || RC)
                    CopyBlock(FromDArr, &ReadBlock, ToDArr,
                              &WriteBlock);
                 else
                 {  if(S_MPIAlltoall &&
                       FromDArr->AMView->VMS == DVM_VMS &&
                       ToDArr->AMView->VMS == DVM_VMS &&
                       (FromDArr->Every && ToDArr->Every) &&
                       DVM_VMS->Is_MPI_COMM != 0)
                       CopyBlock(FromDArr, &ReadBlock, ToDArr,
                                 &WriteBlock);
                 }

                 if(RTL_TRACE && dacopy_Trace &&
                    TstTraceEvent(call_arrcpy_))
                    tprintf("*** arrcpy_: AttemptLocCopy branch "
                            "(n=%d  RC=%d)\n", n, RC);
              }
           }
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_arrcpy_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_arrcpy_);
  return  (DVM_RET, Res);
}


/* ----------------------------------------------------- */    /*E0022*/


DvmType  GetBlock(char *BufferPtr, s_DISARRAY *DArr, s_BLOCK *ReadBlockPtr)
{ s_AMVIEW     *AMS;
  s_VMS        *VMS;
  int           VMSize, DVM_VMSize, i, j, k, Proc;
  RTL_Request  *RecvReq = NULL, *SendReq = NULL;
  int          *RecvFlag = NULL, *SendFlag = NULL;
  s_BLOCK      *RecvBlock = NULL, *wLocalBlock = NULL;
  s_BLOCK       ReadLocalBlock, wBlock;
  int           SendBSize, RecvBSize;
  void         *SendBuf = NULL, **RecvBuf = NULL;
  byte          Step;
  DvmType          tlong;

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

  for(i=0; i < DVM_VMSize; i++)
  {  RecvFlag[i] = 0;
     SendFlag[i] = 0;
  }

  if(DArr->HasLocal && block_Intersect(&ReadLocalBlock, ReadBlockPtr,
                                       &DArr->Block, ReadBlockPtr,
                                       Step))
  {  /* Local part of read block is not empty */    /*E0023*/

     block_GetSize(tlong, &ReadLocalBlock, Step)
     SendBSize = (int)(tlong * (DArr->TLen));
     mac_malloc(SendBuf, void *, SendBSize, 0);
     CopyBlockToMem((char *)SendBuf, &ReadLocalBlock, DArr, Step);
     CopySubmemToMem(BufferPtr, ReadBlockPtr, SendBuf, &ReadLocalBlock,
                     DArr->TLen, Step);

     /* Pass local part of read block to other processors */    /*E0024*/

     for(i=0; i < DVM_VMSize; i++)
     {  Proc = (int)DVM_VMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;

        /* !!!!!!!!!!!!!!!!!!!!!! */    /*E0025*/

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
                                      &SendReq[i], s_DA_Mem) );
           SendFlag[i] = 1;
        }
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }
  }

  /* Receive missing parts of read block from other processors */    /*E0026*/

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
           RecvBSize = (int)(tlong * (DArr->TLen));
           mac_malloc(RecvBuf[i], void *, RecvBSize, 0);
           RecvFlag[i] = 1;
           ( RTL_CALL, rtl_Recvnowait(RecvBuf[i], 1, RecvBSize, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &RecvReq[i], 1) );
        }
     }
  }

  /* Wait   for the end of passing */    /*E0027*/

  for(i=0; i < DVM_VMSize; i++)
  {  if(SendFlag[i])
	( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
  }

  mac_free(&SendBuf);

  /* Wait for the end of receiving and rewrite received parts */    /*E0028*/

  for(i=0; i < VMSize; i++)
  {  if(RecvFlag[i] == 0)
        continue;
     ( RTL_CALL, rtl_Waitrequest(&RecvReq[i]) );
     CopySubmemToMem(BufferPtr, ReadBlockPtr, RecvBuf[i], &RecvBlock[i],
		     DArr->TLen, Step);
     mac_free((void **)&RecvBuf[i]);
  }

  /* End of work */    /*E0029*/

  dvm_FreeArray(RecvReq);
  dvm_FreeArray(RecvFlag);
  dvm_FreeArray(RecvBuf);
  dvm_FreeArray(RecvBlock);
  dvm_FreeArray(SendReq);
  dvm_FreeArray(SendFlag);

  block_GetSize(tlong, ReadBlockPtr, Step)
  return   tlong;
}



DvmType   GetBlockRepl(char *BufferPtr, s_DISARRAY  *DArr,
                    s_BLOCK  *ReadBlockPtr)
{ byte  Step;
  DvmType  tlong;

  Step = GetStepSign(ReadBlockPtr);

  CopyBlockToMem(BufferPtr, ReadBlockPtr, DArr, Step);

  block_GetSize(tlong, ReadBlockPtr, Step)
  return  tlong;
}



DvmType   PutBlock(char  *BufferPtr, s_DISARRAY  *DArr,
                s_BLOCK  *WriteBlockPtr)
{ s_BLOCK      WriteLocalBlock;
  DvmType         WriteBSize;
  void        *WriteBuf = NULL;
  byte         Step;

  Step = GetStepSign(WriteBlockPtr);

  if(DArr->HasLocal && block_Intersect(&WriteLocalBlock, WriteBlockPtr,
                                       &DArr->Block, WriteBlockPtr,
                                       Step))
  {  /* Local part of written block  is not empty */    /*E0030*/

     block_GetSize(WriteBSize, &WriteLocalBlock, Step)
     WriteBSize *= DArr->TLen;
     mac_malloc(WriteBuf, void *, WriteBSize, 0);
     CopyMemToSubmem(WriteBuf, &WriteLocalBlock, BufferPtr,
                     WriteBlockPtr, DArr->TLen, Step);
     CopyMemToBlock(DArr, (char *)WriteBuf, &WriteLocalBlock, Step);
     mac_free(&WriteBuf);
  }

  block_GetSize(WriteBSize, WriteBlockPtr, Step)
  return  WriteBSize;
}



DvmType   PutBlockRepl(char  *BufferPtr, s_DISARRAY  *DArr,
                    s_BLOCK  *WriteBlockPtr)
{ byte  Step;
  DvmType  tlong;

  Step = GetStepSign(WriteBlockPtr);

  CopyMemToBlock(DArr, BufferPtr, WriteBlockPtr, Step);

  block_GetSize(tlong, WriteBlockPtr, Step)
  return  tlong;
}



DvmType   IOGetBlock(char  *BufferPtr, s_DISARRAY  *DArr,
                  s_BLOCK  *ReadBlockPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, k, Proc;
  RTL_Request   *RecvReq = NULL, SendReq;
  int           *RecvFlag = NULL;
  s_BLOCK        ReadLocalBlock, wBlock;
  s_BLOCK       *RecvBlock = NULL, *wLocalBlock = NULL;
  DvmType           ReadBSize, SendBSize, RecvBSize;
  void          *ReadBuf = NULL, *SendBuf = NULL, **RecvBuf = NULL;
  byte           Step;

  Step = GetStepSign(ReadBlockPtr);

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, RecvReq);
  dvm_AllocArray(int, VMSize, RecvFlag);
  dvm_AllocArray(void *, VMSize, RecvBuf);
  dvm_AllocArray(s_BLOCK, VMSize, RecvBlock);

  for(i=0; i < VMSize; i++)
      RecvFlag[i]=0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0031*/

     if(DArr->HasLocal && block_Intersect(&ReadLocalBlock, ReadBlockPtr,
                                          &DArr->Block, ReadBlockPtr,
                                          Step))
     {  /* Local part of read block is not empty */    /*E0032*/

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

     for(i=0; i < VMSize; i++)
     {  if(RecvFlag[i] == 0)
           continue;
        ( RTL_CALL, rtl_Waitrequest(&RecvReq[i]) );
        CopySubmemToMem(BufferPtr, ReadBlockPtr, RecvBuf[i],
                        &RecvBlock[i], DArr->TLen, Step);
        mac_free((void **)&RecvBuf[i]);
     }
  } 
  else
  {  /* Current processor is not I/O processor */    /*E0033*/

     if(DArr->HasLocal && block_Intersect(&ReadLocalBlock, ReadBlockPtr,
                                          &DArr->Block, ReadBlockPtr,
                                          Step))
     {  /* Local part of read block is not empty */    /*E0034*/

        /* !!!!!!!!!!!!!!!!!!!!!! */    /*E0035*/

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

           ( RTL_CALL, rtl_Sendnowait(SendBuf, 1, (int)SendBSize,
	                              DVM_IOProc, DVM_VMS->tag_DACopy,
                                      &SendReq, s_DA_IOMem) );
           ( RTL_CALL, rtl_Waitrequest(&SendReq) );

           mac_free(&SendBuf);
        }
     }
  }

  dvm_FreeArray(RecvReq);
  dvm_FreeArray(RecvFlag);
  dvm_FreeArray(RecvBuf);
  dvm_FreeArray(RecvBlock);

  block_GetSize(ReadBSize, ReadBlockPtr, Step)
  return  ReadBSize;
}



DvmType   IOGetBlockRepl(char  *BufferPtr, s_DISARRAY  *DArr,
                      s_BLOCK  *ReadBlockPtr)
{ byte  Step;
  DvmType  tlong;

  Step = GetStepSign(ReadBlockPtr);
 
  if(MPS_CurrentProc == DVM_IOProc)
     CopyBlockToMem(BufferPtr, ReadBlockPtr, DArr, Step);

  block_GetSize(tlong, ReadBlockPtr, Step)
  return  tlong;
}



DvmType   FillBlock(char  *BufferPtr, s_DISARRAY  *DArr,
                 s_BLOCK  *WriteBlockPtr)
{ s_BLOCK   Block, CurrBlock;
  char      Elm, *DArrElm;
  int       i, BlockEqu, TLen;
  DvmType      LocSize;
  DvmType      Index[MAXARRAYDIM + 1];
  byte      Step = 1;

  if(DArr->HasLocal)
  {  TLen = (int)DArr->TLen;
     Elm = BufferPtr[0];

     for(i=1; i < TLen; i++)
         if(BufferPtr[i] != Elm)
            break;

     Block = block_InitFromSpace(&DArr->Space);
     IsBlockEqu(BlockEqu, &Block, WriteBlockPtr)

     if(i == TLen && BlockEqu)
     {  /* Whole array is filled in with the same byte */    /*E0036*/

        DArrElm = (char *)DArr->ArrBlock.ALoc.Ptr0;
        LocSize = DArr->ArrBlock.ALoc.Size;

        SYSTEM(memset, (DArrElm, Elm, LocSize))
     }
     else
     {  /* Non whole array is filled in or  not with the same byte */    /*E0037*/

        if(block_Intersect(&Block, WriteBlockPtr, &DArr->Block,
                           WriteBlockPtr, 1))
        {  /* Local part of filled block is not empty */    /*E0038*/

           CurrBlock = block_Copy(&Block);

           while(spind_FromBlock(Index, &CurrBlock, &Block, 1))
                 PutLocElm(BufferPtr, DArr, &Index[1])
        }
     }
  } 

  block_GetSize(LocSize, WriteBlockPtr, Step)
  return   LocSize;
}



DvmType   IOPutBlock(char  *BufferPtr, s_DISARRAY  *DArr,
                  s_BLOCK  *WriteBlockPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, k, Proc;
  RTL_Request   *SendReq = NULL, RecvReq;
  int           *SendFlag = NULL;
  void         **SendBuf = NULL, *WriteBuf = NULL, *RecvBuf = NULL;
  s_BLOCK       *SendBlock = NULL, *wLocalBlock = NULL;
  s_BLOCK        WriteLocalBlock;
  DvmType           WriteBSize, SendBSize, RecvBSize;
  byte           Step;

  Step = GetStepSign(WriteBlockPtr);

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  dvm_AllocArray(void *, VMSize, SendBuf);
  dvm_AllocArray(s_BLOCK, VMSize, SendBlock);

  for(i=0; i < VMSize; i++)
      SendFlag[i]=0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0039*/

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
                                      &SendReq[i], s_IOMem_DA) );
        }
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     if(DArr->HasLocal && block_Intersect(&WriteLocalBlock,
                                          WriteBlockPtr, &DArr->Block,
                                          WriteBlockPtr, Step))
     { /* Local part of written block is not empty */    /*E0040*/

       block_GetSize(WriteBSize, &WriteLocalBlock, Step)
       WriteBSize *= DArr->TLen;
       mac_malloc(WriteBuf, void *, WriteBSize, 0);
       CopyMemToSubmem(WriteBuf, &WriteLocalBlock, BufferPtr,
                       WriteBlockPtr, DArr->TLen,Step);
       CopyMemToBlock(DArr, (char *)WriteBuf, &WriteLocalBlock, Step);
       mac_free(&WriteBuf);
     }


     for(i=0; i < VMSize; i++)
     {  if(SendFlag[i] == 0)
           continue;
        ( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
        mac_free((void **)&SendBuf[i]);
     }
  }
  else
  {  /* Current processor is not I/O processor */    /*E0041*/

     if(DArr->HasLocal && block_Intersect(&WriteLocalBlock,
                                          WriteBlockPtr, &DArr->Block,
                                          WriteBlockPtr, Step))
     {  /* Local part of written block is not empty */    /*E0042*/

        block_GetSize(RecvBSize, &WriteLocalBlock, Step)
        RecvBSize *= DArr->TLen;
        mac_malloc(RecvBuf, void *, RecvBSize, 0);
        ( RTL_CALL, rtl_Recvnowait(RecvBuf, 1, (int)RecvBSize,
                                   DVM_IOProc, DVM_VMS->tag_DACopy,
                                   &RecvReq, 1) );
        ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
        CopyMemToBlock(DArr, (char *)RecvBuf, &WriteLocalBlock, Step);
        mac_free(&RecvBuf);
     }
  }

  dvm_FreeArray(SendReq);
  dvm_FreeArray(SendFlag);
  dvm_FreeArray(SendBuf);
  dvm_FreeArray(SendBlock);

  block_GetSize(RecvBSize, WriteBlockPtr, Step)
  return  RecvBSize;
}



DvmType   IOPutBlockRepl(char  *BufferPtr, s_DISARRAY  *DArr,
                      s_BLOCK  *WriteBlockPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, Proc, BSize;
  DvmType           Res;
  RTL_Request   *SendReq = NULL, RecvReq;
  byte           Step;
  char          *_BufferPtr = NULL;

  Step = GetStepSign(WriteBlockPtr);
  
  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;
  block_GetSize(Res, WriteBlockPtr, Step)
  BSize = (int)( Res * (DArr->TLen) );

  dvm_AllocArray(RTL_Request, VMSize, SendReq);

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0043*/

     if((IsSynchr && UserSumFlag) || (BSize & Msk3))
     {  mac_malloc(_BufferPtr, char *, BSize, 0);
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, BSize))

        for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;
           if(Proc == MPS_CurrentProc)
              continue;
           ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, (int)BSize, Proc,
	                              DVM_VMS->tag_DACopy,
                                      &SendReq[i], s_IOMem_DARepl) );
        }
     }
     else
     {  for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;
           if(Proc == MPS_CurrentProc)
              continue;
           ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, (int)BSize, Proc,
	                              DVM_VMS->tag_DACopy,
                                      &SendReq[i], s_IOMem_DARepl) );
        }
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     CopyMemToBlock(DArr, BufferPtr, WriteBlockPtr, Step);

     for(i=0; i < VMSize; i++)
     {  Proc = (int)VMS->VProc[i].lP;
        if(Proc == MPS_CurrentProc)
           continue;
        ( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
     }

     mac_free(&_BufferPtr);
  }
  else
  {  /* Current processor is not I/O processor */    /*E0044*/

     if((IsSynchr && UserSumFlag) || (BSize & Msk3))
     {  mac_malloc(_BufferPtr, char *, BSize, 0);
        ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, BSize, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, &RecvReq, 1) );
        ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
        SYSTEM(memcpy, (BufferPtr, _BufferPtr, BSize))
        mac_free(&_BufferPtr);
     }
     else
     {  ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, BSize, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, &RecvReq, 1) );
        ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
     }

     CopyMemToBlock(DArr, BufferPtr, WriteBlockPtr, Step);
  }

  dvm_FreeArray(SendReq);

  return Res;
}



void   CopyBlock(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                 s_DISARRAY  *ToDArr,  s_BLOCK  *ToBlockPtr)
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
  char          **WriteElmPtr = NULL, *ReadElmPtr, *CharPtr1, *CharPtr2;
  byte            FromStep, ToStep, EquSign = 0, ExchangeScheme,
                  Alltoall = 0;
  byte           *IsVMSBlock;
  s_BLOCK       **VMSBlock;
  SysHandle      *VProc;
  int             Save_s_DA = s_DA_NE_DA;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *sdispls, *rdispls;
  int             r;
#endif

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

  /* */    /*E0045*/

#ifdef  _DVM_MPI_

  if(S_MPIAlltoall && ReadVMS == DVM_VMS && WriteVMS == DVM_VMS &&
     (FromDArr->Every && ToDArr->Every) && DVM_VMS->Is_MPI_COMM != 0)
     Alltoall = 1;

#endif

  /* */    /*E0046*/

  ExchangeScheme = (byte)(MsgExchangeScheme && ReadVMS == WriteVMS &&
                          ReadVMS->HasCurrent && Alltoall == 0);

  /* ---------------------------------------- */    /*E0047*/

  dvm_AllocArray(RTL_Request, ReadVMSize, ReadReq);
  dvm_AllocArray(void *, ReadVMSize, ReadBuf);
  dvm_AllocArray(int, ReadVMSize, IsReadInter);
  dvm_AllocArray(s_BLOCK, ReadVMSize, ReadLocalBlock);
  dvm_AllocArray(int, ReadVMSize, ReadBSize);

  /* Create array of blocks of From array local parts 
     for processor system the From array is mapped on */    /*E0048*/

  VMSBlock   = FromDArr->VMSBlock;
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

  /* ------------------------------------------------ */    /*E0049*/

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
     for processor system the To array is mapped on */    /*E0050*/

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

  /* ----------------------------------------------- */    /*E0051*/

  for(i=0; i < WriteVMSize; i++)
  {  WriteBSize[i] = 0;

     if(VMSBlock[i])
        IsWriteInter[i] = block_Intersect(&WriteLocalBlock[i],
                                          ToBlockPtr, VMSBlock[i],
                                          ToBlockPtr, ToStep);
     else
       IsWriteInter[i] = 0;
  }

  /* */    /*E0052*/

  /* */    /*E0053*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);
  ToOnlyAxis   = GetOnlyAxis(ToBlockPtr);

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0054*/
     /* */    /*E0055*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlock: Axis -> Axis; Recv.\n");

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0056*/

        CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = CurrWriteBlock->Set[i].Lower;
        index_GetLI(MyMinLinInd, ToBlockPtr, WriteIndex, ToStep)

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = CurrWriteBlock->Set[i].Upper;
        index_GetLI(MyMaxLinInd, ToBlockPtr, WriteIndex, ToStep)

        /* */    /*E0057*/

        for(i=0; i < ReadVMSize; i++)
        {  Proc = ReadVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsReadInter[i] == 0)
              continue; /* */    /*E0058*/

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
           {  /* */    /*E0059*/

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
    /* Check if read block and written one are the same */    /*E0060*/

    if(FromRank == ToRank)
    {  for(i=0; i < ToRank; i++)
           if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size ||
              FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step   )
              break;
       if(i == ToRank)
          EquSign = 1;
    }

    if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
       tprintf("*** CopyBlock: EquSign=%d\n", (int)EquSign);


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
    {  /* Written block is intersected with
        local part of current processor */    /*E0061*/

       CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];
       wWriteBlock = block_Copy(CurrWriteBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0062*/

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
             }*/    /*E0063*/

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
             {  /* */    /*E0064*/

                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0065*/

          block_GetSize(LinInd, CurrWriteBlock, ToStep)
          n = (int)LinInd; /* size of local part of written array */    /*E0066*/

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
             {  /* */    /*E0067*/

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

  /* */    /*E0068*/

  /* */    /*E0069*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0070*/
     /* */    /*E0071*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlock: Axis -> Axis; Send.\n");

     if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
     {  /* */    /*E0072*/

        CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = CurrReadBlock->Set[i].Lower;
        index_GetLI(MyMinLinInd, FromBlockPtr, ReadIndex, FromStep)

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = CurrReadBlock->Set[i].Upper;
        index_GetLI(MyMaxLinInd, FromBlockPtr, ReadIndex, FromStep)

        /* */    /*E0073*/

        for(i=0; i < WriteVMSize; i++)
        {  Proc = WriteVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsWriteInter[i] == 0)
              continue; /* */    /*E0074*/

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

           k = (int)(MaxLinInd - MinLinInd + 1); /* */    /*E0075*/
           n = ToDArr->TLen;
           m = k * n;
           WriteBSize[i] = m;   /* */    /*E0076*/

           mac_malloc(ReadElmPtr, char *, m, 0);

           WriteBuf[i] = (void *)ReadElmPtr; /* */    /*E0077*/

           for(j=0; j < FromRank; j++)
               ReadIndex[j] = FromBlockPtr->Set[j].Lower;

           ReadIndex[FromOnlyAxis] +=
           (MinLinInd * FromBlockPtr->Set[FromOnlyAxis].Step);
        
           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0078*/

           ReadIndex[FromOnlyAxis] +=
           FromBlockPtr->Set[FromOnlyAxis].Step;

           LocElmAddr(CharPtr2, FromDArr, ReadIndex) /* */    /*E0079*/
               j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0080*/
           for(p=0; p < k; p++)
           {  CharPtr2 = CharPtr1;

              for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                  *ReadElmPtr = *CharPtr2;

              CharPtr1 += j;
           }

           Save_s_DA = s_DA_Axis_DA;

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0081*/

              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         m, (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], s_DA_Axis_DA) );
           }
        } 

        if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
           Alltoall == 0)
        {  rtl_TstReqColl(0);
           rtl_SendReqColl(1.0);
        }
     }
  }
  else
  { if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
    {  /* Read block is intersected with
        local part of current processor */    /*E0082*/

       CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];
       wReadBlock = block_Copy(CurrReadBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0083*/

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
             }*/    /*E0084*/

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
             {  /* Step on main dimension is equal to 1 */    /*E0085*/

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
             {  /* Step on main dimension is not equal to 1 */    /*E0086*/

                k = (int)(WriteBSize[i] / FromDArr->TLen);

                for(j=0; j < k; j++)
                {  index_FromBlock(ReadIndex, &wWriteBlock,
                                   &FromLocalBlock, EquSign)
                   GetLocElm(FromDArr, ReadIndex, ReadElmPtr)
                   ReadElmPtr += FromDArr->TLen;
                }
             } 

             Save_s_DA = s_DA_EQ_DA;

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0087*/

                ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                           WriteBSize[i],
                                           (int)WriteVMS->VProc[i].lP,
                                           DVM_VMS->tag_DACopy,
                                           &WriteReq[i], s_DA_EQ_DA) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0088*/

          block_GetSize(LinInd, CurrReadBlock, FromStep)
          n = (int)LinInd; /* size of local part of read array */    /*E0089*/

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

          Save_s_DA = s_DA_NE_DA;

          if(ExchangeScheme == 0 && Alltoall == 0)
          {  /* */    /*E0090*/

             for(i=0; i < WriteVMSize; i++)
             { if(WriteBSize[i])
                  ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                             WriteBSize[i],
                                             (int)WriteVMS->VProc[i].lP,
                                             DVM_VMS->tag_DACopy,
                                             &WriteReq[i], s_DA_NE_DA) );
             }
          }
       }

       if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
          Alltoall == 0)
       {  rtl_TstReqColl(0);
          rtl_SendReqColl(1.0);
       }
    }
  }

  /* */    /*E0091*/

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

  /* */    /*E0092*/

#ifdef  _DVM_MPI_

  /* */    /*E0093*/

  if(Alltoall)
  {
     if((MsgSchedule || MsgPartReg) && AlltoallWithMsgSchedule &&
        MaxMsgLength > 0)
     {
        r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0094*/

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
        {  /* */    /*E0095*/

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
                                            &WriteReq[i], Save_s_DA) );
           }

           if(MsgSchedule && UserSumFlag)
           {  /* */    /*E0096*/

              rtl_TstReqColl(0);
              rtl_SendReqColl(1.0);
           }
        }
     }
  }

  if(Alltoall)
  {
     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0097*/

     /* */    /*E0098*/

     Proc = DVMTYPE_MAX;    /*E0099*/
     j = -1;

     for(i=0; i < WriteVMSize; i++)
     {  if(WriteBSize[i] == 0)
           continue;  /* */    /*E0100*/

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

     /* */    /*E0101*/

     MinLinInd = DVMTYPE_MAX;   /*E0102*/
     k = -1;

     for(i=0; i < ReadVMSize; i++)
     {  if(ReadBSize[i] == 0)
           continue;  /* */    /*E0103*/

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

     /* */    /*E0104*/

     dvm_AllocArray(int, WriteVMSize, sdispls);
     dvm_AllocArray(int, ReadVMSize,  rdispls);

     r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0105*/

     for(m=0; m < WriteVMSize; m++)
     {  if(WriteBSize[m] == 0)
           sdispls[m] = 0;
        else
        {
            LinInd = (DvmType)WriteBuf[m] - Proc;

           if(LinInd >= r)
              break;    /* */    /*E0106*/
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
              break;    /* */    /*E0107*/
           rdispls[n] = (int)LinInd;
        }
     }

     if(m < WriteVMSize)
     {  /* */    /*E0108*/

        for(i=0,p=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              sdispls[i] = 0;
           else
           {  sdispls[i] = p;
              p += WriteBSize[i];
           }
        }

        mac_malloc(sendbuf, void *, p, 0);

        /* */    /*E0109*/

        CharPtr1 = (char *)sendbuf;

        for(i=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

          SYSTEM(memcpy, (CharPtr1, (char *)WriteBuf[i], WriteBSize[i]))
          CharPtr1 += WriteBSize[i];
        }
     }

     if(n < ReadVMSize)
     {  /* */    /*E0110*/

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

     /* */    /*E0111*/

     if(RTL_TRACE && MPI_AlltoallTrace && TstTraceEvent(call_arrcpy_))
     {  if(m < WriteVMSize || n < ReadVMSize)
        {  if(m < WriteVMSize && n < ReadVMSize)
              tprintf("*** CopyBlock: AllToAll-branch\n");
           else
           {  if(m < WriteVMSize)
                 tprintf("*** CopyBlock: AllToAll-branch "
                         "(recv_fast)\n");
              else
                 tprintf("*** CopyBlock: AllToAll-branch "
                         "(send_fast)\n");
           } 
        }
        else
           tprintf("*** CopyBlock: AllToAll-branch (super_fast)\n");
     }

     /* */    /*E0112*/

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
     {  /* */    /*E0113*/

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

     DVMMTimeFinish;    /* */    /*E0114*/
  }

#endif

  /* --------------------------------------------- */    /*E0115*/

  for(i=0; i < WriteVMSize; i++)
  {  if(WriteBSize[i] == 0)
        continue;
     if(ExchangeScheme == 0 && Alltoall == 0)
        ( RTL_CALL, rtl_Waitrequest(&WriteReq[i]) );
     mac_free(&WriteBuf[i]);
  }

  /* --------------------------------------------- */    /*E0116*/

  /* */    /*E0117*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0118*/
     /* */    /*E0119*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlock: Axis -> Axis; Wait Recv.\n");

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0120*/

        CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = CurrWriteBlock->Set[i].Lower;
        index_GetLI(MyMinLinInd, ToBlockPtr, WriteIndex, ToStep)

        n = ToDArr->TLen;

        /* */    /*E0121*/

        for(i=0; i < ReadVMSize; i++)
        {  if(ReadBSize[i] == 0)
              continue;
           if(ExchangeScheme == 0 && Alltoall == 0)
              (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));

           ReadElmPtr = (char *)ReadBuf[i]; /* */    /*E0122*/
           for(j=0; j < FromRank; j++)
               ReadIndex[j] = ReadLocalBlock[i].Set[j].Lower;
           index_GetLI(LinInd, FromBlockPtr, ReadIndex, FromStep)

           MinLinInd = dvm_max(LinInd, MyMinLinInd);
           
           k = ReadBSize[i]/n;  /* */    /*E0123*/

           WriteIndex[ToOnlyAxis] = ToBlockPtr->Set[ToOnlyAxis].Lower +
           (MinLinInd * ToBlockPtr->Set[ToOnlyAxis].Step);
        
           LocElmAddr(CharPtr1, ToDArr, WriteIndex) /* */    /*E0124*/
           WriteIndex[ToOnlyAxis] += ToBlockPtr->Set[ToOnlyAxis].Step;

           LocElmAddr(CharPtr2, ToDArr, WriteIndex) /* */    /*E0125*/
               j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0126*/
           for(p=0; p < k; p++)
           {  CharPtr2 = CharPtr1;

              for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                  *CharPtr2 = *ReadElmPtr;

              CharPtr1 += j;
           }

           mac_free(&ReadBuf[i]);
        } 
     }
  }
  else
  { for(i=0; i < ReadVMSize; i++)
    {  if(ReadBSize[i] == 0)
          continue;
       if(ExchangeScheme == 0 && Alltoall == 0)
          (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));

       ReadElmPtr = (char *)ReadBuf[i];
   
       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0127*/

          CurrWriteBlock = &ToLocalBlock[i];

          wWriteBlock = block_Copy(CurrWriteBlock);

          if(CurrWriteBlock->Set[ToRank-1].Step == 1)
          {  /* Step on main dimension is equal to 1 */    /*E0128*/

             LinInd = CurrWriteBlock->Set[ToRank-1].Size;
             n = (int)(LinInd * ToDArr->TLen);
             k = (int)(ReadBSize[i] / n);

             for(j=0; j < k; j++)
             {  index_FromBlock1S(WriteIndex, &wWriteBlock, CurrWriteBlock)
                PutLocElm1(ReadElmPtr, ToDArr, WriteIndex, LinInd)
                ReadElmPtr += n;
             }
          }
          else
          {  /* Step on main dimension is not equal to 1 */    /*E0129*/

             k = (int)(ReadBSize[i] / ToDArr->TLen);

             for(j=0; j < k; j++)
             {  index_FromBlock(WriteIndex, &wWriteBlock, CurrWriteBlock,
                                EquSign)
                PutLocElm(ReadElmPtr, ToDArr, WriteIndex)
                ReadElmPtr += ToDArr->TLen;
             }
          } 
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0130*/

          wWriteBlock = block_Copy(CurrWriteBlock);

          block_GetSize(LinInd, CurrWriteBlock, ToStep)
          n = (int)LinInd; /* size of local part of written array */    /*E0131*/

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

             IsElmOfBlock(k, &ReadLocalBlock[i], ReadIndex)
             if(k == 0)
                continue;

             PutLocElm(ReadElmPtr, ToDArr, WriteIndex)
             ReadElmPtr += ToDArr->TLen;
          }
       }

       mac_free(&ReadBuf[i]);
    }
  }

  dvm_FreeArray(ReadReq); 
  dvm_FreeArray(ReadBuf); 
  dvm_FreeArray(IsReadInter); 
  dvm_FreeArray(ReadLocalBlock); 
  dvm_FreeArray(ReadBSize); 

  dvm_FreeArray(WriteReq); 
  dvm_FreeArray(WriteBuf); 
  dvm_FreeArray(IsWriteInter); 
  dvm_FreeArray(WriteLocalBlock); 
  dvm_FreeArray(WriteBSize);

  dvm_FreeArray(WriteElmPtr);
  dvm_FreeArray(WriteVMInReadVM);

  dvm_FreeArray(ToLocalBlock); 

  return;
}



void   CopyBlock1(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                  s_DISARRAY  *ToDArr,  s_BLOCK  *ToBlockPtr)
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
  char          **WriteElmPtr = NULL, *ReadElmPtr, *CharPtr1, *CharPtr2;
  byte            Step = 0, EquSign = 0, ExchangeScheme,
                  ToSuperFast = 0, FromSuperFast = 0, Alltoall = 0;
  byte           *IsVMSBlock;
  s_BLOCK       **VMSBlock;
  SysHandle      *VProc;

  int             CGSign = 0;
  int             FromVMAxis = 0, ToVMAxis = 0;
  byte           *FromVM = NULL, *ToVM = NULL;
  int             Save_s_DA = s_DA1_NE_DA1;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *sdispls, *rdispls;
#endif


  ReadAMS    = FromDArr->AMView;
  ReadVMS    = ReadAMS->VMS;
  ReadVMSize = ReadVMS->ProcCount;

  WriteAMS    = ToDArr->AMView;
  WriteVMS    = WriteAMS->VMS;
  WriteVMSize = WriteVMS->ProcCount;

  ToRank   = ToBlockPtr->Rank;
  FromRank = FromBlockPtr->Rank;

  dvm_AllocArray(byte, ReadVMSize, FromVM);
  dvm_AllocArray(byte, WriteVMSize, ToVM);

  /* */    /*E0132*/

#ifdef  _DVM_MPI_

  if(S_MPIAlltoall && ReadVMS == DVM_VMS && WriteVMS == DVM_VMS &&
     (FromDArr->Every && ToDArr->Every) && DVM_VMS->Is_MPI_COMM != 0)
     Alltoall = 1;

#endif

  /* */    /*E0133*/

  if(ReadVMS == WriteVMS && ReadVMS->Space.Rank == 2)
  {  /* */    /*E0134*/

     if(FromRank == 1 && ToRank == 1 &&
        (FromDArr->HasLocal || ToDArr->HasLocal))
     {  /* */    /*E0135*/

        if(FromBlockPtr->Set[0].Lower == 0 &&
           ToBlockPtr->Set[0].Lower == 0)
        {  /* */    /*E0136*/

           if(FromDArr->DAAxis[0] != 0)
              FromVMAxis = 1;

           if(FromDArr->DAAxis[1] != 0)
              FromVMAxis = 2;

           if(ToDArr->DAAxis[0] != 0)
              ToVMAxis = 1;

           if(ToDArr->DAAxis[1] != 0)
             ToVMAxis = 2;

           if(FromVMAxis != 0 && ToVMAxis != 0)
           {  /* */    /*E0137*/

              if(FromVMAxis != ToVMAxis)
              {  /* */    /*E0138*/

                 CGSign = 1; /* */    /*E0139*/
              }
              else
              {  /* */    /*E0140*/

                 CGSign = -1; /* */    /*E0141*/
              }

              for(i=0; i < ReadVMSize; i++)
              {  FromVM[i] = 0;
                 ToVM[i] = 0;
              }

              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_arrcpy_))
                 tprintf("*** CopyBlock1: cg-branch; CGSign=%d\n",
                         CGSign);
           }
        }
     }
  }

  /* */    /*E0142*/

  if(CGSign && CG_MPIAlltoall == 0)
     Alltoall = 0;

  /* */    /*E0143*/

  ExchangeScheme = (byte)(MsgExchangeScheme && ReadVMS == WriteVMS &&
                          ReadVMS->HasCurrent && CGSign == 0 &&
                          Alltoall == 0);

  /* ---------------------------------------- */    /*E0144*/

  dvm_AllocArray(RTL_Request, ReadVMSize, ReadReq);
  dvm_AllocArray(void *, ReadVMSize, ReadBuf);
  dvm_AllocArray(int, ReadVMSize, IsReadInter);
  dvm_AllocArray(s_BLOCK, ReadVMSize, ReadLocalBlock);
  dvm_AllocArray(int, ReadVMSize, ReadBSize);

  /* Create array of blocks of From array local parts 
     for processor system the From array is mapped on */    /*E0145*/

  VMSBlock   = FromDArr->VMSBlock;
  IsVMSBlock = FromDArr->IsVMSBlock; 

  if(CGSign != 0)
  {  /* */    /*E0146*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(FromVMAxis == 2)
     {  /* */    /*E0147*/

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
     else
     {  /* */    /*E0148*/

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

  /* ----------------------------------------------- */    /*E0149*/

  dvm_AllocArray(RTL_Request, WriteVMSize, WriteReq);
  dvm_AllocArray(void *, WriteVMSize, WriteBuf);
  dvm_AllocArray(int, WriteVMSize, IsWriteInter);
  dvm_AllocArray(s_BLOCK, WriteVMSize, WriteLocalBlock);
  dvm_AllocArray(int, WriteVMSize, WriteBSize);

  /* Create array of blocks of To array local parts 
     for processor system the To array is mapped on */    /*E0150*/

  VMSBlock   = ToDArr->VMSBlock;
  IsVMSBlock = ToDArr->IsVMSBlock; 

  if(CGSign != 0)
  {  /* */    /*E0151*/

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(ToVMAxis == 2)
     {  /* */    /*E0152*/

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
     {  /* */    /*E0153*/

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

  /* ------------------------------------------------ */    /*E0154*/

  if(CGSign != 0)
  {  /* */    /*E0155*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(ToDArr->HasLocal)
     {  /* */    /*E0156*/

        VMSBlock   = FromDArr->VMSBlock;

        m = ToDArr->Block.Set[0].Lower;
        n = ToDArr->Block.Set[0].Upper;

        if(FromVMAxis == 2)
        {  /* */    /*E0157*/

           for(i=0; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = i;    /* */    /*E0158*/
                 break;
              }
           }

           for( ; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = i;    /* */    /*E0159*/
                 break;
              }
           }

           if(CGSign > 0)
              r = (int)(ReadVMS->CVP[2] % j) * k;
           else
              r = (int)ReadVMS->CVP[1] * k;

           m = q+r;

           for(i=p+r; i <= m; i++)  /* */    /*E0160*/
               FromVM[i] = 1;
        }
        else
        {  /* */    /*E0161*/

           for(r=0, i=0; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = r;    /* */    /*E0162*/
                 break;
              }
           }

           for( ; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = r;    /* */    /*E0163*/
                 break;
              } 
           }

           if(CGSign > 0) 
              r = (int)(ReadVMS->CVP[1] % k);
           else
              r = (int)ReadVMS->CVP[2];

           m = q*k + r;

           for(i=p*k+r; i <= m; i+=k)  /* */    /*E0164*/
               FromVM[i] = 1;
        }
     }

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(FromDArr->HasLocal)
     {  /* */    /*E0165*/

        VMSBlock   = ToDArr->VMSBlock;

        m = FromDArr->Block.Set[0].Lower;
        n = FromDArr->Block.Set[0].Upper;

        if(ToVMAxis == 2)
        {  /* */    /*E0166*/

           for(i=0; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = i;    /* */    /*E0167*/
                 break;
              }
           }

           for( ; i < k; i++)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = i;    /* */    /*E0168*/
                 break;
              }
           }

           if(CGSign > 0)
           {  r = (int)ceil((double)j/(double)k); /* */    /*E0169*/
              m = (int)WriteVMS->CVP[2] + k*r;

              for( ; p <= q; p++)
              {  for(n=(int)WriteVMS->CVP[2]; n < m; n += k)
                 {  if(n >= j)
                       break;

                    i = p + n*k;  /* */    /*E0170*/
                    ToVM[i] = 1;
                 }
              }
           }
           else
           {  r = (int)ReadVMS->CVP[1] * k;
              m = q + r;
               
              for(i=p+r ; i <= m; i++)  /* */    /*E0171*/
                  ToVM[i] = 1;
           }
        }
        else
        {  /* */    /*E0172*/

           for(r=0, i=0; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(m >= VMSBlock[i]->Set[0].Lower &&
                 m <= VMSBlock[i]->Set[0].Upper)
              {  p = r;    /* */    /*E0173*/
                 break;
              }
           }

           for( ; r < j; r++, i+=k)
           {  if(VMSBlock[i] == NULL)
                 continue;

              if(n >= VMSBlock[i]->Set[0].Lower &&
                 n <= VMSBlock[i]->Set[0].Upper)
              {  q = r;    /* */    /*E0174*/
                 break;
              }
           }

           if(CGSign > 0)
           {  r = (int)ceil((double)k/(double)j); /* */    /*E0175*/
              m = (int)WriteVMS->CVP[1] + j*r;

              for( ; p <= q; p++)
              {  r = p*k;

                 for(n=(int)WriteVMS->CVP[1]; n < m; n += j)
                 {  if(n >= k)
                       break;

                    i = r + n;    /* */    /*E0176*/
                    ToVM[i] = 1;
                 }
              }
           }
           else
           {  r = (int)ReadVMS->CVP[2];
              m = q*k + r;

              for(i=p*k+r; i <= m; i+=k) /* */    /*E0177*/
                  ToVM[i] = 1;
           }
        }
     }
  }

  /* */    /*E0178*/

  VMSBlock   = FromDArr->VMSBlock;

  if(CGSign != 0)
  {  /* */    /*E0179*/

     j = (int)ReadVMS->Space.Size[0];
     k = (int)ReadVMS->Space.Size[1];

     if(FromVMAxis == 2)
     {  /* */    /*E0180*/

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
     {  /* */    /*E0181*/

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

  /* */    /*E0182*/

  VMSBlock   = ToDArr->VMSBlock;

  if(CGSign != 0)
  {  /* */    /*E0183*/

     j = (int)WriteVMS->Space.Size[0];
     k = (int)WriteVMS->Space.Size[1];

     if(ToVMAxis == 2)
     {  /* */    /*E0184*/

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
     {  /* */    /*E0185*/

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

  /* */    /*E0186*/

  /* */    /*E0187*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);
  ToOnlyAxis   = GetOnlyAxis(ToBlockPtr);

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0188*/
     /* */    /*E0189*/

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlock1: Axis -> Axis; Recv.\n");

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0190*/

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

        /* */    /*E0191*/

        for(i=0; i < ReadVMSize; i++)
        {  if(FromVM[i] == 0)
              continue;

           Proc = ReadVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsReadInter[i] == 0)
              continue; /* */    /*E0192*/

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
           {  /* */    /*E0193*/

              mac_malloc(ReadBuf[i], void *, ReadBSize[i], 0);
           }
           else
           {  /* */    /*E0194*/

              WriteIndex[ToOnlyAxis] =
              ToBlockPtr->Set[ToOnlyAxis].Lower + MinLinInd;
        
              LocElmAddr(CharPtr1, ToDArr, WriteIndex) /* */    /*E0195*/
              ReadBuf[i] = (void *)CharPtr1;
           }

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0196*/

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
    /* Check if read block and written one are the same */    /*E0197*/

    if(FromRank == ToRank)
    {  for(i=0; i < ToRank; i++)
           if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
       if(i == ToRank)
          EquSign = 1;
    }

    if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
       tprintf("*** CopyBlock1: EquSign=%d\n", (int)EquSign);


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
    {  /* Written block is intersected with
        local part of current processor */    /*E0198*/

       CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];
       wWriteBlock = block_Copy(CurrWriteBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0199*/

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
             }*/    /*E0200*/

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
             {  /* */    /*E0201*/

                ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1,
                                           ReadBSize[i], (int)Proc,
                                           DVM_VMS->tag_DACopy,
                                           &ReadReq[i], 1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0202*/

          block_GetSize(LinInd, CurrWriteBlock, Step)
          n = (int)LinInd; /* size of local part of written array */    /*E0203*/

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
             {  /* */    /*E0204*/

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

  /* */    /*E0205*/

  /* */    /*E0206*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0207*/
     /* */    /*E0208*/

     if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
     {  /* */    /*E0209*/

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

        /* */    /*E0210*/

        for(i=0; i < WriteVMSize; i++)
        {  if(ToVM[i] == 0)
              continue;

           Proc = WriteVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;
           if(IsWriteInter[i] == 0)
              continue; /* */    /*E0211*/

           LinInd = WriteLocalBlock[i].Set[ToOnlyAxis].Lower -
                    ToBlockPtr->Set[ToOnlyAxis].Lower;

           MinLinInd = dvm_max(LinInd, MyMinLinInd);

           LinInd = WriteLocalBlock[i].Set[ToOnlyAxis].Upper -
                    ToBlockPtr->Set[ToOnlyAxis].Lower;

           MaxLinInd = dvm_min(LinInd, MyMaxLinInd);

           if(MinLinInd > MaxLinInd)
              continue;

           k = (int)(MaxLinInd - MinLinInd + 1); /* */    /*E0212*/
           m = k * n;
           WriteBSize[i] = m;    /* */    /*E0213*/

           ReadIndex[FromOnlyAxis] =
           FromBlockPtr->Set[FromOnlyAxis].Lower + MinLinInd;
        
           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0214*/

           if(FromSuperFast)
           {  /* */    /*E0215*/

              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_arrcpy_))
                 tprintf("*** CopyBlock1: Axis -> Axis; "
                         "Send (super fast)\n");

              WriteBuf[i] = (void *)CharPtr1;
           }
           else
           { mac_malloc(ReadElmPtr, char *, m, 0); /* */    /*E0216*/

             WriteBuf[i] = (void *)ReadElmPtr; /* */    /*E0217*/

             if(FromOnlyAxis == (FromRank-1))
             {  if(RTL_TRACE && dacopy_Trace &&
                   TstTraceEvent(call_arrcpy_))
                   tprintf("*** CopyBlock1: Axis -> Axis; "
                           "Send (fast).\n");

                SYSTEM(memcpy, (ReadElmPtr, CharPtr1, m))
             }
             else
             {  if(RTL_TRACE && dacopy_Trace &&
                   TstTraceEvent(call_arrcpy_))
                   tprintf("*** CopyBlock1: Axis -> Axis; Send.\n");

                ReadIndex[FromOnlyAxis]++;

                LocElmAddr(CharPtr2, FromDArr,
                           ReadIndex) /* */    /*E0218*/
                           j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0219*/

                for(p=0; p < k; p++)
                {  CharPtr2 = CharPtr1;

                   for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                       *ReadElmPtr = *CharPtr2;

                   CharPtr1 += j;
                }
             }
           }

           Save_s_DA = s_DA1_Axis_DA1;

           if(ExchangeScheme == 0 && Alltoall == 0)
           {  /* */    /*E0220*/

              ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                         m, (int)Proc,
                                         DVM_VMS->tag_DACopy,
                                         &WriteReq[i], s_DA1_Axis_DA1) );
           }
        } 

        if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
           Alltoall == 0)
        {  rtl_TstReqColl(0);
           rtl_SendReqColl(1.0);
        }
     }
  }
  else
  { if(ReadVMS->HasCurrent && IsReadInter[ReadVMS->CurrentProc])
    {  /* Read block is intersected with
        local part of current processor */    /*E0221*/

       CurrReadBlock = &ReadLocalBlock[ReadVMS->CurrentProc];
       wReadBlock = block_Copy(CurrReadBlock);

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0222*/

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
             }*/    /*E0223*/

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

             Save_s_DA = s_DA1_EQ_DA1;

             if(ExchangeScheme == 0 && Alltoall == 0)
             {  /* */    /*E0224*/

                ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                           WriteBSize[i],
                                           (int)WriteVMS->VProc[i].lP,
                                           DVM_VMS->tag_DACopy,
                                           &WriteReq[i], s_DA1_EQ_DA1) );
             }
          }
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0225*/

          block_GetSize(LinInd, CurrReadBlock, Step)
          n = (int)LinInd; /* size of local part of read array */    /*E0226*/

          for(j=0; j < n; j++)
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock,
                             Step)
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
          {  index_FromBlock(ReadIndex, &wReadBlock, CurrReadBlock,
                             Step)
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

          Save_s_DA = s_DA1_NE_DA1;

          if(ExchangeScheme == 0 && Alltoall == 0)
          {  /* */    /*E0227*/

             for(i=0; i < WriteVMSize; i++)
             { if(WriteBSize[i])
                  ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1,
                                             WriteBSize[i],
                                             (int)WriteVMS->VProc[i].lP,
                                             DVM_VMS->tag_DACopy,
                                             &WriteReq[i],
                                             s_DA1_NE_DA1) );
             }
          }
       }

       if(MsgSchedule && UserSumFlag && ExchangeScheme == 0 &&
          Alltoall == 0)
       {  rtl_TstReqColl(0);
          rtl_SendReqColl(1.0);
       }
    }
  }

  /* */    /*E0228*/

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

  /* */    /*E0229*/

#ifdef  _DVM_MPI_

  /* */    /*E0230*/

  if(Alltoall)
  {
     if((MsgSchedule || MsgPartReg) && AlltoallWithMsgSchedule &&
        MaxMsgLength > 0)
     {
        r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0231*/

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
        {  /* */    /*E0232*/

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
                                            &WriteReq[i], Save_s_DA) );
           }

           if(MsgSchedule && UserSumFlag)
           {  /* */    /*E0233*/

              rtl_TstReqColl(0);
              rtl_SendReqColl(1.0);
           }
        }
     }
  }

  if(Alltoall)
  {
     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0234*/

     /* */    /*E0235*/

     Proc = DVMTYPE_MAX;    /*E0236*/
     j = -1;

     for(i=0; i < WriteVMSize; i++)
     {  if(WriteBSize[i] == 0)
           continue;  /* */    /*E0237*/

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

     /* */    /*E0238*/

     MinLinInd = DVMTYPE_MAX;    /*E0239*/
     k = -1;

     for(i=0; i < ReadVMSize; i++)
     {  if(ReadBSize[i] == 0)
           continue;  /* */    /*E0240*/

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

     /* */    /*E0241*/

     dvm_AllocArray(int, WriteVMSize, sdispls);
     dvm_AllocArray(int, ReadVMSize,  rdispls);

     r = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0242*/

     for(m=0; m < WriteVMSize; m++)
     {  if(WriteBSize[m] == 0)
           sdispls[m] = 0;
        else
        {
            LinInd = (DvmType)WriteBuf[m] - Proc;

           if(LinInd >= r)
              break;    /* */    /*E0243*/
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
              break;    /* */    /*E0244*/
           rdispls[n] = (int)LinInd;
        }
     }

     if(m < WriteVMSize)
     {  /* */    /*E0245*/

        for(i=0,p=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              sdispls[i] = 0;
           else
           {  sdispls[i] = p;
              p += WriteBSize[i];
           }
        }

        mac_malloc(sendbuf, void *, p, 0);

        /* */    /*E0246*/

        CharPtr1 = (char *)sendbuf;

        for(i=0; i < WriteVMSize; i++)
        {  if(WriteBSize[i] == 0)
              continue;

          SYSTEM(memcpy, (CharPtr1, (char *)WriteBuf[i], WriteBSize[i]))
          CharPtr1 += WriteBSize[i];
        }
     }

     if(n < ReadVMSize)
     {  /* */    /*E0247*/

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

     /* */    /*E0248*/

     if(RTL_TRACE && MPI_AlltoallTrace && TstTraceEvent(call_arrcpy_))
     {  if(m < WriteVMSize || n < ReadVMSize)
        {  if(m < WriteVMSize && n < ReadVMSize)
              tprintf("*** CopyBlock1: AllToAll-branch\n");
           else
           {  if(m < WriteVMSize)
                 tprintf("*** CopyBlock1: AllToAll-branch "
                         "(recv_fast)\n");
              else
                 tprintf("*** CopyBlock1: AllToAll-branch "
                         "(send_fast)\n");
           } 
        }
        else
           tprintf("*** CopyBlock1: AllToAll-branch (super_fast)\n");
     }

     /* */    /*E0249*/

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
     {  /* */    /*E0250*/

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

     DVMMTimeFinish;    /* */    /*E0251*/
  }

#endif

  /* --------------------------------------------- */    /*E0252*/

  for(i=0; i < WriteVMSize; i++)
  {  if(ToVM[i] == 0)
        continue;

     if(WriteBSize[i] == 0)
        continue;

     if(ExchangeScheme == 0 && Alltoall == 0)
        ( RTL_CALL, rtl_Waitrequest(&WriteReq[i]) );

     if(FromSuperFast)
        continue;  /* */    /*E0253*/

     mac_free(&WriteBuf[i]);
  }

  /* --------------------------------------------- */    /*E0254*/

  /* */    /*E0255*/

  if(FromOnlyAxis >= 0 && ToOnlyAxis >= 0)
  {  /* */    /*E0256*/
     /* */    /*E0257*/

     if(WriteVMS->HasCurrent && IsWriteInter[WriteVMS->CurrentProc])
     {  /* */    /*E0258*/

        CurrWriteBlock = &WriteLocalBlock[WriteVMS->CurrentProc];

        for(j=0; j < ToRank; j++)
            WriteIndex[j] = CurrWriteBlock->Set[j].Lower;

        MyMinLinInd = WriteIndex[ToOnlyAxis] -
                      ToBlockPtr->Set[ToOnlyAxis].Lower;

        n = ToDArr->TLen;

        /* */    /*E0259*/

        for(i=0; i < ReadVMSize; i++)
        {  if(FromVM[i] == 0)
              continue;

           if(ReadBSize[i] == 0)
              continue;

           if(ExchangeScheme == 0 && Alltoall == 0)
              (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));

           if(ToSuperFast)
              continue;  /* */    /*E0260*/

           ReadElmPtr = (char *)ReadBuf[i]; /* */    /*E0261*/

           LinInd = ReadLocalBlock[i].Set[FromOnlyAxis].Lower -
                    FromBlockPtr->Set[FromOnlyAxis].Lower;

           MinLinInd = dvm_max(LinInd, MyMinLinInd);
           
           WriteIndex[ToOnlyAxis] =
           ToBlockPtr->Set[ToOnlyAxis].Lower + MinLinInd;
        
           LocElmAddr(CharPtr1, ToDArr, WriteIndex) /* */    /*E0262*/
           m = ReadBSize[i]; /* */    /*E0263*/

           if(ToOnlyAxis == (ToRank-1))
           { if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
                tprintf("*** CopyBlock1: Axis -> Axis; "
                        "Wait Recv (fast).\n");

             SYSTEM(memcpy, (CharPtr1, ReadElmPtr, m))
           }
           else
           { if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
                tprintf("*** CopyBlock1: Axis -> Axis; Wait Recv.\n");

             WriteIndex[ToOnlyAxis]++;

             LocElmAddr(CharPtr2, ToDArr, WriteIndex) /* */    /*E0264*/
             k = m/n;               /* */    /*E0265*/
             j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0266*/
             for(p=0; p < k; p++)
             {  CharPtr2 = CharPtr1;

                for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                    *CharPtr2 = *ReadElmPtr;

                CharPtr1 += j;
             }
           } 

           mac_free(&ReadBuf[i]);
        } 
     }
  }
  else
  { for(i=0; i < ReadVMSize; i++)
    {  if(ReadBSize[i] == 0)
          continue;

       if(ExchangeScheme == 0 && Alltoall == 0) 
          (RTL_CALL, rtl_Waitrequest(&ReadReq[i]));

       ReadElmPtr = (char *)ReadBuf[i];

       if(EquSign)
       {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0267*/

          CurrWriteBlock = &ToLocalBlock[i];

          wWriteBlock = block_Copy(CurrWriteBlock);

          LinInd = CurrWriteBlock->Set[ToRank-1].Size;
          n = (int)(LinInd * ToDArr->TLen);
          k = (int)(ReadBSize[i] / n);

          for(j=0; j < k; j++)
          {  index_FromBlock1(WriteIndex, &wWriteBlock, CurrWriteBlock)
             PutLocElm1(ReadElmPtr, ToDArr, WriteIndex, LinInd)
             ReadElmPtr += n;
          } 
       }
       else
       {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0268*/

          wWriteBlock = block_Copy(CurrWriteBlock);

          block_GetSize(LinInd, CurrWriteBlock, Step)
          n = (int)LinInd; /* size of local part of written array */    /*E0269*/

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

             IsElmOfBlock(k, &ReadLocalBlock[i], ReadIndex)
             if(k == 0)
                continue;

             PutLocElm(ReadElmPtr, ToDArr, WriteIndex)
             ReadElmPtr += ToDArr->TLen;
          }
       }

       mac_free(&ReadBuf[i]);
    }
  }

  dvm_FreeArray(ReadReq); 
  dvm_FreeArray(ReadBuf); 
  dvm_FreeArray(IsReadInter); 
  dvm_FreeArray(ReadLocalBlock); 
  dvm_FreeArray(ReadBSize); 

  dvm_FreeArray(WriteReq); 
  dvm_FreeArray(WriteBuf); 
  dvm_FreeArray(IsWriteInter); 
  dvm_FreeArray(WriteLocalBlock); 
  dvm_FreeArray(WriteBSize);

  dvm_FreeArray(WriteElmPtr);
  dvm_FreeArray(WriteVMInReadVM);

  dvm_FreeArray(ToLocalBlock); 

  dvm_FreeArray(FromVM); 
  dvm_FreeArray(ToVM); 

  return;
}



void   CopyBlockRepl(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                     s_DISARRAY  *ToDArr, s_BLOCK  *ToBlockPtr)
{
    DvmType       LinInd, MaxLinInd;
    DvmType       Weight[MAXARRAYDIM];
  s_BLOCK    WriteLocalBlock, wWriteBlock, ReadLocalBlock, wReadBlock;
  DvmType       ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  byte       FromStep, ToStep, EquSign = 0;
  int        i, Rank, Size, FromOnlyAxis, ToOnlyAxis,
             MemToStep, MemFromStep;
  char      *FromAddr, *ToAddr, *CharPtr1, *CharPtr2;

  FromStep = GetStepSign(FromBlockPtr);
  ToStep   = GetStepSign(ToBlockPtr);

  if(ToDArr->HasLocal == 0 || FromDArr->HasLocal == 0 ||
     block_Intersect(&WriteLocalBlock, ToBlockPtr, &ToDArr->Block,
                     ToBlockPtr, ToStep) == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlockRepl: branch 0\n");

     return;
  }

  /* Written block is intersected with
        local part of current processor */    /*E0270*/

  Rank = ToBlockPtr->Rank;

  /* */    /*E0271*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);

  if(FromOnlyAxis >= 0)
  {  /* */    /*E0272*/

     ToOnlyAxis = GetOnlyAxis(ToBlockPtr);

     if(ToOnlyAxis >= 0)
     {  /* */    /*E0273*/
        /* */    /*E0274*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl: branch Axis -> Axis\n");

        /* */    /*E0275*/

        for(i=0; i < Rank; i++)
            WriteIndex[i] = WriteLocalBlock.Set[i].Lower;
        index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)

        for(i=0; i < Rank; i++)
            WriteIndex[i] = WriteLocalBlock.Set[i].Upper;
        index_GetLI(MaxLinInd, ToBlockPtr, WriteIndex, ToStep)

        /* */    /*E0276*/

        for(i=0; i < Rank; i++)
            WriteIndex[i] = ToBlockPtr->Set[i].Lower;

        WriteIndex[ToOnlyAxis] +=
        (LinInd * ToBlockPtr->Set[ToOnlyAxis].Step);
        
        LocElmAddr(ToAddr, ToDArr, WriteIndex) /* */    /*E0277*/

        WriteIndex[ToOnlyAxis] += ToBlockPtr->Set[ToOnlyAxis].Step;

        LocElmAddr(CharPtr1, ToDArr, WriteIndex)/* */    /*E0278*/

        MemToStep = (int)((DvmType)CharPtr1 - (DvmType)ToAddr); /* */    /*E0279*/

        Rank = FromBlockPtr->Rank;

        for(i=0; i < Rank; i++)
            ReadIndex[i] = FromBlockPtr->Set[i].Lower;

        ReadIndex[FromOnlyAxis] +=
        (LinInd * FromBlockPtr->Set[FromOnlyAxis].Step);
        
        LocElmAddr(FromAddr, FromDArr, ReadIndex) /* */    /*E0280*/

        ReadIndex[FromOnlyAxis] += FromBlockPtr->Set[FromOnlyAxis].Step;

        LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0281*/
        MemFromStep = (int)((DvmType)CharPtr1 - (DvmType)FromAddr); /* */    /*E0282*/

        Size = (int)(MaxLinInd - LinInd + 1); /* */    /*E0283*/
        Rank = ToDArr->TLen;

        for(i=0; i < Size; i++)
        {  CharPtr1 = FromAddr;
           CharPtr2 = ToAddr;

           for(ToOnlyAxis=0; ToOnlyAxis < Rank; ToOnlyAxis++,
                                                CharPtr1++, CharPtr2++)
               *CharPtr2 = *CharPtr1;

           FromAddr += MemFromStep;
           ToAddr   += MemToStep;
        }

        return;
     }
  }

  /* ----------------------------------------- */    /*E0284*/

  wWriteBlock = block_Copy(&WriteLocalBlock);
  block_GetWeight(FromBlockPtr, Weight, FromStep);

  if(FromBlockPtr->Set[FromBlockPtr->Rank-1].Step == 1 &&
     ToBlockPtr->Set[Rank-1].Step == 1 &&
     FromBlockPtr->Set[FromBlockPtr->Rank-1].Size ==
     ToBlockPtr->Set[Rank-1].Size)
  {  /* Sizes of main dimensions coincide and steps by this dimensions equal 1 */    /*E0285*/

     MaxLinInd = WriteLocalBlock.Set[Rank-1].Size; /* Size of the main dimension */    /*E0286*/
     block_GetSize(LinInd, &WriteLocalBlock, ToStep)
     Size = (int)( LinInd / MaxLinInd );    /* Size of block without the main dimension*/    /*E0287*/
     if(FromBlockPtr->Rank == Rank)
     {  for(i=0; i < Rank; i++)
        {  if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
           if(FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step)
              break;
        }

        if(i == Rank)
           EquSign = 1;
     }

     if(EquSign)
     {  /* Dimensions of blocks that are read and written are equal.
					 Sizes and steps of all dimensions coincide. */    /*E0288*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl: branch 1\n");

        for(i=0; i < Rank; i++)
            WriteIndex[i] = WriteLocalBlock.Set[i].Lower;
        index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)

        index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex,
                    FromStep)

        ReadLocalBlock.Rank = (byte)Rank;

        for(i=0; i < Rank; i++)
        {  ReadLocalBlock.Set[i].Lower = ReadIndex[i];
           ReadLocalBlock.Set[i].Upper =
           ReadIndex[i] + WriteLocalBlock.Set[i].Size - 1;
           ReadLocalBlock.Set[i].Size = WriteLocalBlock.Set[i].Size;
           ReadLocalBlock.Set[i].Step = WriteLocalBlock.Set[i].Step;
        }

        wReadBlock = block_Copy(&ReadLocalBlock);

        for(i=0; i < Size; i++)
        {  index_FromBlock1S(WriteIndex, &wWriteBlock,
                             &WriteLocalBlock)
           index_FromBlock1S(ReadIndex, &wReadBlock, &ReadLocalBlock)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       MaxLinInd)
        }
     }
     else
     {  /* Either dimensions of read and written blocks aren't equal
           or sizes or steps of some minor dimension don't coincide */    /*E0289*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl: branch 2\n");

        for(i=0; i < Size; i++)
        {  index_FromBlock1S(WriteIndex, &wWriteBlock,
                             &WriteLocalBlock)
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)
           index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex,
                       FromStep)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       MaxLinInd)
        }
     }
  }
  else
  {  /* Either sizes of major dimensions don't coincide or 
        step of some major dimension doesn't equal 1 */    /*E0290*/

     block_GetSize(LinInd, &WriteLocalBlock, ToStep)
     Size = (int)LinInd; /* size of local part of block written */    /*E0291*/

     if(FromBlockPtr->Rank == Rank)
     {  for(i=0; i < Rank; i++)
        {  if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
           if(FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step)
              break;
        }

        if(i == Rank)
           EquSign = 1;
     }

     if(EquSign)
     {  /* Read block and written block have same dimensions and 
           steps by each dimension */    /*E0292*/  

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl: branch 3\n");

        for(i=0; i < Rank; i++)
            WriteIndex[i] = WriteLocalBlock.Set[i].Lower;
        index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)

        index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex,
                    FromStep)

        ReadLocalBlock.Rank = (byte)Rank;

        for(i=0; i < Rank; i++)
        {  ReadLocalBlock.Set[i].Lower = ReadIndex[i];
           ReadLocalBlock.Set[i].Upper =
           ReadIndex[i] + WriteLocalBlock.Set[i].Size - 1;
           ReadLocalBlock.Set[i].Size = WriteLocalBlock.Set[i].Size;
           ReadLocalBlock.Set[i].Step = WriteLocalBlock.Set[i].Step;
        }

        wReadBlock = block_Copy(&ReadLocalBlock);

        for(i=0; i < Size; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock,
                           &WriteLocalBlock, ToStep)
           index_FromBlock(ReadIndex, &wReadBlock,
                           &ReadLocalBlock, FromStep)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
     else
     {  /* Read and written blocks have differnet dimensions or
				  different sizes or steps by some dimension */    /*E0293*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl: branch 4\n");

        for(i=0; i < Size; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock,
                           &WriteLocalBlock, ToStep);
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, ToStep)
           index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex,
                       FromStep)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
  }

  return;
}



void   CopyBlockRepl1(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                      s_DISARRAY  *ToDArr, s_BLOCK  *ToBlockPtr)
{
    DvmType       LinInd, MaxLinInd;
    DvmType       Weight[MAXARRAYDIM];
  s_BLOCK    WriteLocalBlock, wWriteBlock, ReadLocalBlock, wReadBlock;
  DvmType       ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  int        i, Rank, Size, FromOnlyAxis, ToOnlyAxis,
             MemFromStep, MemToStep;
  byte       Step = 0, EquSign = 0;
  char      *FromAddr, *ToAddr, *CharPtr1, *CharPtr2;

  if(ToDArr->HasLocal == 0 || FromDArr->HasLocal == 0 ||
     BlockIntersect(&WriteLocalBlock, ToBlockPtr, &ToDArr->Block) == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlockRepl1: branch 0\n");

     return;
  }

  /* Written block is intersected with
        local part of current processor */    /*E0294*/

  Rank = ToBlockPtr->Rank;

  /* */    /*E0295*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);

  if(FromOnlyAxis >= 0)
  {  /* */    /*E0296*/

     ToOnlyAxis = GetOnlyAxis(ToBlockPtr);

     if(ToOnlyAxis >= 0)
     {  /* */    /*E0297*/
        /* */    /*E0298*/

        /* */    /*E0299*/

        LinInd      = WriteLocalBlock.Set[ToOnlyAxis].Lower -
                      ToBlockPtr->Set[ToOnlyAxis].Lower;
        MaxLinInd   = WriteLocalBlock.Set[ToOnlyAxis].Upper -
                      ToBlockPtr->Set[ToOnlyAxis].Lower;

        /* */    /*E0300*/

        for(i=0; i < Rank; i++)
            WriteIndex[i] = ToBlockPtr->Set[i].Lower;

        WriteIndex[ToOnlyAxis] += LinInd;
        
        LocElmAddr(ToAddr, ToDArr, WriteIndex) /* */    /*E0301*/

        Rank = FromBlockPtr->Rank;

        for(i=0; i < Rank; i++)
            ReadIndex[i] = FromBlockPtr->Set[i].Lower;

        ReadIndex[FromOnlyAxis] += LinInd;
        
        LocElmAddr(FromAddr, FromDArr, ReadIndex) /* */    /*E0302*/
        Size = (int)(MaxLinInd - LinInd + 1); /* */    /*E0303*/

        if(FromOnlyAxis == (Rank-1) &&
           ToOnlyAxis == (ToBlockPtr->Rank-1))
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** CopyBlockRepl1: branch "
                      "Axis -> Axis (fast)\n");

           Size *= ToDArr->TLen; /* */    /*E0304*/
           SYSTEM(memcpy, (ToAddr, FromAddr, Size))
        }
        else
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** CopyBlockRepl1: branch Axis -> Axis\n");

           WriteIndex[ToOnlyAxis]++;

           LocElmAddr(CharPtr1, ToDArr, WriteIndex)/* */    /*E0305*/

           MemToStep = (int)((DvmType)CharPtr1 - (DvmType)ToAddr);/* */    /*E0306*/
           ReadIndex[FromOnlyAxis]++;

           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0307*/
           MemFromStep = (int)((DvmType)CharPtr1 - (DvmType)FromAddr); /* */    /*E0308*/
           Rank = ToDArr->TLen;

           for(i=0; i < Size; i++)
           {  CharPtr1 = FromAddr;
              CharPtr2 = ToAddr;

              for(ToOnlyAxis=0; ToOnlyAxis < Rank;
                  ToOnlyAxis++, CharPtr1++, CharPtr2++)
                  *CharPtr2 = *CharPtr1;

              FromAddr += MemFromStep;
              ToAddr   += MemToStep;
           }
        }

        return;
     }
  }

  /* --------------------------------------------- */    /*E0309*/

  wWriteBlock = block_Copy(&WriteLocalBlock);
  block_GetWeight(FromBlockPtr, Weight, 0);

  if(FromBlockPtr->Rank == Rank)
  {  for(i=0; i < Rank; i++)
         if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
            break;
     if(i == Rank)
        EquSign = 1;
  }

  if(EquSign)
  {  /* Read and written blocks have same dimensions and 
        equal sizes by each dimension */    /*E0310*/  

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** CopyBlockRepl1: branch 1\n");

     for(i=0; i < Rank; i++)
         WriteIndex[i] = WriteLocalBlock.Set[i].Lower;

     index_GetLI(LinInd, ToBlockPtr, WriteIndex, Step)
     index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex, Step)

     ReadLocalBlock.Rank = (byte)Rank;

     for(i=0; i < Rank; i++)
     {  ReadLocalBlock.Set[i].Lower = ReadIndex[i];
        ReadLocalBlock.Set[i].Upper =
        ReadIndex[i] + WriteLocalBlock.Set[i].Size - 1;
        ReadLocalBlock.Set[i].Size = WriteLocalBlock.Set[i].Size;
        ReadLocalBlock.Set[i].Step = 1;
     }

     wReadBlock = block_Copy(&ReadLocalBlock);

     Size = 1;
     Rank--;
     MaxLinInd = WriteLocalBlock.Set[Rank].Size; /* size of the major dimension */    /*E0311*/
     for(i=0; i < Rank; i++)
         Size *= (int)WriteLocalBlock.Set[i].Size; /* size of the block without the major dimension */    /*E0312*/
     for(i=0; i < Size; i++)
     {  index_FromBlock1(WriteIndex, &wWriteBlock, &WriteLocalBlock)
        index_FromBlock1(ReadIndex, &wReadBlock, &ReadLocalBlock)
        CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                    MaxLinInd)
     }
  }
  else
  {  /* Read and written blocks have differnet dimensions or
				different sizes by some dimension*/    /*E0313*/

     if(FromBlockPtr->Set[FromBlockPtr->Rank-1].Size ==
        ToBlockPtr->Set[Rank-1].Size)
     {  /* Read and written blocks have same sizes by the 
           main dimension */    /*E0314*/
        
        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl1: branch 2\n");

        Size = 1;
        Rank--;
        MaxLinInd = WriteLocalBlock.Set[Rank].Size; /* size of the major dimension */    /*E0315*/
        for(i=0; i < Rank; i++)
            Size *= (int)WriteLocalBlock.Set[i].Size; /* size of the block without the major dimension */    /*E0316*/
        for(i=0; i < Size; i++)
        {  index_FromBlock1(WriteIndex, &wWriteBlock,
                            &WriteLocalBlock)
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, Step)
           index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex, Step)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       MaxLinInd)
        }
     }
     else
     {  /* Read and written blocks have different sizes by the 
           main dimension*/    /*E0317*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** CopyBlockRepl1: branch 3\n");

        block_GetSize(LinInd, &WriteLocalBlock, Step)
        Rank = (int)LinInd;

        for(i=0; i < Rank; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock,
                           &WriteLocalBlock, Step);
           index_GetLI(LinInd, ToBlockPtr, WriteIndex, Step)
           index_GetSI(FromBlockPtr, Weight, LinInd, ReadIndex, Step)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
  }

  return;
}



int   AttemptLocCopy(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                     s_DISARRAY  *ToDArr, s_BLOCK  *ToBlockPtr)
{ int        IsFromLocal = 0, IsToLocal = 0, i, FromRank, ToRank, Size,
             FromOnlyAxis, ToOnlyAxis;
  byte       FromStep, ToStep, EquSign = 0, Branch0 = 0;
  s_BLOCK    WriteLocalBlock, wWriteBlock, ReadLocalBlock, wReadBlock;
  DvmType       ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  DvmType       FromLinInd, ToLinInd, FromLocSize, ToLocSize;
  DvmType       Weight[MAXARRAYDIM];
  char      *FromAddr, *ToAddr, *CharPtr1, *CharPtr2; 

  FromStep = GetStepSign(FromBlockPtr);
  ToStep   = GetStepSign(ToBlockPtr);

  if(ToDArr->HasLocal)
     IsToLocal = block_Intersect(&WriteLocalBlock, ToBlockPtr,
                                 &ToDArr->Block, ToBlockPtr, ToStep);
  if(FromDArr->HasLocal)
     IsFromLocal = block_Intersect(&ReadLocalBlock, FromBlockPtr,
                                   &FromDArr->Block, FromBlockPtr,
                                   FromStep);
  if(IsFromLocal == 0 && IsToLocal == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** AttemptLocCopy: branch 0\n");

     return  0;           /* both blocks don't have a local part on current processor */    /*E0318*/
  }

  if(IsFromLocal == 0 || IsToLocal == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** AttemptLocCopy: branch 1\n");

     return  1;           /* one block has a local part on the current
                             processor while the other one doesn't */    /*E0319*/
  }

  /* Both blocks have local parts on the current processor */    /*E0320*/

  ToRank = ToBlockPtr->Rank;
  FromRank = FromBlockPtr->Rank;

  for(i=0; i < ToRank; i++)
      WriteIndex[i] = WriteLocalBlock.Set[i].Lower;
  index_GetLI(ToLocSize, ToBlockPtr, WriteIndex, ToStep)

  for(i=0; i < FromRank; i++)
      ReadIndex[i] = ReadLocalBlock.Set[i].Lower;
  index_GetLI(FromLocSize, FromBlockPtr, ReadIndex, FromStep)

  if(ToLocSize != FromLocSize)
     Branch0 = 1;  /* starting linear indices of local parts
                      of From and To blocks aren't equal */    /*E0321*/

  for(i=0; i < ToRank; i++)
      WriteIndex[i] = WriteLocalBlock.Set[i].Upper;
  index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, ToStep)

  for(i=0; i < FromRank; i++)
      ReadIndex[i] = ReadLocalBlock.Set[i].Upper;
  index_GetLI(FromLinInd, FromBlockPtr, ReadIndex, FromStep)

  if(ToLinInd != FromLinInd)
     Branch0 = 1;  /* end linear indices of local parts of 
                      From and To blocks aren't equal */    /*E0322*/

  /* */    /*E0323*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);

  if(FromOnlyAxis >= 0)
  {  /* */    /*E0324*/

     ToOnlyAxis = GetOnlyAxis(ToBlockPtr);

     if(ToOnlyAxis >= 0)
     {  /* */    /*E0325*/
        /* */    /*E0326*/

        /* */    /*E0327*/

        FromLocSize = dvm_max(FromLocSize, ToLocSize);

        /* */    /*E0328*/

        FromLinInd = dvm_min(FromLinInd, ToLinInd);

        if(FromLocSize > FromLinInd)
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy: branch "
                      "Axis -> Axis (empty)\n");

           return  1; /* */    /*E0329*/
        }

        /* */    /*E0330*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch Axis -> Axis\n");

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = ToBlockPtr->Set[i].Lower;

        WriteIndex[ToOnlyAxis] +=
        (FromLocSize * ToBlockPtr->Set[ToOnlyAxis].Step);
        
        LocElmAddr(ToAddr, ToDArr, WriteIndex) /* */    /*E0331*/

        WriteIndex[ToOnlyAxis] += ToBlockPtr->Set[ToOnlyAxis].Step;

        LocElmAddr(CharPtr1, ToDArr, WriteIndex)/* */    /*E0332*/

        IsToLocal = (int)((DvmType)CharPtr1 - (DvmType)ToAddr); /* */    /*E0333*/

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = FromBlockPtr->Set[i].Lower;

        ReadIndex[FromOnlyAxis] +=
        (FromLocSize * FromBlockPtr->Set[FromOnlyAxis].Step);
        
        LocElmAddr(FromAddr, FromDArr, ReadIndex) /* */    /*E0334*/

        ReadIndex[FromOnlyAxis] += FromBlockPtr->Set[FromOnlyAxis].Step;

        LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0335*/
        IsFromLocal = (int)((DvmType)CharPtr1 - (DvmType)FromAddr); /* */    /*E0336*/

        Size = (int)(FromLinInd - FromLocSize + 1); /* */    /*E0337*/
        ToRank = ToDArr->TLen;

        for(i=0; i < Size; i++)
        {  CharPtr1 = FromAddr;
           CharPtr2 = ToAddr;

           for(FromRank=0; FromRank < ToRank; FromRank++,
                                              CharPtr1++, CharPtr2++)
               *CharPtr2 = *CharPtr1;

           FromAddr += IsFromLocal;
           ToAddr   += IsToLocal;
        }

        if(Branch0)
           return  1; /* */    /*E0338*/

        if(ToDArr->Repl || ToDArr->PartRepl || FromDArr->PartRepl)
           return  1; /* */    /*E0339*/
        else
           return  0;
     }
  }

  /* --------------------------------------------------- */    /*E0340*/

  block_GetSize(ToLocSize, &WriteLocalBlock, ToStep)
  block_GetSize(FromLocSize, &ReadLocalBlock, FromStep)

  if(ToLocSize != FromLocSize)
     Branch0 = 1;  /* sizes of local parts of From and To blocks 
                      aren't equal */    /*E0341*/

  /* Copy block From -> block To for the current processor */    /*E0342*/

  wWriteBlock = block_Copy(&WriteLocalBlock);
  block_GetWeight(FromBlockPtr, Weight, FromStep);

  if(Branch0)
  {  /* Local parts of blocks read and written don't
        exhaust each other */    /*E0343*/

     if(FromRank == ToRank)
     {  for(i=0; i < ToRank; i++)
        {  if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
           if(FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step)
              break;
        }

        if(i == ToRank)
           EquSign = 1;
     }

     if(EquSign)
     {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0344*/

        for(i=0; i < ToRank; i++)
        {  WriteLocalBlock.Set[i].Lower -= ToBlockPtr->Set[i].Lower;
           WriteLocalBlock.Set[i].Upper -= ToBlockPtr->Set[i].Lower;
        }

        for(i=0; i < FromRank; i++)
        {  ReadLocalBlock.Set[i].Lower -= FromBlockPtr->Set[i].Lower;
           ReadLocalBlock.Set[i].Upper -= FromBlockPtr->Set[i].Lower;
        }

        i = block_Intersect(&wReadBlock, &WriteLocalBlock,
                            &ReadLocalBlock, ToBlockPtr, EquSign);

        if(i == 0)
        {  /* Read and written blocks do not have
              common local part */    /*E0345*/

           if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy: branch 2\n");

           return  1;  /* continue interprocessor copying */    /*E0346*/
        }
        else
        {  /* Read and written blocks  have
              common local part */    /*E0347*/

           if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy: branch 3\n");

           for(i=0; i < ToRank; i++)
           {  WriteLocalBlock.Set[i].Lower = wReadBlock.Set[i].Lower +
                                             ToBlockPtr->Set[i].Lower;
              WriteLocalBlock.Set[i].Upper = wReadBlock.Set[i].Upper +
                                             ToBlockPtr->Set[i].Lower;
              WriteLocalBlock.Set[i].Size = wReadBlock.Set[i].Size;
           }

           for(i=0; i < FromRank; i++)
           {  ReadLocalBlock.Set[i].Lower = wReadBlock.Set[i].Lower +
                                            FromBlockPtr->Set[i].Lower;
              ReadLocalBlock.Set[i].Upper = wReadBlock.Set[i].Upper +
                                            FromBlockPtr->Set[i].Lower;
              ReadLocalBlock.Set[i].Size = wReadBlock.Set[i].Size;
           }

           wWriteBlock = block_Copy(&WriteLocalBlock);
           wReadBlock = block_Copy(&ReadLocalBlock);

           block_GetSize(ToLocSize, &WriteLocalBlock, EquSign)

           if(FromBlockPtr->Set[FromRank-1].Step == 1 &&
              ToBlockPtr->Set[ToRank-1].Step == 1)
           {  /* Steps on main dimensions are equal to 1 */    /*E0348*/

              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_arrcpy_))
                 tprintf("*** AttemptLocCopy: branch 3\n");

              ToRank--;
              ToLinInd = WriteLocalBlock.Set[ToRank].Size;/* size of major dimension of
                                                             to block */    /*E0349*/
              Size = (int)(ToLocSize/ToLinInd); /* size of To block without the 
                                                 major dimension */    /*E0350*/

              for(i=0; i < Size; i++)
              {  index_FromBlock1S(WriteIndex, &wWriteBlock,
                                   &WriteLocalBlock)
                 index_FromBlock1S(ReadIndex, &wReadBlock,
                                   &ReadLocalBlock)
                 CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                             ToLinInd)
              }

              return  1;  /* continue interprocessor copying */    /*E0351*/
           }
           else
           {  /* Step on any main dimension is equal to 1 */    /*E0352*/

              if(RTL_TRACE && dacopy_Trace &&
                 TstTraceEvent(call_arrcpy_))
                 tprintf("*** AttemptLocCopy: branch 4\n");

              Size = (int)ToLocSize; /* the size of local part of
                               block written */    /*E0353*/

              for(i=0; i < Size; i++)
              {  index_FromBlock(WriteIndex, &wWriteBlock,
                                 &WriteLocalBlock, EquSign)
                 index_FromBlock(ReadIndex, &wReadBlock,
                                 &ReadLocalBlock, EquSign)
                 CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
              }

              return  1;  /* continue interprocessor copying */    /*E0354*/
           }
        }
     }
     else
     {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0355*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch 5\n");

        Size = (int)ToLocSize; /* the size of local part of
                               block written */    /*E0356*/
        for(i=0; i < Size; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock, &WriteLocalBlock,
                           ToStep);
           index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, ToStep)
           index_GetSI(FromBlockPtr, Weight, ToLinInd, ReadIndex,
                       FromStep)
           IsElmOfBlock(ToRank, &ReadLocalBlock, ReadIndex)
           if(ToRank)
              CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }

        return  1;  /* continue the interprocessor copying */    /*E0357*/
     }
  }

  if(FromBlockPtr->Set[FromRank-1].Step == 1 &&
     ToBlockPtr->Set[ToRank-1].Step == 1 &&
     FromBlockPtr->Set[FromRank-1].Size ==
     ToBlockPtr->Set[ToRank-1].Size)
  {  /* The sizes of major dimensions coincide and their steps are equal 1 */    /*E0358*/

     ToLinInd = WriteLocalBlock.Set[ToRank-1].Size; /* the size of major dimension */    /*E0359*/
     Size = (int)( ToLocSize / ToLinInd );    /* the size of block without  
                                                 the major dimension */    /*E0360*/
     if(FromRank == ToRank)
     {  for(i=0; i < ToRank; i++)
        {  if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
           if(FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step)
              break;
        }

        if(i == ToRank)
           EquSign = 1;
     }

     if(EquSign)
     {  /* Read and written blocks have the same dimensions and
           sizes and steps by each dimension */    /*E0361*/  

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch 6\n");

        wReadBlock = block_Copy(&ReadLocalBlock);

        for(i=0; i < Size; i++)
        {  index_FromBlock1S(WriteIndex, &wWriteBlock, &WriteLocalBlock)
           index_FromBlock1S(ReadIndex, &wReadBlock, &ReadLocalBlock)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       ToLinInd)
        }
     }
     else
     {  /* Either dimensions of read and written blocks aren't 
           the same or sizes or steps of some minor dimension 
           don't coincide */    /*E0362*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch 7\n");

        for(i=0; i < Size; i++)
        {  index_FromBlock1S(WriteIndex, &wWriteBlock, &WriteLocalBlock)
           index_GetLI(FromLinInd, ToBlockPtr, WriteIndex, ToStep)
           index_GetSI(FromBlockPtr, Weight, FromLinInd, ReadIndex,
                       FromStep)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       ToLinInd)
        }
     }
  }
  else
  {  /* Either sizes of major dimensions don't coincide or step 
        by some major dimension is different from 1 */    /*E0363*/

     Size = (int)ToLocSize; /* size of local part of the block written */    /*E0364*/

     if(FromRank == ToRank)
     {  for(i=0; i < ToRank; i++)
        {  if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
              break;
           if(FromBlockPtr->Set[i].Step != ToBlockPtr->Set[i].Step)
              break;
        }

        if(i == ToRank)
           EquSign = 1;
     }

     if(EquSign)
     {  /* Read and written blocks have the same dimensions and sizes and steps 
	         by each dimension */    /*E0365*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch 8\n");

        wReadBlock = block_Copy(&ReadLocalBlock);

        for(i=0; i < Size; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock,
                           &WriteLocalBlock, ToStep)
           index_FromBlock(ReadIndex, &wReadBlock,
                           &ReadLocalBlock, FromStep)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
     else
     {  /* Read and written blocks have different dimensions or
           sizes or steps by some dimension */    /*E0366*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy: branch 9\n");

        for(i=0; i < Size; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock, &WriteLocalBlock,
                           ToStep);
           index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, ToStep)
           index_GetSI(FromBlockPtr, Weight, ToLinInd, ReadIndex,
                       FromStep)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
  }

  if(ToDArr->Repl || ToDArr->PartRepl || FromDArr->PartRepl)
     return  1; /* array To is fully or partially replicated or the From array 
                   is partially replicated*/    /*E0367*/
  else
     return  0;
}



int   AttemptLocCopy1(s_DISARRAY  *FromDArr, s_BLOCK  *FromBlockPtr,
                      s_DISARRAY  *ToDArr, s_BLOCK  *ToBlockPtr)
{ int        IsFromLocal = 0, IsToLocal = 0, i, FromRank, ToRank,
             FromOnlyAxis, ToOnlyAxis, Size;
  byte       Step = 0, EquSign = 0, Branch0 = 0;
  s_BLOCK    WriteLocalBlock, wWriteBlock, ReadLocalBlock, wReadBlock;
  DvmType       ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  DvmType       FromLinInd, ToLinInd, FromLocSize, ToLocSize;
  DvmType       Weight[MAXARRAYDIM];
  char      *FromAddr, *ToAddr, *CharPtr1, *CharPtr2;

  if(ToDArr->HasLocal)
     IsToLocal = BlockIntersect(&WriteLocalBlock, ToBlockPtr,
                                &ToDArr->Block);
  if(FromDArr->HasLocal)
     IsFromLocal = BlockIntersect(&ReadLocalBlock, FromBlockPtr,
                                  &FromDArr->Block);
  if(IsFromLocal == 0 && IsToLocal == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** AttemptLocCopy1: branch 0\n");

     return  0;            /* both blocks doesn't have local parts on the 
															current processor */    /*E0368*/
  }

  if(IsFromLocal == 0 || IsToLocal == 0)
  {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** AttemptLocCopy1: branch 1\n");

     return  1;            /* one block has the local part on the
                              curent processor and the other one doesn't */    /*E0369*/
  }

  /* Both blocks have local parts on the current processor */    /*E0370*/

  ToRank = ToBlockPtr->Rank;
  FromRank = FromBlockPtr->Rank;

  /* */    /*E0371*/

  FromOnlyAxis = GetOnlyAxis(FromBlockPtr);

  if(FromOnlyAxis >= 0)
  {  /* */    /*E0372*/

     ToOnlyAxis = GetOnlyAxis(ToBlockPtr);

     if(ToOnlyAxis >= 0)
     {  /* */    /*E0373*/
        /* */    /*E0374*/

        ToLinInd   = ToBlockPtr->Set[ToOnlyAxis].Lower;
        FromLinInd = FromBlockPtr->Set[FromOnlyAxis].Lower;

        ToLocSize   = WriteLocalBlock.Set[ToOnlyAxis].Lower - ToLinInd;
        FromLocSize = ReadLocalBlock.Set[FromOnlyAxis].Lower -
                      FromLinInd;
         
        if(ToLocSize != FromLocSize)
           Branch0 = 1;  /* */    /*E0375*/

        ToLinInd   = WriteLocalBlock.Set[ToOnlyAxis].Upper - ToLinInd;
        FromLinInd = ReadLocalBlock.Set[FromOnlyAxis].Upper -
                     FromLinInd;
         
        if(ToLinInd != FromLinInd)
           Branch0 = 1;  /* */    /*E0376*/

        /* */    /*E0377*/

        FromLocSize = dvm_max(FromLocSize, ToLocSize);

        /* */    /*E0378*/

        FromLinInd = dvm_min(FromLinInd, ToLinInd);

        if(FromLocSize > FromLinInd)
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy1: branch "
                      "Axis -> Axis (empty)\n");

           return  1; /* */    /*E0379*/
        }

        /* */    /*E0380*/

        for(i=0; i < ToRank; i++)
            WriteIndex[i] = ToBlockPtr->Set[i].Lower;

        WriteIndex[ToOnlyAxis] += FromLocSize;
        
        LocElmAddr(ToAddr, ToDArr, WriteIndex) /* */    /*E0381*/

        for(i=0; i < FromRank; i++)
            ReadIndex[i] = FromBlockPtr->Set[i].Lower;

        ReadIndex[FromOnlyAxis] += FromLocSize;
        
        LocElmAddr(FromAddr, FromDArr, ReadIndex) /* */    /*E0382*/
        Size = (int)(FromLinInd - FromLocSize + 1); /* */    /*E0383*/

        if(FromOnlyAxis == (FromRank-1) && ToOnlyAxis == (ToRank-1))
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy1: branch "
                      "Axis -> Axis (fast)\n");

           Size *= ToDArr->TLen;   /* */    /*E0384*/
           SYSTEM(memcpy, (ToAddr, FromAddr, Size))
        }
        else
        {  if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy1: branch Axis -> Axis\n");

           WriteIndex[ToOnlyAxis]++;

           LocElmAddr(CharPtr1, ToDArr, WriteIndex)/* */    /*E0385*/

           IsToLocal = (int)((DvmType)CharPtr1 - (DvmType)ToAddr);/* */    /*E0386*/
           ReadIndex[FromOnlyAxis]++;

           LocElmAddr(CharPtr1, FromDArr, ReadIndex) /* */    /*E0387*/
           IsFromLocal = (int)((DvmType)CharPtr1 - (DvmType)FromAddr); /* */    /*E0388*/
           ToRank = ToDArr->TLen;

           for(i=0; i < Size; i++)
           {  CharPtr1 = FromAddr;
              CharPtr2 = ToAddr;

              for(FromRank=0; FromRank < ToRank; FromRank++,
                                                 CharPtr1++, CharPtr2++)
                  *CharPtr2 = *CharPtr1;

              FromAddr += IsFromLocal;
              ToAddr   += IsToLocal;
           }
        }

        if(Branch0)
           return  1; /* */    /*E0389*/

        if(ToDArr->Repl || ToDArr->PartRepl || FromDArr->PartRepl)
           return  1; /* */    /*E0390*/
        else
           return  0;
     }
  }

  /* --------------------------------------------- */    /*E0391*/

  for(i=0; i < ToRank; i++)
      WriteIndex[i] = WriteLocalBlock.Set[i].Lower;
  index_GetLI(ToLocSize, ToBlockPtr, WriteIndex, Step)

  for(i=0; i < FromRank; i++)
      ReadIndex[i] = ReadLocalBlock.Set[i].Lower;
  index_GetLI(FromLocSize, FromBlockPtr, ReadIndex, Step)

  if(ToLocSize != FromLocSize)
     Branch0 = 1;  /* starting linear indices of From and To local
                      parts are different */    /*E0392*/

  for(i=0; i < ToRank; i++)
      WriteIndex[i] = WriteLocalBlock.Set[i].Upper;
  index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, Step)

  for(i=0; i < FromRank; i++)
      ReadIndex[i] = ReadLocalBlock.Set[i].Upper;
  index_GetLI(FromLinInd, FromBlockPtr, ReadIndex, Step)

  if(ToLinInd != FromLinInd)
     Branch0 = 1;  /* end indices of From and To blocks local
                      parts are different */    /*E0393*/

  block_GetSize(ToLocSize, &WriteLocalBlock, Step)
  block_GetSize(FromLocSize, &ReadLocalBlock, Step)

  if(ToLocSize != FromLocSize)
     Branch0 = 1;  /* sizes of local parts of From and To blocks 
                      are different */    /*E0394*/

  /* Copy From block -> To block for the current processor */    /*E0395*/

  wWriteBlock = block_Copy(&WriteLocalBlock);
  block_GetWeight(FromBlockPtr, Weight, 0);

  if(FromRank == ToRank)
  {  for(i=0; i < ToRank; i++)
         if(FromBlockPtr->Set[i].Size != ToBlockPtr->Set[i].Size)
            break;
     if(i == ToRank)
        EquSign = 1;
  }

  if(Branch0)
  {  /* Local parts of blocks read and written doesn't exhaust each other */    /*E0396*/

     if(EquSign)
     {  /* dimensions of read block and written block   
           and size of each dimension are equal */    /*E0397*/

        for(i=0; i < ToRank; i++)
        {  WriteLocalBlock.Set[i].Lower -= ToBlockPtr->Set[i].Lower;
           WriteLocalBlock.Set[i].Upper -= ToBlockPtr->Set[i].Lower;
        }

        for(i=0; i < FromRank; i++)
        {  ReadLocalBlock.Set[i].Lower -= FromBlockPtr->Set[i].Lower;
           ReadLocalBlock.Set[i].Upper -= FromBlockPtr->Set[i].Lower;
        }

        i = BlockIntersect(&wReadBlock,
                           &WriteLocalBlock, &ReadLocalBlock);

        if(i == 0)
        {  /* Read and written blocks do not have
              common local part */    /*E0398*/

           if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy1: branch 2\n");

           return  1;  /* continue interprocessor copying */    /*E0399*/
        }
        else
        {  /* Read and written blocks  have
              common local part */    /*E0400*/

           if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
              tprintf("*** AttemptLocCopy1: branch 3\n");

           for(i=0; i < ToRank; i++)
           {  WriteLocalBlock.Set[i].Lower = wReadBlock.Set[i].Lower +
                                             ToBlockPtr->Set[i].Lower;
              WriteLocalBlock.Set[i].Upper = wReadBlock.Set[i].Upper +
                                             ToBlockPtr->Set[i].Lower;
              WriteLocalBlock.Set[i].Size = wReadBlock.Set[i].Size;
           }

           for(i=0; i < FromRank; i++)
           {  ReadLocalBlock.Set[i].Lower = wReadBlock.Set[i].Lower +
                                            FromBlockPtr->Set[i].Lower;
              ReadLocalBlock.Set[i].Upper = wReadBlock.Set[i].Upper +
                                            FromBlockPtr->Set[i].Lower;
              ReadLocalBlock.Set[i].Size = wReadBlock.Set[i].Size;
           }

           wWriteBlock = block_Copy(&WriteLocalBlock);
           wReadBlock = block_Copy(&ReadLocalBlock);

           ToRank--;
           ToLinInd = WriteLocalBlock.Set[ToRank].Size;/* size of major dimension of
                                                          to block */    /*E0401*/
           block_GetSize(ToLocSize, &WriteLocalBlock, Step)
           
           FromRank = (int)(ToLocSize/ToLinInd); /* size of To block without the 
                                                 major dimension */    /*E0402*/
           for(i=0; i < FromRank; i++)
           {  index_FromBlock1(WriteIndex, &wWriteBlock,
                               &WriteLocalBlock)
              index_FromBlock1(ReadIndex, &wReadBlock, &ReadLocalBlock)
              CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                          ToLinInd)
           }

           return  1;  /* continue interprocessor copying */    /*E0403*/
        }
     }
     else
     {  /* dimensions of read block and written block or  
           size of any dimension are not equal */    /*E0404*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy1: branch 4\n");

        FromRank = (int)ToLocSize; /* size of local part of written block */    /*E0405*/
        for(i=0; i < FromRank; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock, &WriteLocalBlock,
                           Step);
           index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, Step)
           index_GetSI(FromBlockPtr, Weight, ToLinInd, ReadIndex, Step)
           IsElmOfBlock(ToRank, &ReadLocalBlock, ReadIndex)
           if(ToRank)
              CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }

        return  1;  /* continue interprocessor copying */    /*E0406*/
     }
  }

  if(EquSign)
  {  /* Read and written blocks have same dimensions and same 
        sizes by each dimension */    /*E0407*/  

     if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
        tprintf("*** AttemptLocCopy1: branch 5\n");

     wReadBlock = block_Copy(&ReadLocalBlock);

     ToRank--;
     ToLinInd = WriteLocalBlock.Set[ToRank].Size; /* size of major dimension of
                                                  to block */    /*E0408*/
     FromRank = (int)(ToLocSize / ToLinInd);  /* size of To block without the 
                                                 major dimension */    /*E0409*/
     for(i=0; i < FromRank; i++)
     {  index_FromBlock1(WriteIndex, &wWriteBlock, &WriteLocalBlock)
        index_FromBlock1(ReadIndex, &wReadBlock, &ReadLocalBlock)
        CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex, ToLinInd)
     }
  }
  else
  {  /* Read and written blocks have different dimensions or 
        different sizes by some dimension */    /*E0410*/

     if(FromBlockPtr->Set[FromRank-1].Size ==
        ToBlockPtr->Set[ToRank-1].Size)
     {  /* Read and written blocks have the equal size of the 
           main dimension*/    /*E0411*/
        
        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy1: branch 6\n");

        ToRank--;
        ToLinInd = WriteLocalBlock.Set[ToRank].Size; /* size of the main dimension */    /*E0412*/
        FromRank = (int)(ToLocSize / ToLinInd); /* size of To block without 
                                                the main dimension */    /*E0413*/
        for(i=0; i < FromRank; i++)
        {  index_FromBlock1(WriteIndex, &wWriteBlock, &WriteLocalBlock)

           index_GetLI(FromLinInd, ToBlockPtr, WriteIndex, Step)
           index_GetSI(FromBlockPtr, Weight, FromLinInd, ReadIndex, Step)
           CopyLocElm1(FromDArr, ReadIndex, ToDArr, WriteIndex,
                       ToLinInd)
        }
     }
     else
     {  /* Read and written blocks have different sizes by the main dimension */    /*E0414*/

        if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_arrcpy_))
           tprintf("*** AttemptLocCopy1: branch 7\n");

        ToRank = (int)ToLocSize;

        for(i=0; i < ToRank; i++)
        {  index_FromBlock(WriteIndex, &wWriteBlock, &WriteLocalBlock,
                           Step);
           index_GetLI(ToLinInd, ToBlockPtr, WriteIndex, Step)
           index_GetSI(FromBlockPtr, Weight, ToLinInd, ReadIndex, Step)
           CopyLocElm(FromDArr, ReadIndex, ToDArr, WriteIndex)
        }
     }
  }

  if(ToDArr->Repl || ToDArr->PartRepl || FromDArr->PartRepl)
     return  1; /* the To array is partially or fully replicated or the
                   From array is partially replicated*/    /*E0415*/
  else
     return  0;
}


#endif   /* _DACOPY_C_ */    /*E0416*/
