#ifndef _HASH_C_
#define _HASH_C_
/**************/

/***************************************\
*  Functions to work with HASH-indexes  *
\***************************************/

/********************************************************************/

void hash_Init(HASH_TABLE* pHashTable, short sKeyRank, size_t nIndexSize, int nTableSize, PFN_CALC_HASH_FUNC pfnHash)
{
    HASH_LIST* pFirstElem = NULL;

    table_Init(&(pHashTable->tElements), nTableSize,
        sizeof(struct tag_HASH_LIST) + sizeof(DvmType) * (sKeyRank - 1), NULL);

    dynmem_calloc(pHashTable->plIndex, DvmType*, nIndexSize, sizeof(DvmType));
    dynmem_calloc(pHashTable->puElements, UDvmType*, nIndexSize, sizeof(UDvmType));
    pHashTable->nIndexSize = nIndexSize;
    pHashTable->sKeyRank = sKeyRank;
    pHashTable->pfnHashFunc = (pfnHash != NULL ? pfnHash : StandartHashCalc);

    /* First element of HASH-table is not used
       as it number is equal 0 */

    pFirstElem = table_GetNew(HASH_LIST, &(pHashTable->tElements));
    SYSTEM(memset, (pFirstElem, 0, sizeof(struct tag_HASH_LIST) + sizeof(DvmType) * (sKeyRank - 1)));

    pHashTable->lStatPut = pHashTable->lStatFind = pHashTable->lStatCompare = 0l;
}

void hash_Done(HASH_TABLE* pHashTable)
{
    table_Done(&(pHashTable->tElements));

    dynmem_free((void **)&(pHashTable->plIndex), pHashTable->nIndexSize * sizeof(DvmType));
    dynmem_free((void **)&(pHashTable->puElements), pHashTable->nIndexSize * sizeof(UDvmType));

    pHashTable->nIndexSize = 0;
    pHashTable->sKeyRank = 0;
    pHashTable->pfnHashFunc = NULL;

    pHashTable->lStatPut = pHashTable->lStatFind = pHashTable->lStatCompare = 0l;
}

/* 0 >= - found */
/* -1   - not found */

DvmType hash_Find(HASH_TABLE* pHashTable, DvmType* plKey)
{
    DvmType lIndex;
    HASH_LIST* pHashElem = NULL;
    DvmType lRes = -1;

    pHashTable->lStatFind++;

    lIndex = pHashTable->plIndex[pHashTable->pfnHashFunc(pHashTable, plKey)];

    while (lIndex > 0)
    {
        pHashElem = table_At(HASH_LIST, &(pHashTable->tElements), lIndex);
        if (pHashElem == NULL)
            break;

        pHashTable->lStatCompare++;

        if (hash_CompareKeys(pHashTable->sKeyRank, plKey, pHashElem->rgKey))
        {
            lRes = pHashElem->lValue;
            break;
        }
        else
        {
            lIndex = pHashElem->lNextElem;
        }
    }

    return lRes;
}

void hash_Insert(HASH_TABLE* pHashTable, DvmType* plKey, DvmType lValue)
{
    HASH_LIST* pHashElem = NULL;
    HASH_VALUE nHashValue;
    DvmType lIndex;
    short i;

    nHashValue = pHashTable->pfnHashFunc(pHashTable, plKey);

    pHashTable->lStatPut++;
    pHashTable->puElements[nHashValue]++;

    lIndex = table_GetNewNo(&(pHashTable->tElements));
    pHashElem = table_At(HASH_LIST, &(pHashTable->tElements), lIndex);

    pHashElem->lNextElem = pHashTable->plIndex[nHashValue];
    pHashElem->lValue = lValue;

    if (pHashTable->sKeyRank == 1)
    {
        pHashElem->rgKey[0] = *plKey;
    }
    else
    {
        for (i = 0; i < pHashTable->sKeyRank; i++)
            pHashElem->rgKey[i] = plKey[i];
    }

    pHashTable->plIndex[nHashValue] = lIndex;
}

void hash_Change(HASH_TABLE* pHashTable, DvmType* plKey, DvmType lValue)
{
    DvmType lIndex;
    HASH_LIST* pHashElem = NULL;

    lIndex = pHashTable->plIndex[pHashTable->pfnHashFunc(pHashTable, plKey)];

    while (lIndex > 0)
    {
        pHashElem = table_At(HASH_LIST, &(pHashTable->tElements), lIndex);
        if (pHashElem == NULL )
            break;

        if (hash_CompareKeys(pHashTable->sKeyRank, plKey, pHashElem->rgKey))
        {
            pHashElem->lValue = lValue;
            break;
        }
        else
        {
            lIndex = pHashElem->lNextElem;
        }
    }
}

