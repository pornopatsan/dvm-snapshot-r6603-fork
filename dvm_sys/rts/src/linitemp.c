#ifndef _LINITEMP_C_
#define _LINITEMP_C_
/******************/    /*E0000*/

#include "dvmlib.inc"
#include "sysext.inc"

long __callstd linit_(long  *InitParamPtr)
/*
     Initialization in Fortran program.
     ----------------------------------

*InitParamPtr - parameter of Run-Time Library initialization.

The function initializes Run-Time Library internal structures according 
to modes of interprocessor exchanges, statistic and trace accumulation, 
and so on defined in configuration files.
Using zero as parameter implies the default initialization.
The function returns zero. 
*/    /*E0001*/
{
  FortranFlag = 1; /* initialization was made from Fortran */    /*E0002*/

  return   rtl_init(*InitParamPtr, dvm_argc, dvm_argv);
}

#endif   /*  _LINITEMP_C_  */    /*E0003*/
