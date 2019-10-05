/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: setutils.c */
#include <stdlib.h>
#include "db.h"

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

extern PCF UnparseBfnd[];
extern PCF UnparseLlnd[];

PTR_SYMB induct_list[MAX_NEST_DEPTH];
int stride[MAX_NEST_DEPTH];
int is_forall[MAX_NEST_DEPTH];

/* variable default value structure. */
struct dflts {
  PTR_SYMB name;
  int value;
  struct dflts *next;
};

typedef struct dflts *PTR_DFLT;
PTR_DFLT glob_dflts = NULL;
PTR_SETS free_sets = NULL;
PTR_REFL free_refl = NULL;
PTR_DEP free_dep = NULL;
/*char *malloc();*/

extern PTR_FILE cur_file;
extern int language;

/* Forward declarations */
int is_not_loc();
void disp_refl();
int make_range();
void disp_refl();
int make_induct_list();

extern int identical();
extern int integer_difference();

int get_dflt(df, s)
int *df;
PTR_SYMB s;
{
  PTR_DFLT p;
  int v;

  p = glob_dflts;
  *df = 1;
  while (p != NULL) {
    if (p->name == s)
      return (p->value);
    p = p->next;
  }
  p = (PTR_DFLT) malloc(sizeof(struct dflts));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,p, 0);
#endif
  p->next = glob_dflts;
  glob_dflts = p;
  p->name = s;
  *df = 1;
  v = 100;
  p->value = v;
  return (v);
}

PTR_SETS alloc_sets()
{
  PTR_SETS s;

  s = (PTR_SETS) malloc(sizeof(struct sets));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,s, 0);
#endif
  if (s == NULL)
    fprintf(stderr, "! out of space for sets!!\n");
  s->use = NULL;
  s->gen = NULL;
  s->in_use = NULL;
  s->in_def = NULL;
  s->out_use = NULL;
  s->out_def = NULL;
  s->arefl = NULL;
  return (s);
}

/*********************************************************************/
/* is_not_local() is used to find out if a reference is to a global  */
/* variable.  The way it works is that it traverses the biffnd tree  */
/* up to the level of a procedure or function checking for local     */
/* declarations.  It understands the static scoping of C.	     */
/*********************************************************************/
static int search_for_dec(b, s)
PTR_BFND b;
PTR_SYMB s;
{
  PTR_BFND par;
  PTR_BLOB p;
  PTR_LLND ll, def;

  par = b->control_parent;
  p = par->entry.Template.bl_ptr1;
  while (p != NULL && p->ref != b) {
    switch (p->ref->variant) {
     case VAR_DECL:
     case STRUCT_DECL:
      ll = p->ref->entry.Template.ll_ptr1;
      while (ll != NULL) {
	def = ll->entry.Template.ll_ptr1;
	while (def != NULL && def->variant == DEREF_OP)
	  def = def->entry.Template.ll_ptr1;

	if ((def != NULL) &&
	    (def->variant == VAR_REF || def->variant == ARRAY_REF)
	    && (s == def->entry.Template.symbol))
	  return (0);
	ll = ll->entry.Template.ll_ptr2;
      }
      break;
     default:
      break;
    }
    p = p->next;
  }
  if (par->variant == GLOBAL || par->variant == FUNC_HEDR)
    return (1);
  else
    return (search_for_dec(par, s));
}

int non_exec_statement(fBF)
PTR_BFND fBF;
{
  switch (fBF->variant) {
   case PROS_COMM:
   case COMM_STAT:
   case EXTERN_STAT:
   case INTRIN_STAT:
   case EQUI_STAT:
   case STMTFN_STAT:
   case ATTR_DECL:
   case DIM_STAT:
   case VAR_DECL:
   case PARAM_DECL:
   case IMPL_DECL:
   case DATA_DECL:
   case SAVE_DECL:
   case BLOCK_DATA:
   case COMMENT_STAT:
   case ENTRY_STAT:
   case CONTROL_END:
    return (1);
   default:
    return (0);
  }
}

int search_for_common_decl(b, s)
PTR_BFND b;
PTR_SYMB s;
{
  PTR_BFND par;
  PTR_BLOB p;
  PTR_LLND ll, def;

  par = b;
  while (par != NULL && par->variant != PROG_HEDR &&
	 par->variant != PROC_HEDR &&
	 par->variant != FUNC_HEDR)
    par = par->control_parent;
  if (par == NULL)
    return (0);

  p = par->entry.Template.bl_ptr1;
  while (p != NULL && non_exec_statement(p->ref)) {
    if (p->ref->variant == COMM_STAT) {
      ll = p->ref->entry.Template.ll_ptr1;	/* COMM_LIST */
      ll = ll->entry.Template.ll_ptr1;	/* EXPR_LIST */
      while (ll != NULL) {
	def = ll->entry.Template.ll_ptr1;
	if ((def != NULL) &&
	    (def->variant == VAR_REF || def->variant == ARRAY_REF) &&
	    (s == def->entry.Template.symbol))
	  return (1);
	ll = ll->entry.Template.ll_ptr2;
      }
    }
    p = p->next;
  }
  return (0);
}

int is_not_local(r)
struct ref *r;
{
  PTR_BFND b;
  PTR_LLND ll;

  b = r->stmt;
  ll = r->refer;
  return (is_not_loc(b, ll));
}

int is_not_loc(b, ll)
PTR_BFND b;
PTR_LLND ll;
{
  PTR_BFND curfun;
  PTR_SYMB s, params;
  PTR_LLND q;
  int i;

  curfun = b;
  while (curfun != NULL && curfun->variant != GLOBAL &&
	 curfun->variant != FUNC_HEDR && curfun->variant != PROC_HEDR)
    curfun = curfun->control_parent;
  if (curfun->variant == FUNC_HEDR || curfun->variant == PROC_HEDR) {
    params = curfun->entry.Template.symbol;
    params = params->entry.proc_decl.in_list;
  }
  else
    params = NULL;

  switch (ll->variant) {
   case VAR_REF:
   case ARRAY_REF:
    s = ll->entry.Template.symbol;
    break;
   case POINTST_OP:
    q = ll;
    while (q != NULL && q->variant != VAR_REF)
      q = q->entry.Template.ll_ptr1;
    if (q == NULL)
      return (1);
    else {
      s = q->entry.Template.symbol;
    }
    break;
   default:
    s = NULL;
    break;
  }
  while (s != NULL && params != NULL) {
    if (params == s)
      return (1);
    params = params->entry.var_decl.next_in;
  }
  if (language == ForSrc) {
    if (search_for_common_decl(b, s))
      return (1);
    if (s->attr == 1)
      return (1);		/* attribute is global */
    return (0);
  }
  if (s != NULL) {
    if ((i = search_for_dec(b, s)) == 0) {
    }
    else {
    }
    return (i);
  }
  else {
    return (1);
  }
}

PTR_REFL remove_locals_from_list(rl)
PTR_REFL rl;
{
  PTR_REFL t, local, global;

  local = NULL;
  global = NULL;
  while (rl != NULL) {
    if (is_not_local(rl->node)) {
      t = rl;
      rl = rl->next;
      t->next = global;
      global = t;
    }
    else {
      t = rl;
      rl = rl->next;
      t->next = local;
      local = t;
    }
  }
  disp_refl(local);
  return (global);
}

int subsumed(p, q)
PTR_LLND p,q;
{
   PTR_LLND pind[10], qind[10], newpind[10], t;
   int pdim, qdim, i, same, not_same[10], k,ns ;

  if (p->variant != ARRAY_REF)
    return (0);
  if (q->variant != ARRAY_REF)
    return (0);
  if (p->entry.Template.symbol != q->entry.Template.symbol)
    return (0);
  
  pdim = 0;
  t = p->entry.Template.ll_ptr1;
  while(t && (t->variant == EXPR_LIST) && pdim < 10){
      pind[pdim++] = t;
      t = t->entry.Template.ll_ptr2;
      /* printf("pind[%d] = %s",pdim-1,(UnparseLlnd[cur_file->lang])(pind[pdim-1]));*/
      }
  qdim = 0;
  t = q->entry.Template.ll_ptr1;
  while(t && (t->variant == EXPR_LIST) && qdim < 10){
      qind[qdim++] = t;
      t = t->entry.Template.ll_ptr2;
     /*  printf("qind[%d] = %s",qdim-1,(UnparseLlnd[cur_file->lang])(qind[qdim-1]));*/
      }

  if(pdim != qdim) return 0;
  if(pdim == 0) return 1;

  ns = 0;
  for(i = 0; i < pdim; i++){
     same = identical(pind[i]->entry.Template.ll_ptr1,
                      qind[i]->entry.Template.ll_ptr1);
     if (same == 0){ ns = 1; not_same[i] = 1;} 
     else not_same[i] = 0;
     }
 
  if(ns == 0) return 1;
  /* if(not_same > 1) return 0; */

  for(k = 0; k < pdim; k++)
     if(not_same[k] &&
        (make_range(pind[k]->entry.Template.ll_ptr1, 
                    qind[k]->entry.Template.ll_ptr1, &(newpind[k])) == 0)) return 0;

  for(k = 0; k < pdim; k++)
    if(not_same[k]){
       if( k == 0)
          p->entry.Template.ll_ptr1->entry.Template.ll_ptr1 = newpind[k];
       else
          pind[k]->entry.Template.ll_ptr1 = newpind[k];
     }
  return 1;
}    
     
