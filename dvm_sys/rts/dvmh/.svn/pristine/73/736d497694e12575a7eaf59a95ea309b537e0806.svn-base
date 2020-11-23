#include <cstdlib>

#pragma GCC visibility push(hidden)

#include "cuda_device.h"

namespace libdvmh {

#ifdef HAVE_CUDA

template <typename T>
__global__ void tryDirectCopyKernel(T *src, T *dst, int count) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < count)
        dst[idx] = src[idx];
}

template <typename T>
bool tryDirectCopyKernelRunner(T *src, T *dst, int count) {
    bool res = true;
    dim3 threads(256);
    dim3 blocks((count + 255) / 256);

    const unsigned max_bl_x = 65535;
    const unsigned max_elem = max_bl_x * threads.x;
    if (blocks.x > max_bl_x) {
        unsigned z = 0;
        for (; z < blocks.x / max_bl_x; z++) {
            tryDirectCopyKernel<<<max_bl_x, threads>>>(src + z * max_elem, dst + z * max_elem, max_elem);
            blocks.x -= max_bl_x;
        }
        if(blocks.x > 0)
            tryDirectCopyKernel<<<blocks, threads>>>(src + z * max_elem, dst + z * max_elem, count % max_elem);
    } else {
        tryDirectCopyKernel<<<blocks, threads>>>(src, dst, count);
    }

    res = res && cudaDeviceSynchronize() == cudaSuccess;
    res = res && cudaGetLastError() == cudaSuccess;

    cudaDeviceSynchronize();
    cudaGetLastError();

    return res;
}

bool tryDirectCopyDToH(void *devAddr, void *hostAddr, int bytes) {
    void *devHostAddr;
    bool res = true;
    res = res && cudaHostGetDevicePointer(&devHostAddr, hostAddr, 0) == cudaSuccess;
    if (res)
        res = res && tryDirectCopyKernelRunner((double *)devAddr, (double *)devHostAddr, bytes / sizeof(double));
    return res;
}

bool tryDirectCopyHToD(void *hostAddr, void *devAddr, int bytes) {
    void *devHostAddr;
    bool res = true;
    res = res && cudaHostGetDevicePointer(&devHostAddr, hostAddr, 0) == cudaSuccess;
    if (res)
        res = res && tryDirectCopyKernelRunner((double *)devHostAddr, (double *)devAddr, bytes / sizeof(double));
    return res;
}

#endif

}

#pragma GCC visibility pop
