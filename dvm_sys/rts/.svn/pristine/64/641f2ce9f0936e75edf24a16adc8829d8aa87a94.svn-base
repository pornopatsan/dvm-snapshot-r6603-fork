#ifndef _GENV_C_
#define _GENV_C_
/**************/    /*E0000*/


SysHandle sysh__Build(byte Type, int EnvInd, int CrtEnvInd, DvmType lP, void *pP)
{ SysHandle Res;

  Res.HeaderPtr       = (uLLng)Type;
  Res.NextHandlePtr   = NULL;
  Res.Type            = Type;
  Res.EnvInd          = EnvInd;
  Res.CrtEnvInd       = CrtEnvInd;
  Res.InitCrtBlockInd = -1;         /* */    /*E0001*/
  Res.CrtBlockInd     = -1;         /* */    /*E0002*/
  Res.CrtAMHandlePtr  = (void *)CurrAMHandlePtr;
  Res.lP              = lP;
  Res.pP              = pP;

  return  Res;
}



s_ENVIRONMENT  *genv_GetEnvironment(int EnvInd)
{
   s_ENVIRONMENT   *Env = NULL;

   if(EnvInd >=0 && EnvInd < gEnvColl->Count)
      Env = coll_At(s_ENVIRONMENT *, gEnvColl, EnvInd);

   return  Env;
}



