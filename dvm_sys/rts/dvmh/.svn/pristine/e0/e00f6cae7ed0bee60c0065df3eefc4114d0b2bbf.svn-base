#pragma once

#include <cassert>
#include <set>
#include <vector>

#include "dvmh_pieces.h"
#include "util.h"

namespace libdvmh {

class DvmhCommunicator;
class MultiprocessorSystem;
class DvmhBuffer;
class DvmhData;

class Intervals {
public:
    int getIntervalCount() const { return intervals.size(); }
    const Interval &getInterval(int i) const { return intervals[i]; }
    bool empty() const { return intervals.empty(); }
    bool isInterval() const { return intervals.size() <= 1; }
    UDvmType getElementCount() const { return elementCount; }
public:
    Intervals() { elementCount = 0; }
    // Note, that it is left implicit on purpose
    Intervals(const Interval &inter) { elementCount = 0; append(inter); }
public:
    void clear() { intervals.clear(); elementCount = 0; }
    void append(const Interval &inter);
    void unite(const Interval &inter);
    void uniteElements(const DvmType elements[], UDvmType size, DvmType multiplier = 1, DvmType summand = 0, bool shrinkSpace = false);
    void intersect(const Interval &inter);
    Interval toInterval() const {
        assert(intervals.size() <= 1);
        return empty() ? Interval::createEmpty() : intervals[0];
    }
    DvmhPieces *toPieces() const;
    Interval getBoundInterval() const;
protected:
    HybridVector<Interval, 10> intervals;
    UDvmType elementCount;
};

class ReplicatedAxisDistribRule;
class DistributedAxisDistribRule;
class BlockAxisDistribRule;
class IndirectAxisDistribRule;

class DvmhAxisDistribRule: private Uncopyable {
public:
    const MultiprocessorSystem *getMPS() const { return mps; }
    int getMPSAxis() const { assert(mpsAxis >= 1); return mpsAxis; }
    const Interval &getSpaceDim() const { return spaceDim; }
    const Interval &getLocalElems() const { return localElems; }
    DvmType getMaxSubparts() const { assert(maxSubparts >= 0); return maxSubparts; }
    bool isReplicated() const { return distribType == dtReplicated; }
    bool isDistributed() const { return !isReplicated(); }
    bool isBlockDistributed() const { return distribType >= dtBlock && distribType <= dtMultBlock; }
    bool isIndirect() const { return distribType >= dtIndirect && distribType <= dtDerived; }
    DistributedAxisDistribRule *asDistributed() { return (DistributedAxisDistribRule *)(isDistributed() ? this : 0); }
    const DistributedAxisDistribRule *asDistributed() const { return (const DistributedAxisDistribRule *)(isDistributed() ? this : 0); }
    BlockAxisDistribRule *asBlockDistributed() { return (BlockAxisDistribRule *)(isBlockDistributed() ? this : 0); }
    const BlockAxisDistribRule *asBlockDistributed() const { return (const BlockAxisDistribRule *)(isBlockDistributed() ? this : 0); }
    IndirectAxisDistribRule *asIndirect() { return (IndirectAxisDistribRule *)(isIndirect() ? this : 0); }
    const IndirectAxisDistribRule *asIndirect() const { return (const IndirectAxisDistribRule *)(isIndirect() ? this : 0); }
public:
    static ReplicatedAxisDistribRule *createReplicated(MultiprocessorSystem *mps, const Interval &spaceDim);
    static BlockAxisDistribRule *createBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim);
    static BlockAxisDistribRule *createWeightBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmhData *weights);
    static BlockAxisDistribRule *createGenBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmhData *givenGbl);
    static BlockAxisDistribRule *createMultBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, UDvmType multQuant);
    static IndirectAxisDistribRule *createIndirect(MultiprocessorSystem *mps, int mpsAxis, DvmhData *givenMap);
    static IndirectAxisDistribRule *createDerived(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmType derivedBuf[],
            UDvmType derivedCount);
public:
    virtual int genSubparts(const double distribPoints[], Interval parts[], int count) const = 0;
    virtual int getProcIndex(DvmType spaceIndex) const = 0;
public:
    virtual ~DvmhAxisDistribRule() {}
protected:
    enum DistribType {dtReplicated, dtBlock, dtWgtBlock, dtGenBlock, dtMultBlock, dtIndirect, dtDerived};
    explicit DvmhAxisDistribRule(DistribType dt, MultiprocessorSystem *aMps, int aMpsAxis = -1): distribType(dt), mps(aMps), mpsAxis(aMpsAxis),
            maxSubparts(-1) {
        assert(mps);
        spaceDim = Interval::createEmpty();
        localElems = Interval::createEmpty();
    }
protected:
    // Global
    DistribType distribType;
    MultiprocessorSystem *mps;
    int mpsAxis;
    Interval spaceDim;
    // Local
    Interval localElems;
    DvmType maxSubparts;
};

