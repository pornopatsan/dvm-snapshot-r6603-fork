#include "distrib.h"

#include "include/dvmhlib_const.h"

#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_log.h"
#include "mps.h"
#include "util.h"

namespace libdvmh {

// Intervals

void Intervals::append(const Interval &inter) {
    if (!inter.empty()) {
        checkInternal2(intervals.empty() || intervals.back()[1] + 1 < inter[0], "Use unite instead");
        intervals.push_back(inter);
        elementCount += inter.size();
    }
}

void Intervals::unite(const Interval &inter) {
    if (inter.empty()) {
        // Nothing to do here
    } else if (intervals.empty() || intervals.back()[1] + 1 < inter[0]) {
        // Append new to the end
        intervals.push_back(inter);
        elementCount += inter.size();
    } else if (inter[1] <= intervals.front()[1] && inter[1] >= intervals.front()[0] - 1) {
        if (inter[0] < intervals.front()[0]) {
            // Extend first
            elementCount += intervals.front()[0] - inter[0];
            intervals.front()[0] = inter[0];
        }
    } else if (inter[0] >= intervals.back()[0] && inter[0] <= intervals.back()[1] + 1) {
        if (inter[1] > intervals.back()[1]) {
            // Extend last
            elementCount += inter[1] - intervals.back()[1];
            intervals.back()[1] = inter[1];
        }
    } else {
        // Common case
        Interval toReplace = Interval::create(upperIndex(&intervals[0], intervals.size(), inter[0]), lowerIndex(&intervals[0], intervals.size(), inter[1]));
        if (toReplace.empty()) {
            // Insert new
            int where = toReplace[0];
            intervals.resize(intervals.size() + 1);
            for (int i = (int)intervals.size() - 1; i > where; i--)
                intervals[i] = intervals[i - 1];
            intervals[where] = inter;
            elementCount += inter.size();
        } else {
            // Replace several with one
            int howManyRemove = (int)toReplace.size() - 1;
            int where = toReplace[0];
            bool countRemoved = howManyRemove * 2 < (int)intervals.size();
            if (countRemoved) {
                for (int i = where + 1; i < where + 1 + howManyRemove; i++)
                    elementCount -= intervals[i].size();
            }
            if (howManyRemove > 0) {
                intervals[where][1] = intervals[toReplace[1]][1];
                for (int i = where + 1; i < (int)intervals.size() - howManyRemove; i++)
                    intervals[i] = intervals[i + howManyRemove];
                intervals.resize(intervals.size() - howManyRemove);
            }
            if (countRemoved)
                elementCount -= intervals[where].size();
            intervals[where].encloseInplace(inter);
            if (countRemoved) {
                elementCount += intervals[where].size();
            } else {
                elementCount = 0;
                for (int i = 0; i < (int)intervals.size(); i++)
                    elementCount += intervals[i].size();
            }
        }
    }
}

static bool doLinearConversion(DvmType elem, DvmType multiplier, DvmType summand, bool shrinkSpace, DvmType *pResult) {
    if (!shrinkSpace) {
        *pResult = elem * multiplier + summand;
        return true;
    } else {
        if ((elem - summand) % multiplier == 0) {
            *pResult = (elem - summand) / multiplier;
            return true;
        } else {
            return false;
        }
    }
}

void Intervals::uniteElements(const DvmType elements[], UDvmType size, DvmType multiplier, DvmType summand, bool shrinkSpace) {
    if (size <= 0) {
        return;
    }
    Interval interval = Interval::createEmpty();
    for (UDvmType elemIdx = 0; elemIdx < size; elemIdx++) {
        DvmType elem = elements[elemIdx];
        if (doLinearConversion(elem, multiplier, summand, shrinkSpace, &elem)) {
            if (interval.empty()) {
                interval[0] = interval[1] = elem;
            } else if (elem == interval[1] + 1) {
                interval[1] = elem;
            } else if (elem == interval[0] - 1) {
                interval[0] = elem;
            } else {
                unite(interval);
                interval[0] = interval[1] = elem;
            }
        }
    }
    if (!interval.empty()) {
        unite(interval);
    }
}

void Intervals::intersect(const Interval &inter) {
    if (empty()) {
        // Nothing to do here
    } else if (getBoundInterval().intersect(inter).empty()) {
        clear();
    } else if (inter[0] <= intervals.front()[0] && inter[1] >= intervals.back()[1]) {
        // Nothing to do here
    } else if (inter[0] <= intervals.front()[1] && inter[1] >= intervals.back()[0]) {
        // Modifications only to edge intervals
        if (inter[0] > intervals.front()[0]) {
            elementCount -= inter[0] - intervals.front()[0];
            intervals.front()[0] = inter[0];
        }
        if (inter[1] < intervals.back()[1]) {
            elementCount -= intervals.back()[1] - inter[1];
            intervals.back()[1] = inter[1];
        }
    } else {
        // Common case
        Interval toLeave = Interval::create(upperIndex(&intervals[0], intervals.size(), inter[0]), lowerIndex(&intervals[0], intervals.size(), inter[1]));
        if (toLeave.empty()) {
            clear();
        } else {
            int removeLeft = toLeave[0];
            int removeRight = (int)intervals.size() - 1 - toLeave[1];
            bool countRemoved = (removeLeft + removeRight) * 2 < (int)intervals.size();
            if (countRemoved) {
                for (int i = 0; i < removeLeft; i++)
                    elementCount -= intervals[i].size();
                for (int i = (int)intervals.size() - removeRight; i < (int)intervals.size(); i++)
                    elementCount -= intervals[i].size();
            }
            if (removeLeft > 0) {
                for (int i = 0; i < (int)intervals.size() - removeLeft - removeRight; i++)
                    intervals[i] = intervals[i + removeLeft];
            }
            if (removeLeft + removeRight > 0)
                intervals.resize((int)intervals.size() - removeLeft - removeRight);
            if (inter[0] > intervals.front()[0]) {
                if (countRemoved)
                    elementCount -= inter[0] - intervals.front()[0];
                intervals.front()[0] = inter[0];
            }
            if (inter[1] < intervals.back()[1]) {
                if (countRemoved)
                    elementCount -= intervals.back()[1] - inter[1];
                intervals.back()[1] = inter[1];
            }
            if (!countRemoved) {
                elementCount = 0;
                for (int i = 0; i < (int)intervals.size(); i++)
                    elementCount += intervals[i].size();
            }
        }
    }
}

DvmhPieces *Intervals::toPieces() const {
    DvmhPieces *res = new DvmhPieces(1);
    for (int i = 0; i < (int)intervals.size(); i++) {
        res->appendOne(&intervals[i]);
    }
    return res;
}

Interval Intervals::getBoundInterval() const {
    if (intervals.size() < 1)
        return Interval::createEmpty();
    return Interval::create(intervals[0][0], intervals.back()[1]);
}

// DvmhAxisDistribRule

ReplicatedAxisDistribRule *DvmhAxisDistribRule::createReplicated(MultiprocessorSystem *mps, const Interval &spaceDim) {
    return new ReplicatedAxisDistribRule(mps, spaceDim);
}

BlockAxisDistribRule *DvmhAxisDistribRule::createBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim) {
    return new BlockAxisDistribRule(dtBlock, mps, mpsAxis, spaceDim, 1);
}

BlockAxisDistribRule *DvmhAxisDistribRule::createWeightBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmhData *weights) {
    return new BlockAxisDistribRule(mps, mpsAxis, spaceDim, weights);
}

BlockAxisDistribRule *DvmhAxisDistribRule::createGenBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmhData *givenGbl) {
    return new BlockAxisDistribRule(mps, mpsAxis, givenGbl, spaceDim);
}

BlockAxisDistribRule *DvmhAxisDistribRule::createMultBlock(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, UDvmType multQuant) {
    return new BlockAxisDistribRule(dtMultBlock, mps, mpsAxis, spaceDim, multQuant);
}

IndirectAxisDistribRule *DvmhAxisDistribRule::createIndirect(MultiprocessorSystem *mps, int mpsAxis, DvmhData *givenMap) {
    return new IndirectAxisDistribRule(mps, mpsAxis, givenMap);
}

IndirectAxisDistribRule *DvmhAxisDistribRule::createDerived(MultiprocessorSystem *mps, int mpsAxis, const Interval &spaceDim, DvmType derivedBuf[],
        UDvmType derivedCount) {
    return new IndirectAxisDistribRule(mps, mpsAxis, spaceDim, derivedBuf, derivedCount);
}

// DistributedAxisDistribRule

DistributedAxisDistribRule::DistributedAxisDistribRule(DistribType dt, MultiprocessorSystem *aMps, int aMpsAxis): DvmhAxisDistribRule(dt, aMps, aMpsAxis) {
    sumGenBlock = new DvmType[mps->getAxis(mpsAxis).procCount + 1];
}

int DistributedAxisDistribRule::getProcIndex(DvmType spaceIndex) const {
    return (std::upper_bound(sumGenBlock, sumGenBlock + mps->getAxis(mpsAxis).procCount + 1, spaceIndex) - sumGenBlock) - 1;
}

// BlockAxisDistribRule

BlockAxisDistribRule::BlockAxisDistribRule(DistribType dt, MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, UDvmType aMultQuant):
        DistributedAxisDistribRule(dt, aMps, aMpsAxis) {
    // BLOCK and MULT_BLOCK
    assert(dt == dtBlock || dt == dtMultBlock);
    spaceDim = aSpaceDim;
    assert(spaceDim.size() > 0);
    multQuant = aMultQuant;
    assert(dt == dtMultBlock || multQuant == 1);
    assert(multQuant > 0 && spaceDim.size() % multQuant == 0);
    int procCount = mps->getAxis(mpsAxis).procCount;
    sumGenBlock[0] = spaceDim[0];
    UDvmType qCount = spaceDim.size() / multQuant;
    if (!mps->getAxis(mpsAxis).isWeighted()) {
        if (qCount >= (UDvmType)procCount) {
            UDvmType minBlock = qCount / procCount;
            UDvmType elemsToAdd = qCount % procCount;
            UDvmType curRemainder = 0;
            for (int p = 0; p < procCount; p++) {
                curRemainder = (curRemainder + elemsToAdd) % procCount;
                UDvmType curBlock = multQuant * (minBlock + (curRemainder < elemsToAdd));
                sumGenBlock[p + 1] = sumGenBlock[p] + curBlock;
            }
        } else {
            int offs = 0;
            if (dvmhSettings.useGenblock)
                offs = (procCount - (int)qCount) / 2;
            for (int p = 0; p < procCount; p++) {
                UDvmType curBlock = multQuant * (p >= offs && p - offs < (int)qCount);
                sumGenBlock[p + 1] = sumGenBlock[p] + curBlock;
            }
        }
    } else {
        MPSAxis axis = mps->getAxis(mpsAxis);
        double sumCoordWeight = 0;
        UDvmType prevEnd = 0;
        for (int p = 0; p < procCount; p++) {
            sumCoordWeight += axis.getCoordWeight(p);
            UDvmType curEnd = (UDvmType)(sumCoordWeight / procCount * qCount);
            if (prevEnd > 0 && curEnd <= prevEnd)
                curEnd = prevEnd + 1;
            if (p == procCount - 1)
                curEnd = qCount;
            UDvmType curBlock = multQuant * (curEnd - prevEnd);
            sumGenBlock[p + 1] = sumGenBlock[p] + curBlock;
            prevEnd = curEnd;
        }
    }
    wgtBlockPart.push_back(1);
    haveWgtPart = spaceDim;
    finishInit();
}

BlockAxisDistribRule::BlockAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, DvmhData *weights):
        DistributedAxisDistribRule(dtWgtBlock, aMps, aMpsAxis) {
    // WGT_BLOCK
    spaceDim = aSpaceDim;
    assert(spaceDim.size() > 0);
    assert(weights->getDataType() == DvmhData::dtFloat || weights->getDataType() == DvmhData::dtDouble);
    assert(weights->getRank() == 1);
    multQuant = 1;
    int procCount = mps->getAxis(mpsAxis).procCount;
    int ourProc = mps->getAxis(mpsAxis).ourProc;
    double wgtSum = 0;
    // TODO: Add support for distributed weights array. maybe
    assert(!weights->isDistributed());
    DvmType weightsLen = weights->getAxisSpace(1).size();
    for (DvmType j = 0; j < weightsLen; j++) {
        double blockWgt;
        if (weights->getDataType() == DvmhData::dtFloat)
            blockWgt = weights->getBuffer(0)->getElement<float>(j);
        else
            blockWgt = weights->getBuffer(0)->getElement<double>(j);
        wgtSum += blockWgt;
    }
    double wgtPerProc = wgtSum / procCount;
    double curSumWgt = 0;
    DvmType lastWgtBlockIdx = -1;
    UDvmType dimSize = spaceDim.size();
    UDvmType *resGenBlock = new UDvmType[procCount];
    int p = 0;
    resGenBlock[p] = 0;
    double sumCoordWeight = mps->getAxis(mpsAxis).getCoordWeight(p);
    for (DvmType j = 0; j < weightsLen; j++) {
        double blockWgt;
        if (weights->getDataType() == DvmhData::dtFloat)
            blockWgt = weights->getBuffer(0)->getElement<float>(j);
        else
            blockWgt = weights->getBuffer(0)->getElement<double>(j);
        DvmType blockStart = spaceDim[0] + j * (dimSize / weightsLen) + std::min(j, DvmType(dimSize % weightsLen));
        DvmType blockEnd = spaceDim[0] + (j + 1) * (dimSize / weightsLen) + std::min(j + 1, DvmType(dimSize % weightsLen)); //non-inclusive
        DvmType curStart = blockStart;
        DvmType curEnd = blockEnd;
        while (curEnd > curStart) {
            DvmType selectedCount;
            if (blockWgt > 0) {
                double selectedPart = std::max(0.0, std::min(1.0, (sumCoordWeight * wgtPerProc - curSumWgt) / blockWgt));
                selectedCount = std::max(DvmType(1), std::min(curEnd - curStart, DvmType((blockEnd - blockStart) * selectedPart)));
            } else
                selectedCount = curEnd - curStart;
            if (p == ourProc && j > lastWgtBlockIdx) {
                wgtBlockPart.push_back(blockWgt);
                if (wgtBlockPart.size() == 1)
                    haveWgtPart[0] = blockStart;
                haveWgtPart[1] = blockEnd - 1;
                lastWgtBlockIdx = j;
            }
            resGenBlock[p] += selectedCount;
            curSumWgt += blockWgt * double(selectedCount) / double(blockEnd - blockStart);
            curStart += selectedCount;
            if (p + 1 < procCount && curSumWgt >= sumCoordWeight * wgtPerProc) {
                p++;
                resGenBlock[p] = 0;
                sumCoordWeight += mps->getAxis(mpsAxis).getCoordWeight(p);
            }
        }
    }
    p++;
    for (; p < procCount; p++)
        resGenBlock[p] = 0;
    checkInternal(resGenBlock[ourProc] == 0 || wgtBlockPart.size() > 0);
    sumGenBlock[0] = spaceDim[0];
    for (int p = 0; p < procCount; p++)
        sumGenBlock[p + 1] = sumGenBlock[p] + resGenBlock[p];
    delete[] resGenBlock;
    finishInit();
}

