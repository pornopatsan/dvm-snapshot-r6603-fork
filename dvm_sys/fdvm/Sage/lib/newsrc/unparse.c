/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


 /**************************************************************************
  *                                                                        *
  *   Unparser for toolbox                                                 *
  *                                                                        *
  *************************************************************************/

#include <stdio.h>
#include <stdlib.h> /* podd 15.03.99*/
#include "compatible.h"   /* Make different system compatible... (PHB) */
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#include "macro.h"
#include "ext_lib.h"
#include "ext_low.h"
/*static FILE *finput;*/
/*static FILE *outfile;*/
static int TabNumber = 0;
static int TabNumberCopy = 0;
static int Number_Of_Flag = 0;
#define MAXFLAG 64
#define  MAXLFLAG 256
#define MAXLEVEL 256
static char TabOfFlag[MAXFLAG][MAXLFLAG];
static int  FlagLenght[MAXFLAG];
static int  FlagLevel[MAXFLAG];
static int  FlagOn[MAXLEVEL][MAXFLAG];
#define MAXLENGHTBUF 5000000
static int Buf_pointer = 0;
static char UnpBuf[MAXLENGHTBUF];
static int max_lenght_buf = MAXLENGHTBUF;
static char* allocated_buf = NULL;
static char *Buf_address;
int CommentOut = 0;
int HasLabel = 0;
#define C_Initialized 1
#define Fortran_Initialized 2
static int Parser_Initiated = 0;
static int Function_Language = 0; /* 0 - undefined, 1 - C language, 2 - Fortran language */

extern void Message();

/* FORWARD DECLARATIONS */
int BufPutString();

/* usage exemple
    Init_Unparser(); or Reset_Unparser(); if Init_Unparser(); has been done

    fprintf(outfile,"%s",Tool_Unparse_Bif(PROJ_FIRST_BIF ()));
*/

/*****************************************************************************/
/*****************************************************************************/
/*****                                                                   *****/
/*****    UNPARSE.C:     Gregory HOGDAL / Eric MARCHAND    July 1992     *****/
/*****    Modified F. Bodin 08/92 .  Modified D. Gannon 3/93 - 6/93      *****/
/*****                                                                   *****/
/*****************************************************************************/
/*****************************************************************************/

/***********************************/
/* function de unparse des bif node */
/***********************************/

#include "f90.h"

typedef struct 
{
  char *str;
  char *(* fct)();
} UNP_EXPR;


static UNP_EXPR Unparse_Def[LAST_CODE];

/************ Unparse Flags **************/
static int In_Write_Flag = 0;
static int Rec_Port_Decl = 0;
static int In_Param_Flag = 0;
static int In_Impli_Flag = 0;
static int In_Class_Flag = 0;
static int Type_Decl_Ptr = 0;
/*****************************************/
static PTR_SYMB construct_name;

/*************** TYPE names in ASCII form ****************/
static char *ftype_name[] = {"integer",
			       "real",
			       "double precision",
			       "character",
			       "logical",
			       "character",
			       "gate",
			       "event",
			       "sequence",
			       "",
			       "",
			       "",
			       "",
			       "complex",
			       "",
			       "",		
			       "",		
			       "",			
			       "",	
			       "",	
			       "",	
			       "",	
			       "",	
			       "",	
			       "",	
			       "",		
			       "",	
			       "",		
			       "",
			       "",
			       "",
			       "",
			       "double complex",
                               "" 
};static char *ctype_name[] = {"int",
			       "float",
			       "double",
			       "char",
			       "logical",
			       "char",
			       "gate",
			       "event",
			       "sequence",
			       "error1",
			       "error2",
			       "error3",
			       "error4",
			       "complex",
			       "void",
			       "error6",		
			       "error7",		
			       "error8",			
			       "error9",	
			       "error10",	
			       "error11",	
			       "error12",	
			       "ElementType",	
			       "error14",	
			       "error15",	
			       "error16",		
			       "error17",	
			       "error18",		
			       "error19",
			       "error20",
			       "error21",
			       "error22",
			       "error23",
                               "long"
};

static
char *ridpointers[] = {
	"-error1-",			/* unused */
	"-error2-",			/* int */
	"char", 		/* char */
	"float",		/* float */
	"double",		/* double */
	"void", 		/* void */
	"-error3-",			/* unused1 */
	"unsigned",		/* unsigned */
	"short",		/* short */
	"long", 		/* long */
	"auto", 		/* auto */
	"static",		/* static */
	"extern",		/* extern */
	"register",		/* register */
	"typedef",		/* typedef */
	"signed",		/* signed */
	"const",		/* const */
	"volatile",		/* volatile */
	"private",		/* private */
	"future",		/* future */
	"virtual",		/* virtual */
	"inline",		/* inline */
	"friend",		/* friend */
	"-error4-",			/* public */
	"-error5-",			/* protected */
        "Sync",                 /* CC++ sync */
        "global",               /* CC++ global */
        "atomic",               /* CC++ atomic */
        "__private",            /* for KSR */
        "restrict",
        "_error6-",
        "__global__",           /* Cuda */
        "__shared__",           /* Cuda */
        "__device__"            /* Cuda */
};

/*********************************************************/

/******* Precedence table of operators for C++ *******/
static short precedence_C[RSHIFT_ASSGN_OP-EQ_OP+1]=
                              {6,      /*  ==  */
                               5,      /*  <   */
                               5,      /*  >   */
                               6,      /*  !=  */
                               5,      /*  <=  */
                               5,      /*  >=  */
                               3,      /*  +   */
                               3,      /*  -   */
                               11,     /*  ||  */
                               2,      /*  *   */
                               2,      /*  /   */
                               2,      /*  %   */
                               10,     /*  &&  */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               8,      /*  ^   */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               1,      /* Minus_op*/
                               1,      /*  !   */
                               13,     /*  =   */ 
                               1,      /*  * (by adr)*/
                               0,      /*  ->  */
                               0,      /*  function  */
                               1,      /*  --  */
                               1,      /*  ++  */
                               7,      /*   &  */
                               9       /*   |  */
                               };
static short precedence2_C[]= {1,      /*  ~   */
                               12,     /*  ?   */
                               0,      /* none */
                               0,      /* none */
                               4,      /*  <<  */
                               4,      /*  >>  */
                               0,      /* none */
                               1,      /*sizeof*/
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               1,      /*(type)*/ 
                               1,      /*&(address)*/
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */
                               0,      /* none */                        
                               13,     /*   += */
                               13,     /*   -= */
                               13,     /*   &= */
                               13,     /*   |= */
                               13,     /*   *= */
                               13,     /*   /= */
                               13,     /*   %= */
                               13,     /*   ^= */
                               13,     /*  <<= */
                               13      /*  >>= */
                              };

/******* Precedence table of operators for Fortran *******/
static char precedence[] = {5,      /* .eq. */
                            5,      /* .lt. */
                            5,      /* .gt. */
                            5,      /* .ne. */
                            5,      /* .le. */
                            5,      /* .ge. */
                            3,      /*  +   */
                            3,      /*  -   */
                            8,      /* .or. */
                            2,      /*  *   */
                            2,      /*  /   */
                            0,      /* none */
                            7,      /* .and. */
                            1,      /*  **  */
                            0,      /* none */
                            4,      /*  //  */
                            8,      /* .xor. */
                            9,      /* .eqv. */
                            9,     /* .neqv. */
                            0,      /* none */
                            0,      /* none */
                            0,      /* none */                      
                            1,     /* Minus_op*/
                            1      /* not op */
                            };

#define type_index(X)   (X-T_INT)                 /* gives the index of a type to access the Table "ftype_name" from a type code */
#define binop(n)    (n >= EQ_OP && n <= NEQV_OP)  /* gives the boolean value of the operation "n" being binary (not unary) */ 
#define C_op(n)     (n >= EQ_OP && n <= RSHIFT_ASSGN_OP)

/* manage the unparse buffer */

void
DealWith_Rid(typei, flg)
     PTR_TYPE typei;
     int flg;  /* if 1 then do virtual */
{ int j;
  
  int index;
   PTR_TYPE type;
  if (!typei)
    return;
  
  for (type = typei; type; )
    {
      switch(TYPE_CODE(type))
	{
	case T_POINTER : 
	case T_REFERENCE :
	case T_FUNCTION :
	case T_ARRAY	:
	  type = TYPE_BASE(type);
	  break;
        case T_MEMBER_POINTER:
          type = TYPE_COLL_BASE(type);
	case T_DESCRIPT :
	  index = TYPE_LONG_SHORT(type);
           /* printf("index = %d\n", index); */
          if( index & BIT_RESTRICT) {
		BufPutString(ridpointers[(int)RID_RESTRICT],0);
		BufPutString(" ", 0);
		}
          if( index & BIT_KSRPRIVATE) {
		BufPutString(ridpointers[(int)RID_KSRPRIVATE],0);
		BufPutString(" ", 0);
		}
          if( index & BIT_EXTERN) {
		BufPutString(ridpointers[(int)RID_EXTERN],0);
		BufPutString(" ", 0);
		}
          if( index & BIT_TYPEDEF) {
		BufPutString(ridpointers[(int)RID_TYPEDEF],0);
		BufPutString(" ", 0);
		}
	  for (j=1; j< MAX_BIT; j= j*2)
	    {
	      switch (index & j)
		{
		case (int) BIT_PRIVATE:   BufPutString(ridpointers[(int)RID_PRIVATE],0);
		  break;
		case (int) BIT_FUTURE:	BufPutString(ridpointers[(int)RID_FUTURE],0);
		  break;
		case (int) BIT_VIRTUAL:  if(flg) BufPutString(ridpointers[(int)RID_VIRTUAL],0);
		  break;
		case (int) BIT_ATOMIC:  if(flg) BufPutString(ridpointers[(int)RID_ATOMIC],0);
		  break;
		case (int) BIT_INLINE:	BufPutString(ridpointers[(int)RID_INLINE],0);
		  break;
		case (int) BIT_UNSIGNED:  BufPutString(ridpointers[(int)RID_UNSIGNED],0);
		  break;
		case (int) BIT_SIGNED :   BufPutString(ridpointers[(int)RID_SIGNED],0);
		  break;
		case (int) BIT_SHORT :	BufPutString(ridpointers[(int)RID_SHORT],0);
		  break;
		case (int) BIT_LONG :	BufPutString(ridpointers[(int)RID_LONG],0);
		  break;
		case (int) BIT_VOLATILE:  BufPutString(ridpointers[(int)RID_VOLATILE],0);
		  break;
		case (int) BIT_CONST   :  BufPutString(ridpointers[(int)RID_CONST],0); 
		  break;
		case (int) BIT_GLOBL   :  BufPutString(ridpointers[(int)RID_GLOBL],0);
		  break; 
		case (int) BIT_SYNC   :  BufPutString(ridpointers[(int)RID_SYNC],0); 
		  break;
		case (int) BIT_TYPEDEF :  /* BufPutString(ridpointers[(int)RID_TYPEDEF],0); */
		  break;
		case (int) BIT_EXTERN  :  /* BufPutString(ridpointers[(int)RID_EXTERN],0); */
		  break;
		case (int) BIT_AUTO :	BufPutString(ridpointers[(int)RID_AUTO],0);
		  break;
		case (int) BIT_STATIC :   BufPutString(ridpointers[(int)RID_STATIC],0);
		  break;
		case (int) BIT_REGISTER:  BufPutString(ridpointers[(int)RID_REGISTER],0);
		  break;
		case (int) BIT_FRIEND:	BufPutString(ridpointers[(int)RID_FRIEND],0);

		}
	      if ((index & j) != 0)
		BufPutString(" ",0);
	    }
	  type = TYPE_DESCRIP_BASE_TYPE(type);
	  break;
	  default:
	  type = NULL;
	}
    }
}

int is_overloaded_type(bif)
   PTR_BFND bif;
{
   PTR_LLND ll;
   if(!bif) return 0;
   ll = BIF_LL1(bif);
   while(ll && (NODE_SYMB(ll) == NULL)) ll = NODE_OPERAND0(ll);
   if(ll == NULL) return 0;
   if(SYMB_ATTR(NODE_SYMB(ll)) & OVOPERATOR) return 1;
   else return 0;
}

PTR_TYPE Find_Type_For_Bif(bif)
     PTR_BFND bif;
{
  PTR_TYPE type = NULL;
  if (BIF_LL1(bif) && (NODE_CODE(BIF_LL1(bif)) == EXPR_LIST))
    { PTR_LLND tp;
      tp = BIF_LL1(bif);
      for (tp = NODE_OPERAND0(tp); tp && (type == NULL); )
	{
	  switch (NODE_CODE(tp)) {
	  case BIT_NUMBER:
	  case ASSGN_OP :
	  case ARRAY_OP:
	  case FUNCTION_OP :
	  case CLASSINIT_OP:
	  case ADDRESS_OP:
	  case DEREF_OP :
	    tp = NODE_OPERAND0(tp);
	    break ;
          case SCOPE_OP:
            tp = NODE_OPERAND1(tp);
            break;
	  case FUNCTION_REF:
	  case ARRAY_REF:
	  case VAR_REF:
          if (tp)
          {
              if (!NODE_SYMB(tp)){
                  printf("syntax error at line %d\n", bif->g_line);
                  exit(1);
              }
              else
                  type = SYMB_TYPE(NODE_SYMB(tp));
          }
	      tp = NULL;
	      break ;
	  default:
             type = NODE_TYPE(tp);
	    break;
	  }
	}
    }
  return type;
}


int Find_Protection_For_Bif(bif)
     PTR_BFND bif;
{
  int protect = 0;
  if (BIF_LL1(bif) && (BIF_CODE(BIF_LL1(bif)) == EXPR_LIST))
    { PTR_LLND tp;
      tp = BIF_LL1(bif);
      for (tp = NODE_OPERAND0(tp); tp && (protect == 0); )
	{
	  switch (NODE_CODE(tp)) {
	  case BIT_NUMBER:
	  case ASSGN_OP :
	  case ARRAY_OP:
	  case FUNCTION_OP :
	  case CLASSINIT_OP:
	  case ADDRESS_OP:
	  case DEREF_OP :
	    tp = NODE_OPERAND0(tp);
	    break ;
          case SCOPE_OP:
	    tp = NODE_OPERAND1(tp);
	    break;
	  case FUNCTION_REF:
	  case ARRAY_REF:
	  case VAR_REF:
	    if (tp)
	      protect = SYMB_ATTR(NODE_SYMB(tp));
	    tp = NULL;
	    break ;
	  }
	}
    }
  return protect;
}

