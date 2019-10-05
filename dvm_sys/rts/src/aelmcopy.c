#ifndef _AELMCOPY_C_
#define _AELMCOPY_C_
/******************/    /*E0000*/

/*****************************************\
* Functions for asynchronous copying of   *
* an distributed array element            * 
\*****************************************/    /*E0001*/

DvmType  __callstd  arwelm_(DvmType FromArrayHeader[], DvmType ToArrayHeader[],
                            DvmType IndexArray[], AddrType *CopyFlagPtr)

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
CopyFlagPtr     - the pointer to the complete operation flag.

The function returns the number of bytes actually read or written
(that is the element size of the source or target array).
*/    /*E0002*/

{ SysHandle    *FromArrayHandlePtr, *ToArrayHandlePtr;
  DvmType          Res = 0;
  s_DISARRAY   *FromDArr, *ToDArr;
  s_AMVIEW     *AMV;
  int           i;

  DVMFTimeStart(call_arwelm_);

  /* Forward to the next element of message tag circle  tag_DACopy
     for the current processor system */    /*E0003*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0004*/

  *CopyFlagPtr = (AddrType)NULL;
  
  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.000: wrong call arwelm_\n"
              "(FromArray and ToArray are not distributed arrays;\n"
              "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
              (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

     if(RTL_TRACE)
     {  dvm_trace(call_arwelm_,
          "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
          "ToBufferPtr=%lx; CopyFlagPtr=%lx;\n",
          (uLLng)FromArrayHeader, FromArrayHeader[0],
          (uLLng)ToArrayHeader, (uLLng)CopyFlagPtr);

        if(TstTraceEvent(call_arwelm_))
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
              "*** RTS err 092.002: wrong call arwelm_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.004: wrong call arwelm_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
         (uLLng)DVM_VMS->HandlePtr);

     if(FromDArr->Repl && AMV->VMS == DVM_VMS)
        Res = GetElmRepl((char *)ToArrayHeader,FromDArr,IndexArray);
     else
        Res = AGetElm((char *)ToArrayHeader,FromDArr,IndexArray,
                      CopyFlagPtr);
  }
  else
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.001: wrong call arwelm_\n"
              "(FromArray and ToArray are distributed arrays;\n"
              "FromArrayHeader[0]=%lx; ToArrayHeader[0]=%lx)\n",
              FromArrayHeader[0], ToArrayHeader[0]);

     ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

     if(RTL_TRACE)
     {  dvm_trace(call_arwelm_,
          "FromBufferPtr=%lx; ToArrayHeader=%lx; "
          "ToArrayHandlePtr=%lx; CopyFlagPtr=%lx;\n",
          (uLLng)FromArrayHeader, (uLLng)ToArrayHeader, ToArrayHeader[0],
          (uLLng)CopyFlagPtr);

        if(TstTraceEvent(call_arwelm_))
        {  for(i=0; i < ToDArr->Space.Rank; i++)
               tprintf("IndexArray[%d]=%ld; ",i,IndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     AMV = ToDArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.003: wrong call arwelm_\n"
              "(ToArray has not been aligned; "
              "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.005: wrong call arwelm_\n"
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
     dvm_trace(ret_arwelm_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_arwelm_);
  return  (DVM_RET, Res);
}



/* For Fortran */    /*E0005*/


DvmType __callstd arwelf_(DvmType FromArrayHeader[],
                          AddrType *ToArrayHeaderPtr, DvmType IndexArray[],
                          AddrType *CopyFlagPtr)

/*
    To avoid warnings of FORTRAN compiler while calling  arwelm_ with
different types of parameters for a distributed array  element reading,
there is function arwelf_ in support system.

arwelf_ differs from function arwelm_  by the second parameter:
*ToArrayHeaderPtr - pointer to memory where a distributed array element
                    is to be read.
Other parameters of arwelf_  and  arwelm_  are the same.
*/    /*E0006*/

{
    return  arwelm_(FromArrayHeader, (DvmType *)*ToArrayHeaderPtr,
                  IndexArray, CopyFlagPtr);
}

/*   --------------------   */    /*E0007*/



DvmType  __callstd  acopel_(DvmType FromArrayHeader[], DvmType FromIndexArray[],
                            DvmType ToArrayHeader[],  DvmType ToIndexArray[],
                            AddrType *CopyFlagPtr)

/*
     Copying one element of distributed array to another.
     ----------------------------------------------------

FromArrayHeader	- the header of the source distributed array.
FromIndexArray	- FromIndexArray[i] is the index of the source element
                  on the (i+1)th dimension.
ToArrayHeader	- the header of the target distributed array.
ToIndexArray	- ToIndexArray[i] is the index of the target element
                  on the (i+1)th dimension.
CopyFlagPtr     - the pointer to the complete operation flag.

The types of the source and target elements have to be the same.
The function returns the number of the copied bytes.

*/    /*E0008*/

{ SysHandle    *FromArrayHandlePtr,*ToArrayHandlePtr;
  DvmType          Res=0;
  s_DISARRAY   *FromDArr,*ToDArr;
  int           i;
  s_AMVIEW     *AMV;

  DVMFTimeStart(call_acopel_);

  /* Forward to the next element of message tag circle  tag_DACopy
     for the current processor system */    /*E0009*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0010*/

  *CopyFlagPtr = (AddrType)NULL;

  if(RTL_TRACE)
     dvm_trace(call_acopel_,
        "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
        "ToArrayHeader=%lx; ToArrayHandlePtr=%lx; "
        "CopyFlagPtr=%lx;\n",
        (uLLng)FromArrayHeader, FromArrayHeader[0], (uLLng)ToArrayHeader,
        ToArrayHeader[0], (uLLng)CopyFlagPtr);

  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

  if(FromArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 092.010: wrong call acopel_\n"
        "(FromArray is not a distributed array;\n"
        "FromArrayHeader=%lx)\n", (uLLng)FromArrayHeader);

  FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

  AMV = FromDArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.012: wrong call acopel_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.014: wrong call acopel_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.011: wrong call acopel_\n"
          "(ToArray is not a distributed array;\n"
          "ToArrayHeader=%lx)\n", (uLLng)ToArrayHeader);

  ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

  AMV = ToDArr->AMView;

  if(AMV == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.013: wrong call acopel_\n"
              "(ToArray has not been aligned; "
              "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

  NotSubsystem(i, DVM_VMS, AMV->VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.015: wrong call acopel_\n"
          "(the ToArray PS is not a subsystem of the current PS;\n"
          "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_acopel_))
     {  for(i=0; i < FromDArr->Space.Rank; i++)
            tprintf("FromIndexArray[%d]=%ld; ", i, FromIndexArray[i]);
        tprintf(" \n");

        for(i=0; i < ToDArr->Space.Rank; i++)
            tprintf("  ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(FromDArr->Repl && FromDArr->AMView->VMS == DVM_VMS)
     Res = CopyElmRepl(FromDArr, FromIndexArray, ToDArr, ToIndexArray);
  else
     Res = ACopyElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray,
                    CopyFlagPtr);

  if(RTL_TRACE)
     dvm_trace(ret_acopel_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_acopel_);
  return  (DVM_RET, Res);
}



DvmType __callstd aelmcp_(DvmType FromArrayHeader[], DvmType FromIndexArray[],
                          DvmType ToArrayHeader[], DvmType ToIndexArray[],
                          DvmType *CopyRegimPtr, AddrType *CopyFlagPtr)

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
CopyFlagPtr     - the pointer to the complete operation flag.

The function returns the number of the copied bytes.
*/    /*E0011*/

{ SysHandle    *FromArrayHandlePtr,*ToArrayHandlePtr;
  DvmType          Res=0;
  s_DISARRAY   *FromDArr,*ToDArr;
  int           i;
  s_AMVIEW     *AMV;

  DVMFTimeStart(call_aelmcp_);
  
  /* Forward to the next element of message tag circle  tag_DACopy
     for the current processor system */    /*E0012*/

  DVM_VMS->tag_DACopy++;

  if((DVM_VMS->tag_DACopy - (msg_DACopy)) >= TagCount)
     DVM_VMS->tag_DACopy = msg_DACopy;

  /* ----------------------------------------------- */    /*E0013*/

  *CopyFlagPtr = (AddrType)NULL;

  ToArrayHandlePtr = TstDVMArray((void *)ToArrayHeader);

  if(ToArrayHandlePtr == NULL)
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 092.020: wrong call aelmcp_\n"
             "(FromArray and ToArray are not distributed arrays;\n"
             "FromArrayHeader=%lx; ToArrayHeader=%lx)\n",
             (uLLng)FromArrayHeader, (uLLng)ToArrayHeader);

     FromDArr=(s_DISARRAY *)FromArrayHandlePtr->pP;

     AMV = FromDArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 092.021: wrong call aelmcp_\n"
              "(FromArray has not been aligned; "
              "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

     NotSubsystem(i, DVM_VMS, AMV->VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 092.023: wrong call aelmcp_\n"
          "(the FromArray PS is not a subsystem of the current PS;\n"
          "FromArrayHeader[0]=%lx; ArrayPSRef=%lx; CurrentPSRef=%lx)\n",
          FromArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

     if(RTL_TRACE)
     {  dvm_trace(call_aelmcp_,
                "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                "ToBufferPtr=%lx; "
                "CopyRegim=%ld; CopyFlagPtr=%lx;\n",
                (uLLng)FromArrayHeader, FromArrayHeader[0],
                (uLLng)ToArrayHeader, *CopyRegimPtr, (uLLng)CopyFlagPtr);

        if(TstTraceEvent(call_aelmcp_))
        {  for(i=0; i < FromDArr->Space.Rank; i++)
               tprintf("FromIndexArray[%d]=%ld; ",i,FromIndexArray[i]);
           tprintf(" \n");
           tprintf(" \n");
        }
     }

     if(*CopyRegimPtr)
     {  if(FromDArr->Repl && AMV->VMS == DVM_VMS)
           Res = IOGetElmRepl((char *)ToArrayHeader,FromDArr,
                              FromIndexArray);
        else
           Res = AIOGetElm((char *)ToArrayHeader, FromDArr,
                           FromIndexArray, CopyFlagPtr);
     }
     else
     {  if(FromDArr->Repl && AMV->VMS == DVM_VMS)
           Res = GetElmRepl((char *)ToArrayHeader,FromDArr,
                            FromIndexArray);
        else
           Res = AGetElm((char *)ToArrayHeader,FromDArr,FromIndexArray,
                         CopyFlagPtr);
     }
  }
  else
  {  FromArrayHandlePtr = TstDVMArray((void *)FromArrayHeader);

     if(FromArrayHandlePtr == NULL)
     {  ToDArr = (s_DISARRAY *)ToArrayHandlePtr->pP;

        AMV = ToDArr->AMView;

        if(AMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 092.022: wrong call aelmcp_\n"
                    "(ToArray has not been aligned; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 092.024: wrong call aelmcp_\n"
                    "(the ToArray PS is not a subsystem "
                    "of the current PS;\n"
                    "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
                    (uLLng)DVM_VMS->HandlePtr);

        if(RTL_TRACE)
        {  dvm_trace(call_aelmcp_,
                   "FromBufferPtr=%lx; ToArrayHeader=%lx; "
                   "ToArrayHandlePtr=%lx; "
                   "CopyRegim=%ld; CopyFlagPtr=%lx;\n",
                   (uLLng)FromArrayHeader, (uLLng)ToArrayHeader,
                   ToArrayHeader[0], *CopyRegimPtr, (uLLng)CopyFlagPtr);

           if(TstTraceEvent(call_aelmcp_))
           {  for(i=0; i < ToDArr->Space.Rank; i++)
                  tprintf("ToIndexArray[%d]=%ld; ",i,ToIndexArray[i]);
              tprintf(" \n");
              tprintf(" \n");
           }
        }
 
        if(*CopyRegimPtr)
        {  if(ToDArr->Repl && AMV->VMS == DVM_VMS)
              Res = AIOPutElmRepl((char *)FromArrayHeader, ToDArr,
                                  ToIndexArray, CopyFlagPtr);
           else
              Res = AIOPutElm((char *)FromArrayHeader, ToDArr,
                              ToIndexArray, CopyFlagPtr);
        }
        else
        {  if(ToDArr->Repl && AMV->VMS == DVM_VMS)
              Res = PutElmRepl((char *)FromArrayHeader,ToDArr,
                               ToIndexArray);
           else
              Res = PutElm((char *)FromArrayHeader,ToDArr,ToIndexArray);
        }
     }
     else
     {  FromDArr = (s_DISARRAY *)FromArrayHandlePtr->pP;

        AMV = FromDArr->AMView;

        if(AMV == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 092.021: wrong call aelmcp_\n"
                    "(FromArray has not been aligned; "
                    "FromArrayHeader[0]=%lx)\n", FromArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 092.023: wrong call aelmcp_\n"
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
                    "*** RTS err 092.022: wrong call aelmcp_\n"
                    "(ToArray has not been aligned; "
                    "ToArrayHeader[0]=%lx)\n", ToArrayHeader[0]);

        NotSubsystem(i, DVM_VMS, AMV->VMS)

        if(i)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 092.024: wrong call aelmcp_\n"
                    "(the ToArray PS is not a subsystem "
                    "of the current PS;\n"
                    "ToArrayHeader[0]=%lx; ArrayPSRef=%lx; "
                    "CurrentPSRef=%lx)\n",
                    ToArrayHeader[0], (uLLng)AMV->VMS->HandlePtr,
                    (uLLng)DVM_VMS->HandlePtr);

        if(RTL_TRACE)
        {  dvm_trace(call_aelmcp_,
                     "FromArrayHeader=%lx; FromArrayHandlePtr=%lx; "
                     "ToArrayHeader=%lx; ToArrayHandlePtr=%lx; "
                     "CopyFlagPtr=%lx;\n",
                     (uLLng)FromArrayHeader, FromArrayHeader[0],
                     (uLLng)ToArrayHeader, ToArrayHeader[0],
                     (uLLng)CopyFlagPtr);

           if(TstTraceEvent(call_aelmcp_))
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
           Res=ACopyElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray,
                        CopyFlagPtr);
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_aelmcp_,"Res=%ld;\n", Res);

  DVMFTimeFinish(ret_aelmcp_);
  return  (DVM_RET, Res);
}



DvmType   __callstd  waitcp_(AddrType  *CopyFlagPtr)
{ int                 i, j, k, n, m, p, q;
  s_COPYCONT         *CopyCont;
  void               *ContInfo = NULL;
  s_ARWElm           *RWElm;
  s_ACopyElm         *CopyElm;
  s_AGetBlock        *GetBlock;
  s_AIOPutBlock      *PutBlock;
  s_AIOPutBlockRepl  *PutBlockRepl;
  s_ACopyBlock       *CopyBlock;

  /* Local variables to continue ACopyBlock */    /*E0014*/

  s_BLOCK    wWriteBlock;
  DvmType       ReadIndex[MAXARRAYDIM + 1], WriteIndex[MAXARRAYDIM + 1];
  char      *ReadElmPtr, *CharPtr1, *CharPtr2;
  s_VMS     *WriteVMS;
  s_BLOCK   *CurrWriteBlock;
  DvmType       LinInd, MyMinLinInd, MinLinInd;

  /* ----------------------------------------------- */    /*E0015*/

  DVMFTimeStart(call_waitcp_);

  if(RTL_TRACE)
     dvm_trace(call_waitcp_,"CopyFlagPtr=%lx;\n", (uLLng)CopyFlagPtr);

  CopyCont = (s_COPYCONT *)*CopyFlagPtr; /* pointer to the head 
                                            of information structure
                                            to continue copying */    /*E0016*/
  if(CopyCont != NULL)
     ContInfo = CopyCont->ContInfo; /* pointer to information structure
                                       for continuation */    /*E0017*/
  if(ContInfo != NULL)
  {  /* There are information to continue copying */    /*E0018*/

     switch(CopyCont->Oper)
     {
       case send_AGetElm:
       /****************/    /*E0019*/

       RWElm = (s_ARWElm *)ContInfo;

       for(i=0; i < DVM_VMS->ProcCount; i++)
       {  if(RWElm->SendFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(RWElm->SendReq[i])) );
       }

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case recv_AGetElm:
       /****************/    /*E0020*/

       RWElm = (s_ARWElm *)ContInfo;

       for(i=0; i < RWElm->VMSize; i++)
       {  if(RWElm->RecvFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(RWElm->RecvReq[i])) );
       }

       if(RWElm->_BufferPtr)
          SYSTEM(memcpy, (RWElm->BufferPtr, RWElm->_BufferPtr,
                          RWElm->DArr->TLen))

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case send_AIOGetElm:
       /******************/    /*E0021*/

       RWElm = (s_ARWElm *)ContInfo;

       if(RWElm->SendFlag[0])
          ( RTL_CALL, rtl_Waitrequest(RWElm->SendReq) );

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case recv_AIOGetElm:
       /******************/    /*E0022*/

       RWElm = (s_ARWElm *)ContInfo;

       if(RWElm->SendFlag[0])
       {  for(i=0; i < RWElm->VMSize; i++)
          { if(RWElm->RecvFlag[i])
               ( RTL_CALL, rtl_Waitrequest(&(RWElm->RecvReq[i])) );
          }

          if(RWElm->_BufferPtr)
             SYSTEM(memcpy, (RWElm->BufferPtr, RWElm->_BufferPtr,
                             RWElm->DArr->TLen))
       }

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case send_AIOPutElm:
       /******************/    /*E0023*/

       RWElm = (s_ARWElm *)ContInfo;

       for(i=0; i < RWElm->VMSize; i++)
       {  if(RWElm->SendFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(RWElm->SendReq[i])) );
       }

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case recv_AIOPutElm:
       /******************/    /*E0024*/

       RWElm = (s_ARWElm *)ContInfo;

       if(RWElm->RecvFlag[0])
       {  ( RTL_CALL, rtl_Waitrequest(RWElm->RecvReq) );
       
          if(RWElm->_BufferPtr)
             SYSTEM(memcpy, (RWElm->BufferPtr, RWElm->_BufferPtr,
                             RWElm->DArr->TLen))

          PutLocElm(RWElm->BufferPtr, RWElm->DArr, RWElm->IndexArray)
       }

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;
 

       case send_AIOPutElmRepl:
       /**********************/    /*E0025*/

       RWElm = (s_ARWElm *)ContInfo;

       for(i=0; i < RWElm->VMSize; i++)
       {  if(RWElm->SendFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(RWElm->SendReq[i])) );
       }

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case recv_AIOPutElmRepl:
       /**********************/    /*E0026*/

       RWElm = (s_ARWElm *)ContInfo;

       ( RTL_CALL, rtl_Waitrequest(RWElm->RecvReq) );

       if(RWElm->_BufferPtr)
          SYSTEM(memcpy, (RWElm->BufferPtr, RWElm->_BufferPtr,
                          RWElm->DArr->TLen))

       PutLocElm(RWElm->BufferPtr, RWElm->DArr, RWElm->IndexArray)

       dvm_FreeArray(RWElm->SendReq);
       dvm_FreeArray(RWElm->RecvReq);
       dvm_FreeArray(RWElm->SendFlag);
       dvm_FreeArray(RWElm->RecvFlag);
       mac_free((void **)&(RWElm->_BufferPtr));

       break;

       case sendrecv_ACopyElm:
       /*********************/    /*E0027*/

       CopyElm = (s_ACopyElm *)ContInfo;

       for(i=0; i < CopyElm->WriteVMSize; i++)
       {  if(CopyElm->WriteFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(CopyElm->WriteReq[i])) );
       }

       for(i=0; i < CopyElm->ReadVMSize; i++)
       {  if(CopyElm->ReadFlag[i] == 0)
             continue;
          ( RTL_CALL, rtl_Waitrequest(&(CopyElm->ReadReq[i])) );
          PutLocElm(CopyElm->ReadBuf[i], CopyElm->ToDArr,
                    CopyElm->ToIndexArray)
       }

       for(i=0; i < CopyElm->ReadVMSize; i++)
       {   mac_free((void **)&(CopyElm->ReadBuf[i]));
       }

       dvm_FreeArray(CopyElm->ReadBuf);
       dvm_FreeArray(CopyElm->ReadFlag);
       dvm_FreeArray(CopyElm->ReadReq);

       for(i=0; i < CopyElm->WriteVMSize; i++)
       {   mac_free((void **)&(CopyElm->WriteBuf[i]));
       }

       dvm_FreeArray(CopyElm->WriteBuf);
       dvm_FreeArray(CopyElm->WriteFlag);
       dvm_FreeArray(CopyElm->WriteReq);

       break;

       case sendrecv_AGetBlock:
       /**********************/    /*E0028*/

       GetBlock = (s_AGetBlock *)ContInfo;

       /* Wait for send completion */    /*E0029*/

       for(i=0; i < DVM_VMS->ProcCount; i++)
       {  if(GetBlock->SendFlag[i])
             ( RTL_CALL, rtl_Waitrequest(&(GetBlock->SendReq[i])) );
       }

       /* Wait for receive completion and rewrite received parts */    /*E0030*/

       for(i=0; i < GetBlock->VMSize; i++)
       {  if(GetBlock->RecvFlag[i] == 0)
             continue;
          ( RTL_CALL, rtl_Waitrequest(&(GetBlock->RecvReq[i])) );
          CopySubmemToMem(GetBlock->BufferPtr, &GetBlock->ReadBlock,
                          GetBlock->RecvBuf[i],
                          &(GetBlock->RecvBlock[i]),
                          GetBlock->DArr->TLen, GetBlock->Step);
          mac_free((void **)&(GetBlock->RecvBuf[i]));
       }

       /* Free memory */    /*E0031*/

       mac_free(&GetBlock->SendBuf);

       dvm_FreeArray(GetBlock->RecvReq);
       dvm_FreeArray(GetBlock->RecvFlag);
       dvm_FreeArray(GetBlock->RecvBuf);
       dvm_FreeArray(GetBlock->RecvBlock);
       dvm_FreeArray(GetBlock->SendReq);
       dvm_FreeArray(GetBlock->SendFlag);

       break;
 
       case send_AIOGetBlock:
       /********************/    /*E0032*/

       GetBlock = (s_AGetBlock *)ContInfo;

       if(GetBlock->SendFlag[0])
          ( RTL_CALL, rtl_Waitrequest(GetBlock->SendReq) );

       /* Free memory */    /*E0033*/

       mac_free(&GetBlock->SendBuf);

       dvm_FreeArray(GetBlock->RecvReq);
       dvm_FreeArray(GetBlock->RecvFlag);
       dvm_FreeArray(GetBlock->RecvBuf);
       dvm_FreeArray(GetBlock->RecvBlock);
       dvm_FreeArray(GetBlock->SendReq);
       dvm_FreeArray(GetBlock->SendFlag);

       break;
 
       case recv_AIOGetBlock:
       /********************/    /*E0034*/

       GetBlock = (s_AGetBlock *)ContInfo;

       for(i=0; i < GetBlock->VMSize; i++)
       {  if(GetBlock->RecvFlag[i] == 0)
             continue;
          ( RTL_CALL, rtl_Waitrequest(&(GetBlock->RecvReq[i])) );
          CopySubmemToMem(GetBlock->BufferPtr, &GetBlock->ReadBlock,
                          GetBlock->RecvBuf[i],
                          &(GetBlock->RecvBlock[i]),
                          GetBlock->DArr->TLen, GetBlock->Step);
          mac_free((void **)&(GetBlock->RecvBuf[i]));
       }

       /* Free memory */    /*E0035*/

       mac_free(&GetBlock->SendBuf);

       dvm_FreeArray(GetBlock->RecvReq);
       dvm_FreeArray(GetBlock->RecvFlag);
       dvm_FreeArray(GetBlock->RecvBuf);
       dvm_FreeArray(GetBlock->RecvBlock);
       dvm_FreeArray(GetBlock->SendReq);
       dvm_FreeArray(GetBlock->SendFlag);

       break;
 
       case send_AIOPutBlock:
       /********************/    /*E0036*/

       PutBlock = (s_AIOPutBlock *)ContInfo;

       for(i=0; i < PutBlock->VMSize; i++)
       {  if(PutBlock->SendFlag[i] == 0)
             continue;
          ( RTL_CALL, rtl_Waitrequest(&(PutBlock->SendReq[i])) );
          mac_free((void **)&(PutBlock->SendBuf[i]));
       }

       mac_free(&PutBlock->RecvBuf);
       dvm_FreeArray(PutBlock->SendReq);
       dvm_FreeArray(PutBlock->RecvReq);
       dvm_FreeArray(PutBlock->SendFlag);
       dvm_FreeArray(PutBlock->RecvFlag);
       dvm_FreeArray(PutBlock->SendBuf);

       break;
 
       case recv_AIOPutBlock:
       /********************/    /*E0037*/

       PutBlock = (s_AIOPutBlock *)ContInfo;

       if(PutBlock->RecvFlag[0])
       {  ( RTL_CALL, rtl_Waitrequest(PutBlock->RecvReq) );
          CopyMemToBlock(PutBlock->DArr, (char *)PutBlock->RecvBuf,
                         &PutBlock->WriteLocalBlock, PutBlock->Step);
       }

       mac_free(&PutBlock->RecvBuf);
       dvm_FreeArray(PutBlock->SendReq);
       dvm_FreeArray(PutBlock->RecvReq);
       dvm_FreeArray(PutBlock->SendFlag);
       dvm_FreeArray(PutBlock->RecvFlag);
       dvm_FreeArray(PutBlock->SendBuf);

       break;
 
       case send_AIOPutBlockRepl:
       /************************/    /*E0038*/

       PutBlockRepl = (s_AIOPutBlockRepl *)ContInfo;

       for(i=0; i < PutBlockRepl->VMSize; i++)
       {  if(PutBlockRepl->VMS->VProc[i].lP == MPS_CurrentProc)
             continue;
          ( RTL_CALL, rtl_Waitrequest(&(PutBlockRepl->SendReq[i])) );
       }

       mac_free(&PutBlockRepl->_BufferPtr);
       dvm_FreeArray(PutBlockRepl->SendReq);
       dvm_FreeArray(PutBlockRepl->RecvReq);

       break;
 
       case recv_AIOPutBlockRepl:
       /************************/    /*E0039*/

       PutBlockRepl = (s_AIOPutBlockRepl *)ContInfo;

       ( RTL_CALL, rtl_Waitrequest(PutBlockRepl->RecvReq) );

       if(PutBlockRepl->RecvSign)
       { SYSTEM(memcpy, (PutBlockRepl->BufferPtr,
                         PutBlockRepl->_BufferPtr, PutBlockRepl->BSize))
         mac_free(&PutBlockRepl->_BufferPtr);
       }

       CopyMemToBlock(PutBlockRepl->DArr, PutBlockRepl->BufferPtr,
                      &PutBlockRepl->WriteBlock, PutBlockRepl->Step);

       dvm_FreeArray(PutBlockRepl->SendReq);
       dvm_FreeArray(PutBlockRepl->RecvReq);

       break;
 
       case sendrecv_ACopyBlock:
       /***********************/    /*E0040*/

       CopyBlock = (s_ACopyBlock *)ContInfo;

       for(i=0; i < CopyBlock->WriteVMSize; i++)
       {  if(CopyBlock->WriteBSize[i] == 0)
             continue;
          if(CopyBlock->ExchangeScheme == 0 && CopyBlock->Alltoall == 0)
             ( RTL_CALL, rtl_Waitrequest(&(CopyBlock->WriteReq[i])) );
          mac_free(&(CopyBlock->WriteBuf[i]));
       }

       /* */    /*E0041*/

       if(CopyBlock->FromOnlyAxis >= 0 && CopyBlock->ToOnlyAxis >= 0)
       {  /* */    /*E0042*/
          /* */    /*E0043*/

          if(RTL_TRACE && dacopy_Trace && TstTraceEvent(call_aarrcp_))
             tprintf("*** ACopyBlock: Axis -> Axis; Wait Recv.\n");

          WriteVMS = CopyBlock->WriteVMS;

          if(WriteVMS->HasCurrent &&
             CopyBlock->IsWriteInter[WriteVMS->CurrentProc])
          {  /* */    /*E0044*/

             CurrWriteBlock =
             &CopyBlock->WriteLocalBlock[WriteVMS->CurrentProc];

             n = CopyBlock->ToDArr->TLen;
             m = CopyBlock->ToBlock.Rank;

             for(j=0; j < m; j++)
                 WriteIndex[j] = CurrWriteBlock->Set[j].Lower;
             index_GetLI(MyMinLinInd, &CopyBlock->ToBlock, WriteIndex,
                         CopyBlock->ToStep)

             /* */    /*E0045*/

             for(i=0; i < CopyBlock->ReadVMSize; i++)
             {  if(CopyBlock->ReadBSize[i] == 0)
                   continue;
                if(CopyBlock->ExchangeScheme == 0 &&
                   CopyBlock->Alltoall == 0)
                   (RTL_CALL, rtl_Waitrequest(&CopyBlock->ReadReq[i]));

                ReadElmPtr = (char *)CopyBlock->ReadBuf[i];/* */    /*E0046*/
                for(j=0; j < CopyBlock->FromBlock.Rank; j++)
                    ReadIndex[j] =
                    CopyBlock->ReadLocalBlock[i].Set[j].Lower;
                index_GetLI(LinInd, &CopyBlock->FromBlock, ReadIndex,
                            CopyBlock->FromStep)

                MinLinInd = dvm_max(LinInd, MyMinLinInd);
           
                k = CopyBlock->ReadBSize[i]/n;  /* */    /*E0047*/

                WriteIndex[CopyBlock->ToOnlyAxis] =
                CopyBlock->ToBlock.Set[CopyBlock->ToOnlyAxis].Lower +
                (MinLinInd *
                 CopyBlock->ToBlock.Set[CopyBlock->ToOnlyAxis].Step);
        
                LocElmAddr(CharPtr1, CopyBlock->ToDArr,
                           WriteIndex)      /* */    /*E0048*/
                WriteIndex[CopyBlock->ToOnlyAxis] +=
                CopyBlock->ToBlock.Set[CopyBlock->ToOnlyAxis].Step;

                LocElmAddr(CharPtr2, CopyBlock->ToDArr,
                           WriteIndex)      /* */    /*E0049*/
                           j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1); /* */    /*E0050*/
                for(p=0; p < k; p++)
                {  CharPtr2 = CharPtr1;

                   for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                       *CharPtr2 = *ReadElmPtr;

                   CharPtr1 += j;
                }

                mac_free(&(CopyBlock->ReadBuf[i]));
             } 
          }
       }
       else
       { for(i=0; i < CopyBlock->ReadVMSize; i++)
         {  if(CopyBlock->ReadBSize[i] == 0)
               continue;
            if(CopyBlock->ExchangeScheme == 0 &&
               CopyBlock->Alltoall == 0)
               (RTL_CALL, rtl_Waitrequest(&(CopyBlock->ReadReq[i])));

            ReadElmPtr = (char *)CopyBlock->ReadBuf[i];

            if(CopyBlock->EquSign)
            {  /* dimensions of read block and written block and  
                and size of each dimension are equal */    /*E0051*/

               CurrWriteBlock = &CopyBlock->ToLocalBlock[i];

               wWriteBlock = block_Copy(CurrWriteBlock);
               n = CopyBlock->ToBlock.Rank;

               if(CurrWriteBlock->Set[n-1].Step == 1)
               {  /* Step on main dimension is equal to 1 */    /*E0052*/

                  LinInd = CurrWriteBlock->Set[n-1].Size;
                  n = (int)(LinInd * CopyBlock->ToDArr->TLen);
                  k = (int)(CopyBlock->ReadBSize[i] / n);

                  for(j=0; j < k; j++)
                  {  index_FromBlock1S(WriteIndex, &wWriteBlock,
                                       CurrWriteBlock)
                     PutLocElm1(ReadElmPtr, CopyBlock->ToDArr,
                                WriteIndex, LinInd)
                     ReadElmPtr += n;
                  }
               }
               else
               {  /* Step on main dimension is not equal to 1 */    /*E0053*/

                  k = (int)(CopyBlock->ReadBSize[i] /
                            CopyBlock->ToDArr->TLen);

                  for(j=0; j < k; j++)
                  {  index_FromBlock(WriteIndex, &wWriteBlock,
                                     CurrWriteBlock, CopyBlock->EquSign)
                     PutLocElm(ReadElmPtr, CopyBlock->ToDArr, WriteIndex)
                     ReadElmPtr += CopyBlock->ToDArr->TLen;
                  }
               } 
            }
            else
            {  /* dimensions of read block and written block or  
                size of any dimension are not equal */    /*E0054*/

               CurrWriteBlock = CopyBlock->CurrWriteBlock;

               wWriteBlock = block_Copy(CurrWriteBlock);

               block_GetSize(LinInd, CurrWriteBlock, CopyBlock->ToStep)
               n = (int)LinInd; /* size of local part 
                              of written block */    /*E0055*/

               for(k=0; k < n; k++)
               { index_FromBlock(WriteIndex, &wWriteBlock,
                                 CurrWriteBlock, CopyBlock->ToStep)
                 index_GetLI(LinInd, &CopyBlock->ToBlock,
                             WriteIndex, CopyBlock->ToStep)
                 index_GetSI(&CopyBlock->FromBlock, CopyBlock->ReadWeight,
                             LinInd, ReadIndex, CopyBlock->FromStep)

                 if(CopyBlock->ReadVMS->HasCurrent &&
                    CopyBlock->IsReadInter[CopyBlock->ReadVMS->CurrentProc])
                 {  IsElmOfBlock(j, &(CopyBlock->ReadLocalBlock
                                      [CopyBlock->ReadVMS->CurrentProc]),
                                    ReadIndex)
                    if(j)
                       continue;
                 }

                 IsElmOfBlock(j, &(CopyBlock->ReadLocalBlock[i]),
                              ReadIndex)
                 if(j == 0)
                    continue;

                 PutLocElm(ReadElmPtr, CopyBlock->ToDArr, WriteIndex)
                 ReadElmPtr += CopyBlock->ToDArr->TLen;
               }
            }

            mac_free(&(CopyBlock->ReadBuf[i]));
         }
       }

       dvm_FreeArray(CopyBlock->ReadReq); 
       dvm_FreeArray(CopyBlock->ReadBuf); 
       dvm_FreeArray(CopyBlock->IsReadInter); 
       dvm_FreeArray(CopyBlock->ReadLocalBlock); 
       dvm_FreeArray(CopyBlock->ReadBSize); 

       dvm_FreeArray(CopyBlock->WriteReq); 
       dvm_FreeArray(CopyBlock->WriteBuf); 
       dvm_FreeArray(CopyBlock->IsWriteInter); 
       dvm_FreeArray(CopyBlock->WriteLocalBlock); 
       dvm_FreeArray(CopyBlock->WriteBSize);

       dvm_FreeArray(CopyBlock->ToLocalBlock); 

       break;

       case sendrecv_ACopyBlock1:
       /************************/    /*E0056*/

       CopyBlock = (s_ACopyBlock *)ContInfo;

       for(i=0; i < CopyBlock->WriteVMSize; i++)
       {  if(CopyBlock->ToVM[i] == 0)
             continue;

          if(CopyBlock->WriteBSize[i] == 0)
             continue;

          if(CopyBlock->ExchangeScheme == 0 && CopyBlock->Alltoall == 0)
             ( RTL_CALL, rtl_Waitrequest(&(CopyBlock->WriteReq[i])) );

          if(CopyBlock->FromSuperFast)
             continue;  /* */    /*E0057*/

          mac_free(&(CopyBlock->WriteBuf[i]));
       }

       /* */    /*E0058*/

       if(CopyBlock->FromOnlyAxis >= 0 && CopyBlock->ToOnlyAxis >= 0)
       {  /* */    /*E0059*/
          /* */    /*E0060*/

          WriteVMS = CopyBlock->WriteVMS;

          if(WriteVMS->HasCurrent &&
             CopyBlock->IsWriteInter[WriteVMS->CurrentProc])
          {  /* */    /*E0061*/

             CurrWriteBlock =
             &CopyBlock->WriteLocalBlock[WriteVMS->CurrentProc];

             m = CopyBlock->ToBlock.Rank;

             for(j=0; j < m; j++)
                 WriteIndex[j] = CurrWriteBlock->Set[j].Lower;

             MyMinLinInd = WriteIndex[CopyBlock->ToOnlyAxis] -
             CopyBlock->ToBlock.Set[CopyBlock->ToOnlyAxis].Lower;

             n = CopyBlock->ToDArr->TLen;

             /* */    /*E0062*/

             for(i=0; i < CopyBlock->ReadVMSize; i++)
             {  if(CopyBlock->FromVM[i] == 0)
                   continue;

                if(CopyBlock->ReadBSize[i] == 0)
                   continue;

                if(CopyBlock->ExchangeScheme == 0 &&
                   CopyBlock->Alltoall == 0)
                   (RTL_CALL, rtl_Waitrequest(&CopyBlock->ReadReq[i]));

                if(CopyBlock->ToSuperFast)
                   continue;/* */    /*E0063*/

                ReadElmPtr =
                (char *)CopyBlock->ReadBuf[i];  /* */    /*E0064*/
                LinInd =
                CopyBlock->ReadLocalBlock[i].Set
                [CopyBlock->FromOnlyAxis].Lower -
                CopyBlock->FromBlock.Set[CopyBlock->FromOnlyAxis].Lower;

                MinLinInd = dvm_max(LinInd, MyMinLinInd);
           
                WriteIndex[CopyBlock->ToOnlyAxis] =
                CopyBlock->ToBlock.Set[CopyBlock->ToOnlyAxis].Lower +
                MinLinInd;

                LocElmAddr(CharPtr1, CopyBlock->ToDArr,
                           WriteIndex)     /* */    /*E0065*/
                m = CopyBlock->ReadBSize[i]; /* */    /*E0066*/

                if(CopyBlock->ToOnlyAxis == (CopyBlock->ToBlock.Rank-1))
                { if(RTL_TRACE && dacopy_Trace &&
                     TstTraceEvent(call_aarrcp_))
                     tprintf("*** ACopyBlock1: Axis -> Axis; "
                             "Wait Recv (fast).\n");

                  SYSTEM(memcpy, (CharPtr1, ReadElmPtr, m))
                }
                else
                { if(RTL_TRACE && dacopy_Trace &&
                     TstTraceEvent(call_aarrcp_))
                     tprintf("*** ACopyBlock1: Axis -> Axis; "
                             "Wait Recv.\n");

                  WriteIndex[CopyBlock->ToOnlyAxis]++;

                  LocElmAddr(CharPtr2, CopyBlock->ToDArr,
                             WriteIndex)   /* */    /*E0067*/
                  k = m/n;               /* */    /*E0068*/
                  j = (int)((DvmType)CharPtr2 - (DvmType)CharPtr1);
                                                /* */    /*E0069*/

                  for(p=0; p < k; p++)
                  {  CharPtr2 = CharPtr1;

                     for(q=0; q < n; q++, ReadElmPtr++, CharPtr2++)
                         *CharPtr2 = *ReadElmPtr;

                     CharPtr1 += j;
                  }
                } 

                mac_free(&CopyBlock->ReadBuf[i]);
             } 
          }
       }
       else
       { for(i=0; i < CopyBlock->ReadVMSize; i++)
         {  if(CopyBlock->ReadBSize[i] == 0)
               continue;
            if(CopyBlock->ExchangeScheme == 0 &&
               CopyBlock->Alltoall == 0)
               (RTL_CALL, rtl_Waitrequest(&(CopyBlock->ReadReq[i])));

            ReadElmPtr = (char *)CopyBlock->ReadBuf[i];

            if(CopyBlock->EquSign)
            {  /* dimensions of read block and written block and  
                size of each dimension are equal*/    /*E0070*/

               CurrWriteBlock = &CopyBlock->ToLocalBlock[i];

               wWriteBlock = block_Copy(CurrWriteBlock);

               n = CopyBlock->ToBlock.Rank;
               LinInd = CurrWriteBlock->Set[n-1].Size;
               n = (int)(LinInd * CopyBlock->ToDArr->TLen);
               k = (int)(CopyBlock->ReadBSize[i] / n);

               for(j=0; j < k; j++)
               {  index_FromBlock1(WriteIndex, &wWriteBlock,
                                   CurrWriteBlock)
                  PutLocElm1(ReadElmPtr, CopyBlock->ToDArr, WriteIndex,
                             LinInd)
                  ReadElmPtr += n;
               } 
            }
            else
            {  /* dimensions of read block and written block or  
                size of any dimension are not equal */    /*E0071*/

               CurrWriteBlock = CopyBlock->CurrWriteBlock;

               wWriteBlock = block_Copy(CurrWriteBlock);

               block_GetSize(LinInd, CurrWriteBlock, CopyBlock->ToStep)
               n = (int)LinInd; /* size of local part of
                                 the written block */    /*E0072*/

               for(k=0; k < n; k++)
               { index_FromBlock(WriteIndex, &wWriteBlock,
                                 CurrWriteBlock, CopyBlock->ToStep)
                 index_GetLI(LinInd, &CopyBlock->ToBlock,
                             WriteIndex, CopyBlock->ToStep)
                 index_GetSI(&CopyBlock->FromBlock,
                             CopyBlock->ReadWeight,
                             LinInd, ReadIndex, CopyBlock->FromStep)

                 if(CopyBlock->ReadVMS->HasCurrent &&
                    CopyBlock->IsReadInter
                    [CopyBlock->ReadVMS->CurrentProc])
                 {  IsElmOfBlock(j, &(CopyBlock->ReadLocalBlock
                                      [CopyBlock->ReadVMS->CurrentProc]),
                                    ReadIndex)
                    if(j)
                       continue;
                 }

                 IsElmOfBlock(j, &(CopyBlock->ReadLocalBlock[i]),
                              ReadIndex)
                 if(j == 0)
                    continue;

                 PutLocElm(ReadElmPtr, CopyBlock->ToDArr, WriteIndex)
                 ReadElmPtr += CopyBlock->ToDArr->TLen;
               }
            }

            mac_free(&(CopyBlock->ReadBuf[i]));
         }
       }

       dvm_FreeArray(CopyBlock->ReadReq); 
       dvm_FreeArray(CopyBlock->ReadBuf); 
       dvm_FreeArray(CopyBlock->IsReadInter); 
       dvm_FreeArray(CopyBlock->ReadLocalBlock); 
       dvm_FreeArray(CopyBlock->ReadBSize); 

       dvm_FreeArray(CopyBlock->WriteReq); 
       dvm_FreeArray(CopyBlock->WriteBuf); 
       dvm_FreeArray(CopyBlock->IsWriteInter); 
       dvm_FreeArray(CopyBlock->WriteLocalBlock); 
       dvm_FreeArray(CopyBlock->WriteBSize);

       dvm_FreeArray(CopyBlock->ToLocalBlock);

       dvm_FreeArray(CopyBlock->FromVM); 
       dvm_FreeArray(CopyBlock->ToVM); 

       break;

       default:
       /******/    /*E0073*/

       break;
     }

     mac_free((void **)&ContInfo);
  }

  if(CopyCont != NULL)
     mac_free((void **)&CopyCont);

  *CopyFlagPtr = 0;

  if(RTL_TRACE)
     dvm_trace(ret_waitcp_," \n");

  DVMFTimeFinish(ret_waitcp_);
  return  (DVM_RET, 0);
}