BlockAxisDistribRule::BlockAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, DvmhData *givenGbl, const Interval &aSpaceDim):
        DistributedAxisDistribRule(dtGenBlock, aMps, aMpsAxis) {
    // GEN_BLOCK
    multQuant = 1;
    spaceDim = aSpaceDim;
    assert(spaceDim.size() > 0);
    assert(givenGbl->getDataType() == DvmhData::dtInt || givenGbl->getDataType() == DvmhData::dtLong || (givenGbl->getTypeType() == DvmhData::ttInteger &&
            givenGbl->getTypeSize() == sizeof(UDvmType)));
    int procCount = mps->getAxis(mpsAxis).procCount;
    sumGenBlock[0] = spaceDim[0];
    UDvmType dimSize = 0;
    for (int p = 0; p < procCount; p++) {
        UDvmType curBlock;
        if (givenGbl->getDataType() == DvmhData::dtInt)
            curBlock = givenGbl->getBuffer(0)->getElement<int>(p);
        else if (givenGbl->getDataType() == DvmhData::dtLong)
            curBlock = givenGbl->getBuffer(0)->getElement<long>(p);
        else
            curBlock = givenGbl->getBuffer(0)->getElement<UDvmType>(p);
        sumGenBlock[p + 1] = sumGenBlock[p] + curBlock;
        dimSize += curBlock;
    }
    assert(dimSize == spaceDim.size());
    wgtBlockPart.push_back(1);
    haveWgtPart = spaceDim;
    finishInit();
}

int BlockAxisDistribRule::genSubparts(const double distribPoints[], Interval parts[], int count) const {
    assert(count >= 0);
    int res = 0;
    if (localElems.empty()) {
        std::fill(parts, parts + count, localElems);
    } else if (distribType == dtGenBlock) {
        int maxWgtIdx = 0;
        for (int i = 1; i < count; i++) {
            if (distribPoints[i + 1] - distribPoints[i] > distribPoints[maxWgtIdx + 1] - distribPoints[maxWgtIdx])
                maxWgtIdx = i;
        }
        for (int i = 0; i < count; i++) {
            if (i < maxWgtIdx)
                parts[i] = Interval::create(localElems[0], localElems[0] - 1);
            else if (i == maxWgtIdx)
                parts[i] = localElems;
            else
                parts[i] = Interval::create(localElems[1] + 1, localElems[1]);
        }
        res = 1;
    } else {
        // TODO: Take into account wgtBlockPart
        DvmType initIdx = localElems[0];
        DvmType endIdx = localElems[1];
        UDvmType wholeSize = localElems.size();
        DvmType prevIdx = initIdx - 1;
        for (int i = 0; i < count; i++) {
            parts[i][0] = prevIdx + 1;
            if (distribPoints[i + 1] == 1 || i == count - 1)
                parts[i][1] = endIdx;
            else
                parts[i][1] = std::max(prevIdx, std::min(endIdx + 1, initIdx + (DvmType)roundDownU((UDvmType)(distribPoints[i + 1] * wholeSize + 1e-8),
                        multQuant)) - 1);
            if (!parts[i].empty())
                res++;
            prevIdx = parts[i][1];
        }
    }
    return res;
}

void BlockAxisDistribRule::changeMultQuant(UDvmType newMultQuant) {
    multQuant = newMultQuant;
    assert(localElems.size() % multQuant == 0);
    finishInit();
}

void BlockAxisDistribRule::changeTypeToBlock() {
    distribType = (multQuant == 1 ? dtBlock : dtMultBlock);
    finishInit();
}

void BlockAxisDistribRule::finishInit() {
    int ourProc = mps->getAxis(mpsAxis).ourProc;
    localElems = Interval::create(sumGenBlock[ourProc], sumGenBlock[ourProc + 1] - 1);
    if (distribType == dtGenBlock)
        maxSubparts = localElems.empty() ? 0 : 1;
    else
        maxSubparts = localElems.size() / multQuant;
}

// ExchangeMap

ExchangeMap::ExchangeMap(DvmhCommunicator *axisComm) {
    assert(axisComm);
    comm = axisComm;
    sendProcs = recvProcs = 0;
    sendStarts = recvStarts = 0;
    sendIndices = recvIndices = 0;
    bestOrder = 0;
    sendProcCount = recvProcCount = 0;
    sendSizes = recvSizes = 0;
    sendBuffers = recvBuffers = 0;
}

static void fillSendRecvFlags(int procCount, int commProcCount, const int procList[], const UDvmType indexStarts[], bool flags[]) {
    assert(procList);
    for (int i = 0; i < procCount; i++)
        flags[i] = false;
    for (int i = 0; i < commProcCount; i++)
        flags[procList[i]] = indexStarts[i + 1] - indexStarts[i] > 0;
}

static void fillSendRecvSizes(int procCount, int commProcCount, const int procList[], const UDvmType indexStarts[], UDvmType sizes[]) {
    assert(procList);
    for (int i = 0; i < procCount; i++)
        sizes[i] = 0;
    for (int i = 0; i < commProcCount; i++)
        sizes[procList[i]] = indexStarts[i + 1] - indexStarts[i];
}

bool ExchangeMap::checkConsistency() const {
    // Checks that for each send/recv operation in send/recv list there is a corresponding recv/send operation on other process
    int procCount = comm->getCommSize();

    UDvmType *sendSizes = new UDvmType[procCount];
    UDvmType *recvSizes = new UDvmType[procCount];
    UDvmType *sendSizesCheck = new UDvmType[procCount];
    UDvmType *recvSizesCheck = new UDvmType[procCount];

    fillSendRecvSizes(procCount, sendProcCount, sendProcs, sendStarts, sendSizes);
    fillSendRecvSizes(procCount, recvProcCount, recvProcs, recvStarts, recvSizes);

    comm->alltoall(sendSizes, recvSizesCheck);
    comm->alltoall(recvSizes, sendSizesCheck);

    bool isConsistent = true;
    for (int i = 0; i < procCount; i++)
        isConsistent = isConsistent && recvSizesCheck[i] == recvSizes[i] && sendSizesCheck[i] == sendSizes[i];

    delete[] sendSizes;
    delete[] recvSizes;
    delete[] sendSizesCheck;
    delete[] recvSizesCheck;

    comm->allreduce(isConsistent, rf_MIN);

    return isConsistent;
}

static void fillProcPointers(int procCount, int commProcCount, const int procList[], int procPointers[], int multiplier = 1) {
    assert(procList);
    for (int i = 0; i < procCount; i++)
        procPointers[i] = 0;
    for (int i = 0; i < commProcCount; i++)
        procPointers[procList[i]] = multiplier * (i + 1);
}

template <typename T>
static int getNextAvailableWave(T selfMask[], const T receivedMask[], int maskArrayLength, int &maskArrayIndex) {
    T currentMask = 0;
    for (; maskArrayIndex < maskArrayLength; maskArrayIndex++) {
        currentMask = receivedMask[maskArrayIndex] & selfMask[maskArrayIndex];
        if (currentMask)
            break;
    }
    checkInternal(currentMask);
    int waveOffset = ilogN(currentMask, 1);
    selfMask[maskArrayIndex] &= ~((T)1 << waveOffset);
    return maskArrayIndex * sizeof(T) * CHAR_BIT + waveOffset;
}

void ExchangeMap::fillBestOrder() {
    // If send/recv lists are not consistent (see checkConsistency), function will hang.

    int ourProc = comm->getCommRank();
    int procCount = comm->getCommSize();

    bool *sendFlags = new bool[procCount];
    bool *recvFlags = new bool[procCount];
    int *sendPointers = new int[procCount];
    int *recvPointers = new int[procCount];

    // Setting flags for sending
    fillSendRecvFlags(procCount, sendProcCount, sendProcs, sendStarts, sendFlags);
    fillSendRecvFlags(procCount, recvProcCount, recvProcs, recvStarts, recvFlags);
    fillProcPointers(procCount, sendProcCount, sendProcs, sendPointers);
    fillProcPointers(procCount, recvProcCount, recvProcs, recvPointers, -1);

    dvmh_log(DEBUG, "Started ordering...");

    int maxWaves = procCount * 2;
    int *tempBestOrder = new int[maxWaves];
    // Masks show availability. 1 in a specific position of the mask means that this process can do
    // communication on this specific "wave", and 0 means that process is already busy at that "wave".
    int maskElemBits = sizeof(unsigned long) * CHAR_BIT;
    int maskArrayLength = divUpS(maxWaves, maskElemBits);
    int maskArraySize = maskArrayLength * sizeof(unsigned long);
    unsigned long *selfAvailabilityMask = new unsigned long[maskArrayLength];
    unsigned long *receivedAvailabilityMask = new unsigned long[maskArrayLength];
    for (int i = 0; i < maxWaves; i++)
        tempBestOrder[i] = 0;
    for (int i = 0; i < maskArrayLength; i++)
        selfAvailabilityMask[i] = ~0UL;

    // step is used for setting fixed preliminary order
    int step = std::max((procCount - 1) / 2, 1);
    // finding correct step closest to procCount / 2 for efficiency on most common communication patterns
    int offs = 0;
    while (gcd(procCount, step + offs) != 1) {
        if (offs <= 0)
            offs = - offs + 1;
        else
            offs = - offs;
    }
    step = step + offs;

    int otherProc = (step - ourProc + procCount) % procCount;
    for (int i = 0; i < procCount; i++) {
        otherProc = (otherProc + step) % procCount;
        if (otherProc == ourProc)
            continue;
        if (!recvFlags[otherProc] && !sendFlags[otherProc])
            continue;
        // When 2 processes communicate, first sends it's mask to second one, which calculates "waves", when they will
        // conduct exchange in an optimized order. Then second one sends back numbers for those "waves".
        if (otherProc > ourProc) {
            int recvWaveNum[2];
            comm->send(otherProc, selfAvailabilityMask, maskArraySize);
            comm->recv(otherProc, recvWaveNum);
            // recvWaveNum[0] contains wave number, when this process should send a message to otherProc
            // recvWaveNum[1] contains wave number, when this process should receive a message from otherProc
            // -1 in any of them means that this type of message exchange is not needed
            assert(sendFlags[otherProc] == (recvWaveNum[0] >= 0));
            assert(recvFlags[otherProc] == (recvWaveNum[1] >= 0));
            if (recvWaveNum[0] >= 0) {
                int waveNum = recvWaveNum[0];
                assert(waveNum < maxWaves);
                int maskArrayIndex = waveNum / maskElemBits;
                int maskBitIndex = waveNum % maskElemBits;
                selfAvailabilityMask[maskArrayIndex] &= ~(1UL << maskBitIndex);
                tempBestOrder[waveNum] = otherProc + 1;
            }
            if (recvWaveNum[1] >= 0) {
                int waveNum = recvWaveNum[1];
                assert(waveNum < maxWaves);
                int maskArrayIndex = waveNum / maskElemBits;
                int maskBitIndex = waveNum % maskElemBits;
                selfAvailabilityMask[maskArrayIndex] &= ~(1UL << maskBitIndex);
                tempBestOrder[waveNum] = - (otherProc + 1);
            }
        } else {
            comm->recv(otherProc, receivedAvailabilityMask, maskArraySize);
            // second process finds 2 (or 1, if only one operation between processes is needed) earliest waves, where both
            // processes are available, and sets their exchange on those waves

            int sendWaveNum[2] = {-1, -1};
            int maskArrayIndex = 0;
            if (sendFlags[otherProc]) {
                int waveNum = getNextAvailableWave(selfAvailabilityMask, receivedAvailabilityMask, maskArrayLength, maskArrayIndex);
                checkInternal(waveNum < maxWaves);
                sendWaveNum[1] = waveNum;
                tempBestOrder[waveNum] = otherProc + 1;
            }
            if (recvFlags[otherProc]) {
                int waveNum = getNextAvailableWave(selfAvailabilityMask, receivedAvailabilityMask, maskArrayLength, maskArrayIndex);
                checkInternal(waveNum < maxWaves);
                sendWaveNum[0] = waveNum;
                tempBestOrder[waveNum] = - (otherProc + 1);
            }
            comm->send(otherProc, sendWaveNum);
        }
    }

    delete[] selfAvailabilityMask;
    delete[] receivedAvailabilityMask;
    delete[] sendFlags;
    delete[] recvFlags;

    // Filling the actual bestOrder with links to send/recv arrays. tempBestOrder has only ranks of processes by now.
    delete[] bestOrder;
    bestOrder = new int[sendProcCount + recvProcCount];
    int currentPosition = 0;
    for (int i = 0; i < maxWaves; i++) {
        if (tempBestOrder[i] > 0) {
            checkInternal(currentPosition < sendProcCount + recvProcCount);
            bestOrder[currentPosition] = sendPointers[tempBestOrder[i] - 1];
            currentPosition++;
        } else if (tempBestOrder[i] < 0) {
            checkInternal(currentPosition < sendProcCount + recvProcCount);
            bestOrder[currentPosition] = recvPointers[ - (tempBestOrder[i] + 1)];
            currentPosition++;
        }
    }
    for (; currentPosition < sendProcCount + recvProcCount; currentPosition++)
        bestOrder[currentPosition] = 0;

    dvmh_log(DEBUG, "Successfully filled bestOrder.");

    delete[] sendPointers;
    delete[] recvPointers;
    delete[] tempBestOrder;
}

