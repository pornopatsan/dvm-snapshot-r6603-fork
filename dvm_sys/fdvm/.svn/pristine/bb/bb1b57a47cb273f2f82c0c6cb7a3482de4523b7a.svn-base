/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


# define    MAX_LP_DEPTH       10
# define    MAX_DEP	11
 
struct ref { /* reference of a variable */
    PTR_BFND  stmt;  /* statement containing reference */
    PTR_LLND  refer; /* pointer to the actual reference */
    } ;
 
struct refl {
    PTR_SYMB id;
    struct ref * node;
    struct refl * next;
 };
 
typedef struct refl * PTR_REFL;

/* Added by Mannho from here */

struct aref {
   PTR_SYMB id;
   PTR_LLND decl_ranges;
   PTR_LLND use_bnd0; /* undecidable list because index with variables */
   PTR_LLND mod_bnd0;
   PTR_LLND use_bnd1; /* decidable with induction variables */
   PTR_LLND mod_bnd1;
   PTR_LLND use_bnd2; /* decidable with only constants */
   PTR_LLND mod_bnd2;
   struct aref *next;
};

typedef struct aref *PTR_AREF;

/* Added by Mannho to here */

struct sets {
	PTR_REFL gen;	 /* local attribute */
	PTR_REFL in_def; /* inhereted attrib */
	PTR_REFL use;	/* local attribute */
	PTR_REFL in_use; /* inherited attrib */
	PTR_REFL out_def; /* synth. attrib  */
	PTR_REFL out_use; /* synth. attrib  */
	PTR_AREF arefl;   /* array reference */
	};
 
 
struct dep { /* data dependencies */
 
    int id;	/* identification for reading/writing */
    PTR_DEP thread;
 
    char     type;	 /* flow-, output-, or anti-dependence */
    char     direct[MAX_DEP]; /* direction/distance vector */
 
    PTR_SYMB   symbol; /* symbol table entry */
    struct ref from;   /* tail of dependence */
    struct ref to;     /* head of dependence */
 
    PTR_DEP  from_fwd, from_back; /* list of dependencies going to tail */
    PTR_DEP  to_fwd, to_back;	  /* list of dependencies going to head */
 
    } ;
 
#define AR_DIM_MAX 5
#define MAX_NEST_DEPTH 10
 
struct subscript{
	int decidable;	/* if 1 then analysis is ok. if 2 then vector range */
			/* if it is 0 it is not analizable.		    */
	PTR_LLND parm_exp; /* this is a symbolic expression involving	    */
			   /* procedure parameters or common variables.     */
	int offset;	   /* This is the constant term in a linear form    */
	PTR_LLND vector;   /* pointer to ddot for vector range		    */
	int coefs[MAX_NEST_DEPTH];  /* if coef[2] = 3 then the second */
				    /* level nesting induction var has*/
				    /* coef 3 in this position.       */
	PTR_LLND coefs_symb[MAX_NEST_DEPTH];
			   /* if coefs[2] is not null then this is the*/
			   /* pointer to a symbolic coef. in terms of */
			   /* procedure parameters, globals or commons*/
	};
