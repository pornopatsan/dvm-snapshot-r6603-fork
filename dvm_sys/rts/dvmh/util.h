#pragma once

#include <cassert>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>

#include "dvmh_types.h"
#include "dvmh_log.h"

namespace libdvmh {

class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable &);
    Uncopyable &operator=(const Uncopyable &);
};

// Demands objects to be trivially copyable and trivially destructible
template <typename T, size_t N>
class SmallVector {
public:
    typedef T *iterator;
    typedef const T *const_iterator;
public:
    bool empty() const { return valuesCount == 0; }
    size_t size() const { return valuesCount; }
    T *begin() { return values; }
    const T *begin() const { return values; }
    T *end() { return begin() + size(); }
    const T *end() const { return begin() + size(); }
    T &back() { assert(!empty()); return *(begin() + size() - 1); }
    const T &back() const { assert(!empty()); return *(begin() + size() - 1); }
    T &front() { assert(!empty()); return *begin(); }
    const T &front() const { assert(!empty()); return *begin(); }
    T &operator[](int i) { return *(begin() + i); }
    const T &operator[](int i) const { return *(begin() + i); }
public:
    SmallVector(): valuesCount(0) {}
    SmallVector(const SmallVector &other): valuesCount(0) {
        *this = other;
    }
public:
    void resize(size_t n) {
        checkInternal3(n <= N, "Too big size requested: %d but maximum is %d", (int)n, (int)N);
        valuesCount = n;
    }
    void resize(size_t n, const T &nv) {
        checkInternal3(n <= N, "Too big size requested: %d but maximum is %d", (int)n, (int)N);
        if (n > valuesCount)
            std::fill(end(), begin() + n, nv);
        valuesCount = n;
    }
    void push_back(const T &nv) {
        checkInternal2(valuesCount < N, "SmallVector overflow");
        *end() = nv;
        valuesCount++;
    }
    void pop_back() { assert(!empty()); resize(size() - 1); }
    void clear() { resize(0); }
    SmallVector &operator=(const SmallVector &other) {
        valuesCount = other.valuesCount;
        std::copy(other.begin(), other.end(), values);
        return *this;
    }
protected:
    T values[N];
    size_t valuesCount;
};

// Demands objects to be trivially copyable and trivially destructible
template <typename T, size_t N>
class HybridVector {
public:
    typedef T *iterator;
    typedef const T *const_iterator;
public:
    bool empty() const { return (isSmall() ? smallV.empty() : bigV->empty()); }
    size_t size() const { return (isSmall() ? smallV.size() : bigV->size()); }
    T *begin() { return (isSmall() ? smallV.begin() : &*bigV->begin()); }
    const T *begin() const { return (isSmall() ? smallV.begin() : &*bigV->begin()); }
    T *end() { return begin() + size(); }
    const T *end() const { return begin() + size(); }
    T &back() { assert(!empty()); return *(begin() + size() - 1); }
    const T &back() const { assert(!empty()); return *(begin() + size() - 1); }
    T &front() { assert(!empty()); return *begin(); }
    const T &front() const { assert(!empty()); return *begin(); }
    T &operator[](int i) { return *(begin() + i); }
    const T &operator[](int i) const { return *(begin() + i); }
    T *operator~() { return begin(); }
    const T *operator~() const { return begin(); }
public:
    HybridVector(): bigV(0) {}
    HybridVector(const HybridVector &other): bigV(0) {
        *this = other;
    }
public:
    void resize(size_t n) {
        if (isSmall() && n <= N) {
            smallV.resize(n);
        } else {
            makeBig();
            bigV->resize(n);
        }
    }
    void resize(size_t n, const T &nv) {
        if (isSmall() && n <= N) {
            smallV.resize(n, nv);
        } else {
            makeBig();
            bigV->resize(n, nv);
        }
    }
    void push_back(const T &nv) { resize(size() + 1, nv); }
    void pop_back() { assert(!empty()); resize(size() - 1); }
    void clear() { resize(0); }
    HybridVector &operator=(const HybridVector &other) {
        delete bigV;
        if (other.isSmall()) {
            bigV = 0;
            smallV = other.smallV;
        } else if (other.size() <= N) {
            bigV = 0;
            smallV.resize(other.size());
            std::copy(other.begin(), other.end(), smallV.begin());
        } else {
            bigV = new std::vector<T>(*other.bigV);
        }
        return *this;
    }
    HybridVector &moveAssign(HybridVector &other) {
        delete bigV;
        if (other.isSmall()) {
            bigV = 0;
            smallV = other.smallV;
        } else {
            bigV = other.bigV;
            other.bigV = 0;
        }
        return *this;
    }
public:
    ~HybridVector() {
        delete bigV;
    }
protected:
    bool isSmall() const { return bigV == 0; }
    void makeBig() {
        if (isSmall())
            bigV = new std::vector<T>(smallV.begin(), smallV.end());
    }
protected:
    SmallVector<T, N> smallV;
    std::vector<T> *bigV;
};

