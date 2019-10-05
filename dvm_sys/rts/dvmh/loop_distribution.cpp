#include "loop_distribution.h"

#include <cstdlib>
#include <cstring>

#include "dvmh_device.h"
#include "dvmh_log.h"

namespace libdvmh {

template <typename IndexT>
class Cache {
public:
    explicit Cache(int aSize, CudaDevice *aCudaDev) {
        count = 0;
        beginIndex = 0;
        size = aSize;
        cudaDev = aCudaDev;
        entries = new CacheEntry[size];
    }
public:
    bool add(int rank, const LoopBounds loopBounds[], int threads[3], UDvmType resBlocks, IndexT *blocksAddress) {
        int nextIndex = (beginIndex + count) % size;
        bool res = false;
        CacheEntry *nextEntry = &entries[nextIndex];
        if (count == size) {
            delete[] nextEntry->loopBounds;
            cudaDev->dispose(nextEntry->blocksAddress);
            count--;
            beginIndex = (beginIndex + 1) % size;
            res = true;
        }
        count++;
        nextEntry->rank = rank;
        nextEntry->loopBounds = new LoopBounds[rank];
        for (int i = 0; i < rank; i++)
            for (int j = 0; j < 3; j++)
                nextEntry->loopBounds[i][j] = loopBounds[i][j];
        typedMemcpy(nextEntry->threads, threads, 3);
        nextEntry->resBlocks = resBlocks;
        nextEntry->blocksAddress = blocksAddress;
        return res;
    }
    bool probe(int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks, IndexT **pBlocksAddress) {
        for (int i = 0; i < count; i++) {
            CacheEntry *entry = &entries[(beginIndex + count - 1 - i) % size];
            bool flag = entry->rank == rank;
            for (int j = 0; flag && j < rank; j++)
                for (int k = 0; flag && k < 3; k++)
                    flag = flag && entry->loopBounds[j][k] == loopBounds[j][k];
            flag = flag && (entry->threads[0] == threads[0] && entry->threads[1] == threads[1] && entry->threads[2] == threads[2]);
            if (flag) {
                *pResBlocks = entry->resBlocks;
                *pBlocksAddress = entry->blocksAddress;
                return true;
            }
        }
        return false;
    }
    void clear() {
        for (int i = 0; i < count; i++) {
            CacheEntry *entry = &entries[(beginIndex + count - 1 - i) % size];
            delete[] entry->loopBounds;
            cudaDev->dispose(entry->blocksAddress);
        }
        beginIndex = 0;
        count = 0;
    }
public:
    ~Cache() {
        clear();
        delete[] entries;
    }
protected:
    struct CacheEntry {
        int rank;
        LoopBounds *loopBounds; //Array of LoopBounds (length=rank)
        int threads[3];
        UDvmType resBlocks;
        IndexT *blocksAddress;
    };
protected:
    int size;
    CudaDevice *cudaDev;
    int count;
    int beginIndex;
    CacheEntry *entries; //Array of CacheEntry (length=size)
};

template <int rank, typename IndexT>
static void genBlocksInfo(const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks, IndexT **pBlocksAddress) {
    typedef IndexT BlockInfo[rank][2];
    BlockInfo *blocksInfo;
    UDvmType resBlocks;

    UDvmType steps[rank];
    for (int i = 0; i < rank - 3; i++)
        steps[i] = 1;
    for (int i = 0; i < 3 && i < rank; i++)
        steps[rank - 1 - i] = threads[i];

    resBlocks = 1;
    for (int i = 0; i < rank; i++) {
        UDvmType count = loopBounds[i].iterCount();
        resBlocks *= divUpU(count, steps[i]);
    }
    blocksInfo = new BlockInfo[resBlocks];
    DvmType curVals[rank];
    UDvmType curLens[rank];
    for (int i = 0; i < rank; i++) {
        curVals[i] = loopBounds[i][0];
        UDvmType rest = divDownS(loopBounds[i][1] - curVals[i], loopBounds[i][2]) + 1;
        if (rest <= steps[i])
            curLens[i] = rest;
        else
            curLens[i] = steps[i];
    }
    for (UDvmType i = 0; i < resBlocks; i++) {
        for (int j = 0; j < rank; j++) {
            blocksInfo[i][j][0] = curVals[j];
            blocksInfo[i][j][1] = curVals[j] + (curLens[j] - 1) * loopBounds[j][2];
        }
// Perform step
        for (int j = rank - 1; j >= 0; j--) {
            curVals[j] += loopBounds[j][2] * curLens[j];
            UDvmType rest = divDownS(loopBounds[j][1] - curVals[j], loopBounds[j][2]) + 1;
            if (rest <= 0)
                curVals[j] = loopBounds[j][0];
            else {
                if (rest <= steps[j])
                    curLens[j] = rest;
                else
                    curLens[j] = steps[j];
                break;
            }
        }
    }
    *pResBlocks = resBlocks;
    *pBlocksAddress = (IndexT *)blocksInfo;
}

static std::vector<Cache<int> *> cachesInt;
static std::vector<Cache<long> *> cachesLong;
static std::vector<Cache<long long> *> cachesLongLong;

void prepareLoopDistributionCaches() {
    int maxDeviceNum = -1;
    for (int i = 0; i < devicesCount; i++)
        if (devices[i]->getType() == dtCuda)
            maxDeviceNum = i;
    if (maxDeviceNum >= 0) {
        cachesInt.resize(maxDeviceNum + 1, 0);
        cachesLong.resize(maxDeviceNum + 1, 0);
        cachesLongLong.resize(maxDeviceNum + 1, 0);
    }
}

template <typename IndexT>
bool dvmhCudaGetDistribution(int deviceNum, int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks, IndexT **pBlocksAddress) {
    CudaDevice *cudaDev = (CudaDevice *)devices[deviceNum];
    std::vector<Cache<IndexT> *> *caches = 0;
    if (typeid(IndexT) == typeid(int))
        caches = (std::vector<Cache<IndexT> *> *)&cachesInt;
    else if (typeid(IndexT) == typeid(long))
        caches = (std::vector<Cache<IndexT> *> *)&cachesLong;
    else if (typeid(IndexT) == typeid(long long))
        caches = (std::vector<Cache<IndexT> *> *)&cachesLongLong;
    else
        assert(false);
    Cache<IndexT> *cache = caches->at(deviceNum);
    if (!cache)
        cache = caches->at(deviceNum) = new Cache<IndexT>(128, cudaDev);
    bool hit = false;
#ifndef DONT_USE_CACHE
    hit = cache->probe(rank, loopBounds, threads, pResBlocks, pBlocksAddress);
    dvmh_log(TRACE, "Cache probe: %s", hit ? "HIT!" : "miss");
#endif
    if (!hit) {
        IndexT *blocksInfo = NULL;
        switch (rank) {
            case 1:
                genBlocksInfo<1>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 2:
                genBlocksInfo<2>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 3:
                genBlocksInfo<3>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 4:
                genBlocksInfo<4>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 5:
                genBlocksInfo<5>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 6:
                genBlocksInfo<6>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 7:
                genBlocksInfo<7>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 8:
                genBlocksInfo<8>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 9:
                genBlocksInfo<9>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 10:
                genBlocksInfo<10>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 11:
                genBlocksInfo<11>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            case 12:
                genBlocksInfo<12>(loopBounds, threads, pResBlocks, &blocksInfo);
                break;
            default:
// TODO: Get rid of this ugly switch
                checkInternal3(0, "Block-distribution for %d-dimensional loops is not implemented yet", rank);
                break;
        }
        UDvmType resBlocks = *pResBlocks;
        IndexT *deviceBlocksInfo;
        deviceBlocksInfo = cudaDev->alloc<IndexT>(resBlocks * rank * 2);
        cudaDev->setValues(deviceBlocksInfo, blocksInfo, resBlocks * rank * 2);
        delete[] blocksInfo;
        *pBlocksAddress = deviceBlocksInfo;

        cache->add(rank, loopBounds, threads, resBlocks, deviceBlocksInfo);
    }
    return hit;
}

template bool dvmhCudaGetDistribution(int deviceNum, int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks, int **pBlocksAddress);
template bool dvmhCudaGetDistribution(int deviceNum, int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks,
        long **pBlocksAddress);
template bool dvmhCudaGetDistribution(int deviceNum, int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks,
        long long **pBlocksAddress);

void clearLoopDistributionCaches() {
    for (int i = 0; i < (int)cachesInt.size(); i++) {
        delete cachesInt[i];
        delete cachesLong[i];
        delete cachesLongLong[i];
    }
    cachesInt.clear();
    cachesLong.clear();
    cachesLongLong.clear();
}

}
