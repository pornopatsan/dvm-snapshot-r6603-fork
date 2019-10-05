#include "loop.h"

#include <cassert>

#include "include/dvmhlib_debug.h"

#include "cuda_reduction.h"
#include "distrib.h"
#include "dvmh_pieces.h"
#include "dvmh_stat.h"
#include "util.h"

namespace libdvmh {

// DvmhReduction

DvmhReduction::DvmhReduction(RedFunction aRedFunc, UDvmType VLength, DvmhData::DataType VType, void *Mem, UDvmType LocElmLength, void *LocMem) {
    redFunc = aRedFunc;
    elemCount = VLength;
    arrayElementType = VType;
    elemSize = DvmhData::getTypeSize(VType);
    arrayAddr = Mem;
    initArrVal = new char[elemSize * elemCount];
    memcpy(initArrVal, arrayAddr, elemSize * elemCount);
    if (LocMem) {
        locSize = LocElmLength;
        locAddr = LocMem;
        initLocVal = new char[locSize * elemCount];
        memcpy(initLocVal, locAddr, locSize * elemCount);
    } else {
        locSize = 0;
        locAddr = 0;
        initLocVal = 0;
    }
}

void DvmhReduction::initValues(void *arrayPtr, void *locPtr) const {
    assert(initArrVal);
    if (arrayPtr) {
        if (redFunc == rfSum || redFunc == rfProd || redFunc == rfXor || redFunc == rfEqu || redFunc == rfNe || redFunc == rfEq) {
            if (redFunc == rfProd) {
                for (UDvmType j = 0; j < elemCount; j++) {
                    char *ptr = (char *)arrayPtr + j * elemSize;
                    switch (arrayElementType) {
                        case DvmhData::dtChar: *(char *)ptr = 1; break;
                        case DvmhData::dtInt: *(int *)ptr = 1; break;
                        case DvmhData::dtLong: *(long *)ptr = 1; break;
                        case DvmhData::dtLongLong: *(long long *)ptr = 1; break;
                        case DvmhData::dtFloat: *(float *)ptr = 1.0f; break;
                        case DvmhData::dtDouble: *(double *)ptr = 1.0; break;
                        case DvmhData::dtFloatComplex: *(float *)ptr = 1.0f; *((float *)ptr + 1) = 0.0f; break;
                        case DvmhData::dtDoubleComplex: *(double *)ptr = 1.0; *((double *)ptr + 1) = 0.0; break;
                        default: assert(false);
                    }
                }
            } else if (redFunc == rfEqu || redFunc == rfEq) {
                for (UDvmType j = 0; j < elemCount; j++) {
                    char *ptr = (char *)arrayPtr + j * elemSize;
                    switch (arrayElementType) {
                        case DvmhData::dtChar: *(signed char *)ptr = -1; break;
                        case DvmhData::dtInt: *(int *)ptr = -1; break;
                        case DvmhData::dtLong: *(long *)ptr = -1; break;
                        case DvmhData::dtLongLong: *(long long *)ptr = -1; break;
                        default: assert(false);
                    }
                }
            } else {
                for (UDvmType j = 0; j < elemCount; j++) {
                    char *ptr = (char *)arrayPtr + j * elemSize;
                    switch (arrayElementType) {
                        case DvmhData::dtChar: *(char *)ptr = 0; break;
                        case DvmhData::dtInt: *(int *)ptr = 0; break;
                        case DvmhData::dtLong: *(long *)ptr = 0; break;
                        case DvmhData::dtLongLong: *(long long *)ptr = 0; break;
                        case DvmhData::dtFloat: *(float *)ptr = 0.0f; break;
                        case DvmhData::dtDouble: *(double *)ptr = 0.0; break;
                        case DvmhData::dtFloatComplex: *(float *)ptr = 0.0f; *((float *)ptr + 1) = 0.0f; break;
                        case DvmhData::dtDoubleComplex: *(double *)ptr = 0.0; *((double *)ptr + 1) = 0.0; break;
                        default: assert(false);
                    }
                }
            }
        } else {
            memcpy(arrayPtr, initArrVal, elemSize * elemCount);
        }
    }

    if (isLoc() && locPtr) {
        assert(initLocVal);
        memcpy(locPtr, initLocVal, locSize * elemCount);
    }
}

void DvmhReduction::initGlobalValues() {
    initValues(arrayAddr, locAddr);
}

void DvmhReduction::postValues(const void *arrayPtr, const void *locPtr) {
    assert(isLoc() <= (locPtr != 0));
    assert(arrayAddr);
    SpinLockGuard guard(lock);
    performOperation(arrayAddr, locAddr, arrayPtr, locPtr);
}

void DvmhReduction::addInitialValues() {
    postValues(initArrVal, initLocVal);
}

#define RED_MAX(T, dest, src, result) do { if (*((T *)dest) < *((T *)src)) { *((T *)dest) = *((T *)src); result = 1; } else result = 0; } while(0)
#define RED_MIN(T, dest, src, result) do { if (*((T *)dest) > *((T *)src)) { *((T *)dest) = *((T *)src); result = 1; } else result = 0; } while(0)
#define RED_AND(T, dest, src, result) do { *((T *)dest) &= *((T *)src); result = 0; } while(0)
#define RED_OR(T, dest, src, result) do { *((T *)dest) |= *((T *)src); result = 0; } while(0)
#define RED_XOR(T, dest, src, result) do { *((T *)dest) ^= *((T *)src); result = 0; } while(0)
#define RED_EQU(T, dest, src, result) do { *((T *)dest) = ~(*((T *)dest) ^ *((T *)src)); result = 0; } while(0)
#define RED_NE(T, dest, src, result) do { *((T *)dest) ^= *((T *)src); result = 0; } while(0)
#define RED_EQ(T, dest, src, result) do { *((T *)dest) = ~(*((T *)dest) ^ *((T *)src)); result = 0; } while(0)
#define RED_SUM(T, dest, src, result) do { *((T *)dest) += *((T *)src); result = 0; } while(0)
#define RED_MULT(T, dest, src, result) do { *((T *)dest) *= *((T *)src); result = 0; } while(0)
void DvmhReduction::performOperation(void *resultArrayPtr, void *resultLocPtr, const void *summandArrayPtr, const void *summandLocPtr) const {
    RedFunction fn = redFunc;
    for (UDvmType i = 0; i < elemCount; i++) {
        int result = -1;
        void *dest = (char *)resultArrayPtr + elemSize * i;
        void *src = (char *)summandArrayPtr + elemSize * i;
        switch (arrayElementType) {
            case DvmhData::dtChar:
                switch (fn) {
                    case rfSum: RED_SUM(char, dest, src, result); break;
                    case rfProd: RED_MULT(char, dest, src, result); break;
                    case rfMax: RED_MAX(char, dest, src, result); break;
                    case rfMin: RED_MIN(char, dest, src, result); break;
                    case rfAnd: RED_AND(char, dest, src, result); break;
                    case rfOr: RED_OR(char, dest, src, result); break;
                    case rfXor: RED_XOR(char, dest, src, result); break;
                    case rfEqu: RED_EQU(char, dest, src, result); break;
                    case rfNe: RED_NE(char, dest, src, result); break;
                    case rfEq: RED_EQ(char, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtInt:
                switch (fn) {
                    case rfSum: RED_SUM(int, dest, src, result); break;
                    case rfProd: RED_MULT(int, dest, src, result); break;
                    case rfMax: RED_MAX(int, dest, src, result); break;
                    case rfMin: RED_MIN(int, dest, src, result); break;
                    case rfAnd: RED_AND(int, dest, src, result); break;
                    case rfOr: RED_OR(int, dest, src, result); break;
                    case rfXor: RED_XOR(int, dest, src, result); break;
                    case rfEqu: RED_EQU(int, dest, src, result); break;
                    case rfNe: RED_NE(int, dest, src, result); break;
                    case rfEq: RED_EQ(int, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtLong:
                switch (fn) {
                    case rfSum: RED_SUM(long, dest, src, result); break;
                    case rfProd: RED_MULT(long, dest, src, result); break;
                    case rfMax: RED_MAX(long, dest, src, result); break;
                    case rfMin: RED_MIN(long, dest, src, result); break;
                    case rfAnd: RED_AND(long, dest, src, result); break;
                    case rfOr: RED_OR(long, dest, src, result); break;
                    case rfXor: RED_XOR(long, dest, src, result); break;
                    case rfEqu: RED_EQU(long, dest, src, result); break;
                    case rfNe: RED_NE(long, dest, src, result); break;
                    case rfEq: RED_EQ(long, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtLongLong:
                switch (fn) {
                    case rfSum: RED_SUM(long long, dest, src, result); break;
                    case rfProd: RED_MULT(long long, dest, src, result); break;
                    case rfMax: RED_MAX(long long, dest, src, result); break;
                    case rfMin: RED_MIN(long long, dest, src, result); break;
                    case rfAnd: RED_AND(long long, dest, src, result); break;
                    case rfOr: RED_OR(long long, dest, src, result); break;
                    case rfXor: RED_XOR(long long, dest, src, result); break;
                    case rfEqu: RED_EQU(long long, dest, src, result); break;
                    case rfNe: RED_NE(long long, dest, src, result); break;
                    case rfEq: RED_EQ(long long, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtFloat:
                switch (fn) {
                    case rfSum: RED_SUM(float, dest, src, result); break;
                    case rfProd: RED_MULT(float, dest, src, result); break;
                    case rfMax: RED_MAX(float, dest, src, result); break;
                    case rfMin: RED_MIN(float, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtDouble:
                switch (fn) {
                    case rfSum: RED_SUM(double, dest, src, result); break;
                    case rfProd: RED_MULT(double, dest, src, result); break;
                    case rfMax: RED_MAX(double, dest, src, result); break;
                    case rfMin: RED_MIN(double, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtFloatComplex:
                switch (fn) {
                    case rfSum: RED_SUM(float_complex, dest, src, result); break;
                    case rfProd: RED_MULT(float_complex, dest, src, result); break;
                    default: assert(false);
                }
                break;
            case DvmhData::dtDoubleComplex:
                switch (fn) {
                    case rfSum: RED_SUM(double_complex, dest, src, result); break;
                    case rfProd: RED_MULT(double_complex, dest, src, result); break;
                    default: assert(false);
                }
                break;
            default: assert(false);
        }
        checkInternal3(result != -1, "No suitable reduction found for function %d and type %d", (int)redFunc, (int)arrayElementType);

        if (isLoc() && resultLocPtr != NULL && result == 1)
            memcpy((char *)resultLocPtr + locSize * i, (char *)summandLocPtr + locSize * i, locSize);
    }
}
#undef RED_SUM
#undef RED_MULT
#undef RED_MAX
#undef RED_MIN
#undef RED_AND
#undef RED_OR
#undef RED_XOR
#undef RED_EQU
#undef RED_NE
#undef RED_EQ

DvmhReduction::~DvmhReduction() {
    assert(initArrVal);
    delete[] initArrVal;
    delete[] initLocVal;
}

// DvmhReductionCuda

DvmhReductionCuda::DvmhReductionCuda(DvmhReduction *aReduction, void **aGpuMemPtr, void **aGpuLocMemPtr, int aDeviceNum) {
    reduction = aReduction;
    itemCount = 0;
    gpuMem = 0;
    gpuLocMem = 0;
    gpuMemPtr = (char **)aGpuMemPtr;
    gpuLocMemPtr = (char **)aGpuLocMemPtr;
    localArray = new char[reduction->elemSize * reduction->elemCount];
    if (reduction->locAddr)
        localLocArray = new char[reduction->locSize * reduction->elemCount];
    else
        localLocArray = 0;
    reduction->initValues(localArray, localLocArray);
    deviceNum = aDeviceNum;
}

void DvmhReductionCuda::prepare(UDvmType items, bool fillFlag) {
    DvmhReduction *red = reduction;
    DvmType elemCount = red->elemCount;
    assert(items > 0);
    itemCount = items;
    CudaDevice *device = (CudaDevice *)devices[deviceNum];
    assert(!gpuMem);
    gpuMem = device->allocBytes(elemCount * red->elemSize * items);
    if (gpuMemPtr)
        *gpuMemPtr = gpuMem;
    assert(!gpuLocMem);
    if (red->locAddr)
        gpuLocMem = device->allocBytes(elemCount * red->locSize * items);
    if (gpuLocMemPtr)
        *gpuLocMemPtr = gpuLocMem;
    if (fillFlag) {
        assert(localArray);
        dvmhCudaReplicate(device, localArray, elemCount * red->elemSize, items, gpuMem);
        if (red->locAddr) {
            assert(localLocArray);
            dvmhCudaReplicate(device, localLocArray, elemCount * red->locSize, items, gpuLocMem);
        }
    }
}

#define CALL_REDFUNC(func, type) { dvmhCudaRed_##func##_##type(cudaDev, itemCount, (type *)gpuMem, elemCount, gpuLocMem, \
        locElementSize, *(type *)localArray, (type *)localArray, localLocArray); catched = true; }
void DvmhReductionCuda::finish() {
    CudaDevice *cudaDev = (CudaDevice *)devices[deviceNum];
    UDvmType elemCount = reduction->elemCount;
    assert(localArray);
    assert((localLocArray != 0) == reduction->isLoc());
    DvmhReduction::RedFunction fn = reduction->redFunc;
    UDvmType locElementSize = reduction->locSize;

    bool catched = false;
    switch(reduction->arrayElementType) {
        case DvmhData::dtChar:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, char); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, char); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, char); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, char); break;
                case DvmhReduction::rfAnd: CALL_REDFUNC(and, char); break;
                case DvmhReduction::rfOr: CALL_REDFUNC(or, char); break;
                case DvmhReduction::rfXor: CALL_REDFUNC(neq, char); break; // xor == neq
                case DvmhReduction::rfEqu: CALL_REDFUNC(eq, char); break; // equ == eq
                case DvmhReduction::rfNe: CALL_REDFUNC(neq, char); break; // ne == neq
                case DvmhReduction::rfEq: CALL_REDFUNC(eq, char); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtInt:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, int); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, int); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, int); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, int); break;
                case DvmhReduction::rfAnd: CALL_REDFUNC(and, int); break;
                case DvmhReduction::rfOr: CALL_REDFUNC(or, int); break;
                case DvmhReduction::rfXor: CALL_REDFUNC(neq, int); break; // xor == neq
                case DvmhReduction::rfEqu: CALL_REDFUNC(eq, int); break; // equ == eq
                case DvmhReduction::rfNe: CALL_REDFUNC(neq, int); break; // ne == neq
                case DvmhReduction::rfEq: CALL_REDFUNC(eq, int); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtLong:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, long); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, long); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, long); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, long); break;
                case DvmhReduction::rfAnd: CALL_REDFUNC(and, long); break;
                case DvmhReduction::rfOr: CALL_REDFUNC(or, long); break;
                case DvmhReduction::rfXor: CALL_REDFUNC(neq, long); break; // xor == equ
                case DvmhReduction::rfEqu: CALL_REDFUNC(eq, long); break;  // equ == eq
                case DvmhReduction::rfNe: CALL_REDFUNC(neq, long); break; // ne == neq
                case DvmhReduction::rfEq: CALL_REDFUNC(eq, long); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtLongLong:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, long_long); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, long_long); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, long_long); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, long_long); break;
                case DvmhReduction::rfAnd: CALL_REDFUNC(and, long_long); break;
                case DvmhReduction::rfOr: CALL_REDFUNC(or, long_long); break;
                case DvmhReduction::rfXor: CALL_REDFUNC(neq, long_long); break; // xor == neq
                case DvmhReduction::rfEqu: CALL_REDFUNC(eq, long_long); break;  // equ == eq
                case DvmhReduction::rfNe: CALL_REDFUNC(neq, long_long); break; // eq == neq
                case DvmhReduction::rfEq: CALL_REDFUNC(eq, long_long); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtFloat:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, float); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, float); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, float); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, float); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtDouble:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, double); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, double); break;
                case DvmhReduction::rfMax: CALL_REDFUNC(max, double); break;
                case DvmhReduction::rfMin: CALL_REDFUNC(min, double); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtFloatComplex:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, float_complex); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, float_complex); break;
                default: assert(false);
            }
            break;
        case DvmhData::dtDoubleComplex:
            switch (fn) {
                case DvmhReduction::rfSum: CALL_REDFUNC(sum, double_complex); break;
                case DvmhReduction::rfProd: CALL_REDFUNC(prod, double_complex); break;
                default: assert(false);
            }
            break;
        default: assert(false);
    }
    checkInternal3(catched, "Reduction by CUDA for this combination (%d, %d) of type and function is not implemented", (int)reduction->arrayElementType,
                (int)reduction->redFunc);
    reduction->postValues(localArray, localLocArray);
}
#undef CALL_REDFUNC

