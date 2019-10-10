#pragma once

#include "dvmhlib_warp_red.h"

template<typename T, typename I, int numI>
__inline__ __device__ void __dvmh_blockReduceDummyLoc(T &val, I *index)
{ }

#ifndef CUDA_FERMI_ARCH

// minloc && maxloc operations
template<typename T, typename I, void func(T &val, I *index)>
__inline__ __device__ void __dvmh_blockReduceLoc(T &val, I *index)
{ func(val, index); }

// single and multiple element operations
template<typename T, void func(T *val)>
__inline__ __device__ void __dvmh_blockReduceN(T *val)
{ return func(val); }
template<typename T, T func(T &A)>
__inline__ __device__ T __dvmh_blockReduce(T &val)
{ return func(val); }

// for arrays of unknown size
template<typename T, typename CE, typename N, void func(T *val, CE coef, N num)>
__inline__ __device__ void __dvmh_blockReduceN(T *val, CE coef, N num)
{ return func(val, coef, num); }



template<typename T, typename I, int numI>
__inline__ __device__ void __dvmh_blockReduceMinLoc(T &val, I *index)
{ __dvmh_blockReduceLoc<T, I, __dvmh_warpReduceScalarLoc<T, I, numI, minloc_<T, I, numI> > >(val, index); }
template<typename T, typename I, int numI>
__inline__ __device__ void __dvmh_blockReduceMaxLoc(T &val, I *index)
{ __dvmh_blockReduceLoc<T, I, __dvmh_warpReduceScalarLoc<T, I, numI, maxloc_<T, I, numI> > >(val, index); }

template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceMinN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, min_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceMaxN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, max_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceANDN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, and_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceORN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, or_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceNEQN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, neq_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceEQN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, eq_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceProdN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, prod_, num> >(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceSumN(T *val)
{ __dvmh_blockReduceN<T, __dvmh_warpReduceScalarN<T, sum_, num> >(val); }

// for arrays of unknown size, 1D
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceMinN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, min_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceMaxN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, max_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceANDN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, and_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceORN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, or_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceNEQN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, neq_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceEQN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, eq_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceProdN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, prod_> >(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceSumN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_warpReduceScalarN<T, CE, N, sum_> >(val, coef, num); }

#else

extern __shared__ char shMem[];

template<typename T> __inline__ __device__ T __dvmh_blockReduceMin(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceMax(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceAND(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceOR(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceNEQ(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceEQ(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceProd(T val);
template<typename T> __inline__ __device__ T __dvmh_blockReduceSum(T val);

template<typename T, void func(T *val, T &fin, const unsigned idx, const int lane)>
__inline__ __device__ T __dvmh_blockReduce(T val)
{
    unsigned idx = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
    T *shared = (T*)shMem;
    int lane = idx % warpSize;

    shared[idx] = val;
    __syncthreads();
    func(shared, val, idx, lane);
    __syncthreads();
    return val;
}

template<typename T, int num, T func(T A)>
__inline__ __device__ void __dvmh_blockReduceN(T *val)
{
#pragma unroll
    for (int i = 0; i < num; ++i)
        val[i] = func(val[i]);
}

template<typename T, typename CE, typename N, T func(T A)>
__inline__ __device__ void __dvmh_blockReduceN(T *val, CE coef, N num)
{
#pragma unroll
    for (int i = 0; i < num; ++i)
        val[i * coef] = func(val[i * coef]);
}

template<typename T, typename I, int numI, void func(T *val, I *index, const unsigned idx, const int lane)>
__inline__ __device__ void __dvmh_blockReduceLoc(T &val, I *index)
{
    unsigned idx = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y;
    unsigned blockDims = blockDim.x * blockDim.y * blockDim.z;
    int lane = idx % warpSize;
    T *shared = (T*)shMem;
    I *sharedIdx = (I*)&shared[blockDims];

    shared[idx] = val;
#pragma unroll
    for (int i = 0; i < numI; ++i)
        sharedIdx[idx + i * blockDims] = index[i];
    __syncthreads();
    func(shared, sharedIdx, idx, lane);
    __syncthreads();
    if (lane == 0)
    {
        val = shared[idx];
#pragma unroll
        for (int i = 0; i < numI; ++i)
            index[i] = sharedIdx[idx + i * blockDims];
    }
}

template<typename T, typename I, int numI>
__inline__ __device__ void __dvmh_blockReduceMinLoc(T &val, I *index)
{ __dvmh_blockReduceLoc<T, I, numI, __dvmh_warpReduceScalarLoc<T, I, numI, minloc_<T, I, numI> > >(val, index); }
template<typename T, typename I, int numI>
__inline__ __device__ void __dvmh_blockReduceMaxLoc(T &val, I *index)
{ __dvmh_blockReduceLoc<T, I, numI, __dvmh_warpReduceScalarLoc<T, I, numI, maxloc_<T, I, numI> > >(val, index); }

template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceMinN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceMin>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceMaxN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceMax>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceANDN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceAND>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceORN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceOR>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceNEQN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceNEQ>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceEQN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceEQ>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceProdN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceProd>(val); }
template<typename T, int num>
__inline__ __device__ void __dvmh_blockReduceSumN(T *val)
{ __dvmh_blockReduceN<T, num, __dvmh_blockReduceSum>(val); }

// for arrays of unknown size, 1D
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceMinN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceMin>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceMaxN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceMax>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceANDN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceAND>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceORN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceOR>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceNEQN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceNEQ>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceEQN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceEQ>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceProdN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceProd>(val, coef, num); }
template<typename T, typename CE, typename N>
__inline__ __device__ void __dvmh_blockReduceSumN(T *val, CE coef, N num)
{ __dvmh_blockReduceN<T, CE, N, __dvmh_blockReduceSum>(val, coef, num); }

#endif

template<typename T>
__inline__ __device__ T __dvmh_blockReduceMin(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, min_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceMax(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, max_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceAND(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, and_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceOR(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, or_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceNEQ(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, neq_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceEQ(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, eq_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceProd(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, prod_> >(val); }
template<typename T>
__inline__ __device__ T __dvmh_blockReduceSum(T val)
{ return __dvmh_blockReduce<T, __dvmh_warpReduceScalar<T, sum_> >(val); }
