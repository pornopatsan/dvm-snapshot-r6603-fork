#include "dvmh_copying.h"

#include <cstring>

#ifdef HAVE_CUDA
#pragma GCC visibility push(default)
#include <cuda_runtime.h>
#pragma GCC visibility pop
#endif

#include "cuda_copying.h"
#include "dvmh_buffer.h"
#include "dvmh_device.h"
#include "dvmh_log.h"
#include "dvmh_stat.h"
#include "util.h"

namespace libdvmh {

class HostCopier {
public:
    void memcpy1D(void *dst, const void *src, UDvmType siz) {
        DvmhTimer tm(needToCollectTimes);
        memcpy(dst, src, siz);
        if (needToCollectTimes) {
            double timeNow = tm.total();
            copyTime += timeNow;
            copyAmount += siz;
            dvmh_log(TRACE, "CPU copy time now = %g. Overall = %g. Overall amount = " UDTFMT, timeNow, copyTime, copyAmount);
        }
    }
    void memcpy2D(void *dst, UDvmType dst_pitch, const void *src, UDvmType src_pitch, UDvmType width, UDvmType height) {
        DvmhTimer tm(needToCollectTimes);
        for (UDvmType i = 0; i < height; i++)
            memcpy((char *)dst + i * dst_pitch, (const char *)src + i * src_pitch, width);
        if (needToCollectTimes) {
            double timeNow = tm.total();
            copyTime += timeNow;
            copyAmount += width * height;
            dvmh_log(TRACE, "CPU copy time now = %g. Overall = %g. Overall amount = " UDTFMT, timeNow, copyTime, copyAmount);
        }
    }
protected:
    static double copyTime;
    static UDvmType copyAmount;
};

double HostCopier::copyTime = 0;
UDvmType HostCopier::copyAmount = 0;

#ifdef HAVE_CUDA

class CudaCopier {
public:
    CudaCopier(CommonDevice *aDev1, CommonDevice *aDev2) {
        dev1 = (aDev1->getType() == dtCuda ? (CudaDevice *)aDev1 : 0);
        dev2 = (aDev2->getType() == dtCuda ? (CudaDevice *)aDev2 : 0);
        assert(dev1 || dev2);
        if (dev1)
            dev1->setAsCurrent();
        else
            dev2->setAsCurrent();
    }
public:
    void memcpy1D(void *dst, const void *src, UDvmType siz) {
        double startT = (needToCollectTimes ? getTime() : 0);
        if (dev1 && dev2) {
            if (dev1->index != dev2->index) {
                // Perform peer access
                dvmh_log(TRACE, "cudaMemcpyPeer");
                checkInternalCuda(cudaMemcpyPeer(dst, dev2->index, src, dev1->index, siz));
            } else {
                // Perform copy inside one device
                dvmh_log(TRACE, "cudaMemcpyDeviceToDevice");
                checkInternalCuda(cudaMemcpy(dst, src, siz, cudaMemcpyDeviceToDevice));
            }
        } else if (dev1) {
            // Perform copy from device to host
            dvmh_log(TRACE, "cudaMemcpyDeviceToHost");
            checkInternalCuda(cudaMemcpy(dst, src, siz, cudaMemcpyDeviceToHost));
        } else {
            // Perform copy from host to device
            dvmh_log(TRACE, "cudaMemcpyHostToDevice");
            checkInternalCuda(cudaMemcpy(dst, src, siz, cudaMemcpyHostToDevice));
        }
        if (needToCollectTimes) {
            double timeNow = getTime() - startT;
            copyTime += timeNow;
            copyAmount += siz;
            dvmh_log(TRACE, "GPU copy time now = %g. Overall = %g. Overall amount = " UDTFMT, timeNow, copyTime, copyAmount);
        }
    }
    void memcpy2D(void *dst, UDvmType dst_pitch, const void *src, UDvmType src_pitch, UDvmType width, UDvmType height) {
        bool real2D = !(dev1 && dev2 && dev1->index != dev2->index); // There is no MemcpyPeer2D function
        if (!real2D) {
            for (UDvmType i = 0; i < height; i++)
                memcpy1D((char *)dst + i * dst_pitch, (const char *)src + i * src_pitch, width);
        } else {
            double startT = (needToCollectTimes ? getTime() : 0);
            if (dev1 && dev2) {
                // Perform copy from device to device
                dvmh_log(TRACE, "cudaMemcpyDeviceToDevice2D");
                checkInternalCuda(cudaMemcpy2D(dst, dst_pitch, src, src_pitch, width, height, cudaMemcpyDeviceToDevice));
            } else if (dev1) {
                // Perform copy from device to host
                dvmh_log(TRACE, "cudaMemcpyDeviceToHost2D");
                checkInternalCuda(cudaMemcpy2D(dst, dst_pitch, src, src_pitch, width, height, cudaMemcpyDeviceToHost));
            } else {
                // Perform copy from host to device
                dvmh_log(TRACE, "cudaMemcpyHostToDevice2D");
                checkInternalCuda(cudaMemcpy2D(dst, dst_pitch, src, src_pitch, width, height, cudaMemcpyHostToDevice));
            }
            if (needToCollectTimes) {
                double timeNow = getTime() - startT;
                copyTime += timeNow;
                copyAmount += width * height;
                dvmh_log(TRACE, "GPU copy time now = %g. Overall = %g. Overall amount = " UDTFMT, timeNow, copyTime, copyAmount);
            }
        }
    }
protected:
    double getTime() {
        if (dev1)
            dev1->deviceSynchronize();
        if (dev2)
            dev2->deviceSynchronize();
        return dvmhTime();
    }
protected:
    CudaDevice *dev1, *dev2;
    static double copyTime;
    static UDvmType copyAmount;
};

double CudaCopier::copyTime = 0;
UDvmType CudaCopier::copyAmount = 0;

#endif

template <typename Copier>
static void defaultCopyCuttingInternal(int rank, UDvmType typeSize, const int axisPerm1[], const DvmType header1[], const int axisPerm2[],
        const DvmType header2[], Interval cutting[], int matched, Copier &copier) {
    int goodFirstNotMatched = (rank > 0 && matched < rank && axisPerm1[rank - matched - 1] == axisPerm2[rank - matched - 1]);
    int weaklyMatched = matched + goodFirstNotMatched;
    int notMatched = rank - weaklyMatched;
    // goodFirstNotMatched = 1, if we can consider matched + 1 dimensions as 1 piece, but it is not consequent with another such piece
    // (in contrary with matched dimensions)
    int effectiveRank = 0;
    for (int i = 1; i <= notMatched; i++)
        if (cutting[axisPerm1[i - 1] - 1].size() > 1)
            effectiveRank++;
    assert(effectiveRank <= notMatched);
    dvmh_log(TRACE, "effective rank = %d, matched = %d, weakly matched = %d, not matched = %d", effectiveRank, matched, weaklyMatched, notMatched);
    if (effectiveRank == 0) {
        // YAHOO! We can copy it by one piece
        char *address1 = (char *)header1[rank + 2] + typeSize * header1[rank + 1];
        char *address2 = (char *)header2[rank + 2] + typeSize * header2[rank + 1];
        UDvmType elemCount = 1;
        for (int i = 1; i <= rank; i++) {
            address1 += typeSize * header1[i - 1 + 1] * cutting[i - 1][0];
            address2 += typeSize * header2[i - 1 + 1] * cutting[i - 1][0];
            elemCount *= cutting[i - 1].size();
        }

        dvmh_log(TRACE, "copying " UDTFMT " bytes (" UDTFMT " elements) address1=%p address2=%p", elemCount * typeSize, elemCount, address1, address2);
        copier.memcpy1D(address2, address1, elemCount * typeSize);
    } else if (effectiveRank == 1) {
        // YAHOO! We can copy it by one 2D command
        char *address1 = (char *)header1[rank + 2] + typeSize * header1[rank + 1];
        char *address2 = (char *)header2[rank + 2] + typeSize * header2[rank + 1];
        for (int i = 1; i <= rank; i++) {
            address1 += typeSize * header1[i - 1 + 1] * cutting[i - 1][0];
            address2 += typeSize * header2[i - 1 + 1] * cutting[i - 1][0];
        }

        UDvmType width = typeSize;
        for (int i = notMatched + 1; i <= rank; i++) {
            assert(axisPerm1[i - 1] == axisPerm2[i - 1]);
            width *= cutting[axisPerm1[i - 1] - 1].size();
        }
        int origAxis, axis1 = -1, axis2 = -1;
        for (int i = 1; i <= notMatched; i++) {
            if (cutting[axisPerm1[i - 1] - 1][0] < cutting[axisPerm1[i - 1] - 1][1])
                axis1 = i;
            if (cutting[axisPerm2[i - 1] - 1][0] < cutting[axisPerm2[i - 1] - 1][1])
                axis2 = i;
        }
        assert(axis1 >= 1 && axis2 >= 1 && axisPerm1[axis1 - 1] == axisPerm2[axis2 - 1]);
        origAxis = axisPerm1[axis1 - 1];
        UDvmType height = cutting[origAxis - 1].size();
        UDvmType pitch1 = header1[origAxis - 1 + 1] * typeSize;
        UDvmType pitch2 = header2[origAxis - 1 + 1] * typeSize;

        dvmh_log(TRACE, "copying " UDTFMT " bytes (2D, pitch1=" UDTFMT " pitch2=" UDTFMT " width=" UDTFMT " height=" UDTFMT ") address1=%p address2=%p",
                height * width, pitch1, pitch2, width, height, address1, address2);
        copier.memcpy2D(address2, pitch2, address1, pitch1, width, height);
    } else {
        // Oh no, we can not copy it by one command... :(
        // Recursively reducing one dimension
        int bestAxis = -1;
        UDvmType bestSize = 0;
        for (int i = 1; i <= notMatched; i++) {
            int origAxis = axisPerm1[i - 1];
            UDvmType curSize = cutting[origAxis - 1].size();
            if (curSize > 1 && (bestSize <= 0 || bestSize > curSize)) {
                bestAxis = origAxis;
                bestSize = curSize;
            }
        }
        assert(bestAxis > 0 && bestSize > 1);
        Interval sav;
        sav = cutting[bestAxis - 1];
        for (DvmType curVal = sav[0]; curVal <= sav[1]; curVal++) {
            cutting[bestAxis - 1][0] = curVal;
            cutting[bestAxis - 1][1] = curVal;
            defaultCopyCuttingInternal(rank, typeSize, axisPerm1, header1, axisPerm2, header2, cutting, matched, copier);
        }
        cutting[bestAxis - 1] = sav;
    }
}

static int getMatched(int rank, const int axisPerm1[], const DvmType header1[], const int axisPerm2[], const DvmType header2[], const Interval cutting[]) {
    int matched = 0;
    UDvmType prevSize = 1;
    for (int i = rank; i > 1; i--) {
        // i - number of dimension in transformed state
        // axisPerm[i - 1] - number of dimension in original state
        int origAxis1 = axisPerm1[i - 1];
        int origAxis2 = axisPerm2[i - 1];
        UDvmType curElements = cutting[origAxis1 - 1].size();
        if (origAxis1 == origAxis2 && header1[origAxis1 - 1] == header2[origAxis2 - 1] && (UDvmType)header1[origAxis1 - 1] == prevSize * curElements)
            matched++;
        else
            break;
        prevSize *= curElements;
    }
    return matched;
}

static void dvmhCopyInternal(int rank, UDvmType typeSize, CommonDevice *dev1, const int axisPerm1[], const DvmType header1[], CommonDevice *dev2,
        const int axisPerm2[], const DvmType header2[], Interval cutting[]) {
    if (dev1->getType() == dtHost && dev2->getType() == dtHost) {
        HostCopier copier;
        defaultCopyCuttingInternal(rank, typeSize, axisPerm1, header1, axisPerm2, header2, cutting,
                getMatched(rank, axisPerm1, header1, axisPerm2, header2, cutting), copier);
    } else if (dev1->getType() == dtCuda || dev2->getType() == dtCuda) {
#ifdef HAVE_CUDA
        if (!dvmhSettings.noDirectCopying && dvmhCudaCanDirectCopy(rank, typeSize, dev1, header1, dev2, header2)) {
            bool res = dvmhCudaDirectCopy(rank, typeSize, dev1, header1, dev2, header2, cutting);
            checkInternal(res);
        } else {
            CudaCopier copier(dev1, dev2);
            defaultCopyCuttingInternal(rank, typeSize, axisPerm1, header1, axisPerm2, header2, cutting,
                    getMatched(rank, axisPerm1, header1, axisPerm2, header2, cutting), copier);
        }
#else
        checkInternal2(0, "RTS is compiled without support for CUDA");
#endif
    }
}

static void dvmhCopy(int rank, UDvmType typeSize, int dev1, const int axisPerm1[], const DvmType header1[], int dev2, const int axisPerm2[],
        const DvmType header2[], Interval cutting[]) {
#ifdef NON_CONST_AUTOS
    int identityAxisPerm[rank];
#else
    int identityAxisPerm[MAX_ARRAY_RANK];
#endif
    for (int i = 0; i < rank; i++)
        identityAxisPerm[i] = i + 1;
    dvmhCopyInternal(rank, typeSize, devices[dev1], (axisPerm1 ? axisPerm1 : identityAxisPerm), header1, devices[dev2],
            (axisPerm2 ? axisPerm2 : identityAxisPerm), header2, cutting);
}

void dvmhCopy(const DvmhBuffer *from, DvmhBuffer *to, const Interval aCutting[]) {
    int dev1 = from->getDeviceNum();
    int dev2 = to->getDeviceNum();
    int rank = from->getRank();
    UDvmType typeSize = from->getTypeSize();
    UDvmType totalSize = typeSize * aCutting->blockSize(rank);
    bool hasCuda = devices[dev1]->getType() == dtCuda || devices[dev2]->getType() == dtCuda;
    if (hasCuda) {
        if (devices[dev1]->getType() == dtCuda)
            ((CudaDevice *)devices[dev1])->deviceSynchronize();
        if (devices[dev2]->getType() == dtCuda)
            ((CudaDevice *)devices[dev2])->deviceSynchronize();
    }
    DvmhTimer tim(hasCuda);
    if ((devices[dev1]->getType() == dtCuda || devices[dev1]->getType() == dtHost) && (devices[dev2]->getType() == dtCuda || devices[dev2]->getType() ==
            dtHost)) {
#ifdef NON_CONST_AUTOS
        DvmType header1[rank + 3], header2[rank + 3];
        Interval cutting[rank];
#else
        DvmType header1[MAX_ARRAY_RANK + 3], header2[MAX_ARRAY_RANK + 3];
        Interval cutting[MAX_ARRAY_RANK];
#endif
        cutting->blockAssign(rank, aCutting);

        const int *axisPerm1 = from->getAxisPerm();
        const int *axisPerm2 = to->getAxisPerm();
        bool noUndiag = !from->isDiagonalized() && !to->isDiagonalized();
        if (from->isDiagonalized() && from->getDiagonalizedState() == to->getDiagonalizedState() &&
                axisPerm1[rank - 1] == axisPerm2[rank - 1] && axisPerm1[rank - 2] == axisPerm2[rank - 2]) {
            int origX = axisPerm1[rank - 1];
            int origY = axisPerm1[rank - 2];
            if (from->getHavePortion()[origX - 1] == to->getHavePortion()[origX - 1] &&
                    from->getHavePortion()[origY - 1] == to->getHavePortion()[origY - 1]) {
                UDvmType Rx = from->getHavePortion()[origX - 1].size();
                UDvmType Ry = from->getHavePortion()[origY - 1].size();
                UDvmType cuttingSize = cutting[origX - 1].size() * cutting[origY - 1].size();
                if (cuttingSize == 1) {
                    DvmType coordX = cutting[origX - 1][0] - from->getHavePortion()[origX - 1][0];
                    DvmType coordY = cutting[origY - 1][0] - from->getHavePortion()[origY - 1][0];
                    DvmType diagonalIdx = dvmhXYToDiagonal(coordX, coordY, Rx, Ry, from->getDiagonalizedState() == 2);
                    coordX = (diagonalIdx % Rx) + from->getHavePortion()[origX - 1][0];
                    coordY = (diagonalIdx / Rx) + from->getHavePortion()[origY - 1][0];
                    cutting[origX - 1][0] = coordX;
                    cutting[origX - 1][1] = coordX;
                    cutting[origY - 1][0] = coordY;
                    cutting[origY - 1][1] = coordY;
                    noUndiag = true;
                } else if (cuttingSize == from->getHavePortion()[origX - 1].size() * from->getHavePortion()[origY - 1].size()) {
                    noUndiag = true;
                }
            }
        }
        if (noUndiag) {
            from->fillHeader(from->getDeviceAddr(), header1, false, 0, false);
            to->fillHeader(to->getDeviceAddr(), header2, false, 0, false);
            dvmhCopy(rank, typeSize, dev1, axisPerm1, header1, dev2, axisPerm2, header2, cutting);
        } else {
            bool diag1 = from->isDiagonalized();
            bool diag2 = to->isDiagonalized();
            DvmhBuffer *undiaged1 = 0, *undiaged2 = 0;
            if (diag1) {
                undiaged1 = from->undiagCutting(cutting);
                undiaged1->fillHeader(header1, false);
            } else {
                from->fillHeader(header1, false);
            }
            if (diag2) {
                undiaged2 = new DvmhBuffer(rank, typeSize, dev2, cutting);
                undiaged2->setTransformState(axisPerm2, 0);
                undiaged2->fillHeader(header2, false);
            } else {
                to->fillHeader(header2, false);
            }
            dvmhCopy(rank, typeSize, dev1, axisPerm1, header1, dev2, axisPerm2, header2, cutting);
            if (diag1)
                delete undiaged1;
            if (diag2) {
                to->replaceUndiagedCutting(undiaged2);
                delete undiaged2;
            }
        }
    } else {
        checkInternal2(0, "Copying for non-CUDA accelerators is not implemented yet");
    }
    if (hasCuda) {
        if (devices[dev1]->getType() == dtCuda)
            ((CudaDevice *)devices[dev1])->deviceSynchronize();
        if (devices[dev2]->getType() == dtCuda)
            ((CudaDevice *)devices[dev2])->deviceSynchronize();
        double tm = tim.total();
        double ptm = (DvmhCopyingPurpose::isCurrentProductive() ? tm : 0.0);
        double ltm = tm - ptm;
        if (devices[dev1]->getType() == dtCuda && devices[dev2]->getType() == dtCuda) {
            dvmh_stat_add_measurement(((CudaDevice *)devices[dev1])->index, DvmhCopyingPurpose::applyCurrent(DVMH_STAT_METRIC_CPY_DTOD), totalSize, ptm / 2.0,
                    ltm / 2.0);
            dvmh_stat_add_measurement(((CudaDevice *)devices[dev2])->index, DvmhCopyingPurpose::applyCurrent(DVMH_STAT_METRIC_CPY_DTOD), totalSize, ptm / 2.0,
                    ltm / 2.0);
        } else if (devices[dev1]->getType() == dtCuda) {
            dvmh_stat_add_measurement(((CudaDevice *)devices[dev1])->index, DvmhCopyingPurpose::applyCurrent(DVMH_STAT_METRIC_CPY_DTOH), totalSize, ptm, ltm);
        } else if (devices[dev2]->getType() == dtCuda) {
            dvmh_stat_add_measurement(((CudaDevice *)devices[dev2])->index, DvmhCopyingPurpose::applyCurrent(DVMH_STAT_METRIC_CPY_HTOD), totalSize, ptm, ltm);
        }
    }
}

}
