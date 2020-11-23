#include "util.h"

#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdlib>
#include <ctime>

#if defined(WIN32)
#pragma GCC visibility push(default)
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#pragma GCC visibility pop
#elif defined(__APPLE__)
#include <sys/time.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef HAVE_LIBFFI
#include <ffi.h>
#endif

#include "include/dvmhlib_const.h"

#include "dvmh_log.h"

namespace libdvmh {

double dvmhTime() {
    // XXX: omp_get_wtime() is not used here because it could be safely used only within one thread
#if defined(WIN32)
    LARGE_INTEGER frequency, measuredTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&measuredTime);
    return double(measuredTime.QuadPart) / frequency.QuadPart;
#elif defined(__APPLE__)
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + 1e-6 * tv.tv_usec;
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec + 1e-9 * tp.tv_nsec;
#endif
}

void dvmhSleep(double sec) {
#ifndef WIN32
    struct timespec req, rem;
    req.tv_sec = (time_t)sec;
    req.tv_nsec = (long)((sec - req.tv_sec) * 1e9);
    while (nanosleep(&req, &rem) < 0 && errno == EINTR)
        req = rem;
#else
    Sleep(sec * 1e3);
#endif
}

int getProcessorCount() {
    int res = 1;
#if defined(__APPLE__)
    FILE *f = popen("sysctl -n machdep.cpu.thread_count", "r");
    if (f) {
        fscanf(f, "%d", &res);
        pclose(f);
    } else {
        dvmh_log(DEBUG, "Can not invoke sysctl in order to read processor count");
    }
#elif !defined(WIN32)
    FILE *f = fopen("/proc/cpuinfo", "rt");
    if (f) {
        int count = 0;
        char buf[300];
        while (fgets(buf, sizeof(buf), f)) {
            if (strstr(buf, "processor"))
                count++;
        }
        fclose(f);
        if (count >= 1)
            res = count;
    } else {
        dvmh_log(DEBUG, "Can not read /proc/cpuinfo");
    }
#else
    SYSTEM_INFO sinfo;
    GetSystemInfo(&sinfo);
    res = sinfo.dwNumberOfProcessors;
#endif
    return res;
}

std::string getExecutingFileName() {
    std::string res;
#if defined(WIN32)
    TCHAR buf[FILENAME_MAX + 1];
    DWORD len = GetModuleFileName(0, buf, FILENAME_MAX);
    if (len > 0) {
        buf[len] = (TCHAR)0;
        if (sizeof(TCHAR) == sizeof(char)) {
            res = (char *)buf;
        } else {
            char mbs[FILENAME_MAX + 1];
            wcstombs(mbs, (wchar_t *)buf, sizeof(mbs));
            res = mbs;
        }
    }
#elif defined(__APPLE__)
#else
    char realName[FILENAME_MAX + 1];
    realName[0] = 0;
    ssize_t len = readlink("/proc/self/exe", realName, FILENAME_MAX);
    if (len > 0) {
        realName[len] = 0;
        res = realName;
    }
#endif
    return res;
}

class CPUInfo {
public:
    int logicId;
    int physId;
    int coreId;
    int coreIndex;
    int htIndex;
public:
    bool filled() const { return logicId >= 0 && physId >= 0 && coreId >= 0; }
    void clear() { logicId = physId = coreId = coreIndex = htIndex = -1; }
public:
    CPUInfo() { clear(); }
public:
    bool operator<(const CPUInfo &other) const { return logicOrder(*this, other); }
public:
    static bool physOrder(const CPUInfo &a, const CPUInfo &b);
    static bool logicOrder(const CPUInfo &a, const CPUInfo &b);
    static bool perfOrder(const CPUInfo &a, const CPUInfo &b);
};

bool CPUInfo::physOrder(const CPUInfo &a, const CPUInfo &b) {
    return a.physId < b.physId || (a.physId == b.physId && (a.coreId < b.coreId || (a.coreId == b.coreId && a.logicId < b.logicId)));
}

bool CPUInfo::logicOrder(const CPUInfo &a, const CPUInfo &b) {
    return a.logicId < b.logicId;
}

bool CPUInfo::perfOrder(const CPUInfo &a, const CPUInfo &b) {
    return a.htIndex < b.htIndex || (a.htIndex == b.htIndex && (a.coreIndex < b.coreIndex || (a.coreIndex == b.coreIndex && a.physId < b.physId)));
}

static int readCpuInfoNumber(const char *line) {
    const char *where = strchr(line, ':');
    if (where)
        return atoi(where + 1);
    return -1;
}

