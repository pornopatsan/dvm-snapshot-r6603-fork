#include "region.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_device.h"
#include "dvmh_pieces.h"
// TODO: Get rid of this dependency
#include "dvmh_rts.h"
#include "dvmh_stat.h"
#include "loop.h"
#include "util.h"

namespace libdvmh {

// DvmhRegionDistribSpace

DvmhRegionDistribSpace::DvmhRegionDistribSpace(DvmhDistribSpace *aDspace): dspace(aDspace) {
    localParts = new Interval *[devicesCount];
    weights = new double[devicesCount];
    for (int i = 0; i < devicesCount; i++) {
        localParts[i] = 0;
        weights[i] = 0.0;
    }
    dspace->addRef();
}

Interval *DvmhRegionDistribSpace::addLocalPart(int dev, double wgt) {
    checkInternal(localParts[dev] == 0);
    localParts[dev] = new Interval[dspace->getRank()];
    std::fill(localParts[dev], localParts[dev] + dspace->getRank(), Interval::createEmpty());
    weights[dev] = wgt;
    return localParts[dev];
}

DvmhRegionDistribSpace::~DvmhRegionDistribSpace() {
    assert(localParts != 0);
    for (int i = 0; i < devicesCount; i++)
        delete[] localParts[i];
    delete[] localParts;
    localParts = 0;
    delete[] weights;
    dspace->removeRef();
}

// DvmhRegionData

DvmhRegionData::DvmhRegionData(DvmhData *aData) {
    renewFlag = false;
    data = aData;
    int rank = data->getRank();
    inPieces = new DvmhPieces(rank);
    outPieces = new DvmhPieces(rank);
    localPieces = new DvmhPieces(rank);
    {
        char buf[100];
        if (data->hasLocal())
            buf[sprintf(buf, "%p", data->getBuffer(0)->getDeviceAddr())] = 0;
        else
            buf[sprintf(buf, "%p", data)] = 0;
        varName = new char[strlen(buf) + 1];
        strcpy(varName, buf);
    }
    localParts = new Interval *[devicesCount];
    for (int i = 0; i < devicesCount; i++)
        localParts[i] = 0;
    varId = -1;
}

bool DvmhRegionData::hasInPieces() const {
    return inPieces->getCount() > 0;
}

bool DvmhRegionData::hasOutPieces() const {
    return outPieces->getCount() > 0;
}

bool DvmhRegionData::hasLocalPieces() const {
    return localPieces->getCount() > 0;
}

Interval *DvmhRegionData::addLocalPart(int dev) {
    checkInternal(localParts[dev] == 0);
    localParts[dev] = new Interval[data->getRank()];
    std::fill(localParts[dev], localParts[dev] + data->getRank(), Interval::createEmpty());
    return localParts[dev];
}

void DvmhRegionData::setName(const char *name, int nameLength) {
    delete[] varName;
    varName = new char[nameLength + 1];
    strncpy(varName, name, nameLength);
    varName[nameLength] = 0;
}

DvmhRegionData::~DvmhRegionData() {
    assert(inPieces != 0);
    delete inPieces;
    inPieces = 0;
    assert(outPieces != 0);
    delete outPieces;
    outPieces = 0;

    assert(localPieces != 0);
    data->clearActual(localPieces);
    delete localPieces;
    localPieces = 0;
    delete[] varName;
    varName = 0;

    assert(localParts != 0);
    for (int i = 0; i < devicesCount; i++)
        delete[] localParts[i];
    delete[] localParts;
    localParts = 0;
}

// DvmhDataDep

void DvmhDataDep::addState(const DvmhDataState &state) {
    if (state.has(myVar)) {
        selfCount++;
    } else {
        stateToCount[state]++;
        otherCount++;
    }
}

// SourcePosition

bool SourcePosition::operator<(const SourcePosition &sp) const {
    if (fileNameHash < sp.fileNameHash) {
        return true;
    } else if (fileNameHash > sp.fileNameHash) {
        return false;
    } else {
        int cmpRes = strcmp(fileName.c_str(), sp.fileName.c_str());
        if (cmpRes < 0)
            return true;
        else if (cmpRes > 0)
            return false;
        else
            return lineNumber < sp.lineNumber;
    }
}

// DvmhRegionPersistentInfo

DvmhRegionPersistentInfo::DvmhRegionPersistentInfo(const SourcePosition &sp, int number): sourcePos(sp), appearanceNumber(number), execCount(0) {
    latestRegionEnd = new AggregateEvent;
}

int DvmhRegionPersistentInfo::getVarId(std::string name) {
    std::map<std::string, int>::iterator it = nameToId.find(name);
    int res;
    if (it == nameToId.end()) {
        res = nameToId.size();
        nameToId.insert(std::make_pair(name, res));
        dataDeps.push_back(DvmhDataDep(this, res));
    } else {
        res = it->second;
    }
    return res;
}

void DvmhRegionPersistentInfo::addDataDep(DvmhRegionData *rdata) {
    int varId = rdata->getVarId();
    DvmhDataDep &dep = dataDeps[varId];
    dep.addState(rdata->getData()->getCurState());
}

void DvmhRegionPersistentInfo::setLatestRegionEnd(DvmhEvent *event, bool owning) {
    delete latestRegionEnd;
    if (owning)
        latestRegionEnd = event;
    else
        latestRegionEnd = event->dup();
}

DvmhRegionPersistentInfo::~DvmhRegionPersistentInfo() {
    delete latestRegionEnd;
}

// DvmhRegionMapping

static double calcTimeFromWeights(const std::vector<const DvmhLoopPersistentInfo *> &loopInfos, const std::vector<int> &deviceNumbers,
        const std::vector<double> &curWeights, std::vector<double> *pCurGradient = 0) {
    // Weights MUST have sum equal to one
    double curTime = 0;
    double worstPerf = 0;
    int worstInd = 0;
    double worstDelta = 0;
    for (int j = 0; j < (int)loopInfos.size(); j++) {
        std::vector<double> curLoopTimes(deviceNumbers.size(), -1);
        std::vector<double> curLoopPerfs(deviceNumbers.size(), 0);
        double loopTime = -1;
        for (int i = 0; i < (int)deviceNumbers.size(); i++) {
            if (curWeights[i] > 0) {
                double devTime = -1;
                loopInfos[j]->getBestParams(deviceNumbers[i], curWeights[i], &devTime);
                checkInternal2(devTime > 0, "Time of loop must be positive");
                devTime *= loopInfos[j]->getExecCount();
                curLoopTimes[i] = devTime;
                curLoopPerfs[i] = curWeights[i] / curLoopTimes[i];
                if (loopTime < devTime)
                    loopTime = devTime;
            }
        }
        checkInternal2(loopTime > 0, "Time of loop must be positive");
        curTime += loopTime;
        if (pCurGradient) {
            int baseInd = -1;
            double basePerf;
            for (int i = 0; i < (int)deviceNumbers.size(); i++) {
                if (curWeights[i] > 0) {
                    baseInd = i;
                    basePerf = curLoopPerfs[i];
                    break;
                }
            }
            checkInternal2(baseInd >= 0, "At least one device should have non-zero weight");
            double coef = 1;
            for (int i = baseInd + 1; i < (int)deviceNumbers.size(); i++) {
                if (curWeights[i] > 0)
                    coef += curLoopPerfs[i] / basePerf;
            }
            std::vector<double> aimWeights(deviceNumbers.size(), 0);
            aimWeights[baseInd] = 1.0 / coef;
            for (int i = baseInd + 1; i < (int)deviceNumbers.size(); i++) {
                if (curWeights[i] > 0)
                    aimWeights[i] = curLoopPerfs[i] / basePerf * aimWeights[baseInd];
            }
            // OK, we have best weights for this loop in aimWeights
            if (loopInfos.size() == 1) {
                // If one loop - we can find exact solution
                pCurGradient->resize(deviceNumbers.size());
                for (int i = 0; i < (int)deviceNumbers.size(); i++)
                    pCurGradient->at(i) = aimWeights[i] - curWeights[i];
            } else {
                // If many - lower one worst device down
                int curWorstInd = 0;
                for (int i = 0; i < (int)deviceNumbers.size(); i++) {
                    if (curLoopTimes[i] > curLoopTimes[curWorstInd])
                        curWorstInd = i;
                }
                double curWorstPerf = curLoopPerfs[curWorstInd];
                double curWorstDelta = aimWeights[curWorstInd] - curWeights[curWorstInd];
                if (j == 0 || curWorstPerf < worstPerf) {
                    worstPerf = curWorstPerf;
                    worstInd = curWorstInd;
                    worstDelta = curWorstDelta;
                }
            }
        }
    }
    if (loopInfos.size() > 1 && pCurGradient) {
        // Lower one worst device down
        pCurGradient->resize(deviceNumbers.size());
        for (int i = 0; i < (int)deviceNumbers.size(); i++) {
            if (i == worstInd)
                pCurGradient->at(i) = worstDelta;
            else
                pCurGradient->at(i) = (-worstDelta) / (deviceNumbers.size() - 1);
        }
    }
    return curTime;
}

static void calcBestWeights(const std::vector<const DvmhLoopPersistentInfo *> &loopInfos, const std::vector<int> &deviceNumbers, std::vector<double> &vec) {
    std::vector<double> bestWeights;
    if (!loopInfos.empty()) {
        double bestTime = -1;
        // Balance load
        // Firstly check what we are given
        {
            std::vector<double> curWeights(deviceNumbers.size(), 0);
            double sumWgt = 0.0;
            for (int i = 0; i < (int)deviceNumbers.size(); i++) {
                double wgt = vec[deviceNumbers[i]];
                curWeights[i] = wgt;
                sumWgt += wgt;
            }
            if (sumWgt > 0) {
                for (int i = 0; i < (int)deviceNumbers.size(); i++)
                    curWeights[i] /= sumWgt;
                double curTime = calcTimeFromWeights(loopInfos, deviceNumbers, curWeights);
                if (curTime > 0 && (curTime < bestTime || bestTime < 0)) {
                    bestTime = curTime;
                    bestWeights = curWeights;
                }
            }
        }
        // Secondly check using only one device
        for (int i = 0; i < (int)deviceNumbers.size(); i++) {
            std::vector<double> curWeights(deviceNumbers.size(), 0);
            curWeights[i] = 1.0;
            double curTime = calcTimeFromWeights(loopInfos, deviceNumbers, curWeights);
            curTime *= 0.95; // Discount 5 per cent to make preference for single-device configurations
            if (curTime > 0 && (curTime < bestTime || bestTime < 0)) {
                bestTime = curTime;
                bestWeights = curWeights;
            }
        }
        // Then try to improve it using work-sharing
        std::vector<double> curWeights(deviceNumbers.size(), 1.0 / deviceNumbers.size());
        bool prettyGood = false;
        int iterLeft = 100;
        while (!prettyGood && iterLeft > 0) {
            iterLeft--;
            std::vector<double> curGradient(deviceNumbers.size(), 0.0);
            double curTime = calcTimeFromWeights(loopInfos, deviceNumbers, curWeights, &curGradient);
            if (curTime > 0 && (curTime < bestTime || bestTime < 0)) {
                bestTime = curTime;
                bestWeights = curWeights;
            }
            prettyGood = true;
            for (int i = 0; i < (int)deviceNumbers.size(); i++)
                prettyGood = prettyGood && fabs(curGradient[i]) < 0.01;
            if (!prettyGood) {
                double sum = 0;
                for (int i = 0; i < (int)deviceNumbers.size(); i++) {
                    curWeights[i] += curGradient[i];
                    if (curWeights[i] < 0)
                        curWeights[i] = 0;
                    sum += curWeights[i];
                }
                if (sum <= 0) {
                    dvmh_log(DEBUG, "Somehow gradient method ended in zero point (o_O)");
                    break;
                }
                for (int i = 0; i < (int)deviceNumbers.size(); i++)
                    curWeights[i] /= sum;
            }
        }
        dvmh_log(DEBUG, "Gradient method ended in %d iterations", 100 - iterLeft);
    }
    // Apply best distribution
    for (int i = 0; i < (int)deviceNumbers.size(); i++) {
        if (!bestWeights.empty())
            vec[deviceNumbers[i]] = bestWeights[i];
        else
            vec[deviceNumbers[i]] = 1.0 / (int)deviceNumbers.size();
    }
}

DvmhRegionMapping::DvmhRegionMapping(DvmhRegion *region, unsigned long availDevices) {
    bool compareDebug = region->withCompareDebug();
    DvmhRegionPersistentInfo *persInfo = region->getPersistentInfo();
    usesDevices = availDevices - (compareDebug ? 1 : 0);
    if (dvmhSettings.schedTech == stSingleDevice) {
        for (int i = 1; i < devicesCount; i++) {
            if (usesDevices & (1ul << i)) {
                usesDevices = (1ul << i);
                break;
            }
        }
    }
    std::vector<int> deviceNumbers;
    for (int i = 0; i < devicesCount; i++) {
        if (usesDevices & (1ul << i))
            deviceNumbers.push_back(i);
    }
    checkInternal2(!deviceNumbers.empty(), "No devices chosen for mapping");
    rdspaces.clear();
    std::vector<double> universalWeights(devicesCount, 0);
    if (deviceNumbers.size() <= 1 || dvmhSettings.schedTech == stSingleDevice || dvmhSettings.schedTech == stSimpleStatic) {
        if (deviceNumbers.size() > 1) {
            for (int i = 0; i < (int)deviceNumbers.size(); i++)
                universalWeights[deviceNumbers[i]] = devices[deviceNumbers[i]]->getPerformance();
        } else {
            universalWeights[deviceNumbers[0]] = 1.0;
        }
        double sum = 0;
        for (int i = 0; i < devicesCount; i++)
            sum += universalWeights[i];
        checkError2(sum > 0, "Cannot determine data distribution in accordance with devices' performances: they are all zeroes");
    } else if ((dvmhSettings.schedTech == stSimpleDynamic || dvmhSettings.schedTech == stDynamic) && persInfo->getExecCount() < (int)deviceNumbers.size()) {
        universalWeights[deviceNumbers[persInfo->getExecCount()]] = 1.0;
    } else if (dvmhSettings.schedTech == stSimpleDynamic) {
        // Pick all the loops, which have performance information for every device in use
        std::vector<const DvmhLoopPersistentInfo *> loopInfos;
        loopInfos.reserve(loopDict.size());
        for (std::map<SourcePosition, DvmhLoopPersistentInfo *>::const_iterator it = loopDict.begin(); it != loopDict.end(); it++) {
            bool toInclude = true;
            for (int i = 0; i < (int)deviceNumbers.size(); i++)
                toInclude = toInclude && it->second->getBestParams(deviceNumbers[i], 1.0).first >= 0;
            if (toInclude)
                loopInfos.push_back(it->second);
        }
        for (int i = 0; i < (int)deviceNumbers.size(); i++)
            universalWeights[deviceNumbers[i]] = lastBestWeights[deviceNumbers[i]];
        calcBestWeights(loopInfos, deviceNumbers, universalWeights);
        int activeDevices = 0;
        for (int i = 0; i < devicesCount; i++) {
            if (devices[i]->hasSlots())
                activeDevices++;
        }
        if ((int)deviceNumbers.size() == activeDevices)
            lastBestWeights = universalWeights;
    } else if (dvmhSettings.schedTech == stUseScheme) {
        checkInternal2(0, "Cannot use scheme (not implemented yet)");
    }
    for (std::map<DvmhDistribSpace *, DvmhRegionDistribSpace *>::iterator it = region->getDspaces()->begin(); it != region->getDspaces()->end(); it++) {
        DvmhDistribSpace *dspace = it->first;
        std::vector<double> &vec = rdspaces[dspace];
        vec = universalWeights;
        if (dvmhSettings.schedTech == stDynamic && persInfo->getExecCount() >= (int)deviceNumbers.size()) {
            // Figure out which loops are mapped to current dspace
            std::vector<const DvmhLoopPersistentInfo *> loopInfos;
            loopInfos.reserve(persInfo->getLoopInfos().size());
            for (int i = 0; i < (int)persInfo->getLoopInfos().size(); i++) {
                const DvmhLoopPersistentInfo *loopInfo = persInfo->getLoopInfos()[i];
                int varId = loopInfo->getVarId();
                if (varId >= 0) {
                    DvmhData *data = region->getDataByVarId(varId);
                    if (data && data->isDistributed() && data->getAlignRule()->getDspace() == dspace) {
                        // Pick only loops, which have performance information for every device in use
                        bool toInclude = true;
                        for (int j = 0; j < (int)deviceNumbers.size(); j++)
                            toInclude = toInclude && loopInfo->getBestParams(deviceNumbers[j], 1.0).first >= 0;
                        if (toInclude)
                            loopInfos.push_back(loopInfo);
                    }
                }
            }
            calcBestWeights(loopInfos, deviceNumbers, vec);
        }
    }
    if (compareDebug)
        usesDevices += 1;
}

// DvmhRegion

DvmhRegion::DvmhRegion(int flags, DvmhRegionPersistentInfo *persInfo) {
    async = dvmhSettings.allowAsync && ((flags & REGION_ASYNC) != 0);
    compareDebug = dvmhSettings.compareDebug || ((flags & REGION_COMPARE_DEBUG) != 0);
    persistentInfo = persInfo;
    startPrereq = new AggregateEvent;
    latestLoopEnd = new AggregateEvent;
    phase = rpRegistrations;
}

void DvmhRegion::renewDatas() {
    checkInternal(phase == rpExecution);
    for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++) {
        DvmhRegionData *rdata = it->second;
        if (rdata->getRenewFlag()) {
            rdata->setRenewFlag(false);
            performRenew(rdata, true);
        }
    }
}

