#include "dvmh.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

#include "include/dvmhlib_const.h"

#include "cuda_reduction.h"
#include "cuda_transform.h"
#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_device.h"
#include "dvmh_predictor.h"
#include "dvmh_rts.h"
#include "dvmh_stat.h"
#include "dvmlib_adapter.h"
#include "loop.h"
#include "loop_distribution.h"
#include "mps.h"
#include "region.h"

using namespace libdvmh;

static DvmhData *findDataForAddr(const void *addr, RegularVar **pRegVar = 0) {
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, addr);
    }
    if (pRegVar)
        *pRegVar = regVar;
    return (regVar ? regVar->getData() : 0);
}

static DvmhData *findDataForDvm(DvmType handle, bool seekInRegular = true) {
    DvmhData *data = getDvmh<DvmhData>(handle);
    if (!data && seekInRegular)
        data = findDataForAddr(getAddrFromDvmDesc(&handle));
    return data;
}

static void dvmhDestroyVariable(DvmhData *data) {
    assert(data);
    DvmhDistribSpace *dspace = 0;
    if (data->isDistributed())
        dspace = data->getAlignRule()->getDspace();
    if (!data->isDistributed() && data->getRepr(0)) {
        void *addr = data->getBuffer(0)->getDeviceAddr();
        RegularVar *regVar = 0;
        {
            SpinLockGuard guard(regularVarsLock);
            regVar = dictFind2(regularVars, addr);
        }
        if (regVar) {
            regVar->setData(0);
            regVar->dataRegionExit();
            checkInternal(regVar->getDataRegionDepth() == 0);
            delete regVar;
            {
                SpinLockGuard guard(regularVarsLock);
                regularVars.erase(addr);
            }
        }
    }
    untieDvm(data);
    allObjects.erase(data);
    delete data;
    if (dspace && dspace->getRefCount() == 0)
        delete dspace; // DvmhDistribSpace automatically unties itself
    dvmh_log(TRACE, "variable destroyed OK data was %p", data);
}

extern "C" void dvmh_finish_() {
    bool cleanup = true;
    dvmhFinalize(cleanup);
}

static void dvmhGetActualInternal(DvmhData *data, const DvmType lowIndex[], const DvmType highIndex[], DvmhCopyingPurpose::Value purpose =
        DvmhCopyingPurpose::dcpGetActual) {
    int rank = data->getRank();
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    if (fillRealBlock(rank, lowIndex, highIndex, data->getSpace(), indexes)) {
        checkError2(data->getSpace()->blockContains(rank, indexes), "Index out of bounds");
        if (indexes->blockIntersectInplace(rank, data->getLocalPlusShadow())) {
            data->syncWriteAccesses();
            PushCurrentPurpose setPurpose(purpose);
            data->getActual(indexes, (currentRegion ? currentRegion->canAddToActual(data, indexes) : true));
        }
    }
}

extern "C" void dvmh_get_actual_subvariable_(void *addr, DvmType lowIndex[], DvmType highIndex[]) {
    checkError2(addr, "NULL pointer is passed to dvmh_get_actual_subvariable");
    DvmhData *data = findDataForAddr(addr);
    if (data)
        dvmhGetActualInternal(data, lowIndex, highIndex);
}

extern "C" void dvmh_get_actual_variable_(void *addr) {
    dvmh_get_actual_subvariable_(addr, 0, 0);
}

extern "C" void dvmh_get_actual_subarray_(DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[]) {
    checkInternal(dvmDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    if (data)
        dvmhGetActualInternal(data, lowIndex, highIndex);
}

extern "C" void dvmh_get_actual_array_(DvmType dvmDesc[]) {
    dvmh_get_actual_subarray_(dvmDesc, 0, 0);
}

extern "C" void dvmh_get_actual_all_() {
    for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
        DvmhData *data = (*it)->as_s<DvmhData>();
        if (data)
            dvmhGetActualInternal(data, 0, 0);
    }
}

static void dvmhSetActualInternal(DvmhData *data, const DvmType lowIndex[], const DvmType highIndex[]) {
    int rank = data->getRank();
#ifdef NON_CONST_AUTOS
    Interval indexes[rank];
#else
    Interval indexes[MAX_ARRAY_RANK];
#endif
    if (fillRealBlock(rank, lowIndex, highIndex, data->getSpace(), indexes)) {
        checkError2(data->getSpace()->blockContains(rank, indexes), "Index out of bounds");
        if (indexes->blockIntersectInplace(rank, data->getLocalPart())) {
            data->syncAllAccesses();
            data->setActual(indexes);
            if (currentRegion)
                currentRegion->markToRenew(data);
        }
    }
}

extern "C" void dvmh_actual_subvariable_(void *addr, DvmType lowIndex[], DvmType highIndex[]) {
    checkError2(addr, "NULL pointer is passed to dvmh_actual_subvariable");
    DvmhData *data = findDataForAddr(addr);
    if (data)
        dvmhSetActualInternal(data, lowIndex, highIndex);
}

extern "C" void dvmh_actual_variable_(void *addr) {
    dvmh_actual_subvariable_(addr, 0, 0);
}

extern "C" void dvmh_actual_subarray_(DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[]) {
    checkInternal(dvmDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    if (data)
        dvmhSetActualInternal(data, lowIndex, highIndex);
}

extern "C" void dvmh_actual_array_(DvmType dvmDesc[]) {
    dvmh_actual_subarray_(dvmDesc, 0, 0);
}

extern "C" void dvmh_actual_all_() {
    for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
        DvmhData *data = (*it)->as_s<DvmhData>();
        if (data)
            dvmhSetActualInternal(data, 0, 0);
    }
}

extern "C" void dvmh_remote_access_(DvmType dvmDesc[]) {
    checkInternal(dvmDesc);
    s_DISARRAY *BArr = (s_DISARRAY *)((SysHandle *)dvmDesc[0])->pP;
    s_REGBUF *RegBuf = BArr->RegBuf;
    DvmType *DArrHeader = (DvmType *)RegBuf->DAHandlePtr->HeaderPtr;
    DvmhData *data = getDvmh<DvmhData>(DArrHeader[0]);
    if (data) {
        int rank = data->getRank();
#ifdef NON_CONST_AUTOS
        Interval indexes[rank];
#else
        Interval indexes[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < rank; i++) {
            indexes[i][0] = RegBuf->InitIndex[i] + data->getAxisSpace(i + 1)[0];
            indexes[i][1] = RegBuf->LastIndex[i] + data->getAxisSpace(i + 1)[0];
            dvmh_log(TRACE, "RegularBufferIndexes[%d] = [" DTFMT ".." DTFMT "]", i, indexes[i][0], indexes[i][1]);
        }
        if (indexes->blockIntersectInplace(rank, data->getLocalPart())) {
            data->syncWriteAccesses();
            PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpRemote);
            data->getActual(indexes, (currentRegion ? currentRegion->canAddToActual(data, indexes) : true));
        }
    }
    dvmh_log(TRACE, "dvmh_remote_access_ OK data was %p", data);
}