class ReplicatedAxisDistribRule: public DvmhAxisDistribRule {
public:
    explicit ReplicatedAxisDistribRule(MultiprocessorSystem *aMps, const Interval &aSpaceDim): DvmhAxisDistribRule(dtReplicated, aMps) {
        spaceDim = aSpaceDim;
        localElems = spaceDim;
        maxSubparts = 0;
    }
public:
    virtual int genSubparts(const double distribPoints[], Interval parts[], int count) const {
        assert(count == 0);
        return 0;
    }
    virtual int getProcIndex(DvmType spaceIndex) const {
        assert(false);
        return 0;
    }
};

class DistributedAxisDistribRule: public DvmhAxisDistribRule {
public:
    Interval getLocalElems(int procIndex) const { return Interval::create(sumGenBlock[procIndex], sumGenBlock[procIndex + 1] - 1); }
    const Interval &getLocalElems() const { return DvmhAxisDistribRule::getLocalElems(); }
public:
    explicit DistributedAxisDistribRule(DistribType dt, MultiprocessorSystem *aMps, int aMpsAxis);
public:
    virtual int getProcIndex(DvmType spaceIndex) const;
public:
    ~DistributedAxisDistribRule() {
        delete[] sumGenBlock;
    }
protected:
    DvmType *sumGenBlock;
};

class BlockAxisDistribRule: public DistributedAxisDistribRule {
public:
    explicit BlockAxisDistribRule(DistribType dt, MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, UDvmType aMultQuant);
    explicit BlockAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, DvmhData *weights);
    explicit BlockAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, DvmhData *givenGbl, const Interval &aSpaceDim);
public:
    virtual int genSubparts(const double distribPoints[], Interval parts[], int count) const;
public:
    // XXX: These functions are only needed for reflecting LibDVM's distribution
    void changeMultQuant(UDvmType newMultQuant);
    void changeTypeToBlock();
protected:
    void finishInit();
protected:
    // Global
    UDvmType multQuant;
    // Local
    HybridVector<double, 1> wgtBlockPart;
    Interval haveWgtPart; // ZERO-based
};

class ExchangeMap: private Uncopyable {
public:
    explicit ExchangeMap(DvmhCommunicator *axisComm);
public:
    bool checkConsistency() const;
    void fillBestOrder();
    bool checkBestOrder() const;
    void freeze();
    void performExchange(void *buf, UDvmType indexOffset, UDvmType elemSize) const;
    void performExchange(DvmhBuffer *dataBuffer, int dataAxis, DvmType axisDataOffset, const Interval boundBlock[]) const;
public:
    ~ExchangeMap();
protected:
    int getNextPartnerAndDirection(int &position) const;
protected:
    DvmhCommunicator *comm;

    int sendProcCount; // Size of sendProcs
    int *sendProcs; // Contains the ordered set of processor indexes.
    UDvmType *sendStarts; // CSR-like. Size is (sendProcCount + 1).
    DvmType *sendIndices; // All in compact form (local notation)

    int recvProcCount; // Size of recvProcs
    int *recvProcs; // Contains the ordered set of processor indexes.
    UDvmType *recvStarts; // CSR-like. Size is (recvProcCount + 1).
    DvmType *recvIndices; // All in compact form (local notation of 2nd type)

    int *bestOrder; // An order, in which it is the best to perform the exchange. Contains negative and positive numbers. Negative means receiving, positive means sending. Absolute value is the ((position in sendStarts/recvStarts array) + 1). NULL means not filled. Size is (sendProcCount + recvProcCount).

    // Preallocated arrays
    UDvmType *sendSizes;
    char **sendBuffers;
    UDvmType *recvSizes;
    char **recvBuffers;
private:
    friend class IndirectAxisDistribRule;
    friend class IndirectShadow;
};

class IndirectShadow: private Uncopyable {
public:
    const std::string &getName() const { return name; }
    const Intervals &getOwnL1Indices() const { return ownL1Indices; }
    ExchangeMap &getExchangeMap() { return exchangeMap; }
public:
    explicit IndirectShadow(const std::string &aName, DvmhCommunicator *axisComm): name(aName), exchangeMap(axisComm) {}
public:
    void fillOwnL1Indices();
protected:
    std::string name;
    Intervals ownL1Indices;
    ExchangeMap exchangeMap;
};

