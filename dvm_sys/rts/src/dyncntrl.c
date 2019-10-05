#ifndef _DYNCNTRL_C_
#define _DYNCNTRL_C_
/******************/

/*******************************\
* Functions for dynamic control *
\*******************************/

void dyn_Init(void)
{
    switch (HashMethod)
    {
        case 0 : GlobalHashFunc = StandartHashCalc; break;
        case 1 : GlobalHashFunc = OffsetHashCalc; break;
    }
    if (HashOffsetValue > 16) HashOffsetValue = 16;

    if (EnableDynControl)
    {
        if (ProcCount > 1)
            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
                "*** RTS err: DYNCONTROL: Dynamic control can be performed only on a single processor\n");

        if (!DebugOptions.AppendErrorFile)
            SYSTEM(remove, (DebugOptions.ErrorFile));

        vartable_Init(&DynVarTable, DebugOptions.VarTableSize, DebugOptions.HashIndexSize, DebugOptions.HashTableSize, GlobalHashFunc);
        error_Init(&DynControlErrors, DebugOptions.MaxErrors);
        DynVarTableInit = 1;
        g_pLastTestedDisArray = NULL;
    }
}

void dyn_Done( void )
{
    if (DynVarTableInit)
    {
        dyn_RemoveAll();
        error_DynControlPrintAll();
    }

    if (EnableDynControl)
    {
        if (DynDebugPrintStatistics && MPS_CurrentProc == DVM_IOProc &&
            DbgInfoPrint && _SysInfoPrint)
        {
            pprintf(0, "*** Dynamic control statistics ***\n");
            hash_PrintStatistics(&DynVarTable.hIndex);
        }
    }

    if (DynVarTableInit)
    {
        vartable_Done(&DynVarTable);
        error_Done(&DynControlErrors);
        DynVarTableInit = 0;
    }
}

void dyn_LevelDone(void)
{
    vartable_RemoveVarOnLevel(&DynVarTable, (int)cntx_GetParallelDepth());
}

void dyn_RemoveAll(void)
{
    vartable_RemoveAll(&DynVarTable);
}

VarInfo *dyn_GetVarInfo(void *addr)
{
    return vartable_FindVar(&DynVarTable, addr);
}

void dyn_CheckVar(char* szOperand, void* pAddr, SysHandle* pHandle, byte bIsWrt)
{
    VarInfo* pVar;
    void* pVAddr;
    size_t lLineIndex;

    pVAddr = pHandle != NULL ? pHandle->pP : pAddr;

    pVar = dyn_GetVarInfo(pVAddr);
    if (bIsWrt != 2)
        pVar = dyn_CheckValidVar(pVar, pAddr, pHandle, bIsWrt);

    if (pVar != NULL)
    {
        lLineIndex = dyn_CalcLI(pVar, pAddr);

        switch (pVar->Type)
        {
            case dtype_READONLY :
                dyn_CheckReadOnlyVar(szOperand, pVar, bIsWrt, lLineIndex);
                break;
            case dtype_PRIVATE :
                dyn_CheckPrivateVar(szOperand, pVar, bIsWrt, lLineIndex);
                break;
            case dtype_REDUCT :
                dyn_CheckReductVar(szOperand, pVar, bIsWrt, lLineIndex);
                break;
            case dtype_DISARRAY :
                dyn_CheckDisArrayVar(szOperand, pVar, pAddr, bIsWrt, lLineIndex);
                break;
        }
    }
}

VarInfo* dyn_CheckValidVar(VarInfo* pCurrent, void* pAddr, SysHandle* pHandle, byte bIsWrt)
{
    VarInfo* pRes = pCurrent;

    /* Variable is not found or not defined at current level */

    if (!pCurrent || (pCurrent->EnvirIndex != (int)cntx_GetParallelDepth()))
    {
        if (pCurrent != NULL)
        {
            switch (pCurrent->Type)
            {
                case dtype_DISARRAY :
                    /* Duplicate variable at next level for initialization */

                    pRes = dyn_DefineDisArray(pHandle, 0, (DISARR_INFO*)pCurrent->Info);
                    dyn_DisArrClearInfo((DISARR_INFO*)pCurrent->Info);
                    break;
                case dtype_REDUCT :
                    pRes = dyn_DefineReduct(0, pAddr);
                    break;
                case dtype_READONLY :
                    pRes = dyn_DefineReadOnly(pAddr, pCurrent->Info);
                    break;
                default :
                    pRes = bIsWrt ? dyn_DefinePrivate(pAddr, 0) : dyn_DefineReadOnly(pAddr, pCurrent->Info);
            }
        }
        else
            pRes = dyn_DefinePrivate(pAddr, 0);
    }

    return pRes;
}

void dyn_CheckReadOnlyVar(char* szOperand, VarInfo *pVar, byte bIsWrt, DvmType lLineIndex)
{
    if (bIsWrt == 1)
        error_DynControl(ERR_DYN_WRITE_RO, szOperand);
    else if (bIsWrt == 0)
        if (!dyn_InitializeCheck(pVar, lLineIndex))
            error_DynControl(ERR_DYN_PRIV_NOTINIT, szOperand);
}

