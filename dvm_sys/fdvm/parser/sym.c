/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/*
 * sym.c -- hash table routines
 */

#include <stdio.h>
#include <stdlib.h>
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
#include "fdvm.h" /*f90.h  10.03.03*/
#include "tokdefs.h"

extern int hash(), ndim;
extern PTR_BFND cur_bfnd, pred_bfnd, global_bfnd;
extern PTR_TYPE vartype, global_default, impltype[], make_type();
extern PTR_LLND make_llnd();
extern PTR_SYMB make_symb();
/* added for FORTRAN 90 */
extern PTR_LLND first_unresolved_call;
extern PTR_LLND last_unresolved_call;
extern void err(), errstr();
char *chkalloc();
void warn1();
extern PTR_HASH hash_table[];
extern int warn_all;
extern int privateall;
/* Scope table variables */
PTR_BFND scope_table[1000];
PTR_TYPE * scope_implicit[1000];
int top_scope_level;
int scope_starts[10];
int top_scope_starts;


struct operand
{
     int opval;
     char *opname;
} oplist [] = {
        {PLUS, "+"},
	{MINUS, "-"},
	{ASTER, "*"},
	{DASTER, "**"},
	{SLASH, "/"},
	{DSLASH, "//"},
	{AND, ".and."},
	{OR, ".or."},
	{XOR, ".xor."},
	{NOT, ".not."},
	{EQ, ".eq."},
	{NE, ".ne."},
	{GT, ".gt."},
	{GE, ".ge."},
	{LT, ".lt."},
	{LE, ".le."},        
	{NEQV, ".neqv."},
	{EQV, ".eqv."},
	{0, 0}
};

void store_implicit()
{PTR_TYPE *impl,*it,*ip;
 int i;
 /*fprintf(stderr,"%s\n",scope_table[top_scope_level]->entry.Template.symbol->ident);*/
 impl = (PTR_TYPE * ) calloc(26, sizeof(PTR_TYPE));
 it=impltype; ip=impl;
 i=26;
 while(--i >= 0)
   *ip++ = *it++;
 /*return(impl)*/;
 scope_implicit[top_scope_level] = impl;
}
     
void restore_implicit()
{PTR_TYPE *impl;
 int i;
 /*fprintf(stderr,"restore%s\n",scope_table[top_scope_level]->entry.Template.symbol->ident);*/
 impl = scope_implicit[top_scope_level];
 for(i=0; i<26; i++)
   impltype[i] = *impl++;
 /* it=impltype;
 i=26;
 while(--i >= 0)
 *it++ = *impl++;*/
}

void
init_scope_table()
{
     scope_table[0] = global_bfnd;
     top_scope_level = 0;
     scope_starts[0]= 0;
     top_scope_starts = 0;
     scope_implicit[0] = NULL;
}

PTR_BFND
cur_scope()
{
     return (scope_table[top_scope_level]);
}

PTR_BFND
parent_scope(present_scope_level)
int present_scope_level;
{
     if (present_scope_level >= 0)
	  return (scope_table[--present_scope_level]);
     else err("Requested scope level non-existent", 314);
     return 0;
}

void
add_scope_level(new_scope, due_to_use_stat)
PTR_BFND new_scope;
int due_to_use_stat;
{
     PTR_BFND tmp;
     
     if (due_to_use_stat)
     {
	  tmp = scope_table[top_scope_level];
	  scope_table[top_scope_level] = new_scope;
	  scope_table[++top_scope_level] = tmp;
     }
     else 
     {
          if(top_scope_level>0 && !scope_implicit[top_scope_level])
            store_implicit();
	  scope_table[++top_scope_level] = new_scope;
	  scope_starts[++top_scope_starts] = top_scope_level; 
     }
}

void
delete_beyond_scope_level(level)
PTR_BFND level;
{    scope_implicit[top_scope_level] = NULL; 
     top_scope_level = scope_starts[top_scope_starts] -  1;
     top_scope_starts --;
     if (top_scope_level < 0) 
	  err("Requested scope level non-existent", 314);
     if(top_scope_level>0)
       restore_implicit();
}	  

int
cur_scope_level()
{
     return (top_scope_level);
}

PTR_BFND
scope_at_level(level)
int level;
{
     if (level >= 0)
	  return (scope_table[level]);
     else  errstr("Requested scope non-existent", 315);
     return 0;
}


/*
 look_up_sym(string) : 
   lookup string in the hash table. If a hash table entry having string 
   as it's name, in curent scope is found then return the hash table entry, 
   else make a hash  table entry with string as it's name and return it.
 */
PTR_HASH 
look_up_sym(string)
register char *string;
{
	int i, index, cur_scope_level();
	register PTR_HASH entry;
	PTR_HASH  make_hash_entry();
	PTR_BFND parent_scope(), scope_at_level(), p;

	i = hash(string);
        for (index = cur_scope_level(); index >= 0; index--) 
	{
	     p = scope_at_level(index);
	     for (entry = hash_table[i]; entry; entry = entry->next_entry) {
		  if (!strcmp(string, entry->ident) && (entry->id_attr) && 
                      (entry->id_attr->scope == p)) {
		       return (entry);
		  }
	     }
	}

	return (make_hash_entry(string));
}

PTR_HASH 
just_look_up_sym_in_scope(scope, string)
PTR_BFND scope;
register char *string;
{
	int i, cur_scope_level();
	register PTR_HASH entry;
	PTR_HASH  make_hash_entry();
	PTR_BFND parent_scope(), scope_at_level(), p;

	i = hash(string);
	p = scope;
	for (entry = hash_table[i]; entry; entry = entry->next_entry) {
	     if (!strcmp(string, entry->ident) && (entry->id_attr) && 
		 (entry->id_attr->scope == p)) {
		  return (entry);
	     }
	}

	return (HSNULL);
}


PTR_HASH
look_up_op(operator)
int operator;
{
     struct operand *p;
     
     for (p = oplist; p->opname; p++)
	  if (p->opval == operator)
	       return (look_up_sym(p->opname));
     errstr("Unknown operator %d", operator, 316);
     return (HSNULL);
}

PTR_HASH 
just_look_up_sym(string)
register char *string;
{
	int i, index, cur_scope_level();
	register PTR_HASH entry;
	PTR_HASH  make_hash_entry();
	PTR_BFND parent_scope(), scope_at_level(), p;

	i = hash(string);
	p = cur_scope();
        for (index = cur_scope_level(); index >= 0; index--) 
	{
	     p = scope_at_level(index);
	     for (entry = hash_table[i]; entry; entry = entry->next_entry) {
		  if (!strcmp(string, entry->ident) && (entry->id_attr) && 
                      (entry->id_attr->scope == p)) {
		       return (entry);
		  }
	     }
	}

	return (HSNULL);
}

PTR_HASH 
make_hash_entry(string)
register char *string;
{
     int i;
     register PTR_HASH entry;

     i = hash(string);

     entry = (struct hash_entry *) chkalloc(sizeof(struct hash_entry));
     entry->ident = copys(string);
     entry->next_entry = hash_table[i];
     hash_table[i] = entry;
     return (entry);
}    
 
