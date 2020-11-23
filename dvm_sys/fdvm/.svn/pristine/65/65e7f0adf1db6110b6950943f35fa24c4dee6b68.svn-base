/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: symb_alg.c */

#include "db.h"

extern PTR_LLND make_llnd();
extern PTR_FILE cur_file;

/*
 * The following routines are used to evaluate low level expressions
 */

int get_symbs(n, p, s)
PTR_LLND p;
PTR_SYMB s[];
int n;
{
  int i;

  if (p == NULL)
    return (n);
  if (p->variant == VAR_REF) {
    for (i = 0; i < n; i++)
      if (s[i] == p->entry.Template.symbol)
	break;
    if (i == n) {
      s[n++] = p->entry.Template.symbol;
    }
  }
  n = get_symbs(n, p->entry.Template.ll_ptr1, s);
  n = get_symbs(n, p->entry.Template.ll_ptr2, s);
  return (n);
}

int eval_exp(p, s, vals, n, valu)	/* returns 0 on failure */
int n;
PTR_LLND p;
PTR_SYMB s[];
int vals[];
int *valu;
{
  int i, lv, rv, rs, ls;

  if (p == NULL)
    return (0);
  if (p->variant == INT_VAL) {
    *valu = p->entry.ival;
    return (1);
  }
  if (p->variant == VAR_REF) {
    for (i = 0; i < n; i++)
      if (s[i] == p->entry.Template.symbol) {
	*valu = vals[i];
	return (1);
      }
    return (0);
  }
  lv = 0;
  rv = 0;
  rs = 0;
  ls = 0;
  rs = eval_exp(p->entry.Template.ll_ptr2, s, vals, n, &rv);
  ls = eval_exp(p->entry.Template.ll_ptr1, s, vals, n, &lv);

  switch (p->variant) {
   case MINUS_OP:
    *valu = -lv;
    break;
   case ADD_OP:
    *valu = lv + rv;
    break;
   case MULT_OP:
    *valu = lv * rv;
    break;
   case DIV_OP:
    *valu = (rv != 0) ? lv / rv : 0;
    break;
   case SUBT_OP:
    *valu = lv - rv;
    break;
   default:
    fprintf(stderr, "bad op: %d\n", p->variant);
    return (0);
    
  }
  if (p->variant != MINUS_OP)
    return (rs * ls);
  else
    return (ls);
}

/* returns 1 if p and q are constant or linear in the same var */
/* and 0 otherwise.  result = 1 if p is less than q for a large value */
/* and result = 0 otherwise */
int numerical_less(p, q, result)
PTR_LLND p, q;
int *result;
{
  PTR_SYMB psyms[20], qsyms[20];
  int pvals[20], qvals[20];
  int pn, qn, pv, qv, ps, qs;

  pn = 0;
  qn = 0;
  pv = 0;
  qv = 0;
  qs = 0;
  ps = 0;
  pn = get_symbs(pn, p, psyms);
  qn = get_symbs(qn, q, qsyms);
  if (pn > 1 || qn > 1)
    return (0);
  if (pn == 1 && qn == 1 && psyms[0] != qsyms[0])
    return (0);
  pvals[0] = 512;
  qvals[0] = 512;
  ps = eval_exp(p, psyms, pvals, pn, &pv);
  qs = eval_exp(q, qsyms, qvals, qn, &qv);
  if (ps * qs == 0)
    return (0);
  *result = (pv < qv) ? 1 : 0;
  return (1);
}


int less(p, q)
PTR_LLND p, q;
{
  char *name1, *name2;
  int i;

  if (p->variant == MINUS_OP)
    p = p->entry.Template.ll_ptr1;
  if (q->variant == MINUS_OP)
    q = q->entry.Template.ll_ptr1;
  if (q->variant == INT_VAL) {
    if (p->variant == INT_VAL) {
      if (p->entry.ival < q->entry.ival)
	return (1);
      else
	return (0);
    }
    else
      return (1);
  }
  if (p->variant == INT_VAL)
    return (0);
  if (p->variant == VAR_REF && q->variant == VAR_REF) {
    name1 = p->entry.Template.symbol->ident;
    name2 = q->entry.Template.symbol->ident;
    i = 0;
    while (name1[i] != '\0' && name2[i] != '\0') {
      if (name1[i] > name2[i])
	return (0);
      if (name1[i] < name2[i])
	return (1);
      i++;
    }
    if (name1[i] == '\0' && name2[i] != '\0')
      return (1);
    else
      return (0);
  }
  if (p->variant == VAR_REF)
    return (1);
  if (q->variant == VAR_REF)
    return (0);
  return (0);
}

