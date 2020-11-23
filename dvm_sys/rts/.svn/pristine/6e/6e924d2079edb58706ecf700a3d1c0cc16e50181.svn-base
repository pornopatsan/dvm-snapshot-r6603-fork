#ifndef _LINITSTD_C_
#define _LINITSTD_C_
/******************/    /*E0000*/

#include "dvmlib.inc"
#include "sysext.inc"


#define ARGSIZE   256


#ifdef _PGI_F_
#define _MS_F_
#endif

#ifdef _MS_F_

#ifdef _PGI_F_
   #define rtl_nargs rtl_nargs_
   #define rtl_getarg rtl_getarg_
   #define dvmstr dvmstr_   
#endif

   DVMUSERFUN
   char  dvmstr[256];
   DVMUSERFUN
   short length=0;

   DVMUSERFUN
   void  __callstd rtl_nargs(int *);
   DVMUSERFUN
   void  __callstd rtl_getarg(int *);
#else
   #ifdef __GNUC__
      #ifndef __INTEL_COMPILER
         #define iargc_ _gfortran_iargc
         #define getarg_ _gfortran_getarg_i4
      #endif
   #endif
   int   iargc_(void);
   void  getarg_( int *, char *, int );
#endif


DvmType __callstd linit_(DvmType  *InitParamPtr)
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
  char  *p;
  int    i;

  #ifdef _MS_F_
     rtl_nargs(&dvm_argc);
  #else
     dvm_argc = (int)(iargc_() + 1);
  #endif

  mac_malloc(dvm_argv, char **, dvm_argc*sizeof(char *), 1);
  
  if(dvm_argv == NULL)
  {
    fprintf(stderr,
            "*** RTS fatal err 000.001: wrong call linit_ "
            "(no memory)\n");
    SYSTEM(exit, (1))
  }

  for(i=0; i < dvm_argc; i++)
  {
     mac_malloc(dvm_argv[i], char *, ARGSIZE+1, 1);

     if (dvm_argv[i] == NULL)
     {
        fprintf(stderr,
        "*** RTS fatal err 000.001: wrong call linit_ (no memory)\n");
        SYSTEM(exit, (1))
     }

     #ifdef _MS_F_
        {
           int j;

           rtl_getarg(&i);

           if(length <= 0)
           {
              fprintf(stderr,
              "*** RTS fatal err 000.002: wrong call getarg "
              "(Param String Lehgth=%d)\n",(int)length);
              SYSTEM(exit, (1))
           }

           for(j=0; j < length; j++)
               dvm_argv[i][j] = dvmstr[j];

           dvm_argv[i][length] = '\x00';
        }
     #else
        getarg_(&i, dvm_argv[i], ARGSIZE );
     #endif 

     p = dvm_argv[i] + ARGSIZE-1;

     while(p >= dvm_argv[i])
     {
        if (*p != ' ' && *p )
        {
           p[1] = '\0';
           break;
        }

        p--;
     }
  }

  FortranFlag = 1; /* initialization was made from Fortran */    /*E0002*/

  return   rtl_init(*InitParamPtr, dvm_argc, dvm_argv);

}


/****************************************\
* Data type control while support system *
*      initialization from Fortran       *
\****************************************/    /*E0003*/


/* */    /*E0004*/


