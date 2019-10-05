#ifndef _DISIO_C_
#define _DISIO_C_
/***************/    /*E0000*/


/*************************************\
* Functions for distributed array I/O *
\*************************************/    /*E0001*/

DvmType  dvm_dfread(DvmType  ArrayHeader[], DvmType  Count, DVMFILE  *StreamPtr)

/*
ArrayHeader - the header of the distributed array.
Count	    - the number of the elements to read.
*StreamPtr  - the descriptor of the input file.

The function dvm_dfread reads no more then Count of the first elements
of the distributed array (in order of allocation this array in memory).
The function returns the number of the copied elements.
*/    /*E0002*/

{
    DvmType            RealCount = 1, Res = 0, PrevWeigth, PrevSize, CurrCount;
  int             i, ArrRank, CurrIndex;
  DvmType           *ArrSize, *InitIndexArray, *LastIndexArray, *StepArray,
                 *ArrWeigth, *CurrIndexArray;
  SysHandle      *ArrayHandlePtr;
  s_DISARRAY     *DArr;
  s_AMVIEW       *AMV;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0003*/
  DVMFTimeStart(call_dvm_dfread);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_dfread,
           "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
           "Count=%ld; Stream=%lx;\n",
           (uLLng)ArrayHeader, ArrayHeader[0], Count, (uLLng)StreamPtr);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.000: wrong call dvm_dfread\n"
         "(the object is not a distributed array; "
         "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV  = DArr->AMView; /* representation according to which the array is aligned*/    /*E0004*/

  if(AMV == NULL)      /* has the array been mapped? */    /*E0005*/
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.001: wrong call dvm_dfread\n"
         "(the array has not been aligned with an abstract machine "
         "representation;\nArrayHeader[0]=%lx)\n", ArrayHeader[0]);
 
  /* Check if processor system on which the array is mapped
     is a subsystem of the current processor system */    /*E0006*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 110.002: wrong call dvm_dfread\n"
          "(the array PS is not a subsystem of the current PS;\n"
          "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ArrRank = (int)( RTL_CALL, getrnk_((ObjectRef *)ArrayHeader) );

  dvm_AllocArray(DvmType, ArrRank, ArrSize);
  dvm_AllocArray(DvmType, ArrRank, InitIndexArray);
  dvm_AllocArray(DvmType, ArrRank, LastIndexArray);
  dvm_AllocArray(DvmType, ArrRank, StepArray);
  dvm_AllocArray(DvmType, ArrRank, ArrWeigth);
  dvm_AllocArray(DvmType, ArrRank, CurrIndexArray);

  for(i=1; i <= ArrRank; i++)
  {
     ArrSize[i-1]=(RTL_CALL, getsiz_((ObjectRef *)ArrayHeader,(DvmType *)&i));

     InitIndexArray[i-1] = 0;
     StepArray[i-1] = 1;
     RealCount *= ArrSize[i-1];
  }

  if(Count > 0)
     RealCount = dvm_min(RealCount, Count);

  PrevWeigth = 1;
  PrevSize = 1;

  for(i=ArrRank-1; i >= 0; i--)
  {  ArrWeigth[i] = PrevWeigth * PrevSize;
     PrevWeigth = ArrWeigth[i];
     PrevSize = ArrSize[i];
  }

  while(RealCount > 0)
  { for(i=0; i < ArrRank; i++)
    { CurrIndexArray[i]=dvm_min(ArrSize[i]-1,RealCount/ArrWeigth[i]-1);
      if(CurrIndexArray[i] < 0)
         CurrIndexArray[i] = 0; 
    }

    for(i=0; i < ArrRank; i++)
    { if(RealCount/ArrWeigth[i])
         break;
    }

    CurrIndex = i;

    for(i=0; i < ArrRank; i++)
        LastIndexArray[i] = InitIndexArray[i] + CurrIndexArray[i];

    Res += ( RTL_CALL, DisArrRead(StreamPtr,ArrayHeader,InitIndexArray, LastIndexArray,StepArray) );

    InitIndexArray[CurrIndex] = CurrIndexArray[CurrIndex];

    CurrCount = 1;        

    for(i=0; i < ArrRank; i++)
        CurrCount *= (CurrIndexArray[i] + 1);
    RealCount -= CurrCount;
  }

  dvm_FreeArray(ArrSize);
  dvm_FreeArray(InitIndexArray);
  dvm_FreeArray(LastIndexArray);
  dvm_FreeArray(StepArray);
  dvm_FreeArray(ArrWeigth);
  dvm_FreeArray(CurrIndexArray);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_dfread,"Res=%ld;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0007*/
  DVMFTimeFinish(ret_dvm_dfread);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}  


 
DvmType DisArrRead(DVMFILE *stream, DvmType ArrayHeader[],
                   DvmType InitIndexArray[],
                   DvmType LastIndexArray[],
                   DvmType StepArray[])