static void addToShadow(DvmhShadow *shadow, s_BOUNDGROUP *bg, bool autoRegister = false) {
    dvmh_log(TRACE, "Adding to shadow %d arrays (all count=%d, newShd count=%d)", bg->NewArrayColl.Count, bg->ArrayColl.Count, bg->NewShdWidthColl.Count);
    for (int i = 0; i < bg->NewArrayColl.Count; i++) {
        s_DISARRAY *dvmHead = (s_DISARRAY *)bg->NewArrayColl.List[i];
        if (autoRegister)
            dvmh_register_array_((DvmType *)dvmHead->HandlePtr->HeaderPtr);
        DvmhData *data = getDvmh<DvmhData>(dvmHead->HandlePtr);
        if (data) {
            DvmhShadowData shdData;
            shdData.data = data;
            s_SHDWIDTH *shdWidth = (s_SHDWIDTH *)bg->NewShdWidthColl.List[i];
            shdData.cornerFlag = shdWidth->MaxShdCount > 1;
            shdData.shdWidths = new ShdWidth[data->getRank()];
            dvmh_log(TRACE, "Registered array (rank=%d) for shadow renew. Corner flag=%d", data->getRank(), (int)shdData.cornerFlag);
            for (int j = 0; j < data->getRank(); j++) {
                shdData.shdWidths[j][0] = shdWidth->ResLowShdWidth[j];
                shdData.shdWidths[j][1] = shdWidth->ResHighShdWidth[j];
                dvmh_log(TRACE, "Shadow sizes: low=" DTFMT " high=" DTFMT "", shdData.shdWidths[j][0], shdData.shdWidths[j][1]);
            }
            shadow->add(shdData);
        }
    }
}

extern "C" void dvmh_shadow_renew_(ShadowGroupRef *group) {
    if (group && *group) {
        assert(group);
        s_BOUNDGROUP *bg = (s_BOUNDGROUP *)((SysHandle *)*group)->pP;
        checkInternal(bg != 0);
        DvmhShadow *shadow = new DvmhShadow;
        addToShadow(shadow, bg);

        shadow->renew(currentRegion, false);

        delete shadow;
    }
}

extern "C" void dvmh_redistribute_(DvmType dvmDesc[], DvmType *newValueFlagRef) {
    checkInternal(dvmDesc && newValueFlagRef);
    bool newValueFlag = (*newValueFlagRef == 1);
    checkError2(currentRegion == 0, "Redistribution is not allowed inside a region");

    // XXX: The same check is present in LibDVM
    if (DVM_VMS->ProcCount == 1)
        return;

    DvmhObject *templateObj = getDvmh(dvmDesc[0]);
    DvmhDistribSpace *dspace = 0;
    if (templateObj && templateObj->is<DvmhData>()) {
        DvmhData *templateData = templateObj->as<DvmhData>();
        if (templateData->isDistributed())
            dspace = templateData->getAlignRule()->getDspace();
    } else if (templateObj && templateObj->is<DvmhDistribSpace>()) {
        dspace = templateObj->as<DvmhDistribSpace>();
    } else {
        checkInternal2(!templateObj, "Only distributed arrays or templates can be passed to dvmh_redistribute");
        SysHandle *hndl = (SysHandle *)dvmDesc[0];
        if (hndl->Type == sht_DisArray) {
            s_DISARRAY *dvmHead = (s_DISARRAY *)hndl->pP;
            checkInternal(dvmHead != 0);
            dspace = getDvmh<DvmhDistribSpace>(dvmHead->AMView->HandlePtr);
        } else {
            checkInternal2(hndl->Type == sht_AMView, "Only distributed arrays or templates can be passed to dvmh_redistribute");
        }
    }
    if (dspace) {
        SysHandle *hndl = getDvm(dspace);
        std::vector<DvmhData *> toDestroy;
        toDestroy.reserve(dspace->getRefCount());
        for (std::set<DvmhData *>::const_iterator it = dspace->getAlignedDatas().begin(); it != dspace->getAlignedDatas().end(); it++) {
            DvmhData *data = *it;
            assert(data);
            if (!newValueFlag)
                dvmhGetActualInternal(data, 0, 0, DvmhCopyingPurpose::dcpRedistribute);
            toDestroy.push_back(data);
        }
        for (int i = 0; i < (int)toDestroy.size(); i++)
            dvmhDestroyVariable(toDestroy[i]);
        checkInternal(getDvmh(hndl) == 0);
    }
}

extern "C" void dvmh_realign_(DvmType dvmDesc[], DvmType *newValueFlagRef) {
    checkInternal(dvmDesc && newValueFlagRef);
    int newValueFlag = *newValueFlagRef;
    checkError2(currentRegion == 0, "Realigning is not allowed inside a region");

    // XXX: The same check is present in LibDVM
    if (DVM_VMS->ProcCount == 1)
        return;

    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    if (data) {
        if (!newValueFlag)
            dvmhGetActualInternal(data, 0, 0, DvmhCopyingPurpose::dcpRedistribute);
        dvmhDestroyVariable(data);
    }
}

extern "C" void dvmh_destroy_variable_(void *addr) {
    checkInternal2(addr, "NULL pointer is passed to dvmh_destroy_variable");
    DvmhData *data = findDataForAddr(addr);
    if (data)
        dvmhDestroyVariable(data);
}

extern "C" void dvmh_destroy_array_(DvmType dvmDesc[]) {
    checkInternal(dvmDesc);
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    if (data)
        dvmhDestroyVariable(data);
}

extern "C" void *dvmh_get_device_addr(DvmType *deviceRef, void *variable) {
    checkInternal(deviceRef && variable);
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, variable);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    checkInternal2(data != 0, "Requesting device address of unregistered variable");
    DvmhBuffer *buf = data->getBuffer(*deviceRef);
    if (buf)
        return buf->getDeviceAddr();
    else
        return 0;
}

extern "C" DvmType dvmh_calculate_offset_(DvmType *deviceRef, void *base, void *variable) {
    checkInternal(deviceRef && variable);
    RegularVar *regVar = 0;
    {
        SpinLockGuard guard(regularVarsLock);
        regVar = dictFind2(regularVars, variable);
    }
    DvmhData *data = (regVar ? regVar->getData() : 0);
    checkInternal2(data != 0, "Requesting offset of unregistered variable");
    DvmhBuffer *buf = data->getBuffer(*deviceRef);
    if (buf) {
        checkInternal2(((DvmType)buf->getDeviceAddr() - (DvmType)base) % data->getTypeSize() == 0, "Unable to calculate offset");
        return ((DvmType)buf->getDeviceAddr() - (DvmType)base) / data->getTypeSize();
    } else {
        return 0;
    }
}