void dyn_CheckPrivateVar(char* szOperand, VarInfo* pVar, byte bIsWrt, DvmType lLineIndex)
{
    DvmType lIter;
    DvmType lSrcLineIndex = -1;
    PRIVATE_INFO* pInfo;

    if (bIsWrt == 0 || bIsWrt == 1)
    {
        pInfo = (PRIVATE_INFO*)(pVar->Info);
        if (pInfo->RemoteBuffer)
            lSrcLineIndex = dyn_Remap(lLineIndex, pVar);

        if (bIsWrt)
        {
            if (pInfo->RemoteBuffer)
                error_DynControl(ERR_DYN_WRITE_REMOTEBUFF, szOperand);
            else
                dyn_InitializeSet(pVar, lLineIndex);
        }
        else
        {
            if (!dyn_InitializeCheck(pVar, lLineIndex))
                error_DynControl(ERR_DYN_PRIV_NOTINIT, szOperand);
        }

        if (pInfo->RemoteBuffer && lSrcLineIndex != -1)
        {
            lIter = cntx_GetAbsoluteParIter();
            if (lIter != -1l)
                dyn_DisArrCheckDataDepends(szOperand, pInfo->RB_Source, NULL, lSrcLineIndex, bIsWrt, lIter);

            if (!bIsWrt)
                if (!dyn_InitializeCheck(pInfo->RB_Source, lSrcLineIndex))
                    error_DynControl(ERR_DYN_DISARR_NOTINIT, szOperand);
        }
    }
}

void dyn_CheckReductVar(char* szOperand, VarInfo* pVar, byte bIsWrt, DvmType lLineIndex)
{
    if (bIsWrt == 0 || bIsWrt == 1)
    {
        if (pVar->Tag == dar_WAIT || pVar->Tag == dar_STARTED)
            error_DynControl(ERR_DYN_ARED_NOCOMPLETE, szOperand);

        if (bIsWrt == 1)
            dyn_InitializeSet(pVar, lLineIndex);
        else
            if (!dyn_InitializeCheck(pVar, lLineIndex))
                error_DynControl(ERR_DYN_PRIV_NOTINIT, szOperand);
    }
}

void dyn_CheckDisArrayVar(char* szOperand, VarInfo* pVar, void* pElmAddr, byte bIsWrt, DvmType lLineIndex)
{
    DvmType lIter;
    DISARR_INFO* pInfo;
    DvmType lSrcLineIndex;

    pInfo = (DISARR_INFO*)(pVar->Info);
    lIter = cntx_GetAbsoluteParIter();
    lSrcLineIndex = dyn_Remap(lLineIndex, pVar);

    if (!dyn_DisArrCheckLimits(szOperand, lLineIndex, pInfo))
        return;

    if (!pInfo->RemoteBuffer && !cntx_IsParallelLevel())
        dyn_DisArrCheckSequential(szOperand, pVar, bIsWrt, lLineIndex);

    if (pInfo->RemoteBuffer && lSrcLineIndex != -1)
        if (!dyn_DisArrCheckLimits(szOperand, lSrcLineIndex, (DISARR_INFO*)(pInfo->RB_Source->Info)))
            return;

    if (bIsWrt != 2)
    {
        if (!pInfo->RemoteBuffer)
            dyn_DisArrCheckBounds(szOperand, pVar, lLineIndex, bIsWrt, lIter);

        if (pInfo->RemoteBuffer)
        {
            if (lSrcLineIndex != -1)
                dyn_DisArrCheckDataDepends(szOperand, pInfo->RB_Source, NULL, lSrcLineIndex, bIsWrt, lIter);
        }
        else
            dyn_DisArrCheckDataDepends(szOperand, pVar, pElmAddr, lLineIndex, bIsWrt, lIter);
    }

    if (bIsWrt == 1)
    {
        if (pInfo->RemoteBuffer)
            error_DynControl(ERR_DYN_WRITE_REMOTEBUFF, szOperand);
        else
            dyn_InitializeSet(pVar, lLineIndex);
    }
    else if (bIsWrt == 0)
    {
        if (!dyn_InitializeCheck(pVar, lLineIndex))
            error_DynControl(ERR_DYN_DISARR_NOTINIT, szOperand);

        if (pInfo->RemoteBuffer && lSrcLineIndex != -1)
            if (!dyn_InitializeCheck(pInfo->RB_Source, lSrcLineIndex))
                error_DynControl(ERR_DYN_DISARR_NOTINIT, szOperand);
    }
}

VarInfo *dyn_DefineVar( byte type, byte Stat, void *addr, SysHandle *Handle, void *Info, PFN_VARTABLE_ELEMDESTRUCTOR pfnDestructor)
{
    DvmType Index;
    void* VAddr;

    VAddr = Handle ? Handle->pP : addr;

    Index = vartable_PutVariable(&DynVarTable, VAddr, Stat ? 0 : (int)cntx_GetParallelDepth(), type, Handle, 0, Info, pfnDestructor);

    return vartable_GetVarInfo(&DynVarTable, Index);
}

VarInfo *dyn_DefineReduct( byte Stat, void *addr )
{
    return dyn_DefineVar(dtype_REDUCT, Stat, addr, NULL, NULL, dyn_DestructReduct);
}

void dyn_DestructReduct(VarInfo* pVar)
{
    VarInfo* pPrev = NULL;

    if (pVar->PrevNo != -1)
    {
        pPrev = vartable_GetVarInfo(&DynVarTable, pVar->PrevNo);
        if (pPrev != NULL)
        {
            if (pPrev->Type == dtype_REDUCT && pVar->Tag == dar_CALC)
                pPrev->Tag = dar_WAIT;
            pPrev->IsInit = pVar->IsInit;
        }
        switch (pVar->Tag)
        {
            case dar_WAIT :
                error_DynControl(ERR_DYN_REDUCT_NOT_STARTED);
                break;
            case dar_STARTED :
                error_DynControl(ERR_DYN_REDUCT_START_WITHOUT_WAIT);
                break;
            case dar_COMPLETED :
            case dar_CALC :
            default :
                break;
        }
    }
    else
    {
        /* define private variable with init flag */

        VarInfo* pNewVar = dyn_DefinePrivate(pVar->VarAddr, 0);
        pNewVar->IsInit = pVar->IsInit;
    }
}