void DvmhRegion::renewData(DvmhData *data, DvmhPieces *restrictTo) {
    DvmhRegionData *rdata = dictFind2(datas, data);
    if (rdata) {
        performRenew(rdata, true, restrictTo);
    }
}

void DvmhRegion::registerData(DvmhData *data, int intent, const Interval indexes[]) {
    checkInternal(phase == rpRegistrations);
    int rank = data->getRank();
    DvmhRegionData *rdata = dictFind2(datas, data);
    if (!rdata) {
        rdata = new DvmhRegionData(data);
        datas[data] = rdata;
    }
    assert(rdata != 0);
    checkError2(!data->isIncomplete(), "Incomplete array is not allowed to be registered in region");
    if (intent & INTENT_IN)
        rdata->getInPieces()->uniteOne(indexes);
    // XXX: Warning. Here is localPlusShadow used as equitant part instead of localPart for OUT and LOCAL. But for what purpose?
    if (intent & INTENT_OUT)
        rdata->getOutPieces()->uniteOne(indexes);
    if (intent & INTENT_LOCAL)
        rdata->getLocalPieces()->uniteOne(indexes);
    dvmh_log(TRACE, "Registered real block for %s%s%s:", (intent & INTENT_IN ? "IN" : ""), (intent & INTENT_OUT ? "OUT" : ""),
            (intent & INTENT_LOCAL ? "LOCAL" : ""));
    custom_log(TRACE, blockOut, rank, indexes);
    DvmhPieces *p = rdata->getLocalPieces()->intersect(rdata->getOutPieces());
    checkError2(p->isEmpty(), "Conflicting OUT and LOCAL clauses detected.");
    delete p;
    if (data->isDistributed())
        registerDspace(data->getAlignRule()->getDspace());
}

