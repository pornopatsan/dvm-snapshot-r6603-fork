#ifndef _AUXILFUN_C_
#define _AUXILFUN_C_
/******************/    /*E0000*/

/***********************\ 
*   Support functions   *
\***********************/    /*E0001*/

DvmType __callstd tstda_(ObjectRef  *ObjectRefPtr)

/*
     Requesting if object is distributed array.

The function returns non-zero value if the object *ObjectRefPtr
is a distributed array, and returns zero in another case.
*/    /*E0002*/

{ DvmType      ArrayType=0;
  SysHandle    *wHandlePtr;
  s_DISARRAY   *DArr;

  StatObjectRef = *ObjectRefPtr; /* for statistics */    /*E0003*/
  DVMFTimeStart(call_tstda_);

  if(RTL_TRACE)
     dvm_trace(call_tstda_,"ObjectRefPtr=%lx; ObjectRef=%lx;\n",
                           (uLLng)ObjectRefPtr, *ObjectRefPtr);

  wHandlePtr = TstDVMArray((void *)ObjectRefPtr);

  if(wHandlePtr)
  {  DArr = (s_DISARRAY *)wHandlePtr->pP;
     ArrayType = 1+DArr->Repl;
  }

  if(RTL_TRACE)
     dvm_trace(ret_tstda_,"ArrayType=%ld;\n", ArrayType);

  DVMFTimeFinish(ret_tstda_);
  return  (DVM_RET, ArrayType);
}



DvmType __callstd getrnk_(ObjectRef  *ObjectRefPtr)

/*
     Requesting size of object.

The function getrnk_ returns the rank of the object,
defined by *ObjectRefPtr.
*/    /*E0004*/

{ DvmType Res=0;
  SysHandle *ObjectDescPtr;

  StatObjectRef = *ObjectRefPtr; /* for statistics */    /*E0005*/
  DVMFTimeStart(call_getrnk_);

  if(RTL_TRACE)
     dvm_trace(call_getrnk_,"ObjectRefPtr=%lx; ObjectRef=%lx;\n",
                            (uLLng)ObjectRefPtr, *ObjectRefPtr);

  ObjectDescPtr=(SysHandle *)*ObjectRefPtr;

  switch (ObjectDescPtr->Type)
  {  case sht_DisArray:
          Res = ((s_DISARRAY *)(ObjectDescPtr->pP))->Space.Rank;
          break;
     case sht_AMView:
          Res = ((s_AMVIEW *)(ObjectDescPtr->pP))->Space.Rank;
          break;
     case sht_VMS:
          Res = ((s_VMS *)(ObjectDescPtr->pP))->Space.Rank;
          break;
     case sht_ParLoop:
          Res = ((s_PARLOOP *)(ObjectDescPtr->pP))->Rank;
          break;
     case sht_ArrayMap:
          Res = ((s_ARRAYMAP *)(ObjectDescPtr->pP))->ArrayRank;
          break;
     case sht_AMViewMap:
          Res = ((s_AMVIEWMAP *)(ObjectDescPtr->pP))->AMViewRank;
          break;
  }

  if(RTL_TRACE)
     dvm_trace(ret_getrnk_,"Rank=%ld;\n", Res);

  DVMFTimeFinish(ret_getrnk_);
  return  (DVM_RET, Res);
}



DvmType __callstd getsiz_(ObjectRef  *ObjectRefPtr, DvmType  *AxisPtr)

/*
     Requesting size of object dimension.

The function returns the size of the object *ObjectRefPtr 
by the dimension *AxisPtr.
*/    /*E0006*/

{ DvmType Res=0;

  StatObjectRef = *ObjectRefPtr; /* for statistics */    /*E0007*/
  DVMFTimeStart(call_getsiz_);

  if(RTL_TRACE)
     dvm_trace(call_getsiz_,
               "ObjectRefPtr=%lx; ObjectRef=%lx; Axis=%ld;\n",
               (uLLng)ObjectRefPtr, *ObjectRefPtr, *AxisPtr);

  Res = GetObjectSize(*ObjectRefPtr, (int)*AxisPtr);

  if(RTL_TRACE)
     dvm_trace(ret_getsiz_,"Size=%ld;\n", Res);

  DVMFTimeFinish(ret_getsiz_);
  return  (DVM_RET, Res);
}



DvmType   __callstd  locsiz_(ObjectRef  *ObjectRefPtr, DvmType  *AxisPtr)

/*
Function locsiz_ is similar to above function getsiz_, but returns
local size of *AxisPtr-dimension (or the local size of all object). 
*/    /*E0008*/

{ DvmType   Res = 0;

  StatObjectRef = *ObjectRefPtr; /* for statistics */    /*E0009*/
  DVMFTimeStart(call_locsiz_);

  if(RTL_TRACE)
     dvm_trace(call_locsiz_,
               "ObjectRefPtr=%lx; ObjectRef=%lx; Axis=%ld;\n",
               (uLLng)ObjectRefPtr, *ObjectRefPtr, *AxisPtr);

  Res = GetObjectLocSize(*ObjectRefPtr, (int)*AxisPtr);

  if(RTL_TRACE)
     dvm_trace(ret_locsiz_,"Size=%ld;\n", Res);

  DVMFTimeFinish(ret_locsiz_);
  return  (DVM_RET, Res);
}



