/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: anal_ind.c */

/**********************************************************************/
/*  This file contains the routines called in sets.c that do all index*/
/*  and subscript analysis.					      */
/**********************************************************************/

#include <stdlib.h>
#include "db.h"
 
#define PLUS 2
#define ZPLUS 3
#define MINUS 4
#define ZMINUS 5
#define PLUSMINUS 6
#define NODEP -1

/* extern variables */
extern PTR_SYMB induct_list[MAX_NEST_DEPTH];
extern int stride[MAX_NEST_DEPTH];
extern int language;
extern PTR_FILE cur_file;

extern PCF UnparseBfnd[];
extern PCF UnparseLlnd[];


/* local variables */
struct subscript blank, extra;
int table_generated = 0;
int np = 2 * MAX_NEST_DEPTH;
int tbl_depth = 4 * MAX_NEST_DEPTH + AR_DIM_MAX;
int num_eqn, num_ineq;
int adm = MAX_NEST_DEPTH;
int *table[MAX_NEST_DEPTH * 4 + AR_DIM_MAX];
int upper_bnd[2 * MAX_NEST_DEPTH], lower_bnd[2 * MAX_NEST_DEPTH];
int dist_ub[2 * MAX_NEST_DEPTH], dist_lb[2 * MAX_NEST_DEPTH];

/* forward references */
PTR_SETS alloc_sets();
PTR_REFL alloc_ref();
int disp_refl();
PTR_REFL copy_refl();
PTR_REFL union_refl();
void add_eqn();
void set_troub();
void print_tbl();
void print_etbl();
void set_vec();
int simple_algebraic();
int reduce();
int solve_system();
int chk_bnds();

/* extern references */
int make_induct_list();
void make_subscr();
int reduce_ll_exp();
int sequiv();
int unif_gen(); 
int gcd();
void make_vect_range();

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

int check_for_indvar(s, d, lis)
PTR_SYMB s, lis[];

int d;
{
  int i;

  for (i = 0; i < d; i++)
    if (s == lis[i])
      return (1);
  return (0);
}

PTR_LLND append_ll_elist(PTR_LLND list, PTR_LLND item);