void DvmhRegion::registerDspace(DvmhDistribSpace *dspace) {
    checkInternal(phase == rpRegistrations);
    DvmhRegionDistribSpace *rdspace = dictFind2(dspaces, dspace);
    if (!rdspace) {
        rdspace = new DvmhRegionDistribSpace(dspace);
        dspaces[dspace] = rdspace;
    }
}

void DvmhRegion::setDataName(DvmhData *data, const char *name, int nameLength) {
    checkInternal(phase == rpRegistrations);
    assert(data);
    DvmhRegionData *rdata = dictFind2(datas, data);
    assert(rdata);
    rdata->setName(name, nameLength);
    rdata->setVarId(persistentInfo->getVarId(rdata->getName()));
    dvmh_log(TRACE, "registered variable name %s", rdata->getName());
}

const char *DvmhRegion::getDataName(DvmhData *data) const {
    assert(data);
    DvmhRegionData *rdata = dictFind2(datas, data);
    if (rdata)
        return rdata->getName();
    else
        return 0;
}

void DvmhRegion::executeOnTargets(unsigned long deviceTypes) {
    checkInternal(phase == rpRegistrations);
    phase = rpExecution;
    canExecuteOnDeviceTypes = deviceTypes | 1ul;
    unsigned long devicesMask = 0;
    for (int i = 0; i < devicesCount; i++) {
        if (devices[i]->hasSlots())
            devicesMask |= (unsigned long)(((1ul << devices[i]->getType()) & deviceTypes) != 0) << i;
    }
    if (devicesMask == 0)
        devicesMask = 1;

    if (compareDebug)
        devicesMask |= 1ul;
    if (compareDebug && ((devicesMask & 1) == 0 || devicesMask == 1)) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Can't do comparative debugging for the region. Possibility to run on both HOST and non-HOST devices is needed.");
        dvmhLogger.endMasterRegion();
        compareDebug = false;
    }
    assert(!compareDebug || ((devicesMask & 1) && devicesMask > 1));

    if (dvmhSettings.schedTech == stDynamic) {
        for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++) {
            if (it->second->hasInPieces()) {
                checkInternal2(it->second->getVarId() >= 0, "Unknown name of variable");
                persistentInfo->addDataDep(it->second);
            }
        }
    }

    // Choose mapping
    DvmhRegionMapping mapping(this, devicesMask);

    // map onto devices
    usesDevices = mapping.usesDevices;
    dvmh_log(TRACE, "devices chosen for mapping = %lu", usesDevices);
    for (std::map<DvmhDistribSpace *, DvmhRegionDistribSpace *>::iterator it = dspaces.begin(); it != dspaces.end(); it++)
        mapSpaceOnDevices(&mapping, it->second);
    for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++)
        mapDataOnDevices(it->second);
    allocateDatasOnDevices();
    for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++) {
        DvmhRegionData *rdata = it->second;
        DvmhData *data = rdata->getData();
        if (compareDebug)
            compareDatas(rdata, rdata->getInPieces());
        performRenew(rdata, false);
        for (int j = 0; j < devicesCount; j++) {
            if ((usesDevices & (1ul << j)) && (rdata->getLocalPart(j) != 0)) {
                dvmh_log(TRACE, "Actual state on device %d:", j);
                custom_log(TRACE, piecesOut, data->getRepr(j)->getActualState());
            }
        }
    }
}

