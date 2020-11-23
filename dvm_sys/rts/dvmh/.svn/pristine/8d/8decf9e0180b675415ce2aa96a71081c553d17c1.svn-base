#pragma once

#include "dvmh_data.h"
#include "dvmlib_incs.h"

namespace libdvmh {

class DvmhDistribSpace;
class DvmhLoopPersistentInfo;
class DvmhLoop;

class DvmSysHandle {
public:
    DvmSysHandle(): handle(0) {}
    DvmSysHandle(SysHandle *handle): handle(handle) {}
    DvmSysHandle(UDvmType handle): handle((SysHandle *)handle) {}
    DvmSysHandle(DvmType handle): handle((SysHandle *)handle) {}
public:
    operator SysHandle *() { return handle; }
    SysHandle *operator->() { return handle; }
    operator bool() { return handle != 0; }
    bool operator<(const DvmSysHandle &other) const { return handle < other.handle; }
protected:
    SysHandle *handle;
};

SysHandle *getDvm(DvmhObject *obj, bool *pDvmIsMain = 0);
DvmhObject *getDvmh(DvmSysHandle handle);
template <class T>
T *getDvmh(DvmSysHandle handle) {
    return static_cast<T *>(getDvmh(handle));
}
void tieDvm(DvmhObject *obj, DvmSysHandle handle, bool dvmIsMain = false);
void untieDvm(DvmhObject *obj);
UDvmType getDvmMapSize();

void *getAddrFromDvm(s_DISARRAY *dvmHead);
void *getAddrFromDvmDesc(DvmType dvmDesc[]);
MultiprocessorSystem *createMpsFromVMS(s_VMS *vms, MultiprocessorSystem *parent);
DvmhDistribSpace *createDistribSpaceFromAMView(s_AMVIEW *amView, MultiprocessorSystem *mps);
DvmhData *createDataFromDvmArray(s_DISARRAY *dvmHead, DvmhDistribSpace *dspace, DvmhData::TypeType givenTypeType = DvmhData::ttUnknown);
DvmhLoop *createLoopFromDvmLoop(s_PARLOOP *dvmLoop, DvmhDistribSpace *dspace);
bool isDvmArrayRegular(DvmSysHandle handle);
bool isDvmObjectStatic(DvmSysHandle handle, bool defaultValue);

DvmhObject *getOrCreateTiedDvmh(DvmSysHandle handle, std::vector<DvmhObject *> *createdObjects = 0);
template <class T>
T *getOrCreateTiedDvmh(DvmSysHandle handle, std::vector<DvmhObject *> *createdObjects = 0) {
    return static_cast<T *>(getOrCreateTiedDvmh(handle, createdObjects));
}

}