int make_range(p,q, newp)
PTR_LLND p,q, *newp;
{
  PTR_LLND plow, phi, qlow, qhi, newlow, newhi,d1,d2;
  PTR_LLND make_llnd();
  int diff, pconst, qconst;

  if(p == NULL) {*newp = NULL; return 1;}
  if(q == NULL) {*newp = NULL; return 1;}
  if(p->variant == STAR_RANGE){ *newp = p; return 1; }
  if(q->variant == STAR_RANGE){ *newp = q; return 1; }

  pconst = qconst = 0;
  if(p->variant == DDOT){ 
       plow = p->entry.Template.ll_ptr1;
       phi  = p->entry.Template.ll_ptr2;
       if(plow == NULL || phi == NULL){
	 *newp = make_llnd(cur_file, STAR_RANGE, NULL, NULL);
	 return 1;
       }
       if(phi->variant == DDOT) phi = p->entry.Template.ll_ptr1;
       }
  else {plow = phi = p; pconst = 1;}
  if(q->variant == DDOT){ 
       qlow = q->entry.Template.ll_ptr1;
       qhi  = q->entry.Template.ll_ptr2;
       if(qlow == NULL || qhi == NULL){
	 *newp = make_llnd(cur_file, STAR_RANGE, NULL, NULL);
	 return 1;
       }
       if(qhi->variant == DDOT) qhi = q->entry.Template.ll_ptr1;
       }
  else {qlow = qhi = q; qconst = 1;}
  if(pconst && qconst == 0){
    if(integer_difference(p,qlow, &diff, &d1) && (diff >= -1)){
          if(diff == 1 || diff == 0){
               /* we have qlow < p ? qhi.  we need to know the range of qhi */
               *newp = q; 
               return 1;
	     }
          else if (diff == -1){
             /* we hve p = qlow-1 < qhi o  */
             *newp = make_llnd(cur_file, DDOT, p, qhi, NULL);
             return 1;
	   }
	}
    if(integer_difference(p,qhi, &diff, &d1) && (diff <= 1)){
          if(diff == -1 || diff == 0){ 
            /* we have qlow  < qhi = p+1 */
              *newp = q; 
               return 1;
	     }
          else if(diff == 1){
             /* we hve qlow < qhi = p-1 < p   */
             *newp = make_llnd(cur_file, DDOT, qlow, p, NULL);
             return 1;
	   }
 	}
    return 0;
  }
  if(pconst == 0 && qconst){
    if(integer_difference(plow,q, &diff, &d1) && (diff <= 1)){
          if(diff == -1 || diff == 0){
               /* we have plow < q ? phi.  we need to know the range of phi */
               *newp = p; 
               return 1;
	     }
          else if(diff == 1){
             /* we hve q = plow-1<plow < phi */
             *newp = make_llnd(cur_file, DDOT, q, phi, NULL);
             return 1;
	   }
      	}
    if(integer_difference(phi,q, &diff, &d1) && (diff >= -1)){
          if(diff == 1 || diff == 0){ 
            /* we have qlow ? p < qhi */
              *newp = p; 
               return 1;
	     }
          else if(diff == -1){
             /* we hve plow < phi = q-1<q  */
             *newp = make_llnd(cur_file, DDOT, plow, q, NULL);
             return 1;
	   }
	}
    return 0;
  }
  if(pconst && qconst){
    if(integer_difference(p,q,&diff,&d1)){
       if (diff == 1){
           *newp = make_llnd(cur_file, DDOT, q,p,NULL);
           return 1;
 }
       else if (diff ==1){
           *newp = make_llnd(cur_file, DDOT, p,q,NULL);
           return 1;
	 }
       else return 0;
     }
  }
  if(integer_difference(plow,qlow,&diff, &d1) == 0){
       /* printf("lo diff is %s", (UnparseLlnd[cur_file->lang])(d1)); */
       return 0;
       }
  if(diff <= 0) newlow = plow; else newlow = qlow;
  if(integer_difference(phi, qhi, &diff,&d2) == 0){
       /* printf("hi diff is %s", (UnparseLlnd[cur_file->lang])(d2)); */
       return 0;
       }
  if(diff <= 0) newhi = qhi;   else newhi  = phi;
  *newp = make_llnd(cur_file, DDOT, newlow, newhi, NULL);
  /* printf("new ref is%s",(UnparseLlnd[cur_file->lang])(*newp)); */
  return 1;
}

  


PTR_LLND merge_ll_array_list(rl)
PTR_LLND rl;
{
  PTR_LLND t, newlist, junk;
  int stop;

  newlist = NULL;
  junk = NULL;
  while (rl != NULL) {
    if (rl->variant != EXPR_LIST) {
      fprintf(stderr, "problem in merge_ll_array_list, not exprlist\n%s\n",
		      (UnparseLlnd[cur_file->lang])(rl));
      break;
    }
    t = newlist;
    stop = 0;
    while (t != NULL) {
      if (subsumed(t->entry.Template.ll_ptr1,
		   rl->entry.Template.ll_ptr1)) {
	stop = 1;
      }
      t = t->entry.Template.ll_ptr2;
    }
    if (stop == 0) {
      t = rl;
      rl = rl->entry.Template.ll_ptr2;
      t->entry.Template.ll_ptr2 = newlist;
      newlist = t;
    }
    else {
      t = rl;
      rl = rl->entry.Template.ll_ptr2;
      t->entry.Template.ll_ptr2 = junk;
      junk = t;
    }
  }
  return (newlist);
}

PTR_REFL merge_array_refs(rl)
PTR_REFL rl;
{

  PTR_REFL t, newlist, junk;
  int stop;

  newlist = NULL;
  junk = NULL;
  while (rl != NULL) {
    t = newlist;
    stop = 0;
    while (t != NULL) {
      if (subsumed(t->node->refer, rl->node->refer)) {
	stop = 1;
      }
      t = t->next;
    }
    if (stop == 0) {
      t = rl;
      rl = rl->next;
      t->next = newlist;
      newlist = t;
    }
    else {
      t = rl;
      rl = rl->next;
      t->next = junk;
      junk = t;
    }
  }
  disp_refl(junk);
  return (newlist);
}


PTR_REFL alloc_ref(bif, ll)
PTR_BFND bif;
PTR_LLND ll;
{
  struct ref *p;
  PTR_REFL q;
  if ((bif == NULL) || (ll == NULL))
    return (NULL);

  if ((ll->variant == VAR_REF) || (ll->variant == ARRAY_REF) ||
      (ll->variant == RECORD_REF) || (ll->variant == POINTST_OP)) {
    p = (struct ref *) malloc(sizeof(struct ref));
#ifdef __SPF
    addToCollection(__LINE__, __FILE__,p, 0);
#endif
    if (p == NULL)
      fprintf(stderr, "! out of space for references !!\n");
    p->stmt = bif;
    p->refer = ll;
    if (free_refl != NULL) {
      q = free_refl;
      free_refl = free_refl->next;
    }
    else
    {
        q = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,q, 0);
#endif
    }
    if (q == NULL)
      fprintf(stderr, "out of space for reference lists !!\n");
    q->next = NULL;
    if (ll->variant == RECORD_REF || ll->variant == POINTST_OP)
      q->id = NULL;
    else
      q->id = p->refer->entry.Template.symbol;
    q->node = p;
    return (q);
  }
  else
    return (NULL);
}

void disp_refl(p)
PTR_REFL p;
{
  PTR_REFL q;

  while (p != NULL) {
    q = p->next;
    p->node = NULL;
    p->id = NULL;
    p->next = free_refl;
    free_refl = p;
    p = q;
  }
}

PTR_REFL copy_refl(p)
PTR_REFL p;
{
    PTR_REFL q;
    PTR_REFL tail, neo_q;

    if (p == NULL)
        return (NULL);
    q = NULL;
    tail = q;

    if (free_refl == NULL)
    {
        q = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,q, 0);
#endif
    }
    else {
        q = free_refl;
        free_refl = free_refl->next;
    }
    if (q == NULL) {
        fprintf(stderr, "!! out of space for reference lists !\n");
        return NULL;
    }
    q->node = p->node;
    q->id = p->id;
    q->next = NULL;
    /* now copy the rest of p */
    tail = q;
    p = p->next;
    while (p) {
        if (free_refl == NULL)
        {
            neo_q = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
            addToCollection(__LINE__, __FILE__,neo_q, 0);
#endif
        }
        else {
            neo_q = free_refl;
            free_refl = free_refl->next;
        }
        if (neo_q == NULL) {
            fprintf(stderr, "!! out of space for reference lists !\n");
            return NULL;
        }
        neo_q->node = p->node;
        neo_q->id = p->id;
        neo_q->next = NULL;
        tail->next = neo_q;
        tail = neo_q;
        p = p->next;
    }
    return q;
}
/* create a new reference list that is the interesction of two others */
/* the intersection is based on names and the actual reference comes  */
/* from the second argument of the pair.			      */
/* in the case of a pair  p   p->a  we include p->a in the intersection */
PTR_REFL intersect_refl(p, q)
PTR_REFL p, q;
{
    PTR_REFL s, t, inter;
    PTR_SYMB id;
    PTR_LLND z;
    int match_found;

    inter = NULL;
    s = q;
    while (p != NULL) {
        id = p->id;
        if (id == NULL) {		/* this is a ref to a p->a sub struct */
            z = p->node->refer;
            while (z != NULL && z->variant != VAR_REF)
                z = z->entry.Template.ll_ptr1;
            if (z == NULL)
                id = NULL;
            else
                id = z->entry.Template.symbol;
        }
        match_found = 0;
        while (s != NULL && (match_found == 0)) {
            if (s->id == NULL) {	/* a ref to a p->a sub struct */
                z = s->node->refer;
                while (z != NULL && z->variant != VAR_REF)
                    z = z->entry.Template.ll_ptr1;
                if (z == NULL)
                    s = s->next;
                else if (z->entry.Template.symbol == id)
                    match_found = 1;
                else
                    s = s->next;
            }
            else {
                if (s->id == id)
                    match_found = 1;
                else
                    s = s->next;
            }
        }

        if (match_found && id != NULL) {
            if (free_refl == NULL)
            {
                t = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
                addToCollection(__LINE__, __FILE__,t, 0);
#endif
            }
            else {
                t = free_refl;
                free_refl = free_refl->next;
            }
            if (t == NULL)
                fprintf(stderr, "!!! out of space for reference lists\n");
            if (p->node != NULL &&
                (p->node->refer->variant == POINTST_OP ||
                    p->node->refer->variant == RECORD_REF)) {
                t->node = p->node;
                t->id = NULL;
            }
            else {
                t->node = s->node;
                t->id = s->id;
            }
            t->next = inter;
            inter = t;
            s = s->next;
        }
        else {
            p = p->next;
            s = q;
        }
    }
    return (inter);
}