PTR_TYPE Find_BaseType(ptype)
     PTR_TYPE ptype;
{
  PTR_TYPE pt;

  if (!ptype)
    return NULL;
  pt = TYPE_BASE (ptype); 
  if (pt)
    { int j;
      j = 0;
      while ((j < 100) && pt)
	{
	  if (TYPE_CODE(pt) == DEFAULT) break;
	  if (TYPE_CODE(pt) == T_INT) break;
	  if (TYPE_CODE(pt) == T_FLOAT) break;
	  if (TYPE_CODE(pt) == T_DOUBLE) break;
	  if (TYPE_CODE(pt) == T_CHAR) break;
	  if (TYPE_CODE(pt) == T_BOOL) break;
	  if (TYPE_CODE(pt) == T_STRING) break;
	  if (TYPE_CODE(pt) == T_COMPLEX) break;
	  if (TYPE_CODE(pt) == T_DCOMPLEX) break;
	  if (TYPE_CODE(pt) == T_VOID) break;
	  if (TYPE_CODE(pt) == T_UNKNOWN) break;
	  if (TYPE_CODE(pt) == T_DERIVED_TYPE) break;
	  if (TYPE_CODE(pt) == T_DERIVED_COLLECTION) break;
	  if (TYPE_CODE(pt) == T_DERIVED_TEMPLATE) break;
          if (TYPE_CODE(pt) == T_DERIVED_CLASS) break;
	  if (TYPE_CODE(pt) == T_CLASS) break;
	  if (TYPE_CODE(pt) == T_COLLECTION) break;
	  if (TYPE_CODE(pt) == T_DESCRIPT) break;  /* by dbg */
          if (TYPE_CODE(pt) == T_LONG) break;     /*15.11.12*/
			  
	  pt = TYPE_BASE (pt);
	  j++;
	}
      if (j == 100)
	{
	  Message("Looping in getting the Basetype; sorry",0);
	  exit(1);
	}
    }
  return pt;
}

PTR_TYPE Find_BaseType2(ptype)         /* breaks out of the loop for pointers and references   BW */
     PTR_TYPE ptype;
{
  PTR_TYPE pt;

  if (!ptype)
    return NULL;
  pt = TYPE_BASE (ptype); 
  if (pt)
    { int j;
      j = 0;
      while ((j < 100) && pt)
	{
	  if (TYPE_CODE(pt) == T_REFERENCE) break;
	  if (TYPE_CODE(pt) == T_POINTER) break;
	  if (TYPE_CODE(pt) == DEFAULT) break;
	  if (TYPE_CODE(pt) == T_INT) break;
	  if (TYPE_CODE(pt) == T_FLOAT) break;
	  if (TYPE_CODE(pt) == T_DOUBLE) break;
	  if (TYPE_CODE(pt) == T_CHAR) break;
	  if (TYPE_CODE(pt) == T_BOOL) break;
	  if (TYPE_CODE(pt) == T_STRING) break;
	  if (TYPE_CODE(pt) == T_COMPLEX) break;
	  if (TYPE_CODE(pt) == T_DCOMPLEX) break;
	  if (TYPE_CODE(pt) == T_VOID) break;
	  if (TYPE_CODE(pt) == T_UNKNOWN) break;
	  if (TYPE_CODE(pt) == T_DERIVED_TYPE) break;
	  if (TYPE_CODE(pt) == T_DERIVED_COLLECTION) break;
	  if (TYPE_CODE(pt) == T_DERIVED_CLASS) break;
	  if (TYPE_CODE(pt) == T_CLASS) break;
	  if (TYPE_CODE(pt) == T_COLLECTION) break;
	  if (TYPE_CODE(pt) == T_DESCRIPT) break;  /* by dbg */
			  
	  pt = TYPE_BASE (pt);
	  j++;
	}
      if (j == 100)
	{
	  Message("Looping in getting the Basetype; sorry",0);
	  exit(1);
	}
    }
  return pt;
}



char *create_unp_str(str)
     char *str;
{
  char *pt;

  if (!str)
    return NULL;
    
  pt = (char *) xmalloc(strlen(str)+1);
  memset(pt, 0, strlen(str)+1);
  strcpy(pt,str);
  return pt;
}     


char *alloc_str(size)
     int size;
{
  char *pt;

  if (!(size++)) return NULL;
  pt = (char *) xmalloc(size);
  memset(pt, 0, size);
  return pt;
}     

void Reset_Unparser()
{
  int i,j;

  /* initialize the number of flag */
  Number_Of_Flag = 0;
  for (i=0; i < MAXFLAG ; i++)
    {
      TabOfFlag[i][0] = '\0';
      FlagLenght[i] = 0;
      for(j=0; j<MAXLEVEL; j++)
        FlagOn[j][i] = 0;
      FlagLevel[i] = 0;
    }
  /* setbuffer to 0 */
  Buf_pointer = 0;  
  Buf_address = &(UnpBuf[0]); /* may be reallocated */
  memset(UnpBuf, 0, MAXLENGHTBUF);
}

void Set_Function_Language(lg)  /*16.12.11 podd*/
   int lg;
{ Function_Language = lg;
}

void Unset_Function_Language()   /*16.12.11 podd*/
{
 Function_Language = 0; 
}

int Check_Lang_Fortran_For_File(proj)  /*16.12.11 podd*/
PTR_PROJ proj;
{ if(Function_Language == CSrc)   /* Csrc=1 */
   return(FALSE);
  else if(Function_Language != CSrc && Check_Lang_Fortran(proj)) 
   return(TRUE);
  else
   return(FALSE);
}

void Init_Unparser()
{
  int i,j;
  

  if (Check_Lang_Fortran_For_File(cur_proj) )  /*16.12.11 podd*/    
    {
      if (Parser_Initiated != Fortran_Initialized)
        {
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].str = create_unp_str( NAME);
#include"unparse.def"
#include"unparseDVM.def"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].fct = NULL;
#include"unparse.def"
#include"unparseDVM.def"
#undef DEFNODECODE
          Parser_Initiated = Fortran_Initialized;
          /* set the first tabulation */
          TabNumber = 1;
        }
    } else
      {
        if (Parser_Initiated != C_Initialized)
          {
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].str = create_unp_str( NAME);
#include"unparseC++.def"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].fct = NULL;
#include"unparseC++.def"
#undef DEFNODECODE
            Parser_Initiated = C_Initialized;
            /* init  precedence table of operators for C++ */ 
            for(i=BIT_COMPLEMENT_OP - EQ_OP; i<=RSHIFT_ASSGN_OP-EQ_OP;i++)
               precedence_C[i] = precedence2_C[i-BIT_COMPLEMENT_OP+EQ_OP];  
          }
      }


  /* initialize the number of flag */
  Number_Of_Flag = 0;
  for (i=0; i < MAXFLAG ; i++)
    {
      TabOfFlag[i][0] = '\0';
      FlagLenght[i] = 0;
      for(j=0; j<MAXLEVEL; j++)
        FlagOn[j][i] = 0;
      FlagLevel[i] = 0;
    }
  /* setbuffer to 0 */
  Buf_pointer = 0;  
  Buf_address = &(UnpBuf[0]); /* may be reallocated */
  memset(UnpBuf, 0, MAXLENGHTBUF);
}

void BufferAllocate(size)
int size;
{ if(!allocated_buf || max_lenght_buf != size)  
    allocated_buf = xmalloc(size); /* reallocated */
  max_lenght_buf = size; 
  Buf_address = allocated_buf;
  memset(Buf_address, 0, size);
   
}

/* function to manage the unparse buffer */

int BufPutChar(c)
     char c;
{
  if (Buf_pointer >= max_lenght_buf)  //MAXLENGHTBUF)
    {
      Message("Unparse Buffer Full",0);
      /*return 0;*/ /*podd*/
      exit(1);
    }
  Buf_address[Buf_pointer] = c;
  Buf_pointer++;
  return 1;
}

int BufPutString(s,len)
     char *s;
     int len;
{
    int length;
    if (!s)
    {
        Message("Null String in BufPutString", 0);
        return 0;
    }
    length = len;
    if (length <= 0)
        length = strlen(s);

    if (Buf_pointer + length >= max_lenght_buf)  //MAXLENGHTBUF)
    {
        Message("Unparse Buffer Full", 0);
        /*return 0;*/ /*podd*/
        exit(1);
    }
    strncpy(&(Buf_address[Buf_pointer]), s, length);
    Buf_pointer += length;
    return 1;
}


int BufPutInt(int i)
/*     int i; */
{
  int length;
  char s[MAXLFLAG];
  
  sprintf(s,"%d",i);
  length = strlen(s);
  
  if (Buf_pointer + length>= max_lenght_buf) //MAXLENGHTBUF)
    {
      Message("Unparse Buffer Full",0);
      /*return 0;*/ /*podd*/
      exit(1);
    }
  strncpy(&(Buf_address[Buf_pointer]),s,length);
  Buf_pointer += length;
  return 1;
}

int Get_Flag_val(str, i)
     char  *str;
     int *i;
{
  int j, con;
  char sflag[MAXLFLAG];
  (*i)++; /* skip the paranthesis */  
  /* extract the flag name */
  j = *i;
  con = 0;
  
  while ((str[j]  != '\0') && (str[j]  != ')'))
    {
      sflag[con] = str[j];      
      con ++;      
      j ++;
    }
  sflag[con] = '\0';
  con ++; 
  
  /* look in table if flag is in */
  
  for (j = 0 ;  j < Number_Of_Flag; j++)
    {
      if (strncmp(TabOfFlag[j],sflag, con) == 0)
          break;
    }
  *i += con;      
  if (j >= Number_Of_Flag)
    {
      /* not found  */
      return 0;
    }
  else          
    return FlagOn[FlagLevel[j]][j];

}

void Treat_Flag(str, i, val)
     char  *str;
     int *i;
     int val;     
{
  int j, con;
  char sflag[MAXLFLAG];
  (*i)++; /* skip the paranthesis */  
  /* extract the flag name */
  j = *i;
  con = 0;
  
  while ((str[j]  != '\0') && (str[j]  != ')'))
    {
      sflag[con] = str[j];      
      con ++;      
      j ++;
    }
  sflag[con] = '\0';
  con ++; 
  
  /* look in table if flag is in */
  
  for (j = 0 ;  j < Number_Of_Flag; j++)
    {
      if (strncmp(TabOfFlag[j],sflag, con) == 0)
          break;
    }
      if (j >= Number_Of_Flag)
        {
          /* not found  */
          strcpy(TabOfFlag[Number_Of_Flag],sflag);
          FlagOn[0][Number_Of_Flag] = val;
          FlagLenght[Number_Of_Flag] = con-1;
          Number_Of_Flag++;          
        } else
          FlagOn[FlagLevel[j]][j] += val;
      *i += con;      
}


void PushPop_Flag(str, i, val)
     char  *str;
     int *i;
     int val;     
{
  int j, con;
  char sflag[MAXLFLAG];
  (*i)++; /* skip the paranthesis */  
  /* extract the flag name */
  j = *i;
  con = 0;
  
  while ((str[j]  != '\0') && (str[j]  != ')'))
    {
      sflag[con] = str[j];      
      con ++;      
      j ++;
    }
  sflag[con] = '\0';
  con ++; 
  
  /* look in table if flag is in */
  
  for (j = 0 ;  j < Number_Of_Flag; j++)
    {
      if (strncmp(TabOfFlag[j],sflag, con) == 0)
          break;
    }
      if (j < Number_Of_Flag)
        {
          /* if a pop, clear old value befor poping */
          if(val< 0) FlagOn[FlagLevel[j]][j] = 0; /* added by dbg to make sure initialized */
          FlagLevel[j]  += val;
          if (FlagLevel[j] < 0)
            FlagLevel[j] = 0;
          if (FlagLevel[j] >= MAXLEVEL)
            {
              Message("Stack of flag overflow; abort()",0);
              abort();
            }
        }
      /* else printf("WARNING(unparser): unknow flag pushed or popped:%s\n",sflag); */
      *i += con;      
}

char * Tool_Unparse_Type();

char *
Tool_Unparse_Symbol (symb)
     PTR_SYMB symb;
{
  PTR_TYPE ov_type;
  if (!symb)
    return NULL;
  if (SYMB_IDENT(symb))
    {
       if((SYMB_ATTR(symb) & OVOPERATOR)){
	  ov_type = SYMB_TYPE(symb);
	  if(TYPE_CODE(ov_type) == T_DESCRIPT){
	      if(TYPE_LONG_SHORT(ov_type) == BIT_VIRTUAL && In_Class_Flag){
               BufPutString ("virtual ",0);
	       if(TYPE_LONG_SHORT(ov_type) == BIT_ATOMIC) BufPutString ("atomic ",0);
	       ov_type = TYPE_DESCRIP_BASE_TYPE(ov_type);
	       }
	      if(TYPE_LONG_SHORT(ov_type) == BIT_INLINE){
               BufPutString ("inline ",0);
	       ov_type = TYPE_DESCRIP_BASE_TYPE(ov_type);
	       }
          }
       } else ov_type = NULL;
              
/*      if ((SYMB_ATTR(symb) & OVOPERATOR) ||
	  (strcmp(SYMB_IDENT(symb),"()")==0) ||
          (strcmp(SYMB_IDENT(symb),"*")==0) ||
          (strcmp(SYMB_IDENT(symb),"+")==0) ||
          (strcmp(SYMB_IDENT(symb),"-")==0) ||
          (strcmp(SYMB_IDENT(symb),"/")==0) ||
          (strcmp(SYMB_IDENT(symb),"=")==0) ||
          (strcmp(SYMB_IDENT(symb),"%")==0) ||
          (strcmp(SYMB_IDENT(symb),"&")==0) ||
          (strcmp(SYMB_IDENT(symb),"|")==0) ||
          (strcmp(SYMB_IDENT(symb),"!")==0) ||
          (strcmp(SYMB_IDENT(symb),"~")==0) ||
          (strcmp(SYMB_IDENT(symb),"^")==0) ||
          (strcmp(SYMB_IDENT(symb),"+=")==0) ||
          (strcmp(SYMB_IDENT(symb),"-=")==0) ||
          (strcmp(SYMB_IDENT(symb),"*=")==0) ||
          (strcmp(SYMB_IDENT(symb),"/=")==0) ||
          (strcmp(SYMB_IDENT(symb),"%=")==0) ||
          (strcmp(SYMB_IDENT(symb),"^=")==0) ||
          (strcmp(SYMB_IDENT(symb),"&=")==0) ||
          (strcmp(SYMB_IDENT(symb),"|=")==0) ||
          (strcmp(SYMB_IDENT(symb),"<<")==0) ||
          (strcmp(SYMB_IDENT(symb),">>")==0) ||
          (strcmp(SYMB_IDENT(symb),"<<=")==0) ||
          (strcmp(SYMB_IDENT(symb),">>=")==0) ||
          (strcmp(SYMB_IDENT(symb),"==")==0) ||
          (strcmp(SYMB_IDENT(symb),"!=")==0) ||
          (strcmp(SYMB_IDENT(symb),"<=")==0) ||
          (strcmp(SYMB_IDENT(symb),">=")==0) ||
          (strcmp(SYMB_IDENT(symb),"<")==0) ||
          (strcmp(SYMB_IDENT(symb),">")==0) ||
          (strcmp(SYMB_IDENT(symb),"&&")==0) ||
          (strcmp(SYMB_IDENT(symb),"||")==0) ||
          (strcmp(SYMB_IDENT(symb),"++")==0) ||
          (strcmp(SYMB_IDENT(symb),"--")==0) ||
          (strcmp(SYMB_IDENT(symb),"->")==0) ||
          (strcmp(SYMB_IDENT(symb),"->*")==0) ||
          (strcmp(SYMB_IDENT(symb),",")==0) ||	  
          (strcmp(SYMB_IDENT(symb),"[]")==0) )
        BufPutString ("operator ",0);
*/	
    }
  /*
  if(ov_type) Tool_Unparse_Type(ov_type, 0);
  else */
  BufPutString (SYMB_IDENT(symb),0);
  return Buf_address;
}


