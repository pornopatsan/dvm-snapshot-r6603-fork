/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/*
 * types.c
 *
 * Routines to handle the type and variable decalrations
 */

#include <stdio.h>
#include "defs.h"
#include "ll.h"
#include "symb.h"
#include "db.h"
#include "f90.h"
#include "fm.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

PTR_LLND make_llnd();
PTR_TYPE make_type();
PTR_BFND get_bfnd();
extern void err();
extern PTR_FILE fi;
void errstr();
int match_args();
int match_an_arg();
int remainder_optional();

extern PTR_TYPE global_int, global_double, global_float, global_bool;
extern PTR_TYPE global_char, global_string, global_complex, global_dcomplex, global_default;

/*
 * set_type takes an id_list and a base type goes through list
 *  settting up the type used for variables and params 
 */
void
set_type(id_list, base, tag)
	PTR_SYMB id_list;
	PTR_TYPE base;
	int     tag;
{
	PTR_SYMB temp;
	PTR_SYMB last;

	temp = id_list;
	while (temp) {
		temp->variant = VARIABLE_NAME;
		temp->type = base;
		temp->entry.var_decl.local = tag;
	/* if parameter the local field should already be set */
		last = temp;
		temp = last->id_list;
		last->id_list = SMNULL;
	}
}


/*
 * install_const takes an id_list and an expr sets up the constant declarations
 */
void 
install_const(id_list, expr)
	PTR_SYMB id_list;
	PTR_LLND expr;
{
	register PTR_SYMB temp, last;

	temp = id_list;
	while (temp) {
		temp->variant = CONST_NAME;
		temp->type = expr->type;
		temp->entry.const_value = expr;
		last = temp;
		temp = last->id_list;
		last->id_list = SMNULL;
	}
}


/*
 * install_array
 */
PTR_TYPE 
install_array(range, base, ndim)
	PTR_LLND range;
	PTR_TYPE base;
	int     ndim;
{
	PTR_TYPE ret;

	ret = make_type(fi,T_ARRAY);
	ret->entry.ar_decl.num_dimensions = ndim;
	ret->entry.ar_decl.base_type = base;
	return (ret);
}


/* 
 * install_param_list gets an proc id and a parameter list
 * and sets them up as parameters
 */
void
install_param_list(id, id_list, result_clause, func)
PTR_SYMB id, id_list;
int func;
PTR_LLND result_clause;
{
	int     num_input = 0;
	register PTR_SYMB temp, last, result_sym_ptr;

	temp = id_list;
	while (temp) {
		temp->variant = VARIABLE_NAME;
		temp->entry.var_decl.next_in = temp->id_list;
		temp->entry.var_decl.local = IO;
		++num_input;
		last = temp;
		temp = last->id_list;
		last->id_list = SMNULL;
	}
	id->entry.proc_decl.in_list = id_list;
	id->entry.proc_decl.num_io = 0;
	id->entry.proc_decl.num_input = num_input;
	id->entry.proc_decl.num_output = (func == FUNCTION_NAME);
	if (result_clause) 
	{
	     result_sym_ptr = result_clause->entry.Template.symbol;
	     /* id->entry.Template.declared_name = result_sym_ptr;*/ /*16.03.03*/
	     result_sym_ptr->type = id->type;
	}
}

PTR_LLND 
construct_entry_list(entry_id, symb_list, entry_kind)
PTR_SYMB entry_id, symb_list;
int entry_kind;
{
     int     num_input = 0;
     PTR_SYMB last_symb;
     PTR_LLND entry_param_list, l, set_ll_list();
    
     entry_param_list = LLNULL;
     if (symb_list) 
     {
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, symb_list);
	  entry_param_list = set_ll_list(l, LLNULL, EXPR_LIST);
	  last_symb = symb_list;
	  symb_list = last_symb->id_list;
	  last_symb->id_list = SMNULL;
	  num_input++;
          last_symb->entry.var_decl.local = IO; /*5.02.03*/
     }

     while (symb_list) 
     {
	  l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, symb_list);
	  entry_param_list = set_ll_list(entry_param_list, l, EXPR_LIST);
	  last_symb = symb_list;
	  symb_list = last_symb->id_list;
	  last_symb->id_list = SMNULL;
	  num_input++;
          last_symb->entry.var_decl.local = IO; /*5.02.03*/
     }

     /* entry_id->entry.proc_decl.in_list = symb_list; */
     entry_id->entry.proc_decl.num_io = 0;
     entry_id->entry.proc_decl.num_input = num_input;
     entry_id->entry.proc_decl.num_output = (entry_kind == FUNCTION_NAME);
     return entry_param_list;
  
}

	  
/*
 * set_expr_type sets the type of an expression and
 *    performs other semantic checks
 */
