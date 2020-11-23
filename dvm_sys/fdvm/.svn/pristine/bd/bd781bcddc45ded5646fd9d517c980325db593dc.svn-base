/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include <stdlib.h>

#include "db.h"
#include "list.h"
 
/* the following declarations are temporary fixes until we */
/* decide how to deal with numbering and write nodes.	   */

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

struct bfnd cbfnd;
struct dep  cdep;

static LIST lis_array;
static int list_not_ready = 1;

/* end of declaration hack */

extern PTR_FILE cur_file;

PTR_BFND make_bfnd();
PTR_BLOB make_blob();
PTR_LLND make_llnd();
PTR_LLND copy_llnd();
PTR_SYMB make_symb();

/************************************************************************
 *									*
 *	List manipuliation functions alloc_list(), push_llnd()		*
 *	push_symb(), free_list() to be used by make_expr()		*
 *									*
 ************************************************************************/

LIST
alloc_list(type)
	int type;
{
	int i;

	if(list_not_ready){
		lis_array   =  (LIST) calloc(NUMLIS, sizeof(struct lis_node));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,lis_array, 0);
#endif
		for(i = 0; i < NUMLIS; i++) 
			lis_array[i].variant = UNUSED; 
		list_not_ready = 0;
	}
	for(i = 0; i < NUMLIS; i++)
		if(lis_array[i].variant == UNUSED){
			lis_array[i].variant = type;
			return(&lis_array[i]);
		}
	return(NULL);
}


/* push the low level node llnd on the front of list lis */
LIST
push_llnd(llnd, lis)
	PTR_LLND llnd;
	LIST lis;
{
	LIST nl;

	nl = alloc_list(LLNDE);
	nl->entry.llnd = llnd;
	nl->next = lis;
	return(nl);
}


/* push the symb node symb on the front of list lis */
LIST
push_symb(symb, lis)
	PTR_SYMB symb;
	LIST lis;
{
	LIST nl;

	nl = alloc_list(SYMNDE);
	nl->entry.symb = symb;
	nl->next = lis;
	return(nl);
}


void
free_list(lis)
	LIST lis;
{
	LIST nxt;

	while(lis != NULL){
		lis->variant = UNUSED;
		nxt = lis->next;
		lis->next = NULL;
		lis = nxt;
	}
}



/************************************************************************
 *									*
 *		blob list manipulation routines car, cdr, append.	*
 *									*
 ************************************************************************/

#define car(bl_list)  bl_list->ref
#define cdr(bl_list)  bl_list->next

PTR_BLOB
cons( bif, bl_list)
	PTR_BFND bif;
	PTR_BLOB bl_list;
{
	return (make_blob(cur_file, bif, bl_list));
}


/* append without copy -- not standard lisp append */
PTR_BLOB
append(bl_list, bif)
	PTR_BLOB bl_list;
	PTR_BFND bif;
{
	PTR_BLOB b;

	if (bl_list == NULL)
		return(make_blob(cur_file, bif, NULL));

	for (b = bl_list; b->next; b = b->next)
		;
	b->next = make_blob(cur_file, bif, NULL);
	return(bl_list);
}




/*
 * get_r_follow_node recursively checks source and all of its decendents until
 * it finds the ith dependence.	 It returns the node on the same level as
 * source.
 */
PTR_BFND
get_r_follow_node(par,source,bfptr,j,i)
    PTR_BFND bfptr, par, source;
    int *j;
    int i;
{
	PTR_DEP  p;
	PTR_BFND targ;
	PTR_BLOB b;
	PTR_BFND child, final; 

	p = bfptr->entry.Template.dep_ptr1;
	while(( p != NULL) && ( *j <= i)) {
		if((p->to.stmt != source) &&
		   ((p->type == 0) ||(p->type == 1) ||(p->type == 2))
		   ){
			if( *j == i){ 
				targ = p->to.stmt;
				while(targ != NULL && targ->variant != GLOBAL &&
				      targ->control_parent != par) targ = targ->control_parent; 
				if(targ->variant == GLOBAL) return(NULL);
				else if (targ == source) p = p->from_fwd;
				else return( targ);
			}
			else {
				p =p->from_fwd;
				*j = (*j)+1;
			}
		}
		else p =p->from_fwd;
	} 
	if(p == NULL && (bfptr->variant == FOR_NODE || bfptr->variant == FORALL_NODE || bfptr->variant == IF_NODE)){ 
		b = bfptr->entry.Template.bl_ptr1;
		while(b != NULL && *j <=i){ 
			child = b->ref;
			final = get_r_follow_node(par,source,child,j,i);
			if(final != NULL && final != source) return(final);
			b = b->next;
		}
	}
	if(p == NULL && bfptr->variant == IF_NODE){ 
		b = bfptr->entry.Template.bl_ptr2;
		while(b != NULL && *j <=i){ 
			child = b->ref;
			final = get_r_follow_node(par,source,child,j,i);
			if(final != NULL && final != source) return(final);
			b = b->next;
		} 
	}
	/* if *j <= i then we are not there yet but out of dependences and childern so return null */
  
	return(NULL);
}