DvmhData *DvmhRegion::getDataByVarId(int varId) const {
    // XXX: Could be slow
    for (std::map<DvmhData *, DvmhRegionData *>::const_iterator it = datas.begin(); it != datas.end(); it++) {
        if (it->second->getVarId() == varId)
            return it->first;
    }
    return 0;
}

void DvmhRegion::fillLocalPart(int dev, DvmhData *data, DvmType part[]) const {
    checkInternal(phase == rpExecution);
    assert(data);
    int rank = data->getRank();
    if (rank == 0)
        return;
    DvmhRegionData *rdata = dictFind2(datas, data);
    Interval *localPart = 0;
    if (rdata)
        localPart = rdata->getLocalPart(dev);
    if (!rdata || !localPart) {
        // Empty local part
        for (int i = 0; i < rank; i++) {
            part[2 * i + 0] = 1;
            part[2 * i + 1] = 0;
        }
    } else {
        for (int i = 0; i < rank; i++) {
            part[2 * i + 0] = localPart[i][0];
            part[2 * i + 1] = localPart[i][1];
        }
    }
}

bool DvmhRegion::hasElement(int dev, DvmhData *data, const DvmType indexArray[]) const {
    checkInternal(phase == rpExecution);
    if (usesDevices & (1ul << dev)) {
        assert(data);
        DvmhRegionData *rdata = dictFind2(datas, data);
        if (!rdata)
            return false;
        if (data->getRank() == 0)
            return true;
        Interval *localPart = rdata->getLocalPart(dev);
        if (!localPart)
            return false;
        if (!localPart->blockContains(data->getRank(), indexArray))
            return false;
        return true;
    } else {
        return false;
    }
}

void DvmhRegion::addRemoteGroup(DvmhData *data) {
    checkInternal(phase == rpExecution);
    if (data->hasLocal()) {
        PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRemote);
        DvmhPieces *pieces = new DvmhPieces(data->getRank());
        pieces->appendOne(data->getSpace());
        for (int i = 0; i < devicesCount; i++) {
            if (usesDevices & (1ul << i)) {
                DvmhRepresentative *repr = data->getRepr(i);
                if (!repr) {
                    DvmhBuffer *hbuff = data->getBuffer(0);
                    assert(hbuff);
                    repr = data->createNewRepr(i, hbuff->getHavePortion());
                }
                data->getActualBase(i, pieces, 0, 1);
            }
        }
        delete pieces;
    }
}

void DvmhRegion::markToRenew(DvmhData *data) {
    checkInternal(phase == rpExecution);
    assert(data);
    DvmhRegionData *rdata = dictFind2(datas, data);
    if (rdata)
        rdata->setRenewFlag();
}

bool DvmhRegion::canAddToActual(DvmhData *data, const DvmhPieces *indexes) const {
    checkInternal(phase == rpExecution);
    assert(data);
    int rank = data->getRank();
    DvmhRegionData *rdata = dictFind2(datas, data);
    if (!rdata)
        return true;
    DvmhPieces *p1 = indexes->subtract(data->getRepr(0)->getActualState());
    DvmhPieces *p2 = new DvmhPieces(rank);
    p2->append(rdata->getOutPieces());
    p2->append(rdata->getLocalPieces());
    p1->intersectInplace(p2);
    delete p2;
    bool res = p1->isEmpty();
    delete p1;
    return res;
}

bool DvmhRegion::canAddToActual(DvmhData *data, const Interval indexes[]) const {
    checkInternal(phase == rpExecution);
    assert(data);
    int rank = data->getRank();
    DvmhPieces *p1 = new DvmhPieces(rank);
    p1->appendOne(indexes);
    bool res = canAddToActual(data, p1);
    delete p1;
    return res;
}

void DvmhRegion::setLatestLoopEnd(DvmhEvent *event, bool owning) {
    delete latestLoopEnd;
    if (owning)
        latestLoopEnd = event;
    else
        latestLoopEnd = event->dup();
}

static void convertIndexes(DvmhData *data, const DvmType localIndex[], DvmType globalIndex[]) {
    int rank = data->getRank();
    for (int i = 0; i < rank; i++) {
        globalIndex[i] = localIndex[i];
    }
    bool res = data->convertLocal2ToGlobal(globalIndex);
    checkInternal2(res, "Can not convert local indexes to global");
}

static char *printIndex(char *cur, const char *prefix, DvmType localIndex, DvmType globalIndex, const char *suffix) {
    if (localIndex != globalIndex) {
        return cur + sprintf(cur, "%s" DTFMT " G " DTFMT "%s", prefix, localIndex, globalIndex, suffix);
    } else {
        return cur + sprintf(cur, "%s" DTFMT "%s", prefix, localIndex, suffix);
    }
}

static void fillAtStr(char atStr[], int rank, const DvmType currentIndex[], const DvmType globalIndex[]) {
    atStr[0] = 0;
    if (rank > 0) {
        char *cur = atStr;
        if (dvmhSettings.useFortranNotation) {
            *cur++ = '(';
            for (int i = rank - 1; i >= 1; i--)
                cur = printIndex(cur, "", currentIndex[i], globalIndex[i], ", ");
            cur = printIndex(cur, "", currentIndex[0], globalIndex[0], ")");
        } else {
            for (int i = 0; i < rank; i++)
                cur = printIndex(cur, "[", currentIndex[i], globalIndex[i], "]");
        }
        *cur = 0;
    }
}

#define DROP_PARENS(...) __VA_ARGS__