void DvmhReductionCuda::advancePtrsBy(UDvmType itemsDone) {
    *gpuMemPtr += itemsDone * reduction->elemSize;
    if (gpuLocMemPtr)
        *gpuLocMemPtr += itemsDone * reduction->locSize;
}

DvmhReductionCuda::~DvmhReductionCuda() {
    if (gpuMem)
        devices[deviceNum]->dispose(gpuMem);
    if (gpuLocMem)
        devices[deviceNum]->dispose(gpuLocMem);
    delete[] localArray;
    delete[] localLocArray;
}

// DvmhLoopHandler

DvmhLoopHandler::DvmhLoopHandler(bool parallel, bool master, DvmHandlerFunc func, int paramCount, int bases) {
    parallelFlag = parallel;
    masterFlag = master;
    f = func;
    paramsCount = paramCount >= 0 ? paramCount : 0;
    if (paramsCount > 0)
        params = new void *[paramsCount];
    else
        params = 0;
    basesCount = bases >= 0 ? bases : 0;
    for (int i = 0; i < paramsCount; i++)
        params[i] = 0;
}

void DvmhLoopHandler::exec(void *curLoop, void *baseVal) {
    int realParamsCount = paramsCount + basesCount + 1;
#ifdef NON_CONST_AUTOS
    void *realParams[realParamsCount];
#else
    void *realParams[MAX_PARAM_COUNT];
#endif
    realParams[0] = curLoop;
    for (int i = 0; i < basesCount; i++)
        realParams[1 + i] = baseVal;
    typedMemcpy(realParams + realParamsCount - paramsCount, params, paramsCount);
    executeFunction(f, realParams, realParamsCount);
}

// DvmhLoopPortion

DvmhLoopPortion::DvmhLoopPortion(DvmhLoop *aLoop, LoopBounds myBounds[], bool doRed, int devNum, int handlNum, HandlerOptimizationParams *optParams,
        int slotCount) {
    assert(aLoop);
    loop = aLoop;
    if (loop->rank > 0) {
        loopBounds = new LoopBounds[loop->rank];
        typedMemcpy(loopBounds, myBounds, loop->rank);
    } else {
        loopBounds = 0;
    }
    assert(devNum >= 0 && devNum < devicesCount);
    deviceNum = devNum;
    assert(handlNum >= 0 && handlNum < (int)loop->handlers[devices[deviceNum]->getType()].size());
    handlerNum = handlNum;
    handler = loop->handlers[devices[deviceNum]->getType()][handlerNum];
    optimizationParams = optParams;
    if (slotCount > 0) {
        slotsToUse = slotCount;
    } else if (optimizationParams) {
        slotsToUse = optimizationParams->getSlotsToUse();
    } else {
        slotsToUse = 1;
    }
    assert(slotsToUse > 0);
    assert((devices[deviceNum]->hasSlots() && slotsToUse <= devices[deviceNum]->getSlotCount()) || (!devices[deviceNum]->hasSlots() && slotsToUse == 1));
    targetSlot = -1;
    doReduction = doRed;
    loopRef = 0;
    calculationTime = 0;
}

void DvmhLoopPortion::perform(DvmhPerformer *performer) {
    if (devices[deviceNum]->getType() == dtCuda) {
        loopRef = new DvmhLoopCuda(this, performer);
    } else if (devices[deviceNum]->getType() == dtHost) {
        loopRef = new DvmhSpecLoop(this);
    }
    loopRef->prepareHandlerExec();
    DvmhTimer tm(needToCollectTimes);
    DvmType dvmLoopRef = (DvmType)loopRef;
    handler->exec(&dvmLoopRef);
    loopRef->syncAfterHandlerExec();
    double timeNow = 0;
    if (needToCollectTimes)
        timeNow = tm.total();
    delete loopRef;
    if (needToCollectTimes && calculationTime <= 0.0)
        calculationTime = timeNow;
}