VarInfo *dyn_DefineDisArray(SysHandle* Handle, byte Stat, DISARR_INFO* Info)
{
    if( Info == NULL )
    {
        if( Handle && ( Handle->Type == sht_DisArray ) )
        {
            dynmem_CallocStruct( struct tag_DISARR_INFO, Info );
            Info->ArrSize = (size_t)space_GetSize( &(((s_DISARRAY *)(Handle->pP))->Space) );

            dynmem_CallocArray( byte, Info->ArrSize, Info->Init );
            dynmem_CallocArray(DvmType, Info->ArrSize, Info->Iter);
            dynmem_CallocArray( short, Info->ArrSize, Info->Attr );
            Info->CurrIter = MAXLONG;
            Info->CollapseRank = (byte)-1;

            Info->collShadows = coll_Init(1, 1, NULL);

            /* info->LocalBlock = NULL; */

            Info->HasLocal = 0;
            Info->FlashBoundState = 0;
            Info->Across = 0;
            Info->DisableLocalCheck = 0;
            Info->ShadowCalculated = 0;

            Info->RemoteBuffer = 0;
            Info->RB_Source = NULL;
            Info->RB_Rank = (byte)0;
            Info->RB_MapIndex = NULL;
        }
    }

    return dyn_DefineVar(dtype_DISARRAY, Stat, NULL, Handle, (void *)Info, dyn_DestructDisArray);
}

void dyn_DestructDisArray(VarInfo* Var)
{
    void* VoidPtr;
    DISARR_INFO* pArrInfo = NULL;

    if (Var->Info)
    {
        pArrInfo = (DISARR_INFO *)Var->Info;
        if (Var->PrevNo == -1)
        {
            VoidPtr = (void *)pArrInfo->Init;
            dynmem_free(&VoidPtr, pArrInfo->ArrSize * sizeof(byte));

            VoidPtr = (void *)pArrInfo->Iter;
            dynmem_free(&VoidPtr, pArrInfo->ArrSize * sizeof(DvmType));

            VoidPtr = (void *)pArrInfo->Attr;
            dynmem_free(&VoidPtr, pArrInfo->ArrSize * sizeof(short));

            VoidPtr = (void *)pArrInfo->RB_MapIndex;
            dynmem_free(&VoidPtr, pArrInfo->RB_Rank * sizeof(DvmType));

            pArrInfo->LocalBlock.Rank = 0;
            pArrInfo->RB_Rank = 0;
            pArrInfo->RB_Source = NULL;

            coll_Done(&pArrInfo->collShadows);

            dynmem_free(&(Var->Info), sizeof(struct tag_DISARR_INFO));
        }
        else
        {
            pArrInfo->Across = 0;
            pArrInfo->DisableLocalCheck = 0;
            if (pArrInfo->FlashBoundState)
            {
                dyn_DisArrClearShadows(pArrInfo);
                pArrInfo->FlashBoundState = 0;
            }
            Var->Info = NULL;
        }
    }
}

VarInfo *dyn_DefinePrivate( void *addr, byte Stat )
{
    PRIVATE_INFO *info = NULL;

    dynmem_CallocStruct( struct tag_PRIVATE_INFO, info );
    info->RemoteBuffer = 0;

    return dyn_DefineVar( dtype_PRIVATE, Stat, addr, NULL, (void *)info, dyn_DestructPrivate );
}

void dyn_DestructPrivate( VarInfo *Var )
{
    VarInfo *Prev;
    void    *VoidPtr;

    if (Var->PrevNo != -1)
    {
        Prev = vartable_GetVarInfo( &DynVarTable, Var->PrevNo );
        dyn_InitializeClear(Prev);
    }

    if (Var->Info)
    {
        VoidPtr = (void *)((PRIVATE_INFO *)Var->Info)->RB_MapIndex;
        dynmem_free(&VoidPtr, ((PRIVATE_INFO *)Var->Info)->RB_Rank * sizeof(DvmType));

        dynmem_free(&(Var->Info), sizeof(struct tag_PRIVATE_INFO));
    }
}

VarInfo *dyn_DefineReadOnly(void *addr, void *Info)
{
    return dyn_DefineVar(dtype_READONLY, 0, addr, NULL, Info, dyn_DestructReadOnly);
}

void dyn_DestructReadOnly(VarInfo* Var)
{
    if (Var->Info && Var->PrevNo == -1)
    {
		dynmem_free(&(Var->Info), 0);
    }
}

VarInfo *dyn_DefineRemoteBufferArray(SysHandle *SrcHandle, SysHandle *DstHandle, DvmType *Index)
{
    VarInfo *SrcVarInfo, *DstVarInfo;
    DISARR_INFO *Info;
    byte ArrRank;

    ArrRank = ((s_DISARRAY *)(SrcHandle->pP))->Space.Rank;

    SrcVarInfo = dyn_GetVarInfo( SrcHandle->pP );
    if( SrcVarInfo == NULL )
        SrcVarInfo = dyn_DefineDisArray( SrcHandle, ((s_DISARRAY *)(SrcHandle->pP))->Static, NULL );

    dyn_DisArrClearInfo( (DISARR_INFO *)SrcVarInfo->Info );

    DstVarInfo = dyn_GetVarInfo( DstHandle->pP );
    if( DstVarInfo == NULL )
        DstVarInfo = dyn_DefineDisArray( DstHandle, ((s_DISARRAY *)(DstHandle->pP))->Static, NULL );

    Info = (DISARR_INFO *)(DstVarInfo->Info);
    Info->RemoteBuffer = (byte)1;
    Info->RB_Rank = ArrRank;
    Info->RB_Source = SrcVarInfo;
    dynmem_AllocArray(DvmType, ArrRank, Info->RB_MapIndex);
    dvm_ArrayCopy(DvmType, Info->RB_MapIndex, Index, ArrRank);

    return DstVarInfo;
}

