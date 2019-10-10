#pragma once

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

template<typename T>
inline __device__ void sum_(T &A, const T &B) { A += B; }
template<typename T>
inline __device__ void prod_(T &A, const T &B) { A *= B; }
template<typename T>
inline __device__ void max_(T &A, const T &B) { A = MAX(A, B); }
template<typename T>
inline __device__ void min_(T &A, const T &B) { A = MIN(A, B); }
template<typename T>
inline __device__ void or_(T &A, const T &B) { A |= B; }
template<typename T>
inline __device__ void and_(T &A, const T &B) { A &= B; }
template<typename T>
inline __device__ void eq_(T &A, const T &B) { A = ~(A ^ B); }
template<typename T>
inline __device__ void neq_(T &A, const T &B) { A ^= B; }
template<typename T, typename I, int numI>
inline __device__ void maxloc_(T &A, I *Aidx, const T &B, const I *Bidx) {
    if (B > A) {
        A = B;
#pragma unroll
        for (unsigned i = 0; i < numI; i++)
            Aidx[i] = Bidx[i];
    }
}
template<typename T, typename I, int numI>
inline __device__ void minloc_(T &A, I *Aidx, const T &B, const I *Bidx) {
    if (B < A) {
        A = B;
#pragma unroll
        for (unsigned i = 0; i < numI; ++i)
            Aidx[i] = Bidx[i];
    }
}

template<typename T, typename I, int numI>
inline __device__ void dummyloc_(T &A, I *Aidx, const T &B, const I *Bidx)
{ }

#ifndef CUDA_FERMI_ARCH
template<typename T, void func(T &A, const T&B)>
inline __device__ void func_and_shfl_down(T &val, const int delta)
#if __CUDACC_VER_MAJOR__ >= 9
{   func(val, __shfl_down_sync(0xFFFFFFFF, val, (unsigned)delta)); }
#else
{   func(val, __shfl_down(val, (unsigned)delta)); }
#endif

template<typename T, typename I, int numI, void func(T &A, I *B, const T&A1, const I *B1)>
__inline__ __device__ void __dvmh_warpReduceScalarLoc(T &val, I *index)
{
    T local;
    I idx[numI];
#pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
    {
#if __CUDACC_VER_MAJOR__ >= 9
        local = __shfl_down_sync(0xFFFFFFFF, val, (unsigned)offset);
#else
        local = __shfl_down(val, (unsigned)offset);
#endif
#pragma unroll
        for (int i = 0; i < numI; ++i)
#if __CUDACC_VER_MAJOR__ >= 9
            idx[i] = __shfl_down_sync(0xFFFFFFFF,index[i], (unsigned)offset);
#else
            idx[i] = __shfl_down(index[i], (unsigned)offset);
#endif
        func(val, index, local, idx);
    }
}

template<typename T, void func(T &A, const T &B), int num>
__inline__ __device__ void __dvmh_warpReduceScalarN(T *val)
{
#pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
#pragma unroll
        for (int i = 0; i < num; ++i)
            func_and_shfl_down<T, func>(val[i], offset);
}

template<typename T, typename CE, typename N, void func(T &A, const T &B)>
__inline__ __device__ void __dvmh_warpReduceScalarN(T *val, CE coef, N num)
{
#pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
#pragma unroll
        for (int i = 0; i < num; ++i)
            func_and_shfl_down<T, func>(val[i * coef], offset);
}

template<typename T, void func(T &A, const T &B)>
__inline__ __device__ T __dvmh_warpReduceScalar(T &val)
{
#pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
#if __CUDACC_VER_MAJOR__ >= 9
        func(val, __shfl_down_sync(0xFFFFFFFF, val, (unsigned)offset));
#else
        func(val, __shfl_down(val, (unsigned)offset));
#endif
    return val;
}

#else

template<typename T, typename I, int numI, void func(T &A, I *B, const T&A1, const I *B1)>
__inline__ __device__ void __dvmh_warpReduceScalarLoc(T *val, I *index, const unsigned idx, const int lane)
{
    if (lane < 16)
        func(val[idx], index + idx, val[idx + 16], index + idx + numI * 16);
    if (lane < 8)
        func(val[idx], index + idx, val[idx + 8], index + idx + numI * 8);
    if (lane < 4)
        func(val[idx], index + idx, val[idx + 4], index + idx + numI * 4);
    if (lane < 2)
        func(val[idx], index + idx, val[idx + 2], index + idx + numI * 2);
    if (lane < 1)
        func(val[idx], index + idx, val[idx + 1], index + idx + numI * 1);
}

template<typename T, void func(T &A, const T &B)>
__inline__ __device__ void __dvmh_warpReduceScalar(T *val, T &fin, const unsigned idx, const int lane)
{
    if (lane < 16)
        func(val[idx], val[idx + 16]);
    if (lane < 8)
        func(val[idx], val[idx + 8]);
    if (lane < 4)
        func(val[idx], val[idx + 4]);
    if (lane < 2)
        func(val[idx], val[idx + 2]);
    if (lane < 1)
    {
        func(val[idx], val[idx + 1]);
        fin = val[idx];
    }
}

#endif

#undef MIN
#undef MAX
