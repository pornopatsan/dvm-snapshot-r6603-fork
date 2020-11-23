/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include <stdlib.h>

#include "db.h"
#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

#define ALLOC(x)  (struct x *) chkalloc(sizeof(struct x))
#define LABUNKNOWN  0

/*
 * External references
 */
extern PTR_FILE cur_file;

/*
 * copyn -- makes a copy of a string with known length
 *
 * input:
 *	  n - length of the string "s"
 *	  s - the string to be copied
 *
 * output:
 *	  pointer to the new string
 */
char   *
copyn(int n, char *s)
/*	int n; */
/*	char *s; */
{
	char *p, *q;

	p = q = (char *) calloc(1, (unsigned) n);
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
	while (--n >= 0)
		*q++ = *s++;
	return (p);
}


/*
 * copys -- makes a copy of a string
 *
 * input:
 *	  s - string to be copied
 *
 * output:
 *	  pointer to the new string
 */
char   *
copys(s)
	char   *s;
{
	return (copyn(strlen(s) + 1, s));
}


char *
chkalloc(int n)
/*	int n; */
{
	char *p;

    if ((p = (char *)calloc(1, (unsigned)n)) != 0)
    {
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,p, 0);
#endif
        return (p);
    }
	return NULL;
}


PTR_BFND
alloc_bfndnt (fi)
	PTR_FILE fi;
{
	register PTR_BFND new;

	new = ALLOC (bfnd);
	new->id = ++(fi->num_bfnds);
	new->thread = BFNULL;
	return (new);
}

PTR_BFND
alloc_bfnd (fi)
	PTR_FILE fi;
{
	register PTR_BFND new;

	new = ALLOC (bfnd);
	new->id = ++(fi->num_bfnds);
	new->thread = BFNULL;
	if (fi->num_bfnds == 1)
		fi->head_bfnd = new;
	else
		fi->cur_bfnd->thread = new;
	fi->cur_bfnd = new;
	return (new);
}


PTR_LLND
alloc_llnd (fi)
	PTR_FILE fi;
{
	register PTR_LLND new;

	new = ALLOC (llnd);
	new->id = ++(fi->num_llnds);
	new->thread = LLNULL;
	if (fi->num_llnds == 1)
		fi->head_llnd = new;
	else
		fi->cur_llnd->thread = new;
	fi->cur_llnd = new;
	return (new);
}


PTR_TYPE
alloc_type (fi)
	PTR_FILE fi;
{
	PTR_TYPE new;

	new = (PTR_TYPE) calloc (1, sizeof (struct data_type));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,new, 0);
#endif
	new->id = ++(fi->num_types);
	new->thread = TYNULL;
	if (fi->num_types == 1)
		fi->head_type = new;
	else
		fi->cur_type->thread = new;
	fi->cur_type = new;
	return (new);
}


PTR_SYMB
alloc_symb (fi)
	PTR_FILE fi;
{
	PTR_SYMB new;

	if (fi->cur_symb && (fi->cur_symb->variant == 0))
		return (fi->cur_symb);
	new = ALLOC (symb);
	new->id = ++(fi->num_symbs);
	new->thread = SMNULL;
	if (fi->num_symbs == 1)
		fi->head_symb = new;
	else
		fi->cur_symb->thread = new;
	fi->cur_symb = new;
	return (new);
}


PTR_LABEL
alloc_lab (fi)
	PTR_FILE fi;
{
	PTR_LABEL new;

	new = ALLOC (Label);
	new->id = ++(fi->num_label);
	new->next = LBNULL;
	if (fi->num_label == 1)
		fi->head_lab = new;
	else
		fi->cur_lab->next = new;
	fi->cur_lab = new;
	return (new);
}
 
 
PTR_DEP
alloc_dep (fi)
	PTR_FILE fi;
{
	PTR_DEP new;

	new = ALLOC (dep);
	new->id = ++(fi->num_dep);
	new->thread = NULL;
	if (fi->num_dep == 1)
	    fi->head_dep = new;
	else
	    fi->cur_dep->thread = new;
	fi->cur_dep = new;
	return (new);
}


/*
 * Make a BIF node
 */
