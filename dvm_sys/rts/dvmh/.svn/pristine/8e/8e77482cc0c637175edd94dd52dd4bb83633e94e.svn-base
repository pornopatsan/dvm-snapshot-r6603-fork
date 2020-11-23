#include "dvmh_data.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <set>

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_device.h"
#include "dvmh_pieces.h"
#include "dvmh_rts.h"
#include "dvmh_stat.h"
#include "mps.h"
#include "region.h"
#include "util.h"

namespace libdvmh {

// DvmhRepresentative

DvmhRepresentative::DvmhRepresentative(DvmhBuffer *aBuffer, bool owning) {
    buffer = aBuffer;
    assert(buffer);
    actualState = new DvmhPieces(buffer->getRank());
    cleanTransformState = false;
    ownBuffer = owning;
    if (dvmhSettings.pageLockHostMemory)
        buffer->pageLock();
}

bool DvmhRepresentative::ownsMemory() const {
    return ownsBuffer() && buffer->ownsMemory();
}

void DvmhRepresentative::doTransform(const int newAxisPerm[], int perDiagonalState) {
    int rank = buffer->getRank();
    if (rank <= 1)
        return;
    // Checking newAxisPerm
    if (newAxisPerm) {
#ifdef NON_CONST_AUTOS
        int hasAxes[rank];
#else
        int hasAxes[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < rank; i++)
            hasAxes[i] = 0;
        for (int i = 0; i < rank; i++) {
            checkInternal(newAxisPerm[i] >= 1 && newAxisPerm[i] <= rank);
            hasAxes[newAxisPerm[i] - 1] = 1;
        }
        for (int i = 0; i < rank; i++)
            checkInternal(hasAxes[i]);
    }
    checkInternal(perDiagonalState >= 0 && perDiagonalState <= 2);
    {
        char buf[300];
        char *s = buf;
        for (int i = 0; i < rank; i++)
            s += sprintf(s, "%d,", (newAxisPerm ? newAxisPerm[i] : i + 1));
        if (rank > 0)
            s--;
        *s = 0;
        dvmh_log(TRACE, "Requested new transformed state: newAxisPerm = (%s), perDiagonalState = %d", buf, perDiagonalState);
    }
    if (!cleanTransformState)
        buffer->doTransform(newAxisPerm, perDiagonalState);
    else
        buffer->setTransformState(newAxisPerm, perDiagonalState);
}

void DvmhRepresentative::undoDiagonal() {
    if (buffer->getDiagonalizedState() != 0)
        buffer->doTransform(buffer->getAxisPerm(), 0);
}

DvmhRepresentative::~DvmhRepresentative() {
    if (ownBuffer)
        delete buffer;
    delete actualState;
}

// DvmhDataState

bool DvmhDataState::operator<(const DvmhDataState &state) const {
    int prodRes = producer.compareTo(state.producer);
    if (prodRes < 0)
        return true;
    if (prodRes > 0)
        return false;
    if (readers.size() < state.readers.size())
        return true;
    if (readers.size() > state.readers.size())
        return false;
    std::set<DvmhRegVar>::const_iterator it2 = state.readers.begin();
    for (std::set<DvmhRegVar>::const_iterator it = readers.begin(); it != readers.end(); it++) {
        int res = it->compareTo(*it2);
        if (res < 0)
            return true;
        if (res > 0)
            return false;
        it2++;
    }
    return false;
}

// LocalizationInfo

void LocalizationInfo::removeFromReferences(const ReferenceDesc &desc) {
    bool found = false;
    for (int i = 0; i < (int)references.size(); i++) {
        if (references[i].data == desc.data) {
            assert(!found);
            assert(references[i].axis = desc.axis);
            std::swap(references[i], references.back());
            references.pop_back();
            found = true;
        }
    }
    assert(found);
}

// DvmhData

DvmhData::DvmhData(UDvmType aTypeSize, TypeType aTypeType, int aRank, const Interval aSpace[], const ShdWidth shadows[]) {
    typeSize = aTypeSize;
    setTypeType(aTypeType);
    finishInitialization(aRank, aSpace, shadows);
}

DvmhData::DvmhData(DataType aDataType, int aRank, const Interval aSpace[], const ShdWidth shadows[]) {
    dataType = aDataType;
    typeSize = getTypeSize(dataType);
    typeType = getTypeType(dataType);
    finishInitialization(aRank, aSpace, shadows);
}

DvmhData *DvmhData::fromRegularArray(DataType dt, void *regArr, DvmType len, DvmType baseIdx) {
    Interval space[1];
    space[0][0] = baseIdx;
    space[0][1] = baseIdx + len - 1;
    DvmhData *res = new DvmhData(dt, 1, space, 0);
    res->realign(0);
    res->setHostPortion(regArr, space);
    res->initActual(0);
    return res;
}

void DvmhData::setTypeType(TypeType tt) {
    typeType = tt;
    dataType = dtUnknown;
    for (int idt = 0; idt < DATA_TYPES; idt++) {
        DataType dt = (DataType)idt;
        if (typeType == getTypeType(dt) && typeSize == getTypeSize(dt)) {
            dataType = dt;
            break;
        }
    }
}

DvmhRepresentative *DvmhData::getRepr(int dev) {
    assert(dev >= 0 && dev < devicesCount);
    return representatives[dev];
}

DvmhBuffer *DvmhData::getBuffer(int dev) {
    DvmhRepresentative *repr = getRepr(dev);
    return (repr ? repr->getBuffer() : 0);
}

static void setBufferAdjustments(DvmhBuffer *buf, DvmhAlignRule *alignRule) {
    if (alignRule && alignRule->hasIndirect()) {
        for (int i = 1; i <= alignRule->getRank(); i++) {
            if (alignRule->isIndirect(i)) {
                int dspaceAxis = alignRule->getDspaceAxis(i);
                const DvmhAxisAlignRule *axRule = alignRule->getAxisRule(dspaceAxis);
                buf->setIndexAdjustment(i, axRule->summandLocal - axRule->summand);
            }
        }
    }
}

void DvmhData::setHostPortion(void *hostAddr, const Interval havePortion[]) {
    checkInternal2(hasLocal(), "Setting host portion for data without local part at all");
    DvmhPieces *saveActual = new DvmhPieces(rank);
    if (representatives[0])
        saveActual->append(representatives[0]->getActualState());
    deleteRepr(0);
    representatives[0] = new DvmhRepresentative(new DvmhBuffer(rank, typeSize, 0, havePortion, hostAddr));
    setBufferAdjustments(representatives[0]->getBuffer(), alignRule);
    representatives[0]->getActualState()->append(saveActual);
    delete saveActual;
    updateHeaders();
}

void DvmhData::addHeader(DvmType aHeader[], const void *base, bool owning) {
    assert(aHeader);
    HeaderDesc newHeader;
    newHeader.ptr = aHeader;
    newHeader.freeBase = base == 0;
    newHeader.own = owning;
    aHeader[rank + 2] = (DvmType)base;
    headers.push_back(newHeader);
    updateHeaders();
}

bool DvmhData::hasHeader(DvmType aHeader[]) const {
    for (int i = 0; i < (int)headers.size(); i++) {
        if (headers[i].ptr == aHeader)
            return true;
    }
    return false;
}

bool DvmhData::removeHeader(DvmType aHeader[]) {
    for (int i = 0; i < (int)headers.size(); i++) {
        if (headers[i].ptr == aHeader) {
            headers[i].ptr[0] = 0;
            if (headers[i].own) {
                delete[] headers[i].ptr;
            }
            if (i + 1 < (int)headers.size()) {
                headers[i] = headers.back();
            }
            headers.pop_back();
            return true;
        }
    }
    return false;
}

DvmType *DvmhData::getAnyHeader(bool nullIfNone) const {
    if (headers.empty()) {
        checkInternal2(nullIfNone, "No headers for the distributed array available");
        return 0;
    }
    return headers[0].ptr;
}

void DvmhData::createHeader() {
    addHeader(new DvmType[rank + 3], 0, true);
}

void DvmhData::initActual(int dev) {
    assert(dev >= -1 && dev < devicesCount);
    if (dev >= 0)
        assert(representatives[dev]);
    if (rank > 0 && space[0].empty())
        dev = -1;
    initShadowProfile();
    for (int i = 0; i < devicesCount; i++) {
        DvmhRepresentative *repr = representatives[i];
        if (repr) {
            DvmhPieces *aState = repr->getActualState();
            aState->clear();
            if (i == dev) {
                if (rank > 0) {
                    aState->appendOne(localPlusShadow);
                    DvmhPieces *p = applyShadowProfile(aState, localPart);
                    aState->clear();
                    aState->append(p);
                    delete p;
                } else
                    aState->appendOne(repr->getBuffer()->getHavePortion());
            }
        }
    }
}

void DvmhData::initActualShadow() {
    initShadowProfile();
    if (hasLocalFlag) {
        DvmhPieces *toIntersect = new DvmhPieces(rank);
        toIntersect->appendOne(localPlusShadow, maxOrder);
        toIntersect->uniteOne(localPart);
        for (int i = 0; i < devicesCount; i++) {
            if (representatives[i]) {
#ifdef NON_CONST_AUTOS
                Interval block[rank];
#else
                Interval block[MAX_ARRAY_RANK];
#endif
                representatives[i]->getActualState()->intersectInplace(toIntersect);
                if (localPlusShadow->blockIntersect(rank, representatives[i]->getBuffer()->getHavePortion(), block))
                    representatives[i]->getActualState()->uniteOne(block, maxOrder);
            }
        }
    }
}

DvmhRepresentative *DvmhData::createNewRepr(int dev, const Interval havePortion[]) {
    deleteRepr(dev);
    representatives[dev] = new DvmhRepresentative(new DvmhBuffer(rank, typeSize, dev, havePortion));
    setBufferAdjustments(representatives[dev]->getBuffer(), alignRule);
    if (dev == 0)
        updateHeaders();
    return representatives[dev];
}

void DvmhData::deleteRepr(int device) {
    DvmhRepresentative *repr = representatives[device];
    if (repr) {
        dvmh_log(TRACE, "Deleting representative on device %d", device);
        delete repr;
    }
    representatives[device] = 0;
}

void DvmhData::extendBlock(int rank, const Interval block[], const ShdWidth shadow[], const Interval bounds[], Interval result[]) {
    for (int i = 0; i < rank; i++) {
        if (!block[i].empty()) {
            result[i][0] = std::max(block[i][0] - (shadow ? shadow[i][0] : 0), bounds[i][0]);
            result[i][1] = std::min(block[i][1] + (shadow ? shadow[i][1] : 0), bounds[i][1]);
        } else {
            result[i] = block[i];
        }
    }
}

void DvmhData::expandIncomplete(DvmType newHigh) {
    assert(isIncomplete() && newHigh >= space[0][0]);
    assert(!isDistributed());
    assert(hasLocal());
    bool allHeadersAreOwn = true;
    for (int i = 0; i < (int)headers.size(); i++)
        allHeadersAreOwn = allHeadersAreOwn && headers[i].own;
    assert(allHeadersAreOwn);
    space[0][1] = newHigh;
    localPart[0][1] = newHigh;
    recalcLocalPlusShadow();
    void *addr = representatives[0]->getBuffer()->getDeviceAddr();
    syncAllAccesses();
    setHostPortion(addr, space);
    initActual(0);
}

void DvmhData::getActualBase(int dev, DvmhPieces *p, const Interval curLocalPart[], bool addToActual) {
    if (curLocalPart) {
        DvmhPieces *tmp = applyShadowProfile(p, curLocalPart);
        performGetActual(dev, tmp, addToActual);
        delete tmp;
    } else
        performGetActual(dev, p, addToActual);
}

void DvmhData::getActualBaseOne(int dev, const Interval realBlock[], const Interval curLocalPart[], bool addToActual) {
    // realBlock must be not bigger than data->space
    DvmhPieces *p = new DvmhPieces(rank);
    p->appendOne(realBlock);
    getActualBase(dev, p, curLocalPart, addToActual);
    delete p;
}

void DvmhData::getActual(const Interval indexes[], bool addToActual) {
    if (hasLocal()) {
        getActualBaseOne(0, indexes, localPart, addToActual);
        curState.addReader(DvmhRegVar());
    }
}

struct BlockAccumulator: private Uncopyable {
    DvmhPieces *p;
    DvmType order;
    explicit BlockAccumulator(int rank, DvmType aOrder = ABS_ORDER): order(aOrder) { p = new DvmhPieces(rank); }
    void operator()(const Interval realBlock[]) { p->appendOne(realBlock, order); }
    ~BlockAccumulator() { delete p; }
};

template <typename T>
static void doWithEveryEdge(int rank, const Interval localPart[], const Interval bounds[], const ShdWidth shdWidths[], T &f) {
    // Inner shadow edges
#ifdef NON_CONST_AUTOS
    Interval realBlock[rank];
#else
    Interval realBlock[MAX_ARRAY_RANK];
#endif
    realBlock->blockAssign(rank, localPart);
    for (int j = 0; j < rank; j++) {
        Interval leftPart;
        leftPart[0] = localPart[j][0];
        leftPart[1] = localPart[j][1];
        if (shdWidths[j][1] > 0 && localPart[j][0] > bounds[j][0]) {
            realBlock[j][0] = localPart[j][0];
            realBlock[j][1] = std::min(realBlock[j][0] + shdWidths[j][1] - 1, localPart[j][1]);
            f(realBlock);
            leftPart[0] = realBlock[j][1] + 1;
        }

        if (shdWidths[j][0] > 0 && localPart[j][1] < bounds[j][1]) {
            realBlock[j][1] = localPart[j][1];
            realBlock[j][0] = std::max(realBlock[j][1] - (shdWidths[j][0] - 1), localPart[j][0]);
            if (realBlock[j][0] < leftPart[0])
                realBlock[j][0] = leftPart[0];
            if (realBlock[j][0] <= realBlock[j][1]) {
                f(realBlock);
            }
            leftPart[1] = realBlock[j][0] - 1;
        }

        if (leftPart[0] > leftPart[1])
            break;
        realBlock[j] = leftPart;
    }
}

void DvmhData::getActualEdges(const Interval aLocalPart[], const ShdWidth shdWidths[], bool addToActual) {
    assert(hasLocal());
    BlockAccumulator edgesAccumulator(rank);
    doWithEveryEdge(rank, aLocalPart, space, shdWidths, edgesAccumulator);
    getActualBase(0, edgesAccumulator.p, 0, addToActual);
}

void DvmhData::getActualIndirectEdges(int axis, const std::string &shadowName, bool addToActual) {
    int dspaceAxis = alignRule->getDspaceAxis(axis);
    IndirectAxisDistribRule *distrRule = dspaceAxis > 0 ? alignRule->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect() : 0;
    assert(distrRule);
    const Intervals &shdIndexes = distrRule->getShadowOwnL1Indexes(shadowName);
    const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(dspaceAxis);
#ifdef NON_CONST_AUTOS
    Interval block[rank];
#else
    Interval block[MAX_ARRAY_RANK];
#endif
    block->blockAssign(rank, localPart);
    for (int i = 0; i < shdIndexes.getIntervalCount(); i++) {
        Interval curInterval = shdIndexes.getInterval(i);
        curInterval -= alRule->summandLocal;
        block[axis - 1] = curInterval;
        getActualBaseOne(0, block, 0, addToActual);
    }
}

template <typename T>
static void doWithEveryShadow(int rank, const Interval localPart[], const Interval bounds[], const ShdWidth shdWidths[], bool cornersFlag, T &f) {
    // Shadow edges with or without corners
#ifdef NON_CONST_AUTOS
    Interval realBlock[rank];
#else
    Interval realBlock[MAX_ARRAY_RANK];
#endif
    if (cornersFlag)
        DvmhData::extendBlock(rank, localPart, shdWidths, bounds, realBlock);
    else
        realBlock->blockAssign(rank, localPart);

    for (int j = 0; j < rank; j++) {
        if (shdWidths[j][0] > 0 && localPart[j][0] > bounds[j][0]) {
            realBlock[j][0] = std::max(localPart[j][0] - shdWidths[j][0], bounds[j][0]);
            realBlock[j][1] = localPart[j][0] - 1;
            f(realBlock);
        }

        if (shdWidths[j][1] > 0 && localPart[j][1] < bounds[j][1]) {
            realBlock[j][1] = std::min(localPart[j][1] + shdWidths[j][1], bounds[j][1]);
            realBlock[j][0] = localPart[j][1] + 1;
            f(realBlock);
        }

        realBlock[j] = localPart[j];
    }
}

template <typename T>
static void doWithEveryShadow(const DvmhAlignRule *alignRule, const Interval localPart[], int axis, const std::string &name, T &f) {
    // Indirect shadow edges
    int rank = alignRule->getRank();
#ifdef NON_CONST_AUTOS
    Interval realBlock[rank];
#else
    Interval realBlock[MAX_ARRAY_RANK];
#endif
    realBlock->blockAssign(rank, localPart);
    int dspaceAxis = alignRule->getDspaceAxis(axis);
    IndirectAxisDistribRule *distrRule = dspaceAxis > 0 ? alignRule->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect() : 0;
    assert(distrRule);
    Intervals shdIndexes;
    bool legalName = distrRule->fillShadowL2Indexes(shdIndexes, name);
    assert(legalName);
    const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(dspaceAxis);
    for (int i = 0; i < shdIndexes.getIntervalCount(); i++) {
        realBlock[axis - 1] = shdIndexes.getInterval(i) - alRule->summandLocal;
        f(realBlock);
    }
}

void DvmhData::getActualShadow(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[], bool addToActual) {
    assert(hasLocal());
    dvmh_log(TRACE, "Requested to get actual shadow widths:");
    custom_log(TRACE, blockOut, rank, (Interval *)curShdWidths);
    BlockAccumulator shadowsAccumulator(rank);
    doWithEveryShadow(rank, curLocalPart, localPlusShadow, curShdWidths, cornerFlag, shadowsAccumulator);
    getActualBase(dev, shadowsAccumulator.p, curLocalPart, addToActual);
}

void DvmhData::clearActual(DvmhPieces *p, int exceptDev) {
    if (!p->isEmpty()) {
        DvmhPieces *tmp = 0;
        for (int i = 0; i < devicesCount; i++) {
            if (representatives[i] && i != exceptDev) {
                if (!tmp) {
                    tmp = new DvmhPieces(rank);
                    for (int i = 0; i < p->getCount(); i++)
                        tmp->appendOne(p->getPiece(i, 0), maxOrder);
                }
                dvmh_log(TRACE, "Clearing actual from device %d. What to clear:", i);
                custom_log(TRACE, piecesOut, tmp);
                dvmh_log(TRACE, "Was:");
                custom_log(TRACE, piecesOut, representatives[i]->getActualState());
                representatives[i]->getActualState()->subtractInplace(tmp, FROM_ABS);
                dvmh_log(TRACE, "Now:");
                custom_log(TRACE, piecesOut, representatives[i]->getActualState());
            }
        }
        delete tmp;
    }
}

void DvmhData::clearActualOne(const Interval piece[], int exceptDev) {
    DvmhPieces *tmp = 0;
    for (int i = 0; i < devicesCount; i++) {
        if (representatives[i] && i != exceptDev) {
            if (!tmp) {
                tmp = new DvmhPieces(rank);
                tmp->appendOne(piece, maxOrder);
            }
            dvmh_log(TRACE, "Clearing actual from device %d. What to clear:", i);
            custom_log(TRACE, piecesOut, tmp);
            dvmh_log(TRACE, "Was:");
            custom_log(TRACE, piecesOut, representatives[i]->getActualState());
            representatives[i]->getActualState()->subtractInplace(tmp, FROM_ABS);
            dvmh_log(TRACE, "Now:");
            custom_log(TRACE, piecesOut, representatives[i]->getActualState());
        }
    }
    delete tmp;
}

void DvmhData::clearActualShadow(const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[], int exceptDev) {
    BlockAccumulator shadowsAccumulator(rank);
    doWithEveryShadow(rank, curLocalPart, localPlusShadow, curShdWidths, cornerFlag, shadowsAccumulator);
    clearActual(shadowsAccumulator.p, exceptDev);
}

void DvmhData::performSetActual(int dev, DvmhPieces *p) {
    assert(representatives[dev]);
    clearActual(p, dev);
    representatives[dev]->getActualState()->unite(p);
}

void DvmhData::performSetActualOne(int dev, const Interval realBlock[]) {
    checkInternal((rank == 0) >= (realBlock == 0));
    assert(representatives[dev]);
    clearActualOne(realBlock, dev);
    representatives[dev]->getActualState()->uniteOne(realBlock);
}

void DvmhData::setActual(DvmhPieces *p) {
    if (hasLocal()) {
        performSetActual(0, p);
        curState = DvmhDataState();
    }
}

void DvmhData::setActual(const Interval indexes[]) {
    if (hasLocal()) {
        performSetActualOne(0, indexes);
        curState = DvmhDataState();
    }
}

void DvmhData::setActualEdges(int dev, const Interval curLocalPart[], const ShdWidth curShdWidths[], DvmhPieces **piecesDone) {
    BlockAccumulator edgesAccumulator(rank);
    doWithEveryEdge(rank, curLocalPart, localPlusShadow, curShdWidths, edgesAccumulator);
    performSetActual(dev, edgesAccumulator.p);
    if (piecesDone)
        (*piecesDone)->unite(edgesAccumulator.p);
}

void DvmhData::setActualShadow(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[], DvmhPieces **piecesDone) {
    BlockAccumulator shadowsAccumulator(rank);
    doWithEveryShadow(rank, curLocalPart, localPlusShadow, curShdWidths, cornerFlag, shadowsAccumulator);
    performSetActual(dev, shadowsAccumulator.p);
    if (piecesDone)
        (*piecesDone)->unite(shadowsAccumulator.p);
}

void DvmhData::setActualIndirectShadow(int dev, int axis, const std::string &shadowName) {
    BlockAccumulator shadowsAccumulator(rank);
    doWithEveryShadow(alignRule, localPart, axis, shadowName, shadowsAccumulator);
    performSetActual(dev, shadowsAccumulator.p);
}

void DvmhData::shadowComputed(int dev, const Interval curLocalPart[], bool cornerFlag, const ShdWidth curShdWidths[]) {
    BlockAccumulator shadowsAccumulator(rank, maxOrder + 1);
    doWithEveryShadow(rank, curLocalPart, localPlusShadow, curShdWidths, cornerFlag, shadowsAccumulator);
    representatives[dev]->getActualState()->unite(shadowsAccumulator.p);
}

void DvmhData::updateShadowProfile(bool cornerFlag, ShdWidth curShdWidths[]) {
    maxOrder++;
#ifdef NON_CONST_AUTOS
    Interval curLocalPart[rank], curBounds[rank];
#else
    Interval curLocalPart[MAX_ARRAY_RANK], curBounds[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        curLocalPart[i][0] = 0;
        curLocalPart[i][1] = 0;
        curBounds[i][0] = -shdWidths[i][0];
        curBounds[i][1] = shdWidths[i][1];
    }
    BlockAccumulator shadowsAccumulator(rank, maxOrder);
    doWithEveryShadow(rank, curLocalPart, curBounds, curShdWidths, cornerFlag, shadowsAccumulator);
    shadowProfile->unite(shadowsAccumulator.p);
    dvmh_log(TRACE, "New maxOrder=" DTFMT " and new shadow profile:", maxOrder);
    custom_log(TRACE, piecesOut, shadowProfile);
}

void DvmhData::updateIndirectShadowProfile(int axis, const std::string &shadowName) {
    maxOrder++;
#ifdef NON_CONST_AUTOS
    Interval block[rank];
#else
    Interval block[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        block[i][0] = 0;
        block[i][1] = 0;
    }
    int dspaceAxis = alignRule->getDspaceAxis(axis);
    IndirectAxisDistribRule *distrRule = dspaceAxis > 0 ? alignRule->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect() : 0;
    assert(distrRule);
    Intervals shdIndexes;
    bool legalName = distrRule->fillShadowL2Indexes(shdIndexes, shadowName);
    assert(legalName);
    const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(dspaceAxis);
    for (int i = 0; i < shdIndexes.getIntervalCount(); i++) {
        block[axis - 1] = shdIndexes.getInterval(i) - alRule->summandLocal;
        if (block[axis - 1] < localPart[axis - 1][0])
            block[axis - 1] -= localPart[axis - 1][0];
        else
            block[axis - 1] -= localPart[axis - 1][1];
        shadowProfile->uniteOne(block, maxOrder);
    }
}

UDvmType DvmhData::getTotalElemCount() const {
    UDvmType elemCount = 1;
    for (int i = 0; i < rank; i++)
        elemCount *= space[i].size();
    return elemCount;
}

UDvmType DvmhData::getLocalElemCount() const {
    UDvmType elemCount = 1;
    for (int i = 0; i < rank; i++)
        elemCount *= localPart[i].size();
    return elemCount;
}

static void adjustAlignRuleAndLocalPart(DvmhAlignRule *alignRule, Interval localPart[], const Interval space[]) {
    // mapOnPart returns alignee's local indexes in case of indirect distributions, but uses maybe wrong summandLocal, we adjust it here
    for (int i = 1; i <= alignRule->getRank(); i++) {
        if (alignRule->isIndirect(i)) {
            int dspaceAxis = alignRule->getDspaceAxis(i);
            assert(dspaceAxis > 0);
            const IndirectAxisDistribRule *axRule = alignRule->getDistribRule()->getAxisRule(dspaceAxis)->asIndirect();
            assert(axRule);
            if (space[i - 1].size() != axRule->getSpaceDim().size()) {
                // Need to compactify indexes to fit in the original space
                MPSAxis mpsAxis = axRule->getMPS()->getAxis(axRule->getMPSAxis());
                UDvmType *elemCounts = new UDvmType[mpsAxis.procCount];
                elemCounts[mpsAxis.ourProc] = localPart[i - 1].size();
                mpsAxis.axisComm->allgather(elemCounts);
                UDvmType elemsBefore = 0;
                for (int j = 0; j < mpsAxis.ourProc; j++)
                    elemsBefore += elemCounts[j];
                delete[] elemCounts;
                DvmType newLocalStart = space[i - 1][0] + (DvmType)elemsBefore;
                localPart[i - 1] += alignRule->getAxisRule(dspaceAxis)->summandLocal;
                alignRule->getAxisRule(dspaceAxis)->summandLocal = localPart[i - 1][0] - newLocalStart;
                localPart[i - 1] -= alignRule->getAxisRule(dspaceAxis)->summandLocal;
            }
        }
        checkInternal(space[i - 1].contains(localPart[i - 1]));
    }
}

void DvmhData::realign(DvmhAlignRule *newAlignRule, bool ownDistribSpace, bool newValueFlag) {
    if (!newAlignRule) {
        assert(!isDistributed());
        localPart->blockAssign(rank, space);
        localPlusShadow->blockAssign(rank, space);
        hasLocalFlag = true;
    } else {
        assert(!isAligned() || isDistributed());
        bool realignFlag = isDistributed();
        if (!realignFlag) {
            assert(!isAligned());
            for (int i = 0; i < devicesCount; i++)
                assert(representatives[i] == 0);
            for (int i = 1; i <= rank; i++) {
                if (newAlignRule->isIndirect(i)) {
                    shdWidths[i - 1][0] = 0;
                    shdWidths[i - 1][1] = 0;
                }
            }
            initShadowProfile();
            assert(!alignRule);
            alignRule = newAlignRule;
            localPart->blockAssign(rank, space);
            hasLocalFlag = alignRule->mapOnPart(alignRule->getDspace()->getLocalPart(), localPart, false);
            adjustAlignRuleAndLocalPart(alignRule, localPart, space);
            recalcLocalPlusShadow();
        } else {
            redistributeCommon(newAlignRule, newValueFlag);
        }
        alignRule->getDspace()->addAlignedData(this);
        ownDspace = ownDistribSpace;
    }
}

void DvmhData::redistribute(const DvmhDistribRule *newDistribRule) {
    assert(isDistributed());
    DvmhAlignRule *newAlignRule = new DvmhAlignRule(*alignRule);
    newAlignRule->setDistribRule(newDistribRule);
    redistributeCommon(newAlignRule);
}

bool DvmhData::hasElement(const DvmType indexes[]) const {
    if (!hasLocal())
        return false;
    bool res = true;
    for (int i = 0; i < rank; i++)
        if (!localPart[i].contains(indexes[i]))
            res = false;
    return res;
}

bool DvmhData::convertGlobalToLocal2(DvmType indexes[], bool onlyFromLocalPart) const {
    bool res = true;
    if (isDistributed() && alignRule->hasIndirect()) {
        int dspaceRank = alignRule->getDspace()->getRank();
        for (int i = 1; i <= dspaceRank; i++) {
            const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(i);
            const IndirectAxisDistribRule *disRule = alignRule->getDspace()->getAxisDistribRule(i)->asIndirect();
            int ax = alRule->axisNumber;
            if (disRule && ax > 0) {
                bool isLocal = false, isShadow = false;
                DvmType localIdx;
                if (onlyFromLocalPart)
                    localIdx = disRule->globalToLocalOwn(indexes[ax - 1] * alRule->multiplier + alRule->summand, &isLocal);
                else
                    localIdx = disRule->globalToLocal2(indexes[ax - 1] * alRule->multiplier + alRule->summand, &isLocal, &isShadow);
                if (isLocal || isShadow) {
                    indexes[ax - 1] = localIdx - alRule->summandLocal;
                } else {
                    res = false;
                }
            }
        }
    }
    return res;
}

bool DvmhData::convertLocal2ToGlobal(DvmType indexes[], bool onlyFromLocalPart) const {
    bool res = true;
    if (isDistributed() && alignRule->hasIndirect()) {
        int dspaceRank = alignRule->getDspace()->getRank();
        for (int i = 1; i <= dspaceRank; i++) {
            const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(i);
            const IndirectAxisDistribRule *disRule = alignRule->getDspace()->getAxisDistribRule(i)->asIndirect();
            int ax = alRule->axisNumber;
            if (disRule && ax > 0) {
                bool isLocal = false, isShadow = false;
                DvmType templLocalIndex = indexes[ax - 1] + alRule->summandLocal;
                DvmType templGlobalIndex = disRule->convertLocal2ToGlobal(templLocalIndex, &isLocal, &isShadow);
                if (isLocal || (isShadow && !onlyFromLocalPart)) {
                    indexes[ax - 1] = (templGlobalIndex - alRule->summand) / alRule->multiplier;
                } else {
                    res = false;
                }
            }
        }
    }
    return res;
}

bool DvmhData::hasIndirectShadow(int axis, const std::string &shadowName) const {
    assert(axis >= 1 && axis <= rank);
    if (!indirectShadows)
        return false;
    return indirectShadows[axis - 1].find(shadowName) != indirectShadows[axis - 1].end();
}

void DvmhData::includeIndirectShadow(int axis, const std::string &shadowName) {
    checkError2(isDistributed(), "Array must be distributed");
    if (hasIndirectShadow(axis, shadowName))
        return;
    int dspaceAxis = alignRule->getDspaceAxis(axis);
    IndirectAxisDistribRule *distrRule = dspaceAxis > 0 ? alignRule->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect() : 0;
    checkError2(distrRule, "Specified distributed array axis is not aligned with indirectly-distributed template axis");
    if (distrRule->getSpaceDim().size() != space[axis - 1].size())
        dvmh_log(WARNING, "Shortened distributed array space is not fully supported for now for indirectly distributed axis with shadow edges");
    Intervals shdIndexes;
    bool legalName = distrRule->fillShadowL2Indexes(shdIndexes, shadowName);
    checkError3(legalName, "Specified shadow edge name '%s' is not found", shadowName.c_str());
    if (!shdIndexes.empty()) {
        const DvmhAxisAlignRule *alRule = alignRule->getAxisRule(dspaceAxis);
        Interval boundInterval = shdIndexes.getBoundInterval();
        boundInterval -= alRule->summandLocal;
        checkError2(space[axis - 1].contains(boundInterval),
                "Failed to add shadow edges. Consider declaring a distributed array of the same size as the template.");
        ShdWidth newWidth;
        newWidth[0] = std::max(shdWidths[axis - 1][0], localPart[axis - 1][0] - boundInterval[0]);
        newWidth[1] = std::max(shdWidths[axis - 1][1], boundInterval[1] - localPart[axis - 1][1]);
        changeShadowWidth(axis, newWidth);
    }
    if (!indirectShadows)
        indirectShadows = new std::set<std::string>[rank];
    indirectShadows[axis - 1].insert(shadowName);
}

void DvmhData::localizeAsReferenceFor(DvmhData *targetData, int targetAxis) {
    assert(dataType == dtInt || dataType == dtLong || dataType == dtLongLong);
    if (localizationInfo.target != ReferenceDesc(targetData, targetAxis)) {
        unlocalizeValues();
        localizeValues(ReferenceDesc(targetData, targetAxis));
    }
}

DvmhEvent *DvmhData::enqueueWriteAccess(DvmhEvent *accessEnd, bool owning) {
    AggregateEvent *res = new AggregateEvent;
    res->addEvent(latestWriterEnd);
    res->addEvent(latestReadersEnd);
    if (owning)
        latestWriterEnd = accessEnd;
    else
        latestWriterEnd = accessEnd->dup();
    latestReadersEnd = new AggregateEvent;
    return res;
}

DvmhEvent *DvmhData::enqueueReadAccess(DvmhEvent *accessEnd, bool owning) {
    if (owning)
        latestReadersEnd->addEvent(accessEnd);
    else
        latestReadersEnd->addEvent(accessEnd->dup());
    return latestWriterEnd->dup();
}

void DvmhData::syncWriteAccesses() {
    latestWriterEnd->wait();
}

void DvmhData::syncAllAccesses() {
    latestWriterEnd->wait();
    latestReadersEnd->wait();
}

bool DvmhData::mpsCanRead(const MultiprocessorSystem *mps) const {
    // TODO: Check distribution to figure out if current MPS is sufficient to access all the data through local parts
    return !isDistributed() || alignRule->getDspace()->getDistribRule()->getMPS()->isSubsystemOf(mps);
}

bool DvmhData::mpsCanWrite(const MultiprocessorSystem *mps) const {
    return !isDistributed() || alignRule->getDspace()->getDistribRule()->getMPS()->isSubsystemOf(mps);
}

bool DvmhData::fillLocalPart(int proc, Interval res[]) const {
    bool ret = false;
    if (!isDistributed()) {
        res->blockAssign(rank, localPart);
        ret = hasLocalFlag;
    } else if (proc == alignRule->getDspace()->getDistribRule()->getMPS()->getCommRank()) {
        res->blockAssign(rank, localPart);
        ret = hasLocalFlag;
    } else {
#ifdef NON_CONST_AUTOS
        Interval dspacePart[alignRule->getDspace()->getRank()];
#else
        Interval dspacePart[MAX_DISTRIB_SPACE_RANK];
#endif
        alignRule->getDspace()->getDistribRule()->fillLocalPart(proc, dspacePart);
        res->blockAssign(rank, space);
        ret = alignRule->mapOnPart(dspacePart, res, false);
    }
    return ret;
}

DvmhData::~DvmhData() {
    syncAllAccesses();
    for (int i = 0; i < devicesCount; i++)
        deleteRepr(i);
    delete[] representatives;
    if (alignRule) {
        DvmhDistribSpace *dspace = alignRule->getDspace();
        dspace->removeAlignedData(this);
        delete alignRule;
        if (ownDspace)
            delete dspace;
    }
    for (int i = 0; i < (int)headers.size(); i++) {
        headers[i].ptr[0] = 0;
        if (headers[i].own)
            delete[] headers[i].ptr;
    }
    assert(shdWidths);
    delete[] shdWidths;
    delete[] indirectShadows;
    assert(space);
    delete[] space;
    assert(localPart);
    delete[] localPart;
    assert(localPlusShadow);
    delete[] localPlusShadow;
    delete shadowProfile;
    assert(localizationInfo.references.empty());
    if (localizationInfo.target.isValid())
        localizationInfo.target.data->localizationInfo.removeFromReferences(ReferenceDesc(this, localizationInfo.target.axis));
    delete latestWriterEnd;
    delete latestReadersEnd;
}

void DvmhData::finishInitialization(int aRank, const Interval aSpace[], const ShdWidth shadows[]) {
    rank = aRank;
    checkInternal2(rank >= 0, "Incorrect rank");
    space = new Interval[rank];
    shdWidths = new ShdWidth[rank];
    for (int i = 0; i < rank; i++) {
        space[i] = aSpace[i];
        assert(!space[i].empty() || (!shadows && i == 0));
    }
    indirectShadows = 0;
    hasLocalFlag = false;
    localPart = new Interval[rank];
    localPlusShadow = new Interval[rank];
    for (int i = 0; i < rank; i++) {
        localPart[i] = Interval::createEmpty();
        localPlusShadow[i] = Interval::createEmpty();
    }
    alignRule = 0;
    representatives = new DvmhRepresentative *[devicesCount];
    for (int i = 0; i < devicesCount; i++)
        representatives[i] = 0;
    for (int i = 0; i < rank; i++) {
        shdWidths[i][0] = (shadows ? shadows[i][0] : 0);
        shdWidths[i][1] = (shadows ? shadows[i][1] : 0);
    }
    shadowProfile = 0;
    initShadowProfile();
    ownDspace = false;
    latestWriterEnd = new AggregateEvent; // Will remain empty
    latestReadersEnd = new AggregateEvent;
}

void DvmhData::initShadowProfile() {
    maxOrder = 1;
#ifdef NON_CONST_AUTOS
    Interval shdRect[rank];
#else
    Interval shdRect[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        shdRect[i][0] = -shdWidths[i][0];
        shdRect[i][1] = shdWidths[i][1];
    }
    delete shadowProfile;
    shadowProfile = new DvmhPieces(rank);
    shadowProfile->appendOne(shdRect, maxOrder);
    for (int i = 0; i < rank; i++) {
        shdRect[i][0] = 0;
        shdRect[i][1] = 0;
    }
    shadowProfile->subtractOne(shdRect);
}

void DvmhData::recalcLocalPlusShadow() {
    extendBlock(rank, localPart, shdWidths, space, localPlusShadow);
}

DvmhPieces *DvmhData::applyShadowProfile(DvmhPieces *p, const Interval curLocalPart[]) const {
    DvmhPieces *sizedProfile = new DvmhPieces(rank);
    sizedProfile->appendOne(curLocalPart);
    for (int i = 0; i < shadowProfile->getCount(); i++) {
#ifdef NON_CONST_AUTOS
        Interval sizedInterval[rank];
#else
        Interval sizedInterval[MAX_ARRAY_RANK];
#endif
        DvmType order;
        const Interval *origInterval = shadowProfile->getPiece(i, &order);
        for (int j = 0; j < rank; j++) {
            int side = origInterval[j][0] > 0;
            sizedInterval[j][0] = origInterval[j][0] + curLocalPart[j][side];
            side = origInterval[j][1] >= 0;
            sizedInterval[j][1] = origInterval[j][1] + curLocalPart[j][side];
        }
        sizedProfile->appendOne(sizedInterval, order);
    }
    sizedProfile->intersectInplace(p);
    return sizedProfile;
}

void DvmhData::performCopy(int dev1, int dev2, const Interval aCutting[]) const {
    if (dev1 != dev2) {
        DvmhRepresentative *repr1, *repr2;
        repr1 = representatives[dev1];
        repr2 = representatives[dev2];
        assert(repr1 && repr2);
        repr1->getBuffer()->copyTo(repr2->getBuffer(), aCutting);
        repr2->setCleanTransformState(false);
    } else {
        dvmh_log(DEBUG, "Something strange happened. Called copy to copy self to self. Doing nothing.");
    }
}

static DvmhPieces *tryToRoundPiece(int rank, DvmhRepresentative *repr1, DvmhRepresentative *repr2, const Interval cutting[], Interval mCutting[]) {
    DvmhPieces *res = 0;
    DvmhBuffer *buff1 = repr1->getBuffer();
    DvmhBuffer *buff2 = repr2->getBuffer();
    mCutting->blockAssign(rank, cutting);
    for (int i = rank - 1; i >= 1; i--) {
        bool flag = true;
        if (cutting[i].extend(4).contains(buff1->getHavePortion()[i]) && buff1->getHavePortion()[i] == buff2->getHavePortion()[i]) {
            if (cutting[i] != buff1->getHavePortion()[i]) {
                mCutting[i] = buff1->getHavePortion()[i];
                DvmhPieces *mcp = new DvmhPieces(rank);
                mcp->appendOne(mCutting);
                DvmhPieces *p1 = repr1->getActualState()->intersect(mcp);
                DvmhPieces *p2 = repr2->getActualState()->intersect(mcp);
                delete mcp;
                p2->subtractInplace(p1);
                if (p2->isEmpty()) {
                    delete res;
                    res = p1;
                } else {
                    delete p1;
                    mCutting[i] = cutting[i];
                    flag = false;
                }
                delete p2;
            }
        } else {
            flag = false;
        }
        if (!flag)
            break;
    }
    if (res) {
        dvmh_log(TRACE, "rounded cutting is");
        custom_log(TRACE, blockOut, rank, mCutting);
    }
    return res;
}

void DvmhData::performGetActual(int dev, DvmhPieces *p, bool addToActual) {
    checkInternal(dev >= 0 && dev < devicesCount && representatives[dev]);
    DvmhPieces *pieces2 = p->subtract(representatives[dev]->getActualState());
    pieces2->compactify();
    for (int i = 0; pieces2->getCount() > 0 && i < devicesCount; i++)
        if (representatives[i] && i != dev) {
            dvmh_log(TRACE, "pieces rest to actualize to device #%d", dev);
            custom_log(TRACE, piecesOut, pieces2);
            dvmh_log(TRACE, "pieces has device #%d", i);
            custom_log(TRACE, piecesOut, representatives[i]->getActualState());
            DvmhPieces *pieces1 = pieces2->intersect(representatives[i]->getActualState(), SET_NOT_LESS);
            pieces1->compactify();
            dvmh_log(TRACE, "pieces that can be actualized");
            custom_log(TRACE, piecesOut, pieces1);
            int count = pieces1->getCount();
            DvmhPieces *copiedPcs = new DvmhPieces(rank);
            for (int j = 0; j < count; j++) {
#ifdef NON_CONST_AUTOS
                Interval roundedPiece[rank];
#else
                Interval roundedPiece[MAX_ARRAY_RANK];
#endif
                DvmType order;
                DvmhPieces *toAdd = tryToRoundPiece(rank, representatives[i], representatives[dev], pieces1->getPiece(j, &order),
                        roundedPiece);
                performCopy(i, dev, roundedPiece);
                if (toAdd) {
                    copiedPcs->unite(toAdd);
                    delete toAdd;
                } else
                    copiedPcs->uniteOne(roundedPiece, order);
            }
            pieces2->subtractInplace(copiedPcs);
            if (addToActual)
                representatives[dev]->getActualState()->unite(copiedPcs);
            delete pieces1;
            delete copiedPcs;
        }
    if (pieces2->getCount() > 0)
        dvmh_log(WARNING, "Can not actualize whole requested block to representative on device %d", dev);
    delete pieces2;
}

void DvmhData::redistributeCommon(DvmhAlignRule *newAlignRule, bool newValueFlag) {
    assert(isDistributed());
    assert(newAlignRule);
    assert(newAlignRule != alignRule);
    bool doComm = !newValueFlag;
    if (!noLibdvm && !alignRule->hasIndirect() && !newAlignRule->hasIndirect())
        doComm = false;
    int newDspaceRank = newAlignRule->getDspace()->getRank();
    int oldDspaceRank = alignRule->getDspace()->getRank();
#ifdef NON_CONST_AUTOS
    Interval newLocalPart[rank], newLocalPlusShadow[rank];
    ShdWidth newShdWidths[rank];
#else
    Interval newLocalPart[MAX_ARRAY_RANK], newLocalPlusShadow[MAX_ARRAY_RANK];
    ShdWidth newShdWidths[MAX_ARRAY_RANK];
#endif
    typedMemcpy(newShdWidths, shdWidths, rank);
    for (int i = 1; i <= rank; i++) {
        if (alignRule->isIndirect(i) || newAlignRule->isIndirect(i)) {
            newShdWidths[i - 1][0] = 0;
            newShdWidths[i - 1][1] = 0;
        }
    }
    newLocalPart->blockAssign(rank, space);
    bool newHasLocal = newAlignRule->mapOnPart(newAlignRule->getDistribRule()->getLocalPart(), newLocalPart, false);
    adjustAlignRuleAndLocalPart(newAlignRule, newLocalPart, space);
    extendBlock(rank, newLocalPart, newShdWidths, space, newLocalPlusShadow);

    checkError2(localizationInfo.references.empty(), "Please, unlocalize all the reference arrays before redistribution.");

    if (newValueFlag) {
        for (int i = 0; i < devicesCount; i++)
            deleteRepr(i);
    } else if (!doComm) {
        // LibDVM does all the communications
        if (hasLocalFlag) {
            checkInternal(representatives[0]);
            if (currentMPS->getCommSize() > 1) {
                PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRedistribute);
                getActualBaseOne(0, localPart, localPart, true);
                representatives[0]->doTransform(0, 0);
            }
            checkInternal(!representatives[0]->ownsMemory()); // It means we can delete the representative without doubt
        }
        if (hasLocalFlag && (!newHasLocal || !localPlusShadow->blockEquals(rank, newLocalPlusShadow) || !localPart->blockEquals(rank, newLocalPart))) {
            for (int i = 0; i < devicesCount; i++)
                deleteRepr(i);
        }
    } else {
        assert(doComm);
        dvmh_log(TRACE, "Transiting from old local part (hasLocal=%d):", hasLocalFlag ? 1 : 0);
        custom_log(TRACE, blockOut, rank, localPart);
        dvmh_log(TRACE, "Transiting to new local part (newHasLocal=%d):", newHasLocal ? 1 : 0);
        custom_log(TRACE, blockOut, rank, newLocalPart);
        checkInternal2(!alignRule->hasIndirect() && !newAlignRule->hasIndirect(), "Realign for indirectly-distributed arrays is not implemented yet");
        const MultiprocessorSystem *oldMPS, *newMPS;
        oldMPS = alignRule->getDistribRule()->getMPS();
        newMPS = newAlignRule->getDistribRule()->getMPS();
        checkError2(oldMPS->isSubsystemOf(currentMPS) && newMPS->isSubsystemOf(currentMPS),
                "The former multiprocessor system or the new one is not a subsystem of the current multiprocessor system");
        MultiprocessorSystem *commMPS = currentMPS; // XXX: Could be chosen shorter communicator
        if (commMPS->getCommRank() >= 0) {
#ifdef NON_CONST_AUTOS
            Interval oldDspacePart[oldDspaceRank], newDspacePart[newDspaceRank], oldProcBlock[rank], newProcBlock[rank], block[rank];
#else
            Interval oldDspacePart[MAX_DISTRIB_SPACE_RANK], newDspacePart[MAX_DISTRIB_SPACE_RANK], oldProcBlock[MAX_ARRAY_RANK], newProcBlock[MAX_ARRAY_RANK],
                    block[MAX_ARRAY_RANK];
#endif
            char *sends = new char[oldMPS->getCommSize()];
            DvmhPieces *accumulator = new DvmhPieces(rank);
            for (int p = 0; p < oldMPS->getCommSize(); p++) {
                sends[p] = 0;
                if (alignRule->getDistribRule()->fillLocalPart(p, oldDspacePart)) {
                    oldProcBlock->blockAssign(rank, space);
                    if (alignRule->mapOnPart(oldDspacePart, oldProcBlock, false)) {
                        bool haveSmth = false;
                        for (int j = 0; !haveSmth && j < accumulator->getCount(); j++)
                            haveSmth = haveSmth || oldProcBlock->blockIntersect(rank, accumulator->getPiece(j), block);
                        // block must be either empty or equal to oldProcBlock
                        if (haveSmth) {
                            checkInternal(block->blockEquals(rank, oldProcBlock));
                        } else {
                            accumulator->appendOne(oldProcBlock); // Since they do not intersect
                            sends[p] = 1;
                        }
                    }
                }
                dvmh_log(TRACE, "sends[%d]=%d", p, sends[p]);
            }
            delete accumulator;
            bool iSend = oldMPS->getCommRank() >= 0 ? sends[oldMPS->getCommRank()] : false;
            int procCount = commMPS->getCommSize();
            int ourProc = commMPS->getCommRank();
            UDvmType *sendSizes = new UDvmType[procCount];
            char **sendBuffers = new char *[procCount];
            UDvmType *recvSizes = new UDvmType[procCount];
            char **recvBuffers = new char *[procCount];
            for (int i = 0; i < procCount; i++) {
                sendSizes[i] = 0;
                sendBuffers[i] = 0;
                recvSizes[i] = 0;
                recvBuffers[i] = 0;
            }
            std::vector<DvmhBuffer *> deferredCopyingTo;
            std::vector<DvmhBuffer *> deferredCopyingFrom;
            DvmhPieces *toGetActual = new DvmhPieces(rank);
            DvmhPieces *toSetActual = new DvmhPieces(rank);
            for (int p = 0; p < procCount; p++) {
                int oldProc = commMPS->getChildCommRank(oldMPS, p);
                int newProc = commMPS->getChildCommRank(newMPS, p);
                bool oldProcHasLocal = false;
                bool newProcHasLocal = false;
                if (oldProc >= 0 && alignRule->getDistribRule()->fillLocalPart(oldProc, oldDspacePart)) {
                    oldProcBlock->blockAssign(rank, space);
                    oldProcHasLocal = alignRule->mapOnPart(oldDspacePart, oldProcBlock, false);
                }
                if (newProc >= 0 && newAlignRule->getDistribRule()->fillLocalPart(newProc, newDspacePart)) {
                    newProcBlock->blockAssign(rank, space);
                    newProcHasLocal = newAlignRule->mapOnPart(newDspacePart, newProcBlock, false);
                }
                if (p != ourProc && newProcHasLocal && iSend && newProcBlock->blockIntersect(rank, localPart, block)) {
                    DvmhPieces *reduced = new DvmhPieces(rank);
                    reduced->appendOne(block);
                    if (oldProcHasLocal)
                        reduced->subtractOne(oldProcBlock);
                    toGetActual->unite(reduced);
                    reduced->compactify();
                    for (int j = 0; j < reduced->getCount(); j++)
                        sendSizes[p] += reduced->getPiece(j)->blockSize(rank) * typeSize;
                    sendBuffers[p] = new char[sendSizes[p]];
                    char *ptr = sendBuffers[p];
                    for (int j = 0; j < reduced->getCount(); j++) {
                        const Interval *curBlock = reduced->getPiece(j);
                        deferredCopyingTo.push_back(new DvmhBuffer(rank, typeSize, 0, curBlock, ptr));
                        ptr += curBlock->blockSize(rank) * typeSize;
                    }
                    delete reduced;
                }
                if (p != ourProc && oldProcHasLocal && sends[oldProc] && oldProcBlock->blockIntersect(rank, newLocalPart, block)) {
                    DvmhPieces *reduced = new DvmhPieces(rank);
                    reduced->appendOne(block);
                    if (hasLocalFlag)
                        reduced->subtractOne(localPart);
                    toSetActual->unite(reduced);
                    reduced->compactify();
                    for (int j = 0; j < reduced->getCount(); j++)
                        recvSizes[p] += reduced->getPiece(j)->blockSize(rank) * typeSize;
                    recvBuffers[p] = new char[recvSizes[p]];
                    char *ptr = recvBuffers[p];
                    for (int j = 0; j < reduced->getCount(); j++) {
                        const Interval *curBlock = reduced->getPiece(j);
                        deferredCopyingFrom.push_back(new DvmhBuffer(rank, typeSize, 0, curBlock, ptr));
                        ptr += curBlock->blockSize(rank) * typeSize;
                    }
                    delete reduced;
                }
            }
            delete[] sends;
            if (hasLocalFlag) {
                if (!representatives[0])
                    createNewRepr(0, localPlusShadow);
                assert(representatives[0]);
                {
                    PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRedistribute);
                    getActualBase(0, toGetActual, localPart, true);
                }
                toGetActual->clear();
                for (int i = 0; i < (int)deferredCopyingTo.size(); i++) {
                    representatives[0]->getBuffer()->copyTo(deferredCopyingTo[i]);
                    delete deferredCopyingTo[i];
                }
                deferredCopyingTo.clear();
            }
            checkInternal(toGetActual->isEmpty() && deferredCopyingTo.empty());
            delete toGetActual;
            commMPS->alltoallv2(sendSizes, sendBuffers, recvSizes, recvBuffers);
            for (int p = 0; p < procCount; p++)
                delete[] sendBuffers[p];
            delete[] sendBuffers;
            delete[] sendSizes;
            if (newHasLocal) {
                if (hasLocalFlag) {
                    if (!localPlusShadow->blockEquals(rank, newLocalPlusShadow) || !localPart->blockEquals(rank, newLocalPart)) {
                        if (localPart->blockIntersect(rank, newLocalPart, block)) {
                            if (!representatives[0])
                                createNewRepr(0, localPlusShadow);
                            {
                                PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRedistribute);
                                getActualBaseOne(0, block, localPart, true);
                            }
                            deferredCopyingFrom.push_back(representatives[0]->getBuffer()->dumpPiece(block));
                            toSetActual->uniteOne(block);
                        }
                        for (int i = 0; i < devicesCount; i++)
                            deleteRepr(i);
                    }
                }
                if (!representatives[0]) {
                    DvmhPieces *p1 = new DvmhPieces(rank);
                    p1->appendOne(newLocalPart);
                    DvmhPieces *p2 = toSetActual->subtract(p1);
                    checkInternal2(p2->isEmpty(), "Too much to set actual");
                    delete p2;
                    p2 = p1->subtract(toSetActual);
                    checkInternal2(p2->isEmpty(), "Not enough to set actual");
                    delete p2;
                    delete p1;
                }
                if (!representatives[0])
                    createNewRepr(0, newLocalPlusShadow);
                for (int i = 0; i < (int)deferredCopyingFrom.size(); i++) {
                    deferredCopyingFrom[i]->copyTo(representatives[0]->getBuffer());
                    delete deferredCopyingFrom[i];
                }
                deferredCopyingFrom.clear();
                clearActual(toSetActual, 0);
                representatives[0]->getActualState()->unite(toSetActual);
                toSetActual->clear();
            } else {
                for (int i = 0; i < devicesCount; i++)
                    deleteRepr(i);
            }
            checkInternal(toSetActual->isEmpty() && deferredCopyingFrom.empty());
            delete toSetActual;
            for (int p = 0; p < procCount; p++)
                delete[] recvBuffers[p];
            delete[] recvBuffers;
            delete[] recvSizes;
        } else {
            checkInternal(!hasLocalFlag && !newHasLocal);
        }
    }
    DvmhDistribSpace *dspace = alignRule->getDspace();
    delete alignRule;
    if (dspace != newAlignRule->getDspace()) {
        dspace->removeAlignedData(this);
        if (ownDspace) {
            checkError2(dspace->getRefCount() == 0, "Realigning distributed array with existing descendants is prohibited");
            delete dspace;
        }
    }
    alignRule = newAlignRule;
    hasLocalFlag = newHasLocal;
    localPart->blockAssign(rank, newLocalPart);
    localPlusShadow->blockAssign(rank, newLocalPlusShadow);
    typedMemcpy(shdWidths, newShdWidths, rank);
    initActualShadow();
}

