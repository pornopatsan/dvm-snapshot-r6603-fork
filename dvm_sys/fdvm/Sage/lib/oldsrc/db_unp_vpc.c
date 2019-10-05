/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/* Modified by Jenq-Kuen Lee Feb 24,1988	  */
/* The simple un-parser for VPC++		  */
#include <stdio.h>
#include <stdlib.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
# include "db.h"
# include "vparse.h"

# define NULLTEST(VAR)	(VAR == NULL? -1 : VAR->id)
# define type_index(X)	(X-T_INT)
# define binop(n)	(n >= EQ_OP && n <= NEQV_OP)
# define BUFLEN 	200000

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

extern PTR_SYMB	cur_symb_head;	/* point to the head of the list of symbols */
extern char buffer[], *bp;

static int	first;
static int	global_tab;
static char	buffera[BUFLEN];
static char	temp_buf[BUFLEN]; /* for temporary usage */
static char	temp1_buf[BUFLEN];
static char	temp2_buf[BUFLEN]; /* for temporary usage */

static int basket_needed();

/*
 * forward references
 */
static void cunp_blck();
static void gen_simple_type();
static void  gen_func_hedr();
static PTR_SYMB find_declarator();
static void cunp_llnd();
int cdrtext();
int is_scope_op_needed();

static
char *cop_name[] = {
	"->",	    /* 0  */
	"!",	    /* 1  */
	"~",	    /* 2  */
	"++",	    /* 3  */
	"--",	    /* 4  */
	"-",	    /* 5  */
	"*",	    /* 6  */
	"&",	    /* 7  */
	"sizeof ",  /* 8  */
	"*",	    /* 9  */
	"/",	    /* 10 */
	"%",	    /* 11 */
	"+",	    /* 12 */
	"-",	    /* 13 */
	">>",	    /* 14 */
	"<<",	    /* 15 */
	"<",	    /* 16 */
	">",	    /* 17 */
	"<=",	    /* 18 */
	">=",	    /* 19 */
	"==",	    /* 20 */
	"!=",	    /* 21 */
	"&",	    /* 22 */
	"^",	    /* 23 */
	"|",	    /* 24 */
	"&&",	    /* 25 */
	"||",	    /* 26 */
	"=",	    /* 27 */
	"+=",	    /* 28 */
	"-=",	    /* 29 */
	"&=",	    /* 30 */
	"|=",	    /* 31 */
	"*=",	    /* 32 */
	"/=",	    /* 33 */
	"%=",	    /* 34 */
	"^=",	    /* 35 */
	"<<=",	    /* 36 */
	">>="	    /* 37 */
};


/* Added for VPC */
static
char *ridpointers[] = {
	"",			/* unused */
	"",			/* int */
	"char", 		/* char */
	"float",		/* float */
	"double",		/* double */
	"void", 		/* void */
	"",			/* unused1 */
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
	"syn",			/* syn */
	"shared",		/* shared */
	"private",		/* private */
	"future",		/* future */
	"virtual",		/* virtual */
	"inline",		/* inline */
	"friend",		/* friend */
	"",			/* public */
	"",			/* protected */
};

/* Added for VPC */
static int
re_map_status(rid_value)
	int rid_value;
{
	switch (rid_value) {

	/* The following flag store in type->entry.descriptive.long_short_flag */
	case (int) BIT_PRIVATE: return((int)RID_PRIVATE);
	case (int) BIT_FUTURE:	return((int)RID_FUTURE);
	case (int) BIT_VIRTUAL: return((int)RID_VIRTUAL);
	case (int) BIT_INLINE:	return((int)RID_INLINE);

	case (int) BIT_UNSIGNED:return((int)RID_UNSIGNED);
	case (int) BIT_SIGNED : return((int)RID_SIGNED);


	case (int) BIT_SHORT :	return((int)RID_SHORT);
	case (int) BIT_LONG :	return((int)RID_LONG);


	case (int) BIT_VOLATILE:return((int)RID_VOLATILE);
	case (int) BIT_CONST   :return((int)RID_CONST);

	case (int) BIT_TYPEDEF :return((int)RID_TYPEDEF); 
	case (int) BIT_EXTERN  :return((int)RID_EXTERN);
	case (int) BIT_AUTO :	return((int)RID_AUTO);
	case (int) BIT_STATIC : return((int)RID_STATIC);
	case (int) BIT_REGISTER:return((int)RID_REGISTER);
	case (int) BIT_FRIEND:	return((int)RID_FRIEND);
	default:
		return(0);
	}
}


static void
put_tabs(n)
	int n;
{
	int i;

	for(i = 0; i < n; i++) {
		*bp++ = ' ';
		*bp++ = ' ';
	}
}


static void
addstr(s)
	char *s;
{
	while( (*bp = *s++) != 0)
		bp++;
}


static void
addstr1(index)
	int index ;
{
	int i;

	i = re_map_status(index);
	if (i) {
		addstr(ridpointers[i]) ;
		*bp++ = ' ';
	}
}



static void
put_right(s, temp_buf)
	char *s ;
	char *temp_buf;
{
	int len,i ;
	char *p;
      
	i=0;
	len = strlen(temp_buf) ;
	for ( p = s ; *p ; p++,i++)
		*(temp_buf + len+ i) = *p ;
	*(temp_buf+len+i+1) = '\0';
}


static void
put_left(s, temp_buf)
	char *s ;
	char *temp_buf;
{
	int i ;
	int len1 ,len2;  

	len1 = strlen(s);
	len2 = strlen(temp_buf) ;
	*(temp_buf+len2+len1) = '\0';
	for ( i=len2 ; i ; i--)
		*(temp_buf + len1+ i-1) = *(temp_buf + i -1 );
	for ( i=0; *s ; i++,s++)
		*(temp_buf + i ) = *s ;

}


static void
clean(temp_buf)
	char *temp_buf;
{
	char *p;

	for (p = temp_buf ; p < temp_buf+BUFLEN ;)
		*p++ = '\0';
}


/*
 * gen_if_node(pbf) --- generate the if statement pointed to by pbf.
 */
static void
gen_branch(branch_type, pbf)
	char	       *branch_type;
	PTR_BFND	pbf;
{
	PTR_BFND	gen_stmt_list();
	addstr(branch_type);
	*bp++ = '(';
	cunp_llnd(pbf->entry.Template.ll_ptr1);
	*bp++ = ')';
}


static void
gen_descriptive_type(symb1)
	PTR_SYMB symb1 ;
{
	int i;
	PTR_TYPE q ;

	for (q = symb1->type; q ;   ) { 
		switch ( q->variant) {
		case T_POINTER : 
		case T_FUNCTION :
			q = q->entry.Template.base_type ;
			break;
		case T_DESCRIPT :
			for (i=1; i< MAX_BIT; i= i*2)
				addstr1(q->entry.descriptive.long_short_flag & i);
			q = q->entry.descriptive.base_type ;
			break ;
		default:	/* It might need more for complicated case */
			q = (PTR_TYPE) NULL ;
		}
	}


} 


