#pragma once

#include "dvmh_types.h"

namespace libdvmh {

void prepareLoopDistributionCaches();
template <typename IndexT>
bool dvmhCudaGetDistribution(int deviceNum, int rank, const LoopBounds loopBounds[], int threads[3], UDvmType *pResBlocks, IndexT **pBlocksAddress);
void clearLoopDistributionCaches();

}
