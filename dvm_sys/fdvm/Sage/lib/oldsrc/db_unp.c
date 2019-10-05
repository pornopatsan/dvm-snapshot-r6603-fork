/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/****************************************************************
 *								*
 *  db_unp.c -- contains the procedures required to unparse the *
 *		bif graph back to source form for Fortran	*
 *								*
 ****************************************************************/

#include <stdlib.h>
#include "db.h"
#include "f90.h"

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
#define NULLTEST(VAR)	(VAR == NULL? -1 : VAR->id)
#define type_index(X)	(X-T_INT)
#define binop(n)	(n >= EQ_OP && n <= NEQV_OP)
 
PTR_SYMB cur_symb_head;		/* point to the head of the list of symbols */
				/* used to search type that LIKE the current*/

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

int   figure_tabs();
#define BUFLEN 500000
char buffer[BUFLEN], *bp;

static int	 in_param = 0;	/* set if unparsing the parameter statement */
static int	 in_impli = 0;	/* set if unparsing the implicit statement */
static PTR_CMNT  cmnt = NULL;	/* point to chain of comment list */
static int	 print_comments = 1; /* 0 if no comments */
static char	 first = 1;	/* used when unparsing LOGGOTO which has two */
				/*  ...  bif nodes */

/*
 * Forward references
 */
static void unp_llnd();


/*
 * Ascii names for operators in the language
 */
static
char *fop_name[] = {
	" .eq. ",
	" .lt. ",
	" .gt. ",
	" .ne. ",
	" .le. ",
	" .ge. ",
	"+",
	"-",
	" .or. ",
	"*",
	"/",
	"",
	" .and. ",
	"**",
	"",
	"//",
	" .xor. ",
	" .eqv. ",
	" .neqv. "
};


/*
 * Precedence table of operators for Fortran
 */
static
char  precedence[] = {	/* precedence table of the operators */
	5,		/* .eq. */
	5,		/* .lt. */
	5,		/* .gt. */
	5,		/* .ne. */
	5,		/* .le. */
	5,		/* .ge. */
	3,		/*  +   */
	3,		/*  -   */
	8,		/* .or. */
	2,		/*  *   */
	2,		/*  /   */
	0,		/* none */
	7,		/* .and. */
	1,		/*  **  */
	0,		/* none */
	4,		/*  //  */
	8,		/* .xor. */
	9,		/* .eqv. */
	9		/* .neqv. */
};


/*
 * Type names in ascii form
 */
static
char *ftype_name[] = {
	"integer",
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
};

	
/****************************************************************
 *								*
 *  put_tabs -- indent the statement by putting some blanks	*
 *								*
 *  Input:							*
 *	   n - number of tabs wanted				*
 *								*
 ****************************************************************/
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


/****************************************************************
 *								*
 *  figure_tabs -- figure out the indentation level of the	*
 *		    given bif node				*
 *								*
 *  Input:							*
 *	bf - the bif node pointer				*
 *								*
 *  Output:							*
 *	an integer indicating the indentation level		*
 *								*
 ****************************************************************/
int
figure_tabs(bf)
	PTR_BFND bf;
{
	int count = 0;

	while(bf->variant != PROG_HEDR && bf->variant != PROC_HEDR &&
	      bf->variant != FUNC_HEDR && bf->variant != GLOBAL){
		if(bf->variant != ELSEIF_NODE) count++;
		bf = bf->control_parent;
	}
	return(count);
}


/****************************************************************
 *								*
 *  addstr -- add the string "s" to output buffer		*
 *								*
 *  Input:							*
 *	   s  - the string to be appended to the buffer		*
 *								*
 *  Side effect:						*
 *	   bp - points to where next character will go		*
 *								*
 ****************************************************************/
static void
addstr(s)
	char *s;
{
	while( (*bp = *s++) != 0)
		bp++;
}


/*
 * pr_ftype_name(ptype) -- print out the variable type.
 */
static int
pr_ftype_name(ptype, def)
	PTR_TYPE ptype;
	int	 def;  /* def = 1 means it is a type define,
			  	print the whole type
			  def = 0 : the type has a name.    */

{	int gen_rec_decl ();


	if (ptype == NULL)  return(0);

	if (def == 0 && ptype->name) { /* print the type name */
	   addstr (ptype->name->ident);
	   return(1);
	}   

	switch (ptype->variant) {
	case T_INT   :
	case T_FLOAT :
	case T_DOUBLE:
	case T_CHAR  :
	case T_BOOL  :
	case T_STRING:
	case T_COMPLEX:
		addstr (ftype_name[ptype->variant - T_INT]);
		break;
	case T_DCOMPLEX:
		addstr (ftype_name[ptype->variant - T_INT]);
		break;
	case T_GATE:
		addstr ("gate");
		break;
	case T_EVENT:
		addstr ("event");
		break;
	case T_SEQUENCE:
		addstr ("sequence");
		break;
	case T_ARRAY :
		pr_ftype_name (ptype->entry.ar_decl.base_type, 0);
		break;
        case T_DERIVED_TYPE:
		addstr("type (");
		addstr(ptype->name->ident);
		addstr(")");
		break;
        case T_POINTER:
		pr_ftype_name(ptype->entry.Template.base_type,0);
		break;
		
	default :
		return 0;
	}
	return (1);
}
     

static void
gen_loop_header(looptype, pbf)
	char	*looptype;
	PTR_BFND pbf;
{
	char	label[7];

	addstr(looptype);
	if ((pbf->variant == PARDO_NODE) || (pbf->variant == PDO_NODE))
           if (pbf->entry.for_node.where_cond)
              {
                 addstr(" ( ");
	         unp_llnd(pbf->entry.for_node.where_cond);
                 addstr(" ) ");
	      }
	if (pbf->entry.for_node.doend) {
		sprintf(label,"%d ",(int)(pbf->entry.for_node.doend->stateno));
		addstr(label);
	}
	addstr(pbf->entry.for_node.control_var->ident);
	addstr(" = ");
	unp_llnd(pbf->entry.for_node.range->entry.binary_op.l_operand);
	addstr(", ");
	unp_llnd(pbf->entry.for_node.range->entry.binary_op.r_operand);
	if (pbf->entry.for_node.increment) {
		addstr(" , ");
		unp_llnd(pbf->entry.for_node.increment);
	}
}


