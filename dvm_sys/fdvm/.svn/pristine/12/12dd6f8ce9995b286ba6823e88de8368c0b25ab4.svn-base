/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* Modified By Jenq-Kuen Lee Sep 30, 1987          */
/* Define constants for communication with parse.y. */
/* Copyright (C) 1987 Free Software Foundation, Inc. */

#include <stdio.h>
enum rid
{
  RID_UNUSED,
  RID_INT,
  RID_CHAR,
  RID_FLOAT,
  RID_DOUBLE,
  RID_VOID,
  RID_UNUSED1,

  RID_UNSIGNED,
  RID_SHORT,
  RID_LONG,
  RID_AUTO,
  RID_STATIC,
  RID_EXTERN,
  RID_REGISTER,
  RID_TYPEDEF,
  RID_SIGNED,
  RID_CONST,
  RID_VOLATILE,
  RID_PRIVATE,
  RID_FUTURE,
  RID_VIRTUAL,
  RID_INLINE,
  RID_FRIEND,
  RID_PUBLIC,
  RID_PROTECTED,
  RID_SYNC,
  RID_GLOBL,
  RID_ATOMIC,
  RID_KSRPRIVATE,
  RID_RESTRICT,
  RID_MAX,
  RID_CUDA_GLOBAL,
  RID_CUDA_SHARED,
  RID_CUDA_DEVICE,

  LONG_UNSIGNED_TYPE_CONST,       /*  For numerical constant  */
  LONG_INTEGER_TYPE_CONST,
  UNSIGNED_TYPE_CONST,
  INTEGER_TYPE_CONST,
  FLOAT_TYPE_CONST,
  LONG_DOUBLE_TYPE_CONST,
  DOUBLE_TYPE_CONST,
                                  /* For char constant   */
  UNSIGNED_CHAR_TYPE_CONST,
  CHAR_TYPE_CONST,
  CHAR_ARRAY_TYPE_CONST,

  PLUS_EXPR ,                    /* Statement code  */
  MINUS_EXPR,
  BIT_AND_EXPR,
  BIT_IOR_EXPR,
  MULT_EXPR,
  TRUNC_DIV_EXPR,
  TRUNC_MOD_EXPR,
  BIT_XOR_EXPR,
  LSHIFT_EXPR ,
  RSHIFT_EXPR,
  LT_EXPR,
  GT_EXPR,
  LE_EXPR,
  GE_EXPR,
  NE_EXPR,
  EQ_EXPR
};

/* #define RID_FIRST_MODIFIER RID_UNSIGNED   */

#define NEXT_FULL     10 /*for comments type, FULL, HALF, NEXT_FULL */

/* for access_flag */
#define BIT_PROTECTED 1 /* note: also see PROTECTED_FIELD */
#define BIT_PUBLIC    2 /* note: also see PUBLIC_FIELD  */ 
#define BIT_PRIVATE   4 /* note: also see PRIVATE_FIELD */
#define BIT_FUTURE    8
#define BIT_VIRTUAL   16
#define BIT_INLINE    32

/*for signed_flag */
#define BIT_UNSIGNED  64
#define BIT_SIGNED    128

/* for long_short_flag */
#define BIT_SHORT     256
#define BIT_LONG      512

/* for mod_flag */
#define BIT_VOLATILE  1024
#define BIT_CONST     1024*2
#define BIT_GLOBL     1024*128*2
#define BIT_SYNC      1024*128*4
#define BIT_ATOMIC    1024*128*8
#define BIT_KSRPRIVATE 1024*128*16
#define BIT_RESTRICT  1024*128*32
/* for storage flag */
#define BIT_TYPEDEF   1024*4
#define BIT_EXTERN    1024*8
#define BIT_AUTO      1024*128  /* swapped values for AUTO and FRIEND */
#define BIT_STATIC    1024*32
#define BIT_REGISTER  1024*64
#define BIT_FRIEND    1024*16 /* so that friend would fit in u_short BW*/ 

#define MAX_BIT       1024*128*64
#define STORAGE_FLAG  1024*(4+8+16+32+64+128)
#define BIT_OPENMP    1024*128*128     /* OpenMP Fortran */
#define BIT_CUDA_GLOBAL 1024*128*256     /* Cuda */
#define BIT_CUDA_SHARED 1024*128*512     /* Cuda */
#define BIT_CUDA_DEVICE 1024*128*1024    /* Cuda */





