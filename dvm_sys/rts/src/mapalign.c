#ifndef _MAPALIGN_C_
#define _MAPALIGN_C_
/******************/    /*E0000*/

/***************************************************************\
* Functions for mapping distributed arrays according to the map *
\***************************************************************/    /*E0001*/

ArrayMapRef __callstd arrmap_(DvmType  ArrayHeader[], DvmType  *StaticSignPtr)

/*
            Getting map of a disributed array.
            ----------------------------------

ArrayHeader    - header of a distributed array.
*StaticSignPtr - sign of static map creation.

Function arrmap_  creates an object (map), describing current mapping
of a distributed array onto an  abstract machine representation and
returns reference to the created object.
*/    /*E0002*/

{ ArrayMapRef     Res;
  SysHandle      *ArrayHandlePtr, *MapHandlePtr;
  s_AMVIEW       *AMV;
  s_DISARRAY     *DArr;
  s_ARRAYMAP     *Map;
  int             ALSize; 

  StatObjectRef = (ObjectRef)ArrayHeader[0];   /* for statistics */    /*E0003*/
  DVMFTimeStart(call_arrmap_);

  if(RTL_TRACE)
     dvm_trace(call_arrmap_,
              "ArrayHeader=%lx; ArrayHandlePtr=%lx; StaticSign=%ld;\n",
              (uLLng)ArrayHeader, ArrayHeader[0], *StaticSignPtr);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(!ArrayHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.000: wrong call arrmap_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              (uLLng)ArrayHandlePtr);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL || DArr->Align == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.001: wrong call arrmap_\n"
              "(the array has not been aligned; ArrayHeader[0]=%lx)\n",
              (uLLng)ArrayHandlePtr);

  dvm_AllocStruct(SysHandle, MapHandlePtr);
  dvm_AllocStruct(s_ARRAYMAP, Map);

  Map->Static = (byte)*StaticSignPtr;
  Map->ArrayRank = DArr->Space.Rank;

  AMV = DArr->AMView;
  Map->AMView = AMV;
  Map->AMViewRank = AMV->Space.Rank;

  ALSize = DArr->Space.Rank + AMV->Space.Rank;

  dvm_AllocArray(s_ALIGN, ALSize, Map->Align);
  dvm_ArrayCopy(s_ALIGN, Map->Align, DArr->Align, ALSize);

  *MapHandlePtr = genv_InsertObject(sht_ArrayMap, Map);
  Map->HandlePtr = MapHandlePtr;  /* pointer to own Handle */    /*E0004*/

  if(TstObject)
     InsDVMObj((ObjectRef)MapHandlePtr);

  Res = (ArrayMapRef)MapHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_arrmap_,"ArrayMapRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res;   /* for statistics */    /*E0005*/
  DVMFTimeFinish(ret_arrmap_);
  return  (DVM_RET, Res);
}



DvmType __callstd malign_(DvmType ArrayHeader[], AMViewRef *AMViewRefPtr,
                          ArrayMapRef *ArrayMapRefPtr)

/*
     Definition of a distributed array location according to a map.
     --------------------------------------------------------------

ArrayHeader     - header of a distributed array.
*AMViewRefPtr   - pointer to an abstract machine representation,
                  that is a pattern for alignment. 
*ArrayMapRefPtr - pointer to a map of a distributed array.

Function malign_ defines mapping of given array on an abstract machine
representation according to a given map.
The function returns non zero value if mapped array has a local part 
on the current processor, otherwise returns zero. 
*/    /*E0006*/

