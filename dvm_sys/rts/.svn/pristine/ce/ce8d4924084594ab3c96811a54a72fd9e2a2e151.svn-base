#ifndef _DYNLIMIT_C_
#define _DYNLIMIT_C_
/******************/

/***********************************\
* Functions for limitation checking *
\***********************************/

/********************************************************************/

void* dynmem__malloc(size_t nSize)
{
    void* pPtr = NULL;
    mac_malloc(pPtr, void*, nSize, 0);

    if (pPtr != NULL)
    {
        DynDebugAllocatedMemory += nSize;
        if (DynDebugMaxAllocatedMemory < DynDebugAllocatedMemory)
            DynDebugMaxAllocatedMemory = DynDebugAllocatedMemory;

        if (DynDebugMemoryLimit > 0 && DynDebugAllocatedMemory > DynDebugMemoryLimit)
        {
            if (EnableTrace) {
                cmptrace_Write(TRACE_DYN_MEMORY_LIMIT);
            }

            EnableDynControl = 0;
            EnableTrace = 0;
            DynDebugMemoryLimit = 0;
            DynDebugExecutionTimeLimit = 0;

            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** Dynamic debugger is disabled: Out of memory limits ***\n");
        }
    }

    return pPtr;
}

void* dynmem__calloc(size_t nSize, size_t nElements)
{
    void* pPtr = NULL;
    mac_calloc(pPtr, void*, nElements, nSize, 0);

    if (pPtr != NULL)
    {
        DynDebugAllocatedMemory += nSize * nElements;
        if (DynDebugMaxAllocatedMemory < DynDebugAllocatedMemory)
            DynDebugMaxAllocatedMemory = DynDebugAllocatedMemory;

        if (DynDebugMemoryLimit > 0 && DynDebugAllocatedMemory > DynDebugMemoryLimit)
        {
            if (EnableTrace) {
                cmptrace_Write(TRACE_DYN_MEMORY_LIMIT);
            }

            EnableDynControl = 0;
            EnableTrace = 0;
            DynDebugMemoryLimit = 0;
            DynDebugExecutionTimeLimit = 0;

            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** Dynamic debugger is disabled: Out of memory limits ***\n");
        }
    }

    return pPtr;
}

void dynmem__free(void** ppPtr, size_t nSize)
{
    mac_free(ppPtr);

    if (DynDebugAllocatedMemory < nSize)
    {
        EnableDynControl = 0;
        EnableTrace = 0;
        DynDebugMemoryLimit = 0;
        DynDebugExecutionTimeLimit = 0;
        epprintf(MultiProcErrReg2,__FILE__,__LINE__,
        "*** RTS err: DYNCONTROL: Error during checking memory limitation\n");
    }

    DynDebugAllocatedMemory -= nSize;
}

void dynmem__printstatistics(void)
{
    if (DynDebugPrintStatistics && MPS_CurrentProc == DVM_IOProc &&
        DbgInfoPrint && _SysInfoPrint)
    {
        pprintf(0, "*** Dynamic debugger memory usage statistics ***\n");
        pprintf(0, "Maximum used memory: %lu byte(s)\n", DynDebugMaxAllocatedMemory);
    }
}

void dyntime__check(void)
{
    if (DynDebugExecutionTimeLimit > 0)
    {
        if ((UDvmType)(dvm_time() - SystemStartTime) > DynDebugExecutionTimeLimit)
        {
            EnableDynControl = 0;
            EnableTrace = 0;
            DynDebugMemoryLimit = 0;
            DynDebugExecutionTimeLimit = 0;

            epprintf(MultiProcErrReg2,__FILE__,__LINE__,
            "*** Dynamic debugger is disabled: Out of execution time limits ***\n");
        }
    }
}

#endif  /* _DYNLIMIT_C_ */
