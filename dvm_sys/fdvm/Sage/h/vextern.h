/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/



/* Modified By Jenq-Kuen Lee Nov 20, 1987 */

extern int NoWarnings;  /* Used by newer code pC++2dep (phb) */
extern int nowarnflag;  /* Used by older obsolete code c2dep, f2dep */

/* The following variable used by verrors.c */
extern int yylineno;
extern char *infname;
extern int nwarn;
extern int errcnt;
extern int errline;
extern int  wait_first_include_name;
extern  char *first_line_name;

/* leave it out */
/*

extern char yytext[];


extern int yyleng;
extern int lineno;
extern int needkwd;
extern int inioctl;
extern int shiftcase;

extern int parstate;
extern int blklevel;

extern int procclass;
extern long procleng;
extern int nentry;
extern int blklevel;
extern int undeftype;
extern int dorange;
extern char intonly;
*/








extern int     num_bfnds;		/* total # of bif nodes */
extern int     num_llnds;		/* total # of low level nodes */
extern int     num_symbs;		/* total # of symbol nodes */
extern int     num_types;		/* total # of types nodes */
extern int     num_blobs;		/* total # of blob nodes */
extern int     num_sets;		/* total # of set nodes */
extern int     num_cmnt;
extern int     num_def;		/* total # of dependncy nodes */
extern int	num_dep;
extern int     num_deflst;
extern int     num_label;		/* total # of label nodes */
extern int     num_files;

extern int     cur_level;		/* current block level */
extern int     next_level;

extern char   *tag[610];

extern PTR_SYMB global_list;

extern PTR_BFND head_bfnd,	/* start of bfnd chain */
	        cur_bfnd,	/* poextern int to current bfnd */
                pred_bfnd,	/* used in finding the predecessor */
                last_bfnd;

extern PTR_LLND head_llnd, cur_llnd;

extern PTR_SYMB head_symb, cur_symb;

extern PTR_TYPE head_type, cur_type;

extern PTR_LABEL head_label, cur_label, thislabel;

extern PTR_FNAME head_file,cur_thread_file;

extern PTR_BLOB head_blob, cur_blob;

extern PTR_SETS head_sets, cur_sets;

extern PTR_DEF head_def, cur_def;

extern PTR_DEFLST head_deflst, cur_deflst;

extern PTR_DEP head_dep, cur_dep, pre_dep;

/*************************************************************************/
/* DECLARE is defined to be null (nothing) so that the variable is declared,
   or it is defined to be "extern". (phb) */

#ifndef DECLARE
#define DECLARE extern
#endif

DECLARE PTR_CMNT head_cmnt, cur_cmnt;
DECLARE PTR_BLOB global_blob ; 
DECLARE PTR_BFND global_bfnd;
DECLARE PTR_SYMB star_symb;
DECLARE PTR_TYPE vartype;
DECLARE PTR_CMNT comments;

#undef DECLARE
/*************************************************************************/

extern PTR_CMNT cur_comment;
/* struct Ctlframe *ctlsp = (struct Ctlframe *)NULL;  */

extern PTR_TYPE make_type();
extern PTR_SYMB make_symb();
extern PTR_BFND make_bfnd();
extern PTR_BFND make_bfndnt(); /* non-threaded ver. (lib/oldsrc/make_nodes.c */
extern PTR_BFND get_bfnd();
extern PTR_BLOB make_blob();
extern PTR_LLND make_llnd();
extern void	 init_hash();

extern PTR_TYPE global_int, global_float, global_double, global_char, global_string,global_void;
extern PTR_TYPE global_bool, global_complex, global_default, global_string_2;

extern char	*ckalloc();
extern char	*copyn(), *copys();

#define ALLOC(x)  (struct x *) ckalloc(sizeof(struct x))

#define INLOOP(x) ((LOOP_NODE <= x) && (x <= WHILE_NODE))
/* Used By pC++2dep  */
extern int ExternLangDecl;  /* PHB */
extern int mod_offset ;
extern int old_line ;
extern int branch_flag;
extern int main_type_flag ;
extern int primary_flag;
extern int function_flag ;
extern int friend_flag ;
extern int cur_flag ;
extern int exception_flag ;
extern PTR_SYMB first_symbol,right_symbol ;
extern PTR_BFND passed_bfnd;
extern PTR_BFND new_cur_bfnd ;
extern PTR_LLND new_cur_llnd ;
extern PTR_TYPE new_cur_type ;
extern PTR_SYMB new_cur_symb;
extern char     *new_cur_fname;
extern char     *line_pos_fname;
extern PTR_HASH cur_id_entry ;
extern PTR_CMNT new_cur_comment;
extern int      yydebug ;
extern int      TRACEON ;
extern int      declare_flag ;
extern int      not_fetch_yet ;  /* for comments */
extern int      recursive_yylex;   /* for comments */
extern int      line_pos_1 ;
extern PTR_FILE fi;
PTR_TYPE get_type();
PTR_LABEL get_label();
extern PTR_SYMB elementtype_symb;
