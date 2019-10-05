/**
 * File contains implementation for functionality
 * to gather DVMH statistics.
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#include <dvmh_rts_stat.h>

void stat_dvmh_write_value(int eventId, double value)
#ifdef _F_TIME_
{
    s_GRPTIMES *cell;
    int group;

    if(!IsExpend) return;

    group = StatGrp[eventId];
    cell = &(CurrInterPtr[group][group]);
    cell->CallCount++;
    cell->ProductTime += value;
    cell->LostTime = value;
}
#else
{ }
#endif