VarInfo *dyn_DefineRemoteBufferScalar(SysHandle *SrcHandle, void *RmtBuff, DvmType *Index)
{
    VarInfo *SrcVarInfo, *DstVarInfo;
    PRIVATE_INFO *Info;
    byte ArrRank;

    ArrRank = ((s_DISARRAY *)(SrcHandle->pP))->Space.Rank;

    SrcVarInfo = dyn_GetVarInfo( SrcHandle->pP );
    if( SrcVarInfo == NULL )
        SrcVarInfo = dyn_DefineDisArray( SrcHandle, ((s_DISARRAY *)(SrcHandle->pP))->Static, NULL );

    dyn_DisArrClearInfo( (DISARR_INFO *)SrcVarInfo->Info );

    DstVarInfo = dyn_GetVarInfo( RmtBuff );
    if( DstVarInfo == NULL )
        DstVarInfo = dyn_DefinePrivate( RmtBuff, (byte)0 );

    dyn_InitializeSet(DstVarInfo, 0);

    Info = (PRIVATE_INFO *)(DstVarInfo->Info);
    Info->RemoteBuffer = (byte)1;
    Info->RB_Rank = ArrRank;
    Info->RB_Source = SrcVarInfo;
    dynmem_AllocArray(DvmType, ArrRank, Info->RB_MapIndex);
    dvm_ArrayCopy(DvmType, Info->RB_MapIndex, Index, ArrRank);

    return DstVarInfo;
}

void dyn_RemoveVar( void *addr )
{
    vartable_RemoveVariable( &DynVarTable, addr );
}

/******************************************************************/

void dyn_InitializeSetArr(SysHandle* pHandle)
{
    VarInfo* pVar = dyn_GetVarInfo(pHandle->pP);
    pVar = dyn_CheckValidVar(pVar, NULL, pHandle, 1);

    if (pVar != NULL)
    {
        memset(((DISARR_INFO*)(pVar->Info))->Init, 0xFF, ((DISARR_INFO*)(pVar->Info))->ArrSize);
    }
}

void dyn_InitializeSetScal(void* pAddr)
{
    VarInfo* pVar = dyn_GetVarInfo(pAddr);
    pVar = dyn_CheckValidVar(pVar, pAddr, NULL, 1);

    if (pVar != NULL)
    {
        pVar->IsInit = 1;
    }
}

void dyn_InitializeSet(VarInfo* pVar, size_t LI)
{
    if (pVar->Handle != NULL)
    {
        ((DISARR_INFO*)(pVar->Info))->Init[LI] = 0xFF;
    }
    else
        pVar->IsInit = 1;
}

void dyn_InitializeClear(VarInfo* pVar)
{
    if (pVar->Handle != NULL)
    {
        SYSTEM(memset, (((DISARR_INFO*)(pVar->Info))->Init, 0, ((DISARR_INFO*)(pVar->Info))->ArrSize));
    }
    else
        pVar->IsInit = 0;
}

int dyn_InitializeCheck(VarInfo* pVar, size_t LI)
{
    int nRes;

    if (pVar->Handle != NULL)
    {
        nRes = (((DISARR_INFO*)(pVar->Info))->Init[LI] != 0);
    }
    else
        nRes = (pVar->IsInit != 0);

    return nRes;
}

void dyn_DisArrClearInfo(DISARR_INFO* pInfo)
{
    if (pInfo != NULL)
    {
        SYSTEM(memset, (pInfo->Iter, 0, pInfo->ArrSize * sizeof(DvmType)));
        SYSTEM(memset, (pInfo->Attr, 0, pInfo->ArrSize * sizeof(short)));
        pInfo->CurrIter = MAXLONG;
        pInfo->CollapseRank = (byte)-1;
    }
}

void dyn_DisArrDisableLocalCheck(SysHandle* pHandle)
{
    DISARR_INFO* pInfo = NULL;
    VarInfo* pVar = dyn_GetVarInfo(pHandle->pP);
    if (pVar != NULL)
    {
        pInfo = (DISARR_INFO *)(pVar->Info);
        pInfo->DisableLocalCheck = 1;
    }
}

