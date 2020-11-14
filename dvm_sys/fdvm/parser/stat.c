/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#define EXTEND_NODE 2  /* move this definition to h/ files. */
                       /* should agree with cftn.gram definition. */
/*
 * stat.c
 *
 * Routines for handling Fortran statements
 *
 */

#include "inc.h"
#include "defines.h"
#include "extern.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

extern int parstate;
extern PTR_SYMB head_symb, global_list, star_symb;
extern PTR_BFND cur_bfnd, global_bfnd, pred_bfnd, cur_scope();
extern PTR_BFND last_bfnd; /*OMP*/
extern PTR_TYPE global_float, global_int, global_bool, global_default, vartype;
extern PTR_LABEL head_label;
extern PTR_LABEL thislabel;
extern PTR_TYPE impltype[];
extern PTR_BLOB head_blob, cur_blob;
extern PTR_CMNT comments;
extern int yylineno;
extern int mod_offset;
extern int nioctl;
extern int yydebug;

void fatalstr();
void execerr();
int chk_params();
void err();
void setimpl();

PTR_BFND get_bfnd();
PTR_LLND make_llnd();
PTR_SYMB make_symb();
PTR_BLOB make_blob();
PTR_LABEL make_label();
PTR_SYMB install_entry();
PTR_SYMB get_proc_symbol();
PTR_HASH correct_symtab();
int end_group = 0;

/* 
   The following two routines are used for reading in input/output
   control lists. 
*/

void
startioctl()
{

	inioctl = YES;
        nioctl = 0; 

}

void
endioctl()
{
	inioctl = NO;

}


/*
 * Follow a chain of blob nodes to get the last
 *
 * input:
 *	 blob - the list to be searched
 *
 * output:
 *	 pointer to the last node in the list
 */
PTR_BLOB
follow_blob(blob)
	PTR_BLOB blob;
{
	register PTR_BLOB next, last;

	for (next = last = blob; next; next = next->next)
		last = next;
	return (last);
}


/* 
 * make_if takes an expression to make an IF_NODE
 * Also allocates a collection point and points the false branch
 * to the collection point
 */
PTR_BFND
make_if(expr)
	PTR_LLND expr;
{
	PTR_BFND p;
	void set_blobs(), make_prog_header();
	void err();

	/*
        if (pred_bfnd->variant == GLOBAL)
	  make_prog_header();
	*/
	
	/*	if (expr->type->variant != T_BOOL) {
		err("Non-logical expression in IF statement", 28);
		expr = LLNULL;
	}
        */ /*06.06.03*/ 
	p = get_bfnd(fi,IF_NODE, SMNULL, expr, LLNULL, LLNULL);
/*	set_blobs(p, pred_bfnd, NEW_GROUP1); */
	return (p);
}


PTR_BFND
make_forall(lexpr,expr)
	PTR_LLND expr,lexpr;
{
	PTR_BFND p;
	void set_blobs(), make_prog_header();
	void err();

	/*
        if (pred_bfnd->variant == GLOBAL)
	  make_prog_header();
	*/
	/*	if (expr && expr->type->variant != T_BOOL) {
		err("Non-logical expression in FORALL statement", 288);
		expr = LLNULL;
	}
         */ /*06.06.03*/ 
	p = get_bfnd(fi,FORALL_NODE, SMNULL, lexpr, expr, LLNULL);
/*	set_blobs(p, pred_bfnd, NEW_GROUP1); */
	return (p);
}

/*
 * make_elseif fixes the control frame to reflect the current paring state
 */

void
make_elseif(expr,s)
	PTR_LLND expr;
        PTR_SYMB s;  /*podd 3.02.03*/
{
	register PTR_BFND p = NULL;
	void execerr(), make_endblock();

	/*	if (expr->type->variant != T_BOOL) {
		err("Non-logical expression in IF statement", 28);
		expr = LLNULL;
	}
        */ /*06.06.03*/ 
	if (pred_bfnd->variant == IF_NODE || pred_bfnd->variant == ELSEIF_NODE)
		p = get_bfnd(fi,ELSEIF_NODE, s, expr, LLNULL, LLNULL);
	else
		err("ELSEIF out of place", 31);
	p->control_parent = pred_bfnd;
	pred_bfnd->entry.Template.bl_ptr2 = make_blob(fi,p, BLNULL);
	pred_bfnd = p;
	cur_blob = p->entry.Template.bl_ptr1 = make_blob(fi,BFNULL, BLNULL);
}

void
make_elsewhere_mask(expr,s)
	PTR_LLND expr;
        PTR_SYMB s;  /*podd 15.02.03*/
{
	register PTR_BFND p = NULL;
	void execerr(), make_endblock();

	/*if (expr->type->variant != T_BOOL) {
		err("Non-logical expression in IF statement", 28);
		expr = LLNULL;
		}*/
	if (pred_bfnd->variant == WHERE_BLOCK_STMT || pred_bfnd->variant == ELSEWH_NODE)
		p = get_bfnd(fi,ELSEWH_NODE, s, expr, LLNULL, LLNULL);
	else
		err("ELSEWHERE out of place", 291);
	p->control_parent = pred_bfnd;
	pred_bfnd->entry.Template.bl_ptr2 = make_blob(fi,p, BLNULL);
	pred_bfnd = p;
	cur_blob = p->entry.Template.bl_ptr1 = make_blob(fi,BFNULL, BLNULL);
}



/*
 * make_else fixes the control stack to reflect the current state of parsing
 * and put all BIF nodes after the saved control frame as the true branch of
 * the If statement
 */
void
make_else(s)
PTR_SYMB s;  /*podd 3.02.03*/
{
	void execerr(), set_blobs();
	PTR_BFND p;

	if (pred_bfnd->variant != IF_NODE && pred_bfnd->variant != ELSEIF_NODE)
	  err("ELSE out of place", 32);
	p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
        if (is_openmp_stmt) { /*OMP*/
            is_openmp_stmt = 0; /*OMP*/
            p -> decl_specs = BIT_OPENMP; /*OMP*/
        } /*OMP*/
	set_blobs(p, pred_bfnd, SAME_GROUP);
        cur_blob = pred_bfnd->entry.Template.bl_ptr2
	  = make_blob(fi,BFNULL, BLNULL);
}