void DvmhLoopPortion::updatePerformanceInfo() const {
    if (calculationTime > 0.0) {
        UDvmType portionElements = 1;
        UDvmType loopElements = 1;
        for (int i = 0; i < loop->rank; i++) {
            portionElements *= loopBounds[i].iterCount();
            loopElements *= loop->localPlusShadow[i].iterCount();
        }
        double coeff = loopElements / portionElements;
        double computeAmount = loopElements;
        double loopTime = calculationTime * coeff;
        double curPerformance = computeAmount / loopTime;
        // TODO: Need to unite info from portions from one device, i. e. properly calculate total thread count, other issues with multi-slot devices. Not sure.
        // TODO: Take into account weights, computeAmount can be not proportional to element count
        loop->persistentInfo->getLoopHandlerPerformanceInfo(deviceNum, handlerNum)->update(computeAmount, curPerformance, optimizationParams);
        if (devices[deviceNum]->getType() == dtCuda)
            dvmh_stat_add_measurement(((CudaDevice *)devices[deviceNum])->index, DVMH_STAT_METRIC_LOOP_PORTION_TIME, calculationTime, calculationTime, 0.0);
    }
}

DvmhLoopPortion::~DvmhLoopPortion() {
    delete[] loopBounds;
    delete optimizationParams;
}

// DvmhLoop

DvmhLoop::DvmhLoop(int aRank, const LoopBounds aLoopBounds[]) {
    region = 0;
    rank = aRank;
    alignRule = 0;
    assert(rank >= 0);
    dependencyMask = 0;
    mappingData = 0;
    mappingDataRules = 0;
    shadowComputeWidths = 0;
    localPlusShadow = 0;
    persistentInfo = 0;
    userCudaBlockFlag = false;
    cudaBlock[0] = 1;
    cudaBlock[1] = 1;
    cudaBlock[2] = 1;
    stage = 0;
    debugFlag = false;
    if (rank > 0) {
        // Parallel loop
        loopBounds = new LoopBounds[rank];
        bool emptyFlag = false;
        for (int i = 0; i < rank; i++) {
            loopBounds[i] = aLoopBounds[i];
            checkError2(loopBounds[i].isCorrect(), "Loop bounds are incorrect");
            DvmType dist = divDownS(loopBounds[i][1] - loopBounds[i][0], loopBounds[i][2]);
            DvmType newEnd = loopBounds[i][0] + dist * loopBounds[i][2];
            if (loopBounds[i][1] != newEnd) {
                dvmh_log(DEBUG, "Adjusting end index. Was " DTFMT ", now " DTFMT " (start=" DTFMT ", step=" DTFMT ")", loopBounds[i][1], newEnd,
                        loopBounds[i][0], loopBounds[i][2]);
                loopBounds[i][1] = newEnd;
            }
            if (dist < 0)
                emptyFlag = true;
        }
        hasLocal = !emptyFlag;
        if (hasLocal) {
            localPart = new LoopBounds[rank];
            typedMemcpy(localPart, loopBounds, rank);
        } else {
            localPart = 0;
        }
    } else {
        // Sequential interval
        hasLocal = true;
        loopBounds = 0;
        localPart = 0;
    }
    phase = lpRegistrations;
}

void DvmhLoop::setAlignRule(DvmhAlignRule *aAlignRule) {
    checkInternal(phase == lpRegistrations);
    assert(!alignRule);
    if (aAlignRule) {
        assert(rank > 0);
        alignRule = aAlignRule;
        if (hasLocal) {
#ifdef NON_CONST_AUTOS
            Interval localIters[rank];
            DvmType absSteps[rank];
#else
            Interval localIters[MAX_LOOP_RANK];
            DvmType absSteps[MAX_LOOP_RANK];
#endif
            for (int i = 0; i < rank; i++) {
                localIters[i] = loopBounds[i].toInterval();
                absSteps[i] = std::abs(loopBounds[i][2]);
            }
            hasLocal = alignRule->getDspace()->hasLocal();
            if (hasLocal)
                hasLocal = alignRule->mapOnPart(alignRule->getDspace()->getLocalPart(), localIters, false, absSteps);
            if (hasLocal) {
                for (int i = 0; i < rank; i++)
                    localPart[i] = LoopBounds::create(localIters[i], loopBounds[i][2]);
            } else {
                delete[] localPart;
                localPart = 0;
            }
        }
    }
}

void DvmhLoop::setCudaBlock(int xSize, int ySize, int zSize) {
    checkInternal(phase == lpRegistrations);
    cudaBlock[0] = xSize;
    cudaBlock[1] = ySize;
    cudaBlock[2] = zSize;
    int blockRank = 3;
    while (blockRank > 0 && cudaBlock[blockRank - 1] == 1)
        blockRank--;
    checkError2(blockRank <= rank, "Setting CUDA block with more dimensions than the parallel loop has is prohibited");
    userCudaBlockFlag = true;
    dvmh_log(TRACE, "Overriden CUDA block: (%d, %d, %d)", cudaBlock[0], cudaBlock[1], cudaBlock[2]);
}

void DvmhLoop::addToAcross(DvmhShadow oldGroup, DvmhShadow newGroup) {
    checkInternal(phase == lpRegistrations);
    acrossOld.append(oldGroup);
    acrossNew.append(newGroup);
    dvmh_log(TRACE, "Count of 'New' ACROSS shadows=%d", acrossNew.dataCount());
    dvmh_log(TRACE, "Count of all ACROSS shadows=%d", acrossNew.dataCount() + acrossOld.dataCount());
}

void DvmhLoop::addToAcross(DvmhData *data, ShdWidth widths[], bool cornerFlag) {
    checkInternal(phase == lpRegistrations);
    DvmhShadowData oldSdata, newSdata;
    oldSdata.data = data;
    newSdata.data = data;
    oldSdata.cornerFlag = cornerFlag;
    newSdata.cornerFlag = cornerFlag;
    oldSdata.shdWidths = new ShdWidth[data->getRank()];
    std::fill(oldSdata.shdWidths, oldSdata.shdWidths + data->getRank(), ShdWidth::createEmpty());
    newSdata.shdWidths = new ShdWidth[data->getRank()];
    typedMemcpy(newSdata.shdWidths, widths, data->getRank());
    // Extract old shadows
    if (data->isDistributed() && alignRule && data->getAlignRule()->getDspace() == alignRule->getDspace()) {
        for (int i = 1; i <= alignRule->getDspace()->getRank(); i++) {
            int ax = data->getAlignRule()->getAxisRule(i)->axisNumber;
            if (ax > 0 && alignRule->getAxisRule(i)->axisNumber > 0) {
                int j = sign(alignRule->getAxisRule(i)->multiplier) * sign(data->getAlignRule()->getAxisRule(i)->multiplier)
                        * sign(loopBounds[alignRule->getAxisRule(i)->axisNumber - 1][2]) > 0;
                oldSdata.shdWidths[ax - 1][j] = newSdata.shdWidths[ax - 1][j];
                newSdata.shdWidths[ax - 1][j] = 0;
            }
        }
    }
    if (!oldSdata.empty())
        acrossOld.add(oldSdata);
    if (!newSdata.empty())
        acrossNew.add(newSdata);
}

void DvmhLoop::addConsistent(DvmhData *data, DvmhAxisAlignRule axisRules[]) {
    DvmhAxisAlignRule *rules = new DvmhAxisAlignRule[data->getRank()];
    typedMemcpy(rules, axisRules, data->getRank());
    consistents.push_back(std::make_pair(data, rules));
}

void DvmhLoop::addRemoteAccess(DvmhData *data, DvmhAxisAlignRule axisRules[]) {
    DvmhAxisAlignRule *rules = new DvmhAxisAlignRule[data->getRank()];
    typedMemcpy(rules, axisRules, data->getRank());
    rmas.push_back(std::make_pair(data, rules));
}

void DvmhLoop::prepareExecution() {
    checkInternal(phase == lpRegistrations);
    phase = lpExecution;
    assert(!localPlusShadow);
    if (localPart) {
        localPlusShadow = new LoopBounds[rank];
        if (alignRule) {
            DvmhDistribSpace *dspace = alignRule->getDspace();
#ifdef NON_CONST_AUTOS
            Interval computePart[dspace->getRank()], resInter[rank];
            DvmType absSteps[rank];
#else
            Interval computePart[MAX_DISTRIB_SPACE_RANK], resInter[MAX_LOOP_RANK];
            DvmType absSteps[MAX_LOOP_RANK];
#endif
            checkInternal2(!dspace->getDistribRule()->hasIndirect() || !shadowComputeWidths, "Shadow compute is unsupported");
            for (int i = 0; i < dspace->getRank(); i++) {
                computePart[i][0] = dspace->getAxisLocalPart(i + 1)[0] - (shadowComputeWidths ? shadowComputeWidths[i][0] : 0);
                computePart[i][1] = dspace->getAxisLocalPart(i + 1)[1] + (shadowComputeWidths ? shadowComputeWidths[i][1] : 0);
            }
            for (int i = 0; i < rank; i++) {
                resInter[i] = loopBounds[i].toInterval();
                absSteps[i] = std::abs(loopBounds[i][2]);
            }
            alignRule->mapOnPart(computePart, resInter, false, absSteps);
            for (int i = 0; i < rank; i++)
                localPlusShadow[i] = LoopBounds::create(resInter[i], loopBounds[i][2]);
        } else {
            typedMemcpy(localPlusShadow, localPart, rank);
        }
    }
    if (rank > 0) {
        renewDependencyMask();
        dvmh_log(TRACE, "Modified dependency mask: " UDTFMT, dependencyMask);
    }
    persistentInfo->fix(this);
    handleShadowCompute(COMPUTE_PREPARE);
    if (region)
        region->renewDatas();
}

class PerformPortionTask: public Executable {
public:
    explicit PerformPortionTask(DvmhLoopPortion *aPortion): portion(aPortion) {
        resNeeded.setResource(ResourcesSpec::rtSlotCount, portion->getSlotsToUse());
    }
public:
    virtual void execute() {
        execute(0);
    }
    virtual void execute(void *arg) {
        DvmhPerformer *performer = (DvmhPerformer *)arg;
        portion->perform(performer);
        portion->updatePerformanceInfo();
        delete portion;
    }
protected:
    DvmhLoopPortion *portion;
};