int rest_constant(p)
PTR_LLND p;
{
  if (p == NULL)
    return (1);
  if (p->variant == INT_VAL)
    return (1);
  if (p->variant == MINUS_OP)
    return (rest_constant(p->entry.Template.ll_ptr1));
  if (p->variant == MULT_OP)
    return (rest_constant(p->entry.Template.ll_ptr1) *
	    rest_constant(p->entry.Template.ll_ptr2));
  if (p->variant == DIV_OP)
    return (rest_constant(p->entry.Template.ll_ptr1) *
	    rest_constant(p->entry.Template.ll_ptr2));
  return (0);
}


int term_less(p, q)
PTR_LLND p, q;
{
  PTR_LLND p_rchld, q_rchld;

  /* assume in normal form */
  if (p == NULL && q == NULL)
    return (0);
  if (p == NULL)
    return (1);
  if (q == NULL)
    return (0);
  if (p->variant == MINUS_OP)
    p = p->entry.Template.ll_ptr1;
  if (q->variant == MINUS_OP)
    q = q->entry.Template.ll_ptr1;
  if (p->variant == DIV_OP && q->variant == DIV_OP) {
    p_rchld = p->entry.Template.ll_ptr2;
    q_rchld = q->entry.Template.ll_ptr2;
    if (less(p_rchld, q_rchld))
      return (1);
    if (less(q_rchld, p_rchld))
      return (0);
    /* must be equal */
    return (term_less(p->entry.Template.ll_ptr1,
		      q->entry.Template.ll_ptr1));
  }
  if (p->variant == DIV_OP && q->variant != DIV_OP) {
    if (rest_constant(p->entry.Template.ll_ptr1))
      return (term_less(p->entry.Template.ll_ptr2, q));
  }
  if (p->variant == MULT_OP && q->variant != MULT_OP) {
    if (rest_constant(p->entry.Template.ll_ptr1))
      return (term_less(p->entry.Template.ll_ptr2, q));
  }
  if (p->variant != DIV_OP && q->variant == DIV_OP) {
    if (rest_constant(q->entry.Template.ll_ptr1))
      return (term_less(p, q->entry.Template.ll_ptr2));
  }
  if (p->variant != MULT_OP && q->variant == MULT_OP) {
    if (rest_constant(q->entry.Template.ll_ptr1))
      return (term_less(p, q->entry.Template.ll_ptr2));
  }
  if (p->variant == MULT_OP && q->variant == MULT_OP) {
    p_rchld = p->entry.Template.ll_ptr2;
    q_rchld = q->entry.Template.ll_ptr2;
    if (less(p_rchld, q_rchld))
      return (1);
    if (less(q_rchld, p_rchld))
      return (0);
    /* must be equal */
    return (term_less(p->entry.Template.ll_ptr1, q->entry.Template.ll_ptr1));
  }
  /* both not mult */
  return (less(p, q));
}