#define SEEN_ERROR(fmt, v1, v2) do { \
    errorCount++; \
    if (isIndirect && (outMessages || globalErrSet)) \
        convertIndexes(data, currentIndex + 1, globalIndex); \
    if (errSet) \
        errSet->uniteOne(currentIndex + 1); \
    if (isIndirect && globalErrSet) \
        globalErrSet->uniteOne(globalIndex); \
    if (outMessages) { \
        fillAtStr(atStr, rank, currentIndex + 1, isIndirect ? globalIndex : currentIndex + 1); \
        dvmh_log(TRACE, "Detected error in variable %s%s. Host's value = " fmt ", Device #%d's value = " fmt, varName, atStr, DROP_PARENS v1, dev, \
                DROP_PARENS v2); \
    } \
} while (0)

#define COMPARE_INTS(T, fmt) if (typeSize == sizeof(T)) { \
    const T &v1 = *(T *)el1; \
    const T &v2 = *(T *)el2; \
    if (!(v1 == v2)) \
        SEEN_ERROR("%" fmt, (v1), (v2)); \
}

#define COMPARE_FLOATS(T, eps, prec, fmt) if (typeSize == sizeof(T)) { \
    const T &v1 = *(T *)el1; \
    const T &v2 = *(T *)el2; \
    if (!(eps > 0 ? std::abs(v1 - v2) < eps || std::abs(v1 - v2) / (std::max(eps * eps, (T)std::abs(v1))) < eps : v1 == v2)) \
        SEEN_ERROR("%.*" fmt, (prec, v1), (prec, v2)); \
}

#define COMPARE_COMPLEXS(T, eps, prec, fmt) if (typeSize == sizeof(T) * 2) { \
    bool ok = true; \
    const T &v1re = *(T *)el1; \
    const T &v2re = *(T *)el2; \
    const T &v1im = *((T *)el1 + 1); \
    const T &v2im = *((T *)el2 + 1); \
    if (eps > 0) { \
        T diff = sqrt((v1re - v2re) * (v1re - v2re) + (v1im - v2im) * (v1im - v2im)); \
        T len = sqrt(v1re * v1re + v1im * v1im); \
        if (!(diff < eps || diff / std::max(eps * eps, len) < eps)) \
            ok = false; \
    } else { \
        if (!(v1re == v2re && v1im == v2im)) \
            ok = false; \
    } \
    if (!ok) \
        SEEN_ERROR("(%.*" fmt ", %.*" fmt ")", (prec, v1re, prec, v1im), (prec, v2re, prec, v2im)); \
}

static UDvmType comparePiece(DvmhRegionData *rdata, int dev, DvmType header1[], DvmType header2[], const Interval piece[], DvmhPieces *errSet = 0,
        DvmhPieces *globalErrSet = 0)
{
    DvmhData *data = rdata->getData();
    int rank = data->getRank();
#ifdef NON_CONST_AUTOS
    DvmType currentIndex[rank + 1], globalIndex[rank];
    UDvmType partialSize[rank + 1];
#else
    DvmType currentIndex[MAX_ARRAY_RANK + 1], globalIndex[MAX_ARRAY_RANK];
    UDvmType partialSize[MAX_ARRAY_RANK + 1];
#endif
    int maxStepRank = 0;
    for (int i = rank - 1; i >= 0; i--) {
        if (header1[1 + i] != header2[1 + i])
            break;
        if (i < rank - 1 && piece[i + 1].size() != (UDvmType)header1[1 + i] / (UDvmType)header1[1 + i + 1])
            break;
        maxStepRank++;
    }
    UDvmType errorCount = 0;
    currentIndex[0] = 0;
    partialSize[0] = 1;
    for (int i = 0; i < rank; i++) {
        currentIndex[1 + i] = piece[i][0];
        partialSize[1 + i] = partialSize[1 + i - 1] * piece[rank - 1 - i].size();
    }
    int currentIndexAlignment = rank;
    const char *varName = rdata->getName();
    char atStr[100];
    atStr[0] = 0;
    bool outMessages = dvmhSettings.logLevel >= TRACE;
    bool isIndirect = data->isDistributed() && data->getAlignRule()->hasIndirect();
    float floatEps = dvmhSettings.compareFloatsEps;
    double doubleEps = dvmhSettings.compareDoublesEps;
    long double longDoubleEps = dvmhSettings.compareLongDoublesEps;
    int floatPrec = floatEps > 0 ? std::max(5, std::min(8, int(0.5 - std::log10(floatEps)) + 1)) : 8;
    int doublePrec = doubleEps > 0 ? std::max(5, std::min(16, int(0.5 - std::log10(doubleEps)) + 1)) : 16;
    int longDoublePrec = longDoubleEps > 0 ? std::max(5, std::min(34, int(0.5 - std::log10(longDoubleEps)) + 1)) : 34;
    DvmType typeSize = data->getTypeSize();
    char *el1 = (char *)header1[rank + 2] + typeSize * header1[rank + 1];
    char *el2 = (char *)header2[rank + 2] + typeSize * header2[rank + 1];
    for (int i = 0; i < rank; i++) {
        el1 += typeSize * header1[1 + i] * currentIndex[1 + i];
        el2 += typeSize * header2[1 + i] * currentIndex[1 + i];
    }
    while (currentIndex[0] == 0) {
        int curCompRank = std::min(currentIndexAlignment + 1, maxStepRank);
        while (curCompRank > 0 &&
                memcmp(el1, el2, typeSize * partialSize[curCompRank - 1] * (piece[rank - curCompRank][1] - currentIndex[1 + rank - curCompRank] + 1)) != 0)
            curCompRank--;
        if (curCompRank == 0) {
            switch (data->getTypeType()) {
                case DvmhData::ttInteger:
                    COMPARE_INTS(char, "d")
                    else COMPARE_INTS(short, "d")
                    else COMPARE_INTS(int, "d")
                    else COMPARE_INTS(long, "ld")
                    else COMPARE_INTS(long long, "lld")
                    break;
                case DvmhData::ttFloating:
                    COMPARE_FLOATS(float, floatEps, floatPrec, "e")
                    else COMPARE_FLOATS(double, doubleEps, doublePrec, "e")
                    else COMPARE_FLOATS(long double, longDoubleEps, longDoublePrec, "Le")
                    break;
                case DvmhData::ttComplex:
                    COMPARE_COMPLEXS(float, floatEps, floatPrec, "e")
                    else COMPARE_COMPLEXS(double, doubleEps, doublePrec, "e")
                    else COMPARE_COMPLEXS(long double, longDoubleEps, longDoublePrec, "Le")
                    break;
                default:
                    assert(false);
            }
        }
        if (curCompRank > 0) {
            int i = rank - curCompRank;
            el1 -= typeSize * header1[1 + i] * (currentIndex[1 + i] - piece[i][0]);
            el2 -= typeSize * header2[1 + i] * (currentIndex[1 + i] - piece[i][0]);
            currentIndex[1 + i] = piece[i][0];
        }
        int i = rank - curCompRank;
        do {
            i--;
            currentIndex[1 + i]++;
            if (i >= 0) {
                el1 += typeSize * header1[1 + i];
                el2 += typeSize * header2[1 + i];
                if (currentIndex[1 + i] > piece[i][1]) {
                    el1 -= typeSize * header1[1 + i] * (currentIndex[1 + i] - piece[i][0]);
                    el2 -= typeSize * header2[1 + i] * (currentIndex[1 + i] - piece[i][0]);
                    currentIndex[1 + i] = piece[i][0];
                }
            }
        } while (i >= 0 && currentIndex[1 + i] == piece[i][0]);
        currentIndexAlignment = rank - i - 1;
    }
    return errorCount;
}
#undef DROP_PARENS
#undef SEEN_ERROR
#undef COMPARE_INTS
#undef COMPARE_FLOATS
#undef COMPARE_COMPLEXS