void fillAffinityPermutation(int affinityPerm[], int totalProcessors, int usedProcessors) {
    for (int i = 0; i < totalProcessors; i++)
        affinityPerm[i] = i;
    if (usedProcessors == totalProcessors || usedProcessors == 0)
        return;
    assert(usedProcessors < totalProcessors);
    std::vector<CPUInfo> procs;
    procs.reserve(totalProcessors);
#ifndef WIN32
    FILE *f = fopen("/proc/cpuinfo", "rt");
    if (f) {
        char buf[300];
        CPUInfo curInfo;
        while (fgets(buf, sizeof(buf), f)) {
            if (strstr(buf, "processor")) {
                if (!procs.empty() && !curInfo.filled()) {
                    dvmh_log(DEBUG, "Unexpected 'processor' line in /proc/cpuinfo: '%s'", buf);
                    procs.clear();
                    break;
                }
                if (curInfo.filled())
                    procs.push_back(curInfo);
                curInfo.clear();
                curInfo.logicId = readCpuInfoNumber(buf);
                if (curInfo.logicId < 0 || curInfo.logicId >= totalProcessors || curInfo.logicId != (int)procs.size()) {
                    dvmh_log(DEBUG, "Unexpected processor logical id in /proc/cpuinfo: '%s'", buf);
                    procs.clear();
                    break;
                }
            } else if (strstr(buf, "physical id")) {
                curInfo.physId = readCpuInfoNumber(buf);
                if (curInfo.physId < 0) {
                    dvmh_log(DEBUG, "Unexpected processor physical id in /proc/cpuinfo: '%s'", buf);
                    procs.clear();
                    break;
                }
            } else if (strstr(buf, "core id")) {
                curInfo.coreId = readCpuInfoNumber(buf);
                if (curInfo.coreId < 0) {
                    dvmh_log(DEBUG, "Unexpected processor core id in /proc/cpuinfo: '%s'", buf);
                    procs.clear();
                    break;
                }
            }
        }
        if (curInfo.filled())
            procs.push_back(curInfo);
        fclose(f);
        if ((int)procs.size() != totalProcessors) {
            dvmh_log(DEBUG, "Unexpected 'processor' line count (%d vs %d) in /proc/cpuinfo", (int)procs.size(), totalProcessors);
            procs.clear();
        }
    }
#endif
    if (procs.empty()) {
        for (int i = 0; i < totalProcessors; i++) {
            CPUInfo dummy;
            dummy.logicId = i;
            dummy.physId = i;
            dummy.coreId = 0;
            procs.push_back(dummy);
        }
    }
    assert((int)procs.size() == totalProcessors);

    std::sort(procs.begin(), procs.end(), CPUInfo::physOrder);
    procs[0].coreIndex = 0;
    procs[0].htIndex = 0;
    for (int i = 1; i < totalProcessors; i++) {
        if (procs[i].physId != procs[i - 1].physId)
            procs[i].coreIndex = 0;
        else if (procs[i].coreId != procs[i - 1].coreId)
            procs[i].coreIndex = procs[i - 1].coreIndex + 1;
        else
            procs[i].coreIndex = procs[i - 1].coreIndex;
        if (procs[i].physId == procs[i - 1].physId && procs[i].coreId == procs[i - 1].coreId)
            procs[i].htIndex = procs[i - 1].htIndex + 1;
        else
            procs[i].htIndex = 0;
    }


    std::vector<int> cpuToOrder(totalProcessors, -1);
    std::sort(procs.begin(), procs.end(), CPUInfo::logicOrder);
    // Blacklisting
    int blacklisted = 0;
    if (totalProcessors >= 240) {
        // XXX: Workaround for Intel MIC
        blacklisted++;
        cpuToOrder[0] = totalProcessors - blacklisted;
        for (int i = 1; i < totalProcessors; i++) {
            if (procs[i].physId == procs[0].physId && procs[i].coreId == procs[0].coreId) {
                blacklisted++;
                cpuToOrder[i] = totalProcessors - blacklisted;
            }
        }
    }

    std::sort(procs.begin(), procs.end(), CPUInfo::perfOrder);
    int curOrder = 0;
    for (int i = 0; i < totalProcessors; i++) {
        int id = procs[i].logicId;
        if (cpuToOrder[id] < 0)
            cpuToOrder[id] = curOrder++;
    }
    assert(curOrder == totalProcessors - blacklisted);

    std::sort(procs.begin(), procs.end(), CPUInfo::physOrder);
    curOrder = 0;
    for (int i = 0; i < totalProcessors; i++)
        if (cpuToOrder[procs[i].logicId] < usedProcessors)
            affinityPerm[curOrder++] = procs[i].logicId;
    for (int i = 0; i < totalProcessors; i++)
        if (cpuToOrder[i] >= usedProcessors)
            affinityPerm[cpuToOrder[i]] = i;
}

int ilog(UDvmType value) {
    int res = -1;
    if (value && !(value & (value - 1U)))
        for (res = 0; ((value >> res) & 1) == 0; res++) ;
    return res;
}

int ilogN(UDvmType value, int n) {
    int res = -1;
    while (n > 1 && value) {
        value = value & (value - 1U);
        n--;
    }
    if (n == 1 && value)
        res = ilog(value - (value & (value - 1U)));
    return res;
}

int oneBitCount(UDvmType value) {
    int res = 0;
    while (value) {
        value = value & (value - 1U);
        res++;
    }
    return res;
}

int valueBits(UDvmType value) {
    int res = 0;
    while (value) {
        res++;
        value >>= 1;
    }
    return res;
}

UDvmType gcd(UDvmType a, UDvmType b) {
    while (b != 0) {
        UDvmType b_old = b;
        b = a % b_old;
        a = b_old;
    }
    return a;
}

UDvmType lcm(UDvmType a, UDvmType b) {
    return a * (b / gcd(a, b));
}

UDvmType roundUpU(UDvmType a, UDvmType b) {
    return divUpU(a, b) * b;
}

DvmType roundUpS(DvmType a, DvmType b) {
    b = std::abs(b);
    return divUpS(a, b) * b;
}

UDvmType roundDownU(UDvmType a, UDvmType b) {
    return divDownU(a, b) * b;
}

DvmType roundDownS(DvmType a, DvmType b) {
    b = std::abs(b);
    return divDownS(a, b) * b;
}

UDvmType divUpU(UDvmType a, UDvmType b) {
    assert(b != 0);
    return (a + b - 1) / b;
}

DvmType divUpS(DvmType a, DvmType b) {
    assert(b != 0);
    if (b < 0) {
        b = -b;
        a = -a;
    }
    return (a >= 0 ? (a + b - 1) / b : a / b);
}

UDvmType divDownU(UDvmType a, UDvmType b) {
    assert(b != 0);
    return a / b;
}

DvmType divDownS(DvmType a, DvmType b) {
    assert(b != 0);
    if (b < 0) {
        b = -b;
        a = -a;
    }
    return (a >= 0 ? a / b : (a - (b - 1)) / b);
}

class CrcTable {
public:
    unsigned long operator[](int i) const { return crcTable[i];}
public:
    CrcTable() {
        assert(CHAR_BIT == 8);
        for (int i = 0; i < 256; i++) {
            unsigned long crc = i;
            for (int j = 0; j < 8; j++)
                crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
            crcTable[i] = crc;
        }
    }
protected:
    unsigned long crcTable[256];
};