{ SysHandle   *MapHandlePtr, *ArrayHandlePtr, *AMVHandlePtr;
  s_DISARRAY  *DArr;
  s_ARRAYMAP  *Map;
  s_AMVIEW    *TempAMV;
  s_SPACE     *ASpace, *AMVSpace;
  DvmType         bSize, Size;
  int          i, j, AR, TR, ALSize, VMSize, Temp;
  DvmType         LongTemp;
  s_BLOCK     *Local;
  s_VMS       *VMS;
  byte         Step = 0;
  char        *DArrElm;
  byte        *IsVMSBlock;
  s_BLOCK    **VMSBlock;
  s_MAP       *DistMap;
  s_ALIGN     *AlList;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0007*/
  DVMFTimeStart(call_malign_);

  if(RTL_TRACE)
  {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
        dvm_trace(call_malign_,
                  "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                  "AMViewRefPtr=NULL; AMViewRef=0; "
                  "ArrayMapRefPtr=%lx; ArrayMapRef=%lx;\n",
                  (uLLng)ArrayHeader, ArrayHeader[0],
                  (uLLng)ArrayMapRefPtr, *ArrayMapRefPtr);
     else 
        dvm_trace(call_malign_,
                  "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                  "AMViewRefPtr=%lx; AMViewRef=%lx; "
                  "ArrayMapRefPtr=%lx; ArrayMapRef=%lx;\n",
                  (uLLng)ArrayHeader, ArrayHeader[0],
                  (uLLng)AMViewRefPtr, *AMViewRefPtr,
                  (uLLng)ArrayMapRefPtr, *ArrayMapRefPtr);
  }

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.010: wrong call malign_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              (uLLng)ArrayHandlePtr);

  MapHandlePtr = (SysHandle *)*ArrayMapRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ArrayMapRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.011: wrong call malign_\n"
                 "(the array map is not a DVM object; "
                 "ArrayMapRef=%lx)\n", *ArrayMapRefPtr);
  }

  if(MapHandlePtr->Type != sht_ArrayMap)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.012: wrong call malign_\n"
              "(the object is not a distributed array map; "
              "ArrayMapRef=%lx)\n",
              *ArrayMapRefPtr);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  Map = (s_ARRAYMAP *)MapHandlePtr->pP;

  ASpace = &DArr->Space;
  AR = ASpace->Rank;

  if(AR != Map->ArrayRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.013: wrong call malign_\n"
              "(Array Rank=%d # Map Array Rank=%d)\n",
              AR, (int)Map->ArrayRank);

  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
  {  TempAMV = Map->AMView;
     AMVHandlePtr = TempAMV->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(!TstDVMObj(AMViewRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 046.014: wrong call malign_\n"
             "(the abstract machine representation is not "
             "a DVM object; AMViewRef=%lx)\n", *AMViewRefPtr);
     }

     AMVHandlePtr = (SysHandle *)*AMViewRefPtr;
     TempAMV = (s_AMVIEW *)AMVHandlePtr->pP;
  }

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 046.015: wrong call malign_\n"
            "(the object is not an abstract machine representation; "
            "AMViewRef=%lx)\n",
            (uLLng)AMVHandlePtr);

  if(TempAMV->Space.Rank != Map->AMViewRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.016: wrong call malign_\n"
              "(AMView Rank=%d # Map AMView Rank=%d)\n",
              TempAMV->Space.Rank, Map->AMViewRank);

  /* Check if distributed array and abstract
        machine representation are mapped    */    /*E0008*/
         
  if(DArr->AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.019: wrong call malign_\n"
              "(the array has already been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  VMS = TempAMV->VMS;

  if(VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.020: wrong call malign_\n"
              "(the representation has not been mapped; "
              "AMViewRef=%lx)\n", (uLLng)AMVHandlePtr);

  VMSize = (int)VMS->ProcCount;

  /*    Check if representation is mapped
     on subsystem of current processor system */    /*E0009*/

  NotSubsystem(i, DVM_VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
      "*** RTS err 046.017: wrong call malign_\n"
      "(the representation PS is not a subsystem of the current PS;\n"
      "AMViewRef=%lx; AMViewPSRef=%lx; CurrentPSRef=%lx)\n",
      (uLLng)AMVHandlePtr, (uLLng)VMS->HandlePtr,
      (uLLng)DVM_VMS->HandlePtr);

  AMVSpace = &TempAMV->Space; 
  TR = AMVSpace->Rank;

  ALSize = AR + TR;

  DArr->AMView = TempAMV;

  dvm_AllocArray(s_ALIGN, ALSize, DArr->Align);
  dvm_ArrayCopy(s_ALIGN, DArr->Align, Map->Align, ALSize);

  coll_Insert(&TempAMV->ArrColl, DArr);

  /* */    /*E0010*/

  j = VMS->Space.Rank;  /* */    /*E0011*/
  for(i=0; i < MaxVMRank; i++)
      DArr->DAAxis[i] = 0;

  for(i=0; i < AR; i++)
  {  AlList = &DArr->Align[i];

     if(AlList->Attr == align_NORMAL)
     {  DistMap = &TempAMV->DISTMAP[AlList->TAxis - 1];

        if(DistMap->Attr == map_BLOCK)
           DArr->DAAxis[DistMap->PAxis - 1] = i+1;
     }
  }

  if(align_Trace && TstTraceEvent(call_malign_))
     for(i=0; i < j; i++)
         tprintf("DArr->DAAxis[%d]=%d\n", i, DArr->DAAxis[i]);

  /* */    /*E0012*/

  dvm_AllocArray(s_BLOCK *, VMSize, DArr->VMSBlock);
  dvm_AllocArray(s_BLOCK, VMSize, DArr->VMSLocalBlock);
  dvm_AllocArray(byte, VMSize, DArr->IsVMSBlock);

  VMSBlock   = DArr->VMSBlock;
  IsVMSBlock = DArr->IsVMSBlock;

  for(i=0; i < VMSize; i++)
      IsVMSBlock[i] = 0;

  Local = NULL;

  if(VMS->HasCurrent)
  {  Local = GetSpaceLB4Proc(VMS->CurrentProc, TempAMV, ASpace,
                             Map->Align, NULL,
                             &DArr->VMSLocalBlock[VMS->CurrentProc]);
     VMSBlock[VMS->CurrentProc]   = Local;
     IsVMSBlock[VMS->CurrentProc] = 1;
  }

  if(Local)
     DArr->HasLocal = TRUE;  /* */    /*E0013*/

  /* */    /*E0014*/

  if(TempAMV->Repl)
  {  DArr->Repl  = 1;
     DArr->Every = 1;
  }
  else
  {  for(i=AR; i < ALSize; i++)
         if(DArr->Align[i].Attr == align_BOUNDREPL ||
            DArr->Align[i].Attr == align_CONSTANT)
            break;

     if(i != ALSize && (VMSize != 1 || Local == NULL))
        DArr->PartRepl = 1;  /* */    /*E0015*/
     else
     {  for(i=0; i < j; i++)
            if(DArr->DAAxis[i] != 0)
               break;

        if(i == j && TempAMV->Every)
        {  /* */    /*E0016*/

           DArr->Repl  = 1; /* */    /*E0017*/
           DArr->Every = 1; /* */    /*E0018*/
        }
        else
        {  for(i=0; i < j; i++)
               if(DArr->DAAxis[i] == 0)
                  break;

           if(i != j)
              DArr->PartRepl = 1; /* */    /*E0019*/

           if(TempAMV->Every)
           {  if(IsVMSBlock[0] == 0)
              {  VMSBlock[0] =
                 GetSpaceLB4Proc(0, TempAMV, ASpace, Map->Align, NULL,
                                 &DArr->VMSLocalBlock[0]);
                 IsVMSBlock[0] = 1;
              }

              j = VMSize - 1;

              if(IsVMSBlock[j] == 0)
              {  VMSBlock[j] =
                 GetSpaceLB4Proc(j, TempAMV, ASpace, Map->Align, NULL,
                                 &DArr->VMSLocalBlock[j]);
                 IsVMSBlock[j] = 1;
              }
  
              if(IsVMSBlock[0] != 0 && VMSBlock[0] != NULL &&
                 IsVMSBlock[j] != 0 && VMSBlock[j] != NULL)
                 DArr->Every = 1;
           }
        }
     }
  }

  /* ---------------------------------------------------------- */    /*E0020*/

  if(DArr->HasLocal)
  {
     DArr->Block          = block_Copy(Local);
     DArr->ArrBlock.Block = block_Copy(Local);

     Local = &DArr->ArrBlock.Block;

     if(DArr->MemPtr == NULL)      /* */    /*E0021*/
        AppendBounds(Local, DArr);

     DArr->ArrBlock.TLen  = DArr->TLen;
     DArr->ArrBlock.sI    = spind_Init((byte)AR);
     block_GetSize(bSize, Local, Step)
     bSize *= DArr->TLen;

     if(DArr->MemPtr == NULL)
     {  Size = bSize + DArr->TLen + DArr->TLen + AlignAddition;
        AlignAddition += AlignMemoryDelta;
        AlignCount++;

        if(AlignCount > AlignMemoryCircle)
        {  AlignAddition = AlignMemoryAddition;
           AlignCount = 0;
        }
     }
     else
        Size = bSize;  /* */    /*E0022*/

     /* */    /*E0023*/

     if(DArr->RegBufSign == 0)
     {  if(DArr->MemPtr == NULL)
        {  getdvmmem(DArr->ArrBlock.ALoc, Size);
        }
        else
        {  /* */    /*E0024*/

           DArr->ArrBlock.ALoc.Ptr0 = DArr->MemPtr;
           DArr->ArrBlock.ALoc.Ptr  = DArr->MemPtr;
           DArr->ArrBlock.ALoc.Size = Size;
        }
     }
     else
     {  /* */    /*E0025*/

        if(DVM_VMS->RemBuf == NULL)
        {  /* */    /*E0026*/

           getdvmmem(DArr->ArrBlock.ALoc, Size);
           DArr->RemBufMem = 1;
           DVM_VMS->RemBuf = DArr->ArrBlock.ALoc.Ptr0;
           DVM_VMS->RemBufSize = Size;
           DVM_VMS->FreeRemBuf = 0;
        }
        else
        {  /* */    /*E0027*/

           if(DVM_VMS->FreeRemBuf == 0)
           {  /* */    /*E0028*/

              getdvmmem(DArr->ArrBlock.ALoc, Size);
           }
           else
           {  /* */    /*E0029*/

              if(Size > DVM_VMS->RemBufSize)
              {  /* */    /*E0030*/

                 mac_free(&(DVM_VMS->RemBuf));
                 getdvmmem(DArr->ArrBlock.ALoc, Size);
                 DVM_VMS->RemBuf = DArr->ArrBlock.ALoc.Ptr0;
                 DVM_VMS->RemBufSize = Size;
              }
              else
              {  /* */    /*E0031*/

                 DArr->ArrBlock.ALoc.Ptr0 = DVM_VMS->RemBuf;
                 DArr->ArrBlock.ALoc.Ptr  = DVM_VMS->RemBuf;
                 DArr->ArrBlock.ALoc.Size = Size;
              }

              DArr->RemBufMem = 1;
              DVM_VMS->FreeRemBuf = 0;
           }
        }
     }

     if(DArr->MemPtr == NULL)
     {  bSize = dvm_abs( (DvmType)( (uLLng)DArr->ArrBlock.ALoc.Ptr0 ) -
                         (DvmType)( (uLLng)DArr->BasePtr ) );
        bSize %= DArr->TLen;

        if(bSize)
           DArr->ArrBlock.ALoc.Ptr = (void *)
                                     ( (uLLng)DArr->ArrBlock.ALoc.Ptr0 +
                                       (DArr->TLen - bSize) );
     }

     if(RTL_TRACE && TstTraceEvent(call_malign_))
     {  /* if extended header */    /*E0032*/

        tprintf("DArr->ArrBlock.ALoc.Ptr0=%lx  "
                "DArr->ArrBlock.ALoc.Ptr=%lx  "
                "Size=%ld\n",
                (uLLng)DArr->ArrBlock.ALoc.Ptr0,
                (uLLng)DArr->ArrBlock.ALoc.Ptr, Size);
     }

     /* Form distributed array header */    /*E0033*/

     bSize = 1;

     for(i=AR-1; i > 0; i--)
     {  ArrayHeader[i] = bSize * (Local->Set[i].Size);
        bSize = ArrayHeader[i];
     }

     ArrayHeader[AR] = -Local->Set[AR-1].Lower;

     for(i=1; i < AR; i++)
        ArrayHeader[AR] -= ArrayHeader[i] * (Local->Set[i-1].Lower);

     ArrayHeader[AR] += ( (uLLng)DArr->ArrBlock.ALoc.Ptr -
                          (uLLng)DArr->BasePtr )/(DArr->TLen);

     /* Complete forming header for Fortran */    /*E0034*/

     if(DArr->ExtHdrSign)    /* if extended header */    /*E0035*/
     {  Temp = AR+1;
        ArrayHeader[Temp] = ArrayHeader[AR] - ArrayHeader[AR+2];

        for(i=2; i <= AR; i++)
            ArrayHeader[Temp] -= (ArrayHeader[Temp-i]*
                                  ArrayHeader[Temp+i]);
     }

     /* Form the rest of distributed array headers */    /*E0036*/

     for(i=0; i < DACount; i++)
     {  if(DAHeaderAddr[i][0] == ArrayHeader[0] &&
           DAHeaderAddr[i] != ArrayHeader)
        {  for(j=0; j <= AR; j++)
               DAHeaderAddr[i][j] = ArrayHeader[j];

           if(DArr->ExtHdrSign)    /* */    /*E0037*/
           {  Temp = AR + AR + 1;

              for( ; j <= Temp; j++)
                  DAHeaderAddr[i][j] = ArrayHeader[j];
           }
        }
     }
     
     /* Check correctness of edge widths for given distribution */    /*E0038*/

     for(i = 0; i < AR; i++)
     {  if(DArr->InitLowShdWidth[i] > DArr->Block.Set[i].Size)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                 "*** RTS err 046.030: wrong call malign_\n"
                 "(Low Shadow Width[%d]=%d > Loc Size=%ld)\n",
                 i, DArr->InitLowShdWidth[i], DArr->Block.Set[i].Size);

        if(DArr->InitHighShdWidth[i] > DArr->Block.Set[i].Size)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                 "*** RTS err 046.031: wrong call malign_\n"
                 "(High Shadow Width[%d]=%d > Loc Size=%ld)\n",
                 i, DArr->InitHighShdWidth[i], DArr->Block.Set[i].Size);
     }
     
     /* Print local part of replicated array*/    /*E0039*/

     if(RTL_TRACE)
     {  if(align_Trace && TstTraceEvent(call_malign_))
        { for(i=0; i < AR; i++)
              tprintf("Local[%d]: Lower=%ld(%ld) Upper=%ld(%ld) "
                      "Size=%ld(%ld) Step=%ld\n",
                      i, DArr->Block.Set[i].Lower, Local->Set[i].Lower,
                      DArr->Block.Set[i].Upper, Local->Set[i].Upper,
                      DArr->Block.Set[i].Size, Local->Set[i].Size,
                      Local->Set[i].Step);
          tprintf(" \n");
          tprintf("Repl=%d PartRepl=%d Every=%d\n",
                  (int)DArr->Repl, (int)DArr->PartRepl,
                  (int)DArr->Every);
        }
     }

     /* Fill allocated array with zero code */    /*E0040*/

     if(DisArrayFill && DArr->MemPtr == NULL)
     {  if(FillCode[0] < 2)
        {  /* Filling  by one byte */    /*E0041*/

           if(FillCode[0])
              i = FillCode[1];
           else
              i = '\x00';

           DArrElm = (char *)DArr->ArrBlock.ALoc.Ptr0;
           LongTemp = (DvmType)DArr->ArrBlock.ALoc.Size;
           SYSTEM(memset, (DArrElm, i, LongTemp))
        }
        else
        {  /* Filling  by some bytes */    /*E0042*/

           s_BLOCK   CurrBlock;
           DvmType      Index[MAXARRAYDIM];

           CurrBlock = block_Copy(&DArr->Block);
           Temp = 0;
           block_GetSize(bSize, &CurrBlock, Temp)
           j = (int)bSize;
           DArrElm = (char *)&FillCode[1];

           for(i=0; i < j; i++)
           {  index_FromBlock(Index, &CurrBlock, &DArr->Block, Temp)
              PutLocElm(DArrElm, DArr, Index)
           }
        }
     }
  }

  bSize = DArr->HasLocal;

  if(RTL_TRACE)
     dvm_trace(ret_malign_,"IsLocal=%ld\n", bSize);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0043*/
  DVMFTimeFinish(ret_malign_);
  return  (DVM_RET, bSize);
}