/*
 * gen_if_node(pbf) --- generate the if statement pointed to by pbf.
 */
static void
gen_branch(branch_tag, branch_type, pbf)
	int	  branch_tag;
	char	 *branch_type;
	PTR_BFND  pbf;
{
	addstr(branch_type);
	*bp++ = '(';
	unp_llnd(pbf->entry.if_node.condition);
	*bp++ = ')';
	if (branch_tag != WHERE_BLOCK_STMT)
		addstr(" then");
}


/****************************************************************
 *								*
 *  unp_llnd -- unparse the given low level node to source	*
 *		string						*
 *								*
 *  Input:							*
 *	   pllnd - low level node to be unparsed		*
 *	   bp (implicitely) - where the output string to be	*
 *		   placed					*
 *								*
 *  Output:							*
 *	   the unparse string where "bp" was pointed to		*
 *								*
 *  Side Effect:						*
 *	   "bp" will be updated to the next character behind	*
 *	   the end of the unparsed string (by "addstr")		*
 *								*
 ****************************************************************/
static void
unp_llnd(pllnd)
	PTR_LLND pllnd;
{
	if (pllnd == NULL) return;

	switch (pllnd->variant) {
	case INT_VAL	:
		{ char sb[64];

		  sprintf(sb, "%d", pllnd->entry.ival);
		  addstr(sb);
		  break;
		}
	case LABEL_REF:
		{ char sb[64];

		  sprintf(sb, "%d",(int)( pllnd->entry.label_list.lab_ptr->stateno));
		  addstr(sb);
		  break;
		}
	case FLOAT_VAL	:
	case DOUBLE_VAL	:
	case STMT_STR	:
		addstr(pllnd->entry.string_val);
		break;		  
	case STRING_VAL	:
	        *bp++ = '\'';
		addstr(pllnd->entry.string_val);
		*bp++ = '\'';
		break;
	case COMPLEX_VAL	:
	        *bp++ = '(';
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = ',';
		unp_llnd(pllnd->entry.Template.ll_ptr2);
		*bp++ = ')';
		break;
	case KEYWORD_VAL	:
		addstr(pllnd->entry.string_val);
		break;
	case KEYWORD_ARG	:
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("=");
		unp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case BOOL_VAL	:
		addstr(pllnd->entry.bval ? ".TRUE." : ".FALSE.");
		break;
	case CHAR_VAL	:
		if (! in_impli)
			*bp++ = '\'';
		*bp++ = pllnd->entry.cval;
		if (! in_impli)
			*bp++ = '\'';
		break;
	case CONST_REF	:
	case VAR_REF	:
	case ENUM_REF	:
	case TYPE_REF   :
	case INTERFACE_REF:
		addstr(pllnd->entry.Template.symbol->ident);
 /* Look out !!!! */
/* Purpose unknown. Commented out. */
/*
                if (pllnd->entry.Template.symbol->type->entry.Template.ranges != LLNULL)
		     unp_llnd(pllnd->entry.Template.symbol->type->entry.Template.ranges);
*/
		break;
	case ARRAY_REF	:
		addstr(pllnd->entry.array_ref.symbol->ident);
		if (pllnd->entry.array_ref.index) {
			*bp++ = '(';
			unp_llnd(pllnd->entry.array_ref.index);
			*bp++ = ')';
		}
		break;
	case ARRAY_OP	:
	        unp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = '(';
		unp_llnd(pllnd->entry.Template.ll_ptr2);
		*bp++ = ')';
		break;
	case RECORD_REF	:
	        unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("%");
		unp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case STRUCTURE_CONSTRUCTOR	:
	        addstr(pllnd->entry.Template.symbol->ident);
		*bp++ = '(';
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		*bp++ = ')';
		break;
	case CONSTRUCTOR_REF	:
		addstr("(/");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("/)");
		break;
	case ACCESS_REF	:
		unp_llnd(pllnd->entry.access_ref.access);
		if (pllnd->entry.access_ref.index != NULL) {
			*bp++ = '(';
			unp_llnd(pllnd->entry.access_ref.index);
			*bp++ = ')';
		}
		break;
        case OVERLOADED_CALL:
		break;
	case CONS	:
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(",");
		unp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case ACCESS	:
		unp_llnd(pllnd->entry.access.array);
		addstr(", FORALL=(");
		addstr(pllnd->entry.access.control_var->ident);
		*bp++ = '=';
		unp_llnd(pllnd->entry.access.range);
		*bp++ = ')';
		break;
	case IOACCESS	:
		*bp++ = '(';
		unp_llnd(pllnd->entry.ioaccess.array);
		addstr(", ");
		addstr(pllnd->entry.ioaccess.control_var->ident);
		*bp++ = '=';
		unp_llnd(pllnd->entry.ioaccess.range);
		*bp++ = ')';
		break;
	case PROC_CALL	:
	case FUNC_CALL	:
		addstr(pllnd->entry.proc.symbol->ident);
		*bp++ = '(';
		unp_llnd(pllnd->entry.proc.param_list);
		*bp++ = ')';
		break;
	case EXPR_LIST	:
		unp_llnd(pllnd->entry.list.item);
		if (in_param) {
			addstr("=");
			unp_llnd(pllnd->entry.list.item->entry.const_ref.symbol->entry.const_value);
		}
		if (pllnd->entry.list.next) {
			addstr(", ");
			unp_llnd(pllnd->entry.list.next);
		}
		break;
	case EQUI_LIST	:
		*bp++ = '(';
		unp_llnd(pllnd->entry.list.item);
		*bp++ = ')';
		if (pllnd->entry.list.next) {
			addstr(", ");
			unp_llnd(pllnd->entry.list.next);
		}
		break;
	case COMM_LIST	  :
	case NAMELIST_LIST:
		if (pllnd->entry.Template.symbol) {
			*bp++ = '/';
			addstr(pllnd->entry.Template.symbol->ident);
			*bp++ = '/';
		}
		unp_llnd(pllnd->entry.list.item);
		if (pllnd->entry.list.next) {
			addstr(", ");
			unp_llnd(pllnd->entry.list.next);
		}
		break;
	case VAR_LIST	 :
	case RANGE_LIST	 :
	case CONTROL_LIST:
		unp_llnd(pllnd->entry.list.item);
		if (pllnd->entry.list.next) {
			addstr(",");
			unp_llnd(pllnd->entry.list.next);
		}
		break;
	case DDOT	:
	        if (pllnd->entry.binary_op.l_operand)
		     unp_llnd(pllnd->entry.binary_op.l_operand);
		*bp++ = in_impli? '-' : ':';
		if (pllnd->entry.binary_op.r_operand) 
		     unp_llnd(pllnd->entry.binary_op.r_operand);
		break;
        case DEFAULT:
		addstr("default");
		break;
	case DEF_CHOICE	:
	case SEQ	:
		unp_llnd(pllnd->entry.seq.ddot);
		if (pllnd->entry.seq.stride) {
			*bp++ = ':';
			unp_llnd(pllnd->entry.seq.stride);
		}
		break;
	case SPEC_PAIR	:
		unp_llnd(pllnd->entry.spec_pair.sp_label);
		*bp++ = '=';
		unp_llnd(pllnd->entry.spec_pair.sp_value);
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
	case MOD_OP	:
	case AND_OP	:
	case EXP_OP	:
	case CONCAT_OP	:
		{
			int i = pllnd->variant - EQ_OP, j;
			PTR_LLND p;
			int num_paren = 0;

			p = pllnd->entry.binary_op.l_operand;
			j = p->variant;
			if (binop(j) && precedence[i] < precedence[j-EQ_OP]) {
				num_paren++;
				*bp++ = '(';
			}
			unp_llnd(p);
			if (num_paren) {
				*bp++ = ')';
				num_paren--;
			}
			addstr(fop_name[i]); /* print the op name */
			p = pllnd->entry.binary_op.r_operand;
			j = p->variant;
			if (binop(j) && precedence[i] <= precedence[j-EQ_OP]) {
				num_paren++;
				*bp++ = '(';
			}
			unp_llnd(p);
			if (num_paren) {
				*bp++ = ')';
				num_paren--;
			}
			break;
		}
	case MINUS_OP	:
		addstr(" -(");
		unp_llnd(pllnd->entry.unary_op.operand);
		*bp++ = ')';
		break;
	case UNARY_ADD_OP	:
		addstr(" +(");
		unp_llnd(pllnd->entry.unary_op.operand);
		*bp++ = ')';
		break;
	case NOT_OP	:
		addstr(" .not. (");
		unp_llnd(pllnd->entry.unary_op.operand);
		*bp++ = ')';
		break;
	case PAREN_OP:
		addstr("(");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(")");
	case ASSGN_OP:
		addstr("=");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
	case STAR_RANGE	:
		addstr(" : ");
		break;
	case IMPL_TYPE:
		pr_ftype_name(pllnd->type, 1);
		if (pllnd->entry.Template.ll_ptr1 != LLNULL)
		{
		     addstr("(");
		     unp_llnd(pllnd->entry.Template.ll_ptr1);
		     addstr(")");
		}
		break;
	case ORDERED_OP :
		addstr("ordered ");
		break;
	case EXTEND_OP :
		addstr("extended ");
		break;
        case MAXPARALLEL_OP:
		addstr("max parallel = ");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case PARAMETER_OP :
		addstr("parameter ");
		break;
	case PUBLIC_OP :
		addstr("public ");
		break;
	case PRIVATE_OP :
		addstr("private ");
		break;
	case ALLOCATABLE_OP :
		addstr("allocatable ");
		break;
	case DIMENSION_OP :
		addstr("dimension (");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr(")");
		break;
	case EXTERNAL_OP :
		addstr("external ");
		break;
	case OPTIONAL_OP :
		addstr("optional ");
		break;
	case IN_OP :
		addstr("intent (in) ");
		break;
	case OUT_OP :
		addstr("intent (out) ");
		break;
	case INOUT_OP :
		addstr("intent (inout) ");
		break;
	case INTRINSIC_OP :
		addstr("intrinsic ");
		break;
	case POINTER_OP :
		addstr("pointer ");
		break;
	case SAVE_OP :
		addstr("save ");
		break;
	case TARGET_OP :
		addstr("target ");
		break;
	case LEN_OP :
		addstr("*");
		unp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case TYPE_OP :
		pr_ftype_name(pllnd->type, 1);
		unp_llnd(pllnd->type->entry.Template.ranges);
		break;
	case ONLY_NODE :
  	        addstr("only: ");
		if (pllnd->entry.Template.ll_ptr1)
		   unp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case DEREF_OP :
   	        unp_llnd(pllnd->entry.Template.ll_ptr1);
		break;
	case RENAME_NODE :
   	        unp_llnd(pllnd->entry.Template.ll_ptr1);
		addstr("=>");
   	        unp_llnd(pllnd->entry.Template.ll_ptr2);
		break;
	case VARIABLE_NAME :
	        addstr(pllnd->entry.Template.symbol->ident);
		break;
	default		:
		fprintf(stderr,"unp_llnd -- bad llnd ptr %d!\n",pllnd->variant);
		break;
	}
}


