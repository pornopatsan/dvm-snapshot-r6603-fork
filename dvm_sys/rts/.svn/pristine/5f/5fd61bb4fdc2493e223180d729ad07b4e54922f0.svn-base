#ifndef _MAPDISTR_C_
#define _MAPDISTR_C_
/******************/    /*E0000*/

/***************************************\
*  Functions for  mapping  an abstract  *
*       machine according to map        *
\***************************************/    /*E0001*/

AMViewMapRef __callstd amvmap_(AMViewRef  *AMViewRefPtr,
                               DvmType  *StaticSignPtr)

/*
      Getting map of an abstract machine representation.
      --------------------------------------------------

*AMViewRefPtr  - reference to an abstract machine representation.
*StaticSignPtr - sign of creation of static map.
 
Function amvmap_ creates an object (map), describing current mapping
of an abstract machine representation on MPS and returns pointer to
the created object.
*/    /*E0002*/

{ AMViewMapRef    Res;
  SysHandle      *AMVHandlePtr, *MapHandlePtr;
  s_AMVIEW       *AMV;
  s_VMS          *VMS;
  s_AMVIEWMAP    *Map;
  int             MapArrSize; 

  StatObjectRef = (ObjectRef)*AMViewRefPtr;   /* for statistics */    /*E0003*/
  DVMFTimeStart(call_amvmap_);

  if(RTL_TRACE)
     dvm_trace(call_amvmap_,
               "AMViewRefPtr=%lx; AMViewRef=%lx; StaticSign=%ld;\n",
               (uLLng)AMViewRefPtr, *AMViewRefPtr, *StaticSignPtr);

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 038.000: wrong call amvmap_\n"
            "(the abstract machine representation is not "
            "a DVM object;\nAMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;
  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.001: wrong call amvmap_\n"
           "(the object is not an abstract machine representation; "
           "AMViewRef=%lx)\n",
           *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  VMS = AMV->VMS;

  if(VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.002: wrong call amvmap_\n"
              "(the abstract machine representation has not been "
              "mapped;\nAMViewRef=%lx)\n", *AMViewRefPtr);

  dvm_AllocStruct(SysHandle, MapHandlePtr);
  dvm_AllocStruct(s_AMVIEWMAP, Map);

  Map->Static = (byte)*StaticSignPtr;
  Map->AMViewRank = AMV->Space.Rank;

  Map->VMS = VMS;
  Map->VMSRank = VMS->Space.Rank;

  MapArrSize = AMV->Space.Rank + VMS->Space.Rank;

  dvm_AllocArray(s_MAP, MapArrSize, Map->DISTMAP);
  dvm_ArrayCopy(s_MAP, Map->DISTMAP, AMV->DISTMAP, MapArrSize);

  MapArrSize = AMV->Space.Rank;
  dvm_ArrayCopy(DvmType, Map->Div, AMV->Div, MapArrSize);

  *MapHandlePtr = genv_InsertObject(sht_AMViewMap, Map);
  Map->HandlePtr = MapHandlePtr;  /* pointer to own Handle */    /*E0004*/

  if(TstObject)
     InsDVMObj((ObjectRef)MapHandlePtr);

  Res = (AMViewMapRef)MapHandlePtr;

  if(RTL_TRACE)
     dvm_trace(ret_amvmap_,"AMViewMapRef=%lx;\n", Res);

  StatObjectRef = (ObjectRef)Res;   /* for statistics */    /*E0005*/
  DVMFTimeFinish(ret_amvmap_);
  return  (DVM_RET, Res);
}



DvmType  __callstd mdistr_(AMViewRef *AMViewRefPtr, PSRef *PSRefPtr,
                           AMViewMapRef *AMViewMapRefPtr)

/*
      Definition of mapping of an abstract machine representation
      -----------------------------------------------------------
                         according to a map.
                         -------------------

*AMViewRefPtr    - reference to an abstract machine representation
                   to be mapped.
*PSRefPtr        - reference to MPS,that determines a set of allocatable
                   resources. 
*AMViewMapRefPtr - reference to the map of an abstract machine
                   representation.

      Function mdistr_ defines the mapping of the given abstract machine
representation on the given MPS according to given map. 
The function returns non zero value if mapped representation has a local
part on the current processor, otherwise returns zero. 
*/    /*E0006*/

