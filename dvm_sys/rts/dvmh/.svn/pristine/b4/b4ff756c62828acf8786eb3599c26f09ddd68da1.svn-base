#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "dvmh_types.h"

namespace libdvmh {

class PersistentSettings {
public:
    std::vector<int> ppn;
    bool haveOpenMP;
    bool useOpenMP;
    bool idleRun;
    bool realMPI;
    bool parallelMPI;
    bool fortranProgram;
    bool noH;
    bool debug;
    UDvmType pageSize;
public:
    int getPpn(int nodeIndex) const { assert(!ppn.empty()); return ppn[nodeIndex % ppn.size()]; }
public:
    PersistentSettings() { setDefault(); }
public:
    void setDefault();
    void setFromInitFlags(DvmType flags);
};

struct ValueSetReport {
    enum ValueSetResult { vsrOk, vsrNotFound, vsrInvalidFormat, vsrInvalidValue };
    std::string paramName;
    ValueSetResult result;
    std::string errorString;
};

class AdjustableSettings {
public:
    LogLevel logLevel;
    LogLevel flushLogLevel;
    std::string logFile;
    bool fatalToStderr;
    std::vector<int> numThreads;
    std::vector<int> numCudas;
    std::vector<double> cpuPerf;
    std::vector<double> cudaPerf;
    bool cudaPreferShared;
    SchedTech schedTech;
    std::string schedFile;
    bool reduceDependencies;
    bool compareDebug;
    bool allowAsync;
    float compareFloatsEps;
    double compareDoublesEps;
    long double compareLongDoublesEps;
    bool alwaysSync;
    bool useFortranNotation;
    bool cacheGpuAllocations;
    bool pageLockHostMemory;
    bool useGenblock;
    bool setAffinity;
    bool optimizeParams;
    bool noDirectCopying;
    bool specializeRtc;
    bool preferCallWithSwitch;
    bool preferBestOrderExchange;
    bool checkExchangeMap;
    int numVariantsForVarRtc;
    UDvmType parallelIOThreshold;
    UDvmType maxIOBufSize;
    int ioThreadCount;
    std::vector<int> procGrid;
public:
    int getThreads(int rank) const { assert(!numThreads.empty()); return numThreads[rank % numThreads.size()]; }
    int getCudas(int rank) const { assert(!numCudas.empty()); return numCudas[rank % numCudas.size()]; }
    double getCpuPerf(int rank) const { assert(!cpuPerf.empty()); return cpuPerf[rank % cpuPerf.size()]; }
    double getCudaPerf(int devIndex) const { assert(!cudaPerf.empty()); return cudaPerf[devIndex % cudaPerf.size()]; }
public:
    AdjustableSettings() { setDefault(); }
public:
    void setDefault();
    std::vector<ValueSetReport> loadFromEnv();
    std::vector<ValueSetReport> loadFromFile(const char *fn);
    void setNoThreads();
    void setNoCudas();
};

class DvmhSettings: public PersistentSettings, public AdjustableSettings {
public:
    void setDefault();
};

extern DvmhSettings dvmhSettings;

}
