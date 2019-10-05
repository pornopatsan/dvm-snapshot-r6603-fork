#pragma once

#include "util.h"

namespace libdvmh {

struct DvmhDiagonalInfo {
    int x_axis;
    DvmType x_first;
    DvmType x_length;
    int y_axis;
    DvmType y_first;
    DvmType y_length;
    int slashFlag;
};

class DvmhBuffer: private Uncopyable {
public:
    int getDeviceNum() const { return deviceNum; }
    void *getDeviceAddr() const { return deviceAddr; }
    int getRank() const { return rank; }
    UDvmType getTypeSize() const { return typeSize; }
    const Interval *getHavePortion() const { return ~havePortion; }
    bool isDiagonalized() const { return diagonalizedState != 0; }
    int getDiagonalizedState() const { return diagonalizedState; }
    const int *getAxisPerm() const { return ~axisPerm; }
    UDvmType getSize() const { assert(!isIncomplete()); return totalSize; }
    bool ownsMemory() const { return ownMemory; }
    bool isIncomplete() const { return rank >= 1 && havePortion[0].empty(); }
    void setIndexAdjustment(int i, DvmType adjustment) { assert(i >= 1 && i <= rank); startIndexAdjustments[i - 1] = adjustment; }
public:
    explicit DvmhBuffer(int aRank, UDvmType aTypeSize, int devNum, const Interval aHavePortion[], void *devAddr = 0, bool aOwnMemory = false);
    explicit DvmhBuffer(const DvmhBuffer& origBuf, const DvmType indexOffset[]); // Reinterpret the same buffer changing index space
public:
    bool pageLock();
    bool isTransformed() const;
    void doTransform(const int newAxisPerm[], int perDiagonalState);
    void setTransformState(const int newAxisPerm[], int perDiagonalState);
    void undoDiagonal();
    void fillHeader(const void *base, DvmType header[], bool adjustForExternalUse, DvmhDiagonalInfo *diagonalInfo = 0, bool zeroDiagonalized = true) const;
    void fillHeader(DvmType header[], bool adjustForExternalUse) const { fillHeader(deviceAddr, header, adjustForExternalUse); }
    void *getNaturalBase(bool adjustForExternalUse) const;
    bool hasElement(DvmType index) const { assert(rank == 1); return hasElement(&index); }
    bool hasElement(const DvmType indexes[]) const;
    void *getElemAddr(DvmType index) const { assert(rank == 1); return getElemAddr(&index); }
    void *getElemAddr(const DvmType indexes[]) const;
    template <typename T>
    T &getElement(DvmType index) const {
        assert(typeSize == sizeof(T));
        return *(T *)getElemAddr(index);
    }
    template <typename T>
    T &getElement(const DvmType indexes[]) const {
        assert(typeSize == sizeof(T));
        return *(T *)getElemAddr(indexes);
    }
    int getMinIndexTypeSize(const Interval block[], bool adjustForExternalUse) const;
    DvmhBuffer *undiagCutting(const Interval cutting[]) const;
    void replaceUndiagedCutting(DvmhBuffer *part);
    void copyTo(DvmhBuffer *to, const Interval piece[] = 0) const;
    bool canDumpInplace(const Interval piece[]) const;
    DvmhBuffer *dumpPiece(const Interval piece[], bool allowInplace = false) const;
    bool overlaps(const DvmhBuffer *other) const;
public:
    ~DvmhBuffer();
protected:
    bool canonicalizeTransformState(int perm[], int &diag) const;
    bool canonicalizeTransformState() { return canonicalizeTransformState(~axisPerm, diagonalizedState); }
    bool isBlockConsecutive(const Interval piece[]) const;
    void applyDiagonal(int newDiagonalState);
    void applyPermDelta(int permDelta[]);
    UDvmType undiagCuttingInternal(void *res, Interval resPortion[], bool backFlag);
protected:
    int deviceNum;
    void *deviceAddr;
    int rank;
    UDvmType typeSize;
    HybridVector<Interval, 10> havePortion; //Array of Intervals (length=rank)
    HybridVector<int, 10> axisPerm; //Array of Indexes of original array (length=rank) (transformed => original)
    HybridVector<DvmType, 10> startIndexAdjustments; //Array of offsets for index space when filling header
    int diagonalizedState; //State of per-diagonal transformation of last 2 dimensions (in case of permutation last 2 dimensions is meant last 2 dimensions in current permutated state). 0 means no per-diagonal transformation. 1 means per-diagonal transformation with slashFlag=0. 2 means per-diagonal transformation with slashFlag=1.
    UDvmType totalSize;
    bool ownMemory;
    bool pageLocked;
};

}