/* -------------------------------------------------- */    /*E0074*/


int AGetElm(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[],
            AddrType *CopyFlagPtr)
{ s_AMVIEW     *AMS;
  s_VMS        *VMS;
  int           VMSize, DVM_VMSize, i, j, Proc;
  s_BLOCK      *wLocalBlock, Block;
  RTL_Request  *SendReq, *RecvReq;
  int          *SendFlag, *RecvFlag;
  char         *_BufferPtr = NULL;
  s_COPYCONT   *CopyCont;
  s_ARWElm     *RWElm;

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;
  DVM_VMSize = DVM_VMS->ProcCount;

  dvm_AllocArray(RTL_Request, DVM_VMSize, SendReq);
  dvm_AllocArray(RTL_Request, DVM_VMSize, RecvReq);
  dvm_AllocArray(int, DVM_VMSize, SendFlag);
  dvm_AllocArray(int, DVM_VMSize, RecvFlag);

  /* Stuctures for continuation */    /*E0075*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ARWElm, RWElm);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)RWElm;

  /* ------------------------- */    /*E0076*/

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
  {  /* An element is from current processor local part */    /*E0077*/

     GetLocElm(DArr, IndexArray, BufferPtr)
     if(_BufferPtr)
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))

     for(i=0; i < DVM_VMSize; i++)
     { Proc = (int)DVM_VMS->VProc[i].lP;

       if(Proc == MPS_CurrentProc)
          continue;

       if( (j = IsProcInVMS(Proc, AMS->VMS)) >= 0 )
           wLocalBlock = GetSpaceLB4Proc(j, AMS, &DArr->Space,
                                         DArr->Align, NULL, &Block);
       else
           wLocalBlock = NULL;

       if(wLocalBlock)
       {  IsElmOfBlock(j, wLocalBlock, IndexArray)
       }
       else
          j = 0;

       if(j == 0)
       {  if(_BufferPtr)
             ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], a_Elm_Mem) );
          else
             ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], a_Elm_Mem) );
          SendFlag[i] = 1;
       }
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffElmCopy);
     }

     CopyCont->Oper = send_AGetElm; /* code of continue operation */    /*E0078*/
  }
  else
  {  /* An element is not from current processor local part */    /*E0079*/

     for(i=0; i < VMSize; i++)
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

     CopyCont->Oper = recv_AGetElm; /* code of continue operation */    /*E0080*/
  }

  /* Save information for asynchrinouse continuation */    /*E0081*/

  RWElm->VMSize     = VMSize;
  RWElm->DArr       = DArr;
  RWElm->IndexArray = IndexArray;
  RWElm->SendFlag   = SendFlag;
  RWElm->RecvFlag   = RecvFlag;
  RWElm->SendReq    = SendReq;
  RWElm->RecvReq    = RecvReq;
  RWElm->BufferPtr  = BufferPtr;
  RWElm->_BufferPtr = _BufferPtr;

  return (int)DArr->TLen;
}



