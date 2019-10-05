/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/****************************************************************
 *								*
 *   dbutils -- contains those utilities that will be used by	*
 *		the data base management routines		*
 *								*
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
# include "db.h"

/*
 * global references
 */
extern int language;
extern PTR_FILE cur_file;

int   read_nodes();

/*
 * Local variables
 */
static PTR_SYMB head_symb;
static char *proj_filename;
static int   temp[200];
static int  *pt;

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
extern void removeFromCollection(void *pointer);
#endif

/****************************************************************
 *								*
 *  alloc_blob -- allocate new space for structure blob		*
 *								*
 *  output:							*
 *	Non-NULL - pointer to the newly allocated structure	*
 *	NULL - something was wrong				*
 *								*
 ****************************************************************/
PTR_BLOB
alloc_blob()
{
    void *p = calloc(1, sizeof(struct blob));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	return ((PTR_BLOB)p);
}


/****************************************************************
 *								*
 *  alloc_blob1 -- allocate new space for structure blob1	*
 *								*
 *  output:							*
 *	Non-NULL - pointer to the newly allocated structure	*
 *	NULL - something was wrong				*
 *								*
 ****************************************************************/
static PTR_BLOB1
alloc_blob1()
{
    void *p = calloc(1, sizeof(struct blob1));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	return ((PTR_BLOB1) p);
}

	
/****************************************************************
 *								*
 *  alloc_info -- allocate new space for structure obj_info	*
 *								*
 *  output:							*
 *	Non-NULL - pointer to the newly allocated structure	*
 *	NULL - something was wrong				*
 *								*
 ****************************************************************/
static PTR_INFO
alloc_info()
{
    void *p = calloc(1, sizeof(struct obj_info));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	return ((PTR_INFO) p);
}


/****************************************************************
 *								*
 *   check_ref -- check if the variable whose id is "id" has	*
 *		  referenced in this statement or not		*
 *   input:							*
 *	    id -- the id of the variable to be checked		*
 *								*
 *   output:							*
 *	    1, if it's been refereneced				*
 *	    0, if not and add it to the table			*
 *								*
 ****************************************************************/
int
check_ref(id)
	int  id;
{
	int	*p;

	for(p = temp; p < pt;)
		if(*p++ == id)
			return(1);
	*pt++ = id;
	return(0);
}


/****************************************************************
 *								*
 *   build_ref -- add "bif" to the reference chain of "symb"	*
 *								*
 *   input:							*
 *	    symb - the symb where the reference to be added	*
 *	    bif  - the statement that references symb		*
 *								*
 ****************************************************************/
void
build_ref(symb, bif)
	PTR_SYMB symb;
	PTR_BFND bif;
{
	register PTR_BLOB   b, b1, b2;

	b = alloc_blob();
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,b, 0);
#endif
	b->ref = bif;
	if (symb->ud_chain == NULL)
		symb->ud_chain = b;
	else {
		for (b1 = b2 = symb->ud_chain; b1; b1 = b1->next)
			b2 = b1;
		b2->next = b;
	}
	b->next = NULL;
}


/****************************************************************
 *								*
 *  make_blob1 -- make a new blob1 node				*
 *								*
 *  input:							*
 *	   tag  - type of this blob1 node			*
 *	   ref  - pointer to the object it references		*
 *	   next - link to the next blob1 node			*
 *								*
 ****************************************************************/
PTR_BLOB1
make_blob1(tag, ref, next)
	int	  tag;
	PTR_BFND  ref;
	PTR_BLOB1 next;
{
	PTR_BLOB1 new;

	new = alloc_blob1();
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,new, 0);
#endif
	new->tag = tag;
	new->ref = (char *) ref;
	new->next = next;
	return (new);
}


/****************************************************************
 *								*
 *  make_obj_info -- make a new obj_info node			*
 *								*
 *  input:							*
 *	   filename - name of the file where this obj_info	*
 *		      resides					*
 *	   g_line   - ablosute line no. of the obj in the file	*
 *	   l_line   - line no. of the object relative to its	*
 *		      parent objec				*
 *	   source   - the objec in the source form		*
 *								*
 ****************************************************************/
PTR_INFO
make_obj_info(filename, g_line, l_line, source)
	char	*filename;
	int	 g_line;
	int	 l_line;
	char	*source;
{
	register PTR_INFO new;

	new = alloc_info();
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,new, 0);
#endif
	new->filename = filename;
	new->g_line = g_line;
	new->l_line = l_line;
	new->source = source;
	return (new);
}