PTR_SYMB
make_sym_entry(var_hash_entry, variant, type, scope, kind)
PTR_HASH var_hash_entry;
int variant, kind;
PTR_TYPE type;
PTR_BFND scope;
{
     PTR_SYMB var_sym_entry;
     PTR_SYMB list;     
     /* If type is undefined, then obtain type from implicit type table. */
     if ((variant != PROGRAM_NAME) && (variant != DEFAULT) &&
	 (variant != PROCEDURE_NAME) && (variant != PROCESS_NAME) && (variant != INTERFACE_NAME)) 
     {
	  if (type == TYNULL)
            {
              type =  impltype[*var_hash_entry->ident - 'a'];
            }
	  else if ((type->variant == T_ARRAY) &&
		   (type->entry.ar_decl.base_type == TYNULL)) {
                 if ((*var_hash_entry->ident-'a') < 0)
                   type->entry.ar_decl.base_type = 0;
                 else
	           type->entry.ar_decl.base_type = 
	    	               impltype[*var_hash_entry->ident - 'a'];
               } 
	
	  if ((type == TYNULL) ||
              ((type->variant == T_ARRAY) &&
               (type->entry.ar_decl.base_type == TYNULL) &&
               (strcmp(var_hash_entry->ident, "_PROCESSORS") != 0)))
	  {
               /*
	       errstr("type unknown of %s",
                                                    var_hash_entry->ident);
               */
	       type = global_default;
	  }
     }
     

     var_sym_entry = make_symb(fi, variant, var_hash_entry->ident);

     /* Point the hash entry to the symbol table entry */
     var_hash_entry->id_attr = var_sym_entry;

     var_sym_entry->variant = variant;
     var_sym_entry->type = type;
     var_sym_entry->scope = scope;
     var_sym_entry->id_list = SMNULL;
     switch (variant) {
     case VARIABLE_NAME: 
	  /* if not a formal parameter, 
             then mark it as local. */
	  if (var_sym_entry->entry.var_decl.local != IO) 
	       var_sym_entry->entry.var_decl.local = kind;
	  break;
     case FUNCTION_NAME:
     case PROCEDURE_NAME:
     case PROCESS_NAME:
	  var_sym_entry->entry.proc_decl.seen = kind;
	  break;
     default:
	  break;
     }

     /* initialize administrative stuff */
     var_sym_entry->outer = SMNULL;
     var_sym_entry->parent = var_hash_entry;
/*     var_sym_entry->decl = HARD; */
     if(scope == cur_scope() && scope->variant == MODULE_STMT ){
       if(privateall)
	 var_sym_entry->attr =  var_sym_entry->attr | PRIVATE_BIT;
       list = scope->entry.Template.symbol->entry.Template.next; /* adding to list of all the identifiers of module */
       scope->entry.Template.symbol->entry.Template.next = var_sym_entry;
       var_sym_entry->entry.Template.next = list;
     }
     return(var_sym_entry);
}

PTR_SYMB
make_constant(var_hash_entry, type)
PTR_HASH var_hash_entry;
PTR_TYPE type;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
   

     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
	  return (make_sym_entry(var_hash_entry, CONST_NAME, type, cur_scope(), 
                                 0));

     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd)
	  return (make_sym_entry(make_hash_entry(var_hash_entry->ident),
                                 CONST_NAME, type, cur_scope(), 0));
     	       
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       var_sym_entry->variant = CONST_NAME;
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_ARRAY:
                    if (type != TYNULL)
		       var_sym_entry->type->entry.ar_decl.base_type = type;
                    return (var_sym_entry);
	       case (T_POINTER):
                    if (type != TYNULL)
		       var_sym_entry->type->entry.Template.base_type = type;
                    return (var_sym_entry);
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_STRUCT:
	       case T_DERIVED_TYPE:
		    if (type != TYNULL) 
			 var_sym_entry->type = type;
		    return (var_sym_entry);
	       }
	       break;
	  case CONST_NAME:
	       if (type != TYNULL)
		    var_sym_entry->type = type;
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent constant declaration %s", var_hash_entry->ident, 17);
	       return (var_sym_entry);/*return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), CONST_NAME,
                            type, cur_scope(), 0));
}     

/* Occurence of Function name in it's body not properly taken care of. */
PTR_SYMB
make_scalar(var_hash_entry, type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE type;
int kind;
{
     PTR_SYMB var_sym_entry, cur_scope_sym_ptr;
     int var_variant;

     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
	  return (make_sym_entry(var_hash_entry, VARIABLE_NAME, type, 
                                 cur_scope(), kind));

     var_variant = var_sym_entry->variant;
     cur_scope_sym_ptr = cur_scope()->entry.Template.symbol;
     if (var_sym_entry->scope != cur_scope()){ /*(var_sym_entry->scope == global_bfnd)*/
	  if ((cur_scope()->variant == FUNC_HEDR) &&
	      (!(strcmp(var_hash_entry->ident, cur_scope_sym_ptr->ident))))
	       if (type == TYNULL)
		    return (var_sym_entry);
               else 
	       {
		    cur_scope_sym_ptr->type = type;
		    /* result_sym_ptr =  cur_scope_sym_ptr->entry.Template.declared_name;
		    if (result_sym_ptr)
			 result_sym_ptr->type = type;
		    */ /*19.03.03*/
		    return (var_sym_entry);
	       }
     	  else return (make_sym_entry(make_hash_entry(var_hash_entry->ident),
                                 VARIABLE_NAME, type, cur_scope(), kind));
     }
     
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_ARRAY:
                    if (type != TYNULL)
		       var_sym_entry->type->entry.ar_decl.base_type = type;
                    return (var_sym_entry);
	       case T_POINTER:
                    if (type != TYNULL)
		       var_sym_entry->type->entry.Template.base_type = type;
                    return (var_sym_entry);
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_STRUCT:
	       case T_DERIVED_TYPE:
		    if (type != TYNULL)
			 var_sym_entry->type = type;
		    return (var_sym_entry);
	       }
	       break;
	  case CONST_NAME:
	       if (type != TYNULL)
		    var_sym_entry->type = type;
	       return (var_sym_entry);
	  case ROUTINE_NAME:
          case FUNCTION_NAME:
	       if (type != TYNULL)
		    var_sym_entry->type = type;
	       return (var_sym_entry);
	  case LABEL_VAR:
	       return (var_sym_entry);
          case TYPE_NAME:
          case PROCEDURE_NAME:
          case INTERFACE_NAME:
          case NAMELIST_NAME:
	       return (var_sym_entry);            
	  default:
	       errstr("Inconsistent declaration of identifier %s",var_hash_entry->ident,16);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), 
                            VARIABLE_NAME, type, cur_scope(), kind));
}     

/*
     make_array(var_hash_entry:PTR_HASH, base_type: PTR_TYPE, dcl_type:int,
                ranges: PTR_LLND, ndim : int)
       makes a array symbol table entry.
*/

