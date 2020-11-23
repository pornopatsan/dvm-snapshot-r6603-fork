#ifndef WIN32
 #ifndef _GNU_SOURCE
  #define _GNU_SOURCE
 #endif
//#define _BSD_SOURCE
//#define _POSIX_C_SOURCE 199309L
#endif

#include "dvmh_rts.h"

#include <cassert>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#ifdef HAVE_CUDA
#pragma GCC visibility push(default)
#include <cuda_runtime.h>
#pragma GCC visibility pop
#endif
#include <pthread.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef HAVE_LIBUNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

#ifndef WIN32
#include <unistd.h>
#ifndef __CYGWIN__
#include <execinfo.h>
#endif
#include <signal.h>
#include <cxxabi.h>
#else
#pragma GCC visibility push(default)
#define NOMINMAX
#include <windows.h>
#pragma GCC visibility pop
#endif

#include "include/dvmhlib2.h"

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_device.h"
#include "dvmh_pieces.h"
#include "dvmh_stat.h"
#include "dvmh_stdio.h"
#ifndef NO_DVM
#include "dvmlib_adapter.h"
#endif
#include "loop.h"
#include "loop_distribution.h"
#include "mps.h"
#include "region.h"
#include "util.h"

using namespace libdvmh;

namespace libdvmh {

#ifdef __APPLE__
#include <crt_externs.h>
char **environ = *_NSGetEnviron();
#endif

MultiprocessorSystem *rootMPS = 0;
MultiprocessorSystem *currentMPS = 0;
DvmhRegion *currentRegion = 0;
DvmhLoop *currentLoop = 0;
bool inited = false;
bool finalized = false;
std::set<DvmhObject *> allObjects; // All user-created arrays and templates
std::vector<char> stringBuffer;
std::vector<std::pair<char *, UDvmType> > stringVariables;
int currentLine = 0;
char currentFile[1024] = {0};
std::map<const void *, RegularVar *> regularVars;
DvmhSpinLock regularVarsLock;

static double timerBase = 0.0;

struct Stone {
    DvmhData *data;
    UDvmType size;
    UDvmType price;
    explicit Stone(DvmhData *aData = 0, UDvmType aSize = 0, UDvmType aPrice = 0): data(aData), size(aSize), price(aPrice) {}
    bool operator<(const Stone &other) const { return double(price) / size < double(other.price) / other.size; }
};

void dvmhTryToFreeSpace(UDvmType memNeeded, int dev, const std::set<DvmhData *> &except) {
    if (dev > 0) {
        std::vector<Stone> stones;
        UDvmType overallSize = 0;
        for (std::set<DvmhObject *>::iterator it = allObjects.begin(); memNeeded > devices[dev]->memLeft() && it != allObjects.end(); it++) {
            DvmhData *data = (*it)->as_s<DvmhData>();
            DvmhRepresentative *repr = (data ? data->getRepr(dev) : 0);
            if (repr && except.find(data) == except.end()) {
                UDvmType size = repr->getBuffer()->getSize();
                DvmhPieces *p = repr->getActualState()->subtract(data->getRepr(0)->getActualState());
                UDvmType price = 0;
                for (int i = 0; i < p->getCount(); i++)
                    price += data->getTypeSize() * p->getPiece(i)->blockSize(data->getRank());
                delete p;
                if (price == 0) {
                    // Nothing to be saved from this representative - delete it
                    data->deleteRepr(dev);
                } else {
                    // There are something to be saved from this representative - collect in stones
                    stones.push_back(Stone(data, size, price));
                    overallSize += size;
                }
            }
        }
        if (memNeeded > devices[dev]->memLeft()) {
            // Knapsack problem
            // Greedy algorithm
            std::sort(stones.begin(), stones.end());
            for (int i = 0; memNeeded > devices[dev]->memLeft() && i < (int)stones.size(); i++) {
                DvmhData *data = stones[i].data;
                data->getActualBase(0, data->getRepr(dev)->getActualState(), 0, 1);
                data->deleteRepr(dev);
            }
        }
        if (memNeeded > devices[dev]->memLeft()) {
            dvmh_log(DEBUG, "Can not free enough space on device #%d, with exception for %d variables. Needed " UDTFMT ", have " UDTFMT, dev,
                    (int)except.size(), memNeeded, devices[dev]->memLeft());
        }
    }
}

char *getStr(DvmType ref, bool clearTrailingBlanks, bool toUpper) {
    if (ref <= 0)
        return 0;
    char *res = &stringBuffer[ref - 1];
    if (clearTrailingBlanks) {
        for (int i = (int)strlen(res) - 1; i >= 0 && isspace(res[i]); i--)
            res[i] = 0;
    }
    if (toUpper) {
        for (int i = 0; res[i]; i++)
            res[i] = toupper(res[i]);
    }
    return res;
}

static void callCoilMPI(int int1, int int2,
        long   long1,   long   long2,
        float  float1,  float  float2,
        double double1, double double2,
        int    *int3,    int    *int4,    int    *int5,
        long   *long3,   long   *long4,   long   *long5,
        float  *float3,  float  *float4,  float  *float5,
        double *double3, double *double4, double *double5)
{
    /* Addition */
    *int3 = int1 + int2;
    *long3 = long1 + long2;
    *float3 = float1 + float2;
    *double3 = double1 + double2;

    /* Multiplication */
    *int4 = int1 * int2;
    *long4 = long1 * long2;
    *float4 = float1 * float2;
    *double4 = double1 * double2;

    /* Division */
    *int5 = int1 / int2;
    *long5 = long1 / long2;
    *float5 = float1 / float2;
    *double5 = double1 / double2;
}
/*
 * Measuring proc power through computing fixed group of arithmetical operations,
 * proc power is time spent on this operations, then proc weight is 1 / proc_power
 */
static double *measureProcPowerMPI(const DvmhCommunicator *comm, LogLevel printLevel) {
    int    int1 = 123,          int2 = 567;
    long   long1 = 1234,        long2 = 4321;
    float  float1 = 0.1234f,    float2 = 54321.0f;
    double double1 = 7654321.0, double2 = 0.1234567;
    int    int3,    int4,    int5;
    long   long3,   long4,   long5;
    float  float3,  float4,  float5;
    double double3, double4, double5;
    double time;
    double minWP;
    UDvmType PPMeasureCount = 10;
    int rank, size;
    double *processPower, meanProcPower, *procWeightArray;

    rank = comm->getCommRank();
    size = comm->getCommSize();

    comm->barrier();
    time = dvmhTime();
    for (UDvmType mperf = 0; mperf < PPMeasureCount; mperf++) {
        callCoilMPI(int1, int2,
                long1,   long2,
                float1,  float2,
                double1, double2,
                &int3,    &int4,    &int5,
                &long3,   &long4,   &long5,
                &float3,  &float4,  &float5,
                &double3, &double4, &double5);
    }
    time = dvmhTime() - time + 1e-6;

    dvmh_log(printLevel, "ProcPowerMeasuring: Proc=%d PPMeasureCount=" UDTFMT " PPMeasureTime=%f", rank, PPMeasureCount, time);

    processPower = new double[size];
    processPower[rank] = time;
    comm->allgather(processPower);

    meanProcPower = 0.0;
    for (int i = 0; i < size; i++)
        meanProcPower += processPower[i];
    meanProcPower /= (double)size;

    procWeightArray = new double[size];
    for (int i = 0; i < size; i++)
        procWeightArray[i] = 1.0 / processPower[i];
    delete[] processPower;

    minWP = procWeightArray[0];
    for (int i = 0; i < size; i++)
        minWP = std::min(minWP, procWeightArray[i]);

    for (int i = 0; i < size; i++)
        procWeightArray[i] /= minWP;

    return procWeightArray;
}

#ifndef WIN32
static struct sigaction fallBackHandlers[32];
static int termSignals[] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGSEGV, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, SIGUSR2, SIGBUS, SIGIO, SIGPROF, SIGSYS,
        SIGTRAP, SIGVTALRM, SIGXCPU, SIGXFSZ};