class AcrossRenewTask: public Executable {
public:
    explicit AcrossRenewTask(const DvmhLoop *aLoop, int devNum, const Interval aDspacePart[]): loop(aLoop), dev(devNum) {
        assert(aDspacePart);
        int rank = loop->alignRule->getDspace()->getRank();
        dspacePart = new Interval[rank];
        dspacePart->blockAssign(rank, aDspacePart);
    }
public:
    virtual void execute() {
        for (int i = 0; i < loop->acrossNew.dataCount(); i++) {
            const DvmhShadowData &sdata = loop->acrossNew.getData(i);
            DvmhData *data = sdata.data;
            int dataRank = data->getRank();
#ifdef NON_CONST_AUTOS
            Interval computePart[dataRank];
#else
            Interval computePart[MAX_ARRAY_RANK];
#endif
            computePart->blockAssign(dataRank, data->getSpace());
            data->getAlignRule()->mapOnPart(dspacePart, computePart, false);
        }
        // TODO: Implement
        //sdata.data->getActualShadow(dev, (localPart ? localPart : sdata.data->getLocalPart()), sdata.cornerFlag, sdata.shdWidths, true);
    }
    virtual void execute(void *) { execute(); }
public:
    virtual ~AcrossRenewTask() {
        delete[] dspacePart;
    }
protected:
    const DvmhLoop *loop;
    Interval *dspacePart;
    int dev;
};

void DvmhLoop::executePart(const LoopBounds part[]) {
    checkInternal(phase == lpExecution);
    AggregateEvent *endEvent = new AggregateEvent;
    executePartAsync(part, 0, endEvent);
    endEvent->wait();
    delete endEvent;
}

AggregateEvent *DvmhLoop::executePartAsync(const LoopBounds part[], DvmhEvent *depSrc, AggregateEvent *endEvent) {
    checkInternal(phase == lpExecution);
    HybridVector<DvmhLoopPortion *, 36> portions;
    DvmhPieces *assignedPart = new DvmhPieces(rank);
    for (int i = 0; i < devicesCount; i++) {
        if ((region && region->usesDevice(i)) || (!region && i == 0)) {
            CommonDevice *device = devices[i];
            DeviceType devType = device->getType();
#ifdef NON_CONST_AUTOS
            LoopBounds partialBounds[rank];
            Interval portionRect[rank];
#else
            LoopBounds partialBounds[MAX_LOOP_RANK];
            Interval portionRect[MAX_LOOP_RANK];
#endif
            if (mapPartOnDevice(part, i, partialBounds)) {
// TODO: Implement ACROSS scheme across devices
                for (int j = 0; j < rank; j++) {
                    if (dependencyMask & ((UDvmType)1 << (rank - (j + 1))))
                        checkError2(partialBounds[j].toInterval().contains(part[j].toInterval()),
                                "ACROSS scheme is not implemented yet for multidevice execution inside one process. Consider using 'targets' clause.");
                }
                // Determine doReduction
                partialBounds->toBlock(rank, portionRect);
                DvmhPieces *p = new DvmhPieces(rank);
                p->appendOne(portionRect);
                DvmhPieces *p2 = assignedPart->intersect(p);
                bool doReduction = p2->isEmpty();
                assignedPart->unite(p);
                delete p2;
                delete p;
// TODO: several parallel handlers on one device simultaneously
                double iterCount = partialBounds->iterCount(rank);
                std::pair<int, HandlerOptimizationParams *> choice = persistentInfo->pickOptimizationParams(i, iterCount);
                int handlerNum = choice.first;
                HandlerOptimizationParams *params = choice.second;
                DvmhLoopHandler *handler = handlers[devType][handlerNum];
                if (handler->isParallel()) {
                    // One portion on device
                    DvmhLoopPortion *portion = new DvmhLoopPortion(this, partialBounds, doReduction, i, handlerNum, params);
                    portions.push_back(portion);
                } else {
                    int accumCount = 0;
                    for (int j = 0; j < params->getSlotsToUse(); j++) {
#ifdef NON_CONST_AUTOS
                        LoopBounds curBounds[rank];
#else
                        LoopBounds curBounds[MAX_LOOP_RANK];
#endif
                        if (!dividePortion(partialBounds, params->getSlotsToUse(), j, curBounds))
                            break;
                        DvmhLoopPortion *portion = new DvmhLoopPortion(this, curBounds, doReduction, i, handlerNum, (j == 0 ? params : params->clone()), 1);
                        if (devType == dtHost)
                            portion->setTargetSlot(j);
                        portions.push_back(portion);
                        accumCount++;
                    }
                    if (accumCount > 1)
                        persistentInfo->getLoopHandlerPerformanceInfo(i, handlerNum)->setAccumulation(accumCount);
// TODO: cut proper portion (WTF???)
                }
            }
        }
    }
    delete assignedPart;
    // Generate executable tasks and commit them
    HybridVector<Executable *, 10> masterTasks;
    for (int i = 0; i < (int)portions.size(); i++) {
        DependentTask *task = new DependentTask;
        if (depSrc)
            depSrc->addDependent(task);
        if (endEvent)
            endEvent->addTaskEndEvent(task);
        task->appendTask(new PerformPortionTask(portions[i]));
        if (portions[i]->getHandler()->isMaster() || !devices[portions[i]->getDeviceNum()]->hasSlots())
            masterTasks.push_back(task);
        else
            devices[portions[i]->getDeviceNum()]->commitTask(task, portions[i]->getTargetSlot());
    }
    portions.clear();
    for (int i = 0; i < (int)masterTasks.size(); i++) {
        isInParloop = true;
        masterTasks[i]->execute();
        delete masterTasks[i];
        isInParloop = false;
    }
    masterTasks.clear();
    return endEvent;
}

void DvmhLoop::afterExecution() {
    checkInternal(phase == lpExecution);
    handleShadowCompute(COMPUTE_DONE);
    if (hasLocal) {
        if (region && region->withCompareDebug()) {
            for (std::map<DvmhData *, DvmhRegionData *>::iterator it = region->getDatas()->begin(); it != region->getDatas()->end(); it++) {
                DvmhRegionData *rdata = it->second;
                DvmhPieces *p = new DvmhPieces(rdata->getData()->getRank());
                p->append(rdata->getOutPieces());
                p->append(rdata->getLocalPieces());
                p->intersectInplace(rdata->getInPieces());
                region->compareDatas(rdata, p);
                delete p;
            }
        }
        persistentInfo->incrementExecCount();
    }
    phase = lpFinished;
    if (debugFlag) {
        dvmh_dbg_loop_end_C();
        if (!reductions.empty()) {
            dvmh_dbg_loop_red_group_delete_C((DvmType)this);
        }
    }
}

DvmhLoop::~DvmhLoop() {
    checkInternal(phase == lpFinished);
    for (int i = 0; i < (int)reductions.size(); i++) {
        assert(reductions[i] != 0);
        delete reductions[i];
    }
    reductions.clear();
    for (int i = 0; i < (int)consistents.size(); i++) {
        assert(consistents[i].second != 0);
        delete[] consistents[i].second;
    }
    consistents.clear();
    for (int i = 0; i < (int)rmas.size(); i++) {
        assert(rmas[i].second != 0);
        delete[] rmas[i].second;
    }
    rmas.clear();
    assert(loopBounds != 0 || rank == 0);
    delete[] loopBounds;
    delete[] localPart;
    delete[] localPlusShadow;
    for (int i = 0; i < DEVICE_TYPES; i++) {
        for (int j = 0; j < (int)handlers[i].size(); j++) {
            assert(handlers[i][j] != 0);
            delete handlers[i][j];
        }
        handlers[i].clear();
    }
    if (alignRule) {
        DvmhDistribSpace *dspace = alignRule->getDspace();
        delete alignRule;
        if (dspace && dspace->getRefCount() == 0)
            delete dspace;
    }
    shadowComputeDatas.clear();
    delete[] shadowComputeWidths;
    delete[] mappingDataRules;
}

void DvmhLoop::renewDependencyMask() {
    // Find dimensions with dependencies, which are generated by ACROSS
    checkInternal(phase == lpExecution);
    dependencyMask = 0;
    int firstDep = -1;
    DvmhShadow acrossAll;
    acrossAll.append(acrossOld);
    acrossAll.append(acrossNew);
    DvmhDistribSpace *dspace = alignRule ? alignRule->getDspace() : 0;
    for (int i = 0; i < acrossAll.dataCount(); i++) {
        const DvmhShadowData *sdata = &acrossAll.getData(i);
        DvmhData *data = sdata->data;
        assert(data->isDistributed());
        for (int j = 0; j < data->getRank(); j++) {
            if (sdata->shdWidths[j][0] > 0 || sdata->shdWidths[j][1] > 0) {
                if (data->getAlignRule()->getDspace() == dspace) {
                    int loopAxis = -1;
                    if (mappingData == data && mappingDataRules) {
                        loopAxis = mappingDataRules[j].axisNumber;
                    } else {
                        int foundDspaceAxis = data->getAlignRule()->getDspaceAxis(j + 1);
                        if (foundDspaceAxis > 0)
                            loopAxis = alignRule->getAxisRule(foundDspaceAxis)->axisNumber;
                    }
                    if (loopAxis > 0) {
                        if (firstDep == -1 || loopAxis < firstDep)
                            firstDep = loopAxis;
                        if (!dvmhSettings.reduceDependencies || (hasLocal && localPlusShadow[loopAxis - 1][0] < localPlusShadow[loopAxis - 1][1]))
                            dependencyMask |= ((UDvmType)1 << (rank - loopAxis));
                    } else {
                        const char *name = (region ? region->getDataName(data) : 0);
                        checkError3(false, "ACROSS array %s has dependency on dimension %d, which can not be tied with the parallel loop", (name ? name : ""),
                                j + 1);
                    }
                } else {
                    const char *name = (region ? region->getDataName(data) : 0);
                    checkError3(false, "ACROSS array %s is mapped on different template, than corresponding parallel loop", (name ? name : ""));
                }
            }
        }
    }
    if (firstDep > 0 && dependencyMask == 0) {
        // There are only collapsed dimensions with dependencies
        // We need to mark at least one dimension
        dependencyMask |= ((UDvmType)1 << (rank - firstDep));
    }
}