PTR_SYMB
make_array(var_hash_entry, base_type, ranges, ndim, kind)
PTR_HASH var_hash_entry;
PTR_TYPE base_type;
PTR_LLND ranges;
int ndim, kind;
{
     PTR_SYMB var_sym_entry, cur_scope_sym_ptr;
     PTR_TYPE array_type;
     int var_variant;
     
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
     {
	  array_type = make_type(fi, T_ARRAY);
	  array_type->entry.ar_decl.base_type = base_type;
	  array_type->entry.ar_decl.num_dimensions = ndim;
	  return (make_sym_entry(var_hash_entry, VARIABLE_NAME, array_type,
                                 cur_scope(), kind));
     }
     
     var_variant = var_sym_entry->variant;
     cur_scope_sym_ptr = cur_scope()->entry.Template.symbol;
     if (var_sym_entry->scope != cur_scope()){ /*(var_sym_entry->scope == global_bfnd)*/
	  if ((cur_scope()->variant == FUNC_HEDR) &&
	      (!(strcmp(var_hash_entry->ident, cur_scope_sym_ptr->ident))))
	       if (base_type == TYNULL)
		    return (var_sym_entry);
               else 
	       {
                    array_type = make_type(fi, T_ARRAY);
	            array_type->entry.ar_decl.base_type = base_type;
	            array_type->entry.ar_decl.num_dimensions = ndim;
		    cur_scope_sym_ptr->type = array_type;
                    /*result_sym_ptr =  cur_scope_sym_ptr->entry.Template.declared_name;
		    if (result_sym_ptr)
			 result_sym_ptr->type = array_type;
		    *//*19.03.03*/
		    return (var_sym_entry);
	       }
     	  else /*return (make_sym_entry(make_hash_entry(var_hash_entry->ident),
		 VARIABLE_NAME, type, cur_scope(), kind));*/
     	       

	    /*  if (var_sym_entry->scope == global_bfnd)*/
         {
	  array_type = make_type(fi, T_ARRAY);
	  array_type->entry.ar_decl.base_type = base_type;
	  array_type->entry.ar_decl.num_dimensions = ndim;
	  return (make_sym_entry(make_hash_entry(var_hash_entry->ident), VARIABLE_NAME, array_type, 
                                 cur_scope(), kind));/*7.03.03*/
         }
    } 
  
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_ARRAY:
                    if (base_type != TYNULL)
                         var_sym_entry->type->entry.ar_decl.base_type = 
			      base_type;
		    if (ndim) 
			 var_sym_entry->type->entry.ar_decl.num_dimensions = ndim;
                    return (var_sym_entry);
	       case T_STRING:
	       case T_POINTER:
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_DCOMPLEX:
	       case T_BOOL:
	       case T_STRUCT:
	       case T_DERIVED_TYPE:
	       {
		    array_type = make_type(fi, T_ARRAY);
		    if (base_type == TYNULL)
			 array_type->entry.ar_decl.base_type = 
			      var_sym_entry->type;
		    else array_type->entry.ar_decl.base_type = base_type;
		    array_type->entry.ar_decl.num_dimensions = ndim;
		    var_sym_entry->type = array_type;
                    return (var_sym_entry);
	       }
               default:
		    return (var_sym_entry);
	       }
	      
	  default:
	       errstr("Inconsistent array declaration of identifier %s", var_hash_entry->ident,18);
	       return (var_sym_entry);/*return (SMNULL);*/
	  }

     array_type = make_type(fi, T_ARRAY);
     array_type->entry.ar_decl.base_type = base_type;
     array_type->entry.ar_decl.num_dimensions = ndim;
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), VARIABLE_NAME, array_type,
			    cur_scope(), kind)); /*7.03.03*/
}

PTR_SYMB
make_pointer(var_hash_entry, base_type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE base_type;
int kind;
{
     PTR_SYMB var_sym_entry;
     PTR_TYPE pointer_type;
     int var_variant;
     
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
     {
	  pointer_type = make_type(fi, T_POINTER);
	  pointer_type->entry.Template.base_type = base_type;
	  return (make_sym_entry(var_hash_entry, VARIABLE_NAME, pointer_type,
                                 cur_scope(), kind));
     }
     
     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd)
     {
	  pointer_type = make_type(fi, T_POINTER);
	  pointer_type->entry.Template.base_type = base_type;
	  return (make_sym_entry(var_hash_entry, VARIABLE_NAME, pointer_type, 
                                 cur_scope(), kind));
     }

     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_POINTER:
                    if (base_type != TYNULL)
                         var_sym_entry->type->entry.Template.base_type = 
			      base_type;
                    return (var_sym_entry);
	       case T_STRING:
	       case T_ARRAY:
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_STRUCT:
	       case T_DERIVED_TYPE:
	       {
		    pointer_type = make_type(fi, T_POINTER);
		    if (base_type == TYNULL)
			 pointer_type->entry.Template.base_type = 
			      var_sym_entry->type;
		    else pointer_type->entry.Template.base_type = base_type;
		    var_sym_entry->type = pointer_type;
                    return (var_sym_entry);
	       }
               default:
		    return (var_sym_entry);
	       }
	       
	  default:
	       errstr("Inconsistent declaration of identifier %s",var_hash_entry->ident,16);
               return (var_sym_entry);		 /*   return (SMNULL);*/
	  }

     pointer_type = make_type(fi, T_POINTER);
     pointer_type->entry.Template.base_type = base_type;
     return (make_sym_entry(var_hash_entry, VARIABLE_NAME, pointer_type,
                             cur_scope(), kind));
}
     
PTR_SYMB
make_function(var_hash_entry, type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE type;
int kind;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL){
	  if (kind == LOCAL)
	       return (make_sym_entry(var_hash_entry, FUNCTION_NAME, type,
                                      cur_scope(), kind));
          else return (make_sym_entry(var_hash_entry, FUNCTION_NAME, type,
                                      global_bfnd, kind));
     }
     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd) {
          if (var_variant == FUNCTION_NAME) 
	  {
	       if (kind == LOCAL)
		    return (make_sym_entry(var_hash_entry, FUNCTION_NAME, type,
                                      cur_scope(), kind));
	       else return var_sym_entry;
          }
          else if (var_variant == ROUTINE_NAME)
	  {
	       var_sym_entry->variant = FUNCTION_NAME;/* FB Modified == before*/
	       return var_sym_entry;
	  }
          else if (var_variant == INTERFACE_NAME)
	       return var_sym_entry;
          else if (var_variant == DEFAULT){
              /* intrinsic function  can have same name as a common block name */
               if(warn_all)
                 warn1("Function has the same name as a common block  %s.",var_hash_entry->ident, 24);      
	       var_sym_entry->variant = FUNCTION_NAME;
               return var_sym_entry;
          }
          else if (var_variant == VARIABLE_NAME)
	  {
	       var_sym_entry->variant = FUNCTION_NAME; /* FB Modified == before*/
	       return var_sym_entry;
	  }
	  else
	  {
	       errstr("Inconsistent function declaration %s", var_hash_entry->ident, 19);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }     	       
     }
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_POINTER:
	       case T_STRUCT:
	       case T_DERIVED_TYPE:
		    /* if not a formal parameter, convert it into 
		       a global function. */
		    if (var_sym_entry->entry.var_decl.local != IO)
		    {
			 var_sym_entry->variant = FUNCTION_NAME;
                         if (kind != LOCAL)
			   var_sym_entry->scope = global_bfnd;
		    }
		    if (type != TYNULL)
			 var_sym_entry->type = type;
		    return (var_sym_entry);
	       case T_ARRAY:
                    if (kind == LOCAL)
		    {
		     errstr("Inconsistent function declaration %s",var_hash_entry->ident,19);
		     return (var_sym_entry);/* return (SMNULL);*/
		    } 
                    else return (var_sym_entry);
	       }
	  case FUNCTION_NAME:
	       var_sym_entry->variant = FUNCTION_NAME;
	       return (var_sym_entry);
	  case ROUTINE_NAME:
	       var_sym_entry->variant = FUNCTION_NAME;
	       return (var_sym_entry);
	  case INTERFACE_NAME:
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent function declaration %s", var_hash_entry->ident, 19);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), FUNCTION_NAME, type, cur_scope(), kind));
}     

