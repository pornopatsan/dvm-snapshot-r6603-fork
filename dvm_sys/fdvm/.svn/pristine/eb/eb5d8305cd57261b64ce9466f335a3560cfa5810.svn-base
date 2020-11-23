/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
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

static PTR_BFND current_par_loop = NULL;
static char *depstrs[] = { "flow","anti","output","huh??","got me?"};
static char *dirstrs[] = { "  ", "= ", "- ", "0-", "+ ", "0+", ". ", "+-"};
extern PCF UnparseBfnd[];
extern PCF UnparseLlnd[];

extern PTR_FILE cur_file;

/* Forward definitions */
static PTR_BLOB1 Nsearch_deps();
static void subtract_list();
static int same_loop();
void search_and_replace_call();

extern void normal_form();
extern int identical();

PTR_LLND search_call(ll, s)
PTR_LLND ll;
PTR_SYMB *s;
{
   PTR_LLND t;
   *s = NULL;
   if(ll == NULL) return(NULL);
   if(ll->variant == FUNC_CALL){
      *s = ll->entry.Template.symbol;
      return(ll->entry.Template.ll_ptr1);
   }
   else{
      t = search_call(ll->entry.Template.ll_ptr1,s);
      if(t != NULL) return(t);
      return(search_call(ll->entry.Template.ll_ptr2,s));
   }
}

PTR_REFL build_refl(b,s)
PTR_BFND b;
PTR_LLND s;
{
   PTR_REFL p,h,l,alloc_ref();
   h = NULL; l = NULL;
   while(s!= NULL){
      p = alloc_ref(b,s->entry.Template.ll_ptr1);
      if(p != NULL){
	 if(h == NULL){ h = p;}
	 if(l != NULL) l->next = p;
	 l = p;
      }
      s = s->entry.Template.ll_ptr2;
   }
   return(h);
}

/* find loop bounds takes a bif pointer b and addresses of */
/* three other pointers low, hi, inc and computes loop bounds */
/* and returns 1 if it succeds in finding these in terms of */
/* constants, parameters and external varaibles and returns */
/* 0 if it failed. */
int find_loop_bounds(b,low,hi,inc)
PTR_BFND b;
PTR_LLND *low, *hi, *inc;
{return (0);}

/* bind call site info will take a pointer to a call statement and */
/* return a expression list of the used and modified sets in terms */
/* of the actual parameters. */
void bind_call_site_info(b, used, modified)
PTR_BFND b;
PTR_LLND *used, *modified;
{
   PTR_LLND funargs, formal_used, formal_modified;
   PTR_SYMB fun, s,formal_args[50];
   PTR_BFND fun_bif;
   /* PTR_BLOB bl; */
   PTR_LLND  u, m, explst;
   int i, num_formal_args;
   PTR_LLND called_with[50];
   PTR_LLND copy_llnd();
   PTR_BFND find_fun_by_name();
   int fun_found ;
    
   *used = NULL; *modified = NULL; fun = NULL; fun_found = 0;
   formal_used = NULL; formal_modified = NULL;
   formal_args[0] = NULL; num_formal_args = 0;;
   if(b == NULL) return;
   if(b->variant == PROC_STAT){
      funargs = b->entry.Template.ll_ptr1;
      fun = b->entry.Template.symbol;
   }
   else  if(b->variant == ASSIGN_STAT){
      funargs = search_call(b->entry.Template.ll_ptr2,&fun);
   }
   else  if(b->variant == EXPR_STMT_NODE){
      funargs = search_call(b->entry.Template.ll_ptr1,&fun);
   }
   /* if(fun != NULL)
	fprintf(stderr, "funargs = %s\n",
			(UnparseBfnd[cur_file->lang])(funargs)); */
   else {
      fprintf(stderr, "serch_call error. node is %s",
		      (UnparseBfnd[cur_file->lang])(b));
      fprintf(stderr, "node type is %d\n",b->variant);
      return;
   }
   if(fun == NULL) return;
   if(funargs == NULL) return;
   fun_bif = find_fun_by_name(fun->ident); /*no longer need loop search*/
   if(fun_bif == NULL){
	fprintf(stderr, "find fun_by_name failed %s\n",fun->ident);
	return;
	}
   else if (strcmp(fun_bif->entry.Template.symbol->ident,fun->ident)){
	fprintf(stderr, "find fun by name returned wrong fun\n");
	return;
	}
   if(fun_bif->variant == PROC_HEDR || fun_bif->variant == FUNC_HEDR){
	 if(!strcmp(fun_bif->entry.Template.symbol->ident,fun->ident)){
	    fun_found = 1;
	    s = fun_bif->entry.Template.symbol;
	    s = s->entry.proc_decl.in_list;
	    while(s != NULL){ /* gather formal args in formal_args */
	       formal_args[num_formal_args++] = s;
	       s = s->entry.var_decl.next_in;
	    }
	    explst = fun_bif->entry.Template.ll_ptr3;
	    if(explst == NULL) return;
	    if(explst->entry.Template.ll_ptr2 == NULL){
	       /* only first pass analysis done */
	       formal_used = explst->entry.Template.ll_ptr1; /* bif graph */
	    }
	    else
	       formal_used = explst->entry.Template.ll_ptr2;
	    explst = fun_bif->entry.Template.ll_ptr2;
	    if(explst == NULL) return;
	    if(explst->entry.Template.ll_ptr2 == NULL){
	       /* only first pass analysis done */
	       formal_modified = explst->entry.Template.ll_ptr1; /* bif graph*/
	    }
	    else
	       formal_modified = explst->entry.Template.ll_ptr2;
	 }
      }
   if(fun_found == 0){
	fprintf(stderr, "could not locate source for function %s\n",fun->ident);
	return;
	}
   if(num_formal_args == 0) return;
   u = copy_llnd(formal_used);
   m = copy_llnd(formal_modified);
   for(i = 0; i < num_formal_args; i++){ /* gather actual args in called_with*/
     if(funargs == NULL){
       printf("ERROR: function not called with enough arguments\n");
       exit(0);
     }
      called_with[i] = copy_llnd(funargs->entry.Template.ll_ptr1);
      funargs = funargs->entry.Template.ll_ptr2;
   }
   search_and_replace_call(&u,num_formal_args,formal_args,called_with);
   search_and_replace_call(&m,num_formal_args,formal_args,called_with);
   *used = u;
   *modified = m;
   /*
     fprintf(stderr, "formal_used are:\n");
     fprintf(stderr, "%s",UnparseLlnd[cur_file->lang](formal_used));
     fprintf(stderr, "actual used are:\n");
     fprintf(stderr, "%s",UnparseLlnd[cur_file->lang](u));
     fprintf(stderr, "formal_modified are:\n");
     fprintf(stderr, "%s",UnparseLlnd[cur_file->lang](formal_modified));
     fprintf(stderr, "actual modified are:\n");
     fprintf(stderr, "%s",UnparseLlnd[cur_file->lang](m));
     fprintf(stderr, "called with:\n");
     for(i = 0; i < num_formal_args; i++)
	fprintf(stderr, " %s,",UnparseLlnd[cur_file->lang](called_with[i]));
     fprintf(stderr, "\n");
     if(formal_args[0] == NULL) return;
     fprintf(stderr, "formal args are:\n");
     for(i = 0; i < num_formal_args; i++)
	fprintf(stderr, " %s,",formal_args[i]->ident);
     fprintf(stderr, "\n");
     */
}