/* returns pointer to i-th bf-node following *bfptr in dep order */
PTR_BFND
get_follow_node(bfptr,i)
	PTR_BFND bfptr;
	int i;
{
	PTR_BFND par    = bfptr->control_parent,
	source = bfptr;
	int	     j	    = 0;
    
	return(get_r_follow_node(par,source,bfptr,&j,i));
}  

/****************************************************************
 *								*
 *		MAKE functions: make_expr(),			*
 *				mk_llnd(),			*
 *				make_ddnd(),			*
 *				mk_symb(),			*
 *				make_asign()			*
 *				make_for()  & mkloop()		*
 *				make_cntlend()			*
 *								*
 ****************************************************************/

PTR_LLND
mk_llnd(PTR_LLND p)
/*	PTR_LLND p;*/
{
	PTR_LLND nd;
	
	nd = make_llnd(cur_file, 0, NULL, NULL, NULL);
	if (p != NULL){
		nd->variant = p->variant;
		nd->type = p->type;   
		nd->entry.Template.symbol = p->entry.Template.symbol;
		nd->entry.Template.ll_ptr1 = p->entry.Template.ll_ptr1;
		nd->entry.Template.ll_ptr2 = p->entry.Template.ll_ptr2;
	} else
		nd->variant = VAR_REF;
	return(nd);
}


PTR_SYMB
mk_symb(name,p)
	char    *name;
	PTR_SYMB p;
{
	PTR_SYMB nd;
	
	nd = make_symb(cur_file, 0, name);
	if (p != NULL){
		nd->variant = p->variant;
		nd->type = p->type;
		nd->next_symb = p->next_symb;
		p->next_symb = nd;
		nd->parent = p->parent;
	} else {
		nd->variant = VARIABLE_NAME;
		nd->type = NULL;
		nd->next_symb = NULL;
		nd->parent = NULL;
	}
	nd->entry.var_decl.local = LOCAL;
	nd->outer = NULL;
	nd->id_list = NULL;
	
	return(nd);
}


static LIST lispt;

/* op = one of ADD_OP SUBT_OP MULT_OP DIV_OP (or other binary ops) */
PTR_LLND
make_oper(op)
	int op;
{
	PTR_LLND nd;

	nd = mk_llnd(NULL);
	nd->variant = op;
	return(nd);
}


PTR_LLND
make_arref(ar,index)
    PTR_SYMB ar;
    PTR_LLND index;
{
	PTR_LLND nd;

	nd = mk_llnd(NULL);
	nd->variant = ARRAY_REF;
	nd->entry.array_ref.symbol = ar;
	nd->entry.array_ref.index = index;
	nd->entry.array_ref.array_elt = NULL;
	return(nd);
}


PTR_LLND
make_int(i)
	int i;
{
	PTR_LLND nd;

	nd = mk_llnd(NULL);
	nd->variant = INT_VAL;
	nd->entry.ival = i;
	return(nd);
}


