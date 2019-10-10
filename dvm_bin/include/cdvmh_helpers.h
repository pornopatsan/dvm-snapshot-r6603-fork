#ifndef CDVMH_HELPERS_H
#define CDVMH_HELPERS_H

#include <dvmhlib2.h>

#ifdef __cplusplus

template <typename T>
struct DataTypeToRT {
    const static DvmType value = sizeof(T);
};

#define DEF_TYPE(T, cons) \
template <> \
struct DataTypeToRT<T> { \
    const static DvmType value = -cons; \
};
DEF_TYPE(char, rt_CHAR)
DEF_TYPE(int, rt_INT)
DEF_TYPE(long, rt_LONG)
DEF_TYPE(float, rt_FLOAT)
DEF_TYPE(double, rt_DOUBLE)
#if __STDC_VERSION__ >= 199900L
DEF_TYPE(float _Complex, rt_FLOAT_COMPLEX)
DEF_TYPE(double _Complex, rt_DOUBLE_COMPLEX)
#endif
DEF_TYPE(long long, rt_LLONG)
#undef DEF_TYPE

template <int N>
class DvmhHeader {
public:
    DvmType &operator[](int i) { return header[i]; }
    operator DvmType *() { return header; }
protected:
    DvmType header[N];
};

template <int line, const char *file, bool isDynamic, int rank, typename T, DvmType s1, DvmType l1, DvmType h1,
        DvmType s2 = 0, DvmType l2 = 0, DvmType h2 = 0,
        DvmType s3 = 0, DvmType l3 = 0, DvmType h3 = 0,
        DvmType s4 = 0, DvmType l4 = 0, DvmType h4 = 0,
        DvmType s5 = 0, DvmType l5 = 0, DvmType h5 = 0,
        DvmType s6 = 0, DvmType l6 = 0, DvmType h6 = 0,
        DvmType s7 = 0, DvmType l7 = 0, DvmType h7 = 0>
class DvmhArray: public DvmhHeader<64> {
public:
    DvmhArray() {
        dvmh_line_C(line, file);
        if (isDynamic)
            dvmh_array_declare_C(*this, rank, DataTypeToRT<T>::value, s1, l1, h1, s2, l2, h2, s3, l3, h3, s4, l4, h4, s5, l5, h5, s6, l6, h6, s7, l7, h7);
        else
            dvmh_array_create_C(*this, rank, DataTypeToRT<T>::value, s1, l1, h1, s2, l2, h2, s3, l3, h3, s4, l4, h4, s5, l5, h5, s6, l6, h6, s7, l7, h7);
    }
    ~DvmhArray() {
        dvmh_line_C(line, file);
        if (isDynamic)
            dvmh_forget_header_(*this);
        else
            dvmh_delete_object_(*this);
    }
};

template <int line, const char *file, int rank, DvmType s1,
        DvmType s2 = 0,
        DvmType s3 = 0,
        DvmType s4 = 0,
        DvmType s5 = 0,
        DvmType s6 = 0,
        DvmType s7 = 0>
class DvmhTemplate: public DvmhHeader<1> {
public:
    DvmhTemplate() {
        dvmh_line_C(line, file);
        dvmh_template_create_C(*this, rank, s1, s2, s3, s4, s5, s6, s7);
    }
    ~DvmhTemplate() {
        dvmh_delete_object_(*this);
    }
};

// new, new[], delete and delete[]

#ifndef __CUDA_ARCH__

#include <cstddef>
#include <stdexcept>
#include <new>

class DvmhDummyAllocator {};
#ifdef DVMH_NEEDS_ALLOCATOR
static DvmhDummyAllocator dvmhDummyAllocator;
#endif

inline void *operator new(size_t size, DvmhDummyAllocator &, const char FN[], int line) throw(std::bad_alloc) {
    // XXX: dvmh_line is not thread-safe
    // dvmh_line_C(line, FN);
    void *res = dvmh_malloc_C(size);
    if (!res)
        throw std::bad_alloc();
    return res;
}

inline void *operator new[](size_t size, DvmhDummyAllocator &, const char FN[], int line) throw(std::bad_alloc) {
    // XXX: dvmh_line is not thread-safe
    // dvmh_line_C(line, FN);
    void *res = dvmh_malloc_C(size);
    if (!res)
        throw std::bad_alloc();
    return res;
}

template <typename T>
inline void dvmh_delete_one(T *obj, const char FN[], int line) throw() {
    if (obj) {
        // XXX: dvmh_line is not thread-safe
        // dvmh_line_C(line, FN);
        dvmh_free_C((void *)obj);
    }
}

template <typename T>
inline void dvmh_delete_array(T objs[], const char FN[], int line) throw() {
    if (objs) {
        // XXX: dvmh_line is not thread-safe
        // dvmh_line_C(line, FN);
        dvmh_free_C((void *)objs);
    }
}