bool ExchangeMap::checkBestOrder() const {
    // This is slow and should be used only for debug.

    unsigned char isBestOrderEmpty = 1;
    if (!bestOrder)
        isBestOrderEmpty = 1 << 1;

    comm->allreduce(isBestOrderEmpty, rf_AND);

    if (isBestOrderEmpty == 1 << 1)
        return true; // No bestOrder is a good bestOrder

    if (isBestOrderEmpty == 0)
        return false; // Not everyone has bestOrder - hence bestOrder is bad

    DvmhTimer tm(true);

    int ourProc = comm->getCommRank();
    int procCount = comm->getCommSize();

    dvmh_log(DEBUG, "ExchangeMap's bestOrder debug start");

    // The first half of debug checks. Makes sure that every operation required is present in bestOrder and only once.

    bool debugStatus = true;

    bool *sendDebug = new bool[sendProcCount];
    bool *recvDebug = new bool[recvProcCount];
    for (int i = 0; i < sendProcCount; i++)
        sendDebug[i] = false;
    for (int i = 0; i < recvProcCount; i++)
        recvDebug[i] = false;
    for (int i = 0; i < sendProcCount + recvProcCount; i++) {
        int procIdx = std::abs(bestOrder[i]) - 1;
        if (bestOrder[i] > 0) {
            assert(procIdx < sendProcCount);
            debugStatus = debugStatus && !sendDebug[procIdx];
            sendDebug[procIdx] = true;
        } else if (bestOrder[i] < 0) {
            assert(procIdx < recvProcCount);
            debugStatus = debugStatus && !recvDebug[procIdx];
            recvDebug[procIdx] = true;
        }
    }
    for (int i = 0; i < sendProcCount; i++) {
        if (!sendDebug[i])
            debugStatus = debugStatus && sendStarts[i + 1] - sendStarts[i] == 0;
    }
    for (int i = 0; i < recvProcCount; i++) {
        if (!recvDebug[i])
            debugStatus = debugStatus && recvStarts[i + 1] - recvStarts[i] == 0;
    }
    delete[] sendDebug;
    delete[] recvDebug;

    comm->allreduce(debugStatus, rf_MIN);

    dvmh_log(DEBUG, "Time to do the first half of debug check for bestOrder: %g", tm.lap());
    if (!debugStatus) {
        dvmh_log(DEBUG, "First half of debug failed, bestOrder was not correct.");
        return false;
    }
    dvmh_log(DEBUG, "First half of debug check succeeded.");

    // The second half of debug checks. Makes sure that there are no deadlocks.

    if (ourProc == 0) {
        int *processIndexes = new int[procCount];
        int myPosition = 0;

        processIndexes[0] = getNextPartnerAndDirection(myPosition);
        for (int i = 1; i < procCount; i++)
            comm->recv(i, processIndexes[i]);

        bool zeroFlag = false;
        while (debugStatus && !zeroFlag) {
            bool activeFlag = false;
            zeroFlag = true;
            for (int i = 0; i < procCount; i++) {
                if (processIndexes[i] == 0)
                    continue;
                zeroFlag = false;
                int partnerProc = std::abs(processIndexes[i]) - 1;
                if (std::abs(processIndexes[partnerProc]) - 1 == i && (sign(processIndexes[i]) * sign(processIndexes[partnerProc]) < 0)) {
                    if (i != 0)
                        comm->recv(i, processIndexes[i]);
                    if (partnerProc != 0)
                        comm->recv(partnerProc, processIndexes[partnerProc]);
                    if (i == 0 || partnerProc == 0)
                        processIndexes[0] = getNextPartnerAndDirection(myPosition);
                    activeFlag = true;
                }
            }
            debugStatus = debugStatus && (activeFlag || zeroFlag);
        }
        for (int i = 1; i < procCount; i++) {
            while (processIndexes[i] != 0)
                comm->recv(i, processIndexes[i]);
        }
        delete[] processIndexes;
    } else {
        int position = 0;
        while (int partnerAndDirection = getNextPartnerAndDirection(position))
            comm->send(0, partnerAndDirection);
        int endFlag = 0;
        comm->send(0, endFlag);
    }

    comm->bcast(0, debugStatus);

    dvmh_log(DEBUG, "Time to do the second half of debug check for bestOrder: %g", tm.lap());
    if (!debugStatus) {
        dvmh_log(DEBUG, "Second half of debug check failed. Deadlock was found.");
        return false;
    }
    dvmh_log(DEBUG, "Second half of debug check succeeded.");

    return true;
}

void ExchangeMap::freeze() {
    sendSizes = new UDvmType[sendProcCount];
    sendBuffers = new char *[sendProcCount];
    recvSizes = new UDvmType[recvProcCount];
    recvBuffers = new char *[recvProcCount];
    for (int i = 0; i < sendProcCount; i++)
        sendBuffers[i] = 0;
    for (int i = 0; i < recvProcCount; i++)
        recvBuffers[i] = 0;
}

void ExchangeMap::performExchange(void *buf, UDvmType indexOffset, UDvmType elemSize) const {
    if (!bestOrder) {
        // non-bestOrder exchange

        for (int i = 0; i < sendProcCount; i++) {
            sendSizes[i] = (sendStarts[i + 1] - sendStarts[i]) * elemSize;
            sendBuffers[i] = (sendSizes[i] ? new char[sendSizes[i]] : 0);

            char *currentAddr = sendBuffers[i];

            Interval acc = Interval::createEmpty();
            for (UDvmType j = sendStarts[i]; j < sendStarts[i + 1]; j++) {
                if (acc.empty()) {
                    acc[0] = acc[1] = sendIndices[j];
                } else if (acc[1] == sendIndices[j] - 1) {
                    acc[1]++;
                } else {
                    memcpy(currentAddr, (char *)buf + (acc[0] + indexOffset) * elemSize, acc.size() * elemSize);
                    currentAddr += acc.size() * elemSize;
                    acc[0] = acc[1] = sendIndices[j];
                }
            }
            if (!acc.empty())
                memcpy(currentAddr, (char *)buf + (acc[0] + indexOffset) * elemSize, acc.size() * elemSize);
        }

        for (int i = 0; i < recvProcCount; i++) {
            recvSizes[i] = (recvStarts[i + 1] - recvStarts[i]) * elemSize;
            recvBuffers[i] = (recvSizes[i] ? new char[recvSizes[i]] : 0);
        }

        comm->alltoallv3(sendProcCount, sendProcs, sendSizes, sendBuffers, recvProcCount, recvProcs, recvSizes, recvBuffers);

        for (int i = 0; i < recvProcCount; i++) {
            char *currentAddr = recvBuffers[i];

            Interval acc = Interval::createEmpty();
            for (UDvmType j = recvStarts[i]; j < recvStarts[i + 1]; j++) {
                if (acc.empty()) {
                    acc[0] = acc[1] = recvIndices[j];
                } else if (acc[1] == recvIndices[j] - 1) {
                    acc[1]++;
                } else {
                    memcpy((char *)buf + (acc[0] + indexOffset) * elemSize, currentAddr, acc.size() * elemSize);
                    currentAddr += acc.size() * elemSize;
                    acc[0] = acc[1] = recvIndices[j];
                }
            }
            if (!acc.empty())
                memcpy((char *)buf + (acc[0] + indexOffset) * elemSize, currentAddr, acc.size() * elemSize);
        }

        for (int i = 0; i < sendProcCount; i++) {
            delete[] sendBuffers[i];
        }
        for (int i = 0; i < recvProcCount; i++) {
            delete[] recvBuffers[i];
        }
    } else {
        // bestOrder exchange
        // calculating max amount of items in one exchange
        // TODO: maybe this should go into ExchangeMap, so we don't count it each time we do shadow renew
        UDvmType max = 0;
        for (int i = 0; i < sendProcCount; i++)
            max = std::max(max, sendStarts[i + 1] - sendStarts[i]);
        for (int i = 0; i < recvProcCount; i++)
            max = std::max(max, recvStarts[i + 1] - recvStarts[i]);

        char *addr = new char[max * elemSize];

        for (int i = 0; i < recvProcCount + sendProcCount; i++) {
            int bestOrderIndex = bestOrder[i];
            char *currentAddr = addr;
            if (bestOrderIndex > 0) {
                bestOrderIndex -= 1;
                Interval acc = Interval::createEmpty();
                for (UDvmType j = sendStarts[bestOrderIndex]; j < sendStarts[bestOrderIndex + 1]; j++) {
                    if (acc.empty()) {
                        acc[0] = acc[1] = sendIndices[j];
                    } else if (acc[1] == sendIndices[j] - 1) {
                        acc[1]++;
                    } else {
                        memcpy(currentAddr, (char *)buf + (acc[0] + indexOffset) * elemSize, acc.size() * elemSize);
                        currentAddr += acc.size() * elemSize;
                        acc[0] = acc[1] = sendIndices[j];
                    }
                }
                if (!acc.empty())
                    memcpy(currentAddr, (char *)buf + (acc[0] + indexOffset) * elemSize, acc.size() * elemSize);
                comm->send(sendProcs[bestOrderIndex], addr, (sendStarts[bestOrderIndex + 1] - sendStarts[bestOrderIndex]) * elemSize);
            } else {
                bestOrderIndex = -bestOrderIndex - 1;
                comm->recv(recvProcs[bestOrderIndex], addr, (recvStarts[bestOrderIndex + 1] - recvStarts[bestOrderIndex]) * elemSize);
                Interval acc = Interval::createEmpty();
                for (UDvmType j = recvStarts[bestOrderIndex]; j < recvStarts[bestOrderIndex + 1]; j++) {
                    if (acc.empty()) {
                        acc[0] = acc[1] = recvIndices[j];
                    } else if (acc[1] == recvIndices[j] - 1) {
                        acc[1]++;
                    } else {
                        memcpy((char *)buf + (acc[0] + indexOffset) * elemSize, currentAddr, acc.size() * elemSize);
                        currentAddr += acc.size() * elemSize;
                        acc[0] = acc[1] = recvIndices[j];
                    }
                }
                if (!acc.empty())
                    memcpy((char *)buf + (acc[0] + indexOffset) * elemSize, currentAddr, acc.size() * elemSize);
            }
        }
        delete[] addr;
    }
}

void ExchangeMap::performExchange(DvmhBuffer *dataBuffer, int dataAxis, DvmType axisDataOffset, const Interval boundBlock[]) const {
    assert(dataBuffer);
    int rank = dataBuffer->getRank();
#ifdef NON_CONST_AUTOS
    Interval dataBlock[rank];
#else
    Interval dataBlock[MAX_ARRAY_RANK];
#endif
    dataBlock->blockAssign(rank, boundBlock);
    bool canAccumulate = true;
    for (int i = 0; i < dataAxis - 1; i++)
        canAccumulate = canAccumulate && boundBlock[i].size() == 1;
    // TODO: Optimize code below, specifically optimizing DvmhBuffer usage (merging multiple into one when possible) and
    // using already existing arrays instead of new ones if possible in non-bestOrder exchange (like sendProcs/recvProcs, possibly more)

    // calculating size of a single exchange item
    UDvmType pieceSize = 1;
    for (int i = 0; i < rank; i++) {
        if (i != dataAxis - 1)
            pieceSize *= dataBlock[i].size();
    }

    UDvmType pieceSizeByte = pieceSize * dataBuffer->getTypeSize();

    if (!bestOrder) {
        // non-bestOrder exchange

        for (int i = 0; i < sendProcCount; i++) {
            sendSizes[i] = (sendStarts[i + 1] - sendStarts[i]) * pieceSizeByte;
            sendBuffers[i] = (sendSizes[i] ? new char[sendSizes[i]] : 0);

            char *currentAddr = sendBuffers[i];

            for (UDvmType j = sendStarts[i]; j < sendStarts[i + 1]; j++) {
                Interval axisPiece = Interval::create(sendIndices[j] + axisDataOffset, sendIndices[j] + axisDataOffset);
                dataBlock[dataAxis - 1] = axisPiece;
                DvmhBuffer reinterpretedAddr(rank, dataBuffer->getTypeSize(), 0, dataBlock, currentAddr);
                dataBuffer->copyTo(&reinterpretedAddr);
                currentAddr += pieceSizeByte;
            }
        }

        for (int i = 0; i < recvProcCount; i++) {
            recvSizes[i] = (recvStarts[i + 1] - recvStarts[i]) * pieceSizeByte;
            recvBuffers[i] = (recvSizes[i] ? new char[recvSizes[i]] : 0);
        }

        comm->alltoallv3(sendProcCount, sendProcs, sendSizes, sendBuffers, recvProcCount, recvProcs, recvSizes, recvBuffers);

        for (int i = 0; i < recvProcCount; i++) {

            char *currentAddr = recvBuffers[i];

            for (UDvmType j = recvStarts[i]; j < recvStarts[i + 1]; j++) {
                Interval axisPiece = Interval::create(recvIndices[j] + axisDataOffset, recvIndices[j] + axisDataOffset);
                dataBlock[dataAxis - 1] = axisPiece;
                DvmhBuffer reinterpretedAddr(rank, dataBuffer->getTypeSize(), 0, dataBlock, currentAddr);
                reinterpretedAddr.copyTo(dataBuffer);
                currentAddr += pieceSizeByte;
            }
        }

        for (int i = 0; i < sendProcCount; i++) {
            delete[] sendBuffers[i];
        }
        for (int i = 0; i < recvProcCount; i++) {
            delete[] recvBuffers[i];
        }
    } else {
        // bestOrder exchange
        // calculating max amount of items in one exchange
        // TODO: maybe this should go into ExchangeMap, so we don't count it each time we do shadow renew
        UDvmType max = 0;
        for (int i = 0; i < sendProcCount; i++)
            max = std::max(max, sendStarts[i + 1] - sendStarts[i]);
        for (int i = 0; i < recvProcCount; i++)
            max = std::max(max, recvStarts[i + 1] - recvStarts[i]);

        char *addr = new char[max * pieceSizeByte];

        for (int i = 0; i < recvProcCount + sendProcCount; i++) {
            int bestOrderIndex = bestOrder[i];
            char *currentAddr = addr;
            if (bestOrderIndex > 0) {
                bestOrderIndex -= 1;
                for (UDvmType j = sendStarts[bestOrderIndex]; j < sendStarts[bestOrderIndex + 1]; j++) {
                    Interval axisPiece = Interval::create(sendIndices[j] + axisDataOffset, sendIndices[j] + axisDataOffset);
                    dataBlock[dataAxis - 1] = axisPiece;
                    DvmhBuffer reinterpretedAddr(rank, dataBuffer->getTypeSize(), 0, dataBlock, currentAddr);
                    dataBuffer->copyTo(&reinterpretedAddr);
                    currentAddr += pieceSizeByte;
                }
                comm->send(sendProcs[bestOrderIndex], addr, (sendStarts[bestOrderIndex + 1] - sendStarts[bestOrderIndex]) * pieceSizeByte);
            } else {
                bestOrderIndex = -bestOrderIndex - 1;
                comm->recv(recvProcs[bestOrderIndex], addr, (recvStarts[bestOrderIndex + 1] - recvStarts[bestOrderIndex]) * pieceSizeByte);
                for (UDvmType j = recvStarts[bestOrderIndex]; j < recvStarts[bestOrderIndex + 1]; j++) {
                    Interval axisPiece = Interval::create(recvIndices[j] + axisDataOffset, recvIndices[j] + axisDataOffset);
                    dataBlock[dataAxis - 1] = axisPiece;
                    DvmhBuffer reinterpretedAddr(rank, dataBuffer->getTypeSize(), 0, dataBlock, currentAddr);
                    reinterpretedAddr.copyTo(dataBuffer);
                    currentAddr += pieceSizeByte;
                }
            }
        }
        delete[] addr;
    }
}