/*************************************************************/
/* find_bounds(b,q,qnew) takes a bifnode-llnd pair (b,q) and */
/*  creates a low level expression that describes the range  */
/* of values that are touched by the reference in the current*/
/* context.  the index expressions are all scalars and ranges*/
/* interms of parameters or constants.	if the index exp is  */
/* undecidable, then the whole range of the index is assumed */
/*  the parameter qnew is a low level list upon which this   */
/*  expression is appended.				     */
/*************************************************************/
PTR_LLND find_bounds(PTR_BFND b, PTR_LLND q, PTR_LLND qnew)
/*PTR_BFND b;*/
/*PTR_LLND q, qnew;*/
{
  PTR_SYMB ind_list[MAX_NEST_DEPTH];
  //PTR_LLND ind_terms[MAX_NEST_DEPTH];
  struct subscript il_lo[MAX_NEST_DEPTH];
  struct subscript il_hi[MAX_NEST_DEPTH];
  struct subscript source[AR_DIM_MAX];	/* a source reference or def. */
  int i, j, count, dumb,sign;
  struct ref sor;
  PTR_LLND qind_list, new_list, q_index, make_llnd(), tmp;
  PTR_LLND exp1, exp2, exp3, build_exp_from_bound();
  PTR_BFND fun;
  PTR_REFL parms;
  PTR_LLND copy_llnd();

  for (i = 0; i < MAX_NEST_DEPTH; i++) {
    ind_list[i] = NULL;
    //ind_terms[i] = NULL;
    for (j = 0; j < MAX_NEST_DEPTH; j++) {
      il_lo[i].coefs_symb[j] = NULL;
      il_hi[i].coefs_symb[j] = NULL;
    }
  }

  make_induct_list(b, ind_list, il_lo, il_hi);
  sor.stmt = b;
  sor.refer = q;
  make_subscr(&sor, source);	/* source is an array of */
				/* subscript records that */
				/* shared by all routines */
				/* find the parameter list */
  fun = b;
  while ((fun->variant != PROG_HEDR) &&
	 (fun->variant != FUNC_HEDR) &&
	 (fun->variant != PROC_HEDR))
    fun = fun->control_parent;
  parms = fun->entry.Template.sets->in_def;

  qind_list = q->entry.Template.ll_ptr1;
  new_list = NULL;
  i = 0;
  while (qind_list != NULL) {
    q_index = qind_list->entry.Template.ll_ptr1;
    if (source[i].decidable == 2) {	/* ddot case */
      PTR_LLND low, hi, ar1, ar2, rl1, rl2, ltmp, htmp;
      /* skip stride for now */
      if (q_index->variant == DDOT && q_index->entry.Template.ll_ptr1 != NULL 
	  && q_index->entry.Template.ll_ptr1->variant == DDOT)
	q_index = q_index->entry.Template.ll_ptr1;
      if (q_index->variant == STAR_RANGE) {
	rl1 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
      }
      else {
	low = copy_llnd(q_index->entry.Template.ll_ptr1);
	hi = copy_llnd(q_index->entry.Template.ll_ptr2);

	rl1 = make_llnd(cur_file, EXPR_LIST, low, NULL, NULL);
	rl2 = make_llnd(cur_file, EXPR_LIST, hi, NULL, NULL);
	ar1 = make_llnd(cur_file,ARRAY_REF,rl1,NULL, q->entry.Template.symbol);
	ar2 = make_llnd(cur_file,ARRAY_REF,rl2,NULL, q->entry.Template.symbol);
	ltmp = find_bounds(b, ar1, NULL);
	htmp = find_bounds(b, ar2, NULL);
	ltmp = ltmp->entry.Template.ll_ptr1;
	htmp = htmp->entry.Template.ll_ptr1;
	
	if (ltmp!= NULL && (ltmp->variant == EXPR_LIST || ltmp->variant == EXPR_LIST))
	  ltmp = ltmp->entry.Template.ll_ptr1;
	if (htmp!= NULL && (htmp->variant == EXPR_LIST || htmp->variant == EXPR_LIST))
	  htmp = htmp->entry.Template.ll_ptr1;
	if(ltmp == NULL) low =	make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL); 
	else if (ltmp->variant == DDOT)
	  low = ltmp->entry.Template.ll_ptr1;
	else
	  low = ltmp;
	if(htmp == NULL) hi =  make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	else if (htmp->variant == DDOT) {
	  hi = htmp->entry.Template.ll_ptr2;
	  if (hi->variant == DDOT)
	    hi = hi->entry.Template.ll_ptr1;
	}
	else
	  hi = htmp;
	if (low->variant == STAR_RANGE)
	  rl1 = low;
	else if (hi->variant == STAR_RANGE)
	  rl1 = hi;
	else {
	  rl1->variant = DDOT;
	  rl1->entry.Template.ll_ptr1 = low;
	  rl1->entry.Template.ll_ptr2 = hi;
	}
      }
      new_list = append_ll_elist(new_list, rl1);
    }
    else if (source[i].decidable == 0) {	/* parm */
      if (q_index == NULL || q_index->variant == STAR_RANGE) {
	exp3 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	new_list = append_ll_elist(new_list, exp3);
      }
      else if (reduce_ll_exp(b, parms, ind_list, q_index, &exp2, &dumb) == 0) {
	/* was not able to resolve */
	if (simple_algebraic(q_index)) {
	  sign = 1;
	  exp1 = build_exp_from_bound(il_lo, &(source[i]),&sign);
	  if (exp1 == NULL) {
	    /* this should only happen if the subscript */
	    /* is very strange. 		    */
	  }
	  if (reduce_ll_exp(b, parms, ind_list, exp1, &exp2, &dumb) == 0) {
	    /* was not able to resolve ! */
	    exp3 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	  }
	  else {
	    exp1 = exp2;
	    count = 0;
	    for (j = 0; j < MAX_NEST_DEPTH; j++)
	      if (source[i].coefs[j] != 0)
		count++;
	    if (count == 0)
	      exp3 = exp1;
	    else {
	      sign = 1;
	      exp2 = build_exp_from_bound(il_hi, &(source[i]),&sign);
	       if (reduce_ll_exp(b, parms, ind_list, exp2, &exp3, &dumb) == 0) {
		exp3 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	      }
	      else {
		exp2 = exp3;
		if(sign > 0)
		   exp3 = make_llnd(cur_file, DDOT, exp1, exp2, NULL);
		else
		   exp3 = make_llnd(cur_file, DDOT, exp2, exp1, NULL);
	      }
	    }
	  }
	  new_list = append_ll_elist(new_list, exp3);
	}
	else {
	  tmp = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	  new_list = append_ll_elist(new_list, tmp);
	}
      }
      else
	new_list = append_ll_elist(new_list, exp2);
    }
    else if (source[i].decidable == 1) {	/* standard linear */
      sign = 1;
      exp1 = build_exp_from_bound(il_lo, &(source[i]),&sign);
       if (exp1 == NULL) {
	/* fprintf(stderr, "OOPS null!\n"); */
	/* this should only happen if the subscript */
	/* is very strange.  or the low bound is strange   */
      }
      if (reduce_ll_exp(b, parms, ind_list, exp1, &exp2, &dumb) == 0) {
	/* was not able to resolve ! */
	exp3 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
      }
      else {
	exp1 = exp2;
	count = 0;
	for (j = 0; j < MAX_NEST_DEPTH; j++)
	  if (source[i].coefs[j] != 0
	      || source[i].coefs_symb[j] != NULL)
	    count++;
	if (count == 0)
	  exp3 = exp1;
	else {
	  sign = 1;
	  exp2 = build_exp_from_bound(il_hi, &(source[i]),&sign);
	   if (reduce_ll_exp(b, parms, ind_list, exp2, &exp3, &dumb) == 0) {
	    exp3 = make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL);
	  }
	  else {
	    exp2 = exp3;
	    if(sign> 0)
	      exp3 = make_llnd(cur_file, DDOT, exp1, exp2, NULL);
	    else
	      exp3 = make_llnd(cur_file, DDOT, exp2, exp1, NULL);
	  }
	}
      }
      new_list = append_ll_elist(new_list, exp3);
    }
    else {
      fprintf(stderr, "source[i].decidable = %d\n", source[i].decidable);
      fprintf(stderr, "strange brew in find_bounds %s\n",
	      (UnparseLlnd[cur_file->lang])(q_index));
      new_list = append_ll_elist(new_list, q_index);
    }
    qind_list = qind_list->entry.Template.ll_ptr2;
    i++;
  }
  if (qnew != NULL)
    qnew->entry.Template.ll_ptr1 = new_list;
  else
    qnew = new_list;
  return (qnew);
}