void DvmhData::changeShadowWidth(int axis, ShdWidth newWidth) {
    assert(axis >= 1 && axis <= rank);
    ShdWidth oldWidth = shdWidths[axis - 1];
    DvmType newShadowOrder = 1;
#ifdef NON_CONST_AUTOS
    Interval shdRect[rank];
#else
    Interval shdRect[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        shdRect[i][0] = -shdWidths[i][0];
        shdRect[i][1] = shdWidths[i][1];
    }
    if (newWidth[0] > oldWidth[0]) {
        shdRect[axis - 1][0] = -newWidth[0];
        shdRect[axis - 1][1] = -oldWidth[0] - 1;
        shadowProfile->uniteOne(shdRect, newShadowOrder);
    } else if (newWidth[0] < oldWidth[0]) {
        shdRect[axis - 1][0] = -oldWidth[0];
        shdRect[axis - 1][1] = -newWidth[0] - 1;
        shadowProfile->subtractOne(shdRect);
    }
    if (newWidth[1] > oldWidth[1]) {
        shdRect[axis - 1][0] = oldWidth[1] + 1;
        shdRect[axis - 1][1] = newWidth[1];
        shadowProfile->uniteOne(shdRect, newShadowOrder);
    } else if (newWidth[1] < oldWidth[1]) {
        shdRect[axis - 1][0] = newWidth[1] + 1;
        shdRect[axis - 1][1] = oldWidth[1];
        shadowProfile->subtractOne(shdRect);
    }
    shdWidths[axis - 1] = newWidth;
    recalcLocalPlusShadow();
    changeReprSize(0, localPlusShadow);
}

