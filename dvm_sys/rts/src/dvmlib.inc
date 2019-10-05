#ifndef _DVMLIB_INC_
#define _DVMLIB_INC_
/******************/    /*E0000*/

#if defined(_WIN64)
#define __LLP64__ 1
#endif

#if defined(__LLP64__)
  typedef long long DvmType;
  typedef unsigned long long UDvmType;
  #define DTFMT "%lld"
  #define UDTFMT "%llu"
  #define DVMTYPE_MIN LLONG_MIN
  #define DVMTYPE_MAX LLONG_MAX
  #define UDVMTYPE_MAX ULLONG_MAX
  #define DVMTYPE_STR "long long"
#else
  typedef long DvmType;
  typedef unsigned long UDvmType;
  #define DTFMT "%ld"
  #define UDTFMT "%lu"
  #define DVMTYPE_MIN LONG_MIN
  #define DVMTYPE_MAX LONG_MAX
  #define UDVMTYPE_MAX ULONG_MAX
  #define DVMTYPE_STR "long"
#endif

#include "common.inc"

#include "system.def"
#include "config.def"
#include "debug.def"
#include "events.def"
#include "tracer_dvmdbg.def"

#include "system.typ"

#include "dyncntrl.typ"
#include "cmptrace.typ"
#include "cntrlerr.typ"

#include "system.mac"
#include "inputpar.mac"

#ifndef _DVM_IOPROC_
   #include "dynlimit.mac"
   #include "dyncntrl.mac"
#endif

#include "sysfun.dec"

#ifndef _DVM_IOPROC_
   #include "dynlimit.dec"
   #include "dyncntrl.dec"
   #include "cmptrace.dec"
   #include "cntrlerr.dec"
   #include "tracer_dvmdbg.dec"
#endif


#endif /* _DVMLIB_INC_ */    /*E0001*/
