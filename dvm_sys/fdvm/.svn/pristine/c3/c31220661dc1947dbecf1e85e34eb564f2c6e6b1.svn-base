/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: mod_ref.c */

/* Modified by Jenq-Kuen Lee Feb 24,1988          */
/* The simple un-parser for VPC++                 */
# include "db.h"
# include "vparse.h"

#define  BLOB1_NULL (PTR_BLOB1)NULL
#define  R_VALUE     0
#define  L_VALUE     1

extern PCF UnparseBfnd[];
extern PTR_BLOB1 chain_blob1();
extern PTR_BLOB1 make_blob1();
extern char *cunparse_llnd();
extern PTR_FILE cur_file;

static void ccheck_bfnd();
static void ccheck_llnd();
void print_out();
void test_mod_ref();
int is_i_code();

static void ccheck_bfnd(pbf, ref_list, mod_list)
PTR_BFND pbf;
PTR_BLOB1 *ref_list, *mod_list;
{
    PTR_BLOB1 list_r, list_m;

    *ref_list = BLOB1_NULL;
    *mod_list = BLOB1_NULL;
    if (!pbf)
	return;

    switch (pbf->variant) {
    case GLOBAL:
	break;
    case PROG_HEDR:
    case PROC_HEDR:
	break;
    case FUNC_HEDR:
	break;
    case IF_NODE:
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case LOGIF_NODE:
    case ARITHIF_NODE:
    case WHERE_NODE:
	break;
    case FOR_NODE:
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pbf->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	ccheck_llnd(pbf->entry.Template.ll_ptr3, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case FORALL_NODE:
    case WHILE_NODE:
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case ASSIGN_STAT:
    case IDENTIFY:
    case PROC_STAT:
    case SAVE_DECL:
    case CONT_STAT:
    case FORMAT_STAT:
	break;
    case LABEL_STAT:
	break;
    case GOTO_NODE:
	break;
    case ASSGOTO_NODE:
    case COMGOTO_NODE:
    case STOP_STAT:
	break;
    case RETURN_STAT:
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case PARAM_DECL:
    case DIM_STAT:
    case EQUI_STAT:
    case DATA_DECL:
    case READ_STAT:
    case WRITE_STAT:
    case OTHERIO_STAT:
    case COMM_STAT:
    case CONTROL_END:
	break;
    case CLASS_DECL:		/* New added for VPC */
	break;
    case ENUM_DECL:		/* New added for VPC */
    case UNION_DECL:		/* New added for VPC */
    case STRUCT_DECL:		/* New added for VPC */
	break;
    case DERIVED_CLASS_DECL:	/* Need More for VPC */
    case VAR_DECL:
	break;
    case EXPR_STMT_NODE:	/* New added for VPC */
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case DO_WHILE_NODE:		/* New added for VPC */
	/* Need study        */
	break;
    case SWITCH_NODE:		/* New added for VPC */
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case CASE_NODE:		/* New added for VPC */
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case DEFAULT_NODE:		/* New added for VPC */
	break;
    case BASIC_BLOCK:
	break;
    case BREAK_NODE:		/* New added for VPC */
	break;
    case CONTINUE_NODE:		/* New added for VPC */
	break;
    case RETURN_NODE:		/* New added for VPC */
	ccheck_llnd(pbf->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case ASM_NODE:		/* New added for VPC */
	break;			/* Need More         */
    case SPAWN_NODE:		/* New added for CC++ */
	break;
    case PARFOR_NODE:		/* New added for CC++ */
	ccheck_llnd(pbf->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case PAR_NODE:		/* New added for CC++ */
	break;
    default:
	fprintf(stderr, "bad bfnd case\n");
	break;			/* don't know what to do at this point */
    }
}


static void ccheck_llnd(pllnd, ref_list, mod_list, type)
PTR_LLND pllnd;
PTR_BLOB1 *ref_list, *mod_list;
int type;
{
    PTR_BLOB1 list_r, list_m;

    *ref_list = (PTR_BLOB1) NULL;
    *mod_list = (PTR_BLOB1) NULL;
    if (pllnd == NULL)
	return;

    switch (pllnd->variant) {
    case INT_VAL:
    case STMT_STR:
    case FLOAT_VAL:
    case DOUBLE_VAL:
    case STRING_VAL:
    case BOOL_VAL:
    case CHAR_VAL:
	break;
    case CONST_REF:
    case ENUM_REF:
	break;
    case VAR_REF:
	if (type == L_VALUE) {
	    *ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	    *mod_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	}
	else {
	    *ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	    *mod_list = (PTR_BLOB1) NULL;
	}
	break;
    case POINTST_OP:		/* New added for VPC */
    case RECORD_REF:		/* Need More */
	if (type == L_VALUE) {
	    *ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	    *mod_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	}
	else {
	    *ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	    *mod_list = (PTR_BLOB1) NULL;
	}
	/* Need more */
	break;
    case ARRAY_OP:
	*ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	if (type == L_VALUE)
	    *mod_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	else
	    *mod_list = BLOB1_NULL;
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case ARRAY_REF:
	*ref_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	if (type == L_VALUE)
	    *mod_list = make_blob1(IsObj, pllnd, (PTR_BLOB1) NULL);
	else
	    *mod_list = BLOB1_NULL;
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case CONSTRUCTOR_REF:
	break;
    case ACCESS_REF:
	break;
    case CONS:
	break;
    case ACCESS:
	break;
    case IOACCESS:
	break;
    case PROC_CALL:
    case FUNC_CALL:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case EXPR_LIST:
	if (type == R_VALUE) {
	    ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	    *ref_list = list_r;
	    *mod_list = list_m;
	    ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	    *ref_list = chain_blob1(*ref_list, list_r);
	    *mod_list = chain_blob1(*mod_list, list_m);
	}
	else {
	    if (pllnd->entry.Template.ll_ptr2) {
		ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
		*ref_list = list_r;
		*mod_list = list_m;
		ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, L_VALUE);
		*ref_list = chain_blob1(*ref_list, list_r);
		*mod_list = chain_blob1(*mod_list, list_m);
	    }
	    else {
		ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, L_VALUE);
		*ref_list = list_r;
		*mod_list = list_m;
	    }
	}
	break;
    case EQUI_LIST:
	break;
    case COMM_LIST:
	break;
    case VAR_LIST:
    case CONTROL_LIST:
	break;
    case RANGE_LIST:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case DDOT:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case COPY_NODE:
	break;
    case VECTOR_CONST:		/* NEW ADDED FOR VPC++  */
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case INIT_LIST:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case BIT_NUMBER:
	break;
    case DEF_CHOICE:
    case SEQ:
	break;
    case SPEC_PAIR:
	break;
    case MOD_OP:
	break;

    case ASSGN_OP:		/* New added for VPC */
    case ARITH_ASSGN_OP:	/* New added for VPC */
    case PLUS_ASSGN_OP:
    case MINUS_ASSGN_OP:
    case AND_ASSGN_OP:
    case IOR_ASSGN_OP:
    case MULT_ASSGN_OP:
    case DIV_ASSGN_OP:
    case MOD_ASSGN_OP:
    case XOR_ASSGN_OP:
    case LSHIFT_ASSGN_OP:
    case RSHIFT_ASSGN_OP:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, L_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case EQ_OP:
    case LT_OP:
    case GT_OP:
    case NOTEQL_OP:
    case LTEQL_OP:
    case GTEQL_OP:
    case ADD_OP:
    case SUBT_OP:
    case OR_OP:
    case MULT_OP:
    case DIV_OP:
    case AND_OP:
    case EXP_OP:
    case LE_OP:			/* New added for VPC *//* Duplicated */
    case GE_OP:			/* New added for VPC *//* Duplicated */
    case NE_OP:			/* New added for VPC *//* Duplicated */
    case BITAND_OP:		/* New added for VPC */
    case BITOR_OP:		/* New added for VPC */
    case LSHIFT_OP:		/* New added for VPC */
    case RSHIFT_OP:		/* New added for VPC */
    case NEW_OP:
    case DELETE_OP:
    case THIS_NODE:
    case SCOPE_OP:
    case INTEGER_DIV_OP:	/* New added for VPC */
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case ADDRESS_OP:		/* New added for VPC */
    case SIZE_OP:		/* New added for VPC */
	break;
    case DEREF_OP:
	break;
    case SUB_OP:		/* duplicated unary minus  */
    case MINUS_OP:		/* unary operations */
    case UNARY_ADD_OP:		/* New added for VPC */
    case BIT_COMPLEMENT_OP:	/* New added for VPC */
    case NOT_OP:
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	break;
    case MINUSMINUS_OP:		/* New added for VPC */
    case PLUSPLUS_OP:		/* New added for VPC */
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, L_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, L_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case STAR_RANGE:
	break;
    case CLASSINIT_OP:		/* New added for VPC */
	break;
    case CAST_OP:		/* New added for VPC */
	break;
    case FUNCTION_OP:
    case EXPR_IF:		/* New added for VPC */
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case EXPR_IF_BODY:		/* New added for VPC */
	ccheck_llnd(pllnd->entry.Template.ll_ptr1, &list_r, &list_m, R_VALUE);
	*ref_list = list_r;
	*mod_list = list_m;
	ccheck_llnd(pllnd->entry.Template.ll_ptr2, &list_r, &list_m, R_VALUE);
	*ref_list = chain_blob1(*ref_list, list_r);
	*mod_list = chain_blob1(*mod_list, list_m);
	break;
    case FUNCTION_REF:		/* New added for VPC */
	break;
    case LABEL_REF:		/* Fortran Version, For VPC we need more */
	break;

    default:
	fprintf(stderr, "ccheck_llnd -- bad llnd ptr %d!\n", pllnd->variant);
	break;
    }
}


/*  Very important routine to see a given bif node of a function is
 *  local-variable declaration or argument declaration
 *   return   1 ---TRUE
 *            0    False
 */
int is_param_decl_interface(var_bf, functor)
PTR_BFND var_bf;
PTR_SYMB functor;
{
    PTR_LLND flow_ptr, lpr;
    PTR_SYMB s;

    switch (var_bf->variant) {
    case VAR_DECL:
    case ENUM_DECL:
    case CLASS_DECL:
    case UNION_DECL:
    case STRUCT_DECL:
    case DERIVED_CLASS_DECL:
	lpr = var_bf->entry.Template.ll_ptr1;
	for (flow_ptr = lpr; flow_ptr; flow_ptr=flow_ptr->entry.Template.ll_ptr1) {
	    if ((flow_ptr->variant == VAR_REF) ||
		(flow_ptr->variant == ARRAY_REF) ||
		(flow_ptr->variant == FUNCTION_REF))
		break;
	}
	if (!flow_ptr) {
	   return 0;
	}

	for (s = functor->entry.member_func.in_list; s;) {
	    if (flow_ptr->entry.Template.symbol == s)
		return (1);
	    s = s->entry.var_decl.next_in;
	}
	return (0);

    default:
	return (0);
    }

}


PTR_BLOB1 chain_blob1(b1, b2)
PTR_BLOB1 b1, b2;
{
    PTR_BLOB1 oldptr, temptr;

    if (!b1)
	return (b2);
    if (!b2)
	return (b1);
    for (oldptr = temptr = b1; temptr; temptr = temptr->next)
	oldptr = temptr;

    oldptr->next = b2;
    return (b1);
}


/* -------------------------------------------------------------------*/
/* The following code for testing ccheck_bfnd and ccheck_llnd         */
void print_out(list, type)
PTR_BLOB1 list;
int type;
{
    PTR_BLOB1 b;
    char *source_ptr;

    if (!list)
	return;
    if (type == R_VALUE)
	fprintf(stderr, "------ reference ---------------------------------------------\n");
    else
	fprintf(stderr, "------ modified ---------------------------------------------\n");
    for (b = list; b; b = b->next) {
	source_ptr = (UnparseBfnd[cur_file->lang])(b->ref);
	fprintf(stderr, "%s\n", source_ptr);
    }

}

void test_mod_ref(pbf)
PTR_BFND pbf;
{
    PTR_BLOB b;
    PTR_BLOB1 list_r, list_m;

    if (!pbf)
	return;
    ccheck_bfnd(pbf, &list_r, &list_m);

    if (is_i_code(pbf)) {
	for (b = pbf->entry.Template.bl_ptr1; b; b = b->next)
	    test_mod_ref(b->ref);
	for (b = pbf->entry.Template.bl_ptr2; b; b = b->next)
	    test_mod_ref(b->ref);
    }

}

int is_i_code(pbf)
PTR_BFND pbf;
{
    switch (pbf->variant) {
    case ENUM_DECL:
    case STRUCT_DECL:
    case UNION_DECL:
	return (0);
    default:
	return (1);
    }
}
