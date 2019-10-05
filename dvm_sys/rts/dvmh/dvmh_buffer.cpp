#include "dvmh_buffer.h"

#include "cuda_transform.h"
#include "dvmh_copying.h"
#include "dvmh_device.h"
#include "dvmh_log.h"
#include "dvmh_stat.h"

namespace libdvmh {

// DvmhBuffer

DvmhBuffer::DvmhBuffer(int aRank, UDvmType aTypeSize, int devNum, const Interval aHavePortion[], void *devAddr, bool aOwnMemory) {
    rank = aRank;
    typeSize = aTypeSize;
    deviceNum = devNum;
    totalSize = typeSize;
    if (rank > 0) {
        havePortion.resize(rank);
        (~havePortion)->blockAssign(rank, aHavePortion);
        startIndexAdjustments.resize(rank);
        for (int i = 0; i < rank; i++) {
            assert(!havePortion[i].empty() || (devAddr && i == 0));
            totalSize *= std::max((UDvmType)1, havePortion[i].size());
            startIndexAdjustments[i] = 0;
        }
    }
    assert(totalSize > 0);
    if (!devAddr) {
        deviceAddr = devices[deviceNum]->allocBytes(totalSize, dvmhSettings.pageSize);
        ownMemory = true;
    } else {
        deviceAddr = devAddr;
        ownMemory = aOwnMemory;
    }
    assert(deviceAddr);
    if (rank > 0) {
        axisPerm.resize(rank);
        for (int i = 0; i < rank; i++)
            axisPerm[i] = i + 1;
    }
    diagonalizedState = 0;
    pageLocked = false;
}

DvmhBuffer::DvmhBuffer(const DvmhBuffer& origBuf, const DvmType indexOffset[]) {
    rank = origBuf.rank;
    typeSize = origBuf.typeSize;
    deviceNum = origBuf.deviceNum;
    totalSize = origBuf.totalSize;
    if (rank > 0) {
        havePortion.resize(rank);
        for (int i = 0; i < rank; i++)
            havePortion[i] = origBuf.havePortion[i] + indexOffset[i];
    }
    deviceAddr = origBuf.deviceAddr;
    ownMemory = false;
    if (rank > 0) {
        axisPerm.resize(rank);
        for (int i = 0; i < rank; i++)
            axisPerm[i] = origBuf.axisPerm[i];
    }
    diagonalizedState = origBuf.diagonalizedState;
    pageLocked = false; // We do it to avoid page unlocking in destructor.
}

bool DvmhBuffer::pageLock() {
    if (!pageLocked && !isIncomplete() && devices[deviceNum]->getType() == dtHost) {
        bool hasCuda = false;
        for (int i = 0; i < devicesCount; i++) {
            if (devices[i]->getType() == dtCuda)
                hasCuda = true;
        }
        if (hasCuda)
            pageLocked = ((HostDevice *)devices[deviceNum])->pageLock(deviceAddr, totalSize);
    }
    return pageLocked;
}

bool DvmhBuffer::isTransformed() const {
    if (diagonalizedState != 0)
        return true;
    for (int i = 0; i < rank; i++) {
        if (axisPerm[i] != i + 1)
            return true;
    }
    return false;
}

void DvmhBuffer::doTransform(const int newAxisPerm[], int perDiagonalState) {
    if (rank <= 1)
        return;
#ifdef NON_CONST_AUTOS
    int mNewAxisPerm[rank], permNoDiag[rank];
#else
    int mNewAxisPerm[MAX_ARRAY_RANK], permNoDiag[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++)
        mNewAxisPerm[i] = (newAxisPerm ? newAxisPerm[i] : i + 1);
    canonicalizeTransformState(mNewAxisPerm, perDiagonalState);
    typedMemcpy(permNoDiag, mNewAxisPerm, rank);
    if (perDiagonalState) {
        int tmp = 0;
        canonicalizeTransformState(permNoDiag, tmp);
    }
    bool same = perDiagonalState == diagonalizedState;
    for (int i = 0; i < rank; i++)
        same = same && mNewAxisPerm[i] == axisPerm[i];
    if (same)
        return;
    if (isDiagonalized()) {
        applyDiagonal(0);
        canonicalizeTransformState();
    }
    assert(!isDiagonalized());
    same = true;
    for (int i = 0; i < rank; i++)
        same = same && permNoDiag[i] == axisPerm[i];
    if (!same) {
#ifdef NON_CONST_AUTOS
        int axisDelta[rank], revAxisPerm[rank];
#else
        int axisDelta[MAX_ARRAY_RANK], revAxisPerm[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < rank; i++)
            revAxisPerm[axisPerm[i] - 1] = i + 1;
        for (int i = 0; i < rank; i++)
            axisDelta[i] = revAxisPerm[permNoDiag[i] - 1];
        applyPermDelta(axisDelta);
    }
    if (perDiagonalState) {
        typedMemcpy(~axisPerm, mNewAxisPerm, rank);
        applyDiagonal(perDiagonalState);
    }
    assert(canonicalizeTransformState() == false);
}

void DvmhBuffer::setTransformState(const int newAxisPerm[], int perDiagonalState) {
    for (int i = 0; i < rank; i++)
        axisPerm[i] = (newAxisPerm ? newAxisPerm[i] : i + 1);
    diagonalizedState = perDiagonalState;
    canonicalizeTransformState();
}

void DvmhBuffer::undoDiagonal() {
    doTransform(getAxisPerm(), 0);
}

void DvmhBuffer::fillHeader(const void *base, DvmType header[], bool adjustForExternalUse, DvmhDiagonalInfo *diagonalInfo, bool zeroDiagonalized) const {
    DvmType collector = 1;
    DvmType offset = 0;
    header[0] = (DvmType)this;
    header[rank + 2] = (DvmType)base;
    checkInternal2(((DvmType)deviceAddr - header[rank + 2]) % typeSize == 0, "Impossible to calculate an offset of the array from the provided base");
    for (int i = rank; i >= 1; i--) {
        int origAx = axisPerm[i - 1];
        if (zeroDiagonalized && i >= rank - 1 && diagonalizedState != 0) {
            header[(origAx - 1) + 1] = 0;
        } else {
            DvmType adj = adjustForExternalUse ? startIndexAdjustments[origAx - 1] : 0;
            offset += collector * (havePortion[origAx - 1][0] + adj);
            header[(origAx - 1) + 1] = collector;
        }
        collector *= havePortion[axisPerm[i - 1] - 1].size();
    }
    header[rank + 1] = ((DvmType)deviceAddr - header[rank + 2]) / typeSize - offset;
    if (diagonalInfo) {
        if (diagonalizedState != 0) {
            int xAxis = axisPerm[rank - 1];
            int yAxis = axisPerm[rank - 2];
            diagonalInfo->slashFlag = diagonalizedState == 2;
            diagonalInfo->x_axis = xAxis;
            diagonalInfo->y_axis = yAxis;
            diagonalInfo->x_first = (havePortion[xAxis - 1][0] + (adjustForExternalUse ? startIndexAdjustments[xAxis - 1] : 0));
            diagonalInfo->y_first = (havePortion[yAxis - 1][0] + (adjustForExternalUse ? startIndexAdjustments[yAxis - 1] : 0));
            diagonalInfo->x_length = havePortion[xAxis - 1].size();
            diagonalInfo->y_length = havePortion[yAxis - 1].size();
        } else {
            diagonalInfo->slashFlag = -1;
        }
    }
}

void *DvmhBuffer::getNaturalBase(bool adjustForExternalUse) const {
    DvmType collector = 1;
    DvmType offset = 0;
    for (int i = rank; i >= 1; i--) {
        int origAx = axisPerm[i - 1];
        DvmType adj = adjustForExternalUse ? startIndexAdjustments[origAx - 1] : 0;
        if (diagonalizedState == 0 || i < rank - 1)
            offset += collector * (havePortion[origAx - 1][0] + adj);
        collector *= havePortion[origAx - 1].size();
    }
    return (char *)deviceAddr - offset * typeSize;
}

bool DvmhBuffer::hasElement(const DvmType indexes[]) const {
    if (isIncomplete())
        return indexes[0] >= havePortion[0].begin() && (getHavePortion() + 1)->blockContains(rank - 1, indexes + 1);
    else
        return getHavePortion()->blockContains(rank, indexes);
}

void *DvmhBuffer::getElemAddr(const DvmType indexes[]) const {
    DvmType collector = 1;
    DvmType offset = 0;
    for (int i = rank; i >= 1; i--) {
        int origAx = axisPerm[i - 1];
        if (diagonalizedState == 0 || i < rank - 1)
            offset += collector * (indexes[origAx - 1] - havePortion[origAx - 1][0]);
        collector *= havePortion[origAx - 1].size();
    }
    if (diagonalizedState != 0) {
        int xAxis = axisPerm[rank - 1];
        int yAxis = axisPerm[rank - 2];
        offset += dvmhXYToDiagonal(indexes[xAxis - 1] - havePortion[xAxis - 1][0], indexes[yAxis - 1] - havePortion[yAxis - 1][0],
                havePortion[xAxis - 1].size(), havePortion[yAxis - 1].size(), (diagonalizedState == 2 ? true : false));
    }
    return (char *)deviceAddr + offset * typeSize;
}

int DvmhBuffer::getMinIndexTypeSize(const Interval block[], bool adjustForExternalUse) const {
    UDvmType collector = 1;
    DvmType offset = 0;
    UDvmType maxAbsIdx = 0;
    for (int i = rank; i >= 1; i--) {
        DvmType adj = adjustForExternalUse ? startIndexAdjustments[axisPerm[i - 1] - 1] : 0;
        if (diagonalizedState == 0 || i < rank - 1)
            offset += collector * (havePortion[axisPerm[i - 1] - 1][0] + adj);
        maxAbsIdx += collector * std::max(std::abs(block[axisPerm[i - 1] - 1][0] + adj), std::abs(block[axisPerm[i - 1] - 1][1] + adj));
        collector *= havePortion[axisPerm[i - 1] - 1].size();
    }
    int natBaseBits = valueBits(maxAbsIdx) + 1;
    int factBaseBits = (offset < 0 ? valueBits(maxAbsIdx + (UDvmType)(-offset)) + 1 : natBaseBits);
    return divUpS(std::max(natBaseBits, factBaseBits), CHAR_BIT);
}

DvmhBuffer *DvmhBuffer::undiagCutting(const Interval cutting[]) const {
    assert(isDiagonalized());
#ifdef NON_CONST_AUTOS
    Interval resPortion[rank];
#else
    Interval resPortion[MAX_ARRAY_RANK];
#endif
    resPortion->blockAssign(rank, cutting);
    DvmhBuffer *res = new DvmhBuffer(rank, typeSize, deviceNum, resPortion);
    res->setTransformState(getAxisPerm(), 0);
    ((DvmhBuffer *)this)->undiagCuttingInternal(res->getDeviceAddr(), resPortion, false);
    return res;
}

void DvmhBuffer::replaceUndiagedCutting(DvmhBuffer *part) {
    assert(isDiagonalized());
#ifdef NON_CONST_AUTOS
    Interval partPortion[rank];
#else
    Interval partPortion[MAX_ARRAY_RANK];
#endif
    partPortion->blockAssign(rank, part->getHavePortion());
    undiagCuttingInternal(part->getDeviceAddr(), partPortion, true);
}

void DvmhBuffer::copyTo(DvmhBuffer *to, const Interval piece[]) const {
#ifdef NON_CONST_AUTOS
    Interval block[rank];
#else
    Interval block[MAX_ARRAY_RANK];
#endif
    assert(to);
    assert(rank == to->rank);
    assert(typeSize == to->typeSize);
    if (piece)
        block->blockAssign(rank, piece);
    else
        getHavePortion()->blockIntersect(rank, to->getHavePortion(), block);
    dvmh_log(TRACE, "Copying block:");
    custom_log(TRACE, blockOut, rank, block);
    dvmh_log(TRACE, "Source on device #%d has:", deviceNum);
    custom_log(TRACE, blockOut, rank, getHavePortion());
    dvmh_log(TRACE, "Destination on device #%d has:", to->deviceNum);
    custom_log(TRACE, blockOut, rank, to->getHavePortion());
    assert(getHavePortion()->blockContains(rank, block) && to->getHavePortion()->blockContains(rank, block));
    dvmhCopy(this, to, block);
}

bool DvmhBuffer::canDumpInplace(const Interval piece[]) const {
    return deviceNum == 0 && isBlockConsecutive(piece);
}

DvmhBuffer *DvmhBuffer::dumpPiece(const Interval piece[], bool allowInplace) const {
    DvmhBuffer *res = 0;
    bool inPlace = allowInplace && canDumpInplace(piece);
    if (!inPlace) {
        res = new DvmhBuffer(rank, typeSize, 0, piece);
        res->pageLock();
        copyTo(res);
    } else {
#ifdef NON_CONST_AUTOS
        DvmType beginIndexes[rank];
#else
        DvmType beginIndexes[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < rank; i++)
            beginIndexes[i] = piece[i].begin();
        res = new DvmhBuffer(rank, typeSize, 0, piece, getElemAddr(beginIndexes));
        dvmh_log(TRACE, "Made new in-place DvmhBuffer at offset " UDTFMT " bytes", (UDvmType)res->deviceAddr - (UDvmType)deviceAddr);
    }
    assert(res);
    return res;
}

bool DvmhBuffer::overlaps(const DvmhBuffer *other) const {
    assert(this != other);
    if (deviceNum == other->deviceNum) {
        char *addr1 = (char *)deviceAddr;
        char *addr2 = (char *)other->deviceAddr;
        return (addr1 <= addr2 && addr1 + totalSize > addr2) || (addr2 <= addr1 && addr2 + other->totalSize > addr1);
    } else {
        return false;
    }
}

DvmhBuffer::~DvmhBuffer() {
    if (pageLocked && devices[deviceNum]->getType() == dtHost)
        ((HostDevice *)devices[deviceNum])->pageUnlock(deviceAddr);
    if (ownMemory && deviceAddr)
        devices[deviceNum]->dispose(deviceAddr);
}

bool DvmhBuffer::canonicalizeTransformState(int perm[], int &diag) const {
    bool changed = false;
    if (rank == 0) {
        assert(diag == 0);
    } else if (rank == 1) {
        assert(perm[0] == 1);
        assert(diag == 0);
    } else {
        if (diag != 0) {
            UDvmType Rx = havePortion[perm[rank - 1] - 1].size();
            UDvmType Ry = havePortion[perm[rank - 2] - 1].size();
            bool slashFlag = diag == 2;
            if (!slashFlag && (Rx == 1 || Ry == 1)) {
                diag = 0;
                changed = true;
            } else if (slashFlag && Ry == 1) {
                diag = 0;
                changed = true;
            }
        }
        if (isIncomplete()) {
            for (int i = 0; i < rank; i++) {
                if (perm[i] != 1)
                    assert(havePortion[perm[i] - 1].size() == 1);
                else
                    break;
            }
        }
        int leaveRight = diag ? 2 : 0;
        assert(leaveRight <= rank);
        if (rank > leaveRight) {
            // TODO: change permutation: let all the one-sized dimensions stay at their original place
        }
    }
    return changed;
}

bool DvmhBuffer::isBlockConsecutive(const Interval piece[]) const {
    bool res = true;
    bool needWholeDim = false;
    for (int i = 0; i < rank; i++) {
        if (needWholeDim) {
            res = res && !isDiagonalized() && axisPerm[i] == i + 1 && havePortion[i] == piece[i];
        } else if (piece[i].size() > 1) {
            res = res && !isDiagonalized() && axisPerm[i] == i + 1;
            needWholeDim = true;
        }
    }
    return res;
}

void DvmhBuffer::applyDiagonal(int newDiagonalState) {
    assert((newDiagonalState == 0) ^ (diagonalizedState == 0));
    assert(!isIncomplete());
    dvmh_log(TRACE, "Applying diagonal transformation newDiagonalState = %d", newDiagonalState);
    if (devices[deviceNum]->getType() == dtCuda)
        ((CudaDevice *)devices[deviceNum])->deviceSynchronize();
    DvmhTimer tm(true);
    UDvmType Rx = havePortion[axisPerm[rank - 1] - 1].size();
    UDvmType Ry = havePortion[axisPerm[rank - 2] - 1].size();
    UDvmType Rz = 1;
    for (int i = 0; i < rank - 2; i++)
        Rz *= havePortion[axisPerm[i] - 1].size();
    bool slashFlag = (newDiagonalState + diagonalizedState) == 2;
    bool backFlag = newDiagonalState == 0;
    void *oldMem = deviceAddr;
    void *newMem = devices[deviceNum]->allocBytes(totalSize, dvmhSettings.pageSize);
    checkInternal2(devices[deviceNum]->getType() == dtCuda, "Diagonal transformation is implemented only for CUDA devices");
    dvmhCudaTransformArray((CudaDevice *)devices[deviceNum], oldMem, typeSize, Rx, Ry, Rz, backFlag, slashFlag, newMem);
    if (ownMemory) {
        devices[deviceNum]->dispose(oldMem);
        deviceAddr = newMem;
    } else {
        devices[deviceNum]->memCopy(oldMem, newMem, totalSize);
        devices[deviceNum]->dispose(newMem);
    }
    if (devices[deviceNum]->getType() == dtCuda)
        ((CudaDevice *)devices[deviceNum])->deviceSynchronize();
    double timeNow = tm.total();
    if (devices[deviceNum]->getType() == dtCuda)
        dvmh_stat_add_measurement(((CudaDevice *)devices[deviceNum])->index, DVMH_STAT_METRIC_UTIL_ARRAY_TRANSFORMATION, totalSize, 0.0, timeNow);
    diagonalizedState = newDiagonalState;
}

void DvmhBuffer::applyPermDelta(int permDelta[]) {
    assert(diagonalizedState == 0);
#ifdef MAX_ARRAY_RANK
    SmallVector<int, MAX_ARRAY_RANK> axes;
#else
    HybridVector<int, 10> axes;
#endif
    for (int i = 0; i < rank; i++) {
        if (permDelta[i] != i + 1)
            axes.push_back(i + 1);
    }
    assert(axes.size() > 1);
    assert(!isIncomplete());
    if (devices[deviceNum]->getType() == dtCuda)
        ((CudaDevice *)devices[deviceNum])->deviceSynchronize();
    DvmhTimer tm(true);
#ifdef NON_CONST_AUTOS
    Interval portion[rank];
#else
    Interval portion[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++)
        portion[i] = havePortion[axisPerm[i] - 1];
    UDvmType aggregatedTypeSize = typeSize;
    for (int i = axes.back() + 1; i <= rank; i++)
        aggregatedTypeSize *= portion[i - 1].size();
    UDvmType Rz = 1;
    for (int i = 1; i < axes[0]; i++)
        Rz *= portion[i - 1].size();
    if (axes.size() == 2) {
        // Transposition of 2 dimensions
        UDvmType Rx = portion[axes[1] - 1].size();
        UDvmType Ry = portion[axes[0] - 1].size();
        UDvmType Rbetween = 1;
        for (int i = axes[0] + 1; i < axes[1]; i++)
            Rbetween *= portion[i - 1].size();
        dvmh_log(TRACE, "Applying transposition of (transformed) axes %d and %d. typeSize=" DTFMT ", Rx=" DTFMT ", Rbetween=" DTFMT ", Ry=" DTFMT
                ", Rz=" DTFMT, axes[0], axes[1], aggregatedTypeSize, Rx, Rbetween, Ry, Rz);
        checkInternal2(devices[deviceNum]->getType() == dtCuda, "Dimension transposition is implemented only for CUDA devices");
        bool inPlace = dvmhCudaCanTransposeInplace((CudaDevice *)devices[deviceNum], deviceAddr, aggregatedTypeSize, Rx, Rbetween, Ry, Rz);
        void *oldMem = deviceAddr;
        void *newMem = (inPlace ? deviceAddr : devices[deviceNum]->allocBytes(totalSize, dvmhSettings.pageSize));
        dvmhCudaTransposeArray((CudaDevice *)devices[deviceNum], oldMem, aggregatedTypeSize, Rx, Rbetween, Ry, Rz, newMem);
        if (!inPlace) {
            if (ownMemory) {
                devices[deviceNum]->dispose(oldMem);
                deviceAddr = newMem;
            } else {
                devices[deviceNum]->memCopy(oldMem, newMem, totalSize);
                devices[deviceNum]->dispose(newMem);
            }
        }
    } else {
        // Any permutation
        int toLeft = axes[0] - 1;
        int toRight = rank - axes.back();
        int modRank = rank - toLeft - toRight;
#ifdef NON_CONST_AUTOS
        int modPerm[modRank];
        UDvmType modSizes[modRank];
#else
        int modPerm[MAX_ARRAY_RANK];
        UDvmType modSizes[MAX_ARRAY_RANK];
#endif
        UDvmType Rbetween = 1;
        for (int i = axes[0]; i <= axes.back(); i++) {
            modPerm[i - axes[0]] = permDelta[i - 1] - toLeft;
            modSizes[i - axes[0]] = portion[i - 1].size();
            Rbetween *= portion[i - 1].size();
        }
        dvmh_log(TRACE, "Applying permutation of %d dimensions", (int)axes.size());
        void *oldMem = deviceAddr;
        void *newMem = devices[deviceNum]->allocBytes(totalSize, dvmhSettings.pageSize);
        checkInternal2(devices[deviceNum]->getType() == dtCuda, "Dimension permutation is implemented only for CUDA devices");
        dvmhCudaPermutateArray((CudaDevice *)devices[deviceNum], oldMem, aggregatedTypeSize, modRank, modSizes, modPerm, Rz, newMem);
        if (ownMemory) {
            devices[deviceNum]->dispose(oldMem);
            deviceAddr = newMem;
        } else {
            devices[deviceNum]->memCopy(oldMem, newMem, totalSize);
            devices[deviceNum]->dispose(newMem);
        }
    }
    if (devices[deviceNum]->getType() == dtCuda)
        ((CudaDevice *)devices[deviceNum])->deviceSynchronize();
    double timeNow = tm.total();
    if (devices[deviceNum]->getType() == dtCuda)
        dvmh_stat_add_measurement(((CudaDevice *)devices[deviceNum])->index, DVMH_STAT_METRIC_UTIL_ARRAY_TRANSFORMATION, totalSize, 0.0, timeNow);
#ifdef NON_CONST_AUTOS
    int oldAxisPerm[rank];
#else
    int oldAxisPerm[MAX_ARRAY_RANK];
#endif
    typedMemcpy(oldAxisPerm, getAxisPerm(), rank);
    for (int i = 0; i < rank; i++)
        axisPerm[i] = oldAxisPerm[permDelta[i] - 1];
}

UDvmType DvmhBuffer::undiagCuttingInternal(void *res, Interval resPortion[], bool backFlag) {
    assert(isDiagonalized());
    int zStepAxis = 1;
    for (int i = rank - 2; i >= 1; i--) {
        int origAxis = axisPerm[i - 1];
        if (resPortion[origAxis - 1].size() > 1) {
            zStepAxis = i;
            break;
        }
    }
    for (int i = 1; i < zStepAxis; i++) {
        int origAxis = axisPerm[i - 1];
        if (resPortion[origAxis - 1].size() > 1) {
            Interval sav;
            sav = resPortion[origAxis - 1];
            UDvmType overallWritten = 0;
            for (DvmType curVal = sav[0]; curVal <= sav[1]; curVal++) {
                resPortion[origAxis - 1][0] = curVal;
                resPortion[origAxis - 1][1] = curVal;
                UDvmType written = undiagCuttingInternal((char *)res + overallWritten, resPortion, backFlag);
                overallWritten += written;
            }
            resPortion[origAxis - 1] = sav;
            return overallWritten;
        }
    }
    UDvmType Rx = havePortion[axisPerm[rank - 1] - 1].size();
    UDvmType Ry = havePortion[axisPerm[rank - 2] - 1].size();
    UDvmType Rz = 1;
    if (zStepAxis <= rank - 2)
        Rz *= resPortion[axisPerm[zStepAxis - 1] - 1].size();
    for (int i = zStepAxis + 1; i <= rank - 2; i++)
        Rz *= havePortion[axisPerm[i - 1] - 1].size();
    UDvmType xStart = resPortion[axisPerm[rank - 1] - 1][0] - havePortion[axisPerm[rank - 1] - 1][0];
    UDvmType xStep = 1;
    UDvmType xCount = resPortion[axisPerm[rank - 1] - 1].size();
    UDvmType yStart = resPortion[axisPerm[rank - 2] - 1][0] - havePortion[axisPerm[rank - 2] - 1][0];
    UDvmType yStep = 1;
    UDvmType yCount = resPortion[axisPerm[rank - 2] - 1].size();
    UDvmType zStep = 1;
    for (int i = zStepAxis + 1; i <= rank - 2; i++)
        zStep *= havePortion[axisPerm[i - 1] - 1].size();
    UDvmType offs = 0;
    UDvmType collector = 1;
    for (int i = rank; i >= 1; i--) {
        int origAxis = axisPerm[i - 1];
        if (i <= rank - 2)
            offs += collector * (resPortion[origAxis - 1][0] - havePortion[origAxis - 1][0]);
        collector *= havePortion[origAxis - 1].size();
    }
    bool slashFlag = (diagonalizedState == 2);
    checkInternal2(devices[deviceNum]->getType() == dtCuda, "Diagonal transformation is implemented only for CUDA devices");
    void *oldMem = (char *)deviceAddr + typeSize * offs;
    void *newMem = res;
    if (backFlag)
        std::swap(oldMem, newMem);
    dvmhCudaTransformCutting((CudaDevice *)devices[deviceNum], oldMem, typeSize, Rx, Ry, Rz, xStart, xStep, xCount, yStart, yStep, yCount, zStep, !backFlag,
            slashFlag, newMem);
    return typeSize * xCount * yCount * (Rz / zStep);
}

}
