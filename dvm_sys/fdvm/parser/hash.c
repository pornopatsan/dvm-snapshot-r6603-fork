/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * hash.c -- hash table routines
 */

#include <stdio.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#include "defs.h"
#include "symb.h"
#include "defines.h"
#include "bif.h"
#include "extern.h"

extern int parstate;
extern PTR_BFND cur_bfnd, pred_bfnd, global_bfnd;
extern PTR_TYPE vartype, global_default, impltype[];
extern void make_prog_header();
PTR_TYPE install_array();
PTR_LLND make_llnd();
PTR_SYMB make_symb();
char *chkalloc();
void free();
void errstr();

PTR_HASH hash_table[hashMax];


/*
 * init_hash -- initialize the hash table
 */
void
init_hash()
{
	register int i;

	for (i = 0; i < hashMax; i++)
		hash_table[i] = HSNULL;
}


/*
 * Hash(string) -- compute hash value of string.
 */
int 
hash(string)
	register char *string;
{
	register int i;

	for (i = 0; *string;)
		i += *string++;
	return (i % hashMax);
}


/*
 * look_up(string) -- lookup string in the hash table and
 *		      install it if not there
 */
PTR_HASH 
look_up(string, decl_type)
register char *string;
int decl_type;
{
	int i;
	register PTR_HASH entry;
	PTR_BFND cur_scope(), p;

	i = hash(string);
	p = cur_scope();
	for (entry = hash_table[i]; entry; entry = entry->next_entry) {
	   if (!strcmp(string, entry->ident) && (entry->id_attr)) {
	      if ((entry->id_attr->scope == p) ||
		  ((entry->id_attr->variant==FUNCTION_NAME) &&
		   (p->variant==FUNC_HEDR) &&
		   (p->entry.Template.symbol==entry->id_attr)))
		 return (entry);
	      if (decl_type == SOFT) {
		 for (p=cur_scope(); NEW_SCOPE(p); p = p->control_parent) {
		    if (entry->id_attr->scope == p)
		       return (entry);
		 }
 		 if (entry->id_attr->scope == p)
		    return (entry);
	      }
	   }
	}
	entry = (struct hash_entry *) chkalloc(sizeof(struct hash_entry));
	entry->ident = copys(string);
	entry->next_entry = hash_table[i];
	hash_table[i] = entry;
	return (entry);
}


PTR_HASH
correct_symtab(h, type)
PTR_HASH h;
int type;
{
   int i;
   PTR_HASH entry;

   i = hash(h->ident);
   for (entry = hash_table[i]; entry; entry = entry->next_entry) {
	   if (!strcmp(h->ident, entry->ident)
	       && ( !(entry->id_attr)
		   ||(entry->id_attr->variant==type))
	       && (h != entry))
	      break;
	}
   if (!entry) return h;
      if (hash_table[i] != h) {
	 fprintf (stderr, "Bug in correct_symtab\n");
	 return h;
      }
   hash_table[i] = hash_table[i]->next_entry;
   free((char *)h);
   return hash_table[i];
}


/*
 * Checks whether the "name" is installed and installs as a SOFT
 * entry if not.
 */
PTR_LLND
check_and_install(h, d, ndim)
PTR_HASH h;
PTR_LLND d;
int ndim;
{
   PTR_BFND cur_scope();
   PTR_SYMB install_entry();
   PTR_TYPE p = NULL;
   PTR_SYMB s;
   PTR_LLND r;
   void set_type(), err();

   /* Check if the variable is already declared */
   if ((s = h->id_attr) && (s->scope == cur_scope()) && s->type) {
      if (d && s->type->variant != T_ARRAY) {
	 p = install_array(d, s->type, ndim);
	 s->type = p;
      }
   }
   else {
      if (h->id_attr && h->id_attr->type)
	 p = h->id_attr->type;
      else if (!undeftype)
	 p = impltype[*h->ident - 'a'];
      else
	 err("Variable type unknown",327);
      if (d)
	 p = install_array(d, p, ndim);
      s = install_entry(h, SOFT);
      set_type(s, p, LOCAL);
   }
   if (d) {
      r = p->entry.ar_decl.ranges
	 = make_llnd(fi,ARRAY_REF, d, LLNULL, s);
   }
   else
      r = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
   return  make_llnd(fi,EXPR_LIST, r, LLNULL, SMNULL);
}



/*
 * install_entry takes a pointer to a hash entry and
 *  makes another symbol table entry for the same id
 */
PTR_SYMB 
install_entry(entry, decl_type)
PTR_HASH entry;
int decl_type;
{
	register PTR_SYMB symb_ptr;
	PTR_BFND cur_scope();
	PTR_BFND p;
	void err();

	if (decl_type == HARD && entry->id_attr &&
	    entry->id_attr->scope != cur_scope())
	   entry = look_up(entry->ident, HARD);
	if ((entry->id_attr) && ((entry->id_attr->scope == cur_scope()) ||
		  ((entry->id_attr->variant==FUNCTION_NAME) &&
		   (entry->id_attr->scope->variant==FUNC_HEDR))))
	{
		if (entry->id_attr->decl == SOFT) {
			entry->id_attr->decl = decl_type;
			return(entry->id_attr);
		}
		if (decl_type == SOFT)
		   return(entry->id_attr);
		/* else */
		errstr("Redeclaration of identifier: %s",entry->id_attr->ident,328);
		/*(void)fprintf(stderr, "id: %s\n", entry->id_attr->ident);*/
		return (SMNULL);
	}
	symb_ptr = make_symb(fi,DEFAULT, entry->ident);
	for (p=cur_scope();
	     NEW_SCOPE(p) && (decl_type==SOFT);
	     p = p->control_parent)
	   ;
	symb_ptr->scope = p;
	symb_ptr->outer = entry->id_attr;
	symb_ptr->parent = entry;
	symb_ptr->decl = decl_type;
	entry->id_attr = symb_ptr;
	symb_ptr->id_list = SMNULL;
	return (symb_ptr);
}



PTR_SYMB 
get_proc_symbol(entry)
	PTR_HASH entry;
{
	register PTR_SYMB symb_ptr;
	PTR_BFND cur_scope();

	symb_ptr = make_symb(fi, PROCEDURE_NAME, entry->ident);
	symb_ptr->scope = global_bfnd;
	symb_ptr->outer = entry->id_attr;
	symb_ptr->parent = entry;
	entry->id_attr = symb_ptr;
	return (symb_ptr);
}

/*
PTR_BFND
cur_scope()
{
	register PTR_BFND p;
*/
        /* Takes cares of main program unit begining without a PROGRAM 
           statement. After rewrite of statement processing has been done,
	   strengthen ( weaken? ) the test.
	*/
/*
	if ((pred_bfnd->variant == GLOBAL) && (parstate == OUTSIDE))
	{
	     make_prog_header();
	     return (pred_bfnd);
	}

	for (p = pred_bfnd;
	     (p->variant != PROG_HEDR) &&
	     (p->variant != PROC_HEDR) &&
	     (p->variant != PROS_HEDR) &&
	     (p->variant != FUNC_HEDR) &&
	     (p->variant != BLOCK_DATA) &&
	     (p->variant != FORALL_NODE) &&
	     (p->variant != GLOBAL) &&
	     (p->variant != CDOALL_NODE) &&
	     (p->variant != SDOALL_NODE) &&
	     (p->variant != DOACROSS_NODE) &&
	     (p->variant != STRUCT_DECL);
	     p = p->control_parent);
	  ;
	     
	return (p);
}
*/