int   AIOGetElm(char  *BufferPtr, s_DISARRAY  *DArr, DvmType  IndexArray[],
                AddrType  *CopyFlagPtr)
{ s_AMVIEW     *AMS;
  s_VMS        *VMS;
  int           VMSize, i, j, Proc;
  s_BLOCK      *wLocalBlock, Block;
  RTL_Request  *SendReq, *RecvReq;
  int          *SendFlag, *RecvFlag;
  char         *_BufferPtr = NULL; 
  s_COPYCONT   *CopyCont;
  s_ARWElm     *RWElm;

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, RecvReq);
  dvm_AllocArray(int, VMSize, RecvFlag);
  dvm_AllocArray(RTL_Request, 1, SendReq);
  dvm_AllocArray(int, 1, SendFlag);

  /* Structures for continuation */    /*E0082*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ARWElm, RWElm);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)RWElm;

  /* ------------------------- */    /*E0083*/

  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      RecvFlag[i] = 0;

  SendFlag[0] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0084*/

     if(DArr->HasLocal)
        IsElmOfBlock(i, &DArr->Block, IndexArray)
     else
        i = 0;

     if(i)
        GetLocElm(DArr, IndexArray, BufferPtr)
     else
     {  SendFlag[0] = 1;

        for(i=0; i < VMSize; i++)
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
     }

     CopyCont->Oper = recv_AIOGetElm; /* code of continue operation */    /*E0085*/
  }
  else
  {  /* Current processor is not I/O processor */    /*E0086*/

     if(DArr->HasLocal)
        IsElmOfBlock(i, &DArr->Block, IndexArray)
     else
        i = 0;
 
     if(i)
     {  if( (i = IsProcInVMS(DVM_IOProc, AMS->VMS)) >= 0 )
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

           SendFlag[0] = 1;

           if(_BufferPtr)
           {  SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))
              ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen,
                                         DVM_IOProc, DVM_VMS->tag_DACopy,
                                         SendReq, a_Elm_IOMem) );
           }
           else
              ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen,
                                         DVM_IOProc, DVM_VMS->tag_DACopy,
                                         SendReq, a_Elm_IOMem) );
        }
     }

     CopyCont->Oper = send_AIOGetElm; /* code of continue operation */    /*E0087*/
  }

  /* Save information for asynchronous continuation */    /*E0088*/

  RWElm->VMSize     = VMSize;
  RWElm->DArr       = DArr;
  RWElm->IndexArray = IndexArray;
  RWElm->SendFlag   = SendFlag;
  RWElm->RecvFlag   = RecvFlag;
  RWElm->SendReq    = SendReq;
  RWElm->RecvReq    = RecvReq;
  RWElm->BufferPtr  = BufferPtr;
  RWElm->_BufferPtr = _BufferPtr;

  return  (int)DArr->TLen;
}



