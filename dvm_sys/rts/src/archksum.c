#ifndef _ARCHKSUM_C_
#define _ARCHKSUM_C_
/******************/


DvmType  __callstd dacsum_(DvmType  ArrayHeader[], double  *CheckSumPtr)
{ SysHandle        *ArrayHandlePtr;
  s_DISARRAY       *DArr;
  s_VMS            *VMS;
  int               i;
  CHECKSUM         *StructPtr;
  dvm_ARRAY_INFO   *pInfo;
  DvmType              Res;

  StatObjectRef = (ObjectRef)ArrayHeader[0];
  DVMFTimeStart(call_dacsum_);

  if(RTL_TRACE)
     dvm_trace(call_dacsum_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 120.100: wrong call dacsum_\n"
       "(the object is not a distributed array; ArrayHeader[0]=%lx)\n",
       ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 120.101: wrong call dacsum_\n"
              "(the array has not been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  VMS = DArr->AMView->VMS;

  NotSubsystem(i, DVM_VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 120.102: wrong call dacsum_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           ArrayHeader[0], (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

  dvm_AllocStruct(CHECKSUM, StructPtr);
  dvm_AllocStruct(dvm_ARRAY_INFO, pInfo);

  StructPtr->pInfo      = (void *)pInfo;
  StructPtr->sum        = 0.;
  StructPtr->errCode = 0;

  pInfo->pAddr     = (void *)ArrayHandlePtr;
  pInfo->bIsDistr  = 1;
  pInfo->lElemType = DArr->Type;

  cs_compute(StructPtr, 1);

  Res = StructPtr->errCode;

  if(Res)
  {  DArr->IsCheckSum = 1;
     DArr->CheckSum   = StructPtr->sum;
     *CheckSumPtr     = StructPtr->sum;
  }
  else
  {  DArr->IsCheckSum = 0;
     DArr->CheckSum   = 0.;
     *CheckSumPtr     = 0.;
  }

  dvm_FreeStruct(pInfo);
  dvm_FreeStruct(StructPtr);

  if(RTL_TRACE)
     dvm_trace(ret_dacsum_,"Calculated=%ld\n", Res);

  StatObjectRef = (ObjectRef)ArrayHeader[0];
  DVMFTimeFinish(ret_dacsum_);
  return  (DVM_RET, Res);
}



DvmType  __callstd getdas_(DvmType  ArrayHeader[], double  *CheckSumPtr)
{ SysHandle        *ArrayHandlePtr;
  s_DISARRAY       *DArr;
  DvmType              Res;

  StatObjectRef = (ObjectRef)ArrayHeader[0];
  DVMFTimeStart(call_getdas_);

  if(RTL_TRACE)
     dvm_trace(call_getdas_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 120.105: wrong call getdas_\n"
       "(the object is not a distributed array; ArrayHeader[0]=%lx)\n",
       ArrayHeader[0]);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  Res = DArr->IsCheckSum;

  if(Res)
     *CheckSumPtr = DArr->CheckSum;
  else
     *CheckSumPtr = 0.;

  if(RTL_TRACE)
     dvm_trace(ret_getdas_,"Calculated=%ld\n", Res);

  StatObjectRef = (ObjectRef)ArrayHeader[0];
  DVMFTimeFinish(ret_getdas_);
  return  (DVM_RET, Res);
}



DvmType  __callstd arcsum_(void  *MemPtr, DvmType  *RankPtr, DvmType  SizeArray[],
                           DvmType  *TypePtr, double  *CheckSumPtr)
{
  int               i, Rank, Type = -1;
  CHECKSUM         *StructPtr;
  dvm_ARRAY_INFO   *pInfo;
  DvmType              Res;

  DVMFTimeStart(call_arcsum_);

  Rank = (int)*RankPtr;
  Type = (int)*TypePtr;

  if(RTL_TRACE)
  {  dvm_trace(call_arcsum_,"MemPtr=%lx; Rank=%d; Type=%d;\n",
                            (uLLng)MemPtr, Rank, Type);

     if(TstTraceEvent(call_arcsum_))
     {  for(i=0; i < Rank; i++)
            tprintf("SizeArray[%d]=%ld; ", i, SizeArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(Rank < 1 || Rank > 7)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 120.110: wrong call arcsum_ "
              "(invalid array rank (=%d))\n", Rank);

  switch(Type)
  {  case rt_CHAR:            break;
     case rt_INT:             break;
     case rt_LOGICAL :        break;
     case rt_LONG:            break;
     case rt_LLONG:           break;
     case rt_FLOAT:           break;
     case rt_DOUBLE:          break;
     case rt_FLOAT_COMPLEX:   break;
     case rt_DOUBLE_COMPLEX:  break;

     default:
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 120.111: wrong call arcsum_ "
                       "(invalid type of array element (=%d))\n", Type);
  }

  for(i=0; i < Rank; i++)
      if(SizeArray[i] < 1)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 120.112: wrong call arcsum_ "
                  "(invalid size of array dimension %d(=%ld))\n",
                  i, SizeArray[i]);

  for(i=0,Res=1; i < Rank; i++)
      Res *= SizeArray[i];

  dvm_AllocStruct(CHECKSUM, StructPtr);
  dvm_AllocStruct(dvm_ARRAY_INFO, pInfo);

  StructPtr->pInfo      = pInfo;
  StructPtr->sum        = 0.;
  StructPtr->errCode = 0;

  pInfo->pAddr     = MemPtr;
  pInfo->bIsDistr  = 0;
  pInfo->lElemType = Type;
  pInfo->lLineSize = Res;

  cs_compute(StructPtr, 1);

  Res = StructPtr->errCode;

  if(Res)
     *CheckSumPtr = StructPtr->sum;
  else
     *CheckSumPtr = 0.;

  dvm_FreeStruct(pInfo);
  dvm_FreeStruct(StructPtr);

  if(RTL_TRACE)
     dvm_trace(ret_arcsum_,"Calculated=%ld\n", Res);

  DVMFTimeFinish(ret_arcsum_);
  return  (DVM_RET, Res);
}



DvmType  __callstd arcsf_(AddrType  *MemAddrPtr, DvmType  *RankPtr,
                         DvmType  SizeArray[], DvmType  *TypePtr,
                         double  *CheckSumPtr)
{
  return  arcsum_((void *)*MemAddrPtr, RankPtr, SizeArray, TypePtr,
                  CheckSumPtr);
}


/* ------------------------------------------------------ */


void   cs_compute(CHECKSUM  *arrays, int  count)
{
  double           *CheckSumArray;
  CHECKSUM         *StructPtr;
  dvm_ARRAY_INFO   *pInfo;
  double            CheckSum, Coeff;
  int               i, j, k, m, n, Step = 0;
  char             *CharPtr;
  int              *IntPtr;
  DvmType             *LongPtr;
  float            *FloatPtr;
  double           *DoublePtr;
  SysHandle        *ArrayHandlePtr;
  s_DISARRAY       *DArr;
  s_VMS            *VMS;
  s_BLOCK          *Local, wBlock;
  DvmType              IndexArray[MAXARRAYDIM + 1];
  RTL_Request      *Req, OutReq;
  double          **ProcCheckSum;
 // int               DistrVector[MAXARRAYDIM];

#ifdef _DVM_MPI_

  double  *DoublePtr1, *DoublePtr2;

#endif

/*  pprintf(3, " ENTERING\n ");*/

  /* ----------------------------------------------- */

  DVM_VMS->tag_common++;

  if((DVM_VMS->tag_common - (msg_common)) >= TagCount)
     DVM_VMS->tag_common = msg_common;

  /* ----------------------------------------------- */

  dvm_AllocArray(double, count, CheckSumArray);

  for(i=0; i < count; i++)
  {  CheckSumArray[i] = 0.;      /* local checksum of the current
                                    distributed array */

     StructPtr = &arrays[i];     /* pointer to the current
                                    CHECKSUM-struct */

     StructPtr->sum        = 0.;
     StructPtr->errCode = 0;  /* impossible to calculate */

     pInfo = (dvm_ARRAY_INFO *)StructPtr->pInfo;

     ArrayHandlePtr = (SysHandle *)pInfo->pAddr;

     /* Check if the object is an array */

     if(ArrayHandlePtr == NULL)
        continue;     /* the object is not an array */

     if(pInfo->bIsDistr)
     {  /* The array is a distributed array */

        if(ArrayHandlePtr->Type != sht_DisArray)
           continue;     /* the object is not a distributed array */

        if(ArrayHandlePtr !=
           TstDVMArray((void *)ArrayHandlePtr->HeaderPtr))
           continue;     /* the object is not a distributed array */

        DArr = (s_DISARRAY *)ArrayHandlePtr->pP; /* descriptor
                                                    of the disributed
                                                    array */

        if(DArr->AMView == NULL)
           continue;     /* the distributed array has not been aligned */

        VMS = DArr->AMView->VMS;      /* processor system
                                         of the disributed array */

        NotSubsystem(j, DVM_VMS, VMS)
        if(j)
           continue;     /* the array PS is not a subsystem
                            of the current PS */
     }

     StructPtr->errCode = 1;  /* calculated */

     CheckSum = 0.;     /* local checksum of the current
                           distributed array */

     if(pInfo->bIsDistr)
     {  /* The array is a distributed array */

        if(DArr->HasLocal) /* if the disributed array has
                              a local section */
        {
            Local  = &DArr->Block;
            k = Local->Rank;

            if ( pInfo->lLineSize <= 0)
                DBG_ASSERT(__FILE__, __LINE__, 0);
/*
   This approach of calculating distributed array size
   returns incorrect size in case of partially distributed array.
   Will use array linesize calculated by debugger instead.

            for(j=0; j < MAXARRAYDIM; j++)
               DistrVector[j] = 0;

            for(j=0; j < k; j++)
            {  n = GetArMapDim(DArr, j+1, &m);

                if(n != 0)
                    DistrVector[n-1] = 1;
            }

            n = VMS->Space.Rank;
            m = 1;

            for(j=0; j < n; j++)
                if(DistrVector[j] == 0)
                    m *= (int)VMS->Space.Size[j];

            Coeff = 1. / (double)space_GetSize(&DArr->Space);
            Coeff /= (double)m;
*/
           /* Local checksum calculation */

           Coeff = 1. / (double) pInfo->lLineSize;

           wBlock = block_Copy(Local);

           block_GetSize(m, Local, Step);
           n = (int)Local->Set[k - 1].Size;
           m /= n;

           switch(DArr->Type)
           {
              default: /* array element type has not been defined */

                       switch (pInfo->lElemType)
                       {  case rt_INT :
                          case rt_LOGICAL :

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             IntPtr = (int *)CharPtr;

                             for(k=0; k < n; k++,IntPtr++)
                                 CheckSum += (*IntPtr * Coeff);
                          }

                          break;

                          case rt_LONG :                         

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock, Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             long *LongPtr = (long *)CharPtr;

                             for(k=0; k < n; k++,LongPtr++)
                                 CheckSum += (double)(*LongPtr * Coeff);
                          }

                          break;
                          case rt_LLONG:

                              for (j = 0; j < m; j++)
                              {
                                  index_FromBlock1(IndexArray, &wBlock, Local)
                                      LocElmAddr(CharPtr, DArr, IndexArray)
                                      long long *LongPtr = (long long *)CharPtr;

                                  for (k = 0; k < n; k++, LongPtr++)
                                      CheckSum += (double)(*LongPtr * Coeff);
                              }

                              break;

                          case rt_FLOAT :

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             FloatPtr = (float *)CharPtr;

                             for(k=0; k < n; k++,FloatPtr++)
                                 CheckSum += (*FloatPtr * Coeff);
                          }

                          break;

                          case rt_DOUBLE :

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             DoublePtr = (double *)CharPtr;

                             for(k=0; k < n; k++,DoublePtr++)
                                 CheckSum += (*DoublePtr * Coeff);
                          }

                          break;

                          case rt_FLOAT_COMPLEX :

                          Coeff *= 0.5;

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             FloatPtr = (float *)CharPtr;

                             for(k=0; k < n; k++,FloatPtr++)
                             {  CheckSum += (*FloatPtr * Coeff);
                                FloatPtr++;
                                CheckSum += (*FloatPtr * Coeff);
                             }
                          }

                          break;

                          case rt_DOUBLE_COMPLEX :

                          Coeff *= 0.5;

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)
                             DoublePtr = (double *)CharPtr;

                             for(k=0; k < n; k++,DoublePtr++)
                             {  CheckSum += (*DoublePtr * Coeff);
                                DoublePtr++;
                                CheckSum += (*DoublePtr * Coeff);
                             }
                          }

                          break;

                          default :

                          Coeff /= (double)DArr->TLen;
                          n *= DArr->TLen;

                          for(j=0; j < m; j++)
                          {  index_FromBlock1(IndexArray, &wBlock,
                                              Local)
                             LocElmAddr(CharPtr, DArr, IndexArray)

                             for(k=0; k < n; k++,CharPtr++)
                                 CheckSum += ((byte)*CharPtr * Coeff);
                          }

                          break;
                       }

                       break;

              case rt_CHAR: /* char */

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)

                      for(k=0; k < n; k++,CharPtr++)
                          CheckSum += ((byte)*CharPtr * Coeff);
                   }

                   break;

              case rt_INT: /* int */
              case rt_LOGICAL :

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      IntPtr = (int *)CharPtr;

                      for(k=0; k < n; k++,IntPtr++)
                          CheckSum += (*IntPtr * Coeff);
                   }

                   break;

              case rt_LONG: /* long */

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      long *LongPtr = (long*)CharPtr;

                      for(k=0; k < n; k++,LongPtr++)
                          CheckSum += (double)(*LongPtr * Coeff);
                   }

                   break;
              case rt_LLONG: /* long long */

                  for (j = 0; j < m; j++)
                  {
                      index_FromBlock1(IndexArray, &wBlock, Local)
                          LocElmAddr(CharPtr, DArr, IndexArray)
                          long long *LongPtr = (long long*)CharPtr;

                      for (k = 0; k < n; k++, LongPtr++)
                          CheckSum += (double)(*LongPtr * Coeff);
                  }

                  break;

              case rt_FLOAT: /* float */

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      FloatPtr = (float *)CharPtr;

                      for(k=0; k < n; k++,FloatPtr++)
                          CheckSum += (*FloatPtr * Coeff);
                   }

                   break;

              case rt_DOUBLE: /* double */

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      DoublePtr = (double *)CharPtr;

                      for(k=0; k < n; k++,DoublePtr++)
                          CheckSum += (*DoublePtr * Coeff);
                   }

                   break;

              case rt_FLOAT_COMPLEX: /* complex float */

                   Coeff *= 0.5;

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      FloatPtr = (float *)CharPtr;

                      for(k=0; k < n; k++,FloatPtr++)
                      {  CheckSum += (*FloatPtr * Coeff);
                         FloatPtr++;
                         CheckSum += (*FloatPtr * Coeff);
                      }
                   }

                   break;

              case rt_DOUBLE_COMPLEX: /* complex double */

                   Coeff *= 0.5;

                   for(j=0; j < m; j++)
                   {  index_FromBlock1(IndexArray, &wBlock, Local)
                      LocElmAddr(CharPtr, DArr, IndexArray)
                      DoublePtr = (double *)CharPtr;

                      for(k=0; k < n; k++,DoublePtr++)
                      {  CheckSum += (*DoublePtr * Coeff);
                         DoublePtr++;
                         CheckSum += (*DoublePtr * Coeff);
                      }
                   }

                   break;
           }
        }
     }
     else
     {  /* The array is a replicated array */

        /* Checksum calculation */

        m = (int)DVM_VMS->ProcCount;

        n = (int)pInfo->lLineSize;  /* array element number */

        Coeff = 1. / (double)n;
        Coeff /= (double)m;

        switch (pInfo->lElemType)
        {  case rt_INT :
           case rt_LOGICAL :

           IntPtr = (int *)pInfo->pAddr;

           for(k=0; k < n; k++,IntPtr++)
               CheckSum += (*IntPtr * Coeff);

           break;

           case rt_LONG:
           {
                long *LongPtr = (long*)pInfo->pAddr;
                for(k=0; k < n; k++,LongPtr++)
                   CheckSum += (*LongPtr * Coeff);
                break;
           }
           case rt_LLONG:
           {
               long long *LongPtr = (long long*)pInfo->pAddr;
               for (k = 0; k < n; k++, LongPtr++)
                   CheckSum += (*LongPtr * Coeff);
               break;
           }
           case rt_FLOAT :

           FloatPtr = (float *)pInfo->pAddr;

           for(k=0; k < n; k++,FloatPtr++)
               CheckSum += (*FloatPtr * Coeff);

           break;

           case rt_DOUBLE :

           DoublePtr = (double *)pInfo->pAddr;

           for(k=0; k < n; k++,DoublePtr++)
               CheckSum += (*DoublePtr * Coeff);

           break;

           case rt_FLOAT_COMPLEX :

           Coeff *= 0.5;

           FloatPtr = (float *)pInfo->pAddr;

           for(k=0; k < n; k++,FloatPtr++)
           {  CheckSum += (*FloatPtr * Coeff);
              FloatPtr++;
              CheckSum += (*FloatPtr * Coeff);
           }

           break;

           case rt_DOUBLE_COMPLEX :

           Coeff *= 0.5;

           DoublePtr = (double *)pInfo->pAddr;

           for(k=0; k < n; k++,DoublePtr++)
           {  CheckSum += (*DoublePtr * Coeff);
              DoublePtr++;
              CheckSum += (*DoublePtr * Coeff);
           }

           break;

           default :

           CharPtr = (char *)pInfo->pAddr;

           for(k=0; k < n; k++,CharPtr++)
               CheckSum += (*CharPtr * Coeff);

           break;
        }
     }

     CheckSumArray[i] = CheckSum;
  }

  /* Global checksum calculation */