static void
cunp_bfnd(tabs,pbf)
	int tabs;
	PTR_BFND     pbf;
{
        /* PTR_BFND	pbfnd, pnext; */
	/* PTR_SYMB	s; */
	/* int		i; */
        /* int		lines; */
        PTR_CMNT cmnt;
	if (!pbf) return;
	/* printf("variant = %d\n", pbf->variant); */
        if ( (cmnt = pbf->entry.Template.cmnt_ptr) != 0)
                while (cmnt != NULL && cmnt->type == FULL) {
                        addstr(cmnt->string);
                        addstr("\n");
                        cmnt = cmnt->next;
                }    

	if (pbf->label) {
		char b[10];

		sprintf(b ,"%-5d ", (int)(pbf->label->stateno));
		addstr(b);
	}

	put_tabs(tabs);

	switch (pbf->variant) {
	case GLOBAL	:
	case PROG_HEDR	:
	case PROC_HEDR	:
		break ;
	case FUNC_HEDR	:
		gen_simple_type(pbf->entry.Template.symbol->type, pbf, tabs);
		gen_func_hedr(pbf->entry.Template.symbol, pbf, tabs);
		break;
	case IF_NODE	:
		gen_branch("if ",pbf);
		break;
	case LOGIF_NODE :
	case ARITHIF_NODE:
	case WHERE_NODE :
		break;
	case FOR_NODE	:
		addstr("for (");
		cunp_llnd(pbf->entry.Template.ll_ptr1);
		*bp++ = ';';
		cunp_llnd(pbf->entry.Template.ll_ptr2);
		*bp++ = ';';
		cunp_llnd(pbf->entry.Template.ll_ptr3);
		addstr(") ") ;
		break;
	case FORALL_NODE	:
	case WHILE_NODE :
		addstr("while (");
		cunp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(") ") ;
		break;
	case ASSIGN_STAT:
	case IDENTIFY:
	case PROC_STAT	:
	case SAVE_DECL:
	case CONT_STAT:
	case FORMAT_STAT:
		break;
	case LABEL_STAT:
		addstr(pbf->entry.Template.lbl_ptr->label_name->ident);
		addstr(" : ");
		break;
	case GOTO_NODE:
		addstr("goto ");
		addstr(pbf->entry.Template.lbl_ptr->label_name->ident);
		addstr(" ;");
		break;
	case ASSGOTO_NODE:
	case COMGOTO_NODE:
	case STOP_STAT:
		break;
	case RETURN_STAT:
		addstr("return");
		if (pbf->entry.Template.ll_ptr1) {
			addstr("(");
			cunp_llnd(pbf->entry.Template.ll_ptr1);
			addstr(");");
		}
		break;
	case PARAM_DECL :
	case DIM_STAT:
	case EQUI_STAT:
	case DATA_DECL:
	case READ_STAT:
	case WRITE_STAT:
	case OTHERIO_STAT:
	case COMM_STAT:
	case CONTROL_END:
		break;
	case ENUM_DECL :		/* New added for VPC */
	case CLASS_DECL:		/* New added for VPC */
	case UNION_DECL:		/* New added for VPC */
	case STRUCT_DECL:		/* New added for VPC */
	case COLLECTION_DECL:
	      {  PTR_BLOB blob ;
		 PTR_SYMB symb,symb1 ;
		 PTR_LLND llptr,llptr2;
		 int i;

		      llptr = pbf->entry.Template.ll_ptr1;
		      symb1 = find_declarator(llptr);
		      if (symb1) gen_descriptive_type(symb1);
		      switch (pbf->variant) {
		      case UNION_DECL: addstr("union ") ;
				       break;
		      case STRUCT_DECL:addstr("struct ") ;
				       break;
		      case ENUM_DECL  : addstr("enum ") ;
				       break;
		      case CLASS_DECL : addstr("class ") ;
				       break;
		      case COLLECTION_DECL : addstr("Collection ") ;
				       break;
		      }
		      if ( (symb=pbf->entry.Template.symbol) != 0) {
			    addstr(symb->ident);
			    *bp++ = ' ';
		      }
		     if (pbf->entry.Template.ll_ptr2) {
			addstr(" : ");
			for (llptr2 = pbf->entry.Template.ll_ptr2,i=0;llptr2; 
			     llptr2= llptr2->entry.Template.ll_ptr2,i++)
			      { if (i) addstr(" , ");
				addstr(llptr2->entry.Template.ll_ptr1->entry.Template.symbol->ident);
			      }
			  }
		       if ( (blob=pbf->entry.Template.bl_ptr1) != 0) 
			{   addstr(" {\n") ;
			    for ( ; blob ; blob = blob->next)
			       cunp_blck(blob->ref, tabs+2);
			    put_tabs(tabs);  addstr("} ");
			  } 
		       cunp_llnd(llptr);
		       *bp++ = ';';
		       break;
	       }
	case DERIVED_CLASS_DECL:	/* Need More for VPC */
	case VAR_DECL:
		{  PTR_SYMB symb1 ;
		   PTR_LLND llptr;

		   llptr = pbf->entry.Template.ll_ptr1;
		   symb1 = find_declarator(llptr);
		   if (symb1)
			   gen_simple_type(symb1->type, pbf, tabs) ;
		   cunp_llnd(llptr);
		   if (pbf->control_parent->variant != ENUM_DECL)
			   addstr(" ;");
		   break;
		}

	case EXPR_STMT_NODE:		/* New added for VPC */
		   cunp_llnd(pbf->entry.Template.ll_ptr1);
		   addstr(" ;");
		   break ;
	case DO_WHILE_NODE:		/* New added for VPC */
					/* Need study	     */
	case SWITCH_NODE :		/* New added for VPC */
		   addstr("switch (");
		   cunp_llnd(pbf->entry.Template.ll_ptr1);
		   *bp++ = ')';
		   break ;
	case CASE_NODE :		/* New added for VPC */
		   addstr("case ");
		   cunp_llnd(pbf->entry.Template.ll_ptr1);
		   addstr(" : ") ;
		   break ;
	case DEFAULT_NODE:		/* New added for VPC */
		   addstr("default :") ;
		   break;
	case BASIC_BLOCK :
		   break ;
	case BREAK_NODE  :		/* New added for VPC */
		   addstr("break;");
		   break;
	case CONTINUE_NODE:		/* New added for VPC */
		   addstr("continue;");
	case RETURN_NODE  :		/* New added for VPC */
		addstr("return");
		if (pbf->entry.Template.ll_ptr1) {
			addstr("(");
			cunp_llnd(pbf->entry.Template.ll_ptr1);
			addstr(");");
		}
		break;
	case ASM_NODE	  :		/* New added for VPC */
		break;			/* Need More	     */
	case SPAWN_NODE :		/* New added for VPC */
		addstr("spawn");
		cunp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(" ; ");
		break;
	case PARFOR_NODE  :		/* New added for VPC */
		addstr("parfor (");
		cunp_llnd(pbf->entry.Template.ll_ptr1);
		*bp++ = ';';
		cunp_llnd(pbf->entry.Template.ll_ptr2);
		*bp++ = ';';
		cunp_llnd(pbf->entry.Template.ll_ptr3);
		addstr(") ") ;
		break;
	case FUTURE_STMT:
	       addstr("future ");
	       cunp_llnd(pbf->entry.Template.ll_ptr1);
	       addstr(" (");
	       cunp_llnd(pbf->entry.Template.ll_ptr2);
	       addstr(")");
	       break;
	case PAR_NODE  :		/* New added for VPC */
		addstr("par ") ;
		break;
	default:
		printf(" unknown biffnode = %d\n", pbf->variant);
		exit(0);
		break;		/* don't know what to do at this point */
	}
	*bp++ = '\n';
}