void DvmhData::changeReprSize(int dev, const Interval havePortion[]) {
    DvmhRepresentative *oldRepr = representatives[dev];
    if (oldRepr) {
        DvmhRepresentative *newRepr = new DvmhRepresentative(new DvmhBuffer(rank, typeSize, dev, havePortion));
        oldRepr->getBuffer()->copyTo(newRepr->getBuffer());
        newRepr->getActualState()->clear();
        newRepr->getActualState()->appendOne(havePortion);
        newRepr->getActualState()->intersectInplace(oldRepr->getActualState());
        deleteRepr(dev);
        representatives[dev] = newRepr;
        setBufferAdjustments(newRepr->getBuffer(), alignRule);
        if (dev == 0)
            updateHeaders();
    }
}

void DvmhData::updateHeaders() {
    for (int i = 0; i < (int)headers.size(); i++) {
        if (representatives[0]) {
            void *base = headers[i].freeBase ? representatives[0]->getBuffer()->getDeviceAddr() : (void *)headers[i].ptr[rank + 2];
            representatives[0]->getBuffer()->fillHeader(base, headers[i].ptr, true);
        } else {
            void *base = headers[i].freeBase ? 0 : (void *)headers[i].ptr[rank + 2];
            fillHeader(rank, typeSize, base, base, 0, 0, headers[i].ptr);
        }
        headers[i].ptr[0] = (DvmType)this;
    }
}

