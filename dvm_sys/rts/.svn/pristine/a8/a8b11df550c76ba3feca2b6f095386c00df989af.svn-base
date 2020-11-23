#ifndef _TABLE_C_
#define _TABLE_C_
/***************/

/*********************************\
*  Functions to work with tables  *
\*********************************/

void table__Init(TABLE* pTable, size_t nTableSize, size_t nElemSize, PFN_TABLE_ELEMDESTRUCTOR pfnDestruct)
{
    pTable->IsInit = 0;

    pTable->TableSize = nTableSize;
    pTable->ElemSize = nElemSize;
    pTable->CurSize = 0;
    pTable->Destruct = pfnDestruct;
    pTable->lastAccessed = -1;
}

void table_Done(TABLE* pTable)
{
    table_RemoveAll(pTable);
    if (pTable->IsInit)
        coll_Done(&(pTable->cTable));

    pTable->IsInit = 0;
    pTable->TableSize = 0;
    pTable->ElemSize = 0;
    pTable->CurSize = 0;
    pTable->Destruct = NULL;
}

DvmType table_Count(TABLE* pTable)
{
    return !pTable->IsInit || pTable->cTable.Count == 0 ?
        0 : (pTable->cTable.Count - 1) * pTable->TableSize + pTable->CurSize;
}

void* table__At(TABLE* pTable, DvmType lNo)
{
    void* pChunk;

    if (lNo >= table_Count(pTable) || lNo < 0)
    {
        eprintf(__FILE__, __LINE__, "*** TABLE error: index out of range. Index = %ld\n", lNo);
    }

    pTable->lastAccessed = lNo;
    pChunk = coll_At(void*, &(pTable->cTable), (size_t)(lNo / pTable->TableSize));
    return (void*)((char*)pChunk + (size_t)((lNo % pTable->TableSize) * pTable->ElemSize));

}

DvmType table__Put(TABLE* pTable, void* pStruct)
{
    void* pChunk;
    DvmType lRes;

    if (!pTable->IsInit)
    {
        pTable->IsInit = 1;
        pTable->cTable = coll_Init(2, 1, NULL);
    }

    if ((pTable->cTable.Count == 0) || (pTable->CurSize == pTable->TableSize))
    {
        pTable->CurSize = 0;
        dynmem_calloc(pChunk, void*, pTable->TableSize, pTable->ElemSize);
        coll_Insert(&(pTable->cTable), pChunk);
    }
    else
    {
        pChunk = coll_At(void*, &(pTable->cTable), pTable->cTable.Count - 1);
    }

    dvm_memcopy((void*)((char*)pChunk + pTable->CurSize * pTable->ElemSize), pStruct, pTable->ElemSize);
    lRes = (pTable->cTable.Count - 1) * pTable->TableSize + pTable->CurSize;
    if ((UDvmType)lRes >= (UDvmType)MAXLONG)
    {
        eprintf(__FILE__, __LINE__, "*** TABLE error: table is full. Count = %lu\n", (UDvmType)lRes);
    }
    pTable->CurSize++;

    return lRes;
}

void* table__GetBack(TABLE* pTable)
{
    DvmType lCount = table_Count(pTable);
    if (lCount == 0)
    {
        eprintf(__FILE__, __LINE__, "*** TABLE error: table is empty.\n");
        return NULL;
    }

    return table__At(pTable, lCount - 1);
}

void* table__GetNew(TABLE* pTable)
{
    return table__At(pTable, table_GetNewNo(pTable));
}

DvmType table_GetNewNo(TABLE* pTable)
{
    void* pChunk;
    DvmType lRes = -1;

    if (!pTable->IsInit)
    {
        pTable->IsInit = 1;
        pTable->cTable = coll_Init(2, 1, NULL);
    }

    if ((pTable->cTable.Count == 0) || (pTable->CurSize == pTable->TableSize))
    {
        pTable->CurSize = 0;
        dynmem_calloc(pChunk, void*, pTable->TableSize, pTable->ElemSize);
        coll_Insert(&(pTable->cTable), pChunk);
    }
    else
    {
        pChunk = coll_At(void*, &(pTable->cTable), pTable->cTable.Count - 1);
    }

    lRes = (pTable->cTable.Count - 1) * pTable->TableSize + pTable->CurSize;
    if ((UDvmType)lRes >= (UDvmType)MAXLONG)
    {
        eprintf(__FILE__, __LINE__, "*** TABLE error: table is full. Count = %lu\n", (UDvmType)lRes);
    }

    pTable->CurSize++;
    return lRes;
}

void table_RemoveFrom(TABLE* pTable, DvmType lIndex)
{
    DvmType l;

    if (pTable->IsInit)
    {
        if (pTable->Destruct != NULL)
        {
            for (l = table_Count(pTable) - 1; l > lIndex; l--)
                (pTable->Destruct)(table__At(pTable, l));
        }

        coll_FreeFrom(&(pTable->cTable), (int)(lIndex / pTable->TableSize));
        pTable->CurSize = (size_t)(lIndex % pTable->TableSize + 1);
    }
}

void table_RemoveLast(TABLE* pTable)
{
    DvmType lCount = table_Count(pTable);
    if (lCount == 0)
    {
        eprintf(__FILE__, __LINE__, "*** TABLE error: table is empty.\n");
        return;
    }

    if (lCount > 1)
        table_RemoveFrom(pTable, lCount - 2);
    else
        table_RemoveAll(pTable);
}

void table_RemoveAll(TABLE* pTable)
{
    DvmType l;

    if (pTable->IsInit)
    {
        if (pTable->Destruct != NULL)
        {
            for (l = table_Count(pTable) - 1; l >= 0; l--)
                (pTable->Destruct)(table__At(pTable, l));
        }

        coll_FreeFrom(&(pTable->cTable), 0);
        pTable->CurSize = 0;
    }
}

void table__Iterator(TABLE* pTable, PFN_TABLEITERATION pfnProc, void* pParam1, void* pParam2)
{
    int i;
    size_t j;
    char* pChunk;

    if (pTable->IsInit)
    {
        for (i = 0; i < pTable->cTable.Count - 1; i++)
        {
            pChunk = coll_At(char*, &(pTable->cTable), i);
            for (j = 0; j < pTable->TableSize; j++)
                pfnProc((void*)(pChunk + j * pTable->ElemSize), pParam1, pParam2);
        }

        pChunk = coll_At(char*, &(pTable->cTable), pTable->cTable.Count - 1);
        for (j = 0; j < pTable->CurSize; j++)
            pfnProc((void*)(pChunk + j * pTable->ElemSize), pParam1, pParam2);
    }
}

DvmType table_GetLastAccessed(TABLE* pTable)
{
	return pTable->lastAccessed;
}


#endif /* _TABLE_C_ */
