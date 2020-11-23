/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/************************************************************************
 *								        *
 *			       BIF NODES				*
 *								        *
 ************************************************************************/

struct bfnd {

     int	variant, id;	/* variant and identification tags */
     int	index;		/* used in the strongly con. comp. routines */
     int	g_line, l_line; /* global & local line numbers */
     int        decl_specs; /* declaration specifiers stored with
                               bif nodes: static, extern, friend, and inline */

     PTR_LABEL	label;
     PTR_BFND	thread;

     PTR_FNAME	filename;	/* point to the source filename */

     PTR_BFND	control_parent; /* current bif node in on the control blob list
				  of control_parent */
     PTR_PLNK	prop_list;	/* property list */
    
     union bfnd_union {
	 
	 struct {
		  PTR_BFND  bf_ptr1;	/* used by the parser and should */
		  PTR_CMNT  cmnt_ptr;   /* to attach comments */

		  PTR_SYMB  symbol;	/* a symbol table entry */

		  PTR_LLND  ll_ptr1;	/* an L-value expr tree */
		  PTR_LLND  ll_ptr2;	/* an R-value expr tree */
		  PTR_LLND  ll_ptr3;	/* a spare expr tree (see below) */

		  PTR_LABEL lbl_ptr;    /* used by do */

		  PTR_BLOB  bl_ptr1;	/* a list of control dep subnodes */
		  PTR_BLOB  bl_ptr2;	/* another such list (for if stmt) */

		  PTR_DEP   dep_ptr1;	/* a list of dependences nodes */
		  PTR_DEP   dep_ptr2;	/* another list of dep nodes */

		  PTR_SETS  sets;	/* a list of sets like GEN, KILL etc */
		}				Template;	      

	 struct {
		  PTR_BFND  proc_list;	/* a list of procedures in this file */
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  list;	/* list of global const and type */

		  PTR_LLND  null_2;
		  PTR_LLND  null_3;
		  PTR_LLND  null_4;
		  
		  PTR_LABEL null_5;

		  PTR_BLOB  control;	/* used for list of procedures */
		  PTR_BLOB  null_6;

		  PTR_DEP   null_7;
		  PTR_DEP   null_8;
		  
		  PTR_SETS  null_9;
		}				Global;

	 struct {
		  PTR_BFND  next_prog;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  prog_symb;

		  PTR_LLND  null_1;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control;
		  PTR_BLOB  format_group;

		  PTR_DEP   null_5;
		  PTR_DEP   null_6;

		  PTR_SETS  null_7;
		}				program;

	 struct {
		  PTR_BFND  next_proc;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  proc_symb;

		  PTR_LLND  null_1;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control;
		  PTR_BLOB  format_group;

		  PTR_DEP   null_5;
		  PTR_DEP   null_6;
		  
		  PTR_SETS  null_7;
		}				procedure;

	 struct {
		  PTR_BFND  next_func;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  func_symb;

		  PTR_LLND  ftype;
		  PTR_LLND  null_1;
		  PTR_LLND  null_2;

		  PTR_LABEL null_3;

		  PTR_BLOB  control;
		  PTR_BLOB  format_group;

		  PTR_DEP   null_4;
		  PTR_DEP   null_5;

		  PTR_SETS  null_6;
		}				function;

	 struct {
		  PTR_BFND  next_bif;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  null_2;
		  PTR_LLND  null_3;
		  PTR_LLND  null_4;

		  PTR_LABEL null_5;

		  PTR_BLOB  control;
		  PTR_BLOB  null_6;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				basic_block;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  null_2;
		  PTR_LLND  null_3;
		  PTR_LLND  null_4;

		  PTR_LABEL null_5;

		  PTR_BLOB  null_6;
		  PTR_BLOB  null_7;

		  PTR_DEP   null_8;
		  PTR_DEP   null_9;

		  PTR_SETS  sets;
		}				control_end;

	 struct {
		  PTR_BFND  true_branch;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  condition;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control_true;
		  PTR_BLOB  control_false;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				if_node;

	 struct {
		  PTR_BFND  true_branch;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  condition;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control_true;
		  PTR_BLOB  control_false;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				where_node;

	 struct {
		  PTR_BFND  loop_end;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  null_2;
		  PTR_LLND  null_3;
		  PTR_LLND  null_4;

		  PTR_LABEL null_5;

		  PTR_BLOB  control;
		  PTR_BLOB  null_6;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				loop_node;

	 struct {
		  PTR_BFND  for_end;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  control_var;

		  PTR_LLND  range;
		  PTR_LLND  increment;
		  PTR_LLND  where_cond;

		  PTR_LABEL doend;

		  PTR_BLOB  control;
		  PTR_BLOB  null_1;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				for_node;

	 struct {
		  PTR_BFND  forall_end;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  control_var;

		  PTR_LLND  range;
		  PTR_LLND  increment;
		  PTR_LLND  where_cond;

		  PTR_LABEL null_1;

		  PTR_BLOB  control;
		  PTR_BLOB  null_2;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				forall_nd;

	 struct {
		  PTR_BFND  alldo_end;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  control_var;

		  PTR_LLND  range;
		  PTR_LLND  increment;
		  PTR_LLND  null_0;

		  PTR_LABEL null_1;

		  PTR_BLOB  control;
		  PTR_BLOB  null_2;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				alldo_nd;

	 struct {
		  PTR_BFND  while_end;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  condition;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control;
		  PTR_BLOB  null_5;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				while_node;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  condition;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  control_true;
		  PTR_BLOB  control_false;

		  PTR_DEP   null_5;
		  PTR_DEP   null_6;

		  PTR_SETS  sets;
		}				exit_node;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  l_value;
		  PTR_LLND  r_value;
		  PTR_LLND  null_2;

		  PTR_LABEL null_3;

		  PTR_BLOB  null_4;
		  PTR_BLOB  null_5;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				assign;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  l_value;
		  PTR_LLND  r_value;
		  PTR_LLND  null_2;

		  PTR_LABEL null_3;

		  PTR_BLOB  null_4;
		  PTR_BLOB  null_5;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				identify;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;

		  PTR_SYMB  null_1;

		  PTR_LLND  spec_string;
		  PTR_LLND  null_2;
		  PTR_LLND  null_3;

		  PTR_LABEL null_4;

		  PTR_BLOB  null_5;
		  PTR_BLOB  null_6;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				format;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  format;		/* used by blaze only */
		  PTR_LLND  expr_list;
		  PTR_LLND  control_list;	/* used by cedar fortan only */

		  PTR_LABEL null_2;

		  PTR_BLOB  null_3;
		  PTR_BLOB  null_4;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
		}				write_stat;

	 struct {
		  PTR_BFND  next_stat;
		  PTR_CMNT  cmnt_ptr;
		  
		  PTR_SYMB  null_1;

		  PTR_LLND  format;		/* used by blaze only */
		  PTR_LLND  var_list;
		  PTR_LLND  control_list;	/* used by cedar fortran */

		  PTR_LABEL null_2;

		  PTR_BLOB  null_3;
		  PTR_BLOB  null_4;

		  PTR_DEP   dep_from;
		  PTR_DEP   dep_to;

		  PTR_SETS  sets;
	     }					read_stat;
       } entry;
  };

#define __BIF_DEF__