static void printStackTrace(bool toTheLog = true, LogLevel logLevel = FATAL) {
#ifndef __CYGWIN__
    void *addrArray[128];
    int count = backtrace(addrArray, (int)(sizeof(addrArray) / sizeof(addrArray[0])));
    if (count > 0) {
        if (toTheLog) {
            dvmhLogger.startBlock(logLevel);
            dvmh_log(logLevel, "*** RTS stack trace:");
        } else {
            fprintf(stderr, "*** RTS stack trace:\n");
        }
        char **symbols = backtrace_symbols(addrArray, count);
#ifndef HAVE_LIBUNWIND
        for (int i = 0; i < count; i++) {
            if (!addrArray[i])
                break;
            char atStr[1024];
            atStr[0] = 0;
            char *s;
            if (symbols[i][0] != '[' && (s = strchr(symbols[i], '('))) {
                *s = 0;
                char cmdline[128];
                cmdline[sprintf(cmdline, "addr2line %p -e %s 2>/dev/null", addrArray[i], symbols[i])] = 0;
                FILE *f = popen(cmdline, "r");
                if (f) {
                    char filepos[1024];
                    if (fgets(filepos, sizeof(filepos), f))
                        atStr[sprintf(atStr, " at %s", filepos)] = 0;
                    pclose(f);
                }
                *s = '(';
            }
            if (toTheLog)
                dvmh_log(logLevel, "#%d %s%s", i, symbols[i], atStr);
            else
                fprintf(stderr, "#%d %s%s\n", i, symbols[i], atStr);
        }
#else
        unw_cursor_t cursor;
        unw_context_t uc;
        unw_getcontext(&uc);
        unw_init_local(&cursor, &uc);
        int i = 0;
        do {
            unw_word_t ip, offset;
            unw_get_reg(&cursor, UNW_REG_IP, &ip);
            if (!ip)
                break;
            char fname[1024];
            fname[0] = '\0';
            unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
            char *demangledFN = 0;
            int status;
            demangledFN = abi::__cxa_demangle(fname, 0, 0, &status);
            if (!demangledFN)
                demangledFN = fname;
            char record[2048];
            char *rs = record;
            rs += sprintf(rs, "#%d ", i);
            char *s;
            char filepos[1024];
            filepos[0] = 0;
            if (i < count && symbols[i][0] != '[' && (s = strchr(symbols[i], '('))) {
                *s = 0;
                rs += sprintf(rs, "%s", symbols[i]);
                char cmdline[128];
                cmdline[sprintf(cmdline, "addr2line %p -e %s 2>/dev/null", (void *)ip, symbols[i])] = 0;
                FILE *f = popen(cmdline, "r");
                if (f) {
                    if (!fgets(filepos, sizeof(filepos), f))
                        filepos[0] = 0;
                    pclose(f);
                }
                *s = '(';
            }
            rs += sprintf(rs, "(%s+0x%llx) [%p]", demangledFN, (unsigned long long)offset, (void *)ip);
            if (filepos[0])
                rs += sprintf(rs, " at %s", filepos);
            *rs = 0;
            if (toTheLog)
                dvmh_log(logLevel, "%s", record);
            else
                fprintf(stderr, "%s\n", record);
            if (demangledFN != fname)
                free(demangledFN);
            i++;
        } while (unw_step(&cursor) > 0);
#endif
        if (toTheLog)
            dvmhLogger.endBlock(logLevel, __FILE__, __LINE__);
        free(symbols);
    }
#endif
}

static void termSignalHook(int signum, siginfo_t *info, void *unused) {
    assert(signum >= 0 && signum < (int)(sizeof(fallBackHandlers) / sizeof(fallBackHandlers[0])));
    struct sigaction *oldAct = &fallBackHandlers[signum];
    if (oldAct->sa_flags & SA_RESETHAND)
        signal(signum, SIG_DFL);
    if (oldAct->sa_handler == SIG_IGN) {
        // Nothing
    } else {
        dvmhLogger.flush();
        printStackTrace();
        dvmhLogger.flush();
        if (oldAct->sa_handler == SIG_DFL) {
            sigaction(signum, oldAct, 0);
            kill(getpid(), signum);
        } else {
            if (oldAct->sa_flags & SA_SIGINFO)
                oldAct->sa_sigaction(signum, info, unused);
            else
                oldAct->sa_handler(signum);
        }
    }
}

static void dumpTrace(int signum, siginfo_t *info, void *unused) {
    printStackTrace(true, NFERROR);
    dvmhLogger.flush();

#ifndef NO_DVM
    // LibDVM's system trace
    trace_Dump(MPS_CurrentProc, TraceBufCountArr[0], TraceBufFullArr[0]);
#endif
}
#endif

static void dvmhLoggerAtExitHook() {
    dvmhLogger.flush();
    if (!finalized) {
#ifndef WIN32
        printStackTrace();
#endif
        dvmhLogger.flush();
    }
}

static void initLogging(int procCount, int myGlobalRank, int myNodeIndex, int myLocalRank, int masterGlobalRank) {
    dvmhLogger.useFile(stderr, false, false);
    if (!dvmhSettings.logFile.empty()) {
        if (strchr(dvmhSettings.logFile.c_str(), '%')) {
            char buf[1024];
            buf[0] = 0;
            int rank = myGlobalRank;
#ifdef _MPI_STUBS_
            const char * envR = getMpiRank();
            if (envR && *envR)
            rank = atoi(envR);
#endif
            sprintf(buf, dvmhSettings.logFile.c_str(), rank);
            FILE *f = fopen(buf, "wt");
            if (f)
                dvmhLogger.useFile(f, true);
        } else {
            FILE *f = fopen(dvmhSettings.logFile.c_str(), "wt");
            if (f)
                dvmhLogger.useFile(f, false);
        }
    }
    dvmhLogger.setMasterFlag(myGlobalRank == masterGlobalRank);
    if (procCount > 1) {
        char hostName[MPI_MAX_PROCESSOR_NAME + 1];
        int nameLen = 0;
        checkInternalMPI(MPI_Get_processor_name(hostName, &nameLen));
        hostName[nameLen] = 0;
        char processName[300];
        int localWidth = 1, width10 = 10;
        int maxPpn = 1;
        for (int i = 0; i < (int)dvmhSettings.ppn.size(); i++)
            maxPpn = std::max(maxPpn, dvmhSettings.ppn[i]);
        while (width10 < maxPpn) {
            localWidth++;
            width10 *= 10;
        }
        int globalWidth = 1;
        width10 = 10;
        while (width10 < procCount) {
            globalWidth++;
            width10 *= 10;
        }
        if (procCount > dvmhSettings.ppn[0] && maxPpn > 1)
            processName[sprintf(processName, "%0*d(%s[%0*d])", globalWidth, myGlobalRank, hostName, localWidth, myLocalRank)] = 0;
        else if (maxPpn == 1)
            processName[sprintf(processName, "%0*d(%s)", globalWidth, myGlobalRank, hostName)] = 0;
        else
            processName[sprintf(processName, "%0*d", globalWidth, myGlobalRank)] = 0;
        dvmhLogger.setProcessName(processName);
    }
    dvmhLogger.setInited();

    if (dvmhLogger.isMasterProc()) {
        dvmhLogger.startMasterRegion();
        time_t t = time(0);
        struct tm *now = localtime(&t);
        dvmh_log(DEBUG, "Starting log. Local time is %02d:%02d:%02d. Date is %02d.%02d.%04d. Logging level is %d (%s).", now->tm_hour, now->tm_min, now->tm_sec,
                now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, dvmhSettings.logLevel, DvmhLogger::getLogLevelName());
        if (getenv("dvmbuild_real") && *getenv("dvmbuild_real"))
            dvmh_log(INFO, "DVM system build is '%s'", getenv("dvmbuild_real"));
        if (getenv("dvmdir") && *getenv("dvmdir"))
            dvmh_log(INFO, "DVM system is installed in '%s'", getenv("dvmdir"));
        dvmhLogger.endMasterRegion();
    }

    atexit(dvmhLoggerAtExitHook);
#ifndef WIN32
    struct sigaction newAction;
    newAction.sa_sigaction = termSignalHook;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = SA_SIGINFO;
    for (int i = 0; i < (int)(sizeof(termSignals) / sizeof(termSignals[0])); i++) {
        int signum = termSignals[i];
        if (signum >= 0 && signum < (int)(sizeof(fallBackHandlers) / sizeof(fallBackHandlers[0]))) {
            struct sigaction *oldAction = &fallBackHandlers[signum];
            if (sigaction(signum, &newAction, oldAction) == 0) {
                dvmh_log(TRACE, "Set signal handler for signal %d. Saved previous handler %p with flags 0x%X.", signum, oldAction->sa_handler,
                        oldAction->sa_flags);
            } else {
                dvmh_log(DEBUG, "Problems with signal handler setting");
            }
        } else {
            dvmh_log(DEBUG, "Unexpected signal number");
        }
    }
    newAction.sa_sigaction = dumpTrace;
    sigaction(SIGUSR1, &newAction, 0);
#endif
}

static cpu_set_t origMainAffinity;
static bool origMainAffinityOK = false;
static cpu_set_t origProcessAffinity;
static bool origProcessAffinityOK = false;

