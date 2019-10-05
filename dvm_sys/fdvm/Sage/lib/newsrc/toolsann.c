/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993,1995             */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/**************************************************************************
* * * Annotation toolbox for Sigma * * * * *
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "compatible.h"   /* Make different system compatible... (PHB) */
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#include "macro.h"
#include "ext_lib.h"
#include "ext_low.h"

#define ASYMBOLEXT   "_%d_"    /* must have a %d field for number */
#define MAX_ANNOTATION 10000
#define ForCOMMENTSTART "C$ann\0"  /* For fortran Must start with big C */
#define ForCOMMENTCONT  "C$cont\0" /* idem */
#define C_COMMENTSTART "//$ann\0"  /* For C Must start with big / */
#define C_COMMENTCONT  "-+-++++--\0" /* not in C */

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
extern void removeFromCollection(void *pointer);
#endif

int TRACEANN = 0;

/* Assertion Tab */

extern int Number_of_proc;
extern PTR_FILE pointer_on_file_proj;
extern PTR_LLND ANNOTATE_NODE;
extern char *STRINGTOPARSE;
extern int  LENSTRINGTOPARSE;
extern int PTTOSTRINGTOPARSE;
extern PTR_BFND ANNOTATIONSCOPE;
extern PTR_TYPE global_int_annotation;
extern char AnnExTensionNumber[];

/* FORWARD DECLARATION */
int Get_Scope_Of_Annotation();
void Propagate_defined_value();
int Set_The_Define_Field();
char *Unparse_Annotation();
PTR_LLND Parse_Annotation();


char *
Remove_Ann_Cont(str)
char *str;
{
  int i =0;
  int j;

  if (str == NULL)
    return NULL;

  if (Check_Lang_Fortran(cur_proj))
    { /* does not apply to C */
      while (str[i] != '\0')
	{
	  if (str[i] == 'C')
	    {
	      if (strncmp(&(str[i]),ForCOMMENTCONT,strlen(ForCOMMENTCONT)) == 0)
		{
		  for (j = 0; j < (int)strlen(ForCOMMENTCONT); j++)
		    str[i+j] = ' ';
		  i = i+j;
		}
	    }
	  i++;
	}
    }
  return str;
}


/* Init annotation System, mainly gathers annotation */
/* we use array to store annotation can be modify to count the size and alloc
   things */

static char    *Annotation_PT[MAX_ANNOTATION];    /* the string        */
static PTR_BFND Annotation_BIFND[MAX_ANNOTATION]; /* the bif node next */
PTR_LLND Annotation_LLND[MAX_ANNOTATION];  /* result of unparse */
static PTR_CMNT Annotation_CMNT[MAX_ANNOTATION];  /* to the comment */
static int      Annotation_Def[MAX_ANNOTATION];   /* is it define      */
static int      Nb_Annotation;             /* number of annotation found */
static char    *Defined_Value_Str[MAX_ANNOTATION];
static int     Defined_Value_Value[MAX_ANNOTATION];

/*     Indicate if comment is an annotation */
Is_Annotation(str)
char *str;
{
  
  if (!str)
    return FALSE;

  if (Check_Lang_Fortran(cur_proj))
    { 
      if (strncmp(ForCOMMENTSTART,str, strlen(ForCOMMENTSTART)) == 0)
	return TRUE;
      else
	return FALSE;
    } else
      {
	if (strncmp(C_COMMENTSTART,str, strlen(C_COMMENTSTART)) == 0)
	  return TRUE;
	else
	  return FALSE;
      }
}

Is_Annotation_Cont(str)
char *str;
{
  
  if (!str)
    return FALSE;

  if (!Check_Lang_Fortran(cur_proj))
    return FALSE;
  if (strncmp(ForCOMMENTCONT,str, strlen(ForCOMMENTCONT)) == 0)
    return TRUE;
  else
    return FALSE;
}


char *
Get_Annotation_String(str)
char * str;
{
  char * pt, *pt1;
  int i,goahead;
  char * stra = NULL;
  pt = str;
  
  if (!str)
    return NULL;

  while((*pt != '\0') && (*pt != '['))
    {
      pt++;
    }
  if (*pt != '[')
    Message("Annotation failed",0);
  /* count the length */
  pt1 = pt;
  i = 0;
  goahead = TRUE;
  while(goahead)
    {
      goahead = FALSE;
      while((*pt1 != '\0') && (*pt1 != '\n'))
	{
	  pt1++;
	  i++;
	}

      if (*pt1 != '\0')
	{
	  if (Is_Annotation_Cont(pt1+1))
	    {
	      goahead = TRUE;
	      pt1++;
	      i++;
	    }
	}
    }
  if (i > 1024)
    {      
      stra = (char *) xmalloc(i+2);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,stra, 0);      
#endif
      memset(stra, 0, i+2);
    }
  else
    {      
      stra = (char *) xmalloc(1024);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,stra, 0);
