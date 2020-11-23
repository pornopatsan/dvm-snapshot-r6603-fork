#ifndef  _SPACE_C_
#define  _SPACE_C_

#include "system.typ"

/****************/    /*E0000*/


DvmType  *spind_Init(byte Rank)
{  DvmType  *Index;
 
   mac_calloc(Index, DvmType *, Rank+1, sizeof(DvmType), 0);
   Index[0] = (DvmType)Rank;

   return  Index;
}



int  spind_FromBlock(DvmType  Index[], s_BLOCK  *CurrBlock,
                     s_BLOCK  *InitBlock, byte  StepSign)
{ int   i, Rank;

  if(CurrBlock->Rank == 0)
     return 0; /* all elements are read */    /*E0001*/

  Rank = CurrBlock->Rank;
  Index[0] = Rank;

  for(i=0; i < Rank; i++)
      Index[i+1] = CurrBlock->Set[i].Lower;

  if(StepSign == 0)
  {  for(i=Rank-1; i >= 0; i--)
         if(CurrBlock->Set[i].Size > 1)
            break;

     if(i < 0)
        CurrBlock->Rank = 0;
     else
     {  CurrBlock->Set[i].Lower++;
        CurrBlock->Set[i].Size--;
   
        for(i++; i < Rank; i++)
        {  CurrBlock->Set[i].Lower = InitBlock->Set[i].Lower;
           CurrBlock->Set[i].Size = InitBlock->Set[i].Size;
        }
     }
  }
  else
  {  for(i=Rank-1; i >= 0; i--)
         if(CurrBlock->Set[i].Size > CurrBlock->Set[i].Step)
            break;

     if(i < 0)
        CurrBlock->Rank = 0;
     else
     {  CurrBlock->Set[i].Lower += CurrBlock->Set[i].Step;
        CurrBlock->Set[i].Size  -= CurrBlock->Set[i].Step;
   
        for(i++; i < Rank; i++)
        {  CurrBlock->Set[i].Lower = InitBlock->Set[i].Lower;
           CurrBlock->Set[i].Size = InitBlock->Set[i].Size;
        }
     }
  }

  return 1;
}



void spind_Done(DvmType **Index)
{
  if(*Index != NULL)
  {  dvm_FreeArray(*Index);
     *Index = NULL;
  }

  return;
}


/* ---------------------------------------- */    /*E0002*/


s_SPACE  space_Init(byte Rank, DvmType *SizeList)
{ s_SPACE     Space;
  int         i;
 
  Space.Rank = Rank;

  if(Space.Rank != 0)
  {  dvm_memcopy(Space.Size, SizeList, sizeof(DvmType) * Space.Rank);
     Space.Mult[Space.Rank-1] = 1;
     for(i=Space.Rank-2; i >= 0; i--)
         Space.Mult[i] = Space.Mult[i+1] * Space.Size[i+1];
  }

  return  Space;
}


/*************************************************************\
*    Function space_GetLI  returns linear index of element    *
* of the given space and with given coordinates in this space *
\*************************************************************/    /*E0003*/

DvmType  space_GetLI(s_SPACE  *Space, DvmType *Index)
{ int     i;
  DvmType    LinIndex = 0;
  DvmType   *ip, *mp;

  if(Index == NULL)
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err: wrong call space_GetLI "
             "(Index = 0)\n");
  if(Space->Rank != Index[0])
     eprintf(__FILE__,__LINE__,
             "*** RTS fatal err: wrong call space_GetLI "
             "((Index[0]=%ld) # (Space->Rank=%d))\n",
             Index[0], (int)Space->Rank);

  for(ip=Index+1,mp=Space->Mult,i=0; i < Space->Rank; ip++,mp++,i++)
      LinIndex += (*ip) * (*mp);

  return  LinIndex;
}


/************************************************\
* Function GetSI  calculates element coordinates *
*  of the given space using given linear index   *
\************************************************/    /*E0004*/

void  space_GetSI(s_SPACE *Space, DvmType LinIndex, DvmType **Index)
{ int    i;
  DvmType  *ip, *mp;

  if(**Index != Space->Rank)
  { spind_Done(Index);
    *Index = spind_Init(Space->Rank);
  }
 
  for(ip=*Index+1,mp=Space->Mult,i=0; i < Space->Rank; ip++,mp++,i++)
  { *ip = LinIndex / (*mp);
    LinIndex  -= (*ip)*(*mp);
  }

  return;
}


/* ---------------------------------------------- */    /*E0005*/


