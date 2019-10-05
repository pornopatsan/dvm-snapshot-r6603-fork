/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/* VPC Version modified by Jenq-Kuen Lee Nov 15 , 1987 */
/* Original Filename : symb.h			       */
/* New filename	     : vsymb.h			       */

/************************************************************************
 *									*
 *		      hash and symbol table entries			*
 *									*
 ************************************************************************/


struct hash_entry
   {
    char *ident;
    struct hash_entry *next_entry;
    PTR_SYMB id_attr;
   };

struct symb {
    int		variant;
    int		id;
    char	*ident;
    struct hash_entry *parent;
    PTR_SYMB	outer;		/* pointer to symbol in enclosing block */
    PTR_SYMB	next_symb;	/* pointer to next symbol in same block */
    PTR_SYMB	id_list;	/* used for making lists of ids */
    PTR_SYMB	thread;		/* list of all allocated symbol pointers */
    PTR_TYPE	type;		/* data type of this identifier */
    PTR_BFND	scope;		/* level at which ident is declared */
    PTR_BLOB	ud_chain;	/* use_definition chain */
    int		attr;		/* attributes of the variable */
    int		dovar;		/* set if used as loop's control variable */
    int		decl;		/* field that the parser use in keeping track
				   of declarations */

    union symb_union {
	PTR_LLND  const_value;	/* for constants */

	struct {		/* for enum-field and record field */
		int tag;
		int offset;    
		PTR_SYMB declared_name ;   /* used for friend construct */
		PTR_SYMB next;
		PTR_SYMB  base_name; /* name of record or enumerated type */
		PTR_LLND  restricted_bit ;  /* Used by VPC++ for restricted bit number */
	} field;

	struct {		/* for variant fields */
		int tag;
		int offset;
		PTR_SYMB next;
		PTR_SYMB base_name;
		PTR_LLND variant_list;
	} variant_field;

	
	struct {		/* for program */
		PTR_SYMB   symb_list;
		PTR_LABEL  label_list;
		PTR_BFND   prog_hedr;
	} prog_decl;

	struct {		/* for PROC */
		int	   seen;
		int	   num_input,  num_output,  num_io;
		PTR_SYMB   in_list;
		PTR_SYMB   out_list;
		PTR_SYMB   symb_list;
		int	   local_size;
		PTR_LABEL  label_list;
		PTR_BFND   proc_hedr;
		PTR_LLND   call_list;
	} proc_decl;

	struct {		/* for FUNC */
		int	   seen;
		int	   num_input,  num_output,  num_io;
		PTR_SYMB   in_list;
		PTR_SYMB   out_list;
		PTR_SYMB   symb_list;
		int	   local_size;
		PTR_LABEL  label_list;
		PTR_BFND   func_hedr;
		PTR_LLND   call_list;
	} func_decl;

	struct {		/* for variable declaration */
		 int	   local;    /* local or input or output or both param*/
	         int	   num1,  num2, num3 ; /*24.02.03*/
		 PTR_SYMB  next_out; /* for list of output parameters*//*perestanovka c next_out *24.02.03*/
		 PTR_SYMB  next_in;  /* for list of input parameters*/
		 int	   offset;
		 int	   dovar;    /* set if being used as DO control var */
	} var_decl;
       
	struct {
		int	   seen ;
		int	   num_input,  num_output, num_io ;
		PTR_SYMB   in_list ;
		PTR_SYMB   out_list ;
		PTR_SYMB   symb_list;
		int	   local_size;
		PTR_LABEL  label_list ;
		PTR_BFND   func_hedr ;
		PTR_LLND   call_list ;
				      /* the following information for field */
		int	   tag ;
		int	   offset ;
		PTR_SYMB   declared_name; /* used for friend construct */
		PTR_SYMB   next ;
		PTR_SYMB   base_name ;
				      /* the following is newly added */

	} member_func	    ;	      /* New  one for VPC */


	/* an attempt to unify the data structure   */
	struct {
		int	   seen ;
		int	   num_input,  num_output, num_io ;
		PTR_SYMB   in_list ;
		PTR_SYMB   out_list ;
		PTR_SYMB   symb_list;
		int	   local_size;
		PTR_LABEL  label_list ;
		PTR_BFND   func_hedr ;
		PTR_LLND   call_list ;
				      /* the following information for field */
		int	   tag ;
		int	   offset ;
		PTR_SYMB   declared_name; /* used for friend construct */
		PTR_SYMB   next ;
		PTR_SYMB   base_name ;

				      /* the following is newly added */
	} Template	;	      /* New  one for VPC */

    } entry;
};

struct data_type {
    int variant;
    int id;
    int length;
    PTR_TYPE	thread;		/* list of all allocated symbol pointers */
    PTR_SYMB	name;		/* type name */
    PTR_BLOB	ud_chain;	/* use_definition chain */
    union type_union {
  /* no entry needed for T_INT, T_CHAR, T_FLOAT, T_DOUBLE, T_VOID T_BOOL */



	struct {		/* for T_SUBRANGE */
	    PTR_TYPE base_type;	/* = to T_INT, T_CHAR, T_FLOAT */
	    PTR_LLND lower, upper;
	} subrange;

	struct {		/* for T_ARRAY */
	    PTR_TYPE base_type; /* New order   */
	    int num_dimensions;
	    PTR_LLND ranges;
	} ar_decl;

	struct {
	    PTR_TYPE base_type ;
	    int	 dummy1;
	    PTR_LLND ranges    ;
	    PTR_LLND kind_len  ;
	    int	 dummy3;
	    int	 dummy4;
	    int	 dummy5;
	}  Template ;		/* for T_DESCRIPT,T_ARRAY,T_FUNCTION,T_POINTER */
	PTR_TYPE base_type;	/* for T_LIST */

	struct {		/* for T_RECORD or T_ENUM */
	    int num_fields;
	    int record_size;
	    PTR_SYMB first;
	} re_decl;
				/* the following is added fro VPC */
       
         struct {
            PTR_SYMB symbol;
            PTR_SYMB scope_symbol;
        } derived_type ; /* for type name deriving type */

	struct {		/* for class T_CLASS T_UNION T_STRUCT */
	    int num_fields;
	    int record_size;
	    PTR_SYMB first;
	    PTR_BFND original_class ;
	    PTR_TYPE base_type;	/* base type or inherited collection */
	} derived_class ;

	struct {		/* for class T_DERIVED_TEMPLATE   */
	    PTR_SYMB templ_name;
	    PTR_LLND args;	/* argument list for templ */
	} templ_decl ;

			        /* for T_MEMBER_POINTER and */
	struct {		/* for class T_DERIVED_COLLECTION   */
	    PTR_SYMB collection_name;
	    PTR_TYPE base_type;	/* base type or inherited collection */
	} col_decl ;

	struct {		/* for T_DESCRIPT */
	    PTR_TYPE  base_type ;
	    int signed_flag ;
	    PTR_LLND ranges    ;
	    int long_short_flag ;
	    int mod_flag ;
	    int storage_flag;
	    int access_flag;
	 } descriptive ;       
	    
    } entry;
}; 


#define __SYMB_DEF__