int   AIOPutElm(char  *BufferPtr, s_DISARRAY  *DArr, DvmType  IndexArray[],
                AddrType  *CopyFlagPtr)
{ s_AMVIEW     *AMS;
  s_VMS        *VMS;
  int           VMSize, i, j, Proc;
  s_BLOCK      *wLocalBlock, Block;
  RTL_Request  *SendReq, *RecvReq;
  int          *SendFlag, *RecvFlag;
  char         *_BufferPtr = NULL;
  s_COPYCONT   *CopyCont;
  s_ARWElm     *RWElm;
 
  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  dvm_AllocArray(RTL_Request, 1, RecvReq);
  dvm_AllocArray(int, 1, RecvFlag);

  /* Structures for continuation */    /*E0089*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ARWElm, RWElm);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)RWElm;

  /* ------------------------- */    /*E0090*/

  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      SendFlag[i] = 0;

  RecvFlag[0] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  { /* Current processor is I/O processor */    /*E0091*/

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
                                        &SendReq[i], a_IOMem_Elm) );
          else
             ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                        DVM_VMS->tag_DACopy,
                                        &SendReq[i], a_IOMem_Elm) );
          SendFlag[i] = 1;
       }
    }

    if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
    {  rtl_TstReqColl(0);
       rtl_SendReqColl(ResCoeffElmCopy);
    }

    CopyCont->Oper = send_AIOPutElm; /* code of continue operation */    /*E0092*/
  }
  else
  { /* Current processor is not I/O processor */    /*E0093*/

    if(DArr->HasLocal)
       IsElmOfBlock(j, &DArr->Block, IndexArray)
    else
       j = 0;

    if(j)
    {  RecvFlag[0] = 1;

       if(_BufferPtr)
         ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen,
                                    DVM_IOProc, DVM_VMS->tag_DACopy,
                                    RecvReq, 0) );
       else
         ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen,
                                    DVM_IOProc, DVM_VMS->tag_DACopy,
                                    RecvReq, 0) );
    }

    CopyCont->Oper = recv_AIOPutElm; /* code of continue operation */    /*E0094*/
  }

  /* Save information for asynchronous continuation */    /*E0095*/

  RWElm->VMSize     = VMSize;
  RWElm->DArr       = DArr;
  RWElm->IndexArray = IndexArray;
  RWElm->SendFlag   = SendFlag;
  RWElm->RecvFlag   = RecvFlag;
  RWElm->SendReq    = SendReq;
  RWElm->RecvReq    = RecvReq;
  RWElm->BufferPtr  = BufferPtr;
  RWElm->_BufferPtr = _BufferPtr;

  return  (int)DArr->TLen;
}