void  __callstd ftcntr_( int       *ElmNumberPtr,
                         AddrType   FirstAddrArray[],
                         AddrType   NextAddrArray[],
                         int        TypeLengthArray[],
                         int        TypeCodeArray[]    )
{
  uLLng   tl;
  int    i;
  DvmType   ElmNumber, TypeLength, TypeCode;

  if ((TypeCodeArray[0] == 2) && (TypeLengthArray[0] != sizeof(long)))
  {
     fprintf(stdout, "*** RTS fatal err 000.000: "
                     "invalid long-type fortran-representation\n");
     SYSTEM(exit, (1))
  }

  ElmNumber = *ElmNumberPtr; /* the number of elements in given arrays */    /*E0005*/

  for(i=0; i < ElmNumber; i++)
  {
     tl = NextAddrArray[i] - FirstAddrArray[i]; /* distance between
                                                   neighbour elements
                                                   of the same type */    /*E0006*/
     TypeLength = TypeLengthArray[i]; /* supposed type length */    /*E0007*/
     TypeCode = TypeCodeArray[i]; /* number of current checked type */    /*E0008*/

     switch(TypeCode)
     {
        case 1: if(tl != sizeof(int) || TypeLength != sizeof(int))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                       "invalid integer-type fortran-representation\n"
                       "fortran type length[%d] = %ld(%ld) "
                       "sizeof(int) = %d\n",
                       i, tl, TypeLength, (int)sizeof(int));
                  SYSTEM(exit, (1))
                }

                break;

        case 2: if (tl != sizeof(long) || TypeLength != sizeof(long))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid long-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(long) = %d\n",
                        i, tl, TypeLength, (int)sizeof(long));
                  SYSTEM(exit, (1))
                }

                break;

        case 3: if(tl != sizeof(float) || TypeLength != sizeof(float))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid float-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(float) = %d\n",
                        i, tl, TypeLength, (int)sizeof(float));
                  SYSTEM(exit, (1))
                }

                break;

        case 4: if(tl != sizeof(double) || TypeLength != sizeof(double))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid double-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(double) = %d\n",
                        i, tl, TypeLength, (int)sizeof(double));
                  SYSTEM(exit, (1))
                }

                break;

        case 5: if(tl != sizeof(char) || TypeLength != sizeof(char))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid char-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(char) = %d\n",
                        i, tl, TypeLength, (int)sizeof(char));
                  SYSTEM(exit, (1))
                }

                break;

        case 6: if(tl != sizeof(short) || TypeLength != sizeof(short))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid short-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(short) = %d\n",
                        i, tl, TypeLength, (int)sizeof(short));
                  SYSTEM(exit, (1))
                }

                break;

        default:
                break;
     }
  }

  return;
}


/* */    /*E0009*/


void  __callstd tpcntr_(DvmType     *ElmNumberPtr,
                         AddrType  FirstAddrArray[],
                         AddrType  NextAddrArray[],
                         DvmType      TypeLengthArray[],
                         DvmType      TypeCodeArray[])
{
  uLLng  tl;
  int   i;
  DvmType  ElmNumber, TypeLength, TypeCode;

  ElmNumber = *ElmNumberPtr; /* the number of elements in given arrays */    /*E0010*/

  for(i=0; i < ElmNumber; i++)
  {
     tl = NextAddrArray[i] - FirstAddrArray[i]; /* distance between neighbour 
                                                   elements of the same type */    /*E0011*/
     TypeLength = TypeLengthArray[i]; /* supposed type length */    /*E0012*/
     TypeCode = TypeCodeArray[i]; /* number of current checked type */    /*E0013*/

     switch(TypeCode)
     {
        case 1: if(tl != sizeof(int) || TypeLength != sizeof(int))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                       "invalid integer-type fortran-representation\n"
                       "fortran type length[%d] = %ld(%ld) "
                       "sizeof(int) = %d\n",
                       i, tl, TypeLength, (int)sizeof(int));
                  SYSTEM(exit, (1))
                }

                break;

        case 2: if (tl != sizeof(DvmType) || TypeLength != sizeof(DvmType))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid long-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(long) = %d\n",
                        i, tl, TypeLength, (int)sizeof(DvmType));
                  SYSTEM(exit, (1))
                }

                break;

        case 3: if(tl != sizeof(float) || TypeLength != sizeof(float))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid float-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(float) = %d\n",
                        i, tl, TypeLength, (int)sizeof(float));
                  SYSTEM(exit, (1))
                }

                break;

        case 4: if(tl != sizeof(double) || TypeLength != sizeof(double))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid double-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(double) = %d\n",
                        i, tl, TypeLength, (int)sizeof(double));
                  SYSTEM(exit, (1))
                }

                break;

        case 5: if(tl != sizeof(char) || TypeLength != sizeof(char))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid char-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(char) = %d\n",
                        i, tl, TypeLength, (int)sizeof(char));
                  SYSTEM(exit, (1))
                }

                break;

        case 6: if(tl != sizeof(short) || TypeLength != sizeof(short))
                {
                  fprintf(stdout, "*** RTS fatal err 000.000: "
                        "invalid short-type fortran-representation\n"
                        "fortran type length[%d] = %ld(%ld) "
                        "sizeof(short) = %d\n",
                        i, tl, TypeLength, (int)sizeof(short));
                  SYSTEM(exit, (1))
                }

                break;

        default:
                break;
     }
  }

  return;
}

#ifdef _PGI_F_
#undef _MS_F_
#endif
#endif   /*  _LINITSTD_C_  */    /*E0014*/