#ifdef  _DVM_MPI_

  if(MPIGather)
  {
     /* Use MPI_Gather function */

     i = (int)(count * DVM_VMS->ProcCount);
     dvm_AllocArray(double, i, DoublePtr1);

     if(MPIInfoPrint && StatOff == 0)
       MPI_GatherTime -= dvm_time();

     SYSTEM(MPI_Gather, (CheckSumArray, count, MPI_DOUBLE,
                         DoublePtr1, count, MPI_DOUBLE,
                         DVM_VMS->VMSCentralProc, DVM_VMS->PS_MPI_COMM))

     if(MPIInfoPrint && StatOff == 0)
       MPI_GatherTime += dvm_time();

     if(MPS_CurrentProc == DVM_VMS->CentralProc)
     {
        /* The current processor is central */

        /* Check if local checksums of replicated arrays are equal

                This feature is not yet implemented. Requires additional communication
                in order to transfer information about different local checksums.

        for ( i=0; i<count; i++ )
        {
            if (!((dvm_ARRAY_INFO *)(arrays[i].pInfo))->bIsDistr)
            {

                for(j=1; j < DVM_VMS->ProcCount; j++) // starting at 1
                {
                    if (trc_CompareValue( (VALUE *)(DoublePtr1+i), (VALUE *)(???) , rt_DOUBLE ) == 0 )
                    {
                        // Different local checksums


                    }
                }
            }
        }                                            */

        for(i=0,DoublePtr2=DoublePtr1; i < DVM_VMS->ProcCount; i++)
        {  if(DVM_VMS->VProc[i].lP != MPS_CurrentProc)
           {  for(j=0; j < count; j++,DoublePtr2++)
                  CheckSumArray[j] += *DoublePtr2;
           }
           else
              DoublePtr2 += count;
        }
     }

     dvm_FreeArray(DoublePtr1);
  }
  else