/************************************************************************
 *									*
 *		     generate simple declaration			*
 *									*
 ************************************************************************/
static void
gen_simple_type(q_type, dum_pbf, tabs)
	PTR_TYPE q_type ;
	PTR_BFND dum_pbf ;
	int tabs;
{
   PTR_TYPE q,q3 ;
   PTR_SYMB s ,symb;
   /* PTR_BLOB blob ; */
   /* PTR_BFND pbf;   */
   int i;

	for (q = q_type ; q ; ) {
		switch (q->variant) {
		case T_REFERENCE:
		case T_POINTER	:
		case T_FUNCTION :
		case T_ARRAY	:
			q = q->entry.Template.base_type ;
			break ;
		case T_DESCRIPT :
			for (i=1; i< MAX_BIT; i *= 2)
				addstr1(q->entry.descriptive.long_short_flag & i);
			q = q->entry.descriptive.base_type ;
			break ;
		case DEFAULT :	q = (PTR_TYPE ) NULL ;
			break ;
		case T_DERIVED_COLLECTION :
		      symb = q->entry.col_decl.collection_name;
		      q3 = q->entry.col_decl.base_type;
		      addstr(symb->ident);
		      if (q3) {
			addstr("<");
			gen_simple_type(q3,dum_pbf,tabs);
			addstr(">");
		      }
		      addstr(" ");
		     q= (PTR_TYPE) NULL ;
		    break;
		case T_DERIVED_TYPE :
			s = q->entry.derived_type.symbol ;
			switch (s->variant) {
			case STRUCT_NAME:	addstr("struct "); break;
			case ENUM_NAME: 	addstr("enum ");   break;
			case UNION_NAME:	addstr("union ");  break;
			case CLASS_NAME:	break;
			case COLLECTION_NAME:	 break;
			case TYPE_NAME:
			default:
				break ;
			}
			addstr(s->ident);
			*bp++ = ' ';
			if (s->variant==COLLECTION_NAME) {
			  if ( (q3=s->type->entry.derived_class.base_type) != 0) {
			      addstr("<");
			      gen_simple_type(q3,dum_pbf,tabs);
			      addstr(">");
			    }
			}
			q = (PTR_TYPE) NULL ;
			break ;

		case T_INT     :
			addstr("int ");
			q= (PTR_TYPE) NULL ;
			break;
		case T_CHAR    :
			addstr("char ");
			q= (PTR_TYPE) NULL ;
			break;
		case T_VOID    :
			addstr("void ");
			q= (PTR_TYPE) NULL ;
			break;
		case T_DOUBLE  :
			addstr("double ");
			q= (PTR_TYPE) NULL ;
			break;
		case T_FLOAT   :
			addstr("float ");
			q= (PTR_TYPE) NULL ;
			break;

		case T_UNION   :
		case T_STRUCT  :
		case T_ENUM    :
		case T_CLASS   :
			switch (q->variant) {
			case T_UNION   : addstr("union ") ;
				break;
			case T_STRUCT  : addstr("struct ") ;
				break;
			case T_ENUM    : addstr("enum ") ;
				break;
			case T_CLASS   : addstr("class ") ;
				break;
			case T_COLLECTION: addstr("Collection ") ;
				       break;
			}

			if ( (symb=q->entry.derived_class.original_class->entry.Template.symbol) != 0) {
				addstr(symb->ident);
				*bp++ = ' ';
			}	    

			q = (PTR_TYPE) NULL ;
			break;
		case T_COLLECTION:
		      if ( (symb=q->entry.derived_class.original_class->entry.Template.symbol) != 0)
			{   addstr(symb->ident);
			    if ( (q3=q->entry.derived_class.base_type) != 0) {
			      addstr("<");
			      gen_simple_type(q3,dum_pbf,tabs);
			      addstr(">");
			    }
			    addstr(" ");
			  }
		      q= (PTR_TYPE) NULL ;
		      break;
	    /* not in leejenq's version
		case T_DERIVED_CLASS:
		   {	PTR_BFND pbf ;

			pbf = q->entry.derived_class.original_class ;
			addstr("class");
			if (symb=pbf->entry.Template.symbol)
				addstr(symb->ident);
			addstr(" : ");
			cunp_llnd(pbf->entry.Template.ll_ptr2);
			if (blob=pbf->entry.Template.bl_ptr1) {
				addstr(" {") ;
				for ( ; blob ; blob = blob->next)
					cunp_bfnd(tabs,blob->ref);
				put_tabs(tabs);  *bp++ = '}';
			}
			break ;
		}
	     */
		default :
			break;
		}
	}
}


static int
cprecedence(op)
	int op ;
{   
    switch (op)  {
	case NEW_OP:	   
	case DELETE_OP:
			    return(2);
	case EQ_OP	:   return(7);
	case LT_OP	:   return(6);
	case GT_OP	:   return(6);
	case NOTEQL_OP	:   return(7);
	case LTEQL_OP	:   return(6);
	case GTEQL_OP	:   return(6);
	case ADD_OP	:   return(4);
	case OR_OP	:   return(12);
	case MULT_OP	:   return(3);
	case DIV_OP	:   return(3);
	case AND_OP	:   return(11);
	case XOR_OP	:   return(9);

	case LE_OP	:   return(6);	/* duplicated */
	case GE_OP	:   return(6);	/* duplicated */
	case NE_OP	:   return(7);	/* duplicated */
	case UNARY_ADD_OP:  return(2);	/* unary operation */
	case SUB_OP	:   return(2);	/* unary operation */
	case SUBT_OP	:   return(11); /* binary operator */
	case MINUS_OP	:   return(2);	/* unary operator */
	case NOT_OP	:   return(2);

	case PLUS_ASSGN_OP:
	case MINUS_ASSGN_OP:
	case AND_ASSGN_OP:
	case  IOR_ASSGN_OP:
	case  MULT_ASSGN_OP:
	case  DIV_ASSGN_OP:
	case MOD_ASSGN_OP:
	case  XOR_ASSGN_OP:
	case  LSHIFT_ASSGN_OP:
	case  RSHIFT_ASSGN_OP :

	case ARITH_ASSGN_OP:
	case ASSGN_OP	:   return(14);
	case DEREF_OP	:   return(2);
	case POINTST_OP :   return(1);
	case RECORD_REF :   return(1);
	case BITAND_OP	:   return(10);
	case BITOR_OP	:   return(10);
	case LSHIFT_OP	:   return(5);
	case RSHIFT_OP	:   return(5);
	case MOD_OP :	return(3);	  /* New added for VPC */
	case ADDRESS_OP:    return(2);
	case SIZE_OP   :    return(2);
	case PLUSPLUS_OP:
	case MINUSMINUS_OP: return(2);
	case EXPR_LIST	:    return(15);
	default 	:   return(0);
	}
}


