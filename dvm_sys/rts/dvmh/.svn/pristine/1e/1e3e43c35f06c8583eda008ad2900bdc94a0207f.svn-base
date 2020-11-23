#include <cstdlib>

#pragma GCC visibility push(hidden)

#include "cuda_transform.h"

#include <cassert>

#ifdef HAVE_CUDA
#include "include/dvmhlib_device.h"
#endif

#include "cuda_utils.h"
#include "dvmh_device.h"
#include "dvmh_log.h"
#include "settings.h"
#include "util.h"

namespace libdvmh {

#ifdef HAVE_CUDA

#define BLOCK_DIM 16

// NOTE: IndexType must be signed type
template <typename T, typename IndexType, int slash, int back, int cmp_X_Y, int manyZ>
__global__ void transformDiagonalWhole(const T *src, T *dst, const IndexType Rx, const IndexType Ry, const IndexType Rz) {
    __shared__ T data[BLOCK_DIM][BLOCK_DIM + 1];
    __shared__ IndexType sharedIdx[BLOCK_DIM][BLOCK_DIM + 1];
    __shared__ bool conditions[BLOCK_DIM][BLOCK_DIM + 1];
    bool condition;

    IndexType shift;
    int revX, revY;
    if (slash == 0) {
        shift = -threadIdx.y;
        revX = BLOCK_DIM - 1 - threadIdx.x;
        revY = BLOCK_DIM - 1 - threadIdx.y;
    } else {
        shift = threadIdx.y - BLOCK_DIM;
        revX = threadIdx.x;
        revY = threadIdx.y;
    }

    IndexType x = (IndexType)blockIdx.x * blockDim.x + threadIdx.x + shift;
    IndexType y = (IndexType)blockIdx.y * blockDim.y + threadIdx.y;
    IndexType z = (IndexType)blockIdx.z * blockDim.z + threadIdx.z;

    dvmh_convert_XY<IndexType, slash, cmp_X_Y>(x, y, Rx, Ry, sharedIdx[threadIdx.y][threadIdx.x]);
    condition = (0 <= x && 0 <= y && x < Rx && y < Ry);
    conditions[threadIdx.y][threadIdx.x] = condition;
    if (back == 1)
        __syncthreads();

#pragma unroll
    for (int zz = z; zz < z + manyZ; ++zz) {
        IndexType normIdx = x + Rx * (y + Ry * zz);

        if (back == 0) {
            if (condition && zz < Rz)
                data[threadIdx.y][threadIdx.x] = src[normIdx];
            __syncthreads();
            if (conditions[revX][revY] && zz < Rz)
                dst[sharedIdx[revX][revY] + zz * Rx * Ry] = data[revX][revY];
        } else {
            if (conditions[revX][revY] && zz < Rz)
                data[revX][revY] = src[sharedIdx[revX][revY] + zz * Rx * Ry];
            __syncthreads();
            if (condition && zz < Rz)
                dst[normIdx] = data[threadIdx.y][threadIdx.x];
        }
    }
}

template <typename T, typename IndexType>
__global__ void transformDiagonalCutting(const T *src, T *dst,
                                         const IndexType Rx, const IndexType Ry, const IndexType zCount,
                                         const IndexType xStart, const IndexType xStep,  const IndexType xCount,
                                         const IndexType yStart, const IndexType yStep,  const IndexType yCount,
                                         const IndexType zStep, const int slash, const bool back, const bool compact)
{
    IndexType x = (IndexType)blockIdx.x * blockDim.x + threadIdx.x;
    IndexType y = (IndexType)blockIdx.y * blockDim.y + threadIdx.y;
    IndexType z = (IndexType)blockIdx.z * blockDim.z + threadIdx.z;
    if (x < xCount && y < yCount && z < zCount) {
        IndexType xReal = xStart + x * xStep;
        IndexType yReal = yStart + y * yStep;
        IndexType zReal = z * zStep;
        IndexType diagIdx;
        dvmh_convert_XY<IndexType>(xReal, yReal, Rx, Ry, slash, diagIdx);
        diagIdx += zReal * Rx * Ry;
        IndexType normIdx = (compact ? x + xCount * (y + yCount * z) : xReal + Rx * (yReal + Ry * zReal));
        if (back)
            dst[normIdx] = src[diagIdx];
        else
            dst[diagIdx] = src[normIdx];
    }
}

// TODO: try to increase manyZ
template <typename T, typename IndexType>
static void transform_array(CudaDevice *cudaDev,
                            const T *fromArr_gpu, T *toArr_gpu,
                            UDvmType Rx, UDvmType Ry, UDvmType Rz,
                            UDvmType xStart, UDvmType xStep, UDvmType xCount, UDvmType yStart, UDvmType yStep, UDvmType yCount,
                            UDvmType zStep, bool back_flag, bool slash_flag, bool compact_flag)
{
    const int manyZ = 1;
    UDvmType zCount = divDownU(Rz, zStep);
    dim3 threads = dim3(BLOCK_DIM, BLOCK_DIM, manyZ);
    dim3 threadsReal = dim3(BLOCK_DIM, BLOCK_DIM, 1);

    bool useWhole = xCount == Rx && yCount == Ry && (zCount == Rz || zCount == 1 || (threads.z == 1 && cudaDev->maxGridSize[2] == 1));
    dim3 blocks(divUpU(xCount, threads.x) + (useWhole ? 1 : 0), divUpU(yCount, threads.y));
    UDvmType zDone = 0;

    // TODO: create new solution for compilation this thing in Windows and old version of GCC
#ifdef WIN32_TMP
    if (cudaDev->setupKernel((const void *)&transformDiagonalWhole<T, IndexType, 0, 0, 0, manyZ>))
#endif
    {
        if(!(cudaDev->preferL1)) {
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 0, 0, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 0, 1, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 0, -1, manyZ>, cudaFuncCachePreferShared);

            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 1, 0, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 1, 1, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 0, 1, -1, manyZ>, cudaFuncCachePreferShared);

            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 0, 0, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 0, 1, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 0, -1, manyZ>, cudaFuncCachePreferShared);

            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 1, 0, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 1, 1, manyZ>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transformDiagonalWhole<T, IndexType, 1, 1, -1, manyZ>, cudaFuncCachePreferShared);

            dvmh_log(TRACE, "Cache configuration 'cudaFuncCachePreferShared' is set for 'transformDiagonalWhole' kernels");
        }
    }

    while (zDone < zCount) {
        blocks.z = std::min(divUpU(zCount - zDone, threadsReal.z), (UDvmType)cudaDev->maxGridSize[2]);
        UDvmType zNow = std::min((UDvmType)threadsReal.z * blocks.z, zCount - zDone);
        UDvmType fromArrOffs = zDone * (compact_flag ? xCount * yCount : zStep * Rx * Ry);
        UDvmType toArrOffs = zDone * zStep * Rx * Ry;
        if (back_flag)
            std::swap(fromArrOffs, toArrOffs);
        if (useWhole) {
            if (!slash_flag) {
                if (!back_flag) {
                    if (Rx == Ry)
                        transformDiagonalWhole<T, IndexType, 0, 0, 0, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else if (Rx < Ry)
                        transformDiagonalWhole<T, IndexType, 0, 0, 1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else
                        transformDiagonalWhole<T, IndexType, 0, 0, -1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);

                } else {
                    if (Rx == Ry)
                        transformDiagonalWhole<T, IndexType, 0, 1, 0, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else if (Rx < Ry)
                        transformDiagonalWhole<T, IndexType, 0, 1, 1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else
                        transformDiagonalWhole<T, IndexType, 0, 1, -1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                }
            } else {
                if (!back_flag) {
                    if (Rx == Ry)
                        transformDiagonalWhole<T, IndexType, 1, 0, 0, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else if (Rx < Ry)
                        transformDiagonalWhole<T, IndexType, 1, 0, 1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else
                        transformDiagonalWhole<T, IndexType, 1, 0, -1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                } else {
                    if (Rx == Ry)
                        transformDiagonalWhole<T, IndexType, 1, 1, 0, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else if (Rx < Ry)
                        transformDiagonalWhole<T, IndexType, 1, 1, 1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                    else
                        transformDiagonalWhole<T, IndexType, 1, 1, -1, manyZ> <<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs,
                                Rx, Ry, zNow);
                }
            }
        } else {
            transformDiagonalCutting<T, IndexType><<<blocks, threadsReal>>>(fromArr_gpu + fromArrOffs, toArr_gpu + toArrOffs, Rx, Ry, zNow, xStart, xStep, xCount,
                    yStart, yStep, yCount, zStep, slash_flag, back_flag, compact_flag);
        }
        zDone += zNow;
    }
}