unsigned long calcCrc32(const unsigned char *buf, int len) {
    static CrcTable crcTable;

    unsigned long crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crcTable[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

#define ADDPARAMS(n) ADD ##n## PARAMS(0)
#define ADD0PARAMS(base)
#define ADD1PARAMS(base) params[base]
#define ADD2PARAMS(base) ADD1PARAMS(base), ADD1PARAMS(base + 1)
#define ADD3PARAMS(base) ADD2PARAMS(base), ADD1PARAMS(base + 2)
#define ADD4PARAMS(base) ADD2PARAMS(base), ADD2PARAMS(base + 2)
#define ADD5PARAMS(base) ADD4PARAMS(base), ADD1PARAMS(base + 4)
#define ADD6PARAMS(base) ADD4PARAMS(base), ADD2PARAMS(base + 4)
#define ADD7PARAMS(base) ADD4PARAMS(base), ADD3PARAMS(base + 4)
#define ADD8PARAMS(base) ADD4PARAMS(base), ADD4PARAMS(base + 4)
#define ADD9PARAMS(base) ADD8PARAMS(base), ADD1PARAMS(base + 8)
#define ADD10PARAMS(base) ADD8PARAMS(base), ADD2PARAMS(base + 8)
#define ADD11PARAMS(base) ADD8PARAMS(base), ADD3PARAMS(base + 8)
#define ADD12PARAMS(base) ADD8PARAMS(base), ADD4PARAMS(base + 8)
#define ADD13PARAMS(base) ADD8PARAMS(base), ADD5PARAMS(base + 8)
#define ADD14PARAMS(base) ADD8PARAMS(base), ADD6PARAMS(base + 8)
#define ADD15PARAMS(base) ADD8PARAMS(base), ADD7PARAMS(base + 8)
#define ADD16PARAMS(base) ADD8PARAMS(base), ADD8PARAMS(base + 8)
#define ADD17PARAMS(base) ADD16PARAMS(base), ADD1PARAMS(base + 16)
#define ADD18PARAMS(base) ADD16PARAMS(base), ADD2PARAMS(base + 16)
#define ADD19PARAMS(base) ADD16PARAMS(base), ADD3PARAMS(base + 16)
#define ADD20PARAMS(base) ADD16PARAMS(base), ADD4PARAMS(base + 16)
#define ADD21PARAMS(base) ADD16PARAMS(base), ADD5PARAMS(base + 16)
#define ADD22PARAMS(base) ADD16PARAMS(base), ADD6PARAMS(base + 16)
#define ADD23PARAMS(base) ADD16PARAMS(base), ADD7PARAMS(base + 16)
#define ADD24PARAMS(base) ADD16PARAMS(base), ADD8PARAMS(base + 16)
#define ADD25PARAMS(base) ADD16PARAMS(base), ADD9PARAMS(base + 16)
#define ADD26PARAMS(base) ADD16PARAMS(base), ADD10PARAMS(base + 16)
#define ADD27PARAMS(base) ADD16PARAMS(base), ADD11PARAMS(base + 16)
#define ADD28PARAMS(base) ADD16PARAMS(base), ADD12PARAMS(base + 16)
#define ADD29PARAMS(base) ADD16PARAMS(base), ADD13PARAMS(base + 16)
#define ADD30PARAMS(base) ADD16PARAMS(base), ADD14PARAMS(base + 16)
#define ADD31PARAMS(base) ADD16PARAMS(base), ADD15PARAMS(base + 16)
#define ADD32PARAMS(base) ADD16PARAMS(base), ADD16PARAMS(base + 16)
#define ADD33PARAMS(base) ADD32PARAMS(base), ADD1PARAMS(base + 32)
#define ADD34PARAMS(base) ADD32PARAMS(base), ADD2PARAMS(base + 32)
#define ADD35PARAMS(base) ADD32PARAMS(base), ADD3PARAMS(base + 32)
#define ADD36PARAMS(base) ADD32PARAMS(base), ADD4PARAMS(base + 32)
#define ADD37PARAMS(base) ADD32PARAMS(base), ADD5PARAMS(base + 32)
#define ADD38PARAMS(base) ADD32PARAMS(base), ADD6PARAMS(base + 32)
#define ADD39PARAMS(base) ADD32PARAMS(base), ADD7PARAMS(base + 32)
#define ADD40PARAMS(base) ADD32PARAMS(base), ADD8PARAMS(base + 32)
#define ADD41PARAMS(base) ADD32PARAMS(base), ADD9PARAMS(base + 32)
#define ADD42PARAMS(base) ADD32PARAMS(base), ADD10PARAMS(base + 32)
#define ADD43PARAMS(base) ADD32PARAMS(base), ADD11PARAMS(base + 32)
#define ADD44PARAMS(base) ADD32PARAMS(base), ADD12PARAMS(base + 32)
#define ADD45PARAMS(base) ADD32PARAMS(base), ADD13PARAMS(base + 32)
#define ADD46PARAMS(base) ADD32PARAMS(base), ADD14PARAMS(base + 32)
#define ADD47PARAMS(base) ADD32PARAMS(base), ADD15PARAMS(base + 32)
#define ADD48PARAMS(base) ADD32PARAMS(base), ADD16PARAMS(base + 32)
#define ADD49PARAMS(base) ADD32PARAMS(base), ADD17PARAMS(base + 32)
#define ADD50PARAMS(base) ADD32PARAMS(base), ADD18PARAMS(base + 32)
#define ADD51PARAMS(base) ADD32PARAMS(base), ADD19PARAMS(base + 32)
#define ADD52PARAMS(base) ADD32PARAMS(base), ADD20PARAMS(base + 32)
#define ADD53PARAMS(base) ADD32PARAMS(base), ADD21PARAMS(base + 32)
#define ADD54PARAMS(base) ADD32PARAMS(base), ADD22PARAMS(base + 32)
#define ADD55PARAMS(base) ADD32PARAMS(base), ADD23PARAMS(base + 32)
#define ADD56PARAMS(base) ADD32PARAMS(base), ADD24PARAMS(base + 32)
#define ADD57PARAMS(base) ADD32PARAMS(base), ADD25PARAMS(base + 32)
#define ADD58PARAMS(base) ADD32PARAMS(base), ADD26PARAMS(base + 32)
#define ADD59PARAMS(base) ADD32PARAMS(base), ADD27PARAMS(base + 32)
#define ADD60PARAMS(base) ADD32PARAMS(base), ADD28PARAMS(base + 32)
#define ADD61PARAMS(base) ADD32PARAMS(base), ADD29PARAMS(base + 32)
#define ADD62PARAMS(base) ADD32PARAMS(base), ADD30PARAMS(base + 32)
#define ADD63PARAMS(base) ADD32PARAMS(base), ADD31PARAMS(base + 32)
#define ADD64PARAMS(base) ADD32PARAMS(base), ADD32PARAMS(base + 32)
#define ADD65PARAMS(base) ADD64PARAMS(base), ADD1PARAMS(base + 64)
#define ADD66PARAMS(base) ADD64PARAMS(base), ADD2PARAMS(base + 64)
#define ADD67PARAMS(base) ADD64PARAMS(base), ADD3PARAMS(base + 64)
#define ADD68PARAMS(base) ADD64PARAMS(base), ADD4PARAMS(base + 64)
#define ADD69PARAMS(base) ADD64PARAMS(base), ADD5PARAMS(base + 64)
#define ADD70PARAMS(base) ADD64PARAMS(base), ADD6PARAMS(base + 64)
#define ADD71PARAMS(base) ADD64PARAMS(base), ADD7PARAMS(base + 64)
#define ADD72PARAMS(base) ADD64PARAMS(base), ADD8PARAMS(base + 64)
#define ADD73PARAMS(base) ADD64PARAMS(base), ADD9PARAMS(base + 64)
#define ADD74PARAMS(base) ADD64PARAMS(base), ADD10PARAMS(base + 64)
#define ADD75PARAMS(base) ADD64PARAMS(base), ADD11PARAMS(base + 64)
#define ADD76PARAMS(base) ADD64PARAMS(base), ADD12PARAMS(base + 64)
#define ADD77PARAMS(base) ADD64PARAMS(base), ADD13PARAMS(base + 64)
#define ADD78PARAMS(base) ADD64PARAMS(base), ADD14PARAMS(base + 64)
#define ADD79PARAMS(base) ADD64PARAMS(base), ADD15PARAMS(base + 64)
#define ADD80PARAMS(base) ADD64PARAMS(base), ADD16PARAMS(base + 64)
#define ADD81PARAMS(base) ADD64PARAMS(base), ADD17PARAMS(base + 64)
#define ADD82PARAMS(base) ADD64PARAMS(base), ADD18PARAMS(base + 64)
#define ADD83PARAMS(base) ADD64PARAMS(base), ADD19PARAMS(base + 64)
#define ADD84PARAMS(base) ADD64PARAMS(base), ADD20PARAMS(base + 64)
#define ADD85PARAMS(base) ADD64PARAMS(base), ADD21PARAMS(base + 64)
#define ADD86PARAMS(base) ADD64PARAMS(base), ADD22PARAMS(base + 64)
#define ADD87PARAMS(base) ADD64PARAMS(base), ADD23PARAMS(base + 64)
#define ADD88PARAMS(base) ADD64PARAMS(base), ADD24PARAMS(base + 64)
#define ADD89PARAMS(base) ADD64PARAMS(base), ADD25PARAMS(base + 64)
#define ADD90PARAMS(base) ADD64PARAMS(base), ADD26PARAMS(base + 64)
#define ADD91PARAMS(base) ADD64PARAMS(base), ADD27PARAMS(base + 64)
#define ADD92PARAMS(base) ADD64PARAMS(base), ADD28PARAMS(base + 64)
#define ADD93PARAMS(base) ADD64PARAMS(base), ADD29PARAMS(base + 64)
#define ADD94PARAMS(base) ADD64PARAMS(base), ADD30PARAMS(base + 64)
#define ADD95PARAMS(base) ADD64PARAMS(base), ADD31PARAMS(base + 64)
#define ADD96PARAMS(base) ADD64PARAMS(base), ADD32PARAMS(base + 64)
#define ADD97PARAMS(base) ADD64PARAMS(base), ADD33PARAMS(base + 64)
#define ADD98PARAMS(base) ADD64PARAMS(base), ADD34PARAMS(base + 64)
#define ADD99PARAMS(base) ADD64PARAMS(base), ADD35PARAMS(base + 64)
#define ADD100PARAMS(base) ADD64PARAMS(base), ADD36PARAMS(base + 64)
#define ADD101PARAMS(base) ADD64PARAMS(base), ADD37PARAMS(base + 64)
#define ADD102PARAMS(base) ADD64PARAMS(base), ADD38PARAMS(base + 64)
#define ADD103PARAMS(base) ADD64PARAMS(base), ADD39PARAMS(base + 64)
#define ADD104PARAMS(base) ADD64PARAMS(base), ADD40PARAMS(base + 64)
#define ADD105PARAMS(base) ADD64PARAMS(base), ADD41PARAMS(base + 64)
#define ADD106PARAMS(base) ADD64PARAMS(base), ADD42PARAMS(base + 64)
#define ADD107PARAMS(base) ADD64PARAMS(base), ADD43PARAMS(base + 64)
#define ADD108PARAMS(base) ADD64PARAMS(base), ADD44PARAMS(base + 64)
#define ADD109PARAMS(base) ADD64PARAMS(base), ADD45PARAMS(base + 64)
#define ADD110PARAMS(base) ADD64PARAMS(base), ADD46PARAMS(base + 64)
#define ADD111PARAMS(base) ADD64PARAMS(base), ADD47PARAMS(base + 64)
#define ADD112PARAMS(base) ADD64PARAMS(base), ADD48PARAMS(base + 64)
#define ADD113PARAMS(base) ADD64PARAMS(base), ADD49PARAMS(base + 64)
#define ADD114PARAMS(base) ADD64PARAMS(base), ADD50PARAMS(base + 64)
#define ADD115PARAMS(base) ADD64PARAMS(base), ADD51PARAMS(base + 64)
#define ADD116PARAMS(base) ADD64PARAMS(base), ADD52PARAMS(base + 64)
#define ADD117PARAMS(base) ADD64PARAMS(base), ADD53PARAMS(base + 64)
#define ADD118PARAMS(base) ADD64PARAMS(base), ADD54PARAMS(base + 64)
#define ADD119PARAMS(base) ADD64PARAMS(base), ADD55PARAMS(base + 64)
#define ADD120PARAMS(base) ADD64PARAMS(base), ADD56PARAMS(base + 64)
#define ADD121PARAMS(base) ADD64PARAMS(base), ADD57PARAMS(base + 64)
#define ADD122PARAMS(base) ADD64PARAMS(base), ADD58PARAMS(base + 64)
#define ADD123PARAMS(base) ADD64PARAMS(base), ADD59PARAMS(base + 64)
#define ADD124PARAMS(base) ADD64PARAMS(base), ADD60PARAMS(base + 64)
#define ADD125PARAMS(base) ADD64PARAMS(base), ADD61PARAMS(base + 64)
#define ADD126PARAMS(base) ADD64PARAMS(base), ADD62PARAMS(base + 64)
#define ADD127PARAMS(base) ADD64PARAMS(base), ADD63PARAMS(base + 64)
#define ADD128PARAMS(base) ADD64PARAMS(base), ADD64PARAMS(base + 64)
#define ADD129PARAMS(base) ADD128PARAMS(base), ADD1PARAMS(base + 128)
#define ADD130PARAMS(base) ADD128PARAMS(base), ADD2PARAMS(base + 128)
#define ADD131PARAMS(base) ADD128PARAMS(base), ADD3PARAMS(base + 128)
#define ADD132PARAMS(base) ADD128PARAMS(base), ADD4PARAMS(base + 128)
#define ADD133PARAMS(base) ADD128PARAMS(base), ADD5PARAMS(base + 128)
#define ADD134PARAMS(base) ADD128PARAMS(base), ADD6PARAMS(base + 128)
#define ADD135PARAMS(base) ADD128PARAMS(base), ADD7PARAMS(base + 128)
#define ADD136PARAMS(base) ADD128PARAMS(base), ADD8PARAMS(base + 128)
#define ADD137PARAMS(base) ADD128PARAMS(base), ADD9PARAMS(base + 128)
#define ADD138PARAMS(base) ADD128PARAMS(base), ADD10PARAMS(base + 128)
#define ADD139PARAMS(base) ADD128PARAMS(base), ADD11PARAMS(base + 128)
#define ADD140PARAMS(base) ADD128PARAMS(base), ADD12PARAMS(base + 128)
#define ADD141PARAMS(base) ADD128PARAMS(base), ADD13PARAMS(base + 128)
#define ADD142PARAMS(base) ADD128PARAMS(base), ADD14PARAMS(base + 128)
#define ADD143PARAMS(base) ADD128PARAMS(base), ADD15PARAMS(base + 128)
#define ADD144PARAMS(base) ADD128PARAMS(base), ADD16PARAMS(base + 128)
#define ADD145PARAMS(base) ADD128PARAMS(base), ADD17PARAMS(base + 128)
#define ADD146PARAMS(base) ADD128PARAMS(base), ADD18PARAMS(base + 128)
#define ADD147PARAMS(base) ADD128PARAMS(base), ADD19PARAMS(base + 128)
#define ADD148PARAMS(base) ADD128PARAMS(base), ADD20PARAMS(base + 128)
#define ADD149PARAMS(base) ADD128PARAMS(base), ADD21PARAMS(base + 128)
#define ADD150PARAMS(base) ADD128PARAMS(base), ADD22PARAMS(base + 128)
#define ADD151PARAMS(base) ADD128PARAMS(base), ADD23PARAMS(base + 128)
#define ADD152PARAMS(base) ADD128PARAMS(base), ADD24PARAMS(base + 128)
#define ADD153PARAMS(base) ADD128PARAMS(base), ADD25PARAMS(base + 128)
#define ADD154PARAMS(base) ADD128PARAMS(base), ADD26PARAMS(base + 128)
#define ADD155PARAMS(base) ADD128PARAMS(base), ADD27PARAMS(base + 128)
#define ADD156PARAMS(base) ADD128PARAMS(base), ADD28PARAMS(base + 128)
#define ADD157PARAMS(base) ADD128PARAMS(base), ADD29PARAMS(base + 128)
#define ADD158PARAMS(base) ADD128PARAMS(base), ADD30PARAMS(base + 128)
#define ADD159PARAMS(base) ADD128PARAMS(base), ADD31PARAMS(base + 128)
#define ADD160PARAMS(base) ADD128PARAMS(base), ADD32PARAMS(base + 128)
#define ADD161PARAMS(base) ADD128PARAMS(base), ADD33PARAMS(base + 128)
#define ADD162PARAMS(base) ADD128PARAMS(base), ADD34PARAMS(base + 128)
#define ADD163PARAMS(base) ADD128PARAMS(base), ADD35PARAMS(base + 128)
#define ADD164PARAMS(base) ADD128PARAMS(base), ADD36PARAMS(base + 128)
#define ADD165PARAMS(base) ADD128PARAMS(base), ADD37PARAMS(base + 128)
#define ADD166PARAMS(base) ADD128PARAMS(base), ADD38PARAMS(base + 128)
#define ADD167PARAMS(base) ADD128PARAMS(base), ADD39PARAMS(base + 128)
#define ADD168PARAMS(base) ADD128PARAMS(base), ADD40PARAMS(base + 128)
#define ADD169PARAMS(base) ADD128PARAMS(base), ADD41PARAMS(base + 128)
#define ADD170PARAMS(base) ADD128PARAMS(base), ADD42PARAMS(base + 128)
#define ADD171PARAMS(base) ADD128PARAMS(base), ADD43PARAMS(base + 128)
#define ADD172PARAMS(base) ADD128PARAMS(base), ADD44PARAMS(base + 128)
#define ADD173PARAMS(base) ADD128PARAMS(base), ADD45PARAMS(base + 128)
#define ADD174PARAMS(base) ADD128PARAMS(base), ADD46PARAMS(base + 128)
#define ADD175PARAMS(base) ADD128PARAMS(base), ADD47PARAMS(base + 128)
#define ADD176PARAMS(base) ADD128PARAMS(base), ADD48PARAMS(base + 128)
#define ADD177PARAMS(base) ADD128PARAMS(base), ADD49PARAMS(base + 128)
#define ADD178PARAMS(base) ADD128PARAMS(base), ADD50PARAMS(base + 128)
#define ADD179PARAMS(base) ADD128PARAMS(base), ADD51PARAMS(base + 128)
#define ADD180PARAMS(base) ADD128PARAMS(base), ADD52PARAMS(base + 128)
#define ADD181PARAMS(base) ADD128PARAMS(base), ADD53PARAMS(base + 128)
#define ADD182PARAMS(base) ADD128PARAMS(base), ADD54PARAMS(base + 128)
#define ADD183PARAMS(base) ADD128PARAMS(base), ADD55PARAMS(base + 128)
#define ADD184PARAMS(base) ADD128PARAMS(base), ADD56PARAMS(base + 128)
#define ADD185PARAMS(base) ADD128PARAMS(base), ADD57PARAMS(base + 128)
#define ADD186PARAMS(base) ADD128PARAMS(base), ADD58PARAMS(base + 128)
#define ADD187PARAMS(base) ADD128PARAMS(base), ADD59PARAMS(base + 128)
#define ADD188PARAMS(base) ADD128PARAMS(base), ADD60PARAMS(base + 128)
#define ADD189PARAMS(base) ADD128PARAMS(base), ADD61PARAMS(base + 128)
#define ADD190PARAMS(base) ADD128PARAMS(base), ADD62PARAMS(base + 128)
#define ADD191PARAMS(base) ADD128PARAMS(base), ADD63PARAMS(base + 128)
#define ADD192PARAMS(base) ADD128PARAMS(base), ADD64PARAMS(base + 128)
#define ADD193PARAMS(base) ADD128PARAMS(base), ADD65PARAMS(base + 128)
#define ADD194PARAMS(base) ADD128PARAMS(base), ADD66PARAMS(base + 128)
#define ADD195PARAMS(base) ADD128PARAMS(base), ADD67PARAMS(base + 128)
#define ADD196PARAMS(base) ADD128PARAMS(base), ADD68PARAMS(base + 128)
#define ADD197PARAMS(base) ADD128PARAMS(base), ADD69PARAMS(base + 128)
#define ADD198PARAMS(base) ADD128PARAMS(base), ADD70PARAMS(base + 128)
#define ADD199PARAMS(base) ADD128PARAMS(base), ADD71PARAMS(base + 128)
#define ADD200PARAMS(base) ADD128PARAMS(base), ADD72PARAMS(base + 128)
#define ADD201PARAMS(base) ADD128PARAMS(base), ADD73PARAMS(base + 128)
#define ADD202PARAMS(base) ADD128PARAMS(base), ADD74PARAMS(base + 128)
#define ADD203PARAMS(base) ADD128PARAMS(base), ADD75PARAMS(base + 128)
#define ADD204PARAMS(base) ADD128PARAMS(base), ADD76PARAMS(base + 128)
#define ADD205PARAMS(base) ADD128PARAMS(base), ADD77PARAMS(base + 128)
#define ADD206PARAMS(base) ADD128PARAMS(base), ADD78PARAMS(base + 128)
#define ADD207PARAMS(base) ADD128PARAMS(base), ADD79PARAMS(base + 128)
#define ADD208PARAMS(base) ADD128PARAMS(base), ADD80PARAMS(base + 128)
#define ADD209PARAMS(base) ADD128PARAMS(base), ADD81PARAMS(base + 128)
#define ADD210PARAMS(base) ADD128PARAMS(base), ADD82PARAMS(base + 128)
#define ADD211PARAMS(base) ADD128PARAMS(base), ADD83PARAMS(base + 128)
#define ADD212PARAMS(base) ADD128PARAMS(base), ADD84PARAMS(base + 128)
#define ADD213PARAMS(base) ADD128PARAMS(base), ADD85PARAMS(base + 128)
#define ADD214PARAMS(base) ADD128PARAMS(base), ADD86PARAMS(base + 128)
#define ADD215PARAMS(base) ADD128PARAMS(base), ADD87PARAMS(base + 128)
#define ADD216PARAMS(base) ADD128PARAMS(base), ADD88PARAMS(base + 128)
#define ADD217PARAMS(base) ADD128PARAMS(base), ADD89PARAMS(base + 128)
#define ADD218PARAMS(base) ADD128PARAMS(base), ADD90PARAMS(base + 128)
#define ADD219PARAMS(base) ADD128PARAMS(base), ADD91PARAMS(base + 128)
#define ADD220PARAMS(base) ADD128PARAMS(base), ADD92PARAMS(base + 128)
#define ADD221PARAMS(base) ADD128PARAMS(base), ADD93PARAMS(base + 128)
#define ADD222PARAMS(base) ADD128PARAMS(base), ADD94PARAMS(base + 128)
#define ADD223PARAMS(base) ADD128PARAMS(base), ADD95PARAMS(base + 128)
#define ADD224PARAMS(base) ADD128PARAMS(base), ADD96PARAMS(base + 128)
#define ADD225PARAMS(base) ADD128PARAMS(base), ADD97PARAMS(base + 128)
#define ADD226PARAMS(base) ADD128PARAMS(base), ADD98PARAMS(base + 128)
#define ADD227PARAMS(base) ADD128PARAMS(base), ADD99PARAMS(base + 128)
#define ADD228PARAMS(base) ADD128PARAMS(base), ADD100PARAMS(base + 128)
#define ADD229PARAMS(base) ADD128PARAMS(base), ADD101PARAMS(base + 128)
#define ADD230PARAMS(base) ADD128PARAMS(base), ADD102PARAMS(base + 128)
#define ADD231PARAMS(base) ADD128PARAMS(base), ADD103PARAMS(base + 128)
#define ADD232PARAMS(base) ADD128PARAMS(base), ADD104PARAMS(base + 128)
#define ADD233PARAMS(base) ADD128PARAMS(base), ADD105PARAMS(base + 128)
#define ADD234PARAMS(base) ADD128PARAMS(base), ADD106PARAMS(base + 128)
#define ADD235PARAMS(base) ADD128PARAMS(base), ADD107PARAMS(base + 128)
#define ADD236PARAMS(base) ADD128PARAMS(base), ADD108PARAMS(base + 128)
#define ADD237PARAMS(base) ADD128PARAMS(base), ADD109PARAMS(base + 128)
#define ADD238PARAMS(base) ADD128PARAMS(base), ADD110PARAMS(base + 128)
#define ADD239PARAMS(base) ADD128PARAMS(base), ADD111PARAMS(base + 128)
#define ADD240PARAMS(base) ADD128PARAMS(base), ADD112PARAMS(base + 128)
#define ADD241PARAMS(base) ADD128PARAMS(base), ADD113PARAMS(base + 128)
#define ADD242PARAMS(base) ADD128PARAMS(base), ADD114PARAMS(base + 128)
#define ADD243PARAMS(base) ADD128PARAMS(base), ADD115PARAMS(base + 128)
#define ADD244PARAMS(base) ADD128PARAMS(base), ADD116PARAMS(base + 128)
#define ADD245PARAMS(base) ADD128PARAMS(base), ADD117PARAMS(base + 128)
#define ADD246PARAMS(base) ADD128PARAMS(base), ADD118PARAMS(base + 128)
#define ADD247PARAMS(base) ADD128PARAMS(base), ADD119PARAMS(base + 128)
#define ADD248PARAMS(base) ADD128PARAMS(base), ADD120PARAMS(base + 128)
#define ADD249PARAMS(base) ADD128PARAMS(base), ADD121PARAMS(base + 128)
#define ADD250PARAMS(base) ADD128PARAMS(base), ADD122PARAMS(base + 128)
#define ADD251PARAMS(base) ADD128PARAMS(base), ADD123PARAMS(base + 128)
#define ADD252PARAMS(base) ADD128PARAMS(base), ADD124PARAMS(base + 128)
#define ADD253PARAMS(base) ADD128PARAMS(base), ADD125PARAMS(base + 128)
#define ADD254PARAMS(base) ADD128PARAMS(base), ADD126PARAMS(base + 128)
#define ADD255PARAMS(base) ADD128PARAMS(base), ADD127PARAMS(base + 128)
#define ADD256PARAMS(base) ADD128PARAMS(base), ADD128PARAMS(base + 128)

#define GEN_CASE(n) \
case n: \
    res = f(ADDPARAMS(n)); \
    break;

#ifdef HAVE_LIBFFI
static ffi_type *argTypes[4096];

struct ArgTypesInitializer {
    explicit ArgTypesInitializer() {
        for (int i = 0; i < (int)(sizeof(argTypes) / sizeof(argTypes[0])); i++)
            argTypes[i] = &ffi_type_pointer;
    }
};

static ArgTypesInitializer argTypesInitializer;
#endif

int executeFunction(DvmHandlerFunc f, void *params[], int paramsCount) {
    int res = 0;
    if (!dvmhSettings.preferCallWithSwitch || paramsCount > 256) {
#ifdef HAVE_LIBFFI
        ffi_cif cif;
        if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, paramsCount, &ffi_type_sint, argTypes) == FFI_OK) {
#ifdef NON_CONST_AUTOS
            void *argPointers[paramsCount];
#else
            void *argPointers[MAX_PARAM_COUNT];
#endif
            for (int i = 0; i < paramsCount; i++)
                argPointers[i] = params + i;
            ffi_sarg rawRes;
            ffi_call(&cif, (void (*)(void))f, &rawRes, argPointers);
            res = rawRes;
            dvmh_log(TRACE, "Call performed through libFFI");
            return res;
        } else {
            dvmh_log(DEBUG, "Error preparing the call through libFFI");
        }
#endif
    }
    switch (paramsCount) {
        GEN_CASE(0)
        GEN_CASE(1)
        GEN_CASE(2)
        GEN_CASE(3)
        GEN_CASE(4)
        GEN_CASE(5)
        GEN_CASE(6)
        GEN_CASE(7)
        GEN_CASE(8)
        GEN_CASE(9)
        GEN_CASE(10)
        GEN_CASE(11)
        GEN_CASE(12)
        GEN_CASE(13)
        GEN_CASE(14)
        GEN_CASE(15)
        GEN_CASE(16)
        GEN_CASE(17)
        GEN_CASE(18)
        GEN_CASE(19)
        GEN_CASE(20)
        GEN_CASE(21)
        GEN_CASE(22)
        GEN_CASE(23)
        GEN_CASE(24)
        GEN_CASE(25)
        GEN_CASE(26)
        GEN_CASE(27)
        GEN_CASE(28)
        GEN_CASE(29)
        GEN_CASE(30)
        GEN_CASE(31)
        GEN_CASE(32)
        GEN_CASE(33)
        GEN_CASE(34)
        GEN_CASE(35)
        GEN_CASE(36)
        GEN_CASE(37)
        GEN_CASE(38)
        GEN_CASE(39)
        GEN_CASE(40)
        GEN_CASE(41)
        GEN_CASE(42)
        GEN_CASE(43)
        GEN_CASE(44)
        GEN_CASE(45)
        GEN_CASE(46)
        GEN_CASE(47)
        GEN_CASE(48)
        GEN_CASE(49)
        GEN_CASE(50)
        GEN_CASE(51)
        GEN_CASE(52)
        GEN_CASE(53)
        GEN_CASE(54)
        GEN_CASE(55)
        GEN_CASE(56)
        GEN_CASE(57)
        GEN_CASE(58)
        GEN_CASE(59)
        GEN_CASE(60)
        GEN_CASE(61)
        GEN_CASE(62)
        GEN_CASE(63)
        GEN_CASE(64)
        GEN_CASE(65)
        GEN_CASE(66)
        GEN_CASE(67)
        GEN_CASE(68)
        GEN_CASE(69)
        GEN_CASE(70)
        GEN_CASE(71)
        GEN_CASE(72)
        GEN_CASE(73)
        GEN_CASE(74)
        GEN_CASE(75)
        GEN_CASE(76)
        GEN_CASE(77)
        GEN_CASE(78)
        GEN_CASE(79)
        GEN_CASE(80)
        GEN_CASE(81)
        GEN_CASE(82)
        GEN_CASE(83)
        GEN_CASE(84)
        GEN_CASE(85)
        GEN_CASE(86)
        GEN_CASE(87)
        GEN_CASE(88)
        GEN_CASE(89)
        GEN_CASE(90)
        GEN_CASE(91)
        GEN_CASE(92)
        GEN_CASE(93)
        GEN_CASE(94)
        GEN_CASE(95)
        GEN_CASE(96)
        GEN_CASE(97)
        GEN_CASE(98)
        GEN_CASE(99)
        GEN_CASE(100)
        GEN_CASE(101)
        GEN_CASE(102)
        GEN_CASE(103)
        GEN_CASE(104)
        GEN_CASE(105)
        GEN_CASE(106)
        GEN_CASE(107)
        GEN_CASE(108)
        GEN_CASE(109)
        GEN_CASE(110)
        GEN_CASE(111)
        GEN_CASE(112)
        GEN_CASE(113)
        GEN_CASE(114)
        GEN_CASE(115)
        GEN_CASE(116)
        GEN_CASE(117)
        GEN_CASE(118)
        GEN_CASE(119)
        GEN_CASE(120)
        GEN_CASE(121)
        GEN_CASE(122)
        GEN_CASE(123)
        GEN_CASE(124)
        GEN_CASE(125)
        GEN_CASE(126)
        GEN_CASE(127)
        GEN_CASE(128)
        GEN_CASE(129)
        GEN_CASE(130)
        GEN_CASE(131)
        GEN_CASE(132)
        GEN_CASE(133)
        GEN_CASE(134)
        GEN_CASE(135)
        GEN_CASE(136)
        GEN_CASE(137)
        GEN_CASE(138)
        GEN_CASE(139)
        GEN_CASE(140)
        GEN_CASE(141)
        GEN_CASE(142)
        GEN_CASE(143)
        GEN_CASE(144)
        GEN_CASE(145)
        GEN_CASE(146)
        GEN_CASE(147)
        GEN_CASE(148)
        GEN_CASE(149)
        GEN_CASE(150)
        GEN_CASE(151)
        GEN_CASE(152)
        GEN_CASE(153)
        GEN_CASE(154)
        GEN_CASE(155)
        GEN_CASE(156)
        GEN_CASE(157)
        GEN_CASE(158)
        GEN_CASE(159)
        GEN_CASE(160)
        GEN_CASE(161)
        GEN_CASE(162)
        GEN_CASE(163)
        GEN_CASE(164)
        GEN_CASE(165)
        GEN_CASE(166)
        GEN_CASE(167)
        GEN_CASE(168)
        GEN_CASE(169)
        GEN_CASE(170)
        GEN_CASE(171)
        GEN_CASE(172)
        GEN_CASE(173)
        GEN_CASE(174)
        GEN_CASE(175)
        GEN_CASE(176)
        GEN_CASE(177)
        GEN_CASE(178)
        GEN_CASE(179)
        GEN_CASE(180)
        GEN_CASE(181)
        GEN_CASE(182)
        GEN_CASE(183)
        GEN_CASE(184)
        GEN_CASE(185)
        GEN_CASE(186)
        GEN_CASE(187)
        GEN_CASE(188)
        GEN_CASE(189)
        GEN_CASE(190)
        GEN_CASE(191)
        GEN_CASE(192)
        GEN_CASE(193)
        GEN_CASE(194)
        GEN_CASE(195)
        GEN_CASE(196)
        GEN_CASE(197)
        GEN_CASE(198)
        GEN_CASE(199)
        GEN_CASE(200)
        GEN_CASE(201)
        GEN_CASE(202)
        GEN_CASE(203)
        GEN_CASE(204)
        GEN_CASE(205)
        GEN_CASE(206)
        GEN_CASE(207)
        GEN_CASE(208)
        GEN_CASE(209)
        GEN_CASE(210)
        GEN_CASE(211)
        GEN_CASE(212)
        GEN_CASE(213)
        GEN_CASE(214)
        GEN_CASE(215)
        GEN_CASE(216)
        GEN_CASE(217)
        GEN_CASE(218)
        GEN_CASE(219)
        GEN_CASE(220)
        GEN_CASE(221)
        GEN_CASE(222)
        GEN_CASE(223)
        GEN_CASE(224)
        GEN_CASE(225)
        GEN_CASE(226)
        GEN_CASE(227)
        GEN_CASE(228)
        GEN_CASE(229)
        GEN_CASE(230)
        GEN_CASE(231)
        GEN_CASE(232)
        GEN_CASE(233)
        GEN_CASE(234)
        GEN_CASE(235)
        GEN_CASE(236)
        GEN_CASE(237)
        GEN_CASE(238)
        GEN_CASE(239)
        GEN_CASE(240)
        GEN_CASE(241)
        GEN_CASE(242)
        GEN_CASE(243)
        GEN_CASE(244)
        GEN_CASE(245)
        GEN_CASE(246)
        GEN_CASE(247)
        GEN_CASE(248)
        GEN_CASE(249)
        GEN_CASE(250)
        GEN_CASE(251)
        GEN_CASE(252)
        GEN_CASE(253)
        GEN_CASE(254)
        GEN_CASE(255)
        GEN_CASE(256)

        default:
            checkInternal3(0, "Function execution with %d arguments is not implemented yet", paramsCount);
            break;
    }
    dvmh_log(TRACE, "Call performed with the help of switch");
    return res;
}

