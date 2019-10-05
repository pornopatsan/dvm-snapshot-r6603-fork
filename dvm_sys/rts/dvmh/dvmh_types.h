#pragma once

#include <climits>
#include <algorithm>
#include <typeinfo>

#include "include/dvmhlib_types.h"

#if defined(__LLP64__)
typedef unsigned long long UDvmType;
#define DTFMT "%lld"
#define UDTFMT "%llu"
#define DVMTYPE_MIN LLONG_MIN
#define DVMTYPE_MAX LLONG_MAX
#define UDVMTYPE_MAX ULLONG_MAX
#define DVMTYPE_STR "long long"
#else
typedef unsigned long UDvmType;
#define DTFMT "%ld"
#define UDTFMT "%lu"
#define DVMTYPE_MIN LONG_MIN
#define DVMTYPE_MAX LONG_MAX
#define UDVMTYPE_MAX ULONG_MAX
#define DVMTYPE_STR "long"
#endif

namespace libdvmh {

enum LogLevel {INTERR = -1, FATAL = 0, NFERROR = 1, WARNING = 2, INFO = 3, DEBUG = 4, TRACE = 5, LOG_LEVELS, DONT_LOG};
enum SchedTech {stSingleDevice = 0, stSimpleStatic = 1, stSimpleDynamic = 2, stDynamic = 3, stUseScheme = 4, SCHED_TECHS};
enum InitFlag {ifFortran = 1, ifNoH = 2, ifSequential = 4, ifOpenMP = 8, ifDebug = 16};

class Interval {
public:
    DvmType &operator[](int i) { return values[i]; }
    DvmType operator[](int i) const { return values[i]; }
    DvmType &begin() { return values[0]; }
    DvmType begin() const { return values[0]; }
    DvmType &end() { return values[1]; }
    DvmType end() const { return values[1]; }
public:
    static Interval create(DvmType begin, DvmType end) { Interval res; res[0] = begin; res[1] = end; return res; }
    static Interval createEmpty() { return create(0, -1); }
public:
    bool operator==(const Interval &other) const { return values[0] == other.values[0] && values[1] == other.values[1]; }
    bool operator!=(const Interval &other) const { return !(*this == other); }
    bool operator<(const DvmType &val) const { return values[1] < val; }
    bool operator>(const DvmType &val) const { return values[0] > val; }
    Interval operator+(const DvmType &val) const { return create(values[0] + val, values[1] + val); }
    Interval operator-(const DvmType &val) const { return create(values[0] - val, values[1] - val); }
    Interval &operator+=(const DvmType &val) { values[0] += val; values[1] += val; return *this; }
    Interval &operator-=(const DvmType &val) { values[0] -= val; values[1] -= val; return *this; }
    UDvmType size() const { return (empty() ? 0 : (UDvmType)(values[1] - values[0] + 1)); }
    bool empty() const { return values[0] > values[1]; }
    bool contains(DvmType index) const { return index >= values[0] && index <= values[1]; }
    bool contains(const Interval &other) const { return values[0] <= other[0] && values[1] >= other[1]; }
    Interval extend(DvmType toLeft, DvmType toRight) const { return create(values[0] - toLeft, values[1] + toRight); }
    Interval extend(DvmType width) const { return create(values[0] - width, values[1] + width); }
    Interval intersect(const Interval &other) const;
    Interval &intersectInplace(const Interval &other);
    Interval &encloseInplace(const Interval &other);
    UDvmType blockSize(int rank) const;
    bool blockEmpty(int rank) const;
    bool blockEquals(int rank, const Interval other[]) const;
    bool blockContains(int rank, const DvmType indexes[]) const;
    bool blockContains(int rank, const Interval other[]) const;
    bool blockIntersect(int rank, const Interval other[], Interval res[]) const;
    bool blockIntersectInplace(int rank, const Interval other[]);
    bool blockIntersects(int rank, const Interval other[]) const;
    Interval *blockEncloseInplace(int rank, const Interval other[]);
    Interval *blockAssign(int rank, const Interval other[]) { std::copy(other, other + rank, this); return this; }
public: // XXX: Must be public to be POD
    DvmType values[2]; // (Начало,Конец) включительно
};

class LoopBounds {
public:
    DvmType &operator[](int i) { return values[i]; }
    DvmType operator[](int i) const { return values[i]; }
    DvmType &begin() { return values[0]; }
    DvmType begin() const { return values[0]; }
    DvmType &end() { return values[1]; }
    DvmType end() const { return values[1]; }
    DvmType &step() { return values[2]; }
    DvmType step() const { return values[2]; }
public:
    static LoopBounds create(DvmType begin, DvmType end, DvmType step = 1);
    static LoopBounds create(const Interval &interval, DvmType step = 1);
public:
    Interval toInterval() const { return values[2] > 0 ? Interval::create(values[0], values[1]) : Interval::create(values[1], values[0]); }
    void toBlock(int rank, Interval res[]) const;
    bool isCorrect() const { return values[2] != 0; }
    bool empty() const { return (values[2] > 0 && values[0] > values[1]) || (values[2] < 0 && values[0] < values[1]); }
    UDvmType iterCount() const;
    UDvmType iterCount(int rank) const;
public: // XXX: Must be public to be POD
    DvmType values[3]; // (Начало,Конец,Шаг) включительно
};

class ShdWidth {
public:
    DvmType &operator[](int i) { return values[i]; }
    DvmType operator[](int i) const { return values[i]; }
    DvmType &lower() { return values[0]; }
    DvmType lower() const { return values[0]; }
    DvmType &upper() { return values[1]; }
    DvmType upper() const { return values[1]; }
public:
    static ShdWidth create(DvmType lower, DvmType upper) { ShdWidth res; res[0] = lower; res[1] = upper; return res; }
    static ShdWidth createEmpty() { return create(0, 0); }
public:
    UDvmType size() const { return values[0] + values[1]; }
    bool empty() const { return values[0] == 0 && values[1] == 0; }
public: // XXX: Must be public to be POD
    DvmType values[2]; //Lower, Upper
};

#if defined(__NVCC__) || defined(__CUDACC__)
# define DEV_HOST __device__ __host__
#else
# define DEV_HOST
#endif

struct float_complex {
    float re;
    float im;
    DEV_HOST float_complex &operator+=(const float_complex &other) { re += other.re; im += other.im; return *this; }
    DEV_HOST float_complex &operator*=(const float_complex &other) {
        float oldRe = re, oldIm = im;
        re = other.re * oldRe - other.im * oldIm;
        im = other.im * oldRe + oldIm * other.re;
        return *this;
    }
};
struct double_complex {
    double re;
    double im;
    DEV_HOST double_complex &operator+=(const double_complex &other) { re += other.re; im += other.im; return *this; }
    DEV_HOST double_complex &operator*=(const double_complex &other) {
        double oldRe = re, oldIm = im;
        re = other.re * oldRe - other.im * oldIm;
        im = other.im * oldRe + oldIm * other.re;
        return *this;
    }
};
typedef long long long_long;

#if defined(WIN32) || defined(__APPLE__) || defined(__CYGWIN__)
typedef UDvmType cpu_set_t;
#define CPU_SET(i, s) (void)(*(s) |= ((cpu_set_t)1 << (i)))
#define CPU_ISSET(i, s) ((*(s) & ((cpu_set_t)1 << (i))) != (cpu_set_t)0)
#define CPU_ZERO(s) (void)(*(s) = 0)
#define CPU_EQUAL(s1, s2) (*(s1) == *(s2))
#define CPU_OR(dst, s1, s2) (*(dst) = *(s1) | *(s2))
#endif

class Executable;
class BulkTask;

class DvmhObject {
public:
    template <class T>
    T *as() {
        return static_cast<T *>(this);
    }
    template <class T>
    const T *as() const {
        return static_cast<const T *>(this);
    }
    template <class T>
    T *as_s() {
        return dynamic_cast<T *>(this);
    }
    template <class T>
    const T *as_s() const {
        return dynamic_cast<const T *>(this);
    }
    template <class T>
    bool is() const {
        return as_s<T>() != 0;
    }
    template <class T>
    bool isExactly() const {
        return typeid(*this) == typeid(T);
    }
public:
    DvmhObject();
public:
    bool checkSignature() const;
    void addOnDeleteHook(Executable *task);
public:
    virtual ~DvmhObject();
protected:
    char signature[10];
    BulkTask *deleteHook;
};

class DvmhFile;

#ifndef WIN32
#define NON_CONST_AUTOS 1
#define THREAD_LOCAL __thread
#else
#undef NON_CONST_AUTOS
#define THREAD_LOCAL __declspec(thread)
#endif

#ifndef CUDA_INT_INDEX
typedef DvmType CudaIndexType;
#else
typedef int CudaIndexType;
#endif

#ifndef HAVE_CUDA
struct dim3 {
    unsigned x, y, z;
};
typedef void *cudaStream_t;
#endif

#ifndef NON_CONST_AUTOS
#define MAX_DEVICES_COUNT 9
#define MAX_ARRAY_RANK 8
#define MAX_DISTRIB_SPACE_RANK (MAX_ARRAY_RANK + 2)
#define MAX_MPS_RANK MAX_DISTRIB_SPACE_RANK
#define MAX_PIECES_RANK (MAX_DISTRIB_SPACE_RANK + 1)
#define MAX_LOOP_RANK 12
#define MAX_PARAM_COUNT 256
#endif

#ifndef NO_DVM
const bool noLibdvm = false;
#else
const bool noLibdvm = true;
#endif

}