void DvmhLoop::shadowComputeOneRData(DvmhDistribSpace *dspace, const ShdWidth dspaceShdWidths[], ShadowComputeStage stage, DvmhData *data,
        DvmhRegionData *rdata) {
    int dataRank = data->getRank();
    if (dataRank > 0 && data->isDistributed() && data->getAlignRule()->getDspace() == dspace && (rdata->hasOutPieces() || rdata->hasLocalPieces())) {
#ifdef NON_CONST_AUTOS
        ShdWidth widths[dataRank];
#else
        ShdWidth widths[MAX_ARRAY_RANK];
#endif
        for (int j = 0; j < dataRank; j++) {
            widths[j][0] = 0;
            widths[j][1] = 0;
        }
        for (int j = 0; j < dspace->getRank(); j++) {
            DvmhAxisAlignRule axisRule;
            axisRule = *data->getAlignRule()->getAxisRule(j + 1);
            int ax = axisRule.axisNumber;
            if (ax > 0) {
                if (axisRule.multiplier > 0) {
                    widths[ax - 1][0] = dspaceShdWidths[j][0] / axisRule.multiplier;
                    widths[ax - 1][1] = dspaceShdWidths[j][1] / axisRule.multiplier;
                } else {
                    widths[ax - 1][0] = dspaceShdWidths[j][1] / (-axisRule.multiplier);
                    widths[ax - 1][1] = dspaceShdWidths[j][0] / (-axisRule.multiplier);
                }
            }
        }
        // XXX: We do not diagnose an error there because it is possible that in fact that array won't be written in its shadow in the loop
        for (int j = 0; j < dataRank; j++) {
            widths[j][0] = std::min(widths[j][0], data->getShdWidth(j + 1)[0]);
            widths[j][1] = std::min(widths[j][1], data->getShdWidth(j + 1)[1]);
        }
        if (stage == COMPUTE_PREPARE) {
            PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpShadow);
            for (int j = 0; j < devicesCount; j++) {
                if (rdata->getLocalPart(j)) {
                    data->getActualShadow(j, rdata->getLocalPart(j), true, widths, true);
                }
            }
        } else if (stage == COMPUTE_DONE) {
            for (int j = 0; j < devicesCount; j++) {
                if (rdata->getLocalPart(j)) {
                    data->shadowComputed(j, rdata->getLocalPart(j), true, widths);
                }
            }
            data->updateShadowProfile(true, widths);
            rdata->setRenewFlag();
        } else {
            checkInternal2(0, "Unknown shadow compute stage");
        }
    }
}

void DvmhLoop::handleShadowCompute(ShadowComputeStage stage) {
    checkInternal(phase == lpExecution);
    if (region && shadowComputeWidths) {
        if (!shadowComputeDatas.empty()) {
            for (int i = 0; i < (int)shadowComputeDatas.size(); i++) {
                DvmhData *data = shadowComputeDatas[i];
                DvmhRegionData *rdata = dictFind2(*region->getDatas(), data);
                shadowComputeOneRData(alignRule->getDspace(), shadowComputeWidths, stage, data, rdata);
            }
        } else {
            for (std::map<DvmhData *, DvmhRegionData *>::iterator it = region->getDatas()->begin(); it != region->getDatas()->end(); it++)
                shadowComputeOneRData(alignRule->getDspace(), shadowComputeWidths, stage, it->first, it->second);
        }
    }
    // TODO: What to do with loops with SHADOW_COMPUTE outside regions? Should shadow profile be updated? On which arrays?
}

bool DvmhLoop::mapPartOnDevice(const LoopBounds part[], int device, LoopBounds res[]) const {
    checkInternal(phase == lpExecution);
    bool hasLocal = true;
#ifdef NON_CONST_AUTOS
    Interval resInter[rank];
#else
    Interval resInter[MAX_LOOP_RANK];
#endif
    part->toBlock(rank, resInter);
    if (rank > 0) {
        if (alignRule != 0) {
            DvmhDistribSpace *dspace = alignRule->getDspace();
            assert(dspace != 0);
#ifdef NON_CONST_AUTOS
            Interval localDspace[dspace->getRank()];
#else
            Interval localDspace[MAX_DISTRIB_SPACE_RANK];
#endif
            if (region) {
                // Loop (or sequential part) inside region
                DvmhRegionDistribSpace *rdspace = dictFind2(*region->getDspaces(), dspace);
                assert(rdspace != 0);
                hasLocal = hasLocal && rdspace->getLocalPart(device) != 0;
                if (hasLocal)
                    localDspace->blockAssign(dspace->getRank(), rdspace->getLocalPart(device));
            } else {
                // Loop outside region
                hasLocal = hasLocal && device == 0;
                hasLocal = hasLocal && dspace->hasLocal();
                if (hasLocal)
                    localDspace->blockAssign(dspace->getRank(), dspace->getLocalPart());
            }
            if (hasLocal && !alignRule->hasIndirect()) {
#ifdef NON_CONST_AUTOS
                Interval computePart[dspace->getRank()];
                DvmType absSteps[rank];
#else
                Interval computePart[MAX_DISTRIB_SPACE_RANK];
                DvmType absSteps[MAX_LOOP_RANK];
#endif
                for (int i = 0; i < dspace->getRank(); i++) {
                    computePart[i][0] = localDspace[i][0] - (shadowComputeWidths ? shadowComputeWidths[i][0] : 0);
                    computePart[i][1] = localDspace[i][1] + (shadowComputeWidths ? shadowComputeWidths[i][1] : 0);
                }
                for (int i = 0; i < rank; i++)
                    absSteps[i] = std::abs(part[i][2]);
                hasLocal = alignRule->mapOnPart(computePart, resInter, true, absSteps);
            }
        }
    }
    for (int i = 0; i < rank; i++)
        res[i] = LoopBounds::create(resInter[i], part[i][2]);
    if (hasLocal && rank > 0) {
        for (int i = 0; i < rank; i++)
            dvmh_log(TRACE, "loop localPart [" DTFMT ".." DTFMT "] step " DTFMT, res[i][0], res[i][1], res[i][2]);
    }
    return hasLocal;
}

bool DvmhLoop::dividePortion(LoopBounds partialBounds[], int totalCount, int curIndex, LoopBounds curBounds[]) const {
    checkInternal(phase == lpExecution);
    int divideDimension = -1;
    bool res = false;
    for (int i = 1; i <= rank; i++) {
        if ((dependencyMask & ((UDvmType)1 << (rank - i))) == 0) {
            if (partialBounds[i - 1].iterCount() > 1) {
                divideDimension = i;
                break;
            }
        }
    }
    if (divideDimension > 0) {
        DvmType st = partialBounds[divideDimension - 1][0];
        DvmType step = partialBounds[divideDimension - 1][2];
        DvmType iterCount = partialBounds[divideDimension - 1].iterCount();
        DvmType myIterCount = iterCount / totalCount + (curIndex < iterCount % totalCount);
        if (myIterCount > 0) {
            typedMemcpy(curBounds, partialBounds, rank);
            DvmType iterSkip = curIndex * (iterCount / totalCount) + std::min((DvmType)curIndex, iterCount % totalCount);
            curBounds[divideDimension - 1][0] = st + step * iterSkip;
            curBounds[divideDimension - 1][1] = curBounds[divideDimension - 1][0] + step * (myIterCount - 1);
            res = true;
        }
    } else {
        if (curIndex == 0) {
            typedMemcpy(curBounds, partialBounds, rank);
            res = true;
        }
    }
    return res;
}

// HostHandlerOptimizationParams

HostHandlerOptimizationParams *HostHandlerOptimizationParams::genWithDefaults() const {
    HostHandlerOptimizationParams *params = new HostHandlerOptimizationParams(deviceNum);
    int resThreads = 1;
    if (isParallel)
        resThreads = std::max(1, devices[deviceNum]->getSlotCount());
    params->setThreads(resThreads, 0);
    return params;
}

std::vector<int> HostHandlerOptimizationParams::genParamsRanges() {
    serialization.clear();
    if (explFlag == 1 || !dvmhSettings.optimizeParams)
        return std::vector<int>(1, 0);
    if (isParallel) {
        for (int i = devices[deviceNum]->getSlotCount(); i >= 2; i--)
            serialization.push_back(i);
    }
    serialization.push_back(1);
    return std::vector<int>(1, serialization.size());
}

void HostHandlerOptimizationParams::fixSerializedIndex(HandlerOptimizationParams *p, int paramNumber) const {
    int givenThreads = ((HostHandlerOptimizationParams *)p)->getThreads();
    int found = 0;
    for (int i = 0; i < (int)serialization.size(); i++) {
        if (givenThreads == serialization[i]) {
            ((HostHandlerOptimizationParams *)p)->serializedIndex = i;
            found = 1;
            break;
        }
    }
    assert(found);
}

void HostHandlerOptimizationParams::applySerialized(HandlerOptimizationParams *p, const std::vector<int> values) const {
    if (values[0] >= 0) {
        assert(values[0] < (int)serialization.size());
        ((HostHandlerOptimizationParams *)p)->setThreads(serialization[values[0]], 2);
        ((HostHandlerOptimizationParams *)p)->serializedIndex = values[0];
    }
}


// CudaHandlerOptimizationParams

CudaHandlerOptimizationParams *CudaHandlerOptimizationParams::genWithDefaults() const {
    CudaHandlerOptimizationParams *params = new CudaHandlerOptimizationParams(deviceNum);
    params->maxRank = maxRank;
    if (explFlag == 1) {
        params->setBlock(block, explFlag);
    } else {
        int block[3];
        block[0] = block[1] = block[2] = 1;
        if (maxRank >= 3) {
            block[0] = 32;
            block[1] = 4;
            block[2] = 2;
        } else if (maxRank >= 2) {
            block[0] = 32;
            block[1] = 8;
        } else if (maxRank >= 1) {
            block[0] = 256;
        }
        params->setBlock(block, 0);
    }
    return params;
}

