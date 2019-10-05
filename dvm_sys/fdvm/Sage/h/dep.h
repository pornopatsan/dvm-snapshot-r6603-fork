/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/************************************************************************/
/*									*/
/*			    DEPENDENCE NODES				*/
/*									*/
/************************************************************************/

# define    MAX_LP_DEPTH       10
# define    MAX_DEP     (MAX_LP_DEPTH+1)

struct ref { /* reference of a variable */
    PTR_BFND  stmt;  /* statement containing reference */
    PTR_LLND  refer; /* pointer to the actual reference */
    } ;


struct dep { /* data dependencies */

    int id;	/* identification for reading/writing */
    PTR_DEP thread;

    char     type;		  /* flow-, output-, or anti-dependence */
    char     direct[MAX_DEP];	  /* direction/distance vector */

    PTR_SYMB   symbol;		  /* symbol table entry */
    struct ref from;		  /* tail of dependence */
    struct ref to;		  /* head of dependence */
    PTR_BFND from_hook, to_hook;  /* bifs where dep is hooked in */

    PTR_DEP  from_fwd, from_back; /* list of dependencies going to tail */
    PTR_DEP  to_fwd, to_back;     /* list of dependencies going to head */

    } ;