int get_fargs_index(s,n,fargs)
PTR_SYMB s;
int n;
PTR_SYMB fargs[];
{
   int i;
   for(i = 0; i < n; i++) 
      if(fargs[i] == s) return(i);
   return(-1);
}

void add_offset(offset,term)
PTR_LLND offset, *term;
{
   PTR_LLND p,q,r, make_llnd(), copy_llnd();
   if(offset == NULL){
	fprintf(stderr, "bad offset in add_offset\n");
	return;
	}
   if(term == NULL){
	fprintf(stderr, "badd term in add_offset\n");
	return;
	}
   if(*term == NULL){
	fprintf(stderr, " null term in add_offset\n");
	}
   if(*term == NULL || (
	offset->variant == DDOT && *term != NULL && (*term)->variant == DDOT)){
      q = make_llnd(cur_file, STAR_RANGE,NULL,NULL,NULL);
      *term = q;
   }
   else if((*term)->variant == STAR_RANGE){
      /* term is of the form x[:] and no offset will help */
   }
   else if(offset->variant == STAR_RANGE){ /* MANNHO add 9/10 */
      *term = offset;
   }
   else if((*term)->variant == DDOT){
      PTR_LLND offset1, offset2;
      offset1 = copy_llnd(offset);
      p = (*term)->entry.Template.ll_ptr1;
      q = make_llnd(cur_file, ADD_OP,p,offset1,NULL);
      /* MANNHO delete
	 if(cur_file->lang == ForSrc){
	    p = make_llnd(cur_file, INT_VAL,NULL,NULL,NULL);
	    p->entry.ival = 1;
	    q = make_llnd(cur_file, SUBT_OP,q,p,NULL);
	 }
	 */
      normal_form(&q); /* normal_form(&q); */
      (*term)->entry.Template.ll_ptr1 = q;
      p = (*term)->entry.Template.ll_ptr2;
      offset2 = copy_llnd(offset);
      q = make_llnd(cur_file, ADD_OP,p,offset2,NULL);
      /* MANNHO delete
	 if(cur_file->lang == ForSrc){
		p = make_llnd(cur_file, INT_VAL,NULL,NULL,NULL);
		p->entry.ival = 1;
		q = make_llnd(cur_file, SUBT_OP,q,p,NULL);
		}
		*/
      /* normal_form(&q); */
      normal_form(&q); 
      (*term)->entry.Template.ll_ptr2 = q;
   }
   else if(offset->variant == DDOT){
      r = copy_llnd(*term);
      offset = copy_llnd(offset);
      p = offset->entry.Template.ll_ptr1;
      q = make_llnd(cur_file, ADD_OP,p,r,NULL);
      offset->entry.Template.ll_ptr1 = q;
      p = offset->entry.Template.ll_ptr2;
      q = make_llnd(cur_file, ADD_OP,p,r,NULL);
      offset->entry.Template.ll_ptr2 = q;
      *term = offset;
   }
   else{
      offset = copy_llnd(offset);
      q = make_llnd(cur_file, ADD_OP,*term,offset,NULL);
      *term = q;
   }
}