/*
     Reading from file to sub-array of distributed array.
     ----------------------------------------------------

*StreamPtr	- the input file descriptor.
ArrayHeader	- the header of the distributed array.
InitIndexArray	- InitIndexArray[i] is the initial value of the index
                  variable of the (i+1)th dimension of the distributed
                  array.
LastIndexArray	- LastIndexArray[i] is the last value of the index
                  variable of the (i+1)th dimension of the distributed
                  array.
StepArray	- StepArray[i] is the step lalue for the index variable
                  of the (i+1)th dimension of the distributed array.

The function returns the number of the copied elements.
*/    /*E0008*/

{ SysHandle      *ArrayDescPtr;
  DvmType            Res = 0, Res1 = 0;
  DvmType           *OutInitIndexArray, *OutLastIndexArray, *OutStepArray;
  byte            ArrRank;
  s_REGULARSET   *SetPtr;
  s_BLOCK         FB;
  s_DISARRAY     *Ar;
  int	          i;
  s_AMVIEW       *AMV;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0009*/
  DVMFTimeStart(call_DisArrRead);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_DisArrRead,
               "Stream=%lx; ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)stream, (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayDescPtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayDescPtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.003: wrong call DisArrRead\n"
         "(the object is not a distributed array; "
         "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  Ar = (s_DISARRAY *)(ArrayDescPtr->pP);
  AMV  = Ar->AMView; /* representation according to which the array is aligned */    /*E0010*/

  if(AMV == NULL)    /* has the array been mapped ? */    /*E0011*/
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.004: wrong call DisArrRead\n"
         "(the array has not been aligned with an abstract machine "
         "representation;\nArrayHeader[0]=%lx)\n", ArrayHeader[0]);
 
  /* Check if processor system on which the array is mapped
     is a subsystem of the current processor system */    /*E0012*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 110.005: wrong call DisArrRead\n"
          "(the array PS is not a subsystem of the current PS;\n"
          "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ArrRank = Ar->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_DisArrRead))
     {  for(i=0; i < ArrRank; i++)
            tprintf("InitIndexArray[%d]=%ld; ",i,InitIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("LastIndexArray[%d]=%ld; ",i,LastIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("     StepArray[%d]=%ld; ",i,StepArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  dvm_AllocArray(DvmType, ArrRank, OutInitIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutLastIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutStepArray);

  Res = GetIndexArray(ArrayDescPtr, InitIndexArray, LastIndexArray,
                      StepArray, OutInitIndexArray, OutLastIndexArray,
                      OutStepArray, 0);
  if(Res == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 110.006: wrong call DisArrRead\n"
       "(invalid index or step; "
       "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  dvm_AllocArray(s_REGULARSET, ArrRank, SetPtr);

  for(i=0; i < ArrRank; i++)
      SetPtr[i] = rset_Build(OutInitIndexArray[i], OutLastIndexArray[i],
                             OutStepArray[i]);

  FB = block_Init(ArrRank, SetPtr);

  Res1 = dvm_ReadArray(stream, Ar, &FB);

  if(Res1 == 0)
     Res = 0;

  dvm_FreeArray(OutInitIndexArray);
  dvm_FreeArray(OutLastIndexArray);
  dvm_FreeArray(OutStepArray);
  dvm_FreeArray(SetPtr);

#ifndef _DVM_IOPROC_

  if (EnableDynControl)
  { /* initialisation of distributed array */    /*E0013*/

    if(Res != 0)
       ( RTL_CALL, dread_((AddrType*)&ArrayHeader) );
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_DisArrRead,"Res=%ld;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0014*/
  DVMFTimeFinish(ret_DisArrRead);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}



DvmType dvm_dfwrite(DvmType ArrayHeader[], DvmType Count, DVMFILE *StreamPtr)

/*
ArrayHeader - the header of the distributed array.
Count	    - the number of the elements to be write.
*StreamPtr  - the descriptor of the input file.

The function dvm_dfwrite writes no more then Count of the first elements
of the distributed array (in order of allocation this array in memory).

The function returns the number of the copied elements.
*/    /*E0015*/