void sort_term(p)
PTR_LLND p;
{
  int notdone;
  PTR_LLND q;
  PTR_LLND lchild, rchild, gchild;

  if(p == NULL) return;
  if (p->variant == MINUS_OP)
    p = p->entry.Template.ll_ptr1;
  if (p->variant != MULT_OP && p->variant != DIV_OP)
    return;
  notdone = 1;
  while (notdone) {
    q = p;
    notdone = 0;
    while (q != NULL && q->entry.Template.ll_ptr1 != NULL) {
      lchild = q->entry.Template.ll_ptr1;
      rchild = q->entry.Template.ll_ptr2;
      if(lchild == NULL || rchild == NULL) return;
      if (lchild->variant == INT_VAL && rchild->variant == INT_VAL) {
	notdone = 1;
	if (q->variant == SUBT_OP)
	  q->entry.ival = lchild->entry.ival - rchild->entry.ival;
	else if (q->variant == ADD_OP)
	  q->entry.ival = rchild->entry.ival + lchild->entry.ival;
	else if (q->variant == MULT_OP)
	  q->entry.ival = rchild->entry.ival * lchild->entry.ival;
	else if (q->variant == DIV_OP &&
		 rchild->entry.ival != 0)
	  q->entry.ival = lchild->entry.ival / rchild->entry.ival;
	else
	  q->entry.ival = 888888;
	q->variant = INT_VAL;
	/* better dispose of lchild and rchild later */
	q->entry.Template.ll_ptr1 = NULL;
	q->entry.Template.ll_ptr2 = NULL;
      }
      else if ((q->variant == MULT_OP &&
		lchild->variant != MULT_OP && lchild->variant != DIV_OP)
	       && less(lchild, rchild)) {
	notdone = 1;
	q->entry.Template.ll_ptr1 = rchild;
	q->entry.Template.ll_ptr2 = lchild;
      }
      else if (q->variant == MULT_OP && lchild->variant == MULT_OP) {
	gchild = lchild->entry.Template.ll_ptr2;
	if (rchild->variant == INT_VAL && gchild->variant == INT_VAL) {
	  notdone = 1;
	  rchild->entry.ival = rchild->entry.ival * gchild->entry.ival;
	  q->entry.Template.ll_ptr1 = lchild->entry.Template.ll_ptr1;
	}
	else if (less(gchild, rchild)) {
	  notdone = 1;
	  q->entry.Template.ll_ptr2 = gchild;
	  lchild->entry.Template.ll_ptr2 = rchild;
	}
      }
      q = q->entry.Template.ll_ptr1;
    }
  }
}

void sort_exp(p)
PTR_LLND p;
{
  int notdone, var;
  PTR_LLND q, q1;
  PTR_LLND lchild, rchild, gchild;

  q = p;
  while (q != NULL && (q->variant != ADD_OP && q->variant != SUBT_OP)) {
    if (q != NULL && (q->variant == MULT_OP || q->variant == DIV_OP))
      sort_term(q);
    if (q->variant == DIV_OP) {
      if (q->entry.Template.ll_ptr1->variant == ADD_OP ||
	  q->entry.Template.ll_ptr1->variant == SUBT_OP)
	sort_exp(q->entry.Template.ll_ptr1);
      if (q->entry.Template.ll_ptr2->variant == ADD_OP ||
	  q->entry.Template.ll_ptr2->variant == SUBT_OP)
	sort_exp(q->entry.Template.ll_ptr2);
    }
    q = q->entry.Template.ll_ptr1;
  }
  q1 = q;
  if (q1 == NULL)
    return;

  while (q != NULL) {
    if (q->variant == ADD_OP || q->variant == SUBT_OP)
      sort_term(q->entry.Template.ll_ptr2);
    else if (q->variant == MULT_OP || q->variant == DIV_OP)
      sort_term(q);
    if (q->variant == ADD_OP || q->variant == SUBT_OP)
      q = q->entry.Template.ll_ptr1;
    else
      q = NULL;
  }

  notdone = 1;
  q = q1;
  while (notdone) {
    q = p;
    notdone = 0;
    while (q != NULL && q->variant != MULT_OP && q->variant != DIV_OP &&
	   q->entry.Template.ll_ptr1 != NULL) {
      lchild = q->entry.Template.ll_ptr1;
      rchild = q->entry.Template.ll_ptr2;
      if(lchild == NULL || rchild == NULL) return; /* should never happen! */
      if (lchild->variant == INT_VAL && rchild->variant == INT_VAL) {
	var = q->variant;
	q->variant = INT_VAL;
	if (var == ADD_OP)
	  q->entry.ival = lchild->entry.ival + rchild->entry.ival;
	else
	  q->entry.ival = lchild->entry.ival - rchild->entry.ival;

	q->entry.Template.ll_ptr1 = NULL;
	q->entry.Template.ll_ptr2 = NULL;
	notdone = 1;
      }
      else if ((lchild->variant != ADD_OP && lchild->variant != SUBT_OP)
	       && term_less(lchild, rchild)) {
	notdone = 1;
	q->entry.Template.ll_ptr1 = rchild;
	q->entry.Template.ll_ptr2 = lchild;
	if (q->variant == SUBT_OP) {
	  q->variant = ADD_OP;
	  lchild = make_llnd(cur_file, INT_VAL, NULL, NULL, 0);
	  q->entry.Template.ll_ptr1=make_llnd(cur_file,SUBT_OP,lchild,rchild,
					      NULL);
	}
      }
      else if (lchild->variant == ADD_OP || lchild->variant == SUBT_OP) {
	gchild = lchild->entry.Template.ll_ptr2;
	if (term_less(gchild, rchild)) {
	  notdone = 1;
	  q->entry.Template.ll_ptr2 = gchild;
	  lchild->entry.Template.ll_ptr2 = rchild;
	  var = q->variant;
	  q->variant = lchild->variant;
	  lchild->variant = var;
	}
      }
      q = q->entry.Template.ll_ptr1;
    }
  }
}

