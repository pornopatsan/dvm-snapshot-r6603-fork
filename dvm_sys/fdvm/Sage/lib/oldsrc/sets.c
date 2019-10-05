/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* File: sets.c */
#include "db.h"

extern PCF UnparseBfnd[];
extern PCF UnparseLlnd[];

extern PTR_FILE cur_file;

#define PLUS 2
#define ZPLUS 3
#define MINUS 4
#define ZMINUS 5
#define PLUSMINUS 6
#define NODEP -1
#define FLOWD 1
#define OUTPUTD 2
#define ANTID -1
#define INPUTD 3

extern char *tag[611];
extern struct subscript source[AR_DIM_MAX];   /* a source reference or def. */
extern struct subscript destin[AR_DIM_MAX];   /* a destination ref. or def. */
extern PTR_SYMB induct_list[MAX_NEST_DEPTH];
extern int is_forall[MAX_NEST_DEPTH];
extern int language;		/* is either ForSrc or CSrc */
extern int num_ll_allocated;

extern char *funparse_bfnd();
extern char *cunparse_bfnd();
extern char *funparse_llnd();
extern char *cunparse_llnd();
extern void collect_garbage();
extern void normal_form();
extern void bind_call_site_info();
extern PTR_LLND make_llnd();
extern PTR_FILE cur_file;
extern int show_deps;
extern void disp_refl();
int search_decl();
extern int comp_dist();
extern int identical();
extern void assign();
int node_count = 0;

void fix_symbol_list( b)
PTR_BFND b;
{
   PTR_BLOB bp;
   PTR_SYMB f, v;
	if(b == NULL || b->variant != GLOBAL) return;
	bp = b->entry.Template.bl_ptr1;	
	while(bp){
	   if(bp->ref->variant == PROC_HEDR ||
	      bp->ref->variant == FUNC_HEDR){
		f = bp->ref->entry.Template.symbol;
		if(f->entry.proc_decl.symb_list == NULL){
			v = f->thread;
			while(v){
				if(v->scope == bp->ref){
					f->entry.proc_decl.symb_list = v;
					v = NULL;
					}
				else{
					v = v->thread;
				    }
				}
			}
		}
	   bp=bp->next;
	   }
 }

			


/*******************************************************************/
/*	The following external functions found in setutils.c and   */
/*	anal_index.c. and symb_alg.c				   */
/*******************************************************************/

void *malloc();
PTR_SETS alloc_sets();
PTR_REFL alloc_ref();
PTR_REFL copy_refl();
PTR_REFL union_refl();
PTR_REFL intersect_refl();
PTR_REFL make_name_list();
PTR_REFL remove_locals_from_list();
PTR_REFL build_refl(), merge_array_refs();
void print_subscr();
void append_refl();
void normal_form();
void bind_call_site_info();

/* Gather_ref is a function that makes a reference node and a list */
/* for each reference to a varialbe at the tree rooted at the low  */
/* level node ll.  the parameter defs is used by C programs.  in   */
/* this case defs points to a list of definitions that are generated*/
/* durring the evaluation of this expression.			    */

PTR_REFL gather_refl(rnd, defs, bif, ll)
int rnd;			/* flag = 1 to gather refs for func. calls */
PTR_REFL *defs; 		/* for C expressions that define values */
PTR_BFND bif;
PTR_LLND ll;
{
  PTR_REFL p, q, t;
  PTR_REFL r;
  PTR_LLND a;

  if (ll == NULL)
    return (NULL);

  if (bif->variant == PROC_STAT && rnd) {
    PTR_LLND bused, bmodified;
    PTR_REFL brlu, brlm;
    /* assume global analysis done. */
    bind_call_site_info(bif, &bused, &bmodified);
    brlu = build_refl(bif, bused);
    brlu = merge_array_refs(brlu);
    brlu = merge_array_refs(brlu); /* one more pass */
    brlm = build_refl(bif, bmodified);
    brlm = merge_array_refs(brlm);
    brlm = merge_array_refs(brlm); /* one more pass */
    append_refl(defs, brlm);
    return (brlu);
  }

  if (ll->variant == VAR_REF)
    return (alloc_ref(bif, ll));
  else if ((ll->variant == PROC_CALL) || (ll->variant == FUNC_CALL))
    if (rnd) {
      PTR_LLND bused, bmodified;
      PTR_REFL brlu, brlm;
      /* assume global analysis done. */
      bind_call_site_info(bif, &bused, &bmodified);
      brlu = build_refl(bif, bused);
      brlu = merge_array_refs(brlu);
      brlu = merge_array_refs(brlu); /* one more pass */
      brlm = build_refl(bif, bmodified);
      brlm = merge_array_refs(brlm);
      brlm = merge_array_refs(brlm); /* one more pass */
      append_refl(defs, brlm);
      return (brlu);
    }
    else
      return (NULL);
  else if (ll->variant == ARRAY_REF) {
    r = alloc_ref(bif, ll);
    p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    if (rnd == 0 && bif->variant == PROC_STAT)
      t = p;
    else {
      t = union_refl(r, p);
      disp_refl(p);
    }
    return (t);
  }
  else if (ll->variant == DEREF_OP) {
    p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    return (p);
  }
  else if (ll->variant == ADDRESS_OP) {
    p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    return (p);
  }
  else if (ll->variant == POINTST_OP || ll->variant == RECORD_REF) {
    /* a->b type operation.  in this case we have a */
    /* reference to a substructure of a struct.  */
    r = alloc_ref(bif, ll);
    r->id = NULL;
    return (r);
  }
  else if (ll->variant == PLUSPLUS_OP || ll->variant == MINUSMINUS_OP) {
    p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    q = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    /* better check for predecriment too! */
    append_refl(defs, q);
    disp_refl(q);
    return (p);
  }
  else if (ll->variant == ASSGN_OP || ll->variant == ARITH_ASSGN_OP) {
    if (ll->entry.Template.ll_ptr2->variant == DEREF_OP) {
      /* create an equivalence pair for later use */
      /* i don't know what to return */
      return (NULL);
    }
    else {
      p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr2);
      a = ll->entry.Template.ll_ptr1;
      if (a->variant == VAR_REF || a->variant == POINTST_OP
	  || a->variant == RECORD_REF) {
	r = alloc_ref(bif, a);
	append_refl(defs, r);
	if (ll->variant == ARITH_ASSGN_OP) {
	  r = alloc_ref(bif, a);
	  append_refl(&p, r);
	}
	return (p);
      }
      else if (a->variant == ARRAY_REF) {
	r = alloc_ref(bif, a);
	append_refl(defs, r);
	q = gather_refl(rnd, defs, bif, a->entry.Template.ll_ptr1);
	t = union_refl(p, q);
	disp_refl(p);
	disp_refl(q);
	if (ll->variant == ARITH_ASSGN_OP) {
	  r = alloc_ref(bif, a);
	  append_refl(&t, r);
	}
	return (t);
      }
      else if (a->variant == DEREF_OP) {
	/* not so sure about this! */
	q = gather_refl(rnd, defs, bif, a->entry.Template.ll_ptr1);
	if (ll->variant == ARITH_ASSGN_OP) {
	  r = alloc_ref(bif, a);
	  append_refl(&q, r);
	}
	return (q);
      }
      else {
	q = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
	append_refl(defs, q);
	disp_refl(q);
	if (ll->variant == ARITH_ASSGN_OP) {
	  r = alloc_ref(bif, a);
	  append_refl(&p, r);
	}
	return (p);
      }
    }
  }
  else {
    p = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr1);
    q = gather_refl(rnd, defs, bif, ll->entry.Template.ll_ptr2);
    t = union_refl(p, q);
    disp_refl(p);
    disp_refl(q);
    return (t);
  }
}

