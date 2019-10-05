#pragma once

#include <cmath>
#include <map>
#include <queue>
#include <vector>

#ifdef HAVE_CUDA
#pragma GCC visibility push(default)
#include <cuda_runtime.h>
#pragma GCC visibility pop
#endif
#include <pthread.h>

#include "dvmh_data.h"
#include "dvmh_device.h"
#include "region.h"
#include "util.h"

namespace libdvmh {

class DvmhAxisAlignRule;

// Структура, описывающая редукционную переменную(массив)
class DvmhReduction {
public:
    enum RedFunction {rfSum = 1, rfProd = 2, rfMax = 3, rfMin = 4, rfAnd = 5, rfOr = 6, rfXor = 7, rfEqu = 8, rfNe = 9, rfEq = 10, RED_FUNCS};
public:
    RedFunction redFunc; // редукционная функция
    DvmhData::DataType arrayElementType; // тип элемента редукционного массива

    void *arrayAddr;
    UDvmType elemSize;
    UDvmType elemCount;

    void *locAddr;
    UDvmType locSize;
public:
    bool isLoc() const { return locAddr != 0; }
    bool isScalar() const { return elemCount == 1; }
public:
    explicit DvmhReduction(RedFunction aRedFunc, UDvmType VLength, DvmhData::DataType VType, void *Mem, UDvmType LocElmLength, void *LocMem);
public:
    void initValues(void *arrayPtr, void *locPtr) const;
    void initGlobalValues();
    void postValues(const void *arrayPtr, const void *locPtr);
    void addInitialValues();
    void performOperation(void *resultArrayPtr, void *resultLocPtr, const void *summandArrayPtr, const void *summandLocPtr) const;
public:
    ~DvmhReduction();
protected:
    DvmhSpinLock lock;
    char *initArrVal;
    char *initLocVal;
};

class DvmhReductionCuda {
public:
    DvmhReduction *getReduction() const { return reduction; }
    bool isPrepared() const { return gpuMem != 0; }
    DvmType getItemCount() const { return itemCount; }
public:
    DvmhReductionCuda(DvmhReduction *aReduction, void **aGpuMemPtr, void **aGpuLocMemPtr, int aDeviceNum);
public:
    void prepare(UDvmType items, bool fillFlag = false);
    void finish();
    void advancePtrsBy(UDvmType itemsDone);
public:
    ~DvmhReductionCuda();
protected:
    DvmhReduction *reduction;
    DvmType itemCount;
    char *gpuMem; // ссылка на аллоцированную на девайсе память
    char *gpuLocMem;
    char **gpuMemPtr;
    char **gpuLocMemPtr;
    char *localArray; // Own copy of reduction variable in host memory
    char *localLocArray; // Own copy of reduction variable in host memory
    int deviceNum;
};

class DvmhLoopHandler {
public:
    bool isParallel() const { return parallelFlag; }
    bool isMaster() const { return masterFlag; }
    void setParam(int i, void *val) { assert(i >= 0 && i < paramsCount); params[i] = val; }
public:
    explicit DvmhLoopHandler(bool parallel, bool master, DvmHandlerFunc func, int paramCount, int bases = 0);
public:
    void exec(void *curLoop, void *baseVal = 0);
public:
    ~DvmhLoopHandler() {
        delete[] params;
    }
protected:
    bool parallelFlag;
    bool masterFlag;
    DvmHandlerFunc f;
    void **params; // Array of void * (length=paramsCount)
    int paramsCount;
    int basesCount;
};

class DvmhLoopPortion;
class DvmhLoopPersistentInfo;

// Структура, описывающая DVMH цикл:
class DvmhLoop: public DvmhObject {
public:
    enum Phase {lpRegistrations = 0, lpExecution = 1, lpFinished = 2};
public:
    DvmhRegion *region; // Вычислительный регион, в котором цикл
    int rank; // Количество измерений
    LoopBounds *loopBounds; // полный параллелепипед витков DVM-цикла.
    bool hasLocal;
    LoopBounds *localPart; // подпараллелепипед витков DVM-цикла, который нужно выполнить текущему процессору в этот раз (в случае конвейера делается несколько запусков)
    LoopBounds *localPlusShadow;
    DvmhAlignRule *alignRule;
    HybridVector<DvmhReduction *, 10> reductions; //Array of pointers to DvmhReduction
    HybridVector<std::pair<DvmhData *, DvmhAxisAlignRule *>, 10> consistents; //Array of pointers to DvmhData and associated array of DvmhAxisAlignRule for CONSISTENT clauses
    HybridVector<std::pair<DvmhData *, DvmhAxisAlignRule *>, 10> rmas; //Array of pointers to DvmhData and associated array of DvmhAxisAlignRule for REMOTE_ACCESS clauses
    ShdWidth *shadowComputeWidths; //Array of (lower,upper) widths (in terms of DistribSpace) of computing shadow edges (length=alignRule->dspace->rank)
    HybridVector<DvmhData *, 10> shadowComputeDatas; //Array of DvmhData's which are registered for shadow compute
    HybridVector<DvmhLoopHandler *, 2> handlers[DEVICE_TYPES]; //Array of array of pointers to DvmhLoopHandler
    int cudaBlock[3];
    bool userCudaBlockFlag; // True, if cudaBlock have been set outside by user
    DvmType stage;
    DvmhLoopPersistentInfo *persistentInfo;
    UDvmType dependencyMask; // 0 - no dependency, 1 - have dependency. Lowest bit - innermost loop.
    DvmhShadow acrossOld; // Need to update this before execution (on the whole localPart)
    DvmhShadow acrossNew; // Need to update this before execution and push to HOST after execution
    DvmhData *mappingData; // Reference to data, on which loop is mapped, if any
    DvmhAxisAlignRule *mappingDataRules;
    bool debugFlag;
public:
    Phase getPhase() const { return phase; }
    void setRegion(DvmhRegion *aRegion) { assert(!region && phase == lpRegistrations); region = aRegion; }
    void setPersistentInfo(DvmhLoopPersistentInfo *persInfo) { assert(!persistentInfo && phase == lpRegistrations && persInfo); persistentInfo = persInfo; }
public:
    explicit DvmhLoop(int aRank, const LoopBounds aLoopBounds[]);
public:
    void setAlignRule(DvmhAlignRule *aAlignRule);
    void addReduction(DvmhReduction *red) {
        reductions.push_back(red);
    }
    void addShadowComputeData(DvmhData *data) {
        shadowComputeDatas.push_back(data);
    }
    void addHandler(int deviceType, DvmhLoopHandler *handler) {
        handlers[deviceType].push_back(handler);
    }
    void setCudaBlock(int xSize, int ySize = 1, int zSize = 1);
    void addToAcross(DvmhShadow oldGroup, DvmhShadow newGroup);
    void addToAcross(DvmhData *data, ShdWidth widths[], bool cornerFlag = false);
    void addConsistent(DvmhData *data, DvmhAxisAlignRule axisRules[]);
    void addRemoteAccess(DvmhData *data, DvmhAxisAlignRule axisRules[]);
    void prepareExecution();
    void executePart(const LoopBounds part[]);
    AggregateEvent *executePartAsync(const LoopBounds part[], DvmhEvent *depSrc = 0, AggregateEvent *endEvent = 0);
    void afterExecution();
public:
    ~DvmhLoop();
protected:
    enum ShadowComputeStage {COMPUTE_PREPARE, COMPUTE_DONE};
    void renewDependencyMask();
    static void shadowComputeOneRData(DvmhDistribSpace *dspace, const ShdWidth dspaceShdWidths[], ShadowComputeStage stage, DvmhData *data,
            DvmhRegionData *rdata);
    void handleShadowCompute(ShadowComputeStage stage);
    bool mapPartOnDevice(const LoopBounds part[], int device, LoopBounds res[]) const;
    bool dividePortion(LoopBounds partialBounds[], int totalCount, int curIndex, LoopBounds curBounds[]) const;
protected:
    Phase phase;
};

class HandlerOptimizationParams {
public:
    virtual int getSlotsToUse() const = 0;
    virtual int getSerializedIndex(int paramNumber) const = 0;
public:
    explicit HandlerOptimizationParams(int deviceNumber): deviceNum(deviceNumber) {}
    virtual HandlerOptimizationParams *clone() const = 0;
public:
    virtual HandlerOptimizationParams *genWithDefaults() const = 0;
    virtual void setFromLoop(DvmhLoop *loop) = 0;
    virtual std::vector<int> genParamsRanges() = 0;
    virtual void fixSerializedIndex(HandlerOptimizationParams *p, int paramNumber) const = 0;
    virtual void applySerialized(HandlerOptimizationParams *p, const std::vector<int> values) const = 0;
public:
    virtual ~HandlerOptimizationParams() {} // just in case
protected:
    int deviceNum;
};

class HostHandlerOptimizationParams: public HandlerOptimizationParams {
public:
    virtual int getSlotsToUse() const { return threads; }
    virtual int getSerializedIndex(int paramNumber) const { return serializedIndex; }
    void setThreads(int aThreads, int expl) { threads = aThreads; explFlag = expl; }
    int getThreads(int *pExpl = 0) const { if (pExpl) *pExpl = explFlag; return threads; }
public:
    explicit HostHandlerOptimizationParams(int dev): HandlerOptimizationParams(dev), threads(1), serializedIndex(-1), explFlag(0), isParallel(false) {}
    virtual HostHandlerOptimizationParams *clone() const {
        return new HostHandlerOptimizationParams(*this);
    }
public:
    virtual HostHandlerOptimizationParams *genWithDefaults() const;
    virtual void setFromLoop(DvmhLoop *loop) {
        isParallel = loop->rank - (loop->dependencyMask > 0) > 0;
    }
    virtual std::vector<int> genParamsRanges();
    virtual void fixSerializedIndex(HandlerOptimizationParams *p, int paramNumber) const;
    virtual void applySerialized(HandlerOptimizationParams *p, const std::vector<int> values) const;
protected:
    int threads;
    int serializedIndex;
    std::vector<int> serialization;
    int explFlag;
    bool isParallel;
};

class CudaHandlerOptimizationParams: public HandlerOptimizationParams {
private:
    struct MyCudaBlock {
        int x, y, z;
        MyCudaBlock(int block[3]): x(block[0]), y(block[1]), z(block[2]) {}
    };
public:
    virtual int getSlotsToUse() const { return 1; }
    virtual int getSerializedIndex(int paramNumber) const { return serializedIndex; }
    void setBlock(const int aBlock[3], int expl) { roundBlock(aBlock, block); explFlag = expl; }
    int getBlock(int aBlock[3]) const { typedMemcpy(aBlock, block, 3); return explFlag; }
    void setSharedCount(int aSharedCount) { sharedCount = aSharedCount; }
    void setRegsCount(int aRegCount) { regCount = aRegCount; }
public:
    explicit CudaHandlerOptimizationParams(int dev): HandlerOptimizationParams(dev), serializedIndex(-1), explFlag(0), regCount(1), sharedCount(0), maxRank(0) {
        block[0] = block[1] = block[2] = 1;
        device = (CudaDevice *)devices[dev];
    }
    virtual CudaHandlerOptimizationParams *clone() const {
        return new CudaHandlerOptimizationParams(*this);
    }
public:
    virtual CudaHandlerOptimizationParams *genWithDefaults() const;
    virtual void setFromLoop(DvmhLoop *loop) {
        maxRank = std::min(3, loop->rank - (loop->dependencyMask > 0));
        if (loop->userCudaBlockFlag)
            setBlock(loop->cudaBlock, 1);
    }
    virtual std::vector<int> genParamsRanges();
    virtual void fixSerializedIndex(HandlerOptimizationParams *p, int paramNumber) const;
    virtual void applySerialized(HandlerOptimizationParams *p, const std::vector<int> values) const;
protected:
    void roundBlock(const int inBlock[3], int outBlock[3]) const;
protected:
    int block[3];
    int serializedIndex;
    std::vector<MyCudaBlock> serialization;
    int explFlag; // Flag of explicitly given block - that is by user(1) or automatic mechanisms(2), but not default block.
    int regCount;
    int sharedCount;
    int maxRank;
    CudaDevice *device;
};

struct ParamInterval {
    int begin;
    int end;
    double part;
    bool operator<(const ParamInterval &p) const { return part < p.part; }
};

class DvmhSpecLoop;

class DvmhLoopPortion {
public:
    DvmhLoop *getLoop() const { return loop; }
    bool getDoReduction() const { return doReduction; }
    const LoopBounds *getLoopBounds() const { return loopBounds; }
    LoopBounds getLoopBounds(int i) const { assert(i >= 1 && i <= loop->rank); return loopBounds[i - 1]; }
    int getDeviceNum() const { return deviceNum; }
    int getHandlerNum() const { return handlerNum; }
    DvmhLoopHandler *getHandler() const { return handler; }
    HandlerOptimizationParams *getOptParams() const { return optimizationParams; }
    int getSlotsToUse() const { return slotsToUse; }
    int getTargetSlot() const { return targetSlot; }
    void setTargetSlot(int val) { targetSlot = val; }
    double getCalcTime() const { return calculationTime; }
    void setCalcTime(double val) { calculationTime = val; }
public:
    explicit DvmhLoopPortion(DvmhLoop *aLoop, LoopBounds myBounds[], bool doRed, int devNum, int handlNum, HandlerOptimizationParams *optParams = 0,
            int slotCount = -1);
public:
    void perform(DvmhPerformer *performer);
    void updatePerformanceInfo() const;
public:
    ~DvmhLoopPortion();
protected:
    // Static info
    DvmhLoop *loop;
    LoopBounds *loopBounds; //Array of LoopBounds (length=loop->dimension)
    bool doReduction;
    int deviceNum;
    int handlerNum;
    DvmhLoopHandler *handler;
    HandlerOptimizationParams *optimizationParams;
    int slotsToUse;
    int targetSlot;