PTR_LLND get_array_dim_decl(AR) /* MANNHO add */
	 PTR_LLND AR; /* ARRAY_REF */
{
   PTR_LLND RL, R_L = NULL, ll0, ll1;
   PTR_TYPE TY;
   PTR_LLND copy_llnd(), make_llnd();

   TY = AR->entry.Template.symbol->type;
   switch (TY->variant) {
   case T_ARRAY : /* MANNHO mod */
      R_L = TY->entry.ar_decl.ranges;
      if (R_L->variant != EXPR_LIST) R_L = R_L->entry.Template.ll_ptr1;
      break;
   case T_POINTER :
      R_L = NULL;
      break;
   }

   if (R_L == NULL) return(NULL);

   RL = R_L = copy_llnd(R_L);
   while (RL) {
      ll1 = RL->entry.Template.ll_ptr1;
      if (ll1->variant != DDOT) {
	 if (cur_file->lang == ForSrc)
	    ll0 = make_llnd(cur_file, INT_VAL, NULL, NULL, 1);
	 else
	    ll0 = make_llnd(cur_file, INT_VAL, NULL, NULL, 0);
	 RL->entry.Template.ll_ptr1 = make_llnd(cur_file, DDOT, ll0, ll1, NULL);
      }
      RL = RL->entry.Template.ll_ptr2;
   }
   return (R_L);
}