s_REGULARSET  rset_Build(DvmType Lower, DvmType Upper, DvmType Step)
{ s_REGULARSET   Res;

  Res.Lower = Lower;
  Res.Upper = Upper;
  Res.Size  = Upper - Lower + 1;
  Res.Step  = Step;

  return  Res;
}



s_BLOCK  block_Init(byte ARank, s_REGULARSET *ASet)
{ s_BLOCK    Res;
  int        i;
 
  Res.Rank = ARank;

  for(i=0; i < ARank; i++)
      Res.Set[i] = ASet[i];

  return  Res;
}



s_BLOCK  block_InitFromArr(byte  ARank, DvmType  InitArray[],
                           DvmType LastArray[], DvmType  StepArray[])
{ s_BLOCK    Res;
  int        i;
 
  Res.Rank = ARank;

  for(i=0; i < ARank; i++)
  {  Res.Set[i].Lower = InitArray[i];
     Res.Set[i].Upper = LastArray[i];
     Res.Set[i].Size  = LastArray[i] - InitArray[i] + 1;
     Res.Set[i].Step  = StepArray[i];
  }

  return  Res;
}



s_BLOCK  block_ZInit(byte ARank)
{ s_BLOCK    Res;
  int        i;
 
  Res.Rank = ARank;

  for (i=0; i < Res.Rank; i++)
  {  Res.Set[i].Lower = 0;
     Res.Set[i].Upper = 0;
     Res.Set[i].Size = 0;
     Res.Set[i].Step = 0;
  }

  return  Res;
}



s_BLOCK  block_Copy(s_BLOCK *ABlock)
{ int      i;
  s_BLOCK  Res;
 
  Res.Rank = ABlock->Rank;

  for(i=0; i < Res.Rank; i++)
      Res.Set[i] = ABlock->Set[i];

  return  Res;
}



s_BLOCK  block_InitFromSpace(s_SPACE *ASpace)
{ s_BLOCK   Res;
  int       i;
  
  Res.Rank = ASpace->Rank;

  for(i=0; i < Res.Rank; i++)
      Res.Set[i] = rset_Build(0, ASpace->Size[i]-1, 1);

  return  Res;
}



DvmType  block_GetLI(s_BLOCK *B, DvmType Index[], byte StepSign)
{
    DvmType  Res = 0, CompressSize;
  int   i, Rank;

  Rank = B->Rank;

  if(StepSign == 0)
  {  for(i=0; i < Rank; i++)
        Res = Res*B->Set[i].Size + (Index[i+1] - B->Set[i].Lower);
  }
  else
  {  for(i=0; i < Rank; i++)
  {
      CompressSize = (DvmType)ceil((double)B->Set[i].Size /
                                   (double)B->Set[i].Step );
        Res = Res*CompressSize +
              (Index[i+1] - B->Set[i].Lower) / B->Set[i].Step;
     }
  }

  return  Res;
}



void  block_GetSI(s_BLOCK *B, DvmType Weight[], DvmType LinIndex,
                  DvmType Index[], byte StepSign)
{ int    i, Rank;
  DvmType   Ind;

  Rank = B->Rank;
  Index[0] = Rank;

  if(StepSign == 0)
  {  for(i=0; i < Rank; i++)
     {  Ind = LinIndex / Weight[i];
        LinIndex -= Ind * Weight[i];
        Index[i+1] = Ind + B->Set[i].Lower;
     }
  }
  else
  {  for(i=0; i < Rank; i++)
     {  Ind = LinIndex / Weight[i];
        LinIndex -= Ind * Weight[i];
        Index[i+1] = Ind*B->Set[i].Step + B->Set[i].Lower;
     }
  }

  return;
}



void  block_GetWeight(s_BLOCK *B, DvmType Weight[], byte StepSign)
{ int   i, Rank;
  DvmType  CompressSize;

  Rank = B->Rank;
  Weight[Rank-1] = 1;

  if(StepSign == 0)
  {  for(i=Rank-2; i >= 0; i--)
        Weight[i] = Weight[i+1] * B->Set[i+1].Size;
  }
  else
  {  for(i=Rank-2; i >= 0; i--)
     {  CompressSize = (DvmType)ceil( (double)B->Set[i+1].Size /
                                   (double)B->Set[i+1].Step );
        Weight[i] = Weight[i+1] * CompressSize;
     }
  }

  return;
}  



s_BLOCK  block_Compress(s_BLOCK  *B)
{ s_BLOCK  Res;
  int      i;

  Res.Rank = B->Rank;

  for(i=0; i < Res.Rank; i++)
     Res.Set[i] = rset_Build(B->Set[i].Lower,
                             B->Set[i].Lower +
                             (DvmType)ceil((double)B->Set[i].Size /
                                         (double)B->Set[i].Step ) - 1,
                             1 );

  return  Res;
} 