int simple_algebraic(p)
PTR_LLND p;
{
  if (p == NULL)
    return (1);
  switch (p->variant) {
   case EXPR_LIST:
   case ADD_OP:
   case DIV_OP:
   case MULT_OP:
   case SUBT_OP:
   case MINUS_OP:
    return (simple_algebraic(p->entry.Template.ll_ptr1) *
	    simple_algebraic(p->entry.Template.ll_ptr2));
   case VAR_REF:
   case CONST_REF:
   case INT_VAL:
    return (1);
   default:
    return (0);
  }
}

PTR_LLND append_ll_elist(list, item)
PTR_LLND list, item;
{
  PTR_LLND tmp, make_llnd();

  if (list == NULL) {
    tmp = make_llnd(cur_file, EXPR_LIST, item, NULL, NULL);
    return (tmp);
  }
  if (list->variant != EXPR_LIST) {
    fprintf(stderr, "append_ll_elist screw up\n");
    return (list);
  }
  else if (list->entry.list.next == NULL) {
    tmp = append_ll_elist(NULL, item);
    list->entry.list.next = tmp;
    return (list);
  }
  else {
    append_ll_elist(list->entry.list.next, item);
    return (list);
  }
}

PTR_LLND build_exp_from_bound(il, sub, sign)
struct subscript il[MAX_NEST_DEPTH];
struct subscript *sub;
int *sign;
{
  PTR_LLND exp, exp2, exp3, exp4, make_llnd();
  int j;
  
  if (sub->decidable == 2) {	/* ddot case */
    return (sub->vector);
  }
  if (sub->decidable == 0 /* && simple_algebraic(sub->parm_exp) == 0 */ ) {
    /* parameter expression (we hope) */
    /* first we need to check for other vars */
    return (sub->parm_exp);
  }
  if (sub->decidable == 1) {  /* standard linear */
    exp = NULL;
    if (sub->parm_exp == NULL) {
      exp = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
      exp->entry.ival = sub->offset;
    }
    else
      exp = sub->parm_exp;
    for (j = 0; j < MAX_NEST_DEPTH; j++) {
      if (sub->coefs_symb[j] != NULL) { /* symbolic case! */
	exp3 = build_exp_from_bound(il, &(il[j]), sign);
	if (exp3 == NULL) {
	  exp4 = NULL;
	  exp = NULL;
	}
	else if (exp3->variant == DDOT) {
	  fprintf(stderr, "DDOT case\n");
	  exp4 = exp3;
	}
	else {	/* exp3 is loop bound which must mult by symbolic coef */
	  exp4 = make_llnd(cur_file, MULT_OP, sub->coefs_symb[j],
			   exp3, NULL);
	}
	if (exp != NULL) {
	  exp3 = make_llnd(cur_file, ADD_OP, exp4, exp, NULL);
	  exp = exp3;
	}
	else
	  exp = exp4;
      }
      else if (sub->coefs[j] != 0) { /* a nice integer coef. */
	exp3 = build_exp_from_bound(il, &(il[j]),sign);
	if (exp3 == NULL) {
	  exp4 = NULL;
	  exp = NULL;
	}
	else if (exp3->variant == DDOT) {
	  fprintf(stderr, "DDOT case\n");
	  exp4 = exp3;
	}
	else if (sub->coefs[j] == 1)
	  exp4 = exp3;
	else {
	  exp2 = make_llnd(cur_file, INT_VAL, NULL, NULL, NULL);
	  exp2->entry.ival = sub->coefs[j];
	  if(sub->coefs[j] < 0) *sign = -1;
	  exp2->type = cur_file->head_type;	/* always INT type */
	  exp4 = make_llnd(cur_file, MULT_OP, exp2, exp3, NULL);
	}
	if (exp != NULL) {
	  exp3 = make_llnd(cur_file, ADD_OP, exp4, exp, NULL);
	  exp = exp3;
	}
	else
	  exp = exp4;
      }
    }
    return (exp);
  }
  else
    return (make_llnd(cur_file, STAR_RANGE, NULL, NULL, NULL));
}

