// Place for supplementary __device__ function definitions
#include "dvmhlib_f2c.h"
#include "dvmhlib_block_red.h"

template<typename IndexT, int slash, int cmp_X_Y>
__forceinline__ __device__ void dvmh_convert_XY(const IndexT x, const IndexT y, const IndexT Rx, const IndexT Ry, IndexT &idx) {
    if (slash == 0) {
        if (cmp_X_Y == 0) {
            if (x + y < Rx)
                idx = y + (1 + x + y)*(x + y) / 2;
            else
                idx = Rx*(Rx - 1) + x - (2 * Rx - x - y - 1)*(2 * Rx - x - y - 2) / 2;
        } else if (cmp_X_Y == 1) {
            if (x + y < Rx)
                idx = y + ((1 + x + y)*(x + y)) / 2;
            else if (x + y < Ry)
                idx = ((1 + Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x + y - Rx);
            else
                idx = Rx*Ry - Ry + y - (((Rx + Ry - y - x - 1)* (Rx + Ry - y - x - 2)) / 2);
        } else {
            if (x + y < Ry)
                idx = x + (1 + x + y)*(x + y) / 2;
            else if (x + y < Rx)
                idx = (1 + Ry)*Ry / 2 + (Ry - y - 1) + Ry * (x + y - Ry);
            else
                idx = Rx*Ry - Rx + x - ((Rx + Ry - y - x - 1)* (Rx + Ry - y - x - 2) / 2);
        }
    } else {
        if (cmp_X_Y == 0) {
            if (x + Rx - 1 - y < Rx)
                idx = Rx - 1 - y + (x + Rx - y)*(x + Rx - 1 - y) / 2;
            else
                idx = Rx*(Rx - 1) + x - (Rx - x + y)*(Rx - x + y - 1) / 2;
        } else if (cmp_X_Y == 1) {
            if (x + Ry - 1 - y < Rx)
                idx = Ry - 1 - y + ((x + Ry - y)*(x + Ry - 1 - y)) / 2;
            else if (x + Ry - 1 - y < Ry)
                idx = ((1 + Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x + Ry - 1 - y - Rx);
            else
                idx = Rx*Ry - 1 - y - (((Rx + y - x)* (Rx + y - x - 1)) / 2);
        } else {
            if (x + Ry - 1 - y < Ry)
                idx = x + (1 + x + Ry - 1 - y)*(x + Ry - 1 - y) / 2;
            else if (x + Ry - 1 - y < Rx)
                idx = (1 + Ry)*Ry / 2 + y + Ry * (x - y - 1);
            else
                idx = Rx*Ry - Rx + x - ((Rx + y - x)* (Rx + y - x - 1) / 2);
        }
    }
}


template<typename IndexT>
__inline__ __device__ void dvmh_convert_XY(const IndexT x, const IndexT y, const IndexT Rx, const IndexT Ry, const int slash, IndexT &idx) {
    if (!slash) {
        if (Rx == Ry) {
            if (x + y < Rx)
                idx = y + (1 + x + y)*(x + y) / 2;
            else
                idx = Rx*(Rx - 1) + x - (2 * Rx - x - y - 1)*(2 * Rx - x - y - 2) / 2;
        } else if (Rx < Ry) {
            if (x + y < Rx)
                idx = y + ((1 + x + y)*(x + y)) / 2;
            else if (x + y < Ry)
                idx = ((1 + Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x + y - Rx);
            else
                idx = Rx*Ry - Ry + y - (((Rx + Ry - y - x - 1)* (Rx + Ry - y - x - 2)) / 2);
        } else {
            if (x + y < Ry)
                idx = x + (1 + x + y)*(x + y) / 2;
            else if (x + y < Rx)
                idx = (1 + Ry)*Ry / 2 + (Ry - y - 1) + Ry * (x + y - Ry);
            else
                idx = Rx*Ry - Rx + x - ((Rx + Ry - y - x - 1)* (Rx + Ry - y - x - 2) / 2);
        }
    } else {
        if (Rx == Ry) {
            if (x + Rx - 1 - y < Rx)
                idx = Rx - 1 - y + (x + Rx - y)*(x + Rx - 1 - y) / 2;
            else
                idx = Rx*(Rx - 1) + x - (Rx - x + y)*(Rx - x + y - 1) / 2;
        } else if (Rx < Ry) {
            if (x + Ry - 1 - y < Rx)
                idx = Ry - 1 - y + ((x + Ry - y)*(x + Ry - 1 - y)) / 2;
            else if (x + Ry - 1 - y < Ry)
                idx = ((1 + Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x + Ry - 1 - y - Rx);
            else
                idx = Rx*Ry - 1 - y - (((Rx + y - x)* (Rx + y - x - 1)) / 2);
        } else {
            if (x + Ry - 1 - y < Ry)
                idx = x + (1 + x + Ry - 1 - y)*(x + Ry - 1 - y) / 2;
            else if (x + Ry - 1 - y < Rx)
                idx = (1 + Ry)*Ry / 2 + y + Ry * (x - y - 1);
            else
                idx = Rx*Ry - Rx + x - ((Rx + y - x)* (Rx + y - x - 1) / 2);
        }
    }
}