static void initAffinities(int procCount, int myGlobalRank, int myNodeIndex, int myLocalRank) {
    if (dvmhSettings.setAffinity) {
        int myPpn = dvmhSettings.getPpn(myNodeIndex);
        int nodeBaseRank = myGlobalRank - myLocalRank;
        int myCudas = dvmhSettings.getCudas(myGlobalRank);
        int myThreads = dvmhSettings.getThreads(myGlobalRank);
        int totalProcessors = getProcessorCount();

        int myProcsStart = 0;
        int myProcCount = totalProcessors;
        int reqProcessors = 0;
        for (int i = 0; i < myPpn; i++)
            reqProcessors += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + dvmhSettings.getCudas(nodeBaseRank + i));
        if (reqProcessors <= totalProcessors) {
            // One core per slot.
            myProcsStart = 0;
            for (int i = 0; i < myLocalRank; i++)
                myProcsStart += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + dvmhSettings.getCudas(nodeBaseRank + i));
            myProcCount = std::max(1, myThreads + myCudas);
            for (int i = 0; i < myThreads; i++)
                CPU_SET(myProcsStart + i, devices[0]->getPerformer(i)->getAffinity());
            for (int i = 0; i < myCudas; i++) {
                CPU_SET(myProcsStart + myThreads + i, devices[1 + i]->getPerformer(0)->getAffinity());
                ((CudaDevice *)devices[1 + i])->blockingSync = false;
            }
        } else {
            reqProcessors = 0;
            for (int i = 0; i < myPpn; i++)
                reqProcessors += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + !!dvmhSettings.getCudas(nodeBaseRank + i));
            if (reqProcessors <= totalProcessors) {
                // One core per Host slot. Other devices share other cores.
                myProcsStart = 0;
                for (int i = 0; i < myLocalRank; i++)
                    myProcsStart += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + !!dvmhSettings.getCudas(nodeBaseRank + i));
                myProcCount = std::max(1, myThreads + !!myCudas);
                for (int i = 0; i < myThreads; i++)
                    CPU_SET(myProcsStart + i, devices[0]->getPerformer(i)->getAffinity());
                for (int i = 0; i < myCudas; i++) {
                    CPU_SET(myProcsStart + myThreads, devices[1 + i]->getPerformer(0)->getAffinity());
                    ((CudaDevice *)devices[1 + i])->blockingSync = myCudas > 1;
                }
            } else {
                reqProcessors = 0;
                for (int i = 0; i < myPpn; i++)
                    reqProcessors += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i));
                if (reqProcessors <= totalProcessors) {
                    // One core per Host slot. Other devices share these cores
                    myProcsStart = 0;
                    for (int i = 0; i < myLocalRank; i++)
                        myProcsStart += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i));
                    myProcCount = std::max(1, myThreads);
                    for (int i = 0; i < myThreads; i++)
                        CPU_SET(myProcsStart + i, devices[0]->getPerformer(i)->getAffinity());
                    for (int i = 0; i < myCudas; i++) {
                        for (int j = 0; j < myProcCount; j++)
                            CPU_SET(myProcsStart + j, devices[1 + i]->getPerformer(0)->getAffinity());
                        ((CudaDevice *)devices[1 + i])->blockingSync = myThreads + myCudas > 1;
                    }
                } else {
                    if (myPpn <= totalProcessors) {
                        // All performers share cores
                        int addProcs = 0;
                        for (int i = 0; i < myPpn; i++)
                            addProcs += std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + !!dvmhSettings.getCudas(nodeBaseRank + i)) - 1;
                        double ratio = double(totalProcessors - myPpn) / addProcs;
                        reqProcessors = 0;
                        for (int i = 0; i < myPpn; i++)
                            reqProcessors += 1 + (int)((std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + !!dvmhSettings.getCudas(nodeBaseRank + i)) - 1)
                                    * ratio);
                        checkInternal(reqProcessors <= totalProcessors);
                        myProcsStart = 0;
                        for (int i = 0; i < myLocalRank; i++)
                            myProcsStart += 1 + (int)((std::max(1, dvmhSettings.getThreads(nodeBaseRank + i) + !!dvmhSettings.getCudas(nodeBaseRank + i)) - 1)
                                    * ratio);
                        myProcCount = 1 + (int)((std::max(1, myThreads + !!myCudas) - 1) * ratio);
                        for (int i = 0; i < myThreads; i++)
                            for (int j = 0; j < myProcCount; j++)
                                CPU_SET(myProcsStart + j, devices[0]->getPerformer(i)->getAffinity());
                        for (int i = 0; i < myCudas; i++) {
                            for (int j = 0; j < myProcCount; j++)
                                CPU_SET(myProcsStart + j, devices[1 + i]->getPerformer(0)->getAffinity());
                            ((CudaDevice *)devices[1 + i])->blockingSync = myThreads + myCudas > 1;
                        }
                    } else {
                        // All performers on all processes of this node share all the processors available
                        reqProcessors = totalProcessors;
                        myProcsStart = 0;
                        myProcCount = totalProcessors;
                        for (int i = 0; i < myThreads; i++)
                            for (int j = 0; j < totalProcessors; j++)
                                CPU_SET(j, devices[0]->getPerformer(i)->getAffinity());
                        for (int i = 0; i < myCudas; i++) {
                            for (int j = 0; j < totalProcessors; j++)
                                CPU_SET(j, devices[1 + i]->getPerformer(0)->getAffinity());
                            ((CudaDevice *)devices[1 + i])->blockingSync = true;
                        }
                    }
                }
            }
        }
        checkInternal(reqProcessors <= totalProcessors);
        checkInternal(myProcsStart < totalProcessors);
        checkInternal(myProcCount >= 1);
        cpu_set_t processAffinity;
        CPU_ZERO(&processAffinity);
        for (int i = 0; i < myProcCount; i++)
            CPU_SET(myProcsStart + i, &processAffinity);

        cpu_set_t mainAffinity;
        CPU_ZERO(&mainAffinity);
        mainAffinity = processAffinity;

        int *affinityPerm = new int[totalProcessors];
        fillAffinityPermutation(affinityPerm, totalProcessors, reqProcessors);
        for (int i = 0; i < devicesCount; i++)
            for (int j = 0; j < devices[i]->getSlotCount(); j++)
                applyAffinityPermutation(devices[i]->getPerformer(j)->getAffinity(), affinityPerm, totalProcessors);
        applyAffinityPermutation(&processAffinity, affinityPerm, totalProcessors);
        applyAffinityPermutation(&mainAffinity, affinityPerm, totalProcessors);
        delete[] affinityPerm;
        affinityPerm = 0;

        origMainAffinityOK = getAffinity(&origMainAffinity);

        dvmh_log(DEBUG, "Setting main thread affinity to use set of processors {%s}", affinityToStr(&mainAffinity).c_str());
        setAffinity(&mainAffinity);

#ifndef WIN32
        // On Linux, process affinity does not exist. There are only separate affinities for threads.
        //sched_setaffinity(getpid(), sizeof(processAffinity), &processAffinity);
        memcpy(&origProcessAffinity, &origMainAffinity, sizeof(cpu_set_t));
        origProcessAffinityOK = origMainAffinityOK;
#else
        dvmh_log(DEBUG, "Setting process affinity to use set of processors {%s}", affinityToStr(&processAffinity).c_str());
        {
            DWORD_PTR mask, tmp;
            if (GetProcessAffinityMask(GetCurrentProcess(), &mask, &tmp)) {
                origProcessAffinityOK = true;
                origProcessAffinity = mask;
            }
            mask = processAffinity;
            SetProcessAffinityMask(GetCurrentProcess(), mask);
        }
#endif

#ifdef _OPENMP
        if (dvmhSettings.useOpenMP) {
            if (devices[0]->hasSlots()) {
                #pragma omp parallel num_threads(devices[0]->getSlotCount())
                {
                    int tid = omp_get_thread_num();
                    if (tid > 0) {
                        char threadName[64];
                        threadName[sprintf(threadName, "omp%d", tid)] = 0;
                        DvmhLogger::setThreadName(threadName);
                    }
                    dvmh_log(DEBUG, "Setting affinity for OpenMP thread #%d to use set of processors {%s}", tid,
                            affinityToStr(devices[0]->getPerformer(tid)->getAffinity()).c_str());
                    setAffinity(devices[0]->getPerformer(tid)->getAffinity());
                }
            } else {
                #pragma omp parallel num_threads(1)
                {
                    setAffinity(&mainAffinity);
                }
            }
        }