void
make_elsewhere(s)
     PTR_SYMB s; /*15.02.03*/
{
	void execerr(), set_blobs();
	PTR_BFND p;

	if (pred_bfnd->variant != WHERE_BLOCK_STMT  && pred_bfnd->variant != ELSEWH_NODE)
	   err("ELSEWHERE out of place", 291 );
	p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
        if (is_openmp_stmt) { /*OMP*/
            is_openmp_stmt = 0; /*OMP*/
            p -> decl_specs = BIT_OPENMP; /*OMP*/
        } /*OMP*/
	set_blobs(p, pred_bfnd, SAME_GROUP);
        cur_blob = pred_bfnd->entry.Template.bl_ptr2
	  = make_blob(fi,BFNULL, BLNULL);
}


/*
 * make_endblock sets up the statement list to the proper branch according
 * to the control stack's status and then pop the control stack.
 */
void
make_endblock(p) 
PTR_BFND p;
{
	PTR_BFND past_pred;
	PTR_BLOB q;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL))
	   fatalstr("Illegal end of block",(char *)NULL,258);
	if (cur_blob->ref==BFNULL) { /* empty block body */
/*	  pred_bfnd->entry.Template.bl_ptr1 = BLNULL; */
	  cur_blob->ref = p;
        }
	else
	   if (p) set_blobs(p, pred_bfnd, SAME_GROUP);
	past_pred = pred_bfnd;
	pred_bfnd = pred_bfnd->control_parent;
	if (pred_bfnd->variant == GLOBAL) parstate = OUTSIDE;
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	cur_blob = (q && (q->ref == past_pred)) ?
	             q
		    :follow_blob(pred_bfnd->entry.Template.bl_ptr2);
}


void
make_endif(s) 
PTR_SYMB s;  /*podd 3.02.03*/
{      
	PTR_BFND p, past_pred;
	PTR_BLOB q;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || ((pred_bfnd->variant != IF_NODE) &&
	        (pred_bfnd->variant != ELSEIF_NODE))) {
		fatalstr("Illegal END IF", (char *)NULL, 260);
	}
	if ((!cur_blob->ref) || (cur_blob->ref->variant != CONTROL_END)) {
	   p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
	   p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
           if (is_openmp_stmt) { /*OMP*/
              is_openmp_stmt = 0; /*OMP*/
              p -> decl_specs = BIT_OPENMP; /*OMP*/
           } /*OMP*/
           /*change by podd*/
	   if(thislabel){
              thislabel->statbody = p;
              thislabel->labtype = LABEXEC;
              p->label = thislabel;   
           }                   
	   set_blobs(p, pred_bfnd, SAME_GROUP);
	}
	while (pred_bfnd->variant == ELSEIF_NODE)
	  pred_bfnd = pred_bfnd->control_parent;
	past_pred = pred_bfnd;
	pred_bfnd = pred_bfnd->control_parent;
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	cur_blob = (q && (q->ref == past_pred)) ?
	             q
		    :follow_blob(pred_bfnd->entry.Template.bl_ptr2);
/*	cur_blob = follow_blob(pred_bfnd->entry.Template.bl_ptr1);*/
}

void
make_endwhere(s) 
     PTR_SYMB s;  /*15.02.03*/
{
	PTR_BFND p, past_pred;
	PTR_BLOB q;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || ((pred_bfnd->variant != WHERE_BLOCK_STMT) &&
	        (pred_bfnd->variant != ELSEWH_NODE))) {
		fatalstr("Illegal ENDWHERE", (char *)NULL, 290);
	}
	if ((!cur_blob->ref) || (cur_blob->ref->variant != CONTROL_END)) {
	   p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
           if (is_openmp_stmt) { /*OMP*/
              is_openmp_stmt = 0; /*OMP*/
              p -> decl_specs = BIT_OPENMP; /*OMP*/
           } /*OMP*/
	   set_blobs(p, pred_bfnd, SAME_GROUP);
           /*change by podd*/
	   if(thislabel){
              thislabel->statbody = p;
              thislabel->labtype = LABEXEC;
              p->label = thislabel;   
           }                   
	}
	/* pred_bfnd = pred_bfnd->control_parent;*/
	/* cur_blob = follow_blob(pred_bfnd->entry.Template.bl_ptr1);*/
	while (pred_bfnd->variant == ELSEWH_NODE)
	  pred_bfnd = pred_bfnd->control_parent;
	past_pred = pred_bfnd;
	pred_bfnd = pred_bfnd->control_parent;
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	cur_blob = (q && (q->ref == past_pred)) ?
	             q
		    :follow_blob(pred_bfnd->entry.Template.bl_ptr2);
}

void
make_endforall(s) 
     PTR_SYMB s; 
{
	PTR_BFND p, past_pred;
	PTR_BLOB q;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || (pred_bfnd->variant != FORALL_NODE)) {
		fatalstr("Illegal ENDFORALL", (char *)NULL, 289);
	}
	if ((!cur_blob->ref) || (cur_blob->ref->variant != CONTROL_END)) {
	   p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
           if (is_openmp_stmt) { /*OMP*/
              is_openmp_stmt = 0; /*OMP*/
              p -> decl_specs = BIT_OPENMP; /*OMP*/
           } /*OMP*/
	   set_blobs(p, pred_bfnd, SAME_GROUP);
           /*change by podd*/
	   if(thislabel){
              thislabel->statbody = p;
              thislabel->labtype = LABEXEC;
              p->label = thislabel;   
           }                   
	}
	past_pred = pred_bfnd;
	pred_bfnd = pred_bfnd->control_parent;
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	cur_blob = (q && (q->ref == past_pred)) ?
	             q
		    :follow_blob(pred_bfnd->entry.Template.bl_ptr2);
}