static int before(bsor, bdes)
PTR_BFND bsor, bdes;
{
  return (bsor->id < bdes->id);
}


PTR_REFL rem_kill(in, gen)
PTR_REFL in, gen;
{
  /* search "in" for things in "in" that are killed by gen. */
  /* for scalars this means we just look at the ID. */
  /* for arrays we have to check for an induction variable expression */
  /* that is constant in the current iteration. */
  PTR_REFL t, g, rk, tmp;

  t = copy_refl(in);
  for (g = gen; g; g = g->next)
    for (tmp = t; tmp; tmp = tmp->next)
      if (tmp->id == g->id) {
	if ((tmp->node && (tmp->node->refer->variant == POINTST_OP ||
			   tmp->node->refer->variant == RECORD_REF)) ||
	    (g->node && (g->node->refer->variant == POINTST_OP ||
			 g->node->refer->variant == RECORD_REF))
	  ) {
	  /* don't know what to do! */
	}
	/* have a hit here. */
	else if (tmp->node->refer->variant == VAR_REF) {
	  tmp->id = NULL;
	  tmp->node = NULL;
	  /* just killed a scalar */
	}
	else {
	  /* it is an ARRAY_REF so we need much work */
	  /* the key is to kill definitions to the same subscripted */
	  /* variables that are defined in the same iteration	 */
	  /* and are lexically before the current definition.	 */
	  /* But you must then do subscript analysis.  the code  */
	  /* below gives the idea.  funct. match_subs not yet done */
	  /* it does not hurt to leave this out. the extra dep.  */
	  /* that are generated are not harmfull.		 */
	  /* for now we only kill off unsubscripted array refs */
	  /* because they are redefinitions of the whole array */
	  if (tmp->node->refer->variant == ARRAY_REF)
	    if (g->node->refer->entry.array_ref.index == NULL) {
	      tmp->id = NULL;
	      tmp->node = NULL;
	    }
	}
      }

  /* now prune out all killed nodes from t */
  rk = NULL;
  while (t) {
    tmp = t;
    t = t->next;
    tmp->next = NULL;
    if (tmp->node == NULL)
      disp_refl(tmp);
    else {
      tmp->next = rk;
      rk = tmp;
    }
  }
  return (rk);
}


/****************************************************************************
 * the rountines search_local and remove_local are used to surpress carried *
 * deps for forall loops.  search the reference list looking for references *
 * to locals								    *
 ****************************************************************************/
int search_local(b, s)
PTR_BFND b;
PTR_SYMB s;
{
  PTR_SYMB locs;
  PTR_BLOB blob;

  if (b->variant == FORALL_NODE) {
    locs = b->entry.forall_nd.control_var;
    while (locs != NULL && s != locs)
      locs = locs->next_symb;
    if (locs == s)
      return (0);
    else
      return (1);
  }
  else if (language != ForSrc) {
    blob = b->entry.Template.bl_ptr1;
    return (search_decl(blob, s));
  }
  else
    return (1);
}

int search_decl(blob, s)
PTR_BLOB blob;
PTR_SYMB s;
{
  PTR_BFND b;
  PTR_LLND ll, v;

  while (blob != NULL && blob->ref->variant != CONTROL_END) {
    b = blob->ref;
    if (b->variant == VAR_DECL) {
      ll = b->entry.Template.ll_ptr1;
      /* ll should be an expression list */
      while (ll != NULL) {
	if (ll->entry.Template.ll_ptr1 != NULL) {
	  v = ll->entry.Template.ll_ptr1;
	  if ((v->variant == VAR_REF ||
	       v->variant == ARRAY_REF) &&
	      v->entry.Template.symbol == s)
	    return (0);
	}
	ll = ll->entry.Template.ll_ptr2;
      }
    }
    blob = blob->next;
  }
  return (1);
}


PTR_REFL remove_locals(b, in)
PTR_BFND b;
PTR_REFL in;
{
  PTR_SYMB i;
  PTR_REFL t, rk, tmp;
  PTR_BFND loop;
  int notfound;

  /* prune out all killed nodes from t */
  rk = NULL;
  t = in;
  while (t != NULL) {
    tmp = t;
    t = t->next;
    i = tmp->id;
    tmp->next = NULL;
    loop = b;
    notfound = 1;
    while (loop != NULL &&
	   (loop->variant != FOR_NODE &&
	    loop->variant != WHILE_NODE &&
	    loop->variant != LOOP_NODE &&
	    loop->variant != CDOALL_NODE &&
	    loop->variant != PARFOR_NODE &&
	    loop->variant != IF_NODE &&
	    loop->variant != LOGIF_NODE &&
	    loop->variant != PAR_NODE)) {
      loop = loop->control_parent;
    }
    if (loop != NULL)
      notfound = search_local(loop, i);
    if (notfound == 0)
      disp_refl(tmp);
    else {
      tmp->next = rk;
      rk = tmp;
    }
  }
  return (rk);
}

int is_star_range(p)
PTR_LLND p;
{
  PTR_LLND q, q2;

  if (p->entry.Template.ll_ptr1 == NULL)
    return (1);
  q = p->entry.Template.ll_ptr1;/* q should be an index list */
  q2 = q->entry.Template.ll_ptr1;	/* q2 is the first index     */
  if ((q2 == NULL || q2->variant == STAR_RANGE)
      && q->entry.Template.ll_ptr2 == NULL) {
    return (1);
  }
  return (0);
}

PTR_REFL remove_scalar_dups(s)
PTR_REFL s;
{
  PTR_SYMB i;
  PTR_REFL t, arr_no_subs, arr_with_subs, final, loop, tmp, point_exps;
  PTR_LLND p;
  int notfound;

  /* prune out all killed nodes from t */
  final = NULL;
  arr_no_subs = NULL;
  arr_with_subs = NULL;
  point_exps = NULL;
  t = s;
  while (t != NULL) {
    tmp = t;
    t = t->next;
    p = tmp->node->refer;
    i = p->entry.Template.symbol;
    tmp->next = NULL;
    if (p->variant == VAR_REF ||
	(p->variant == ARRAY_REF && is_star_range(p))) {
      if (p->variant == ARRAY_REF) {
	loop = arr_no_subs;
	notfound = 1;
	while (loop != NULL) {
	  if (loop->node->refer->entry.Template.symbol == i) {
	    notfound = 0;
	  }
	  loop = loop->next;
	}
	if (notfound) {
	  tmp->next = arr_no_subs;
	  arr_no_subs = tmp;
	}
      }
      else {
	loop = final;
	notfound = 1;
	while (loop != NULL) {
	  if (loop->node->refer->entry.Template.symbol == i)
	    notfound = 0;
	  loop = loop->next;
	}
	if (notfound) {
	  tmp->next = final;
	  final = tmp;
	}
      }
    }
    else if (tmp->node->refer->variant == ARRAY_REF) {
      tmp->next = arr_with_subs;
      arr_with_subs = tmp;
    }
    else 
     if(tmp->node->refer->variant==POINTST_OP
	|| tmp->node->refer->variant == RECORD_REF) {
      tmp->next = point_exps;
      point_exps = tmp;
    }
  }				/* end while */
  t = arr_with_subs;
  while (t != NULL) {
    tmp = t;
    t = t->next;
    i = tmp->node->refer->entry.Template.symbol;
    tmp->next = NULL;
    notfound = 1;
    loop = arr_no_subs;
    while (loop != NULL) {
      if (loop->node->refer->entry.Template.symbol == i)
	notfound = 0;
      loop = loop->next;
    }
    if (notfound) {
      tmp->next = final;
      final = tmp;
    }
  }
  t = arr_no_subs;
  while (t != NULL) {
    tmp = t;
    t = t->next;
    tmp->next = final;
    final = tmp;
  }
  t = point_exps;
  while (t != NULL) {
    tmp = t;
    t = t->next;
    tmp->next = final;
    final = tmp;
  }
  return (final);
}