/**************************************************************/
/* compute dist vect.  calculates the distance vector between */
/* two references source and destination.  The vector is an   */
/* array of integers of the form ( len, dist1, dist2, ....)   */
/* trouble is an array which indicates one of several problems*/
/*  if trouble[0] = 1 then there is no intersection!	      */
/*  if trouble[i] = PLUSMINUS then the i-th component is "<=>"*/
/*  if trouble[i] = PLUS then vector is "+" ,i.e. positive    */
/*	  but variable in nature. similar for ZPLUS which     */
/*		  means the vector is "0+" = non-negative     */
/*    other cases are ZMINUS="0-" and MINUS = "-"	      */
/*  if trouble[i] = NODEP then no depend. on this index at all*/
/*    NOTE: trouble[i] = NODEP is the case for scalars.       */
/* the first component of vec is the length of the vector.    */
/* function returns nothing				      */
/**************************************************************/
int comp_dist(vec, trouble, sor, des, lexord)
int vec[], trouble[];
struct ref *sor;
struct ref *des;
int lexord;			/* true if sor precedes des in lex order */
{
  PTR_SYMB sor_ind_l[MAX_NEST_DEPTH], des_ind_l[MAX_NEST_DEPTH];
  struct subscript il_lo[MAX_NEST_DEPTH];
  struct subscript il_hi[MAX_NEST_DEPTH];
  struct subscript source[AR_DIM_MAX];	/* a source reference or def. */
  struct subscript destin[AR_DIM_MAX];	/* a destination ref. or def. */
  int inorder, i, j, sd, dd, depth, step, depfound;
  //int eqntbl[AR_DIM_MAX][2 * MAX_NEST_DEPTH + 1];
  PTR_SYMB s;

  if (table_generated == 0)
  {
      for (i = 0; i < tbl_depth; i++)
      {
          table[i] = (int *)calloc(2 * MAX_NEST_DEPTH + 1, sizeof(int));
#ifdef __SPF
          addToCollection(__LINE__, __FILE__,table[i], 0);
#endif
      }
      table_generated = 1;
  }
  for (i = 0; i < tbl_depth; i++)
    for (j = 0; j < np + 1; j++) {
      table[i][j] = 0;
     // if (i < AR_DIM_MAX)
	//eqntbl[i][j] = 0;
    }

  blank.decidable = 1;
  extra.decidable = 1;
  extra.offset = 0;
  blank.offset = 0;
  for (i = 0; i < MAX_NEST_DEPTH; i++) {
    sor_ind_l[i] = NULL;
    des_ind_l[i] = NULL;
    blank.coefs[i] = 0;
    il_lo[i].decidable = 1;
    il_hi[i].decidable = 1;
    il_lo[i].offset = 0;
    il_hi[i].offset = 0;
    for (j = 0; j < MAX_NEST_DEPTH; j++) {
      il_lo[i].coefs[j] = 0;
      il_hi[i].coefs[j] = 0;
    }
  }

  sd = make_induct_list(sor->stmt, sor_ind_l, il_lo, il_hi);

  dd = make_induct_list(des->stmt, des_ind_l, il_lo, il_hi);

  depth = (sd < dd) ? sd : dd;
  inorder = (sor->stmt->g_line < des->stmt->g_line) ? 1 : 0;

  i = 0;
  while ((i < depth) && (des_ind_l[i] == sor_ind_l[i]))
    i++;
  if (i < depth)
    depth = i;

  make_subscr(sor, source);
  make_subscr(des, destin);
  /* for each subscript expression we need to check for */
  /* symbolic references.  if they are the same we are	*/
  /* ok.  if they are different we set the flag to be	*/
  /* undecidable.				       */
  for (j = 0; j < AR_DIM_MAX; j++) {
    if ((source[j].parm_exp != NULL) ||
	(destin[j].parm_exp != NULL)) {
      if (sequiv(source[j].parm_exp, destin[j].parm_exp) == 0) {
	/* the following is temporary.	we   */
	/* should do a symbolic subtraction  */
	source[j].offset = 1;
	destin[j].offset = 0;
	source[j].decidable = 1;
	destin[j].decidable = 1;
	source[j].parm_exp = NULL;
	destin[j].parm_exp = NULL;
      }
    }
  }
  s = sor->refer->entry.Template.symbol;
  for (i = 1; i < MAX_NEST_DEPTH; i++) {
    vec[i] = 0;
    trouble[i] = NODEP;
  }
  vec[0] = depth;
  trouble[0] = 0;
  /* first check for uniformly generated cases */
  if ((s->type->variant == T_ARRAY || s->type->variant == T_POINTER)
       && unif_gen(sor, des, vec, trouble, source, destin));
  else {
    /* if a scalar ... */
    if (s->type->variant != T_ARRAY && s->type->variant != T_POINTER) {
      for (i = 1; i <= depth; i++) {
	trouble[i] = 0;
	vec[i] = 0;
      }

      if (inorder == 0) {
	vec[depth] = 1;
	trouble[depth] = 0;
      }
      return (1);
    }
    else
      /* if not uniform do generalized shoestak */
      for (step = 0; step <= depth; step++) {
	if (solve_system(step, depth, sd, sor_ind_l,
			 dd, des_ind_l, il_lo, il_hi, source, destin) != 0) {
	  set_troub(step + 1, vec, trouble, PLUS);
	}
	else if (step == 0)
	  trouble[0] = 1;
      }
  }
  depfound = 0;

  for (i = 1; i < MAX_NEST_DEPTH; i++) {
    if (vec[i] != 0 || trouble[i] != NODEP)
      depfound = 1;
    if (trouble[i] == -99)
      trouble[i] = 0;
  }

  if (depfound == 0 && !lexord)
    trouble[0] = 1;
  return (1);			/* return value means nothing here */

}

