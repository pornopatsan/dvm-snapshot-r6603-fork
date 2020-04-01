#include "dvmh_stdio.h"
#include "include/dvmhlib_stdio.h"
#include "include/dvmh_runtime_api.h"

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdarg>
#ifdef HAVE_PTRDIFF_T
#include <cstddef>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef HAVE_INTMAX_T
#include <stdint.h>
#endif
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#ifndef WIN32
#include <unistd.h>
#else
#pragma GCC visibility push(default)
#include <io.h>
#pragma GCC visibility pop
#endif

#include "include/dvmhlib_const.h"

#include "distrib.h"
#include "dvmh_buffer.h"
#include "dvmh_data.h"
#include "dvmh_log.h"
#include "dvmh_pieces.h"
#include "dvmh_rts.h"
#include "loop.h"
#include "mps.h"
#include "util.h"

extern "C" void eiof_();  //io stat
extern "C" void biof_();  //io stat
extern "C" void bcopy_();  //io stat
extern "C" void ecopy_();  //io stat
using namespace libdvmh;

namespace libdvmh {

static bool testAndClear(char *buf, const char *accept) {
    char *s;
    bool res = false;
    if ((s = strpbrk(buf, accept))) {
        res = true;
        while (*(s + 1)) {
            *s = *(s + 1);
            s++;
        }
        *s = 0;
    }
    return res;
}

static bool localizeFileName(char *outBuf, const char *origName) {
    bool res = false;
    if (strchr(origName, '%')) {
        outBuf[sprintf(outBuf, origName, currentMPS->getCommRank())] = 0;
        res = true;
    } else {
        strcpy(outBuf, origName);
    }
    return res;
}

static TaskQueue *taskQueue = 0;
static THREAD_LOCAL bool isIOThread = false;

DvmhFile *DvmhFile::wrapFILE(FILE *stream, bool local, bool parallel, bool async) {
    DvmhFile *res = new DvmhFile;
    res->wrap(stream, local, parallel, async);
    return res;
}

DvmhFile *DvmhFile::openNew(const char *filename, const char *mode) {
    DvmhFile *res = new DvmhFile;
    if (!res->open(filename, mode)) {
        delete res;
        res = 0;
    }
    return res;
}

DvmhFile *DvmhFile::openTemporary(bool local, bool parallel, bool async) {
    DvmhFile *res = new DvmhFile;
    if (!res->openTmp(local, parallel, async)) {
        delete res;
        res = 0;
    }
    return res;
}

bool DvmhFile::openTmp(bool local, bool parallel, bool async) {
    checkClosed();
    reset();
    setLocal(local);
    setParallel(parallel);
    setAsync(async);
    stream = 0;
    int err = 0;
    if (hasStream()) {
        if (!parallelFlag) {
            stream = tmpfile();
        } else {
            // XXX: Works only if dvmh_tmpnam gives a name on the shared file system, what is quite uncommon.
            char fn[L_tmpnam];
            if (dvmh_tmpnam(fn)) {
                fileName = fn;
                if (isIOProc())
                    removeOnClose = true;
                stream = fopen(fn, "w+b");
            }
        }
        err = stream == 0;
    }
    err = uniteIntErr(err, true);
    if (err) {
        bool wasParallel = parallelFlag;
        cleanupOpened();
        if (wasParallel) {
            // Try to open it in a non-parallel mode
            err = openTmp(local, false, async) ? 0 : 1;
            if (!err)
                dvmh_log(DEBUG, "File was opened in a sequential mode due to errors while trying to open it in a parallel mode");
        }
    }
    return err == 0;
}

bool DvmhFile::close() {
    checkMPS();
    syncOperations();
    int ret = 0;
    if (hasStream())
        ret = fclose(stream);
    ret = -uniteIntErr(-ret, true);
    if (removeOnClose)
        remove(fileName.c_str());
    reset();
    return ret == 0;
}

bool DvmhFile::flush() {
    checkMPS();
    syncOperations();
    int ret = 0;
    if (isIOProc())
        ret = fflush(stream);
    ret = -uniteIntErr(-ret, false);
    return ret == 0;
}

bool DvmhFile::open(const char *filename, const char *mode) {
    char tp[16];
    strcpy(tp, mode);
    checkClosed();
    reset();
    setLocal(testAndClear(tp, "lL"));
    setParallel(testAndClear(tp, "pP"));
    setAsync(testAndClear(tp, "sS"));
    if (parallelFlag && strpbrk(tp, "aA") != 0) {
        setParallel(false);
        dvmhLogger.startMasterRegion();
        dvmh_log(NFERROR, "Append mode is not allowed to use with parallel I/O. Please, use \"r+\" mode instead.");
        dvmhLogger.endMasterRegion();
    }
    stream = 0;
    int err = 0;
    if (hasStream()) {
        if (localFlag) {
            char realFN[FILENAME_MAX + 1];
            localizeFileName(realFN, filename);
            fileName = realFN;
            stream = fopen(realFN, tp);
        } else {
            fileName = filename;
            stream = fopen(filename, tp);
        }
        err = stream == 0;
    }
    err = uniteIntErr(err, true);
    if (err) {
        bool wasParallel = parallelFlag;
        cleanupOpened();
        if (wasParallel) {
            // Try to open it in a non-parallel mode
            strcpy(tp, mode);
            testAndClear(tp, "pP");
            err = open(filename, tp) ? 0 : 1;
            if (!err)
                dvmh_log(DEBUG, "File was opened in a sequential mode due to errors while trying to open it in a parallel mode");
        }
    }
    return err == 0;
}

bool DvmhFile::changeMode(const char *mode) {
    char tp[16];
    strcpy(tp, mode);
    checkMPS();
    syncOperations();
    bool newLocal = testAndClear(tp, "lL");
    bool newParallel = testAndClear(tp, "pP");
    bool newAsync = testAndClear(tp, "sS");
    checkError2(localFlag == newLocal, "It is not allowed to change locality of the opened file. Close it and open instead.");
    if (newLocal || owningMPS->getCommSize() <= 1)
        newParallel = false;
    if (newParallel && strpbrk(tp, "aA") != 0) {
        newParallel = false;
        dvmhLogger.startMasterRegion();
        dvmh_log(NFERROR, "Append mode is not allowed to use with parallel I/O. Please, use \"r+\" mode instead.");
        dvmhLogger.endMasterRegion();
    }
    checkError2(parallelFlag == newParallel, "It is not allowed to change parallelism of the opened file. Close it and open instead.");
    setAsync(newAsync);
    int err = 0;
    if (hasStream())
        err = freopen(0, tp, stream) == 0;
    err = uniteIntErr(err, true);
    if (err)
        cleanupOpened();
    return err == 0;
}

bool DvmhFile::setBuffer(char *buf, int mode, size_t size) {
    checkMPS();
    syncOperations();
    int ret = 0;
    if (hasStream())
        ret = setvbuf(stream, buf, mode, size) != 0;
    ret = uniteIntErr(ret, true);
    return ret == 0;
}

class AsyncStringPutter: public Executable {
public:
    explicit AsyncStringPutter(DvmhFile *f, const char *s, bool deleteFlag): f(f) {
        if (!deleteFlag) {
            if (f->isIOProc()) {
                int len = strlen(s);
                this->s = new char[len + 1];
                memcpy(this->s, s, len + 1);
            } else {
                this->s = 0;
            }
        } else {
            this->s = const_cast<char *>(s);
        }
    }
public:
    virtual void execute() {
        if (f->isIOProc()) {
            fputs(s, f->stream);
            f->flushLocalIfAsync();
        }
    }
    virtual void execute(void *) { execute(); }
public:
    ~AsyncStringPutter() {
        delete[] s;
    }
protected:
    DvmhFile *f;
    char *s;
};

int DvmhFile::printFormatted(const char *format, va_list arg, bool noResult) {
    // XXX: This implementation lacks support for the '%n' format on other than I/O processors.
    // XXX: This implementation does not consume variadic arguments on other than I/O processors.
    //      C99 Standard describes this parameter's state on exit as indeterminate, so let it be.
    checkMPS();
    int res = EOF;
    if (asyncFlag && noResult) {
        int bufLen = 4096;
        char *printfBuffer = new char[bufLen + 1];
        int lenNeeded = vsnprintf(printfBuffer, bufLen, format, arg);
        checkError3(lenNeeded <= bufLen, "There is a length restriction for asynchronous printf operations. Maximum length is %d, needed %d.", bufLen,
                lenNeeded);
        commitAsyncOp(new AsyncStringPutter(this, printfBuffer, true));
    } else {
        syncOperations();
        if (isIOProc())
            res = vfprintf(stream, format, arg);
        if (!noResult) {
            if (!localFlag)
                ioBcast(res);
            else
                checkTheSame(res);
        }
    }
    return res;
}

class AsyncScanner: public Executable {
public:
    int res;
public:
    explicit AsyncScanner(DvmhFile *f, const char *format, va_list arg, bool noResult);
public:
    virtual void execute();
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    std::string format;
    std::vector<int> resultToToAssign;
    std::vector<std::pair<void *, UDvmType> > varParams;
    std::vector<std::pair<int, bool> > strVarIndices; // index + longFlag (wide char flag, in fact)
    bool noResult;
};

AsyncScanner::AsyncScanner(DvmhFile *f, const char *format, va_list arg, bool noResult): f(f), format(format), resultToToAssign(1, 0), noResult(noResult) {
    // XXX: This implementation treats the format string as a non-multibyte character string
    const char *s = format;
    while (*s) {
        if (*s == '%') {
            s++;
            bool noAssign = false;
            UDvmType varSize = 0;
            void *varAddr = 0;
            bool longFlag = false;
            bool assignCounts = true;
            UDvmType fieldWidth = 0;
            // No-assignment flag
            if (*s == '*') {
                s++;
                noAssign = true;
            }
            // Field width
            while (isdigit(*s)) {
                fieldWidth *= 10;
                fieldWidth += *s - '0';
                s++;
            }
            // Modifier
            if (*s == 'h') {
                s++;
                if (*s == 'h') {
                    s++;
                    varSize = sizeof(char);
                } else {
                    varSize = sizeof(short);
                }
            } else if (*s == 'l') {
                s++;
                if (*s == 'l') {
                    s++;
                    varSize = sizeof(long long);
                } else {
                    longFlag = true;
                }
            } else if (*s == 'j') {
                s++;
#ifdef HAVE_INTMAX_T
                varSize = sizeof(intmax_t);
#else
                varSize = sizeof(long long);
#endif
            } else if (*s == 'z') {
                s++;
                varSize = sizeof(size_t);
            } else if (*s == 't') {
                s++;
#ifdef HAVE_PTRDIFF_T
                varSize = sizeof(ptrdiff_t);
#else
                varSize = sizeof(char *);
#endif
            } else if (*s == 'L') {
                s++;
                varSize = sizeof(long double);
            }
            // Specifier
            if (*s == 'd' || *s == 'i' || *s == 'o' || *s == 'u' || *s == 'x' || *s == 'n') {
                if (*s == 'n')
                    assignCounts = false;
                s++;
                if (varSize == 0)
                    varSize = longFlag ? sizeof(long) : sizeof(int);
            } else if (*s == 'a' || *s == 'e' || *s == 'f' || *s == 'g') {
                s++;
                if (varSize == 0)
                    varSize = longFlag ? sizeof(double) : sizeof(float);
            } else if (*s == 'c') {
                s++;
                varSize = (longFlag ? sizeof(wchar_t) : sizeof(char)) * (fieldWidth > 0 ? fieldWidth : 1);
            } else if (*s == 's' || *s == '[') {
                if (*s == '[') {
                    s++;
                    if (*s == '^')
                        s++;
                    if (*s == ']')
                        s++;
                    while (*s != ']')
                        s++;
                }
                s++;
                if (!noAssign)
                    strVarIndices.push_back(std::make_pair(varParams.size(), longFlag));
            } else if (*s == 'P') {
                s++;
                varSize = sizeof(void *);
            } else if (*s == '%') {
                s++;
                noAssign = true;
            }
            if (!noAssign) {
                varAddr = va_arg(arg, void *);
                varParams.push_back(std::make_pair(varAddr, varSize));
                int prev = resultToToAssign.back();
                if (assignCounts)
                    resultToToAssign.push_back(prev + 1);
                else
                    resultToToAssign.back() = prev + 1;
            }
        } else {
            s++;
        }
    }
}

void AsyncScanner::execute() {
    res = EOF;
    if (f->isIOProc()) {
        void **allParams = new void *[2 + varParams.size()];
        allParams[0] = f->stream;
        allParams[1] = (void *)format.c_str();
        for (int i = 0; i < (int)varParams.size(); i++)
            allParams[2 + i] = varParams[i].first;
        res = executeFunction((DvmHandlerFunc)fscanf, allParams, 2 + varParams.size());
        delete[] allParams;
    }
    if (!f->isAlone()) {
        UDvmType bufSize = 0;
        char *buf = 0;
        if (f->isIOProc()) {
            int assignedCount = res >= 0 ? resultToToAssign[res] : 0;
            for (int i = 0; i < (int)strVarIndices.size(); i++) {
                int ind = strVarIndices[i].first;
                bool longFlag = strVarIndices[i].second;
                void *varAddr = varParams[ind].first;
                if (ind < assignedCount)
                    varParams[ind].second = longFlag ? sizeof(wchar_t) * (wcslen((wchar_t *)varAddr) + 1) : strlen((char *)varAddr) + 1;
            }
            bufSize = 1 * sizeof(int) + strVarIndices.size() * sizeof(UDvmType);
            for (int i = 0; i < assignedCount; i++)
                bufSize += varParams[i].second;
            buf = new char[bufSize];
            BufferWalker bufWalk(buf, bufSize);
            bufWalk.putValue(res);
            for (int i = 0; i < (int)strVarIndices.size(); i++)
                bufWalk.putValue(varParams[strVarIndices[i].first].second);
            for (int i = 0; i < assignedCount; i++)
                bufWalk.putData(varParams[i].first, varParams[i].second);
        }
        f->ioBcast(&buf, &bufSize);
        if (!f->isIOProc()) {
            BufferWalker bufWalk(buf, bufSize);
            bufWalk.extractValue(res);
            int assignedCount = res >= 0 ? resultToToAssign[res] : 0;
            for (int i = 0; i < (int)strVarIndices.size(); i++)
                bufWalk.extractValue(varParams[strVarIndices[i].first].second);
            for (int i = 0; i < assignedCount; i++)
                bufWalk.extractData(varParams[i].first, varParams[i].second);
        }
        delete[] buf;
    } else if (!noResult) {
        f->checkTheSame(res);
    }
}

int DvmhFile::scanFormatted(const char *format, va_list arg, bool noResult) {
    checkMPS();
    AsyncScanner *op = new AsyncScanner(this, format, arg, noResult);
    int res = EOF;
    if (noResult) {
        commitAsyncOp(op, true);
    } else {
        syncOperations();
        op->execute();
        res = op->res;
        delete op;
    }
    return res;
}

int DvmhFile::getChar() {
    checkMPS();
    syncOperations();
    int res = EOF;
    if (isIOProc())
        res = fgetc(stream);
    if (!localFlag)
        ioBcast(res);
    else
        checkTheSame(res);
    return res;
}

class AsyncLineGetter: public Executable {
public:
    int err;
public:
    explicit AsyncLineGetter(DvmhFile *f, char *s, int n, bool noResult): f(f), s(s), n(n), noResult(noResult) {}
public:
    virtual void execute() {
        err = 0;
        if (f->isIOProc())
            err = fgets(s, n, f->stream) == s ? 0 : 1;
        if (!f->isAlone()) {
            char *buf = 0;
            UDvmType bufSize = 0;
            if (f->isIOProc()) {
                UDvmType myStrSize = err ? 0 : strlen(s) + 1;
                bufSize = sizeof(int) + sizeof(UDvmType) + myStrSize;
                buf = new char[bufSize];
                BufferWalker bufWalk(buf, bufSize);
                bufWalk.putValue(err);
                bufWalk.putValue(myStrSize);
                bufWalk.putData(s, myStrSize);
            }
            f->ioBcast(&buf, &bufSize);
            if (!f->isIOProc()) {
                UDvmType myStrSize;
                BufferWalker bufWalk(buf, bufSize);
                bufWalk.extractValue(err);
                bufWalk.extractValue(myStrSize);
                bufWalk.extractData(s, myStrSize);
            }
            delete[] buf;
        } else if (!noResult) {
            f->checkTheSame(err);
        }
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    char *s;
    int n;
    bool noResult;
};

bool DvmhFile::getLine(char *s, int n, bool noResult) {
    checkMPS();
    AsyncLineGetter *op = new AsyncLineGetter(this, s, n, noResult);
    int err = 0;
    if (noResult) {
        commitAsyncOp(op, true);
    } else {
        syncOperations();
        op->execute();
        err = op->err;
        delete op;
    }
    return err == 0;
}

class AsyncCharPutter: public Executable {
public:
    explicit AsyncCharPutter(DvmhFile *f, int c): f(f), c(c) {}
public:
    virtual void execute() {
        if (f->isIOProc()) {
            fputc(c, f->stream);
            f->flushLocalIfAsync();
        }
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    int c;
};

int DvmhFile::putChar(int c, bool noResult) {
    checkMPS();
    int res = EOF;
    if (asyncFlag && noResult) {
        commitAsyncOp(new AsyncCharPutter(this, c));
    } else {
        syncOperations();
        if (isIOProc())
            res = fputc(c, stream);
        if (!noResult) {
            if (!localFlag)
                ioBcast(res);
            else
                checkTheSame(res);
        }
    }
    return res;
}

bool DvmhFile::putString(const char *s, bool noResult) {
    checkMPS();
    bool res = false;
    if (asyncFlag && noResult) {
        commitAsyncOp(new AsyncStringPutter(this, s, false));
    } else {
        syncOperations();
        if (isIOProc())
            res = fputs(s, stream) >= 0;
        if (!noResult) {
            if (!localFlag)
                ioBcast(res);
            else
                checkTheSame(res);
        }
    }
    return res;
}

class AsyncCharUngetter: public Executable {
public:
    explicit AsyncCharUngetter(DvmhFile *f, int c): f(f), c(c) {}
public:
    virtual void execute() {
        if (f->isIOProc())
            ungetc(c, f->stream);
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    int c;
};

bool DvmhFile::ungetChar(int c, bool noResult) {
    checkMPS();
    bool res = false;
    if (parallelFlag) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Doing ungetc on a parallel I/O file may lead to errors if the ungetted char is intended to be read by the fread function");
        dvmhLogger.endMasterRegion();
    }
    if (asyncFlag && noResult) {
        commitAsyncOp(new AsyncCharUngetter(this, c));
    } else {
        syncOperations();
        if (isIOProc())
            res = ungetc(c, stream) == c;
        if (!noResult) {
            if (!localFlag)
                ioBcast(res);
            else
                checkTheSame(res);
        }
    }
    return res;
}

class AsyncReader: public Executable {
public:
    UDvmType res;
public:
    explicit AsyncReader(DvmhFile *f, void *ptr, UDvmType size, UDvmType nmemb, bool noResult): f(f), ptr(ptr), size(size), nmemb(nmemb), noResult(noResult) {}
public:
    virtual void execute();
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    void *ptr;
    UDvmType size;
    UDvmType nmemb;
    bool noResult;
};

void AsyncReader::execute() {
    res = 0;
    if (f->parallelFlag && nmemb > 1 && size * nmemb >= dvmhSettings.parallelIOThreshold) {
        f->syncParallelBefore();
        int procCount = f->owningMPS->getCommSize();
        int myRank = f->owningMPS->getCommRank();
        UDvmType *blocks = new UDvmType[procCount];
        UDvmType minBlock = nmemb / procCount;
        UDvmType elemsToAdd = nmemb % procCount;
        UDvmType curRemainder = 0;
        UDvmType elemsBefore = 0;
        for (int p = 0; p < procCount; p++) {
            curRemainder = (curRemainder + elemsToAdd) % procCount;
            UDvmType curBlock = minBlock + (curRemainder < elemsToAdd);
            blocks[p] = curBlock;
            if (p < myRank)
                elemsBefore += curBlock;
        }
        UDvmType myBlock = blocks[myRank];
        int seekErr = f->myFseek(size * elemsBefore, SEEK_CUR);
        UDvmType myRes = 0;
        if (!seekErr)
            myRes = fread((char *)ptr + size * elemsBefore, size, myBlock, f->stream);
        std::pair<int, int> errRank;
        errRank.first = myRes < myBlock;
        errRank.second = myRank;
        f->myComm->allreduce(errRank, rf_MAXLOC);
        if (errRank.first) {
            if (myRank == errRank.second)
                res = elemsBefore + myRes;
            bcopy_();  //io stat
            f->myComm->bcast(errRank.second, res);
            ecopy_();  //io stat
        } else {
            res = nmemb;
        }
        UDvmType *factSizes = new UDvmType[procCount];
        UDvmType elemsLeft = res;
        for (int p = 0; p < procCount; p++) {
            UDvmType curBlock = std::min(elemsLeft, blocks[p]);
            factSizes[p] = size * curBlock;
            elemsLeft -= curBlock;
        }
        f->myComm->allgatherv(ptr, factSizes);
        delete[] factSizes;
        delete[] blocks;
        int lastRank = (errRank.first ? errRank.second : procCount - 1);
        f->syncParallelAfter(lastRank);
    } else {
        if (f->isIOProc())
            res = fread(ptr, size, nmemb, f->stream);
        if (!f->isAlone() && size * nmemb < 1024) {
            char *buf = 0;
            UDvmType bufSize = 0;
            if (f->isIOProc()) {
                bufSize = sizeof(UDvmType) + size * res;
                buf = new char[bufSize];
                BufferWalker bw(buf, bufSize);
                bw.putValue(res);
                bw.putData(ptr, size * res);
            }
            f->ioBcast(&buf, &bufSize);
            if (!f->isIOProc()) {
                BufferWalker bw(buf, bufSize);
                bw.extractValue(res);
                bw.extractData(ptr, size * res);
            }
            delete[] buf;
        } else if (!f->isAlone()) {
            f->ioBcast(res);
            f->ioBcast(ptr, size * res);
        } else if (!noResult) {
            f->checkTheSame(res);
        }
    }
}

UDvmType DvmhFile::read(void *ptr, UDvmType size, UDvmType nmemb, bool noResult) {
    checkMPS();
    if (size == 0)
        nmemb = 0;
    AsyncReader *op = new AsyncReader(this, ptr, size, nmemb, noResult);
    UDvmType res = 0;
    if (noResult) {
        commitAsyncOp(op, true);
    } else {
        syncOperations();
        op->execute();
        res = op->res;
        delete op;
    }
    return res;
}

class AsyncSeqWriter: public Executable {
public:
    explicit AsyncSeqWriter(DvmhFile *f, const void *ptr, UDvmType size, UDvmType nmemb): f(f), size(size), nmemb(nmemb) {
        this->ptr = new char[size * nmemb];
        memcpy(this->ptr, ptr, size * nmemb);
    }
public:
    virtual void execute() {
        if (f->isIOProc()) {
            fwrite(ptr, size, nmemb, f->stream);
            f->flushLocalIfAsync();
        }
    }
    virtual void execute(void *) { execute(); }
public:
    ~AsyncSeqWriter() {
        delete[] ptr;
    }
protected:
    DvmhFile *f;
    char *ptr;
    UDvmType size;
    UDvmType nmemb;
};

class AsyncParWriter: public Executable {
public:
    UDvmType res;
public:
    explicit AsyncParWriter(DvmhFile *f, const void *ptr, UDvmType size, UDvmType nmemb, bool bufferize): f(f), size(size), nmemb(nmemb), bufferized(bufferize)
    {
        if (!bufferize) {
            this->ptr = (char *)ptr;
        } else {
            this->ptr = new char[size * nmemb];
            memcpy(this->ptr, ptr, size * nmemb);
        }
    }
public:
    virtual void execute();
    virtual void execute(void *) { execute(); }
public:
    ~AsyncParWriter() {
        if (bufferized)
            delete[] ptr;
    }
protected:
    DvmhFile *f;
    char *ptr;
    UDvmType size;
    UDvmType nmemb;
    bool bufferized;
};

void AsyncParWriter::execute() {
    res = 0;
    f->syncParallelBefore(size * nmemb);
    int procCount = f->owningMPS->getCommSize();
    int myRank = f->owningMPS->getCommRank();
    UDvmType minBlock = nmemb / procCount;
    UDvmType elemsToAdd = nmemb % procCount;
    UDvmType curRemainder = 0;
    UDvmType myBlock = 0;
    UDvmType elemsBefore = 0;
    for (int p = 0; p < procCount; p++) {
        curRemainder = (curRemainder + elemsToAdd) % procCount;
        UDvmType curBlock = minBlock + (curRemainder < elemsToAdd);
        if (p < myRank) {
            elemsBefore += curBlock;
        } else if (p == myRank) {
            myBlock = curBlock;
            break;
        }
    }
    int seekErr = f->myFseek(size * elemsBefore, SEEK_CUR);
    UDvmType myRes = 0;
    if (!seekErr)
        myRes = fwrite(ptr + size * elemsBefore, size, myBlock, f->stream);
    std::pair<int, int> errRank;
    errRank.first = myRes < myBlock;
    errRank.second = myRank;
    f->myComm->allreduce(errRank, rf_MAXLOC);
    if (errRank.first) {
        if (myRank == errRank.second)
            res = elemsBefore + myRes;
        bcopy_();  //io stat
        f->myComm->bcast(errRank.second, res);
        ecopy_();  //io stat
    } else {
        res = nmemb;
    }
    int lastRank = (errRank.first ? errRank.second : procCount - 1);
    f->syncParallelAfter(lastRank);
}

UDvmType DvmhFile::write(const void *ptr, UDvmType size, UDvmType nmemb, bool noResult) {
    checkMPS();
    if (size == 0)
        nmemb = 0;
    UDvmType res = 0;
    if (parallelFlag && nmemb > 1 && size * nmemb >= dvmhSettings.parallelIOThreshold) {
        AsyncParWriter *op = new AsyncParWriter(this, ptr, size, nmemb, isFullyAsync() && noResult);
        if (noResult) {
            commitAsyncOp(op, true);
        } else {
            syncOperations();
            op->execute();
            res = op->res;
            delete op;
        }
    } else {
        if (asyncFlag && noResult) {
            commitAsyncOp(new AsyncSeqWriter(this, ptr, size, nmemb));
        } else {
            syncOperations();
            if (isIOProc())
                res = fwrite(ptr, size, nmemb, stream);
            if (!noResult) {
                if (!localFlag)
                    ioBcast(res);
                else
                    checkTheSame(res);
            }
        }
    }
    return res;
}

bool DvmhFile::getPosition(fpos_t *pos) const {
    checkMPS();
    syncOperations();
    int err = 0;
    if (isIOProc())
        err = fgetpos(stream, pos) != 0;
    err = uniteIntErr(err, false);
    return err == 0;
}

class AsyncSeeker: public Executable {
public:
    explicit AsyncSeeker(DvmhFile *f, long long offset, int whence): f(f), offset(offset), whence(whence) {}
public:
    virtual void execute() {
        if (f->isIOProc())
            f->myFseek(offset, whence);
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    long long offset;
    int whence;
};

bool DvmhFile::seek(long long offset, int whence, bool noResult) {
    checkMPS();
    int err = 0;
    if (asyncFlag && noResult) {
        commitAsyncOp(new AsyncSeeker(this, offset, whence));
    } else {
        syncOperations();
        if (isIOProc())
            err = myFseek(offset, whence) != 0;
        if (!noResult)
            err = uniteIntErr(err, false);
    }
    return err == 0;
}

bool DvmhFile::setPosition(const fpos_t *pos) {
    // XXX: No asynchronous option because according to the standard it sets errno on failure
    checkMPS();
    syncOperations();
    int err = 0;
    if (isIOProc())
        err = fsetpos(stream, pos) != 0;
    err = uniteIntErr(err, false);
    return err == 0;
}

long long DvmhFile::tell() const {
    checkMPS();
    syncOperations();
    struct {
        long long pos;
        int errNo;
    } buf = {-1, 0};
    if (isIOProc()) {
        buf.pos = myFtell();
        buf.errNo = errno;
    }
    ioBcast(&buf, sizeof(buf));
    // XXX: No checkTheSame here because local files can differ by size
    if (buf.pos < 0)
        errno = buf.errNo;
    return buf.pos;
}

class AsyncRewinder: public Executable {
public:
    explicit AsyncRewinder(DvmhFile *f): f(f) {}
public:
    virtual void execute() {
        if (f->isIOProc())
            rewind(f->stream);
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
};

bool DvmhFile::rewind() {
    checkMPS();
    commitAsyncOp(new AsyncRewinder(this));
    return true;
}

class AsyncErrorsClearer: public Executable {
public:
    explicit AsyncErrorsClearer(DvmhFile *f): f(f) {}
public:
    virtual void execute() {
        if (f->isIOProc())
            clearerr(f->stream);
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
};

void DvmhFile::clearErrors() {
    checkMPS();
    commitAsyncOp(new AsyncErrorsClearer(this));
}

bool DvmhFile::eof() const {
    checkMPS();
    syncOperations();
    bool res = false;
    if (isIOProc())
        res = feof(stream) != 0;
    if (!localFlag)
        ioBcast(res);
    else
        checkTheSame(res);
    return res;
}

bool DvmhFile::isInErrorState() const {
    checkMPS();
    syncOperations();
    int err = 0;
    if (isIOProc())
        err = ferror(stream) != 0;
    err = uniteIntErr(err, false);
    return err != 0;
}

class AsyncTruncater: public Executable {
public:
    int err;
public:
    explicit AsyncTruncater(DvmhFile *f, bool noResult): f(f), noResult(noResult) {}
public:
    virtual void execute() {
        err = 0;
        if (f->isIOProc()) {
            fflush(f->stream);
#ifndef WIN32
            err = ftruncate(fileno(f->stream), f->myFtell()) != 0;
#else
            err = _chsize_s(_fileno(f->stream), f->myFtell()) != 0;
#endif
        }
        if (!noResult)
            err = f->uniteIntErr(err, false);
    }
    virtual void execute(void *) { execute(); }
protected:
    DvmhFile *f;
    bool noResult;
};

bool DvmhFile::truncate(bool noResult) {
    checkMPS();
    AsyncTruncater *op = new AsyncTruncater(this, noResult);
    int err = 0;
    if (asyncFlag && noResult) {
        commitAsyncOp(op);
    } else {
        syncOperations();
        op->execute();
        err = op->err;
        delete op;
    }
    return err == 0;
}

void DvmhFile::commitAsyncOp(Executable *op, bool demandsFullyAsync) {
    if (asyncFlag && (!demandsFullyAsync || isFullyAsync())) {
        DependentTask *task;
        if (isa<DependentTask>(op)) {
            task = asa<DependentTask>(op);
        } else {
            task = new DependentTask;
            task->appendTask(op);
        }
        lastAsyncOpEnd->addDependent(task);
        delete lastAsyncOpEnd;
        lastAsyncOpEnd = task->createEndEvent();
        taskQueue->commitTask(task);
    } else {
        syncOperations();
        op->execute();
        delete op;
    }
}

void DvmhFile::syncOperations() const {
    if (asyncFlag) {
        mut.lock();
        isWaitingNow = true;
        mut.unlock();
        lastAsyncOpEnd->wait();
        mut.lock();
        isWaitingNow = false;
        mut.unlock();
    }
}

int DvmhFile::seekLocal(long long offset, int whence) {
    assert(hasStream());
    int result = myFseek(offset, whence);
    return result;
}

UDvmType DvmhFile::readLocal(void *ptr, UDvmType size, UDvmType nmemb) {
    assert(hasStream());
    UDvmType result = fread(ptr, size, nmemb, stream);
    return result;
}

UDvmType DvmhFile::writeLocal(void *ptr, UDvmType size, UDvmType nmemb) {
    assert(hasStream());
    UDvmType result = fwrite(ptr, size, nmemb, stream);
    return result;
}

void DvmhFile::flushLocalIfAsync() {
    assert(hasStream());
    if (isIOThread) {
        mut.lock();
        bool doSync = !isWaitingNow;
        mut.unlock();
        if (doSync)
            syncToFS();
    }
}

void DvmhFile::checkMPS() const {
    if (!localFlag) {
        checkError2(isMainThread && !isInParloop, "File operations on a global file are permitted only from the sequential part of the program");
        checkError2(currentMPS->isSimilarTo(owningMPS), "File operations on a global file are allowed only in the task which owns the file");
    }
}

void DvmhFile::syncParallelBefore(UDvmType reserveBytes) {
    static const char msg[] = "Error encountered while preparing a parallel I/O operation";
    if (parallelFlag) {
        struct {
            fpos_t pos;
            int err;
        } buf;
        if (isIOProc()) {
            if (reserveBytes > 0) {
                fpos_t pos;
                fgetpos(stream, &pos);
                myFseek(reserveBytes - 1, SEEK_CUR);
                char c = 0;
                fwrite(&c, 1, 1, stream);
                fsetpos(stream, &pos);
            }
            syncToFS(true);
            buf.err = fgetpos(stream, &buf.pos);
        }
        ioBcast(&buf, sizeof(buf));
        checkInternal2(!buf.err, msg);
        buf.err = fsetpos(stream, &buf.pos);
        myComm->allreduce(buf.err, rf_SUM);
        checkInternal2(!buf.err, msg);
    }
}

void DvmhFile::syncParallelAfter(int lastCommRank) {
    static const char msg[] = "Error encountered while finishing a parallel I/O operation";
    if (parallelFlag) {
        syncToFS();
        myComm->barrier();
        if (lastCommRank < 0) {
            // The last is unknown, try to figure it out by the means of ftell
            long long pos;
            pos = myFtell();
            myComm->allreduce(pos, rf_MAX);
            checkInternal2(pos >= 0, msg);
            int err = 0;
            if (isIOProc())
                err = myFseek(pos, SEEK_SET);
            ioBcast(err);
            checkInternal2(!err, msg);
        } else {
            // The last is specified, use its position
            int curRoot = lastCommRank;
            struct {
                fpos_t pos;
                int err;
            } buf;
            if (owningMPS->getCommRank() == curRoot)
                buf.err = fgetpos(stream, &buf.pos);
            bcopy_();  //io stat
            myComm->bcast(curRoot, &buf, sizeof(buf));
            ecopy_();  //io stat
            checkInternal2(!buf.err, msg);
            if (isIOProc())
                buf.err = fsetpos(stream, &buf.pos);
            ioBcast(buf.err);
            checkInternal2(!buf.err, msg);
        }
    }
}

bool DvmhFile::isAlone() const {
    bool result = localFlag || owningMPS->getCommSize() <= 1;
    return result;
}

bool DvmhFile::isIOProc() const {
    bool result = localFlag || owningMPS->isIOProc();
    return result;
}

void DvmhFile::init() {
    setLocal(true);
    setParallel(false);
    asyncFlag = false;
    lastAsyncOpEnd = new HappenedEvent();
    stream = 0;
    fileName.clear();
    removeOnClose = false;
    myComm = 0;
    ownComm = false;
    isWaitingNow = false;
}

void DvmhFile::reset() {
    if (!localFlag)
        owningMPS->deleteFile(fid);
    delete lastAsyncOpEnd;
    if (ownComm)
        delete myComm;
    init();
}

void DvmhFile::setLocal(bool local) {
    localFlag = local;
    if (!localFlag) {
        owningMPS = currentMPS;
        fid = owningMPS->newFile(this);
        myComm = owningMPS;
        ownComm = false;
    } else {
        owningMPS = 0;
        fid = -1;
        myComm = 0;
        ownComm = false;
        parallelFlag = false;
    }
}

void DvmhFile::setAsync(bool async) {
    if (ownComm && !async) {
        delete myComm;
        myComm = owningMPS;
        ownComm = false;
    }
    asyncFlag = taskQueue != 0 && async;
    if (asyncFlag && !localFlag && dvmhSettings.parallelMPI) {
        myComm = new DvmhCommunicator(*owningMPS);
        ownComm = true;
    }
}

void DvmhFile::ioBcast(void *buf, UDvmType size) const {
    if (!isAlone()){
    	bcopy_();  //io stat
        myComm->bcast(owningMPS->getIOProc(), buf, size);
        ecopy_();  //io stat
    }
}

void DvmhFile::ioBcast(char **pBuf, UDvmType *pSize) const {
    if (!isAlone()){
    	bcopy_();  //io stat
        myComm->bcast(owningMPS->getIOProc(), pBuf, pSize);
        ecopy_();  //io stat
    }
}

void DvmhFile::wrap(FILE *f, bool local, bool parallel, bool async) {
    checkClosed();
    reset();
    setLocal(local);
    setParallel(parallel);
    setAsync(async);
    if (hasStream())
        stream = f;
    else
        stream = 0;
}

bool DvmhFile::needsUnite() const {
    bool result = !isAlone() || (localFlag && isMainThread && !isInParloop && currentMPS->getCommSize() > 1);
    return result;
}

int DvmhFile::uniteIntErr(int err, bool collectiveOp) const {
    bool reduce = localFlag || (collectiveOp && parallelFlag);
    if (needsUnite()) {
        struct {
            int ret;
            int errNo;
        } buf = {0, 0};
        buf.ret = err;
        if (err)
            buf.errNo = errno;
        if (!reduce) {
            ioBcast(&buf, sizeof(buf));
        } else {
            (localFlag ? currentMPS : myComm)->allreduce(buf.ret, rf_MAX);
            if (buf.ret)
                (localFlag ? currentMPS : myComm)->allreduce(buf.errNo, rf_MAX);
        }
        err = buf.ret;
        if (err != 0)
            errno = buf.errNo;
    }
    return err;
}

template <typename T>
void DvmhFile::checkTheSame(const T &res) const {
    if (needsUnite()) {
        T minRes = res, maxRes = res;
        currentMPS->allreduce(minRes, rf_MIN);
        currentMPS->allreduce(maxRes, rf_MAX);
        checkError2(minRes == maxRes, "Inconsistent state of collective operation on bunch of local files detected.");
    }
}

template void DvmhFile::checkTheSame(const bool &res) const;
template void DvmhFile::checkTheSame(const int &res) const;
template void DvmhFile::checkTheSame(const UDvmType &res) const;

void DvmhFile::cleanupOpened() {
    int errNo = errno;
    if (stream)
        fclose(stream);
    if (removeOnClose)
        remove(fileName.c_str());
    reset();
    errno = errNo;
}

int DvmhFile::myFseek(long long offset, int whence) {
    int res;
#ifndef WIN32
    res = fseek(stream, offset, whence);
#else
    res = _fseeki64(stream, offset, whence);
#endif
    return res;
}

long long DvmhFile::myFtell() const {
    long long res;
#ifndef WIN32
    res = ftell(stream);
#else
    res = _ftelli64(stream);
#endif
    return res;
}

void DvmhFile::syncToFS(bool onlyIOProc) {
    if (onlyIOProc ? isIOProc() : hasStream()) {
        fflush(stream);
#if !defined(WIN32) && !defined(__APPLE__)
        fdatasync(fileno(stream));
#elif !defined(WIN32)
        fsync(fileno(stream));
#else
        _commit(_fileno(stream));
#endif
    }
}

void DvmhFile::checkClosed() const {
    checkInternal(localFlag && stream == 0);
}

FILE *dvmh_stdin = 0, *dvmh_stdout = 0, *dvmh_stderr = 0;

static DvmhFile *orig_stdin = 0, *orig_stdout = 0, *orig_stderr = 0;
static std::map<int, DvmhFile *> fortranFiles;

static void ioThreadSetup() {
    // Set broad affinity
    int procCount = getProcessorCount();
    cpu_set_t aff;
    CPU_ZERO(&aff);
    for (int i = 0; i < procCount; i++)
        CPU_SET(i, &aff);
    setAffinity(&aff);
    // Set flag
    isIOThread = true;
}

void stdioInit() {
    orig_stdin = DvmhFile::wrapFILE(stdin, false, false, false);
    orig_stdout = DvmhFile::wrapFILE(stdout, false, false, false);
    orig_stderr = DvmhFile::wrapFILE(stderr, false, false, false);
    dvmh_stdin = (FILE *)orig_stdin;
    dvmh_stdout = (FILE *)orig_stdout;
    dvmh_stderr = (FILE *)orig_stderr;
    if (dvmhSettings.ioThreadCount > 0) {
        taskQueue = new TaskQueue;
        for (int i = 0; i < dvmhSettings.ioThreadCount; i++)
            taskQueue->addDefaultPerformer(0, MethodExecutor::create(ioThreadSetup), 0);
        taskQueue->waitSleepingGrabbers(dvmhSettings.ioThreadCount);
    }
}

void stdioFinish() {
    for (std::map<int, DvmhFile *>::iterator it = fortranFiles.begin(); it != fortranFiles.end(); it++) {
        it->second->close();
        delete it->second;
    }
    fortranFiles.clear();
    orig_stdin->syncOperations();
    orig_stdout->syncOperations();
    orig_stderr->syncOperations();
    delete orig_stdin;
    delete orig_stdout;
    delete orig_stderr;
    orig_stdin = orig_stdout = orig_stderr = 0;
    if (taskQueue) {
        taskQueue->waitSleepingGrabbers(dvmhSettings.ioThreadCount);
        delete taskQueue;
        taskQueue = 0;
    }
    dvmh_stdin = dvmh_stdout = dvmh_stderr = 0;
}

static int uniteIntErr(int err, bool reduce) {
    if (isMainThread && !isInParloop && currentMPS->getCommSize() > 1) {
        struct {
            int ret;
            int errNo;
        } buf = {0, 0};
        buf.ret = err;
        if (err)
            buf.errNo = errno;
        if (!reduce) {
            currentMPS->ioBcast(&buf, sizeof(buf));
        } else {
            currentMPS->allreduce(buf.ret, rf_MAX);
            if (buf.ret)
                currentMPS->allreduce(buf.errNo, rf_MAX);
        }
        err = buf.ret;
        if (err != 0)
            errno = buf.errNo;
    }
    return err;
}

enum RWKind {rwkRead, rwkWrite};

}

extern "C" int dvmh_remove(const char *filename) {
    biof_(); //io stat
    // Supposes a global FS, only one processor does the action
    checkError2(isMainThread && !isInParloop, "Global file operations are permitted only from the sequential part of the program. Use dvmh_remove_local API function if appropriate.");
 
    int err = 0;
    if (currentMPS->isIOProc())
        err = remove(filename) != 0;
    err = uniteIntErr(err, false);
    eiof_();  
    return err;
}

extern "C" int dvmh_remove_local(const char *filename) {
    biof_();  //io stat
    // Supposes either a local FS, or different files on every processor
    char realFN[FILENAME_MAX + 1];
    localizeFileName(realFN, filename);
    int err = remove(realFN) != 0;
    err = uniteIntErr(err, true);
    eiof_();  
    return err;
}

extern "C" int dvmh_rename(const char *oldFN, const char *newFN) {
    biof_();  //io stat
    // Supposes a global FS, only one processor does the action
    checkError2(isMainThread && !isInParloop, "Global file operations are permitted only from the sequential part of the program. Use dvmh_rename_local API function if appropriate.");
    int err = 0;
    if (currentMPS->isIOProc())
        err = rename(oldFN, newFN) != 0;
    err = uniteIntErr(err, false);
    eiof_();  
    return err;
}

extern "C" int dvmh_rename_local(const char *oldFN, const char *newFN) {
    biof_();  //io stat
    // Supposes either a local FS, or different files on every processor
    char realOldFN[FILENAME_MAX + 1], realNewFN[FILENAME_MAX + 1];
    localizeFileName(realOldFN, oldFN);
    localizeFileName(realNewFN, newFN);
    int err = rename(realOldFN, realNewFN) != 0;
    err = uniteIntErr(err, true);
    eiof_();  
    return err;
}

extern "C" FILE *dvmh_tmpfile(void) {
    biof_();  //io stat
    // Global temporary file
    checkError2(isMainThread && !isInParloop, "Global file operations are permitted only from the sequential part of the program. Use dvmh_tmpfile_local API function if appropriate.");
    FILE * result = (FILE *)DvmhFile::openTemporary(false, false, false);
    eiof_();  
    return result;
}

extern "C" FILE *dvmh_tmpfile_local(void) {
    biof_();  //io stat
    // Local temporary file
    FILE * result = (FILE *)DvmhFile::openTemporary(true, false, false);
    eiof_();  
    return result;
}

extern "C" char *dvmh_tmpnam(char *s) {
    biof_();  //io stat
    // Global temporary file name, which could be useless if temporary folder is not shared
    checkError2(isMainThread && !isInParloop, "Global file operations are permitted only from the sequential part of the program. Use dvmh_tmpnam_local API function if appropriate.");
    if (currentMPS->getCommSize() == 1)
        return tmpnam(s);
    static char internalBuffer[L_tmpnam];
    struct {
        int res;
        int errNo;
        char str[L_tmpnam];
    } buf;
    char *res = 0;
    if (currentMPS->isIOProc()) {
        res = tmpnam(s);
        if (!res) {
            buf.res = -1;
            buf.errNo = errno;
        } else {
            buf.res = 0;
            strcpy(buf.str, res);
        }
    }
    currentMPS->ioBcast(&buf, sizeof(buf));
    if (buf.res != 0) {
        res = 0;
        errno = buf.errNo;
    } else if (s) {
        strcpy(s, buf.str);
        res = s;
    } else {
        strcpy(internalBuffer, buf.str);
        res = internalBuffer;
    }
    eiof_();  //io stat
    return res;
}

extern "C" char *dvmh_tmpnam_local(char *s) {
    biof_();  //io stat
    // Local temporary file name
    char *result = tmpnam(s);
    eiof_(); 
    return result;
}

extern "C" int dvmh_fclose(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fclose function");
    int res = stream->close() ? 0 : EOF;
    if (stream != orig_stdin && stream != orig_stdout && stream != orig_stderr)
        delete stream;
    eiof_();  
    return res;
}

extern "C" int dvmh_fflush(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    bool res = false;
    if (stream) {
        res = stream->flush();
    } else {
        res = true;
        for (int i = 0; i < currentMPS->getFileCount(); i++)
            res = currentMPS->getFile(i)->flush() && res;
    }
    eiof_(); 
    return res ? 0 : EOF;
}

extern "C" FILE *dvmh_fopen(const char *filename, const char *mode) {
     biof_();  //io stat
    FILE *result = (FILE *)DvmhFile::openNew(filename, mode);
    eiof_();  
    return result;
}

extern "C" FILE *dvmh_freopen(const char *filename, const char *mode, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the freopen function");
    bool res = false;
    if (filename) {
        stream->close();
        res = stream->open(filename, mode);
    } else {
        res = stream->changeMode(mode);
    }
    if (!res && stream != orig_stdin && stream != orig_stdout && stream != orig_stderr)
        delete stream;
    eiof_();  //io stat
    return res ? (FILE *)stream : 0;
}

extern "C" void dvmh_setbuf(FILE *astream, char *buf) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the setbuf function");
    stream->setBuffer(buf, (buf ? _IOFBF : _IONBF), BUFSIZ);
    eiof_();  //io stat
}

extern "C" int dvmh_setvbuf(FILE *astream, char *buf, int mode, size_t size) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the setvbuf function");
    int result = stream->setBuffer(buf, mode, size) ? 0 : EOF;
    eiof_();  //io stat
    return result;
}

extern "C" int dvmh_fprintf(FILE *stream, const char *format, ...) {
    biof_();  //io stat
    checkError2(stream, "NULL stream is passed to the fprintf function");
    int res;
    va_list ap;
    va_start(ap, format);
    res = dvmh_vfprintf(stream, format, ap);
    va_end(ap);
    eiof_();  //io stat
    return res;
}

extern "C" void dvmh_void_fprintf(FILE *stream, const char *format, ...) {
    biof_();  //io stat
    checkError2(stream, "NULL stream is passed to the fprintf function");
    va_list ap;
    va_start(ap, format);
    dvmh_void_vfprintf(stream, format, ap);
    va_end(ap);
    eiof_();  //io stat
}

extern "C" int dvmh_fscanf(FILE *stream, const char *format, ...) {
    biof_();  //io stat
    checkError2(stream, "NULL stream is passed to the fscanf function");
    int res;
    va_list ap;
    va_start(ap, format);
    res = dvmh_vfscanf(stream, format, ap);
    va_end(ap);
    eiof_();  //io stat
    return res;
}

extern "C" void dvmh_void_fscanf(FILE *stream, const char *format, ...) {
    biof_();  //io stat
    checkError2(stream, "NULL stream is passed to the fscanf function");
    va_list ap;
    va_start(ap, format);
    dvmh_void_vfscanf(stream, format, ap);
    va_end(ap);
    eiof_();  //io stat
}

extern "C" int dvmh_printf(const char *format, ...) {
    biof_();  //io stat
    int res;
    va_list ap;
    va_start(ap, format);
    res = dvmh_vfprintf((FILE *)orig_stdout, format, ap);
    va_end(ap);
    eiof_();  //io stat
    return res;
}

extern "C" void dvmh_void_printf(const char *format, ...) {
    biof_();  //io stat
    va_list ap;
    va_start(ap, format);
    dvmh_void_vfprintf((FILE *)orig_stdout, format, ap);
    va_end(ap);
    eiof_();  //io stat
}

extern "C" int dvmh_scanf(const char *format, ...) {
    biof_();  //io stat
    int res;
    va_list ap;
    va_start(ap, format);
    res = dvmh_vfscanf((FILE *)orig_stdin, format, ap);
    va_end(ap);
    eiof_();  //io stat
    return res;
}

extern "C" void dvmh_void_scanf(const char *format, ...) {
    biof_();  //io stat
    va_list ap;
    va_start(ap, format);
    dvmh_void_vfscanf((FILE *)orig_stdin, format, ap);
    va_end(ap);
    eiof_();  //io stat
}

extern "C" int dvmh_vfprintf(FILE *astream, const char *format, va_list arg) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the vfprintf function");
    int result = stream->printFormatted(format, arg);
    return result;
}

extern "C" void dvmh_void_vfprintf(FILE *astream, const char *format, va_list arg) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the vfprintf function");
    stream->printFormatted(format, arg, true);
}

extern "C" int dvmh_vfscanf(FILE *astream, const char *format, va_list arg) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the vfscanf function");
    int result = stream->scanFormatted(format, arg);
    return result;
}

