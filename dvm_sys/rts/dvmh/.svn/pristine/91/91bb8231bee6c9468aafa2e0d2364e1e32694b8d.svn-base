#pragma once

#include <cstdio>
#include <string>

#include "dvmh_async.h"

namespace libdvmh {

class DvmhCommunicator;
class MultiprocessorSystem;

class DvmhFile {
public:
    bool isLocal() const { return localFlag; }
    bool isParallel() const { return parallelFlag; }
    MultiprocessorSystem *getOwningMPS() const { return owningMPS; }
    std::string getFileName() const { return fileName; }
    void setRemoveOnClose(bool val) { if (isIOProc()) removeOnClose = val; }
    DvmhCommunicator *getComm() const { return myComm; }
public:
    static DvmhFile *wrapFILE(FILE *stream, bool local, bool parallel, bool async);
    static DvmhFile *openNew(const char *filename, const char *mode);
    static DvmhFile *openTemporary(bool local, bool parallel, bool async);
public:
    bool openTmp(bool local, bool parallel, bool async);
    bool close();
    bool flush();
    bool open(const char *filename, const char *mode);
    bool changeMode(const char *mode);
    bool setBuffer(char *buf, int mode, size_t size);
    int printFormatted(const char *format, va_list arg, bool noResult = false);
    int scanFormatted(const char *format, va_list arg, bool noResult = false);
    int getChar();
    bool getLine(char *s, int n, bool noResult = false);
    int putChar(int c, bool noResult = false);
    bool putString(const char *s, bool noResult = false);
    bool ungetChar(int c, bool noResult = false);
    UDvmType read(void *ptr, UDvmType size, UDvmType nmemb, bool noResult = false);
    UDvmType write(const void *ptr, UDvmType size, UDvmType nmemb, bool noResult = false);
    bool getPosition(fpos_t *pos) const;
    bool seek(long long offset, int whence, bool noResult = false);
    bool setPosition(const fpos_t *pos);
    long long tell() const;
    bool rewind();
    void clearErrors();
    bool eof() const;
    bool isInErrorState() const;
    bool truncate(bool noResult = false);
public:
    void commitAsyncOp(Executable *op, bool demandsFullyAsync = false);
    void syncOperations() const;
public:
    int seekLocal(long long offset, int whence);
    UDvmType readLocal(void *ptr, UDvmType size, UDvmType nmemb);
    UDvmType writeLocal(void *ptr, UDvmType size, UDvmType nmemb);
    void flushLocalIfAsync();
    void syncParallelBefore(UDvmType reserveBytes = 0);
    void syncParallelAfter(int lastCommRank = -1);
public:
    ~DvmhFile() {
        checkMPS();
        reset();
        delete lastAsyncOpEnd;
    }
protected:
    void setParallel(bool parallel) { parallelFlag = parallel && !isAlone(); }
    bool isAlone() const;
    bool isIOProc() const;
    bool hasStream() const { return isIOProc() || parallelFlag; }
    bool isFullyAsync() const { return asyncFlag && (isAlone() || ownComm); }
protected:
    DvmhFile() { init(); }
protected:
    void checkMPS() const;
    void init();
    void reset();
    void setLocal(bool local);
    void setAsync(bool async);
    void ioBcast(void *buf, UDvmType size) const;
    void ioBcast(char **pBuf, UDvmType *pSize) const;
    template <typename T>
    void ioBcast(T &val) const { ioBcast(&val, sizeof(val)); }
    void wrap(FILE *f, bool local, bool parallel, bool async);
    bool needsUnite() const;
    int uniteIntErr(int err, bool collectiveOp) const;
    template <typename T>
    void checkTheSame(const T &res) const;
    void cleanupOpened();
    int myFseek(long long offset, int whence);
    long long myFtell() const;
    void syncToFS(bool onlyIOProc = false);
    void checkClosed() const;
protected:
    // Global
    bool localFlag;
    bool parallelFlag;
    bool asyncFlag;
    MultiprocessorSystem *owningMPS;
    int fid;
    DvmhCommunicator *myComm;
    bool ownComm;

    // Local
    FILE *stream;
    std::string fileName;
    bool removeOnClose;
    DvmhEvent *lastAsyncOpEnd;
    mutable bool isWaitingNow;
    mutable DvmhSpinLock mut;

    friend class AsyncStringPutter;
    friend class AsyncScanner;
    friend class AsyncLineGetter;
    friend class AsyncCharPutter;
    friend class AsyncCharUngetter;
    friend class AsyncReader;
    friend class AsyncSeqWriter;
    friend class AsyncParWriter;
    friend class AsyncSeeker;
    friend class AsyncRewinder;
    friend class AsyncErrorsClearer;
    friend class AsyncTruncater;
};

void stdioInit();
void stdioFinish();

}
