#pragma once

#include <cassert>
#include <map>
#include <set>
#include <vector>

#include "dvmh_types.h"
#include "dvmh_async.h"

namespace libdvmh {

class MultiprocessorSystem;
class DvmhData;
class DvmhLoop;
class DvmhLoopCuda;
class DvmhRegion;

// Regular variable description
class RegularVar {
public:
    UDvmType getSize() const { return varSize; }
    UDvmType getDataRegionDepth() const { return dataRegionDepth; }
    DvmhData *getData() const { return data; }
    void setData(DvmhData *aData) { assert(!aData || dataRegionDepth > 0); data = aData; }
public:
    explicit RegularVar(bool autoEnter, UDvmType size): varSize(size), dataRegionDepth(autoEnter ? 1 : 0), data(0) {}
public:
    UDvmType expandSize(UDvmType size) {
        varSize = std::max(varSize, size);
        return varSize;
    }
    UDvmType dataRegionEnter() {
        return ++dataRegionDepth;
    }
    UDvmType dataRegionExit() {
        assert(dataRegionDepth > 0);
        return --dataRegionDepth;
    }
protected:
    UDvmType varSize;
    UDvmType dataRegionDepth;
    DvmhData *data;
};

void dvmhTryToFreeSpace(UDvmType memNeeded, int dev, const std::set<DvmhData *> &except);
void dvmhInitialize();
void dvmhFinalize(bool cleanup);
void handlePreAcross(DvmhLoop *loop, const LoopBounds curLoopBounds[]);
void handlePostAcross(DvmhLoop *loop, const LoopBounds curLoopBounds[]);
int autotransformInternal(DvmhLoopCuda *cloop, DvmhData *data);
DvmhObject *passOrGetOrCreateDvmh(DvmType handle, bool addToAllObjects, std::vector<DvmhObject *> *createdObjects = 0);

extern MultiprocessorSystem *rootMPS;
extern MultiprocessorSystem *currentMPS;
extern DvmhRegion *currentRegion;
extern DvmhLoop *currentLoop;
extern bool inited;
extern bool finalized;
extern std::set<DvmhObject *> allObjects; // All user-created arrays and templates
extern std::vector<char> stringBuffer;
extern std::vector<std::pair<char *, UDvmType> > stringVariables;
extern int currentLine;
extern char currentFile[1024];
extern std::map<const void *, RegularVar *> regularVars;
extern DvmhSpinLock regularVarsLock;
  
extern std::map<int, DvmhFile *> fortranFiles;
  
struct stringLessComparator {
  bool operator()(const char *a, const char *b) const {
    return strcmp(a, b) < 0;
  }
};


char *getStr(DvmType ref, bool clearTrailingBlanks = false, bool toUpper = false);
inline char *getStr(const DvmType *pRef, bool clearTrailingBlanks = false, bool toUpper = false) { return getStr(*pRef, clearTrailingBlanks, toUpper); }
inline char *getStrAddr(DvmType ref) { return (ref > 0 ? stringVariables[ref - 1].first : 0); }
inline char *getStrAddr(const DvmType *pRef) { return getStrAddr(*pRef); }
inline UDvmType getStrSize(DvmType ref) { return (ref > 0 ? stringVariables[ref - 1].second : 0); }
inline UDvmType getStrSize(const DvmType *pRef) { return getStrSize(*pRef); }

}