ExchangeMap::~ExchangeMap() {
    delete[] sendProcs; delete[] sendStarts; delete[] sendIndices;
    delete[] recvProcs; delete[] recvStarts; delete[] recvIndices;
    delete[] bestOrder;
    delete[] sendSizes;
    delete[] sendBuffers;
    delete[] recvSizes;
    delete[] recvBuffers;
}

int ExchangeMap::getNextPartnerAndDirection(int &position) const {
    int res = 0;
    while (position < sendProcCount + recvProcCount && bestOrder[position] == 0)
        position++;
    if (position < sendProcCount + recvProcCount) {
        assert(bestOrder[position] != 0);
        int procIdx = std::abs(bestOrder[position]) - 1;
        if (bestOrder[position] > 0)
            res = (sendProcs ? sendProcs[procIdx] : procIdx) + 1;
        else
            res = - (recvProcs ? recvProcs[procIdx] : procIdx) - 1;
        position++;
    }
    return res;
}

// IndirectShadow

void IndirectShadow::fillOwnL1Indices() {
    ownL1Indices.clear();
    UDvmType totalElements = exchangeMap.sendStarts[exchangeMap.sendProcCount];
    if (totalElements > 0) {
        DvmType *indexBuf = new DvmType[totalElements];
        for (UDvmType eli = 0; eli < totalElements; eli++) {
            indexBuf[eli] = exchangeMap.sendIndices[eli];
        }
        std::sort(indexBuf, indexBuf + totalElements);
        Interval toAdd = Interval::create(indexBuf[0], indexBuf[0]);
        for (UDvmType eli = 1; eli < totalElements; eli++) {
            if (indexBuf[eli] == toAdd.end() + 1) {
                toAdd.end() += 1;
            } else {
                ownL1Indices.unite(toAdd);
                toAdd = Interval::create(indexBuf[eli], indexBuf[eli]);
            }
        }
        ownL1Indices.unite(toAdd);
        delete[] indexBuf;
    }
}

// IndirectAxisDistribRule

IndirectAxisDistribRule::IndirectAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, DvmhData *givenMap):
        DistributedAxisDistribRule(dtIndirect, aMps, aMpsAxis) {
    shdWidth = ShdWidth::createEmpty();
    DvmhTimer tm(true);
    assert(givenMap->getDataType() == DvmhData::dtInt || givenMap->getDataType() == DvmhData::dtLong);
    assert(givenMap->getRank() == 1);
    assert(givenMap->isAligned());
    spaceDim = givenMap->getAxisSpace(1);
    assert(spaceDim.size() > 0);
    int procCount = mps->getAxis(mpsAxis).procCount;
    int ourProc = mps->getAxis(mpsAxis).ourProc;
    DvmhCommunicator *axisComm = mps->getAxis(mpsAxis).axisComm;
    createGlobalToLocal();
    // TODO: Do not demand it, allow any (block?) distribution
    checkError2(!givenMap->isDistributed() ||
            (!givenMap->getAlignRule()->getDspace()->getDistribRule()->hasIndirect() && givenMap->getAxisLocalPart(1) == globalToLocal->getAxisLocalPart(1)),
            "Map array for the indirect distribution can only be a regular array, a fully replicated array or a BLOCK-distributed array");
    givenMap->syncWriteAccesses();
    givenMap->getActual(givenMap->getLocalPlusShadow(), true);
    // XXX: It is not correct in some cases
    bool givenRepl = givenMap->getAxisSpace(1) == givenMap->getAxisLocalPart(1);
    UDvmType *fakeGenBlock = new UDvmType[procCount];
    memset(fakeGenBlock, 0, sizeof(UDvmType) * procCount);
    dvmh_log(DEBUG, "Time to prepare: %g", tm.lap());
    DvmhBuffer *mapArr = (givenMap->hasLocal() ? givenMap->getBuffer(0) : 0);
    DvmhBuffer *myGlobalToLocal = (globalToLocal->hasLocal() ? globalToLocal->getBuffer(0) : 0);
    Interval part;
    UDvmType *gblPart = 0;
    part = givenMap->getAxisLocalPart(1);
    for (DvmType gi = part[0]; gi <= part[1]; gi++) {
        int curProc = (givenMap->getDataType() == DvmhData::dtInt ? mapArr->getElement<int>(gi) : (int)(mapArr->getElement<long>(gi)));
        checkError3(curProc >= 0, "Invalid value encountered in mapping array at index " DTFMT, gi);
        curProc = curProc % procCount;
        fakeGenBlock[curProc]++;
    }
    if (!givenRepl) {
        gblPart = new UDvmType[procCount];
        typedMemcpy(gblPart, fakeGenBlock, procCount);
        axisComm->allreduce(fakeGenBlock, rf_SUM, procCount);
    }
    // fakeGenBlock is done
    dvmh_log(DEBUG, "Time to fill fakeGenBlock: %g", tm.lap());
    fillGenblock(fakeGenBlock);
    delete[] fakeGenBlock;
    createLocal2ToGlobal();
    dvmh_log(DEBUG, "Time to fill gbl and create ltg: %g", tm.lap());
    tm.push();
    if (givenRepl) {
        UDvmType filledCount = 0;
        UDvmType *filledByProc = new UDvmType[procCount];
        memset(filledByProc, 0, sizeof(UDvmType) * procCount);
        part = givenMap->getAxisLocalPart(1);
        Interval myPart = globalToLocal->getAxisLocalPart(1);
        for (DvmType gi = part[0]; gi <= part[1]; gi++) {
            int curProc = (givenMap->getDataType() == DvmhData::dtInt ? mapArr->getElement<int>(gi) : (int)(mapArr->getElement<long>(gi)));
            curProc = curProc % procCount;
            if (curProc == ourProc)
                local2ToGlobal[filledCount++] = gi;
            if (myPart.contains(gi))
                myGlobalToLocal->getElement<DvmType>(gi) = sumGenBlock[curProc] + (DvmType)filledByProc[curProc];
            filledByProc[curProc]++;
        }
        assert(filledCount == localElems.size());
        delete[] filledByProc;
        // local2ToGlobal and globalToLocal are done
    } else {
        assert(gblPart);
        UDvmType *sendSizes = new UDvmType[procCount];
        DvmType **sendBuffers = new DvmType *[procCount];
        UDvmType *sendFilled = new UDvmType[procCount];
        for (int p = 0; p < procCount; p++) {
            sendSizes[p] = gblPart[p] * sizeof(DvmType);
            sendBuffers[p] = gblPart[p] > 0 ? new DvmType[gblPart[p]] : 0;
            sendFilled[p] = 0;
        }
        dvmh_log(DEBUG, "Time to prepare send buffers: %g", tm.lap());
        part = givenMap->getAxisLocalPart(1);
        for (DvmType gi = part[0]; gi <= part[1]; gi++) {
            int curProc = (givenMap->getDataType() == DvmhData::dtInt ? mapArr->getElement<int>(gi) : (int)(mapArr->getElement<long>(gi)));
            curProc = curProc % procCount;
            sendBuffers[curProc][sendFilled[curProc]++] = gi;
        }
        delete[] sendFilled;
        dvmh_log(DEBUG, "Time to fill send buffers: %g", tm.lap());
        UDvmType *recvSizes = new UDvmType[procCount];
        char **recvBuffers = new char *[procCount];
        axisComm->alltoallv1(sendSizes, (char **)sendBuffers, recvSizes, recvBuffers);
        for (int p = 0; p < procCount; p++)
            delete[] sendBuffers[p];
        delete[] sendSizes;
        delete[] sendBuffers;
        dvmh_log(DEBUG, "Time to alltoallv and delete[]: %g", tm.lap());
        UDvmType filledCount = 0;
        for (int p = 0; p < procCount; p++) {
            assert(recvSizes[p] % sizeof(DvmType) == 0);
            memcpy(&local2ToGlobal[filledCount], recvBuffers[p], recvSizes[p]);
            filledCount += recvSizes[p] / sizeof(DvmType);
            delete[] recvBuffers[p];
        }
        assert(filledCount == localElems.size());
        delete[] recvSizes;
        delete[] recvBuffers;
        dvmh_log(DEBUG, "Time to copy received buffers: %g", tm.lap());
        std::sort(local2ToGlobal, local2ToGlobal + localElems.size());
        dvmh_log(DEBUG, "Time to sort local2ToGlobal: %g", tm.lap());
        delete[] gblPart;
        // local2ToGlobal is done
        // TODO: Work also with other-type distributed mapping array, maybe
        part = globalToLocal->getAxisLocalPart(1);
        for (DvmType gi = part[0]; gi <= part[1]; gi++) {
            int curProc = (givenMap->getDataType() == DvmhData::dtInt ? mapArr->getElement<int>(gi) : (int)(mapArr->getElement<long>(gi)));
            curProc = curProc % procCount;
            myGlobalToLocal->getElement<DvmType>(gi) = curProc;
        }
        // globalToLocal is filled with processor indexes
        dvmh_log(DEBUG, "Time to prefill globalToLocal: %g", tm.lap());
        finishGlobalToLocal();
        dvmh_log(DEBUG, "Time to finish globalToLocal: %g", tm.lap());
        // globalToLocal is done
    }
    tm.pop();
    dvmh_log(DEBUG, "Time to fill local2ToGlobal and globalToLocal: %g", tm.lap());
    dvmh_log(DEBUG, "Indirect localElems=" DTFMT ".." DTFMT, localElems[0], localElems[1]);
    if (0) {
        for (int i = 0; i < (int)localElems.size(); i++)
            dvmh_log(TRACE, "local2ToGlobal[" DTFMT "]=" DTFMT, localElems[0] + i, local2ToGlobal[i]);
        for (DvmType gi = globalToLocal->getAxisLocalPart(1)[0]; gi <= globalToLocal->getAxisLocalPart(1)[1]; gi++)
            dvmh_log(TRACE, "globalToLocal[" DTFMT "]=" DTFMT, gi, globalToLocal->getBuffer(0)->getElement<DvmType>(gi));
    }
    shdElements = 0;
    dvmh_log(DEBUG, "Total time for IndirectAxis creation: %g", tm.total());
}

static void sortAndTrim(DvmType buf[], UDvmType bufSize, Interval space, UDvmType *pToSkip, UDvmType *pToTruncate) {
    DvmhTimer tm(true);
    std::sort(buf, buf + bufSize);
    dvmh_log(DEBUG, "Time to sort: %g", tm.lap());
    UDvmType toSkip = 0;
    while (toSkip < bufSize && buf[toSkip] < space.begin())
        toSkip++;
    UDvmType toTruncate = 0;
    while (toSkip + toTruncate < bufSize && buf[bufSize - 1 - toTruncate] > space.end())
        toTruncate++;
    dvmh_log(DEBUG, "Time to trim: %g", tm.lap());
    *pToSkip = toSkip;
    *pToTruncate = toTruncate;
}

static void sortTrimAndFillUnique(DvmType buf[], UDvmType bufSize, Interval space, UDvmType *pUniqueCount, UDvmType **pUniqueStart) {
    UDvmType toSkip, toTruncate;
    sortAndTrim(buf, bufSize, space, &toSkip, &toTruncate);
    DvmhTimer tm(true);
    UDvmType &uniqueCount = *pUniqueCount;
    uniqueCount = bufSize > toSkip + toTruncate ? 1 : 0;
    for (UDvmType i = toSkip + 1; i < bufSize - toTruncate; i++) {
        if (buf[i - 1] != buf[i])
            uniqueCount++;
    }
    UDvmType *&uniqueStart = *pUniqueStart;
    uniqueStart = new UDvmType[uniqueCount + 1];
    uniqueStart[0] = toSkip;
    UDvmType uniqueIndex = 1;
    for (UDvmType i = toSkip + 1; i < bufSize - toTruncate; i++) {
        if (buf[i - 1] != buf[i])
            uniqueStart[uniqueIndex++] = i;
    }
    uniqueStart[uniqueCount] = bufSize - toTruncate;
    dvmh_log(DEBUG, "Time to fill unique: %g", tm.lap());
}

