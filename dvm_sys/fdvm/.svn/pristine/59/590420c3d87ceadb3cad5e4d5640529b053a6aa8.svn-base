/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/* Simple compatibility module for pC++/Sage (phb) */

/* include it only once... */
#ifndef COMPATIBLE_H
#define COMPATIBLE_H

#include "sage.h"

#ifndef _NEEDALLOCAH_
#  if (defined(__ksr__) || (defined(SAGE_solaris2) && !defined(__GNUC__)))
#    define _NEEDALLOCAH_
#  endif
#endif

#ifdef __hpux
#  ifndef SYS5
#    define SYS5 1
#  endif
#endif
 
#ifdef _SEQUENT_
#  define NO_u_short
 
#  ifndef SYS5
#    define SYS5 1
#  endif
#endif
 
#ifdef sparc
#  if (defined(__svr4__) || defined(SAGE_solaris2))  /* Solaris 2!!! YUK! */
#    ifndef SYS5
#      define SYS5 1
#    endif
#  endif
#endif

#ifndef SYS5
#  define BSD 1
#endif

#ifdef _NEEDCALLOC_
# ifdef CALLOC_DEF
#   undef CALLOC_DEF
# endif

# ifndef CALLOC_DEF
#   ifdef __GNUC__
      extern void *calloc();
#     define CALLOC_DEF
#   endif
# endif

# ifndef CALLOC_DEF
#   ifdef __ksr__
      extern void *calloc();
#     define CALLOC_DEF
#   endif
# endif

# ifndef CALLOC_DEF
#   ifdef cray
#     include "fixcray.h"
#   endif
# endif

# ifndef CALLOC_DEF
    extern char *calloc();
# endif

#endif

#endif