#endif
    } else {
        cpu_set_t processAffinity;
        CPU_ZERO(&processAffinity);
        getAffinity(&processAffinity);
        cpu_set_t mainAffinity;
        CPU_ZERO(&mainAffinity);
#ifdef _OPENMP
        if (dvmhSettings.useOpenMP) {
            if (devices[0]->hasSlots()) {
                #pragma omp parallel num_threads(devices[0]->getSlotCount())
                {
                    int tid = omp_get_thread_num();
                    if (tid > 0) {
                        char threadName[64];
                        threadName[sprintf(threadName, "omp%d", tid)] = 0;
                        DvmhLogger::setThreadName(threadName);
                    }
                    getAffinity(devices[0]->getPerformer(tid)->getAffinity());
                    dvmh_log(DEBUG, "Reading affinity of OpenMP thread #%d to use set of processors {%s}", tid,
                            affinityToStr(devices[0]->getPerformer(tid)->getAffinity()).c_str());
                    CPU_OR(&processAffinity, &processAffinity, devices[0]->getPerformer(tid)->getAffinity());
                }
            } else {
                #pragma omp parallel num_threads(1)
                {
                    getAffinity(&mainAffinity);
                    CPU_OR(&processAffinity, &processAffinity, &mainAffinity);
                }
            }
        }
#endif
        cpu_set_t emptySet;
        CPU_ZERO(&emptySet);
        if (!CPU_EQUAL(&emptySet, &processAffinity)) {
            for (int i = 1; i < devicesCount; i++)
                for (int j = 0; j < devices[i]->getSlotCount(); j++)
                    CPU_OR(devices[i]->getPerformer(j)->getAffinity(), devices[i]->getPerformer(j)->getAffinity(), &processAffinity);
        }
    }
}

template <typename T>
static int shrinkSequence(std::vector<T> &seq) {
    int l, r, res;
    l = 0;
    r = 0;
    int len = seq.size();
    res = len;
    int *zFunc = new int[len];
    zFunc[0] = 0;
    for (int i = 1; i < len / 2 + 1; i++) {
        int val = 0;
        if (r >= i)
            val = std::min(zFunc[i - l], r - i + 1);
        while (i + val < len && seq[val] == seq[i + val])
            val++;
        if (i + val - 1 > r) {
            l = i;
            r = i + val - 1;
        }
        zFunc[i] = val;
        if (r == len - 1) {
            res = i;
            break;
        }
    }
    delete[] zFunc;
    seq.resize(res);
    return res;
}

static void setIsInParloop() {
    isInParloop = true;
}

static void initDevices(int procCount, int myGlobalRank, int myNodeIndex, int myLocalRank) {
    int myPpn = dvmhSettings.getPpn(myNodeIndex);
    int nodeBaseRank = myGlobalRank - myLocalRank;
    checkInternal(devices.empty());
    if (dvmhSettings.idleRun) {
        devices.push_back(new HostDevice(myGlobalRank));
        devicesCount = devices.size();
        return;
    }
    int totalProcessors = getProcessorCount();
    checkInternal(totalProcessors >= 1);
    dvmh_log(DEBUG, "Found %d processors", totalProcessors);
    dvmhLogger.startMasterRegion();
    dvmh_log(DEBUG, "Maximum capacity of cpu_set_t is %d", (int)sizeof(cpu_set_t) * CHAR_BIT);
    dvmhLogger.endMasterRegion();
    int totalCudas = 0;
#ifdef HAVE_CUDA
    dvmhLogger.startMasterRegion();
    dvmh_log(INFO, "DVM system is compiled with support for CUDA");
    dvmhLogger.endMasterRegion();
    int reqCudas = dvmhSettings.numCudas.empty() ? -1 : dvmhSettings.getCudas(myGlobalRank);
    if (reqCudas < 0 || reqCudas > 0) {
        cudaError_t err = cudaGetDeviceCount(&totalCudas);
        if (err != cudaSuccess) {
            checkError3(reqCudas <= 0, "Cannot request CUDA device count. Requested number of devices (%d) cannot be allocated.", reqCudas);
            dvmh_log(WARNING, "Cannot request CUDA device count - not using CUDA at all");
            totalCudas = 0;
        } else {
            dvmh_log(DEBUG, "Found %d CUDA devices", totalCudas);
        }
    }
    if (reqCudas < 0) {
        int myCudas = totalCudas / gcd(myPpn, totalCudas);
        dvmhSettings.numCudas.resize(procCount);
        dvmhSettings.numCudas[myGlobalRank] = myCudas;
        rootMPS->allgather(dvmhSettings.numCudas);
        shrinkSequence(dvmhSettings.numCudas);
    }
    if (dvmhSettings.getCudas(myGlobalRank) > totalCudas)
        dvmh_log(WARNING, "Requested number of CUDA devices is more than machine has. Requested %d, have %d.",
                dvmhSettings.getCudas(myGlobalRank), totalCudas);
#else
    dvmhLogger.startMasterRegion();
    dvmh_log(INFO, "DVM system is compiled without support for CUDA");
    dvmhLogger.endMasterRegion();
    dvmhSettings.setNoCudas();
#endif
    int nodeCudas = 0;
    for (int i = 0; i < myPpn; i++)
        nodeCudas += dvmhSettings.getCudas(nodeBaseRank + i);
    int myCudas = dvmhSettings.getCudas(myGlobalRank);
    if (dvmhSettings.numThreads.empty()) {
        int myThreads = std::max(0, (totalProcessors - nodeCudas) / myPpn);
        dvmhSettings.numThreads.resize(procCount);
        dvmhSettings.numThreads[myGlobalRank] = myThreads;
        rootMPS->allgather(dvmhSettings.numThreads);
        shrinkSequence(dvmhSettings.numThreads);
    }
    int nodeThreads = 0;
    for (int i = 0; i < myPpn; i++)
        nodeThreads += dvmhSettings.getThreads(nodeBaseRank + i);
#ifdef _OPENMP
    dvmhLogger.startMasterRegion();
    dvmh_log(INFO, "DVM system is compiled with support for OpenMP");
    dvmhLogger.endMasterRegion();
    omp_set_num_threads(std::max(1, dvmhSettings.getThreads(myGlobalRank)));
#else
    dvmhLogger.startMasterRegion();
    dvmh_log(INFO, "DVM system is compiled without support for OpenMP");
    dvmhLogger.endMasterRegion();
#endif
    // Create host device
    devices.push_back(new HostDevice(myGlobalRank));
    devicesCount = devices.size();
    // Create CUDA devices
    if (myCudas > 0) {
        checkInternal(!dvmhSettings.cudaPerf.empty());
        int lowIndex = 0;
#ifdef _MPI_STUBS_
        const char * envR = getMpiRank();
        if (envR && *envR) {
            int curPpn = dvmhSettings.getPpn(myNodeIndex);
            const char * envP = getenv("DVMH_PPN");
            if (envP && *envP)
                curPpn = atoi(envP);
            dvmh_log(INFO, "MPI/DVMH program PMI_RANK=%s, PPN=%d", envR, curPpn);
            lowIndex = atoi(envR)%curPpn;
        } 
#endif
        for (int i = 0; i < myLocalRank; i++)
            lowIndex += dvmhSettings.getCudas(nodeBaseRank + i);
        for (int i = 0; i < myCudas; i++) {
            devices.push_back(new CudaDevice((lowIndex + i) % totalCudas));
            devicesCount = devices.size();
        }
    }

    checkInternal(devicesCount >= 1);
    checkError3((int)sizeof(unsigned long) * CHAR_BIT >= devicesCount, "Maximum device count is %d, can not handle %d devices in one process.",
            (int)(sizeof(unsigned long) * CHAR_BIT), devicesCount);
    if (devicesCount <= 1 && !devices[0]->hasSlots())
        dvmh_log(DEBUG, "No devices selected for execution. Using only the main CPU thread.");

    initAffinities(procCount, myGlobalRank, myNodeIndex, myLocalRank);

    for (int i = 0; i < devicesCount; i++) {
        devices[i]->startPerformers();
        if (devices[i]->getType() == dtCuda) {
            assert(devices[i]->getSlotCount() > 0);
            ((CudaDevice *)devices[i])->setAsCurrent();
        }
    }

#ifdef HAVE_CUDA
    for (int i = 0; i < devicesCount; i++) {
        for (int j = 0; j < devicesCount; j++) {
            if (i != j && devices[i]->getType() == dtCuda && devices[j]->getType() == dtCuda &&
                    ((CudaDevice *)devices[i])->isMaster() && ((CudaDevice *)devices[j])->isMaster()) {
                int dev1 = ((CudaDevice *)devices[i])->index;
                int dev2 = ((CudaDevice *)devices[j])->index;
                if (dev1 != dev2) {
                    int canFlag;
                    checkInternalCuda(cudaDeviceCanAccessPeer(&canFlag, dev1, dev2));
                    if (canFlag) {
                        dvmh_log(DEBUG, "Enabling Peer Access from device %d to device %d", dev1, dev2);
                        checkInternalCuda(cudaSetDevice(dev1));
// XXX: can hang here for some systems
                        checkInternalCuda(cudaDeviceEnablePeerAccess(dev2, 0));
                        ((CudaDevice *)devices[i])->canAccessPeers |= (1 << dev2);
                    } else {
                        dvmh_log(DEBUG, "Not enabling Peer Access from device %d to device %d due to %s", dev1, dev2, (canFlag ? "system hanging" :
                                "impossibility"));
                    }
                }
            }
        }
    }
    for (int i = 0; i < devicesCount; i++) {
        for (int j = 0; j < devicesCount; j++) {
            if (i != j && devices[i]->getType() == dtCuda && devices[j]->getType() == dtCuda &&
                    !((CudaDevice *)devices[i])->isMaster() && ((CudaDevice *)devices[j])->isMaster() &&
                    ((CudaDevice *)devices[i])->index == ((CudaDevice *)devices[j])->index)
                ((CudaDevice *)devices[i])->canAccessPeers = ((CudaDevice *)devices[j])->canAccessPeers;
        }
    }
#endif

    for (int i = 0; i < devicesCount; i++) {
        for (int j = 0; j < devices[i]->getSlotCount(); j++)
            devices[i]->commitTask(MethodExecutor::create(setIsInParloop), j);
    }
#ifdef _OPENMP
    if (dvmhSettings.useOpenMP && devices[0]->hasSlots()) {
        #pragma omp parallel num_threads(devices[0]->getSlotCount())
        {
            setIsInParloop();
        }
    }
#endif
    isInParloop = false;

    if (devicesCount > 1 || devices[0]->hasSlots()) {
        // Print info on devices in use
        char devicesStr[4096];
        char *devptr = devicesStr;
        for (int i = 0; i < devicesCount; i++)
            if (devices[i]->hasSlots()) {
                if (devices[i]->getType() == dtHost)
                    devptr += sprintf(devptr, " HOST (%d slots, performance=%g)", devices[i]->getSlotCount(), devices[i]->getPerformance());
                else if (devices[i]->getType() == dtCuda) {
                    devptr += sprintf(devptr, " CUDA (device numbers:");
                    for (; i < devicesCount && devices[i]->getType() == dtCuda; i++)
                        devptr += sprintf(devptr, " %d(performance=%g)", ((CudaDevice *)devices[i])->index, devices[i]->getPerformance());
                    i--;
                    devptr += sprintf(devptr, ")");
                } else
                    devptr += sprintf(devptr, " UNKNOWN (%d slots, performance=%g)", devices[i]->getSlotCount(), devices[i]->getPerformance());
            }
        int totalSlotCount = 0;
        for (int i = 0; i < devicesCount; i++)
            totalSlotCount += devices[i]->getSlotCount();
        *devptr = 0;
        dvmh_log(DEBUG, "Set of devices in use:%s. Total device count = %d. Total slot count = %d",
                devicesStr, devicesCount, totalSlotCount);
    }
    {
        int grandTotalThreads = 0;
        int grandTotalCudas = 0;
        for (int i = 0; i < procCount; i++) {
            int cudas = dvmhSettings.getCudas(i);
            int threads = dvmhSettings.getThreads(i);
            grandTotalCudas += cudas;
            grandTotalThreads += threads;
            if (cudas == 0 && threads == 0)
                grandTotalThreads++;
        }
        dvmhLogger.startMasterRegion();
        dvmh_log(INFO, "Number of processes in use: %d", procCount);
        dvmh_log(INFO, "Total number of CPU threads in use: %d", grandTotalThreads);
        dvmh_log(INFO, "Total number of CUDA devices in use: %d", grandTotalCudas);
        dvmhLogger.endMasterRegion();
    }
    if (dvmhSettings.compareDebug) {
        for (int i = 0; i < procCount; i++) {
            if (dvmhSettings.getCudas(i) == 0) {
                dvmhLogger.startMasterRegion();
                dvmh_log(WARNING, "Comparative debugging can not be enabled. Please, allow execution on accelerator(s).");
                dvmhLogger.endMasterRegion();
                dvmhSettings.compareDebug = false;
                break;
            }
        }
    }
    if (dvmhSettings.compareDebug) {
        dvmhLogger.startMasterRegion();
        dvmh_log(INFO, "The program is started in the comparative debugging mode");
        dvmhLogger.endMasterRegion();
    }
    if (dvmhSettings.logLevel >= DEBUG && (devicesCount > 1 || devices[0]->hasSlots()))
        DvmhLogger::setThreadName("main");
}