int
mapping(op)
int op ;
{
    switch (op)  {
	case EQ_OP	:   return(20);
	case LT_OP	:   return(16);
	case GT_OP	:   return(17);
	case NOTEQL_OP	:   return(21);
	case LTEQL_OP	:   return(18);
	case GTEQL_OP	:   return(19);
	case ADD_OP	:   return(12);
	case OR_OP	:   return(26);
	case MULT_OP	:   return(9);
	case DIV_OP	:   return(10);
	case AND_OP	:   return(25);
	case XOR_OP	:   return(23);

	case LE_OP	:   return(18); /* duplicated */
	case GE_OP	:   return(19); /* duplicated */
	case NE_OP	:   return(21); /* duplicated */
	case SUB_OP	:   return(5); /* unary operator */
	case MINUS_OP	:   return(5); /* unary operator */
	case SUBT_OP   :   return(5); /* binary operator */
	case NOT_OP	:   return(1);

	case PLUS_ASSGN_OP: return(28);
	case MINUS_ASSGN_OP:return(29);
	case AND_ASSGN_OP:  return(30);
	case  IOR_ASSGN_OP: return(31);
	case  MULT_ASSGN_OP:return(32);
	case  DIV_ASSGN_OP: return(33);
	case MOD_ASSGN_OP:  return(34);
	case  XOR_ASSGN_OP: return(35);
	case  LSHIFT_ASSGN_OP:return(36);
	case  RSHIFT_ASSGN_OP :return(37);
	case ASSGN_OP	:   return(27);

	case DEREF_OP	:   return(6);
	case POINTST_OP :   return(0);
	case BITAND_OP	:   return(22);
	case BITOR_OP	:   return(24);
	case LSHIFT_OP	:   return(15);
	case RSHIFT_OP	:   return(14);
	case MINUSMINUS_OP: return(4);		   /* New added for VPC */
	case PLUSPLUS_OP  : return(3);		   /* New added for VPC */
	case UNARY_ADD_OP : return(12); 	   /* New added for VPC */
	case BIT_COMPLEMENT_OP :return(2);	   /* New added for VPC */
	case MOD_OP :	return(11);	   /* New added for VPC */
	case SIZE_OP :	     return(8); 	   /* New added for VPC */
	case ADDRESS_OP:     return(7);
	default 	:   sprintf(buffera, "bad case 1");
			    return(0);
	}
}


static void
gen_op(value)
	int value;
{
    switch (value) {
    case ((int) PLUS_EXPR) : addstr("+= ");
	break;
    case ((int) MINUS_EXPR): addstr("-= ");
	break;
    case ((int) BIT_AND_EXPR):addstr("&= ");
	break;
    case ((int) BIT_IOR_EXPR):addstr("|= ");
	break;
    case ((int) MULT_EXPR):  addstr("*= ");
	break;
    case ((int) TRUNC_DIV_EXPR): addstr("/= ");
	break;
    case ((int) TRUNC_MOD_EXPR): addstr("%= ");
	break;
    case ((int) BIT_XOR_EXPR): addstr("^= ");
	break;
    case ((int) LSHIFT_EXPR): addstr("<<= ");
	break;
    case ((int) RSHIFT_EXPR): addstr(">>= ");
	break;
	default : addstr("= ");
    }
}

static char left_mod[2000];
static void
gen_simple_type_2(q_type, dum_pbf, tabs)
	PTR_TYPE q_type;
	PTR_BFND dum_pbf;
	int tabs;
{
   PTR_BFND pbf;
   PTR_TYPE q ;
   PTR_SYMB s ,symb;
   PTR_BLOB blob ;
   PTR_LLND r1;
   /*  char *old_bp; */
   int level ;
   int i;
char * bp_save;

   left_mod[0] = '\0';
   level= 0 ;
   clean(temp_buf);
    for (q = q_type ; q ; )
    {
     switch (q->variant) {
     case T_POINTER  :
	put_left("*",temp_buf);
	level = 1;
	q = q->entry.Template.base_type ;
	break;
     case T_REFERENCE:
	put_left("&",temp_buf);
	level = 1;
	q = q->entry.Template.base_type ;
	break;
     case T_FUNCTION :
	put_left("(",temp_buf);
	put_right(")",temp_buf);
	put_right("()",temp_buf);
	q = q->entry.Template.base_type ;
	break;
     case T_ARRAY    :
	if (level >0) {
	    put_left("(",temp_buf);
	    put_right(")",temp_buf);
	  }
	clean(temp1_buf);
	bp_save = bp;  /* Backup before switching buffer */
	put_left(buffer,temp1_buf);  /* Backup before switching buffer */
	clean(buffer);
	bp = &(buffer[0]);
	for (r1=q->entry.ar_decl.ranges;r1; r1= r1->entry.Template.ll_ptr2)
	  {
	    addstr("[");
	    cunp_llnd(r1->entry.Template.ll_ptr1);
	    addstr("]");
	  }
	put_right(buffer,temp_buf);
	clean(buffer);
	bp = bp_save;
	put_left(temp1_buf,buffer);
	q = q->entry.Template.base_type ;
	break ;
     case T_DESCRIPT :
	clean(temp1_buf);
	bp_save = bp;                /* Backup before switching buffer */
	put_left(buffer,temp1_buf);  /* Backup before switching buffer */
	clean(buffer);  
	bp = &(buffer[0]);
	for (i=1; i< MAX_BIT; i= i*2)
	  addstr1(q->entry.descriptive.long_short_flag & i);
	put_right(buffer, left_mod);
	clean(buffer); 
	bp = bp_save;
	put_left(temp1_buf,buffer);
	q = q->entry.descriptive.base_type ; 
	break ;
    case DEFAULT : 
	put_left("int ",temp_buf);
	q = (PTR_TYPE ) NULL ;
	break ;
    case T_DERIVED_TYPE :
	clean(temp1_buf);
	bp_save = bp;                /* Backup before switching buffer */
	put_left(buffer,temp1_buf);  /* Backup before switching buffer */
	clean(buffer);
	bp = &(buffer[0]);
	s = q->entry.derived_type.symbol ;
	switch (s->variant) {
	case STRUCT_NAME: addstr("struct "); break;
	case ENUM_NAME:   addstr("enum ");   break;
	case UNION_NAME:  addstr("union ");  break;
	case CLASS_NAME:  addstr("class ");  break;
	case COLLECTION_NAME:  addstr("Collection ");  break;
	case TYPE_NAME:
	default:
	  break ;
	}
	addstr(s->ident);
	addstr(" ");
	put_left(buffer,temp_buf);
	clean(buffer);
	bp = bp_save;
	put_left(temp1_buf,buffer);
	q = (PTR_TYPE) NULL ;
	break ;
   case T_INT	  :
       put_left("int ",temp_buf);
       q= (PTR_TYPE) NULL ;
       break;
   case T_CHAR	  :
       put_left("char ",temp_buf);
       q= (PTR_TYPE) NULL ;
       break;
   case T_VOID	  :
	put_left("void ",temp_buf);
	q= (PTR_TYPE) NULL ;
	break;
   case T_DOUBLE  :
	put_left("double ",temp_buf);
	q= (PTR_TYPE) NULL ;
	break;
   case T_FLOAT   :
	put_left("float ",temp_buf);
	q= (PTR_TYPE) NULL ;
	break;
   case T_UNION   :
   case T_STRUCT  :
   case T_ENUM	  :
   case T_CLASS   :
   case T_COLLECTION:
   case T_DERIVED_CLASS:
     clean(temp1_buf);
     bp_save = bp;                /* Backup before switching buffer */
     put_left(buffer,temp1_buf);  /* Backup before switching buffer */
     clean(buffer);
     bp = &(buffer[0]);
     switch (q->variant) {
     case T_UNION   : addstr("union ") ;
       break;
     case T_STRUCT  : addstr("struct ") ;
       break;
     case T_ENUM    : addstr("enum ") ;
       break;
     case T_DERIVED_CLASS:
     case T_CLASS   : addstr("class ") ;
       break;
     case T_COLLECTION	: addstr("Collection ") ;
       break;
     }
     if ( (symb=q->entry.derived_class.original_class->entry.Template.symbol) != 0)
       {   addstr(symb->ident);
	   addstr(" ");
	 }	   
     pbf = q->entry.derived_class.original_class ;
     if (pbf->entry.Template.ll_ptr2) {
       addstr(" : ");
       cunp_llnd(pbf->entry.Template.ll_ptr2);
     }
     if ( (blob=q->entry.derived_class.original_class->entry.Template.bl_ptr1) != 0)
       {   addstr(" {\n") ;
	   for ( ; blob ; blob = blob->next)
	     { 
	       cdrtext(blob->ref,tabs,0,100);  
	       addstr("\n");
	     }
	   put_tabs(tabs);  addstr("} ");
	 }
     put_left(buffer,temp_buf);
     clean(buffer);
     bp = bp_save;
     put_left(temp1_buf,buffer);
     q = (PTR_TYPE) NULL ;
     break;
  default :	 sprintf(buffera,"unexpected type");
      }
   }
   put_left(left_mod, temp_buf);
   addstr(temp_buf);
}