{
    DvmType            RealCount = 1, Res = 0, PrevWeigth, PrevSize, CurrCount;
  int             i, ArrRank, CurrIndex;
  DvmType           *ArrSize, *InitIndexArray, *LastIndexArray, *StepArray,
                 *ArrWeigth, *CurrIndexArray;
  SysHandle      *ArrayHandlePtr;
  s_DISARRAY     *DArr;
  s_AMVIEW       *AMV;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0016*/
  DVMFTimeStart(call_dvm_dfwrite);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_dvm_dfwrite,
            "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
            "Count=%ld; Stream=%lx;\n",
            (uLLng)ArrayHeader, ArrayHeader[0], Count, (uLLng)StreamPtr);

  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.009: wrong call dvm_dfwrite\n"
         "(the object is not a distributed array; "
         "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;
  AMV  = DArr->AMView; /* representation according to which the array is aligned */    /*E0017*/

  if(AMV == NULL)      /* has the array been mapped ? */    /*E0018*/
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.010: wrong call dvm_dfwrite\n"
         "(the array has not been aligned with an abstract machine "
         "representation;\nArrayHeader[0]=%lx)\n", ArrayHeader[0]);

 
  /* Check if processor system on which the array is mapped
     is a subsystem of the current processor system */    /*E0019*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 110.011: wrong call dvm_dfwrite\n"
          "(the array PS is not a subsystem of the current PS;\n"
          "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ArrRank=(int)( RTL_CALL, getrnk_((ObjectRef *)ArrayHeader) );

  dvm_AllocArray(DvmType, ArrRank, ArrSize);
  dvm_AllocArray(DvmType, ArrRank, InitIndexArray);
  dvm_AllocArray(DvmType, ArrRank, LastIndexArray);
  dvm_AllocArray(DvmType, ArrRank, StepArray);
  dvm_AllocArray(DvmType, ArrRank, ArrWeigth);
  dvm_AllocArray(DvmType, ArrRank, CurrIndexArray);

  for(i=1; i <= ArrRank; i++)
  { 
     ArrSize[i-1]=( RTL_CALL, getsiz_((ObjectRef *)ArrayHeader,(DvmType *)&i) );
 
     InitIndexArray[i-1] = 0;
     StepArray[i-1] = 1;
     RealCount *= ArrSize[i-1];
  }

  if(Count > 0)
     RealCount = dvm_min(RealCount, Count);

  PrevWeigth = 1;
  PrevSize = 1;

  for(i=ArrRank-1; i >= 0; i--)
  {  ArrWeigth[i] = PrevWeigth * PrevSize;
     PrevWeigth = ArrWeigth[i];
     PrevSize = ArrSize[i];
  } 

  while(RealCount > 0)
  { for(i=0; i < ArrRank; i++)
    { CurrIndexArray[i]=dvm_min(ArrSize[i]-1,RealCount/ArrWeigth[i]-1);
      if(CurrIndexArray[i] < 0)
         CurrIndexArray[i] = 0; 
    }
    for(i=0; i < ArrRank; i++)
    { if(RealCount/ArrWeigth[i])
         break;
    }

    CurrIndex = i;

    for(i=0; i < ArrRank; i++)
        LastIndexArray[i] = InitIndexArray[i] + CurrIndexArray[i];

    Res += ( RTL_CALL,DisArrWrite(StreamPtr,ArrayHeader,InitIndexArray,
                                  LastIndexArray,StepArray) );

    InitIndexArray[CurrIndex] = CurrIndexArray[CurrIndex];

    CurrCount = 1;
    for(i=0; i < ArrRank; i++)
        CurrCount *= (CurrIndexArray[i] + 1);
    RealCount -= CurrCount;
  }

  dvm_FreeArray(ArrSize);
  dvm_FreeArray(InitIndexArray);
  dvm_FreeArray(LastIndexArray);
  dvm_FreeArray(StepArray);
  dvm_FreeArray(ArrWeigth);
  dvm_FreeArray(CurrIndexArray);

  if(RTL_TRACE)
     dvm_trace(ret_dvm_dfwrite,"Res=%ld;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0020*/
  DVMFTimeFinish(ret_dvm_dfwrite);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}  



DvmType  DisArrWrite(DVMFILE  *stream, DvmType  ArrayHeader[],
                                    DvmType  InitIndexArray[],
                                    DvmType  LastIndexArray[],
                                    DvmType  StepArray[])

/*
     Writing sub-array of distributed array to file.
     -----------------------------------------------

*StreamPtr	- the descriptor of the output file.
ArrayHeader	- the header of the distributed array.
InitIndexArray	- InitIndexArray[i] is the initial value of the index
                  variable of the (i+1)th dimension of the distributed
                  array.
LastIndexArray	- LastIndexArray[i] is the last value of the index
                  variable of the (i+1)th dimension of the distributed
                  array.
StepArray	- StepArray[i] is the step value for the index variable 
                  of the (i+1)th dimension of the distributed array.

The function returns the number of the copied elements.
*/    /*E0021*/