template <class T>
static void traversePiece(int rank, void *addr, UDvmType typeSize, const Interval havePortion[], const Interval piece[], T &f) {
#ifdef NON_CONST_AUTOS
    DvmType currentIndex[rank + 1];
    UDvmType partialSize[rank + 1];
    Interval currentPiece[rank];
#else
    DvmType currentIndex[MAX_ARRAY_RANK + 1];
    UDvmType partialSize[MAX_ARRAY_RANK + 1];
    Interval currentPiece[MAX_ARRAY_RANK];
#endif
    int stepRank = rank > 0 ? 1 : 0;
    UDvmType stepSize = 1;
    for (int i = rank - 2; i >= 0; i--) {
        if (piece[i + 1] != havePortion[i + 1])
            break;
        stepRank++;
        stepSize *= piece[i + 1].size();
    }
    if (rank > 0)
        stepSize *= piece[rank - stepRank].size();
    currentIndex[0] = 0;
    partialSize[0] = typeSize;
    for (int i = 0; i < rank; i++) {
        currentIndex[1 + i] = piece[i][0];
        partialSize[1 + i] = partialSize[1 + i - 1] * havePortion[rank - 1 - i].size();
    }
    char *ptr = (char *)addr;
    for (int i = 0; i < rank; i++)
        ptr += partialSize[rank - 1 - i] * (piece[i][0] - havePortion[i][0]);
    for (int i = 0; i < rank; i++) {
        currentPiece[i][0] = piece[i][0];
        if (i < rank - stepRank)
            currentPiece[i][1] = piece[i][0];
        else
            currentPiece[i][1] = piece[i][1];
    }
    while (currentIndex[0] == 0) {
        f(ptr, currentPiece, stepSize);
        if (stepRank > 0) {
            int i = rank - stepRank;
            ptr -= partialSize[rank - 1 - i] * (currentIndex[1 + i] - piece[i][0]);
            currentIndex[1 + i] = piece[i][0];
        }
        int i = rank - stepRank;
        do {
            i--;
            currentIndex[1 + i]++;
            if (i >= 0) {
                ptr += partialSize[rank - 1 - i];
                if (currentIndex[1 + i] > piece[i][1]) {
                    ptr -= partialSize[rank - 1 - i] * (currentIndex[1 + i] - piece[i][0]);
                    currentIndex[1 + i] = piece[i][0];
                }
                currentPiece[i][0] = currentIndex[1 + i];
                currentPiece[i][1] = currentIndex[1 + i];
            }
        } while (i >= 0 && currentIndex[1 + i] == piece[i][0]);
    }
}