PTR_BFND
make_bfnd (PTR_FILE fi, int node_type, PTR_SYMB symb_ptr, PTR_LLND ll1, PTR_LLND ll2, PTR_LLND ll3)
/*	PTR_FILE fi; */
/*	int	 node_type; */
/*	PTR_SYMB symb_ptr;  */
/*	PTR_LLND ll1, ll2, ll3; */
{
	register PTR_BFND new_bfnd;

	new_bfnd = alloc_bfnd (fi);	/* should set up id field */
	new_bfnd->variant = node_type;
	new_bfnd->filename = NULL;
	new_bfnd->entry.Template.symbol = symb_ptr;
	new_bfnd->entry.Template.ll_ptr1 = ll1;
	new_bfnd->entry.Template.ll_ptr2 = ll2;
	new_bfnd->entry.Template.ll_ptr3 = ll3;
	new_bfnd->entry.Template.cmnt_ptr = NULL;
	fi->cur_bfnd = new_bfnd;
	return (new_bfnd);
}

PTR_BFND
make_bfndnt (fi, node_type, symb_ptr, ll1, ll2, ll3)
	PTR_FILE fi;
	int	 node_type;
	PTR_SYMB symb_ptr;
	PTR_LLND ll1, ll2, ll3;
{
	register PTR_BFND new_bfnd;

	new_bfnd = alloc_bfndnt (fi);	/* should set up id field */
	new_bfnd->variant = node_type;
	new_bfnd->filename = NULL;
	new_bfnd->entry.Template.symbol = symb_ptr;
	new_bfnd->entry.Template.ll_ptr1 = ll1;
	new_bfnd->entry.Template.ll_ptr2 = ll2;
	new_bfnd->entry.Template.ll_ptr3 = ll3;
	new_bfnd->entry.Template.cmnt_ptr = NULL;
	fi->cur_bfnd = new_bfnd;
	return (new_bfnd);
}

/*
 * Make a new low level node
 */
PTR_LLND
make_llnd (PTR_FILE fi, int node_type, PTR_LLND ll1, PTR_LLND ll2, PTR_SYMB symb_ptr)
/*	PTR_FILE fi; */
/*	int	 node_type; */
/*	PTR_LLND ll1, ll2; */
/*	PTR_SYMB symb_ptr; */
{
	PTR_LLND new_llnd;

	new_llnd = alloc_llnd (fi);	/* should set up id field */

	new_llnd->variant = node_type;
	new_llnd->type = TYNULL;
	new_llnd->entry.Template.ll_ptr1 = ll1;
	new_llnd->entry.Template.ll_ptr2 = ll2;
	switch (node_type) {
	  case INT_VAL:
        /*    new_llnd->entry.ival = (int) symb_ptr; */
            break;
	  case BOOL_VAL:
         /*   new_llnd->entry.bval = (int) symb_ptr; */
            break;
	  default:
            new_llnd->entry.Template.symbol = symb_ptr;
            break;
	  }
	return (new_llnd);
}


/*
 * Make a new low level node for label
 */
PTR_LLND
make_llnd_label (fi, node_type, lab)
	PTR_FILE  fi;
	int	  node_type;
	PTR_LABEL lab;
{
	PTR_LLND new_llnd;

	new_llnd = alloc_llnd (fi);	/* should set up id field */

	new_llnd->variant = node_type;
	new_llnd->type = TYNULL;
	new_llnd->entry.label_list.lab_ptr = lab;
	new_llnd->entry.label_list.null_1 = LLNULL;
	new_llnd->entry.label_list.next = LLNULL;
	return (new_llnd);
}


/*
 * Make a new symb node
 */
PTR_SYMB
make_symb (fi, node_type, string)
	PTR_FILE fi;
	int	 node_type;
	char	*string;
{
	PTR_SYMB new_symb;

	new_symb = alloc_symb (fi);
	new_symb->variant = node_type;
	new_symb->ident = copys (string);
	return (new_symb);
}


/*
 * Make a new type node
 */
PTR_TYPE
make_type (fi, node_type)
	PTR_FILE fi;
	int      node_type;
{
	PTR_TYPE new_type;

	new_type = alloc_type (fi);
	new_type->entry.Template.ranges =  NULL;
	new_type->variant = node_type;
	return (new_type);
}


/*
 * Make a new label node for Fortran.  VPC has its own get_labe
 */
