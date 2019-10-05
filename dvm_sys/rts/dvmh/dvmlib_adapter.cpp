#ifndef NO_DVM

#include "dvmlib_adapter.h"

#include <cassert>

#include "distrib.h"
#include "dvmh_async.h"
#include "dvmh_log.h"
#include "loop.h"
#include "mps.h"

namespace libdvmh {

struct DvmObjectDesc {
    DvmSysHandle handle;
    bool isMain;
    DvmObjectDesc(): isMain(false) {}
};

static std::map<DvmhObject *, DvmObjectDesc> objMap;
static std::map<DvmSysHandle, DvmhObject *> reverseObjMap;
static DvmhSpinLock mapsLock;

SysHandle *getDvm(DvmhObject *obj, bool *pDvmIsMain) {
    SpinLockGuard guard(mapsLock);
    std::map<DvmhObject *, DvmObjectDesc>::iterator it = objMap.find(obj);
    if (it != objMap.end()) {
        if (pDvmIsMain)
            *pDvmIsMain = it->second.isMain;
        return it->second.handle;
    } else {
        return 0;
    }
}

DvmhObject *getDvmh(DvmSysHandle handle) {
    SpinLockGuard guard(mapsLock);
    std::map<DvmSysHandle, DvmhObject *>::iterator it = reverseObjMap.find(handle);
    if (it != reverseObjMap.end())
        return it->second;
    else
        return 0;
}

void tieDvm(DvmhObject *obj, DvmSysHandle handle, bool dvmIsMain) {
    assert(obj);
    assert(handle);
    SpinLockGuard guard(mapsLock);
    DvmObjectDesc &desc = objMap[obj];
    desc.handle = handle;
    desc.isMain = dvmIsMain;
    reverseObjMap[handle] = obj;
}

void untieDvm(DvmhObject *obj) {
    SpinLockGuard guard(mapsLock);
    std::map<DvmhObject *, DvmObjectDesc>::iterator it = objMap.find(obj);
    if (it != objMap.end()) {
        reverseObjMap.erase(it->second.handle);
        objMap.erase(it);
    }
}

UDvmType getDvmMapSize() {
    SpinLockGuard guard(mapsLock);
    return objMap.size();
}


void *getAddrFromDvm(s_DISARRAY *dvmHead) {
    assert(dvmHead != 0);
    dvmh_log(DONT_LOG, "DVM-array is allocated on %p (" DTFMT ")", dvmHead->ArrBlock.ALoc.Ptr, (DvmType)dvmHead->ArrBlock.ALoc.Ptr);
    return dvmHead->ArrBlock.ALoc.Ptr;
}

void *getAddrFromDvmDesc(DvmType dvmDesc[]) {
    s_DISARRAY *dvmHead = (s_DISARRAY *)((SysHandle *)dvmDesc[0])->pP;
    return getAddrFromDvm(dvmHead);
}

class DvmhObjectForgetter: public Executable {
public:
    virtual void execute() { assert(false); }
    virtual void execute(void *par) {
        DvmhObject *obj = (DvmhObject *)par;
        untieDvm(obj);
    }
};

static MPI_Comm extractMpiComm(s_VMS *vms) {
    while (vms && !vms->Is_MPI_COMM)
        vms = vms->PHandlePtr ? (s_VMS *)vms->PHandlePtr->pP : 0;
    return vms ? vms->PS_MPI_COMM : MPI_COMM_NULL;
}

MultiprocessorSystem *createMpsFromVMS(s_VMS *vms, MultiprocessorSystem *parent) {
    int rank = vms->Space.Rank;
#ifdef NON_CONST_AUTOS
    int sizes[rank];
#else
    int sizes[MAX_MPS_RANK];
#endif
    for (int i = 0; i < rank; i++)
        sizes[i] = vms->Space.Size[i];
    for (int i = 0; i < rank; i++)
        dvmh_log(TRACE, "Mirrored MPS sizes[%d]=%d", i, sizes[i]);
    dvmh_log(TRACE, "Is_MPI_COMM=%d", vms->Is_MPI_COMM);
    MultiprocessorSystem *res = new MultiprocessorSystem(extractMpiComm(vms), vms->Space.Rank, sizes);
    if (parent)
        parent->attachChildMPS(res);
    res->addOnDeleteHook(new DvmhObjectForgetter);
    return res;
}

static Interval extractLocalPartFromAMView(s_AMVIEW *AMV, int AMVAxis, int VMSAxis, double DisPar, int Coord) {
    int i = VMSAxis;
    s_VMS *VMS = AMV->VMS;
    DvmType CLower, CUpper;
    s_SPACE *VMSpace = &VMS->Space;
    s_SPACE *AMVSpace = &AMV->Space;
    DvmType AMVDSize = AMVSpace->Size[AMVAxis];
    if (VMS == AMV->WeightVMS) {
        if (AMV->GenBlockCoordWeight[i])
            CLower = AMV->PrevSumGenBlockCoordWeight[i][Coord];
        else
            CLower = roundDownS((DvmType)(DisPar * AMV->PrevSumCoordWeight[i][Coord]), AMV->Div[AMVAxis]);
    } else {
        CLower = roundDownS((DvmType)(DisPar * VMS->PrevSumCoordWeight[i][Coord]), AMV->Div[AMVAxis]);
    }

    if (Coord == VMSpace->Size[i] - 1) {
       CUpper = AMVDSize - 1;
    } else {
        if (VMS == AMV->WeightVMS) {
            if (AMV->GenBlockCoordWeight[i])
                CUpper = CLower + AMV->GenBlockCoordWeight[i][Coord] - 1;
            else
                CUpper = std::min(AMVDSize, roundDownS((DvmType)(DisPar * AMV->PrevSumCoordWeight[i][Coord + 1]), AMV->Div[AMVAxis])) - 1;
        } else {
            CUpper = std::min(AMVDSize, roundDownS((DvmType)(DisPar * VMS->PrevSumCoordWeight[i][Coord + 1]), AMV->Div[AMVAxis])) - 1;
        }
    }
    return Interval::create(CLower, CUpper);
}

DvmhDistribSpace *createDistribSpaceFromAMView(s_AMVIEW *amView, MultiprocessorSystem *mps) {
    checkInternal(amView);
    int dspaceRank = amView->Space.Rank;
#ifdef NON_CONST_AUTOS
    Interval space[dspaceRank], localPart[dspaceRank];
#else
    Interval space[MAX_DISTRIB_SPACE_RANK], localPart[MAX_DISTRIB_SPACE_RANK];
#endif
    for (int i = 0; i < dspaceRank; i++) {
        space[i][0] = 0;
        space[i][1] = amView->Space.Size[i] - 1;
        localPart[i][0] = amView->Local.Set[i].Lower;
        localPart[i][1] = amView->Local.Set[i].Upper;
        dvmh_log(DEBUG, "dspaceLow[%d] = " DTFMT " dspaceHigh[%d] = " DTFMT " localPart[%d] = " DTFMT ".." DTFMT, i, space[i][0], i, space[i][1], i,
                localPart[i][0], localPart[i][1]);
    }
    bool hasLocal = amView->HasLocal;
    DvmhDistribSpace *res = new DvmhDistribSpace(dspaceRank, space);
    DvmhDistribRule *rule = new DvmhDistribRule(dspaceRank, mps);
    for (int i = 0; i < dspaceRank; i++) {
        DvmhAxisDistribRule *axRule = 0;
        if (amView->DISTMAP[i].Attr == map_BLOCK) {
            int mpsAxis = amView->DISTMAP[i].PAxis;
            int procCount = mps->getAxis(mpsAxis).procCount;
            assert(procCount == amView->VMS->Space.Size[mpsAxis - 1]);
            UDvmType *genBlock = new UDvmType[procCount];
            for (int p = 0; p < procCount; p++)
                genBlock[p] = extractLocalPartFromAMView(amView, i, mpsAxis - 1, amView->DISTMAP[i].DisPar, p).size();
            for (int p = 0; p < procCount; p++)
                dvmh_log(TRACE, "Array genBlock[%d]=" UDTFMT, p, genBlock[p]);
            DvmhData *gblData = DvmhData::fromRegularArray((sizeof(UDvmType) == sizeof(long) ? DvmhData::dtULong : DvmhData::dtULongLong), genBlock, procCount);
            axRule = DvmhAxisDistribRule::createGenBlock(mps, mpsAxis, res->getAxisSpace(i + 1), gblData);
            delete gblData;
            if (!amView->GenBlockCoordWeight[i]) {
                if (amView->Div[i] != 1)
                    axRule->asBlockDistributed()->changeMultQuant(amView->Div[i]);
                axRule->asBlockDistributed()->changeTypeToBlock();
            }
        } else {
            axRule = DvmhAxisDistribRule::createReplicated(mps, res->getAxisSpace(i + 1));
        }
        assert(axRule);
        rule->setAxisRule(i + 1, axRule);
    }
    res->redistribute(rule);
    dvmh_log(TRACE, "resultHasLocal = %d, sourceHasLocal = %d", (int)res->hasLocal(), (int)hasLocal);
    assert(res->hasLocal() == hasLocal);
    if (hasLocal) {
        for (int i = 0; i < dspaceRank; i++) {
            dvmh_log(TRACE, "resultLocalPart[%d] = " DTFMT ".." DTFMT, i, res->getAxisLocalPart(i + 1)[0], res->getAxisLocalPart(i + 1)[1]);
            assert(localPart[i] == res->getAxisLocalPart(i + 1));
        }
    }
    res->addOnDeleteHook(new DvmhObjectForgetter);
    return res;
}

static void translateAlign(s_ALIGN curAl, const Interval &curSpace, DvmType starti, DvmhAxisAlignRule *pMyAlign) {
    // XXX: Assumes zero-based DistribSpace.
    dvmh_log(TRACE, "imported params: Attr=%d Axis=%d A=" DTFMT " B=" DTFMT " Bound=" DTFMT " TAxis=%d", curAl.Attr, curAl.Axis, curAl.A, curAl.B, curAl.Bound,
            curAl.TAxis);
    if (curAl.Attr == align_REPLICATE) {
        pMyAlign->setReplicated(1, curSpace);
    } else if (curAl.Attr == align_BOUNDREPL) {
        if (curAl.A > 0)
            pMyAlign->setReplicated(curAl.A, Interval::create(curAl.B, curAl.B + curAl.A * (curAl.Bound - 1)));
        else
            pMyAlign->setReplicated(-curAl.A, Interval::create(curAl.B + curAl.A * (curAl.Bound - 1), curAl.B));
    } else if (curAl.Attr == align_CONSTANT) {
        pMyAlign->setConstant(curAl.B);
    } else {
        pMyAlign->setLinear(curAl.Axis, curAl.A, curAl.B - starti * curAl.A);
    }
    dvmh_log(TRACE, "starti=" DTFMT " mapping Axis=%d Multiplier=" DTFMT " Summand=" DTFMT " ReplicInterval=" DTFMT ".." DTFMT, starti, pMyAlign->axisNumber,
            pMyAlign->multiplier, pMyAlign->summand, pMyAlign->replicInterval[0], pMyAlign->replicInterval[1]);
}

DvmhData *createDataFromDvmArray(s_DISARRAY *dvmHead, DvmhDistribSpace *dspace, DvmhData::TypeType givenTypeType) {
    DvmhData *data;
    checkInternal(dvmHead != NULL);
    DvmType *dvmheader = (DvmType *)dvmHead->HandlePtr->HeaderPtr;
    checkInternal(dvmheader != NULL);
    if (!dvmHead->HasLocal)
        dvmHead->ArrBlock.ALoc.Ptr = 0;
    int rank = dvmHead->Space.Rank;
#ifdef NON_CONST_AUTOS
    Interval space[rank], localPart[rank], hostPortion[rank];
    ShdWidth shdWidths[rank];
#else
    Interval space[MAX_ARRAY_RANK], localPart[MAX_ARRAY_RANK], hostPortion[MAX_ARRAY_RANK];
    ShdWidth shdWidths[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        DvmType starti = dvmheader[rank + (rank - 1 - i) + 2];
        if (dvmHead->RegBufSign && dvmHead->HasLocal) {
            dvmh_log(DEBUG, "Using local part (if any) as space for regular buffer array");
            space[i][0] = dvmHead->Block.Set[i].Lower + starti;
            space[i][1] = dvmHead->Block.Set[i].Upper + starti;
        } else {
            space[i][0] = starti;
            space[i][1] = space[i][0] + dvmHead->Space.Size[i] - 1;
        }

        localPart[i][0] = dvmHead->Block.Set[i].Lower + starti;
        localPart[i][1] = dvmHead->Block.Set[i].Upper + starti;

        shdWidths[i][0] = dvmHead->InitLowShdWidth[i];
        shdWidths[i][1] = dvmHead->InitHighShdWidth[i];

        hostPortion[i][0] = dvmHead->ArrBlock.Block.Set[i].Lower + starti;
        hostPortion[i][1] = dvmHead->ArrBlock.Block.Set[i].Upper + starti;
        dvmh_log(DEBUG, "[%d] space " DTFMT ".." DTFMT "; localPart " DTFMT ".." DTFMT "; shdWidths " DTFMT "," DTFMT "; hostPortion " DTFMT ".." DTFMT "", i,
                space[i][0], space[i][1], localPart[i][0], localPart[i][1], shdWidths[i][0], shdWidths[i][1], hostPortion[i][0], hostPortion[i][1]);
    }
    if (dvmHead->Type >= 0)
        data = new DvmhData((DvmhData::DataType)dvmHead->Type, rank, space, shdWidths);
    else
        data = new DvmhData(dvmHead->TLen, givenTypeType, rank, space, shdWidths);
    DvmhAlignRule *alignRule = 0;
    if (dspace != 0) {
#ifdef NON_CONST_AUTOS
        DvmhAxisAlignRule axisRules[dspace->getRank()];
#else
        DvmhAxisAlignRule axisRules[MAX_DISTRIB_SPACE_RANK];
#endif
        for (int i = 0; i < dspace->getRank(); i++) {
            s_ALIGN curAl = dvmHead->Align[data->getRank() + i];
            DvmType starti = curAl.Axis > 0 ? space[curAl.Axis - 1][0] : 0;
            translateAlign(curAl, dspace->getAxisSpace(i + 1), starti, &axisRules[i]);
        }
        alignRule = new DvmhAlignRule(rank, dspace, axisRules);
    } else {
        alignRule = 0;
    }
    if (dvmHead->RegBufSign && !dvmHead->HasLocal)
        assert(!data->hasLocal());
    else
        data->realign(alignRule);
    assert(data->hasLocal() == (dvmHead->HasLocal != 0));
    if (data->hasLocal()) {
        for (int i = 0; i < rank; i++) {
            dvmh_log(TRACE, "data->localPart[%d]=" DTFMT ".." DTFMT, i, data->getAxisLocalPart(i + 1)[0], data->getAxisLocalPart(i + 1)[1]);
            dvmh_log(TRACE, "DVM's localPart[%d]=" DTFMT ".." DTFMT, i, localPart[i][0], localPart[i][1]);
        }
        for (int i = 0; i < rank; i++)
            assert(data->getAxisLocalPart(i + 1)[0] == localPart[i][0] && data->getAxisLocalPart(i + 1)[1] == localPart[i][1]);
    }
    dvmh_log(DEBUG, "hasLocal = %d typeSize = " DTFMT " typeType = %d DVM-type = %d", (int)data->hasLocal(), data->getTypeSize(), (int)data->getTypeType(),
            dvmHead->Type);
    if (data->hasLocal()) {
        data->setHostPortion(getAddrFromDvm(dvmHead), hostPortion);
        data->initActual(0);
    }
    data->addOnDeleteHook(new DvmhObjectForgetter);

    return data;
}

DvmhLoop *createLoopFromDvmLoop(s_PARLOOP *dvmLoop, DvmhDistribSpace *dspace) {
    DvmhLoop *loop;
    int rank = dvmLoop->Rank;
#ifdef NON_CONST_AUTOS
    LoopBounds loopBounds[rank];
#else
    LoopBounds loopBounds[MAX_LOOP_RANK];
#endif
    for (int i = 0; i < rank; i++) {
        loopBounds[i] = LoopBounds::create(Interval::create(dvmLoop->Set[i].Lower, dvmLoop->Set[i].Upper),
                dvmLoop->Set[i].Step * (dvmLoop->Invers[i] ? -1 : 1));
        dvmh_log(TRACE, "Loop [%d]: " DTFMT " " DTFMT " " DTFMT, i, loopBounds[i][0], loopBounds[i][1], loopBounds[i][2]);
    }
    assert(dspace != 0);
#ifdef NON_CONST_AUTOS
    DvmhAxisAlignRule axisRules[dspace->getRank()];
#else
    DvmhAxisAlignRule axisRules[MAX_DISTRIB_SPACE_RANK];
#endif
    for (int i = 0; i < dspace->getRank(); i++) {
        s_ALIGN curAl = dvmLoop->Align[rank + i];
        DvmType starti = curAl.Axis > 0 ? dvmLoop->InitIndex[curAl.Axis - 1] : 0;
        translateAlign(curAl, dspace->getAxisSpace(i + 1), starti, &axisRules[i]);
    }
    loop = new DvmhLoop(rank, loopBounds);
    loop->setAlignRule(new DvmhAlignRule(rank, dspace, axisRules));
    if (dvmLoop->AddBnd) {
        checkInternal(dvmLoop->TempDArr);
        loop->shadowComputeWidths = new ShdWidth[dspace->getRank()];
        for (int i = 0; i < dspace->getRank(); i++) {
            s_ALIGN curAl = dvmLoop->TempDArr->Align[dvmLoop->TempDArr->Space.Rank + i];
            DvmhAxisAlignRule axisRule;
            translateAlign(curAl, dspace->getAxisSpace(i + 1), 0, &axisRule);
            if (axisRule.axisNumber > 0) {
                ShdWidth w;
                if (dvmLoop->AddBnd == 1) {
                    w[0] = dvmLoop->TempDArr->InitLowShdWidth[axisRule.axisNumber - 1];
                    w[1] = dvmLoop->TempDArr->InitHighShdWidth[axisRule.axisNumber - 1];
                } else {
                    w[0] = dvmLoop->LowShd[axisRule.axisNumber - 1];
                    w[1] = dvmLoop->HighShd[axisRule.axisNumber - 1];
                }
                if (axisRule.multiplier > 0) {
                    loop->shadowComputeWidths[i][0] = w[0] * axisRule.multiplier;
                    loop->shadowComputeWidths[i][1] = w[1] * axisRule.multiplier;
                } else {
                    loop->shadowComputeWidths[i][0] = w[1] * (-axisRule.multiplier);
                    loop->shadowComputeWidths[i][1] = w[0] * (-axisRule.multiplier);
                }
            } else {
                loop->shadowComputeWidths[i][0] = 0;
                loop->shadowComputeWidths[i][1] = 0;
            }
        }
    }
    loop->addOnDeleteHook(new DvmhObjectForgetter);
    return loop;
}

bool isDvmArrayRegular(DvmSysHandle handle) {
    s_DISARRAY *dvmHead = (s_DISARRAY *)handle->pP;
    assert(dvmHead);
    return dvmHead->MemPtr != NULL;
}

bool isDvmObjectStatic(DvmSysHandle handle, bool defaultValue) {
    switch (handle->Type) {
        case sht_DisArray: return ((s_DISARRAY *)handle->pP)->Static;
        case sht_AMView: return ((s_AMVIEW *)handle->pP)->Static;
        default: return defaultValue;
    }
    return defaultValue;
}

DvmhObject *getOrCreateTiedDvmh(DvmSysHandle handle, std::vector<DvmhObject *> *createdObjects) {
    DvmhObject *res = getDvmh(handle);
    if (res) {
        return res;
    }
    assert(handle);
    if (handle->Type == sht_DisArray) {
        dvmh_log(TRACE, "Mirroring DISARRAY");
        bool isRegular = isDvmArrayRegular(handle);
        s_DISARRAY *dvmHead = (s_DISARRAY *)handle->pP;
        checkInternal(dvmHead != 0);
        DvmhDistribSpace *dspace = 0;
        if (!isRegular)
            dspace = getOrCreateTiedDvmh<DvmhDistribSpace>(dvmHead->AMView->HandlePtr, createdObjects);
        DvmhData *data = createDataFromDvmArray(dvmHead, dspace);
        if (createdObjects)
            createdObjects->push_back(data);
        if (!isRegular)
            tieDvm(data, handle, true);
        res = data;
    } else if (handle->Type == sht_AMView) {
        dvmh_log(TRACE, "Mirroring AMVIEW");
        s_AMVIEW *amView = (s_AMVIEW *)handle->pP;
        checkInternal(amView != 0);
        MultiprocessorSystem *mps = getOrCreateTiedDvmh<MultiprocessorSystem>(amView->VMS->HandlePtr, createdObjects);
        DvmhDistribSpace *dspace = createDistribSpaceFromAMView(amView, mps);
        if (createdObjects)
            createdObjects->push_back(dspace);
        tieDvm(dspace, handle, true);
        res = dspace;
    } else if (handle->Type == sht_VMS) {
        dvmh_log(TRACE, "Mirroring VMS");
        s_VMS *vms = (s_VMS *)handle->pP;
        checkInternal(vms != 0);
        MultiprocessorSystem *parent = 0;
        if (vms->PHandlePtr)
            parent = getOrCreateTiedDvmh<MultiprocessorSystem>(vms->PHandlePtr, createdObjects);
        MultiprocessorSystem *mps = createMpsFromVMS(vms, parent);
        if (createdObjects)
            createdObjects->push_back(mps);
        tieDvm(mps, handle, true);
        res = mps;
    } else if (handle->Type == sht_ParLoop) {
        dvmh_log(TRACE, "Mirroring PARLOOP");
        s_PARLOOP *dvmLoop = (s_PARLOOP *)handle->pP;
        DvmhData *templData = 0;
        if (dvmLoop->TempDArr)
            templData = getOrCreateTiedDvmh<DvmhData>(dvmLoop->TempDArr->HandlePtr, createdObjects);
        DvmhDistribSpace *dspace = getOrCreateTiedDvmh<DvmhDistribSpace>(dvmLoop->AMView->HandlePtr, createdObjects);
        assert(dspace);
        DvmhLoop *loop = createLoopFromDvmLoop(dvmLoop, dspace);
        if (createdObjects)
            createdObjects->push_back(loop);
        if (templData)
            loop->mappingData = templData;
        tieDvm(loop, handle, true);
        res = loop;
    }
    return res;
}

#endif

}