extern "C" void *dvmh_get_natural_base(DvmType *deviceRef, DvmType dvmDesc[]) {
    checkInternal(deviceRef && dvmDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    checkInternal2(data != 0, "Requesting natural base of unregistered variable");
    DvmhBuffer *buf = data->getBuffer(*deviceRef);
    if (buf)
        return buf->getNaturalBase(true);
    else
        return 0;
}

extern "C" void dvmh_fill_header_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[]) {
    checkInternal(deviceRef && dvmDesc && dvmhDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    checkInternal2(data != 0, "Requesting to fill device header of unregistered variable");
    DvmhBuffer *buf = data->getBuffer(*deviceRef);
    if (buf)
        buf->fillHeader(base, dvmhDesc, true);
    else
        memset(dvmhDesc, 0, sizeof(DvmType) * (data->getRank() + 3));
}

extern "C" void dvmh_fill_header_ex_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[], DvmType *outTypeOfTransformation, DvmType extendedParams[]) {
    checkInternal(deviceRef && dvmDesc && dvmhDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    checkError2(data != 0, "Requesting to fill device header of unregistered variable");
    DvmhBuffer *buf = data->getBuffer(*deviceRef);
    if (buf) {
        DvmhDiagonalInfo diagInfo;
        buf->fillHeader(base, dvmhDesc, true, &diagInfo);
        int transType = buf->isTransformed() + buf->isDiagonalized();
        if (outTypeOfTransformation)
            *outTypeOfTransformation = transType;
        if (extendedParams) {
            if (transType == 2) {
                extendedParams[0] = diagInfo.x_axis;
                extendedParams[1] = diagInfo.x_first;
                extendedParams[2] = diagInfo.x_length;
                extendedParams[3] = diagInfo.y_axis;
                extendedParams[4] = diagInfo.y_first;
                extendedParams[5] = diagInfo.y_length;
                extendedParams[6] = diagInfo.slashFlag;
            }
        }
    } else {
        memset(dvmhDesc, 0, sizeof(DvmType) * (data->getRank() + 3));
        if (outTypeOfTransformation)
            *outTypeOfTransformation = 0;
    }
}

extern "C" DvmType region_create_(DvmType *flagsRef) {
    checkInternal(flagsRef);
    checkError2(currentRegion == 0, "Nested regions are not allowed");
    SourcePosition sp(DVM_FILE[0], DVM_LINE[0]);
    DvmhRegionPersistentInfo *persInfo = dictFind2(regionDict, sp);
    if (!persInfo) {
        int appearanceNumber = regionDict.size() + 1;
        persInfo = new DvmhRegionPersistentInfo(sp, appearanceNumber);
        regionDict[sp] = persInfo;
    }
    assert(persInfo);
    int flags = *flagsRef;
    DvmhRegion *region = new DvmhRegion(flags, persInfo);
    dvmh_log(TRACE, "region_create ok");
    currentRegion = region;
    return (DvmType)region;
}

extern "C" void region_register_subarray_(DvmType *regionRef, DvmType *intentRef, DvmType dvmDesc[], DvmType lowIndex[], DvmType highIndex[], DvmType *elemType)
{
    checkInternal(regionRef && intentRef && dvmDesc && elemType);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    if (!data) {
        data = getOrCreateTiedDvmh<DvmhData>(dvmDesc[0]);
        if (data->getTypeType() == DvmhData::ttUnknown)
            data->setTypeType(DvmhData::getTypeType((DvmhData::DataType)*elemType));
        allObjects.insert(data);
        if (data->isAligned() && !data->isDistributed()) {
            void *addr = data->getBuffer(0)->getDeviceAddr();
            SpinLockGuard guard(regularVarsLock);
            RegularVar *regVar = dictFind2(regularVars, addr);
            if (regVar) {
                checkInternal(regVar->getDataRegionDepth() == 1);
            } else {
                regVar = new RegularVar(true, 0);
                regularVars[addr] = regVar;
            }
            regVar->setData(data);
        }
    }
    assert(data != 0);
    int rank = data->getRank();
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
#ifdef NON_CONST_AUTOS
    Interval realBlock[rank];
#else
    Interval realBlock[MAX_ARRAY_RANK];
#endif
    fillRealBlock(rank, lowIndex, highIndex, data->getSpace(), realBlock);
    checkError2(data->getSpace()->blockContains(rank, realBlock), "Index out of bounds");
    realBlock->blockIntersectInplace(rank, data->getLocalPlusShadow());
    region->registerData(data, *intentRef, realBlock);
    dvmh_log(TRACE, "region_register_subarray OK");
}

extern "C" void region_register_array_(DvmType *regionRef, DvmType *intentRef, DvmType dvmDesc[], DvmType *elemType) {
    checkInternal(regionRef && intentRef && dvmDesc && elemType);
    checkError2(dvmDesc[0], "NULL pointer is passed to region_register_array");
    if (((SysHandle *)dvmDesc[0])->Type == sht_AMView) {
        DvmhDistribSpace *dspace = getOrCreateTiedDvmh<DvmhDistribSpace>(dvmDesc[0]);
        DvmhRegion *region = (DvmhRegion *)*regionRef;
        checkInternal2(region != 0, "NULL region reference");
        region->registerDspace(dspace);
    } else {
        region_register_subarray_(regionRef, intentRef, dvmDesc, 0, 0, elemType);
    }
}

extern "C" void region_register_scalar_(DvmType *regionRef, DvmType *intentRef, void *addr, DvmType *sizeRef, DvmType *varType) {
    checkInternal(regionRef && intentRef && sizeRef && varType);
    checkError2(addr, "NULL pointer is passed to region_register_scalar");
    RegularVar *regVar = 0;
    DvmhData *data = findDataForAddr(addr, &regVar);
    if (!data) {
        DvmhData::TypeType tt = DvmhData::ttUnknown;
        if (varType)
            tt = DvmhData::getTypeType((DvmhData::DataType)*varType);
        data = new DvmhData(*sizeRef, tt, 0, 0);
        data->realign(0);
        data->setHostPortion(addr, 0);
        data->initActual(0);
        if (regVar) {
            checkInternal(regVar->getDataRegionDepth() == 1);
        } else {
            regVar = new RegularVar(true, 0);
            {
                SpinLockGuard guard(regularVarsLock);
                regularVars[addr] = regVar;
            }
        }
        regVar->setData(data);
        allObjects.insert(data);
    }
    assert(data != 0);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
    region->registerData(data, *intentRef, 0);
    dvmh_log(TRACE, "region_register_scalar OK");
}

extern "C" void region_set_name_array_(DvmType *regionRef, DvmType dvmDesc[], const char *name, int nameLength) {
    checkInternal(regionRef && dvmDesc && name);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
    if (((SysHandle *)dvmDesc[0])->Type == sht_AMView) {
        // TODO: Handle name of a TEMPLATE somehow
    } else {
        DvmhData *data = findDataForDvm(dvmDesc[0]);
        checkInternal2(data != 0, "Setting name for unregistered variable");
        region->setDataName(data, name, nameLength);
    }
}

extern "C" void region_set_name_variable_(DvmType *regionRef, void *addr, const char *name, int nameLength) {
    checkInternal(regionRef && name);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
    checkError2(addr, "NULL pointer is passed to region_set_name_variable");
    DvmhData *data = findDataForAddr(addr);
    checkInternal2(data != 0, "Setting name for unregistered variable");
    region->setDataName(data, name, nameLength);
}

extern "C" void region_execute_on_targets_(DvmType *regionRef, DvmType *devicesRef) {
    checkInternal(regionRef && devicesRef);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region, "NULL region reference");
    region->executeOnTargets(*devicesRef);
}

