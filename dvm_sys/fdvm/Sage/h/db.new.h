/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/****************************************************************
 *								*
 *   db.h -- contains all definitions needed by the data base	*
 *	     management routines				*
 *								*
 ****************************************************************/


#ifndef CallSiteE

#ifndef FILE
#   include <stdio.h>
#endif

#ifndef DEP_DIR
#   include "defs.h"
#endif

#ifndef __BIF_DEF__
#   include "bif.h"
#endif

#ifndef __LL_DEF__
#   include "ll.h"
#endif

#ifndef __SYMB_DEF__
#   include "symb.h"
#endif

#ifndef MAX_LP_DEPTH
#   include "sets.h"
#endif


/*
 * Definitions for inquiring the information about variables
 */
#define Use	1	/* for inquiring USE info */
#define Mod	2	/* for inquiring MOD info */
#define UseMod	3	/* for inquiring both USE and MOD info */
#define Alias	4	/* for inquiring ALIAS information */


/*
 * Definitions for inquiring the information about procedures
 * This previous four definitions are shared here
 */
#define ProcDef    5	/* procedure's definition */
#define CallSite   6	/* list of the call sites of this procedure */
#define CallSiteE  7	/* the call sites extended with loop info */
#define ExternProc 8	/* list of external procedures references */

/*
 * Definitions for inquiring the information about files
 */
#define IncludeFile   1	/* list of files included by this file */
#define GlobalVarRef  2	/* list of global variables referenced */
#define ExternProcRef 3	/* list of external procedure referenced */


/*
 * Definitions for inquiring the information about project
 */
#define ProjFiles   1	/* get a list of .dep files make up the project */
#define ProjNames   2	/* list of all procedures in the project */
#define UnsolvRef   3	/* list of unsolved global references */
#define ProjGlobals 4	/* list of all global declarations */
#define ProjSrc	    5	/* list of source files (e.g. .h, .c and .f) */
/*
 * Definition for blobl tree
 */
#define IsLnk	0	/* this blob1 node is only a link */
#define IsObj	1	/* this blob1 node is a real object */


/*****************************
 * Some data structures used *
 ******************************/

typedef struct proj_obj *PTR_PROJ;
typedef struct file_obj *PTR_FILE;
typedef struct blob1	*PTR_BLOB1;
typedef struct obj_info	*PTR_INFO;


/*
 * structure for the whole project
 */
struct proj_obj {
	char	 *proj_name;	/* project filename */
	PTR_BLOB  file_chain;	/* list of all opened files in the project */
	PTR_BLOB *hash_tbl;	/* hash table of procedures declared */
	PTR_PROJ  next;		/* point to next project */
};


/*
 * Structure for each files in the project
 */
struct file_obj {
	char	 *filename;		/* filename of the .dep file */
	FILE	 *fid;			/* its file id */
	int	  lang;			/* type of language */
	PTR_HASH *hash_tbl;		/* hash table for this file obj */
	PTR_BFND  global_bfnd;		/* global BIF node for this file */
	PTR_BFND  head_bfnd,		/* head of BIF node for this file */
		  cur_bfnd;
	PTR_LLND  head_llnd,		/* head of low level node */
		  cur_llnd;
	PTR_SYMB  head_symb,		/* head of symbol node */
		  cur_symb;
	PTR_TYPE  head_type,		/* head of type node */
		  cur_type;
	PTR_BLOB  head_blob,		/* head of blob node */
		  cur_blob;
	PTR_DEP   head_dep,		/* head of dependence node */
		  cur_dep;
	PTR_LABEL head_lab,		/* head of label node */
		  cur_lab;
	PTR_CMNT  head_cmnt,		/* head of comment node */
		  cur_cmnt;
	PTR_FNAME head_file;
	int	  num_blobs,		/* no. of blob nodes */
		  num_bfnds,		/* no. of bif nodes */
		  num_llnds,		/* no. of ll nodes */
		  num_symbs,		/* no. of symb nodes */
		  num_label,		/* no. of label nodes */
		  num_types,		/* no. of type nodes */
		  num_files,		/* no. of filename nodes */
		  num_dep,		/* no. of dependence nodes */
		  num_cmnt;		/* no. of comment nodes */
};


/*
 * A cons obj structure
 */
struct blob1{
	char	   tag;	/* type of this blob node */
	char	  *ref;	/* pointer to the objects of interest */
	PTR_BLOB1  next;/* point to next cons obj */
};


/*
 * Structure for information objects
 */
struct obj_info {
	char	*filename;	/* filename of the reference */
	int	 g_line;	/* absolute line number in the file */
	int	 l_line;	/* relative line number to the object */
	char	*source;	/* source line */
};


/*
 * Structure for property list
 */
struct prop_link {
	char	*prop_name;	/* property name */
	char	*prop_val;	/* property value */
	PTR_PLNK next;		/* point to the next property list */
};

/*
 * declaration of data base routines
 */
typedef char *(*PCF)();

extern PCF UnparseBfnd[];
extern PCF UnparseLlnd[];
extern PCF UnparseSymb[];
extern PCF UnparseType[];

PTR_PROJ  OpenProj();
PTR_BLOB1 GetProjInfo();
PTR_BLOB1 GetProcInfo();
PTR_BLOB1 GetTypeInfo();
PTR_BLOB1 GetTypeDef ();
PTR_BLOB1 GetVarInfo ();
PTR_BLOB1 GetDepInfo ();

#endif CallSiteE