extern "C" void dvmh_void_vfscanf(FILE *astream, const char *format, va_list arg) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the vfscanf function");
    stream->scanFormatted(format, arg, true);
}

extern "C" int dvmh_vprintf(const char *format, va_list arg) {
    return dvmh_vfprintf((FILE *)orig_stdout, format, arg);
}

extern "C" void dvmh_void_vprintf(const char *format, va_list arg) {
    dvmh_void_vfprintf((FILE *)orig_stdout, format, arg);
}

extern "C" int dvmh_vscanf(const char *format, va_list arg) {
    return dvmh_vfscanf((FILE *)orig_stdin, format, arg);
}

extern "C" void dvmh_void_vscanf(const char *format, va_list arg) {
    dvmh_void_vfscanf((FILE *)orig_stdin, format, arg);
}

extern "C" int dvmh_fgetc(FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fgetc function");
    int result = stream->getChar();
    return result;
}

extern "C" char *dvmh_fgets(char *s, int n, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fgets function");
    return stream->getLine(s, n) ? s : 0;
}

extern "C" void dvmh_void_fgets(char *s, int n, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fgets function");
    stream->getLine(s, n, true);
}

extern "C" int dvmh_fputc(int c, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fputc function");
    return stream->putChar(c);
}

extern "C" void dvmh_void_fputc(int c, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fputc function");
    stream->putChar(c, true);
}