PTR_LLND copy_llnd(p)
PTR_LLND p;
{
  PTR_LLND newp;

  if (p == NULL)
    return (NULL);
  newp = make_llnd(cur_file, p->variant, NULL, NULL, p->entry.Template.symbol);
  newp->entry.Template.ll_ptr1 = copy_llnd(p->entry.Template.ll_ptr1);
  newp->entry.Template.ll_ptr2 = copy_llnd(p->entry.Template.ll_ptr2);
  return (newp);
}

int integer_difference(p,q, value, dif)
PTR_LLND p,q, *dif;
int *value;
{
   PTR_LLND s;
   void simplify(), normal_form();

   s = make_llnd(cur_file, SUBT_OP, copy_llnd(p),copy_llnd(q), NULL);
   normal_form(&s);
   *dif = s;
   if(s->variant == INT_VAL){
	 *value = s->entry.ival;
	 return 1;
	 }
   else if (s->variant == MINUS_OP){
	 s = s->entry.Template.ll_ptr1;
	 *value = -s->entry.ival;
	 return 1;
	 }
   return 0;
}

int no_division(p)
PTR_LLND p;
{
  return (1);
#if 0
  while (p != NULL && p->variant == MULT_OP)
    p = p->entry.Template.ll_ptr1;
  if (p == NULL)
    return (1);
  if (p->variant == DIV_OP)
    return (0);
  return (1);
#endif
}


void expand(p)
PTR_LLND p;
{
  PTR_LLND lson, rson, lgchld, rgchld, cpy, new;
  if (p == NULL)
    return;

  if (p->variant == MULT_OP) {
    lson = p->entry.Template.ll_ptr1;
    rson = p->entry.Template.ll_ptr2;
    if (lson->variant == MULT_OP) {
      expand(p->entry.Template.ll_ptr1);
      lson = p->entry.Template.ll_ptr1;
    }
    if (rson->variant == MULT_OP) {
      expand(p->entry.Template.ll_ptr2);
      rson = p->entry.Template.ll_ptr2;
    }
    if ((lson->variant == ADD_OP || lson->variant == SUBT_OP)) {
      lgchld = lson->entry.Template.ll_ptr1;
      rgchld = lson->entry.Template.ll_ptr2;
      cpy = copy_llnd(rson);
      new = make_llnd(cur_file, MULT_OP, rgchld, rson, NULL);
      lson->entry.Template.ll_ptr1 = lgchld;
      lson->entry.Template.ll_ptr2 = cpy;
      p->entry.Template.ll_ptr2 = new;
      p->variant = lson->variant;
      lson->variant = MULT_OP;
    }
    else if ((rson->variant == ADD_OP || rson->variant == SUBT_OP) &&
	     no_division(rson->entry.Template.ll_ptr2) &&
	     no_division(rson->entry.Template.ll_ptr1)) {
      lgchld = rson->entry.Template.ll_ptr1;
      rgchld = rson->entry.Template.ll_ptr2;
      cpy = copy_llnd(lson);
      new = make_llnd(cur_file, MULT_OP, lson, lgchld, NULL);
      rson->entry.Template.ll_ptr1 = cpy;
      rson->entry.Template.ll_ptr2 = rgchld;

      p->entry.Template.ll_ptr1 = new;
      p->variant = rson->variant;
      rson->variant = MULT_OP;
    }
  }
  expand(p->entry.Template.ll_ptr2);
  expand(p->entry.Template.ll_ptr1);
}

void left_allign_term(p)	/* need fix for divide, similar to - fix
				 * below */