static
void cunp_llnd(pllnd)
PTR_LLND	pllnd;
{
  PTR_LLND pll2;
  char ch;  
	if (pllnd == NULL) return;

	switch (pllnd->variant) {
	case INT_VAL	:
	    { char sb[64];

	      sprintf(sb, "%d", pllnd->entry.ival);
	      addstr(sb);
	      break;
	    }
	case STMT_STR	: break ;
	case FLOAT_VAL	:
	case DOUBLE_VAL :
		addstr(pllnd->entry.string_val);
		break;
	case STRING_VAL :
		*bp++ = '"';
		sprintf(buffera, "%s", pllnd->entry.string_val);
		addstr(buffera);
		*bp++ = '"';
		break;
	case BOOL_VAL	:
		break;
	case CHAR_VAL	:
		ch = pllnd->entry.cval;
		switch (ch) {
		case  '\t': addstr("\'\\");  addstr("t\'"); return;
		case '\n':  addstr("\'\\"); addstr("n\'"); return;
		case '\b':  addstr("\'\\"); addstr("b\'"); return;
		case '\f':  addstr("\'\\"); addstr("f\'"); return;
		case '\r':  addstr("\'\\"); addstr("r\'"); return;
		case '\0':  addstr("\'\\"); addstr("0\'"); return;
		case '\\':  addstr("\'\\"); addstr("\\"); addstr("\'"); return;
		case '\'':  addstr("\'\\"); addstr("\'\'"); return;
		default: break;
		}
		sprintf(buffera, "\'%c\'",pllnd->entry.cval);
		addstr(buffera);
		break;
	case THIS_NODE:
	       addstr("this");
	       break;
	case CONST_REF	:
	case VAR_REF	:
	case ENUM_REF	:
		addstr(pllnd->entry.Template.symbol->ident);
		break;
	case RECORD_REF:		    
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = '.';
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break ;
	case ARRAY_OP :
		*bp++ = '(';
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		for (pll2 = pllnd->entry.Template.ll_ptr2;pll2; pll2= pll2->entry.Template.ll_ptr2) {
			*bp++ = '[';
			cunp_llnd(pll2->entry.Template.ll_ptr1);
			*bp++ = ']';
		}
		*bp++ = ')';
		break;

	case ARRAY_REF	:
		addstr(pllnd->entry.array_ref.symbol->ident);
		for (pll2 = pllnd->entry.Template.ll_ptr1;pll2; pll2= pll2->entry.Template.ll_ptr2) {
			*bp++ = '[';
			cunp_llnd(pll2->entry.Template.ll_ptr1);
			*bp++ = ']';
		}
		break;
	case CONSTRUCTOR_REF	:
		break;
	case ACCESS_REF :
		break;
	case CONS	:
		break;
	case ACCESS	:
		break;
	case IOACCESS	:
		break;
	case PROC_CALL	:
	case FUNC_CALL	:
		addstr(pllnd->entry.Template.symbol->ident);
		*bp++ = '(';
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = ')';
		break;
	case EXPR_LIST	:
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		if (pllnd->entry.Template.ll_ptr2) {
			addstr(",");
			cunp_llnd(pllnd->entry.Template.ll_ptr2);
		}
		break;
	case EQUI_LIST	:
		break;
	case COMM_LIST	:
		break;
	case VAR_LIST	:
	case CONTROL_LIST	:
		break;
	case RANGE_LIST :
		*bp++ = '[';
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = ']';
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case DDOT	:
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(":");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case COPY_NODE :
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("#");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case VECTOR_CONST :	/* NEW ADDED FOR VPC++	*/
		addstr("[ ");
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(" ]"); 
		break ;
	case INIT_LIST:
		addstr("{ ");
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(" }"); 
		break ;
	case BIT_NUMBER:
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(" : ");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break ;
	case DEF_CHOICE :
	case SEQ	:
		break;
	case SPEC_PAIR	:
		break;


	case EQ_OP	:
	case LT_OP	:
	case GT_OP	:
	case NOTEQL_OP	:
	case LTEQL_OP	:
	case GTEQL_OP	:
	case ADD_OP	:
	case SUBT_OP	:
	case OR_OP	:
	case MULT_OP	:
	case DIV_OP	:
	case AND_OP	:
	case XOR_OP	:
	case POINTST_OP :	/* New added for VPC */
	case LE_OP	:	/* New added for VPC *//*Duplicated*/
	case GE_OP	:	/* New added for VPC *//*Duplicated*/
	case NE_OP	:	/* New added for VPC *//*Duplicated*/

	case PLUS_ASSGN_OP:
	case MINUS_ASSGN_OP:
	case AND_ASSGN_OP:
	case  IOR_ASSGN_OP:
	case  MULT_ASSGN_OP:
	case  DIV_ASSGN_OP:
	case MOD_ASSGN_OP:
	case  XOR_ASSGN_OP:
	case  LSHIFT_ASSGN_OP:
	case  RSHIFT_ASSGN_OP :

	case ARITH_ASSGN_OP:
	case ASSGN_OP	:	/* New added for VPC */
	case BITAND_OP	  :	/* New added for VPC */
	case BITOR_OP	  :	/* New added for VPC */
	case LSHIFT_OP	       : /* New added for VPC */
	case RSHIFT_OP	       : /* New added for VPC */
	case MOD_OP :	/* New added for VPC */
	    {
		    int i, j ;
		    PTR_LLND p;

		    i = pllnd->variant ;
		    p = pllnd->entry.Template.ll_ptr1 ;
		    j = p->variant;
		    if ( cprecedence(i) < cprecedence(j) ) {
			    *bp++ = '(';
			    cunp_llnd(p);
			    *bp++ = ')';
			    if (pllnd->variant != ARITH_ASSGN_OP)
				    addstr(cop_name[mapping(i)] );
			    else
				    gen_op(pllnd->entry.Template.symbol->variant); 
		    } else {
			    cunp_llnd(p);
			    if (pllnd->variant != ARITH_ASSGN_OP)
				    addstr(cop_name[mapping(i)]);
			    else
				    gen_op(pllnd->entry.Template.symbol->variant);
		    }
		    p = pllnd->entry.Template.ll_ptr2;
		    j = p->variant;
		    if ( cprecedence(i) <= cprecedence(j)) {
			    *bp++ = '(';
			    cunp_llnd(p);
			    *bp++ = ')';
		    } else
			    cunp_llnd(p);
		    break ;
	    }
	case SUB_OP   : 	/* duplicated unary minus  */
	case MINUS_OP	:	/* unary operations */
	case UNARY_ADD_OP      : /* New added for VPC */
	case BIT_COMPLEMENT_OP : /* New added for VPC */
	case NOT_OP	:
	case DEREF_OP :
	case SIZE_OP	  :	/* New added for VPC */
	case ADDRESS_OP   :	/* New added for VPC */
	    {
		    int i, j;
		    PTR_LLND p;

		    i = pllnd->variant ;
		    p = pllnd->entry.Template.ll_ptr1 ;
		    j = p->variant;
		    addstr(cop_name[mapping(i)] );
		    if ( cprecedence(i) < cprecedence(j) ) {
			    *bp++ = '(';
			    cunp_llnd(p);
			    *bp++ = ')';
		    } else
			    cunp_llnd(p);
	    } 
		break;
	case SAMETYPE_OP      : 	      /* New added for VPC */
	    addstr("SameType (");
	    cunp_llnd(pllnd->entry.Template.ll_ptr1);
	    addstr(" , ");
	    cunp_llnd(pllnd->entry.Template.ll_ptr2);
	    addstr(")");
		break;
	case MINUSMINUS_OP:	/* New added for VPC */
	case PLUSPLUS_OP  :	/* New added for VPC */
	    {
		    int i ,j ;
		    PTR_LLND p;

		    i = pllnd->variant;
		    if	( (p = pllnd->entry.Template.ll_ptr1) != 0) {
			    j = p->variant;
			    addstr(cop_name[mapping(i)] );
			    if ( cprecedence(i) < cprecedence(j) ) {
				    *bp++ = '(';
				    cunp_llnd(p);
				    *bp++ = ')';
			    } else
				    cunp_llnd(p);
		    } else {
			    p = pllnd->entry.Template.ll_ptr2 ;
			    j = p->variant;
			    if ( cprecedence(i) < cprecedence(j) ) {
				    *bp++ = '(';
				    cunp_llnd(p);
				    *bp++ = ')';
			    } else
				    cunp_llnd(p);
			    addstr(cop_name[mapping(i)] );
		    }
	    } 
		break;

	case STAR_RANGE :
		addstr(" : ");
		break;
	case FUNCTION_OP :	/* New added for VPC */
		*bp++ = '(';
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = ')';		    
		*bp++ = '(';
		cunp_llnd(pllnd->entry.Template.ll_ptr2);   
		*bp++ = ')';		    
		break ;
	case CLASSINIT_OP :	/* New added for VPC */
	    {
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = '(';
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		*bp++ = ')';
	    }
		break ;
	case DELETE_OP:
		 addstr("delete ");
		 if (pllnd->entry.Template.ll_ptr2) {
		   *bp++ ='[';
		   cunp_llnd(pllnd->entry.Template.ll_ptr2);
		   addstr("] ");
		 }
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case SCOPE_OP:
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("::");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case NEW_OP:
		{ PTR_LLND pllnd1;
		  addstr("new ");
		  pllnd1 = pllnd->entry.Template.ll_ptr1;
		  gen_simple_type_2(pllnd1->type,BFNULL,global_tab);
		  if (pllnd->entry.Template.ll_ptr2) {
		    *bp++= '(';
		    cunp_llnd(pllnd->entry.Template.ll_ptr2);
		    addstr(") ");
		  }
		  break;
		}
	case CAST_OP :		/* New added for VPC */
		*bp++ = '(';
		gen_simple_type_2(pllnd->type, BFNULL, global_tab);
		*bp++ = ')';
		*bp++ = ' ';
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case EXPR_IF	  :	/* New added for VPC */
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(" ? ");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case EXPR_IF_BODY :	/* New added for VPC */
		cunp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(" : ");
		cunp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case FUNCTION_REF :	/* New added for VPC */
		addstr(pllnd->entry.Template.symbol->ident);
		*bp++ = '(';
		/*		cunp_llnd(pllnd->entry.Template.ll_ptr1);   */
		*bp++ = ')';
		break ;
	case LABEL_REF: 	/* Fortran Version, For VPC we need more */
	    { char sb[64];

	      sprintf(sb, "%d", (int)(pllnd->entry.label_list.lab_ptr->stateno));
	      addstr(sb);
	      break;
	    }
	default :
		break;
	}
}