PTR_LLND
hmake_expr()
{
	LIST lis;
	PTR_LLND nd;
     
	if (lispt  == NULL)
		return(NULL);

	lis = lispt;
	lispt = lis->next;
	if (lis->variant == SYMNDE){
		nd = mk_llnd(NULL);
		if(lis->entry.symb->variant == VARIABLE_NAME)
			nd->variant = VAR_REF;
		else
			fprintf(stderr, "wrong symbol type in make_expr");
		nd->entry.Template.symbol = lis->entry.symb;
		return(nd);
	} else if(lis->variant == LLNDE){
		nd = lis->entry.llnd;
		switch (nd->variant) {
		case DDOT	:
		case EQ_OP	:
		case LT_OP	:
		case GT_OP	:
		case NOTEQL_OP	:
		case LTEQL_OP	:
		case GTEQL_OP	:
		case ADD_OP	:
		case SUBT_OP 	:
		case OR_OP	:
		case MULT_OP 	:
		case DIV_OP	:
		case MOD_OP	:
		case AND_OP	:
		case EXP_OP	:
			if (nd->entry.binary_op.l_operand == NULL){
				nd->entry.binary_op.l_operand =
					hmake_expr();
				nd->entry.binary_op.r_operand =
					hmake_expr();
			}
			break;
		case MINUS_OP	:
		case NOT_OP	:
			if (nd->entry.unary_op.operand == NULL){
				nd->entry.unary_op.operand =
					hmake_expr();
			}
			break;
			
		default:
			break;
		}
		return(nd);
	}
	return NULL;
}


/*
 * this routine creates a low level expression tree from the preorder
 * list of llnds and symbol pointers then deletes the list
 */
PTR_LLND
make_expr(lis)
	LIST lis;
{
	LIST L;
	PTR_LLND n;

	L = lis;
	lispt = lis;
	n = hmake_expr();
	free_list(L);
	return(n);
}


PTR_BFND
make_asign(lhs,rhs)
	PTR_LLND lhs,rhs;
{
	return(make_bfnd(cur_file, ASSIGN_STAT, NULL, lhs, rhs, NULL));
}


PTR_BFND
make_for(index,range)
	PTR_SYMB index;
	PTR_LLND range;
{
	return(make_bfnd(cur_file, FOR_NODE, index, range, NULL, NULL));
}


/*
 * make a for_node like *p
 * this is a special version used by distribute
 */
PTR_BFND
mkloop(p)
	PTR_BFND p;
{
	PTR_BFND newp;
 
	/* we should be making new copies of the following structures! */
	newp = make_bfnd(cur_file,
			 FOR_NODE,
			 p->entry.Template.symbol,
			 p->entry.Template.ll_ptr1,
			 p->entry.Template.ll_ptr2,
			 p->entry.Template.ll_ptr3);

	newp->entry.Template.bf_ptr1  = p->entry.Template.bf_ptr1;
	newp->entry.Template.cmnt_ptr  = p->entry.Template.cmnt_ptr;
	
	newp->filename = p->filename;
	return(newp);
}



PTR_BFND
make_cntlend(par)
	PTR_BFND par;
{
	PTR_BFND b;

	b = make_bfnd(cur_file, CONTROL_END, NULL, NULL, NULL, NULL);
	b->control_parent = par;
	return(b);
}
 

static int modified = 0;

/* create a NEW low level node tree with cvar replaced by newref */
PTR_LLND
replace_ref(lnd,cvar,newref)
	PTR_LLND lnd;
	PTR_SYMB cvar;
	PTR_LLND newref;
{
	PTR_LLND pllnd, rtnval;
	
	if (lnd == NULL) return(NULL);
	
	pllnd = mk_llnd(lnd);
	rtnval = pllnd;

	switch (pllnd->variant) {
	case CONST_REF:
	case VAR_REF  :
	case ENUM_REF :
		if( pllnd->entry.Template.symbol==cvar){
			/* replace with subtree consisting of newref */
			modified = 1;
			rtnval = copy_llnd(newref);
		}
		break;
	case ARRAY_REF:
		pllnd->entry.array_ref.index =
			replace_ref(pllnd->entry.array_ref.index,cvar,newref);
		if (pllnd->entry.array_ref.array_elt != NULL) {
			pllnd->entry.array_ref.array_elt =
				replace_ref(pllnd->entry.array_ref.array_elt,cvar,newref);
		}
		break;
	case RECORD_REF:
		if (pllnd->entry.record_ref.rec_field != NULL) {
			pllnd->entry.record_ref.rec_field =
				replace_ref(pllnd->entry.record_ref.rec_field,cvar,newref);
		}
		break;
	case PROC_CALL	:
	case FUNC_CALL	:
		pllnd->entry.proc.param_list =
			replace_ref(pllnd->entry.proc.param_list,cvar,newref);
		break;
	case VAR_LIST	:
	case EXPR_LIST	:
	case RANGE_LIST	:
		pllnd->entry.list.item =
			replace_ref(pllnd->entry.list.item,cvar,newref);
		if (pllnd->entry.list.next != NULL) {
			pllnd->entry.list.next =
				replace_ref(pllnd->entry.list.next,cvar,newref);
		}
		break;

	case CASE_CHOICE:
	case DDOT	:
		pllnd->entry.binary_op.l_operand =
			replace_ref(pllnd->entry.binary_op.l_operand,cvar,newref);
		pllnd->entry.binary_op.r_operand =
			replace_ref(pllnd->entry.binary_op.r_operand,cvar,newref);
		break;
		/* binary ops */
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
		pllnd->entry.binary_op.l_operand =
			replace_ref(pllnd->entry.binary_op.l_operand,cvar,newref);
		pllnd->entry.binary_op.r_operand =
			replace_ref(pllnd->entry.binary_op.r_operand,cvar,newref);
		break;
	case MINUS_OP:
	case NOT_OP	 :
		pllnd->entry.unary_op.operand =
			replace_ref(pllnd->entry.unary_op.operand,cvar,newref);
		break;
	default:
		break;
	}
	return(rtnval);
}