/*  make name list makes a reference list based on a list of symbol */
/*  table names.  The node field is null.  This is used for making  */
/*  a dummy list for arguments to procedures.			    */
PTR_REFL make_name_list(p)
PTR_SYMB p;
{
  PTR_REFL list, t;

  list = NULL;
  while (p != NULL) {
      if (free_refl == NULL)
      {
          t = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
          addToCollection(__LINE__, __FILE__,t, 0);
#endif
      }
    else {
      t = free_refl;
      free_refl = free_refl->next;
    }
    if (t == NULL)
      fprintf(stderr, "!!! out of space for reference lists\n");
    t->node = NULL;
    t->id = p;
    t->next = list;
    list = t;
    p = p->entry.var_decl.next_in;
  }
  return (list);
}

void append_refl(s, p)		/* and remove dups */
PTR_REFL *s, p;
{
    PTR_REFL t;
    struct ref *n;

    while (p != NULL) {
        n = p->node;
        t = *s;
        while ((t != NULL) && (t->node != n))
            t = t->next;
        if (t == NULL) {
            if (free_refl == NULL)
            {
                t = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
                addToCollection(__LINE__, __FILE__,t, 0);
#endif
            }
            else {
                t = free_refl;
                free_refl = free_refl->next;
            }
            if (t == NULL)
                fprintf(stderr, "!!! out of space for reference lists\n");
            t->node = p->node;
            t->id = p->id;
            t->next = *s;
            *s = t;
        }
        p = p->next;
    }
}

PTR_REFL union_refl(p, q)
PTR_REFL p, q;
{
    PTR_REFL s, t;
    struct ref *n;

    s = copy_refl(q);
    while (p != NULL) {
        n = p->node;
        t = q;
        while ((t != NULL) && (t->node != n))
            t = t->next;
        if (t == NULL) {
            if (free_refl == NULL)
            {
                t = (PTR_REFL)malloc(sizeof(struct refl));
#ifdef __SPF
                addToCollection(__LINE__, __FILE__,t, 0);
#endif
            }
            else {
                t = free_refl;
                free_refl = free_refl->next;
            }
            if (t == NULL) {
                fprintf(stderr, "!!! out of space for reference lists\n");
                exit(0);
            }
            t->node = p->node;
            t->id = p->id;
            t->next = s;
            s = t;
        }
        p = p->next;
    }
    return (s);
}

void assign(to, from) 
PTR_REFL *to;
PTR_REFL from;
{
  disp_refl(*to);
  *to = from;
}

void print_refl(p)
PTR_REFL p;
{
  int i;
  PTR_LLND z;

  fprintf(stderr, " ref list :");
  i = 0;
  while (p != NULL) {
    if (p->id != NULL)
      fprintf(stderr, " %s", p->id->ident);
    else {
      fprintf(stderr, " pointer de-ref");
      z = p->node->refer;
      while (z != NULL && z->variant != VAR_REF)
	z = z->entry.Template.ll_ptr1;
      if (z == NULL)
	fprintf(stderr, "-unknown");
      else
	fprintf(stderr, " %s", z->entry.Template.symbol->ident);
    }
    p = p->next;
    i++;
    if (i > 10) {
      i = 0;
      fprintf(stderr, "\n");
    }
  }
  fprintf(stderr, "\n");
}

int is_param(plist, s)
PTR_REFL plist;
PTR_SYMB s;
{
  while (plist != NULL) {
    if (plist->id == s)
      return (1);
    plist = plist->next;
  }
  return (0);
}


/********************************************************************/
/*  function equiv_ll_exp(p,q) returns 1 if p and q are equivalent  */
/*   algebraic expressions. both are low level experessions	    */
/********************************************************************/

int equiv_ll_exp(p, q)
PTR_LLND p, q;
{
  if (p == NULL && q == NULL)
    return (1);
  if (p == NULL || q == NULL)
    return (0);
  return (0);
}

int flat_check(p, q)
PTR_LLND p, q;
{
  if (p == NULL && q == NULL)
    return (1);
  if (p == NULL || q == NULL)
    return (0);
  if (p->variant != q->variant)
    return (0);
  if (p->variant == VAR_REF || p->variant == ARRAY_REF) {
    if (p->entry.var_ref.symbol != q->entry.var_ref.symbol)
      return (0);
  }
  if (flat_check(p->entry.Template.ll_ptr1, q->entry.Template.ll_ptr1) == 0)
    return (0);
  if (flat_check(p->entry.Template.ll_ptr2, q->entry.Template.ll_ptr2) == 0)
    return (0);
  return (1);
}


/********************************************************************/
/* function reduce_ll_exp(p,newp) takes a low level pointer and     */
/*  returns a new expression (or the same old one) that is a an     */
/*  simple algebraic expression in terms of constants and parameter */
/*  common references.	the function returns 1 if sucessfull and 0  */
/*  if it failed.  if a 2 is returned then an integer value has been*/
/*  generated and its value is return in the value newv.	    */
/*  newp is the pointer to the new expression.			    */
/********************************************************************/
int reduce_ll_exp(b, plist, induct_list, p, newp, newv)
PTR_BFND b;			/* bif node of expression (needed for
				 * context) */
PTR_REFL plist; 		/* list of parameters and commons in
				 * enclosing scope */
