#include <cstdlib>

#pragma GCC visibility push(hidden)

#include "cuda_reduction.h"

#ifdef HAVE_CUDA
#include "include/dvmhlib_block_red.h"
#endif

#include "cuda_utils.h"
#include "dvmh_device.h"
#include "dvmh_log.h"
#include "dvmh_stat.h"
#include "util.h"

namespace libdvmh {

#ifdef HAVE_CUDA

template <typename T, typename SizeType>
__global__ void performReplicate(T val, SizeType quantity, T *devPtr) {
    SizeType tid = (SizeType)blockIdx.x * blockDim.x + threadIdx.x;
    SizeType step = (SizeType)gridDim.x * blockDim.x;
    while (tid < quantity) {
        devPtr[tid] = val;
        tid += step;
    }
}

template <typename T, typename SizeType>
__global__ void performReplicateCommon(char *devPtr, SizeType recordSize, SizeType quantity) {
    unsigned vectorSize = blockDim.x;
    SizeType tid = (SizeType)blockIdx.x * blockDim.y + threadIdx.y;
    SizeType step = (SizeType)gridDim.x * blockDim.y;
    while (tid < quantity) {
        vectMemcpy<T, SizeType>(devPtr + tid * recordSize, devPtr, recordSize, vectorSize, threadIdx.x);
        tid += step;
    }
}
#endif

void dvmhCudaReplicate(CudaDevice *cudaDev, void *addr, UDvmType recordSize, UDvmType quantity, void *devPtr) {
#ifdef HAVE_CUDA
    bool canDoMemset = recordSize < 100000;
    for (UDvmType i = 0; canDoMemset && i < recordSize; i++)
        canDoMemset = canDoMemset && (*(char *)addr == *((char *)addr + i));
    if (canDoMemset) {
        quantity *= recordSize;
        recordSize = 1;
    }
    dim3 threads(256);
    dim3 blocks(std::min((UDvmType)cudaDev->maxGridSize[0], divUpU(quantity, threads.x)));
    if (recordSize == sizeof(char)) {
        cudaMemset(devPtr, *(char *)addr, quantity);
    } else if (recordSize == sizeof(short)) {
        performReplicate<<<blocks, threads>>>(*(short *)addr, quantity, (short *)devPtr);
    } else if (recordSize == sizeof(float)) {
        performReplicate<<<blocks, threads>>>(*(float *)addr, quantity, (float *)devPtr);
    } else if (recordSize == sizeof(double)) {
        performReplicate<<<blocks, threads>>>(*(double *)addr, quantity, (double *)devPtr);
    } else if (recordSize == sizeof(double2)) {
        performReplicate<<<blocks, threads>>>(*(double2 *)addr, quantity, (double2 *)devPtr);
    } else if (recordSize == sizeof(float3)) {
        performReplicate<<<blocks, threads>>>(*(float3 *)addr, quantity, (float3 *)devPtr);
    } else if (recordSize == sizeof(double3)) {
        performReplicate<<<blocks, threads>>>(*(double3 *)addr, quantity, (double3 *)devPtr);
    } else {
        dim3 threads(8, 32);
        dim3 blocks(std::min((UDvmType)cudaDev->maxGridSize[0], divUpU(quantity, threads.y)));
        cudaDev->setValue(devPtr, addr, recordSize);
        if (canTreatAs<double>(devPtr) && recordSize % sizeof(double) == 0)
            performReplicateCommon<double><<<blocks, threads>>>((char *)devPtr, recordSize, quantity);
        else if (canTreatAs<float>(devPtr) && recordSize % sizeof(float) == 0)
            performReplicateCommon<float><<<blocks, threads>>>((char *)devPtr, recordSize, quantity);
        else
            performReplicateCommon<char><<<blocks, threads>>>((char *)devPtr, recordSize, quantity);
    }
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

#ifdef HAVE_CUDA

#ifndef CUDA_FERMI_ARCH

#if __CUDACC_VER_MAJOR__ >= 9
inline __device__ float_complex __shfl_down_sync(unsigned mask, const float_complex &c1, const unsigned delta) {
    float_complex ret;
    ret.re = ::__shfl_down_sync(mask, c1.re, delta);
    ret.im = ::__shfl_down_sync(mask, c1.im, delta);
    return ret;
}

inline __device__ double_complex __shfl_down_sync(unsigned mask, const double_complex &c1, const unsigned delta) {
    double_complex ret;
    ret.re = ::__shfl_down_sync(mask, c1.re, delta);
    ret.im = ::__shfl_down_sync(mask, c1.im, delta);
    return ret;
}
#else
inline __device__ float_complex __shfl_down(const float_complex &c1, const unsigned delta) {
    float_complex ret;
    ret.re = ::__shfl_down(c1.re, delta);
    ret.im = ::__shfl_down(c1.im, delta);
    return ret;
}

inline __device__ double_complex __shfl_down(const double_complex &c1, const unsigned delta) {
    double_complex ret;
    ret.re = ::__shfl_down(c1.re, delta);
    ret.im = ::__shfl_down(c1.im, delta);
    return ret;
}
#endif

#endif

#define DEF_HOST(func, body) \
template <typename T> \
inline bool func##_func(T &a, const T &b) body

DEF_HOST(sum, {a += b; return false;})
DEF_HOST(prod, {a *= b; return false;})
DEF_HOST(max, {if (a < b) {a = b; return true;} return false;})
DEF_HOST(min, {if (a > b) {a = b; return true;} return false;})
DEF_HOST(and, {a &= b; return false;})
DEF_HOST(or, {a |= b; return false;})
DEF_HOST(neq, {a ^= b; return false;})
DEF_HOST(eq, {a = ~(a ^ b); return false;})

#undef DEF_HOST

template <typename T, typename SizeType, void func(T &A, const T &B), T __dvmh_blockRED(T val)>
__global__ void perform_reduction_kernel_scalar(const T *data_array, T *result, SizeType elem_count, const T init_val) {
    SizeType idx = (SizeType)blockIdx.x * blockDim.x + threadIdx.x;
    SizeType stride = (SizeType)blockDim.x * gridDim.x;
    T thread_val = init_val;

    for (SizeType k = idx; k < elem_count; k += stride)
        func(thread_val, data_array[k]);

    thread_val = __dvmh_blockRED(thread_val);
    if (idx % warpSize == 0)
        result[idx / warpSize] = thread_val;
}

template <typename T, typename SizeType, void func(T &A, const T &B), T __dvmh_blockRED(T val)>
__global__ void perform_reduction_kernel_array(const T *data_array, T *result, SizeType elem_count, const T init_val, SizeType array_dim) {
    SizeType idx = (SizeType)blockIdx.x * blockDim.x + threadIdx.x;
    SizeType stride = (SizeType)blockDim.x * gridDim.x;

    for (SizeType arr_d = 0; arr_d < array_dim; arr_d++) {
        T thread_val = init_val;
        for (SizeType k = idx; k < elem_count; k += stride)
            func(thread_val, data_array[k]);

        thread_val = __dvmh_blockRED(thread_val);
        if (idx % warpSize == 0)
            result[idx / warpSize] = thread_val;

        data_array += elem_count;
        result += stride / warpSize;
    }
}

template <typename T, typename SizeType, void func_loc(T &A, SizeType *Aidx, const T &B, const SizeType *Bidx), void __dvmh_blockRED_LOC(T &val, SizeType *loc)>
__global__ void perform_reduction_kernel_scalar_loc(const T *data_array, T *result, SizeType *result_loc, SizeType elem_count, const T init_val) {
    SizeType idx = (SizeType)blockIdx.x * blockDim.x + threadIdx.x;
    SizeType stride = (SizeType)blockDim.x * gridDim.x;
    SizeType data_loc = idx;
    T thread_val = init_val;
    SizeType thread_loc = data_loc;

    for (SizeType k = idx; k < elem_count; k += stride, data_loc += stride)
        func_loc(thread_val, &thread_loc, data_array[k], &data_loc);

    __dvmh_blockRED_LOC(thread_val, &thread_loc);
    if (idx % warpSize == 0) {
        result[idx / warpSize] = thread_val;
        result_loc[idx / warpSize] = thread_loc;
    }
}

template <typename T,
          typename SizeType,
          bool func(T &, const T &),
          void func_dev(T &A, const T &B),
          void func_dev_loc(T &A, SizeType *Aidx, const T &B, const SizeType *Bidx),
          T    __dvmh_blockRED    (T val),
          void __dvmh_blockRED_LOC(T &val, SizeType *loc) >
static void perform_reduction(CudaDevice *cudaDev, UDvmType items, T *vars, UDvmType length, const char *locs, UDvmType locSize, const T init_val, T *res,
        char *locRes)
{
    dim3 threads = dim3(512);
    dim3 blocks = dim3(cudaDev->SMcount * cudaDev->maxThreadsPerSM / threads.x);
    unsigned hostBlocks = threads.x * blocks.x / cudaDev->warpSize;
    bool computeFlag;
    if (hostBlocks > items) {
        hostBlocks = items;
        computeFlag = false;
    } else {
        computeFlag = true;
    }

    cudaDev->deviceSynchronize();
    DvmhTimer tm(true);

    T *tempRes = 0;
    SizeType *tempLocRes = 0;

    if (computeFlag) {
        tempRes = cudaDev->alloc<T>(hostBlocks * length);
        if (locSize > 0)
            tempLocRes = cudaDev->alloc<SizeType>(hostBlocks * length);
        dvmh_log(TRACE, "blocks=%u threads=%u sizeof(T)=%d locSize=" UDTFMT " length=" UDTFMT, blocks.x, threads.x, (int)sizeof(T), locSize, length);
    } else {
        tempRes = vars;
        dvmh_log(TRACE, "items=" UDTFMT " (not computing on GPU) locSize=" UDTFMT " length=" UDTFMT, items, locSize, length);
    }
#ifdef CUDA_FERMI_ARCH
    unsigned sharedPerBlock = threads.x * (sizeof(T) + (locSize > 0 ? sizeof(SizeType) : 0));
#else
    unsigned sharedPerBlock = 0;
#endif
    if (computeFlag) {
        if (locSize <= 0) {
            if (length == 1)
                perform_reduction_kernel_scalar<T, SizeType, func_dev, __dvmh_blockRED><<<blocks, threads, sharedPerBlock>>>(vars, tempRes, items, init_val);
            else
                perform_reduction_kernel_array<T, SizeType, func_dev, __dvmh_blockRED><<<blocks, threads, sharedPerBlock>>>(vars, tempRes, items, init_val,
                        length);
        } else {
            if (length == 1)
                perform_reduction_kernel_scalar_loc<T, SizeType, func_dev_loc, __dvmh_blockRED_LOC><<<blocks, threads, sharedPerBlock>>>(vars, tempRes,
                        tempLocRes, items, init_val);
            else
                checkInternal2(false, "Reduction of minloc/maxloc for arrays is not supported yet");
        }
    }

    T *tempHostRes = new T[hostBlocks * length];
    SizeType *tempHostLocRes = new SizeType[hostBlocks * length];

    cudaDev->getValues(tempRes, tempHostRes, hostBlocks * length);
    if (locSize > 0) {
        if (computeFlag) {
            cudaDev->getValues(tempLocRes, tempHostLocRes, hostBlocks * length);
        } else {
            for (unsigned i = 0; i < hostBlocks; i++)
                tempHostLocRes[i] = i;
        }
    }

    if (computeFlag) {
        cudaDev->dispose(tempRes);
        if (locSize > 0)
            cudaDev->dispose(tempLocRes);
    }

    for (UDvmType j = 0; j < length; j++) {
        SizeType hostLocRes = tempHostLocRes[0];
        for (unsigned i = 0; i < hostBlocks; i++) {
            if (func(res[j], tempHostRes[j * hostBlocks + i]))
                if (locSize > 0)
                    hostLocRes = tempHostLocRes[i];
        }
        if (locSize > 0)
            cudaDev->getValue(locs + hostLocRes * locSize, locRes + j * locSize, locSize);
    }

    delete[] tempHostRes;
    delete[] tempHostLocRes;
    //XXX: it is not necessary here
    //cudaDev->deviceSynchronize();
    double elapsedT = tm.total();
    dvmh_stat_add_measurement(cudaDev->index, DVMH_STAT_METRIC_UTIL_ARRAY_REDUCTION, elapsedT, 0.0, elapsedT);
}

#define DEFINE_REDFUNC(func, funcLoc, funcBlock, funcBlockLoc, type) DECLARE_REDFUNC(func, type) { \
    if (items <= UINT_MAX) \
        perform_reduction<type, unsigned, func##_func<type>, func##_<type>, funcLoc##_<type, unsigned, 1>, __dvmh_blockReduce##funcBlock<type>, \
                __dvmh_blockReduce##funcBlockLoc<type, unsigned, 1>  >(cudaDev, items, vars, length, locs, locSize, init_val, res, locRes); \
    else \
        perform_reduction<type, UDvmType, func##_func<type>, func##_<type>, funcLoc##_<type, UDvmType, 1>, __dvmh_blockReduce##funcBlock<type>, \
                __dvmh_blockReduce##funcBlockLoc<type, UDvmType, 1>  >(cudaDev, items, vars, length, locs, locSize, init_val, res, locRes); \
}

#else
#define DEFINE_REDFUNC(func, funcLoc, funcBlock, funcBlockLoc, type) DECLARE_REDFUNC(func, type) { \
    checkInternal2(0, "RTS is compiled without support for CUDA"); \
}
#endif

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, char);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, char);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, char);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, char);
DEFINE_REDFUNC(and, dummyloc, AND, DummyLoc, char);
DEFINE_REDFUNC(or, dummyloc, OR, DummyLoc, char);
DEFINE_REDFUNC(neq, dummyloc, NEQ, DummyLoc, char);
DEFINE_REDFUNC(eq, dummyloc, EQ, DummyLoc, char);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, int);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, int);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, int);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, int);
DEFINE_REDFUNC(and, dummyloc, AND, DummyLoc, int);
DEFINE_REDFUNC(or, dummyloc, OR, DummyLoc, int);
DEFINE_REDFUNC(neq, dummyloc, NEQ, DummyLoc, int);
DEFINE_REDFUNC(eq, dummyloc, EQ, DummyLoc, int);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, long);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, long);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, long);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, long);
DEFINE_REDFUNC(and, dummyloc, AND, DummyLoc, long);
DEFINE_REDFUNC(or, dummyloc, OR, DummyLoc, long);
DEFINE_REDFUNC(neq, dummyloc, NEQ, DummyLoc, long);
DEFINE_REDFUNC(eq, dummyloc, EQ, DummyLoc, long);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, long_long);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, long_long);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, long_long);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, long_long);
DEFINE_REDFUNC(and, dummyloc, AND, DummyLoc, long_long);
DEFINE_REDFUNC(or, dummyloc, OR, DummyLoc, long_long);
DEFINE_REDFUNC(neq, dummyloc, NEQ, DummyLoc, long_long);
DEFINE_REDFUNC(eq, dummyloc, EQ, DummyLoc, long_long);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, float);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, float);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, float);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, float);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, double);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, double);
DEFINE_REDFUNC(max, maxloc, Max, MaxLoc, double);
DEFINE_REDFUNC(min, minloc, Min, MinLoc, double);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, float_complex);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, float_complex);

DEFINE_REDFUNC(sum, dummyloc, Sum, DummyLoc, double_complex);
DEFINE_REDFUNC(prod, dummyloc, Prod, DummyLoc, double_complex);

}

#pragma GCC visibility pop