int solve_system(step,depth,sd,sor_ind_l,dd,des_ind_l,il_lo,il_hi,source,destin)
int step, depth, sd, dd;
PTR_SYMB sor_ind_l[MAX_NEST_DEPTH], des_ind_l[MAX_NEST_DEPTH];
struct subscript il_lo[];
struct subscript il_hi[];
struct subscript source[];	/* a source reference or def. */
struct subscript destin[];	/* a destination ref. or def. */
{
  struct subscript lo, hi;
  int i, j, k, max_depth;
  int num_eqn, num_ineq;

  max_depth = (sd > dd) ? sd : dd;

  /* now build equation rows of the table */
  num_eqn = -1;
  for (j = 0; j < AR_DIM_MAX; j++) {
    if (source[j].decidable != -1 || destin[j].decidable != -1)
      add_eqn(table[j], &source[j], &destin[j]);
    else if (num_eqn == -1)
      num_eqn = j;
  }
  /* add step equations */
  for (k = 0; k < step; k++) {
    for (j = 0; j < MAX_NEST_DEPTH; j++) {
      extra.coefs[j] = 0;
      blank.coefs[j] = 0;
    }
    extra.coefs[k] = 1;
    blank.coefs[k] = 1;
    add_eqn(table[num_eqn], &extra, &blank);
    num_eqn++;
    blank.coefs[k] = 0;
  }

  /* fix normalization for stride */
  for (i = 0; i < depth; i++) {
    if (stride[i] != 1) {
      for (j = 0; j < num_eqn; j++) {
	table[j][i] = table[j][i] * stride[i];
	table[j][MAX_NEST_DEPTH + i] =
	  table[j][MAX_NEST_DEPTH + i] * stride[i];
      }

      if (stride[i] < 0) {
	for (j = 0; j < num_eqn; j++)
	  if (table[j][i] < 0)
	    for (k = 0; k <= np; k++)
	      table[j][k] = -table[j][k];
      }
    }
  }

  num_ineq = 0;

  /* now add direction inequality at position step */
  for (j = 0; j < MAX_NEST_DEPTH; j++) {
    extra.coefs[j] = 0;
    blank.coefs[j] = 0;
  }
  extra.coefs[step] = -1;
  blank.coefs[step] = -1;
  extra.offset = -1;
  add_eqn(table[num_eqn], &extra, &blank);
  extra.coefs[step] = 0;
  blank.coefs[step] = 0;
  extra.offset = 0;

