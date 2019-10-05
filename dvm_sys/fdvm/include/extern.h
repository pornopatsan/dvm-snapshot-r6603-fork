/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#include "db.h"

extern char *infname;
extern char yytext[];
extern int yyleng;
extern int errline;
extern int needkwd;
extern int implkwd;
extern int optkwd;
extern int opt_kwd_;
extern int as_op_kwd_;
extern int opt_kwd_hedr;
extern int opt_kwd_r;
extern int optcorner;
extern int optcall;
extern int opt_l_s;
extern int colon_flag;
extern int inioctl;
extern int shiftcase;
extern int nowarnflag;
extern int nwarn;
extern int errcnt;
extern int yylineno;
extern int mod_offset;
extern int num_files;

extern int parstate;
extern int procclass;
extern long procleng;
extern int nentry;
extern int blklevel;
extern int undeftype;
extern int dorange;
extern char intonly;
extern PTR_TYPE global_unknown;
extern PTR_FNAME head_file, cur_thread_file;
extern PTR_FILE fi;

char	*copyn(), *copys();

#define INLOOP(x) ((LOOP_NODE <= x) && (x <= WHILE_NODE))

#define NEW_SCOPE(x) ((x->variant == FORALL_NODE)|| \
		      (x->variant == CDOALL_NODE)|| \
		      (x->variant == SDOALL_NODE)|| \
		      (x->variant == DOACROSS_NODE)|| \
		      (x->variant == CDOACROSS_NODE)|| \
		      (x->variant == FOR_NODE))

extern int is_openmp_stmt; /*OMP*/
extern int operator_slash; /*OMP*/
#define BIT_OPENMP    1024*128*128     /* OpenMP Fortran */
