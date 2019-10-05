#ifndef DVMHLIB_CONST_H
#define DVMHLIB_CONST_H

#define LIBDVMH_VERSION "1.0"
#define LIBDVMH_VERSION_MAJOR 1
#define LIBDVMH_VERSION_MINOR 0

#define INITFLAG_FORTRAN 1
#define INITFLAG_NOH 2
#define INITFLAG_SEQUENTIAL 4
#define INITFLAG_OPENMP 8
#define INITFLAG_DEBUG 16

#define rt_UNKNOWN (-1)
#define rt_CHAR 0
#define rt_INT 1
#define rt_LONG 2
#define rt_FLOAT 3
#define rt_DOUBLE 4
#define rt_FLOAT_COMPLEX 5
#define rt_DOUBLE_COMPLEX 6
#define rt_LOGICAL 7
#define rt_LLONG 8
#define rt_UCHAR 9
#define rt_UINT 10
#define rt_ULONG 11
#define rt_ULLONG 12
#define rt_SHORT 13
#define rt_USHORT 14
#define rt_POINTER 15

#define REGION_ASYNC 1
#define REGION_COMPARE_DEBUG 2

#define INTENT_IN 1
#define INTENT_OUT 2
#define INTENT_LOCAL 4
#define INTENT_INOUT 3
#define INTENT_INLOCAL 5
#define INTENT_USE 8

#define UNDEF_BOUND (-2147483648LL)

#define DEVICE_TYPE_HOST 1
#define DEVICE_TYPE_CUDA 2

#define HANDLER_TYPE_PARALLEL 1
#define HANDLER_TYPE_MASTER 2

#define rf_SUM 1
#define rf_PROD 2
#define rf_MULT 2
#define rf_MAX 3
#define rf_MIN 4
#define rf_AND 5
#define rf_OR 6
#define rf_XOR 7
#define rf_EQU 8
#define rf_NE 9
#define rf_EQ 10
#define rf_MAXLOC 11
#define rf_MINLOC 12

#define FORTRAN_CUDA 0
#define C_CUDA 1
#define UNKNOWN_CUDA -1

#define CUDA_MAX_GRID_X 1
#define CUDA_MAX_GRID_Y 2
#define CUDA_MAX_GRID_Z 3

#endif