PTR_LABEL
make_label (fi, l)
	PTR_FILE fi;
	long	 l;
{       
	PTR_LABEL new_lab;
	PTR_BFND  this_scope;
        int num;/*podd*/
        num = fi->cur_bfnd ? fi->cur_bfnd->g_line : 0; /*podd*/
	if (l <= 0 || l > 99999) {
	  /*	fprintf (stderr, "Error 038 on line %d of %s: Label out of range\n", num, fi->filename); */
		l = 0;
	}
	this_scope = NULL;
	for (new_lab = fi->head_lab; new_lab; new_lab = new_lab->next)
		if (new_lab->stateno == l && new_lab->scope == this_scope)
			return (new_lab);

	new_lab = alloc_lab (fi);

	new_lab->stateno = l;
	new_lab->scope = this_scope;
	new_lab->labused = NO;
	new_lab->labdefined = NO;
	new_lab->labinacc = NO;
	new_lab->labtype = LABUNKNOWN;
	new_lab->statbody = BFNULL;
	return (new_lab);
}


/*
 * Make a DEP node
 */
PTR_DEP
make_dep(fi, sym,t,lls,lld,bns,bnd,dv)
	PTR_FILE fi;
	PTR_SYMB sym;		/* symbol for variable name		*/
	char t; 		/* type: 0=flow 1=anti 2 = output	*/
	PTR_LLND lls, lld;	/* term source and destination		*/
	PTR_BFND bns, bnd;	/* biff nd source and destination	*/
	char *dv;		/* dep. vector: 1="="	2="<"  4=">" ?	*/
{
	int i;
	PTR_DEP d;

	if ((d = alloc_dep(fi)) == NULL)
		return NULL;
	d->type = t;
	d->symbol = sym;
	d->from.stmt = bns; d->from.refer = lls;
	d->to.stmt   = bnd; d->to.refer   = lld;
	for(i=0; i < MAX_DEP; i++) d->direct[i] = 0;
	for(i=0; i < MAX_NEST_DEPTH; i++) d->direct[i] = dv[i];

	return(d);
}


/*------------------------------------------------------*
 *			alloc_blob			*
 *------------------------------------------------------*/
PTR_BLOB
alloc_blob1(fi)
	PTR_FILE fi;
{
	PTR_BLOB new;

	new = ALLOC(blob);
	++(fi->num_blobs);
	return (new);
}


PTR_CMNT
alloc_cmnt (fi)
	PTR_FILE fi;
{
	PTR_CMNT new;

	new = ALLOC (cmnt);
	new->id = ++(fi->num_cmnt);
	new->thread = CMNULL;
	if (fi->num_cmnt == 1)
		fi->head_cmnt = new;
	else
		fi->cur_cmnt->thread = new;
	fi->cur_cmnt = new;
	return (new);
}


/*------------------------------------------------------*
 *			make_blob			*
 *------------------------------------------------------*/
PTR_BLOB
make_blob (fi, ref, next)
	PTR_FILE fi;
	PTR_BFND ref;
	PTR_BLOB next;
{
	PTR_BLOB new;

	new = alloc_blob1(fi);
	new->ref = ref;
	new->next = next;
	return (new);
}


PTR_CMNT
make_comment (fi, s, t)
	PTR_FILE fi;
	char	*s;
	int	 t;
{
	PTR_CMNT new;

	new = alloc_cmnt(fi);
	new->string = copys (s);
	new->type = t;
	return (new);
}


void
MakeBfnd (node_type, symb_ptr, ll1, ll2, ll3)
	int	 node_type;
	PTR_SYMB symb_ptr;
	PTR_LLND ll1, ll2, ll3;
{
	PTR_BFND b;

	b = make_bfnd (cur_file, node_type, symb_ptr, ll1, ll2, ll3);
	fprintf(stderr, "%d\n", b->id);
}


void
MakeLlnd (node_type, ll1, ll2, symb_ptr)
	int	 node_type;
	PTR_LLND ll1, ll2;
	PTR_SYMB symb_ptr;
{
	PTR_LLND l;

	l = make_llnd (cur_file, node_type, ll1, ll2, symb_ptr);
	fprintf(stderr, "%d\n", l->id);
}


void
Makellnd_label (node_type, lab)
	int     node_type;
	PTR_LABEL lab;
{
	make_llnd_label (cur_file, node_type, lab);
}


