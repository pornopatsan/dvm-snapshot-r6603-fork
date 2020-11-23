 /***********************************************************
  *
  * History:
  *
  * $Log: trcreduc.c,v $
  * Revision 1.9  2006/03/29 18:26:59  atsign
  * no message
  *
  * Revision 1.8  2005/09/10 08:42:01  atsign
  * no message
  *
  * Revision 1.3  2004/10/02 08:18:53  Maxim V. Kudryavtsev
  * log message added to the top of file
  *
  *
  ************************************************************/

#ifndef _TRCREDUC_C_
#define _TRCREDUC_C_
/******************/

/****************************************\
* Functions to trace reduction variables *
\****************************************/

void trcreduct_Insert(s_REDVAR* RVar)
{
    REDUCT_INFO* pInfo = NULL;
    int nOffset = 0;
    int nSize = sizeof(long);
    DvmType lParDepth = 0;
    int i;
    char* VarPtr = RVar->Mem;
    char* LocPtr = RVar->LocMem;

    switch (RVar->LocIndType)
    {
        case 0 : nSize = sizeof(long); break;
        case 1 : nSize = sizeof(int); break;
        case 2 : nSize = sizeof(short); break;
        case 3 : nSize = sizeof(char); break;
        default:
            pprintf(3, "*** RTS warning (debugger) !!! \n");
    }

    if (RVar)
    {
        dynmem_AllocStruct(struct tag_REDUCT_INFO, pInfo);
        pInfo->Current = NULL;
        pInfo->Initial = NULL;
        pInfo->StartReduct = 0;
        pInfo->RVar = RVar;
    }

    lParDepth = cntx_GetInitParallelDepth();

    for (i = 0; i < RVar->VLength; i++)
    {
        vartable_PutVariable(&ReductVarTable, VarPtr, lParDepth, RVar->Func, 0,
            cntx_IsInitParLoop(), (void *)pInfo, trcreduct_VarDestructor);
        /* Only first element has pointer to variable information */
        pInfo = NULL;

        VarPtr += RVar->RedElmLength;

        if (LocPtr)
        {
            for (nOffset = 0; nOffset < RVar->LocElmLength; nOffset += nSize)
            {
                vartable_PutVariable(&ReductVarTable, LocPtr + nOffset, lParDepth,
                    RVar->Func, 0, 0, NULL, trcreduct_VarDestructor);
            }

            LocPtr += RVar->LocElmLength;
        }
    }

    if (ManualReductCalc)
        trcreduct_StoreInitial(RVar);
}

REDUCT_INFO* trcreduct_Find(void* addr)
{
    VarInfo* Var;

    Var = vartable_FindVar(&ReductVarTable, addr);

    return (Var ? (REDUCT_INFO *)(Var->Info) : NULL);
}

void trcreduct_Remove(s_REDVAR* RVar)
{
    int i;
    char* VarPtr = RVar->Mem;
    char* LocPtr = RVar->LocMem;
    int nOffset = 0;
    int nSize = sizeof(long);

    switch (RVar->LocIndType)
    {
        case 0 : nSize = sizeof(long); break;
        case 1 : nSize = sizeof(int); break;
        case 2 : nSize = sizeof(short); break;
        case 3 : nSize = sizeof(char); break;
    }

    for (i = 0; i < RVar->VLength; i++)
    {
        trcreduct_Complete(VarPtr, RVar->VType);
        vartable_RemoveVariable(&ReductVarTable, VarPtr);
        VarPtr += RVar->RedElmLength;
        if (LocPtr)
        {
            for (nOffset = 0; nOffset < RVar->LocElmLength; nOffset += nSize)
            {
                vartable_RemoveVariable(&ReductVarTable, LocPtr + nOffset);
            }
            LocPtr += RVar->LocElmLength;
        }
    }
}

int trcreduct_IsReduct(void* addr)
{
    int Res = 0;
    VarInfo* Var;

    Var = vartable_FindVar(&ReductVarTable, addr);

    if (Var)
    {
        if (Var->EnvirIndex == cntx_GetInitParallelDepth() - 1) /* parent */
            Res = 1;
    }

    return Res;
}