byte  GetStepSign(s_BLOCK  *B)
{ int  i, Rank;

  Rank = B->Rank;

  for(i=0; i < Rank; i++)
      if(B->Set[i].Step != 1)
         return 1;

  return 0;
}



int  GetOnlyAxis(s_BLOCK  *B)
{ int  i, Rank, OnlyAxis = -1;

  Rank = B->Rank;

  for(i=0; i < Rank; i++)
  {  if(B->Set[i].Lower == B->Set[i].Upper)
        continue;
     if(OnlyAxis < 0)
         OnlyAxis = i;
     else
         return  -1;
  }

  if(OnlyAxis < 0)
     return  Rank-1;
  return OnlyAxis;
}


/* ------------------------------------------------- */    /*E0006*/


void  CopyBlockToMem(char *Mem, s_BLOCK *MemBlock, s_DISARRAY *DArr,
                     byte StepSign)
{ char    *Ptr;
  s_BLOCK  CurrBlock;
  int      TLen;
  DvmType     Index[MAXARRAYDIM + 1];

  if(StepSign == 0)
     CopyBlock2Mem(Mem, MemBlock, &DArr->ArrBlock);
  else
  {  Ptr = Mem;
     CurrBlock = block_Copy(MemBlock);
     TLen = (int)DArr->TLen;

     while( spind_FromBlock(Index, &CurrBlock, MemBlock, 1) )
     {  GetLocElm(DArr, &Index[1], Ptr)
        Ptr += TLen;
     }
  }

  return;
}



void  CopyBlock2Mem(char *Mem, s_BLOCK *MemBlock, s_ARRBLOCK *B)
{ char         *pSB, *pB;
  int           i, Rank;
  DvmType          ContSize, Count;
  DvmType          sI[MAXARRAYDIM + 1];

  pSB  = (char *)Mem;
  pB   = (char *)B->ALoc.Ptr;
  Rank = B->Block.Rank;
  sI[0] = Rank;

  PrepareCopy(Rank,pB,B->Block,*MemBlock,sI,ContSize,Count,B->TLen);

  for (i = 0; i < Count; i++)
  {  dvm_memcopy((char *)pSB, pB, ContSize);
     pB   = (char *)B->ALoc.Ptr;
     IncCopyPtr(Rank,pB,pSB,B->Block,*MemBlock,ContSize,sI,B->TLen);
  }

  return;
}



void  CopyMemToBlock(s_DISARRAY *DArr, char *Mem, s_BLOCK *MemBlock,
                     byte StepSign)
{ char    *Ptr;
  s_BLOCK  CurrBlock;
  int      TLen;
  DvmType     Index[MAXARRAYDIM + 1];

  if(StepSign == 0)
     CopyMem2Block(&DArr->ArrBlock, Mem, MemBlock);
  else
  {  Ptr = Mem;
     CurrBlock = block_Copy(MemBlock);
     TLen = (int)DArr->TLen;

     while( spind_FromBlock(Index, &CurrBlock, MemBlock, 1) )
     {  PutLocElm(Ptr, DArr, &Index[1])
        Ptr += TLen;
     }
  }

  return;
}



void  CopyMem2Block(s_ARRBLOCK *B, char *Mem, s_BLOCK *MemBlock)
{ char         *pSB, *pB;
  int           i, Rank;
  DvmType          ContSize, Count;
  DvmType          sI[MAXARRAYDIM + 1];

  pSB   = (char *)Mem;
  pB    = (char *)B->ALoc.Ptr;
  Rank  = B->Block.Rank;
  sI[0] = Rank;

  PrepareCopy(Rank, pB, B->Block, *MemBlock, sI, ContSize,
              Count, B->TLen);

  for (i = 0; i< Count; i++)
  {
     dvm_memcopy(pB, (char *)pSB, ContSize);
     pB = (char *)B->ALoc.Ptr;
     IncCopyPtr(Rank, pB, pSB, B->Block, *MemBlock, ContSize,
                sI, B->TLen);
  }

  return;
}



void  CopyMemToSubmem(void *SMem, s_BLOCK *SB, void *Mem,
                      s_BLOCK *MB, int TLen, byte StepSign)
{ s_BLOCK  CompressSB, CompressMB;

  if(StepSign == 0)
     CopyMem2Submem(SMem, SB, Mem, MB, TLen);
  else
  {  CompressSB = block_Compress(SB); 
     CompressMB = block_Compress(MB); 
     CopyMem2Submem(SMem, &CompressSB, Mem, &CompressMB, TLen);
  }

  return;
}