void dvmhInitialize() {
    checkInternal2(!inited, "LibDVMH is already initialized");

    // Pre-adjust, load and post-adjust settings
    dvmhSettings.useFortranNotation = dvmhSettings.fortranProgram;
    std::vector<ValueSetReport> fromEnv = dvmhSettings.loadFromEnv();
    if (dvmhSettings.noH || dvmhSettings.idleRun) {
        dvmhSettings.schedTech = stSingleDevice;
        dvmhSettings.allowAsync = false;
        dvmhSettings.setNoCudas();
        if (dvmhSettings.fortranProgram || dvmhSettings.idleRun)
            dvmhSettings.setNoThreads();
    }
    if (dvmhSettings.idleRun) {
        dvmhSettings.optimizeParams = false;
        dvmhSettings.ioThreadCount = 0;
        dvmhSettings.setAffinity = false;
        dvmhSettings.compareDebug = false;
    }

    // Initialize root multiprocessor system
    checkInternal(currentMPS == 0);
    checkInternal(rootMPS == 0);
    if (!dvmhSettings.idleRun) {
#ifndef NO_DVM
        checkInternal2(DVM_VMS->HasCurrent, "Current process is not in the root processor system");
        currentMPS = createMpsFromVMS(DVM_VMS, 0);
        currentMPS->setWeights(ProcWeightArray);
        tieDvm(currentMPS, DVM_VMS->HandlePtr, true);
#endif
        if (noLibdvm) {
            int worldSize;
            checkInternalMPI(MPI_Comm_size(MPI_COMM_WORLD, &worldSize));
            int rank = dvmhSettings.procGrid.size();
            if (rank == 0)
                rank = 1;
    #ifdef NON_CONST_AUTOS
            int procSizes[rank], periods[rank];
    #else
            int procSizes[MAX_MPS_RANK], periods[MAX_MPS_RANK];
    #endif
            if (!dvmhSettings.procGrid.empty()) {
                for (int i = 0; i < rank; i++)
                    procSizes[i] = dvmhSettings.procGrid[i];
            } else {
                assert(rank == 1);
                procSizes[0] = worldSize;
            }
            int checkTotalSize = 1;
            for (int i = 0; i < rank; i++)
                checkTotalSize *= procSizes[i];
            checkInternal2(checkTotalSize == worldSize, "Processor grid mismatches actual process count");
            for (int i = 0; i < rank; i++)
                periods[i] = 0;
            MPI_Comm cartComm;
            checkInternalMPI(MPI_Cart_create(MPI_COMM_WORLD, rank, procSizes, periods, 1, &cartComm));
            checkInternal(cartComm != MPI_COMM_NULL);
            currentMPS = new MultiprocessorSystem(cartComm, rank, procSizes);
            if (false) {
                // TODO: Enable only by some parameter
                double *weights = measureProcPowerMPI(currentMPS, DEBUG);
                currentMPS->setWeights(weights);
                delete[] weights;
            }
        }
    } else {
        currentMPS = new MultiprocessorSystem();
    }
    rootMPS = currentMPS;

    // Find out our place in the cluster
    int procCount, myGlobalRank, myNodeIndex, myLocalRank;
    procCount = rootMPS->getCommSize();
    myGlobalRank = rootMPS->getCommRank();
    if (!dvmhSettings.idleRun) {
        char myName[MPI_MAX_PROCESSOR_NAME + 1];
        int myLen = 0;
        checkInternalMPI(MPI_Get_processor_name(myName, &myLen));
        myName[myLen] = 0;
        char *buf = new char[procCount * (MPI_MAX_PROCESSOR_NAME + 1)];
        strcpy(buf + myGlobalRank * (MPI_MAX_PROCESSOR_NAME + 1), myName);
        rootMPS->allgather(buf, MPI_MAX_PROCESSOR_NAME + 1);
        std::set<std::string> checkSet;
        dvmhSettings.ppn.clear();
        dvmhSettings.ppn.push_back(1);
        checkSet.insert(std::string(buf));
        for (int i = 1; i < procCount; i++) {
            const char *prevName = buf + (i - 1) * (MPI_MAX_PROCESSOR_NAME + 1);
            const char *curName = buf + i * (MPI_MAX_PROCESSOR_NAME + 1);
            if (strcmp(prevName, curName))
                dvmhSettings.ppn.push_back(1);
            else
                dvmhSettings.ppn.back()++;
            checkSet.insert(std::string(curName));
        }
        checkError2(checkSet.size() == dvmhSettings.ppn.size(), "Processes on the same node must be adjacent");
        delete[] buf;
        shrinkSequence(dvmhSettings.ppn);
    } else {
        dvmhSettings.ppn.clear();
        dvmhSettings.ppn.push_back(1);
    }
    assert(!dvmhSettings.ppn.empty());
    int baseNodeRank = 0;
    myLocalRank = -1;
    for (myNodeIndex = 0; ; myNodeIndex++) {
        int curPpn = dvmhSettings.getPpn(myNodeIndex);
        if (baseNodeRank + curPpn > myGlobalRank) {
            myLocalRank = myGlobalRank - baseNodeRank;
            break;
        }
        baseNodeRank += curPpn;
    }
    assert(myLocalRank >= 0);

    // Reduce settings
    if (!dvmhSettings.idleRun)
        rootMPS->allreduce(dvmhSettings.parallelMPI, rf_PROD);

    // Logging
    initLogging(procCount, myGlobalRank, myNodeIndex, myLocalRank, rootMPS->getIOProc());

    // Fundamental checks
    checkInternal3(sizeof(DvmType) >= sizeof(void *), "DVM type is defined too small. sizeof(DvmType)=%d sizeof(void *)=%d.", (int)sizeof(DvmType),
            (int)sizeof(void *));
    checkInternal3(sizeof(DvmType) >= sizeof(size_t), "DVM type is defined too small. sizeof(DvmType)=%d sizeof(size_t)=%d.", (int)sizeof(DvmType),
            (int)sizeof(size_t));

    // General settings checks
    if (!dvmhSettings.haveOpenMP && dvmhSettings.useOpenMP) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "It is strongly recommended to recompile DVM system with enabled OpenMP support in order to use OpenMP effectively");
        dvmhLogger.endMasterRegion();
    }
    std::set<std::string> remainingEnvVars;
    for (int i = 0; environ[i]; i++)
        if (!strncmp("DVMH_", environ[i], 5))
            remainingEnvVars.insert(std::string(environ[i], strchr(environ[i], '=') - environ[i]));
    // XXX: Some variables are not intended to be read by DvmhSettings
    remainingEnvVars.erase("DVMH_ARGS");
    for (int i = 0; i < (int)fromEnv.size(); i++) {
        if (fromEnv[i].result != ValueSetReport::vsrNotFound)
            remainingEnvVars.erase(fromEnv[i].paramName);
        checkError3(fromEnv[i].result != ValueSetReport::vsrInvalidFormat, "Failed to parse %s environment variable%s%s", fromEnv[i].paramName.c_str(),
                (fromEnv[i].errorString.empty() ? "" : ": "), fromEnv[i].errorString.c_str());
        checkError3(fromEnv[i].result != ValueSetReport::vsrInvalidValue, "Invalid value found in %s environment variable%s%s", fromEnv[i].paramName.c_str(),
                (fromEnv[i].errorString.empty() ? "" : ": "), fromEnv[i].errorString.c_str());
    }
    if (!remainingEnvVars.empty()) {
        std::string varList;
        for (std::set<std::string>::iterator it = remainingEnvVars.begin(); it != remainingEnvVars.end(); it++)
            varList += (varList.empty() ? "" : ", ") + *it;
        dvmh_log(WARNING, "Unrecognized DVMH environment variables found (check the spelling): %s", varList.c_str());
    }
    if (!dvmhSettings.procGrid.empty()) {
        for (int i = 0; i < (int)dvmhSettings.procGrid.size(); i++)
            checkInternal2(dvmhSettings.procGrid[i] == rootMPS->getAxis(i + 1).procCount, "Processor grid mismatch");
    }

    dvmhLogger.startMasterRegion();
    if (!dvmhSettings.idleRun && dvmhSettings.realMPI)
        dvmh_log(INFO, "MPI is in THREAD_%s mode", (dvmhSettings.parallelMPI ? "MULTIPLE" : "FUNNELED"));
    dvmh_log(DEBUG, "Memory page size is " UDTFMT " bytes", dvmhSettings.pageSize);
    dvmhLogger.endMasterRegion();
    // Devices
    initDevices(procCount, myGlobalRank, myNodeIndex, myLocalRank);
    prepareLoopDistributionCaches();

    double sumWgt = 0.0;
    for (int i = 0; i < devicesCount; i++) {
        double wgt = 0.0;
        if (devices[i]->hasSlots()) {
            if (devices[i]->getType() == dtHost)
                wgt = dvmhSettings.getCpuPerf(myGlobalRank);
            else if (devices[i]->getType() == dtCuda)
                wgt = dvmhSettings.getCudaPerf(((CudaDevice *)devices[i])->index);
            else
                checkInternal2(0, "Internal inconsistency");
        }
        lastBestWeights.push_back(wgt);
        sumWgt += wgt;
    }
    if (sumWgt > 0)
        for (int i = 0; i < devicesCount; i++)
            lastBestWeights[i] /= sumWgt;
    checkInternal(loopDict.empty());
    checkInternal(regionDict.empty());
    checkInternal(allObjects.empty());
    checkInternal(currentRegion == 0);
    checkInternal(currentLoop == 0);
    stdioInit();

    // Redistribute/realign skip settings
    if (!dvmhSettings.idleRun) {
        AllowRedisRealnBypass = 0;
        if (procCount == 1) {
            int cudas = dvmhSettings.getCudas(0);
            int threads = dvmhSettings.getThreads(0);
            if ((cudas == 1 && threads == 0) || cudas == 0)
                AllowRedisRealnBypass = 1;
        }
    }

    inited = true;
    dvmh_barrier();
    timerBase = dvmhTime();
    DvmhModuleInitializer::executeInit();
}