#endif
      memset(stra, 0,1024);
    }
  strncpy(stra,pt,i);
  stra = Remove_Carriage_Return(stra);
  stra = Remove_Ann_Cont(stra);
  return stra;
}

/* basically got to the carriage return */
char *
Get_to_Next_Annotation_String(str)
char *str;
{
  char * pt;
  pt = str;
  if (!Check_Lang_Fortran(cur_proj))
    return NULL;
  pt++;  /* avoid pb of looping */    
  while((*pt != '\0'))
    {
      if (*pt == 'C')
	{
	  if (strncmp(pt,ForCOMMENTSTART, strlen(ForCOMMENTSTART)) == 0)
	      break;
	}
      pt++;
    }
  if (*pt == '\n') 
    pt++;
  if (*pt == '\0')
    return NULL;
  return pt;
}

/* basically go thrue the program and parse annotation, and set
   if they are defined */
initAnnotation()
{
  PTR_CMNT cmnt;
  PTR_BFND ptbif;
  int count =0;
  int i;
  char *str;

  global_int_annotation = GetAtomicType(T_INT);
  memset((char *) Annotation_PT, 0, sizeof(char) *MAX_ANNOTATION);
  memset((char *) Annotation_BIFND, 0, sizeof(PTR_BFND) *MAX_ANNOTATION);
  memset((char *) Annotation_LLND, 0, sizeof(PTR_LLND) *MAX_ANNOTATION);
  memset((char *) Annotation_CMNT, 0, sizeof(PTR_CMNT) *MAX_ANNOTATION);
  memset((char *) Annotation_Def, 0, sizeof(int) *MAX_ANNOTATION);

  ptbif = PROJ_FIRST_BIF();
  count =0;
  while (ptbif)
    {
      if (BIF_CMNT(ptbif))
	{
	  cmnt = BIF_CMNT(ptbif);
	  str = CMNT_STRING(cmnt);
	  while (str)
	    {
	      if (Is_Annotation(str))
		{
		  Annotation_PT[count] = Get_Annotation_String(str);
		  Annotation_CMNT[count] = cmnt;	
		  Annotation_BIFND[count] = ptbif;
		  count++;
		  if (MAX_ANNOTATION <= count)
		    {
		      Message("Too many annotations",0);
		      exit(1);
		    }
		}
	      str = Get_to_Next_Annotation_String(str);
	    }

	}
      ptbif = BIF_NEXT(ptbif);
    }
  Nb_Annotation = count;

  for (i=0; i < Nb_Annotation; i++)
    {
      if (TRACEANN) printf("See annotation %s\n",Annotation_PT[i]);
    }
  

  /* unparse the annotation */
  if (TRACEANN) printf("---------------------------------------------\n\n\n");
  for (i=0; i < Nb_Annotation; i++)
    {
      sprintf(AnnExTensionNumber,ASYMBOLEXT,i);
      Annotation_LLND[i] = Parse_Annotation(Annotation_PT[i],
					    Annotation_BIFND[i]);
      if (!Annotation_LLND[i])
        Message("Annotation Parse Error",BIF_LINE(Annotation_BIFND[i]));
      
      if (TRACEANN) printf("Unparse :: %s\n",Unparse_Annotation(Annotation_LLND[i]));
    }
  if (TRACEANN) printf("---------------------------------------------\n\n\n");
  /* setup which annotation is defined */
  Set_The_Define_Field();
  /* propagate the defined value */
  Propagate_defined_value();
  if (TRACEANN)
    {
      PTR_BFND first,last;
       printf("---------------------------------------------\n\n\n");
      for (i=0; i < Nb_Annotation; i++)
	{
	  Get_Scope_Of_Annotation(i,&first,&last);
	  if (first)
	     printf("A(%d) Scope first (line %d) :: %s", i,BIF_LINE(first), funparse_bfnd(first));
	  if (last)
	     printf("A(%d) Scope last (line %d)  :: %s", i, BIF_LINE(last), funparse_bfnd(last));
	}
    }

  /* unparse the annotation */
  if (TRACEANN) 
    {
      
      printf("---------------------------------------------\n\n\n");
      for (i=0; i < Nb_Annotation; i++)
        {
          printf("Unparse :: %s\n",Unparse_Annotation(Annotation_LLND[i]));
        }
    }
  return  1;
}