#endif

// restrict

#if defined(__CUDACC__)

#define DVMH_RESTRICT __restrict__
#define DVMH_RESTRICT_REF

#elif defined(__GNUG__)

#define DVMH_RESTRICT __restrict__
#define DVMH_RESTRICT_REF __restrict__

#elif defined(_MSC_VER)

#define DVMH_RESTRICT __restrict
#define DVMH_RESTRICT_REF __restrict

#else

#define DVMH_RESTRICT
#define DVMH_RESTRICT_REF

#endif

#else

/* C99 standard provides 'restrict' keyword */

#define DVMH_RESTRICT restrict

#endif

#ifdef __CUDACC__

// Indexing coefficients

template <int n, typename IndexT>
class DvmhArrayCoefficients;

template <typename IndexT>
class DvmhArrayCoefficients<0, IndexT> {};

template <typename IndexT>
class DvmhArrayCoefficients<1, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<0, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1): c1(aC1) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<0, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<2, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<1, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2): c1(aC1), rest(aC2) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<1, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<3, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<2, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2, const IndexT &aC3): c1(aC1), rest(aC2, aC3) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<2, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<4, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<3, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2, const IndexT &aC3, const IndexT &aC4): c1(aC1), rest(aC2, aC3, aC4) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<3, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<5, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<4, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2, const IndexT &aC3, const IndexT &aC4, const IndexT &aC5): c1(aC1),
            rest(aC2, aC3, aC4, aC5) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<4, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<6, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<5, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2, const IndexT &aC3, const IndexT &aC4, const IndexT &aC5, const IndexT &aC6):
            c1(aC1), rest(aC2, aC3, aC4, aC5, aC6) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<5, IndexT> &cdr() const { return rest; }
};

template <typename IndexT>
class DvmhArrayCoefficients<7, IndexT> {
    const IndexT &c1;
    DvmhArrayCoefficients<6, IndexT> rest;
public:
    __device__ DvmhArrayCoefficients(const IndexT &aC1, const IndexT &aC2, const IndexT &aC3, const IndexT &aC4, const IndexT &aC5, const IndexT &aC6,
            const IndexT &aC7): c1(aC1), rest(aC2, aC3, aC4, aC5, aC6, aC7) {}
    __device__ const IndexT &car() const { return c1; }
    __device__ const DvmhArrayCoefficients<6, IndexT> &cdr() const { return rest; }
};

// Diagonalized indexing

