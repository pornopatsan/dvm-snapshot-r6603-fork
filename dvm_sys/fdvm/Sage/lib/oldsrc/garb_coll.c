/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "db.h"



PTR_LLND free_ll_list = NULL;
static int num_marked;
int num_ll_allocated = 0;


static void
mark_llnd(p)
PTR_LLND p;
{
	if(p == NULL || p->id == -1)
		return;
	p->id = -1; num_marked++;
	mark_llnd(p->entry.Template.ll_ptr1);
	mark_llnd(p->entry.Template.ll_ptr2);
}


static void
mark_refl(p)
	PTR_REFL p;
{
	for (; p; p = p->next)
		if(p->node != NULL) 
			mark_llnd(p->node->refer);
}
	

static void
mark_arefl(p)
	PTR_AREF p;
{
	for (; p; p = p->next){
   		mark_llnd(p->decl_ranges);
   		mark_llnd(p->use_bnd0); 
   		mark_llnd(p->mod_bnd0);
   		mark_llnd(p->use_bnd1); 
   		mark_llnd(p->mod_bnd1);
   		mark_llnd(p->use_bnd2);
   		mark_llnd(p->mod_bnd2);
	}
}


static void
mark_sets(s)
	struct sets *s;
{
	if(s == NULL) return;

	mark_refl(s->gen);
	mark_refl(s->in_def);
	mark_refl(s->use);
	mark_refl(s->in_use);
	mark_refl(s->out_def);
	mark_refl(s->out_use);
	mark_arefl(s->arefl);
}


static void
mark_depnds(p)
	PTR_DEP p;
{
	int depcnt;
	depcnt = 0;

	for (; p != NULL; p = p->thread){
		mark_llnd(p->to.refer);
		mark_llnd(p->from.refer);
		depcnt++;
	}
}


static void
mark_symb(fi)
	PTR_FILE fi;
{
	PTR_SYMB s;

	for (s = fi->head_symb; s; s = s->thread) {
		if (s->variant == CONST_NAME)
			mark_llnd(s->entry.const_value);
      		else if(s->variant == FIELD_NAME)
			mark_llnd(s->entry.field.restricted_bit);
		else if(s->variant == VAR_FIELD)
			mark_llnd(s->entry.variant_field.variant_list);
		else if (s->variant == PROCEDURE_NAME || 
			 s->variant == FUNCTION_NAME)
			mark_llnd(s->entry.proc_decl.call_list);
		else if(s->variant == MEMBER_FUNC)
			mark_llnd(s->entry.member_func.call_list);

	}
}


static void
mark_type(fi)
	PTR_FILE fi;
{
	PTR_TYPE s;
	for (s = fi->head_type; s; s = s->thread) {
		if(s->variant == T_ARRAY)
			mark_llnd(s->entry.ar_decl.ranges);
		else if(s->variant == T_DESCRIPT || 
			s->variant == T_POINTER ||
			s->variant == T_LIST ||
			s->variant == T_FUNCTION)
			mark_llnd(s->entry.Template.ranges);
		else if(s->variant == T_SUBRANGE){
			mark_llnd(s->entry.subrange.lower);
			mark_llnd(s->entry.subrange.upper);
			}
		else{
		 mark_llnd(s->entry.Template.ranges);
		 }
	      }
}



static void
mark_bfnd(b)
	PTR_BFND b;
{
	PTR_BLOB bl;

	if(b == NULL) return;

	mark_llnd(b->entry.Template.ll_ptr1);
	mark_llnd(b->entry.Template.ll_ptr2);
	mark_llnd(b->entry.Template.ll_ptr3);
	mark_sets(b->entry.Template.sets); 

	for (bl = b->entry.Template.bl_ptr1; bl; bl = bl->next)
		mark_bfnd(bl->ref);

	for (bl = b->entry.Template.bl_ptr2; bl; bl = bl->next)
		mark_bfnd(bl->ref);
}


void
collect_garbage(fi)
	PTR_FILE fi;
{
	PTR_LLND p, t;
	int count;

	p = free_ll_list;
	count = 0;
	while(p != NULL){
		count++;
		p = p->thread;
	}

	count = 0;
	for (p = fi->head_llnd; p && p != fi->cur_llnd; p = p->thread){
		p->id = 0;
		count++;
	}

	fi->cur_llnd->id = 0; count++;

	num_marked = 0;
	mark_bfnd(fi->head_bfnd);
	/* printf("num marked from bfnd = %d\n", num_marked); */

	num_marked = 0;
	mark_depnds(fi->head_dep);
	/* printf("num marked from deps= %d\n", num_marked); */

	num_marked = 0;
	mark_symb(fi);
	/* printf("num marked from symb= %d\n", num_marked); */

	num_marked = 0;
	mark_type(fi);
	/* printf("num marked from type= %d\n", num_marked); */

	num_marked = 0;
	p = fi->head_llnd;
	fi->cur_llnd = fi->head_llnd;
	count = 1;
	p->id = count++; p = p->thread;
	fi->cur_llnd->thread = NULL;
    
	while(p != NULL){
		if(p->id == -1){ /*touched */
			fi->cur_llnd->thread = p;
			fi->cur_llnd = p;
                        p = p->thread;
			fi->cur_llnd->id = count++;
			fi->cur_llnd->thread = NULL;
		} else if(p->id == 0) {
			t = p; p = p->thread;
			t->id = -2; num_marked++;
			t->thread= free_ll_list;
			t->entry.Template.ll_ptr1 = NULL;
			t->entry.Template.ll_ptr2 = NULL;
			t->entry.Template.symbol = NULL;
			t->variant = 800;
			free_ll_list = t;
		      }
               else { printf("error in garbage collection\n");
		      exit(0);
		    }
		}
	fi->num_llnds = count -1 ;
	num_ll_allocated = 0;
	printf(" total llnodes = %d garbage collected = %d\n",count, num_marked);
}

int num_of_llnds(fi)
PTR_FILE fi;
{ return fi->num_llnds; }
