#ifndef _MPSUSR_INC_
#define _MPSUSR_INC_
/******************/    /*E0000*/

#ifdef _DVM_MPI_

   #ifndef _NT_MPI_
   #ifdef _WIN_MPI_
      #ifndef __STDC__
         #define __STDC__
         #define __NDEF__STDC__
      #endif
   #endif
   #endif


#ifndef _MPI_STUBS_
   #include <mpi.h>
#else
   #include "mpi_stubs.h"
#endif


   #ifdef _WIN_MPI_
      #ifdef __NDEF__STDC__
         #undef __STDC__
      #endif
   #endif

   
#else
   #ifdef _DVM_GNS_
      #include "gns.h"
   #else
      #ifdef _DVM_ROU_
         #include "gns.h"
      #else
         #ifdef _DVM_PVM_
            #include "pvm3.h"
         #endif
      #endif
   #endif
#endif

#endif    /* _MPSUSR_INC_ */    /*E0001*/