/* u is a reference to an expression describing the result of an action */
/* by a call to the function.  fargs is the associated set of formal	*/
/* formal parameters.  call is the actual values passed to the formal	*/
/* parameter.  search_and_replace modifies u so that it reflects the	*/
/* the action in terms of the actual parameters.			*/
void search_and_replace_call(u,n,fargs,call)
PTR_LLND *u;
int n;
PTR_SYMB fargs[];
PTR_LLND call[];
{
   int i;
   PTR_LLND v,index,a,b, b1, b2;
   PTR_LLND make_llnd(), copy_llnd(), linearize_array_range();
   PTR_LLND get_array_dim_decl();
   
   if (*u == NULL) return ;
   /* *u is the result of the call in terms of the formal params */
   switch((*u)->variant){
   case VAR_REF:
      /* find the position of *u in the parameter list */  
      i = get_fargs_index((*u)->entry.Template.symbol,n,fargs);
      if (i<0) return ;
      if(call[i]->variant == ADDRESS_OP) v = call[i]->entry.Template.ll_ptr1;
      else v = call[i];
      *u = copy_llnd(v);
      break;
   case ARRAY_REF:
      i = get_fargs_index((*u)->entry.Template.symbol,n,fargs);
      if(i < 0) return ;
      v = call[i];  /* v is the expression that is passed in position i */
      if(v->variant == VAR_REF){
	 (*u)->entry.Template.symbol = v->entry.Template.symbol;
	 search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
				 n,fargs,call);
	 search_and_replace_call(&((*u)->entry.Template.ll_ptr2),
				 n,fargs,call);
      }
      else if(cur_file->lang != ForSrc && v->variant == ARRAY_REF){
	 /* if v has dim 1 greater than *u */
	 index = (*u)->entry.Template.ll_ptr1;
	 (*u)->entry.Template.symbol = v->entry.Template.symbol;
	 search_and_replace_call(&index,n,fargs,call);
	 index = v->entry.Template.ll_ptr1;
	 while(index->entry.Template.ll_ptr2 != NULL)
	    index = index->entry.Template.ll_ptr2;
	 index->entry.Template.ll_ptr2 = (*u)->entry.Template.ll_ptr1;
	 (*u)->entry.Template.ll_ptr1 = v->entry.Template.ll_ptr1;
      }
      else if(v->variant == ADDRESS_OP){
	 /* something like &(x[i]) */
	 a = v->entry.Template.ll_ptr1; /* the x[i] part */
	 if(a->variant == EXPR_LIST) a = a->entry.Template.ll_ptr1;
	 (*u)->entry.Template.symbol=a->entry.Template.symbol;	
	 if(a->variant == VAR_REF ){
	    search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
				    n,fargs,call);
	 }
	 else if(a->variant == ARRAY_REF){
	    PTR_LLND second_index;
	    /* we are adding the offset from &(x[i]) to y[10:2] */
	    /* u is a *pointer to the summary data and a is a pointer to */
	    /* the actual argument.  make u look like a  */ 
	    search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
				    n,fargs,call);
	    b = (*u)->entry.Template.ll_ptr1; /* range list */
	    index = a->entry.Template.ll_ptr1; /*range list */
	    if(index != NULL) second_index = index->entry.Template.ll_ptr2;
	    else second_index = NULL;
	    if(index == NULL){
	    }
	    else if(b == NULL){
	       (*u)->entry.Template.ll_ptr1 = copy_llnd(index);
	    }	
	    else {
	       b1 = b->entry.Template.ll_ptr1;
	       b2 = b->entry.Template.ll_ptr2;
	       b->entry.Template.ll_ptr1 =
		  copy_llnd(index->entry.Template.ll_ptr1);
	       b->entry.Template.ll_ptr2 = copy_llnd(second_index);
	       while (b->entry.Template.ll_ptr2 != NULL)
		  b = b->entry.Template.ll_ptr2;
	       add_offset(b1, &(b->entry.Template.ll_ptr1));
	       b->entry.Template.ll_ptr2 = b2;
	    }
	 }
	 else fprintf(stderr, "a variant is %d\n",a->variant);
      }
      else if (cur_file->lang == ForSrc && v->variant == ARRAY_REF) {
	 /* u is a *pointer to a copy of the summary data and v points to */
	 /* the passed argument.  make u look like v. */
	 int udim, adim;
	 a = v;
	 if(a->variant == EXPR_LIST) a = a->entry.Template.ll_ptr1;
	 if(a->variant == VAR_REF ){
	    (*u)->entry.Template.symbol=a->entry.Template.symbol;
	    /* u now has the symbol of v, now do the substitution on the subscripts */
	    search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
				    n,fargs,call);
	 }
	 else if(a->variant == ARRAY_REF){
	    PTR_LLND size,ls,rs,adec;
	    /* we are adding the offset from &(a[i]) to u[10:2] */
	    /* u is a *pointer to the summary data and a is a pointer to */
	    /* the actual argument.  make u look like a. first fix the index */
	    /* terms in u */
	    search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
				    n,fargs,call);
	    /* next get the dimensions of these array references. */
	    /* let b be the index expression range list for *u.  */
	    udim = (*u)->entry.Template.symbol->type->entry.ar_decl.num_dimensions;
	    adim =    a->entry.Template.symbol->type->entry.ar_decl.num_dimensions;
	    size = get_array_dim_decl(*u); /* MANNHO mod */
	    adec = get_array_dim_decl(a);
	    if(adec->variant == EXPR_LIST || adec->variant == RANGE_LIST) adec = adec->entry.Template.ll_ptr1;
	    
	    search_and_replace_call(&size,n,fargs,call);
	    (*u)->entry.Template.symbol=a->entry.Template.symbol;  
	    /* we now must linearize the segments described by *u and */
	    /* then add the offset provided by a */
	    b = (*u)->entry.Template.ll_ptr1; /* range list */
	    index = a->entry.Template.ll_ptr1; /*range list */
	    if(index == NULL && udim == adim){
			/* *u already has the correct form */
		}
	    else if(index == NULL && adim < udim){
			/* if adim = 1 and udim is bigger */
			b = linearize_array_range(b,udim,size);
			ls = b->entry.Template.ll_ptr1->entry.Template.ll_ptr1;
			rs = b->entry.Template.ll_ptr1->entry.Template.ll_ptr2;
			add_offset(adec->entry.Template.ll_ptr1,
			     &(b->entry.Template.ll_ptr1));
			b->entry.Template.ll_ptr2 = NULL;
			/* fprintf(stderr," %s ",UnparseLlnd[cur_file->lang](b)); */
		}
	    else if(b == NULL){
			(*u)->entry.Template.ll_ptr1 = copy_llnd(index);
			}      
	    else if(index == NULL && adim > udim){
			int ii;
			PTR_LLND c;
			c = make_llnd(cur_file, INT_VAL,NULL,NULL,NULL);
			c->entry.ival = 1;
			for(ii = 0; ii < (adim-udim); ii++){
				b->entry.Template.ll_ptr2 =
					make_llnd(cur_file, EXPR_LIST,copy_llnd(c),NULL,NULL);
				b = b->entry.Template.ll_ptr2;
				}
			b->entry.Template.ll_ptr2 = NULL;
			}
	    else {
			b = linearize_array_range(b,udim,size);
			add_offset(index->entry.Template.ll_ptr1,
			     &(b->entry.Template.ll_ptr1));
			if(index->entry.Template.ll_ptr2 == NULL) b->entry.Template.ll_ptr2 = NULL;
			else{
			     if(index->entry.Template.ll_ptr2 !=NULL &&
				index->entry.Template.ll_ptr2->variant != EXPR_LIST)
				 b->entry.Template.ll_ptr2 = 
				make_llnd(cur_file, EXPR_LIST,index->entry.Template.ll_ptr2,NULL,NULL);
			     else b->entry.Template.ll_ptr2 = index->entry.Template.ll_ptr2;
			     }
			
			}
	 }
	 else fprintf(stderr, "a variant is %d\n",a->variant);
      }
      else{  /* something like p+3  for a pointer p */
	 fprintf(stderr, "a strange pointer case in ser. and repl.\n");
      }
      break;
   default:  /* an expression */
      search_and_replace_call(&((*u)->entry.Template.ll_ptr1),
			      n,fargs,call);
      search_and_replace_call(&((*u)->entry.Template.ll_ptr2),
			      n,fargs,call);;
   }
}