void DvmhRegion::compareDatas(DvmhRegionData *rdata, DvmhPieces *area) {
    checkInternal(phase == rpExecution);
    assert(rdata != 0);
    DvmhData *data = rdata->getData();
    assert(data != 0);
    int rank = data->getRank();
    DvmhData::TypeType typeType = data->getTypeType();
    UDvmType typeSize = data->getTypeSize();
    const char *varName = rdata->getName();
    dvmh_log(DEBUG, "Trying to compare variable %s with rank=%d", varName, rank);
    UDvmType errorCount = 0;
    if (data->getDataType() != DvmhData::dtUnknown) {
        DvmhBuffer *buff = data->getBuffer(0);
        DvmhPieces *errSet = new DvmhPieces(rank);
        DvmhPieces *globalErrSet = new DvmhPieces(rank);
#ifdef NON_CONST_AUTOS
        DvmType header1[rank + 3], header2[rank + 3];
#else
        DvmType header1[MAX_ARRAY_RANK + 3], header2[MAX_ARRAY_RANK + 3];
#endif
        buff->fillHeader(header1, false);
        for (int i = 1; i < devicesCount; i++) {
            if (usesDevice(i) && data->getRepr(i)) {
                DvmhPieces *p2 = new DvmhPieces(rank);
                p2->append(data->getRepr(i)->getActualState());
                if (area)
                    p2->intersectInplace(area);
                dvmh_log(TRACE, "Variable %s what to compare:", varName);
                custom_log(TRACE, piecesOut, p2);
                for (int j = 0; j < p2->getCount(); j++) {
                    DvmType order = ABS_ORDER;
                    const Interval *piece = p2->getPiece(j, &order);
                    if (order == ABS_ORDER) {
                        DvmhBuffer *buf = data->getBuffer(i)->dumpPiece(piece);
                        buf->fillHeader(header2, false);
                        UDvmType pieceErrorCount = comparePiece(rdata, i, header1, header2, piece, errSet, globalErrSet);
                        if (pieceErrorCount > 0) {
                            data->getRepr(i)->getActualState()->subtractOne(piece);
                            dvmh_log(DEBUG, "Discarding actual state from following piece on device #%d:", i);
                            custom_log(DEBUG, blockOut, rank, piece);
                        }
                        errorCount += pieceErrorCount;
                        delete buf;
                    }
                }
                delete p2;
            }
        }
        if (errorCount > 0) {
            dvmh_log(DEBUG, "Found " UDTFMT " errors", errorCount);
            DvmhPieces *setToReport = globalErrSet->isEmpty() ? errSet : globalErrSet;
            setToReport->compactify();
            dvmhLogger.startBlock(NFERROR);
            if (setToReport->getCount() <= 8) {
                dvmh_log(NFERROR, "Detected " UDTFMT " errors in variable %s. Following set of indexes differs:", errorCount, varName);
                custom_log(NFERROR, piecesOut, setToReport);
            } else {
                dvmh_log(NFERROR, "Detected " UDTFMT " errors in variable %s. They are localized inside the block:", errorCount, varName);
                custom_log(NFERROR, blockOut, rank, ~setToReport->getBoundRect().rect);
            }
            dvmhLogger.endBlock(NFERROR, __FILE__, __LINE__);
        }
        delete errSet;
        delete globalErrSet;
        dvmh_log(DEBUG, "Variable %s successfully compared", varName);
    } else {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Variable %s can not be compared due to unsupported value type (type=%s, size=" UDTFMT ")", varName,
                (typeType == DvmhData::ttInteger ? "integer" : (typeType == DvmhData::ttFloating ? "floating" :
                (typeType == DvmhData::ttComplex ? "complex" : "unknown"))), typeSize);
        dvmhLogger.endMasterRegion();
    }
}

void DvmhRegion::finish() {
    checkInternal(phase == rpExecution);
    if (compareDebug) {
        for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++)
            compareDatas(it->second, it->second->getOutPieces());
    }
    persistentInfo->incrementExecCount();
    phase = rpFinished;
}

DvmhRegion::~DvmhRegion() {
    checkInternal(phase == rpFinished);
    for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++)
        delete it->second;
    datas.clear();
    for (std::map<DvmhDistribSpace *, DvmhRegionDistribSpace *>::iterator it = dspaces.begin(); it != dspaces.end(); it++) {
        delete it->second;
        DvmhDistribSpace *dspace = it->first;
        if (dspace && dspace->getRefCount() == 0)
            delete dspace;
    }
    dspaces.clear();
    delete startPrereq;
    delete latestLoopEnd;
}