void 
set_expr_type(expr)
	PTR_LLND expr;
{
	int     r_type, l_type, ilen;
	PTR_TYPE temp, l_operand, r_operand, copy_type_node();
	PTR_LLND len;
        len = NULL;
        ilen = 0;
	switch (expr->variant) {

	    case (ARRAY_MULT):	/* to be changed */
	    case (PROC_CALL):
	    case (DEF_CHOICE):
	    case (VAR_LIST):
	    case (EXPR_LIST):
	    case (RANGE_LIST):
	    case (RANGE_OP):
	    case (STAR_RANGE):
		expr->type = global_default;
		break;

	    case (INT_VAL):
		expr->type = global_int;
		break;
	    case (FLOAT_VAL):
		expr->type = global_float;
		break;
	    case (DOUBLE_VAL):
		expr->type = global_double;
		break;
	    case (BOOL_VAL):
		expr->type = global_bool;
		break;
	    case (CHAR_VAL):
		expr->type = global_char;
		break;
	    case (STMT_STR):
	    case (STRING_VAL):
		expr->type = global_string;
		break;
	    case (COMPLEX_VAL):
		expr->type = global_complex;
		break;
	    case (CONST_REF):	/* check that the symbol referred to is a
				 * constant and then return type */
	       if (expr->entry.const_ref.symbol->variant != CONST_NAME) {
		 /*err("Not a constant identifier");*/ /*podd*/
			expr->type = global_default;
		} else
			expr->type = expr->entry.const_ref.symbol->type;
		break;
	    case (ENUM_REF):
	    case (VAR_REF):
		expr->type = expr->entry.var_ref.symbol->type;
		if (expr->type == global_default)
			err("Identifier not declared",321);
		break;

	    case (ARRAY_REF):	/* check to see if symbol is array ??? */
		expr->type = expr->entry.array_ref.array_elt ?
			expr->entry.array_ref.array_elt->type :
			expr->entry.array_ref.symbol->type;
		break;

	    case (RECORD_REF):
		expr->type = expr->entry.record_ref.rec_field ?
			expr->entry.record_ref.rec_field->type :
			expr->entry.record_ref.symbol->type;
		break;
	    case (MOD_OP):
	    case (LOWER_OP):
	    case (UPPER_OP):
		expr->type = global_int;
		break;
	    case (AND_OP):
	    case (OR_OP):
	    case (EQ_OP):
	    case (LT_OP):
	    case (GT_OP):
	    case (NOTEQL_OP):
	    case (LTEQL_OP):
	    case (EQV_OP):
	    case (NEQV_OP):
	    case (GTEQL_OP):
		 if ((expr->entry.binary_op.l_operand == LLNULL) ||
                     (expr->entry.binary_op.l_operand->type == TYNULL) ||
		     (expr->entry.binary_op.r_operand == LLNULL) ||
                     (expr->entry.binary_op.r_operand->type == TYNULL))
		   /*err("Inconsistent operands to boolean operation", 26);*/
                     expr->type = global_default;
		if (expr->entry.binary_op.l_operand->type->variant == T_ARRAY)
		{
		     expr->type = copy_type_node(expr->entry.binary_op.l_operand->type);
		     expr->type->entry.ar_decl.base_type =  global_bool;
		}
                else if (expr->entry.binary_op.r_operand->type->variant == T_ARRAY)
		{
		     expr->type = copy_type_node(expr->entry.binary_op.r_operand->type);
		     expr->type->entry.ar_decl.base_type =  global_bool;
		}
		else  expr->type =  global_bool;
		break;
	    case (DIV_OP):
	    case (ADD_OP):
	    case (SUBT_OP):
	    case (MULT_OP):
            case (EXP_OP):
		 if ((expr->entry.binary_op.l_operand == LLNULL) ||
                     (expr->entry.binary_op.l_operand->type == TYNULL) ||
		     (expr->entry.binary_op.r_operand == LLNULL) ||
                     (expr->entry.binary_op.r_operand->type == TYNULL))
		   /* err("Inconsistent operands to arithmetic operation", 27);*/
                     expr->type = global_default;
  	        l_operand = expr->entry.binary_op.l_operand->type;
		r_operand = expr->entry.binary_op.r_operand->type;
		if (! l_operand || ! r_operand)
			expr->type = global_default;
		else {
		        if (l_operand->variant == T_ARRAY)
			     l_type = l_operand->entry.ar_decl.base_type->variant;
			else
			     l_type = l_operand->variant;
		        if (r_operand->variant == T_ARRAY)
			     r_type = r_operand->entry.ar_decl.base_type->variant;
			else
			     r_type = r_operand->variant;
                        if(l_operand->entry.Template.ranges)
                        { len=(l_operand->entry.Template.ranges)->entry.Template.ll_ptr1;
                          if(len && len->variant==INT_VAL)
                             ilen=len->entry.ival; 
                          if(l_type==T_FLOAT && ilen==8)                        
                             l_type=T_DOUBLE;
                          if(l_type==T_COMPLEX && ilen==16)                        
                             l_type=T_DCOMPLEX;
                        }
                        if(r_operand->entry.Template.ranges)
                        { len=(r_operand->entry.Template.ranges)->entry.Template.ll_ptr1;
                          if(len && len->variant==INT_VAL)
                             ilen=len->entry.ival; 
                          if(r_type==T_FLOAT && ilen==8)                        
                             r_type=T_DOUBLE;
                          if(r_type==T_COMPLEX && ilen==16)                        
                             r_type=T_DCOMPLEX;
                        }

			if (l_type == T_DCOMPLEX || r_type == T_DCOMPLEX)
				temp = global_dcomplex;
			else if (l_type == T_COMPLEX || r_type == T_COMPLEX)
				temp = global_complex;
			else if (l_type == T_DOUBLE || r_type == T_DOUBLE)
				temp = global_double;
			else if (l_type == T_FLOAT || r_type == T_FLOAT)
				temp = global_float;
			else if (l_type == T_INT && r_type == T_INT) 
				temp = global_int;
				
		        else temp = global_default;

			if (l_operand->variant == T_ARRAY)
			{
			     expr->type = copy_type_node(expr->entry.binary_op.l_operand->type);
			     expr->type->entry.ar_decl.base_type =  temp;
			}
                        else if (r_operand->variant == T_ARRAY)
		        {
		            expr->type = copy_type_node(expr->entry.binary_op.r_operand->type);
		            expr->type->entry.ar_decl.base_type =  temp;
		        }
			else  expr->type =  temp;
		   }
		break;
	    case (NOT_OP):
	    case (UNARY_ADD_OP):
	    case (MINUS_OP):
		expr->type = expr->entry.unary_op.operand->type;
		break;
	   /* case (EXP_OP):
		expr->type = expr->entry.binary_op.l_operand->type;
		break;
            */
	    case (CONCAT_OP):
		expr->type = expr->entry.binary_op.l_operand->type;
		break;
	    case (DDOT):
		expr->type = expr->entry.binary_op.r_operand->type;
		break;
	    default:
		err("Expression variant not known",322);
		break;

	}
}