extern "C" void region_handle_consistent_(DvmType *regionRef, DAConsistGroupRef *group) {
    checkInternal(regionRef && group);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to region_handle_consistent");
    s_DACONSISTGROUP *dag = (s_DACONSISTGROUP *)((SysHandle *)*group)->pP;
    checkInternal(dag != 0);
    if (DVM_VMS->ProcCount > 1) {
        for (int i = 0; i < dag->RDA.Count; i++) {
            s_DISARRAY *dvmHead = (s_DISARRAY *)dag->RDA.List[i];
            DvmhData *data = findDataForDvm((DvmType)dvmHead->HandlePtr);
            if (data) {
                int rank = data->getRank();
#ifdef NON_CONST_AUTOS
                Interval indexes[rank];
#else
                Interval indexes[MAX_ARRAY_RANK];
#endif
                bool bad = false;
                for (int j = 0; j < rank; j++) {
                    Interval axisSpace = data->getAxisSpace(j + 1);
                    indexes[j][0] = dvmHead->DArrBlock.Set[j].Lower + axisSpace[0];
                    indexes[j][1] = dvmHead->DArrBlock.Set[j].Upper + axisSpace[0];
                    dvmh_log(TRACE, "Consistent[%d] AllocSign=%d [" DTFMT ".." DTFMT "] of [" DTFMT ".." DTFMT "]", j, dvmHead->AllocSign, indexes[j][0],
                            indexes[j][1], axisSpace[0], axisSpace[1]);
                    if (!axisSpace.contains(indexes[j]))
                        bad = true;
                }
                if (bad)
                    dvmh_log(DEBUG, "Bad consistent local part");
                if (data->hasLocal()) {
                    if (bad)
                        indexes->blockAssign(rank, data->getLocalPart());
                    if (indexes->blockIntersectInplace(rank, data->getLocalPart())) {
                        data->syncAllAccesses();
                        PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpConsistent);
                        data->getActual(indexes, (currentRegion ? currentRegion->canAddToActual(data, indexes) : true));
                    }
                }
                dvmhSetActualInternal(data, 0, 0);
            }
        }
    }
}

extern "C" void region_after_waitrb_(DvmType *regionRef, DvmType dvmDesc[]) {
    checkInternal(regionRef && dvmDesc);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    if (!data) {
        s_DISARRAY *dvmHead = (s_DISARRAY *)((SysHandle *)dvmDesc[0])->pP;
        checkInternal(dvmHead != 0);
        DvmhDistribSpace *dspace = 0;
        data = createDataFromDvmArray(dvmHead, dspace);
        tieDvm(data, dvmDesc[0], true);
        allObjects.insert(data);
    }
    region->addRemoteGroup(data);
    dvmh_log(TRACE, "region_after_waitrb_ OK data was %p", data);
}

extern "C" void region_destroy_rb_(DvmType *regionRef, DvmType dvmDesc[]) {
    checkInternal(regionRef && dvmDesc);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region != 0, "NULL region reference");
    dvmh_destroy_array_(dvmDesc);
}

extern "C" void region_end_(DvmType *regionRef) {
    checkInternal(regionRef);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    checkInternal2(region && region == currentRegion, "Incorrect region reference is passed to region_end");
    region->finish();
    delete region;
    currentRegion = 0;
    dvmh_log(TRACE, "region_end_ done");
}

extern "C" DvmType loop_create_(DvmType *regionRef, LoopRef *InDvmLoop) {
    checkInternal(regionRef && InDvmLoop);
    DvmhRegion *region = (DvmhRegion *)*regionRef;
    DvmhLoop *loop;
    SourcePosition sp(DVM_FILE[0], DVM_LINE[0]);
    DvmhLoopPersistentInfo *persInfo = dictFind2(loopDict, sp);
    if (!persInfo) {
        persInfo = new DvmhLoopPersistentInfo(sp, (region ? region->getPersistentInfo() : 0));
        loopDict[sp] = persInfo;
    }
    assert(persInfo);
    if (*InDvmLoop) {
        // Parallel loop
        loop = getOrCreateTiedDvmh<DvmhLoop>(*InDvmLoop);
        loop->setRegion(region);
        loop->setPersistentInfo(persInfo);
        if (region) {
            if (loop->mappingData) {
                DvmhRegionData *rdata = dictFind2(*region->getDatas(), loop->mappingData);
                checkInternal2(rdata, "Array, on which parallel loop is mapped, must be registered in the region");
                dvmh_log(TRACE, "loop mapped on variable %s", rdata->getName());
                persInfo->setVarId(rdata->getVarId());
            } else {
                DvmhRegionDistribSpace *rdspace = dictFind2(*region->getDspaces(), loop->alignRule->getDspace());
                checkInternal2(rdspace, "Template, on which parallel loop is mapped, must be registered in the region");
            }
        }
        if (loop->mappingData)
            allObjects.insert(loop->mappingData);
    } else {
        // Sequential part
        loop = new DvmhLoop(0, 0);
        loop->setRegion(region);
        loop->setPersistentInfo(persInfo);
    }
    dvmh_log(TRACE, "loop created");
    return (DvmType)loop;
}

extern "C" void loop_insred_(DvmType *InDvmhLoop, RedRef *InRedRefPtr) {
    checkInternal(InDvmhLoop && InRedRefPtr);
    DvmhLoop *loop = (DvmhLoop *)*InDvmhLoop;
    checkInternal2(loop != 0, "NULL loop reference");
    s_REDVAR *dvmRVar = (s_REDVAR *)((SysHandle *)(*InRedRefPtr))->pP;
    checkInternal(dvmRVar != 0);
    int funcNumber = dvmRVar->Func == rf_MAXLOC ? DvmhReduction::rfMax : (dvmRVar->Func == rf_MINLOC ? DvmhReduction::rfMin : dvmRVar->Func);
    DvmhReduction *reduction = new DvmhReduction((DvmhReduction::RedFunction)funcNumber, dvmRVar->VLength, (DvmhData::DataType)dvmRVar->VType, dvmRVar->Mem,
            dvmRVar->LocElmLength, dvmRVar->LocMem);
    loop->addReduction(reduction);
}

extern "C" void loop_across_(DvmType *InDvmhLoop, ShadowGroupRef *oldGroup, ShadowGroupRef *newGroup) {
    checkInternal(InDvmhLoop);
    DvmhLoop *loop = (DvmhLoop *)(*InDvmhLoop);
    checkInternal2(loop, "NULL loop reference");

    // Save info for ACROSS handling
    DvmhShadow myOldGroup, myNewGroup;
    s_BOUNDGROUP *oldBG = 0;
    if (oldGroup && *oldGroup)
        oldBG = (s_BOUNDGROUP *)((SysHandle *)*oldGroup)->pP;
    if (oldBG)
        addToShadow(&myOldGroup, oldBG, loop->region == 0);
    s_BOUNDGROUP *newBG = 0;
    if (newGroup && *newGroup)
        newBG = (s_BOUNDGROUP *)((SysHandle *)*newGroup)->pP;
    if (newBG)
        addToShadow(&myNewGroup, newBG, loop->region == 0);
    loop->addToAcross(myOldGroup, myNewGroup);
}

extern "C" void loop_set_cuda_block_(DvmType *InDvmhLoop, DvmType *InXRef, DvmType *InYRef, DvmType *InZRef) {
    checkInternal(InDvmhLoop && InXRef && InYRef && InZRef);
    DvmhLoop *loop = (DvmhLoop *)*InDvmhLoop;
    checkInternal2(loop != 0, "NULL loop reference");
    loop->setCudaBlock(*InXRef, *InYRef, *InZRef);
}