static int
is_param_decl(var_bf, functor)
	PTR_BFND var_bf ;
	PTR_SYMB functor ;
{
	PTR_LLND flow_ptr,lpr ;
	PTR_SYMB s ;

	switch (var_bf->variant) {
	case VAR_DECL :
	case ENUM_DECL:
	case CLASS_DECL:
	case UNION_DECL:
	case STRUCT_DECL:
	case DERIVED_CLASS_DECL :
		lpr = var_bf->entry.Template.ll_ptr1 ;
		for (flow_ptr = lpr; flow_ptr ; flow_ptr = flow_ptr->entry.Template.ll_ptr1) {
			if ((flow_ptr->variant == VAR_REF) ||
			    (flow_ptr->variant == ARRAY_REF) ||
			    (flow_ptr->variant == FUNCTION_REF) ) break ;
		}
		if (!flow_ptr)
			return(0);

		for (s = functor->entry.member_func.in_list; s ;  s = s->entry.var_decl.next_in)
			if (flow_ptr->entry.Template.symbol == s)
				return(1);
		break;
	default :
		break;
	}
	return(0) ;
}


static int
this_is_decl(variant)
int variant ;
{
	switch(variant) {
	case CLASS_DECL :
	case UNION_DECL :
	case STRUCT_DECL :
	case ENUM_DECL :
	case VAR_DECL :
	case DERIVED_CLASS_DECL:
		return(1);
	default :
		break;
	}
	return(0);
}


