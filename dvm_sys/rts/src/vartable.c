#ifndef _VARTABLE_C_
#define _VARTABLE_C_
/******************/

/******************************************\
*  Functions to work with variable tables  *
\******************************************/

/********************************************************************/

void vartable_Init(VAR_TABLE* pVarTable, int nVarTableSize, int nIndexSize, int nHashTableSize, PFN_CALC_HASH_FUNC pfnHashFunc)
{
    hash1_Init(&(pVarTable->hIndex), nIndexSize, nHashTableSize, pfnHashFunc);
    table_Init(&(pVarTable->vTable), nVarTableSize, sizeof(struct tag_VarInfo), NULL);
}

void vartable_Done(VAR_TABLE* pVarTable)
{
    table_Done(&(pVarTable->vTable));
    hash_Done(&(pVarTable->hIndex));
}


/* return Number of Variable in VAR_TABLE */

VarInfo* vartable_GetVarInfo(VAR_TABLE* pVarTable, DvmType NoVar )
{
    return table_At(VarInfo, &(pVarTable->vTable), NoVar);
}

VarInfo* vartable_FindVar(VAR_TABLE* pVarTable, void* pAddr)
{
    DvmType lNo;
    VarInfo* pRes = NULL;

    lNo = hash1_Find(&(pVarTable->hIndex), pAddr);

    if (lNo != -1)
    {
        pRes = vartable_GetVarInfo(pVarTable, lNo);
        if (pRes->Busy == 0)
            pRes = NULL;
    }

    return pRes;
}

DvmType vartable_FindNoVar(VAR_TABLE* pVarTable, void* pAddr)
{
    return hash1_Find(&(pVarTable->hIndex), pAddr);
}

DvmType vartable_PutVariable(VAR_TABLE* pVarTable, void* pAddr, int nEnv, byte bType, SysHandle* pHandle,
                           int nTag, void* pInfo, PFN_VARTABLE_ELEMDESTRUCTOR pfnDestructor)
{
    DvmType lRes;
    VarInfo Var;
    VarInfo* pPrev = NULL;

    Var.Busy = 1;
    Var.VarAddr = pAddr;
    Var.PrevNo = hash1_Find(&(pVarTable->hIndex), pAddr);
    Var.EnvirIndex = nEnv;
    Var.Type = bType;
    Var.Handle = pHandle;
    Var.Info = pInfo;
    Var.Tag = nTag;
    Var.IsInit = 0;
    Var.pfnDestructor = pfnDestructor;

    /* inherit the initialization flag from the previous definition */

    if (Var.PrevNo != -1)
    {
        pPrev = vartable_GetVarInfo(pVarTable, Var.PrevNo);
        if (pPrev != NULL)
        {
            Var.IsInit = pPrev->IsInit;
        }
    }


    lRes = table_Put(&(pVarTable->vTable), &Var);

    if (Var.PrevNo != -1)
        hash1_Change(&(pVarTable->hIndex), pAddr, lRes);
    else
        hash1_Insert(&(pVarTable->hIndex), pAddr, lRes);

    return lRes;
}

void vartable_VariableDone(VarInfo* pVar)
{
    if (pVar != NULL)
    {
        pVar->Busy = 0;
        if(pVar->pfnDestructor != NULL)
           pVar->pfnDestructor(pVar);
    }
}

void vartable_RemoveVariable(VAR_TABLE* pVarTable, void* pAddr)
{
    VarInfo* pVar = NULL;

    pVar = vartable_FindVar(pVarTable, pAddr);
    if (pVar != NULL)
    {
        if (pVar->PrevNo != -1)
            hash1_Change(&(pVarTable->hIndex), pAddr, pVar->PrevNo);
        else
            hash1_Remove(&(pVarTable->hIndex), pAddr);

        vartable_VariableDone(pVar);
    }
}

void IterRemoveProc(DvmType lNoVar, VAR_TABLE* pVarTable)
{
    vartable_VariableDone(vartable_GetVarInfo(pVarTable, lNoVar));
}

void vartable_RemoveAll(VAR_TABLE* pVarTable)
{
    hash_Iterator(&(pVarTable->hIndex), (PFN_HASHITERATION)IterRemoveProc, pVarTable);
    hash_RemoveAll(&(pVarTable->hIndex));
}

void IterLevelProc(VarInfo* pVar, int* pnLevel, VAR_TABLE* pVarTable)
{
    if ((pVar->EnvirIndex >= *pnLevel) && pVar->Busy)
    {
        vartable_VariableDone(pVar);

        if (pVar->PrevNo != -1)
            hash1_Change(&(pVarTable->hIndex), pVar->VarAddr, pVar->PrevNo);
        else
            hash1_Remove(&(pVarTable->hIndex), pVar->VarAddr);
    }
}

void vartable_RemoveVarOnLevel(VAR_TABLE* pVarTable, int nLevel)
{
    table_Iterator(&(pVarTable->vTable), IterLevelProc, &nLevel, pVarTable);
}

void vartable_Iterator(VAR_TABLE* pVarTable, PFN_VARTABLEITERATION pfnFunc)
{
    DvmType lCount;
    DvmType i;
    VarInfo* pVar = NULL;

    lCount = table_Count(&(pVarTable->vTable));
    for (i = 0; i < lCount; i++)
    {
        pVar = table_At(VarInfo, &(pVarTable->vTable), i);
        if (pVar->Busy)
        {
            pfnFunc(pVar);
        }
    }
}

void vartable_LevelIterator(VAR_TABLE* pVarTable, int nLevel, PFN_VARTABLEITERATION pfnFunc)
{
    DvmType lCount;
    DvmType i;
    VarInfo* pVar = NULL;

    lCount = table_Count(&(pVarTable->vTable));
    for (i = 0; i < lCount; i++)
    {
        pVar = table_At(VarInfo, &(pVarTable->vTable), i);
        if (pVar->Busy && pVar->EnvirIndex == nLevel)
        {
            pfnFunc(pVar);
        }
    }
}

#endif  /* _VARTABLE_C_ */
