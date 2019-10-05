#ifndef _MPS_INC_
#define _MPS_INC_
/***************/    /*E0000*/

#ifdef _DVM_MPI_
   #ifdef _MPI_STUBS_
      #include "mpi_stubs.c"
   #endif
   #include "mpi_init.c"
   #include "mpi_mps.c"
#else
   #ifdef _DVM_GNS_
      #include "gns_init.c"
      #include "gns_mps.c"
   #else
      #ifdef _DVM_ROU_
         #include "gns_init.c"
         #include "gns_mps.c"
         #include "libgns.c"
      #else
         #ifdef _DVM_PVM_
            #include "pvm_init.c"
            #include "pvm_mps.c"
         #else
            #include "emp_init.c"
            #include "emp_mps.c"
         #endif
      #endif
   #endif
#endif

#endif /* _MPS_INC_ */    /*E0001*/