/* routine to make double dot node  low..hi  from an expression */
PTR_LLND
make_ddnd(pllnd,cvar,low,hi)
	PTR_LLND pllnd,low,hi;
	PTR_SYMB cvar;
{
	PTR_LLND tmp, dotnd;
	
	tmp = replace_ref(pllnd,cvar,low);
	if(modified){ 
		dotnd = mk_llnd(NULL);
		dotnd->variant = DDOT;
		dotnd->entry.Template.symbol = NULL;
		dotnd->entry.Template.ll_ptr1 = tmp;
		dotnd->entry.Template.ll_ptr2 = 
			replace_ref(pllnd,cvar,hi);
		return(dotnd);
	}
	else return(pllnd);
}


/*
 * create a new ddot node for every array-ref in expression containing
 * a reference to cvar
 */
void
expand_ref(pllnd,cvar,low,hi)
	PTR_LLND	pllnd;
	PTR_SYMB	cvar;
	PTR_LLND	low,hi;
{
	if (pllnd == NULL) return;

	switch (pllnd->variant) {
	case ARRAY_REF:
		/* [ */
		modified = 0;	/* set changed flag */
		if((pllnd->entry.array_ref.index->variant != EXPR_LIST) &&
		   (pllnd->entry.array_ref.index->variant != RANGE_LIST))
			pllnd->entry.array_ref.index =
				make_ddnd(pllnd->entry.array_ref.index,cvar,low,hi);
		else expand_ref(pllnd->entry.array_ref.index,cvar,low,hi);
		
		/* otherwise this is a scalar reference and should */
		/* not be changed here. In any case reset flag	   */
		modified = 0;
		/* ] */
		break;
	case RECORD_REF:
		if (pllnd->entry.record_ref.rec_field != NULL) 
			expand_ref(pllnd->entry.record_ref.rec_field,cvar,low,hi);
		break;
	case PROC_CALL:
	case FUNC_CALL:
		expand_ref(pllnd->entry.proc.param_list,cvar,low,hi);
		break;
	case VAR_LIST :
	case EXPR_LIST:
	case RANGE_LIST:
		/* the other place where something can happen is here	*
		 * if we have a[i,j] and we are vectorizing j then this *
		 * should be a[i,low..hi], unless it is i we are after	*/
		modified = 0;
		pllnd->entry.list.item =
			make_ddnd(pllnd->entry.list.item,cvar,low,hi);
		modified = 0;
		if (pllnd->entry.list.next != NULL) {
			/* pllnd->entry.list.next = */
			expand_ref(pllnd->entry.list.next,cvar,low,hi);
			modified = 0;
		}
		break;
	case EQ_OP	:
	case LT_OP	:
	case GT_OP	:
	case NOTEQL_OP:
	case LTEQL_OP:
	case GTEQL_OP:
	case ADD_OP	:
	case SUBT_OP:
	case OR_OP	:
	case MULT_OP:
	case DIV_OP	:
	case MOD_OP	:
	case AND_OP	:
	case EXP_OP	:
		expand_ref(pllnd->entry.binary_op.l_operand,cvar,low,hi);
		expand_ref(pllnd->entry.binary_op.r_operand,cvar,low,hi);
		break;
	case MINUS_OP:
		expand_ref(pllnd->entry.unary_op.operand,cvar,low,hi);
		break;
	case NOT_OP	:
		expand_ref(pllnd->entry.unary_op.operand,cvar,low,hi);
		break;
	default:
		break;
	}
}
