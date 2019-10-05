/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include "tag"

#define hashMax		1007   /*max hash table size */

/**************** variant tags for dependence nodes *********************/

#define DEP_DIR         0200	/* direction vector information only */
#define DEP_DIST        0000	/* direction and distance vector */

#define NO_ALL_ST_DEP   0010    /* no all statiionary dir for this pair of statements */
#define DEP_CROSS       0100	/* dependence MUST wrap around loop */
#define DEP_UNCROSS     0000	/* dependence MAY not wrap around loop */

#define DEP_FLOW        0
#define DEP_ANTI        1
#define DEP_OUTPUT      2

/************************************************************************/

typedef struct bfnd	*PTR_BFND;
typedef struct llnd	*PTR_LLND;
typedef struct blob	*PTR_BLOB;
//typedef struct string	*PTR_STRING;
typedef struct symb	*PTR_SYMB;
typedef struct hash_entry *PTR_HASH;
typedef struct data_type  *PTR_TYPE;
typedef struct dep	*PTR_DEP;
typedef struct sets	*PTR_SETS;
typedef struct def	*PTR_DEF;
typedef struct deflst	*PTR_DEFLST;
typedef struct Label	*PTR_LABEL;
typedef struct cmnt     *PTR_CMNT;
typedef struct file_name *PTR_FNAME;
typedef struct prop_link *PTR_PLNK;

struct blob {
	PTR_BFND  ref;
	PTR_BLOB  next;
}; 


struct Label {
	int		id;		/* identification tag */
	PTR_BFND 	scope;		/* level at which ident is declared */
	PTR_BLOB	ud_chain;	/* use-definition chain */
	unsigned	labused :1;	/* if it's been referenced */
	unsigned	labinacc:1;	/* illegal use of this label */
	unsigned	labdefined:1;	/* if this label been defined */
	unsigned	labtype:2;	/* UNKNOWN, EXEC, FORMAT, and OTHER */
	long		stateno;	/* statement label */
	PTR_LABEL	next;		/* point to next label entry */
	PTR_BFND	statbody;	/* point to body of statement */
	PTR_SYMB	label_name;	/* label name for VPC++       */
					/* The variant will be LABEL_NAME */
};


struct Ctlframe	{
	int		ctltype;	/* type of control frame */
	int		level;		/* block level */
	int		dolabel;	/* DO loop's end label */
	PTR_SYMB	donamep;	/* DO loop's control variable name */
	PTR_SYMB	block_list;	/* start of local decl */
	PTR_SYMB	block_end;	/* end of local decl */
	PTR_BFND	loop_hedr;	/* save the current loop header */
	PTR_BFND	header;		/* header of the block */
	PTR_BFND	topif;		/* keep track of if header */
	struct Ctlframe *next;	 	/* thread */
};

struct cmnt {
	int id;
	int type;
        int counter;                     /* New Added for VPC++ */
	char* string;
	struct cmnt *next;
	struct cmnt *thread;
};


struct file_name {		/* for keep source filenames in the project */
	int id;
	char *name;
	PTR_FNAME next;
};


#define NO	   0
#define YES	   1
#ifndef FALSE
#  define FALSE	   0
#endif
#ifndef TRUE
#  define TRUE	   1
#endif
#define BOOL	 int
#define EOL 	  -1
#define SAME_GROUP 0
#define NEW_GROUP1 1
#define NEW_GROUP2 2
#define FULL       0
#define HALF       1

#define DEFINITE	1
#define DEFINITE_SAME	7
#define DEFINITE_DIFFER	0
#define FIRST_LARGER	2
#define SECOND_LARGER	4


/*
 * Tags for various languages
 */
#define ForSrc	0	/* This is a Fortran program */
#define CSrc	1	/* This is a C program */
#define BlaSrc	2	/* This is a Blaze program */


#define BFNULL		(PTR_BFND) 0
#define LLNULL		(PTR_LLND) 0
#define BLNULL		(PTR_BLOB) 0
#define SMNULL		(PTR_SYMB) 0
#define HSNULL		(PTR_HASH) 0
#define TYNULL		(PTR_TYPE) 0
#define LBNULL		(PTR_LABEL)0
#define CMNULL          (PTR_CMNT)0