PTR_SYMB induct_list[]; 	/* induction variable list for current scope */
PTR_LLND p, *newp;
int *newv;
{
  int lf, rf, lv, rv;
  PTR_LLND lp, rp, make_llnd();

  lv = 0;
  rv = 0;
  lf = 0;
  rf = 0;
  if (p == NULL) {
    *newp = NULL;
    return (1);
  }
  if ((p->variant == EXPR_LIST || p->variant == RANGE_LIST)
      && p->entry.Template.ll_ptr2 == NULL)
    p = p->entry.Template.ll_ptr1;
  if (p->variant == VAR_REF) {
    /* first check for scalar propogation possibility */
    if (p->entry.Template.ll_ptr1 != NULL) {
      lf = reduce_ll_exp(b, plist, induct_list,
			 p->entry.Template.ll_ptr1, newp, newv);
      return (lf);
    }
    /* second check to see if this is a parameter or global */
    else if (is_param(plist, p->entry.Template.symbol) ||
	     is_not_loc(b, p)) {
      *newp = p;
      return (1);
    }
    /* this is some other variable and no propogation */
    /* can reduce it to a simple expression. give up  */
    else {
      *newp = p;
      return (0);
    }
  }
  else if (p->variant == CONST_REF) {
    *newp = p->entry.Template.symbol->entry.const_value;
    if ((*newp)->variant == INT_VAL) {
      *newv = (*newp)->entry.ival;
      return (2);
    }
    return (1);
  }
  else if (p->variant == INT_VAL) {
    *newv = p->entry.ival;
    *newp = p;
    return (2);
  }
  else if (p->variant != ADD_OP && p->variant != SUBT_OP &&
	   p->variant != MULT_OP && p->variant != DIV_OP &&
	   p->variant != MINUS_OP) {
    *newp = p;
    return (0);
  }
  else {
    lf = reduce_ll_exp(b, plist, induct_list,
		       p->entry.Template.ll_ptr1, &lp, &lv);
    rf = reduce_ll_exp(b, plist, induct_list,
		       p->entry.Template.ll_ptr2, &rp, &rv);
    if (lf == 2 && rf == 2) {
      *newp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
      switch (p->variant) {
       case ADD_OP:
	(*newp)->entry.ival = lv + rv;
	break;
       case SUBT_OP:
	(*newp)->entry.ival = lv - rv;
	break;
       case MULT_OP:
	(*newp)->entry.ival = lv * rv;
	break;
       case MINUS_OP:
	(*newp)->entry.ival = -lv;	/* not sure */
	break;
       case DIV_OP:
	if (rv != 0)
	  (*newp)->entry.ival = lv / rv;
	else
	  return (0);
	break;
       default:
	*newp = p;
	*newv = 0;
	return (0);
      }
      (*newp)->type = cur_file->head_type;
      *newv = (*newp)->entry.ival;
      return (2);
    }
    else {			/* both not integer case */
      if (lf == 2 && lv == 1 && p->variant == MULT_OP) {
	*newp = rp;
	return (rf);
      }
      if ((lf == 2) && (lv < 0)) {
	switch (p->variant) {
	 case ADD_OP:
	  *newp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  (*newp)->entry.ival = -lv;
	  *newp = make_llnd(cur_file, SUBT_OP, rp, *newp, NULL);
	  return (rf);
	 
	 case SUBT_OP:
	  *newp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  (*newp)->entry.ival = -lv;
	  *newp = make_llnd(cur_file, ADD_OP, rp, *newp, NULL);
	  return (rf);
	 
	 case MULT_OP:
	  if (lv == -1) {
	    if (rp->variant == MINUS_OP) {
	      *newp = rp->entry.Template.ll_ptr1;
	      *newv = rv;
	      return (rf);
	    }
	    else {
	      *newp = make_llnd(cur_file, MINUS_OP, rp, NULL, NULL);
	      return (rf);
	    }
	  }
	  break;
	 case MINUS_OP:
	 case DIV_OP:
	 default:
	  break;
	}
      } 			/* end if lf == 2 && lv < 0 */

      if (rf == 2 && rv == 1 && p->variant == MULT_OP) {
	*newp = lp;
	return (lf);
      }
      if (rf == 2 && (rv < 0)) {
	switch (p->variant) {
	 case ADD_OP:
	  *newp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  (*newp)->entry.ival = -rv;
	  *newp = make_llnd(cur_file, SUBT_OP, lp, *newp, NULL);
	  return (lf);
	  
	 case SUBT_OP:
	  *newp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  (*newp)->entry.ival = -rv;
	  *newp = make_llnd(cur_file, ADD_OP, lp, *newp, NULL);
	  return (lf);
	  
	 case MULT_OP:
	  if (rv == -1) {
	    if (rp->variant == MINUS_OP) {
	      *newp = lp->entry.Template.ll_ptr1;
	      *newv = lv;
	      return (lf);
	    }
	    else {
	      *newp = make_llnd(cur_file, MINUS_OP, lp, NULL, NULL);
	      return (lf);
	    }
	  }
	  break;
	 case MINUS_OP:
	 case DIV_OP:
	 default:
	  break;
	}
      } 			/* end if rf == 2 && rv < 0 */
      if (p->variant == ADD_OP) {
	if (rp->variant == MINUS_OP) {
	  *newp = make_llnd(cur_file, SUBT_OP, lp,
			    rp->entry.Template.ll_ptr1, NULL);
	  return (lf * rf);
	}
	if (lp->variant == MINUS_OP) {
	  *newp = make_llnd(cur_file, SUBT_OP, rp,
			    lp->entry.Template.ll_ptr1, NULL);
	  return (lf * rf);
	}
      }
      *newp = make_llnd(cur_file, p->variant,lp,rp,p->entry.Template.symbol);
      if (lf == 0 || rf == 0) {
	*newp = p;
	return (0);
      }
      if (lf == 1 || rf == 1) {
	lf = 1;
	rf = 1;
      }
      return (lf * rf);
    }
  }
}


/********************************************************************/
/* comp_offset computes the constant term in a low level expression */
/* the value is in coef and a 1 is returned.  If a 0 is returned    */
/* this means that no integer order zero term was computable.	    */
/* if a 2 is returned then a ddot was found ".."  coef contains the */
/* lower value and extra_coef contains the upper value.  Note: we   */
/* assume that the .. is at the root of the tree.		    */
/* if a 3 is returned then this is not a normal algebraic expression*/
/* if a 4 is returned then this is an algebraic expression using    */
/*   procedure parameters and vexp points to a ll tree representing */
/* the symbolic part of the constant.				    */
/* if a 5 is returned then it is a ddot with parameters.	    */
/* chkdflts = 1 means that the user should be prompted for defautls */
/* if a variable with no default value is found then a 3 will be    */
/* returned.  note: this needs more thought!			    */
/********************************************************************/
int extra_coef = 0;
int comp_offset(plist, induct_list, chkdflts, ll, coef, vexp)
PTR_REFL plist; 		/* list of parameters and commons in
				 * enclosing scope */
PTR_SYMB induct_list[]; 	/* induction variable list for current scope */
int chkdflts;
PTR_LLND ll;
int *coef;
PTR_LLND *vexp;
{
  int i, lf, rf, lcoef, rcoef, tmp;
  PTR_LLND lltmp, lexp, rexp;
  PTR_LLND make_llnd(), copy_llnd();

  tmp = 0;
  *coef = 0;
  *vexp = NULL;
  if (ll == NULL)
    return (0);
  else if (ll->variant == VAR_REF) {
    /* first check to see if this an induction variable */
    for (i = 0; i < MAX_NEST_DEPTH; i++) {
      if (ll->entry.Template.symbol == induct_list[i])
	return (0);
    }
    /* second check for scalar propogation possibility */
    if (ll->entry.Template.ll_ptr1 != NULL) {
      return (comp_offset(plist, induct_list, chkdflts,
			  ll->entry.Template.ll_ptr1, coef, vexp)
	);
    }
    /* third check to see if this is a scalar parameter */
    /* in this modified version the induction test was	*/
    /* put at the top and all unknown expressions are	*/
    /* returned as type 4.				    */
    else {
      *vexp = copy_llnd(ll);
      return (4);
    }
  }
  else if (ll->variant == CONST_REF) {
    lltmp = ll->entry.Template.symbol->entry.const_value;
    if (lltmp->variant == INT_VAL) {
      *coef = lltmp->entry.ival;
      *vexp = copy_llnd(ll);
      return (1);
    }
    else
      return (0);
  }
  else if (ll->variant == INT_VAL) {
    *coef = ll->entry.ival;
    *vexp = copy_llnd(ll);
    return (1);
  }
  else {
    lf = comp_offset(plist, induct_list, chkdflts,
		     ll->entry.Template.ll_ptr1, &lcoef, &lexp);
    rf = comp_offset(plist, induct_list, chkdflts,
		     ll->entry.Template.ll_ptr2, &rcoef, &rexp);
    if (lf == 3 || rf == 3)
      return (3);
    if (lf == 5 || rf == 5)
      return (5);
    switch (ll->variant) {
     case DDOT:
      if (lf == 1)
	*coef = lcoef;
      else
	*coef = 0;
      if (rf == 1)
	extra_coef = rcoef;
      else
	extra_coef = 0;
      if ((lf == 1) || (rf == 1))
	return (2);
      if (lf == 4 || rf == 4)
	return (5);
      else
	return (0);
     case ADD_OP:
      tmp = 0;
      if (lf == 4 && rf == 0) {
	*vexp = lexp;
	return (4);
      }
      if (rf == 4 && lf == 0) {
	*vexp = rexp;
	return (4);
      }
      if (lf == 4 || rf == 4) {
	if (rexp->variant == MINUS_OP)
	  *vexp = make_llnd(cur_file, SUBT_OP, lexp,
			    rexp->entry.Template.ll_ptr1, NULL);
	else
	  *vexp = make_llnd(cur_file, ADD_OP, lexp, rexp, NULL);
	return (4);
      }
      if (lf == 1)
	tmp = lcoef;
      if (rf == 1)
	tmp = tmp + rcoef;
      if ((lf == 1) || (rf == 1)) {
	*coef = tmp;
	*vexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	(*vexp)->entry.ival = tmp;
	return (1);
      }
      else
	return (0);
     case SUBT_OP:
      tmp = 0;
      if (lf == 4 && rf == 0) {
	*vexp = lexp;
	return (4);
      }
      if (rf == 4 && lf == 0) {
	if (rexp->variant == INT_VAL) {
	  rexp->entry.ival = -(rexp->entry.ival);
	  *vexp = rexp;
	  return (4);
	}
	if (rexp->variant != MINUS_OP)
	  *vexp = make_llnd(cur_file, MINUS_OP, rexp, NULL, NULL);
	else
	  *vexp = rexp->entry.Template.ll_ptr1;
	return (4);
      }
      if (lf == 4 || rf == 4) {
	if (rexp->variant == MINUS_OP)
	  *vexp = make_llnd(cur_file, ADD_OP, lexp,
			    rexp->entry.Template.ll_ptr1, NULL);
	else
	  *vexp = make_llnd(cur_file, SUBT_OP, lexp, rexp, NULL);
	return (4);
      }
      if (lf == 1)
	tmp = lcoef;
      if (rf == 1)
	tmp = tmp - rcoef;
      if ((lf == 1) || (rf == 1)) {
	*coef = tmp;
	*vexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	(*vexp)->entry.ival = tmp;
	return (1);
      }
      else
	return (0);
     case MULT_OP:
      if (lf == 4 && rf == 0)
	return (0);
      if (rf == 4 && lf == 0)
	return (0);
      if (lf == 4 || rf == 4) {
	if (rexp->variant == MULT_OP) { /* left associate terms */
	  lltmp = rexp->entry.Template.ll_ptr1;
	  lltmp = make_llnd(cur_file, MULT_OP, lexp, lltmp, NULL);
	  *vexp = make_llnd(cur_file, MULT_OP, lltmp,
			    rexp->entry.Template.ll_ptr2, NULL);
	  return (4);
	}
	if (rf == 1) {
	  *vexp = make_llnd(cur_file, MULT_OP, rexp, lexp, NULL);
	}
	else {
	  *vexp = make_llnd(cur_file, MULT_OP, lexp, rexp, NULL);
	}
	return (4);
      }
      if ((lf == 1) && (rf == 1)) {
	*coef = lcoef * rcoef;
	*vexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	(*vexp)->entry.ival = *coef;
	return (1);
      }
      else
	return (0);
     case MINUS_OP:
      if (lf == 4) {
	if (lexp->variant == MINUS_OP)
	  *vexp = lexp->entry.Template.ll_ptr1;
	else
	  *vexp = make_llnd(cur_file, MINUS_OP, lexp, NULL, NULL);
      }
      else if (lf == 1) {
	*vexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	*coef = -lcoef;
	(*vexp)->entry.ival = *coef;
      }
      return (lf);
     case DIV_OP:
      if (lf == 4 && rf == 0)
	return (0);
      if (rf == 4 && lf == 0)
	return (0);
      if (lf == 4 || rf == 4) {
	*vexp = make_llnd(cur_file, DIV_OP, lexp, rexp, NULL);
	return (4);
      }
      if ((rcoef != 0) && (lf == 1) && (rf == 1)) {
	*coef = lcoef / rcoef;
	*vexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	(*vexp)->entry.ival = *coef;
	return (1);
      }
      else
	return (0);
     case EXPR_LIST:
      if (ll->entry.Template.ll_ptr2 == NULL) {
	*vexp = lexp;
	*coef = lcoef;
	return (lf);
      }
     default:
      *coef = 0;
      return (3);		/* not normal */
    }
  }
}

