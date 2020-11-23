#ifndef _DACONSIS_C_
#define _DACONSIS_C_
/******************/    /*E0000*/

/* */    /*E0001*/

DvmType  __callstd  strtac_(DvmType  ArrayHeader[], LoopRef  *LoopRefPtr,
                            DvmType  AxisArray[], DvmType  CoeffArray[],
                            DvmType  ConstArray[], DvmType  *RenewSignPtr)
{ SysHandle     *ArrayHandlePtr, *LoopHandlePtr;
  s_DISARRAY    *DArr;
  s_AMVIEW      *AMV;
  s_PARLOOP     *PL;
  int            AR, LR, i, j, k, n, m, NormCount = 0, ConstCount = 0,
                 ReplCount = 0, Step = 0, RenewSign;
  s_SPACE        PLSpace;
  s_BLOCK       *Local, PLBlock, wBlock;
  int            AxisAr[MAXARRAYDIM];
  DvmType           ai, bi;
  char          *CharPtr;
  DvmType           IndexArray1[MAXARRAYDIM + 1], IndexArray2[MAXARRAYDIM + 1];
  s_REGULARSET  *PLSet, *DArrSet;
  byte           IsSend = 0;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *rdispls, *sdispls, *sendcounts;
#endif

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0002*/
  DVMFTimeStart(call_strtac_);

  /* */    /*E0003*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0004*/

  RenewSign = (int)*RenewSignPtr;

  if(RTL_TRACE)
     dvm_trace(call_strtac_,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
               "LoopRefPtr=%lx; LoopRef=%lx; RenewSign=%d;\n",
               (uLLng)ArrayHeader, ArrayHeader[0],
               (uLLng)LoopRefPtr, *LoopRefPtr, RenewSign);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.001: wrong call strtac_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr           = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR             = DArr->Space.Rank;
  DArr->DAAxisM1 = -1;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_strtac_))
     {  for(i=0; i < AR; i++)
            tprintf(" AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.002: wrong call strtac_\n"
              "(the replicated DVM array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0005*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.003: wrong call strtac_\n"
              "(the replicated DVM array PS is not the current PS;\n"
              "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
              ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0006*/

  if(DArr->Repl == 0 || DArr->Every == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.008: wrong call strtac_\n"
              "(the object is not a replicated DVM array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0007*/

  if(DArr->CG != NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.020: wrong call strtac_\n"
              "(the replicated DVM array has been inserted "
              "in the consistent group;\n"
              "ArrayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  /* */    /*E0008*/

  if(DArr->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.009: wrong call strtac_\n"
              "(a consistent operation has already been started; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0009*/

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.004: wrong call strtac_\n"
              "(the object is not a parallel loop; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 135.005: wrong call strtac_\n"
                "(the current context is not the parallel loop; "
                "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;
  LR = PL->Rank;

  AMV = PL->AMView;

  if(AMV == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.006: wrong call strtac_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  k = (int)DVM_VMS->ProcCount;

  if(RenewSign == 0)
  {  if(DArr->File != NULL)
     {  if(DArr->Line == DVM_LINE[0])
        {  SYSTEM_RET(i, strcmp, (DVM_FILE[0], DArr->File))
           if(i != 0)
              RenewSign = 1;
        }
        else
           RenewSign = 1;
     }
     else
        RenewSign = 1;
  }

  if(RenewSign != 0 && DArr->File != NULL)
  {  /* */    /*E0010*/

     DArr->RealConsist = 0;
     DArr->CWriteBSize = 0;

     /* */    /*E0011*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     if(DArr->AllocSign != 0)
     {  /* */    /*E0012*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0013*/

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  mac_free(&DArr->CReadBuf[i]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0014*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0015*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  if(DArr->ReadBlockNumber[i] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0016*/

                    mac_free(&DArr->CReadBuf[i]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(i=0; i < k; i++)
        {  dvm_FreeArray(DArr->CRBlockPtr[i]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }
  }

  /* */    /*E0017*/

  DArr->File = DVM_FILE[0];
  DArr->Line = DVM_LINE[0];

  DArr->CentralPSPtr     = NULL;
  DArr->DArrWBlockPtr    = NULL;
  DArr->WriteBlockNumber = 0;

  DArr->ReadBlockNumber  = NULL;
  DArr->CRBlockPtr       = NULL;

  if(PL->Empty)
  {  /* */    /*E0018*/

     DArr->ConsistSign = 1;
     DArr->RealConsist = 0;
     DArr->AllocSign   = 0;

     DArr->CWriteBSize = 0;

     DArr->CWriteBuf   = NULL;
     DArr->CWriteReq   = NULL;

     DArr->CReadBSize  = NULL;
     DArr->CReadBuf    = NULL;
     DArr->CReadReq    = NULL;
     DArr->CReadBlock  = NULL;

     DArr->File = NULL;
     DArr->Line = 0;

     if(RTL_TRACE)
        dvm_trace(ret_strtac_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0019*/
     DVMFTimeFinish(ret_strtac_);
     return  (DVM_RET, 1);
  }

  /* */    /*E0020*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.007: wrong call strtac_\n"
              "(the parallel loop PS is not the current PS;\n"
              "LoopRef=%lx; LoopPSRef=%lx; CurrentPSRef=%lx)\n",
              *LoopRefPtr, (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0021*/

  if(RenewSign)
  {  for(i=0; i < AR; i++)
     {  if(AxisArray[i] < -1)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 135.010: wrong call strtac_\n"
                    "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);

        if(AxisArray[i] > LR)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 135.011: wrong call strtac_\n"
                    "(AxisArray[%d]=%ld > %d; LoopRef=%lx)\n",
                    i, AxisArray[i], LR, *LoopRefPtr);

        if(AxisArray[i] >= 0)
        {  if(CoeffArray[i] != 0)
           { if(AxisArray[i] == 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                         "*** RTS err 135.012: wrong call strtac_\n"
                         "(AxisArray[%d]=0)\n", i);

             ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
                  ConstArray[i];

             if(ai < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 135.013: wrong call strtac_\n"
                     "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                     " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                     i, CoeffArray[i], AxisArray[i]-1,
                     PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                     *LoopRefPtr);

             if(ai >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 135.014: wrong call strtac_\n"
                     "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                     " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                     i, CoeffArray[i], AxisArray[i]-1,
                     PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                     DArr->Space.Size[i], *LoopRefPtr);

             ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
                  ConstArray[i];

             if(ai < 0)
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 135.015: wrong call strtac_\n"
                     "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                     " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                     i, CoeffArray[i], AxisArray[i]-1,
                     PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                     *LoopRefPtr);

             if(ai >= DArr->Space.Size[i])
                epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                     "*** RTS err 135.016: wrong call strtac_\n"
                     "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                     " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                     i, CoeffArray[i], AxisArray[i]-1,
                     PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                     DArr->Space.Size[i], *LoopRefPtr);
           }
           else
           {  if(ConstArray[i] < 0)
                 epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                          "*** RTS err 135.017: wrong call strtac_\n"
                          "(ConstArray[%d]=%ld < 0)\n",
                          i, ConstArray[i]);

              if(ConstArray[i] >= DArr->Space.Size[i])
                 epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                          "*** RTS err 135.018: wrong call strtac_\n"
                          "(ConstArray[%d]=%ld >= %ld)\n",
                          i, ConstArray[i], DArr->Space.Size[i]);
           }
        }
     }

     /* */    /*E0022*/

     for(i=0; i < LR; i++)
         AxisAr[i] = 0;

     for(i=0; i < AR; i++)
     {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
           continue;

        if(AxisAr[AxisArray[i]-1])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 135.019: wrong call strtac_\n"
                    "(AxisArray[%d]=AxisArray[%d]=%ld)\n",
                    AxisAr[AxisArray[i]-1]-1, i, AxisArray[i]);

        AxisAr[AxisArray[i]-1] = i+1;
     }
  }

  /* */    /*E0023*/

  n = DVM_VMS->Space.Rank;

  for(i=0; i < AR; i++)
  {  AxisAr[i] = (int)AxisArray[i];

     if(AxisAr[i] == -1)
     {  ReplCount++; /* */    /*E0024*/
        continue;
     }

     if(AxisAr[i] == 0)
     {  ConstCount++;    /* */    /*E0025*/
        continue;
     }

     for(j=0; j < n; j++)
         if(PL->PLAxis[j] == AxisAr[i])
            break;

     if(j < n && DVM_VMS->Space.Size[j] > 1)
        NormCount++;     /* */    /*E0026*/
     else
     {  ReplCount++;     /* */    /*E0027*/
        AxisAr[i] = -1;
     }
  }

  if(NormCount == 0 || k == 1)
  {  /* */    /*E0028*/

     DArr->ConsistSign = 1;
     DArr->RealConsist = 0;
     DArr->AllocSign   = 0;

     DArr->CWriteBSize = 0;

     DArr->CWriteBuf   = NULL;
     DArr->CWriteReq   = NULL;

     DArr->CReadBSize  = NULL;
     DArr->CReadBuf    = NULL;
     DArr->CReadReq    = NULL;
     DArr->CReadBlock  = NULL;

     DArr->File = NULL;
     DArr->Line = 0;

     if(RTL_TRACE)
        dvm_trace(ret_strtac_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0029*/
     DVMFTimeFinish(ret_strtac_);
     return  (DVM_RET, 2);
  }

  if(RenewSign)
  {  dvm_AllocArray(int, k, DArr->CReadBSize);
     dvm_AllocArray(void *, k, DArr->CReadBuf);
  }

  DArr->ConsistSign = 1;       /* */    /*E0030*/
  DArr->RealConsist = 1;       /* */    /*E0031*/

#ifdef  _DVM_MPI_
  if(MPIGather)     /* */    /*E0032*/
  {  DArr->CReadReq  = NULL;
     DArr->CWriteReq = NULL;
  }
  else
#endif
  {  if(RenewSign)
     {  dvm_AllocArray(RTL_Request, k, DArr->CReadReq);
        dvm_AllocArray(RTL_Request, k, DArr->CWriteReq);
     }
  }

  /* */    /*E0033*/

  if(RenewSign)
  {  DArr->AllocSign = 1;  /* */    /*E0034*/

     if(IsSynchr == 0)
     {  if(NormCount == 1)
        {  for(j=0; j < AR; j++)
               if(AxisAr[j] > 0)
                  break;

           if(j < AR)
           {  for(n=0; n < j; n++)
                  if(AxisAr[n] != 0)
                     break;

              if(n == j)
              {  for(m=j+1; m < AR; m++)
                     if(AxisAr[m] >= 0)
                        break;

                 if(m == AR)
                    DArr->AllocSign = 0;  /* */    /*E0035*/
              }
           }  
        }
     }
  }

  /* */    /*E0036*/

  if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_strtac_))
  {  if(DArr->AllocSign)
        tprintf("*** Alloc buffer consistent branch\n");
     else
        tprintf("*** Array memory consistent branch\n");
  }

  if(RenewSign)
  {  /* */    /*E0037*/

     if(DArr->AllocSign == 0)
     {  DArr->CReadBlock = NULL;

        for(j=0; j < AR; j++)
        {  if(AxisAr[j] == 0)
           {  IndexArray1[j] = ConstArray[j];
              IndexArray2[j] = ConstArray[j];
           }
           else
           {  IndexArray1[j] = DArr->Block.Set[j].Lower;
              IndexArray2[j] = DArr->Block.Set[j].Upper;
           }
        }
     }
     else
     {  dvm_AllocArray(s_BLOCK, k, DArr->CReadBlock);
     }

     /* */    /*E0038*/
  
     PLSpace.Rank = (byte)LR;
  
     for(i=0; i < LR; i++)
         PLSpace.Size[i] = PL->Set[i].Size;

     PLSet = PLBlock.Set;

     for(i=0; i < k; i++)
     {  Local = GetSpaceLB4Proc(i, AMV, &PLSpace, PL->Align,
                                NULL, &PLBlock);

        if(DVM_VMS->VProc[i].lP == MPS_CurrentProc)
        {  /* */    /*E0039*/

           DArr->CReadBSize[i] = 0;
           DArr->CReadBuf[i]   = NULL;

           if(Local == NULL)
           {  DArr->CWriteBSize = 0;
              DArr->CWriteBuf   = NULL;
              continue;
           }
        
           for(j=0; j < LR; j++)
           {  PLSet[j].Lower += PL->InitIndex[j];
              PLSet[j].Upper += PL->InitIndex[j];
           }

           DArr->DArrBlock = block_Copy(&DArr->Block);
           Local   = &DArr->DArrBlock;
           DArrSet = Local->Set;

           for(j=0; j < AR; j++)
           {  if(AxisAr[j] == -1)
                 continue;

              if(AxisAr[j] == 0)
              {  DArrSet[j].Lower = ConstArray[j];
                 DArrSet[j].Upper = ConstArray[j];
                 DArrSet[j].Size  = 1;
                 continue;
              }

              ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                   ConstArray[j];
              bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                   ConstArray[j];

              DArrSet[j].Lower = dvm_min(ai, bi);
              DArrSet[j].Upper = dvm_max(ai, bi);
              DArrSet[j].Size  = DArrSet[j].Upper - DArrSet[j].Lower
                                 + 1;
           }
           if(DArr->AllocSign)
           {  /* */    /*E0040*/
              block_GetSize(DArr->CWriteBSize, Local, Step);
              DArr->CWriteBSize *= DArr->TLen;              
              mac_malloc(DArr->CWriteBuf, void *, DArr->CWriteBSize, 0);

              CharPtr = (char *)DArr->CWriteBuf;

              wBlock = block_Copy(Local);

              ai = DArrSet[AR-1].Size;
              if (ai > 0) {
                 n  = (int)(ai * DArr->TLen);
                 m  = (int)(DArr->CWriteBSize / n);

                 for(j=0; j < m; j++)
                 {  index_FromBlock1(IndexArray1, &wBlock, Local)
                    GetLocElm1(DArr, IndexArray1, CharPtr, ai)
                    CharPtr += n;
                 }
              }
           }
           else
           {  /* */    /*E0041*/

              if(AxisAr[AR-1] == 0)
                 IndexArray2[AR-1] = ConstArray[AR-1];
              else
                 IndexArray2[AR-1] = DArr->Block.Set[AR-1].Upper;

              for(j=0; j < AR; j++)
              {  if(AxisAr[j] <= 0)
                    continue;

                 ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                      ConstArray[j];
                 bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                      ConstArray[j];

                 IndexArray1[j] = dvm_min(ai, bi);
                 IndexArray2[j] = dvm_max(ai, bi);
              }

              LocElmAddr(CharPtr, DArr, IndexArray1);
              DArr->CWriteBuf = (void *)CharPtr;
              ai = (uLLng)CharPtr;

              IndexArray2[AR-1]++;
              LocElmAddr(CharPtr, DArr, IndexArray2);
              bi = (uLLng)CharPtr;

              DArr->CWriteBSize = (int)(bi - ai);
           }
        }
        else
        {  /* */    /*E0042*/

           if(Local == NULL)
           {  DArr->CReadBSize[i] = 0;
              DArr->CReadBuf[i]   = NULL;
              continue;
           }

           for(j=0; j < LR; j++)
           {  PLSet[j].Lower += PL->InitIndex[j];
              PLSet[j].Upper += PL->InitIndex[j];
           }

           if(DArr->AllocSign)
           {  /* */    /*E0043*/

              DArr->CReadBlock[i] = block_Copy(&DArr->Block);
              DArrSet = DArr->CReadBlock[i].Set;

              for(j=0; j < AR; j++)
              {  if(AxisAr[j] == -1)
                    continue;

                 if(AxisAr[j] == 0)
                 {  DArrSet[j].Lower = ConstArray[j];
                    DArrSet[j].Upper = ConstArray[j];
                    DArrSet[j].Size  = 1;
                    continue;
                 }

                 ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                      ConstArray[j];
                 bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                      ConstArray[j];

                 DArrSet[j].Lower = dvm_min(ai, bi);
                 DArrSet[j].Upper = dvm_max(ai, bi);
                 DArrSet[j].Size  = DArrSet[j].Upper - DArrSet[j].Lower
                                    + 1;
              }

              block_GetSize(DArr->CReadBSize[i], &DArr->CReadBlock[i],
                            Step);
              DArr->CReadBSize[i] *= DArr->TLen;              
              mac_malloc(DArr->CReadBuf[i], void *, DArr->CReadBSize[i],
                         0);
           }
           else
           {  /* */    /*E0044*/

              if(AxisAr[AR-1] == 0)
                 IndexArray2[AR-1] = ConstArray[AR-1];
              else
                 IndexArray2[AR-1] = DArr->Block.Set[AR-1].Upper;

              for(j=0; j < AR; j++)
              {  if(AxisAr[j] <= 0)
                    continue;

                 ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                      ConstArray[j];
                 bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                      ConstArray[j];

                 IndexArray1[j] = dvm_min(ai, bi);
                 IndexArray2[j] = dvm_max(ai, bi);
              }

              LocElmAddr(CharPtr, DArr, IndexArray1);
              DArr->CReadBuf[i] = (void *)CharPtr;
              ai = (uLLng)CharPtr;

              IndexArray2[AR-1]++;
              LocElmAddr(CharPtr, DArr, IndexArray2);
              bi = (uLLng)CharPtr;

              DArr->CReadBSize[i] = (int)(bi - ai);
           }
        }
     }
  }
  else
  {  /* */    /*E0045*/

     if(DArr->AllocSign && DArr->CWriteBSize)
     {  /* */    /*E0046*/

        Local   = &DArr->DArrBlock;
        DArrSet = Local->Set;

        CharPtr = (char *)DArr->CWriteBuf;

        wBlock = block_Copy(Local);

        ai = DArrSet[AR-1].Size;
        if (ai > 0) {
           n = (int)(ai * DArr->TLen);
           m = (int)(DArr->CWriteBSize / n);

           for(j=0; j < m; j++)
           {  index_FromBlock1(IndexArray1, &wBlock, Local)
              GetLocElm1(DArr, IndexArray1, CharPtr, ai)
              CharPtr += n;
           }
        }
     }
  }

  /* */    /*E0047*/

#ifdef  _DVM_MPI_

  if(MPIGather)
  {  /* */    /*E0048*/

     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0049*/

     if(DArr->CWriteBSize == 0)
        sendbuf = (void *)&AlltoallMem;
     else
        sendbuf = DArr->CWriteBuf;

     /* */    /*E0050*/

     if(DArr->ConsistProcCount != k)
     {  if(DArr->ConsistProcCount != 0)
        {  dvm_FreeArray(DArr->sdispls);
           dvm_FreeArray(DArr->sendcounts);
           dvm_FreeArray(DArr->rdispls);
        }

        DArr->ConsistProcCount = k;
        dvm_AllocArray(int, k, DArr->sdispls);
        dvm_AllocArray(int, k, DArr->sendcounts);
        dvm_AllocArray(int, k, DArr->rdispls);
     }

     sdispls    = DArr->sdispls;
     sendcounts = DArr->sendcounts;
     rdispls    = DArr->rdispls;

     for(i=0; i < k; i++)
     {  sdispls[i] = 0;
        sendcounts[i] = 0;

        j = (int)DVM_VMS->VProc[i].lP;

        if(j == MPS_CurrentProc)
           continue;

        if(DArr->CentralPSPtr != NULL)
        {  m = DArr->WriteBlockNumber;

           for(n=0; n < m; n++)
               if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                  break;

           if(n == m)
              continue;  /* */    /*E0051*/
        } 

        sendcounts[i] = DArr->CWriteBSize;
     }

     /* */    /*E0052*/

     ai = DVMTYPE_MAX;     /*E0053*/
     j = -1;

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0054*/

        bi = (DvmType)DArr->CReadBuf[i];

        if(bi < ai)
        {  j = i;    /* */    /*E0055*/ 
           ai = bi;  /* */    /*E0056*/
        }
     }

     if(j < 0)
        recvbuf = (void *)&AlltoallMem;
     else
        recvbuf = DArr->CReadBuf[j];

     /* */    /*E0057*/

     dvm_AllocArray(int, k,  rdispls);

     j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0058*/

     for(n=0; n < k; n++)
     {  if(DArr->CReadBSize[n] == 0)
           rdispls[n] = 0;
        else
        {
            bi = (DvmType)DArr->CReadBuf[n] - ai;

           if(bi >= j)
              break;    /* */    /*E0059*/
           rdispls[n] = (int)bi;
        }
     }

     if(n < k)
     {  /* */    /*E0060*/

        for(i=0,m=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              rdispls[i] = 0;
           else
           {  rdispls[i] = m;
              m += DArr->CReadBSize[i];
           }
        }

        mac_malloc(recvbuf, void *, m, 0);
     }

     /* */    /*E0061*/

     if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_strtac_))
     {  if(n < k)
           tprintf("*** MPI_Allgatherv consistent branch\n");
        else
           tprintf("*** MPI_Allgatherv consistent branch (fast)\n");
     }

     /* */    /*E0062*/

     MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_CHAR,
                   recvbuf, DArr->CReadBSize, rdispls, MPI_CHAR,
                   DVM_VMS->PS_MPI_COMM);

     if(n < k)
     {  /* */    /*E0063*/

        CharPtr = (char *)recvbuf;

        for(i=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              continue;

           SYSTEM(memcpy, ((char *)DArr->CReadBuf[i], CharPtr,
                           DArr->CReadBSize[i]))
           CharPtr += DArr->CReadBSize[i];
        }

        mac_free(&recvbuf);
     }

     DVMMTimeFinish;    /* */    /*E0064*/
  }
  else

