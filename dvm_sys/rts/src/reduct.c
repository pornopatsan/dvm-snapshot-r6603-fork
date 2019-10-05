#ifndef _REDUCT_C_
#define _REDUCT_C_

#include "system.typ"

/****************/    /*E0000*/

/*********************\
* Reduction functions *
\*********************/    /*E0001*/


RedRef  __callstd crtred_(DvmType *RedFuncNumbPtr, void *RedArrayPtr,
                          DvmType *RedArrayTypePtr, DvmType *RedArrayLengthPtr,
                          void *LocArrayPtr, DvmType *LocElmLengthPtr,
                          DvmType *StaticSignPtr)

/*
*RedFuncNumbPtr	   - the number of the reduction function.
RedArrayPtr	   - pointer to the reduction array-variable.
*RedArrayTypePtr   - the type of the elements of the array-variable.
*RedArrayLengthPtr - the number of the elements in the array-variable.
LocArrayPtr        - pointer to the array containing additional
                     information about reduction function.
*LocElmLengthPtr   - the size (in bytes) of an element of the array with
                     additional information.
*StaticSignPtr	   - the flag of the static reduction declaration.

The function crtred_ creates a descriptor of the reduction.
The function returns reference to this descriptor
(or the reference to the reduction).
*/    /*E0002*/

{ SysHandle    *RVHandlePtr, *RedArrayHandlePtr = NULL,
               *LocArrayHandlePtr = NULL;
  s_REDVAR     *RVPtr;
  RedRef        Res;
  s_AMVIEW     *RedAMView, *LocAMView;
  s_VMS        *RedVMS;
  int           i, RedVMSRank, RedArrayRank;
  s_DISARRAY   *RedDArr, *LocDArr;
  s_BLOCK      *Local;
  byte          Step = 0;
  DvmType          bSize;

  if(RgSave)
  {  /* */    /*E0003*/

     /* */    /*E0004*/

     for(i=0; i < RvCount; i++)
     {  if(DVM_LINE[DVM_LEVEL] != RvLine[i])
           continue;
        SYSTEM_RET(RedVMSRank, strcmp, (DVM_FILE[DVM_LEVEL], RvFile[i]))
        if(RedVMSRank == 0)
           break;
     }

     if(i < RvCount)
     {  Res = (RedRef)RvRef[i];
        return  (DVM_RET, Res);
     }
  }

  /* */    /*E0005*/

  DVMFTimeStart(call_crtred_);

  dvm_AllocStruct(SysHandle, RVHandlePtr);
  dvm_AllocStruct(s_REDVAR, RVPtr);

  RedArrayHandlePtr = TstDVMArray(RedArrayPtr);
  LocArrayHandlePtr = TstDVMArray(LocArrayPtr);

  if(RedArrayHandlePtr != NULL)
  {  /* */    /*E0006*/

     if(RTL_TRACE)
     {  if(LocArrayHandlePtr != NULL)
        {  /* */    /*E0007*/

           dvm_trace(call_crtred_,
             "RedFuncNumb=%ld; RedArrayHeader=%lx; "
             "RedArrayHandlePtr=%lx; "
             "RedArrayType=%ld; RedArrayLength=%ld; "
             "LocArrayHeader=%lx; LocArrayHandlePtr=%lx; "
             "LocElmLength=%ld; StaticSign=%ld;\n",
             *RedFuncNumbPtr,(uLLng)RedArrayPtr, (uLLng)RedArrayHandlePtr,
             *RedArrayTypePtr, *RedArrayLengthPtr, (uLLng)LocArrayPtr,
             (uLLng)LocArrayHandlePtr, *LocElmLengthPtr, *StaticSignPtr);
        }
        else
        {  /* */    /*E0008*/

           dvm_trace(call_crtred_,
             "RedFuncNumb=%ld; RedArrayHeader=%lx; "
             "RedArrayHandlePtr=%lx; "
             "RedArrayType=%ld; RedArrayLength=%ld; "
             "LocArrayPtr=%lx; LocElmLength=%ld; StaticSign=%ld;\n",
             *RedFuncNumbPtr,(uLLng)RedArrayPtr, (uLLng)RedArrayHandlePtr,
             *RedArrayTypePtr, *RedArrayLengthPtr, (uLLng)LocArrayPtr,
             *LocElmLengthPtr, *StaticSignPtr);
        }
     }

     RedDArr        = (s_DISARRAY *)RedArrayHandlePtr->pP;
     RedArrayRank   = RedDArr->Space.Rank;
     RVPtr->RedDArr = RedDArr;
     RedAMView      = RedDArr->AMView;

     /* */    /*E0009*/

     if(RedAMView == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.010: wrong call crtred_\n"
              "(the reduction array has not been aligned; "
              "RedArrayHeader[0]=%lx)\n", (uLLng)RedArrayHandlePtr);

     /* */    /*E0010*/

     RedVMS = RedAMView->VMS;

     NotSubsystem(i, DVM_VMS, RedVMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 069.011: wrong call crtred_\n"
          "(the reduction array PS is not a subsystem "
          "of the current PS;\n"
          "RedArrayHeader[0]=%lx; RedArrayPSRef=%lx; "
          "CurrentPSRef=%lx)\n",
          (uLLng)RedArrayHandlePtr, (uLLng)RedVMS->HandlePtr,
          (uLLng)DVM_VMS->HandlePtr);

     RVPtr->DAAxisPtr = RedDArr->DAAxis; /* */    /*E0011*/

     /* */    /*E0012*/

     RedVMSRank = RedVMS->Space.Rank;

     for(i=0; i < RedVMSRank; i++)
         if(RVPtr->DAAxisPtr[i] == 0)
            break;

     if(i == RedVMSRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.012: wrong call crtred_\n"
              "(the reduction array is not a replicated array; "
              "RedArrayHeader[0]=%lx)\n", (uLLng)RedArrayHandlePtr);

     /* */    /*E0013*/

     if(RedDArr->HasLocal)
     {  Local = &RedDArr->ArrBlock.Block;
        block_GetSize(bSize, Local, Step)
        RVPtr->VLength = (int)bSize;      /* number of elements
                                             in reduction
                                             variable-array */    /*E0014*/
        RVPtr->Mem = (char *)
        RedDArr->ArrBlock.ALoc.Ptr;       /* reduction variable-array
                                             address */    /*E0015*/
     }
     else
     {  RVPtr->VLength = 0; /* number of elements
                               in reduction variable-array */    /*E0016*/
        RVPtr->Mem = NULL;  /* reduction variable-array address */    /*E0017*/
     }

     if(LocArrayHandlePtr != NULL)
     {  /* */    /*E0018*/

        LocDArr        = (s_DISARRAY *)LocArrayHandlePtr->pP;
        RVPtr->LocDArr = LocDArr;
        LocAMView      = LocDArr->AMView;

        /* */    /*E0019*/

        if(LocAMView == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 069.013: wrong call crtred_\n"
                 "(the local information array has not been aligned; "
                 "LocArrayHeader[0]=%lx)\n", (uLLng)LocArrayHandlePtr);

        /* */    /*E0020*/

        if(IsArrayEqu(RedArrayHandlePtr, LocArrayHandlePtr,
                      0, 0, NULL, NULL) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 069.014: wrong call crtred_\n"
                 "(the reduction array is not equal to "
                 "the local information array;\n"
                 "RedArrayHeader[0]=%lx; LocArrayHeader[0]=%lx)\n",
                 (uLLng)RedArrayHandlePtr, (uLLng)LocArrayHandlePtr);

        for(i=0; i < RedArrayRank; i++)
            if( RedDArr->InitLowShdWidth[i]  !=
                LocDArr->InitLowShdWidth[i] ||
                RedDArr->InitHighShdWidth[i] !=
                LocDArr->InitHighShdWidth[i] )
                break;

        if(i != RedArrayRank)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 069.015: wrong call crtred_\n"
                 "(boundaries of the reduction array are not equal to "
                 "boundaries of the local information array;\n"
                 "RedArrayHeader[0]=%lx; LocArrayHeader[0]=%lx)\n",
                 (uLLng)RedArrayHandlePtr, (uLLng)LocArrayHandlePtr);

        if(LocDArr->HasLocal)
        {  RVPtr->LocMem       = (char *)
           LocDArr->ArrBlock.ALoc.Ptr; /* additional information
                                          address */    /*E0021*/
           RVPtr->LocElmLength =
           LocDArr->TLen;              /* length of one element with
                                          additional inforrmation */    /*E0022*/
        }
        else
        {  RVPtr->LocMem = NULL;    /* additional information address */    /*E0023*/
           RVPtr->LocElmLength = 0; /* length of one element with
                                       additional inforrmation */    /*E0024*/
        }
     }
     else
     {  /* */    /*E0025*/

        RVPtr->LocDArr = NULL;
        LocDArr        = NULL;
        RVPtr->LocMem  = (char *)LocArrayPtr;   /* additional information
                                                address */    /*E0026*/
        RVPtr->LocElmLength =
        (int)*LocElmLengthPtr;     /* length of one element with
                                      additional inforrmation */    /*E0027*/
     }
  }
  else
  {  /* */    /*E0028*/

     if(LocArrayHandlePtr != NULL)
     {  if(RTL_TRACE)
           dvm_trace(call_crtred_,
                  "RedFuncNumb=%ld; RedArrayPtr=%lx; RedArrayType=%ld; "
                  "RedArrayLength=%ld; "
                  "LocArrayHeader=%lx; LocArrayHandlePtr=%lx; "
                  "LocElmLength=%ld; StaticSign=%ld;\n",
                  *RedFuncNumbPtr, (uLLng)RedArrayPtr, *RedArrayTypePtr,
                  *RedArrayLengthPtr, (uLLng)LocArrayPtr,
                  (uLLng)LocArrayHandlePtr, *LocElmLengthPtr,
                  *StaticSignPtr);

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 069.008: wrong call crtred_\n"
               "(the reduction variable is not a disributed array, but "
               "the local information variable is a distributed array; "
               "RedArrayPtr=%lx; LocArrayHeader[0]=%lx)\n",
               (uLLng)RedArrayPtr, (uLLng)LocArrayHandlePtr);
     }

     if(RTL_TRACE)
        dvm_trace(call_crtred_,
                  "RedFuncNumb=%ld; RedArrayPtr=%lx; RedArrayType=%ld; "
                  "RedArrayLength=%ld; LocArrayPtr=%lx; "
                  "LocElmLength=%ld; StaticSign=%ld;\n",
                  *RedFuncNumbPtr,(uLLng)RedArrayPtr,*RedArrayTypePtr,
                  *RedArrayLengthPtr,(uLLng)LocArrayPtr,
                  *LocElmLengthPtr,*StaticSignPtr);

     RedDArr          = NULL;
     LocDArr          = NULL;
     RVPtr->RedDArr   = NULL;
     RVPtr->LocDArr   = NULL;
     RVPtr->DAAxisPtr = NULL; /* */    /*E0029*/
     RVPtr->Mem     = (char *)RedArrayPtr;    /* reduction variable-array
                                         address */    /*E0030*/
     RVPtr->VLength = (int)*RedArrayLengthPtr; /* number of elements
                                                  in reduction
                                                  variable-array */    /*E0031*/
     RVPtr->LocMem  = (char *)LocArrayPtr;   /* additional information
                                             address */    /*E0032*/
     RVPtr->LocElmLength =
     (int)*LocElmLengthPtr;     /* length of one element with
                                   additional inforrmation */    /*E0033*/
  }

  RVPtr->Func         = (byte)*RedFuncNumbPtr; /* reduction function
                                                  number */    /*E0034*/
  RVPtr->VType        = (int)*RedArrayTypePtr; /* reduction
                                                  variable type */    /*E0035*/
  RVPtr->Static       = (byte)*StaticSignPtr;  /* flag of static variable   */    /*E0036*/
  RVPtr->RG           = NULL; /* pointer to reduction group */    /*E0037*/
  RVPtr->AMView       = NULL; /* pointer to abstract machine
                                 representation for subtask group */    /*E0038*/
  RVPtr->BufAddr      = NULL; /* variable address in buffer */    /*E0039*/
  RVPtr->LocIndType   = 1;    /* type of index variables of local
                                 maximum or minimum, 1 - integer */    /*E0040*/

  switch(RVPtr->VType)
  {  case rt_INT   :         RVPtr->RedElmLength = sizeof(int);
                             break;
     case rt_LONG  :         RVPtr->RedElmLength = sizeof(long);
                             break;
     case rt_LLONG:          RVPtr->RedElmLength = sizeof(long long);
                             break;
     case rt_DOUBLE:         RVPtr->RedElmLength = sizeof(double);
                             break;
     case rt_FLOAT :         RVPtr->RedElmLength = sizeof(float);
                             break;
     case rt_DOUBLE_COMPLEX: RVPtr->RedElmLength = 2*sizeof(double);
                             break;
     case rt_FLOAT_COMPLEX:  RVPtr->RedElmLength = 2*sizeof(float);
                             break;
     default:        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                         "*** RTS err 069.000: wrong call crtred_\n"
                         "(invalid number of reduction variable type; "
                         "RedArrayType=%d)\n", RVPtr->VType);
  }

  /* Check correctness of reduction function number */    /*E0041*/

  if(RVPtr->Func != rf_SUM && RVPtr->Func != rf_MULT &&
     RVPtr->Func != rf_MAX && RVPtr->Func != rf_MIN  &&
     RVPtr->Func != rf_AND && RVPtr->Func != rf_OR   &&
     RVPtr->Func != rf_XOR && RVPtr->Func != rf_EQU  &&
     RVPtr->Func != rf_EQ  && RVPtr->Func != rf_NE     )
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.001: wrong call crtred_\n"
              "(invalid number of reduction function; "
              "RedFuncNumb=%d)\n", (int)RVPtr->Func);

  /* Check reduction operation correspondence
         to reductionm variable type             */    /*E0042*/

  if((RVPtr->Func == rf_AND || RVPtr->Func == rf_OR ||
      RVPtr->Func == rf_XOR || RVPtr->Func == rf_EQU) &&
      RVPtr->VType != rt_INT && RVPtr->VType != rt_LONG && RVPtr->VType != rt_LLONG)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.002: wrong call crtred_\n"
              "(invalid reduction variable type; "
              "RedFuncNumb=%d; RedArrayType=%d)\n",
              (int)RVPtr->Func, RVPtr->VType);

  if((RVPtr->VType == rt_FLOAT_COMPLEX ||
      RVPtr->VType == rt_DOUBLE_COMPLEX)  &&
      RVPtr->Func != rf_SUM && RVPtr->Func != rf_MULT)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.003: wrong call crtred_\n"
              "(invalid complex type of the reduction variable; "
              "RedFuncNumb=%d; RedArrayType=%d)\n",
              (int)RVPtr->Func, RVPtr->VType);

  if(RVPtr->Func == rf_MIN || RVPtr->Func == rf_MAX)
  {  if( (RVPtr->LocElmLength != 0  &&  RVPtr->LocMem != NULL) ||
         LocDArr != NULL )
     {  if(RedArrayHandlePtr != NULL && LocArrayHandlePtr == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 069.009: wrong call crtred_\n"
               "(the reduction variable is a disributed array, but the "
               "local information variable is not a distributed array; "
               "RedArrayHeader[0]=%lx; LocArrayPtr=%lx)\n",
               (uLLng)RedArrayHandlePtr, (uLLng)LocArrayPtr);

        switch(RVPtr->Func)
        {  case rf_MIN:   RVPtr->Func = rf_MINLOC;  break;
           case rf_MAX:   RVPtr->Func = rf_MAXLOC;  break;
        }
     }
     else
     {  RVPtr->LocElmLength = 0;
        RVPtr->LocMem       = NULL;
     }
  }
  else
  {  RVPtr->LocElmLength = 0;
     RVPtr->LocMem       = NULL;
     RVPtr->LocDArr      = NULL;
  }

  RVPtr->BlockSize =
  RVPtr->VLength * (RVPtr->RedElmLength +
                    RVPtr->LocElmLength); /* total length of reduction
                                             variable */    /*E0043*/

  *RVHandlePtr = genv_InsertObject(sht_RedVar, RVPtr);
  RVPtr->HandlePtr = RVHandlePtr; /* pointer to own Handle */    /*E0044*/

  if(TstObject)
     InsDVMObj((ObjectRef)RVHandlePtr);

  Res = (RedRef)RVHandlePtr;

  if(RgSave)
  {  /* */    /*E0045*/

     if(RvCount < (RvNumber-1))
     {  /* */    /*E0046*/

        RvLine[RvCount] = DVM_LINE[DVM_LEVEL];
        RvFile[RvCount] = DVM_FILE[DVM_LEVEL];
        RvRef[RvCount]  = (uLLng)Res;
        RvCount++;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtred_,"RedRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0047*/
  DVMFTimeFinish(ret_crtred_);
  return  (DVM_RET, Res);
}



/*     For FORTRAN      */    /*E0048*/

RedRef __callstd crtrdf_(DvmType *RedFuncNumbPtr, AddrType *RedArrayAddrPtr,
                         DvmType *RedArrayTypePtr,DvmType *RedArrayLengthPtr,
                         void *LocArrayPtr, DvmType *LocElmLengthPtr,
                         DvmType *StaticSignPtr)
{
  return  crtred_(RedFuncNumbPtr, (void *)*RedArrayAddrPtr,
                  RedArrayTypePtr, RedArrayLengthPtr, LocArrayPtr,
                  LocElmLengthPtr, StaticSignPtr);
}

/*  ------------------  */    /*E0049*/


DvmType  __callstd lindtp_(RedRef  *RedRefPtr, DvmType  *LocIndTypePtr)
{ SysHandle  *RVHandlePtr;
  int         i;
  void       *CurrAM;
  s_REDVAR   *RVar;

  StatObjectRef = (ObjectRef)*RedRefPtr;    /* for statistics */    /*E0050*/
  DVMFTimeStart(call_lindtp_);

  if(RTL_TRACE)
     dvm_trace(call_lindtp_,
               "RedRefPtr=%lx; RedRef=%lx; LocIndType=%ld;\n",
               (uLLng)RedRefPtr, *RedRefPtr, *LocIndTypePtr);

  RVHandlePtr = (SysHandle *)*RedRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(RedRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 069.020: wrong call lindtp_\n"
            "(the reduction variable is not a DVM object; "
            "RedRef=%lx)\n", *RedRefPtr);
  }

  if(RVHandlePtr->Type != sht_RedVar)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 069.021: wrong call lindtp_\n"
           "(the object is not a reduction variable; RedRef=%lx)\n",
           *RedRefPtr);

  /* Check if reduction variable created in current subtask           */    /*E0051*/

  i = gEnvColl->Count - 1;          /* current context index */    /*E0052*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0053*/

  if(RVHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 069.022: wrong call lindtp_\n"
         "(the reduction variable was not created "
         "by the current subtask;\n"
         "RedRef=%lx; RedEnvIndex=%d; CurrentEnvIndex=%d)\n",
         *RedRefPtr, RVHandlePtr->CrtEnvInd, i);

  /* Check in reduction variable has been
        included in reduction a group     */    /*E0054*/

  RVar = (s_REDVAR *)RVHandlePtr->pP;

  if(RVar->RG)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 069.023: wrong call lindtp_\n"
         "(the reduction variable has already been inserted "
         "in the reduction group;\nRedRef=%lx; RedGroupRef=%lx)\n",
         *RedRefPtr, (uLLng)RVar->RG->HandlePtr);

  RVar->LocIndType = (int)*LocIndTypePtr; /* variable type ->
                                             in structure */    /*E0055*/

  /* Check code of the given type of index variables */    /*E0056*/

  if(RVar->LocIndType < 0 || RVar->LocIndType > 3)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 069.024: wrong call lindtp_\n"
              "(invalid LocIndType=%d)\n", RVar->LocIndType);

  if(RTL_TRACE)
     dvm_trace(ret_lindtp_," \n");

  DVMFTimeFinish(ret_lindtp_);
  return  0;
}