class IndirectShadowBlock {
public:
    const Interval &getBlock() const { return block;}
    bool isIncludedIn(int shadowIndex) const { return (shadowMask & (1UL << shadowIndex)) != 0; }
    void includeIn(int shadowIndex) { shadowMask |= (1UL << shadowIndex); }
public:
    explicit IndirectShadowBlock(const Interval &aBlock): block(aBlock), shadowMask(0) {}
public:
    IndirectShadowBlock extractOnLeft(DvmType amount, int shadowIndex) {
        IndirectShadowBlock res(*this);
        res.block[1] = res.block[0] + amount - 1;
        res.includeIn(shadowIndex);
        block[0] += amount;
        return res;
    }
    IndirectShadowBlock extractOnRight(DvmType amount, int shadowIndex) {
        IndirectShadowBlock res(*this);
        res.block[0] = res.block[1] - amount + 1;
        res.includeIn(shadowIndex);
        block[1] -= amount;
        return res;
    }
protected:
    Interval block; // Local indexes (2nd type)
    unsigned long shadowMask; // Set of shadow edges it is included in
};

struct ShadowElementInfo {
    DvmType globalIndex;
    DvmType local2Index;
    DvmType local1Index;
    int ownerProcessorIndex;
public:
    bool operator<(const ShadowElementInfo &other) const { return globalIndex < other.globalIndex; }
    static bool orderByLocal2(const ShadowElementInfo &a, const ShadowElementInfo &b) { return a.local2Index < b.local2Index; }
public:
    static ShadowElementInfo wrap(DvmType index) {
        ShadowElementInfo res;
        res.globalIndex = index;
        res.local2Index = index;
        res.local1Index = index;
        res.ownerProcessorIndex = index;
        return res;
    }
};

class IndirectAxisDistribRule: public DistributedAxisDistribRule {
public:
    explicit IndirectAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, DvmhData *givenMap);
    explicit IndirectAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, DvmType derivedBuf[], UDvmType derivedCount);
public:
    bool fillPart(Intervals &res, const Interval &globalSpacePart, DvmType regInterval = 1) const;
    DvmType globalToLocalOwn(DvmType ind, bool *pLocalElem = 0) const;
    DvmType globalToLocal2(DvmType ind, bool *pLocalElem = 0, bool *pShadowElem = 0) const;
    DvmType convertLocal2ToGlobal(DvmType ind, bool *pLocalElem = 0, bool *pShadowElem = 0) const;
    virtual int genSubparts(const double distribPoints[], Interval parts[], int count) const;
    void addShadow(DvmType elemBuf[], UDvmType elemCount, const std::string &name);
    bool fillShadowL2Indexes(Intervals &res, const std::string &name) const;
    const Intervals &getShadowOwnL1Indexes(const std::string &name) const;
    DvmType *getLocal2ToGlobal(DvmType *pFirstIndex = 0) const;
    const ExchangeMap &getShadowExchangeMap(const std::string &name) const;
public:
    virtual ~IndirectAxisDistribRule();
protected:
    void createGlobalToLocal();
    void fillGenblock(const UDvmType fakeGenBlock[]);
    void createLocal2ToGlobal();
    void finishGlobalToLocal();
    void fillShadowLocal1Indexes();
    void fillExchangeMap(int shadowIndex);
    int findShadow(const std::string &name) const;
protected:
    // Global
    DvmhData *globalToLocal; // BLOCK-distributed, type DvmType
    // Local
    DvmType *local2ToGlobal; // Local part + shadow edges. Local part is ordered by global index. Shadow edges are not.
    ShdWidth shdWidth;
    ShadowElementInfo *shdElements; // Ordered by global index array of all the shadow elements (regardless of their positions in local indexing)
    std::vector<IndirectShadow *> shadows;
    std::vector<IndirectShadowBlock> shadowBlocks;
};

class DvmhDistribRule: private Uncopyable {
public:
    int getRank() const { return rank; }
    const MultiprocessorSystem *getMPS() const { return MPS; }
    DvmhAxisDistribRule *getAxisRule(int i) { assert(i >= 1 && i <= rank && axes[i - 1]); return axes[i - 1]; }
    const DvmhAxisDistribRule *getAxisRule(int i) const { assert(i >= 1 && i <= rank && axes[i - 1]); return axes[i - 1]; }
    bool isIndirect(int i) const { return getAxisRule(i)->isIndirect(); }
    int getMpsAxesUsed() const { assert(filledFlag); return mpsAxesUsed; }
    bool hasIndirect() const { assert(filledFlag); return hasIndirectFlag; }
    bool hasLocal() const { assert(filledFlag); return hasLocalFlag; }
    const Interval *getLocalPart() const { assert(filledFlag); return localPart; }
public:
    explicit DvmhDistribRule(int aRank, const MultiprocessorSystem *aMPS);
public:
    void setAxisRule(int axis, DvmhAxisDistribRule *rule);
    bool fillLocalPart(int proc, Interval res[]) const;
public:
    ~DvmhDistribRule();
protected:
    // Global
    int rank;
    DvmhAxisDistribRule **axes; //Array of pointers to DvmhAxisDistribRule (length=rank)
    const MultiprocessorSystem *MPS;
    int mpsAxesUsed;
    bool hasIndirectFlag;
    bool filledFlag;
    // Local
    bool hasLocalFlag;
    Interval *localPart;
};