DvmType   __callstd  exlsiz_(DvmType ArrayHeader[], DvmType *AxisPtr)
{ DvmType          Res = 0;
  SysHandle    *ArrayHandlePtr;
  s_DISARRAY   *DArr;
  int           i, Rank;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* */    /*E0010*/
  DVMFTimeStart(call_exlsiz_);

  if(RTL_TRACE)
     dvm_trace(call_exlsiz_,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; Axis=%ld;\n",
               (uLLng)ArrayHeader, ArrayHeader[0], *AxisPtr);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 120.020: wrong call exlsiz_\n"
              "(the object is not a distributed array; "
              "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  Rank = DArr->Space.Rank;

  if(*AxisPtr < 0 || *AxisPtr > Rank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 120.021: wrong call exlsiz_\n"
              "(invalid parameter *AxisPtr; "
              "ArrayHeader=%lx; *AxisPtr=%ld)\n",
              (uLLng)ArrayHeader, *AxisPtr);

  if(DArr->HasLocal)
  {  if(*AxisPtr > 0)
        Res = DArr->ArrBlock.Block.Set[*AxisPtr-1].Size;
     else
     {  Res = 1;

        for(i=0; i < Rank; i++)
            Res *= DArr->ArrBlock.Block.Set[i].Size;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_exlsiz_,"Size=%ld;\n", Res);

  DVMFTimeFinish(ret_exlsiz_);
  return  (DVM_RET, Res);
}



DvmType __callstd getlen_(DvmType  ArrayHeader[])

/*
     Getting length of distributed array.

ArrayHeader - header of distributed array.
Function getlen_ returns length of the given
distributed array element in byte.
*/    /*E0011*/

{ DvmType             Res;
  SysHandle        *ArrayHandlePtr;
  s_DISARRAY       *DArr;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0012*/
  DVMFTimeStart(call_getlen_);

  if(RTL_TRACE)
     dvm_trace(call_getlen_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 120.000: wrong call getlen_\n"
          "(the object is not distributed array; "
          "ArrayHeader=%x)\n", (uLLng)ArrayHeader);

  DArr=(s_DISARRAY *)ArrayHandlePtr->pP;
  Res=DArr->TLen;

  if(RTL_TRACE)
     dvm_trace(ret_getlen_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_getlen_);
  return  (DVM_RET, Res);
}



DvmType  __callstd tstelm_(DvmType  ArrayHeader[], DvmType  IndexArray[])

/*
      Requesting if element is allocated
      in local part of distributed array.

ArrayHeader - the header of the distributed array.
IndexArray  - IndexArray[i] is the index of the element
              of the distributed array by (i+1)th dimension.
The function returns non-zero value if the element is allocated
in the local part of the distributed array, and returns zero
in another case.
*/    /*E0013*/

{ DvmType             Res = 0;
  int              Rank, i;
  SysHandle       *ArrayHandlePtr;
  s_DISARRAY      *DArr;
  s_REGULARSET    *Set; 

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0014*/
  DVMFTimeStart(call_tstelm_);

  if(ALL_TRACE)
     dvm_trace(call_tstelm_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
    epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 120.001: wrong call tstelm_\n"
            "(the object is not a distributed array; "
            "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  Rank = DArr->Space.Rank;

  if(ALL_TRACE)
  {  if(TstTraceEvent(call_tstelm_))
     {  int i;

        for(i=0; i < Rank; i++)
           tprintf("IndexArray[%d]=%ld; ", i, IndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

#ifndef _DVM_IOPROC_
  if(EnableDynControl)
      dyn_DisArrTestElement(ArrayHandlePtr);
#endif

  if(DArr->HasLocal)
  {  Set = DArr->Block.Set; 

     for(i=0; i < Rank; i++)
         if(IndexArray[i] < Set[i].Lower ||
            IndexArray[i] > Set[i].Upper)
            break;

     Res = (Rank == i);
  }

  if(ALL_TRACE)
     dvm_trace(ret_tstelm_,"Res=%ld;\n", Res);

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0015*/
  DVMFTimeFinish(ret_tstelm_);
  return  (DVM_RET, Res);
}



DvmType  __callstd  locind_(DvmType ArrayHeader[], DvmType InitIndexArray[],
                            DvmType LastIndexArray[])

/*
     Getting initial and final index values
     of a local part of distributed array.

ArrayHeader    - header of distributed array.
InitIndexArray - array: i- element will contain initial value of array
                 local part index along (i+1)- dimension.
LastIndexArray - array: i- element will contain final value of array 
                 local part index along (i+1)- dimension.

Sizes of arrays InitIndexArray and LastIndexArray must be equal to
dimensions of distributed array.  
The function returns non zero value if the array has a local part 
otherwise returns zero. 
*/    /*E0016*/

{ SysHandle    *ArrayHandlePtr;
  DvmType          HasLocal = 0;
  s_DISARRAY   *DArr;
  int           i;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0017*/
  DVMFTimeStart(call_locind_);

  if(RTL_TRACE)
     dvm_trace(call_locind_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 120.002: wrong call locind_\n"
             "(the object is not a distributed array; "
             "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  HasLocal = DArr->HasLocal;

  if(HasLocal)
  {  for(i=0; i < DArr->Block.Rank; i++)
     {  InitIndexArray[i] = DArr->Block.Set[i].Lower;
        LastIndexArray[i] = DArr->Block.Set[i].Upper;
     }
  }

  if(RTL_TRACE)
  {  if(HasLocal)
     {  if(TstTraceEvent(ret_locind_))
        {  for(i=0; i < DArr->Space.Rank; i++)
               tprintf("InitIndexArray[%d]=%ld; ", i, InitIndexArray[i]);
           tprintf(" \n");

           for(i=0; i < DArr->Space.Rank; i++)
               tprintf("LastIndexArray[%d]=%ld; ", i, LastIndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     dvm_trace(ret_locind_,"HasLocal=%ld;\n", HasLocal);
  } 

  DVMFTimeFinish(ret_locind_);
  return  (DVM_RET, HasLocal);
}
 


DvmType  __callstd  pllind_(DvmType InitIndexArray[], DvmType LastIndexArray[],
                            DvmType StepArray[])
{
  s_PARLOOP    *PL;
  DvmType      HasLocal = 0;
  int           i, Rank;

  DVMFTimeStart(call_pllind_);

  if(RTL_TRACE)
     dvm_trace(call_pllind_," \n");

  /* */    /*E0018*/

  PL = (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

  if(PL == NULL)
     epprintf(MultiProcErrReg1, __FILE__, __LINE__,
              "*** RTS err 120.040: wrong call pllind_\n"
              "(the current context is not a parallel loop)\n");

  if(PL->AMView == NULL && PL->Empty == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 120.041: wrong call pllind_\n"
              "(the current parallel loop has not been mapped)\n");

  Rank = PL->Rank;

  if(PL->Local && PL->HasLocal)
  {  /* */    /*E0019*/

     HasLocal = 1;

     for(i=0; i < Rank; i++)
     {
        if(PL->Invers[i])
        {  InitIndexArray[i] = PL->Local[i].Upper + PL->InitIndex[i];
           LastIndexArray[i] = PL->Local[i].Lower + PL->InitIndex[i];
           StepArray[i]      = -PL->Local[i].Step;
        }
        else
        {  InitIndexArray[i] = PL->Local[i].Lower + PL->InitIndex[i];
           LastIndexArray[i] = PL->Local[i].Upper + PL->InitIndex[i];
           StepArray[i]      = PL->Local[i].Step;
        }
     }
  }
  else
  {  /* */    /*E0020*/

     HasLocal = 0;
  }

  if(RTL_TRACE)
  {  if(HasLocal)
     {  if(TstTraceEvent(ret_pllind_))
        {  for(i=0; i < Rank; i++)
               tprintf("InitIndexArray[%d]=%ld; ", i, InitIndexArray[i]);
           tprintf(" \n");

           for(i=0; i < Rank; i++)
               tprintf("LastIndexArray[%d]=%ld; ", i, LastIndexArray[i]);
           tprintf(" \n");

           for(i=0; i < Rank; i++)
               tprintf("StepArray[%d]=%ld; ", i, StepArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     dvm_trace(ret_pllind_,"HasLocal=%ld;\n", HasLocal);
  } 

  DVMFTimeFinish(ret_pllind_);
  return  (DVM_RET, HasLocal);
}



DvmType  __callstd  exlind_(DvmType ArrayHeader[], DvmType InitIndexArray[],
                            DvmType LastIndexArray[])
{ SysHandle    *ArrayHandlePtr;
  DvmType          HasLocal = 0;
  s_DISARRAY   *DArr;
  int           i;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* */    /*E0021*/
  DVMFTimeStart(call_exlind_);

  if(RTL_TRACE)
     dvm_trace(call_exlind_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 120.030: wrong call exlind_\n"
             "(the object is not a distributed array; "
             "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  HasLocal = DArr->HasLocal;

  if(HasLocal)
  {  for(i=0; i < DArr->Block.Rank; i++)
     {  InitIndexArray[i] = DArr->ArrBlock.Block.Set[i].Lower;
        LastIndexArray[i] = DArr->ArrBlock.Block.Set[i].Upper;
     }
  }

  if(RTL_TRACE)
  {  if(HasLocal)
     {  if(TstTraceEvent(ret_exlind_))
        {  for(i=0; i < DArr->Space.Rank; i++)
               tprintf("InitIndexArray[%d]=%ld; ", i, InitIndexArray[i]);
           tprintf(" \n");

           for(i=0; i < DArr->Space.Rank; i++)
               tprintf("LastIndexArray[%d]=%ld; ", i, LastIndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     dvm_trace(ret_exlind_, "HasLocal=%ld;\n", HasLocal);
  } 

  DVMFTimeFinish(ret_exlind_);
  return  (DVM_RET, HasLocal);
}


#ifndef _DVM_IOPROC_

DvmType  __callstd setind_(DvmType ArrayHeader[], DvmType InitIndexArray[],
                           DvmType LastIndexArray[], DvmType StepArray[] )

/*
      Assignment of distributed array index values.

ArrayHeader    - header of distributed array.
InitIndexArray - array: i- element contains initial value of assigned 
                 array element index along (i+1)- dimension.
LastIndexArray - array: i- element contains final value of assigned 
                 array element index along (i+1)- dimension.
StepArray      - array: i - element contains step of (i+1)-dimension
                 index while sequential index polling.   

Function setind_ assigns initial and final values and steps for
the indexes of distributed array element.The function returns zero.
*/    /*E0022*/

{ SysHandle     *ArrayDescPtr;
  DvmType          *OutInitIndexArray, *OutLastIndexArray, *OutStepArray;
  byte           ArrRank;
  s_REGULARSET  *SetPtr;
  s_DISARRAY    *Ar;
  int            i;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0023*/
  DVMFTimeStart(call_setind_);

  if(RTL_TRACE)
     dvm_trace(call_setind_, "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                             (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayDescPtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayDescPtr == NULL)
    epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 120.003: wrong call setind_\n"
           "(the object is not a distributed array; "
           "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  Ar = (s_DISARRAY *)(ArrayDescPtr->pP);
  ArrRank = Ar->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_setind_))
     {  for(i=0; i < ArrRank; i++)
            tprintf("InitIndexArray[%d]=%ld; ", i, InitIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("LastIndexArray[%d]=%ld; ", i, LastIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("     StepArray[%d]=%ld; ", i, StepArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  dvm_AllocArray(DvmType, ArrRank, OutInitIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutLastIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutStepArray);

  if( !GetIndexArray(ArrayDescPtr, InitIndexArray, LastIndexArray,
                     StepArray, OutInitIndexArray, OutLastIndexArray,
                     OutStepArray, 0) )
      epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 120.004: wrong call setind_\n"
          "(invalid index or step; "
          "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  dvm_AllocArray(s_REGULARSET, ArrRank, SetPtr);

  for(i=0; i < ArrRank; i++)
      SetPtr[i] = rset_Build(OutInitIndexArray[i], OutLastIndexArray[i],
                             OutStepArray[i]);

  Ar->InitBlock = block_Init(ArrRank, SetPtr);
  Ar->CurrBlock = block_Copy(&Ar->InitBlock);

  dvm_FreeArray(OutInitIndexArray);
  dvm_FreeArray(OutLastIndexArray);
  dvm_FreeArray(OutStepArray);
  dvm_FreeArray(SetPtr);

  if(RTL_TRACE)
     dvm_trace(ret_setind_, " \n");

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0024*/
  DVMFTimeFinish(ret_setind_);
  return  (DVM_RET, 0);
}



DvmType  __callstd getind_(DvmType  ArrayHeader[], DvmType  NextIndexArray[])

/*
ArrayHeader    - header of distributed array.
NextIndexArray - array: i-element will contain next array index value
                 along (i+1)- dimension.

Function getind_ gets next values of the given array element indexes.
Function returns non zero value if next indexes are polled and 
returns zero if subset of distributed array elements assigned
by setind_ is exhausted.
*/    /*E0025*/

{ DvmType              Res = 0;
  int               i, Rank;
  SysHandle         *ArrayHandlePtr;
  s_DISARRAY        *DArr;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0026*/
  DVMFTimeStart(call_getind_);

  if(ALL_TRACE)
     dvm_trace(call_getind_,"ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
                            (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayHandlePtr = TstDVMArray(ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 120.005: wrong call getind_\n"
            "(the object is not a distributed array; "
            "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  Rank = DArr->Space.Rank;

  if(DArr->CurrBlock.Rank)
  {  Res = 1;

     for(i=0; i < Rank; i++)
         NextIndexArray[i] = DArr->CurrBlock.Set[i].Lower;

     for(i=Rank-1; i >= 0; i--)
         if(DArr->CurrBlock.Set[i].Size > DArr->CurrBlock.Set[i].Step)
            break;

     if(i < 0)
        DArr->CurrBlock.Rank = 0;
     else
     {  DArr->CurrBlock.Set[i].Lower += DArr->CurrBlock.Set[i].Step;
        DArr->CurrBlock.Set[i].Size  -= DArr->CurrBlock.Set[i].Step;

        for(i++; i < Rank; i++)
        {  DArr->CurrBlock.Set[i].Lower = DArr->InitBlock.Set[i].Lower;
           DArr->CurrBlock.Set[i].Size = DArr->InitBlock.Set[i].Size;
        }
     }
  }

  if(ALL_TRACE)
  {  if(TstTraceEvent(ret_getind_))
     {  for(i=0; i < Rank; i++)
            tprintf("NextIndex[%d]=%ld; ", i, NextIndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }

     dvm_trace(ret_getind_,"Res=%ld;\n", Res);
  }

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0027*/
  DVMFTimeFinish(ret_getind_);
  return  (DVM_RET, Res);
}

#endif


#ifndef _DVM_IOPROC_

DvmType  __callstd delobj_(ObjectRef *ObjectRefPtr)

/*
      Deleting object.

*ObjectRefPtr - reference to the deleted object.
The function returns zero.
*/    /*E0028*/

{ SysHandle       *ObjHandlePtr;
  s_COLLECTION    *ObjColl = NULL;
  s_ENVIRONMENT   *Env;
  int              EnvInd, CrtEnvInd, CurrEnvInd, ObjInd, PrgBlockInd;
  s_PRGBLOCK      *PB;
  void            *pP, *CurrAM;

  ObjHandlePtr = (SysHandle *)*ObjectRefPtr;

  if(TstObject)
  {  if(TstDVMObj(ObjectRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 120.006: wrong call delobj_ "
            "(the deleted object is not a DVM object;\n"
            "ObjectRef=%lx)\n", *ObjectRefPtr);
  }

  if(ObjHandlePtr == NULL || ObjHandlePtr->pP == NULL ||
     ObjHandlePtr->Type == sht_NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 120.007: wrong call delobj_\n"
            "(invalid type of the deleted object;\nObjectRef=%lx)\n",
            *ObjectRefPtr);

  /* */    /*E0029*/

  EnvInd = 0;

  switch(ObjHandlePtr->Type)
  {
    case sht_DisArray   :
         break;

    case sht_VMS        :
         break;

    case sht_AMView     :
         break;

    case sht_BoundsGroup:
         EnvInd = ShgSave;
         break;

    case sht_RedVar     :
         EnvInd = RgSave;
         break;

    case sht_RedGroup   :
         EnvInd = RgSave;
         break;

    case sht_ArrayMap   :
         break;

    case sht_AMViewMap  :
         break;

    case sht_RegBufGroup:
         break;

    case sht_DAConsistGroup:
         break;

    case sht_IdBufGroup:
         break;
  }

  if(EnvInd)
     return  (DVM_RET, 0);  /* */    /*E0030*/

  /* */    /*E0031*/

  StatObjectRef = *ObjectRefPtr; /* for statistics */    /*E0032*/
  DVMFTimeStart(call_delobj_);

  if(RTL_TRACE)
     dvm_trace(call_delobj_,"ObjectRefPtr=%lx; ObjectRef=%lx;\n",
                            (uLLng)ObjectRefPtr, *ObjectRefPtr);

  pP = ObjHandlePtr->pP;

  EnvInd     = ObjHandlePtr->EnvInd;    /* context index in which an object
                                           has been created, or -1
                                           ( for static object )*/    /*E0033*/ 
  Env = genv_GetEnvironment(EnvInd);    /* object creation context
                                           or NULL*/    /*E0034*/
  switch(ObjHandlePtr->Type)
  { case sht_DisArray   : if(Env)
                             ObjColl = &Env->DisArrList;
                          break;
    case sht_VMS        : if(Env)
                             ObjColl = &Env->VMSList;
                          break;
    case sht_AMView     : if(Env)
                             ObjColl = &Env->AMViewList;
                          break;
    case sht_BoundsGroup: if(Env)
                             ObjColl = &Env->BoundGroupList;
                          break;
    case sht_RedVar     : if(Env)
                             ObjColl = &Env->RedVars;
                          break;
    case sht_RedGroup   : if(Env)
                             ObjColl = &Env->RedGroups;
                          break;
    case sht_ArrayMap   : if(Env)
                             ObjColl = &Env->ArrMaps;
                          break;
    case sht_AMViewMap  : if(Env)
                             ObjColl = &Env->AMVMaps;
                          break;
    case sht_RegBufGroup: if(Env)
                             ObjColl = &Env->RegBufGroups;
                          break;
    case sht_DAConsistGroup: if(Env)
                                ObjColl = &Env->DAConsistGroups;
                             break;
    case sht_IdBufGroup : if(Env)
                             ObjColl = &Env->IdBufGroups;
                          break;
    default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 120.007: wrong call delobj_\n"
                      "(invalid type of the deleted object;\n"
                      "ObjectRef=%lx)\n",
                      *ObjectRefPtr);
  }

  CurrEnvInd = gEnvColl->Count - 1;     /* current context index */    /*E0035*/
  CrtEnvInd  = ObjHandlePtr->CrtEnvInd; /* context index in which an object
                                           has been created*/    /*E0036*/
  CurrAM = (void *)CurrAMHandlePtr;     /* Handle of current task */    /*E0037*/

  if(ObjHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 120.008: wrong call delobj_\n"
          "(the deleted object was not created by the current subtask;\n"
          "ObjectRef=%lx; ObjectEnvIndex=%d; CurrentEnvIndex=%d)\n",
          *ObjectRefPtr, CrtEnvInd, CurrEnvInd);

  DelObjFlag++;

  if(ObjColl)
  {  if(Env->PrgBlock.Count != 0  &&
        (ObjInd = coll_IndexOf(ObjColl, pP)) >= 0)
     {  PB = coll_At(s_PRGBLOCK *, &Env->PrgBlock,
                     Env->PrgBlock.Count-1);

        switch(ObjHandlePtr->Type)
        { case sht_DisArray   : PrgBlockInd = PB->ind_DisArray; break;
          case sht_VMS        : PrgBlockInd = PB->ind_VMS; break;
          case sht_AMView     : PrgBlockInd = PB->ind_AMView; break;
          case sht_BoundsGroup: PrgBlockInd = PB->ind_BoundsGroup;
                                break;
          case sht_RedVar     : PrgBlockInd = PB->ind_RedVars; break;
          case sht_RedGroup   : PrgBlockInd = PB->ind_RedGroups;
                                break;
          case sht_ArrayMap   : PrgBlockInd = PB->ind_ArrMaps; break;
          case sht_AMViewMap  : PrgBlockInd = PB->ind_AMVMaps; break;
          case sht_RegBufGroup: PrgBlockInd = PB->ind_RegBufGroups;
                                break;
          case sht_DAConsistGroup: PrgBlockInd = PB->ind_DAConsistGroups;
                                   break;
          case sht_IdBufGroup : PrgBlockInd = PB->ind_IdBufGroups;
                                break;
        }

        if(ObjInd <= PrgBlockInd && DVM_LEVEL == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 120.009: wrong call delobj_\n"
                    "(the deleted object was not created in "
                    "the current program block;\n"
                    "ObjectRef=%lx; ObjInd=%d <= PrgBlockInd=%d)\n",
                    *ObjectRefPtr, ObjInd, PrgBlockInd);
     }

     coll_Free(ObjColl, pP);
  }
  else
  {  switch(ObjHandlePtr->Type)
     { case sht_DisArray   : ( RTL_CALL,
                               disarr_Done((s_DISARRAY *)pP) );
                             break;
       case sht_VMS        : ( RTL_CALL,
                               vms_Done((s_VMS *)pP) );
                             break;
       case sht_AMView     : ( RTL_CALL,
                               amview_Done((s_AMVIEW *)pP) );
                             break;
       case sht_BoundsGroup: ( RTL_CALL,
                               bgroup_Done((s_BOUNDGROUP *)pP) );
                             break;
       case sht_RedVar     : ( RTL_CALL,
                               RedVar_Done((s_REDVAR *)pP) );
                             break;
       case sht_RedGroup   : ( RTL_CALL,
                               RedGroup_Done((s_REDGROUP *)pP) );
                             break;
       case sht_ArrayMap   : ( RTL_CALL,
                               ArrMap_Done((s_ARRAYMAP *)pP) );
                             break;
       case sht_AMViewMap  : ( RTL_CALL,
                               AMVMap_Done((s_AMVIEWMAP *)pP) );
                             break;
       case sht_RegBufGroup: ( RTL_CALL,
                               RegBufGroup_Done((s_REGBUFGROUP *)pP));
                             break;
       case sht_DAConsistGroup:
       ( RTL_CALL, DAConsistGroup_Done((s_DACONSISTGROUP *)pP));
                             break;
       case sht_IdBufGroup : ( RTL_CALL,
                               IdBufGroup_Done((s_IDBUFGROUP *)pP) );
                             break;
     }

     dvm_FreeStruct(pP);
  }

  DelObjFlag--;

  if(RTL_TRACE)
     dvm_trace(ret_delobj_,"\n");

  DVMFTimeFinish(ret_delobj_);
  return  (DVM_RET, 0);
}

#endif


DvmType  __callstd tstio_(void)

/*
      Requesting if current processor is I/O processor.

The function returns 1 if the current processor is
the I/O processor, and returns zero in another case.
*/    /*e0038*/

{ DvmType   Res = 0;

  DVMFTimeStart(call_tstio_);

  if(RTL_TRACE)
     dvm_trace(call_tstio_,"\n");

  if(MPS_CurrentProc == DVM_IOProc)
     Res = 1;

  if(RTL_TRACE)
     dvm_trace(ret_tstio_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_tstio_);
  return  (DVM_RET, Res);
}



AddrType  __callstd dvmadr_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getash_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getai_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getal_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getaf_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getad_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getac_(void  *VarPtr)
{
  return (AddrType)VarPtr;
}



AddrType  __callstd getach_(char  *VarPtr, DvmType  StrLength)
{
  StrLength = StrLength;
  return (AddrType)VarPtr;
}



DvmType  __callstd srmem_(DvmType *MemoryCountPtr, AddrType StartAddrArray[],
                          DvmType LengthArray[])

/*
     Sending memory areas of I/O processor.

*MemoryCountPtr	- the number of the sent areas of the memory.
StartAddrArray	- StartAddrArray[i] is the start address of 
                  (i+1)th area (adjusted to AddrType type).
LengthArray	- LenghtArray[i] is the size of the (i+1)th area
                  (in bytes).

The function srmem_ sends the memory areas of the I/O processor
to another processors. The function returns zero. 
*/    /*E0039*/

{ int    i;
  char  *MemPtr;

  DVMFTimeStart(call_srmem_);

  if(RTL_TRACE)
  {  dvm_trace(call_srmem_,"MemoryCount=%ld;\n", *MemoryCountPtr);

     if(TstTraceEvent(call_srmem_))
     {  for(i=0; i < *MemoryCountPtr; i++)
            tprintf("StartAddrArray[%ld]=%lx; ",
                    i,(uLLng)StartAddrArray[i]);
        tprintf(" \n");
        for(i=0; i < *MemoryCountPtr; i++)
            tprintf("   LengthArray[%ld]=%lx; ",i,(uLLng)LengthArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  for(i=0; i < *MemoryCountPtr; i++)
  { if(LengthArray[i] <= 0 ||
       LengthArray[i] > INT_MAX) /*(int)(((word)(-1))>>1)*/    /*E0040*/
       epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 120.200: wrong call srmem_ "
                "(LengthArray[%ld]=%ld)\n",
                i, LengthArray[i]);

    if((IsSynchr && UserSumFlag) || (LengthArray[i] & Msk3))
    {  mac_malloc(MemPtr, char *, LengthArray[i], 0);
       if(MPS_CurrentProc == DVM_IOProc)
          SYSTEM(memcpy, (MemPtr, (char *)StartAddrArray[i],
                         (int)LengthArray[i]))

       ( RTL_CALL, rtl_BroadCast(MemPtr, 1, (int)LengthArray[i],
                                 DVM_IOProc, NULL) );

       if(MPS_CurrentProc != DVM_IOProc)
          SYSTEM(memcpy, ((char *)StartAddrArray[i], MemPtr,
                          (int)LengthArray[i]))
       mac_free((void **)&MemPtr); 
    }
    else
       ( RTL_CALL, rtl_BroadCast((char *)StartAddrArray[i],
                                 1, (int)LengthArray[i], DVM_IOProc,
                                 NULL) );
  }

  if(RTL_TRACE)
     dvm_trace(ret_srmem_," \n");

  DVMFTimeFinish(ret_srmem_);
  return  (DVM_RET, 0);
}


void  __callstd bcopy_(void)  //io stat
{
  DVMFTimeStart(call_acsend_);
  return;
}

void  __callstd ecopy_(void)  //io stat
{
  DVMFTimeFinish(ret_acsend_);
  return;
}


void  __callstd biof_(void)
{
  DVMFTimeStart(call_biof_);

  if(RTL_TRACE)
     dvm_trace(call_biof_," \n");

  CurrOperFix = 1;      /* fix the current operation */    /*E0041*/
  CurrOper = IOGrp;     /* input/output operation is running */    /*E0042*/

  /* */    /*E0043*/

#ifdef _F_TIME_

  if(TimeExpendPrint > 1 && IsExpend)
  {  GrpTimesPtr = &TaskInter[UserGrp][UserGrp];

     TaskInterProductTime = GrpTimesPtr->ProductTime;
     TaskInterLostTime    = GrpTimesPtr->LostTime;
     GrpTimesPtr->ProductTime = 0.;
     GrpTimesPtr->LostTime    = 0.;
  }

  if(RTL_STAT && IsExpend)
  {  GrpTimesPtr = &CurrInterPtr[UserGrp][UserGrp];

     CurrInterProductTime = GrpTimesPtr->ProductTime;
     CurrInterLostTime    = GrpTimesPtr->LostTime;
     GrpTimesPtr->ProductTime = 0.;
     GrpTimesPtr->LostTime    = 0.;
  }

  if((RTL_STAT || TimeExpendPrint > 1) && IsExpend)
  {  FromGrpPtr = &StatGrpStack[0];

     SaveProductTime = FromGrpPtr->ProductTime; 
     SaveLostTime    = FromGrpPtr->LostTime;
     FromGrpPtr->ProductTime = 0.;
     FromGrpPtr->LostTime    = 0.;
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_biof_," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_biof_);
  return;
}



void  __callstd eiof_(void)
{
  DVMFTimeStart(call_eiof_);

  if(RTL_TRACE)
     dvm_trace(call_eiof_," \n");

  CurrOperFix = 0;

  /* */    /*E0044*/

#ifdef _F_TIME_

  if(TimeExpendPrint > 1 && IsExpend)
  {  GrpTimesPtr = &TaskInter[UserGrp][UserGrp];

     TaskInter[IOGrp][UserGrp].ProductTime += GrpTimesPtr->ProductTime;
     TaskInter[IOGrp][UserGrp].LostTime    += GrpTimesPtr->LostTime;
     GrpTimesPtr->ProductTime = TaskInterProductTime;
     GrpTimesPtr->LostTime    = TaskInterLostTime;
     TaskInterProductTime = 0.;
     TaskInterLostTime    = 0.;
  }

  if(RTL_STAT && IsExpend)
  { GrpTimesPtr = &CurrInterPtr[UserGrp][UserGrp];

    CurrInterPtr[IOGrp][UserGrp].ProductTime += GrpTimesPtr->ProductTime;
    CurrInterPtr[IOGrp][UserGrp].LostTime    += GrpTimesPtr->LostTime;
    GrpTimesPtr->ProductTime = CurrInterProductTime;
    GrpTimesPtr->LostTime    = CurrInterLostTime;
    CurrInterProductTime = 0.;
    CurrInterLostTime    = 0.;
  }

  if((RTL_STAT || TimeExpendPrint > 1) && IsExpend)
  { FromGrpPtr = &StatGrpStack[0];

    if(TimeExpendPrint > 1)
    { TaskInter[IOGrp][UserGrp].ProductTime += FromGrpPtr->ProductTime;
      TaskInter[IOGrp][UserGrp].LostTime    += FromGrpPtr->LostTime;
    }

    if(RTL_STAT)
    { CurrInterPtr[IOGrp][UserGrp].ProductTime +=
      FromGrpPtr->ProductTime;
      CurrInterPtr[IOGrp][UserGrp].LostTime    +=
      FromGrpPtr->LostTime;
    }

    FromGrpPtr->ProductTime = SaveProductTime;
    FromGrpPtr->LostTime    = SaveLostTime;
    SaveProductTime = 0.;
    SaveLostTime    = 0.;
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_eiof_," \n");

  (DVM_RET);

  DVMFTimeFinish(ret_eiof_);
  return;
}



#ifndef _DVM_IOPROC_

DvmType  __callstd  acsend_(LoopRef  *LoopRefPtr, AddrType *MemAddrPtr,
                            DvmType     *ElmTypePtr, DvmType     *ElmNumberPtr)
{ SysHandle      *LoopHandlePtr;
  s_PARLOOP      *PL;
  int             Invers, Count, Size, Proc, Rank, PLDim, PSDim, Length;
  char           *Mem, *MemPtr;
  RTL_Request     Req;
  s_VMS          *VMS;

  StatObjectRef = *LoopRefPtr; /* for statistics */    /*E0045*/
  DVMFTimeStart(call_acsend_);

  if(RTL_TRACE)
     dvm_trace(call_acsend_,
               "LoopRefPtr=%lx; LoopRef=%lx;\n"
               "MemAddr=%lx; ElmType=%ld; ElmNumber=%ld;\n",
               (uLLng)LoopRefPtr, *LoopRefPtr, *MemAddrPtr,
               *ElmTypePtr, *ElmNumberPtr);

  if(*LoopRefPtr != 1 && *LoopRefPtr != -1)
  {  /* check the given parallel loop */    /*E0046*/

     LoopHandlePtr = (SysHandle *)*LoopRefPtr;

     if(LoopHandlePtr->Type != sht_ParLoop)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.010: wrong call acsend_\n"
                 "(the object is not a parallel loop; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     if(TstObject)
     { PL=
       (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

       if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
          epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 219.011: wrong call acsend_\n"
                   "(the current context is not the parallel loop; "
                   "LoopRef=%lx)\n", *LoopRefPtr);
     }

     PL = (s_PARLOOP *)LoopHandlePtr->pP;

     if(PL->AMView == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.012: wrong call acsend_\n"
                 "(the parallel loop has not been mapped; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     VMS = PL->AMView->VMS;

     Rank = PL->Rank;

     /* search for loop distributed dimension with max number*/    /*E0047*/

     for(PLDim=Rank; PLDim > 0; PLDim--)
         if( (PSDim = GetPLMapDim(PL, PLDim)) > 0)
            break; /* distributed dimension has been found */    /*E0048*/

     if(PLDim == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.017: wrong call acsend_\n"
                 "(all dimensions of the parallel loop are replicated; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     Invers = PL->Invers[PLDim-1];
  }
  else
  {  VMS = DVM_VMS;
     PSDim = 1;

     if(*LoopRefPtr == 1)
        Invers = 0;
     else
        Invers = 1;
  }

  switch(*ElmTypePtr)
  { case  1: Size = sizeof(int);
             break;
    case  2: Size = sizeof(DvmType);
             break;
    case  3: Size = sizeof(float);
             break;
    case  4: Size = sizeof(double);
             break;
    default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 219.015: wrong call acsend_\n"
                      "(invalid element type %ld)\n", *ElmTypePtr);
  }

  Count = (int)*ElmNumberPtr;

  if(Count <= 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 219.016: wrong call acsend_\n"
              "(invalid element number %d)\n", Count);

  Mem = (char *)*MemAddrPtr;
  Length = Count * Size;

  if(Invers == 0)
  {  /* loop step on the found dimension is positive */    /*E0049*/

     Proc = GetNextProc(VMS, PSDim);

     if(Proc >= 0)
     {  if((IsSynchr && UserSumFlag) || (Length & Msk3))
        {  mac_malloc(MemPtr, char *, Length, 0);
           SYSTEM(memcpy, (MemPtr, Mem, Length))

           ( RTL_CALL, rtl_Sendnowait(MemPtr, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );

           mac_free((void **)&MemPtr);
        }
        else
        {  ( RTL_CALL, rtl_Sendnowait(Mem, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );
        }
     }
  }
  else
  {  /* loop step on the found dimension is negative */    /*E0050*/

     Proc = GetPrevProc(VMS, PSDim);

     if(Proc >= 0)
     {  if((IsSynchr && UserSumFlag) || (Length & Msk3))
        {  mac_malloc(MemPtr, char *, Length, 0);
           SYSTEM(memcpy, (MemPtr, Mem, Length))

           ( RTL_CALL, rtl_Sendnowait(MemPtr, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );

           mac_free((void **)&MemPtr);
        }
        else
        {  ( RTL_CALL, rtl_Sendnowait(Mem, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );
        }
     }
  }
   
  if(RTL_TRACE)
     dvm_trace(ret_acsend_," \n");

  DVMFTimeFinish(ret_acsend_);
  return  (DVM_RET, 0);
}



DvmType  __callstd  acrecv_(LoopRef  *LoopRefPtr, AddrType *MemAddrPtr,
                            DvmType     *ElmTypePtr, DvmType     *ElmNumberPtr)
{ SysHandle      *LoopHandlePtr;
  s_PARLOOP      *PL;
  int             Invers, Count, Size, Proc, Rank, PLDim, PSDim, Length;
  char           *Mem, *MemPtr;
  RTL_Request     Req;
  s_VMS          *VMS;

  StatObjectRef = *LoopRefPtr; /* for statistics */    /*E0051*/
  DVMFTimeStart(call_acrecv_);

  if(RTL_TRACE)
     dvm_trace(call_acrecv_,
               "LoopRefPtr=%lx; LoopRef=%lx;\n"
               "MemAddr=%lx; ElmType=%ld; ElmNumber=%ld;\n",
               (uLLng)LoopRefPtr, *LoopRefPtr, *MemAddrPtr,
               *ElmTypePtr, *ElmNumberPtr);

  if(*LoopRefPtr != 1 && *LoopRefPtr != -1)
  {  /* check the given parallel loop */    /*E0052*/

     LoopHandlePtr = (SysHandle *)*LoopRefPtr;

     if(LoopHandlePtr->Type != sht_ParLoop)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.020: wrong call acrecv_\n"
                 "(the object is not a parallel loop; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     if(TstObject)
     { PL=
       (coll_At(s_ENVIRONMENT *, gEnvColl, gEnvColl->Count-1))->ParLoop;

       if(PL != (s_PARLOOP *)LoopHandlePtr->pP)
          epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                   "*** RTS err 219.021: wrong call acrecv_\n"
                   "(the current context is not the parallel loop; "
                   "LoopRef=%lx)\n", *LoopRefPtr);
     }

     PL = (s_PARLOOP *)LoopHandlePtr->pP;

     if(PL->AMView == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.022: wrong call acrecv_\n"
                 "(the parallel loop has not been mapped; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     VMS = PL->AMView->VMS;

     Rank = PL->Rank;

     /* Search for loop distributed dimension with max number  */    /*E0053*/

     for(PLDim=Rank; PLDim > 0; PLDim--)
         if( (PSDim = GetPLMapDim(PL, PLDim)) > 0)
            break; /* distributed dimension is found */    /*E0054*/

     if(PLDim == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 219.027: wrong call acrecv_\n"
                 "(all dimensions of the parallel loop are replicated; "
                 "LoopRef=%lx)\n", *LoopRefPtr);

     Invers = PL->Invers[PLDim-1];
  }
  else
  {  VMS = DVM_VMS;
     PSDim = 1;

     if(*LoopRefPtr == 1)
        Invers = 0;
     else
        Invers = 1;
  }

  switch(*ElmTypePtr)
  { case  1: Size = sizeof(int);
             break;
    case  2: Size = sizeof(DvmType);
             break;
    case  3: Size = sizeof(float);
             break;
    case  4: Size = sizeof(double);
             break;
    default: epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 219.025: wrong call acrecv_\n"
                      "(invalid element type %ld)\n", *ElmTypePtr);
  }

  Count = (int)*ElmNumberPtr;

  if(Count <= 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 219.026: wrong call acrecv_\n"
              "(invalid element number %d)\n", Count);

  Mem = (char *)*MemAddrPtr;
  Length = Count * Size;

  if(Invers == 0)
  {  /* loop step on the found dimension is positive */    /*E0055*/

     Proc = GetPrevProc(VMS, PSDim);

     if(Proc >= 0)
     {  if((IsSynchr && UserSumFlag) || (Length & Msk3))
        {  mac_malloc(MemPtr, char *, Length, 0);

           ( RTL_CALL, rtl_Recvnowait(MemPtr, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );

           SYSTEM(memcpy, (Mem, MemPtr, Length))
           mac_free((void **)&MemPtr);
        }
        else
        {  ( RTL_CALL, rtl_Recvnowait(Mem, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );
        } 
     }
  }
  else
  {  /* loop step on the found dimension is negative*/    /*E0056*/

     Proc = GetNextProc(VMS, PSDim);

     if(Proc >= 0)
     {  if((IsSynchr && UserSumFlag) || (Length & Msk3))
        {  mac_malloc(MemPtr, char *, Length, 0);

           ( RTL_CALL, rtl_Recvnowait(MemPtr, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );

           SYSTEM(memcpy, (Mem, MemPtr, Length))
           mac_free((void **)&MemPtr);
        }
        else
        {  ( RTL_CALL, rtl_Recvnowait(Mem, Count, Size, Proc,
                                      msg_across, &Req, 0) );
           ( RTL_CALL, rtl_Waitrequest(&Req) );
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_acrecv_," \n");

  DVMFTimeFinish(ret_acrecv_);
  return  (DVM_RET, 0);
}

#endif



double  __callstd  dvtime_(void)
{
  return  (dvm_time() - Init_dvm_time + 0.000001);
}

double  __callstd  dmtime_(void)
{
  return  (MPI_AllreduceTime +  MPI_AlltoallvTime +  MPI_BcastTime +  MPI_BarrierTime +  MPI_GatherTime + SendCallTime + RecvCallTime);
}


#ifdef  _DVM_MPI_

double  DVM_Wtime(void)
{
  if(UserSumFlag)
     return  0.000001;
  else
     return  MPI_Wtime();
}

#endif


/* ----------------------------------------------------- */    /*E0057*/


void  InsDVMObj(ObjectRef  DVMObj)
{
  if(DVMObj == 0)
     return;

  if(DVMObjCount >= MaxDVMObjCount)
     epprintf(MultiProcErrReg2,__FILE__,__LINE__,
         "*** RTS fatal err 220.000: DVM Object Count = "
         "Max DVM Object Count(%d)\n",
         MaxDVMObjCount);

  DVMObjRef[DVMObjCount] = DVMObj;
  DVMObjCount++;

  return;
}



void  DelDVMObj(ObjectRef  DVMObj)
{ int  i;

  for(i=0; i < DVMObjCount; i++)
  {  if(DVMObjRef[i] == DVMObj)
     {  for( ; i < DVMObjCount; i++)
            DVMObjRef[i] = DVMObjRef[i+1];

        DVMObjCount--;
        break;
     }
  }

  return;
}



int  TstDVMObj(ObjectRef  *DVMObjRefPtr)
{ int        i;
  ObjectRef  DVMObj;

  if(DVMObjRefPtr == NULL)
     return  0;

  DVMObj = *DVMObjRefPtr;

  for(i=0; i < DVMObjCount; i++)
      if(DVMObjRef[i] == DVMObj)
         break;

  return  !(i == DVMObjCount);
}

            

SysHandle   *TstDVMArray(void *Buffer)
{ DvmType        *ArrayHeader;
  int          i;
  SysHandle   *ArrayHandlePtr;
  SysHandle   *Res = NULL;  /* initially non distributed array */    /*E0058*/

  if(Buffer == NULL)
     return  NULL;

  ArrayHeader = (DvmType *)Buffer;  /* pointer to header */    /*E0059*/

  for(i=0; i < DACount; i++)
      if(DAHeaderAddr[i] == ArrayHeader)
         break;

  if(i < DACount)
  {  if(TstObject)
     {  if(TstDVMObj((ObjectRef *)ArrayHeader) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS fatal err 220.001: invalid ArrayHeader\n"
                    "(the array is not a DVM object; "
                    "ArrayHeader=%lx ArrayHeader[0]=%lx)\n",
                    (uLLng)ArrayHeader, ArrayHeader[0]);
     }

     ArrayHandlePtr = (SysHandle *)ArrayHeader[0]; /* pointer
                                                  to descriptor */    /*E0060*/
     if(ArrayHandlePtr->Type == sht_DisArray)
        Res = ArrayHandlePtr;
  }

  return  Res;
}



DvmType  GetObjectSize(ObjectRef  ObjRef, int  Axis)
{ DvmType         Res = 0, Size, Step, AxisSize;
  int          i, Rank;
  SysHandle   *ObjectDescPtr;
  s_DISARRAY  *DA;
  s_AMVIEW    *AM;
  s_VMS       *VM;
  s_PARLOOP   *PL;

  ObjectDescPtr = (SysHandle *)ObjRef;

  switch(ObjectDescPtr->Type)
  {  case sht_DisArray:

          DA = (s_DISARRAY *)(ObjectDescPtr->pP);
          Rank = DA->Space.Rank;

          if(Axis < 0 || Axis > Rank)
             break;

          if(Axis > 0)
             Res = DA->Space.Size[Axis-1];
          else
             Res = space_GetSize(&DA->Space);

          break;

     case sht_AMView:

          AM = (s_AMVIEW *)(ObjectDescPtr->pP);

          if(Axis)
             Res = AM->Space.Size[Axis-1];
          else
             Res = space_GetSize(&AM->Space);

          break;

     case sht_VMS:

          VM = (s_VMS *)(ObjectDescPtr->pP);

          if(Axis)
             Res = VM->TrueSpace.Size[Axis-1];
          else
             Res = space_GetSize(&VM->TrueSpace);

          break;

     case sht_ParLoop:

          PL = (s_PARLOOP *)(ObjectDescPtr->pP);

          if(PL->Set == NULL)
             break;

          if(Axis)
          {  Size = PL->Set[Axis-1].Size;
             Step = PL->Set[Axis-1].Step;
             Res = Size / Step;

             if(Size % Step)
                Res++;
          }
          else
          {  Res = 1;
             Rank = PL->Rank;

             for(i=0; i < Rank; i++)
             {  Size = PL->Set[i].Size;
                Step = PL->Set[i].Step;
                AxisSize = Size / Step;

                if(Size % Step)
                   AxisSize++;

                Res *= AxisSize;
             }
          }

          break;
  }

  return   Res;
}



DvmType   GetObjectLocSize(ObjectRef  ObjRef, int  Axis)
{ DvmType         Res = 0, Size, Step, AxisSize;
  int          i, Rank;
  SysHandle   *ObjectDescPtr;
  s_DISARRAY  *DA;
  s_AMVIEW    *AM;
  s_VMS       *VM;
  s_PARLOOP   *PL;

  ObjectDescPtr = (SysHandle *)ObjRef;

  switch(ObjectDescPtr->Type)
  {  case sht_DisArray:

          DA = (s_DISARRAY *)(ObjectDescPtr->pP);
          Rank = DA->Space.Rank;

          if(DA->HasLocal == 0 || Axis < 0 || Axis > Rank)
             break;

          if(Axis > 0)
             Res = DA->Block.Set[Axis-1].Size;
          else
          {  Res = 1;

             for(i=0; i < Rank; i++)
                 Res *= DA->Block.Set[i].Size;
          }

          break;

     case sht_AMView:

          AM = (s_AMVIEW *)(ObjectDescPtr->pP);

          if(AM->HasLocal == 0)
             break;

          if(Axis)
             Res = AM->Local.Set[Axis-1].Size;
          else
          {  Res = 1;
             Rank = AM->Space.Rank;

             for(i=0; i < Rank; i++)
                 Res *= AM->Local.Set[i].Size;
          }

          break;

     case sht_VMS:

          VM = (s_VMS *)(ObjectDescPtr->pP);

          if(Axis)
             Res = VM->TrueSpace.Size[Axis-1];
          else
             Res = space_GetSize(&VM->TrueSpace);

          break;

     case sht_ParLoop:

          PL = (s_PARLOOP *)(ObjectDescPtr->pP);

          if(PL->Local == NULL || PL->HasLocal == 0)
             break;

          if(Axis)
          {  Size = PL->Local[Axis-1].Size;
             Step = PL->Local[Axis-1].Step;
             Res = Size / Step;

             if(Size % Step)
                Res++;
          }
          else
          {  Res = 1;
             Rank = PL->Rank;

             for(i=0; i < Rank; i++)
             {  Size = PL->Local[i].Size;
                Step = PL->Local[i].Step;
                AxisSize = Size / Step;

                if(Size % Step)
                   AxisSize++;

                Res *= AxisSize;
             }
          }

          break;
  }
             
  return Res;
}



/* */    /*E0061*/

s_BLOCK  *GetSpaceLB4Proc(DvmType ProcLI, s_AMVIEW *AMV, s_SPACE *Space,
                          s_ALIGN *AlignList, s_BLOCK *StepBlock,
                          s_BLOCK *LocalBlock)
{ s_BLOCK     *Res = NULL;
  s_ALIGN     *CAlign, *SpAlign;
  s_VMS       *VMS;
  s_SPACE     *VMSpace, *AMVSpace;
  s_MAP       *MapList;
  s_MAP       *CMap;
  int          AMVRank, VMRank;
  DvmType      CLower, CUpper, AMVDSize, SpLower, SpUpper;
  int          i, AMVAxis, SpAxis, Coord;
  byte         IsBlockEmpty = FALSE;
  DvmType      SpB1, SpB2;
  DvmType      *ProcSI;
  double       Temp;
  DvmType      ProcVector[MAXARRAYDIM + 1];

  if (AMV->VMS != NULL && ProcLI < AMV->VMS->ProcCount && ProcLI >=0)
  {  Res = LocalBlock;

     Res->Rank = Space->Rank;
     Coord     = Space->Rank;

     for(i=0; i < Coord; i++)
     {  Res->Set[i].Lower = 0;
        Res->Set[i].Upper = Space->Size[i]-1;
        Res->Set[i].Size  = Space->Size[i];
        Res->Set[i].Step  = 1;
     }

     if(StepBlock)
     {  for(i=0; i < Coord; i++)
            Res->Set[i].Step = StepBlock->Set[i].Step;
     }

     AMVSpace = &AMV->Space;
     VMS      = AMV->VMS;
     VMSpace  = &VMS->Space;
     AMVRank  = AMVSpace->Rank;
     VMRank   = VMSpace->Rank;

     MapList  = AMV->DISTMAP;

     ProcVector[0] = VMRank;
     ProcSI = ProcVector;

     space_GetSI(VMSpace, ProcLI, &ProcSI);

     for (i = 0; i < VMRank; i++)
     {  CMap = &MapList[AMVRank + i];

        switch (CMap->Attr)
        {  case map_NORMVMAXIS :
                AMVAxis  = CMap->Axis - 1;
                AMVDSize = AMVSpace->Size[AMVAxis];

                Coord  = (int)ProcSI[i+1];

                if(VMS == AMV->WeightVMS)
                {  if(AMV->GenBlockCoordWeight[i]) /* */    /*E0062*/
                      CLower = AMV->PrevSumGenBlockCoordWeight[i][Coord];
                   else
                   {  CLower = (DvmType)
                      (CMap->DisPar * AMV->PrevSumCoordWeight[i][Coord]);

                      if(AMV->Div[AMVAxis] != 1)
                         CLower = (CLower / AMV->Div[AMVAxis]) *
                                  AMV->Div[AMVAxis];
                   }
                }
                else
                {  CLower = (DvmType)
                   (CMap->DisPar * VMS->PrevSumCoordWeight[i][Coord]);

                   if(AMV->Div[AMVAxis] != 1)
                      CLower = (CLower / AMV->Div[AMVAxis]) *
                               AMV->Div[AMVAxis];
                } 

                if(Coord == VMSpace->Size[i]-1)
                   CUpper = AMVDSize - 1;
                else
                {  if(VMS == AMV->WeightVMS)
                   {  if(AMV->GenBlockCoordWeight[i]) /* */    /*E0063*/
                         CUpper = CLower +
                                  AMV->GenBlockCoordWeight[i][Coord] - 1;
                      else
                      { CUpper = (DvmType)(CMap->DisPar *
                                 AMV->PrevSumCoordWeight[i][Coord+1]);

                        if(AMV->Div[AMVAxis] != 1)
                           CUpper = (CUpper / AMV->Div[AMVAxis]) *
                                    AMV->Div[AMVAxis];

                        CUpper -= 1;
                      }
                   }
                   else
                   {  CUpper = (DvmType)(CMap->DisPar *
                               VMS->PrevSumCoordWeight[i][Coord+1]);

                      if(AMV->Div[AMVAxis] != 1)
                         CUpper = (CUpper / AMV->Div[AMVAxis]) *
                                  AMV->Div[AMVAxis];

                      CUpper -= 1;
                   }
                } 

                IsBlockEmpty = (byte)(IsBlockEmpty | (CLower > CUpper));
                IsBlockEmpty = (byte)(IsBlockEmpty |
                                      (CUpper >= AMVDSize));
                if(IsBlockEmpty)
                   break;

                CAlign = &AlignList[Space->Rank + AMVAxis];

                switch(CAlign->Attr)
                {  case align_NORMTAXIS :
                        SpAxis  = CAlign->Axis - 1;
                        SpAlign = &AlignList[SpAxis];

                        Temp = (double)(CLower - SpAlign->B) /
                               (double)SpAlign->A;
                        SpB1 = (DvmType)ceil(Temp);

                        Temp = (double)(CUpper - SpAlign->B) /
                               (double)SpAlign->A;
                        SpB2 = (DvmType)floor(Temp);
                        
                        if(SpB2 < 0 || SpB1 >= Space->Size[SpAxis] ||
                           SpB2 < SpB1)
                           IsBlockEmpty = TRUE;
                        else
                        {  SpLower = dvm_max(SpB1, 0);
                           SpUpper = dvm_min(SpB2,
                                             Space->Size[SpAxis]-1);
                           Res->Set[SpAxis].Lower = SpLower;
                           Res->Set[SpAxis].Upper = SpUpper;
                           Res->Set[SpAxis].Size  = SpUpper-SpLower+1;
                        }

                        break;

                   case align_BOUNDREPL :
                        SpB1 = (DvmType)ceil((double)(CLower - CAlign->B) /
                                          (double)CAlign->A);
                        SpB2 = (CUpper - CAlign->B)/CAlign->A;
                        SpLower = dvm_min(SpB1, SpB2);
                        SpUpper = dvm_max(SpB1, SpB2);
                        if(SpUpper < 0 || SpLower >= CAlign->Bound)
                           IsBlockEmpty = TRUE;

                        break;

                   case align_REPLICATE :
                        break;

                   case align_CONSTANT :
                        if(CAlign->B < CLower || CAlign->B > CUpper)
                           IsBlockEmpty = TRUE;
                        break;
                }

                break;

           case map_REPLICATE :
                break;

           case map_CONSTANT :
                if((DvmType)CMap->DisPar != ProcSI[i+1])
                   IsBlockEmpty = TRUE;
                break;
        }
     }

     if(IsBlockEmpty)
        Res = NULL;
  }

  return  Res;
}


#endif  /*  _AUXILFUN_C_  */    /*E0064*/
