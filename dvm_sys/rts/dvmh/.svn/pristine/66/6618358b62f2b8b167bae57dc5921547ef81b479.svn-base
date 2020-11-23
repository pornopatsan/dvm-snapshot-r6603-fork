#pragma once

#include <cassert>
#include <set>

#pragma GCC visibility push(default)
#ifndef _MPI_STUBS_
#include <mpi.h>
#else
#include <mpi_stubs.h>
#endif
#pragma GCC visibility pop

#include "dvmh_types.h"
#include "util.h"

namespace libdvmh {

class DvmhCommunicator;

class MPSAxis {
public:
    // Global
    int procCount;
    int commStep;
    // Local
    int ourProc; // -1 means this processor does not belong to this MPS
    DvmhCommunicator *axisComm;
public:
    double getCoordWeight(int p) const { assert(p >= 0 && p < procCount); return (coordWeights ? coordWeights[p] : 1.0); }
    bool isWeighted() const { return weightedFlag; }
public:
    explicit MPSAxis(int aCommStep = 1, int aOurProc = 0, DvmhCommunicator *aAxisComm = 0): procCount(1), commStep(aCommStep), ourProc(aOurProc),
            axisComm(aAxisComm), coordWeights(0), weightedFlag(false), ownComm(false) {}
protected:
    void setCoordWeights(double wgts[], bool copyFlag = false);
    void freeCoordWeights() {
        delete[] coordWeights;
        coordWeights = 0;
        weightedFlag = false;
    }
    void freeAxisComm();
protected:
    double *coordWeights;
    bool weightedFlag;
    bool ownComm;
private:
    friend class DvmhCommunicator;
    friend class MultiprocessorSystem;
};

class DvmhReduction;

class DvmhCommunicator: public DvmhObject {
public:
    int getRank() const { return rank; }
    MPI_Comm getComm() const { return comm; }
    int getCommRank() const { return commRank; }
    int getCommSize() const { return commSize; }
    MPSAxis getAxis(int axisNum) const {
        assert(axisNum >= 1);
        return (axisNum <= rank ? axes[axisNum - 1] : MPSAxis(commSize, (commRank >= 0 ? 0 : -1), (commRank >= 0 ? selfComm : 0)));
    }
public:
    explicit DvmhCommunicator(MPI_Comm aComm, int aRank, const int sizes[]);
    explicit DvmhCommunicator(MPI_Comm aComm);
    explicit DvmhCommunicator();
    DvmhCommunicator(const DvmhCommunicator &other) {
        initDefault();
        *this = other;
    }
    DvmhCommunicator &operator=(const DvmhCommunicator &other);
public:
    int getCommRank(int aRank, const int indexes[]) const;
    int getCommRankAxisOffset(int axisNum, int offs, bool periodic = false) const;
    int getAxisIndex(int aCommRank, int axisNum) const;
    void fillAxisIndexes(int aCommRank, int aRank, int indexes[]) const;

    UDvmType bcast(int root, void *buf, UDvmType size) const;
    template <typename T>
    UDvmType bcast(int root, T &var) const {
        return bcast(root, &var, sizeof(T));
    }
    UDvmType bcast(int root, char **pBuf, UDvmType *pSize) const;
    UDvmType send(int dest, const void *buf, UDvmType size) const;
    template <typename T>
    UDvmType send(int dest, const T &val) const {
        return send(dest, &val, sizeof(T));
    }
    UDvmType recv(int src, void *buf, UDvmType size) const;
    template <typename T>
    UDvmType recv(int src, T &var) const {
        return recv(src, &var, sizeof(T));
    }
    UDvmType allgather(void *buf, UDvmType elemSize) const;
    template <typename T>
    UDvmType allgather(T array[]) const {
        return allgather(array, sizeof(T));
    }
    template <typename T>
    UDvmType allgather(std::vector<T> &array) const {
        return allgather(&array[0]);
    }
    UDvmType allgatherv(void *buf, const UDvmType sizes[]) const;
    UDvmType alltoall(const void *sendBuffer, UDvmType elemSize, void *recvBuffer) const;
    template <typename T>
    UDvmType alltoall(const T sendArray[], T recvArray[]) const {
        return alltoall(sendArray, sizeof(T), recvArray);
    }
    // The version when we know only sendSizes, recvBuffers will be allocated inside
    UDvmType alltoallv1(const UDvmType sendSizes[], char *sendBuffers[], UDvmType recvSizes[], char *recvBuffers[]) const;
    // The version when we know all the sizes and preallocated buffers
    UDvmType alltoallv2(const UDvmType sendSizes[], char *sendBuffers[], const UDvmType recvSizes[], char * const recvBuffers[]) const;
    // The version when we know all the sizes and preallocated buffers and we pass the list of processors we interact with
    UDvmType alltoallv3(int sendProcCount, const int sendProcs[], const UDvmType sendSizes[], char *sendBuffers[], int recvProcCount, const int recvProcs[],
            const UDvmType recvSizes[], char * const recvBuffers[]) const;
    void barrier() const;
    template <typename T>
    void allreduce(T &var, int redFunc) const;
    template <typename T>
    void allreduce(T arr[], int redFunc, UDvmType count) const;
    void allreduce(DvmhReduction *red) const;
public:
    virtual ~DvmhCommunicator() { clear(); }
protected:
    void initDefault();
    void clear();
    void resetNextAsyncTag() const;
    void advanceNextAsyncTag() const;
protected:
    // Global
    int rank;
    MPSAxis *axes; // Array of MPSAxis (length=rank)
    MPI_Comm comm;
    bool ownComm;
    int commSize;
    mutable int nextAsyncTag;
    // Local
    int commRank; // -1 means this processor does not belong to this Communicator
protected:
    static DvmhCommunicator *selfComm;
};

class MultiprocessorSystem: public DvmhCommunicator, private Uncopyable {
public:
    int getIOProc() const { return ioProc; }
    bool isIOProc() const { return commRank == ioProc; }
    int getFileCount() const { return myFiles.size(); }
    DvmhFile *getFile(int i) const { return myFiles[i]; }
public:
    explicit MultiprocessorSystem(MPI_Comm aComm, int aRank, const int sizes[]);
    explicit MultiprocessorSystem();
public:
    int getChildCommRank(const MultiprocessorSystem *child, int aCommRank) const;
    int getParentCommRank(const MultiprocessorSystem *parent, int aCommRank) const;
    int getOtherCommRank(const MultiprocessorSystem *other, int aCommRank) const;
    void setWeights(const double wgts[], int len = 0);
    void attachChildMPS(MultiprocessorSystem *child);
    bool isSubsystemOf(const MultiprocessorSystem *otherMPS) const;
    UDvmType ioBcast(void *buf, UDvmType size) const {
        return bcast(ioProc, buf, size);
    }
    int newFile(DvmhFile *stream);
    void deleteFile(int fn);
    bool isSimilarTo(const MultiprocessorSystem *other) const;
public:
    ~MultiprocessorSystem();
protected:
    void detachChildMPS(MultiprocessorSystem *child);
protected:
    // Global
    MultiprocessorSystem *parentMPS;
    std::set<MultiprocessorSystem *> children;
    int *parentToOur; // Array of commRanks of this MPS (length=parentMPS->commSize)
    int *ourToParent; // Array of commRanks of parent MPS (length=commSize)
    double *procWeights;
    int ioProc;
    HybridVector<DvmhFile *, 10> myFiles;
};

}