    // Execution-time stuff
    DvmhSpecLoop *loopRef;
    double calculationTime;
};

class ParamDesc {
// Rework it. Pick not seen (and seen, maybe) values in more intelligent order.
public:
    bool hasValues() const { return range > 0; }
    int getBestVal() const { return bestVal; }
    bool hasNotSeen() const { return !notSeen.empty(); }
    double getRes(int val) const { assert(inRange(val)); return times[val]; }
    DvmType getOrder(int val) const { assert(inRange(val)); return orders[val]; }
    double getNotSeenPart() const { assert(!notSeen.empty()); return notSeen.top().part; }
    DvmType getOldestOrder() const { assert(!seen.empty()); return seen.begin()->first; }
    bool isSeen(int val) const { return inRange(val) ? orders[val] > 0 : false; }
public:
    explicit ParamDesc(int aRange);
public:
    int pickNotSeen();
    int pickSeen();
    void update(int val, double res, DvmType order);
protected:
    bool inRange(int val) const { return val >= 0 && val < range; }
protected:
    int range;
    std::map<DvmType, int> seen;
    std::priority_queue<ParamInterval> notSeen;
    std::vector<double> times;
    std::vector<DvmType> orders;
    int bestVal;
};

class LoopHandlerPerformanceInfo {
private:
    typedef std::map<double, std::pair<double, DvmType> > TableFunc; // log(iterationCount) => time coefficient (relative to referenceTime) + order
public:
    void setRootParams(HandlerOptimizationParams *aRootParams) { assert(rootParams == 0); rootParams = aRootParams; }
    HandlerOptimizationParams *getRootParams() const { return rootParams; }
    void setDeviceNumber(int aDeviceNumber) { deviceNumber = aDeviceNumber; }
    void setAccumulation(int count = 1);
public:
    LoopHandlerPerformanceInfo();
    LoopHandlerPerformanceInfo(const LoopHandlerPerformanceInfo &info);
    LoopHandlerPerformanceInfo &operator=(const LoopHandlerPerformanceInfo &info);
public:
    std::map<double, double> genPerfTableFunc() const;
    void update(double iterCount, double perf, HandlerOptimizationParams *params);
    std::pair<double, HandlerOptimizationParams *> getBestParams(double iterCount) const;
    HandlerOptimizationParams *pickOptimizationParams(double iterCount);
public:
    ~LoopHandlerPerformanceInfo();
protected:
    void initParams();
    double getCoef(double logIterCount) const;
    DvmType getOrder(double logIterCount);
protected:
    int deviceNumber;