int AIOPutElmRepl(char *BufferPtr, s_DISARRAY *DArr, DvmType IndexArray[],
                  AddrType *CopyFlagPtr)
{ s_AMVIEW      *AMS;
  s_VMS         *VMS;
  int            VMSize, i, Proc;
  RTL_Request   *SendReq, *RecvReq;
  int           *SendFlag, *RecvFlag;
  char          *_BufferPtr = NULL;
  s_COPYCONT    *CopyCont;
  s_ARWElm      *RWElm;

  AMS = DArr->AMView;
  VMS = AMS->VMS;
  VMSize = VMS->ProcCount;

  dvm_AllocArray(RTL_Request, VMSize, SendReq);
  dvm_AllocArray(int, VMSize, SendFlag);
  dvm_AllocArray(RTL_Request, 1, RecvReq);
  dvm_AllocArray(int, 1, RecvFlag);

  /* Structures for continuation */    /*E0096*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ARWElm, RWElm);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)RWElm;

  /* ------------------------- */    /*E0097*/

  if((IsSynchr && UserSumFlag) || (DArr->TLen & Msk3))
  {  mac_malloc(_BufferPtr, char *, DArr->TLen, 0);
  }

  for(i=0; i < VMSize; i++)
      SendFlag[i] = 0;

  if(MPS_CurrentProc == DVM_IOProc)
  {  /* Current processor is I/O processor */    /*E0098*/

     PutLocElm(BufferPtr,DArr,IndexArray)

     if(_BufferPtr)
        SYSTEM(memcpy, (_BufferPtr, BufferPtr, DArr->TLen))

     for(i=0; i < VMSize; i++)
     {  Proc = (int)VMS->VProc[i].lP;
        if(Proc == MPS_CurrentProc)
           continue;
        if(_BufferPtr)
           ( RTL_CALL, rtl_Sendnowait(_BufferPtr, 1, DArr->TLen, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_IOMem_ElmRepl) );
        else 
           ( RTL_CALL, rtl_Sendnowait(BufferPtr, 1, DArr->TLen, Proc,
                                      DVM_VMS->tag_DACopy,
                                      &SendReq[i], a_IOMem_ElmRepl) );
        SendFlag[i] = 1;
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffElmCopy);
     }

     CopyCont->Oper = send_AIOPutElmRepl; /* code of continue operation */    /*E0099*/
  }
  else
  {  /* Current processor is not I/O processor */    /*E0100*/

     if(_BufferPtr)
        ( RTL_CALL, rtl_Recvnowait(_BufferPtr, 1, DArr->TLen, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, RecvReq, 0) );
     else 
        ( RTL_CALL, rtl_Recvnowait(BufferPtr, 1, DArr->TLen, DVM_IOProc,
                                   DVM_VMS->tag_DACopy, RecvReq, 0) );

     CopyCont->Oper = recv_AIOPutElmRepl; /* code of continue operation */    /*E0101*/
  }

  /* Save information for asynchronous continuation */    /*E0102*/

  RWElm->VMSize     = VMSize;
  RWElm->DArr       = DArr;
  RWElm->IndexArray = IndexArray;
  RWElm->SendFlag   = SendFlag;
  RWElm->RecvFlag   = RecvFlag;
  RWElm->SendReq    = SendReq;
  RWElm->RecvReq    = RecvReq;
  RWElm->BufferPtr  = BufferPtr;
  RWElm->_BufferPtr = _BufferPtr;

  return (int)DArr->TLen;
}