template <typename T>
class Unlocalizer {
public:
    explicit Unlocalizer(const IndirectAxisDistribRule *axisRule, const Interval &spaceDim, DvmType summandLocal): spaceDim(spaceDim) {
        local2ToGlobal = axisRule->getLocal2ToGlobal(&totalOffset);
        totalOffset -= summandLocal;
    }
    void operator() (void *ptr, const Interval piece[], UDvmType pieceSize) {
        T *myPtr = (T *)ptr;
        for (UDvmType eli = 0; eli < pieceSize; eli++) {
            myPtr[eli] = convertElement(myPtr[eli]);
        }
    }
protected:
    T convertElement(T oldVal) {
        if (spaceDim.contains(oldVal))
            return local2ToGlobal[(DvmType)oldVal - totalOffset];
        else
            return oldVal;
    }
protected:
    Interval spaceDim;
    DvmType *local2ToGlobal;
    DvmType totalOffset;
};

void DvmhData::unlocalizeValues() {
    if (!localizationInfo.target.isValid())
        return;
    assert(localizationInfo.target.data->isDistributed());
    int dspaceAxis = localizationInfo.target.data->getAlignRule()->getDspaceAxis(localizationInfo.target.axis);
    assert(dspaceAxis > 0);
    const IndirectAxisDistribRule *axisRule = localizationInfo.target.data->getAlignRule()->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect();
    assert(axisRule);
    DvmhPieces *totalActualState = new DvmhPieces(rank);
    for (int i = 0; i < devicesCount; i++) {
        if (representatives[i])
            totalActualState->unite(representatives[i]->getActualState());
    }
    if (representatives[0]) {
        DvmhRepresentative *repr = representatives[0];
        DvmhBuffer *buf = repr->getBuffer();
        getActualBase(0, totalActualState, localPart, true);
        for (int i = 1; i < devicesCount; i++) {
            if (representatives[i]) {
                representatives[i]->getActualState()->clear();
                representatives[i]->setCleanTransformState();
            }
        }
        repr->doTransform(0, 0);
        repr->getActualState()->compactify();
        Interval spaceDim = localizationInfo.target.data->getAxisSpace(localizationInfo.target.axis);
        DvmType summandLocal = localizationInfo.target.data->getAlignRule()->getAxisRule(dspaceAxis)->summandLocal;
        for (int i = 0; i < repr->getActualState()->getCount(); i++) {
            const Interval *piece = repr->getActualState()->getPiece(i);
            if (dataType == dtInt) {
                Unlocalizer<int> unlocalizer(axisRule, spaceDim, summandLocal);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, unlocalizer);
            } else if (dataType == dtLong) {
                Unlocalizer<long> unlocalizer(axisRule, spaceDim, summandLocal);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, unlocalizer);
            } else if (dataType == dtLongLong) {
                Unlocalizer<long long> unlocalizer(axisRule, spaceDim, summandLocal);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, unlocalizer);
            } else {
                assert(false);
            }
        }
    } else {
        checkInternal2(totalActualState->isEmpty(), "Host representative needed to hold actual values");
    }
    localizationInfo.target.data->localizationInfo.removeFromReferences(ReferenceDesc(this, localizationInfo.target.axis));
    localizationInfo.target.clear();
}

template <typename T>
class Localizer {
public:
    explicit Localizer(const IndirectAxisDistribRule *axisRule, const Interval &spaceDim, const Interval &localPlusShadow, const DvmhAxisAlignRule *alRule):
            axisRule(axisRule), spaceDim(spaceDim), localPlusShadow(localPlusShadow), alRule(alRule) {}
    void operator() (void *ptr, const Interval piece[], UDvmType pieceSize) {
        T *myPtr = (T *)ptr;
        for (UDvmType eli = 0; eli < pieceSize; eli++) {
            myPtr[eli] = convertElement(myPtr[eli]);
        }
    }
protected:
    T convertElement(T oldVal) {
        if (spaceDim.contains(oldVal)) {
            bool isLocal = false;
            bool isShadow = false;
            DvmType localIdx = axisRule->globalToLocal2((DvmType)oldVal * alRule->multiplier + alRule->summand, &isLocal, &isShadow);
            checkError3(isLocal || isShadow, "Can not localize element with value " DTFMT, (DvmType)oldVal);
            localIdx -= alRule->summandLocal;
            checkError3(localPlusShadow.contains(localIdx), "Can not localize element with value " DTFMT, (DvmType)oldVal);
            return localIdx;
        } else {
            return oldVal;
        }
    }
protected:
    const IndirectAxisDistribRule *axisRule;
    Interval spaceDim;
    Interval localPlusShadow;
    const DvmhAxisAlignRule *alRule;
};

void DvmhData::localizeValues(ReferenceDesc target) {
    assert(!localizationInfo.target.isValid());
    DvmhData *targetData = target.data;
    int targetAxis = target.axis;
    checkInternal(targetAxis >= 1 && targetAxis <= targetData->getRank());
    if (!targetData->isDistributed())
        return;
    int dspaceAxis = targetData->getAlignRule()->getDspaceAxis(targetAxis);
    if (dspaceAxis <= 0)
        return;
    IndirectAxisDistribRule *axisRule = targetData->getAlignRule()->getDspace()->getAxisDistribRule(dspaceAxis)->asIndirect();
    if (!axisRule)
        return;
    DvmhPieces *totalActualState = new DvmhPieces(rank);
    for (int i = 0; i < devicesCount; i++) {
        if (representatives[i])
            totalActualState->unite(representatives[i]->getActualState());
    }
    if (representatives[0]) {
        DvmhRepresentative *repr = representatives[0];
        DvmhBuffer *buf = repr->getBuffer();
        getActualBase(0, totalActualState, localPart, true);
        for (int i = 1; i < devicesCount; i++) {
            if (representatives[i]) {
                representatives[i]->getActualState()->clear();
                representatives[i]->setCleanTransformState();
            }
        }
        repr->doTransform(0, 0);
        repr->getActualState()->compactify();
        Interval spaceDim = targetData->getAxisSpace(targetAxis);
        Interval lpsDim = targetData->getLocalPlusShadow()[targetAxis - 1];
        const DvmhAxisAlignRule *alRule = targetData->getAlignRule()->getAxisRule(dspaceAxis);
        for (int i = 0; i < repr->getActualState()->getCount(); i++) {
            const Interval *piece = repr->getActualState()->getPiece(i);
            if (dataType == dtInt) {
                Localizer<int> localizer(axisRule, spaceDim, lpsDim, alRule);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, localizer);
            } else if (dataType == dtLong) {
                Localizer<long> localizer(axisRule, spaceDim, lpsDim, alRule);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, localizer);
            } else if (dataType == dtLongLong) {
                Localizer<long long> localizer(axisRule, spaceDim, lpsDim, alRule);
                traversePiece(rank, buf->getDeviceAddr(), buf->getTypeSize(), buf->getHavePortion(), piece, localizer);
            } else {
                assert(false);
            }
        }
    } else {
        checkInternal2(totalActualState->isEmpty(), "Host representative needed to hold actual values");
    }
    localizationInfo.target = target;
    targetData->localizationInfo.references.push_back(ReferenceDesc(this, targetAxis));
}

// DvmhShadowData

DvmhShadowData::DvmhShadowData(const DvmhShadowData &sdata): data(sdata.data), isIndirect(sdata.isIndirect),
        shdWidths(0), cornerFlag(sdata.cornerFlag), indirectAxis(sdata.indirectAxis), indirectName(sdata.indirectName) {
    if (sdata.shdWidths) {
        shdWidths = new ShdWidth[data->getRank()];
        typedMemcpy(shdWidths, sdata.shdWidths, data->getRank());
    }
}

DvmhShadowData &DvmhShadowData::operator=(const DvmhShadowData &sdata) {
    data = sdata.data;
    isIndirect = sdata.isIndirect;
    delete[] shdWidths;
    shdWidths = 0;
    if (sdata.shdWidths) {
        shdWidths = new ShdWidth[data->getRank()];
        typedMemcpy(shdWidths, sdata.shdWidths, data->getRank());
    }
    cornerFlag = sdata.cornerFlag;
    indirectAxis = sdata.indirectAxis;
    indirectName = sdata.indirectName;
    return *this;
}

bool DvmhShadowData::empty() const {
    assert(data);
    if (!isIndirect) {
        assert(shdWidths);
        for (int i = 0; i <  data->getRank(); i++) {
            if (shdWidths[i][0] > 0 || shdWidths[i][1] > 0)
                return false;
        }
        return true;
    } else {
        assert(indirectAxis >= 1 && indirectAxis <= data->getRank());
        return false;
    }
}

// DvmhShadow

static int calcOneOffset(int i, const int stepCount[], bool noZero) {
    if (i < std::abs(stepCount[0])) {
        if (stepCount[0] >= 0)
            i -= stepCount[0];
        else
            i = -stepCount[0] - i;
    } else {
        i -= std::abs(stepCount[0]);
        if (noZero)
            i++;
        if (stepCount[1] < 0)
            i = -i;
    }
    return i;
}

static void doCoordShift(int i, int rank, bool cornerFlag, const int stepCount[], const int beginCoords[], int resCoords[]) {
    if (cornerFlag) {
        int restProd = 1;
        for (int j = rank - 1; j >= 0; j--){
            int offs = calcOneOffset((i / restProd) % (std::abs(stepCount[2 * j + 0]) + std::abs(stepCount[2 * j + 1]) + 1), stepCount + 2 * j, false);
            resCoords[j] = beginCoords[j] + offs;
            restProd *= std::abs(stepCount[2 * j + 0]) + std::abs(stepCount[2 * j + 1]) + 1;
        }
    } else {
        int k;
        for (k = 0; k < rank; k++) {
            if (i < std::abs(stepCount[2 * k + 0]) + std::abs(stepCount[2 * k + 1]))
                break;
            else
                i -= std::abs(stepCount[2 * k + 0]) + std::abs(stepCount[2 * k + 1]);
        }

        for (int j = 0; j < rank; j++) {
            int offs = 0;
            if (j == k)
                offs = calcOneOffset(i, stepCount + 2 * k, true);
            resCoords[j] = beginCoords[j] + offs;
        }
    }
}

