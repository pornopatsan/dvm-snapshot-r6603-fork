#ifndef _ELMCOPY_C_
#define _ELMCOPY_C_
/*****************/    /*E0000*/

/***************************************************\
* Functions to copy an element of distributed array *
\***************************************************/    /*E0001*/

DvmType __callstd rwelm_(DvmType FromArrayHeader[], DvmType ToArrayHeader[], DvmType IndexArray[])

/*
     Reading distributed array element and assigning value to element.
     -----------------------------------------------------------------

FromArrayHeader	- the header of the source distributed array
                  or the pointer to the source memory area.
ToArrayHeader   - the header of the distributed array, which contains
                  the target element, or the pointer to the target
                  memory area.
IndexArray      - IndexArray[i] is an index of the source
                  or target element on the (i+1)th dimension.

The function returns the number of bytes actually read or written
(that is the element size of the source or target array).
*/    /*E0002*/

{ SysHandle    *FromArrayHandlePtr, *ToArrayHandlePtr;
  DvmType          Res = 0;
  s_DISARRAY   *FromDArr, *ToDArr;
  s_AMVIEW     *AMV;
  int           i;

  DVMFTimeStart(call_rwelm_);
  
  /* Forward to the next element of message tag circle tag_DACopy
     for the current processor system */    /*E0003*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0004*/

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.000: wrong call rwelm_\n"
              "(FromArray and ToArray are not distributed arrays;\n"
              "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
              (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

     if(RTL_TRACE)
     {  dvm_trace(call_rwelm_,
                  "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                  "ToBufferPtr=%lx;\n",
                  (uLLng)FromArrayHeader, FromArrayHeader[0],
                  (uLLng)ToArrayHeader);

        if(TstTraceEvent(call_rwelm_))
        {  int i;

           for(i=0; i < FromDArr->Space.Rank; i++)
               tprintf("IndexArray[%d]=%ld; ",i,IndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     AMV = FromDArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.002: wrong call rwelm_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.004: wrong call rwelm_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

     if(FromDArr->Repl && AMV->VMS == DVM_VMS)
        Res = GetElmRepl((char *)ToArrayHeader,FromDArr,IndexArray);
     else
        Res = GetElm((char *)ToArrayHeader,FromDArr,IndexArray);

  }
  else
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);
     if(FromArrayHandlePtr)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.001: wrong call rwelm_ "
              "(FromArray and ToArray are distributed arrays;\n"
              "FromArrayHeader[0]=%lx; ToArrayHeader[0]=%lx)\n",
              FromArrayHeader[0], ToArrayHeader[0]);

     ToDArr=(s_DISARRAY *)ToArrayHandlePtr->pP;

     if(RTL_TRACE)
     {  dvm_trace(call_rwelm_,
                  "FromBufferPtr=%lx; ToArrayHeader=%lx; "
                  "ToArrayHandlePtr=%lx;\n",
                  (uLLng)FromArrayHeader, (uLLng)ToArrayHeader,
                  ToArrayHeader[0]);

        if(TstTraceEvent(call_rwelm_))
        {  for(i=0; i < ToDArr->Space.Rank; i++)
               tprintf("IndexArray[%d]=%ld; ",i,IndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     AMV = ToDArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.003: wrong call rwelm_\n"
              "(ToArray has not been aligned; "
              "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.005: wrong call rwelm_\n"
          "(the ToArray PS is not a subsystem of the current PS;\n"
          "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

     if(ToDArr->Repl && AMV->VMS == DVM_VMS)
        Res = PutElmRepl((char *)FromArrayHeader,ToDArr,IndexArray);
     else
        Res = PutElm((char *)FromArrayHeader,ToDArr,IndexArray);

  }

  if(RTL_TRACE)
     dvm_trace(ret_rwelm_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_rwelm_);
  return  (DVM_RET, Res);
}



/*       For FORTRAN        */    /*E0005*/


DvmType __callstd rwelmf_(DvmType FromArrayHeader[], AddrType *ToArrayHeaderPtr, DvmType IndexArray[])

/*
    To avoid warnings of FORTRAN compiler while calling  rwelm_ with
different types of parameters for a distributed array  element reading,
there is function rwelmf_ in support system.

rwelmf_ differs from function rwelm_  by the second parameter:
*ToArrayHeaderPtr - pointer to memory where a distributed array element
                    is to be read.
Other parameters of rwelmf_  and  rwelm_  are the same.
*/    /*E0006*/

{
    return  rwelm_(FromArrayHeader, (DvmType *)*ToArrayHeaderPtr,
                 IndexArray);
}

/*   --------------------   */    /*E0007*/



DvmType __callstd copelm_(DvmType FromArrayHeader[], DvmType FromIndexArray[],
                          DvmType ToArrayHeader[],  DvmType ToIndexArray[])

/*
     Copying one element of distributed array to another.
     ----------------------------------------------------

FromArrayHeader	- the header of the source distributed array.
FromIndexArray	- FromIndexArray[i] is the index of the source element
                  on the (i+1)th dimension.
ToArrayHeader	- the header of the target distributed array.
ToIndexArray	- ToIndexArray[i] is the index of the target element
                  on the (i+1)th dimension.

The types of the source and target elements have to be the same.
The function returns the number of the copied bytes.

*/    /*E0008*/

{ SysHandle    *FromArrayHandlePtr,*ToArrayHandlePtr;
  DvmType          Res=0;
  s_DISARRAY   *FromDArr,*ToDArr;
  int           i;
  s_AMVIEW     *AMV;

  DVMFTimeStart(call_copelm_);

  /* Forward to the next element of message tag circle tag_DACopy
     for the current processor system */    /*E0009*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0010*/

  if(RTL_TRACE)
     dvm_trace(call_copelm_,
               "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
               "ToArrayHeader=%lx; ToArrayHandlePtr=%lx;\n",
               (uLLng)FromArrayHeader, FromArrayHeader[0],
               (uLLng)ToArrayHeader, ToArrayHeader[0]);

  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

  if(FromArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 090.010: wrong call copelm_\n"
        "(FromArray is not a distributed array;\n"
        "FromArrayHeader=%lx)\n", (uLLng)FromArrayHeader);

  FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

  AMV = FromDArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.012: wrong call copelm_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.014: wrong call copelm_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.011: wrong call copelm_ "
          "(ToArray is not a distributed array;\n"
          "ToArrayHeader=%lx)\n", (uLLng)ToArrayHeader);

  ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

  AMV = ToDArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.013: wrong call copelm_\n"
              "(ToArray has not been aligned; "
              "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.015: wrong call copelm_\n"
          "(the ToArray PS is not a subsystem of the current PS;\n"
          "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_copelm_))
     {  for(i=0; i < FromDArr->Space.Rank; i++)
            tprintf("FromIndexArray[%d]=%ld; ",i,FromIndexArray[i]);
        tprintf(" \n");

        for(i=0; i < ToDArr->Space.Rank; i++)
            tprintf("  ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(FromDArr->Repl && FromDArr->AMView->VMS == DVM_VMS)
     Res = CopyElmRepl(FromDArr,FromIndexArray,ToDArr,ToIndexArray);
  else
     Res = CopyElm(FromDArr,FromIndexArray,ToDArr,ToIndexArray);

  if(RTL_TRACE)
     dvm_trace(ret_copelm_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_copelm_);
  return  (DVM_RET, Res);
}



DvmType __callstd elmcpy_(DvmType FromArrayHeader[], DvmType FromIndexArray[],
                          DvmType ToArrayHeader[],DvmType ToIndexArray[],
                          DvmType *CopyRegimPtr)

/*
     Unified coping of element of distributed array.
     -----------------------------------------------

FromArrayHeader	- the header of the source distributed array,
                  or the pointer to the source memory area.
FromIndexArray	- FromIndexArray[i] is the index of the source element
                  on the (i+1)th dimension.
ToArrayHeader	- the header of the target distributed array, 
                  or the pointer to the target memory area.
ToIndexArray	- ToIndexArray[i] is the index of the target element
                  on the (i+1)th dimension.
*CopyRegimPtr	- the mode of copying.

The function returns the number of the copied bytes.
*/    /*E0011*/

{ SysHandle    *FromArrayHandlePtr,*ToArrayHandlePtr;
  DvmType          Res=0;
  s_DISARRAY   *FromDArr,*ToDArr;
  int           i;
  s_AMVIEW     *AMV;

  DVMFTimeStart(call_elmcpy_);
  
  /* Forward to the next element of message tag circle tag_DACopy
     for the current processor system */    /*E0012*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0013*/

  ToArrayHandlePtr=TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr=TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.020: wrong call elmcpy_\n"
             "(FromArray and ToArray are not distributed arrays;\n"
             "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
             (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     FromDArr=(s_DISARRAY *)FromArrayHandlePtr->pP;

     AMV = FromDArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.021: wrong call elmcpy_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.023: wrong call elmcpy_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

     if(RTL_TRACE)
     {  dvm_trace(call_elmcpy_,
                  "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                  "ToBufferPtr=%lx; CopyRegim=%ld;\n",
                  (uLLng)FromArrayHeader, FromArrayHeader[0],
                  (uLLng)ToArrayHeader, *CopyRegimPtr);

        if(TstTraceEvent(call_elmcpy_))
        {  for(i=0; i < FromDArr->Space.Rank; i++)
               tprintf("FromIndexArray[%d]=%ld; ",i,FromIndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     if(*CopyRegimPtr)
     {  if(FromDArr->Repl && AMV->VMS == DVM_VMS)
           Res = IOGetElmRepl((char *)ToArrayHeader,FromDArr, FromIndexArray);
        else
           Res = IOGetElm((char *)ToArrayHeader,FromDArr, FromIndexArray);
     }
     else
     {  if(FromDArr->Repl && AMV->VMS == DVM_VMS)
           Res = GetElmRepl((char *)ToArrayHeader,FromDArr, FromIndexArray);
        else
           Res = GetElm((char *)ToArrayHeader,FromDArr,FromIndexArray);
     }
  }
  else
  {  FromArrayHandlePtr=TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
     {  ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

        AMV = ToDArr->AMView;

        if(AMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.022: wrong call elmcpy_\n"
                    "(ToArray has not been aligned; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.024: wrong call elmcpy_\n"
                    "(the ToArray PS is not a subsystem "
                    "of the current PS;\n"
                    "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
                    (uLLng)DVM_VMS->HandlePtr);

        if(RTL_TRACE)
        {  dvm_trace(call_elmcpy_,
                     "FromBufferPtr=%lx; ToArrayHeader=%lx; "
                     "ToArrayHandlePtr=%lx; CopyRegim=%ld;\n",
                     (uLLng)FromArrayHeader, (uLLng)ToArrayHeader,
                     ToArrayHeader[0], *CopyRegimPtr);

           if(TstTraceEvent(call_elmcpy_))
           {  for(i=0; i < ToDArr->Space.Rank; i++)
                  tprintf("ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
              tprintf(" \n");
              tprintf(" \n");
           }
        }
 
        if(*CopyRegimPtr)
        {  if(ToDArr->Repl && AMV->VMS == DVM_VMS)
              Res = IOPutElmRepl((char *)FromArrayHeader,ToDArr, ToIndexArray);
           else
              Res = IOPutElm((char *)FromArrayHeader,ToDArr, ToIndexArray);
        }
        else
        {  if(ToDArr->Repl && AMV->VMS == DVM_VMS)
              Res = PutElmRepl((char *)FromArrayHeader,ToDArr, ToIndexArray);
           else
              Res = PutElm((char *)FromArrayHeader,ToDArr,ToIndexArray);
        }
     }
     else
     {  FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

        AMV = FromDArr->AMView;

        if(AMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.021: wrong call elmcpy_\n"
                    "(FromArray has not been aligned; "
                    "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.023: wrong call elmcpy_\n"
                    "(the FromArray PS is not a subsystem "
                    "of the current PS;\n"
                    "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
                    (uLLng)DVM_VMS->HandlePtr);


        ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

        AMV = ToDArr->AMView;

        if(AMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.022: wrong call elmcpy_\n"
                    "(ToArray has not been aligned; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 090.024: wrong call elmcpy_\n"
                    "(the ToArray PS is not a subsystem "
                    "of the current PS;\n"
                    "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
                    (uLLng)DVM_VMS->HandlePtr);

        if(RTL_TRACE)
        {  dvm_trace(call_elmcpy_,
                     "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                     "ToArrayHeader=%lx; ToArrayHandlePtr=%lx;\n",
                     (uLLng)FromArrayHeader, FromArrayHeader[0],
                     (uLLng)ToArrayHeader, ToArrayHeader[0]);

           if(TstTraceEvent(call_elmcpy_))
           {  for(i=0; i < FromDArr->Space.Rank; i++)
                  tprintf("FromIndexArray[%d]=%ld; ",
                          i,FromIndexArray[i]);
              tprintf(" \n");

              for(i=0; i < ToDArr->Space.Rank; i++)
                  tprintf(" ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
              tprintf(" \n");
              tprintf(" \n");
           }
        }

        if(FromDArr->Repl && FromDArr->AMView->VMS == DVM_VMS)
           Res=CopyElmRepl(FromDArr,FromIndexArray,ToDArr,ToIndexArray);
        else
           Res=CopyElm(FromDArr,FromIndexArray,ToDArr,ToIndexArray);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_elmcpy_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_elmcpy_);
  return  (DVM_RET, Res);
}



DvmType __callstd rlocel_(DvmType ArrayHeader[], DvmType IndexArray[], void *BufferPtr)

/*
     Read an element of a distributed array local part.
     --------------------------------------------------

ArrayHeader - header of a distributed array.
IndexArray  - array: i- element contains index value of a read 
              element along (i+1)- dimension.
BufferPtr   - pointer to the memory where read element content value 
              will be written.  

Function returns the length of distributed array elements in bytes.
*/    /*E0014*/

{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArr;
  DvmType           Res;
  int            i;

  DVMFTimeStart(call_rlocel_);
  
  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.030: wrong call rlocel_\n"
              "(the object is not a distributed array;\n"
              "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 090.031: wrong call rlocel_\n"
              "(the array has not been aligned; ArrayHeader[0]=%lx)\n",
              ArrayHeader[0]);

  if(ALL_TRACE)
  {  dvm_trace(call_rlocel_,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx; BufferPtr=%lx;\n",
               (uLLng)ArrayHeader, ArrayHeader[0], (uLLng)BufferPtr);

     if(TstTraceEvent(call_rlocel_))
     {  for(i=0; i < DArr->Space.Rank; i++)
            tprintf("IndexArray[%d]=%ld; ",i,IndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  GetLocElm(DArr, IndexArray, BufferPtr)
  Res = DArr->TLen;

  if(ALL_TRACE)
     dvm_trace(ret_rlocel_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_rlocel_);
  return  (DVM_RET, Res);
}



DvmType __callstd wlocel_(void *BufferPtr, DvmType ArrayHeader[], DvmType IndexArray[])

/*
     Assign value to a local part element of a distributed array.
     ------------------------------------------------------------

ArrayHeader - header of distributed array.
IndexArray  - array: i- element contains index value of a modified  
              element along (i+1)- dimension of the distributed array.
BufferPtr   - pointer to memory where assigned value is.

  Function returns the length of distributed array elements in bytes.
*/    /*E0015*/

{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArr;
  DvmType           Res;
  int            i;

  DVMFTimeStart(call_wlocel_);
  
  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.040: wrong call wlocel_\n"
             "(the object is not a distributed array;\n"
             "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.041: wrong call wlocel_\n"
             "(the array has not been aligned; ArrayHeader[0]=%lx)\n",
             ArrayHeader[0]);

  if(ALL_TRACE)
  {  dvm_trace(call_wlocel_,
               "BufferPtr=%lx; ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)BufferPtr, (uLLng)ArrayHeader, ArrayHeader[0]);

     if(TstTraceEvent(call_wlocel_))
     {  for(i=0; i < DArr->Space.Rank; i++)
            tprintf("IndexArray[%d]=%ld; ",i,IndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  PutLocElm(BufferPtr, DArr, IndexArray)
  Res = DArr->TLen;

  if(ALL_TRACE)
     dvm_trace(ret_wlocel_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_wlocel_);
  return  (DVM_RET, Res);
}



DvmType __callstd clocel_(DvmType FromArrayHeader[], DvmType FromIndexArray[],
                          DvmType ToArrayHeader[],  DvmType ToIndexArray[])

/*
 Copy a local part element of some distributed array into a local part
 ---------------------------------------------------------------------
                element of another distributed array.
                -------------------------------------

FromArrayHeader - header of a read distributed array.
FromIndexArray  - array: i-element contains index value of a read  
                  element along (i+1)-dimension.
ToArrayHeader   - header of a distributed array, where an element
                  is to be written.
ToIndexArray    - array: j-element contains index value of a modified  
                  element along (j+1)-dimension.

The function returns number of copied bytes.
*/    /*E0016*/

{ SysHandle    *FromArrayHandlePtr,*ToArrayHandlePtr;
  DvmType          Res=0;
  s_DISARRAY   *FromDArr,*ToDArr;
  int           i;

  DVMFTimeStart(call_clocel_);

  if(ALL_TRACE)
     dvm_trace(call_clocel_,
               "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
               "ToArrayHeader=%lx; ToArrayHandlePtr=%lx;\n",
               (uLLng)FromArrayHeader, FromArrayHeader[0],
               (uLLng)ToArrayHeader, ToArrayHeader[0]);

  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

  if(FromArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 090.050: wrong call clocel_\n"
        "(FromArray is not a distributed array;\n"
        "FromArrayHeader=%lx)\n", (uLLng)FromArrayHeader);

  FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;
         
  if(FromDArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.052: wrong call clocel_\n"
             "(FromArray has not been aligned; "
             "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 090.051: wrong call clocel_\n"
          "(ToArray is not a distributed array;\n"
          "ToArrayHeader=%lx)\n", (uLLng)ToArrayHeader);

  ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

  if(ToDArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.053: wrong call clocel_\n"
             "(ToArray has not been aligned; "
             "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

  if(ALL_TRACE)
  {  if(TstTraceEvent(call_clocel_))
     {  for(i=0; i < FromDArr->Space.Rank; i++)
            tprintf("FromIndexArray[%d]=%ld; ",i,FromIndexArray[i]);
        tprintf(" \n");

        for(i=0; i < ToDArr->Space.Rank; i++)
            tprintf("  ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  CopyLocElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray)
  Res = ToDArr->TLen;

  if(ALL_TRACE)
     dvm_trace(ret_clocel_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_clocel_);
  return  (DVM_RET, Res);
}



char *GetLocElmAddr(DvmType ArrayHeader[], DvmType IndexArray[])

/*
      Gettind distributed array local part element address. 
      -----------------------------------------------------

ArrayHeader - header of distributed array.
IndexArray  - array: i-element contains index value of an element
              along (j+1)-dimension of distributed array.
Function returns pointer to the first byte of an element.
*/    /*E0017*/

{ SysHandle     *ArrayHandlePtr;
  s_DISARRAY    *DArr;
  char          *ElmAddr;
  int            i;

  DVMFTimeStart(call_GetLocElmAddr);
  
  ArrayHandlePtr = TstDVMArray((void *)ArrayHeader);

  if(ArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
       "*** RTS err 090.060: wrong call GetLocElmAddr\n"
       "(the object is not a distributed array;\n"
       "ArrayHeader=%lx)\n", (uLLng)ArrayHeader);

  DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

  if(DArr->AMView == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 090.061: wrong call GetLocElmAddr\n"
             "(the array has not been aligned; ArrayHeader[0]=%lx)\n",
             ArrayHeader[0]);

  if(ALL_TRACE)
  {  dvm_trace(call_GetLocElmAddr,
               "ArrayHeader=%lx; ArrayHandlePtr=%lx;\n",
               (uLLng)ArrayHeader, ArrayHeader[0]);

     if(TstTraceEvent(call_GetLocElmAddr))
     {  for(i=0; i < DArr->Space.Rank; i++)
            tprintf("IndexArray[%d]=%ld; ", i, IndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  LocElmAddr(ElmAddr, DArr, IndexArray)

  if(ALL_TRACE)
     dvm_trace(ret_GetLocElmAddr, "ElmAddr=%lx;\n", (uLLng)ElmAddr);

  DVMFTimeFinish(ret_GetLocElmAddr);
  return  (DVM_RET, ElmAddr);
}


/* -------------------------------------------------- */    /*E0018*/


int  GetElm(char  *BufferPtr, s_DISARRAY  *DArr, DvmType  IndexArray[])
{ s_AMVIEW     *AMS;
  s_VMS        *VMS;
  int           VMSize, DVM_VMSize, i, j, Proc;
  s_BLOCK      *wLocalBlock, Block;
  RTL_Request  *SendReq, *RecvReq;
  int          *SendFlag, *RecvFlag;
  char         *_BufferPtr = NULL;

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;
  DVM_VMSize = DVM_VMS->ProcCount;

  dvm_AllocArray(RTL_Request, DVM_VMSize, SendReq);
  dvm_AllocArray(RTL_Request, DVM_VMSize, RecvReq);
  dvm_AllocArray(int, DVM_VMSize, SendFlag);
  dvm_AllocArray(int, DVM_VMSize, RecvFlag);

  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < DVM_VMSize; i++)
  {  SendFlag[i] = 0;
     RecvFlag[i] = 0;
  }

  if(DArr->HasLocal)
     IsElmOfBlock(i, &DArr->Block, IndexArray)
  else
     i = 0;

  if(i)
  {  /* Element belongs to local part of current processor */    /*E0019*/

     GetLocElm(DArr, IndexArray, BufferPtr)
     if(_BufferPtr)
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))

     for(i=0; i < DVM_VMSize; i++)
     { Proc = (int)DVM_VMS->VProc[i].lP;

       if(Proc == MPS_CurrentProc)
          continue;

       /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0020*/

       if( (j = IsProcInVMS(Proc, AMS->VMS)) >= 0 )
           wLocalBlock = GetSpaceLB4Proc(j, AMS, &DArr->Space,
                                         DArr->Align, NULL, &Block);
       else
           wLocalBlock = NULL;

       if(wLocalBlock)
          IsElmOfBlock(j, wLocalBlock, IndexArray)
       else
          j = 0;

       if(j == 0)
       {  if(_BufferPtr)
             ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], s_Elm_Mem) );
          else
             ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], s_Elm_Mem) );
          SendFlag[i] = 1;
       }
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     for(i=0; i < DVM_VMSize; i++)
     {  if(SendFlag[i])
           ( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
     }
  }
  else
  {  /* Element does not belong to local part of current processor */    /*E0021*/

     for(i=0; i < VMSize; i++)
     { Proc = (int)VMS->VProc[i].lP;

       if(Proc == MPS_CurrentProc)
          continue;

       wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space, DArr->Align,
                                     NULL, &Block);

       if(wLocalBlock)
          IsElmOfBlock(j, wLocalBlock, IndexArray)
       else
          j = 0;

       if(j)
       {  if(_BufferPtr)
             ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &RecvReq[i], 0) );
          else
             ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &RecvReq[i], 0) );
          RecvFlag[i] = 1;
       }
     }

     for(i=0; i < VMSize; i++)
     {  if(RecvFlag[i])
	   ( RTL_CALL, rtl_Waitrequest(&RecvReq[i]) );
     }

     if(_BufferPtr)
        SYSTEM(memcpy, (BufferPtr, _BufferPtr, DArr->TLen))
  }

  dvm_FreeArray(SendReq);
  dvm_FreeArray(RecvReq);
  dvm_FreeArray(SendFlag);
  dvm_FreeArray(RecvFlag);
  mac_free((void **)&_BufferPtr);

  return (int)DArr->TLen;
}



int   GetElmRepl(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[])
{ 
  GetLocElm(DArr, IndexArray, BufferPtr)
  return   (int)DArr->TLen;
}



int   PutElm(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[])
{ int  i;

  if(DArr->HasLocal)
     IsElmOfBlock(i, &DArr->Block, IndexArray)
  else
     i = 0;

  if(i)
     PutLocElm(BufferPtr, DArr, IndexArray)

  return   (int)DArr->TLen;
}



int   PutElmRepl(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[])
{ 
  PutLocElm(BufferPtr, DArr, IndexArray)
  return   (int)DArr->TLen;
}



int IOGetElm(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[])
{ s_AMVIEW    *AMS;
  s_VMS       *VMS;
  int          VMSize, i, j, Proc;
  s_BLOCK     *wLocalBlock, Block;
  RTL_Request  SendReq, *RecvReq;
  int         *RecvFlag;
  char        *_BufferPtr = NULL; 

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, RecvReq);
  dvm_AllocArray(int, VMSize, RecvFlag);
  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      RecvFlag[i] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0022*/

     if(DArr->HasLocal)
        IsElmOfBlock(i, &DArr->Block, IndexArray)
     else
        i = 0;

     if(i)
        GetLocElm(DArr, IndexArray, BufferPtr)
     else
     {  for(i=0; i < VMSize; i++)
        { Proc = (int)VMS->VProc[i].lP;

          if(Proc == MPS_CurrentProc)
             continue;

          wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                        DArr->Align, NULL, &Block);

          if(wLocalBlock)
             IsElmOfBlock(j, wLocalBlock, IndexArray)
          else
             j = 0;

          if(j)
          {  if(_BufferPtr)
                ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen,
                                           Proc, DVM_VMS->tag_DACopy,
                                           &RecvReq[i], 0) );
             else 
                ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen,
                                           Proc, DVM_VMS->tag_DACopy,
                                           &RecvReq[i], 0) );
             RecvFlag[i] = 1;
          }
        }

        for(i=0; i < VMSize; i++)
        { if(RecvFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&RecvReq[i]) );
        }

        if(_BufferPtr)
           SYSTEM(memcpy, (BufferPtr, _BufferPtr, DArr->TLen))
     }
  }
  else
  {  /* Current processor is not I/O processor */    /*E0023*/

     if(DArr->HasLocal)
        IsElmOfBlock(j, &DArr->Block, IndexArray)
     else
        j = 0;

     if(j)
     {
        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */    /*E0024*/

        if( (i = IsProcInVMS(DVM_IOProc, AMS->VMS)) >= 0 )
            wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space,
                                          DArr->Align, NULL, &Block);
        else
            wLocalBlock = NULL;

        if(wLocalBlock)
           IsElmOfBlock(j, wLocalBlock, IndexArray)
        else
           j = 0;

        if(j == 0)
        {  GetLocElm(DArr, IndexArray, BufferPtr)

           if(_BufferPtr)
           {  SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))
              ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen,
                                         DVM_IOProc, DVM_VMS->tag_DACopy,
                                         &SendReq, s_Elm_IOMem) );
           }
           else
              ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen,
                                         DVM_IOProc, DVM_VMS->tag_DACopy,
                                         &SendReq, s_Elm_IOMem) );
           ( RTL_CALL, rtl_Waitrequest(&SendReq) );
        }
     }
  }

  dvm_FreeArray(RecvReq);
  dvm_FreeArray(RecvFlag);
  mac_free((void **)&_BufferPtr);

  return  (int)DArr->TLen;
}