extern "C" int dvmh_fputs(const char *s, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fputs function");
    return stream->putString(s) ? 0 : EOF;
}

extern "C" void dvmh_void_fputs(const char *s, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fputs function");
    stream->putString(s, true);
}

extern "C" int dvmh_getc(FILE *stream) {
    checkError2(stream, "NULL stream is passed to the getc function");
    return dvmh_fgetc(stream);
}

extern "C" int dvmh_getchar(void) {
    return dvmh_fgetc((FILE *)orig_stdin);
}

extern "C" char *dvmh_gets(char *s) {
    return dvmh_fgets(s, INT_MAX, (FILE *)orig_stdin);
}

extern "C" void dvmh_void_gets(char *s) {
    dvmh_void_fgets(s, INT_MAX, (FILE *)orig_stdin);
}

extern "C" int dvmh_putc(int c, FILE *stream) {
    checkError2(stream, "NULL stream is passed to the putc function");
    return dvmh_fputc(c, stream);
}

extern "C" void dvmh_void_putc(int c, FILE *stream) {
    checkError2(stream, "NULL stream is passed to the putc function");
    dvmh_void_fputc(c, stream);
}

extern "C" int dvmh_putchar(int c) {
    return dvmh_fputc(c, (FILE *)orig_stdout);
}