{ SysHandle      *ArrayDescPtr;
  DvmType            Res = 0, Res1 = 0;
  DvmType           *OutInitIndexArray, *OutLastIndexArray, *OutStepArray;
  byte            ArrRank;
  s_REGULARSET   *SetPtr;
  s_BLOCK         FB;
  s_DISARRAY     *Ar;
  int	          i;
  s_AMVIEW       *AMV;
  byte            s_RTL_TRACE, s_StatOff;

  s_StatOff = StatOff;
  if(Is_IO_STAT == 0)
     StatOff = 1;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0022*/
  DVMFTimeStart(call_DisArrWrite);

  s_RTL_TRACE = RTL_TRACE;

  if(Is_IOFun_TRACE == 0)
     RTL_TRACE = 0;

  if(RTL_TRACE)
     dvm_trace(call_DisArrWrite,
               "Stream=%lx; ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)stream, (uLLng)ArrayHeader, ArrayHeader[0]);

  ArrayDescPtr = TstDVMArray((void *)ArrayHeader);
  if(ArrayDescPtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 110.012: wrong call DisArrWrite\n"
        "(the object is not a distributed array; "
        "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  Ar  = (s_DISARRAY *)(ArrayDescPtr->pP);
  AMV = Ar->AMView; /* representation according to which the array is aligned */    /*E0023*/

  if(AMV == NULL)    /* has the array been mapped ? */    /*E0024*/
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 110.013: wrong call DisArrWrite\n"
         "(the array has not been aligned with an abstract machine "
         "representation;\nArrayHeader[0]=%lx)\n", ArrayHeader[0]);
 
  /* Check if processor system on which the array is mapped
     is a subsystem of the current processor system */    /*E0025*/

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 110.014: wrong call DisArrWrite\n"
          "(the array PS is not a subsystem of the current PS;\n"
          "ArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ArrRank=Ar->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_DisArrWrite))
     {  for(i=0; i < ArrRank; i++)
            tprintf("InitIndexArray[%d]=%ld; ",i,InitIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("LastIndexArray[%d]=%ld; ",i,LastIndexArray[i]);
        tprintf(" \n");
        for(i=0; i < ArrRank; i++)
            tprintf("     StepArray[%d]=%ld; ",i,StepArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  dvm_AllocArray(DvmType, ArrRank, OutInitIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutLastIndexArray);
  dvm_AllocArray(DvmType, ArrRank, OutStepArray);

  Res = GetIndexArray(ArrayDescPtr, InitIndexArray, LastIndexArray,
                      StepArray, OutInitIndexArray, OutLastIndexArray,
                      OutStepArray, 0);
  if(Res == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 110.015: wrong call DisArrWrite\n"
       "(invalid index or step; "
       "ArrayHeader[0]=%lx)\n", ArrayHeader[0]);

  dvm_AllocArray(s_REGULARSET, ArrRank, SetPtr);

  for(i=0; i < ArrRank; i++)
      SetPtr[i] = rset_Build(OutInitIndexArray[i], OutLastIndexArray[i],
                             OutStepArray[i]);

  FB = block_Init(ArrRank, SetPtr);

  Res1 = dvm_WriteArray(stream, Ar, &FB);

  if(Res1 == 0)
     Res = 0;

  dvm_FreeArray(OutInitIndexArray);
  dvm_FreeArray(OutLastIndexArray);
  dvm_FreeArray(OutStepArray);
  dvm_FreeArray(SetPtr);

  if(RTL_TRACE)
     dvm_trace(ret_DisArrWrite,"Res=%ld;\n", Res);

  RTL_TRACE = s_RTL_TRACE;

  StatObjectRef = (ObjectRef)ArrayHeader[0]; /* for statistics */    /*E0026*/
  DVMFTimeFinish(ret_DisArrWrite);
  StatOff = s_StatOff;
  return  (DVM_RET, Res);
}


/* --------------------------------------------------------- */    /*E0027*/


DvmType   GetIndexArray(SysHandle *ArrayHandlePtr, DvmType InInitIndexArray[],
                     DvmType InLastIndexArray[], DvmType InStepArray[],
                     DvmType OutInitIndexArray[], DvmType OutLastIndexArray[],
                     DvmType OutStepArray[], byte Regim)
{ s_DISARRAY    *Ar;
  byte           ArrRank;
  int            i;
  DvmType           AxisSize, tlong, Res = 1;

  Ar = (s_DISARRAY *)(ArrayHandlePtr->pP);
  ArrRank = Ar->Space.Rank;

  /* Check input indexes and  step and form output indexes and step */    /*E0028*/

  for(i=0; i < ArrRank; i++)
  { AxisSize = Ar->Space.Size[i];

    OutInitIndexArray[i] = InInitIndexArray[i];
    OutLastIndexArray[i] = InLastIndexArray[i];
    OutStepArray[i] = InStepArray[i];

    if(Regim)
    { /* Index hard control regime */    /*E0029*/

      if(InInitIndexArray[i] >= AxisSize || InInitIndexArray[i] < 0)
         return 0;
      if(InLastIndexArray[i] >= AxisSize || InLastIndexArray[i] < 0)
         return 0;
      if(InInitIndexArray[i] > InLastIndexArray[i])
         return 0;
      if(InStepArray[i] <= 0)
         return 0;
    }
    else
    { /* Index soft control regime */    /*E0030*/

      if(InInitIndexArray[i] == -1)
      { OutInitIndexArray[i] = 0;
        OutLastIndexArray[i] = AxisSize - 1;
        OutStepArray[i] = 1;
      }
      else
      { OutLastIndexArray[i] = dvm_min(OutLastIndexArray[i],AxisSize-1);

        if(OutInitIndexArray[i] > OutLastIndexArray[i])
           OutInitIndexArray[i] = OutLastIndexArray[i];
        if(OutInitIndexArray[i] < 0 || OutLastIndexArray[i] < 0)
           return 0;
        if(OutInitIndexArray[i] == OutLastIndexArray[i])
           OutStepArray[i] = 1;
        if(OutStepArray[i] <= 0)
           return 0;
      }
    } 
  }

  /* Count elements in block */    /*E0031*/

  for(i=0; i < ArrRank; i++)
  { tlong = OutLastIndexArray[i] - OutInitIndexArray[i] + 1;
    AxisSize = tlong / OutStepArray[i];
    if(tlong % OutStepArray[i])
       AxisSize++;

    Res *= AxisSize;
  }

  if(RTL_TRACE && FullTrace && OutIndexTrace &&
     DVM_LEVEL <= MaxTraceLevel)
  {  for(i=0; i < ArrRank; i++)
       tprintf("ResInitIndexArray[%d]=%ld; ",i,OutInitIndexArray[i]);
     tprintf(" \n");
     for(i=0; i < ArrRank; i++)
       tprintf("ResLastIndexArray[%d]=%ld; ",i,OutLastIndexArray[i]);
     tprintf(" \n");
     for(i=0; i < ArrRank; i++)
       tprintf("     ResStepArray[%d]=%ld; ",i,OutStepArray[i]);
     tprintf(" \n");
     tprintf(" \n");
  }

  return  Res;
}


/* --------------------------------------------------- */    /*E0032*/


DvmType  dvm_ReadArray(DVMFILE *F, s_DISARRAY *Ar, s_BLOCK *FB)
{
    DvmType         FileBSize, AllocSize, ReadCount, BSize, VMSize, HiSize,
               MaxReadCount = 1;
    DvmType         ResArr[2 + sizeof(double) / sizeof(DvmType)] = { 0 };
  s_BLOCK      ReadBlock, *LocalBlock, SendBlock, Block;
  int          FAStyleArr[2+sizeof(double)/sizeof(int)] = {FA_ALL};
  byte         Step = 0;
  void        *MBuf, *Buf;
  s_SPACE     *ArSpace;
  s_ALIGN     *ArAlign;
  s_AMVIEW    *AMV;
  s_VMS       *VMS;
  int          i, Proc;
  PSRef        CurrVMRef;
  byte         IsRead = 0;
  RTL_Request  Req;

  ArSpace = &Ar->Space;     /* array index space */    /*E0033*/
  ArAlign = Ar->Align;      /* array mapping rules */    /*E0034*/
  AMV = Ar->AMView;         /* an abstract machine representation
                               on which the array is mapped */    /*E0035*/
  VMS = AMV->VMS;           /* descriptor of array processor system */    /*E0036*/
  VMSize  = VMS->ProcCount; /* the number of processors in 
                               array processor system  */    /*E0037*/
  CurrVMRef = (PSRef)DVM_VMS->HandlePtr; /* pointer to the current 
                                            processor system */    /*E0038*/

  for(i=0; i < FB->Rank; i++)
      if(FB->Set[i].Step != 1)
         Step = 1;               /* flag of step usage */    /*E0039*/

  ReadBlock = block_Copy(FB);
  HiSize = (DvmType)ceil((double)FB->Set[0].Size /
                       (double)FB->Set[0].Step );

  if (MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0040*/

     if(F == NULL)
     {  if(DAReadPlane)
        {  /* */    /*E0041*/

           FAStyleArr[0] = FA_ERROR;
           ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                     DVM_IOProc, &CurrVMRef) );
        }

        return 0;
     }

     IsRead = 0;

     #ifdef _DVM_ZLIB_

     if(F->zip)
     {  IsRead = 1;  /* */    /*E0042*/

        if(F->zlibFile == NULL)
        {  if(DAReadPlane)
           {  /* */    /*E0043*/

              FAStyleArr[0] = FA_ERROR;
              ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                     DVM_IOProc, &CurrVMRef) );
           }
 
           return 0;
 
        }
     }

     #endif

     if(IsRead == 0)
     {  if(F->File == NULL)
        {  if(DAReadPlane)
           {  /* */    /*E0044*/

              FAStyleArr[0] = FA_ERROR;
              ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                        DVM_IOProc, &CurrVMRef) );
           }

           return 0;
        }
     }

     block_GetSize(FileBSize, FB, Step)
     AllocSize = FileBSize * Ar->TLen;

     if(DAReadPlane)
     {  /* */    /*E0045*/

        mac_malloc(MBuf, void *, AllocSize, 1);

        if(MBuf == NULL)
        {  FAStyleArr[0] = FA_PLANE;
           ReadBlock.Set[0] = rset_Build(FB->Set[0].Lower,
                                         FB->Set[0].Lower,
                                         FB->Set[0].Step);
           AllocSize = (FileBSize / HiSize) * Ar->TLen;
           MaxReadCount = HiSize; 
           mac_malloc(MBuf, void *, AllocSize, 0);
        }

        ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                  DVM_IOProc, &CurrVMRef) );
     }
     else
     {  /* */    /*E0046*/

        mac_malloc(MBuf, void *, AllocSize, 0);
     }

     /* Read from file and send around distributed array */    /*E0047*/

     for(ReadCount=0; ReadCount < MaxReadCount; ReadCount++)
     {  IsRead = 0;

        #ifdef _DVM_ZLIB_

        if(F->zip)
        {  IsRead = 1;  /* */    /*E0048*/

           SYS_CALL(gzread);
           ResArr[0] += gzread(F->zlibFile, (voidp)MBuf,
                              (unsigned)AllocSize);
           SYS_RET;
        }

        #endif

        if(IsRead == 0)
        {  SYS_CALL(fread);
           ResArr[0] += AllocSize * fread(MBuf, (size_t)AllocSize, 1,
                                          F->File);
           SYS_RET;
        }

        for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;

           if((LocalBlock =
               GetSpaceLB4Proc(i, AMV, ArSpace, ArAlign, FB, &Block))
               == NULL)
              continue;

           if(block_Intersect(&SendBlock, &ReadBlock, LocalBlock,
                              FB, Step) == 0)
              continue;

           block_GetSize(BSize, &SendBlock, Step)
           BSize *= Ar->TLen;
           mac_malloc(Buf, void *, BSize, 0);

           CopyMemToSubmem(Buf, &SendBlock, MBuf, &ReadBlock,
                           Ar->TLen, Step);

           if(Proc == MPS_CurrentProc)
              CopyMemToBlock(Ar, (char *)Buf, &SendBlock, Step);
           else
           {
              ( RTL_CALL, rtl_Send(Buf, 1, (int)BSize,
                                   Proc) );
/*++++++
              ( RTL_CALL, rtl_Sendnowait(Buf, 1, (int)BSize,
                                         Proc, msg_common,
                                         &Req, 0) );
              ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0049*/
           }

           mac_free(&Buf);
        }

        ReadBlock.Set[0] =
        rset_Build(ReadBlock.Set[0].Lower + FB->Set[0].Step,
                   ReadBlock.Set[0].Lower + FB->Set[0].Step, 1);
     }

     mac_free(&MBuf);
  }
  else
  {  /* Current processor is not I/O processor */    /*E0050*/

     if(DAReadPlane)
     {  /* */    /*E0051*/

        ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                  DVM_IOProc, &CurrVMRef) );
     }

     if(VMS->HasCurrent)       /* if the current processor belongs to
                                  array processor sysytem */    /*E0052*/
     {  switch (FAStyleArr[0])
        {  case FA_ERROR  :

                return 0;

           case FA_ALL    :

                if(Ar->HasLocal == 0)
                   break;

                if(block_Intersect(&SendBlock, FB, &Ar->Block, FB,
                                   Step) == 0)
                   break;

                block_GetSize(BSize, &SendBlock, Step)
                BSize *= Ar->TLen;
                mac_malloc(Buf, void *, BSize, 0);

                ( RTL_CALL, rtl_Recv(Buf, 1, (int)BSize,
                                     DVM_IOProc) );
/*++++++
                ( RTL_CALL, rtl_Recvnowait(Buf, 1, (int)BSize,
                                           DVM_IOProc,
                                           msg_common, &Req, 0) );
                ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0053*/

                CopyMemToBlock(Ar, (char *)Buf, &SendBlock, Step);
                mac_free(&Buf);

                break;

           case FA_PLANE  :

                if(!Ar->HasLocal)
                   break;

                ReadBlock.Set[0] = rset_Build(FB->Set[0].Lower,
                                              FB->Set[0].Lower, 1);
                MaxReadCount = HiSize;

                for(ReadCount=0; ReadCount < MaxReadCount; ReadCount++)
                { if(block_Intersect(&SendBlock, &ReadBlock, &Ar->Block,
                                     FB, Step))
                  { block_GetSize(BSize, &SendBlock, Step)
                    BSize *= Ar->TLen;
                    mac_malloc(Buf, void *, BSize, 0);

                    ( RTL_CALL, rtl_Recv(Buf, 1, (int)BSize,
                                         DVM_IOProc) );
/*++++++
                    ( RTL_CALL, rtl_Recvnowait(Buf, 1, (int)BSize,
                                               DVM_IOProc,
                                               msg_common, &Req, 0) );
                    ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0054*/

                    CopyMemToBlock(Ar, (char *)Buf, &ReadBlock, Step);
                    mac_free(&Buf);
                  }

                  ReadBlock.Set[0] =
                  rset_Build(ReadBlock.Set[0].Lower + FB->Set[0].Step,
                             ReadBlock.Set[0].Lower + FB->Set[0].Step,
                             1);
                }

                break;
        }   
     }
  }

  if(DAVoidRead == 0)
  {  /* */    /*E0055*/

      (RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(DvmType), DVM_IOProc,
                               &CurrVMRef) );
  }
  else
     ResArr[0] = 0;  /* */    /*E0056*/

  return    ResArr[0];  /* */    /*E0057*/
}



