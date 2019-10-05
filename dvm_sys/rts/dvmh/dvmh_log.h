#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "settings.h"

#ifdef __GNUC__
#define G_GNUC_PRINTF(format_idx, arg_idx) __attribute__((__format__(__printf__, format_idx, arg_idx)))
#else
#define G_GNUC_PRINTF(format_idx, arg_idx)
#endif

// External logging function
extern "C" int dvmhLog(libdvmh::LogLevel level, const char *fileName, int lineNumber, const char *fmt, ...) G_GNUC_PRINTF(4, 5);

namespace libdvmh {

class DvmhMutex;

class DvmhLogger {
public:
    bool isMasterProc() const { return masterFlag || separateFlag; }
    void setMasterFlag(bool value) { masterFlag = value; }
    void setInited() { inited = true; }
public:
    DvmhLogger();
public:
    void setProcessName(const char *name);
    void useFile(FILE *f, bool isSeparateFile, bool closeOnEnd = true);
    int log(LogLevel level, const char *fileName, int lineNumber, const char *fmt, ...) G_GNUC_PRINTF(5, 6);
    int log(LogLevel level, const char *fileName, int lineNumber, const char *fmt, va_list &ap);
    void startBlock(LogLevel level);
    void endBlock(LogLevel level, const char *fileName, int lineNumber);
    void startMasterRegion();
    void endMasterRegion();
    void flush();
public:
    static const char *getLogLevelName(LogLevel level);
    static const char *getLogLevelName() { return getLogLevelName(dvmhSettings.logLevel); }
    static void setThreadName(const char *name);
    static void clearThreadName();
public:
    ~DvmhLogger();
protected:
    bool shouldEmitMessage(LogLevel level) const;
protected:
    bool inited;
    bool separateFlag;
    bool masterFlag;
    char *processName;
    FILE *logFile;
    bool closeFlag;
    int blockDepth;
    bool firstLineFlag;
    int masterDepth;
    char *buf;
    DvmhMutex *mut;
    static THREAD_LOCAL char *threadName;
};

void blockOut(LogLevel level, const char *fileName, int lineNumber, int rank, const Interval inter[], DvmType order = 0);

extern DvmhLogger dvmhLogger;

// Convenience macros

#define custom_log(level, func, ...) do { \
    LogLevel _lvl = (level); \
    if (_lvl <= TRACE && _lvl <= dvmhSettings.logLevel) \
        func(_lvl, __FILE__, __LINE__, __VA_ARGS__); \
} while (0)
#define dvmh_log(level, ...) custom_log(level, dvmhLog, __VA_ARGS__)

#define checkCommon(expr, level, fmt, ...) do { \
    bool successFlag = (expr) ? true : false; \
    if (!successFlag) { \
        dvmh_log(level, fmt, __VA_ARGS__); \
        ((level) == INTERR ? abort() : exit(1)); \
    } \
} while (0)

#define checkError3(expr, fmt, ...) checkCommon(expr, FATAL, fmt, __VA_ARGS__)
#define checkError2(expr, msg) checkError3(expr, "%s", msg)
#define checkErrorNumb(expr, number, fmt, ...) checkError3(expr, "*** RTS err %s: " fmt, number, __VA_ARGS__)

#define checkErrorCuda(expr) do { \
    cudaError_t _err = (expr); \
    checkError3(_err == cudaSuccess, "CUDA error occured in \"%s\": Error #%d - \"%s\"", #expr, _err, cudaGetErrorString(_err)); \
} while (0)

#define checkInternal3(expr, fmt, ...) checkCommon(expr, INTERR, fmt, __VA_ARGS__)
#define checkInternal2(expr, msg) checkInternal3(expr, "%s", msg)
#define checkInternal(expr) checkInternal2(expr, "\"" #expr "\" is not true.")
#define checkInternalNumb(expr, number, fmt, ...) checkInternal3(expr, "*** RTS fatal err %s: " fmt, number, __VA_ARGS__)

#define checkInternalCuda(expr) do { \
    cudaError_t _err = (expr); \
    checkInternal3(_err == cudaSuccess, "CUDA error occured in \"%s\": Error #%d - \"%s\"", #expr, _err, cudaGetErrorString(_err)); \
} while (0)
#define checkInternalCU(expr) do { \
    CUresult result = (expr); \
    if (result != CUDA_SUCCESS) { \
        const char *msg; \
        cuGetErrorName(result, &msg); \
        checkInternal3(0, "CUDA driver error occured in \"%s\": Error #%d - \"%s\"", #expr, result, msg); \
    } \
} while (0)

#define checkInternalMPI(expr) do { \
    int _err = (expr); \
    if (_err != MPI_SUCCESS) { \
        char msg[MPI_MAX_ERROR_STRING + 1]; \
        int len = 0; \
        MPI_Error_string(_err, msg, &len); \
        msg[len] = 0; \
        checkInternal3(0, "MPI error occured in \"%s\": Error #%d - \"%s\"", #expr, _err, msg); \
    } \
} while (0)

#define warningNumb(number, fmt, ...) dvmh_log(WARNING, "*** RTS warning %s: " fmt, number, __VA_ARGS__)

}
