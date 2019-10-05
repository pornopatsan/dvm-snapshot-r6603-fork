#ifndef _ALIGN_C_
#define _ALIGN_C_
/***************/    /*E0000*/

/*************************************\
* Mapping distributed array functions *
\*************************************/    /*E0001*/

DvmType __callstd align_(DvmType ArrayHeader[], PatternRef *PatternRefPtr,
                         DvmType AxisArray[], DvmType CoeffArray[],
                         DvmType ConstArray[])
/*
     Aligning distributed array.
     ---------------------------

ArrayHeader	- the header of the distributed array to be aligned.
*PatternRefPtr	- reference to the alignment pattern.
AxisArray	- AxisArray[j] is a dimension number of the distributed
                  array used in the linear alignment rule for the
                  pattern (j+1)-th dimension. 
CoeffArray	- CoeffArray[j] is a coefficient for distributed array
                  index variable used in the linear alignment rule for
                  the pattern (j+1)-th dimension.
ConstArray	- ConstArray[j] is a constant used in the linear
                  alignment rule for the pattern (j+1)th dimension.

The function align_ defines the location of the specified distributed
array in the assigned abstract machine representation space.
The function returns nonzero value if mapped array has a local part on
the current processor, otherwice - returns zero.
*/    /*E0002*/