int   IOGetElmRepl(char  *BufferPtr, s_DISARRAY  *DArr, DvmType  IndexArray[])
{ 
  if(MPS_CurrentProc == DVM_IOProc)
     GetLocElm(DArr, IndexArray, BufferPtr)
  return  (int)DArr->TLen;
}



int   IOPutElm(char  *BufferPtr, s_DISARRAY  *DArr, DvmType  IndexArray[])
{ s_AMVIEW    *AMS;
  s_VMS       *VMS;
  int          VMSize, i, j, Proc;
  s_BLOCK     *wLocalBlock, Block;
  RTL_Request *SendReq, RecvReq;
  int         *SendFlag;
  char        *_BufferPtr = NULL;
 
  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      SendFlag[i] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  { /* Current processor is I/O processor */    /*E0025*/

    if(DArr->HasLocal)
       IsElmOfBlock(i, &DArr->Block, IndexArray)
    else
       i = 0;

    if(i)
       PutLocElm(BufferPtr, DArr, IndexArray)

    if(_BufferPtr)
       SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))

    for(i=0; i < VMSize; i++)
    {  Proc = (int)VMS->VProc[i].lP;

       if(Proc == MPS_CurrentProc)
          continue;

       wLocalBlock = GetSpaceLB4Proc(i, AMS, &DArr->Space, DArr->Align,
                                     NULL, &Block);

       if(wLocalBlock)
          IsElmOfBlock(j, wLocalBlock, IndexArray)
       else
          j = 0;

       if(j)
       {  if(_BufferPtr)
             ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], s_IOMem_Elm) );
          else
             ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], s_IOMem_Elm) );
          SendFlag[i] = 1;
       }
    }

    if(MsgSchedule && UserSumFlag)
    {  rtl_TstReqColl(0);
       rtl_SendReqColl(1.0);
    }

    for(i=0; i < VMSize; i++)
    {  if(SendFlag[i])
	  ( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
    }
  }
  else
  { /* Current processor is not I/O processor */    /*E0026*/

    if(DArr->HasLocal)
       IsElmOfBlock(i, &DArr->Block, IndexArray)
    else
       i = 0;

    if(i)
    {  if(_BufferPtr)
         ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen,
                                    DVM_IOProc, DVM_VMS->tag_DACopy,
                                    &RecvReq, 0) );
       else
         ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen,
                                    DVM_IOProc, DVM_VMS->tag_DACopy,
                                    &RecvReq, 0) );
       ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
       
       if(_BufferPtr)
          SYSTEM(memcpy, (BufferPtr, _BufferPtr, DArr->TLen))
       PutLocElm(BufferPtr, DArr, IndexArray)
    }
  }

  dvm_FreeArray(SendReq);
  dvm_FreeArray(SendFlag);
  mac_free((void **)&_BufferPtr);

  return  (int)DArr->TLen;
}