static void doInterprocessShadowRenew(DvmhData *data, bool cornerFlag, const ShdWidth shdWidths[]) {
    checkInternal(data->isDistributed());
    const DvmhAlignRule *alRule = data->getAlignRule();
    const DvmhDistribRule *disRule = alRule->getDistribRule();
    const MultiprocessorSystem *dataMPS = disRule->getMPS();
    if (dataMPS->getCommRank() >= 0 && data->hasLocal()) {
        assert(data->getRepr(0));
        int dataRank = data->getRank();
        int dspaceRank = alRule->getDspace()->getRank();
        int mpsRank = std::max(dataMPS->getRank(), disRule->getMpsAxesUsed());
        int commRank = dataMPS->getCommRank();
        for (int i = 0; i < dataRank; i++) {
            if (alRule->isIndirect(i + 1))
                checkInternal(shdWidths[i].empty() || data->getLocalPart()[i] == data->getSpace()[i]);
        }
#ifdef NON_CONST_AUTOS
        int recvStepCount[2 * mpsRank], sendStepCount[2 * mpsRank];
#else
        int recvStepCount[2 * MAX_MPS_RANK], sendStepCount[2 * MAX_MPS_RANK];
#endif
        for (int i = 0; i < 2 * mpsRank; i++)
            recvStepCount[i] = sendStepCount[i] = 0;
        int sendNeighbNum = cornerFlag ? 1 : 0;
        int recvNeighbNum = cornerFlag ? 1 : 0;
        for (int i = 0; i < dataRank; i++) {
            int dspaceAxis = alRule->getDspaceAxis(i + 1);
            if (dspaceAxis > 0 && disRule->getAxisRule(dspaceAxis)->isDistributed()) {
                const DvmhAxisDistribRule *axDisRule = disRule->getAxisRule(dspaceAxis);
                const DvmhAxisAlignRule *axAlRule = alRule->getAxisRule(dspaceAxis);
                int mpsAxis = axDisRule->getMPSAxis();
                int myProc = dataMPS->getAxis(mpsAxis).ourProc;
                ShdWidth shd = shdWidths[i];
                Interval lp = data->getLocalPart()[i];
                Interval lps = data->getLocalPlusShadow()[i];
                Interval sp = data->getSpace()[i];
                if (shd[0] > 0 && lp[0] > sp[0]) {
                    // Need to receive something to the left shadow edge
                    DvmType dataIndex = std::max(lp[0] - shd[0], lps[0]);
                    DvmType dspaceIndex = axAlRule->multiplier * dataIndex + axAlRule->summand;
                    int proc = axDisRule->getProcIndex(dspaceIndex);
                    recvStepCount[2 * (mpsAxis - 1) + 0] = myProc - proc;
                } else {
                    recvStepCount[2 * (mpsAxis - 1) + 0] = 0;
                }
                if (shd[1] > 0 && lp[1] < sp[1]) {
                    // Need to receive something to the right shadow edge
                    DvmType dataIndex = std::min(lp[1] + shd[1], lps[1]);
                    DvmType dspaceIndex = axAlRule->multiplier * dataIndex + axAlRule->summand;
                    int proc = axDisRule->getProcIndex(dspaceIndex);
                    recvStepCount[2 * (mpsAxis - 1) + 1] = proc - myProc;
                } else {
                    recvStepCount[2 * (mpsAxis - 1) + 1] = 0;
                }
                if (shd[1] > 0 && lp[0] > sp[0]) {
                    // Need to send something to the left
                    DvmType dataIndex = std::max(lp[0] - shd[1], sp[0]);
                    DvmType dspaceIndex = axAlRule->multiplier * dataIndex + axAlRule->summand;
                    int proc = axDisRule->getProcIndex(dspaceIndex);
                    sendStepCount[2 * (mpsAxis - 1) + 0] = myProc - proc;
                } else {
                    sendStepCount[2 * (mpsAxis - 1) + 0] = 0;
                }
                if (shd[0] > 0 && lp[1] < sp[1]) {
                    // Need to send something to the right
                    DvmType dataIndex = std::min(lp[1] + shd[0], sp[1]);
                    DvmType dspaceIndex = axAlRule->multiplier * dataIndex + axAlRule->summand;
                    int proc = axDisRule->getProcIndex(dspaceIndex);
                    sendStepCount[2 * (mpsAxis - 1) + 1] = proc - myProc;
                } else {
                    sendStepCount[2 * (mpsAxis - 1) + 1] = 0;
                }
                if (cornerFlag) {
                    recvNeighbNum *= std::abs(recvStepCount[2 * (mpsAxis - 1) + 0]) + std::abs(recvStepCount[2 * (mpsAxis - 1) + 1]) + 1;
                    sendNeighbNum *= std::abs(sendStepCount[2 * (mpsAxis - 1) + 0]) + std::abs(sendStepCount[2 * (mpsAxis - 1) + 1]) + 1;
                } else {
                    recvNeighbNum += std::abs(recvStepCount[2 * (mpsAxis - 1) + 0]) + std::abs(recvStepCount[2 * (mpsAxis - 1) + 1]);
                    sendNeighbNum += std::abs(sendStepCount[2 * (mpsAxis - 1) + 0]) + std::abs(sendStepCount[2 * (mpsAxis - 1) + 1]);
                }
            }
        }
        // Actually, in case of cornerFlag recvNeighbNum and sendNeighbNum are +1 of the real number
        int *sendProcs = new int[sendNeighbNum];
        UDvmType *sendSizes = new UDvmType[sendNeighbNum];
        char **sendBuffers = new char *[sendNeighbNum];
        int *recvProcs = new int[recvNeighbNum];
        UDvmType *recvSizes = new UDvmType[recvNeighbNum];
        char **recvBuffers = new char *[recvNeighbNum];
        for (int i = 0; i < sendNeighbNum; i++) {
            sendProcs[i] = -1;
            sendSizes[i] = 0;
            sendBuffers[i] = 0;
        }
        for (int i = 0; i < recvNeighbNum; i++) {
            recvProcs[i] = -1;
            recvSizes[i] = 0;
            recvBuffers[i] = 0;
        }
        std::vector<DvmhBuffer *> deferredCopyFrom;
        deferredCopyFrom.reserve(recvNeighbNum);
#ifdef NON_CONST_AUTOS
        int myCoords[mpsRank], coords[mpsRank];
        Interval procDspacePart[dspaceRank], procBlock[dataRank], procLocalPlusShadow[dataRank], commonBlock[dataRank];
#else
        int myCoords[MAX_MPS_RANK], coords[MAX_MPS_RANK];
        Interval procDspacePart[MAX_DISTRIB_SPACE_RANK], procBlock[MAX_ARRAY_RANK], procLocalPlusShadow[MAX_ARRAY_RANK], commonBlock[MAX_ARRAY_RANK];
#endif
        dataMPS->fillAxisIndexes(commRank, mpsRank, myCoords);
        for (int i = 0; i < sendNeighbNum; i++) {
            doCoordShift(i, mpsRank, cornerFlag, sendStepCount, myCoords, coords);
            int otherCommRank = dataMPS->getCommRank(mpsRank, coords);
            sendProcs[i] = otherCommRank;
            if (otherCommRank == commRank)
                continue;
            if (disRule->fillLocalPart(otherCommRank, procDspacePart)) {
                procBlock->blockAssign(dataRank, data->getSpace());
                if (alRule->mapOnPart(procDspacePart, procBlock, false)) {
                    data->extendBlock(procBlock, procLocalPlusShadow);
                    if (data->getLocalPart()->blockIntersect(dataRank, procLocalPlusShadow, commonBlock)) {
                        UDvmType totalSize = data->getTypeSize() * commonBlock->blockSize(dataRank);
                        sendSizes[i] = totalSize;
                        sendBuffers[i] = new char[totalSize];
                        DvmhBuffer buf(dataRank, data->getTypeSize(), 0, commonBlock, sendBuffers[i]);
                        data->getBuffer(0)->copyTo(&buf);
                    }
                }
            }
        }
        for (int i = 0; i < recvNeighbNum; i++) {
            doCoordShift(i, mpsRank, cornerFlag, recvStepCount, myCoords, coords);
            int otherCommRank = dataMPS->getCommRank(mpsRank, coords);
            recvProcs[i] = otherCommRank;
            if (otherCommRank == commRank)
                continue;
            if (disRule->fillLocalPart(otherCommRank, procDspacePart)) {
                procBlock->blockAssign(dataRank, data->getSpace());
                if (alRule->mapOnPart(procDspacePart, procBlock, false)) {
                    if (procBlock->blockIntersect(dataRank, data->getLocalPlusShadow(), commonBlock)) {
                        UDvmType totalSize = data->getTypeSize() * commonBlock->blockSize(dataRank);
                        recvSizes[i] = totalSize;
                        recvBuffers[i] = new char[totalSize];
                        deferredCopyFrom.push_back(new DvmhBuffer(dataRank, data->getTypeSize(), 0, commonBlock, recvBuffers[i]));
                    }
                }
            }
        }

        dataMPS->alltoallv3(sendNeighbNum, sendProcs, sendSizes, sendBuffers, recvNeighbNum, recvProcs, recvSizes, recvBuffers);

        delete[] sendProcs;
        delete[] sendSizes;
        for (int i = 0; i < sendNeighbNum; i++)
            delete[] sendBuffers[i];
        delete[] sendBuffers;

        for (int i = 0; i < (int)deferredCopyFrom.size(); i++) {
            deferredCopyFrom[i]->copyTo(data->getBuffer(0));
            delete deferredCopyFrom[i];
        }
        deferredCopyFrom.clear();

        delete[] recvProcs;
        delete[] recvSizes;
        for (int i = 0; i < recvNeighbNum; i++)
            delete[] recvBuffers[i];
        delete[] recvBuffers;
    }
}

void doIndirectShadowRenew(DvmhData *data, int dataAxis, const std::string &shadowName) {
    checkError2(data->hasIndirectShadow(dataAxis, shadowName), "Shadow edge must be included before renewal");
    int dspaceAxis = data->getAlignRule()->getDspaceAxis(dataAxis);
    DvmType axisDataOffset = -data->getAlignRule()->getAxisRule(dspaceAxis)->summandLocal;
    const IndirectAxisDistribRule *disRule = data->getAlignRule()->getDistribRule()->getAxisRule(dspaceAxis)->asIndirect();
    assert(disRule);
    const ExchangeMap &xchgMap = disRule->getShadowExchangeMap(shadowName);
    DvmhBuffer *dataBuffer = data->getBuffer(0);
    if (!dataBuffer) {
        xchgMap.performExchange(0, 0, 0);
    } else {
        assert(dataBuffer);
        int rank = data->getRank();
        const Interval *localPart = data->getLocalPart();
        const Interval *havePortion = dataBuffer->getHavePortion();
        int oneSizeFromLeft = 0;
        int fullSizeFromRight = 0;
        for (int i = 0; i < rank; i++) {
            if (localPart[i].size() > 1)
                break;
            oneSizeFromLeft++;
        }
        for (int i = rank - 1; i >= 0; i--) {
            if (havePortion[i] != localPart[i])
                break;
            fullSizeFromRight++;
        }
        if (dataAxis <= 1 + oneSizeFromLeft && fullSizeFromRight >= (rank - dataAxis)) {
            // Can treat it as 1-dimensional array
#ifdef NON_CONST_AUTOS
            DvmType startIndexes[rank];
#else
            DvmType startIndexes[MAX_ARRAY_RANK];
#endif
            for (int i = 0; i < rank; i++)
                startIndexes[i] = localPart[i][0];
            startIndexes[dataAxis - 1] = havePortion[dataAxis - 1][0];
            UDvmType elemSize = dataBuffer->getTypeSize() * (havePortion + dataAxis)->blockSize(rank - dataAxis);
            xchgMap.performExchange(dataBuffer->getElemAddr(startIndexes), -havePortion[dataAxis - 1][0] + axisDataOffset, elemSize);
        } else {
            xchgMap.performExchange(dataBuffer, dataAxis, axisDataOffset, localPart);
        }
    }
}

void DvmhShadow::renew(DvmhRegion *currentRegion, bool doComm) const {
    PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpShadow);
    for (int i = 0; i < (int)datas.size(); i++) {
        const DvmhShadowData &sdata = datas[i];
        DvmhData *data = sdata.data;
        if (data->hasLocal()) {
            if (!sdata.isIndirect) {
                data->getActualEdges(data->getLocalPart(), sdata.shdWidths, (currentRegion ? currentRegion->canAddToActual(data, data->getLocalPart()) : true));
                data->setActualShadow(0, data->getLocalPart(), sdata.cornerFlag, sdata.shdWidths);
            } else {
                data->getActualIndirectEdges(sdata.indirectAxis, sdata.indirectName,
                        (currentRegion ? currentRegion->canAddToActual(data, data->getLocalPart()) : true));
                data->setActualIndirectShadow(0, sdata.indirectAxis, sdata.indirectName);
            }
        }
        if (doComm) {
            if (!sdata.isIndirect)
                doInterprocessShadowRenew(sdata.data, sdata.cornerFlag, sdata.shdWidths);
            else
                doIndirectShadowRenew(sdata.data, sdata.indirectAxis, sdata.indirectName);
        }
        if (!sdata.isIndirect)
            data->updateShadowProfile(sdata.cornerFlag, sdata.shdWidths);
        else
            data->updateIndirectShadowProfile(sdata.indirectAxis, sdata.indirectName);
        if (data->hasLocal() && currentRegion)
            currentRegion->markToRenew(data);
    }
}

void dvmhCopyArrayArray(DvmhData *src, const Interval srcBlock[], const DvmType srcSteps[], DvmhData *dst, const Interval dstBlock[], const DvmType dstSteps[],
        const int dstAxisToSrc[]) {
    checkInternal(src->isAligned() && dst->isAligned());
    int srcRank = src->getRank();
    int dstRank = dst->getRank();
#ifdef NON_CONST_AUTOS
    bool seenSrcAxes[srcRank];
    Interval compBlock[dstRank + 1], srcLocalBlock[srcRank], dstLocalBlock[dstRank];
    int compAxisToSrc[dstRank + 1], compAxisToDst[dstRank + 1];
#else
    bool seenSrcAxes[MAX_ARRAY_RANK];
    Interval compBlock[MAX_ARRAY_RANK + 1], srcLocalBlock[MAX_ARRAY_RANK], dstLocalBlock[MAX_ARRAY_RANK];
    int compAxisToSrc[MAX_ARRAY_RANK + 1], compAxisToDst[MAX_ARRAY_RANK + 1];
#endif
    int compRank = 0;
    for (int i = 0; i < srcRank; i++) {
        assert(!srcBlock[i].empty() && srcSteps[i] >= 1);
        assert((srcBlock[i].size() - 1) % srcSteps[i] == 0);
        seenSrcAxes[i] = false;
        if (srcBlock[i].size() > 1) {
            compBlock[compRank][0] = 0;
            compBlock[compRank][1] = (srcBlock[i].size() - 1) / srcSteps[i];
            compAxisToSrc[compRank] = i + 1;
            compRank++;
        }
    }
    compBlock[compRank] = Interval::create(0, 0);
    compAxisToSrc[compRank] = -1;
    compAxisToDst[compRank] = -1;
    compRank++;
    int prevSeenSrcAxis = -1;
    for (int i = 0; i < dstRank; i++) {
        assert(!dstBlock[i].empty() && dstSteps[i] >= 1);
        assert((dstBlock[i].size() - 1) % dstSteps[i] == 0);
        int srcAxis = dstAxisToSrc[i];
        if (srcAxis > 0) {
            assert(srcAxis <= srcRank);
            assert((dstBlock[i].size() - 1) / dstSteps[i] == (srcBlock[srcAxis - 1].size() - 1) / srcSteps[srcAxis - 1]);
            assert(!seenSrcAxes[srcAxis - 1]);
            seenSrcAxes[srcAxis - 1] = true;
            if (prevSeenSrcAxis > 0) {
                // XXX: This requirement could be relaxed if necessary
                assert(srcAxis > prevSeenSrcAxis);
            }
            prevSeenSrcAxis = srcAxis;
            if (dstBlock[i].size() > 1) {
                int compAx = -1;
                for (int j = 0; j < compRank; j++) {
                    if (compAxisToSrc[j] == srcAxis) {
                        compAx = j + 1;
                        break;
                    }
                }
                assert(compAx > 0);
                compAxisToDst[compAx] = i + 1;
            }
        } else {
            // XXX: This requirement could be relaxed if necessary
            assert(dstBlock[i].size() == 1);
        }
    }
    for (int i = 0; i < srcRank; i++) {
        if (!seenSrcAxes[i])
            assert(srcBlock[i].size() == 1);
    }
    bool srcHasLocalBlock = src->hasLocal();
    bool dstHasLocalBlock = dst->hasLocal();
    if (src->hasLocal())
        srcHasLocalBlock = shrinkBlock(srcRank, srcBlock, srcSteps, src->getLocalPart(), srcLocalBlock);
    if (dst->hasLocal())
        dstHasLocalBlock = shrinkBlock(dstRank, dstBlock, dstSteps, dst->getLocalPart(), dstLocalBlock);
    if (!src->isDistributed()) {
        // Have all the data locally
        assert(src->hasLocal());
        if (dst->hasLocal()) {
            assert(src->getRepr(0) && dst->getRepr(0));
        }
        checkInternal2(0, "Not implemented yet");
    } else if (!dst->isDistributed()) {
        // Allgatherv case or even bcast case
        checkInternal2(0, "Not implemented yet");
    } else {
        assert(src->isDistributed() && dst->isDistributed());
        // Common case
        checkInternal2(0, "Not implemented yet");
    }
}

