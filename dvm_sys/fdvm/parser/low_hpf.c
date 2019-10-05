
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
#define HPF_STRING 2

/*int debug =NO;  used in db.c*/

PTR_FILE pointer_on_file_proj;
/*static int number_of_bif_node = 0;*/
/*int number_of_ll_node = 0; this counters are useless anymore */
/*static int number_of_symb_node  = 0;
static int number_of_type_node = 0;*/
char  *default_filename;
int Warning_count = 0;

/* FORWARD DECLARATIONS (phb) */
void Message();
char * filter();
int *evaluateExpression();

extern int write_nodes();
extern char* Tool_Unparse2_LLnode(); 
extern void Init_HPFUnparser();
extern char* Tool_Unparse_Bif ();
extern char* Tool_Unparse_Type();

#define MAXFIELDSYMB 10
#define MAXFIELDTYPE 10
/* records propoerties and type of node */
char node_code_type[LAST_CODE];
/* Number of argument-words in each kind of tree-node.  */
int node_code_length[LAST_CODE];
enum typenode node_code_kind[LAST_CODE];
/* special table for infos on type and symbol */
char info_type[LAST_CODE][MAXFIELDTYPE];
char info_symb[LAST_CODE][MAXFIELDSYMB];
char general_info[LAST_CODE][MAXFIELDSYMB];

/*static struct bif_stack_level   *stack_level = NULL;
static struct bif_stack_level *current_level = NULL;*/

PTR_BFND  getFunctionHeader();

/*****************************************************************************
 *                                                                           *
 *                   Procedure of general use                                *
 *                                                                           *
 *****************************************************************************/

/* Modified to return a pointer (64bit clean) (phb) */
/***************************************************************************/
char* xmalloc(size)
     int size;
{
  char *val;
  val = (char *) malloc (size);
  
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
char* mymalloc(size)
int size;
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
  if (!pt1)
    {
      Message("sorry : out of memory\n",0);
      exit(1);
    }
 
  pt2 = (ptchaining) malloc(sizeof(struct chaining));
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
      Message("hasTypeBaseType not applied to a type node",0);
      return 0;
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
      Message("isStructType not applied to a type node",0);
      return 0;
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
      Message("isPointerType not applied to a type node",0);
      return 0;
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
      Message("isUnionType not applied to a type node",0);
      return 0;
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
      Message("EnumType not applied to a type node",0);
      return 0;
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
      Message("hasTypeSymbol not applied to a type node",0);
      return 0;
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
      Message("isAtomicType not applied to a type node",0);
      return 0;
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
      Message("hasNodeASymb not applied to a bif or low level node",0);
      return 0;
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
      Message("isNodeAConst not applied to a bif or low level node",0);
      return 0;
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
      Message("isAStructDeclBif not applied to a bif",0);
      return 0;
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
      Message("isAUnionDeclBif not applied to a bif",0);
      return 0;
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
      Message("isAEnumDeclBif not applied to a bif",0);
      return 0;
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
      Message("isADeclBif not applied to a bif",0);
      return 0;
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
      Message("isAControlEnd not applied to a bif",0);
      return 0;
    }
  if (general_info[variant][0] == 'c')
    return TRUE;
  else
    return FALSE;
}