static UDvmType sortTrimAndMakeUnique(DvmType buf[], UDvmType bufSize, Interval space) {
    UDvmType toSkip, toTruncate;
    sortAndTrim(buf, bufSize, space, &toSkip, &toTruncate);
    DvmhTimer tm(true);
    UDvmType uniqueCount = 0;
    if (bufSize > toSkip + toTruncate) {
        if (uniqueCount != toSkip)
            buf[uniqueCount] = buf[toSkip];
        uniqueCount++;
    }
    for (UDvmType i = toSkip + 1; i < bufSize - toTruncate; i++) {
        if (buf[i - 1] != buf[i]) {
            if (uniqueCount != i)
                buf[uniqueCount] = buf[i];
            uniqueCount++;
        }
    }
    dvmh_log(DEBUG, "Time to make unique: %g", tm.lap());
    return uniqueCount;
}

IndirectAxisDistribRule::IndirectAxisDistribRule(MultiprocessorSystem *aMps, int aMpsAxis, const Interval &aSpaceDim, DvmType derivedBuf[],
        UDvmType derivedCount):
        DistributedAxisDistribRule(dtDerived, aMps, aMpsAxis) {
    DvmhTimer tm(true);
    shdWidth = ShdWidth::createEmpty();
    shdElements = 0;
    spaceDim = aSpaceDim;
    assert(spaceDim.size() > 0);
    createGlobalToLocal();
    int procCount = mps->getAxis(mpsAxis).procCount;
    int ourProc = mps->getAxis(mpsAxis).ourProc;
    DvmhCommunicator *axisComm = mps->getAxis(mpsAxis).axisComm;
    UDvmType uniqueCount = 0;
    UDvmType *uniqueStart = 0;
    sortTrimAndFillUnique(derivedBuf, derivedCount, spaceDim, &uniqueCount, &uniqueStart);
    assert(uniqueStart);
    dvmh_log(DEBUG, "Time to sort,trim,fill unique: %g", tm.lap());
    UDvmType *sendSizes = new UDvmType[procCount];
    char **sendBuffers = new char *[procCount];
    const BlockAxisDistribRule *axRule = globalToLocal->getAlignRule()->getDspace()->getAxisDistribRule(1)->asBlockDistributed();
    UDvmType nextConsider = 0;
    DvmType nextEl = derivedBuf[uniqueStart[nextConsider]];
    for (int p = 0; p < procCount; p++) {
        Interval part = axRule->getLocalElems(p);
        UDvmType partStart = uniqueCount;
        UDvmType elemsToSend = 0;
        while (nextConsider < uniqueCount && nextEl <= part.end()) {
            elemsToSend++;
            if (elemsToSend == 1)
                partStart = nextConsider;
            nextConsider++;
            nextEl = derivedBuf[uniqueStart[nextConsider]];
        }
        sendSizes[p] = (sizeof(DvmType) + 1) * elemsToSend;
        char *buf = elemsToSend > 0 ? new char[(sizeof(DvmType) + 1) * elemsToSend] : 0;
        DvmType *elemBuf = (DvmType *)buf;
        unsigned char *priorityBuf = (unsigned char *)(buf + sizeof(DvmType) * elemsToSend);
        for (UDvmType eli = 0; eli < elemsToSend; eli++) {
            UDvmType idx = uniqueStart[partStart + eli];
            elemBuf[eli] = derivedBuf[idx];
            priorityBuf[eli] = std::min((UDvmType)255, uniqueStart[partStart + eli + 1] - idx);
        }
        sendBuffers[p] = buf;
    }
    dvmh_log(DEBUG, "Time to fill send buffers: %g", tm.lap());
    UDvmType *recvSizes = new UDvmType[procCount];
    char **recvBuffers = new char *[procCount];
    axisComm->alltoallv1(sendSizes, sendBuffers, recvSizes, recvBuffers);
    for (int p = 0; p < procCount; p++)
        delete[] sendBuffers[p];
    dvmh_log(DEBUG, "Time to alltoallv and delete[]: %g", tm.lap());
    unsigned char *myPriorities = new unsigned char[globalToLocal->getLocalElemCount()];
    memset(myPriorities, 0, globalToLocal->getLocalElemCount());
    std::vector<DvmType> *rejectedElems = new std::vector<DvmType>[procCount];
    for (int p = 0; p < procCount; p++) {
        assert(recvSizes[p] % (sizeof(DvmType) + 1) == 0);
        UDvmType count = recvSizes[p] / (sizeof(DvmType) + 1);
        DvmType *elemBuf = (DvmType *)recvBuffers[p];
        unsigned char *priorityBuf = (unsigned char *)(recvBuffers[p] + sizeof(DvmType) * count);
        for (UDvmType eli = 0; eli < count; eli++) {
            DvmType curEl = elemBuf[eli];
            unsigned char curPriority = priorityBuf[eli];
            assert(globalToLocal->getAxisLocalPart(1).contains(curEl));
            DvmType localEli = curEl - globalToLocal->getAxisLocalPart(1).begin();
            dvmh_log(DONT_LOG, "Received element " DTFMT " with priority %d from %d", curEl, (int)curPriority, p);
            int curProc = p;
            if (curPriority > myPriorities[localEli]) {
                DvmType &resProc = globalToLocal->getBuffer(0)->getElement<DvmType>(curEl);
                if (myPriorities[localEli] > 0)
                    rejectedElems[resProc].push_back(curEl);
                resProc = curProc;
                myPriorities[localEli] = curPriority;
            } else {
                rejectedElems[curProc].push_back(curEl);
            }
        }
        delete[] recvBuffers[p];
    }
    for (UDvmType i = 0; i < globalToLocal->getLocalElemCount(); i++) {
        if (myPriorities[i] == 0)
            checkError3(0, "No processor has claimed an ownership of the element at index " DTFMT, globalToLocal->getAxisLocalPart(1).begin() + (DvmType)i);
    }
    delete[] myPriorities;
    dvmh_log(DEBUG, "Time to fill my globalToLocal part and rejectedElems: %g", tm.lap());
    for (int p = 0; p < procCount; p++) {
        sendSizes[p] = sizeof(DvmType) * rejectedElems[p].size();
        sendBuffers[p] = (char *)(!rejectedElems[p].empty() ? &rejectedElems[p][0] : 0);
    }
    UDvmType overlaySize = axisComm->alltoallv1(sendSizes, sendBuffers, recvSizes, recvBuffers) / sizeof(DvmType);
    delete[] rejectedElems;
    dvmh_log(DEBUG, "Time to alltoallv overlay: %g", tm.lap());
    DvmType *overlayElems = new DvmType[overlaySize];
    UDvmType filledCount = 0;
    for (int p = 0; p < procCount; p++) {
        assert(recvSizes[p] % sizeof(DvmType) == 0);
        UDvmType curCount = recvSizes[p] / sizeof(DvmType);
        for (UDvmType eli = 0; eli < curCount; eli++) {
            overlayElems[filledCount + eli] = ((DvmType *)recvBuffers[p])[eli];
            dvmh_log(TRACE, "Adding to overlay element with global index " DTFMT, overlayElems[filledCount + eli]);
        }
        filledCount += curCount;
        delete[] recvBuffers[p];
    }
    assert(filledCount == overlaySize);
    dvmh_log(DEBUG, "Time to fill overlayElems: %g", tm.lap());
    std::sort(overlayElems, overlayElems + overlaySize);
    dvmh_log(DEBUG, "Time to sort overlayElems: %g", tm.lap());
    for (UDvmType i = 1; i < overlaySize; i++)
        assert(overlayElems[i] > overlayElems[i - 1]);
    delete[] sendSizes;
    delete[] sendBuffers;
    delete[] recvSizes;
    delete[] recvBuffers;
    assert(overlaySize <= uniqueCount);
    UDvmType myLocalCount = uniqueCount - overlaySize;
    UDvmType *fakeGenBlock = new UDvmType[procCount];
    fakeGenBlock[ourProc] = myLocalCount;
    axisComm->allgather(fakeGenBlock);
    dvmh_log(DEBUG, "Time to allgather fakeGenBlock: %g", tm.lap());
    fillGenblock(fakeGenBlock);
    delete[] fakeGenBlock;
    createLocal2ToGlobal();
    UDvmType li = 0;
    UDvmType si = 0;
    for (UDvmType ui = 0; ui < uniqueCount; ui++) {
        DvmType curEl = derivedBuf[uniqueStart[ui]];
        while (si < overlaySize && overlayElems[si] < curEl)
            si++;
        if (si >= overlaySize || overlayElems[si] != curEl)
            local2ToGlobal[li++] = curEl;
    }
    assert(li == myLocalCount);
    delete[] uniqueStart;
    dvmh_log(DEBUG, "Time to fill local part of local2ToGlobal: %g", tm.lap());
    finishGlobalToLocal();
    dvmh_log(DEBUG, "Time to finish globalToLocal: %g", tm.lap());
    addShadow(overlayElems, overlaySize, "overlay");
    delete[] overlayElems;
    dvmh_log(DEBUG, "Time to add 'overlay' shadow edge: %g", tm.lap());
    dvmh_log(DEBUG, "Total time for IndirectAxis creation: %g", tm.total());
}

bool IndirectAxisDistribRule::fillPart(Intervals &res, const Interval &globalSpacePart, DvmType regInterval) const {
    res.clear();
    if (localElems.empty())
        return false;
    UDvmType myLocalSize = localElems.size();
    DvmType *myLocalToGlobal = local2ToGlobal + shdWidth[0];
    if (regInterval == 1 && globalSpacePart[0] <= myLocalToGlobal[0] && globalSpacePart[1] >= myLocalToGlobal[myLocalSize - 1]) {
        res.append(localElems);
    } else if (regInterval == 1) {
        res.append(Interval::create(upperIndex(myLocalToGlobal, myLocalSize, globalSpacePart[0]), lowerIndex(myLocalToGlobal, myLocalSize, globalSpacePart[1]))
                + localElems[0]);
    } else {
        dvmh_log(TRACE, "Bad case for IndirectAxisDistribRule::fillPart encountered");
        Interval enclosingInter = Interval::create(upperIndex(myLocalToGlobal, myLocalSize, globalSpacePart[0]), lowerIndex(myLocalToGlobal, myLocalSize,
                globalSpacePart[1]));
        Interval accumInter = Interval::createEmpty();
        for (DvmType li = enclosingInter[0]; li <= enclosingInter[1]; li++) {
            bool toTake = (myLocalToGlobal[li] - globalSpacePart[0]) % regInterval == 0;
            if (toTake) {
                if (accumInter.empty())
                    accumInter = Interval::create(li, li);
                else
                    accumInter[1]++;
            } else {
                if (!accumInter.empty()) {
                    res.append(accumInter + localElems[0]);
                    accumInter = Interval::createEmpty();
                }
            }
        }
        if (!accumInter.empty()) {
            res.append(accumInter + localElems[0]);
            accumInter = Interval::createEmpty();
        }
    }
    return !res.empty();
}

DvmType IndirectAxisDistribRule::globalToLocalOwn(DvmType ind, bool *pLocalElem) const {
    bool isLocal = false;
    DvmType res = localElems[0] + exactIndex(local2ToGlobal + shdWidth[0], localElems.size(), ind);
    if (localElems.contains(res))
        isLocal = true;
    if (pLocalElem)
        *pLocalElem = isLocal;
    return res;
}

DvmType IndirectAxisDistribRule::globalToLocal2(DvmType ind, bool *pLocalElem, bool *pShadowElem) const {
    bool isLocal = false, isShadow = false;
    DvmType res = globalToLocalOwn(ind, &isLocal);
    if (!isLocal) {
        DvmType shdInd = exactIndex(shdElements, shdWidth.size(), ShadowElementInfo::wrap(ind));
        if (shdInd >= 0 && shdInd < (DvmType)shdWidth.size()) {
            res = shdElements[shdInd].local2Index;
            isShadow = true;
        }
    }
    if (pLocalElem)
        *pLocalElem = isLocal;
    if (pShadowElem)
        *pShadowElem = isShadow;
    return res;
}

DvmType IndirectAxisDistribRule::convertLocal2ToGlobal(DvmType ind, bool *pLocalElem, bool *pShadowElem) const {
    bool isLocal = false, isShadow = false;
    DvmType firstIndex = localElems[0] - shdWidth[0];
    DvmType arrayOffset = ind - firstIndex;
    if ((arrayOffset >= 0 && arrayOffset < shdWidth[0]) ||
            (arrayOffset >= (DvmType)(shdWidth[0] + localElems.size()) && arrayOffset < (DvmType)(localElems.size() + shdWidth.size())))
    {
        isShadow = true;
    } else if (arrayOffset >= (DvmType)shdWidth[0] && arrayOffset < (DvmType)(localElems.size() + shdWidth[0])) {
        isLocal = true;
    }
    DvmType res = 0;
    if (isLocal || isShadow)
        res = local2ToGlobal[arrayOffset];
    if (pLocalElem)
        *pLocalElem = isLocal;
    if (pShadowElem)
        *pShadowElem = isShadow;
    return res;
}

int IndirectAxisDistribRule::genSubparts(const double distribPoints[], Interval parts[], int count) const {
    assert(count >= 0);
    int res = 0;
    if (localElems.empty()) {
        std::fill(parts, parts + count, localElems);
    } else if (count > 0) {
        int maxWgtIdx = 0;
        for (int i = 1; i < count; i++) {
            if (distribPoints[i + 1] - distribPoints[i] > distribPoints[maxWgtIdx + 1] - distribPoints[maxWgtIdx])
                maxWgtIdx = i;
        }
        for (int i = 0; i < count; i++) {
            if (i < maxWgtIdx)
                parts[i] = Interval::create(localElems[0], localElems[0] - 1);
            else if (i == maxWgtIdx)
                parts[i] = localElems;
            else
                parts[i] = Interval::create(localElems[1] + 1, localElems[1]);
        }
        res = 1;
    }
    return res;
}