/***********************************************************************/
/*								       */
/*	dependence manipulation routines rm_dep() and append_dep()     */
/*	taken from lists.c in bled.  should be deleted from that file  */
/*								       */
/***********************************************************************/
void rm_dep(b, d)		/* remove dep d from the list out of b */
PTR_BFND b;
PTR_DEP d;
{
  PTR_DEP s, olds = NULL;

  s = b->entry.Template.dep_ptr1;
  if (s == d) {
    b->entry.Template.dep_ptr1 = d->from_fwd;
    d->from_fwd = NULL;
  }
  else {
    while ((s != NULL) && (s != d)) {
      olds = s;
      s = s->from_fwd;
    }
    if (s) {
      olds->from_fwd = s->from_fwd;
      d->from_fwd = NULL;
    }
  }
}

static int check_dep_copy(b, t, s, bf, lf, bt, lt)
PTR_BFND b, bf, bt;
PTR_SYMB s;
int t;
PTR_LLND lf, lt;
{
   PTR_DEP lst;
   lst = b->entry.Template.dep_ptr1;
   while(lst){
     if(lst->type == t && lst->symbol == s &&
	lst->from.stmt == bf && lst->from.refer == lf &&	
	lst->to.stmt == bt && lst->to.refer == lt)
	   return 0;
     lst = lst->from_fwd;
   }
   return 1;
 }

void append_dep(b, d)		/* add the dep d to the list from b */
PTR_BFND b;
PTR_DEP d;
{
  PTR_BFND t;

  d->from_fwd = b->entry.Template.dep_ptr1;
  b->entry.Template.dep_ptr1 = d;
  t = d->to.stmt;
  d->to_fwd = t->entry.Template.dep_ptr2;
  t->entry.Template.dep_ptr2 = d;
}



/**************************************************************/
/* make deps is the key routine that checks two references to */
/* see if they are in fact a dependence.  if so a new dep is  */
/* created and linked into the structure		      */
/**************************************************************/
void make_deps(type, def, use)
PTR_REFL def, use;
int type;
{
  PTR_REFL g;			/* temporary reference list		 */
  PTR_SYMB s;			/* symbol for varialble name		 */
  PTR_SYMB ivar;		/* an induction variable name		 */
  int i, j, befr, notrub, type1;
  int vect[MAX_NEST_DEPTH], troub[MAX_NEST_DEPTH];

  PTR_DEP dptr; 		/* pointer to dependence inserted	 */
  PTR_DEP make_dep();		/* functions from list.c		 */
  char t;			/* type: 0=flow 1=anti 2 = output	 */
  PTR_LLND lls, lld;		/* term source and destination		 */
  PTR_BFND bns, bnd;		/* biff nd source and destination	 */
  char dv[MAX_NEST_DEPTH];	/* dep. vector: 1="="	2="<"  4=">" ?	 */
  while (def != NULL) {
    s = def->id;
    g = use;
    if ((s != NULL) && (s->type != NULL) &&
	((type != INPUTD) || (s->type->variant == T_ARRAY)))
      while (g != NULL) {
	if (g->id == s) {
	  /* compute the distance vector and trouble vector */

	  befr = before(def->node->stmt, g->node->stmt);
	  comp_dist(vect, troub, def->node, g->node, befr);

	  /* first zero out all vector components */
	  /* outside the scope of the variable */

	  /* this is to fix the problem with */
	  /* nested foralls.		   */
	  s = def->id;
	  notrub = 1;
	  for (i = vect[0]; i >= 1; i--) {
	    if (is_forall[i - 1]) {
	      ivar = induct_list[i - 1];
	      while (ivar != NULL && ivar != s)
		ivar = ivar->next_symb;
	      if (ivar == s) {	/* found local */
		notrub = 0;
	      }
	    }
	    if (notrub == 0) {
	      vect[i] = 0;
	      troub[i] = 0;
	    }
	  }


	  if (troub[0] == 1) {
	    /* no dependence here */
	  }
	  else {
	    /* dependence  exists, so generate the record and information */
	    bns = def->node->stmt;
	    lls = def->node->refer;
	    bnd = g->node->stmt;
	    lld = g->node->refer;
	    type1 = type;
	    if(bns == bnd && (lls != lld) && identical(lls, lld)){
		/* this is an accumulation recurrence if lls and lld are */
		/* identical. They should be compared.	if they are the  */
		/* same, create an accumulation dep ACCD.  Check this	 */
		/* for flow and avoid generating the output and anti deps*/ 
		if (type1 == FLOWD) type1 = 5;
		else type1 = 6;
	      }
	    /* convert to standard bif constants */
	    switch (type1) {
	    case 5: /* ACCD: */
	       t = 3;
	       break;
	     case FLOWD:
	      if (show_deps)
		fprintf(stderr, "flow dependence on var:`%s' -", s->ident);
	      t = 0;
	      break;
	     case OUTPUTD:
	     case -OUTPUTD:
	      if (show_deps)
		fprintf(stderr, " output dependence on var:`%s' -", s->ident);
	      t = 2;
	      break;
	     case ANTID:
	      if (show_deps)
		fprintf(stderr, "anti dependence on var:`%s' -", s->ident);
	      t = 1;
	      break;
	     case INPUTD:
	      t = 4;
	      break;
	     default:
	      if (show_deps)
		fprintf(stderr, " bad type -");
	      t = 5;
	    }
	    if(t == 5) break;
	    if (show_deps &&(t != 4))
		fprintf(stderr, "((level=%d)", vect[0]);
	    for (j = 0; j < MAX_NEST_DEPTH; j++)
	      dv[j] = 0;
	    for (j = 1; j <= vect[0]; j++)
	      switch (troub[j]) {
	       case NODEP:
	       case -99:
	       case 0:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", %d ", vect[j]);
		if (vect[j] > 0)
		  dv[j] = 4;
		else if (vect[j] == 0)
		  dv[j] = 1;
		else
		  dv[j] = 2;
		break;
	       case PLUS:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", +");
		dv[j] = 4;
		break;
	       case ZPLUS:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", 0/+");
		dv[j] = 5;
		break;
	       case MINUS:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", -");
		dv[j] = 2;
		break;
	       case ZMINUS:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", 0/-");
		dv[j] = 3;
		break;
	       case PLUSMINUS:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", +/-");
		dv[j] = 7;
		break;
	       default:
		if (show_deps)
		  if (t != 4)
		    fprintf(stderr, ", ??%d ", troub[j]);
		dv[j] = 8;
	      }
	    if (show_deps && (t != 4))
		fprintf(stderr, ")\n");
	    for (j = 1; j <= vect[0]; j++) {
	      if (is_forall[j - 1] && (t != 4)) {
		if (troub[j] == 0 || troub[j] == NODEP
		    || troub[j] == -99) {
		  if (vect[j] != 0)
		    fprintf(stderr, "WARNING!! may be potential concurrency conflict\n");
		}
		else
		  fprintf(stderr, "WARNING!! May be potential Concurrency conflict\n");
	      }
	    }


	    /* now make the dependences...	 */
	    /* only generate uniformly generated input deps. */
	    /* Temp for cftn. disable input deps */
	    /* disabled: note unif_gen has more arguments */
	    if (t != 4 && t != 5 &&
		check_dep_copy(bns,t,s,bns,lls,bnd,lld)){
		   dptr = make_dep(cur_file, s, t, lls, lld, bns, bnd, dv);
		   append_dep(bns, dptr);
	    }

	    /* note: only appends to from list	*/
	    /* if you want more fix append_dep */
	  }
	}
	else {
	  /* symbols do not agree */
	}
	g = g->next;
      }
    def = def->next;
  }
}
/***************************************************************/
/* link_set_list() builds a expr list of low level expressions */
/* that describe the use of variable in the list.  it will list*/
/* each scalar only once and for each array reference it will  */
/* build an expression that describes the use of the variable  */
/* using ddot form. lots of common subexpressions are used.    */
/* find_bounds() is found in anal_ind.c 		       */
/***************************************************************/

