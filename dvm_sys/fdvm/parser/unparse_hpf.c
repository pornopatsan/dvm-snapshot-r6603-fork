 /**************************************************************************
  *                                                                        *
  *   Unparser for toolbox                                                 *
  *                                                                        *
  *************************************************************************/

#include <stdio.h>

#include "compatible.h"   /* Make different system compatible... (PHB) */
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdlib.h>

#include "dvm_tag.h"
#include "fdvm.h"
#include "macro.h"
#include "ext_lib.h"
#include "ext_low.h"
static int TabNumber =0;
static int Number_Of_Flag = 0;
#define TASK_PROC_GENERATE 0
#define MAXFLAG 64
#define  MAXLFLAG 256
#define MAXLEVEL 256
#define IS_DISTRIBUTE_ARRAY(A) ((SYMB_ATTR((A)) & DISTRIBUTE_BIT) || (SYMB_ATTR((A)) & ALIGN_BIT) || (SYMB_ATTR((A)) & INHERIT_BIT))
char *copys(char *);

PTR_SYMB SymbolID[MAXFLAG];
int Number_Of_Symbol = 0;
int On_count = 0;
int TaskRegionUnparse = 0;
int HPF_VERSION=0;
int errnumber=0;
int NumberOfIndependent=0;
static char TabOfFlag[MAXFLAG][MAXLFLAG];
static int  FlagLenght[MAXFLAG];
static int  FlagLevel[MAXFLAG];
static int  FlagOn[MAXLEVEL][MAXFLAG];
#define MAXLENGHTBUF 750000
static int Buf_pointer = 0;
static char UnpBuf[MAXLENGHTBUF];
static char *Buf_address;
int CommentOut = 0;
int Pointer = 0;
char *hpfname;
#define C_Initialized 1
#define Fortran_Initialized 2
static int Parser_Initiated = 0;
PTR_FILE current_file=NULL;
PTR_LLND On_Clause=NULL;
PTR_LLND ReductionList=NULL;
PTR_LLND NewSpecList=NULL;
extern void Message();
/* FORWARD DECLARATIONS */
int BufPutString();
PTR_LLND FindMapDir();
PTR_LLND ChangeRedistributeOntoTask();
PTR_LLND FindRealignDir();
PTR_LLND FindRedistributeDir();
PTR_LLND FindDynamicDir();
int UnparseEndofCircle();
PTR_BFND FindDistrAlignCombinedDir();
PTR_LLND FindPointerDescriptor();
void gen_hpf_name ();
void PointerDeclaration();
int ArrayOfPointerDeclaration();
void GenerateType();
void Init_HPFUnparser();
void ResetSymbolId();
int NumberOfForNode();
int FindPointerDir();
int FindPointerDeclaration();
int FindCommonHeapDeclaration();
int Puttab();
int Find_SaveSymbol();
int CheckNullDistribution();
char *Tool_Unparse_Bif();
char *Tool_Unparse2_LLnode (); 
char * Tool_Unparse_Type();
char * Tool_Unparse_Symbol();
PTR_BFND FindBeginingOfBlock();
PTR_BFND FindEndOfBlock();
int CheckAcross();
int CheckReduction();
int IfReduction();
int FindRedInExpr();
PTR_LLND AddToReductionList();
int FindInNewList();
int isForNodeEndStmt();
int ForNodeStmt();
PTR_LLND FreeReductionList();

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
/*****************************************/

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
			       "double complex"
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
			       "error23"
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
        "atomic",                /* CC++ atomic */
        "__private",             /* for KSR */
        "restrict"
};

/*********************************************************/

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
                            1,     /* Minus_op*/
                            1      /* not op */
                            };

#define type_index(X)   (X-T_INT)                 /* gives the index of a type to access the Table "ftype_name" from a type code */
#define binop(n)    (n >= EQ_OP && n <= NEQV_OP)  /* gives the boolean value of the operation "n" being binary (not unary) */
/* In order to change ON-block procedure call */
typedef struct func_call *PTR_FCALL;

struct func_call 
    {
     PTR_LLND   func_ref;
     PTR_BFND   first;
     PTR_BFND	last;
     PTR_FCALL  next;
    };
    
PTR_LLND parameter_list=NULL;
PTR_SYMB function_name=NULL;
int ON_BLOCK=0;
int ON_BEGIN=0;
PTR_FCALL TaskRegion=NULL;
extern char *chkalloc();
#define ALLOC(x)  (struct x *) chkalloc(sizeof(struct x))
#define FUNC_REF(NODE) ((NODE)->func_ref)
#define FUNC_FIRST(NODE) ((NODE)->first)
#define FUNC_LAST(NODE) ((NODE)->last)
#define FUNC_NEXT(NODE) ((NODE)->next)