SysHandle  genv_InsertObject(byte ObjType, void *Obj)
{
  SysHandle          Res;
  s_DISARRAY        *DArrDesc;
  s_VMS             *VMSDesc;
  s_AMVIEW          *AMVDesc;
  s_BOUNDGROUP      *BGDesc;
  s_REDVAR          *RVDesc;
  s_REDGROUP        *RGDesc;
  s_ARRAYMAP        *ArrMapDesc;
  s_AMVIEWMAP       *AMVMapDesc;
  s_REGBUFGROUP     *RBGDesc;
  s_DACONSISTGROUP  *DAGDesc;
  s_IDBUFGROUP      *IBGDesc;

  byte               Static;
  s_ENVIRONMENT     *Env;
  s_COLLECTION      *ObjColl = NULL;
  int                EnvInd;
  
  EnvInd = gEnvColl->Count - 1; /* current context index */    /*E0003*/
  Env  = coll_At(s_ENVIRONMENT *, gEnvColl, EnvInd); /* current context */    /*E0004*/

  switch (ObjType)
  {
     case sht_DisArray:

                           DArrDesc = (s_DISARRAY *)Obj;
                           DArrDesc->EnvInd = EnvInd;
                           DArrDesc->CrtEnvInd = EnvInd;
                           DArrDesc->HandlePtr = NULL;
                           Static = DArrDesc->Static;

                           if(Static)
                              DArrDesc->EnvInd = -1; 

                           ObjColl = &(Env->DisArrList);

                           break;

     case sht_VMS:

                           VMSDesc = (s_VMS *)Obj;
                           VMSDesc->EnvInd = EnvInd;
                           VMSDesc->CrtEnvInd = EnvInd;
                           VMSDesc->HandlePtr = NULL;
                           Static = VMSDesc->Static;

                           if(Static)
                              VMSDesc->EnvInd = -1; 

                           ObjColl = &(Env->VMSList);

                           break;

     case sht_AMView:

                           AMVDesc = (s_AMVIEW *)Obj;
                           AMVDesc->EnvInd = EnvInd;
                           AMVDesc->CrtEnvInd = EnvInd;
                           AMVDesc->HandlePtr = NULL;
                           Static = AMVDesc->Static;

                           if(Static)
                              AMVDesc->EnvInd = -1; 

                           ObjColl = &(Env->AMViewList);

                           break;

     case sht_BoundsGroup:

                           BGDesc = (s_BOUNDGROUP *)Obj;
			   BGDesc->EnvInd = EnvInd;
			   BGDesc->CrtEnvInd = EnvInd;
			   BGDesc->HandlePtr = NULL;
                           Static = BGDesc->Static;

                           if(Static)
                              BGDesc->EnvInd = -1; 

			   ObjColl = &(Env->BoundGroupList);

			   break;

     case sht_RedVar:

                           RVDesc = (s_REDVAR *)Obj;
                           RVDesc->EnvInd = EnvInd;
                           RVDesc->CrtEnvInd = EnvInd;
                           RVDesc->HandlePtr = NULL;
                           Static = RVDesc->Static;

                           if(Static)
                              RVDesc->EnvInd = -1; 

                           ObjColl = &(Env->RedVars);

                           break;

     case sht_RedGroup:

                           RGDesc = (s_REDGROUP *)Obj;
                           RGDesc->EnvInd = EnvInd;
                           RGDesc->CrtEnvInd = EnvInd;
                           RGDesc->EnvDiff =
                           ( Env->ParLoop != NULL &&
                             Env->ParLoop->Set == NULL ) ? 1 : 0;
                           RGDesc->HandlePtr = NULL;
                           Static = RGDesc->Static;

                           if(Static)
                              RGDesc->EnvInd = -1; 

                           ObjColl = &(Env->RedGroups);

                           break;

     case sht_ArrayMap:

                           ArrMapDesc = (s_ARRAYMAP *)Obj;
                           ArrMapDesc->EnvInd = EnvInd;
                           ArrMapDesc->CrtEnvInd = EnvInd;
                           ArrMapDesc->HandlePtr = NULL;
                           Static = ArrMapDesc->Static;

                           if(Static)
                              ArrMapDesc->EnvInd = -1; 

                           ObjColl = &(Env->ArrMaps);

                           break;

     case sht_AMViewMap:

                           AMVMapDesc = (s_AMVIEWMAP *)Obj;
                           AMVMapDesc->EnvInd = EnvInd;
                           AMVMapDesc->CrtEnvInd = EnvInd;
                           AMVMapDesc->HandlePtr = NULL;
                           Static = AMVMapDesc->Static;

                           if(Static)
                              AMVMapDesc->EnvInd = -1; 

                           ObjColl = &(Env->AMVMaps);

                           break;

     case sht_RegBufGroup:

                           RBGDesc = (s_REGBUFGROUP *)Obj;
                           RBGDesc->EnvInd = EnvInd;
                           RBGDesc->CrtEnvInd = EnvInd;
                           RBGDesc->HandlePtr = NULL;
                           Static = RBGDesc->Static;

                           if(Static)
                              RBGDesc->EnvInd = -1; 

                           ObjColl = &(Env->RegBufGroups);

                           break;

     case sht_DAConsistGroup:

                              DAGDesc = (s_DACONSISTGROUP *)Obj;
                              DAGDesc->EnvInd = EnvInd;
                              DAGDesc->CrtEnvInd = EnvInd;
                              DAGDesc->HandlePtr = NULL;
                              Static = DAGDesc->Static;

                              if(Static)
                                 DAGDesc->EnvInd = -1; 

                              ObjColl = &(Env->DAConsistGroups);

                              break;

     case sht_IdBufGroup:

                           IBGDesc = (s_IDBUFGROUP *)Obj;
                           IBGDesc->EnvInd = EnvInd;
                           IBGDesc->CrtEnvInd = EnvInd;
                           IBGDesc->HandlePtr = NULL;
                           Static = IBGDesc->Static;

                           if(Static)
                              IBGDesc->EnvInd = -1; 

                           ObjColl = &(Env->IdBufGroups);

                           break;
  }

  if(ObjColl && Static == 0)
  {  coll_Insert(ObjColl, Obj);
     Res = sysh_Build(ObjType, EnvInd, EnvInd, 0, Obj);
  }
  else
     Res = sysh_Build(ObjType, -1, EnvInd, 0, Obj);

  if(ObjColl)
  {  Res.InitCrtBlockInd = Env->PrgBlock.Count; /* */    /*E0005*/
     Res.CrtBlockInd     = Env->PrgBlock.Count; /* */    /*E0006*/
  }

  return  Res;
}



/* ENVIRONMENT */    /*E0007*/