/* 
 * chase_qual_index  gets a partially set up variable and an index list
 * and has to chase down array elements or record fields to hang the list
 */
void 
chase_qual_index(ref, index_list)
	PTR_LLND ref, index_list;
{
	if (ref->variant == ARRAY_REF)
		chase_qual_index(ref->entry.array_ref.array_elt, index_list);
	else if (ref->variant == RECORD_REF)
		chase_qual_index(ref->entry.record_ref.rec_field, index_list);
	else if (ref->variant == VAR_REF) {

	/*
	 * check that this is an array ref symbol and that dimensions are ok
	 * etc. 
	 */
		ref->variant = ARRAY_REF;
		ref->entry.array_ref.index = index_list;
	}
}


/*
 * chase_qual_field gets ref and a field_id and has to be set up
 */
void 
chase_qual_field(ref, field_id)
	PTR_LLND ref;
	PTR_SYMB field_id;
{
	if (ref->variant == ARRAY_REF) {
		if (! ref->entry.array_ref.array_elt)
			ref->entry.array_ref.array_elt =
				make_llnd(fi,VAR_REF, LLNULL, LLNULL, field_id);
		else
			chase_qual_field(ref->entry.array_ref.array_elt, field_id);
	} else if (ref->variant == RECORD_REF) {
		if (! ref->entry.record_ref.rec_field)
			ref->entry.record_ref.rec_field =
				make_llnd(fi,VAR_REF, LLNULL, LLNULL, field_id);
		else
			chase_qual_field(ref->entry.record_ref.rec_field, field_id);
	} else if (ref->variant == VAR_REF) {
		ref->variant = RECORD_REF;
		ref->entry.record_ref.rec_field =
			make_llnd(fi,VAR_REF, LLNULL, LLNULL, field_id);
	} else
		err("Error in chase filed ids", 323);
}