void
make_endselect(s) 
     PTR_SYMB s; 
{
	PTR_BFND p, past_pred;
	PTR_BLOB q;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || (pred_bfnd->variant != SWITCH_NODE)) {
		fatalstr("Illegal ENDSELECT", (char *)NULL, 286);
	}
	if ((!cur_blob->ref) || (cur_blob->ref->variant != CONTROL_END)) {
	   p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
           if (is_openmp_stmt) { /*OMP*/
              is_openmp_stmt = 0; /*OMP*/
              p -> decl_specs = BIT_OPENMP; /*OMP*/
           } /*OMP*/
	   set_blobs(p, pred_bfnd, SAME_GROUP);
           
	   if(thislabel){
              thislabel->statbody = p;
              thislabel->labtype = LABEXEC;
              p->label = thislabel;   
           }                   
	}
	past_pred = pred_bfnd;
	pred_bfnd = pred_bfnd->control_parent;
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	cur_blob = (q && (q->ref == past_pred)) ?
	             q
		    :follow_blob(pred_bfnd->entry.Template.bl_ptr2);
}


void
make_endextend() 
{
	PTR_BFND p;
	void set_blobs();

	if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || ((pred_bfnd->variant != PDO_NODE) &&
	        (pred_bfnd->variant != PSECTIONS_NODE))) {
/*		fatalstr("Illegal end extend[ed] statement\n", (char *)NULL);*/
	}
	if ((!cur_blob->ref) || (cur_blob->ref->variant != CONTROL_END)) {
	   p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
           if (is_openmp_stmt) { /*OMP*/
              is_openmp_stmt = 0; /*OMP*/
              p -> decl_specs = BIT_OPENMP; /*OMP*/
           } /*OMP*/
	   set_blobs(p, pred_bfnd, SAME_GROUP);
	}
	pred_bfnd = pred_bfnd->control_parent;
	cur_blob = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	if (cur_blob->ref->variant == CONTROL_END)
	cur_blob = follow_blob(pred_bfnd->entry.Template.bl_ptr2);
}

/* 18th, Dec. 90 */
/* modified to take care of PCF FORTRAN's variety of END statements */

PTR_BFND
make_enddoall()
{
        PTR_BFND p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	PTR_BLOB q;
        if (is_openmp_stmt) { /*OMP*/
           is_openmp_stmt = 0; /*OMP*/
           p -> decl_specs = BIT_OPENMP; /*OMP*/
        } /*OMP*/
	if ((!pred_bfnd) ||
	    ((pred_bfnd->variant != CDOALL_NODE) &&
	     (pred_bfnd->variant != SDOALL_NODE) &&
	     (pred_bfnd->variant != DOACROSS_NODE) &&
	     (pred_bfnd->variant != CDOACROSS_NODE) &&
             (pred_bfnd->variant != PARDO_NODE) &&
             (pred_bfnd->variant != PROCESS_DO_STAT) &&
             (pred_bfnd->variant != PDO_NODE)))
	   execerr("enddoall statement out of place", (char *)NULL);
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	if (q != cur_blob) {
	   pred_bfnd->entry.Template.bl_ptr1 =
                 pred_bfnd->entry.Template.bl_ptr2;
	   pred_bfnd->entry.Template.bl_ptr2 = BLNULL;
	}
	return p;
}

PTR_BFND
make_endprocesses()
{
        PTR_BFND p = get_bfnd(fi,PROCESSES_END, SMNULL, LLNULL, LLNULL, LLNULL);
	PTR_BLOB q;

	if ((!pred_bfnd) || (pred_bfnd->variant != PROCESSES_STAT))
	   execerr("endprocesses statement out of place", (char *)NULL);
	q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	if (q != cur_blob) {
	   pred_bfnd->entry.Template.bl_ptr1 =
                 pred_bfnd->entry.Template.bl_ptr2;
	   pred_bfnd->entry.Template.bl_ptr2 = BLNULL;
	}
	return p;
}


void
make_loop()
{
	PTR_BFND p;
	void set_blobs();

	if ((pred_bfnd->variant != CDOALL_NODE) &&
	    (pred_bfnd->variant != SDOALL_NODE) &&
	    (pred_bfnd->variant != DOACROSS_NODE) &&
	    (pred_bfnd->variant != CDOACROSS_NODE))
	  execerr("loop statement out of place", (char *)NULL);
	p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
        if (is_openmp_stmt) { /*OMP*/
           is_openmp_stmt = 0; /*OMP*/
           p -> decl_specs = BIT_OPENMP; /*OMP*/
        } /*OMP*/
	set_blobs(p, pred_bfnd, SAME_GROUP);
	cur_blob = pred_bfnd->entry.Template.bl_ptr1
	  = make_blob(fi,(PTR_BFND) NULL, (PTR_BLOB) NULL);
}


/*
 * Setup of logical IF statement
 */
PTR_BFND
make_logif(if_stmt, body)
	PTR_BFND if_stmt, body;
{
        /*PTR_BFND temp;*/
        PTR_BLOB new_blob;
	void set_blobs(), make_prog_header();

/*
	if (expr->type->variant != T_BOOL) {
		err("non-logical expression in IF statement");
		expr = LLNULL;
	} 
	if (pred_bfnd->variant == GLOBAL)
	  make_prog_header();
	temp = get_bfnd(fi,LOGIF_NODE, SMNULL, expr, LLNULL, LLNULL);
	set_blobs(temp, pred_bfnd, SAME_GROUP);
*/
	new_blob = make_blob(fi,body, BLNULL);
	if_stmt->entry.Template.bl_ptr1 = new_blob;
	body->control_parent = if_stmt;
	return (if_stmt);
}