void DvmhRegion::performRenew(DvmhRegionData *rdata, bool updateOut, DvmhPieces *restrictTo) {
    checkInternal(phase == rpExecution);
    unsigned long devicesMask = usesDevices;
    DvmhData *data = rdata->getData();
    int rank = data->getRank();
    DvmhPieces *outPlusLocal = new DvmhPieces(rank);
    outPlusLocal->append(rdata->getOutPieces());
    outPlusLocal->append(rdata->getLocalPieces());
    // Getting actual for 'in' parameters and, in case if specified, 'out' and 'local'
#ifdef NON_CONST_AUTOS
    DvmhPieces *needToBeActual[devicesCount];
#else
    DvmhPieces *needToBeActual[MAX_DEVICES_COUNT];
#endif
    for (int i = 0; i < devicesCount; i++)
        needToBeActual[i] = 0;
    if (rdata->hasInPieces() || (updateOut && outPlusLocal->getCount() > 0)) {
        for (int j = 0; j < devicesCount; j++) {
            if ((devicesMask & (1ul << j)) && (rdata->getLocalPart(j) || rank == 0)) {
#ifdef NON_CONST_AUTOS
                Interval localPlusShadow[rank];
#else
                Interval localPlusShadow[MAX_ARRAY_RANK];
#endif
                data->extendBlock(rdata->getLocalPart(j), localPlusShadow);
                DvmhPieces *p1 = new DvmhPieces(rank);
                p1->appendOne(localPlusShadow);
                p1->intersectInplace(rdata->getInPieces());
                if (updateOut && outPlusLocal->getCount() > 0) {
                    DvmhPieces *p2 = new DvmhPieces(rank);
                    p2->appendOne(localPlusShadow);
                    p2->intersectInplace(outPlusLocal);
                    p1->unite(p2);
                    delete p2;
                }
                if (restrictTo) {
                    p1->intersectInplace(restrictTo);
                }
                needToBeActual[j] = p1;
                bool isShadow = false;
                if (rank > 0) {
                    DvmhPieces *p2 = new DvmhPieces(rank);
                    p2->appendOne(rdata->getLocalPart(j));
                    p2->intersectInplace(p1);
                    p2->subtractInplace(data->getRepr(j)->getActualState());
                    isShadow = p2->isEmpty();
                    delete p2;
                }
                PushCurrentPurpose purpose((isShadow ? DvmhCopyingPurpose::dcpShadow : DvmhCopyingPurpose::dcpInRegion), true);
                data->getActualBase(j, p1, rdata->getLocalPart(j), 1);
                data->getCurState().addReader(DvmhRegVar(persistentInfo, rdata->getVarId()));
            }
        }
    }

    for (int i = 0; i < devicesCount; i++) {
        if (devicesMask & (1ul << i)) {
            DvmhRepresentative *repr = data->getRepr(i);
            if (repr) {
                repr->setCleanTransformState(false);
                if (!restrictTo && (needToBeActual[i] == 0 || needToBeActual[i]->isEmpty())) {
                    // There is nothing that needs to be actual
                    repr->getActualState()->subtractInplace(outPlusLocal);
                    if (repr->getActualState()->isEmpty()) {
                        // There is nothing actual that will not be overwritten
                        repr->setCleanTransformState(true);
                        const char *name = rdata->getName();
                        dvmh_log(DEBUG, "Setting clean transform state for variable %s on device %d", (name ? name : "<no name>"), i);
                    }
                }
            }
        }
        delete needToBeActual[i];
        needToBeActual[i] = 0;
    }

    DvmhPieces *restrictedOutPlusLocal = outPlusLocal;
    if (restrictTo) {
        restrictedOutPlusLocal = outPlusLocal->intersect(restrictTo);
    }
    if (restrictedOutPlusLocal->getCount() > 0) {
        // Clearing actual for 'out' and 'local' parameters
        data->clearActual(restrictedOutPlusLocal);

        // Marking as actual for 'out' and 'local' parameters
        for (int j = 0; j < devicesCount; j++) {
            if ((devicesMask & (1ul << j)) && (rdata->getLocalPart(j) || rank == 0)) {
                DvmhRepresentative *repr = data->getRepr(j);
                assert(repr != 0);
                DvmhPieces *p1 = new DvmhPieces(rank);
                p1->appendOne(rdata->getLocalPart(j));
                p1->intersectInplace(restrictedOutPlusLocal);
                repr->getActualState()->unite(p1);
                delete p1;
            }
        }
        data->getCurState() = DvmhDataState(persistentInfo, rdata->getVarId());
    }
    if (restrictTo) {
        delete restrictedOutPlusLocal;
    }
    delete outPlusLocal;
}

void DvmhRegion::mapSpaceOnDevices(DvmhRegionMapping *mapping, DvmhRegionDistribSpace *rdspace) {
    checkInternal(phase == rpExecution);
    assert(rdspace != 0);
    DvmhDistribSpace *dspace = rdspace->getDistribSpace();
    assert(dspace != 0);
    unsigned long devicesMask = usesDevices;
#ifdef NON_CONST_AUTOS
    double deviceWeights[devicesCount];
    double distribPoints[devicesCount + 1];
#else
    double deviceWeights[MAX_DEVICES_COUNT];
    double distribPoints[MAX_DEVICES_COUNT + 1];
#endif
    for (int i = 0; i < devicesCount; i++)
        deviceWeights[i] = mapping->rdspaces[dspace][i];

    double totalWeight = 0;
    if (compareDebug)
        devicesMask -= 1;
    for (int i = 0; i < devicesCount; i++) {
        if (devicesMask & (1ul << i))
            totalWeight += deviceWeights[i];
        else
            deviceWeights[i] = 0;
    }
    if (totalWeight <= 0) {
        for (int i = 0; i < devicesCount; i++) {
            if (devicesMask & (1ul << i)) {
                deviceWeights[i] = 1.0;
                totalWeight += deviceWeights[i];
            }
        }
    }
    for (int i = 0; i < devicesCount; i++)
        deviceWeights[i] /= totalWeight;
    int loadedDevicesCount = 0;
    distribPoints[0] = 0;
    for (int i = 0; i < devicesCount; i++) {
        distribPoints[i + 1] = distribPoints[i] + deviceWeights[i];
        if (deviceWeights[i] > 0)
            loadedDevicesCount++;
    }
    distribPoints[devicesCount] = 1;
    {
        char buf[4096];
        memset(buf, 0, sizeof(buf));
        char *s = buf;
        s += sprintf(s, "DistribPoints:");
        for (int i = 0; i < devicesCount + 1; i++)
            s += sprintf(s, " %g", distribPoints[i]);
        dvmh_log(TRACE, "%s", buf);
    }

    dvmh_log(TRACE, "Local DistribSpace:");
    custom_log(TRACE, blockOut, dspace->getRank(), dspace->getLocalPart());
    int firstDistr = 0;
    while (firstDistr < dspace->getRank() && dspace->getAxisDistribRule(firstDistr + 1)->getMaxSubparts() == 0)
        firstDistr++;
    if (firstDistr >= dspace->getRank()) {
        // No axes allowed to block distribution
        if (dspace->hasLocal()) {
            for (int i = 0; i < devicesCount; i++) {
                if ((devicesMask & (1ul << i)) && deviceWeights[i] > 0) {
                    assert(rdspace->getLocalPart(i) == 0);
                    rdspace->addLocalPart(i, 1.0)->blockAssign(dspace->getRank(), dspace->getLocalPart());
                    dvmh_log(TRACE, "Due to nothing to can be distributed, whole local distribSpace is on device %d", i);
                    // TODO: Do something with reduction
                }
            }
        } else {
            dvmh_log(TRACE, "Empty local part of distribSpace");
        }
    } else {
#ifdef NON_CONST_AUTOS
        Interval devParts[devicesCount];
#else
        Interval devParts[MAX_DEVICES_COUNT];
#endif
        int fd = firstDistr;
        while (fd < dspace->getRank() && dspace->getAxisDistribRule(fd + 1)->getMaxSubparts() < loadedDevicesCount)
            fd++;
        if (fd >= dspace->getRank())
            fd = firstDistr;
        dvmh_log(TRACE, "Distributing among %d-th (1-based) Axis", fd + 1);
        dspace->getAxisDistribRule(fd + 1)->genSubparts(distribPoints, devParts, devicesCount);
        for (int i = 0; i < devicesCount; i++) {
            if ((devicesMask & (1ul << i)) && deviceWeights[i] > 0) {
                assert(rdspace->getLocalPart(i) == 0);
                if (!devParts[i].empty()) {
                    rdspace->addLocalPart(i, deviceWeights[i])->blockAssign(dspace->getRank(), dspace->getLocalPart());
                    rdspace->getLocalPart(i)[fd] = devParts[i];
                    dvmh_log(TRACE, "distribSpace part on device %d: [" DTFMT ".." DTFMT "]", i, devParts[i][0], devParts[i][1]);
                }
            } else {
                assert(devParts[i].empty());
            }
        }
    }
    if (compareDebug) {
        rdspace->addLocalPart(0, 1.0)->blockAssign(dspace->getRank(), dspace->getLocalPart());
        dvmh_log(TRACE, "Whole distribSpace part on HOST for compare debug");
    }
}