DvmType __callstd mrealn_(DvmType  ArrayHeader[], AMViewRef  *AMViewRefPtr,
                          ArrayMapRef  *ArrayMapRefPtr,DvmType  *NewSignPtr)

/*
     Change of distributed array location according to a map.
     --------------------------------------------------------

ArrayHeader     - header of a distributed array to be realigned.
*AMViewRefPtr   - pointer to an abstract machine representation
                  that is a pattern for alignement.
*ArrayMapRefPtr - reference to a map of a distributed array.
*NewSignPtr     - sign of renewal of a redistributed array content
                  (equal to 1).

    Function mrealn_ cancels alignment earlier established for an array
with header ArrayHeader by function malign_ (or align_), and defines for
this array new alignment according to a given map.
The function returns non zero value if realigned array has a local part 
on the current processor, otherwise returns zero. 
*/    /*E0044*/

{ SysHandle   *MapHandlePtr, *ArrayHandlePtr, *AMVHandlePtr, *NewArrayHandlePtr;
  s_DISARRAY  *DArr, *NewDArr;
  s_ARRAYMAP  *Map;
  int          i, j, EnvInd, Coll_Ind;
  DvmType         AR, TypeSize, StaticSign, ReDistrSign, CopyRegim = 0,
               Res, Temp, ExtHdrSign;
  DvmType         LowShdWidthArray[MAXARRAYDIM],
               HiShdWidthArray[MAXARRAYDIM], StepArray[MAXARRAYDIM];
  DvmType         NewArrayHeader[2 * MAXARRAYDIM + 2];
  s_AMVIEW    *TempAMV = NULL, *DArrAMV = NULL;
  s_VMS       *DArrVMS;
  byte         SDisArrayFill;
  s_AMS       *wAMS;
  ObjectRef    ObjRef;
  s_ENVIRONMENT *Env;
  s_COLLECTION  *DAColl;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0045*/
  DVMFTimeStart(call_mrealn_);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 046.052: wrong call mrealn_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              (uLLng)ArrayHandlePtr);

  if(RTL_TRACE)
  {  if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
        dvm_trace(call_mrealn_,
                 "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                 "AMViewRefPtr=NULL; AMViewRef=0; "
                 "ArrayMapRefPtr=%lx; ArrayMapRef=%lx; NewSign=%lx;\n",
                 (uLLng)ArrayHeader, ArrayHeader[0],
                 (uLLng)ArrayMapRefPtr, *ArrayMapRefPtr, *NewSignPtr);
     else 
        dvm_trace(call_mrealn_,
                 "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                 "AMViewRefPtr=%lx; AMViewRef=%lx; "
                 "ArrayMapRefPtr=%lx; ArrayMapRef=%lx; NewSign=%lx;\n",
                 (uLLng)ArrayHeader, ArrayHeader[0],
                 (uLLng)AMViewRefPtr, *AMViewRefPtr,
                 (uLLng)ArrayMapRefPtr, *ArrayMapRefPtr, *NewSignPtr);
  }

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL) /* whether distributed array is mapped */    /*E0046*/
  {  SDisArrayFill = DisArrayFill; /* save attribute of array filling by
                                      zero byte*/    /*E0047*/
     /* If it is necessary to fill in first time distributed array
        by zero byte */    /*E0048*/

     DisArrayFill = (byte)(DisArrayFill || *NewSignPtr == 2);

     Res = ( RTL_CALL, malign_(ArrayHeader, AMViewRefPtr,
                               ArrayMapRefPtr) );

     DisArrayFill = SDisArrayFill; /* restore attribute of array filling by
                                      zero byte */    /*E0049*/ 
  }
  else
  {  MapHandlePtr = (SysHandle *)*ArrayMapRefPtr;

     if(TstObject)
     {  if(!TstDVMObj(ArrayMapRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 046.050: wrong call mrealn_\n"
                    "(the array map is not a DVM object; "
                    "ArrayMapRef=%lx)\n", (uLLng)MapHandlePtr);
     }

     if(MapHandlePtr->Type != sht_ArrayMap)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.051: wrong call mrealn_\n"
                 "(the object is not a distributed array map; "
                 "ArrayMapRef=%lx)\n",
                 *ArrayMapRefPtr);

     if( (DArr->ReDistr >> 1) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.053: wrong call mrealn_\n"
                 "(ArrayHeader[0]=%lx; ReDistrPar=%d)\n",
                 ArrayHeader[0], (int)DArr->ReDistr);

     Map = (s_ARRAYMAP *)MapHandlePtr->pP;

     if(AMViewRefPtr == NULL || *AMViewRefPtr == 0)
     {  TempAMV = Map->AMView;
        AMVHandlePtr = TempAMV->HandlePtr;
     }
     else
     {  if(TstObject)
        {  if(TstDVMObj(AMViewRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 046.054: wrong call mrealn_\n"
                "(the abstract machine representation is not "
                "a DVM object; AMViewRef=%lx)\n", *AMViewRefPtr);
        }

        AMVHandlePtr = (SysHandle *)*AMViewRefPtr;
        TempAMV = (s_AMVIEW *)AMVHandlePtr->pP;
     }

     if(AMVHandlePtr->Type != sht_AMView)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 046.055: wrong call mrealn_\n"
             "(the object is not an abstract machine representation; "
             "AMViewRef=%lx)\n",
             (uLLng)AMVHandlePtr);

     AR  = DArr->Space.Rank;

     if(AR != Map->ArrayRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.056: wrong call mrealn_\n"
                 "(Array Rank=%d # Map Array Rank=%d)\n",
                 AR, (int)Map->ArrayRank);

     if(TempAMV->Space.Rank != Map->AMViewRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.057: wrong call mrealn_\n"
                 "(AMView Rank=%d # Map AMView Rank=%d)\n",
                 TempAMV->Space.Rank, Map->AMViewRank);

     DArrAMV = DArr->AMView;

     /* Check if distributed array is mapped on
         subsystem of current processor system  */    /*E0050*/

     DArrVMS = DArrAMV->VMS;
 
     NotSubsystem(i, DVM_VMS, DArrVMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 046.059: wrong call mrealn_\n"
         "(the array PS is not a subsystem of the current PS;\n"
         "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
         ArrayHeader[0], (uLLng)DArrVMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

     /* Check if abstract machine representatin is mapped */    /*E0051*/
         
     if(TempAMV->VMS == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 046.061: wrong call mrealn_\n"
                 "(the representation has not been mapped; "
                 "AMViewRef=%lx)\n", (uLLng)AMVHandlePtr);

     /* Check if representation is mapped on
        subsystem of current processor system */    /*E0052*/

     NotSubsystem(i, DVM_VMS, TempAMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 046.058: wrong call mrealn_\n"
         "(the representation PS is not a subsystem "
         "of the current PS;\n"
         "AMViewRef=%lx; AMViewPSRef=%lx; CurrentPSRef=%lx)\n",
         (uLLng)AMVHandlePtr, (uLLng)TempAMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

     TypeSize    = DArr->TLen;
     if (DArr->Type > 0)
         TypeSize = -DArr->Type;
     StaticSign  = DArr->Static;
     ReDistrSign = DArr->ReDistr;

     for(i=0; i < AR; i++)
     {  LowShdWidthArray[i] = DArr->InitLowShdWidth[i];
        HiShdWidthArray[i]  = DArr->InitHighShdWidth[i];
     }

     ExtHdrSign = DArr->ExtHdrSign;

     ( RTL_CALL, crtda_(NewArrayHeader, &ExtHdrSign, DArr->BasePtr,
                        &AR, &TypeSize,
                        DArr->Space.Size, &StaticSign, &ReDistrSign,
                        LowShdWidthArray, HiShdWidthArray) );

     TypeSize = DArr->TLen;
     /* */    /*E0053*/

     ((SysHandle *)NewArrayHeader[0])->CrtBlockInd =
     ArrayHandlePtr->CrtBlockInd;

     /* XXX: If this function will ever be used - part below must
        be fixed like in regular align function */
     if(TempAMV->HandlePtr->CrtBlockInd > ArrayHandlePtr->CrtBlockInd)
        TempAMV->HandlePtr->CrtBlockInd = ArrayHandlePtr->CrtBlockInd;

     if(TempAMV->AMHandlePtr != NULL)
     {  if(TempAMV->AMHandlePtr->CrtBlockInd >
           ArrayHandlePtr->CrtBlockInd)
           TempAMV->AMHandlePtr->CrtBlockInd =
           ArrayHandlePtr->CrtBlockInd;
     }

     if(TempAMV->VMS->HandlePtr->CrtBlockInd >
        ArrayHandlePtr->CrtBlockInd)
        TempAMV->VMS->HandlePtr->CrtBlockInd =
        ArrayHandlePtr->CrtBlockInd;

     if(TempAMV->WeightVMS != NULL)
     {  if(TempAMV->WeightVMS->HandlePtr->CrtBlockInd >
           ArrayHandlePtr->CrtBlockInd)
           TempAMV->WeightVMS->HandlePtr->CrtBlockInd =
           ArrayHandlePtr->CrtBlockInd;
     }

     for(i=0; i < TempAMV->AMSColl.Count; i++)
     {  wAMS = coll_At(s_AMS *, &TempAMV->AMSColl, i);

        if(wAMS->HandlePtr->CrtBlockInd > ArrayHandlePtr->CrtBlockInd)
           wAMS->HandlePtr->CrtBlockInd = ArrayHandlePtr->CrtBlockInd;

        if(wAMS->ParentAMView != NULL)
        {  if(wAMS->ParentAMView->HandlePtr->CrtBlockInd >
              ArrayHandlePtr->CrtBlockInd)
              wAMS->ParentAMView->HandlePtr->CrtBlockInd =
              ArrayHandlePtr->CrtBlockInd;
        }

        if(wAMS->VMS != NULL)
        {  if(wAMS->VMS->HandlePtr->CrtBlockInd >
              ArrayHandlePtr->CrtBlockInd)
              wAMS->VMS->HandlePtr->CrtBlockInd =
              ArrayHandlePtr->CrtBlockInd;
        }
     }

     /* Rewrite lower index values stored in header */    /*E0054*/

     if(DArr->ExtHdrSign)  /* if extended header */    /*E0055*/
     {  Temp = 2*AR + 1;
        for(i=AR+2; i <= Temp; i++)
            NewArrayHeader[i] = ArrayHeader[i];
     }

     SDisArrayFill = DisArrayFill; /* save attribute of array filling by
                                      zero byte*/    /*E0056*/
     /* If it is necessary to fill in first time distributed array
        by zero byte */    /*E0057*/

     DisArrayFill = (byte)((DisArrayFill && *NewSignPtr == 1) ||
                            *NewSignPtr == 2);

     Res = (RTL_CALL, malign_(NewArrayHeader, AMViewRefPtr,
                              ArrayMapRefPtr) );

     DisArrayFill = SDisArrayFill; /* restore attribute of array filling by
                                      zero byte */    /*E0058*/
   
     if(*NewSignPtr == 0)
     {  /* Copy an old array into new one */    /*E0059*/
     
        for(i=0; i < AR; i++)
        {  LowShdWidthArray[i] = 0;
           HiShdWidthArray[i] = DArr->Space.Size[i]-1;
           StepArray[i] = 1;
        }

        ( RTL_CALL, arrcpy_(ArrayHeader, LowShdWidthArray,
                            HiShdWidthArray, StepArray,
                            NewArrayHeader, LowShdWidthArray,
                            HiShdWidthArray, StepArray, &CopyRegim) );
     }


     /* Form all an old array headers except
        ArrayHeader as an new array headers  */    /*E0060*/

     for(i=0; i < DACount; i++)
     {  if(DAHeaderAddr[i][0] == ArrayHeader[0] &&
           DAHeaderAddr[i] != ArrayHeader)
   
        {  for(j=0; j <= AR; j++)
               DAHeaderAddr[i][j] = NewArrayHeader[j];

           if(DArr->ExtHdrSign)    /* if extended header */    /*E0061*/
           {  Temp = 2*AR + 1;

              for( ; j <= Temp; j++)
                  DAHeaderAddr[i][j] = NewArrayHeader[j];
           }
        }
     }

     EnvInd = gEnvColl->Count - 1; /* current context index */
     Env  = coll_At(s_ENVIRONMENT *, gEnvColl, EnvInd); /* current context */
     DAColl = &(Env->DisArrList); /* collection for distributed arrays */
     Coll_Ind = coll_IndexOf(DAColl, DArr); /* previous position of array in collection */

     if (Coll_Ind != -1)
     {  NewArrayHandlePtr = TstDVMArray(NewArrayHeader);
        NewDArr = (s_DISARRAY *)NewArrayHandlePtr->pP;

        /* switching position from last to previous */
        coll_Delete(DAColl,NewDArr);
        coll__AtInsert(DAColl,Coll_Ind,NewDArr);
     }

     j = DArr->ExtHdrSign; /* save flag of
                              extended header */    /*E0062*/

     ( RTL_CALL, delda_(ArrayHeader) );/* delete old array */    /*E0063*/

     /* Form ArrayHeader as a header of new array */    /*E0064*/

     for(i=0; i <= AR; i++)
         ArrayHeader[i] = NewArrayHeader[i];

     if(j)                    /* if extended header */    /*E0065*/
     {  Temp = 2*AR + 1;
        for( ; i <= Temp; i++)
            ArrayHeader[i] = NewArrayHeader[i];
     }

     DAHeaderAddr[DACount-1] = ArrayHeader;/* in place of NewArrayHeader */    /*E0066*/

     ((SysHandle *)NewArrayHeader[0])->HeaderPtr = (uLLng)ArrayHeader;

      ArrayHandlePtr = DArrAMV->HandlePtr;

     if(DArrAMV->ArrColl.Count == 0 &&
        ArrayHandlePtr->InitCrtBlockInd == 0)
        {  ObjRef = (ObjectRef)ArrayHandlePtr;

           ( RTL_CALL, delobj_(&ObjRef) ); /* delayed deletion of s_AMVIEW object */
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_mrealn_,"IsLocal=%ld\n", Res);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0068*/
  DVMFTimeFinish(ret_mrealn_);
  return  (DVM_RET, Res);
}



DvmType  __callstd delarm_(ArrayMapRef  *ArrayMapRefPtr)

/*
      Map deleting.
      -------------

*ArrayMapRefPtr - reference to a distributed array map.

Function delarm_ deletes the map of a distributed array created by
function arrmap_.

The function returns zero.
*/    /*E0069*/

{ SysHandle  *MapHandlePtr;

  StatObjectRef = (ObjectRef)*ArrayMapRefPtr;    /* for statistics */    /*E0070*/
  DVMFTimeStart(call_delarm_);

  if(RTL_TRACE)
     dvm_trace(call_delarm_,"ArrayMapRefPtr=%lx; ArrayMapRef=%lx;\n",
                            (uLLng)ArrayMapRefPtr, *ArrayMapRefPtr);

  MapHandlePtr = (SysHandle *)*ArrayMapRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(ArrayMapRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 046.090: wrong call delarm_\n"
            "(the array map is not a DVM object; "
            "ArrayMapRef=%lx)\n", *ArrayMapRefPtr);
  }

  if(MapHandlePtr->Type != sht_ArrayMap)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 046.091: wrong call delarm_\n"
         "(the object is not a distributed array map; "
         "ArrayMapRef=%lx)\n",
         *ArrayMapRefPtr);

  ( RTL_CALL, delobj_((ObjectRef *)ArrayMapRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delarm_," \n");

  StatObjectRef = (ObjectRef)*ArrayMapRefPtr;    /* for statistics */    /*E0071*/
  DVMFTimeFinish(ret_delarm_);
  return  (DVM_RET, 0);
}



void ArrMap_Done(s_ARRAYMAP  *Map)
{ 
  if(RTL_TRACE)
     dvm_trace(call_ArrMap_Done,
               "ArrayMapRef=%lx;\n",(uLLng)Map->HandlePtr);

  dvm_FreeArray(Map->Align);
  Map->AMView = NULL;

  if(TstObject)
     DelDVMObj((ObjectRef)Map->HandlePtr);

  Map->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(Map->HandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_ArrMap_Done," \n");

  (DVM_RET);

  return;
}


#endif   /*  _MAPALIGN_C_  */    /*E0072*/
