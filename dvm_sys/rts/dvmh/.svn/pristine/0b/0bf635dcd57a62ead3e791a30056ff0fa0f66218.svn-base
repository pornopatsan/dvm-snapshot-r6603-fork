#pragma once

#include "dvmh_types.h"

namespace libdvmh {

class CommonDevice;

bool dvmhCudaCanDirectCopy(int rank, UDvmType typeSize, CommonDevice *dev1, const DvmType header1[], CommonDevice *dev2, const DvmType header2[]);

bool dvmhCudaDirectCopy(int rank, UDvmType typeSize, CommonDevice *dev1, const DvmType header1[], CommonDevice *dev2, const DvmType header2[],
        const Interval cutting[]);

}
