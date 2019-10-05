#include <cstdlib>

#pragma GCC visibility push(hidden)

#include "cuda_copying.h"

#include <cassert>

#include "dvmh_device.h"
#include "dvmh_log.h"
#include "util.h"

namespace libdvmh {

bool dvmhCudaCanDirectCopy(int rank, UDvmType typeSize, CommonDevice *dev1, const DvmType header1[], CommonDevice *dev2, const DvmType header2[]) {
#ifdef HAVE_CUDA
    if (rank <= 7 && (typeSize == sizeof(float) || typeSize == sizeof(double) || typeSize == sizeof(double2))) {
        if (dev1->getType() == dtCuda && dev2->getType() == dtCuda && dev1 == dev2)
            return true; // Local copying
        if (dev1->getType() == dtCuda && dev2->getType() == dtCuda) {
            CudaDevice *cudaDev1 = (CudaDevice *)dev1;
            CudaDevice *cudaDev2 = (CudaDevice *)dev2;
            if (cudaDev1->canAccessPeers & (1 << cudaDev2->index))
                return true; // Can access peer
        }
        void *tmpVar;
        if (dev1->getType() == dtCuda && dev2->getType() == dtHost && ((CudaDevice *)dev1)->canMapHostMemory) {
            // Copying to host memory
            ((CudaDevice *)dev1)->setAsCurrent();
            return (cudaHostGetDevicePointer(&tmpVar, (void *)header2[rank + 2], 0) == cudaSuccess);
        }
        if (dev1->getType() == dtHost && dev2->getType() == dtCuda && ((CudaDevice *)dev2)->canMapHostMemory) {
            // Copying from host memory
            ((CudaDevice *)dev2)->setAsCurrent();
            return (cudaHostGetDevicePointer(&tmpVar, (void *)header1[rank + 2], 0) == cudaSuccess);
        }
    }
    return false;
#else
    return false;
#endif
}

#ifdef HAVE_CUDA

template <typename T>
__global__ void copyCutting0d(const T *base1, CudaIndexType offs1, T *base2, CudaIndexType offs2)
{
    CudaIndexType idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx == 0)
        base2[offs2] = base1[offs1];
}

template <typename T>
__global__ void copyCutting1d(CudaIndexType idxOffs,
        CudaIndexType L1,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1) {
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1] = base1[offs1 + coef1_1 * idx1];
    }
}

template <typename T>
__global__ void copyCutting2d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2) {
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2] = base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2];
    }
}

template <typename T>
__global__ void copyCutting3d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2, CudaIndexType L3,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2, CudaIndexType coef1_3,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2, CudaIndexType coef2_3)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2 * L3) {
        CudaIndexType idx3 = idx % L3;
        idx /= L3;
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2 + coef2_3 * idx3] = base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2 + coef1_3 * idx3];
    }
}

template <typename T>
__global__ void copyCutting4d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2, CudaIndexType L3, CudaIndexType L4,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2, CudaIndexType coef1_3, CudaIndexType coef1_4,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2, CudaIndexType coef2_3, CudaIndexType coef2_4)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2 * L3 * L4) {
        CudaIndexType idx4 = idx % L4;
        idx /= L4;
        CudaIndexType idx3 = idx % L3;
        idx /= L3;
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2 + coef2_3 * idx3 + coef2_4 * idx4] =
                base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2 + coef1_3 * idx3 + coef1_4 * idx4];
    }
}

template <typename T>
__global__ void copyCutting5d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2, CudaIndexType L3, CudaIndexType L4, CudaIndexType L5,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2, CudaIndexType coef1_3, CudaIndexType coef1_4, CudaIndexType coef1_5,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2, CudaIndexType coef2_3, CudaIndexType coef2_4, CudaIndexType coef2_5)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2 * L3 * L4 * L5) {
        CudaIndexType idx5 = idx % L5;
        idx /= L5;
        CudaIndexType idx4 = idx % L4;
        idx /= L4;
        CudaIndexType idx3 = idx % L3;
        idx /= L3;
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2 + coef2_3 * idx3 + coef2_4 * idx4 + coef2_5 * idx5] =
                base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2 + coef1_3 * idx3 + coef1_4 * idx4 + coef1_5 * idx5];
    }
}

