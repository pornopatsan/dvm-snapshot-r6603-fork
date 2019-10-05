/**
 * Contains the statistics functions for DVMH.
 *
 * @link dvmh_events.def         contains events definition
 * @link ../src/dvmh_rts_stat.h  functions implementation
 *
 * @author Aleksei Shubert <alexei@shubert.ru>
 */

#ifndef _DVMH_RTS_STAT_H_
#define _DVMH_RTS_STAT_H_

#include "compile.def"
#include "dvmh_events.def" // Events definition
#include "dvmh_stat.h"

/**
* Write DVMH event value to the matrix of the interval statistics.
*
* Accumulates calls to the function.
* Every call to the function will add value to wrote before; starts with zero.
*
* @see events_dvmh.def dvmh events definition
*
* @param eventId event identifier
* @param value   event value
*/
DVMUSERFUN
void stat_dvmh_write_value(int eventId, double value);

#endif