extern "C" void dvmh_void_putchar(int c) {
    dvmh_void_fputc(c, (FILE *)orig_stdout);
}

extern "C" int dvmh_puts(const char *s) {
    return dvmh_fputs(s, (FILE *)orig_stdout);
}

extern "C" void dvmh_void_puts(const char *s) {
    dvmh_void_fputs(s, (FILE *)orig_stdout);
}

extern "C" int dvmh_ungetc(int c, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the the ungetc function");
    return stream->ungetChar(c) ? c : EOF;
}

extern "C" void dvmh_void_ungetc(int c, FILE *astream) {
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the the ungetc function");
    stream->ungetChar(c, true);
}

static bool dvmhReadWriteRegular(DvmhData *data, UDvmType size, Interval inter, const Interval interSpace[], DvmhFile *stream, RWKind dir, UDvmType *pBytesDone,
        bool noResult) {
    // TODO: Move all replicated cases here, not only those, which can be dumped in-place
    DvmhBuffer *hbuff = data->getBuffer(0);
    checkInternal(!data->isDistributed() && hbuff->canDumpInplace(interSpace));
    int rank = data->getRank();
    UDvmType typeSize = data->getTypeSize();
    UDvmType nmemb = size > 0 ? (inter.size() * typeSize) / size : 0;
#ifdef NON_CONST_AUTOS
    DvmType startIndexes[rank];
#else
    DvmType startIndexes[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++)
        startIndexes[i] = interSpace[i][0];
    startIndexes[rank - 1] += inter[0];
    if (dir == rwkRead)
        *pBytesDone = stream->read(hbuff->getElemAddr(startIndexes), size, nmemb, noResult) * size;
    else
        *pBytesDone = stream->write(hbuff->getElemAddr(startIndexes), size, nmemb, noResult) * size;
    return noResult ? true : *pBytesDone == size * nmemb;
}

static UDvmType readWriteBytesLocal(void *buf, UDvmType size, UDvmType bytesCount, DvmhFile *stream, RWKind dir, UDvmType &partiallyDone) {
    UDvmType bytesDone = 0;
    char *bufPtr = (char *)buf;
    while (bytesDone < bytesCount) {
        UDvmType bytesRest = bytesCount - bytesDone;
        UDvmType bytesNow, sizeNow;
        if (partiallyDone > 0) {
            bytesNow = std::min(size - partiallyDone, bytesRest);
            sizeNow = bytesNow;
        } else if (bytesRest >= size) {
            bytesNow = roundDownU(bytesRest, size);
            sizeNow = size;
        } else {
            bytesNow = bytesRest;
            sizeNow = bytesNow;
        }
        UDvmType curDone;
        if (dir == rwkRead)
            curDone = stream->readLocal(bufPtr + bytesDone, sizeNow, bytesNow / sizeNow) * sizeNow;
        else
            curDone = stream->writeLocal(bufPtr + bytesDone, sizeNow, bytesNow / sizeNow) * sizeNow;
        bytesDone += curDone;
        partiallyDone = (partiallyDone + curDone) % size;
        if (curDone < bytesNow)
            break;
    }
    return bytesDone;
}

