#pragma once

#include <cassert>
#include <set>
#include <vector>

#include "util.h"

namespace libdvmh {

class DvmhBuffer;
class DvmhPieces;
class DvmhEvent;
class AggregateEvent;
class DvmhCommunicator;
class MultiprocessorSystem;
class DvmhDistribRule;
class DvmhAlignRule;
class DvmhRegionPersistentInfo;
class DvmhRegion;

class DvmhRepresentative: private Uncopyable {
public:
    const DvmhBuffer *getBuffer() const { return buffer; }
    DvmhBuffer *getBuffer() { return buffer; }
    DvmhPieces *getActualState() const { return actualState; }
    void setCleanTransformState(bool value = true) { cleanTransformState = value; }
    bool ownsBuffer() const { return ownBuffer; }
public:
    explicit DvmhRepresentative(DvmhBuffer *aBuffer, bool owning = true);
public:
    bool ownsMemory() const;
    void doTransform(const int newAxisPerm[], int perDiagonalState);
    void undoDiagonal();
public:
    ~DvmhRepresentative();
protected:
    DvmhBuffer *buffer;
    DvmhPieces *actualState;
    bool cleanTransformState;
    bool ownBuffer;
};

class DvmhRegVar {
public:
    DvmhRegVar(): region(0), varId(0) {}
    explicit DvmhRegVar(DvmhRegionPersistentInfo *aRegion, int aVarId): region(aRegion), varId(aVarId) {}
public:
    int compareTo(const DvmhRegVar &v) const {
        return (*this < v ? -1 : (*this == v ? 0 : 1));
    }
    bool operator<(const DvmhRegVar &v) const {
        return (region < v.region) || (region == v.region && varId < v.varId);
    }
    bool operator==(const DvmhRegVar &v) const {
        return region == v.region && varId == v.varId;
    }
protected:
    // Global
    DvmhRegionPersistentInfo *region; // NULL for get/set actual
    int varId;
};

class DvmhDataState {
public:
    explicit DvmhDataState(const DvmhRegVar &aProducer = DvmhRegVar()): producer(aProducer) {}
    explicit DvmhDataState(DvmhRegionPersistentInfo *region, int varId): producer(region, varId) {}
public:
    bool operator<(const DvmhDataState &state) const;
    bool has(const DvmhRegVar &v) const {
        return producer == v || readers.find(v) != readers.end();
    }
    void addReader(const DvmhRegVar &v) {
        readers.insert(v);
    }
protected:
    // Global
    DvmhRegVar producer;
    std::set<DvmhRegVar> readers;
};

class DvmhData;

class ReferenceDesc {
public:
    DvmhData *data;
    int axis;
public:
    ReferenceDesc(): data(0), axis(-1) {}
    explicit ReferenceDesc(DvmhData *data, int axis): data(data), axis(axis) {}
public:
    bool isValid() const { return data && axis > 0; }
    bool operator!=(const ReferenceDesc &other) const { return data != other.data || axis != other.axis; }
    void clear() {
        data = 0;
        axis = -1;
    }
};

class LocalizationInfo {
public:
    ReferenceDesc target;
    std::vector<ReferenceDesc> references;
public:
    void removeFromReferences(const ReferenceDesc &desc);
};

struct HeaderDesc {
    DvmType *ptr;
    bool own;
    bool freeBase;
};

class DvmhData: public DvmhObject {
public:
    enum DataType {dtUnknown = -1, dtChar = 0, dtInt = 1, dtLong = 2, dtFloat = 3, dtDouble = 4, dtFloatComplex = 5, dtDoubleComplex = 6, dtLogical = 7,
            dtLongLong = 8, dtUChar = 9, dtUInt = 10, dtULong = 11, dtULongLong = 12, dtShort = 13, dtUShort = 14, dtPointer = 15, DATA_TYPES};
    enum TypeType {ttUnknown = -1, ttInteger = 0, ttFloating = 1, ttComplex = 2, TYPE_TYPES};
    inline static TypeType getTypeType(DataType rt) {
        switch (rt) {
            case dtUnknown: return ttUnknown;
            case dtChar: return ttInteger;
            case dtUChar: return ttInteger;
            case dtShort: return ttInteger;
            case dtUShort: return ttInteger;
            case dtInt: return ttInteger;
            case dtUInt: return ttInteger;
            case dtLong: return ttInteger;
            case dtULong: return ttInteger;
            case dtLongLong: return ttInteger;
            case dtULongLong: return ttInteger;
            case dtFloat: return ttFloating;
            case dtDouble: return ttFloating;
            case dtFloatComplex: return ttComplex;
            case dtDoubleComplex: return ttComplex;
            case dtLogical: return ttInteger;
            case dtPointer: return ttUnknown;
            default: assert(false);
        }
        return ttUnknown;
    }
    inline static UDvmType getTypeSize(DataType rt) {
        switch (rt) {
            case dtUnknown: return 0;
            case dtChar: return sizeof(char);
            case dtUChar: return sizeof(char);
            case dtShort: return sizeof(short);
            case dtUShort: return sizeof(short);
            case dtInt: return sizeof(int);
            case dtUInt: return sizeof(int);
            case dtLong: return sizeof(long);
            case dtULong: return sizeof(long);
            case dtLongLong: return sizeof(long long);
            case dtULongLong: return sizeof(long long);
            case dtFloat: return sizeof(float);
            case dtDouble: return sizeof(double);
            case dtFloatComplex: return 2 * sizeof(float);
            case dtDoubleComplex: return 2 * sizeof(double);
            case dtLogical: return sizeof(int);
            case dtPointer: return sizeof(void *);
            default: assert(false);
        }
        return 0;
    }
public:
    UDvmType getTypeSize() const { return typeSize; }
    TypeType getTypeType() const { return typeType; }
    DataType getDataType() const { return dataType; }
    int getRank() const { return rank; }
    const Interval *getSpace() const { return space; }
    const Interval &getAxisSpace(int i) const { assert(i >= 1 && i <= rank); return space[i - 1]; }
    bool isIncomplete() const { return rank >= 1 && space[0].empty(); }
    const ShdWidth &getShdWidth(int i) const { assert(i >= 1 && i <= rank); return shdWidths[i - 1]; }
    const std::set<std::string> *getIndirectShadows() const { return indirectShadows; }
    bool isAligned() const { return alignRule != 0 || hasLocal(); }
    bool isDistributed() const { return alignRule != 0; }
    const DvmhAlignRule *getAlignRule() const { return alignRule; }
    bool hasOwnDistribSpace() const { return ownDspace; }
    DvmhDataState &getCurState() { return curState; }
    bool hasLocal() const { return hasLocalFlag; }
    const Interval *getLocalPart() const { return localPart; }
    const Interval &getAxisLocalPart(int i) const { assert(i >= 1 && i <= rank); return localPart[i - 1]; }
    const Interval *getLocalPlusShadow() const { return localPlusShadow; }
public:
    explicit DvmhData(UDvmType aTypeSize, TypeType aTypeType, int aRank, const Interval aSpace[], const ShdWidth shadows[] = 0);
    explicit DvmhData(DataType aDataType, int aRank, const Interval aSpace[], const ShdWidth shadows[] = 0);
    static DvmhData *fromRegularArray(DataType dt, void *regArr, DvmType len, DvmType baseIdx = 0);
public:
    void setTypeType(TypeType tt);
    DvmhRepresentative *getRepr(int dev);
    DvmhBuffer *getBuffer(int dev);
    void setHostPortion(void *hostAddr, const Interval havePortion[]);
    void addHeader(DvmType aHeader[], const void *base, bool owning = false);
    bool hasHeader(DvmType aHeader[]) const;
    bool removeHeader(DvmType aHeader[]);
    DvmType *getAnyHeader(bool nullIfNone = false) const;
    void createHeader();
    void initActual(int dev);
    void initActualShadow();
    DvmhRepresentative *createNewRepr(int dev, const Interval havePortion[]);
    void deleteRepr(int device);
    void extendBlock(const Interval block[], Interval result[]) const {
        extendBlock(rank, block, shdWidths, localPlusShadow, result);
    }
    static void extendBlock(int rank, const Interval block[], const ShdWidth shadow[], const Interval bounds[], Interval result[]);
    void expandIncomplete(DvmType newHigh);
    void getActualBase(int dev, DvmhPieces *p, const Interval curLocalPart[], bool addToActual);
    void getActualBaseOne(int dev, const Interval realBlock[], const Interval curLocalPart[], bool addToActual);
    void getActual(const Interval indexes[], bool addToActual);
    void getActualEdges(const Interval aLocalPart[], const ShdWidth shdWidths[], bool addToActual);
    void getActualIndirectEdges(int axis, const std::string &shadowName, bool addToActual);
    void getActualShadow(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[], bool addToActual);
    void clearActual(DvmhPieces *p, int exceptDev = -1);
    void clearActualOne(const Interval piece[], int exceptDev = -1);
    void clearActualShadow(const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[], int exceptDev = -1);
    void performSetActual(int dev, DvmhPieces *p);
    void performSetActualOne(int dev, const Interval realBlock[]);
    void setActual(DvmhPieces *p);
    void setActual(const Interval indexes[]);
    void setActualShadow(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[]);
    void setActualIndirectShadow(int dev, int axis, const std::string &shadowName);
    void shadowComputed(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[]);
    void updateShadowProfile(bool cornerFlag, ShdWidth curShdWidths[]);
    void updateIndirectShadowProfile(int axis, const std::string &shadowName);
    UDvmType getTotalElemCount() const;
    UDvmType getLocalElemCount() const;
    void realign(DvmhAlignRule *newAlignRule, bool ownDistribSpace = false, bool newValueFlag = false);
    void redistribute(const DvmhDistribRule *newDistribRule);
    bool hasElement(const DvmType indexes[]) const;
    bool convertGlobalToLocal2(DvmType indexes[], bool onlyFromLocalPart = false) const;
    bool convertLocal2ToGlobal(DvmType indexes[], bool onlyFromLocalPart = false) const;
    bool hasIndirectShadow(int axis, const std::string &shadowName) const;
    void includeIndirectShadow(int axis, const std::string &shadowName);
    void localizeAsReferenceFor(DvmhData *targetData, int targetAxis);
    void unlocalize() { unlocalizeValues(); }
    DvmhEvent *enqueueWriteAccess(DvmhEvent *accessEnd, bool owning = false);
    DvmhEvent *enqueueReadAccess(DvmhEvent *accessEnd, bool owning = false);
    void syncWriteAccesses();
    void syncAllAccesses();
    bool mpsCanRead(const MultiprocessorSystem *mps) const;
    bool mpsCanWrite(const MultiprocessorSystem *mps) const;
    bool fillLocalPart(int proc, Interval res[]) const;
public:
    ~DvmhData();
protected:
    void finishInitialization(int aRank, const Interval aSpace[], const ShdWidth shadows[]);
    void initShadowProfile();
    void recalcLocalPlusShadow();
    DvmhPieces *applyShadowProfile(DvmhPieces *p, const Interval curLocalPart[]) const;
    void performCopy(int dev1, int dev2, const Interval aCutting[]) const;
    void performGetActual(int dev, DvmhPieces *p, bool addToActual);
    void redistributeCommon(DvmhAlignRule *newAlignRule, bool newValueFlag = false);
    void changeShadowWidth(int axis, ShdWidth newWidth);
    void changeReprSize(int dev, const Interval havePortion[]);
    void updateHeaders();
    void unlocalizeValues();
    void localizeValues(ReferenceDesc target);
    struct ShadowComputedPerformer;
    struct ProfileUpdater;
protected:
    // Global
    // Static info
    UDvmType typeSize;
    TypeType typeType;
    DataType dataType;
    int rank;
    Interval *space; //Array of Intervals (length=rank)
    ShdWidth *shdWidths; //Array of ShdWidths (length=rank)
    std::set<std::string> *indirectShadows; // Array of std::set<std::string> (length=rank), can be NULL
    // Dynamic info
    DvmhAlignRule *alignRule;
    bool ownDspace;
    DvmhDataState curState; // Current state in terms of which region is producer and which regions are readers
    // Local
    bool hasLocalFlag;
    Interval *localPart; //Array of Intervals (length=rank)
    Interval *localPlusShadow; //Array of Intervals (length=rank)
    DvmType maxOrder;
    DvmhPieces *shadowProfile; // Shadow profile. Dimension is rank. Space is -shdWidths[i][0]..shdWidths[i][1]
    HybridVector<HeaderDesc, 4> headers;
    LocalizationInfo localizationInfo;
    DvmhEvent *latestWriterEnd;
    AggregateEvent *latestReadersEnd;
    // By-device dynamic info
    DvmhRepresentative **representatives; //Array of pointers to DvmhRepresentatives (length=devicesCount)
};

class DvmhShadowData {
public:
    DvmhData *data;
    bool isIndirect;

