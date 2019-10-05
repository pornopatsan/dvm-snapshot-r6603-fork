#ifndef _AMS_C_
#define _AMS_C_
/*************/    /*E0000*/

/*******************************************\
* Functions of an abstract machine creation *
\*******************************************/    /*E0001*/

AMRef __callstd getam_(void)
/*
      Requesting current abstract machine.
      ------------------------------------

This function returns a reference to current abstract machine.
*/    /*e0002*/
{ AMRef  Res;

  DVMFTimeStart(call_getam_);

  if(RTL_TRACE)
     dvm_trace(call_getam_,"\n");

  Res = (AMRef)CurrAMHandlePtr;
 
  if(RTL_TRACE)
     dvm_trace(ret_getam_,"AMRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0003*/
  DVMFTimeFinish(ret_getam_);
  return  (DVM_RET, Res);
}



AMViewRef __callstd crtamv_(AMRef *AMRefPtr,DvmType *RankPtr,
                            DvmType SizeArray[], DvmType *StaticSignPtr)
/*
          Creating abstract machine representation.
          -----------------------------------------          

*AMRefPtr       - a reference to the abstract machine.
*RankPtr        - a rank of created representation.
SizeArray       - vector of the sizes of the representation dimensions.
                  SizeArray[i] is a size of the i+1 dimension
                 (0 <= i <= *RankPtr-1).
*StaticSignPtr	- the flag that defines whether to create
                  static representation or not.

The function creates a representation of the assigned abstract machine
as an array of abstract machines of the next hierarchy level.
The function returns reference to the created representation.
*/    /*e0004*/
{ AMViewRef       Res;
  SysHandle      *AMViewPtr, *AMHandlePtr;
  s_AMVIEW       *AMV;
  int             i;
  s_AMS          *AMS, *CAMS;

  if(AMRefPtr == NULL)
     StatObjectRef = 0;                    /* for statistics */    /*E0005*/
  else
     StatObjectRef = (ObjectRef)*AMRefPtr; /* for statistics */    /*E0006*/

  DVMFTimeStart(call_crtamv_);

  if(RTL_TRACE)
  {  if(AMRefPtr == NULL || *AMRefPtr == 0)
        dvm_trace(call_crtamv_,
                "AMRefPtr=NULL; AMRef=0; Rank=%ld; StaticSign=%ld;\n",
                *RankPtr, *StaticSignPtr);
     else
        dvm_trace(call_crtamv_,
                "AMRefPtr=%lx; AMRef=%lx; Rank=%ld; StaticSign=%ld;\n",
                (uLLng)AMRefPtr, *AMRefPtr, *RankPtr, *StaticSignPtr);

     if(TstTraceEvent(call_crtamv_))
     {  for(i=0; i < *RankPtr; i++)
            tprintf("SizeArray[%d]=%ld; ", i, SizeArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  if(AMRefPtr == NULL || *AMRefPtr == 0)
     AMHandlePtr = CurrAMHandlePtr;
  else
  {
     if(TstObject)
     {  if(TstDVMObj(AMRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 030.000: wrong call crtamv_\n"
             "(the abstract machine is not a DVM object; "
             "AMRef=%lx)\n", *AMRefPtr);
     }

     AMHandlePtr = (SysHandle *)*AMRefPtr;

     if(AMHandlePtr->Type != sht_AMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 030.001: wrong call crtamv_\n"
               "(the object is not an abstract machine; AMRef=%lx)\n",
               *AMRefPtr);

     AMS  = (s_AMS *)AMHandlePtr->pP;        /* parent AM*/    /*E0007*/
     CAMS = (s_AMS *)CurrAMHandlePtr->pP;    /* current AM*/    /*E0008*/

     /* Check if parent machine is 
         current machine descendant */    /*E0009*/

     NotDescendant(i, CAMS, AMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 030.002: wrong call crtamv_\n"
                 "(the parental abstract machine is not a descendant "
                 "of the current abstract machine;\n"
                 "AMRef=%lx; CurrentAMRef=%lx)\n",
                 (uLLng)AMHandlePtr, (uLLng)CAMS->HandlePtr);
  }

  dvm_AllocStruct(SysHandle, AMViewPtr);
  dvm_AllocStruct(s_AMVIEW, AMV);

  AMV->Static      = (byte)*StaticSignPtr;
  AMV->Space       = space_Init((byte)*RankPtr,SizeArray);
  AMV->AMHandlePtr = AMHandlePtr; /* pointer to parent 
                                     abstract machine Handle*/    /*E0010*/
  AMV->VMS         = NULL;
  AMV->DISTMAP     = NULL;
  AMV->Repl        = 0;
  AMV->PartRepl    = 0;
  AMV->Every       = 0;
  AMV->HasLocal    = 0;
  AMV->ArrColl     = coll_Init(AMVArrCount, AMVArrCount, NULL);
  AMV->AMSColl     = coll_Init(AMVAMSCount, AMVAMSCount, NULL);

  AMV->IsGetAMR = 0; /* */    /*E0011*/
  AMV->DVM_LINE = DVM_LINE[0]; /* */    /*E0012*/
  AMV->DVM_FILE = DVM_FILE[0]; /* */    /*E0013*/

  /* Arrays of processor coordinate weights 
     and arrays of summary previous processor coordinate weights*/    /*E0014*/
  
  AMV->WeightVMS = NULL;  /* pointer to processor system
                             for which weight coordinates are defined */    /*E0015*/

  for(i=0; i < MAXARRAYDIM; i++)
  {  AMV->CoordWeight[i] = NULL;
     AMV->PrevSumCoordWeight[i] = NULL;
     AMV->GenBlockCoordWeight[i] = NULL;
     AMV->PrevSumGenBlockCoordWeight[i] = NULL;
  }
  AMV->setCW=0;
  AMV->disWeightVMS = NULL;  /* pointer to processor system
                             for which weight coordinates are defined */    /*E0015*/

  for(i=0; i < MAXARRAYDIM; i++)
  {  AMV->disCoordWeight[i] = NULL;
     AMV->disPrevSumCoordWeight[i] = NULL;
     AMV->disGenBlockCoordWeight[i] = NULL;
     AMV->disPrevSumGenBlockCoordWeight[i] = NULL;
  }

  /* information about measurement of the time 
     of loop iteration group execution mapped on the given representation */    /*E0016*/

  AMV->TimeMeasure = 0;

  for(i=0; i < MAXARRAYDIM; i++)
  {  AMV->GroupNumber[i]      = 0;
     AMV->GroupWeightArray[i] = NULL;
     AMV->Is_gettar[i]        = 0;
     AMV->Div[i]              = 1;
     AMV->disDiv[i]              = 1;
  }

  AMV->DivReset = 0;

  /* ------------------------------------------------------- */    /*E0017*/ 

  *AMViewPtr = genv_InsertObject(sht_AMView, AMV);
  AMV->HandlePtr = AMViewPtr; /* pointer to own Handle */    /*E0018*/

  AMS = (s_AMS *)AMHandlePtr->pP;    /* parent AM */    /*E0019*/
  coll_Insert(&AMS->SubSystem, AMV); /* to the list of abstract machine 
                                        representations */    /*E0020*/

  if(TstObject)
     InsDVMObj((ObjectRef)AMViewPtr);

  Res=(AMViewRef)AMViewPtr;

  if(RTL_TRACE)
     dvm_trace(ret_crtamv_,"AMViewRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0021*/
  DVMFTimeFinish(ret_crtamv_);
  return  (DVM_RET, Res);
}



AMRef  __callstd  getamr_(AMViewRef  *AMViewRefPtr, DvmType        IndexArray[])
{ AMRef       Res;
  SysHandle  *AMVHandlePtr, *AMSHandlePtr;
  s_AMVIEW   *AMV;
  int         i, Rank;
  DvmType        LinInd;
  DvmType        SpindIndex[MAXARRAYDIM + 1];
  s_AMS      *AMS, *PAMS;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0022*/
  DVMFTimeStart(call_getamr_);

  if(RTL_TRACE)
     dvm_trace(call_getamr_,"AMViewRefPtr=%lx; AMViewRef=%lx;\n",
                            (uLLng)AMViewRefPtr, *AMViewRefPtr);

  if(TstObject)
  {  if(TstDVMObj(AMViewRefPtr) == 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 030.020: wrong call getamr_\n"
                 "(the representation is not a DVM object; "
                 "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 030.021: wrong call getamr_\n"
              "(the object is not an abstract machine representation; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  Rank = AMV->Space.Rank;

  if(RTL_TRACE)
  {  if(TstTraceEvent(call_getamr_))
     {  for(i=0; i < Rank; i++)
            tprintf("IndexArray[%d]=%ld; ", i, IndexArray[i]);
        tprintf(" \n");
        tprintf(" \n");
     }
  }

  /* Check correctness of abstract machine indexes */    /*E0023*/

  for(i=0; i < Rank; i++)
  {  if(IndexArray[i] >= AMV->Space.Size[i])
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 030.022: wrong call getamr_\n"
                 "(IndexArray[%d]=%ld >= %ld; AMViewRef=%lx)\n",
                 i, IndexArray[i], AMV->Space.Size[i], *AMViewRefPtr);
  }

  for(i=0; i < Rank; i++)
  {  if(IndexArray[i] < 0)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 030.023: wrong call getamr_\n"
                 "(IndexArray[%d]=%ld < 0; AMViewRef=%lx)\n",
                 *AMViewRefPtr, i, IndexArray[i]);
  }

  /* Caculate abstract machine linear index */    /*E0024*/

  SpindIndex[0] = Rank;

  for(i=0; i < Rank; i++)
      SpindIndex[i+1] = IndexArray[i];

  LinInd = space_GetLI(&AMV->Space, SpindIndex);

  /* Check if an abstract machine with calculated
           linear index  exists */    /*E0025*/

  for(i=0; i < AMV->AMSColl.Count; i++)
  {
     AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

     if(AMS->HandlePtr->lP == LinInd)
        break;   /* requested abstract machine has already been created */    /*E0026*/
  }

  if(i != AMV->AMSColl.Count)
     Res = (AMRef)AMS->HandlePtr; /* return reference to existing
                                     abstract machine */    /*E0027*/
  else
  {
     /* Requested abstract machine is not exist */    /*E0028*/

     dvm_AllocStruct(SysHandle, AMSHandlePtr);
     dvm_AllocStruct(s_AMS, AMS);

     *AMSHandlePtr = sysh_Build(sht_AMS, gEnvColl->Count-1,
                                AMV->CrtEnvInd, LinInd, AMS);

     AMS->EnvInd = AMSHandlePtr->EnvInd; /* index of current context */    /*E0029*/
     AMS->CrtEnvInd = AMSHandlePtr->CrtEnvInd; /* index of context where
                                                  the object has been created */    /*E0030*/
     AMS->HandlePtr = AMSHandlePtr; /* pointer to the own Handle */    /*E0031*/
     AMS->ParentAMView = AMV; /* pointer to representation of
                                a parent abstract machine */    /*E0032*/
     AMS->VMS = NULL; /* abstract machine is not mapped */    /*E0033*/
     AMS->SubSystem = coll_Init(AMSAMVCount, AMSAMVCount, NULL);

     coll_Insert(&AMV->AMSColl, AMS); /* to the list of
                                         created abstract machine */    /*E0034*/
     PAMS = (s_AMS *)(AMV->AMHandlePtr->pP);
     AMS->TreeIndex = PAMS->TreeIndex + 1; /* distance to the abstract
                                              machine tree root */    /*E0035*/
     AMS->ExecTime = 0.; /* */    /*E0036*/
     AMS->RunTime  = 0.; /* */    /*E0037*/
     AMS->IsMapAM  = 0;  /* */    /*E0038*/

     if(TstObject)
        InsDVMObj((ObjectRef)AMSHandlePtr);

     Res = (AMRef)AMSHandlePtr;
  }

  AMV->IsGetAMR = 1; /* */    /*E0039*/

  /* Create processor system and map  
     an abstract machine on it */    /*E0040*/

  if(AMV->VMS && AMS->VMS == NULL)
     /* Parent abstract machine representation is mapped 
        by distr_ function, 
        but requested abstract machine is not mapped */    /*E0041*/

     CrtTaskForAMS(AMS); /* Create processor system and map  
                            requested abstract machine on it */    /*E0042*/     
  
  if(RTL_TRACE)
     dvm_trace(ret_getamr_,"AMRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res; /* for statistics */    /*E0043*/
  DVMFTimeFinish(ret_getamr_);
  return  (DVM_RET, Res);
}



#ifndef _DVM_IOPROC_

DvmType  __callstd delamv_(AMViewRef  *AMViewRefPtr)
/*
      Deleting abstract machine representation.
      -----------------------------------------

*AMViewRefPtr - reference to the abstract machine representation.

The function deletes an abstract machine representation,
created by function crtmv_.
The function returns zero.
*/    /*e0044*/
{ SysHandle  *AMVHandlePtr;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0045*/
  DVMFTimeStart(call_delamv_);

  if(RTL_TRACE)
     dvm_trace(call_delamv_,"AMViewRefPtr=%lx; AMViewRef=%lx;\n",
                            (uLLng)AMViewRefPtr, *AMViewRefPtr);

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 030.010: wrong call delamv_\n"
            "(the representation is not a DVM object; "
            "AMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr=(SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 030.011: wrong call delamv_\n"
           "(the object is not an abstract machine representation; "
           "AMViewRef=%lx)\n", *AMViewRefPtr);

  ( RTL_CALL, delobj_(AMViewRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delamv_," \n");

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0046*/
  DVMFTimeFinish(ret_delamv_);
  return  (DVM_RET, 0);
}

#endif


/* ----------------------------------------------------------- */    /*E0047*/


/********************************************************************\
* Create processor system for a given element of parent abstract     *
* machine representation mapped by distr_ function,                  *
* mapping the element on the processor system                        *
\********************************************************************/    /*E0048*/

void  CrtTaskForAMS(s_AMS  *AMS)
{ s_VMS       *VMS;
  PSRef        VMRef, NewVMRef;
  int          i, j, VMRank, AMVRank, MapArrSize, ALSize;
  s_MAP       *MapArray;
  DvmType        *SizeArray, *Lower, *Upper;
  DvmType         Static;
  s_AMVIEW    *AMV;
  s_BLOCK    **LocBlockArray;
  s_BLOCK     *LocBlock;
  s_ALIGN     *AlignList;

  AMV = AMS->ParentAMView; /* representation which contain the
                              abstract machine */    /*E0049*/
  if(AMV == NULL || AMS->VMS)
     return;  /* abstract machine is already mapped
                 or not belong to any representation */    /*E0050*/
  VMS = AMV->VMS; /* processor system on which
                     a representation has been mapped */    /*E0051*/
  if(VMS == NULL)
     return;      /* representation containing an abstract machine
                     is not mapped */    /*E0052*/

  VMRef      = (PSRef)VMS->HandlePtr; /* reference to the processor system
                                         with subsystem to be created */    /*E0053*/
  Static     = AMV->Static;      /* representation inherits a static 
                                    abstract machine attribute */    /*E0054*/
  VMRank     = VMS->Space.Rank;  /* processor system dimension */    /*E0055*/
  AMVRank    = AMV->Space.Rank;  /* representation dimension */    /*E0056*/
  MapArray   = AMV->DISTMAP;     /* map of representation */    /*E0057*/
  MapArrSize = VMRank + AMVRank; /* size of representation map */    /*E0058*/
  
  dvm_AllocArray(DvmType, MapArrSize, SizeArray);
  dvm_AllocArray(DvmType, MapArrSize, Lower);
  dvm_AllocArray(DvmType, MapArrSize, Upper);

  /* define created processor system dimensions */    /*E0059*/

  for(i=0; i < VMRank; i++)
  {  if(MapArray[i+AMVRank].Attr == map_REPLICATE)
        SizeArray[i] = VMS->Space.Size[i]; /* if replication is 
                                              along (i+1)-th dimension */    /*E0060*/
     else
        SizeArray[i] = 1;
  }

  /* Create the map of equivalent  alignment */    /*E0061*/

  ALSize = 2 * AMVRank;

  dvm_AllocArray(s_ALIGN, ALSize, AlignList);

  /* Create two parts of AlignList 
     for equivalent alignment */    /*E0062*/

  for(i=0; i < AMVRank; i++)
  {  AlignList[i].Attr  = align_NORMAL;
     AlignList[i].Axis  = (byte)(i+1);
     AlignList[i].TAxis = (byte)(i+1);
     AlignList[i].A     = 1;
     AlignList[i].B     = 0;
     AlignList[i].Bound = 0;
  }

  for(i=AMVRank; i < ALSize; i++)
  {  AlignList[i].Attr  = align_NORMTAXIS;
     AlignList[i].Axis  = (byte)(i-AMVRank+1);
     AlignList[i].TAxis = (byte)(i-AMVRank+1);
     AlignList[i].A     = 0;
     AlignList[i].B     = 0;
     AlignList[i].Bound = 0;
  }

  /* Create local blocks for all processors */    /*E0063*/

  dvm_AllocArray(s_BLOCK *, VMS->ProcCount, LocBlockArray);
  dvm_AllocArray(s_BLOCK, VMS->ProcCount, LocBlock);

  for(i=0; i < VMS->ProcCount; i++)
      LocBlockArray[i] = GetSpaceLB4Proc(i, AMV, &AMV->Space,
                                         AlignList, NULL, &LocBlock[i]);

  /* Ctreate processor system and map an abstract machine on it */    /*E0064*/

  Lower[0] = AMVRank;
  space_GetSI(&AMV->Space, AMS->HandlePtr->lP, &Lower);

  /* Find processor on which the abstract machine is mapped */    /*E0065*/

  for(i=0; i < VMS->ProcCount; i++)      /* loop in processor */    /*E0066*/
  {  IsElmOfBlock(j, LocBlockArray[i], &Lower[1])

     if(LocBlockArray[i] != NULL && j)
     {  /* The abstract machine is mapped on i-th processor */    /*E0067*/

        /* Define processor coordinate initial value
           for each dimension */    /*E0068*/

        Upper[0] = VMRank;
        space_GetSI(&VMS->Space, i, &Upper);

        for(j=0; j < VMRank; j++)
        {  if(MapArray[j+AMVRank].Attr == map_REPLICATE)
              Lower[j] = 0;
           else
              Lower[j] = Upper[j+1];
        }

        /* Create processor system */    /*E0069*/

        for(j=0; j < VMRank; j++)
            Upper[j] = Lower[j] + SizeArray[j] - 1;

        NewVMRef = ( RTL_CALL, crtps_(&VMRef, Lower, Upper, &Static) );

        /* Mapping the abstract machine on 
           the new processor system */    /*E0070*/

        AMS->VMS = (s_VMS *)((SysHandle *)NewVMRef)->pP;
        coll_Insert(&AMS->VMS->AMSColl, AMS);/* to the list of machines
                                                mapped on the processor system */    /*E0071*/
        break;   /* leave the loop of processor search */    /*E0072*/
     }
  }

  /* Free memory */    /*E0073*/

  dvm_FreeArray(AlignList);

  dvm_FreeArray(LocBlockArray);
  dvm_FreeArray(LocBlock);

  dvm_FreeArray(SizeArray);
  dvm_FreeArray(Lower);
  dvm_FreeArray(Upper);

  return;
}


/* ----------------------------------------------------- */    /*E0074*/


#ifndef _DVM_IOPROC_

void  amview_Done(s_AMVIEW  *AMV)
{ s_DISARRAY   *wDArr;
  int           i;
  s_AMS        *AMS;
  s_AMVIEW     *wAMV;
  AMViewRef     AMVRef;
  PSRef         VMRef;

  if(RTL_TRACE)
     dvm_trace(call_amview_Done,"AMViewRef=%lx;\n", (uLLng)AMV->HandlePtr);

  dvm_FreeArray(AMV->DISTMAP);

  /* Delete representation from representation list */    /*E0075*/

  if(AMV->VMS)
     coll_Delete(&AMV->VMS->AMVColl, AMV);

  /* Delete all distributed arrays,
     mapped on deleted representation */    /*E0076*/

  while(AMV->ArrColl.Count)
  {
     wDArr = coll_At(s_DISARRAY *, &AMV->ArrColl, AMV->ArrColl.Count-1);

     wDArr->CrtEnvInd = gEnvColl->Count - 1;
     wDArr->HandlePtr->CrtEnvInd = gEnvColl->Count - 1;
     wDArr->HandlePtr->CrtAMHandlePtr = CurrAMHandlePtr;

     ( RTL_CALL, DelDA(wDArr) );
  }

  coll_Done(&AMV->ArrColl);

  /* Delete from the representation list of the parent abstract machine */    /*E0077*/

  AMS = (s_AMS *)(AMV->AMHandlePtr->pP);
  coll_Delete(&AMS->SubSystem, AMV);

  /* Delete all created abstract machines */    /*E0078*/

  for(i=0; i < AMV->AMSColl.Count; i++)
  {
     AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

     if(AMS->VMS)
     {
        coll_Delete(&AMS->VMS->AMSColl, AMS); /* from the list of machines
                                                 mapped on processor system */    /*E0079*/

        /* If representation has been mapped by distr_ 
           delete processor system on which the abstract machine
           has been mapped */    /*E0080*/

        if(AMV->VMS)
        {  VMRef = (PSRef)AMS->VMS->HandlePtr;
           ( RTL_CALL, delps_(&VMRef) );
        }
     }

     /* Delete all representations of the deleted abstract machine */    /*E0081*/

     while(AMS->SubSystem.Count)
     {  wAMV = coll_At(s_AMVIEW *, &AMS->SubSystem,
                       AMS->SubSystem.Count-1);
        AMVRef = (AMViewRef)wAMV->HandlePtr;

        ( RTL_CALL, delamv_(&AMVRef) );
     } 

     coll_Done(&AMS->SubSystem);

     if(TstObject)
        DelDVMObj((ObjectRef)AMS->HandlePtr);

     dvm_FreeStruct(AMS->HandlePtr);
  }

  coll_Done(&AMV->AMSColl);

  /* ------------------------------------------- */    /*E0082*/

  for(i=0; i < MAXARRAYDIM; i++)
  {  dvm_FreeArray(AMV->CoordWeight[i]);
     dvm_FreeArray(AMV->PrevSumCoordWeight[i]);
     dvm_FreeArray(AMV->GenBlockCoordWeight[i]);
     dvm_FreeArray(AMV->PrevSumGenBlockCoordWeight[i]);
     dvm_FreeArray(AMV->GroupWeightArray[i]);
  }

  if(AMV->HandlePtr)                        /* for redis_ */    /*E0083*/
  {  if(TstObject)
        DelDVMObj((ObjectRef)AMV->HandlePtr);
     AMV->HandlePtr->Type = sht_NULL;
     dvm_FreeStruct(AMV->HandlePtr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_amview_Done," \n");

  (DVM_RET);

  return;
}

#endif


#endif /* _AMS_C_ */    /*E0084*/