void
make_extend(s)
     PTR_SYMB s;
{
	void execerr(), set_blobs();
	PTR_BFND p, past_pred;
	PTR_BLOB q;
       if ((pred_bfnd==BFNULL) || (pred_bfnd->control_parent==BFNULL)
	     || ((pred_bfnd->variant != FOR_NODE) &&
	         (pred_bfnd->variant != WHILE_NODE))) {
		fatalstr("Illegal END DO",(char *)NULL, 259);
	}
	p = get_bfnd(fi,CONTROL_END, s, LLNULL, LLNULL, LLNULL);
	if (is_openmp_stmt) { /*OMP*/
		is_openmp_stmt = 0;  /*OMP*/
		p -> decl_specs = BIT_OPENMP; /*OMP*/
	} /*OMP*/
           /*change by podd*/
	   if(thislabel){
              thislabel->statbody = p;
              thislabel->labtype = LABEXEC;
              p->label = thislabel;   
           }                   
	set_blobs(p, pred_bfnd, SAME_GROUP);
	if ((pred_bfnd->variant == PDO_NODE) &&
            (pred_bfnd->index == EXTEND_NODE))
	{
	     cur_blob = pred_bfnd->entry.Template.bl_ptr2
		                          = make_blob(fi,BFNULL, BLNULL);
        }
	else 
	{
	     past_pred = pred_bfnd;
	     pred_bfnd = pred_bfnd->control_parent;
	     q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	     cur_blob = (q && (q->ref == past_pred)) ?
		         q : follow_blob(pred_bfnd->entry.Template.bl_ptr2);
	}
}

void
make_section_extend()
{
	void execerr(), set_blobs();
	PTR_BFND p, past_pred, set_stat_list();
	PTR_BLOB q;
	
   {
	/* mark end of section */
	p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	set_stat_list(pred_bfnd, p);
	/* mark end of psections's sections */
	p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	set_blobs(p, pred_bfnd, SAME_GROUP);
	/* prepare for extend statements of psections in case */
	if ((pred_bfnd->variant == PSECTIONS_NODE) && 
            (pred_bfnd->index == EXTEND_NODE))
	{
	     cur_blob = pred_bfnd->entry.Template.bl_ptr2
		                          = make_blob(fi,BFNULL, BLNULL);
        }
	else 
	{
	     past_pred = pred_bfnd;
	     pred_bfnd = pred_bfnd->control_parent;
	     q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	     cur_blob = (q && (q->ref == past_pred)) ?
		         q : follow_blob(pred_bfnd->entry.Template.bl_ptr2);
	}
   }
}

void
make_section(section_name, wait_list)
PTR_LLND section_name, wait_list;
{
	void execerr(), set_blobs();
	PTR_BFND p, set_stat_list();
	
   {
	if (pred_bfnd->variant == SECTION_NODE)
	{
	     p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	     set_stat_list(pred_bfnd, p);
	}
	if ((pred_bfnd->variant == PSECTIONS_NODE) ||
            (pred_bfnd->variant == PARSECTIONS_NODE))
	{
        p = get_bfnd(fi,SECTION_NODE, SMNULL, section_name, wait_list, LLNULL);
	set_stat_list(pred_bfnd, p);
/*	pred_bfnd = p;*/
	}
	else printf("stat.c:make_section: SECTION NODE's attachment point is not a PSECTION NODE\n"); 
	
   }
}

/* 
 * procedure_call gets an id and parameter list.
 * It handles procedures not seen yet
 */
PTR_SYMB
procedure_call(entry)
PTR_HASH entry;
{
	register PTR_SYMB symb_ptr;

	entry = correct_symtab(entry, PROCEDURE_NAME);
	for (symb_ptr = entry->id_attr; symb_ptr; symb_ptr = symb_ptr->outer)
		if (symb_ptr->variant == PROCEDURE_NAME)
			return (symb_ptr);

	symb_ptr = get_proc_symbol(entry);
	symb_ptr->variant = PROCEDURE_NAME;
	symb_ptr->type = global_default;
	symb_ptr->entry.proc_decl.seen = NO;
	return (symb_ptr);
}


/*
 * proceduer match_parameters gets a proc_id and param_list and performs type
 * checking
 */
void
match_parameters(proc_id, param_list)
	PTR_SYMB proc_id;
	PTR_LLND param_list;
{
	PTR_LLND new;
	void err();

	new = make_llnd(fi,FUNC_CALL, param_list, LLNULL, proc_id);
	if (proc_id->entry.proc_decl.seen == YES)
		if (!chk_params(proc_id->entry.proc_decl.in_list, param_list))
		  /*	err(" Parameter mismatch ")*/ /*podd*/ ;

 /*
  * if procedure declaration not seen yet and otherwise too add this call to
  * the call list 
  */
	new->entry.proc.next_call = proc_id->entry.proc_decl.call_list;
	proc_id->entry.proc_decl.call_list = new;
}


/*
 * chk_params checks formals against actuals -- to be added later
 */
int
chk_params(formal, actual)
	PTR_SYMB formal;
	PTR_LLND actual;
{
	return (1);
}


/*
 * set_stat_list  -- links together an old BIF node list with a new one
 *
 * input:
 *	  old_list - old BIF node list
 *	  stat	   - new BIF node
 *
 * output:
 *	  a BIF node list that links these two together
 */