/****************************************************************
 *								*
 *   visit_llnd -- recursively visit the low level nodes and	*
 *		   find those use and def info it references	*
 *								*
 *   input:							*
 *	    bif  - the bif node to which the llnd belongs	*
 *	    llnd - the low level node to be visit		*
 *								*
 ****************************************************************/
void
visit_llnd(bif, llnd)
	PTR_BFND	bif;
	PTR_LLND	llnd;
{
	if (llnd == NULL) return;

	switch (llnd->variant) {
	case LABEL_REF:
		{
		}
		break;
	case CONST_REF	:
	case VAR_REF	:
	case ARRAY_REF	:
		if(check_ref(llnd->entry.Template.symbol->id) == 0)
			build_ref(llnd->entry.Template.symbol, bif);
		break;
	case CONSTRUCTOR_REF	:
		break;
	case ACCESS_REF	:
		break;
	case CONS	:
		break;
	case ACCESS	:
		break;
	case IOACCESS	:
		break;
	case PROC_CALL	:
	case FUNC_CALL	:
		visit_llnd(bif, llnd->entry.proc.param_list);
		break;
	case EXPR_LIST	:
		visit_llnd(bif, llnd->entry.list.item);
		if (llnd->entry.list.next)
			visit_llnd(bif, llnd->entry.list.next);
		break;
	case EQUI_LIST	:
		visit_llnd(bif, llnd->entry.list.item);
		if (llnd->entry.list.next) {
			visit_llnd(bif, llnd->entry.list.next);
		}
		break;
	case COMM_LIST	:
		if (llnd->entry.Template.symbol) {
/*			addstr(llnd->entry.Template.symbol->ident);
 */		}
		visit_llnd(bif, llnd->entry.list.item);
		if (llnd->entry.list.next)
			visit_llnd(bif, llnd->entry.list.next);
		break;
	case VAR_LIST	:
	case RANGE_LIST	:
	case CONTROL_LIST	:
		visit_llnd(bif, llnd->entry.list.item);
		if (llnd->entry.list.next)
			visit_llnd(bif, llnd->entry.list.next);
		break;
	case DDOT	:
		visit_llnd(bif, llnd->entry.binary_op.l_operand);
		if (llnd->entry.binary_op.r_operand)
			visit_llnd(bif, llnd->entry.binary_op.r_operand);
		break;
	case DEF_CHOICE	:
	case SEQ	:
		visit_llnd(bif, llnd->entry.seq.ddot);
		if (llnd->entry.seq.stride)
			visit_llnd(bif, llnd->entry.seq.stride);
		break;
	case SPEC_PAIR	:
		visit_llnd(bif, llnd->entry.spec_pair.sp_label);
		visit_llnd(bif, llnd->entry.spec_pair.sp_value);
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
		visit_llnd(bif, llnd->entry.binary_op.l_operand);
		visit_llnd(bif, llnd->entry.binary_op.r_operand);
		break;
	case MINUS_OP	:
	case NOT_OP	:
		visit_llnd(bif, llnd->entry.unary_op.operand);
		break;
	case STAR_RANGE	:
		break;
	default	:
		break;
	}
}
       

/****************************************************************
 *								*
 *   visit_bfnd -- visits the subtree "bif" and generates the	*
 *		     use-definition info of the variables it	*
 *		     references					*
 *   input:							*
 *	     bif - the root of the tree to be visitd		*
 *								*
 *   side effect:						*
 *	     build the ud_chain at where the static variable	*
 *	     "head_symb" points to				*
 *								*
 ****************************************************************/