PTR_LLND *p;
{
  PTR_LLND root_rc, tail_r_chain, last_r_chain, q;
  if (*p == NULL)
    return;
  if ((*p)->variant == MULT_OP) {
    if ((*p)->entry.Template.ll_ptr2->variant != DIV_OP)
      left_allign_term(&((*p)->entry.Template.ll_ptr2));
    left_allign_term(&((*p)->entry.Template.ll_ptr1));

    /* now link these together */

    root_rc = (*p)->entry.Template.ll_ptr2;
    q = root_rc;
    last_r_chain = NULL;
    while (q->variant == MULT_OP /* || q->variant == DIV_OP */ ) {
      last_r_chain = q;
      q = q->entry.Template.ll_ptr1;
    }
    tail_r_chain = q;
    if (root_rc == tail_r_chain)
      return;
    last_r_chain->entry.Template.ll_ptr1 = *p;
    (*p)->entry.Template.ll_ptr2 = tail_r_chain;
    *p = root_rc;
  }
  if ((*p)->variant == DIV_OP) {
    left_allign_term(&((*p)->entry.Template.ll_ptr1));
    left_allign_term(&((*p)->entry.Template.ll_ptr2));
  }
  return;
}


void left_allign_exp(p)
PTR_LLND *p;
{
  PTR_LLND root_rc, tail_r_chain, last_r_chain, q;

  if (*p == NULL)
    return;
  if ((*p)->variant == ADD_OP || (*p)->variant == SUBT_OP) {
    left_allign_exp(&((*p)->entry.Template.ll_ptr1));
    left_allign_exp(&((*p)->entry.Template.ll_ptr2));

    /* now link these together */

    root_rc = (*p)->entry.Template.ll_ptr2;
    if(root_rc == NULL) return;
    if ((*p)->variant == SUBT_OP) {
      for (q = root_rc; q != NULL &&
	   (q->variant == ADD_OP || q->variant == SUBT_OP);
	   q = q->entry.Template.ll_ptr1)
	if (q->variant == SUBT_OP)
	  q->variant = ADD_OP;
	else if (q->variant == ADD_OP)
	  q->variant = SUBT_OP;
    }
    q = root_rc;
    last_r_chain = NULL;
    while (q->variant == ADD_OP || q->variant == SUBT_OP) {
      last_r_chain = q;
      q = q->entry.Template.ll_ptr1;
    }
    tail_r_chain = q;
    if (root_rc == tail_r_chain)
      return;
    last_r_chain->entry.Template.ll_ptr1 = *p;
    (*p)->entry.Template.ll_ptr2 = tail_r_chain;
    *p = root_rc;
  }
  else if ((*p)->variant == MULT_OP || (*p)->variant == DIV_OP) {
    left_allign_term(p);
  }
  else {
    left_allign_exp(&((*p)->entry.Template.ll_ptr1));
    left_allign_exp(&((*p)->entry.Template.ll_ptr2));
  }
  return;
}


void clear_unary_minus(p)
PTR_LLND p;
{
  PTR_LLND after_minus;

  while (p != NULL &&
	 p->variant != ADD_OP && p->variant != SUBT_OP)
    p = p->entry.Template.ll_ptr1;
  if (p == NULL)
    return;
  if (p->variant == ADD_OP || p->variant == SUBT_OP) {
    if (p->entry.Template.ll_ptr2->variant == MINUS_OP) {
      after_minus =
	p->entry.Template.ll_ptr2->entry.Template.ll_ptr1;
      p->entry.Template.ll_ptr2 = after_minus;
      if (p->variant == ADD_OP)
	p->variant = SUBT_OP;
      else
	p->variant = ADD_OP;
    }
    clear_unary_minus(p->entry.Template.ll_ptr1);
  }
}

int get_term_coef(p)
PTR_LLND p;
{
  int sign, lval;

  sign = 1;
  while (p != NULL && p->variant == MINUS_OP) {
    p = p->entry.Template.ll_ptr1;
    sign = -sign;
  }
  if (p == NULL)
    return (sign);
  if (p->variant == ADD_OP || p->variant == SUBT_OP)
    /* should only happen with division as parent */
    return (1);
  if (p->variant == VAR_REF)
    return (sign);
  if (p->variant == INT_VAL)
    return (sign * p->entry.ival);
  if (p->variant == MULT_OP) {
    lval = sign * get_term_coef(p->entry.Template.ll_ptr1);
    if (p->entry.Template.ll_ptr2->variant == INT_VAL)
      return (lval * p->entry.Template.ll_ptr2->entry.ival);
    else
      return (lval);
  }
  if (p->variant == DIV_OP) {
    return (sign);
  }
  else {
    fprintf(stderr, "bad coeficient extraction in get_term_coef\n");
    return (1);
  }
}