    double accumIters;
    double accumTime;
    int accumRest;

    HandlerOptimizationParams *rootParams;
    HandlerOptimizationParams *bestParams;
    double bestCoef;
    std::vector<ParamDesc> params;
    DvmType execCount;
    double referenceTime;
    TableFunc tableFunc;
    DvmhSpinLock lock;
    static const int maxTblFuncSize = 16;
};

class DvmhLoopPersistentInfo {
public:
    LoopHandlerPerformanceInfo *getLoopHandlerPerformanceInfo(int deviceNum, int handlerNum) { return &performanceInfos[deviceNum][handlerNum]; }
    int getHandlersCount(int deviceNum) const { return performanceInfos[deviceNum].size(); }
    void setVarId(int aVarId) { mappingVarId = aVarId; }
    int getVarId() const { return mappingVarId; }
    void incrementExecCount() { execCount++; }
    DvmType getExecCount() const { return execCount; }
public:
    explicit DvmhLoopPersistentInfo(const SourcePosition &sp, DvmhRegionPersistentInfo *aRegionInfo);
public:
    void fix(DvmhLoop *loop);
    std::pair<int, HandlerOptimizationParams *> getBestParams(int dev, double part, double *pTime = 0) const;
    double estimateTime(DvmhRegion *region, int quantCount = 1) const;
    std::pair<int, HandlerOptimizationParams *> pickOptimizationParams(int dev, double iterCount);
protected:
    SourcePosition sourcePos;