PTR_SYMB
make_external(var_hash_entry, type)
PTR_HASH var_hash_entry;
PTR_TYPE type;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
       return (make_sym_entry(var_hash_entry, ROUTINE_NAME, type,
				 global_bfnd, NO));       

     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd) { 
	  if ((var_variant == FUNCTION_NAME) ||
              (var_variant == PROCEDURE_NAME) ||
              (var_variant == PROCESS_NAME))
	       return var_sym_entry;
          else if (var_variant == ROUTINE_NAME)
	  {
	       return var_sym_entry;
	  }
	  else
	  {
	       errstr("Inconsistent procedure declaration %s", var_hash_entry->ident, 20);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }     	       
     }
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_POINTER:
	       case T_STRUCT:
	       case T_ARRAY:
	       case T_DERIVED_TYPE:
		    /* if not a formal parameter, convert it into 
		       a global function. */
		    if (var_sym_entry->entry.var_decl.local != IO)
		    {
			 var_sym_entry->variant = ROUTINE_NAME;
			 var_sym_entry->scope = global_bfnd;
		    }
		    if (type != TYNULL)
			 var_sym_entry->type = type;
		    return (var_sym_entry);
	       }
	  case ROUTINE_NAME:
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent procedure declaration %s", var_hash_entry->ident, 20);
	       return (var_sym_entry); /*return (SMNULL);*/
	  }     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), ROUTINE_NAME, type, global_bfnd, NO));
}     

PTR_SYMB
make_intrinsic(var_hash_entry, type)
PTR_HASH var_hash_entry;
PTR_TYPE type;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
       return (make_sym_entry(var_hash_entry, ROUTINE_NAME, type,
				 cur_scope(), LOCAL));       

     var_variant = var_sym_entry->variant;
     	       
     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_POINTER:
	       case T_STRUCT:
	       case T_ARRAY:
	       case T_DERIVED_TYPE:
		    /* if not a formal parameter, convert it into 
		       a global function. */
		 if (var_sym_entry->entry.var_decl.local != IO)/*7.03.03*/
		         var_sym_entry->variant = ROUTINE_NAME;		      
		    
		    if (type != TYNULL)
			 var_sym_entry->type = type;
		    return (var_sym_entry);
	       }
          case FUNCTION_NAME:
          case PROCEDURE_NAME:
	  case ROUTINE_NAME:
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent procedure declaration %s", var_hash_entry->ident, 20);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), ROUTINE_NAME, type, cur_scope(), LOCAL));
}     

     
PTR_SYMB
make_procedure(var_hash_entry, kind)
PTR_HASH var_hash_entry;
int kind;
{
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL) {
	  if (kind == LOCAL)
	       return (make_sym_entry(var_hash_entry, PROCEDURE_NAME, TYNULL,
                                      cur_scope(), kind));
          else return (make_sym_entry(var_hash_entry, PROCEDURE_NAME, TYNULL,
                                      global_bfnd, kind));
     }
     if (var_sym_entry->scope == global_bfnd) {
          if (var_sym_entry->variant == PROCEDURE_NAME){
	       if (kind == LOCAL) /*10.03.03*/
		    return (make_sym_entry(make_hash_entry(var_hash_entry->ident), PROCEDURE_NAME, TYNULL,
                                      cur_scope(), kind));
	       else return var_sym_entry;
                 /* if (var_sym_entry->variant == PROCEDURE_NAME)
	        return (var_sym_entry);*/
	  }
          else if (var_sym_entry->variant == PROCESS_NAME)
          {
               var_sym_entry->variant = PROCEDURE_NAME;
               return var_sym_entry;
          }
          else if (var_sym_entry->variant == ROUTINE_NAME)
	  {
	       var_sym_entry->variant = PROCEDURE_NAME;
	       return var_sym_entry;
	  }
          else if (var_sym_entry->variant == INTERFACE_NAME)
	       return var_sym_entry;
	  else
	  {
	       errstr("Inconsistent subroutine declaration %s", var_hash_entry->ident, 21);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     }	       
     if (var_sym_entry->scope == cur_scope())
	  switch(var_sym_entry->variant)
	  {
	  case PROCEDURE_NAME:
	       return (var_sym_entry);
	  case VARIABLE_NAME:
	       /* if not a formal parameter, convert it into 
                  a global procedure. */
	       if (var_sym_entry->entry.var_decl.local != IO)
	       {
		    var_sym_entry->variant = PROCEDURE_NAME;
                    if(kind != LOCAL)
		      var_sym_entry->scope = global_bfnd;
	       }
	       return (var_sym_entry);
          case PROCESS_NAME:
               var_sym_entry->variant = PROCEDURE_NAME;
               return (var_sym_entry);
	  case ROUTINE_NAME:
	       var_sym_entry->variant = PROCEDURE_NAME;
	       return (var_sym_entry);
	  case INTERFACE_NAME:
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent subroutine declaration %s", var_hash_entry->ident, 21);
	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident),
                            PROCEDURE_NAME, TYNULL, global_bfnd, kind));
}     
     

PTR_SYMB			/* make_process added for FORTRAN M */
make_process(var_hash_entry, kind)
PTR_HASH var_hash_entry;
int kind;
{
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL) {
	  if (kind == LOCAL)
	       return (make_sym_entry(var_hash_entry, PROCESS_NAME, TYNULL,
                                      cur_scope(), kind));
          else return (make_sym_entry(var_hash_entry, PROCESS_NAME, TYNULL,
                                      global_bfnd, kind));
     }
     if (var_sym_entry->scope == global_bfnd) {
	  if (var_sym_entry->variant == PROCESS_NAME)
	       return (var_sym_entry);
          else if (var_sym_entry->variant == PROCEDURE_NAME)
	  {
	       var_sym_entry->variant = PROCESS_NAME;
	       return var_sym_entry;
	  }
          else if (var_sym_entry->variant == ROUTINE_NAME)
	  {
	       var_sym_entry->variant = PROCESS_NAME;
	       return var_sym_entry;
	  }
          else if (var_sym_entry->variant == INTERFACE_NAME)
	       return var_sym_entry;
	  else
	  {
	       errstr("Inconsistent process %s %d", var_hash_entry->ident, var_sym_entry->variant, 317);
	       return (SMNULL);
	  }
     } 	       
     if (var_sym_entry->scope == cur_scope())
	  switch(var_sym_entry->variant)
	  {
	  case PROCESS_NAME:
	       return (var_sym_entry);
	  case VARIABLE_NAME:
	       /* if not a formal parameter, convert it into 
                  a global procedure. */
	       if (var_sym_entry->entry.var_decl.local != IO)
	       {
		    var_sym_entry->variant = PROCESS_NAME;
		    var_sym_entry->scope = global_bfnd;
	       }
	       return (var_sym_entry);
          case PROCEDURE_NAME:
               var_sym_entry->variant = PROCESS_NAME;
               return (var_sym_entry);
	  case ROUTINE_NAME:
	       var_sym_entry->variant = PROCESS_NAME;
	       return (var_sym_entry);
	  case INTERFACE_NAME:
	       return (var_sym_entry);
	  default:
	       errstr("Inconsistent process %s", var_hash_entry->ident,317);
	       return (SMNULL);
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident),
                            PROCESS_NAME, TYNULL, global_bfnd, kind));
}     
     