RedGroupRef __callstd crtrg_(DvmType  *StaticSignPtr, DvmType  *DelRedSignPtr)

/*
            Creating reduction group.
            -------------------------

*StaticSignPtr - the flag of the static reduction group creation.
*DelRedSignPtr - the flag of deleting of the all reduction descriptors
                 while deleting the reduction group.

The function crtrg_ creates empty reduction group .
The function returns reference to the created group.
*/    /*E0057*/

{ SysHandle    *RGHandlePtr;
  s_REDGROUP   *RG;
  RedGroupRef   Res;
  int           i, j;

  if(RgSave)
  {  /* */    /*E0058*/

     /* */    /*E0059*/

     for(i=0; i < RgCount; i++)
     {  if(DVM_LINE[DVM_LEVEL] != RgLine[i])
           continue;
        SYSTEM_RET(j, strcmp, (DVM_FILE[DVM_LEVEL], RgFile[i]))
        if(j == 0)
           break;
     }

     if(i < RgCount)
     {  Res = (RedGroupRef)RgRef[i];
        RG = (s_REDGROUP *)((SysHandle *)Res)->pP;
        RG->SaveSign = 1;  /* */    /*E0060*/
        return  (DVM_RET, Res);
     }
  }

  /* */    /*E0061*/

  DVMFTimeStart(call_crtrg_);

  if(RTL_TRACE)
     dvm_trace(call_crtrg_,"StaticSign=%ld; DelRedSign=%ld;\n",
                           *StaticSignPtr,*DelRedSignPtr);

  dvm_AllocStruct(s_REDGROUP, RG);
  dvm_AllocStruct(SysHandle, RGHandlePtr);

  RG->VMS              = NULL; /* processor system reduction
                                  group not defined yet */    /*E0062*/
  RG->PSSpaceVMS       = NULL; /* */    /*E0063*/
  RG->CrtPSSign        = 0;    /* */    /*E0064*/
  RG->CrtVMSSign       = 0;    /* */    /*E0065*/
  RG->BlockSize        = 0;    /* length of all group variables
                                  together with additional info */    /*E0066*/
  RG->Static           = (byte)*StaticSignPtr;
  RG->DelRed           = (byte)*DelRedSignPtr;
  RG->StrtFlag         = 0;    /* group not started */    /*E0067*/
  RG->RV               = coll_Init(RedVarGrpCount,RedVarGrpCount,NULL);
  RG->NoWaitBufferPtr  = NULL;
  RG->InitBuf          = NULL;
  RG->ResBuf           = NULL;
  RG->Flag             = NULL;
  RG->Req              = NULL;
  RG->MPIReduce        = 0;    /* */    /*E0068*/
  RG->DA               = NULL; /* */    /*E0069*/
  RG->DAAxisPtr        = NULL; /* */    /*E0070*/
  RG->TskRDSign        = 0;    /* */    /*E0071*/
  RG->IsBuffers        = 0;    /* */    /*E0072*/
  RG->IsNewVars        = 0;    /* */    /*E0073*/
  RG->ResetSign        = 0;    /* */    /*E0074*/

  *RGHandlePtr = genv_InsertObject(sht_RedGroup, RG);
  RG->HandlePtr = RGHandlePtr;  /* pointer to own Handle */    /*E0075*/

  if(TstObject)
     InsDVMObj((ObjectRef)RGHandlePtr);

  Res = (RedGroupRef)RGHandlePtr;

  RG->SaveSign = 0;  /* */    /*E0076*/

  if(RgSave)
  {  /* */    /*E0077*/

     if(RgCount < (RgNumber-1))
     {  /* */    /*E0078*/

        RgLine[RgCount] = DVM_LINE[DVM_LEVEL];
        RgFile[RgCount] = DVM_FILE[DVM_LEVEL];
        RgRef[RgCount]  = (uLLng)Res;
        RgCount++;
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_crtrg_,"RedGroupRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0079*/
  DVMFTimeFinish(ret_crtrg_);
  return  (DVM_RET, Res);
}


static int haveElements(s_VMS *PSSpaceVMS, int i, int j, s_AMVIEW *AMV, s_ALIGN *AlignRules) {
    int res = 1;
    if (AMV && AlignRules && AMV->AMVAxis[i]) {
        s_MAP *CMap = &AMV->DISTMAP[AMV->Space.Rank + i];
        int AMVAxis = CMap->Axis - 1;
        DvmType CLower, CUpper;
        s_ALIGN *CAlign = &AlignRules[AMVAxis];
        if (PSSpaceVMS == AMV->WeightVMS) {
            if (AMV->GenBlockCoordWeight[i])
                CLower = AMV->PrevSumGenBlockCoordWeight[i][j];
            else {
                CLower = (DvmType)(CMap->DisPar * AMV->PrevSumCoordWeight[i][j]);
                if (AMV->Div[AMVAxis] != 1)
                    CLower = (CLower / AMV->Div[AMVAxis]) * AMV->Div[AMVAxis];
            }
        } else {
            CLower = (DvmType)(CMap->DisPar * PSSpaceVMS->PrevSumCoordWeight[i][j]);
            if (AMV->Div[AMVAxis] != 1)
                CLower = (CLower / AMV->Div[AMVAxis]) * AMV->Div[AMVAxis];
        }
        if (j == PSSpaceVMS->Space.Size[i] - 1) {
           CUpper = AMV->Space.Size[AMVAxis] - 1;
        } else {
            if (PSSpaceVMS == AMV->WeightVMS) {
                if (AMV->GenBlockCoordWeight[i])
                    CUpper = CLower + AMV->GenBlockCoordWeight[i][j] - 1;
                else {
                    CUpper = (DvmType)(CMap->DisPar * AMV->PrevSumCoordWeight[i][j + 1]);
                    if (AMV->Div[AMVAxis] != 1)
                        CUpper = (CUpper / AMV->Div[AMVAxis]) * AMV->Div[AMVAxis];
                    CUpper -= 1;
                }
           } else {
                CUpper = (DvmType)(CMap->DisPar * PSSpaceVMS->PrevSumCoordWeight[i][j + 1]);
                if (AMV->Div[AMVAxis] != 1)
                    CUpper = (CUpper / AMV->Div[AMVAxis]) * AMV->Div[AMVAxis];
                CUpper -= 1;
           }
        }
        res = CLower <= CUpper && CUpper <= AMV->Space.Size[AMVAxis] - 1;
        if (CAlign->Attr == align_CONSTANT)
            res = res && (CAlign->B >= CLower && CAlign->B <= CUpper);
        else if (CAlign->Attr == align_REPLICATE)
            res = res;
        else if (CAlign->Attr == align_BOUNDREPL) {
            DvmType SpB1 = (DvmType)ceil((double)(CLower - CAlign->B) / (double)CAlign->A);
            DvmType SpB2 = (CUpper - CAlign->B) / CAlign->A;
            DvmType SpLower = dvm_min(SpB1, SpB2);
            DvmType SpUpper = dvm_max(SpB1, SpB2);
            res = res && (SpUpper >= 0 && SpLower < CAlign->Bound);
        }
    }
    return res;
}

DvmType __callstd insred_(RedGroupRef  *RedGroupRefPtr, RedRef  *RedRefPtr, PSSpaceRef  *PSSpaceRefPtr, DvmType  *RenewSignPtr)

/*
     Including reduction in reduction group.
     ---------------------------------------

*RedGroupRefPtr	- reference to the reduction group.
*RedRefPtr      - reference to the reduction.
*PSSpaceRefPtr  - reference to the specificator of processor space.

Including reduction in reduction group means only registration of this
operation as a member of the group reduction operation.
The function returns zero.
*/    /*E0080*/

{ SysHandle       *RGHandlePtr, *RVHandlePtr, *ParHandlePtr;
  s_REDGROUP      *RG;
  s_REDVAR        *RVar;
  int              i, VarSize, LocSize, RenewSign;
  void            *CurrAM;
  s_PARLOOP       *PL;
  s_AMVIEW        *AMV = 0;
  s_ALIGN         *AlignRules = 0;
  s_VMS           *DArrVMS, *PSSpaceVMS;
  s_AMS           *AMS;
  s_DISARRAY      *DArr;
  DvmType             InitIndexArray[MAXARRAYDIM],
                   LastIndexArray[MAXARRAYDIM];
  int             *SpaceAxis = NULL;
  PSRef            RGPSRef;

  RVHandlePtr = (SysHandle *)*RedRefPtr;
  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  RG   = (s_REDGROUP *)RGHandlePtr->pP;
  RVar = (s_REDVAR *)RVHandlePtr->pP;

  if(RgSave && RG->VMS != NULL)
     goto DynDyn;

  StatObjectRef = (ObjectRef)*RedRefPtr;    /* for statistics */    /*E0081*/
  DVMFTimeStart(call_insred_);

  RG->IsNewVars = 1;  /* */    /*E0082*/

  if(RenewSignPtr == NULL)
     RenewSign = 0;
  else
     RenewSign = *RenewSignPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.000: wrong call insred_\n"
            "(the reduction group is not a DVM object; "
            "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 070.001: wrong call insred_\n"
          "(the object is not a reduction group; RedGroupRef=%lx)\n",
          *RedGroupRefPtr);

  if(TstObject)
  {  if(TstDVMObj(RedRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.002: wrong call insred_\n"
            "(the reduction variable is not a DVM object; "
            "RedRef=%lx)\n", *RedRefPtr);
  }

  if(RVHandlePtr->Type != sht_RedVar)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.003: wrong call insred_\n"
           "(the object is not a reduction variable; RedRef=%lx)\n",
           *RedRefPtr);

  /* Check if reduction group and reduction
     variable in current subtask are created */    /*E0083*/

  i = gEnvColl->Count - 1;          /* current context index */    /*E0084*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0085*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.005: wrong call insred_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, i);

  if(RVHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 070.006: wrong call insred_\n"
         "(the reduction variable was not created "
         "by the current subtask;\n"
         "RedRef=%lx; RedEnvIndex=%d; CurrentEnvIndex=%d)\n",
         *RedRefPtr, RVHandlePtr->CrtEnvInd, i);

  /* Check if the group is alredy running */    /*E0086*/

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.004: wrong call insred_\n"
           "(the reduction group has already been started; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  /* Check if reduction variable is included
            into some reduction group        */    /*E0087*/

  if(RVar->RG && RVar->RG != RG)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 070.007: wrong call insred_\n"
         "(the reduction variable has already been inserted "
         "in the reduction group;\nRedRef=%lx; RedGroupRef=%lx)\n",
         *RedRefPtr, (uLLng)RVar->RG->HandlePtr);

  /* Check processor space specifier
          of reduction variable      */    /*E0088*/

  if(PSSpaceRefPtr == NULL || *PSSpaceRefPtr == 0)
  {  ParHandlePtr = NULL;
     RVar->AMView = NULL;
     PSSpaceVMS   = DVM_VMS;

     if(RTL_TRACE)
        dvm_trace(call_insred_,
                  "RedGroupRefPtr=%lx; RedGroupRef=%lx; "
                  "RedRefPtr=%lx; RedRef=%lx; "
                  "PSSpaceRefPtr=NULL; PSSpaceRef=0; RenewSign=%d;\n",
                  (uLLng)RedGroupRefPtr, *RedGroupRefPtr,
                  (uLLng)RedRefPtr, *RedRefPtr, RenewSign);
  }
  else
  {  ParHandlePtr = (SysHandle *)*PSSpaceRefPtr;

     switch(ParHandlePtr->Type)
     {  case sht_ParLoop:
        /***************/    /*E0089*/

        if(RTL_TRACE)
           dvm_trace(call_insred_,
                     "RedGroupRefPtr=%lx; RedGroupRef=%lx; "
                     "RedRefPtr=%lx; RedRef=%lx; "
                     "LoopRefPtr=%lx; LoopRef=%lx; RenewSign=%d;\n",
                     (uLLng)RedGroupRefPtr, *RedGroupRefPtr,
                     (uLLng)RedRefPtr, *RedRefPtr,
                     (uLLng)PSSpaceRefPtr, *PSSpaceRefPtr, RenewSign);

        if(TstObject)
        { PL=(coll_At(s_ENVIRONMENT *, gEnvColl,
                      gEnvColl->Count-1))->ParLoop; /* current
                                                       parallel loop */    /*E0090*/

          if(PL != (s_PARLOOP *)ParHandlePtr->pP)
             epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 070.008: wrong call insred_\n"
                      "(the given parallel loop is not the current "
                      "parallel loop;\nLoopRef=%lx)\n",
                      *PSSpaceRefPtr);
        }

        PL = (s_PARLOOP *)ParHandlePtr->pP;

        /* Check if parallel loop is mapped */    /*E0091*/

        if(PL->AMView == NULL && PL->Empty == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 070.009: wrong call insred_\n"
                    "(the parallel loop has not been mapped; "
                    "LoopRef=%lx)\n", *PSSpaceRefPtr);

        RVar->AMView = NULL;

        if(PL->AMView)
           PSSpaceVMS = PL->AMView->VMS;
        else
           PSSpaceVMS = DVM_VMS;

        SpaceAxis = PL->PLAxis; /* */    /*E0093*/
        AMV = PL->AMView ? PL->AMView : 0;
        AlignRules = PL->Align ? PL->Align + PL->Rank : 0;

        break;

        case sht_AMView:
        /**************/    /*E0094*/

        if(RTL_TRACE)
           dvm_trace(call_insred_,
                     "RedGroupRefPtr=%lx; RedGroupRef=%lx; "
                     "RedRefPtr=%lx; RedRef=%lx; "
                     "AMViewRefPtr=%lx; AMViewRef=%lx; RenewSign=%d;\n",
                     (uLLng)RedGroupRefPtr, *RedGroupRefPtr,
                     (uLLng)RedRefPtr, *RedRefPtr,
                     (uLLng)PSSpaceRefPtr, *PSSpaceRefPtr, RenewSign);

        if(TstObject)
        {  if(TstDVMObj(PSSpaceRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 070.010: wrong call insred_\n"
                       "(the abstract machine representation is not "
                       "a DVM object; AMViewRef=%lx)\n",
                       (uLLng)ParHandlePtr);
        }

        AMV = (s_AMVIEW *)ParHandlePtr->pP;

        if(AMV->VMS) /*  whether the representation
                        is mapped by distr_ function */    /*E0095*/
        {  PSSpaceVMS   = AMV->VMS;
           RVar->AMView = NULL;
        }
        else
        {  /* The representation is mapped by distr_ function */    /*E0096*/
           /* */    /*E0097*/

           PSSpaceVMS    = DVM_VMS;
           RVar->AMView  = AMV;
           RG->TskRDSign = 1; /* */    /*E0098*/

           /* */    /*E0099*/

           if((RG->PSSpaceVMS != NULL && RG->PSSpaceVMS != DVM_VMS) ||
              RG->CrtPSSign)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 070.011: wrong call insred_\n"
                       "(the processor system of the reduction group "
                       "is not the current processor system;\n"
                       "RedGroupRef=%lx; RedRef=%lx; "
                       "RGPSSpaceRef=%lx; CurrentPSRef=%lx\n",
                       *RedGroupRefPtr, *RedRefPtr,
                       (uLLng)RG->PSSpaceVMS, (uLLng)DVM_VMS->HandlePtr);

           if(RG->DA != NULL)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 070.012: wrong call insred_\n"
                 "(the reduction group contains "
                 "a reduction distributed array;\n"
                 "RedGroupRef=%lx; RedRef=%lx; RedArrayHeader[0]=%lx\n",
                 *RedGroupRefPtr, *RedRefPtr, (uLLng)RG->DA->HandlePtr);

           /* Check if all representation mapped abstract
              machines are mapped on the current processor
                        system or its subsystem            */    /*E0100*/

           VarSize = AMV->AMSColl.Count;

           for(i=0; i < VarSize; i++) /* loop on examined
                                                    abstract machines */    /*E0101*/
           {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

              if(AMS->VMS == NULL)
                 continue;  /* abstract machine is not mapped */    /*E0102*/

              NotSubsystem(VarSize, DVM_VMS, AMS->VMS)

              if(VarSize)
                 epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                      "*** RTS err 070.013: wrong call insred_\n"
                      "(the processor system of the abstract machine "
                      "is not a subsystem of the current processor "
                      "system;\nAMViewRef=%lx; AMRef=%lx; AMPSRef=%lx; "
                      "CurrentPSRef=%lx)\n", (uLLng)ParHandlePtr,
                      (uLLng)AMS->HandlePtr, (uLLng)AMS->VMS->HandlePtr,
                      (uLLng)DVM_VMS->HandlePtr);
           }
        }

        if (AMV->VMS)
            SpaceAxis = AMV->AMVAxis;

        break;

        case sht_DisArray:
        /****************/    /*E0103*/

        if(RTL_TRACE)
           dvm_trace(call_insred_,
              "RedGroupRefPtr=%lx; RedGroupRef=%lx; "
              "RedRefPtr=%lx; RedRef=%lx; "
              "ArrayHeader=%lx; ArrayHandlePtr=%lx; RenewSign=%d;\n",
              (uLLng)RedGroupRefPtr, *RedGroupRefPtr,
              (uLLng)RedRefPtr, *RedRefPtr,
              (uLLng)PSSpaceRefPtr, *PSSpaceRefPtr, RenewSign);

        ParHandlePtr = TstDVMArray((void *)PSSpaceRefPtr);

        if(ParHandlePtr == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 070.014: wrong call insred_\n"
                    "(the object is not a distributed array; "
                    "ArrayHeader[0]=%lx)\n", (uLLng)*PSSpaceRefPtr);

        DArr   = (s_DISARRAY *)ParHandlePtr->pP;

        /* Check if the processor space specification array is mapped */    /*E0104*/

        if(DArr->AMView == NULL)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 070.015: wrong call insred_\n"
                    "(the array has not been aligned; "
                    "ArrayHeader[0]=%lx)\n", (uLLng)ParHandlePtr);

        PSSpaceVMS   = DArr->AMView->VMS;
        RVar->AMView = NULL;

        SpaceAxis = DArr->DAAxis; /* */    /*E0105*/
        AMV = DArr->AMView;
        AlignRules = DArr->Align + DArr->Space.Rank;

        break;

        case sht_VMS:
        /***********/    /*E0106*/

        if(RTL_TRACE)
           dvm_trace(call_insred_,
                     "RedGroupRefPtr=%lx; RedGroupRef=%lx; "
                     "RedRefPtr=%lx; RedRef=%lx; "
                     "PSRefPtr=%lx; PSRef=%lx; RenewSign=%d;\n",
                     (uLLng)RedGroupRefPtr, *RedGroupRefPtr,
                     (uLLng)RedRefPtr, *RedRefPtr,
                     (uLLng)PSSpaceRefPtr, *PSSpaceRefPtr, RenewSign);

        if(TstObject)
        {  if(TstDVMObj(PSSpaceRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 070.016: wrong call insred_\n"
                       "(the processor system is not a DVM object; "
                       "PSRef=%lx)\n", (uLLng)ParHandlePtr);
        }

        PSSpaceVMS   = (s_VMS *)ParHandlePtr->pP;
        RVar->AMView = NULL;

        break;

        default:
        /******/    /*E0107*/

        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 070.026: wrong call insred_\n"
                 "(invalid specification of processor space; "
                 "PSSpaceRef=%lx\n", (uLLng)ParHandlePtr);
     }
  }

  /* Check if all elements of the processor system
     from processor scape specifier are included
            into current processor system          */    /*E0108*/

  NotSubsystem(i, DVM_VMS, PSSpaceVMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 070.027: wrong call insred_\n"
              "(the processor system of the reduction variable "
              "is not a subsystem of the current processor system;\n"
              "PSSpaceRef=%lx; RedVarPSRef=%lx; CurrentPSRef=%lx\n",
              (uLLng)ParHandlePtr, (uLLng)PSSpaceVMS->HandlePtr,
              (uLLng)DVM_VMS->HandlePtr);

  /* */    /*E0109*/

  if(RG->PSSpaceVMS && RG->PSSpaceVMS != PSSpaceVMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.028: wrong call insred_\n"
            "(the processor system of the current reduction variable "
            "is not equal to the processor system of the first "
            "reduction variable;\n"
            "PSSpaceRef=%lx; CurrentRedVarPSRef=%lx; "
            "FirstRedVarPSRef=%lx\n",
            (uLLng)ParHandlePtr, (uLLng)PSSpaceVMS->HandlePtr,
            (uLLng)RG->PSSpaceVMS->HandlePtr);

  /* */    /*E0110*/

  if(RG->VMS != NULL)
  {  if(RG->CrtVMSSign != 0)
     {  RGPSRef = (PSRef)RG->VMS->HandlePtr;
        ( RTL_CALL, delps_(&RGPSRef) );
     }

     RG->CrtVMSSign = 0;
     RG->VMS = NULL;
  }

  /* */    /*E0111*/

  VarSize = PSSpaceVMS->Space.Rank;  /* */    /*E0112*/

  LocSize = 0; /* */    /*E0113*/

    for (i = 0; i < VarSize; i++) {
        InitIndexArray[i] = 0;
        LastIndexArray[i] = PSSpaceVMS->Space.Size[i] - 1;
        if (SpaceAxis != NULL && SpaceAxis[i] == 0) {
            DvmType replCount = 0;
            DvmType myReplIdx = -1;
            DvmType myIdx = PSSpaceVMS->CVP[i + 1];
            DvmType myEmptyIdx = myIdx;
            int j;
            for (j = 0; j < PSSpaceVMS->Space.Size[i]; j++) {
                if (haveElements(PSSpaceVMS, i, j, AMV, AlignRules)) {
                    replCount++;
                    if (j == myIdx)
                        myReplIdx = j;
                    if (j < myIdx)
                        myEmptyIdx--;
                }
            }
            if (replCount > 1) {
                LocSize = 1;
                if (myReplIdx < 0) {
                    DvmType emptyCount = PSSpaceVMS->Space.Size[i] - replCount;
                    DvmType minBlock = emptyCount / replCount;
                    DvmType elemsToAdd = emptyCount % replCount;
                    DvmType curRemainder = 0;
                    DvmType dispatched = 0;
                    myReplIdx = 0;
                    for (j = 0; j < PSSpaceVMS->Space.Size[i]; j++) {
                        if (haveElements(PSSpaceVMS, i, j, AMV, AlignRules)) {
                            DvmType curBlock;
                            curRemainder = (curRemainder + elemsToAdd) % replCount;
                            curBlock = minBlock + (curRemainder < elemsToAdd);
                            if (myEmptyIdx < dispatched + curBlock)
                                break;
                            dispatched += curBlock;
                            myReplIdx++;
                        }
                    }
                }
                InitIndexArray[i] = myReplIdx;
                LastIndexArray[i] = myReplIdx;
            }
        }
    }

  /* */    /*E0118*/

  if(RG->PSSpaceVMS == NULL)
  {  /* */    /*E0119*/

     RG->PSSpaceVMS = PSSpaceVMS;
     RG->CrtPSSign  = LocSize;

     for(i=0; i < VarSize; i++)
     {  RG->SpaceInitIndex[i] = InitIndexArray[i];
        RG->SpaceLastIndex[i] = LastIndexArray[i];
     }
  }
  else
  {  /* */    /*E0120*/

     RG->CrtPSSign  = RG->CrtPSSign || LocSize;

     /* XXX: Why intersection? */
     for(i=0; i < VarSize; i++)
     {  RG->SpaceInitIndex[i] = dvm_max(RG->SpaceInitIndex[i],
                                        InitIndexArray[i]);
        RG->SpaceLastIndex[i] = dvm_min(RG->SpaceLastIndex[i],
                                        LastIndexArray[i]);
     }
  }

  /* */    /*E0121*/

  if(RG->TskRDSign && (PSSpaceVMS != DVM_VMS || LocSize))
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 070.029: wrong call insred_\n"
              "(the processor system of the reduction variable "
              "is not the current processor system;\n"
              "RedGroupRef=%lx; RedRef=%lx; "
              "PSSpaceRef=%lx; CurrentPSRef=%lx\n",
              *RedGroupRefPtr, *RedRefPtr,
              (uLLng)PSSpaceVMS, (uLLng)DVM_VMS->HandlePtr);


  /* -------------------------------------------------------- */    /*E0122*/

  if(RVar->RedDArr != NULL)
  {  /* */    /*E0123*/

     DArrVMS = RVar->RedDArr->AMView->VMS; /* */    /*E0124*/
     VarSize = DArrVMS->Space.Rank;

     /* */    /*E0125*/

     if(RG->PSSpaceVMS != DArrVMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.020: wrong call insred_\n"
            "(the processor system of the reduction array "
            "is not equal to the processor system of the first "
            "reduction variable;\n"
            "RedArrayPSRef=%lx; FirstRedVarPSRef=%lx\n",
            (uLLng)DArrVMS->HandlePtr, (uLLng)RG->PSSpaceVMS->HandlePtr);

     /* */    /*E0126*/

     if(RG->TskRDSign)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 070.021: wrong call insred_\n"
                 "(the reduction group contains "
                 "a task reduction variable;\n"
                 "RedGroupRef=%lx; RedRef=%lx; RedArrayHeader[0]=%lx\n",
                 *RedGroupRefPtr, *RedRefPtr,
                 (uLLng)RVar->RedDArr->HandlePtr);

     if(DArrVMS->HasCurrent)
     {  /* */    /*E0127*/

        for(i=0; i < VarSize; i++)
        {  if(RVar->DAAxisPtr[i] == 0)
           {  /* */    /*E0128*/

              InitIndexArray[i] = 0;
              LastIndexArray[i] = DArrVMS->Space.Size[i] - 1;
           }
           else
           {  /* */    /*E0129*/

              InitIndexArray[i] = DArrVMS->CVP[i+1];
              LastIndexArray[i] = DArrVMS->CVP[i+1];
           }
        }

        for(i=0; i < VarSize; i++)
        {  RG->SpaceInitIndex[i] = dvm_max(RG->SpaceInitIndex[i],
                                           InitIndexArray[i]);
           RG->SpaceLastIndex[i] = dvm_min(RG->SpaceLastIndex[i],
                                           LastIndexArray[i]);
        }
     }

     if(RG->DA == NULL)
     {  /* */    /*E0130*/

        RG->DA = RVar->RedDArr; /* */    /*E0131*/

        RG->DAAxisPtr = RVar->DAAxisPtr;
     }
     else
     {  /* */    /*E0132*/

        /* */    /*E0133*/

        for(i=0; i < VarSize; i++)
            if( (RVar->DAAxisPtr[i] == 0 && RG->DAAxisPtr[i] != 0) ||
                (RVar->DAAxisPtr[i] != 0 && RG->DAAxisPtr[i] == 0) )
                break;

        if(i != VarSize)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                "*** RTS err 070.023: wrong call insred_\n"
                "(replication dimensions of the current reduction "
                "array PS are not equal to replication dimensions "
                "of the first reduction array PS;\n"
                "RedGroupRef=%lx; RedRef=%lx;\n"
                "RedArrayHeader[0]=%lx; RedArrayPSRef=%lx; "
                "FirstRedArrayHeader[0]=%lx; FirstRedArrayPSRef=%lx)\n",
                *RedGroupRefPtr, *RedRefPtr,
                (uLLng)RVar->RedDArr->HandlePtr, (uLLng)DArrVMS->HandlePtr,
                (uLLng)RG->DA->HandlePtr,
                (uLLng)RG->PSSpaceVMS->HandlePtr);

        /* */    /*E0134*/

        if(RVar->RedDArr->HasLocal != 0 && RG->DA->HasLocal == 0)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                "*** RTS err 070.024: wrong call insred_\n"
                "(the current reduction array has been mapped onto "
                "the current processor, but the first "
                "reduction array has "
                "not been mapped onto the current processor;\n"
                "RedGroupRef=%lx; RedRef=%lx;\n"
                "RedArrayHeader[0]=%lx; RedArrayPSRef=%lx; "
                "FirstRedArrayHeader[0]=%lx; FirstRedArrayPSRef=%lx)\n",
                *RedGroupRefPtr, *RedRefPtr,
                (uLLng)RVar->RedDArr->HandlePtr, (uLLng)DArrVMS->HandlePtr,
                (uLLng)RG->DA->HandlePtr,
                (uLLng)RG->PSSpaceVMS->HandlePtr);

        if(RVar->RedDArr->HasLocal == 0 && RG->DA->HasLocal != 0)
           epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                "*** RTS err 070.025: wrong call insred_\n"
                "(the first reduction array has been mapped onto "
                "the current processor, but the current "
                "reduction array has "
                "not been mapped onto the current processor;\n"
                "RedGroupRef=%lx; RedRef=%lx;\n"
                "RedArrayHeader[0]=%lx; RedArrayPSRef=%lx; "
                "FirstRedArrayHeader[0]=%lx; FirstRedArrayPSRef=%lx)\n",
                *RedGroupRefPtr, *RedRefPtr,
                (uLLng)RVar->RedDArr->HandlePtr, (uLLng)DArrVMS->HandlePtr,
                (uLLng)RG->DA->HandlePtr,
                (uLLng)RG->PSSpaceVMS->HandlePtr);
     }
  }

  if(RVar->RG == NULL)
  {  /* Reduction variable is not included into the group */    /*E0135*/

     RVar->VarInd = RG->RV.Count; /* number of reduction variable
                                     group variable list */    /*E0136*/

     if(RG->RV.Count == 0)
     {  /* Reduction group is empty */    /*E0137*/

        VarSize = (int)RG->PSSpaceVMS->ProcCount;

        dvm_AllocArray(RTL_Request, VarSize, RG->Req);
        dvm_AllocArray(int, VarSize, RG->Flag);
        dvm_AllocArray(char *, VarSize, RG->NoWaitBufferPtr);

        for(i=0; i < VarSize; i++)
        {  RG->NoWaitBufferPtr[i] = NULL;
           RG->Flag[i] = 1;  /* flag: waiting completion
                                   of receiveing by central has been
                                   performed already */    /*E0138*/
           RG->Req[i].BufAddr = NULL;
        }

        if(RVar->BlockSize > 0)
        {  mac_malloc(RG->NoWaitBufferPtr[0], char *,
                      RVar->BlockSize, 0);

           RVar->BufAddr = RG->NoWaitBufferPtr[0];/* included variable
                                                     address in buffer */    /*E0139*/
        }

        RG->BlockSize = RVar->BlockSize; /* */    /*E0140*/
     }
     else
     {  /* Reduction group is not empty */    /*E0141*/

        LocSize = RG->BlockSize + RVar->BlockSize;

        if(RVar->BlockSize > 0)
        {  if(RG->BlockSize > 0)
           {  mac_realloc(RG->NoWaitBufferPtr[0], char *,
                          RG->NoWaitBufferPtr[0], LocSize, 0);
           }
           else
           {  mac_malloc(RG->NoWaitBufferPtr[0], char *, LocSize, 0);
           }

           RVar->BufAddr = RG->NoWaitBufferPtr[0] + RG->BlockSize;
        }

        RG->BlockSize = LocSize;    /* current total length
                                           of group variables */    /*E0142*/
     }

     /* Include variable into the group */    /*E0143*/

     coll_Insert(&RG->RV, RVar); /* add to the list of variables
                                    included into the group */    /*E0144*/
  }

  /*   Store variable valyue in buffer
     together with additional information */    /*E0145*/

  if(RVar->RG == NULL || RenewSign)
  {  VarSize = RVar->VLength * RVar->RedElmLength; /* reduction
                                                      variable-array
                                                      length */    /*E0146*/
     LocSize = RVar->VLength * RVar->LocElmLength; /* additional info
                                                      length */    /*E0147*/
     if(VarSize > 0)
        dvm_memcopy(RVar->BufAddr, RVar->Mem, VarSize);

     if(LocSize > 0)
        dvm_memcopy(RVar->BufAddr + VarSize, RVar->LocMem, LocSize);
  }

  /* --------------------------------------- */    /*E0148*/

  RVar->RG = RG;               /* fix reduction group
                                  for reduction variable */    /*E0149*/

  if(RTL_TRACE)
  {  if(RedVarTrace && TstTraceEvent(call_insred_))
        PrintRedVars(&RG->RV, RVar->VarInd); /* print reduction
                                                variable */    /*E0150*/
  }

