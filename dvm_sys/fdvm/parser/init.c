/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include "inc.h"
#include "defines.h"
#include "db.h"

extern int yylineno;
extern PTR_FILE fi;


/* In the following "set" means set the value to YES, or 1 */
int     debug;
int	errline;		/* line number of 1st error */
int     inioctl;		/* set if processing I/O control stmt */
int     needkwd;		/* set if stmt needs keyword */
int     optkwd;                 /* set if stmt needs optional keyword */
int     implkwd;                /* set if processing  IMPLICIT statement */
/*!!!*/
int     opt_kwd_;               /* set if stmt needs optional keyword (fdvm.gram)*/
int     as_op_kwd_;             /* set if stmt needs keyword ASSIGNMENT  or OPERATOR */
int     opt_kwd_hedr;           /* set if stmt needs after type specification */
int     opt_kwd_r;              /* set if stmt needs optional keyword followed by '(' (fdvm.gram)*/
int     optcorner;              /* set if stmt needs optional keyword CORNER(fdvm.gram)*/
int     optcall;                /* set if stmt needs optional "call" */
int     opt_l_s;                /* set if stmt needs optional "location"
                                 or "submachine" */
int     colon_flag;             /* set if stmt needs keyword followed by
                                   colon. */
int     is_openmp_stmt = 0;	/* set if OpenMP stmt*/ /*OMP*/
int operator_slash;             /*OMP*/

int     nioctl;
int     data_stat;
extern int     yydebug;
long    yystno = 0;
int     yyleng ;
char    yyquote ;
int     yylisting = 0;		/* set if listing to stdio required */
int     yylonglines = 0;	/* set if non-standard line length allowed */
int     prflag;
int     dorange;		/* current do range label */
int     undeftype = NO;		/* set if IMPLICIT NONE */
int     maxdim = 15;		/* max allowable number of dimensions */
int     nowarnflag = NO;	/* set if don't want warning messages */
int     shiftcase = YES;	/* convert variable to lower case */
int	num_files;
char    intonly;		/* set if only integer param allowed */
int     warn_all;               /* set if -w option specified*/
int     errcnt = 0;		/* error count */
int     nwarn = 0;		/* number of warning */
int	mod_offset;		/* line no offset of current module */

int     num_bfnds;		/* total # of bif nodes */
int     num_llnds;		/* total # of low level nodes */
int     num_symbs;		/* total # of symbol nodes */
int     num_types;		/* total # of types nodes */
int     num_blobs;		/* total # of blob nodes */
int     num_sets;		/* total # of set nodes */
int     num_cmnt;
int     num_def;		/* total # of dependncy nodes */
int	num_dep;
int     num_deflst;
int     num_label;		/* total # of label nodes */

PTR_LLND first_unresolved_call = LLNULL, last_unresolved_call = LLNULL;

PTR_SYMB global_list;

PTR_BFND head_bfnd,	/* start of bfnd chain */
	 cur_bfnd,	/* point to current bfnd */
         pred_bfnd,	/* used in finding the predecessor */
         last_bfnd;

PTR_LLND head_llnd, cur_llnd;
PTR_SYMB head_symb, cur_symb;
PTR_TYPE head_type, cur_type;
PTR_LABEL head_label, cur_label, thislabel;
PTR_BLOB head_blob, cur_blob;
PTR_SETS head_sets, cur_sets;
PTR_DEF head_def, cur_def;
PTR_FNAME head_file, cur_thread_file, the_file;
PTR_DEFLST head_deflst, cur_deflst;
PTR_DEP head_dep, cur_dep, pre_dep;
PTR_CMNT head_cmnt, cur_cmnt;
PTR_BFND global_bfnd;
PTR_SYMB star_symb;
PTR_TYPE vartype;
PTR_CMNT comments;
PTR_CMNT cur_comment;

PTR_TYPE make_type();
PTR_SYMB make_symb();
PTR_BFND get_bfnd();
PTR_BLOB make_blob();
void	 init_hash();
void     init_scope_table();

PTR_TYPE global_int, global_float, global_double, global_char, global_string;
PTR_TYPE global_bool, global_complex, global_dcomplex, global_gate,
         global_event, global_sequence,global_default,global_string_2;

char    yytext[1000];		/* text consumed for current lexeme */
char   *infname;

char    saveall;		/* set if "SAVE ALL" */
int    privateall;		/* set if "PRIVATE ALL" */
char    substars;		/* set if * parameters seen */
int     blklevel;		/* current block level */
int     parstate;	/* current parser's state */

PTR_TYPE impltype[26];		/* implicit type for 'a' to 'z' */

int     procno;			/* procedure # in current file */
int     proctype;		/* procedure type */
char   *procname;		/* procedure name */

int     procclass;
int     nentry;			/* # of entries */
char    multitype;