/* MANNHO delete whole this procedure
PTR_LLND get_leading_arr_dim(s)
PTR_SYMB s;
{
	PTR_LLND x, copy_llnd();
	x = s->type->entry.ar_decl.ranges;
	if(x->variant == ARRAY_REF) x = x->entry.Template.ll_ptr1;
	if(x->variant == EXPR_LIST) x = x->entry.Template.ll_ptr1;
	return(copy_llnd(x));
}
*/

void make_zero_base(ref, decl) /* MANNHO add */
PTR_LLND ref, decl;
{
	PTR_LLND ref_index, ref_low, ref_up, decl_low, dlow;
        PTR_LLND make_llnd(), copy_llnd();

	while (ref) {
		ref_index = ref->entry.Template.ll_ptr1;
		decl_low =decl->entry.Template.ll_ptr1->entry.Template.ll_ptr1;

		if (ref_index->variant == DDOT) {
			ref_low = ref_index->entry.Template.ll_ptr1;
			ref_up	= ref_index->entry.Template.ll_ptr2;
		   if(ref_low != NULL && decl_low != NULL){
			dlow = copy_llnd(decl_low);
			ref_low = make_llnd(cur_file, SUBT_OP, ref_low, dlow, NULL);
		      }
		   if(ref_up != NULL && decl_low != NULL){
			dlow = copy_llnd(decl_low);
			ref_up	= make_llnd(cur_file, SUBT_OP, ref_up,	dlow, NULL);
		      }
			ref_index->entry.Template.ll_ptr1 = ref_low;
			ref_index->entry.Template.ll_ptr2 = ref_up;
		    }
		 else if(decl_low != NULL && ref_index->variant != STAR_RANGE){
			dlow = copy_llnd(decl_low);
			ref_index = make_llnd(cur_file, SUBT_OP, ref_index, dlow, NULL);
			ref->entry.Template.ll_ptr1 = ref_index;
		}

		ref  =	ref->entry.Template.ll_ptr2;
		decl = decl->entry.Template.ll_ptr2;
	}
}

/* linearize_array_range takes a range list and returns a range */
/* list consiting of a 1-D ddot discription of the range	*/
PTR_LLND linearize_array_range(rl,dim,size) /* MANNHO mod */
PTR_LLND rl; /* a range list of expressions and ddots */
int dim;
PTR_LLND size; /* size is the declared dimension of the parameter */
{
	PTR_LLND RL, sz1, s;
	PTR_LLND size_upto, size_up, addend, low, up, one;
	PTR_LLND index, index_low, index_up;
	int shift_needed;
        PTR_LLND make_llnd(), copy_llnd();

	make_zero_base(rl, size);
	s = size; shift_needed = 0;
	while(s != NULL){
	    sz1 = s->entry.Template.ll_ptr1;
	    if(sz1->entry.Template.ll_ptr1 != NULL &&
	      (( sz1->entry.Template.ll_ptr1->variant != CONST_REF && 
		 sz1->entry.Template.ll_ptr1->variant != INT_VAL)  ||
	       sz1->entry.Template.ll_ptr1->entry.ival != 1)){
	             printf(" ival is %d\n",sz1->entry.Template.ll_ptr1->entry.ival);
	             shift_needed = 1;
		   }
	    s = s->entry.Template.ll_ptr2;
	  }
	s = copy_llnd(size);
	make_zero_base(size, s); 
	if(shift_needed) s = copy_llnd(size);
	/*
	fprintf(stderr, " rl = %s",UnparseLlnd[cur_file->lang](rl));
	fprintf(stderr, " size = %s",UnparseLlnd[cur_file->lang](size));
	*/
	size_upto = NULL; low = NULL; up = NULL;
	RL = rl;
	while (RL) {
		index = RL->entry.Template.ll_ptr1;
		sz1 = size->entry.Template.ll_ptr1;
		if (index->variant == DDOT) {
			index_low = index->entry.Template.ll_ptr1;
			index_up  = index->entry.Template.ll_ptr2;
		} else {
			index_low = index;
			index_up  = copy_llnd(index);
		}
		if(index->variant == STAR_RANGE){
			index->variant = DDOT;
			index_low = sz1->entry.Template.ll_ptr1;
			index_up = sz1->entry.Template.ll_ptr2;
			}
		if (low == NULL) { /* 1st index */
			low = index_low;
			up  = index_up;
			}
		else {
			if(low != NULL && size_upto != NULL){
			    addend = make_llnd(cur_file, MULT_OP, copy_llnd(size_upto),
					   index_low, NULL);
			    low = make_llnd(cur_file, ADD_OP, low, addend, NULL);
			    }
			if(up != NULL && size_upto != NULL){
			   addend = make_llnd(cur_file, MULT_OP, copy_llnd(size_upto),
					   index_up, NULL);
			   up  = make_llnd(cur_file, ADD_OP, up,  addend, NULL);
			   }
		}
		size_up = s->entry.Template.ll_ptr1->entry.Template.ll_ptr2;
		if(shift_needed){
		  one = make_llnd(cur_file, INT_VAL, NULL, NULL, 1);
		  size_up = make_llnd(cur_file, ADD_OP, size_up, one, NULL);
		}
		size_upto = (size_upto == NULL) ?
				size_up :
				make_llnd(cur_file, MULT_OP, size_upto, size_up, NULL);
		size = size->entry.Template.ll_ptr2;
		s = s->entry.Template.ll_ptr2;
		RL   =	 RL->entry.Template.ll_ptr2;
	}
	if (low == NULL && up == NULL){
		RL = make_llnd(cur_file,STAR_RANGE,NULL, NULL, NULL);
		}
	else if (identical(low, up)) {
		RL = low;
		/* free_ll_tree(up); */
	} else {
		RL = make_llnd(cur_file, DDOT, low, up, NULL);
	}
	rl->entry.Template.ll_ptr1 = RL;
	rl->entry.Template.ll_ptr2 = NULL;
	return(rl);
}