void replace_coef(p, v)
PTR_LLND p;
int v;
{
  PTR_LLND new_int, new_var, q;
  if (p == NULL) {
    fprintf(stderr, "replace_coef failed\n");
    return;
  }
  if (p->variant == INT_VAL) {
    p->entry.ival = v;
    return;
  }
  if (p->variant == ADD_OP || p->variant == SUBT_OP) {
    if (v == 1)
      return;
    replace_coef(p->entry.Template.ll_ptr1, v);
    replace_coef(p->entry.Template.ll_ptr2, v);
    return;
  }
  if (p->variant == VAR_REF) {
    if (v == 1)
      return;
    p->variant = MULT_OP;
    new_int = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
    new_int->entry.ival = v;
    new_var = make_llnd(cur_file, VAR_REF,NULL,NULL,p->entry.Template.symbol);
    p->entry.Template.ll_ptr1 = new_int;
    p->entry.Template.ll_ptr2 = new_var;
    p->entry.Template.symbol = NULL;
    return;
  }
  else if (v == 1 && p->variant == MULT_OP &&
	   rest_constant(p->entry.Template.ll_ptr1)) {
    new_var = p->entry.Template.ll_ptr2;
    p->variant = new_var->variant;
    p->entry.Template.symbol = new_var->entry.Template.symbol;
    p->entry.Template.ll_ptr1 = new_var->entry.Template.ll_ptr1;
    p->entry.Template.ll_ptr2 = new_var->entry.Template.ll_ptr2;
  }
  else if (p->variant == MULT_OP &&
	   p->entry.Template.ll_ptr1->variant == DIV_OP)
    replace_coef(p->entry.Template.ll_ptr2, v);
  else if (p->variant == DIV_OP) {
    if (v == 1)
      return;
    q = make_llnd(cur_file, DIV_OP, p->entry.Template.ll_ptr1,
		  p->entry.Template.ll_ptr2, NULL);
    p->entry.Template.ll_ptr1 = q;
    p->variant = MULT_OP;
    new_int = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
    new_int->entry.ival = v;
    p->entry.Template.ll_ptr2 = new_int;
  }
  else
    replace_coef(p->entry.Template.ll_ptr1, v);
}


int identical(p, q)
PTR_LLND p, q;
{
  if (q == NULL && p == NULL)
    return (1);
  if (q == NULL && p != NULL)
    return (0);
  if (q != NULL && p == NULL)
    return (0);

  /* now p and q not null */
  if (p->variant != q->variant)
    return (0);
  switch (p->variant) {
   case VAR_REF:
    return (p->entry.Template.symbol == q->entry.Template.symbol);
    
   case ARRAY_REF:
    if (p->entry.Template.symbol != q->entry.Template.symbol)
      return (0);
    else
      return (identical(q->entry.Template.ll_ptr1,
			p->entry.Template.ll_ptr1) *
	      identical(q->entry.Template.ll_ptr2,
			p->entry.Template.ll_ptr2));
    
   case INT_VAL:
    return (p->entry.ival == q->entry.ival);
    
   default:
    return (identical(q->entry.Template.ll_ptr1,
		      p->entry.Template.ll_ptr1) *
	    identical(q->entry.Template.ll_ptr2,
		      p->entry.Template.ll_ptr2));
    
  }
}


