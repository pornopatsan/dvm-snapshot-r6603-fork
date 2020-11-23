/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/* This file is used to automatically generate a "#include" header */
/* 
mkCextern $SAGEROOT/lib/newsrc/low_level.c > ! $SAGEROOT/lib/include/ext_low.h
mkC++extern $SAGEROOT/lib/newsrc/low_level.c > ! $SAGEROOT/lib/include/extcxx_low.h
*/

#include <stdio.h>

#include <stdlib.h>
#include <stdarg.h> /* ANSI variable argument header */

#include "compatible.h"   /* Make different system compatible... (PHB) */
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#include "vpc.h"
#include "macro.h"
#include "ext_lib.h"

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
extern void removeFromCollection(void *pointer);
#endif

#define MAX_FILE 1000 /*max number of files in a project*/
#define MAXFIELDSYMB 10
#define MAXFIELDTYPE 10
#define MAX_SYMBOL_FOR_DUPLICATE 1000
char Current_File_name[256];

int debug =NO; /* used in db.c*/

PTR_FILE pointer_on_file_proj;
static int number_of_bif_node = 0;
int number_of_ll_node = 0; /* this counters are useless anymore ??*/
static int number_of_symb_node  = 0;
static int number_of_type_node = 0;
char  *default_filename;
int Warning_count = 0;

/* FORWARD DECLARATIONS (phb) */
int buildLinearRepSign();
int makeLinearExpr_Sign();
int getLastLabelId();
int isItInSection();
int Init_Tool_Box();
void Message();
PTR_BFND rec_num_near_search();
PTR_BFND Redo_Bif_Next_Chain_Internal();
PTR_SYMB duplicateSymbol();
void Redo_Bif_Next_Chain();
PTR_LABEL getLastLabel();
PTR_BFND getNodeBefore ();
char *filter();
PTR_BFND getLastNodeList();
int *evaluateExpression();
PTR_SYMB duplicateSymbolOfRoutine();
void SetCurrentFileTo();
void UnparseProgram_ThroughAllocBuffer();

extern int write_nodes();
extern char* Tool_Unparse2_LLnode(); 
extern void Init_Unparser();
extern void Set_Function_Language();
extern void Unset_Function_Language();
extern char* Tool_Unparse_Bif ();
extern char* Tool_Unparse_Type();
extern void BufferAllocate();

int out_free_form;
int out_upper_case;
int out_line_unlimit;
PTR_SYMB last_file_symbol;

static int CountNullBifNext = 0; /* for internal debugging */

/* records propoerties and type of node */
char node_code_type[LAST_CODE];
/* Number of argument-words in each kind of tree-node.  */
int node_code_length[LAST_CODE];
enum typenode node_code_kind[LAST_CODE];
/* special table for infos on type and symbol */
char info_type[LAST_CODE][MAXFIELDTYPE];
char info_symb[LAST_CODE][MAXFIELDSYMB];
char general_info[LAST_CODE][MAXFIELDSYMB];

/*static struct bif_stack_level   *stack_level = NULL;*/
/*static struct bif_stack_level *current_level = NULL;*/

PTR_BFND  getFunctionHeader();

/*****************************************************************************
 *                                                                           *
 *                   Procedure of general use                                *
 *                                                                           *
 *****************************************************************************/

/* Modified to return a pointer (64bit clean) (phb) */
/***************************************************************************/
char* xmalloc(int size)
{
  char *val;
  val = (char *) malloc (size);
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,val, 0);  
#endif
  if (val == 0)
    Message("Virtual memory exhausted (malloc failed)",0);
  return val;
}

/* list of allocated data */
static ptstack_chaining Current_Allocated_Data = NULL;
static ptstack_chaining First_STACK= NULL;

/***************************************************************************/
void make_a_malloc_stack()
{
  ptstack_chaining pt;
    
  pt = (ptstack_chaining) malloc(sizeof(struct stack_chaining));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,pt, 0);
#endif
  if (!pt)
    {
      Message("sorry : out of memory\n",0);
      exit(1);
    }
  
  if (Current_Allocated_Data)
    Current_Allocated_Data->next = pt;
  pt->first = NULL;
  pt->last = NULL;
  pt->prev = Current_Allocated_Data;
  if (Current_Allocated_Data)
    pt->level = Current_Allocated_Data->level +1;
  else
    pt->level = 0;
/*  printf("make_a_malloc_stack %d \n",pt->level);*/
  Current_Allocated_Data = pt;
  if (First_STACK == NULL)
    First_STACK = pt;
}

/***************************************************************************/
void myfree()
{
  ptstack_chaining pt;
  ptchaining pt1, pt2;
  if (!Current_Allocated_Data)
     {
      Message("Stack not defined\n",0);
      exit(1);
    }

  pt2 = Current_Allocated_Data->first;

/*  printf("myfree %d \n", Current_Allocated_Data->level);*/
  while (pt2)
    {
#ifdef __SPF
      removeFromCollection(pt2->zone);
#endif
      free(pt2->zone);
      pt2->zone = 0;
      pt2 = pt2->list;
    }

  pt2 = Current_Allocated_Data->first;
  while (pt2)
    {
      pt1 = pt2;
      pt2 = pt2->list;
#ifdef __SPF
      removeFromCollection(pt1);
#endif
      free(pt1);
    } 
  pt = Current_Allocated_Data;
  Current_Allocated_Data = pt->prev;
  Current_Allocated_Data->next = NULL;
#ifdef __SPF
  removeFromCollection(pt);
#endif
  free(pt);
}


/***************************************************************************/
char* mymalloc(int size)
{
  char *pt1;
  ptchaining pt2;
  if (!Current_Allocated_Data)
    {
      Message("Allocated Stack not defined\n",0);
      exit(1);
    }

/*  if (Current_Allocated_Data->level > 0)
    printf("mymalloc  %d \n", Current_Allocated_Data->level); */
  pt1 = (char *) malloc(size);
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,pt1, 0);
#endif
  if (!pt1)
    {
      Message("sorry : out of memory\n",0);
      exit(1);
    }
 
  pt2 = (ptchaining) malloc(sizeof(struct chaining));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,pt2, 0);
#endif
  if (!pt2 )
    {
      Message("sorry : out of memory\n",0);
      exit(1);
    }
  
  pt2->zone = pt1;
  pt2->list = NULL;

  if (Current_Allocated_Data->first == NULL)
    Current_Allocated_Data->first = pt2;
  
  if (Current_Allocated_Data->last == NULL)
    Current_Allocated_Data->last = pt2;
  else
    {
      Current_Allocated_Data->last->list = pt2;
      Current_Allocated_Data->last = pt2;
    }
  return pt1;
}

/***************** Provides infos on nodes ********************************
 *                                                                        *
 *     based on the table info in include dir *.def                       *
 *                                                                        *
 **************************************************************************/

/***************************************************************************/
int isATypeNode(variant)
int variant;
{
  return (TYPENODE == (int) node_code_kind[variant]);
}

/***************************************************************************/
int isASymbNode(variant)
int variant;
{
  return (SYMBNODE == (int) node_code_kind[variant]);
}

/***************************************************************************/
int isABifNode(variant)
int variant;
{
  return (BIFNODE == (int) node_code_kind[variant]);
}

/***************************************************************************/
int isALoNode(variant)
int variant;
{
  return (LLNODE == (int) node_code_kind[variant]);
}

/***************************************************************************/
int hasTypeBaseType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("hasTypeBaseType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][2] == 'b')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isStructType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("isStructType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][0] == 's')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isPointerType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("isPointerType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][0] == 'p')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isUnionType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("isUnionType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][0] == 'u')
        return TRUE;
    else
        return FALSE;
}


/***************************************************************************/
int isEnumType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("EnumType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][0] == 'e')
        return TRUE;
    else
        return FALSE;
}