{ s_AMVIEW       *AMV;
  SysHandle      *AMVHandlePtr, *VMSHandlePtr, *MapHandlePtr;
  s_AMVIEWMAP    *Map;
  s_VMS          *VMS;
  int             VMR, AMR, i, ALSize, VMSize, MapArrSize;
  s_ALIGN        *AlList;
  s_BLOCK        *Local, **LocBlockPtr = NULL, Block, *LocBlock = NULL;
  s_AMS          *PAMS, *CAMS;
  byte            IsConst = 0;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0007*/
  DVMFTimeStart(call_mdistr_);

  if(RTL_TRACE)
  {  if(!TstTraceEvent(call_mdistr_))
        dvm_trace(call_mdistr_," \n");
     else
     {  if(PSRefPtr == NULL || *PSRefPtr == 0)
           dvm_trace(call_mdistr_,
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "PSRefPtr=NULL; PSRef=0; "
                     "AMViewMapRefPtr=%lx; AMViewMapRef=%lx;\n",
                     (uLLng)AMViewRefPtr, *AMViewRefPtr,
                     (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr);
        else
           dvm_trace(call_mdistr_,
                     "AMViewRefPtr=%lx; AMViewRef=%lx; "
                     "PSRefPtr=%lx; PSRef=%lx; "
                     "AMViewMapRefPtr=%lx; AMViewMapRef=%lx;\n",
                     (uLLng)AMViewRefPtr, *AMViewRefPtr,
                     (uLLng)PSRefPtr, *PSRefPtr,
                     (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr);
     }
  }

  if(TstObject)
  {  if(!TstDVMObj(AMViewRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.010: wrong call mdistr_\n"
           "(the abstract machine representation is not a DVM "
           "object;\nAMViewRef=%lx)\n", *AMViewRefPtr);
  }

  AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

  if(AMVHandlePtr->Type != sht_AMView)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 038.011: wrong call mdistr_\n"
            "(the object is not an abstract machine representation; "
            "AMViewRef=%lx)\n",
            *AMViewRefPtr);

  AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  AMR = AMV->Space.Rank;

  if(AMV->DivReset)
  {
     AMV->DivReset = 0;

     for(i=0; i < MAXARRAYDIM; i++)
         AMV->Div[i] = 1;
  }

  if(TstObject)
  {  if(!TstDVMObj(AMViewMapRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 038.012: wrong call mdistr_\n"
            "(the map of the abstract machine representation is not "
            "a DVM object;\nAMViewMapRef=%lx)\n", *AMViewMapRefPtr);
  }

  MapHandlePtr = (SysHandle *)*AMViewMapRefPtr;

  if(MapHandlePtr->Type != sht_AMViewMap)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 038.013: wrong call mdistr_\n"
        "(the object is not a map; AMViewMapRef=%lx)\n",
        *AMViewMapRefPtr);

  Map = (s_AMVIEWMAP *)MapHandlePtr->pP;

  if(PSRefPtr == NULL || *PSRefPtr == 0)
  {  VMS = Map->VMS;
     VMSHandlePtr = VMS->HandlePtr;
  }
  else
  {  if(TstObject)
     {  if(!TstDVMObj(PSRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 038.014: wrong call mdistr_\n"
             "(the processor system is not a DVM object; "
             "PSRef=%lx)\n", *PSRefPtr);
     }

     VMSHandlePtr=(SysHandle *)*PSRefPtr;
     VMS = (s_VMS *)VMSHandlePtr->pP;
  }

  if(VMSHandlePtr->Type != sht_VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.015: wrong call mdistr_\n"
              "(the object is not a processor system; PSRef=%lx)\n",
              (uLLng)VMSHandlePtr);

  VMR = VMS->Space.Rank;

  if(AMR != Map->AMViewRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.016: wrong call mdistr_\n"
              "(AMView Rank=%d # Map AMView Rank=%d)\n",
              AMR, (int)Map->AMViewRank);

  if(VMR != Map->VMSRank)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.017: wrong call mdistr_\n"
              "(PS Rank=%d # Map PS Rank=%d)\n",
              VMR, (int)Map->VMSRank);

  /* Check if representation already mapped */    /*E0008*/

  if(AMV->VMS)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.020: wrong call mdistr_\n"
              "(the representation has already been mapped; "
              "AMViewRef=%lx)\n", *AMViewRefPtr);

  /* Check if any abstract machine
      of representation is mapped  */    /*E0009*/

  for(i=0; i < AMV->AMSColl.Count; i++)
  {   PAMS = coll_At(s_AMS *, &AMV->AMSColl, i);

      if(PAMS->VMS)
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err 038.021: wrong call mdistr_\n"
               "(the daughter abstract machine of the representation "
               "has already been mapped;\nAMViewRef=%lx; "
               "DaughterAMRef=%lx)\n",
               *AMViewRefPtr, (uLLng)PAMS->HandlePtr);
  }

  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent
                                           abstract machine */    /*E0010*/
  CAMS = (s_AMS *)CurrAMHandlePtr->pP;  /* current abstract machine */    /*E0011*/

  /* Check if parent abstarct machine is mapped */    /*E0012*/

  if(PAMS->VMS == NULL)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.024: wrong call mdistr_\n"
              "(the parental AM has not been mapped;\n"
              "AMViewRef=%lx; ParentAMRef=%lx)\n",
              *AMViewRefPtr, (uLLng)PAMS->HandlePtr);

  /* Check if all the processors of the specified processor
           system are in the parent processor system        */    /*E0013*/

  NotSubsystem(i, PAMS->VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.025: wrong call mdistr_\n"
           "(the given PS is not a subsystem of the parental PS;\n"
           "PSRef=%lx; ParentPSRef=%lx)\n",
           (uLLng)VMSHandlePtr, (uLLng)(PAMS->VMS->HandlePtr));

  /* Check if all the processors of the specified processor
           system are in the current processor system       */    /*E0014*/

  NotSubsystem(i, DVM_VMS, VMS)

  if(i)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.026: wrong call mdistr_\n"
           "(the given PS is not a subsystem of the current PS;\n"
           "PSRef=%lx; CurrentPSRef=%lx)\n",
           (uLLng)VMSHandlePtr, (uLLng)DVM_VMS->HandlePtr);

  /* Fix distribution in tables */    /*E0015*/

  AMV->VMS = VMS;

  MapArrSize = AMR + VMR;
  dvm_AllocArray(s_MAP, MapArrSize, AMV->DISTMAP);
  dvm_ArrayCopy(s_MAP, AMV->DISTMAP, Map->DISTMAP, MapArrSize);

  dvm_ArrayCopy(DvmType, AMV->Div, Map->Div, AMR);

  coll_Insert(&VMS->AMVColl, AMV); /* into the list
                                      of representations mapped on
                                      processor system */    /*E0016*/

  /* Create local part of representation */    /*E0017*/

  ALSize = AMR + AMR;
  dvm_AllocArray(s_ALIGN, ALSize, AlList);

  /* Form two parts of AlList for identical alignment */    /*E0018*/

  for(i=0; i < AMR; i++)
  {  AlList[i].Attr  = align_NORMAL;
     AlList[i].Axis  = (byte)(i+1);
     AlList[i].TAxis = (byte)(i+1);
     AlList[i].A     = 1;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  for(i=AMR; i < ALSize; i++)
  {  AlList[i].Attr  = align_NORMTAXIS;
     AlList[i].Axis  = (byte)(i-AMR+1);
     AlList[i].TAxis = (byte)(i-AMR+1);
     AlList[i].A     = 0;
     AlList[i].B     = 0;
     AlList[i].Bound = 0;
  }

  /* Calculate local part */    /*E0019*/

  Local = NULL;

  if(VMS->HasCurrent)
     Local = GetSpaceLB4Proc(VMS->CurrentProc, AMV,
                             &AMV->Space, AlList, NULL, &Block);

  if(Local)
  {  AMV->HasLocal = TRUE;
     AMV->Local = block_Copy(Local);
  }

  /* Form an attribute of completely replicated representation */    /*E0020*/

  for(i=AMR; i < MapArrSize; i++)
     if(AMV->DISTMAP[i].Attr == map_CONSTANT)
        IsConst = 1; /* flag:  constant rule of mapping took place */    /*E0021*/

  VMSize = VMS->ProcCount;

  if(VMSize == 1)
     AMV->Repl = 1; /* replication along all dimensions
                       of the processor system */    /*E0022*/
  else
  {  for(i=AMR; i < MapArrSize; i++)
         if(AMV->DISTMAP[i].Attr != map_REPLICATE)
            break;

     if(i == MapArrSize)
        AMV->Repl = 1; /* replication along all dimensions
                          of the processor system */    /*E0023*/
  }

  /* Form an attribute of partly replicated representation */    /*E0024*/

  if(AMV->Repl == 0)
  {  for(i=AMR; i < MapArrSize; i++)
         if(AMV->DISTMAP[i].Attr == map_REPLICATE)
            break;

     if(i != MapArrSize)
        AMV->PartRepl = 1; /* representation is replicated along some
                              processor system dimension 
                              (not all) */    /*E0025*/
  }

  /* Form an attribute, that there is at least one element
              of representation on each processor          */    /*E0026*/

  if(AMV->Repl)
     AMV->Every = 1;
  else
  {  if(AMV->AMSColl.Count)
     {  dvm_AllocArray(s_BLOCK *, VMSize, LocBlockPtr);
        dvm_AllocArray(s_BLOCK, VMSize, LocBlock);

        for(i=0; i < VMSize; i++)
            LocBlockPtr[i] = GetSpaceLB4Proc(i, AMV, &AMV->Space,
                                             AlList, NULL, &LocBlock[i]);

        for(i=0; i < VMSize; i++)
            if(LocBlockPtr[i] == NULL)
               break;

        if(i == VMSize)
           AMV->Every = 1; /* there is an element of representation 
                           on each processor */    /*E0027*/
     }
     else
     {  Local = GetSpaceLB4Proc(VMSize-1, AMV, &AMV->Space, AlList,
                                NULL, &Block);

        if(Local && (IsConst == 0 || VMR == 1))
           AMV->Every = 1; /* representation is on
                              every processor */    /*E0028*/
     }
  }

  CrtVMSForAMS(AMV, LocBlockPtr, AlList);/* create subtasks for 
                                            local part of representation
                                            and all interrogated machines */    /*E0029*/
  if(LocBlockPtr)
  {  dvm_FreeArray(LocBlockPtr);
     dvm_FreeArray(LocBlock);
  }

  dvm_FreeArray(AlList);

  /* Print resulting tables */    /*E0030*/

  if(RTL_TRACE)
  {  if(distr_Trace && TstTraceEvent(call_mdistr_))
     {  for(i=0; i < MapArrSize; i++)
            tprintf("DistMap[%d]: Attr=%d Axis=%d "
               "PAxis=%d DisPar=%lf\n",
               i, (int)AMV->DISTMAP[i].Attr, (int)AMV->DISTMAP[i].Axis,
               (int)AMV->DISTMAP[i].PAxis, AMV->DISTMAP[i].DisPar);
        tprintf(" \n");

        if(AMV->HasLocal)
        {  for(i=0; i < AMR; i++) 
               tprintf("Local[%d]: Lower=%ld Upper=%ld "
                       "Size=%ld Step=%ld\n",
                       i, AMV->Local.Set[i].Lower,
                       AMV->Local.Set[i].Upper,
                       AMV->Local.Set[i].Size, AMV->Local.Set[i].Step);
          tprintf(" \n");
        }

        tprintf("Repl=%d PartRepl=%d Every=%d\n",
                (int)AMV->Repl, (int)AMV->PartRepl, (int)AMV->Every);
        tprintf(" \n");
     }
  }

  /* ------------------------------ */    /*E0031*/
       
  if(RTL_TRACE)
     dvm_trace(ret_mdistr_,"IsLocal=%d;\n",(int)AMV->HasLocal);

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0032*/
  DVMFTimeFinish(ret_mdistr_);
  return  (DVM_RET, (DvmType)AMV->HasLocal);
}