PTR_LLND link_set_list(s)
PTR_REFL s;
{
  PTR_LLND p, q, newq, make_llnd(), find_bounds();
  PTR_BFND b;
  PTR_REFL remove_scalar_dups();
  PTR_LLND remove_array_dups(), merge_ll_array_list();

  s = remove_scalar_dups(s);
  p = NULL;
  while (s != NULL) {
    switch (s->node->refer->variant) {
     case VAR_REF:
     case POINTST_OP:
     case RECORD_REF:
      p = make_llnd(cur_file, EXPR_LIST, s->node->refer, p, NULL);
      break;
     case ARRAY_REF:
      q = s->node->refer;
      b = s->node->stmt;
      newq = make_llnd(cur_file, ARRAY_REF,NULL,NULL,q->entry.Template.symbol);
      newq = find_bounds(b, q, newq);
      /* now put q into normal form */
      normal_form(&(newq->entry.Template.ll_ptr1));
      q = newq->entry.Template.ll_ptr1;
      /* now link into expr list chain p */
      p = make_llnd(cur_file, EXPR_LIST, newq, p, NULL);
      break;
     default:
      fprintf(stderr, "something wrong here ");
      break;
    }
    s = s->next;
  }
  return (merge_ll_array_list(merge_ll_array_list(p))); /* two passes */
}

PTR_LLND remove_array_dups(elist)
PTR_LLND elist;
{
  PTR_LLND star_range_list;
  PTR_LLND tmp_list;
  PTR_LLND final_list, cons, item, p, q;
  PTR_SYMB var;
  int not_found;

  /* first pull off all star range arrays from elist and put them */
  /* on the star_range_list.  Others go to tmp_list.  Then tmp_list */
  /* compared to star_range list.  If not there it is added to final */
  /* list and star_range_list is appended to tmp_list.		  */
  star_range_list = NULL;
  tmp_list = NULL;
  final_list = NULL;
  while (elist != NULL) {
    cons = elist;
    elist = elist->entry.Template.ll_ptr2;
    cons->entry.Template.ll_ptr2 = NULL;
    item = cons->entry.Template.ll_ptr1;
    var = item->entry.Template.symbol;
    p = star_range_list;
    q = tmp_list;
    if (item->variant == ARRAY_REF && is_star_range(item)) {
      not_found = 1;
      while (p != NULL) {
	if (var == p->entry.Template.ll_ptr1->entry.Template.symbol) {
	  not_found = 0;
	  break;
	}
	p = p->entry.Template.ll_ptr2;
      }
      if (not_found) {
	cons->entry.Template.ll_ptr2 = star_range_list;
	star_range_list = cons;
      }
    }
    else {
      not_found = 1;
      while (q != NULL) {
	if (identical(q->entry.Template.ll_ptr1, item)) {
	  not_found = 0;
	  break;
	}
	q = q->entry.Template.ll_ptr2;
      }
      if (not_found) {
	cons->entry.Template.ll_ptr2 = tmp_list;
	tmp_list = cons;
      }
    }
  }
  while (tmp_list != NULL) {
    cons = tmp_list;
    tmp_list = tmp_list->entry.Template.ll_ptr2;
    cons->entry.Template.ll_ptr2 = NULL;
    item = cons->entry.Template.ll_ptr1;
    var = item->entry.Template.symbol;
    p = star_range_list;
    if (item->variant == ARRAY_REF) {
      not_found = 1;
      while (p != NULL) {
	if (var == p->entry.Template.ll_ptr1->entry.Template.symbol) {
	  not_found = 0;
	  break;
	}
	p = p->entry.Template.ll_ptr2;
      }
      if (not_found) {
	cons->entry.Template.ll_ptr2 = final_list;
	final_list = cons;
      }
    }
    else {
      cons->entry.Template.ll_ptr2 = final_list;
      final_list = cons;
    }
  }
  q = final_list;
  while (q != NULL && q->entry.Template.ll_ptr2 != NULL)
    q = q->entry.Template.ll_ptr2;
  if (q == NULL)
    final_list = star_range_list;
  else
    q->entry.Template.ll_ptr2 = star_range_list;
  return (final_list);
}
/* buid_recur_expr will try to reduce simple recurrences like */
/* i = i+1 in loop into expressions involving an induction var*/
PTR_LLND build_recur_expr(stmt, s,lls, lld)
PTR_BFND stmt;
PTR_SYMB s;
PTR_LLND lls,lld;
{
   PTR_BFND parent;
   PTR_LLND init_val, index_ref, rhs, new_expr, coef, lb, one;
   PTR_LLND copy_llnd();
  
   parent = stmt->control_parent;
   if(parent->variant == FOR_NODE || parent->variant == CDOALL_NODE){
	  if(stmt->variant == ASSIGN_STAT){
	    init_val = lld->entry.Template.ll_ptr1;
	    lb = copy_llnd(parent->entry.Template.ll_ptr1->entry.Template.ll_ptr1);
	    index_ref = make_llnd(cur_file,VAR_REF,NULL,NULL,
				  parent->entry.Template.symbol);
	    one = make_llnd(cur_file,INT_VAL,NULL,NULL,NULL);
	    one->entry.ival = 0; 
	    lb = make_llnd(cur_file,SUBT_OP,one,lb,NULL);
	    index_ref = make_llnd(cur_file,ADD_OP,index_ref,lb,NULL);
	    rhs = stmt->entry.Template.ll_ptr2;
	    /*
	    printf("index:%s init_val:%s rhs:%s",
		   (UnparseLlnd[cur_file->lang])(index_ref),
		   (UnparseLlnd[cur_file->lang])(init_val),
		   (UnparseLlnd[cur_file->lang])(rhs));
	    */
	    if(rhs->variant == ADD_OP){
		 if(rhs->entry.Template.ll_ptr1 == lld)
		      coef = rhs->entry.Template.ll_ptr2;
		 else if(rhs->entry.Template.ll_ptr2 == lld)
		      coef = rhs->entry.Template.ll_ptr1;
		 else return NULL;
		 new_expr = make_llnd(cur_file,MULT_OP,
				       copy_llnd(coef),index_ref,NULL);
		 new_expr = make_llnd(cur_file,ADD_OP,new_expr,init_val,NULL);
		/*printf("new expr:%s",(UnparseLlnd[cur_file->lang])(new_expr));*/
		 return new_expr;
	       }
	    else if(rhs->variant == SUBT_OP){
		 if(rhs->entry.Template.ll_ptr1 == lld)
		      coef = rhs->entry.Template.ll_ptr2;
		 else return NULL;
		 if(coef == NULL) return NULL;
		 new_expr = make_llnd(cur_file,MULT_OP,
				       copy_llnd(coef),index_ref,NULL);
		 new_expr = make_llnd(cur_file,SUBT_OP,init_val,new_expr,NULL);
		/*printf("new expr:%s",(UnparseLlnd[cur_file->lang])(new_expr));*/
		 return new_expr;
	    }
	    else return NULL;
	  }
	  else return NULL;
	}
   else return NULL;
}
/* propogate will do the scalar propogation.  (test version). */
void propogate(def, use)
PTR_REFL def, use;
{
  PTR_REFL g;			/* temporary reference list		 */
  PTR_SYMB s;			/* symbol for varialble name		 */
  PTR_LLND lls, lld;		/* term source and destination		 */
  PTR_BFND bns; 		/* biff nd source and destination	 */
  PTR_LLND p;

  /* search through each of the definitions */
  while (def != NULL) {
    s = def->id;		/* s is the symbol table entry */
    g = use;
    if ((s != NULL) && (s->type != NULL) &&
	(s->type->variant == T_INT))
      while (g != NULL) {
	if (g->id == s) {
	  lld = g->node->refer;
	  if (def->node->stmt == g->node->stmt) {
	    /* definition is reaching itself where it is used */
	    /* printf("recurrence\n"); */
	    lld = g->node->refer;
	    lls = def->node->refer;
	    if(lld->entry.Template.ll_ptr1 != NULL)
	      lld->entry.Template.ll_ptr1 = build_recur_expr(g->node->stmt,s,lls,lld);
	    else lld->entry.Template.ll_ptr1 = NULL;
	    }
	  else{
	    /* a definition is reaching a different use */
	    bns = def->node->stmt;
	    lld = g->node->refer;
	    lls = def->node->refer;
	    if (bns->variant == FOR_NODE) {
	      lld->entry.Template.ll_ptr1 = NULL;
	    }
	    else if (bns->variant != EXPR_STMT_NODE) {
	      /* a Fortran  assignment, p <- rhs of source */
	      p = bns->entry.Template.ll_ptr2;
	      if (lld->entry.Template.ll_ptr1 == NULL)
		lld->entry.Template.ll_ptr1 = p;
	      else if (lld->entry.Template.ll_ptr1 != p)
		lld->entry.Template.ll_ptr1 = NULL;
	    }
	    else {
	      /* a C EXPR_STMT_NODE */
	      p = bns->entry.Template.ll_ptr1;
	      /* assume it is expr list then asign op */
	      p = p->entry.Template.ll_ptr1;
	      while (p != NULL &&
		     p->entry.Template.ll_ptr1 != lls)
		p = p->entry.Template.ll_ptr2;
	      if (p != NULL)
		p = p->entry.Template.ll_ptr2;
	      if (lld->entry.Template.ll_ptr1 == NULL)
		lld->entry.Template.ll_ptr1 = p;
	      else if (lld->entry.Template.ll_ptr1 != p)
		lld->entry.Template.ll_ptr1 = NULL;
	    }
	  }
	}
	else {
	  /* symbols do not agree */
	}
	g = g->next;
      }
    def = def->next;
  }
}


