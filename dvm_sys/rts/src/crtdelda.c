#ifndef _CRTDELDA_C_
#define _CRTDELDA_C_
/******************/    /*E0000*/

/****************************************************\
* Functions to create and delete a distributed array * 
\****************************************************/    /*E0001*/

DvmType  __callstd crtda_(DvmType   ArrayHeader[], DvmType  *ExtHdrSignPtr,
                          void  *BasePtr, DvmType  *RankPtr,
                          DvmType  *TypeSizePtr, DvmType  SizeArray[],
                          DvmType  *StaticSignPtr, DvmType  *ReDistrSignPtr,
                          DvmType   LowShdWidthArray[], DvmType HiShdWidthArray[])

/*
      Distributed array creating.
      ---------------------------

ArrayHeader	 - header of the array to be created.
BasePtr 	 - base pointer, used to access to distributed array
                   elements.
*RankPtr	 - rank of the created array.
*TypeSizePtr	 - size of the array element (in bytes).
SizeArray	 - SizeArray[i] is a size of the (i+1)th dimension
                   of the created array (0<= i <= *RankPtr-1).
*StaticSignPtr	 - the flag of the static array creation.
*ReDistrSignPtr	 - the permission flag of using of created distributed
                   array as a parameter in the function redis_ .
LowShdWidthArray - LowShdWidthArray[i] is a width of the low shadow
                   edge of the (i+1)-th dimension of the distributed
                   array.
HiShdWidthArray  - HiShdWidthArray[i] is a width of the high shadow
                   edge of the (i+1)-th dimension of the distributed
                   array.

The creation of a distributed array using function crtda_ means only
an initialization of the internal system structures describing
the array.
The function returns zero.
*/    /*e0002*/