static UDvmType readWriteBytesLocal(void *buf, UDvmType size, UDvmType bytesCount, DvmhFile *stream, RWKind dir) {
    UDvmType partiallyDone = 0;
    return readWriteBytesLocal(buf, size, bytesCount, stream, dir, partiallyDone);
}

namespace libdvmh {

class AsyncPiecesRWLocal: public Executable {
public:
    UDvmType bytesCount;
    UDvmType bytesDone;
public:
    explicit AsyncPiecesRWLocal(DvmhFile *f, DvmhPieces *p, DvmhData *data, UDvmType size, RWKind dir);
public:
    virtual void execute();
    virtual void execute(void *) { execute(); }
public:
    ~AsyncPiecesRWLocal() {
        delete p;
        delete[] buf;
    }
protected:
    void prepare();
protected:
    DvmhFile *f;
    DvmhPieces *p;
    DvmhData *data;
    UDvmType size;
    RWKind dir;
    char *buf;
};

AsyncPiecesRWLocal::AsyncPiecesRWLocal(DvmhFile *f, DvmhPieces *p, DvmhData *data, UDvmType size, RWKind dir): f(f), p(p), data(data), size(size), dir(dir) {
    int rank = data->getRank();
    UDvmType typeSize = data->getTypeSize();
    DvmhBuffer *hbuff = data->getBuffer(0);
#ifdef NON_CONST_AUTOS
    Interval block[rank];
#else
    Interval block[MAX_ARRAY_RANK];
#endif
    bytesCount = 0;
    for (int i = 0; i < p->getCount(); i++) {
        if (data->hasLocal() && p->getPiece(i)->blockIntersect(rank, data->getLocalPart(), block)) {
            UDvmType bytesNow = typeSize * block->blockSize(rank);
            bytesCount += bytesNow;
        }
    }
    buf = new char[bytesCount];
    if (dir == rwkWrite) {
        UDvmType bytesDone = 0;
        for (int i = 0; i < p->getCount(); i++) {
            if (data->hasLocal() && p->getPiece(i)->blockIntersect(rank, data->getLocalPart(), block)) {
                UDvmType bytesNow = typeSize * block->blockSize(rank);
                DvmhBuffer bufWrap(rank, typeSize, 0, block, buf + bytesDone);
                hbuff->copyTo(&bufWrap, block);
                bytesDone += bytesNow;
            }
        }
    }
}

void AsyncPiecesRWLocal::execute() {
    bytesDone = readWriteBytesLocal(buf, size, bytesCount, f, dir);
    if (dir == rwkRead) {
        int rank = data->getRank();
        UDvmType typeSize = data->getTypeSize();
        DvmhBuffer *hbuff = data->getBuffer(0);
#ifdef NON_CONST_AUTOS
        Interval block[rank];
#else
        Interval block[MAX_ARRAY_RANK];
#endif
        UDvmType bytesDone = 0;
        for (int i = 0; i < p->getCount(); i++) {
            if (data->hasLocal() && p->getPiece(i)->blockIntersect(rank, data->getLocalPart(), block)) {
                UDvmType bytesNow = typeSize * block->blockSize(rank);
                DvmhBuffer bufWrap(rank, typeSize, 0, block, buf + bytesDone);
                bufWrap.copyTo(hbuff, block);
                bytesDone += bytesNow;
            }
        }
    } else {
        f->flushLocalIfAsync();
    }
}

static bool dvmhReadWriteLocal(DvmhData *data, UDvmType size, Interval inter, const Interval interSpace[], DvmhFile *stream, RWKind dir, UDvmType *pBytesDone,
        bool noResult) {
    checkInternal(stream->isLocal());
    checkError2(isMainThread && !isInParloop, "I/O operations with distributed arrays are permitted only from the sequential part of the program");
    bool res = false;
    int rank = data->getRank();
    // Perform the I/O operation
    AsyncPiecesRWLocal *op = new AsyncPiecesRWLocal(stream, DvmhPieces::createFromLinear(rank, inter, interSpace), data, size, dir);
    if (noResult) {
        *pBytesDone = 0;
        stream->commitAsyncOp(op);
        res = true;
    } else {
        stream->syncOperations();
        op->execute();
        *pBytesDone = op->bytesDone;
        res = op->bytesDone == op->bytesCount;
        delete op;
        if (currentMPS->getCommSize() > 1) {
            int err = res == false;
            currentMPS->allreduce(err, rf_MAX);
            res = err == 0;
            *pBytesDone = res ? inter.size() * data->getTypeSize() : 0;
        }
    }
    return res;
}

class AsyncReadWrite: public Executable {
public:
    UDvmType bytesCount;
    UDvmType bytesDone;
public:
    explicit AsyncReadWrite(DvmhFile *f, Interval inter, const Interval interSpace[], DvmhData *data, UDvmType size, RWKind dir);
public:
    virtual void execute() {
        if (!isParallel)
            executeSerial();
        else
            executeParallel();
    }
    virtual void execute(void *) { execute(); }
public:
    ~AsyncReadWrite() {
        delete[] interSpace;
        delete[] sends;
        if (dir == rwkWrite)
            delete hbuff;
    }
protected:
    void createSends();
    void executeSerial();
    void executeParallel();
protected:
    DvmhFile *f;
    Interval inter;
    Interval *interSpace;
    DvmhData *data;
    UDvmType size;
    RWKind dir;
    bool isParallel;
    int iSend;
    int *sends;
    DvmhBuffer *hbuff;
};

AsyncReadWrite::AsyncReadWrite(DvmhFile *f, Interval inter, const Interval interSpace[], DvmhData *data, UDvmType size, RWKind dir): f(f),
        inter(inter), data(data), size(size), dir(dir), sends(0) {
    int rank = data->getRank();
    this->interSpace = new Interval[rank];
    this->interSpace->blockAssign(rank, interSpace);
    if (dir == rwkWrite)
        checkError2(data->mpsCanRead(currentMPS), "Can not access all the data for reading (try realigning or redistributing)");
    else
        checkError2(data->mpsCanWrite(currentMPS), "Can not access all the data for writing (try realigning or redistributing)");
    isParallel = f->isParallel() && inter.size() * data->getTypeSize() >= dvmhSettings.parallelIOThreshold;
    createSends();
    hbuff = 0;
    if (iSend) {
        hbuff = data->getBuffer(0);
        if (dir == rwkWrite) {
            DvmhPieces *p = DvmhPieces::createFromLinear(rank, inter, interSpace);
            DvmhPieces p1(rank);
            p1.appendOne(data->getLocalPart());
            p1.intersectInplace(p);
            DvmhBuffer *myHrepr = new DvmhBuffer(rank, hbuff->getTypeSize(), 0, ~p1.getBoundRect().rect);
            hbuff->copyTo(myHrepr);
            hbuff = myHrepr;
            delete p;
        }
    }
}

void AsyncReadWrite::createSends() {
    const MultiprocessorSystem *ioMPS = f->getOwningMPS();
    int rank = data->getRank();
    bool isIOProc = ioMPS->isIOProc();
    int procCount = ioMPS->getCommSize();
    int myRank = ioMPS->getCommRank();
    int ioProc = ioMPS->getIOProc();
    if (isIOProc || isParallel)
        sends = new int[procCount];
    if (data->isDistributed()) {
        const DvmhAlignRule *alignRule = data->getAlignRule();
        DvmhDistribSpace *dspace = alignRule->getDspace();
        const DvmhDistribRule *distrRule = dspace->getDistribRule();
        int dspaceRank = dspace->getRank();
        const MultiprocessorSystem *dataMPS = distrRule->getMPS();
        int dataMpsRank = std::max(dataMPS->getRank(), distrRule->getMpsAxesUsed());
        int ioProcDataMpsCommRank = ioMPS->getOtherCommRank(dataMPS, ioProc);
        checkError2(ioProcDataMpsCommRank >= -1,
                "I/O operations on a distributed array, which is not related to the file's multiprocessor system are not supported");
#ifdef NON_CONST_AUTOS
        int onlyFromProc[dataMpsRank], procIndexes[dataMpsRank];
        Interval block[rank], ioProcPart[rank];
#else
        int onlyFromProc[MAX_MPS_RANK], procIndexes[MAX_MPS_RANK];
        Interval block[MAX_ARRAY_RANK], ioProcPart[MAX_ARRAY_RANK];
#endif
        for (int i = 0; i < rank; i++)
            ioProcPart[i] = Interval::createEmpty();
        if (ioProcDataMpsCommRank >= 0)
            data->fillLocalPart(ioProcDataMpsCommRank, ioProcPart);
        for (int p = 0; p < procCount; p++) {
            int sendsValue = 0;
            int dataMpsProc = ioMPS->getOtherCommRank(dataMPS, p);
            assert(dataMpsProc >= -1);
            if (dataMpsProc >= 0 && data->fillLocalPart(dataMpsProc, block)) {
                if (!isParallel && ioProcPart->blockContains(rank, block)) {
                    // TODO: Maybe do something similar (short path) for parallel I/O as well
                    if (p == ioProc)
                        sendsValue = p + 1;
                    else
                        sendsValue = (dir == rwkWrite ? 0 : -(ioProc + 1));
                } else {
                    for (int i = 0; i < dataMpsRank; i++)
                        onlyFromProc[i] = 0;
                    for (int i = 1; i <= dspaceRank; i++) {
                        if (!distrRule->getAxisRule(i)->isReplicated()) {
                            int mpsAxis = distrRule->getAxisRule(i)->getMPSAxis();
                            int dataAxis = alignRule->getAxisRule(i)->axisNumber;
                            if (dataAxis > 0) {
                                onlyFromProc[mpsAxis - 1] = -1;
                            } else {
                                DvmType leftMostIndex = (dataAxis == 0 ? alignRule->getAxisRule(i)->summand : alignRule->getAxisRule(i)->replicInterval.begin());
                                int proc = distrRule->getAxisRule(i)->getProcIndex(leftMostIndex);
                                onlyFromProc[mpsAxis - 1] = proc;
                            }
                        }
                    }
                    dataMPS->fillAxisIndexes(dataMpsProc, dataMpsRank, procIndexes);
                    bool ok = true;
                    for (int i = 0; i < dataMpsRank; i++) {
                        if (onlyFromProc[i] >= 0 && onlyFromProc[i] != procIndexes[i]) {
                            procIndexes[i] = onlyFromProc[i];
                            ok = false;
                        }
                    }
                    if (ok)
                        sendsValue = p + 1;
                    else
                        sendsValue = (dir == rwkWrite ? 0 : -(dataMPS->getOtherCommRank(ioMPS, dataMPS->getCommRank(dataMpsRank, procIndexes)) + 1));
                }
            }
            if (sends)
                sends[p] = sendsValue;
            if (p == myRank)
                iSend = sendsValue;
        }
    } else {
        // TODO: Handle this as a separate case
        if (!isParallel) {
            if (isIOProc) {
                for (int p = 0; p < procCount; p++)
                    sends[p] = (dir == rwkWrite ? 0 : -(ioProc + 1));
                sends[ioProc] = ioProc + 1;
                iSend = ioProc + 1;
            } else {
                iSend = (dir == rwkWrite ? 0 : -(ioProc + 1));
            }
        } else {
            for (int p = 0; p < procCount; p++)
                sends[p] = (dir == rwkWrite ? 0 : -(ioProc + 1));
            sends[ioProc] = ioProc + 1;
            iSend = sends[myRank];
        }
    }
}

void AsyncReadWrite::executeSerial() {
    UDvmType bufSize = dvmhSettings.maxIOBufSize;
    MultiprocessorSystem *ioMPS = f->getOwningMPS();
    bool isIOProc = ioMPS->isIOProc();
    int procCount = ioMPS->getCommSize();
    int myRank = ioMPS->getCommRank();
    int rank = data->getRank();
    const MultiprocessorSystem *dataMPS = data->isDistributed() ? data->getAlignRule()->getDspace()->getDistribRule()->getMPS() : 0;
    UDvmType typeSize = data->getTypeSize();
    UDvmType elemCount = inter.size();
    bytesCount = typeSize * elemCount;
    bytesDone = 0;
    UDvmType partiallyDone = 0;
#ifdef NON_CONST_AUTOS
    Interval block[rank], procBlock[rank];
#else
    Interval block[MAX_ARRAY_RANK], procBlock[MAX_ARRAY_RANK];
#endif
    char *buf = 0;
    if (isIOProc)
        buf = new char[bufSize];
    UDvmType maxElemsAtOnce = divDownU(bufSize, typeSize);
    UDvmType blockCount = divUpU(elemCount, maxElemsAtOnce);
    maxElemsAtOnce = divUpU(elemCount, blockCount);
    UDvmType elemsDone = 0;
    while (elemsDone < elemCount) {
        UDvmType elemsNow = std::min(maxElemsAtOnce, elemCount - elemsDone);
        DvmhPieces *curPieces = DvmhPieces::createFromLinear(rank, Interval::create(inter[0] + elemsDone, inter[0] + elemsDone + elemsNow - 1),
                interSpace);
        UDvmType curBytes = elemsNow * typeSize;
        UDvmType curDone = 0;
        // TODO: Add asynchronous behaviour here regarding MPI
        if (isIOProc) {
            if (dir == rwkRead)
                curDone = readWriteBytesLocal(buf, size, curBytes, f, dir, partiallyDone);
            for (int p = 0; p < procCount; p++) {
                if (sends[p]) {
                    if (data->isDistributed())
                        data->fillLocalPart(ioMPS->getOtherCommRank(dataMPS, p), procBlock);
                    else
                        procBlock->blockAssign(rank, data->getSpace());
                    if (p == myRank) {
                        UDvmType bufOffs = 0;
                        for (int i = 0; i < curPieces->getCount(); i++) {
                            const Interval *curPiece = curPieces->getPiece(i);
                            DvmhBuffer bufWrapper(rank, typeSize, 0, curPiece, buf + bufOffs);
                            if (curPiece->blockIntersect(rank, procBlock, block)) {
                                if (dir == rwkRead)
                                    bufWrapper.copyTo(hbuff, block);
                                else
                                    hbuff->copyTo(&bufWrapper, block);
                            }
                            bufOffs += bufWrapper.getSize();
                        }
                    } else {
                        UDvmType sendRecvBufSize = 0;
                        for (int i = 0; i < curPieces->getCount(); i++)
                            if (curPieces->getPiece(i)->blockIntersect(rank, procBlock, block))
                                sendRecvBufSize += block->blockSize(rank) * typeSize;
                        char *sendRecvBuf = new char[sendRecvBufSize];
                        if (dir == rwkWrite){
                        	bcopy_();  //io stat
                            f->getComm()->recv(p, sendRecvBuf, sendRecvBufSize);
                            ecopy_();  //io stat
                        }
                        UDvmType bufOffs = 0, sendRecvBufOffs = 0;
                        for (int i = 0; i < curPieces->getCount(); i++) {
                            const Interval *curPiece = curPieces->getPiece(i);
                            DvmhBuffer bufWrapper(rank, typeSize, 0, curPiece, buf + bufOffs);
                            if (curPiece->blockIntersect(rank, procBlock, block)) {
                                DvmhBuffer sendRecvBufWrapper(rank, typeSize, 0, block, sendRecvBuf + sendRecvBufOffs);
                                if (dir == rwkRead)
                                    bufWrapper.copyTo(&sendRecvBufWrapper, block);
                                else
                                    sendRecvBufWrapper.copyTo(&bufWrapper, block);
                                sendRecvBufOffs += sendRecvBufWrapper.getSize();
                            }
                            bufOffs += bufWrapper.getSize();
                        }
                        if (dir == rwkRead){
                        	bcopy_();  //io stat
                            f->getComm()->send(p, sendRecvBuf, sendRecvBufSize);
                            ecopy_();  //io stat
                        }
                        delete[] sendRecvBuf;
                    }
                }
            }
            if (dir == rwkWrite)
                curDone = readWriteBytesLocal(buf, size, curBytes, f, dir, partiallyDone);
        } else if (iSend) {
            const Interval *procBlock = data->getLocalPart();
            UDvmType sendRecvBufSize = 0;
            for (int i = 0; i < curPieces->getCount(); i++)
                if (curPieces->getPiece(i)->blockIntersect(rank, procBlock, block))
                    sendRecvBufSize += block->blockSize(rank) * typeSize;
            char *sendRecvBuf = new char[sendRecvBufSize];
            if (dir == rwkRead){
            	bcopy_();  //io stat
                f->getComm()->recv(ioMPS->getIOProc(), sendRecvBuf, sendRecvBufSize);
                ecopy_();  //io stat
            }
            UDvmType sendRecvBufOffs = 0;
            for (int i = 0; i < curPieces->getCount(); i++) {
                if (curPieces->getPiece(i)->blockIntersect(rank, procBlock, block)) {
                    DvmhBuffer sendRecvBufWrapper(rank, typeSize, 0, block, sendRecvBuf + sendRecvBufOffs);
                    if (dir == rwkRead)
                        sendRecvBufWrapper.copyTo(hbuff, block);
                    else
                        hbuff->copyTo(&sendRecvBufWrapper, block);
                    sendRecvBufOffs += sendRecvBufWrapper.getSize();
                }
            }
            if (dir == rwkWrite){
            	bcopy_();  //io stat
                f->getComm()->send(ioMPS->getIOProc(), sendRecvBuf, sendRecvBufSize);
                ecopy_();  //io stat
            }
            delete[] sendRecvBuf;
        }
        bcopy_();  //io stat
        f->getComm()->bcast(ioMPS->getIOProc(), curDone);
        ecopy_();  //io stat
        delete curPieces;
        elemsDone += elemsNow;
        bytesDone += curDone;
        if (curDone < curBytes)
            break;
    }
    delete[] buf;
}

void AsyncReadWrite::executeParallel() {
    MultiprocessorSystem *ioMPS = f->getOwningMPS();
    int procCount = ioMPS->getCommSize();
    int myRank = ioMPS->getCommRank();
    int rank = data->getRank();
    const MultiprocessorSystem *dataMPS = data->isDistributed() ? data->getAlignRule()->getDspace()->getDistribRule()->getMPS() : 0;
    UDvmType typeSize = data->getTypeSize();
    UDvmType elemCount = inter.size();
    bytesCount = typeSize * elemCount;
    bytesDone = 0;
    UDvmType partiallyDone = 0;
#ifdef NON_CONST_AUTOS
    Interval block[rank], procBlock[rank];
#else
    Interval block[MAX_ARRAY_RANK], procBlock[MAX_ARRAY_RANK];
#endif
    if (dir == rwkWrite)
        f->syncParallelBefore(bytesCount);
    else
        f->syncParallelBefore();
    Interval *writeIntervals = new Interval[procCount];
    UDvmType minBlock = elemCount / procCount;
    UDvmType elemsToAdd = elemCount % procCount;
    UDvmType curRemainder = 0;
    UDvmType elemsBefore = 0;
    for (int p = 0; p < procCount; p++) {
        curRemainder = (curRemainder + elemsToAdd) % procCount;
        UDvmType curBlock = minBlock + (curRemainder < elemsToAdd);
        writeIntervals[p][0] = (p == 0 ? inter[0] : writeIntervals[p - 1][1] + 1);
        writeIntervals[p][1] = writeIntervals[p][0] + (DvmType)curBlock - 1;
        if (p < myRank)
            elemsBefore += curBlock;
    }
    int lastRank = procCount - 1;
    char *writeBuf = new char[writeIntervals[myRank].size() * typeSize];
    if (dir == rwkRead) {
        int seekErr = f->seekLocal(elemsBefore * typeSize, SEEK_CUR);
        UDvmType myDone = 0;
        if (!seekErr)
            myDone = readWriteBytesLocal(writeBuf, size, writeIntervals[myRank].size() * typeSize, f, dir, partiallyDone);
        std::pair<int, int> errRank;
        errRank.first = myDone < writeIntervals[myRank].size() * typeSize;
        errRank.second = myRank;
        f->getComm()->allreduce(errRank, rf_MAXLOC);
        if (errRank.first) {
            UDvmType factElemCount = 0;
            if (myRank == errRank.second)
                factElemCount = elemsBefore + divDownU(myDone, typeSize);
            bcopy_();  //io stat
            f->getComm()->bcast(errRank.second, factElemCount);
            ecopy_();  //io stat
            UDvmType elemsLeft = factElemCount;
            for (int p = 0; p < procCount; p++) {
                UDvmType curBlock = std::min(elemsLeft, writeIntervals[p].size());
                writeIntervals[p][0] = (p == 0 ? inter[0] : writeIntervals[p - 1][1] + 1);
                writeIntervals[p][1] = writeIntervals[p][0] + (DvmType)curBlock - 1;
                elemsLeft -= curBlock;
            }
            lastRank = errRank.second;
        }
        bytesDone += Interval::create(writeIntervals[0][0], writeIntervals[procCount - 1][1]).size() * typeSize;
    }
    char *sendBuf = 0;
    if (iSend)
        sendBuf = new char[data->getLocalPart()->blockSize(rank) * typeSize];
    char *recvBuf = new char[writeIntervals[myRank].size() * typeSize];
    // TODO: Make somehow Alltoallv function substitution without limits on size
    int *sendcounts = new int[procCount];
    int *sdispls = new int[procCount];
    int *recvcounts = new int[procCount];
    int *rdispls = new int[procCount];
    UDvmType sendBufOffs = 0;
    for (int p = 0; p < procCount; p++) {
        UDvmType curProcSend = 0;
        if (iSend) {
            DvmhPieces *procPieces = DvmhPieces::createFromLinear(rank, writeIntervals[p], interSpace);
            for (int i = 0; i < procPieces->getCount(); i++) {
                if (procPieces->getPiece(i)->blockIntersect(rank, data->getLocalPart(), block)) {
                    if (dir == rwkWrite) {
                        DvmhBuffer sendBufWrap(rank, typeSize, 0, block, sendBuf + sendBufOffs + curProcSend);
                        hbuff->copyTo(&sendBufWrap, block);
                    }
                    curProcSend += block->blockSize(rank) * typeSize;
                }
            }
            delete procPieces;
        }
        assert(curProcSend <= INT_MAX);
        sendcounts[p] = curProcSend;
        assert(sendBufOffs <= INT_MAX);
        sdispls[p] = sendBufOffs;
        sendBufOffs += curProcSend;
    }
    UDvmType recvBufOffs = 0;
    DvmhPieces *myWritePieces = DvmhPieces::createFromLinear(rank, writeIntervals[myRank], interSpace);
    for (int p = 0; p < procCount; p++) {
        if (sends[p] > 0) {
            UDvmType curProcWrite = 0;
            if (data->isDistributed())
                data->fillLocalPart(ioMPS->getOtherCommRank(dataMPS, p), procBlock);
            else
                procBlock->blockAssign(rank, data->getSpace());
            UDvmType writeBufOffs = 0;
            for (int i = 0; i < myWritePieces->getCount(); i++) {
                const Interval *writePiece = myWritePieces->getPiece(i);
                if (writePiece->blockIntersect(rank, procBlock, block)) {
                    if (dir == rwkRead) {
                        DvmhBuffer writeBufWrap(rank, typeSize, 0, writePiece, writeBuf + writeBufOffs);
                        DvmhBuffer recvBufWrap(rank, typeSize, 0, block, recvBuf + recvBufOffs + curProcWrite);
                        writeBufWrap.copyTo(&recvBufWrap, block);
                    }
                    curProcWrite += block->blockSize(rank) * typeSize;
                }
                writeBufOffs += writePiece->blockSize(rank) * typeSize;
            }
            assert(curProcWrite <= INT_MAX);
            recvcounts[p] = curProcWrite;
            assert(recvBufOffs <= INT_MAX);
            rdispls[p] = recvBufOffs;
            recvBufOffs += curProcWrite;
        } else {
            recvcounts[p] = 0;
            rdispls[p] = 0;
        }
    }
    for (int p = 0; p < procCount; p++) {
        if (sends[p] < 0) {
           assert(dir == rwkRead);
           recvcounts[p] = recvcounts[(-sends[p]) - 1];
           rdispls[p] = rdispls[(-sends[p]) - 1];
        }
    }
    // TODO: Use DvmhCommunicator's methods
    if (dir == rwkRead)
        checkInternalMPI(MPI_Alltoallv(recvBuf, recvcounts, rdispls, MPI_BYTE, sendBuf, sendcounts, sdispls, MPI_BYTE, f->getComm()->getComm()));
    else
        checkInternalMPI(MPI_Alltoallv(sendBuf, sendcounts, sdispls, MPI_BYTE, recvBuf, recvcounts, rdispls, MPI_BYTE, f->getComm()->getComm()));
    if (dir == rwkRead && iSend) {
        for (int p = 0; p < procCount; p++) {
            UDvmType curProcSend = 0;
            DvmhPieces *procPieces = DvmhPieces::createFromLinear(rank, writeIntervals[p], interSpace);
            for (int i = 0; i < procPieces->getCount(); i++) {
                if (procPieces->getPiece(i)->blockIntersect(rank, data->getLocalPart(), block)) {
                    DvmhBuffer sendBufWrap(rank, typeSize, 0, block, sendBuf + sdispls[p] + curProcSend);
                    sendBufWrap.copyTo(hbuff, block);
                    curProcSend += block->blockSize(rank) * typeSize;
                }
            }
            delete procPieces;
        }
    }
    delete[] sendBuf;
    delete[] sendcounts;
    delete[] sdispls;
    if (dir == rwkWrite) {
        for (int p = 0; p < procCount; p++) {
            if (sends[p] > 0) {
                UDvmType curProcWrite = 0;
                if (data->isDistributed())
                    data->fillLocalPart(ioMPS->getOtherCommRank(dataMPS, p), procBlock);
                else
                    procBlock->blockAssign(rank, data->getSpace());
                UDvmType writeBufOffs = 0;
                for (int i = 0; i < myWritePieces->getCount(); i++) {
                    const Interval *writePiece = myWritePieces->getPiece(i);
                    if (writePiece->blockIntersect(rank, procBlock, block)) {
                        DvmhBuffer writeBufWrap(rank, typeSize, 0, writePiece, writeBuf + writeBufOffs);
                        DvmhBuffer recvBufWrap(rank, typeSize, 0, block, recvBuf + rdispls[p] + curProcWrite);
                        recvBufWrap.copyTo(&writeBufWrap, block);
                        curProcWrite += block->blockSize(rank) * typeSize;
                    }
                    writeBufOffs += writePiece->blockSize(rank) * typeSize;
                }
            }
        }
    }
    delete myWritePieces;
    delete[] recvBuf;
    delete[] recvcounts;
    delete[] rdispls;
    if (dir == rwkWrite) {
        int seekErr = f->seekLocal(elemsBefore * typeSize, SEEK_CUR);
        UDvmType myDone = 0;
        if (!seekErr)
            myDone = readWriteBytesLocal(writeBuf, size, writeIntervals[myRank].size() * typeSize, f, dir, partiallyDone);
        std::pair<int, int> errRank;
        errRank.first = myDone < writeIntervals[myRank].size() * typeSize;
        errRank.second = myRank;
        f->getComm()->allreduce(errRank, rf_MAXLOC);
        if (errRank.first) {
            UDvmType factElemCount = 0;
            if (myRank == errRank.second)
                factElemCount = elemsBefore + divDownU(myDone, typeSize);
            bcopy_();  //io stat
            f->getComm()->bcast(errRank.second, factElemCount);
            ecopy_();  //io stat
            lastRank = errRank.second;
            bytesDone += factElemCount * typeSize;
        } else {
            bytesDone += elemCount * typeSize;
        }
    }
    delete[] writeBuf;
    delete[] writeIntervals;
    f->syncParallelAfter(lastRank);
}

}