PTR_LLND
Parse_Annotation(string,scope)
     char * string;
     PTR_BFND scope;
{
  PTTOSTRINGTOPARSE = 0;
  STRINGTOPARSE = string;
  ANNOTATIONSCOPE = scope;
  ANNOTATE_NODE = NULL;
  LENSTRINGTOPARSE = strlen(string) +1;
  
  yyparse();

  return  ANNOTATE_NODE;
}


PTR_LLND 
Get_Define_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;
  int i;
 if (!ann)
   return(NULL);
  pt = ann;
  for(i =0 ; i < 0; i++)
    pt = NODE_OPERAND1(pt);

  return(NODE_OPERAND0(pt));

}


char *
Get_Define_Label_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;

  pt = ann;
 
 if(!pt) 
   return NULL;
 if (!NODE_OPERAND0(pt))
   return NULL;

 /* it a function call name with one parameter */
 pt = NODE_OPERAND0 (NODE_OPERAND0(pt));
 /* pt is Expr_list */

 if (pt && NODE_OPERAND0(pt))
   return(NODE_STRING_POINTER(NODE_OPERAND0(pt)));
 else
   return NULL;
}


char * 
Get_Label_Field(ann)
PTR_LLND ann;
{
  PTR_LLND pt;
  int i;

  if(!ann) 
    return NULL;

  pt = ann;
  for(i =0 ; i < 1; i++)
    pt = NODE_OPERAND1(pt);

  
 if (!NODE_OPERAND0(pt))
   return NULL;

 /* it a function call name with one parameter */
 pt = NODE_OPERAND0 (NODE_OPERAND0(pt));
 /* pt is Expr_list */

 if (pt && NODE_OPERAND0(pt))
   return(NODE_STRING_POINTER(NODE_OPERAND0(pt)));
 else
   return NULL;
}


PTR_LLND 
Get_ApplyTo_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;
  int i;

  if(!ann) 
    return NULL;

  pt = ann;
  for(i =0 ; i < 2; i++)
    pt = NODE_OPERAND1(pt);

 if(!pt) 
   return NULL;
 if (!NODE_OPERAND0(pt))
   return NULL;

 /* it a function call name with one parameter */
 pt = NODE_OPERAND0 (NODE_OPERAND0(pt));
 /* pt is Expr_list */

 if (pt && NODE_OPERAND0(pt))
   return(NODE_OPERAND0(pt));
 else
   return NULL;

}

PTR_LLND 
Get_ApplyToIf_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;
  int i;

  pt = ann;
  for(i =0 ; i < 2; i++)
    pt = NODE_OPERAND1(pt);


 if(!pt) 
   return NULL;
 if (!NODE_OPERAND0(pt))
   return NULL;

 /* it a function call name with two parameters, we want the second one */
 pt = NODE_OPERAND0 (NODE_OPERAND0(pt));
 /* pt is Expr_list */

 if (pt && NODE_OPERAND1(pt))
   return(NODE_OPERAND0(NODE_OPERAND1(pt)));
 else
   return NULL;
}


PTR_LLND 
Get_LocalVar_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;
  int i;

  if(!ann) 
    return NULL;

  pt = ann;
  for(i =0 ; i < 3; i++)
    pt = NODE_OPERAND1(pt);

  return(NODE_OPERAND0(pt));

}


PTR_LLND 
Get_Annotation_Field(ann)
PTR_LLND ann;
{
 PTR_LLND pt;
  int i;

  if(!ann) 
    return NULL;

  pt = ann;
  for(i =0 ; i < 4; i++)
    pt = NODE_OPERAND1(pt);

  return(NODE_OPERAND0(pt));

}


char *
Get_Annotation_Field_Label(ann)
PTR_LLND ann;
{
 PTR_LLND pt;

 if (!ann)
   return NULL;

 pt = Get_Annotation_Field(ann);
 
 if (!pt)
   return NULL;

 if (NODE_CODE(pt) != FUNC_CALL)
   {
     Message("Pb in annotation field",0);
     return NULL;
   }
 
 return Get_Function_Name_For_Call(pt);
}