template <typename IndexT>
__inline__ __device__ IndexT dvmhConvertXY(const IndexT &x, const IndexT &y, const IndexT &Rx, const IndexT &Ry, const int &tfmType) {
    IndexT idx;
    if (!(tfmType & 4)) {
        if (Rx == Ry) {
            if (x + y < Rx)
                idx = y + (1 + x + y) * (x + y) / 2;
            else
                idx = Rx * (Rx - 1) + x - (2 * Rx - x - y - 1) * (2 * Rx - x - y - 2) / 2;
        }
        else if (Rx < Ry) {
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
            if (x + Rx - 1 - y < Rx)
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

template <typename IndexT>
class DvmhDiagInfo {
public:
    const int &tfmType, &xAxis, &yAxis;
    const IndexT &Rx, &Ry, &xOffset, &yOffset;
public:
    __device__ DvmhDiagInfo(const int &aTfmType, const int &aXAxis, const int &aYAxis, const IndexT &aRx, const IndexT &aRy, const IndexT &aXOffset,
            const IndexT &aYOffset): tfmType(aTfmType), xAxis(aXAxis), yAxis(aYAxis), Rx(aRx), Ry(aRy), xOffset(aXOffset), yOffset(aYOffset) {}
    __device__ IndexT getToAdd(const IndexT &xIdx, const IndexT &yIdx) const {
        if (tfmType & 2)
            return dvmhConvertXY(xIdx - xOffset, yIdx - yOffset, Rx, Ry, tfmType);
        else
            return 0;
    }
};

// These helpers support non-transformed arrays

template <int n, typename ElemT, typename PtrT = ElemT *, typename IndexT = DvmType>
class DvmhArrayHelper {
    const PtrT &data;
    const DvmhArrayCoefficients<n - 1, IndexT> &coefs;
public:
    __device__ DvmhArrayHelper(const PtrT &aData, const DvmhArrayCoefficients<n - 1, IndexT> &aCoefs): data(aData), coefs(aCoefs) {}
    __device__ DvmhArrayHelper<n - 1, ElemT, PtrT, IndexT> operator[](const IndexT &i1) const {
        return DvmhArrayHelper<n - 1, ElemT, PtrT, IndexT>(data + i1 * coefs.car(), coefs.cdr());
    }
};

template <typename ElemT, typename PtrT, typename IndexT>
class DvmhArrayHelper<1, ElemT, PtrT, IndexT> {
    const static int n = 1;
    const PtrT &data;
    const DvmhArrayCoefficients<n - 1, IndexT> &coefs;
public:
    __device__ DvmhArrayHelper(const PtrT &aData, const DvmhArrayCoefficients<n - 1, IndexT> &aCoefs): data(aData), coefs(aCoefs) {}
    __device__ ElemT &operator[](const IndexT &i1) const {
        return data[i1];
    }
};

// These helpers support transformed arrays (non-diagonalized)

template <int n, typename ElemT, typename PtrT = ElemT *, typename IndexT = DvmType>
class DvmhPermutatedArrayHelper {
    const PtrT &data;
    const DvmhArrayCoefficients<n, IndexT> &coefs;
public:
    __device__ DvmhPermutatedArrayHelper(const PtrT &aData, const DvmhArrayCoefficients<n, IndexT> &aCoefs): data(aData), coefs(aCoefs) {}
    __device__ DvmhPermutatedArrayHelper<n - 1, ElemT, PtrT, IndexT> operator[](const IndexT &i1) const {
        return DvmhPermutatedArrayHelper<n - 1, ElemT, PtrT, IndexT>(data + i1 * coefs.car(), coefs.cdr());
    }
};

template <typename ElemT, typename PtrT, typename IndexT>
class DvmhPermutatedArrayHelper<1, ElemT, PtrT, IndexT> {
    const static int n = 1;
    const PtrT &data;
    const DvmhArrayCoefficients<n, IndexT> &coefs;
public:
    __device__ DvmhPermutatedArrayHelper(const PtrT &aData, const DvmhArrayCoefficients<n, IndexT> &aCoefs): data(aData), coefs(aCoefs) {}
    __device__ ElemT &operator[](const IndexT &i1) const {
        return data[i1 * coefs.car()];
    }
};

// These helpers support diagonalized arrays

template <int n, int i, typename ElemT, typename PtrT, typename IndexT>
class DvmhDiagIndexer {
    const PtrT &data;
    const DvmhArrayCoefficients<i, IndexT> &coefs;
    const DvmhDiagInfo<IndexT> &diagInfo;
    IndexT &xIdx, &yIdx;
public:
    __device__ DvmhDiagIndexer(const PtrT &aData, const DvmhArrayCoefficients<i, IndexT> &aCoefs, const DvmhDiagInfo<IndexT> &aDiagInfo, IndexT &x, IndexT &y):
            data(aData), coefs(aCoefs), diagInfo(aDiagInfo), xIdx(x), yIdx(y) {}
    __device__ DvmhDiagIndexer<n, i - 1, ElemT, PtrT, IndexT> operator[](const IndexT &i1) {
        if (n - i + 1 == diagInfo.xAxis)
            xIdx = i1;
        if (n - i + 1 == diagInfo.yAxis)
            yIdx = i1;
        return DvmhDiagIndexer<n, i - 1, ElemT, PtrT, IndexT>(data + i1 * coefs.car(), coefs.cdr(), diagInfo, xIdx, yIdx);
    }
};

template <int n, typename ElemT, typename PtrT, typename IndexT>
class DvmhDiagIndexer<n, 1, ElemT, PtrT, IndexT> {
    const static int i = 1;
    const PtrT &data;
    const DvmhArrayCoefficients<i, IndexT> &coefs;
    const DvmhDiagInfo<IndexT> &diagInfo;
    IndexT &xIdx, &yIdx;
public:
    __device__ DvmhDiagIndexer(const PtrT &aData, const DvmhArrayCoefficients<i, IndexT> &aCoefs, const DvmhDiagInfo<IndexT> &aDiagInfo, IndexT &x, IndexT &y):
            data(aData), coefs(aCoefs), diagInfo(aDiagInfo), xIdx(x), yIdx(y) {}
    __device__ ElemT &operator[](const IndexT &i1) {
        if (n - i + 1 == diagInfo.xAxis)
            xIdx = i1;
        if (n - i + 1 == diagInfo.yAxis)
            yIdx = i1;
        return data[i1 * coefs.car() + diagInfo.getToAdd(xIdx, yIdx)];
    }
};

template <int n, typename ElemT, typename PtrT = ElemT *, typename IndexT = DvmType>
class DvmhDiagonalizedArrayHelper: public DvmhDiagIndexer<n, n, ElemT, PtrT, IndexT> {
    IndexT xIdx, yIdx;
public:
    __device__ DvmhDiagonalizedArrayHelper(const PtrT &aData, const DvmhArrayCoefficients<n, IndexT> &aCoefs, const DvmhDiagInfo<IndexT> &aDiagInfo):
            DvmhDiagIndexer<n, n, ElemT, PtrT, IndexT>(aData, aCoefs, aDiagInfo, xIdx, yIdx) {}
};

__device__ double sqrt(int v) { return sqrt((double)v); }

#endif

#endif