int IOPutElmRepl(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[])
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, Proc;
  RTL_Request   *SendReq, RecvReq;
  int           *SendFlag;
  char          *_BufferPtr = NULL;

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      SendFlag[i] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0027*/

     PutLocElm(BufferPtr, DArr, IndexArray)

     if(_BufferPtr)
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))

     for(i=0; i < VMSize; i++)
     {  Proc = (int)VMS->VProc[i].lP;
        if(Proc == MPS_CurrentProc)
           continue;
        if(_BufferPtr)
           ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], s_IOMem_ElmRepl) );
        else 
           ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], s_IOMem_ElmRepl) );
        SendFlag[i] = 1;
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }

     for(i=0; i < VMSize; i++)
     {  if(SendFlag[i])
	   ( RTL_CALL, rtl_Waitrequest(&SendReq[i]) );
     }
  }
  else
  {  /* Current processor is not I/O processor */    /*E0028*/

     if(_BufferPtr)
        ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, &RecvReq, 0) );
     else 
        ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, &RecvReq, 0) );
     ( RTL_CALL, rtl_Waitrequest(&RecvReq) );
     if(_BufferPtr)
        SYSTEM(memcpy, (BufferPtr, _BufferPtr, DArr->TLen))
     PutLocElm(BufferPtr, DArr, IndexArray)
  }

  dvm_FreeArray(SendReq);
  dvm_FreeArray(SendFlag);
  mac_free((void **)&_BufferPtr);

  return (int)DArr->TLen;
}