{ SysHandle   *TempHandlePtr, *ArrayHandlePtr;
  s_DISARRAY  *TempDArr = NULL, *DArr = NULL;
  s_AMVIEW    *TempAMV = NULL;
  s_ALIGN     *TAlign = NULL, *AlList, *ResList;
  s_SPACE     *TSpace, *ASpace, *AMVSpace;
  DvmType         bSize, Size;
  int          i, j, AR, TR, ALSize, VMSize, Temp;
  DvmType         LongTemp, longI;
  s_ALIGN      aAl, tAl;
  s_BLOCK     *Local;
  DvmType         TstArray[MAXARRAYDIM];
  byte         Step = 0;
  s_VMS       *VMS;
  char        *DArrElm;
  byte        *IsVMSBlock;
  s_BLOCK    **VMSBlock;
  s_MAP       *DistMap;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0003*/
  DVMFTimeStart(call_align_);

  if(TstObject)
  {  if(TstDVMObj(PatternRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 044.000: wrong call align_\n"
             "(the pattern is not a DVM object; "
             "PatternRef=%lx)\n", *PatternRefPtr);
  }

  TempHandlePtr = (SysHandle *)*PatternRefPtr;

  switch(TempHandlePtr->Type)
  {  case sht_DisArray:

     if(RTL_TRACE)
        dvm_trace(call_align_,
            "ArrayHeader=%lx; ArrayHandlePtr=%lx; PatternRefPtr=%lx; "
            "PatternRef=%lx (DisArray);\n",
            (uLLng)ArrayHeader, ArrayHeader[0],
            (uLLng)PatternRefPtr, *PatternRefPtr);

     TempDArr = (s_DISARRAY *)TempHandlePtr->pP;
     TempAMV  = TempDArr->AMView;
     TAlign   = TempDArr->Align;
     TSpace   = &TempDArr->Space;
     break;

     case sht_AMView:

     if(RTL_TRACE)
        dvm_trace(call_align_,
            "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
            "PatternRefPtr=%lx; PatternRef=%lx (AMView);\n",
            (uLLng)ArrayHeader, ArrayHeader[0],
            (uLLng)PatternRefPtr, *PatternRefPtr);

     TempAMV = (s_AMVIEW *)TempHandlePtr->pP;
     TSpace  = &TempAMV->Space;
     break;

     default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 044.001: wrong call align_\n"
                       "(the object is not an aligning pattern; "
                       "PatternRef=%lx)\n",
                       *PatternRefPtr);
  }

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
    epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 044.002: wrong call align_\n"
             "(the object is not a distributed array; "
             "ArrayHeader[0]=%lx)\n",
             (uLLng)ArrayHandlePtr);

  DArr   = (s_DISARRAY *)ArrayHandlePtr->pP;

  /* Check if aligned array and map pattern are mapped */    /*E0004*/

  if(DArr->AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 044.005: wrong call align_\n"
              "(the array has already been aligned; "
              "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  if(TempAMV == NULL || TempAMV->VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 044.006: wrong call align_\n"
              "(the pattern has not been mapped; "
              "PatternRef=%lx)\n", (uLLng)TempHandlePtr);

  VMS = TempAMV->VMS;
  VMSize = (int)VMS->ProcCount;

  /* Check if align pattern is mapped on sudsystem of
     the current processor system */    /*E0005*/

  NotSubsystem(i, DVM_VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 044.003: wrong call align_\n"
           "(the pattern PS is not a subsystem of the current PS;\n"
           "PatternRef=%lx; PatternPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)TempHandlePtr, (uLLng)VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

  ASpace = &DArr->Space;

  AR     = ASpace->Rank;
  TR     = TSpace->Rank;
  ALSize = AR+TR;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_align_))
     {  for(i=0; i < TR; i++)
            tprintf(" AxisArray[%d]=%ld; ",i,AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
            tprintf("CoeffArray[%d]=%ld; ",i,CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
            tprintf("ConstArray[%d]=%ld; ",i,ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  /* Check correctness of given parameters */    /*E0006*/

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] < -1)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 044.019: wrong call align_\n"
                 "(AxisArray[%d]=%ld < -1)\n", i, AxisArray[i]);
     if(AxisArray[i] > AR)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 044.020: wrong call align_\n"
                 "(AxisArray[%d]=%ld > %d)\n", i, AxisArray[i], AR);
     if(AxisArray[i] >= 0)
     {  if(ConstArray[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 044.021: wrong call align_\n"
                    "(ConstArray[%d]=%ld < 0)\n", i, ConstArray[i]);
        if(ConstArray[i] >= TSpace->Size[i])
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 044.022: wrong call align_\n"
                    "(ConstArray[%d]=%ld >= %ld)\n", i, ConstArray[i],
                                                     TSpace->Size[i]);
        if(CoeffArray[i] < 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 044.023: wrong call align_\n"
                    "(CoeffArray[%d]=%ld < 0)\n", i, CoeffArray[i]);
        if(CoeffArray[i] > 0)
        { if(AxisArray[i] == 0)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 044.024: wrong call align_\n"
                      "(AxisArray[%d]=0)\n", i);

          bSize = CoeffArray[i]*(ASpace->Size[AxisArray[i]-1]-1) +
                  ConstArray[i];

          if(bSize >= TSpace->Size[i])
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 044.025: wrong call align_\n"
                      "( (CoeffArray[%d]=%ld) * %ld + "
                      "(ConstArray[%d]=%ld) >= %ld )\n",
                      i, CoeffArray[i], ASpace->Size[AxisArray[i]-1]-1,
                      i, ConstArray[i], TSpace->Size[i]);
        }
      }
  }

  for(i=0; i < AR; i++)
      TstArray[i] = 0;

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] <= 0 || CoeffArray[i] == 0)
        continue;
     if(TstArray[AxisArray[i]-1])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 044.026: wrong call align_\n"
                 "(AxisArray[%ld]=AxisArray[%d]=%ld)\n",
                 TstArray[AxisArray[i]-1]-1, i, AxisArray[i]);
     TstArray[AxisArray[i]-1] = i+1;
  }

  /*************************\
  *   Alignment execution   *
  \*************************/    /*E0007*/

  dvm_AllocArray(s_ALIGN, ALSize, AlList);

  /* Preliminary fill of 1-st and 2-nd parts AlList */    /*E0008*/ 

  for(i=0; i < AR; i++)
  {  AlList[i].Attr  = align_COLLAPSE;
     AlList[i].Axis  = (byte)(i+1);
     AlList[i].TAxis = 0;
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  for(i=AR; i < ALSize; i++)
  {  AlList[i].Attr  = align_NORMTAXIS;
     AlList[i].Axis  = 0;
     AlList[i].TAxis = (byte)(i-AR+1);
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  /* Fill of AlList according to the given parameters */    /*E0009*/

  for(i=0; i < TR; i++)
  {  if(AxisArray[i] == -1)
     {  AlList[i+AR].Attr  = align_REPLICATE;
        AlList[i+AR].Axis  = 0;
	AlList[i+AR].TAxis = (byte)(i+1);
        AlList[i+AR].A     = 0;
        AlList[i+AR].B     = 0;
     }
     else
     {  if(CoeffArray[i]==0)
        {  AlList[i+AR].Attr  = align_CONSTANT;
           AlList[i+AR].Axis  = 0;
           AlList[i+AR].TAxis = (byte)(i+1);
           AlList[i+AR].A     = 0;
           AlList[i+AR].B     = ConstArray[i];
        }
        else
        {  AlList[i+AR].Axis = (byte)AxisArray[i];
	   AlList[i+AR].A    = CoeffArray[i];
           AlList[i+AR].B    = ConstArray[i];

	   AlList[AxisArray[i]-1].Attr  = align_NORMAL;
	   AlList[AxisArray[i]-1].Axis  = (byte)AxisArray[i];
	   AlList[AxisArray[i]-1].TAxis = (byte)(i+1);
	   AlList[AxisArray[i]-1].A     = CoeffArray[i];
           AlList[AxisArray[i]-1].B     = ConstArray[i];
        }
     }
  }  

  /* Execute superposition of mappings */    /*E0010*/

  AMVSpace = &TempAMV->Space;
  ALSize   = AR + AMVSpace->Rank;

  if(TAlign)
  {  dvm_AllocArray(s_ALIGN, ALSize, ResList);

     for(i=0; i < AMVSpace->Rank; i++)
         ResList[i+AR] = TAlign[i+TR];

     for(i=0; i < AR; i++)
     {  aAl = AlList[i];

        if(aAl.Attr == align_NORMAL)
        {  tAl = TAlign[aAl.TAxis - 1];

           switch (tAl.Attr)
           {  case align_NORMAL   : aAl.TAxis = tAl.TAxis;
                                    aAl.A *= tAl.A;
                                    aAl.B = aAl.B * tAl.A + tAl.B;
                                    ResList[i] = aAl;
                                    ResList[AR+aAl.TAxis-1].Axis =
                                    (byte)(i+1);
                                    ResList[AR+aAl.TAxis-1].A = aAl.A;
                                    ResList[AR+aAl.TAxis-1].B = aAl.B;
                                    break;
              case align_COLLAPSE : aAl.TAxis = 0;
                                    aAl.Attr  = align_COLLAPSE;
                                    ResList[i] = aAl;
                                    break;
           }
        }
        else
           ResList[i] = aAl;   /* if COLLAPSE */    /*E0011*/
     }

     for(i=0; i < TR; i++)
     {  aAl = AlList[i+AR];

        switch(aAl.Attr)
        { case align_CONSTANT : tAl = TAlign[aAl.TAxis-1];
                                if(tAl.Attr == align_NORMAL)
                                {  aAl.TAxis = tAl.TAxis;
                                   aAl.B = tAl.A*aAl.B + tAl.B;
                                   ResList[AR+tAl.TAxis-1] = aAl;
                                }
                                break;
          case align_REPLICATE: tAl = TAlign[aAl.TAxis-1];
                                if(tAl.Attr == align_NORMAL)
                                { aAl.TAxis = tAl.TAxis;

                                  if(tAl.A != 1 || tAl.B != 0)
                                  { aAl.Attr  = align_BOUNDREPL;
                                    aAl.A     = tAl.A;
                                    aAl.B     = tAl.B;
                                    aAl.Bound =
                                    TSpace->Size[tAl.TAxis-1];
                                  }

                                  ResList[AR+tAl.TAxis-1] = aAl;
                                }
                                break;
       }
     }

     dvm_FreeArray(AlList);
     AlList = ResList;
  }

  /* Print resulting  mapping */    /*E0012*/

  if(RTL_TRACE)
  {  if(align_Trace && TstTraceEvent(call_align_))
     {  for(i=0; i < ALSize; i++)
            tprintf("AlignMap[%d]: Attr=%d Axis=%d TAxis=%d "
                    "A=%ld B=%ld Bound=%ld\n",
                    i, (int)AlList[i].Attr, (int)AlList[i].Axis,
                    (int)AlList[i].TAxis, AlList[i].A, AlList[i].B,
                    AlList[i].Bound);
        tprintf(" \n");
     }
  }

  /* Form resulting alignment */    /*E0013*/

  DArr->AMView = TempAMV;

  dvm_AllocArray(s_ALIGN, ALSize, DArr->Align);
  dvm_ArrayCopy(s_ALIGN, DArr->Align, AlList, ALSize);

  coll_Insert(&TempAMV->ArrColl, DArr);

  /* */    /*E0014*/

  j = VMS->Space.Rank;  /* */    /*E0015*/
  for(i=0; i < MaxVMRank; i++)
      DArr->DAAxis[i] = 0;

  for(i=0; i < AR; i++)
  {  ResList = &AlList[i];

     if(ResList->Attr == align_NORMAL)
     {  DistMap = &TempAMV->DISTMAP[ResList->TAxis - 1];

        if(DistMap->Attr == map_BLOCK)
           DArr->DAAxis[DistMap->PAxis - 1] = i+1;
     }
  }

  if(align_Trace && TstTraceEvent(call_align_))
     for(i=0; i < j; i++)
         tprintf("DArr->DAAxis[%d]=%d\n", i, DArr->DAAxis[i]);

  /* */    /*E0016*/

  dvm_AllocArray(s_BLOCK *, VMSize, DArr->VMSBlock);
  dvm_AllocArray(s_BLOCK, VMSize, DArr->VMSLocalBlock);
  dvm_AllocArray(byte, VMSize, DArr->IsVMSBlock);

  VMSBlock   = DArr->VMSBlock;
  IsVMSBlock = DArr->IsVMSBlock; 

  for(i=0; i < VMSize; i++)
      IsVMSBlock[i] = 0;

  Local = NULL;

  if(VMS->HasCurrent)
  {  Local = GetSpaceLB4Proc(VMS->CurrentProc, TempAMV, ASpace, AlList,
                             NULL,
                             &DArr->VMSLocalBlock[VMS->CurrentProc]);
     VMSBlock[VMS->CurrentProc]   = Local;
     IsVMSBlock[VMS->CurrentProc] = 1;
  }

  if(Local)
     DArr->HasLocal = TRUE;   /* */    /*E0017*/

  /* */    /*E0018*/

  if(TempAMV->Repl)
  {  DArr->Repl  = 1;
     DArr->Every = 1;
  }
  else
  {  for(i=AR; i < ALSize; i++)
         if(AlList[i].Attr == align_BOUNDREPL ||
            AlList[i].Attr == align_CONSTANT)
            break;

     if(i != ALSize && (VMSize != 1 || Local == NULL))
        DArr->PartRepl = 1;  /* */    /*E0019*/
     else
     {  for(i=0; i < j; i++)
            if(DArr->DAAxis[i] != 0)
               break;

        if(i == j && TempAMV->Every)
        {  /* */    /*E0020*/

           DArr->Repl  = 1; /* */    /*E0021*/
           DArr->Every = 1; /* */    /*E0022*/
        }
        else
        {  for(i=0; i < j; i++)
               if(DArr->DAAxis[i] == 0)
                  break;

           if(i != j)
              DArr->PartRepl = 1; /* */    /*E0023*/

           if(TempAMV->Every)
           {  if(IsVMSBlock[0] == 0)
              {  VMSBlock[0] =
                 GetSpaceLB4Proc(0, TempAMV, ASpace, AlList, NULL,
                                 &DArr->VMSLocalBlock[0]);
                 IsVMSBlock[0] = 1;
              }

              j = VMSize - 1;

              if(IsVMSBlock[j] == 0)
              {  VMSBlock[j] =
                 GetSpaceLB4Proc(j, TempAMV, ASpace, AlList, NULL,
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

  /* ---------------------------------------------------------- */    /*E0024*/

  if(DArr->HasLocal)
  {
     DArr->Block          = block_Copy(Local);
     DArr->ArrBlock.Block = block_Copy(Local);

     Local = &DArr->ArrBlock.Block;

     if(DArr->MemPtr == NULL)      /* */    /*E0025*/  
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
        Size = bSize;  /* */    /*E0026*/

     /* */    /*E0027*/

     if(DArr->RegBufSign == 0)
     {  if(DArr->MemPtr == NULL)
        {  getdvmmem(DArr->ArrBlock.ALoc, Size);
        }
        else
        {  /* */    /*E0028*/

           DArr->ArrBlock.ALoc.Ptr0 = DArr->MemPtr;
           DArr->ArrBlock.ALoc.Ptr  = DArr->MemPtr;
           DArr->ArrBlock.ALoc.Size = Size;
        }
     }
     else
     {  /* */    /*E0029*/

        if(DVM_VMS->RemBuf == NULL)
        {  /* */    /*E0030*/

           getdvmmem(DArr->ArrBlock.ALoc, Size);
           DArr->RemBufMem = 1;
           DVM_VMS->RemBuf = DArr->ArrBlock.ALoc.Ptr0;
           DVM_VMS->RemBufSize = Size;
           DVM_VMS->FreeRemBuf = 0;
        }
        else
        {  /* */    /*E0031*/

           if(DVM_VMS->FreeRemBuf == 0)
           {  /* */    /*E0032*/

              getdvmmem(DArr->ArrBlock.ALoc, Size);
           }
           else
           {  /* */    /*E0033*/

              if(Size > DVM_VMS->RemBufSize)
              {  /* */    /*E0034*/

                 mac_free(&(DVM_VMS->RemBuf));
                 getdvmmem(DArr->ArrBlock.ALoc, Size);
                 DVM_VMS->RemBuf = DArr->ArrBlock.ALoc.Ptr0;
                 DVM_VMS->RemBufSize = Size;
              }
              else
              {  /* */    /*E0035*/

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

     if(RTL_TRACE && TstTraceEvent(call_align_))
     {  /* */    /*E0036*/

        tprintf("DArr->ArrBlock.ALoc.Ptr0=%lx  "
                "DArr->ArrBlock.ALoc.Ptr=%lx  "
                "Size=%ld\n",
                (uLLng)DArr->ArrBlock.ALoc.Ptr0,
                (uLLng)DArr->ArrBlock.ALoc.Ptr, Size);
     } 

     /* Form the header of distributed array */    /*E0037*/

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

     /* Complete  header forming for Fortran */    /*E0038*/

     if(DArr->ExtHdrSign)    /* if Fortran */    /*E0039*/
     {  Temp = AR+1;
        ArrayHeader[Temp] = ArrayHeader[AR] - ArrayHeader[AR+2];

        for(i=2; i <= AR; i++)
            ArrayHeader[Temp] -= (ArrayHeader[Temp-i]*
                                  ArrayHeader[Temp+i]);
     }

     /* Form the rest of headers of distributed array */    /*E0040*/

     for(i=0; i < DACount; i++)
     {  if(DAHeaderAddr[i][0] == ArrayHeader[0] &&
           DAHeaderAddr[i] != ArrayHeader)
        {  for(j=0; j <= AR; j++)
               DAHeaderAddr[i][j] = ArrayHeader[j];

           if(DArr->ExtHdrSign)    /* if Fortran */    /*E0041*/
           {  Temp = AR + AR + 1;

              for( ; j <= Temp; j++)
                  DAHeaderAddr[i][j] = ArrayHeader[j];
           }
        }
     }

     /* Check correctness of shadow edge widths for the distribution */    /*E0042*/

     for(i = 0; i < AR; i++)
     {  if(DArr->InitLowShdWidth[i] > DArr->Block.Set[i].Size)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                 "*** RTS err 044.027: wrong call align_ "
                 "(Low Shadow Width[%d]=%d > Loc Size=%ld)\n",
                 i, DArr->InitLowShdWidth[i], DArr->Block.Set[i].Size);

        if(DArr->InitHighShdWidth[i] > DArr->Block.Set[i].Size)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                 "*** RTS err 044.028: wrong call align_ "
                 "(High Shadow Width[%d]=%d > Loc Size=%ld)\n",
                 i, DArr->InitHighShdWidth[i], DArr->Block.Set[i].Size);
     }
     
     /* Print local part of mapped array */    /*E0043*/

     if(RTL_TRACE)
     {  if(align_Trace && TstTraceEvent(call_align_))
        {  for(i=0; i < AR; i++)
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

     /* Fill allocated array with zero code */    /*E0044*/

     if(DisArrayFill && DArr->MemPtr == NULL)
     {  if(FillCode[0] < 2)
        {  /* Filling with a byte */    /*E0045*/

           if(FillCode[0])
              i = FillCode[1];
           else
              i = '\x00';

           DArrElm = (char *)DArr->ArrBlock.ALoc.Ptr0;
           LongTemp = (DvmType)DArr->ArrBlock.ALoc.Size;
           SYSTEM(memset, (DArrElm, i, LongTemp))
        }
        else
        {  /* Filling with some bytes */    /*E0046*/

           s_BLOCK   CurrBlock;

           CurrBlock = block_Copy(&DArr->Block);
           Temp = 0;
           block_GetSize(bSize, &CurrBlock, Temp)
           LongTemp = bSize;
           DArrElm = (char *)&FillCode[1];

           for(longI=0; longI < LongTemp; longI++)
           {  index_FromBlock(TstArray, &CurrBlock, &DArr->Block, Temp)
              PutLocElm(DArrElm, DArr, TstArray)
           }
        }
     }
  }

  dvm_FreeArray(AlList);

  bSize = DArr->HasLocal;

  if(RTL_TRACE)
     dvm_trace(ret_align_,"IsLocal=%ld\n", bSize);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0047*/
  DVMFTimeFinish(ret_align_);
  return  (DVM_RET, bSize);
}



DvmType __callstd realn_(DvmType ArrayHeader[], PatternRef *PatternRefPtr,
                        DvmType AxisArray[], DvmType CoeffArray[],
                        DvmType ConstArray[], DvmType *NewSignPtr)
/*
     Realigning distributed array.
     -----------------------------

ArrayHeader	- the header of the distributed array to be realigned; 
*PatternRefPtr	- reference to the alignment pattern;
AxisArray	- AxisArray[j] is a dimension number of the distributed
                  array used in linear alignment rule for pattern
                  (j+1)th dimension. 
CoeffArray	- CoeffArray[j] is a coefficient for the distributed
                  array index variable used in linear alignment rule
                  for the pattern (j+1)th dimension.
ConstArray	- ConstArray[j] is a constant used in the linear
                  alignment rule for the pattern (j+1)th dimension.
*NewSignPtr	- the flag of updating of the distributed array.

The function realn_ cancels the allocation of the distributed array
with header *ArrayHeaderPtr, defined previously by the function align_.
The function returns nonzero value if remapped array hasa local part
on the current processor, otherwice - returns zero.
*/    /*E0048*/

{ SysHandle   *TempHandlePtr, *ArrayHandlePtr;
  s_DISARRAY  *TempDArr = NULL, *DArr;
  s_AMVIEW    *TempAMV = NULL, *DArrAMV = NULL;
  int          TR, i, j;
  DvmType         AR, TypeSize, StaticSign, ReDistrSign, CopyRegim = 0,
               Res, Temp, ExtHdrSign;
  DvmType         LowShdWidthArray[MAXARRAYDIM],
               HiShdWidthArray[MAXARRAYDIM], StepArray[MAXARRAYDIM];
  DvmType         NewArrayHeader[2 * MAXARRAYDIM + 2];
  s_VMS       *DArrVMS;
  byte         SDisArrayFill;
  s_AMS       *wAMS;
  ObjectRef    ObjRef;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0049*/
  DVMFTimeStart(call_realn_);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 044.042: wrong call realn_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              (uLLng)ArrayHandlePtr);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(TstObject)
  {  if(TstDVMObj(PatternRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 044.040: wrong call realn_\n"
                "(the pattern is not a DVM object; "
                "PatternRef=%lx)\n", *PatternRefPtr);
  }

  TempHandlePtr = (SysHandle *)*PatternRefPtr;

  switch(TempHandlePtr->Type)
  {  case sht_DisArray:

     if(RTL_TRACE)
        dvm_trace(call_realn_,
                  "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                  "PatternRefPtr=%lx; PatternRef=%lx (DisArray); "
                  "NewSign=%lx;\n",
                  (uLLng)ArrayHeader, ArrayHeader[0],
                  (uLLng)PatternRefPtr, *PatternRefPtr, *NewSignPtr);

     TempDArr = (s_DISARRAY *)TempHandlePtr->pP;
     TempAMV  = TempDArr->AMView;
     TR       = TempDArr->Space.Rank;
     break;

     case sht_AMView:

     if(RTL_TRACE)
        dvm_trace(call_realn_,
           "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
           "PatternRefPtr=%lx; PatternRef=%lx (AMView); NewSign=%lx;\n",
           (uLLng)ArrayHeader, ArrayHeader[0],
           (uLLng)PatternRefPtr, *PatternRefPtr, *NewSignPtr);

     TempAMV = (s_AMVIEW *)TempHandlePtr->pP;
     TR      = TempAMV->Space.Rank;
     break;

     default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 044.041: wrong call realn_\n"
                       "(the object is not an aligning pattern; "
                       "PatternRef=%lx)\n",
                       (uLLng)*PatternRefPtr);
  }

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_realn_))
     {  for(i=0; i < TR; i++)
            tprintf(" AxisArray[%d]=%ld; ",i,AxisArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
        tprintf("CoeffArray[%d]=%ld; ",i,CoeffArray[i]);
        tprintf(" \n");
        for(i=0; i < TR; i++)
            tprintf("ConstArray[%d]=%ld; ",i,ConstArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  /* skip realign if launched only on 1 proc */
  if ((DVM_VMS->ProcCount == 1)&&(DArr->AMView != NULL))
  {
    if(RTL_TRACE)
        dvm_trace(ret_realn_,"IsLocal=%ld\n", (DvmType)1);

    StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */
    DVMFTimeFinish(ret_realn_);

    return  (DVM_RET, (DvmType)1);
  }

     /* if array for realign is from higher context - push everything
        that will be connected with it to that higher context */
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

  if(DArr->AMView == NULL) /* if distributed array is mapped */    /*E0050*/
  {  SDisArrayFill = DisArrayFill; /* save flag of array filling 
                                      with 0- byte */    /*E0051*/
     /* is it necessary to fill in the first distributed array
        with 0-byte   */    /*E0052*/

     DisArrayFill = (byte)(DisArrayFill || *NewSignPtr == 2);

     Res = ( RTL_CALL, align_(ArrayHeader, PatternRefPtr, AxisArray,
                              CoeffArray, ConstArray) );

     DisArrayFill = SDisArrayFill; /* restore flag of array filling 
                                      with 0- byte */    /*E0053*/ 
  }
  else
  {  if( (DArr->ReDistr >> 1) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 044.043: wrong call realn_\n"
               "(ArrayHeader[0]=%lx; ReDistrPar=%d)\n",
               (uLLng)ArrayHandlePtr, (int)DArr->ReDistr);

     DArrAMV = DArr->AMView;

     /* Check if aligned array is mapped on current processor system
                or on its subsystem */    /*E0054*/

     DArrVMS = DArrAMV->VMS;

     NotSubsystem(i, DVM_VMS, DArrVMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 044.045: wrong call realn_\n"
           "(the array PS is not a subsystem of the current PS;\n"
           "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
           ArrayHeader[0], (uLLng)DArrVMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     /* Check if map pattern is mapped */    /*E0055*/

     if(TempAMV == NULL || TempAMV->VMS == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 044.047: wrong call realn_\n"
                 "(the pattern has not been mapped; "
                 "PatternRef=%lx)\n", (uLLng)TempHandlePtr);

     /* Check if align pattern os mapped on subsustem
         of current processor system  */    /*E0056*/

     NotSubsystem(i, DVM_VMS, TempAMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 044.044: wrong call realn_\n"
           "(the pattern PS is not a subsystem of the current PS;\n"
           "PatternRef=%lx; PatternPSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)TempHandlePtr, (uLLng)TempAMV->VMS->HandlePtr,
           (uLLng)DVM_VMS->HandlePtr);

     AR          = DArr->Space.Rank;
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
     /* */    /*E0057*/

     ((SysHandle *)NewArrayHeader[0])->CrtBlockInd =
     ArrayHandlePtr->CrtBlockInd;

     /* Rewrite low index values saved  the header */    /*E0058*/

     if(DArr->ExtHdrSign)  /* for extended header */    /*E0059*/
     {  Temp = 2*AR + 1;
        for(i=AR+2; i <= Temp; i++)
            NewArrayHeader[i] = ArrayHeader[i];
     }

     SDisArrayFill = DisArrayFill; /* save flag of array filling 
                                      with 0- byte */    /*E0060*/
     /* is it necessary to fill in the first distributed array
        with 0-byte  */    /*E0061*/

     DisArrayFill = (byte)((DisArrayFill && *NewSignPtr == 1) ||
                            *NewSignPtr == 2);

     Res = (RTL_CALL, align_(NewArrayHeader, PatternRefPtr, AxisArray,
                             CoeffArray, ConstArray) );

     DisArrayFill = SDisArrayFill; /* restore flag of array filling 
                                      with 0- byte  */    /*E0062*/

     if(*NewSignPtr == 0)
     {  /* Copy old array into the new one*/    /*E0063*/
     
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

     if(EnableTrace)
        trc_ArrayReRegister((SysHandle *)ArrayHeader[0],
                            (SysHandle *)NewArrayHeader[0]);

     /* Form all headers of an old array except
       ArrayHeader as headers of new array   */    /*E0064*/

     for(i=0; i < DACount; i++)
     {  if(DAHeaderAddr[i][0] == ArrayHeader[0] &&
           DAHeaderAddr[i] != ArrayHeader)
        {  for(j=0; j <= AR; j++)
               DAHeaderAddr[i][j] = NewArrayHeader[j];
           if(DArr->ExtHdrSign)    /* if Fortran */    /*E0065*/
           {  Temp = 2*AR + 1;
              for( ; j <= Temp; j++)
                  DAHeaderAddr[i][j] = NewArrayHeader[j];
           }
        }
     }

     j = DArr->ExtHdrSign; /* save flag of 
                              extended header */    /*E0066*/

     ( RTL_CALL, delda_(ArrayHeader) );/* delete old array */    /*E0067*/

     /* Form ArrayHeader as a header of new array */    /*E0068*/

     for(i=0; i <= AR; i++)
         ArrayHeader[i] = NewArrayHeader[i];

     if(j)                    /* for extended header */    /*E0069*/
     {  Temp = 2*AR + 1;

        for( ; i <= Temp; i++)
            ArrayHeader[i] = NewArrayHeader[i];
     }

     DAHeaderAddr[DACount-1] = ArrayHeader;/* at the place of
                                              NewArrayHeader */    /*E0070*/

     ((SysHandle *)NewArrayHeader[0])->HeaderPtr = (uLLng)ArrayHeader;

  }

  if(RTL_TRACE)
     dvm_trace(ret_realn_,"IsLocal=%ld\n", Res);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0072*/
  DVMFTimeFinish(ret_realn_);
  return  (DVM_RET, Res);
}


/* ------------------------------------------------ */    /*E0073*/


void AppendBounds(s_BLOCK *ABlock, s_DISARRAY *DArr)
{ int i, j, n;

  if(BoundAddition < 0 || UserSumFlag == 0 || EnableDynControl)
  {
     for(i = 0; i < ABlock->Rank; i++)
     {  if(ABlock->Set[i].Lower - DArr->InitLowShdWidth[i] >= 0)
        {  ABlock->Set[i].Lower -= DArr->InitLowShdWidth[i];
           ABlock->Set[i].Size += DArr->InitLowShdWidth[i];
        }

        if(ABlock->Set[i].Upper + DArr->InitHighShdWidth[i] <
           DArr->Space.Size[i])
        {  ABlock->Set[i].Upper += DArr->InitHighShdWidth[i];
           ABlock->Set[i].Size += DArr->InitHighShdWidth[i];
        }
     }
  }
  else
  {
     for(i = 0; i < ABlock->Rank; i++)
     {  if(ABlock->Set[i].Lower - DArr->InitLowShdWidth[i] >= 0)
        {  ABlock->Set[i].Lower -= DArr->InitLowShdWidth[i];
           ABlock->Set[i].Size += DArr->InitLowShdWidth[i];
        }

        if((ABlock->Set[i].Upper + DArr->InitHighShdWidth[i]) >=
           DArr->Space.Size[i])
        {  n = i+1;

           for(j=0; j < MaxVMRank; j++)
               if(DArr->DAAxis[j] >= n)
                  break;  /* */    /*E0074*/

           if(j < MaxVMRank)
           {  n = (int)((ABlock->Set[i].Size & 0x1) ^ 0x1);
              ABlock->Set[i].Size  += n;
              ABlock->Set[i].Upper += n;
           }
        }
        else
        {  ABlock->Set[i].Size += (DArr->InitHighShdWidth[i] +
                                   BoundAddition);
           n = (int)((ABlock->Set[i].Size & 0x1) ^ 0x1);

           ABlock->Set[i].Size  += n;
           ABlock->Set[i].Upper += (DArr->InitHighShdWidth[i] +
                                    BoundAddition + n);
        }
     }
  }

  return;
}



AMViewRef  __callstd  getamv_(DvmType  ArrayHeader[])
{ SysHandle   *ArrayHandlePtr;
  s_AMVIEW    *AMV;
  AMViewRef    Res = 0;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0075*/
  DVMFTimeStart(call_getamv_);

  if(RTL_TRACE)
     dvm_trace(call_getamv_,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(!ArrayHandlePtr)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 044.060: wrong call getamv_\n"
              "(the object is not a distributed array; "
              "ArrayHeader[0]=%lx)\n",
              ArrayHeader[0]);

  AMV = ((s_DISARRAY *)ArrayHandlePtr->pP)->AMView;

  if(AMV)
     Res  = (AMViewRef)AMV->HandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_getamv_,"AMViewRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0076*/
  DVMFTimeFinish(ret_getamv_);
  return  (DVM_RET, Res);
}


#endif   /*  _ALIGN_C_  */    /*E0077*/