void
visit_bfnd(bif)
	PTR_BFND bif;
{
	register PTR_BLOB b;

	if(bif == NULL)
		return;
	pt = temp;		/* reset the pointer */

	switch(bif->variant) {
	    case GLOBAL:
	    case PROG_HEDR:
	    case PROC_HEDR:
	    case FUNC_HEDR:
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case FOR_NODE:
		build_ref(bif->entry.Template.symbol, bif); /* control var */
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check range */
		visit_llnd(bif, bif->entry.Template.ll_ptr2); /* check incr */
		visit_llnd(bif, bif->entry.Template.ll_ptr3); /* where cond */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case CDOALL_NODE:
		build_ref(bif->entry.Template.symbol, bif); /* control var */
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check range */
		visit_llnd(bif, bif->entry.Template.ll_ptr2); /* check incr */
		visit_llnd(bif, bif->entry.Template.ll_ptr3); /* where cond */
		for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case WHILE_NODE:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case WHERE_NODE:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			visit_bfnd(b->ref);
		for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case IF_NODE:
	    case ELSEIF_NODE:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			visit_bfnd(b->ref);
		for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
			visit_bfnd(b->ref);
		break;
	    case LOGIF_NODE:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		visit_bfnd(bif->entry.Template.bl_ptr1->ref);
		break;
	    case ARITHIF_NODE:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		break;
	    case ASSIGN_STAT:
	    case IDENTIFY:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check l_val */
		visit_llnd(bif, bif->entry.Template.ll_ptr2); /* check r_val */
		break;
	    case PROC_STAT:
		visit_llnd(bif, bif->entry.Template.ll_ptr1); /* check l_val */
		break;
	    case CONT_STAT:
	    case FORMAT_STAT:
	    case GOTO_NODE:
	    case ASSGOTO_NODE:
	    case COMGOTO_NODE:
	    case STOP_STAT:
	    case VAR_DECL:
	    case PARAM_DECL:
	    case DIM_STAT:
	    case EQUI_STAT:
	    case DATA_DECL:
	    case IMPL_DECL:
	    case READ_STAT:
	    case WRITE_STAT:
	    case OTHERIO_STAT:
	    case COMM_STAT:
	    case CONTROL_END:
		break;
	    default:
		break;
	}
}


/****************************************************************
 *								*
 *   cvisit_llnd -- recursively visit the low level nodes and	*
 *	 	    find those use and def info it references	*
 *                  for VPC++					*
 *								*
 *   input:							*
 *	    bif  - the bif node to which the llnd belongs	*
 *	    llnd - the low level node to be visit		*
 *								*
 ****************************************************************/
void
cvisit_llnd(bif,llnd)
PTR_BFND        bif;
PTR_LLND        llnd;

{    
        if (!llnd) return;

        switch (llnd->variant) {
        case INT_VAL    :
        case STMT_STR   : 
        case FLOAT_VAL  :
        case DOUBLE_VAL :
        case STRING_VAL :
        case BOOL_VAL   :
        case CHAR_VAL   :
                break;
        case CONST_REF  :
        case ENUM_REF   :
                break;
        case VAR_REF    :
		if(check_ref(llnd->entry.Template.symbol->id) == 0)
			build_ref(llnd->entry.Template.symbol, bif);
		break;
        case POINTST_OP :                /* New added for VPC */
        case RECORD_REF:	         /* Need More */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                /* Need More work for pointer combined with structure */
                break ;
	case ARRAY_OP :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case ARRAY_REF  :
		if(check_ref(llnd->entry.Template.symbol->id) == 0)
			build_ref(llnd->entry.Template.symbol, bif);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                break;
        case CONSTRUCTOR_REF    :
                break;
        case ACCESS_REF :
                break;
        case CONS       :
                break;
        case ACCESS     :
                break;
        case IOACCESS   :
                break;
        case PROC_CALL  :
        case FUNC_CALL  :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                break;
        case EXPR_LIST  :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
                break;
        case EQUI_LIST  :
                break;
        case COMM_LIST  :
                break;
        case VAR_LIST   :
        case CONTROL_LIST       :
                break;
        case RANGE_LIST :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
                break;
        case DDOT       :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
                break;
        case COPY_NODE :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
                break;
        case VECTOR_CONST : /* NEW ADDED FOR VPC++  */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
		break;
        case INIT_LIST:
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                break ;
        case BIT_NUMBER:
                break ;
        case DEF_CHOICE :
        case SEQ        :
                break;
        case SPEC_PAIR  :
                break;
        case MOD_OP     :
	        break;
        case ASSGN_OP   :                /* New added for VPC */
        case ARITH_ASSGN_OP:             /* New added for VPC */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case EQ_OP      :
        case LT_OP      :
        case GT_OP      :
        case NOTEQL_OP  :
        case LTEQL_OP   :
        case GTEQL_OP   :
        case ADD_OP     :
        case SUBT_OP    :
        case OR_OP      :
        case MULT_OP    :
        case DIV_OP     :
        case AND_OP     :
        case EXP_OP     :
        case LE_OP      :                /* New added for VPC *//*Duplicated*/
        case GE_OP      :                /* New added for VPC *//*Duplicated*/
        case NE_OP      :                /* New added for VPC *//*Duplicated*/
        case BITAND_OP    :              /* New added for VPC */
        case BITOR_OP     :              /* New added for VPC */
        case LSHIFT_OP         :         /* New added for VPC */
        case RSHIFT_OP         :         /* New added for VPC */
        case INTEGER_DIV_OP :             /* New added for VPC */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
                break;
        case FUNCTION_OP:
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case ADDRESS_OP   :               /* New added for VPC */
        case SIZE_OP      :               /* New added for VPC */
                break;
        case DEREF_OP :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
	        break;
        case SUB_OP   :                  /* duplicated unary minus  */
        case MINUS_OP   :                /* unary operations */
        case UNARY_ADD_OP      :         /* New added for VPC */
        case BIT_COMPLEMENT_OP :         /* New added for VPC */
        case NOT_OP     :
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
		break;
        case MINUSMINUS_OP:              /* New added for VPC */
        case PLUSPLUS_OP  :              /* New added for VPC */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case STAR_RANGE :
                break;
        case CLASSINIT_OP :               /* New added for VPC */
                break ;
        case CAST_OP :             /* New added for VPC */
	         break;
        case EXPR_IF      :               /* New added for VPC */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case EXPR_IF_BODY :               /* New added for VPC */
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr1);
                cvisit_llnd(bif,llnd->entry.Template.ll_ptr2);
		break;
        case FUNCTION_REF :               /* New added for VPC */
                break ;
        case LABEL_REF: /* Fortran Version, For VPC we need more */
                  break;

        default                 :
                  break;

        }
}