class DvmhDistribSpace: public DvmhObject, private Uncopyable {
public:
    int getRank() const { return rank; }
    const Interval *getSpace() const { return space; }
    const Interval &getAxisSpace(int i) const { assert(i >= 1 && i <= rank); return space[i - 1]; }
    bool isDistributed() const { return distribRule != 0; }
    bool hasLocal() const { assert(isDistributed()); return distribRule->hasLocal(); }
    const Interval *getLocalPart() const { assert(isDistributed()); return distribRule->getLocalPart(); }
    const Interval &getAxisLocalPart(int i) const { assert(isDistributed() && i >= 1 && i <= rank); return getLocalPart()[i - 1]; }
    const DvmhDistribRule *getDistribRule() const { return distribRule; }
    DvmhAxisDistribRule *getAxisDistribRule(int i) { assert(distribRule); return distribRule->getAxisRule(i); }
    const DvmhAxisDistribRule *getAxisDistribRule(int i) const { assert(distribRule); return distribRule->getAxisRule(i); }
    int getRefCount() const { return refCount; }
    const std::set<DvmhData *> &getAlignedDatas() const { return alignedDatas; }
public:
    explicit DvmhDistribSpace(int aRank, const Interval aSpace[]);
public:
    void addRef() { refCount++; }
    int removeRef() { refCount--; assert(refCount >= 0); return refCount; }
    void addAlignedData(DvmhData *data) { assert(isDistributed()); alignedDatas.insert(data); }
    void removeAlignedData(DvmhData *data) { alignedDatas.erase(data); }
    bool isSuitable(const DvmhDistribRule *aDistribRule) const;
    void redistribute(DvmhDistribRule *aDistribRule);
public:
    ~DvmhDistribSpace();
protected:
    // Global
    // Static info
    int rank;
    Interval *space; //Array of Intervals (length=rank)
    // Dynamic info
    DvmhDistribRule *distribRule;
    std::set<DvmhData *> alignedDatas;
    // TODO: remove, maybe
    int refCount; //Counter of mapped arrays, loops and regions
};

class DvmhAxisAlignRule {
public:
    // Global
    int axisNumber; // -1 means replicate. 0 means mapping to constant. >=1 means linear.
    DvmType multiplier; // For replicate and linear. Linear: DistribSpace's index = Array's index * multiplier + summand. Replicate: step in DistribSpace's indices.
    DvmType summand; // For constant and linear. Linear: DistribSpace's index = Array's index * multiplier + summand. Constant: DistribSpace's index.
    Interval replicInterval; // For replicate. DistribSpace's indices, step is multiplier.
    mutable DvmType summandLocal; // For indirectly-distributed axes and linear rules. Is equal to summand if corresponding space sizes do not differ.
public:
    void setReplicated(DvmType aMultiplier, const Interval &aReplicInterval);
    void setConstant(DvmType aSummand);
    void setLinear(int aAxisNumber, DvmType aMultiplier, DvmType aSummand);
    void composite(const DvmhAxisAlignRule rules[]);
};

class DvmhAlignRule {
public:
    int getRank() const { return rank; }
    DvmhDistribSpace *getDspace() const { return dspace; }
    const DvmhAxisAlignRule *getAxisRule(int i) const { assert(i >= 1 && i <= dspace->getRank()); return &map[i - 1]; }
    int getDspaceAxis(int i) const { assert(i >= 1 && i <= rank); return dspaceAxis[i - 1]; }
    bool isIndirect(int i) const { assert(i >= 1 && i <= rank); return dspaceAxis[i - 1] > 0 && dspace->getAxisDistribRule(dspaceAxis[i - 1])->isIndirect(); }
    bool hasIndirect() const { return hasIndirectFlag; }
    const DvmhDistribRule *getDistribRule() const { return disRule; }
public:
    explicit DvmhAlignRule(int aRank, DvmhDistribSpace *aDspace, const DvmhAxisAlignRule axisRules[]);
    DvmhAlignRule(const DvmhAlignRule &other);
public:
    bool mapOnPart(const Interval dspacePart[], Interval res[], bool allInLocalIndexes, const DvmType intervals[] = 0) const;
    bool mapOnPart(const Intervals dspacePart[], Intervals res[], const Interval spacePart[], const DvmType intervals[] = 0) const;
    void setDistribRule(const DvmhDistribRule *newDisRule);
public:
    ~DvmhAlignRule();
protected:
    // Global
    int rank;
    bool hasIndirectFlag;
    DvmhDistribSpace *dspace;
    const DvmhDistribRule *disRule;
    DvmhAxisAlignRule *map; //Array of structures (length=dspace->rank)
    int *dspaceAxis;
};

}