char *
Unparse_Annotation(ann)
PTR_LLND ann;
{
  char *str;
  char temp[256];

  if(!ann) 
    return NULL;

  str = (char *) xmalloc(1024);
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,str, 0);
#endif
  sprintf(str,"[");
  if (Get_Define_Label_Field(ann))
    {
      sprintf(temp,"IfDef(\"%s\");",Get_Define_Label_Field(ann));
      strcat(str,temp);
    }

  if (Get_Label_Field(ann))
    {
      sprintf(temp,"Label(\"%s\");",Get_Label_Field(ann));
      strcat(str,temp);
    }

  if (Get_ApplyTo_Field(ann))
    { /* need  more than that */
      sprintf(temp,"ApplyTo( %s) ",Remove_Carriage_Return(cunparse_llnd(Get_ApplyTo_Field(ann))));
      strcat(str,temp);
      if (Get_ApplyToIf_Field(ann))
	{
	  sprintf(temp,"If ( %s) ;",Remove_Carriage_Return(cunparse_llnd(Get_ApplyToIf_Field(ann))));
	  strcat(str,temp);
	} else
	  strcat(str,";");
    } 
  
  if (Get_LocalVar_Field(ann))
    {
      sprintf(temp,"%s; ",Remove_Carriage_Return(cunparse_llnd(Get_LocalVar_Field(ann))));
      strcat(str,temp);
    }

  if (Get_Annotation_Field(ann))
    {
      sprintf(temp,"%s",Remove_Carriage_Return(cunparse_llnd(Get_Annotation_Field(ann))));
      strcat(str,temp);
    }

  strcat(str,"]");
  return(str);
}


char *
Does_Annotation_Defines(ann, value)
int *value;
PTR_LLND ann;
{
  PTR_LLND pt,pt1;
  char *name;
  int *res1;

  if (! (pt = Get_Annotation_Field(ann)))
    return NULL;

  name = Get_Function_Name_For_Call(pt);
  
  if(strcmp(name,"Define") == 0)
     if ((pt1 = Get_First_Parameter_For_Call(pt)))
       {
	 res1 = evaluateExpression(Get_Second_Parameter_For_Call(pt));
	 if (res1[0] != -1)
	   *value =  res1[1];
	 
	 return NODE_STRING_POINTER(pt1);
       }

   return NULL;
}

/* set all the annotation that are defined */
int Set_The_Define_Field()
{
  int i,j;
  char *str, *tsrt;
  int value;
  int found;
  /* set up those field 
    Annotation_Def[]
    char    *Defined_Value_Str[MAX_ANNOTATION];
    int     Defined_Value_Value[MAX_ANNOTATION];
  */

  for (i = 0; i < Nb_Annotation; i++)
  {
      if (Get_Define_Field(Annotation_LLND[i]) == NULL)
      {
          /* independant defined */
          if (TRACEANN) 
              printf("Annotation Defined : %s\n", tsrt = Unparse_Annotation(Annotation_LLND[i]));
#ifdef __SPF
          removeFromCollection(tsrt);
#endif
          free(tsrt);

          Annotation_Def[i] = TRUE;
          /* check if it defined something */
          Defined_Value_Str[i] =
              Does_Annotation_Defines(Annotation_LLND[i]
                  , &value);
          Defined_Value_Value[i] = value;
      }
  }
  /* end of initial setup */
  /* propagate forward only */
      for (i=0; i<  Nb_Annotation ; i++)
	{
	  str = Get_Define_Label_Field(Annotation_LLND[i]);
	  if (str)
	    { /* look if the word is defined */
	      found = FALSE;
	      for (j = i-1; j>= 0 ; j--)
		{
		  if (Defined_Value_Str[j])
		    {
		      if (strcmp(str,Defined_Value_Str[j]) == 0)
			{
			  found = TRUE;
			  break;
			}
		    }
		}
	      if (found)
		{
		  Annotation_Def[i] = TRUE;
		  if (TRACEANN) printf("Annotation Defined : %s\n",Unparse_Annotation(Annotation_LLND[i]));
		  /* check if it defined something */
		  Defined_Value_Str[i] = 
		    Does_Annotation_Defines(Annotation_LLND[i]
					    , &value);
		  Defined_Value_Value[i] = value;
		}

	    }
	}
 return 0; 
}