typedef struct
{
  int typ;
  union {char *S;
//         int  I;
           long  I;
        } val;
} operand;

/* macro def. of operand type */
#define UNDEF_TYP   0
#define STRING_TYP  1
#define INTEGER_TYP 2

/* macro def. of comparison operators */
#define COMP_UNDEF -1 /* Bodin */
#define COMP_EQUAL 0
#define COMP_DIFF  1



void Get_Type_Operand (str, iptr, ptype,Op)
     char *str;
     int *iptr;
     PTR_TYPE ptype;
     operand *Op;
{

  Op->typ = UNDEF_TYP;
  if (strncmp(&(str[*iptr]),"%CHECKFLAG", strlen("%CHECKFLAG"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%CHECKFLAG");
      Op->val.I = Get_Flag_val(str, iptr);
    } else
  if (strncmp(&(str[*iptr]),"%STRCST", strlen("%STRCST"))== 0)               /* %STRCST : String Constant */
    {
      int i_save;

      *iptr += strlen("%STRCST");
      while (str[*iptr] == ' ') {(*iptr)++;} /* skip spaces before string */
      if (str[*iptr] != '\'')
        {
          Message (" *** Missing \"'\" after %STRCST *** ",0);
        }
      i_save = ++(*iptr);
      while ((str[*iptr] != '\0') && (str[*iptr] != '\'')) (*iptr)++;
      Op->val.S = alloc_str ((*iptr) - i_save);
      strncpy (Op->val.S, &(str[i_save]), (*iptr) - i_save);
      Op->typ = STRING_TYP;
    } else
  if (strncmp(&(str[*iptr]),"%NULL", strlen("%NULL"))== 0)                   /* %NULL : Integer Constant (or false boolean) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = 0;
      *iptr += strlen("%NULL");
    } else
  if (strncmp(&(str[*iptr]),"%INIMPLI", strlen("%INIMPLI"))== 0)             /* %INIMPLI : In_Impli_Statement (integer / boolean flag) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = In_Impli_Flag;
      *iptr += strlen("%INIMPLI");
    } else
      {
        Message (" *** Unknown operand in %IF (condition) for Type Node *** ",0);
      }
}

void Get_LL_Operand (str, iptr, ll, Op)
     char *str;
     int *iptr;
     PTR_LLND ll;
     operand *Op;
{

  Op->typ = UNDEF_TYP;
  if (strncmp(&(str[*iptr]),"%CHECKFLAG", strlen("%CHECKFLAG"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%CHECKFLAG");
      Op->val.I = Get_Flag_val(str, iptr);
    } else
  if (strncmp(&(str[*iptr]),"%STRCST", strlen("%STRCST"))== 0)               /* %STRCST : String Constant */
    {
      int i_save;

      *iptr += strlen("%STRCST");
      while (str[*iptr] == ' ') {(*iptr)++;} /* skip spaces before string */
      if (str[*iptr] != '\'')
        {
          Message  (" *** Missing \"'\" after %STRCST *** ",0);
        }
      i_save = ++(*iptr);
      while ((str[*iptr] != '\0') && (str[*iptr] != '\'')) (*iptr)++;
      Op->val.S = alloc_str ((*iptr) - i_save);
      strncpy (Op->val.S, &(str[i_save]), (*iptr) - i_save);
      Op->typ = STRING_TYP;
    } else
  if (strncmp(&(str[*iptr]),"%SYMBOL", strlen("%SYMBOL"))== 0)               /* %SYMBOL : Symbol pointer (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) NODE_SYMB (ll);
      *iptr += strlen("%SYMBOL");
    } else
  if (strncmp(&(str[*iptr]),"%SYMBID", strlen("%SYMBID"))== 0)               /* %SYMBID : Symbol identifier (string) */
    {
      Op->typ = STRING_TYP;
      if (NODE_SYMB (ll))
        Op->val.S = SYMB_IDENT (NODE_SYMB (ll));
      else
        Op->val.S = NULL;
      *iptr += strlen("%SYMBID");
    } else
  if (strncmp(&(str[*iptr]),"%VALUE", strlen("%VALUE"))== 0)               /* %VALUE: Symbol value */
    {
      Op->typ = INTEGER_TYP;
      if (NODE_TEMPLATE_LL1 (ll) && NODE_SYMB (NODE_TEMPLATE_LL1 (ll)) && NODE_CODE(NODE_SYMB (NODE_TEMPLATE_LL1 (ll)))==CONST_NAME)
        Op->val.I = (long) (NODE_SYMB (NODE_TEMPLATE_LL1(ll)))->entry.const_value;
      else
        Op->val.I =  0;
      *iptr += strlen("%VALUE");
    } else
  if (strncmp(&(str[*iptr]),"%NULL", strlen("%NULL"))== 0)                   /* %NULL : Integer Constant (or false boolean) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = 0;
      *iptr += strlen("%NULL");
    } else
  if (strncmp(&(str[*iptr]),"%LL1", strlen("%LL1"))== 0)                     /* %LL1 : Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;      
      Op->val.I = (long) NODE_TEMPLATE_LL1 (ll);
      *iptr += strlen("%LL1");
    } else
  if (strncmp(&(str[*iptr]),"%LL2", strlen("%LL2"))== 0)                     /* %LL2 : Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) NODE_TEMPLATE_LL2 (ll);
      *iptr += strlen("%LL2");
    } else
  if (strncmp(&(str[*iptr]),"%LABUSE", strlen("%LABUSE"))== 0)               /* %LABUSE : label ptr (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) NODE_LABEL (ll);
      *iptr += strlen("%LABUSE");
    } else
  if (strncmp(&(str[*iptr]),"%L1CODE", strlen("%L1CODE"))== 0)               /* %L1CODE : Code (variant) of Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (NODE_TEMPLATE_LL1 (ll))
        Op->val.I = NODE_CODE (NODE_TEMPLATE_LL1 (ll));
      else
        Op->val.I = 0;
      *iptr += strlen("%L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L2CODE", strlen("%L2CODE"))== 0)               /* %L2CODE : Code (variant) of Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (NODE_TEMPLATE_LL2 (ll))
        Op->val.I = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
      else
        Op->val.I = 0;
      *iptr += strlen("%L2CODE");
    } else
  if (strncmp(&(str[*iptr]),"%INWRITE", strlen("%INWRITE"))== 0)             /* %INWRITE : In_Write_Statement (integer / boolean flag) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = In_Write_Flag;
      *iptr += strlen("%INWRITE");
    } else
  if (strncmp(&(str[*iptr]),"%RECPORT", strlen("%RECPORT"))== 0)            /* %RECPORT : reccursive_port_decl (integer / boolean flag) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = Rec_Port_Decl;
      *iptr += strlen("%RECPORT");
    } else
  if (strncmp(&(str[*iptr]),"%INPARAM", strlen("%INPARAM"))== 0)             /* %INPARAM : In_Param_Statement (integer / boolean flag) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = In_Param_Flag;
      *iptr += strlen("%INPARAM");
    } else
  if (strncmp(&(str[*iptr]),"%INIMPLI", strlen("%INIMPLI"))== 0)             /* %INIMPLI : In_Impli_Statement (integer / boolean flag) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = In_Impli_Flag;
      *iptr += strlen("%INIMPLI");
    } else
      if (strncmp(&(str[*iptr]),"%L1L2*L1CODE", strlen("%L1L2*L1CODE"))== 0)       /* %L1L2L1CODE : Code (variant) of Low Level Node 1 of Low Level Node 2 of Low Level Node 1 (integer) */
    {
      PTR_LLND temp;
      
      Op->typ = INTEGER_TYP;
      if (NODE_OPERAND0(ll))
        {
          temp = NODE_OPERAND0(ll);
          while (temp && NODE_OPERAND1(temp)) temp =  NODE_OPERAND1(temp);
          if (temp && NODE_OPERAND0(temp))
            Op->val.I = NODE_CODE (NODE_OPERAND0(temp));
          else
            Op->val.I = 0;
        }
      else
        Op->val.I = 0;
      *iptr += strlen("%L1L2*L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%TYPEDECL", strlen("%TYPEDECL"))== 0)            /* %TYPEDECL */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = Type_Decl_Ptr;
      *iptr += strlen("%TYPEDECL");
    } else
  if (strncmp(&(str[*iptr]),"%TYPEBASE", strlen("%TYPEBASE"))== 0)            /* %TYPEBASE */
    { PTR_TYPE type;
      Op->typ = INTEGER_TYP;
      if (NODE_SYMB(ll))
        type = SYMB_TYPE( NODE_SYMB (ll));
      else
        type = NULL;
      if (type && (TYPE_CODE(type) == T_ARRAY))
	{  
	   type = Find_BaseType(type);
	} 
      Op->val.I = (long) type;
      *iptr += strlen("%TYPEBASE");

    } else
      {
        Message  (" *** Unknown operand in %IF (condition) for LL Node *** ",0);
      }
}


void Get_Bif_Operand (str, iptr, bif,Op)
     char *str;
     int *iptr;
     PTR_BFND bif;
     operand *Op;
{

  Op->typ = UNDEF_TYP;
  if (strncmp(&(str[*iptr]),"%ELSIFBLOB2", strlen("%ELSIFBLOB2"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%ELSIFBLOB2");
      if (BIF_BLOB2(bif) && (BIF_CODE(BLOB_VALUE(BIF_BLOB2(bif))) == ELSEIF_NODE))
        Op->val.I = 1;
      else
        Op->val.I = 0;      
    } else
  if (strncmp(&(str[*iptr]),"%ELSWHBLOB2", strlen("%ELSWHBLOB2"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%ELSWHBLOB2");
      if (BIF_BLOB2(bif) && (BIF_CODE(BLOB_VALUE(BIF_BLOB2(bif))) == ELSEWH_NODE))
        Op->val.I = 1;
      else
        Op->val.I = 0;      
    } else
  if (strncmp(&(str[*iptr]),"%LABEL", strlen("%LABEL"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%LABEL");
      Op->val.I = (long) BIF_LABEL(bif);
    } else
  if (strncmp(&(str[*iptr]),"%CHECKFLAG", strlen("%CHECKFLAG"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      *iptr += strlen("%CHECKFLAG");
      Op->val.I = Get_Flag_val(str, iptr);
    } else
  if (strncmp(&(str[*iptr]),"%BLOB1", strlen("%BLOB1"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_BLOB1(bif);
      *iptr += strlen("%BLOB1");
    } else
  if (strncmp(&(str[*iptr]),"%BLOB2", strlen("%BLOB2"))== 0)  
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long)  BIF_BLOB2(bif);
      *iptr += strlen("%BLOB2");
    } else
      if (strncmp(&(str[*iptr]),"%BIFCP", strlen("%BIFCP"))== 0)  
        {
          Op->typ = INTEGER_TYP;
          if (BIF_CP(bif))
            Op->val.I = BIF_CODE(BIF_CP(bif));
          else
            Op->val.I = 0;          
          *iptr += strlen("%BIFCP");

    } else
      if (strncmp(&(str[*iptr]),"%CPBIF", strlen("%CPBIF"))== 0)  
        {
          Op->typ = INTEGER_TYP;
          if (BIF_CP(bif) && BIF_CP(BIF_CP(bif)))
            Op->val.I = BIF_CODE(BIF_CP(BIF_CP(bif)));
          else
            Op->val.I = 0;          
          *iptr += strlen("%CPBIF");

        } else
          if (strncmp(&(str[*iptr]),"%VALINT", strlen("%VALINT"))== 0)  
            {
              Op->typ = INTEGER_TYP;              
              Op->val.I = atoi(&(str[*iptr + strlen("%VALINT")]));    /* %VALINT-12232323  space is necessary after the number*/
              /* skip to next statement */
              while (str[*iptr] != ' ') (*iptr)++;
            } else
  if (strncmp(&(str[*iptr]),"%RECURSBIT", strlen("%RECURSBIT"))== 0)         /* %RECURSBIT : Symbol Attribut (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = RECURSIVE_BIT;
      *iptr += strlen("%RECURSBIT");
    } else
  if (strncmp(&(str[*iptr]),"%EXPR_LIST", strlen("%EXPR_LIST"))== 0)         /* %EXPR_LIST : int constant EXPR_LIST code for Low Level Node (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = EXPR_LIST;
      *iptr += strlen("%EXPR_LIST");
    } else
  if (strncmp(&(str[*iptr]),"%SPEC_PAIR", strlen("%SPEC_PAIR"))== 0)         /* %SPEC_PAIR : int constant SPEC_PAIR code for Low Level Node (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = SPEC_PAIR;
      *iptr += strlen("%SPEC_PAIR");
    } else
  if (strncmp(&(str[*iptr]),"%IOACCESS", strlen("%IOACCESS"))== 0)           /* %IOACCESS : int constant IOACCESS code for Low Level Node (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = IOACCESS;
      *iptr += strlen("%IOACCESS");
    } else
  if (strncmp(&(str[*iptr]),"%STRCST", strlen("%STRCST"))== 0)               /* %STRCST : String Constant */
    {
      int i_save;

      *iptr += strlen("%STRCST");
      while (str[*iptr] == ' ') {(*iptr)++;} /* skip spaces before string */
      if (str[*iptr] != '\'')
        {
          Message  (" *** Missing \"'\" after %STRCST *** ",0);
        }
      i_save = ++(*iptr);
      while ((str[*iptr] != '\0') && (str[*iptr] != '\'')) (*iptr)++;
      Op->val.S = alloc_str ((*iptr) - i_save);
      strncpy (Op->val.S, &(str[i_save]), (*iptr) - i_save);
      Op->typ = STRING_TYP;
       (*iptr)++; /* skip the ' */
    } else
  if (strncmp(&(str[*iptr]),"%SYMBOL", strlen("%SYMBOL"))== 0)               /* %SYMBOL : Symbol pointer (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_SYMB (bif);
      *iptr += strlen("%SYMBOL");
    } else
  if (strncmp(&(str[*iptr]),"%SATTR", strlen("%SATTR"))== 0)                 /* %SATTR : Symbol Attribut (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (BIF_SYMB (bif))->attr;
      *iptr += strlen("%SATTR");
    } else
  if (strncmp(&(str[*iptr]),"%SYMBID", strlen("%SYMBID"))== 0)               /* %SYMBID : Symbol identifier (string) */
    {
      Op->typ = STRING_TYP;
      if (BIF_SYMB (bif))
        Op->val.S = SYMB_IDENT (BIF_SYMB (bif));
      else
        Op->val.S = NULL;
      *iptr += strlen("%SYMBID");
    } else
  if (strncmp(&(str[*iptr]),"%NULL", strlen("%NULL"))== 0)                   /* %NULL : Integer Constant (or false boolean) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = 0;
      *iptr += strlen("%NULL");
    } else
  if (strncmp(&(str[*iptr]),"%LL1", strlen("%LL1"))== 0)                     /* %LL1 : Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_LL1 (bif);
      *iptr += strlen("%LL1");
    } else
  if (strncmp(&(str[*iptr]),"%LL2", strlen("%LL2"))== 0)                     /* %LL2 : Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_LL2 (bif);
      *iptr += strlen("%LL2");
    } else
  if (strncmp(&(str[*iptr]),"%LL3", strlen("%LL3"))== 0)                     /* %LL3 : Low Level Node 3 (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_LL3 (bif);
      *iptr += strlen("%LL3");
    } else
  if (strncmp(&(str[*iptr]),"%LABUSE", strlen("%LABUSE"))== 0)               /* %LABUSE : label ptr (used for do : doend) (integer) */
    {
      Op->typ = INTEGER_TYP;
      Op->val.I = (long) BIF_LABEL_USE (bif);
      *iptr += strlen("%LABUSE");
    } else
  if (strncmp(&(str[*iptr]),"%L1CODE", strlen("%L1CODE"))== 0)               /* %L1CODE : Code (variant) of Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (BIF_LL1 (bif))
        Op->val.I = NODE_CODE (BIF_LL1 (bif));
      else
        Op->val.I = 0;
      *iptr += strlen("%L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L2CODE", strlen("%L2CODE"))== 0)               /* %L2CODE : Code (variant) of Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (BIF_LL2 (bif))
        Op->val.I = NODE_CODE (BIF_LL2 (bif));
      else
        Op->val.I = 0;
      *iptr += strlen("%L2CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L1L2L1CODE", strlen("%L1L2L1CODE"))== 0)       /* %L1L2L1CODE : Code (variant) of Low Level Node 1 of Low Level Node 2 of Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (BIF_LL1 (bif) && NODE_TEMPLATE_LL2 (BIF_LL1 (bif)) && NODE_TEMPLATE_LL1 (NODE_TEMPLATE_LL2 (BIF_LL1 (bif))))
        Op->val.I = NODE_CODE (NODE_TEMPLATE_LL1 (NODE_TEMPLATE_LL2 (BIF_LL1 (bif))));
      else
        Op->val.I = 0;
      *iptr += strlen("%L1L2L1CODE");
    } else
      if (strncmp(&(str[*iptr]),"%L1L2*L1CODE", strlen("%L1L2*L1CODE"))== 0)       /* %L1L2L1CODE : Code (variant) of Low Level Node 1 of Low Level Node 2 of Low Level Node 1 (integer) */
    {
      PTR_LLND temp;
      
      Op->typ = INTEGER_TYP;
      if (BIF_LL1 (bif) && NODE_TEMPLATE_LL2 (BIF_LL1 (bif)) && NODE_TEMPLATE_LL1 (NODE_TEMPLATE_LL2 (BIF_LL1 (bif))))
        {
          temp = BIF_LL1 (bif);
          while (NODE_OPERAND1(temp)) temp =  NODE_OPERAND1(temp);
          if (NODE_TEMPLATE_LL1 (temp))
            Op->val.I = NODE_CODE (NODE_TEMPLATE_LL1 (temp));
          else
            Op->val.I = 0;
        }
      else
        Op->val.I = 0;
      *iptr += strlen("%L1L2*L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L2L1STR", strlen("%L2L1STR"))== 0)             /* %L2L1STR : String (string_val) of Low Level Node 1 of Low Level Node 2 (string) */
    {
      Op->typ = STRING_TYP;
      if (BIF_LL2 (bif) && NODE_TEMPLATE_LL1 (BIF_LL2 (bif)))
        Op->val.S = NODE_STR (NODE_TEMPLATE_LL1 (BIF_LL2 (bif)));
      else
        Op->val.S = NULL;
      *iptr += strlen("%L2L1STR");

    } else
      {
        Message  (" *** Unknown operand in %IF (condition) for Bif Node *** ",0);
      }
}


int
GetComp (str, iptr)
     char *str;
     int *iptr;
{
  int Comp;

  if (strncmp(&(str[*iptr]),"==", strlen("==")) == 0)         /* == : Equal */
    {
      Comp = COMP_EQUAL;
      *iptr += strlen("==");
    } else
  if (strncmp(&(str[*iptr]),"!=", strlen("!=")) == 0)         /* != : Different */
    {
      Comp = COMP_DIFF;
      *iptr += strlen("!=");
    } else
      {
        Message (" *** Unknown comparison operator in %IF (condition) *** ",0);
        Comp = COMP_UNDEF;
      }
  return Comp;
}

int
Eval_Type_Condition(str, ptype)
     char *str;
     PTR_TYPE ptype;
{
  int Result = 0;
  int i = 0;
  operand Op1, Op2;
  int Comp;

  while (str[i] == ' ') {i++;} /* skip spaces before '(condition)' */
  if (str[i++] != '(')
    {
      Message  (" *** Missing (condition) after %IF *** ",0);
      
      return 0;
    } else
  while (str[i] == ' ') {i++;} /* skip spaces before first operand */
  Get_Type_Operand(str, &i, ptype, &Op1);
  while (str[i] == ' ') {i++;} /* skip spaces before the comparison operator */
  Comp = GetComp(str, &i);
  while (str[i] == ' ') {i++;} /* skip spaces before second operand */
  Get_Type_Operand(str, &i, ptype, &Op2);
  while (str[i] == ' ') {i++;} /* skip spaces before the closing round bracket */
  if (str[i] != ')')
    {
      Message (" *** Missing ')' after %IF (condition *** ",0);
      return i;
    } else
  i++;
  if ((Op1.typ != UNDEF_TYP) && (Op1.typ == Op2.typ) && (Comp !=COMP_UNDEF))
    {
      switch (Op1.typ)
        {
          case STRING_TYP : Result = strcmp (Op1.val.S, Op2.val.S);
            break;
          case INTEGER_TYP : Result = Op1.val.I - Op2.val.I;
            break;
        }
      if (Comp == COMP_EQUAL) Result = !Result;
      if (Result) return i; /* continue from here to the corresponding %ELSE if exists */
      else                  /* continue at the corresponding %ELSE */
        {
          int ifcount_local = 1;
          while (str[i])
            {
              while (str[i] != '%') {
                                      if (str[i]) i++;
                                      else return i;
                                    }
              i++;
              if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* Counts %IF */
                {
                  ifcount_local++;
                  i += strlen("IF");
                } else
              if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* Counts %ENDIF ; stop skipping if corresponding */
                {
                  ifcount_local--;
                  i += strlen("ENDIF");
                  if (ifcount_local == 0) return i;
                } else
              if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* Counts %ELSE ; stop skipping if corresponding*/
                {
                  i += strlen("ELSE");
                  if (ifcount_local == 1) return i;
                }
            }
          return i;
        }
    } else
        {
          Message (" *** Error in condition for %IF command *** 1",0);
          return i;
        }
}


int
Eval_LLND_Condition(str, ll)
     char *str;
     PTR_LLND ll;
{
  int Result = 0;
  int i = 0;
  operand Op1, Op2;
  int Comp = 0;

  while (str[i] == ' ') {i++;} /* skip spaces before '(condition)' */
  if (str[i++] != '(')
    {
      Message  (" *** Missing (condition) after %IF *** ",0);
      return 0;
    } else
  while (str[i] == ' ') {i++;} /* skip spaces before first operand */
 Get_LL_Operand(str, &i, ll, &Op1);
  while (str[i] == ' ') {i++;} /* skip spaces before the comparison operator */
  Comp = GetComp(str, &i);
  while (str[i] == ' ') {i++;} /* skip spaces before second operand */
  Get_LL_Operand(str, &i, ll, &Op2);
  while (str[i] == ' ') {i++;} /* skip spaces before the closing round bracket */
  if (str[i] != ')')
    {
      Message (" *** Missing ')' after %IF (condition *** ",0);
      i++;      
      return i;
    }  else
       i++; 

  if ((Op1.typ != UNDEF_TYP) && (Op1.typ == Op2.typ) && (Comp != COMP_UNDEF))
    {
      switch (Op1.typ)
        {
          case STRING_TYP : Result = strcmp (Op1.val.S, Op2.val.S);
            break;
          case INTEGER_TYP : Result = Op1.val.I - Op2.val.I;
            break;
        }
      if (Comp == COMP_EQUAL) Result = !Result;
      if (Result) return i; /* continue from here to the corresponding %ELSE if exists */
      else                  /* continue at the corresponding %ELSE */
        {
          int ifcount_local = 1;
          while (str[i])
            {
              while (str[i] != '%') {
                                      if (str[i]) i++;
                                      else return i;
                                    }
              i++;
              if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* Counts %IF */
                {
                  ifcount_local++;
                  i += strlen("IF");
                } else
              if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* Counts %ENDIF ; stop skipping if corresponding */
                {
                  ifcount_local--;
                  i += strlen("ENDIF");
                  if (ifcount_local == 0) return i;
                } else
              if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* Counts %ELSE ; stop skipping if corresponding*/
                {
                  i += strlen("ELSE");
                  if (ifcount_local == 1) return i;
                }
            }
          return i;
        }
    } else
        {
          Message  (" *** Error in condition for %IF command *** 2",0);
          return i;
        }
}


int
Eval_Bif_Condition(str, bif)
     char *str;
     PTR_BFND bif;
{
  int Result = 0;
  int i = 0;
  operand Op1, Op2;
  int Comp;

  while (str[i] == ' ') {i++;} /* skip spaces before '(condition)' */
  if (str[i++] != '(')
    {
      Message (" *** Missing (condition) after %IF *** ",0);
      return 0;
    } else
  while (str[i] == ' ') {i++;} /* skip spaces before first operand */
  Get_Bif_Operand(str, &i, bif, &Op1);
  while (str[i] == ' ') {i++;} /* skip spaces before the comparison operator */
  Comp = GetComp(str, &i);
  while (str[i] == ' ') {i++;} /* skip spaces before second operand */
  Get_Bif_Operand(str, &i, bif, &Op2);
  while (str[i] == ' ') {i++;} /* skip spaces before the closing round bracket */

  if (str[i] != ')')
    {
      Message  (" *** Missing ')' after %IF (condition *** ",0);
      return i;
    } else
  i++;
  if ((Op1.typ != UNDEF_TYP) && (Op1.typ == Op2.typ) && (Comp != COMP_UNDEF))
    {
      switch (Op1.typ)
        {
          case STRING_TYP : Result = strcmp (Op1.val.S, Op2.val.S);
            break;
          case INTEGER_TYP : Result = Op1.val.I - Op2.val.I;
            break;
        }
      if (Comp == COMP_EQUAL) Result = !Result;
      if (Result) return i; /* continue from here to the corresponding %ELSE if exists */
      else                  /* continue at the corresponding %ELSE */
        {
          int ifcount_local = 1;
          while (str[i])
            {
              while (str[i] != '%') {
                                      if (str[i]) i++;
                                      else return i;
                                    }
              i++;
              if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* Counts %IF */
                {
                  ifcount_local++;
                  i += strlen("IF");
                } else
              if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* Counts %ENDIF ; stop skipping if corresponding */
                {
                  ifcount_local--;
                  i += strlen("ENDIF");
                  if (ifcount_local == 0) return i;
                } else
              if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* Counts %ELSE ; stop skipping if corresponding*/
                {
                  i += strlen("ELSE");
                  if (ifcount_local == 1) return i;
                }
            }
          return i;
        }
    } else
        {
          Message  (" *** Error in condition for %IF command *** 3",0);
          return i;
        }
}


int
SkipToEndif (str)
     char *str;
{
  int ifcount_local = 1;
  int i = 0;

  while (str[i])
    {
      while (str[i] != '%') {
                              if (str[i]) i++;
                              else return i;
                            }
      i++;
      if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                         /* Counts %IF */
        {
          ifcount_local++;
          i += strlen("IF");
        } else
      if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)                   /* Counts %ENDIF ; stop skipping if corresponding */
        {
          ifcount_local--;
          i += strlen("ENDIF");
          if (ifcount_local == 0) return i;
        }
    }
  return i;
}

char *Tool_Unparse2_LLnode (); 

char *
Tool_Unparse_Type (ptype)
     PTR_TYPE ptype;
     /*int def;*/        /* def = 1 : defined type*/
                     /*   def = 0 : named type */
{
  int variant;
  int kind;
  char *str;
  char c;
  int i;

  if (!ptype)
    return NULL;

  variant = TYPE_CODE (ptype);
  kind = (int) node_code_kind [(int) variant];
  if (kind != (int)TYPENODE)
    Message ("Error in Unparse, not a type node", 0);

  str = Unparse_Def [variant].str;

  /* now we have to interpret the code to unparse it */

  if (str == NULL)
    return NULL;
  if (strcmp ( str, "n") == 0)
    {
      Message("Node not define for unparse",0);
      return NULL;
    }
  

  i = 0 ;
  c = str[i];
  while (c != '\0')
    {
      if (c == '%')
        {
          i++;
          c = str[i];
          /******** WE HAVE TO INTERPRET THE COMMAND *********/
          if (c  == '%')                                                 /* %% : Percent Sign */
            {
              BufPutString ("%",0);
              i++;
            } else
          if (strncmp(&(str[i]),"ERROR", strlen("ERROR"))== 0)           /* %ERROR : Generate error message */
            {
              Message("Error Node not defined",0);
	      BufPutInt(variant);
              BufPutString ("-----TYPE ERROR--------",0);
              i += strlen("ERROR");
            } else
          if (strncmp(&(str[i]),"NL", strlen("NL"))== 0)                 /* %NL : NewLine */
            {
              /*int j;*/ /* podd 15.03.99*/
              BufPutChar ('\n');
/*              for (j = 0; j < TabNumber; j++)
                if (j>1)
                  BufPutString ("   ",0);
                else
                  BufPutString ("       ",0);*/
               i += strlen("NL");
            } else
           if (strncmp(&(str[i]),"NOTABNL", strlen("NOTABNL"))== 0)                 /* %NL : NewLine */
            {
              BufPutChar ('\n');
              i += strlen("NOTABNL");
            } else
	   if (strncmp(&(str[i]),"RIDPT", strlen("RIDPT"))== 0)      
                { /*int j;*/ /* podd 15.03.99*/
		  DealWith_Rid(ptype,In_Class_Flag);
                  i += strlen("RIDPT");
                } else
           if (strncmp(&(str[i]),"TABNAME", strlen("TABNAME"))== 0)       /* %TABNAME : Self Name from Table */
                {
		  if (Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
		    BufPutString (ftype_name [type_index (TYPE_CODE (ptype))],0);
		  else
		    {
		      BufPutString (ctype_name [type_index (TYPE_CODE (ptype))],0);
		    }
                  i += strlen("TABNAME");
                } else
                  if (strncmp(&(str[i]),"TAB", strlen("TAB"))== 0)               /* %TAB : Tab */
               {
              BufPutString ("      ",0); /* cychen */
              i += strlen("TAB");
            } else
	 if (strncmp(&(str[i]),"SETFLAG", strlen("SETFLAG"))== 0)
	   {               
		i = i + strlen("SETFLAG");
		Treat_Flag(str, &i,1);
	   } else
	 if (strncmp(&(str[i]),"UNSETFLAG", strlen("UNSETFLAG"))== 0) 
	   {               
		i = i + strlen("UNSETFLAG");
		Treat_Flag(str, &i,-1);
	   } else
         if (strncmp(&(str[i]),"PUSHFLAG", strlen("PUSHFLAG"))== 0)
           {               
                i = i + strlen("PUSHFLAG");
                PushPop_Flag(str, &i,1);
           } else
         if (strncmp(&(str[i]),"POPFLAG", strlen("POPFLAG"))== 0) 
            {               
                i = i + strlen("POPFLAG");
                PushPop_Flag(str, &i,-1);
            } else
          if (strncmp(&(str[i]),"PUTTAB", strlen("PUTTAB"))== 0)               /* %TAB : Tab */
        {
                  int j, k;

                  if (Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else {
                        for (k=0; k<6; k++) {
                          if (HasLabel == 0)
                            BufPutString (" ",0); /* cychen */
                          HasLabel = HasLabel/10;
                        };
                      }
                  else
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else 
                        BufPutString ("      ",0); /* cychen */

                  i += strlen("PUTTAB");

                } else
           if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* %IF : If ; syntax : %IF (condition) then_bloc [%ELSE else_bloc] %ENDIF */
            {
              i += strlen("IF");
              i += Eval_Type_Condition(&(str[i]), ptype);
            } else
          if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* %ELSE : Else */
            {
              i += strlen("ELSE");
              i += SkipToEndif(&(str[i]));  /* skip to the corresponding endif */
            } else
          if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* %ENDIF : End of If */
            {
              i += strlen("ENDIF");
            } else
          if (strncmp(&(str[i]),"SUBTYPE", strlen("SUBTYPE"))== 0)     /* %SUBTYPE :  find the next type for (CAST) */
	    {
		PTR_TYPE pt;
                pt = TYPE_BASE(ptype);
		if(pt) Tool_Unparse_Type(pt);
		i += strlen("SUBTYPE");
	     } else
          if (strncmp(&(str[i]),"BASETYPE", strlen("BASETYPE"))== 0)     /* %BASETYPE : Base Type Name Identifier */
            {
	      if (Check_Lang_Fortran_For_File(cur_proj))  /*16.12.11 podd*/
		BufPutString (ftype_name [type_index (TYPE_CODE (TYPE_BASE (ptype)))],0);
	      else
		{
		  PTR_TYPE pt;
		  pt = Find_BaseType(ptype);
		  if (pt)
		    {
		      Tool_Unparse_Type(pt);
		    } else{
		      /* printf("offeding node type node: %d\n", ptype->id);
		      Message("basetype not found",0);
                      */
		      }
		}
              i += strlen("BASETYPE");
            } else

          if (strncmp(&(str[i]),"FBASETYPE", strlen("FBASETYPE"))== 0)     /* %FBASETYPE : Base Type Name Identifier */
            {
		  PTR_TYPE pt;
		  pt = Find_BaseType2(ptype);
		  if (pt)
		    {
		      Tool_Unparse_Type(pt);
		    } else{
		      /* printf("offeding node type node: %d\n", ptype->id);
		      Message("basetype not found",0);
                      */
		      }
              i += strlen("FBASETYPE");
            } else


          if (strncmp(&(str[i]),"STAR", strlen("STAR"))== 0)    
            {
              PTR_TYPE pt;
              int flg;
              pt = ptype;
       /*       while (pt)        */
                {
                  if (TYPE_CODE(pt) == T_POINTER){
                    BufPutString ("*",0); 
		    flg = pt->entry.Template.dummy5;  
		    if(flg & BIT_RESTRICT) BufPutString(" restrict ",0);
		    if(flg & BIT_CONST) BufPutString(" const ",0);
		    if(flg & BIT_GLOBL) BufPutString(" global ",0);
		    if(flg & BIT_SYNC) BufPutString(" Sync ",0);
		    if(flg & BIT_VOLATILE) BufPutString(" volatile ",0);
		    }
                  else
                  if (TYPE_CODE(pt) == T_REFERENCE){
                    BufPutString ("&",0);
		    flg = pt->entry.Template.dummy5;  
		    if(flg & BIT_RESTRICT) BufPutString(" restrict ",0);
		    if(flg & BIT_CONST) BufPutString(" const ",0);
		    if(flg & BIT_GLOBL) BufPutString(" global ",0);
		    if(flg & BIT_SYNC) BufPutString(" Sync ",0);
		    if(flg & BIT_VOLATILE) BufPutString(" volatile ",0);
		    }
 /*                 else
                    break;
                  if(TYPE_CODE(pt) == T_MEMBER_POINTER)
                          pt = TYPE_COLL_BASE(pt);
                  else pt = TYPE_BASE(pt);           */
                } 
              i += strlen("STAR");
            } else
          if (strncmp(&(str[i]),"RANGES", strlen("RANGES"))== 0)         /* %RANGES : Ranges */
            {
              Tool_Unparse2_LLnode (TYPE_RANGES (ptype));
	      if(TYPE_KIND_LEN(ptype)){
	        BufPutString("(",0);
	        Tool_Unparse2_LLnode (TYPE_KIND_LEN(ptype));
		BufPutString(")",0);
	      }
              i += strlen("RANGES");
            } else
          if (strncmp(&(str[i]),"NAMEID", strlen("NAMEID"))== 0)         /* %NAMEID : Name Identifier */
            {
	      if (ptype->name)
		BufPutString ( ptype->name->ident,0);
	      else
		{
		  BufPutString ("-------TYPE ERROR (NAMEID)------",0);
		}
              i += strlen("NAMEID");
            } else
	 if (strncmp(&(str[i]),"SYMBID", strlen("SYMBID"))== 0)         /* %NAMEID : Name Identifier */
		{
		  if (TYPE_SYMB_DERIVE(ptype)){
		    PTR_SYMB cname;
		    cname = TYPE_SYMB_DERIVE(ptype);
                    if(TYPE_CODE(ptype) == T_DERIVED_TYPE){
			if((SYMB_CODE(cname) == STRUCT_NAME) && (SYMB_TYPE(cname) == NULL) 
                           &&(BIF_CODE(SYMB_SCOPE(cname)) == GLOBAL))
				BufPutString("struct ", 0);
                        if((SYMB_CODE(cname) == CLASS_NAME) && (SYMB_TYPE(cname) == NULL) 
                           &&(BIF_CODE(SYMB_SCOPE(cname)) == GLOBAL))
				BufPutString("class ", 0);
                        if((SYMB_CODE(cname) == UNION_NAME) && (SYMB_TYPE(cname) == NULL) 
                           &&(BIF_CODE(SYMB_SCOPE(cname)) == GLOBAL))
				BufPutString("union ", 0);
			}
                     if(TYPE_SCOPE_SYMB_DERIVE(ptype) && TYPE_CODE(ptype) != T_DERIVED_TEMPLATE && TYPE_CODE(ptype) != T_DERIVED_COLLECTION) {
                         Tool_Unparse_Symbol(TYPE_SCOPE_SYMB_DERIVE(ptype));
                         BufPutString("::",0);
                         }
		    Tool_Unparse_Symbol(cname);
		    }
                  else if(TYPE_CODE(ptype) == T_MEMBER_POINTER)
                    Tool_Unparse_Symbol(TYPE_COLL_NAME(ptype));
		  else
		    {
                      printf("node = %d, variant = %d\n",TYPE_ID(ptype), TYPE_CODE(ptype));
		      BufPutString ("-------TYPE ERROR (ISYMBD)------",0);
		    }
		  i += strlen("SYMBID");
		} else
          if (strncmp(&(str[i]),"RANGLL1", strlen("RANGLL1"))== 0)       /* %RANGLL1 : Low Level Node 1 of Ranges */
            {
              if (TYPE_RANGES (ptype))
                Tool_Unparse2_LLnode (NODE_TEMPLATE_LL1 (TYPE_RANGES (ptype)));
              i += strlen("RANGLL1");
            } else
          if (strncmp(&(str[i]),"COLLBASE", strlen("COLLBASE"))== 0)       /* %COLL BASE */
            {
              if (TYPE_COLL_BASE(ptype))
		Tool_Unparse_Type(TYPE_COLL_BASE(ptype));
              i += strlen("COLLBASE");
            }  else
          if (strncmp(&(str[i]),"TMPLARGS", strlen("TMPLARGS"))== 0)       /* %RANGLL1 : Low Level Node 1 of Ranges */
            {
              if (TYPE_TEMPL_ARGS(ptype))
		Tool_Unparse2_LLnode(TYPE_TEMPL_ARGS(ptype));
              i += strlen("TMPLARGS");
            }  else
          Message  (" *** Unknown type node COMMAND *** ",0);
        }

       else
         {
           BufPutChar (c);
           i++;
         }
      c = str[i];
    }
  return  Buf_address;
}


char *
Tool_Unparse2_LLnode(ll)
     PTR_LLND ll;
{
  int variant;
  int kind;
  char *str;
  char c;
  int i;
  
  if (!ll)
    return NULL;
  
  variant = NODE_CODE (ll);
  kind = (int) node_code_kind[(int) variant];
  if (kind != (int)LLNODE)
    {
      Message("Error in Unparse, not a llnd node",0);
      BufPutInt(variant);
      BufPutString ("------ERROR--------",0);
      return NULL;
    }
  
  str = Unparse_Def[variant].str;
  
  /* now we have to interpret the code to unparse it */
  
  if (str == NULL)
    return NULL;
  if (strcmp( str, "n") == 0)
    return NULL;

  i = 0 ;
  c = str[i];
  while (c != '\0')
    {
      if (c == '%')
        {
          i++;
          c = str[i];
          /******** WE HAVE TO INTERPRET THE COMMAND *********/
          if (c  == '%')                                                 /* %% : Percent Sign */
            {
              BufPutString ("%",0);
              i++;
            } else
          if (strncmp(&(str[i]),"ERROR", strlen("ERROR"))== 0)           /* %ERROR : Generate error message */
            {
              Message ("--- unparsing error[0] : ",0);
	      BufPutInt(variant);
              BufPutString ("------ERROR--------",0);
              i += strlen("ERROR");
            } else
          if (strncmp(&(str[i]),"NL", strlen("NL"))== 0)                 /* %NL : NewLine */
            {
	      /*  int j;*/ /* podd 15.03.99*/
              BufPutChar ('\n');
/*              for (j = 0; j < TabNumber; j++)
                if (j>1)
                  BufPutString ("   ",0);
                else
                  BufPutString ("       ",0);*/
               i += strlen("NL");
            } else
          if (strncmp(&(str[i]),"TAB", strlen("TAB"))== 0)               /* %TAB : Tab */
            {
              BufPutString ("      ",0); /* cychen */
              i += strlen("TAB");
            } else
          if (strncmp(&(str[i]),"DELETE_COMMA", strlen("DELETE_COMMA"))== 0)               /* %DELETE_COMMA : , */
            {
	      if (Buf_address[Buf_pointer-1]==',') 
			{
			  Buf_address[Buf_pointer-1]=' ';
			  Buf_pointer--;	
			}
			  i += strlen("DELETE_COMMA");
            } else
          if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* %IF : If ; syntax : %IF (condition) then_bloc [%ELSE else_bloc] %ENDIF */
            {
              i += strlen("IF");
              i += Eval_LLND_Condition(&(str[i]), ll);
            } else
          if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* %ELSE : Else */
            {
              i += strlen("ELSE");
              i += SkipToEndif(&(str[i]));  /* skip to the corresponding endif */
            } else
          if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* %ENDIF : End of If */
            {
              i += strlen("ENDIF");
            } else
          if (strncmp(&(str[i]),"LL1", strlen("LL1"))== 0)               /* %LL1 : Low Level Node 1 */
            {
              Tool_Unparse2_LLnode(NODE_TEMPLATE_LL1(ll));
              i += strlen("LL1");
            } else
          if (strncmp(&(str[i]),"LL2", strlen("LL2"))== 0)               /* %LL2 : Low Level Node 2 */
            {
              Tool_Unparse2_LLnode(NODE_TEMPLATE_LL2(ll));
              i += strlen("LL2");
            } else
          if (strncmp(&(str[i]),"SYMBID", strlen("SYMBID"))== 0)         /* %SYMBID : Symbol identifier */
            {
              Tool_Unparse_Symbol (NODE_SYMB (ll));
              i += strlen("SYMBID");
            } else
          if (strncmp(&(str[i]),"DOPROC", strlen("DOPROC"))== 0)             /* for subclass qualification */
            { int flg;
              if(NODE_TYPE(ll) && (NODE_CODE(NODE_TYPE(ll)) == T_DESCRIPT)){
		flg = (NODE_TYPE(ll))->entry.Template.dummy5;
		if(flg & BIT_VIRTUAL) BufPutString(" virtual ",0);
		if(flg & BIT_ATOMIC) BufPutString(" atomic ",0);
		if(flg & BIT_PRIVATE) BufPutString(" private ",0);
		if(flg & BIT_PROTECTED) BufPutString(" protected ",0);
		if(flg & BIT_PUBLIC) BufPutString(" public ",0);
		}
	       else BufPutString(" public ", 0);
		/* note: this last else condition is to fix a bug in
		   the dep2C++ which does not create the right types
		   when converting a collection to a class.
                */
              i += strlen("DOPROC");
            } else
          if (strncmp(&(str[i]),"TYPE", strlen("TYPE"))== 0)             /* %TYPE : Type */
            { 
              if(NODE_SYMB(ll) &&  (SYMB_ATTR(NODE_SYMB(ll)) & OVOPERATOR)){
		  /* this is an overloaded operator.  don't do type */
		}
              else{ Tool_Unparse_Type (NODE_TYPE (ll)); }
              i += strlen("TYPE");
            } else
          if (strncmp(&(str[i]),"L1SYMBCST", strlen("L1SYMBCST"))== 0)   /* %L1SYMBCST  : Constant Value of Low Level Node Symbol */
            {
              if (NODE_TEMPLATE_LL1 (ll) && NODE_SYMB (NODE_TEMPLATE_LL1 (ll)))
                {
                   Tool_Unparse2_LLnode((NODE_SYMB (NODE_TEMPLATE_LL1 (ll)))->entry.const_value);
                }
              i += strlen("L1SYMBCST");
            } else
          if (strncmp(&(str[i]),"INTKIND", strlen("INTKIND"))== 0)         /* %INTKIND : Integer Value */
            { PTR_LLND kind;
              if (NODE_INT_CST_LOW (ll) < 0)
                 BufPutString ("(",0);
              BufPutInt (NODE_INT_CST_LOW (ll));
	      if( ( kind=TYPE_KIND_LEN(NODE_TYPE(ll)) ) ) {
                 BufPutString ("_",0);
                 Tool_Unparse2_LLnode(kind);
              }
              if (NODE_INT_CST_LOW (ll) < 0)
                    BufPutString (")",0);
                  
              i += strlen("INTKIND");
            } else
          if (strncmp(&(str[i]),"STATENO", strlen("STATENO"))== 0)       /* %STATENO : Statement number */
            {
              if (NODE_LABEL (ll))
                {
                  BufPutInt ( LABEL_STMTNO (NODE_LABEL (ll)));
                }
              i += strlen("STATENO");
            } else
          if (strncmp(&(str[i]),"LABELNAME", strlen("LABELNAME"))== 0)       /* %LABELNAME : Statement label *//*podd 06.01.13*/
            {
              if (NODE_LABEL (ll))
                {
                  BufPutString ( SYMB_IDENT(LABEL_SYMB (NODE_LABEL (ll))),0); 
                }
              i += strlen("LABELNAME");
            } else
          if (strncmp(&(str[i]),"KIND", strlen("KIND"))== 0)         /* %KIND : KIND parameter */
            { PTR_LLND kind;
	    if( ( kind=TYPE_KIND_LEN(NODE_TYPE(ll)) ) ) {
                 BufPutString ("_",0);
                 Tool_Unparse2_LLnode(kind);
            }
              i += strlen("KIND");
            } else
          if (strncmp(&(str[i]),"STRKIND", strlen("STRKIND"))== 0)         /* %STRKIND : KIND parameter of String Value */
            { PTR_LLND kind;
	    if( ( kind=TYPE_KIND_LEN(NODE_TYPE(ll)) ) ) {
                 Tool_Unparse2_LLnode(kind);
                 BufPutString ("_",0);
            }
              i += strlen("STRKIND");
            } else
          if (strncmp(&(str[i]),"SYMQUOTE", strlen("SYMQUOTE"))== 0)         /* %SYMQUOTE : first Symbol of String Value:" or ' */
            { 
	    if( ( TYPE_QUOTE(NODE_TYPE(ll)) == 2 ) ) {
                 BufPutChar ('\"');
            } else
                 BufPutChar ('\'');
              i += strlen("SYMQUOTE");

            } else
          if (strncmp(&(str[i]),"STRVAL", strlen("STRVAL"))== 0)         /* %STRVAL : String Value */
            {
              BufPutString (NODE_STR (ll),0);
              i += strlen("STRVAL");
            } else
          if (strncmp(&(str[i]),"BOOLVAL", strlen("BOOLVAL"))== 0)       /* %BOOLVAL : String Value */
            {
              BufPutString (NODE_BV (ll) ? ".TRUE." : ".FALSE.",0);
              i += strlen("BOOLVAL");
            } else
          if (strncmp(&(str[i]),"CHARVAL", strlen("CHARVAL"))== 0)       /* %CHARVAL : Char Value */
            {
	      switch(NODE_CV(ll)){
    		     case '\n':BufPutChar('\\'); BufPutChar('n'); break;
    		     case '\t':BufPutChar('\\'); BufPutChar('t'); break;
    		     case '\r':BufPutChar('\\'); BufPutChar('r'); break;
		     case '\f':BufPutChar('\\'); BufPutChar('f'); break;
    		     case '\b':BufPutChar('\\'); BufPutChar('b'); break;
    		     case '\a':BufPutChar('\\'); BufPutChar('a'); break;
    		     case '\v':BufPutChar('\\'); BufPutChar('v'); break;
		    default:
              		BufPutChar (NODE_CV (ll));
		    }
              i += strlen("CHARVAL");
            } else
          if (strncmp(&(str[i]),"ORBCPL1", strlen("ORBCPL1"))== 0)         /* %ORBCPL1 : Openning Round Brackets on Precedence of Low Level Node 1 for C++*/
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL1 (ll));
              if (C_op (llvar) && (precedence_C [variant - EQ_OP] < precedence_C [llvar - EQ_OP])) 
                BufPutString ("(",0);
              i += strlen("ORBCPL1");
            } else
          if (strncmp(&(str[i]),"CRBCPL1", strlen("CRBCPL1"))== 0)         /* %CRBCPL1 : Closing Round Brackets on Precedence of Low Level Node 1 for C++ */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL1 (ll));
              if (C_op (llvar) && (precedence_C [variant - EQ_OP] < precedence_C [llvar - EQ_OP]))
                BufPutString (")",0);
              i += strlen("CRBCPL1");
            } else
          if (strncmp(&(str[i]),"ORBCPL2", strlen("ORBCPL2"))== 0)         /* %ORBCPL2 : Openning Round Brackets on Precedence of Low Level Node 2 for C++ */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
              if (C_op (llvar) && (precedence_C [variant - EQ_OP] <= precedence_C [llvar - EQ_OP]))
                BufPutString ("(",0);
              i += strlen("ORBCPL2");
            } else
          if (strncmp(&(str[i]),"CRBCPL2", strlen("CRBCPL2"))== 0)         /* %CRBCPL2 : Closing Round Brackets on Precedence of Low Level Node 2 for C++ */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
              if (C_op (llvar) && (precedence_C [variant - EQ_OP] <= precedence_C [llvar - EQ_OP]))             
                BufPutString (")",0);
              i += strlen("CRBCPL2");
            } else
        
          if (strncmp(&(str[i]),"ORBPL1", strlen("ORBPL1"))== 0)         /* %ORBPL1 : Openning Round Brackets on Precedence of Low Level Node 1 */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL1 (ll));
              if (binop (llvar) && (precedence [variant - EQ_OP] < precedence [llvar - EQ_OP]))
                BufPutString ("(",0);
              i += strlen("ORBPL1");
            } else
          if (strncmp(&(str[i]),"CRBPL1", strlen("CRBPL1"))== 0)         /* %CRBPL1 : Closing Round Brackets on Precedence of Low Level Node 1 */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL1 (ll));
              if (binop (llvar) && (precedence [variant - EQ_OP] < precedence [llvar - EQ_OP]))
                BufPutString (")",0);
              i += strlen("CRBPL1");
            } else
          if (strncmp(&(str[i]),"ORBPL2", strlen("ORBPL2"))== 0)         /* %ORBPL2 : Openning Round Brackets on Precedence of Low Level Node 2 */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
              if (binop (llvar) && (precedence [variant - EQ_OP] <= precedence [llvar - EQ_OP]))
                BufPutString ("(",0);
              i += strlen("ORBPL2");
            } else
          if (strncmp(&(str[i]),"CRBPL2", strlen("CRBPL2"))== 0)         /* %CRBPL2 : Closing Round Brackets on Precedence of Low Level Node 2 */
            {
              int llvar = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
              if (binop (llvar) && (precedence [variant - EQ_OP] <= precedence [llvar - EQ_OP]))
                BufPutString (")",0);
              i += strlen("CRBPL2");
            } else
	 if (strncmp(&(str[i]),"SETFLAG", strlen("SETFLAG"))== 0)
	   {               
		i = i + strlen("SETFLAG");
		Treat_Flag(str, &i,1);
	   } else
	 if (strncmp(&(str[i]),"UNSETFLAG", strlen("UNSETFLAG"))== 0) 
	   {               
		i = i + strlen("UNSETFLAG");
		Treat_Flag(str, &i,-1);
	   } else
         if (strncmp(&(str[i]),"PUSHFLAG", strlen("PUSHFLAG"))== 0)
           {               
                i = i + strlen("PUSHFLAG");
                PushPop_Flag(str, &i,1);
           } else
         if (strncmp(&(str[i]),"POPFLAG", strlen("POPFLAG"))== 0) 
            {               
                i = i + strlen("POPFLAG");
                PushPop_Flag(str, &i,-1);
            } else
         if (strncmp(&(str[i]),"PURE", strlen("PURE"))== 0) /* for pure function declarations */
	{
	   PTR_SYMB symb;
	   symb = NODE_SYMB(ll);
	   if(symb && (SYMB_TEMPLATE_DUMMY8(symb) &  128)) BufPutString ("= 0",0);
           i += strlen("PURE");
           }
          else
          if (strncmp(&(str[i]),"CNSTF", strlen("CNSTF"))== 0)    /* for const functions */
           {
              PTR_SYMB symb;
              if (NODE_SYMB (ll)){
                symb = BIF_SYMB (ll);
		if(SYMB_TEMPLATE_DUMMY8(symb) & 64) BufPutString(" const",0); 
		}
	      i += strlen("CNSTF");
             } else        
         if (strncmp(&(str[i]),"CNSTCHK", strlen("CNSTCHK"))== 0) /* do "const", vol" after * */
	{
	   int flg;
           PTR_TYPE t;
           if((t = NODE_TYPE(ll)) &&( (NODE_CODE(t) == T_POINTER) ||
		(NODE_CODE(t) == T_REFERENCE))){
		flg = t->entry.Template.dummy5;  
		    if(flg & BIT_RESTRICT) BufPutString(" restrict ",0);
		    if(flg & BIT_CONST) BufPutString(" const ",0);
		    if(flg & BIT_GLOBL) BufPutString(" global ",0);
		    if(flg & BIT_SYNC) BufPutString(" Sync ",0);
		if(flg & BIT_VOLATILE) BufPutString(" volatile ",0);
		}
           i += strlen("CNSTCHK");
           }
          else
         if (strncmp(&(str[i]),"VARLISTTY", strlen("VARLISTTY"))== 0)  /* %VARLIST : list of variables / parameters */
            {
              PTR_SYMB symb, s;
	      PTR_LLND args, arg_item = NULL, t;
	      PTR_TYPE typ;
              int new_op_flag; /* 1 if this is a new op */
              new_op_flag = 0;
              if(NODE_CODE(ll) == CAST_OP ){
			args = NODE_OPERAND1(ll);
			new_op_flag = 1;
                        }
	      else if(NODE_CODE(ll) != FUNCTION_OP){ 
			args = NODE_OPERAND0(ll);
			/* symb = SYMB_FUNC_PARAM(NODE_SYMB(ll)); */
		  	}
              else {  /* this is a pointer to a function parameter */
			args = NODE_OPERAND1(ll);
			t = NODE_OPERAND0(ll); /* node_code(t) == deref_op */
			t = NODE_OPERAND0(t);  /* node_code(t) == var_ref */
			s = NODE_SYMB(t);
			if(s) symb = SYMB_NEXT(s);
			else symb = NULL;
		}	
              while (args )
                      { 
                        int typflag;
			if(new_op_flag) t = args;
			else{
				arg_item = NODE_OPERAND0(args);
				t = arg_item;
                        	typflag = 1;
				while(t && typflag){
                           		if((NODE_CODE(t) == VAR_REF) || (NODE_CODE(t) == ARRAY_REF))
                                    		typflag = 0;
                           	else if (NODE_CODE(t) == SCOPE_OP) t = NODE_OPERAND1(t);
                           	else  t = NODE_OPERAND0(t);
                           } 
			  }
 			if(t){
			      symb = NODE_SYMB(t); 
			      typ = NODE_TYPE(t);
			      if(symb && (typ == NULL)) typ = SYMB_TYPE(symb);
			      if(new_op_flag || symb  ) {
                                 typflag = 1;
			         while(typ && typflag){
                                          if(TYPE_CODE(typ) == T_ARRAY || 
				             TYPE_CODE(typ) == T_FUNCTION ||
			                     TYPE_CODE(typ) == T_REFERENCE ||
				             TYPE_CODE(typ) == T_POINTER) typ = TYPE_BASE(typ);
				          else 	if(TYPE_CODE(typ) == T_MEMBER_POINTER)
                                                      typ = TYPE_COLL_BASE(typ);
					  else typflag = 0;
				          } 
                                   }
                              if(typ) Tool_Unparse_Type (typ);
                              BufPutString (" ",0);
			   }
			else printf("unp could not find var ref!\n");
			if(new_op_flag){
			        Tool_Unparse2_LLnode(args);
				args = LLNULL;
				new_op_flag = 0;
				}
			else{
			     Tool_Unparse2_LLnode(arg_item);
			     args = NODE_OPERAND1(args);
			    }
                        if (args) BufPutString (", ",0);
                      }
              i += strlen("VARLISTTY");
            } 
	      else 
	   if (strncmp(&(str[i]),"VARLIST", strlen("VARLIST"))== 0)    /* %VARLIST : list of variables / parameters */
		  {
		    PTR_SYMB symb;
		    if (NODE_SYMB (ll))
		      symb = SYMB_FUNC_PARAM (NODE_SYMB (ll));
		    else
		      symb = NULL;
		    while (symb)
		      {
			BufPutString ( SYMB_IDENT (symb),0);
			symb = SYMB_NEXT_DECL (symb);
			if (symb) BufPutString (", ",0);
		      }
		    i += strlen("VARLIST");
		  } else
    	    if (strncmp(&(str[i]),"STRINGLEN", strlen("STRINGLEN"))== 0)
		      {
			PTR_SYMB symb;
			PTR_TYPE type;
			if (NODE_SYMB (ll))
			  symb = NODE_SYMB (ll);
			else
			  symb = NULL;
			if (symb)
			  {
			    type = SYMB_TYPE(symb);
			    if (type && (TYPE_CODE(type) == T_ARRAY))
			      {
				type = Find_BaseType(type);
			      }
			    if (type && (TYPE_CODE(type) == T_STRING))
			      {
				if (TYPE_RANGES(type))
				  Tool_Unparse2_LLnode(TYPE_RANGES(type));
			      }
			  }
			i += strlen("STRINGLEN");
      
		 }  else
			Message  (" *** Unknown low level node COMMAND *** ",0);
	}
      else
	{
	  BufPutChar ( c);
	  i++; /* Bodin */
	}
      c = str[i];
    }
  return Buf_address;
}