PTR_TYPE
copy_type_node(typenode)
PTR_TYPE typenode;
{
     PTR_TYPE new_node;
     
     new_node = make_type(fi, typenode->variant);
     
     new_node->entry.Template.base_type = typenode->entry.Template.base_type;
     new_node->entry.Template.ranges = typenode->entry.Template.ranges;
     new_node->entry.Template.dummy1 = typenode->entry.Template.dummy1;
     new_node->entry.Template.kind_len = typenode->entry.Template.kind_len;
     new_node->entry.Template.dummy3 = typenode->entry.Template.dummy3;
     new_node->entry.Template.dummy4 = typenode->entry.Template.dummy4;
     new_node->entry.Template.dummy5 = typenode->entry.Template.dummy5;

     return (new_node);
}

PTR_SYMB
resolve_overloading(interface_symbol, argptr)
PTR_SYMB interface_symbol;
PTR_LLND argptr;
{
     PTR_SYMB current;
     void reset_args();
     
     if (interface_symbol->variant != INTERFACE_NAME)
	  return (interface_symbol);
     
     current = interface_symbol->entry.Template.symb_list; /*entry.Template.declared_name;*//*19.03.03*/
     while (current) 
     {
	  reset_args(current);
	  if (match_args(current, argptr))
	       return (current);
	  current = current->entry.Template.declared_name;
     }
     return (SMNULL);
}

int
match_args(contender_proc, argptr)
PTR_SYMB contender_proc;
PTR_LLND argptr;
{
     PTR_SYMB formal_argptr, formal_arg, find_keyword_arg();
     PTR_LLND argref, actual_arg;
     int match_status;
     
     formal_argptr = contender_proc->entry.Template.in_list;
     while (argptr && formal_argptr)
     {
	  argref = argptr->entry.list.item;
	  if (argref->variant == KEYWORD_ARG)
	  {
	       formal_arg = find_keyword_arg(contender_proc,
                             argref->entry.Template.ll_ptr1->entry.string_val);
	       actual_arg = argref->entry.Template.ll_ptr2;
	  }
	  else 
	  {
	       formal_arg = formal_argptr;
	       actual_arg = argref;
	  }
	  match_status = match_an_arg(actual_arg, formal_arg);
	  if (!match_status)
	       return (0);
	  formal_argptr = formal_argptr->entry.var_decl.next_in;
	  argptr = argptr->entry.list.next;
     }
     if (remainder_optional(contender_proc))
	  return (1);
     else return (0);
}