PTR_BFND
set_stat_list(old_list, stat)
	PTR_BFND old_list, stat;
{
	PTR_BFND ret=BFNULL;
	BOOL    start_new_group = NO;
	void fatal(), set_blobs(), make_prog_header(), close_groups();
       
	if (!stat) return (old_list);
        /* The proper place for this piece of code is in cur_scope(). */
	/* if (pred_bfnd->variant == GLOBAL)
	    make_prog_header(); */
	switch (stat->variant) {
 	    case (IF_NODE):
	    case (LOGIF_NODE):
            case (FORALL_STAT):
	          close_groups();
	          return (old_list);
  	    /* start of group */
 
            case (INTERFACE_STMT):
            case (INTERFACE_OPERATOR):
            case (INTERFACE_ASSIGNMENT):
            case (STRUCT_DECL):
	    case (CDOALL_NODE): 
	    case (SDOALL_NODE):
	    case (DOACROSS_NODE):
	    case (CDOACROSS_NODE):
	    case (FOR_NODE):
	    case (PROCESS_DO_STAT):
	    case (WHILE_NODE):
	    case (FORALL_NODE):
            case (PARDO_NODE):
            case (PROCESSES_STAT):
	    case (PDO_NODE):
	    case (PARREGION_NODE):
	    case (PSECTIONS_NODE):
            case (PARSECTIONS_NODE):
	    case (SECTION_NODE):
            case (CRITSECTION_NODE):
            case (SINGLEPROCESS_NODE):
	    case (SWITCH_NODE):
	    case (WHERE_BLOCK_STMT):
	    case (OMP_SECTION_DIR): /*OMP*/
	    case (OMP_PARALLEL_DIR): /*OMP*/
	    case (OMP_SINGLE_DIR): /*OMP*/
	    case (OMP_MASTER_DIR): /*OMP*/
	    case (OMP_CRITICAL_DIR): /*OMP*/
	    case (OMP_ORDERED_DIR): /*OMP*/
            case (OMP_WORKSHARE_DIR): /*OMP*/
	    case (OMP_PARALLEL_SECTIONS_DIR): /*OMP*/
	    case (OMP_PARALLEL_WORKSHARE_DIR): /*OMP*/
          /*  case (ACC_REGION_DIR): */   /*ACC*/
          /*  case (ACC_DATA_REGION_DIR): */   /*ACC*/
	        start_new_group = NEW_GROUP1;
	    /* NO_OP nodes */
	    case (VAR_DECL):
	    case (PARAM_DECL):
	    case (COMM_STAT):
	    case (NAMELIST_STAT):
	    case (PROS_COMM):
	    case (DIM_STAT):
            case (HPF_TEMPLATE_STAT):
            case (HPF_PROCESSORS_STAT):
	    case (DVM_DISTRIBUTE_DIR):
	    case (DVM_ALIGN_DIR):
	    case (DVM_DYNAMIC_DIR):
            case (DVM_SHADOW_DIR):
	    case (DVM_VAR_DECL):
            case (DVM_POINTER_DIR):
            case (DVM_HEAP_DIR):
	    case (DVM_INHERIT_DIR):
	    case (DVM_TASK_DIR):
            case (DVM_REDUCTION_GROUP_DIR):
	    case (DVM_REMOTE_GROUP_DIR):
            case (DVM_INDIRECT_GROUP_DIR):
            case (DVM_CONSISTENT_GROUP_DIR):
            case (DVM_CONSISTENT_DIR):
            case (DVM_ASYNCID_DIR):
            case (ACC_ROUTINE_DIR):
	    case (DATA_DECL):
	    case (EXTERN_STAT):
	    case (INTRIN_STAT):
	    case (EQUI_STAT):
	    case (IMPL_DECL):
	    case (SAVE_DECL):
	    case (INCLUDE_STAT):
	    case (ATTR_DECL):
	    case (INPORT_DECL):
	    case (OUTPORT_DECL):
            case (ALLOCATABLE_STMT):
	    case (SEQUENCE_STMT):
            case (PRIVATE_STMT):
            case (PUBLIC_STMT):
            case (OPTIONAL_STMT):
            case (POINTER_STMT):
            case (TARGET_STMT):
	    case (STATIC_STMT):
	    /* not an assignment stat */
	    case (GOTO_NODE):
	    case (ASSGOTO_NODE):
	    case (COMGOTO_NODE):
	    case (ARITHIF_NODE):
	    case (LOOP_NODE):
	    case (EXIT_NODE):
            case (CONT_STAT):
	    case (RETURN_STAT):
	    case (STOP_STAT):
	    case (PAUSE_NODE):
	    case (CASE_NODE):
	    case (DEFAULT_NODE):
            /* other */
	    case (ASSIGN_STAT):
	    case (STMTFN_STAT):
	    case (SUM_ACC):
	    case (MULT_ACC):
	    case (MAX_ACC):
	    case (MIN_ACC):
	    case (CAT_ACC):
	    case (OR_ACC):
	    case (AND_ACC):
	    case (READ_STAT):
	    case (WRITE_STAT):
	    case (PRINT_STAT):
	    case (BACKSPACE_STAT):
	    case (REWIND_STAT):
	    case (ENDFILE_STAT):
	    case (INQUIRE_STAT):
	    case (OPEN_STAT):
	    case (CLOSE_STAT):
	    case (OTHERIO_STAT):
	    case (FORMAT_STAT):
	    case (PROC_STAT):
            case (PROS_STAT):
            case (PROS_STAT_LCTN):
            case (PROS_STAT_SUBM):
	    case (ASSLAB_STAT):
            case (LOCK_NODE):
	    case (UNLOCK_NODE):
            case (POST_NODE):
	    case (WAIT_NODE):
	    case (CLEAR_NODE):
            case (POSTSEQ_NODE):
	    case (WAITSEQ_NODE):
	    case (SETSEQ_NODE):
	    case (PRIVATE_NODE):
            case (GUARDS_NODE):
	    case (CYCLE_STMT):
	    case (EXIT_STMT):
	    case (CONTAINS_STMT):
	    case (WHERE_NODE):
	    case (USE_STMT):
	    case (MODULE_PROC_STMT):
	    case (OVERLOADED_ASSIGN_STAT): 
	    case (POINTER_ASSIGN_STAT):
	    case (OVERLOADED_PROC_STAT):
            case (INTENT_STMT):
	    case (CHANNEL_STAT):
	    case (MERGER_STAT):
            case (DVM_REDISTRIBUTE_DIR):
            case (DVM_PARALLEL_ON_DIR):
            case (HPF_INDEPENDENT_DIR):
            case (DVM_SHADOW_GROUP_DIR):
            case (DVM_SHADOW_START_DIR):
            case (DVM_SHADOW_WAIT_DIR):
            case (DVM_REDUCTION_START_DIR):
            case (DVM_REDUCTION_WAIT_DIR):
            case (DVM_CONSISTENT_START_DIR):
            case (DVM_CONSISTENT_WAIT_DIR):
	    case (DVM_REALIGN_DIR):
	    case (DVM_NEW_VALUE_DIR):
	    case (DVM_REMOTE_ACCESS_DIR):
            case (DVM_TASK_REGION_DIR):
            case (DVM_END_TASK_REGION_DIR):
            case (DVM_ON_DIR):
            case (DVM_END_ON_DIR):
            case (DVM_MAP_DIR):
            case (DVM_PARALLEL_TASK_DIR):
            case (DVM_RESET_DIR):
            case (DVM_PREFETCH_DIR):
            case (DVM_INDIRECT_ACCESS_DIR):
            case (DVM_OWN_DIR):
            case (DVM_INTERVAL_DIR):
            case (DVM_ENDINTERVAL_DIR):
            case (DVM_EXIT_INTERVAL_DIR):
            case (DVM_DEBUG_DIR):
            case (DVM_ENDDEBUG_DIR):
            case (DVM_TRACEON_DIR):  
            case (DVM_TRACEOFF_DIR):
            case (DVM_BARRIER_DIR):
            case (DVM_CHECK_DIR):
            case (DVM_ASYNCHRONOUS_DIR):
            case (DVM_ENDASYNCHRONOUS_DIR):
            case (DVM_ASYNCWAIT_DIR):
            case (DVM_F90_DIR):
            case (DVM_IO_MODE_DIR):
            case (DVM_CP_CREATE_DIR):
            case (DVM_CP_LOAD_DIR):
            case (DVM_CP_SAVE_DIR):
            case (DVM_CP_WAIT_DIR):
            case (DVM_LOCALIZE_DIR):
            case (DVM_SHADOW_ADD_DIR):
	    case (MOVE_PORT):
            case (DVM_TEMPLATE_CREATE_DIR):
            case (DVM_TEMPLATE_DELETE_DIR):
	    case (SEND_STAT):
	    case (RECEIVE_STAT):
	    case (ENDCHANNEL_STAT):
	    case (PROBE_STAT):
	    case (ALLOCATE_STMT):
	    case (DEALLOCATE_STMT):
	    case (NULLIFY_STMT):
	    case (OMP_DO_DIR): /*OMP*/
	    case (OMP_END_DO_DIR): /*OMP*/
	    case (OMP_PARALLEL_DO_DIR): /*OMP*/
	    case (OMP_END_PARALLEL_DO_DIR): /*OMP*/
	    case (OMP_BARRIER_DIR): /*OMP*/
	    case (OMP_ATOMIC_DIR): /*OMP*/
	    case (OMP_FLUSH_DIR): /*OMP*/
	    case (OMP_THREADPRIVATE_DIR): /*OMP*/
	    case (OMP_ONETHREAD_DIR): /*OMP*/
            case (ACC_REGION_DIR):             /*ACC*/
            case (ACC_END_REGION_DIR):         /*ACC*/
            case (ACC_CHECKSECTION_DIR):       /*ACC*/
            case (ACC_END_CHECKSECTION_DIR):   /*ACC*/
            case (ACC_GET_ACTUAL_DIR):         /*ACC*/ 
            case (ACC_ACTUAL_DIR):             /*ACC*/
            case (SPF_ANALYSIS_DIR):           /*SPF*/
            case (SPF_PARALLEL_DIR):           /*SPF*/
            case (SPF_TRANSFORM_DIR):          /*SPF*/ 
            case (SPF_PARALLEL_REG_DIR):       /*SPF*/
            case (SPF_END_PARALLEL_REG_DIR):   /*SPF*/ 
            case (SPF_CHECKPOINT_DIR):         /*SPF*/ 
	      if (start_new_group) {
		if (stat->variant == CDOALL_NODE
		    || stat->variant == SDOALL_NODE
		    || stat->variant == DOACROSS_NODE
		    || stat->variant == CDOACROSS_NODE)
		  set_blobs(stat, pred_bfnd, NEW_GROUP2);
		else
		  set_blobs(stat, pred_bfnd, NEW_GROUP1);
	      } else
		  set_blobs(stat, pred_bfnd, SAME_GROUP);
		break;
	    case (CONTROL_END):
	    case (PROCESSES_END):
           /* case (ACC_END_REGION_DIR): */   /*ACC*/
	    case (OMP_END_PARALLEL_DIR): /*OMP*/
	    case (OMP_END_SINGLE_DIR): /*OMP*/
	    case (OMP_END_MASTER_DIR): /*OMP*/
	    case (OMP_END_CRITICAL_DIR): /*OMP*/
	    case (OMP_END_ORDERED_DIR): /*OMP*/
	    case (OMP_END_PARALLEL_WORKSHARE_DIR): /*OMP*/
	    case (OMP_END_WORKSHARE_DIR): /*OMP*/
	    case (OMP_END_PARALLEL_SECTIONS_DIR): /*OMP*/
	    case (OMP_END_SECTIONS_DIR): /*OMP*/ {
	        make_endblock(stat);
		break;
	    }
	    default:
		err("Compiler bug (stat.c)", 0);
		ret = old_list;
	}
     
	close_groups();
	return (ret);
}