extern "C" void loop_shadow_compute_(DvmType *InDvmhLoop, DvmType dvmDesc[]) {
    checkInternal(InDvmhLoop && dvmDesc);
    DvmhLoop *loop = (DvmhLoop *)(*InDvmhLoop);
    checkInternal2(loop, "NULL loop reference");
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    checkInternal(loop->shadowComputeWidths);
    checkInternal(data);
    checkError2(data->isDistributed(), "Array must be distributed or aligned");
    checkError2(data->getAlignRule()->getDspace() == loop->alignRule->getDspace(), "Array must be aligned with the same template as loop mapped to");
    DvmhRegionData *rdata = dictFind2(*loop->region->getDatas(), data);
    checkInternal2(rdata, "Only previously registered arrays can be passed to loop_shadow_compute");
    loop->addShadowComputeData(data);
}

extern "C" void loop_register_handler_(DvmType *InDvmhLoop, DvmType *deviceTypeRef, DvmType *flagsRef, DvmHandlerFunc f, DvmType *basesCount, DvmType *paramCount, ...) {
    checkInternal(InDvmhLoop && deviceTypeRef && flagsRef && f && basesCount && paramCount);
    DvmhLoop *loop = (DvmhLoop *)*InDvmhLoop;
    checkInternal2(loop != 0, "NULL array reference");
    DeviceType deviceType = (DeviceType)ilog(*deviceTypeRef);
    checkInternal(deviceType >= 0 && deviceType < DEVICE_TYPES);
    checkInternal(*basesCount >= 0);
    checkInternal(*paramCount >= 0);
    DvmhLoopHandler *handler = new DvmhLoopHandler((*flagsRef & HANDLER_TYPE_PARALLEL) != 0, (*flagsRef & HANDLER_TYPE_MASTER) != 0, f, *paramCount,
            *basesCount);
    if (*paramCount > 0) {
        va_list ap;
        va_start(ap, paramCount);
        for (int i = 0; i < *paramCount; i++)
            handler->setParam(i, va_arg(ap, void *));
        va_end(ap);
    }
    loop->addHandler(deviceType, handler);
    dvmh_log(TRACE, "registered handler #%d for %s. isMaster=%d isParallel=%d paramsCount=%d", (int)loop->handlers[deviceType].size() - 1,
            (deviceType == dtHost ? "HOST" : "CUDA"), (int)handler->isMaster(), (int)handler->isParallel(), (int)*paramCount);
}

extern "C" void loop_perform_(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhLoop *loop = (DvmhLoop *)*InDvmhLoop;
    checkInternal2(loop != 0, "NULL loop reference");
    DvmhRegion *region = loop->region;
    if (loop->rank > 0) {
#ifdef NON_CONST_AUTOS
        LoopBounds toExecute[loop->rank];
#else
        LoopBounds toExecute[MAX_LOOP_RANK];
#endif
        LoopRef dvmLoop = (LoopRef)getDvm(loop);
        checkInternal(dvmLoop);
        s_PARLOOP *parLoop = (s_PARLOOP *)(((SysHandle *)dvmLoop)->pP);
        checkInternal(parLoop);
        // Should be after shadow renewals
        loop->prepareExecution();
        //loop->acrossNew.clearActual();
        if (!loop->hasLocal) {
            handlePreAcross(loop, loop->loopBounds);
            handlePostAcross(loop, loop->loopBounds);
        }
        bool finishLoop = false;
        for (DvmType iter = 0; !finishLoop; iter++) {
            if (dopl_(&dvmLoop) != 0) {
                for (int i = 0; i < loop->rank; i++) {
                    toExecute[i][0] = (*parLoop->MapList[i].InitIndexPtr);
                    toExecute[i][1] = (*parLoop->MapList[i].LastIndexPtr);
                    toExecute[i][2] = (*parLoop->MapList[i].StepPtr);
                }
            } else {
                break;
            }
            handlePreAcross(loop, toExecute);
            loop->executePart(toExecute);
            handlePostAcross(loop, toExecute);
        }
    } else {
        loop->prepareExecution();
        loop->executePart(0);
    }
    loop->afterExecution();
    delete loop;
    dvmh_log(TRACE, "loop ended async=%d", (int)(region ? region->isAsync() : false));
}

extern "C" DvmType loop_get_device_num_(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    return sloop->getDeviceNum();
}

extern "C" DvmType loop_has_element_(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType indexArray[]) {
    checkInternal(InDvmhLoop && dvmDesc && indexArray);
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    checkInternal2(data, "Array is not registered");
    return ((DvmhSpecLoop *)*InDvmhLoop)->hasElement(data, indexArray);
}

extern "C" void loop_fill_bounds_(DvmType *InDvmhLoop, DvmType lowIndex[], DvmType highIndex[], DvmType stepIndex[]) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    for (int i = 0; i < loop->rank; i++) {
        if (lowIndex)
            lowIndex[i] = sloop->getPortion()->getLoopBounds(i + 1).begin();
        if (highIndex)
            highIndex[i] = sloop->getPortion()->getLoopBounds(i + 1).end();
        if (stepIndex)
            stepIndex[i] = sloop->getPortion()->getLoopBounds(i + 1).step();
    }
}

extern "C" void loop_fill_local_part_(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType part[]) {
    checkInternal(InDvmhLoop && dvmDesc && part);
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    checkInternal2(data, "Array is not registered");
    ((DvmhSpecLoop *)*InDvmhLoop)->fillLocalPart(data, part);
}

extern "C" void loop_red_init_(DvmType *InDvmhLoop, DvmType *InRedNumRef, void *arrayPtr, void *locPtr) {
    checkInternal(InDvmhLoop && InRedNumRef && arrayPtr);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    checkInternal3(*InRedNumRef >= 1 && *InRedNumRef <= (int)sloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)*InRedNumRef);
    DvmhReduction *red = sloop->getLoop()->reductions[*InRedNumRef - 1];
    assert(red);
    red->initValues(arrayPtr, locPtr);
}

extern "C" DvmType loop_get_slot_count_(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    return sloop->getPortion()->getSlotsToUse();
}

extern "C" DvmType loop_get_dependency_mask_(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    return loop->dependencyMask;
}