void fillHeader(int rank, UDvmType typeSize, const void *base, const void *devAddr, const int axisPerm[], const Interval portion[], DvmType header[]) {
    DvmType collector = 1;
    DvmType offset = 0;
    header[rank + 2] = (DvmType)base;
    checkInternal2(((DvmType)devAddr - header[rank + 2]) % typeSize == 0, "Impossible to calculate an offset of the array from the provided base");
    for (int i = rank; i >= 1; i--) {
        int origAxis = (axisPerm ? axisPerm[i - 1] : i);
        offset += collector * (portion ? portion[origAxis - 1][0] : 0);
        header[(origAxis - 1) + 1] = collector;
        collector *= (portion ? portion[origAxis - 1].size() : 1);
    }
    header[rank + 1] = ((DvmType)devAddr - header[rank + 2]) / typeSize - offset;
}

bool fillRealBlock(int rank, const DvmType lowIndex[], const DvmType highIndex[], const Interval havePortion[], Interval realBlock[]) {
    bool res = true;
    for (int i = 0; i < rank; i++) {
        if (!lowIndex || lowIndex[i] == UNDEF_BOUND)
            realBlock[i][0] = havePortion[i][0];
        else
            realBlock[i][0] = lowIndex[i];
        if (!highIndex || highIndex[i] == UNDEF_BOUND)
            realBlock[i][1] = havePortion[i][1];
        else
            realBlock[i][1] = highIndex[i];
        res = res && !realBlock[i].empty();
    }
    return res;
}

