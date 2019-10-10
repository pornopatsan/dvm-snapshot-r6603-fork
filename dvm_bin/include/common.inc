#ifndef _COMMON_INC_
#define _COMMON_INC_
/******************/    /*E0000*/

#include "mpsusr.inc"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <limits.h>

#include "dvmvers.h"
#include "compile.def"

#ifdef _DVM_ZLIB_
   #include "zlib.h"
#endif

#ifdef _i860_
   #include <channel.h>
   #include <times.h>
#else
   #include <time.h>

   #ifdef _UNIX_
      #include <unistd.h>
      #include <sys/time.h>
      #include <sys/types.h>
      #include <sys/stat.h>
   #else
      #if defined(_DVM_LLIO_) || defined(_STRUCT_STAT_)
          #include <sys\stat.h>
      #endif

      #if defined(_DVM_LLIO_) || defined(_ACCESS_FUN_)
          #include <io.h>
      #endif
   #endif
#endif


#include "user.def"

#include "user.typ"

#include "common.mac"
#include "trace.mac"
#include "user.mac"

#include "usrfun.dec"
#include "usrdebug.dec"


#endif /* _COMMON_INC_ */    /*E0001*/