DvmType  __callstd mredis_(AMViewRef *AMViewRefPtr, PSRef *PSRefPtr,
                           AMViewMapRef *AMViewMapRefPtr,DvmType *NewSignPtr)

/*
      Change of mapping of an abstract machine representation
      -------------------------------------------------------
                       according to a map.
                       -------------------

*AMViewRefPtr    - reference to an abstract machine representation
                   whose mapping is to be  changed.
*PSRefPtr        - reference to MPS, that determines a set of resources
                   for the new distribution. 
*AMViewMapRefPtr - reference to the map of  an abstract machine 
                   representation.
*NewSignPtr      - sign of renewal of all redistributed array content
                   (equal to 1).
 
   Function mredis_ cancels a parental abstract machine resource
allocation, established earlier for child abstract machines by
function mdistr_ (or distr_).
The function returns non zero value if remapped representation
has a local part on the current processor,otherwise returns zero. 
*/    /*E0033*/

{ SysHandle      *ArrayHandlePtr, *AMVHandlePtr, *NewAMVHandlePtr,
                 *MapHandlePtr, *VMSHandlePtr;
  s_DISARRAY     *DArr, *wDArr;
  s_AMVIEW       *AMV, *NewAMV;
  AMViewRef       AMVRef, NewAMVRef;
  AMRef           AMRefr;
  DvmType         AMVRank, AMVStatic, StaticMap = 0, Res = 0;
  DvmType        *ArrayHeader;
  ArrayMapRef     ArrMapRef;
  byte            SaveReDistr;
  s_AMS          *AMS, *CAMS, *PAMS;
  int             i, VMR, AMR;
  s_AMVIEWMAP    *Map;
  s_VMS          *VMS;

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0034*/
  DVMFTimeStart(call_mredis_);

  ArrayHandlePtr = TstDVMArray((DvmType *)AMViewRefPtr);

  if(RTL_TRACE)
  {  if(!TstTraceEvent(call_mredis_))
        dvm_trace(call_mredis_," \n");
     else
     {  if(ArrayHandlePtr)
        {  if(PSRefPtr == NULL || *PSRefPtr == 0)
              dvm_trace(call_mredis_,
                        "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                        "PSRefPtr=NULL; PSRef=0; "
                        "AMViewMapRefPtr=%lx; AMViewMapRef=%lx; "
                        "NewSign=%ld;\n",
                        (uLLng)AMViewRefPtr, *AMViewRefPtr,
                        (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr,
                        *NewSignPtr);
           else
              dvm_trace(call_mredis_,
                        "ArrayHeader=%lx; ArrayHandlePtr=%lx; "
                        "PSRefPtr=%lx; PSRef=%lx; "
                        "AMViewMapRefPtr=%lx; AMViewMapRef=%lx; "
                        "NewSign=%ld;\n",
                        (uLLng)AMViewRefPtr, *AMViewRefPtr,
                        (uLLng)PSRefPtr, *PSRefPtr,
                        (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr,
                        *NewSignPtr);
        }
        else
        {  if(PSRefPtr == NULL || *PSRefPtr == 0)
              dvm_trace(call_mredis_,
                        "AMViewRefPtr=%lx; AMViewRef=%lx; "
                        "PSRefPtr=NULL; PSRef=0; "
                        "AMViewMapRefPtr=%lx; AMViewMapRef=%lx; "
                        "NewSign=%ld;\n",
                        (uLLng)AMViewRefPtr, *AMViewRefPtr,
                        (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr,
                        *NewSignPtr);
           else
              dvm_trace(call_mredis_,
                        "AMViewRefPtr=%lx; AMViewRef=%lx; "
                        "PSRefPtr=%lx; PSRef=%lx; "
                        "AMViewMapRefPtr=%lx; AMViewMapRef=%lx; "
                        "NewSign=%ld;\n",
                        (uLLng)AMViewRefPtr, *AMViewRefPtr,
                        (uLLng)PSRefPtr, *PSRefPtr,
                        (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr,
                        *NewSignPtr);
        }
     }
  }

  if(ArrayHandlePtr)
  {  /* Representation is defined indirectly through array */    /*E0035*/

     DArr = (s_DISARRAY *)ArrayHandlePtr->pP;

     if( !(DArr->ReDistr & 0x1) )
         epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                  "*** RTS err 038.030: wrong call mredis_\n"
                  "(ReDistrPar=%d; ArrayHeader[0]=%lx)\n",
                  (int)DArr->ReDistr, (uLLng)ArrayHandlePtr);

     AMV = DArr->AMView;

     if(AMV == NULL)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 038.031: wrong call mredis_\n"
                 "(the array has not been aligned; "
                 "ArrayHeader[0]=%lx)\n", (uLLng)ArrayHandlePtr);

     AMVHandlePtr = AMV->HandlePtr;
  }
  else
  {  /* Representation is directly defined */    /*E0036*/

     if(TstObject)
     {  if(!TstDVMObj(AMViewRefPtr))
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 038.032: wrong call mredis_\n"
             "(the abstract machine representation is not a "
             "DVM object;\nAMViewRef=%lx)\n", *AMViewRefPtr);
     }

     AMVHandlePtr = (SysHandle *)*AMViewRefPtr;

     if(AMVHandlePtr->Type != sht_AMView)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.033: wrong call mredis_\n"
           "(the object is not an abstract machine representation; "
           "AMViewRef=%lx)\n",
           *AMViewRefPtr);

     AMV = (s_AMVIEW *)AMVHandlePtr->pP;
  }

  if(AMV->DivReset)
  {
     AMV->DivReset = 0;

     for(i=0; i < MAXARRAYDIM; i++)
         AMV->Div[i] = 1;
  }

  if(AMV->VMS == NULL) /* whether representation is
                          remapped */    /*E0037*/
     Res = ( RTL_CALL, mdistr_(AMViewRefPtr, PSRefPtr,
                               AMViewMapRefPtr) );
  else
  {  PAMS = (s_AMS *)AMV->AMHandlePtr->pP; /* parent
                                              abstarct machine */    /*E0038*/
     CAMS = (s_AMS *)CurrAMHandlePtr->pP;  /* current abstract 
                                              machine */    /*E0039*/

     /* Check if there are children of representation elements */    /*E0040*/

     for(i=0; i < AMV->AMSColl.Count; i++)
     {  AMS = coll_At(s_AMS *, &AMV->AMSColl, i);

        if(AMS->SubSystem.Count)/* whether abstract machine is list */    /*E0041*/
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                    "*** RTS err 038.037: wrong call mredis_\n"
                    "(the abstract machine of the representation "
                    "has descendants; AMViewRef=%lx; AMRef=%lx\n",
                    *AMViewRefPtr, (uLLng)AMS->HandlePtr);
     }
  

     if(TstObject)
     {  if(TstDVMObj(AMViewMapRefPtr) == 0)
           epprintf(MultiProcErrReg1,__FILE__,__LINE__,
             "*** RTS err 038.038: wrong call mredis_\n"
             "(the map of the abstract machine representation is not "
             "a DVM object;\nAMViewMapRef=%lx)\n", *AMViewMapRefPtr);
     }

     MapHandlePtr = (SysHandle *)*AMViewMapRefPtr;

     if(MapHandlePtr->Type != sht_AMViewMap)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
           "*** RTS err 038.039: wrong call mredis_\n"
           "(the object is not a map; AMViewMapRef=%lx)\n",
           *AMViewMapRefPtr);

     Map = (s_AMVIEWMAP *)MapHandlePtr->pP;


     if(PSRefPtr == NULL || *PSRefPtr == 0)
     {  VMS          = Map->VMS;
        VMSHandlePtr = VMS->HandlePtr;
     }
     else
     {  if(TstObject)
        {  if(TstDVMObj(PSRefPtr) == 0)
              epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                       "*** RTS err 038.040: wrong call mredis_\n"
                       "(the processor system is not a DVM object; "
                       "PSRef=%lx)\n", *PSRefPtr);
        }

        VMSHandlePtr = (SysHandle *)*PSRefPtr;
        VMS          = (s_VMS *)VMSHandlePtr->pP;
     }

     if(VMSHandlePtr->Type != sht_VMS)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 038.041: wrong call mredis_\n"
                 "(the object is not a processor system; PSRef=%lx)\n",
                 (uLLng)VMSHandlePtr);


     /* Check if all processors of specified processor
            system are in parent processor system      */    /*E0042*/

     NotSubsystem(i, PAMS->VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.042: wrong call mredis_ "
              "(the given PS is not a subsystem of the parental PS; "
              "PSRef=%lx; ParentPSRef=%lx)\n",
              (uLLng)VMSHandlePtr, (uLLng)(PAMS->VMS->HandlePtr));

     /* Check if all processor of specified processor
           system are in current processor system     */    /*E0043*/

     NotSubsystem(i, DVM_VMS, VMS)

     if(i)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
              "*** RTS err 038.043: wrong call mredis_ "
              "(the given PS is not a subsystem of the current PS; "
              "PSRef=%lx; CurrentPSRef=%lx)\n",
              (uLLng)VMSHandlePtr, (uLLng)DVM_VMS->HandlePtr);

     AMR = AMV->Space.Rank;

     if(AMR != Map->AMViewRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 038.044: wrong call mredis_\n"
                 "(AMView Rank=%d # Map AMView Rank=%d)\n",
                 AMR, (int)Map->AMViewRank);

     VMR = VMS->Space.Rank;

     if(VMR != Map->VMSRank)
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
                 "*** RTS err 038.045: wrong call mredis_\n"
                 "(PS Rank=%d # Map PS Rank=%d)\n",
                 VMR, (int)Map->VMSRank);

     /* Create a new representation of an abstract machine */    /*E0044*/

     AMVRef    = (AMViewRef)AMVHandlePtr;
     AMRefr    = (AMRef)AMV->AMHandlePtr; /* reference to parent
                                             abstarct machine    */    /*E0045*/
     AMVRank   = AMV->Space.Rank;
     AMVStatic = AMV->Static;

     NewAMVRef = ( RTL_CALL, crtamv_(&AMRefr, &AMVRank, AMV->Space.Size, &AMVStatic) );

     NewAMVHandlePtr = (SysHandle *)NewAMVRef;
     NewAMV          = (s_AMVIEW *)NewAMVHandlePtr->pP;

     /* Transfer of processor system coordinate weights */    /*E0046*/

     NewAMV->WeightVMS = AMV->WeightVMS;
     AMV->WeightVMS = NULL;

     for(i=0; i < MAXARRAYDIM; i++)
     {
        NewAMV->CoordWeight[i] = AMV->CoordWeight[i];
        NewAMV->PrevSumCoordWeight[i] = AMV->PrevSumCoordWeight[i];
        NewAMV->GenBlockCoordWeight[i] = AMV->GenBlockCoordWeight[i];
        NewAMV->PrevSumGenBlockCoordWeight[i] =
        AMV->PrevSumGenBlockCoordWeight[i];
        NewAMV->Div[i] = AMV->Div[i];

        AMV->CoordWeight[i] = NULL;
        AMV->PrevSumCoordWeight[i] = NULL;
        AMV->GenBlockCoordWeight[i] = NULL;
        AMV->PrevSumGenBlockCoordWeight[i] = NULL;
        AMV->Div[i] = 1;
     }

     /* Transfer of all interrrogated abstarct machines
          from the old representation to the new one    */    /*E0047*/

     NewAMV->AMSColl = AMV->AMSColl;
     AMV->AMSColl    = coll_Init(AMVAMSCount, AMVAMSCount, NULL);

     for(i=0; i < NewAMV->AMSColl.Count; i++)
     {
        AMS = coll_At(s_AMS *, &NewAMV->AMSColl, i);
        AMS->ParentAMView = NewAMV;
        AMS->VMS = NULL;
     }

     /* Mapping new representation */    /*E0048*/

     Res = ( RTL_CALL, mdistr_(&NewAMVRef, PSRefPtr, AMViewMapRefPtr) );

     /* Redistribute all arrays of an old representation */    /*E0049*/

     while(AMV->ArrColl.Count)
     {
        wDArr = coll_At(s_DISARRAY *, &AMV->ArrColl,
                        AMV->ArrColl.Count - 1);
        ArrayHeader = (DvmType *)wDArr->HandlePtr->HeaderPtr;
        SaveReDistr = wDArr->ReDistr;
        wDArr->ReDistr = 3; 

        ArrMapRef = ( RTL_CALL, arrmap_(ArrayHeader, &StaticMap) );
        ( RTL_CALL, mrealn_(ArrayHeader, &NewAMVRef, &ArrMapRef, NewSignPtr) );
        ( RTL_CALL, delarm_(&ArrMapRef) );

        wDArr = (s_DISARRAY *)((SysHandle *)ArrayHeader[0])->pP;
        wDArr->ReDistr = SaveReDistr;
     }

     /* Delete an old representation and save Handle */    /*E0050*/

     AMV->HandlePtr = NULL;    /* to save Handle */    /*E0051*/

     ( RTL_CALL, delamv_(&AMVRef) );

     /* Form an old  Handle as a  Handle of new representation */    /*E0052*/

     *AMVHandlePtr     = *NewAMVHandlePtr;
     NewAMV->HandlePtr = AMVHandlePtr;

     /* Delete new Handle */    /*E0053*/

     if(TstObject)
        DelDVMObj((ObjectRef)NewAMVHandlePtr);

     dvm_FreeStruct(NewAMVHandlePtr);
  }

  if(RTL_TRACE)
     dvm_trace(ret_mredis_,"IsLocal=%ld;\n", Res);

  StatObjectRef = (ObjectRef)*AMViewRefPtr; /* for statistics */    /*E0054*/
  DVMFTimeFinish(ret_mredis_);
  return  (DVM_RET, Res);
}