/***************************************************************************/
void Message(s, l)
char *s;
int l;
{
  if (l != 0)
    fprintf(stderr,"Warning : %s line %d\n",s, l);
  else
    fprintf(stderr,"Warning : %s\n",s);
 Warning_count++;
}
/***************************************************************************/
int Check_Lang_Fortran(proj)
PTR_PROJ proj;
{
  PTR_FILE ptf;
  PTR_BLOB ptb;
  /* Change FALSE to TRUE */
  if (!proj)
    return TRUE;
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


/***************************************************************************/
char* filter(s)
char *s;
{
  char c;
  int i = 1;
  char temp[1024];
  int temp_i = 0;
  int buf_i = 0;
  int commentline = 0;
  char *resul, *init;
  
  if (!s) return NULL;
  if (strlen(s)==0) return s;

  /* allocate very simple, but to redo later, allocate two times the size */
  make_a_malloc_stack();
  resul = (char *) mymalloc((int)(2*strlen(s)));
  memset(resul, 0, 2*strlen(s));
  init = resul;
  /* find the separator */
  c = s[0];

  if ((c != ' ')
      &&(c != '\n')
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
      if ( c=='!')  commentline = HPF_STRING;
      else commentline = 1;
  else
    commentline = 0;

  temp_i = 0;
  i = 0;
  buf_i =0;
  while (c!='\0')
    {
      c = s[i];
      temp[ buf_i] = c;
      if (c == '\n')
        {
          temp[ buf_i+1] = '\0';
          sprintf(resul,"%s",temp);
          resul = resul + strlen(temp);
          temp_i = -1;
          buf_i = -1;
          if ((s[i+1] != ' ')
              &&(s[i+1] != '\n')
              && (s[i+1] != '0')
              && (s[i+1] != '1') 
              && (s[i+1] != '2') 
              && (s[i+1] != '3') 
              && (s[i+1] != '4') 
              && (s[i+1] != '5') 
              && (s[i+1] != '6') 
              && (s[i+1] != '7') 
              && (s[i+1] != '8') 
              && (s[i+1] != '9'))
	      if (s[i+1] == '!')  commentline = HPF_STRING;
	      else commentline = 1;
          else
            commentline = 0;
        } else
          {
            if ((temp_i == 71) && !commentline)
              { 
                /* insert where necessary */
                temp[ buf_i+1]  = '\0';
                sprintf(resul,"%s\n",temp);
                resul = resul + strlen(temp)+1;
                sprintf(resul,"     +");
                resul = resul + strlen("     +");
                commentline = 0;
                memset(temp, 0, 1024);
                temp_i = strlen("     +")-1;
                buf_i = -1;
              }
	    if ((temp_i == 71) && (commentline==HPF_STRING))
              { 
                /* insert where necessary */
		int count=0;
		for(;s[i]!='$';i--,count++)
            	    {
		    if (strncmp(&(s[i]),"ONTO", strlen("ONTO"))== 0)
			break;
		    if (strncmp(&(s[i]),"BEGIN", strlen("BEGIN"))== 0)
			break;
		    if (strncmp(&(s[i]),"ON", strlen("ON"))== 0)
			{
			i+=3;count-=3;		
			break;
			}
		    if (strncmp(&(s[i]),"WITH", strlen("WITH"))== 0)
			break;
		    if (strncmp(&(s[i]),"NEW", strlen("NEW"))== 0)
			break;
		    if (strncmp(&(s[i]),"REDUCTION", strlen("REDUCTION"))== 0)
			break;
		    if (strncmp(&(s[i]),"TEMPLATE", strlen("TEMPLATE"))== 0)
			break;
		    if (strncmp(&(s[i]),"SHADOW", strlen("SHADOW"))== 0)
			break;
		    if (strncmp(&(s[i]),"INHERIT", strlen("INHERIT"))== 0)
			break;
		    if (strncmp(&(s[i]),"DYNAMIC", strlen("DYNAMIC"))== 0)
			break;
		    if (strncmp(&(s[i]),"DIMENSION", strlen("DIMENSION"))== 0)
			break;
		    if (strncmp(&(s[i]),"PROCESSORS", strlen("PROCESSORS"))== 0)
			break;
		    if (strncmp(&(s[i]),"DISTRIBUTE", strlen("DISTRIBUTE"))== 0)
			break;
		    if (strncmp(&(s[i]),"ALIGN", strlen("ALIGN"))== 0)
			break;
		    if (strncmp(&(s[i]),"::", strlen("::"))== 0)
			{
			/*i+=3;count-=3;*/
			break;
			}
		    }
		i--;count++;      
		if (count<36)
		    temp[ buf_i+1-count]  = '\0';
		else 
		    {
		    i+=count;
		    temp[ buf_i+1]  = '\0';
		    }
                sprintf(resul,"%s\n",temp);
                resul = resul + strlen(temp)+1;
                sprintf(resul,"!HPF$*");
                resul = resul + strlen("!HPF$*");
                /*8commentline = 0;*/
                memset(temp, 0, 1024);
                temp_i = strlen("!HPF$*")-1;
                buf_i = -1;
              }
          }
      i++;
      temp_i++;
      buf_i++;
    }

  return init;  
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
  
  pt1 = (PTR_SYMB  *) xmalloc((int)( sizeof(PTR_SYMB  *) * 
			      (NbValues + ALLOCATECHUNKVALUE)));
  pt2 =  (int *) xmalloc((int)( sizeof(int  *) * (NbValues + ALLOCATECHUNKVALUE)));
  
  for (i=0; i<NbValues + ALLOCATECHUNKVALUE; i++) {
    pt1[i] = NULL;
    pt2[i] = 0;
  }
  
  for (i=0 ; i < NbValues; i++) {
    pt1[i] = ValuesSymb[i];
    pt2[i] = ValuesInt[i];
  }
  
  if (NbValues) {
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
  if (ValuesSymb)
    free(ValuesSymb);
  if (ValuesInt)
    free(ValuesInt);
  ValuesSymb = NULL;
  ValuesInt = NULL;
}

/***************************************************************************/
int* evaluateExpression(expr)
     PTR_LLND expr;
{
  int *res, *op1, *op2;
  
  res = (int *) xmalloc((int)(2 * sizeof(int)));
  memset((char *) res, 0, 2 * sizeof (int));
  op1 = (int *) xmalloc((int)(2 * sizeof(int)));
  memset((char *)op1, 0, 2 * sizeof (int));
  op2 = (int *) xmalloc((int)(2 * sizeof(int)));
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
      free(op1); free(op2);
      return res;
    case ADD_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] + op2 [1];
      free(op1); free(op2);
      return res;
   case MULT_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] * op2 [1];
      free(op1); free(op2);
      return res;
   case SUBT_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] - op2 [1];
      free(op1); free(op2);
      return res;
   case DIV_OP :
        op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] / op2 [1];
      free(op1); free(op2);
      return res;
   case MOD_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] % op2 [1];
      free(op1); free(op2);
      return res;
   case EXP_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      op2  = evaluateExpression (NODE_OPERAND1 (expr));
      if ((op1 [0] == -1) || (op2 [0] == -1))
        res [0] = -1;
      else
        res [1] = op1 [1] ^ op2 [1];
      free(op1); free(op2);
      return res;

    case MINUS_OP :
      op1  = evaluateExpression (NODE_OPERAND0 (expr));
      if (op1 [0] == -1)
        res [0] = -1;
      else
        res [1] = - op1 [1];      
      free(op1);
      return res;
    case VAR_REF: /* assume here that some value for Symbole are given*/
      {
        int ind;
        if ((ind = getElementEvaluate(NODE_SYMB(expr))) != -1)
          {
             res [1] = ValuesInt[ind];
             free(op1); free(op2);
             return res;
           } else
             {
                res [0] = -1;
                free(op1); free(op2);
                return res;
             }
        break;
      }
   default :
     res [0] = -1;
      free(op1); free(op2);
      return res;
    }
    return res;
}

/***************************************************************************/
int patternMatchExpression(ll1,ll2)
     PTR_LLND ll1,ll2;
{
  int *res1, *res2;
  
  if (ll1 == ll2)
    return TRUE;
  
 if (!ll1 || !ll2)
   return FALSE;

  if (NODE_CODE(ll1) != NODE_CODE(ll2))
    return FALSE;
  /* first test if constant equations identical */
  res1 = evaluateExpression(ll1);
  res2 = evaluateExpression(ll2);
  if ((res1[0] != -1) &&
      (res2[0] != -1) &&
      (res1[1] == res2[1]))
    {
      free(res1);
      free(res2);
      return TRUE;
    }
  if ((res1[0] != -1) && (res2[0] == -1))
    {
      free(res1);
      free(res2);
      return FALSE;
    }
  if ((res1[0] == -1) && (res2[0] != -1))
    {
      free(res1);
      free(res2);
      return FALSE;
    }
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