void dyn_DisArrCheckBounds(char *Operand, VarInfo *Var, size_t LI, byte isWrt, DvmType Iter)
{
    s_PARLOOP* PL = NULL;
    s_DISARRAY* DA = NULL;
    DISARR_INFO* DAInfo = NULL;

    DA = (s_DISARRAY *)(Var->Handle->pP);
    DAInfo = (DISARR_INFO *)(Var->Info);

    dyn_CalcSI(LI, &(DA->ArrBlock.Block), &(DA->ArrBlock.sI));

    PL = dyn_GetCurrLoop();
    if (PL == NULL || Iter == -1)
    {
        return;
    }

    if (DAInfo->DisableLocalCheck != 0)
    {
        return;
    }

    /* Check edges of element */

    if (DAInfo->CurrIter != Iter)
    {
        DAInfo->CurrIter = Iter;
        DAInfo->HasLocal = (byte)((RTL_CALL, dyn_GetLocalBlock(&(DAInfo->LocalBlock), DA, PL)) == 0);
    }

    if (DAInfo->HasLocal)
    {
        if (dyn_InsideBlock(&(DAInfo->LocalBlock), DA->ArrBlock.sI))
        {
            /* reset an attribute of edge exchange */

            /*
            if (isWrt && PL->IterFlag == ITER_NORMAL)
            {
                DAInfo->FlashBoundState = 1;
            }
            */

            if (PL->AddBnd)
            {
                dyn_DisArrDefineShadowCompute(DA, DAInfo);
            }
            else if (isWrt)
            {
                DAInfo->FlashBoundState = 1;
            }

        }
        else
        {
            int nShadow = dyn_FindShadowIndex(&(DAInfo->LocalBlock), DA->ArrBlock.sI, DAInfo);
            if (nShadow != -1)
            {
                if (isWrt)
                {
                    if (DAInfo->Across != 2)
                        error_DynControl(ERR_DYN_WRITE_IN_BOUND, Operand);
                }
                else
                {
                    DISARR_SHADOW* pArrShadow = coll_At(DISARR_SHADOW*, &DAInfo->collShadows, nShadow);
                    if (!pArrShadow->bValid)
                    {
                        if (pArrShadow->pBndGroup != NULL)
                        {
                            if (!pArrShadow->pBndGroup->IsStrtsh)
                            {
                                error_DynControl(ERR_DYN_NONLOCAL_ACCESS, Operand);
                            }
                            else if (PL->IterFlag != ITER_BOUNDS_LAST)
                            {
                                error_DynControl(ERR_DYN_BOUND_RENEW_NOCOMPLETE, Operand);
                            }
                        }
                        else
                            error_DynControl(ERR_DYN_NONLOCAL_ACCESS, Operand);
                    }
                }
            }
            else
                error_DynControl(ERR_DYN_NONLOCAL_ACCESS, Operand);
        }
    }
    else
        error_DynControl(ERR_DYN_NONLOCAL_ACCESS, Operand);
}

void dyn_DisArrCheckSequential(char* szOperand, VarInfo* pVar, byte bIsWrt, DvmType lLineIndex)
{
    DISARR_INFO* pInfo;

    pInfo = (DISARR_INFO*)(pVar->Info);

    if (g_pLastTestedDisArray == NULL)
    {
        /* No array element was tested. Display the error */

        if (bIsWrt == 1)
            error_DynControl(ERR_DYN_SEQ_WRITEARRAY, szOperand);
        else if (bIsWrt == 0)
            error_DynControl(ERR_DYN_SEQ_READARRAY, szOperand);
    }
    else if (pVar->Handle != g_pLastTestedDisArray)
    {
        /* Access to other arrays */

        if (((s_DISARRAY*)(pVar->Handle->pP))->AMView !=
            ((s_DISARRAY*)(g_pLastTestedDisArray->pP))->AMView)
        {
            if (bIsWrt == 1)
                error_DynControl(ERR_DYN_SEQ_WRITEARRAY, szOperand);
            else if (bIsWrt == 0)
                error_DynControl(ERR_DYN_SEQ_READARRAY, szOperand);
        }
    }
    else
    {
        /* Access to tested array */

    }

    if (bIsWrt == 1)
    {
        g_pLastTestedDisArray = NULL;
        dyn_DisArrClearShadows(pInfo);
    }
}

void dyn_DisArrCheckDataDepends(char* szOperand, VarInfo* pVar, void* pElmAddr, size_t lLineIndex, byte bIsWrt, DvmType lIter)
{
    DISARR_INFO* pInfo = NULL;
    VarInfo* pElmVar = NULL;
    s_DISARRAY* pArrInfo = NULL;
    int i = 0;

    if (!cntx_IsParallelLevel() || lIter == -1)
        return;

    pInfo = (DISARR_INFO *)(pVar->Info);

    /* Verify the case of using distributed array for calculating reduction */

    pElmVar = dyn_GetVarInfo(pElmAddr);
    if (NULL != pElmVar && dtype_REDUCT == pElmVar->Type)
    {
        /* The element of distributed array registered as reduction variable */

        if ((byte)-1 == pInfo->CollapseRank)
        {
            /* The collapse rank is not calculated yet. Calculate it. */

            pArrInfo = (s_DISARRAY*)(pVar->Handle->pP);
            for (i = 0, pInfo->CollapseRank = 0; i < pArrInfo->AMView->Space.Rank; i++)
            {
                if (map_COLLAPSE == pArrInfo->AMView->DISTMAP[i].Attr)
                    pInfo->CollapseRank++;
            }
        }

        /* If the collapse rank is not equal to 0 then this is multiplicated array. */
        /* Don't verify data dependency for it. */

        if (0 != pInfo->CollapseRank)
            return;
    }

    /* Check data dependencies in loop */

    if ((pInfo->Attr[lLineIndex] & DDS_ACCESSED) == 0)
    {
        /* The element is not accessed */

        pInfo->Attr[lLineIndex] = (short)(DDS_ACCESSED | (bIsWrt ? DDS_WRITE : DDS_READ) | DDS_SINGLEITER);
        pInfo->Iter[lLineIndex] = lIter;
    }
    else
    {
        if (pInfo->Iter[lLineIndex] != lIter)
        {
            if (0 != (pInfo->Attr[lLineIndex] & DDS_WRITE) || (bIsWrt && lIter < pInfo->Iter[lLineIndex]))
            {
                if (!pInfo->Across)
                    error_DynControl(ERR_DYN_DATA_DEPEND, szOperand);
            }
            pInfo->Attr[lLineIndex] |= DDS_MULTIPLEITERS;
        }

        if (bIsWrt)
            pInfo->Attr[lLineIndex] |= DDS_WRITE;

        pInfo->Iter[lLineIndex] = lIter;
    }
}