template <typename T>
__global__ void copyCutting6d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2, CudaIndexType L3, CudaIndexType L4, CudaIndexType L5, CudaIndexType L6,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2, CudaIndexType coef1_3, CudaIndexType coef1_4, CudaIndexType coef1_5,
        CudaIndexType coef1_6,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2, CudaIndexType coef2_3, CudaIndexType coef2_4, CudaIndexType coef2_5,
        CudaIndexType coef2_6)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2 * L3 * L4 * L5 * L6) {
        CudaIndexType idx6 = idx % L6;
        idx /= L6;
        CudaIndexType idx5 = idx % L5;
        idx /= L5;
        CudaIndexType idx4 = idx % L4;
        idx /= L4;
        CudaIndexType idx3 = idx % L3;
        idx /= L3;
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2 + coef2_3 * idx3 + coef2_4 * idx4 + coef2_5 * idx5 + coef2_6 * idx6] =
                base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2 + coef1_3 * idx3 + coef1_4 * idx4 + coef1_5 * idx5 + coef1_6 * idx6];
    }
}

template <typename T>
__global__ void copyCutting7d(CudaIndexType idxOffs,
        CudaIndexType L1, CudaIndexType L2, CudaIndexType L3, CudaIndexType L4, CudaIndexType L5, CudaIndexType L6, CudaIndexType L7,
        const T *base1, CudaIndexType offs1, CudaIndexType coef1_1, CudaIndexType coef1_2, CudaIndexType coef1_3, CudaIndexType coef1_4, CudaIndexType coef1_5,
        CudaIndexType coef1_6, CudaIndexType coef1_7,
        T *base2, CudaIndexType offs2, CudaIndexType coef2_1, CudaIndexType coef2_2, CudaIndexType coef2_3, CudaIndexType coef2_4, CudaIndexType coef2_5,
        CudaIndexType coef2_6, CudaIndexType coef2_7)
{
    CudaIndexType idx = idxOffs + blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < L1 * L2 * L3 * L4 * L5 * L6 * L7) {
        CudaIndexType idx7 = idx % L7;
        idx /= L7;
        CudaIndexType idx6 = idx % L6;
        idx /= L6;
        CudaIndexType idx5 = idx % L5;
        idx /= L5;
        CudaIndexType idx4 = idx % L4;
        idx /= L4;
        CudaIndexType idx3 = idx % L3;
        idx /= L3;
        CudaIndexType idx2 = idx % L2;
        idx /= L2;
        CudaIndexType idx1 = idx;
        base2[offs2 + coef2_1 * idx1 + coef2_2 * idx2 + coef2_3 * idx3 + coef2_4 * idx4 + coef2_5 * idx5 + coef2_6 * idx6 + coef2_7 * idx7] =
                base1[offs1 + coef1_1 * idx1 + coef1_2 * idx2 + coef1_3 * idx3 + coef1_4 * idx4 + coef1_5 * idx5 + coef1_6 * idx6 + coef1_7 * idx7];
    }
}

template <typename T>
static void doDeal(int rank, dim3 blocks, dim3 threads, UDvmType doneAmount, const CudaIndexType Ls[], void *base1in, DvmType offs1, const DvmType header1[],
        void *base2in, DvmType offs2, const DvmType header2[]) {
    T *base1 = (T *)base1in;
    T *base2 = (T *)base2in;
    if (rank == 0) {
        copyCutting0d<<<blocks, threads>>>(base1, offs1, base2, offs2);
    } else if (rank == 1) {
        copyCutting1d<<<blocks, threads>>>(doneAmount, Ls[0], base1, offs1, header1[1], base2, offs2, header2[1]);
    } else if (rank == 2) {
        copyCutting2d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], base1, offs1, header1[1], header1[2], base2, offs2, header2[1], header2[2]);
    } else if (rank == 3) {
        copyCutting3d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], Ls[2],
                base1, offs1, header1[1], header1[2], header1[3],
                base2, offs2, header2[1], header2[2], header2[3]);
    } else if (rank == 4) {
        copyCutting4d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], Ls[2], Ls[3],
                base1, offs1, header1[1], header1[2], header1[3], header1[4],
                base2, offs2, header2[1], header2[2], header2[3], header2[4]);
    } else if (rank == 5) {
        copyCutting5d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], Ls[2], Ls[3], Ls[4],
                base1, offs1, header1[1], header1[2], header1[3], header1[4], header1[5],
                base2, offs2, header2[1], header2[2], header2[3], header2[4], header2[5]);
    } else if (rank == 6) {
        copyCutting6d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], Ls[2], Ls[3], Ls[4], Ls[5],
                base1, offs1, header1[1], header1[2], header1[3], header1[4], header1[5], header1[6],
                base2, offs2, header2[1], header2[2], header2[3], header2[4], header2[5], header2[6]);
    } else if (rank == 7) {
        copyCutting7d<<<blocks, threads>>>(doneAmount, Ls[0], Ls[1], Ls[2], Ls[3], Ls[4], Ls[5], Ls[6],
                base1, offs1, header1[1], header1[2], header1[3], header1[4], header1[5], header1[6], header1[7],
                base2, offs2, header2[1], header2[2], header2[3], header2[4], header2[5], header2[6], header2[7]);
    } else
        checkInternal2(false, "Internal inconsistency");
}