class BufferWalker {
public:
    UDvmType getRestSize() const { return restSize; }
public:
    explicit BufferWalker(const void *buffer, UDvmType size = UDVMTYPE_MAX) { reset(buffer, size); }
public:
    void reset(const void *buffer, UDvmType size = UDVMTYPE_MAX) {
        buf = (char *)buffer;
        bufPtr = buf;
        restSize = size;
    }
    void reset() {
        restSize += (bufPtr - buf);
        bufPtr = buf;
    }
    void *getDataInplace(UDvmType size) {
        checkInternal(restSize >= size);
        void *res = bufPtr;
        bufPtr += size;
        restSize -= size;
        return res;
    }
    template <typename T>
    T &getValueInplace() { return *(T *)getDataInplace(sizeof(T)); }
    template <typename T>
    T extractValue() { return getValueInplace<T>(); }
    template <typename T>
    void extractValue(T &res) { res = extractValue<T>(); }
    void extractData(void *dst, UDvmType size) { memcpy(dst, getDataInplace(size), size); }
    template <typename T>
    void putValue(const T &v) { getValueInplace<T>() = v; }
    void putData(const void *src, UDvmType size) { memcpy(getDataInplace(size), src, size); }
protected:
    char *buf;
    char *bufPtr;
    UDvmType restSize;
};

template <typename T>
inline typename T::mapped_type dictFind2(const T &mmap, typename T::key_type key, typename T::mapped_type notFoundVal = typename T::mapped_type()) {
    typename T::const_iterator it = mmap.find(key);
    return (it != mmap.end() ? it->second : notFoundVal);
}
template <typename T1, typename T2>
DvmType upperIndex(const T1 *v, UDvmType size, const T2 &val) {
    return std::lower_bound(v, v + size, val) - v;
}
template <typename T1, typename T2>
DvmType lowerIndex(const T1 *v, UDvmType size, const T2 &val) {
    DvmType upper = upperIndex(v, size, val);
    if (upper >= 0 && upper < (DvmType)size && !(v[upper] > val))
        return upper;
    else
        return upper - 1;
}
template <typename T1, typename T2>
DvmType exactIndex(const T1 *v, UDvmType size, const T2 &val) {
    DvmType upper = upperIndex(v, size, val);
    if (upper >= 0 && upper < (DvmType)size) {
        if (!(val < v[upper]))
            return upper;
        else
            return -1 - upper;
    } else
        return upper;
}

double dvmhTime();
void dvmhSleep(double sec);
int getProcessorCount();
std::string getExecutingFileName();
void fillAffinityPermutation(int affinityPerm[], int totalProcessors, int usedProcessors);

int ilog(UDvmType value);
int ilogN(UDvmType value, int n);
int oneBitCount(UDvmType value);
int valueBits(UDvmType value);
UDvmType gcd(UDvmType a, UDvmType b);
UDvmType lcm(UDvmType a, UDvmType b);
UDvmType roundUpU(UDvmType a, UDvmType b);
DvmType roundUpS(DvmType a, DvmType b);
UDvmType roundDownU(UDvmType a, UDvmType b);
DvmType roundDownS(DvmType a, DvmType b);
UDvmType divUpU(UDvmType a, UDvmType b);
DvmType divUpS(DvmType a, DvmType b);
UDvmType divDownU(UDvmType a, UDvmType b);
DvmType divDownS(DvmType a, DvmType b);
template <typename T>
inline int sign(T a) {
    return (a > 0 ? 1 : (a < 0 ? (-1) : 0));
}
unsigned long calcCrc32(const unsigned char *buf, int len);

template <typename T>
T *typedMemcpy(T dest[], const T src[], UDvmType elemCount) {
    std::copy(src, src + elemCount, dest);
    return dest;
}
int executeFunction(DvmHandlerFunc f, void *params[], int paramsCount);

void fillHeader(int rank, UDvmType typeSize, const void *base, const void *devAddr, const int axisPerm[], const Interval portion[], DvmType header[]);
bool fillRealBlock(int rank, const DvmType lowIndex[], const DvmType highIndex[], const Interval havePortion[], Interval realBlock[]);
bool makeBlockReal(int rank, const Interval havePortion[], Interval block[]);
DvmType dvmhXYToDiagonal(DvmType x, DvmType y, DvmType Rx, DvmType Ry, bool slash);
Interval shrinkInterval(const Interval &universal, DvmType step, const Interval &constraint);
bool shrinkBlock(int rank, const Interval universal[], const DvmType steps[], const Interval constraint[], Interval res[]);

template <class T1, class T2>
bool isa(T2 *obj) { return dynamic_cast<T1 *>(obj) != 0; }
template <class T1, class T2>
T1 *asa(T2 *obj) { return static_cast<T1 *>(obj); }

class DvmhTimer {
public:
    explicit DvmhTimer(bool autoStart = false) {
        prevT = startT = 0;
        if (autoStart)
            start();
    }
public:
    void start() {
        startT = dvmhTime();
        prevT = startT;
    }
    double lap() {
        double nextT = dvmhTime();
        double res = nextT - prevT;
        prevT = nextT;
        return res;
    }
    double total() const {
        return dvmhTime() - startT;
    }
    void push() {
        stack.push_back(prevT);
    }
    void pop() {
        prevT = stack.back();
        stack.pop_back();
    }
protected:
    double startT;
    double prevT;
    HybridVector<double, 4> stack;
};

// TODO: Find a place for this
extern bool needToCollectTimes;

const char* getMpiRank();
}