PTR_BLOB1
   NGetCallInfo(filename,line)
char *filename;
int line;
{
   PTR_BLOB1 lb, nb,tb;
   PTR_BFND b, FindBifNode();
   char *s;
   PTR_LLND used, modified;
   
   used = NULL; modified = NULL;
   b = FindBifNode(filename,line);
   if(b == NULL){ 
      nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,nb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"Could not find code at line %d\n",line);
      nb->ref = s;
      nb->next = NULL;
      return(nb);
   }
   if(b->variant != PROC_STAT && b->variant != EXPR_STMT_NODE){ 
      nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,nb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"Cound not find call at line %d\n",line);
      nb->ref = s;
      nb->next = NULL;
      return(nb);
   }
   bind_call_site_info(b,&used,&modified);
   if(used == NULL){ 
      tb = nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,tb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"nothing used in call. \n");
      nb->ref = s;
      nb->next = NULL;
      lb = nb;
   }
   else{ 
      tb = nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,tb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"variables used in call are: \n");
      nb->ref = s;
      tb->next = nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,tb->next, 0);
#endif
      s = (UnparseLlnd[cur_file->lang])(used);
      nb->ref = s;
      nb->next = NULL;
      lb = nb;
   }
   if(modified == NULL){ 
      lb->next = nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,lb->next, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"nothing modified by call. \n");
      nb->ref = s;
      nb->next = NULL;
      return(tb);
   }
   else{ 
      lb->next = nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,lb->next, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"variables modified  in call are: \n");
      nb->ref = s;
      nb->next = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,nb->next, 0);
#endif
      nb = nb->next;
      s = (UnparseLlnd[cur_file->lang])(modified);
      nb->ref = s;
      nb->next = NULL;
      return(tb);
   }
}



PTR_BLOB1
   NGetDepInfo(filename, line)
