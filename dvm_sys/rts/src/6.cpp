#ifndef _6_CPP_
#define _6_CPP_
/*************/

#include "dvmlib.inc"
#include "sysext.inc"

#include <map>
using namespace std;
 
#include "allocmem.c"         // with DvmType
#include "trace.c"            // with DvmType
#include "events.c"           //
#include "statevnt.c"         //
#include "groups.c"           //
#include "intergrp.c"         //

#include "statist.c"          // with DvmType
#include "interval.c"         // with DvmType

#include "dvmh_rts_stat.c"    //
#include "dvmh_stat.c"        // with UDvmType

#include "omp_dbg.c"          // with DvmType

#include "mpitds.c"           // with DvmType

#endif /* _6_CPP_ */
