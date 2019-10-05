#ifndef _OBJEQU_C_
#define _OBJEQU_C_
/****************/    /*E0000*/
 
/*******************************************\
* Functions to check equivalents of objects *
\*******************************************/    /*E0001*/

int  IsVMSEqu(s_VMS  *VMS1Ptr, s_VMS  *VMS2Ptr)
{ int  i, VMSSize;
           
  IsSpaceEqu(VMSSize, &VMS1Ptr->Space, &VMS2Ptr->Space)

  if(VMSSize == 0)
     return 0;

  if(VMS1Ptr->HasCurrent != VMS2Ptr->HasCurrent)
     return 0;

  if(VMS1Ptr->CrtEnvInd != VMS2Ptr->CrtEnvInd)
     return 0;

  if(VMS1Ptr->PHandlePtr != VMS2Ptr->PHandlePtr)
     return 0;

  if(VMS1Ptr->HasCurrent &&
     VMS1Ptr->CurrentProc != VMS2Ptr->CurrentProc)
     return 0;

  VMSSize = VMS1Ptr->ProcCount;

  for(i=0; i < VMSSize; i++)
      if(VMS1Ptr->VProc[i].lP != VMS2Ptr->VProc[i].lP)
         return 0;

  return 1;
}



int  IsAMViewEqu(s_AMVIEW  *AMV1Ptr, s_AMVIEW  *AMV2Ptr)
{ int     BlockEqu;
  s_MAP  *DISTMAP1, *DISTMAP2;

  IsSpaceEqu(BlockEqu, &AMV1Ptr->Space, &AMV2Ptr->Space)
  if(BlockEqu == 0)
     return 0;
  if(AMV1Ptr->HasLocal != AMV2Ptr->HasLocal)
     return 0;
  if(AMV1Ptr->CrtEnvInd != AMV2Ptr->CrtEnvInd)
     return 0;

  if(AMV1Ptr->HasLocal)
  {  IsBlockEqu(BlockEqu, &AMV1Ptr->Local, &AMV2Ptr->Local)
     if(BlockEqu == 0)
        return 0;
  }

  DISTMAP1 = AMV1Ptr->DISTMAP;
  DISTMAP2 = AMV2Ptr->DISTMAP;

  if(DISTMAP1 && DISTMAP2)
  {  if(IsMapEqu(DISTMAP1, DISTMAP2) == 0)
        return 0;
  }
  else
  {  if(DISTMAP1 || DISTMAP2)
        return 0;
  }

  if(AMV1Ptr->AMHandlePtr != AMV2Ptr->AMHandlePtr)
     return 0;

  if(AMV1Ptr->VMS && AMV2Ptr->VMS)
  {  if(AMV1Ptr->VMS != AMV2Ptr->VMS
        /*IsVMSEqu(AMV1Ptr->VMS, AMV2Ptr->VMS) == 0*/    /*E0002*/)
        return 0;
  }
  else
  {  if(AMV1Ptr->VMS || AMV2Ptr->VMS)
        return 0;
  }

  return 1;
}


// exteneded by Zakharov D. 18.07.2018
int  IsArrayEqu(SysHandle *Array1HandlePtr, SysHandle *Array2HandlePtr,
                byte TLenEqu, byte BaseShdCheck,
                s_SHDWIDTH *ShdWidth1, s_SHDWIDTH *ShdWidth2)
{ s_DISARRAY    *DA1, *DA2;
  int            i, ALSize;
  s_ALIGN       *Align1, *Align2;

  DA1 = (s_DISARRAY *)Array1HandlePtr->pP;
  DA2 = (s_DISARRAY *)Array2HandlePtr->pP;

  IsSpaceEqu(ALSize, &DA1->Space, &DA2->Space)

  if(ALSize == 0)
     return 0;

  if(TLenEqu && (DA1->TLen != DA2->TLen))
     return 0;

  if(DA1->HasLocal != DA2->HasLocal)
     return 0;

  if(DA1->CrtEnvInd != DA2->CrtEnvInd)
     return 0;

  if(DA1->HasLocal)
  {  IsBlockEqu(ALSize, &DA1->Block, &DA2->Block)

     if(ALSize == 0)
        return 0;

     IsArrBlockEqu(ALSize, &DA1->ArrBlock, &DA2->ArrBlock, TLenEqu)

     if(ALSize == 0)
        return 0;
  }

  if(DA1->AMView && DA2->AMView)  
  {  if(DA1->AMView != DA2->AMView
        /*IsAMViewEqu(DA1->AMView, DA2->AMView) == 0*/    /*E0003*/)
        return 0;
  }
  else
  {  if(DA1->AMView || DA2->AMView)
        return 0;
  }
  
  ALSize = DA1->Space.Rank + DA1->AMView->Space.Rank;

  for(i=0; i < ALSize; i++)
  {  Align1 = &DA1->Align[i];
     Align2 = &DA2->Align[i];

     if(IsAlignEqu(Align1 , Align2) == 0)
        return 0;
  }

  if (BaseShdCheck)
  {  ALSize = DA1->Space.Rank;

     for(i=0; i < ALSize; i++)
     {  if(DA1->InitLowShdWidth[i] !=
           DA2->InitLowShdWidth[i])
           return 0;
        if(DA1->InitHighShdWidth[i] !=
           DA2->InitHighShdWidth[i])
           return 0;
     }

  }

  if(ShdWidth1 != NULL && ShdWidth2 != NULL)
  {  ALSize = DA1->Space.Rank;

     for(i=0; i < ALSize; i++)
     {  if(ShdWidth1->ResLowShdWidth[i] !=
           ShdWidth2->ResLowShdWidth[i])
           return 0;
        if(ShdWidth1->ResHighShdWidth[i] !=
           ShdWidth2->ResHighShdWidth[i])
           return 0;
        if(ShdWidth1->ShdSign[i] != ShdWidth2->ShdSign[i])
           return 0;

        if(ShdWidth1->InitDimIndex[i] != ShdWidth2->InitDimIndex[i])
           return 0;
        if(ShdWidth1->DimWidth[i] != ShdWidth2->DimWidth[i])
           return 0;
        if(ShdWidth1->InitLowShdIndex[i] !=
           ShdWidth2->InitLowShdIndex[i])
           return 0;
        if(ShdWidth1->InitHiShdIndex[i] != ShdWidth2->InitHiShdIndex[i])
           return 0;
     }

     if(ShdWidth1->MaxShdCount != ShdWidth2->MaxShdCount)
        return 0;
  }

  return 1;
}


#endif   /*  _OBJEQU_C_  */    /*E0004*/