  num_ineq = 1;
  /* now add vector range subscript ineq. */
  for (j = 0; j < AR_DIM_MAX; j++) {
    if (source[j].decidable == 2) {
      /* source is vector in component j */
      make_vect_range(sd, source[j].vector, sor_ind_l, &lo, &hi);
      add_eqn(table[num_eqn + num_ineq], &lo, &blank);
      add_eqn(table[num_eqn + num_ineq + 1], &hi, &blank);
      num_ineq = num_ineq + 2;
    }
    if (destin[j].decidable == 2) {
      /* destin is vector in component j */
      make_vect_range(dd, destin[j].vector, des_ind_l, &lo, &hi);
      add_eqn(table[num_eqn + num_ineq], &lo, &blank);
      add_eqn(table[num_eqn + num_ineq + 1], &hi, &blank);
      num_ineq = num_ineq + 2;
    }
  }


  /* now add induction bound inequalities */
  for (j = 0; j < max_depth; j++) {
    /* reverse lo */
    il_lo[j].offset = -il_lo[j].offset;
    for (i = 0; i < MAX_NEST_DEPTH; i++)
      il_lo[j].coefs[i] = -il_lo[j].coefs[i];
    il_lo[j].coefs[j] = 1;	/* perhaps repalce by stride ? */
    il_hi[j].coefs[j] = -1;

    if (il_lo[j].decidable == 1) {
      add_eqn(table[num_eqn + num_ineq], &il_lo[j], &blank);
      num_ineq = num_ineq + 1;
    }
    if (il_hi[j].decidable == 1) {
      add_eqn(table[num_eqn + num_ineq], &il_hi[j], &blank);
      num_ineq = num_ineq + 1;
    }
    /* reset lo and reverse hi */
    for (i = 0; i < MAX_NEST_DEPTH; i++) {
      il_lo[j].coefs[i] = -il_lo[j].coefs[i];
      il_hi[j].coefs[i] = -il_hi[j].coefs[i];
    }
    il_lo[j].offset = -il_lo[j].offset;
    il_hi[j].offset = -il_hi[j].offset;
    if (il_lo[j].decidable == 1) {
      add_eqn(table[num_eqn + num_ineq], &blank, &il_lo[j]);
      num_ineq = num_ineq + 1;
    }
    if (il_hi[j].decidable == 1) {
      add_eqn(table[num_eqn + num_ineq], &blank, &il_hi[j]);
      num_ineq = num_ineq + 1;
    }
    /* reset hi */
    for (i = 0; i < MAX_NEST_DEPTH; i++) {
      il_hi[j].coefs[i] = -il_hi[j].coefs[i];
    }
    il_hi[j].offset = -il_hi[j].offset;
    il_lo[j].coefs[j] = 0;
    il_hi[j].coefs[j] = 0;

  }

  /* table complete.. now put in reduced form */
  if (reduce(table, num_eqn, num_eqn + num_ineq) == 0)
    return (0);
  else
    return (1);
}

void add_eqn(table, source, destin)
struct subscript *source;	/* a source reference or def. */
struct subscript *destin;	/* a destination ref. or def. */
int table[];
{
  int i;

  if (source->decidable < 1 || destin->decidable < 1)
    for (i = 0; i < np + 1; i++)
      table[i] = 0;
  else {
    for (i = 0; i < MAX_NEST_DEPTH; i++) {
      table[i] = source->coefs[i];
      table[i + MAX_NEST_DEPTH] = -(destin->coefs[i]);
    }
    table[np] = source->offset - destin->offset;
  }
}

void print_tbl(depth, neqn, neq, tbl)
int depth, neqn, neq;
int *tbl[];
{
  int i, j;

  depth = depth;		/* make lint happy, depth unused */

  fprintf(stderr, "|---------------table----------------------|\n");
  fprintf(stderr, "|  i   j   k   i'   j'	  k'   const   relat|\n");
  fprintf(stderr, "|------------------------------------------|\n");
  j = np / 2;
  for (i = 0; i < neqn; i++)
    fprintf(stderr, "| %2d  %2d  %2d  %2d   %2d   %2d	%4d	     ==  |\n",
	   tbl[i][0], tbl[i][1], tbl[i][2],
	   tbl[i][j], tbl[i][j + 1], tbl[i][j + 2], tbl[i][np]);
  fprintf(stderr, "|------------------------------------------|\n");
  for (i = neqn; i < neqn + neq; i++)
    fprintf(stderr, "| %2d  %2d  %2d  %2d   %2d   %2d	%4d	     >=  |\n",
	   tbl[i][0], tbl[i][1], tbl[i][2],
	   tbl[i][j], tbl[i][j + 1], tbl[i][j + 2], tbl[i][np]);
  fprintf(stderr, "|------------------------------------------|\n");
}