static void copyIntervalsBlockRecursive(DvmhBuffer *sparseBuf, const Intervals sparseBlock[], DvmhBuffer *compactBuf, const Interval compactBlock[],
        bool fromSparse, Interval curSparseBlock[], DvmType curCompactIndexOffset[], int axis) {
    if (axis <= sparseBuf->getRank()) {
        Interval curCompactInterval = Interval::createEmpty();
        for (int i = 0; i < sparseBlock[axis - 1].getIntervalCount(); i++) {
            curSparseBlock[axis - 1] = sparseBlock[axis - 1].getInterval(i);
            if (i == 0) {
                curCompactInterval[0] = compactBlock[axis - 1][0];
            } else {
                curCompactInterval[0] = curCompactInterval[1] + 1;
            }
            curCompactInterval[1] = curCompactInterval[0] + (DvmType)curSparseBlock[axis - 1].size() - 1;
            curCompactIndexOffset[axis - 1] = curSparseBlock[axis - 1][0] - curCompactInterval[0];
            copyIntervalsBlockRecursive(sparseBuf, sparseBlock, compactBuf, compactBlock, fromSparse, curSparseBlock, curCompactIndexOffset, axis + 1);
        }
    } else {
        DvmhBuffer reinterpretedCompact(*compactBuf, curCompactIndexOffset);
        if (fromSparse)
            sparseBuf->copyTo(&reinterpretedCompact, curSparseBlock);
        else
            reinterpretedCompact.copyTo(sparseBuf, curSparseBlock);
    }
}

static void copyIntervalsBlock(DvmhBuffer *sparseBuf, const Intervals sparseBlock[], DvmhBuffer *compactBuf, const Interval compactBlock[], bool fromSparse)
{
    int rank = sparseBuf->getRank();
    assert(rank == compactBuf->getRank());
    assert(sparseBuf->getTypeSize() == compactBuf->getTypeSize());
    for (int i = 0; i < rank; i++) {
        assert(sparseBlock[i].getElementCount() == compactBlock[i].size());
        assert(sparseBuf->getHavePortion()[i].contains(sparseBlock[i].getBoundInterval()));
        assert(compactBuf->getHavePortion()[i].contains(compactBlock[i]));
    }
#ifdef NON_CONST_AUTOS
    Interval curSparseBlock[rank];
    DvmType curCompactIndexOffset[rank];
#else
    Interval curSparseBlock[MAX_ARRAY_RANK];
    DvmType curCompactIndexOffset[MAX_ARRAY_RANK];
#endif
    copyIntervalsBlockRecursive(sparseBuf, sparseBlock, compactBuf, compactBlock, fromSparse, curSparseBlock, curCompactIndexOffset, 1);
}

static void copyIntervalsBlock(DvmhBuffer *sparseBuf, const Intervals sparseBlock[], void *mem, bool fromSparse) {
    int rank = sparseBuf->getRank();
#ifdef NON_CONST_AUTOS
    Interval compactBlock[rank];
#else
    Interval compactBlock[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        compactBlock[i][0] = 0;
        compactBlock[i][1] = sparseBlock[i].getElementCount() - 1;
    }
    DvmhBuffer wrap(rank, sparseBuf->getTypeSize(), 0, compactBlock, mem, false);
    copyIntervalsBlock(sparseBuf, sparseBlock, &wrap, compactBlock, fromSparse);
}

static void copyReplicatedToIndirect(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    checkInternal(!src->isDistributed());
    checkInternal(dst->isDistributed() && dst->getAlignRule()->hasIndirect());
    checkInternal(src->getRank() == dst->getRank());
    int rank = src->getRank();
    checkInternal(src->getSpace()->blockEquals(rank, dst->getSpace()));
    if (mps->getCommRank() < 0)
        return;

    const MultiprocessorSystem *dstMps = dst->getAlignRule()->getDistribRule()->getMPS();
    bool dstHasLocal = dstMps->getCommRank() >= 0 && dst->hasLocal();
    if (dstHasLocal) {
        Intervals *dstBlock = new Intervals[rank];
        for (int i = 1; i <= rank; i++) {
            if (dst->getAlignRule()->isIndirect(i)) {
                // TODO: In case of copying only part we need to tweak only dstBlock definition
                int dspaceAxis = dst->getAlignRule()->getDspaceAxis(i);
                const IndirectAxisDistribRule *axRule = dst->getAlignRule()->getDistribRule()->getAxisRule(dspaceAxis)->asIndirect();
                DvmType firstIndex;
                DvmType *local2ToGlobal = axRule->getLocal2ToGlobal(&firstIndex);
                local2ToGlobal += axRule->getLocalElems()[0] - firstIndex; // Only our own elements
                const DvmhAxisAlignRule *axAlRule = dst->getAlignRule()->getAxisRule(dspaceAxis);
                dstBlock[i - 1].uniteElements(local2ToGlobal, axRule->getLocalElems().size(), axAlRule->multiplier, axAlRule->summand, true);
            } else {
                dstBlock[i - 1].append(dst->getLocalPart()[i - 1]);
            }
        }
        dst->setActual(dst->getLocalPart());
        copyIntervalsBlock(src->getBuffer(0), dstBlock, dst->getBuffer(0), dst->getLocalPart(), true);
    }
}

static void copyBlockToIndirect(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    checkInternal(src->isDistributed() && !src->getAlignRule()->hasIndirect());
    checkInternal(dst->isDistributed() && dst->getAlignRule()->hasIndirect());
    checkInternal(src->getRank() == dst->getRank());
    int rank = src->getRank();
    checkInternal(src->getSpace()->blockEquals(rank, dst->getSpace()));
    if (mps->getCommRank() < 0)
        return;

    // TODO: Employ iSend technique from stdio instead of srcSeen
    // TODO: Satisfy locally what we can

    // We accomplish this in several steps:
    // 1. Figure out dst's local part on each processor
    // 2. Go through src's block distribution and figure out which elements from which processors do we need
    // 3. Make exchange in alltoallv1 manner with requests
    // 4. Get actual to host
    // 5. Make exchange in alltoallv2 manner with data
    // 6. Write to local part
    // 7. Set actual

    const MultiprocessorSystem *srcMps = src->getAlignRule()->getDistribRule()->getMPS();
    const MultiprocessorSystem *dstMps = dst->getAlignRule()->getDistribRule()->getMPS();
    bool dstHasLocal = dstMps->getCommRank() >= 0 && dst->hasLocal();
    Intervals *dstBlock = new Intervals[rank];
    if (dstHasLocal) {
        for (int i = 1; i <= rank; i++) {
            if (dst->getAlignRule()->isIndirect(i)) {
                // TODO: In case of copying only part we need to tweak only dstBlock definition
                int dspaceAxis = dst->getAlignRule()->getDspaceAxis(i);
                const IndirectAxisDistribRule *axRule = dst->getAlignRule()->getDistribRule()->getAxisRule(dspaceAxis)->asIndirect();
                DvmType firstIndex;
                DvmType *local2ToGlobal = axRule->getLocal2ToGlobal(&firstIndex);
                local2ToGlobal += axRule->getLocalElems()[0] - firstIndex; // Only our own elements
                const DvmhAxisAlignRule *axAlRule = dst->getAlignRule()->getAxisRule(dspaceAxis);
                dstBlock[i - 1].uniteElements(local2ToGlobal, axRule->getLocalElems().size(), axAlRule->multiplier, axAlRule->summand, true);
            } else {
                dstBlock[i - 1].append(dst->getLocalPart()[i - 1]);
            }
        }
    }

    UDvmType *sendSizes = new UDvmType[mps->getCommSize()];
    UDvmType *recvSizes = new UDvmType[mps->getCommSize()];
    UDvmType *dataRecvSizes = new UDvmType[mps->getCommSize()];
    char **sendBuffers = new char *[mps->getCommSize()];
    char **recvBuffers = new char *[mps->getCommSize()];
    char **dataRecvBuffers = new char *[mps->getCommSize()];
    Interval **needFromLocal = new Interval *[mps->getCommSize()];
    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        sendSizes[proc] = 0;
        sendBuffers[proc] = 0;
        recvSizes[proc] = 0;
        recvBuffers[proc] = 0;
        dataRecvSizes[proc] = 0;
        dataRecvBuffers[proc] = 0;
        needFromLocal[proc] = 0;
    }

    // Preparing requests to src's owners
    if (dstHasLocal) {
        DvmhPieces *srcSeen = new DvmhPieces(rank);
        int srcDspaceRank = src->getAlignRule()->getDspace()->getRank();
        for (int proc = 0; proc < mps->getCommSize(); proc++) {
            int srcProc = mps->getChildCommRank(srcMps, proc);
            if (srcProc >= 0) {
#ifdef NON_CONST_AUTOS
                Interval srcDspaceBlock[srcDspaceRank], srcBlock[rank];
#else
                Interval srcDspaceBlock[MAX_DISTRIB_SPACE_RANK], srcBlock[MAX_ARRAY_RANK];
#endif
                bool srcHasLocal = src->getAlignRule()->getDistribRule()->fillLocalPart(srcProc, srcDspaceBlock);
                srcBlock->blockAssign(rank, src->getSpace());
                srcHasLocal = srcHasLocal && src->getAlignRule()->mapOnPart(srcDspaceBlock, srcBlock, false);
                if (srcHasLocal) {
                    // For each block we either have already seen it entirely or have not seen any part of it
                    DvmhPieces *p = new DvmhPieces(rank);
                    p->appendOne(srcBlock);
                    DvmhPieces *p2 = srcSeen->intersect(p);
                    if (p2->isEmpty()) {
                        srcSeen->appendOne(srcBlock);
                        Intervals *curDstBlock = new Intervals[rank];
                        UDvmType totalMessageSize = 0;
                        bool notEmpty = true;
                        for (int i = 0; i < rank; i++) {
                            curDstBlock[i] = dstBlock[i];
                            curDstBlock[i].intersect(srcBlock[i]);
                            notEmpty = notEmpty && !curDstBlock[i].empty();
                            totalMessageSize += (1 + curDstBlock[i].getIntervalCount() * 2) * sizeof(DvmType);
                        }
                        if (notEmpty) {
                            sendSizes[proc] = totalMessageSize;
                            sendBuffers[proc] = new char[totalMessageSize];
                            UDvmType totalElemCount = 1;
                            BufferWalker walk(sendBuffers[proc], totalMessageSize);
                            for (int i = 0; i < rank; i++) {
                                UDvmType axisElemCount = 0;
                                walk.putValue((DvmType)curDstBlock[i].getIntervalCount());
                                for (int j = 0; j < curDstBlock[i].getIntervalCount(); j++) {
                                    walk.putValue(curDstBlock[i].getInterval(j)[0]);
                                    walk.putValue(curDstBlock[i].getInterval(j)[1]);
                                    axisElemCount += curDstBlock[i].getInterval(j).size();
                                }
                                totalElemCount *= axisElemCount;
                            }
#ifdef NON_CONST_AUTOS
                            DvmType beginIndexes[rank], endIndexes[rank];
#else
                            DvmType beginIndexes[MAX_ARRAY_RANK], endIndexes[MAX_ARRAY_RANK];
#endif
                            for (int i = 0; i < rank; i++) {
                                Interval boundInterval = curDstBlock[i].getBoundInterval();
                                beginIndexes[i] = boundInterval[0];
                                endIndexes[i] = boundInterval[1];
                            }
                            needFromLocal[proc] = new Interval[rank];
                            bool convertRes = dst->convertGlobalToLocal2(beginIndexes, true);
                            assert(convertRes);
                            convertRes = dst->convertGlobalToLocal2(endIndexes, true);
                            assert(convertRes);
                            for (int i = 0; i < rank; i++) {
                                needFromLocal[proc][i][0] = beginIndexes[i];
                                needFromLocal[proc][i][1] = endIndexes[i];
                            }
                            dataRecvSizes[proc] = totalElemCount * dst->getTypeSize();
                            dataRecvBuffers[proc] = new char[dataRecvSizes[proc]];
                        }
                        delete[] curDstBlock;
                    }
                    delete p2;
                    delete p;
                }
            }
        }
        delete srcSeen;
        srcSeen = 0;
    }
    delete[] dstBlock;
    dstBlock = 0;

    mps->alltoallv1(sendSizes, sendBuffers, recvSizes, recvBuffers);

    // Preparing answer with data from src's local parts
    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        sendSizes[proc] = 0;
        delete[] sendBuffers[proc];
        sendBuffers[proc] = 0;

        if (recvSizes[proc] > 0) {
            Intervals *srcDataToSend = new Intervals[rank];
            UDvmType totalElemCount = 1;
#ifdef NON_CONST_AUTOS
            Interval toGetActual[rank];
#else
            Interval toGetActual[MAX_ARRAY_RANK];
#endif
            BufferWalker walk(recvBuffers[proc], recvSizes[proc]);
            for (int i = 0; i < rank; i++) {
                UDvmType axisElemCount = 0;
                int count = walk.extractValue<DvmType>();
                for (int j = 0; j < count; j++) {
                    DvmType start = walk.extractValue<DvmType>();
                    DvmType end = walk.extractValue<DvmType>();
                    srcDataToSend[i].append(Interval::create(start, end));
                    axisElemCount += (end - start + 1);
                }
                totalElemCount *= axisElemCount;
                toGetActual[i] = srcDataToSend[i].getBoundInterval();
            }
            sendSizes[proc] = totalElemCount * dst->getTypeSize();
            sendBuffers[proc] = new char[sendSizes[proc]];
            {
                PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpArrayCopy);
                src->getActualBaseOne(0, toGetActual, 0, true);
            }
            copyIntervalsBlock(src->getBuffer(0), srcDataToSend, sendBuffers[proc], true);
        }

        recvSizes[proc] = 0;
        delete[] recvBuffers[proc];
        recvBuffers[proc] = 0;
    }

    mps->alltoallv2(sendSizes, sendBuffers, dataRecvSizes, dataRecvBuffers);

    // Copying to dst
    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        sendSizes[proc] = 0;
        delete[] sendBuffers[proc];
        sendBuffers[proc] = 0;

        if (dataRecvSizes[proc] > 0) {
            assert(needFromLocal[proc]);
            assert(needFromLocal[proc]->blockSize(rank) * dst->getTypeSize() == dataRecvSizes[proc]);
            DvmhBuffer wrap(rank, dst->getTypeSize(), 0, needFromLocal[proc], dataRecvBuffers[proc]);
            dst->setActual(needFromLocal[proc]);
            wrap.copyTo(dst->getBuffer(0), needFromLocal[proc]);
        }

        dataRecvSizes[proc] = 0;
        delete[] dataRecvBuffers[proc];
        dataRecvBuffers[proc] = 0;
        delete[] needFromLocal[proc];
        needFromLocal[proc] = 0;
    }

    for (int p = 0; p < mps->getCommSize(); p++) {
        delete[] sendBuffers[p];
        delete[] recvBuffers[p];
        delete[] dataRecvBuffers[p];
        delete[] needFromLocal[p];
    }
    delete[] sendBuffers;
    delete[] recvBuffers;
    delete[] dataRecvBuffers;
    delete[] needFromLocal;
    delete[] sendSizes;
    delete[] recvSizes;
    delete[] dataRecvSizes;
}