DvmType  dvm_WriteArray(DVMFILE *F, s_DISARRAY *Ar, s_BLOCK *FB)
{
    DvmType         FileBSize, AllocSize, WriteCount, BSize, VMSize, HiSize,
               MaxWriteCount = 1;
    DvmType         ResArr[2 + sizeof(double) / sizeof(DvmType)] = { 0 };
  s_BLOCK      WriteBlock, *LocalBlock, RecvBlock, Block;
  int          FAStyleArr[2+sizeof(double)/sizeof(int)] = {FA_ALL};
  byte         Step = 0;
  void        *MBuf, *Buf;
  s_SPACE     *ArSpace;
  s_ALIGN     *ArAlign;
  s_AMVIEW    *AMV;
  s_VMS       *VMS;
  int          i, Proc;
  PSRef        CurrVMRef;
  byte         IsWrite = 0;
  RTL_Request  Req;

  ArSpace = &Ar->Space;     /* array index space */    /*E0058*/
  ArAlign = Ar->Align;      /* array mapping rules */    /*E0059*/
  AMV = Ar->AMView;         /* an abstract machine representation
                               on which the array is mapped */    /*E0060*/
  VMS = AMV->VMS;           /* descriptor of array processor system */    /*E0061*/
  VMSize  = VMS->ProcCount; /* the number of processors in the 
                               array processor system */    /*E0062*/
  CurrVMRef = (PSRef)DVM_VMS->HandlePtr; /* pointer to the current 
                                            processor system */    /*E0063*/

  for(i=0; i < FB->Rank; i++)
      if(FB->Set[i].Step != 1)
         Step = 1;             /* flag of step usage */    /*E0064*/

  WriteBlock = block_Copy(FB);
  HiSize = (DvmType)ceil((double)FB->Set[0].Size /
                       (double)FB->Set[0].Step );

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0065*/

     if(F == NULL)
     {  if(DAWritePlane)
        {  /* */    /*E0066*/

           FAStyleArr[0] = FA_ERROR;
           ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                     DVM_IOProc, &CurrVMRef) );
        }

        return 0;
     }

     IsWrite = 0;

     #ifdef _DVM_ZLIB_

     if(F->zip)
     {  IsWrite = 1;  /* */    /*E0067*/

        if(F->zlibFile == NULL)
        {  if(DAWritePlane)
           {  /* */    /*E0068*/

              FAStyleArr[0] = FA_ERROR;
              ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                        DVM_IOProc, &CurrVMRef) );
           }

           return 0;
        }
     }

     #endif

     if(IsWrite == 0)
     {  if(F->File == NULL)
        {  if(DAWritePlane)
           {  /* */    /*E0069*/

              FAStyleArr[0] = FA_ERROR;
              ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                        DVM_IOProc, &CurrVMRef) );
           }

           return 0;
        }
     }

     block_GetSize(FileBSize, FB, Step)
     AllocSize = FileBSize * Ar->TLen;

     if(DAWritePlane)
     {  /* */    /*E0070*/

        mac_malloc(MBuf, void *, AllocSize, 1);

        if(MBuf == NULL)
        {  if(FB->Rank == 1 && WriteStatByParts == 0 &&
              WriteStatByFwrite)
              eprintf(__FILE__,__LINE__,
                  "*** RTS fatal err: wrong call dvm_WriteArray "
                  "(no memory, array rank = 1, WriteStatByParts = 0, "
                  "WriteStatByFwrite = 1)\n");

           FAStyleArr[0] = FA_PLANE;
           WriteBlock.Set[0] = rset_Build(FB->Set[0].Lower,
                                       FB->Set[0].Lower, 1);
           AllocSize = (FileBSize / HiSize) * Ar->TLen;
           MaxWriteCount = HiSize; 
           mac_malloc(MBuf, void *, AllocSize, 0);
        }

        ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                  DVM_IOProc, &CurrVMRef) );
     }
     else
     {  /* */    /*E0071*/

        mac_malloc(MBuf, void *, AllocSize, 0);
     }

     /* Receive distributed array and write it in file */    /*E0072*/

     for(WriteCount=0; WriteCount < MaxWriteCount; WriteCount++)
     {  for(i=0; i < VMSize; i++)
        {  Proc = (int)VMS->VProc[i].lP;
           if((LocalBlock = 
               GetSpaceLB4Proc(i, AMV, ArSpace, ArAlign, FB, &Block))
               == NULL)
              continue;

           if(block_Intersect(&RecvBlock, &WriteBlock, LocalBlock,
                              FB, Step) == 0)
              continue;

           block_GetSize(BSize, &RecvBlock, Step)
           BSize *= Ar->TLen;
           mac_malloc(Buf, void *, BSize, 0);

           if(Proc == MPS_CurrentProc)
              CopyBlockToMem((char *)Buf, &RecvBlock, Ar, Step);
           else
           { 
              ( RTL_CALL, rtl_Recv(Buf, 1, (int)BSize,
                                   Proc) );
/*++++++
              ( RTL_CALL, rtl_Recvnowait(Buf, 1, (int)BSize,
                                         Proc,
                                         msg_common, &Req, 0) );
              ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0073*/

           }

           CopySubmemToMem(MBuf, &WriteBlock, Buf, &RecvBlock,
                           Ar->TLen, Step);

           mac_free(&Buf);
        }

        IsWrite = 0;

        #ifdef _DVM_ZLIB_

        if(F->zip)
        {  IsWrite = 1;  /* */    /*E0074*/

           SYS_CALL(gzwrite);
           if(WriteStat || WriteStatByFwrite == 0)
              ResArr[0] += gzwrite(F->zlibFile, (voidp)MBuf,
                                   (unsigned)AllocSize);
           SYS_RET;
        }

        #endif 

        if(IsWrite == 0)
        {  SYS_CALL(fwrite);
           if(WriteStat || WriteStatByFwrite == 0)
              ResArr[0] += AllocSize * fwrite(MBuf, (size_t)AllocSize,
                                              1, F->File);
           SYS_RET;
        }

        WriteBlock.Set[0] =
        rset_Build(WriteBlock.Set[0].Lower + FB->Set[0].Step,
                   WriteBlock.Set[0].Lower + FB->Set[0].Step, 1);
     }

     mac_free(&MBuf);
  }
  else
  {  /* Current processor is not I/O processor */    /*E0075*/

     if(DAWritePlane)
     {  /* */    /*E0076*/

        ( RTL_CALL, rtl_BroadCast(FAStyleArr, 1, sizeof(int),
                                  DVM_IOProc, &CurrVMRef) );
     }

     if(VMS->HasCurrent)       /* if the current processor belongs to
                                  array processor sysytem */    /*E0077*/
     {  switch (FAStyleArr[0])
        {  case FA_ERROR  :

                return 0;

           case FA_ALL    :

                if(!Ar->HasLocal)
                   break;

                if(!block_Intersect(&RecvBlock, FB, &Ar->Block, FB,
                                    Step))
                   break;

                block_GetSize(BSize, &RecvBlock, Step)
                BSize *= Ar->TLen;
                mac_malloc(Buf, void *, BSize, 0);
                CopyBlockToMem((char *)Buf, &RecvBlock, Ar, Step);

                ( RTL_CALL, rtl_Send(Buf, 1, (int)BSize,
                                     DVM_IOProc) );
/*++++++
                ( RTL_CALL, rtl_Sendnowait(Buf, 1, (int)BSize,
                                           DVM_IOProc, msg_common,
                                           &Req, 0) );
                ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0078*/

                mac_free(&Buf);

                break;

           case FA_PLANE  :

                if(!Ar->HasLocal)
                   break;

                WriteBlock.Set[0] = rset_Build(FB->Set[0].Lower,
                                               FB->Set[0].Lower, 1);
                MaxWriteCount = HiSize;

                for(WriteCount=0; WriteCount < MaxWriteCount;
                    WriteCount++)
                { if(block_Intersect(&RecvBlock, &WriteBlock,
                                     &Ar->Block, FB, Step))
                  {  block_GetSize(BSize, &RecvBlock, Step)
                     BSize *= Ar->TLen;
                     mac_malloc(Buf, void *, BSize, 0);
                     CopyBlockToMem((char *)Buf, &RecvBlock, Ar, Step);

                     ( RTL_CALL, rtl_Send(Buf, 1, (int)BSize,
                                          DVM_IOProc) );
/*++++++
                     ( RTL_CALL, rtl_Sendnowait(Buf, 1, (int)BSize,
                                                DVM_IOProc, msg_common,
                                                &Req, 0) );
                     ( RTL_CALL, rtl_Waitrequest(&Req) );
*/    /*e0079*/

                     mac_free(&Buf);
                  }

                  WriteBlock.Set[0] =
                  rset_Build(WriteBlock.Set[0].Lower + FB->Set[0].Step,
                             WriteBlock.Set[0].Lower + FB->Set[0].Step,
                             1);
                }

                break;
        }   
     }
  }

  if(DAVoidWrite == 0)
  {  /* */    /*E0080*/

      (RTL_CALL, rtl_BroadCast(ResArr, 1, sizeof(DvmType), DVM_IOProc, &CurrVMRef) );
  }
  else
     ResArr[0] = 0;  /* */    /*E0081*/     

  return    ResArr[0];  /* */    /*E0082*/
}


#endif  /* _DISIO_C_ */    /*E0083*/