void dvmhFinalize(bool cleanup) {
    stdioFinish();
    // Generating output information
    LogLevel statLevel = DEBUG;
    for (std::map<SourcePosition, DvmhLoopPersistentInfo *>::iterator it = loopDict.begin(); it != loopDict.end(); it++) {
        dvmh_log(statLevel, "Performance statistics for parallel loop at %s(%d):", it->first.getFileName().c_str(), it->first.getLineNumber());
        for (int dev = 0; dev < devicesCount; dev++) {
            int devWritten = 0;
            for (int hand = 0; hand < it->second->getHandlersCount(dev); hand++) {
                LoopHandlerPerformanceInfo *pinfo = it->second->getLoopHandlerPerformanceInfo(dev, hand);
                std::map<double, double> tf = pinfo->genPerfTableFunc();
                if (!tf.empty()) {
                    if (!devWritten) {
                        dvmh_log(statLevel, "    Device #%d:", dev);
                        devWritten = 1;
                    }
                    dvmh_log(statLevel, "        Handler #%d:", hand);
                    HandlerOptimizationParams *params = pinfo->getBestParams(1).second;
                    const char *explTexts[] = {"default", "user", "optimizer"};
                    if (devices[dev]->getType() == dtHost) {
                        int explBlock;
                        int threads = ((HostHandlerOptimizationParams *)params)->getThreads(&explBlock);
                        dvmh_log(statLevel, "            Best parameters: threads=%d(%s)", threads, explTexts[explBlock]);
                    } else if (devices[dev]->getType() == dtCuda) {
                        int block[3];
                        int explBlock = ((CudaHandlerOptimizationParams *)params)->getBlock(block);
                        dvmh_log(statLevel, "            Best parameters: thread-block=(%d,%d,%d)(%s)", block[0], block[1], block[2], explTexts[explBlock]);
                    }
                    dvmh_log(statLevel, "            Table function (iterations => performance):");
                    for (std::map<double, double>::iterator it2 = tf.begin(); it2 != tf.end(); it2++)
                        dvmh_log(statLevel, "                %12g => %g", it2->first, it2->second);
                }
            }
        }
    }
    if (dvmhSettings.schedTech == stSimpleDynamic) {
        dvmhLogger.startMasterRegion();
        dvmh_log(INFO, "Simple dynamic run performances:");
        dvmhLogger.endMasterRegion();
        // TODO: Unite CPU_PERF and CUDAS_PERF among processors
        std::vector<double> cudaPerfs;
        for (int i = 0; i < devicesCount; i++)
            if (devices[i]->hasSlots()) {
                if (devices[i]->getType() == dtHost) {
                    dvmh_log(INFO, "DVMH_CPU_PERF='%g'", lastBestWeights[i]);
                } else if (devices[i]->getType() == dtCuda) {
                    int index = ((CudaDevice *)devices[i])->index;
                    if ((int)cudaPerfs.size() < index + 1)
                        cudaPerfs.resize(index + 1, 0.0);
                    cudaPerfs[index] = lastBestWeights[i];
                }
            }
        if (!cudaPerfs.empty()) {
            char buf[300];
            char *s = buf;
            s += sprintf(s, "%g", cudaPerfs[0]);
            for (int i = 1; i < (int)cudaPerfs.size(); i++)
                s += sprintf(s, " %g", cudaPerfs[i]);
            *s = 0;
            dvmh_log(INFO, "DVMH_CUDAS_PERF='%s'", buf);
        }
    }
    if (dvmhSettings.schedTech == stDynamic || dvmhSettings.optimizeParams) {
        int regionCount = regionDict.size();
        FILE *f = fopen(dvmhSettings.schedFile.c_str(), "wt");
        fprintf(f, "# Scheduling scheme and optimization parameters' values of DVMH-program\n");
        fprintf(f, "# Region enumeration with their properties\n");
        fprintf(f, "Begin Regions\n");
        int counter = 0;
        std::map<int, int> appearanceToIdent;
        std::vector<DvmhRegionPersistentInfo *> byIdent;
        for (std::map<SourcePosition, DvmhRegionPersistentInfo *>::iterator it = regionDict.begin(); it != regionDict.end(); it++) {
            counter++;
            appearanceToIdent[it->second->getAppearanceNumber()] = counter;
            byIdent.push_back(it->second);
            fprintf(f, "Identifier=%d SourceFile=\"%s\" SourceLine=%d LoopCount=%d\n", counter, it->first.getFileName().c_str(), it->first.getLineNumber(),
                    (int)it->second->getLoopInfos().size());
        }
        fprintf(f, "End Regions\n");
        fprintf(f, "RegionAppearanceOrder=\"");
        for (std::map<int, int>::iterator it = appearanceToIdent.begin(); it != appearanceToIdent.end(); it++)
            fprintf(f, "%s%d", (it != appearanceToIdent.begin() ? " " : ""), it->second);
        fprintf(f, "\"\n");
        fprintf(f, "# Description of mapping regions to devices\n");
        fprintf(f, "Begin Mapping\n");
        for (int i = 0; i < regionCount; i++) {
            // TODO: Add loop by distrib spaces (somehow), then add loop by device numbers
            // TODO: Get correct weights
            // TODO: Add distrib space info (somehow)
            int dev = devicesCount - 1;
            fprintf(f, "Region=%d DeviceNumber=%d DeviceWeight=%g\n", i + 1, dev, 1.0);
        }
        fprintf(f, "End Mapping\n");
        fprintf(f, "# Description of optimization parameters values of loops\n");
        fprintf(f, "Begin Parameters\n");
        for (int i = 0; i < regionCount; i++) {
            const std::vector<DvmhLoopPersistentInfo *> &loopInfos = byIdent[i]->getLoopInfos();
            for (int j = 0; j < (int)loopInfos.size(); j++) {
                for (int dev = 0; dev < devicesCount; dev++) {
                    // TODO: Pass correct 'part' value based on weights
                    std::pair<int, HandlerOptimizationParams *> bestParams = loopInfos[j]->getBestParams(dev, 1.0);
                    if (bestParams.first >= 0) {
                        // If it has launches on this device
                        fprintf(f, "Loop=%d.%d DeviceNumber=%d Handler=%d", i + 1, j + 1, dev, bestParams.first);
                        if (devices[dev]->getType() == dtHost) {
                            int threads = ((HostHandlerOptimizationParams *)bestParams.second)->getThreads();
                            fprintf(f, " Threads=%d", threads);
                        } else if (devices[dev]->getType() == dtCuda) {
                            int block[3];
                            ((CudaHandlerOptimizationParams *)bestParams.second)->getBlock(block);
                            fprintf(f, " BlockX=%d BlockY=%d BlockZ=%d", block[0], block[1], block[2]);
                        }
                        fprintf(f, "\n");
                    }
                }
            }
        }
        fprintf(f, "End Parameters\n");
        fclose(f);
    }
    if (cleanup) {
        bool canDeleteDevices = true;
        // Delete global variables
        DvmhModuleInitializer::executeFinish();
        if (!allObjects.empty()) {
            dvmh_log(DEBUG, "There are DVMH objects which aren't deleted");
            canDeleteDevices = false;
        }
        if (!regularVars.empty())
            dvmh_log(DEBUG, "There are non-exited data regions");
        // Cleaning global stuff
        for (std::map<SourcePosition, DvmhRegionPersistentInfo *>::iterator it = regionDict.begin(); it != regionDict.end(); it++)
            delete it->second;
        regionDict.clear();
        for (std::map<SourcePosition, DvmhLoopPersistentInfo *>::iterator it = loopDict.begin(); it != loopDict.end(); it++)
            delete it->second;
        loopDict.clear();
        std::set<int> resetCudas;
        for (int i = 0; i < devicesCount; i++) {
            devices[i]->barrier();
            if (devices[i]->getType() == dtCuda)
                resetCudas.insert(((CudaDevice *)devices[i])->index);
        }
        clearLoopDistributionCaches();
        // TODO: Clean NVRTC stuff up too
        if (canDeleteDevices) {
            for (int i = 0; i < devicesCount; i++)
                delete devices[i];
            devices.clear();
            devicesCount = 0;
#ifdef HAVE_CUDA
            for (std::set<int>::iterator it = resetCudas.begin(); it != resetCudas.end(); it++) {
                checkInternalCuda(cudaSetDevice(*it));
                checkInternalCuda(cudaDeviceReset());
            }
#endif
        }
        // TODO: Maybe somehow stop started OpenMP threads.
#ifdef WIN32
        if (origProcessAffinityOK) {
            DWORD_PTR mask = origProcessAffinity;
            SetProcessAffinityMask(GetCurrentProcess(), mask);
        }
#endif
        if (origMainAffinityOK)
            setAffinity(&origMainAffinity);
        dvmhLogger.flush();
#ifndef WIN32
        for (int i = 0; i < (int)(sizeof(termSignals) / sizeof(termSignals[0])); i++) {
            int signum = termSignals[i];
            if (signum >= 0 && signum < (int)(sizeof(fallBackHandlers) / sizeof(fallBackHandlers[0]))) {
                struct sigaction *oldAct = &fallBackHandlers[signum];
                sigaction(signum, oldAct, 0);
            }
        }
#endif
    }
    finalized = true;
}