PTR_SYMB
make_program(var_hash_entry)
PTR_HASH var_hash_entry;
{
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
	  return (make_sym_entry(var_hash_entry, PROGRAM_NAME, TYNULL, 
                                 global_bfnd, 0));

     if (var_sym_entry->scope == global_bfnd)
	  {
	       errstr("Inconsistent program declaration %s", var_hash_entry->ident, 22);

	       return (var_sym_entry); /*return (SMNULL);*/
	  }
     	       
     if (var_sym_entry->scope == cur_scope())
	  {
	       errstr("Inconsistent program declaration %s", var_hash_entry->ident, 22);

	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), PROGRAM_NAME,
                            TYNULL, global_bfnd, 0));
}     


PTR_SYMB
make_module(var_hash_entry)
PTR_HASH var_hash_entry;
{
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
	  return (make_sym_entry(var_hash_entry, MODULE_NAME, TYNULL, 
                                 global_bfnd, 0));

     if (var_sym_entry->scope == global_bfnd)
	  {
	       errstr("Inconsistent module declaration %s", var_hash_entry->ident, 331);

	        return (var_sym_entry); /* return (SMNULL);*/
	  }
     	       
     if (var_sym_entry->scope == cur_scope())
	  {
	       errstr("Inconsistent module declaration %s", var_hash_entry->ident, 331);

	       return (var_sym_entry); /* return (SMNULL);*/
	  }
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), MODULE_NAME,
                            TYNULL, global_bfnd, 0));
}     




PTR_SYMB
make_common(var_hash_entry)
PTR_HASH var_hash_entry;
{
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
       return (make_sym_entry(var_hash_entry, DEFAULT, TYNULL, global_bfnd,
                              0));

     if (var_sym_entry->scope == global_bfnd) {
	  if (var_sym_entry->variant == DEFAULT)
	       return (var_sym_entry);
	  else 
	  { if(var_sym_entry->variant == FUNCTION_NAME){
                      /* intrinsic function  can have same name as a common block name */
               if(warn_all)
                 warn1("Common block have same name as a function  %s", var_hash_entry->ident, 25);
                return (var_sym_entry);
             }
             else
	       errstr("Inconsistent common declaration  %s", var_hash_entry->ident, 23);

	  return (var_sym_entry); /* return (SMNULL);*/
	  }
     } 	       
     /* A local entity can have same name as a common block name. So, ignore
        the local entity. */
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), DEFAULT,
                            TYNULL, global_bfnd, 0));
}     


PTR_SYMB
make_parallel_region(var_hash_entry)  /*SPF*/
PTR_HASH var_hash_entry;
{ 
     PTR_SYMB var_sym_entry;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
       return (make_sym_entry(var_hash_entry, SPF_REGION_NAME, TYNULL, global_bfnd,
                              0));

     if (var_sym_entry->scope == global_bfnd) {
	  if (var_sym_entry->variant != SPF_REGION_NAME)
	       errstr("Inconsistent region declaration  %s", var_hash_entry->ident, 630);
	  return (var_sym_entry);
	  
     } 	       
     
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), SPF_REGION_NAME,
                            TYNULL, global_bfnd, 0));
}     

PTR_TYPE 
make_type_node(typespec, lengspec)
PTR_TYPE typespec;
PTR_LLND lengspec;
{
     PTR_TYPE t;
     
     t = typespec;
     if (lengspec != LLNULL) 
     {
	  t = make_type(fi, typespec->variant);
          if(typespec->variant == T_STRING)
            t->entry.Template.dummy1 = typespec->entry.Template.dummy1; /* dummy1=2 for string constant inclosing " */  
          if(lengspec->variant == LEN_OP) {
	    t->entry.Template.ranges = lengspec;
	    t->entry.Template.kind_len = typespec->entry.Template.kind_len;
          }	    
          else
            t->entry.Template.kind_len = lengspec;	      
     }
     return (t);
}

PTR_SYMB
make_derived_type(var_hash_entry, type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE type;
int kind;
{
     PTR_SYMB var_sym_entry;
     PTR_TYPE struct_type;
     int var_variant;
     
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
     {
	  struct_type = make_type(fi, T_STRUCT);
	  return (make_sym_entry(var_hash_entry, TYPE_NAME, struct_type,
                                 cur_scope(), kind));
     }
     
     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd)
     {
	  struct_type = make_type(fi, T_STRUCT);
	  return (make_sym_entry(make_hash_entry(var_hash_entry->ident), TYPE_NAME, struct_type, 
                                 cur_scope(), kind));/*16.03.03*/
     }

     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  case VARIABLE_NAME:
	       /*  var_sym_entry's type */
	       var_sym_entry->variant = TYPE_NAME;
	       switch (var_sym_entry->type->variant) 
	       {
	       case T_STRING:
	       case T_ARRAY:
                    if (type != TYNULL)
                         var_sym_entry->type->entry.ar_decl.base_type = 
			      type;
                    return (var_sym_entry);
	       case T_INT:
	       case T_FLOAT:
	       case T_DOUBLE:
	       case T_COMPLEX:
	       case T_BOOL:
	       case T_POINTER:
	       case T_STRUCT:
	       {
		    struct_type = make_type(fi, T_STRUCT);
		    var_sym_entry->type = struct_type;
                    return (var_sym_entry);
	       }
               default:
		    return (var_sym_entry);
	       }
	  case TYPE_NAME:   /*added 24.04.12*/
	       {
		    struct_type = make_type(fi, T_STRUCT);
		    var_sym_entry->type = struct_type;
                    return (var_sym_entry);
	       }
    
	  default:
	       errstr("Inconsistent struct declaration of identifier %s", var_hash_entry->ident,318);
	       return (var_sym_entry);  /*  return (SMNULL);*/
	  }

     struct_type = make_type(fi, T_STRUCT);
     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), TYPE_NAME, struct_type,
			    cur_scope(), kind)); /*16.03.03*/
}

PTR_SYMB
make_local_entity(var_hash_entry, variant, type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE type;
int variant, kind;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
     {
	  return (make_sym_entry(var_hash_entry, variant, type,
                                 cur_scope(), kind));
     }
     
     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd)
     {
	  return (make_sym_entry(make_hash_entry(var_hash_entry->ident), variant, type, 
                                 cur_scope(), kind)); /*16.03.03*/
     }

     if (var_sym_entry->scope == cur_scope())
	  switch(var_variant) 
	  {
	  default:
	       if (variant == var_variant)
		    return(var_sym_entry);
	       else 
               {
                 errstr("Inconsistent declaration of identifier %s", var_hash_entry->ident, 16);
		 return (var_sym_entry); /*return (SMNULL);*/
	       }
	  };

     return (make_sym_entry(make_hash_entry(var_hash_entry->ident), variant, type, cur_scope(), kind));/*16.03.03*/
}