void
MakeSymb (node_type, string)
	int     node_type;
	char   *string;
{
	PTR_SYMB s;

	s = make_symb (cur_file, node_type, string);
	fprintf(stderr, "%d\n", s->id);
}


void
Maketype (node_type)
	int     node_type;
{
	PTR_TYPE t;
	t = make_type (cur_file, node_type);
	fprintf(stderr, "%d\n", t->id);
}


void
MakeLabel (l)
	long    l;
{
	PTR_LABEL l1;

	l1 = make_label (cur_file, l);
	fprintf(stderr, "%d\n",l1->id);
}


void
MakeBlob (ref, next)
	PTR_BFND ref;
	PTR_BLOB next;
{
	make_blob (cur_file, ref, next);
}


void
MakeComment (s, t)
	char   *s;
	int     t;
{
	PTR_CMNT c;

	c = make_comment (cur_file, s, t);
	fprintf(stderr, "%d\n",c->id);
}


/*
 * declare variable can be used to create a new variable in the
 * symbol table that is "like" another variable.  For example
 * if x is in a statement b and you wish to make a new variable
 * with id x_new that is an array of the same type as x (which
 * is a scalar), this function creates the new varaible and
 * creates a declartion for it at the appropriate scope level
 */
PTR_SYMB 
declare_variable (id, like, dimension, scope)
	char    *id;		/* identifier for new variable	 */
	PTR_SYMB like;		/* the Template variable	 */
	int      dimension;	/* if > 1 then this is an array */
				/* version of Template variable */
	PTR_BFND scope;		/* pointer to a statment that is */
				/* in the block where this is to */
				/* be declared */
{
	PTR_LLND expr_list, reference;
	PTR_BFND decl_stmt;
	PTR_LLND dimen_expr;
	PTR_SYMB new_var;

	if (like == NULL) {
		fprintf (stderr, "no Template in declare_varaible\n");
		return (NULL);
	}
	if (id == NULL) {
		fprintf (stderr, "no id in declare_variable\n");
		return (NULL);
	}
	if (scope == NULL) {
		fprintf (stderr, "no scope in declare_varaible\n");
		return (NULL);
	}
	new_var = make_symb (cur_file, VARIABLE_NAME, id);
	if (dimension <= 1) {
		if (like->type == NULL) {
			fprintf (stderr, "problems with type of like in declare_variable\n");
			return (NULL);
		}
		new_var->type = like->type;
		if (like->type->variant == T_ARRAY) {
			dimen_expr = make_llnd (cur_file, INT_VAL, NULL, NULL, NULL);
			dimen_expr = like->type->entry.ar_decl.ranges ->
				entry.Template.ll_ptr1;
			reference = make_llnd (cur_file, ARRAY_REF, dimen_expr,
					       NULL, new_var);
		} else
			reference = make_llnd (cur_file, VAR_REF, NULL, NULL, new_var);
	} else {
		dimen_expr = make_llnd (cur_file, INT_VAL, NULL, NULL, NULL);
		dimen_expr->entry.ival = dimension;
		reference = make_llnd (cur_file, ARRAY_REF, dimen_expr, NULL, new_var);
		new_var->type = make_type (cur_file, T_ARRAY);
		new_var->type->entry.ar_decl.base_type = like->type;
		new_var->type->entry.ar_decl.num_dimensions = 1;
		new_var->type->entry.ar_decl.ranges = dimen_expr;
	}
	expr_list = make_llnd (cur_file, EXPR_LIST, reference, NULL, NULL);
	decl_stmt = make_bfnd (cur_file, VAR_DECL, NULL, expr_list, NULL, NULL);
	scope = scope->control_parent;
	while (scope != NULL &&
	       scope->variant != GLOBAL && scope->variant != PROC_HEDR &&
	       scope->variant != PROG_HEDR && scope->variant != FUNC_HEDR &&
	       scope->variant != FOR_NODE && scope->variant != CDOALL_NODE &&
	       scope->variant != PARFOR_NODE && scope->variant != PAR_NODE)
		scope = scope->control_parent;
	if (scope == NULL || scope->variant == GLOBAL) {
		fprintf(stderr, "bad scope in declare_variable \n");
		return (NULL);
	}
	scope->entry.Template.bl_ptr1 = make_blob (cur_file, decl_stmt,
					   scope->entry.Template.bl_ptr1);
	return (new_var);
}