void trcreduct_VarDestructor(VarInfo* Var)
{
    REDUCT_INFO* pInfo = NULL;
    void* VoidPtr = NULL;

    if (NULL != Var->Info)
    {
        pInfo = (REDUCT_INFO*)Var->Info;

        if (NULL != pInfo->Current)
        {
            VoidPtr = (void*)pInfo->Current;
            dynmem_free(&VoidPtr, pInfo->RVar->BlockSize);
        }

        if (NULL != pInfo->Initial)
        {
            VoidPtr = (void*)pInfo->Initial;
            dynmem_free(&VoidPtr, pInfo->RVar->BlockSize);
        }

        dynmem_free(&(Var->Info), sizeof(struct tag_REDUCT_INFO));
    }
}

/********************************************************************/

void trcreduct_StoreInitial(s_REDVAR* RVar)
{
    REDUCT_INFO* pInfo;

    pInfo = trcreduct_Find(RVar->Mem);

    if (pInfo != NULL && pInfo->Initial == NULL && pInfo->StartReduct == 0)
    {
        dynmem_malloc(pInfo->Initial, char*, RVar->BlockSize);

        dvm_memcopy(pInfo->Initial, RVar->Mem,
                    (RVar->RedElmLength * RVar->VLength));
        if (RVar->LocElmLength > 0)
            dvm_memcopy((pInfo->Initial +
                         RVar->RedElmLength * RVar->VLength),
                         RVar->LocMem,
                         (RVar->LocElmLength * RVar->VLength));
    }
}

void trcreduct_Calculate(void)
{
    vartable_LevelIterator(&ReductVarTable, cntx_GetInitParallelDepth() - 1, trcreduct_CalculateVar);
}

