#include "cdvmh_log.h"

#include <cstdio>
#include <cstdarg>

namespace cdvmh {

static const char * logMessages[] = {"Internal error", "error", "warning", "info", "debug", "trace"};

LogLevel logLevel = WARNING;

static bool cdvmhLogInternal(LogLevel level, const std::string &fileName, int lineNumber, int errorCode, const char *fmt, va_list &ap) {
    if (level >= 0 && level <= logLevel) {
        char buf[5000];
        char *s = buf;
        if (!fileName.empty())
            s += sprintf(s, "%s:%d: ", fileName.c_str(), lineNumber);
        if (errorCode > 0)
            s += sprintf(s, "%s #%d: ", logMessages[level], errorCode);
        else
            s += sprintf(s, "%s: ", logMessages[level]);
        if (fmt)
            s += vsprintf(s, fmt, ap);
        if (s[-1] != '\n')
            s += sprintf(s, "\n");
        *s = 0;
        fprintf(stderr, "%s", buf);
        return true;
    } else {
        return false;
    }
}

bool cdvmhLog(LogLevel level, const std::string &fileName, int lineNumber, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    bool res = cdvmhLogInternal(level, fileName, lineNumber, -1, fmt, ap);
    va_end(ap);
    return res;
}

bool cdvmhLog(LogLevel level, const std::string &fileName, int lineNumber, int errorCode, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    bool res = cdvmhLogInternal(level, fileName, lineNumber, errorCode, fmt, ap);
    va_end(ap);
    return res;
}

}