/*****************************************************************/
/* search symb searches a ll tree returns 0 if a const. is found */
/* a -2 if another symbol is found as a multiplicative factor	 */
/* for example, searching for i in 2*i*(5+j) returns -2 	 */
/* a -1 if it is found but not in a linear combination. 	 */
/* and a  1 if it is and coef has the value of the  coefecient	 */
/* In the case that a ddot ".." is found a 2 is returned and	 */
/* coef has the value of the low bound term and extra_coef has	 */
/* the high value.  Note this implies that .. is at the root of  */
/* the tree.							 */
/* chkdflts=1 means that the usr should be prompted for defautls */
/*****************************************************************/

/* returns 1 if constant coef and *coef is set. 	*/
/* returns -2 if non-constant coef and *exp is set	*/
/* returns 0 if constant but not coef and *coef is set	*/
/* returns 2 if non-constant non-coef is found. *exp set*/
/* returns -1 for non-linear expressions in s		*/

int new_search_symb(s, induct_list, ll, coef, exp)
PTR_SYMB s;
PTR_SYMB induct_list[];
PTR_LLND ll, *exp;
int *coef;
{
  int lval, rval;
  PTR_LLND lexp, rexp, nll, make_llnd(), copy_llnd();
  int lcoef, rcoef;

  if (ll == NULL) {
    *coef = 0;
    return (0);
  }
  lexp = NULL;
  rexp = NULL;
  if (ll->variant == VAR_REF) {
    if (ll->entry.Template.symbol == s) {
      *coef = 1;
      *exp = NULL;
      return (1);
    }
    if (ll->entry.Template.ll_ptr1 != NULL) {
      return (
      new_search_symb(s, induct_list, ll->entry.Template.ll_ptr1, coef, exp)
	);
    }
    else {
      *exp = ll;
      return (2);
    }
  }
  else if (ll->variant == INT_VAL) {
    *coef = ll->entry.ival;
    *exp = NULL;
    return (0);
  }
  else {
   lval=new_search_symb(s,induct_list,ll->entry.Template.ll_ptr1,&lcoef,&lexp);
   rval=new_search_symb(s,induct_list,ll->entry.Template.ll_ptr2,&rcoef,&rexp);
    switch (ll->variant) {
     case MINUS_OP:
      if (lval == 1 || lval == 0) {
	*coef = -lcoef;
	return (lval);
      }
      else if (lval == -2 || lval == 2) {
	if (lexp->variant == MINUS_OP)
	  *exp = lexp->entry.Template.ll_ptr1;
	else
	  *exp = make_llnd(cur_file, MINUS_OP, lexp, NULL, NULL);
	return (lval);
      }
      else
	return (-1);
     case MULT_OP:
     case DIV_OP:
      if (rval == 1) {		/* right side is const coef of s */
	switch (lval) {
	 case 0:
	  if (ll->variant == MULT_OP) {
	    *coef = lcoef * rcoef;
	    return (1);
	  }
	  else if (rcoef != 0) {
	    *coef = lcoef / rcoef;
	    return (1);
	  }
	  else
	    return (-1);
	 case -2:
	 case -1:
	 case 1:
	  return (-1);
	 case 2:
	  if (rcoef == 1)
	    *exp = lexp;
	  else {
	    if (ll->variant == DIV_OP && rcoef == 0)
	      return (-1);
	    nll = make_llnd(cur_file, INT_VAL, NULL, NULL, rcoef);
	    nll = make_llnd(cur_file, ll->variant, lexp, nll, NULL);
	    *exp = nll;
	  }
	  return (-2);
	}
      }
      else if (rval == 0) {	/* right side is just a constant */
	switch (lval) {
	 case 0:
	  if (ll->variant == MULT_OP) {
	    *coef = lcoef * rcoef;
	    return (0);
	  }
	  else if (rcoef != 0) {
	    *coef = lcoef / rcoef;
	    return (0);
	  }
	  else
	    return (-1);
	 case -2:		/* left side is non-const coef of s */
	 case 2:		/* or non-const non-coef */
	  if (rcoef == 1)
	    *exp = lexp;
	  else {
	    nll = make_llnd(cur_file, INT_VAL, NULL, NULL, rcoef);
	    nll = make_llnd(cur_file, ll->variant, lexp, nll, NULL);
	    *exp = nll;
	  }
	  return (lval);
	 case 1:
	  if (ll->variant == MULT_OP) {
	    *coef = lcoef * rcoef;
	    return (1);
	  }
	  else if (rcoef != 0) {
	    *coef = lcoef / rcoef;
	    return (1);
	  }
	  else
	    return (-1);
	 case -1:
	  return (-1);
	}
      }
      else if (rval == 2) {	/* right side is a non-constant non coef */
	switch (lval) {
	 case 1:
	 case 0:
	  if (lcoef == 1)
	    *exp = rexp;
	  else {
	    nll = make_llnd(cur_file, INT_VAL, NULL, NULL, lcoef);
	    nll = make_llnd(cur_file, MULT_OP, nll, rexp, NULL);
	    *exp = nll;
	  }
	  if (lval == 0)
	    return (2);
	  else
	    return (-2);
	 case 2:
	  *exp = ll;
	  return (2);
	 case -2:
	  *exp = make_llnd(cur_file, MULT_OP, lexp, rexp, NULL);
	  return (-2);
	 case -1:
	  return (-1);
	}
      }
      else if (rval == -2) {	/* right side is a coef of s but not const */
	switch (lval) {
	 case 1:
	 case -2:
	 case -1:
	  return (-1);
	 case 0:
	  if (lcoef == 1)
	    *exp = rexp;
	  else {
	    nll = make_llnd(cur_file, INT_VAL, NULL, NULL, lcoef);
	    nll = make_llnd(cur_file, MULT_OP, nll, rexp, NULL);
	    *exp = nll;
	  }
	  return (-2);
	 case 2:
	  *exp = make_llnd(cur_file, MULT_OP, lexp, rexp, NULL);
	  return (-2);
	}
      }
      else			/* rval == -1 */
	return (-1);
     case ADD_OP:
     case SUBT_OP:
      if (rval == 1) {		/* right side is const times s */
	switch (lval) {
	 case 1:		/* lhs is const coef */
	  if (ll->variant == ADD_OP)
	    *coef = lcoef + rcoef;
	  else
	    *coef = lcoef - rcoef;
	  return (1);
	 case -2:		/* lhs is non-const coef */
	  nll = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  if (ll->variant == ADD_OP)
	    nll->entry.ival = rcoef;
	  else
	    nll->entry.ival = -rcoef;
	  if (lexp->variant == MINUS_OP) {
	    lexp = lexp->entry.Template.ll_ptr1;
	    *exp = make_llnd(cur_file, SUBT_OP, nll, lexp, NULL);
	  }
	  else
	    *exp = make_llnd(cur_file, ADD_OP, lexp, nll, NULL);
	  return (-2);
	 case -1:
	  return (-1);
	 case 0:		/* lhs is const */
	 case 2:		/* lhs is non const */
	  if (ll->variant == ADD_OP)
	    *coef = rcoef;
	  else
	    *coef = -rcoef;
	  return (1);
	}
      }
      else if (rval == -2) {	/* right side is non-const times s */
	switch (lval) {
	 case 1:		/* lhs is const coef */
	  lexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  if (lexp->variant == ADD_OP)
	    lexp->entry.ival = lcoef;
	  else
	    lexp->entry.ival = -lcoef;
	 case -2:		/* lhs is non-const coef */
	  *exp = make_llnd(cur_file, ll->variant, lexp, rexp, NULL);
	  return (-2);
	 case -1:
	  return (-1);
	 case 0:		/* lhs is const */
	 case 2:		/* lhs is non const */
	  if (ll->variant == SUBT_OP) {
	    rexp = make_llnd(cur_file, MINUS_OP, rexp, NULL, NULL);
	  }
	  *exp = rexp;
	  return (-2);
	}
      }
      else if (rval == 0) {	/* right side is just constant */
	switch (lval) {
	 case 1:		/* lhs is const coef */
	  *coef = lcoef;
	  return (1);
	 case -2:		/* lhs is non-const coef */
	  *exp = lexp;
	  return (-2);
	 case -1:
	  return (-1);
	 case 0:		/* lhs is const */
	  if (ll->variant == ADD_OP)
	    *coef = lcoef + rcoef;
	  else
	    *coef = lcoef - rcoef;
	  return (0);
	 case 2:		/* lhs is non const */
	  nll = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  nll->entry.ival = rcoef;
	  *exp = make_llnd(cur_file, ll->variant, lexp, nll, NULL);
	  return (2);
	}
      }
      else if (rval == 2) {	/* right side in non-const non coef */
	switch (lval) {
	 case 1:		/* lhs is const coef */
	  *coef = lcoef;
	  return (1);
	 case -2:		/* lhs is non-const coef */
	  *exp = lexp;
	  return (-2);
	 case -1:
	  return (-1);
	 case 0:		/* lhs is const */
	  lexp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  lexp->entry.ival = lcoef;
	 case 2:		/* lhs is non const */
	  *exp = make_llnd(cur_file, ll->variant, lexp, rexp, NULL);
	  return (2);
	}
      }
      else			/* if(rval == -1) */
	return (-1);
     case DDOT:
     case ARRAY_REF:
     case FUNC_CALL:
      return (-1);
     default:
      return (-1);
    }
  }
}