#endif
  {  /* */    /*E0065*/

     if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_strtac_))
        tprintf("*** Point_Point consistent branch\n");

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0066*/

        j = (int)DVM_VMS->VProc[i].lP;

        ( RTL_CALL, rtl_Recvnowait(DArr->CReadBuf[i], 1,
                                   DArr->CReadBSize[i], j,
                                   DVM_VMS->tag_DACopy,
                                   &DArr->CReadReq[i], 1) );
     }

     if(DArr->CWriteBSize != 0)
     {  for(i=0; i < k; i++)
        {  j = (int)DVM_VMS->VProc[i].lP;

           if(j == MPS_CurrentProc)
              continue;

           IsSend = 1;

           ( RTL_CALL, rtl_Sendnowait(DArr->CWriteBuf, 1,
                                      DArr->CWriteBSize, j,
                                      DVM_VMS->tag_DACopy,
                                      &DArr->CWriteReq[i],
                                      a_DA1_EQ_DA1) );
        }
     }
  }

  if(IsSend && MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
  {  /* */    /*E0067*/

     rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffDACopy);
  }

  if(RTL_TRACE)
     dvm_trace(ret_strtac_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0068*/
  DVMFTimeFinish(ret_strtac_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  waitac_(DvmType  ArrayHeader[])
{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArr;
  int            i, j, k, m, n, p, q, ARm1, Step = 0;
  char          *CharPtr;
  s_BLOCK       *BlockPtr, wBlock;
  DvmType           tlong;
  DvmType           IndexArray[MAXARRAYDIM + 1];

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0069*/
  DVMFTimeStart(call_waitac_);

  if(RTL_TRACE)
     dvm_trace(call_waitac_, "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.020: wrong call waitac_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  ARm1 = DArr->Space.Rank - 1;

  /* */    /*E0070*/

  if(DVM_LEVEL == 0 && DArr->CG != NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.022: wrong call waitac_\n"
              "(the replicated DVM array has been inserted "
              "in the consistent group;\n"
              "ArrayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  if(DArr->ConsistSign == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.024: wrong call waitac_\n"
              "(a consistent operation has not been started; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr->ConsistSign = 0;

  if(DArr->RealConsist == 0)
  {  if(RTL_TRACE)
        dvm_trace(ret_waitac_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0071*/
     DVMFTimeFinish(ret_waitac_);
     return  (DVM_RET, 0);
  }

  k = (int)DVM_VMS->ProcCount;

#ifdef  _DVM_MPI_  
  if(MPIGather == 0)
#endif
  {  /* */    /*E0072*/

     /* */    /*E0073*/

     if(DArr->CWriteBSize != 0)
     {  for(i=0; i < k; i++)
        {  j = (int)DVM_VMS->VProc[i].lP;

           if(j == MPS_CurrentProc)
              continue;

           if(DArr->CentralPSPtr != NULL)
           {  m = DArr->WriteBlockNumber;

              for(n=0; n < m; n++)
                  if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                     break;

              if(n == m)
                 continue;  /* */    /*E0074*/
           } 

           ( RTL_CALL, rtl_Waitrequest(&DArr->CWriteReq[i]) );
        }
     }

     /* */    /*E0075*/

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0076*/

        ( RTL_CALL, rtl_Waitrequest(&DArr->CReadReq[i]) );
     }
  }

  if( DArr->AllocSign != 0 ||
      (DArr->ReadBlockNumber != NULL && DArr->CRBlockPtr != NULL) )
  {  /* */    /*E0077*/

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0078*/

        CharPtr  = (char *)DArr->CReadBuf[i];

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0079*/

           BlockPtr = &DArr->CReadBlock[i];
           wBlock   = block_Copy(BlockPtr);

           tlong = BlockPtr->Set[ARm1].Size;
           if (tlong > 0) {
              n = (int)(tlong * DArr->TLen);
              m = (int)(DArr->CReadBSize[i] / n);

              for(j=0; j < m; j++)
              {  index_FromBlock1(IndexArray, &wBlock, BlockPtr)
                 PutLocElm1(CharPtr, DArr, IndexArray, tlong)
                 CharPtr += n;
              }
           }
        }
        else
        {  /* */    /*E0080*/

           q = DArr->ReadBlockNumber[i];

           if(q != 1 || DArr->DAAxisM1 != 0 || IsSynchr != 0)
           {  for(p=0; p < q; p++)
              {
                 BlockPtr = &DArr->CRBlockPtr[i][p];
                 wBlock   = block_Copy(BlockPtr);

                 tlong = BlockPtr->Set[ARm1].Size;
                 if (tlong > 0) {
                    n = (int)(tlong * DArr->TLen);
                    block_GetSize(m, BlockPtr, Step);
                    m  = (int)(m / tlong);

                    for(j=0; j < m; j++)
                    {  index_FromBlock1(IndexArray, &wBlock, BlockPtr)
                       PutLocElm1(CharPtr, DArr, IndexArray, tlong)
                       CharPtr += n;
                    }
                 }
              }
           }
        }
     }
  }

  /* */    /*E0081*/

  if( DVM_LEVEL == 0 &&
      ((strtac_FreeBuf && DArr->CentralPSPtr == NULL) ||
       (consda_FreeBuf && DArr->CentralPSPtr != NULL)) )
  {
     /* */    /*E0082*/

     DArr->CWriteBSize = 0;
     DArr->Line        = 0;
     DArr->File        = NULL;

     /* */    /*E0083*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     if(DArr->AllocSign != 0)
     {  /* */    /*E0084*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0085*/

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  mac_free(&DArr->CReadBuf[i]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0086*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0087*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  if(DArr->ReadBlockNumber[i] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0088*/

                    mac_free(&DArr->CReadBuf[i]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(i=0; i < k; i++)
        {  dvm_FreeArray(DArr->CRBlockPtr[i]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_waitac_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0089*/
  DVMFTimeFinish(ret_waitac_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  rstrda_(DvmType  ArrayHeader[])
{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArr;
  int            i, k;

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0090*/
  DVMFTimeStart(call_rstrda_);

  if(RTL_TRACE)
     dvm_trace(call_rstrda_, "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.000: wrong call rstrda_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr   = (s_DISARRAY *)ArrayHandlePtr->pP;

  /* */    /*E0091*/

  if(DArr->Repl == 0 || DArr->Every == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.001: wrong call rstrda_\n"
              "(the object is not a replicated DVM array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0092*/

  if(DArr->CG != NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.002: wrong call rstrda_\n"
              "(the replicated DVM array has been inserted "
              "in the consistent group;\n"
              "ArrayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  if(DArr->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 136.005: wrong call rstrda_\n"
              "(a consistent operation has not been completed; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0093*/

  DArr->RealConsist = 0;
  DArr->CWriteBSize = 0;
  DArr->File        = NULL;
  DArr->Line        = 0;

  /* */    /*E0094*/

  dvm_FreeArray(DArr->CReadReq); 
  dvm_FreeArray(DArr->CWriteReq); 
  dvm_FreeArray(DArr->CReadBSize);

  dvm_FreeArray(DArr->CentralPSPtr);
  dvm_FreeArray(DArr->DArrWBlockPtr);

  k = (int)DVM_VMS->ProcCount;

  if(DArr->AllocSign != 0)
  {  /* */    /*E0095*/

     if(DArr->CReadBlock != NULL)
     {  /* */    /*E0096*/

        if(DArr->CReadBuf != NULL)
        {  for(i=0; i < k; i++)
           {  mac_free(&DArr->CReadBuf[i]);
           }
        }

        mac_free(&DArr->CWriteBuf);
        dvm_FreeArray(DArr->CReadBlock);
     }
     else
     {  /* */    /*E0097*/

        if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
           IsSynchr != 0)
        {  /* */    /*E0098*/

           mac_free(&DArr->CWriteBuf);
        }

        if(DArr->CReadBuf != NULL)
        {  for(i=0; i < k; i++)
           {  if(DArr->ReadBlockNumber[i] != 1 || DArr->DAAxisM1 != 0 ||
                 IsSynchr != 0)
              {  /* */    /*E0099*/

                 mac_free(&DArr->CReadBuf[i]);
              }
           }
        }
     }
  }

  dvm_FreeArray(DArr->CReadBuf);
  dvm_FreeArray(DArr->ReadBlockNumber);

  DArr->WriteBlockNumber =  0;
  DArr->DAAxisM1         = -1;
  DArr->AllocSign        =  0;

  if(DArr->CRBlockPtr != NULL)
  {  for(i=0; i < k; i++)
     {  dvm_FreeArray(DArr->CRBlockPtr[i]);
     }

     dvm_FreeArray(DArr->CRBlockPtr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_rstrda_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0100*/
  DVMFTimeFinish(ret_rstrda_);
  return  (DVM_RET, 0);
}



DAConsistGroupRef  __callstd crtcg_(DvmType  *StaticSignPtr, DvmType  *DelDASignPtr)
{ SysHandle           *DAGHandlePtr;
  s_DACONSISTGROUP    *DAG;
  DAConsistGroupRef    Res;

  DVMFTimeStart(call_crtcg_);

  if(RTL_TRACE)
     dvm_trace(call_crtcg_, "StaticSign=%ld; DelDASign=%ld;\n",
                            *StaticSignPtr, *DelDASignPtr);

  dvm_AllocStruct(s_DACONSISTGROUP, DAG);
  dvm_AllocStruct(SysHandle, DAGHandlePtr);

  DAG->Static      = (byte)*StaticSignPtr;
  DAG->RDA         = coll_Init(ConsistDAGrpCount, ConsistDAGrpCount,
                               NULL);
  DAG->DelDA       = (byte)*DelDASignPtr; /* */    /*E0101*/
  DAG->ConsistSign = 0;    /* */    /*E0102*/
  DAG->ResetSign   = 0;    /* */    /*E0103*/

  *DAGHandlePtr  = genv_InsertObject(sht_DAConsistGroup, DAG);
  DAG->HandlePtr = DAGHandlePtr; /* */    /*E0104*/

  if(TstObject)
     InsDVMObj((ObjectRef)DAGHandlePtr);

  Res = (DAConsistGroupRef)DAGHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_crtcg_,"DAConsistGroupRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* */    /*E0105*/
  DVMFTimeFinish(ret_crtcg_);
  return  (DVM_RET, Res);
}



DvmType  __callstd  inscg_(DAConsistGroupRef  *DAConsistGroupRefPtr,
                           DvmType  ArrayHeader[], LoopRef  *LoopRefPtr,
                           DvmType  AxisArray[], DvmType  CoeffArray[],
                           DvmType  ConstArray[], DvmType  *RenewSignPtr)
{ SysHandle         *ArrayHandlePtr, *LoopHandlePtr, *DAGHandlePtr;
  s_DACONSISTGROUP  *DAG;
  s_DISARRAY        *DArr;
  s_AMVIEW          *AMV;
  s_PARLOOP         *PL;
  int                AR, LR, i, j, k, n, m, NormCount = 0,
                     ConstCount = 0, ReplCount = 0, Step = 0, RenewSign;
  s_SPACE            PLSpace;
  s_BLOCK           *Local, PLBlock;
  int                AxisAr[MAXARRAYDIM];
  DvmType               ai, bi;
  char              *CharPtr;
  DvmType               IndexArray1[MAXARRAYDIM + 1],
                     IndexArray2[MAXARRAYDIM+1];
  s_REGULARSET      *PLSet, *DArrSet;

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0106*/
  DVMFTimeStart(call_inscg_);

  RenewSign = (int)*RenewSignPtr;

  if(RTL_TRACE)
     dvm_trace(call_inscg_,
               "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx;\n"
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
               "LoopRefPtr=%lx; LoopRef=%lx; RenewSign=%d;\n",
               (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr,
               (uLLng)ArrayHeader, ArrayHeader[0],
               (uLLng)LoopRefPtr, *LoopRefPtr, RenewSign);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.023: wrong call inscg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.024: wrong call inscg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;

  /* */    /*E0107*/

  if(DAG->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.026: wrong call inscg_\n"
              "(a consistent operation has not been completed; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.001: wrong call inscg_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr           = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR             = DArr->Space.Rank;
  DArr->DAAxisM1 = -1;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_inscg_))
     {  for(i=0; i < AR; i++)
            tprintf(" AxisArray[%d]=%ld; ", i, AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("CoeffArray[%d]=%ld; ", i, CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < AR; i++)
            tprintf("ConstArray[%d]=%ld; ", i, ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.002: wrong call inscg_\n"
              "(the replicated DVM array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0108*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.003: wrong call inscg_\n"
              "(the replicated DVM array PS is not the current PS;\n"
              "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
              ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0109*/

  if(DArr->Repl == 0 || DArr->Every == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.008: wrong call inscg_\n"
              "(the object is not a replicated DVM array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0110*/

  if(DArr->CG && DArr->CG != DAG)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.021: wrong call inscg_\n"
              "(the replicated DVM array has already been inserted "
              "in the consistent group;\n"
              "AeeayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  /* */    /*E0111*/

  if(DArr->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.009: wrong call inscg_\n"
              "(a consistent operation has not been completed; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0112*/

  LoopHandlePtr = (SysHandle *)*LoopRefPtr;

  if(LoopHandlePtr->Type != sht_ParLoop)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.004: wrong call inscg_\n"
              "(the object is not a parallel loop; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  if(TstObject)
  { PL=(coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

    if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 137.005: wrong call inscg_\n"
                "(the current context is not the parallel loop; "
                "LoopRef=%lx)\n", *LoopRefPtr);
  }

  PL = (s_PARLOOP *)LoopHandlePtr->pP;
  LR = PL->Rank;

  AMV = PL->AMView;

  if(AMV == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.006: wrong call inscg_\n"
              "(the parallel loop has not been mapped; "
              "LoopRef=%lx)\n", *LoopRefPtr);

  k = (int)DVM_VMS->ProcCount;

  if(RenewSign == 0)
  {  if(DArr->File != NULL)
     {  if(DArr->Line == DVM_LINE[0])
        {  SYSTEM_RET(i, strcmp, (DVM_FILE[0], DArr->File))
           if(i != 0)
              RenewSign = 1;
        }
        else
           RenewSign = 1;
     }
     else
        RenewSign = 1;
  }

  if( (DArr->CG == NULL && DArr->File != NULL) ||
      (DArr->CG != NULL && RenewSign != 0) )

  {  /* */    /*E0113*/

     DArr->RealConsist = 0;
     DArr->CWriteBSize = 0;

     /* */    /*E0114*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     if(DArr->AllocSign != 0)
     {  /* */    /*E0115*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0116*/

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  mac_free(&DArr->CReadBuf[i]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0117*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0118*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  if(DArr->ReadBlockNumber[i] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0119*/

                    mac_free(&DArr->CReadBuf[i]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(i=0; i < k; i++)
        {  dvm_FreeArray(DArr->CRBlockPtr[i]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }
  }

  /* */    /*E0120*/

  DArr->File = DVM_FILE[0];
  DArr->Line = DVM_LINE[0];

  DArr->CentralPSPtr     = NULL;
  DArr->DArrWBlockPtr    = NULL;
  DArr->WriteBlockNumber = 0;

  DArr->ReadBlockNumber  = NULL;
  DArr->CRBlockPtr       = NULL;

  if(PL->Empty)
  {  /* */    /*E0121*/

     if(DArr->CG == NULL)
     {  /* */    /*E0122*/

        coll_Insert(&DAG->RDA, DArr); /* */    /*E0123*/
           
        DArr->CG = DAG;               /* */    /*E0124*/

        DArr->RealConsist = 0;
        DArr->AllocSign   = 0;

        DArr->CWriteBSize = 0;

        DArr->CWriteBuf   = NULL;
        DArr->CWriteReq   = NULL;

        DArr->CReadBSize  = NULL;
        DArr->CReadBuf    = NULL;
        DArr->CReadReq    = NULL;
        DArr->CReadBlock  = NULL;

        DArr->Line = 0;
        DArr->File = NULL;
     }

     if(RTL_TRACE)
        dvm_trace(ret_inscg_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0125*/
     DVMFTimeFinish(ret_inscg_);
     return  (DVM_RET, 1);
  }

  /* */    /*E0126*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.007: wrong call inscg_\n"
              "(the parallel loop PS is not the current PS;\n"
              "LoopRef=%lx; LoopPSRef=%lx; CurrentPSRef=%lx)\n",
              *LoopRefPtr, (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0127*/

  for(i=0; i < AR; i++)
  {  if(AxisArray[i] < -1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.010: wrong call inscg_\n"
                 "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);

     if(AxisArray[i] > LR)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.011: wrong call inscg_\n"
                 "(AxisArray[%d]=%ld > %d; LoopRef=%lx)\n",
                 i, AxisArray[i], LR, *LoopRefPtr);

     if(AxisArray[i] >= 0)
     {  if(CoeffArray[i] != 0)
        { if(AxisArray[i] == 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 137.012: wrong call inscg_\n"
                      "(AxisArray[%d]=0)\n", i);

          ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Lower +
               ConstArray[i];

          if(ai < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 137.013: wrong call inscg_\n"
                      "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                      " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                      i, CoeffArray[i], AxisArray[i]-1,
                      PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                      *LoopRefPtr);

          if(ai >= DArr->Space.Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 137.014: wrong call inscg_\n"
                      "( (CoeffArray[%d]=%ld) * (LoopInitIndex[%ld]=%ld)"
                      " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                      i, CoeffArray[i], AxisArray[i]-1,
                      PL->Set[AxisArray[i]-1].Lower, i, ConstArray[i],
                      DArr->Space.Size[i], *LoopRefPtr);

          ai = CoeffArray[i] * PL->Set[AxisArray[i]-1].Upper +
               ConstArray[i];

          if(ai < 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 137.015: wrong call inscg_\n"
                      "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                      " + (ConstArray[%d]=%ld) < 0;\nLoopRef=%lx )\n",
                      i, CoeffArray[i], AxisArray[i]-1,
                      PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                      *LoopRefPtr);

          if(ai >= DArr->Space.Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 137.016: wrong call inscg_\n"
                      "( (CoeffArray[%d]=%ld) * (LoopLastIndex[%ld]=%ld)"
                      " + (ConstArray[%d]=%ld) >= %ld;\nLoopRef=%lx )\n",
                      i, CoeffArray[i], AxisArray[i]-1,
                      PL->Set[AxisArray[i]-1].Upper, i, ConstArray[i],
                      DArr->Space.Size[i], *LoopRefPtr);
        }
        else
        {  if(ConstArray[i] < 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 137.017: wrong call inscg_\n"
                       "(ConstArray[%d]=%ld < 0)\n",
                       i, ConstArray[i]);

           if(ConstArray[i] >= DArr->Space.Size[i])
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 137.018: wrong call inscg_\n"
                       "(ConstArray[%d]=%ld >= %ld)\n",
                       i, ConstArray[i], DArr->Space.Size[i]);
        }
     }
  }

  /* */    /*E0128*/

  for(i=0; i < LR; i++)
      AxisAr[i] = 0;

  for(i=0; i < AR; i++)
  {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
        continue;

     if(AxisAr[AxisArray[i]-1])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.019: wrong call inscg_\n"
                 "(AxisArray[%d]=AxisArray[%d]=%ld)\n",
                 AxisAr[AxisArray[i]-1]-1, i, AxisArray[i]);

     AxisAr[AxisArray[i]-1] = i+1;
  }

  /* */    /*E0129*/

  n = DVM_VMS->Space.Rank;

  for(i=0; i < AR; i++)
  {  AxisAr[i] = (int)AxisArray[i];

     if(AxisAr[i] == -1)
     {  ReplCount++; /* */    /*E0130*/
        continue;
     }

     if(AxisAr[i] == 0)
     {  ConstCount++;    /* */    /*E0131*/
        continue;
     }

     for(j=0; j < n; j++)
         if(PL->PLAxis[j] == AxisAr[i])
            break;

     if(j < n && DVM_VMS->Space.Size[j] > 1)
        NormCount++;     /* */    /*E0132*/
     else
     {  ReplCount++;     /* */    /*E0133*/
        AxisAr[i] = -1;
     }
  }

  if(NormCount == 0 || (DArr->CG != NULL && RenewSign == 0) || k == 1)
  {  /* */    /*E0134*/

     if(DArr->CG == NULL)
     {  /* */    /*E0135*/

        coll_Insert(&DAG->RDA, DArr); /* */    /*E0136*/
           
        DArr->CG = DAG;               /* */    /*E0137*/
        DArr->RealConsist = 0;
        DArr->AllocSign   = 0;

        DArr->CWriteBSize = 0;

        DArr->CWriteBuf   = NULL;
        DArr->CWriteReq   = NULL;

        DArr->CReadBSize  = NULL;
        DArr->CReadBuf    = NULL;
        DArr->CReadReq    = NULL;
        DArr->CReadBlock  = NULL;

        DArr->Line = 0;
        DArr->File = NULL; 
     }

     if(RTL_TRACE)
        dvm_trace(ret_inscg_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0138*/
     DVMFTimeFinish(ret_inscg_);
     return  (DVM_RET, 2);
  }

  /* */    /*E0139*/

  if(DArr->CG == NULL)
  {  coll_Insert(&DAG->RDA, DArr); /* */    /*E0140*/
           
     DArr->CG = DAG;               /* */    /*E0141*/
  }

  /* -------------------------- */    /*E0142*/

  dvm_AllocArray(int, k, DArr->CReadBSize);
  dvm_AllocArray(void *, k, DArr->CReadBuf);

  DArr->RealConsist = 1;     /* */    /*E0143*/

#ifdef  _DVM_MPI_
  if(MPIGather)     /* */    /*E0144*/
  {  DArr->CReadReq  = NULL;
     DArr->CWriteReq = NULL;
  }
  else
#endif
  {  dvm_AllocArray(RTL_Request, k, DArr->CReadReq);
     dvm_AllocArray(RTL_Request, k, DArr->CWriteReq);
  }

  /* */    /*E0145*/

  DArr->AllocSign = 1;  /* */    /*E0146*/

  if(IsSynchr == 0)
  {  if(NormCount == 1)
     {  for(j=0; j < AR; j++)
            if(AxisAr[j] > 0)
               break;

        if(j < AR)
        {  for(n=0; n < j; n++)
               if(AxisAr[n] != 0)
                  break;

           if(n == j)
           {  for(m=j+1; m < AR; m++)
                  if(AxisAr[m] >= 0)
                     break;

              if(m == AR)
                 DArr->AllocSign = 0;  /* */    /*E0147*/
           }
        }  
     }
  }

  /* */    /*E0148*/

  if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_inscg_))
  {  if(DArr->AllocSign)
        tprintf("*** Alloc buffer consistent branch\n");
     else
        tprintf("*** Array memory consistent branch\n");
  }

  if(DArr->AllocSign == 0)
  {  DArr->CReadBlock = NULL;

     for(j=0; j < AR; j++)
     {  if(AxisAr[j] == 0)
        {  IndexArray1[j] = ConstArray[j];
           IndexArray2[j] = ConstArray[j];
        }
        else
        {  IndexArray1[j] = DArr->Block.Set[j].Lower;
           IndexArray2[j] = DArr->Block.Set[j].Upper;
        }
     }
  }
  else
  {  dvm_AllocArray(s_BLOCK, k, DArr->CReadBlock);
  }

  /* */    /*E0149*/
  
  PLSpace.Rank = (byte)LR;

  for(i=0; i < LR; i++)
      PLSpace.Size[i] = PL->Set[i].Size;

  PLSet = PLBlock.Set;

  for(i=0; i < k; i++)
  {  Local = GetSpaceLB4Proc(i, AMV, &PLSpace, PL->Align,
                             NULL, &PLBlock);

     if(DVM_VMS->VProc[i].lP == MPS_CurrentProc)
     {  /* */    /*E0150*/

        DArr->CReadBSize[i] = 0;
        DArr->CReadBuf[i]   = NULL;

        if(Local == NULL)
        {  DArr->CWriteBSize = 0;
           DArr->CWriteBuf   = NULL;
           continue;
        }
        
        for(j=0; j < LR; j++)
        {  PLSet[j].Lower += PL->InitIndex[j];
           PLSet[j].Upper += PL->InitIndex[j];
        }

        DArr->DArrBlock = block_Copy(&DArr->Block);
        Local = &DArr->DArrBlock;
        DArrSet = Local->Set;

        for(j=0; j < AR; j++)
        {  if(AxisAr[j] == -1)
              continue;

           if(AxisAr[j] == 0)
           {  DArrSet[j].Lower = ConstArray[j];
              DArrSet[j].Upper = ConstArray[j];
              DArrSet[j].Size  = 1;
              continue;
           }

           ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                ConstArray[j];
           bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                ConstArray[j];

           DArrSet[j].Lower = dvm_min(ai, bi);
           DArrSet[j].Upper = dvm_max(ai, bi);
           DArrSet[j].Size  = DArrSet[j].Upper - DArrSet[j].Lower + 1;
        }

        if(DArr->AllocSign)
        {  /* */    /*E0151*/
           block_GetSize(DArr->CWriteBSize, Local, Step);
           DArr->CWriteBSize *= DArr->TLen;              
           mac_malloc(DArr->CWriteBuf, void *, DArr->CWriteBSize, 0);
        }
        else
        {  /* */    /*E0152*/

           if(AxisAr[AR-1] == 0)
              IndexArray2[AR-1] = ConstArray[AR-1];
           else
              IndexArray2[AR-1] = DArr->Block.Set[AR-1].Upper;

           for(j=0; j < AR; j++)
           {  if(AxisAr[j] <= 0)
                 continue;

              ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                   ConstArray[j];
              bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                   ConstArray[j];

              IndexArray1[j] = dvm_min(ai, bi);
              IndexArray2[j] = dvm_max(ai, bi);
           }

           LocElmAddr(CharPtr, DArr, IndexArray1);
           DArr->CWriteBuf = (void *)CharPtr;
           ai = (uLLng)CharPtr;

           IndexArray2[AR-1]++;
           LocElmAddr(CharPtr, DArr, IndexArray2);
           bi = (uLLng)CharPtr;

           DArr->CWriteBSize = (int)(bi - ai);
        }
     }
     else
     {  /* */    /*E0153*/

        if(Local == NULL)
        {  DArr->CReadBSize[i] = 0;
           DArr->CReadBuf[i]   = NULL;
           continue;
        }

        for(j=0; j < LR; j++)
        {  PLSet[j].Lower += PL->InitIndex[j];
           PLSet[j].Upper += PL->InitIndex[j];
        }

        if(DArr->AllocSign)
        {  /* */    /*E0154*/

           DArr->CReadBlock[i] = block_Copy(&DArr->Block);
           DArrSet = DArr->CReadBlock[i].Set;

           for(j=0; j < AR; j++)
           {  if(AxisAr[j] == -1)
                 continue;

              if(AxisAr[j] == 0)
              {  DArrSet[j].Lower = ConstArray[j];
                 DArrSet[j].Upper = ConstArray[j];
                 DArrSet[j].Size  = 1;
                 continue;
              }

              ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                   ConstArray[j];
              bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                   ConstArray[j];

              DArrSet[j].Lower = dvm_min(ai, bi);
              DArrSet[j].Upper = dvm_max(ai, bi);
              DArrSet[j].Size  = DArrSet[j].Upper - DArrSet[j].Lower + 1;
           }

           block_GetSize(DArr->CReadBSize[i], &DArr->CReadBlock[i],
                         Step);
           DArr->CReadBSize[i] *= DArr->TLen;              
           mac_malloc(DArr->CReadBuf[i], void *, DArr->CReadBSize[i], 0);
        }
        else
        {  /* */    /*E0155*/

           if(AxisAr[AR-1] == 0)
              IndexArray2[AR-1] = ConstArray[AR-1];
           else
              IndexArray2[AR-1] = DArr->Block.Set[AR-1].Upper;

           for(j=0; j < AR; j++)
           {  if(AxisAr[j] <= 0)
                 continue;

              ai = PLSet[AxisAr[j]-1].Lower * CoeffArray[j] +
                   ConstArray[j];
              bi = PLSet[AxisAr[j]-1].Upper * CoeffArray[j] +
                   ConstArray[j];

              IndexArray1[j] = dvm_min(ai, bi);
              IndexArray2[j] = dvm_max(ai, bi);
           }

           LocElmAddr(CharPtr, DArr, IndexArray1);
           DArr->CReadBuf[i] = (void *)CharPtr;
           ai = (uLLng)CharPtr;

           IndexArray2[AR-1]++;
           LocElmAddr(CharPtr, DArr, IndexArray2);
           bi = (uLLng)CharPtr;

           DArr->CReadBSize[i] = (int)(bi - ai);
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_inscg_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0156*/
  DVMFTimeFinish(ret_inscg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  strtcg_(DAConsistGroupRef  *DAConsistGroupRefPtr)
{ SysHandle         *DAGHandlePtr;
  s_DACONSISTGROUP  *DAG;
  s_DISARRAY        *DArr;
  int                i, j, k, n, m, p, q, Step = 0;
  byte               IsSend = 0;
  char              *CharPtr;
  s_BLOCK           *BlockPtr, wBlock;
  DvmType               ai;
  DvmType               IndexArray1[MAXARRAYDIM + 1];

#ifdef  _DVM_MPI_
  void              *sendbuf, *recvbuf;
  int               *rdispls, *sdispls, *sendcounts;
  DvmType           bi;
#endif

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0157*/
  DVMFTimeStart(call_strtcg_);

  /* */    /*E0158*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0159*/

  if(RTL_TRACE)
     dvm_trace(call_strtcg_,
               "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx;\n",
               (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.030: wrong call strtcg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.031: wrong call strtcg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;

  /* */    /*E0160*/

  if(DAG->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.033: wrong call strtcg_\n"
              "(a consistent operation has already been started; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG->ConsistSign = 1;  /* */    /*E0161*/

  k = (int)DVM_VMS->ProcCount;
  q = DAG->RDA.Count;

  for(p=0; p < q; p++)
  {  DArr = coll_At(s_DISARRAY *, &DAG->RDA, p);

     if(DArr->ConsistSign != 0)
        continue;

     DArr->ConsistSign = 1; /* */    /*E0162*/

     if(DArr->RealConsist == 0)
        continue;

     /* */    /*E0163*/

     /* */    /*E0164*/

     if(DArr->CReadBlock != NULL)
     {  /* */    /*E0165*/

        if(DArr->AllocSign)
        {  BlockPtr = &DArr->DArrBlock;
           CharPtr  = (char *)DArr->CWriteBuf;

           wBlock = block_Copy(BlockPtr);

           ai = BlockPtr->Set[DArr->Space.Rank-1].Size;
           if (ai > 0) {
              n  = (int)(ai * DArr->TLen);
              m  = (int)(DArr->CWriteBSize / n);

              for(j=0; j < m; j++)
              {  index_FromBlock1(IndexArray1, &wBlock, BlockPtr)
                 GetLocElm1(DArr, IndexArray1, CharPtr, ai)
                 CharPtr += n;
              }
           }
        }
     }
     else
     {  /* */    /*E0166*/

        if(DArr->AllocSign)
        {  CharPtr = (char *)DArr->CWriteBuf;

           for(j=0; j < DArr->WriteBlockNumber; j++)
           {  BlockPtr = &DArr->DArrWBlockPtr[j];
              wBlock   = block_Copy(BlockPtr);

              ai = BlockPtr->Set[DArr->Space.Rank-1].Size;
              if (ai > 0) {
                 n  = (int)(ai * DArr->TLen);
                 block_GetSize(m, BlockPtr, Step);

                 m  = (int)(m / ai);

                 for(i=0; i < m; i++)
                 {  index_FromBlock1(IndexArray1, &wBlock, BlockPtr)
                    GetLocElm1(DArr, IndexArray1, CharPtr, ai)
                    CharPtr += n;
                 }
              }
           }
        } 
     }

#ifdef  _DVM_MPI_

     if(MPIGather)
     {  /* */    /*E0167*/

        DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0168*/

        if(DArr->CWriteBSize == 0)
           sendbuf = (void *)&AlltoallMem;
        else
           sendbuf = DArr->CWriteBuf;

        /* */    /*E0169*/

        if(DArr->ConsistProcCount != k)
        {  if(DArr->ConsistProcCount != 0)
           {  dvm_FreeArray(DArr->sdispls);
              dvm_FreeArray(DArr->sendcounts);
              dvm_FreeArray(DArr->rdispls);
           }

           DArr->ConsistProcCount = k;
           dvm_AllocArray(int, k, DArr->sdispls);
           dvm_AllocArray(int, k, DArr->sendcounts);
           dvm_AllocArray(int, k, DArr->rdispls);
        }

        sdispls    = DArr->sdispls;
        sendcounts = DArr->sendcounts;
        rdispls    = DArr->rdispls;

        for(i=0; i < k; i++)
        {  sdispls[i] = 0;
           sendcounts[i] = 0;

           j = (int)DVM_VMS->VProc[i].lP;

           if(j == MPS_CurrentProc)
              continue;

           if(DArr->CentralPSPtr != NULL)
           {  m = DArr->WriteBlockNumber;

              for(n=0; n < m; n++)
                  if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                     break;

              if(n == m)
                 continue;  /* */    /*E0170*/
           } 

           sendcounts[i] = DArr->CWriteBSize;
        }

        /* */    /*E0171*/

        ai = DVMTYPE_MAX;    /*E0172*/
        j = -1;

        for(i=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              continue;  /* */    /*E0173*/

           bi = (DvmType)DArr->CReadBuf[i];

           if(bi < ai)
           {  j = i;    /* */    /*E0174*/
              ai = bi;  /* */    /*E0175*/
           }
        }

        if(j < 0)
           recvbuf = (void *)&AlltoallMem;
        else
           recvbuf = DArr->CReadBuf[j];

        /* */    /*E0176*/

        j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0177*/

        for(n=0; n < k; n++)
        {  if(DArr->CReadBSize[n] == 0)
              rdispls[n] = 0;
           else
           {
               bi = (DvmType)DArr->CReadBuf[n] - ai;

              if(bi >= j)
                 break;    /* */    /*E0178*/
              rdispls[n] = (int)bi;
           }
        }

        if(n < k)
        {  /* */    /*E0179*/

           for(i=0,m=0; i < k; i++)
           {  if(DArr->CReadBSize[i] == 0)
                 rdispls[i] = 0;
              else
              {  rdispls[i] = m;
                 m += DArr->CReadBSize[i];
              }
           }

           mac_malloc(recvbuf, void *, m, 0);
        }

        /* */    /*E0180*/

        if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_strtcg_))
        {  if(n < k)
              tprintf("*** MPI_Allgatherv consistent branch\n");
           else
              tprintf("*** MPI_Allgatherv consistent branch (fast)\n");
        }

        /* */    /*E0181*/

        MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_CHAR,
                      recvbuf, DArr->CReadBSize, rdispls, MPI_CHAR,
                      DVM_VMS->PS_MPI_COMM);

        if(n < k)
        {  /* */    /*E0182*/

           CharPtr = (char *)recvbuf;

           for(i=0; i < k; i++)
           {  if(DArr->CReadBSize[i] == 0)
                 continue;

              SYSTEM(memcpy, ((char *)DArr->CReadBuf[i], CharPtr,
                              DArr->CReadBSize[i]))
              CharPtr += DArr->CReadBSize[i];
           }

           mac_free(&recvbuf);
        }

        DVMMTimeFinish;    /* */    /*E0183*/
     }
     else

#endif
     {  /* */    /*E0184*/

        /* */    /*E0185*/

        if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_strtcg_))
           tprintf("*** Point_Point consistent branch\n");

        for(i=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              continue;  /* */    /*E0186*/

           j = (int)DVM_VMS->VProc[i].lP;

           ( RTL_CALL, rtl_Recvnowait(DArr->CReadBuf[i], 1,
                                      DArr->CReadBSize[i], j,
                                      DVM_VMS->tag_DACopy,
                                      &DArr->CReadReq[i], 1) );
        }

        if(DArr->CWriteBSize != 0)
        {  for(i=0; i < k; i++)
           {  j = (int)DVM_VMS->VProc[i].lP;

              if(j == MPS_CurrentProc)
                 continue;

              if(DArr->CentralPSPtr != NULL)
              {  m = DArr->WriteBlockNumber;

                 for(n=0; n < m; n++)
                     if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                        break;

                 if(n == m)
                    continue;  /* */    /*E0187*/
              } 

              IsSend = 1;
 
              ( RTL_CALL, rtl_Sendnowait(DArr->CWriteBuf, 1,
                                         DArr->CWriteBSize, j,
                                         DVM_VMS->tag_DACopy,
                                         &DArr->CWriteReq[i],
                                         a_DA1_EQ_DA1) );
           }
        }
     }
  }

  if(IsSend && MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
  {  /* */    /*E0188*/

     rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffDACopy);
  }

  if(RTL_TRACE)
     dvm_trace(ret_strtcg_," \n");

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0189*/
  DVMFTimeFinish(ret_strtcg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  waitcg_(DAConsistGroupRef  *DAConsistGroupRefPtr)
{ SysHandle         *DAGHandlePtr;
  s_DACONSISTGROUP  *DAG;
  DvmType              *ArrayHeader;
  int                i, j;
  s_DISARRAY        *DArr;

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0190*/
  DVMFTimeStart(call_waitcg_);

  if(RTL_TRACE)
     dvm_trace(call_waitcg_,
               "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx;\n",
               (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.040: wrong call waitcg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.041: wrong call waitcg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;

  /* */    /*E0191*/

  if(DAG->ConsistSign == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.043: wrong call waitcg_\n"
              "(a consistent operation has not been started; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG->ConsistSign = 0;  /* */    /*E0192*/

  j = DAG->RDA.Count;

  for(i=0; i < j; i++)
  {  DArr = coll_At(s_DISARRAY *, &DAG->RDA, i);

     if(DArr->ConsistSign == 0)
        continue;

     ArrayHeader = (DvmType *)DArr->HandlePtr->HeaderPtr;
     ( RTL_CALL, waitac_(ArrayHeader) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_waitcg_," \n");

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0193*/
  DVMFTimeFinish(ret_waitcg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  delcg_(DAConsistGroupRef  *DAConsistGroupRefPtr)
{ SysHandle          *DAGHandlePtr;
  s_DACONSISTGROUP   *DAG;

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0194*/
  DVMFTimeStart(call_delcg_);

  if(RTL_TRACE)
     dvm_trace(call_delcg_,
         "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx;\n",
         (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.050: wrong call delcg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.051: wrong call delcg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;
  
  /* */    /*E0195*/

  if(DAG->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.053: wrong call delcg_\n"
              "(the consistent operation has not been completed; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  ( RTL_CALL, delobj_(DAConsistGroupRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delcg_," \n");

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0196*/
  DVMFTimeFinish(ret_delcg_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------------------ */    /*E0197*/


DvmType  __callstd  consda_(DvmType  ArrayHeader[], AMViewRef  *AMViewRefPtr,
                            DvmType  *ArrayAxis, DvmType  *RenewSignPtr)
{ SysHandle     *ArrayHandlePtr, *AMVHandlePtr;
  s_DISARRAY    *DArr;
  s_AMVIEW      *AMV;
  s_AMS         *AMS, *AMS1;
  int            AR, ARM1, i, j, k, n, m, p, Step = 0, DAAxisM1, Size,
                 RenewSign;
  s_BLOCK       *Local, wBlock;
  DvmType           ai, bi;
  char          *CharPtr;
  DvmType           IndexArray1[MAXARRAYDIM + 1], IndexArray2[MAXARRAYDIM + 1];
  s_REGULARSET  *DArrSet;
  byte           IsSend = 0;

#ifdef  _DVM_MPI_
  void           *sendbuf, *recvbuf;
  int            *rdispls, *sdispls, *sendcounts;
#endif

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0198*/
  DVMFTimeStart(call_consda_);

  /* */    /*E0199*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0200*/

  DAAxisM1  = (int)(*ArrayAxis - 1);
  RenewSign = (int)*RenewSignPtr;

  if(RTL_TRACE)
     dvm_trace(call_consda_,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
               "AMViewRefPtr=%lx; AMViewRef=%lx; ArrayAxis=%d; "
               "RenewSign=%d;\n",
               (uLLng)ArrayHeader, ArrayHeader[0],
               (uLLng)AMViewRefPtr, *AMViewRefPtr, DAAxisM1+1,
               RenewSign);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.051: wrong call consda_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;
  ARM1 = AR - 1;

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.052: wrong call consda_\n"
              "(the replicated DVM array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0201*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.053: wrong call consda_\n"
              "(the replicated DVM array PS is not the current PS;\n"
              "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
              ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0202*/

  if(DArr->Repl == 0 || DArr->Every == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.058: wrong call consda_\n"
              "(the object is not a replicated DVM array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0203*/

  if(DArr->CG != NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.070: wrong call consda_\n"
              "(the replicated DVM array has been inserted "
              "in the consistent group;\n"
              "ArrayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  /* */    /*E0204*/

  if(DArr->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.059: wrong call consda_\n"
              "(a consistent operation has already been started; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0205*/

  if(DAAxisM1 < 0 || DAAxisM1 >= AR)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.054: wrong call consda_\n"
              "(invalid dimension number of the replicated DVM array;\n"
              "ArrayHeader[0]=%lx; ArrayAxis=%d;)\n",
              ArrayHeader[0], DAAxisM1+1);

  DArr->DAAxisM1 = DAAxisM1;

  /* ----------------------------------------------------- */    /*E0206*/

  if(TstObject)
  {  if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 135.055: wrong call consda_\n"
                 "(the abstract machine representation "
                 "is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.056: wrong call consda_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;

  if(AMV->Space.Rank != 1)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.057: wrong call consda_\n"
              "(dimension of the abstract machine representation "
              "is not equal to 1;\n"
              "AMViewRef=%lx; Rank=%d;)\n",
              *AMViewRefPtr, (int)AMV->Space.Rank);

  Size = (int)DArr->Space.Size[DAAxisM1];

  if(AMV->Space.Size[0] < Size)
     Size = AMV->Space.Size[0]; /* */    /*E0207*/

  if(AMV->Space.Size[0] < Size)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.060: wrong call consda_\n"
              "(size of the abstract machine representation "
              "< dimension size of the replicated DVM array;\n"
              "AMViewRef=%lx; AMViewSize=%ld; "
              "ArrayHeader[0]=%lx; ArraySize[%d]=%d)\n",
              *AMViewRefPtr, AMV->Space.Size[0], ArrayHeader[0],
              DAAxisM1, Size);

  /* */    /*E0208*/

  if(AMV->VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 135.061: wrong call consda_ "
              "(the abstract machine representation has "
              "been mapped by the function distr_; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  k = (int)DVM_VMS->ProcCount;

  if(RenewSign == 0)
  {  if(DArr->File != NULL)
     {  if(DArr->Line == DVM_LINE[0])
        {  SYSTEM_RET(i, strcmp, (DVM_FILE[0], DArr->File))
           if(i != 0)
              RenewSign = 1;
        }
        else
           RenewSign = 1;
     }
     else
        RenewSign = 1;
  }

  if(RenewSign != 0 && DArr->File != NULL)
  {  /* */    /*E0209*/

     DArr->RealConsist = 0;
     DArr->CWriteBSize = 0;

     /* */    /*E0210*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     if(DArr->AllocSign != 0)
     {  /* */    /*E0211*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0212*/

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  mac_free(&DArr->CReadBuf[i]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0213*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0214*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  if(DArr->ReadBlockNumber[i] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0215*/

                    mac_free(&DArr->CReadBuf[i]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(i=0; i < k; i++)
        {  dvm_FreeArray(DArr->CRBlockPtr[i]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }
  }

  /* */    /*E0216*/

  DArr->File = DVM_FILE[0];
  DArr->Line = DVM_LINE[0];

  /* */    /*E0217*/

  if(RenewSign)
  {  if(AMV->AMSColl.Count < Size)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 135.062: wrong call consda_\n"
                 "(abstract machine number "
                 "< dimension size of the replicated DVM array;\n"
                 "AMViewRef=%lx; AMNumber=%d; "
                 "ArrayHeader[0]=%lx; ArraySize[%d]=%d)\n",
                 *AMViewRefPtr, AMV->AMSColl.Count, ArrayHeader[0],
                 DAAxisM1, Size);

     for(i=0; i < Size; i++)
     {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

        if(AMS->VMS == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 135.063: wrong call consda_\n"
                    "(the abstract machine has not been mapped; "
                    "AMViewRef=%lx; AMRef=%lx; AMInd=%d)\n",
                    *AMViewRefPtr, (uLLng)AMS->HandlePtr, i);

        NotSubsystem(j, DVM_VMS, AMS->VMS)

        if(j)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 135.064: wrong call consda_\n"
                    "(the abstract machine PS is not a subsystem of "
                    "the current PS;\n"
                    "AMViewRef=%lx; AMRef=%lx; AMInd=%d; AMPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    *AMViewRefPtr, (uLLng)AMS->HandlePtr, i,
                    (uLLng)AMS->VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
     }
  }

  DArr->CReadBlock = NULL;
  DArr->AllocSign  = 1;

  if(k == 1)
  {  /* */    /*E0218*/

     DArr->ConsistSign = 1;
     DArr->RealConsist = 0;
     DArr->AllocSign   = 0;

     DArr->CentralPSPtr     = NULL;
     DArr->DArrWBlockPtr    = NULL;
     DArr->WriteBlockNumber = 0;

     DArr->ReadBlockNumber  = NULL;
     DArr->CRBlockPtr       = NULL;

     DArr->CWriteBSize      = 0;

     DArr->CWriteBuf        = NULL;
     DArr->CWriteReq        = NULL;

     DArr->CReadBSize       = NULL;
     DArr->CReadBuf         = NULL;
     DArr->CReadReq         = NULL;

     DArr->File = NULL;
     DArr->Line = 0;

     if(RTL_TRACE)
        dvm_trace(ret_consda_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0219*/
     DVMFTimeFinish(ret_consda_);
     return  (DVM_RET, 1);
  }

  if(RenewSign)
  {  dvm_AllocArray(int, k, DArr->CReadBSize);
     dvm_AllocArray(void *, k, DArr->CReadBuf);
     dvm_AllocArray(int, k, DArr->ReadBlockNumber);
     dvm_AllocArray(s_BLOCK *, k, DArr->CRBlockPtr);
  }

  DArr->ConsistSign = 1;       /* */    /*E0220*/
  DArr->RealConsist = 1;       /* */    /*E0221*/

#ifdef  _DVM_MPI_
  if(MPIGather)     /* */    /*E0222*/
  {  DArr->CReadReq  = NULL;
     DArr->CWriteReq = NULL;
  }
  else
#endif
  {  if(RenewSign)
     {  dvm_AllocArray(RTL_Request, k, DArr->CReadReq);
        dvm_AllocArray(RTL_Request, k, DArr->CWriteReq);
     }
  }

  if(RenewSign)
  {  /* */    /*E0223*/

     /* */    /*E0224*/
  
     for(i=0; i < k; i++)
     {
        if(DVM_VMS->VProc[i].lP == MPS_CurrentProc)
        {  /* */    /*E0225*/

           DArr->CReadBSize[i]      = 0;
           DArr->CReadBuf[i]        = NULL;
           DArr->ReadBlockNumber[i] = 0;
           DArr->CRBlockPtr[i]      = NULL;

           /* */    /*E0226*/

           DArr->WriteBlockNumber = 0;

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != MPS_CurrentProc)
                 continue;    /* */    /*E0227*/
              j++;
              DArr->WriteBlockNumber++;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              j--;
           }

           /* */    /*E0228*/

           if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_consda_))
              tprintf("*** WriteBlockNumber=%d\n",
                      DArr->WriteBlockNumber);

           DArr->CWriteBSize = 0;

           if(DArr->WriteBlockNumber == 0)
           {  /* */    /*E0229*/

              DArr->CWriteBuf     = NULL;
              DArr->CentralPSPtr  = NULL;
              DArr->DArrWBlockPtr = NULL;
              continue;
           }

           if(DArr->WriteBlockNumber != 1 || DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0230*/

              /* */    /*E0231*/

              dvm_AllocArray(s_BLOCK, DArr->WriteBlockNumber,
                             DArr->DArrWBlockPtr);
              dvm_AllocArray(s_VMS *, DArr->WriteBlockNumber,
                             DArr->CentralPSPtr);

              /* */    /*E0232*/

              n = 0;  /* */    /*E0233*/

              for(j=0; j < Size; j++)
              {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS->VMS->CentralProc != MPS_CurrentProc)
                    continue;    /* */    /*E0234*/

                 DArr->DArrWBlockPtr[n] = block_Copy(&DArr->Block);
                 Local = &DArr->DArrWBlockPtr[n];
                 DArrSet = Local->Set;

                 DArrSet[DAAxisM1].Lower = j; /* */    /*E0235*/
                 j++;

                 for( ; j < Size; j++)
                 {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                    if(AMS1->VMS != AMS->VMS)
                       break;
                 }

                 j--;

                 DArrSet[DAAxisM1].Upper = j;
                 DArrSet[DAAxisM1].Size  = DArrSet[DAAxisM1].Upper -
                                           DArrSet[DAAxisM1].Lower + 1;

                 DArr->CentralPSPtr[n] = AMS->VMS;

                 block_GetSize(m, Local, Step);
                 DArr->CWriteBSize += m;

                 n++;
              }

              DArr->CWriteBSize *= DArr->TLen;

              /* */    /*E0236*/

              mac_malloc(DArr->CWriteBuf, void *, DArr->CWriteBSize, 0);

              /* */    /*E0237*/

              CharPtr = (char *)DArr->CWriteBuf;

              for(j=0; j < DArr->WriteBlockNumber; j++)
              {  Local = &DArr->DArrWBlockPtr[j];
                 wBlock = block_Copy(Local);
                 DArrSet = Local->Set;

                 ai = DArrSet[ARM1].Size;
                 if (ai > 0) {
                    n  = (int)(ai * DArr->TLen);
                    block_GetSize(m, Local, Step);

                    m  = (int)(m / ai);

                    for(p=0; p < m; p++)
                    {  index_FromBlock1(IndexArray1, &wBlock, Local)
                       GetLocElm1(DArr, IndexArray1, CharPtr, ai)
                       CharPtr += n;
                    }
                 }
              }
           }
           else
           {  /* */    /*E0238*/

              dvm_AllocArray(s_VMS *, DArr->WriteBlockNumber,
                             DArr->CentralPSPtr);
              DArr->DArrWBlockPtr = NULL;

              DArrSet = DArr->Block.Set;

              for(j=0; j < AR; j++)
              {  IndexArray1[j] = DArrSet[j].Lower;
                 IndexArray2[j] = DArrSet[j].Upper;
              }

              for(j=0; j < Size; j++)
              {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS->VMS->CentralProc != MPS_CurrentProc)
                    continue;    /* */    /*E0239*/

                 IndexArray1[0] = j;      /* */    /*E0240*/
                 j++;

                 for( ; j < Size; j++)
                 {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                    if(AMS1->VMS != AMS->VMS)
                       break;
                 }

                 IndexArray2[0] = j - 1;  /* */    /*E0241*/
                 DArr->CentralPSPtr[0] = AMS->VMS;
                 break;
              }

              LocElmAddr(CharPtr, DArr, IndexArray1);
              DArr->CWriteBuf = (void *)CharPtr;
              ai = (uLLng)CharPtr;

              IndexArray2[ARM1]++;
              LocElmAddr(CharPtr, DArr, IndexArray2);
              bi = (uLLng)CharPtr;

              DArr->CWriteBSize = (int)(bi - ai);
           }
        }
        else
        {  /* */    /*E0242*/

           /* */    /*E0243*/

           DArr->ReadBlockNumber[i] = 0;
           n = 0;

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
                 continue;    /* */    /*E0244*/
              j++;
              DArr->ReadBlockNumber[i]++;

              if(IsProcInVMS(MPS_CurrentProc, AMS->VMS) < 0)
                 n = 1;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              j--;
           }
        
           /* */    /*E0245*/

           if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_consda_))
              tprintf("*** ReadBlockNumber[%d]=%d; n=%d;\n",
                      i, DArr->ReadBlockNumber[i], n);

           DArr->CReadBSize[i] = 0;

           if(DArr->ReadBlockNumber[i] == 0 || n == 0)
           {  /* */    /*E0246*/

              DArr->CReadBuf[i]        = NULL;
              DArr->ReadBlockNumber[i] = 0;
              DArr->CRBlockPtr[i]      = NULL;
              continue;
           }

           if(DArr->ReadBlockNumber[i] != 1 || DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0247*/

              /* */    /*E0248*/

              dvm_AllocArray(s_BLOCK, DArr->ReadBlockNumber[i],
                             DArr->CRBlockPtr[i]);

              /* */    /*E0249*/

              n = 0;  /* */    /*E0250*/

              for(j=0; j < Size; j++)
              {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
                    continue;    /* */    /*E0251*/

                 DArr->CRBlockPtr[i][n] = block_Copy(&DArr->Block);
                 Local = &DArr->CRBlockPtr[i][n];
                 DArrSet = Local->Set;

                 DArrSet[DAAxisM1].Lower = j; /* */    /*E0252*/
                 j++;

                 for( ; j < Size; j++)
                 {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                    if(AMS1->VMS != AMS->VMS)
                       break;
                 }

                 j--;

                 DArrSet[DAAxisM1].Upper = j;
                 DArrSet[DAAxisM1].Size  = DArrSet[DAAxisM1].Upper -
                                           DArrSet[DAAxisM1].Lower + 1;

                 block_GetSize(m, Local, Step);
                 DArr->CReadBSize[i] += m;

                 n++;
              }

              /* */    /*E0253*/

              DArr->CReadBSize[i] *= DArr->TLen;

              mac_malloc(DArr->CReadBuf[i], void *, DArr->CReadBSize[i],
                         0);
           }
           else
           {  /* */    /*E0254*/

              DArr->CRBlockPtr[i] = NULL;

              DArrSet = DArr->Block.Set;

              for(j=0; j < AR; j++)
              {  IndexArray1[j] = DArrSet[j].Lower;
                 IndexArray2[j] = DArrSet[j].Upper;
              }

              for(j=0; j < Size; j++)
              {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
                    continue;    /* */    /*E0255*/

                 IndexArray1[0] = j;       /* */    /*E0256*/
                 j++;

                 for( ; j < Size; j++)
                 {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                    if(AMS1->VMS != AMS->VMS)
                       break;
                 }

                 IndexArray2[0] = j - 1;   /* */    /*E0257*/
                 break;
              }

              LocElmAddr(CharPtr, DArr, IndexArray1);
              DArr->CReadBuf[i] = (void *)CharPtr;
              ai = (uLLng)CharPtr;

              IndexArray2[ARM1]++;
              LocElmAddr(CharPtr, DArr, IndexArray2);
              bi = (uLLng)CharPtr;

              DArr->CReadBSize[i] = (int)(bi - ai);
           }
        }
     }
  }
  else
  {  /* */    /*E0258*/

     if(DArr->CWriteBSize)
     {
        if(DArr->WriteBlockNumber != 1 || DAAxisM1 != 0 || IsSynchr != 0)
        {
           /* */    /*E0259*/

           CharPtr = (char *)DArr->CWriteBuf;

           for(j=0; j < DArr->WriteBlockNumber; j++)
           {  Local = &DArr->DArrWBlockPtr[j];
              wBlock = block_Copy(Local);
              DArrSet = Local->Set;

              ai = DArrSet[ARM1].Size;
              if (ai > 0) {
                 n  = (int)(ai * DArr->TLen);
                 block_GetSize(m, Local, Step);

                 m  = (int)(m / ai);

                 for(p=0; p < m; p++)
                 {  index_FromBlock1(IndexArray1, &wBlock, Local)
                    GetLocElm1(DArr, IndexArray1, CharPtr, ai)
                    CharPtr += n;
                 }
              }
           }
        }
     }
  }

  /* */    /*E0260*/

#ifdef  _DVM_MPI_

  if(MPIGather)
  {  /* */    /*E0261*/

     DVMMTimeStart(call_MPI_Alltoallv);    /* */    /*E0262*/

     if(DArr->CWriteBSize == 0)
        sendbuf = (void *)&AlltoallMem;
     else
        sendbuf = DArr->CWriteBuf;

     /* */    /*E0263*/

     if(DArr->ConsistProcCount != k)
     {  if(DArr->ConsistProcCount != 0)
        {  dvm_FreeArray(DArr->sdispls);
           dvm_FreeArray(DArr->sendcounts);
           dvm_FreeArray(DArr->rdispls);
        }

        DArr->ConsistProcCount = k;
        dvm_AllocArray(int, k, DArr->sdispls);
        dvm_AllocArray(int, k, DArr->sendcounts);
        dvm_AllocArray(int, k, DArr->rdispls);
     }

     sdispls    = DArr->sdispls;
     sendcounts = DArr->sendcounts;
     rdispls    = DArr->rdispls;

     for(i=0; i < k; i++)
     {  sdispls[i] = 0;
        sendcounts[i] = 0;

        j = (int)DVM_VMS->VProc[i].lP;

        if(j == MPS_CurrentProc)
           continue;

        if(DArr->CentralPSPtr != NULL)
        {  m = DArr->WriteBlockNumber;

           for(n=0; n < m; n++)
               if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                  break;

           if(n == m)
              continue;  /* */    /*E0264*/
        } 

        sendcounts[i] = DArr->CWriteBSize;
     }

     /* */    /*E0265*/

     ai = DVMTYPE_MAX;    /*E0266*/
     j = -1;

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0267*/

        bi = (DvmType)DArr->CReadBuf[i];

        if(bi < ai)
        {  j = i;    /* */    /*E0268*/
           ai = bi;  /* */    /*E0269*/
        }
     }

     if(j < 0)
        recvbuf = (void *)&AlltoallMem;
     else
        recvbuf = DArr->CReadBuf[j];

     /* */    /*E0270*/

     j = INT_MAX;   /*(int)(((word)(-1)) >> 1);*/    /*E0271*/

     for(n=0; n < k; n++)
     {  if(DArr->CReadBSize[n] == 0)
           rdispls[n] = 0;
        else
        {
            bi = (DvmType)DArr->CReadBuf[n] - ai;

           if(bi >= j)
              break;    /* */    /*E0272*/
           rdispls[n] = (int)bi;
        }
     }

     if(n < k)
     {  /* */    /*E0273*/

        for(i=0,m=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              rdispls[i] = 0;
           else
           {  rdispls[i] = m;
              m += DArr->CReadBSize[i];
           }
        }

        mac_malloc(recvbuf, void *, m, 0);
     }

     /* */    /*E0274*/

     if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_consda_))
     {  if(n < k)
           tprintf("*** MPI_Allgatherv consistent branch\n");
        else
           tprintf("*** MPI_Allgatherv consistent branch (fast)\n");
     }

     /* */    /*E0275*/

     MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_CHAR,
                   recvbuf, DArr->CReadBSize, rdispls, MPI_CHAR,
                   DVM_VMS->PS_MPI_COMM);

     if(n < k)
     {  /* */    /*E0276*/

        CharPtr = (char *)recvbuf;

        for(i=0; i < k; i++)
        {  if(DArr->CReadBSize[i] == 0)
              continue;

           SYSTEM(memcpy, ((char *)DArr->CReadBuf[i], CharPtr,
                           DArr->CReadBSize[i]))
           CharPtr += DArr->CReadBSize[i];
        }

        mac_free(&recvbuf);
     }

     DVMMTimeFinish;    /* */    /*E0277*/
  }
  else

#endif
  {  /* */    /*E0278*/

     if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_consda_))
        tprintf("*** Point_Point consistent branch\n");

     for(i=0; i < k; i++)
     {  if(DArr->CReadBSize[i] == 0)
           continue;  /* */    /*E0279*/

        j = (int)DVM_VMS->VProc[i].lP;

        ( RTL_CALL, rtl_Recvnowait(DArr->CReadBuf[i], 1,
                                   DArr->CReadBSize[i], j,
                                   DVM_VMS->tag_DACopy,
                                   &DArr->CReadReq[i], 1) );
     }

     if(DArr->CWriteBSize != 0)
     {  for(i=0; i < k; i++)
        {  j = (int)DVM_VMS->VProc[i].lP;

           if(j == MPS_CurrentProc)
              continue;

           if(DArr->CentralPSPtr != NULL)
           {  m = DArr->WriteBlockNumber;

              for(n=0; n < m; n++)
                  if(IsProcInVMS(j, DArr->CentralPSPtr[n]) < 0 )
                     break;

              if(n == m)
                 continue;  /* */    /*E0280*/
           } 

           IsSend = 1;

           ( RTL_CALL, rtl_Sendnowait(DArr->CWriteBuf, 1,
                                      DArr->CWriteBSize, j,
                                      DVM_VMS->tag_DACopy,
                                      &DArr->CWriteReq[i],
                                      a_DA1_EQ_DA1) );
        }
     }
  }

  if(IsSend && MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
  {  /* */    /*E0281*/

     rtl_TstReqColl(0);
     rtl_SendReqColl(ResCoeffDACopy);
  }

  if(RTL_TRACE)
     dvm_trace(ret_consda_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0282*/
  DVMFTimeFinish(ret_consda_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  inclcg_(DAConsistGroupRef *DAConsistGroupRefPtr,
                            DvmType  ArrayHeader[], AMViewRef  *AMViewRefPtr,
                            DvmType  *ArrayAxisPtr, DvmType  *RenewSignPtr)
{ SysHandle         *ArrayHandlePtr, *DAGHandlePtr, *AMVHandlePtr;
  s_DACONSISTGROUP  *DAG;
  s_DISARRAY        *DArr;
  s_AMVIEW          *AMV;
  s_AMS             *AMS, *AMS1;
  int                AR, ARM1, i, j, k, n, m, DAAxisM1,
                     Size, RenewSign, Step = 0;
  s_BLOCK           *Local;
  DvmType               ai, bi;
  char              *CharPtr;
  DvmType               IndexArray1[MAXARRAYDIM + 1],
                     IndexArray2[MAXARRAYDIM+1];
  s_REGULARSET      *DArrSet;

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0283*/
  DVMFTimeStart(call_inclcg_);

  DAAxisM1  = (int)(*ArrayAxisPtr - 1);
  RenewSign = (int)*RenewSignPtr;

  if(RTL_TRACE)
     dvm_trace(call_inclcg_,
               "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx; "
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
               "AMViewRefPtr=%lx; AMViewRef=%lx; ArrayAxis=%d; "
               "RenewSign=%d;\n",
               (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr,
               (uLLng)ArrayHeader, ArrayHeader[0],
               (uLLng)AMViewRefPtr, *AMViewRefPtr, DAAxisM1+1, RenewSign);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.060: wrong call inclcg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.061: wrong call inclcg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;

  /* */    /*E0284*/

  if(DAG->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.063: wrong call inclcg_\n"
              "(a consistent operation has not been completed; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.065: wrong call inclcg_\n"
              "(the replicated array is not a DVM array;\n"
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;
  ARM1 = AR - 1;

  AMV  = DArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.067: wrong call inclcg_\n"
              "(the replicated DVM array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0285*/

  if(AMV->VMS != DVM_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.068: wrong call inclcg_\n"
              "(the replicated DVM array PS is not the current PS;\n"
              "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
              ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0286*/

  if(DArr->Repl == 0 || DArr->Every == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.069: wrong call inclcg_\n"
              "(the object is not a replicated DVM array; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0287*/

  if(DArr->CG && DArr->CG != DAG)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.070: wrong call inclcg_\n"
              "(the replicated DVM array has already been inserted "
              "in the consistent group;\n"
              "AeeayHeader[0]=%lx; DAConsistGroupRef=%lx)\n",
              ArrayHeader[0], (uLLng)DArr->CG->HandlePtr);

  /* */    /*E0288*/

  if(DArr->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.072: wrong call inclcg_\n"
              "(a consistent operation has not been completed; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  /* */    /*E0289*/

  if(DAAxisM1 < 0 || DAAxisM1 >= AR)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.074: wrong call inclcg_\n"
              "(invalid dimension number of the replicated DVM array;\n"
              "ArrayHeader[0]=%lx; ArrayAxis=%d;)\n",
              ArrayHeader[0], DAAxisM1+1);

  DArr->DAAxisM1 = DAAxisM1;

  /* ----------------------------------------------------- */    /*E0290*/

  if(TstObject)
  {  if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.080: wrong call inclcg_\n"
                 "(the abstract machine representation "
                 "is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.081: wrong call inclcg_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;

  if(AMV->Space.Rank != 1)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.083: wrong call inclcg_\n"
              "(dimension of the abstract machine representation "
              "is not equal to 1;\n"
              "AMViewRef=%lx; Rank=%d;)\n",
              *AMViewRefPtr, (int)AMV->Space.Rank);

  Size = (int)DArr->Space.Size[DAAxisM1];

  if(AMV->Space.Size[0] < Size)
     Size = AMV->Space.Size[0]; /* */    /*E0291*/ 

  if(AMV->Space.Size[0] < Size)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.085: wrong call inclcg_\n"
              "(size of the abstract machine representation "
              "< dimension size of the replicated DVM array;\n"
              "AMViewRef=%lx; AMViewSize=%ld; "
              "ArrayHeader[0]=%lx; ArraySize[%d]=%d)\n",
              *AMViewRefPtr, AMV->Space.Size[0], ArrayHeader[0],
              DAAxisM1, Size);

  /* */    /*E0292*/

  if(AMV->VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.087: wrong call inclcg_ "
              "(the abstract machine representation has "
              "been mapped by the function distr_; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  /* */    /*E0293*/

  if(AMV->AMSColl.Count < Size)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.088: wrong call inclcg_\n"
              "(abstract machine number "
              "< dimension size of the replicated DVM array;\n"
              "AMViewRef=%lx; AMNumber=%d; "
              "ArrayHeader[0]=%lx; ArraySize[%d]=%d)\n",
              *AMViewRefPtr, AMV->AMSColl.Count, ArrayHeader[0],
              DAAxisM1, Size);

  for(i=0; i < Size; i++)
  {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

     if(AMS->VMS == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.090: wrong call inclcg_\n"
                 "(the abstract machine has not been mapped; "
                 "AMViewRef=%lx; AMRef=%lx; AMInd=%d)\n",
                 *AMViewRefPtr, (uLLng)AMS->HandlePtr, i);

     NotSubsystem(j, DVM_VMS, AMS->VMS)

     if(j)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.091: wrong call inclcg_\n"
                 "(the abstract machine PS is not a subsystem of "
                 "the current PS;\n"
                 "AMViewRef=%lx; AMRef=%lx; AMInd=%d; AMPSRef=%lx; "
                 "CurrentPSRef=%lx)\n",
                 *AMViewRefPtr, (uLLng)AMS->HandlePtr, i,
                 (uLLng)AMS->VMS->HandlePtr, (uLLng)DVM_VMS->HandlePtr);
  }

  k = (int)DVM_VMS->ProcCount;

  if(RenewSign == 0)
  {  if(DArr->File != NULL)
     {  if(DArr->Line == DVM_LINE[0])
        {  SYSTEM_RET(i, strcmp, (DVM_FILE[0], DArr->File))
           if(i != 0)
              RenewSign = 1;
        }
        else
           RenewSign = 1;
     }
     else
        RenewSign = 1;
  }

  if( (DArr->CG == NULL && DArr->File != NULL) ||
      (DArr->CG != NULL && RenewSign != 0) )
  {  /* */    /*E0294*/

     DArr->RealConsist = 0;
     DArr->CWriteBSize = 0;

     /* */    /*E0295*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     if(DArr->AllocSign != 0)
     {  /* */    /*E0296*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0297*/

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  mac_free(&DArr->CReadBuf[i]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0298*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0299*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(i=0; i < k; i++)
              {  if(DArr->ReadBlockNumber[i] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0300*/

                    mac_free(&DArr->CReadBuf[i]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(i=0; i < k; i++)
        {  dvm_FreeArray(DArr->CRBlockPtr[i]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }
  }

  /* */    /*E0301*/

  DArr->File = DVM_FILE[0];
  DArr->Line = DVM_LINE[0];

  DArr->CReadBlock = NULL;
  DArr->AllocSign  = 1;

  if(k == 1 || (DArr->CG != NULL && RenewSign == 0))
  {  /* */    /*E0302*/

     if(DArr->CG == NULL)
     {  /* */    /*E0303*/

        coll_Insert(&DAG->RDA, DArr); /* */    /*E0304*/
           
        DArr->CG = DAG;               /* */    /*E0305*/
        DArr->RealConsist = 0;
        DArr->AllocSign   = 0;

        DArr->CentralPSPtr     = NULL;
        DArr->DArrWBlockPtr    = NULL;
        DArr->WriteBlockNumber = 0;

        DArr->ReadBlockNumber  = NULL;
        DArr->CRBlockPtr       = NULL;

        DArr->CWriteBSize      = 0;

        DArr->CWriteBuf        = NULL;
        DArr->CWriteReq        = NULL;

        DArr->CReadBSize       = NULL;
        DArr->CReadBuf         = NULL;
        DArr->CReadReq         = NULL;

        DArr->File = NULL;
        DArr->Line = 0;
     }

     if(RTL_TRACE)
        dvm_trace(ret_inclcg_," \n");

     StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0306*/
     DVMFTimeFinish(ret_inclcg_);
     return  (DVM_RET, 1);
  }

  /* */    /*E0307*/

  if(DArr->CG == NULL)
  {  coll_Insert(&DAG->RDA, DArr); /* */    /*E0308*/
           
     DArr->CG = DAG;               /* */    /*E0309*/
  }

  /* -------------------------- */    /*E0310*/

  dvm_AllocArray(int, k, DArr->CReadBSize);
  dvm_AllocArray(void *, k, DArr->CReadBuf);
  dvm_AllocArray(int, k, DArr->ReadBlockNumber);
  dvm_AllocArray(s_BLOCK *, k, DArr->CRBlockPtr);

  DArr->RealConsist = 1;     /* */    /*E0311*/

#ifdef  _DVM_MPI_
  if(MPIGather)     /* */    /*E0312*/
  {  DArr->CReadReq  = NULL;
     DArr->CWriteReq = NULL;
  }
  else
#endif
  {  dvm_AllocArray(RTL_Request, k, DArr->CReadReq);
     dvm_AllocArray(RTL_Request, k, DArr->CWriteReq);
  }

  /* */    /*E0313*/
  
  for(i=0; i < k; i++)
  {
     if(DVM_VMS->VProc[i].lP == MPS_CurrentProc)
     {  /* */    /*E0314*/

        DArr->CReadBSize[i]      = 0;
        DArr->CReadBuf[i]        = NULL;
        DArr->ReadBlockNumber[i] = 0;
        DArr->CRBlockPtr[i]      = NULL;

        /* */    /*E0315*/

        DArr->WriteBlockNumber = 0;

        for(j=0; j < Size; j++)
        {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

           if(AMS->VMS->CentralProc != MPS_CurrentProc)
              continue;    /* */    /*E0316*/
           j++;
           DArr->WriteBlockNumber++;

           for( ; j < Size; j++)
           {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS1->VMS != AMS->VMS)
                 break;
           }

           j--;
        }

        /* */    /*E0317*/

        if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_inclcg_))
           tprintf("*** WriteBlockNumber=%d\n", DArr->WriteBlockNumber);

        DArr->CWriteBSize = 0;

        if(DArr->WriteBlockNumber == 0)
        {  /* */    /*E0318*/

           DArr->CWriteBuf     = NULL;
           DArr->CentralPSPtr  = NULL;
           DArr->DArrWBlockPtr = NULL;
           continue;
        }

        if(DArr->WriteBlockNumber != 1 || DAAxisM1 != 0 || IsSynchr != 0)
        {  /* */    /*E0319*/

           /* */    /*E0320*/

           dvm_AllocArray(s_BLOCK, DArr->WriteBlockNumber,
                          DArr->DArrWBlockPtr);
           dvm_AllocArray(s_VMS *, DArr->WriteBlockNumber,
                          DArr->CentralPSPtr);

           /* */    /*E0321*/

           n = 0;  /* */    /*E0322*/

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != MPS_CurrentProc)
                 continue;    /* */    /*E0323*/

              DArr->DArrWBlockPtr[n] = block_Copy(&DArr->Block);
              Local = &DArr->DArrWBlockPtr[n];
              DArrSet = Local->Set;

              DArrSet[DAAxisM1].Lower = j; /* */    /*E0324*/
              j++;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              j--;

              DArrSet[DAAxisM1].Upper = j;
              DArrSet[DAAxisM1].Size  = DArrSet[DAAxisM1].Upper -
                                        DArrSet[DAAxisM1].Lower + 1;

              DArr->CentralPSPtr[n] = AMS->VMS;

              block_GetSize(m, Local, Step);
              DArr->CWriteBSize += m;

              n++;
           }

           DArr->CWriteBSize *= DArr->TLen;

           /* */    /*E0325*/

           mac_malloc(DArr->CWriteBuf, void *, DArr->CWriteBSize, 0);
        }
        else
        {  /* */    /*E0326*/

           dvm_AllocArray(s_VMS *, DArr->WriteBlockNumber,
                             DArr->CentralPSPtr);
           DArr->DArrWBlockPtr = NULL;

           DArrSet = DArr->Block.Set;

           for(j=0; j < AR; j++)
           {  IndexArray1[j] = DArrSet[j].Lower;
              IndexArray2[j] = DArrSet[j].Upper;
           }

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != MPS_CurrentProc)
                 continue;    /* */    /*E0327*/

              IndexArray1[0] = j;      /* */    /*E0328*/
              j++;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              IndexArray2[0] = j - 1;  /* */    /*E0329*/
              DArr->CentralPSPtr[0] = AMS->VMS;
              break;
           }

           LocElmAddr(CharPtr, DArr, IndexArray1);
           DArr->CWriteBuf = (void *)CharPtr;
           ai = (uLLng)CharPtr;

           IndexArray2[ARM1]++;
           LocElmAddr(CharPtr, DArr, IndexArray2);
           bi = (uLLng)CharPtr;

           DArr->CWriteBSize = (int)(bi - ai);
        }
     }
     else
     {  /* */    /*E0330*/

        /* */    /*E0331*/

        DArr->ReadBlockNumber[i] = 0;
        n = 0;

        for(j=0; j < Size; j++)
        {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

           if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
              continue;    /* */    /*E0332*/
           j++;
           DArr->ReadBlockNumber[i]++;

           if(IsProcInVMS(MPS_CurrentProc, AMS->VMS) < 0)
              n = 1;

           for( ; j < Size; j++)
           {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS1->VMS != AMS->VMS)
                 break;
           }

           j--;
        }
        
        /* */    /*E0333*/

        if(RTL_TRACE && DAConsistTrace && TstTraceEvent(call_inclcg_))
           tprintf("*** ReadBlockNumber[%d]=%d; n=%d;\n",
                   i, DArr->ReadBlockNumber[i], n);

        DArr->CReadBSize[i] = 0;

        if(DArr->ReadBlockNumber[i] == 0 || n == 0)
        {  /* */    /*E0334*/

           DArr->CReadBuf[i]        = NULL;
           DArr->ReadBlockNumber[i] = 0;
           DArr->CRBlockPtr[i]      = NULL;
           continue;
        }

        /* */    /*E0335*/

        if(DArr->ReadBlockNumber[i] != 1 || DAAxisM1 != 0 ||
           IsSynchr != 0)
        {  /* */    /*E0336*/

           dvm_AllocArray(s_BLOCK, DArr->ReadBlockNumber[i],
                          DArr->CRBlockPtr[i]);

           /* */    /*E0337*/

           n = 0;  /* */    /*E0338*/

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
                 continue;    /* */    /*E0339*/

              DArr->CRBlockPtr[i][n] = block_Copy(&DArr->Block);
              Local = &DArr->CRBlockPtr[i][n];
              DArrSet = Local->Set;

              DArrSet[DAAxisM1].Lower = j; /* */    /*E0340*/
              j++;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              j--;

              DArrSet[DAAxisM1].Upper = j;
              DArrSet[DAAxisM1].Size  = DArrSet[DAAxisM1].Upper -
                                        DArrSet[DAAxisM1].Lower + 1;

              block_GetSize(m, Local, Step);
              DArr->CReadBSize[i] += m;

              n++;
           }

           /* */    /*E0341*/

           DArr->CReadBSize[i] *= DArr->TLen;

           mac_malloc(DArr->CReadBuf[i], void *, DArr->CReadBSize[i], 0);
        }
        else
        {  /* */    /*E0342*/

           DArr->CRBlockPtr[i] = NULL;

           DArrSet = DArr->Block.Set;

           for(j=0; j < AR; j++)
           {  IndexArray1[j] = DArrSet[j].Lower;
              IndexArray2[j] = DArrSet[j].Upper;
           }

           for(j=0; j < Size; j++)
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

              if(AMS->VMS->CentralProc != DVM_VMS->VProc[i].lP)
                 continue;    /* */    /*E0343*/

              IndexArray1[0] = j;       /* */    /*E0344*/
              j++;

              for( ; j < Size; j++)
              {  AMS1 = coll_At(s_AMS *, &AMV->AMSColl, j);

                 if(AMS1->VMS != AMS->VMS)
                    break;
              }

              IndexArray2[0] = j - 1;   /* */    /*E0345*/
              break;
           }

           LocElmAddr(CharPtr, DArr, IndexArray1);
           DArr->CReadBuf[i] = (void *)CharPtr;
           ai = (uLLng)CharPtr;

           IndexArray2[ARM1]++;
           LocElmAddr(CharPtr, DArr, IndexArray2);
           bi = (uLLng)CharPtr;

           DArr->CReadBSize[i] = (int)(bi - ai);
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_inclcg_," \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0];    /* */    /*E0346*/
  DVMFTimeFinish(ret_inclcg_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------------------ */    /*E0347*/


DvmType  __callstd  rstcg_(DAConsistGroupRef  *DAConsistGroupRefPtr, DvmType  *DelDASignPtr)
{ SysHandle          *DAGHandlePtr, *NewDAGHandlePtr;
  s_DACONSISTGROUP   *DAG;
  DvmType                StaticSign, DelDASign;

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0348*/
  DVMFTimeStart(call_rstcg_);

  if(RTL_TRACE)
     dvm_trace(call_rstcg_,
               "DAConsistGroupRefPtr=%lx; DAConsistGroupRef=%lx; "
               "DelDASign=%ld;\n",
               (uLLng)DAConsistGroupRefPtr, *DAConsistGroupRefPtr,
               *DelDASignPtr);

  DAGHandlePtr = (SysHandle *)*DAConsistGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(DAConsistGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 137.055: wrong call rstcg_\n"
                 "(the consistent group is not a DVM object; "
                 "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);
  }

  if(DAGHandlePtr->Type != sht_DAConsistGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.056: wrong call rstcg_\n"
              "(the object is not a consistent group;\n"
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  DAG = (s_DACONSISTGROUP *)DAGHandlePtr->pP;
  
  /* */    /*E0349*/

  if(DAG->ConsistSign)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 137.058: wrong call rstcg_\n"
              "(a consistent operation has not been completed; "
              "DAConsistGroupRef=%lx)\n", *DAConsistGroupRefPtr);

  StaticSign = DAG->Static;
  DelDASign  = DAG->DelDA;

  DAG->ResetSign = 1;   /* */    /*E0350*/
  DAG->DelDA    = (byte)*DelDASignPtr;

  ( RTL_CALL, delobj_(DAConsistGroupRefPtr) );

  NewDAGHandlePtr = (SysHandle *)
                    ( RTL_CALL, crtrg_(&StaticSign, &DelDASign) );

  DAG = (s_DACONSISTGROUP *)NewDAGHandlePtr->pP;
  DAGHandlePtr->pP = NewDAGHandlePtr->pP;
  DAG->HandlePtr = DAGHandlePtr;

  NewDAGHandlePtr->Type = sht_NULL;
  dvm_FreeStruct(NewDAGHandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_rstcg_," \n");

  StatObjectRef = (ObjectRef)*DAConsistGroupRefPtr; /* */    /*E0351*/
  DVMFTimeFinish(ret_rstcg_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------------------ */    /*E0352*/


void   DAConsistGroup_Done(s_DACONSISTGROUP  *DAG)
{ int                     i, j, k;
  s_DISARRAY             *DArr;
  DAConsistGroupRef       DAGrpRef;
  DvmType                   *ArrayHeader;

  if(RTL_TRACE)
     dvm_trace(call_DAConsistGroup_Done, "DAConsistGroupRef=%lx;\n",
                                         (uLLng)DAG->HandlePtr);

  if(DAG->ConsistSign)
  {  DAGrpRef = (DAConsistGroupRef)DAG->HandlePtr;
     ( RTL_CALL, waitcg_(&DAGrpRef) );
  }

  /* */    /*E0353*/

  for(i=0; i < DAG->RDA.Count; i++)
  {  DArr = coll_At(s_DISARRAY *, &DAG->RDA, i);
     DArr->CG = NULL;   /* */    /*E0354*/
     ArrayHeader = (DvmType *)DArr->HandlePtr->HeaderPtr;

     if(DArr->ConsistSign)
        ( RTL_CALL, waitac_(ArrayHeader) );

     /* */    /*E0355*/

     DArr->RealConsist = 0;
     DArr->CWriteBSize = 0;
     DArr->Line        = 0;
     DArr->File        = NULL; 

     /* */    /*E0356*/

     dvm_FreeArray(DArr->CReadReq); 
     dvm_FreeArray(DArr->CWriteReq); 
     dvm_FreeArray(DArr->CReadBSize);

     dvm_FreeArray(DArr->CentralPSPtr);
     dvm_FreeArray(DArr->DArrWBlockPtr);

     j = (int)DArr->AMView->VMS->ProcCount;

     if(DArr->AllocSign != 0)
     {  /* */    /*E0357*/

        if(DArr->CReadBlock != NULL)
        {  /* */    /*E0358*/

           if(DArr->CReadBuf != NULL)
           {  for(k=0; k < j; k++)
              {  mac_free(&DArr->CReadBuf[k]);
              }
           }

           mac_free(&DArr->CWriteBuf);
           dvm_FreeArray(DArr->CReadBlock);
        }
        else
        {  /* */    /*E0359*/

           if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
              IsSynchr != 0)
           {  /* */    /*E0360*/

              mac_free(&DArr->CWriteBuf);
           }

           if(DArr->CReadBuf != NULL)
           {  for(k=0; k < j; k++)
              {  if(DArr->ReadBlockNumber[k] != 1 ||
                    DArr->DAAxisM1 != 0 || IsSynchr != 0)
                 {  /* */    /*E0361*/

                    mac_free(&DArr->CReadBuf[k]);
                 }
              }
           }
        }
     }

     dvm_FreeArray(DArr->CReadBuf);
     dvm_FreeArray(DArr->ReadBlockNumber);

     DArr->WriteBlockNumber =  0;
     DArr->DAAxisM1         = -1;
     DArr->AllocSign        =  0;

     if(DArr->CRBlockPtr != NULL)
     {  for(k=0; k < j; k++)
        {  dvm_FreeArray(DArr->CRBlockPtr[k]);
        }

        dvm_FreeArray(DArr->CRBlockPtr);
     }

     if(DAG->DelDA)
     {  if(DelObjFlag)   /* */    /*E0362*/
           ( RTL_CALL, delda_(ArrayHeader) );
        else             /* */    /*E0363*/
        {  if(DArr->Static == 0)
              ( RTL_CALL, delda_(ArrayHeader) );
        }
     }
  }

  dvm_FreeArray(DAG->RDA.List);

  if(TstObject)
     DelDVMObj((ObjectRef)DAG->HandlePtr);

  /* */    /*E0364*/

  if(DAG->ResetSign == 0)
  {  /* */    /*E0365*/

     DAG->HandlePtr->Type = sht_NULL;
     dvm_FreeStruct(DAG->HandlePtr);
  }

  DAG->ResetSign = 0;

  if(RTL_TRACE)
     dvm_trace(ret_DAConsistGroup_Done, " \n");

  (DVM_RET);

  return;
}


#endif  /*  _DACONSIS_C_  */    /*E0366*/