std::vector<int> CudaHandlerOptimizationParams::genParamsRanges() {
    serialization.clear();
    if (explFlag == 1 || !dvmhSettings.optimizeParams)
        return std::vector<int>(1, 0);
    if (maxRank == 0) {
        int curBlock[3] = {1, 1, 1};
        serialization.push_back(curBlock);
    } else {
        int steps[3] = {8, 1, 1};
        int step = lcm(steps[0] * steps[1] * steps[2], device->warpSize);
        int beg = step;
        int end = device->maxThreadsPerBlock;
        end = std::min((UDvmType)end, (device->regsPerSM / roundUpU(regCount * device->warpSize, device->regsGranularity)) * device->warpSize);
        while (roundUpU(end * sharedCount, device->sharedGranularity) > device->sharedPerSM)
            end--;
        end = roundDownU(end, step);
        // Good ones
        for (int cur = beg; cur <= end; cur += step) {
            int curBlock[3];
            for (curBlock[0] = steps[0]; curBlock[0] <= (int)device->maxBlockSize[0] && curBlock[0] <= cur; curBlock[0] += steps[0]) {
                for (curBlock[1] = steps[1]; curBlock[1] <= (int)device->maxBlockSize[1] && curBlock[0] * curBlock[1] <= cur; curBlock[1] += steps[1]) {
                    curBlock[2] = cur / curBlock[0] / curBlock[1];
                    if (curBlock[2] <= (int)device->maxBlockSize[2] && curBlock[2] % steps[2] == 0 && curBlock[0] * curBlock[1] * curBlock[2] == cur) {
                        if ((maxRank >= 3 || curBlock[2] == 1) && (maxRank >= 2 || curBlock[1] == 1))
                            serialization.push_back(curBlock);
                    }
                }
            }
        }
        // All other
        int curBlock[3];
        for (curBlock[0] = 1; curBlock[0] <= (int)device->maxBlockSize[0] && curBlock[0] <= end; curBlock[0]++) {
            for (curBlock[1] = 1; curBlock[1] <= (int)device->maxBlockSize[1] && curBlock[0] * curBlock[1] <= end; curBlock[1]++) {
                for (curBlock[2] = 1; curBlock[2] <= (int)device->maxBlockSize[2] && curBlock[0] * curBlock[1] * curBlock[2] <= end; curBlock[2]++) {
                    if (curBlock[0] * curBlock[1] * curBlock[2] < beg || curBlock[0] * curBlock[1] * curBlock[2] % step != 0 ||
                            (curBlock[0] % steps[0] != 0 || curBlock[1] % steps[1] != 0 || curBlock[2] % steps[2] != 0)) {
                        if ((maxRank >= 3 || curBlock[2] == 1) && (maxRank >= 2 || curBlock[1] == 1))
                            serialization.push_back(curBlock);
                    }
                }
            }
        }
        // TODO: add information for distinction of good ones and all other to optimizer
    }
    return std::vector<int>(1, serialization.size());
}

void CudaHandlerOptimizationParams::fixSerializedIndex(HandlerOptimizationParams *p, int paramNumber) const {
    int givenBlock[3];
    ((CudaHandlerOptimizationParams *)p)->getBlock(givenBlock);
    int found = 0;
    for (int i = 0; i < (int)serialization.size(); i++) {
        if (givenBlock[0] == serialization[i].x && givenBlock[1] == serialization[i].y && givenBlock[2] == serialization[i].z) {
            ((CudaHandlerOptimizationParams *)p)->serializedIndex = i;
            found = 1;
            break;
        }
    }
    assert(found);
}

void CudaHandlerOptimizationParams::applySerialized(HandlerOptimizationParams *p, const std::vector<int> values) const {
    if (values[0] >= 0) {
        assert(values[0] < (int)serialization.size());
        int newBlock[3];
        newBlock[0] = serialization[values[0]].x;
        newBlock[1] = serialization[values[0]].y;
        newBlock[2] = serialization[values[0]].z;
        ((CudaHandlerOptimizationParams *)p)->setBlock(newBlock, 2);
        ((CudaHandlerOptimizationParams *)p)->serializedIndex = values[0];
    }
}

void CudaHandlerOptimizationParams::roundBlock(const int inBlock[3], int outBlock[3]) const {
    outBlock[0] = outBlock[1] = outBlock[2] = 1;
    if (maxRank > 0) {
        int goal = roundUpU(inBlock[0] * inBlock[1] * inBlock[2], device->warpSize);
        int rest = goal;
        for (int i = 0; i < maxRank; i++) {
            int val = std::min(rest, inBlock[i]);
            while (rest % val != 0)
                val++;
            outBlock[i] = val;
            rest /= val;
        }
        outBlock[0] *= rest;
        assert(goal == outBlock[0] * outBlock[1] * outBlock[2]);
    }
}

// ParamDesc

ParamDesc::ParamDesc(int aRange) {
    range = aRange;
    if (range < 0)
        range = 0;
    bestVal = -1;
    if (range > 0) {
        ParamInterval pi;
        pi.begin = 0;
        pi.end = range - 1;
        pi.part = 1.0;
        notSeen.push(pi);
        times.resize(range, -1);
        orders.resize(range, 0);
    }
}

int ParamDesc::pickNotSeen() {
    assert(!notSeen.empty());
    ParamInterval wasInt = notSeen.top();
    notSeen.pop();
    int res = (wasInt.begin + wasInt.end) / 2;
    if (res > wasInt.begin) {
        ParamInterval newInt;
        newInt.begin = wasInt.begin;
        newInt.end = res - 1;
        newInt.part = wasInt.part / (wasInt.end - wasInt.begin + 1) * (newInt.end - newInt.begin + 1);
        notSeen.push(newInt);
    }
    if (res < wasInt.end) {
        ParamInterval newInt;
        newInt.begin = res + 1;
        newInt.end = wasInt.end;
        newInt.part = wasInt.part / (wasInt.end - wasInt.begin + 1) * (newInt.end - newInt.begin + 1);
        notSeen.push(newInt);
    }
    return res;
}

int ParamDesc::pickSeen() {
    assert(!seen.empty());
    return seen.begin()->second;
}

void ParamDesc::update(int val, double res, DvmType order) {
    assert(val >= 0 && val < range);
    if (orders[val] > 0)
        seen.erase(orders[val]);
    orders[val] = order;
    seen[orders[val]] = val;
    if (bestVal < 0 || times[bestVal] < 0 || times[bestVal] > res) {
        bestVal = val;
    } else if (bestVal == val) {
        // Maybe no more best
        // XXX: may be slow
        times[bestVal] = res;
        for (int i = 0; i < range; i++) {
            if (times[i] >= 0 && times[i] < times[bestVal])
                bestVal = i;
        }
    }
    times[val] = res;
}

// LoopHandlerPerformanceInfo

void LoopHandlerPerformanceInfo::setAccumulation(int count) {
    accumIters = 0;
    accumTime = 0;
    if (count > 1)
        accumRest = count;
    else
        accumRest = 0;
}

LoopHandlerPerformanceInfo::LoopHandlerPerformanceInfo() {
    deviceNumber = -1;
    accumIters = 0;
    accumTime = 0;
    accumRest = 0;
    rootParams = 0;
    bestParams = 0;
    bestCoef = 1;
    execCount = 0;
    referenceTime = -1;
}

LoopHandlerPerformanceInfo::LoopHandlerPerformanceInfo(const LoopHandlerPerformanceInfo &info) {
    deviceNumber = info.deviceNumber;
    accumIters = info.accumIters;
    accumTime = info.accumTime;
    accumRest = info.accumRest;
    rootParams = info.rootParams ? info.rootParams->clone() : 0;
    bestParams = info.bestParams ? info.bestParams->clone() : 0;
    bestCoef = info.bestCoef;
    params = info.params;
    execCount = info.execCount;
    referenceTime = info.referenceTime;
    tableFunc = info.tableFunc;
}

LoopHandlerPerformanceInfo &LoopHandlerPerformanceInfo::operator=(const LoopHandlerPerformanceInfo &info) {
    if (this != &info) {
        deviceNumber = info.deviceNumber;
        accumIters = info.accumIters;
        accumTime = info.accumTime;
        accumRest = info.accumRest;
        delete rootParams;
        rootParams = info.rootParams ? info.rootParams->clone() : 0;
        delete bestParams;
        bestParams = info.bestParams ? info.bestParams->clone() : 0;
        bestCoef = info.bestCoef;
        params = info.params;
        execCount = info.execCount;
        referenceTime = info.referenceTime;
        tableFunc = info.tableFunc;
    }
    return *this;
}

std::map<double, double> LoopHandlerPerformanceInfo::genPerfTableFunc() const {
    std::map<double, double> res;
    for (TableFunc::const_iterator it = tableFunc.begin(); it != tableFunc.end(); it++)
        res.insert(std::make_pair(exp(it->first), exp(it->first) / (referenceTime * bestCoef * it->second.first)));
    return res;
}