extern "C" DvmType loop_guess_index_type_(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    DvmhLoop *loop = sloop->getLoop();
    assert(loop);
    int dev = sloop->getDeviceNum();
    int sizeNeeded = 0;
    int maxPossibleSize = sizeof(long long);
    DvmhRegion *region = loop->region;
    if (region) {
        if (region->usesDevice(dev)) {
            for (std::map<DvmhData *, DvmhRegionData *>::iterator it = region->getDatas()->begin(); it != region->getDatas()->end(); it++) {
                DvmhData *data = it->first;
                DvmhRegionData *rdata = it->second;
                if (rdata->getLocalPart(dev) || data->getRank() == 0) {
#ifdef NON_CONST_AUTOS
                    Interval exBlock[data->getRank()];
#else
                    Interval exBlock[MAX_ARRAY_RANK];
#endif
                    data->extendBlock(rdata->getLocalPart(dev), exBlock);
                    assert(data->getRepr(dev));
                    sizeNeeded = std::max(sizeNeeded, data->getBuffer(dev)->getMinIndexTypeSize(exBlock, true));
                    if (sizeNeeded >= maxPossibleSize)
                        break;
                }
            }
        }
    } else {
        for (std::set<DvmhObject *>::iterator it = allObjects.begin(); it != allObjects.end(); it++) {
            DvmhData *data = (*it)->as_s<DvmhData>();
            if (data && data->getRepr(dev)) {
                DvmhBuffer *buf = data->getBuffer(dev);
                sizeNeeded = std::max(sizeNeeded, buf->getMinIndexTypeSize(buf->getHavePortion(), true));
                if (sizeNeeded >= maxPossibleSize)
                    break;
            }
        }
    }
    int res;
    if (sizeNeeded <= (int)sizeof(int)) {
        res = rt_INT;
    } else if (sizeNeeded <= (int)sizeof(long)) {
        res = rt_LONG;
    } else if (sizeNeeded <= (int)sizeof(long long)) {
        res = rt_LLONG;
    } else {
        dvmh_log(WARNING, "There is a danger to not to be able to address arrays correctly.");
        res = rt_LLONG;
    }
    dvmh_log(TRACE, "Guessed index type for the loop on device #%d is %s", dev,
            (res == rt_INT ? "int" : (res == rt_LONG ? "long" : (res == rt_LLONG ? "long long" : "unknown"))));
    return res;
}

extern "C" void loop_cuda_register_red(DvmType *InDvmhLoop, DvmType InRedNum, void **ArrayPtr, void **LocPtr) {
    checkInternal(InDvmhLoop && ArrayPtr);
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    checkInternal2(cloop, "NULL loop reference");
    DvmhLoop *loop = cloop->getLoop();
    assert(loop);
    checkInternal3(InRedNum >= 1 && InRedNum <= (int)loop->reductions.size(), "Invalid reduction operation number (%d)", (int)InRedNum);
    DvmhReductionCuda *reduction = new DvmhReductionCuda(loop->reductions[InRedNum - 1], ArrayPtr, LocPtr, cloop->getDeviceNum());
    cloop->addReduction(reduction);
    checkInternal(cloop->reductions.size() <= loop->reductions.size());
    dvmh_log(TRACE, "Reduction #%d registered for CUDA", (int)InRedNum);
}

extern "C" void loop_cuda_red_init(DvmType *InDvmhLoop, DvmType InRedNum, void **devArrayPtr, void **devLocPtr) {
    checkInternal(InDvmhLoop);
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    checkInternal2(cloop, "NULL loop reference");
    checkInternal3(InRedNum >= 1 && InRedNum <= (int)cloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)InRedNum);
    DvmhReduction *red = cloop->getLoop()->reductions[InRedNum - 1];
    assert(red);
    char *arrayPtr = 0;
    if (devArrayPtr)
        arrayPtr = new char[red->elemSize * red->elemCount];
    char *locPtr = 0;
    if (devLocPtr && red->isLoc())
        locPtr = new char[red->locSize * red->elemCount];
    red->initValues(arrayPtr, locPtr);
    checkInternal(devices[cloop->getDeviceNum()]->getType() == dtCuda);
    CudaDevice *cudaDev = (CudaDevice *)devices[cloop->getDeviceNum()];
    if (devArrayPtr) {
        *devArrayPtr = cudaDev->allocBytes(red->elemSize * red->elemCount);
        cudaDev->setValue(*devArrayPtr, arrayPtr, red->elemSize * red->elemCount);
        cloop->addToToFree(*devArrayPtr);
    }
    if (devLocPtr) {
        if (locPtr) {
            *devLocPtr = cudaDev->allocBytes(red->locSize * red->elemCount);
            cudaDev->setValue(*devLocPtr, locPtr, red->locSize * red->elemCount);
            cloop->addToToFree(*devLocPtr);
        } else
            *devLocPtr = 0;
    }
    delete[] locPtr;
    delete[] arrayPtr;
}

extern "C" void loop_cuda_red_prepare(DvmType *InDvmhLoop, DvmType InRedNum, DvmType InCount, DvmType InFillFlag) {
    checkInternal(InDvmhLoop);
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    checkInternal2(cloop, "NULL loop reference");
    checkInternal3(InRedNum >= 1 && InRedNum <= (int)cloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)InRedNum);
    DvmhReduction *red = cloop->getLoop()->reductions[InRedNum - 1];
    assert(red);
    DvmhReductionCuda *cudaRed = cloop->getCudaRed(red);
    checkInternal2(cudaRed, "This reduction wasn't registered for CUDA execution");
    checkInternal3(InCount > 0, "Buffer length must be positive. Given " DTFMT, InCount);
    cudaRed->prepare(InCount, (InFillFlag ? true : false));
}

extern "C" void *loop_cuda_get_local_part(DvmType *InDvmhLoop, DvmType dvmDesc[], DvmType indexType) {
    checkInternal(InDvmhLoop && dvmDesc);
    DvmhData *data = getDvmh<DvmhData>(dvmDesc[0]);
    checkInternal2(data, "Array must be registered");
    if (indexType == rt_INT)
        return ((DvmhLoopCuda *)*InDvmhLoop)->getLocalPart<int>(data);
    else if (indexType == rt_LONG)
        return ((DvmhLoopCuda *)*InDvmhLoop)->getLocalPart<long>(data);
    else if (indexType == rt_LLONG)
        return ((DvmhLoopCuda *)*InDvmhLoop)->getLocalPart<long long>(data);
    else
        checkInternal2(false, "Index type could be rt_INT, rt_LONG or rt_LLONG");
    return 0;
}

