#ifndef _3_CPP_
#define _3_CPP_
/*************/

#include "dvmlib.inc"
#include "sysext.inc"

#include <map>
using namespace std;

#include "mps.inc"

#include "objequ.c"  // 
#include "msgpas.c"  // with DvmType
#include "rtl_fun.c" //
#include "genv.c"    // with DvmType

#ifndef _DVM_IOPROC_
   #include "v_turch.c"  // with DvmType
   #include "bounds.c"   // with DvmType
   #include "parloop.c"  // with DvmType
   #include "subtasks.c" // with DvmType
#endif


#endif /* _3_CPP_ */