int search_symb(chkdflts, s, ll, coef)
int chkdflts;
PTR_SYMB s;
PTR_LLND ll;
int *coef;
{
  int i, lf, rf, lcoef, rcoef, tmp;
  PTR_LLND lltmp;

  tmp = 0;
  *coef = 0;
  if (ll == NULL)
    return (0);
  else if (ll->variant == VAR_REF) {
    if (ll->entry.Template.symbol == s) {
      *coef = 1;
      return (1);
    }
    else {
      /* first try a variable propogation to find s */
      if (ll->entry.Template.ll_ptr1 != NULL) {
	return (
		search_symb(chkdflts, s, ll->entry.Template.ll_ptr1, coef)
	  );
      }
      else if (chkdflts) {
	for (i = 0; i < MAX_NEST_DEPTH; i++) {
	  if (ll->entry.Template.symbol == induct_list[i])
	    return (-3);
	}
	 return (0);
      }
      else
	return (-3);
    }
  }
  else if (ll->variant == CONST_REF) {
    lltmp = ll->entry.Template.symbol->entry.const_value;
    if (lltmp->variant == INT_VAL) {
      *coef = lltmp->entry.ival;
      return (0);
    }
    else
      return (-3);
  }
  else if (ll->variant == INT_VAL) {
    *coef = ll->entry.ival;
    return (0);
  }
  else {
    lf = search_symb(chkdflts, s, ll->entry.Template.ll_ptr1, &lcoef);
    rf = search_symb(chkdflts, s, ll->entry.Template.ll_ptr2, &rcoef);
    switch (ll->variant) {
     case DDOT:
      if (lf == 1)
	*coef = lcoef;
      else
	*coef = 0;
      if (rf == 1)
	extra_coef = rcoef;
      else
	extra_coef = 0;
      if ((lf == 1) || (rf == 1))
	return (2);
      else {
	if (lf * rf == 0)
	  return (0);
	else
	  return ((lf <= rf) ? rf : lf);
      }
     case ADD_OP:
      if (lf == 1)
	tmp = lcoef;
      if (rf == 1)
	tmp = tmp + rcoef;
      if ((lf == 1) || (rf == 1)) {
	*coef = tmp;
	return (1);
      }
      else {
	*coef = rcoef + lcoef;
	if (lf * rf == 0)
	  return (0);
	else
	  return ((lf <= rf) ? rf : lf);
      }
     case SUBT_OP:
      if (lf == 1)
	tmp = lcoef;
      if (rf == 1)
	tmp = tmp - rcoef;
      if ((lf == 1) || (rf == 1)) {
	*coef = tmp;
	return (1);
      }
      else {
	*coef = lcoef - rcoef;
	if (lf * rf == 0)
	  return (0);
	else
	  return ((lf <= rf) ? rf : lf);
      }
     case MULT_OP:
      tmp = 1;
      if ((lf == 1) || (lf == 0))
	tmp = lcoef;
      if ((rf == 1) || (rf == 0))
	tmp = tmp * rcoef;
      if ((lf * rf) == 0) {
	*coef = tmp;
	return (lf + rf);
      }
      else if ((lf == 1) && (rf == 1)) {
	*coef = 1;
	return (-1);
      }
      else {
	*coef = 1;
	return (-2);
      }
     case MINUS_OP:
      *coef = -lcoef;
      return (lf);
     default:
      *coef = 999;
      return (-2);
    }
  }
}

void print_subscr(r, arr, induct_list)
PTR_SYMB induct_list[];
struct ref *r;
struct subscript arr[];
{
  int i, j;
  PTR_LLND ll;
  char *s;

  ll = r->refer;
  if (induct_list[0] == NULL)
    return;
  for (j = 0; j < 2; j++) {
    fprintf(stderr, "______________________________________________________\n");
    fprintf(stderr, "|	ID  | decidable | offset |    %s |    %s |    %s | parm_exp \n",
	   induct_list[0]->ident,
	   (induct_list[1] == NULL) ? "-" : induct_list[1]->ident,
	   (induct_list[2] == NULL) ? "-" : induct_list[2]->ident);
    fprintf(stderr, "|-----------------------------------------------------|\n");
    if (arr[j].parm_exp != NULL)
      s = (UnparseLlnd[cur_file->lang])(arr[j].parm_exp);
    else
      s = "";
    fprintf(stderr, "|	 %s  |	  %d	  |   %d   |	%d |   %d  |   %d  |%s\n",
	   ll->entry.array_ref.symbol->ident,
	   arr[j].decidable, arr[j].offset,
	   arr[j].coefs[0], arr[j].coefs[1], arr[j].coefs[2], s
      );
    fprintf(stderr, "|-----------------------------------------------------|\n");
    for (i = 0; i < 2; i++) {
      if (arr[j].coefs_symb[i] != NULL)
	fprintf(stderr, "    arr[%d].coefs_symb[%d] = %s\n", j, i,
			(UnparseLlnd[cur_file->lang])(arr[j].coefs_symb[i]));
    }
    fprintf(stderr, "|-----------------------------------------------------|\n");
  }
}

/* structure equiv. takes two low level pointers to expressions and test */
/* them for equivalence as expressions.  if equif returns 1 else 0	 */
/* this version checks only syntatic equiv.  algebraic equiv will be needed */
int sequiv(sub1, sub2)
PTR_LLND sub1, sub2;
{
  if ((sub1 == NULL) && (sub2 == NULL))
    return (1);
  if (((sub1 == NULL) && (sub2 != NULL)) ||
      ((sub1 != NULL) && (sub2 == NULL)))
    return (0);
  /* both not null */
  if (sub1->variant != sub2->variant)
    return (0);
  else {
    if (sub1->variant == VAR_REF) {
      if (sub1->entry.Template.symbol ==
	  sub2->entry.Template.symbol)
	return (1);
      else
	return (0);
    }
    else {
      if (sequiv(sub1->entry.Template.ll_ptr1,
		 sub2->entry.Template.ll_ptr1) &&
	  sequiv(sub1->entry.Template.ll_ptr2,
		 sub2->entry.Template.ll_ptr2)
	)
	return (1);
      else
	return (0);
    }
  }
}

/* make_subscr(r,arr) creates the subscript array for the reference r */
void make_subscr(r, arr)
struct ref *r;
struct subscript arr[];
{
  int i, j;
  PTR_BFND b, fun;
  PTR_REFL plist;
  PTR_LLND ll, tl, index_exper, parexp, exp;
  struct subscript il_lo[MAX_NEST_DEPTH];
  struct subscript il_hi[MAX_NEST_DEPTH];
  int depth, found, coef;

  b = r->stmt;
  ll = r->refer;
  for (j = 0; j < AR_DIM_MAX; j++) {
    arr[j].decidable = -1;
    arr[j].parm_exp = NULL;
    arr[j].offset = 0;
    arr[j].vector = NULL;
    for (i = 0; i < MAX_NEST_DEPTH; i++) {
      arr[j].coefs[i] = 0;
      arr[j].coefs_symb[i] = NULL;
    }
  }

  /* now make build the set of valid induction variables */
  depth = make_induct_list(b, induct_list, il_lo, il_hi);
  /* now find the parameters and common vars for this scope */
  fun = b;
  while (fun != NULL && (fun->variant != PROG_HEDR) &&
	 (fun->variant != FUNC_HEDR) &&
	 (fun->variant != PROC_HEDR))
    fun = fun->control_parent;
  if (fun == NULL)
    return;
  if(fun->entry.Template.sets == NULL) plist = NULL;
  else plist = fun->entry.Template.sets->in_def;

