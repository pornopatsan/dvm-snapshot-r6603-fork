#pragma once

#include <cassert>
#include <cstring>
#include <set>
#include <string>
#include <map>
#include <vector>

#include <pthread.h>

#include "dvmh_types.h"
#include "dvmh_async.h"

namespace libdvmh {

#ifndef CPU_EQUAL
#define CPU_EQUAL(s1, s2) (memcmp(s1, s2, sizeof(cpu_set_t)) == 0)
#endif
#ifndef CPU_OR
#define CPU_OR(d, s1, s2) cpuSetOr(d, s1, s2);
#endif
void cpuSetOr(cpu_set_t *d, const cpu_set_t *s1, const cpu_set_t *s2);

bool setAffinity(cpu_set_t *pMask);
bool getAffinity(cpu_set_t *pMask);
std::string affinityToStr(cpu_set_t *pMask);
void applyAffinityPermutation(cpu_set_t *a, int affinityPerm[], int totalProcessors);

// Device == memory space
// Device types
enum DeviceType {dtHost = 0, dtCuda = 1, DEVICE_TYPES};

class CommonDevice;

class DvmhPerformer: private Uncopyable {
public:
    CommonDevice *getDevice() const { return device; }
    int getOwnIndex() const { return selfIndex; }
    cpu_set_t *getAffinity() { return &affinity; }
    bool isStarted() const { MutexGuard guard(mut); return started; }
public:
    explicit DvmhPerformer(CommonDevice *dev, int ind);
public:
    void start(TaskQueue *taskQueue);
    void softStop();
public:
    ~DvmhPerformer();
protected:
    static void *threadFunc(void *arg);
protected:
    CommonDevice *device;
    TaskQueue *q;
    int selfIndex;
    pthread_t thread;
    cpu_set_t affinity;

    DvmhSpinLock lock;
    bool shouldStop;
    mutable DvmhMutex mut;
    bool started;
    DvmhCondVar threadStarted;
};

class CommonDevice: private Uncopyable {
public:
    DeviceType getType() const { return deviceType; }
    void setPerformance(double perf) { devicePerformance = perf; }
    double getPerformance() const { return devicePerformance; }
    int getSlotCount() const { return slotCount; }
    bool hasSlots() const { return slotCount > 0; }
    DvmhPerformer *getPerformer(int i) { return performers[i]; }
public:
    explicit CommonDevice(DeviceType aDeviceType, int aSlotCount, bool separateTaskQueues = false);
public:
    void startPerformers() {
        for (int i = 0; i < slotCount; i++)
            performers[i]->start(performerTaskQueues[i]);
    }
    virtual void setup() = 0;
    virtual char *allocBytes(UDvmType memNeeded, UDvmType alignment = 1) = 0;
    virtual void dispose(void *addr) = 0;
    virtual UDvmType memLeft() = 0;
    virtual void setValue(void *devAddr, const void *srcAddr, UDvmType size) = 0;
    virtual void getValue(const void *devAddr, void *dstAddr, UDvmType size) = 0;
    virtual void memCopy(void *dstAddr, const void *srcAddr, UDvmType size) = 0;

    template <typename T>
    T *alloc(UDvmType elemCount, UDvmType alignment = 1) {
        return (T *)this->allocBytes(elemCount * sizeof(T), alignment);
    }
    template <typename T>
    void setValues(T *devAddr, const T *srcAddr, UDvmType elemCount) {
        this->setValue(devAddr, srcAddr, elemCount * sizeof(T));
    }
    template <typename T>
    void getValues(const T *devAddr, T *dstAddr, UDvmType elemCount) {
        this->getValue(devAddr, dstAddr, elemCount * sizeof(T));
    }

    void barrier();
    void commitTask(Executable *task, int forPerformer = -1);
public:
    virtual ~CommonDevice();
protected:
    DeviceType deviceType;
    double devicePerformance;
    int slotCount;
    bool separateQueues;
    DvmhPerformer **performers; /*Array of pointers to DvmhPerformer (length=slotCount)*/

    TaskQueue **performerTaskQueues; /*Array of pointers to TaskQueue (length=slotCount)*/
private:
    friend class DvmhPerformer;
};

class HostDevice: public CommonDevice {
public:
    HostDevice(int rank);
public:
    virtual void setup() {}
    virtual char *allocBytes(UDvmType memNeeded, UDvmType alignment = 1);
    virtual void dispose(void* addr);
    virtual UDvmType memLeft() { return UDVMTYPE_MAX; }
    virtual void setValue(void *devAddr, const void *srcAddr, UDvmType size);
    virtual void getValue(const void *devAddr, void *dstAddr, UDvmType size);
    virtual void memCopy(void *dstAddr, const void *srcAddr, UDvmType size);
public:
    bool pageLock(void *addr, UDvmType size);
    void pageUnlock(void *addr);
public:
    virtual ~HostDevice();
protected:
    std::map<void *, UDvmType> pageLocked;
};

class CudaDevice: public CommonDevice {
public:
    // Which device
    int index;

    // Settings
    bool blockingSync;
    bool preferL1;

    // Info
    bool canMapHostMemory;
    bool unifiedAddressing;
    unsigned sharedPerSM;
    unsigned regsPerSM;
    unsigned maxGridSize[3];
    unsigned maxBlockSize[3];
    unsigned maxThreadsPerBlock;
    unsigned maxThreadsPerSM;
    unsigned maxBlocksPerSM;
    unsigned maxWarpsPerSM;
    unsigned warpSize;
    unsigned maxRegsPerThread;
    unsigned regsGranularity;
    unsigned sharedGranularity;
    unsigned canAccessPeers; // Bitmask
    unsigned SMcount;
public:
    bool isMaster() const { return masterDevice == 0; }
public:
    explicit CudaDevice(int aIndex);
public:
    virtual void setup();
    virtual char *allocBytes(UDvmType memNeeded, UDvmType alignment = 1);
    virtual void dispose(void *addr);
    virtual UDvmType memLeft();
    virtual void setValue(void *devAddr, const void *srcAddr, UDvmType size);
    virtual void getValue(const void *devAddr, void *dstAddr, UDvmType size);
    virtual void memCopy(void *dstAddr, const void *srcAddr, UDvmType size);
    void deviceSynchronize();
    void setAsCurrent() const;
    bool setupKernel(const void *addr);
public:
    virtual ~CudaDevice();
protected:
    void renewMemoryLeft();
protected:
    DvmhSpinLock lock;
    UDvmType memoryLeft;
    std::map<UDvmType, std::vector<void *> > freePieces;
    std::map<void *, UDvmType> allocatedPieces;
    UDvmType memInFreePieces;
    std::string name;
    std::set<const void *> kernelsSetupped;
    CudaDevice *masterDevice;
};

extern std::vector<CommonDevice *> devices;
extern int devicesCount;

}