PTR_SYMB
make_global_entity(var_hash_entry, variant, type, kind)
PTR_HASH var_hash_entry;
PTR_TYPE type;
int variant, kind;
{
     PTR_SYMB var_sym_entry;
     int var_variant;
     
     var_sym_entry = var_hash_entry->id_attr;
     if (var_sym_entry == SMNULL)
     {
	  return (make_sym_entry(var_hash_entry, variant, type,
                                 global_bfnd, kind));
     }
     
     var_variant = var_sym_entry->variant;
     if (var_sym_entry->scope == global_bfnd)
	  switch(var_variant) 
	  {
	  default:
	       if (variant == var_variant)
		    return(var_sym_entry);
	       else 
               {
                 errstr("Inconsistent declaration of identifier %s", var_hash_entry->ident, 16);
		 return (var_sym_entry); /* return (SMNULL);*/
	       }
	  };
     errstr("Inconsistent declaration of identifier %s", var_hash_entry->ident, 16);
     return (var_sym_entry); /*return (SMNULL);*/
}

void
process_type(type_var, type_fields_end)
PTR_SYMB type_var;
PTR_BFND type_fields_end;
{
     PTR_SYMB sym_temp, last_field;
     PTR_LLND ll_temp;
     PTR_BFND temp;
     PTR_BLOB blob_temp;
     int count = 0;
     
     temp = cur_scope();
     blob_temp = temp->entry.Template.bl_ptr1;
     while(blob_temp->ref->variant != VAR_DECL)
          blob_temp = blob_temp->next;
     /*last_field = blob_temp->ref->entry.Template.ll_ptr1->entry.list.item->entry.Template.symbol;*/
     type_var->type->variant = T_STRUCT;
     type_var->type->name = type_var; /*18.03.03*/
     /* The field derived class is a constraint put by the design. Maybe instead of T_STRUCT, T_RECORD 
        might be better. But then it would be inconsistent with C++. */
     /*type_var->type->entry.derived_class.first = last_field;*/
     last_field = SMNULL;
     while (blob_temp && (blob_temp->ref != type_fields_end))
     {
          ll_temp = blob_temp->ref->entry.Template.ll_ptr1;
	  while (ll_temp != LLNULL) 
	  {
	       count++;
               if(ll_temp->entry.list.item->variant == ASSGN_OP || ll_temp->entry.list.item->variant == POINTST_OP)/*2.07.03*/
                  sym_temp = ll_temp->entry.list.item->entry.Template.ll_ptr1->entry.Template.symbol; 
               else
	          sym_temp = ll_temp->entry.list.item->entry.Template.symbol;
	      /* sym_temp->entry.Template.tag = FIELD_NAME;*/
               sym_temp->entry.field.tag = sym_temp->variant;
               sym_temp->variant = FIELD_NAME;
               sym_temp->entry.field.base_name = type_var;
               if(last_field){
	         last_field->entry.field.next = sym_temp;
                 last_field = sym_temp;
               }
               else {
                 last_field = sym_temp;
                 type_var->type->entry.derived_class.first = last_field;
               }
	       
	       ll_temp = ll_temp->entry.list.next;
	  }
	  blob_temp = blob_temp->next;
     }
     type_var->type->entry.derived_class.num_fields = count;
     last_field->entry.field.next = SMNULL;     
}     

void
process_interface(end_of_interface)
PTR_BFND end_of_interface;
{
     PTR_SYMB sym_temp, last_symbol, interface_symbol;
     PTR_BFND temp;
     PTR_BLOB blob_temp;
     PTR_LLND  list;
     
     temp = pred_bfnd;
     blob_temp = temp->entry.Template.bl_ptr1;
     interface_symbol = pred_bfnd->entry.Template.symbol;
     if (interface_symbol == SMNULL) return;
     if((pred_bfnd->variant != INTERFACE_STMT)) return;
     
     if(!blob_temp->ref) return;
 
     last_symbol = SMNULL;
     while (blob_temp && (blob_temp->ref != end_of_interface))
     {
	 sym_temp = blob_temp->ref->entry.Template.symbol;
         if(sym_temp != SMNULL){
          if(last_symbol) {
	    last_symbol->entry.Template.declared_name = sym_temp;
            last_symbol = sym_temp;
          }
          else {
            last_symbol = sym_temp;
            interface_symbol->entry.Template.symb_list = last_symbol;/*19.03.03*/
          }
	  blob_temp = blob_temp->next;
          continue;
         }
         /* MODULE_PROC_STMT(module procedure statement)*/
         list = blob_temp->ref->entry.Template.ll_ptr1;
         while(list){
            sym_temp = list->entry.Template.ll_ptr1->entry.Template.symbol;
            if(last_symbol) {
	         last_symbol->entry.Template.declared_name = sym_temp;
                 last_symbol = sym_temp;
             }
             else {
                last_symbol = sym_temp;
                interface_symbol->entry.Template.symb_list = last_symbol;/*19.03.03*/
             }   
            list = list->entry.Template.ll_ptr2;                   
          } 
	  blob_temp = blob_temp->next;  
     }
     last_symbol->entry.Template.declared_name = SMNULL;
}     

/*
void
process_interface(end_of_interface)
PTR_BFND end_of_interface;
{
     PTR_SYMB sym_temp, last_symbol, interface_symbol;
     PTR_BFND temp;
     PTR_BLOB blob_temp;
     
     temp = pred_bfnd;
     blob_temp = temp->entry.Template.bl_ptr1;
     interface_symbol = pred_bfnd->entry.Template.symbol;
     if (interface_symbol == SMNULL) return;
     if((pred_bfnd->variant != INTERFACE_STMT)) return;
     
     if(!blob_temp->ref) return;
     last_symbol = blob_temp->ref->entry.Template.symbol;
     interface_symbol->entry.Template.declared_name = last_symbol;
     while (blob_temp && (blob_temp->ref != end_of_interface))
     {
	  sym_temp = blob_temp->ref->entry.Template.symbol;
	  last_symbol->entry.Template.declared_name = sym_temp;
	  blob_temp = blob_temp->next;
	  last_symbol = sym_temp;
     }
     last_symbol->entry.Template.declared_name = SMNULL;
}     
*/

PTR_TYPE
lookup_type(name)
PTR_HASH name;
{
 /*    PTR_HASH hash_temp; */
     PTR_SYMB sym_temp;
     PTR_TYPE ty_temp;
     
 /*    hash_temp = just_look_up_sym(name->ident);  */ /*05.04.17*/
 /*    if (hash_temp && (sym_temp = hash_temp->id_attr) && (sym_temp->variant == TYPE_NAME))
     {
	  ty_temp = make_type(fi, T_DERIVED_TYPE);
	  ty_temp->name = sym_temp;
          ty_temp->entry.derived_type.symbol = sym_temp;
	  return (ty_temp);
     }
     errstr("Undefined type %s", name->ident,319);
     return (TYNULL);
 */  /*24.04.12*/
     if (name && (sym_temp = name->id_attr) && (sym_temp->variant == TYPE_NAME))
       ;
     else
       sym_temp = make_sym_entry(name, TYPE_NAME, TYNULL, cur_scope(), LOCAL); 
     ty_temp = make_type(fi, T_DERIVED_TYPE);
     ty_temp->name = sym_temp;
     ty_temp->entry.derived_type.symbol = sym_temp;
     return (ty_temp);

}