template <typename T>
static void growArray(T *&arr, UDvmType prevSize, UDvmType toLeft, UDvmType toRight) {
    T *newArr = new T[toLeft + prevSize + toRight];
    if (arr) {
        typedMemcpy(newArr + toLeft, arr, prevSize);
        delete[] arr;
    }
    arr = newArr;
}

void IndirectAxisDistribRule::addShadow(DvmType elemBuf[], UDvmType elemCount, const std::string &name) {
    DvmhTimer tm(true);
    dvmh_log(DEBUG, "Making shadow edge '%s'", name.c_str());
    int myShadowNumber = -1;
    for (int i = 0; i < (int)shadows.size(); i++) {
        if (shadows[i])
            checkError3(shadows[i]->getName() != name, "Shadow edge with name '%s' already exists", name.c_str());
        else if (myShadowNumber < 0)
            myShadowNumber = i;
    }
    if (myShadowNumber < 0) {
        myShadowNumber = shadows.size();
        shadows.push_back(0);
    }
    UDvmType uniqueCount = sortTrimAndMakeUnique(elemBuf, elemCount, spaceDim);
    dvmh_log(DEBUG, "Time to sort,trim,make unique: %g", tm.lap());
    // Subtract local part
    elemCount = std::set_difference(elemBuf, elemBuf + uniqueCount, local2ToGlobal + shdWidth[0], local2ToGlobal + shdWidth[0] + localElems.size(), elemBuf)
            - elemBuf;
    // Now we have no out-of-space indexes, no duplicates, no local-part indexes
    dvmh_log(DEBUG, "Time to subtract local part: %g", tm.lap());
    // Calculate new elements count
    UDvmType newElemCount = 0;
    UDvmType si = 0;
    for (UDvmType eli = 0; eli < elemCount; eli++) {
        DvmType curEl = elemBuf[eli];
        while (si < shdWidth.size() && shdElements[si].globalIndex < curEl)
            si++;
        if (si >= shdWidth.size()) {
            newElemCount += elemCount - eli;
            break;
        }
        if (shdElements[si].globalIndex != curEl)
            newElemCount++;
    }
    dvmh_log(DEBUG, "Time to calculate new elements count: %g", tm.lap());
    tm.push();
    if (newElemCount < elemCount) {
        // Compactify old elements
        DvmType *l2Convert = new DvmType[shdWidth.size()];
        for (UDvmType si = 0; si < shdWidth.size(); si++)
            l2Convert[si] = si;
        bool isIdentical = true; // Flag if there is no need to convert indices
        int shadowBlockCount = shadowBlocks.size();
        for (int i = 0; i < shadowBlockCount; i++) {
            Interval curBlock = shadowBlocks[i].getBlock();
            std::vector<bool> foundElems(curBlock.size(), false);
            UDvmType foundCount = 0;
            UDvmType si = 0;
            for (UDvmType eli = 0; eli < elemCount; eli++) {
                DvmType curEl = elemBuf[eli];
                while (si < shdWidth.size() && shdElements[si].globalIndex < curEl)
                    si++;
                if (si < shdWidth.size() && shdElements[si].globalIndex == curEl && curBlock.contains(shdElements[si].local2Index)) {
                    foundCount++;
                    foundElems[shdElements[si].local2Index - curBlock.begin()] = true;
                }
            }
            if (foundCount == curBlock.size()) {
                // Add the whole block to our new shadow edge
                shadowBlocks[i].includeIn(myShadowNumber);
            } else if (foundCount > 0) {
                bool done = false;
                UDvmType count = 0;
                for (UDvmType j = 0; j < curBlock.size(); j++) {
                    if (foundElems[j])
                        count++;
                    else
                        break;
                }
                if (count == foundCount) {
                    // Split the block and add its first part to our new shadow edge
                    shadowBlocks.push_back(shadowBlocks[i].extractOnLeft(foundCount, myShadowNumber));
                    done = true;
                } else if (count == 0) {
                    UDvmType count = 0;
                    for (UDvmType j = 0; j < curBlock.size(); j++) {
                        if (foundElems[foundElems.size() - 1 - j])
                            count++;
                        else
                            break;
                    }
                    if (count == foundCount) {
                        // Split the block and add its second part to our new shadow edge
                        shadowBlocks.push_back(shadowBlocks[i].extractOnRight(foundCount, myShadowNumber));
                        done = true;
                    }
                }
                if (!done) {
                    // It is not an one piece at either edge. Reorder the block's elements so that first part belongs to our new shadow edge.
                    isIdentical = false;
                    DvmType ni = 0;
                    DvmType offs = shdWidth[0] + (curBlock[0] < localElems[0] ? -localElems[0] : -(localElems[1] + 1));
                    for (UDvmType j = 0; j < curBlock.size(); j++) {
                        if (foundElems[j]) {
                            l2Convert[curBlock[0] + j + offs] = curBlock[0] + ni + offs;
                            ni++;
                        }
                    }
                    for (UDvmType j = 0; j < curBlock.size(); j++) {
                        if (!foundElems[j]) {
                            l2Convert[curBlock[0] + j + offs] = curBlock[0] + ni + offs;
                            ni++;
                        }
                    }
                    // Split the block and add its first part to our new shadow edge
                    shadowBlocks.push_back(shadowBlocks[i].extractOnLeft(foundCount, myShadowNumber));
                }
            }
        }
        dvmh_log(DEBUG, "Time to modify shadowBlocks and fill l2Convert: %g", tm.lap());
        // shadowBlocks is done
        // l2Convert is ready
        if (!isIdentical) {
            // Converting L2 indices in all our structures.
            for (UDvmType i = 0; i < shdWidth.size(); i++) {
                DvmType prevL2 = shdElements[i].local2Index;
                DvmType prevOffs = shdWidth[0] + (prevL2 < localElems[0] ? -localElems[0] : -(localElems[1] + 1));
                DvmType prevL2Compact = prevL2 + prevOffs;
                DvmType newL2Compact = l2Convert[prevL2Compact];
                if (newL2Compact != prevL2Compact) {
                    DvmType newOffs = shdWidth[0] + (newL2Compact < shdWidth[0] ? -localElems[0] : -(localElems[1] + 1));
                    DvmType newL2 = newL2Compact - newOffs;
                    shdElements[i].local2Index = newL2;
                    local2ToGlobal[newL2 + shdWidth[0] - localElems[0]] = shdElements[i].globalIndex;
                }
            }
            for (int i = 0; i < (int)shadows.size(); i++) {
                if (shadows[i]) {
                    ExchangeMap &xchgMap = shadows[i]->getExchangeMap();
                    for (UDvmType j = 0; j < xchgMap.recvStarts[xchgMap.recvProcCount]; j++) {
                        DvmType prevL2 = xchgMap.recvIndices[j];
                        DvmType prevOffs = shdWidth[0] + (prevL2 < localElems[0] ? -localElems[0] : -(localElems[1] + 1));
                        DvmType prevL2Compact = prevL2 + prevOffs;
                        DvmType newL2Compact = l2Convert[prevL2Compact];
                        if (newL2Compact != prevL2Compact) {
                            DvmType newOffs = shdWidth[0] + (newL2Compact < shdWidth[0] ? -localElems[0] : -(localElems[1] + 1));
                            DvmType newL2 = newL2Compact - newOffs;
                            xchgMap.recvIndices[j] = newL2;
                        }
                    }
                }
            }
            // Conversion is done
            // TODO: Need to move data in arrays, maybe. Or declare that shadow adding is a disruptive operation regarding values in shadow edges.
        }
        dvmh_log(DEBUG, "Time to perform shadow edge indexes conversion: %g", tm.lap());
    }
    tm.pop();
    dvmh_log(DEBUG, "Time to compactify old elements: %g", tm.lap());
    tm.push();
    if (newElemCount > 0) {
        // Add new elements
        ShdWidth myWidth;
        myWidth[1] = std::min(spaceDim[1] - (localElems[1] + shdWidth[1]), (DvmType)newElemCount);
        myWidth[0] = newElemCount - myWidth[1];
        growArray(shdElements, shdWidth.size(), 0, myWidth.size());
        growArray(local2ToGlobal, localElems.size() + shdWidth.size(), myWidth[0], myWidth[1]);
        dvmh_log(DEBUG, "Time to grow indexing arrays: %g", tm.lap());
        DvmType newElemIdx = 0;
        si = 0;
        for (UDvmType eli = 0; eli < elemCount; eli++) {
            DvmType curEl = elemBuf[eli];
            while (si < shdWidth.size() && shdElements[si].globalIndex < curEl)
                si++;
            if (si >= shdWidth.size() || shdElements[si].globalIndex != curEl) {
                shdElements[shdWidth.size() + newElemIdx].globalIndex = curEl;
                DvmType l2i;
                if (newElemIdx < myWidth[0])
                    l2i = localElems[0] - shdWidth[0] - myWidth[0] + newElemIdx;
                else
                    l2i = localElems[1] + 1 + shdWidth[1] + (newElemIdx - myWidth[0]);
                shdElements[shdWidth.size() + newElemIdx].local2Index = l2i;
                shdElements[shdWidth.size() + newElemIdx].local1Index = spaceDim[0] - 1;
                shdElements[shdWidth.size() + newElemIdx].ownerProcessorIndex = -1;
                local2ToGlobal[l2i - localElems[0] + shdWidth[0] + myWidth[0]] = curEl;
                newElemIdx++;
            }
        }
        dvmh_log(DEBUG, "Time to add new indexes to indexing arrays: %g", tm.lap());
        assert(newElemIdx == (DvmType)newElemCount);
        shdWidth[0] += myWidth[0];
        shdWidth[1] += myWidth[1];
        if (myWidth[0] > 0) {
            Interval newBlock;
            newBlock[0] = localElems[0] - shdWidth[0];
            newBlock[1] = newBlock[0] + myWidth[0] - 1;
            shadowBlocks.push_back(IndirectShadowBlock(newBlock));
            shadowBlocks.back().includeIn(myShadowNumber);
        }
        if (myWidth[1] > 0) {
            Interval newBlock;
            newBlock[1] = localElems[1] + shdWidth[1];
            newBlock[0] = newBlock[1] - myWidth[1] + 1;
            shadowBlocks.push_back(IndirectShadowBlock(newBlock));
            shadowBlocks.back().includeIn(myShadowNumber);
        }
        std::sort(shdElements, shdElements + shdWidth.size()); // in fact, we can use merge_inplace
        dvmh_log(DEBUG, "Time to sort shdElements: %g", tm.lap());
    }
    tm.pop();
    dvmh_log(DEBUG, "Time to add index information: %g", tm.lap());
    fillShadowLocal1Indexes();
    dvmh_log(DEBUG, "Time to fill local1 indexes for shadow elements: %g", tm.lap());
    IndirectShadow *newShadow = new IndirectShadow(name, mps->getAxis(mpsAxis).axisComm);
    assert(myShadowNumber >= 0 && myShadowNumber < (int)shadows.size());
    assert(!shadows[myShadowNumber]);
    shadows[myShadowNumber] = newShadow;
    fillExchangeMap(myShadowNumber);
    dvmh_log(DEBUG, "Time to fill exchange map: %g", tm.lap());
    shadows[myShadowNumber]->fillOwnL1Indices();
    dvmh_log(DEBUG, "Time to fill own L1 indices: %g", tm.lap());
    dvmh_log(DEBUG, "Total time for addShadow: %g", tm.total());
}

bool IndirectAxisDistribRule::fillShadowL2Indexes(Intervals &res, const std::string &name) const {
    int shadowIndex = findShadow(name);
    if (shadowIndex < 0)
        return false;
    res.clear();
    for (int i = 0; i < (int)shadowBlocks.size(); i++) {
        if (shadowBlocks[i].isIncludedIn(shadowIndex))
            res.unite(shadowBlocks[i].getBlock());
    }
    return true;
}

const Intervals &IndirectAxisDistribRule::getShadowOwnL1Indexes(const std::string &name) const {
    int shadowIndex = findShadow(name);
    assert(shadowIndex >= 0);
    return shadows[shadowIndex]->getOwnL1Indices();
}

DvmType *IndirectAxisDistribRule::getLocal2ToGlobal(DvmType *pFirstIndex) const {
    if (pFirstIndex)
        *pFirstIndex = localElems[0] - shdWidth[0];
    return local2ToGlobal;
}

const ExchangeMap &IndirectAxisDistribRule::getShadowExchangeMap(const std::string &name) const {
    int shadowIndex = findShadow(name);
    assert(shadowIndex >= 0);
    return shadows[shadowIndex]->getExchangeMap();
}

IndirectAxisDistribRule::~IndirectAxisDistribRule() {
    delete globalToLocal;
    delete[] local2ToGlobal;
    delete[] shdElements;
    for (int i = 0; i < (int)shadows.size(); i++)
        delete shadows[i];
    shadows.clear();
}

void IndirectAxisDistribRule::createGlobalToLocal() {
    globalToLocal = new DvmhData(sizeof(DvmType), DvmhData::ttInteger, 1, &spaceDim, 0);
    {
        DvmhDistribSpace *dspace = new DvmhDistribSpace(1, &spaceDim);
        DvmhDistribRule *disRule = new DvmhDistribRule(1, mps);
        disRule->setAxisRule(1, DvmhAxisDistribRule::createBlock(mps, mpsAxis, spaceDim));
        dspace->redistribute(disRule);
        DvmhAxisAlignRule axRule;
        axRule.setLinear(1, 1, 0);
        DvmhAlignRule *alRule = new DvmhAlignRule(1, dspace, &axRule);
        globalToLocal->realign(alRule, true);
    }
    dvmh_log(DEBUG, "globalToLocal's localPart is " DTFMT ".." DTFMT, globalToLocal->getAxisLocalPart(1)[0], globalToLocal->getAxisLocalPart(1)[1]);
    if (globalToLocal->hasLocal()) {
        if (!globalToLocal->getRepr(0))
            globalToLocal->createNewRepr(0, globalToLocal->getLocalPart());
        globalToLocal->initActual(0);
    }
}