char *filename;
int line;
{
   PTR_BFND b,bpar;
   PTR_DEP d;
   int depth;
   char * s;
   PTR_BLOB1 nb, lb, btmp;

   PTR_BLOB q;
   PTR_SYMB induct_list[100], local_list[100], rename_list[100];
   int induct_num, local_num, rename_num;
   /* PTR_LLND used, modified; */
   PTR_BFND FindBifNode();
   int i;
   
   induct_num = 0; local_num = 0; rename_num = 0;
   b = FindBifNode(filename,line);
   /*  bind_call_site_info(b,&used,&modified);*/
   if(b == NULL){ 
      nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,nb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"Could not find code at line %d\n",line);
      nb->ref = s;
      nb->next = NULL;
      return(nb);
   }
   /* if b is a loop, we look for all loop carried deps for */
   /* this loop.  otherwise just list dependence going out */
   if(b->variant == FOR_NODE || b->variant == WHILE_NODE){
      depth = 0;
      bpar = b;
      current_par_loop = b;
      while(bpar != NULL && bpar->variant != GLOBAL){
	 if(bpar->variant == FOR_NODE ||
	    bpar->variant == CDOALL_NODE ||
	    bpar->variant == WHILE_NODE ||
	    bpar->variant == FORALL_NODE) depth++;
	 bpar = bpar->control_parent;
      }  
      q = b->entry.Template.bl_ptr1;
      nb = (PTR_BLOB1) malloc(sizeof(struct blob1));
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,nb, 0);
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"Loop Carried Dependences Prohibiting Parallelism:\n");
      nb->ref = s;
      nb->next = NULL;
      nb = Nsearch_deps(nb,q,depth,induct_list, &induct_num,
			local_list,&local_num, rename_list, &rename_num);
      if (nb->next == NULL)
      {
          if (induct_num == 0 && local_num == 0 && rename_num == 0)
              sprintf(nb->ref, "this loop is perfect! parallelize it.\n");
          else
              sprintf(nb->ref,
              "Loop is Parallelizable. First fix these problems.\n");
      }
      for(lb = nb; lb->next != NULL; lb = lb->next);
      if(induct_num > 0){
	 btmp = (PTR_BLOB1) malloc(sizeof(struct blob1));
	 lb->next = btmp;  lb = btmp;
	 s = malloc(256);
#ifdef __SPF
     addToCollection(__LINE__, __FILE__,btmp, 0);
     addToCollection(__LINE__, __FILE__,s, 0);
#endif
	 sprintf(s,"The following seem to be pseudo induction variables:\n");
	 lb->ref = s;
	 lb->next = NULL;
	 for(i = 0; i < induct_num; i++){
	    lb->next = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,lb->next, 0);
#endif
	    lb = lb->next;
	    s = malloc(3+strlen(induct_list[i]->ident) );
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,s, 0);
#endif
	    sprintf(s,"%s\n",induct_list[i]->ident);
	    lb->next = NULL;
	    lb->ref = s;
	 }
	 subtract_list(induct_list,&induct_num,local_list,&local_num);
	 subtract_list(induct_list,&induct_num,rename_list,&rename_num);
      }
      if(local_num > 0){
	 btmp = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
     addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
	 lb->next = btmp;  lb = btmp;
	 s = malloc(256);
#ifdef __SPF
     addToCollection(__LINE__, __FILE__,s, 0);
#endif
	 sprintf(s,"Variables that should be made local to loop:\n");
	 lb->ref = s;
	 lb->next = NULL;
	 for(i = 0; i < local_num; i++){
	    lb->next = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,lb->next, 0);
#endif
	    lb = lb->next;
	    s = malloc(3+strlen(local_list[i]->ident));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,s, 0);
#endif
	    sprintf(s,"%s\n",local_list[i]->ident);
	    lb->next = NULL;
	    lb->ref = s;
	 }
	 subtract_list(local_list, &local_num, rename_list, &rename_num);
      }
      if(rename_num > 0){
	 btmp = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
     addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
	 lb->next = btmp;  lb = btmp;
	 s = malloc(256);
#ifdef __SPF
     addToCollection(__LINE__, __FILE__,s, 0);
#endif
	 sprintf(s,"Variables that are reused in a funny way:\n");
	 lb->ref = s;
	 lb->next = NULL;
	 for(i = 0; i < rename_num; i++){
	    lb->next = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,lb->next, 0);
#endif
	    lb = lb->next;
	    s = malloc(64);
#ifdef __SPF
        addToCollection(__LINE__, __FILE__,s, 0);
#endif
	    sprintf(s,"%s\n",rename_list[i]->ident);
	    lb->next = NULL;
	    lb->ref = s;
	 }
      }
      return(nb);
   }				/* if loop case */
   d = b->entry.Template.dep_ptr1;
   nb = NULL; 
   btmp = (PTR_BLOB1) malloc(sizeof(struct blob1));
#ifdef __SPF
   addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
   s = malloc(256);
#ifdef __SPF
   addToCollection(__LINE__, __FILE__,s, 0);
#endif
   sprintf(s,"variant of this node is %d\n",b->variant);
   btmp->ref = s;
   btmp->next = NULL;
   nb = lb = btmp;
   while(d != NULL){
      btmp = (PTR_BLOB1) malloc( sizeof(struct blob1));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
      if (nb == NULL){ nb = btmp; lb = btmp;}
      else{ lb->next = btmp; lb = btmp;}
      s = malloc(256);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,s, 0);
#endif
      sprintf(s,"id:%s type:%s to line %d dir_vect =(%s,%s,%s)\n",
	      d->symbol->ident, depstrs[(int) (d->type)], 
	      d->to.stmt->g_line,
	      dirstrs[(int) (d->direct[1])], dirstrs[(int) (d->direct[2])], 
	      dirstrs[(int) (d->direct[3])]);
      btmp->ref = s;
      btmp->next = NULL;
      d = d->from_fwd;
   }
   return(nb);
}

static void subtract_list(a,na, b, nb)
PTR_SYMB a[], b[];
int *na, *nb;
{
   int i, j;
   for(i = 0; i < *na; i++){
      for(j = 0; j < *nb; j++){
	 if(a[i] == b[j]){
	    if(j < *nb-1) b[j] = b[*nb -1];
	    (*nb)--;
	 }
      }
   }
}

int pointer_as_array(d)
PTR_DEP d;
{
   /*
     if(d->from.refer == NULL) fprintf(stderr, "no from llnode\n");
     if(d->to.refer == NULL) fprintf(stderr, "no to llnode\n");
     fprintf(stderr, " from <%s to <%s\n",
     unparse_llnd(d->from.refer), unparse_llnd(d->to.refer));
     */
   if (d->to.refer->variant == ARRAY_REF || d->from.refer->variant==ARRAY_REF)
      return 1;
   else return 0;
}