/***************************************************************************/
int hasTypeSymbol(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("hasTypeSymbol not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][1] == 's')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isAtomicType(variant)
int variant;
{
    if (!isATypeNode(variant))
    {
#if !__SPF
        Message("isAtomicType not applied to a type node", 0);
#endif
        return FALSE;
    }
    if (info_type[variant][0] == 'a')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int hasNodeASymb(variant)
int variant;
{
    if ((!isABifNode(variant)) && (!isALoNode(variant)))
    {
#if !__SPF
        Message("hasNodeASymb not applied to a bif or low level node", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][2] == 's')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isNodeAConst(variant)
int variant;
{
    if ((!isABifNode(variant)) && (!isALoNode(variant)))
    {
#if !__SPF
        Message("isNodeAConst not applied to a bif or low level node", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][1] == 'c')
        return TRUE;
    else
        return FALSE;
}


/***************************************************************************/
int isAStructDeclBif(variant)
int variant;
{
    if (!isABifNode(variant))
    {
#if !__SPF
        Message("isAStructDeclBif not applied to a bif", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][1] == 's')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isAUnionDeclBif(variant)
int variant;
{
    if (!isABifNode(variant))
    {
#if !__SPF
        Message("isAUnionDeclBif not applied to a bif", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][1] == 'u')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isAEnumDeclBif(variant)
int variant;
{
    if (!isABifNode(variant))
    {
#if !__SPF
        Message("isAEnumDeclBif not applied to a bif", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][1] == 'e')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isADeclBif(variant)
int variant;
{
    if (!isABifNode(variant))
    {
#if !__SPF
        Message("isADeclBif not applied to a bif", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][0] == 'd')
        return TRUE;
    else
        return FALSE;
}

/***************************************************************************/
int isAControlEnd(variant)
int variant;
{
    if (!isABifNode(variant))
    {
#if !__SPF
        Message("isAControlEnd not applied to a bif", 0);
#endif
        return FALSE;
    }
    if (general_info[variant][0] == 'c')
        return TRUE;
    else
        return FALSE;
}

#ifdef __SPF
extern void printLowLevelWarnings(const char *fileName, const int line, const wchar_t* messageR, const char *message, const int group);
#endif
/***************************************************************************/
void Message(char *s, int l)
{
    if (l != 0)
        fprintf(stderr, "Warning : %s line %d\n", s, l);
    else
        fprintf(stderr, "Warning : %s\n", s);
    Warning_count++;
#ifdef __SPF
    if (l == 0)
        l = 1;

    printLowLevelWarnings(cur_file->filename, l, NULL, s, 4001);
#endif
}

/***************************************************************************/
/* A set of functions for dealing with a free list for low_level node      */
/***************************************************************************/

static int ExpressionNodeInFreeList = 0;
static ptstack_chaining expressionFreeNodeList = NULL;

void setFreeListForExpressionNode()
{
  if (ExpressionNodeInFreeList) return;
  
  ExpressionNodeInFreeList = 1;
  if (!expressionFreeNodeList)
    {
      expressionFreeNodeList = (ptstack_chaining) xmalloc(sizeof(struct stack_chaining));
      expressionFreeNodeList->first = NULL;
      expressionFreeNodeList->last = NULL;
      expressionFreeNodeList->prev = NULL;
      expressionFreeNodeList->level = 0;
    }
}


void resetFreeListForExpressionNode()
{
  ExpressionNodeInFreeList = 0;
}


/* Added for garbage collection */
void libFreeExpression(ll)
     PTR_LLND ll;
{
  ptchaining  pt2;

  if (!ExpressionNodeInFreeList) return;
  if (!ll)  return;
  if (!expressionFreeNodeList)
     {
      Message("Free list for expression node not defined\n",0);
      exit(1);
    }
  pt2 = (ptchaining) xmalloc(sizeof(struct chaining));
  pt2->zone = (char *) ll;
  pt2->list = NULL;

  if (expressionFreeNodeList->first == NULL)
    expressionFreeNodeList->first = pt2;

  if (expressionFreeNodeList->last == NULL)
    expressionFreeNodeList->last = pt2;
  else
    {
      expressionFreeNodeList->last->list = pt2;
      expressionFreeNodeList->last = pt2;
    }
}

char *allocateFreeListNodeExpression()
{
  char *pt;
  ptchaining  pt2;

  if (!ExpressionNodeInFreeList) return xmalloc(sizeof (struct llnd));
  if (!expressionFreeNodeList)
     {
      Message("Free list for expression node not defined\n",0);
      exit(1);
    }
  if (expressionFreeNodeList->first == NULL) return xmalloc(sizeof (struct llnd));
  
  pt2 = expressionFreeNodeList->first;
  if (expressionFreeNodeList->first == expressionFreeNodeList->last)
    {
      expressionFreeNodeList->first = NULL;
      expressionFreeNodeList->last = NULL;
    } else
      expressionFreeNodeList->first = pt2->list;
  
  pt = pt2->zone;
#ifdef __SPF
  removeFromCollection(pt2);
#endif
  free(pt2);
  memset((char *) pt, 0 , sizeof (struct llnd));
  return pt;
}


/***************************************************************************/
POINTER newNode(code)
     int code;
{
   PTR_BFND tb = NULL;
   PTR_LLND tl = NULL;
   PTR_TYPE tt = NULL;
   PTR_SYMB ts = NULL;
   PTR_LABEL tlab;
   PTR_CMNT tcmnt;
   PTR_BLOB tbl;
   int length;
   int kind;
  
   if (code == CMNT_KIND)
   { /* lets create a comment */

       length = sizeof(struct cmnt);
       tcmnt = (PTR_CMNT)xmalloc(length);
       memset((char *)tcmnt, 0, length);
       CMNT_ID(tcmnt) = ++CUR_FILE_NUM_CMNT();
       CMNT_NEXT(tcmnt) = PROJ_FIRST_CMNT();
       PROJ_FIRST_CMNT() = tcmnt;
       return (POINTER)tcmnt;
   }

  if (code == LABEL_KIND)
    { /* lets create a label */
      PTR_LABEL last;
      
      /* allocating space... PHB */
      length = sizeof (struct Label); 
      tlab = (PTR_LABEL) xmalloc(length);
      memset((char *) tlab, 0, length);
      LABEL_ID(tlab) = ++CUR_FILE_NUM_LABEL();

      if ((last=getLastLabel())) /* is there an existing label? PHB */
        {
        LABEL_NEXT(last)=tlab;
        return (POINTER) tlab;
        }
      else  /* There is no existing label, make one PHB */
        {
        LABEL_NEXT(tlab) = LBNULL;
        PROJ_FIRST_LABEL() = tlab;  /* set pointer to first label */
        return (POINTER) tlab;
        }
    }

  if (code == BLOB_KIND)
    { 
      length = sizeof (struct blob);
      tbl = (PTR_BLOB) xmalloc (length);
      memset((char *) tbl, 0, length);
      CUR_FILE_NUM_BLOBS()++; 
      return (POINTER) tbl;
    }
  

  kind = (int) node_code_kind[(int) code];
  switch (kind)
    {
    case BIFNODE:
      length = sizeof (struct bfnd);      
      break;
    case LLNODE :
     length = sizeof (struct llnd);  
      break;
    case SYMBNODE:
     length = sizeof (struct symb);  
      break;
    case TYPENODE:
     length = sizeof (struct data_type);   
      break;
    default:
      Message("Node inconnu",0);
    }

  switch (kind)
    {
    case BIFNODE:
      tb = (PTR_BFND) xmalloc(length);
      memset((char *) tb, 0, length);
      BIF_ID (tb)  = ++CUR_FILE_NUM_BIFS ();
      number_of_bif_node++;
      /*BIF_ID (tb) = number_of_bif_node++;*/
      BIF_CODE(tb) = code;
      BIF_FILE_NAME(tb) = CUR_FILE_HEAD_FILE();/* recently added, to check */
      CUR_FILE_CUR_BFND() = tb;
      BIF_LINE(tb) = 0; /* set to know that this is a new node */ 
      break;
    case LLNODE :
      if (ExpressionNodeInFreeList)
        tl = (PTR_LLND) allocateFreeListNodeExpression();
      else
        {
          tl = (PTR_LLND) xmalloc(length);
          memset((char *) tl, 0, length);
        }
      NODE_ID (tl) = ++CUR_FILE_NUM_LLNDS();
      NODE_NEXT (tl) = LLNULL;
      number_of_ll_node++;
      if (CUR_FILE_NUM_LLNDS() == 1)
	PROJ_FIRST_LLND () = tl;
      else
	NODE_NEXT (CUR_FILE_CUR_LLND()) = tl;
      CUR_FILE_CUR_LLND() = tl;
      NODE_CODE(tl) = code;
      break;
    case SYMBNODE:
      ts = (PTR_SYMB)  xmalloc(length); 
      memset((char *) ts, 0, length);
      number_of_symb_node++;
      SYMB_ID (ts) = ++CUR_FILE_NUM_SYMBS();
      SYMB_CODE(ts) = code;
      if (CUR_FILE_NUM_SYMBS() == 1)
	PROJ_FIRST_SYMB () =  ts;
      else
	SYMB_NEXT (CUR_FILE_CUR_SYMB()) = ts;
      CUR_FILE_CUR_SYMB() = ts;
      SYMB_NEXT (ts) = NULL;
      SYMB_SCOPE (ts) = PROJ_FIRST_BIF();/* the default value */
      break;
    case TYPENODE:
      /*tt = (PTR_TYPE) alloc_type ( cur_file ); xmalloc(length);
      number_of_type_node++;
      TYPE_ID (tt) = number_of_type_node++;
      TYPE_NEXT (tt) = NULL;*/
      
      tt = (PTR_TYPE) xmalloc (length);
      memset((char *) tt, 0, length);
      number_of_type_node++;
      TYPE_ID (tt) = ++CUR_FILE_NUM_TYPES();
      TYPE_CODE (tt) = code;
      TYPE_NEXT (tt) = NULL;
      if (CUR_FILE_NUM_TYPES () == 1)
        PROJ_FIRST_TYPE() = tt;
      else 
        TYPE_NEXT (CUR_FILE_CUR_TYPE()) = tt;
      CUR_FILE_CUR_TYPE() = tt;
      /* for VPC very ugly and should be removed later */
      if (code == T_POINTER)  TYPE_TEMPLATE_DUMMY1(tt) = 1 ;
      if (code == T_REFERENCE) TYPE_TEMPLATE_DUMMY1(tt) = 1 ;
      break;
    default:
      Message("Node inconnu",0);
    }


  switch (kind)
    {
    case BIFNODE:
      return (POINTER) tb; 
    case LLNODE :
      return (POINTER) tl;
    case SYMBNODE:
      return (POINTER) ts; 
    case TYPENODE:
      return (POINTER) tt;
    default:
      Message("Node inconnu",0);
    }
   return NULL;
}

/***************************************************************************/
PTR_LLND copyLlNode(node)
     PTR_LLND node;
{
   PTR_LLND t;
   int code;
  
  if (!node)
    return NULL;

  code = NODE_CODE (node);
  if (node_code_kind[(int) code] != LLNODE)
    Message("bif_copy_node != low_level_node",0);

  t = (PTR_LLND) newNode (code);
  
  NODE_SYMB(t)  = NODE_SYMB(node);
  NODE_TYPE(t) = NODE_TYPE(node);
  NODE_OPERAND0(t) = copyLlNode(NODE_OPERAND0(node));
  NODE_OPERAND1(t) = copyLlNode(NODE_OPERAND1(node));
  return t;
}

/***************************************************************************/
PTR_LLND makeInt(low)
     int low;
{
   PTR_LLND t =  (PTR_LLND) newNode(INT_VAL);
  NODE_TYPE(t) = NULL;
  NODE_INT_CST_LOW (t) = low;
  return t;
}

/* Originally coded by fbodin, but the code used K&R varargs conventions,
   I have rewritten the code to use ANSI conventions (phb) */
/***************************************************************************/
PTR_LLND newExpr(int code, PTR_TYPE ntype, ... )
{
  va_list p;
  PTR_LLND t;
  int length;

  /* Create a new node of type 'code' */
  t = (PTR_LLND) newNode(code);
  NODE_TYPE(t) = ntype;

  /* calculate the number of args required for this type of node */
  length = node_code_length[code];

  /* Set pointer p to the very first variable argument in list */
  va_start(p,ntype);  

  if (hasNodeASymb(code))
    {
      /* Extract third argument (type PTR_SYMB), inc arg pointer p */
      PTR_SYMB arg0 = va_arg(p, PTR_SYMB);
      NODE_SYMB(t) = arg0;
    } 
  if (length != 0)
    {
      if (length == 2)
	{
	  /* This is equivalent to the loop below, but faster.  */
	  /* Extract another argument (type PTR_LLND), inc arg pointer p */
	  PTR_LLND arg0 = va_arg(p, PTR_LLND);
	  /* Extract another argument (type PTR_LLND), inc arg pointer p */
	  PTR_LLND arg1 = va_arg(p, PTR_LLND);
	  NODE_OPERAND0(t) = arg0;
	  NODE_OPERAND1(t) = arg1;
	  va_end (p);
	  return t;
	}
      else 
	if (length == 1)
	  {
	    /* This is equivalent to the loop below, but faster.  */
	    /* Extract another argument (type PTR_LLND), inc arg pointer p */
	    PTR_LLND arg0 = va_arg(p, PTR_LLND);
	    NODE_OPERAND0(t) = arg0;
	    va_end(p);
	    return t;
	  } else
	    Message("A low level node have more than two operands",0);
    }
  va_end(p);
  return t;
}

/***************************************************************************/
PTR_SYMB newSymbol(code, name, type)
     int code;
     char *name;
     PTR_TYPE type;
{
   PTR_SYMB t;
  char *str;
  
  if(name){
    str = (char *) xmalloc(strlen(name) +1);
    strcpy(str,name);
    }
  else str=NULL;
  t = (PTR_SYMB) newNode (code);
  SYMB_IDENT (t) = str;
  SYMB_TYPE (t) = type;
  return t;
}

/***************************************************************************/
int Check_Lang_C(proj)
PTR_PROJ proj;
{
  PTR_FILE ptf;
  PTR_BLOB ptb;
  if (!proj)
    return TRUE;
  for (ptb = PROJ_FILE_CHAIN (proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);

/*      if (debug)
	fprintf(stderr,"%s\n",FILE_FILENAME (ptf)); */

      if (FILE_LANGUAGE (ptf) != CSrc)
	return(FALSE);
    }
  return(TRUE);
}


/***************************************************************************/
int Check_Lang_Fortran(proj)
PTR_PROJ proj;
{
  PTR_FILE ptf;
  PTR_BLOB ptb;
  if (!proj)
    return FALSE;
  for (ptb = PROJ_FILE_CHAIN (proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);
    /*  if (debug)
	fprintf(stderr,"%s\n",FILE_FILENAME (ptf)); */

      if (FILE_LANGUAGE(ptf) != ForSrc)
	return(FALSE);
    }
  return(TRUE);
}


/* Procedure for unparse a program use when debug is required 
   the current project is taking */
/***************************************************************************/
void UnparseProgram(fout)
     FILE *fout;
{
/*  char *s;
  PTR_BLOB b, bl;
  PTR_FILE f;
  */ /*podd 15.03.99*/
  if (Check_Lang_Fortran(cur_proj))
    {  
      Init_Unparser();
            
      fprintf(fout,"%s",filter(Tool_Unparse_Bif(PROJ_FIRST_BIF())));
    } else
      {
	Init_Unparser();
	fprintf(fout,"%s",Tool_Unparse_Bif(PROJ_FIRST_BIF()));
      }
}

/***************************************************************************/
void UnparseProgram_ThroughAllocBuffer(fout,filept,size)
     FILE *fout;
     PTR_FILE filept;
     int size;
{
/*  char *s;
  PTR_BLOB b, bl;
  PTR_FILE f;
  */ /*podd 29.01.07*/
  
           //SetCurrentFileTo(filept);
           //SwitchToFile(GetFileNumWithPt(filept));

  if (Check_Lang_Fortran(cur_proj))
    { 
      Init_Unparser();
     
      BufferAllocate(size);
     
      fprintf(fout,"%s",filter(Tool_Unparse_Bif(PROJ_FIRST_BIF())));
    } else
      {
	Init_Unparser();
	fprintf(fout,"%s",Tool_Unparse_Bif(PROJ_FIRST_BIF()));
      }
}

/* Procedure for unparse a program use when debug is required 
   the current project is taking */
/***************************************************************************/
void UnparseBif(bif)
     PTR_BFND bif;
{
/*  char *s;
  PTR_BLOB b, bl;
*/ /* podd 15.03.99*/
  if (Check_Lang_Fortran(cur_proj))
    {  
      Init_Unparser();
      printf("%s",filter(Tool_Unparse_Bif(bif)));
    } else
      {
	Init_Unparser();
	printf("%s",(Tool_Unparse_Bif(bif)));
      }

}

/***************************************************************************/

/* podd 28.01.07 */   /*change podd 16.12.11*/
char *UnparseBif_Char(bif,lang)
     PTR_BFND bif;
     int lang; /* 0 - undefined, 1 - C language, 2 - Fortran language */
{
    char *s;
/*  PTR_BLOB b, bl;
*/ /* podd 15.03.99*/
  if (Check_Lang_Fortran(cur_proj) && lang != CSrc)  /*podd 16.12.11*/
    {  
      Init_Unparser();
      s = filter(Tool_Unparse_Bif(bif));
    } else
      { if(lang == CSrc)
          Set_Function_Language(CSrc);
	Init_Unparser();
	s = Tool_Unparse_Bif(bif);
        if(lang == CSrc)
          Unset_Function_Language();
      }
   return(s);
}

/* Kataev N.A. 03.09.2013 base on UnparseBif_Char with change podd 16.12.11
   Kataev N.A. 19.10.2013 fix
*/
char *UnparseLLND_Char(llnd)
     PTR_LLND llnd;    
{
    char *s;
    Init_Unparser();
    s = Tool_Unparse2_LLnode(llnd);    
   return(s);
}

/* Procedure for unparse a program use when debug is required 
   the current project is taking */
/***************************************************************************/
void UnparseLLND(ll)
     PTR_LLND ll;
{
  Init_Unparser();
  printf("%s",Tool_Unparse2_LLnode(ll));
}

/***************************************************************************/
char* UnparseTypeBuffer(type)
     PTR_TYPE type;
{
  Init_Unparser();
  return Tool_Unparse_Type(type);
}

/***************************************************************************/
int open_proj_toolbox(char* proj_name, char* proj_file)
{
    char* mem[MAX_FILE];          /* for file in the project */
    int   no = 0;           /* number of file in the project */
    int   c;
    FILE* fd;               /* file descriptor for project */
    char** p, * t;
    char* tmp, tmpa[3000];

    tmp = &(tmpa[0]);

    if ((fd = fopen(proj_file, "r")) == NULL)
        return -1;

    p = mem;
    t = tmp;
    while ((c = getc(fd)) != EOF) 
    {
        
        //if (c != ' ') /* assum no blanks in filename */

        {
            if (c == '\n') 
            {
                if (t != tmp) 
                {   /* not a blank line */
                    *t = '\0';
                    *p = (char*)malloc((unsigned)(strlen(tmp) + 1));
#ifdef __SPF
                    addToCollection(__LINE__, __FILE__, *p, 0);
#endif
                    strcpy(*p++, tmp);
                    t = tmp;
                }
            }
            else 
                *t++ = c;
        }
    }

    fclose(fd);
    no = p - mem;
    if (no > 0)
    {
        /* Now make it the active project */
        if ((cur_proj = OpenProj(proj_name, no, mem)))
        {
            cur_file = (PTR_FILE)BLOB_VALUE(CUR_PROJ_FILE_CHAIN());
            pointer_on_file_proj = cur_file;
            return 0;
        }
        else
        {
            fprintf(stderr, "-2 Cannot open project\n");
            return -2;
        }
    }
    else
    {
        fprintf(stderr, "-3 No files in the project\n");
        return -3;
    }
}

int open_proj_files_toolbox(char* proj_name, char** file_list, int no)
{    
    if (no > 0)
    {
        /* Now make it the active project */
        if ((cur_proj = OpenProj(proj_name, no, file_list)))
        {
            cur_file = (PTR_FILE)BLOB_VALUE(CUR_PROJ_FILE_CHAIN());
            pointer_on_file_proj = cur_file;
            return 0;
        }
        else
        {
            fprintf(stderr, "-2 Cannot open project\n");
            return -2;
        }
    }
    else
    {
        fprintf(stderr, "-3 No files in the project\n");
        return -3;
    }
}

static int ToolBOX_INIT = 0;
/***************************************************************************/
void Reset_Tool_Box()
{
  Init_Tool_Box();
}

/***************************************************************************/
void Reset_Bif_Next()
{
  PTR_BLOB ptb;
  if (cur_proj)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  pointer_on_file_proj = (PTR_FILE) BLOB_VALUE (ptb);
	  Redo_Bif_Next_Chain(PROJ_FIRST_BIF());
	}
    } else
      if(pointer_on_file_proj)
	Redo_Bif_Next_Chain(PROJ_FIRST_BIF());
}

/***************************************************************************/
int Init_Tool_Box()
{

    PTR_BLOB ptb;

    pointer_on_file_proj = cur_file;
    number_of_type_node = CUR_FILE_NUM_TYPES() + 1;
    number_of_ll_node = CUR_FILE_NUM_LLNDS() + 1;
    number_of_bif_node = CUR_FILE_NUM_BIFS() + 1;
    number_of_symb_node = CUR_FILE_NUM_SYMBS() + 1;
    last_file_symbol = CUR_FILE_CUR_SYMB(); /* podd 23.06.15 */

    if (CUR_FILE_NAME()) strcpy(Current_File_name, CUR_FILE_NAME());
    if (ToolBOX_INIT)
        return 0;

    ToolBOX_INIT = 1;

    make_a_malloc_stack();

    /* initialisation des noeuds */
#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT,f1,f2,f3,f4,f5) node_code_type[SYM] = TYPE;
#include"bif_node.def"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT,f1,f2,f3,f4,f5) node_code_length[SYM] =LENGTH;
#include"bif_node.def"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT,f1,f2,f3,f4,f5) node_code_kind[SYM] = NT;
#include"bif_node.def"
#undef DEFNODECODE

/* set special table for symbol and type */
#define DEFNODECODE(SYMB,f1,f2,f3,f4,f5) info_type[SYMB][0] = f1; info_type[SYMB][1] = f2; info_type[SYMB][2] = f3;  info_type[SYMB][3] = f4;   info_type[SYMB][4] = f5;  
#include"type.def"
#undef DEFNODECODE

#define DEFNODECODE(SYMB,f1,f2,f3,f4,f5) info_symb[SYMB][0] = f1; info_symb[SYMB][1] = f2; info_symb[SYMB][2] = f3;  info_symb[SYMB][3] = f4;   info_symb[SYMB][4] = f5;  
#include"symb.def"
#undef DEFNODECODE

#define DEFNODECODE(SYM, NAME, TYPE, LENGTH, NT,f1,f2,f3,f4,f5) general_info[SYM][0] = f1; general_info[SYM][1] = f2; general_info[SYM][2] = f3;  general_info[SYM][3] = f4;   general_info[SYM][4] = f5; 
#include"bif_node.def"
#undef DEFNODECODE

    if (cur_proj)
    {
        for (ptb = PROJ_FILE_CHAIN(cur_proj); ptb; ptb = BLOB_NEXT(ptb))
        {
            pointer_on_file_proj = (PTR_FILE)BLOB_VALUE(ptb);
            Redo_Bif_Next_Chain_Internal(PROJ_FIRST_BIF());
        }
    }
    pointer_on_file_proj = cur_file;
    number_of_type_node = CUR_FILE_NUM_TYPES() + 1;
    number_of_ll_node = CUR_FILE_NUM_LLNDS() + 1;
    number_of_bif_node = CUR_FILE_NUM_BIFS() + 1;
    number_of_symb_node = CUR_FILE_NUM_SYMBS() + 1;

    return 1;
  
}

/* For debug  */
/***************************************************************************/
void writeDepFileInDebugdep()
{
  PTR_BFND thebif;
  int i;

  thebif = PROJ_FIRST_BIF();
  i = 1;
  for (;thebif;thebif=BIF_NEXT(thebif), i++) 
       BIF_ID(thebif) = i;     
   
  CUR_FILE_NUM_BIFS() = i-1;

  if (write_nodes(cur_file,"debug.dep") < 0)
      Message("Error, write_nodes() failed (000)",0);

}

int isBlankString(char *str)
{int i; 

 for(i=0;i<72;i++)
   if(str[i] !=' ')
     return(0);
 return(1);

}

/* this function converts a letter to uppercase except char strings (text inside quotes) */
char to_upper_case (char c, int *quote)
{ 
    if(c == '\'' || c == '\"')
    {
       if(*quote == c)
          *quote = 0;
       else if(*quote==0)
          *quote = c;
       return c;
    }
    if(c >= 0 && islower(c) && *quote==0)
       return toupper(c);
    return c;        
}

char* filter(char *s)
{
    char c;
    int i = 1, quote = 0;

    // 14.10.2016 Kolganov. Switch constant buffer to dynamic
    int temp_size = 4096;
    char *temp = (char*)malloc(sizeof(char) * temp_size);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,temp, 0);
#endif

    int temp_i = 0;
    int buf_i = 0;
    int commentline = 0;
    char *resul, *init;
    int OMP, DVM, SPF; /*OMP*/
    OMP = DVM = SPF = 0;

    if (!s) 
        return NULL;
    if (strlen(s) == 0) 
        return s;
    make_a_malloc_stack();
    //XXX: result is not free at the end of procedure!!
    resul = (char *)mymalloc(2 * strlen(s));
    memset(resul, 0, 2 * strlen(s));
    init = resul;
    c = s[0];

    if ((c != ' ')
        && (c != '\n')
        && (c != '0')
        && (c != '1')
        && (c != '2')
        && (c != '3')
        && (c != '4')
        && (c != '5')
        && (c != '6')
        && (c != '7')
        && (c != '8')
        && (c != '9'))
        commentline = 1;
    else
        commentline = 0;
    if (commentline) 
    { 
        if ( (s[1] == '$') && (s[2] == 'O') && (s[3] == 'M') && (s[4] == 'P')) 
        {
            OMP = 1;
            DVM = SPF = 0; 
        }        
        else if ( (s[1] == '$') && (s[2] == 'S') && (s[3] == 'P') && (s[4] == 'F')) 
        {
            SPF = 1;
            OMP = DVM = 0;
        } 
        else if (s[1] == '$') 
        {
            OMP = 2;
            DVM = SPF = 0;
        }
        else if ( (s[1] == 'D') && (s[2] == 'V') && (s[3] == 'M') && (s[4] == '$'))  
        {
            DVM = 1;
            OMP = SPF = 0;
        }
        else 
            OMP = DVM = SPF = 0;        
    }
    temp_i = 0;
    i = 0;
    buf_i = 0;    
    while (c != '\0')
    {
        c = s[i];
        temp[buf_i] = out_upper_case && (!commentline || DVM || SPF || OMP) ? to_upper_case(c,&quote) : c;
        if (c == '\n')
        {
            if (buf_i + 1 > temp_size)
            {
                temp_size *= 2;
#ifdef __SPF
                removeFromCollection(temp);
#endif
                temp = (char*)realloc(temp, sizeof(char) * temp_size);
#ifdef __SPF
                addToCollection(__LINE__, __FILE__,temp, 0);
#endif
            }

            temp[buf_i + 1] = '\0';
            sprintf(resul, "%s", temp);
            resul = resul + strlen(temp);
            temp_i = -1;
            buf_i = -1;
            if ((s[i + 1] != ' ')
                && (s[i + 1] != '\n')
                && (s[i + 1] != '0')
                && (s[i + 1] != '1')
                && (s[i + 1] != '2')
                && (s[i + 1] != '3')
                && (s[i + 1] != '4')
                && (s[i + 1] != '5')
                && (s[i + 1] != '6')
                && (s[i + 1] != '7')
                && (s[i + 1] != '8')
                && (s[i + 1] != '9'))
                commentline = 1;
            else
                commentline = 0;
            if (commentline) 
            {
                if ( (s[i+2] == '$') && (s[i+3] == 'O') && (s[i+4] == 'M') && (s[i+5] == 'P'))  
                {
                    OMP = 1;
                    DVM = SPF = 0;
                }
                else if ( (s[i+2] == '$') && (s[i+3] == 'S') && (s[i+4] == 'P') && (s[i+5] == 'F')) 
                {
                    SPF = 1;
                    OMP = DVM = 0;                    
                } 
                else if (s[i + 2] == '$') 
                {
                    OMP = 2;
                    DVM = SPF = 0;
                }
                else 
                {
                    if ( (s[i+2] == 'D') && (s[i+3] == 'V') && (s[i+4] == 'M') && (s[i+5] == '$'))   
                    {
                        DVM = 1;
                        OMP = SPF = 0;
                    }
                    else OMP = DVM = SPF = 0;
                }
            }
        }
        else 
        {
            if (((!out_free_form && temp_i == 71) || (out_free_form && !out_line_unlimit && temp_i == 131)) && !commentline && (s[i + 1] != '\n'))
            {
                if (buf_i + 1 > temp_size)
                {
                    temp_size *= 2;
#ifdef __SPF
                    removeFromCollection(temp);
#endif
                    temp = (char*)realloc(temp, sizeof(char) * temp_size);
#ifdef __SPF
                    addToCollection(__LINE__, __FILE__,temp, 0);
#endif
                }
                /* insert where necessary */
                temp[buf_i + 1] = '\0';
                if (out_free_form)
                {
                    sprintf(resul, "%s&\n", temp);
                    resul = resul + strlen(temp) + 2;
                }
                else 
                {
                    sprintf(resul, "%s\n", temp);
                    resul = resul + strlen(temp) + 1;
                }
                if (!out_free_form && isBlankString(temp))   /*24.06.13*/
                    /* string of 72 blanks in fixed form */
                    sprintf(resul, "      ");
                else
                    sprintf(resul, "     &");
                resul = resul + strlen("     &");
                commentline = 0;
                memset(temp, 0, sizeof(char) * temp_size);
                temp_i = strlen("     &") - 1;
                buf_i = -1;
            }

            if (((!out_free_form && temp_i == 71) || (out_free_form && !out_line_unlimit && temp_i == 131)) && commentline && (s[i + 1] != '\n') && ((OMP == 1) || (OMP == 2) || (DVM == 1) || (SPF == 1))) /*07.08.17*/
            {
                if (buf_i + 1 > temp_size)
                {
                    temp_size *= 2;
#ifdef __SPF
                    removeFromCollection(temp);
#endif
                    temp = (char*)realloc(temp, sizeof(char) * temp_size);
#ifdef __SPF
                    addToCollection(__LINE__, __FILE__,temp, 0);
#endif
                }

                temp[buf_i + 1] = '\0';
                if (out_free_form)
                {
                    sprintf(resul, "%s&\n", temp);
                    resul = resul + strlen(temp) + 2;
                }
                else 
                {
                    sprintf(resul, "%s\n", temp);
                    resul = resul + strlen(temp) + 1;
                }
                if (OMP == 1) 
                {
                    sprintf(resul, "!$OMP&");
                    resul = resul + strlen("!$OMP&");
                    temp_i = strlen("!$OMP&") - 1;
                }
                if (OMP == 2)
                {
                    sprintf(resul, "!$   &");
                    resul = resul + strlen("!$   &");
                    temp_i = strlen("!$   &") - 1;
                }
                if (DVM == 1)
                {
                    sprintf(resul, "!DVM$&");
                    resul = resul + strlen("!DVM$&");
                    temp_i = strlen("!DVM$&") - 1;
                }

                if (SPF == 1)
                {
                    sprintf(resul, "!$SPF&");
                    resul = resul + strlen("!$SPF&");
                    temp_i = strlen("!$SPF&") - 1;
                }
                memset(temp, 0, sizeof(char) * temp_size);
                temp_i = strlen("     +") - 1;
                buf_i = -1;
            }
        }
        i++;
        temp_i++;
        buf_i++;
        if (buf_i > temp_size)
        {
            temp_size *= 2;
#ifdef __SPF
            removeFromCollection(temp);
#endif
            temp = (char*)realloc(temp, sizeof(char) * temp_size);
#ifdef __SPF
            addToCollection(__LINE__, __FILE__,temp, 0);
#endif
        }
    }
#ifdef __SPF
    removeFromCollection(temp);
#endif
    free(temp);
    return init;
}



/* BW, june 1994
   this function is used in duplicateStmtsBlock to determine how many
   bif nodes need to be copied
*/
/***************************************************************************/
int numberOfBifsInBlobList(blob)
PTR_BLOB blob;
{
  PTR_BFND cur_bif;

  if(!blob) return 0;
  cur_bif = BLOB_VALUE(blob);
  return (numberOfBifsInBlobList(BIF_BLOB1(cur_bif))
       + numberOfBifsInBlobList(BIF_BLOB2(cur_bif))
       + numberOfBifsInBlobList(BLOB_NEXT(blob)) + 1);
}

/***************************************************************************/
int findBifInList1(bif_source, bif_cherche)
PTR_BFND bif_source, bif_cherche;
{
  PTR_BLOB temp;
  
  if ((bif_cherche == NULL) || (bif_source == NULL)) 
    return FALSE;
  
  for (temp = BIF_BLOB1 (bif_source); temp ; temp = BLOB_NEXT (temp))
    if (BLOB_VALUE (temp) == bif_cherche)
      return TRUE;
  return FALSE;
}

/***************************************************************************/
int findBifInList2(bif_source, bif_cherche)
PTR_BFND bif_source, bif_cherche;
{
  PTR_BLOB temp;
  
  if ((bif_cherche == NULL) || (bif_source == NULL)) 
    return FALSE;

  for (temp = BIF_BLOB2 (bif_source); temp ; temp = BLOB_NEXT (temp))
    if (BLOB_VALUE (temp) == bif_cherche)
      return TRUE;
  return FALSE;
}

/***************************************************************************/
int findBif(bif_source, bif_target, i)
PTR_BFND bif_source, bif_target;
int i;
{
  switch(i){
  case 0:
    if (findBifInList1 (bif_source, bif_target))
      return TRUE;
    else return findBifInList2 (bif_source, bif_target);
   
  case 1:
    return findBifInList1 (bif_source, bif_target);
    
  case 2:
    return findBifInList2 (bif_source, bif_target);
    
  }
  return 0;
}


/***************************************************************************/
PTR_BLOB appendBlob(b1, b2)
PTR_BLOB b1, b2;
{
    if (b1) {
         PTR_BLOB p, q;

        for (p = b1; p; p = BLOB_NEXT (p)) /* skip to the end of b1 */
            q = p;
        BLOB_NEXT (q) = b2;
    } else
        b1 = b2;
    return b1;
}

/* 
 *delete a bif node from the list of blob node
 */
/***************************************************************************/
PTR_BFND deleteBfndFromBlobAndLabel(bf,label)
     PTR_BFND bf;
     PTR_LABEL label;
{
    PTR_BLOB first;
    PTR_BLOB bl1, bl2;

    if (label) {
      first = LABEL_UD_CHAIN(label);     
      if (first && (BLOB_VALUE (first) == bf))
	{
	  bl2 = first;
	  LABEL_UD_CHAIN(label) = BLOB_NEXT (first);
	  return (BLOB_VALUE (bl2));
	}

      for (bl1 = bl2 = first; bl1; bl1 = BLOB_NEXT (bl1)) {
	if (BLOB_VALUE (bl1) == bf) {
	  BLOB_NEXT (bl2) = BLOB_NEXT (bl1);
	  return (BLOB_VALUE (bl2));
	}
	bl2 = bl1;
      }
      return NULL;
    }
    return NULL;
}

/***************************************************************************/
PTR_BLOB lookForBifInBlobList(first, bif)
PTR_BLOB first;
PTR_BFND bif;
{
  PTR_BLOB tail;
  if (first == NULL)
    return NULL;
  for (tail = first; tail; tail = BLOB_NEXT(tail) )
    {
      if (BLOB_VALUE(tail) == bif)
        return tail;
    }
  return NULL;
}

/***************************************************************************/
PTR_BFND childfInBlobList(first, num)
PTR_BLOB first;
int num;
{
  PTR_BLOB tail;
  int len = 0;
  if (first == NULL)
    return NULL;
  for (tail = first; tail; tail = BLOB_NEXT(tail) )
    {
      if (len == num)
        return BLOB_VALUE(tail);
      len++;
    }
  return NULL;
}

/***************************************************************************/
int blobListLength(first)
PTR_BLOB first;
{
  PTR_BLOB tail;
  int len = 0;
  if (first == NULL)
    return(0);
  for (tail = first; tail; tail = BLOB_NEXT(tail) )
    len++;
  return(len);
}

/***************************************************************************/
PTR_BFND lastBifInBlobList1(noeud)
     PTR_BFND noeud;
{
  PTR_BLOB bl1 = NULL;
  if (!noeud )
    return NULL;
  /* on va cherche le dernier dans la liste */
  for (bl1 = BIF_BLOB1(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
    {
      if (BLOB_NEXT(bl1) == NULL)
	break;
    }
  if (bl1)
    return BLOB_VALUE(bl1);
  else
    return NULL;
}

/***************************************************************************/
PTR_BFND lastBifInBlobList2(noeud)
     PTR_BFND noeud;
{
  PTR_BLOB bl1 = NULL;
  if (!noeud )
    return NULL;
  /* on va cherche le dernier dans la liste */
  for (bl1 = BIF_BLOB2(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
    {
      if (BLOB_NEXT(bl1) == NULL)
	break;
    }
  if (bl1)
    return BLOB_VALUE(bl1);
  else
    return NULL;
}

/***************************************************************************/
PTR_BFND lastBifInBlobList(noeud)
     PTR_BFND noeud;
{
  if (!BIF_INDEX(noeud))
    return lastBifInBlobList1( noeud);
  else
    return lastBifInBlobList2( noeud);
}

/***************************************************************************/
PTR_BLOB lastBlobInBlobList1(noeud)
     PTR_BFND noeud;
{
  PTR_BLOB bl1 = NULL;
  if (!noeud )
    return NULL;
  /* on va cherche le dernier dans la liste */
  for (bl1 = BIF_BLOB1(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
    {
      if (BLOB_NEXT(bl1) == NULL)
	break;
    }
  if (bl1)
    return bl1;
  else
    return NULL;
}

/***************************************************************************/
PTR_BLOB lastBlobInBlobList2(noeud)
     PTR_BFND noeud;
{
  PTR_BLOB bl1 = NULL;
  if (!noeud )
    return NULL;
  /* on va cherche le dernier dans la liste */
  for (bl1 = BIF_BLOB2(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
    {
      if (BLOB_NEXT(bl1) == NULL)
	break;
    }
  if (bl1)
    return bl1;
  else
    return NULL;
}

/***************************************************************************/
PTR_BLOB lastBlobInBlobList(noeud)
     PTR_BFND noeud;
{
  if (!BIF_INDEX(noeud))
    return lastBlobInBlobList1( noeud);
  else
    return lastBlobInBlobList2( noeud);
}

/*
 *
 * append dans la blob liste d'un noeud bif, un noeud bif
 *
 */
/***************************************************************************/
int appendBfndToList1(biftoinsert, noeud)
     PTR_BFND biftoinsert, noeud;
{
  PTR_BLOB bl1;

  if (!noeud || !biftoinsert)
    return 0;
  if (BIF_BLOB1(noeud) == NULL)
    {
      BIF_BLOB1(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
      BLOB_VALUE(BIF_BLOB1(noeud)) = biftoinsert; 
      BLOB_NEXT(BIF_BLOB1(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
        /* on va cherche le dernier dans la liste */
        for (bl1 = BIF_BLOB1(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
          {
            if (BLOB_NEXT(bl1) == NULL)
              break;
          }
        BLOB_NEXT(bl1) = (PTR_BLOB) newNode (BLOB_KIND);
        BLOB_VALUE(BLOB_NEXT(bl1)) = biftoinsert;
	BIF_CP(biftoinsert) = noeud;
        BLOB_NEXT(BLOB_NEXT(bl1)) = NULL;
      }
  
  return 1;
}

/***************************************************************************/
int appendBfndToList2(biftoinsert, noeud)
     PTR_BFND biftoinsert, noeud;
{
  PTR_BLOB bl1;

  if (!noeud || !biftoinsert)
    return 0;
  if (BIF_BLOB2(noeud) == NULL)
    {
      BIF_BLOB2(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
      BLOB_VALUE (BIF_BLOB2(noeud)) = biftoinsert;      
      BLOB_NEXT (BIF_BLOB2(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
        /* on va cherche le dernier dans la liste */
        for (bl1 = BIF_BLOB2(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
          {
            if (BLOB_NEXT(bl1) == NULL)
              break;
          }
        BLOB_NEXT(bl1) = (PTR_BLOB) newNode (BLOB_KIND);
        BLOB_VALUE(BLOB_NEXT(bl1)) = biftoinsert;
        BLOB_NEXT(BLOB_NEXT(bl1)) = NULL;
	BIF_CP(biftoinsert) = noeud;
      }
  
  return 1;
}

/* replace chain_up() */
/***************************************************************************/
int appendBfndToList(noeud, biftoinsert)
     PTR_BFND biftoinsert, noeud;
{
  /* use the index field to set the right blob node list */
  if (!noeud || !biftoinsert)
    return 0;
  if (!BIF_INDEX(noeud))
    return appendBfndToList1(biftoinsert, noeud);
  else
    return appendBfndToList2(biftoinsert, noeud);
}


/***************************************************************************/
int firstBfndInList1(biftoinsert, noeud)
     PTR_BFND biftoinsert, noeud;
{
  PTR_BLOB bl2;

  if (!noeud || !biftoinsert)
    return 0;
  if (BIF_BLOB1(noeud) == NULL)
    {
      BIF_BLOB1(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
      BLOB_VALUE (BIF_BLOB1(noeud)) = biftoinsert;
      BLOB_NEXT (BIF_BLOB1(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
        bl2 = BIF_BLOB1(noeud);
        BIF_BLOB1(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
        BLOB_VALUE (BIF_BLOB1(noeud)) = biftoinsert;
        BLOB_NEXT (BIF_BLOB1(noeud)) =  bl2 ;
	BIF_CP(biftoinsert) = noeud;
      }
  return 1;
}


/***************************************************************************/
int firstBfndInList2(biftoinsert, noeud)
     PTR_BFND biftoinsert, noeud;
{
  PTR_BLOB bl2;
  if (!noeud || !biftoinsert)
    return 0;
  if (BIF_BLOB2(noeud) == NULL)
    {
      BIF_BLOB2(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
      BLOB_VALUE (BIF_BLOB2(noeud)) = biftoinsert;
      BLOB_NEXT (BIF_BLOB2(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
        bl2 = BIF_BLOB2(noeud);
        BIF_BLOB2(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
        BLOB_VALUE (BIF_BLOB2(noeud)) = biftoinsert; 
        BLOB_NEXT (BIF_BLOB2(noeud)) =  bl2 ;
	BIF_CP(biftoinsert) = noeud;
      }
  return 1;
}

/***************************************************************************/
int insertBfndInList1(biftoinsert, current, noeud)
     PTR_BFND biftoinsert, noeud,current;
{
  PTR_BLOB bl1 = NULL, bl2;
  if (!noeud || !biftoinsert || !current)
    return 0;
  if (BIF_BLOB1(noeud) == NULL)
    {
      BIF_BLOB1(noeud) = (PTR_BLOB) newNode (BLOB_KIND);
      BLOB_VALUE (BIF_BLOB1(noeud)) = biftoinsert;
      BLOB_NEXT (BIF_BLOB1(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
       /* on va cherche current  dans la liste */
        for (bl1 = BIF_BLOB1(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
          {
            if (BLOB_VALUE(bl1) == current)
              break;
          }

        if (!bl1)
          {
            Message("insertBfndInList1 failed",0);
	    return FALSE;
          }

        bl2 = BLOB_NEXT(bl1);
        BLOB_NEXT(bl1) = (PTR_BLOB) newNode (BLOB_KIND);
        BLOB_VALUE (BLOB_NEXT(bl1)) = biftoinsert;
        BLOB_NEXT (BLOB_NEXT(bl1)) =  bl2; 
	BIF_CP(biftoinsert) = noeud;
      }
  return TRUE;
}

/***************************************************************************/
int insertBfndInList2(biftoinsert, current, noeud)
     PTR_BFND biftoinsert, noeud,current;
{
  PTR_BLOB bl1 = NULL, bl2;

  if (!noeud || !biftoinsert || !current)
    return 0;
  if (BIF_BLOB2(noeud) == NULL)
    {
      BIF_BLOB2(noeud) = (PTR_BLOB)  newNode (BLOB_KIND);
      BLOB_VALUE (BIF_BLOB2(noeud)) = biftoinsert;
      BLOB_NEXT (BIF_BLOB2(noeud)) = NULL;
      BIF_CP(biftoinsert) = noeud;
    } else
      {
       /* on va cherche current  dans la liste */
        for (bl1 = BIF_BLOB2(noeud); bl1; bl1 = BLOB_NEXT(bl1)) 
          {
            if (BLOB_VALUE(bl1) == current)
              break;
          }

        if (!bl1)
          {
            Message("insertBfndInList2 failed",0);
            abort();
          }

        bl2 = BLOB_NEXT(bl1);
        BLOB_NEXT(bl1) = (PTR_BLOB)  newNode (BLOB_KIND);
        BLOB_VALUE (BLOB_NEXT(bl1)) = biftoinsert;
        BLOB_NEXT(BLOB_NEXT(bl1)) =   bl2 ; 
	BIF_CP(biftoinsert) = noeud;

      }
  return 1;
}

/* enleve in noeud de la liste de bif node si s'y trouve */
/***************************************************************************/
PTR_BLOB deleteBfndFrom(b1,b2)
     PTR_BFND b1,b2;
{
  PTR_BLOB temp, last, res = NULL;
  
  if (!b1)
    return NULL;
  
  last = NULL;
  for (temp = BIF_BLOB1(b1) ; temp ; temp = BLOB_NEXT (temp))
    {
      if (BLOB_VALUE(temp) == b2)
        {
          res = temp;
          if (last  == NULL)
            {
              BIF_BLOB1(b1) = BLOB_NEXT (temp);              
              break;
            }          
          else
            {
              BLOB_NEXT (last) = BLOB_NEXT (temp);
              break;
            }
        }
      last = temp;      
    }
  
  if (!res)
    {
      last = NULL;
      for (temp = BIF_BLOB2(b1) ; temp ; temp = BLOB_NEXT (temp))
        {
          if (BLOB_VALUE(temp) == b2)
            {
               res = temp;
              if (last  == NULL)
                {
                  BIF_BLOB2(b1) = BLOB_NEXT (temp);
                  break;
                }          
              else
                {
                  BLOB_NEXT (last) = BLOB_NEXT (temp);
                  break;
                }
            }
          last = temp; 
        }
    }
  return res;
}


/***************************************************************************/
PTR_BFND getNodeBefore(b)
     PTR_BFND b;
{
  PTR_BFND temp, first;
  
  if (!b)
    return NULL;

  if (BIF_CP(b))
    first = BIF_CP(b);
  else
    first = PROJ_FIRST_BIF();

  for (temp = first; temp ; temp = BIF_NEXT(temp))
    {
      if (BIF_NEXT(temp) == b)
        return temp;
    }
 
  if (BIF_CP(b))
    {
      for (temp = BIF_CP(BIF_CP(b)); temp ; temp = BIF_NEXT(temp))
        {
          if (BIF_NEXT(temp) == b)
            return temp;
        }
    }
  if (debug)
    Message("Node Before not found ",0);
  return NULL;
}

/***************************************************************************/
void updateControlParent(first,last,cp)
PTR_BFND first,cp,last;

{
  PTR_BFND temp;
  
  for (temp = first; temp && (temp != last); temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        BIF_CP(temp) = cp;
    }

  if (!isItInSection(first,last,BIF_CP(last)))
    BIF_CP(last) = cp;
}


/***************************************************************************/
PTR_BFND getWhereToInsertInBfnd(where,cpin)
PTR_BFND where, cpin;
{
  PTR_BFND temp;
  PTR_BLOB blob;
  
  if (!cpin || !where)
    return NULL;

  if (findBifInList1 (cpin, where))
    return where;
  if (findBifInList2 (cpin, where))
    return where;

  
  for (blob = BIF_BLOB1(cpin) ; blob; blob = BLOB_NEXT(blob))
    {
      temp = getWhereToInsertInBfnd(where,BLOB_VALUE(blob));
      if (temp)
        return BLOB_VALUE(blob);
    }

  for (blob = BIF_BLOB2(cpin) ; blob; blob = BLOB_NEXT(blob))
    {
      temp = getWhereToInsertInBfnd(where,BLOB_VALUE(blob));
      if (temp)
        return BLOB_VALUE(blob);
    }

  return NULL;
  
}


/* Given a node where we want to insert another node, 
   compute the control parent */
/***************************************************************************/
PTR_BFND computeControlParent(where)
PTR_BFND where;
{
  PTR_BFND cp;

      
  if (!where)
    {
      Message("where not defined in computeControlParent: abort()",0);
      abort();
    }

  if (!BIF_CP(where))
    {
      switch(BIF_CODE(where))
        { /* node that can be a bif control parent */
        case  GLOBAL	       :
        case  PROG_HEDR      :
        case  PROC_HEDR	:	
        case  PROS_HEDR	:	
        case  BASIC_BLOCK	:
        case  IF_NODE		:
	case WHERE_BLOCK_STMT :
        case  LOOP_NODE	:	
        case  FOR_NODE	:	
        case  FORALL_NODE	:
        case  WHILE_NODE	:
        case  CDOALL_NODE	:
        case  SDOALL_NODE	:
        case  DOACROSS_NODE	:
        case  CDOACROSS_NODE	:
        case  FUNC_HEDR	:
	case  ENUM_DECL:
	case  STRUCT_DECL:
	case  UNION_DECL:
	case  CLASS_DECL:
	case  TECLASS_DECL:
	case  COLLECTION_DECL:
	case  SWITCH_NODE:
        case   ELSEIF_NODE    :
          return where;     
        default:
          Message("No Control Parent in computeControlParent: abort()",0);
          abort();
        }
    }

  switch(BIF_CODE(where))
    {
    case CONT_STAT :
      if (BIF_CP(where) &&
          (BIF_CODE(BIF_CP(where)) != FOR_NODE) &&
          (BIF_CODE(BIF_CP(where)) != WHILE_NODE) &&
          (BIF_CODE(BIF_CP(where)) != LOOP_NODE) &&
          (BIF_CODE(BIF_CP(where)) != CDOALL_NODE) &&
          (BIF_CODE(BIF_CP(where)) != SDOALL_NODE) &&
          (BIF_CODE(BIF_CP(where)) != DOACROSS_NODE) &&
          (BIF_CODE(BIF_CP(where)) != CDOACROSS_NODE))
        {
          cp = BIF_CP(where);
          break;
        }
    case CONTROL_END :
	  cp = BIF_CP(BIF_CP(where)); /* handle by the function insert in */
          break;
      /* that a node with a list of blobs */
    case  GLOBAL	:
    case  PROG_HEDR     :
    case  PROC_HEDR	:	
    case  PROS_HEDR	:	
    case  BASIC_BLOCK	:
    case  IF_NODE	:
    case WHERE_BLOCK_STMT :
    case  LOOP_NODE	:	
    case  FOR_NODE	:	
    case  FORALL_NODE	:
    case  WHILE_NODE	:
    case  CDOALL_NODE	:
    case  SDOALL_NODE	:
    case  DOACROSS_NODE	:
    case  CDOACROSS_NODE :
    case  FUNC_HEDR	:
    case  ENUM_DECL:
    case  STRUCT_DECL:
    case  UNION_DECL:
    case  CLASS_DECL:
    case  TECLASS_DECL:
    case  COLLECTION_DECL:
    case  SWITCH_NODE:
    case   ELSEIF_NODE    :
      cp = where;
          break; 
        default:
          cp = BIF_CP(where); /* dont specify it */
    }

  return cp;  
}


/***************************************************************************/
int insertBfndListIn(first,where,cpin)
PTR_BFND first,where;
PTR_BFND cpin;
{
  PTR_BFND cp;
  PTR_BFND biforblob;
  PTR_BFND temp, last;  
  int inblob2;

  if (!first)
    return 0;
  
  if (!where)
    {
      Message("where not defined in insertBfndListIn: abort()",0);
      abort();
    }

  if (!cpin)
    cp = computeControlParent(where);
  else
    cp = cpin;

  /* find where in the blob list where to insert it */
  /* treat first the special case of if_node */
  if ((BIF_CODE(where) == CONTROL_END) && BIF_CP(where) &&
      (BIF_CODE(BIF_CP(where)) == IF_NODE || BIF_CODE(BIF_CP(where)) == ELSEIF_NODE) &&
      (!findBifInList2 (BIF_CP(where),where)) &&
      BIF_BLOB2(BIF_CP(where)))
    {
      cp = BIF_CP(where);
      inblob2 = TRUE;
      biforblob = NULL;
      last =  getLastNodeList(first);
    }
    else
      {
        biforblob =  getWhereToInsertInBfnd(where,cp);
        last =  getLastNodeList(first);
        inblob2 = findBifInList2 (cp,biforblob);
/*        if (BIF_CODE(where) == ELSEIF_NODE)
          inblob2 = TRUE;*/
      }

  for (temp = first; temp; temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        {
          if (!biforblob)
            {
              if (inblob2)
                firstBfndInList2(temp, cp);
              else
                firstBfndInList1(temp, cp);
            } else
              {
                if (inblob2)
                  insertBfndInList2(temp,biforblob, cp);
                else
                  insertBfndInList1(temp,biforblob, cp);
              }          
          biforblob = temp;       
        }
    }

  updateControlParent(first,last,cp); 
  BIF_NEXT(last) = BIF_NEXT(where);
  BIF_NEXT(where) = first;
  return 1;  
}

/***************************************************************************/
int insertBfndListInList1(first,cpin)
PTR_BFND first;
PTR_BFND cpin;
{
  PTR_BFND biforblob;
  PTR_BFND temp, last;  

  if (!first || !cpin)
    return 0;
  
  biforblob = NULL; 
  last =  getLastNodeList(first);
  for (temp = first; temp; temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        {
          if (!biforblob)
            {
                firstBfndInList1(temp, cpin);
            } else
              {
                  insertBfndInList1(temp,biforblob, cpin);
              }          
          biforblob = temp;       
        }
    }

  updateControlParent(first,last,cpin); 
  return 1;
}

/***************************************************************************/
int appendBfndListToList1(first,cpin)
PTR_BFND first;
PTR_BFND cpin;
{
  PTR_BFND biforblob;
  PTR_BFND temp, last;  

  if (!first || !cpin)
    return 0;
  
  biforblob = NULL; 
  last =  getLastNodeList(first);
  for (temp = first; temp; temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        {
          if (!biforblob)
            {
                appendBfndToList1(temp, cpin);
            } else
              {
                  insertBfndInList1(temp,biforblob, cpin);
              }          
          biforblob = temp;       
        }
    }
  
  updateControlParent(first,last,cpin); 
  
  return 1;
}


/***************************************************************************/
int firstInBfndList2(first,cpin)
PTR_BFND first;
PTR_BFND cpin;
{
  PTR_BFND biforblob;
  PTR_BFND temp, last;  

  if (!first || !cpin)
    return 0;
  
  biforblob = NULL;  
  last =  getLastNodeList(first);
  for (temp = first; temp; temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        {
          if (!biforblob)
            {
                firstBfndInList2(temp, cpin);
            } else
              {
                  insertBfndInList2(temp,biforblob, cpin);
              }          
          biforblob = temp;       
        }
    }

  updateControlParent(first,last,cpin); 
  return 1;
}

/***************************************************************************/
int appendBfndListToList2(first,cpin)
PTR_BFND first;
PTR_BFND cpin;
{
  PTR_BFND biforblob;
  PTR_BFND temp, last;  

  if (!first || !cpin)
    return 0;
  
  biforblob = NULL; 
  last =  getLastNodeList(first);
  for (temp = first; temp; temp = BIF_NEXT(temp))
    {      
      if (!isItInSection(first,last,BIF_CP(temp)))
        {
          if (!biforblob)
            {
                appendBfndToList2(temp, cpin);
            } else
              {
                  insertBfndInList2(temp,biforblob, cpin);
              }          
          biforblob = temp;       
        }
    }

  updateControlParent(first,last,cpin); 
  return 1;
}

/***************************************************************************/
void insertBfndBeforeIn(biftoinsert, bif_current, cpin)
     PTR_BFND bif_current, biftoinsert,cpin;
{
  PTR_BFND  the_one_before = NULL;

  if (! bif_current || ! biftoinsert)
    {
      Message("NULL bif node in biftoinsert\n",0);
      exit(-1);
    }
  
  
  if (BIF_CODE (bif_current) == GLOBAL)
    {
      Message("Cannot insert before global\n",0);
      exit(-1);
    }  

  the_one_before = getNodeBefore (bif_current);
  insertBfndListIn (biftoinsert, the_one_before,cpin);
  
} 


/* warning to be used carefully; i.e. remove sons before a root */
/***************************************************************************/
PTR_BFND deleteBfnd(bif)
     PTR_BFND bif;
{
  PTR_BFND temp;
  
  temp = getNodeBefore (bif);
  deleteBfndFrom (BIF_CP (bif), bif);
  if (temp) 
    BIF_NEXT (temp) = BIF_NEXT (bif);
  return temp;
}


/***************************************************************************/
int isItInSection(bif_depart, bif_fin, noeud)
     PTR_BFND bif_depart, bif_fin, noeud;
{
  PTR_BFND temp;
  
  if (! noeud)
    return FALSE;
  
  for (temp = bif_depart; temp; temp = BIF_NEXT (temp))
    {
      if (temp ==  noeud)
        return TRUE;
      if (temp == bif_fin)
        return FALSE;
    }
  return FALSE;
  
}


/***************************************************************************/
PTR_BFND extractBifSectionBetween(bif_depart, bif_fin)
     PTR_BFND bif_depart, bif_fin;
{
  PTR_BFND temp;
  
  if (bif_depart && bif_fin)
    {
      for (temp = bif_depart; temp !=  bif_fin; temp = BIF_NEXT (temp))
        {
          if (!isItInSection(bif_depart, bif_fin,BIF_CP (temp)))
            {
              deleteBfndFrom(BIF_CP (temp),temp);
              BIF_CP (temp) = NULL;              
            }
        }
  
      /* on traite maintenant bif_fin */
      if (!isItInSection(bif_depart, bif_fin,BIF_CP ( bif_fin)))
        {
          deleteBfndFrom(BIF_CP (bif_fin), bif_fin);
           BIF_CP (bif_fin) = NULL;   
        }

      temp = getNodeBefore(bif_depart);
      if (temp && bif_fin)
        BIF_NEXT(temp) = BIF_NEXT (bif_fin);
      BIF_NEXT (bif_fin) = NULL;
    }  
  
  return bif_depart;
}

/***************************************************************************/
PTR_BFND getLastNodeList(b)
     PTR_BFND b;
{
  PTR_BFND temp;  
  for (temp = b; temp; temp = BIF_NEXT(temp))
    {      
      if (!BIF_NEXT(temp))
        {
          return temp;          
        }
    }
  return temp;  
}

/***************************************************************************/
PTR_BFND getLastNodeOfStmt(b)
     PTR_BFND b;
{
  PTR_BLOB temp,last = NULL;
  if (!b)
    return NULL;
  if (BIF_BLOB2(b))
    {
      for (temp = BIF_BLOB2(b); temp ; temp = BLOB_NEXT(temp))
        {
          last = temp;
        }
    } else
     {
       for (temp = BIF_BLOB1(b); temp ; temp = BLOB_NEXT(temp))
         {
           last = temp;
         }      
     } 
  if (last)
    {  
      if (Check_Lang_Fortran(cur_proj))
        return BLOB_VALUE(last);
      else
        { /* in C the Control end may not exist */
          return getLastNodeOfStmt(BLOB_VALUE(last));
        }
    }
  else
    return b;
}

/* version that does not assume, there is a last */
/***************************************************************************/
PTR_BFND getLastNodeOfStmtNoControlEnd(b)
     PTR_BFND b;
{
  PTR_BLOB temp,last = NULL;
  if (!b)
    return NULL;
  if (BIF_BLOB2(b))
    {
      for (temp = BIF_BLOB2(b); temp ; temp = BLOB_NEXT(temp))
        {
          last = temp;
        }
    } else
     {
       for (temp = BIF_BLOB1(b); temp ; temp = BLOB_NEXT(temp))
         {
           last = temp;
         }      
     } 
  if (last)
    {  
      return getLastNodeOfStmt(BLOB_VALUE(last));
    }
  else
    return b;
}

/* preset some values of symbols for evaluateExpression*/
#define ALLOCATECHUNKVALUE  100
static PTR_SYMB  *ValuesSymb = NULL;
static int       *ValuesInt  = NULL;
static int        NbValues   = 0;
static int        NbElement   = 0;

/***************************************************************************/
void allocateValueEvaluate()
{
  int i;
  PTR_SYMB  *pt1;
  int       *pt2;
  
  pt1 = (PTR_SYMB  *) xmalloc( sizeof(PTR_SYMB  *) * 
			      (NbValues + ALLOCATECHUNKVALUE));
  pt2 =  (int *) xmalloc( sizeof(int  *) * (NbValues + ALLOCATECHUNKVALUE));
  
  for (i=0; i<NbValues + ALLOCATECHUNKVALUE; i++) {
    pt1[i] = NULL;
    pt2[i] = 0;
  }
  
  for (i=0 ; i < NbValues; i++) {
    pt1[i] = ValuesSymb[i];
    pt2[i] = ValuesInt[i];
  }
  
  if (NbValues) {
#ifdef __SPF
      removeFromCollection(ValuesSymb);
      removeFromCollection(ValuesInt);
#endif
    free(ValuesSymb);
    free(ValuesInt);
  }
    
  ValuesSymb = pt1;
  ValuesInt = pt2;
  NbValues = NbValues + ALLOCATECHUNKVALUE;
}

/***************************************************************************/
void addElementEvaluate(symb, val)
     PTR_SYMB symb;
     int val;
{
  if (!symb)
    return;
  while (NbValues <= ( NbElement+1))
    {
      allocateValueEvaluate();
    }
  ValuesSymb[NbElement] = symb;
  ValuesInt[NbElement] = val;
  NbElement++;
}


/***************************************************************************/
int getElementEvaluate(symb)
     PTR_SYMB symb;
{
  int i;
  if (!symb)
    return -1;
  for (i=0 ; i < NbElement; i++)
    {
      if (ValuesSymb[i] == symb)
        return i;
    }
  return -1;
}


/***************************************************************************/
void resetPresetEvaluate()
{
  NbValues = 0;
  NbElement  = 0;
  if (ValuesSymb) {
#ifdef __SPF
      removeFromCollection(ValuesSymb);      
#endif
      free(ValuesSymb);
  }
  if (ValuesInt)
  {
#ifdef __SPF      
      removeFromCollection(ValuesInt);
#endif
      free(ValuesInt);
  }
  ValuesSymb = NULL;
  ValuesInt = NULL;
}

/***************************************************************************/
int* evaluateExpression(expr)
     PTR_LLND expr;
{
  int *res, *op1, *op2, i;
  
  res = (int *) xmalloc(2 * sizeof(int));
  memset((char *) res, 0, 2 * sizeof (int));
  op1 = (int *) xmalloc(2 * sizeof(int));
  memset((char *)op1, 0, 2 * sizeof (int));
  op2 = (int *) xmalloc(2 * sizeof(int));
  memset((char *) op2, 0, 2 * sizeof (int));
  if (! expr)
    {
      res [0] = -1;
      return res;
    }
  
  switch (BIF_CODE (expr)) 
    {
    case INT_VAL:
      res [1] = NODE_INT_CST_LOW (expr);
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
    case ADD_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] + op2 [1];
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
   case MULT_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] * op2 [1];
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
   case SUBT_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] - op2 [1];
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
   case DIV_OP :
        op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1) || (op2[1] == 0))  /*28.05.17 Kolganov*/
        res [0] = -1;
      else
        res [1] = op1 [1] / op2 [1];
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
   case MOD_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] % op2 [1];
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
   case EXP_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else {
        res [1] = op1 [1];
        for(i=1; i<op2 [1]; i++)
          res [1] = res [1] * op1 [1];
      }
#ifdef __SPF      
      removeFromCollection(op1);
      removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;

    case MINUS_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      if (op1 [0] == -1)
        res [0] = -1;
      else
        res [1] = - op1 [1];
#ifdef __SPF      
      removeFromCollection(op1);      
#endif
      free(op1);
      return res;
    case VAR_REF: /* assume here that some value for Symbole are given*/
      {
        int ind;
        if ((ind = getElementEvaluate(NODE_SYMB(expr))) != -1)
          {
             res [1] = ValuesInt[ind];
#ifdef __SPF      
             removeFromCollection(op1);
             removeFromCollection(op2);
#endif
             free(op1); free(op2);
             return res;
           } else
             {
                res [0] = -1;
#ifdef __SPF      
                removeFromCollection(op1);
                removeFromCollection(op2);
#endif
                free(op1); free(op2);
                return res;
             }
      }
   default :
     res [0] = -1;
#ifdef __SPF      
     removeFromCollection(op1);
     removeFromCollection(op2);
#endif
      free(op1); free(op2);
      return res;
    }
}

/***************************************************************************/
PTR_BFND duplicateStmts(body)
     PTR_BFND body;
{
 PTR_BFND copie, last, temp, cherche, lastnode;
 int lenght,i,j;
 PTR_BFND *alloue;
 PTR_BLOB blobtemp;
 PTR_LABEL *label_insection;
 PTR_LABEL lab;
 int maxlabelname;

 if (! body) return NULL;
  /* on calcul d'abord la longueur */

 maxlabelname = getLastLabelId();
 
 lenght = 0;
 for (temp = body; temp ; temp = BIF_NEXT(temp))
   {
     lenght++;
     lastnode = temp;     
   }
 alloue = (PTR_BFND *) xmalloc(2*lenght * sizeof(PTR_BFND));
 memset((char *) alloue, 0, 2* lenght * sizeof(PTR_BFND));
 
 /* label part, we record label */
 label_insection = (PTR_LABEL *) xmalloc(2*lenght * sizeof(PTR_LABEL));
 memset((char *) label_insection, 0, 2* lenght * sizeof(PTR_LABEL)); 
 temp = body;
 last = NULL;
 for (i = 0; i < lenght; i++)
   {
      copie = (PTR_BFND) newNode (BIF_CODE (temp));
      BIF_SYMB (copie) = BIF_SYMB (temp);
      BIF_LL1 (copie) = copyLlNode(BIF_LL1 (temp));
      BIF_LL2 (copie) = copyLlNode(BIF_LL2 (temp));
      BIF_LL3 (copie) = copyLlNode(BIF_LL3 (temp));
      BIF_DECL_SPECS (copie) = BIF_DECL_SPECS(temp);
      if (last)
        BIF_NEXT(last) = copie;

     
      if (BIF_LABEL(temp))/* && (LABEL_BODY(BIF_LABEL(temp)) == temp))*/
	{
	  /* create a new label */
	  label_insection[2*i+1] = (PTR_LABEL) newNode(LABEL_KIND); 
	  maxlabelname++;
	  LABEL_STMTNO(label_insection[2*i+1]) = maxlabelname;
	  LABEL_BODY(label_insection[2*i+1]) = copie;
	  LABEL_USED(label_insection[2*i+1]) = LABEL_USED(BIF_LABEL(temp));
	  LABEL_ILLEGAL(label_insection[2*i+1])=LABEL_ILLEGAL(BIF_LABEL(temp));
	  LABEL_DEFINED(label_insection[2*i+1])=LABEL_DEFINED(BIF_LABEL(temp));
	  BIF_LABEL(copie) = label_insection[2*i+1];
	  label_insection[2*i] = BIF_LABEL(temp);
	} 	  

      /* on fait corresponde temp et copie */
      alloue[2*i] = temp;
      alloue[2*i+1] = copie;
      temp = BIF_NEXT(temp);
      last = copie;
   }

 /* On met a jour les labels */
 temp = body;
 for (i = 0; i < lenght; i++)
   {
     int cas;
     copie = alloue[2*i+1]; 
     lab = NULL;
     
     /* We treat first the COMGOTO_NODE first */
     if (BIF_CODE(temp) == COMGOTO_NODE)
       {
         PTR_LLND listlab, ptl;
         int trouve = 0;	
         
         listlab = BIF_LL1(copie);
         while (listlab)
           {
             ptl = NODE_OPERAND0(listlab);
             /* we look in the list */
             if (ptl)
               {                
                 lab = NODE_LABEL(ptl);
                 trouve = 0;	 
                 for (j = 0; j < lenght; j++)
                   {
                     if (label_insection[2*j])
                       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
                         {
                           trouve = j+1;
                           break;
                         }
                   }
                 if(trouve)
                   {
                     NODE_LABEL(ptl) =  label_insection[2*(trouve-1)+1];
                   }
               }
             listlab = NODE_OPERAND1(listlab);             
           }
         temp = BIF_NEXT(temp);
         continue;         
       }
     
         
     if (BIF_LL3(temp) && (NODE_CODE(BIF_LL3(temp)) == LABEL_REF))
       {
	 lab = NODE_LABEL(BIF_LL3(temp));
	 cas = 2;	
       }
     else
       {
	 lab = BIF_LABEL_USE(temp);
	 cas = 1;
       }
     if (lab)
       { /* look where the label is the label is defined somewhere */
	 int trouve = 0;	 
	 for (j = 0; j < lenght; j++)
	   {
	     if (label_insection[2*j])
	       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
		 {
		   trouve = j+1;
		   break;
		 }
	   }
	 if(trouve)
	   {
	     if (cas == 1)
	       {
		 BIF_LABEL_USE(copie) = label_insection[2*(trouve-1)+1];
	       }
	     if (cas == 2)
	       {
		 if (BIF_LL3(copie))
		   {
		     NODE_LABEL(BIF_LL3(copie)) = label_insection[2*(trouve-1)+1];
		   }
	       }
	   } else
	     {
	       if (cas == 1)
		 BIF_LABEL_USE(copie) = lab; /* outside */
	       /* if ((cas == 2) no change */
	     }
       }
     temp = BIF_NEXT(temp);
   }
 
 /* on met a jour le blob list */
 copie = alloue[1];
 for (temp = body; temp ; temp = BIF_NEXT(temp))
   {
     if (BIF_BLOB1(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB1(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList1(cherche, copie);
           }
       }
     if (BIF_BLOB2(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB2(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList2(cherche, copie);
           }
       }
     copie = BIF_NEXT(copie);
   }

 /* on remet ici a jour les CP */
 copie = alloue[1];
 for (temp = body; temp ; temp = BIF_NEXT(temp))
   {
     if (isItInSection(body, lastnode, BIF_CP(temp)))
       { /* on cherche le bif_cp pour la copie */
         cherche = NULL;
         for (i = 0; i <lenght ; i++)
           {
             if (alloue[2*i] == BIF_CP(temp))
               {
                 cherche = alloue[2*i+1];
                 break;
               }
           }
         BIF_CP(copie) = cherche;
       }
     copie = BIF_NEXT(copie);
   }
 copie = alloue[1];
#ifdef __SPF      
 removeFromCollection(alloue);
 removeFromCollection(label_insection);
#endif
 free(alloue);
 free(label_insection);
 return copie;
}

/***************************************************************************/
int isLabelId(num)
   int num; 
{ PTR_LABEL lab;
  for(lab=PROJ_FIRST_LABEL(); lab; lab=LABEL_NEXT(lab)) 
     if( num == LABEL_STMTNO(lab))
       return 1;
  return 0;
}

/***************************************************************************/
int getNewLabelId(llab)
  int llab;
{  
  while(isLabelId(llab))
    ++llab;

  return llab;
}
   

/************************************** New version, does not need extract **************/
/********************************* only copies a statement and the children *************/
PTR_BFND duplicateStmtsNoExtract(PTR_BFND body)
{
    PTR_BFND copie, last, temp, cherche, lastnode;
    int lenght, i, j;
    PTR_BFND *alloue;
    PTR_BLOB blobtemp;
    PTR_LABEL *label_insection;
    PTR_LABEL lab;
    int maxlabelname, newlabelname;

    if (!body)
        return NULL;
    /* on calcul d'abord la longueur */

    maxlabelname = getLastLabelId();
    newlabelname = 0;
    lastnode = getLastNodeOfStmt(body);

    /*podd 03.06.14*/
    if (BIF_CODE(body) == IF_NODE || BIF_CODE(body) == ELSEIF_NODE)
        while (BIF_CODE(lastnode) == ELSEIF_NODE)
            lastnode = getLastNodeOfStmt(lastnode);
    else if (BIF_CODE(body) == FOR_NODE || BIF_CODE(body) == WHILE_NODE)
    {
        while (BIF_CODE(lastnode) == FOR_NODE || BIF_CODE(lastnode) == WHILE_NODE)
            lastnode = getLastNodeOfStmt(lastnode);
        if (BIF_CODE(lastnode) == LOGIF_NODE)
            lastnode = BIF_NEXT(lastnode);
    }

    lenght = 0;
    for (temp = body; temp; temp = BIF_NEXT(temp))
    {
        lenght++;
        if (lastnode == temp)
            break;
    }
    alloue = (PTR_BFND *)xmalloc(2 * lenght * sizeof(PTR_BFND));
    memset((char *)alloue, 0, 2 * lenght * sizeof(PTR_BFND));

    /* label part, we record label */
    label_insection = (PTR_LABEL *)xmalloc(2 * lenght * sizeof(PTR_LABEL));
    memset((char *)label_insection, 0, 2 * lenght * sizeof(PTR_LABEL));
    temp = body;
    last = NULL;
    for (i = 0; i < lenght; i++)
    {
        copie = (PTR_BFND)newNode(BIF_CODE(temp));
        BIF_SYMB(copie) = BIF_SYMB(temp);
        BIF_LL1(copie) = copyLlNode(BIF_LL1(temp));
        BIF_LL2(copie) = copyLlNode(BIF_LL2(temp));
        BIF_LL3(copie) = copyLlNode(BIF_LL3(temp));
        if (last)
            BIF_NEXT(last) = copie;


        if (BIF_LABEL(temp))/* && (LABEL_BODY(BIF_LABEL(temp)) == temp))*/
        {
            /* create a new label */
            label_insection[2 * i + 1] = (PTR_LABEL)newNode(LABEL_KIND);
            if (maxlabelname < 99999)
                newlabelname = ++maxlabelname;
            else
                newlabelname = getNewLabelId(++newlabelname);
            LABEL_STMTNO(label_insection[2 * i + 1]) = newlabelname;
            LABEL_BODY(label_insection[2 * i + 1]) = copie;
            LABEL_USED(label_insection[2 * i + 1]) = LABEL_USED(BIF_LABEL(temp));
            LABEL_ILLEGAL(label_insection[2 * i + 1]) = LABEL_ILLEGAL(BIF_LABEL(temp));
            LABEL_DEFINED(label_insection[2 * i + 1]) = LABEL_DEFINED(BIF_LABEL(temp));
            BIF_LABEL(copie) = label_insection[2 * i + 1];
            label_insection[2 * i] = BIF_LABEL(temp);
        }

        /* on fait corresponde temp et copie */
        alloue[2 * i] = temp;
        alloue[2 * i + 1] = copie;
        temp = BIF_NEXT(temp);
        last = copie;
    }

    /* On met a jour les labels */
    temp = body;
    for (i = 0; i < lenght; i++)
    {
        int cas, kind;
        copie = alloue[2 * i + 1];
        lab = NULL;

        /* We treat first the COMGOTO_NODE first */
        switch (BIF_CODE(temp)) 
        {
        case COMGOTO_NODE:
        case ASSGOTO_NODE:
            kind = 2;
            break;
        case ARITHIF_NODE:
            kind = 3;
            break;
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
        case BACKSPACE_STAT:
        case REWIND_STAT:
        case ENDFILE_STAT:
        case INQUIRE_STAT:
        case OPEN_STAT:
        case CLOSE_STAT:
            kind = 1;
            break;
        default:
            kind = 0;
            break;
        }


        if (kind == 1)
        {
            PTR_LLND lb, list;

            list = BIF_LL2(copie); /*control list or format*/
            if (list && NODE_CODE(list) == EXPR_LIST)
            {
                for (; list; list = NODE_OPERAND1(list))
                {
                    lb = NODE_OPERAND1(NODE_OPERAND0(list));
                    if (NODE_CODE(lb) == LABEL_REF)
                        lab = NODE_LABEL(lb);
                    if (lab)
                    { /* look where the label is the label is defined somewhere */
                        int trouve = 0;
                        for (j = 0; j < lenght; j++)
                        {
                            if (label_insection[2 * j])
                                if (LABEL_STMTNO(label_insection[2 * j]) == LABEL_STMTNO(lab))
                                {
                                    trouve = j + 1;
                                    break;
                                }
                        }
                        if (trouve)
                        {
                            NODE_LABEL(lb) = label_insection[2 * (trouve - 1) + 1];
                        }
                    }
                }
            }

            else if (list && (NODE_CODE(list) == SPEC_PAIR))
            {
                lb = (NODE_OPERAND1(list));
                if (NODE_CODE(lb) == LABEL_REF)
                    lab = NODE_LABEL(lb);
                if (lab)
                { /* look where the label is the label is defined somewhere */
                    int trouve = 0;
                    for (j = 0; j < lenght; j++)
                    {
                        if (label_insection[2 * j])
                            if (LABEL_STMTNO(label_insection[2 * j]) == LABEL_STMTNO(lab))
                            {
                                trouve = j + 1;
                                break;
                            }
                    }
                    if (trouve)
                    {
                        NODE_LABEL(lb) = label_insection[2 * (trouve - 1) + 1];
                    }
                }
            }
            temp = BIF_NEXT(temp);
            continue;
        }


        if (kind > 1)
        {
            PTR_LLND listlab, ptl;
            int trouve = 0;

            listlab = (kind == 2) ? BIF_LL1(copie) : BIF_LL2(copie);
            while (listlab)
            {
                ptl = NODE_OPERAND0(listlab);
                /* we look in the list */
                if (ptl)
                {
                    lab = NODE_LABEL(ptl);
                    trouve = 0;
                    for (j = 0; j < lenght; j++)
                    {
                        if (label_insection[2 * j])
                            if (LABEL_STMTNO(label_insection[2 * j]) == LABEL_STMTNO(lab))
                            {
                                trouve = j + 1;
                                break;
                            }
                    }
                    if (trouve)
                    {
                        NODE_LABEL(ptl) = label_insection[2 * (trouve - 1) + 1];
                    }
                }
                listlab = NODE_OPERAND1(listlab);
            }
            temp = BIF_NEXT(temp);
            continue;
        }



        lab = NULL;
        if (BIF_LL3(temp) && (NODE_CODE(BIF_LL3(temp)) == LABEL_REF))
        {
            lab = NODE_LABEL(BIF_LL3(temp));
            cas = 2;
        }
        else if (BIF_LL1(temp) && (NODE_CODE(BIF_LL1(temp)) == LABEL_REF))
        {
            lab = NODE_LABEL(BIF_LL1(temp));
            cas = 3;
        }
        else
        {
            lab = BIF_LABEL_USE(temp);
            cas = 1;
        }
        if (lab)
        { /* look where the label is the label is defined somewhere */
            int trouve = 0;
            for (j = 0; j < lenght; j++)
            {
                if (label_insection[2 * j])
                    if (LABEL_STMTNO(label_insection[2 * j]) == LABEL_STMTNO(lab))
                    {
                        trouve = j + 1;
                        break;
                    }
            }
            if (trouve)
            {
                if (cas == 1)
                {
                    BIF_LABEL_USE(copie) = label_insection[2 * (trouve - 1) + 1];
                }
                if (cas == 2)
                {
                    if (BIF_LL3(copie))
                    {
                        NODE_LABEL(BIF_LL3(copie)) = label_insection[2 * (trouve - 1) + 1];
                    }
                }
                if (cas == 3)
                {
                    if (BIF_LL1(copie))
                    {
                        NODE_LABEL(BIF_LL1(copie)) = label_insection[2 * (trouve - 1) + 1];
                    }
                }

            }
            else
            {
                if (cas == 1)
                    BIF_LABEL_USE(copie) = lab; /* outside */
                      /* if ((cas == 2) no change */
            }
        }
        temp = BIF_NEXT(temp);
    }

    /* on met a jour le blob list */
    copie = alloue[1];
    for (temp = body; temp; temp = BIF_NEXT(temp))
    {
        if (BIF_BLOB1(temp))
        { /* on doit cree la blob liste */
            for (blobtemp = BIF_BLOB1(temp); blobtemp;
                blobtemp = BLOB_NEXT(blobtemp))
            {
                /* on cherche la reference dans le tableaux allouer */
                cherche = NULL;
                for (i = 0; i < lenght; i++)
                {
                    if (alloue[2 * i] == BLOB_VALUE(blobtemp))
                    {
                        cherche = alloue[2 * i + 1];
                        break;
                    }
                }
                appendBfndToList1(cherche, copie);
            }
        }
        if (BIF_BLOB2(temp))
        { /* on doit cree la blob liste */
            for (blobtemp = BIF_BLOB2(temp); blobtemp;
                blobtemp = BLOB_NEXT(blobtemp))
            {
                /* on cherche la reference dans le tableaux allouer */
                cherche = NULL;
                for (i = 0; i < lenght; i++)
                {
                    if (alloue[2 * i] == BLOB_VALUE(blobtemp))
                    {
                        cherche = alloue[2 * i + 1];
                        break;
                    }
                }
                appendBfndToList2(cherche, copie);
            }
        }
        copie = BIF_NEXT(copie);
        if (temp == lastnode)
            break;
    }

    /* on remet ici a jour les CP */
    copie = alloue[1];
    for (temp = body; temp; temp = BIF_NEXT(temp))
    {
        if (isItInSection(body, lastnode, BIF_CP(temp)))
        { /* on cherche le bif_cp pour la copie */
            cherche = NULL;
            for (i = 0; i < lenght; i++)
            {
                if (alloue[2 * i] == BIF_CP(temp))
                {
                    cherche = alloue[2 * i + 1];
                    break;
                }
            }
            BIF_CP(copie) = cherche;
        }
        else
            BIF_CP(copie) = NULL;
        copie = BIF_NEXT(copie);
        if (temp == lastnode)
            break;
    }
    copie = alloue[1];
#ifdef __SPF      
    removeFromCollection(alloue);
    removeFromCollection(label_insection);
#endif
    free(alloue);
    free(label_insection);
    return copie;
}



/* (ajm)
   This function will copy one statement and all of its children 
   (presumably; I didn't touch that one way or the other). 

   It differs from low_level.c:duplicateStmt (v1.00) in that does not 
   copy all of the BIF_NEXT successors of the statement as well.

*/

/***************************************************************************/
PTR_BFND duplicateOneStmt(body)
     PTR_BFND body;
{
 PTR_BFND copie, last, temp, cherche, lastnode;
 int lenght,i,j;
 PTR_BFND *alloue;
 PTR_BLOB blobtemp;
 PTR_LABEL *label_insection;
 PTR_LABEL lab;
 int maxlabelname;

 if (! body) return NULL;
  /* on calcul d'abord la longueur */

 maxlabelname = getLastLabelId();
 
 lenght = 0;
/* Changed area, by ajm 1-Feb-94 */
#if 0
 for (temp = body; temp ; temp = BIF_NEXT(temp))
   {
     lenght++;
     lastnode = temp;     
   }
#else
 if ( body != 0 )
 {
      lenght = 1;
      lastnode = body;/*podd 12.03.99*/
 }
#endif /* ajm */

 alloue = (PTR_BFND *) xmalloc(2*lenght * sizeof(PTR_BFND));
 memset((char *) alloue, 0, 2* lenght * sizeof(PTR_BFND));
 
 /* label part, we record label */
 label_insection = (PTR_LABEL *) xmalloc(2*lenght * sizeof(PTR_LABEL));
 memset((char *) label_insection, 0, 2* lenght * sizeof(PTR_LABEL)); 
 temp = body;
 last = NULL;
 for (i = 0; i < lenght; i++)
   {
      copie = (PTR_BFND) newNode (BIF_CODE (temp));
      BIF_SYMB (copie) = BIF_SYMB (temp);
      BIF_LL1 (copie) = copyLlNode(BIF_LL1 (temp));
      BIF_LL2 (copie) = copyLlNode(BIF_LL2 (temp));
      BIF_LL3 (copie) = copyLlNode(BIF_LL3 (temp));
      BIF_DECL_SPECS (copie) = BIF_DECL_SPECS(temp);

      if (last)
        BIF_NEXT(last) = copie;

     
      if (BIF_LABEL(temp))/* && (LABEL_BODY(BIF_LABEL(temp)) == temp))*/
	{
	  /* create a new label */
	  label_insection[2*i+1] = (PTR_LABEL) newNode(LABEL_KIND); 
	  maxlabelname++;
	  LABEL_STMTNO(label_insection[2*i+1]) = maxlabelname;
	  LABEL_BODY(label_insection[2*i+1]) = copie;
	  LABEL_USED(label_insection[2*i+1]) = LABEL_USED(BIF_LABEL(temp));
	  LABEL_ILLEGAL(label_insection[2*i+1])=LABEL_ILLEGAL(BIF_LABEL(temp));
	  LABEL_DEFINED(label_insection[2*i+1])=LABEL_DEFINED(BIF_LABEL(temp));
	  BIF_LABEL(copie) = label_insection[2*i+1];
	  label_insection[2*i] = BIF_LABEL(temp);
	} 	  

      /* on fait corresponde temp et copie */
      alloue[2*i] = temp;
      alloue[2*i+1] = copie;
      temp = BIF_NEXT(temp);
      last = copie;
   }

 /* On met a jour les labels */
 temp = body;
 for (i = 0; i < lenght; i++)
   {
     int cas;
     copie = alloue[2*i+1]; 
     lab = NULL;
     
     /* We treat first the COMGOTO_NODE first */
     if (BIF_CODE(temp) == COMGOTO_NODE)
       {
         PTR_LLND listlab, ptl;
         int trouve = 0;	
         
         listlab = BIF_LL1(copie);
         while (listlab)
           {
             ptl = NODE_OPERAND0(listlab);
             /* we look in the list */
             if (ptl)
               {                
                 lab = NODE_LABEL(ptl);
                 trouve = 0;	 
                 for (j = 0; j < lenght; j++)
                   {
                     if (label_insection[2*j])
                       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
                         {
                           trouve = j+1;
                           break;
                         }
                   }
                 if(trouve)
                   {
                     NODE_LABEL(ptl) =  label_insection[2*(trouve-1)+1];
                   }
               }
             listlab = NODE_OPERAND1(listlab);             
           }
         temp = BIF_NEXT(temp);
         continue;         
       }
     
         
     if (BIF_LL3(temp) && (NODE_CODE(BIF_LL3(temp)) == LABEL_REF))
       {
	 lab = NODE_LABEL(BIF_LL3(temp));
	 cas = 2;	
       }
     else
       {
	 lab = BIF_LABEL_USE(temp);
	 cas = 1;
       }
     if (lab)
       { /* look where the label is the label is defined somewhere */
	 int trouve = 0;	 
	 for (j = 0; j < lenght; j++)
	   {
	     if (label_insection[2*j])
	       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
		 {
		   trouve = j+1;
		   break;
		 }
	   }
	 if(trouve)
	   {
	     if (cas == 1)
	       {
		 BIF_LABEL_USE(copie) = label_insection[2*(trouve-1)+1];
	       }
	     if (cas == 2)
	       {
		 if (BIF_LL3(copie))
		   {
		     NODE_LABEL(BIF_LL3(copie)) = label_insection[2*(trouve-1)+1];
		   }
	       }
	   } else
	     {
	       if (cas == 1)
		 BIF_LABEL_USE(copie) = lab; /* outside */
	       /* if ((cas == 2) no change */
	     }
       }
     temp = BIF_NEXT(temp);
   }
 
 /* on met a jour le blob list */
 copie = alloue[1];
/* Change by ajm */
#if 0
   for (temp = body; temp ; temp = BIF_NEXT(temp))
#else
   for (temp = body; temp ; temp = 0 /* not BIF_NEXT(temp)!! */ )
#endif
   {
     if (BIF_BLOB1(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB1(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList1(cherche, copie);
           }
       }
     if (BIF_BLOB2(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB2(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList2(cherche, copie);
           }
       }
     copie = BIF_NEXT(copie);
   }

 /* on remet ici a jour les CP */
 copie = alloue[1];

/* Change by ajm */
#if 0
   for (temp = body; temp ; temp = BIF_NEXT(temp))
#else
   for (temp = body; temp ; temp = 0 /* not BIF_NEXT(temp)!! */ )
#endif
   {
     if (isItInSection(body, lastnode, BIF_CP(temp)))
       { /* on cherche le bif_cp pour la copie */
         cherche = NULL;
         for (i = 0; i <lenght ; i++)
           {
             if (alloue[2*i] == BIF_CP(temp))
               {
                 cherche = alloue[2*i+1];
                 break;
               }
           }
         BIF_CP(copie) = cherche;
       }
     copie = BIF_NEXT(copie);
   }
 copie = alloue[1];
#ifdef __SPF      
 removeFromCollection(alloue);
 removeFromCollection(label_insection);
#endif
 free(alloue);
 free(label_insection);
 return copie;
}

/* BW, june 1994
   This function will copy all the remaining statements in the block to which
   the given statement belongs (and all of its children) including the given statement 

   It is yet another variation on duplicateStmts and duplicateOneStmt. It
   is not "fundamentally right" because it does not reproduce the "top-level" blob
   structure of the block being copied (but it does reproduce the internal 
   blob structure). 
   As a result, calling duplicateStmtsBlock with an argument that is itself a result 
   of duplicateStmtsBlock will not work.

*/

/***************************************************************************/
PTR_BFND duplicateStmtsBlock(body,saveLabelId)
     PTR_BFND body;
     int saveLabelId;
{
 PTR_BFND copie, last, temp, cherche, lastnode;
 int lenght,i,j;
 PTR_BFND *alloue;
 PTR_BLOB blobtemp;
 PTR_LABEL *label_insection;
 PTR_LABEL lab;
 int maxlabelname, newlabelname;
 PTR_BLOB blob;
 int iii;

 if (! body) return NULL;
  /* on calcul d'abord la longueur */

 maxlabelname = getLastLabelId();
 newlabelname = 0;  /*podd 13.01.14*/
 lenght = 0;

/* need to find the correct blob chain and then count how many bif nodes we want to copy */
  if (!BIF_CP(body))
    {
     Message("Error in duplicateStmtsBlock, can't find control parent", 0);
     exit(0);
    }
  else
    {
      blob = lookForBifInBlobList(BIF_BLOB1(BIF_CP(body)), body);
      if (!blob)
        blob = lookForBifInBlobList(BIF_BLOB2(BIF_CP(body)), body);
      if (!blob)
        {
         Message("Error in duplicateStmtsBlock, can't find the blob", 0);
         exit(0);
        }
    }

 lenght = numberOfBifsInBlobList(blob);

 for (temp = body, i = 0; i < lenght ; temp = BIF_NEXT(temp), i++)
     lastnode = temp;     

 alloue = (PTR_BFND *) xmalloc(2*lenght * sizeof(PTR_BFND));
 memset((char *) alloue, 0, 2* lenght * sizeof(PTR_BFND));
 
 /* label part, we record label */
 label_insection = (PTR_LABEL *) xmalloc(2*lenght * sizeof(PTR_LABEL));
 memset((char *) label_insection, 0, 2* lenght * sizeof(PTR_LABEL)); 
 temp = body;
 last = NULL;
 for (i = 0; i < lenght; i++)
   {
      copie = (PTR_BFND) newNode (BIF_CODE (temp));
      BIF_SYMB (copie) = BIF_SYMB (temp);
      BIF_LL1 (copie) = copyLlNode(BIF_LL1 (temp));
      BIF_LL2 (copie) = copyLlNode(BIF_LL2 (temp));
      BIF_LL3 (copie) = copyLlNode(BIF_LL3 (temp));
      BIF_DECL_SPECS (copie) = BIF_DECL_SPECS(temp);

      if (last)
        BIF_NEXT(last) = copie;

     
      if (BIF_LABEL(temp))/* && (LABEL_BODY(BIF_LABEL(temp)) == temp))*/
	{
	  /* create a new label */
	  label_insection[2*i+1] = (PTR_LABEL) newNode(LABEL_KIND);
          
          if(saveLabelId)
              newlabelname = LABEL_STMTNO(BIF_LABEL(temp));  /*use  Label Id of source statement*/
          else
	  { if(maxlabelname<99999)      /*podd 06.04.13*/
              newlabelname = ++maxlabelname;  
            else
              newlabelname = getNewLabelId(++newlabelname); 
          }
	  /*maxlabelname++;*/  /*podd 06.04.13*/
	  LABEL_STMTNO(label_insection[2*i+1]) = newlabelname; /* maxlabelname-> newlabelname  *//*podd 13.01.14*/
	  LABEL_BODY(label_insection[2*i+1]) = copie;
	  LABEL_USED(label_insection[2*i+1]) = LABEL_USED(BIF_LABEL(temp));
	  LABEL_ILLEGAL(label_insection[2*i+1])=LABEL_ILLEGAL(BIF_LABEL(temp));
	  LABEL_DEFINED(label_insection[2*i+1])=LABEL_DEFINED(BIF_LABEL(temp));
	  BIF_LABEL(copie) = label_insection[2*i+1];
	  label_insection[2*i] = BIF_LABEL(temp);
	} 	  

      /* on fait corresponde temp et copie */
      alloue[2*i] = temp;
      alloue[2*i+1] = copie;
      temp = BIF_NEXT(temp);
      last = copie;
   }

 /* On met a jour les labels */ /*podd 06.04.13  this fragment (renewing of label references ) is copied from function duplicateStmtsNoExtract()*/
 temp = body;
 for (i = 0; i < lenght; i++)
   {
     int cas, kind;
     copie = alloue[2*i+1]; 
     lab = NULL;
     
     /* We treat first the COMGOTO_NODE first */
     switch(BIF_CODE(temp)) {
       case COMGOTO_NODE:
       case ASSGOTO_NODE:
            kind = 2;
            break;
       case ARITHIF_NODE:
            kind = 3;
            break;
       case WRITE_STAT:
       case READ_STAT:
       case PRINT_STAT:
       case BACKSPACE_STAT:
       case REWIND_STAT:
       case ENDFILE_STAT:
       case INQUIRE_STAT:
       case OPEN_STAT:
       case CLOSE_STAT:
            kind = 1;
            break;
       default:
            kind = 0;
            break;
     }


     if(kind == 1)
       {
         PTR_LLND lb, list;
	
         list = BIF_LL2(copie); /*control list or format*/
         if(list && NODE_CODE(list) == EXPR_LIST)
          {  
           for(;list;list=NODE_OPERAND1(list))
           { 
            lb = NODE_OPERAND1(NODE_OPERAND0(list)); 
            if(NODE_CODE(lb) == LABEL_REF)
              lab = NODE_LABEL(lb); 
            if (lab)
             { /* look where the label is the label is defined somewhere */
	       int trouve = 0;	 
	       for (j = 0; j < lenght; j++)
	       {
	         if (label_insection[2*j])
	           if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
		   {
		     trouve = j+1;
		     break;
		   }
	       }
               if(trouve)
               {
	       NODE_LABEL(lb) = label_insection[2*(trouve-1)+1];
               }
             } 
           }
          }

          else if(list && (NODE_CODE(list) == SPEC_PAIR))
          {  
            lb =(NODE_OPERAND1(list)); 
            if(NODE_CODE(lb) == LABEL_REF)
              lab = NODE_LABEL(lb);
            if (lab)
             { /* look where the label is the label is defined somewhere */
	       int trouve = 0;	 
	       for (j = 0; j < lenght; j++)
	       {
	         if (label_insection[2*j])
	           if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
		   {
		     trouve = j+1;
		     break;
		   }
	       }
              if(trouve)
              {
	        NODE_LABEL(lb) = label_insection[2*(trouve-1)+1];
              }
            }       
          }
          temp = BIF_NEXT(temp);
          continue;
      }


      if(kind > 1)
       {
         PTR_LLND listlab, ptl;
         int trouve = 0;	
         
         listlab = (kind==2) ? BIF_LL1(copie) : BIF_LL2(copie);
         while (listlab)
           {
             ptl = NODE_OPERAND0(listlab);
             /* we look in the list */
             if (ptl)
               {                
                 lab = NODE_LABEL(ptl);
                 trouve = 0;	 
                 for (j = 0; j < lenght; j++)
                   {
                     if (label_insection[2*j])
                       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
                         {
                           trouve = j+1;
                           break;
                         }
                   }
                 if(trouve)
                   {
                     NODE_LABEL(ptl) =  label_insection[2*(trouve-1)+1];
                   }
               }
             listlab = NODE_OPERAND1(listlab);             
           }
         temp = BIF_NEXT(temp);
         continue;         
       }


     
     lab=NULL;    
     if (BIF_LL3(temp) && (NODE_CODE(BIF_LL3(temp)) == LABEL_REF))
       {
	 lab = NODE_LABEL(BIF_LL3(temp)); 
	 cas = 2;	
       }
     else if (BIF_LL1(temp) && (NODE_CODE(BIF_LL1(temp)) == LABEL_REF))
       {
	 lab = NODE_LABEL(BIF_LL1(temp));
	 cas = 3;	
       }
     else
       {
	 lab = BIF_LABEL_USE(temp);
	 cas = 1;
       }
     if (lab)
       { /* look where the label is the label is defined somewhere */
	 int trouve = 0;	 
	 for (j = 0; j < lenght; j++)
	   {
	     if (label_insection[2*j])
	       if (LABEL_STMTNO(label_insection[2*j]) == LABEL_STMTNO(lab))
		 {
		   trouve = j+1;
		   break;
		 }
	   }
	 if(trouve)
	   {
	     if (cas == 1)
	       {
		 BIF_LABEL_USE(copie) = label_insection[2*(trouve-1)+1];
	       }
	     if (cas == 2)
	       {
		 if (BIF_LL3(copie))
		   {
		     NODE_LABEL(BIF_LL3(copie)) = label_insection[2*(trouve-1)+1];
		   }
	       }
             if (cas == 3)
	       {
		 if (BIF_LL1(copie))
		   {
		     NODE_LABEL(BIF_LL1(copie)) = label_insection[2*(trouve-1)+1];
		   }
	       }

	   } else
	     {
	       if (cas == 1)
		 BIF_LABEL_USE(copie) = lab; /* outside */
	       /* if ((cas == 2) no change */
	     }
       }
     temp = BIF_NEXT(temp);
   }

 
 /* on met a jour le blob list */  
 copie = alloue[1];
 for (temp = body, iii = 0; iii <lenght ; temp = BIF_NEXT(temp), iii++)
   {
     if (BIF_BLOB1(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB1(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList1(cherche, copie);
           }
       }
     if (BIF_BLOB2(temp))
       { /* on doit cree la blob liste */
         for (blobtemp = BIF_BLOB2(temp);blobtemp; 
              blobtemp = BLOB_NEXT(blobtemp)) 
           {
             /* on cherche la reference dans le tableaux allouer */
             cherche = NULL;
             for (i = 0; i <lenght ; i++)
               {
                 if (alloue[2*i] == BLOB_VALUE(blobtemp))
                     {
                       cherche = alloue[2*i+1];
                       break;
                     }
               }
             appendBfndToList2(cherche, copie);
           }
       }
     copie = BIF_NEXT(copie);
   }

 /* on remet ici a jour les CP */
 copie = alloue[1];
 for (temp = body, iii = 0; iii < lenght ; temp = BIF_NEXT(temp), iii++)
   {
     if (isItInSection(body, lastnode, BIF_CP(temp)))
       { /* on cherche le bif_cp pour la copie */
         cherche = NULL;
         for (i = 0; i <lenght ; i++)
           {
             if (alloue[2*i] == BIF_CP(temp))
               {
                 cherche = alloue[2*i+1];
                 break;
               }
           }
         BIF_CP(copie) = cherche;
       }
     copie = BIF_NEXT(copie);
   }
 copie = alloue[1];
#ifdef __SPF      
 removeFromCollection(alloue);
 removeFromCollection(label_insection);
#endif
 free(alloue);
 free(label_insection);
 return copie;
}
   
/***************************************************************************/
PTR_BFND getFunctionHeader(symb)
     PTR_SYMB symb;
{
  PTR_BFND thebif;
  if ((SYMB_CODE(symb) != FUNCTION_NAME) &&
      (SYMB_CODE(symb) != PROCESS_NAME) &&
      (SYMB_CODE(symb) != PROCEDURE_NAME) &&
      (SYMB_CODE(symb) != PROGRAM_NAME))
    return NULL;

/*  if (SYMB_FUNC_HEDR(symb))
    return SYMB_FUNC_HEDR(symb); */

  thebif = PROJ_FIRST_BIF();
  for (;thebif;thebif=BIF_NEXT(thebif)) 
    {
      if (((BIF_CODE(thebif) == FUNC_HEDR)  ||
           (BIF_CODE(thebif) == PROC_HEDR)  || 
           (BIF_CODE(thebif) == PROS_HEDR)  || 
           (BIF_CODE(thebif) == PROG_HEDR))
          &&
          (BIF_SYMB(thebif) == symb))
        return thebif;
    }
  return NULL;
}

/******* same as getFunctionHeader but looks in all the files *******/

PTR_BFND getFunctionHeaderAllFile(symb)
     PTR_SYMB symb;
{
  PTR_FILE ptf, saveptf;
  PTR_BLOB ptb;
  PTR_BFND tmp;

  if ((SYMB_CODE(symb) != FUNCTION_NAME) &&
      (SYMB_CODE(symb) != PROCESS_NAME) &&
      (SYMB_CODE(symb) != PROCEDURE_NAME) &&
      (SYMB_CODE(symb) != PROGRAM_NAME))

    return NULL;
  
  /* look in currentfile first*/
  tmp = getFunctionHeader(symb);
  if (tmp)
    {
      return tmp;
    }

  saveptf = pointer_on_file_proj;
  /* have to look in the bif list   return SYMB_FUNC_HEDR(symb); seens not to be set */
  
  for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);
      cur_file = ptf;      
      /* reset the toolbox and pointers*/
      Init_Tool_Box();
      tmp = getFunctionHeader(symb);
      if (tmp)
        {
          /* reset the pointers */
          cur_file = saveptf;
          Init_Tool_Box();
          return tmp;
        }
    }
  cur_file = saveptf;
  Init_Tool_Box();
  return NULL;
}
     
/***************************************************************************/
PTR_BFND getGlobalFunctionHeader(name)
     char *name;
{
  PTR_SYMB tsymb;
  PTR_FILE ptf;
  PTR_BLOB ptb;
  PTR_BFND tmp;
  

  for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);
      cur_file = ptf;      
      /* reset the toolbox and pointers*/
      Init_Tool_Box();
      
      for (tsymb = PROJ_FIRST_SYMB() ; tsymb; tsymb = SYMB_NEXT(tsymb))
        {
          if ( SYMB_IDENT(tsymb) && (strcmp (name, SYMB_IDENT(tsymb)) == 0)
              && ((SYMB_CODE(tsymb) == FUNCTION_NAME) ||
                  (SYMB_CODE(tsymb) == PROCESS_NAME) ||
                  (SYMB_CODE(tsymb) == PROCEDURE_NAME)))
            {
              tmp = getFunctionHeader(tsymb);
              if (tmp)
                return tmp;
            }
        }
    }
  return NULL;
}


/* return true if declare in the function or global */
/***************************************************************************/
int inScope(header, symb)
     PTR_BFND header;
     PTR_SYMB symb;
{
  PTR_BFND temp;

  if (!SYMB_SCOPE(symb))
     return FALSE;

  if (BIF_CODE(SYMB_SCOPE(symb)) == GLOBAL)
    return TRUE;

  temp = SYMB_SCOPE(symb);
  while (temp)
    {
      if (temp == header)
	return TRUE;
      temp = BIF_CP(temp);
    }
  return FALSE;
}

/* return return FUNC_HEDR or Global*/
/***************************************************************************/
PTR_BFND getFuncScope(header)
     PTR_BFND header;
{
  /*  PTR_BFND temp;*/ /*podd 15.03.99*/
  
  if (!header)
    return NULL;

  if (BIF_CODE(header) == GLOBAL)
    return header;
  if (BIF_CODE(header) == FUNC_HEDR)
    return header;
  return getFuncScope(BIF_CP(header));
}

/***************************************************************************/
int localToFunction(header, symb)
     PTR_BFND header;
     PTR_SYMB symb;
{
  PTR_BFND temp;

  if (!SYMB_SCOPE(symb))
     return FALSE;

  if (BIF_CODE(SYMB_SCOPE(symb)) == GLOBAL)
    return FALSE;

  temp = SYMB_SCOPE(symb);
  if (temp == header)
    return FALSE;    /* this a parameter */
  while (temp)
    {
      if (temp == header)
	return TRUE;
      temp = BIF_CP(temp);
    }
  return FALSE;
}

/***************************************************************************/
int isInStmt(stmt,pt)
     PTR_BFND stmt,pt;
{
  if (!stmt || !pt)
    return FALSE;

 if (BIF_CODE(stmt) == GLOBAL)
    return TRUE;
  
  if (BIF_CODE(pt) == GLOBAL)
    return FALSE;

  if (stmt == BIF_CP(pt))
    return TRUE;

  return isInStmt(stmt,BIF_CP(pt));
}

/***************************************************************************/
int exprListLength(first)
PTR_LLND first;
{
  PTR_LLND tail;
  int len = 0;
  if (first == NULL)
    return(0);
  for (tail = first; tail; tail = NODE_OPERAND1(tail) )
    len++;
  return(len);
}

/****************************************************************
 *								*
 *   Get_ll_with_id  -- find the low level node in the current	*
 *		   active file with given id number		*
 *								*
 *   Input:							*
 *	id -- the low level node id number			*
 *								*
 *   Output:							*
 *	the corresponding low level node pointer if found	*
 *	nil otherwise						*
 *								*
 ****************************************************************/
/***************************************************************************/
PTR_LLND Get_ll_with_id(id)
    int id;
{
    PTR_LLND ll;

    for (ll = PROJ_FIRST_LLND (); ll ; ll = NODE_NEXT (ll))
	if (NODE_ID (ll) == id)
	    return ll;
    return NULL;
}

/***************************************************************************/
PTR_BFND Get_bif_with_id(id)
     int id;
{
  PTR_BFND bif;
  
  for (bif = PROJ_FIRST_BIF(); bif ; bif = BIF_NEXT (bif))
    if (BIF_ID (bif) == id)
      return bif;
  return NULL;
}

/***************************************************************************/
PTR_SYMB Get_Symb_with_id(id)
     int id;
{
  PTR_SYMB symb;
  
  for (symb = PROJ_FIRST_SYMB (); symb ; symb = SYMB_NEXT (symb))
    if (SYMB_ID (symb) == id)
      return symb;
  return NULL;
}

/***************************************************************************/
PTR_TYPE Get_type_with_id(id)
     int id;
     
{
  PTR_TYPE type;
  
  for (type = PROJ_FIRST_TYPE (); type ; type = TYPE_NEXT (type))
    if (TYPE_ID (type) == id)
      return type;
  return NULL;
}

/***************************************************************************/
PTR_LABEL Get_label_with_id(id)
     int id;
{
  PTR_LABEL label;
  
  for (label = PROJ_FIRST_LABEL (); label ; label = LABEL_NEXT (label))
    if (LABEL_ID (label) == id)
      return label;
  return NULL;
}

/***************************************************************************/
PTR_CMNT Get_cmnt_with_id(id)
     int id;
{
  PTR_CMNT comment;
  
  for (comment = PROJ_FIRST_CMNT (); comment ; comment = CMNT_NEXT (comment))
    if (CMNT_ID (comment) == id)
      return comment;
  return NULL;
}
   
/***************************************************************************/
int getLastLabelId()
{
    int maxlabelname;
    PTR_LABEL lab;

    maxlabelname = 0;
    for (lab = PROJ_FIRST_LABEL(); lab; lab = LABEL_NEXT(lab))
    {
        if (maxlabelname < LABEL_STMTNO(lab))
        {
            maxlabelname = LABEL_STMTNO(lab);
        }
    }

    return maxlabelname;
}
   
 
/***************************************************************************/
PTR_LABEL getLastLabel()
{
  PTR_LABEL lab;
  
  for (lab = PROJ_FIRST_LABEL(); lab; lab = LABEL_NEXT(lab))
    {
      if  (!LABEL_NEXT(lab))
	return lab;
    }
  return NULL;
}
    
/***************************************************************************/
PTR_SYMB getSymbolWithName(name, scope)
     char *name;
     PTR_BFND scope;
{
  PTR_SYMB tsymb;
  PTR_BFND temp;
  
 for (tsymb = PROJ_FIRST_SYMB() ; tsymb; tsymb = SYMB_NEXT(tsymb))
   {
     if ( SYMB_IDENT(tsymb) && (strcmp (name, SYMB_IDENT(tsymb)) == 0))
       {
         /* we check the scope */
         temp = scope;         
         while (temp)
           {
             if (SYMB_SCOPE(tsymb) == temp)
                  return tsymb;
	     if (BIF_CODE(temp) != GLOBAL)
	       temp = BIF_CP(temp);
	     else
	       temp = NULL;
           }
       }
   }
  /* si non trouver on en recupere 1 */
  for (tsymb = PROJ_FIRST_SYMB() ; tsymb; tsymb = SYMB_NEXT(tsymb))
    {
       if ( SYMB_IDENT(tsymb) && (strcmp (name, SYMB_IDENT(tsymb)) == 0))
         {
           return tsymb;
         }
     }
  return NULL;
}


/***************************************************************************/
PTR_SYMB getSymbolWithNameInScope(name, scope)
     char *name;
     PTR_BFND scope;
{
  PTR_SYMB tsymb;
  PTR_BFND temp;
  
 for (tsymb = PROJ_FIRST_SYMB() ; tsymb; tsymb = SYMB_NEXT(tsymb))
   {
     if ( SYMB_IDENT(tsymb) && (strcmp (name, SYMB_IDENT(tsymb)) == 0))
       {
         /* we check the scope */
         if (scope)
           return tsymb;
         temp = scope;         
         while (temp)
           {
             if (SYMB_SCOPE(tsymb) == temp)
                  return tsymb;
	     if (BIF_CODE(temp) != GLOBAL)
	       temp = BIF_CP(temp);
	     else
	       temp = NULL;
           }
       }
   }
  return NULL;
}

/***************************************************************************/
int makeLinearExpr(exp,coef,symb,size, last)
     PTR_LLND exp;
     int *coef;
     PTR_SYMB *symb;
     int size;
     int *last;    
{
  return makeLinearExpr_Sign(exp,coef,symb,size, last,1,1);
}

/* initialy coeff are 0, return 1 if Ok, 0 if abort*/
/***************************************************************************/
int makeLinearExpr_Sign(exp,coef,symb,size, last,sign,factor)
     PTR_LLND exp;
     int *coef;
     PTR_SYMB *symb;
     int size;
     int *last;  
     int sign;     
     int factor;
{
  int code;
  int i, *res1,*res2;
  
  if (!exp)
    return TRUE;
  
  code = NODE_CODE(exp);
  switch (code)
    {
    case VAR_REF:
      for (i=0; i< size; i++)
        {
          if (NODE_SYMB(exp) == symb[i])
            {
              coef[i] = coef[i] + sign*factor;
              break;
            }
        }
      break;
    case SUBT_OP:
      makeLinearExpr_Sign(NODE_OPERAND0(exp),coef,symb,size,last,sign,factor);
      makeLinearExpr_Sign(NODE_OPERAND1(exp),coef,symb,size,last,-1*sign,factor);
      break;
    case ADD_OP:
      makeLinearExpr_Sign(NODE_OPERAND0(exp),coef,symb,size,last,sign,factor);
      makeLinearExpr_Sign(NODE_OPERAND1(exp),coef,symb,size,last,sign,factor);
      break;
    case MULT_OP:
      res1 = evaluateExpression (NODE_OPERAND0(exp));
      res2 = evaluateExpression (NODE_OPERAND1(exp));
      if ((res1[0] != -1) && (res2[0] != -1))
        {
          *last = *last + factor*sign*(res1[1]*res2[1]);
        } else
          {
            if (res1[0] != -1) 
              {
                /* la constante est le fils gauche */
                if (NODE_CODE(NODE_OPERAND1(exp)) != VAR_REF)
                  return  makeLinearExpr_Sign(NODE_OPERAND1(exp),coef,symb,size, last,sign,res1[1]*factor);
                for (i=0; i< size; i++)
                  {
                    if (NODE_SYMB(NODE_OPERAND1(exp)) == symb[i])
                      {
                        coef[i] = coef[i] + factor*sign*(res1[1]);
                        break;
                      }
                  }
              } else
                if (res2[0] != -1) 
                  {
                    /* la constante est le fils droit */
                    if (NODE_CODE(NODE_OPERAND0(exp)) != VAR_REF)
                      return  makeLinearExpr_Sign(NODE_OPERAND0(exp),coef,symb,size, last,sign,res2[1]*factor);

                    for (i=0; i< size; i++)
                      {
                        if (NODE_SYMB(NODE_OPERAND0(exp)) == symb[i])
                          {
                            coef[i] = coef[i] + factor*sign*(res2[1]);
                            break;
                          }
                      }
                  } else                    
                    return FALSE;
          }
      break;
    case INT_VAL:
      *last = *last + factor*sign*(NODE_INT_CST_LOW(exp));
      break;
    default:
      
      return FALSE;
    }
  return TRUE;
}


/***************************************************************************/
char* Get_Function_Name_For_Call(pt)
PTR_LLND pt;
{
  if (!pt)
    return NULL;

  if (NODE_CODE(pt) != FUNC_CALL)
    return NULL;
  if (NODE_SYMB(pt))
    return SYMB_IDENT(NODE_SYMB(pt));
  else
    return NULL;
}

/***************************************************************************/
PTR_BFND Get_Last_Node_Of_Project()
{
  PTR_BFND temp;

  for (temp = PROJ_FIRST_BIF();  temp;  temp = BIF_NEXT(temp))
    {
      if (!BIF_NEXT(temp))
	return temp;
    }
  return NULL;
}

/***************************************************************************/
PTR_LLND Follow_Llnd(ll,c)
PTR_LLND ll;
int c;
{
  PTR_LLND pt;
  if(c == 2)
    {
      pt = ll;
      while(pt)
	{
	  if (NODE_OPERAND1(pt) == NULL)
	    return pt;
	  pt = NODE_OPERAND1(pt);	  
	}
    } else
      {
        pt = ll;
        while(pt)
          {
            if (NODE_OPERAND1(pt) == NULL)
              return NODE_OPERAND0(pt);
            pt = NODE_OPERAND1(pt);	  
          }
      }
  return NULL;
}

/***************************************************************************/
PTR_LLND Follow_Llnd0(ll)
PTR_LLND ll;
{
  PTR_LLND pt;
  pt = ll;
  while(pt)
    {
      if (NODE_OPERAND0(pt) == NULL)
	return pt;
      pt = NODE_OPERAND0(pt);	  
    }
  return NULL;
}

/***************************************************************************/
PTR_LLND Get_Th_Parameter_For_Call(ptin,n)
     PTR_LLND ptin;
     int n;
{
  int i;
  PTR_LLND pt;
  pt = ptin;
  
  if (!pt)
    return NULL;

  if (NODE_CODE(pt) != FUNC_CALL)
    return NULL;

  if (!NODE_OPERAND0(pt))
    return NULL;

  pt = NODE_OPERAND0(pt); 
  
  if (!NODE_OPERAND0(pt))
    return NULL; /* the node is an expression list with only 
                    one node, an expression list */
  pt = NODE_OPERAND0(pt); 
  for (i = 0; (i < n) && pt; i++)
    {
      pt = NODE_OPERAND1 (pt);
    }
  
  if (pt)
    return NODE_OPERAND0(pt);
  else
     return NULL;
}

/* old function kept for the annotation part */
/***************************************************************************/
PTR_LLND Get_First_Parameter_For_Call(pt)
PTR_LLND pt;
{
  if (!pt)
    return NULL;

  if (NODE_CODE(pt) != FUNC_CALL)
    return NULL;

  if (!NODE_OPERAND0(pt))
    return NULL;
  pt = NODE_OPERAND0 (NODE_OPERAND0(pt));
  return pt;

}

/***************************************************************************/
PTR_LLND Get_Second_Parameter_For_Call(pt)
PTR_LLND pt;
{
  if (!pt)
    return NULL;

  if (NODE_CODE(pt) != FUNC_CALL)
    return NULL;

  if (!NODE_OPERAND0(pt))
    return NULL;

  pt = NODE_OPERAND1(NODE_OPERAND0(pt));
  if (!pt)
    return NULL;
  else
    return NODE_OPERAND0(pt);

}

/***************************************************************************/
int Is_String_Val_With_Val(expr,str)
     PTR_LLND expr;
     char *str;
{
  if (!str)
    return FALSE;
  if (!expr)
    return FALSE;

  if (NODE_CODE(expr) != STRING_VAL)
    return FALSE;

  if (strcmp(NODE_STRING_POINTER(expr),str) == 0)
    return TRUE;

    return FALSE;

}

/***************************************************************************/
void Replace_String_In_Expression(exprold, str, exprnew)
     char *str;
     PTR_LLND exprold, exprnew;
{
  if (!exprold || !exprnew)
    return ;

  if (Is_String_Val_With_Val(NODE_OPERAND0(exprold),str))
    NODE_OPERAND0(exprold) = exprnew;
  else
    Replace_String_In_Expression(NODE_OPERAND0(exprold), str, exprnew);
  
      
  if (Is_String_Val_With_Val(NODE_OPERAND1(exprold),str))
    NODE_OPERAND1(exprold) = exprnew;
  else
    Replace_String_In_Expression(NODE_OPERAND1(exprold), str, exprnew);
  
}

/* Originally coded by fbodin, but the code used K&R varargs conventions,
   I have rewritten the code to use ANSI conventions (phb) */
/***************************************************************************/
PTR_LLND Make_Function_Call(PTR_SYMB symb, PTR_TYPE type, int taille, ...)
{
  va_list p;
  PTR_LLND func_call, temp, op, last;
  int i; /* nb parametres*/  /* FRENCH! */
  
  /* Set pointer p to the very first variable argument in list */
  va_start(p,taille);

  func_call = newExpr(FUNC_CALL,type,NULL);
  NODE_SYMB(func_call) = symb;

  last = NULL;
  temp = NULL;
  
  /* Loop through remaining arguments, extracting them */
  for (i=1;i<=taille;i++)
    {
      /* Extract 3+ith argument (type PTR_LLND), inc arg pointer p */
      op = va_arg(p,PTR_LLND);
      temp = newExpr(EXPR_LIST,NULL,op,NULL);
      if (!last) 
        NODE_OPERAND0(func_call) = temp;
      else
        NODE_OPERAND1(last) = temp;
      last  = temp;      
    }
  va_end(p);
  return func_call;
}

/*******************************MISCELLANEOUS*******************************/
char* Remove_Carriage_Return(str)
char *str;
{
  int i =0;

  if (str == NULL)
    return NULL;

  while (str[i] != '\0')
    {
      if (str[i] == '\n')
	str[i] = ' ';
      i++;
    }
  return str;
}



/***************************************************************************/
int Apply_To_Bif(bif, f)
     PTR_BFND bif;
     int (*f)();
{
  PTR_BLOB blob;
  int resul;
  
  if (!bif)
    return FALSE;
  
  resul = f(bif);  

  for (blob = BIF_BLOB1 (bif); blob; blob = BLOB_NEXT (blob))
    {
      resul = resul |  Apply_To_Bif (BLOB_VALUE (blob), f);
    }
  
  for (blob = BIF_BLOB2 (bif); blob; blob = BLOB_NEXT (blob))
    {
      resul =  resul | Apply_To_Bif (BLOB_VALUE (blob), f);
    }
  return resul;
  
}

/* reset to null the bif_next */
/***************************************************************************/
void Reset_Bif_Next_Chain(start)
     PTR_BFND start;
{
  PTR_BLOB blob;

  if (!start)
    return ;
    
  BIF_NEXT(start) = NULL;

  for (blob = BIF_BLOB1(start); blob ; blob = BLOB_NEXT(blob))
    {
      Reset_Bif_Next_Chain(BLOB_VALUE(blob));
    }
  for (blob = BIF_BLOB2(start); blob ; blob = BLOB_NEXT(blob))
    {
       Reset_Bif_Next_Chain(BLOB_VALUE(blob));
    }
}

/***************************************************************************/
void Count_Bif_Next_Chain(start)
     PTR_BFND start;
{
  PTR_BLOB blob;

  if (!start)
    return;
    
  if (BIF_NEXT(start) == NULL)
    {
      CountNullBifNext++;
/*      Message("Bif_Next is Null", BIF_LINE(start));*/
      
    }

  for (blob = BIF_BLOB1(start); blob ; blob = BLOB_NEXT(blob))
    {
      Count_Bif_Next_Chain(BLOB_VALUE(blob));
    }
  for (blob = BIF_BLOB2(start); blob ; blob = BLOB_NEXT(blob))
    {
       Count_Bif_Next_Chain(BLOB_VALUE(blob));
    }
}

/* Redo the BIF_NEXT Field for the DATA base so it is correct in the data 
   return the last also reset the BIF_ID to be added to low level and call
   in init toolbax */
static int idfirst = 1;
/***************************************************************************/
PTR_BFND Redo_Bif_Next_Chain_Internal(start)
     PTR_BFND start;
{
    PTR_BLOB blob;
    PTR_BFND last;

    if (!start)
        return NULL;

//  BIF_ID(start) = idfirst;   /*podd 29.06.20 */
//  idfirst++;

    if (BIF_BLOB1(start))
    {
        BIF_NEXT(start) = BLOB_VALUE(BIF_BLOB1(start));
    }
    last = start;
    for (blob = BIF_BLOB1(start); blob; blob = BLOB_NEXT(blob))
    {
        last = Redo_Bif_Next_Chain_Internal(BLOB_VALUE(blob));
        if (BLOB_NEXT(blob))
            BIF_NEXT(last) = BLOB_VALUE(BLOB_NEXT(blob));

        if (!last)
            last = BLOB_VALUE(blob);
    }

    if (last)
    {
        if (BIF_BLOB2(start))
            BIF_NEXT(last) = BLOB_VALUE(BIF_BLOB2(start));
    }
    for (blob = BIF_BLOB2(start); blob; blob = BLOB_NEXT(blob))
    {
        last = Redo_Bif_Next_Chain_Internal(BLOB_VALUE(blob));
        if (BLOB_NEXT(blob))
            BIF_NEXT(last) = BLOB_VALUE(BLOB_NEXT(blob));
        if (!last)
            last = BLOB_VALUE(blob);
    }
    return last;
}


/***************************************************************************/
PTR_BFND LocalRedoBifNextChain(start)
     PTR_BFND start;
{
  PTR_BLOB blob;
  PTR_BFND last;

  if (!start)
    return NULL;  
  if (BIF_BLOB1(start))
    {
      BIF_NEXT(start) = BLOB_VALUE(BIF_BLOB1(start));
    }
  last = start;
  for (blob = BIF_BLOB1(start); blob ; blob = BLOB_NEXT(blob))
    {
      last = LocalRedoBifNextChain(BLOB_VALUE(blob));
      if (BLOB_NEXT(blob))
        BIF_NEXT(last) = BLOB_VALUE(BLOB_NEXT(blob));
      
      if (!last)
        last = BLOB_VALUE(blob);
    }

  if (last)
    {
      if (BIF_BLOB2(start))
        BIF_NEXT(last) = BLOB_VALUE(BIF_BLOB2(start));
    }
  for (blob = BIF_BLOB2(start); blob ; blob = BLOB_NEXT(blob))
    {
      last = LocalRedoBifNextChain(BLOB_VALUE(blob));
      if (BLOB_NEXT(blob))
        BIF_NEXT(last) = BLOB_VALUE(BLOB_NEXT(blob));
      if (!last)
        last = BLOB_VALUE(blob);
    }
  return last;
}

/***************************************************************************/
void Redo_Bif_Next_Chain(start)
     PTR_BFND start;
{
  int i;
  PTR_BFND thebif;
  Reset_Bif_Next_Chain(start);
  Redo_Bif_Next_Chain_Internal(start);
  Count_Bif_Next_Chain(start);
  thebif = PROJ_FIRST_BIF();
  i = 1;
  for (;thebif;thebif=BIF_NEXT(thebif), i++) 
    BIF_ID(thebif) = i;        
  CUR_FILE_NUM_BIFS() = i-1;
}

/****************************** Added by D.Windheiser ************************/
/* find the bif node corresponding to a given line number
   Note that if line is a continuation line, there is no bif node 
   with this line number
   So the corresponding bif node is the bif node with the
   largest line number lower or equal to the line num 
*/

/****************************************************************
 *								*
 *   FindNearBifNode -- find the corresponding BIF node given a	*
 *		    filename and line number			*
 *								*
 *   Input:							*
 *	    filename - name of the file to be looked upon	*
 *	    line     - line number to be checked		*
 *								*
 *   Output:							*
 *	    A bif pointer (PTR_BFND) points to the bif node	*
 *	    corresponds to the given line number		*
*	    NULL if error occured				*
 *								*
 ****************************************************************/

/***************************************************************************/

/***************************************************************************/
PTR_BFND FindNearBifNode(filename, line)
	char	*filename;
	int	 line;
	
{
  PTR_FILE ptf;
  PTR_BLOB ptb;
  

  for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);
      cur_file = ptf;      
      /* reset the toolbox and pointers*/
      Reset_Tool_Box();
      
      if (strcmp(Current_File_name,filename) == 0)
        { /* now look for the line number */
          return rec_num_near_search(line);
        }
    }
  fprintf(stderr,"%s",filename);
  Message("No such file in this project",0);
  return NULL;
}

/****************************************************************
 *								*
 *  rec_num_near_search --                                      *
 *                    search for the bif node that	        *
 *		      corresponds to the num'th line in the	*
 *		      current file           			*
 *								*
 *  Inputs:							*
 *         - line number of current file                        *
 *								*
 *  Output:							*
 *	The bif node pointer if one exists for the given line	*
 *	in the given file					*
 *								*
 ****************************************************************/
/***************************************************************************/
PTR_BFND rec_num_near_search(num)
     int	 num;
{
  PTR_BFND temp,last = NULL;
  
  for (temp = PROJ_FIRST_BIF (); temp ; temp = BIF_NEXT(temp))
    {
      if (BIF_LINE(temp) == num)
        return temp;
      if (BIF_LINE(temp) > num)
        return last;
      last =temp;
    }
  return(NULL);
}



/********* Add a comment to a node *************************************/


/***************************************************************************/
void LibAddComment(PTR_BFND bif, char *str)
{
    char *pt;
    PTR_CMNT cmnt;

    if (!bif || !str)
        return;

    if (!BIF_CMNT(bif))
    {
        pt = (char *)xmalloc(strlen(str) + 1);
        cmnt = (PTR_CMNT)newNode(CMNT_KIND);
        strcpy(pt, str);
        CMNT_STRING(cmnt) = pt;
        BIF_CMNT(bif) = cmnt;
    }
    else
    {
        cmnt = BIF_CMNT(bif);
        if (CMNT_STRING(cmnt))
        {
            pt = (char *)xmalloc(strlen(str) + strlen(CMNT_STRING(cmnt)) + 1);
            sprintf(pt, "%s%s", CMNT_STRING(cmnt), str);
            CMNT_STRING(cmnt) = pt;
        }
        else
        {
            pt = (char *)xmalloc(strlen(str) + 1);
            sprintf(pt, "%s", str);
            CMNT_STRING(cmnt) = pt;
        }
    }
}


/* ajm */
/********************** Set a node's comment *******************************/
//Kolganov 15.11.2017
void LibDelAllComments(PTR_BFND bif)
{
    PTR_CMNT cmnt;
    char *pt;

    if (!bif)
        return;

    if (BIF_CMNT(bif))
    {
        if (CMNT_STRING(BIF_CMNT(bif)))
        {
#ifdef __SPF      
            removeFromCollection(CMNT_STRING(BIF_CMNT(bif)));
#endif
            free(CMNT_STRING(BIF_CMNT(bif)));
            CMNT_STRING(BIF_CMNT(bif)) = NULL;
        }

        cmnt = BIF_CMNT(bif);
        // remove comment from list before free
        if (cmnt == PROJ_FIRST_CMNT())
        {
            if (cmnt->thread)
                PROJ_FIRST_CMNT() = cmnt->thread;
            else
                PROJ_FIRST_CMNT() = NULL;
        }
        else
        {
            PTR_CMNT before = PROJ_FIRST_CMNT();
            while (before->thread)
            {
                if (before->thread == cmnt)
                {
                    if (cmnt->thread)
                    {
                        before->thread = cmnt->thread;
                        cmnt->thread = NULL;
                    }
                    else
                        before->thread = NULL;
                    break;
                }
                before = before->thread;
            }
        }
        /*
#ifdef __SPF      
        removeFromCollection(BIF_CMNT(bif));
#endif
        free(BIF_CMNT(bif));*/
        BIF_CMNT(bif) = NULL;
    }
}

void LibSetAllComments(PTR_BFND bif, char *str)
{
     PTR_CMNT cmnt;
     char *pt;

     if ( !bif || !str )
	  return;
     
     LibDelAllComments(bif);

     pt = (char *) xmalloc(strlen(str) + 1);
     cmnt = (PTR_CMNT) newNode(CMNT_KIND);
     strcpy(pt, str);
     CMNT_STRING(cmnt) = pt;
     BIF_CMNT(bif) = cmnt;
}

/***************************************************************************/
int patternMatchExpression(ll1,ll2)
     PTR_LLND ll1,ll2;
{
 /*  char *string1, *string2;*/ /*podd 15.03.99*/
  int *res1, *res2;
  
  if (ll1 == ll2)
    return TRUE;
  
 if (!ll1 || !ll2)
   return FALSE;

  if (NODE_CODE(ll1) != NODE_CODE(ll2))
    return FALSE;

  /* because of identical names does not work also no commutativity
     string1 = funparse_llnd(ll1);
     string2 = funparse_llnd(ll2);
     if (strcmp(string1, string2) == 0)
     return TRUE;
    */
  /* first test if constant equations identical */
  res1 = evaluateExpression(ll1);
  res2 = evaluateExpression(ll2);
  if ((res1[0] != -1) &&
      (res2[0] != -1) &&
      (res1[1] == res2[1]))
    {
#ifdef __SPF      
      removeFromCollection(res1);
      removeFromCollection(res2);
#endif
      free(res1);
      free(res2);
      return TRUE;
    }
  if ((res1[0] != -1) && (res2[0] == -1))
    {
#ifdef __SPF      
      removeFromCollection(res1);
      removeFromCollection(res2);
#endif
      free(res1);
      free(res2);
      return FALSE;
    }
  if ((res1[0] == -1) && (res2[0] != -1))
    {
#ifdef __SPF      
      removeFromCollection(res1);
      removeFromCollection(res2);
#endif
      free(res1);
      free(res2);
      return FALSE;
    }
#ifdef __SPF      
  removeFromCollection(res1);
  removeFromCollection(res2);
#endif
  free(res1);
  free(res2);

  /* for each  kind of node do the pattern match */
  switch (NODE_CODE(ll1))
    {
    case VAR_REF:
      if (NODE_SYMB(ll1) == NODE_SYMB(ll2))
        return TRUE;
      break;

      /* commutatif operator */
    case EQ_OP:
      if ((NODE_SYMB(ll1) == NODE_SYMB(ll2)) &&
          patternMatchExpression(NODE_OPERAND0(ll1),
                                   NODE_OPERAND1(ll2)) &&
          patternMatchExpression(NODE_OPERAND0(ll1),
                                   NODE_OPERAND1(ll2))) 
        return TRUE;
    default :
      if ((NODE_SYMB(ll1) == NODE_SYMB(ll2)) &&
          patternMatchExpression(NODE_OPERAND0(ll1),
                                    NODE_OPERAND0(ll2)) &&
          patternMatchExpression(NODE_OPERAND1(ll1),
                                   NODE_OPERAND1(ll2))) 
        return TRUE;
    }
  return FALSE;
}


/* 
  new functions added, they have a match with the one in the C++
  interface library
*/
/***************************************************************************/
void SetCurrentFileTo(file)
     PTR_FILE file;
{
  if (!file)
    return;
  if (pointer_on_file_proj == file)
    return;
  cur_file  = file;
  /* reset the toolbox and pointers*/
  Init_Tool_Box();
}


/***************************************************************************/
int LibnumberOfFiles()
{
  PTR_BLOB ptb;
  int count = 0;
  if (cur_proj)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  count++;
	}
    } else
      if(pointer_on_file_proj)
	return 1;
  return count;
}

/***************************************************************************/
PTR_FILE GetPointerOnFile(dep_file_name)
     char *dep_file_name;
{
/*  PTR_FILE pt;*/ /*podd 15.03.99*/
  PTR_BLOB ptb;
  if (cur_proj && dep_file_name)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  cur_file  = (PTR_FILE) BLOB_VALUE (ptb);
	  /* reset the toolbox and pointers*/
	  SetCurrentFileTo(cur_file);
	  if (CUR_FILE_NAME() && !strcmp(CUR_FILE_NAME(),dep_file_name))
	    return  pointer_on_file_proj;
	}
    }
  return NULL;
}

/***************************************************************************/
int GetFileNum(dep_file_name)
     char *dep_file_name;
{
  PTR_FILE pt;
  PTR_BLOB ptb;
  int count= 0;
  if (cur_proj && dep_file_name)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  count++;
	  pt = (PTR_FILE) BLOB_VALUE (ptb);
	  /* reset the toolbox and pointers*/
	  SetCurrentFileTo(pt);
	  if (FILE_FILENAME(pt) && !strcmp(FILE_FILENAME(pt),dep_file_name))
	    return  count;
	}
    }
  return 0;
}


/***************************************************************************/
int GetFileNumWithPt(dep_file)
     PTR_FILE dep_file;
{
  PTR_FILE pt;
  PTR_BLOB ptb;
  int count= 0;
  if (cur_proj && dep_file)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  count++;
	  pt = (PTR_FILE) BLOB_VALUE (ptb);
	  /* reset the toolbox and pointers*/
	  SetCurrentFileTo(pt);
	  if (pt==dep_file)
	    return  count;
	}
    }
  return 0;
}


/***************************************************************************/
PTR_FILE GetFileWithNum(num)
     int num;
{
  PTR_FILE pt;
  PTR_BLOB ptb;
  int count= 0;
  if (cur_proj)
    {
      for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
	{
	  pt = (PTR_FILE) BLOB_VALUE (ptb);
	  /* reset the toolbox and pointers*/
	   SetCurrentFileTo(pt);
	  if (count == num)
	    return pt;
	  count++;
	}
    }
  return NULL;
}

/***************************************************************************/
void LibsaveDepFile(str)
     char *str;
{
  PTR_BFND thebif;
  int i;
  if (!str)
    {
      Message("No name specified in saveDepFile",0);
      return;
    }
  thebif = PROJ_FIRST_BIF();
  i = 1;
  for (;thebif;thebif=BIF_NEXT(thebif), i++) 
       BIF_ID(thebif) = i;     
   
  CUR_FILE_NUM_BIFS() = i-1;

  if (write_nodes(cur_file,str) < 0)
      Message("Error, write_nodes() failed (001)",0);

}

/***************************************************************************/
int getNumberOfFunction()
{
    PTR_BFND thebif;
    int count = 0;

    thebif = PROJ_FIRST_BIF();
    for (; thebif; thebif = BIF_NEXT(thebif))
    {
        if ((BIF_CODE(thebif) == FUNC_HEDR) || (BIF_CODE(thebif) == PROC_HEDR) ||
            (BIF_CODE(thebif) == PROS_HEDR) || (BIF_CODE(thebif) == PROG_HEDR))
        {
            if (thebif->control_parent->variant != INTERFACE_STMT && 
                thebif->control_parent->variant != INTERFACE_OPERATOR &&
                thebif->control_parent->variant != INTERFACE_ASSIGNMENT)
                count++;
        }        
    }
    return count;
}
  
/***************************************************************************/
PTR_BFND getFunctionNumHeader(int num)
{
    PTR_BFND thebif;
    int count = 0;

    thebif = PROJ_FIRST_BIF();
    for (; thebif; thebif = BIF_NEXT(thebif))
    {
        if ((BIF_CODE(thebif) == FUNC_HEDR) || (BIF_CODE(thebif) == PROC_HEDR) ||
            (BIF_CODE(thebif) == PROS_HEDR) || (BIF_CODE(thebif) == PROG_HEDR))
        {
            if (thebif->control_parent->variant != INTERFACE_STMT &&
                thebif->control_parent->variant != INTERFACE_OPERATOR &&
                thebif->control_parent->variant != INTERFACE_ASSIGNMENT)
            {
                if (count == num)
                    return thebif;
                count++;
            }
        }        
    }
    return NULL;
}
  
/***************************************************************************/
int getNumberOfStruct()
{
  PTR_BFND thebif;
  int count  =0;

  thebif = PROJ_FIRST_BIF();
  for (;thebif;thebif=BIF_NEXT(thebif)) 
    {
      if (isAStructDeclBif(BIF_CODE(thebif)))
	count++;
    }

  return count;
}

/***************************************************************************/
PTR_BFND getStructNumHeader(num)
     int num;
{
  PTR_BFND thebif;
  int count  =0;

  thebif = PROJ_FIRST_BIF();
  for (;thebif;thebif=BIF_NEXT(thebif)) 
    {
      if (isAStructDeclBif(BIF_CODE(thebif)))
	{
	  if (count == num)
	    return thebif;
	  count++;
	}
    }
  return NULL;
}

/***************************************************************************/
PTR_BFND getFirstStmt()
{
  return PROJ_FIRST_BIF();
}

/***************************************************************************/
PTR_TYPE GetAtomicType(tt)
     int tt;
{
  PTR_TYPE ttype = NULL;

  if(!isAtomicType(tt))
    {
      Message("Misuse of GetAtomicType",0);
      return NULL;
    }
  for (ttype = PROJ_FIRST_TYPE () ; ttype; ttype = TYPE_NEXT(ttype))
    {
      if (TYPE_CODE(ttype) == tt)
	return ttype;
    }
  return (ttype);
}

/***************************************************************************/
PTR_BFND LiblastDeclaration(start)
PTR_BFND start;
{
  PTR_BFND temp;
  
  if (start)
    temp = start;
  else
    temp = PROJ_FIRST_BIF ();
  for ( ; temp; temp = BIF_NEXT(temp))
    {
      if ( BIF_NEXT(temp) && !isADeclBif(BIF_CODE(BIF_NEXT(temp))))
	return temp;
    }
  Message("LiblastDeclaration return NULL",0);
  return NULL;  
}

/***************************************************************************/
int LibIsSymbolInScope(bif,symb)
     PTR_BFND bif;
     PTR_SYMB symb;
{
  PTR_BFND scope;
  
  if (!symb || !bif)
    return FALSE;
  scope = SYMB_SCOPE(symb);
/*  return isItInSection(BIF_CP(bif), getLastNodeOfStmt(BIF_CP(bif)), scope);*/
  if (scope)
/* assume scope is the declaration of the variable, otherwise to be removed*/
    return isItInSection(BIF_CP(scope), getLastNodeOfStmt(BIF_CP(scope)), bif);
  else
    return FALSE;
}

/***************************************************************************/
int IsRefToSymb(expr,symb)
     PTR_LLND expr;
     PTR_SYMB symb;
{
  
  if (!expr)
    return FALSE;
  
  if (!hasNodeASymb(NODE_CODE(expr)))
    return FALSE;
  
  if (NODE_SYMB(expr) != symb)
    return FALSE;
  return TRUE;
}

/***************************************************************************/
void LibreplaceSymbByExp(exprold, symb, exprnew)
     PTR_SYMB symb;
     PTR_LLND exprold, exprnew;
{
  if (!exprold)
    return ;

  if (IsRefToSymb(NODE_OPERAND0(exprold),symb))
    NODE_OPERAND0(exprold) = exprnew;
  else
    LibreplaceSymbByExp(NODE_OPERAND0(exprold), symb, exprnew);
        
  if (IsRefToSymb(NODE_OPERAND1(exprold),symb))
    NODE_OPERAND1(exprold) = exprnew;
  else
    LibreplaceSymbByExp(NODE_OPERAND1(exprold), symb, exprnew);
}

/***************************************************************************/
void LibreplaceSymbByExpInStmts(debut, fin, symb, expr)
     PTR_BFND debut, fin;
     PTR_SYMB symb;
     PTR_LLND expr;
{
  PTR_BFND temp;
  
  for (temp = debut; temp ; temp = BIF_NEXT(temp))
    {
      if (IsRefToSymb(BIF_LL1(temp),symb))
        BIF_LL1(temp) = expr;
      else
        LibreplaceSymbByExp(BIF_LL1(temp), symb, expr);

      if (IsRefToSymb(BIF_LL2(temp),symb))
        BIF_LL2(temp) = expr;
      else
        LibreplaceSymbByExp(BIF_LL2(temp), symb, expr);

      if (IsRefToSymb(BIF_LL3(temp),symb))
        BIF_LL3(temp) = expr;
      else
        LibreplaceSymbByExp(BIF_LL3(temp), symb, expr);
      if (fin && (temp == fin))
        break;
    }  
}
   
/***************************************************************************/
PTR_LLND LibIsSymbolInExpression(exprold, symb)
     PTR_SYMB symb;
     PTR_LLND exprold;
{
  PTR_LLND pt =NULL;
  if (!exprold)
    return NULL;

  if (IsRefToSymb(NODE_OPERAND0(exprold),symb))
    return NODE_OPERAND0(exprold);
  else
    pt = LibIsSymbolInExpression(NODE_OPERAND0(exprold), symb);
  if (pt)
    return pt;

  if (IsRefToSymb(NODE_OPERAND1(exprold),symb))
    return NODE_OPERAND1(exprold) ;
  else
    pt = LibIsSymbolInExpression(NODE_OPERAND1(exprold), symb);

  return pt;
}

/***************************************************************************/
PTR_BFND LibWhereIsSymbDeclare(symb)
      PTR_SYMB symb;
{
  PTR_BFND scopeof, temp, last;
  if (!symb)
    return NULL;
  
  scopeof = SYMB_SCOPE(symb);
  if (!scopeof)
     return NULL;
  
  last = getLastNodeOfStmt(scopeof);
  
  for (temp = scopeof; temp ; temp=BIF_NEXT(temp))
    {
#if __SPF
      //SKIP SPF dirs
      //for details see dvm_tag.h
      if (scopeof->variant >= 950 && scopeof->variant <= 958)
          continue;
#endif
      if (LibIsSymbolInExpression(BIF_LL1(temp), symb))
        return temp;
      if (LibIsSymbolInExpression(BIF_LL2(temp), symb))
        return temp;
      if (temp == last)
        break;
    }
  return NULL;
}



/* return a symbol in a declaration list 
 replace find_suit_declarator() but also more ... 
 replace also find_parameter_name()
*/
/***************************************************************************/
PTR_LLND giveLlSymbInDeclList(expr)
PTR_LLND expr;
{
  PTR_LLND list1, list2;
  if (!expr)
    return NULL;
  
  if (NODE_CODE(expr) == EXPR_LIST)
    {
      for (list1= expr; list1; list1 = NODE_OPERAND1(list1))
	{
	  if (NODE_OPERAND0(list1))
	    {
	      for (list2= NODE_OPERAND0(list1); list2; )
		{
		  if (hasNodeASymb(NODE_CODE(list2)))
		    {
		      if (NODE_SYMB(list2))
			return list2;
		    }
		if(NODE_CODE(list2) == SCOPE_OP) list2 = NODE_OPERAND1(list2);
		else list2 = NODE_OPERAND0(list2);
		}
	    }
	}
    } else
      {
	for (list2= expr; list2; )
	  {
	    if (hasNodeASymb(NODE_CODE(list2)))
	      {
		if (NODE_SYMB(list2))
		  return list2;
	      }
	      if(NODE_CODE(list2) == SCOPE_OP) list2 = NODE_OPERAND1(list2);
	     else list2 = NODE_OPERAND0(list2);
	  }
      }
/*  Message("giveSymbInDeclList did not find the symbol (crash will happen)",0); */
  return NULL;
}

/* return the first non null type in the base type list */
/***************************************************************************/
PTR_TYPE lookForInternalBasetype(type)
     PTR_TYPE type;
{
  if (!type)
    return NULL;

  if (TYPE_CODE(type) == T_MEMBER_POINTER){
	if (TYPE_COLL_BASE(type))
	   return lookForInternalBasetype(TYPE_COLL_BASE(type));
        else
           return type;
	}
  else if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
	return lookForInternalBasetype(TYPE_BASE(type));
      else
	return type;
    }
  else
    return type;
}


/* return the first non null type in the base type list */
/***************************************************************************/
PTR_TYPE lookForTypeDescript(type)
     PTR_TYPE type;
{
  if (!type)
    return NULL;

  if (TYPE_CODE(type) == T_DESCRIPT)
    return type;
  if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
	return lookForTypeDescript(TYPE_BASE(type));
      else
	return NULL;
    }
  else
    return  NULL;
}

/***************************************************************************/
int getTypeNumDimension(type)
     PTR_TYPE type;
{
  if (!type)
    return 0;
  return exprListLength(TYPE_DECL_RANGES(type));
}

/***************************************************************************/
int isElementType(type)
PTR_TYPE type;
{
  if (!type)
    return 0;

  if (TYPE_CODE(type) == T_DERIVED_TYPE)
    {
      if (TYPE_SYMB_DERIVE(type) &&
	  SYMB_IDENT(TYPE_SYMB_DERIVE(type)) &&
	  (strcmp(SYMB_IDENT(TYPE_SYMB_DERIVE(type)), "ElementType") == 0))
	return 1;
    }
  return 0;
}

/***************************************************************************/
PTR_TYPE getDerivedTypeWithName(str)
     char *str;
{
  PTR_TYPE ttype = NULL;
  for (ttype = PROJ_FIRST_TYPE () ; ttype; ttype = TYPE_NEXT(ttype))
    {
      if (TYPE_CODE(ttype) == T_DERIVED_TYPE)
        {
          if (TYPE_SYMB_DERIVE(ttype) &&
              SYMB_IDENT(TYPE_SYMB_DERIVE(ttype)) &&
              (strcmp(SYMB_IDENT(TYPE_SYMB_DERIVE(ttype)), str) == 0))
            return ttype;
        }
    }
  return (ttype);
}


/***************************************************************************/
int sameName(symb1,symb2)
 PTR_SYMB symb1,symb2;
{
  if (!symb1 || !symb2)
    return FALSE;

  if (!SYMB_IDENT(symb1) || !SYMB_IDENT(symb2))
    return FALSE;
  
  if (strcmp(SYMB_IDENT(symb1),SYMB_IDENT(symb2)) == 0)
    return TRUE;
  else
    return FALSE;
}


/***************************************************************************/
PTR_SYMB lookForNameInParamList(functor,name)
PTR_SYMB functor;
char *name;
{ 
   PTR_SYMB list1;
   
   if (!functor || !name)
     return NULL;

   for ( list1 = SYMB_MEMBER_PARAM(functor) ; list1 ; list1 = SYMB_NEXT_DECL(list1))
      {    
	if (!strcmp(SYMB_IDENT(list1),name)) 
	  return(list1) ;
      }
   return(NULL);
 }

/***************************************************************************/
PTR_TYPE FollowTypeBaseAndDerived(type)
PTR_TYPE type;
{
  PTR_TYPE tmp;
  PTR_SYMB symb;
  if (!type)
    return NULL;
  if (isAtomicType(TYPE_CODE(type)))
    return type;
  tmp = lookForInternalBasetype(type);
  if (hasTypeSymbol(TYPE_CODE(tmp)))
    {
      symb = TYPE_SYMB_DERIVE(tmp);
      if (symb && SYMB_TYPE(symb))
	return FollowTypeBaseAndDerived(SYMB_TYPE(symb));
      else
	return tmp;
    }
  return tmp;
}

/* replace chain_up_type() */
/***************************************************************************/
PTR_TYPE addToBaseTypeList(type1,type2)
     PTR_TYPE type1,type2;
{ 
 PTR_TYPE tmp;
  if (!type2) return(type1);
  if (!type1) return(type2);

 tmp = lookForInternalBasetype(type2);
 if (tmp)
   {
     TYPE_BASE(tmp) = type1;
     return(type2);
   } else
     Message("error in addToBaseTypeList",0);
 return NULL;
}

/* return the symbol it inherit from */
/***************************************************************************/
PTR_SYMB doesClassInherit(bif)
     PTR_BFND bif;
{
  PTR_LLND ll;
  int lenght;
  if (!bif)
    return NULL;
  
  ll = BIF_LL2(bif);

  
  lenght = exprListLength(ll);
  if (lenght > 1)
    Message("Multiple inheritance not allowed",BIF_LINE(bif));
  ll = giveLlSymbInDeclList(ll);

  if (ll)
    return NODE_SYMB(ll);
  else
    return NULL;
}

/***************************************************************************/
PTR_SYMB getClassNextFieldOrMember(symb)
     PTR_SYMB symb;
{
  if (!symb)
    return NULL;

  if (SYMB_CODE(symb) == FIELD_NAME)
    return SYMB_NEXT_FIELD(symb);
  else  
    if (SYMB_CODE(symb) == MEMBER_FUNC)
      return SYMB_MEMBER_NEXT(symb);
  else
    return symb->next_symb;

  /* return NULL; */
}

/* find_first_field(pred) and find_first_field_2(pred)*/
/***************************************************************************/
PTR_SYMB getFirstFieldOfStruct(pred)
PTR_BFND  pred ;
{   
  /*  PTR_LLND ll_ptr1; */ /* podd 15.03.99*/
  PTR_LLND l2;
  /* PTR_BFND bf1 ;*/ /* podd 15.03.99*/
  PTR_BLOB blob;

  if (!pred)
      return  NULL;

  if (isAStructDeclBif(BIF_CODE(pred)) || isAUnionDeclBif(BIF_CODE(pred)) ||
      isAEnumDeclBif(BIF_CODE(pred)))
    {
      if (!(blob= BIF_BLOB1(pred)))
	{  
	  return NULL;
	}
      else
	{  
	  for ( ; blob ; blob = BLOB_NEXT(blob))
	    {
	      if (BLOB_VALUE(blob))
		l2 = giveLlSymbInDeclList(BIF_LL1(BLOB_VALUE(blob)));
	      else
		l2 = NULL;
	      if (l2) 
		{  
		  return NODE_SYMB(l2);
		}
	    }
	}
    }    
  return(NULL);
}


/***************************************************************************/
PTR_LLND addToExprList(expl,ll)
PTR_LLND expl, ll;
{
  PTR_LLND  tmp, lptr;

  if (!ll)
    return expl;
  if (!expl)
    return newExpr(EXPR_LIST,NULL,ll,NULL);

    tmp = newExpr(EXPR_LIST,NULL,ll,NULL);
    lptr = Follow_Llnd(expl,2);
    NODE_OPERAND1(lptr) = tmp;

  return expl;
}


/***************************************************************************/
PTR_LLND addToList(first,pt)
PTR_LLND first, pt;
{
  PTR_LLND  tail = first;
 
  if (!pt)
    return first;
  if (!first)
    return pt;
  else {
    while (NODE_OPERAND1(tail))
      tail = NODE_OPERAND1(tail);
    NODE_OPERAND1(tail) = pt;
    return first;
  }
}


/* was find_class_bfnd(object)*/ 
/***************************************************************************/
PTR_BFND getObjectStmt(object)
PTR_SYMB object;
{ 
  PTR_TYPE type;
  if (!object)
    return NULL;
  type = FollowTypeBaseAndDerived(SYMB_TYPE(object));
  if (type)
    {
      if (isStructType(TYPE_CODE(type)) ||
	  isEnumType(TYPE_CODE(type)) ||
	  isUnionType(TYPE_CODE(type))
	  )
	{    
	  return TYPE_COLL_ORI_CLASS(type);
	} else
	  Message("unexpected class/struct constructs",0);
    }
  return NULL;
}

/* was chain_field_symb() */
/***************************************************************************/
void addSymbToFieldList(first_one, current_one)
     PTR_SYMB first_one,current_one ;
{  
  PTR_SYMB old_symb,symb;

  if (!first_one || !current_one)
    return;
  for ( old_symb = symb = first_one ;symb  ;   )
    {  
      old_symb = symb ;
      symb = getClassNextFieldOrMember(symb);
    }
  if (SYMB_CODE(old_symb) == FIELD_NAME)
    SYMB_NEXT_FIELD(old_symb)  =  current_one ;
  else /* if(SYMB_CODE(old_symb) = MEMBER_FUNC) */
    SYMB_MEMBER_NEXT(old_symb) =  current_one ;
  old_symb->next_symb = current_one; 
}


/* 
  look for Array Reference From an expression
  There are chained in an expression list
*/
/***************************************************************************/
PTR_LLND LibarrayRefs(expr,listin)
     PTR_LLND expr,listin;
{
  PTR_LLND list = listin;
 
  if (!expr)
    return listin;
  
  if (NODE_CODE(expr) == ARRAY_REF)
    {
      list = addToExprList(list, expr);
    }
  list = LibarrayRefs(NODE_OPERAND0(expr),list);
  list = LibarrayRefs(NODE_OPERAND1(expr),list);
  return list;
}
 

/* all reference to a symbol (does not go inside array index expression ...)*/
/***************************************************************************/
PTR_LLND LibsymbRefs(expr,listin)
     PTR_LLND expr,listin;
{
  PTR_LLND list = listin;

  if (!expr)
    return listin;
  
  if (hasNodeASymb(NODE_CODE(expr)))
    {
      list = addToExprList(list, expr); 
      return list;
    }
  list = LibsymbRefs(NODE_OPERAND0(expr),list);
  list = LibsymbRefs(NODE_OPERAND1(expr),list);
  return list;
}

/***************************************************************************/
void LibreplaceWithStmt(biftoreplace,newbif)
     PTR_BFND biftoreplace,newbif;
{
  PTR_BFND before,parent,last;

  if (!biftoreplace|| !newbif)
    return;

  before = getNodeBefore(biftoreplace);
  parent = BIF_CP(biftoreplace);
  last = getLastNodeOfStmt(biftoreplace);
  
  extractBifSectionBetween(biftoreplace,last);
  insertBfndListIn(newbif,before,parent);

}

/***************************************************************************/
PTR_BFND LibdeleteStmt(bif)
     PTR_BFND bif;
{
  PTR_BFND last,current;

  if (!bif)
    return NULL;
  last = getLastNodeOfStmt(bif);
  /*podd 03.06.14*/
  current = bif;  /*podd 19.11.14*/
  if(BIF_CODE(bif)==IF_NODE || BIF_CODE(bif)==ELSEIF_NODE)
     while(current != last && BIF_CODE(last)==ELSEIF_NODE)
     {   current = last; last = getLastNodeOfStmt(last); }
  else if(BIF_CODE(bif)==FOR_NODE || BIF_CODE(bif)==WHILE_NODE)
  {  while( ((current != last) && (BIF_CODE(last) == FOR_NODE)) || (BIF_CODE(last) == WHILE_NODE) )
     {   current = last; last = getLastNodeOfStmt(last); }
     if(BIF_CODE(last)==LOGIF_NODE && BIF_CP(BIF_NEXT(last))==last) 
        last = BIF_NEXT(last);
  }
  extractBifSectionBetween(bif,last);
  return bif;
}

/***************************************************************************/
int LibIsSymbolReferenced(bif,symb)
     PTR_BFND bif;
     PTR_SYMB symb;
{
  PTR_BFND last,temp;

  if (!bif)
    return FALSE;
  last = getLastNodeOfStmt(bif);
  
  for (temp = bif; temp; temp = BIF_NEXT (temp))
    {
      if (IsRefToSymb(BIF_LL1(temp),symb) || 
          LibIsSymbolInExpression(BIF_LL1(temp),symb))
        return TRUE;
 
      if (IsRefToSymb(BIF_LL2(temp),symb) || 
          LibIsSymbolInExpression(BIF_LL2(temp),symb))
        return TRUE;
 
      if (IsRefToSymb(BIF_LL3(temp),symb) || 
          LibIsSymbolInExpression(BIF_LL3(temp),symb))
        return TRUE;
      if (temp == last)
        break;
    }
  return FALSE;
}


/***************************************************************************/
PTR_BFND LibextractStmt(bif)
     PTR_BFND bif;
{
  /*PTR_BFND last;*/ /* podd 15.03.99*/
  return  LibdeleteStmt (bif);
}


/***************************************************************************/
PTR_LLND getPositionInExprList(first,pos)
PTR_LLND first;
int pos;
{
  PTR_LLND tail;
  int len = 0;
  if (first == NULL)
    return NULL;
  for (tail = first; (len <pos) && tail; tail = NODE_OPERAND1(tail) ){
    len++;
  }
  
  if (tail)
    return NODE_OPERAND0(tail);

  return NULL;
}


/***************************************************************************/
PTR_LLND getPositionInList(first, pos)
PTR_LLND first;
int pos;
{
  PTR_LLND tail;
  int len = 0;
  if (first == NULL)
    return NULL;
  for (tail = first; (len <pos) && tail; tail = NODE_OPERAND1(tail) ){
    len++;
  }
 
  return tail;
 
}


/***************************************************************************/
int lenghtOfParamList(symb)
PTR_SYMB symb;
{
  PTR_SYMB ptsymb;
  int count = 0;
  if (!symb)
    return 0;
  ptsymb = SYMB_FUNC_PARAM (symb);
  while (ptsymb)
    {
      count++;
      ptsymb = SYMB_NEXT_DECL (ptsymb);
    }
  return count;
}


/***************************************************************************/
PTR_SYMB GetThParam(symb,pos)
PTR_SYMB symb;
int pos;
{
  PTR_SYMB ptsymb;
  int count = 0;
  if (!symb)
    return NULL;
  ptsymb = SYMB_FUNC_PARAM (symb);
  while (ptsymb)
    {
      if (count == pos)
	return  ptsymb;
      count++;
      ptsymb = SYMB_NEXT_DECL (ptsymb);
    }
  return NULL;
}


/* Code by ajm. */
/***************************************************************************/
void insertSymbInArgList(function_symb, arglist_loc, new_symb)
PTR_SYMB function_symb,new_symb;
int arglist_loc;
{

     PTR_SYMB next_symb;
     PTR_SYMB current_symb;

     int count = 0;

     if (function_symb == 0 )
	  return;

     current_symb = SYMB_FUNC_PARAM (function_symb);

     if ( arglist_loc == 0 )
     {
	  SYMB_FUNC_PARAM (function_symb) = new_symb;
	  SYMB_NEXT_DECL(new_symb) = current_symb;
     }	  
     else
     {
	  /* Each time we enter this while loop, we have already
	     checked out the symbol CURRENT_SYMB. */
	  
	  while (current_symb != 0)
	  {
	       count ++;
	       next_symb = SYMB_NEXT_DECL(current_symb);

	       if ( count == arglist_loc )
	       {
		    SYMB_NEXT_DECL(current_symb) = new_symb;
		    SYMB_NEXT_DECL(new_symb) = next_symb;
		    break;
	       }

	       current_symb = next_symb;
	  }

	  if ( count < arglist_loc )
	  {
	       Message ("Warning: Request to insert parameter way past existing last param.",0);
	  }
     }
}


/***************************************************************************/
void appendSymbToArgList(symb,ne)
PTR_SYMB symb,ne;
{
  PTR_SYMB ptsymb;
  /* int count = 0;*/
  if (!symb)
    return;
  ptsymb = SYMB_FUNC_PARAM (symb);
  if (!ptsymb)
    SYMB_FUNC_PARAM (symb) = ne;
  else
    {
      while (ptsymb)
	{
	  if (!SYMB_NEXT_DECL (ptsymb))
	    {
	      SYMB_NEXT_DECL (ptsymb) = ne;
	      break;
	    }
	  ptsymb = SYMB_NEXT_DECL (ptsymb);
	}
    }
}

/***************************************************************************/
int lenghtOfFieldList(symb)
PTR_SYMB symb;
{
  PTR_SYMB ptsymb=0;  /* BW and PHB */
  PTR_TYPE type;
  int count = 0;
  if (!symb)
    return 0;
  type = SYMB_TYPE(symb);
  if (type)
    {
      if(TYPE_CODE(type) == T_DESCRIPT)
         type = TYPE_BASE(type);
      ptsymb = TYPE_COLL_FIRST_FIELD(type);
    }
  while (ptsymb)
    {
      count++;
      ptsymb = getClassNextFieldOrMember (ptsymb);
    }
  return count;
}

/***************************************************************************/
int lenghtOfFieldListForType(type)
     PTR_TYPE type;
{
  PTR_SYMB ptsymb = NULL;
  int count = 0;
  if (type)
    ptsymb = TYPE_COLL_FIRST_FIELD(type);
  while (ptsymb)
    {
      count++;
      ptsymb = getClassNextFieldOrMember (ptsymb);
    }
  return count;
}

/***************************************************************************/
PTR_SYMB GetThOfFieldList(symb,pos)
PTR_SYMB symb;
int pos;
{
  PTR_SYMB ptsymb = NULL;
  PTR_TYPE type;
  int count = 0;
  if (!symb)
    return  NULL;
  type = SYMB_TYPE(symb);
  if (type)
    {
      if(TYPE_CODE(type) == T_DESCRIPT)
         type = TYPE_BASE(type);
      ptsymb = TYPE_COLL_FIRST_FIELD(type);
    }
  while (ptsymb)
    {
      count++;
      if (count == pos)
	return  ptsymb;
      ptsymb = getClassNextFieldOrMember (ptsymb);
    }
  return  NULL;
}

/***************************************************************************/
PTR_SYMB GetThOfFieldListForType(type,pos)
PTR_TYPE type;
int pos;
{
  PTR_SYMB ptsymb = NULL;
  int count = 0;
  if (type)
    ptsymb = TYPE_FIRST_FIELD(type);
  while (ptsymb)
    {
      count++;
      if (count == pos)
	return  ptsymb;
      ptsymb = getClassNextFieldOrMember(ptsymb);
    }
  return  NULL;
}

/***************************************************************************/
int countInStmtNode1(first,ki1)
     PTR_BFND first;
     int ki1;
{
  PTR_BLOB temp;
  PTR_BFND bif;
  int count =0;

  if (!first)
    return 0;
  
  for (temp = BIF_BLOB1 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if (BIF_CODE(bif) == ki1)
              count++;
      }
    }
  
  for (temp = BIF_BLOB2 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if (BIF_CODE(bif) == ki1)
              count++;
      }
    }
  return count;
}
/***************************************************************************/
PTR_BFND GetcountInStmtNode1(first,ki1,num)
     PTR_BFND first;
     int ki1,num;
{
  PTR_BLOB temp;
  PTR_BFND bif;
  int count =0;

  if (!first)
    return NULL;
  
  for (temp = BIF_BLOB1 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if (BIF_CODE(bif) == ki1)
          {
              if (count == num)
                  return bif;
              count++;
          }
      }
    }
  
  for (temp = BIF_BLOB2 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if (BIF_CODE(bif) == ki1)
          {
              if (count == num)
                  return bif;
              count++;
          }
      }
    }
  return NULL;
}


/***************************************************************************/
int countInStmtNode2(first,ki1,ki2)
     PTR_BFND first;
     int ki1,ki2;
{
  PTR_BLOB temp;
  PTR_BFND bif;
  int count =0;

  if (!first)
    return 0;
  
  for (temp = BIF_BLOB1 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if ((BIF_CODE(bif) == ki1) ||
              (BIF_CODE(bif) == ki2))
              count++;
      }
    }
   
  for (temp = BIF_BLOB2 (first); temp ; temp = BLOB_NEXT (temp))
    {
      if ((bif = BLOB_VALUE(temp)) != 0)
      {
          if ((BIF_CODE(bif) == ki1) ||
              (BIF_CODE(bif) == ki2))
              count++;
      }
    }
  return count;
}



/***************************************************************************/
PTR_BFND getMainProgram()
{
  PTR_BFND thebif;

  if (Check_Lang_Fortran(cur_proj))
    {
      for ( thebif = PROJ_FIRST_BIF();thebif;thebif=BIF_NEXT(thebif)) 
	{
	  if (BIF_CODE(thebif) == PROG_HEDR)
	    return thebif;
	}
    } else
      {
	for ( thebif = PROJ_FIRST_BIF();thebif;thebif=BIF_NEXT(thebif)) 
	  {
	    if (BIF_CODE(thebif) == FUNC_HEDR)
	      {
		if (BIF_SYMB(thebif) &&  SYMB_IDENT(BIF_SYMB(thebif))
		    && !strcmp("main",SYMB_IDENT(BIF_SYMB(thebif))))
		  return thebif;
	      }
	  }
      }
  return NULL;
}

/******************************************************************************/
/*  findPtrRefExp   -- a help procedure for makeDeclExp                        */
/*  BW, Feb 1994                                                              */
/*  given a symbol, and a type, this procedure constructs DEREF_OP/ADDRESS_OP */
/*  expressions corresponding to pointer/reference types                      */

PTR_LLND findPtrRefExp(symb, type)
      PTR_SYMB symb;
      PTR_TYPE type;
{
  PTR_LLND newex;
  PTR_TYPE oldtype=type;

  
  if (TYPE_CODE(type) == T_ARRAY)   /* for arrays ptrs and refs come */
                                    /* after T_ARRAY                 */
     newex = newExpr(ARRAY_REF,SYMB_TYPE(symb),symb,TYPE_RANGES(type));
  else {  /* for other types, go through all * an & to get to the first */
          /*  non-ptr/ref type */
       while ( (TYPE_CODE(type) == T_POINTER) 
            ||(TYPE_CODE(type) == T_REFERENCE)) {
         type = TYPE_BASE(type);
         }
 
       newex = newExpr(VAR_REF,type,symb);
       }

   if (SYMB_TYPE(symb) && (isAtomicType(TYPE_CODE(type))
			  || (TYPE_CODE(type) == T_DERIVED_COLLECTION)
			  || (TYPE_CODE(type) == T_DERIVED_TYPE)
                          || (TYPE_CODE(type) == T_DESCRIPT)
                          || (TYPE_CODE(type) == T_ARRAY)
                          || (TYPE_CODE(type) == T_FUNCTION)
			  )
      ) 
  {  /* traverse the ptr/ref types building DEREF_OP/ADDRESS_OP expressions */
     if (TYPE_CODE(type) == T_ARRAY) type = TYPE_BASE(oldtype);
     else  type = oldtype;
       while ( (TYPE_CODE(type) == T_POINTER) 
            ||(TYPE_CODE(type) == T_REFERENCE)) {
        if (TYPE_CODE(type) == T_POINTER) newex = newExpr(DEREF_OP,type,newex);
        else newex = newExpr(ADDRESS_OP,type,newex);
        type = TYPE_BASE(type);
        }  
  }
  else
            Message("Sorry declareAVar not completed yet",0);

  return newex;
}


/***************************************************************************/
/* BW feb 94                                                               */

PTR_LLND makeDeclExp(symb)
     PTR_SYMB symb;
{
    PTR_TYPE type;
    PTR_LLND newex = NULL;
    if (!symb)
        return NULL;
    if (SYMB_TYPE(symb) && (isAtomicType(TYPE_CODE(SYMB_TYPE(symb)))
        || (TYPE_CODE(SYMB_TYPE(symb)) == T_DERIVED_COLLECTION)
        || (TYPE_CODE(SYMB_TYPE(symb)) == T_DERIVED_TYPE)
        || (TYPE_CODE(SYMB_TYPE(symb)) == T_DESCRIPT))
       )
    {
        newex = newExpr(VAR_REF, SYMB_TYPE(symb), symb);
    }



    else if (SYMB_TYPE(symb) && ((TYPE_CODE(SYMB_TYPE(symb)) == T_POINTER) || (TYPE_CODE(SYMB_TYPE(symb)) == T_REFERENCE)) )
    {
        newex = findPtrRefExp(symb, SYMB_TYPE(symb));
    }
    else if (SYMB_TYPE(symb) && (TYPE_CODE(SYMB_TYPE(symb)) == T_ARRAY))
    {
        PTR_TYPE typebase;
        type = SYMB_TYPE(symb);
        typebase = lookForInternalBasetype(type);
        if ((TYPE_CODE(TYPE_BASE(SYMB_TYPE(symb))) == T_POINTER) || (TYPE_CODE(TYPE_BASE(SYMB_TYPE(symb))) == T_REFERENCE) )
        {
            newex = findPtrRefExp(symb, SYMB_TYPE(symb));
        }
        else newex = newExpr(ARRAY_REF, SYMB_TYPE(symb), symb, TYPE_RANGES(type));
    }
    else
        Message("Sorry, makeDeclExp not completed yet", 0);
    return newex;
}

/***************************************************************************/
/*  added by BW to handle functions and pointers to functions              */
/*  same as makeDeclExp but need a pointer to the parameter list                                                                       */

PTR_LLND makeDeclExpWPar(symb, parlist)
     PTR_SYMB symb;
     PTR_LLND parlist;
{
/*  PTR_BFND decl;
    PTR_TYPE type;
*/ /* podd 15.03.99*/
  PTR_LLND expr = NULL;
  if (!symb)
    return NULL;
  if(!parlist)
    return NULL;

  if (SYMB_CODE(symb) == FUNCTION_NAME)
      
    {

        expr= newExpr(FUNCTION_REF,SYMB_TYPE(symb), symb, parlist, NULL);
    } 



else if (SYMB_TYPE(symb) && ((TYPE_CODE(SYMB_TYPE(symb)) == T_POINTER)
			  || (TYPE_CODE(SYMB_TYPE(symb)) == T_REFERENCE)
			  )
        )
    {
        expr = findPtrRefExp(symb,SYMB_TYPE(symb));
        expr = newExpr(FUNCTION_OP, SYMB_TYPE(symb),expr,parlist);


    }
 else
	Message("Sorry makeDeclExpWPar not completed yet",0);
 return expr;
}

/***************************************************************************/
/* BW feb 94                                                               */

PTR_BFND makeDeclStmt(symb)
     PTR_SYMB symb;
{
  PTR_BFND decl;
  PTR_TYPE type;
  if (!symb)
    return NULL;


  decl = (PTR_BFND) newNode(VAR_DECL);
  BIF_LL1(decl) = addToExprList(BIF_LL1(decl),
                                 makeDeclExp(symb));


  /* (ajm) I make no claim that I have correctly transposed these tests
     or that I understand what goes on here. Hopefully it works for
     me, though. */
  if (SYMB_TYPE(symb) && (TYPE_CODE(SYMB_TYPE(symb)) == T_ARRAY)) 
  {
       PTR_TYPE typebase;
       type = SYMB_TYPE(symb);
       typebase = lookForInternalBasetype(type);

       if (Check_Lang_Fortran(cur_proj)) 
       {
	    BIF_LL2(decl) = newExpr(TYPE_OP,typebase,NULL); 
       }
       else
       {
	    NODE_TYPE(BIF_LL1(decl)) = typebase;
       }
  }
  else
  { 
       if (Check_Lang_Fortran(cur_proj)) 
       {
	    BIF_LL2(decl) = newExpr(TYPE_OP,SYMB_TYPE(symb),NULL);
       }
       else
       {
	    NODE_TYPE(BIF_LL1(decl)) = SYMB_TYPE(symb);
       }
  }

  return decl;
}

/***************************************************************************/
/*  added by BW to handle functions and pointers to functions              */
/*                                                                         */

PTR_BFND makeDeclStmtWPar(symb, parlist)
     PTR_SYMB symb;
     PTR_LLND parlist;
{
  PTR_BFND decl;

  if (!symb)
    return NULL;
  if(!parlist)
    return NULL;

  decl = (PTR_BFND) newNode(VAR_DECL);

  BIF_LL1(decl) = addToExprList(BIF_LL1(decl),
                          makeDeclExpWPar(symb,parlist));

  if (Check_Lang_Fortran(cur_proj))
    {
      BIF_LL2(decl) = newExpr(TYPE_OP,SYMB_TYPE(symb),NULL);
    }
  else
    {
      /* ajm: WARNING -- I think there's a bug here. Is the type of 
	 the symbol the type that is supposed to be declared? (Isn't
	 it T_ARRAY. */
      NODE_TYPE(BIF_LL1(decl)) = SYMB_TYPE(symb);
    }
  return decl;
}

/***************************************************************************/
/* modified feb 94, BW                                                     */
/*                                                                         */
void declareAVar(symb, where)
     PTR_SYMB symb;
     PTR_BFND where;
{
  SYMB_SCOPE(symb) = where;
  insertBfndListIn(makeDeclStmt(symb),where,where);
}

/***************************************************************************/
/*  added by BW to handle functions and pointers to functions              */
/*  feb 94                                                                 */

void declareAVarWPar(symb, parlist, where)
     PTR_SYMB symb;
     PTR_LLND parlist;
     PTR_BFND where;
{
  SYMB_SCOPE(symb) = where;
  insertBfndListIn(makeDeclStmtWPar(symb,parlist),where,where);
}


/* add control end and set properly the BIF_NEXT node*/
/***************************************************************************/
void addControlEndToStmt(bif)
PTR_BFND bif;
{
  PTR_BFND cend,last;
  if (!bif)
    return; 
  cend = (PTR_BFND) newNode(CONTROL_END);
  BIF_CP(cend) = bif;
  last = getLastNodeOfStmtNoControlEnd(bif);
  appendBfndListToList1(cend,bif);
  if (last)
    {
      BIF_NEXT(cend) = BIF_NEXT(last);
      BIF_NEXT(last) = cend ; 
    } else
      {
	BIF_NEXT(cend) = BIF_NEXT(bif);
	BIF_NEXT(bif) = cend ; 
      }

  if (BIF_BLOB2(bif))
    {
      cend = (PTR_BFND) newNode(CONTROL_END);
      BIF_CP(cend) = bif;
      appendBfndListToList2(cend,bif);
      if (BIF_CP(bif))
	LocalRedoBifNextChain(BIF_CP(bif));
      else
	LocalRedoBifNextChain(bif);
    }
}


/***************************************************************************/
void addControlEndToList2(bif)
PTR_BFND bif;
{
  PTR_BFND cend;
  if (!bif)
    return; 
  cend = (PTR_BFND) newNode(CONTROL_END);
  BIF_CP(cend) = bif;
  appendBfndListToList2(cend,bif);
  if (BIF_CP(bif))
    LocalRedoBifNextChain(BIF_CP(bif));
  else
    LocalRedoBifNextChain(bif);
}

/***************************************************************************/
int LibClanguage()
{
return !(Check_Lang_Fortran(cur_proj));
}

/***************************************************************************/
int LibFortranlanguage()
{
  return (Check_Lang_Fortran(cur_proj));
}

/***************************************************************************/
PTR_BFND LibextractStmtBody(bif)
PTR_BFND bif;
{
  PTR_BFND cend, body;

   if (!bif)
    return NULL;
  cend = getLastNodeOfStmt(bif);
  if (cend && isAControlEnd(BIF_CODE(cend)))
    cend = getNodeBefore(cend);
  if (cend != bif)
    body =  extractBifSectionBetween(BIF_NEXT(bif),cend);
  else
    body =  NULL;
  return body;
}

/* function for loops */
/***************************************************************************/
int LibisEnddoLoop(loop)
     PTR_BFND loop;
{
  PTR_BFND bif;
  
  if (BIF_CODE (loop) != FOR_NODE) 
    return FALSE;
  bif = getLastNodeOfStmt(loop);
  if (BIF_CODE (bif) != CONTROL_END)
    return FALSE;
  else 
    return TRUE;
}

/***************************************************************************/
int LibperfectlyNested(loop)
    PTR_BFND loop;
{
  int i = 0;
  PTR_BFND temp = loop;
  
  if (!loop)
    return 0;

  while (temp && BIF_NEXT (temp) &&
	 (BIF_CODE (BIF_NEXT (temp)) == FOR_NODE) &&
	 (BIF_CODE (temp) == FOR_NODE) 
	 && (LibisEnddoLoop (temp)) &&
	 (blobListLength(BIF_BLOB1 (temp)) == 2))
    {
      i++;
      temp = BIF_NEXT (temp);
    }
  
  if (temp &&
      (BIF_CODE (temp) == FOR_NODE) && 
      (LibisEnddoLoop(temp)))
    i++;
  return  i;
}

/***************************************************************************/
PTR_BFND LibgetNextNestedLoop(b)
     PTR_BFND b;
{
  PTR_BLOB blob;
  
  for(blob = BIF_BLOB1(b) ; blob; blob = BLOB_NEXT(blob))
    {
      if (BIF_CODE(BLOB_VALUE(blob)) == FOR_NODE)
        return(BLOB_VALUE(blob));
    }
  return(NULL);
}

/***************************************************************************/
PTR_BFND LibgetInnermostLoop(b)
     PTR_BFND b;
{
  PTR_BFND loop;
  int nb;
  
  if (!b)
    return NULL;
  nb = LibperfectlyNested(b);
  loop = b;
  while (LibgetNextNestedLoop(loop) && nb)
    {
      loop = LibgetNextNestedLoop(loop);
      nb --;
    }  
  return loop;
}

/***************************************************************************/
PTR_BFND LibgetPreviousNestedLoop(b)
PTR_BFND b;
{
  PTR_BFND loop;
  
  if (!b) return NULL;
  
  loop = BIF_CP(b);
  while(loop)
    {
      if (BIF_CODE(loop) == FOR_NODE)
        return loop;       
      if (BIF_CODE(loop) == GLOBAL)
        return NULL;
      loop = BIF_CP(loop);      
    }        
  return(NULL);
}

/***************************************************************************/

PTR_BFND LibGetScopeForDeclare(scope)
PTR_BFND scope;
{
  
  PTR_BFND temp;
  
  temp = scope;
  while(temp)
    {
      if ((BIF_CODE(temp) == PROC_HEDR) ||
          (BIF_CODE(temp) == PROS_HEDR) ||
          (BIF_CODE(temp) == PROG_HEDR) ||
          (BIF_CODE(temp) == GLOBAL) ||
          (BIF_CODE(temp) == FUNC_HEDR))
        break;
      temp = BIF_CP(temp);
    }
  return(temp);
}


/***************************************************************************/
PTR_BFND getScopeForLabel(scope)
PTR_BFND scope;
{
  
  PTR_BFND temp;
  /* rule for fortran */
  temp = scope;
  while(temp)
    {
      if ((BIF_CODE(temp) == PROC_HEDR) ||
          (BIF_CODE(temp) == PROS_HEDR) ||
          (BIF_CODE(temp) == PROG_HEDR) ||
          (BIF_CODE(temp) == GLOBAL) ||
          (BIF_CODE(temp) == FUNC_HEDR))
        break;
      temp = BIF_CP(temp);
    }
  return(temp);
}

/***************************************************************************/
PTR_BLOB getLabelUDChain(PTR_LABEL label, PTR_BFND scope)
{
    PTR_BFND func, last, temp;
    PTR_BLOB blob, first;
    PTR_LABEL tl;

    if (!label)
        return NULL;
    
    func = getScopeForLabel(scope);
    if (!func)
        return NULL;
    last = getLastNodeOfStmt(func);

    blob = NULL;
    first = NULL;
    for (temp = func; temp && (temp != last); temp = BIF_NEXT(temp))
    {
        tl = BIF_LABEL_USE(temp);
        if (BIF_LL3(temp) && (NODE_CODE(BIF_LL3(temp)) == LABEL_REF))
            tl = NODE_LABEL(BIF_LL3(temp));

        // Kolganov 18.07.18, take into account of ARITH lables and COMGOTO
        if (temp->variant == ARITHIF_NODE || temp->variant == COMGOTO_NODE || temp->variant == ASSGOTO_NODE)
        {
            PTR_LLND lb;
            if (temp->variant == COMGOTO_NODE || temp->variant == ASSGOTO_NODE)
                lb = BIF_LL1(temp);
            else
                lb = BIF_LL2(temp);
            PTR_LABEL arith_lab[256];

            int idx = 0;
            while (lb)
            {
                arith_lab[idx++] = NODE_LABEL(NODE_OPERAND0(lb));
                lb = NODE_OPERAND1(lb);
            }

            int z;
            for (z = 0; z < idx; ++z)
            {
                if (arith_lab[z] && (LABEL_STMTNO(arith_lab[z]) == LABEL_STMTNO(label)))
                {
                    if (blob)
                    {
                        BLOB_NEXT(blob) = (PTR_BLOB)newNode(BLOB_KIND);
                        blob = BLOB_NEXT(blob);
                        BLOB_VALUE(blob) = temp;
                    }
                    else
                    {
                        blob = (PTR_BLOB)newNode(BLOB_KIND);
                        BLOB_VALUE(blob) = temp;
                        first = blob;
                    }
                    break;
                }
            }
        }
        else
        {
            if (tl && (LABEL_STMTNO(tl) == LABEL_STMTNO(label)))
            {
                if (blob)
                {
                    BLOB_NEXT(blob) = (PTR_BLOB)newNode(BLOB_KIND);
                    blob = BLOB_NEXT(blob);
                    BLOB_VALUE(blob) = temp;
                }
                else
                {
                    blob = (PTR_BLOB)newNode(BLOB_KIND);
                    BLOB_VALUE(blob) = temp;
                    first = blob;
                }
            }
        }
    }
    return first;
}

/***************************************************************************/

void LibconvertLogicIf(PTR_BFND ifst)
{
    if (!ifst)
        return;
    if (BIF_CODE(ifst) == LOGIF_NODE)
    {/* Convert to if */
        PTR_BFND last, ctl;
        BIF_CODE(ifst) = IF_NODE;
        /* need to add a contro_end */
        last = getLastNodeOfStmt(ifst);
        ctl = (PTR_BFND)newNode(CONTROL_END);
        insertBfndListIn(ctl, last, ifst);
    }
}

/***************************************************************************/
int convertToEnddoLoop(PTR_BFND loop)
{
    PTR_BFND cend, bif, lastcend;
    PTR_BLOB blob, list_ud;
    PTR_LABEL label;
    PTR_CMNT comment;
    
    if (!loop)
        return 0;

    if (BIF_CODE(loop) != FOR_NODE)
        return 0;

    if (!LibisEnddoLoop(loop))
    {
        bif = getLastNodeOfStmt(loop);
        if (!bif)
            return 0;
        while (BIF_CODE(bif) == FOR_NODE)
        {
            /* because of continue stmt shared by loops */
            bif = getLastNodeOfStmt(bif);
            if (!bif)
                return 0;
        }

        if (BIF_CODE(bif) == CONT_STAT)
        {
            if (BIF_LABEL(bif) != NULL)
            {
                label = BIF_LABEL(bif);
                if (BIF_LABEL_USE(loop) &&
                    (LABEL_STMTNO(BIF_LABEL_USE(loop)) == LABEL_STMTNO(label)))
                {
                    list_ud = getLabelUDChain(label, loop);
                    if (blobListLength(list_ud) <= 1)
                    {
                        cend = (PTR_BFND)newNode(CONTROL_END);
                        BIF_CP(cend) = loop;
                        BIF_LABEL_USE(loop) = NULL;
                        BIF_CMNT(cend) = BIF_CMNT(bif);
                        BIF_LINE(cend) = BIF_LINE(bif); /*Bakhtin 26.01.10*/
                        bif = deleteBfnd(bif);
                        insertBfndListIn(cend, bif, loop);
                    }
                    else
                    { /* more than on uses of the label check if ok */
                        for (blob = list_ud; blob;
                            blob = BLOB_NEXT(blob))
                        {
                            if (!BLOB_VALUE(blob) || (BIF_CODE(BLOB_VALUE(blob)) != FOR_NODE))
                                return 0;
                        }
                        /* we insert as much enddo than necessary */
                        comment = BIF_CMNT(bif);
                        bif = deleteBfnd(bif);
                        lastcend = bif;
                        for (blob = list_ud; blob; blob = BLOB_NEXT(blob))
                        {
                            if (BLOB_VALUE(blob) && (BIF_CODE(BLOB_VALUE(blob)) == FOR_NODE))
                            {
                                BIF_LABEL_USE(BLOB_VALUE(blob)) = NULL;
                                cend = (PTR_BFND)newNode(CONTROL_END);
                                BIF_CMNT(cend) = comment;
                                BIF_LINE(cend) = BIF_LINE(lastcend); /*Bakhtin 26.01.10*/
                                comment = NULL;
                                BIF_CMNT(bif) = NULL;
                                insertBfndListIn(cend, lastcend, BLOB_VALUE(blob));
                                /*lastcend = Get_Node_Before(cend); */
                            }
                        }
                    }
                    return 1;
                }
                else
                    return 0;  /* something is wrong the label is not the same */
            }
            else
            { /* should not appear CONTINUE without label */
                cend = (PTR_BFND)newNode(CONTROL_END);/*podd 12.03.99*/
                BIF_CMNT(cend) = BIF_CMNT(bif);
                BIF_LINE(cend) = BIF_LINE(bif); /*Bakhtin 26.01.10*/
                bif = deleteBfnd(bif);
                insertBfndListIn(cend, bif, loop);
                return 0;
            }

        }
        else
        { /* this not a enddo or a cont stat; probably a statement */
            label = BIF_LABEL(bif);
            list_ud = getLabelUDChain(label, loop);
            if (label && blobListLength(list_ud) <= 1)
            {
                cend = (PTR_BFND)newNode(CONTROL_END);
                BIF_LINE(cend) = BIF_LINE(bif); /*Bakhtin 26.01.10*/
                insertBfndListIn(cend, bif, loop);
                BIF_LABEL(bif) = NULL;
                BIF_LABEL_USE(loop) = NULL;
            }
            else
                return 0;
        }
        return 1;
    }
    else
        return 1;
}


/* (fbodin) Duplicate Symbol and type routine (modified phb) */
/***************************************************************************/
PTR_TYPE duplicateType(type)
     PTR_TYPE type;
{
  PTR_TYPE newtype;  
  if (!type)
    return NULL;
  
  if (!isATypeNode(NODE_CODE(type)))
    {
      Message("duplicateType; Not a type node",0);
      return NULL;
    }
  if (isAtomicType(TYPE_CODE(type)) && TYPE_CODE(type)!= T_STRING && !TYPE_RANGES(type) && !TYPE_KIND_LEN(type))
    return(GetAtomicType(TYPE_CODE(type)));  /*07.06.06*/ /*22.04.14*/
 
  /***** Allocate a new node *****/
  newtype = (PTR_TYPE) newNode(TYPE_CODE(type));

  /* Copy the fields that are NOT in the union */
  TYPE_SYMB(newtype) = TYPE_SYMB(type);
  TYPE_LENGTH(newtype) =TYPE_LENGTH(type);

  /* Copy the size of the union (all of the fields) (phb)*/
  memcpy(&(newtype->entry),&(type->entry),sizeof(type->entry));

  if (isAtomicType(TYPE_CODE(type)))
    {
      if (TYPE_RANGES(type))
        TYPE_RANGES(newtype) = copyLlNode(TYPE_RANGES(type));
      if (TYPE_KIND_LEN(type))
        TYPE_KIND_LEN(newtype) = copyLlNode(TYPE_KIND_LEN(type)); /*22.04.14*/
      return newtype;
    }
  if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
       TYPE_BASE(newtype) = duplicateType(TYPE_BASE(type));
    }
  if (hasTypeSymbol(TYPE_CODE(type)))
    {
      TYPE_SYMB_DERIVE(newtype) = TYPE_SYMB_DERIVE(type);
    }
  switch (TYPE_CODE(type))
    {
    case T_ARRAY		:
      TYPE_RANGES(newtype) = copyLlNode(TYPE_RANGES(type));
      break;	
    case T_DESCRIPT		:
      TYPE_LONG_SHORT(newtype) = TYPE_LONG_SHORT(type);
      break;
    }
  return newtype;
}

/***************************************************************************/

PTR_SYMB duplicateSymbolAcrossFiles();

PTR_TYPE duplicateTypeAcrossFiles(type)
     PTR_TYPE type;
{
  PTR_TYPE newtype;  
  if (!type)
    return NULL;
  
  if (!isATypeNode(NODE_CODE(type)))
    {
      Message("duplicateTypeAcrossFiles; Not a type node",0);
      return NULL;
    }
  if (isAtomicType(TYPE_CODE(type)) && TYPE_CODE(type)!= T_STRING && !TYPE_RANGES(type) && !TYPE_KIND_LEN(type))
    return(GetAtomicType(TYPE_CODE(type)));   /*07.06.06*/ /*22.04.14*/
  
  /***** Allocate a new node *****/
  newtype = (PTR_TYPE) newNode(TYPE_CODE(type));

  /* Copy the fields that are NOT in the union */
  TYPE_SYMB(newtype) = TYPE_SYMB(type);
  TYPE_LENGTH(newtype) =TYPE_LENGTH(type);

  /* Copy the size of the union (all of the fields) (phb)*/
  memcpy(&(newtype->entry),&(type->entry),sizeof(type->entry));

  if (isAtomicType(TYPE_CODE(type)))
    {
      if (TYPE_RANGES(type))
        TYPE_RANGES(newtype) = copyLlNode(TYPE_RANGES(type)); /*07.06.06*/
      if (TYPE_KIND_LEN(type))
        TYPE_KIND_LEN(newtype) = copyLlNode(TYPE_KIND_LEN(type)); /*22.04.14*/

      return newtype;
    }

  if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
       TYPE_BASE(newtype) = duplicateTypeAcrossFiles(TYPE_BASE(type));
    }
  if (hasTypeSymbol(TYPE_CODE(type)))
    {
      TYPE_SYMB_DERIVE(newtype) = duplicateSymbolAcrossFiles(TYPE_SYMB_DERIVE(type));
    }
  switch (TYPE_CODE(type))
    {
    case T_ARRAY		:
      TYPE_RANGES(newtype) = copyLlNode(TYPE_RANGES(type));
      break;	
    case T_DESCRIPT		:
      TYPE_LONG_SHORT(newtype) = TYPE_LONG_SHORT(type);
      break;
    }
  return newtype;
}


/***************************************************************************/
PTR_SYMB duplicateParamList(symb)
     PTR_SYMB symb;
{
  PTR_SYMB first, previous, ptsymb,ts;
  ptsymb = SYMB_FUNC_PARAM (symb);
  ts = NULL;
  first = NULL;
  previous = NULL;
  while (ptsymb)
    {
      ts = duplicateSymbol(ptsymb);
      if (!first)
        first = ts;
      if (previous)
        SYMB_NEXT_DECL (previous) = ts;
      previous = ts;
      ptsymb = SYMB_NEXT_DECL (ptsymb);
    }
  if (ts)
    SYMB_NEXT_DECL (ts) = NULL;
  return first;
}


/***************************************************************************/
PTR_SYMB duplicateSymbol(symb)
     PTR_SYMB symb;
{
  PTR_SYMB  newsymb;
  /* char *str;*/ /* podd 15.03.99*/
  if (!symb)
    return NULL;
  
  if (!isASymbNode(NODE_CODE(symb)))
    {
      Message("duplicateSymbol; Not a symbol node",0);
      return NULL;
    }
  newsymb = (PTR_SYMB) newSymbol(SYMB_CODE(symb),SYMB_IDENT(symb),SYMB_TYPE(symb));
  
  SYMB_ATTR(newsymb) = SYMB_ATTR(symb);

  /* Copy the size of the union (all of the fields) (phb)*/
  memcpy(&(newsymb->entry.Template),&(symb->entry.Template),
        sizeof(newsymb->entry.Template));

  /*dirty trick for debug, to identify copie/
  str = (char *) xmalloc(512);
  sprintf(str,"DEBUG%d%s",newsymb,SYMB_IDENT(newsymb));
  SYMB_IDENT(newsymb) = str;
  */
  /* copy the expression for Constant Node */
  if (SYMB_CODE(newsymb) == CONST_NAME)
    SYMB_VAL(newsymb) = copyLlNode(SYMB_VAL(newsymb));
  return newsymb;
}

/***************************************************************************/
PTR_SYMB duplicateSymbolLevel1(symb)
     PTR_SYMB symb;
{
  PTR_SYMB  newsymb;

  if (!symb)
    return NULL;

  if (!isASymbNode(NODE_CODE(symb)))
    {
      Message("duplicateSymbolLevel1; Not a symbol node",0);
      return NULL;
    }
  newsymb = duplicateSymbol(symb);

  /* to be updated later Not that simple*/
  switch (SYMB_CODE(symb))
    {
    case MEMBER_FUNC:
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROCESS_NAME:
      SYMB_FUNC_PARAM (newsymb) = duplicateParamList(symb);
      break;
    }
  return newsymb;
}

/***************************************************************************/
PTR_BFND getBodyOfSymb(symb)
PTR_SYMB symb;
{
  /*  PTR_SYMB  newsymb = NULL;*/
  PTR_BFND body = NULL;
  PTR_TYPE type;
  if (!symb)
    return NULL;

  if (!isASymbNode(NODE_CODE(symb)))
    {
      Message("getbodyofsymb; not a symbol node",0);
      return NULL;
    }
  switch (SYMB_CODE(symb))
    {
    case MEMBER_FUNC:
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROCESS_NAME:
      body = SYMB_FUNC_HEDR(symb);
      if (!body)
        body = getFunctionHeaderAllFile(symb);
      break;
    case PROGRAM_NAME:
      body = symb->entry.prog_decl.prog_hedr;
      if (!body)
        body = getFunctionHeaderAllFile(symb);
      break;

    case CLASS_NAME:
    case TECLASS_NAME:
    case COLLECTION_NAME:
      type = SYMB_TYPE(symb);
      if (type)
        {
          body = TYPE_COLL_ORI_CLASS(type);
        } else
          {
            Message("body of collection or class not found",0);
	    return NULL;
          }
      break;
    }
  return body;
}


/***************************************************************************/
void replaceSymbInExpression(PTR_LLND exprold, PTR_SYMB symb, PTR_SYMB new)
{
    if (!exprold || !symb || !new)
        return;
    if (!isASymbNode(SYMB_CODE(symb)))
    {
        Message(" not a symbol node in replaceSymbInExpression", 0);
        return;
    }
    if (!isASymbNode(SYMB_CODE(new)))
    {
        Message(" not a symbol node in replaceSymbInExpression", 0);
        return;
    }

    if (hasNodeASymb(NODE_CODE(exprold)))
    {
        if (NODE_SYMB(exprold) == symb)
            NODE_SYMB(exprold) = new;
    }
    replaceSymbInExpression(NODE_OPERAND0(exprold), symb, new);
    replaceSymbInExpression(NODE_OPERAND1(exprold), symb, new);
}

/***************************************************************************/
void replaceSymbInStmts(debut, fin, symb, new)
     PTR_BFND debut, fin;
     PTR_SYMB symb,new;
{
    PTR_BFND temp;

    for (temp = debut; temp; temp = BIF_NEXT(temp))
    {
        if (BIF_SYMB(temp) == symb)
            BIF_SYMB(temp) = new;
        replaceSymbInExpression(BIF_LL1(temp), symb, new);
        replaceSymbInExpression(BIF_LL2(temp), symb, new);
        replaceSymbInExpression(BIF_LL3(temp), symb, new);
        if (fin && (temp == fin))
            break;
    }
}

/***************************************************************************/
void replaceSymbInExpressionSameName(exprold,symb, new)
     PTR_LLND exprold;
     PTR_SYMB symb, new;
{
  if (!exprold || !symb || !new)
    return;
  if (!isASymbNode(SYMB_CODE(symb)))
    {
      Message(" not a symbol node in replaceSymbInExpressionSameName",0);
      return;
    }
  if (!isASymbNode(SYMB_CODE(new)))
    {
      Message(" not a symbol node in replaceSymbInExpressionSameName",0);
      return;
    }
  if (hasNodeASymb(NODE_CODE(exprold)))
    {
      if (sameName(NODE_SYMB(exprold),symb))
	{
	  NODE_SYMB(exprold) = new;
	}
    }
  replaceSymbInExpressionSameName(NODE_OPERAND0(exprold), symb, new);
  replaceSymbInExpressionSameName(NODE_OPERAND1(exprold), symb, new);
}


/***************************************************************************/
void replaceSymbInStmtsSameName(debut, fin, symb, new)
     PTR_BFND debut, fin;
     PTR_SYMB symb,new;
{
  PTR_BFND temp;
  
  for (temp = debut; temp ; temp = BIF_NEXT(temp))
    {
      if (sameName(BIF_SYMB(temp),symb))
	BIF_SYMB(temp) = new;
      replaceSymbInExpressionSameName(BIF_LL1(temp), symb,new);
      replaceSymbInExpressionSameName(BIF_LL2(temp), symb,new);
      replaceSymbInExpressionSameName(BIF_LL3(temp), symb,new);
      if (fin && (temp == fin))
        break;
    }  
}

/***************************************************************************/
PTR_SYMB duplicateSymbolLevel2(symb)
     PTR_SYMB symb;
{
  PTR_SYMB  newsymb;
  PTR_BFND body,newbody,last,before,cp;
  PTR_SYMB ptsymb,ptref;
  if (!symb)
    return NULL;

  if (!isASymbNode(NODE_CODE(symb)))
    {
      Message("duplicateSymbolLevel2; Not a symbol node",0);
      return NULL;
    }
  newsymb = duplicateSymbolLevel1(symb);

  /* to be updated later Not that simple*/
  switch (SYMB_CODE(symb))
    {
    case MEMBER_FUNC:
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROCESS_NAME:
      /* duplicate the body */
      body = getBodyOfSymb(symb);
      if (body)
	{
	  before = getNodeBefore(body);
	  cp = BIF_CP(body);
	  last = getLastNodeOfStmt(body);
	  body =  extractBifSectionBetween(body,last);
	  newbody = duplicateStmts (body);
	  insertBfndListIn (body, before,cp);
	  insertBfndListIn (newbody, before,cp);
	  BIF_SYMB(newbody) = newsymb;
	  SYMB_FUNC_HEDR(newsymb) = newbody;
	  /* we have to propagate change in the param list in the new body */
	  ptsymb = SYMB_FUNC_PARAM (newsymb);
	  ptref =  SYMB_FUNC_PARAM (symb);
	  last = getLastNodeOfStmt(newbody);
	  while (ptsymb)
	    {
	      replaceSymbInStmts(newbody,last,ptref,ptsymb);
	      ptsymb = SYMB_NEXT_DECL (ptsymb);
	      ptref = SYMB_NEXT_DECL (ptref);
	    }
	}
      break;
    case CLASS_NAME:
    case TECLASS_NAME:
    case COLLECTION_NAME:
    case STRUCT_NAME:
    case UNION_NAME:
      body = getBodyOfSymb(symb);
      if (body)
	{
	  before = getNodeBefore(body);
	  cp = BIF_CP(body);
	  last = getLastNodeOfStmt(body);
	  body =  extractBifSectionBetween(body,last);
	  newbody = duplicateStmts (body);
	  insertBfndListIn (body, before,cp);
	  insertBfndListIn (newbody, before,cp);
	  BIF_SYMB(newbody) = newsymb;
	  /* probably more to do here */
	  SYMB_TYPE(newsymb) = duplicateType(SYMB_TYPE(symb));
	  /* set the new body for the symbol */
	  TYPE_COLL_ORI_CLASS(SYMB_TYPE(newsymb)) = newbody;
	}
      break;
    }
  return newsymb;
}

/***************************************************************************/
int arraySymbol(symb)
     PTR_SYMB symb;
{
  PTR_TYPE type;
  if (!symb)
    return FALSE;
  type = SYMB_TYPE(symb);
  if (!type)
    return FALSE;
  if (TYPE_CODE(type) == T_ARRAY)
    return TRUE;  
  return FALSE;
}

/***************************************************************************/
int pointerType(type)
     PTR_TYPE type;
{
  if (!type)
    return FALSE;
  return  isPointerType(TYPE_CODE(type));
}

/***************************************************************************/
int isIntegerType(type)
     PTR_TYPE type;
{
  if (!type)
    return FALSE;
  return (TYPE_CODE(type) == T_INT);
}

/***************************************************************************/
/* this function was all wrong, fixed May 25 1994, BW                      */
PTR_SYMB getFieldOfStructWithName(name,typein)
     char *name;
     PTR_TYPE typein;
{
  PTR_TYPE type;
  PTR_SYMB ptsymb = NULL;
  if (!typein || !name)
    return  NULL;

      type = SYMB_TYPE(TYPE_SYMB_DERIVE(typein));  


      if(TYPE_CODE(type) == T_DESCRIPT)
         type = TYPE_BASE(type); 
         /* the if statement above is necessary because of another bug */
         /* with "friend" specifier                                    */
      ptsymb = TYPE_COLL_FIRST_FIELD(type);


  if (! (ptsymb)) Message("did not find the first field\n",0);

  while (ptsymb)
    {
      if (!strcmp(SYMB_IDENT(ptsymb), name))
	return  ptsymb;
      ptsymb = getClassNextFieldOrMember (ptsymb);
    }
  return  NULL;
}

/***************************************************************************/
PTR_LLND addLabelRefToExprList(expl,label)
     PTR_LLND expl;
     PTR_LABEL label;
{
  PTR_LLND  tmp, lptr,pt;

  if (!label)
    return expl;
  pt = (PTR_LLND) newNode(LABEL_REF);
  NODE_LABEL(pt) = label;
  tmp = newExpr(EXPR_LIST,NULL,pt,NULL);
  if (!expl)
    return tmp;
  lptr = Follow_Llnd(expl,2);
  NODE_OPERAND1(lptr) = tmp;
  return expl;
}

/***************************************************************************/
PTR_BFND getStatementNumber(bif,pos)
     int pos;
     PTR_BFND bif;
{
  PTR_BFND ptbfnd = NULL;
  /*  PTR_TYPE type;*/ /* podd 15.03.99*/
  int count = 0;
  if (!bif)
    return  NULL;
    ptbfnd = bif;
  while (ptbfnd)
    {
      count++;
      if (count == pos)
	return  ptbfnd;
      ptbfnd = BIF_NEXT(ptbfnd);
    }
  return  NULL;

}

/***************************************************************************/
PTR_LLND deleteNodeInExprList(first,pos)
PTR_LLND first;
int pos;
{
  PTR_LLND tail,old = NULL;
  int len = 0;
  if (first == NULL)
    return NULL;

  if (pos == 0)
    return NODE_OPERAND1(first);
  for (tail = first; tail; tail = NODE_OPERAND1(tail) )
    {
      len++;
      if (len  == pos)
	{
	  NODE_OPERAND1(old) = NODE_OPERAND1(tail);
	  return first;
	}
      old = tail;
    }
  
  return first;
}

/***************************************************************************/
PTR_LLND deleteNodeWithItemInExprList(first,ll)
PTR_LLND first,ll;
{
  PTR_LLND tail,old = NULL;
  if (first == NULL)
    return NULL;

  if (NODE_OPERAND0(first) == ll)
    return NODE_OPERAND1(first);
  for (tail = first; tail; tail = NODE_OPERAND1(tail) )
    {
      if (NODE_OPERAND0(tail) == ll)
	{
	  NODE_OPERAND1(old) = NODE_OPERAND1(tail);
	  return first;
	}
      old = tail;
    }
  return first;
}

/***************************************************************************/
PTR_LLND addSymbRefToExprList(expl,symb)
     PTR_LLND expl;
     PTR_SYMB symb;
{
  PTR_LLND  tmp, lptr,pt;

  if (!symb)
    return expl;
  pt = newExpr(VAR_REF,SYMB_TYPE(symb), symb);
  tmp = newExpr(EXPR_LIST,NULL,pt,NULL);
  if (!expl)
    return tmp;
  lptr = Follow_Llnd(expl,2);
  NODE_OPERAND1(lptr) = tmp;
  return expl;
}

/* functions mainly dedicated to libcreatecollectionwithtype */
/***************************************************************************/
void duplicateAllSymbolDeclaredInStmt(symb,stmt, oldident)
     PTR_SYMB symb; /* symb is not to duplicate */
     PTR_BFND stmt;
     char *oldident;
{
  PTR_SYMB oldsymb, newsymb, ptsymb, ptref;
  PTR_BFND cur,last,last1;
  /*PTR_BFND body;*/ /* podd 15.03.99*/
  PTR_BFND cur1,last2;
  PTR_LLND ll1, ll2;
  char str[512], *str1 = NULL;
  PTR_SYMB tabsymbold[MAX_SYMBOL_FOR_DUPLICATE];
  PTR_SYMB tabsymbnew[MAX_SYMBOL_FOR_DUPLICATE];
  int nbintabsymb = 0;
  int i;
  if (!stmt || !symb )
    return;

  last = getLastNodeOfStmt(stmt);
  
  /* if that is a class/collection we have to take care of the constructor and destructor */
  if (oldident)
    {
      str1 = (char *) xmalloc(strlen(SYMB_IDENT(symb))+2);
      if ((int)strlen(oldident) >= 511)
	{
	  Message("internal error: string too long exit",0);
	  exit(1);
	}
      sprintf(str1,"~%s",SYMB_IDENT(symb)); 
      sprintf(str,"~%s",oldident);
    }
  for (cur = stmt; cur ; cur =  BIF_NEXT(cur))
    {
      if ((BIF_CODE(cur) == FUNC_HEDR) && (isInStmt(stmt,cur)))
	{ /* local declaration, update the owner */
	  if (BIF_SYMB(cur))
	    {
	      oldsymb = BIF_SYMB(cur);
	      newsymb = duplicateSymbolLevel1(BIF_SYMB(cur));
	      
/*	      str1 = (char *) xmalloc(512);
	      sprintf(str1,"COPYFORDEBUG%d%s",newsymb,SYMB_IDENT(newsymb));
	      SYMB_IDENT(newsymb) = str1;*/
	      tabsymbold[nbintabsymb] = oldsymb;
	      tabsymbnew[nbintabsymb] = newsymb;
	      nbintabsymb ++;
	      if (nbintabsymb >= MAX_SYMBOL_FOR_DUPLICATE)
		{
		  Message("To many symbol in duplicateAllSymbolDeclaredInStmt",0);
		  exit(1);
		}
	      BIF_SYMB(cur) = newsymb;
	      SYMB_FUNC_HEDR(newsymb) = cur;
	      SYMB_SCOPE(newsymb) = stmt;
	      ptsymb = SYMB_FUNC_PARAM (newsymb);
	      ptref =  SYMB_FUNC_PARAM (oldsymb);
	      last2 = getLastNodeOfStmt(cur);
	      while (ptsymb)
		{
		  replaceSymbInStmts(cur,last2,ptref,ptsymb);
		  ptsymb = SYMB_NEXT_DECL (ptsymb);
		  ptref = SYMB_NEXT_DECL (ptref);
		}
	      duplicateAllSymbolDeclaredInStmt(newsymb,cur,oldident);
	      if (SYMB_CODE(newsymb) == MEMBER_FUNC)
		{ /* there is more to do here */
		  SYMB_MEMBER_BASENAME(newsymb) = symb;
		}
	      if (oldident)
		{ /* change name of constructor and destructor */
		  if (!strcmp(SYMB_IDENT(newsymb),oldident)) 
		    {
		      SYMB_IDENT(newsymb) = SYMB_IDENT(symb);
		    }
		  if (!strcmp(SYMB_IDENT(newsymb),str)) 
		    {
		      SYMB_IDENT(newsymb) = str1;
		    }
		}
	      cur = getLastNodeOfStmt(cur);
	    }
	}
      if ((BIF_CODE(cur) == VAR_DECL) && (isInStmt(stmt,cur)))
	{ /* we have to declare what is declare there */
	  /*  ll1= BIF_LL1(cur); this is the declaration */
	  	  
	  for (ll1= BIF_LL1(cur); ll1; ll1 = NODE_OPERAND1(ll1))
	    {
	      ll2 = giveLlSymbInDeclList(NODE_OPERAND0(ll1));
	      if (ll2 && NODE_SYMB(ll2) && (NODE_SYMB(ll2) != symb))
		{
		  oldsymb = NODE_SYMB(ll2);
		  NODE_SYMB(ll2) = duplicateSymbolLevel2(NODE_SYMB(ll2));
		  tabsymbold[nbintabsymb] = oldsymb;
		  tabsymbnew[nbintabsymb] = NODE_SYMB(ll2);
		  nbintabsymb ++;
		  if (nbintabsymb >= MAX_SYMBOL_FOR_DUPLICATE)
		    {
		      Message("To many symbol in duplicateAllSymbolDeclaredInStmt",0);
		      exit(1);
		    }
		  /* apply recursively */
		  if (getBodyOfSymb(NODE_SYMB(ll2)) && (!isInStmt(stmt,getBodyOfSymb(NODE_SYMB(ll2)))))
		    {
		      duplicateAllSymbolDeclaredInStmt(NODE_SYMB(ll2), getBodyOfSymb(NODE_SYMB(ll2)),oldident);
		    }
		  /* if member function we must attach the new symbol of
		     collection also true for field name */
		  if (SYMB_CODE(NODE_SYMB(ll2)) == MEMBER_FUNC)
		    { /* there is more to do here */
		      SYMB_MEMBER_BASENAME(NODE_SYMB(ll2)) = symb;
		    }
		  if (SYMB_CODE(NODE_SYMB(ll2)) == FIELD_NAME)
		    { /* there is more to do here */
		      SYMB_FIELD_BASENAME(NODE_SYMB(ll2)) = symb;
		    }
		  SYMB_SCOPE(NODE_SYMB(ll2)) = stmt; /* is that correct??? */
		  
		  if (oldident)
		    { /* change name of constructor and destructor */

		      if (!strcmp(SYMB_IDENT(NODE_SYMB(ll2)),oldident)) 
			{
			  SYMB_IDENT(NODE_SYMB(ll2)) = SYMB_IDENT(symb);
			}
		      if (!strcmp(SYMB_IDENT(NODE_SYMB(ll2)),str)) 
			{
			  SYMB_IDENT(NODE_SYMB(ll2)) = str1;
			}
		     
		    }
		  /* we have to replace the old symbol in the section */
		  replaceSymbInStmts(stmt,last,oldsymb,NODE_SYMB(ll2));		  
		}
	    }
	}
      if (cur == last)
	break;
    }

  /* we need to replace in the member function the symbol declared in the structure */
  for (cur = stmt; cur ; cur =  BIF_NEXT(cur))
    {
      if ((BIF_CODE(cur) == FUNC_HEDR) && isInStmt(stmt,cur))
	{ /* local declaration, update the owner */
	  if (BIF_SYMB(cur))
	    {
	      cur1 = stmt;
	      last1 = getLastNodeOfStmt(cur1);
	      for (i=0; i<nbintabsymb; i++)
		replaceSymbInStmts(cur1,last1,tabsymbold[i],tabsymbnew[i]);
	    }
	}
      if ((BIF_CODE(cur) == VAR_DECL) && isInStmt(stmt,cur))
	{ /* we have to declare what is declare there */
	  
	  for (ll1= BIF_LL1(cur); ll1; ll1 = NODE_OPERAND1(ll1))
	    {
	      ll2 = giveLlSymbInDeclList(NODE_OPERAND0(ll1));
	      if (ll2 && NODE_SYMB(ll2) && (NODE_SYMB(ll2) != symb))
		{
		  oldsymb = NODE_SYMB(ll2);
		  cur1 = getBodyOfSymb(oldsymb);
		  if (cur1 && !isInStmt(stmt,cur1))
		    {
		      last1 = getLastNodeOfStmt(cur1);
		      for (i=0; i<nbintabsymb; i++)
			replaceSymbInStmts(cur1,last1,tabsymbold[i],tabsymbnew[i]);
		    }
		}
	    }
	}
      if (cur == last)
	break;
    }
}

/*
  function to implement replace type in 
*/
/***************************************************************************/
int isTypeEquivalent(type1, type2)
     PTR_TYPE type1, type2;
{
/*  PTR_TYPE type_a,type_b ;
  int level1, level2 ;
  int temp_var1,temp_var2;
  PTR_TYPE b1,b2;
*/ /*podd 15.03.99 */
  PTR_SYMB symb1,symb2;
 

  if (type1 == type2)
    return TRUE;  
  if (!type1 ||!type2)
    return FALSE;
  
  if (!isATypeNode(TYPE_CODE(type1)))
    {
      Message("isTypeEquivalent; arg1 Not a type node",0);
      return 0;
    }
  if (!isATypeNode(TYPE_CODE(type2)))
    {
      Message("isTypeEquivalent; arg2 Not a type node",0);
      return 0;
    }
  
  if (TYPE_CODE(type1) != TYPE_CODE(type2))
      return 0;

  if (isAtomicType(TYPE_CODE(type1)) && !TYPE_RANGES(type1) && !TYPE_RANGES(type2) && !TYPE_KIND_LEN(type1) && !TYPE_KIND_LEN(type2))
     return(1);

  if (hasTypeBaseType(TYPE_CODE(type1)) && hasTypeBaseType(TYPE_CODE(type2))) 
     {
       if (TYPE_BASE(type1))
	 return isTypeEquivalent(TYPE_BASE(type1),TYPE_BASE(type2));
     }
  if ((TYPE_CODE(type1) ==T_DERIVED_COLLECTION) &&
      (TYPE_CODE(type2) ==T_DERIVED_COLLECTION))
     { /* check for class<nnn> */
       symb1 = TYPE_SYMB_DERIVE(type1);
       symb2 = TYPE_SYMB_DERIVE(type2);
       if (symb1 && symb2)
	 {
	   if (symb1 == symb2)
	     return isTypeEquivalent(TYPE_COLL_BASE(type1), TYPE_COLL_BASE(type2));
	   else
	     if (sameName(symb1,symb2)) /* this is a type name, the same ident should be enough*/
	       return isTypeEquivalent(TYPE_COLL_BASE(type1), TYPE_COLL_BASE(type2));
	     else
	       return 0;
	 }
     } else 
       if (hasTypeSymbol(TYPE_CODE(type1)))
	 {
	   symb1 = TYPE_SYMB_DERIVE(type1);
	   symb2 = TYPE_SYMB_DERIVE(type2);
	   if (symb1 && symb2)
	     {
	       if (symb1 == symb2)
		 return 1;
	       else
		 if (sameName(symb1,symb2)) /* this is a type name, the same ident should be enough*/
		   return 1;
		 else
		   return 0;
	     }
	 }
   return(0);
}


/***************************************************************************/
int lookForTypeInType(type,comp)
     PTR_TYPE type,comp;
{
  if (!type)
    return 0;
  if (!isATypeNode(TYPE_CODE(type)))
    {
      Message("lookForTypeInType; arg1 Not a type node",0);
      return 0;
    }
  if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
	{
	  if (isTypeEquivalent(TYPE_BASE(type), comp))
	    {
	      return 1;
	    } 
	  return lookForTypeInType(TYPE_BASE(type),comp);
	}
    }
  return 0;
}

/***************************************************************************/
int replaceTypeInType(type,comp,new)
     PTR_TYPE type,comp,new;
{
  if (!type)
    return 0;
  if (!isATypeNode(TYPE_CODE(type)))
    {
      Message("replaceTypeInType; arg1 Not a type node",0);
      return 0;
    }
  if (hasTypeBaseType(TYPE_CODE(type))) 
    {
      if (TYPE_BASE(type))
	{
	  if (isTypeEquivalent(TYPE_BASE(type), comp))
	    {
	      TYPE_BASE(type) = new;
	      return 1;
	    } 
	  return replaceTypeInType(TYPE_BASE(type),comp,new);
	}
    }
  return 0;
}

/***************************************************************************/
void replaceTypeForSymb(symb, type, new)
PTR_SYMB symb;
PTR_TYPE type, new;
{
  PTR_TYPE ts;
  PTR_SYMB ptsymb;
  if (!symb || !type || !new) 
    return;
  
  if (!isATypeNode(TYPE_CODE(type)))
    {
      Message(" not a type node in replaceTypeForSymb",0);
      return;
    }
  if (!isASymbNode(SYMB_CODE(symb)))
    {
      Message(" not a symbol node in replaceTypeForSymb",0);
      return;
    }
  ts = SYMB_TYPE(symb);
  if (isTypeEquivalent(ts,type))
    {
      SYMB_TYPE(symb) = new;
    } else
      if (lookForTypeInType(ts,type))
	{
	  SYMB_TYPE(symb) = duplicateType(SYMB_TYPE(symb));
	  replaceTypeInType(SYMB_TYPE(symb),type, new);
	}
  /* look if have a param list */
  switch (SYMB_CODE(symb))
    {
    case MEMBER_FUNC:
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROCESS_NAME:
      ptsymb = SYMB_FUNC_PARAM (symb);
      while (ptsymb)
	{
	  replaceTypeForSymb(ptsymb,type,new);
	  ptsymb = SYMB_NEXT_DECL (ptsymb);
	}
      break;
    }
}

/***************************************************************************/
void replaceTypeInExpression(exprold, type, new)
     PTR_LLND exprold;
     PTR_TYPE type, new;
{
  /*  PTR_SYMB symb, newsymb;*/  /* podd 15.03.99*/

  if (!exprold || !type || !new)
    return;

  if (!isATypeNode(TYPE_CODE(type)))
    {
      Message(" not a type node in replaceTypeInExpression",0);
      return;
    }
  if (!isATypeNode(TYPE_CODE(new)))
    {
      Message(" not a type node in replaceTypeInExpression",0);
      return;
    }
  
  if (isTypeEquivalent(NODE_TYPE(exprold),type))
    {
      NODE_TYPE(exprold) = new;
    } else
      {
	if (lookForTypeInType(NODE_TYPE(exprold),type))
	  {
	    NODE_TYPE(exprold) = duplicateType(NODE_TYPE(exprold));
	    replaceTypeInType(NODE_TYPE(exprold),type,new);
	  }	  
      }

/*  if (hasNodeASymb(NODE_CODE(exprold))) do not do that it will alias some symbols not to be changes
    {
      if (symb = NODE_SYMB(exprold))
	{
	  replaceTypeForSymb(symb,type,new);
	}
    }*/

  replaceTypeInExpression(NODE_OPERAND0(exprold), type, new);
  replaceTypeInExpression(NODE_OPERAND1(exprold), type, new);
  
}


/***************************************************************************/
void replaceTypeInStmts(debut, fin, type, new)
     PTR_BFND debut, fin;
     PTR_TYPE type,new;
{
  PTR_BFND temp;
  
  for (temp = debut; temp ; temp = BIF_NEXT(temp))
    {
/*      if (BIF_SYMB(temp)) do not do that it will alias some symbols not to be changes
	{
	  replaceTypeForSymb(BIF_SYMB(temp),type,new);
	}*/
      replaceTypeInExpression(BIF_LL1(temp), type,new);
      replaceTypeInExpression(BIF_LL2(temp), type,new);
      replaceTypeInExpression(BIF_LL3(temp), type,new);
      if (fin && (temp == fin))
        break;
    }  
}

/* the following fonction are mainly dedicated to libcreatecollectionwithtype 
 used in the C++ library also with symb == NULL */
/***************************************************************************/
void replaceTypeUsedInStmt(symb,stmt,type,new)
     PTR_SYMB symb; /* symb is not to duplicate */
     PTR_BFND stmt;
     PTR_TYPE type,new;
{
  PTR_SYMB oldsymb;
  PTR_BFND cur,last,body;
  PTR_LLND ll1, ll2;
  if (!stmt)
    return;
  last = getLastNodeOfStmt(stmt);
  if (symb)
    replaceTypeForSymb(symb,type,new);
  replaceTypeInStmts(stmt,last,type,new);	  
  for (cur = stmt; cur ; cur =  BIF_NEXT(cur))
    {
      if (symb)
	{
	  if (isADeclBif(BIF_CODE(cur)) && (isInStmt(stmt,cur)))
	    { /* we have to declare what is declare there */
	      for (ll1= BIF_LL1(cur); ll1; ll1 = NODE_OPERAND1(ll1))
		{
		  ll2 = giveLlSymbInDeclList(NODE_OPERAND0(ll1));
		  if (ll2 && NODE_SYMB(ll2) && (NODE_SYMB(ll2) != symb))
		    {
		      oldsymb = NODE_SYMB(ll2);
		      /*symbol is declared here so change the type*/
		      replaceTypeForSymb(oldsymb,type,new);
		      /* apply recursively */
		      body = getBodyOfSymb(NODE_SYMB(ll2));
		      if (body && (!isInStmt(stmt,body)))
			{
			  replaceTypeUsedInStmt(NODE_SYMB(ll2),body,type,new);
			  replaceTypeInStmts(body,getLastNodeOfStmt(body),type,new);
			}
		    }
		}
	    }
	} else
	  { /* simpler we have just to look the stmt 
	       this is an replacement for everywhere */
	    if (isADeclBif(BIF_CODE(cur)))
	    { /* we have to declare what is declare there */
	      for (ll1= BIF_LL1(cur); ll1; ll1 = NODE_OPERAND1(ll1))
		{
		  ll2 = giveLlSymbInDeclList(NODE_OPERAND0(ll1));
		  if (ll2 && NODE_SYMB(ll2) && (NODE_SYMB(ll2) != symb))
		    {
		      oldsymb = NODE_SYMB(ll2);
		      /*symbol is declared here so change the type*/
		      replaceTypeForSymb(oldsymb,type,new);
		    }
		}
	    }
	  }
      if (cur == last)
	break;
    }
}

/***************************************************************************/
PTR_TYPE createDerivedCollectionType(col,etype)
     PTR_SYMB col;
     PTR_TYPE etype;
{
  PTR_TYPE  newtc;
  newtc =  (PTR_TYPE) newNode(T_DERIVED_COLLECTION); /*wasted*/
  TYPE_COLL_BASE(newtc) = etype;
  TYPE_SYMB_DERIVE(newtc) = col;
  return newtc;
}

/* the following function is not trivial 
   take a collection<type> and generate the right
   instance of the collection with name 
   collection_typename.
   replace the type in the new body by the right one 
   needs many duplication, not only 
   duplicate for the code, but also for symbol type and so on
   this function is presently use in the translator pc++2c++
   make basically an identical work as Templates........
    elemtype is going to replace elementtype;

    warning, all the symbol are not duplicated, expression are not duplicated too
    useless to to it for all (at least for the moment)
 */

/***************************************************************************/
PTR_BFND LibcreateCollectionWithType(colltype, elemtype)
     PTR_TYPE colltype, elemtype;
{
  PTR_SYMB coltoduplicate, copystruct,se = NULL;
  PTR_TYPE etype,newt,newtc;
  int len;
  char *newname;
  if (!colltype || !elemtype)
    return NULL;

  /* the symbol we are duplicating */
  coltoduplicate = TYPE_SYMB_DERIVE(colltype);
  etype = getDerivedTypeWithName("ElementType");
  if (!coltoduplicate || !etype)
    {
      Message("internal error in libcreatecollectionwithtype",0);
      return NULL;
    }
  if (TYPE_CODE(elemtype) == T_DERIVED_TYPE)
    {
      se = TYPE_SYMB_DERIVE(elemtype);
      if (!se)
	{
	  Message("The element type must be a class type-1",0);
	  exit(1);
	}
      if (!SYMB_TYPE(se))
	{
	  Message("The element type must be a class type-2",0);
	  exit(1);
	}
      if (SYMB_TYPE(se) && ((TYPE_CODE(SYMB_TYPE(se)) != T_CLASS)
	&& (TYPE_CODE(SYMB_TYPE(se)) != T_TECLASS)))
	{
	  Message("The element type must be a class type-3",0);
	  exit(1);
	}
    }
  /* look for element type is given by  iselementtype(type) */
  /* first we have to duplicate the code look at all the symbol */
  /* first duplicate the collection structure then we will do the methods 
     declare outside of the structure */
  copystruct = duplicateSymbolLevel2(coltoduplicate);
  if (!copystruct)
    Message("internal error in LibcreateCollectionWithType",0);

  /* duplicate at level 2 so must it is not necessary to do more 
   for duplicating */
  /* we have to set the new ID for the symbol according to the element type */
  len = strlen(SYMB_IDENT(copystruct)) + strlen(SYMB_IDENT(se))+10;
  newname = (char *) xmalloc(len);
  memset(newname, 0, len);
  sprintf(newname,"%s__%s",SYMB_IDENT(copystruct),SYMB_IDENT(se));

  SYMB_IDENT(copystruct) = newname;

  /* duplicate the symbol declared inside so we can attach a new type eventually */
  duplicateAllSymbolDeclaredInStmt(copystruct, getBodyOfSymb(copystruct),SYMB_IDENT(coltoduplicate));

  /* the collection body and the method have been duplicated no we have to replace the type */
  /* first replace element type */
  replaceTypeUsedInStmt(copystruct, getBodyOfSymb(copystruct),etype,elemtype);

  /* now replace type like DistributedArray<ElementType> but first construct the new type
   corresponding to that */
  newt = (PTR_TYPE) newNode(T_DERIVED_CLASS);
  TYPE_SYMB_DERIVE(newt) = copystruct;
  /* need to create a type for reference */
  newtc =  createDerivedCollectionType(coltoduplicate,etype);
  replaceTypeUsedInStmt(copystruct, getBodyOfSymb(copystruct),newtc,newt);

  /* replacing DistributedArray<vector> for instance  is done elsewhere*/
  return getBodyOfSymb(copystruct);
}

/***************************************************************************/
int LibisMethodOfElement(symb)
     PTR_SYMB symb;
{
  if (!symb) return FALSE;
  if ((int) SYMB_ATTR(symb) & (int) ELEMENT_FIELD)
    return TRUE;
  else
    return FALSE;
}

/***************************************************************************/
PTR_BFND LibfirstElementMethod(coll)
     PTR_BFND coll;
{
  PTR_BFND pt,last;
  PTR_SYMB symb;
  PTR_LLND ll;
   if (!coll )
    return NULL;
  last = getLastNodeOfStmt(coll);
  for (pt = coll; pt && (pt != BIF_NEXT(last)); pt = BIF_NEXT(pt))
    {
      if (isADeclBif(BIF_CODE(pt))
	  && (BIF_CP(pt) == coll))
	{
	  ll = giveLlSymbInDeclList(BIF_LL1(pt));
	  if (ll && NODE_SYMB(ll))
	    {
	      symb = NODE_SYMB(ll);
	      if (LibisMethodOfElement(symb))
		return pt;
	    }
	}
    }
  return NULL;
}


/***************************************************************************/
int buildLinearRep(exp,coef,symb,size,last)
     PTR_LLND exp;
     int *coef;
     PTR_SYMB *symb;
     int size;
     int *last;    
{
  return buildLinearRepSign(exp,coef,symb,size, last,1,1);
}


/* initialy coeff are 0, return 1 if Ok, 0 if abort*/
/***************************************************************************/
int buildLinearRepSign(exp,coef,symb,size, last,sign,factor)
     PTR_LLND exp;
     int *coef;
     PTR_SYMB *symb;
     int size;
     int *last;  
     int sign;     
     int factor;
{
  int code;
  int i, *res1,*res2;
  
  if (!exp)
    return TRUE;
  
  code = NODE_CODE(exp);
  switch (code)
    {
    case VAR_REF:
      for (i=0; i< size; i++)
        {
          if (NODE_SYMB(exp) == symb[i])
            {
              coef[i] = coef[i] + sign*factor;
              return TRUE;
            }
        }
      return FALSE;
     
    case SUBT_OP:
      if (!buildLinearRepSign(NODE_OPERAND0(exp),coef,symb,size,last,sign,factor))
        return FALSE;
      if (!buildLinearRepSign(NODE_OPERAND1(exp),coef,symb,size,last,-1*sign,factor))
        return FALSE;
      break; 
    case ADD_OP:
      if (!buildLinearRepSign(NODE_OPERAND0(exp),coef,symb,size,last,sign,factor))
        return FALSE;
      if (!buildLinearRepSign(NODE_OPERAND1(exp),coef,symb,size,last,sign,factor))
        return FALSE;
      break;
    case MULT_OP:
      res1 = evaluateExpression (NODE_OPERAND0(exp));
      res2 = evaluateExpression (NODE_OPERAND1(exp));
      if ((res1[0] != -1) && (res2[0] != -1))
        {
          *last = *last + factor*sign*(res1[1]*res2[1]);
        } else
          { 
            int found;
            if (res1[0] != -1) 
              {
                /* la constante est le fils gauche */
                if (NODE_CODE(NODE_OPERAND1(exp)) != VAR_REF)
                  return  buildLinearRepSign(NODE_OPERAND1(exp),coef,symb,size, last,sign,res1[1]*factor);
                found = 0;
                for (i=0; i< size; i++)
                  {
                    if (NODE_SYMB(NODE_OPERAND1(exp)) == symb[i])
                      {
                        coef[i] = coef[i] + factor*sign*(res1[1]);
                        found = 1;
                        break;
                      }
                  }
                if (!found) return FALSE;
              } else
                if (res2[0] != -1) 
                  {
                    /* la constante est le fils droit */
                    if (NODE_CODE(NODE_OPERAND0(exp)) != VAR_REF)
                      return  buildLinearRepSign(NODE_OPERAND0(exp),coef,symb,size, last,sign,res2[1]*factor);
                    found =0;
                    for (i=0; i< size; i++)
                      {
                        if (NODE_SYMB(NODE_OPERAND0(exp)) == symb[i])
                          {
                            coef[i] = coef[i] + factor*sign*(res2[1]);
                            found = 1;
                            break;
                          }
                      }
                    if (!found) return FALSE;
                  } else                    
                    return FALSE;
          }
      break;
    case INT_VAL:
      *last = *last + factor*sign*(NODE_INT_CST_LOW(exp));
      break;
    default:
      
      return FALSE;
    }
  return TRUE;
}


/********************** FB ADDED JULY 94 ***********************
 * ALLOW TO COPY A FULL SYMBOL ACCROSS FILE                    *
 * THIS IS A FRAGILE FUNCTION BE CAREFUL WITH IT               *
 ***************************************************************/


void resetDoVarForSymb()
{
  PTR_FILE ptf, saveptf;
  PTR_BLOB ptb;
  /* PTR_BFND tmp;*/ /* podd 15.03.99*/
  PTR_SYMB tsymb;

  saveptf = pointer_on_file_proj;
  for (ptb = PROJ_FILE_CHAIN (cur_proj); ptb ; ptb =  BLOB_NEXT (ptb))
    {
      ptf = (PTR_FILE) BLOB_VALUE (ptb);
      cur_file = ptf;      
      /* reset the toolbox and pointers*/
      Init_Tool_Box();
      for (tsymb = PROJ_FIRST_SYMB() ; tsymb; tsymb = SYMB_NEXT(tsymb))
        {
          tsymb->dovar = 0;
        }
    }
  cur_file = saveptf;
  Init_Tool_Box();
}


void updateTypesAndSymbolsInBody(symb, stmt, where)
     PTR_BFND stmt, where;
     PTR_SYMB symb;
{
  PTR_SYMB oldsymb, newsymb, param;
  PTR_BFND cur,last;
  PTR_LLND ll1, ll2;
  PTR_TYPE type,new;
  int isparam;
  if (!stmt)
    return;
  last = getLastNodeOfStmt(stmt);
  for (cur = stmt; cur ; cur =  BIF_NEXT(cur))
    {
      if (isADeclBif(BIF_CODE(cur)))
        { /* we have to declare what is declare there */
          for (ll1= BIF_LL1(cur); ll1; ll1 = NODE_OPERAND1(ll1))
            {
              ll2 = giveLlSymbInDeclList(NODE_OPERAND0(ll1));
              if (ll2 && NODE_SYMB(ll2) && (NODE_SYMB(ll2) != symb))
                {
                  oldsymb = NODE_SYMB(ll2);
                  if (oldsymb != symb)
                    {
                      /* should check for param since already propagated
                         needs TO BE WRITTEN EXPRESSION?????? */
                       param = SYMB_FUNC_PARAM (symb);
                       isparam = 0;
                       while (param)
                         {
                           if (param == oldsymb )
                             {
                               isparam = 1;
                               break;
                             }
                           param  = SYMB_NEXT_DECL (param );
                         }
                       if (! isparam)
                         {
                           newsymb = duplicateSymbolAcrossFiles(oldsymb, where);
                           SYMB_SCOPE(newsymb) = stmt;
                           type = SYMB_TYPE(oldsymb);
                           new = duplicateTypeAcrossFiles(type);
                           SYMB_TYPE(newsymb) = new;
                           replaceTypeInStmts(stmt, last, type, new);
                           replaceSymbInStmts(stmt,last,oldsymb,newsymb);
                         }
                    }
                }
            }
        }
      if (cur == last)
        break;
    }
}



PTR_SYMB duplicateSymbolAcrossFiles(symb, where)
     PTR_SYMB symb;
     PTR_BFND where;
{
  PTR_SYMB  newsymb;
  PTR_BFND body,newbody,last,before,cp;
  PTR_SYMB ptsymb,ptref;
  if (!symb)
    return NULL;

  if (!isASymbNode(NODE_CODE(symb)))
    {
      Message("duplicateSymbolAcrossFiles; Not a symbol node",0);
      return NULL;
    }
  if (symb->dovar)
    {
      /* already duplicated don't do it again */
      return symb;
    }
  newsymb = duplicateSymbolLevel1(symb);
  newsymb->dovar = 1;
  symb->dovar = 1;
  /* need a function resetDovar for all files and all symb to be called before*/
  SYMB_SCOPE(newsymb) = where;
  /* to be updated later Not that simple*/
  switch (SYMB_CODE(symb))
    {
    case MEMBER_FUNC:
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROCESS_NAME:
      /* find the body in the right file????*/
      body = getBodyOfSymb(symb);
      if (body)
	{
	  before = getNodeBefore(body);
	  cp = BIF_CP(body);
	  last = getLastNodeOfStmt(body);
	  newbody = duplicateStmtsNoExtract(body);
          if (BIF_CODE (where) == GLOBAL)
            insertBfndListIn (newbody, where,where);
          else
            insertBfndListIn (newbody, where,BIF_CP(where));
	  BIF_SYMB(newbody) = newsymb;
	  SYMB_FUNC_HEDR(newsymb) = newbody;
	  /* we have to propagate change in the param list in the new body */
	  ptsymb = SYMB_FUNC_PARAM (newsymb);
	  ptref =  SYMB_FUNC_PARAM (symb);
	  last = getLastNodeOfStmt(newbody);
	  while (ptsymb)
	    {
              SYMB_SCOPE(ptsymb) = newbody;
	      replaceSymbInStmts(newbody,last,ptref,ptsymb);
	      ptsymb = SYMB_NEXT_DECL (ptsymb);
	      ptref = SYMB_NEXT_DECL (ptref);
	    }
          /* update the all the symbol and type used in the statement */
          updateTypesAndSymbolsInBody(newsymb,newbody, where);
/*          printf(">>>>>>>>>>>>>>>>>>>>>>\n");
          UnparseProgram(stdout);
          printf("<<<<<<<<<<<<<<<<<<<<<<\n");*/
	}
      break;
    case TECLASS_NAME:
    case CLASS_NAME:
    case COLLECTION_NAME:
    case STRUCT_NAME:
    case UNION_NAME:
      body = getBodyOfSymb(symb);
      if (body)
	{
	  cp = BIF_CP(body);/*podd 12.03.99*/
          before = getNodeBefore(body);/*podd 12.03.99*/
          newbody = duplicateStmtsNoExtract(body);       
	  insertBfndListIn (newbody, before,cp);
	  BIF_SYMB(newbody) = newsymb;
	  /* probably more to do here */
	  SYMB_TYPE(newsymb) = duplicateTypeAcrossFiles(SYMB_TYPE(symb));
	  /* set the new body for the symbol */
	  TYPE_COLL_ORI_CLASS(SYMB_TYPE(newsymb)) = newbody;
          updateTypesAndSymbolsInBody(newsymb,newbody, where);
	}
      break;
    }
  return newsymb;
}
/*-----------------------------------------------------------------*/
/*podd 20.03.07*/

void updateExpression(exp, symb, newsymb)
 PTR_LLND exp;
 PTR_SYMB symb, newsymb;
{
  PTR_SYMB param,newparam;
  param    = SYMB_FUNC_PARAM (symb);
  newparam = SYMB_FUNC_PARAM (newsymb);
  while(param)
  {
    replaceSymbInExpression(exp,param, newparam);
    param=SYMB_NEXT_DECL(param);
    newparam=SYMB_NEXT_DECL(newparam);
  }
}

/*podd 06.06.06*/
void updateTypeAndSymbolInStmts(PTR_BFND stmt, PTR_BFND last, PTR_SYMB oldsymb, PTR_SYMB newsymb)
{
   PTR_TYPE type, new;

   type = SYMB_TYPE(oldsymb);
   new = duplicateTypeAcrossFiles(type);
   SYMB_TYPE(newsymb) = new;
   replaceTypeInStmts(stmt, last, type, new);
   replaceSymbInStmts(stmt, last, oldsymb, newsymb);
}

/*podd 26.02.19*/
void replaceSymbByNameInExpression(PTR_LLND exprold, PTR_SYMB new)
{  
   if(!exprold)
      return;
   if (hasNodeASymb(NODE_CODE(exprold)))
   {    
      if ( !strcmp(SYMB_IDENT(NODE_SYMB(exprold)), new->ident) )
         NODE_SYMB(exprold) = new; 
   }
   replaceSymbByNameInExpression(NODE_OPERAND0(exprold), new);
   replaceSymbByNameInExpression(NODE_OPERAND1(exprold), new);
}

/*podd 26.02.19*/
void replaceSymbByNameInConstantValues(PTR_SYMB first_const_name, PTR_SYMB new)
{
   PTR_SYMB s;
   for (s=first_const_name; s; s = SYMB_LIST(s))
   {   
      replaceSymbByNameInExpression (SYMB_VAL(s),new); 
   }
}
/*podd 26.02.19*/
void updateConstantSymbolsInParameterValues(PTR_SYMB first_const_name)
{
   PTR_SYMB symb, prev_symb;
   for (symb=first_const_name; symb; symb = SYMB_LIST(symb))
   {  
      replaceSymbByNameInConstantValues(first_const_name,symb);
   }
   
   symb=first_const_name;
   while (symb)
   {
      prev_symb = symb;
      symb = SYMB_LIST(symb);
      SYMB_LIST(prev_symb) = SMNULL;  
   }   
}

/*podd 26.02.19*/
void replaceSymbInType(PTR_TYPE type, PTR_SYMB newsymb)
{  
  if (!type)
    return;
  
  if (!isATypeNode(NODE_CODE(type)))
  {
      Message("duplicateTypeAcrossFiles; Not a type node",0);
      return ;
  }

  if (isAtomicType(TYPE_CODE(type)))
  {
     replaceSymbByNameInExpression(TYPE_RANGES(type),newsymb);
     replaceSymbByNameInExpression(TYPE_KIND_LEN(type),newsymb);
  }

  if (hasTypeBaseType(TYPE_CODE(type))) 
     replaceSymbInType(TYPE_BASE(type), newsymb);
   

  if ( TYPE_CODE(type) == T_ARRAY)
     replaceSymbByNameInExpression(TYPE_RANGES(type),newsymb);
}

/*podd 26.02.19*/
void replaceSymbInTypeOfSymbols(PTR_SYMB newsymb,PTR_SYMB first_new)
{
   PTR_SYMB symb;
   for( symb=first_new; symb; symb = SYMB_NEXT(symb) )
      replaceSymbInType(SYMB_TYPE(symb),newsymb);
}

/*podd 26.02.19*/
void updatesSymbolsInTypeExpressions(PTR_BFND new_stmt)
{
   PTR_SYMB symb, first_new;
   first_new= BIF_SYMB(new_stmt);
   for( symb=first_new; symb; symb = SYMB_NEXT(symb)) 
      replaceSymbInTypeOfSymbols(symb,first_new);
}

void updateTypesAndSymbolsInBodyOfRoutine(PTR_SYMB symb, PTR_BFND stmt, PTR_BFND new_stmt, PTR_BFND where)
{
    PTR_SYMB oldsymb, param, newsymb, until, const_list, first_const_name;
    PTR_BFND last;
    PTR_TYPE type;
    
    if (!stmt)
        return;
    const_list = first_const_name = SMNULL; /* to make a list of constant names  */

    last = getLastNodeOfStmt(stmt);

    if (BIF_NEXT(last) && BIF_CODE(BIF_NEXT(last)) != COMMENT_STAT)
        until = BIF_SYMB(BIF_NEXT(last));
    else
        until = SYMB_NEXT(last_file_symbol);    //last_file_symbol is last symbol of source file's Symbol Table

    param = SYMB_FUNC_PARAM(symb);
    for (oldsymb = SYMB_NEXT(symb); oldsymb && oldsymb != until; oldsymb = SYMB_NEXT(oldsymb))
    {
        if (SYMB_SCOPE(oldsymb) == stmt)
        {              
            if (SYMB_TEMPLATE_DUMMY1(oldsymb) != IO)  /*is not a dummy parameter */
            {
                newsymb = duplicateSymbolOfRoutine(oldsymb, where);
                if(SYMB_CODE(newsymb)==CONST_NAME) 
                {
                    if(first_const_name == SMNULL)
                    {
                       first_const_name = const_list = newsymb;
                       newsymb->id_list = SMNULL;
                    }
                    const_list->id_list = newsymb;
                    newsymb->id_list = SMNULL;
                    const_list = newsymb;
                }

                SYMB_SCOPE(newsymb) = new_stmt;
                updateTypeAndSymbolInStmts(new_stmt, getLastNodeOfStmt(new_stmt), oldsymb, newsymb);
            }
        }
    }
    updateConstantSymbolsInParameterValues(first_const_name); /*podd 26.02.19*/
    updatesSymbolsInTypeExpressions(new_stmt);                /*podd 26.02.19*/

}

PTR_SYMB duplicateSymbolOfRoutine(PTR_SYMB symb, PTR_BFND where)
{
    PTR_SYMB newsymb;
    PTR_BFND body, newbody, last, before, cp;
    PTR_SYMB ptsymb, ptref;
    
    if (!symb)
        return NULL;

    if (!isASymbNode(NODE_CODE(symb)))
    {
        Message("duplicateSymbolAcrossFiles; Not a symbol node", 0);
        return NULL;
    }
    
    newsymb = duplicateSymbolLevel1(symb);
    newsymb->dovar = 1;
    symb->dovar = 1;
    /* need a function resetDovar for all files and all symb to be called before*/
    SYMB_SCOPE(newsymb) = where;

    /* to be updated later Not that simple*/
    switch (SYMB_CODE(symb))
    {
    case FUNCTION_NAME:
    case PROCEDURE_NAME:
    case PROGRAM_NAME:

        body = getBodyOfSymb(symb);
        if (body && (cp = BIF_CP(body)) && cp->variant != INTERFACE_STMT && cp->variant != INTERFACE_ASSIGNMENT && cp->variant != INTERFACE_OPERATOR) /*20.04.17*/
        {
            before = getNodeBefore(body);
            last = getLastNodeOfStmt(body);
            newbody = duplicateStmtsNoExtract(body);
            if (where)
            {
                if (BIF_CODE(where) == GLOBAL)
                    insertBfndListIn(newbody, where, where);
                else
                    insertBfndListIn(newbody, where, BIF_CP(where));
            }
            BIF_SYMB(newbody) = newsymb;
            if(SYMB_CODE(newsymb) == PROGRAM_NAME)
               newsymb->entry.prog_decl.prog_hedr = newbody;
            else 
               SYMB_FUNC_HEDR(newsymb) = newbody;
            last = getLastNodeOfStmt(newbody);
            updateTypeAndSymbolInStmts(newbody, last, symb, newsymb);
            
            /* we have to propagate change in the param list in the new body */
            if(SYMB_CODE(newsymb) == PROGRAM_NAME)
               ptsymb = ptref = SMNULL;
            else
            {
               ptsymb = SYMB_FUNC_PARAM(newsymb);
               ptref  = SYMB_FUNC_PARAM(symb);
            }
            while (ptsymb)
            {
                SYMB_SCOPE(ptsymb) = newbody;
                updateTypeAndSymbolInStmts(newbody, last, ptref, ptsymb);
                ptsymb = SYMB_NEXT_DECL(ptsymb);
                ptref  = SYMB_NEXT_DECL(ptref);
            }
            /* update the all the symbol and type used in the statement */
            updateTypesAndSymbolsInBodyOfRoutine(symb, body, newbody, where);

                        /*          printf(">>>>>>>>>>>>>>>>>>>>>>\n");
                                    UnparseProgram(stdout);
                                    printf("<<<<<<<<<<<<<<<<<<<<<<\n");     */
        }
        break;
    }
    return newsymb;
}