#endif

bool dvmhCudaDirectCopy(int rank, UDvmType typeSize, CommonDevice *dev1, const DvmType header1[], CommonDevice *dev2, const DvmType header2[],
        const Interval cutting[]) {
    static double copyTime = 0;
    static UDvmType copyAmount = 0;
    if (!dvmhCudaCanDirectCopy(rank, typeSize, dev1, header1, dev2, header2))
        return false;
#ifdef HAVE_CUDA
#ifdef NON_CONST_AUTOS
    CudaIndexType Ls[rank];
#else
    CudaIndexType Ls[MAX_ARRAY_RANK];
#endif
    DvmType offs1, offs2;
    UDvmType totalAmount;
    offs1 = header1[rank + 1];
    offs2 = header2[rank + 1];
    totalAmount = 1;
    for (int i = 0; i < rank; i++) {
        Ls[i] = cutting[i].size();
        offs1 += cutting[i].begin() * header1[i + 1];
        offs2 += cutting[i].begin() * header2[i + 1];
        totalAmount *= Ls[i];
    }
    checkInternal2(dev1->getType() == dtCuda || dev2->getType() == dtCuda, "Internal inconsistency");
    CudaDevice *dev;
    if (dev1->getType() == dtCuda)
        dev = (CudaDevice *)dev1;
    else
        dev = (CudaDevice *)dev2;
    dev->setAsCurrent();
    void *base1 = (void *)header1[rank + 2];
    void *base2 = (void *)header2[rank + 2];
    if (dev1->getType() == dtHost)
        checkInternalCuda(cudaHostGetDevicePointer(&base1, base1, 0));
    if (dev2->getType() == dtHost)
        checkInternalCuda(cudaHostGetDevicePointer(&base2, base2, 0));
    DvmhTimer tm;
    if (needToCollectTimes) {
        dev->deviceSynchronize();
        tm.start();
    }
    UDvmType doneAmount = 0;
    while (doneAmount < totalAmount) {
        dim3 threads(256);
        UDvmType currentAmount = std::min(totalAmount - doneAmount, (UDvmType)(threads.x) * dev->maxGridSize[0]);
        dim3 blocks(divUpU(currentAmount, threads.x));
        dvmh_log(TRACE, "copying " UDTFMT " bytes (" UDTFMT " elements) directly by GPU", currentAmount * typeSize, currentAmount);
        if (typeSize == sizeof(float))
            doDeal<float>(rank, blocks, threads, doneAmount, Ls, base1, offs1, header1, base2, offs2, header2);
        else if (typeSize == sizeof(double))
            doDeal<double>(rank, blocks, threads, doneAmount, Ls, base1, offs1, header1, base2, offs2, header2);
        else if (typeSize == sizeof(double2))
            doDeal<double2>(rank, blocks, threads, doneAmount, Ls, base1, offs1, header1, base2, offs2, header2);
        else
            checkInternal2(false, "Internal inconsistency");
        if (dvmhSettings.alwaysSync)
            dev->deviceSynchronize();
        doneAmount += currentAmount;
    }
    dev->deviceSynchronize();
    if (needToCollectTimes) {
        double timeNow = tm.total();
        copyTime += timeNow;
        copyAmount += totalAmount * typeSize;
        dvmh_log(TRACE, "GPU direct copy time now = %g. Overall = %g. Overall amount = " UDTFMT, timeNow, copyTime, copyAmount);
    }
    return true;
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
    return false;
#endif
}

}

#pragma GCC visibility pop