static bool dvmhReadWriteDistributed(DvmhData *data, UDvmType size, Interval inter, const Interval interSpace[], DvmhFile *stream, RWKind dir,
        UDvmType *pBytesDone, bool noResult) {
    // TODO: Handle only distributed
    checkInternal(!stream->isLocal());
    bool res = false;
    AsyncReadWrite *op = new AsyncReadWrite(stream, inter, interSpace, data, size, dir);
    if (noResult) {
        *pBytesDone = 0;
        stream->commitAsyncOp(op, true);
        res = true;
    } else {
        stream->syncOperations();
        op->execute();
        *pBytesDone = op->bytesDone;
        res = op->bytesDone == op->bytesCount;
        delete op;
    }
    return res;
}

static bool dvmhReadWriteGeneral(DvmhData *data, UDvmType size, Interval inter, const Interval interSpace[], DvmhFile *stream, RWKind dir, UDvmType *pItemsDone,
        bool noResult) {
    int rank = data->getRank();
    UDvmType typeSize = data->getTypeSize();
    checkInternal2(data->getSpace()->blockContains(rank, interSpace), "Invalid block");
    checkInternal2(Interval::create(0, (DvmType)interSpace->blockSize(rank) - 1).contains(inter), "Invalid interval");
    if (size > 0)
        checkInternal2((inter.size() * typeSize) % size == 0, "Invalid size");
    else
        checkInternal2(inter.empty(), "Invalid size");
    if (dir == rwkRead)
        data->syncAllAccesses();
    else
        data->syncWriteAccesses();
    DvmhBuffer *hbuff = data->getBuffer(0);
    if (data->hasLocal()) {
        // Get/set actual
        // XXX: we can economize on get_actual operations, but it doesn't seem to be important
        DvmhPieces *p = DvmhPieces::createFromLinear(rank, inter, interSpace);
        DvmhPieces *p1 = new DvmhPieces(rank);
        p1->appendOne(data->getLocalPart());
        p1->intersectInplace(p);
        delete p;
        if (dir == rwkWrite) {
            data->getActualBase(0, p1, 0, (currentRegion ? currentRegion->canAddToActual(data, p1) : true));
        } else {
            data->setActual(p1);
            if (currentRegion)
                currentRegion->markToRenew(data);
        }
        delete p1;
    }
    UDvmType bytesDone = 0;
    bool res = false;
    if (!data->isDistributed() && hbuff->canDumpInplace(interSpace)) {
        res = dvmhReadWriteRegular(data, size, inter, interSpace, stream, dir, &bytesDone, noResult);
    } else if (stream->isLocal()) {
        res = dvmhReadWriteLocal(data, size, inter, interSpace, stream, dir, &bytesDone, noResult);
    } else {
        res = dvmhReadWriteDistributed(data, size, inter, interSpace, stream, dir, &bytesDone, noResult);
    }
    if (pItemsDone)
        *pItemsDone = size > 0 ? divDownU(bytesDone, size) : 0;
    return res;
}