#endif
  {
     /* Use "point-point" MPI function */

     if(MPS_CurrentProc == DVM_VMS->CentralProc)
     {
        /* The current processor is central */

        dvm_AllocArray(RTL_Request, DVM_VMS->ProcCount, Req);
        dvm_AllocArray(double *, DVM_VMS->ProcCount, ProcCheckSum);

        for(i=0; i < DVM_VMS->ProcCount; i++)
            if(DVM_VMS->VProc[i].lP != MPS_CurrentProc)
            {  dvm_AllocArray(double, count, ProcCheckSum[i]);

               ( RTL_CALL, rtl_Recvnowait(ProcCheckSum[i],
                                          count, sizeof(double),
                                          (int)DVM_VMS->VProc[i].lP,
                                          DVM_VMS->tag_common,
                                          &Req[i], 0) );
            }

        for(i=0; i < DVM_VMS->ProcCount; i++)
            if(DVM_VMS->VProc[i].lP != MPS_CurrentProc)
            {  ( RTL_CALL, rtl_Waitrequest(&Req[i]) );

                for(j=0; j < count; j++)
                    CheckSumArray[j] += ProcCheckSum[i][j];

                dvm_FreeArray(ProcCheckSum[i]);
            }

        dvm_FreeArray(ProcCheckSum);
        dvm_FreeArray(Req);
     }
     else
     {
        /* The current processor is not central */

        ( RTL_CALL, rtl_Sendnowait(CheckSumArray, count, sizeof(double),
                                   DVM_VMS->CentralProc,
                                   DVM_VMS->tag_common, &OutReq,
                                   RedNonCentral) );
        ( RTL_CALL, rtl_Waitrequest(&OutReq) );
     }
  }

  ( RTL_CALL, rtl_BroadCast(CheckSumArray, count, sizeof(double),
                            DVM_VMS->CentralProc, NULL) );

  for(i=0; i < count; i++)
      arrays[i].sum = CheckSumArray[i];

  dvm_FreeArray(CheckSumArray);

/*  pprintf(3, " EXITING\n ");*/
  return;
}


#endif  /* _ARCHKSUM_C_ */