/****************************************************************
 *								*
 *  funp_bfnd -- unparse the given bif node to source string	*
 *								*
 *  Input:							*
 *	   tabs- number of tabs (2 spaces) for indenting	*
 *	   pbf - bif node to be unparsed			*
 *	   bp (implicitely) - where the output string to be	*
 *		   placed					*
 *								*
 *  Output:							*
 *	   the unparse string where "bp" was pointed to		*
 *								*
 *  Side Effect:						*
 *	   "bp" will be updated to the next character behind	*
 *	   the end of the unparsed string (by "addstr")		*
 *								*
 ****************************************************************/
static void
funp_bfnd(tabs,pbf)
	int tabs;
	PTR_BFND     pbf;
{
	PTR_SYMB  s;
 
	if (pbf == NULL) return;
	if (pbf->label) {
		char b[10];

		sprintf(b ,"%-5d ", (int)(pbf->label->stateno));
		addstr(b);
	} else
		addstr("      ");

	put_tabs(tabs);
	switch (pbf->variant) {
	case GLOBAL	:
		break;
	case PROG_HEDR	:		/* program header	*/
	        addstr("program ");
		if (pbf->entry.program.prog_symb &&
		    strcmp(pbf->entry.program.prog_symb->ident, (char *)"_MAIN")) {
			addstr(pbf->entry.program.prog_symb->ident);
		}
		break;
	case BLOCK_DATA	:		
	        addstr("block data ");
		if (pbf->entry.program.prog_symb &&
		    strcmp(pbf->entry.program.prog_symb->ident, (char *)"_BLOCK")) {
			addstr(pbf->entry.program.prog_symb->ident);
		}
		break;
	case PROC_HEDR	: 
	        if (pbf->entry.procedure.proc_symb->attr & RECURSIVE_BIT) 
		   addstr("recursive");
		addstr("subroutine ");
		addstr(pbf->entry.procedure.proc_symb->ident);
		*bp++ = '(';
		s = pbf->entry.procedure.proc_symb->entry.proc_decl.in_list;
		while (s) {
			addstr(s->ident);
			s = s->entry.var_decl.next_in;
			if (s) *bp++ = ',';
		}
		*bp++ = ')';
		break;
	case FUNC_HEDR	:
	        if (pbf->entry.function.func_symb->attr & RECURSIVE_BIT) 
		   addstr("recursive");
		addstr(ftype_name[type_index(pbf->entry.function.func_symb->type->variant)]);
		addstr(" function ");
		addstr(pbf->entry.function.func_symb->ident);
		*bp++ = '(';
		s = pbf->entry.function.func_symb->entry.proc_decl.in_list;
		while (s) {
			addstr(s->ident);
			s = s->entry.var_decl.next_in;
			if (s) *bp++ = ',';
		}
		addstr(") ");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("result (");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(")");
		}
		break;
	case ENTRY_STAT	:
		addstr("entry ");
		addstr(pbf->entry.function.func_symb->ident);
		*bp++ = '(';
                unp_llnd(pbf->entry.Template.ll_ptr1);
		/*
		s = pbf->entry.function.func_symb->entry.proc_decl.in_list;
		while (s) {
			addstr(s->ident);
			s = s->entry.var_decl.next_in;
			if (s) *bp++ = ',';
		}
                */
		addstr(") ");
		break;
	   case INTERFACE_STMT:
	   {
		PTR_SYMB s;
		char *c;
		
		addstr("interface ");
		if ( (s = (pbf->entry.Template.symbol)) != 0) 
		{
		     c = s->ident;
		     if (*c == '.') 
		     {
			  addstr("operator (");
			  addstr(c);
			  addstr(")");
		     }
		     else if (*c == '=') 
		     {
			  addstr("assignment (");
			  addstr("=");
			  addstr(")");
		     }
		     else addstr(c);
		}
	   }
		break;
        case MODULE_STMT:
		addstr("module ");
		addstr(pbf->entry.Template.symbol->ident);
		break;
        case CASE_NODE:
		if (pbf->entry.Template.ll_ptr3)
		{
		     unp_llnd(pbf->entry.Template.ll_ptr3);
		     addstr(":");
		}
		addstr("select case (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(")");
		break;
	case SWITCH_NODE	:
	        addstr("case (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(")");
		if (pbf->entry.Template.symbol)
		   addstr(pbf->entry.Template.symbol->ident);
	        break;
	case IF_NODE	:
		/* if (pbf->entry.Template.ll_ptr3)
		{
		     unp_llnd(pbf->entry.Template.ll_ptr3);
		     addstr(":");
		} */
		gen_branch(IF_NODE, "if ", pbf);
		break;
	case LOGIF_NODE	:
		addstr("if (");
		unp_llnd(pbf->entry.if_node.condition);
		addstr(") ");
		break;
	case ELSEIF_NODE:
		gen_branch(IF_NODE, "else if", pbf);
		break;
	case ARITHIF_NODE:
		addstr("if (");
		unp_llnd(pbf->entry.if_node.condition);
		addstr(") ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		break;
	case WHERE_BLOCK_STMT:
		gen_branch(WHERE_BLOCK_STMT, "where ", pbf);
		break;
	case WHERE_NODE:
		addstr("where (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(") ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr(" = ");
		unp_llnd(pbf->entry.Template.ll_ptr3);
		break;
	case PARDO_NODE :
	        gen_loop_header("parallel do ", pbf);
                break;
	case PDO_NODE :
	        gen_loop_header("pdo ", pbf);
                break;
	case FOR_NODE	:
		if (pbf->entry.Template.ll_ptr3)
		{
		     unp_llnd(pbf->entry.Template.ll_ptr3);
		     addstr(":");
		}
		gen_loop_header("do ",pbf); 
		break;
	case CDOALL_NODE	:
		gen_loop_header("cdoall ",pbf);
		break;
	case WHILE_NODE	:
		if (pbf->entry.Template.ll_ptr3)
		{
		     unp_llnd(pbf->entry.Template.ll_ptr3);
		     addstr(":");
		}
		addstr("do ");
		if (pbf->entry.for_node.doend) {
			char	label[7];

			sprintf(label,"%d ",(int)(pbf->entry.for_node.doend->stateno));
			addstr(label);
		}
		addstr(" while (");
		unp_llnd(pbf->entry.while_node.condition);
		*bp++ = ')';
		break;
	case ASSIGN_STAT:
		unp_llnd(pbf->entry.assign.l_value);
		addstr(" = ");
		unp_llnd(pbf->entry.assign.r_value);
		break;
	case IDENTIFY:
		addstr("identify ");
		unp_llnd(pbf->entry.identify.l_value);
		*bp++ = ' ';
		unp_llnd(pbf->entry.identify.r_value);
		break;
	case PRIVATE_STMT:
		addstr("private ");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr(":: ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		}
		break;
	case PUBLIC_STMT:
		addstr("public ");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr(":: ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		}
		break;
	case STRUCT_DECL:
		{
		     PTR_LLND l;
		     addstr("type ");
		     
		     if ( (l = pbf->entry.Template.ll_ptr1) != 0) 
		     {
			  addstr(",");
			  unp_llnd(l);
			  addstr("::");
		     }
		     
		     addstr(pbf->entry.Template.symbol->ident);
		}
		break;
	case SEQUENCE_STMT:
		addstr("sequence ");
		break;
	case CONTAINS_STMT:
		addstr("contains ");
		break;
        case OVERLOADED_ASSIGN_STAT:
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr("=");
		unp_llnd(pbf->entry.Template.ll_ptr3);
		break;
	case OVERLOADED_PROC_STAT:
	case PROC_STAT	:
		addstr("call ");
		addstr(pbf->entry.Template.symbol->ident);
		*bp++ = '(';
		unp_llnd(pbf->entry.Template.ll_ptr1);
		*bp++ = ')';
		break;
	case STMTFN_STAT:
		{PTR_SYMB p;
		 PTR_LLND body;

		body = pbf->entry.Template.ll_ptr1;
		p = body->entry.Template.symbol;
		addstr(p->ident);
		*bp++ = '(';
		p=p->entry.func_decl.in_list;
		while (p) {
			addstr(p->ident);
			if( (p=p->entry.var_decl.next_in) != 0) *bp++ = ',';
		}
		addstr(") = ");
		unp_llnd(body->entry.Template.ll_ptr1);
		break;
		}
	case SAVE_DECL:
		addstr("save ");
		if (pbf->entry.Template.ll_ptr1)
			unp_llnd(pbf->entry.Template.ll_ptr1);
		else
			addstr("all");
		break;
	case CONT_STAT:
		addstr("continue");
		break;
	case FORMAT_STAT:
/*		addstr("format ("); */
		unp_llnd(pbf->entry.format.spec_string);
/*		*bp++ = ')'; */
		break;
	case GOTO_NODE:
		addstr("goto ");
		unp_llnd(pbf->entry.Template.ll_ptr3);
		break;
	case ASSGOTO_NODE:
		addstr("goto ");
		addstr(pbf->entry.Template.symbol->ident);
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case COMGOTO_NODE:
		addstr("goto (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(") ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		break;
	case STOP_STAT:
		addstr("stop");
		if (pbf->entry.Template.ll_ptr1) {
			addstr("'");
			unp_llnd(pbf->entry.Template.ll_ptr1);
			addstr("'");
		}
		break;
	case RETURN_STAT:
		addstr("return");
		break;
	case OPTIONAL_STMT:
		addstr("optional :: ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case VAR_DECL:
		{
		PTR_LLND p = pbf->entry.Template.ll_ptr1;
	     /*	PTR_TYPE q;

		q = p->entry.list.item->entry.Template.symbol->type;
		if (q->variant == T_ARRAY)
			q = q->entry.ar_decl.base_type;
		addstr(ftype_name[type_index(q->variant)]);
		*bp++ = ' '; */
		unp_llnd(pbf->entry.Template.ll_ptr2);
		if (pbf->entry.Template.ll_ptr3)
		{
		     addstr(",");
		     unp_llnd(pbf->entry.Template.ll_ptr3);
		     addstr("::");
		}
		else addstr(" ");
		unp_llnd(p);
		break;
		}
	case INTENT_STMT:
		{
		PTR_SYMB s;
		PTR_LLND p = pbf->entry.Template.ll_ptr1;

		addstr("intent ");
		s = p->entry.list.item->entry.Template.symbol;
		if (s->attr & IN_BIT)
		     addstr("(in) :: ");
		if (s->attr & OUT_BIT)
		     addstr("(out) :: ");
		if (s->attr & INOUT_BIT)
		     addstr("(inout) :: ");
		unp_llnd(p);
		break;
		}
	case PARAM_DECL:
		addstr("parameter (");
		in_param = 1;
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(")");
		in_param = 0;
		break;
        case DIM_STAT:
		addstr("dimension ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case ALLOCATABLE_STMT:
		addstr("allocatable :: ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case POINTER_STMT:
		addstr("pointer :: ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case TARGET_STMT:
		addstr("target :: ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case ALLOCATE_STMT:
		addstr("allocate (");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		if (pbf->entry.Template.ll_ptr2) 
		{
		     addstr(", stat = ");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
		}
		addstr(")");
		break;
        case DEALLOCATE_STMT:
		addstr("deallocate (");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		if (pbf->entry.Template.ll_ptr2) 
		{
		     addstr(", stat = ");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
		}
		addstr(")");
		break;
        case NULLIFY_STMT:
		addstr("nullify (");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(")");
		break;
        case MODULE_PROC_STMT:
		addstr("module procedure ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case POINTER_ASSIGN_STAT:
	        addstr(pbf->entry.Template.symbol->ident);
		addstr("=> ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case CYCLE_STMT:
		addstr("cycle ");
	        addstr(pbf->entry.Template.symbol->ident);
		break;
        case EXIT_STMT:
		addstr("exit ");
	        addstr(pbf->entry.Template.symbol->ident);
		break;
        case USE_STMT:
		addstr("use ");
	        addstr(pbf->entry.Template.symbol->ident);
		if (pbf->entry.Template.ll_ptr1) 
		{
		     addstr(", ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		}
		break;
	case EQUI_STAT:
		addstr("equivalence ");
	case DATA_DECL:
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case IMPL_DECL:
		addstr("implicit ");
		if (pbf->entry.Template.ll_ptr1 == NULL)
			addstr("none");
		else {
			in_impli = 1;
			unp_llnd(pbf->entry.Template.ll_ptr1);
			in_impli = 0;
		}
		break;
	case EXTERN_STAT:
		addstr("external ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case INTRIN_STAT:
		addstr("intrinsic ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case PARREGION_NODE:
		addstr("parallel ");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("( ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(") ");
		}
		break;
	case PARSECTIONS_NODE:
		addstr("parallel sections");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("( ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(") ");
		}
		break;
	case PSECTIONS_NODE:
		addstr("psections ");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("(");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(") ");
		}
		break;
	case SINGLEPROCESS_NODE:
		addstr("single process");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("( ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(") ");
		}
		break;
	case CRITSECTION_NODE:
		addstr("critical section");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("( ");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
		     addstr(") ");
		}
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
		     addstr(") ");
		}
		break;
        case GUARDS_NODE:
		addstr("guards ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr("(");
	        unp_llnd(pbf->entry.Template.ll_ptr2);
                addstr(")");
		break;
	case LOCK_NODE:
		addstr("lock (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case UNLOCK_NODE:
		addstr("unlock (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case POST_NODE:
		addstr("post (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case WAIT_NODE:
		addstr("wait (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case CLEAR_NODE:
		addstr("clear (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case POSTSEQ_NODE:
		addstr("post (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(", ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
                addstr(")");
		if (pbf->entry.Template.ll_ptr3)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr3);
                     addstr(")");
		}
		break;
        case WAITSEQ_NODE:
		addstr("wait (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(", ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
                addstr(")");
		if (pbf->entry.Template.ll_ptr3)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr3);
                     addstr(")");
		}
		break;
        case SETSEQ_NODE:
		addstr("set (");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		addstr(", ");
		unp_llnd(pbf->entry.Template.ll_ptr2);
                addstr(")");
		if (pbf->entry.Template.ll_ptr3)
		{
		     addstr("guards (");
		     unp_llnd(pbf->entry.Template.ll_ptr3);
                     addstr(")");
		}
		break;
        case SECTION_NODE:
		addstr("section");
		if (pbf->entry.Template.ll_ptr1)
		{
		     addstr("(");
		     unp_llnd(pbf->entry.Template.ll_ptr1);
                     addstr(")");
		}
		if (pbf->entry.Template.ll_ptr2)
		{
		     addstr("wait (");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
                     addstr(")");
		}
		break;
        case ASSIGN_NODE:
		addstr("assign ( ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		break;
        case RELEASE_NODE:
		addstr("release ( ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
                addstr(")");
		break;
        case PRIVATE_NODE:
		addstr("private ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case READ_STAT:
	   {
		PTR_LLND p;
		PTR_LLND q;
		
		addstr("read ");
		p = pbf->entry.Template.ll_ptr2;
		q = p->entry.Template.ll_ptr1;

		if ((p->variant == EXPR_LIST)  ||
		    ((p->variant == SPEC_PAIR) && 
		     (strcmp(q->entry.string_val,"fmt") != 0)))
		{
		     addstr("(");
		     unp_llnd(pbf->entry.Template.ll_ptr2);
		     addstr(") ");
		}
		else 
		{
		     unp_llnd(pbf->entry.Template.ll_ptr2->entry.Template.ll_ptr2);
		     if (pbf->entry.Template.ll_ptr1 != LLNULL)
			  addstr(",");
		}
		unp_llnd(pbf->entry.Template.ll_ptr1);
	   }
		break;
	case WRITE_STAT:
		addstr("write ");
		addstr("(");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr(") ");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case PRINT_STAT:
		addstr("print ");
		unp_llnd(pbf->entry.Template.ll_ptr2->entry.Template.ll_ptr2);
		if (pbf->entry.Template.ll_ptr1 != LLNULL)
			  addstr(",");
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case OPEN_STAT:
		addstr("open ");
		addstr("(");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr(") ");
		break;
	case CLOSE_STAT:
		addstr("close ");
		addstr("(");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr(") ");
		break;
	case INQUIRE_STAT:
		addstr("inquire ");
		addstr("(");
		unp_llnd(pbf->entry.Template.ll_ptr2);
		addstr(") ");
		break;
        case SKIPPASTEOF_NODE:
	   {
		PTR_LLND p;
		PTR_LLND q;
		
		addstr("skip past eof ");
		p = pbf->entry.Template.ll_ptr2;
		q = p->entry.Template.ll_ptr2;

		if (p->variant == EXPR_LIST)
		{
		     addstr("(");
		     unp_llnd(p);
		     addstr(") ");
		}
		else unp_llnd(q);
	   }
		break;
	case BACKSPACE_STAT:
	   {
		PTR_LLND p;
		PTR_LLND q;
		
		addstr("backspace ");
		p = pbf->entry.Template.ll_ptr2;
		q = p->entry.Template.ll_ptr2;

		if (p->variant == EXPR_LIST)
		{
		     addstr("(");
		     unp_llnd(p);
		     addstr(") ");
		}
		else unp_llnd(q);
	   }
		break;
	case ENDFILE_STAT:
	   {
		PTR_LLND p;
		PTR_LLND q;
		
		addstr("endfile ");
		p = pbf->entry.Template.ll_ptr2;
		q = p->entry.Template.ll_ptr2;

		if (p->variant == EXPR_LIST)
		{
		     addstr("(");
		     unp_llnd(p);
		     addstr(") ");
		}
		else unp_llnd(q);
	   }
		break;
	case REWIND_STAT:
	   {
		PTR_LLND p;
		PTR_LLND q;
		
		addstr("rewind ");
		p = pbf->entry.Template.ll_ptr2;
		q = p->entry.Template.ll_ptr2;

		if (p->variant == EXPR_LIST)
		{
		     addstr("(");
		     unp_llnd(p);
		     addstr(") ");
		}
		else unp_llnd(q);
	   }
		break;
	case OTHERIO_STAT:
		unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case COMM_STAT:
		addstr("common ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
        case NAMELIST_STAT:
		addstr("namelist ");
	        unp_llnd(pbf->entry.Template.ll_ptr1);
		break;
	case CONTROL_END:
		break;
	default:
		break;		/* don't know what to do at this point */
	}

	if (pbf->variant != CONTROL_END) {
		if (print_comments && cmnt && cmnt->type != FULL)
			addstr(cmnt->string);
		if (pbf->variant != LOGIF_NODE)
			*bp++ = '\n';
	   }
}

/****************************************************************
 *								*
 *  funp_blck -- unparse the given bif node to source string	*
 *		 along with its control children (block)	*
 *								*
 *  Input:							*
 *	   bif - bif node to be unparsed			*
 *	   tab - number of tabs (2 spaces) for indenting	*
 *	   bp (implicitely) - where the output string to be	*
 *		   placed					*
 *								*
 *  Output:							*
 *	   the unparse string where "bp" was pointed to		*
 *								*
 *  Side Effect:						*
 *	   "bp" will be updated to the next character behind	*
 *	   the end of the unparsed string (by "addstr")		*
 *								*
 ****************************************************************/
static void
funp_blck(bif, tab)
	PTR_BFND bif;
	int	 tab;
{
    PTR_BLOB b;

    if (print_comments && (cmnt = bif->entry.Template.cmnt_ptr) != NULL)
	while (cmnt != NULL && cmnt->type == FULL) {
	    addstr(cmnt->string);
	    *bp++ = '\n';
	    cmnt = cmnt->next;
	}

    funp_bfnd(tab, bif);

    if (bif->variant != CDOALL_NODE && bif->variant != SDOALL_NODE) {
	for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
	    if (b->ref->variant != CONTROL_END)
		funp_blck(b->ref, tab+1);
	    else {
		PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;

		if (print_comments && cmnt)
		    while (cmnt != NULL && cmnt->type == FULL) {
			addstr(cmnt->string);
			*bp++ = '\n';
			cmnt = cmnt->next;
		    }
		switch(bif->variant) {
		case FOR_NODE:
		case PARDO_NODE:
		case PDO_NODE:
		case WHILE_NODE:
		    if (!bif->entry.Template.lbl_ptr) {
			put_tabs(tab-1);
			if (bif->variant == PARDO_NODE)
            		   addstr("        end parallel do");
			else if (bif->variant == PDO_NODE)
			        addstr("        end pdo");
			else addstr("        end do");
		    }
		    break;
		case IF_NODE: 
	        case ELSEIF_NODE:
		    put_tabs(tab-1);
		    if (bif->entry.Template.bl_ptr2)
			addstr("        else");
		    else
			addstr("        end if");
		    break;
	        case WHERE_BLOCK_STMT:
		    put_tabs(tab);
		    if (bif->entry.Template.bl_ptr2)
			addstr("        elsewhere");
		    else
			addstr("        end where");
		    break;
	        case CASE_NODE:
		    put_tabs(tab-1);
		    addstr("        end select ");
		    if (bif->entry.Template.symbol)
			 addstr(bif->entry.Template.symbol->ident);
		    break;
	        case SWITCH_NODE:
		    put_tabs(tab-1);
		    break;
		case PROG_HEDR:
		case PROC_HEDR:
		case FUNC_HEDR:
 	        case BLOCK_DATA:
		    addstr("        end");
		    break;
		case MODULE_STMT:
		    addstr("        end module ");
		    addstr(bif->entry.Template.symbol->ident);
		    break;
		case INTERFACE_STMT:
		    put_tabs(tab-1);
		    addstr("        end interface");
		    break;
		case STRUCT_DECL:
		    put_tabs(tab-1);
		    addstr("        end type ");
		    addstr(bif->entry.Template.symbol->ident);
		    break;
	        case PARREGION_NODE:
		    put_tabs(tab-1);
		    addstr("        end parallel");
		    break;
	        case PARSECTIONS_NODE:
		    put_tabs(tab-1);
		    addstr("        end parallel sections");
		    break;
	        case PSECTIONS_NODE:
		    put_tabs(tab-1);
		    addstr("        end psections");
		    break;
	        case SINGLEPROCESS_NODE:
		    put_tabs(tab-1);
		    addstr("        end single process");
		    break;
	        case CRITSECTION_NODE:
		    put_tabs(tab-1);
		    addstr("        end critical section");
		    if (bif->entry.Template.ll_ptr1)
		    {
			 addstr("(");
			 unp_llnd(bif->entry.Template.ll_ptr1);
			 addstr(")");
		    }
		    break;
                /* case SECTION_NODE: */
		default:
		    break; 
		}
		if (print_comments && cmnt && cmnt->type != FULL)
		    addstr(cmnt->string);
		*bp++ = '\n';
	    }

	for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
	    if (b->ref->variant != CONTROL_END)
		funp_blck(b->ref, tab+1);
	    else {
		PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;

		if (print_comments && cmnt)
		    while (cmnt != NULL && cmnt->type == FULL) {
			addstr(cmnt->string);
			*bp++ = '\n';
			cmnt = cmnt->next;
		    }
		put_tabs(tab); 
		if (bif->variant == PDO_NODE) 
		     addstr("       end extended");
		if (bif->variant == PSECTIONS_NODE)
		     addstr("      end extended");
		if (bif->variant == WHERE_BLOCK_STMT)
		     addstr("      end where");
		if ((bif->variant == IF_NODE) || (bif->variant == ELSEIF_NODE))
		     addstr("      end if");
		if (print_comments && cmnt && cmnt->type != FULL)
		    addstr(cmnt->string);
		*bp++ = '\n';
	    }
    } else {
	for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
	    if (b->ref->variant != CONTROL_END)
		funp_blck(b->ref, tab+1);
	    else {
		PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;

		if (print_comments && cmnt)
		    while (cmnt != NULL && cmnt->type == FULL) {
			addstr(cmnt->string);
			*bp++ = '\n';
			cmnt = cmnt->next;
		    }
		if (!bif->entry.Template.lbl_ptr) {
		    put_tabs(tab-1);
		    addstr("        loop");
		}
		if (print_comments && cmnt && cmnt->type != FULL)
		    addstr(cmnt->string);
		*bp++ = '\n';
	    }

	for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
	    if (b->ref->variant != CONTROL_END)
		funp_blck(b->ref, tab+1);
	    else {
		PTR_CMNT cmnt = b->ref->entry.Template.cmnt_ptr;

		if (print_comments && cmnt)
		    while (cmnt != NULL && cmnt->type == FULL) {
			addstr(cmnt->string);
			*bp++ = '\n';
			cmnt = cmnt->next;
		    }
		put_tabs(tab); 
		if (bif->variant == CDOALL_NODE)
		    addstr("      end cdoall");
		else
		    addstr("      end sdoall");
		if (print_comments && cmnt && cmnt->type != FULL)
		    addstr(cmnt->string);
		*bp++ = '\n';
	    }
    }
}


/****************************************************************
 *								*
 *  funparse_type -- unparse the type node for Fortran		*
 *								*
 *  input:							*
 *	   type -- the node to be unparsed			*
 *								*
 *  output:							*
 *	   the unparsed string					* 
 *								*
 ****************************************************************/
char *
funparse_type(type)
	PTR_TYPE type;
{
	char *b1;

	if (type == NULL)
		return NULL;

	bp = buffer;
	switch (type->variant) {
	case T_INT   :
	case T_FLOAT :
	case T_DOUBLE:
	case T_CHAR  :
	case T_BOOL  :
	case T_STRING:
		addstr(ftype_name[type_index(type->variant)]);
                if ((type->entry.Template.ranges) != LLNULL)
		     unp_llnd(type->entry.Template.ranges);
		break;
	case T_ARRAY:
		addstr(ftype_name[type_index(type->entry.ar_decl.base_type->variant)]);
		*bp++ = ' ';
		unp_llnd(type->entry.ar_decl.ranges);
		break;
	default:
		return NULL;
	}
	*bp++ = '\n';
	*bp++ = '\0';
	b1 = malloc(strlen(buffer) + 1);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,b1, 0);
#endif
	(void) strcpy(b1, buffer);
	bp = buffer;
	*bp = '\0';
	return b1;
}


/****************************************************************
 *								*
 *  funparse_symb -- unparse the symbol node for Fortran	*
 *								*
 *  input:							*
 *	   symb -- the node to be unparsed			*
 *								*
 *  output:							*
 *	   the unparsed string					* 
 *								*
 ****************************************************************/
char *
funparse_symb(symb)
	PTR_SYMB symb;
{
	int i;
	char buf[100], *b1, *b2;
	PTR_TYPE t;

	b1 = buf;
	for (i = 1; i<10; i++)
		*b1++ = ' ';
	t = symb->type;
	i = t->variant < T_ARRAY? t->variant: t->entry.ar_decl.base_type->variant;
	b2 = ftype_name[type_index(i)];
	while ( (*b1 = *b2++) != 0)
		b1++;
	*b1++ = ' ';
	if (t->variant < T_ARRAY) {
		b2 = symb->ident;
		while ( (*b1 = *b2++) != 0)
			b1++;
	} else {
		bp = buffer;
		unp_llnd(t->entry.ar_decl.ranges);
		b2 = buffer;
		while ( (*b1 = *b2++) != 0)
			b1++;
	}		
	*b1++ = '\n';
	*b1++ = '\0';
	b2 = malloc(strlen(buf) + 1);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,b2, 0);
#endif
	(void) strcpy(b2, buf);
	*buffer = '\0';
	return b2;
}


/****************************************************************
 *								*
 *  funparse_llnd -- unparse the low level node for Fortran	*
 *								*
 *  input:							*
 *	   llnd -- the node to be unparsed			*
 *								*
 *  output:							*
 *	   the unparsed string					* 
 *								*
 ****************************************************************/
char *
funparse_llnd(llnd)
	PTR_LLND llnd;
{
	int len;
	char *p;

	bp = buffer;	/* reset the buffer pointer */
	unp_llnd(llnd);
	*bp++ = '\n';
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer); 	/* copy the buffer for output */
	*buffer = '\0';
	return p;
}


/****************************************************************
 *								*
 *  funparse_bfnd -- unparse the bif node for Fortran		*
 *								*
 *  input:							*
 *	   bif -- the node to be unparsed			*
 *								*
 *  output:							*
 *	   the unparsed string					* 
 *								*
 ****************************************************************/
char *
funparse_bfnd(bif)
	PTR_BFND bif;
{
	int len;
	char *p;

	first = 1;		/* Mark this is the first bif node */
	bp = buffer;		/* reset the buffer pointer */
	funp_bfnd(0, bif);
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer); 	/* copy the buffer for output */
	*buffer = '\0';
	return (p);
}


/****************************************************************
 *								*
 *  funparse_bfnd_w_tab -- unparse the bif node for Fortran	*
 *								*
 *  input:							*
 *	   bif -- the node to be unparsed			*
 *								*
 *  output:							*
 *	   the unparsed string					* 
 *								*
 ****************************************************************/
char *
funparse_bfnd_w_tab(tab, bif)
	int	 tab;
	PTR_BFND bif;
{
	int len;
	char *p;

	first = 1;		/* Mark this is the first bif node */
	bp = buffer;		/* reset the buffer pointer */
	funp_bfnd(tab, bif);
	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer); 	/* copy the buffer for output */
	*buffer = '\0';
	return (p);
}


char *
funparse_blck(bif)
	PTR_BFND bif;
{
	int len;
	char *p;

	bp = buffer;		/* reset the buffer pointer */
	funp_blck(bif, figure_tabs(bif));

	*bp++ = '\0';
	len = (bp - buffer) + 1; /* calculate the string length */
	p = malloc(len);	/* allocate space for returned value */
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	strcpy(p, buffer); 	/* copy the buffer for output */
	*buffer = '\0';
	return (p);
}