void handlePreAcross(DvmhLoop *loop, const LoopBounds curLoopBounds[]) {
    dvmh_log(TRACE, "PreAcross start");
    for (int i = 0; i < loop->acrossNew.dataCount(); i++) {
        const DvmhShadowData *sdata = &loop->acrossNew.getData(i);
        DvmhData *data = sdata->data;
        int dataRank = data->getRank();
#ifdef NON_CONST_AUTOS
        Interval roundedPart[dataRank];
        bool forwardDirection[dataRank], leftmostPart[dataRank], rightmostPart[dataRank];
        ShdWidth inWidths[dataRank], outWidths[dataRank];
#else
        Interval roundedPart[MAX_ARRAY_RANK];
        bool forwardDirection[MAX_ARRAY_RANK], leftmostPart[MAX_ARRAY_RANK], rightmostPart[MAX_ARRAY_RANK];
        ShdWidth inWidths[MAX_ARRAY_RANK], outWidths[MAX_ARRAY_RANK];
#endif
        bool hasSomething = loop->fillLoopDataRelations(curLoopBounds, data, forwardDirection, roundedPart, leftmostPart, rightmostPart);
        if (hasSomething) {
            DvmhLoop::fillAcrossInOutWidths(dataRank, sdata->shdWidths, forwardDirection, leftmostPart, rightmostPart, inWidths, outWidths);
            for (int j = 0; j < dataRank; j++) {
                dvmh_log(TRACE, "PreAcross axis %d ShdWidths - " DTFMT "," DTFMT "; forward - %d; leftmost - %d, rightmost - %d, inWidths - " DTFMT "," DTFMT "; outWidths - " DTFMT "," DTFMT,
                        j, sdata->shdWidths[j][0], sdata->shdWidths[j][1], forwardDirection[j], leftmostPart[j], rightmostPart[j], inWidths[j][0],
                        inWidths[j][1], outWidths[j][0], outWidths[j][1]);
            }
            DvmhPieces *setActual = new DvmhPieces(dataRank);
            data->setActualShadow(0, roundedPart, sdata->cornerFlag, inWidths, &setActual);
            data->setActualEdges(0, roundedPart, outWidths, &setActual);
            if (loop->region) {
                loop->region->renewData(data, setActual);
            }
        }
    }
    dvmh_log(TRACE, "PreAcross end");
}