#endif

void dvmhCudaTransformCutting(CudaDevice *cudaDev, void *fromArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Ry, UDvmType Rz, UDvmType xStart,
        UDvmType xStep, UDvmType xCount, UDvmType yStart, UDvmType yStep, UDvmType yCount, UDvmType zStep, bool back_flag, bool slash_flag, void *toArr_gpu,
        bool compact_flag)
{
#ifdef HAVE_CUDA
    cudaDev->setAsCurrent();
    checkInternal2(toArr_gpu, "Null pointer is not allowed as destination address");
    checkInternal2(fromArr_gpu != toArr_gpu, "Diagonal transformation can not be done 'in place'");
    dvmh_log(TRACE, "Diagonal-transform prev=%p typeSize=" UDTFMT " Rx=" UDTFMT " Ry=" UDTFMT " Rz=" UDTFMT " back=%d slash=%d next=%p", fromArr_gpu, typeSize,
            Rx, Ry, Rz, (int)back_flag, (int)slash_flag, toArr_gpu);

#define COND_TRANSFORM(type, IndexType) \
if (typeSize == sizeof(type) && canTreatAs<type>(fromArr_gpu, toArr_gpu)) \
    transform_array<type, IndexType>(cudaDev, (const type *)fromArr_gpu, (type *)toArr_gpu, \
                    Rx, Ry, Rz, \
                    xStart, xStep, xCount, yStart, yStep, yCount, \
                    zStep, back_flag, slash_flag, compact_flag);

    UDvmType maxIndexInTfm = Rx * Ry * Rz;
    if (maxIndexInTfm <= INT_MAX) {
        COND_TRANSFORM(char, int)
        else COND_TRANSFORM(short, int)
        else COND_TRANSFORM(float, int)
        else COND_TRANSFORM(double, int)
        else COND_TRANSFORM(float3, int)
        else COND_TRANSFORM(double2, int)
        else COND_TRANSFORM(double3, int)
        else COND_TRANSFORM(double4, int)
        else {
            // TODO: Add common case with any typeSize
            checkInternal3(0, "Per-diagonal transformation is not implemented for typeSize=" UDTFMT " yet", typeSize);
        }
    } else {
        COND_TRANSFORM(char, DvmType)
        else COND_TRANSFORM(short, DvmType)
        else COND_TRANSFORM(float, DvmType)
        else COND_TRANSFORM(double, DvmType)
        else COND_TRANSFORM(float3, DvmType)
        else COND_TRANSFORM(double2, DvmType)
        else COND_TRANSFORM(double3, DvmType)
        else COND_TRANSFORM(double4, DvmType)
        else {
            // TODO: Add common case with any typeSize
            checkInternal3(0, "Per-diagonal transformation is not implemented for typeSize=" UDTFMT " yet", typeSize);
        }
    }

#undef COND_TRANSFORM
    if (dvmhSettings.alwaysSync)
        cudaDev->deviceSynchronize();
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

void dvmhCudaTransformArray(CudaDevice *cudaDev,
                            void *fromArr_gpu, UDvmType typeSize,
                            UDvmType Rx, UDvmType Ry, UDvmType Rz,
                            bool back_flag, bool slash_flag,
                            void *toArr_gpu)
{
    dvmhCudaTransformCutting(cudaDev, fromArr_gpu, typeSize, Rx, Ry, Rz, 0, 1, Rx, 0, 1, Ry, 1, back_flag, slash_flag, toArr_gpu);
}

#ifdef HAVE_CUDA

template <typename T, typename SizeType>
__global__ void transposeMatrixFast_comp(const T *inputMatrix, T *outputMatrix,
                                         SizeType dimX, SizeType dimY,
                                         SizeType pX, SizeType pY, SizeType pZ)
{
    __shared__ T temp[BLOCK_DIM][BLOCK_DIM + 1];

    SizeType x1Index = (blockIdx.x + pX) * blockDim.x + threadIdx.x;
    SizeType y1Index = (blockIdx.y + pY) * blockDim.y + threadIdx.y;
    SizeType x2Index = (blockIdx.y + pY) * blockDim.y + threadIdx.x;
    SizeType y2Index = (blockIdx.x + pX) * blockDim.x + threadIdx.y;
    SizeType zIndex = blockIdx.z + pZ;
    SizeType zAdd = zIndex * dimX * dimY;
    SizeType idx1 = x1Index + y1Index * dimX + zAdd;
    SizeType idx2 = x2Index + y2Index * dimY + zAdd;

    if ((x1Index < dimX) && (y1Index < dimY)) {
        temp[threadIdx.y][threadIdx.x] = inputMatrix[idx1];
    }

    __syncthreads();

    if ((x2Index < dimY) && (y2Index < dimX)) {
        outputMatrix[idx2] = temp[threadIdx.x][threadIdx.y];
    }
}

template <typename T, typename SizeType>
__global__ void transposeMatrixFast(const T *inputMatrix, T *outputMatrix,
                                    SizeType dimX, SizeType dimB, SizeType dimY,
                                    SizeType pX, SizeType pY, SizeType pZ)
{
    __shared__ T temp[BLOCK_DIM][BLOCK_DIM + 1];

    SizeType x1Index = (blockIdx.x + pX) * blockDim.x + threadIdx.x;
    SizeType y1Index = (blockIdx.y + pY) * blockDim.y + threadIdx.y;
    SizeType x2Index = (blockIdx.y + pY) * blockDim.y + threadIdx.x;
    SizeType y2Index = (blockIdx.x + pX) * blockDim.x + threadIdx.y;
    SizeType zIndex = blockIdx.z + pZ;
    SizeType zAdd = zIndex * dimX * dimB * dimY;
    SizeType idx1 = x1Index + y1Index * dimX * dimB + zAdd;
    SizeType idx2 = x2Index + y2Index * dimY * dimB + zAdd;

    for (SizeType k = 0; k < dimB; k++) {
        if (k > 0)
            __syncthreads();

        if ((x1Index < dimX) && (y1Index < dimY)) {
            temp[threadIdx.y][threadIdx.x] = inputMatrix[idx1 + k * dimX];
        }

        __syncthreads();

        if ((x2Index < dimY) && (y2Index < dimX)) {
            outputMatrix[idx2 + k * dimY] = temp[threadIdx.x][threadIdx.y];
        }
    }
}

template <typename T, typename SizeType>
__global__ void transposeMatrixFast_eq(T *Matrix, SizeType dim, SizeType pX, SizeType pY, SizeType pZ) {
    __shared__ T temp[BLOCK_DIM][BLOCK_DIM + 1];
    T second;

    SizeType xIndex  = (blockIdx.x + pX) * blockDim.x + threadIdx.x;
    SizeType yIndex  = (blockIdx.y + pY) * blockDim.y + threadIdx.y;
    SizeType xIndex1 = (blockIdx.y + pY) * blockDim.y + threadIdx.x;
    SizeType yIndex1 = (blockIdx.x + pX) * blockDim.x + threadIdx.y;
    SizeType zIndex  = blockIdx.z + pZ;

    SizeType idx  = xIndex + yIndex * dim + zIndex * dim * dim;
    SizeType idx1 = xIndex1 + yIndex1 * dim + zIndex * dim * dim;

    if (blockIdx.x * blockDim.x <= blockIdx.y * blockDim.y) {
        if ((xIndex < dim) && (yIndex < dim))
            temp[threadIdx.y][threadIdx.x] = Matrix[idx];

        if (blockIdx.x * blockDim.x != blockIdx.y * blockDim.y) {
            if ((xIndex1 < dim) && (yIndex1 < dim))
                second = Matrix[idx1];
        }
        __syncthreads();

        if ((xIndex1 < dim) && (yIndex1 < dim)) {
            Matrix[idx1] = temp[threadIdx.x][threadIdx.y];
            temp[threadIdx.x][threadIdx.y] = second;
        }

        if (blockIdx.x * blockDim.x != blockIdx.y * blockDim.y) {
            __syncthreads();
            if ((xIndex < dim) && (yIndex < dim))
                Matrix[idx] = temp[threadIdx.y][threadIdx.x];
        }
    }
}

template <typename T, typename SizeType>
__global__ void transposeMatrixCommon(const char *inputMatrix, char *outputMatrix,
                                      SizeType typeSize, SizeType dimX, SizeType dimB, SizeType dimY)
{
    unsigned vectorSize = blockDim.x;
    SizeType xIndex = (SizeType)blockIdx.x * blockDim.y + threadIdx.y;
    SizeType yIndex = (SizeType)blockIdx.y * blockDim.z + threadIdx.z;
    SizeType zIndex = blockIdx.z;
    SizeType zAdd = zIndex * dimX * dimB * dimY;
    SizeType idx1 = xIndex + yIndex * dimX * dimB + zAdd;
    SizeType idx2 = yIndex + xIndex * dimY * dimB + zAdd;

    if ((xIndex < dimX) && (yIndex < dimY)) {
        for (SizeType k = 0; k < dimB; k++) {
            vectMemcpy<T, SizeType>(outputMatrix + typeSize * (idx2 + k * dimY), inputMatrix + typeSize * (idx1 + k * dimX), typeSize, vectorSize, threadIdx.x);
        }
    }
}

template <typename T, typename IndexType>
static void transpose_array(CudaDevice *cudaDev, const T *fromArr_gpu, T *toArr_gpu, UDvmType Rx, UDvmType Rb, UDvmType Ry, UDvmType Rz) {
    dim3 threads(BLOCK_DIM, BLOCK_DIM);

    UDvmType maxZ = cudaDev->maxGridSize[2];
    UDvmType maxY = cudaDev->maxGridSize[1];
    UDvmType maxX = cudaDev->maxGridSize[0];

    // TODO: create new solution for compilation this thing in Windows  and old version of GCC
#ifdef WIN32_TMP
    if (cudaDev->setupKernel((const void *)&transposeMatrixFast_eq<T, IndexType>))
#endif
    {
        if(!(cudaDev->preferL1)) {
            cudaFuncSetCacheConfig(&transposeMatrixFast_eq<T, IndexType>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transposeMatrixFast_comp<T, IndexType>, cudaFuncCachePreferShared);
            cudaFuncSetCacheConfig(&transposeMatrixFast<T, IndexType>, cudaFuncCachePreferShared);

            dvmh_log(TRACE, "set cache configuration 'cudaFuncCachePreferShared' for 'transposeMatrixFast' kernels");
        }
    }

    bool inPlace = fromArr_gpu == toArr_gpu && Rb == 1 && Rx == Ry;
    // XXX: Not all the cases are checked
    UDvmType totalBlocks[3] = {divUpU(Rx, threads.x), divUpU(Ry, threads.y), Rz};
    UDvmType restZ = totalBlocks[2];
    for (UDvmType zIdx = 0; zIdx < divUpU(totalBlocks[2], maxZ); ++zIdx) {
        dim3 bl;
        if (restZ <= maxZ)
            bl.z = restZ;
        else
            bl.z = maxZ;
        UDvmType restY = totalBlocks[1];
        for (UDvmType yIdx = 0; yIdx < divUpU(totalBlocks[1], maxY); ++yIdx) {
            if (restY <= maxY)
                bl.y = restY;
            else
                bl.y = maxY;
            UDvmType restX = totalBlocks[0];
            for (UDvmType xIdx = 0; xIdx < divUpU(totalBlocks[0], maxX); ++xIdx) {
                if (restX <= maxX)
                    bl.x = restX;
                else
                    bl.x = maxX;

                if (inPlace)
                    transposeMatrixFast_eq<T, IndexType><<<bl, threads>>>(toArr_gpu, Rx, xIdx * maxX, yIdx * maxY, zIdx * maxZ);
                else if (Rb == 1)
                    transposeMatrixFast_comp<T, IndexType><<<bl, threads>>>(fromArr_gpu, toArr_gpu, Rx, Ry, xIdx * maxX, yIdx * maxY, zIdx * maxZ);
                else
                    transposeMatrixFast<T, IndexType><<<bl, threads>>>(fromArr_gpu, toArr_gpu, Rx, Rb, Ry, xIdx * maxX, yIdx * maxY, zIdx * maxZ);
                restX -= bl.x;
            }
            restY -= bl.y;
        }
        restZ -= bl.z;
    }
}

template <typename IndexType>
static void transpose_array(CudaDevice *cudaDev, const char *fromArr_gpu, char *toArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Rb, UDvmType Ry,
        UDvmType Rz) {
    dim3 threads(8, 8, 8);
    dim3 blocks(divUpU(Rx, threads.y), divUpU(Ry, threads.z));
    UDvmType maxZ = cudaDev->maxGridSize[2];
    UDvmType restZ = Rz;
    while (restZ > 0) {
        if (restZ <= maxZ)
            blocks.z = restZ;
        else if (restZ <= maxZ * 2)
            blocks.z = restZ / 2;
        else
            blocks.z = maxZ;
        if (canTreatAs<double>(fromArr_gpu, toArr_gpu, typeSize) && typeSize % sizeof(double) == 0)
            transposeMatrixCommon<double, IndexType><<<blocks, threads>>>(fromArr_gpu, toArr_gpu, typeSize, Rx, Rb, Ry);
        else if (canTreatAs<float>(fromArr_gpu, toArr_gpu, typeSize) && typeSize % sizeof(float) == 0)
            transposeMatrixCommon<float, IndexType><<<blocks, threads>>>(fromArr_gpu, toArr_gpu, typeSize, Rx, Rb, Ry);
        else
            transposeMatrixCommon<char, IndexType><<<blocks, threads>>>(fromArr_gpu, toArr_gpu, typeSize, Rx, Rb, Ry);
        fromArr_gpu += typeSize * Rx * Rb * Ry * blocks.z;
        toArr_gpu += typeSize * Rx * Rb * Ry * blocks.z;
        restZ -= blocks.z;
    }
}
#endif

bool dvmhCudaCanTransposeInplace(CudaDevice *cudaDev, void *addr, UDvmType typeSize, UDvmType Rx, UDvmType Rb, UDvmType Ry, UDvmType Rz) {
#ifdef HAVE_CUDA
    return Rb == 1 && Rx == Ry &&
            ((typeSize == sizeof(char) && canTreatAs<char>(addr)) ||
            (typeSize == sizeof(short) && canTreatAs<short>(addr)) ||
            (typeSize == sizeof(float) && canTreatAs<float>(addr)) ||
            (typeSize == sizeof(double) && canTreatAs<double>(addr)) ||
            (typeSize == sizeof(float3) && canTreatAs<float3>(addr)) ||
            (typeSize == sizeof(double2) && canTreatAs<double2>(addr)) ||
            (typeSize == sizeof(double3) && canTreatAs<double3>(addr)) ||
            (typeSize == sizeof(double4) && canTreatAs<double4>(addr)));
#else
    return false;
#endif
}

void dvmhCudaTransposeArray(CudaDevice *cudaDev, void *fromArr_gpu, UDvmType typeSize, UDvmType Rx, UDvmType Rb, UDvmType Ry, UDvmType Rz, void *toArr_gpu) {
#ifdef HAVE_CUDA
    cudaDev->setAsCurrent();
    checkInternal2(toArr_gpu, "Null pointer is not allowed as destination address");
    if (fromArr_gpu == toArr_gpu)
        checkInternal2(dvmhCudaCanTransposeInplace(cudaDev, toArr_gpu, typeSize, Rx, Rb, Ry, Rz),
                "Dimension transposition can not be done 'in place' for non-square matrices of scalar type");
#define COND_TRANSPOSE(type, IndexType) \
if (typeSize == sizeof(type) && canTreatAs<type>(fromArr_gpu, toArr_gpu)) \
    transpose_array<type, IndexType>(cudaDev, (const type *)fromArr_gpu, (type *)toArr_gpu, Rx, Rb, Ry, Rz);

    UDvmType maxIndexInTfm = Rx * Ry * Rz * Rb;
    if (maxIndexInTfm <= UINT_MAX) {
        COND_TRANSPOSE(char, unsigned)
        else COND_TRANSPOSE(short, unsigned)
        else COND_TRANSPOSE(float, unsigned)
        else COND_TRANSPOSE(double, unsigned)
        else COND_TRANSPOSE(float3, unsigned)
        else COND_TRANSPOSE(double2, unsigned)
        else COND_TRANSPOSE(double3, unsigned)
        else COND_TRANSPOSE(double4, unsigned)
        else
            transpose_array<unsigned>(cudaDev, (const char *)fromArr_gpu, (char *)toArr_gpu, typeSize, Rx, Rb, Ry, Rz);
    } else {
        COND_TRANSPOSE(char, UDvmType)
        else COND_TRANSPOSE(short, UDvmType)
        else COND_TRANSPOSE(float, UDvmType)
        else COND_TRANSPOSE(double, UDvmType)
        else COND_TRANSPOSE(float3, UDvmType)
        else COND_TRANSPOSE(double2, UDvmType)
        else COND_TRANSPOSE(double3, UDvmType)
        else COND_TRANSPOSE(double4, UDvmType)
        else
            transpose_array<UDvmType>(cudaDev, (const char *)fromArr_gpu, (char *)toArr_gpu, typeSize, Rx, Rb, Ry, Rz);
    }
#undef COND_TRANSPOSE
    if (dvmhSettings.alwaysSync)
        cudaDev->deviceSynchronize();
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

#ifdef HAVE_CUDA
// XXX: not checked
template <typename T, typename SizeType>
__global__ void permutateMatrixFast(SizeType totalBlocks, const T *oldMem, T *newMem, int rank, SizeType *sizes, SizeType *accumSizes, int *perm) {
    if (blockIdx.x + (SizeType)gridDim.x * blockIdx.y < totalBlocks) {
        SizeType idx2 = threadIdx.x + blockDim.x * (blockIdx.x + (SizeType)gridDim.x * blockIdx.y);
        SizeType idx1 = 0;
        SizeType tmp = idx2;
        for (int i = rank; i >= 1; i--) {
            int origAxis = perm[i - 1];
            SizeType axisIndex = tmp % sizes[origAxis - 1];
            idx1 += axisIndex * accumSizes[origAxis - 1];
            tmp /= sizes[origAxis - 1];
        }
        newMem[idx2] = oldMem[idx1];
    }
}

// XXX: not checked
template <typename T, typename SizeType>
__global__ void permutateMatrixCommon(SizeType totalBlocks, const char *oldMem, char *newMem,
                                      SizeType typeSize, int rank, SizeType *sizes, SizeType *accumSizes, int *perm)
{
    if (blockIdx.x + (SizeType)gridDim.x * blockIdx.y < totalBlocks) {
        unsigned vectorSize = blockDim.x;
        SizeType idx2 = threadIdx.y + blockDim.y * (blockIdx.x + (SizeType)gridDim.x * blockIdx.y);
        SizeType idx1 = 0;
        SizeType tmp = idx2;
        for (int i = rank; i >= 1; i--) {
            int origAxis = perm[i - 1];
            SizeType axisIndex = tmp % sizes[origAxis - 1];
            idx1 += axisIndex * accumSizes[origAxis - 1];
            tmp /= sizes[origAxis - 1];
        }
        vectMemcpy<T, SizeType>(newMem + typeSize * idx2, oldMem + typeSize * idx1, typeSize, vectorSize, threadIdx.x);
    }
}

template <typename SizeType>
static void permutate_array(CudaDevice *cudaDev, const char *oldMem, char *newMem, SizeType typeSize, int rank, UDvmType sizes[], int perm[], SizeType Rz) {
    SizeType len = 1;
    for (int i = 0; i < rank; i++)
        len *= sizes[i];
    bool useFastFlag = (typeSize == sizeof(float) && canTreatAs<float>(oldMem, newMem)) || (typeSize == sizeof(double) && canTreatAs<double>(oldMem, newMem));
    dim3 threads;
    if (useFastFlag) {
        threads.x = BLOCK_DIM * BLOCK_DIM;
    } else {
        threads.x = 8;
        threads.y = 512 / threads.x;
    }
    dim3 blocks;
    SizeType totalBlocks = divUpU(len, (useFastFlag ? threads.x : threads.y));
    if (totalBlocks <= cudaDev->maxGridSize[0]) {
        blocks.x = totalBlocks;
    } else if (totalBlocks <= (UDvmType)cudaDev->maxGridSize[0] * (UDvmType)cudaDev->maxGridSize[1]) {
        if (cudaDev->maxGridSize[0] < cudaDev->maxGridSize[1]) {
            blocks.x = cudaDev->maxGridSize[0];
            blocks.y = divUpU(totalBlocks, blocks.x);
        } else if (cudaDev->maxGridSize[1] < cudaDev->maxGridSize[0]) {
            blocks.y = cudaDev->maxGridSize[1];
            blocks.x = divUpU(totalBlocks, blocks.y);
        } else {
            blocks.x = sqrt((double)totalBlocks);
            blocks.y = divUpU(totalBlocks, blocks.x);
        }
    } else {
        checkInternal2(0, "Too big array. Can not handle permutation");
    }
    UDvmType maxZ = cudaDev->maxGridSize[2];
    SizeType restZ = Rz;
    SizeType *devSizes;
    SizeType *devAccumSizes;
    int *devPerm;
    devSizes = cudaDev->alloc<SizeType>(rank);
    devAccumSizes = cudaDev->alloc<SizeType>(rank);
    devPerm = cudaDev->alloc<int>(rank);
#ifdef NON_CONST_AUTOS
    SizeType hostSizes[rank];
#else
    SizeType hostSizes[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++)
        hostSizes[i] = sizes[i];
    cudaDev->setValues(devSizes, hostSizes, rank);
    hostSizes[rank - 1] = 1;
    for (int i = rank - 1; i > 0; i--)
        hostSizes[i - 1] = hostSizes[i] * sizes[i];
    cudaDev->setValues(devAccumSizes, hostSizes, rank);
    cudaDev->setValues(devPerm, perm, rank);
    while (restZ > 0) {
        if (restZ <= maxZ)
            blocks.z = restZ;
        else if (restZ <= maxZ * 2)
            blocks.z = restZ / 2;
        else
            blocks.z = maxZ;
        if (useFastFlag && typeSize == sizeof(float)) {
            permutateMatrixFast<<<blocks, threads>>>(totalBlocks, (const float *)oldMem, (float *)newMem, rank, devSizes, devAccumSizes, devPerm);
        } else if (useFastFlag && typeSize == sizeof(double)) {
            permutateMatrixFast<<<blocks, threads>>>(totalBlocks, (const double *)oldMem, (double *)newMem, rank, devSizes, devAccumSizes, devPerm);
        } else if (!useFastFlag) {
            if (canTreatAs<double>(oldMem, newMem, typeSize) && typeSize % sizeof(double) == 0)
                permutateMatrixCommon<double, SizeType><<<blocks, threads>>>(totalBlocks, oldMem, newMem, typeSize, rank, devSizes, devAccumSizes, devPerm);
            else if (canTreatAs<float>(oldMem, newMem, typeSize) && typeSize % sizeof(float) == 0)
                permutateMatrixCommon<float, SizeType><<<blocks, threads>>>(totalBlocks, oldMem, newMem, typeSize, rank, devSizes, devAccumSizes, devPerm);
            else
                permutateMatrixCommon<char, SizeType><<<blocks, threads>>>(totalBlocks, oldMem, newMem, typeSize, rank, devSizes, devAccumSizes, devPerm);
        } else {
            // WTF???
            checkInternal2(0, "Internal inconsistency");
        }
        oldMem += typeSize * len * blocks.z;
        newMem += typeSize * len * blocks.z;
        restZ -= blocks.z;
    }
    cudaDev->dispose(devSizes);
    cudaDev->dispose(devAccumSizes);
    cudaDev->dispose(devPerm);
}
#endif

void dvmhCudaPermutateArray(CudaDevice *cudaDev, void *oldMem, UDvmType typeSize, int rank, UDvmType sizes[], int perm[], UDvmType Rz, void *newMem) {
#ifdef HAVE_CUDA
    cudaDev->setAsCurrent();
    checkInternal2(newMem, "Null pointer is not allowed as destination address");
    checkInternal2(oldMem != newMem, "Dimension permutation can not be done 'in place'");
    UDvmType totalSize = typeSize;
    for (int i = 0; i < rank; i++)
        totalSize *= sizes[i];
    if (totalSize <= UINT_MAX)
        permutate_array<unsigned>(cudaDev, (const char *)oldMem, (char *)newMem, typeSize, rank, sizes, perm, Rz);
    else
        permutate_array<UDvmType>(cudaDev, (const char *)oldMem, (char *)newMem, typeSize, rank, sizes, perm, Rz);
    if (dvmhSettings.alwaysSync)
        cudaDev->deviceSynchronize();
#else
    checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
}

#ifdef HAVE_CUDA
// XXX: Not used
template <typename T, typename baseT, unsigned BL, unsigned numFields, typename SizeType>
__global__ void transform_AoS_to_SoA_equal_type(T *inStruct, baseT *outArrays, SizeType N, bool back) {
    SizeType gIdx = (SizeType)blockIdx.x * blockDim.x + threadIdx.x;

    unsigned numWarp = threadIdx.x / warpSize;
    unsigned locIdx = threadIdx.x % warpSize;
    SizeType idx = sizeof(T) / sizeof(baseT) * ((SizeType)blockIdx.x * blockDim.x + warpSize * numWarp);

    baseT *str = (baseT *)inStruct;
    __shared__ baseT tmp[BL * numFields];

    if (gIdx < N) {
        if (back) {
        #pragma unroll
            for (unsigned i = 0; i < numFields; ++i)
                tmp[numFields * locIdx + i + warpSize * numFields * numWarp] = outArrays[gIdx + i * N];
        #pragma unroll
            for (unsigned i = 0; i < numFields; ++i)
                str[warpSize * i + locIdx + idx] = tmp[warpSize * i + locIdx + warpSize * numFields * numWarp];
        } else {
        #pragma unroll
            for (unsigned i = 0; i < numFields; ++i)
                tmp[warpSize * i + locIdx + warpSize * numFields * numWarp] = str[warpSize * i + locIdx + idx];
        #pragma unroll
            for (unsigned i = 0; i < numFields; ++i)
                outArrays[gIdx + i * N] = tmp[numFields * locIdx + i + warpSize * numFields * numWarp];
        }
    }
}
#endif

}

#pragma GCC visibility pop