void IndirectAxisDistribRule::fillGenblock(const UDvmType fakeGenBlock[]) {
    int procCount = mps->getAxis(mpsAxis).procCount;
    int ourProc = mps->getAxis(mpsAxis).ourProc;
    UDvmType totalCount = 0;
    for (int i = 0; i < procCount; i++) {
        dvmh_log(TRACE, "fakeGenBlock[%d]=" UDTFMT, i, fakeGenBlock[i]);
        totalCount += fakeGenBlock[i];
    }
    checkInternal2(totalCount == spaceDim.size(), "Element count mismatch");
    sumGenBlock[0] = spaceDim[0];
    for (int i = 0; i < procCount; i++)
        sumGenBlock[i + 1] = sumGenBlock[i] + fakeGenBlock[i];
    // fake sumGenBlock is done
    localElems = Interval::create(sumGenBlock[ourProc], sumGenBlock[ourProc + 1] - 1);
    maxSubparts = localElems.empty() ? 0 : 1;
}

void IndirectAxisDistribRule::createLocal2ToGlobal() {
    if (!localElems.empty() || !shdWidth.empty())
        local2ToGlobal = new DvmType[localElems.size() + shdWidth.size()];
    else
        local2ToGlobal = 0;
}

void IndirectAxisDistribRule::finishGlobalToLocal() {
    // Converts globalToProc to globalToLocal. Properly filled local2ToGlobal and sumGenBlock needed.
    int procCount = mps->getAxis(mpsAxis).procCount;
    UDvmType *filledByProc = new UDvmType[procCount];
    UDvmType *filledByMe = new UDvmType[procCount];
    memset(filledByMe, 0, sizeof(UDvmType) * procCount);
    if (!localElems.empty()) {
        const BlockAxisDistribRule *axRule = globalToLocal->getAlignRule()->getDspace()->getAxisDistribRule(1)->asBlockDistributed();
        UDvmType nextConsider = 0;
        DvmType nextEl = local2ToGlobal[shdWidth[0]];
        for (int p = 0; p < procCount - 1; p++) {
            filledByMe[p + 1] = filledByMe[p];
            Interval part = axRule->getLocalElems(p);
            while (nextConsider < localElems.size() && nextEl <= part.end()) {
                filledByMe[p + 1]++;
                nextConsider++;
                nextEl = local2ToGlobal[shdWidth[0] + nextConsider];
            }
        }
    }
    mps->getAxis(mpsAxis).axisComm->alltoall(filledByMe, filledByProc);
    delete[] filledByMe;
    Interval part = globalToLocal->getAxisLocalPart(1);
    for (DvmType gi = part[0]; gi <= part[1]; gi++) {
        int curProc = globalToLocal->getBuffer(0)->getElement<DvmType>(gi);
        globalToLocal->getBuffer(0)->getElement<DvmType>(gi) = sumGenBlock[curProc] + (DvmType)filledByProc[curProc];
        filledByProc[curProc]++;
    }
    delete[] filledByProc;
    if (dvmhSettings.logLevel >= TRACE) {
        Interval part = globalToLocal->getAxisLocalPart(1);
        for (DvmType gi = part[0]; gi <= part[1]; gi++) {
            dvmh_log(TRACE, "globalToLocal[" DTFMT "] = " DTFMT, gi, globalToLocal->getBuffer(0)->getElement<DvmType>(gi));
        }
    }
}

void IndirectAxisDistribRule::fillShadowLocal1Indexes() {
    DvmhTimer tm(true);
    int procCount = mps->getAxis(mpsAxis).procCount;
    DvmhCommunicator *axisComm = mps->getAxis(mpsAxis).axisComm;
    UDvmType *sendSizes = new UDvmType[procCount];
    for (int p = 0; p < procCount; p++)
        sendSizes[p] = 0;
    std::vector<UDvmType> shdElementsToFill;
    for (UDvmType i = 0; i < shdWidth.size(); i++) {
        if (shdElements[i].local1Index < spaceDim[0]) {
            DvmType gi = shdElements[i].globalIndex;
            if (globalToLocal->getAxisLocalPart(1).contains(gi)) {
                shdElements[i].local1Index = globalToLocal->getBuffer(0)->getElement<DvmType>(gi);
                shdElements[i].ownerProcessorIndex = getProcIndex(shdElements[i].local1Index);
            } else {
                int p = globalToLocal->getAlignRule()->getDspace()->getAxisDistribRule(1)->asBlockDistributed()->getProcIndex(gi);
                assert(p >= 0 && p < procCount);
                shdElements[i].ownerProcessorIndex = p;
                sendSizes[p] += sizeof(DvmType);
                shdElementsToFill.push_back(i);
            }
        }
    }
    dvmh_log(DEBUG, "Time to fill shdElementsToFill: %g", tm.lap());
    DvmType **sendBuffers = new DvmType *[procCount];
    UDvmType *filledCount = new UDvmType[procCount];
    for (int p = 0; p < procCount; p++) {
        if (sendSizes[p])
            sendBuffers[p] = new DvmType[sendSizes[p] / sizeof(DvmType)];
        else
            sendBuffers[p] = 0;
        filledCount[p] = 0;
    }
    for (UDvmType j = 0; j < shdElementsToFill.size(); j++) {
        UDvmType i = shdElementsToFill[j];
        DvmType gi = shdElements[i].globalIndex;
        int p = shdElements[i].ownerProcessorIndex;
        sendBuffers[p][filledCount[p]] = gi;
        filledCount[p]++;
    }
    dvmh_log(DEBUG, "Time to fill sendBuffers: %g", tm.lap());
    DvmType **recvBuffers = new DvmType *[procCount];
    UDvmType *recvSizes = new UDvmType[procCount];
    axisComm->alltoallv1(sendSizes, (char **)sendBuffers, recvSizes, (char **)recvBuffers);
    dvmh_log(DEBUG, "Time to alltoallv1: %g", tm.lap());
    for (int p = 0; p < procCount; p++) {
        UDvmType elemCount = recvSizes[p] / sizeof(DvmType);
        for (UDvmType j = 0; j < elemCount; j++) {
            DvmType gi = recvBuffers[p][j];
            assert(globalToLocal->getAxisLocalPart(1).contains(gi));
            recvBuffers[p][j] = globalToLocal->getBuffer(0)->getElement<DvmType>(gi);
        }
    }
    dvmh_log(DEBUG, "Time to convert global indexes: %g", tm.lap());
    std::swap(recvSizes, sendSizes);
    std::swap(recvBuffers, sendBuffers);
    UDvmType answersReceived = axisComm->alltoallv2(sendSizes, (char **)sendBuffers, recvSizes, (char **)recvBuffers) / sizeof(DvmType);
    assert(shdElementsToFill.size() == answersReceived);
    dvmh_log(DEBUG, "Time to alltoallv2: %g", tm.lap());
    for (int p = 0; p < procCount; p++)
        filledCount[p] = 0;
    for (UDvmType j = 0; j < shdElementsToFill.size(); j++) {
        UDvmType i = shdElementsToFill[j];
        int p = shdElements[i].ownerProcessorIndex;
        shdElements[i].local1Index = recvBuffers[p][filledCount[p]];
        shdElements[i].ownerProcessorIndex = getProcIndex(shdElements[i].local1Index);
        filledCount[p]++;
    }
    if (dvmhSettings.logLevel >= TRACE) {
        for (UDvmType i = 0; i < shdWidth.size(); i++) {
            dvmh_log(TRACE, "shdElements[" DTFMT "]: globalIndex=" DTFMT " local1Index=" DTFMT " local2Index=" DTFMT " owner=%d", i, shdElements[i].globalIndex,
                    shdElements[i].local1Index, shdElements[i].local2Index, shdElements[i].ownerProcessorIndex);
        }
    }
    dvmh_log(DEBUG, "Time to fill shdElements[i].local1Index: %g", tm.lap());
    delete[] filledCount;
    delete[] recvSizes;
    delete[] sendSizes;
    for (int p = 0; p < procCount; p++) {
        delete[] recvBuffers[p];
        delete[] (char *)sendBuffers[p];
    }
    delete[] sendBuffers;
    delete[] recvBuffers;
}

void IndirectAxisDistribRule::fillExchangeMap(int shadowIndex) {
    DvmhTimer tm(true);
    ExchangeMap &xchgMap = shadows[shadowIndex]->getExchangeMap();
    int procCount = mps->getAxis(mpsAxis).procCount;
    DvmhCommunicator *axisComm = mps->getAxis(mpsAxis).axisComm;
    // Firstly, the 'recv' part
    UDvmType totalRecvElements = 0;
    for (int i = 0; i < (int)shadowBlocks.size(); i++) {
        if (shadowBlocks[i].isIncludedIn(shadowIndex))
            totalRecvElements += shadowBlocks[i].getBlock().size();
    }
    UDvmType *shdElementsToRecv = new UDvmType[totalRecvElements];
    DvmType *globalIndexesToRecv = new DvmType[totalRecvElements];
    UDvmType counter = 0;
    for (int i = 0; i < (int)shadowBlocks.size(); i++) {
        if (shadowBlocks[i].isIncludedIn(shadowIndex)) {
            Interval block = shadowBlocks[i].getBlock();
            DvmType startI = block[0] - localElems[0] + shdWidth[0];
            DvmType endI = startI + block.size();
            for (DvmType j = startI; j < endI; j++)
                globalIndexesToRecv[counter++] = local2ToGlobal[j];
        }
    }
    assert(counter == totalRecvElements);
    std::sort(globalIndexesToRecv, globalIndexesToRecv + totalRecvElements);
    UDvmType *fromProcessors = new UDvmType[procCount + 1];
    for (int p = 0; p <= procCount; p++)
        fromProcessors[p] = 0;
    for (UDvmType i = 0, j = 0; i < totalRecvElements; i++) {
        while (shdElements[j].globalIndex < globalIndexesToRecv[i])
            j++;
        shdElementsToRecv[i] = j;
        fromProcessors[shdElements[j].ownerProcessorIndex + 1]++;
    }
    delete[] globalIndexesToRecv;
    for (int p = 1; p <= procCount; p++)
        fromProcessors[p] += fromProcessors[p - 1];
    assert(totalRecvElements == fromProcessors[procCount]);
    int notEmptyCount = 0;
    for (int p = 0; p < procCount; p++) {
        if (fromProcessors[p + 1] - fromProcessors[p] > 0)
            notEmptyCount++;
    }
    xchgMap.recvProcCount = notEmptyCount;
    xchgMap.recvProcs = new int[notEmptyCount];
    xchgMap.recvStarts = new UDvmType[notEmptyCount + 1];
    xchgMap.recvIndices = new DvmType[totalRecvElements];
    for (int p = 0, notEmptyIdx = 0; p < procCount; p++) {
        if (fromProcessors[p + 1] - fromProcessors[p] > 0) {
            assert(notEmptyIdx < notEmptyCount);
            xchgMap.recvProcs[notEmptyIdx] = p;
            xchgMap.recvStarts[notEmptyIdx] = fromProcessors[p];
            notEmptyIdx++;
        }
    }
    xchgMap.recvStarts[notEmptyCount] = totalRecvElements;
    for (UDvmType i = 0; i < totalRecvElements; i++) {
        UDvmType j = shdElementsToRecv[i];
        int p = shdElements[j].ownerProcessorIndex;
        xchgMap.recvIndices[fromProcessors[p]] = shdElements[j].local2Index;
        fromProcessors[p]++;
    }
    // The 'recv' part is done
    dvmh_log(DEBUG, "Time to fill the recv part of the ExchangeMap: %g", tm.lap());
    // Working on the 'send' part
    UDvmType *sendSizes = new UDvmType[procCount];
    DvmType **sendBuffers = new DvmType *[procCount];
    for (int p = procCount - 1; p >= 0; p--) {
        UDvmType elemCount = fromProcessors[p] - (p > 0 ? fromProcessors[p - 1] : 0);
        if (elemCount > 0)
            sendBuffers[p] = new DvmType[elemCount];
        else
            sendBuffers[p] = 0;
        sendSizes[p] = elemCount * sizeof(DvmType);
        fromProcessors[p] = 0;
    }
    for (UDvmType i = 0; i < totalRecvElements; i++) {
        UDvmType j = shdElementsToRecv[i];
        int p = shdElements[j].ownerProcessorIndex;
        sendBuffers[p][fromProcessors[p]] = shdElements[j].local1Index;
        fromProcessors[p]++;
    }
    delete[] shdElementsToRecv;
    delete[] fromProcessors;
    UDvmType *recvSizes = new UDvmType[procCount];
    DvmType **recvBuffers = new DvmType *[procCount];
    UDvmType totalSendElems = axisComm->alltoallv1(sendSizes, (char **)sendBuffers, recvSizes, (char **)recvBuffers) / sizeof(DvmType);
    notEmptyCount = 0;
    for (int p = 0; p < procCount; p++) {
        if (recvSizes[p] > 0)
            notEmptyCount++;
    }
    xchgMap.sendProcCount = notEmptyCount;
    xchgMap.sendProcs = new int[notEmptyCount];
    xchgMap.sendStarts = new UDvmType[notEmptyCount + 1];
    xchgMap.sendIndices = new DvmType[totalSendElems];
    UDvmType nextSendElemIndex = 0;
    for (int p = 0, notEmptyIdx = 0; p < procCount; p++) {
        UDvmType elemCount = recvSizes[p] / sizeof(DvmType);
        if (elemCount > 0) {
            assert(notEmptyIdx < notEmptyCount);
            xchgMap.sendProcs[notEmptyIdx] = p;
            xchgMap.sendStarts[notEmptyIdx] = nextSendElemIndex;
            for (UDvmType i = 0; i < elemCount; i++)
                xchgMap.sendIndices[nextSendElemIndex++] = recvBuffers[p][i];
            notEmptyIdx++;
        }
        delete[] sendBuffers[p];
        delete[] (char *)recvBuffers[p];
    }
    assert(nextSendElemIndex == totalSendElems);
    xchgMap.sendStarts[notEmptyCount] = totalSendElems;
    delete[] sendSizes;
    delete[] sendBuffers;
    delete[] recvSizes;
    delete[] recvBuffers;
    // The 'send' part is done
    dvmh_log(DEBUG, "Time to fill the send part of the ExchangeMap: %g", tm.lap());
    xchgMap.freeze();
    if (dvmhSettings.checkExchangeMap) {
        bool isConsistent = xchgMap.checkConsistency();
        checkInternal2(isConsistent, "ExchangeMap consistency check failed");
        dvmh_log(DEBUG, "Time to check consistency of the ExchangeMap: %g", tm.lap());
    }
    if (dvmhSettings.preferBestOrderExchange) {
        xchgMap.fillBestOrder();
        dvmh_log(DEBUG, "Time to fill bestOrder of the ExchangeMap: %g", tm.lap());
    }
    if (dvmhSettings.checkExchangeMap) {
        bool isConsistent = xchgMap.checkBestOrder();
        checkInternal2(isConsistent, "BestOrder consistency check failed");
        dvmh_log(DEBUG, "Time to check bestOrder of the ExchangeMap: %g", tm.lap());
    }
    dvmh_log(DEBUG, "Total time to fill the ExchangeMap: %g", tm.total());
}