DynDyn:

  if (EnableDynControl)
  {
      char* VarPtr = RVar->Mem;
      char* LocPtr = RVar->LocMem;
      int nOffset = 0;
      int nSize = sizeof(long);

      switch (RVar->LocIndType)
      {
          case 0 : nSize = sizeof(long); break;
          case 1 : nSize = sizeof(int); break;
          case 2 : nSize = sizeof(short); break;
          case 3 : nSize = sizeof(char); break;
      }

      for (i = 0; i < RVar->VLength; i++)
      {
          dyn_DefineReduct(RVar->Static, VarPtr);
          VarPtr += RVar->RedElmLength;

          if (LocPtr)
          {
              for (nOffset = 0; nOffset < RVar->LocElmLength;
                   nOffset += nSize)
                   dyn_DefineReduct(RVar->Static, LocPtr + nOffset);

              LocPtr += RVar->LocElmLength;
          }
      }
  }

  if(RgSave && RG->VMS != NULL)
  {  ( RTL_CALL, saverg_(RedGroupRefPtr) );
     return  (DVM_RET, 0);
  }

  if(RTL_TRACE)
     dvm_trace(ret_insred_," \n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0151*/
  DVMFTimeFinish(ret_insred_);
  return  (DVM_RET, 0);
}



DvmType  __callstd saverg_(RedGroupRef  *RedGroupRefPtr)