static PTR_BLOB1
   Nsearch_deps(nb,q,depth,induct_list, induct_num,
		local_list,local_num,rename_list,rename_num)
PTR_BLOB1	nb;
PTR_BLOB	q;
int	depth;
PTR_SYMB induct_list[], local_list[], rename_list[];
int *induct_num, *local_num, *rename_num;
{
   PTR_BFND  bchild;
   PTR_DEP   d;
   char *s;
   PTR_BLOB1 lb = NULL, btmp;
   int i,found;
   PTR_LLND from_list[500];
   int from_line[500], to_line[500];
   int from_num;
   
   if(nb != NULL) lb = nb;
   from_num = 0;
   while(q != NULL){
      bchild = q->ref;
      q = q->next;
      d = bchild->entry.Template.dep_ptr1;
      while(d != NULL){
	 /* if the dependence is a carried array dependence (on a array type */
	 /* or used as an array (fix)) or it is a flow dependence that is    */
	 /* caried then classify appropriately. */
	 if (((d->symbol->type->variant == T_ARRAY || pointer_as_array(d)) &&
	      d->direct[depth] >1) || (d->type == 0 && d->direct[depth] >1)){
	    /* this is a loop carried flow dependence */
	    if(d->from.stmt == d->to.stmt && 
	       (d->symbol->type->variant == T_INT ||
	       (pointer_as_array(d) == 0 &&
	       d->symbol->type->variant == T_POINTER) )){
			for(i = 0, found = 0; i < *induct_num; i++) 
				if( induct_list[i] == d->symbol) found = 1;
			if(found == 0) induct_list[(*induct_num)++] = d->symbol;
		}
	    else if(same_loop(d->from.stmt,d->to.stmt)){
	       found = 0;
	       for(i = 0; i < from_num; i++)
			if(d->from.refer == from_list[i] && d->from.stmt->g_line == from_line[i]
			   && d->to.stmt->g_line == to_line[i]) found = 1;
	       if(found == 0){
			btmp = (PTR_BLOB1) malloc( sizeof(struct blob1));
#ifdef __SPF
            addToCollection(__LINE__, __FILE__,btmp, 0);
#endif
			if (nb == NULL){ nb = btmp; lb = btmp;}
			else{ lb->next = btmp; lb = btmp;}
			s = malloc(256);
#ifdef __SPF
            addToCollection(__LINE__, __FILE__,s, 0);
#endif     
			sprintf(s, "an assignment to %s at line %d used in line %d in another iteration\n",
				(UnparseLlnd[cur_file->lang])(d->from.refer),
				d->from.stmt->g_line, d->to.stmt->g_line);
			btmp->ref = s;
			btmp->next = NULL;
			from_list[from_num] = d->from.refer;
			from_line[from_num] = d->from.stmt->g_line;
			to_line[from_num++] = d->to.stmt->g_line;
			}
		}
	 }
	 else if(d->symbol->type->variant != T_ARRAY && d->type != 0 &&
		 d->direct[depth] > 1 && same_loop(d->from.stmt,d->to.stmt)){
	    /* this is a loop caried output or anti dep */
	    /* add symbol to list for suggestion for localization */
	    for(i = 0, found = 0; i < *local_num; i++) 
	       if( local_list[i] == d->symbol) found = 1;
	    if(found == 0) local_list[(*local_num)++] = d->symbol;
	 }
	 else if(d->type == 2 && d->direct[depth] <= 1 &&
		 same_loop(d->from.stmt,d->to.stmt)){
	    /* this is an output dependence of distance 0 */
	    /* suggest renaming. */
	    for(i = 0, found = 0; i < *rename_num; i++) 
	       if( rename_list[i] == d->symbol) found = 1;
	    if(found == 0) rename_list[(*rename_num)++] = d->symbol;
	 }
	 d = d->from_fwd;
      }
      if(bchild->entry.Template.bl_ptr1 != NULL){
	 nb = Nsearch_deps(nb,bchild->entry.Template.bl_ptr1,depth,induct_list,
			   induct_num, local_list,
			   local_num, rename_list, rename_num);
	 lb = nb; while(lb != NULL && lb->next != NULL) lb = lb->next;
      }
      if(bchild->entry.Template.bl_ptr2 != NULL){
	 nb = Nsearch_deps(nb,bchild->entry.Template.bl_ptr2,depth,induct_list,
			   induct_num, local_list,
			   local_num, rename_list, rename_num);
	 lb = nb; while(lb != NULL && lb->next != NULL) lb = lb->next;
      }
   }
   return(nb);
}

static int same_loop(from, to)
PTR_BFND from, to;
{
   PTR_BFND c;
   c = from;
   while(c != NULL && c->variant != GLOBAL && c != current_par_loop)
      c = c->control_parent;
   if(c != current_par_loop) return(0);
   c = to;
   while(c != NULL && c->variant != GLOBAL && c != current_par_loop)
      c = c->control_parent;
   if(c != current_par_loop) return(0);
   return(1);
}