void
close_groups()
{
     PTR_BFND past_pred;
     PTR_BLOB q;
     
     while (end_group > 0) {
	  past_pred = pred_bfnd;
	  pred_bfnd = pred_bfnd->control_parent;
	  q = follow_blob(pred_bfnd->entry.Template.bl_ptr1);
	  cur_blob = (q && (q->ref == past_pred)) ?
	       q : follow_blob(pred_bfnd->entry.Template.bl_ptr2); 
/*	  cur_blob = follow_blob(pred_bfnd->entry.Template.bl_ptr1); */
	  --end_group;
     }
}


/*
 *  Makes a PROG_HEDR statement for programs which does not start with
 *  a program name.
 */ 
void
make_prog_header()
{/*never used function*/
     PTR_BFND first_bfnd;
     /*PTR_BLOB b;*/
     void set_blobs();
     first_bfnd = BFNULL;
     parstate = INSIDE;
     set_blobs(first_bfnd, global_bfnd, NEW_GROUP1);
}



PTR_SYMB
proc_decl_init(entry, type)
PTR_HASH entry;
int type;
{
	PTR_SYMB symb_ptr;

	undeftype = NO;
	entry = correct_symtab(entry, type);
	mod_offset = yylineno - 1;
	for (symb_ptr = head_symb; symb_ptr; symb_ptr = symb_ptr->thread)
		if ((strcmp(symb_ptr->ident, entry->ident) == 0) &&
		    (symb_ptr->variant == PROCEDURE_NAME))
			break;
	if (!symb_ptr)
		symb_ptr = install_entry(entry, SOFT);
	if (type == FUNCTION_NAME)
	   symb_ptr->type = (vartype == global_default) ?
	                        (undeftype ? global_unknown
			               :impltype[*entry->ident - 'a'])
			          : vartype;

	symb_ptr->variant = type;
	symb_ptr->scope = global_bfnd;
	symb_ptr->entry.proc_decl.seen = YES;
	return (symb_ptr);
}