void UnparseTaskRegion(PTR_FCALL TaskRegion)
{
PTR_BFND bif;
PTR_FCALL temp=NULL;
TaskRegionUnparse=1;
for(;TaskRegion;temp=TaskRegion,TaskRegion=FUNC_NEXT(TaskRegion))
    {
    PTR_LLND llnd;
    if (temp)
	{
	FUNC_NEXT(temp)=NULL;
	FUNC_FIRST(temp)=NULL;
	FUNC_LAST(temp)=NULL;
	free(FUNC_REF(temp));
	free(temp);
	}
    BufPutString("\n",0);
    Puttab();
    BufPutString("subroutine ",0);
    Tool_Unparse2_LLnode(FUNC_REF(TaskRegion));
    BufPutString("\n",0);
    llnd=NODE_OPERAND0(FUNC_REF(TaskRegion));
    for(;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
	{
	if (NODE_CODE(NODE_OPERAND0(llnd))==ARRAY_REF)
	    {
	    if (NODE_SYMB(NODE_OPERAND0(llnd)))
		{
		PTR_SYMB sym=NODE_SYMB(NODE_OPERAND0(llnd));
		if ((SYMB_ATTR(sym)&DISTRIBUTE_BIT)||
		   (SYMB_ATTR(sym)&ALIGN_BIT))
		    {
		    Puttab();
		    BufPutString("DIMENSION ",0);
		    Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
		    BufPutString("(",0);
		    Tool_Unparse2_LLnode(TYPE_DECL_RANGES(SYMB_TYPE(sym)));
		    BufPutString(")",0);
		    BufPutString("\n",0);
		    BufPutString("!HPF$",0);
		    Puttab();
		    Buf_pointer-=5;
		    BufPutString("INHERIT ",0);
		    Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
		    BufPutString("\n",0);
		    }
		else
		    {
		    Puttab();
		    BufPutString("DIMENSION ",0);
		    Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
		    BufPutString("(",0);
		    Tool_Unparse2_LLnode(TYPE_DECL_RANGES(SYMB_TYPE(sym)));
		    BufPutString(")",0);
		    BufPutString("\n",0);
		    }     
		}
	    }
	else 
	    {		    
	    PTR_SYMB sym=NODE_SYMB(NODE_OPERAND0(llnd));
	    if ((NODE_CODE(NODE_OPERAND0(llnd))==CONST_REF)||
		(NODE_CODE(NODE_OPERAND0(llnd))==VAR_REF))
		{
		Puttab();
		Tool_Unparse_Type(SYMB_TYPE(sym));
		BufPutString(" ",0);
		Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
		BufPutString("\n",0);
		}
	    }
	}
    for(bif=BIF_NEXT(FUNC_FIRST(TaskRegion));bif&&(bif!=FUNC_LAST(TaskRegion));bif=BIF_NEXT(bif))
	Tool_Unparse_Bif(bif);
    Puttab();
    BufPutString("end \n\n",0);
    }
if (temp)
    {
    FUNC_NEXT(temp)=NULL;
    FUNC_FIRST(temp)=NULL;
    FUNC_LAST(temp)=NULL;
    free(FUNC_REF(temp));
    free(temp);
    }
    
TaskRegionUnparse=0;
}    
    

PTR_FCALL 
FindLast ( ptr )
PTR_FCALL ptr;
{
PTR_FCALL ptr_func=ptr;
if (!ptr) return NULL;
while(ptr_func)
	{
	  if (FUNC_NEXT(ptr_func) == NULL)
	    return ptr_func;
	  ptr_func = FUNC_NEXT(ptr_func);	  
	}
return NULL;
}

PTR_LLND
make_llnode (node_type, ll1, ll2, symb_ptr)
	int	 node_type;
	PTR_LLND ll1, ll2;
	PTR_SYMB symb_ptr;
{
	PTR_LLND new_llnd;

	new_llnd = ALLOC (llnd);
	new_llnd->variant = node_type;
	new_llnd->type = TYNULL;
	new_llnd->entry.Template.ll_ptr1 = ll1;
	new_llnd->entry.Template.ll_ptr2 = ll2;
	switch (node_type) {
	  case INT_VAL:
            /*new_llnd->entry.ival = (int) symb_ptr;*/
            break;
	  case BOOL_VAL:
            /*new_llnd->entry.bval = (int) symb_ptr;*/
            break;
	  default:
            new_llnd->entry.Template.symbol = symb_ptr;
            break;
	  }
	return (new_llnd);
}

PTR_SYMB
make_funcsymb (string)
	char	*string;
{
	PTR_SYMB new_symb;
	new_symb = ALLOC (symb);
	new_symb->variant = ARRAY_REF;
	new_symb->ident = copys (string);
	return (new_symb);
}

void ResetSymbolDovar()
{
PTR_SYMB symb;
for (symb = current_file->head_symb; symb ; symb = SYMB_NEXT (symb))
    if (SYMB_DOVAR (symb)) SYMB_DOVAR (symb) &=~2 ;
}



/* In order to change ON-block procedure call end*/

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
	    { if(!NODE_SYMB(tp)){
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
      for (tp = NODE_OPERAND0(tp); tp && (protect == 0); ) /*(protect == NULL)*/
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
    
  pt = (char *) xmalloc((int)(strlen(str)+1));
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

int Reset_Unparser()
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
  return 0;
}



/* function to manage the unparse buffer */

int BufPutChar(c)
     char c;
{
  if (Buf_pointer >= MAXLENGHTBUF)
    {
      Message("Unparse Buffer Full",0);
      return 0;
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
      Message("Null String in BufPutString",0);
      return 0;
    }
  length = len;
  if (length <= 0)
    length  = strlen(s);
  
  if (Buf_pointer + length>= MAXLENGHTBUF)
    {
      Message("Unparse Buffer Full",0);
      return 0;
    }
  strncpy(&(Buf_address[Buf_pointer]),s,length);
  Buf_pointer += length;
  return 1;
}


int BufPutInt(i)
     int i;
{
  int length;
  char s[MAXLFLAG];
  
  sprintf(s,"%d",i);
  length = strlen(s);
  
  if (Buf_pointer + length>= MAXLENGHTBUF)
    {
      Message("Unparse Buffer Full",0);
      return 0;
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
              
      if ((SYMB_ATTR(symb) & OVOPERATOR) ||
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
          (strcmp(SYMB_IDENT(symb),"new")==0) ||
          (strcmp(SYMB_IDENT(symb),"delete")==0) ||
          (strcmp(SYMB_IDENT(symb),"[]")==0) )
        BufPutString ("operator ",0);
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
        Op->val.I = (long) NULL;
      *iptr += strlen("%L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L2CODE", strlen("%L2CODE"))== 0)               /* %L2CODE : Code (variant) of Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (NODE_TEMPLATE_LL2 (ll))
        Op->val.I = NODE_CODE (NODE_TEMPLATE_LL2 (ll));
      else
        Op->val.I = (long) NULL;
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
            Op->val.I = (long) NULL;
        }
      else
        Op->val.I = (long) NULL;
      *iptr += strlen("%L1L2*L1CODE");
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
        Op->val.I = (long) NULL;
      *iptr += strlen("%L1CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L2CODE", strlen("%L2CODE"))== 0)               /* %L2CODE : Code (variant) of Low Level Node 2 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (BIF_LL2 (bif))
        Op->val.I = NODE_CODE (BIF_LL2 (bif));
      else
        Op->val.I = (long) NULL;
      *iptr += strlen("%L2CODE");
    } else
  if (strncmp(&(str[*iptr]),"%L1L2L1CODE", strlen("%L1L2L1CODE"))== 0)       /* %L1L2L1CODE : Code (variant) of Low Level Node 1 of Low Level Node 2 of Low Level Node 1 (integer) */
    {
      Op->typ = INTEGER_TYP;
      if (BIF_LL1 (bif) && NODE_TEMPLATE_LL2 (BIF_LL1 (bif)) && NODE_TEMPLATE_LL1 (NODE_TEMPLATE_LL2 (BIF_LL1 (bif))))
        Op->val.I = NODE_CODE (NODE_TEMPLATE_LL1 (NODE_TEMPLATE_LL2 (BIF_LL1 (bif))));
      else
        Op->val.I = (long) NULL;
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
            Op->val.I = (long) NULL;
        }
      else
        Op->val.I = (long) NULL;
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
     /*int def;*/        /* def = 1 : defined type */
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
                /*int j;*/
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
                { 
		  DealWith_Rid(ptype,In_Class_Flag);
                  i += strlen("RIDPT");
                } else
           if (strncmp(&(str[i]),"TABNAME", strlen("TABNAME"))== 0)       /* %TABNAME : Self Name from Table */
                {
		  if (Check_Lang_Fortran(cur_proj))
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
                  int j;
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
	      if (Check_Lang_Fortran(cur_proj))
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
                     if(TYPE_SCOPE_SYMB_DERIVE(ptype) && TYPE_CODE(ptype) != T_DERIVED_TEMPLATE) {
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
  /*BufPutInt(variant);
  printf("LLNODE : %i\n%s",variant,Buf_address);*/
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&
      ((variant==ARRAY_REF)||
       (variant==VAR_REF)||
       (variant==CONST_REF))&&
       ON_BLOCK&&NODE_SYMB(ll)) 
      {
      PTR_LLND ptr,new_node,new_symb;
      if (!SYMB_DOVAR(NODE_SYMB(ll))&&
          (!(SYMB_ATTR(NODE_SYMB(ll))&TASK_BIT))&&
	  (!(SYMB_ATTR(NODE_SYMB(ll))&PROCESSORS_BIT)))
          {
          new_symb=make_llnode(variant,LLNULL,LLNULL,NODE_SYMB(ll));
	  new_node=make_llnode(EXPR_LIST,new_symb,LLNULL,SMNULL);
          if (parameter_list)
              { 
              ptr=Follow_Llnd(parameter_list,2);
	      NODE_OPERAND1(ptr)=new_node;
	      }
          if (!parameter_list) parameter_list=new_node;
	  SYMB_DOVAR(NODE_SYMB(ll))=2;
	  }
      }
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
             /* int j;*/
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
	  if (strncmp(&(str[i]),"BACK", strlen("BACK"))== 0)
            {
              Buf_pointer--;
              i += strlen("BACK");
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
          if (strncmp(&(str[i]),"NEWSPEC", strlen("NEWSPEC"))== 0)
            {
	      if (NewSpecList) free(NewSpecList);
              NewSpecList=NODE_OPERAND0(ll);
              i += strlen("NEWSPEC");
            } else
	      if (strncmp(&(str[i]),"COMMONDECL", strlen("COMMONDECL"))== 0)               /* %DECLARATION : We need to change declaration of Pointer descriptor */
            {
              int count=0;
	      if ((NODE_CODE(ll)==COMM_LIST)&&NODE_OPERAND0(ll)) count=FindCommonHeapDeclaration(NODE_OPERAND0(ll));
	      if (!count) 
		{
		  count=0;
		  Treat_Flag("(POINTER)",&count, 1);
		}
              i += strlen("COMMONDECL");
            } else
	      if (strncmp(&(str[i]),"CHECK_HEAP", strlen("CHECK_HEAP"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
		{
		 /* char *str;
		  if (ll) str=SYMB_IDENT(NODE_SYMB (ll));
		  if (!strcmp(str,"heap")||!strcmp(str,"HEAP")) */                  
		if(SYMB_ATTR(NODE_SYMB (ll))&HEAP_BIT)
		    {
		      /*BufPutString("<HEAP_BIT",0);
		      BufPutString(SYMB_IDENT(NODE_SYMB (ll)),0);
		      BufPutString("/HEAP_BIT>",0);*/
		      Tool_Unparse2_LLnode (NODE_OPERAND0(ll));
		      return Buf_address;
		    }
		  i += strlen("CHECK_HEAP");
		} else
		if (strncmp(&(str[i]),"CHECK_PTR", strlen("CHECK_PTR"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
		{
		  if (ll) 
		      if(SYMB_ATTR(NODE_SYMB (ll))&DVM_POINTER_ARRAY_BIT)
		          BufPutString("%PTR",0);
		  i += strlen("CHECK_PTR");
		} else
		if (strncmp(&(str[i]),"CHECK_PROC_REF", strlen("CHECK_PROC_REF"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
		{
		    int k=0;
		    if (ll) 
		      if(SYMB_ATTR(NODE_SYMB (ll))&PROCESSORS_BIT)
		          if (!Get_Flag_val("(PROC_REF)",&k)) 
			      return Buf_address;
		  i += strlen("CHECK_PROC_REF");
		} else
		if (strncmp(&(str[i]),"DELETE_COMMA", strlen("DELETE_COMMA"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
		{
		  if (Buf_address[Buf_pointer-1]==',') 
		      Buf_pointer--;
		  i += strlen("DELETE_COMMA");
		} else
		if (strncmp(&(str[i]),"CHECK_DYNAMIC", strlen("CHECK_DYNAMIC"))== 0)
		{
		  if (NODE_OPERAND0(ll)||NODE_OPERAND1(ll)) 
		      i += strlen("CHECK_DYNAMIC");
		  else {
		         int k=0;
			 if (Get_Flag_val("(DYNAMIC)",&k)) 
			     {
			     Buf_pointer--;
			     return Buf_address;
			     }
			 k=0;
			 Treat_Flag("(DYNAMIC)",&k,1);
		         BufPutString("DYNAMIC",0);
			 i += strlen("CHECK_DYNAMIC");		      
		         return Buf_address;
			}
		} else
	
		  if (strncmp(&(str[i]),"SAVE_SYMBOL", strlen("SAVE_SYMBOL"))== 0)         /* %SAVE_SYMBOL : Reduction variables list */
		{
		  if (Find_SaveSymbol (ll)) 
		    {
		      if (Buf_address[Buf_pointer-1]==',') Buf_pointer--;
		      return Buf_address;
		    }
		  i += strlen("SAVE_SYMBOL");
		} else
		    if (strncmp(&(str[i]),"DELETE_SYMBOL", strlen("DELETE_SYMBOL"))== 0)         /* %DELETE_SYMBOL : Clear reduction variables list */
		{
		  Number_Of_Symbol=0;
		  i += strlen("DELETE_SYMBOL");
		} else
	      if (strncmp(&(str[i]),"POINTER_NAME", strlen("POINTER_NAME"))== 0)         /* %POINTER_NAME : We should change the definition of Pointer descriptor  */
            {
	      if ((NODE_CODE(ll)==VAR_REF)||(NODE_CODE(ll)==ARRAY_REF))
		if(SYMB_ATTR(NODE_SYMB (ll))&DVM_POINTER_BIT) 
		  {
		    Tool_Unparse_Symbol (NODE_SYMB (ll));
		    if (NODE_CODE(ll)==ARRAY_REF)
		      {
			if(NODE_OPERAND0(ll))
			  {
			    BufPutChar('(');
			    Tool_Unparse2_LLnode (NODE_OPERAND0(ll));
			    BufPutChar(')');
			  }
		      }
		     return Buf_address;
		  }
              i += strlen("POINTER_NAME");
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
          if (strncmp(&(str[i]),"INTVAL", strlen("INTVAL"))== 0)         /* %INTVAL : Integer Value */
            {
              if (NODE_INT_CST_LOW (ll) >= 0)
                {
                  BufPutInt (NODE_INT_CST_LOW (ll));
                } else
                  {
                    BufPutString ("(",0);
                    BufPutInt (NODE_INT_CST_LOW (ll));
                    BufPutString (")",0);
                  }
              i += strlen("INTVAL");
            } else
          if (strncmp(&(str[i]),"STATENO", strlen("STATENO"))== 0)       /* %STATENO : Statement number */
            {
              if (NODE_LABEL (ll))
                {
                  BufPutInt ((int)( LABEL_STMTNO (NODE_LABEL (ll))));
                }
              i += strlen("STATENO");
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
		if (strncmp(&(str[i]),"CHPFCONTTAB", strlen("CHPFCONTTAB"))== 0)               /* %TAB : Tab */
                {
                  int j;
		  BufPutString ("\n!HPF$*",0);
                  for (j = 0; j < TabNumber; j++)
                    if (j>0)
                      BufPutString ("   ",0);
                    else
                      BufPutString ("      ",0);
		      Buf_pointer-=7;
                  i += strlen("CHPFCONTTAB");
                } else
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

char *

Tool_Unparse_Bif(bif)
     PTR_BFND bif;
{
  int variant;
  int kind;
  char *str;
  char c;
  int i;
  int AfterDOLabel=0;

  if (!bif)
    return NULL;

  variant = BIF_CODE(bif);
  /*BufPutInt(variant);
  printf("BIFNODE : %i\n%s",variant,Buf_address);*/

  /*BufPutInt(isForNodeEndStmt(bif));
  BufPutInt(variant);*/
  kind = (int) node_code_kind[(int) variant];
  if (kind  != (int)BIFNODE)
    Message("Error in Unparse, not a bif node",0);
  str = Unparse_Def[variant].str;
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&TaskRegionUnparse) 
  {   if (!BIF_ID(bif)) return Buf_address;
      else BIF_ID(bif)=0;
  }      
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&(variant==CONTROL_END)) 
      if (NODE_CODE(BIF_CP(bif))==PROG_HEDR)
          if (TaskRegion) 
	      {
	      if (TabNumber>1) TabNumber--;
	      Puttab();
	      BufPutString("end \n\n",0);
	      UnparseTaskRegion(TaskRegion);
	      return Buf_address;
	      }
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&(variant==DVM_ON_DIR)) 
      {
      char *name;
      int i,temp=Buf_pointer;
      PTR_FCALL fcall,new_node;
      Tool_Unparse2_LLnode(BIF_LL1(bif));
      name=xmalloc(Buf_pointer-temp);
      for(i=0;i+temp<Buf_pointer;i++)
          if(Buf_address[temp+i]!='(')
              name[i]=Buf_address[temp+i];
	  else name[i]='_';
      name[i-1]='\0';
      Buf_pointer=temp;
      function_name=make_funcsymb(name);
      free(name);
      new_node=ALLOC(func_call);
      FUNC_FIRST(new_node)=bif;
      if (TaskRegion)
              { 
              fcall=FindLast(TaskRegion);
	      FUNC_NEXT(fcall)=new_node;
	      }
      else TaskRegion=new_node;
      }
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&(variant==DVM_END_ON_DIR)) 
      {
      PTR_FCALL fcall;
      ON_BLOCK=0;
      Buf_pointer=ON_BEGIN;
      if (TaskRegion)
              { 
              fcall=FindLast(TaskRegion);
	      FUNC_LAST(fcall)=bif;
	      FUNC_REF(fcall)=make_llnode(FUNC_CALL,parameter_list,LLNULL,function_name);
	      }      
      TabNumber++;
      Puttab();
      TabNumber--;
      BufPutString("call ",0);
      Tool_Unparse2_LLnode(make_llnode(FUNC_CALL,parameter_list,LLNULL,function_name));
      BufPutString("\n",0);
      parameter_list=NULL;
      ResetSymbolDovar();
      }
  /* printf("variant = %d, str = %s\n", variant, str); */
  /* now we have to interpret the code to unparse it */
  
  if (str == NULL)
    return NULL;
  if (strcmp( str, "n") == 0)
    if (strcmp ( str, "n") == 0)
    {
      Message("Node not define for unparse",BIF_LINE(bif));
      return NULL;
    }

  
  i = 0 ;
  if (Get_Flag_val("(COMMENT)",&i)) BufPutChar('C');
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
		      if (CMNT_STRING(BIF_CMNT(bif)))
			{
			  BufPutChar('\n');
			  BufPutString(CMNT_STRING(BIF_CMNT(bif)),0);
			  if (!Check_Lang_Fortran(cur_proj))
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
            } else
              if (strncmp(&(str[i]),"SETFLAG", strlen("SETFLAG"))== 0)
            {               
              i = i + strlen("SETFLAG");
              Treat_Flag(str, &i,1);
            } else
	      if (strncmp(&(str[i]),"INDEPENDENT_DO", strlen("INDEPENDENT_DO"))== 0)
            {               
              int count=0;
		if (BIF_CODE(bif)==DVM_PARALLEL_ON_DIR)
	          {
		  if (BIF_LL3(bif))
		      {
		      if (NODE_CODE(BIF_LL3(bif))==EXPR_LIST)
		          {
			  PTR_LLND expr_list;
			  for(expr_list=BIF_LL3(bif);expr_list;expr_list=NODE_OPERAND1(expr_list))
			      count++;
			  }
		      }
		  }
	      NumberOfIndependent=count-1;
	      i = i + strlen("INDEPENDENT_DO");
            } else
	      if (strncmp(&(str[i]),"CHECK_INDEPENDENT_DO", strlen("CHECK_INDEPENDENT_DO"))== 0)
            {               
		if (NumberOfIndependent>0)
		    {
		    BufPutString("!HPF$",0);
	            Puttab();
	            Buf_pointer-=5;
	            BufPutString("INDEPENDENT\n",0);	      
		    }
		NumberOfIndependent--;
	      i = i + strlen("CHECK_INDEPENDENT_DO");
            } else
	      if (strncmp(&(str[i]),"RESET_INDEPENDENT_DO", strlen("RESET_INDEPENDENT_DO"))== 0)
            {               
		NumberOfIndependent=0;
	      i = i + strlen("RESET_INDEPENDENT_DO");
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
            { 
		int j=0;     
                  if (!Get_Flag_val("(NO_NL)",&j)) 
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
              if (strncmp(&(str[i]),"TAB", strlen("TAB"))== 0)               /* %TAB : Tab */
                {
              BufPutString ("      ",0); /* cychen */
              i += strlen("TAB");
            } else
              if (strncmp(&(str[i]),"PUTTAB", strlen("PUTTAB"))== 0)               /* %TAB : Tab */
                {
                  int j;
		  int k=0;     
                  if (!Get_Flag_val("(NO_NL)",&k)) 
		      for (j = 0; j < TabNumber; j++)
                	if (j>0)
                          BufPutString ("   ",0);
                	else
                          BufPutString ("      ",0); /* cychen */
                  i += strlen("PUTTAB");
                } else
		if (strncmp(&(str[i]),"CHPFTAB", strlen("CHPFTAB"))== 0)               /* %TAB : Tab */
                {
                  int j;
		  BufPutString ("!HPF$",0);
                  for (j = 0; j < TabNumber; j++)
                    if (j>0)
                      BufPutString ("   ",0);
                    else
                      BufPutString ("      ",0);
		      Buf_pointer-=6;
                  i += strlen("CHPFTAB");
                } else
		if (strncmp(&(str[i]),"GETRED", strlen("GETRED"))== 0)               /* %TAB : Tab */
                {
		  if (FindRedInExpr(BIF_LL1(bif),BIF_LL2 (bif)))
			{
			if (BIF_LL1(bif)&&NODE_SYMB(BIF_LL1(bif))&&(!IS_DISTRIBUTE_ARRAY(NODE_SYMB(BIF_LL1(bif))))&&(!SYMB_DOVAR(NODE_SYMB(BIF_LL1(bif)))))
				{
				BufPutString("Reduction var :",0);
				Tool_Unparse2_LLnode(BIF_LL1(bif));
				BufPutString("\n",0);
				if (!FindInNewList(NewSpecList,BIF_LL1(bif)))
				  if(!FindInNewList(ReductionList,BIF_LL1(bif)))
				   {		
				   ReductionList=AddToReductionList(ReductionList,BIF_LL1(bif));
				   BufPutString("Reduction LIST :",0);
				   Tool_Unparse2_LLnode(ReductionList);
				   BufPutString("\n",0);
                                   }
				}
			}
                  i += strlen("GETRED");
                } else
		if (strncmp(&(str[i]),"INSERT_REDUCTION", strlen("INSERT_REDUCTION"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
		{
		  if (ReductionList)
		     {
                      BufPutString("MY_REDUCTION (",0);
 		      Tool_Unparse2_LLnode(ReductionList);
                      BufPutString(")",0);
		      ReductionList=FreeReductionList(ReductionList);
		      /*if (NewSpecList)  free(NewSpecList);*/
		     }			
		  i += strlen("INSERT_REDUCTION");
		} else
		if (strncmp(&(str[i]),"CHPFCONTTAB", strlen("CHPFCONTTAB"))== 0)               /* %TAB : Tab */
                {
                  int j;
		  BufPutString ("\n!HPF$*",0);
                  for (j = 0; j < TabNumber; j++)
                    if (j>0)
                      BufPutString ("   ",0);
                    else
                      BufPutString ("      ",0);
		      Buf_pointer-=7;
                  i += strlen("CHPFCONTTAB");
                } else
              if (strncmp(&(str[i]),"INCTAB", strlen("INCTAB"))== 0)               /* increment tab */
            {
              TabNumber++;              
              i += strlen("INCTAB");
            }  else
              if (strncmp(&(str[i]),"DECTAB", strlen("DECTAB"))== 0)               /*deccrement tab */
            {
	      if (Check_Lang_Fortran(cur_proj))
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
		{
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
		    BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL_USE (bif))));
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
                      BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
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
		/*if (BIF_CP(bif)&&(BIF_CODE(BIF_CP(bif))==FOR_NODE)&&
		    (HPF_VERSION==2))
		        {
			AfterDOLabel=1;
			if (BIF_LABEL_USE(BIF_CP(bif))&&
			   (LABEL_STMTNO (BIF_LABEL(bif))!=LABEL_STMTNO (BIF_LABEL_USE(BIF_CP(bif)))))
				BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
			else if (!BIF_LABEL_USE(BIF_CP(bif)))
				BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
			}
		else21052001*/
                /*BufPutInt (LABEL_STMTNO (BIF_LABEL(bif)));*/
		if (BIF_CP(bif)&&(BIF_CODE(BIF_CP(bif))==FOR_NODE)&&
		    (HPF_VERSION==2))
		        {
			AfterDOLabel=1;
			if (BIF_LABEL_USE(BIF_CP(bif))&&
			   (LABEL_STMTNO (BIF_LABEL(bif))!=LABEL_STMTNO (BIF_LABEL_USE(BIF_CP(bif)))))
				BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
			else if (!BIF_LABEL_USE(BIF_CP(bif)))
				BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
			}
		else
                BufPutInt ((int)(LABEL_STMTNO (BIF_LABEL(bif))));
                }
              i += strlen("LABEL");
            } else
          if (strncmp(&(str[i]),"SYMBTYPE", strlen("SYMBTYPE"))== 0)     /* SYMBTYPE : Type of Symbol */
            {
              if (BIF_SYMB (bif) && SYMB_TYPE (BIF_SYMB (bif)))
		{
		  if (Check_Lang_Fortran(cur_proj))
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
	  if (strncmp(&(str[i]),"DELETE_COMMA", strlen("DELETE_COMMA"))== 0)         /* %CHECK_HEAP : Pointer HEAP(PA) -> PA */
	    {
	    if (Buf_address[Buf_pointer-1]==',') 
	      Buf_pointer--;
	    i += strlen("DELETE_COMMA");
	     } else
	    if (strncmp(&(str[i]),"TASKERROR0", strlen("TASKERROR0"))== 0)
            {
	    /*Message("Error in block-task-region\n end-task-region-directive: CDVM$ END TASK_REGION is missing before TASK_REGION construct.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 210 on line %d of %s : End-task-region-directive: CDVM$ END TASK_REGION is missing before TASK_REGION construct\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("TASKERROR0");
            } else
	    if (strncmp(&(str[i]),"TASKERROR1", strlen("TASKERROR1"))== 0)
            {
	    /*Message("Error in block-task-region\n task-region-directive: CDVM$ TASK_REGION is missing before on-block.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 211 on line %d of %s : CDVM$ TASK_REGION is missing before on-block \n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("TASKERROR1");
            } else
	    if (strncmp(&(str[i]),"TASKERROR2", strlen("TASKERROR2"))== 0)
            {
	    /*Message("Error in block-task-region\n task-region-directive: CDVM$ TASK_REGION is missing before end-on-directive.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 212 on line %d of %s : CDVM$ TASK_REGION is missing before end-on-directive \n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("TASKERROR2");
            } else
	    if (strncmp(&(str[i]),"TASKERROR3", strlen("TASKERROR3"))== 0)
            {
	    /*Message("Error in block-task-region\n end-task-region-directive: CDVM$ END TASK_REGION is missing before end of program.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 213 on line %d of %s : End-task-region-directive: CDVM$ END TASK_REGION is missing before end of program\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("TASKERROR3");
            } else
	    if (strncmp(&(str[i]),"TASKERROR4", strlen("TASKERROR4"))== 0)
            {
	    /*Message("Error in block-task-region\n task-region-directive: CDVM$ TASK_REGION is missing before end-task-region-directive.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 214 on line %d of %s : End-task-region-directive: CDVM$ TASK_REGION is missing before end-task-region-directive\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("TASKERROR4");
            } else    
	    if (strncmp(&(str[i]),"ONERROR", strlen("ONERROR"))== 0)
            {
            if (On_count)
		{
		fprintf(stderr,"Error 215 on line %d of %s : Error in TASK_REGION construct\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		fprintf(stderr,"Warning 216 on line %d of %s : Incorrect number of ON and END_ON directives\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		errnumber++;
		/*Message("Error in TASK_REGION construct\nThis may be due to incorrect number of ON and END_ON directives.",BIF_LINE(bif));*/
		On_count=0;
		}    
	    i += strlen("ONERROR");
            } else
	    if (strncmp(&(str[i]),"HPF1_POINTER", strlen("HPF1_POINTER"))== 0)
            {
	    /*Message("Error in DVM_POINTER_DIR\n Can`t work this pointers in HPF1.",BIF_LINE(bif));*/
	    fprintf(stderr,"Error 197 on line %d of %s : Can`t work this pointers in HPF1\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
	    errnumber++;
	    i += strlen("HPF1_POINTER");
            } else    	    
	    if (strncmp(&(str[i]),"ONPLUS", strlen("ONPLUS"))== 0)
        	{
        	int k=0;
		On_count++;
		Treat_Flag("(ON_REGION)",&k,1);
		i += strlen("ONPLUS");
        	} 
	    else
	    if (strncmp(&(str[i]),"ONMINUS", strlen("ONMINUS"))== 0)
        	{
        	int k=0;
		On_count--;
		Treat_Flag("(ON_REGION)",&k,-1);
	        i += strlen("ONMINUS");
        	} 
	    else
	    if (strncmp(&(str[i]),"ONINIT", strlen("ONINIT"))== 0)
        	{
        	On_count=0;
		i += strlen("ONINIT");
        	} 
	    else
	    if (strncmp(&(str[i]),"RESETID", strlen("RESETID"))== 0)
        	{
        	ResetSymbolId();
		i += strlen("RESETID");
        	} 
	    else
	      if (strncmp(&(str[i]),"CHANGE_TABNUMBER", strlen("CHANGE_TABNUMBER"))== 0)
		{
		  int count=0,label;
		  count=NumberOfForNode(bif,&label);
		  if(count)
		      TabNumber-=count;
		  if (TabNumber<1) TabNumber=1;
		  i += strlen("CHANGE_TABNUMBER");
		}
	    else
	      if (strncmp(&(str[i]),"FIND_DO", strlen("FIND_DO"))== 0)       /* %FIND_DO : We need to find the corresponding FOR_NODE for CONT_STAT*/
		{
		  i += strlen("FIND_DO");
		  /*21052001if (UnparseEndofCircle(bif))
		  	return Buf_address;*/
		if (UnparseEndofCircle(bif))
		  	return Buf_address;
		} else
	      if (strncmp(&(str[i]),"SAVE", strlen("SAVE"))== 0)
		{
		  Pointer=Buf_pointer;
		  i += strlen("SAVE");
		} else
	      if (strncmp(&(str[i]),"LOAD", strlen("LOAD"))== 0)
		{
		  Buf_pointer=Pointer;
		  i += strlen("LOAD");
		} else
          if (strncmp(&(str[i]),"FIND_MAP", strlen("FIND_MAP"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
              PTR_LLND llnd;
	      if (BIF_LL1(bif))
                {
                  llnd=FindMapDir(BIF_LL1(bif),bif);
                  if (llnd)
                    {
                      Tool_Unparse2_LLnode(llnd);
                    }
		  else  
		    {
		      /*Message ("Error in Unparse, can`t find a corresponding MAP_DIR", BIF_LINE(bif));*/
		      fprintf(stderr,"Error 195 on line %d of %s : Can`t find a corresponding MAP_DIR\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		      errnumber++;
		      Tool_Unparse2_LLnode(BIF_LL1(bif));
		    }
		}
              i += strlen("FIND_MAP");
            } else
	    if (strncmp(&(str[i]),"CHECK_REDISTRIBUTE_DIR", strlen("CHECK_REDISTRIBUTE_DIR"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
              PTR_LLND llnd;
	      if (BIF_LL1(bif))
                {
                  for (llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
                      if (FindRedistributeDir(NODE_OPERAND0(llnd),bif))
                	 {
			    if (!FindDynamicDir(NODE_OPERAND0(llnd),bif))
			     {
			     BufPutString("!HPF$",0);
			     Puttab();
			     Buf_pointer-=5;
			     BufPutString("DYNAMIC ",0);
			     Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
			     BufPutString("\n",0);
			     }
			 }
                 }
              i += strlen("CHECK_REDISTRIBUTE_DIR");
            } else
	    if (strncmp(&(str[i]),"CHECK_REALIGN_DIR", strlen("CHECK_REALIGN_DIR"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
              PTR_LLND llnd;
	      if (BIF_LL1(bif))
                {
                  for (llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
                      if (FindRealignDir(NODE_OPERAND0(llnd),bif))
                	 {
			    if (!FindDynamicDir(NODE_OPERAND0(llnd),bif))
			     {
			     BufPutString("!HPF$",0);
			     Puttab();
			     Buf_pointer-=5;
			     BufPutString("DYNAMIC ",0);
			     Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
			     BufPutString("\n",0);
			     }
			 }
                 }
              i += strlen("CHECK_REALIGN_DIR");
            } else
	    if (strncmp(&(str[i]),"CHECK_FORMAT_NULL", strlen("CHECK_FORMAT_NULL"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
	      if (BIF_LL1(bif))
                {
		if (!BIF_LL2(bif)&&!BIF_LL3(bif))	     
		    {
		    BufPutString("!HPF$",0);
		    Puttab();
		    Buf_pointer-=5;
    		    BufPutString("DYNAMIC ",0);
		    Tool_Unparse2_LLnode(BIF_LL1(bif));
            	    BufPutString("\n",0);
		    return Buf_address;
		    }
		}
              i += strlen("CHECK_FORMAT_NULL");
            } else
	    if (strncmp(&(str[i]),"CHECK_DVMBIT", strlen("CHECK_DVMBIT"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
	      if (BIF_LL1(bif))
                {
		PTR_LLND ptr;
		if (BIF_CODE(bif)==DVM_ALIGN_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_ALIGN_BIT)
			    {
			    fprintf(stderr,"Error 200 on line %d of %s:Object '%s' already has ALIGN_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_ALIGN_BIT;
			}
		    }
		if (BIF_CODE(bif)==HPF_PROCESSORS_STAT)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_PROCESSORS_BIT)
			    {
			    fprintf(stderr,"Error 201 on line %d of %s :Object '%s' already has PROCESSORS_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_PROCESSORS_BIT;
			}
		    }
		    
		if (BIF_CODE(bif)==DVM_DISTRIBUTE_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_DISTRIBUTE_BIT)
			    {
			    fprintf(stderr,"Error 202 on line %d of %s :Object '%s' already has DISTRIBUTE_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_DISTRIBUTE_BIT;
			}
		    }
		if (BIF_CODE(bif)==HPF_TEMPLATE_STAT)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_TEMPLATE_BIT)
			    {
			    fprintf(stderr,"Error 203 on line %d of %s :Object '%s' already has TEMPLATE_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_TEMPLATE_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_INHERIT_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_INHERIT_BIT)
			    {
			    fprintf(stderr,"Error 204 on line %d of %s :Object '%s' already has INHERIT_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_INHERIT_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_DYNAMIC_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_DYNAMIC_BIT)
			    {
			    fprintf(stderr,"Error 205 on line %d of %s :Object '%s' already has DYNAMIC_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_DYNAMIC_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_SHADOW_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_SHADOW_BIT)
			    {
			    fprintf(stderr,"Error 206 on line %d of %s :Object '%s' already has SHADOW_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_SHADOW_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_TASK_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_TASK_BIT)
			    {
			    fprintf(stderr,"Error 207 on line %d of %s :Object '%s' already has TASK_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_TASK_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_POINTER_DIR)
		    {
		    for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if (SYMB_ID(NODE_SYMB(llnd))==ALREADY_DVM_POINTER_BIT)
			    {
			    fprintf(stderr,"Error 208 on line %d of %s :Object '%s' already has DVM_POINTER_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else SYMB_ID(NODE_SYMB(llnd))=ALREADY_DVM_POINTER_BIT;
			}
		    }
		if (BIF_CODE(bif)==DVM_VAR_DECL)
		    {
			PTR_LLND ptr_llnd;
			int BIT=0;
			for(ptr_llnd=BIF_LL3(bif);ptr_llnd&&(NODE_CODE(ptr_llnd)==EXPR_LIST);ptr_llnd=NODE_OPERAND1(ptr_llnd))
			    {
			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==DISTRIBUTE_OP) 
				BIT|=ALREADY_DISTRIBUTE_BIT;
			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==PROCESSORS_OP) 
				BIT|=ALREADY_PROCESSORS_BIT;
    			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==ALIGN_OP) 
				{
				BIT|=ALREADY_ALIGN_BIT;
				}
			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==TEMPLATE_OP) 
				{
				BIT|=ALREADY_TEMPLATE_BIT;
				}
    			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==SHADOW_OP) 
				{
				BIT|=ALREADY_SHADOW_BIT;
				}
    			    if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==DYNAMIC_OP) 
				{
				BIT|=ALREADY_DYNAMIC_BIT;
				}
			    }
		    for(ptr=BIF_LL1(bif);BIT&&ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
			{
			PTR_LLND llnd=NODE_OPERAND0(ptr);
			if (!llnd||(NODE_CODE(llnd)!=ARRAY_REF)) break;
			if ((BIT&ALREADY_DYNAMIC_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_DYNAMIC_BIT))
			    {
			    fprintf(stderr,"Error 205 on line %d of %s :Object '%s' already has DYNAMIC_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_DYNAMIC_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_DYNAMIC_BIT;
			if ((BIT&ALREADY_PROCESSORS_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_PROCESSORS_BIT))
			    {
			    fprintf(stderr,"Error 201 on line %d of %s :Object '%s' already has PROCESSORS_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_PROCESSORS_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_PROCESSORS_BIT;    
			if ((BIT&ALREADY_SHADOW_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_SHADOW_BIT))
			    {
			    fprintf(stderr,"Error 206 on line %d of %s :Object '%s' already has SHADOW_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_SHADOW_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_SHADOW_BIT;
			if ((BIT&ALREADY_TEMPLATE_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_TEMPLATE_BIT))
			    {
			    fprintf(stderr,"Error 203 on line %d of %s :Object '%s' already has TEMPLATE_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_TEMPLATE_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_TEMPLATE_BIT;
			if ((BIT&&ALREADY_ALIGN_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_ALIGN_BIT))
			    {
			    fprintf(stderr,"Error 200 on line %d of %s :Object '%s' already has ALIGN_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_ALIGN_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_ALIGN_BIT;
			if ((BIT&ALREADY_DISTRIBUTE_BIT)&&(SYMB_ID(NODE_SYMB(llnd))==ALREADY_DISTRIBUTE_BIT))
			    {
			    fprintf(stderr,"Error 202 on line %d of %s :Object '%s' already has DISTRIBUTE_BIT\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name,SYMB_IDENT(NODE_SYMB(llnd)));
			    errnumber++;
			    }
			else 
			    if (BIT&ALREADY_DISTRIBUTE_BIT) SYMB_ID(NODE_SYMB(llnd))=ALREADY_DISTRIBUTE_BIT;
			}
		    }
		}
              i += strlen("CHECK_DVMBIT");
            } else
	    if (strncmp(&(str[i]),"CHECK_COMBINED_DIR", strlen("CHECK_COMBINED_DIR"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {
              PTR_LLND llnd;
	      if (BIF_LL1(bif))
                {
                  for (llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
                      {
		      PTR_SYMB s=SMNULL;
		      if (NODE_CODE(NODE_OPERAND0(llnd))==ARRAY_REF)
		          s=NODE_SYMB(NODE_OPERAND0(llnd));
		      if (!s) continue;
		      if(s->attr & DISTRIBUTE_BIT) 
		          if (FindRedistributeDir(NODE_OPERAND0(llnd),bif))
                	     {
				if (!FindDynamicDir(NODE_OPERAND0(llnd),bif))
			         {
			         BufPutString("\n!HPF$",0);
				 Puttab();
				 Buf_pointer-=5;
				 BufPutString("DYNAMIC ",0);
			         Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
			         }
			     }
		      if(s->attr & ALIGN_BIT) 
		          if (FindRealignDir(NODE_OPERAND0(llnd),bif))
                	     {
				if (!FindDynamicDir(NODE_OPERAND0(llnd),bif))
			    	 {
			    	 BufPutString("\n!HPF$",0);
				 Puttab();
				 Buf_pointer-=5;
				 BufPutString("DYNAMIC ",0);
			    	 Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
				 }
			     }
		              
		      }
                }
              i += strlen("CHECK_COMBINED_DIR");
            } else
	    if (strncmp(&(str[i]),"CHECK_SHADOW_OP", strlen("CHECK_SHADOW_OP"))== 0)       /* %FIND_MAP : We need to find the corresponding MAP_DIR*/
            {            
	    if (BIF_CODE(bif)==DVM_VAR_DECL)
		    {
		    PTR_LLND ptr_llnd;
		    int OpNumber=0;
		    int Shadow=0;
		    for(ptr_llnd=BIF_LL3(bif);ptr_llnd&&(NODE_CODE(ptr_llnd)==EXPR_LIST);ptr_llnd=NODE_OPERAND1(ptr_llnd))
			{
			if (NODE_CODE(NODE_OPERAND0(ptr_llnd))==SHADOW_OP)
			    Shadow=1;
			else  OpNumber++;
			}
		    if ((Shadow==1)&&!OpNumber) return Buf_address;
		    }	
              i += strlen("CHECK_SHADOW_OP");
            } else	    
	   if (strncmp(&(str[i]),"CHECK_DAC_DIR", strlen("CHECK_DAC_DIR"))== 0)       /* %CHECK_DAC_DIR : We need to find FDVM-directive that can be transformed into DYNAMIC-directive*/
            {
              PTR_LLND llnd;
	      PTR_BFND ptr_bif;
	      int pointer,beg;
	      beg=Buf_pointer;
	      BufPutString("!HPF$",0);
	      Puttab();
	      Buf_pointer-=5;
	      BufPutString("DYNAMIC ",0);
	      pointer=Buf_pointer;
	      if (BIF_LL1(bif))
                {
		    for (llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
                      if ((ptr_bif=FindDistrAlignCombinedDir(NODE_OPERAND0(llnd),bif)))
                	 {
			 if (!CheckNullDistribution(ptr_bif))
			    {
			    Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
			    BufPutString(" ,",0);
			    }			     
			 }
		    Buf_pointer--;
		    BufPutString("\n",0);			 
		}
	      if (pointer==Buf_pointer) Buf_pointer=beg;
	      return Buf_address;
              i += strlen("CHECK_DAC_DIR");
            } else
	  if (strncmp(&(str[i]),"UNPARSE_ON", strlen("UNPARSE_ON"))== 0)       /* %APPEND_ON : We need to append ON clause after FOR statement in INDEPENDENT cycle for DVM_PARALLEL_TASK_DIR*/
            {
              PTR_LLND llnd;
	      if (On_Clause)
                {
                  llnd=FindMapDir(On_Clause,bif);
                  if (llnd)
                    {
                      BufPutString("!HPF$",0);
		      Puttab();
		      Buf_pointer-=5;
		      BufPutString("ON ( ",0);
		      Tool_Unparse2_LLnode(llnd);
		      BufPutString(" ) BEGIN",0);
                    }
		  else  
		    {
		    /*Message ("Error in Unparse, can`t find a corresponding MAP_DIR for DVM_PARALLEL_TASK_DIR", BIF_LINE(bif));*/
		    fprintf(stderr,"Error 195 on line %d of %s : Can`t find a corresponding MAP_DIR\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		    errnumber++;  		      
		    }
		}
              i += strlen("UNPARSE_ON");
            } else
	      if (strncmp(&(str[i]),"CHECK_ACROSS", strlen("CHECK_ACROSS"))== 0) 
            {               
              i = i + strlen("CHECK_ACROSS");
	      if (CheckAcross(bif)) return Buf_address;
            } else
	      if (strncmp(&(str[i]),"CHECK_REDUCTION", strlen("CHECK_REDUCTION"))== 0) 
            {               
              i = i + strlen("CHECK_REDUCTION");
	      if (CheckReduction(bif)) return Buf_address;
            } else
	      if (strncmp(&(str[i]),"FIND_REDUCTION", strlen("FIND_REDUCTION"))== 0) 
            {               
	      ForNodeStmt(BIF_NEXT(bif));
              i = i + strlen("FIND_REDUCTION");
            } else

	      if (strncmp(&(str[i]),"PAR_TASK_MAP", strlen("PAR_TASK_MAP"))== 0)       /* %PAR_TASK_MAP : We need to save ON clause for DVM_PARALLEL_TASK_DIR*/
            {
              On_Clause=BIF_LL1(bif);
              i += strlen("PAR_TASK_MAP");
            } else
	      if (strncmp(&(str[i]),"CHECK_REDISTRIBUTE_ON_MAP", strlen("CHECK_REDISTRIBUTE_ON_MAP"))== 0)       /* %CHECK_REDISTRIBUTE_ON_MAP : We need to find the corresponding MAP_DIR*/
            {
              PTR_LLND llnd,ptr_llnd;
	      if (BIF_LL3(bif))
                {
                  ptr_llnd=ChangeRedistributeOntoTask(BIF_LL3(bif));
		  if (ptr_llnd)
		    {
		      llnd=FindMapDir(ptr_llnd,bif);
		      if (llnd)
			{
			  /*if (HPF_VERSION==1)
			      Tool_Unparse_Symbol(NODE_SYMB(llnd));
			  else  */
			  Tool_Unparse2_LLnode(llnd);
			}
		      else  
			{
			  /*Message ("Error in Unparse, can`t find a corresponding MAP_DIR", BIF_LINE(bif));*/
			  fprintf(stderr,"Error 195 on line %d of %s : Can`t find a corresponding MAP_DIR\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		          errnumber++;
			  Tool_Unparse2_LLnode(BIF_LL3(bif));
			}
		    }
		  else Tool_Unparse2_LLnode(BIF_LL3(bif));
		}
		  i += strlen("CHECK_REDISTRIBUTE_ON_MAP");
            } else
          if (strncmp(&(str[i]),"CHECK_ALLOCATE", strlen("CHECK_ALLOCATE"))== 0)       /* %CHECK_ALLOCATE : We must change the allocation for DVM`s pointers*/
            {
              PTR_LLND llnd;
	      if (NODE_CODE(BIF_LL2(bif))==FUNC_CALL)
                {
                 if (!strcmp(SYMB_IDENT(NODE_SYMB(BIF_LL2(bif))),"allocate"))
		   if ((NODE_CODE(BIF_LL1(bif))==ARRAY_REF)&& (SYMB_ATTR(NODE_SYMB(BIF_LL1(bif)))&DVM_POINTER_BIT))
		     {
		       int count=0;
		       char *str;
		       count=FindPointerDir(NODE_SYMB(BIF_LL1(bif)),bif);
		       if (count)
			 {
			   int i=0;
			   BufPutString("ALLOCATE(", 0);
			   Tool_Unparse2_LLnode(BIF_LL1(bif));
			   BufPutChar('(');		       
			   llnd=NODE_OPERAND0(BIF_LL2(bif));
			   if (!llnd||(NODE_CODE(llnd)!=EXPR_LIST)) 
			       {
			       /*Message("Incorrect call of allocate function",BIF_LINE(bif));*/
			       fprintf(stderr,"Error 196 on line %d of %s : Incorrect call of allocate function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		               errnumber++;
			       BufPutString("))\n", 0);
			       return Buf_address;
			       };
			   if (NODE_OPERAND0(llnd))
			       {
			       if (NODE_CODE(NODE_OPERAND0(llnd))==VAR_REF)
			           {
				     fprintf(stderr,"Error 196 on line %d of %s : Incorrect call of allocate function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		                     fprintf(stderr,"Warning 197 on line %d of %s : You can`t use variable as SDIM array function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
				     errnumber++;
			             /*Message("Incorrect call of allocate function",0);
				     Message("You can`t use variable as SDIM array",BIF_LINE(bif));*/
				     BufPutString("))\n", 0);
			             return Buf_address;
			            }
				else 
				    if (NODE_CODE(NODE_OPERAND0(llnd))==ARRAY_REF)
					{
				         int ar_index=0;
					 PTR_LLND ptr1=LLNULL,ptr2=LLNULL;
					 str=SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd)));
					 llnd=NODE_OPERAND0(NODE_OPERAND0(llnd));
					 if (!llnd)
					     {
			                       for(i=1;i<=count;i++)
			                         {
			                          if (i!=1) BufPutChar(',');
			                          BufPutString(str,0);
			                          BufPutChar('(');
					          BufPutInt(i);
    			                          BufPutChar(')');
			                         }
					       BufPutString("))\n", 0);
			                       return Buf_address;
			                     };
					if (NODE_OPERAND0(llnd)) ptr1=NODE_OPERAND0(llnd);
					if (NODE_OPERAND1(llnd)) ptr2=NODE_OPERAND0(NODE_OPERAND1(llnd));
					if (ptr1&&!ptr2)
					     {
			                       /*Message("Incorrect call of allocate function",BIF_LINE(bif));*/
			                       fprintf(stderr,"Error 196 on line %d of %s : Incorrect call of allocate function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
				    	       errnumber++;
					       BufPutString("))\n", 0);
			                       return Buf_address;
			                     };
					 if (ptr1&&ptr2&&(NODE_CODE(ptr1)==INT_VAL)) 
					     {
					     ar_index=1;
					     if(NODE_CODE(ptr2)==INT_VAL)
					         {
			                           fprintf(stderr,"Error 196 on line %d of %s : Incorrect call of allocate function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		                                   fprintf(stderr,"Warning 198 on line %d of %s : You can`t use 2 integer constants in SDIM array\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
				                   errnumber++;			             			             
						   /*Message("Incorrect call of allocate function",0);
						   Message("You can`t use 2 integer constants in SDIM array",BIF_LINE(bif));*/
			                           BufPutString("))\n", 0);
			                           return Buf_address;
			                         };
					     }
					 else 
					     if (ptr1&&ptr2&&(NODE_CODE(ptr2)==INT_VAL)) 
					         ar_index=2;
					     else 
					         {
			                           fprintf(stderr,"Error 196 on line %d of %s : Incorrect call of allocate function\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
		                                   fprintf(stderr,"Warning 199 on line %d of %s : You can`t use 2 variables in SDIM array\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
				                   errnumber++;			             			             
						   /* Message("Incorrect call of allocate function",0);
						   Message("You can`t use 2 variables in SDIM array",BIF_LINE(bif));*/
			                           BufPutString("))\n", 0);
			                           return Buf_address;
			                         };
					 for(i=1;(i<=count)&&ar_index;i++)
			                 {
			                   if (i!=1) BufPutChar(',');
			                   BufPutString(str,0);
			                   BufPutChar('(');
			                   if (ar_index==1)  
					       {
					       BufPutInt(i);
					       BufPutChar(',');
					       Tool_Unparse2_LLnode(ptr2);
					       }
					   else   
					       {
					       Tool_Unparse2_LLnode(ptr1);
					       BufPutChar(',');
					       BufPutInt(i);
					       }   
			                   BufPutChar(')');
			                 }
			        }
			    }	
			   BufPutChar(')');
			   BufPutString(")\n", 0);
			   return Buf_address;
			 }
		     }
		}
	      else
		if (NODE_CODE(bif)==ASSIGN_STAT) 
		   {
		   llnd=BIF_LL1(bif);
		   if(((NODE_CODE(llnd)==ARRAY_REF)||(NODE_CODE(llnd)==VAR_REF))&&NODE_SYMB(llnd)&&(SYMB_ATTR(NODE_SYMB(llnd))&DVM_POINTER_BIT)) 
		     /*		   if(SYMB_ATTR(NODE_SYMB(llnd))&DVM_POINTER_BIT)*/ 
                      {
		      Tool_Unparse2_LLnode(llnd);
		      BufPutString(" => ",0);
		      Tool_Unparse2_LLnode(BIF_LL2(bif));
		      BufPutString("\n",0);
		      return Buf_address;
		      }
 		   }	
	      i += strlen("CHECK_ALLOCATE");
            } else
	        if (strncmp(&(str[i]),"CHECK_FUNC", strlen("CHECK_FUNC"))== 0)       /* %CHECK_FUNC : We must change function ALLOCATE*/
            { 
	      int count=0;
	      if (NODE_SYMB(bif))
		if (!strcmp(SYMB_IDENT(NODE_SYMB(bif)),"ALLOCATE")||
		    !strcmp(SYMB_IDENT(NODE_SYMB(bif)),"allocate"))
		  Treat_Flag("(COMMENT)",&count,1);
	      i += strlen("CHECK_FUNC");
            } else
	      if (strncmp(&(str[i]),"DECLARATION", strlen("DECLARATION"))== 0)               /* %DECLARATION : We need to change declaration of Pointer descriptor */
            {
              int count=0;
	      if (BIF_LL1(bif)) count=FindPointerDeclaration(BIF_LL1(bif));
	      if (!count) 
		{
		  count=0;
		  Treat_Flag("(POINTER)",&count, 1);
		}
              i += strlen("DECLARATION");
            } else
	   if (strncmp(&(str[i]),"DESCRIPTOR", strlen("DESCRIPTOR"))== 0)               /* %DECLARATION : We need to change declaration of Pointer descriptor */
            {
              PTR_LLND ptr,llnd;
	      if (BIF_LL1(bif))
	         {  
	          for(ptr=BIF_LL1(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
		      {
		      if (NODE_OPERAND0(ptr))
		          {
			  llnd=FindPointerDescriptor(NODE_OPERAND0(ptr),bif);
			  if (NODE_OPERAND0(llnd)) continue;
			  if (!llnd) 
			      {
			      /*Message("Can`t find a descriptor for POINTER",BIF_LINE(bif));*/
			      fprintf(stderr,"Error 222 on line %d of %s : Can`t find a descriptor for POINTER\n",BIF_LINE(bif),BIF_FILE_NAME(bif)->name);
			      errnumber++;			             			             
			      Tool_Unparse2_LLnode(NODE_OPERAND0(ptr));
			      }
			  else Tool_Unparse2_LLnode(llnd);
			  BufPutChar(',');
			  }
		       }
		   Buf_pointer--;   	
		 }        
	      i += strlen("DESCRIPTOR");
            } else
	  if (strncmp(&(str[i]),"PTR_ARRAY", strlen("PTR_ARRAY"))== 0)               /* %DECLARATION : We need to change declaration of Pointer descriptor */
            {
	      if (BIF_LL1(bif))
	         {  
	         ArrayOfPointerDeclaration(bif);
		 PointerDeclaration(bif);
		 return Buf_address;
		 }        
	      i += strlen("PTR_ARRAY");
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
		  int protect = 0; /*protect = NULL*/
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
		{
                  Message (" *** Unknown bif node COMMAND *** ",0);
		  Message (&(str[i]),0);
		}
        }
       else
         {
           BufPutChar( c);
           i++;
         }
      c = str[i];
    }
  if (AfterDOLabel&&(HPF_VERSION==2)) UnparseEndofCircle(bif);    
  if (TASK_PROC_GENERATE&&(HPF_VERSION==2)&&(variant==DVM_ON_DIR)) 
      {
      ON_BLOCK=1;
      ON_BEGIN=Buf_pointer;
      };
  return Buf_address;
}

void DefineHPF1()
{
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].str = create_unp_str( NAME);
#include"unparse1.hpf"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].fct = NULL;
#include"unparse1.hpf"
#undef DEFNODECODE
}
void DefineHPF2()
{
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].str = create_unp_str( NAME);
#include"unparse.hpf"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT) Unparse_Def[SYM].fct = NULL;
#include"unparse.hpf"
#undef DEFNODECODE
}

void Init_HPFUnparser()
{
  int i,j;
  CommentOut=1; /*Ignore comment*/
      if (Parser_Initiated != Fortran_Initialized)
        {
	  /*printf("\nHPF_VERSION = %d",HPF_VERSION);*/
	  if (HPF_VERSION==1)  DefineHPF1();
	    else DefineHPF2();
          Parser_Initiated = Fortran_Initialized;
          /* set the first tabulation */
          TabNumber = 1;
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
  /* Type definition for all BIF,LL,etc NODES */ 
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT,f1,f2,f3,f4,f5) node_code_kind[SYM] = NT;
#include"bif_node.def"
#undef DEFNODECODE
  /* setbuffer to 0 */
  Buf_pointer = 0;  
  Buf_address = &(UnpBuf[0]); /* may be reallocated */
  memset(UnpBuf, 0, MAXLENGHTBUF);
}

/**************************************************************/
int UnparseFDVMProgram(fout,fi)
     FILE *fout;
     PTR_FILE fi;
{
  
  Init_HPFUnparser();
  current_file=fi;
  errnumber=0;
  fprintf(fout,"%s",filter(Tool_Unparse_Bif(fi->head_bfnd)));
  return errnumber;
}

PTR_LLND FindMapDir(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
	  { 
	    if (BIF_CODE(bif)!=DVM_MAP_DIR) continue;
	    if (patternMatchExpression(ptr_llnd,BIF_LL1(bif))) 
	      {
		return BIF_LL2(bif);
	      }
	  }
return NULL;    
}

int NumberOfForNode(PTR_BFND ptrbif,int *label)
{
int incircle=0;
PTR_BFND bif,end=FindEndOfBlock(ptrbif);
int count=0;
if (!BIF_LABEL(ptrbif)) return 0;
for(bif=FindBeginingOfBlock(ptrbif);bif&&(bif!=end);bif=BIF_NEXT(bif))
      { 
 	if (incircle&&(BIF_CODE(bif)==GOTO_NODE)) 
		{
	           if (BIF_LL3(bif)&&NODE_LABEL (BIF_LL3(bif)))
                	{
	                  if ( LABEL_STMTNO (NODE_LABEL (BIF_LL3(bif)))==LABEL_STMTNO (BIF_LABEL(ptrbif)))
				{
				*label=LABEL_STMTNO (BIF_LABEL(ptrbif));
				}
        	        }

		}
	if (BIF_CODE(bif)!=FOR_NODE) continue;
	if (BIF_LABEL_USE (bif))
	  {
	   if (LABEL_STMTNO (BIF_LABEL_USE (bif))==LABEL_STMTNO (BIF_LABEL(ptrbif))) 
		{
			count++;
	        	incircle=1;
		}

	  }
       }
return count;    
}

int FindPointerDir(PTR_SYMB symb,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
	  { 
	    PTR_LLND llnd;
	    if (BIF_CODE(bif)!=DVM_POINTER_DIR) continue;
            for(llnd=BIF_LL1(bif);llnd;llnd=NODE_OPERAND1(llnd))
	    if (!strcmp(SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd))),SYMB_IDENT(symb))) 
	      {
		int count=0;
		char *str;
		str=funparse_llnd(BIF_LL2(bif));
		for(;*str!='\0';str++)
		  if (*str==':') count++;
		return count;		  
	      }
	  }
return 0;    
}

int Puttab() 
{                  
  int j;
  for (j = 0; j < TabNumber; j++)
    if (j>0)
      BufPutString ("   ",0);
    else
      BufPutString ("      ",0); /* cychen */
return j;
}

PTR_LLND ChangeRedistributeOntoTask(PTR_LLND llnd)
{
if (!llnd) return LLNULL;
/*if (NODE_CODE(llnd)!=EXPR_LIST) return LLNULL;
if (NODE_CODE(NODE_OPERAND0(llnd))!=ARRAY_REF) return LLNULL;
if (!(SYMB_ATTR(NODE_SYMB(NODE_OPERAND0(llnd)))&TASK_BIT)) return LLNULL;*/
if (NODE_CODE(llnd)!=ARRAY_REF) return LLNULL;
if (!(SYMB_ATTR(NODE_SYMB(llnd))&TASK_BIT)) return LLNULL;
/*return NODE_OPERAND0(llnd);*/
return llnd;
}

int Find_SaveSymbol(PTR_LLND llnd)
{
int i;
if (NODE_CODE(llnd)!=VAR_REF)  return 0;
for (i=0;i<Number_Of_Symbol;i++)
  if (SymbolID[i]==NODE_SYMB(llnd)) return 1;
SymbolID[Number_Of_Symbol++]=NODE_SYMB(llnd);
if (Number_Of_Symbol==MAXFLAG)  
  {
    Message("Too many reductions variables; sorry",0);
    Number_Of_Symbol--;
  }
return 0;
}

int FindPointerDeclaration(PTR_LLND llnd)
{
int count=0;
for(;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
  if ((NODE_CODE(NODE_OPERAND0(llnd))==VAR_REF)||(NODE_CODE(NODE_OPERAND0(llnd))==ARRAY_REF))
    {
	/*printf("\nFindPointerDecl : %s ",SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd))));*/
      if (!(SYMB_ATTR(NODE_SYMB(NODE_OPERAND0(llnd)))&DVM_POINTER_BIT)) 
	{
	  /*printf(": %s ",SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd))));*/
	  /*if(strcmp(SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd))),"heap"))*/
 	  if(!(SYMB_ATTR(NODE_SYMB (NODE_OPERAND0(llnd)))&HEAP_BIT))
	    {
	      if (count) BufPutChar(',');
	      /*printf(": %s",SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd))));*/
	      Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
	      count++;
	      /*BufPutString("QWERTY\n",0);*/
	    }
	};
    }
  else 
    {
      if (count) BufPutChar(',');
      Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
      count++;
    };
return count;
}

int FindCommonHeapDeclaration(PTR_LLND llnd)
{
int count=0;
for(;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
  if ((NODE_CODE(NODE_OPERAND0(llnd))==VAR_REF)||(NODE_CODE(NODE_OPERAND0(llnd))==ARRAY_REF))
    {
    if(!(SYMB_ATTR(NODE_SYMB (NODE_OPERAND0(llnd)))&HEAP_BIT))
       {
       if (count) BufPutChar(',');
       Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
       count++;
       }
    }
  else 
    {
      if (count) BufPutChar(',');
      Tool_Unparse2_LLnode(NODE_OPERAND0(llnd));
      count++;
    };
return count;
}

PTR_LLND FindRedistributeDir(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
     { 	
	if (BIF_CODE(bif)!=DVM_REDISTRIBUTE_DIR) continue;
        if (BIF_LL1(bif)) 
	    {
	    PTR_LLND llnd;
	    for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		{
		if (patternMatchExpression(ptr_llnd,NODE_OPERAND0(llnd)))
		    return NODE_OPERAND0(llnd);
		}
	    } 	 
     }	
return NULL;    
}

PTR_LLND FindRealignDir(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
     { 	
	if (BIF_CODE(bif)!=DVM_REALIGN_DIR) continue;
        if (BIF_LL1(bif)) 
	    {
	    PTR_LLND llnd;
	    for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		{
		if (patternMatchExpression(ptr_llnd,NODE_OPERAND0(llnd)))
		    return NODE_OPERAND0(llnd);
		}
	    } 	 
     }	
return NULL;    
}

PTR_LLND FindDynamicDir(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
     { 	
	if (BIF_CODE(bif)!=DVM_DYNAMIC_DIR) continue;
        if (BIF_LL1(bif)) 
	    {
	    PTR_LLND llnd;
	    for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		{
		if (patternMatchExpression(ptr_llnd,NODE_OPERAND0(llnd)))
		    return NODE_OPERAND0(llnd);
		}
	    } 	 
     }
/*for(bif=current_file->head_bfnd;bif;bif=BIF_NEXT(bif))
     { 	
	PTR_LLND ptr;
	int ok=0;
	if (BIF_CODE(bif)!=DVM_VAR_DECL) continue;
	for(ptr=BIF_LL3(bif);ptr&&(NODE_CODE(ptr)==EXPR_LIST);ptr=NODE_OPERAND1(ptr))
	    if (NODE_CODE(NODE_OPERAND0(ptr))==DYNAMIC_OP) ok=1;
        if (BIF_LL1(bif)) 
	    {
	    PTR_LLND llnd;
	    for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		{
		if (patternMatchExpression(ptr_llnd,NODE_OPERAND0(llnd)))
		    if (ok)
			return NODE_OPERAND0(llnd);
		    else return 0;
		}
	    } 	 
     }
*/     	
return NULL;    
}

int UnparseEndofCircle(PTR_BFND bif)
{
int TabNum;
int i,count=0,label=0;
TabNum=TabNumber;
count=NumberOfForNode(bif,&label);
if (label) BufPutInt(label);
if(count)
    {
      if (TabNumber>1) TabNumber--;
      for(i=0;i<count;i++)
    	{
	  if (i==count-1)
	    {
	     int k = 0 ;
	     if (Get_Flag_val("(TASK_DIR)",&k))
	       { 
	         BufPutString("!HPF$",0);
	         Puttab();
		 Buf_pointer-=5;
		 BufPutString("END ON\n",0);
		 if (TabNumber>1) TabNumber--;
		 k=0;
		 Treat_Flag("(TASK_DIR)",&k,-1);
		 TabNum--;
	       }
	    }
	  Puttab();
	  if (TabNumber>1) TabNumber--;    
	  BufPutString("end do\n",0);
	}
      TabNumber=TabNum;
    }
return count;
/*21052001*/
}

PTR_BFND FindDistrAlignCombinedDir(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
     { 	
	if ((BIF_CODE(bif)==DVM_DISTRIBUTE_DIR)||
	    (BIF_CODE(bif)==DVM_ALIGN_DIR)||
	    (BIF_CODE(bif)==DVM_VAR_DECL))
	    { 
            if (BIF_LL1(bif)) 
    		{
		PTR_LLND llnd;
		for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		    {
		    if (patternMatchExpression(ptr_llnd,NODE_OPERAND0(llnd)))
			{
			return bif;
			}
		    }
		}
	    }		 	 
     }	
return NULL;    
}

PTR_LLND FindPointerDescriptor(PTR_LLND ptr_llnd,PTR_BFND ptr_bif)
{
PTR_BFND bif,end=FindEndOfBlock(ptr_bif);
for(bif=FindBeginingOfBlock(ptr_bif);bif&&(bif!=end);bif=BIF_NEXT(bif))
     { 	
	if (BIF_CODE(bif)!=VAR_DECL) continue;
        if (BIF_LL1(bif)) 
	    {
	    PTR_LLND llnd;
	    for(llnd=BIF_LL1(bif);llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
		{
		if (!strcmp(SYMB_IDENT(NODE_SYMB(ptr_llnd)),SYMB_IDENT(NODE_SYMB(NODE_OPERAND0(llnd)))))
		    return NODE_OPERAND0(llnd);
		}
	    } 	 
     }	
return NULL;    
}

int ArrayOfPointerDeclaration(PTR_BFND bif)
{
PTR_LLND llnd=BIF_LL1(bif);
int ok=1,empty=1;
int count=0;
for (;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
    {
    PTR_LLND descr;
    descr=FindPointerDescriptor(NODE_OPERAND0(llnd),bif);
    if (ok&&!count)    
	{
	char *str=NULL;
	str=funparse_llnd(BIF_LL2(bif));
	for(;*str!='\0';str++)
	 if (*str==':') count++;
	}
    if (count&&descr&&NODE_OPERAND0(descr))
	{ 
	char FLAG[10];
	int i=0;
	if (count>10) 
	    {
	    Message("Can`t work : too many dimensions in DVM_POINTER_DIR",0);
	    return 0;
	    }
	sprintf(FLAG,"(PTRTYPE%i)",count);    
	if (ok&&!Get_Flag_val(FLAG,&i)) 
	    {
	    i=0;
	    Treat_Flag(FLAG, &i,1);
	    GenerateType(count, BIF_LL3(bif));
	    };
	if(ok)
	    {
	    Puttab();
	    BufPutString("TYPE (PTR_ARRAY",0);
	    BufPutInt(count);
	    BufPutString(") :: ",0);
	    ok=0;
	    empty=0;
	    BufPutString(SYMB_IDENT(NODE_SYMB(descr)),0);
	    SYMB_ATTR(NODE_SYMB(descr))|=DVM_POINTER_ARRAY_BIT;
	    BufPutChar('(');
	    Tool_Unparse2_LLnode(NODE_OPERAND0(descr));
	    BufPutChar(')');
	    continue;
	    }
	if (!ok)
	    {
	    empty=0;
	    BufPutChar(',');
	    BufPutString(SYMB_IDENT(NODE_SYMB(descr)),0);
	    SYMB_ATTR(NODE_SYMB(descr))|=DVM_POINTER_ARRAY_BIT;
	    BufPutChar('(');
	    Tool_Unparse2_LLnode(NODE_OPERAND0(descr));
	    BufPutChar(')');
	    }
	}
    }	    
if (!empty) BufPutChar('\n');
return 1;
}

void PointerDeclaration(PTR_BFND bif)
{
PTR_LLND llnd=BIF_LL1(bif);
int ok=1,empty=1;
int count=0;
for (;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
    {
    PTR_LLND descr;
    descr=FindPointerDescriptor(NODE_OPERAND0(llnd),bif);
    if (ok&&!count)
	    {
	    char *str=NULL;
	    str=funparse_llnd(BIF_LL2(bif));
	    for(;*str!='\0';str++)
		 if (*str==':') count++;
	    }    
    if ((count)&&descr&&!NODE_OPERAND0(descr))
	{ 
	if(ok)
	    {
	    Puttab();
            Tool_Unparse2_LLnode(BIF_LL3(bif));
	    BufPutString(", POINTER, DIMENSION (",0);
	    Tool_Unparse2_LLnode(BIF_LL2(bif));
	    BufPutString(") :: ",0);
	    ok=0;
	    empty=0;
	    BufPutString(SYMB_IDENT(NODE_SYMB(descr)),0);
	    continue;
	    }
	if (!ok)
	    {
	    empty=0;
	    BufPutChar(',');
	    BufPutString(SYMB_IDENT(NODE_SYMB(descr)),0);
	    }
	}
    }	    
if (!empty) BufPutChar('\n');
}

void GenerateType(int count, PTR_LLND llnd_type)
{
    int i;
    Puttab();
    BufPutString("TYPE PTR_ARRAY",0);
    BufPutInt(count);
    BufPutChar('\n');
    TabNumber++;
    Puttab();
    Tool_Unparse2_LLnode(llnd_type);
    BufPutString(", POINTER, DIMENSION (",0);
    for(i=0;i<count;i++)
	{
         BufPutString(":,",0);
         }
    Buf_pointer--;
    BufPutString(") :: PTR\n",0);
    BufPutString("!HPF$",0);
    Puttab();
    Buf_pointer-=5;
    BufPutString("DYNAMIC PTR",0);
    TabNumber--;
    BufPutChar('\n');
    Puttab();
    BufPutString("END TYPE PTR_ARRAY",0);
    BufPutInt(count);
    BufPutChar('\n');
}

void gen_hpf_name (char *filename)
{
   register int i;
   hpfname = (char *) malloc((unsigned)(strlen(filename)+4));
   strcpy (hpfname,filename);
   for (i = strlen(filename)-1 ; i >= 0 ; i --)
        if ( filename[i] == '.' )
             break;
 hpfname[i+1]='h';
 hpfname[i+2]='p';
 hpfname[i+3]='f';
 hpfname[i+4]='\0';
}
   
int CheckNullDistribution(PTR_BFND bif)
{
if (BIF_CODE(bif)==DVM_DISTRIBUTE_DIR)
    {
    if (!BIF_LL2(bif)&&!BIF_LL3(bif))	     
	return 1;
    };
if (BIF_CODE(bif)==DVM_ALIGN_DIR)
    {
    if (!BIF_LL2(bif)&&!BIF_LL3(bif))	     
	return 1;
    };    
if (BIF_CODE(bif)==DVM_VAR_DECL)
    {
    PTR_LLND llnd=BIF_LL3(bif);
    for(;llnd&&(NODE_CODE(llnd)==EXPR_LIST);llnd=NODE_OPERAND1(llnd))
	{
	 /*BufPutInt(NODE_CODE(NODE_OPERAND0(llnd)));*/
         if (NODE_CODE(NODE_OPERAND0(llnd))==DYNAMIC_OP)
		return 1;
	 if (NODE_CODE(NODE_OPERAND0(llnd))==DISTRIBUTE_OP)
	    if (!NODE_OPERAND0(NODE_OPERAND0(llnd))&&!NODE_OPERAND1(NODE_OPERAND0(llnd))) 
		return 1;		
	 if (NODE_CODE(NODE_OPERAND0(llnd))==ALIGN_OP)
	    if (!NODE_OPERAND0(NODE_OPERAND0(llnd))&&!NODE_OPERAND1(NODE_OPERAND0(llnd))) 
		return 1;
	};
    }
/*BufPutInt(1234567);*/
return 0;
}

void ResetSymbolId()
{
int i,j;
char FLAG[10];
PTR_SYMB symb;
for (symb = current_file->head_symb; symb ; symb = SYMB_NEXT (symb))
    SYMB_ID (symb) = 0;
for (j=0;j<10;j++)
    {
    sprintf(FLAG,"(PTRTYPE%i)",j);
    i=0;
    if (Get_Flag_val(FLAG,&i)) 
    	{
    	i=0;
    	Treat_Flag(FLAG, &i,-1);
    	};
    }
}

PTR_BFND FindBeginingOfBlock(PTR_BFND ptr_bif)
{
PTR_BFND bif,ptr=NULL;
for(bif=current_file->head_bfnd;bif&&(bif!=ptr_bif);bif=BIF_NEXT(bif))
    { 	
    if ((BIF_CODE(bif)==PROG_HEDR)||
        (BIF_CODE(bif)==PROC_HEDR)||
        (BIF_CODE(bif)==PROS_HEDR))
        ptr=bif;
    }
return ptr;
}

PTR_BFND FindEndOfBlock(PTR_BFND ptr_bif)
{
PTR_BFND bif;
for(bif=ptr_bif;bif;bif=BIF_NEXT(bif))
    { 	
    if (BIF_CODE(bif)==CONTROL_END)
	if ((BIF_CODE(BIF_CP(bif))==PROG_HEDR)||
    	    (BIF_CODE(BIF_CP(bif))==PROC_HEDR)||
    	    (BIF_CODE(BIF_CP(bif))==PROS_HEDR))
	    {
	    return bif;
	    }	    
    }
return NULL;
}
    
int CheckAcross(PTR_BFND bif)
{
PTR_LLND llnd;
if (!bif) return 0;
for (llnd=BIF_LL2(bif);llnd&&NODE_CODE(llnd)==EXPR_LIST;llnd=NODE_OPERAND1(llnd))
    {
    if (NODE_OPERAND0(llnd)&&(NODE_CODE(NODE_OPERAND0(llnd))==ACROSS_OP))
	return 1;
    }
return 0;
}
int CheckReduction(PTR_BFND bif)
{
PTR_LLND llnd;
if (!bif) return 0;
for (llnd=BIF_LL2(bif);llnd&&NODE_CODE(llnd)==EXPR_LIST;llnd=NODE_OPERAND1(llnd))
    {
    if (NODE_OPERAND0(llnd)&&(NODE_CODE(NODE_OPERAND0(llnd))==REDUCTION_OP))
	return 1;
    }
return 0;
}
    
int IfReduction(PTR_LLND e1, PTR_LLND e2)
{ 
  if(!e1||!e2)  return(0); 
  if(NODE_CODE(e1) != NODE_CODE(e2))
      return(0);
  if(NODE_CODE(e1) != VAR_REF && NODE_CODE(e1) != ARRAY_REF)
      return(0);
  if(NODE_SYMB(e1) != NODE_SYMB(e2))
    return(0);
  if(NODE_CODE(e1) == ARRAY_REF && !patternMatchExpression(NODE_OPERAND0(e1),NODE_OPERAND0(e2)))
    return(0);
  return (1);    
}   
int FindRedInExpr(PTR_LLND red, PTR_LLND expr)
{
if(!expr)  return 0;
if (!red) return 0;
if (NODE_CODE(red)!=VAR_REF && NODE_CODE(red)!=ARRAY_REF) return 0;

if(NODE_CODE(red)==VAR_REF && NODE_CODE(red) == NODE_CODE(expr))  
  {
  if (NODE_SYMB(red)== NODE_SYMB(expr))
      return 1;
  else return 0;
  }

if(NODE_CODE(red)==ARRAY_REF && NODE_CODE(red) == NODE_CODE(expr))
 { 
   if (NODE_SYMB(red) == NODE_SYMB(expr))
      return(patternMatchExpression(NODE_OPERAND0(red),NODE_OPERAND0(expr)));
 }
return (FindRedInExpr(red,NODE_OPERAND0(expr))+FindRedInExpr(red,NODE_OPERAND1(expr)));
}

PTR_LLND AddToReductionList(PTR_LLND redlist, PTR_LLND newred)
{
PTR_LLND new_node,ptr;
new_node=make_llnode(EXPR_LIST,newred,LLNULL,SMNULL);
if (new_node)
    {
    if (redlist)
    	{
    	ptr=Follow_Llnd(redlist,2);
    	NODE_OPERAND1(ptr)=new_node;
    	}
    if (!redlist) redlist=new_node;
    }
else
    {
    return NULL;
    }
return redlist; 
}

int FindInNewList(PTR_LLND newlist, PTR_LLND red)
{
PTR_LLND ExprList;
if (!newlist) return 0;

if (!red) return 0;
if (NODE_CODE(red)!=VAR_REF && NODE_CODE(red)!=ARRAY_REF) return 0;
for (ExprList=newlist;ExprList&&(NODE_CODE(ExprList)==EXPR_LIST);ExprList=NODE_OPERAND1(ExprList))
    {
    if (NODE_CODE(NODE_OPERAND0(ExprList))==VAR_REF || NODE_CODE(NODE_OPERAND0(ExprList))==ARRAY_REF )
	if (NODE_SYMB(NODE_OPERAND0(ExprList))==NODE_SYMB(red))
	    return 1;
    }
return 0;
}

int isForNodeEndStmt(PTR_BFND stmt)
{
 PTR_LABEL lab, do_lab;
 PTR_BFND parent;
 if(!(lab=BIF_LABEL(stmt)) && BIF_CODE(stmt) != CONTROL_END) /*the statement has no label and*/
   return(0);                                               /*is not ENDDO */
 parent = BIF_CP(stmt);
 if (parent)
    {
    if(BIF_CODE(parent)!=FOR_NODE)  /*parent isn't DO statement*/
      return(0);
    do_lab = BIF_LABEL_USE(parent); /* label of loop end or NULL*/
    if(do_lab) /*  DO statement with label */
      if(lab && LABEL_STMTNO(lab) == LABEL_STMTNO(do_lab))
                           /* the statement label is the label of loop end*/
        return(1);
      else
        return(0);
    else   /*  DO statement without label */
      if(BIF_CODE(stmt) == CONTROL_END)
        return(1);
      else
        return(0);
    }
 else return (0);
}
int ForNodeStmt(PTR_BFND stmt)
{
PTR_BFND bif;
int count=0,label;
for(bif=stmt;bif;bif=BIF_NEXT(bif))
	  { 
	    /*BufPutString("Count=",0);
            BufPutInt(count);*/
	    if (BIF_CODE(bif)==ASSIGN_STAT) 
		{
		  if (FindRedInExpr(BIF_LL1(bif),BIF_LL2 (bif)))
			{
			if (BIF_LL1(bif)&&NODE_SYMB(BIF_LL1(bif))&&(!IS_DISTRIBUTE_ARRAY(NODE_SYMB(BIF_LL1(bif))))&&(!SYMB_DOVAR(NODE_SYMB(BIF_LL1(bif)))))
				{
				/*BufPutString("Reduction var :",0);
				Tool_Unparse2_LLnode(BIF_LL1(bif));
				BufPutString("\n",0);*/
				if (!FindInNewList(NewSpecList,BIF_LL1(bif)))
				  if(!FindInNewList(ReductionList,BIF_LL1(bif)))
				   {		
				   ReductionList=AddToReductionList(ReductionList,BIF_LL1(bif));
				   /*BufPutString("Reduction LIST :",0);
				   Tool_Unparse2_LLnode(ReductionList);
				   BufPutString("\n",0);*/
                                   }
				}
			}
                continue;
		}
	    if (BIF_CODE(bif)==FOR_NODE) 
		{
		count++;
		continue;
		}
	    else
		{
		if (isForNodeEndStmt(bif))
		   {
		   if (BIF_CODE(bif)==CONTROL_END)
			count--;
		   else
			count-=NumberOfForNode(bif,&label);
		   }
		}			
	   if (!count) break;
	   }
return 0;    
}

PTR_LLND FreeReductionList(PTR_LLND redlist)
{
PTR_LLND llnd,ptr;
llnd=redlist;
while((ptr=llnd))
  {
    if (NODE_OPERAND1(llnd) == NULL)
        break;
    llnd = NODE_OPERAND1(llnd);
    NODE_OPERAND0(ptr)=NULL;
    NODE_OPERAND1(ptr)=NULL;
    free(ptr);
   }
return (NULL);
}

int ForNodeLabel(PTR_BFND stmt)
{
PTR_BFND bif;
int count=0,label;
/*int LABEL=0;*/
for(bif=stmt;bif;bif=BIF_NEXT(bif))
	  { 
	    if (BIF_CODE(bif)==FOR_NODE) 
		{
		count++;
		continue;
		}
	    else
		{
		if (isForNodeEndStmt(bif))
		   {
		   if (BIF_CODE(bif)==CONTROL_END)
			count--;
		   else
			count-=NumberOfForNode(bif,&label);
		   }
		}			
	   if (!count) break;
	   }
return 0;    
}