void print_etbl(depth, neqn, tbl)
int depth, neqn;
int tbl[AR_DIM_MAX][2 * MAX_NEST_DEPTH + 1];
{
  int i, j;

  depth = depth;		/* make lint happy, depth unused */

  fprintf(stderr, "|---------------table----------------------|\n");
  fprintf(stderr, "|  i   j   k   i'   j'	  k'   const   relat|\n");
  fprintf(stderr, "|------------------------------------------|\n");
  j = np / 2;
  for (i = 0; i < neqn; i++)
    fprintf(stderr, "| %2d  %2d  %2d  %2d   %2d   %2d	%4d	     ==  |\n",
	   tbl[i][0], tbl[i][1], tbl[i][2],
	   tbl[i][j], tbl[i][j + 1], tbl[i][j + 2], tbl[i][np]);
  fprintf(stderr, "|------------------------------------------|\n");
}

int reduce(tbl, num_eqn, tbl_depth)
int *tbl[];
int num_eqn, tbl_depth;
{
  int j, i, k, t, mgcd, piv, pcol, opc, alf, bet;
  int *tmp;

  for (i = 0; i < 2 * MAX_NEST_DEPTH; i++) {
    upper_bnd[i] = 32000;
    lower_bnd[i] = -32000;
    if (i < MAX_NEST_DEPTH) {
      dist_lb[i] = -32000;
      dist_ub[i] = 32000;
    }
  }

  for (i = 0; i < tbl_depth; i++)
    if (chk_bnds(tbl, i, upper_bnd, lower_bnd) == 0)
      return (0);

  pcol = -1;
  /* first eliminate by using the equations */
  for (j = 0; j < num_eqn; j++) {
    /* find leader pivod equation */
    piv = -1;
    opc = pcol;
    for (k = opc + 1; k < MAX_NEST_DEPTH * 2; k++)
      for (t = j; t < num_eqn; t++)
	if (opc == pcol && tbl[t][k] != 0) {
	  pcol = k;
	  piv = t;
	}

    if (piv > -1) {
      /* swap to bring to top */
      tmp = tbl[j];
      tbl[j] = tbl[piv];
      tbl[piv] = tmp;
      /* first reduce by gcd of row */
      if (tbl[j][pcol] < 0)
	for (i = 0; i <= np; i++)
	  tbl[j][i] = -tbl[j][i];
      mgcd = gcd(np - 1, tbl[j]);
      if (mgcd > 1) {
	/* first test for bad congruence class */
	if ((tbl[j][np] % mgcd) != 0)
	  return (0);
	for (i = 0; i <= np; i++)
	  tbl[j][i] = tbl[j][i] / mgcd;
      }
      /* now do elimination on pcol */
      alf = tbl[j][pcol];
      if (alf == 0)
	fprintf(stderr, "reduce error\n");
      else if (alf < 0) {
	alf = -alf;
	for (i = 0; i <= np; i++)
	  tbl[j][i] = -tbl[j][i];
      }
      for (k = j + 1; k < tbl_depth; k++) {
	if ((bet = tbl[k][pcol]) != 0) {
	  /* first reduce row k */
	  for (i = pcol; i <= np; i++)
	    tbl[k][i] = alf * tbl[k][i] - bet * tbl[j][i];
	  /* test for dim 1 or 0 constraint */
	  if (chk_bnds(tbl, k, upper_bnd, lower_bnd) == 0)
	    return (0);
	}
      }
    }				/* end of piv found case */
  }				/* end of factorization loop */
  /* second eliminate by adding inequalities */
  for (j = num_eqn; j < tbl_depth; j++) {
    /* find leader pivod equation */
    piv = -1;
    opc = pcol;
    for (k = opc + 1; k < MAX_NEST_DEPTH * 2; k++)
      for (t = j; t < tbl_depth; t++)
	if (opc == pcol && tbl[t][k] > 0) {
	  pcol = k;
	  piv = t;
	}

    if (piv > -1) {
      /* swap to bring to top */
      tmp = tbl[j];
      tbl[j] = tbl[piv];
      tbl[piv] = tmp;
      /* now do elimination on pcol */
      alf = tbl[j][pcol];
      if (alf <= 0)
	fprintf(stderr, "reduce error\n");
      for (k = j + 1; k < tbl_depth; k++) {
	if ((bet = tbl[k][pcol]) < 0) {
	  /* first do the ellimination */
	  for (i = 0; i <= np; i++)
	    tbl[k][i] = alf * tbl[k][i] - bet * tbl[j][i];
	  /* now check for constraint errors */
	  if (chk_bnds(tbl, k, upper_bnd, lower_bnd) == 0)
	    return (0);
	}
      }
    }				/* end of piv found case */
  }				/* end of factorization loop */

  /* now look for contradictions in eqnations */
  for (j = 0; j < tbl_depth; j++)
    if (chk_bnds(tbl, j, upper_bnd, lower_bnd) == 0)
      return (0);
  return (1);
}

