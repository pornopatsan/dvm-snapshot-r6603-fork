#include "mps.h"

#include "loop.h"
#include "settings.h"

namespace libdvmh {

// MPSAxis

void MPSAxis::setCoordWeights(double wgts[], bool copyFlag) {
    if (copyFlag) {
        coordWeights = new double[procCount];
        typedMemcpy(coordWeights, wgts, procCount);
    } else {
        coordWeights = wgts;
    }
    double sum = 0;
    for (int i = 0; i < procCount; i++)
        sum += coordWeights[i];
    double coef = procCount / sum;
    for (int i = 0; i < procCount; i++)
        coordWeights[i] *= coef;
    weightedFlag = false;
    for (int i = 0; i < procCount; i++)
        if (fabs(coordWeights[i] - 1.0) > 1e-2)
            weightedFlag = true;
}

void MPSAxis::freeAxisComm() {
    if (ownComm)
        delete axisComm;
    axisComm = 0;
    ownComm = false;
}

// DvmhCommunicator

DvmhCommunicator::DvmhCommunicator(MPI_Comm aComm, int aRank, const int sizes[]) {
    rank = aRank;
    assert(rank >= 0);
    while (rank > 0 && sizes[rank - 1] == 1)
        rank--;
    axes = new MPSAxis[rank];
    comm = aComm;
    ownComm = false;
    if (comm == MPI_COMM_NULL) {
        commRank = -1;
    } else {
        int tmpInt;
        checkInternalMPI(MPI_Comm_rank(comm, &tmpInt));
        commRank = tmpInt;
    }
    commSize = 1;
    for (int i = 0; i < rank; i++) {
        assert(sizes[i] > 0);
        axes[i].procCount = sizes[i];
        commSize *= sizes[i];
    }
    if (commRank >= 0) {
        int tmpInt;
        checkInternalMPI(MPI_Comm_size(comm, &tmpInt));
        checkInternal(tmpInt == commSize);
    }
    int accumSize = 1;
    for (int i = 0; i < rank; i++) {
        int idx = rank - 1 - i;
        int pc = axes[idx].procCount;
        axes[idx].commStep = accumSize;
        if (commRank >= 0)
            axes[idx].ourProc = commRank / accumSize % pc;
        else
            axes[idx].ourProc = -1;
        accumSize *= pc;
    }
    if (commRank >= 0) {
        for (int i = 0; i < rank; i++) {
            if (sizes[i] == 1) {
                axes[i].axisComm = selfComm;
            } else if (sizes[i] == commSize) {
                axes[i].axisComm = this;
            } else {
                int color = getCommRankAxisOffset(i + 1, -axes[i].ourProc);
                MPI_Comm axisComm;
                checkInternalMPI(MPI_Comm_split(comm, color, commRank, &axisComm));
                axes[i].axisComm = new DvmhCommunicator(axisComm, 1, sizes + i);
                axes[i].axisComm->ownComm = true;
                axes[i].ownComm = true;
            }
        }
    }
    resetNextAsyncTag();
}

DvmhCommunicator::DvmhCommunicator(MPI_Comm aComm) {
    assert(aComm != MPI_COMM_NULL);
    rank = 1;
    axes = new MPSAxis[rank];
    comm = aComm;
    ownComm = false;
    checkInternalMPI(MPI_Comm_size(comm, &commSize));
    checkInternalMPI(MPI_Comm_rank(comm, &commRank));
    axes[0].procCount = commRank;
    axes[0].ourProc = commRank;
    axes[0].axisComm = this;
    resetNextAsyncTag();
}

DvmhCommunicator::DvmhCommunicator() {
    initDefault();
}

DvmhCommunicator &DvmhCommunicator::operator=(const DvmhCommunicator &other) {
    clear();
    rank = other.rank;
    axes = new MPSAxis[rank];
    if (other.comm == MPI_COMM_SELF || other.comm == MPI_COMM_NULL) {
        comm = other.comm;
        ownComm = false;
    } else {
        checkInternalMPI(MPI_Comm_dup(other.comm, &comm));
        ownComm = true;
    }
    commSize = other.commSize;
    for (int i = 0; i < rank; i++) {
        axes[i].procCount = other.axes[i].procCount;
        axes[i].commStep = other.axes[i].commStep;
        axes[i].ourProc = other.axes[i].ourProc;
        if (other.axes[i].axisComm) {
            if (other.axes[i].axisComm == &other) {
                axes[i].axisComm = this;
            } else if (!other.axes[i].ownComm) {
                axes[i].axisComm = other.axes[i].axisComm;
            } else {
                axes[i].axisComm = new DvmhCommunicator(*other.axes[i].axisComm);
                axes[i].ownComm = true;
            }
        }
    }
    commRank = other.commRank;
    return *this;
}

int DvmhCommunicator::getCommRank(int aRank, const int indexes[]) const {
    int res = 0;
    for (int i = 0; i < aRank; i++) {
        MPSAxis ax = getAxis(i + 1);
        assert(indexes[i] >= 0 && indexes[i] < ax.procCount);
        res += indexes[i] * ax.commStep;
    }
    return res;
}

int DvmhCommunicator::getCommRankAxisOffset(int axisNum, int offs, bool periodic) const {
    checkInternal(commRank >= 0);
    MPSAxis ax = getAxis(axisNum);
    if (!periodic) {
        assert(ax.ourProc + offs >= 0 && ax.ourProc + offs < ax.procCount);
        return commRank + offs * ax.commStep;
    } else {
        return commRank + ((ax.ourProc + offs % ax.procCount + ax.procCount) % ax.procCount - ax.ourProc) * ax.commStep;
    }
}

int DvmhCommunicator::getAxisIndex(int aCommRank, int axisNum) const {
    assert(aCommRank >= 0 && aCommRank < commSize);
    MPSAxis ax = getAxis(axisNum);
    return aCommRank / ax.commStep % ax.procCount;
}

void DvmhCommunicator::fillAxisIndexes(int aCommRank, int aRank, int indexes[]) const {
    assert(aCommRank >= 0 && aCommRank < commSize);
    for (int i = 0; i < aRank; i++) {
        MPSAxis ax = getAxis(i + 1);
        indexes[i] = aCommRank / ax.commStep % ax.procCount;
    }
}

UDvmType DvmhCommunicator::bcast(int root, void *buf, UDvmType size) const {
    resetNextAsyncTag();
    UDvmType rest = size;
    while (rest > 0) {
        int curSize;
        if (rest > INT_MAX)
            curSize = INT_MAX;
        else
            curSize = rest;
        checkInternalMPI(MPI_Bcast((char *)buf + (size - rest), curSize, MPI_BYTE, root, comm));
        rest -= curSize;
    }
    return size;
}

UDvmType DvmhCommunicator::bcast(int root, char **pBuf, UDvmType *pSize) const {
    resetNextAsyncTag();
    char fastBuffer[1024];
    UDvmType fastBufSize = sizeof(fastBuffer);
    UDvmType &size = *pSize;
    char *&buf = *pBuf;
    if (commSize == 1)
        return size;
    UDvmType fastBufDataSize = fastBufSize - sizeof(UDvmType);
    if (commRank == root) {
        BufferWalker fbw(fastBuffer, fastBufSize);
        fbw.putValue(size);
        fbw.putData(buf, std::min(size, fastBufDataSize));
    }
    bcast(root, fastBuffer, fastBufSize);
    if (commRank != root) {
        BufferWalker fbw(fastBuffer, fastBufSize);
        fbw.extractValue(size);
        buf = new char[size];
        fbw.extractData(buf, std::min(size, fastBufDataSize));
    }
    if (size > fastBufDataSize)
        bcast(root, buf + fastBufDataSize, size - fastBufDataSize);
    return size;
}

UDvmType DvmhCommunicator::send(int dest, const void *buf, UDvmType size) const {
    UDvmType rest = size;
    while (rest > 0) {
        int curSize;
        if (rest > INT_MAX)
            curSize = INT_MAX;
        else
            curSize = rest;
        checkInternalMPI(MPI_Send((char *)buf + (size - rest), curSize, MPI_BYTE, dest, 0, comm));
        rest -= curSize;
    }
    return size;
}

UDvmType DvmhCommunicator::recv(int src, void *buf, UDvmType size) const {
    UDvmType rest = size;
    while (rest > 0) {
        int curSize;
        if (rest > INT_MAX)
            curSize = INT_MAX;
        else
            curSize = rest;
        MPI_Status status;
        checkInternalMPI(MPI_Recv((char *)buf + (size - rest), curSize, MPI_BYTE, src, 0, comm, &status));
        int receivedCount = 0;
        checkInternalMPI(MPI_Get_count(&status, MPI_BYTE, &receivedCount));
        assert(receivedCount == curSize);
        rest -= curSize;
    }
    return size;
}

UDvmType DvmhCommunicator::allgather(void *buf, UDvmType elemSize) const {
    resetNextAsyncTag();
    checkInternal(elemSize <= (UDvmType)INT_MAX);
    checkInternalMPI(MPI_Allgather(MPI_IN_PLACE, elemSize, MPI_BYTE, buf, elemSize, MPI_BYTE, comm));
    return elemSize * commSize;
}

UDvmType DvmhCommunicator::allgatherv(void *buf, const UDvmType sizes[]) const {
    resetNextAsyncTag();
    int *recvcounts = new int[commSize];
    int *displs = new int[commSize + 1];
    for (int i = 0; i < commSize; i++) {
        recvcounts[i] = 0;
        displs[i] = 0;
    }
    int procDone = 0;
    UDvmType bytesDone = 0;
    for (int i = 0; i < commSize; i++) {
        if (sizes[i] > INT_MAX) {
            if (procDone < i) {
                checkInternalMPI(MPI_Allgatherv(MPI_IN_PLACE, recvcounts[commRank], MPI_BYTE, (char *)buf + bytesDone, recvcounts, displs, MPI_BYTE, comm));
                bytesDone += (UDvmType)displs[i - 1] + (UDvmType)recvcounts[i - 1];
                for (int j = procDone; j < i; j++) {
                    recvcounts[j] = 0;
                    displs[j] = 0;
                }
                procDone = i;
            }
            bcast(i, (char *)buf + bytesDone, sizes[i]);
            bytesDone += sizes[i];
            procDone = i + 1;
        } else {
            recvcounts[i] = sizes[i];
            if ((UDvmType)displs[i] + sizes[i] > INT_MAX) {
                checkInternalMPI(MPI_Allgatherv(MPI_IN_PLACE, recvcounts[commRank], MPI_BYTE, (char *)buf + bytesDone, recvcounts, displs, MPI_BYTE, comm));
                bytesDone += (UDvmType)displs[i] + (UDvmType)recvcounts[i];
                for (int j = procDone; j <= i; j++) {
                    recvcounts[j] = 0;
                    displs[j] = 0;
                }
                procDone = i + 1;
            }
            displs[i + 1] = displs[i] + recvcounts[i];
        }
    }
    if (procDone < commSize) {
        checkInternalMPI(MPI_Allgatherv(MPI_IN_PLACE, recvcounts[commRank], MPI_BYTE, (char *)buf + bytesDone, recvcounts, displs, MPI_BYTE, comm));
        bytesDone += (UDvmType)displs[commSize - 1] + (UDvmType)recvcounts[commSize - 1];
        procDone = commSize;
    }
    delete[] recvcounts;
    delete[] displs;
    return bytesDone;
}

UDvmType DvmhCommunicator::alltoall(const void *sendBuffer, UDvmType elemSize, void *recvBuffer) const {
    resetNextAsyncTag();
    checkInternal(elemSize <= INT_MAX);
    checkInternalMPI(MPI_Alltoall((void *)sendBuffer, elemSize, MPI_BYTE, recvBuffer, elemSize, MPI_BYTE, comm));
    return elemSize * commSize;
}

UDvmType DvmhCommunicator::alltoallv1(const UDvmType sendSizes[], char *sendBuffers[], UDvmType recvSizes[], char *recvBuffers[]) const {
    alltoall(sendSizes, recvSizes);
    for (int p = 0; p < commSize; p++)
        recvBuffers[p] = recvSizes[p] > 0 ? new char[recvSizes[p]] : 0;
    return alltoallv2(sendSizes, sendBuffers, recvSizes, recvBuffers);
}

UDvmType DvmhCommunicator::alltoallv2(const UDvmType sendSizes[], char *sendBuffers[], const UDvmType recvSizes[], char * const recvBuffers[]) const {
    dvmh_log(TRACE, "I am processor %d of %d", commRank, commSize);
    for (int p = 0; p < commSize; p++)
        dvmh_log(TRACE, "I send to %d processor " UDTFMT " bytes", p, sendSizes[p]);
    for (int p = 0; p < commSize; p++)
        dvmh_log(TRACE, "I receive from %d processor " UDTFMT " bytes", p, recvSizes[p]);
    dvmh_log(TRACE, "Current next async tag is %d", nextAsyncTag);
    assert(recvSizes[commRank] == sendSizes[commRank]);
    int sendCount = 0;
    int recvCount = 0;
    for (int p = 0; p < commSize; p++) {
        if (recvSizes[p] > 0)
            assert(recvBuffers[p]);
        if (sendSizes[p] > 0 && p != commRank)
            sendCount++;
        if (recvSizes[p] > 0 && p != commRank)
            recvCount++;
    }
    dvmh_log(TRACE, "Really I send to %d processors and receive from %d processors (excluded myself)", sendCount, recvCount);
    std::vector<MPI_Request> reqs;
    reqs.reserve(sendCount);
    for (int i = 0; i < commSize; i++) {
        int p = (commRank + i) % commSize;
        if (sendSizes[p] > 0 && p != commRank) {
            UDvmType rest = sendSizes[p];
            int tag = nextAsyncTag;
            while (rest > 0) {
                int curSize;
                if (rest > INT_MAX)
                    curSize = INT_MAX;
                else
                    curSize = rest;
                MPI_Request req;
                checkInternalMPI(MPI_Isend(sendBuffers[p] + (sendSizes[p] - rest), curSize, MPI_BYTE, p, tag, comm, &req));
                dvmh_log(TRACE, "Sent to %d processor %d bytes", p, curSize);
                reqs.push_back(req);
                rest -= curSize;
            }
        }
    }
    UDvmType bytesDone = 0;
    if (recvSizes[commRank] > 0) {
        if (recvBuffers[commRank] != sendBuffers[commRank])
            memcpy(recvBuffers[commRank], sendBuffers[commRank], recvSizes[commRank]);
        bytesDone += recvSizes[commRank];
    }
    int receivedCount = 0;
    while (receivedCount < recvCount) {
        int tag = nextAsyncTag;
        MPI_Status st;
        checkInternalMPI(MPI_Probe(MPI_ANY_SOURCE, tag, comm, &st));
        int p = st.MPI_SOURCE;
        assert(recvSizes[p] > 0);
        UDvmType rest = recvSizes[p];
        while (rest > 0) {
            int curSize;
            if (rest > INT_MAX)
                curSize = INT_MAX;
            else
                curSize = rest;
            checkInternalMPI(MPI_Recv(recvBuffers[p] + (recvSizes[p] - rest), curSize, MPI_BYTE, p, tag, comm, &st));
            int receivedSize = 0;
            checkInternalMPI(MPI_Get_count(&st, MPI_BYTE, &receivedSize));
            dvmh_log(TRACE, "Received from %d processor %d bytes", p, receivedSize);
            assert(receivedSize == curSize);
            rest -= curSize;
        }
        receivedCount++;
        bytesDone += recvSizes[p];
    }
    checkInternalMPI(MPI_Waitall(reqs.size(), &reqs[0], MPI_STATUSES_IGNORE));
    advanceNextAsyncTag();
    return bytesDone;
}

UDvmType DvmhCommunicator::alltoallv3(int sendProcCount, const int sendProcs[], const UDvmType sendSizes[], char *sendBuffers[], int recvProcCount,
        const int recvProcs[], const UDvmType recvSizes[], char * const recvBuffers[]) const {
    dvmh_log(TRACE, "I am processor %d of %d", commRank, commSize);
    for (int pi = 0; pi < sendProcCount; pi++)
        dvmh_log(TRACE, "I send to %d processor " UDTFMT " bytes", sendProcs[pi], sendSizes[pi]);
    for (int pi = 0; pi < recvProcCount; pi++)
        dvmh_log(TRACE, "I receive from %d processor " UDTFMT " bytes", recvProcs[pi], recvSizes[pi]);
    dvmh_log(TRACE, "Current next async tag is %d", nextAsyncTag);
    int sendCount = 0;
    int recvCount = 0;
    int selfSendIndex = -1;
    int selfRecvIndex = -1;
    bool recvProcsOrdered = true;
    for (int pi = 0; pi < sendProcCount; pi++) {
        bool itsMe = sendProcs[pi] == commRank;
        if (sendSizes[pi] > 0 && !itsMe)
            sendCount++;
        if (itsMe)
            selfSendIndex = pi;
    }
    for (int pi = 0; pi < recvProcCount; pi++) {
        bool itsMe = recvProcs[pi] == commRank;
        if (recvSizes[pi] > 0 && !itsMe)
            recvCount++;
        if (itsMe)
            selfRecvIndex = pi;
        recvProcsOrdered = recvProcsOrdered && (pi == 0 || recvProcs[pi] > recvProcs[pi - 1]);
    }
    assert((selfSendIndex >= 0) == (selfRecvIndex >= 0));
    if (selfSendIndex >= 0)
        assert(sendSizes[selfSendIndex] == recvSizes[selfRecvIndex]);
    dvmh_log(TRACE, "Really I send to %d processors and receive from %d processors (excluded myself)", sendCount, recvCount);
    std::vector<MPI_Request> reqs;
    reqs.reserve(sendCount);
    for (int pi = 0; pi < sendProcCount; pi++) {
        int p = sendProcs[pi];
        if (sendSizes[pi] > 0 && p != commRank) {
            UDvmType rest = sendSizes[pi];
            int tag = nextAsyncTag;
            while (rest > 0) {
                int curSize;
                if (rest > INT_MAX)
                    curSize = INT_MAX;
                else
                    curSize = rest;
                MPI_Request req;
                checkInternalMPI(MPI_Isend(sendBuffers[pi] + (sendSizes[pi] - rest), curSize, MPI_BYTE, p, tag, comm, &req));
                dvmh_log(TRACE, "Sent to %d processor %d bytes", p, curSize);
                reqs.push_back(req);
                rest -= curSize;
            }
        }
    }
    UDvmType bytesDone = 0;
    if (selfRecvIndex >= 0 && recvSizes[selfRecvIndex] > 0) {
        if (recvBuffers[selfRecvIndex] != sendBuffers[selfSendIndex])
            memcpy(recvBuffers[selfRecvIndex], sendBuffers[selfSendIndex], recvSizes[selfRecvIndex]);
        bytesDone += recvSizes[selfRecvIndex];
    }
    int receivedCount = 0;
    std::vector<std::pair<int, int> > recvIndexSearch;
    if (!recvProcsOrdered) {
        recvIndexSearch.reserve(recvProcCount);
        for (int pi = 0; pi < recvProcCount; pi++)
            recvIndexSearch.push_back(std::make_pair(recvProcs[pi], pi));
        std::sort(recvIndexSearch.begin(), recvIndexSearch.end());
    }
    while (receivedCount < recvCount) {
        int tag = nextAsyncTag;
        MPI_Status st;
        checkInternalMPI(MPI_Probe(MPI_ANY_SOURCE, tag, comm, &st));
        int p = st.MPI_SOURCE;
        int pi;
        if (recvProcsOrdered) {
            pi = exactIndex(recvProcs, recvProcCount, p);
        } else {
            int idx = upperIndex(&recvIndexSearch[0], recvProcCount, std::make_pair(p, -1));
            assert(idx >= 0 && idx < recvProcCount);
            assert(recvIndexSearch[idx].first == p);
            pi = recvIndexSearch[idx].second;
        }
        assert(pi >= 0 && pi < recvProcCount);
        assert(recvSizes[pi] > 0);
        UDvmType rest = recvSizes[pi];
        while (rest > 0) {
            int curSize;
            if (rest > INT_MAX)
                curSize = INT_MAX;
            else
                curSize = rest;
            checkInternalMPI(MPI_Recv(recvBuffers[pi] + (recvSizes[pi] - rest), curSize, MPI_BYTE, p, tag, comm, &st));
            int receivedSize = 0;
            checkInternalMPI(MPI_Get_count(&st, MPI_BYTE, &receivedSize));
            dvmh_log(TRACE, "Received from %d processor %d bytes", p, receivedSize);
            assert(receivedSize == curSize);
            rest -= curSize;
        }
        receivedCount++;
        bytesDone += recvSizes[pi];
    }
    checkInternalMPI(MPI_Waitall(reqs.size(), &reqs[0], MPI_STATUSES_IGNORE));
    advanceNextAsyncTag();
    return bytesDone;
}

void DvmhCommunicator::barrier() const {
    resetNextAsyncTag();
    checkInternalMPI(MPI_Barrier(comm));
}

static void allReduceMPI(void *addr, MPI_Datatype dt, MPI_Op op, MPI_Comm comm, UDvmType count = 1) {
    UDvmType rest = count;
    MPI_Aint lb, extent;
    MPI_Type_get_extent(dt, &lb, &extent);
    while (rest > 0) {
        int curCount;
        if (rest > INT_MAX)
            curCount = INT_MAX;
        else
            curCount = rest;
        checkInternalMPI(MPI_Allreduce(MPI_IN_PLACE, (char *)addr + (count - rest) * extent, curCount, dt, op, comm));
        rest -= curCount;
    }
}

static MPI_Op dvmRedFuncToMPI(int redFunc, bool *pSuccess = 0) {
    if (pSuccess)
        *pSuccess = true;
    switch (redFunc) {
        case rf_SUM: return MPI_SUM;
        case rf_PROD: return MPI_PROD;
        case rf_MAX: return MPI_MAX;
        case rf_MIN: return MPI_MIN;
        case rf_AND: return MPI_BAND;
        case rf_OR: return MPI_BOR;
        case rf_XOR: return MPI_BXOR;
        case rf_NE: return MPI_BXOR;
        case rf_MAXLOC: return MPI_MAXLOC;
        case rf_MINLOC: return MPI_MINLOC;
        default: checkInternal(pSuccess);
    }
    *pSuccess = false;
    return MPI_SUM;
}

static MPI_Datatype dvmTypeToMPI(DvmhData::DataType dt, bool *pSuccess) {
    if (pSuccess)
        *pSuccess = true;
    switch (dt) {
        case DvmhData::dtChar: return MPI_CHAR;
        case DvmhData::dtUChar: return MPI_UNSIGNED_CHAR;
        case DvmhData::dtShort: return MPI_SHORT;
        case DvmhData::dtUShort: return MPI_UNSIGNED_SHORT;
        case DvmhData::dtInt: return MPI_INT;
        case DvmhData::dtUInt: return MPI_UNSIGNED;
        case DvmhData::dtLong: return MPI_LONG;
        case DvmhData::dtULong: return MPI_UNSIGNED_LONG;
        case DvmhData::dtLongLong: return MPI_LONG_LONG;
        case DvmhData::dtULongLong: return MPI_UNSIGNED_LONG_LONG;
        case DvmhData::dtFloat: return MPI_FLOAT;
        case DvmhData::dtDouble: return MPI_DOUBLE;
        case DvmhData::dtFloatComplex: return MPI_C_FLOAT_COMPLEX;
        case DvmhData::dtDoubleComplex: return MPI_C_DOUBLE_COMPLEX;
        case DvmhData::dtLogical: return MPI_INT;
        default: checkInternal(pSuccess);
    }
    *pSuccess = false;
    return MPI_BYTE;
}

template <>
void DvmhCommunicator::allreduce(bool &var, int redFunc) const {
    resetNextAsyncTag();
    unsigned char tmp = var ? 1 : 0;
    allReduceMPI(&tmp, MPI_UNSIGNED_CHAR, dvmRedFuncToMPI(redFunc), comm);
    var = tmp ? true : false;
}

template <>
void DvmhCommunicator::allreduce(unsigned char &var, int redFunc) const {
    resetNextAsyncTag();
    allReduceMPI(&var, MPI_UNSIGNED_CHAR, dvmRedFuncToMPI(redFunc), comm);
}

template <>
void DvmhCommunicator::allreduce(int &var, int redFunc) const {
    resetNextAsyncTag();
    allReduceMPI(&var, MPI_INT, dvmRedFuncToMPI(redFunc), comm);
}

template <>
void DvmhCommunicator::allreduce(UDvmType &var, int redFunc) const {
    resetNextAsyncTag();
    allReduceMPI(&var, (sizeof(UDvmType) == sizeof(long) ? MPI_UNSIGNED_LONG : MPI_UNSIGNED_LONG_LONG), dvmRedFuncToMPI(redFunc), comm);
}

template <>
void DvmhCommunicator::allreduce(std::pair<int, int> &var, int redFunc) const {
    resetNextAsyncTag();
    allReduceMPI(&var, MPI_2INT, dvmRedFuncToMPI(redFunc), comm);
}

template <>
void DvmhCommunicator::allreduce(long long &var, int redFunc) const {
    resetNextAsyncTag();
    allReduceMPI(&var, MPI_LONG_LONG, dvmRedFuncToMPI(redFunc), comm);
}

template <>
void DvmhCommunicator::allreduce(UDvmType arr[], int redFunc, UDvmType count) const {
    resetNextAsyncTag();
    allReduceMPI(arr, (sizeof(UDvmType) == sizeof(long) ? MPI_UNSIGNED_LONG : MPI_UNSIGNED_LONG_LONG), dvmRedFuncToMPI(redFunc), comm, count);
}

static THREAD_LOCAL const DvmhReduction *currentRed = 0;

static void dvmhReductionMPIOp(char *in, char *inout, int *len, MPI_Datatype *datatype) {
    UDvmType pitch = currentRed->elemCount * (currentRed->elemSize + currentRed->locSize);
    UDvmType locOffset = currentRed->elemCount * currentRed->elemSize;
    for (int j = 0; j < *len; j++) {
        currentRed->performOperation(inout, inout + locOffset, in, in + locOffset);
        in += pitch;
        inout += pitch;
    }
}

void DvmhCommunicator::allreduce(DvmhReduction *red) const {
    resetNextAsyncTag();
    MPI_Datatype dt;
    MPI_Op op;
    // Try to use a shortcut if reduction is 'simple'
    if (!red->isLoc()) {
        bool funcOk;
        op = dvmRedFuncToMPI(red->redFunc, &funcOk);
        if (funcOk) {
            bool typeOk;
            dt = dvmTypeToMPI(red->arrayElementType, &typeOk);
            if (typeOk) {
                allReduceMPI(red->arrayAddr, dt, op, comm, red->elemCount);
                return;
            }
        }
    }
    // Common case
    MPI_Op_create((MPI_User_function *)dvmhReductionMPIOp, 1, &op);
    MPI_Type_contiguous(red->elemCount * (red->elemSize + red->locSize), MPI_BYTE, &dt);
    MPI_Type_commit(&dt);
    currentRed = red;
    char *addr = new char[red->elemCount * (red->elemSize + red->locSize)];
    memcpy(addr, red->arrayAddr, red->elemCount * red->elemSize);
    if (red->isLoc())
        memcpy(addr + red->elemCount * red->elemSize, red->locAddr, red->elemCount * red->locSize);
    allReduceMPI(addr, dt, op, comm);
    memcpy(red->arrayAddr, addr, red->elemCount * red->elemSize);
    if (red->isLoc())
        memcpy(red->locAddr, addr + red->elemCount * red->elemSize, red->elemCount * red->locSize);
    delete[] addr;
    MPI_Type_free(&dt);
    MPI_Op_free(&op);
    currentRed = 0;
}

void DvmhCommunicator::initDefault() {
    rank = 0;
    axes = new MPSAxis[rank];
    comm = MPI_COMM_SELF;
    ownComm = false;
    commSize = 1;
    commRank = 0;
    resetNextAsyncTag();
}

void DvmhCommunicator::clear() {
    for (int i = 0; i < rank; i++)
        axes[i].freeAxisComm();
    delete[] axes;
    if (ownComm)
        checkInternalMPI(MPI_Comm_free(&comm));
    rank = -1;
    axes = 0;
    comm = MPI_COMM_NULL;
    ownComm = false;
    commSize = 0;
    commRank = -1;
    resetNextAsyncTag();
}

void DvmhCommunicator::resetNextAsyncTag() const {
    nextAsyncTag = 1;
}

void DvmhCommunicator::advanceNextAsyncTag() const {
    if (nextAsyncTag < INT_MAX)
        nextAsyncTag++;
    else
        barrier();
}

DvmhCommunicator *DvmhCommunicator::selfComm = new DvmhCommunicator();

// MultiprocessorSystem

MultiprocessorSystem::MultiprocessorSystem(MPI_Comm aComm, int aRank, const int sizes[]):
            DvmhCommunicator(aComm, aRank, sizes) {
    parentMPS = 0;
    parentToOur = 0;
    ourToParent = 0;
    procWeights = 0;
    ioProc = 0;
    assert(ioProc >= 0 && ioProc < commSize);
}

MultiprocessorSystem::MultiprocessorSystem() {
    parentMPS = 0;
    parentToOur = 0;
    ourToParent = 0;
    procWeights = 0;
    ioProc = 0;
}

int MultiprocessorSystem::getChildCommRank(const MultiprocessorSystem *child, int aCommRank) const {
    if (child == this) {
        return aCommRank;
    } else {
        checkInternal(child->parentMPS);
        int parentCommRank = getChildCommRank(child->parentMPS, aCommRank);
        return (parentCommRank >= 0 ? child->parentToOur[parentCommRank] : -1);
    }
}

int MultiprocessorSystem::getParentCommRank(const MultiprocessorSystem *parent, int aCommRank) const {
    if (parent == this) {
        return aCommRank;
    } else {
        checkInternal(parentMPS);
        return parentMPS->getParentCommRank(parent, ourToParent[aCommRank]);
    }
}

int MultiprocessorSystem::getOtherCommRank(const MultiprocessorSystem *other, int aCommRank) const {
    if (this == other)
        return aCommRank;
    else if (other->isSubsystemOf(this))
        return getChildCommRank(other, aCommRank);
    else if (this->isSubsystemOf(other))
        return getParentCommRank(other, aCommRank);
    else
        return -2;
}

void MultiprocessorSystem::setWeights(const double wgts[], int len) {
    delete[] procWeights;
    procWeights = new double[commSize];
    if (len <= 0)
        len = commSize;
    if (len >= commSize) {
        typedMemcpy(procWeights, wgts, commSize);
    } else {
        for (int i = 0; i < commSize; i++)
            procWeights[i] = wgts[i % len];
    }
    double sum = 0;
    for (int i = 0; i < commSize; i++)
        sum += procWeights[i];
    checkInternal(sum > 0);
    double coef = commSize / sum;
    for (int i = 0; i < commSize; i++)
        procWeights[i] *= coef;
    for (int ax = 0; ax < rank; ax++) {
        axes[ax].freeCoordWeights();
        int pc = axes[ax].procCount;
        int step = axes[ax].commStep;
        if (pc > 1) {
            double *coordWeights = new double[pc];
            for (int i = 0; i < pc; i++)
                coordWeights[i] = 0;
            for (int i = 0; i < commSize; i++)
                coordWeights[i / step % pc] += procWeights[i];
            bool weighted = false;
            double etalon = coordWeights[0];
            double sum = etalon;
            for (int i = 1; i < pc; i++) {
                double val = coordWeights[i];
                if (!((etalon < 1e-6 && val < 1e-6) || (etalon > 1e-6 && fabs(1.0 - val / etalon) < 1e-2)))
                    weighted = true;
                sum += val;
            }
            if (sum < 1e-6)
                weighted = false;
            if (weighted)
                axes[ax].setCoordWeights(coordWeights);
            else
                delete[] coordWeights;
        }
    }
}

static bool areEqual(MPI_Comm comm1, MPI_Comm comm2) {
    bool res = false;
    if (comm1 == comm2)
        res = true;
    if (!res) {
        int result = MPI_UNEQUAL;
        checkInternalMPI(MPI_Comm_compare(comm1, comm2, &result));
        if (result == MPI_IDENT || result == MPI_CONGRUENT)
            res = true;
    }
    return res;
}

static bool areSimilar(MPI_Comm comm1, MPI_Comm comm2) {
    bool res = false;
    if (comm1 == comm2)
        res = true;
    if (!res) {
        int result = MPI_UNEQUAL;
        checkInternalMPI(MPI_Comm_compare(comm1, comm2, &result));
        if (result != MPI_UNEQUAL)
            res = true;
    }
    return res;
}

void MultiprocessorSystem::attachChildMPS(MultiprocessorSystem *child) {
    assert(child);
    assert(children.find(child) == children.end());
    assert(child->parentMPS == 0);
    child->parentMPS = this;
    child->parentToOur = new int[commSize];
    if (areEqual(comm, child->comm)) {
        for (int i = 0; i < commSize; i++)
            child->parentToOur[i] = i;
    } else if (areSimilar(comm, child->comm)) {
        int *iota = new int[commSize];
        for (int i = 0; i < commSize; i++)
            iota[i] = i;
        MPI_Group myGroup, childGroup;
        checkInternalMPI(MPI_Comm_group(comm, &myGroup));
        checkInternalMPI(MPI_Comm_group(child->comm, &childGroup));
        checkInternalMPI(MPI_Group_translate_ranks(myGroup, commSize, iota, childGroup, child->parentToOur));
        delete[] iota;
    } else {
        child->parentToOur[commRank] = child->getCommRank();
        allgather(child->parentToOur);
    }
    child->ourToParent = new int[child->commSize];
    for (int i = 0; i < commSize; i++) {
        if (child->parentToOur[i] >= 0)
            child->ourToParent[child->parentToOur[i]] = i;
    }
}

bool MultiprocessorSystem::isSubsystemOf(const MultiprocessorSystem *otherMPS) const {
    assert(otherMPS);
    const MultiprocessorSystem *curMPS = this;
    while (curMPS && curMPS != otherMPS)
        curMPS = curMPS->parentMPS;
    return (curMPS == otherMPS);
}

int MultiprocessorSystem::newFile(DvmhFile *stream) {
    assert(stream);
    int fn = -1;
    for (int i = 0; i < (int)myFiles.size(); i++)
        if (!myFiles[i]) {
            myFiles[i] = stream;
            fn = i;
            break;
        }
    if (fn < 0) {
        myFiles.push_back(stream);
        fn = myFiles.size() - 1;
    }
    return fn;
}

void MultiprocessorSystem::deleteFile(int fn) {
    checkInternal(fn >= 0 && fn < (int)myFiles.size() && myFiles[fn]);
    myFiles[fn] = 0;
    while (!myFiles.empty() && !myFiles.back())
        myFiles.pop_back();
}

bool MultiprocessorSystem::isSimilarTo(const MultiprocessorSystem *other) const {
    // TODO: If current processor does not belong to this MPS, then comm is MPI_COMM_NULL and it should be compared differently (perharps through parent)
    return this == other || areSimilar(comm, other->comm);
}

MultiprocessorSystem::~MultiprocessorSystem() {
    bool similarToParent = parentMPS ? isSimilarTo(parentMPS) : false;
    checkInternal(children.empty() || similarToParent);
    while (!children.empty()) {
        MultiprocessorSystem *child = *children.begin();
        detachChildMPS(child);
        parentMPS->attachChildMPS(child);
    }
    checkInternal(myFiles.empty());
    if (parentMPS)
        parentMPS->detachChildMPS(this);
    assert(parentToOur == 0);
    assert(ourToParent == 0);
    for (int i = 0; i < rank; i++)
        axes[i].freeCoordWeights();
    delete[] procWeights;
}

void MultiprocessorSystem::detachChildMPS(MultiprocessorSystem *child) {
    assert(children.find(child) != children.end());
    assert(child->parentMPS == this);
    child->parentMPS = 0;
    delete[] child->parentToOur;
    child->parentToOur = 0;
    delete[] child->ourToParent;
    child->ourToParent = 0;
    children.erase(child);
}

}