int same_upto_coef(p, q)
PTR_LLND p, q;
{
  PTR_LLND plc, prc, qlc, qrc;
  if (p == NULL && q == NULL)
    return (1);
  if (p == NULL)
    return (0);
  if (q == NULL)
    return (0);
  if (p->variant == MINUS_OP)
    p = p->entry.Template.ll_ptr1;
  if (q->variant == MINUS_OP)
    q = q->entry.Template.ll_ptr1;
  if (rest_constant(p) && rest_constant(q))
    return (1);
  plc = p->entry.Template.ll_ptr1;
  prc = p->entry.Template.ll_ptr2;
  qlc = q->entry.Template.ll_ptr1;
  qrc = q->entry.Template.ll_ptr2;
  if (p->variant == VAR_REF) {
    if (q->variant == VAR_REF) {
      if (p->entry.Template.symbol == q->entry.Template.symbol)
	return (1);
      else
	return (0);
    }
    else if (q->variant == MULT_OP || q->variant == DIV_OP) {
      if (rest_constant(qlc) &&
	  qrc->variant == VAR_REF &&
	  qrc->entry.Template.symbol == p->entry.Template.symbol
	)
	return (1);
      else
	return (0);
    }
    else
      return (0);
  }
  else if (q->variant == VAR_REF) {
    if (p->variant == MULT_OP || p->variant == DIV_OP) {
      if (rest_constant(plc) &&
	  prc->variant == VAR_REF &&
	  prc->entry.Template.symbol == q->entry.Template.symbol
	)
	return (1);
      else
	return (0);
    }
    else
      return (0);
  }
  else if ((p->variant == ADD_OP && q->variant == ADD_OP) ||
	   (p->variant == SUBT_OP && q->variant == SUBT_OP) ||
	   (p->variant == DIV_OP && q->variant == DIV_OP))
    return (identical(p, q));
  else if (p->variant == MULT_OP && q->variant == DIV_OP) {
    if ( (rest_constant(prc) && same_upto_coef(plc, q))
            ||
	     (rest_constant(plc) && same_upto_coef(prc, q)) )
      return (1);
    else
      return (0);
  }
  else if (q->variant == MULT_OP && p->variant == DIV_OP) {
    if ( (rest_constant(qrc) && same_upto_coef(qlc, p))
           ||
	     (rest_constant(qlc) && same_upto_coef(qrc, p)) )
      return (1);
    else
      return (0);
  }
  else if (p->variant == q->variant) {
    if (same_upto_coef(plc, qlc) && same_upto_coef(prc, qrc))
      return (1);
    else
      return (0);
  }
  else
    return (0);
}


void simplify(p)
PTR_LLND *p;
{
  PTR_LLND q, left, lower, right, qlast, qnext;
  PTR_LLND rec_nrm_frm();
  int not_done, val, var, vl, vr, lvar;

  /* clear_unary_minus(*p); */
  not_done = 1;

  if ((*p)->variant == MULT_OP || (*p)->variant == DIV_OP ||
      (*p)->variant == ADD_OP || (*p)->variant == SUBT_OP) {
    if((*p)->entry.Template.ll_ptr1 == NULL) return;
    if ((*p)->entry.Template.ll_ptr1->variant != VAR_REF &&
	(*p)->entry.Template.ll_ptr1->variant != INT_VAL)
      (*p)->entry.Template.ll_ptr1 =
	rec_nrm_frm((*p)->entry.Template.ll_ptr1);
    if((*p)->entry.Template.ll_ptr2 == NULL) return;
    if ((*p)->entry.Template.ll_ptr2->variant != VAR_REF &&
	(*p)->entry.Template.ll_ptr2->variant != INT_VAL)
      (*p)->entry.Template.ll_ptr2 =
	rec_nrm_frm((*p)->entry.Template.ll_ptr2);
  }

  while (not_done) {
    not_done = 0;
    q = *p;
    qlast = NULL;
    while (q != NULL && q->variant != MULT_OP && q->variant != DIV_OP &&
	   q->entry.Template.ll_ptr1 != NULL) {
      var = q->variant;
      if (var == ADD_OP || var == SUBT_OP) {
	right = q->entry.Template.ll_ptr2;
	left = q->entry.Template.ll_ptr1;
	if (left->variant != ADD_OP && left->variant != SUBT_OP) {
	  if (same_upto_coef(left, right)) {
	    not_done = 1;
	    vl = get_term_coef(left);
	    vr = get_term_coef(right);
	    if (var == ADD_OP)
	      val = vl + vr;
	    else
	      val = vl - vr;
	    if (val == 0) {
	      if (qlast != NULL) {
		qlast->entry.Template.ll_ptr1 =
		  make_llnd(cur_file, INT_VAL, NULL, NULL, 0);
	      }
	      else
		*p = make_llnd(cur_file, INT_VAL, NULL, NULL, 0);
	    }
	    else {
	      if (val < 0) {
		if (var == ADD_OP)
		  q->variant = SUBT_OP;
		else
		  q->variant = ADD_OP;
		val = -val;
	      }
	      replace_coef(right, val);
	      q->variant = right->variant;
	      if (right->variant != VAR_REF)
		q->entry.Template.symbol = NULL;
	      else
		q->entry.Template.symbol =
		  right->entry.Template.symbol;
	      q->entry.Template.ll_ptr1
		= right->entry.Template.ll_ptr1;
	      q->entry.Template.ll_ptr2
		= right->entry.Template.ll_ptr2;
	    }
	  }
	}
	else {
	  lvar = left->variant;
	  lower = left->entry.Template.ll_ptr2;
	  if (same_upto_coef(lower, right)) {
	    not_done = 1;
	    vl = get_term_coef(lower);
	    vr = get_term_coef(right);
	    if (var == ADD_OP)
	      val = vr;
	    else
	      val = -vr;
	    if (lvar == ADD_OP)
	      val = val + vl;
	    else
	      val = val - vl;
	    if (val == 0) {
	      if (qlast != NULL) {
		qlast->entry.Template.ll_ptr1 =
		  left->entry.Template.ll_ptr1;
	      }
	      else
		*p = left->entry.Template.ll_ptr1;
	    }
	    else {
	      q->variant = ADD_OP;
	      if (val >= 0)
		replace_coef(right, val);
	      else {
		replace_coef(right, -val);
		q->variant = SUBT_OP;
	      }
	      q->entry.Template.ll_ptr1 =
		left->entry.Template.ll_ptr1;
	    }
	  }
	}
      }
      qlast = q;
      q = q->entry.Template.ll_ptr1;
    }
  }				/* end of outer while */
  /* now eliminate left over 0 terms. */
  q = *p;
  qlast = NULL;
  qnext = NULL;
  while (q != NULL && ((qnext = q->entry.Template.ll_ptr1) != NULL)
	 && (q->variant == ADD_OP || q->variant == SUBT_OP)
	 && (qnext->variant == ADD_OP || qnext->variant == SUBT_OP)) {
    qlast = q;
    q = q->entry.Template.ll_ptr1;
  }
  if (qnext == NULL)
    return;
  if (qnext->variant == INT_VAL && qnext->entry.ival == 0) {
    if (q->variant == ADD_OP) {
      if (qlast != NULL) {
	qlast->entry.Template.ll_ptr1 =
	  q->entry.Template.ll_ptr2;
	/* dispose of q and qnext */
      }
      else {
	*p = q->entry.Template.ll_ptr2;
	/* dispose of q and qnext */
      }
    }
    else if (q->variant == SUBT_OP) {
      q->variant = MINUS_OP;
      q->entry.Template.ll_ptr1 =
	q->entry.Template.ll_ptr2;
      q->entry.Template.ll_ptr2 = NULL;
      /* dispose of qnext */
    }
  }

}