/****************************************************************
 *								*
 *   cvisit_bfnd -- visits the subtree "bif" and generates the	*
 *	            use-definition info of the variables it	*
 *		    references	for VPC++			*
 *   input:							*
 *	     bif - the root of the tree to be visitd		*
 *								*
 *   side effect:						*
 *	     build the ud_chain at where the static variable	*
 *	     "head_symb" points to				*
 *								*
 ****************************************************************/
void
cvisit_bfnd(bif)
PTR_BFND     bif;

{
        register PTR_BLOB  b;
	void cvisit_llnd();

        if (!bif) return;
        pt = temp;        /* reset the pointer */

        switch (bif->variant) {
        case GLOBAL     :
        case PROG_HEDR  :
        case PROC_HEDR  :  
        case FUNC_HEDR  :
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
                break;
        case IF_NODE    :
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); /* check cond */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
		for (b = bif->entry.Template.bl_ptr2; b; b = b->next)
			cvisit_bfnd(b->ref);
                break;
        case LOGIF_NODE :
        case ARITHIF_NODE:
        case WHERE_NODE :
                break;
        case FOR_NODE   :
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		cvisit_llnd(bif, bif->entry.Template.ll_ptr2); 
		cvisit_llnd(bif, bif->entry.Template.ll_ptr3); 
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
                break;
        case FORALL_NODE        :
        case WHILE_NODE :
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
                break;
        case ASSIGN_STAT:
        case IDENTIFY:
        case PROC_STAT  :
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
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
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
        case CLASS_DECL:                /* New added for VPC */
		break;
        case ENUM_DECL :                /* New added for VPC */
        case UNION_DECL:                /* New added for VPC */
        case STRUCT_DECL:               /* New added for VPC */
	        break;
        case DERIVED_CLASS_DECL:        /* Need More for VPC */
        case VAR_DECL:
		break;
        case EXPR_STMT_NODE:            /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		break ;
        case DO_WHILE_NODE:             /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
                break;
        case SWITCH_NODE :              /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
		break ;
        case CASE_NODE :                /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		break ;
        case DEFAULT_NODE:              /* New added for VPC */
		break;
        case BASIC_BLOCK :
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
	        break ;
        case BREAK_NODE  :              /* New added for VPC */
	        break;
        case CONTINUE_NODE:             /* New added for VPC */
		break;
        case RETURN_NODE  :             /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		break;
        case ASM_NODE     :             /* New added for VPC */
               break;                   /* Need More         */
        case SPAWN_NODE :             /* New added for VPC */
               break;
        case PARFOR_NODE  :             /* New added for VPC */
		cvisit_llnd(bif, bif->entry.Template.ll_ptr1); 
		cvisit_llnd(bif, bif->entry.Template.ll_ptr2); 
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
               break;
        case PAR_NODE  :             /* New added for VPC */
		for (b = bif->entry.Template.bl_ptr1; b; b = b->next)
			cvisit_bfnd(b->ref);
		break;
        default:
                break; 
	}
     
}