byte dyn_DisArrCheckLimits(char* szOperand, DvmType LI, DISARR_INFO* pInfo)
{
    if (LI < 0 || (size_t)LI >= pInfo->ArrSize)
    {
        error_DynControl(ERR_DYN_DISARR_LIMIT, szOperand);
        eprintf(__FILE__, __LINE__, "*** DYNCONTROL *** : Using element outside of array limits: %s\n", szOperand);
        return 0;
    }

    return 1;
}

void dyn_DisArrTestVal(SysHandle* Handle, DvmType* InitIndex, DvmType* LastIndex, DvmType* Step)
{
    size_t  elem;
    VarInfo *Var;
    byte Rank;
    int i;
    s_SPACE *ArrSpace;
    DvmType *SI;
    int Finish = 0;

    if( Handle != NULL && InitIndex != NULL &&
        LastIndex != NULL && Step != NULL )
    {
        Var = dyn_GetVarInfo( Handle->pP );
        Var = dyn_CheckValidVar( Var, NULL, Handle, 0 );

        if( Var && Var->Handle )
        {
            ArrSpace = &(((s_DISARRAY *)(Handle->pP))->Space);
            Rank = ArrSpace->Rank;
            SI = spind_Init( Rank );

            for( i = 0; i < Rank; i++ )
                SI[i+1] = InitIndex[i];

            while( !Finish )
            {
                elem = (size_t)space_GetLI( ArrSpace, SI );
                if( !dyn_DisArrCheckLimits( "", elem, (DISARR_INFO *)(Var->Info) ) )
                    break;

                if( ((DISARR_INFO *)(Var->Info))->Init[elem] == 0 )
                {
                    error_DynControl( ERR_DYN_DISARR_NOTINIT, "" );
                }

                for( i = Rank - 1; i >= 0; i-- )
                {
                    SI[i+1] += Step[i];
                    if( SI[i+1] > LastIndex[i] )
                    {
                        if( i == 0 )
                        {
                            Finish = 1;
                            break;
                        }
                        else
                            SI[i+1] = InitIndex[i];
                    }
                    else
                        break;
                }
            }

            spind_Done( &SI );
        }
    }
}

void dyn_DisArrSetVal(SysHandle *Handle, DvmType *InitIndex, DvmType *LastIndex, DvmType *Step)
{
    size_t  elem;
    VarInfo *Var;
    byte Rank;
    int i;
    s_SPACE *ArrSpace;
    DvmType *SI;
    int Finish = 0;

    if( Handle != NULL && InitIndex != NULL &&
        LastIndex != NULL && Step != NULL )
    {
        Var = dyn_GetVarInfo( Handle->pP );
        Var = dyn_CheckValidVar( Var, NULL, Handle, 0 );

        if( Var && Var->Handle )
        {
            ArrSpace = &(((s_DISARRAY *)(Handle->pP))->Space);
            Rank = ArrSpace->Rank;
            SI = spind_Init( Rank );

            for( i = 0; i < Rank; i++ )
                SI[i+1] = InitIndex[i];

            while( !Finish )
            {
                elem = (size_t)space_GetLI( ArrSpace, SI );
                if( !dyn_DisArrCheckLimits( "", elem, (DISARR_INFO *)(Var->Info) ) )
                    break;

                ((DISARR_INFO *)(Var->Info))->Init[elem] = 0xFF;

                for( i = Rank - 1; i >= 0; i-- )
                {
                    SI[i+1] += Step[i];
                    if( SI[i+1] > LastIndex[i] )
                    {
                        if( i == 0 )
                        {
                            Finish = 1;
                            break;
                        }
                        else
                            SI[i+1] = InitIndex[i];
                    }
                    else
                        break;
                }
            }

            spind_Done( &SI );
        }
    }
}

void dyn_DisArrDefineShadow(s_DISARRAY* pArr, s_BOUNDGROUP* pBndGroup, s_SHDWIDTH* pRtsShadow)
{
    int i;
    int nRank;
    DISARR_SHADOW* pArrShadow = NULL;
    DISARR_INFO* pInfo = NULL;
    VarInfo* pVar = dyn_GetVarInfo(pArr);

    if (pVar != NULL)
    {
        pInfo = (DISARR_INFO*)pVar->Info;
        if (pInfo != NULL)
        {
            nRank = pArr->Space.Rank;
            dynmem_CallocStruct(DISARR_SHADOW, pArrShadow);
            for (i = 0; i < nRank; i++)
            {
                pArrShadow->ResLowShdWidth[i] = pRtsShadow->ResLowShdWidth[i];
                pArrShadow->ResHighShdWidth[i] = pRtsShadow->ResHighShdWidth[i];
                pArrShadow->ShdSign[i] = pRtsShadow->ShdSign[i];
            }
            pArrShadow->MaxShdCount = pRtsShadow->MaxShdCount;
            pArrShadow->pBndGroup = pBndGroup;
            pArrShadow->bValid = 0;

            coll_Insert(&pInfo->collShadows, pArrShadow);
        }
    }
}