static int
not_explicit(s, pbf)
	PTR_SYMB  s ;
	PTR_BFND pbf ;
{
	PTR_BLOB blob ;
	PTR_LLND lptr1;
	PTR_SYMB symbptr;

	for (blob = pbf->entry.Template.bl_ptr1 ; blob ; blob = blob->next ) {
		if (!this_is_decl(blob->ref->variant )) return(1);
		for (lptr1=blob->ref->entry.Template.ll_ptr1 ; lptr1; lptr1 = lptr1->entry.Template.ll_ptr2) {
			symbptr = find_declarator(lptr1);
			if ( s == symbptr) return(0);
		}
	}
	return(1);
}


static int
not_class(pbf)
PTR_BFND pbf;
{
  switch(pbf->variant) {
  case GLOBAL :
  case CLASS_DECL :
  case UNION_DECL :
  case STRUCT_DECL :
  case ENUM_DECL :
  case FUNC_HEDR :
  case DERIVED_CLASS_DECL:  return(0);
    default :		    return(1);
  }
}

int cdrtext(bfptr,tab,curh,maxh)
PTR_BFND bfptr;
int tab,curh,maxh;
{
        int lev;
        register PTR_BLOB b;
        /*   register PTR_BLOB  p; */
        int      left_param ;
	int token = 0;

        left_param = 0;
        lev = maxh-curh;

        global_tab = tab ;
        cunp_bfnd(tab, bfptr);
        global_tab = tab ;
/*
        if ((current_proc == global_bfnd) && (bfptr->control_parent == global_bfnd))
                return(token);
*/

        if ((basket_needed(bfptr,1) > 1)&&(not_class(bfptr)))
           {   put_tabs(tab); 
	      addstr("{ \n");
	   }
	  
        for (b = bfptr->entry.Template.bl_ptr1; b; b = b->next)
	  {
/*               PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;
                 if (cmnt)
                       while (cmnt != NULL && cmnt->type == FULL) {
                                        addstr(cmnt->string );
                                        addstr( "\n" );
                                        cmnt = cmnt->next;
                                }
*/
                  switch(bfptr->variant){
	          case CLASS_DECL :
		  case COLLECTION_DECL:
                  case UNION_DECL :
                  case ENUM_DECL:
                  case STRUCT_DECL :
                  case DERIVED_CLASS_DECL : break ;
                  case FUNC_HEDR :
                      if (left_param==0)
                            {
                             if (!is_param_decl(b->ref,bfptr->entry.Template.symbol))
			       {   put_tabs(tab); addstr("{ \n");
                                   left_param= 1 ;
				 }

                           }
                       token = cdrtext(b->ref,tab+1,curh+1,maxh);
                       break ; 
                  default :
                          token = cdrtext(b->ref,tab+1,curh+1,maxh);
	          }
/*                 if (cmnt && cmnt->type != FULL)
                     {  addstr( cmnt->string );
                        addstr( "\n" );
		      }
*/
	     } 
       if (bfptr->variant == FUNC_HEDR)
         {
                if (left_param == 0)  {
                         put_tabs(tab); addstr("{ \n");
		       }
                put_tabs(tab); addstr("} \n");
	    }

        if ((basket_needed(bfptr, 1) > 1)&&(not_class(bfptr)))
           {   put_tabs(tab); addstr("} \n");
	   }

        if (basket_needed(bfptr,2) > 0)
	  {    put_tabs(tab); addstr("else \n");
	     }
        if (basket_needed(bfptr,2) > 1)
           {   put_tabs(tab); addstr("{ \n");
	   }


        for (b = bfptr->entry.Template.bl_ptr2; b; b = b->next)
          {                  
            /*  PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;*/
                       token = cdrtext(b->ref,tab+1,curh+1,maxh);
   
          }

        if (basket_needed(bfptr,2) > 1)
           {   put_tabs(tab); addstr("} \n");
	   }
/*        if (cmnt && cmnt->type != FULL)
            while  (cmnt && cmnt->type != FULL)
                     {  tm_put_string(Wid,cmnt->string,token);
                        cmnt =cmnt->next ;
		      }
         addstr( "\n" );
*/
        return (token);


 }






static int 
basket_needed(bf, index)
	PTR_BFND  bf ;
	int index ;
{
	PTR_BLOB blob1 ,blob ;

	switch (index) {
	case 1 :
		if (bf->variant == FUNC_HEDR || bf->variant == BASIC_BLOCK)
			return(2);
		blob = bf->entry.Template.bl_ptr1 ;
		if (blob == NULL)  return(0) ;
		if (((blob1= blob->next) == NULL) || 
		    (blob1->ref->variant == CONTROL_END)) return(1);
		break;
	case 2 :
		blob = bf->entry.Template.bl_ptr2 ;
		if (!blob)  return(0) ;
		if (((blob1= blob->next) == NULL) ||
		    (blob1->ref->variant == CONTROL_END)) return(1);
		break;
	}
	return(2) ;
}


static void
cunp_blck(bfptr, tab)
	PTR_BFND bfptr;
	int	 tab;
{
    PTR_BLOB b;
    int  left_param ;

    left_param = 0;
    cunp_bfnd(tab, bfptr);

    if ((basket_needed(bfptr,1) > 1)&&(not_class(bfptr))) {
	put_tabs(tab);
	addstr("{\n");
    }
	  
    for (b = bfptr->entry.Template.bl_ptr1; b; b = b->next) {
	switch(bfptr->variant) {
	case CLASS_DECL :
	case UNION_DECL :
	case ENUM_DECL:
	case STRUCT_DECL :
	case DERIVED_CLASS_DECL :
	    break ;
	case FUNC_HEDR :
	    if (left_param==0)
		if (!is_param_decl(b->ref,bfptr->entry.Template.symbol)) {
		    put_tabs(tab);
		    addstr("{\n");
		    left_param= 1 ;
		}
	    cunp_blck(b->ref, tab+1);
	    break ;
	case CONTROL_END:
	    break; 
	default :
	    cunp_blck(b->ref, tab+1);
	    break;
	}
    } 
    if (bfptr->variant == FUNC_HEDR) {
	if (left_param == 0) {
	    put_tabs(tab);
	    addstr("{\n");
	}
	put_tabs(tab);
	addstr("}\n");
    }

    if ((basket_needed(bfptr, 1) > 1)&&(not_class(bfptr))) {
	put_tabs(tab);
	addstr("}\n");
    }

    if (basket_needed(bfptr,2) > 0) {
	put_tabs(tab);
	addstr("else\n");
    }
    if (basket_needed(bfptr,2) > 1) {
	put_tabs(tab);
	addstr("{\n");
    }

    for (b = bfptr->entry.Template.bl_ptr2; b; b = b->next)
	cunp_blck(b->ref, tab+1);

    if (basket_needed(bfptr,2) > 1) {
	put_tabs(tab);
	addstr("}\n");
    }
}