extern "C" DvmType loop_cuda_autotransform(DvmType *InDvmhLoop, DvmType dvmDesc[]) {
    checkInternal(InDvmhLoop && dvmDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    checkInternal2(data, "Array must be registered");
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    checkInternal2(cloop, "NULL loop reference");
    return autotransformInternal(cloop, data);
}

extern "C" void loop_cuda_get_config(DvmType *InDvmhLoop, DvmType InSharedPerThread, DvmType InRegsPerThread, void *aInOutThreads, void *aOutStream,
        DvmType *OutSharedPerBlock) {
    dim3 *InOutThreads = (dim3 *)aInOutThreads;
    cudaStream_t *OutStream = (cudaStream_t *)aOutStream;
    checkInternal(InDvmhLoop);
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    int block[3] = {0, 0, 0};
    if (InOutThreads && InOutThreads->x >= 1 && InOutThreads->y >= 1 && InOutThreads->z >= 1) {
        // Using outside-specified default value
        block[0] = InOutThreads->x;
        block[1] = InOutThreads->y;
        block[2] = InOutThreads->z;
    }
    int sharedPerThread, regsPerThread;
    if (InSharedPerThread >= 0)
        sharedPerThread = InSharedPerThread;
    else
        sharedPerThread = 0;
    if (InRegsPerThread > 0)
        regsPerThread = InRegsPerThread;
    else {
        // XXX: Maybe too conservative assumption
        regsPerThread = ((CudaDevice *)devices[cloop->getDeviceNum()])->maxRegsPerThread;
    }
    cloop->pickBlock(sharedPerThread, regsPerThread, block);
    if (InOutThreads) {
        InOutThreads->x = block[0];
        InOutThreads->y = block[1];
        InOutThreads->z = block[2];
        dvmh_log(TRACE, "Using CUDA block (%d, %d, %d)", block[0], block[1], block[2]);
    }
    if (OutStream)
        *OutStream = cloop->cudaStream;
    if (OutSharedPerBlock)
        *OutSharedPerBlock = block[0] * block[1] * block[2] * sharedPerThread;
    cloop->counter = 0;
    cloop->dynSharedPerBlock = block[0] * block[1] * block[2] * sharedPerThread;
}

extern "C" DvmType loop_cuda_do(DvmType *InDvmhLoop, void *aOutBlocks, void **InOutBlocksInfo, DvmType indexType) {
    static double dvmhGPUCalcTime = 0;
#ifdef HAVE_CUDA
    dim3 *OutBlocks = (dim3 *)aOutBlocks;
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    DvmhLoop *loop = cloop->getLoop();
    DvmhLoopPortion *portion = cloop->getPortion();
    CudaDevice *device = (CudaDevice *)devices[cloop->getDeviceNum()];

    if (cloop->counter < 0)
        loop_cuda_get_config(InDvmhLoop, 0, 0, 0, 0, 0);
    int block[3];
    ((CudaHandlerOptimizationParams *)portion->getOptParams())->getBlock(block);
    int warpsPerBlock = divUpU(block[0] * block[1] * block[2], device->warpSize);

    if (dvmhSettings.alwaysSync && cloop->counter > 0) {
        checkErrorCuda(cudaDeviceSynchronize()); // Catch errors of CUDA handler's kernels
        checkErrorCuda(cudaGetLastError()); // Catch errors of CUDA handler's kernels
    }
    if (cloop->counter == 0) {
        UDvmType resBlocks = 1;
        if (loop->rank > 0) {
            checkInternal(InOutBlocksInfo);
            if (indexType == rt_INT) {
                int *deviceBlocksInfo;
                dvmhCudaGetDistribution(cloop->getDeviceNum(), loop->rank, cloop->getPortion()->getLoopBounds(), block, &resBlocks, &deviceBlocksInfo);
                *InOutBlocksInfo = deviceBlocksInfo;
            } else if (indexType == rt_LONG) {
                long *deviceBlocksInfo;
                dvmhCudaGetDistribution(cloop->getDeviceNum(), loop->rank, cloop->getPortion()->getLoopBounds(), block, &resBlocks, &deviceBlocksInfo);
                *InOutBlocksInfo = deviceBlocksInfo;
            } else if (indexType == rt_LLONG) {
                long long *deviceBlocksInfo;
                dvmhCudaGetDistribution(cloop->getDeviceNum(), loop->rank, cloop->getPortion()->getLoopBounds(), block, &resBlocks, &deviceBlocksInfo);
                *InOutBlocksInfo = deviceBlocksInfo;
            } else
                checkInternal2(false, "Index type could be rt_INT, rt_LONG or rt_LLONG");
        }
        UDvmType resWarps = resBlocks * warpsPerBlock;
        for (int i = 0; i < (int)cloop->reductions.size(); i++) {
            assert(cloop->reductions[i]);
            // TODO: Do not fill it when it becomes possible to do
            cloop->reductions[i]->prepare(resWarps, true);
        }
        cloop->overallBlocks = resBlocks;
        cloop->restBlocks = resBlocks;
        cloop->latestBlocks = 0;
        cloop->counter++;
        if (needToCollectTimes) {
            device->deviceSynchronize();
            portion->setCalcTime(dvmhTime());
        }
    }
    if (cloop->counter == 1) {
        if (cloop->restBlocks <= 0) {
            cloop->counter++;
        } else {
            UDvmType maxBlocks = device->maxGridSize[0];
            UDvmType toExec = cloop->restBlocks <= maxBlocks ? cloop->restBlocks : (cloop->restBlocks / 2 <= maxBlocks ? cloop->restBlocks / 2 : maxBlocks);
            if (loop->rank > 0) {
                checkInternal(InOutBlocksInfo);
                *InOutBlocksInfo = (char *)(*InOutBlocksInfo) + (indexType == rt_INT ? sizeof(int) : indexType == rt_LONG ? sizeof(long) : sizeof(long long)) *
                        cloop->latestBlocks * loop->rank * 2;
            }
            for (int i = 0; i < (int)cloop->reductions.size(); i++) {
                UDvmType itemsDone = cloop->latestBlocks * warpsPerBlock;
                cloop->reductions[i]->advancePtrsBy(itemsDone);
            }
            checkInternal(OutBlocks);
            OutBlocks->x = toExec;
            OutBlocks->y = 1;
            OutBlocks->z = 1;
            cloop->latestBlocks = toExec;
            cloop->restBlocks -= toExec;
            dvmh_log(TRACE, "loop_cuda_do block=(%d,%d,%d) grid=(%d,%d,%d)", block[0], block[1], block[2], OutBlocks->x, OutBlocks->y, OutBlocks->z);
            return 1;
        }
    }
    if (cloop->counter == 2) {
        if (loop->rank > 0) {
            checkInternal(InOutBlocksInfo);
            *InOutBlocksInfo = 0;
        }
        cloop->finishAllReds();
        if (needToCollectTimes) {
            device->deviceSynchronize();
            portion->setCalcTime(dvmhTime() - portion->getCalcTime());
            dvmhGPUCalcTime += portion->getCalcTime();
            dvmh_log(TRACE, "Calculation time now = %g. Overall = %g", portion->getCalcTime(), dvmhGPUCalcTime);
        }
        return 0;
    }
    checkInternal2(false, "Internal inconsistency");
    return 0;
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
    return 0;
#endif
}

extern "C" DvmType loop_cuda_get_red_step(DvmType *InDvmhLoop) {
    checkInternal(InDvmhLoop);
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    checkInternal2(cloop, "NULL loop reference");
    DvmType res = 0;
    for (int i = 0; i < (int)cloop->reductions.size(); i++) {
        if (!cloop->reductions[i]->getReduction()->isScalar()) {
            if (res == 0)
                res = cloop->reductions[i]->getItemCount();
            else
                checkInternal2(res == cloop->reductions[i]->getItemCount(), "Reduction step inconsistency encountered");
        }
    }
    return res;
}

extern "C" void loop_red_finish(DvmType *InDvmhLoop, DvmType InRedNum) {
    checkInternal(InDvmhLoop);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    if (devices[sloop->getDeviceNum()]->getType() == dtCuda) {
        DvmhLoopCuda *cloop = (DvmhLoopCuda *)sloop;
        checkInternal3(InRedNum >= 1 && InRedNum <= (int)sloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)InRedNum);
        DvmhReduction *reduction = sloop->getLoop()->reductions[InRedNum - 1];
        assert(reduction);
        cloop->finishRed(reduction);
    }
}

