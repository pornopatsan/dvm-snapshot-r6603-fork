#include "dvmh_stat.h"

namespace libdvmh {

THREAD_LOCAL DvmhCopyingPurpose::Value DvmhCopyingPurpose::currentVal = dcpNone;

DvmhCopyingPurpose::Value DvmhCopyingPurpose::setCurrent(Value newVal, bool isSoft) {
    Value prev = currentVal;
    if (!isSoft || (newVal != dcpNone && prev == dcpNone))
        currentVal = newVal;
    return prev;
}

#ifndef NO_DVM
dvmh_stat_metric_names DvmhCopyingPurpose::applyCurrent(dvmh_stat_metric_names defaultMetric) {
    dvmh_stat_metric_names reference;
    switch (currentVal) {
        case dcpShadow:
            reference = DVMH_STAT_METRIC_CPY_SHADOW_DTOH;
            break;
        case dcpRemote:
            reference = DVMH_STAT_METRIC_CPY_REMOTE_DTOH;
            break;
        case dcpRedistribute:
            reference = DVMH_STAT_METRIC_CPY_REDIST_DTOH;
            break;
        case dcpInRegion:
            reference = DVMH_STAT_METRIC_CPY_IN_REG_DTOH;
            break;
        case dcpGetActual:
            reference = DVMH_STAT_METRIC_CPY_GET_ACTUAL;
            break;
        //TODO: Add consistent
        default:
            reference = DVMH_STAT_METRIC_CPY_DTOH;
            break;
    }
    return (dvmh_stat_metric_names)(defaultMetric + (reference - DVMH_STAT_METRIC_CPY_DTOH));
}
#endif

}
