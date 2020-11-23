#pragma once

#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "include/dvmhlib_const.h"

#include "dvmh_data.h"
#include "dvmh_device.h"

namespace libdvmh {

class DvmhPieces;
class DvmhDistribSpace;
class DvmhLoopPersistentInfo;

class DvmhRegionDistribSpace {
public:
    Interval *getLocalPart(int dev) const { return localParts[dev]; }
    DvmhDistribSpace *getDistribSpace() const { return dspace; }
    double getWeight(int dev) const { return weights[dev]; }
public:
    explicit DvmhRegionDistribSpace(DvmhDistribSpace *aDspace);
public:
    Interval *addLocalPart(int dev, double wgt);
public:
    ~DvmhRegionDistribSpace();
protected:
    DvmhDistribSpace *dspace;
    double *weights; //Array of weights (length=devicesCount)
    Interval **localParts; //Array of array of Intervals (length1=devicesCount, length2=dspace->rank)
};

class DvmhRegionData {
public:
    DvmhData *getData() const { return data; }
    bool getRenewFlag() const { return renewFlag; }
    void setRenewFlag(bool value = true) { renewFlag = value; }
    DvmhPieces *getInPieces() const { return inPieces; }
    DvmhPieces *getOutPieces() const { return outPieces; }
    DvmhPieces *getLocalPieces() const { return localPieces; }
    Interval *getLocalPart(int dev) const { return localParts[dev]; }
    const char *getName() const { return varName; }
    int getVarId() const { return varId; }
    void setVarId(int v) { varId = v; }
public:
    explicit DvmhRegionData(DvmhData *aData);
public:
    bool hasInPieces() const;
    bool hasOutPieces() const;
    bool hasLocalPieces() const;
    Interval *addLocalPart(int dev);
    void setName(const char *name, int nameLength);
public:
    ~DvmhRegionData();
protected:
    bool renewFlag;
    DvmhData *data;
    DvmhPieces *inPieces;
    DvmhPieces *outPieces;
    DvmhPieces *localPieces;
    char *varName;
    int varId;
    Interval **localParts; //Array of array of Intervals (length1=devicesCount, length2=data->rank)
};

class DvmhRegionPersistentInfo;

class DvmhDataDep {
public:
    DvmType getCount() const { return selfCount + otherCount; }
public:
    explicit DvmhDataDep(const DvmhRegVar &v): myVar(v), selfCount(0), otherCount(0) {}
    explicit DvmhDataDep(DvmhRegionPersistentInfo *region, int varId): myVar(region, varId), selfCount(0), otherCount(0) {}
public:
    void addState(const DvmhDataState &state);
protected:
    DvmhRegVar myVar;
    DvmType selfCount; //count of self-establishing
    DvmType otherCount; //overall count from stateToCount
    std::map<DvmhDataState, DvmType> stateToCount;
};

class SourcePosition {
public:
    const std::string &getFileName() const { return fileName; }
    int getLineNumber() const { return lineNumber; }
public:
    explicit SourcePosition(std::string aFileName, int aLineNumber): fileName(aFileName), lineNumber(aLineNumber) {
        fileNameHash = calcCrc32((const unsigned char *)fileName.c_str(), fileName.size());
    }
public:
    bool operator<(const SourcePosition &sp) const;
protected:
    std::string fileName;
    int lineNumber;
    unsigned long fileNameHash;
};

class DvmhRegionPersistentInfo {
public:
    const std::vector<DvmhLoopPersistentInfo *> &getLoopInfos() const { return loopInfos; }
    int getAppearanceNumber() const { return appearanceNumber; }
    DvmType getExecCount() const { return execCount; }
    void incrementExecCount() { execCount++; }
    DvmhEvent *getLatestRegionEnd() const { return latestRegionEnd; }
public:
    explicit DvmhRegionPersistentInfo(const SourcePosition &sp, int number);
public:
    void addLoopInfo(DvmhLoopPersistentInfo *aLoopInfo) {
        loopInfos.push_back(aLoopInfo);
    }
    int getVarId(std::string name);
    void addDataDep(DvmhRegionData *rdata);
    void setLatestRegionEnd(DvmhEvent *event, bool owning = false);
public:
    ~DvmhRegionPersistentInfo();
protected:
    SourcePosition sourcePos;
// TODO: Add data-correspondence (instance of a region). In the form of map<varName, DvmhData *> maybe

