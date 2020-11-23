#pragma once

#include "dvmh_types.h"

namespace libdvmh {

class CudaDevice;

void dvmhCudaTransformCutting(CudaDevice *cudaDev, void *fromArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Ry, UDvmType Rz, UDvmType xStart,
        UDvmType xStep, UDvmType xCount, UDvmType yStart, UDvmType yStep, UDvmType yCount, UDvmType zStep, bool back_flag, bool slash_flag, void *toArr_gpu,
        bool compact_flag = true);
void dvmhCudaTransformArray(CudaDevice *cudaDev, void *fromArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Ry, UDvmType Rz, bool back_flag, bool slash_flag,
        void *toArr_gpu);

bool dvmhCudaCanTransposeInplace(CudaDevice *cudaDev, void *addr, UDvmType typeSize, UDvmType Rx, UDvmType Rb, UDvmType Ry, UDvmType Rz);
void dvmhCudaTransposeArray(CudaDevice *cudaDev, void *fromArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Rb, UDvmType Ry, UDvmType Rz, void *toArr_gpu);

void dvmhCudaPermutateArray(CudaDevice *cudaDev, void *oldMem, UDvmType typeSize, int rank, UDvmType sizes[], int perm[], UDvmType Rz, void *newMem);

}