void dyn_DisArrDefineShadowCompute(s_DISARRAY* pArr, DISARR_INFO* pInfo)
{
    int i;
    int nRank;
    DISARR_SHADOW* pArrShadow = NULL;

    if (!pInfo->ShadowCalculated)
    {
        nRank = pArr->Space.Rank;
        dynmem_CallocStruct(DISARR_SHADOW, pArrShadow);
        for (i = 0; i < nRank; i++)
        {
            pArrShadow->ResLowShdWidth[i] = pArr->InitLowShdWidth[i];
            pArrShadow->ResHighShdWidth[i] = pArr->InitHighShdWidth[i];
            pArrShadow->ShdSign[i] = 7;
        }
        pArrShadow->MaxShdCount = nRank;
        pArrShadow->pBndGroup = NULL;
        pArrShadow->bValid = 1;

        coll_Insert(&pInfo->collShadows, pArrShadow);
        pInfo->ShadowCalculated = 1;
    }
}

void dyn_DisArrCompleteShadows(s_DISARRAY* pArr, s_BOUNDGROUP* pBndGroup)
{
    int i;
    DISARR_SHADOW* pArrShadow = NULL;
    DISARR_INFO* pInfo = NULL;
    VarInfo* pVar = dyn_GetVarInfo(pArr);

    if (pVar != NULL)
    {
        pInfo = (DISARR_INFO*)pVar->Info;
        if (pInfo != NULL)
        {
            for (i = 0; i < pInfo->collShadows.Count; i++)
            {
                pArrShadow = coll_At(DISARR_SHADOW*, &pInfo->collShadows, i);
                if (pBndGroup == pArrShadow->pBndGroup)
                {
                    pArrShadow->bValid = 1;
                }
            }
        }
    }
}

void dyn_DisArrClearShadows(DISARR_INFO* pInfo)
{
    int i = 0;
    DISARR_SHADOW* pArrShadow = NULL;

    for (i = 0; i < pInfo->collShadows.Count; i++)
    {
        pArrShadow = coll_At(DISARR_SHADOW*, &pInfo->collShadows, i);
        pArrShadow->bValid = 0;
    }
    pInfo->ShadowCalculated = 0;

    dyn_DisArrFreeUnusedShadows(pInfo);
}

void dyn_DisArrFreeUnusedShadows(DISARR_INFO* pInfo)
{
    int i = 0;
    DISARR_SHADOW* pArrShadow = NULL;

    while (i < pInfo->collShadows.Count)
    {
        pArrShadow = coll_At(DISARR_SHADOW*, &pInfo->collShadows, i);
        if (pArrShadow->pBndGroup == NULL && !pArrShadow->bValid)
        {
            coll_AtFree(&pInfo->collShadows, i);
        }
        else
            i++;
    }
}

void dyn_DisArrShadowGroupDeleted(s_BOUNDGROUP* pBndGroup)
{
    int i = 0;
    int j = 0;
    DISARR_SHADOW* pArrShadow = NULL;
    VarInfo* pVar = NULL;
    DISARR_INFO* pInfo = NULL;

    for (i = 0; i < pBndGroup->ArrayColl.Count; i++)
    {
        pVar = dyn_GetVarInfo(coll_At(void *, &pBndGroup->ArrayColl, i));
        if (pVar != NULL && pVar->Info != NULL)
        {
            pInfo = (DISARR_INFO*)(pVar->Info);
            for (j = 0; j < pInfo->collShadows.Count; j++)
            {
                pArrShadow = coll_At(DISARR_SHADOW*, &pInfo->collShadows, j);
                if (pArrShadow->pBndGroup == pBndGroup)
                    pArrShadow->pBndGroup = NULL;
            }
            dyn_DisArrFreeUnusedShadows(pInfo);
        }
    }
}

void dyn_DisArrTestElement(SysHandle* pHandle)
{
    g_pLastTestedDisArray = pHandle;
}

void dyn_DisArrAcross(s_DISARRAY* pArr, byte bAcrossType)
{
    DISARR_INFO* pInfo = NULL;
    VarInfo* pVar = dyn_GetVarInfo(pArr);

    if (pVar != NULL)
    {
        pInfo = (DISARR_INFO*)pVar->Info;
        if (pInfo != NULL)
        {
            pInfo->Across = bAcrossType;
        }
    }
}

int dyn_InsideBlock(s_BLOCK* pBlock, DvmType* sI)
{
    int i;

    if (pBlock == NULL || pBlock->Rank != sI[0])
        return 0;

    for (i = 0; i < pBlock->Rank; i++)
    {
        if ((sI[i+1] < pBlock->Set[i].Lower) ||
            (sI[i+1] > pBlock->Set[i].Lower + pBlock->Set[i].Size - 1))
            return 0;
    }

    return 1;
}

int dyn_FindShadowIndex(s_BLOCK* pBlock, DvmType* sI, DISARR_INFO* pInfo)
{
    int s, i;
    int nBounds;
    int nRes;
    int nLast = -1;
    DISARR_SHADOW* pArrShadow = NULL;

    for (s = 0; s < pInfo->collShadows.Count; s++)
    {
        pArrShadow = coll_At(DISARR_SHADOW*, &pInfo->collShadows, s);
        nBounds = 0;
        nRes = s;

        for (i = 0; i < pBlock->Rank; i++)
        {
            if ((sI[i+1] < pBlock->Set[i].Lower - pArrShadow->ResLowShdWidth[i]) ||
                (sI[i+1] > pBlock->Set[i].Lower + pBlock->Set[i].Size - 1 + pArrShadow->ResHighShdWidth[i]))
            {
                nRes = -1;
                break;
            }

            if ((sI[i+1] < pBlock->Set[i].Lower) ||
                (sI[i+1] > pBlock->Set[i].Lower + pBlock->Set[i].Size - 1))
            {
                nBounds++;
            }
        }
        if (nRes != -1 && nBounds <= pArrShadow->MaxShdCount)
        {
            nLast = nRes;
            if (pArrShadow->bValid)
                break;
        }
    }

    return nLast;
}