s_ENVIRONMENT  *env_Init(SysHandle *AMHandlePtr)
{
  s_ENVIRONMENT   *Env;

  dvm_AllocStruct(s_ENVIRONMENT, Env);

  Env->AMHandlePtr = AMHandlePtr; 

  Env->PrgBlock = coll_Init(PrgBlockCount , PrgBlockCount, NULL);

  /* Lists of created objects in current program environment */    /*E0008*/

#ifndef _DVM_IOPROC_

  Env->VMSList       = coll_Init(VMSCount, VMSCount, vms_Done);
  Env->AMViewList    = coll_Init(AMVCount, AMVCount, amview_Done);
  Env->DisArrList    = coll_Init(DisArrCount, DisArrCount, disarr_Done);
  Env->BoundGroupList= coll_Init(BoundGrpCount, BoundGrpCount,
                                 bgroup_Done);
  Env->RedVars       = coll_Init(RedVarCount, RedVarCount, RedVar_Done);
  Env->RedGroups     = coll_Init(RedGrpCount, RedGrpCount,
                                 RedGroup_Done);
  Env->ArrMaps       = coll_Init(ArrMapCount, ArrMapCount, ArrMap_Done);
  Env->AMVMaps       = coll_Init(AMVMapCount, AMVMapCount, AMVMap_Done);
  Env->RegBufGroups  = coll_Init(RegBufGrpCount, RegBufGrpCount,
                                 RegBufGroup_Done);
  Env->DAConsistGroups = coll_Init(ConsistDAGrpCount, ConsistDAGrpCount,
                                   DAConsistGroup_Done);
  Env->IdBufGroups   = coll_Init(IdBufGrpCount, IdBufGrpCount,
                                 IdBufGroup_Done);
#else

  Env->VMSList       = coll_Init(VMSCount, VMSCount, NULL);
  Env->AMViewList    = coll_Init(AMVCount, AMVCount, NULL);
  Env->DisArrList    = coll_Init(DisArrCount, DisArrCount, NULL);
  Env->BoundGroupList= coll_Init(BoundGrpCount, BoundGrpCount, NULL);
  Env->RedVars       = coll_Init(RedVarCount, RedVarCount, NULL);
  Env->RedGroups     = coll_Init(RedGrpCount, RedGrpCount, NULL);
  Env->ArrMaps       = coll_Init(ArrMapCount, ArrMapCount, NULL);
  Env->AMVMaps       = coll_Init(AMVMapCount, AMVMapCount, NULL);
  Env->RegBufGroups  = coll_Init(RegBufGrpCount, RegBufGrpCount, NULL);
  Env->DAConsistGroups = coll_Init(ConsistDAGrpCount, ConsistDAGrpCount,
                                   NULL);
  Env->IdBufGroups   = coll_Init(IdBufGrpCount,IdBufGrpCount, NULL);

#endif

  Env->ParLoop = NULL; /* pointer to patallel loop */    /*E0009*/

  return  Env;
}



void  env_Done(s_ENVIRONMENT  *Env)
{ 
  if(RTL_TRACE)
     dvm_trace(call_env_Done,
               "AMRef=%lx; CurrEnvIndex=%d;\n",
               (uLLng)Env->AMHandlePtr, (int)(gEnvColl->Count-1));

  coll_Done(&(Env->PrgBlock));

  if(RgSave)
  {  dvm_FreeArray(Env->RedGroups.List);

     Env->RedGroups.Count    = 0;
     Env->RedGroups.Reserv   = 0;
     Env->RedGroups.CountInc = 0;

     dvm_FreeArray(Env->RedVars.List);

     Env->RedVars.Count    = 0;
     Env->RedVars.Reserv   = 0;
     Env->RedVars.CountInc = 0;
  }
  else
  {  coll_Done(&(Env->RedGroups));
     coll_Done(&(Env->RedVars));
  }

  if(ShgSave)
  {  dvm_FreeArray(Env->BoundGroupList.List);

     Env->BoundGroupList.Count    = 0;
     Env->BoundGroupList.Reserv   = 0;
     Env->BoundGroupList.CountInc = 0;
  }
  else
     coll_Done(&(Env->BoundGroupList));

  coll_Done(&(Env->RegBufGroups));
  coll_Done(&(Env->DAConsistGroups));
  coll_Done(&(Env->IdBufGroups));
  coll_Done(&(Env->ArrMaps));
  coll_Done(&(Env->DisArrList));
  coll_Done(&(Env->AMVMaps));
  coll_Done(&(Env->AMViewList));
  coll_Done(&(Env->VMSList));

#ifndef _DVM_IOPROC_

  if(Env->ParLoop != NULL)
  {  ( RTL_CALL, parloop_Done(Env->ParLoop) );
     dvm_FreeStruct(Env->ParLoop);
  }

#endif

  if(RTL_TRACE)
     dvm_trace(ret_env_Done," \n");

  (DVM_RET);

  return;
}


#endif /* _GENV_C_ */    /*E0010*/
