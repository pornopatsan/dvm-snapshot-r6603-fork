#pragma once

#include <cassert>
#include <algorithm>
#include <map>
#include <queue>
#include <vector>

#include <pthread.h>
#include <sched.h>

#include "dvmh_log.h"
#include "util.h"

namespace libdvmh {

class ResourcesSpec {
public:
    enum ResourceType {rtSlotCount = 0, rtCount};
public:
    void setResource(ResourceType rt, int val) { assert(val >= 0); vals[(int)rt] = val; }
public:
    ResourcesSpec() {
        for (int i = 0; i < rtCount; i++)
            vals[i] = 0;
    }
public:
    bool hasResources(const ResourcesSpec &needed) const;
    bool tryGrabResources(const ResourcesSpec &needed);
    void grabResources(const ResourcesSpec &needed) {
        checkInternal(hasResources(needed) == true);
        for (int i = 0; i < rtCount; i++)
            vals[i] -= needed.vals[i];
    }
    void addResources(const ResourcesSpec &returned) {
        for (int i = 0; i < rtCount; i++)
            vals[i] += returned.vals[i];
    }
    void expandTo(const ResourcesSpec &other) {
        for (int i = 0; i < rtCount; i++)
            vals[i] = std::max(vals[i], other.vals[i]);
    }
    bool empty() const;
protected:
    int vals[(int)rtCount];
};

class Executable {
public:
    ResourcesSpec getResNeeded() { return resNeeded; }
public:
    virtual void execute() = 0;
    virtual void execute(void *) = 0;
public:
    virtual ~Executable() {}
protected:
    ResourcesSpec resNeeded;
};

class DummyExecutable: public Executable {
public:
    virtual void execute() {}
    virtual void execute(void *) {}
};

class MethodExecutor {
protected:
    class FunctionExecutor0: public Executable {
    public:
        explicit FunctionExecutor0(void (*f)()): f(f) {}
    public:
        virtual void execute() { (*f)(); }
        virtual void execute(void *) { execute(); }
    protected:
        void (*f)();
    };
    template <class T>
    class MethodExecutor0: public Executable {
    public:
        explicit MethodExecutor0(T *obj, void (T::*f)()): obj(obj), f(f) {}
    public:
        virtual void execute() { (obj->*f)(); }
        virtual void execute(void *) { execute(); }
    protected:
        T *obj;
        void (T::*f)();
    };
    template <class T, typename ParamT>
    class MethodExecutor1: public Executable {
    public:
        explicit MethodExecutor1(T *obj, void (T::*f)(ParamT), ParamT param): obj(obj), f(f), param(param) {}
    public:
        virtual void execute() { (obj->*f)(param); }
        virtual void execute(void *) { execute(); }
    protected:
        T *obj;
        void (T::*f)(ParamT);
        ParamT param;
    };
public:
    static Executable *create(void (*f)()) {
        return new FunctionExecutor0(f);
    }
    template <class T>
    static Executable *create(T *obj, void (T::*f)()) {
        return new MethodExecutor0<T>(obj, f);
    }
    template <class T, typename ParamT>
    static Executable *create(T *obj, void (T::*f)(ParamT), ParamT param) {
        return new MethodExecutor1<T, ParamT>(obj, f, param);
    }
};

class DvmhSpinLock: private Uncopyable {
public:
#ifndef __APPLE__
    DvmhSpinLock() { checkInternal(pthread_spin_init(&myLock, PTHREAD_PROCESS_PRIVATE) == 0); }
#else
    DvmhSpinLock() { __asm__ __volatile__ ("" ::: "memory"); myLock = 0; }
#endif
public:
#ifndef __APPLE__
    void lock() { checkInternal(pthread_spin_lock(&myLock) == 0); }
#else
    void lock();
#endif
#ifndef __APPLE__
    void unlock() { checkInternal(pthread_spin_unlock(&myLock) == 0); }
#else
    void unlock() { __asm__ __volatile__ ("" ::: "memory"); myLock = 0; }
#endif
public:
#ifndef __APPLE__
    ~DvmhSpinLock() { checkInternal(pthread_spin_destroy(&myLock) == 0); }
#else
    ~DvmhSpinLock() {}
#endif
protected:
#ifndef __APPLE__
    pthread_spinlock_t myLock;
#else
    int myLock;
#endif
};

class DvmhMutex: private Uncopyable {
public:
    explicit DvmhMutex(bool recursive = false);
public:
    void lock() { checkInternal(pthread_mutex_lock(&myLock) == 0); }
    void unlock() { checkInternal(pthread_mutex_unlock(&myLock) == 0); }
public:
    ~DvmhMutex() { checkInternal(pthread_mutex_destroy(&myLock) == 0); }
protected:
    pthread_mutex_t myLock;
private:
    friend class DvmhCondVar;
};

class DvmhCondVar: private Uncopyable {
public:
    DvmhMutex *getMutex() const { return mut; }
    void setMutex(DvmhMutex *aMut) { assert(aMut); mut = aMut; }
public:
    explicit DvmhCondVar(DvmhMutex *aMut = 0): mut(aMut) {
        checkInternal(pthread_cond_init(&myCond, 0) == 0);
    }
public:
    void wait(DvmhMutex *aMut = 0) { checkInternal(pthread_cond_wait(&myCond, (aMut ? &aMut->myLock : &mut->myLock)) == 0); }
    void signal() { checkInternal(pthread_cond_signal(&myCond) == 0); }
    void broadcast() { checkInternal(pthread_cond_broadcast(&myCond) == 0); }
public:
    ~DvmhCondVar() { checkInternal(pthread_cond_destroy(&myCond) == 0); }
protected:
    pthread_cond_t myCond;
    DvmhMutex *mut;
};

template <class T>
class LockGuard: private Uncopyable {
public:
    LockGuard(T &lock): myLock(lock) { myLock.lock(); }
public:
    ~LockGuard() { myLock.unlock(); }
protected:
    T &myLock;
};

typedef LockGuard<DvmhSpinLock> SpinLockGuard;
typedef LockGuard<DvmhMutex> MutexGuard;

class BulkTask;
class DependentTask;

class DvmhEvent: private Uncopyable {
public:
    DvmhEvent() {}
public:
    virtual bool addDependent(DependentTask *task) = 0;
    virtual bool isSet() = 0;
    bool wait();
    virtual DvmhEvent *dup();
public:
    virtual ~DvmhEvent() {}
};

class HappenedEvent: public DvmhEvent {
public:
    virtual bool addDependent(DependentTask *task) { return false; }
    virtual bool isSet() { return true; }
    virtual HappenedEvent *dup() { return new HappenedEvent(); }
};

class TaskEndEvent: public DvmhEvent {
public:
    explicit TaskEndEvent(BulkTask *task);
public:
    virtual bool addDependent(DependentTask *task) {
        assert(task);
        return link->addDependent(task);
    }
    virtual bool isSet() {
        return !link->isTaskActive();
    }
    virtual TaskEndEvent *dup() {
        return new TaskEndEvent(link);
    }
public:
    ~TaskEndEvent() {
        link->eventIsDead();
    }
protected:
    class DependencyLink {
    public:
        bool isTaskActive() const { return taskIsActive; }
    public:
        DependencyLink() {
            taskIsActive = true;
            eventsAlive = 1;
        }
    public:
        bool addDependent(DependentTask *task);
        void taskIsDone();
        void eventIsDead();
        void addNewEvent();
    protected:
        ~DependencyLink() {
            assert(!taskIsActive && eventsAlive == 0 && dependents.empty());
        }
    protected:
        DvmhMutex mut;
        bool taskIsActive;
        int eventsAlive;
        HybridVector<DependentTask *, 10> dependents;
    };
protected:
    explicit TaskEndEvent(DependencyLink *link): link(link) {
        link->addNewEvent();
    }
protected:
    DependencyLink *link;
};

class AggregateEvent: public DvmhEvent {
public:
    AggregateEvent(): canAddEvent(true) {}
    explicit AggregateEvent(DvmhEvent *event, bool owning = true): canAddEvent(true) {
        addEvent(event, owning);
    }
public:
    void addEvent(DvmhEvent *event, bool owning = true) {
        SpinLockGuard guard(lock);
        checkInternal(canAddEvent);
        events.push_back(std::make_pair(event, owning));
    }
    void addTaskEndEvent(BulkTask *task) {
        addEvent(new TaskEndEvent(task));
    }
    void freeze() {
        canAddEvent = false;
    }
    virtual bool addDependent(DependentTask *task);
    virtual bool isSet();
public:
    virtual ~AggregateEvent();
protected:
    DvmhSpinLock lock;
    bool canAddEvent;
    HybridVector<std::pair<DvmhEvent *, bool>, 10> events;
};

class BulkTask: public Executable, private Uncopyable {
public:
    void setAutoDelete(bool val) { checkInternal(!executed); autoDelete = val; }
public:
    BulkTask(): executed(false), frozen(false), autoDelete(false) {}
public:
    void prependTask(Executable *task, bool owning = true) {
        addToList(prependList, task, owning);
    }
    void appendTask(Executable *task, bool owning = true) {
        addToList(appendList, task, owning);
    }
    void addFinishTask(Executable *task, bool owning = true) {
        addToList(finishList, task, owning);
    }
    TaskEndEvent *createEndEvent() {
        checkInternal(!frozen);
        return new TaskEndEvent(this);
    }
    void freeze() {
        frozen = true;
    }
    virtual void execute() {
        executeInternal();
    }
    virtual void execute(void *param) {
        executeInternal(true, param);
    }
public:
    virtual ~BulkTask();
protected:
    typedef HybridVector<std::pair<Executable *, bool>, 10> ListType;
protected:
    void addToList(ListType &list, Executable *task, bool owning);
    void processList(ListType &list, bool forwardOrder, bool execute, bool paramSpecified = false, void *param = 0);
    void executeInternal(bool paramSpecified = false, void *param = 0);
protected:
    ListType prependList;
    ListType appendList;
    ListType finishList;
    bool executed;
    bool frozen;
    bool autoDelete;
};

class DependentTask: public BulkTask {
public:
    DependentTask() {
        dependenciesLeft = 0;
        noDependencies.setMutex(&mut);
        autoExec = false;
    }
public:
    void addReadyTask(Executable *task, bool owning = true);
    bool canExecute() const {
        checkInternal(!executed);
        MutexGuard guard(mut);
        return dependenciesLeft == 0;
    }
    void addDependency(DvmhEvent *event) {
        event->addDependent(this);
    }
    int addDependency() {
        checkInternal(!executed && !frozen);
        MutexGuard guard(mut);
        return ++dependenciesLeft;
    }
    int removeDependency();
    void waitCanExecute();
    void setAutoExec(bool val);
    virtual void execute() {
        waitCanExecute();
        executeInternal();
    }
    virtual void execute(void *param) {
        waitCanExecute();
        executeInternal(true, param);
    }
public:
    virtual ~DependentTask();
protected:
    mutable DvmhMutex mut;
    int dependenciesLeft;
    DvmhCondVar noDependencies;
    ListType readyList;
    bool autoExec;
};

class TaskQueue: private Uncopyable {
public:
    explicit TaskQueue(const ResourcesSpec &resSpec = ResourcesSpec(), int maxAccumulate = -1);
public:
    bool tryCommitTask(Executable *task);
    void commitTask(Executable *task);
    bool tryGrabTask(Executable *&task);
    Executable *grabTask();
    void returnResources(const ResourcesSpec &ress);
    void waitSleepingGrabbers(int count);
    void addDefaultPerformer(void *passArg, Executable *startTask, Executable *endTask, pthread_t *pThread = 0);
    void discardOnePerformer() {
        addTaskInternal(0);
    }
public:
    ~TaskQueue();
protected:
    bool haveConformingTask() const {
        return !readyTasks.empty() && (!readyTasks.front() || availResources.hasResources(readyTasks.front()->getResNeeded()));
    }
    bool needToWakeGrabber() const { return haveConformingTask() && grabberSleeperCount > 0; }
    void addTaskInternal(Executable *task);
    Executable *getTaskInternal();
    void taskIsReady(UDvmType key);
protected:
    DvmhMutex mut;
    DvmhCondVar havePlace;
    DvmhCondVar haveTask;
    DvmhCondVar taskGrabbed;
    DvmhCondVar newSleeper;
    int grabberSleeperCount;
    int committerSleeperCount;
    int barrierSleeperCount;
    std::queue<Executable *> readyTasks;
    std::map<UDvmType, DependentTask *> waitingTasks;
    UDvmType incomingCounter;
    int maxSize;
    ResourcesSpec allResources;
    ResourcesSpec availResources;
};

class AsyncDirectory {
public:
    explicit AsyncDirectory(): nextId(1) {}
public:
    UDvmType getTicketForTaskEnd(BulkTask *task);
    UDvmType getTicketForEvent(DvmhEvent *event);
    DvmhEvent *getEventForTicket(UDvmType id);
    void waitForTicket(UDvmType id);
    void waitAllTill(UDvmType lastId);
    void waitAll();
protected:
    void deleteTicket(UDvmType id);
    DvmhEvent *dupEventFor(UDvmType id);
protected:
    UDvmType nextId;
    std::map<UDvmType, DvmhEvent *> idToEvent;
    DvmhSpinLock lock;
};

extern AsyncDirectory asyncDirectory;

extern THREAD_LOCAL bool isMainThread;

}
