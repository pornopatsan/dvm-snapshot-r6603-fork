#pragma once

namespace libdvmh {

#ifdef HAVE_CUDA

template <typename T>
inline bool canTreatAs(const void *addr) {
    return UDvmType(addr) % sizeof(T) == 0;
}

template <>
inline bool canTreatAs<float3>(const void *addr) {
    return UDvmType(addr) % sizeof(float) == 0;
}

template <>
inline bool canTreatAs<double3>(const void *addr) {
    return UDvmType(addr) % sizeof(double) == 0;
}

template <>
inline bool canTreatAs<double4>(const void *addr) {
    return UDvmType(addr) % (2 * sizeof(double)) == 0;
}

template <typename T>
inline bool canTreatAs(const void *addr1, const void *addr2, UDvmType regOffs = 0) {
    return canTreatAs<T>(addr1) && canTreatAs<T>(addr2) && canTreatAs<T>((const char *)addr1 + regOffs);
}

template <typename T, typename SizeType>
inline __device__ void vectMemcpy(char *dest, const char *src, SizeType len, unsigned vectorSize, unsigned vectorIndex) {
    SizeType copied = 0;
    unsigned atOnce = sizeof(T) * vectorSize;
    while (copied + atOnce <= len) {
        ((T *)(dest + copied))[vectorIndex] = ((T *)(src + copied))[vectorIndex];
        copied += atOnce;
    }
    atOnce = (len - copied) / sizeof(T);
    if (vectorIndex < atOnce)
        ((T *)(dest + copied))[vectorIndex] = ((T *)(src + copied))[vectorIndex];
    copied += atOnce * sizeof(T);
    if (vectorIndex == 0) {
        while (copied < len) {
            dest[copied + 1] = src[copied + 1];
            copied++;
        }
    }
}

#endif

}