void handlePostAcross(DvmhLoop *loop, const LoopBounds curLoopBounds[]) {
    dvmh_log(TRACE, "PostAcross start");
    for (int i = 0; i < loop->acrossNew.dataCount(); i++) {
        const DvmhShadowData *sdata = &loop->acrossNew.getData(i);
        DvmhData *data = sdata->data;
        int dataRank = data->getRank();
#ifdef NON_CONST_AUTOS
        Interval roundedPart[dataRank];
        bool forwardDirection[dataRank], leftmostPart[dataRank], rightmostPart[dataRank];
        ShdWidth inWidths[dataRank], outWidths[dataRank];
#else
        Interval roundedPart[MAX_ARRAY_RANK];
        bool forwardDirection[MAX_ARRAY_RANK], leftmostPart[MAX_ARRAY_RANK], rightmostPart[MAX_ARRAY_RANK];
        ShdWidth inWidths[MAX_ARRAY_RANK], outWidths[MAX_ARRAY_RANK];
#endif
        bool hasSomething = loop->fillLoopDataRelations(curLoopBounds, data, forwardDirection, roundedPart, leftmostPart, rightmostPart);
        if (hasSomething) {
            DvmhLoop::fillAcrossInOutWidths(dataRank, sdata->shdWidths, forwardDirection, leftmostPart, rightmostPart, inWidths, outWidths);
            for (int j = 0; j < dataRank; j++) {
                dvmh_log(TRACE, "PostAcross axis %d ShdWidths - " DTFMT "," DTFMT "; forward - %d; leftmost - %d, rightmost - %d, inWidths - " DTFMT "," DTFMT "; outWidths - " DTFMT "," DTFMT,
                        j, sdata->shdWidths[j][0], sdata->shdWidths[j][1], forwardDirection[j], leftmostPart[j], rightmostPart[j], inWidths[j][0],
                        inWidths[j][1], outWidths[j][0], outWidths[j][1]);
            }
            if (loop->region) {
                PushCurrentPurpose purpose(DvmhCopyingPurpose::dcpShadow);
                data->getActualEdges(roundedPart, inWidths, loop->region->canAddToActual(data, roundedPart));
                data->getActualShadow(0, roundedPart, sdata->cornerFlag, outWidths, true);
            }
        }
    }
    dvmh_log(TRACE, "PostAcross end");
}

int autotransformInternal(DvmhLoopCuda *cloop, DvmhData *data) {
    DvmhLoop *loop = cloop->getLoop();
    checkInternal(loop);
    UDvmType depMask = loop->dependencyMask;
    int depCount = oneBitCount(depMask);
    DvmhRepresentative *repr = data->getRepr(cloop->getDeviceNum());
    int res = 0;
    int dataRank = data->getRank();
    if (dataRank > 0 && repr) {
        bool okFlag = false;
        HybridVector<int, 10> corr = loop->getArrayCorrespondence(data, true);
        if (loop->rank > depCount && depCount <= 1) {
            // If loop has independent dimension and there are at most 1 dependency. We can just permutate axes of data (hopefully)
            int loopFastestAxis = loop->rank - ilogN(~depMask, 1);
            assert(loopFastestAxis >= 1 && loopFastestAxis <= loop->rank);
            int dataFastestAxis = std::abs(corr[loopFastestAxis - 1]);
            if (dataFastestAxis > 0) {
                // If we can tie data dimension with fastest loop dimension
#ifdef NON_CONST_AUTOS
                int axisPerm[dataRank];
#else
                int axisPerm[MAX_ARRAY_RANK];
#endif
                typedMemcpy(axisPerm, repr->getBuffer()->getAxisPerm(), dataRank);
                int oldPlace = std::find(axisPerm, axisPerm + dataRank, dataFastestAxis) - axisPerm;
                assert(oldPlace >= 0 && oldPlace < dataRank);
                int newPlace = dataRank - 1;
                if (oldPlace != newPlace)
                    std::swap(axisPerm[oldPlace], axisPerm[newPlace]);
                repr->doTransform(axisPerm, 0);
                okFlag = true;
            }
        } else if (depCount >= 2) {
            // We have at least two dimensions with dependencies. We can diagonalize them (hopefully)
            int loopFastestAxis = loop->rank - ilogN(depMask, 1);
            assert(loopFastestAxis >= 1 && loopFastestAxis <= loop->rank);
            int loopPrevAxis = loop->rank - ilogN(depMask, 2);
            assert(loopPrevAxis >= 1 && loopPrevAxis <= loop->rank);
            int dataFastestAxis = std::abs(corr[loopFastestAxis - 1]);
            int dataPrevAxis = std::abs(corr[loopPrevAxis - 1]);
            if (dataFastestAxis > 0 && dataPrevAxis > 0) {
                // If we can tie two data dimensions with two fastest loop dimensions
                dvmh_log(TRACE, "loopFastestAxis=%d loopPrevAxis=%d dataFastestAxis=%d dataPrevAxis=%d",
                        loopFastestAxis, loopPrevAxis, dataFastestAxis, dataPrevAxis);
                assert(dataFastestAxis != dataPrevAxis);
#ifdef NON_CONST_AUTOS
                int axisPerm[dataRank];
#else
                int axisPerm[MAX_ARRAY_RANK];
#endif
                typedMemcpy(axisPerm, repr->getBuffer()->getAxisPerm(), dataRank);
                int oldPlace = std::find(axisPerm, axisPerm + dataRank, dataFastestAxis) - axisPerm;
                assert(oldPlace >= 0 && oldPlace < dataRank);
                int newPlace = dataRank - 1;
                if (oldPlace != newPlace)
                    std::swap(axisPerm[oldPlace], axisPerm[newPlace]);
                oldPlace = std::find(axisPerm, axisPerm + dataRank, dataPrevAxis) - axisPerm;
                assert(oldPlace >= 0 && oldPlace < dataRank);
                newPlace = dataRank - 2;
                if (oldPlace != newPlace)
                    std::swap(axisPerm[oldPlace], axisPerm[newPlace]);
                int dirFastest = sign(corr[loopFastestAxis - 1]);
                int dirPrev = sign(corr[loopPrevAxis - 1]);
                int slashFlag = dirFastest * dirPrev < 0;
                repr->doTransform(axisPerm, 1 + slashFlag);
                okFlag = true;
            }
        }
        if (!okFlag) {
            // Cannot determine best transformation. Using nearest state (undo diagonalization in case we have at most one dependency)
            if (depCount <= 1)
                repr->undoDiagonal();
        }
        repr->setCleanTransformState(false);
        res = (repr->getBuffer()->isTransformed() ? 1 : 0) + (repr->getBuffer()->isDiagonalized() ? 1 : 0);
    }
    return res;
}

DvmhObject *passOrGetOrCreateDvmh(DvmType handle, bool addToAllObjects, std::vector<DvmhObject *> *createdObjects) {
    DvmhObject *obj = (DvmhObject *)handle;
#ifndef NO_DVM
    if (obj && (getDvmh(handle) || !obj->checkSignature())) {
        assert(allObjects.find(obj) == allObjects.end());
        std::vector<DvmhObject *> localCreatedObjects;
        if (!createdObjects)
            createdObjects = &localCreatedObjects;
        DvmhObject *obj2 = getOrCreateTiedDvmh(handle, createdObjects);
        if (obj2)
            obj = obj2;
        if (addToAllObjects && !createdObjects->empty())
            allObjects.insert(obj);
    }
#endif
    return obj;
}

}

// DVMH runtime API (excluding the I/O part)

extern "C" void dvmh_barrier() {
    for (int i = 0; i < devicesCount; i++)
        devices[i]->barrier();
    currentMPS->barrier();
}

extern "C" int dvmh_get_total_num_procs() {
    return currentMPS->getCommSize();
}

extern "C" int dvmh_get_num_proc_axes() {
    return currentMPS->getRank();
}

extern "C" int dvmh_get_num_procs(int axis) {
    checkError2(axis > 0, "Axis number must be positive");
    return currentMPS->getAxis(axis).procCount;
}

extern "C" double dvmh_wtime() {
    return dvmhTime() - timerBase;
}

// DvmhModuleInitializer

DvmhModuleInitializer::DvmhModuleInitializer(void (*fInit)(), void (*fFinish)()) {
    next = head;
    head = this;
    funcInit = fInit;
    funcFinish = fFinish;
    if (inited) {
        // Initialization was already performed. Could be the case for dynamic libraries.
        checkError2(rootMPS->isSimilarTo(currentMPS), "Initialization must be completed by the same multiprocessor system");
        funcInit();
    }
}

void DvmhModuleInitializer::executeInit() {
    DvmhModuleInitializer *cur = head;
    while (cur) {
        cur->funcInit();
        cur = cur->next;
    }
}

void DvmhModuleInitializer::executeFinish() {
    DvmhModuleInitializer *cur = head;
    while (cur) {
        if (cur->funcFinish)
            cur->funcFinish();
        cur = cur->next;
    }
}

DvmhModuleInitializer *DvmhModuleInitializer::head = 0;