/* return the annotation with label -1 for not found */
int
Get_Annotation_With_Label(str)
char *str;
{ int i;
  char *strc;


  for (i=0; i < Nb_Annotation; i++)
    {
      strc = Get_Label_Field(Annotation_LLND[i]);
      if (strc)
	{
	  if (strcmp(strc, str) == 0)
	    {
	      return i;
	    }
	}
    }
  return -1;
}


/* Compute the first and last bif node a annotation applies */

int Get_Scope_Of_Annotation(nb,first,last)
int nb;
PTR_BFND *first, *last;
{
  PTR_LLND ann,f1,f2;
  PTR_LLND field_apply;
  char *str;
  int nb2;

  ann = Annotation_LLND[nb];
  if (!ann)
    {
      *first = NULL;
      *last = NULL;
      return FALSE;      
    }
  if (!Annotation_Def[nb])
     {
       *first = NULL;
       *last = NULL;
       return TRUE;      
     }

  /* the first case is easy */
  field_apply = Get_ApplyTo_Field(ann);
  if (!field_apply)
    {
      *first = Annotation_BIFND[nb];
      *last  = Annotation_BIFND[nb];
      return TRUE;  
    }
  
  /* depend on */
  f1 = field_apply;
  if (!f1)
    {
      *first = Annotation_BIFND[nb];
      *last  = Annotation_BIFND[nb];
      return FALSE;      
    }
  switch(NODE_CODE(f1))
    {
    case VAR_REF:
      Message("Function Call in Get_Scope_Of_Annotation not yet implemented, sorry",0);
      break;
    case STRING_VAL :
      str = NODE_STRING_POINTER(f1);
      if (strcmp(str,"NextStmt") == 0)
	{
	  *first = Annotation_BIFND[nb];
	  *last  = Annotation_BIFND[nb];
	  return TRUE;
	}
      if (strcmp(str,"NextAnnotation") == 0)
	{
	  *first = Annotation_BIFND[nb];
	  *last  = Annotation_BIFND[nb+1];
	  if (*last == NULL)
	    *last = Get_Last_Node_Of_Project();
	  return TRUE;
	}
      if (strcmp(str,"EveryWhere") == 0)
	{
	  *first = PROJ_FIRST_BIF();
	  *last  = Get_Last_Node_Of_Project();
	  return TRUE;
	}
      if (strcmp(str,"Follow") == 0)
	{
	  *first = Annotation_BIFND[nb];
	  *last = Get_Last_Node_Of_Project();
	  return TRUE;
	}
      if (strcmp(str,"CurrentScope") == 0)
	{
	  *first = BIF_CP(Annotation_BIFND[nb]);
	  if (*first)
	    *last = getLastNodeOfStmt(*first);
	  else
	    *last = NULL;
	  return TRUE;
	}
      Message("Pb in Get_Scope_Of_Annotation",0);
        break;
    case  EXPR_LIST :
      *first = Annotation_BIFND[nb];
      if (NODE_OPERAND0(f1))
	{
	  f2 = NODE_OPERAND0(f1);
	  if (f2 && (NODE_CODE(f2) == STRING_VAL))
	    {
	      str = NODE_STRING_POINTER(f2);
	      nb2 = Get_Annotation_With_Label(str);
	      if (nb2!= -1)
		{
		  *first  = Annotation_BIFND[nb2];
		} else
		  Message("Pb in Get_Scope_Of_Annotation",0);
	    } else
	      Message("Pb in Get_Scope_Of_Annotation",0);
	}
      f2 = NODE_OPERAND0(NODE_OPERAND1(f1));
      if (f2 && (NODE_CODE(f2) == STRING_VAL))
	{
	  str = NODE_STRING_POINTER(f2);
	  nb2 = Get_Annotation_With_Label(str);
	  if (nb2!= -1)
	    {
	      *last  = getNodeBefore(Annotation_BIFND[nb2]);
	    } else
	      Message("Pb in Get_Scope_Of_Annotation",0);
	} else
	  Message("Pb in Get_Scope_Of_Annotation",0);

      break;
    default:
      {
	Message("Pb in Get_Scope_Of_Annotation",0);
	return FALSE;
	}
    }
  return TRUE;
}


/* for all defined value, propagate forward */