/***************************************************************/
/* build sets is called four times.Once with pass = 1 and once */
/* with pass = 2.  On the first pass:			       */
/*   1. synthesized attributes: gen and use are passed up tree */
/*   2. the id fields of the biff nodes are renumbered in      */
/*	control flow tree preorder. i.e. lexical order	       */
/* on the second pass:					       */
/*   1. the inherited attributes are propogated down the tree  */
/*   2. dependence arcs are generated.			       */
/* the variable rnd is used to destinguish between using info  */
/* from a global analysis sweep and ignoring the effect of     */
/* function calls.					       */
/***************************************************************/
PTR_SETS build_sets(int rnd, PTR_BFND b, PTR_REFL in_use, PTR_REFL in_def,int pass)
/*int rnd;*/			/* rnd = 0 first time and rnd = 1 after
				 * global analysis */
/*PTR_BFND b;*/
/*PTR_REFL in_use, in_def;*/
/*int pass;*/
{
  PTR_BLOB bl;
  PTR_SETS s;
  PTR_REFL gen, use, out_use, out_def, detmp;
  PTR_REFL out_useT, out_useF, out_defT, out_defF;
  PTR_REFL remove_locals();
  PTR_LLND link_set_list();
  PTR_REFL tmp1, tmp2, tmp3;

  if (b == NULL)
    fprintf(stderr, "null bfnd!!\n");

  if (b != NULL)
    switch (b->variant) {

     case GLOBAL:
      node_count = 0;
      bl = b->entry.Template.bl_ptr1;
      b->id = node_count++;
      while ((bl != NULL) && (bl->ref != b)) {
	if ((bl->ref->variant == PROG_HEDR) ||
	    (bl->ref->variant == FUNC_HEDR) ||
	    (bl->ref->variant == PROC_HEDR))
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	bl = bl->next;
      }
      break;

     case PROG_HEDR:
      /* PASS 1 ---------------------- */
      /* visit each child		 */
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->out_use = NULL;
	b->entry.Template.sets->in_use = NULL;
	b->entry.Template.sets->out_def = NULL;
	b->entry.Template.sets->in_def = NULL;
	b->entry.Template.sets->gen = NULL;
	b->entry.Template.sets->use = NULL;
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	  bl = bl->next;
	}
	return (b->entry.Template.sets);
      }
      else {
	PTR_REFL t1, t2;
	/* PASS 2 ---------------------- */
	in_use = NULL;
	out_def = NULL;
	out_use = NULL;
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_use, out_def, pass);
	  out_use = s->out_use;
	  out_def = s->out_def;
	  bl = bl->next;
	}
	/* at this point intersect out_use and */
	/* out_def with the global and commons */
	/* and set to out_use and out_def      */
	t1 = intersect_refl(b->entry.Template.sets->in_def, out_def);
	t2 = remove_locals_from_list(out_def);
	b->entry.Template.sets->out_def = union_refl(t1, t2);
	disp_refl(t1);
	disp_refl(t2);
	t1 = intersect_refl(b->entry.Template.sets->in_def, out_use);
	t2 = remove_locals_from_list(out_use);
	b->entry.Template.sets->out_use = union_refl(t1, t2);
	disp_refl(t1);
	disp_refl(t2);
	if (rnd == 0) {
	  fprintf(stderr, "%%program %s --\n",
		 b->entry.procedure.proc_symb->ident);
	  fprintf(stderr, "%s\n",
		 b->entry.procedure.proc_symb->ident);
	  fprintf(stderr, ">>L %d \n", b->g_line);
	  fprintf(stderr, "%%defines variables\n");
	  b->entry.Template.ll_ptr2 =
	    make_llnd(cur_file, EXPR_LIST, NULL, NULL, NULL);
	  b->entry.Template.ll_ptr2->entry.Template.ll_ptr1 =
	    link_set_list(b->entry.Template.sets->out_def);
	  b->entry.Template.ll_ptr3 =
	    make_llnd(cur_file, EXPR_LIST, NULL, NULL, NULL);
	  b->entry.Template.ll_ptr3->entry.Template.ll_ptr1 =
	    link_set_list(b->entry.Template.sets->out_use);
	  fprintf(stderr, "%s",
		  (UnparseLlnd[cur_file->lang])(b->entry.Template.ll_ptr2));
	  fprintf(stderr, "%% and uses\n");
	  fprintf(stderr, "%s\n",
		  (UnparseLlnd[cur_file->lang])(b->entry.Template.ll_ptr3));
	  fprintf(stderr, "\n");
	}
	return (b->entry.Template.sets);
      }

     case PROC_HEDR:
     case FUNC_HEDR:
      /* PASS 1 ---------------------- */
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->out_use = NULL;
	b->entry.Template.sets->in_use = NULL;
	b->entry.Template.sets->out_def = NULL;
	b->entry.Template.sets->in_def = NULL;
	b->entry.Template.sets->gen = NULL;
	b->entry.Template.sets->use = NULL;
	/* set in_def  to be a ref list of all */
	/* parameters to this proc.  this is	 */
	/* appended with commons and then it is */
	/* interesected with the real ref and  */
	/* use list in pass 2.		 */
	b->entry.Template.sets->in_def =
	  make_name_list(b->entry.Template.symbol->entry.proc_decl.in_list);
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	  bl = bl->next;
	}
	return (b->entry.Template.sets);
      }
      else {
	PTR_REFL t1, t2;

	/* PASS 2 ---------------------- */
	/* visit each child		 */
	/* in_def = in_params; in_use = {}; out_def = in_def; out_use = {};
	 * for each child do pass out_use and out_def; visit child; out_use =
	 * child.out_use; out_def = child.out_def; end; */
	in_use = NULL;
	out_def = NULL;
	out_use = NULL;
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_use, out_def, pass);
	  out_use = s->out_use;
	  out_def = s->out_def;
	  bl = bl->next;
	}
	/* interest out_use and out_def with the */
	/* parameters and common statements	    */
	t1 = intersect_refl(b->entry.Template.sets->in_def, out_def);
	t2 = remove_locals_from_list(out_def);
	b->entry.Template.sets->out_def = union_refl(t1, t2);
	disp_refl(t1);
	disp_refl(t2);
	t1 = intersect_refl(b->entry.Template.sets->in_def, out_use);
	t2 = remove_locals_from_list(out_use);
	b->entry.Template.sets->out_use = union_refl(t1, t2);
	disp_refl(t1);
	disp_refl(t2);
	t1 = b->entry.Template.sets->out_def;
	t2 = b->entry.Template.sets->out_use;
	if (rnd == 0) {
	  b->entry.Template.ll_ptr2 =
	    make_llnd(cur_file, EXPR_LIST, NULL, NULL, NULL);
	  b->entry.Template.ll_ptr2->entry.Template.ll_ptr1 =
	    link_set_list(t1);
	  b->entry.Template.ll_ptr3 =
	    make_llnd(cur_file, EXPR_LIST, NULL, NULL, NULL);
	  b->entry.Template.ll_ptr3->entry.Template.ll_ptr1 =
	    link_set_list(t2);
	  fprintf(stderr, "%%procedure %s-\n",
		 b->entry.procedure.proc_symb->ident);
	  fprintf(stderr, "%s", (UnparseBfnd[cur_file->lang])(b));
	  fprintf(stderr, ">>L %d \n", b->g_line);
	  fprintf(stderr, "%%which defines values for-\n");
	  fprintf(stderr, "%s",
		  (UnparseLlnd[cur_file->lang])(b->entry.Template.ll_ptr2));
	  fprintf(stderr, "\n%%and uses values-\n");
	  fprintf(stderr, "%s\n",
		  (UnparseLlnd[cur_file->lang])(b->entry.Template.ll_ptr3));
	}
	return (b->entry.Template.sets);
      }
     case COMM_STAT:
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->gen = NULL;
	b->entry.Template.sets->use = NULL;
	/* now gather up all the varaibles and */
	/* link them in to the parent node.	  */
	/* not done yet.			  */
	detmp = NULL;
	tmp1 = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr1);
	tmp2 = b->control_parent->entry.Template.sets->in_def;
	while ((tmp2 != NULL) && (tmp2->next != NULL))
	  tmp2 = tmp2->next;
	if (tmp2 == NULL)
	  b->control_parent->entry.Template.sets->in_def = tmp1;
	else
	  tmp2->next = tmp1;
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ----------------------- */
	/* just pass everything through! */
	b->entry.Template.sets->out_def = in_def;
	b->entry.Template.sets->out_use = in_use;
	return (b->entry.Template.sets);
      }
     case EXPR_STMT_NODE:
      /* PASS 1 ----------------------- */
      /* make synth. attribs gen, use */
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	if (b->entry.Template.sets->gen == NULL) {
	  detmp = NULL;
	  tmp1 = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr1);
	  /* we only want the first.  the others are uses */
	  b->entry.Template.sets->gen = detmp;
	  b->entry.Template.sets->use = tmp1;
	}
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ----------------------- */
	b->entry.Template.sets->in_use = copy_refl(in_use);
	b->entry.Template.sets->in_def = copy_refl(in_def);

	/* set local kill = { X in in_def | ref(X) in gen } */
	out_def = rem_kill(in_def, b->entry.Template.sets->gen);

	assign(&out_def,union_refl(out_def, b->entry.Template.sets->gen));
	b->entry.Template.sets->out_def = out_def;

	/* out_use = in_use + use */
	b->entry.Template.sets->out_use =
	  union_refl(in_use, b->entry.Template.sets->use);
	propogate(in_def, b->entry.Template.sets->use);
	return (b->entry.Template.sets);
      }
     case ASSIGN_STAT:
     case M_ASSIGN_STAT:
     case SUM_ACC:
     case MULT_ACC:
     case MAX_ACC:
     case MIN_ACC:
     case CAT_ACC:
     case OR_ACC:
     case AND_ACC:
     case READ_STAT:
     case WRITE_STAT:
     case PROC_STAT:
      /* PASS 1 ----------------------- */
      /* make synth. attribs gen, use */
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	if (b->entry.Template.sets->gen == NULL) {
	  detmp = NULL;
	  tmp1 = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr1);
	  if (b->variant == PROC_STAT) {
	    b->entry.Template.sets->gen = detmp;
	    b->entry.Template.sets->use = tmp1;
	    return (b->entry.Template.sets);
	  }
	  /* we only want the first.  the others are uses */
	  if (tmp1 == NULL) {
	    tmp2 = NULL;
	    b->entry.Template.sets->gen = NULL;
	  }
	  else {
	    tmp2 = tmp1->next;
	    tmp1->next = NULL;
	    b->entry.Template.sets->gen = tmp1;
	  }
	}
	else
	  tmp2 = NULL;
	if (b->entry.Template.sets->use == NULL) {
	  detmp = NULL;
	  tmp1 = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr2);
	  if (tmp2 != NULL) {
	    tmp3 = union_refl(tmp1, tmp2);
	    disp_refl(tmp1);
	    disp_refl(tmp2);
	  }
	  else
	    tmp3 = tmp1;
	  b->entry.Template.sets->use = tmp3;
	}
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ----------------------- */
	b->entry.Template.sets->in_use = copy_refl(in_use);
	b->entry.Template.sets->in_def = copy_refl(in_def);

	/* set local kill = { X in in_def | ref(X) in gen } */
	out_def = rem_kill(in_def, b->entry.Template.sets->gen);

	/* create synth. attrib. out_def = in_def - kill + gen */
	assign(&out_def,
	       union_refl(out_def, b->entry.Template.sets->gen)
	  );
	b->entry.Template.sets->out_def = out_def;

	/* out_use = in_use + use */
	b->entry.Template.sets->out_use =
	  union_refl(in_use, b->entry.Template.sets->use);

	propogate(in_def, b->entry.Template.sets->use);
	return (b->entry.Template.sets);
      }

     case LOOP_NODE:
     case FOR_NODE:
     case WHILE_NODE:
      /* PASS 1 ---------------------- */
      /* for each child collect gen and use */
      if (pass == 1) {
	b->id = node_count++;
	use = NULL;
	gen = NULL;
	detmp = NULL;
	if (b->entry.Template.symbol == NULL) { /* this is  a C loop */
	  use = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr1);
	  gen = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr2);
	  assign(&use, union_refl(use, gen));
	  gen = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr3);
	  assign(&use, union_refl(use, gen));
	  assign(&gen, detmp);
	}
	else
	  use = gather_refl(rnd, &detmp, b, b->entry.for_node.range);
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	  assign(&use, union_refl(use, s->use));
	  gen = rem_kill(gen, s->gen);	/* try to fix propogation prob */
	  assign(&gen, union_refl(gen, s->gen));
	  bl = bl->next;
	}
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->out_use = NULL;
	b->entry.Template.sets->in_use = NULL;
	b->entry.Template.sets->out_def = NULL;
	b->entry.Template.sets->in_def = NULL;
	b->entry.Template.sets->gen = remove_locals(b, gen);
	b->entry.Template.sets->use = remove_locals(b, use);
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ---------------------- */
	s = b->entry.Template.sets;
	b->entry.Template.sets->in_use = copy_refl(in_use);
	b->entry.Template.sets->out_def = copy_refl(in_def);
	/* first take care of range varible propogation. */
	detmp = NULL;
	if (b->entry.Template.symbol == NULL) { /* this is  a C loop */
	  use = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr1);
	  gen = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr2);
	  assign(&use, union_refl(use, gen));
	  gen = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr3);
	  assign(&use, union_refl(use, gen));
	  gen = detmp;
	}
	else
	  use = gather_refl(rnd, &detmp, b, b->entry.for_node.range);
	propogate(in_def, use);
	/* now	take care of children */
	out_use = union_refl(in_use, s->use);
	out_def = union_refl(in_def, s->gen);
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_use, out_def, pass);
	  assign(&out_use, copy_refl(s->out_use));
	  assign(&out_def, copy_refl(s->out_def));
	  bl = bl->next;
	}
	b->entry.Template.sets->out_use = out_use;
	b->entry.Template.sets->out_def = out_def;
	return (b->entry.Template.sets);
      }
     case PARFOR_NODE:
     case CDOALL_NODE:
      /* PASS 1 ----------------------		 */
      /* for each child collect gen and use		 */
      if (pass == 1) {
	b->id = node_count++;
	use = NULL;
	gen = NULL;
	detmp = NULL;
	if (b->variant == PARFOR_NODE) {
	  use = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr2);
	  bl = b->entry.Template.bl_ptr1;
	}
	else {
	  use = gather_refl(rnd, &detmp, b, b->entry.for_node.range);
	  bl = b->entry.Template.bl_ptr2;
	}
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	  assign(&use, union_refl(use, s->use));
	  assign(&gen, union_refl(gen, s->gen));
	  bl = bl->next;
	}
	if (b->variant == CDOALL_NODE &&
	    b->entry.Template.bl_ptr1 != NULL) {
	  bl = b->entry.Template.bl_ptr1;
	  while ((bl != NULL) && (bl->ref != b)) {
	    s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	    assign(&use, union_refl(use, s->use));
	    assign(&gen, union_refl(gen, s->gen));
	    bl = bl->next;
	  }
	}
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->out_use = NULL;
	b->entry.Template.sets->in_use = NULL;
	b->entry.Template.sets->out_def = NULL;
	b->entry.Template.sets->in_def = NULL;
	/* here is difference with other loops	 */
	/* locals must be deleted from gen and use	 */
	b->entry.Template.sets->gen = remove_locals(b, gen);
	b->entry.Template.sets->use = remove_locals(b, use);
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ---------------------- */
	s = b->entry.Template.sets;
	b->entry.Template.sets->in_use = copy_refl(in_use);
	b->entry.Template.sets->in_def = copy_refl(in_def);
	detmp = NULL;
	if (b->variant == PARFOR_NODE) {
	  use = gather_refl(rnd, &detmp, b, b->entry.Template.ll_ptr2);
	  bl = b->entry.Template.bl_ptr1;
	}
	else {
	  use = gather_refl(rnd, &detmp, b, b->entry.for_node.range);
	  bl = b->entry.Template.bl_ptr2;
	}
	out_use = union_refl(in_use, s->use);
	out_def = union_refl(in_def, s->gen);
	propogate(in_def, use);
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_use, out_def, pass);
	  assign(&out_use, copy_refl(s->out_use));
	  assign(&out_def, copy_refl(s->out_def));
	  bl = bl->next;
	}
	if (b->variant == CDOALL_NODE &&
	    b->entry.Template.bl_ptr1 != NULL) {
	  bl = b->entry.Template.bl_ptr1;
	  while ((bl != NULL) && (bl->ref != b)) {
	    s = build_sets(rnd, bl->ref, out_use, out_def, pass);
	    assign(&out_use, copy_refl(s->out_use));
	    assign(&out_def, copy_refl(s->out_def));
	    bl = bl->next;
	  }
	}
	b->entry.Template.sets->out_use = out_use;
	b->entry.Template.sets->out_def = out_def;
	return (b->entry.Template.sets);
      }
     case LOGIF_NODE:
     case ELSEIF_NODE:
     case IF_NODE:
      /* PASS 1 ---------------------- */
      /* for each child collect gen and use */
      if (pass == 1) {
	b->id = node_count++;
	use = NULL;
	gen = NULL;
	use = gather_refl(rnd, &gen, b, b->entry.Template.ll_ptr1);
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	  assign(&use, union_refl(use, s->use));
	  assign(&gen, union_refl(gen, s->gen));
	  bl = bl->next;
	}
	if (b->variant != LOGIF_NODE) {
	  bl = b->entry.Template.bl_ptr2;
	  while ((bl != NULL) && (bl->ref != b)) {
	    s = build_sets(rnd, bl->ref, NULL, NULL, pass);
	    assign(&use, union_refl(use, s->use));
	    assign(&gen, union_refl(gen, s->gen));
	    bl = bl->next;
	  }
	}
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->out_use = NULL;
	b->entry.Template.sets->in_use = NULL;
	b->entry.Template.sets->out_def = NULL;
	b->entry.Template.sets->in_def = NULL;
	b->entry.Template.sets->gen = gen;
	b->entry.Template.sets->use = use;
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ------------------------------------------------ */
	/* for each branch do					  */
	/* out_use = in_use; out_def_branch = in_def;	  */
	/* for each child do			  */
	/* pass out_use and out_def_branch;  */
	/* visit child			  */
	/* out_use = child.out_use;	  */
	/* out_def_branch = child.out_def;   */
	/* end; 				  */
	/* out_def = out_def_lbranch+out_def_rbranch */
	/* ________________________________________________________ */
	out_defT = in_def;
	out_useT = in_use;
	/* visit True children */
	b->entry.Template.sets->in_use =
	  copy_refl(in_use);
	b->entry.Template.sets->in_def =
	  copy_refl(in_def);
	bl = b->entry.Template.bl_ptr1;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_useT, out_defT, pass);
	  out_useT = s->out_use;
	  out_defT = s->out_def;
	  bl = bl->next;
	}
	out_defF = in_def;
	out_useF = in_use;
	/* visit False children */
	bl = b->entry.Template.bl_ptr2;
	while ((bl != NULL) && (bl->ref != b)) {
	  s = build_sets(rnd, bl->ref, out_useF, out_defF, pass);
	  out_useF = s->out_use;
	  out_defF = s->out_def;
	  bl = bl->next;
	}
	gen = NULL;
	use = gather_refl(rnd, &gen, b, b->entry.Template.ll_ptr1);
	assign(&use, union_refl(out_useF, use));
	assign(&gen, union_refl(out_defF, gen));
	b->entry.Template.sets->out_use =
	  union_refl(use, out_useT);
	b->entry.Template.sets->out_def =
	  union_refl(gen, out_defT);

	return (b->entry.Template.sets);
      }
     case EXIT_NODE:
      fprintf(stderr, "exit node found! no dep ananysis!\n");

     default:			/* assume a no op */
      if (pass == 1) {
	b->id = node_count++;
	if (b->entry.Template.sets == NULL)
	  b->entry.Template.sets = alloc_sets();
	b->entry.Template.sets->gen = NULL;
	b->entry.Template.sets->use = NULL;
	return (b->entry.Template.sets);
      }
      else {
	/* PASS 2 ----------------------- */
	/* just pass everything through! */
	b->entry.Template.sets->out_def = in_def;
	b->entry.Template.sets->out_use = in_use;
	return (b->entry.Template.sets);
      }
    }
  return (NULL);
}