{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArrPtr;
  int            i;

  DVMFTimeStart(call_crtda_);

  if(RTL_TRACE)
  {  dvm_trace(call_crtda_,
              "ArrayHeader=%lx; ExtHdrSign=%ld; BasePtr=%lx; "
              "Rank=%ld; "
              "TypeSize=%ld; StaticSign=%ld; ReDistrSign=%ld;\n",
              (uLLng)ArrayHeader, *ExtHdrSignPtr, (uLLng)BasePtr,
              *RankPtr, *TypeSizePtr, *StaticSignPtr, *ReDistrSignPtr);

     if(TstTraceEvent(call_crtda_))
     {  for(i=0; i < *RankPtr; i++)
           tprintf("SizeArray[%d]=%ld; ",i,SizeArray[i]);
        tprintf(" \n");
        for(i=0; i < *RankPtr; i++)
           tprintf("LowShdWidthArray[%d]=%ld; ",i,LowShdWidthArray[i]);
        tprintf(" \n");
        for(i=0; i < *RankPtr; i++)
           tprintf(" HiShdWidthArray[%d]=%ld; ",i,HiShdWidthArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(TstDVMArray(ArrayHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.000: wrong call crtda_\n"
              "(ArrayHeader already exists; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  if(*RankPtr > MAXARRAYDIM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.001: wrong call crtda_ "
              "(Array Rank=%ld > %d)\n", *RankPtr, (int)MAXARRAYDIM);

  if(*RankPtr <= 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.001: wrong call crtda_ "
              "(Array Rank=%ld)\n", *RankPtr);

  dvm_AllocStruct(s_DISARRAY, DArrPtr);
  dvm_AllocStruct(SysHandle, ArrayHandlePtr);

  DArrPtr->ExtHdrSign     = (byte)*ExtHdrSignPtr;

  if(BasePtr == NULL)
     DArrPtr->BasePtr     = GlobalBasePtr;
  else
     DArrPtr->BasePtr     = BasePtr;

  DArrPtr->Static         = (byte)*StaticSignPtr;
  DArrPtr->ReDistr        = (byte)*ReDistrSignPtr;
  DArrPtr->Space          = space_Init((byte)*RankPtr,SizeArray);

  if(*TypeSizePtr < -6)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.005: wrong call crtda_ "
              "(invalid type of array element (=%ld))\n", *TypeSizePtr);

  if(*TypeSizePtr > 0)
  {  DArrPtr->TLen = (word)*TypeSizePtr;
     DArrPtr->Type    = -1;  /* */    /*E0003*/
  }
  else
  {  DArrPtr->Type = -*TypeSizePtr;

     switch(DArrPtr->Type)
     {  case rt_CHAR:            DArrPtr->TLen = sizeof(char);
                                 break;

        case rt_INT:             DArrPtr->TLen = sizeof(int);
                                 break;

        case rt_LONG:            DArrPtr->TLen = sizeof(long);
                                 break;
        case rt_LLONG:            DArrPtr->TLen = sizeof(long long);
                                 break;
        case rt_FLOAT:           DArrPtr->TLen = sizeof(float);
                                 break;

        case rt_DOUBLE:          DArrPtr->TLen = sizeof(double);
                                 break;

        case rt_FLOAT_COMPLEX:   DArrPtr->TLen = 2*sizeof(float);
                                 break;

        case rt_DOUBLE_COMPLEX:  DArrPtr->TLen = 2*sizeof(double);
                                 break;
     }
  }

  DArrPtr->AMView         = NULL;
  DArrPtr->Align          = NULL;
  DArrPtr->HasLocal       = FALSE;
  DArrPtr->Repl           = 0;
  DArrPtr->PartRepl       = 0;
  DArrPtr->Every          = 0;
  DArrPtr->InitBlock.Rank = 0;
  DArrPtr->CurrBlock.Rank = 0;
  DArrPtr->VMSBlock       = NULL;
  DArrPtr->VMSLocalBlock  = NULL;
  DArrPtr->IsVMSBlock     = NULL;
  DArrPtr->MemPtr         = NULL;
  DArrPtr->IsCheckSum     = 0;

  /* */    /*E0004*/

  DArrPtr->ConsistSign = 0;     /* */    /*E0005*/
  DArrPtr->RealConsist = 0;     /* */    /*E0006*/
  DArrPtr->AllocSign   = 0;     /* */    /*E0007*/

  DArrPtr->CReadBSize  = NULL;  /* */    /*E0008*/
  DArrPtr->CReadBuf    = NULL;  /* */    /*E0009*/
  DArrPtr->CReadBlock  = NULL;  /* */    /*E0010*/
  DArrPtr->CReadReq    = NULL;  /* */    /*E0011*/

  DArrPtr->ReadBlockNumber = NULL;  /* */    /*E0012*/
  DArrPtr->CRBlockPtr  = NULL;  /* */    /*E0013*/

  DArrPtr->CWriteBSize = 0;     /* */    /*E0014*/
  DArrPtr->CWriteBuf   = NULL;  /* */    /*E0015*/
  DArrPtr->CWriteReq   = NULL;  /* */    /*E0016*/

  DArrPtr->WriteBlockNumber = 0;    /* */    /*E0017*/
  DArrPtr->DArrWBlockPtr    = NULL; /* */    /*E0018*/
  DArrPtr->CentralPSPtr = NULL; /* */    /*E0019*/

  DArrPtr->DAAxisM1    = -1;;   /* */    /*E0020*/

  DArrPtr->File        = NULL;  /* */    /*E0021*/
  DArrPtr->Line        = 0;     /* */    /*E0022*/

  DArrPtr->CG          = NULL;  /* */    /*E0023*/
  DArrPtr->ConsistProcCount = 0; /* */    /*E0024*/
  DArrPtr->sdispls = NULL;       /* */    /*E0025*/
  DArrPtr->sendcounts = NULL;    /* */    /*E0026*/
  DArrPtr->rdispls = NULL;       /* */    /*E0027*/

  /* List of group of edges containing the array */    /*E0028*/

  DArrPtr->BG = coll_Init(ArrGrpCount, ArrGrpCount, NULL);

  /* Resulting esge widths for each group of edges
     containing the array */    /*E0029*/

  DArrPtr->ResShdWidthColl = coll_Init(ArrGrpCount, ArrGrpCount, NULL);

  DArrPtr->RegBuf         = NULL;
  DArrPtr->RegBufSign     = 0;
  DArrPtr->RemBufMem      = 0;
  DArrPtr->IdBuf          = NULL;

  for(i=0; i < *RankPtr; i++)
  { if(LowShdWidthArray[i] < 0 || LowShdWidthArray[i] > SizeArray[i])
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 040.002: wrong call crtda_ "
               "(LowShdWidthArray[%d]=%ld)\n", i, LowShdWidthArray[i]);

    DArrPtr->InitLowShdWidth[i] = (int)LowShdWidthArray[i];

    if(HiShdWidthArray[i] < 0 || HiShdWidthArray[i] > SizeArray[i])
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 040.003: wrong call crtda_ "
                "(HiShdWidthArray[%d]=%ld)\n", i, HiShdWidthArray[i]);

    DArrPtr->InitHighShdWidth[i] = (int)HiShdWidthArray[i];
  } 

  *ArrayHandlePtr = genv_InsertObject(sht_DisArray, DArrPtr);
  DArrPtr->HandlePtr = ArrayHandlePtr; /* pointer to own
                                          Handle */    /*E0030*/

  ArrayHandlePtr->HeaderPtr = (uLLng)ArrayHeader;
  ArrayHeader[0] = (uLLng)ArrayHandlePtr;

  ArrayHandlePtr->BasePtr = DArrPtr->BasePtr; /* base pointer for macros
                                                 DAElm1, ... , DAElm7 */    /*E0031*/

  if(DACount >= MaxDACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 040.004: wrong call crtda_\n(DistrArray Count = "
         "Max DistrArray Count(%d))\n", MaxDACount);

  DAHeaderAddr[DACount] = ArrayHeader;
  DACount++;

  if(TstObject)
     InsDVMObj((ObjectRef)ArrayHandlePtr);


  if( EnableDynControl )
      dyn_DefineDisArray( ArrayHandlePtr, (byte)*StaticSignPtr, NULL );


  if(RTL_TRACE)
     dvm_trace(ret_crtda_,"ArrayHandlePtr=%lx;\n",
                          (uLLng)ArrayHandlePtr);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0032*/
  DVMFTimeFinish(ret_crtda_);
  return  (DVM_RET, 0);
}



DvmType  __callstd crtrda_(DvmType   ArrayHeader[], DvmType  *ExtHdrSignPtr,
                           void  *BasePtr, DvmType  *RankPtr,
                           DvmType  *TypeSizePtr, DvmType  SizeArray[],
                           DvmType  *StaticSignPtr, DvmType  *ReDistrSignPtr,
                           void  *MemPtr)

/*
      Replicated array creating.
      ---------------------------

ArrayHeader	 - header of the array to be created.
BasePtr 	 - base pointer, used to access to replicatid array
                   elements.
*RankPtr	 - rank of the created array.
*TypeSizePtr	 - size of the array element (in bytes).
SizeArray	 - SizeArray[i] is a size of the (i+1)th dimension
                   of the created array (0<= i <= *RankPtr-1).
*StaticSignPtr	 - the flag of the static array creation.
*ReDistrSignPtr	 - the permission flag of using of created replicated
                   array as a parameter in the function redis_ .
MemPtr           - pointer to array memory.
*/    /*E0033*/

{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArrPtr;
  int            i;
  AMViewRef      AMVRef;
  DvmType        LongZero = 0;
  DvmType        AxisArray[1] = { 0 }, DistrParamArray[1] = { 0 };
  DvmType        AlignAxisArray[MAXARRAYDIM] = {-1, -1, -1, -1, -1, -1, -1},
                 CoeffArray[MAXARRAYDIM] = {0, 0, 0, 0, 0, 0, 0},
                 ConstArray[MAXARRAYDIM] = {0, 0, 0, 0, 0, 0, 0};

  DVMFTimeStart(call_crtrda_);

  if(RTL_TRACE)
  {  dvm_trace(call_crtrda_,
              "ArrayHeader=%lx; ExtHdrSign=%ld; BasePtr=%lx; "
              "Rank=%ld; "
              "TypeSize=%ld; StaticSign=%ld; ReDistrSign=%ld; "
              "MemPtr=%lx;\n",
              (uLLng)ArrayHeader, *ExtHdrSignPtr, (uLLng)BasePtr,
              *RankPtr, *TypeSizePtr, *StaticSignPtr, *ReDistrSignPtr,
              (uLLng)MemPtr);

     if(TstTraceEvent(call_crtrda_))
     {  for(i=0; i < *RankPtr; i++)
            tprintf("SizeArray[%d]=%ld; ", i, SizeArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(TstDVMArray(ArrayHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 041.000: wrong call crtrda_\n"
              "(ArrayHeader already exists; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  if(*RankPtr > MAXARRAYDIM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 041.001: wrong call crtrda_ "
              "(Array Rank=%ld > %d)\n", *RankPtr, (int)MAXARRAYDIM);

  dvm_AllocStruct(s_DISARRAY, DArrPtr);
  dvm_AllocStruct(SysHandle, ArrayHandlePtr);

  DArrPtr->ExtHdrSign     = (byte)*ExtHdrSignPtr;

  if(BasePtr == NULL)
     DArrPtr->BasePtr     = GlobalBasePtr;
  else
     DArrPtr->BasePtr     = BasePtr;

  DArrPtr->Static         = (byte)*StaticSignPtr;
  DArrPtr->ReDistr        = (byte)*ReDistrSignPtr;
  DArrPtr->Space          = space_Init((byte)*RankPtr, SizeArray);

  if(*TypeSizePtr < -6)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 041.005: wrong call crtrda_ "
              "(invalid type of array element (=%ld))\n", *TypeSizePtr);

  if(*TypeSizePtr > 0)
  {  DArrPtr->TLen = (word)*TypeSizePtr;
     DArrPtr->Type    = -1;  /* */    /*E0034*/
  }
  else
  {  DArrPtr->Type = -*TypeSizePtr;

     switch(DArrPtr->Type)
     {  case rt_CHAR:            DArrPtr->TLen = sizeof(char);
                                 break;

        case rt_INT:             DArrPtr->TLen = sizeof(int);
                                 break;

        case rt_LONG:            DArrPtr->TLen = sizeof(long);
                                 break;
        case rt_LLONG:            DArrPtr->TLen = sizeof(long long);
                                 break;
        case rt_FLOAT:           DArrPtr->TLen = sizeof(float);
                                 break;

        case rt_DOUBLE:          DArrPtr->TLen = sizeof(double);
                                 break;

        case rt_FLOAT_COMPLEX:   DArrPtr->TLen = 2*sizeof(float);
                                 break;

        case rt_DOUBLE_COMPLEX:  DArrPtr->TLen = 2*sizeof(double);
                                 break;
     }
  }

  DArrPtr->AMView         = NULL;
  DArrPtr->Align          = NULL;
  DArrPtr->HasLocal       = 0;
  DArrPtr->Repl           = 0;
  DArrPtr->PartRepl       = 0;
  DArrPtr->Every          = 0;
  DArrPtr->InitBlock.Rank = 0;
  DArrPtr->CurrBlock.Rank = 0;
  DArrPtr->VMSBlock       = NULL;
  DArrPtr->VMSLocalBlock  = NULL;
  DArrPtr->IsVMSBlock     = NULL;
  DArrPtr->MemPtr         = MemPtr;
  DArrPtr->IsCheckSum     = 0;

  /* */    /*E0035*/

  DArrPtr->ConsistSign = 0;     /* */    /*E0036*/
  DArrPtr->RealConsist = 0;     /* */    /*E0037*/
  DArrPtr->AllocSign   = 0;     /* */    /*E0038*/

  DArrPtr->CReadBSize  = NULL;  /* */    /*E0039*/
  DArrPtr->CReadBuf    = NULL;  /* */    /*E0040*/
  DArrPtr->CReadBlock  = NULL;  /* */    /*E0041*/
  DArrPtr->CReadReq    = NULL;  /* */    /*E0042*/

  DArrPtr->ReadBlockNumber = NULL;  /* */    /*E0043*/
  DArrPtr->CRBlockPtr  = NULL;  /* */    /*E0044*/

  DArrPtr->CWriteBSize = 0;     /* */    /*E0045*/
  DArrPtr->CWriteBuf   = NULL;  /* */    /*E0046*/
  DArrPtr->CWriteReq   = NULL;  /* */    /*E0047*/

  DArrPtr->WriteBlockNumber = 0;    /* */    /*E0048*/
  DArrPtr->DArrWBlockPtr    = NULL; /* */    /*E0049*/
  DArrPtr->CentralPSPtr = NULL; /* */    /*E0050*/

  DArrPtr->DAAxisM1    = -1;;   /* */    /*E0051*/

  DArrPtr->File        = NULL;  /* */    /*E0052*/
  DArrPtr->Line        = 0;     /* */    /*E0053*/

  DArrPtr->CG          = NULL;  /* */    /*E0054*/
  DArrPtr->ConsistProcCount = 0; /* */    /*E0055*/
  DArrPtr->sdispls = NULL;       /* */    /*E0056*/
  DArrPtr->sendcounts = NULL;    /* */    /*E0057*/
  DArrPtr->rdispls = NULL;       /* */    /*E0058*/

  /* */    /*E0059*/

  DArrPtr->BG = coll_Init(ArrGrpCount, ArrGrpCount, NULL);

  /* */    /*E0060*/

  DArrPtr->ResShdWidthColl = coll_Init(ArrGrpCount, ArrGrpCount, NULL);

  DArrPtr->RegBuf         = NULL;
  DArrPtr->RegBufSign     = 0;
  DArrPtr->RemBufMem      = 0;
  DArrPtr->IdBuf          = NULL;

  for(i=0; i < *RankPtr; i++)
  { DArrPtr->InitLowShdWidth[i]  = 0;
    DArrPtr->InitHighShdWidth[i] = 0;
  } 

  *ArrayHandlePtr = genv_InsertObject(sht_DisArray, DArrPtr);
  DArrPtr->HandlePtr = ArrayHandlePtr; /* */    /*E0061*/

  ArrayHandlePtr->HeaderPtr = (uLLng)ArrayHeader;
  ArrayHeader[0] = (uLLng)ArrayHandlePtr;

  ArrayHandlePtr->BasePtr = DArrPtr->BasePtr; /* base pointer for macros
                                                 DAElm1, ... , DAElm7 */    /*E0062*/

  if(DACount >= MaxDACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 041.004: wrong call crtrda_\n(DistrArray Count = "
         "Max DistrArray Count(%d))\n", MaxDACount);

  DAHeaderAddr[DACount] = ArrayHeader;
  DACount++;

  if(TstObject)
     InsDVMObj((ObjectRef)ArrayHandlePtr);

  if( EnableDynControl )
      dyn_DefineDisArray( ArrayHandlePtr, (byte)*StaticSignPtr, NULL );

  /* */    /*E0063*/
  
  AMVRef = (RTL_CALL, crtamv_(NULL, RankPtr, SizeArray, StaticSignPtr));

  /* */    /*E0064*/

  (RTL_CALL, distr_(&AMVRef, NULL, &LongZero, AxisArray, DistrParamArray));

  /* */    /*E0065*/

  (RTL_CALL, align_(ArrayHeader, (PatternRef *)&AMVRef, AlignAxisArray,
                    CoeffArray, ConstArray));

  if(RTL_TRACE)
     dvm_trace(ret_crtrda_, "ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHandlePtr);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* */    /*E0066*/
  DVMFTimeFinish(ret_crtrda_);
  return  (DVM_RET, 0);
}


/* */    /*E0067*/


DvmType  __callstd crtda9_(DvmType   ArrayHeader[], DvmType  *ExtHdrSignPtr,
                           AddrType  *BasePtrAddr, DvmType  *RankPtr,
                           DvmType  *TypeSizePtr, DvmType  SizeArray[],
                           DvmType *StaticSignPtr, DvmType  *ReDistrSignPtr,
                           DvmType   LowShdWidthArray[], DvmType HiShdWidthArray[])
{

  return crtda_(ArrayHeader, ExtHdrSignPtr, (void *)*BasePtrAddr,
                RankPtr, TypeSizePtr, SizeArray, StaticSignPtr,
                ReDistrSignPtr, LowShdWidthArray, HiShdWidthArray);
}



DvmType  __callstd crtraf_(DvmType   ArrayHeader[], DvmType  *ExtHdrSignPtr,
                           void  *BasePtr, DvmType  *RankPtr,
                           DvmType  *TypeSizePtr, DvmType  SizeArray[],
                           DvmType  *StaticSignPtr, DvmType  *ReDistrSignPtr,
                           AddrType  *MemAddrPtr)
{
  return  crtrda_(ArrayHeader, ExtHdrSignPtr, BasePtr, RankPtr,
                  TypeSizePtr, SizeArray, StaticSignPtr, ReDistrSignPtr,
                  (void *)*MemAddrPtr);
}



DvmType  __callstd crtra9_(DvmType ArrayHeader[], DvmType *ExtHdrSignPtr,
                           AddrType  *BasePtrAddr, DvmType *RankPtr,
                           DvmType *TypeSizePtr, DvmType SizeArray[],
                           DvmType *StaticSignPtr, DvmType *ReDistrSignPtr,
                           AddrType  *MemAddrPtr)
{
  return  crtrda_(ArrayHeader, ExtHdrSignPtr, (void *)*BasePtrAddr,
                  RankPtr, TypeSizePtr, SizeArray, StaticSignPtr,
                  ReDistrSignPtr, (void *)*MemAddrPtr);
}


/*  ------------------  */    /*E0068*/


DvmType  __callstd addhdr_(DvmType NewArrayHeader[], DvmType ArrayHeader[])
{ SysHandle   *ArrayHandlePtr;
  int          AR, i;
  s_DISARRAY  *DArr;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0069*/
  DVMFTimeStart(call_addhdr_);

  if(RTL_TRACE)
     dvm_trace(call_addhdr_,
         "NewArrayHeader=%lx; ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
         (uLLng)NewArrayHeader, (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 040.015: wrong call addhdr_\n"
       "(the object is not a distributed array; ArrayHeader[0]=%lx)\n",
       ArrayHeader[0]);

  if(TstDVMArray(NewArrayHeader))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.016: wrong call addhdr_\n"
              "(NewArrayHeader already exists; "
              "NewArrayHeader[0]=%lx)\n", NewArrayHeader[0]);

  if(DACount >= MaxDACount)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 040.017: wrong call addhdr_\n"
              "(ArrayHeader Count = Max ArrayHeader Count(%d))\n",
              MaxDACount);

  DAHeaderAddr[DACount] = NewArrayHeader;
  DACount++;

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AR   = DArr->Space.Rank;

  for(i=0; i <= AR; i++)
      NewArrayHeader[i] = ArrayHeader[i];
  
  if(DArr->ExtHdrSign)    /* if Fortran */    /*E0070*/
  {  AR += (AR + 1);

     for( ; i <= AR; i++)
         NewArrayHeader[i] = ArrayHeader[i];
  }

  if(RTL_TRACE)
     dvm_trace(ret_addhdr_,"NewArrayHandlePtr=%lx;\n",
                            NewArrayHeader[0]);

  StatObjectRef = (ObjectRef)NewArrayHeader[0]; /* for statistics */    /*E0071*/
  DVMFTimeFinish(ret_addhdr_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delhdr_(DvmType  ArrayHeader[])
{
   SysHandle  *ArrayHandlePtr;
   int         i;
   DvmType     lHandlePtr;

   StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0072*/
   DVMFTimeStart(call_delhdr_);

   if(RTL_TRACE)
      dvm_trace(call_delhdr_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                              (uLLng)ArrayHeader, ArrayHeader[0]);

   ArrayHandlePtr = TstDVMArray(ArrayHeader);

   if(ArrayHandlePtr == NULL)
      epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 040.018: wrong call delhdr_\n"
               "(the object is not an array header; "
               "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

   if((uLLng)ArrayHeader == (uLLng)ArrayHandlePtr->HeaderPtr)
   {
      /* It is the main header */    /*E0073*/
 
      /* Find new main header */    /*E0074*/

      lHandlePtr = (DvmType)ArrayHandlePtr;

      for(i=0; i < DACount; i++)
      {
         if(DAHeaderAddr[i][0] == lHandlePtr &&
            DAHeaderAddr[i] != ArrayHeader)
            break;
      }

      if(i == DACount)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 040.019: wrong call delhdr_\n"
                  "(ArrayHeader is the sole header; "
                  "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

      /* Change the main header */    /*E0075*/

      ArrayHandlePtr->HeaderPtr = (uLLng)DAHeaderAddr[i];
   }

   /* Delete the header */    /*E0076*/

   for(i=0; i < DACount; i++)
   {
      if(DAHeaderAddr[i] == ArrayHeader)
      {
         for( ; i < DACount - 1; i++)
             DAHeaderAddr[i] = DAHeaderAddr[i+1];

         DACount--;
         break;
      }
   }

   if(RTL_TRACE)
      dvm_trace(ret_delhdr_," \n");

   StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0077*/
   DVMFTimeFinish(ret_delhdr_);
   return  (DVM_RET, 0);
}



DvmType  __callstd delda_(DvmType ArrayHeader[])

/*
      Distributed array  deleting.
      ----------------------------

ArrayHeader - the header of the array to be deleted. 

The function deletes the distributed array created by function crtda_. 
The function returns zero.
*/    /*E0078*/

{
   SysHandle      *ArrayHandlePtr;

   StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0079*/
   DVMFTimeStart(call_delda_);

   if(RTL_TRACE)
      dvm_trace(call_delda_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

   ArrayHandlePtr = TstDVMArray(ArrayHeader);

   if(ArrayHandlePtr == NULL)
      epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 040.020: wrong call delda_\n"
        "(the object is not a distributed array; ArrayHeader[0]=%lx)\n",
        ArrayHeader[0]);

   ( RTL_CALL, delobj_((ObjectRef *)ArrayHeader) );

   if(RTL_TRACE)
      dvm_trace(ret_delda_," \n");

   StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0080*/
   DVMFTimeFinish(ret_delda_);
   return  (DVM_RET, 0);
}


/* ----------------------------------------------- */    /*E0081*/


void  DelDA(s_DISARRAY  *DArr)
{
   ObjectRef  DARef;

   DARef = (ObjectRef)DArr->HandlePtr;

   if(RTL_TRACE)
      dvm_trace(call_DelDA,"ArrayHandlePtr=%lx;\n", DARef);

   ( RTL_CALL, delobj_(&DARef) );

   if(RTL_TRACE)
      dvm_trace(ret_DelDA," \n");

   (DVM_RET);
   return;
}



void  disarr_Done(s_DISARRAY  *DArr)
{
   DvmType        lHandlePtr;
   int            i, j;
   s_REGBUF       *RegBuf;
   s_REGBUFGROUP  *RBG;
   s_IDBUF        *IdBuf;
   s_IDBUFGROUP   *IBG;
   AMViewRef       AMVRef;
   s_BOUNDGROUP   *BGPtr;
   SysHandle      *HandlePtr, *AMVHandlePtr;
   ObjectRef       ObjRef;

   HandlePtr = DArr->HandlePtr;

   if(RTL_TRACE)
   {
      dvm_trace(call_disarr_Done,
                "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                HandlePtr->HeaderPtr, (uLLng)HandlePtr);
   }

   if(EnableDynControl)
   {
      dyn_RemoveVar((void *)DArr);
   }

   /* Free arrays of array local part blocks */    /*E0082*/

   if(DArr->VMSBlock)
   {
      dvm_FreeArray(DArr->IsVMSBlock);
      dvm_FreeArray(DArr->VMSBlock);
      dvm_FreeArray(DArr->VMSLocalBlock);
   }

   /* Delete all edge group
     containing the deleted array */    /*E0083*/

   if(ShgSave == 0)
   {
      while(DArr->BG.Count)
      {
         BGPtr = coll_At(s_BOUNDGROUP *, &DArr->BG, 0);
         ( RTL_CALL, delshg_((ShadowGroupRef *)&BGPtr->HandlePtr) );
      }
   }

   dvm_FreeArray(DArr->BG.List); /* Delete list of edge group
                                   containing the array */    /*E0084*/

   dvm_FreeArray(DArr->ResShdWidthColl.List); /* delete the list of 
                                               resulting edge widths */    /*E0085*/

   /* Remove deleted array from the list of 
     abstract machine representation arrays */    /*E0086*/

   if(DArr->AMView)
      coll_Delete(&DArr->AMView->ArrColl, DArr);

   /* Complete using the array as a buffer of remote elements */    /*E0087*/

   RegBuf = DArr->RegBuf;

   if(RegBuf)
   {
      if(RegBuf->LoadSign)
         ( RTL_CALL, waitcp_(&RegBuf->CopyFlag) );

      RBG = RegBuf->RBG;

      if(RBG) /* is the deleted buffer included in any group ? */    /*E0088*/
         coll_Delete(&RBG->RB, DArr);  /* delete the buffer from the
                                         group buffer list */    /*E0089*/
      if(RegBuf->crtrbp_sign)
      {
         /*  The buffer was created by crtrbp_ function. 
           Delete temporary representation of current AM */    /*E0090*/

         AMVRef = (AMViewRef)DArr->AMView->HandlePtr;
         ( RTL_CALL, delamv_(&AMVRef) );
      }

      dvm_FreeStruct(DArr->RegBuf);
   }

   /*   Complete using the array as a buffer
     of remote elements of non-regular access */    /*E0091*/

   IdBuf = DArr->IdBuf;

   if(IdBuf)
   {
      if(IdBuf->LoadSign)
         ( RTL_CALL, WaitLoadBuffer(DArr) );

      IBG = IdBuf->IBG;

      if(IBG) /* is the deleted buffer included in any group ? */    /*E0092*/
         coll_Delete(&IBG->IB, DArr);  /* delete the buffer from the
                                         group buffer list */    /*E0093*/
      dvm_FreeStruct(DArr->IdBuf);
   }

   DArr->Space.Rank = 0;

   if(DArr->Align)
      dvm_FreeArray(DArr->Align);

   if(DArr->HasLocal)
   {
      DArr->Block.Rank = 0;
      spind_Done(&DArr->ArrBlock.sI);

      /* */    /*E0094*/

      if(DArr->RegBufSign == 0)
      {
         /* */    /*E0095*/

         if(DArr->MemPtr == NULL)
         {
            freedvmmem(&DArr->ArrBlock.ALoc);
         }
         else
         {
            /* */    /*E0096*/

            DArr->ArrBlock.ALoc.Ptr0 = NULL;
            DArr->ArrBlock.ALoc.Ptr  = NULL;
            DArr->ArrBlock.ALoc.Size = 0;
         }
      }
      else
      {
         /* */    /*E0097*/

         if(DArr->RemBufMem == 0)
         {
            /* */    /*E0098*/

            freedvmmem(&DArr->ArrBlock.ALoc);
         }
         else
         {
            /* */    /*E0099*/

            DVM_VMS->FreeRemBuf = 1;
         }
      }

      /* ---------------------------------------- */    /*E0100*/

      DArr->ArrBlock.Block.Rank = 0;
   }

   DArr->HasLocal = FALSE;

   lHandlePtr = (DvmType)HandlePtr;

   for(i=0; i < DACount; i++)
   {
      if(DAHeaderAddr[i][0] == lHandlePtr)
      {
         for(j=i ; j < DACount - 1; j++)
             DAHeaderAddr[j] = DAHeaderAddr[j+1];
         i--;
         DACount--;
      }
   }

   if(TstObject)
      DelDVMObj((ObjectRef)HandlePtr);
  
   HandlePtr->Type = sht_NULL;
   dvm_FreeStruct(HandlePtr);

   /* */    /*E0101*/

   if(DArr->CG) /* */    /*E0102*/
      coll_Delete(&DArr->CG->RDA, DArr);  /* */    /*E0103*/

   /* */    /*E0104*/

   dvm_FreeArray(DArr->CReadReq); 
   dvm_FreeArray(DArr->CWriteReq); 
   dvm_FreeArray(DArr->CReadBSize);

   dvm_FreeArray(DArr->CentralPSPtr);
   dvm_FreeArray(DArr->DArrWBlockPtr);

   dvm_FreeArray(DArr->sdispls);
   dvm_FreeArray(DArr->sendcounts);
   dvm_FreeArray(DArr->rdispls);

   if(DArr->AllocSign != 0)
   {
      /* */    /*E0105*/

      if(DArr->CReadBlock != NULL)
      {
         /* */    /*E0106*/

         if(DArr->CReadBuf != NULL)
         {
            for(i=0; i < DArr->AMView->VMS->ProcCount; i++)
            {
               mac_free(&DArr->CReadBuf[i]);
            }
         }

         mac_free(&DArr->CWriteBuf);
         dvm_FreeArray(DArr->CReadBlock);
      }
      else
      {
         /* */    /*E0107*/

         if(DArr->WriteBlockNumber != 1 || DArr->DAAxisM1 != 0 ||
            IsSynchr != 0)
         {
            /* */    /*E0108*/

            mac_free(&DArr->CWriteBuf);
         }

         if(DArr->CReadBuf != NULL)
         {
            for(i=0; i < DArr->AMView->VMS->ProcCount; i++)
            {
               if(DArr->ReadBlockNumber[i] != 1 ||
                  DArr->DAAxisM1 != 0 || IsSynchr != 0)
               {
                  /* */    /*E0109*/

                  mac_free(&DArr->CReadBuf[i]);
               }
            }
         }
      }
   }

   dvm_FreeArray(DArr->CReadBuf);
   dvm_FreeArray(DArr->ReadBlockNumber);

   if(DArr->CRBlockPtr != NULL)
   {
      for(i=0; i < DArr->AMView->VMS->ProcCount; i++)
      {
         dvm_FreeArray(DArr->CRBlockPtr[i]);
      }

      dvm_FreeArray(DArr->CRBlockPtr);
   }

   /* */    /*E0110*/

   if(DArr->MemPtr != NULL)
   {
      AMVRef = (AMViewRef)DArr->AMView->HandlePtr;
 
      ( RTL_CALL, delamv_(&AMVRef) );
   }

   /* Delayed deletion for AM representation.
      Happens if its context ended and it has
      no more arrays associated with it */
   if((DArr->AMView != NULL) && (DArr->RegBufSign == 0) &&
      (DArr->MemPtr == NULL))
   {
      AMVHandlePtr = DArr->AMView->HandlePtr;

      if(DArr->AMView->ArrColl.Count == 0 &&
         AMVHandlePtr->InitCrtBlockInd == 0)
      {  ObjRef = (ObjectRef)AMVHandlePtr;

         ( RTL_CALL, delobj_(&ObjRef) );
      }
   }

   if(RTL_TRACE)
      dvm_trace(ret_disarr_Done," \n");

   (DVM_RET);

   return;
}


#endif  /*  _CRTDELDA_C_  */    /*E0111*/
