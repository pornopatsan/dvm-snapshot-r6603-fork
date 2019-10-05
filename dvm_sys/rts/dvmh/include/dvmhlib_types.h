#ifndef DVMHLIB_TYPES_H
#define DVMHLIB_TYPES_H

#if defined(_WIN64)
#define __LLP64__ 1
#endif

#if defined(__LLP64__)
typedef long long DvmType;
#else
typedef long DvmType;
#endif

#ifdef __cplusplus
typedef int (*DvmHandlerFunc)(...);
#else
typedef int (*DvmHandlerFunc)();
#endif

#endif