static void copyIndirectToBlock(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    checkInternal(src->isDistributed() && src->getAlignRule()->hasIndirect());
    checkInternal(dst->isDistributed() && !dst->getAlignRule()->hasIndirect());
    checkInternal(src->getRank() == dst->getRank());
    int rank = src->getRank();
    checkInternal(src->getSpace()->blockEquals(rank, dst->getSpace()));
    if (mps->getCommRank() < 0)
        return;

    // TODO: Employ iSend technique from stdio instead of going through processors
    // TODO: Satisfy locally what we can

    // We accomplish this in several steps:
    // 1. Figure out src's local part on each processor
    // 2. Go through dst's block distribution and figure out which processor which elements needs from us
    // 3. Get actual to host
    // 4. Make exchange in alltoallv1 manner with data and description of what we have sent
    // 5. Write to local part
    // 6. Set actual

    const MultiprocessorSystem *srcMps = src->getAlignRule()->getDistribRule()->getMPS();
    const MultiprocessorSystem *dstMps = dst->getAlignRule()->getDistribRule()->getMPS();
    bool srcHasLocal = srcMps->getCommRank() >= 0 && src->hasLocal();
    Intervals *srcBlock = new Intervals[rank];
    if (srcHasLocal) {
        for (int i = 1; i <= rank; i++) {
            if (src->getAlignRule()->isIndirect(i)) {
                // TODO: In case of copying only part we need to tweak only dstBlock definition
                int dspaceAxis = src->getAlignRule()->getDspaceAxis(i);
                const IndirectAxisDistribRule *axRule = src->getAlignRule()->getDistribRule()->getAxisRule(dspaceAxis)->asIndirect();
                DvmType firstIndex;
                DvmType *local2ToGlobal = axRule->getLocal2ToGlobal(&firstIndex);
                local2ToGlobal += axRule->getLocalElems()[0] - firstIndex; // Only our own elements
                const DvmhAxisAlignRule *axAlRule = src->getAlignRule()->getAxisRule(dspaceAxis);
                srcBlock[i - 1].uniteElements(local2ToGlobal, axRule->getLocalElems().size(), axAlRule->multiplier, axAlRule->summand, true);
            } else {
                srcBlock[i - 1].append(src->getLocalPart()[i - 1]);
            }
        }
    }

    UDvmType *sendSizes = new UDvmType[mps->getCommSize()];
    UDvmType *recvSizes = new UDvmType[mps->getCommSize()];
    char **sendBuffers = new char *[mps->getCommSize()];
    char **recvBuffers = new char *[mps->getCommSize()];
    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        sendSizes[proc] = 0;
        sendBuffers[proc] = 0;
        recvSizes[proc] = 0;
        recvBuffers[proc] = 0;
    }

    // Preparing data with descriptions to send to dst's owners
    bool iSend = srcHasLocal;
    if (srcHasLocal) {
        int srcDspaceRank = src->getAlignRule()->getDspace()->getRank();
#ifdef NON_CONST_AUTOS
        Interval srcDspaceBlock[srcDspaceRank], curSrcBlock[rank], mySrcBlock[rank];
#else
        Interval srcDspaceBlock[MAX_DISTRIB_SPACE_RANK], curSrcBlock[MAX_ARRAY_RANK], mySrcBlock[MAX_ARRAY_RANK];
#endif
        mySrcBlock->blockAssign(rank, src->getSpace());
        src->getAlignRule()->mapOnPart(src->getAlignRule()->getDistribRule()->getLocalPart(), mySrcBlock, false);
        for (int proc = 0; proc < mps->getCommRank(); proc++) {
            int srcProc = mps->getChildCommRank(srcMps, proc);
            if (srcProc >= 0) {
                bool curSrcHasLocal = src->getAlignRule()->getDistribRule()->fillLocalPart(srcProc, srcDspaceBlock);
                curSrcBlock->blockAssign(rank, src->getSpace());
                curSrcHasLocal = curSrcHasLocal && src->getAlignRule()->mapOnPart(srcDspaceBlock, curSrcBlock, false);
                if (curSrcHasLocal && curSrcBlock->blockEquals(rank, mySrcBlock)) {
                    iSend = false;
                    break;
                }
            }
        }
    }
    if (srcHasLocal && iSend) {
        int dstDspaceRank = dst->getAlignRule()->getDspace()->getRank();
        for (int proc = 0; proc < mps->getCommSize(); proc++) {
            int dstProc = mps->getChildCommRank(dstMps, proc);
            if (dstProc >= 0) {
#ifdef NON_CONST_AUTOS
                Interval dstDspaceBlock[dstDspaceRank], dstBlock[rank];
#else
                Interval dstDspaceBlock[MAX_DISTRIB_SPACE_RANK], dstBlock[MAX_ARRAY_RANK];
#endif
                bool dstHasLocal = dst->getAlignRule()->getDistribRule()->fillLocalPart(dstProc, dstDspaceBlock);
                dstBlock->blockAssign(rank, dst->getSpace());
                dstHasLocal = dstHasLocal && dst->getAlignRule()->mapOnPart(dstDspaceBlock, dstBlock, false);
                if (dstHasLocal) {
                    Intervals *curSrcBlock = new Intervals[rank];
                    bool notEmpty = true;
                    UDvmType totalMessageSize = 0;
                    UDvmType totalElemCount = 1;
                    for (int i = 0; i < rank; i++) {
                        curSrcBlock[i] = srcBlock[i];
                        curSrcBlock[i].intersect(dstBlock[i]);
                        notEmpty = notEmpty && !curSrcBlock[i].empty();
                        totalMessageSize += (1 + curSrcBlock[i].getIntervalCount() * 2) * sizeof(DvmType);
                        UDvmType axisElemCount = 0;
                        for (int j = 0; j < curSrcBlock[i].getIntervalCount(); j++) {
                            axisElemCount += curSrcBlock[i].getInterval(j).size();
                        }
                        totalElemCount *= axisElemCount;
                    }
                    totalMessageSize += totalElemCount * dst->getTypeSize();
                    if (notEmpty) {
                        sendSizes[proc] = totalMessageSize;
                        sendBuffers[proc] = new char[totalMessageSize];
                        BufferWalker walk(sendBuffers[proc], totalMessageSize);
                        // Putting metadata (array of Intervals)
                        for (int i = 0; i < rank; i++) {
                            walk.putValue((DvmType)curSrcBlock[i].getIntervalCount());
                            for (int j = 0; j < curSrcBlock[i].getIntervalCount(); j++) {
                                walk.putValue(curSrcBlock[i].getInterval(j)[0]);
                                walk.putValue(curSrcBlock[i].getInterval(j)[1]);
                            }
                        }
                        // Putting data itself
#ifdef NON_CONST_AUTOS
                        DvmType beginIndexes[rank], endIndexes[rank];
                        Interval localSrcBlock[rank];
#else
                        DvmType beginIndexes[MAX_ARRAY_RANK], endIndexes[MAX_ARRAY_RANK];
                        Interval localSrcBlock[MAX_ARRAY_RANK];
#endif
                        for (int i = 0; i < rank; i++) {
                            Interval boundInterval = curSrcBlock[i].getBoundInterval();
                            beginIndexes[i] = boundInterval[0];
                            endIndexes[i] = boundInterval[1];
                        }
                        bool convertRes = src->convertGlobalToLocal2(beginIndexes, true);
                        assert(convertRes);
                        convertRes = src->convertGlobalToLocal2(endIndexes, true);
                        assert(convertRes);
                        for (int i = 0; i < rank; i++) {
                            localSrcBlock[i][0] = beginIndexes[i];
                            localSrcBlock[i][1] = endIndexes[i];
                        }
                        assert(localSrcBlock->blockSize(rank) == totalElemCount);
                        {
                            PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpArrayCopy);
                            src->getActualBaseOne(0, localSrcBlock, 0, true);
                        }
                        DvmhBuffer wrap(rank, src->getTypeSize(), 0, localSrcBlock, walk.getDataInplace(totalElemCount * src->getTypeSize()));
                        src->getBuffer(0)->copyTo(&wrap, localSrcBlock);
                    }
                    delete[] curSrcBlock;
                }
            }
        }
    }
    delete[] srcBlock;
    srcBlock = 0;

    mps->alltoallv1(sendSizes, sendBuffers, recvSizes, recvBuffers);

    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        delete[] sendBuffers[proc];
        sendBuffers[proc] = 0;
        sendSizes[proc] = 0;
    }

    for (int proc = 0; proc < mps->getCommSize(); proc++) {
        if (recvSizes[proc] > 0) {
            Intervals *dstDataArrived = new Intervals[rank];
            UDvmType totalElemCount = 1;
#ifdef NON_CONST_AUTOS
            Interval toSetActual[rank];
#else
            Interval toSetActual[MAX_ARRAY_RANK];
#endif
            BufferWalker walk(recvBuffers[proc], recvSizes[proc]);
            for (int i = 0; i < rank; i++) {
                UDvmType axisElemCount = 0;
                int count = walk.extractValue<DvmType>();
                for (int j = 0; j < count; j++) {
                    DvmType start = walk.extractValue<DvmType>();
                    DvmType end = walk.extractValue<DvmType>();
                    dstDataArrived[i].append(Interval::create(start, end));
                    axisElemCount += (end - start + 1);
                }
                totalElemCount *= axisElemCount;
                // XXX: Since we copy all the data it is fine to use boundInterval. We can even set actual the whole local part. But if we will be copying only part - take care. But, anyway, if copying part is a solid segment in global indexes, bound interval is still a working approach.
                toSetActual[i] = dstDataArrived[i].getBoundInterval();
            }
            dst->setActual(toSetActual);
            copyIntervalsBlock(dst->getBuffer(0), dstDataArrived, walk.getDataInplace(totalElemCount * dst->getTypeSize()), false);
        }

        recvSizes[proc] = 0;
        delete[] recvBuffers[proc];
        recvBuffers[proc] = 0;
    }

    for (int p = 0; p < mps->getCommSize(); p++) {
        delete[] sendBuffers[p];
        delete[] recvBuffers[p];
    }
    delete[] sendBuffers;
    delete[] recvBuffers;
    delete[] sendSizes;
    delete[] recvSizes;
}

static void copyIndirectToReplicated(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    // We accomplish this in several steps:
    // 1. Figure out src's local part on each processor
    // 2. Go through src's distribution and figure out which processor sends and how many elements (we still don't know which elements)
    // 3. Get actual to host
    // 4. Make exchange in alltoallv2/3 or allgatherv manner with data and description of what we have sent (not Intervals, the whole local2ToGlobal array, size of the message must be known beforehand)
    // 5. Write to local part
    // 6. Set actual

    checkInternal2(0, "Not implemented yet");
}

static void copyIndirectToIndirect(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    // Some thoughts:
    // Since both sides (src and dst) don't know each others distributions, we have a symmetrical situation and it will be better to gather info on src's side so that we avoid sending one more 'request' message
    // So each processor knows his own elements of src and tries to find out to which processors do they belong
    // Uses its own globalToLocal, makes request in alltoallv1 manner to processor which know distribution.
    // When the processor understands where each element to send it makes alltoallv1 like in IndirectToBlock with data and array of Intervals (and actually we can make these Intervals in dst's local indexes right away and apply them exactly as in IndirectToBlock case)

    checkInternal2(0, "Not implemented yet");
}

static void copyBlockToReplicated(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    checkInternal(src->isDistributed() && !src->getAlignRule()->hasIndirect());
    checkInternal(!dst->isDistributed());
    checkInternal(src->getRank() == dst->getRank());
    int rank = src->getRank();
    checkInternal(src->getSpace()->blockEquals(rank, dst->getSpace()));
    if (mps->getCommRank() < 0)
        return;

    checkInternal2(0, "Not implemented yet");
}

static void copyBlockToBlock(DvmhData *src, DvmhData *dst, const MultiprocessorSystem *mps) {
    checkInternal2(0, "Not implemented yet");
}

void dvmhCopyArrayWhole(DvmhData *src, DvmhData *dst) {
    checkInternal(src->isAligned() && dst->isAligned());
    checkInternal(src->getRank() == dst->getRank());
    int rank = src->getRank();
    checkInternal(src->getSpace()->blockEquals(rank, dst->getSpace()));
    checkInternal(src->getTypeSize() == dst->getTypeSize());
    const MultiprocessorSystem *mps = 0;
    static const char message[] = "Array's multiprocessor system must be a subsystem of the current multiprocessor system";
    if (src->isDistributed() && !dst->isDistributed()) {
        const MultiprocessorSystem *mps1 = src->getAlignRule()->getDistribRule()->getMPS();
        assert(mps1);
        // XXX: Ideally, we should take rootMPS, but in reality some non-distributed arrays can be used as temporary in subtasks, so we fill them as wide as possible
        mps = currentMPS;
        checkError2(mps1->isSubsystemOf(mps), message);
    } else if (!src->isDistributed() && dst->isDistributed()) {
        mps = dst->getAlignRule()->getDistribRule()->getMPS();
        assert(mps);
    } else if (src->isDistributed() && dst->isDistributed()) {
        const MultiprocessorSystem *mps1 = src->getAlignRule()->getDistribRule()->getMPS();
        const MultiprocessorSystem *mps2 = dst->getAlignRule()->getDistribRule()->getMPS();
        assert(mps1);
        assert(mps2);
        if (mps1->isSubsystemOf(mps2)) {
            mps = mps2;
        } else if (mps2->isSubsystemOf(mps1)) {
            mps = mps1;
        } else {
            // TODO: Maybe find somehow minimal equitant multiprocessor system (and erase the following checks)
            mps = currentMPS;
            checkError2(mps1->isSubsystemOf(mps), message);
            checkError2(mps2->isSubsystemOf(mps), message);
        }
    }
    if (mps) {
        checkError2(mps->isSubsystemOf(currentMPS), message);
    }

    if (!src->isDistributed() && !dst->isDistributed()) {
        // Replicated to replicated
        {
            PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpArrayCopy);
            src->getActualBaseOne(0, src->getSpace(), 0, true);
        }
        src->getBuffer(0)->copyTo(dst->getBuffer(0), src->getSpace());
        dst->setActual(dst->getSpace());
    } else if (!src->isDistributed() && !dst->getAlignRule()->hasIndirect()) {
        // Replicated to block
        if (dst->hasLocal()) {
            {
                PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpArrayCopy);
                src->getActualBaseOne(0, dst->getLocalPart(), 0, true);
            }
            src->getBuffer(0)->copyTo(dst->getBuffer(0), dst->getLocalPart());
            dst->setActual(dst->getLocalPart());
        }
    } else if (!src->isDistributed()) {
        copyReplicatedToIndirect(src, dst, mps);
    } else if (!src->getAlignRule()->hasIndirect() && !dst->isDistributed()) {
        copyBlockToReplicated(src, dst, mps);
    } else if (!src->getAlignRule()->hasIndirect() && !dst->getAlignRule()->hasIndirect()) {
        copyBlockToBlock(src, dst, mps);
    } else if (!src->getAlignRule()->hasIndirect()) {
        copyBlockToIndirect(src, dst, mps);
    } else if (!dst->isDistributed()) {
        copyIndirectToReplicated(src, dst, mps);
    } else if (!dst->getAlignRule()->hasIndirect()) {
        copyIndirectToBlock(src, dst, mps);
    } else {
        copyIndirectToIndirect(src, dst, mps);
    }
}

}
