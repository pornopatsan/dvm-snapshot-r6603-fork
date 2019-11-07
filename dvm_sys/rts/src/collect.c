#ifndef _COLLECT_C_
#define _COLLECT_C_
/*****************/    /*E0000*/

 
s_COLLECTION  coll__Init(int  Reserv, int  CountInc,
                         t_Destructor  *RecDestr)
{
   s_COLLECTION  Coll;
 
   Coll.Count    = 0;
   Coll.CountInc = CountInc;
   Coll.Reserv   = Reserv;
   dvm_AllocArray(void *, Coll.Reserv, Coll.List);
   Coll.RecDestr = RecDestr;

   return  Coll;
}



int   coll__AtInsert(s_COLLECTION  *Coll, int  Index, void  *NewRec)
{
   int       i;
   void    **wP;
   void    **wPP;

   if((Index > Coll->Count) OR (Index >= MaxCollCount))
      epprintf(MultiProcErrReg1,__FILE__,__LINE__,
               "*** RTS err: wrong call coll__AtInsert "
               "(Index >= MaxCollCount)" );

   Coll->Count ++;
 
   if(Coll->Count > Coll->Reserv)
   {
      Coll->Reserv += Coll->CountInc;

      if(Coll->Reserv > MaxCollCount)
         Coll->Reserv = MaxCollCount;

      dvm_ReallocArray(void *, Coll->Reserv, Coll->List);
   }
 
   wP  = &(Coll->List[Coll->Count-1]);
   wPP = wP-1;

   for(i=Coll->Count-1; i > Index; i--,wP--,wPP--)
       *wP = *wPP;

   Coll->List[Index] = NewRec;
 
   return  Index;
}



void   coll_AtDelete(s_COLLECTION  *Coll, int  Index)
{
   int      i;
   void   **wP;
   void   **wPP;
 
   if((Index >= 0) AND (Index < Coll->Count))
   {
      wP = &(Coll->List[Index]);
      wPP = wP + 1;

      for(i=Index; i < Coll->Count-1; i++,wP++,wPP++)
          *wP=*wPP;

      Coll->Count --;

      if(Coll->Count < Coll->Reserv - Coll->CountInc)
      {
         Coll->Reserv -= Coll->CountInc;
 
         if(Coll->Reserv <= 0)
            Coll->Reserv = Coll->CountInc;

         dvm_ReallocArray(void *, Coll->Reserv, Coll->List);
      }
   }
}



int   coll__IndexOf(s_COLLECTION  *Coll, void  *Record)
{ 
   int         i;
   void      **wP;

   wP = Coll->List;

   for(i=0; i < Coll->Count; i++,wP++)
       if(*wP == Record)
          break;

   if(i == Coll->Count)
      i = -1;
   
   return  i;
}



void   coll_DeleteAll(s_COLLECTION  *Coll)
{
   dvm_FreeArray(Coll->List);
   *Coll = coll__Init(Coll->CountInc, Coll->CountInc, Coll->RecDestr);

  return;
}



void   coll_AtFree(s_COLLECTION  *Coll, int  Index)
{ 
   void  *Record;

   if((Index >= 0) AND (Index < Coll->Count))
   {
      Record = Coll->List[Index];

      if(Coll->RecDestr != NULL)
        ( RTL_CALL, (*Coll->RecDestr)(Record) );

      dvm_FreeStruct(Record);
      coll_AtDelete(Coll, Index);
   }

   return;
}



void   coll_FreeFrom(s_COLLECTION  *Coll, int  FromIndex)
{
   int  i;

   for(i=Coll->Count-1; i > FromIndex; i--)
       coll_AtFree(Coll, i);

   return;
}



