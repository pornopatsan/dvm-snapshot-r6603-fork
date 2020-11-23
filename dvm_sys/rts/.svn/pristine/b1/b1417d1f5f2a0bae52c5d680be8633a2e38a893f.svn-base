#ifndef _PRGBLOCK_C_
#define _PRGBLOCK_C_
/******************/    /*E0000*/

/************************************\
* Functions to define program blocks *
\************************************/    /*E0001*/

DvmType __callstd begbl_(void)
/*
     Program blocks. Begin of block.
     -------------------------------

The function defines the beginning of the localization block for the
following system objects:

  -  distributed array;
  -  abstract machine representation;
  -  processor system;
  -  allocation map of the distributed array;
  -  allocation map of the abstract machine representation;
  -  reduction;
  -  reduction group;
  -  shadow edge group of the distributed arrays.

The function returns zero.
*/    /*E0002*/
{ s_ENVIRONMENT   *Env;
  s_PRGBLOCK      *PB;

  DVMFTimeStart(call_begbl_);

  if(RTL_TRACE)
     dvm_trace(call_begbl_," \n");

  Env = genv_GetCurrEnv();

  dvm_AllocStruct(s_PRGBLOCK, PB);

  PB->BlockInd = Env->PrgBlock.Count + 1; /* */    /*E0003*/

  PB->ind_VMS          = Env->VMSList.Count-1;
  PB->ind_AMView       = Env->AMViewList.Count-1;
  PB->ind_DisArray     = Env->DisArrList.Count-1;
  PB->ind_BoundsGroup  = Env->BoundGroupList.Count-1;
  PB->ind_RedVars      = Env->RedVars.Count-1;
  PB->ind_RedGroups    = Env->RedGroups.Count-1;
  PB->ind_ArrMaps      = Env->ArrMaps.Count-1;
  PB->ind_AMVMaps      = Env->AMVMaps.Count-1;
  PB->ind_RegBufGroups = Env->RegBufGroups.Count-1;
  PB->ind_DAConsistGroups = Env->DAConsistGroups.Count-1;
  PB->ind_IdBufGroups  = Env->IdBufGroups.Count-1;

  coll_Insert(&Env->PrgBlock, PB);

  if(RTL_TRACE)
     dvm_trace(ret_begbl_," \n");

  DVMFTimeFinish(ret_begbl_);
  return  (DVM_RET, 0);
}



DvmType __callstd endbl_(void)
/*
     Program blocks. End of block.
     -----------------------------

The function endbl_ marks the end of the localization block.
The function returns zero.
*/    /*E0004*/
{ s_ENVIRONMENT  *Env;
  s_PRGBLOCK     *PB;

  DVMFTimeStart(call_endbl_);

  if(RTL_TRACE)
     dvm_trace(call_endbl_," \n");

  Env = genv_GetCurrEnv();

  if(Env->PrgBlock.Count == 0)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 050.000: wrong call endbl_ "
              "(Prog Block Count = 0)\n");
 
  PB = coll_At(s_PRGBLOCK *, &Env->PrgBlock, Env->PrgBlock.Count - 1);

  coll_ObjFreeFrom(&Env->RedGroups, sht_RedGroup, PB->BlockInd);
  coll_ObjFreeFrom(&Env->RedVars, sht_RedVar, PB->BlockInd);
  coll_ObjFreeFrom(&Env->BoundGroupList, sht_BoundsGroup, PB->BlockInd);
  coll_ObjFreeFrom(&Env->RegBufGroups, sht_RegBufGroup, PB->BlockInd);
  coll_ObjFreeFrom(&Env->DAConsistGroups, sht_DAConsistGroup,
                   PB->BlockInd);
  coll_ObjFreeFrom(&Env->IdBufGroups, sht_IdBufGroup, PB->BlockInd);
  coll_ObjFreeFrom(&Env->ArrMaps, sht_ArrayMap, PB->BlockInd);
  coll_ObjFreeFrom(&Env->DisArrList, sht_DisArray, PB->BlockInd);
  coll_ObjFreeFrom(&Env->AMVMaps, sht_AMViewMap, PB->BlockInd);
  coll_ObjFreeFrom(&Env->AMViewList, sht_AMView, PB->BlockInd);
  coll_ObjFreeFrom(&Env->VMSList, sht_VMS, PB->BlockInd);

  coll_Free(&Env->PrgBlock, PB);

  if(RTL_TRACE)
     dvm_trace(ret_endbl_," \n");

  DVMFTimeFinish(ret_endbl_);
  return  (DVM_RET, 0);
}


#endif   /* _PRGBLOCK_C_ */    /*E0005*/
