#include "dvmh_device.h"

#include <cassert>
#include <cstring>

#ifdef HAVE_CUDA
#pragma GCC visibility push(default)
#include <cuda_runtime.h>
#pragma GCC visibility pop
#endif
#if defined(WIN32) || defined(__CYGWIN__)
#pragma GCC visibility push(default)
#define NOMINMAX
#include <windows.h>
#pragma GCC visibility pop
#endif

#include "cuda_device.h"
#include "dvmh_log.h"
#include "dvmh_stat.h"
#include "settings.h"

namespace libdvmh {

void cpuSetOr(cpu_set_t *d, const cpu_set_t *s1, const cpu_set_t *s2) {
    for (int i = 0; i < (int)sizeof(cpu_set_t); i++)
        ((unsigned char *)d)[i] = ((const unsigned char *)s1)[i] | ((const unsigned char *)s2)[i];
}

bool setAffinity(cpu_set_t *pMask) {
    bool res = false;
#if defined(WIN32) || defined(__CYGWIN__)
    DWORD_PTR cpuset = *pMask;
    if (SetThreadAffinityMask(GetCurrentThread(), cpuset) != 0)
        res = true;
#elif defined(__APPLE__)
    // XXX: OSX has no such mechanism
#else
    if (pthread_setaffinity_np(pthread_self(), sizeof(*pMask), pMask) == 0)
        res = true;
#endif
    return res;
}

bool getAffinity(cpu_set_t *pMask) {
    bool res = false;
#if defined(WIN32) || defined(__CYGWIN__)
    DWORD_PTR tmp = 1, prev;
    prev = SetThreadAffinityMask(GetCurrentThread(), tmp);
    if (prev) {
        if (prev != tmp)
            SetThreadAffinityMask(GetCurrentThread(), prev);
        *pMask = prev;
        res = true;
    }
#elif defined(__APPLE__)
    // XXX: OSX has no such mechanism
    CPU_ZERO(pMask);
#else
    if (pthread_getaffinity_np(pthread_self(), sizeof(*pMask), pMask) == 0)
        res = true;
#endif
    return res;
}

std::string affinityToStr(cpu_set_t *pMask) {
    std::string res;
    res.resize(sizeof(cpu_set_t) * CHAR_BIT * 7 + 10, ' ');
    char *s = (char *)res.c_str();
    int cpuCount = 0;
    for (int i = 0; i < (int)sizeof(cpu_set_t) * CHAR_BIT; i++) {
        if (CPU_ISSET(i, pMask)) {
            s += sprintf(s, "%s%d", (cpuCount > 0 ? ", " : ""), i);
            cpuCount++;
        }
    }
    res.resize(s - res.c_str());
    return res;
}

void applyAffinityPermutation(cpu_set_t *a, int affinityPerm[], int totalProcessors) {
    cpu_set_t tmp;
    CPU_ZERO(&tmp);
    for (int i = 0; i < totalProcessors; i++)
        if (CPU_ISSET(i, a))
            CPU_SET(affinityPerm[i], &tmp);
    CPU_ZERO(a);
    for (int i = 0; i < totalProcessors; i++)
        if (CPU_ISSET(i, &tmp))
            CPU_SET(i, a);
}

std::vector<CommonDevice *> devices; //Array of pointers to Devices (length=devicesCount)
int devicesCount = 0;

// DvmhPerformer

DvmhPerformer::DvmhPerformer(CommonDevice *dev, int ind) {
    device = dev;
    selfIndex = ind;
    started = false;
    shouldStop = false;
    q = 0;
    CPU_ZERO(&affinity);
    threadStarted.setMutex(&mut);
}

void DvmhPerformer::start(TaskQueue *taskQueue) {
    assert(taskQueue);
    MutexGuard guard(mut);
    assert(!started);
    {
        SpinLockGuard guard(lock);
        shouldStop = false;
    }
    q = taskQueue;
    checkInternal(pthread_create(&thread, 0, &threadFunc, this) == 0);
    while (!started)
        threadStarted.wait();
}

void DvmhPerformer::softStop() {
    MutexGuard guard(mut);
    if (started) {
        SpinLockGuard guard(lock);
        shouldStop = true;
    }
}

DvmhPerformer::~DvmhPerformer() {
    if (isStarted()) {
        softStop();
        checkInternal(pthread_join(thread, 0) == 0);
    }
    assert(!isStarted());
}

void *DvmhPerformer::threadFunc(void *arg) {
    DvmhPerformer *self = (DvmhPerformer *)arg;
    CommonDevice *device = self->device;
    int deviceIndex = -1;
    for (int i = 0; i < devicesCount; i++)
        if (devices[i] == device) {
            deviceIndex = i;
            break;
        }
    int selfIndex = self->selfIndex;
    bool masterFlag = selfIndex == 0;
    {
        char threadName[64];
        threadName[sprintf(threadName, "dev%dperf%d", deviceIndex, selfIndex)] = 0;
        DvmhLogger::setThreadName(threadName);
    }
    dvmh_log(DEBUG, "Performer %d on device %d started", selfIndex, deviceIndex);

    // Set affinity
    cpu_set_t zeroSet;
    CPU_ZERO(&zeroSet);
    if (!CPU_EQUAL(self->getAffinity(), &zeroSet)) {
        dvmh_log(DEBUG, "Setting affinity for performer #%d on device #%d to use set of processors {%s}", selfIndex, deviceIndex,
                affinityToStr(self->getAffinity()).c_str());
        setAffinity(self->getAffinity());
    }

    if (masterFlag)
        device->setup();

    // Signal started
    {
        MutexGuard guard(self->mut);
        self->started = true;
        self->threadStarted.broadcast();
    }
    for (;;) {
        {
            SpinLockGuard guard(self->lock);
            if (self->shouldStop)
                break;
        }
        Executable *task = self->q->grabTask();
        if (!task)
            break;
        ResourcesSpec ress = task->getResNeeded();
        task->execute(self);
        delete task;
        self->q->returnResources(ress);
    }
    // Signal stopped
    {
        MutexGuard guard(self->mut);
        self->started = false;
    }
    return 0;
}

// CommonDevice

CommonDevice::CommonDevice(DeviceType aDeviceType, int aSlotCount, bool separateTaskQueues) {
    deviceType = aDeviceType;
    devicePerformance = 1;
    slotCount = aSlotCount;
    assert(slotCount >= 0);
    separateQueues = separateTaskQueues;
    performers = new DvmhPerformer *[slotCount];
    for (int i = 0; i < slotCount; i++)
        performers[i] = new DvmhPerformer(this, i);
    performerTaskQueues = new TaskQueue *[slotCount];
    if (separateQueues) {
        ResourcesSpec ress;
        ress.setResource(ResourcesSpec::rtSlotCount, 1);
        for (int i = 0; i < slotCount; i++)
            performerTaskQueues[i] = new TaskQueue(ress);
    } else {
        ResourcesSpec ress;
        ress.setResource(ResourcesSpec::rtSlotCount, slotCount);
        for (int i = 0; i < slotCount; i++) {
            if (i == 0)
                performerTaskQueues[i] = new TaskQueue(ress);
            else
                performerTaskQueues[i] = performerTaskQueues[0];
        }
    }
}

void CommonDevice::barrier() {
    if (!separateQueues) {
        if (slotCount > 0)
            performerTaskQueues[0]->waitSleepingGrabbers(slotCount);
    } else {
        for (int i = 0; i < slotCount; i++)
            performerTaskQueues[i]->waitSleepingGrabbers(1);
    }
}

void CommonDevice::commitTask(Executable *task, int forPerformer) {
    if (forPerformer < 0) {
        assert(slotCount > 0);
        performerTaskQueues[0]->commitTask(task);
    } else {
        assert(forPerformer < slotCount);
        performerTaskQueues[forPerformer]->commitTask(task);
    }
}

CommonDevice::~CommonDevice() {
    if (separateQueues) {
        for (int i = 0; i < slotCount; i++) {
            if (performers[i]->isStarted())
                performerTaskQueues[i]->waitSleepingGrabbers(1);
            delete performerTaskQueues[i];
        }
    } else {
        int startedCount = 0;
        for (int i = 0; i < slotCount; i++)
            startedCount += performers[i]->isStarted() == true;
        if (slotCount > 0) {
            performerTaskQueues[0]->waitSleepingGrabbers(startedCount);
            delete performerTaskQueues[0];
        }
    }
    delete[] performerTaskQueues;
    for (int i = 0; i < slotCount; i++)
        delete performers[i];
    delete[] performers;
}

// HostDevice

HostDevice::HostDevice(int rank): CommonDevice(dtHost, dvmhSettings.getThreads(rank), true) {
    devicePerformance = dvmhSettings.getCpuPerf(rank);
}

char *HostDevice::allocBytes(UDvmType memNeeded, UDvmType alignment) {
    if (memNeeded == 0)
        return 0;
    char *res;
    if (alignment <= 1) {
        res = (char *)malloc(memNeeded);
    } else {
#ifndef WIN32
        if (posix_memalign((void **)&res, alignment, memNeeded) != 0)
            res = 0;
#else
        // We do not use _aligned_malloc since it requires _aligned_free, what is a problem.
        res = (char *)malloc(memNeeded);
#endif
    }
    checkInternal3(res, "Memory allocation failed (" UDTFMT " bytes requested)", memNeeded);
    dvmh_log(TRACE, "Allocated " UDTFMT " bytes on CPU on %p", memNeeded, res);
    return res;
}

void HostDevice::dispose(void *addr) {
    pageUnlock(addr);
    free(addr);
    dvmh_log(TRACE, "Deallocated %p on CPU", addr);
}

void HostDevice::setValue(void *devAddr, const void *srcAddr, UDvmType size) {
    memcpy(devAddr, srcAddr, size);
}

void HostDevice::getValue(const void *devAddr, void *dstAddr, UDvmType size) {
    memcpy(dstAddr, devAddr, size);
}

void HostDevice::memCopy(void *dstAddr, const void *srcAddr, UDvmType size) {
    memcpy(dstAddr, srcAddr, size);
}

bool HostDevice::pageLock(void *addr, UDvmType size) {
    if (size == 0)
        return true;
#ifdef HAVE_CUDA
    std::map<void *, UDvmType>::iterator it = pageLocked.find(addr);
    if (it == pageLocked.end() || it->second < size) {
        cudaError_t err = cudaHostRegister(addr, size, cudaHostRegisterPortable);
        if (err == cudaSuccess) {
            pageLocked[addr] = size;
            return true;
        } else
            return false;
    } else
        return true;
#else
    return false;
#endif
}

void HostDevice::pageUnlock(void* addr) {
#ifdef HAVE_CUDA
    if (pageLocked.find(addr) != pageLocked.end()) {
        checkInternalCuda(cudaHostUnregister(addr));
        pageLocked.erase(addr);
    }
#endif
}

HostDevice::~HostDevice() {
    checkInternal2(pageLocked.empty(), "Deleteing host device in inconsistent state");
}

// CudaDevice

#ifdef HAVE_CUDA
class DeviceChanger {
public:
    explicit DeviceChanger(int index) {
        checkInternalCuda(cudaGetDevice(&devSave));
        if (devSave != index)
            checkInternalCuda(cudaSetDevice(index));
        else
            devSave = -1;
    }
public:
    ~DeviceChanger() {
        if (devSave >= 0)
            checkInternalCuda(cudaSetDevice(devSave));
    }
protected:
    int devSave;
};
#endif

CudaDevice::CudaDevice(int aIndex): CommonDevice(dtCuda, 1), index(aIndex), memoryLeft(0), memInFreePieces(0) {
    devicePerformance = dvmhSettings.getCudaPerf(index);
    blockingSync = false;
    preferL1 = !dvmhSettings.cudaPreferShared;
    canAccessPeers = 0;
}

void CudaDevice::setup() {
#ifdef HAVE_CUDA
    struct cudaDeviceProp cudaProp;
    checkInternalCuda(cudaGetDeviceProperties(&cudaProp, index));
    name = std::string(cudaProp.name);
    canMapHostMemory = cudaProp.canMapHostMemory != 0;
    if (dvmhSettings.noDirectCopying)
        canMapHostMemory = false;
    unifiedAddressing = cudaProp.unifiedAddressing != 0;
    sharedPerSM = cudaProp.sharedMemPerBlock;
    regsPerSM = cudaProp.regsPerBlock;
    dvmh_log(DEBUG, "On CUDA device %d: sharedPerSM=%d, regsPerSM=%d, canMapHostMemory=%d", index, sharedPerSM, regsPerSM, (int)canMapHostMemory);
    for (int j = 0; j < 3; j++) {
        maxGridSize[j] = cudaProp.maxGridSize[j];
        maxBlockSize[j] = cudaProp.maxThreadsDim[j];
    }
    maxThreadsPerBlock = cudaProp.maxThreadsPerBlock;
    warpSize = cudaProp.warpSize;
    maxThreadsPerSM = cudaProp.maxThreadsPerMultiProcessor;
    maxWarpsPerSM = cudaProp.maxThreadsPerMultiProcessor / warpSize;
    SMcount = cudaProp.multiProcessorCount;
    if (cudaProp.major == 1) {
        // for CC 1.x
        maxBlocksPerSM = 8;
        maxRegsPerThread = 127;
        regsGranularity = 128;
        sharedGranularity = 256;
    } else if (cudaProp.major == 2) {
        // for CC 2.x
        maxBlocksPerSM = 8;
        maxRegsPerThread = 63;
        regsGranularity = 64;
        sharedGranularity = 128;
    } else if (cudaProp.major == 3) {
        // for CC 3.x
        maxBlocksPerSM = 16;
        maxRegsPerThread = 255;
        regsGranularity = 256;
        sharedGranularity = 512;
    } else if (cudaProp.major == 5) {
        // for CC 5.x
        maxBlocksPerSM = 32;
        maxRegsPerThread = 255;
        regsGranularity = 256;
        sharedGranularity = 512;
    } else {
        // unknown for future CC
        maxBlocksPerSM = 8;
        maxRegsPerThread = 63;
        regsGranularity = 256;
        sharedGranularity = 512;
    }

    masterDevice = 0;
    for (int i = 0; i < devicesCount && devices[i] != this; i++) {
        if (devices[i]->getType() == dtCuda && ((CudaDevice *)devices[i])->index == index) {
            masterDevice = ((CudaDevice *)devices[i]);
            break;
        }
    }
    if (!masterDevice)
        checkInternalCuda(cudaSetDeviceFlags((blockingSync ? cudaDeviceScheduleBlockingSync : cudaDeviceScheduleYield) | (canMapHostMemory ? cudaDeviceMapHost : 0)));
    checkInternalCuda(cudaSetDevice(index));
    checkInternalCuda(cudaDeviceSetCacheConfig(preferL1 ? cudaFuncCachePreferL1 : cudaFuncCachePreferShared));
    checkInternalCuda(cudaFree(0));
    if (masterDevice) {
        canMapHostMemory = masterDevice->canMapHostMemory;
    } else if (canMapHostMemory) {
        // Check whether it really can map host memory
        HostDevice *hostDev = (HostDevice *)devices[0];
        const int N = 1024 * 1024 * 16;
        int *buf = hostDev->alloc<int>(N, dvmhSettings.pageSize);
        int size = N * sizeof(int);
        bool res = true;
        float timeKernelCopy;
        float timeMemcpyCopy;
        float speedUpH2D;
        float speedUpD2H;
        cudaEvent_t st, en;

        if (hostDev->pageLock(buf, size)) {
            dvmh_log(DEBUG, "For CUDA-device #%d: try to run direct copy test between CPU and GPU with test size %.0f MB", index, size / 1024.0f / 1024.0f);

            checkInternalCuda(cudaEventCreate(&st));
            checkInternalCuda(cudaEventCreate(&en));

            int *devBuf = alloc<int>(N);
            memset(buf, 0, size);

            checkInternalCuda(cudaEventRecord(st, 0));
            setValues(devBuf, buf, N);
            checkInternalCuda(cudaEventRecord(en, 0));
            checkInternalCuda(cudaEventSynchronize(en));
            checkInternalCuda(cudaEventElapsedTime(&timeMemcpyCopy, st, en));

            for (int i = 0; i < N; i++)
                buf[i] = i;

            checkInternalCuda(cudaEventRecord(st, 0));
            res = res && tryDirectCopyHToD(buf, devBuf, size);
            checkInternalCuda(cudaEventRecord(en, 0));
            checkInternalCuda(cudaEventSynchronize(en));
            checkInternalCuda(cudaEventElapsedTime(&timeKernelCopy, st, en));

            speedUpH2D = timeMemcpyCopy / timeKernelCopy;
            dvmh_log(DEBUG, "For CUDA-device #%d: speed up of direct copy from CPU to GPU - %.2f times", index, speedUpH2D);
            memset(buf, 0, size);

            checkInternalCuda(cudaEventRecord(st, 0));
            getValues(devBuf, buf, N);
            checkInternalCuda(cudaEventRecord(en, 0));
            checkInternalCuda(cudaEventSynchronize(en));
            checkInternalCuda(cudaEventElapsedTime(&timeMemcpyCopy, st, en));

            for (int i = 0; i < N; i++)
                res = res && buf[i] == i;
            memset(buf, 0, size);

            checkInternalCuda(cudaEventRecord(st, 0));
            res = res && tryDirectCopyDToH(devBuf, buf, size);
            checkInternalCuda(cudaEventRecord(en, 0));
            checkInternalCuda(cudaEventSynchronize(en));
            checkInternalCuda(cudaEventElapsedTime(&timeKernelCopy, st, en));

            speedUpD2H = timeMemcpyCopy / timeKernelCopy;
            dvmh_log(DEBUG, "For CUDA-device #%d: speed up of direct copy from GPU to CPU - %.2f times", index, speedUpD2H);

            for (int i = 0; i < N; i++)
                res = res && buf[i] == i;

            dispose(devBuf);
            checkInternalCuda(cudaEventDestroy(st));
            checkInternalCuda(cudaEventDestroy(en));
        } else {
            res = false;
        }
        hostDev->dispose(buf);

        res = res && (speedUpH2D > 0.5 || speedUpD2H > 0.5);
        if (!res) {
            dvmh_log(DEBUG, "For CUDA-device #%d: turning off direct copying due to failed test", index);
            canMapHostMemory = false;
        } else {
            dvmh_log(DEBUG, "For CUDA-device #%d: turning on direct copying", index);
        }
    }
    if (!masterDevice) {
        renewMemoryLeft();
        dvmh_stat_set_gpu_info(index, -1, name.c_str());
    }
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

char *CudaDevice::allocBytes(UDvmType memNeeded, UDvmType alignment) {
    if (masterDevice)
        return masterDevice->allocBytes(memNeeded, alignment);
    if (memNeeded == 0)
        return 0;
#ifdef HAVE_CUDA
    char *res;
    SpinLockGuard guard(lock);
    DeviceChanger dc(index);
    std::map<UDvmType, std::vector<void *> >::iterator it = freePieces.find(memNeeded);
    if (it != freePieces.end()) {
        res = (char *)it->second.back();
        it->second.pop_back();
        if (it->second.empty())
            freePieces.erase(it);
        memInFreePieces -= memNeeded;
        memoryLeft -= memNeeded;
        dvmh_log(TRACE, "Reused memory on GPU #%d " UDTFMT " bytes, place=%p, free " UDTFMT " bytes", index, memNeeded, res, memoryLeft);
    } else {
        if (memoryLeft < memInFreePieces + memNeeded)
            renewMemoryLeft();
        if (memoryLeft < memInFreePieces + memNeeded) {
            for (std::map<UDvmType, std::vector<void *> >::iterator it = freePieces.begin(); it != freePieces.end(); it++)
                for (int i = 0; i < (int)it->second.size(); i++)
                    checkInternalCuda(cudaFree(it->second[i]));
            freePieces.clear();
            memInFreePieces = 0;
            renewMemoryLeft();
        }
        checkInternalCuda(cudaMalloc((void **)&res, memNeeded));
        renewMemoryLeft();
        dvmh_log(TRACE, "Allocated memory on GPU #%d " UDTFMT " bytes, place=%p, free " UDTFMT " bytes", index, memNeeded, res, memoryLeft);
    }
    assert(res);
    allocatedPieces[res] = memNeeded;
    return res;
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
    return 0;
#endif
}

void CudaDevice::dispose(void* addr) {
    if (masterDevice) {
        masterDevice->dispose(addr);
        return;
    }
    if (!addr)
        return;
#ifdef HAVE_CUDA
    SpinLockGuard guard(lock);
    DeviceChanger dc(index);
    std::map<void *, UDvmType>::iterator it = allocatedPieces.find(addr);
    assert(it != allocatedPieces.end());
    UDvmType memSize = it->second;
    allocatedPieces.erase(it);
    if (dvmhSettings.cacheGpuAllocations) {
        freePieces[memSize].push_back(addr);
        memInFreePieces += memSize;
        memoryLeft += memSize;
    } else {
        checkInternalCuda(cudaFree(addr));
        renewMemoryLeft();
    }
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

UDvmType CudaDevice::memLeft() {
    if (masterDevice)
        return masterDevice->memLeft();
#ifdef HAVE_CUDA
    SpinLockGuard guard(lock);
    return memoryLeft;
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
    return 0;
#endif
}

void CudaDevice::setValue(void *devAddr, const void *srcAddr, UDvmType size) {
#ifdef HAVE_CUDA
    DeviceChanger dc(index);
    checkInternalCuda(cudaMemcpy(devAddr, srcAddr, size, cudaMemcpyHostToDevice));
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void CudaDevice::getValue(const void *devAddr, void *dstAddr, UDvmType size) {
#ifdef HAVE_CUDA
    DeviceChanger dc(index);
    checkInternalCuda(cudaMemcpy(dstAddr, devAddr, size, cudaMemcpyDeviceToHost));
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void CudaDevice::memCopy(void *dstAddr, const void *srcAddr, UDvmType size) {
#ifdef HAVE_CUDA
    DeviceChanger dc(index);
    checkInternalCuda(cudaMemcpy(dstAddr, srcAddr, size, cudaMemcpyDeviceToDevice));
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void CudaDevice::deviceSynchronize() {
#ifdef HAVE_CUDA
    DeviceChanger dc(index);
    checkInternalCuda(cudaDeviceSynchronize());
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void CudaDevice::setAsCurrent() const {
#ifdef HAVE_CUDA
    checkInternalCuda(cudaSetDevice(index));
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

bool CudaDevice::setupKernel(const void *addr) {
    if (masterDevice)
        return masterDevice->setupKernel(addr);
    return kernelsSetupped.insert(addr).second;
}

CudaDevice::~CudaDevice() {
#ifdef HAVE_CUDA
    checkInternal2(allocatedPieces.empty(), "Deleting CUDA device in inconsistent state");
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void CudaDevice::renewMemoryLeft() {
    assert(!masterDevice);
#ifdef HAVE_CUDA
    size_t total, left;
    checkInternalCuda(cudaMemGetInfo(&left, &total));
    left += memInFreePieces;
    if (left <= UDVMTYPE_MAX)
        memoryLeft = (UDvmType)left;
    else
        memoryLeft = UDVMTYPE_MAX;
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

}
