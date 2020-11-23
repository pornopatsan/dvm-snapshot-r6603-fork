/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/************************************************************************/
/*									*/
/*			  low level nodes				*/
/*									*/
/************************************************************************/

struct llnd {

    int		variant, id;   /* variant and identification tags */

    PTR_LLND	thread;	   /* connects nodes together by allocation order */

    PTR_TYPE	type;	   /* to be modified */

    union llnd_union {

	char *string_val;/* for integers floats doubles and strings*/
	int	ival;
	double	dval;	    /* for floats and doubles */
	char	cval;
	int	bval;	    /* for booleans */

	struct {			/* for range, upper, and lower */
		PTR_SYMB  symbol;
		int	  dim;
	       }					array_op;

	struct {
		PTR_SYMB  symbol;

		PTR_LLND  ll_ptr1;
		PTR_LLND  ll_ptr2;
	       }					Template;

	struct {	    /* for complexes and double complexes */
		PTR_SYMB  null;

		PTR_LLND  real_part;
		PTR_LLND  imag_part;
	       }					complex;

	struct {
		PTR_LABEL lab_ptr;

		PTR_LLND  null_1;
		PTR_LLND  next;
	       }					label_list;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  item; 
		PTR_LLND  next;
	       }					list;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  size;
		PTR_LLND  list;
	       }					cons;
	
	struct {
		PTR_SYMB  control_var;
		   
		PTR_LLND  array;
		PTR_LLND  range;
	       }					access;

	struct {
		PTR_SYMB  control_var;

		PTR_LLND  array;
		PTR_LLND  range;
	       }					ioaccess;

	struct {
		PTR_SYMB  symbol;

		PTR_LLND  null_1;
		PTR_LLND  null_2;
	       }					const_ref;

	struct {
		PTR_SYMB  symbol;

		PTR_LLND  null_1;
		PTR_LLND  null_2;
	       }					var_ref;

	struct {
		PTR_SYMB  symbol;

		PTR_LLND  index;
		PTR_LLND  array_elt;
	       }					array_ref;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  access;
		PTR_LLND  index;
	       }					access_ref;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  cons;
		PTR_LLND  index;
	       }					cons_ref;

	struct {
		PTR_SYMB  symbol;

		PTR_LLND  null_1;
		PTR_LLND  rec_field;  /* for record fields */
	       }					record_ref; 


	struct {
		PTR_SYMB  symbol;

		PTR_LLND  param_list;
		PTR_LLND  next_call;
	       }					proc;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  operand;  
		PTR_LLND  null_2;
	       }					unary_op; 

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  l_operand;
		PTR_LLND  r_operand;
	       }					binary_op;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  ddot;
		PTR_LLND  stride;
	       }					seq;

	struct {
		PTR_SYMB  null_1;

		PTR_LLND  sp_label;
		PTR_LLND  sp_value;
	       }					spec_pair;

    } entry;
};

#define __LL_DEF__