/* find_declarator :
 * <1> Given a ll_node <expr_list> to follow ll_ptr1 to find declarator
 * <2> return the symb pointer
 */
static PTR_SYMB
find_declarator(expr_list)
	PTR_LLND expr_list ;
{
	PTR_SYMB symb;
	PTR_LLND p ;

	if (! expr_list)
		return(SMNULL);
	symb = SMNULL ;
	for ( p = expr_list->entry.Template.ll_ptr1 ; p ;   ) {
	    switch (p->variant) {
		case BIT_NUMBER:
		case ASSGN_OP :
		case ARRAY_OP:
		case FUNCTION_OP :
		case CLASSINIT_OP:
		case ADDRESS_OP:
		case DEREF_OP : p = p->entry.Template.ll_ptr1 ;
			break ;
		case FUNCTION_REF:
		case ARRAY_REF:
		case VAR_REF:
			symb = p->entry.Template.symbol ;
			p = LLNULL ;
			break ;
		}
	}
	return(symb);
}


static void
gen_func_hedr(functor, pbf, tabs)
	PTR_SYMB functor ;
	PTR_BFND pbf ;
	int	 tabs ;
{
	PTR_SYMB s ;
	PTR_TYPE q ;
	PTR_LLND pllnd;
	int i;

	for (q = functor->type; q ;	) { 
		switch ( q->variant) {
		case T_POINTER : 
			*bp++ = '*';
			q = q->entry.Template.base_type ;
			break;
                case T_REFERENCE:
                       *bp++ = '&';
                       q = q->entry.Template.base_type ;
                       break;
		default:	/* It might need more for complicated case */
			q = (PTR_TYPE) NULL ;
		}
	}
        if (is_scope_op_needed(pbf,functor)) {
           addstr(functor->entry.member_func.base_name->ident);
           addstr("::");
        }
	addstr(functor->ident);
	*bp++ = '(';
	for ( i=0, s = functor->entry.member_func.in_list ; s ; i++ ) {
		if (i)	*bp++ = ',';  
		if (not_explicit(s, pbf)) {
			gen_simple_type(s->type, BFNULL, tabs);
			for (q = s->type; q ;	) {
				switch ( q->variant) {
				case T_POINTER : 
					*bp++ = '*';
					q = q->entry.Template.base_type;
					break;
	                        case T_REFERENCE:
                                        *bp++ ='&';
                                         q = q->entry.Template.base_type ;
                                         break;
				default: /* It might need more for complicated case */
					q = (PTR_TYPE) NULL;
				}
			}
	  
		}
		addstr(s->ident);
		s = s->entry.var_decl.next_in;
	}
	*bp++ = ')';
        pllnd = pbf->entry.Template.ll_ptr1;
        pllnd = pllnd->entry.Template.ll_ptr1;
        if (pllnd &&(pllnd->variant == BIT_NUMBER)){
            addstr(" : ");
            cunp_llnd(pllnd->entry.Template.ll_ptr2);
            }
} 

int
is_scope_op_needed(pbf,functor)
PTR_BFND pbf;
PTR_SYMB functor;
{
  PTR_BFND parent;

  if (functor->variant!=MEMBER_FUNC) return(0);
  parent = pbf->control_parent;
  if (parent->variant==GLOBAL)       return(1);
  else                               return(0);

}

char *
cunparse_llnd(llnd)
	PTR_LLND llnd;
{
	int len;
	char *p;

	bp = buffer;	/* reset the buffer pointer */
	cunp_llnd(llnd);
	*bp++ = '\n';
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer);	/* copy the buffer for output */
	*buffer = '\0';
	return p;
}


char *
cunparse_bfnd(bif)
	PTR_BFND bif;
{
	char *p;
	int len;

	first = 1;	/* Mark this is the first bif node */
	bp = buffer;	/* reset the buffer pointer */
	cunp_bfnd(0, bif) ;
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer);	/* copy the buffer for output */
	*buffer = '\0';
	return p;

}


static void
gen_declarator(s)
	PTR_SYMB s ;
{
	PTR_TYPE q ;
	char * old_bp ;

	clean(temp_buf);
	put_right(s->ident,temp_buf);
	for (q = s->type; q ;	) { 
		switch ( q->variant) {
		case T_POINTER : 
			put_left("*",temp_buf);
			q = q->entry.Template.base_type ;
			break;
		case T_ARRAY :
			clean(temp2_buf);
			put_right(buffer,temp2_buf);
			clean(buffer);
			old_bp = bp ;
			bp = buffer ;
			cunp_llnd(q->entry.ar_decl.ranges);
			bp = old_bp;
			put_right(buffer,temp_buf);
			clean(buffer);
			put_right(temp2_buf,buffer);
			q = q->entry.Template.base_type ;
			break;
		case T_FUNCTION:
			put_left("(",temp_buf);
			put_right(")",temp_buf);
			put_right("()",temp_buf);
			q = q->entry.Template.base_type ;
			break;

		default:	/* It might need more for complicated case */
			q = (PTR_TYPE) NULL ;
		}
	}
	addstr(temp_buf);	
}


char *
cunparse_symb(symb)
	PTR_SYMB symb;
{
	int len;
	char *p;

	first = 1;		/* Mark this is the first bif node */
	bp = buffer;		/* reset the buffer pointer */
	gen_simple_type(symb->type,BFNULL,0);
	gen_declarator(symb);
	*bp++ = '\n';
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer);	/* copy the buffer for output */
	*buffer = '\0';
	return p;
}


/****************************************************************
 *								*
 *	    for cunparse_type					*
 *								*
 ****************************************************************/

char *
cunparse_type(q_type)
PTR_TYPE q_type;
{
	int len;
	char *p;

	first = 1;	/* Mark this is the first bif node */
	bp = buffer;	/* reset the buffer pointer */
	gen_simple_type_2(q_type,BFNULL,0);
	*bp++ = '\n';
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer);	/* copy the buffer for output */
	*buffer = '\0';
	return p;
}


char *
cunparse_blck(bif)
	PTR_BFND bif;
{
	int len;
	char *p;

	first = 1;		/* Mark this is the first bif node */
	bp = buffer;		/* reset the buffer pointer */

	cunp_blck(bif, 0);
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer);	/* copy the buffer for output */
	*buffer = '\0';
	return (p);
}