void gendeps(b)
PTR_BFND b;
{
  PTR_BLOB bl;

  if (b != NULL)
    switch (b->variant) {

     case GLOBAL:
      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	bl = bl->next;
      }
      break;

     case PROG_HEDR:
      /* visit each child */
      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	bl = bl->next;
      }
      break;
     case PROC_HEDR:
     case FUNC_HEDR:
      /* visit each child */
      if (show_deps)
	fprintf(stderr, "---------Procedure %s------------------\n",
	       b->entry.procedure.proc_symb->ident);
      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	if (num_ll_allocated > 10000)
	  collect_garbage(cur_file);
	bl = bl->next;
      }
      break;
     case EXPR_STMT_NODE:
     case ASSIGN_STAT:
     case M_ASSIGN_STAT:
     case SUM_ACC:
     case MULT_ACC:
     case MAX_ACC:
     case MIN_ACC:
     case CAT_ACC:
     case OR_ACC:
     case AND_ACC:
     case READ_STAT:
     case WRITE_STAT:
     case PROC_STAT:
      if (num_ll_allocated > 10000)
	collect_garbage(cur_file);
      if (show_deps)
	fprintf(stderr, "----- line %d \n", b->g_line);
      make_deps(FLOWD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->use);

      make_deps(OUTPUTD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->gen);

      make_deps(ANTID, b->entry.Template.sets->in_use,
		b->entry.Template.sets->gen);

      make_deps(INPUTD, b->entry.Template.sets->in_use,
		b->entry.Template.sets->use);

      break;

     case LOOP_NODE:
     case FOR_NODE:
     case WHILE_NODE:
      if (show_deps)
	fprintf(stderr, "----- line %d \n", b->g_line);
      make_deps(FLOWD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->use);

      make_deps(OUTPUTD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->gen);

      make_deps(ANTID, b->entry.Template.sets->in_use,
		b->entry.Template.sets->gen);

      make_deps(INPUTD, b->entry.Template.sets->in_use,
		b->entry.Template.sets->use);

      if (num_ll_allocated > 10000)
	collect_garbage(cur_file);
      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	if (num_ll_allocated > 10000)
	  collect_garbage(cur_file);
	bl = bl->next;
      }
      break;
     case FORALL_NODE:
     case CDOALL_NODE:
     case PARFOR_NODE:
      if (show_deps)
	fprintf(stderr, "----- line %d \n", b->g_line);
      make_deps(FLOWD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->use);

      make_deps(OUTPUTD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->gen);

      make_deps(ANTID, b->entry.Template.sets->in_use,
		b->entry.Template.sets->gen);

      make_deps(INPUTD, b->entry.Template.sets->in_use,
		b->entry.Template.sets->use);

      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	bl = bl->next;
      }
      break;
     case LOGIF_NODE:
     case IF_NODE:
      if (show_deps)
	fprintf(stderr, "----- line %d \n", b->g_line);
      make_deps(FLOWD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->use);

      make_deps(OUTPUTD, b->entry.Template.sets->in_def,
		b->entry.Template.sets->gen);

      make_deps(ANTID, b->entry.Template.sets->in_use,
		b->entry.Template.sets->gen);

      make_deps(INPUTD, b->entry.Template.sets->in_use,
		b->entry.Template.sets->use);

      /* visit True children */
      bl = b->entry.Template.bl_ptr1;
      while ((bl != NULL) && (bl->ref != b)) {
	gendeps(bl->ref);
	if (num_ll_allocated > 10000)
	  collect_garbage(cur_file);
	bl = bl->next;
      }
      /* visit False children */
      if (b->variant != LOGIF_NODE) {
	bl = b->entry.Template.bl_ptr2;
	while ((bl != NULL) && (bl->ref != b)) {
	  gendeps(bl->ref);
	  if (num_ll_allocated > 10000)
	    collect_garbage(cur_file);
	  bl = bl->next;
	}
      }
      break;
     case EXIT_NODE:
      fprintf(stderr, "exit node found! no dep ananysis!\n");
      break;
     default:			/* assume a no op */
      /* just pass everything through! */
      break;
    }
}

void relink(fi)
PTR_FILE fi;
{
  PTR_BFND bf_ptr;
  int count = 1;

  for (bf_ptr = fi->head_bfnd; bf_ptr != NULL; bf_ptr = bf_ptr->thread)
    bf_ptr->id = count++;
}