PTR_SYMB
component(type_sym_entry, fieldname)
PTR_SYMB type_sym_entry;
char *fieldname;
{
	int i;
	register PTR_HASH entry;
	register PTR_SYMB t;
        PTR_SYMB OriginalSymbol();
        
	i = hash(fieldname);
	for (entry = hash_table[i]; entry; entry = entry->next_entry) {
	     t = entry->id_attr; 
	        if (!strcmp(fieldname, entry->ident) && t && 
		 (t->variant == FIELD_NAME) && OriginalSymbol(t->entry.field.base_name) == OriginalSymbol(type_sym_entry)) /*type_sym_entry->type->name*/ /*BaseSymbol(type_sym_entry)*//*(!strcmp(t->entry.field.base_name->ident, type_sym_entry->ident)) */
		   return (t);
	     }
	
	return (SMNULL);
}

PTR_SYMB
lookup_type_symbol(type_sym_entry)
PTR_SYMB type_sym_entry;
{
	int i;
	register PTR_HASH entry;
	register PTR_SYMB s;
        if( type_sym_entry->type->variant==T_STRUCT && type_sym_entry->type->name)
           return (type_sym_entry);

        i = hash(type_sym_entry->ident);         
	for (entry = hash_table[i]; entry; entry = entry->next_entry) {
            s = entry->id_attr; 
            if (!strcmp(type_sym_entry->ident, entry->ident) && s)
            if(s->variant == TYPE_NAME && s->type->variant==T_STRUCT && s->type->name)
               return (s);  
        }
        return (SMNULL);
}

PTR_LLND
deal_with_options(name, type, attributes, dims, ndim, value, spec_dims)
PTR_HASH name;
PTR_TYPE type;
int attributes, ndim;
PTR_LLND dims, value, spec_dims;
{
     PTR_SYMB s;
     PTR_LLND l;
     PTR_TYPE t;

     l = LLNULL;
     s = SMNULL;
     t = TYNULL;

     if ((attributes & DIMENSION_BIT) && (dims != LLNULL))
     {   
	  t = make_type(fi, T_ARRAY);
	  t->entry.ar_decl.base_type = type;
	  t->entry.ar_decl.num_dimensions = ndim;
	  t->entry.ar_decl.ranges = dims; 
      }
     else
          t = type;

     if (attributes & PARAMETER_BIT) 
     {
	  s = make_constant(name, type);
	  s->entry.const_value = value;
          if(!value)
             errstr("An initialization expression is missing: %s", s->ident,267 );  
	  s->attr = s->attr | attributes;
          if((t->variant==T_STRING) || (t->variant==T_ARRAY)) 
	    l = make_llnd(fi, ARRAY_REF, spec_dims, LLNULL, s);	
	  else
	    l = make_llnd(fi, CONST_REF, LLNULL, LLNULL, s);
          s->type = t; /*7.03.03*/ 
	  return (l);
     }
     if (attributes & EXTERNAL_BIT) 
     {
	  s = make_scalar(name, type, NO);
	  s->attr = s->attr | attributes;
	  /*s->variant = ROUTINE_NAME;*//*7.02.03*/
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	  return (l);
     }

     if (attributes & INTRINSIC_BIT) 
     {
          s =  make_intrinsic(name, type); /*make_function(name, type, NO);*/
	  s->attr = s->attr | attributes;
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	  return (l);
     }

     s = make_scalar(name, type, LOCAL);
     s->attr = s->attr | attributes;

     if ((attributes & DIMENSION_BIT) && (dims != LLNULL))
     {   
          s->type = t;
          /*s = make_array(name, type, dims, ndim, LOCAL);*/
	  l = make_llnd(fi, ARRAY_REF, spec_dims, LLNULL, s);	
	  /*s->type->entry.ar_decl.ranges = dims;*/
     }

     if (l == LLNULL)
     {
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
     }

     if (value != LLNULL){
       if(value->variant == POINTST_OP) {
          value->entry.Template.ll_ptr1 = l;
          l = value;          
       } else
	  l = make_llnd(fi, ASSGN_OP, l, value, SMNULL);
          
          s->attr = s->attr | DATA_BIT;  /*7.03.03*/    
     }

     return (l);
}


/*
PTR_LLND
deal_with_options(name, type, attributes, dims, ndim, value, spec_dims)
PTR_HASH name;
PTR_TYPE type;
int attributes, ndim;
PTR_LLND dims, value, spec_dims;
{
     PTR_SYMB s;
     PTR_LLND l;

     l = LLNULL;
     if (attributes & PARAMETER_BIT) 
     {
	  s = make_constant(name, type);
	  s->entry.const_value = value;
	  s->attr = s->attr | attributes;
	  l = make_llnd(fi, CONST_REF, LLNULL, LLNULL, s);
	  * return (l);**7.03.03*
     }
     if (attributes & EXTERNAL_BIT) 
     {
	  s = make_scalar(name, type, NO);
	  s->attr = s->attr | attributes;
	  *s->variant = ROUTINE_NAME;**7.02.03*
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	  return (l);
     }
     if (attributes & INTRINSIC_BIT) 
     {
          s =  make_intrinsic(name, type); *make_function(name, type, NO);*
	  s->attr = s->attr | attributes;
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	  return (l);
     }
     if ((attributes & DIMENSION_BIT) && (dims != LLNULL))
     {   
          s = make_array(s->parent, TYNULL, dims, ndim, LOCAL);
	  l = make_llnd(fi, ARRAY_REF, spec_dims, LLNULL, s);
	  s->type->entry.ar_decl.ranges = dims;
     else
          s = make_scalar(name, type, LOCAL);

     s->attr = s->attr | attributes;


     if (l == LLNULL)
     {
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
     }

     if (value != LLNULL){
	  l = make_llnd(fi, ASSGN_OP, l, value, SMNULL); 
          s->attr = s->attr | DATA_BIT;  *7.03.03*   
     }
     return (l);
}
*/
/*    
PTR_BFND
parent_scope(p)
register PTR_BFND p;
{
     for (p = p->control_parent;
	     (p != BFNULL) &&
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
	     (p->variant != CDOACROSS_NODE) &&
	     (p->variant != STRUCT_DECL);
	     p = p->control_parent)
	  ;
	return (p);
}
*/

int is_array_section_ref(list)
PTR_LLND list;
{
     int num_triplets = 0;
     PTR_LLND p;
     PTR_TYPE t;

     p = list;
     while (p != LLNULL) 
     {
	  /* subscript range */
	  if (p->entry.Template.ll_ptr1->variant == DDOT) num_triplets++;
	  if ((t = list->entry.Template.ll_ptr1->type)) 
	  {
	       if (t->variant == KEYWORD_ARG) return (0);
	       /* vector subscript */
	       if ((t->variant == T_ARRAY) && (t->entry.ar_decl.base_type->variant == T_INT))
	       num_triplets++;
	  }
	  p = p->entry.Template.ll_ptr2;
     }
     
     return (num_triplets);
}

int is_substring_ref(list)
PTR_LLND list;
{    PTR_LLND l1;
     
     l1 = list->entry.Template.ll_ptr1;
     if (l1->variant == KEYWORD_ARG) return (0);
     if (list && (list->entry.Template.ll_ptr2 == LLNULL) && 
	 l1 && (l1->variant == DDOT) && 
	 (l1->entry.Template.ll_ptr1->variant == DDOT))
	  return (1);
     else return (0);
}