int ACopyElm(s_DISARRAY *FromDArr, DvmType FromIndexArray[],
             s_DISARRAY *ToDArr, DvmType ToIndexArray[],
             AddrType *CopyFlagPtr)
{ s_AMVIEW      *ReadAMS, *WriteAMS;
  s_VMS         *ReadVMS, *WriteVMS;
  int            ReadVMSize, WriteVMSize, i, j, Proc;
  s_BLOCK       *LocalBlock = NULL, Block;
  RTL_Request   *ReadReq, *WriteReq;
  int           *ReadFlag, *WriteFlag, IntersectFlag;
  void          **ReadBuf, **WriteBuf;
  s_COPYCONT    *CopyCont;
  s_ACopyElm    *CopyElm;

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

  /* Structures for continuation */    /*E0103*/

  dvm_AllocStruct(s_COPYCONT, CopyCont);
  dvm_AllocStruct(s_ACopyElm, CopyElm);

  *CopyFlagPtr       = (AddrType)CopyCont;
  CopyCont->ContInfo = (void *)CopyElm;

  /* ------------------------- */    /*E0104*/

  if(ToDArr->HasLocal)
     IsElmOfBlock(i, &ToDArr->Block, ToIndexArray)
  else
     i = 0;

  if(i)
  {  /* Written element is from current processor local part */    /*E0105*/

     if(FromDArr->HasLocal)
        IsElmOfBlock(j, &FromDArr->Block, FromIndexArray)
     else
        j = 0;

     if(j)
     {  /* Read element is from current processor local part */    /*E0106*/

        CopyLocElm(FromDArr, FromIndexArray, ToDArr, ToIndexArray)
     }
     else
     {  /* Read element is not from current processor local part */    /*E0107*/

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
  {  /* Read element is from current processor local part */    /*E0108*/
     
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
        {  GetLocElm(FromDArr, FromIndexArray, WriteBuf[i])
           WriteFlag[i] = 1;
           ( RTL_CALL, rtl_Sendnowait(WriteBuf[i], 1, FromDArr->TLen,
                                      Proc, DVM_VMS->tag_DACopy,
                                      &WriteReq[i], a_Elm_Elm) );
        }
     }

     if(MsgSchedule && UserSumFlag && DVM_LEVEL == 0)
     {  rtl_TstReqColl(0);
        rtl_SendReqColl(ResCoeffElmCopy);
     }
  }

  CopyCont->Oper = sendrecv_ACopyElm; /* code of continue operation */    /*E0109*/

  /* Save information for asynchronous continuation */    /*E0110*/

  CopyElm->ReadVMSize   = ReadVMSize;
  CopyElm->WriteVMSize  = WriteVMSize;
  CopyElm->ReadFlag     = ReadFlag;
  CopyElm->WriteFlag    = WriteFlag;
  CopyElm->ReadReq      = ReadReq;
  CopyElm->WriteReq     = WriteReq;
  CopyElm->ReadBuf      = ReadBuf;
  CopyElm->WriteBuf     = WriteBuf;
  CopyElm->ToDArr       = ToDArr;
  CopyElm->ToIndexArray = ToIndexArray;

  return  (int)FromDArr->TLen;
}


#endif   /* _AELMCOPY_C_ */    /*E0111*/
