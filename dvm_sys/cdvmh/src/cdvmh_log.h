#pragma once

#include <string>

namespace cdvmh {

enum LogLevel {INTERNAL = 0, ERROR = 1, WARNING = 2, INFO = 3, DEBUG = 4, TRACE = 5, DONT_LOG};
extern LogLevel logLevel;

#ifdef __GNUC__
#define G_GNUC_PRINTF(format_idx, arg_idx) __attribute__((__format__(__printf__, format_idx, arg_idx)))
#else
#define G_GNUC_PRINTF(format_idx, arg_idx)
#endif
bool cdvmhLog(LogLevel level, const std::string &fileName, int lineNumber, const char *fmt, ...) G_GNUC_PRINTF(4, 5);
bool cdvmhLog(LogLevel level, const std::string &fileName, int lineNumber, int errorCode, const char *fmt, ...) G_GNUC_PRINTF(5, 6);

#define cdvmh_log(level, ...) cdvmhLog(level, __FILE__, __LINE__, __VA_ARGS__)
#define intErr(...) do { cdvmhLog(INTERNAL, __FILE__, __LINE__, __VA_ARGS__); exit(1); } while (0)
#define checkIntErr(expr, ...) do { if (!(expr)) intErr(__VA_ARGS__); } while (0)
#define userErr(file, line, ...) do { cdvmhLog(ERROR, file, line, __VA_ARGS__); exit(1); } while (0)
#define checkUserErr(expr, file, line, ...) do { if (!(expr)) userErr(file, line, __VA_ARGS__); } while (0)
#define checkDirErr(expr, ...) checkUserErr(expr, (curPragma ? curPragma->fileName : ""), (curPragma ? curPragma->line : 1), __VA_ARGS__)

#ifdef _MSC_VER
#define checkCommonN(expr, level, file, line, errorCode, ...) do { \
    if (!(expr)) { \
        cdvmhLog(level, file, line, errorCode, MSG(errorCode), __VA_ARGS__); \
        if (level == INTERNAL) abort(); \
        if (level == ERROR) exit(1); \
    } \
} while(0)
#define checkIntErrN(expr, errorCode, ...) checkCommonN(expr, INTERNAL, __FILE__, __LINE__, errorCode, __VA_ARGS__)
#define intErrN(errorCode, ...) checkIntErrN(false, errorCode, __VA_ARGS__)
#define checkUserErrN(expr, file, line, errorCode,...) checkCommonN(expr, ERROR, file, line, errorCode, __VA_ARGS__)
#define userErrN(file, line, errorCode,...) checkUserErrN(false, file, line, errorCode, __VA_ARGS__)
#define checkDirErrN(expr, errorCode, ...) checkUserErrN(expr, (curPragma ? curPragma->fileName : ""), (curPragma ? curPragma->line : 1), errorCode, __VA_ARGS__)
#else
#define checkCommonN(expr, level, file, line, errorCode, ...) do { \
    if (!(expr)) { \
        cdvmhLog(level, std::string(file), line, errorCode, MSG(errorCode) , ##__VA_ARGS__); \
        if (level == INTERNAL) abort(); \
        if (level == ERROR) exit(1); \
    } \
} while(0)
#define checkIntErrN(expr, ...) checkCommonN(expr, INTERNAL, __FILE__, __LINE__, __VA_ARGS__)
#define intErrN(...) checkIntErrN(false, __VA_ARGS__)
#define checkUserErrN(expr, file, line, ...) checkCommonN(expr, ERROR, file, line, __VA_ARGS__)
#define userErrN(...) checkUserErrN(false, __VA_ARGS__)
#define checkDirErrN(expr, ...) checkUserErrN(expr, (curPragma ? curPragma->fileName : ""), (curPragma ? curPragma->line : 1), __VA_ARGS__)
#endif
}