/*
      Storing values of reduction variables.
      --------------------------------------

*RedGroupRefPtr	- reference to the reduction group.

The function stores initial values of all variables of the reduction
group  to use them for computation of results of the group reduction
operation.
The function returns zero.
*/    /*E0152*/

{ SysHandle       *RGHandlePtr;
  s_REDGROUP      *RG;
  int              i, j, nSize, nOffset;
  void            *CurrAM;
  char            *VarPtr, *LocPtr;
  s_REDVAR        *RVar;

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0153*/
  DVMFTimeStart(call_saverg_);

  if(RTL_TRACE)
     dvm_trace(call_saverg_,"RedGroupRefPtr=%lx; RedGroupRef=%lx;\n",
                            (uLLng)RedGroupRefPtr, *RedGroupRefPtr);

  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.030: wrong call saverg_\n"
            "(the reduction group is not a DVM object; "
            "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 070.031: wrong call saverg_\n"
          "(the object is not a reduction group; RedGroupRef=%lx)\n",
          *RedGroupRefPtr);

  RG  = (s_REDGROUP *)RGHandlePtr->pP;

  /* Check if reduction group is created in current subtask */    /*E0154*/

  i = gEnvColl->Count - 1;          /* current context index */    /*E0155*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM  */    /*E0156*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.032: wrong call saverg_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, i);

  /* Check if the group is running */    /*E0157*/

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.033: wrong call saverg_\n"
           "(the reduction group has already been started; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  /* Check if there is at least one variable in the group */    /*E0158*/

  if(RG->RV.Count == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.034: wrong call saverg_\n"
           "(the reduction group is empty; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  if(RTL_TRACE)
  {  if(RedVarTrace && TstTraceEvent(call_saverg_))
        PrintRedVars(&RG->RV, -1); /* print reduction variables */    /*E0159*/
  }

  CopyRedVars2Buffer(&RG->RV, RG->NoWaitBufferPtr[0]);

  if(EnableDynControl && RgSave == 0)
  {  nSize = sizeof(long);

     for(j=0; j < RG->RV.Count; j++)
     {  RVar    = coll_At(s_REDVAR *, &RG->RV, j);

        VarPtr = RVar->Mem;
        LocPtr = RVar->LocMem;

        switch (RVar->LocIndType)
        {
           case 0 : nSize = sizeof(long); break;
           case 1 : nSize = sizeof(int); break;
           case 2 : nSize = sizeof(short); break;
           case 3 : nSize = sizeof(char); break;
        }

        for(i=0; i < RVar->VLength; i++)
        {
           dyn_DefineReduct(RVar->Static, VarPtr);
           VarPtr += RVar->RedElmLength;

           if(LocPtr)
           {
              for (nOffset = 0; nOffset < RVar->LocElmLength;
                   nOffset += nSize)
                   dyn_DefineReduct(RVar->Static, LocPtr + nOffset);

              LocPtr += RVar->LocElmLength;
           }
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_saverg_," \n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0160*/
  DVMFTimeFinish(ret_saverg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd saverv_(RedRef  *RedRefPtr)

/*
      Storing value of the reduction variable.
      ----------------------------------------

*RedRefPtr - reference to the reduction variable.

The function stores the initial value of the variable to use this
for computation of results of the group reduction operation.
The function returns zero.
*/    /*E0161*/

{ SysHandle       *RVHandlePtr;
  s_REDVAR        *RV;
  s_REDGROUP      *RG;
  int              i, VarSize, LocSize, nSize, nOffset;
  void            *CurrAM;
  char            *VarPtr, *LocPtr;

  StatObjectRef = (ObjectRef)*RedRefPtr; /* for statistics */    /*E0162*/
  DVMFTimeStart(call_saverv_);

  if(RTL_TRACE)
     dvm_trace(call_saverv_,"RedRefPtr=%lx; RedRef=%lx;\n",
                            (uLLng)RedRefPtr, *RedRefPtr);

  RVHandlePtr = (SysHandle *)*RedRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.040: wrong call saverv_\n"
            "(the reduction variable is not a DVM object; "
            "RedRef=%lx)\n", *RedRefPtr);
  }

  if(RVHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 070.041: wrong call saverv_\n"
          "(the object is not a reduction variable; RedRef=%lx)\n",
          *RedRefPtr);

  RV  = (s_REDVAR *)RVHandlePtr->pP;

  /* Check if reduction variable is created in current subtask */    /*E0163*/

  i = gEnvColl->Count - 1;          /* current context index */    /*E0164*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0165*/

  if(RVHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.042: wrong call saverv_\n"
        "(the reduction variable was not created by the current "
        "subtask;\nRedRef=%lx; RedEnvIndex=%d; CurrentEnvIndex=%d)\n",
        *RedRefPtr, RVHandlePtr->CrtEnvInd, i);

  /* Check if variable is included in some group */    /*E0166*/

  if(RV->RG == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.043: wrong call saverv_\n"
            "(the reduction variable has not been inserted "
            "in a reduction group; "
            "RedRef=%lx)\n", *RedRefPtr);

  /* Check if group, the variable belongs to, is running */    /*E0167*/

  RG = RV->RG;

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.044: wrong call saverv_\n"
           "(the reduction group of the variable has already "
           "been started;\nRedRef=%lx; RedGroupRef=%lx)\n",
           *RedRefPtr, (uLLng)RG->HandlePtr);

  /*    Store variable value in buffer
     together with additional information */    /*E0168*/

  VarSize = RV->VLength * RV->RedElmLength; /* reduction variable-array
                                               length */    /*E0169*/
  LocSize = RV->VLength * RV->LocElmLength; /* adiitional info length */    /*E0170*/
  if(VarSize > 0)
     dvm_memcopy(RV->BufAddr, RV->Mem, VarSize);

  if(LocSize > 0)
     dvm_memcopy(RV->BufAddr + VarSize, RV->LocMem, LocSize);

  /* --------------------------------------- */    /*E0171*/

  if(RTL_TRACE)
  {  if(RedVarTrace && TstTraceEvent(call_saverv_))
        PrintRedVars(&RG->RV, RV->VarInd); /* print reduction
                                              variable */    /*E0172*/
  }

  if(EnableDynControl)
  {  nSize = sizeof(long);

     VarPtr = RV->Mem;
     LocPtr = RV->LocMem;

     switch (RV->LocIndType)
     {
        case 0 : nSize = sizeof(long);  break;
        case 1 : nSize = sizeof(int);   break;
        case 2 : nSize = sizeof(short); break;
        case 3 : nSize = sizeof(char);  break;
     }

     for(i=0; i < RV->VLength; i++)
     {
        dyn_DefineReduct(RV->Static, VarPtr);
        VarPtr += RV->RedElmLength;

        if(LocPtr)
        {
           for (nOffset = 0; nOffset < RV->LocElmLength;
                nOffset += nSize)
                dyn_DefineReduct(RV->Static, LocPtr + nOffset);

           LocPtr += RV->LocElmLength;
        }
     }
  }

  if(RTL_TRACE)
     dvm_trace(ret_saverv_," \n");

  StatObjectRef = (ObjectRef)*RedRefPtr; /* for statistics */    /*E0173*/
  DVMFTimeFinish(ret_saverv_);
  return  (DVM_RET, 0);
}



DvmType  __callstd strtrd_(RedGroupRef  *RedGroupRefPtr)

/*
      Starting reduction group.
      -------------------------

*RedGroupRefPtr	- reference to the reduction group.

The function starts all reduction operations over all reduction
variables of the group.
The function returns zero.
*/    /*E0174*/

{ SysHandle      *RGHandlePtr;
  s_VMS          *VMS, *wVMS;
  s_REDGROUP     *RG;
  int             i, Central, j, Rank;
  void           *CurrAM;
  PSRef           RGPSRef;
  DvmType            StaticSign = 1, tlong;

  if(StrtRedSynchr)
     (RTL_CALL, bsynch_()); /* */    /*E0175*/

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0176*/
  DVMFTimeStart(call_strtrd_);

  /* Forward to the next element of message tag circle
        tag_RedVar for the current processor system    */    /*E0177*/

  DVM_VMS->tag_RedVar++;

  if((DVM_VMS->tag_RedVar - (msg_RedVar)) >= TagCount)
     DVM_VMS->tag_RedVar = msg_RedVar;

  /* ----------------------------------------------- */    /*E0178*/

  if(RTL_TRACE)
     dvm_trace(call_strtrd_,"RedGroupRefPtr=%lx; RedGroupRef=%lx;\n",
                            (uLLng)RedGroupRefPtr, *RedGroupRefPtr);

  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.050: wrong call strtrd_\n"
            "(the reduction group is not a DVM object; "
            "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 070.051: wrong call strtrd_\n"
          "(the object is not a reduction group; RedGroupRef=%lx)\n",
          *RedGroupRefPtr);

  RG = (s_REDGROUP *)RGHandlePtr->pP;

  /* Check if reduction group is created in current subtask */    /*E0179*/

  i = gEnvColl->Count - 1;          /* current context index */    /*E0180*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0181*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.052: wrong call strtrd_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, i);

  /* Check if group is alredy running */    /*E0182*/

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.053: wrong call strtrd_\n"
           "(the reduction group has already been started; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  /* Check if there is atleast one variable in the group */    /*E0183*/

  if(RG->RV.Count == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.054: wrong call strtrd_\n"
           "(the reduction group is empty; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  if(RG->VMS == NULL)
  {  /* */    /*E0184*/

     if(RG->DA != NULL || RG->CrtPSSign)
     {  /* */    /*E0185*/

        /* */    /*E0186*/

        VMS = RG->PSSpaceVMS;
        Central = VMS->RedSubSystem.Count;
        Rank = VMS->Space.Rank;

        for(i=0; i < Central; i++)
        {  wVMS = coll_At(s_VMS *, &VMS->RedSubSystem, i);

           for(j=0; j < Rank; j++)
           {  if(RG->SpaceInitIndex[j] != wVMS->InitIndex[j])
                 break;
              tlong = wVMS->Space.Size[j] + wVMS->InitIndex[j] - 1;
              if(RG->SpaceLastIndex[j] != tlong)
                 break;
           }

           if(j == Rank)
              break;    /* */    /*E0187*/
        }

        if(i == Central)
        {  /* */    /*E0188*/

           RGPSRef = (PSRef)VMS->HandlePtr;

           RGPSRef = (RTL_CALL, crtps_(&RGPSRef, RG->SpaceInitIndex, RG->SpaceLastIndex, &StaticSign));
           RG->VMS = (s_VMS *)((SysHandle *)RGPSRef)->pP;
        }
        else
        {  /* */    /*E0189*/

           RG->VMS = wVMS;
           coll_AtDelete(&VMS->RedSubSystem, i);
        }

        RG->CrtVMSSign = 1; /* */    /*E0190*/
     }
     else
        RG->VMS = RG->PSSpaceVMS; /* */    /*E0191*/
  }

  VMS = RG->VMS; /* reduction group processor system */    /*E0192*/

  if(RTL_TRACE)
  { if(RedVarTrace && TstTraceEvent(call_strtrd_))
       PrintRedVars(&RG->RV, -1); /* print reduction variables */    /*E0193*/
  }

  RG->StrtFlag = 1;               /* flag of started group */    /*E0194*/

  /* */    /*E0195*/

  i = -1;

  #ifdef _DVM_MPI_
     if(MPIReduce)
        i = DoNotMPIRed(RG);

     if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
        tprintf("*** DoNotMPIRed = %d\n", i);
     if(MPIReducePrint && _SysInfoPrint && MPS_CurrentProc == DVM_IOProc)
        rtl_iprintf("\n*** RTS: DoNotMPIRed = %d\n", i);
  #endif

  if(i != 0)
  { coll_Insert(gRedGroupColl, RG); /* into the list of started groups */    /*E0196*/

    RG->MessageCount = MessageCount[16]; /* the number of received
                                            messages */    /*E0197*/

    /* -------------------------------------------------- */    /*E0198*/

    for(i=0; i < VMS->ProcCount; i++)
    {  RG->Flag[i] = 1;  /* flag: waiting completion
                            by central has been already performed */    /*E0199*/
       RG->Req[i].BufAddr = NULL;
    }

    if(RG->IsBuffers && RG->IsNewVars)
    {  /* */    /*E0200*/

       if(RG->BlockSize > 0)
          for(i=1; i < VMS->ProcCount; i++)
          {   mac_free(&RG->NoWaitBufferPtr[i]);
          }

       RG->IsBuffers = 0;
    }

    if(RG->IsBuffers == 0)
    {  /* */    /*E0201*/

       if(RG->BlockSize > 0)
          for(i=1; i < VMS->ProcCount; i++)
          {   mac_malloc(RG->NoWaitBufferPtr[i], char *,
                         RG->BlockSize, 0);
          }
       else
          for(i=1; i < VMS->ProcCount; i++)
              RG->NoWaitBufferPtr[i] = NULL;
    }

    RG->IsBuffers = 1;  /* */    /*E0202*/
    RG->IsNewVars = 0;  /* */    /*E0203*/

    if(VMS->HasCurrent) /* whether current processor
                           belongs to reduction group processor system */    /*E0204*/
    {
       if( EnableDynControl )
           dyn_AReductSetState( &RG->RV, dar_STARTED );


       Central = VMS->CentralProc; /* internal number
                                      of central processor */    /*E0205*/

       if(MPS_CurrentProc == Central)
       {  for(i=0; i < VMS->ProcCount; i++)
          {  if(VMS->VProc[i].lP != MPS_CurrentProc)
             {  RG->Flag[i] = 0; /* flag: waiting completion
                                    by central has not performed */    /*E0206*/
                if(RTL_TRACE)
                   ( RTL_CALL, red_Recvnowait(RG->NoWaitBufferPtr[i], 1,
                                              RG->BlockSize,
                                              (int)VMS->VProc[i].lP,
                                              DVM_VMS->tag_RedVar,
                                              &RG->Req[i]) );
                else
                   ( RTL_CALL, rtl_Recvnowait(RG->NoWaitBufferPtr[i], 1,
                                              RG->BlockSize,
                                              (int)VMS->VProc[i].lP,
                                              DVM_VMS->tag_RedVar,
                                              &RG->Req[i], 0) );
             }
             else
             {  RG->Flag[i] = 1;
                CopyRedVars2Buffer(&RG->RV, RG->NoWaitBufferPtr[i]);
                SetRedVars(&RG->RV);
             }
          }
       }
       else
       {  CorrectRedVars(&RG->RV, RG->NoWaitBufferPtr[0]);

          if(RTL_TRACE)
          {  ( RTL_CALL, red_Sendnowait(RG->NoWaitBufferPtr[0], 1,
                                        RG->BlockSize, Central,
                                        DVM_VMS->tag_RedVar,
                                        &RG->Req[0], RedNonCentral) );
             ( RTL_CALL, red_Recvnowait(RG->NoWaitBufferPtr[1], 1,
                                        RG->BlockSize, Central,
                                        DVM_VMS->tag_RedVar,
                                        &RG->Req[1]) );
          }
          else
          {  ( RTL_CALL, rtl_Sendnowait(RG->NoWaitBufferPtr[0], 1,
                                        RG->BlockSize, Central,
                                        DVM_VMS->tag_RedVar,
                                        &RG->Req[0], RedNonCentral) );
             ( RTL_CALL, rtl_Recvnowait(RG->NoWaitBufferPtr[1], 1,
                                        RG->BlockSize, Central,
                                        DVM_VMS->tag_RedVar,
                                        &RG->Req[1], 0) );
          }
       }
    }
  }
  else
  { /* */    /*E0207*/

    if(VMS->HasCurrent) /* */    /*E0208*/
    {
       if( EnableDynControl )
           dyn_AReductSetState( &RG->RV, dar_STARTED );
    }
  }

  if(RTL_TRACE)
     dvm_trace(ret_strtrd_," \n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0209*/
  DVMFTimeFinish(ret_strtrd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd waitrd_(RedGroupRef  *RedGroupRefPtr)

/*
      Waiting for completion of reduction group.
      ------------------------------------------

*RedGroupRefPtr	- reference to the reduction group.

The function awaits the completion of  the all reduction operations
of the group.
The function returns zero.
*/    /*E0210*/

{ SysHandle      *RGHandlePtr;
  s_VMS          *VMS;
  int             Central, Proc, i, j, n;
  s_REDGROUP     *RG;
  void           *CurrAM;
  s_REDVAR       *RV;
  s_AMVIEW       *AMV;
  s_AMS          *AMS;
  DvmType            IntProc;
  byte           *TskList = NULL;

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0211*/
  DVMFTimeStart(call_waitrd_);

  if(RTL_TRACE)
     dvm_trace(call_waitrd_,"RedGroupRefPtr=%lx; RedGroupRef=%lx;\n",
                            (uLLng)RedGroupRefPtr, *RedGroupRefPtr);

  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.060: wrong call waitrd_\n"
            "(the reduction group is not a DVM object; "
            "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
         "*** RTS err 070.061: wrong call waitrd_\n"
         "(the object is not a reduction group; RedGroupRef=%lx)\n",
         *RedGroupRefPtr);

  RG = (s_REDGROUP *)RGHandlePtr->pP;

  /* Check if reduction group is created in current subtask */    /*E0212*/

  Proc   = gEnvColl->Count - 1;     /* current context index */    /*E0213*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0214*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.062: wrong call waitrd_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, Proc);

  /* Check if group is running */    /*E0215*/

  if(RG->StrtFlag == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.063: wrong call waitrd_\n"
           "(the reduction group has not been started; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  VMS = RG->VMS; /* reduction group processor system */    /*E0216*/

  if(RTL_TRACE)
  {  if(RedVarTrace && TstTraceEvent(call_waitrd_))
     {  PrintRedVars(&RG->RV, -1); /* print reduction variables */    /*E0217*/
        tprintf(" \n");
     }
  }

  if(RG->MPIReduce == 0)
  { if(VMS->HasCurrent) /* whether current processor belongs to
                           reduction group processor system */    /*E0218*/
    {  Central = VMS->CentralProc; /* internal number of
                                      central processor */    /*E0219*/

       if(Central == MPS_CurrentProc)
       {  if(coll_IndexOf(gRedGroupColl, RG) >= 0)
          {
             /* */    /*E0220*/

             if(RG->TskRDSign)
             {  for(i=0; i < RG->RV.Count; i++)
                {  RV = coll_At(s_REDVAR *, &RG->RV, i);

                   if(RV->AMView)
                      break;  /* */    /*E0221*/
                }

                n = RV->AMView->AMSColl.Count;
                mac_malloc(TskList, byte *, n, 0);
             }

             for(Proc=0; Proc < VMS->ProcCount; Proc++)
             {  if(VMS->VProc[Proc].lP != MPS_CurrentProc)
                {
                   if(RG->Flag[Proc] == 0)
                   {  if(RTL_TRACE)
                         ( RTL_CALL, red_Waitrequest(&RG->Req[Proc]) );
                      else
                         ( RTL_CALL, rtl_Waitrequest(&RG->Req[Proc]) );
                      RG->Flag[Proc] = 1; /* flag: waiting completion
                                             by central performed */    /*E0222*/
                   }

                   IntProc = VMS->VProc[Proc].lP; /* internal number
                                                  of currnet processor */    /*E0223*/

                   for(i=0; i < RG->RV.Count; i++)
                   {  RV = coll_At(s_REDVAR *, &RG->RV, i);

                      if(RV->AMView)
                      {  /* Reduction in subtasks */    /*E0224*/

                         for(j=0; j < n; j++)
                             TskList[j] = 1;

                         AMV = RV->AMView;

                         for(j=0; j < AMV->AMSColl.Count; j++)
                         {  AMS = coll_At(s_AMS *, &AMV->AMSColl, j);

                            if(TskList[j] == 0)
                               continue; /* */    /*E0225*/

                            if(AMS->VMS == NULL)
                               continue;  /* abstract machine
                                             is not mapped */    /*E0226*/

                            if(AMS->VMS->CentralProc != IntProc)
                               continue; /* current processor is not
                                            central in current subtask */    /*E0227*/

                            if(AMS->VMS->HasCurrent)
                               continue; /* */    /*E0228*/

                            CalculateRedVars(&RG->RV,
                               RG->NoWaitBufferPtr[Proc],
                               RG->NoWaitBufferPtr[VMS->VMSCentralProc],
                               i);

                            TskList[j] = 0;
                            break;
                         }
                      }
                      else
                      {  /* Reduction in processor system */    /*E0229*/

                         CalculateRedVars(&RG->RV,
                            RG->NoWaitBufferPtr[Proc],
                            RG->NoWaitBufferPtr[VMS->VMSCentralProc],
                            i);
                      }
                   }
                }
             }

             coll_Delete(gRedGroupColl, RG);  /* from the list
                                                 of started groups */    /*E0230*/
             CopyRedVars2Buffer(&RG->RV, RG->NoWaitBufferPtr[0]);

             for(Proc=0; Proc < VMS->ProcCount; Proc++)
             {  if(VMS->VProc[Proc].lP == MPS_CurrentProc)
                   continue;
                if(RTL_TRACE)
                   ( RTL_CALL, red_Sendnowait(RG->NoWaitBufferPtr[0], 1,
                                              RG->BlockSize,
                                              (int)VMS->VProc[Proc].lP,
                                              DVM_VMS->tag_RedVar,
                                              &RG->Req[Proc],
                                              RedCentral) );
                else
                   ( RTL_CALL, rtl_Sendnowait(RG->NoWaitBufferPtr[0], 1,
                                              RG->BlockSize,
                                              (int)VMS->VProc[Proc].lP,
                                              DVM_VMS->tag_RedVar,
                                              &RG->Req[Proc],
                                              RedCentral) );
             }

             if(MsgSchedule && UserSumFlag)
             {  rtl_TstReqColl(0);
                rtl_SendReqColl(0.);
             }
          }

          mac_free(&TskList);

          for(Proc=0; Proc < VMS->ProcCount; Proc++)
          {  if(VMS->VProc[Proc].lP == MPS_CurrentProc)
                continue;
             if(RTL_TRACE)
                ( RTL_CALL, red_Waitrequest(&RG->Req[Proc]) );
             else
                ( RTL_CALL, rtl_Waitrequest(&RG->Req[Proc]) );
          }
       }
       else
       {  if(RTL_TRACE)
          {  ( RTL_CALL, red_Waitrequest(&RG->Req[0]) );
             ( RTL_CALL, red_Waitrequest(&RG->Req[1]) );
          }
          else
          {  ( RTL_CALL, rtl_Waitrequest(&RG->Req[0]) );
             ( RTL_CALL, rtl_Waitrequest(&RG->Req[1]) );
          }

          coll_Delete(gRedGroupColl, RG); /* from the list
                                             of started groups */    /*E0231*/
          CopyBuffer2RedVars(&RG->RV, RG->NoWaitBufferPtr[1]);
       }

       if( EnableDynControl )
           dyn_AReductSetState( &RG->RV, dar_COMPLETED );
    }
  }
  else
  { /* */    /*E0232*/

    if(EnableDynControl && VMS->HasCurrent)
        dyn_AReductSetState( &RG->RV, dar_COMPLETED );
  }


  RG->StrtFlag  = 0;    /* group not started */    /*E0233*/
  RG->MPIReduce = 0;    /* */    /*E0234*/

  if(RTL_TRACE)
  {  if(RedVarTrace && TstTraceEvent(ret_waitrd_))
        PrintRedVars(&RG->RV, -1); /* print reduction variables */    /*E0235*/
  }

  if(RTL_TRACE)
     dvm_trace(ret_waitrd_," \n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0236*/
  DVMFTimeFinish(ret_waitrd_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delrg_(RedGroupRef  *RedGroupRefPtr)

/*
      Deleting reduction group.
      -------------------------

*RedGroupRefPtr	- reference to the reduction group.

The function deletes the reduction group created by function crtrg_.
The function returns zero.
*/    /*E0237*/

{ SysHandle    *RGHandlePtr;
  s_REDGROUP   *RG;
  int           i;
  void         *CurrAM;

  if(RgSave)
     return (DVM_RET, 0); /* */    /*E0238*/

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0239*/
  DVMFTimeStart(call_delrg_);

  if(RTL_TRACE)
     dvm_trace(call_delrg_,"RedGroupRefPtr=%lx; RedGroupRef=%lx;\n",
                           (uLLng)RedGroupRefPtr, *RedGroupRefPtr);

  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 070.070: wrong call delrg_\n"
             "(the reduction group is not a DVM object; "
             "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.071: wrong call delrg_\n"
           "(the object is not a reduction group; RedGroupRef=%lx)\n",
           *RedGroupRefPtr);

  RG = (s_REDGROUP *)RGHandlePtr->pP;

  /* Check if reduction group is created in current subtask */    /*E0240*/

  i      = gEnvColl->Count - 1;     /* current ontext index */    /*E0241*/
  CurrAM = (void *)CurrAMHandlePtr; /* current AM */    /*E0242*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.072: wrong call delrg_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, i);

  /* Check if reduction completed */    /*E0243*/

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.073: wrong call delrg_\n"
           "(the reduction has not been completed; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  ( RTL_CALL, delobj_(RedGroupRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delrg_,"\n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* for statistics */    /*E0244*/
  DVMFTimeFinish(ret_delrg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd rstrg_(RedGroupRef  *RedGroupRefPtr, DvmType  *DelRedSignPtr)
{ SysHandle    *RGHandlePtr, *NewRGHandlePtr;
  s_REDGROUP   *RG;
  int           i;
  void         *CurrAM;
  DvmType          StaticSign, DelRedSign;

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* */    /*E0245*/
  DVMFTimeStart(call_rstrg_);

  if(RTL_TRACE)
     dvm_trace(call_rstrg_,
               "RedGroupRefPtr=%lx; RedGroupRef=%lx; DelRedSign=%ld;\n",
               (uLLng)RedGroupRefPtr, *RedGroupRefPtr, *DelRedSignPtr);

  RGHandlePtr = (SysHandle *)*RedGroupRefPtr;

  if(TstObject)
  {  if(TstDVMObj(RedGroupRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 070.090: wrong call rstrg_\n"
             "(the reduction group is not a DVM object; "
             "RedGroupRef=%lx)\n", *RedGroupRefPtr);
  }

  if(RGHandlePtr->Type != sht_RedGroup)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.091: wrong call rstrg_\n"
           "(the object is not a reduction group; RedGroupRef=%lx)\n",
           *RedGroupRefPtr);

  RG = (s_REDGROUP *)RGHandlePtr->pP;

  /* */    /*E0246*/

  i      = gEnvColl->Count - 1;     /* */    /*E0247*/
  CurrAM = (void *)CurrAMHandlePtr; /* */    /*E0248*/

  if(RGHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.092: wrong call rstrg_\n"
        "(the reduction group was not created by the current subtask;\n"
        "RedGroupRef=%lx; RedGroupEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedGroupRefPtr, RGHandlePtr->CrtEnvInd, i);

  /* */    /*E0249*/

  if(RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.093: wrong call rstrg_\n"
           "(the reduction has not been completed; "
           "RedGroupRef=%lx)\n", *RedGroupRefPtr);

  StaticSign = RG->Static;
  DelRedSign = RG->DelRed;

  RG->ResetSign = 1;   /* */    /*E0250*/
  RG->DelRed    = (byte)*DelRedSignPtr;

  ( RTL_CALL, delobj_(RedGroupRefPtr) );

  NewRGHandlePtr = (SysHandle *)
                   ( RTL_CALL, crtrg_(&StaticSign, &DelRedSign) );

  RG = (s_REDGROUP *)NewRGHandlePtr->pP;
  RGHandlePtr->pP = NewRGHandlePtr->pP;
  RG->HandlePtr = RGHandlePtr;

  NewRGHandlePtr->Type = sht_NULL;
  dvm_FreeStruct(NewRGHandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_rstrg_,"\n");

  StatObjectRef = (ObjectRef)*RedGroupRefPtr; /* */    /*E0251*/
  DVMFTimeFinish(ret_rstrg_);
  return  (DVM_RET, 0);
}



DvmType  __callstd delred_(RedRef  *RedRefPtr)

/*
      Deleting reduction.
      -------------------

*RedRefPtr - reference to the reduction.

The function deletes the reduction  descriptor created by function
crtred_.
The function returns zero.
*/    /*E0252*/

{ SysHandle    *RedVarHandlePtr;
  s_REDVAR     *RVar;
  int           i;
  void         *CurrAM;

  if(RgSave)
     return (DVM_RET, 0); /* */    /*E0253*/

  StatObjectRef = (ObjectRef)*RedRefPtr;    /* for statistics */    /*E0254*/
  DVMFTimeStart(call_delred_);

  if(RTL_TRACE)
     dvm_trace(call_delred_,"RedRefPtr=%lx; RedRef=%lx;\n",
                            (uLLng)RedRefPtr, *RedRefPtr);

  RedVarHandlePtr=(SysHandle *)*RedRefPtr;

  if(TstObject)
  {  if(!TstDVMObj(RedRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 070.080: wrong call delred_\n"
            "(the reduction variable is not a DVM object; "
            "RedRef=%lx)\n", *RedRefPtr);
  }

  if(RedVarHandlePtr->Type != sht_RedVar)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
          "*** RTS err 070.081: wrong call delred_\n"
          "(the object is not a reduction variable; RedRef=%lx)\n",
          *RedRefPtr);

  RVar = (s_REDVAR *)RedVarHandlePtr->pP;

  /* Check if reduction variable is created in current subtask */    /*E0255*/

  i      = gEnvColl->Count - 1;     /* current context index */    /*E0256*/
  CurrAM = (void *)CurrAMHandlePtr; /* curretn AM */    /*E0257*/

  if(RedVarHandlePtr->CrtAMHandlePtr != CurrAM)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 070.082: wrong call delred_\n"
        "(the reduction variable was not created by "
        "the current subtask;\n"
        "RedRef=%lx; RedEnvIndex=%d; "
        "CurrentEnvIndex=%d)\n",
        *RedRefPtr, RedVarHandlePtr->CrtEnvInd, i);

  /* Check if reduction completed */    /*E0258*/

  if(RVar->RG && RVar->RG->StrtFlag)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 070.083: wrong call delred_\n"
           "(the reduction has not been completed;\n"
           "RedRef=%lx; RedGroupRef=%lx)\n",
           *RedRefPtr, (uLLng)RVar->RG->HandlePtr);

  ( RTL_CALL, delobj_(RedRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delred_," \n");

  StatObjectRef = (ObjectRef)*RedRefPtr;  /* for statistics */    /*E0259*/
  DVMFTimeFinish(ret_delred_);
  return  (DVM_RET, 0);
}


/* ------------------------------------------------ */    /*E0260*/


void  CopyRedVars2Buffer(s_COLLECTION *RedVars, void *Buffer)
{ int            i;
  int            VarSize, LocSize;
  s_REDVAR      *RVar;
  char          *BPtr = (char *)Buffer;

  if(Buffer == NULL)
     return;

  for(i=0; i < RedVars->Count; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);

     if(RVar->Mem == NULL)
        continue;

     VarSize = RVar->RedElmLength * RVar->VLength;
     LocSize = RVar->LocElmLength * RVar->VLength;

     if(VarSize > 0)
        dvm_memcopy(BPtr, RVar->Mem, VarSize);

     BPtr += VarSize;

     if(LocSize > 0)
        dvm_memcopy(BPtr, RVar->LocMem, LocSize);

     BPtr += LocSize;
  }
}



void  CopyBuffer2RedVars(s_COLLECTION *RedVars, void *Buffer)
{ int            i;
  int            VarSize, LocSize;
  s_REDVAR      *RVar;
  char          *BPtr = (char *)Buffer;

  if(Buffer == NULL)
     return;

  for(i=0; i < RedVars->Count; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);

     if(RVar->Mem == NULL)
        continue;

     VarSize = RVar->RedElmLength * RVar->VLength;
     LocSize = RVar->LocElmLength * RVar->VLength;

     if(VarSize > 0)
        dvm_memcopy(RVar->Mem, BPtr, VarSize);

     BPtr += VarSize;

     if(LocSize > 0)
        dvm_memcopy(RVar->LocMem, BPtr, LocSize);

     BPtr += LocSize;
  }
}



void  CalculateRedVars(s_COLLECTION *RedVars, void *Buffer1,
                       void *Buffer2, int VarInd)
{ int        i, j, VarSize, LocSize, VLength, InitInd, LastInd;
  char      *Mem, *LocMem, *VarPtr1, *LocPtr, *BPtr1 = (char *)Buffer1,
            *VarPtr2, *BPtr2 = (char *)Buffer2;
  float     *FloatPtr;
  double    *DoublePtr;
  float      FReal1, FImag1, FReal2, FImag2;
  double     DReal1, DImag1, DReal2, DImag2;
  s_REDVAR  *RVar;

  if(Buffer1 == NULL || Buffer2 == NULL)
     return;

  if(VarInd < 0)
  {  InitInd = 0;
     LastInd = RedVars->Count;
  }
  else
  { InitInd = VarInd;
    LastInd = VarInd + 1;
  }

  for (i=0; i < InitInd; i++)
  {  RVar   = coll_At(s_REDVAR *, RedVars, i);
     BPtr1 += RVar->BlockSize;
     BPtr2 += RVar->BlockSize;
  }

  for (i=InitInd; i < LastInd; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);
     Mem     = RVar->Mem;

     if(Mem == NULL)
        continue;

     LocMem  = RVar->LocMem;
     VarSize = RVar->RedElmLength;
     LocSize = RVar->LocElmLength;
     VLength = RVar->VLength;
     VarPtr1 = BPtr1;
     LocPtr  = VarPtr1 + VarSize*VLength;
     VarPtr2 = BPtr2;

     BPtr1  += RVar->BlockSize;
     BPtr2  += RVar->BlockSize;

     switch (RVar->Func)
     {  case rf_SUM     :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(int, Mem, VarPtr1, +, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(long, Mem, VarPtr1, +, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperator(long long, Mem, VarPtr1, +, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(double, Mem, VarPtr1, +, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(float, Mem, VarPtr1, +, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_DOUBLE_COMPLEX:
                     for(j=0; j < VLength; j++)
                     {  *((double *)Mem) += *((double *)VarPtr1);
                        Mem     += sizeof(double);
                        VarPtr1 += sizeof(double);
                        *((double *)Mem) += *((double *)VarPtr1);
                        Mem     += sizeof(double);
                        VarPtr1 += sizeof(double);
                     }
                     break;

                case rt_FLOAT_COMPLEX:
                     for(j=0; j < VLength; j++)
                     {  *((float *)Mem) += *((float *)VarPtr1);
                        Mem     += sizeof(float);
                        VarPtr1 += sizeof(float);
                        *((float *)Mem) += *((float *)VarPtr1);
                        Mem     += sizeof(float);
                        VarPtr1 += sizeof(float);
                     }
                     break;
             }
             break;

        case rf_MULT    :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(int, Mem, VarPtr1, *, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(long, Mem, VarPtr1, *, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperator(long long, Mem, VarPtr1, *, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(double, Mem, VarPtr1, *, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(float, Mem, VarPtr1, *, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_DOUBLE_COMPLEX:
                     for(j=0; j < VLength; j++)
                     {  DReal1    = *(double *)VarPtr1;
                        VarPtr1  += sizeof(double);
                        DImag1    = *(double *)VarPtr1;

                        DoublePtr = (double *)Mem;
                        DReal2    = *DoublePtr;
                        Mem      += sizeof(double);
                        DImag2    = *(double *)Mem;

                        *DoublePtr     = DReal1*DReal2 - DImag1*DImag2;
                        *(double *)Mem = DReal1*DImag2 + DReal2*DImag1;

                        Mem     += sizeof(double);
                        VarPtr1 += sizeof(double);
                     }
                     break;

                case rt_FLOAT_COMPLEX:
                     for(j=0; j < VLength; j++)
                     {  FReal1    = *(float *)VarPtr1;
                        VarPtr1  += sizeof(float);
                        FImag1    = *(float *)VarPtr1;

                        FloatPtr = (float *)Mem;
                        FReal2   = *FloatPtr;
                        Mem     += sizeof(float);
                        FImag2   = *(float *)Mem;

                        *FloatPtr     = FReal1*FReal2 - FImag1*FImag2;
                        *(float *)Mem = FReal1*FImag2 + FReal2*FImag1;

                        Mem     += sizeof(float);
                        VarPtr1 += sizeof(float);
                     }
                     break;
             }
             break;

        case rf_MAX     :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(int, Mem, dvm_max, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(long, Mem, dvm_max, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyFunc(long long, Mem, dvm_max, VarPtr1, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(double, Mem, dvm_max, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(float, Mem, dvm_max, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
              }
              break;

        case rf_MIN     :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(int, Mem, dvm_min, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(long, Mem, dvm_min, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyFunc(long long, Mem, dvm_min, VarPtr1, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(double, Mem, dvm_min, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     {  ApplyFunc(float, Mem, dvm_min, VarPtr1, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
             }
             break;

        case rf_MINLOC  :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyMinLoc(int, Mem, VarPtr1, LocMem, LocPtr,
                                    LocSize);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        LocMem  += LocSize;
                        LocPtr  += LocSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyMinLoc(long, Mem, VarPtr1, LocMem, LocPtr, LocSize);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        LocMem  += LocSize;
                        LocPtr  += LocSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyMinLoc(long long, Mem, VarPtr1, LocMem, LocPtr, LocSize);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                        LocMem += LocSize;
                        LocPtr += LocSize;
                    }
                    break;

                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     { ApplyMinLoc(double, Mem, VarPtr1, LocMem, LocPtr,
                                   LocSize);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                       LocMem  += LocSize;
                       LocPtr  += LocSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     { ApplyMinLoc(float, Mem, VarPtr1, LocMem, LocPtr,
                                   LocSize);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                       LocMem  += LocSize;
                       LocPtr  += LocSize;
                     }
                     break;
             }
             break;

        case rf_MAXLOC  :
             switch (RVar->VType)
             {  case rt_INT   :
                     for(j=0; j < VLength; j++)
                     {  ApplyMaxLoc(int, Mem, VarPtr1, LocMem, LocPtr,
                                    LocSize);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        LocMem  += LocSize;
                        LocPtr  += LocSize;
                     }
                     break;

                case rt_LONG  :
                     for(j=0; j < VLength; j++)
                     {  ApplyMaxLoc(long, Mem, VarPtr1, LocMem, LocPtr, LocSize);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        LocMem  += LocSize;
                        LocPtr  += LocSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyMaxLoc(long long, Mem, VarPtr1, LocMem, LocPtr, LocSize);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                        LocMem += LocSize;
                        LocPtr += LocSize;
                    }
                    break;

                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     { ApplyMaxLoc(double, Mem, VarPtr1, LocMem, LocPtr,
                                   LocSize);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                       LocMem  += LocSize;
                       LocPtr  += LocSize;
                     }
                     break;

                case rt_FLOAT :
                     for(j=0; j < VLength; j++)
                     { ApplyMaxLoc(float, Mem, VarPtr1, LocMem, LocPtr,
                                   LocSize);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                       LocMem  += LocSize;
                       LocPtr  += LocSize;
                     }
                     break;
             }
             break;

        case rf_AND     :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(int, Mem, VarPtr1, &, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(long, Mem, VarPtr1, &, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;
                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperator(long long, Mem, VarPtr1, &, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
             }
             break;

        case rf_OR      :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(int, Mem, VarPtr1, |, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(long, Mem, VarPtr1, |, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperator(long long, Mem, VarPtr1, | , Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
             }
             break;

        case rf_XOR     :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(int, Mem, VarPtr1, ^, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     {  ApplyOperator(long, Mem, VarPtr1, ^, Mem);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperator(long long, Mem, VarPtr1, ^, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
             }
             break;

        case rf_EQU     :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     { ApplyOperatorWithInv(int, Mem, VarPtr1, ^, Mem);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     { ApplyOperatorWithInv(long, Mem, VarPtr1, ^, Mem);
                       Mem     += VarSize;
                       VarPtr1 += VarSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyOperatorWithInv(long long, Mem, VarPtr1, ^, Mem);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                    }
                    break;
             }
             break;

        case rf_NE    :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(int, Mem, ||, VarPtr1, !=,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(long, Mem, ||, VarPtr1, !=, VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyLogOperator(long long, Mem, || , VarPtr1, != , VarPtr2);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                    }
                    break;

                case rt_FLOAT:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(float, Mem, ||, VarPtr1, !=,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(double, Mem, ||, VarPtr1, !=,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;
             }
             break;

        case rf_EQ    :
             switch (RVar->VType)
             {  case rt_INT :
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(int, Mem, &&, VarPtr1, ==,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_LONG:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(long, Mem, &&, VarPtr1, ==, VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_LLONG:
                    for (j = 0; j < VLength; j++)
                    {
                        ApplyLogOperator(long long, Mem, &&, VarPtr1, == , VarPtr2);
                        Mem += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                    }
                    break;

                case rt_FLOAT:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(float, Mem, &&, VarPtr1, ==,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;

                case rt_DOUBLE:
                     for(j=0; j < VLength; j++)
                     {  ApplyLogOperator(double, Mem, &&, VarPtr1, ==,
                                         VarPtr2);
                        Mem     += VarSize;
                        VarPtr1 += VarSize;
                        VarPtr2 += VarSize;
                     }
                     break;
             }
             break;
     }
  }

  return;
}



void  CorrectRedVars(s_COLLECTION *RedVars, void *Buffer)
{ int            i, j, VarSize, LocSize, VLength;
  s_REDVAR      *RVar;
  char          *VarPtr, *Mem, *BPtr = (char *)Buffer;
  float         *FloatPtr;
  double        *DoublePtr;
  float          FReal1, FImag1, FReal2, FImag2;
  double         DReal1, DImag1, DReal2, DImag2;
  float          Fa2b2;
  double         Da2b2;

  if(Buffer == NULL)
     return;

  for(i=0; i < RedVars->Count; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);
     Mem     = RVar->Mem; /* variable address in program memory */    /*E0261*/

     if(Mem == NULL)
        continue;

     VarPtr  = BPtr;      /* variable address in buffer */    /*E0262*/
     VarSize = RVar->RedElmLength;
     LocSize = RVar->LocElmLength;
     VLength = RVar->VLength;

     BPtr   += RVar->BlockSize;

     switch(RVar->Func)
     {  case rf_SUM:

           switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(int, VarPtr, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(long, VarPtr, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      ApplyOperator(long long, VarPtr, Mem, -, VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(double, VarPtr, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(float, VarPtr, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_DOUBLE_COMPLEX:
                   for(j=0; j < VLength; j++)
                   {  *((double *)VarPtr) = *((double *)Mem) -
                                            *((double *)VarPtr);
                      Mem    += sizeof(double);
                      VarPtr += sizeof(double);
                      *((double *)VarPtr) = *((double *)Mem) -
                                            *((double *)VarPtr);
                      Mem    += sizeof(double);
                      VarPtr += sizeof(double);
                   }
                   break;

              case rt_FLOAT_COMPLEX:
                   for(j=0; j < VLength; j++)
                   {  *((float *)VarPtr) = *((float *)Mem) -
                                           *((float *)VarPtr);
                      Mem    += sizeof(float);
                      VarPtr += sizeof(float);
                      *((float *)VarPtr) = *((float *)Mem) -
                                           *((float *)VarPtr);
                      Mem    += sizeof(float);
                      VarPtr += sizeof(float);
                   }
                   break;
           }
           break;

        case rf_MULT:

           switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  if(*(int *)VarPtr == 0)
                         *(int *)VarPtr = 1;
                      ApplyOperator(int, VarPtr, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {
                       if (*(long *)VarPtr == 0)
                           *(long *)VarPtr = 1;
                       ApplyOperator(long, VarPtr, Mem, / , VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      if (*(long long *)VarPtr == 0)
                          *(long long*)VarPtr = 1;
                      ApplyOperator(long long, VarPtr, Mem, / , VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  if(*(double *)VarPtr == 0.)
                         *(double *)VarPtr = 1.;
                      ApplyOperator(double, VarPtr, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  if(*(float *)VarPtr == 0.f)
                         *(float *)VarPtr = 1.f;
                      ApplyOperator(float, VarPtr, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_DOUBLE_COMPLEX:
                   for(j=0; j < VLength; j++)
                   {  DoublePtr = (double *)VarPtr;
                      DReal2    = *DoublePtr;
                      VarPtr   += sizeof(double);
                      DImag2    = *(double *)VarPtr;

                      DReal1    = *(double *)Mem;
                      Mem      += sizeof(double);
                      DImag1    = *(double *)Mem;

                      Da2b2 = DReal2*DReal2 + DImag2*DImag2;

                      if(Da2b2 == 0.)
                      {  *DoublePtr        = DReal1;
                         *(double *)VarPtr = DImag1;
                      }
                      else
                      {  *DoublePtr =
                         (DReal1*DReal2 + DImag1*DImag2)/Da2b2;
                         *(double *)VarPtr =
                         (DReal2*DImag1 - DReal1*DImag2)/Da2b2;
                      }

                      Mem    += sizeof(double);
                      VarPtr += sizeof(double);
                   }
                   break;

              case rt_FLOAT_COMPLEX :
                   for(j=0; j < VLength; j++)
                   {  FloatPtr = (float *)VarPtr;
                      FReal2   = *FloatPtr;
                      VarPtr  += sizeof(float);
                      FImag2   = *(float *)VarPtr;

                      FReal1    = *(float *)Mem;
                      Mem      += sizeof(float);
                      FImag1    = *(float *)Mem;

                      Fa2b2 = FReal2*FReal2 + FImag2*FImag2;

                      if(Fa2b2 == 0.f)
                      {  *FloatPtr        = FReal1;
                         *(float *)VarPtr = FImag1;
                      }
                      else
                      {  *FloatPtr =
                         (FReal1*FReal2 + FImag1*FImag2)/Fa2b2;
                         *(float *)VarPtr =
                         (FReal2*FImag1 - FReal1*FImag2)/Fa2b2;
                      }

                      Mem    += sizeof(float);
                      VarPtr += sizeof(float);
                   }
                   break;
           }
           break;

        case rf_XOR     :

           switch (RVar->VType)
           {  case rt_INT :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(int, VarPtr, Mem, ^, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG:
                   for(j=0; j < VLength; j++)
                   {
                       ApplyOperator(long, VarPtr, Mem, ^, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      ApplyOperator(long long, VarPtr, Mem, ^, VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;
           }
           break;

        case rf_EQU     :

           switch (RVar->VType)
           {  case rt_INT :
                   for(j=0; j < VLength; j++)
                   { ApplyOperatorWithInv(int, VarPtr, Mem, ^, VarPtr);
                     Mem    += VarSize;
                     VarPtr += VarSize;
                   }
                   break;

              case rt_LONG:
                   for(j=0; j < VLength; j++)
                   {
                       ApplyOperatorWithInv(long, VarPtr, Mem, ^, VarPtr);
                     Mem    += VarSize;
                     VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      ApplyOperatorWithInv(long long, VarPtr, Mem, ^, VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;
           }
           break;

        default:

           switch (RVar->VType)
           {  case rt_INT   :
                for(j=0; j < VLength; j++)
                {  CopyRedVarToBuf(int, VarPtr, Mem);
                   Mem    += VarSize;
                   VarPtr += VarSize;
                }
                break;

              case rt_LONG  :
                for(j=0; j < VLength; j++)
                {
                    CopyRedVarToBuf(long, VarPtr, Mem);
                   Mem    += VarSize;
                   VarPtr += VarSize;
                }
                break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      CopyRedVarToBuf(long long, VarPtr, Mem);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                for(j=0; j < VLength; j++)
                {  CopyRedVarToBuf(double, VarPtr, Mem);
                   Mem    += VarSize;
                   VarPtr += VarSize;
                }
                break;

              case rt_FLOAT :
                for(j=0; j < VLength; j++)
                {  CopyRedVarToBuf(float, VarPtr, Mem);
                   Mem    += VarSize;
                   VarPtr += VarSize;
                }
                break;
           }

           if(LocSize > 0)
           {  dvm_memcopy(VarPtr, RVar->LocMem, (LocSize * VLength));
           }
           break;
     }
  }

  return;
}



void  MPI_CorrectRedVars(s_COLLECTION *RedVars, void *Buffer)
{ int            i, j, VarSize, LocSize, VLength;
  s_REDVAR      *RVar;
  char          *VarPtr, *Mem, *BPtr = (char *)Buffer;

  if(Buffer == NULL)
     return;

  for(i=0; i < RedVars->Count; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);
     Mem     = RVar->Mem; /* */    /*E0263*/

     if(Mem == NULL)
        continue;

     VarPtr  = BPtr;      /* */    /*E0264*/
     VarSize = RVar->RedElmLength;
     LocSize = RVar->LocElmLength;
     VLength = RVar->VLength;

     BPtr   += RVar->BlockSize;

     switch(RVar->Func)
     {  case rf_SUM:

           switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(int, Mem, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {
                       ApplyOperator(long, Mem, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      ApplyOperator(long long, Mem, Mem, -, VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(double, Mem, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(float, Mem, Mem, -, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_DOUBLE_COMPLEX:
                   for(j=0; j < VLength; j++)
                   {  *((double *)Mem) = *((double *)Mem) -
                                            *((double *)VarPtr);
                      Mem    += sizeof(double);
                      VarPtr += sizeof(double);
                      *((double *)Mem) = *((double *)Mem) -
                                            *((double *)VarPtr);
                      Mem    += sizeof(double);
                      VarPtr += sizeof(double);
                   }
                   break;

              case rt_FLOAT_COMPLEX:
                   for(j=0; j < VLength; j++)
                   {  *((float *)Mem) = *((float *)Mem) -
                                           *((float *)VarPtr);
                      Mem    += sizeof(float);
                      VarPtr += sizeof(float);
                      *((float *)Mem) = *((float *)Mem) -
                                           *((float *)VarPtr);
                      Mem    += sizeof(float);
                      VarPtr += sizeof(float);
                   }
                   break;
           }
           break;

        case rf_MULT:

           switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  if(*(int *)VarPtr == 0)
                         *(int *)VarPtr = 1;
                      ApplyOperator(int, Mem, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {
                       if (*(long *)VarPtr == 0)
                           *(long *)VarPtr = 1;
                       ApplyOperator(long, Mem, Mem, / , VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      if (*(long long *)VarPtr == 0)
                          *(long long*)VarPtr = 1;
                      ApplyOperator(long long, Mem, Mem, / , VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  if(*(double *)VarPtr == 0.)
                         *(double *)VarPtr = 1.;
                      ApplyOperator(double, Mem, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  if(*(float *)VarPtr == 0.f)
                         *(float *)VarPtr = 1.f;
                      ApplyOperator(float, Mem, Mem, /, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;
           }
           break;

        case rf_XOR     :

           switch (RVar->VType)
           {  case rt_INT :
                   for(j=0; j < VLength; j++)
                   {  ApplyOperator(int, Mem, Mem, ^, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LONG:
                   for(j=0; j < VLength; j++)
                   {
                       ApplyOperator(long, Mem, Mem, ^, VarPtr);
                      Mem    += VarSize;
                      VarPtr += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      ApplyOperator(long long, Mem, Mem, ^, VarPtr);
                      Mem += VarSize;
                      VarPtr += VarSize;
                  }
                  break;
           }
           break;
     }
  }

  return;
}



void  SetRedVars(s_COLLECTION  *RedVars)
{ int            i, j, VarSize, VLength;
  s_REDVAR      *RVar;
  char          *Mem;

  for(i=0; i < RedVars->Count; i++)
  {  RVar    = coll_At(s_REDVAR *, RedVars, i);
     Mem     = RVar->Mem; /* reduction variable-array address
                             in program memory */    /*E0265*/

     if(Mem == NULL)
        continue;

     VarSize = RVar->RedElmLength; /* per element length
                                      of reduction variable-array */    /*E0266*/
     VLength = RVar->VLength; /* number of elements in
                                 reduction variable-array */    /*E0267*/

     if(RVar->Func == rf_EQ || RVar->Func == rf_NE)
     {  if(RVar->Func == rf_EQ)
        {  switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  *(int *)Mem = 1;
                      Mem    += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {
                       *(long *)Mem = 1;
                      Mem    += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      *(long long *)Mem = 1;
                      Mem += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  *(double *)Mem = 1.;
                      Mem    += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  *(float *)Mem = 1.f;
                      Mem    += VarSize;
                   }
                   break;
           }
        }
        else
        {  switch (RVar->VType)
           {  case rt_INT   :
                   for(j=0; j < VLength; j++)
                   {  *(int *)Mem = 0;
                      Mem    += VarSize;
                   }
                   break;

              case rt_LONG  :
                   for(j=0; j < VLength; j++)
                   {
                       *(long *)Mem = 0;
                      Mem    += VarSize;
                   }
                   break;

              case rt_LLONG:
                  for (j = 0; j < VLength; j++)
                  {
                      *(long long *)Mem = 0;
                      Mem += VarSize;
                  }
                  break;

              case rt_DOUBLE:
                   for(j=0; j < VLength; j++)
                   {  *(double *)Mem = 0.;
                      Mem    += VarSize;
                   }
                   break;

              case rt_FLOAT :
                   for(j=0; j < VLength; j++)
                   {  *(float *)Mem = 0.f;
                      Mem    += VarSize;
                   }
                   break;
           }
        }
     }
  }

  return;
}



void  PrintRedVars(s_COLLECTION  *RedVars, int  VarInd)
{ int            i, j, InitInd, LastInd, Count;
  s_REDVAR      *RVar;
  char          *CharPtr;

  if(VarInd < 0)
  {  InitInd = 0;
     LastInd = RedVars->Count;
  }
  else
  {  InitInd = VarInd;
     LastInd = VarInd + 1;
  }

  for(i=InitInd; i < LastInd; i++)
  {  RVar = coll_At(s_REDVAR *, RedVars, i);

     if(RVar->Mem == NULL)
        continue;

     switch(RVar->Func)
     {  case rf_SUM   : tprintf("rf_SUM;    ");
                        break;
        case rf_MULT  : tprintf("rf_MULT;   ");
                        break;
        case rf_MAX   : tprintf("rf_MAX;    ");
                        break;
        case rf_MIN   : tprintf("rf_MIN;    ");
                        break;
        case rf_MINLOC: tprintf("rf_MINLOC; ");
                        break;
        case rf_MAXLOC: tprintf("rf_MAXLOC; ");
                        break;
        case rf_AND   : tprintf("rf_AND;    ");
                        break;
        case rf_OR    : tprintf("rf_OR;     ");
                        break;
        case rf_XOR   : tprintf("rf_XOR;    ");
                        break;
        case rf_EQU   : tprintf("rf_EQU;    ");
                        break;
        case rf_NE    : tprintf("rf_NE;     ");
                        break;
        case rf_EQ    : tprintf("rf_EQ;     ");
                        break;
     }

     switch(RVar->VType)
     {  case rt_INT:
             tprintf("rt_INT;            ");
             break;
        case rt_LONG:
             tprintf("rt_LONG;           ");
             break;
        case rt_LLONG:
            tprintf("rt_LLONG;           ");
            break;
        case rt_FLOAT:
             tprintf("rt_FLOAT;          ");
             break;
        case rt_DOUBLE:
             tprintf("rt_DOUBLE;         ");
             break;
        case rt_FLOAT_COMPLEX:
             tprintf("rt_FLOAT_COMPLEX;  ");
             break;
        case rt_DOUBLE_COMPLEX:
             tprintf("rt_DOUBLE_COMPLEX; ");
             break;
     }

     tprintf("RVAddr = %lx; RVVal = ", (uLLng)RVar->Mem);

     CharPtr = (char *)RVar->Mem;

     switch(RVar->VType)
     {  case rt_INT:
             tprintf("%d\n", *(int *)RVar->Mem);
             CharPtr += sizeof(int);
             break;
        case rt_LONG:
            tprintf("%ld\n", *(long *)RVar->Mem);
            CharPtr += sizeof(long);
             break;
        case rt_LLONG:
            tprintf("%lld\n", *(long long *)RVar->Mem);
            CharPtr += sizeof(long long);
            break;
        case rt_FLOAT:
             tprintf("%f\n", *(float *)RVar->Mem);
             CharPtr += sizeof(float);
             break;
        case rt_DOUBLE:
             tprintf("%lf\n", *(double *)RVar->Mem);
             CharPtr += sizeof(double);
             break;
        case rt_FLOAT_COMPLEX:
             CharPtr += sizeof(float);
             tprintf("%f(%f)\n", *(float *)RVar->Mem,
                                 *(float *)CharPtr);
             CharPtr += sizeof(float);
             break;
        case rt_DOUBLE_COMPLEX:
             CharPtr += sizeof(double);
             tprintf("%lf(%lf)\n", *(double *)RVar->Mem,
                                   *(double *)CharPtr);
             CharPtr += sizeof(double);
             break;
     }

     Count = dvm_min(RedVarTrace, RVar->VLength);

     for(j=1; j < Count; j++)
     {  tprintf("                              ");
        tprintf("RVAddr = %lx; RVVal = ", (uLLng)CharPtr);

        switch(RVar->VType)
        {  case rt_INT:
                tprintf("%d\n", *(int *)CharPtr);
                CharPtr += sizeof(int);
                break;
           case rt_LONG:
               tprintf("%ld\n", *(long *)CharPtr);
               CharPtr += sizeof(long);
                break;
           case rt_LLONG:
               tprintf("%lld\n", *(long long *)CharPtr);
               CharPtr += sizeof(long long);
               break;
           case rt_FLOAT:
                tprintf("%f\n", *(float *)CharPtr);
                CharPtr += sizeof(float);
                break;
           case rt_DOUBLE:
                tprintf("%lf\n", *(double *)CharPtr);
                CharPtr += sizeof(double);
                break;
           case rt_FLOAT_COMPLEX:
                tprintf("%f", *(float *)CharPtr);
                CharPtr += sizeof(float);
                tprintf("(%f)\n", *(float *)CharPtr);
                CharPtr += sizeof(float);
                break;
           case rt_DOUBLE_COMPLEX:
                tprintf("%lf", *(double *)CharPtr);
                CharPtr += sizeof(double);
                tprintf("(%lf)\n", *(double *)CharPtr);
                CharPtr += sizeof(double);
                break;
        }
     }
  }

  return;
}


/* ---------------------------------------------- */    /*E0268*/


void   RedGroup_Done(s_REDGROUP  *RG)
{ int            i;
  s_REDVAR      *RVar;
  RedRef         RVRef;
  RedGroupRef    RedGrpRef;
/*
  PSRef          RGPSRef;
*/    /*E0269*/

  if(RgSave)
  { (DVM_RET);
    return;    /* */    /*E0270*/
  }

  if(RTL_TRACE)
     dvm_trace(call_RedGroup_Done, "RedGroupRef=%lx;\n",
                                   (uLLng)RG->HandlePtr);
  if(RG->StrtFlag)
  {  RedGrpRef = (RedGroupRef)RG->HandlePtr;
     ( RTL_CALL, waitrd_(&RedGrpRef) );
  }

  /* */    /*E0271*/

  if(RG->CrtVMSSign != 0 && RG->VMS != NULL)
  {  /* */    /*E0272*/

     coll_Insert(&RG->PSSpaceVMS->RedSubSystem, RG->VMS);

/*
     RGPSRef = (PSRef)RG->VMS->HandlePtr;
     ( RTL_CALL, delps_(&RGPSRef) );
*/    /*E0273*/
  }

  /* ----------------------------------------------- */    /*E0274*/

  if(RG->NoWaitBufferPtr != NULL)
  {
     /* */    /*E0275*/

     mac_free(&RG->ResBuf);
     mac_free(&RG->InitBuf);

     if(RG->IsBuffers)
        for(i=1; i < RG->VMS->ProcCount; i++)
        {   mac_free(&RG->NoWaitBufferPtr[i]);
        }

     RG->IsBuffers = 0;
     RG->IsNewVars = 0;

     mac_free(&RG->NoWaitBufferPtr[0]);

     dvm_FreeArray(RG->NoWaitBufferPtr);
     dvm_FreeArray(RG->Req);
     dvm_FreeArray(RG->Flag);
  }

  RG->NoWaitBufferPtr = NULL;

  /* Delete reduction variables */    /*E0276*/

  for(i=0; i < RG->RV.Count; i++)
  {  RVar = coll_At(s_REDVAR *, &RG->RV, i);

     RVar->RG      = NULL;  /* turn off pointer to cancelled group */    /*E0277*/
     RVar->AMView  = NULL;  /* */    /*E0278*/
     RVar->BufAddr = NULL;  /* */    /*E0279*/

     RVRef = (RedRef)RVar->HandlePtr;

     if(RG->DelRed)
     {  if(DelObjFlag)   /* explicit group deleting */    /*E0280*/
           ( RTL_CALL, delred_(&RVRef) );
        else             /* implicit group deleting */    /*E0281*/
        {  if(RVar->Static == 0)
              ( RTL_CALL, delred_(&RVRef) );
        }
     }
  }

  dvm_FreeArray(RG->RV.List);

  if(TstObject)
     DelDVMObj((ObjectRef)RG->HandlePtr);

  /* Delete own Handle */    /*E0282*/

  if(RG->ResetSign == 0)
  {  /* */    /*E0283*/

     RG->HandlePtr->Type = sht_NULL;
     dvm_FreeStruct(RG->HandlePtr);
  }

  RG->ResetSign = 0;

  if(RTL_TRACE)
     dvm_trace(ret_RedGroup_Done," \n");

  (DVM_RET);

  return;
}



void RedVar_Done(s_REDVAR *RV)
{ s_REDGROUP  *RG;
  s_REDVAR    *RVar;
  int          i;

  if(RgSave)
  { (DVM_RET);
    return;    /* */    /*E0284*/
  }

  if(RTL_TRACE)
     dvm_trace(call_RedVar_Done,"RedRef=%lx;\n", (uLLng)RV->HandlePtr);

  if(TstObject)
     DelDVMObj((ObjectRef)RV->HandlePtr);

  RG = RV->RG;

  if(RG) /* whether the cancelled variable
            is included into some group */    /*E0285*/
  {  /* Recompute variable indeces in group */    /*E0286*/

     for(i=RV->VarInd+1; i < RG->RV.Count; i++)
     {  RVar = coll_At(s_REDVAR *, &RG->RV, i);
        RVar->VarInd--;
     }

     coll_AtDelete(&RG->RV, RV->VarInd);  /* delete variable
                                             from group variable list */    /*E0287*/
  }

  RV->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(RV->HandlePtr); /* free own Handle */    /*E0288*/

  if(RTL_TRACE)
     dvm_trace(ret_RedVar_Done," \n");

  (DVM_RET);

  return;
}


/* */    /*E0289*/

#ifdef  _DVM_MPI_

int  DoNotMPIRed(s_REDGROUP  *RG)
{ int              i, Count, MaxBlockSize = 0, j, k, m, n;
  s_REDVAR        *RV, *RV1;
  s_COLLECTION    *RGRV;
  MPI_Datatype     datatype;
  MPI_Op           op;
  char            *CharPtr1, *CharPtr2;
  s_VMS           *VMS;
  SysHandle       *VProc;
  double           time;
  void            *ResBuf;

  RG->MPIReduce = 0;

  Count = RG->RV.Count;
  RGRV  = &RG->RV;

  if(RG->TskRDSign)
     return  1;    /* */    /*E0290*/

  if(DVM_VMS->Is_MPI_COMM == 0)
     return  2;    /* */    /*E0291*/

  for(i=0; i < Count; i++)
  {  RV = coll_At(s_REDVAR *, RGRV, i);

     if(RV->Func == rf_MINLOC || RV->Func == rf_MAXLOC ||
        RV->Func == rf_EQU || RV->Func == rf_EQ || RV->Func == rf_NE)
        return  3; /* */    /*E0292*/

     if((RV->VType == rt_FLOAT_COMPLEX || RV->VType == rt_DOUBLE_COMPLEX)
        && RV->Func == rf_MULT)
        return  4; /* */    /*E0293*/

     if(RV->Mem == NULL)
        return  5; /* */    /*E0294*/
  }

  DVMMTimeStart(call_MPI_Allreduce);

  VMS = RG->VMS;  /* */    /*E0295*/

  if(VMS->Is_MPI_COMM == 0)
  {  /* */    /*E0296*/

     VMS->Is_MPI_COMM = 1; /* */    /*E0297*/

     if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
     {
        tprintf(" \n");
        tprintf("*** MPI_Comm Creating ***\n");
        time = dvm_time();
     }

     MPI_Comm_split(DVM_VMS->PS_MPI_COMM, VMS->CentralProc,
                    VMS->CurrentProc, &VMS->PS_MPI_COMM);
     MPI_Comm_group(VMS->PS_MPI_COMM, &VMS->ps_mpi_group);

     if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
     {  time = dvm_time() - time;
     }
  }
  else
  {  if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
     {
        tprintf(" \n");
        tprintf("*** MPI_Comm Information ***\n");
        time = 0.;
     }
  }

  if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
  {  i = (int)VMS->ProcCount;

     tprintf("&MPI_Comm=%lx; &MPI_Group=%lx; ProcNumber=%d; time=%lf;\n",
             (uLLng)&VMS->PS_MPI_COMM, (uLLng)&VMS->ps_mpi_group, i, time);

     j = (int)ceil( (double)i / 10.); /* */    /*E0298*/
     VProc = VMS->VProc;

     tprintf("MPI_Group:");

     for(k=0,m=0,i--; k < j; k++)
     {  for(n=0; n < 10; n++,m++)
        {  tprintf(" %4ld", VProc[m].lP);

           if(m == i)
              break;
        }

        tprintf("\n");
     }
  }

  if(MPS_CurrentProc != VMS->CentralProc)
     MPI_CorrectRedVars(RGRV, RG->NoWaitBufferPtr[0]);

  if(MPIReduce < 2 || Count == 1)
  {  /* */    /*E0299*/

     if(RG->DA == NULL || Count != 1)
     {
        if(RG->ResBuf && RG->IsNewVars)
        {  /* */    /*E0300*/

           mac_free(&RG->ResBuf);
        }

        if(RG->ResBuf == NULL)
        {  /* */    /*E0301*/

           for(i=0; i < Count; i++)
           {  RV = coll_At(s_REDVAR *, RGRV, i);

              MaxBlockSize = dvm_max(MaxBlockSize, RV->BlockSize);
           }

           mac_malloc(RG->ResBuf, void *, MaxBlockSize, 0);
        }

        ResBuf = RG->ResBuf;
     }
     else
     {  /* */    /*E0302*/

        RV = coll_At(s_REDVAR *, RGRV, 0);
        MaxBlockSize = RV->BlockSize;

        if(VMS->ResBuf == NULL)
        {
           mac_malloc(VMS->ResBuf, void *, MaxBlockSize, 0);
           VMS->MaxBlockSize = MaxBlockSize;
        }
        else
        {  if(VMS->MaxBlockSize < MaxBlockSize)
           {  mac_free(&VMS->ResBuf);
              mac_malloc(VMS->ResBuf, void *, MaxBlockSize, 0);
              VMS->MaxBlockSize = MaxBlockSize;
           }
        }

        ResBuf = VMS->ResBuf;
     }

     RG->IsNewVars = 0; /* */    /*E0303*/

     for(i=0; i < Count; i++)
     {  RV = coll_At(s_REDVAR *, RGRV, i);

        switch(RV->VType)
        {  case rt_INT:             datatype = MPI_INT;      break;
           case rt_LONG:            datatype = MPI_LONG;     break;
           case rt_LLONG:           datatype = MPI_LONG_LONG;break;
           case rt_FLOAT:           datatype = MPI_FLOAT;    break;
           case rt_DOUBLE:          datatype = MPI_DOUBLE;   break;
           case rt_FLOAT_COMPLEX:   datatype = MPI_FLOAT;    break;
           case rt_DOUBLE_COMPLEX:  datatype = MPI_DOUBLE;   break;
        }

        switch(RV->Func)
        {  case rf_SUM:     op = MPI_SUM;     break;
           case rf_MULT:    op = MPI_PROD;    break;
           case rf_MAX:     op = MPI_MAX;     break;
           case rf_MIN:     op = MPI_MIN;     break;
           case rf_AND:     op = MPI_BAND;    break;
           case rf_OR:      op = MPI_BOR;     break;
           case rf_XOR:     op = MPI_BXOR;    break;
        }

        if(RV->VType == rt_FLOAT_COMPLEX ||
           RV->VType == rt_DOUBLE_COMPLEX)
           n = RV->VLength + RV->VLength;
        else
           n = RV->VLength;

        if(MPIInfoPrint && StatOff == 0)
           MPI_AllreduceTime -= dvm_time();

        MPI_Allreduce((void *)RV->Mem, ResBuf, n,
                      datatype, op, VMS->PS_MPI_COMM);

        if(MPIInfoPrint && StatOff == 0)
           MPI_AllreduceTime += dvm_time();

        if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
        {  tprintf("\n");
           tprintf("*** MPI_Allreduce ***\n");

           switch(RV->VType)
           {  case rt_INT:             CharPtr1 = "MPI_INT";      break;
              case rt_LONG:            CharPtr1 = "MPI_LONG";     break;
              case rt_LLONG:           CharPtr1 = "MPI_LONG_LONG";break;
              case rt_FLOAT:           CharPtr1 = "MPI_FLOAT";    break;
              case rt_DOUBLE:          CharPtr1 = "MPI_DOUBLE";   break;
              case rt_FLOAT_COMPLEX:   CharPtr1 = "MPI_FLOAT";    break;
              case rt_DOUBLE_COMPLEX:  CharPtr1 = "MPI_DOUBLE";   break;
           }

           switch(RV->Func)
           {  case rf_SUM:     CharPtr2 = "MPI_SUM";     break;
              case rf_MULT:    CharPtr2 = "MPI_PROD";    break;
              case rf_MAX:     CharPtr2 = "MPI_MAX";     break;
              case rf_MIN:     CharPtr2 = "MPI_MIN";     break;
              case rf_AND:     CharPtr2 = "MPI_BAND";    break;
              case rf_OR:      CharPtr2 = "MPI_BOR";     break;
              case rf_XOR:     CharPtr2 = "MPI_BXOR";    break;
           }

           tprintf("VarCount=%d; MPI_Datatype=%s; MPI_Op=%s; "
                   "&MPI_Comm=%lx;\n\n",
                   n, CharPtr1, CharPtr2, (uLLng)&VMS->PS_MPI_COMM);
        }

        k = RV->BlockSize;

        if(k > 16)
        {  SYSTEM(memcpy, (RV->Mem, ResBuf, k));
        }
        else
        {  CharPtr1 = RV->Mem;
           CharPtr2 = (char *)ResBuf;

           for(j=0; j < k; j++, CharPtr1++, CharPtr2++)
               *CharPtr1 = *CharPtr2;
        }
     }
  }
  else
  {  /* */    /*E0304*/

     for(i=0; i < Count; i++)
     {  RV = coll_At(s_REDVAR *, RGRV, i);

        RV->Already = 0; /* */    /*E0305*/
     }

     for(i=0; i < Count; i++)
     {  RV = coll_At(s_REDVAR *, RGRV, i);

        if(RV->Already)
           continue; /* */    /*E0306*/

        RV->Already = i+1; /* */    /*E0307*/
        k = RV->BlockSize;

        /* */    /*E0308*/

        for(j=i+1; j < Count; j++)
        {  RV1 = coll_At(s_REDVAR *, RGRV, j);

           if(RV1->Already)
              continue; /* */    /*E0309*/

           if(RV->VType != RV1->VType || RV->Func != RV1->Func)
              continue; /* */    /*E0310*/

           RV1->Already = i+1;
           RV1->CommonBlockSize = 0;/* */    /*E0311*/
           k += RV1->BlockSize;
        }

        RV->CommonBlockSize = k; /* */    /*E0312*/

        MaxBlockSize = dvm_max(MaxBlockSize, k);
     }

     if(RG->IsNewVars)
     {  mac_free(&RG->InitBuf);
        mac_free(&RG->ResBuf);

        RG->IsNewVars = 0;
     }

     if(RG->InitBuf == NULL)
     {
        mac_malloc(RG->InitBuf, void *, MaxBlockSize, 0);
     }

     if(RG->ResBuf == NULL)
     {
        mac_malloc(RG->ResBuf,  void *, MaxBlockSize, 0);
     }

     for(i=0; i < Count; i++)
     {  RV = coll_At(s_REDVAR *, RGRV, i);

        if(RV->CommonBlockSize == 0)
           continue; /* */    /*E0313*/

        switch(RV->VType)
        {  case rt_INT:             datatype = MPI_INT;      break;
           case rt_LONG:            datatype = MPI_LONG;     break;
           case rt_LLONG:           datatype = MPI_LONG_LONG;break;
           case rt_FLOAT:           datatype = MPI_FLOAT;    break;
           case rt_DOUBLE:          datatype = MPI_DOUBLE;   break;
           case rt_FLOAT_COMPLEX:   datatype = MPI_FLOAT;    break;
           case rt_DOUBLE_COMPLEX:  datatype = MPI_DOUBLE;   break;
        }

        switch(RV->Func)
        {  case rf_SUM:     op = MPI_SUM;     break;
           case rf_MULT:    op = MPI_PROD;    break;
           case rf_MAX:     op = MPI_MAX;     break;
           case rf_MIN:     op = MPI_MIN;     break;
           case rf_AND:     op = MPI_BAND;    break;
           case rf_OR:      op = MPI_BOR;     break;
           case rf_XOR:     op = MPI_BXOR;    break;
        }

        /* */    /*E0314*/

        CharPtr2 = (char *)RG->InitBuf;

        for(j=i; j < Count; j++)
        {  RV1 = coll_At(s_REDVAR *, RGRV, j);

           if(RV1->Already != RV->Already)
              continue; /* */    /*E0315*/

           k = RV1->BlockSize;

           if(k > 16)
           {  SYSTEM(memcpy, (CharPtr2, RV1->Mem, k));
              CharPtr2 += k;
           }
           else
           {  CharPtr1 = RV1->Mem;

              for(n=0; n < k; n++, CharPtr1++, CharPtr2++)
                  *CharPtr2 = *CharPtr1;
           }
        }

        /* */    /*E0316*/

        k = RV->CommonBlockSize / RV->RedElmLength; /* */    /*E0317*/
        if(RV->VType == rt_FLOAT_COMPLEX ||
           RV->VType == rt_DOUBLE_COMPLEX)
           k += k;

        if(MPIInfoPrint && StatOff == 0)
           MPI_AllreduceTime -= dvm_time();

        MPI_Allreduce(RG->InitBuf, RG->ResBuf, k,
                      datatype, op, VMS->PS_MPI_COMM);

        if(MPIInfoPrint && StatOff == 0)
           MPI_AllreduceTime += dvm_time();

        if(RTL_TRACE && MPI_ReduceTrace && TstTraceEvent(call_strtrd_))
        {  tprintf("\n");
           tprintf("*** MPI_Allreduce ***\n");

           switch(RV->VType)
           {  case rt_INT:             CharPtr1 = "MPI_INT";      break;
              case rt_LONG:            CharPtr1 = "MPI_LONG";     break;
              case rt_LLONG:           CharPtr1 = "MPI_LONG_LONG";break;
              case rt_FLOAT:           CharPtr1 = "MPI_FLOAT";    break;
              case rt_DOUBLE:          CharPtr1 = "MPI_DOUBLE";   break;
              case rt_FLOAT_COMPLEX:   CharPtr1 = "MPI_FLOAT";    break;
              case rt_DOUBLE_COMPLEX:  CharPtr1 = "MPI_DOUBLE";   break;
           }

           switch(RV->Func)
           {  case rf_SUM:     CharPtr2 = "MPI_SUM";     break;
              case rf_MULT:    CharPtr2 = "MPI_PROD";    break;
              case rf_MAX:     CharPtr2 = "MPI_MAX";     break;
              case rf_MIN:     CharPtr2 = "MPI_MIN";     break;
              case rf_AND:     CharPtr2 = "MPI_BAND";    break;
              case rf_OR:      CharPtr2 = "MPI_BOR";     break;
              case rf_XOR:     CharPtr2 = "MPI_BXOR";    break;
           }

           tprintf("VarCount=%d; MPI_Datatype=%s; MPI_Op=%s; "
                   "&MPI_Comm=%lx;\n\n",
                   k, CharPtr1, CharPtr2, (uLLng)&VMS->PS_MPI_COMM);
        }

        /* */    /*E0318*/

        CharPtr2 = (char *)RG->ResBuf;

        for(j=i; j < Count; j++)
        {  RV1 = coll_At(s_REDVAR *, RGRV, j);

           if(RV1->Already != RV->Already)
              continue; /* */    /*E0319*/

           k = RV1->BlockSize;

           if(k > 16)
           {  SYSTEM(memcpy, (RV1->Mem, CharPtr2, k));
              CharPtr2 += k;
           }
           else
           {  CharPtr1 = RV1->Mem;

              for(n=0; n < k; n++, CharPtr1++, CharPtr2++)
                  *CharPtr1 = *CharPtr2;
           }
        }
     }
  }

  RG->MPIReduce = 1;

  DVMMTimeFinish;

  return  0;
}

#endif


/* ------------------------------------------------- */    /*E0320*/


int  red_Sendnowait(void *buf, int count, int size, int procnum,
                    int tag, RTL_Request *RTL_ReqPtr, int MsgPartition)
{ int  rc;

  if(RTL_TRACE)
     dvm_trace(call_red_Sendnowait,"buf=%lx; count=%d; size=%d; "
               "req=%lx; procnum=%d(%d); procid=%d; tag=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
               ProcNumberList[procnum], ProcIdentList[procnum], tag);

  rc = ( RTL_CALL, rtl_Sendnowait(buf, count, size, procnum,
                                  tag, RTL_ReqPtr, MsgPartition) );

  if(RTL_TRACE)
     dvm_trace(ret_red_Sendnowait,"rc=%d; req=%lx; MsgPartition=%d;\n",
                                   rc, (uLLng)RTL_ReqPtr, MsgPartition);

  return  (DVM_RET, rc);
}



int  red_Recvnowait(void *buf, int count, int size, int procnum,
                    int tag, RTL_Request *RTL_ReqPtr)
{ int  rc;

  if(RTL_TRACE)
     dvm_trace(call_red_Recvnowait,"buf=%lx; count=%d; size=%d; "
               "req=%lx; procnum=%d(%d); procid=%ld; tag=%d;\n",
               (uLLng)buf, count, size, (uLLng)RTL_ReqPtr, procnum,
               ProcNumberList[procnum], ProcIdentList[procnum], tag);

  rc = ( RTL_CALL, rtl_Recvnowait(buf, count, size, procnum,
                                  tag, RTL_ReqPtr, 0) );

  if(RTL_TRACE)
     dvm_trace(ret_red_Recvnowait,"rc=%d; req=%lx;\n",
                                   rc, (uLLng)RTL_ReqPtr);

  return  (DVM_RET, rc);
}



void  red_Waitrequest(RTL_Request *RTL_ReqPtr)
{
  if(RTL_TRACE)
  {  int  procnum = RTL_ReqPtr->ProcNumber;

     dvm_trace(call_red_Waitrequest,
               "req=%lx; procnum=%d(%d); procid=%d;\n",
               (uLLng)RTL_ReqPtr, procnum, ProcNumberList[procnum],
               ProcIdentList[procnum]);
  }

  ( RTL_CALL, rtl_Waitrequest(RTL_ReqPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_red_Waitrequest,"req=%lx;\n", (uLLng)RTL_ReqPtr);

  (DVM_RET);

  return;
}



int  red_Testrequest(RTL_Request *RTL_ReqPtr)
{ int  rc;

  if(RTL_TRACE)
  {  int  procnum = RTL_ReqPtr->ProcNumber;

     dvm_trace(call_red_Testrequest,
               "req=%lx; procnum=%d(%d); procid=%d;\n",
               (uLLng)RTL_ReqPtr, procnum, ProcNumberList[procnum],
               ProcIdentList[procnum]);
  }

  rc = ( RTL_CALL, rtl_Testrequest(RTL_ReqPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_red_Testrequest,"rc=%d; req=%lx;\n",
                                    rc, (uLLng)RTL_ReqPtr);

  return  (DVM_RET, rc);
}


#endif   /*  _REDUCT_C_  */    /*E0321*/