void
bind()
{
     PTR_SYMB s, sym;
     PTR_LLND tmp;
     PTR_HASH hash_node;
     
     while (first_unresolved_call != LLNULL) 
     {
	  s = first_unresolved_call->entry.Template.symbol;
	  if (s->decl == 0)
	  {
	       hash_node = just_look_up_sym_in_scope(s->scope->control_parent, s->ident);
	       if (hash_node == HSNULL) 
	       {
		    s->scope = global_bfnd;
	       }
	       else if ((hash_node->id_attr->variant == s->variant) || (hash_node->id_attr->variant == VARIABLE_NAME))
	       {
		    sym = hash_node->id_attr;
		    /*  remove s from symbol table. */
		    s->parent->id_attr = SMNULL;
		    first_unresolved_call->entry.Template.symbol = sym;
	       }
	       else  errstr("Inconsistent call %s", s->ident, 320);
	  }
			 
	  tmp = first_unresolved_call;
	  first_unresolved_call = tmp->entry.Template.ll_ptr2;
	  tmp->entry.Template.ll_ptr2 = LLNULL; 
     }
}     

void
late_bind_if_needed(ll_node)
PTR_LLND ll_node;
{
     PTR_SYMB s;
     
     s = ll_node->entry.Template.symbol;
     if ((s->entry.var_decl.local == IO) || 
	 (s->decl == YES))
	  return;
     else 
     {
	  if (first_unresolved_call == LLNULL) 
	       last_unresolved_call = first_unresolved_call = ll_node;
	  else 
	  {
	       last_unresolved_call->entry.Template.ll_ptr2 = ll_node; 
	       last_unresolved_call = ll_node;
	  }
     }	  
}  
     
void
redefine_func_arg_type()
{PTR_BFND hedr;
 PTR_SYMB arg,proc,res;
 hedr = cur_scope();
 if((hedr->variant == FUNC_HEDR) ||(hedr->variant == PROC_HEDR))
   proc = hedr->entry.Template.symbol;
 else
   return;
 
 if((hedr->variant == FUNC_HEDR) && (hedr->entry.Template.ll_ptr2 == LLNULL)){
   if (proc->type->variant == T_ARRAY)
      proc->type->entry.ar_decl.base_type = impltype[*proc->ident - 'a'];
   else
      proc->type = impltype[*proc->ident - 'a'];
   if (hedr->entry.Template.ll_ptr1 != LLNULL){
     res =  hedr->entry.Template.ll_ptr1->entry.Template.symbol;
     res->type =  impltype[*res->ident - 'a']; 
   }
 }
 for(arg = proc->entry.proc_decl.in_list; arg; arg=arg->entry.var_decl.next_in)
  if (arg->type->variant == T_ARRAY)
      arg->type->entry.ar_decl.base_type = impltype[*arg->ident - 'a'];
   else
      arg->type = impltype[*arg->ident - 'a'];
}

int 
in_rename_list(symb,list)
     PTR_SYMB symb;
     PTR_LLND list;
{ PTR_SYMB s;
  PTR_LLND l;
 for(l = list; l ; l = l->entry.Template.ll_ptr2){
   s = l->entry.Template.ll_ptr1->entry.Template.ll_ptr2->entry.Template.symbol;
   if(!strcmp(symb->ident,s->ident))
     return(1);
 }
 return(0);
}

void
copy_sym_data(source, dest)
PTR_SYMB source, dest;
{PTR_SYMB BaseSymbol();
   if(source->variant == CONST_NAME) {/* named constant */ /*16.03.03 */
     dest->entry.const_value = source->entry.const_value;
     dest->entry.Template.base_name = BaseSymbol(source); /*27.01.12*/
     dest->attr = source->attr;   /*06.11.12*/
     return;
   }
   if(dest->entry.Template.seen == BY_USE) return;

     dest->attr = source->attr; /* source->attr  & (~PRIVATE_BIT) & (~PUBLIC_BIT);*/
     dest->attr = dest->attr & (~PRIVATE_BIT);
     dest->attr = dest->attr & (~PUBLIC_BIT);
     if(privateall)
	 dest->attr =  dest->attr | PRIVATE_BIT;
     dest->entry.Template.seen = BY_USE; /*source->entry.Template.seen;*/
     dest->entry.Template.num_input = source->entry.Template.num_input;
     dest->entry.Template.num_output = source->entry.Template.num_output;
     dest->entry.Template.num_io = source->entry.Template.num_io;
     dest->entry.Template.in_list = source->entry.Template.in_list;
     dest->entry.Template.out_list = source->entry.Template.out_list;
     dest->entry.Template.symb_list = source->entry.Template.symb_list;
     dest->entry.Template.local_size = source->entry.Template.local_size;
     dest->entry.Template.label_list = source->entry.Template.label_list;
     dest->entry.Template.func_hedr = source->entry.Template.func_hedr;
     dest->entry.Template.call_list = source->entry.Template.call_list;
     dest->entry.Template.tag = source->entry.Template.tag;
     dest->entry.Template.offset = source->entry.Template.offset;
     dest->entry.Template.declared_name = source->entry.Template.declared_name;
     /*dest->entry.Template.next = source->entry.Template.next;*/
     /*dest->entry.Template.base_name = source->entry.Template.base_name;*/

     dest->entry.Template.base_name = BaseSymbol(source); /*25.03.03*/ /*source;*/    
}

void 
copy_module_scope(sym_mod,list)
     PTR_SYMB sym_mod;
     PTR_LLND list;
 
{PTR_SYMB new_symb, source;
 for(source=sym_mod->entry.Template.next; source; source=source->entry.Template.next) {
   if((source->attr & PRIVATE_BIT) && (!(source->attr & PUBLIC_BIT)) )
     continue;
   if(list && in_rename_list(source,list))
      continue;
    new_symb = make_local_entity(source->parent, source->variant, source->type, LOCAL);
    copy_sym_data(source,new_symb);
  
   /* if(new_symb->entry.Template.seen != BY_USE) {
       copy_sym_data(source,new_symb);  
       new_symb->entry.Template.base_name = source; 
      }
   */      
 }
   return;
}
      
PTR_SYMB
BaseSymbol(sym)
     PTR_SYMB sym;
{ PTR_SYMB s;
 s = sym;
 while(s && s->entry.Template.seen == BY_USE)
   s = s->entry.Template.base_name;
 return(s);
}

PTR_SYMB
OriginalSymbol(sym)
     PTR_SYMB sym;
{ 
 if(sym->entry.Template.base_name) 
   return(sym->entry.Template.base_name);
 else
   return(sym);
}

int isResultVar(sym)
     PTR_SYMB sym;
{ PTR_BFND curstmt;
  curstmt = cur_scope();
  if ((sym->variant == FUNCTION_NAME) &&(curstmt->variant == FUNC_HEDR) && (!curstmt->entry.Template.ll_ptr1) &&
	      (!(strcmp(sym->parent->ident, curstmt->entry.Template.symbol->ident))))
    return(1); /* function name is a result variable name */
  else
    return(0);
}

void replace_symbol_in_expr(PTR_LLND expr, PTR_SYMB symb)
{                                                      
   if(!expr) 
      return;
   if(expr->variant == VAR_REF)
      if(!strcmp(expr->entry.Template.symbol->ident, symb->ident))
         expr->entry.Template.symbol = symb;
   replace_symbol_in_expr(expr->entry.Template.ll_ptr1,symb);
   replace_symbol_in_expr(expr->entry.Template.ll_ptr2,symb);
}