void trcreduct_CalculateVar(VarInfo* Var)
{
    s_REDVAR* RVar = NULL;
    REDUCT_INFO* pInfo = NULL;
    char* VarBuf = NULL;
    char* LocBuf = NULL;
    char* VarPtr = NULL;
    char* LocPtr = NULL;
    char* InitBuf = NULL;
    int i, LocSize, VarSize, LocArSize, VarArSize, VLength;

    pInfo = (REDUCT_INFO *)Var->Info;

    if (pInfo != NULL && pInfo->StartReduct == 0)
    {
        RVar = pInfo->RVar;

        VarSize   = RVar->RedElmLength;
        LocSize   = RVar->LocElmLength;
        VLength   = RVar->VLength;
        VarArSize = VarSize * VLength;
        LocArSize = LocSize * VLength;
        VarPtr    = RVar->Mem;
        LocPtr    = RVar->LocMem;

        if (pInfo->Current == NULL)
        {
            /* First step. Storing */

            dynmem_malloc(pInfo->Current, char *, RVar->BlockSize);
            dvm_memcopy(pInfo->Current, VarPtr, VarArSize);
            if (LocSize > 0)
                dvm_memcopy((pInfo->Current + VarArSize), LocPtr, LocArSize);
        }
        else
        {
            /* Other steps. Calculating */

            VarBuf  = pInfo->Current;
            LocBuf  = pInfo->Current + VarArSize;
            InitBuf = pInfo->Initial;

            switch(RVar->Func)
            {
                case rf_SUM     :
                    switch(RVar->VType)
                    {
                        case rt_INT    :
                        case rt_LOGICAL :
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(int, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(int, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(long, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long long, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(long long, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(double, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(double, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_FLOAT  :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(float, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(float, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE_COMPLEX :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(double, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(double, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += sizeof(double);
                                VarBuf += sizeof(double);

                                ApplyOperator(double, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(double, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += sizeof(double);
                                VarBuf += sizeof(double);
                            }
                            break;
                        case rt_FLOAT_COMPLEX:
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(float, VarPtr, VarPtr, -, InitBuf);
                                ApplyOperator(float, VarBuf, VarPtr, +, VarBuf);
                                VarPtr += sizeof(float);
                                VarBuf += sizeof(float);
                            }
                            break;
                    }
                    break;
                case rf_MULT    :
                    switch (RVar->VType)
                    {
                        case rt_INT :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(int, VarPtr, VarPtr, /, InitBuf);
                                ApplyOperator(int, VarBuf, VarPtr, *, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long, VarPtr, VarPtr, /, InitBuf);
                                ApplyOperator(long, VarBuf, VarPtr, *, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long long, VarPtr, VarPtr, / , InitBuf);
                                ApplyOperator(long long, VarBuf, VarPtr, *, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(double, VarPtr, VarPtr, /, InitBuf);
                                ApplyOperator(double, VarBuf, VarPtr, *, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_FLOAT  :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(float, VarPtr, VarPtr, /, InitBuf);
                                ApplyOperator(float, VarBuf, VarPtr, *, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE_COMPLEX :
                            for(i = 0; i < VLength; i++)
                            {
                                double dReal1 = *((double*)InitBuf);
                                double dComplex1 = *((double*)InitBuf + 1);

                                double dReal2 = *((double*)VarPtr);
                                double dComplex2 = *((double*)VarPtr + 1);

                                double dDescriminant = dReal1 * dReal1 + dComplex1 * dComplex1;
                                if (0. == dDescriminant)
                                {
                                    *((double*)VarPtr) = dReal2;
                                    *((double*)VarPtr + 1) = dComplex2;
                                }
                                else
                                {
                                    *((double*)VarPtr) = (dReal2 * dReal1 + dComplex2 * dComplex1) / dDescriminant;
                                    *((double*)VarPtr + 1) = (dComplex2 * dReal1 - dReal2 * dComplex1) / dDescriminant;
                                }

                                dReal1 = *((double*)VarPtr);
                                dComplex1 = *((double*)VarPtr + 1);

                                dReal2 = *((double*)VarBuf);
                                dComplex2 = *((double*)VarBuf + 1);

                                *((double*)VarBuf) = dReal1 * dReal2 - dComplex1 * dComplex2;
                                *((double*)VarBuf + 1) = dReal1 * dComplex2 + dReal2 * dComplex1;

                                VarPtr += 2 * sizeof(double);
                                VarBuf += 2 * sizeof(double);
                            }
                            break;
                        case rt_FLOAT_COMPLEX:
                            for(i = 0; i < VLength; i++)
                            {
                                float fReal1 = *((float*)InitBuf);
                                float fComplex1 = *((float*)InitBuf + 1);

                                float fReal2 = *((float*)VarPtr);
                                float fComplex2 = *((float*)VarPtr + 1);

                                float fDescriminant = fReal1 * fReal1 + fComplex1 * fComplex1;
                                if (0. == fDescriminant)
                                {
                                    *((float*)VarPtr) = fReal2;
                                    *((float*)VarPtr + 1) = fComplex2;
                                }
                                else
                                {
                                    *((float*)VarPtr) = (fReal2 * fReal1 + fComplex2 * fComplex1) / fDescriminant;
                                    *((float*)VarPtr + 1) = (fComplex2 * fReal1 - fReal2 * fComplex1) / fDescriminant;
                                }

                                fReal1 = *((float*)VarPtr);
                                fComplex1 = *((float*)VarPtr + 1);

                                fReal2 = *((float*)VarBuf);
                                fComplex2 = *((float*)VarBuf + 1);

                                *((float*)VarBuf) = fReal1 * fReal2 - fComplex1 * fComplex2;
                                *((float*)VarBuf + 1) = fReal1 * fComplex2 + fReal2 * fComplex1;

                                VarPtr += 2 * sizeof(float);
                                VarBuf += 2 * sizeof(float);
                            }
                            break;
                    }
                    break;
                case rf_MAX :
                    switch (RVar->VType)
                    {
                        case rt_INT :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(int, VarBuf, dvm_max, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(long, VarBuf, dvm_max, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyFunc(long long, VarBuf, dvm_max, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(double, VarBuf, dvm_max, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_FLOAT  :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(float, VarBuf, dvm_max, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
                case rf_MIN :
                    switch (RVar->VType)
                    {
                        case rt_INT    :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(int, VarBuf, dvm_min, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(long, VarBuf, dvm_min, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyFunc(long long, VarBuf, dvm_min, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(double, VarBuf, dvm_min, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_FLOAT  :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyFunc(float,  VarBuf, dvm_min, VarPtr, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
                case rf_MINLOC  :
                    switch(RVar->VType)
                    {
                        case rt_INT :
                        case rt_LOGICAL :
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyMinLoc(int, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMinLoc(long, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyMinLoc(long long, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMinLoc(double, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_FLOAT :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMinLoc(float, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                    }
                    break;
                case rf_MAXLOC  :
                    switch (RVar->VType)
                    {
                        case rt_INT :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMaxLoc(int, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMaxLoc(long, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyMaxLoc(long long, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_DOUBLE :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMaxLoc(double, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                        case rt_FLOAT  :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyMaxLoc(float, VarBuf, VarPtr, LocBuf, LocPtr, LocSize);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                                LocPtr += LocSize;
                                LocBuf += LocSize;
                            }
                            break;
                    }
                    break;
                case rf_AND     :
                    switch(RVar->VType)
                    {
                        case rt_INT    :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(int, VarBuf, VarPtr, &, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long, VarBuf, VarPtr, &, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long long, VarBuf, VarPtr, &, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
                case rf_OR :
                    switch(RVar->VType)
                    {
                        case rt_INT    :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(int, VarBuf, VarPtr, |, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long, VarBuf, VarPtr, |, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long long, VarBuf, VarPtr, | , VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
                case rf_XOR :
                    switch(RVar->VType)
                    {
                        case rt_INT :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(int, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperator(int, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperator(long, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperator(long long, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperator(long long, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
                case rf_EQU     :
                    switch(RVar->VType)
                    {
                        case rt_INT    :
                        case rt_LOGICAL :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperatorWithInv(int, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperatorWithInv(int, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LONG   :
                            for(i = 0; i < VLength; i++)
                            {
                                ApplyOperatorWithInv(long, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperatorWithInv(long, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                        case rt_LLONG:
                            for (i = 0; i < VLength; i++)
                            {
                                ApplyOperatorWithInv(long long, VarPtr, VarPtr, ^, InitBuf);
                                ApplyOperatorWithInv(long long, VarBuf, VarPtr, ^, VarBuf);
                                VarPtr += VarSize;
                                VarBuf += VarSize;
                            }
                            break;
                    }
                    break;
            }
        }

        dvm_memcopy(RVar->Mem, pInfo->Initial, VarArSize);
        if (LocSize > 0)
            dvm_memcopy(RVar->LocMem, (pInfo->Initial + VarArSize), LocArSize);

    } /* if Info != NULL */

}

void trcreduct_Complete(void* addr, DvmType Type)
{
    VarInfo* pVar;

    pVar = vartable_FindVar(&ReductVarTable, addr);

    if (NULL != pVar)
    {
        if (pVar->Tag)
        {
            DELAY_TRACE trc;
            SYSTEM(strncpy, (trc.File, DVM_FILE[0], MaxSourceFileName));
            trc.File[MaxSourceFileName] = 0;
            trc.Line = DVM_LINE[0];
            trc.Type = Type;
            trc.Value = addr;
            table_Put(&DelayTrace, &trc);
        }
        else
            pCmpOperations->Variable(DVM_FILE[0], DVM_LINE[0], "", trc_REDUCTVAR,
                Type, addr, 1, NULL);
    }
}

void trcreduct_CopyResult(s_COLLECTION* RedVars)
{
    s_REDVAR     *RVar;
    REDUCT_INFO  *Info;
    int           i;

    for( i = 0; i < RedVars->Count; i++ )
    {
        RVar = coll_At( s_REDVAR *, RedVars, i );
        Info = trcreduct_Find( RVar->Mem );

        if (Info != NULL)
        {
            Info->StartReduct = 1;

            dvm_memcopy( RVar->Mem, Info->Current,
                         (RVar->RedElmLength * RVar->VLength) );
            if (RVar->LocElmLength > 0)
                dvm_memcopy( RVar->LocMem,
                             (Info->Current +
                             RVar->RedElmLength * RVar->VLength),
                             (RVar->LocElmLength * RVar->VLength) );
        }
    }
}

#endif  /* _TRCREDUC_C_ */