void DvmhRegion::mapDataOnDevices(DvmhRegionData *rdata) {
    checkInternal(phase == rpExecution);
    assert(rdata != 0);
    DvmhData *data = rdata->getData();
    assert(data != 0);
    int rank = data->getRank();
    unsigned long devicesMask = usesDevices;
    if (rdata->getName()) {
        dvmh_log(TRACE, "Mapping %s on devices", rdata->getName());
    }
    for (int j = 0; j < devicesCount; j++) {
        if (devicesMask & (1ul << j)) {
            assert(rdata->getLocalPart(j) == 0);
            bool hasLocal = true;
            if (rank > 0) {
#ifdef NON_CONST_AUTOS
                Interval part[rank];
#else
                Interval part[MAX_ARRAY_RANK];
#endif
                part->blockAssign(rank, data->getLocalPart());
                if (data->isDistributed()) {
                    const DvmhAlignRule *rule = data->getAlignRule();
                    DvmhDistribSpace *dspace = rule->getDspace();
                    assert(dspace != 0);
                    DvmhRegionDistribSpace *rdspace = dictFind2(dspaces, dspace);
                    assert(rdspace != 0);
                    hasLocal = hasLocal && rdspace->getLocalPart(j) != 0 && data->hasLocal();
                    if (hasLocal) {
                        Interval *localPart = rdspace->getLocalPart(j);
                        dvmh_log(TRACE, "rdspace local part (device %d):", j);
                        custom_log(TRACE, blockOut, dspace->getRank(), localPart);
                        hasLocal = rule->mapOnPart(localPart, part, true);
                    }
                }
                if (hasLocal) {
                    DvmhPieces *p = new DvmhPieces(rank);
                    p->unite(rdata->getInPieces());
                    p->unite(rdata->getLocalPieces());
                    p->unite(rdata->getOutPieces());
                    DvmhPieces *p2 = new DvmhPieces(rank);
                    p2->appendOne(part);
                    p->intersectInplace(p2);
                    delete p2;
                    if (!p->isEmpty())
                        part->blockAssign(rank, ~p->getBoundRect().rect);
                    else
                        hasLocal = false;
                    delete p;
                }
                if (hasLocal)
                    rdata->addLocalPart(j)->blockAssign(rank, part);
            }
            if (hasLocal && rank > 0) {
                dvmh_log(TRACE, "array localPart on device %d:", j);
                custom_log(TRACE, blockOut, rank, rdata->getLocalPart(j));
            }
        }
    }
    dvmh_log(TRACE, "Data %s is mapped", rdata->getName());
}

static void extendAndRoundPortion(DvmhData *data, const Interval localPart[], Interval portion[]) {
    int rank = data->getRank();
    data->extendBlock(localPart, portion);
    // Rounding to host representative
    DvmhBuffer *hbuff = data->getBuffer(0);
    assert(hbuff);
    for (int i = rank - 1; i >= 1; i--) {
        if (portion[i].extend(4).contains(hbuff->getHavePortion()[i]))
            portion[i] = hbuff->getHavePortion()[i];
        else
            break;
    }
    // Rounding to square
    double sizeCoef = 1.0;
    for (int i = rank - 1; i >= 0; i--) {
        UDvmType origSize = portion[i].size();
        UDvmType maxAllowed = origSize;
        for (int j = 0; j < rank; j++) {
            UDvmType curSize = portion[j].size();
            if (curSize > maxAllowed && (double)curSize / origSize * sizeCoef <= 1.1)
                maxAllowed = curSize;
        }
        if (maxAllowed > origSize) {
            portion[i][1] += maxAllowed - origSize;
            sizeCoef *= (double)maxAllowed / origSize;
        }
    }
}

void DvmhRegion::allocateDatasOnDevices() {
    checkInternal(phase == rpExecution);
    unsigned long devicesMask = usesDevices;
    std::set<DvmhData *> regionDatas;
    for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++)
        regionDatas.insert(it->first);
    for (int j = 1; j < devicesCount; j++) {
        // Check sizes, collect information, delete not equitant representatives
        UDvmType memNeeded = 0;
        std::priority_queue<std::pair<UDvmType, DvmhData *> > datasWithSurplus;
        for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++) {
            DvmhRegionData *rdata = it->second;
            assert(rdata != 0);
            DvmhData *data = rdata->getData();
            assert(data != 0);
            int rank = data->getRank();
            if ((devicesMask & (1ul << j)) && (rdata->getLocalPart(j) || rank == 0)) {
#ifdef NON_CONST_AUTOS
                Interval portion[rank];
#else
                Interval portion[MAX_ARRAY_RANK];
#endif
                extendAndRoundPortion(data, rdata->getLocalPart(j), portion);
                UDvmType curMemNeeded = data->getTypeSize() * portion->blockSize(rank);
                DvmhRepresentative *repr = data->getRepr(j);
                if (!repr || !repr->getBuffer()->getHavePortion()->blockContains(rank, portion)) {
                    if (repr) {
// TODO: Move data inside device, not through host memory
                        data->getActualBase(0, repr->getActualState(), 0, true);
                        data->deleteRepr(j);
                        repr = data->getRepr(j);
                        assert(repr == 0);
                    }
                    memNeeded += curMemNeeded;
                } else {
                    UDvmType curSize = repr->getBuffer()->getSize();
                    if (curSize > curMemNeeded)
                        datasWithSurplus.push(std::make_pair(curSize - curMemNeeded, data));
                }
            }
        }
        // Try to free enough space
        dvmhTryToFreeSpace(memNeeded, j, regionDatas);
        while (memNeeded > devices[j]->memLeft() && !datasWithSurplus.empty()) {
            UDvmType surplus = datasWithSurplus.top().first;
            DvmhData *data = datasWithSurplus.top().second;
            datasWithSurplus.pop();
            assert(data->getBuffer(j)->getSize() >= surplus);
            memNeeded += data->getBuffer(j)->getSize() - surplus;
            data->getActualBase(0, data->getRepr(j)->getActualState(), 0, 1);
            data->deleteRepr(j);
        }
        checkError3(memNeeded <= devices[j]->memLeft(), "Not enough memory on device #%d for allocation of all needed region variables", j);
        // Allocate
        for (std::map<DvmhData *, DvmhRegionData *>::iterator it = datas.begin(); it != datas.end(); it++) {
            DvmhRegionData *rdata = it->second;
            assert(rdata != 0);
            DvmhData *data = rdata->getData();
            assert(data != 0);
            int rank = data->getRank();
            if ((devicesMask & (1ul << j)) && (rdata->getLocalPart(j) || rank == 0) && !data->getRepr(j)) {
#ifdef NON_CONST_AUTOS
                Interval portion[rank];
#else
                Interval portion[MAX_ARRAY_RANK];
#endif
                extendAndRoundPortion(data, rdata->getLocalPart(j), portion);
                data->createNewRepr(j, portion);
            }
        }
    }
}

std::map<SourcePosition, DvmhRegionPersistentInfo *> regionDict; //SourcePosition => DvmhRegionPersistentInfo *
std::vector<double> lastBestWeights; // length = devicesCount

}