void  CopyMem2Submem(void *SMem, s_BLOCK *SB, void *Mem,
                     s_BLOCK *MB, int TLen)
{ char         *pSB, *pB;
  int           i, Rank;
  DvmType          ContSize, Count;
  DvmType          sI[MAXARRAYDIM + 1];

  pSB   = (char *)SMem;
  pB    = (char *)Mem;
  Rank  = MB->Rank;
  sI[0] = Rank;

  PrepareCopy(Rank, pB, *MB, *SB, sI, ContSize, Count, TLen);

  for (i=0; i < Count; i++)
  {  dvm_memcopy(pSB, pB, ContSize);
     pB   = (char *)Mem;
     IncCopyPtr(Rank, pB, pSB, *MB, *SB, ContSize,  sI, TLen);
  }

  return;
}



void  CopySubmemToMem(void *Mem, s_BLOCK *MB, void *SMem,
                      s_BLOCK *SB, int TLen, byte StepSign)
{ s_BLOCK  CompressMB, CompressSB;

  if(StepSign == 0)
     CopySubmem2Mem(Mem, MB, SMem, SB, TLen);
  else
  {  CompressMB = block_Compress(MB); 
     CompressSB = block_Compress(SB); 
     CopySubmem2Mem(Mem, &CompressMB, SMem, &CompressSB, TLen);
  }

  return;
}



void  CopySubmem2Mem(void *Mem,s_BLOCK *MB,void *SMem,s_BLOCK *SB,
                     int TLen)
{ char         *pSB, *pB;
  int           i, Rank;
  DvmType          ContSize, Count;
  DvmType          sI[MAXARRAYDIM + 1];

  pSB   = (char *)SMem;
  pB    = (char *)Mem;
  Rank  = MB->Rank;
  sI[0] = Rank;

  PrepareCopy(Rank, pB, *MB, *SB, sI, ContSize, Count, TLen);

  for (i=0; i < Count; i++)
  {  dvm_memcopy(pB, pSB, ContSize);
     pB   = (char *)Mem;
     IncCopyPtr(Rank, pB, pSB, *MB, *SB, ContSize,  sI, TLen);
  }

  return;
}



int   block_Intersect(s_BLOCK *ResB, s_BLOCK *B1, s_BLOCK *B2,
                      s_BLOCK *ComB, byte StepSign)
{ int   i, Rank, Res = FALSE;
  DvmType  Step, Lower;

  if(StepSign == 0)
     return  BlockIntersect(ResB, B1, B2);

  Rank = ComB->Rank;

  if( Rank == B1->Rank && BlockIntersect(ResB, B1, B2) )
  {  /* Blocks are intersected.  
        Correct ResB using steps for all dimensions */    /*E0007*/

     for(i=0; i < Rank; i++)
     {  Step  = ComB->Set[i].Step;
        Lower = ComB->Set[i].Lower;

        ResB->Set[i].Lower = Lower + Step *
        (DvmType)ceil( (double)(ResB->Set[i].Lower-Lower) /
                    (double)Step );

        ResB->Set[i].Upper = Lower + Step *
        ( (ResB->Set[i].Upper-Lower) / Step );

        ResB->Set[i].Size = ResB->Set[i].Upper - ResB->Set[i].Lower + 1;

        if(ResB->Set[i].Size < 0)
           break;

        ResB->Set[i].Step = Step;
     }

     Res = (i == Rank);
  }

  return  Res;
}



int  BlockIntersect(s_BLOCK *ResB, s_BLOCK *B1, s_BLOCK *B2)
{ int             i, Rank, Res = 0;
  s_REGULARSET   *D1, *D2, *RD;

  if(B1 && B2)
  {  Rank = B1->Rank;

     if(Rank == B2->Rank)
     {  ResB->Rank = (byte)Rank;

        for(i=0; i < Rank; i++)
        { D1 = &B1->Set[i];
          D2 = &B2->Set[i];
          RD = &ResB->Set[i];

          RD->Lower = dvm_max(D1->Lower, D2->Lower);
          RD->Size  = dvm_min( (D1->Lower+D1->Size),
                               (D2->Lower+D2->Size) ) - RD->Lower;
          RD->Upper = RD->Lower + RD->Size - 1;
          RD->Step  = dvm_min(D1->Step, D2->Step);

          Res = (RD->Size > 0);

          if(Res == 0)
             break;
        }
     }
  }

  return  Res;
}


#endif /* _SPACE_C_ */    /*E0008*/