/*
 * End of declaration section of procedure.  Allocate storage.
 */
void
enddcl()
{ PTR_BFND scope;
 PTR_SYMB fname,res;
	parstate = INEXEC;
        /*16.03.03*/
        scope=cur_scope();
        if(scope->variant == FUNC_HEDR && scope->entry.Template.ll_ptr1){ /*current scope is function header with RESULT clause*/
          fname = scope->entry.Template.symbol;
          res = scope->entry.Template.ll_ptr1->entry.Template.symbol;
          fname->type = res->type; /* characteristics of result identifier are set on function name*/
          fname->attr =fname->attr | res->attr;
        }  
/*
	docommon();
	doequiv();
	docomleng();
*/
}

/*
 * Start a new procedure
 */
void
newprog()
{
	void execerr();

	mod_offset = yylineno - 1;
	/* if (parstate != OUTSIDE)
		execerr("missing end statement", (char *)NULL); */	
        if (parstate == OUTSIDE) {
	  setimpl(global_float, 'a', 'z');
	  setimpl(global_int, 'i', 'n');
        } 
        parstate = INSIDE;

}


/*
 * Main program or Block data
 */
void
startproc(prgname, class)
	PTR_SYMB prgname;
	int     class;
{
}


/*
 *  pop_lab either marks all labels in inner blocks unreachable
 *    or moves all labels referred to in inner blocks out a level
 */ /*
void
pop_lab()
{
	register PTR_LABEL lp;

	for (lp = head_label; lp; lp = lp->next)
		if (lp->labdefined) {
*/		/* mark all labels in inner blocks unreachable */
/*			if (lp->blklevel > blklevel)
				lp->labinacc = YES;
		} else if (lp->blklevel > blklevel) {
*/		/* move all labels referred to in inner blocks out a level */
/*			lp->blklevel = blklevel;
		}
}
*/

/*
 * make_do handles the DO statement.
 *
 * input:
 *	  label - the label of the last statement of the DO loop
 *	  spec  - list of low level nodes specifying the starting,
 *		  value, end value, and increment of the loop
 *
 * output:
 *	  returns a BIF node which contains the necessary information
 *	  of a loop header
 */
PTR_BFND
make_do(type, label, symbol, start, end, stride)
        int      type;
	PTR_LABEL label;
	PTR_SYMB symbol;
        PTR_LLND start, end, stride;
{
       
	PTR_BFND bfnd;
	PTR_LLND ddot;

        if (symbol != SMNULL)
	  symbol->dovar = 1;
        if(type == FOR_NODE)
	  ddot = make_llnd(fi, DDOT, start, end, SMNULL);
        else
          ddot = start;
	bfnd = get_bfnd(fi, type, symbol, ddot, stride, LLNULL);
	bfnd->entry.for_node.doend = label;
	return (bfnd);
}

/* Assuming where_cond is unused in FORTRAN */
PTR_BFND
make_pardo(type, label, symbol, start, end, stride, qualities)
        int      type;
	PTR_LABEL label;
	PTR_SYMB symbol;
        PTR_LLND start, end, stride, qualities;
{
       
	PTR_BFND bfnd;
	PTR_LLND ddot;

	ddot = make_llnd(fi, DDOT, start, end, SMNULL);
	bfnd = get_bfnd(fi, type, symbol, ddot, stride, LLNULL);
	bfnd->entry.for_node.doend = label;
	bfnd->entry.for_node.where_cond = qualities;
	return (bfnd);
}

PTR_BFND
make_processdo(type, label, symbol, start, end, stride)
        int      type;
	PTR_LABEL label;
	PTR_SYMB symbol;
        PTR_LLND start, end, stride;
{
       
	PTR_BFND bfnd;
	PTR_LLND ddot;

        symbol->dovar = 1;
	ddot = make_llnd(fi, DDOT, start, end, SMNULL);
	bfnd = get_bfnd(fi, type, symbol, ddot, stride, LLNULL);
	bfnd->entry.for_node.doend = label;
	return (bfnd);
}