static bool dvmhReadWrite(DvmhObject *obj, UDvmType size, UDvmType nmemb, DvmhFile *stream, RWKind dir, UDvmType *pItemsDone = 0, bool noResult = false) {
    static const char *names[2] = {"fread", "fwrite"};
    checkInternal3(obj->is<DvmhData>(), "Only distributed arrays are allowed to pass to dvmh_%s function", names[dir]);
    DvmhData *data = obj->as<DvmhData>();
    UDvmType typeSize = data->getTypeSize();
    UDvmType elemCount = size * nmemb / typeSize;
    checkError3(size * nmemb == elemCount * typeSize, "Invalid size parameters are passed to %s function. size = " UDTFMT ", nmemb = " UDTFMT
            ", while array type size = " UDTFMT, names[dir], size, nmemb, typeSize);
    checkError3(elemCount <= data->getTotalElemCount(), "Too big size parameters are passed to %s function. size = " UDTFMT ", nmemb = " UDTFMT
            ", while array type size = " UDTFMT " and array element count = " UDTFMT, names[dir], size, nmemb, typeSize, data->getTotalElemCount());
    bool result = dvmhReadWriteGeneral(data, size, Interval::create(0, (DvmType)elemCount - 1), data->getSpace(), stream, dir, pItemsDone, noResult);
    return result; 
}

extern "C" size_t dvmh_fread(void *ptr, size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fread function");
    size_t result = stream->read(ptr, size, nmemb);
    eiof_();  //io stat
    return result;
}

extern "C" size_t dvmh_fread_distrib(DvmType dvmDesc[], size_t size, size_t nmemb, FILE *astream) {
    biof_(); //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fread function");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    UDvmType itemsDone = 0;
    dvmhReadWrite(obj, size, nmemb, stream, rwkRead, &itemsDone);
    eiof_();  //io stat
    return itemsDone;
}

extern "C" void dvmh_void_fread(void *ptr, size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fread function");
    stream->read(ptr, size, nmemb, true);
    eiof_();  //io stat
}

extern "C" void dvmh_void_fread_distrib(DvmType dvmDesc[], size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fread function");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    dvmhReadWrite(obj, size, nmemb, stream, rwkRead, 0, true);
    eiof_();  //io stat
}

extern "C" size_t dvmh_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fwrite function");
    size_t result = stream->write(ptr, size, nmemb);
    eiof_();  //io stat
    return result; 
}

extern "C" size_t dvmh_fwrite_distrib(const DvmType dvmDesc[], size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fwrite function");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    UDvmType itemsDone = 0;
    dvmhReadWrite(obj, size, nmemb, stream, rwkWrite, &itemsDone);
    eiof_();  //io stat
    return itemsDone;
}

extern "C" void dvmh_void_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fwrite function");
    stream->write(ptr, size, nmemb, true);
    eiof_();  //io stat
}

extern "C" void dvmh_void_fwrite_distrib(const DvmType dvmDesc[], size_t size, size_t nmemb, FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fwrite function");
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    dvmhReadWrite(obj, size, nmemb, stream, rwkWrite, 0, true);
    eiof_();  //io stat
}

extern "C" int dvmh_fgetpos(FILE *astream, fpos_t *pos) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fgetpos function");
    int result = stream->getPosition(pos) ? 0 : EOF;
    eiof_(); 
    return result;
}

extern "C" int dvmh_fseek(FILE *astream, long offset, int whence) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fseek function");
    int result = stream->seek(offset, whence) ? 0 : EOF;
    eiof_();
    return result; 
}

extern "C" void dvmh_void_fseek(FILE *astream, long offset, int whence) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fseek function");
    stream->seek(offset, whence, true);
    eiof_();
}

extern "C" int dvmh_fsetpos(FILE *astream, const fpos_t *pos) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the fsetpos function");
    int result = stream->setPosition(pos) ? 0 : EOF;
    eiof_();
    return result;
}

extern "C" long dvmh_ftell(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the ftell function");
    long result = stream->tell();
    eiof_();
    return result; 
}

extern "C" void dvmh_rewind(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the rewind function");
    stream->rewind();
    eiof_();
}

extern "C" void dvmh_clearerr(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the clearerr function");
    stream->clearErrors();
    eiof_();
}

extern "C" int dvmh_feof(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the feof function");
    int result = stream->eof() ? 1 : 0;
    eiof_();
    return result; 
}

extern "C" int dvmh_ferror(FILE *astream) {
    biof_();  //io stat
    DvmhFile *stream = (DvmhFile *)astream;
    checkError2(stream, "NULL stream is passed to the ferror function");
    int result = stream->isInErrorState() ? 1 : 0;
    eiof_();
    return result; 
}

extern "C" void dvmh_perror(const char *s) {
    biof_();  //io stat
    int errNo = errno;
    if (orig_stderr->isLocal() || (isMainThread && !isInParloop)) {
        if (!orig_stderr->isLocal())
            currentMPS->allreduce(errNo, rf_MAX);
        dvmh_fprintf((FILE *)orig_stderr, "%s%s%s\n", (s && *s ? s : ""), (s && *s ? ": " : ""), strerror(errNo));
    } else {
        dvmh_log(NFERROR, "File operations on a global file are permitted only from the sequential part of the program");
        dvmh_log(NFERROR, "perror() is the only exception, which falls back to stderr in this case");
        fprintf(stderr, "%s%s%s\n", (s && *s ? s : ""), (s && *s ? ": " : ""), strerror(errNo));
    }
    eiof_();  //io stat
}

// Interface functions for Fortran-DVMH, which mimic standard Fortran I/O functions

static void fillIOStatMsgInternal(DvmType errorLevel, DvmType *pIOStat, const DvmType *pIOMsgStrRef, const char *msg, bool localFlag) {
    if (pIOStat)
        *pIOStat = errorLevel;
    if (errorLevel != 0 && *pIOMsgStrRef) {
        if (!msg) {
            int errNo = errno;
            if (!localFlag)
                currentMPS->allreduce(errNo, rf_MAX);
            msg = strerror(errNo);
        }
        int len = std::min(getStrSize(pIOMsgStrRef), (UDvmType)strlen(msg));
        memcpy(getStrAddr(pIOMsgStrRef), msg, len);
    }
}

static void fillIOStatMsg(DvmType errorLevel, DvmType *pIOStat, const DvmType *pIOMsgStrRef, const char *msg) {
    fillIOStatMsgInternal(errorLevel, pIOStat, pIOMsgStrRef, msg, true);
}

static void fillIOStatMsg(DvmType errorLevel, DvmType *pIOStat, const DvmType *pIOMsgStrRef, bool localFlag = true) {
    fillIOStatMsgInternal(errorLevel, pIOStat, pIOMsgStrRef, 0, localFlag);
}