int
match_an_arg(actual_arg, formal_arg)
PTR_LLND actual_arg;
PTR_SYMB formal_arg;
{
     PTR_TYPE actual_arg_type, formal_arg_type;

     actual_arg_type = actual_arg->type;
     formal_arg_type = formal_arg->type;
     
     if (actual_arg_type->variant != formal_arg_type->variant) 
	  return (0);
     
     if ((actual_arg_type->variant == T_DERIVED_TYPE) &&
	 (actual_arg_type->name != formal_arg_type->name))
	  return (0);

     if (actual_arg_type->variant == T_ARRAY)
     {
	  if (actual_arg_type->entry.ar_decl.base_type->variant !=
 	      formal_arg_type->entry.ar_decl.base_type->variant)
	       return (0);

	  if ((actual_arg_type->entry.ar_decl.base_type->variant == T_DERIVED_TYPE)
	      && (actual_arg_type->entry.ar_decl.base_type->name !=
		  formal_arg_type->entry.ar_decl.base_type->name))
	       return (0);
	  
	  if (actual_arg_type->entry.ar_decl.num_dimensions !=
	      formal_arg_type->entry.ar_decl.num_dimensions)
	       return (0);
     }

     formal_arg->decl = 1;
     return (1);
}

void
reset_args(proc)
PTR_SYMB proc;
{
     PTR_SYMB temp;
     
     temp = proc->entry.Template.in_list;
     while (temp != SMNULL) 
     {
	  temp->decl = 0;
	  temp = temp->entry.var_decl.next_in;
     }
}

PTR_SYMB
find_keyword_arg(proc, keyword)
PTR_SYMB proc;
char *keyword;
{
     PTR_SYMB temp;
     
     temp = proc->entry.Template.in_list;
     while (temp != SMNULL) 
     {
	  if (!strcmp(temp->ident, keyword))
	       return (temp);
	  temp = temp->entry.var_decl.next_in;
     }
     
     return (SMNULL);
}

int
remainder_optional(proc)
PTR_SYMB proc;
{
     PTR_SYMB temp;
     
     temp = proc->entry.Template.in_list;
     while (temp != SMNULL) 
     {
	  if ((temp->decl == 0) && !(temp->attr & OPTIONAL_BIT))
	       return (0);
	  temp = temp->entry.var_decl.next_in;
     }
     
     return (1);
}     

PTR_LLND
intrinsic_op_node(opname, op, rand1, rand2)
char *opname;
int op;
PTR_LLND rand1, rand2;
{
     PTR_HASH hash_node;
     PTR_SYMB s, sym;
     PTR_LLND temp1, temp2, l, result;
     extern PTR_HASH just_look_up_sym();
     
     hash_node = just_look_up_sym(opname);
     if (hash_node == HSNULL) 
     {
	result = make_llnd(fi, op, rand1, rand2, SMNULL);
        set_expr_type(result); 
        return (result);
     }
     else 
     {
	  s = hash_node->id_attr;
	  if (s->variant != INTERFACE_NAME)
	  {
             errstr("Can't resolve call %s", opname,324);
	     return (LLNULL);
 	  }

	  temp1 = make_llnd(fi, EXPR_LIST, rand1, LLNULL, SMNULL);
	  temp2 = make_llnd(fi, EXPR_LIST, rand2, LLNULL, SMNULL);
	  temp1->entry.Template.ll_ptr2 = temp2;
	  sym = resolve_overloading(s, temp1);
	  if (sym != SMNULL)
	  {
	       l = make_llnd(fi, FUNC_CALL, temp1, LLNULL, sym);
	       l->type = sym->type;
	       result = make_llnd(fi, OVERLOADED_CALL, l, LLNULL, s);
	       result->type = sym->type;
               return (result);
	  }
	  else {
	       result = make_llnd(fi, op, rand1, rand2, SMNULL);
	       set_expr_type(result); 
	       return (result);
	  }
     }
 return (result);
}

PTR_LLND
defined_op_node(hash_node, rand1, rand2)
PTR_HASH hash_node;
PTR_LLND rand1, rand2;