int IndirectAxisDistribRule::findShadow(const std::string &name) const {
    for (int i = 0; i < (int)shadows.size(); i++) {
        if (shadows[i] && shadows[i]->getName() == name)
            return i;
    }
    return -1;
}

// DvmhDistribRule

DvmhDistribRule::DvmhDistribRule(int aRank, const MultiprocessorSystem *aMPS) {
    rank = aRank;
    MPS = aMPS;
    assert(rank > 0);
    assert(MPS);
    axes = new DvmhAxisDistribRule *[rank];
    for (int i = 0; i < rank; i++)
        axes[i] = 0;
    hasIndirectFlag = false;
    hasLocalFlag = false;
    mpsAxesUsed = 0;
    localPart = new Interval[rank];
    for (int i = 0; i < rank; i++)
        localPart[i] = Interval::createEmpty();
    filledFlag = false;
}

void DvmhDistribRule::setAxisRule(int axis, DvmhAxisDistribRule *rule) {
    delete axes[axis - 1];
    assert(rule);
    axes[axis - 1] = rule;
    hasIndirectFlag = false;
    hasLocalFlag = true;
    filledFlag = true;
    std::set<int> usesMpsAxes;
    mpsAxesUsed = 0;
    for (int i = 0; i < rank; i++) {
        if (axes[i] && axes[i]->isIndirect())
            hasIndirectFlag = true;
        if (!axes[i] || axes[i]->getLocalElems().empty())
            hasLocalFlag = false;
        if (!axes[i])
            filledFlag = false;
        if (axes[i]) {
            assert(axes[i]->getMPS() == MPS);
            localPart[i] = axes[i]->getLocalElems();
            if (!axes[i]->isReplicated()) {
                mpsAxesUsed = std::max(mpsAxesUsed, axes[i]->getMPSAxis());
                bool newOne = usesMpsAxes.insert(axes[i]->getMPSAxis()).second;
                assert(newOne);
            }
        }
    }
}

bool DvmhDistribRule::fillLocalPart(int proc, Interval res[]) const {
    assert(filledFlag);
    if (proc == MPS->getCommRank()) {
        res->blockAssign(rank, localPart);
        return hasLocalFlag;
    }
    bool ret = true;
    for (int i = 0; i < rank; i++) {
        if (axes[i]->isReplicated()) {
            res[i] = axes[i]->getSpaceDim();
        } else {
            int axisProc = MPS->getAxisIndex(proc, axes[i]->getMPSAxis());
            if (axisProc == MPS->getAxis(axes[i]->getMPSAxis()).ourProc)
                res[i] = axes[i]->getLocalElems();
            else
                res[i] = axes[i]->asDistributed()->getLocalElems(axisProc);
        }
        ret = ret && !res[i].empty();
    }
    return ret;
}

DvmhDistribRule::~DvmhDistribRule() {
    for (int i = 0; i < rank; i++)
        delete axes[i];
    delete[] axes;
    delete[] localPart;
}

// DvmhDistribSpace

DvmhDistribSpace::DvmhDistribSpace(int aRank, const Interval aSpace[]) {
    rank = aRank;
    space = new Interval[rank];
    distribRule = 0;
    refCount = 0;
    for (int i = 0; i < rank; i++)
        space[i] = aSpace[i];
}

bool DvmhDistribSpace::isSuitable(const DvmhDistribRule *aDistribRule) const {
    bool res = true;
    res = res && aDistribRule->getRank() == rank;
    for (int i = 0; i < rank; i++)
        res = res && aDistribRule->getAxisRule(i + 1)->getSpaceDim() == space[i];
    return res;
}

void DvmhDistribSpace::redistribute(DvmhDistribRule *aDistribRule) {
    assert(isSuitable(aDistribRule));
    for (std::set<DvmhData *>::iterator it = alignedDatas.begin(); it != alignedDatas.end(); it++)
        (*it)->redistribute(aDistribRule);
    delete distribRule;
    distribRule = aDistribRule;
    assert(distribRule);
}

DvmhDistribSpace::~DvmhDistribSpace() {
    checkError2(alignedDatas.empty(), "Can not delete the template if any distributed arrays aligned with it remain");
    checkInternal(refCount == 0);
    assert(space);
    delete[] space;
    delete distribRule;
}

// DvmhAxisAlignRule

void DvmhAxisAlignRule::setReplicated(DvmType aMultiplier, const Interval &aReplicInterval) {
    if (aReplicInterval.size() == 1) {
        setConstant(aReplicInterval[0]);
    } else {
        axisNumber = -1;
        multiplier = aMultiplier;
        assert(multiplier > 0);
        summand = 0;
        summandLocal = summand;
        replicInterval = aReplicInterval;
    }
}

void DvmhAxisAlignRule::setConstant(DvmType aSummand) {
    axisNumber = 0;
    multiplier = 0;
    summand = aSummand;
    summandLocal = summand;
    replicInterval = Interval::createEmpty();
}

void DvmhAxisAlignRule::setLinear(int aAxisNumber, DvmType aMultiplier, DvmType aSummand) {
    axisNumber = aAxisNumber;
    assert(axisNumber > 0);
    multiplier = aMultiplier;
    assert(multiplier != 0);
    summand = aSummand;
    summandLocal = summand;
    replicInterval = Interval::createEmpty();
}

void DvmhAxisAlignRule::composite(const DvmhAxisAlignRule rules[]) {
    DvmhAxisAlignRule oldRule = *this;
    if (oldRule.axisNumber > 0) {
        int j = oldRule.axisNumber - 1;
        if (rules[j].axisNumber == -1) {
            DvmType mult;
            Interval replicInt;
            if (oldRule.multiplier > 0) {
                mult = oldRule.multiplier * rules[j].multiplier;
                replicInt[0] = rules[j].replicInterval[0] * oldRule.multiplier + oldRule.summand;
                replicInt[1] = rules[j].replicInterval[1] * oldRule.multiplier + oldRule.summand;
            } else {
                mult = (-1) * oldRule.multiplier * rules[j].multiplier;
                replicInt[0] = rules[j].replicInterval[1] * oldRule.multiplier + oldRule.summand;
                replicInt[1] = rules[j].replicInterval[0] * oldRule.multiplier + oldRule.summand;
            }
            setReplicated(mult, replicInt);
        } else if (rules[j].axisNumber == 0) {
            setConstant(rules[j].summand * oldRule.multiplier + oldRule.summand);
        } else {
            setLinear(rules[j].axisNumber, rules[j].multiplier * oldRule.multiplier, rules[j].summand * oldRule.multiplier + oldRule.summand);
        }
    }
}

// DvmhAlignRule

DvmhAlignRule::DvmhAlignRule(int aRank, DvmhDistribSpace *aDspace, const DvmhAxisAlignRule axisRules[]) {
    dspace = aDspace;
    assert(dspace);
    dspace->addRef();
    rank = aRank;
    assert(rank >= 0);
    map = new DvmhAxisAlignRule[dspace->getRank()];
    dspaceAxis = new int[rank];
    for (int i = 0; i < dspace->getRank(); i++)
        map[i] = axisRules[i];
    setDistribRule(dspace->getDistribRule());
}

DvmhAlignRule::DvmhAlignRule(const DvmhAlignRule &other) {
    dspace = other.dspace;
    dspace->addRef();
    rank = other.rank;
    map = new DvmhAxisAlignRule[dspace->getRank()];
    for (int i = 0; i < dspace->getRank(); i++)
        map[i] = other.map[i];
    dspaceAxis = new int[rank];
    for (int i = 0; i < rank; i++)
        dspaceAxis[i] = other.dspaceAxis[i];
    hasIndirectFlag = other.hasIndirectFlag;
    disRule = other.disRule;
}

bool DvmhAlignRule::mapOnPart(const Interval dspacePart[], Interval res[], bool allInLocalIndexes, const DvmType intervals[]) const {
    dvmh_log(TRACE, "Mapping rect to dspace part. Rect (%s):", (allInLocalIndexes ? "local" : "global"));
    custom_log(TRACE, blockOut, rank, res);
    dvmh_log(TRACE, "Part:");
    custom_log(TRACE, blockOut, dspace->getRank(), dspacePart);
    bool hasLocal = true;
    for (int i = 0; i < rank; i++)
        hasLocal = hasLocal && !res[i].empty();
    for (int i = 0; i < dspace->getRank(); i++) {
        Interval dspaceInt = dspacePart[i];
        const DvmhAxisAlignRule *curRule = getAxisRule(i + 1);
        int ax = curRule->axisNumber;
        Interval replicInt = curRule->replicInterval;
        DvmType mult = curRule->multiplier;
        DvmType summ = curRule->summand;
        if (!disRule->isIndirect(i + 1)) {
            if (ax == -1) {
                if (shrinkInterval(replicInt, mult, dspaceInt).empty())
                    hasLocal = false;
            } else if (ax == 0) {
                if (!dspaceInt.contains(summ))
                    hasLocal = false;
            } else {
                Interval tmp;
                if (mult > 0)
                    tmp = Interval::create(divUpS(dspaceInt[0] - summ, mult), divDownS(dspaceInt[1] - summ, mult));
                else
                    tmp = Interval::create(divDownS(dspaceInt[0] - summ, mult), divUpS(dspaceInt[1] - summ, mult));
                res[ax - 1] = shrinkInterval(res[ax - 1], intervals ? intervals[ax - 1] : 1, tmp);
                if (res[ax - 1].empty())
                    hasLocal = false;
            }
        } else {
            // If indirect - dspacePart is in local indexes
            DvmType summLocal = curRule->summandLocal;
            const IndirectAxisDistribRule *axRule = disRule->getAxisRule(i + 1)->asIndirect();
            if (ax == -1) {
                Intervals havePart;
                axRule->fillPart(havePart, replicInt, mult);
                havePart.intersect(dspaceInt);
                if (havePart.empty())
                    hasLocal = false;
            } else if (ax == 0) {
                Intervals havePart;
                axRule->fillPart(havePart, Interval::create(summ, summ));
                havePart.intersect(dspaceInt);
                if (havePart.empty())
                    hasLocal = false;
            } else {
                DvmType step = (intervals ? intervals[ax - 1] : 1);
                assert(mult == 1);
                checkError2(step == 1, "It is not allowed to use step for loop dimension mapped on non-block distributed axis");
                if (allInLocalIndexes) {
                    res[ax - 1].intersectInplace(dspaceInt - summLocal);
                } else {
                    Intervals havePart;
                    // Converting to DistribSpace global space
                    res[ax - 1] += summ;
                    // Converting to DistribSpace local space
                    axRule->fillPart(havePart, res[ax - 1], step);
                    havePart.intersect(dspaceInt);
                    // We do not allow multi-interval results and convert them to local distrib space's indexes
                    if (!havePart.empty())
                        res[ax - 1] = havePart.toInterval();
                    else
                        res[ax - 1][1] = res[ax - 1][0] - 1;
                    res[ax - 1] -= summLocal;
                }
                if (res[ax - 1].empty())
                    hasLocal = false;
            }
        }
    }
    if (!hasLocal) {
        for (int i = 0; i < rank; i++) {
            if (getDspaceAxis(i + 1) <= 0)
                res[i][1] = res[i][0] - 1;
        }
    }
    dvmh_log(TRACE, "Result (local, hasLocal=%d):", (int)hasLocal);
    custom_log(TRACE, blockOut, rank, res);
    return hasLocal;
}

bool DvmhAlignRule::mapOnPart(const Intervals dspacePart[], Intervals res[], const Interval spacePart[], const DvmType intervals[]) const {
    bool hasLocal = true;
    for (int i = 0; i < dspace->getRank(); i++) {
        bool axisHasLocal = false;
        for (int j = 0; j < dspacePart[i].getIntervalCount(); j++) {
            //Interval axisDspacePart = dspacePart[i].getInterval(j);
            // TODO: implement, maybe
            checkInternal2(false, "Not implemented");
        }
        if (!axisHasLocal)
            hasLocal = false;
    }
    return hasLocal;
}

void DvmhAlignRule::setDistribRule(const DvmhDistribRule *newDisRule) {
    disRule = newDisRule;
    assert(disRule->getRank() == dspace->getRank());
    for (int i = 0; i < dspace->getRank(); i++)
        assert(disRule->getAxisRule(i + 1)->getSpaceDim() == dspace->getSpace()[i]);
    for (int i = 0; i < rank; i++)
        dspaceAxis[i] = -1;
    hasIndirectFlag = false;
    for (int i = 0; i < dspace->getRank(); i++) {
        if (disRule->isIndirect(i + 1)) {
            checkError2(map[i].axisNumber == 0 || map[i].multiplier == 1,
                    "It is not allowed to use coefficient for linear rule on non-block distributed template axis");
            if (map[i].axisNumber > 0)
                hasIndirectFlag = true;
        }
        if (map[i].axisNumber > 0)
            dspaceAxis[map[i].axisNumber - 1] = i + 1;
    }
}

DvmhAlignRule::~DvmhAlignRule() {
    assert(map);
    delete[] map;
    assert(dspaceAxis);
    delete[] dspaceAxis;
    assert(dspace);
    dspace->removeRef();
}

}