int   CopyElm(s_DISARRAY  *FromDArr, DvmType  FromIndexArray[],
              s_DISARRAY *ToDArr, DvmType  ToIndexArray[])
{ s_AMVIEW      *ReadAMS, *WriteAMS;
  s_VMS         *ReadVMS, *WriteVMS;
  int            ReadVMSize, WriteVMSize, i, j, Proc;
  s_BLOCK       *LocalBlock = NULL, Block;
  RTL_Request   *ReadReq, *WriteReq;
  int           *ReadFlag, *WriteFlag, IntersectFlag;
  void          **ReadBuf, **WriteBuf;

  ReadAMS = FromDArr->AMView;
  ReadVMS = ReadAMS->VMS;
  ReadVMSize = ReadVMS->ProcCount;

  WriteAMS = ToDArr->AMView;
  WriteVMS = WriteAMS->VMS;
  WriteVMSize = WriteVMS->ProcCount;

  dvm_AllocArray(RTL_Request, ReadVMSize, ReadReq);
  dvm_AllocArray(int, ReadVMSize, ReadFlag);
  dvm_AllocArray(void *, ReadVMSize, ReadBuf);

  for(i=0; i < ReadVMSize; i++)
  {  mac_malloc(ReadBuf[i], void *, FromDArr->TLen, 0);
     ReadFlag[i] = 0;
  }

  dvm_AllocArray(RTL_Request, WriteVMSize, WriteReq);
  dvm_AllocArray(int, WriteVMSize, WriteFlag);
  dvm_AllocArray(void *, WriteVMSize, WriteBuf);

  for(i=0; i < WriteVMSize; i++)
  {  mac_malloc(WriteBuf[i], void *, ToDArr->TLen, 0);
     WriteFlag[i] = 0;
  }

  if(ToDArr->HasLocal)
     IsElmOfBlock(i, &ToDArr->Block, ToIndexArray)
  else
     i = 0;

  if(i)
  {  /* Written element belongs to local part of current processor */    /*E0029*/

     if(FromDArr->HasLocal)
        IsElmOfBlock(i, &FromDArr->Block, FromIndexArray)
     else
        i = 0;

     if(i)
     {  /* Read element belongs to local part of current processor */    /*E0030*/

        CopyLocElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray)
     }
     else
     {  /*    Read element does not belong
           to local part of current processor */    /*E0031*/

        for(i=0; i < ReadVMSize; i++)
        {  Proc = (int)ReadVMS->VProc[i].lP;

           if(Proc == MPS_CurrentProc)
              continue;

           LocalBlock = GetSpaceLB4Proc(i, ReadAMS, &FromDArr->Space,
                                        FromDArr->Align, NULL, &Block);

           if(LocalBlock)
              IsElmOfBlock(j, LocalBlock, FromIndexArray)
           else
              j = 0;

           if(j)
           {  ReadFlag[i] = 1;
	      ( RTL_CALL, rtl_Recvnowait(ReadBuf[i], 1, FromDArr->TLen,
                                         Proc, DVM_VMS->tag_DACopy,
                                         &ReadReq[i], 0) );
           }
        }
     }
  }

  if(FromDArr->HasLocal)
     IsElmOfBlock(i, &FromDArr->Block, FromIndexArray)
  else
     i = 0;

  if(i)
  {  /* Read element belongs to local part of current processor */    /*E0032*/
     
     for(i=0; i < WriteVMSize; i++)
     {  Proc = (int)WriteVMS->VProc[i].lP;

        if(Proc == MPS_CurrentProc)
           continue;

        j = IsProcInVMS(Proc, ReadVMS);

        if(j >= 0)
           LocalBlock = GetSpaceLB4Proc(j, ReadAMS, &FromDArr->Space,
                                        FromDArr->Align, NULL, &Block);
        else
           LocalBlock = NULL;

        IsElmOfBlock(j, LocalBlock, FromIndexArray)

        IntersectFlag = LocalBlock && j;

        if(IntersectFlag)
           continue;

        LocalBlock = GetSpaceLB4Proc(i, WriteAMS, &ToDArr->Space,
                                     ToDArr->Align, NULL, &Block);
        if(LocalBlock)
           IsElmOfBlock(j, LocalBlock, ToIndexArray)
        else
           j = 0;

        if(j)
        { GetLocElm(FromDArr, FromIndexArray, WriteBuf[i])
          WriteFlag[i] = 1;
          ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1, FromDArr->TLen,
                                     Proc, DVM_VMS->tag_DACopy,
                                     &WriteReq[i], s_Elm_Elm) );
        }
     }

     if(MsgSchedule && UserSumFlag)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(1.0);
     }
  }

  for(i=0; i < WriteVMSize; i++)
  {  if(WriteFlag[i])
        ( RTL_CALL, rtl_Waitrequest(&WriteReq[i]) );
  }

  for(i=0; i < ReadVMSize; i++)
  {  if(ReadFlag[i] == 0)
        continue;
     ( RTL_CALL, rtl_Waitrequest(&ReadReq[i]) );
     PutLocElm(ReadBuf[i], ToDArr, ToIndexArray)
  }

  for(i=0; i < ReadVMSize; i++)
  {   mac_free((void **)&ReadBuf[i]);
  }

  dvm_FreeArray(ReadBuf);
  dvm_FreeArray(ReadFlag);
  dvm_FreeArray(ReadReq);

  for(i=0; i < WriteVMSize; i++)
  {   mac_free((void **)&WriteBuf[i]);
  }

  dvm_FreeArray(WriteBuf);
  dvm_FreeArray(WriteFlag);
  dvm_FreeArray(WriteReq);

  return  (int)FromDArr->TLen;
}



int   CopyElmRepl(s_DISARRAY *FromDArr, DvmType FromIndexArray[],
                  s_DISARRAY *ToDArr,   DvmType ToIndexArray[])
{ int  i;

  if(ToDArr->HasLocal)
     IsElmOfBlock(i, &ToDArr->Block, ToIndexArray)
  else
     i = 0;

  if(i)
  {  /* Written element belongs to local part of current processor */    /*E0033*/

     CopyLocElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray)
  }

  return  (int)ToDArr->TLen;
}


#endif   /* _ELMCOPY_C_ */    /*E0034*/