    // For block shadows
    ShdWidth *shdWidths;
    bool cornerFlag;

    // For indirect shadows
    int indirectAxis;
    std::string indirectName;
public:
    DvmhShadowData(): data(0), isIndirect(false), shdWidths(0), cornerFlag(false), indirectAxis(-1) {}
    DvmhShadowData(const DvmhShadowData &sdata);
public:
    DvmhShadowData &operator=(const DvmhShadowData &sdata);
    bool empty() const;
public:
    ~DvmhShadowData() {
        delete[] shdWidths;
    }
};

class DvmhShadow {
public:
    int dataCount() const { return datas.size(); }
    bool empty() const { return dataCount() == 0; }
    const DvmhShadowData &getData(int i) const { assert(i >= 0 && i < dataCount()); return datas[i]; }
public:
    void add(const DvmhShadowData &aSdata) {
        datas.push_back(aSdata);
    }
    void renew(DvmhRegion *currentRegion, bool doComm) const;
    void append(const DvmhShadow &others) {
        for (int i = 0; i < (int)others.datas.size(); i++)
            add(others.datas[i]);
    }
protected:
    std::vector<DvmhShadowData> datas;
};

void dvmhCopyArrayArray(DvmhData *src, const Interval srcBlock[], const DvmType srcSteps[], DvmhData *dst, const Interval dstBlock[], const DvmType dstSteps[],
        const int dstAxisToSrc[]);
void dvmhCopyArrayWhole(DvmhData *src, DvmhData *dst);

}