int chk_bnds(tbl, k, upper_bnd, lower_bnd)
int *tbl[];
int k;
int upper_bnd[], lower_bnd[];
{
  int i, first, second, third, gama;

  third = -1;
  first = -1;
  second = -1;
  for (i = 0; i < np; i++)
    if (tbl[k][i] != 0) {
      if (first == -1)
	first = i;
      else if (second == -1)
	second = i;
      else if (third == -1)
	third = i;
    }
  if (first == -1) {		/* this is a dimension 0 constraint */
    if ((k < num_eqn) & (tbl[k][np] != 0))
      return (0);
    if ((k >= num_eqn) & (tbl[k][np] < 0))
      return (0);
  }
  else if (second == -1) {	/* this is a dimension 1 constraint */
    if (k < num_eqn) {
      gama = -tbl[k][np] / tbl[k][first];
      /* var first has lower bound gama and upper bound gama */
      if (gama < lower_bnd[first])
	return (0);
      lower_bnd[first] = gama;
      if (gama > upper_bnd[first])
	return (0);
      upper_bnd[first] = gama;
    }
    else {			/* this is an inequality */
      if (tbl[k][first] > 0) {	/* the inequality is > */
	gama = -tbl[k][np] / tbl[k][first];
	/* gama is a new lower bound */
	if (gama > upper_bnd[first])
	  return (0);
	if (gama > lower_bnd[first])
	  lower_bnd[first] = gama;
      }
      else {			/* the inequality is < */
	gama = -tbl[k][np] / tbl[k][first];
	/* gama is a new upper bound */
	if (gama < lower_bnd[first])
	  return (0);
	if (gama < upper_bnd[first])
	  upper_bnd[first] = gama;
      }
    }
  }				/* end dim 1 case */
  else if (third == -1 && (second - first) == MAX_NEST_DEPTH) { 
  
  /* dimension 2 case involving i and i' look for i' - i > k forms */
    if (tbl[k][first] == -tbl[k][second]) {
      if (k < num_eqn) {
	dist_ub[first] = -tbl[k][np] / tbl[k][second];
	dist_lb[first] = dist_ub[first];
      }
      else if (tbl[k][second] < 0
	       && dist_ub[first] > tbl[k][np] / tbl[k][first])
	dist_ub[first] = tbl[k][np] / tbl[k][first];
      else if (tbl[k][second] > 0
	       && dist_lb[first] < tbl[k][np] / tbl[k][second])
	dist_lb[first] = -tbl[k][np] / tbl[k][second];
      if (dist_ub[first] < dist_lb[first])
	return (0);
    }
  }				/* end dim 2 case */
  return (1);
}


/*****************************************************************/
/* set_vec check the previous state of the troub and val vectors */
/* to see if a previous index computation has determined values  */
/* for the i-th induction var that differ from the current one.  */
/* if a val of zero is set troub[i] is set to -99 as a reminder. */
/*****************************************************************/
void set_vec(i, vec, troub, val)
int i;
int vec[], troub[];
int val;
{
  if ((vec[i] != 0) || (troub[i] == -99)) {
    if (vec[i] != val)
      troub[0] = 1;
    if (val == 0)
      troub[i] = -99;
  }
  else if (((val < 0) && (troub[i] == ZPLUS)) ||
	   ((val > 0) && (troub[i] == ZMINUS)) ||
	   ((val == 0) && ((troub[i] == PLUS) || (troub[i] == MINUS)))
    )
    troub[0] = 1;
  else {
    vec[i] = val;
    if (val == 0)
      troub[i] = -99;
    else
      troub[i] = 0;
  }
}

void set_troub(i, vec, troub, val)
int i;
int vec[], troub[];
int val;
{
  switch (val) {
   case PLUS:
    if ((vec[i] < 0) || (troub[i] == -99) ||
	(troub[i] == ZMINUS))
      troub[0] = 1;
    break;
   case MINUS:
    if ((vec[i] > 0) || (troub[i] == -99) ||
	(troub[i] == ZPLUS))
      troub[0] = 1;
    break;
   case ZPLUS:
    if ((vec[i] < 0) || (troub[i] == MINUS))
      troub[0] = 1;
    break;
   case ZMINUS:
    if ((vec[i] > 0) || (troub[i] == PLUS))
      troub[0] = 1;
    break;
   case PLUSMINUS:		/* does not invalidate anything! */
    break;
   default:
    troub[i] = val;
  }
  if ((troub[i] == NODEP) && (vec[i] == 0))
    troub[i] = val;
}