void dyn_AReductSetState(s_COLLECTION* pRedVars, byte bState)
{
    int i;
    int j;
    s_REDVAR* RVar;
    char* VarPtr = NULL;
    char* LocPtr = NULL;
    int nOffset = 0;
    int nSize = sizeof(long);

    for (i = 0; i < pRedVars->Count; i++)
    {
        RVar = coll_At(s_REDVAR *, pRedVars, i);

        VarPtr = RVar->Mem;
        LocPtr = RVar->LocMem;

        nSize = sizeof(long);
        switch (RVar->LocIndType)
        {
            case 0 : nSize = sizeof(long); break;
            case 1 : nSize = sizeof(int); break;
            case 2 : nSize = sizeof(short); break;
            case 3 : nSize = sizeof(char); break;
        }

        for (j = 0; j < RVar->VLength; j++)
        {
            dyn_AReductSetStateByAddr(VarPtr, bState);
            VarPtr += RVar->RedElmLength;

            if (LocPtr != NULL)
            {
                for (nOffset = 0; nOffset < RVar->LocElmLength; nOffset += nSize)
                    dyn_AReductSetStateByAddr(LocPtr + nOffset, bState);

                LocPtr += RVar->LocElmLength;
            }
        }
    }
}

void dyn_AReductSetStateByAddr(void* pAddr, byte bState)
{
    VarInfo* pVar = dyn_GetVarInfo(pAddr);

    if (pVar != NULL)
    {
        if (bState == dar_COMPLETED)
        {
            if (pVar->Tag != dar_STARTED)
                error_DynControl(ERR_DYN_REDUCT_WAIT_BSTART);

            pVar->Tag = bState;
            dyn_RemoveVar(pAddr);
        }
        else
            pVar->Tag = bState;
    }
}

s_PARLOOP* dyn_GetCurrLoop(void)
{
    s_ENVIRONMENT *Env;

    Env = genv_GetEnvironment( gEnvColl->Count - 1 );
    if( Env != NULL )
    {
        return Env->ParLoop;
    }

    return NULL;
}

void dyn_CalcSI(DvmType LI, s_BLOCK *pBlock, DvmType **psI)
{
    int    i;
    DvmType  *ip, mp;
    byte   Rank;

    Rank = pBlock->Rank;

    if( *psI != NULL && **psI != Rank )
        spind_Done( psI );
    if( *psI == NULL )
        *psI = spind_Init( Rank );

    for( i = Rank - 2, mp = 1; i >= 0; i-- )
        mp *= pBlock->Set[i+1].Size;

    for( ip = *psI + 1, i = 0; i < Rank; ip++, i++ )
    {
        *ip = LI / mp;
        LI -= (*ip)*mp;
        *ip += pBlock->Set[i].Lower;
        if( i + 1 < Rank )
            mp /= pBlock->Set[i+1].Size;
    }
}

size_t dyn_CalcLI( VarInfo *Var, void *addr )
{
    s_DISARRAY *DA;

    if( Var->Handle )
    {
        DA = (s_DISARRAY *)(Var->Handle->pP);
        return DYN_ARR_OFFSET( size_t, addr, DA->ArrBlock.ALoc.Ptr, DA->ArrBlock.TLen );
    }

    return 0;
}

DvmType dyn_Remap(DvmType lLineIndex, VarInfo* pVar)
{
    s_DISARRAY* DA;
    s_DISARRAY* DASrc;
    DISARR_INFO* DAInfo;
    PRIVATE_INFO* PVInfo;
    int i;
    DvmType *sI = NULL;
    size_t Res = (size_t)-1;

    if (dtype_DISARRAY == pVar->Type)
    {
        DAInfo = (DISARR_INFO *)(pVar->Info);

        if (DAInfo->RemoteBuffer && DAInfo->RB_Source != NULL)
        {
            DA = (s_DISARRAY *)(pVar->Handle->pP);
            DASrc = (s_DISARRAY *)(DAInfo->RB_Source->Handle->pP);
            sI = spind_Init(DAInfo->RB_Rank);

            dyn_CalcSI(lLineIndex, &(DA->ArrBlock.Block), &sI);
            if (sI[0] == (DvmType)(DAInfo->RB_Rank))
            {
                for (i = 0; i < DAInfo->RB_Rank; i++)
                {
                    if (DAInfo->RB_MapIndex[i] != -1)
                        sI[i + 1] = DAInfo->RB_MapIndex[i];
                }
                Res = (size_t)block_GetLI(&(DASrc->ArrBlock.Block), sI, 0);
            }
        }
    }
    else if (pVar->Type == dtype_PRIVATE)
    {
        PVInfo = (PRIVATE_INFO *)(pVar->Info);

        if (PVInfo->RemoteBuffer && PVInfo->RB_Source != NULL)
        {
            DASrc = (s_DISARRAY *)(PVInfo->RB_Source->Handle->pP);
            sI = spind_Init(PVInfo->RB_Rank);

            for( i = 0; i < PVInfo->RB_Rank; i++ )
            {
                if( PVInfo->RB_MapIndex[i] != -1 )
                    sI[ i+1 ] = PVInfo->RB_MapIndex[ i ];
            }
            Res = (size_t)block_GetLI( &(DASrc->ArrBlock.Block), sI, 0 );
        }
    }

    if( sI != NULL )
        spind_Done( &sI );

    return Res;
}


#endif  /* _DYNCNTRL_C_ */
