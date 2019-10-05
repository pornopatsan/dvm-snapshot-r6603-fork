#include "settings.h"

#include <cstdlib>
#include <cstring>
#include <float.h>
#include <algorithm>
#include <sstream>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "dvmh_log.h"

namespace libdvmh {

template <typename T>
static void setUniversal(std::vector<T> &list, const T &val) {
    list.resize(1);
    list[0] = val;
}

// PersistentSettings

void PersistentSettings::setDefault() {
    setUniversal(ppn, 1);
#ifdef _OPENMP
    haveOpenMP = true;
#else
    haveOpenMP = false;
#endif
    useOpenMP = false;
    idleRun = true;
#ifdef _MPI_STUBS_
    realMPI = false;
#else
    realMPI = true;
#endif
    parallelMPI = false;
    fortranProgram = false;
    noH = true;
    debug = false;
#ifndef WIN32
    pageSize = sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    pageSize = si.dwPageSize;
#endif
}

void PersistentSettings::setFromInitFlags(DvmType flags) {
    dvmhSettings.fortranProgram = (flags & ifFortran) != 0;
    dvmhSettings.noH = (flags & ifNoH) != 0;
    dvmhSettings.idleRun = (flags & ifSequential) != 0;
    dvmhSettings.useOpenMP = (flags & ifOpenMP) != 0;
    dvmhSettings.debug = (flags & ifDebug) != 0;
}

// AdjustableSettings

static std::string getEnv(const char *name, bool *pIsSet = 0) {
    const char *sett = getenv(name);
    if (pIsSet)
        *pIsSet = sett != 0;
    return sett ? sett : "";
}

static std::string trim(const std::string &a) {
    int len = a.size();
    int toSkip = 0;
    while (toSkip < len && isspace(a[toSkip]))
        toSkip++;
    int toTruncate = 0;
    while (toSkip + toTruncate < len && isspace(a[len - 1 - toTruncate]))
        toTruncate++;
    return toSkip + toTruncate > 0 ? std::string(a, toSkip, len - toSkip - toTruncate) : a;
}

static std::string toLower(const std::string &a) {
    std::string res;
    res.resize(a.size());
    std::transform(a.begin(), a.end(), res.begin(), tolower);
    return res;
}

template<typename T>
static std::string toStr(const T &v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}

void AdjustableSettings::setDefault() {
    logLevel = NFERROR;
    flushLogLevel = FATAL;
    logFile.clear();
    fatalToStderr = true;
    numThreads.clear();
    numCudas.clear();
    setUniversal(cpuPerf, 1.0);
    setUniversal(cudaPerf, 1.0);
    cudaPreferShared = false;
    schedTech = stSimpleStatic;
    schedFile = "Scheme.dvmh";
    reduceDependencies = false;
    compareDebug = false;
    allowAsync = false;
    compareFloatsEps = FLT_EPSILON * 1000.0f;
    compareDoublesEps = DBL_EPSILON * 10000.0;
    compareLongDoublesEps = LDBL_EPSILON * 100000.0L;
    alwaysSync = false;
    useFortranNotation = false;
    cacheGpuAllocations = true;
    useGenblock = false;
    std::string sett = toLower(trim(getEnv("I_MPI_PIN")));
    if (sett == "disable" || sett == "no" || sett ==  "off" || sett == "0")
        setAffinity = true;
    else if ((sett == "enable" || sett == "yes" || sett == "on" || sett == "1") || !trim(getEnv("I_MPI_PIN_PROCESSOR_LIST")).empty() ||
            !trim(getEnv("I_MPI_PIN_DOMAIN")).empty())
        setAffinity = false;
    else
        setAffinity = true;
#ifdef _OPENMP
    if (!trim(getEnv("KMP_AFFINITY")).empty())
        setAffinity = false;
#endif
    optimizeParams = false;
    noDirectCopying = false;
    specializeRtc = true;
    preferCallWithSwitch = true;
    preferBestOrderExchange = true;
    checkExchangeMap = true;
    numVariantsForVarRtc = 3;
    parallelIOThreshold = 100 * 1024 * 1024;
    maxIOBufSize = 100 * 1024 * 1024;
    ioThreadCount = 5;
    procGrid.clear();
}

template <typename T>
static bool parseValue(const char *str, T *pVal);

#define DEF_PARSE_SSCANF(T, fmt) \
template <> \
bool parseValue(const char *str, T *pVal) { \
    T tmp; \
    int readCount = 0; \
    bool readOk = sscanf(str, fmt "%n", &tmp, &readCount) == 1; \
    bool noGarbage = trim(str + readCount).empty(); \
    if (readOk && noGarbage) { \
        *pVal = tmp; \
        return true; \
    } \
    return false; \
}

DEF_PARSE_SSCANF(int, "%d")
DEF_PARSE_SSCANF(float, "%f")
DEF_PARSE_SSCANF(double, "%lf")
DEF_PARSE_SSCANF(long double, "%Lf")
DEF_PARSE_SSCANF(UDvmType, UDTFMT)

#undef DEF_PARSE_SSCANF

template <>
bool parseValue(const char *str, bool *pVal) {
    bool res = true;
    std::string strVal = toLower(trim(str));
    if (strVal == "enable" || strVal == "yes" || strVal == "on" || strVal == "1")
        *pVal = true;
    else if (strVal == "disable" || strVal == "no" || strVal == "off" || strVal == "0")
        *pVal = false;
    else
        res = false;
    return res;
}

template <>
bool parseValue(const char *str, std::string *pVal) {
    *pVal = trim(str);
    return true;
}

template <>
bool parseValue(const char *str, LogLevel *pVal) {
    //{INTERR = -1, FATAL = 0, NFERROR = 1, WARNING = 2, INFO = 3, DEBUG = 4, TRACE = 5, LOG_LEVELS, DONT_LOG}
    bool res = true;
    std::string strVal = toLower(trim(str));
    int intVal;
    if (parseValue(str, &intVal)) {
        if (intVal < (int)FATAL)
            intVal = (int)FATAL;
        if (intVal > (int)TRACE)
            intVal = (int)TRACE;
        *pVal = (LogLevel)intVal;
    } else if (strVal == "fatal") {
        *pVal = FATAL;
    } else if (strVal == "error" || strVal == "err") {
        *pVal = NFERROR;
    } else if (strVal == "warning" || strVal == "warn") {
        *pVal = WARNING;
    } else if (strVal == "info") {
        *pVal = INFO;
    } else if (strVal == "debug") {
        *pVal = DEBUG;
    } else if (strVal == "trace") {
        *pVal = TRACE;
    } else {
        res = false;
    }
    return res;
}

template <>
bool parseValue(const char *str, SchedTech *pVal) {
    //{stSingleDevice = 0, stSimpleStatic = 1, stSimpleDynamic = 2, stDynamic = 3, stUseScheme = 4, SCHED_TECHS}
    bool res = true;
    std::string strVal = toLower(trim(str));
    int intVal;
    if (parseValue(str, &intVal)) {
        if (intVal < (int)stSingleDevice || intVal > (int)stUseScheme)
            res = false;
        else
            *pVal = (SchedTech)intVal;
    } else if (strVal == "single" || strVal == "device") {
        *pVal = stSingleDevice;
    } else if (strVal == "static") {
        *pVal = stSimpleStatic;
    } else if (strVal == "dynamic1") {
        *pVal = stSimpleDynamic;
    } else if (strVal == "dynamic2") {
        *pVal = stDynamic;
    } else if (strVal == "scheme") {
        *pVal = stUseScheme;
    } else {
        res = false;
    }
    return res;
}

template <typename T>
static bool parseValue(const char *str, std::vector<T> *pVal) {
    bool res = true;
    std::vector<T> resList;
    char *buf = new char[strlen(str) + 1];
    strcpy(buf, str);
    char *s = strtok(buf, " ,");
    while (s) {
        T nextVal;
        res = parseValue(s, &nextVal) && res;
        resList.push_back(nextVal);
        s = strtok(0, " ,");
    }
    delete[] buf;
    if (res)
        *pVal = resList;
    return res;
}

namespace {

template <typename T>
bool isNonNegative(const T &val, std::string &msg) {
    if (val < 0) {
        msg = "must be non-negative";
        return false;
    }
    return true;
}

template <typename T>
bool isPositive(const T &val, std::string &msg) {
    if (val <= 0) {
        msg = "must be positive";
        return false;
    }
    return true;
}

}

template <typename T>
static bool isNonEmpty(const T &val, std::string &msg) {
    if (val.empty()) {
        msg = "must be non-empty";
        return false;
    }
    return true;
}

template <typename T, bool (*elemValid)(const T &, std::string &), bool emptyOK>
static bool listValid(const std::vector<T> &val, std::string &msg) {
    bool res = true;
    msg.clear();
    for (int i = 0; i < (int)val.size(); i++) {
        std::string elemMsg;
        if (!elemValid(val[i], elemMsg)) {
            if (!msg.empty())
                msg += ", ";
            msg += "at index " + toStr(i) + " " + elemMsg;
            res = false;
        }
    }
    if (val.empty() && !emptyOK) {
        msg = "must be non-empty";
        res = false;
    }
    return res;
}

template <typename T>
static void setValueFromEnv(std::vector<ValueSetReport> &res, const char *name, T *pVal, bool (*isValid)(const T &, std::string &) = 0) {
    res.resize(res.size() + 1);
    ValueSetReport &vsr = res.back();
    vsr.paramName = name;
    const char *sett = getenv(name);
    if (!sett) {
        vsr.result = ValueSetReport::vsrNotFound;
        return;
    }
    T tmp;
    bool parseOk = parseValue(sett, &tmp);
    if (!parseOk) {
        vsr.result = ValueSetReport::vsrInvalidFormat;
        return;
    }
    if (isValid) {
        std::string msg;
        if (!isValid(tmp, msg)) {
            vsr.result = ValueSetReport::vsrInvalidValue;
            vsr.errorString = msg;
            return;
        }
    }
    *pVal = tmp;
    vsr.result = ValueSetReport::vsrOk;
}

std::vector<ValueSetReport> AdjustableSettings::loadFromEnv() {
    std::vector<ValueSetReport> res;
    setValueFromEnv(res, "DVMH_LOGLEVEL", &logLevel);
    setValueFromEnv(res, "DVMH_FLUSHLOGLEVEL", &flushLogLevel);
    setValueFromEnv(res, "DVMH_LOGFILE", &logFile, isNonEmpty<std::string>);
    setValueFromEnv(res, "DVMH_FATAL_TO_STDERR", &fatalToStderr);
    setValueFromEnv(res, "DVMH_NUM_THREADS", &numThreads, listValid<int, isNonNegative<int>, true>);
    setValueFromEnv(res, "DVMH_NUM_CUDAS", &numCudas, listValid<int, isNonNegative<int>, true>);
    setValueFromEnv(res, "DVMH_CPU_PERF", &cpuPerf, listValid<double, isNonNegative<double>, false>);
    setValueFromEnv(res, "DVMH_CUDAS_PERF", &cudaPerf, listValid<double, isNonNegative<double>, false>);
    setValueFromEnv(res, "DVMH_CUDA_PREFER_SHARED", &cudaPreferShared);
    setValueFromEnv(res, "DVMH_SCHED_TECH", &schedTech);
    setValueFromEnv(res, "DVMH_SCHED_FILE", &schedFile, isNonEmpty<std::string>);
    setValueFromEnv(res, "DVMH_REDUCE_DEPS", &reduceDependencies);
    setValueFromEnv(res, "DVMH_COMPARE_DEBUG", &compareDebug);
    setValueFromEnv(res, "DVMH_ALLOW_ASYNC", &allowAsync);
    setValueFromEnv(res, "DVMH_COMPARE_FLOATS_EPS", &compareFloatsEps, isNonNegative<float>);
    setValueFromEnv(res, "DVMH_COMPARE_DOUBLES_EPS", &compareDoublesEps, isNonNegative<double>);
    setValueFromEnv(res, "DVMH_COMPARE_LONGDOUBLES_EPS", &compareLongDoublesEps, isNonNegative<long double>);
    setValueFromEnv(res, "DVMH_SYNC_CUDA", &alwaysSync);
    setValueFromEnv(res, "DVMH_FORTRAN_NOTATION", &useFortranNotation);
    setValueFromEnv(res, "DVMH_CACHE_CUDA_ALLOC", &cacheGpuAllocations);
    setValueFromEnv(res, "DVMH_USE_GENBLOCK", &useGenblock);
    setValueFromEnv(res, "DVMH_SET_AFFINITY", &setAffinity);
    setValueFromEnv(res, "DVMH_OPT_PARAMS", &optimizeParams);
    setValueFromEnv(res, "DVMH_NO_DIRECT_COPY", &noDirectCopying);
    setValueFromEnv(res, "DVMH_SPECIALIZE_RTC", &specializeRtc);
    setValueFromEnv(res, "DVMH_PREFER_CALL_SWITCH", &preferCallWithSwitch);
    setValueFromEnv(res, "DVMH_PREFER_BEST_ORDER", &preferBestOrderExchange);
    setValueFromEnv(res, "DVMH_CHECK_EXCHANGE_MAP", &checkExchangeMap);
    setValueFromEnv(res, "DVMH_NUM_VARIANTS_FOR_VAR_RTC", &numVariantsForVarRtc, isNonNegative<int>);
    setValueFromEnv(res, "DVMH_PARALLEL_IO_THRES", &parallelIOThreshold);
    setValueFromEnv(res, "DVMH_IO_BUF_SIZE", &maxIOBufSize);
    setValueFromEnv(res, "DVMH_IO_THREAD_COUNT", &ioThreadCount, isNonNegative<int>);
    setValueFromEnv(res, "DVMH_PROC_GRID", &procGrid, listValid<int, isPositive<int>, true>);
    return res;
}

std::vector<ValueSetReport> AdjustableSettings::loadFromFile(const char *fn) {
    std::vector<ValueSetReport> res;
    // TODO: An alternative way to load settings
    return res;
}

void AdjustableSettings::setNoThreads() {
    setUniversal(numThreads, 0);
}

void AdjustableSettings::setNoCudas() {
    setUniversal(numCudas, 0);
}

// DvmhSettings

void DvmhSettings::setDefault() {
    PersistentSettings::setDefault();
    AdjustableSettings::setDefault();
}

DvmhSettings dvmhSettings;

}