void   coll_ObjFreeFrom(s_COLLECTION  *Coll, byte  ObjType,
                        int  BlockInd)
{
   int                 i;
   s_DISARRAY         *DArr;
   s_VMS              *VMS;
   s_AMVIEW           *AMV;
   s_BOUNDGROUP       *BG;
   s_REDVAR           *RV;
   s_REDGROUP         *RG;
   s_ARRAYMAP         *DArrMap;
   s_AMVIEWMAP        *AMVMap;
   s_REGBUFGROUP      *REGBuf;
   s_DACONSISTGROUP   *DAG;
   s_IDBUFGROUP       *IDBuf;

   switch(ObjType)
   {
     case sht_DisArray   :

          for(i=Coll->Count-1; i >= 0; i--)
          {
              DArr = coll_At(s_DISARRAY *, Coll, i);

              if(DArr->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_VMS        :

          for(i=Coll->Count-1; i >= 0; i--)
          {
              VMS = coll_At(s_VMS *, Coll, i);

              if(VMS->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_AMView     :

          for(i=Coll->Count-1; i >= 0; i--)
          {
              AMV = coll_At(s_AMVIEW *, Coll, i);

              /* set flag to allow delayed deletion for
                 s_AMVIEW objects from finished context. */
              if(AMV->HandlePtr->InitCrtBlockInd == BlockInd)
                 AMV->HandlePtr->InitCrtBlockInd = 0;

              if((AMV->HandlePtr->CrtBlockInd == BlockInd) ||
                ((AMV->ArrColl.Count == 0) && (AMV->HandlePtr->InitCrtBlockInd == 0)))
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_BoundsGroup:

          if(ShgSave)
             break;

          for(i=Coll->Count-1; i >= 0; i--)
          {
              BG = coll_At(s_BOUNDGROUP *, Coll, i);

              if(BG->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_RedVar     :

          if(RgSave)
             break;

          for(i=Coll->Count-1; i >= 0; i--)
          {
              RV = coll_At(s_REDVAR *, Coll, i);

              if(RV->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_RedGroup   :

          if(RgSave)
             break;

          for(i=Coll->Count-1; i >= 0; i--)
          {
              RG = coll_At(s_REDGROUP *, Coll, i);

              if(RG->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_ArrayMap   :

          for(i=Coll->Count-1; i >= 0; i--)
          {
              DArrMap = coll_At(s_ARRAYMAP *, Coll, i);

              if(DArrMap->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_AMViewMap  :

          for(i=Coll->Count-1; i >= 0; i--)
          {
              AMVMap = coll_At(s_AMVIEWMAP *, Coll, i);

              if(AMVMap->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_RegBufGroup:

          for(i=Coll->Count-1; i >= 0; i--)
          {
              REGBuf = coll_At(s_REGBUFGROUP *, Coll, i);

              if(REGBuf->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_DAConsistGroup:

          for(i=Coll->Count-1; i >= 0; i--)
          {
              DAG = coll_At(s_DACONSISTGROUP *, Coll, i);

              if(DAG->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;

     case sht_IdBufGroup:

          for(i=Coll->Count-1; i >= 0; i--)
          {
              IDBuf = coll_At(s_IDBUFGROUP *, Coll, i);

              if(IDBuf->HandlePtr->CrtBlockInd == BlockInd)
                 coll_AtFree(Coll, i);
          }

          break;
   }

   return;
}



void   coll__Free(s_COLLECTION  *Coll, void  *Record)
{
   if(Record != NULL)
   {
      if(Coll->RecDestr != NULL)
         ( RTL_CALL, (*Coll->RecDestr)(Record) );

      coll_Delete(Coll, Record);
      dvm_FreeStruct(Record);
   }

   return;
}



void   coll_Done(s_COLLECTION  *Coll)
{
   int    i;
   void  *Record;

   for(i=0; i < Coll->Count; i++)
   {
      Record = Coll->List[i];

      if(Coll->RecDestr != NULL)
        ( RTL_CALL, (*Coll->RecDestr)(Record) );

      dvm_FreeStruct(Record);
   }

   dvm_FreeArray(Coll->List);
   Coll->Count    = 0;
   Coll->Reserv   = 0;
   Coll->CountInc = 0;

   return;
}


#endif  /* _COLLECT_C_ */    /*E0001*/