/*
 *  General initialization routine
 */
void 
initialize()
{
	errcnt = 0;		/* error count */
	nwarn = 0;		/* number of warning */
	parstate = OUTSIDE;	/* current parser's state */
	yylineno = 0;           /* global line no from lex */
	mod_offset = 0;
	num_files = 0;
	num_bfnds = 0;		/* total # of bif nodes */
	num_llnds = 0;		/* total # of low level nodes */
	num_symbs = 0;		/* total # of symbol nodes */
	num_types = 0;		/* total # of types nodes */
	num_blobs = 0;		/* total # of blob nodes */
	num_sets = 0;		/* total # of set nodes */
	num_cmnt = 0;
	num_def = 0;		/* total # of dependncy nodes */
	num_dep = 0;
	num_deflst = 0;
	num_label = 0;		/* total # of label nodes */
	global_list = (PTR_SYMB) NULL;
	head_bfnd = BFNULL;
	cur_bfnd = BFNULL;
	pred_bfnd = BFNULL;      /* used in finding the predecessor */
	last_bfnd = BFNULL;

	head_llnd = LLNULL;
	cur_llnd = LLNULL;
	head_symb = SMNULL;
	cur_symb = SMNULL;
	head_cmnt = CMNULL;
	cur_cmnt  = CMNULL;
	head_type = TYNULL;
	cur_type = TYNULL;
	head_label = LBNULL;
	cur_label = LBNULL;
	thislabel = LBNULL;
	head_blob = BLNULL;
	cur_blob = BLNULL;
	head_sets = (PTR_SETS)NULL;
	cur_sets = (PTR_SETS)NULL;
        head_file = (PTR_FNAME)NULL;
	head_def = (PTR_DEF)NULL;
	cur_def = (PTR_DEF)NULL;
	head_deflst = (PTR_DEFLST)NULL;
	cur_deflst = (PTR_DEFLST)NULL;
	head_dep = (PTR_DEP)NULL;
	cur_dep = (PTR_DEP)NULL;
	pre_dep = (PTR_DEP)NULL;
	global_bfnd = BFNULL;
	comments = CMNULL;
	cur_comment = CMNULL;
	global_int	= make_type(fi,T_INT);
	global_float	= make_type(fi,T_FLOAT);
	global_double	= make_type(fi,T_DOUBLE);
	global_char	= make_type(fi,T_CHAR);
	global_string	= make_type(fi,T_STRING);
	global_bool	= make_type(fi,T_BOOL);
	global_complex	= make_type(fi,T_COMPLEX);
	global_dcomplex	= make_type(fi,T_DCOMPLEX);
        global_gate     = make_type(fi,T_GATE);
        global_event    = make_type(fi,T_EVENT);
	global_sequence = make_type(fi,T_SEQUENCE);
	global_default	= make_type(fi,DEFAULT);
	global_string_2	= make_type(fi,T_STRING);
        global_string_2->entry.Template.dummy1=2;

	star_symb	= make_symb(fi,DEFAULT, "*");
	fi->global_bfnd = global_bfnd = get_bfnd(fi,GLOBAL, SMNULL, LLNULL, LLNULL, LLNULL);
	global_bfnd->filename=(PTR_FNAME)NULL;/*later we put something there*/
        cur_blob = global_bfnd->entry.Template.bl_ptr1
		 = make_blob (fi,BFNULL, BLNULL);
	pred_bfnd = global_bfnd;
	init_hash();
	init_scope_table();
}


/*
 * Set implicit type and length for each alphabetic chars in the range c1 - c2
 */
void
setimpl(type, c1, c2)
	PTR_TYPE type;
	int     c1, c2;
{
	register int i;
	char    buff[100];
	void err();

	if (c1 == 0 || c2 == 0)
		return;

	if (c1 > c2) {
	   (void)sprintf(buff, "characters out of order in implicit:%c-%c", c1, c2);
	   err(buff);
	} else {
		for (i = c1; i <= c2; ++i) {
			impltype[i - 'a'] = type;
		}
	}
}


/*
 *  Initialization routine for each new program unit
 */
void 
procinit()
{
	parstate = OUTSIDE;
	blklevel = 1;
	saveall = NO;
        privateall = 0;
	substars = NO;
	nwarn = 0;
	thislabel = LBNULL;
	needkwd = 0;
/*!!!*/
        opt_kwd_ = 0;
        opt_kwd_r = 0;
        optcorner = 0;

	++procno;
	proctype = T_UNKNOWN;
	procname = "MAIN_    ";
	nentry = 0;
	multitype = NO;
	blklevel = 1;
	dorange = 0;

	if (undeftype)
		 setimpl(global_default, 'a', 'z');
	else {
		 setimpl(global_float, 'a', 'z');
		 setimpl(global_int, 'i', 'n');
	}
}