void LoopHandlerPerformanceInfo::update(double iterCount, double perf, HandlerOptimizationParams *curParams) {
    SpinLockGuard guard(lock);
    if (accumRest > 0) {
        accumIters += iterCount;
        accumTime = std::max(accumTime, iterCount / perf);
        accumRest--;
        if (accumRest == 0) {
            iterCount = accumIters;
            perf = accumIters / accumTime;
        } else {
            return;
        }
    }
    execCount++;
    double curTime = iterCount / perf;
    double logIterCount = log(iterCount);
    if (referenceTime < 0)
        initParams();
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i].hasValues()) {
            if (curParams->getSerializedIndex(i) < 0)
                rootParams->fixSerializedIndex(curParams, i);
        }
    }
    if (referenceTime < 0) {
        referenceTime = curTime;
        for (int i = 0; i < (int)params.size(); i++) {
            if (params[i].hasValues())
                params[i].update(curParams->getSerializedIndex(i), 1.0, execCount);
        }
        tableFunc[logIterCount] = std::make_pair(1.0, execCount);
    } else {
        DvmType partOrder = getOrder(logIterCount);
        int hasSamePart = partOrder > 0;
        int hasSameParamsCount = hasSamePart;
        int optParamCount = 0;
        for (int i = 0; i < (int)params.size(); i++) {
            if (params[i].hasValues()) {
                if (params[i].isSeen(curParams->getSerializedIndex(i)))
                    hasSameParamsCount++;
                optParamCount++;
            }
        }
        int newInd = -2;
        if (hasSameParamsCount >= optParamCount + 1) {
            // All were, discarding one oldest
            DvmType oldestVal = partOrder;
            newInd = -1;
            for (int i = 0; i < (int)params.size(); i++) {
                if (params[i].hasValues()) {
                    if (params[i].getOrder(curParams->getSerializedIndex(i)) < oldestVal) {
                        oldestVal = params[i].getOrder(curParams->getSerializedIndex(i));
                        newInd = i;
                    }
                }
            }
        } else if (hasSameParamsCount >= optParamCount) {
            // One is new
            if (!hasSamePart) {
                newInd = -1;
            } else {
                for (int i = 0; i < (int)params.size(); i++) {
                    if (params[i].hasValues()) {
                        if (!params[i].isSeen(curParams->getSerializedIndex(i))) {
                            newInd = i;
                            break;
                        }
                    }
                }
            }
        } else {
            // Bad launch. The only way we can handle it - discard the whole
            dvmh_log(DEBUG, "Bad mix of parameters received");
            return;
        }
        assert(newInd >= -1 && newInd < (int)params.size());
        double res = curTime / referenceTime;
        double curCoef = getCoef(logIterCount);
        if (newInd != -1)
            res /= curCoef;
        for (int i = 0; i < (int)params.size(); i++) {
            if (params[i].hasValues()) {
                if (newInd != i)
                    res /= params[i].getRes(curParams->getSerializedIndex(i));
            }
        }
        if (newInd == -1) {
            if (res / curCoef > 5) {
                dvmh_log(DEBUG, "Too bad experimental result (%g times slower), ignoring", res / curCoef);
            } else {
                double w = 0;
                if (tableFunc.size() >= 2) {
                    double totalWidth = tableFunc.rbegin()->first - tableFunc.begin()->first;
                    assert(totalWidth >= 0);
                    w = totalWidth / maxTblFuncSize / 2;
                }
                if (w < 1e-9)
                    w = 1e-9;
                tableFunc.erase(tableFunc.lower_bound(logIterCount - w), tableFunc.upper_bound(logIterCount + w));
                double smoothDegree = 12;
                tableFunc[logIterCount] = std::make_pair(pow(0.5 * pow(res, 1 / smoothDegree) + 0.5 * pow(curCoef, 1 / smoothDegree), smoothDegree), execCount);
            }
        } else {
            params[newInd].update(curParams->getSerializedIndex(newInd), res, execCount);
        }
    }
    // Modify best params
    if (!bestParams)
        bestParams = curParams->clone();
    std::vector<int> serVals(params.size(), -1);
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i].hasValues())
            serVals[i] = params[i].getBestVal();
    }
    rootParams->applySerialized(bestParams, serVals);
    bestCoef = 1;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i].isSeen(params[i].getBestVal()))
            bestCoef *= params[i].getRes(params[i].getBestVal());
    }
    // Keep table function thin
    while ((int)tableFunc.size() > maxTblFuncSize) {
        TableFunc::iterator prevIt = tableFunc.begin();
        TableFunc::iterator curIt = prevIt;
        ++curIt;
        TableFunc::iterator minIt = prevIt;
        double minDist = 2 * (curIt->first - prevIt->first);
        while (curIt != tableFunc.end()) {
            TableFunc::iterator nextIt = curIt;
            ++nextIt;
            double curDist;
            if (nextIt != tableFunc.end())
                curDist = nextIt->first - prevIt->first;
            else
                curDist = 2 * (curIt->first - prevIt->first);
            if (curDist < minDist) {
                minDist = curDist;
                minIt = curIt;
            }
            curIt = nextIt;
        }
        tableFunc.erase(minIt);
    }
}

std::pair<double, HandlerOptimizationParams *> LoopHandlerPerformanceInfo::getBestParams(double iterCount) const {
    if (tableFunc.empty())
        return std::make_pair(-1.0, (HandlerOptimizationParams *)0);
    assert(iterCount > 0);
    HandlerOptimizationParams *resParams = bestParams;
    double coef = bestCoef * getCoef(log(iterCount));
    double resPerf = iterCount / (referenceTime * coef);
    return std::make_pair(resPerf, resParams);
}

HandlerOptimizationParams *LoopHandlerPerformanceInfo::pickOptimizationParams(double iterCount) {
    HandlerOptimizationParams *resParams = 0;
    assert(rootParams);
    resParams = rootParams->genWithDefaults();
    int firstRun = referenceTime < 0;
    if (!firstRun) {
        double logIterCount = log(iterCount);
        DvmType partOrder = getOrder(logIterCount);
        int hasSamePart = partOrder > 0;
        int newInd = (hasSamePart ? -2 : -1);
        int seenInd = (hasSamePart ? -1 : -2);
        if (hasSamePart) {
            // We can choose at most one unseen parameter
            double maxPart = -1;
            for (int i = 0; i < (int)params.size(); i++) {
                if (params[i].hasValues()) {
                    if (params[i].hasNotSeen() && maxPart < params[i].getNotSeenPart()) {
                        maxPart = params[i].getNotSeenPart();
                        newInd = i;
                    }
                }
            }
            if (newInd < 0) {
                DvmType oldestVal = partOrder;
                for (int i = 0; i < (int)params.size(); i++) {
                    if (params[i].hasValues()) {
                        if (params[i].getOldestOrder() < oldestVal) {
                            oldestVal = params[i].getOldestOrder();
                            seenInd = i;
                        }
                    }
                }
            }
        }
        assert(newInd < 0 || seenInd < 0);
        std::vector<int> vals((int)params.size(), -1);
        for (int i = 0; i < (int)params.size(); i++) {
            if (params[i].hasValues()) {
                if (newInd == i)
                    vals[i] = params[i].pickNotSeen();
                else if (seenInd == i)
                    vals[i] = params[i].pickSeen();
                else
                    vals[i] = params[i].getBestVal();
            }
        }
        rootParams->applySerialized(resParams, vals);
    }
    return resParams;
}

void LoopHandlerPerformanceInfo::initParams() {
    std::vector<int> ranges = rootParams->genParamsRanges();
    params.clear();
    for (int i = 0; i < (int)ranges.size(); i++) {
        params.push_back(ParamDesc(ranges[i]));
    }
}

double LoopHandlerPerformanceInfo::getCoef(double logIterCount) const {
    double iterCount = exp(logIterCount);
    double res = 1;
    TableFunc::const_iterator it2 = tableFunc.lower_bound(logIterCount);
    if (it2 != tableFunc.end()) {
        TableFunc::const_iterator it1 = it2;
        if (it1 != tableFunc.begin())
            it1--;
        if (it1->first < it2->first) {
            double range = it2->first - it1->first;
            double perf1 = exp(it1->first) / it1->second.first;
            double perf2 = exp(it2->first) / it2->second.first;
            double resPerf = (it2->first - logIterCount) / range * perf1 + (logIterCount - it1->first) / range * perf2;
            res = iterCount / resPerf;
        } else {
            double nextIterCount = exp(it2->first);
            res = it2->second.first * iterCount / nextIterCount;
        }
    } else if (!tableFunc.empty()) {
        double lastIterCount = exp(tableFunc.rbegin()->first);
        res = tableFunc.rbegin()->second.first * iterCount / lastIterCount;
    }
    return res;
}

DvmType LoopHandlerPerformanceInfo::getOrder(double logIterCount) {
    double w = 0;
    if (tableFunc.size() >= 2) {
        double totalWidth = tableFunc.rbegin()->first - tableFunc.begin()->first;
        assert(totalWidth >= 0);
        w = totalWidth / maxTblFuncSize / 2;
    }
    if (w < 1e-9)
        w = 1e-9;
    TableFunc::iterator it1 = tableFunc.lower_bound(logIterCount - w);
    TableFunc::iterator it2 = tableFunc.upper_bound(logIterCount + w);
    DvmType partOrder = execCount;
    int coveredCount = 0;
    for (TableFunc::iterator it3 = it1; it3 != it2; it3++) {
        coveredCount++;
        if (partOrder > it3->second.second)
            partOrder = it3->second.second;
    }
    if (coveredCount == 0)
        return -1;
    else
        return partOrder;
}

LoopHandlerPerformanceInfo::~LoopHandlerPerformanceInfo() {
    delete rootParams;
    delete bestParams;
}

// DvmhLoopPersistentInfo

DvmhLoopPersistentInfo::DvmhLoopPersistentInfo(const SourcePosition &sp, DvmhRegionPersistentInfo *aRegionInfo): sourcePos(sp), regionInfo(aRegionInfo) {
    if (regionInfo)
        regionInfo->addLoopInfo(this);
    performanceInfos.resize(devicesCount);
    execCount = 0;
    typicalSpace = 1;
    mappingVarId = -1;
}

void DvmhLoopPersistentInfo::fix(DvmhLoop *loop) {
    bool firstLaunch = execCount == 0;
    for (int i = 0; i < devicesCount; i++) {
        if (performanceInfos[i].size() != loop->handlers[devices[i]->getType()].size()) {
            checkInternal2(firstLaunch, "Handlers count somehow changed for the parallel loop");
            performanceInfos[i].clear();
            performanceInfos[i].resize(loop->handlers[devices[i]->getType()].size());
            for (int j = 0; j < (int)performanceInfos[i].size(); j++) {
                performanceInfos[i][j].setDeviceNumber(i);
                if (devices[i]->getType() == dtHost)
                    performanceInfos[i][j].setRootParams(new HostHandlerOptimizationParams(i));
                else if (devices[i]->getType() == dtCuda)
                    performanceInfos[i][j].setRootParams(new CudaHandlerOptimizationParams(i));
            }
        }
        for (int j = 0; j < (int)performanceInfos[i].size(); j++) {
            performanceInfos[i][j].getRootParams()->setFromLoop(loop);
        }
    }
    if (loop->hasLocal) {
        double currentSpace = 0;
        currentSpace = 1;
        for (int i = 0; i < loop->rank; i++)
            currentSpace *= loop->localPlusShadow[i].iterCount();
        typicalSpace = (typicalSpace * execCount + currentSpace) / (execCount + 1);
    }
}