void hash_Remove(HASH_TABLE* pHashTable, DvmType* plKey)
{
    DvmType lIndex;
    HASH_LIST* pCurrElem = NULL;
    HASH_LIST* pPrevElem = NULL;
    HASH_VALUE nHashValue;

    nHashValue = pHashTable->pfnHashFunc(pHashTable, plKey);

    lIndex = pHashTable->plIndex[nHashValue];

    while (lIndex > 0)
    {
        pCurrElem = table_At(HASH_LIST, &(pHashTable->tElements), lIndex);
        if (pCurrElem == NULL)
            break;

        if (hash_CompareKeys(pHashTable->sKeyRank, plKey, pCurrElem->rgKey))
        {
            if (pPrevElem == NULL)
            {
                pHashTable->plIndex[nHashValue] = pCurrElem->lNextElem;
            }
            else
            {
                pPrevElem->lNextElem = pCurrElem->lNextElem;
            }
            break;
        }
        else
        {
            pPrevElem = pCurrElem;
            lIndex = pCurrElem->lNextElem;
        }
    }
}

void hash_Iterator(HASH_TABLE* pHashTable, PFN_HASHITERATION pfnProc, void* pParam)
{
    DvmType lIndex;
    size_t i;
    HASH_LIST* pHashElem = NULL;

    for (i = 0; i < pHashTable->nIndexSize; i++)
    {
        lIndex = pHashTable->plIndex[i];

        while (lIndex > 0)
        {
            pHashElem = table_At(HASH_LIST, &(pHashTable->tElements), lIndex);
            if (pHashElem == NULL )
                break;

            pfnProc(pHashElem->lValue, pParam);

            lIndex = pHashElem->lNextElem;
        }
    }
}

void hash_RemoveAll(HASH_TABLE* pHashTable)
{
    table_RemoveAll(&(pHashTable->tElements));
    pHashTable->tElements.CurSize = 1;

    SYSTEM(memset, (pHashTable->plIndex, 0, pHashTable->nIndexSize * sizeof(DvmType)));
}

int hash_CompareKeys(short sKeyRank, DvmType* plKey1, DvmType* plKey2)
{
    short i;

    if (sKeyRank == 1)
        return *plKey1 == *plKey2;

    for (i = 0; i < sKeyRank; i++)
    {
        if (plKey1[i] != plKey2[i])
            return 0;
    }

    return 1;
}

void hash_PrintStatistics(HASH_TABLE* pHashTable)
{
    unsigned i;
    char str[71], buff[20];
    int Len1, Len2;

    pprintf(0, "'Put' operations : %lu\n", pHashTable->lStatPut);
    pprintf(0, "'Find' operations : %lu\n", pHashTable->lStatFind);
    pprintf(0, "'Compare' operations : %lu\n", pHashTable->lStatCompare);
    pprintf(0, "HashTable elements :\n");

    for (*str = 0, i = 0; i < pHashTable->nIndexSize; i++)
    {
        SYSTEM(sprintf, (buff, (pHashTable->puElements[i] != 0 ? "%lu/" : "-/"), pHashTable->puElements[i]));
        SYSTEM_RET(Len1, strlen, (str));
        SYSTEM_RET(Len2, strlen, (buff));
        if (Len1 + Len2 > 70)
        {
            pprintf(0, "%s\n", str );
            *str = 0;
        }
        SYSTEM(strcat, (str, buff));
    }
    if (*str != 0)
        pprintf(0, "%s\n", str );
}

HASH_VALUE StandartHashCalc(HASH_TABLE* pHashTable, DvmType* plKey)
{
    short i;
    DvmType lSum = 0;

    if (pHashTable->sKeyRank == 1)
        return (HASH_VALUE)(plKey[0] % pHashTable->nIndexSize);

    for (i = 0; i < pHashTable->sKeyRank; i++)
    {
        lSum += plKey[i];
    }

    return (HASH_VALUE)(lSum % pHashTable->nIndexSize);
}

HASH_VALUE OffsetHashCalc(HASH_TABLE* pHashTable, DvmType* plKey)
{
    short i;
    DvmType lSum = 0;

    if (pHashTable->sKeyRank == 1)
        return (HASH_VALUE)((plKey[0] >> HashOffsetValue) % pHashTable->nIndexSize);

    for (i = 0; i < pHashTable->sKeyRank; i++)
    {
        lSum += plKey[i];
    }

    return (HASH_VALUE)((lSum >> HashOffsetValue) % pHashTable->nIndexSize);
}

#endif  /* _HASH_C_ */