  /* now for each array index position build the vector of coefs. */
  /* start with the left most position numbered by i */
  i = 0;
  if (ll->variant == ARRAY_REF) {
    tl = ll->entry.array_ref.index;
    while (tl != NULL) {
      if ((tl->variant == VAR_LIST) ||
	  (tl->variant == EXPR_LIST) ||
	  (tl->variant == RANGE_LIST)) {
	index_exper = tl->entry.Template.ll_ptr1;
	if (index_exper == NULL ||
	    index_exper->variant == STAR_RANGE) {
	  arr[i].vector = index_exper;
	  arr[i].decidable = 0;
	  arr[i].coefs[depth] = 0;
	}
	else if (index_exper->variant == DDOT) {
	  /* we have a vector			 */
	  /* set the decidable flag to 2	 */
	  /* and save a pointr to the vector	 */
	  /* bounds for later use		 */
	  /* we set the coef in position	 */
	  /* depth to be 1 so this is		 */
	  /* a pseudo loop.  the bounds of the	 */
	  /* loops will be set			 */
	  /* as inequalities.  NOTE: for stride  */
	  /* vectors we will			 */
	  /* set the coef to be equal to thestride */
	  arr[i].vector = index_exper;
	  arr[i].decidable = 2;
	  arr[i].coefs[depth] = 1;
	}
	else {
	  /* this is just a standard scalar expression */
	  arr[i].decidable = 1;
	  parexp = NULL;
	  found = comp_offset(plist, induct_list, 1,
			      index_exper, &coef, &parexp);
	  if (found == 1)
	    arr[i].offset = coef;
	  if (found == 4) {
	    arr[i].offset = 0;
	    arr[i].parm_exp = parexp;
	  }
	  for (j = 0; j < depth; j++) {
	    found=new_search_symb(induct_list[j],
				  induct_list,index_exper, &coef, &exp);
	    switch (found) {
	     case 1:		/* constant coef */
	      arr[i].coefs[j] = coef;
	      break;
	     case -2:		/* variable coef */
	      arr[i].coefs_symb[j] = exp;
	      break;
	     case -1:
	      arr[i].decidable = 0;
	     case 0:
	     case 2:
	      arr[i].coefs[j] = 0;
	      break;
	    }
	  }
	  for (j = depth; j < MAX_NEST_DEPTH; j++)
	    arr[i].coefs[j] = 0;
	  if (arr[i].decidable == -1)
	    arr[i].decidable = 3;
	}
	tl = tl->entry.Template.ll_ptr2;
	i++;
      }
      else {			/* must be a simple 1 Dim. subscript */
	arr[i].decidable = 1;
	parexp = NULL;
	found = comp_offset(plist, induct_list, 1, tl, &coef, &parexp);
	if (found != 0)
	  arr[i].offset = coef;
	if (found == 4) {
	  arr[i].offset = 0;
	  arr[i].parm_exp = parexp;
	}
	for (j = 0; j < depth; j++) {
	  found = new_search_symb(induct_list[j], induct_list, tl,&coef,&exp);
	  switch (found) {
	   case 1:		/* constant coef */
	    arr[i].coefs[j] = coef;
	    break;
	   case -2:		/* variable coef */
	    arr[i].coefs_symb[j] = exp;
	    break;
	   case -1:
	    arr[i].decidable = 0;
	   case 0:
	   case 2:
	    arr[i].coefs[j] = 0;
	    break;
	  }
	}
	for (j = depth; j < MAX_NEST_DEPTH; j++)
	  arr[i].coefs[j] = 0;
	tl = NULL;
      }
    }				/* end while */
  }				/* end if array_ref */
}

/********************************************************************/
/* search_inc_scalar(b) looks for a scalar variable in the condition*/
/*  that is modified in the body of the loop.			    */
/*  this is returned and used as an induction varialble in the	    */
/*   routine below. There are two utility routines which recursively*/
/*   search the condition tree and the body of the loop 	    */
/********************************************************************/
int ll_search(ll, s)
PTR_LLND ll;
PTR_SYMB s;
{
  if (ll == NULL)
    return (0);
  else {
    switch (ll->variant) {
     case VAR_REF:
      if (ll->entry.var_ref.symbol == s)
	return (1);
      else
	return (0);
     case ARRAY_REF:
      return (ll_search(ll->entry.array_ref.index, s));
     case CONST_REF:
      return (0);
     default:
      if (ll_search(ll->entry.Template.ll_ptr1, s))
	return (1);
      else
	return (ll_search(ll->entry.Template.ll_ptr2, s));
    }
  }
}

int body_search(b, s)
PTR_BFND b;
PTR_SYMB s;
{
  PTR_BLOB x;

  if (b == NULL)
    return (0);
  else {
    switch (b->variant) {
     case ASSIGN_STAT:
     case M_ASSIGN_STAT:
     case SUM_ACC:
     case MULT_ACC:
     case MAX_ACC:
     case MIN_ACC:
     case CAT_ACC:
     case OR_ACC:
     case AND_ACC:
      return (ll_search(b->entry.Template.ll_ptr1, s));
     case FOR_NODE:
     case FORALL_NODE:
     case WHILE_NODE:
      x = b->entry.Template.bl_ptr1;
      while (x != NULL && x->ref != b) {
	if (body_search(x->ref, s))
	  return (1);
	x = x->next;
      }
      return (0);
     case IF_NODE:
      x = b->entry.if_node.control_true;
      while (x != NULL) {
	if (body_search(x->ref, s))
	  return (1);
	x = x->next;
      }
      x = b->entry.if_node.control_false;;
      while (x != NULL) {
	if (body_search(x->ref, s))
	  return (1);
	x = x->next;
      }
      return (0);
     default:
      return (0);
    }
  }
}

PTR_SYMB induc_search(b, ll)
PTR_BFND b;
PTR_LLND ll;
{
  PTR_SYMB s;

  if (ll == NULL)
    return (NULL);
  else {
    switch (ll->variant) {
     case VAR_REF:
      if (body_search(b, ll->entry.var_ref.symbol))
	return (ll->entry.var_ref.symbol);
      else
	return (NULL);
     case ARRAY_REF:
      return (induc_search(b, ll->entry.array_ref.index));
     case CONST_REF:
      return (NULL);
     default:
      if ((s = induc_search(b, ll->entry.Template.ll_ptr1))
	  != NULL)
	return (s);
      else
	return (induc_search(b, ll->entry.Template.ll_ptr2));
    }
  }
}


PTR_SYMB search_inc_scalar(b)
PTR_BFND b;
{
  PTR_LLND v;

  v = b->entry.while_node.condition;
  return (induc_search(b, v));
}


/********************************************************************/
/* Make_induct_list(b,induct_list ) creates the induction list as   */
/* seen from this point in the graph.  the function returns the nest*/
/* level and it also side effects four other arrays: il_lo, il_hi   */
/* which describe the low and hi bounds for the list and the vectors*/
/* stride and is_forall.  In the case of a stride component that is */
/* not one, we normalize the induction list arrays as follows.	    */
/* if the stride is not a constant il_lo and il_hi is set undecidble*/
/* otherwise il_lo is set to 0 and il_hi becomes (il_hi-il_lo)/str  */
/* The way this works:	it goes up the tree and fills in the loop   */
/* index variables from the top down to this point.		    */
/* In the case of WHILE loops and C for loops as well as while loops*/
/* we must try to identify an induction 			    */
/* variable.  We will do this by searching the test condition for   */
/* first scalar variable.  This is not accurate.  What we should do */
/* is search for a scalar variable that changes value in the body of*/
/* the iteration, but that is not done yet. I will do it later.     */
/********************************************************************/
int make_induct_list(b, induct_list, il_lo, il_hi)
PTR_BFND b;
PTR_SYMB induct_list[];
struct subscript il_lo[];
struct subscript il_hi[];
{
  int i, j, found, coef;
  PTR_LLND p, lv, rv, q, pexp;
  PTR_REFL plist;
  PTR_BFND proc;