    DvmhRegionPersistentInfo *regionInfo;
    int mappingVarId;
    std::vector<std::vector<LoopHandlerPerformanceInfo> > performanceInfos; //Array of array of LoopHandlerPerformanceInfo (length1=deviceCount;length2=count of handlers suitable for that device)
    DvmType execCount;
    double typicalSpace;
};

class DvmhSpecLoop: public DvmhObject {
public:
    DvmhLoopPortion *getPortion() const { return portion; }
    DvmhLoop *getLoop() const { return portion->getLoop(); }
    int getDeviceNum() const { return portion->getDeviceNum(); }
public:
    explicit DvmhSpecLoop(DvmhLoopPortion *aPortion): portion(aPortion) {
        assert(portion);
    }
public:
    bool hasElement(DvmhData *data, const DvmType indexArray[]) const;
    void fillLocalPart(DvmhData *data, DvmType part[]) const;
    virtual void prepareHandlerExec() {}
    virtual void syncAfterHandlerExec() {}
public:
    virtual ~DvmhSpecLoop() {}
protected:
    DvmhLoopPortion *portion;
};

class DvmhLoopCuda: public DvmhSpecLoop {
public:
    int counter; // Временное решение для управления ходом исполнения цикла - 0 - только начался, 1 - в обработке, 2 - кончился
    UDvmType restBlocks; // Количество пока не обработанных блоков
    UDvmType latestBlocks; // Количество посланных в последний раз на обработку блоков
    UDvmType overallBlocks; // Общее количество блоков(на весь входной цикл)
    HybridVector<DvmhReductionCuda *, 10> reductions; // Редукции, зарегистрированные для доделывания в РТС
    CudaDevice *cudaDev;
    cudaStream_t cudaStream;
    UDvmType dynSharedPerBlock;
    HybridVector<void *, 10> toFree; // Array of (void *)
    bool kernelsUsePGI;
public:
    explicit DvmhLoopCuda(DvmhLoopPortion *aPortion, DvmhPerformer *performer): DvmhSpecLoop(aPortion), counter(-1), cudaStream(0), kernelsUsePGI(false) {
        cudaDev = ((CudaDevice *)devices[getDeviceNum()]);
    }
public:
    void addToToFree(void *addr);
    template <typename IndexT>
    IndexT *getLocalPart(DvmhData *data);
    void addReduction(DvmhReductionCuda *red) {
        reductions.push_back(red);
    }
    void pickBlock(int sharedPerThread, int regsPerThread, int block[]);
    void finishRed(DvmhReduction *red);
    DvmhReductionCuda *getCudaRed(DvmhReduction *red, int *pRedInd = 0);
    void finishAllReds();
    virtual void prepareHandlerExec() {
        cudaDev->setAsCurrent();
    }
    virtual void syncAfterHandlerExec();
public:
    virtual ~DvmhLoopCuda();
};

extern std::map<SourcePosition, DvmhLoopPersistentInfo *> loopDict;
extern THREAD_LOCAL bool isInParloop;

}
