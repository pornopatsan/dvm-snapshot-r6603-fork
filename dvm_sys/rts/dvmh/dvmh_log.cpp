#include "dvmh_log.h"

#include <cstdarg>
#include <cstring>

#include "dvmh_async.h"
// TODO: Get rid of this dependency
#ifndef NO_DVM
#include "dvmlib_incs.h"
#endif
#include "util.h"

using namespace libdvmh;

extern "C" int dvmhLog(LogLevel level, const char *fileName, int lineNumber, const char *fmt, ...) {
    if (level >= INTERR && level <= dvmhSettings.logLevel) {
        va_list ap;
        va_start(ap, fmt);
        int res = dvmhLogger.log(level, fileName, lineNumber, fmt, ap);
        va_end(ap);
        return res;
    } else {
        return 0;
    }
}

namespace libdvmh {

extern int currentLine;
extern char currentFile[];

// Some constants
static const char *logMessagesBase[] = {"INTERNAL", "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE"};
static const char **logMessages = logMessagesBase + 1;
static const char *compatLogMessagesBase[] = {"fatal err", "err", "err", "warning", "info", "debug", "trace"};
static const char **compatLogMessages = compatLogMessagesBase + 1;
static int userFileLen = 26;
static int sysFileLen = 26;

DvmhLogger::DvmhLogger() {
    inited = false;
    separateFlag = false;
    masterFlag = true;
    processName = 0;
    logFile = stderr;
    closeFlag = false;
    blockDepth = 0;
    firstLineFlag = false;
    masterDepth = 0;
    buf = new char[1048576];
    mut = new DvmhMutex(true);
}

void DvmhLogger::setProcessName(const char *name) {
    delete[] processName;
    processName = 0;
    if (name) {
        int len = strlen(name);
        processName = new char[len + 1];
        strcpy(processName, name);
    }
}

void DvmhLogger::useFile(FILE *f, bool isSeparateFile, bool closeOnEnd) {
    assert(f);
    if (logFile && logFile != stderr && logFile != stdout && closeFlag)
        fclose(logFile);
    logFile = f;
    separateFlag = isSeparateFile;
    closeFlag = closeOnEnd;
}

int DvmhLogger::log(LogLevel level, const char *fileName, int lineNumber, const char *fmt, ...) {
    if (level >= INTERR && level <= dvmhSettings.logLevel) {
        va_list ap;
        va_start(ap, fmt);
        int res = log(level, fileName, lineNumber, fmt, ap);
        va_end(ap);
        return res;
    } else {
        return 0;
    }
}

static char *pad(char *s, int written, int size) {
    s += written;
    while (written < size) {
        *s++ = ' ';
        written++;
    }
    return s;
}

int DvmhLogger::log(LogLevel level, const char *fileName, int lineNumber, const char *fmt, va_list &ap) {
    if (shouldEmitMessage(level)) {
        bool compatMode = dvmhSettings.logLevel < DEBUG;
#ifndef NO_DVM
        const char *currentFile = (DVM_FILE[0] ? DVM_FILE[0] : "unknown");
        int currentLine = DVM_LINE[0];
#endif
        MutexGuard guard(*mut);
        char *s = buf;
        char *prefixFull = s;
        if (processName && (masterDepth == 0 || !compatMode))
            s += sprintf(s, "%s: ", processName);
        char *prefix = (separateFlag ? s : prefixFull);
        if (!compatMode) {
            s = pad(s, sprintf(s, "[%s] ", getLogLevelName(level)), 11);
            s = pad(s, sprintf(s, "%s:%d ", currentFile, currentLine), userFileLen);
            s = pad(s, sprintf(s, "%s:%d ", fileName, lineNumber), sysFileLen);
            if (threadName)
                s += sprintf(s, "THREAD %s: ", threadName);
        }
        *s++ = 0;
        char *msg = s;
        if (fmt)
            s += vsprintf(s, fmt, ap);
        if (s > msg && compatMode) {
            if (blockDepth == 0) {
                *s++ = '\n';
                if (level <= WARNING)
                    s += sprintf(s, "USRFILE=%s;  USRLINE=%d;\n", currentFile, currentLine);
                if (level <= FATAL)
                    s += sprintf(s, "SYSFILE=%s;  SYSLINE=%d;\n", fileName, lineNumber);
            }
        }
        *s++ = 0;
        char *firstLinePrefix = s;
        if (*msg && compatMode && (blockDepth == 0 || firstLineFlag)) {
            if (threadName)
                s += sprintf(s, "THREAD %s: ", threadName);
            if (strncmp(msg, "*** RTS ", 8))
                s += sprintf(s, "*** RTS %s: ", getLogLevelName(level));
        }
        *s++ = 0;
        s = msg;
        int msgCount = 0;
        for (;;) {
            char *s2 = strchr(s, '\n');
            if (s2)
                *s2 = 0;
            if (*s) {
                msgCount++;
                fprintf(logFile, "%s%s%s\n", prefix, (msgCount == 1 ? firstLinePrefix : ""), s);
                if (dvmhSettings.fatalToStderr && level <= FATAL && logFile != stderr && logFile != stdout)
                    fprintf(stderr, "%s%s%s\n", prefixFull, (msgCount == 1 ? firstLinePrefix : ""), s);
            }
            if (!s2)
                break;
            s = s2 + 1;
        }
        if (level <= dvmhSettings.flushLogLevel)
            fflush(logFile);
        if (msgCount > 0)
            firstLineFlag = false;
        return msgCount;
    } else {
        return 0;
    }
}

void DvmhLogger::startBlock(LogLevel level) {
    if (level >= INTERR && level <= dvmhSettings.logLevel) {
        mut->lock();
        blockDepth++;
        if (blockDepth == 1)
            firstLineFlag = true;
    }
}

void DvmhLogger::endBlock(LogLevel level, const char *fileName, int lineNumber) {
    if (blockDepth == 1 && shouldEmitMessage(level)) {
        bool compatMode = dvmhSettings.logLevel < DEBUG;
        if (compatMode) {
#ifndef NO_DVM
            const char *currentFile = (DVM_FILE[0] ? DVM_FILE[0] : "unknown");
            int currentLine = DVM_LINE[0];
#endif
            char *s = buf;
            char *prefix = s;
            if (processName && !separateFlag && masterDepth == 0)
                s += sprintf(s, "%s: ", processName);
            *s++ = 0;
            char *msg = s;
            if (level <= WARNING)
                s += sprintf(s, "USRFILE=%s;  USRLINE=%d;\n", currentFile, currentLine);
            if (level <= FATAL)
                s += sprintf(s, "SYSFILE=%s;  SYSLINE=%d;\n", fileName, lineNumber);
            *s++ = 0;
            s = msg;
            for (;;) {
                char *s2 = strchr(s, '\n');
                if (s2)
                    *s2 = 0;
                if (*s) {
                    fprintf(logFile, "%s%s\n", prefix, s);
                    if (dvmhSettings.fatalToStderr && level <= FATAL && logFile != stderr && logFile != stdout)
                        fprintf(stderr, "%s%s\n", prefix, s);
                }
                if (!s2)
                    break;
                s = s2 + 1;
            }
            if (level <= dvmhSettings.flushLogLevel)
                fflush(logFile);
        }
    }
    if (level >= INTERR && level <= dvmhSettings.logLevel) {
        assert(blockDepth > 0);
        blockDepth--;
        mut->unlock();
    }
}

void DvmhLogger::startMasterRegion() {
    mut->lock();
    masterDepth++;
}

void DvmhLogger::endMasterRegion() {
    assert(masterDepth > 0);
    masterDepth--;
    mut->unlock();
}

void DvmhLogger::flush() {
    fflush(logFile);
}

const char *DvmhLogger::getLogLevelName(LogLevel level) {
    bool compatMode = dvmhSettings.logLevel < DEBUG;
    return (compatMode ? compatLogMessages : logMessages)[level];
}

void DvmhLogger::setThreadName(const char *name) {
    delete[] threadName;
    threadName = 0;
    if (name) {
        int len = strlen(name);
        threadName = new char[len + 1];
        strcpy(threadName, name);
    }
}

void DvmhLogger::clearThreadName() {
    delete[] threadName;
    threadName = 0;
}

DvmhLogger::~DvmhLogger() {
    delete[] processName;
    delete[] buf;
    if (logFile && logFile != stderr && logFile != stdout && closeFlag)
        fclose(logFile);
    delete mut;
}

bool DvmhLogger::shouldEmitMessage(LogLevel level) const {
    if (inited)
        return (isMasterProc() || masterDepth == 0) && level >= INTERR && level <= dvmhSettings.logLevel;
    else
        return level >= INTERR && level <= NFERROR && level <= dvmhSettings.logLevel;
}

THREAD_LOCAL char *DvmhLogger::threadName = 0;

void blockOut(LogLevel level, const char *fileName, int lineNumber, int rank, const Interval inter[], DvmType order) {
    char buf[300];
    char *cb = buf;
    *cb = 0;
    if (order > 0)
        cb += sprintf(cb, DTFMT, order);
    if (dvmhSettings.useFortranNotation) {
        if (rank > 0) {
            cb += sprintf(cb, "(");
            for (int i = rank - 1; i >= 1; i--)
                cb += sprintf(cb, DTFMT ":" DTFMT ", ", inter[i][0], inter[i][1]);
            cb += sprintf(cb, DTFMT ":" DTFMT ")", inter[0][0], inter[0][1]);
        }
    } else {
        for (int i = 0; i < rank; i++)
            cb += sprintf(cb, "[" DTFMT ".." DTFMT "]", inter[i][0], inter[i][1]);
    }
    *cb = 0;
    dvmhLogger.log(level, fileName, lineNumber, "%s", buf);
}

DvmhLogger dvmhLogger;

}