extern "C" void loop_red_post_(DvmType *InDvmhLoop, DvmType *InRedNumRef, void *arrayPtr, void *locPtr) {
    checkInternal(InDvmhLoop && InRedNumRef && arrayPtr);
    DvmhSpecLoop *sloop = (DvmhSpecLoop *)*InDvmhLoop;
    checkInternal2(sloop, "NULL loop reference");
    int InRedNum = *InRedNumRef;
    checkInternal3(InRedNum >= 1 && InRedNum <= (int)sloop->getLoop()->reductions.size(), "Invalid reduction operation number (%d)", (int)InRedNum);
    DvmhReduction *reduction = sloop->getLoop()->reductions[InRedNum - 1];
    if (sloop->getPortion()->getDoReduction())
        reduction->postValues(arrayPtr, locPtr);
}

static void fillAxisPerm(int rank, DvmType depMask, DvmType *idxPerm, int depCount) {
    // idxPerm - permutation of loop axes. Numeration from zero from innermost. Permutated => original
    int count = 0;
    int h = 0;
    int hd = depCount;
    for (int i = 0; i < rank; i++) {
        if ((depMask >> i) & 1) {
            count++;
            idxPerm[h] = i;
            h++;
        } else {
            idxPerm[hd] = i;
            hd++;
        }
    }
    checkInternal3(count == depCount, "Detected number of dependencies does not live up to expected number of dependencies. Expected %d, detected %d. Mask "
            DTFMT ".", depCount, count, depMask);
}

template <typename T>
static void applyPerm(int rank, T *vals, DvmType *idxPerm) {
#ifdef NON_CONST_AUTOS
    T tmp[rank];
#else
    T tmp[MAX_LOOP_RANK];
#endif
    for (int i = 1; i <= rank; i++)
        tmp[i - 1] = vals[rank - 1 - idxPerm[rank - 1 - (i - 1)]];
    typedMemcpy(vals, tmp, rank);
}

extern "C" void dvmh_change_filled_bounds(DvmType *boundsLow, DvmType *boundsHigh, DvmType *loopSteps, DvmType rank, DvmType depCount, DvmType depMask,
        DvmType *idxPerm) {
    fillAxisPerm(rank, depMask, idxPerm, depCount);
    if (depCount > 0 && depCount < rank) {
        applyPerm(rank, boundsLow, idxPerm);
        applyPerm(rank, boundsHigh, idxPerm);
        applyPerm(rank, loopSteps, idxPerm);
    }
}

// Additional stuff
extern "C" DvmType dvmh_register_array_(DvmType dvmDesc[]) {
    checkInternal(dvmDesc);
    DvmhData *data = findDataForDvm(dvmDesc[0]);
    if (!data) {
        data = getOrCreateTiedDvmh<DvmhData>(dvmDesc[0]);
        allObjects.insert(data);
        if (data->isAligned() && !data->isDistributed()) {
            void *addr = data->getBuffer(0)->getDeviceAddr();
            SpinLockGuard guard(regularVarsLock);
            RegularVar *regVar = dictFind2(regularVars, addr);
            if (regVar) {
                checkInternal(regVar->getDataRegionDepth() == 1);
            } else {
                regVar = new RegularVar(true, 0);
                regularVars[addr] = regVar;
            }
            regVar->setData(data);
        }
    }
    assert(data != 0);
    return (DvmType)data;
}
extern "C" DvmType dvmh_register_scalar_(void *addr, DvmType *sizeRef) {
    checkInternal(sizeRef);
    checkError2(addr, "Can not register NULL pointer");
    RegularVar *regVar = 0;
    DvmhData *data = findDataForAddr(addr, &regVar);
    if (!data) {
        data = new DvmhData(*sizeRef, DvmhData::ttUnknown, 0, 0);
        data->realign(0);
        data->setHostPortion(addr, 0);
        data->initActual(0);
        if (regVar) {
            checkInternal(regVar->getDataRegionDepth() == 1);
        } else {
            regVar = new RegularVar(true, 0);
            {
                SpinLockGuard guard(regularVarsLock);
                regularVars[addr] = regVar;
            }
        }
        regVar->setData(data);
        allObjects.insert(data);
    }
    assert(data != 0);
    return (DvmType)data;
}

extern "C" DvmType loop_cuda_get_device_prop(DvmType *InDvmhLoop, DvmType prop) {
    DvmhLoopCuda *cloop = (DvmhLoopCuda *)*InDvmhLoop;
    if (prop == CUDA_MAX_GRID_X)
        return cloop->cudaDev->maxGridSize[0];
    else if (prop == CUDA_MAX_GRID_Y)
        return cloop->cudaDev->maxGridSize[1];
    else if (prop == CUDA_MAX_GRID_Z)
        return cloop->cudaDev->maxGridSize[2];
    else
        return -1;
}

static std::map<SourcePosition, DvmhPredictor*> loopLastInfo;
struct StagePerf {
    StagePerf() { stage = 0; time = 0.; }

    int stage;
    double time;
};


static void maxStagePerf(StagePerf *in, StagePerf *inout, int *len, MPI_Datatype *dptr) {
    if (in->time > inout->time) {
        inout->time = in->time;
        inout->stage = in->stage;
    }
}

extern "C" DvmType dvmh_get_next_stage_(DvmType *dvmhRegRef, LoopRef *loopRef, DvmType *lineNumber, const char *fileName, int len) {
    if (DVM_VMS->ProcCount == 1) // if only one proc
        return 0;

    SourcePosition sp(fileName, lineNumber[0]);
    DvmhLoopPersistentInfo *currLoopInfo = dictFind2(loopDict, sp);
    DvmhPredictor *currLastInfo = NULL;
    if (loopLastInfo.find(sp) != loopLastInfo.end()) {
        currLastInfo = loopLastInfo[sp];
    } else {
        // first iteration
        dvmh_log(TRACE, "creating predictor...");
        DvmhPredictor *tmp = new DvmhPredictor();
        tmp->addStage(currentMPS->getCommSize());
        loopLastInfo[sp] = tmp;

        return currentMPS->getCommSize();
    }

    if (currLastInfo->isGlobalStable() == true) {
        // nothing changes
        return currLastInfo->getBestStage();
    } else {
        int lastStage = currLastInfo->getLastStage();
        double lastTime = currLoopInfo->estimateTime(currentRegion, lastStage);

        StagePerf sp, maxSP;
        sp.stage = lastStage;
        sp.time = lastTime;
        maxSP.stage = lastStage;
        maxSP.time = lastTime;
        // TODO: It is a standard MAXLOC operation, why not use an already existing implementation from mps.h?
        MPI_Op op;
        MPI_Datatype dt;
        // TODO: Pretty fragile assumption on struct StagePerf padding
        MPI_Type_contiguous(2, MPI_DOUBLE, &dt);
        MPI_Type_commit(&dt);
        MPI_Op_create((MPI_User_function*)maxStagePerf, 1, &op);
        MPI_Allreduce(&sp, &maxSP, 1, dt, op, currentMPS->getComm());
        MPI_Op_free(&op);
        MPI_Type_free(&dt);

        currLastInfo->addTime(maxSP.stage, maxSP.time);

        int stable = currLastInfo->isStable() ? 1 : 0;
        int globalStable = stable;
        if (!currLastInfo->isStable()) {
            int newStage = currLastInfo->predictStage();
            currLastInfo->addStage(newStage);

        }

        currentMPS->allreduce(globalStable, rf_MIN);

        if (globalStable)
            currLastInfo->setGlobalStable(true);

        return currLastInfo->getLastStage();
    }
}