bool makeBlockReal(int rank, const Interval havePortion[], Interval block[]) {
    bool res = true;
    for (int i = 0; i < rank; i++) {
        if (block[i][0] == UNDEF_BOUND)
            block[i][0] = havePortion[i][0];
        if (block[i][1] == UNDEF_BOUND)
            block[i][1] = havePortion[i][1];
        res = res && !block[i].empty();
    }
    return res;
}

DvmType dvmhXYToDiagonal(DvmType x, DvmType y, DvmType Rx, DvmType Ry, bool slash) {
    DvmType idx;
    if (!slash) {
        if (Rx == Ry) {
            if (x + y < Rx)
                idx = y + (1 + x + y) * (x + y) / 2;
            else
                idx = Rx * (Rx - 1) + x - (2 * Rx - x - y - 1) * (2 * Rx - x - y - 2) / 2;
        } else if (Rx < Ry) {
            if (x + y < Rx)
                idx = y + ((1 + x + y) * (x + y)) / 2;
            else if (x + y < Ry)
                idx = ((1 + Rx) * Rx) / 2 + Rx - x - 1 + Rx * (x + y - Rx);
            else
                idx = Rx * Ry - Ry + y - (((Rx + Ry - y - x - 1) * (Rx + Ry - y - x - 2)) / 2);
        } else {
            if (x + y < Ry)
                idx = x + (1 + x + y) * (x + y) / 2;
            else if (x + y < Rx)
                idx = (1 + Ry) * Ry / 2 + (Ry - y - 1) + Ry * (x + y - Ry);
            else
                idx = Rx * Ry - Rx + x - ((Rx + Ry - y - x - 1) * (Rx + Ry - y - x - 2) / 2);
        }
    } else {
        if (Rx == Ry) {
            if(x + Rx - 1 - y < Rx)
                idx = Rx - 1 - y + (x + Rx - y) * (x + Rx - 1 - y) / 2;
            else
                idx = Rx * (Rx - 1) + x - (Rx - x + y) * (Rx - x + y - 1) / 2;
        } else if (Rx < Ry) {
            if (x + Ry - 1 - y < Rx)
                idx = Ry - 1 - y + ((x + Ry - y) * (x + Ry - 1 - y)) / 2;
            else if (x + Ry - 1 - y < Ry)
                idx = ((1 + Rx) * Rx) / 2 + Rx - x - 1 + Rx * (x + Ry - 1 - y - Rx);
            else
                idx = Rx * Ry - 1 - y - (((Rx + y - x) * (Rx + y - x - 1)) / 2);
        } else {
            if (x + Ry - 1 - y < Ry)
                idx = x + (1 + x + Ry - 1 - y) * (x + Ry - 1 - y) / 2;
            else if (x + Ry - 1 - y < Rx)
                idx = (1 + Ry) * Ry / 2 + y + Ry * (x - y - 1);
            else
                idx = Rx * Ry - Rx + x - ((Rx + y - x) * (Rx + y - x - 1) / 2);
        }
    }
    return idx;
}

Interval shrinkInterval(const Interval &universal, DvmType step, const Interval &constraint) {
    Interval res;
    res[0] = universal[0] + roundUpS(std::max(universal[0], constraint[0]) - universal[0], step);
    res[1] = universal[1] - roundUpS(universal[1] - std::min(universal[1], constraint[1]), step);
    return res;
}

bool shrinkBlock(int rank, const Interval universal[], const DvmType steps[], const Interval constraint[], Interval res[]) {
    bool notEmpty = true;
    for (int i = 0; i < rank; i++) {
        res[i] = shrinkInterval(universal[i], steps[i], constraint[i]);
        notEmpty = notEmpty && !res[i].empty();
    }
    return notEmpty;
}

// TODO: Find a place for this
bool needToCollectTimes = true;

//TODO: need to add other compilers
const char* getMpiRank() {
    const char * envR = getenv("PMI_RANK"); /* OMPI_COMM_WORLD_LOCAL_RANK for OpenMPI? */
    return envR;
}

}