char *Tool_Unparse_Bif(PTR_BFND bif)
{
    int variant;
    int kind;
    char *str;
    char c;
    int i;

    if (!bif)
        return NULL;

    variant = BIF_CODE(bif);
#ifdef __SPF
    if (variant < 0)
        return NULL;
#endif
    kind = (int) node_code_kind[(int) variant];
    if (kind  != (int)BIFNODE)
        Message("Error in Unparse, not a bif node", 0);
    if (BIF_LINE(bif) == -1)
        BufPutString("!$", 0);
    //if (BIF_DECL_SPECS(bif) == BIT_OPENMP) BufPutString("!$",0); 
    str = Unparse_Def[variant].str;
    /*printf("variant = %d, str = %s\n", variant, str);*/
    /* now we have to interpret the code to unparse it */
  
    if (str == NULL)
        return NULL;
    if (strcmp( str, "n") == 0)
        if (strcmp(str, "n") == 0)
        {
            Message("Node not define for unparse", BIF_LINE(bif));
            return NULL;
        }


  i = 0 ;
  c = str[i];
  while ((c != '\0') && (c != '\n'))
    {
      if (c == '%')
        {
          i++;
          c = str[i];
          /******** WE HAVE TO INTERPRET THE COMMAND *********/
          if (c  == '%')                                                 /* %% : Percent Sign */
            {
              BufPutString ("%",0);
              i++;
            } else
          if (strncmp(&(str[i]),"CMNT", strlen("CMNT"))== 0)
            {               
              i = i + strlen("CMNT");
              if (!CommentOut)
              {
                  /* print the attached comment first */
                  if (BIF_CMNT(bif))
                  {
                      /*  int j;*/ /* podd 15.03.99*/
                      if (CMNT_STRING(BIF_CMNT(bif)))
                      {
                          BufPutChar('\n');
                          BufPutString(CMNT_STRING(BIF_CMNT(bif)), 0);
                          if (!Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
                              BufPutChar('\n');
                      }
                  }
              }
            } else
          if (strncmp(&(str[i]),"DECLSPEC", strlen("DECLSPEC"))== 0)             /* %DECLSPEC : for extern, static, inline, friend */
            {
              int index = BIF_DECL_SPECS(bif);
              i = i + strlen("DECLSPEC");
              if( index & BIT_EXTERN) {
		 BufPutString(ridpointers[(int)RID_EXTERN],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_STATIC) {
		 BufPutString(ridpointers[(int)RID_STATIC],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_INLINE) {
	         BufPutString(ridpointers[(int)RID_INLINE],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_FRIEND) {
		 BufPutString(ridpointers[(int)RID_FRIEND],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_CUDA_GLOBAL) {
		 BufPutString(ridpointers[(int)RID_CUDA_GLOBAL],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_CUDA_SHARED) {
		 BufPutString(ridpointers[(int)RID_CUDA_SHARED],0);
		 BufPutString(" ", 0);
		 }
              if( index & BIT_CUDA_DEVICE) {
		 BufPutString(ridpointers[(int)RID_CUDA_DEVICE],0);
		 BufPutString(" ", 0);
                 }
              if (index & BIT_CONST) {
                 BufPutString(ridpointers[(int)RID_CONST], 0);
                 BufPutString(" ", 0);
                 }
            } else
          if (strncmp(&(str[i]),"SETFLAG", strlen("SETFLAG"))== 0)
            {               
              i = i + strlen("SETFLAG");
              Treat_Flag(str, &i,1);
            } else
          if (strncmp(&(str[i]),"UNSETFLAG", strlen("UNSETFLAG"))== 0) 
            {               
              i = i + strlen("UNSETFLAG");
              Treat_Flag(str, &i,-1);
            } else
          if (strncmp(&(str[i]),"PUSHFLAG", strlen("PUSHFLAG"))== 0)
                        {               
                          i = i + strlen("PUSHFLAG");
                          PushPop_Flag(str, &i,1);
                        } else
                          if (strncmp(&(str[i]),"POPFLAG", strlen("POPFLAG"))== 0) 
                            {               
                              i = i + strlen("POPFLAG");
                              PushPop_Flag(str, &i,-1);
                            } else
          if (strncmp(&(str[i]),"ERROR", strlen("ERROR"))== 0)           /* %ERROR : Generate error message */
            {
              Message("--- stmt unparsing error[1] : ",0);
              i += strlen("ERROR");
              BufPutString (" *** UNPARSING ERROR OCCURRED HERE ***\n",0);
            } else
          if (strncmp(&(str[i]),"NL", strlen("NL"))== 0)                 /* %NL : NewLine */
            { /*int j; */ /* podd 15.03.99*/             
              BufPutChar ('\n');
/*              for (j = 0; j < TabNumber; j++)
                if (j>1)
                  BufPutString ("   ",0);
                else
                  BufPutString ("       ",0);*/
              i += strlen("NL");
            } else
          if (strncmp(&(str[i]),"NOTABNL", strlen("NOTABNL"))== 0)                 /* %NL : NewLine */
            {
              BufPutChar ('\n');
              i += strlen("NOTABNL");
            } else
          if (strncmp(&(str[i]),"TABOFF", strlen("TABOFF"))== 0)               /* turn off tabulation */
            {
              TabNumberCopy = TabNumber;
              TabNumber = 0;              
              i += strlen("TABOFF");
            }  else
          if (strncmp(&(str[i]),"TABON", strlen("TABON"))== 0)                 /* turn on tabulation */
            {
              TabNumber  = TabNumberCopy;              
              i += strlen("TABON");
            }  else
          if (strncmp(&(str[i]),"TAB", strlen("TAB"))== 0)               /* %TAB : Tab */
                {
              BufPutString ("      ",0); /* cychen */
              i += strlen("TAB");
            } else
          if (strncmp(&(str[i]),"PUTTABCOMT", strlen("PUTTABCOMT"))== 0)               /* %TAB : Tab */
            {
                  int j, k;
                  if (Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else {
                        for (k=0; k<6; k++) {
                          if (HasLabel == 0)
                            BufPutString (" ",0); /* cychen */
                          HasLabel = HasLabel/10;
                        };
		  Buf_pointer-=5;
                      }
                  else
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else
                        BufPutString ("      ",0); /* cychen */

                  i += strlen("PUTTABCOMT");
            } else
          if (strncmp(&(str[i]),"PUTTAB", strlen("PUTTAB"))== 0)               /* %TAB : Tab */
            {
                  int j, k;
 
                  if (Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else {
                        for (k=0; k<6; k++) {
                          if (HasLabel == 0)
                            BufPutString (" ",0); /* cychen */
                          HasLabel = HasLabel/10;
                        };
                      }
                  else
                    for (j = 0; j < TabNumber; j++)
                      if (j>0)
                        BufPutString ("   ",0);
                      else
                        BufPutString ("      ",0); /* cychen */

                  i += strlen("PUTTAB");

            } else
          if (strncmp(&(str[i]),"INCTAB", strlen("INCTAB"))== 0)               /* increment tab */
            {
              TabNumber++;              
              i += strlen("INCTAB");
            }  else
          if (strncmp(&(str[i]),"DECTAB", strlen("DECTAB"))== 0)               /*deccrement tab */
            {
	      if (Check_Lang_Fortran_For_File(cur_proj)) /*16.12.11 podd*/
		{
		  if (TabNumber>1)
		    TabNumber--;              
		} else
		  TabNumber--; 
              i += strlen("DECTAB");
            }  else
          if (strncmp(&(str[i]),"IF", strlen("IF"))== 0)                 /* %IF : If ; syntax : %IF (condition) then_bloc [%ELSE else_bloc] %ENDIF */
            {
              i += strlen("IF");
              i += Eval_Bif_Condition(&(str[i]), bif);
            } else
          if (strncmp(&(str[i]),"ELSE", strlen("ELSE"))== 0)             /* %ELSE : Else */
            {
              i += strlen("ELSE");
              i += SkipToEndif(&(str[i]));  /* skip to the corresponding endif */
            } else
          if (strncmp(&(str[i]),"ENDIF", strlen("ENDIF"))== 0)           /* %ENDIF : End of If */
            {
              i += strlen("ENDIF");
            } else
          if (strncmp(&(str[i]),"BLOB1", strlen("BLOB1"))== 0)           /* %BLOB1 : All Blob 1 */
            {
              PTR_BLOB blob;

              for (blob = BIF_BLOB1(bif);blob; blob = BLOB_NEXT (blob))
                {
                  Tool_Unparse_Bif(BLOB_VALUE(blob));
                }
              i += strlen("BLOB1");
            } else
          if (strncmp(&(str[i]),"BLOB2", strlen("BLOB2"))== 0)           /* %BLOB2 : All Blob 2 */
            {
              PTR_BLOB blob;

              for (blob = BIF_BLOB2(bif);blob; blob = BLOB_NEXT (blob))
                {
                  Tool_Unparse_Bif(BLOB_VALUE(blob));
                }
              i += strlen("BLOB2");
            } else
          if (strncmp(&(str[i]),"LL1", strlen("LL1"))== 0)               /* %LL1 : Low Level Node 1 */
            {
              Tool_Unparse2_LLnode(BIF_LL1(bif));
              i += strlen("LL1");
            } else
          if (strncmp(&(str[i]),"LL2", strlen("LL2"))== 0)               /* %LL2 : Low Level Node 2 */
            {
              Tool_Unparse2_LLnode (BIF_LL2 (bif));
              i += strlen("LL2");
            } else
          if (strncmp(&(str[i]),"LL3", strlen("LL3"))== 0)               /* %LL3 : Low Level Node 3 */
            {
              Tool_Unparse2_LLnode(BIF_LL3(bif));
              i += strlen("LL3");
            } else
          if (strncmp(&(str[i]),"L2L2", strlen("L2L2"))== 0)          /* %L2L2 : Low Level Node 2 of Low Level Node 2 */
            {
              if (BIF_LL2 (bif))
                Tool_Unparse2_LLnode (NODE_TEMPLATE_LL2 (BIF_LL2 (bif)));
              i += strlen("L2L2");
            } else
          if (strncmp(&(str[i]),"FUNHD", strlen("FUNHD"))== 0)         /* %FUNHD track down a function header */
	    {
	       PTR_LLND p;
               p = BIF_LL1(bif);
	       while(p && NODE_CODE(p) != FUNCTION_REF) p = NODE_OPERAND0(p);
	       if(p == NULL) printf("unparse error in FUNHD!!\n");
	       else Tool_Unparse2_LLnode(p);
              i += strlen("FUNHD");
	    } else
          if (strncmp(&(str[i]),"SYMBIDFUL", strlen("SYMBIDFUL"))== 0)         /* %SYMBID : Symbol identifier */
            {
	      if (BIF_SYMB(bif) && SYMB_MEMBER_BASENAME(BIF_SYMB(bif)))
		{
		  Tool_Unparse_Symbol(SYMB_MEMBER_BASENAME(BIF_SYMB(bif)));
		  BufPutString("::",0);
		}
              Tool_Unparse_Symbol(BIF_SYMB(bif));
              i += strlen("SYMBIDFUL");
            } else
	  if (strncmp(&(str[i]),"SYMBID", strlen("SYMBID"))== 0)         /* %SYMBID : Symbol identifier */
            {
              Tool_Unparse_Symbol(BIF_SYMB(bif));
              i += strlen("SYMBID");
            } else
          if (strncmp(&(str[i]),"SYMBSCOPE", strlen("SYMBSCOPE"))== 0)         /* %SYMBSCOPE : Symbol identifier */
            {
	      if (BIF_SYMB(bif) && SYMB_MEMBER_BASENAME(BIF_SYMB(bif)))
		{ printf("SYMBSCOPE\n");
		  Tool_Unparse_Symbol(SYMB_MEMBER_BASENAME(BIF_SYMB(bif)));
		}
              i += strlen("SYMBSCOPE");
            } else
          if (strncmp(&(str[i]),"SYMBDC", strlen("SYMBDC"))== 0)         /* %SYMBSCOPE : Symbol identifier */
            {
	      if (BIF_LL3(bif) ||
                 (BIF_SYMB(bif) && SYMB_MEMBER_BASENAME(BIF_SYMB(bif))))
		{
		  BufPutString("::",0);
		}
              i += strlen("SYMBDC");
            } else
	      
          if (strncmp(&(str[i]),"STATENO", strlen("STATENO"))== 0)       /* %STATENO : Statement number */
            {
              if (BIF_LABEL_USE (bif))
                {
                  BufPutInt (LABEL_STMTNO (BIF_LABEL_USE (bif)));
                }
              i += strlen("STATENO");
            } else
          if (strncmp(&(str[i]),"LABELENDIF", strlen("LABELENDIF"))== 0)       /* %STATENO : Statement number */
                {
                  PTR_BFND temp;
                  PTR_BLOB blob;

                  temp = NULL;
                  if (!BIF_BLOB2(bif))
                    blob = BIF_BLOB1(bif);
                  else
                     blob = BIF_BLOB2(bif);
                  for (;blob; blob = BLOB_NEXT (blob))
                    {
                      temp = BLOB_VALUE(blob);
                      if (temp && (BIF_CODE(temp) == CONTROL_END))
                        {
                          if (BIF_LABEL(temp))
                            break;
                        }
                      temp = NULL;
                    }
                  if (temp && BIF_LABEL(temp))
                    {
                      BufPutInt (LABEL_STMTNO (BIF_LABEL(temp)));
                    }
              i += strlen("LABELENDIF");
            } else
          if (strncmp(&(str[i]),"LABNAME", strlen("LABNAME")) == 0)  /* %LABNAME for C labels: added by dbg */
            {  
              if(BIF_LABEL_USE(bif)){
                if(LABEL_SYMB(BIF_LABEL_USE(bif)))
			BufPutString (SYMB_IDENT(LABEL_SYMB(BIF_LABEL_USE(bif))), 0);
                else printf("label-symbol error\n");
              } else printf("label error\n");
             i += strlen("LABNAME");
            } else
          if (strncmp(&(str[i]),"LABEL", strlen("LABEL"))== 0)       /* %STATENO : Statement number */
            {
              if (BIF_LABEL(bif))
                {
                  HasLabel = LABEL_STMTNO (BIF_LABEL(bif));
                  BufPutInt (LABEL_STMTNO (BIF_LABEL(bif)));
                }
              i += strlen("LABEL");
            } else
          if (strncmp(&(str[i]),"SYMBTYPE", strlen("SYMBTYPE"))== 0)     /* SYMBTYPE : Type of Symbol */
            {
              if (BIF_SYMB (bif) && SYMB_TYPE (BIF_SYMB (bif)))
		{
		  if (Check_Lang_Fortran_For_File(cur_proj))/*16.12.11 podd*/
                  BufPutString ( ftype_name [type_index (TYPE_CODE (SYMB_TYPE (BIF_SYMB (bif))))],0);
		  else if((SYMB_ATTR(BIF_SYMB(bif)) & OVOPERATOR ) == 0){
			PTR_LLND el;
                        el = BIF_LL1(bif);
			if((BIF_CODE(BIF_CP(bif)) == TEMPLATE_FUNDECL) &&
			    el && NODE_TYPE(el))
			    Tool_Unparse_Type(NODE_TYPE(el));
			else
		           Tool_Unparse_Type(SYMB_TYPE (BIF_SYMB (bif)));
			}
		}
              i += strlen("SYMBTYPE");
            } else
          if (strncmp(&(str[i]),"CNSTF", strlen("CNSTF"))== 0)    /* for const functions */
            {
              PTR_SYMB symb;
              if (BIF_SYMB (bif)){
                symb = BIF_SYMB (bif);
	        /* if(SYMB_TEMPLATE_DUMMY8(symb) & 64) BufPutString(" const",0); */
		}
	      i += strlen("CNSTF");
             } else        
          if (strncmp(&(str[i]),"VARLISTTY", strlen("VARLISTTY"))== 0)       /* %VARLIST : list of variables / parameters */
            {
              PTR_SYMB symb;
              if (BIF_SYMB (bif))
                symb = SYMB_FUNC_PARAM (BIF_SYMB (bif));
              else
                symb = NULL;
              while (symb)
                {
		  Tool_Unparse_Type (SYMB_TYPE(symb));
		  BufPutString (" ",0);
                  BufPutString ( SYMB_IDENT (symb),0);
                  symb = SYMB_NEXT_DECL (symb);
                  if (symb) BufPutString (", ",0);
                }
              i += strlen("VARLISTTY");
            } else
          if (strncmp(&(str[i]),"TMPLARGS", strlen("TMPLARGS"))== 0)       
            {
              PTR_SYMB symb;
              /* PTR_SYMB s; */ /* podd 15.03.99*/
	      PTR_LLND args, arg_item, t;
	      PTR_TYPE typ;
              if(BIF_CODE(bif) == FUNC_HEDR) args = BIF_LL3(bif);
              else args = BIF_LL1(bif);
              while (args )
                      { 
                        int typflag;
			arg_item = NODE_OPERAND0(args);
			if(arg_item == NULL) printf("MAJOR TEMPLATE UNPARSE ERROR. contact dbg \n");
			t = arg_item;
                        typflag = 1;
			while(t && typflag){
                           if((NODE_CODE(t) == VAR_REF) || (NODE_CODE(t) == ARRAY_REF))
                                    typflag = 0;
                           else if (NODE_CODE(t) == SCOPE_OP) t = NODE_OPERAND1(t);
                           else  t = NODE_OPERAND0(t);
                           } 
 			if(t){
			      symb = NODE_SYMB(t); 
			      typ = NODE_TYPE(t);
			      if(typ == NULL) typ = SYMB_TYPE(symb);
			      if((int)strlen(symb->ident) > 0){ /* special case for named arguments */
                                 typflag = 1;
			         while(typ && typflag){
                                          if(TYPE_CODE(typ) == T_ARRAY || 
				             TYPE_CODE(typ) == T_FUNCTION ||
			                     TYPE_CODE(typ) == T_REFERENCE ||
				             TYPE_CODE(typ) == T_POINTER) typ = TYPE_BASE(typ);
				          else 	if(TYPE_CODE(typ) == T_MEMBER_POINTER)
                                                      typ = TYPE_COLL_BASE(typ);
					  else typflag = 0;
				          } 
                                 }
			      else BufPutString("class ", 0); 
                              Tool_Unparse_Type (typ);
                              BufPutString (" ",0);
			   }
			/* else printf("could not find var ref!\n"); */
                        Tool_Unparse2_LLnode(arg_item);
			args = NODE_OPERAND1(args);
                        if (args) BufPutString (", ",0);
                      }
              i += strlen("TMPLARGS");
            } else
          if (strncmp(&(str[i]),"CONSTRU", strlen("CONSTRU"))== 0)       
            {
              /*PTR_SYMB symb;*/ /* podd 15.03.99*/
              PTR_LLND ll;
              if (BIF_LL1(bif))
                {
                  ll = NODE_OPERAND0(BIF_LL1(bif));
                  if (ll)
                    ll = NODE_OPERAND1(ll);
                  if (ll)
                    {
                      BufPutString (":",0);
                      Tool_Unparse2_LLnode(ll);
                    }
                }
              i += strlen("CONSTRU");
            } else
	  if (strncmp(&(str[i]),"L1SYMBID", strlen("L1SYMBID"))== 0)     /* %L1SYMBID : Symbol of Low Level Node 1 */
            {
              if (BIF_LL1 (bif))
                  Tool_Unparse_Symbol (NODE_SYMB (BIF_LL1 (bif)));
              i += strlen("L1SYMBID");
            } else
          if (strncmp(&(str[i]),"VARLIST", strlen("VARLIST"))== 0)       /* %VARLIST : list of variables / parameters */
            {
              PTR_SYMB symb;
              if (BIF_SYMB (bif))
                symb = SYMB_FUNC_PARAM (BIF_SYMB (bif));
              else
                symb = NULL;
              while (symb)
                {
                  BufPutString ( SYMB_IDENT (symb),0);
                  symb = SYMB_NEXT_DECL (symb);
                  if (symb) BufPutString (", ",0);
                }
              i += strlen("VARLIST");
            } else
	  if (strncmp(&(str[i]),"RIDPT", strlen("RIDPT"))== 0)      
                { 
		  PTR_TYPE type = NULL;

		  type = Find_Type_For_Bif(bif);
		  if (type )
		    {
			DealWith_Rid(type, In_Class_Flag);
		    }
		  else if(BIF_CODE(bif) == CLASS_DECL)
		    {
			DealWith_Rid(SYMB_TYPE(BIF_SYMB(bif)), In_Class_Flag);
		    } 
                  i += strlen("RIDPT");
                } else
          if (strncmp(&(str[i]),"INCLASSON", strlen("INCLASSON"))== 0)   
            {
              In_Class_Flag = 1;
              i += strlen("INCLASSON");
            } else
          if (strncmp(&(str[i]),"INCLASSOFF", strlen("INCLASSOFF"))== 0) 
            {
              In_Class_Flag = 0;
              i += strlen("INCLASSOFF");
            } else
          if (strncmp(&(str[i]),"INWRITEON", strlen("INWRITEON"))== 0)   /* %INWRITEON : In_Write_Statement Flag ON */
            {
              In_Write_Flag = 1;
              i += strlen("INWRITEON");
            } else
          if (strncmp(&(str[i]),"INWRITEOFF", strlen("INWRITEOFF"))== 0) /* %INWRITEOFF : In_Write_Statement Flag OFF */
            {
              In_Write_Flag = 0;
              i += strlen("INWRITEOFF");
            } else
          if (strncmp(&(str[i]),"RECPORTON", strlen("RECPORTON"))== 0)   /* %RECPORTON : recursive_port_decl Flag ON */
            {
              Rec_Port_Decl = 1;
              i += strlen("RECPORTON");
            } else
          if (strncmp(&(str[i]),"RECPORTOFF", strlen("RECPORTOFF"))== 0) /* %RECPORTOFF : recursive_port_decl Flag OFF */
            {
              Rec_Port_Decl = 0;
              i += strlen("RECPORTOFF");
            } else

          if (strncmp(&(str[i]),"INPARAMON", strlen("INPARAMON"))== 0)   /* %INPARAMON : In_Param_Statement Flag ON */
            {
              In_Param_Flag = 1;
              i += strlen("INPARAMON");
            } else
          if (strncmp(&(str[i]),"INPARAMOFF", strlen("INPARAMOFF"))== 0) /* %INPARAMOFF : In_Param_Statement Flag OFF */
            {
              In_Param_Flag = 0;
              i += strlen("INPARAMOFF");
            } else
          if (strncmp(&(str[i]),"INIMPLION", strlen("INIMPLION"))== 0)   /* %INIMPLION : In_Impli_Statement Flag ON */
            {
              In_Impli_Flag = 1;
              i += strlen("INIMPLION");
            } else
          if (strncmp(&(str[i]),"INIMPLIOFF", strlen("INIMPLIOFF"))== 0) /* %INIMPLIOFF : In_Impli_Statement Flag OFF */
            {
              In_Impli_Flag = 0;
              i += strlen("INIMPLIOFF");

            } else /*podd 3.02.03*/
          if (strncmp(&(str[i]),"SAVENAME", strlen("SAVENAME"))== 0) /* save construct name for ELSE and ENDIF */
            {
              construct_name = BIF_SYMB(bif);
              i += strlen("SAVENAME");
            } else /*podd 3.02.03*/
          if (strncmp(&(str[i]),"CNTRNAME", strlen("CNTRNAME"))== 0) /* save construct name for ELSE and ENDIF */
            {
              Tool_Unparse_Symbol(construct_name);
              i += strlen("CNTRNAME");

           } else
    	    if (strncmp(&(str[i]),"TYPEDECLON", strlen("TYPEDECLON"))== 0) /* %TYPEDECLON */
	   {       if( BIF_LL2(bif) && NODE_TYPE(BIF_LL2(bif)) &&  TYPE_CODE(NODE_TYPE(BIF_LL2(bif))) == T_STRING)
                     Type_Decl_Ptr = (long) NODE_TYPE(BIF_LL2(bif));
                   else
                     Type_Decl_Ptr = 0;
           i += strlen("TYPEDECLON");
           } else
    	    if (strncmp(&(str[i]),"TYPEDECLOF", strlen("TYPEDECLOF"))== 0) /* %TYPEDECLOF */
	    {    Type_Decl_Ptr = 0;
                 i += strlen("TYPEDECLOF");
            } else
	      if (strncmp(&(str[i]),"TYPE", strlen("TYPE"))== 0)
		{
		  PTR_TYPE type = NULL;
		  type = Find_Type_For_Bif(bif);
		  if (!type)
		    {
		      Message("TYPE not found",0);
		      BufPutString("------TYPE ERROR----",0);
		    }
		  if( !is_overloaded_type(bif) ) 
		      Tool_Unparse_Type (type);
              i += strlen("TYPE");
            } else
	      if (strncmp(&(str[i]),"PROTECTION", strlen("PROTECTION"))== 0)
		{
		  int protect = 0;
		  protect  = Find_Protection_For_Bif(bif);
		  if (protect)
		    {
		      if (protect & 128)
			{
			  /* BufPutString("MethodOfElement:\n",0);  a temporary fix until dep2C++ done */
                          BufPutString("public:\n", 0);
			} else
			  {
			    switch (protect)
			      { /* find the definition of the flag someday */
			      case 64:  BufPutString("public:\n",0); break;
			      case 32:  BufPutString("protected:\n",0); break;
			      case 16:  BufPutString("private:\n",0); break;
			      }
			  }
		    }
              i += strlen("PROTECTION");
            } else
              if (strncmp(&(str[i]),"DUMMY", strlen("DUMMY"))== 0) /* %DUMMY Do nothing */
            {
              i += strlen("DUMMY");

            } else
          Message (" *** Unknown bif node COMMAND *** ",0);
        }
       else
         {
           BufPutChar( c);
           i++;
         }
      c = str[i];
    }
  return Buf_address;
}

