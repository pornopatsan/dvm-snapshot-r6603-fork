#pragma once

#include "dvmh_types.h"

namespace libdvmh {

class CudaDevice;

void dvmhCudaReplicate(CudaDevice *cudaDev, void *addr, UDvmType recordSize, UDvmType quantity, void *devPtr);

#define DECLARE_REDFUNC(func, type) \
void dvmhCudaRed_##func##_##type(CudaDevice *cudaDev, UDvmType items, type *vars, UDvmType length, char *locs, UDvmType locSize, type init_val, type *res, \
        char *locRes)

DECLARE_REDFUNC(sum, char);
DECLARE_REDFUNC(prod, char);
DECLARE_REDFUNC(max, char);
DECLARE_REDFUNC(min, char);
DECLARE_REDFUNC(and, char);
DECLARE_REDFUNC(or, char);
DECLARE_REDFUNC(neq, char);
DECLARE_REDFUNC(eq, char);

DECLARE_REDFUNC(sum, int);
DECLARE_REDFUNC(prod, int);
DECLARE_REDFUNC(max, int);
DECLARE_REDFUNC(min, int);
DECLARE_REDFUNC(and, int);
DECLARE_REDFUNC(or, int);
DECLARE_REDFUNC(neq, int);
DECLARE_REDFUNC(eq, int);

DECLARE_REDFUNC(sum, long);
DECLARE_REDFUNC(prod, long);
DECLARE_REDFUNC(max, long);
DECLARE_REDFUNC(min, long);
DECLARE_REDFUNC(and, long);
DECLARE_REDFUNC(or, long);
DECLARE_REDFUNC(neq, long);
DECLARE_REDFUNC(eq, long);

DECLARE_REDFUNC(sum, long_long);
DECLARE_REDFUNC(prod, long_long);
DECLARE_REDFUNC(max, long_long);
DECLARE_REDFUNC(min, long_long);
DECLARE_REDFUNC(and, long_long);
DECLARE_REDFUNC(or, long_long);
DECLARE_REDFUNC(neq, long_long);
DECLARE_REDFUNC(eq, long_long);

DECLARE_REDFUNC(sum, float);
DECLARE_REDFUNC(prod, float);
DECLARE_REDFUNC(max, float);
DECLARE_REDFUNC(min, float);

DECLARE_REDFUNC(sum, double);
DECLARE_REDFUNC(prod, double);
DECLARE_REDFUNC(max, double);
DECLARE_REDFUNC(min, double);

DECLARE_REDFUNC(sum, float_complex);
DECLARE_REDFUNC(prod, float_complex);

DECLARE_REDFUNC(sum, double_complex);
DECLARE_REDFUNC(prod, double_complex);

}