std::pair<int, HandlerOptimizationParams *> DvmhLoopPersistentInfo::getBestParams(int dev, double part, double *pTime) const {
    double bestPerf = -1;
    int bestHandler = -1;
    HandlerOptimizationParams *bestParams = 0;
    double iterCount = typicalSpace * part;
    for (int i = 0; i < (int)performanceInfos[dev].size(); i++) {
        std::pair<double, HandlerOptimizationParams *> cur = performanceInfos[dev][i].getBestParams(iterCount);
        if (cur.first > bestPerf && cur.first > 0) {
            bestPerf = cur.first;
            bestHandler = i;
            bestParams = cur.second;
        }
    }
    if (bestHandler >= 0 && pTime)
        *pTime = iterCount / bestPerf;
    return std::make_pair(bestHandler, bestParams);
}

double DvmhLoopPersistentInfo::estimateTime(DvmhRegion *region, int quantCount) const {
    double res = -1;
    if (!region) {
        double curTime = -1;
        if (getBestParams(0, 1.0 / quantCount, &curTime).first >= 0)
            res = curTime * quantCount;
    } else {
        DvmhData *mappingData = region->getDataByVarId(mappingVarId);
        if (!mappingData)
            return -1;
        assert(mappingData->isDistributed());
        DvmhDistribSpace *dspace = mappingData->getAlignRule()->getDspace();
        DvmhRegionDistribSpace *rdspace = region->getDspaces()->at(dspace);
        assert(rdspace);
        for (int i = 0; i < devicesCount; i++) {
            if (region->usesDevice(i)) {
                double curTime = -1;
                if (getBestParams(i, rdspace->getWeight(i) / quantCount, &curTime).first >= 0) {
                    // TODO: Take into account dependency case, when not maximum but sum is needed
                    res = std::max(res, curTime * quantCount);
                }
            }
        }
    }
    return res;
}

std::pair<int, HandlerOptimizationParams *> DvmhLoopPersistentInfo::pickOptimizationParams(int dev, double iterCount) {
// TODO: choose optimal handler for particular device
    int handlerNum = 0;
    HandlerOptimizationParams *params = performanceInfos[dev][handlerNum].pickOptimizationParams(iterCount);
    return std::make_pair(handlerNum, params);
}

// DvmhSpecLoop

bool DvmhSpecLoop::hasElement(DvmhData *data, const DvmType indexArray[]) const {
    assert(portion);
    DvmhLoop *loop = getLoop();
    assert(loop);
    if (loop->region) {
        return loop->region->hasElement(getDeviceNum(), data, indexArray);
    } else {
        bool res = data->hasLocal();
        if (data->hasLocal()) {
            for (int i = 0; i < data->getRank(); i++)
                res = res && data->getAxisLocalPart(i + 1).contains(indexArray[i]);
        }
        return res;
    }
}

void DvmhSpecLoop::fillLocalPart(DvmhData *data, DvmType part[]) const {
    assert(portion);
    DvmhLoop *loop = getLoop();
    assert(loop);
    if (loop->region) {
        loop->region->fillLocalPart(getDeviceNum(), data, part);
    } else {
        for (int i = 0; i < data->getRank(); i++) {
            part[2 * i + 0] = (data->hasLocal() ? data->getAxisLocalPart(i + 1)[0] : 1);
            part[2 * i + 1] = (data->hasLocal() ? data->getAxisLocalPart(i + 1)[1] : 0);
        }
    }
    for (int i = 0; i < data->getRank(); i++) {
        if (data->isDistributed() && data->getAlignRule()->isIndirect(i + 1)) {
            // Only all the space axis is acceptable for an indirect axis
            checkError2(part[2 * i + 0] == data->getAxisSpace(i + 1)[0] && part[2 * i + 1] == data->getAxisSpace(i + 1)[1],
                    "Cannot properly fill the local part because of the presence of an indirectly distributed axis");
        }
    }
}

// DvmhLoopCuda

void DvmhLoopCuda::addToToFree(void *addr) {
    toFree.push_back(addr);
}

template <typename IndexT>
IndexT *DvmhLoopCuda::getLocalPart(DvmhData *data) {
    int dataRank = data->getRank();
#ifdef NON_CONST_AUTOS
    DvmType part[dataRank * 2];
    IndexT hPart[dataRank * 2];
#else
    DvmType part[MAX_ARRAY_RANK * 2];
    IndexT hPart[MAX_ARRAY_RANK * 2];
#endif
    fillLocalPart(data, part);
    for (int i = 0; i < dataRank * 2; i++)
        hPart[i] = part[i];
    CudaDevice *cudaDev = (CudaDevice *)devices[getDeviceNum()];
    IndexT *dPart;
    dPart = cudaDev->alloc<IndexT>(dataRank * 2);
    cudaDev->setValues(dPart, hPart, dataRank * 2);
    addToToFree(dPart);
    return dPart;
}

template int *DvmhLoopCuda::getLocalPart(DvmhData *data);
template long *DvmhLoopCuda::getLocalPart(DvmhData *data);
template long long *DvmhLoopCuda::getLocalPart(DvmhData *data);

void DvmhLoopCuda::pickBlock(int sharedPerThread, int regsPerThread, int block[]) {
    DvmhLoop *loop = getLoop();
    CudaDevice *device = (CudaDevice *)devices[getDeviceNum()];
    int maxBlockRank = loop->rank - (loop->dependencyMask > 0);
    int defBlock[3];
    bool outsideDefault = block[0] >= 1 && block[1] >= 1 && block[2] >= 1;
    if (outsideDefault)
        typedMemcpy(defBlock, block, 3);
    int explBlock = ((CudaHandlerOptimizationParams *)portion->getOptParams())->getBlock(block);
    if (!explBlock && outsideDefault) {
        ((CudaHandlerOptimizationParams *)portion->getOptParams())->setBlock(defBlock, explBlock);
        ((CudaHandlerOptimizationParams *)portion->getOptParams())->getBlock(block);
    }
    int blockRank = 3;
    while (blockRank > 0 && block[blockRank - 1] == 1)
        blockRank--;
    checkError2(blockRank <= maxBlockRank, "Setting CUDA block with more dimensions than the parallel loop has (minus 1 in case of dependency) is prohibited");
    const char *axisNames[3] = {"x", "y", "z"};
    for (int i = 0; i < 3; i++)
        checkError3(block[i] <= (int)device->maxBlockSize[i], "Block size on axis %s (%d) is bigger than device can handle (%u)", axisNames[i], block[i],
                device->maxBlockSize[i]);
    checkError3(block[0] * block[1] * block[2] <= (int)device->maxThreadsPerBlock, "Block size (%d) is bigger than device can handle (%u)",
            block[0] * block[1] * block[2], device->maxThreadsPerBlock);
    int count = sharedPerThread;
    ((CudaHandlerOptimizationParams *)loop->persistentInfo->getLoopHandlerPerformanceInfo(portion->getDeviceNum(), portion->getHandlerNum())->getRootParams())
            ->setSharedCount(count);
    // TODO: determine didntKnow
    bool didntKnow = true;
    if (count > 0) {
        int needed = block[0] * block[1] * block[2] * count;
        int has = device->sharedPerSM;
        dvmh_log(TRACE, "Shared amount requested %d. Have %d", needed, has);
        if (needed > has) {
            checkError3(!loop->userCudaBlockFlag && didntKnow, "Shared memory amount is insufficient. Have %d, needed %d", has, needed);
            if (maxBlockRank >= 1)
                block[0] = std::max(1, (int)roundDownU(std::min(256, has / count), device->warpSize));
            else
                block[0] = 1;
            block[1] = 1;
            block[2] = 1;
            explBlock = 0;
        }
    }
    count = regsPerThread;
    ((CudaHandlerOptimizationParams *)loop->persistentInfo->getLoopHandlerPerformanceInfo(portion->getDeviceNum(), portion->getHandlerNum())->getRootParams())
            ->setRegsCount(count);
    // TODO: determine didntKnow
    didntKnow = true;
    if (count > 0) {
        int needed = block[0] * block[1] * block[2] * count;
        int has = device->regsPerSM;
        dvmh_log(TRACE, "Register count requested %d. Have %d", needed, has);
        if (needed > has) {
            checkError3(!loop->userCudaBlockFlag && didntKnow, "Register count is insufficient. Have %d, needed %d", has, needed);
            if (maxBlockRank >= 1)
                block[0] = std::max(1, (int)roundDownU(std::min(256, has / count), device->warpSize));
            else
                block[0] = 1;
            block[1] = 1;
            block[2] = 1;
            explBlock = 0;
        }
    }
    ((CudaHandlerOptimizationParams *)portion->getOptParams())->setBlock(block, explBlock);
    ((CudaHandlerOptimizationParams *)portion->getOptParams())->getBlock(block);
}

void DvmhLoopCuda::finishRed(DvmhReduction *red) {
    int redInd = -1;
    DvmhReductionCuda *cudaRed = getCudaRed(red, &redInd);
    checkInternal2(cudaRed, "Unknown reduction");
    if (getPortion()->getDoReduction())
        cudaRed->finish();
    delete cudaRed;
    if (reductions.size() > 1)
        reductions[redInd] = reductions[reductions.size() - 1];
    reductions.pop_back();
}

DvmhReductionCuda *DvmhLoopCuda::getCudaRed(DvmhReduction *red, int *pRedInd) {
    DvmhReductionCuda *cudaRed = 0;
    for (int i = 0; i < (int)reductions.size(); i++) {
        if (reductions[i]->getReduction() == red) {
            cudaRed = reductions[i];
            if (pRedInd)
                *pRedInd = i;
            break;
        }
    }
    return cudaRed;
}

void DvmhLoopCuda::finishAllReds() {
    for (int i = 0; i < (int)reductions.size(); i++) {
        if (getPortion()->getDoReduction())
            reductions[i]->finish();
        delete reductions[i];
    }
    reductions.clear();
}

void DvmhLoopCuda::syncAfterHandlerExec() {
#ifdef HAVE_CUDA
    checkErrorCuda(cudaDeviceSynchronize()); // Catch errors of CUDA handler's kernels
    checkErrorCuda(cudaGetLastError()); // Catch errors of CUDA handler's kernels
#endif
}

DvmhLoopCuda::~DvmhLoopCuda() {
    CudaDevice *device = (CudaDevice *)devices[getDeviceNum()];
    checkInternal(reductions.empty());
    for (int i = 0; i < (int)toFree.size(); i++)
        device->dispose(toFree[i]);
}

std::map<SourcePosition, DvmhLoopPersistentInfo *> loopDict; //SourcePosition => DvmhLoopPersistentInfo *

THREAD_LOCAL bool isInParloop = false;

}