/****************************************************************
 *								*
 *  gen_udchain -- visits the bif tree of the given "proj"	*
 *		   and generates the use-definition info the	*
 *		   proj has referenced				*
 *								*
 *   input:							*
 *	    proj -- the project to be visited			*
 *								*
 ****************************************************************/
void
gen_udchain(proj)
	PTR_FILE proj;
{
	if(proj->head_bfnd == NULL)
		return;

	proj_filename = (char *) calloc(strlen(proj->filename), sizeof(char));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,proj_filename, 0);
#endif
	head_symb = proj->head_symb;
	switch (language) {
	case ForSrc:
		visit_bfnd(proj->global_bfnd);
		break;
	case CSrc:
		cvisit_bfnd(proj->global_bfnd);
		break;
	default:
		break;
	}
}


void
dump_udchain(proj)
	PTR_FILE proj;
{
	register PTR_SYMB s;
	register PTR_BLOB b;

	if(proj->global_bfnd)
	    for (s = proj->head_symb; s; s = s->thread) {
		if (s->ud_chain) {
		    fprintf(stderr, "Variable \"%s\" referenced at line(s) -- ",
			       s->ident);
		    for(b = s->ud_chain; b; b = b->next)
			    fprintf(stderr, "%d%s", b->ref->g_line,
				   (b->next? ", ": "\n"));
	    }
	}
}


static void
clean_hash_tbl(fi)
	PTR_FILE fi;
{
	register PTR_HASH h, h1, h2;

	for (h = *(fi->hash_tbl); h < *(fi->hash_tbl)+hashMax; h++)
		if (h) {
			for (h1 = h->next_entry; h1; h1 = h2) {
				h2 = h1->next_entry;
#ifdef __SPF
                removeFromCollection(h1);
#endif
				free(h1);
			}
			h = NULL;
		}
}


static void
free_dep(fi)
	PTR_FILE fi;
{
	register PTR_BLOB bl1, bl2;
	register PTR_BFND bf;

	clean_hash_tbl(fi);
	for (bf = fi->global_bfnd; bf; bf = bf->thread) {
		for (bl1 = bf->entry.Template.bl_ptr1; bl1; bl1 = bl2) {
			bl2 = bl1->next;
#ifdef __SPF
            removeFromCollection(bl1);
#endif
			free(bl1);
		}
		for (bl1 = bf->entry.Template.bl_ptr2; bl1; bl1 = bl2) {
			bl2 = bl1->next;
#ifdef __SPF
            removeFromCollection(bl1);
#endif
			free(bl1);
		}
	}
 
	if (fi->num_bfnds)
		free(fi->head_bfnd);

    if (fi->num_llnds)
    {
#ifdef __SPF
        removeFromCollection(fi->head_llnd);
#endif
        free(fi->head_llnd);
    }

	if (fi->num_symbs) {
		register PTR_SYMB s;

        for (s = fi->head_symb; s; s = s)
        {
#ifdef __SPF
            removeFromCollection(s->ident);
#endif
            free(s->ident);
        }
#ifdef __SPF
        removeFromCollection(fi->head_symb);
#endif
		free(fi->head_symb);
	}

    if (fi->num_label)
    {
#ifdef __SPF
        removeFromCollection(fi->head_lab);
#endif
        free(fi->head_lab);
    }

    if (fi->num_types)
    {
#ifdef __SPF
        removeFromCollection(fi->head_type);
#endif
        free(fi->head_type);
    }

    if (fi->num_dep)
    {
#ifdef __SPF
        removeFromCollection(fi->head_dep);
#endif
        free(fi->head_dep);
    }

	if (fi->num_cmnt) {
		register PTR_CMNT c;

        for (c = fi->head_cmnt; c; c = c->next)
        {
#ifdef __SPF
            removeFromCollection(c->string);
#endif
            free(c->string);
        }
#ifdef __SPF
        removeFromCollection(fi->head_cmnt);
#endif
		free(fi->head_cmnt);
	}
}


int
replace_dep(filename)
	char	*filename;
{
	PTR_FILE fi;
	PTR_BLOB bl;
	extern PTR_PROJ cur_proj;

	for (bl = cur_proj->file_chain; bl; bl = bl->next) {
		fi = (PTR_FILE) bl->ref;
		if (!strcmp(fi->filename, filename)) {
#ifdef __SPF
            removeFromCollection(fi);
#endif
			free_dep(fi);
			read_nodes(fi);
			return (1);
		}
	}
	return (0);
}
