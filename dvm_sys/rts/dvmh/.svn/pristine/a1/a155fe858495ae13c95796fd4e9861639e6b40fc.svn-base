#include "dvmh_async.h"

namespace libdvmh {

// ResourcesSpec

bool ResourcesSpec::hasResources(const ResourcesSpec &needed) const {
    bool res = true;
    for (int i = 0; i < rtCount; i++)
        res = res && vals[i] >= needed.vals[i];
    return res;
}

bool ResourcesSpec::tryGrabResources(const ResourcesSpec &needed) {
    bool res = hasResources(needed);
    if (res)
        for (int i = 0; i < rtCount; i++)
            vals[i] -= needed.vals[i];
    return res;
}

bool ResourcesSpec::empty() const {
    bool res = true;
    for (int i = 0; i < rtCount; i++)
        res = res && vals[i] == 0;
    return res;
}

// DvmhSpinLock

#ifdef __APPLE__
void DvmhSpinLock::lock() {
    while (true) {
        for (int i = 0; i < 10000; i++) {
            if (__sync_bool_compare_and_swap(&myLock, 0, 1))
                return;
        }
        sched_yield();
    }
}
#endif

// DvmhMutex

DvmhMutex::DvmhMutex(bool recursive) {
    pthread_mutexattr_t attr;
    checkInternal(pthread_mutexattr_init(&attr) == 0);
    if (recursive)
        checkInternal(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0);
    checkInternal(pthread_mutex_init(&myLock, &attr) == 0);
    checkInternal(pthread_mutexattr_destroy(&attr) == 0);
}

// DvmhEvent

bool DvmhEvent::wait() {
    if (this->isSet())
        return false;
    DependentTask *task = new DependentTask();
    bool res = this->addDependent(task);
    if (res)
        task->waitCanExecute();
    delete task;
    return res;
}

DvmhEvent *DvmhEvent::dup() {
    if (this->isSet())
        return new HappenedEvent();
    DependentTask *task = new DependentTask();
    this->addDependent(task);
    DvmhEvent *res = task->createEndEvent();
    task->freeze();
    task->setAutoDelete(true);
    task->setAutoExec(true);
    return res;
}

// TaskEndEvent

TaskEndEvent::TaskEndEvent(BulkTask *task) {
    assert(task);
    link = new DependencyLink();
    task->addFinishTask(MethodExecutor::create(link, &DependencyLink::taskIsDone));
}

bool TaskEndEvent::DependencyLink::addDependent(DependentTask* task) {
    MutexGuard guard(mut);
    if (taskIsActive) {
        dependents.push_back(task);
        task->addDependency();
    }
    return taskIsActive;
}

void TaskEndEvent::DependencyLink::taskIsDone() {
    bool allDone = false;
    {
        MutexGuard guard(mut);
        assert(taskIsActive);
        taskIsActive = false;
        for (int i = 0; i < (int)dependents.size(); i++)
            dependents[i]->removeDependency();
        dependents.clear();
        allDone = !taskIsActive && eventsAlive == 0;
    }
    if (allDone)
        delete this;
}

void TaskEndEvent::DependencyLink::eventIsDead() {
    bool allDone = false;
    {
        MutexGuard guard(mut);
        assert(eventsAlive > 0);
        eventsAlive--;
        allDone = !taskIsActive && eventsAlive == 0;
    }
    if (allDone)
        delete this;
}

void TaskEndEvent::DependencyLink::addNewEvent() {
    MutexGuard guard(mut);
    eventsAlive++;
}

// AggregateEvent

bool AggregateEvent::addDependent(DependentTask *task) {
    SpinLockGuard guard(lock);
    bool res = false;
    for (int i = 0; i < (int)events.size(); i++) {
        bool curRes = events[i].first->addDependent(task);
        if (!curRes) {
            // Exclude useless subordinate event
            if (events[i].second)
                delete events[i].first;
            if (i < (int)events.size() - 1)
                events[i] = events.back();
            events.pop_back();
            i--;
        }
        res = res || curRes;
    }
    canAddEvent = false; // No subordinate event adding after the start of the dependency injection
    return res;
}

bool AggregateEvent::isSet() {
    SpinLockGuard guard(lock);
    bool res = true;
    for (int i = 0; i < (int)events.size() && res; i++) {
        bool curRes = events[i].first->isSet();
        if (curRes) {
            // Exclude useless subordinate event
            if (events[i].second)
                delete events[i].first;
            if (i < (int)events.size() - 1)
                events[i] = events.back();
            events.pop_back();
            i--;
        }
        res = res && curRes;
    }
    canAddEvent = false; // No subordinate event adding after the start of enquiring
    return res;
}

AggregateEvent::~AggregateEvent() {
    for (int i = 0; i < (int)events.size(); i++)
        if (events[i].second)
            delete events[i].first;
}

// BulkTask

BulkTask::~BulkTask() {
    if (!executed) {
        processList(prependList, false, false);
        processList(appendList, true, false);
        processList(finishList, true, true);
    } else {
        assert(prependList.empty());
        assert(appendList.empty());
        assert(finishList.empty());
    }
}

void BulkTask::addToList(ListType &list, Executable *task, bool owning) {
    checkInternal(!executed && !frozen);
    assert(task);
    list.push_back(std::make_pair(task, owning));
    resNeeded.expandTo(task->getResNeeded());
}

void BulkTask::processList(ListType &list, bool forwardOrder, bool executeFlag, bool paramSpecified, void *param) {
    for (int i = 0; i < (int)list.size(); i++) {
        int listI = (forwardOrder ? i : list.size() - i - 1);
        if (executeFlag) {
            if (paramSpecified)
                list[listI].first->execute(param);
            else
                list[listI].first->execute();
        }
        if (list[listI].second)
            delete list[listI].first;
    }
    list.clear();
}

void BulkTask::executeInternal(bool paramSpecified, void *param) {
    checkInternal(!executed);
    processList(prependList, false, true, paramSpecified, param);
    processList(appendList, true, true, paramSpecified, param);
    processList(finishList, true, true, paramSpecified, param);
    executed = true;
    if (autoDelete)
        delete this;
}

// DependentTask

void DependentTask::addReadyTask(Executable *task, bool owning) {
    checkInternal(frozen && !executed);
    MutexGuard guard(mut);
    if (dependenciesLeft == 0) {
        task->execute(this);
        if (owning)
            delete task;
    } else
        readyList.push_back(std::make_pair(task, owning));
}

int DependentTask::removeDependency() {
    checkInternal(!executed);
    int res;
    bool execHere = false;
    {
        MutexGuard guard(mut);
        dependenciesLeft--;
        assert(dependenciesLeft >= 0);
        if (dependenciesLeft == 0) {
            processList(readyList, true, true, true, this);
            noDependencies.broadcast();
        }
        res = dependenciesLeft;
        execHere = autoExec && dependenciesLeft == 0;
    }
    if (execHere)
        execute();
    return res;
}

void DependentTask::waitCanExecute() {
    checkInternal(!executed);
    MutexGuard guard(mut);
    while (dependenciesLeft > 0)
        noDependencies.wait();
}

void DependentTask::setAutoExec(bool val) {
    checkInternal(frozen && !executed);
    bool execHere = false;
    {
        MutexGuard guard(mut);
        autoExec = val;
        execHere = autoExec && dependenciesLeft == 0;
    }
    if (execHere)
        execute();
}

DependentTask::~DependentTask() {
    if (!executed) {
        // Need to wait all the dependencies at least
        waitCanExecute();
    }
    assert(dependenciesLeft == 0);
}

// TaskQueue

TaskQueue::TaskQueue(const ResourcesSpec &resSpec, int maxAccumulate): mut(true) {
    maxSize = maxAccumulate;
    if (maxSize < 0)
        maxSize = -1;
    grabberSleeperCount = 0;
    committerSleeperCount = 0;
    barrierSleeperCount = 0;
    havePlace.setMutex(&mut);
    haveTask.setMutex(&mut);
    taskGrabbed.setMutex(&mut);
    newSleeper.setMutex(&mut);
    allResources = resSpec;
    availResources = allResources;
    incomingCounter = 0;
}

bool TaskQueue::tryCommitTask(Executable *task) {
    assert(task);
    checkInternal(allResources.hasResources(task->getResNeeded()));
    bool res;
    MutexGuard guard(mut);
    if (maxSize == 0) {
        res = waitingTasks.empty() && readyTasks.empty() && availResources.hasResources(task->getResNeeded()) && grabberSleeperCount > 0;
        if (isa<DependentTask>(task))
            res = res && asa<DependentTask>(task)->canExecute();
    } else if (maxSize > 0) {
        res = (int)(waitingTasks.size() + readyTasks.size()) < maxSize;
    } else {
        res = true;
    }
    if (res)
        addTaskInternal(task);
    return res;
}

void TaskQueue::commitTask(Executable *task) {
    assert(task);
    checkInternal(allResources.hasResources(task->getResNeeded()));
    MutexGuard guard(mut);
    if (maxSize >= 0) {
        while (!(waitingTasks.empty() && readyTasks.empty()) && (int)(waitingTasks.size() + readyTasks.size()) >= maxSize) {
            committerSleeperCount++;
            if (barrierSleeperCount > 0)
                newSleeper.broadcast();
            havePlace.wait();
            committerSleeperCount--;
        }
    }
    addTaskInternal(task);
}

bool TaskQueue::tryGrabTask(Executable *&task) {
    bool res;
    MutexGuard guard(mut);
    res = haveConformingTask();
    if (res)
        task = getTaskInternal();
    return res;
}

Executable *TaskQueue::grabTask() {
    MutexGuard guard(mut);
    while (!haveConformingTask()) {
        grabberSleeperCount++;
        if (barrierSleeperCount > 0)
            newSleeper.broadcast();
        haveTask.wait();
        grabberSleeperCount--;
    }
    return getTaskInternal();
}

void TaskQueue::returnResources(const ResourcesSpec &ress) {
    MutexGuard guard(mut);
    availResources.addResources(ress);
    if (needToWakeGrabber())
        haveTask.signal();
    assert(allResources.hasResources(availResources));
}

void TaskQueue::waitSleepingGrabbers(int count) {
    MutexGuard guard(mut);
    while (!readyTasks.empty() || grabberSleeperCount < count) {
        barrierSleeperCount++;
        newSleeper.wait();
        barrierSleeperCount--;
    }
}

struct DefaultPerformerSettings {
    void *passArg;
    Executable *startTask;
    TaskQueue *q;
    Executable *endTask;
};

static void *defaultPerformerFunc(void *arg) {
    DefaultPerformerSettings *sett = (DefaultPerformerSettings *)arg;
    if (sett->startTask) {
        sett->startTask->execute(sett->passArg);
        delete sett->startTask;
    }
    for (;;) {
        Executable *task = sett->q->grabTask();
        if (!task)
            break;
        ResourcesSpec ress = task->getResNeeded();
        task->execute(sett->passArg);
        delete task;
        sett->q->returnResources(ress);
    }
    if (sett->endTask) {
        sett->endTask->execute(sett->passArg);
        delete sett->endTask;
    }
    delete sett;
    return 0;
}

void TaskQueue::addDefaultPerformer(void *passArg, Executable *startTask, Executable *endTask, pthread_t *pThread) {
    pthread_t thread;
    if (!pThread)
        pThread = &thread;
    DefaultPerformerSettings *sett = new DefaultPerformerSettings;
    sett->passArg = passArg;
    sett->startTask = startTask;
    sett->q = this;
    sett->endTask = endTask;
    checkInternal(pthread_create(pThread, 0, &defaultPerformerFunc, sett) == 0);
}

TaskQueue::~TaskQueue() {
    checkInternal(waitingTasks.empty());
    checkInternal(readyTasks.empty());
    checkInternal(committerSleeperCount == 0);
    checkInternal(barrierSleeperCount == 0);
    if (grabberSleeperCount > 0) {
        MutexGuard guard(mut);
        for (int i = 0; i < grabberSleeperCount; i++)
            readyTasks.push(0);
        haveTask.broadcast();
        committerSleeperCount++;
        while (!readyTasks.empty())
            havePlace.wait();
        committerSleeperCount--;
    }
    assert(grabberSleeperCount == 0);
}

void TaskQueue::addTaskInternal(Executable *task) {
    if (task && isa<BulkTask>(task))
        asa<BulkTask>(task)->freeze();
    bool isReady = true;
    if (task && isa<DependentTask>(task)) {
        DependentTask *depTask = asa<DependentTask>(task);
        if (!depTask->canExecute()) {
            isReady = false;
            incomingCounter++;
            waitingTasks.insert(std::make_pair(incomingCounter, depTask));
            depTask->addReadyTask(MethodExecutor::create(this, &TaskQueue::taskIsReady, incomingCounter));
        }
    }
    if (isReady)
        readyTasks.push(task);
    if (needToWakeGrabber())
        haveTask.signal();
    if (maxSize == 0)
        taskGrabbed.wait();
}

Executable *TaskQueue::getTaskInternal() {
    Executable *task = readyTasks.front();
    readyTasks.pop();
    if (task)
        availResources.grabResources(task->getResNeeded());
    if (committerSleeperCount > 0)
        havePlace.signal();
    if (needToWakeGrabber())
        haveTask.signal();
    if (maxSize == 0)
        taskGrabbed.broadcast();
    return task;
}

void TaskQueue::taskIsReady(UDvmType key) {
    MutexGuard guard(mut);
    std::map<UDvmType, DependentTask *>::iterator it = waitingTasks.find(key);
    checkInternal(it != waitingTasks.end());
    readyTasks.push(it->second);
    waitingTasks.erase(it);
    if (needToWakeGrabber())
        haveTask.signal();
}

// AsyncDirectory

UDvmType AsyncDirectory::getTicketForTaskEnd(BulkTask *task) {
    assert(task);
    SpinLockGuard guard(lock);
    UDvmType id = nextId;
    nextId++;
    idToEvent[id] = task->createEndEvent();
    task->addFinishTask(MethodExecutor::create(this, &AsyncDirectory::deleteTicket, id));
    return id;
}

UDvmType AsyncDirectory::getTicketForEvent(DvmhEvent *event) {
    assert(event);
    if (event->isSet())
        return 0;
    DependentTask *task = new DependentTask;
    task->addDependency(event);
    UDvmType id = getTicketForTaskEnd(task);
    task->freeze();
    task->setAutoDelete(true);
    task->setAutoExec(true);
    return id;
}

DvmhEvent *AsyncDirectory::getEventForTicket(UDvmType id) {
    DvmhEvent *ev = dupEventFor(id);
    if (!ev)
        ev = new HappenedEvent();
    return ev;
}

void AsyncDirectory::waitForTicket(UDvmType id) {
    DvmhEvent *ev = dupEventFor(id);
    if (ev) {
        ev->wait();
        delete ev;
    }
}

void AsyncDirectory::waitAllTill(UDvmType lastId) {
    {
        SpinLockGuard guard(lock);
        assert(lastId < nextId);
    }
    for (;;) {
        UDvmType id = 0;
        {
            SpinLockGuard guard(lock);
            if (!idToEvent.empty())
                id = idToEvent.begin()->first;
        }
        if (id == 0 || id > lastId)
            break;
        waitForTicket(id);
    }
}

void AsyncDirectory::waitAll() {
    for (;;) {
        UDvmType id = 0;
        {
            SpinLockGuard guard(lock);
            if (!idToEvent.empty())
                id = idToEvent.rbegin()->first;
        }
        if (id == 0)
            break;
        waitForTicket(id);
    }
}

void AsyncDirectory::deleteTicket(UDvmType id) {
    SpinLockGuard guard(lock);
    assert(id < nextId);
    delete idToEvent[id];
    idToEvent.erase(id);
}

DvmhEvent *AsyncDirectory::dupEventFor(UDvmType id) {
    DvmhEvent *ev = 0;
    {
        SpinLockGuard guard(lock);
        assert(id < nextId);
        std::map<UDvmType, DvmhEvent *>::iterator it = idToEvent.find(id);
        if (it != idToEvent.end())
            ev = it->second->dup();
    }
    return ev;
}

AsyncDirectory asyncDirectory;

THREAD_LOCAL bool isMainThread = false;

class IsMainThreadInitializer {
public:
    IsMainThreadInitializer() { isMainThread = true; }
};

static IsMainThreadInitializer isMainThreadInitializer;

}