DvmType  __callstd delmvm_(AMViewMapRef  *AMViewMapRefPtr)

/*
                   Map deleting.
                   -------------

*AMViewMapRefPtr - reference to the map of an abstract machine
                   representation.

Function delmvm_ deletes the map of an abstract machine representation
created by function amvmap_.

The function returns zero.
*/    /*E0055*/

{ SysHandle   *MapHandlePtr;

  StatObjectRef = (ObjectRef)*AMViewMapRefPtr;    /* for statistics */    /*E0056*/
  DVMFTimeStart(call_delmvm_);

  if(RTL_TRACE)
     dvm_trace(call_delmvm_,"AMViewMapRefPtr=%lx; AMViewMapRef=%lx;\n",
                            (uLLng)AMViewMapRefPtr, *AMViewMapRefPtr);

  if(TstObject)
  {  if(!TstDVMObj(AMViewMapRefPtr))
        epprintf(MultiProcErrReg1,__FILE__,__LINE__,
            "*** RTS err 038.050: wrong call delmvm_\n"
            "(the map of the abstract machine representation is not "
            "a DVM object;\nAMViewMapRef=%lx)\n", *AMViewMapRefPtr);
  }

  MapHandlePtr = (SysHandle *)*AMViewMapRefPtr;

  if(MapHandlePtr->Type != sht_AMViewMap)
     epprintf(MultiProcErrReg1,__FILE__,__LINE__,
        "*** RTS err 038.051: wrong call delmvm_\n"
        "(the object is not a map; AMViewMapRef=%lx)\n",
        *AMViewMapRefPtr);

  ( RTL_CALL, delobj_((ObjectRef *)AMViewMapRefPtr) );

  if(RTL_TRACE)
     dvm_trace(ret_delmvm_," \n");

  StatObjectRef = (ObjectRef)*AMViewMapRefPtr;    /* for statistics */    /*E0057*/
  DVMFTimeFinish(ret_delmvm_);
  return  (DVM_RET, 0);
}



void  AMVMap_Done(s_AMVIEWMAP  *Map)
{ 
  if(RTL_TRACE)
     dvm_trace(call_AMVMap_Done,"AMViewMapRef=%lx;\n",
                                 (uLLng)Map->HandlePtr);

  dvm_FreeArray(Map->DISTMAP);
  Map->VMS = NULL;

  if(TstObject)
     DelDVMObj((ObjectRef)Map->HandlePtr);
 
  Map->HandlePtr->Type = sht_NULL;
  dvm_FreeStruct(Map->HandlePtr);

  if(RTL_TRACE)
     dvm_trace(ret_AMVMap_Done," \n");

  (DVM_RET);

  return;
}


#endif   /*  _MAPDISTR_C_  */    /*E0058*/