    int appearanceNumber;
    std::map<std::string, int> nameToId;
    std::vector<DvmhDataDep> dataDeps; //Array of DvmhDataDep
    std::vector<DvmhLoopPersistentInfo *> loopInfos; //Array of DvmhLoopPersistentInfo *
    DvmType execCount;
    // TODO: Remove
    DvmhEvent *latestRegionEnd;
};

class DvmhRegion;

class DvmhRegionMapping {
public:
    unsigned long usesDevices;
    std::map<DvmhDistribSpace *, std::vector<double> > rdspaces; // DvmhDistribSpace * => weight vector
public:
    explicit DvmhRegionMapping(DvmhRegion *region, unsigned long availDevices);
};

class DvmhRegion: public DvmhObject {
public:
    enum Phase {rpRegistrations = 0, rpExecution = 1, rpFinished = 2};
public:
    Phase getPhase() const { return phase; }
    bool isAsync() const { return async; }
    bool withCompareDebug() const { return compareDebug; }
    DvmhRegionPersistentInfo *getPersistentInfo() const { return persistentInfo; }
    bool usesDevice(int dev) const { assert(phase == rpExecution); return (usesDevices & (1ul << dev)) != 0; }
    std::map<DvmhData *, DvmhRegionData *> *getDatas() { return &datas; }
    std::map<DvmhDistribSpace *, DvmhRegionDistribSpace *> *getDspaces() { return &dspaces; }
    DvmhEvent *getLatestLoopEnd() const { return latestLoopEnd; }
    bool canExecuteOn(DeviceType dt) const { assert(phase == rpExecution); return !!(canExecuteOnDeviceTypes & dt); }
public:
    explicit DvmhRegion(int flags, DvmhRegionPersistentInfo *persInfo);
public:
    void renewDatas();
    void renewData(DvmhData *data, DvmhPieces *restrictTo = 0);
    void registerData(DvmhData *data, int intent, const Interval indexes[]);
    void registerDspace(DvmhDistribSpace *dspace);
    void setDataName(DvmhData *data, const char *name, int nameLength);
    void setDataName(DvmhData *data, const char *name) { setDataName(data, name, strlen(name)); }
    const char *getDataName(DvmhData *data) const;
    void executeOnTargets(unsigned long deviceTypes);
    DvmhData *getDataByVarId(int varId) const;
    void fillLocalPart(int dev, DvmhData *data, DvmType part[]) const;
    bool hasElement(int dev, DvmhData *data, const DvmType indexArray[]) const;
    void addRemoteGroup(DvmhData *data);
    void markToRenew(DvmhData *data);
    bool canAddToActual(DvmhData *data, const DvmhPieces *indexes) const;
    bool canAddToActual(DvmhData *data, const Interval indexes[]) const;
    void setLatestLoopEnd(DvmhEvent *event, bool owning = false);
    void compareDatas(DvmhRegionData *rdata, DvmhPieces *area = 0);
    void finish();
public:
    ~DvmhRegion();
protected:
    void performRenew(DvmhRegionData *rdata, bool updateOut, DvmhPieces *restrictTo = 0);
    void mapSpaceOnDevices(DvmhRegionMapping *mapping, DvmhRegionDistribSpace *rdspace);
    void mapDataOnDevices(DvmhRegionData *rdata);
    void allocateDatasOnDevices();
protected:
    Phase phase;
    bool async;
    bool compareDebug;
    DvmhRegionPersistentInfo *persistentInfo;
    unsigned long canExecuteOnDeviceTypes;
    unsigned long usesDevices;
    std::map<DvmhData *, DvmhRegionData *> datas; //DvmhData * => DvmhRegionData *
    std::map<DvmhDistribSpace *, DvmhRegionDistribSpace *> dspaces; //DvmhDistribSpace * => DvmhRegionDistribSpace *
    AggregateEvent *startPrereq;
    DvmhEvent *latestLoopEnd;
};

extern std::map<SourcePosition, DvmhRegionPersistentInfo *> regionDict;
extern std::vector<double> lastBestWeights;

}