  if ((b == NULL) || (b->variant == GLOBAL)) {
    return (0);
  }
  else {
    for (j = 0; j < MAX_NEST_DEPTH; j++) {
      il_lo[j].decidable = -1;
      il_lo[j].parm_exp = NULL;
      il_lo[j].offset = 0;
      il_lo[j].vector = NULL;
      for (i = 0; i < MAX_NEST_DEPTH; i++) {
	il_lo[j].coefs[i] = 0;
	il_lo[j].coefs_symb[i] = NULL;
      }
      il_hi[j].decidable = -1;
      il_hi[j].parm_exp = NULL;
      il_hi[j].offset = 0;
      il_hi[j].vector = NULL;
      for (i = 0; i < MAX_NEST_DEPTH; i++) {
	il_hi[j].coefs[i] = 0;
	il_hi[j].coefs_symb[i] = NULL;
      }
    }
    /* first generate the list of parameters of the function */
    proc = b;
    while (proc != NULL && (proc->variant != PROC_HEDR) &&
	   (proc->variant != FUNC_HEDR) &&
	   (proc->variant != PROG_HEDR))
      proc = proc->control_parent;
    if (proc == NULL)
      return 0;
    if (proc->entry.Template.sets == NULL)
      plist = NULL;
    else
      plist = proc->entry.Template.sets->out_use;

    /* now recursive apply procedure */
    i = make_induct_list(b->control_parent, induct_list, il_lo, il_hi);
    if ((b->variant == FOR_NODE) ||
	(b->variant == FORALL_NODE)) {
      if (i > MAX_NEST_DEPTH) {
	fprintf(stderr, " nest too deep ! \n");
	return (0);
      }
      if (b->entry.for_node.control_var == NULL) {
	/* must be a C for loop */
	lv = b->entry.Template.ll_ptr1; /* exp list */
	if (lv == NULL) {
	  /* try to go for the increment exp */
	  lv = b->entry.Template.ll_ptr3;
	  rv = lv->entry.Template.ll_ptr1;	/* op */
	  lv = rv->entry.Template.ll_ptr1;
	  induct_list[i] =
	    lv->entry.Template.symbol;
	  lv = NULL;
	  il_lo[i].decidable = 0;
	}
	else {
	  rv = lv->entry.Template.ll_ptr1;	/* asign op */
	  lv = rv->entry.Template.ll_ptr1;	/* var ref */
	  il_lo[i].decidable = 1;
	  induct_list[i] = lv->entry.Template.symbol;
	  lv = rv->entry.Template.ll_ptr2;	/* start val */
	}
	is_forall[i] = 0;
	/* now do hi bound for C case */
	rv = b->entry.Template.ll_ptr2; /* 2nd expr */
	rv = rv->entry.Template.ll_ptr1;
	rv = rv->entry.Template.ll_ptr2;
	stride[i] = 1;		/* these two lines are bogus */
	il_hi[i].decidable = 1;
      }
      else {			/* fortran case */
	induct_list[i] = b->entry.for_node.control_var;
	if (b->variant == FORALL_NODE)
	  is_forall[i] = 1;
	else
	  is_forall[i] = 0;
	/* now create low and hi bounds */
	p = b->entry.for_node.range;
	if (p->variant != DDOT)
	  fprintf(stderr, "bad range node\n");
	lv = p->entry.Template.ll_ptr1;
	rv = p->entry.Template.ll_ptr2;
	il_lo[i].decidable = 1;
	il_hi[i].decidable = 1;
	stride[i] = 1;
	if ((lv->variant == DDOT) ||
	    (b->entry.for_node.increment != NULL)) {
	  /* we have a stride term! */
	  if (b->entry.for_node.increment != NULL)
	    q = b->entry.for_node.increment;
	  else {
	    q = rv;
	    rv = lv->entry.Template.ll_ptr2;
	    lv = lv->entry.Template.ll_ptr1;
	  }
	  /* we currently only support constant strides */
	  /* this can be improved to general expressions */
	  found = comp_offset(plist, induct_list, 1, q, &coef, &pexp);
	  if (found != 3)
	    stride[i] = coef;
	  if ((found == 4) || (found == 3) || (stride[i] == 0)) {
	    il_lo[i].decidable = 0;
	    il_hi[i].decidable = 0;
	    stride[i] = 1;
	  }
	}
      } 			/* end fortran case */
      pexp = NULL;
      found = comp_offset(plist, induct_list, 1, lv, &coef, &pexp);
      if (found >= 3)
	il_lo[i].decidable = 0;
      if (found == 4)
	il_lo[i].parm_exp = pexp;
      else
	il_lo[i].parm_exp = NULL;
      if (found != 0)
	il_lo[i].offset = coef;
      pexp = NULL;
      found = comp_offset(plist, induct_list, 1, rv, &coef, &pexp);
      if (found >= 3)
	il_hi[i].decidable = 0;
      if (found == 4)
	il_hi[i].parm_exp = pexp;
      else
	il_hi[i].parm_exp = NULL;
      if (found != 0)
	il_hi[i].offset = coef;
      for (j = 0; j < i; j++) {
	found = search_symb(0, induct_list[j], lv, &coef);
	if (found >= 1)
	  il_lo[i].coefs[j] = coef;
	else if (found == 0)
	  il_lo[i].coefs[j] = 0;
	else if ((found == -1) ||
		 (found == -2))
	  il_lo[i].decidable = 0;

	found = search_symb(0, induct_list[j], rv, &coef);
	if (found >= 1)
	  il_hi[i].coefs[j] = coef;
	else if (found == 0)
	  il_hi[i].coefs[j] = 0;
	else if ((found == -1) ||
		 (found == -2))
	  il_hi[i].decidable = 0;
      }
      /* now normalize for stride */
      if (stride[i] != 1) {
	il_hi[i].offset =
	  (il_hi[i].offset - il_lo[i].offset) / stride[i];
	il_lo[i].offset = 0;
	for (j = 0; j < i; j++) {
	  il_hi[i].coefs[j] =
	    (il_hi[i].coefs[j] - il_lo[i].coefs[j]) / stride[i];
	  il_lo[i].coefs[j] = 0;
	}
      }
      return (i + 1);
    }
    else if (b->variant == WHILE_NODE) {
      if (i > MAX_NEST_DEPTH) {
	fprintf(stderr, " nest too deep ! \n");
	return (0);
      }
      induct_list[i] = search_inc_scalar(b);;
      /* now create low and hi bounds */
      il_lo[i].decidable = 0;
      il_hi[i].decidable = 0;
      for (j = 0; j < i; j++) {
	il_lo[i].coefs[j] = 0;
	il_hi[i].coefs[j] = 0;
      }

      return (i + 1);
    }
    else
      return (i);
  }
}
/* make_vect_range takes a pointer to a .. node */
/* for a vector reference and builds two	*/
/* subscript records.  One for the lo end the	*/
/* other for the hi end.  induct_list is	*/
/* the current active induction list.		*/
void make_vect_range(depth, p, induct_list, lo, hi)
PTR_LLND p;
PTR_SYMB induct_list[];
struct subscript *lo;
struct subscript *hi;
int depth;
{
  int i, j, found, coef;
  PTR_LLND lv, rv, plv, prv;
  PTR_REFL plist;		/* this is a dummy. need to add this as
				 * parameter */
  if (p->variant != DDOT)
    fprintf(stderr, "bad range node in vector\n");
  for (i = 0; i < MAX_NEST_DEPTH; i++) {
    lo->coefs[i] = 0;
    hi->coefs[i] = 0;
  }
  lo->offset = 0;
  hi->offset = 0;
  lv = p->entry.Template.ll_ptr1;
  rv = p->entry.Template.ll_ptr2;
  lo->decidable = 1;
  plist = NULL; 		/* ignore parametes in vector range for now */
  found = comp_offset(plist, induct_list, 1, lv, &coef, &plv);
  if (found >= 3)
    lo->decidable = 0;
  if (found != 0)
    lo->offset = coef;
  hi->decidable = 1;
  found = comp_offset(plist, induct_list, 1, rv, &coef, &prv);
  if (found >= 3)
    hi->decidable = 0;
  if (found != 0)
    hi->offset = coef;
  for (j = 0; j < i; j++) {
    found = search_symb(0, induct_list[j], lv, &coef);
    if (found >= 1)
      lo->coefs[j] = coef;
    else if (found == 0)
      lo->coefs[j] = 0;
    else if ((found == -1) ||
	     (found == -2))
      lo->decidable = 0;

    found = search_symb(0, induct_list[j], rv, &coef);
    if (found >= 1)
      hi->coefs[j] = coef;
    else if (found == 0)
      hi->coefs[j] = 0;
    else if ((found == -1) ||
	     (found == -2))
      hi->decidable = 0;
  }
  lo->offset = -lo->offset;
  for (i = 0; i < MAX_NEST_DEPTH; i++) {
    lo->coefs[i] = -lo->coefs[i];
  }
  lo->coefs[depth] = 1; 	/* perhaps repalce by stride ? */
  hi->coefs[depth] = -1;
}

/************************************************/
/* standard gcd routines: gcd of two vectors.	*/
/*  zeros are not counted.			*/
/************************************************/
int sgcd(a, b)
int a, b;
{
  int tmp;

  if (a < 0)
    a = -a;
  if (b < 0)
    b = -b;
  if (a > b) {
    tmp = b;
    b = a;
    a = tmp;
  }
  if (a == 0)
    return (b);
  else
    return (sgcd(a, b % a));
}

int gcd(d, x)
int d;
int x[];
{
  int i, g;
  g = 0;
  for (i = 0; i < d; i++) {
    g = sgcd(g, x[i]);
  }
  return (g);
}


void clean_loops(b)
PTR_BFND b;
{
  PTR_BLOB x;

  if (b == NULL)
    return ;
  else {
    switch (b->variant) {
     case GLOBAL:
     case PROG_HEDR:
     case PROC_HEDR:
     case FUNC_HEDR:
     case FOR_NODE:
     case FORALL_NODE:
     case WHILE_NODE:
      x = b->entry.Template.bl_ptr1;
      while (x != NULL && x->ref != b) {
	clean_loops(x->ref);
	if (x->next != NULL &&
	    x->next->ref == b)
	  x->next = NULL;
	x = x->next;
      }
      break;
     case IF_NODE:
      x = b->entry.if_node.control_true;
      while (x != NULL) {
	clean_loops(x->ref);
	if (x->next != NULL &&
	    x->next->ref == b)
	  x->next = NULL;
	x = x->next;
      }
      x = b->entry.if_node.control_false;;
      while (x != NULL) {
	clean_loops(x->ref);
	if (x->next != NULL &&
	    x->next->ref == b)
	  x->next = NULL;
	x = x->next;
      }
      break;
     default:
      break;
    }
  }
}