void Propagate_defined_value()
{
  int i; 
  int j;
  PTR_LLND val;
  char *str;
  for (i=0 ; i< Nb_Annotation ; i++)
    {
      if (Defined_Value_Str[i])
	{
	  val = makeInt(Defined_Value_Value[i]);
	  str = Defined_Value_Str[i];
	  for (j = i+1 ; j< Nb_Annotation ; j++)
	    {
	      if (Annotation_LLND[j])
		if (Get_Annotation_Field_Label(Annotation_LLND[j]))
		  {
		    if (strcmp(Get_Annotation_Field_Label(Annotation_LLND[j]), 
			       "Define") != 0)
		      Replace_String_In_Expression(NODE_OPERAND1(NODE_OPERAND1(Annotation_LLND[j])), str, val);
		  } else
		    Replace_String_In_Expression(NODE_OPERAND1(NODE_OPERAND1(Annotation_LLND[j])), str, val); 
	    }
	}
    }
}

/* return NULL if not annotation of kind apply, otherwise return the 
   llnd expression corresponding to the annotation 
   Very dumb version, but simple one (warning, because of label an annotation
   does not apply where it is necessarely, except for defined annotation )*/

PTR_LLND
Does_Annotation_Apply(kind,bif)
     char *kind;
     PTR_BFND bif;
{
  int i;
  PTR_BFND first,last;

  for (i=0 ; i< Nb_Annotation ; i++)
    {
      if (Annotation_Def[i])
	{
	  if (kind)
	    {
	      if (strcmp(Get_Annotation_Field_Label(Annotation_LLND[i]), kind) == 0)
		{
		  if (Get_Scope_Of_Annotation(i,&first,&last))
		    {
		      if (isItInSection(first, last, bif))
			return Get_Annotation_Field(Annotation_LLND[i]);
		    }
		}
	    }else
	      {
		if (Get_Scope_Of_Annotation(i,&first,&last))
		  {
		    if (isItInSection(first, last, bif))
		      return Get_Annotation_Field(Annotation_LLND[i]);
		  }
	      }
	}
    }
  return NULL;
}     


PTR_LLND
Get_Annotation_Field_List_For_Stmt(bif)
     PTR_BFND bif;
{
  int i;
  PTR_BFND first,last;
  PTR_LLND list = NULL, pt =NULL;
  
 
  for (i=0 ; i< Nb_Annotation ; i++)
    {
      if (Annotation_Def[i])
	{
	  if (Get_Scope_Of_Annotation(i,&first,&last))
	    {
	      if (isItInSection(first, last, bif))
		{
		  if (!list)
		    {
		      list = newExpr(EXPR_LIST,NULL, 
				   Get_Annotation_Field(Annotation_LLND[i]),
				   NULL);
		      pt = list;
		    }else
		      {
			NODE_OPERAND1(pt) = newExpr(EXPR_LIST,NULL, 
				   Get_Annotation_Field(Annotation_LLND[i]),
				   NULL);
			pt = NODE_OPERAND1(pt);
		      }
		  
		}
	    }
	}
    }
  return list;
}     



PTR_LLND
Get_Annotation_List_For_Stmt(bif)
     PTR_BFND bif;
{
  int i;
  PTR_BFND first,last;
  PTR_LLND list = NULL, pt =NULL;
  
 
  for (i=0 ; i< Nb_Annotation ; i++)
    {
      if (Annotation_Def[i])
	{
	  if (Get_Scope_Of_Annotation(i,&first,&last))
	    {
	      if (isItInSection(first, last, bif))
		{
		  if (!list)
		    {
		      list = newExpr(EXPR_LIST,NULL, 
				       Annotation_LLND[i],
				       NULL);
		      pt = list;
		    }else
		      {
			NODE_OPERAND1(pt) = newExpr(EXPR_LIST,NULL, 
						      Annotation_LLND[i],
						      NULL);
			pt = NODE_OPERAND1(pt);
		      }
		  
		}
	    }
	}
    }
  return list;
}     

/* Access functions */
int
Get_Number_of_Annotation()
{
  return Nb_Annotation;
}


PTR_BFND
Get_Annotation_Bif(id)
     int id;
{
  return Annotation_BIFND[id];
}


PTR_LLND
Get_Annotation_Expr(id)
     int id;
{
  return Annotation_LLND[id];
}

char *
Get_String_of_Annotation(id)
     int id;
{
  return Annotation_PT[id];
}

PTR_CMNT
Get_Annotation_Comment(id)
     int id;
{
  return Annotation_CMNT[id];
}


int
Is_Annotation_Defined(id)
     int id;
{
  return Annotation_Def[id];
}


char *
Annotation_Defines_string(id)
     int id;
{
  return Defined_Value_Str[id];
}

int
Annotation_Defines_string_Value(id)
     int id;
{
  return Defined_Value_Value[id];
}