{
     PTR_SYMB s, sym;
     PTR_LLND temp1,temp2, l, result;
     PTR_TYPE  type;
     extern PTR_HASH just_look_up_sym();
     
     if (hash_node == HSNULL) 
     { 
	  err("Unknown operator",316);
	  return (LLNULL);
     }
     else 
     {
	  s = hash_node->id_attr;
          if(s == SMNULL ){
             errstr("Unknown operator %s",hash_node->ident,316);
	     return (LLNULL);
          }
	  if ( s->variant != INTERFACE_NAME)
 
	  {  
             errstr("Can't resolve call %s", s->ident,324);
	     return (LLNULL);
 	  }
	 
	  temp1 = make_llnd(fi, EXPR_LIST, rand1, LLNULL, SMNULL);
          if(rand2 != LLNULL){
	    temp2 = make_llnd(fi, EXPR_LIST, rand2, LLNULL, SMNULL);
	    temp1->entry.Template.ll_ptr2 = temp2;
          }
	  sym = resolve_overloading(s, temp1);
          l = make_llnd(fi, DEFINED_OP, rand1, rand2, s);
          if (sym != SMNULL)
             type = sym->type;
          else
             type = global_default;
          l->type = type;
	  result = make_llnd(fi, OVERLOADED_CALL, l, LLNULL, sym);
	  result->type = type;
          return (result);
	  /*
	  if (sym != SMNULL)
	  { 
	       l = make_llnd(fi, FUNC_CALL, temp1, LLNULL, sym);
 
	       l->type = sym->type;
	       result = make_llnd(fi, OVERLOADED_CALL, l, LLNULL, sym);
	       result->type = sym->type;
               return (result);
	  }
	  else {
	    errstr("Can't resolve call %s", hash_node->ident,324);

	     return (result);
	  }
         */ /*2.07.03*/
     }
 return (result);
}

	       

PTR_BFND
subroutine_call(subroutine_name, argptr)
PTR_SYMB subroutine_name;
PTR_LLND argptr;
{
     PTR_BFND stmt;
     PTR_LLND ll_ptr;
     PTR_SYMB current;
     void reset_args();
     
  /*   if (subroutine_name->variant != INTERFACE_NAME) */ /*19.08.03*/
     {
	  stmt = get_bfnd(fi,PROC_STAT, subroutine_name, argptr, LLNULL, LLNULL);
	  return (stmt);
     }
     
     current = subroutine_name->entry.Template.symb_list; /*entry.Template.declared_name;*//*19.03.03*/
     while (current) 
     {
	  reset_args(current);
	  if (match_args(current, argptr))
	  {
	       ll_ptr = make_llnd(fi, PROC_CALL, argptr, LLNULL, current);
	       stmt = get_bfnd(fi,OVERLOADED_PROC_STAT, subroutine_name, argptr, ll_ptr,LLNULL);
	       return (stmt);
	  }
	  current = current->entry.Template.declared_name;
     }
     return (BFNULL);
}


PTR_BFND			/* process_call added for FORTRAN M */
process_call(process_name, argptr, p_mapping, type)
PTR_SYMB process_name;
PTR_LLND argptr;
PTR_LLND p_mapping;
int type;
{
     PTR_BFND stmt = NULL;
     /*PTR_LLND ll_ptr;*/
     /*PTR_SYMB current;*/
     void reset_args();
     
     if (process_name->variant != INTERFACE_NAME) 
     {
	  switch (type) {
	    case PLAIN:
	      stmt = get_bfnd(fi, PROS_STAT, process_name, argptr,
							 p_mapping, LLNULL);
              break;
            case LCTN:
	      stmt = get_bfnd(fi, PROS_STAT_LCTN, process_name, argptr,
							 p_mapping, LLNULL);
	      break;
            case SUBM:
	      stmt = get_bfnd(fi, PROS_STAT_SUBM, process_name, argptr,
							 p_mapping, LLNULL);
	      break;
	    default:
              errstr("Invalid type of process call %d", type,325);
	  }
	  return (stmt);
     }
     
     return (BFNULL);
}