extern "C" void dvmh_ftn_open_(const DvmType *pUnit, const DvmType *pAccessStr, const DvmType *pActionStr, const DvmType *pAsyncStr, const DvmType *pBlankStr,
        const DvmType *pDecimalStr, const DvmType *pDelimStr, const DvmType *pEncodingStr, const DvmType *pFileStr, const DvmType *pFormStr,
        const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pNewUnitRef, const DvmType *pPadStr,
        const DvmType *pPositionStr, const DvmType *pRecl, const DvmType *pRoundStr, const DvmType *pSignStr, const DvmType *pStatusStr,
        const DvmType *pDvmModeStr)
{
    checkInternal(pUnit);
    checkInternal(pAccessStr);
    checkInternal(pActionStr);
    checkInternal(pAsyncStr);
    checkInternal(pBlankStr);
    checkInternal(pDecimalStr);
    checkInternal(pDelimStr);
    checkInternal(pEncodingStr);
    checkInternal(pFileStr);
    checkInternal(pFormStr);
    checkInternal(pErrFlagRef);
    checkInternal(pIOStatRef);
    checkInternal(pIOMsgStrRef);
    checkInternal(pNewUnitRef);
    checkInternal(pPadStr);
    checkInternal(pPositionStr);
    checkInternal(pRecl);
    checkInternal(pRoundStr);
    checkInternal(pSignStr);
    checkInternal(pStatusStr);
    checkInternal(pDvmModeStr);
    int unit = *pUnit;
    if (*pNewUnitRef) {
        // Pick a new unit
        // TODO: Make new unit value the same for global files
        for (int i = -2; ; i--) {
            if (fortranFiles.find(i) == fortranFiles.end()) {
                unit = i;
                break;
            }
        }
        *(DvmType *)*pNewUnitRef = unit;
    }
    checkError3(unit != -1, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    const char *access = (*pAccessStr ? getStr(pAccessStr, true, true) : "SEQUENTIAL");
    checkError3(!strcmp(access, "STREAM"), "Unsupported ACCESS value '%s' specified. Only 'STREAM' is supported.", access);
    const char *action = (*pActionStr ? getStr(pActionStr, true, true) : "READWRITE");
    checkError3(!strcmp(action, "READ") || !strcmp(action, "WRITE") || !strcmp(action, "READWRITE"), "Invalid ACTION value '%s' specified", action);
    const char *async = (*pAsyncStr ? getStr(pAsyncStr, true, true) : "NO");
    checkError3(!strcmp(async, "YES") || !strcmp(async, "NO"), "Invalid ASYNCHRONOUS value '%s' specified", async);
    if (*pBlankStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying BLANK has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *blank = (*pBlankStr ? getStr(pBlankStr, true, true) : "NULL");
    checkError3(!strcmp(blank, "ZERO") || !strcmp(blank, "NULL"), "Invalid BLANK value '%s' specified", blank);
    if (*pDecimalStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying DECIMAL has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *decimal = (*pDecimalStr ? getStr(pDecimalStr, true, true) : "POINT");
    checkError3(!strcmp(decimal, "COMMA") || !strcmp(decimal, "POINT"), "Invalid DECIMAL value '%s' specified", decimal);
    if (*pEncodingStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying ENCODING has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *encoding = (*pEncodingStr ? getStr(pEncodingStr, true, true) : "DEFAULT");
    checkError3(!strcmp(encoding, "UTF-8") || !strcmp(encoding, "DEFAULT"), "Invalid ENCODING value '%s' specified", encoding);
    const char *status = (*pStatusStr ? getStr(pStatusStr, true, true) : "UNKNOWN");
    checkError3(!strcmp(status, "OLD") || !strcmp(status, "NEW") || !strcmp(status, "SCRATCH") || !strcmp(status, "REPLACE") || !strcmp(status, "UNKNOWN"),
            "Invalid STATUS value '%s' specified", status);
    const char *fn = getStr(pFileStr, true);
    if (fn)
        checkError2(strcmp(status, "SCRATCH"), "FILE name cannot be specified for a 'SCRATCH' file");
    else
        checkError2(unitConnected || !strcmp(status, "SCRATCH"), "FILE name cannot be omitted for not-previously-connected unit or not a 'SCRATCH' file");
    bool formatted;
    if (*pFormStr) {
        const char *form = getStr(pFormStr, true, true);
        checkError3(!strcmp(form, "FORMATTED") || !strcmp(form, "UNFORMATTED"), "Invalid FORM value '%s' specified", form);
        formatted = form[0] == 'F';
    } else {
        formatted = strcmp(access, "SEQUENTIAL") == 0;
    }
    if (*pPadStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying PAD has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *pad = (*pPadStr ? getStr(pPadStr, true, true) : "YES");
    checkError3(!strcmp(pad, "YES") || !strcmp(pad, "NO"), "Invalid PAD value '%s' specified", pad);
    const char *position = (*pPositionStr ? getStr(pPositionStr, true, true) : "ASIS");
    checkError3(!strcmp(position, "ASIS") || !strcmp(position, "REWIND") || !strcmp(position, "APPEND"), "Invalid POSITION value '%s' specified", position);
    checkError2(*pRecl == 0, "Specifying RECL is unsupported");
    if (*pRoundStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying ROUND has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *round = (*pRoundStr ? getStr(pRoundStr, true, true) : "PROCESSOR_DEFINED");
    checkError3(!strcmp(round, "UP") || !strcmp(round, "DOWN") || !strcmp(round, "ZERO") || !strcmp(round, "NEAREST") || !strcmp(round, "COMPATIBLE") ||
            !strcmp(round, "PROCESSOR_DEFINED"), "Invalid ROUND value '%s' specified", round);
    if (*pSignStr) {
        dvmhLogger.startMasterRegion();
        dvmh_log(WARNING, "Specifying SIGN has no effect");
        dvmhLogger.endMasterRegion();
    }
    const char *sign = (*pSignStr ? getStr(pSignStr, true, true) : "PROCESSOR_DEFINED");
    checkError3(!strcmp(sign, "PLUS") || !strcmp(sign, "SUPPRESS") || !strcmp(sign, "PROCESSOR_DEFINED"), "Invalid SIGN value '%s' specified", sign);
    const char *dvmMode = (*pDvmModeStr ? getStr(pDvmModeStr, true, true) : "");
    bool localFlag = false;
    bool parallelFlag = false;
    bool asyncFlag = false;
    if (!strcmp(async, "YES"))
        asyncFlag = true;
    for (int i = 0; dvmMode[i]; i++) {
        if (dvmMode[i] == 'L')
            localFlag = true;
        else if (dvmMode[i] == 'P')
            parallelFlag = true;
        else if (dvmMode[i] == 'S')
            asyncFlag = true;
        else
            checkInternal3(false, "Unknown DVM file opening mode letter '%c'", dvmMode[i]);
    }
    bool res = true;
    if (unitConnected && fn && fortranFiles[unit]->getFileName() != fn) {
        // Connecting a new file instead of the old one
        res = res && fortranFiles[unit]->close();
        delete fortranFiles[unit];
        fortranFiles.erase(unit);
        unitConnected = false;
    }
    DvmhFile *f = 0;
    if (res && unitConnected) {
        // Changing the mode of a previously connected file
        f = fortranFiles[unit];
        assert(f);
        // Actually, we do not support any changeable modes at all, so only file position can be handled
    } else if (res) {
        if (!strcmp(status, "SCRATCH")) {
            f = DvmhFile::openTemporary(localFlag, parallelFlag, asyncFlag);
        } else {
            assert(fn);
            char mode[16];
            char *modePtr = mode;
            if (!strcmp(action, "READ")) {
                *modePtr++ = 'r';
            } else if (!strcmp(action, "WRITE")) {
                *modePtr++ = 'w';
            } else if (!strcmp(action, "READWRITE")) {
                if (!strcmp(status, "OLD") || !strcmp(status, "UNKNOWN"))
                    *modePtr++ = 'r';
                else
                    *modePtr++ = 'w';
                *modePtr++ = '+';
            }
            if (formatted)
                *modePtr++ = 't';
            else
                *modePtr++ = 'b';
            if (localFlag)
                *modePtr++ = 'l';
            if (parallelFlag)
                *modePtr++ = 'p';
            if (asyncFlag)
                *modePtr++ = 's';
            *modePtr++ = 0;
            f = DvmhFile::openNew(fn, mode);
            if (!f && !strcmp(action, "READWRITE") && !strcmp(status, "UNKNOWN")) {
                mode[0] = 'w';
                f = DvmhFile::openNew(fn, mode);
            }
        }
        if (f)
            fortranFiles[unit] = f;
        res = res && f;
    }
    if (f) {
        if (!strcmp(position, "REWIND"))
            res = res && f->seek(0, SEEK_SET);
        else if (!strcmp(position, "APPEND"))
            res = res && f->seek(0, SEEK_END);
    }
    fillIOStatMsg((res ? 0 : EOF), (DvmType *)*pIOStatRef, pIOMsgStrRef, localFlag);
    if (*pErrFlagRef)
        *(DvmType *)*pErrFlagRef = (res ? 0 : 1);
    checkError2(res || *pIOStatRef || *pErrFlagRef, "Unhandled I/O error occurred during the OPEN operation");
    stringBuffer.clear();
    stringVariables.clear();
}

extern "C" void dvmh_ftn_close_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef,
        const DvmType *pStatusStr) {
    checkInternal(pUnit && pErrFlagRef && pIOStatRef && pIOMsgStrRef && pStatusStr);
    int unit = *pUnit;
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    if (*pStatusStr) {
        const char *status = getStr(pStatusStr, true, true);
        checkError3(!strcmp(status, "KEEP") || !strcmp(status, "DELETE"), "Invalid STATUS value '%s' specified", status);
        if (unitConnected)
            fortranFiles[unit]->setRemoveOnClose(strcmp(status, "DELETE") == 0);
    }
    bool res = true;
    bool localFlag = true;
    if (unitConnected) {
        localFlag = fortranFiles[unit]->isLocal();
        res = res && fortranFiles[unit]->close();
        delete fortranFiles[unit];
        fortranFiles.erase(unit);
    }
    fillIOStatMsg((res ? 0 : EOF), (DvmType *)*pIOStatRef, pIOMsgStrRef, localFlag);
    if (*pErrFlagRef)
        *(DvmType *)*pErrFlagRef = (res ? 0 : 1);
    checkError2(res || *pIOStatRef || *pErrFlagRef, "Unhandled I/O error occurred during the CLOSE operation");
    stringBuffer.clear();
    stringVariables.clear();
}

extern "C" DvmType dvmh_ftn_connected_(const DvmType *pUnit, const DvmType *pFailIfYes) {
    checkInternal(pUnit && pFailIfYes);
    int unit = *pUnit;
    int doFail = *pFailIfYes;
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    checkInternal3(doFail == 0 || doFail == 1, "Invalid failIfYes value passed: %d", doFail);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    bool res = unitConnected;
    if (unitConnected && doFail)
        checkError2(false, "This type of I/O manipulation is not allowed for files opened with DVMH parallel I/O system");
    return res ? 1 : 0;
}

static void dvmhRWUnf(int unit, DvmType *pEndFlag, DvmType *pErrFlag, DvmType *pIOStat, const DvmType *pIOMsgStrRef, DvmType pos, const DvmType dvmDesc[],
        DvmType specifiedRank, va_list &ap, RWKind dir) {
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    if (!unitConnected) {
        if (pEndFlag)
            *pEndFlag = 0;
        if (pErrFlag)
            *pErrFlag = 1;
        fillIOStatMsg(EOF, pIOStat, pIOMsgStrRef, "UNIT is not connected");
        checkError3(pIOStat || pErrFlag, "Unhandled I/O error occurred during the %s operation", (dir == rwkRead ? "READ" : "WRITE"));
        return;
    }
    DvmhFile *f = fortranFiles[unit];
    assert(f);
    if (pos) {
        checkError2(pos > 0, "Position must be positive");
        if (!f->seek(pos - 1, SEEK_SET)) {
            if (pEndFlag)
                *pEndFlag = 0;
            if (pErrFlag)
                *pErrFlag = 1;
            fillIOStatMsg(EOF, pIOStat, pIOMsgStrRef, f->isLocal());
            checkError3(pIOStat || pErrFlag, "Unhandled I/O error occurred during the %s operation", (dir == rwkRead ? "READ" : "WRITE"));
            return;
        }
    }
    DvmhObject *obj = passOrGetOrCreateDvmh(dvmDesc[0], true);
    const char *call[2] = {"dvmh_ftn_read_unf", "dvmh_ftn_write_unf"};
    checkError3(obj, "NULL pointer is passed to %s", call[dir]);
    checkError3(allObjects.find(obj) != allObjects.end(), "Unknown object is passed to %s", call[dir]);
    checkInternal3(obj->isExactly<DvmhData>(), "Only array can be passed to %s", call[dir]);
    DvmhData *data = obj->as<DvmhData>();
    int rank = data->getRank();
#ifdef NON_CONST_AUTOS
    Interval block[rank];
#else
    Interval block[MAX_ARRAY_RANK];
#endif
    if (specifiedRank) {
        checkError3(rank == specifiedRank, "Rank in %s call must be the same as in declaration of the variable", (dir == rwkRead ? "READ" : "WRITE"));
        for (int i = 0; i < rank; i++) {
            DvmType *pV = va_arg(ap, DvmType *);
            checkInternal(pV);
            block[i][0] = *pV;
            pV = va_arg(ap, DvmType *);
            checkInternal(pV);
            block[i][1] = *pV;
        }
        makeBlockReal(rank, data->getSpace(), block);
        checkError2(data->getSpace()->blockContains(rank, block), "Index out of bounds");
    } else {
        block->blockAssign(rank, data->getSpace());
    }
    bool noResult = !pEndFlag && !pErrFlag && !pIOStat && !*pIOMsgStrRef;
    UDvmType size = data->getTypeSize();
    bool res = dvmhReadWriteGeneral(data, size, Interval::create(0, (DvmType)block->blockSize(rank) - 1), block, f, dir, 0, noResult);
    if (!noResult) {
        if (pEndFlag)
            *pEndFlag = res ? 0 : 1;
        if (pErrFlag)
            *pErrFlag = res ? 0 : 1;
        fillIOStatMsg(res ? 0 : EOF, pIOStat, pIOMsgStrRef);
        checkError3(res || pIOStat || pErrFlag, "Unhandled I/O error occurred during the %s operation", (dir == rwkRead ? "READ" : "WRITE"));
    }
}

extern "C" void dvmh_ftn_read_unf_(const DvmType *pUnit, const DvmType *pEndFlagRef, const DvmType *pErrFlagRef, const DvmType *pIOStatRef,
        const DvmType *pIOMsgStrRef, const DvmType *pPos, const DvmType dvmDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    checkInternal(pUnit && pEndFlagRef && pErrFlagRef && pIOStatRef && pIOMsgStrRef && pPos && dvmDesc && pSpecifiedRank);
    va_list ap;
    va_start(ap, pSpecifiedRank);
    dvmhRWUnf(*pUnit, (DvmType *)*pEndFlagRef, (DvmType *)*pErrFlagRef, (DvmType *)*pIOStatRef, pIOMsgStrRef, *pPos, dvmDesc, *pSpecifiedRank, ap, rwkRead);
    va_end(ap);
    stringVariables.clear();
}

extern "C" void dvmh_ftn_write_unf_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef,
        const DvmType *pPos, const DvmType dvmDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) {
    checkInternal(pUnit && pErrFlagRef && pIOStatRef && pIOMsgStrRef && pPos && dvmDesc && pSpecifiedRank);
    va_list ap;
    va_start(ap, pSpecifiedRank);
    dvmhRWUnf(*pUnit, 0, (DvmType *)*pErrFlagRef, (DvmType *)*pIOStatRef, pIOMsgStrRef, *pPos, dvmDesc, *pSpecifiedRank, ap, rwkWrite);
    va_end(ap);
    stringVariables.clear();
}

extern "C" void dvmh_ftn_endfile_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef) {
    checkInternal(pUnit && pErrFlagRef && pIOStatRef && pIOMsgStrRef);
    int unit = *pUnit;
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    bool res = unitConnected;
    bool localFlag = true;
    if (unitConnected) {
        localFlag = fortranFiles[unit]->isLocal();
        res = res && fortranFiles[unit]->truncate();
    }
    fillIOStatMsg((res ? 0 : EOF), (DvmType *)*pIOStatRef, pIOMsgStrRef, localFlag);
    if (*pErrFlagRef)
        *(DvmType *)*pErrFlagRef = (res ? 0 : 1);
    checkError2(res || *pIOStatRef || *pErrFlagRef, "Unhandled I/O error occurred during the ENDFILE operation");
    stringVariables.clear();
}

extern "C" void dvmh_ftn_rewind_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef) {
    checkInternal(pUnit && pErrFlagRef && pIOStatRef && pIOMsgStrRef);
    int unit = *pUnit;
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    bool res = unitConnected;
    bool localFlag = true;
    if (unitConnected) {
        localFlag = fortranFiles[unit]->isLocal();
        res = res && fortranFiles[unit]->rewind();
    }
    fillIOStatMsg((res ? 0 : EOF), (DvmType *)*pIOStatRef, pIOMsgStrRef, localFlag);
    if (*pErrFlagRef)
        *(DvmType *)*pErrFlagRef = (res ? 0 : 1);
    checkError2(res || *pIOStatRef || *pErrFlagRef, "Unhandled I/O error occurred during the REWIND operation");
    stringVariables.clear();
}

extern "C" void dvmh_ftn_flush_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef) {
    checkInternal(pUnit && pErrFlagRef && pIOStatRef && pIOMsgStrRef);
    int unit = *pUnit;
    checkError3(unit != -1 && unit != 0, "Invalid UNIT value %d specified", unit);
    bool unitConnected = fortranFiles.find(unit) != fortranFiles.end();
    bool res = unitConnected;
    if (!unitConnected) {
        fillIOStatMsg(EOF, (DvmType *)*pIOStatRef, pIOMsgStrRef, "UNIT is not connected");
    } else {
        bool localFlag = fortranFiles[unit]->isLocal();
        res = res && fortranFiles[unit]->flush();
        fillIOStatMsg((res ? 0 : EOF), (DvmType *)*pIOStatRef, pIOMsgStrRef, localFlag);
    }
    if (*pErrFlagRef)
        *(DvmType *)*pErrFlagRef = (res ? 0 : 1);
    checkError2(res || *pIOStatRef || *pErrFlagRef, "Unhandled I/O error occurred during the FLUSH operation");
    stringVariables.clear();
}