PTR_LLND
rec_nrm_frm(cp)
PTR_LLND cp;
{
  expand(cp);
  left_allign_exp(&cp);
  sort_exp(cp);
  simplify(&cp);
  return (cp);
}


void elim_stupid_expr_list(p)
PTR_LLND *p;
{
  if (*p == NULL)
    return;
  if ((*p)->variant == INT_VAL || (*p)->variant == VAR_REF)
    return;
  if ((*p)->variant == EXPR_LIST) {
    if ((*p)->entry.Template.ll_ptr2 == NULL)
      p = &((*p)->entry.Template.ll_ptr1);
    else
      return;
  }
  elim_stupid_expr_list(&((*p)->entry.Template.ll_ptr1));
  elim_stupid_expr_list(&((*p)->entry.Template.ll_ptr2));
}

PTR_LLND norm_frm_exp(p)
PTR_LLND p;
{
  PTR_LLND cp;

  cp = copy_llnd(p);
  elim_stupid_expr_list(&cp);
  return (rec_nrm_frm(cp));
}


void normal_form(p)
PTR_LLND *p;
{
  if (p == NULL)
    return;
  if (*p == NULL)
    return;
  switch ((*p)->variant) {
   case STAR_RANGE:
    break;
   case ARRAY_REF:
    normal_form(&((*p)->entry.Template.ll_ptr1));
    break;
   case RANGE_LIST:
   case EXPR_LIST:
    normal_form(&((*p)->entry.Template.ll_ptr1));
    normal_form(&((*p)->entry.Template.ll_ptr2));
    break;
   case DDOT:
    normal_form(&((*p)->entry.Template.ll_ptr1));
    normal_form(&((*p)->entry.Template.ll_ptr2));
    break;
   case ADD_OP:
   case SUBT_OP:
   case MULT_OP:
   case DIV_OP:
   case MINUS_OP:
   case VAR_REF:
   case INT_VAL:
    *p = norm_frm_exp(*p);
    break;
   default:
    fprintf(stderr, "bad case in normal_form %d\n", (*p)->variant);
    break;
  }
}
