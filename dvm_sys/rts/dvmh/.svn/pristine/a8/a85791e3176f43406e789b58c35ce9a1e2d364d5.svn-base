/**
 * This file enables RTS functionality for gathering of the DVMH statistics.
 *
 * @link ../include/dvmh_rts_stat.h statistics functions
 * @link ../include/dvmh_events.h   events definitions
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#pragma once

#include "dvmh_types.h"

#ifndef NO_DVM
#pragma GCC visibility push(default)
#include <dvmh_rts_stat.h>
#include <dvmh_stat.h>
#pragma GCC visibility pop
#else
#define dvmh_stat_add_measurement(...) ((void)0)
#define dvmh_stat_set_gpu_info(...) ((void)0)
#endif

#include "util.h"

/** Functions:
 *      - dvmh_stat_add_measurement(gpuNo, metric, value, timeProductive, timeLost) добавить измерение некоторой метрики
 *      - dvmh_stat_set_gpu_info(gpuNo, id, name) установить информацию о GPU
 **/

namespace libdvmh {

class DvmhCopyingPurpose {
public:
    enum Value {dcpNone, dcpShadow, dcpRemote, dcpRedistribute, dcpInRegion, dcpGetActual, dcpConsistent, dcpArrayCopy};
public:
    static Value getCurrent() { return currentVal; }
    static bool isCurrentProductive() { return currentVal == dcpInRegion || currentVal == dcpGetActual; }
public:
    static Value setCurrent(Value newVal, bool isSoft = false);
#ifndef NO_DVM
    static dvmh_stat_metric_names applyCurrent(dvmh_stat_metric_names defaultMetric);
#endif
protected:
    static THREAD_LOCAL Value currentVal;
};

class PushCurrentPurpose: private Uncopyable {
public:
    explicit PushCurrentPurpose(DvmhCopyingPurpose::Value newVal, bool isSoft = false) {
        prev = DvmhCopyingPurpose::setCurrent(newVal, isSoft);
    }
public:
    ~PushCurrentPurpose() {
        DvmhCopyingPurpose::setCurrent(prev);
    }
protected:
    DvmhCopyingPurpose::Value prev;
};

}