PTR_BFND
make_processes()
{
	PTR_BFND p;
	void make_prog_header();

	if (pred_bfnd->variant == GLOBAL)
	  make_prog_header();

	p = get_bfnd(fi, PROCESSES_STAT, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

void
set_blobs(stat, parent, new_group)
PTR_BFND stat, parent;
int new_group;
{
	if (((new_group == NEW_GROUP1) && stat->entry.Template.bl_ptr1) ||
	    ((new_group == NEW_GROUP2) && stat->entry.Template.bl_ptr2))
	  return;
	stat->control_parent = parent;
	if (!cur_blob) {
	  /*(void)fprintf(stderr, "cur_blob is null !\n");*/ /*podd*/
           err("Compiler bug", 0);
	   return;
	}
	if (cur_blob->ref) {
		cur_blob->next = make_blob(fi,stat, BLNULL);
		cur_blob = cur_blob->next;
	} else 
	  cur_blob->ref = stat;
	if (new_group) {
		pred_bfnd = stat;
		cur_blob = make_blob(fi,BFNULL, BLNULL);
		if (new_group == NEW_GROUP1)
		   stat->entry.Template.bl_ptr1 = cur_blob;
		else
		   stat->entry.Template.bl_ptr2 = cur_blob;
	}
}

PTR_LABEL make_label_node(fi, l)
   	PTR_FILE fi;
	long	 l;
   {    PTR_LABEL new_lab;
        PTR_BFND  this_scope;
        this_scope = cur_scope();
	for (new_lab = fi->head_lab; new_lab; new_lab = new_lab->next)
		if (new_lab->stateno == l && new_lab->scope == this_scope)
			return (new_lab);
        new_lab =make_label(fi, l);
        if(new_lab->stateno == 0)
           err("Label out of range",38);
        new_lab -> scope = this_scope;
        return (new_lab);
   }

int is_interface_stat(st)
     PTR_BFND st;
{if(st->variant == INTERFACE_STMT || st->variant == INTERFACE_ASSIGNMENT || st->variant == INTERFACE_OPERATOR)
   return(1);
 else
   return(0);
}

PTR_BFND
make_endparallel()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_PARALLEL_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	/*PTR_BLOB q;*/
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_PARALLEL_DIR))
	   execerr("OMP END PARALLEL DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_parallel()
{
	PTR_BFND p = get_bfnd(fi, OMP_PARALLEL_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}


PTR_BFND
make_endsingle()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_SINGLE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	/*PTR_BLOB q;*/

	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_SINGLE_DIR))
	   execerr("OMP END SINGLE DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_single()
{
	PTR_BFND p;
	p = get_bfnd(fi, OMP_SINGLE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endmaster()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_MASTER_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_MASTER_DIR))
	   execerr("OMP END MASTER DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_master()
{
	PTR_BFND p = get_bfnd(fi, OMP_MASTER_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endordered()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_ORDERED_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	/*PTR_BLOB q;*/

	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_ORDERED_DIR))
	   execerr("OMP END ORDERED DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_ordered()
{
	PTR_BFND p;
	p = get_bfnd(fi, OMP_ORDERED_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endcritical()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_CRITICAL_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_CRITICAL_DIR))
	   execerr("OMP END CRITICAL DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_critical()
{
	PTR_BFND p = get_bfnd(fi, OMP_CRITICAL_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endsections()
{
        PTR_BFND p;
	/*PTR_BLOB q;*/
	/* mark end of section */
	p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	set_stat_list(pred_bfnd, p);
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_SECTIONS_DIR)) {
		fprintf (stderr,"%d",pred_bfnd->variant);
	   execerr("OMP END SECTIONS DIR out of place", (char *)NULL);
	}
        p = get_bfnd(fi,OMP_END_SECTIONS_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return p;
}

PTR_BFND
make_sections(PTR_LLND clause)
{
	PTR_BFND p;
	p = get_bfnd(fi, OMP_SECTIONS_DIR, SMNULL, clause, LLNULL, LLNULL);
	set_blobs(p, pred_bfnd, NEW_GROUP1);
	p = get_bfnd(fi, OMP_SECTION_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_ompsection()
{
    PTR_BFND p;
    /*PTR_BLOB q;*/
    if (pred_bfnd) {
	if (pred_bfnd->variant == OMP_SECTION_DIR) {
		if (last_bfnd->variant == OMP_SECTION_DIR) {
			return BFNULL;	
		} else {
			p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
			set_stat_list(pred_bfnd, p);
		}
	}
    }
    p = get_bfnd(fi, OMP_SECTION_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
    return p;
}

PTR_BFND
make_endparallelsections()
{
        PTR_BFND p;
	/*PTR_BLOB q;*/
	/* mark end of section */
	p = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	set_stat_list(pred_bfnd, p);
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_PARALLEL_SECTIONS_DIR)) {
		fprintf (stderr,"%d",pred_bfnd->variant);
		execerr("OMP END PARALLEL SECTIONS DIR out of place", (char *)NULL);
	}
        p = get_bfnd(fi,OMP_END_PARALLEL_SECTIONS_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return p;
}

PTR_BFND
make_parallelsections(PTR_LLND clause)
{
	PTR_BFND p;
	p = get_bfnd(fi, OMP_PARALLEL_SECTIONS_DIR, SMNULL, clause, LLNULL, LLNULL);
	set_blobs(p, pred_bfnd, NEW_GROUP1);
	p = get_bfnd(fi, OMP_SECTION_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endworkshare()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_WORKSHARE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	/*PTR_BLOB q;*/
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_WORKSHARE_DIR))
	   execerr("OMP END WORKSHARE DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_workshare()
{
	PTR_BFND p = get_bfnd(fi, OMP_WORKSHARE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}

PTR_BFND
make_endparallelworkshare()
{
        PTR_BFND p = get_bfnd(fi,OMP_END_PARALLEL_WORKSHARE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	if ((!pred_bfnd) || (pred_bfnd->variant != OMP_PARALLEL_WORKSHARE_DIR))
	   execerr("OMP END PARALLEL WORKSHARE DIR out of place", (char *)NULL);
	return p;
}

PTR_BFND
make_parallelworkshare()
{
	PTR_BFND p;
	p = get_bfnd(fi, OMP_PARALLEL_WORKSHARE_DIR, SMNULL, LLNULL, LLNULL, LLNULL);
	return (p);
}
