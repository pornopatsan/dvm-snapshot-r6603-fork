
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 340 "gram1.y"

#include <string.h>
#include "inc.h"
#include "extern.h"
#include "defines.h"
#include "fdvm.h"
#include "fm.h"

/* We may use builtin alloca */
#include "compatible.h"
#ifdef _NEEDALLOCAH_
#  include <alloca.h>
#endif

#define EXTEND_NODE 2  /* move the definition to h/ files. */

extern PTR_BFND global_bfnd, pred_bfnd;
extern PTR_SYMB star_symb;
extern PTR_SYMB global_list;
extern PTR_TYPE global_bool;
extern PTR_TYPE global_int;
extern PTR_TYPE global_float;
extern PTR_TYPE global_double;
extern PTR_TYPE global_char;
extern PTR_TYPE global_string;
extern PTR_TYPE global_string_2;
extern PTR_TYPE global_complex;
extern PTR_TYPE global_dcomplex;
extern PTR_TYPE global_gate;
extern PTR_TYPE global_event;
extern PTR_TYPE global_sequence;
extern PTR_TYPE global_default;
extern PTR_LABEL thislabel;
extern PTR_CMNT comments, cur_comment;
extern PTR_BFND last_bfnd;
extern PTR_TYPE impltype[];
extern int nioctl;
extern int maxdim;
extern long yystno;	/* statement label */
extern char stmtbuf[];	/* input buffer */
extern char *commentbuf;	/* comments buffer from scanner */
extern PTR_BLOB head_blob;
extern PTR_BLOB cur_blob;
extern PTR_TYPE vartype; /* variable type */
extern int end_group;
extern char saveall;
extern int privateall;
extern int needkwd;
extern int implkwd;
extern int opt_kwd_hedr;
/* added for FORTRAN 90 */
extern PTR_LLND first_unresolved_call;
extern PTR_LLND last_unresolved_call;
extern int data_stat;
extern char yyquote;

extern int warn_all;
extern int statement_kind; /* kind of statement: 1 - HPF-DVM-directive, 0 - Fortran statement*/ 
int extend_flag = 0;

static int do_name_err;
static int ndim;	/* number of dimension */
/*!!! hpf */
static int explicit_shape; /*  1 if shape specification is explicit */
/* static int varleng;*/	/* variable size */
static int lastwasbranch = NO;	/* set if last stmt was a branch stmt */
static int thiswasbranch = NO;	/* set if this stmt is a branch stmt */
static PTR_SYMB type_var = SMNULL;
static PTR_LLND stat_alloc = LLNULL; /* set if ALLOCATE/DEALLOCATE stmt has STAT-clause*/
/* static int subscripts_status = 0; */
static int type_options,type_opt;   /* The various options used to declare a name -
                                      RECURSIVE, POINTER, OPTIONAL etc.         */
static PTR_BFND module_scope;
static int position = IN_OUTSIDE;            
static int attr_ndim;           /* number of dimensions in DIMENSION (array_spec)
                                   attribute declaration */
static PTR_LLND attr_dims;     /* low level representation of array_spec in
                                   DIMENSION (array_spec) attribute declarartion. */
static int in_vec = NO;	      /* set if processing array constructor */


/* Line 189 of yacc.c  */
#line 155 "gram1.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PERCENT = 1,
     AMPERSAND = 2,
     ASTER = 3,
     CLUSTER = 4,
     COLON = 5,
     COMMA = 6,
     DASTER = 7,
     DEFINED_OPERATOR = 8,
     DOT = 9,
     DQUOTE = 10,
     GLOBAL_A = 11,
     LEFTAB = 12,
     LEFTPAR = 13,
     MINUS = 14,
     PLUS = 15,
     POINT_TO = 16,
     QUOTE = 17,
     RIGHTAB = 18,
     RIGHTPAR = 19,
     AND = 20,
     DSLASH = 21,
     EQV = 22,
     EQ = 23,
     EQUAL = 24,
     FFALSE = 25,
     GE = 26,
     GT = 27,
     LE = 28,
     LT = 29,
     NE = 30,
     NEQV = 31,
     NOT = 32,
     OR = 33,
     TTRUE = 34,
     SLASH = 35,
     XOR = 36,
     REFERENCE = 37,
     AT = 38,
     ACROSS = 39,
     ALIGN_WITH = 40,
     ALIGN = 41,
     ALLOCATABLE = 42,
     ALLOCATE = 43,
     ARITHIF = 44,
     ASSIGNMENT = 45,
     ASSIGN = 46,
     ASSIGNGOTO = 47,
     ASYNCHRONOUS = 48,
     ASYNCID = 49,
     ASYNCWAIT = 50,
     BACKSPACE = 51,
     BAD_CCONST = 52,
     BAD_SYMBOL = 53,
     BARRIER = 54,
     BLOCKDATA = 55,
     BLOCK = 56,
     BOZ_CONSTANT = 57,
     BYTE = 58,
     CALL = 59,
     CASE = 60,
     CHARACTER = 61,
     CHAR_CONSTANT = 62,
     CHECK = 63,
     CLOSE = 64,
     COMMON = 65,
     COMPLEX = 66,
     COMPGOTO = 67,
     CONSISTENT_GROUP = 68,
     CONSISTENT_SPEC = 69,
     CONSISTENT_START = 70,
     CONSISTENT_WAIT = 71,
     CONSISTENT = 72,
     CONSTRUCT_ID = 73,
     CONTAINS = 74,
     CONTINUE = 75,
     CORNER = 76,
     CYCLE = 77,
     DATA = 78,
     DEALLOCATE = 79,
     HPF_TEMPLATE = 80,
     DEBUG = 81,
     DEFAULT_CASE = 82,
     DEFINE = 83,
     DERIVED = 84,
     DIMENSION = 85,
     DISTRIBUTE = 86,
     DOWHILE = 87,
     DOUBLEPRECISION = 88,
     DOUBLECOMPLEX = 89,
     DP_CONSTANT = 90,
     DVM_POINTER = 91,
     DYNAMIC = 92,
     ELEMENTAL = 93,
     ELSE = 94,
     ELSEIF = 95,
     ELSEWHERE = 96,
     ENDASYNCHRONOUS = 97,
     ENDDEBUG = 98,
     ENDINTERVAL = 99,
     ENDUNIT = 100,
     ENDDO = 101,
     ENDFILE = 102,
     ENDFORALL = 103,
     ENDIF = 104,
     ENDINTERFACE = 105,
     ENDMODULE = 106,
     ENDON = 107,
     ENDSELECT = 108,
     ENDTASK_REGION = 109,
     ENDTYPE = 110,
     ENDWHERE = 111,
     ENTRY = 112,
     EXIT = 113,
     EOLN = 114,
     EQUIVALENCE = 115,
     ERROR = 116,
     EXTERNAL = 117,
     F90 = 118,
     FIND = 119,
     FORALL = 120,
     FORMAT = 121,
     FUNCTION = 122,
     GATE = 123,
     GEN_BLOCK = 124,
     HEAP = 125,
     HIGH = 126,
     IDENTIFIER = 127,
     IMPLICIT = 128,
     IMPLICITNONE = 129,
     INCLUDE_TO = 130,
     INCLUDE = 131,
     INDEPENDENT = 132,
     INDIRECT_ACCESS = 133,
     INDIRECT_GROUP = 134,
     INDIRECT = 135,
     INHERIT = 136,
     INQUIRE = 137,
     INTERFACEASSIGNMENT = 138,
     INTERFACEOPERATOR = 139,
     INTERFACE = 140,
     INTRINSIC = 141,
     INTEGER = 142,
     INTENT = 143,
     INTERVAL = 144,
     INOUT = 145,
     IN = 146,
     INT_CONSTANT = 147,
     LABEL = 148,
     LABEL_DECLARE = 149,
     LET = 150,
     LOCALIZE = 151,
     LOGICAL = 152,
     LOGICALIF = 153,
     LOOP = 154,
     LOW = 155,
     MAXLOC = 156,
     MAX = 157,
     MAP = 158,
     MINLOC = 159,
     MIN = 160,
     MODULE_PROCEDURE = 161,
     MODULE = 162,
     MULT_BLOCK = 163,
     NAMEEQ = 164,
     NAMELIST = 165,
     NEW_VALUE = 166,
     NEW = 167,
     NULLIFY = 168,
     OCTAL_CONSTANT = 169,
     ONLY = 170,
     ON = 171,
     ON_DIR = 172,
     ONTO = 173,
     OPEN = 174,
     OPERATOR = 175,
     OPTIONAL = 176,
     OTHERWISE = 177,
     OUT = 178,
     OWN = 179,
     PARALLEL = 180,
     PARAMETER = 181,
     PAUSE = 182,
     PLAINDO = 183,
     PLAINGOTO = 184,
     POINTER = 185,
     POINTERLET = 186,
     PREFETCH = 187,
     PRINT = 188,
     PRIVATE = 189,
     PRODUCT = 190,
     PROGRAM = 191,
     PUBLIC = 192,
     PURE = 193,
     RANGE = 194,
     READ = 195,
     REALIGN_WITH = 196,
     REALIGN = 197,
     REAL = 198,
     REAL_CONSTANT = 199,
     RECURSIVE = 200,
     REDISTRIBUTE_NEW = 201,
     REDISTRIBUTE = 202,
     REDUCTION_GROUP = 203,
     REDUCTION_START = 204,
     REDUCTION_WAIT = 205,
     REDUCTION = 206,
     REMOTE_ACCESS_SPEC = 207,
     REMOTE_ACCESS = 208,
     REMOTE_GROUP = 209,
     RESET = 210,
     RESULT = 211,
     RETURN = 212,
     REWIND = 213,
     SAVE = 214,
     SECTION = 215,
     SELECT = 216,
     SEQUENCE = 217,
     SHADOW_ADD = 218,
     SHADOW_COMPUTE = 219,
     SHADOW_GROUP = 220,
     SHADOW_RENEW = 221,
     SHADOW_START_SPEC = 222,
     SHADOW_START = 223,
     SHADOW_WAIT_SPEC = 224,
     SHADOW_WAIT = 225,
     SHADOW = 226,
     STAGE = 227,
     STATIC = 228,
     STAT = 229,
     STOP = 230,
     SUBROUTINE = 231,
     SUM = 232,
     SYNC = 233,
     TARGET = 234,
     TASK = 235,
     TASK_REGION = 236,
     THEN = 237,
     TO = 238,
     TRACEON = 239,
     TRACEOFF = 240,
     TRUNC = 241,
     TYPE = 242,
     TYPE_DECL = 243,
     UNDER = 244,
     UNKNOWN = 245,
     USE = 246,
     VIRTUAL = 247,
     VARIABLE = 248,
     WAIT = 249,
     WHERE = 250,
     WHERE_ASSIGN = 251,
     WHILE = 252,
     WITH = 253,
     WRITE = 254,
     COMMENT = 255,
     WGT_BLOCK = 256,
     HPF_PROCESSORS = 257,
     IOSTAT = 258,
     ERR = 259,
     END = 260,
     OMPDVM_ATOMIC = 261,
     OMPDVM_BARRIER = 262,
     OMPDVM_COPYIN = 263,
     OMPDVM_COPYPRIVATE = 264,
     OMPDVM_CRITICAL = 265,
     OMPDVM_ONETHREAD = 266,
     OMPDVM_DO = 267,
     OMPDVM_DYNAMIC = 268,
     OMPDVM_ENDCRITICAL = 269,
     OMPDVM_ENDDO = 270,
     OMPDVM_ENDMASTER = 271,
     OMPDVM_ENDORDERED = 272,
     OMPDVM_ENDPARALLEL = 273,
     OMPDVM_ENDPARALLELDO = 274,
     OMPDVM_ENDPARALLELSECTIONS = 275,
     OMPDVM_ENDPARALLELWORKSHARE = 276,
     OMPDVM_ENDSECTIONS = 277,
     OMPDVM_ENDSINGLE = 278,
     OMPDVM_ENDWORKSHARE = 279,
     OMPDVM_FIRSTPRIVATE = 280,
     OMPDVM_FLUSH = 281,
     OMPDVM_GUIDED = 282,
     OMPDVM_LASTPRIVATE = 283,
     OMPDVM_MASTER = 284,
     OMPDVM_NOWAIT = 285,
     OMPDVM_NONE = 286,
     OMPDVM_NUM_THREADS = 287,
     OMPDVM_ORDERED = 288,
     OMPDVM_PARALLEL = 289,
     OMPDVM_PARALLELDO = 290,
     OMPDVM_PARALLELSECTIONS = 291,
     OMPDVM_PARALLELWORKSHARE = 292,
     OMPDVM_RUNTIME = 293,
     OMPDVM_SECTION = 294,
     OMPDVM_SECTIONS = 295,
     OMPDVM_SCHEDULE = 296,
     OMPDVM_SHARED = 297,
     OMPDVM_SINGLE = 298,
     OMPDVM_THREADPRIVATE = 299,
     OMPDVM_WORKSHARE = 300,
     OMPDVM_NODES = 301,
     OMPDVM_IF = 302,
     IAND = 303,
     IEOR = 304,
     IOR = 305,
     ACC_REGION = 306,
     ACC_END_REGION = 307,
     ACC_CHECKSECTION = 308,
     ACC_END_CHECKSECTION = 309,
     ACC_GET_ACTUAL = 310,
     ACC_ACTUAL = 311,
     ACC_TARGETS = 312,
     ACC_ASYNC = 313,
     ACC_HOST = 314,
     ACC_CUDA = 315,
     ACC_LOCAL = 316,
     ACC_INLOCAL = 317,
     ACC_CUDA_BLOCK = 318,
     BY = 319,
     IO_MODE = 320,
     CP_CREATE = 321,
     CP_LOAD = 322,
     CP_SAVE = 323,
     CP_WAIT = 324,
     FILES = 325,
     VARLIST = 326,
     STATUS = 327,
     EXITINTERVAL = 328,
     TEMPLATE_CREATE = 329,
     TEMPLATE_DELETE = 330,
     SPF_ANALYSIS = 331,
     SPF_PARALLEL = 332,
     SPF_TRANSFORM = 333,
     SPF_NOINLINE = 334,
     SPF_PARALLEL_REG = 335,
     SPF_END_PARALLEL_REG = 336,
     SPF_PRIVATES_EXPANSION = 337,
     SPF_FISSION = 338,
     BINARY_OP = 341,
     UNARY_OP = 342
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 421 "gram1.y"

    int token;
    char charv;
    char *charp;
    PTR_BFND bf_node;
    PTR_LLND ll_node;
    PTR_SYMB symbol;
    PTR_TYPE data_type;
    PTR_HASH hash_entry;
    PTR_LABEL label;



/* Line 214 of yacc.c  */
#line 547 "gram1.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 625 "gram1.y"

void add_scope_level();
void delete_beyond_scope_level();
PTR_HASH look_up_sym();
PTR_HASH just_look_up_sym();
PTR_HASH just_look_up_sym_in_scope();
PTR_HASH look_up_op();
PTR_SYMB make_constant();
PTR_SYMB make_scalar();
PTR_SYMB make_array();
PTR_SYMB make_pointer();
PTR_SYMB make_function();
PTR_SYMB make_external();
PTR_SYMB make_intrinsic();
PTR_SYMB make_procedure();
PTR_SYMB make_process();
PTR_SYMB make_program();
PTR_SYMB make_module();
PTR_SYMB make_common();
PTR_SYMB make_parallel_region();
PTR_SYMB make_derived_type();
PTR_SYMB make_local_entity();
PTR_SYMB make_global_entity();
PTR_TYPE make_type_node();
PTR_TYPE lookup_type(), make_type();
void     process_type();
void     process_interface();
void     bind();
void     late_bind_if_needed();
PTR_SYMB component();
PTR_SYMB lookup_type_symbol();
PTR_SYMB resolve_overloading();
PTR_BFND cur_scope();
PTR_BFND subroutine_call();
PTR_BFND process_call();
PTR_LLND deal_with_options();
PTR_LLND intrinsic_op_node();
PTR_LLND defined_op_node();
int is_substring_ref();
int is_array_section_ref();
PTR_LLND dim_expr(); 
PTR_BFND exit_stat();
PTR_BFND make_do();
PTR_BFND make_pardo();
PTR_BFND make_enddoall();
PTR_TYPE install_array(); 
PTR_SYMB install_entry(); 
void install_param_list();
PTR_LLND construct_entry_list();
void copy_sym_data();
PTR_LLND check_and_install();
PTR_HASH look_up();
PTR_BFND get_bfnd(); 
PTR_BLOB make_blob();
PTR_LABEL make_label();
PTR_LABEL make_label_node();
int is_interface_stat();
PTR_LLND make_llnd (); 
PTR_LLND make_llnd_label (); 
PTR_TYPE make_sa_type(); 
PTR_SYMB procedure_call();
PTR_BFND proc_list();
PTR_SYMB set_id_list();
PTR_LLND set_ll_list();
PTR_LLND add_to_lowLevelList(), add_to_lowList();
PTR_BFND set_stat_list() ;
PTR_BLOB follow_blob();
PTR_SYMB proc_decl_init();
PTR_CMNT make_comment();
PTR_HASH correct_symtab();
char *copyn();
char *convic();
char *StringConcatenation();
int atoi();
PTR_BFND make_logif();
PTR_BFND make_if();
PTR_BFND make_forall();
void startproc();
void match_parameters();
void make_else();
void make_elseif();
void make_endif();
void make_elsewhere();
void make_elsewhere_mask();
void make_endwhere();
void make_endforall();
void make_endselect();
void make_extend();
void make_endextend();
void make_section();
void make_section_extend();
void doinclude();
void endproc();
void err();
void execerr();
void flline();
void warn();
void warn1();
void newprog();
void set_type();
void dclerr();
void enddcl();
void install_const();
void setimpl();
void copy_module_scope();
void replace_symbol_in_expr();
long convci();
void set_expr_type();
void errstr();
void yyerror();
void set_blobs();
void make_loop();
void startioctl();
void endioctl();
void redefine_func_arg_type();
int isResultVar();

/* used by FORTRAN M */
PTR_BFND make_processdo();
PTR_BFND make_processes();
PTR_BFND make_endprocesses();

PTR_BFND make_endparallel();/*OMP*/
PTR_BFND make_parallel();/*OMP*/
PTR_BFND make_endsingle();/*OMP*/
PTR_BFND make_single();/*OMP*/
PTR_BFND make_endmaster();/*OMP*/
PTR_BFND make_master();/*OMP*/
PTR_BFND make_endordered();/*OMP*/
PTR_BFND make_ordered();/*OMP*/
PTR_BFND make_endcritical();/*OMP*/
PTR_BFND make_critical();/*OMP*/
PTR_BFND make_endsections();/*OMP*/
PTR_BFND make_sections();/*OMP*/
PTR_BFND make_ompsection();/*OMP*/
PTR_BFND make_endparallelsections();/*OMP*/
PTR_BFND make_parallelsections();/*OMP*/
PTR_BFND make_endworkshare();/*OMP*/
PTR_BFND make_workshare();/*OMP*/
PTR_BFND make_endparallelworkshare();/*OMP*/
PTR_BFND make_parallelworkshare();/*OMP*/



/* Line 264 of yacc.c  */
#line 704 "gram1.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   5622

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  343
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  520
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1255
/* YYNRULES -- Number of states.  */
#define YYNSTATES  2473

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   342

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,     1,
       2,   341,   342
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     8,    12,    16,    20,    24,    27,
      29,    31,    33,    37,    41,    46,    52,    58,    62,    67,
      71,    72,    75,    78,    81,    83,    85,    90,    96,   101,
     107,   110,   116,   118,   119,   121,   122,   124,   125,   128,
     132,   134,   138,   140,   142,   144,   145,   146,   147,   149,
     151,   153,   155,   157,   159,   161,   163,   165,   167,   169,
     171,   173,   176,   181,   184,   190,   192,   194,   196,   198,
     200,   202,   204,   206,   208,   210,   212,   214,   217,   221,
     227,   233,   236,   238,   240,   242,   244,   246,   248,   250,
     252,   254,   256,   258,   260,   262,   264,   266,   268,   270,
     272,   274,   276,   278,   283,   291,   294,   298,   306,   313,
     314,   317,   323,   325,   330,   332,   334,   336,   339,   341,
     346,   348,   350,   352,   354,   356,   358,   361,   364,   367,
     369,   371,   379,   383,   388,   392,   397,   401,   404,   410,
     411,   414,   417,   423,   424,   429,   435,   436,   439,   443,
     445,   447,   449,   451,   453,   455,   457,   459,   461,   463,
     464,   466,   472,   479,   486,   487,   489,   495,   505,   507,
     509,   512,   515,   516,   517,   520,   523,   529,   534,   539,
     543,   548,   552,   557,   561,   565,   570,   576,   580,   585,
     591,   595,   599,   601,   605,   608,   613,   617,   622,   626,
     630,   634,   638,   642,   646,   648,   653,   655,   657,   662,
     666,   667,   668,   673,   675,   679,   681,   685,   688,   692,
     696,   701,   704,   705,   707,   709,   713,   719,   721,   725,
     726,   728,   736,   738,   742,   745,   748,   752,   754,   756,
     761,   765,   768,   770,   772,   774,   776,   780,   782,   786,
     788,   790,   797,   799,   801,   804,   807,   809,   813,   815,
     818,   821,   823,   827,   829,   833,   839,   841,   843,   845,
     848,   851,   855,   859,   861,   865,   869,   871,   875,   877,
     879,   883,   885,   889,   891,   893,   897,   903,   904,   905,
     907,   912,   917,   919,   923,   927,   930,   932,   936,   940,
     947,   954,   962,   964,   966,   970,   974,   976,   978,   982,
     986,   987,   991,   992,   995,   999,  1001,  1003,  1006,  1010,
    1012,  1014,  1016,  1020,  1022,  1026,  1028,  1030,  1034,  1039,
    1040,  1043,  1046,  1048,  1050,  1054,  1056,  1060,  1062,  1063,
    1064,  1065,  1068,  1069,  1071,  1073,  1075,  1078,  1081,  1086,
    1088,  1092,  1094,  1098,  1100,  1102,  1104,  1106,  1110,  1114,
    1118,  1122,  1126,  1129,  1132,  1135,  1139,  1143,  1147,  1151,
    1155,  1159,  1163,  1167,  1171,  1175,  1179,  1182,  1186,  1190,
    1192,  1194,  1196,  1198,  1200,  1202,  1208,  1215,  1220,  1226,
    1230,  1232,  1234,  1240,  1245,  1248,  1249,  1251,  1257,  1258,
    1260,  1262,  1266,  1268,  1272,  1275,  1277,  1279,  1281,  1283,
    1285,  1287,  1291,  1295,  1301,  1303,  1305,  1309,  1312,  1318,
    1323,  1328,  1332,  1335,  1337,  1338,  1339,  1346,  1348,  1350,
    1352,  1357,  1363,  1365,  1370,  1376,  1377,  1379,  1383,  1385,
    1387,  1389,  1392,  1396,  1400,  1403,  1405,  1408,  1411,  1414,
    1418,  1426,  1430,  1434,  1436,  1439,  1442,  1444,  1447,  1451,
    1453,  1455,  1457,  1463,  1471,  1472,  1479,  1484,  1496,  1510,
    1515,  1519,  1523,  1531,  1540,  1544,  1546,  1549,  1552,  1556,
    1558,  1562,  1563,  1565,  1566,  1568,  1570,  1573,  1579,  1586,
    1588,  1592,  1596,  1597,  1600,  1602,  1608,  1616,  1617,  1619,
    1623,  1627,  1634,  1640,  1647,  1652,  1658,  1664,  1667,  1669,
    1671,  1682,  1684,  1688,  1693,  1697,  1701,  1705,  1709,  1716,
    1723,  1729,  1738,  1741,  1745,  1749,  1757,  1765,  1766,  1768,
    1773,  1776,  1781,  1783,  1786,  1789,  1791,  1793,  1794,  1795,
    1796,  1799,  1802,  1805,  1808,  1811,  1813,  1816,  1819,  1823,
    1828,  1831,  1835,  1837,  1841,  1845,  1847,  1849,  1851,  1855,
    1857,  1859,  1864,  1870,  1872,  1874,  1878,  1882,  1884,  1889,
    1891,  1893,  1895,  1898,  1901,  1904,  1906,  1910,  1914,  1919,
    1924,  1926,  1930,  1932,  1938,  1940,  1942,  1944,  1948,  1952,
    1956,  1960,  1964,  1968,  1970,  1974,  1980,  1986,  1992,  1993,
    1994,  1996,  2000,  2002,  2004,  2008,  2012,  2016,  2020,  2023,
    2027,  2031,  2032,  2034,  2036,  2038,  2040,  2042,  2044,  2046,
    2048,  2050,  2052,  2054,  2056,  2058,  2060,  2062,  2064,  2066,
    2068,  2070,  2072,  2074,  2076,  2078,  2080,  2082,  2084,  2086,
    2088,  2090,  2092,  2094,  2096,  2098,  2100,  2102,  2104,  2106,
    2108,  2110,  2112,  2114,  2116,  2118,  2120,  2122,  2124,  2126,
    2128,  2130,  2132,  2134,  2136,  2138,  2140,  2142,  2144,  2146,
    2148,  2150,  2152,  2154,  2156,  2158,  2160,  2164,  2168,  2171,
    2175,  2177,  2181,  2183,  2187,  2189,  2193,  2195,  2200,  2204,
    2206,  2210,  2212,  2216,  2221,  2223,  2228,  2233,  2238,  2242,
    2246,  2248,  2252,  2256,  2258,  2262,  2266,  2268,  2272,  2276,
    2278,  2282,  2283,  2289,  2296,  2305,  2307,  2311,  2313,  2315,
    2317,  2322,  2324,  2325,  2328,  2332,  2335,  2340,  2341,  2343,
    2349,  2354,  2361,  2366,  2368,  2373,  2378,  2380,  2387,  2389,
    2393,  2395,  2399,  2401,  2406,  2408,  2410,  2414,  2416,  2418,
    2422,  2424,  2425,  2427,  2430,  2434,  2436,  2439,  2445,  2450,
    2455,  2462,  2464,  2468,  2470,  2472,  2479,  2484,  2486,  2490,
    2492,  2494,  2496,  2498,  2500,  2504,  2506,  2508,  2510,  2517,
    2522,  2524,  2529,  2531,  2533,  2535,  2537,  2542,  2545,  2553,
    2555,  2560,  2562,  2564,  2576,  2577,  2580,  2584,  2586,  2590,
    2592,  2596,  2598,  2602,  2604,  2608,  2610,  2614,  2616,  2620,
    2629,  2631,  2635,  2638,  2641,  2649,  2651,  2655,  2657,  2661,
    2663,  2668,  2670,  2674,  2676,  2678,  2679,  2681,  2683,  2686,
    2688,  2690,  2692,  2694,  2696,  2698,  2700,  2702,  2704,  2706,
    2715,  2722,  2731,  2738,  2740,  2747,  2754,  2761,  2768,  2770,
    2774,  2780,  2782,  2786,  2795,  2802,  2809,  2814,  2820,  2826,
    2829,  2832,  2833,  2835,  2839,  2841,  2846,  2854,  2856,  2860,
    2864,  2866,  2870,  2876,  2880,  2884,  2886,  2890,  2892,  2894,
    2898,  2902,  2906,  2910,  2921,  2930,  2941,  2942,  2943,  2945,
    2948,  2953,  2958,  2965,  2967,  2969,  2971,  2973,  2975,  2977,
    2979,  2981,  2983,  2985,  2987,  2994,  2999,  3004,  3008,  3018,
    3020,  3022,  3026,  3028,  3034,  3040,  3050,  3051,  3053,  3055,
    3059,  3063,  3067,  3071,  3075,  3082,  3086,  3090,  3094,  3098,
    3106,  3112,  3114,  3116,  3120,  3125,  3127,  3129,  3133,  3135,
    3137,  3141,  3145,  3148,  3152,  3157,  3162,  3168,  3174,  3176,
    3179,  3184,  3189,  3194,  3195,  3197,  3200,  3208,  3215,  3219,
    3223,  3231,  3237,  3239,  3243,  3245,  3250,  3253,  3257,  3261,
    3266,  3273,  3277,  3280,  3284,  3286,  3288,  3293,  3299,  3303,
    3310,  3313,  3318,  3321,  3323,  3327,  3331,  3332,  3334,  3338,
    3341,  3344,  3347,  3350,  3360,  3366,  3368,  3372,  3375,  3378,
    3381,  3391,  3396,  3398,  3402,  3404,  3406,  3409,  3410,  3418,
    3420,  3425,  3427,  3431,  3433,  3435,  3437,  3454,  3455,  3459,
    3463,  3467,  3471,  3478,  3488,  3494,  3496,  3500,  3506,  3508,
    3512,  3514,  3516,  3518,  3520,  3522,  3524,  3526,  3528,  3530,
    3532,  3534,  3536,  3538,  3540,  3542,  3544,  3546,  3548,  3550,
    3552,  3554,  3556,  3558,  3560,  3562,  3564,  3566,  3569,  3572,
    3577,  3581,  3586,  3592,  3594,  3596,  3598,  3600,  3602,  3604,
    3606,  3608,  3610,  3616,  3619,  3622,  3625,  3628,  3631,  3637,
    3639,  3641,  3643,  3648,  3653,  3658,  3663,  3665,  3667,  3669,
    3671,  3673,  3675,  3677,  3679,  3681,  3683,  3685,  3687,  3689,
    3691,  3693,  3698,  3702,  3707,  3713,  3715,  3717,  3719,  3721,
    3726,  3730,  3733,  3738,  3742,  3747,  3751,  3756,  3762,  3764,
    3766,  3768,  3770,  3772,  3774,  3776,  3784,  3790,  3792,  3794,
    3796,  3798,  3803,  3807,  3812,  3818,  3820,  3822,  3827,  3831,
    3836,  3842,  3844,  3846,  3849,  3851,  3854,  3859,  3863,  3868,
    3872,  3877,  3883,  3885,  3887,  3889,  3891,  3893,  3895,  3897,
    3899,  3901,  3903,  3905,  3908,  3913,  3917,  3920,  3925,  3929,
    3932,  3936,  3939,  3942,  3945,  3948,  3951,  3954,  3958,  3961,
    3967,  3970,  3976,  3979,  3985,  3987,  3989,  3993,  3997,  3998,
    3999,  4001,  4003,  4005,  4007,  4009,  4011,  4015,  4018,  4024,
    4029,  4032,  4038,  4043,  4046,  4049,  4051,  4053,  4057,  4060,
    4063,  4066,  4071,  4076,  4081,  4086,  4091,  4096,  4098,  4100,
    4102,  4106,  4109,  4112,  4114,  4116,  4118,  4120,  4122,  4124,
    4126,  4131,  4136,  4141,  4144,  4146,  4148,  4152,  4154,  4156,
    4162,  4168,  4170,  4174,  4176,  4178,  4180,  4186,  4192,  4198,
    4200,  4204,  4207,  4213,  4216,  4222
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     344,     0,    -1,    -1,   344,   345,   116,    -1,   346,   347,
     564,    -1,   346,   364,   564,    -1,   346,   509,   564,    -1,
     346,   133,   360,    -1,   346,   247,    -1,   257,    -1,     1,
      -1,   150,    -1,   193,   348,   355,    -1,    57,   348,   356,
      -1,   233,   348,   350,   357,    -1,   349,   233,   348,   350,
     357,    -1,   124,   348,   351,   357,   353,    -1,   352,   357,
     353,    -1,   114,   354,   357,   353,    -1,   164,   348,   354,
      -1,    -1,   202,   361,    -1,   195,   361,    -1,    95,   361,
      -1,   354,    -1,   354,    -1,   385,   124,   348,   354,    -1,
     385,   349,   124,   348,   354,    -1,   349,   124,   348,   354,
      -1,   349,   385,   124,   348,   354,    -1,   361,   362,    -1,
     361,   213,    15,   354,    21,    -1,   129,    -1,    -1,   354,
      -1,    -1,   354,    -1,    -1,    15,    21,    -1,    15,   358,
      21,    -1,   359,    -1,   358,     8,   359,    -1,   354,    -1,
       5,    -1,    64,    -1,    -1,    -1,    -1,   369,    -1,   370,
      -1,   371,    -1,   401,    -1,   397,    -1,   565,    -1,   406,
      -1,   407,    -1,   408,    -1,   466,    -1,   387,    -1,   402,
      -1,   412,    -1,   216,   476,    -1,   216,   476,   477,   443,
      -1,   123,   475,    -1,   183,   476,    15,   449,    21,    -1,
     377,    -1,   378,    -1,   383,    -1,   380,    -1,   382,    -1,
     398,    -1,   399,    -1,   400,    -1,   365,    -1,   453,    -1,
     451,    -1,   379,    -1,   142,   476,    -1,   142,   476,   354,
      -1,   141,   476,    15,   367,    21,    -1,   140,   476,    15,
      26,    21,    -1,   107,   518,    -1,    10,    -1,   366,    -1,
     368,    -1,    17,    -1,    16,    -1,     5,    -1,     9,    -1,
      37,    -1,    23,    -1,    22,    -1,    35,    -1,    38,    -1,
      34,    -1,    25,    -1,    32,    -1,    29,    -1,    28,    -1,
      31,    -1,    30,    -1,    33,    -1,    24,    -1,   245,   476,
     477,   354,    -1,   245,     8,   476,   361,   376,   477,   354,
      -1,   112,   476,    -1,   112,   476,   354,    -1,   385,   372,
     354,   476,   459,   391,   396,    -1,   371,     8,   354,   459,
     391,   396,    -1,    -1,     7,     7,    -1,     8,   361,   373,
       7,     7,    -1,   374,    -1,   373,     8,   361,   374,    -1,
     183,    -1,   376,    -1,    44,    -1,    87,   459,    -1,   119,
      -1,   145,    15,   375,    21,    -1,   143,    -1,   178,    -1,
     187,    -1,   216,    -1,   230,    -1,   236,    -1,   361,   148,
      -1,   361,   180,    -1,   361,   147,    -1,   194,    -1,   191,
      -1,   145,   476,    15,   375,    21,   477,   354,    -1,   377,
       8,   354,    -1,   178,   476,   477,   354,    -1,   378,     8,
     354,    -1,   230,   476,   477,   405,    -1,   379,     8,   405,
      -1,   191,   476,    -1,   191,   476,   477,   381,   445,    -1,
      -1,   219,   476,    -1,   194,   476,    -1,   194,   476,   477,
     384,   445,    -1,    -1,   389,   386,   393,   386,    -1,   244,
      15,   354,    21,   386,    -1,    -1,   388,   354,    -1,   387,
       8,   354,    -1,    13,    -1,     6,    -1,   390,    -1,   144,
      -1,   200,    -1,    68,    -1,    90,    -1,    91,    -1,   154,
      -1,    63,    -1,    -1,   392,    -1,     5,   539,   496,   540,
     386,    -1,     5,   539,    15,   540,     5,    21,    -1,     5,
     539,    15,   540,   482,    21,    -1,    -1,   392,    -1,    15,
     560,   394,   395,    21,    -1,    15,   560,   394,   395,     8,
     560,   394,   395,    21,    -1,   482,    -1,     5,    -1,   551,
     482,    -1,   551,     5,    -1,    -1,    -1,    26,   482,    -1,
      18,   482,    -1,    87,   477,   476,   354,   459,    -1,   397,
       8,   354,   459,    -1,    44,   476,   477,   405,    -1,   398,
       8,   405,    -1,   187,   476,   477,   405,    -1,   399,     8,
     405,    -1,   236,   476,   477,   405,    -1,   400,     8,   405,
      -1,    67,   476,   405,    -1,    67,   476,   404,   405,    -1,
     401,   533,   404,   533,   405,    -1,   401,     8,   405,    -1,
     167,   476,   403,   485,    -1,   402,   533,   403,   533,   485,
      -1,   402,     8,   485,    -1,    37,   354,    37,    -1,    23,
      -1,    37,   354,    37,    -1,   354,   459,    -1,   119,   476,
     477,   354,    -1,   406,     8,   354,    -1,   143,   476,   477,
     354,    -1,   407,     8,   354,    -1,   117,   476,   409,    -1,
     408,     8,   409,    -1,    15,   410,    21,    -1,   411,     8,
     411,    -1,   410,     8,   411,    -1,   354,    -1,   354,    15,
     481,    21,    -1,   490,    -1,   413,    -1,    80,   475,   414,
     416,    -1,   413,   533,   416,    -1,    -1,    -1,   417,    37,
     418,    37,    -1,   419,    -1,   417,     8,   419,    -1,   430,
      -1,   418,     8,   430,    -1,   420,   422,    -1,   420,   422,
     423,    -1,   420,   422,   424,    -1,   420,   422,   423,   424,
      -1,   420,   427,    -1,    -1,   354,    -1,   354,    -1,    15,
     425,    21,    -1,    15,   426,     7,   426,    21,    -1,   439,
      -1,   425,     8,   439,    -1,    -1,   439,    -1,    15,   428,
       8,   421,    26,   425,    21,    -1,   429,    -1,   428,     8,
     429,    -1,   422,   423,    -1,   422,   424,    -1,   422,   423,
     424,    -1,   427,    -1,   431,    -1,   420,   421,     5,   431,
      -1,   434,     5,   431,    -1,   420,   421,    -1,   433,    -1,
     435,    -1,   437,    -1,    36,    -1,    36,   246,   499,    -1,
      27,    -1,    27,   246,   499,    -1,    64,    -1,   432,    -1,
     420,   485,    15,   560,   478,    21,    -1,    59,    -1,   434,
      -1,    17,   434,    -1,    16,   434,    -1,   149,    -1,   149,
     246,   499,    -1,   436,    -1,    17,   436,    -1,    16,   436,
      -1,   201,    -1,   201,   246,   499,    -1,    92,    -1,    92,
     246,   499,    -1,    15,   438,     8,   438,    21,    -1,   435,
      -1,   433,    -1,   440,    -1,    17,   440,    -1,    16,   440,
      -1,   439,    17,   440,    -1,   439,    16,   440,    -1,   441,
      -1,   440,     5,   441,    -1,   440,    37,   441,    -1,   442,
      -1,   442,     9,   441,    -1,   149,    -1,   421,    -1,    15,
     439,    21,    -1,   444,    -1,   443,     8,   444,    -1,   405,
      -1,   404,    -1,   446,   448,   447,    -1,   445,     8,   446,
     448,   447,    -1,    -1,    -1,   354,    -1,   177,    15,   367,
      21,    -1,    47,    15,    26,    21,    -1,   450,    -1,   449,
       8,   450,    -1,   354,    26,   482,    -1,   163,   452,    -1,
     354,    -1,   452,     8,   354,    -1,   248,   476,   454,    -1,
     248,   476,   454,     8,   363,   457,    -1,   248,   476,   454,
       8,   363,   172,    -1,   248,   476,   454,     8,   363,   172,
     455,    -1,   354,    -1,   456,    -1,   455,     8,   456,    -1,
     354,    18,   354,    -1,   354,    -1,   458,    -1,   457,     8,
     458,    -1,   354,    18,   354,    -1,    -1,    15,   460,    21,
      -1,    -1,   461,   462,    -1,   460,     8,   462,    -1,   463,
      -1,     7,    -1,   482,     7,    -1,   482,     7,   463,    -1,
       5,    -1,   482,    -1,   465,    -1,   464,     8,   465,    -1,
     149,    -1,   130,   476,   467,    -1,   131,    -1,   468,    -1,
     467,     8,   468,    -1,   469,    15,   472,    21,    -1,    -1,
     470,   471,    -1,   231,   390,    -1,   385,    -1,   473,    -1,
     472,     8,   473,    -1,   474,    -1,   474,    16,   474,    -1,
     129,    -1,    -1,    -1,    -1,     7,     7,    -1,    -1,   480,
      -1,   482,    -1,   500,    -1,   551,   482,    -1,   560,   479,
      -1,   480,     8,   560,   479,    -1,   482,    -1,   481,     8,
     482,    -1,   483,    -1,    15,   482,    21,    -1,   498,    -1,
     486,    -1,   494,    -1,   501,    -1,   482,    17,   482,    -1,
     482,    16,   482,    -1,   482,     5,   482,    -1,   482,    37,
     482,    -1,   482,     9,   482,    -1,   366,   482,    -1,    17,
     482,    -1,    16,   482,    -1,   482,    25,   482,    -1,   482,
      29,   482,    -1,   482,    31,   482,    -1,   482,    28,   482,
      -1,   482,    30,   482,    -1,   482,    32,   482,    -1,   482,
      24,   482,    -1,   482,    33,   482,    -1,   482,    38,   482,
      -1,   482,    35,   482,    -1,   482,    22,   482,    -1,    34,
     482,    -1,   482,    23,   482,    -1,   482,   366,   482,    -1,
      17,    -1,    16,    -1,   354,    -1,   485,    -1,   488,    -1,
     487,    -1,   485,    15,   560,   478,    21,    -1,   485,    15,
     560,   478,    21,   492,    -1,   488,    15,   478,    21,    -1,
     488,    15,   478,    21,   492,    -1,   486,     3,   129,    -1,
     485,    -1,   488,    -1,   485,    15,   560,   478,    21,    -1,
     488,    15,   478,    21,    -1,   485,   492,    -1,    -1,   492,
      -1,    15,   493,     7,   493,    21,    -1,    -1,   482,    -1,
     495,    -1,   495,   246,   499,    -1,   496,    -1,   496,   246,
     499,    -1,   497,   491,    -1,    36,    -1,    27,    -1,   201,
      -1,    92,    -1,   149,    -1,    64,    -1,   485,   246,    64,
      -1,   496,   246,    64,    -1,    15,   483,     8,   483,    21,
      -1,   485,    -1,   496,    -1,   482,     7,   482,    -1,   482,
       7,    -1,   482,     7,   482,     7,   482,    -1,   482,     7,
       7,   482,    -1,     7,   482,     7,   482,    -1,     7,     7,
     482,    -1,     7,   482,    -1,     7,    -1,    -1,    -1,    14,
     395,   502,   557,   503,    20,    -1,   485,    -1,   488,    -1,
     489,    -1,   505,     8,   560,   489,    -1,   505,     8,   560,
     551,   485,    -1,   504,    -1,   506,     8,   560,   504,    -1,
     506,     8,   560,   551,   485,    -1,    -1,   485,    -1,   508,
       8,   485,    -1,   530,    -1,   529,    -1,   512,    -1,   520,
     512,    -1,   102,   538,   518,    -1,   103,   538,   517,    -1,
     108,   518,    -1,   510,    -1,   520,   510,    -1,   521,   530,
      -1,   521,   239,    -1,   520,   521,   239,    -1,    97,   538,
      15,   482,    21,   239,   517,    -1,    96,   538,   517,    -1,
     106,   538,   517,    -1,   513,    -1,    76,   538,    -1,   522,
     530,    -1,   522,    -1,   520,   522,    -1,   105,   538,   517,
      -1,   566,    -1,   829,    -1,   845,    -1,    89,   538,    15,
     482,    21,    -1,    89,   538,   539,   528,   540,   600,   511,
      -1,    -1,     8,   361,   254,    15,   482,    21,    -1,   254,
      15,   482,    21,    -1,   185,   538,   539,   528,   540,   533,
     526,    26,   482,     8,   482,    -1,   185,   538,   539,   528,
     540,   533,   526,    26,   482,     8,   482,     8,   482,    -1,
      62,   538,   514,   517,    -1,    84,   538,   517,    -1,   110,
     538,   517,    -1,   218,   538,   361,    62,    15,   482,    21,
      -1,   520,   218,   538,   361,    62,    15,   482,    21,    -1,
      15,   516,    21,    -1,   482,    -1,   482,     7,    -1,     7,
     482,    -1,   482,     7,   482,    -1,   515,    -1,   516,     8,
     515,    -1,    -1,   354,    -1,    -1,   354,    -1,    75,    -1,
     519,     7,    -1,   155,   538,    15,   482,    21,    -1,   122,
     538,    15,   523,   525,    21,    -1,   524,    -1,   523,     8,
     524,    -1,   526,    26,   500,    -1,    -1,     8,   482,    -1,
     354,    -1,   526,    26,   482,     8,   482,    -1,   526,    26,
     482,     8,   482,     8,   482,    -1,    -1,   149,    -1,   113,
     538,   517,    -1,    98,   538,   517,    -1,    98,   538,    15,
     482,    21,   517,    -1,   252,   538,    15,   482,    21,    -1,
     520,   252,   538,    15,   482,    21,    -1,   531,   482,    26,
     482,    -1,   188,   538,   486,    18,   482,    -1,    48,   538,
     465,   240,   354,    -1,    77,   538,    -1,   532,    -1,   541,
      -1,    46,   538,    15,   482,    21,   465,     8,   465,     8,
     465,    -1,   534,    -1,   534,    15,    21,    -1,   534,    15,
     535,    21,    -1,   214,   538,   493,    -1,   537,   538,   493,
      -1,    79,   538,   517,    -1,   115,   538,   517,    -1,    45,
     538,    15,   507,   505,    21,    -1,    81,   538,    15,   507,
     506,    21,    -1,   170,   538,    15,   508,    21,    -1,   253,
     538,    15,   482,    21,   486,    26,   482,    -1,   152,   415,
      -1,   186,   538,   465,    -1,    49,   538,   354,    -1,    49,
     538,   354,   533,    15,   464,    21,    -1,    69,   538,    15,
     464,    21,   533,   482,    -1,    -1,     8,    -1,    61,   538,
     354,   560,    -1,   560,   536,    -1,   535,     8,   560,   536,
      -1,   482,    -1,   551,   482,    -1,     5,   465,    -1,   184,
      -1,   232,    -1,    -1,    -1,    -1,   542,   548,    -1,   542,
     563,    -1,   542,     5,    -1,   542,     9,    -1,   544,   548,
      -1,   546,    -1,   552,   548,    -1,   552,   547,    -1,   552,
     548,   555,    -1,   552,   547,     8,   555,    -1,   553,   548,
      -1,   553,   548,   557,    -1,   554,    -1,   554,     8,   557,
      -1,   543,   538,   561,    -1,    53,    -1,   215,    -1,   104,
      -1,   545,   538,   561,    -1,   176,    -1,    66,    -1,   139,
     538,   561,   548,    -1,   139,   538,   561,   548,   557,    -1,
     563,    -1,     5,    -1,    15,   562,    21,    -1,    15,   549,
      21,    -1,   550,    -1,   549,     8,   560,   550,    -1,   562,
      -1,     5,    -1,     9,    -1,   551,   482,    -1,   551,     5,
      -1,   551,     9,    -1,   166,    -1,   197,   538,   561,    -1,
     256,   538,   561,    -1,   190,   538,   562,   561,    -1,   190,
     538,     5,   561,    -1,   556,    -1,   555,     8,   556,    -1,
     486,    -1,    15,   555,     8,   527,    21,    -1,   483,    -1,
     559,    -1,   558,    -1,   483,     8,   483,    -1,   483,     8,
     559,    -1,   559,     8,   483,    -1,   559,     8,   559,    -1,
     558,     8,   483,    -1,   558,     8,   559,    -1,   498,    -1,
      15,   482,    21,    -1,    15,   483,     8,   527,    21,    -1,
      15,   559,     8,   527,    21,    -1,    15,   558,     8,   527,
      21,    -1,    -1,    -1,   563,    -1,    15,   562,    21,    -1,
     486,    -1,   494,    -1,   562,   484,   562,    -1,   562,     5,
     562,    -1,   562,    37,   562,    -1,   562,     9,   562,    -1,
     484,   562,    -1,   562,    23,   562,    -1,   129,    26,   482,
      -1,    -1,   257,    -1,   567,    -1,   615,    -1,   590,    -1,
     569,    -1,   580,    -1,   575,    -1,   627,    -1,   630,    -1,
     706,    -1,   572,    -1,   581,    -1,   583,    -1,   585,    -1,
     587,    -1,   635,    -1,   641,    -1,   638,    -1,   766,    -1,
     764,    -1,   591,    -1,   616,    -1,   645,    -1,   695,    -1,
     693,    -1,   694,    -1,   696,    -1,   697,    -1,   698,    -1,
     699,    -1,   700,    -1,   708,    -1,   710,    -1,   715,    -1,
     712,    -1,   714,    -1,   718,    -1,   716,    -1,   717,    -1,
     729,    -1,   733,    -1,   734,    -1,   737,    -1,   736,    -1,
     738,    -1,   739,    -1,   740,    -1,   741,    -1,   644,    -1,
     723,    -1,   724,    -1,   725,    -1,   728,    -1,   742,    -1,
     745,    -1,   750,    -1,   755,    -1,   757,    -1,   758,    -1,
     759,    -1,   760,    -1,   762,    -1,   721,    -1,   765,    -1,
      82,   476,   568,    -1,   567,     8,   568,    -1,   354,   459,
      -1,    94,   476,   570,    -1,   571,    -1,   570,     8,   571,
      -1,   354,    -1,   138,   476,   573,    -1,   574,    -1,   573,
       8,   574,    -1,   354,    -1,   228,   476,   579,   576,    -1,
      15,   577,    21,    -1,   578,    -1,   577,     8,   578,    -1,
     482,    -1,   482,     7,   482,    -1,     7,    15,   481,    21,
      -1,   354,    -1,   259,   476,   354,   459,    -1,   303,   476,
     354,   459,    -1,   580,     8,   354,   459,    -1,   136,   476,
     582,    -1,   581,     8,   582,    -1,   354,    -1,   211,   476,
     584,    -1,   583,     8,   584,    -1,   354,    -1,   205,   476,
     586,    -1,   585,     8,   586,    -1,   354,    -1,    70,   476,
     588,    -1,   587,     8,   588,    -1,   354,    -1,   175,   354,
     459,    -1,    -1,    88,   476,   594,   597,   589,    -1,   204,
     538,   594,   598,   600,   589,    -1,   204,   538,   598,   600,
     589,     7,     7,   592,    -1,   593,    -1,   592,     8,   593,
      -1,   594,    -1,   595,    -1,   354,    -1,   354,    15,   481,
      21,    -1,   354,    -1,    -1,   598,   600,    -1,    15,   599,
      21,    -1,   600,   601,    -1,   599,     8,   600,   601,    -1,
      -1,    58,    -1,    58,    15,   560,   614,    21,    -1,   126,
      15,   602,    21,    -1,   258,    15,   602,     8,   482,    21,
      -1,   165,    15,   482,    21,    -1,     5,    -1,   137,    15,
     602,    21,    -1,    86,    15,   603,    21,    -1,   354,    -1,
      15,   604,    21,   361,   255,   606,    -1,   605,    -1,   604,
       8,   605,    -1,   482,    -1,   482,     7,   482,    -1,   607,
      -1,   607,    15,   608,    21,    -1,   354,    -1,   609,    -1,
     608,     8,   609,    -1,   482,    -1,   754,    -1,    40,   610,
     611,    -1,   354,    -1,    -1,   612,    -1,    17,   613,    -1,
     611,    17,   613,    -1,   482,    -1,   551,   482,    -1,   551,
     482,     8,   551,   482,    -1,    43,   476,   618,   620,    -1,
     199,   538,   619,   620,    -1,   199,   538,   620,     7,     7,
     617,    -1,   619,    -1,   617,     8,   619,    -1,   354,    -1,
     485,    -1,    15,   625,    21,   361,   255,   621,    -1,   624,
      15,   622,    21,    -1,   623,    -1,   622,     8,   623,    -1,
     482,    -1,     5,    -1,   500,    -1,   354,    -1,   626,    -1,
     625,     8,   626,    -1,   354,    -1,     5,    -1,     7,    -1,
     628,     7,     7,   476,   354,   459,    -1,   627,     8,   354,
     459,    -1,   629,    -1,   628,     8,   361,   629,    -1,    82,
      -1,   259,    -1,   303,    -1,    94,    -1,    87,    15,   460,
      21,    -1,   228,   576,    -1,    43,    15,   625,    21,   361,
     255,   621,    -1,    43,    -1,    88,   598,   600,   589,    -1,
      88,    -1,    67,    -1,   385,     8,   361,    93,   476,    15,
     631,    21,     7,     7,   633,    -1,    -1,   632,     7,    -1,
     631,     8,     7,    -1,   634,    -1,   633,     8,   634,    -1,
     354,    -1,   127,   476,   636,    -1,   637,    -1,   636,     8,
     637,    -1,   354,    -1,    74,   476,   639,    -1,   640,    -1,
     639,     8,   640,    -1,   354,    -1,    51,   476,   642,    -1,
      51,   476,     8,   361,    67,     7,     7,   642,    -1,   643,
      -1,   642,     8,   643,    -1,   354,   459,    -1,   168,   538,
      -1,   182,   538,    15,   646,    21,   648,   652,    -1,   647,
      -1,   646,     8,   647,    -1,   485,    -1,   600,   173,   649,
      -1,   600,    -1,   485,    15,   650,    21,    -1,   651,    -1,
     650,     8,   651,    -1,   482,    -1,     5,    -1,    -1,   653,
      -1,   654,    -1,   653,   654,    -1,   658,    -1,   678,    -1,
     686,    -1,   655,    -1,   663,    -1,   665,    -1,   664,    -1,
     656,    -1,   659,    -1,   660,    -1,     8,   361,   209,    15,
     701,     7,   702,    21,    -1,     8,   361,   209,    15,   702,
      21,    -1,     8,   361,    71,    15,   657,     7,   702,    21,
      -1,     8,   361,    71,    15,   702,    21,    -1,   354,    -1,
       8,   361,   169,    15,   662,    21,    -1,     8,   361,   282,
      15,   662,    21,    -1,     8,   361,   191,    15,   662,    21,
      -1,     8,   361,   320,    15,   661,    21,    -1,   482,    -1,
     482,     8,   482,    -1,   482,     8,   482,     8,   482,    -1,
     486,    -1,   662,     8,   486,    -1,     8,   361,   135,    15,
     701,     7,   719,    21,    -1,     8,   361,   135,    15,   719,
      21,    -1,     8,   361,   229,    15,   482,    21,    -1,     8,
     361,    41,   666,    -1,     8,   361,    41,   666,   666,    -1,
      15,   600,   667,   668,    21,    -1,   148,     7,    -1,   180,
       7,    -1,    -1,   669,    -1,   668,     8,   669,    -1,   691,
      -1,   691,    15,   670,    21,    -1,   691,    15,   670,    21,
      15,   672,    21,    -1,   671,    -1,   670,     8,   671,    -1,
     482,     7,   482,    -1,   673,    -1,   672,     8,   673,    -1,
     674,     7,   675,     7,   676,    -1,   674,     7,   675,    -1,
     674,     7,   676,    -1,   674,    -1,   675,     7,   676,    -1,
     675,    -1,   676,    -1,   361,   217,   677,    -1,   361,   157,
     677,    -1,   361,   128,   677,    -1,    15,   480,    21,    -1,
       8,   361,   208,    15,   679,   683,   680,     8,   682,    21,
      -1,     8,   361,   208,    15,   679,   683,   680,    21,    -1,
       8,   361,   208,    15,   679,   681,   680,     7,   682,    21,
      -1,    -1,    -1,   354,    -1,   361,   683,    -1,   682,     8,
     361,   683,    -1,   684,    15,   486,    21,    -1,   685,    15,
     662,     8,   482,    21,    -1,   234,    -1,   192,    -1,   162,
      -1,   159,    -1,    35,    -1,    22,    -1,    24,    -1,    33,
      -1,   247,    -1,   158,    -1,   161,    -1,     8,   361,   223,
      15,   688,    21,    -1,     8,   361,   224,   687,    -1,     8,
     361,   226,   687,    -1,     8,   361,   221,    -1,     8,   361,
     221,    15,   691,    15,   577,    21,    21,    -1,   354,    -1,
     689,    -1,   688,     8,   689,    -1,   691,    -1,   691,    15,
     690,    78,    21,    -1,   691,    15,   690,   577,    21,    -1,
     691,    15,   690,   577,    21,    15,   361,    78,    21,    -1,
      -1,   485,    -1,   691,    -1,   692,     8,   691,    -1,   225,
     538,   687,    -1,   224,   538,   687,    -1,   227,   538,   687,
      -1,   226,   538,   687,    -1,   222,   538,   687,    15,   688,
      21,    -1,   206,   538,   681,    -1,   207,   538,   681,    -1,
      72,   538,   657,    -1,    73,   538,   657,    -1,   210,   538,
      15,   701,     7,   702,    21,    -1,   210,   538,    15,   702,
      21,    -1,   354,    -1,   703,    -1,   702,     8,   703,    -1,
     691,    15,   704,    21,    -1,   691,    -1,   705,    -1,   704,
       8,   705,    -1,   482,    -1,     7,    -1,   237,   476,   707,
      -1,   706,     8,   707,    -1,   354,   459,    -1,   238,   538,
     709,    -1,   238,   538,   709,   678,    -1,   238,   538,   709,
     656,    -1,   238,   538,   709,   678,   656,    -1,   238,   538,
     709,   656,   678,    -1,   354,    -1,   111,   538,    -1,   709,
      15,   482,    21,    -1,   709,    15,   500,    21,    -1,   174,
     538,   487,   713,    -1,    -1,   658,    -1,   109,   538,    -1,
     160,   538,   711,   361,   175,   596,   459,    -1,   160,   538,
     711,   361,   321,   486,    -1,   189,   538,   701,    -1,   212,
     538,   701,    -1,   135,   538,    15,   701,     7,   719,    21,
      -1,   135,   538,    15,   719,    21,    -1,   720,    -1,   719,
       8,   720,    -1,   691,    -1,   691,    15,   481,    21,    -1,
     134,   538,    -1,   134,   538,   658,    -1,   134,   538,   722,
      -1,   134,   538,   658,   722,    -1,     8,   361,   208,    15,
     662,    21,    -1,    50,   538,   727,    -1,    99,   538,    -1,
      52,   538,   727,    -1,   354,    -1,   726,    -1,   726,    15,
     481,    21,    -1,   120,   538,   486,    26,   486,    -1,    83,
     538,   732,    -1,    83,   538,   732,    15,   730,    21,    -1,
     560,   731,    -1,   730,     8,   560,   731,    -1,   551,   482,
      -1,   149,    -1,   100,   538,   732,    -1,   146,   538,   735,
      -1,    -1,   482,    -1,   330,   538,   481,    -1,   101,   538,
      -1,   241,   538,    -1,   242,   538,    -1,    56,   538,    -1,
      65,   538,   560,    15,   535,    21,   395,   477,   662,    -1,
     322,   538,    15,   743,    21,    -1,   744,    -1,   743,     8,
     744,    -1,   361,   315,    -1,   361,   318,    -1,   361,   182,
      -1,   220,   538,    15,   746,    26,   613,    21,   600,   749,
      -1,   485,    15,   747,    21,    -1,   748,    -1,   747,     8,
     748,    -1,   603,    -1,   754,    -1,   132,   692,    -1,    -1,
     153,   538,    15,   485,    18,   751,    21,    -1,   485,    -1,
     485,    15,   752,    21,    -1,   753,    -1,   752,     8,   753,
      -1,   754,    -1,     7,    -1,     5,    -1,   323,   538,   482,
       8,   361,   328,    15,   662,    21,     8,   361,   327,    15,
     481,    21,   756,    -1,    -1,     8,   361,   182,    -1,     8,
     361,   318,    -1,   324,   538,   482,    -1,   325,   538,   482,
      -1,   325,   538,   482,     8,   361,   315,    -1,   326,   538,
     482,     8,   361,   329,    15,   485,    21,    -1,   331,   538,
      15,   761,    21,    -1,   489,    -1,   761,     8,   489,    -1,
     332,   538,    15,   763,    21,    -1,   485,    -1,   763,     8,
     485,    -1,   815,    -1,   768,    -1,   767,    -1,   785,    -1,
     788,    -1,   789,    -1,   790,    -1,   791,    -1,   797,    -1,
     800,    -1,   805,    -1,   806,    -1,   807,    -1,   810,    -1,
     811,    -1,   812,    -1,   813,    -1,   814,    -1,   816,    -1,
     817,    -1,   818,    -1,   819,    -1,   820,    -1,   821,    -1,
     822,    -1,   823,    -1,   824,    -1,   268,   538,    -1,   275,
     538,    -1,   291,   538,   600,   769,    -1,   291,   538,   600,
      -1,   533,   600,   770,   600,    -1,   769,   533,   600,   770,
     600,    -1,   772,    -1,   781,    -1,   776,    -1,   777,    -1,
     773,    -1,   774,    -1,   775,    -1,   779,    -1,   780,    -1,
     827,    15,   828,   826,    21,    -1,   191,   771,    -1,   282,
     771,    -1,   285,   771,    -1,   265,   771,    -1,   299,   771,
      -1,    84,    15,   361,   778,    21,    -1,   191,    -1,   299,
      -1,   288,    -1,   304,    15,   482,    21,    -1,   289,    15,
     482,    21,    -1,   208,    15,   782,    21,    -1,   600,   784,
       7,   783,    -1,   662,    -1,    17,    -1,    16,    -1,     5,
      -1,    37,    -1,   162,    -1,   159,    -1,    35,    -1,    22,
      -1,    24,    -1,    33,    -1,   305,    -1,   306,    -1,   307,
      -1,   247,    -1,   297,   538,   600,   786,    -1,   297,   538,
     600,    -1,   533,   600,   787,   600,    -1,   786,   533,   600,
     787,   600,    -1,   772,    -1,   781,    -1,   773,    -1,   774,
      -1,   279,   538,   600,   804,    -1,   279,   538,   600,    -1,
     296,   538,    -1,   269,   538,   600,   792,    -1,   269,   538,
     600,    -1,   272,   538,   600,   804,    -1,   272,   538,   600,
      -1,   533,   600,   793,   600,    -1,   792,   533,   600,   793,
     600,    -1,   772,    -1,   781,    -1,   773,    -1,   774,    -1,
     795,    -1,   794,    -1,   290,    -1,   298,    15,   361,   796,
       8,   482,    21,    -1,   298,    15,   361,   796,    21,    -1,
     230,    -1,    94,    -1,   284,    -1,   295,    -1,   300,   538,
     600,   798,    -1,   300,   538,   600,    -1,   533,   600,   799,
     600,    -1,   798,   533,   600,   799,   600,    -1,   772,    -1,
     773,    -1,   280,   538,   600,   801,    -1,   280,   538,   600,
      -1,   533,   600,   802,   600,    -1,   801,   533,   600,   802,
     600,    -1,   804,    -1,   803,    -1,   266,   771,    -1,   287,
      -1,   302,   538,    -1,   281,   538,   600,   804,    -1,   281,
     538,   600,    -1,   292,   538,   600,   808,    -1,   292,   538,
     600,    -1,   533,   600,   809,   600,    -1,   808,   533,   600,
     809,   600,    -1,   772,    -1,   781,    -1,   776,    -1,   777,
      -1,   773,    -1,   774,    -1,   775,    -1,   779,    -1,   780,
      -1,   795,    -1,   794,    -1,   276,   538,    -1,   293,   538,
     600,   769,    -1,   293,   538,   600,    -1,   277,   538,    -1,
     294,   538,   600,   769,    -1,   294,   538,   600,    -1,   278,
     538,    -1,   301,   476,   771,    -1,   286,   538,    -1,   273,
     538,    -1,   290,   538,    -1,   274,   538,    -1,   264,   538,
      -1,   263,   538,    -1,   283,   538,   771,    -1,   283,   538,
      -1,   267,   538,    15,   485,    21,    -1,   267,   538,    -1,
     271,   538,    15,   485,    21,    -1,   271,   538,    -1,    37,
     354,   827,    37,   828,    -1,   825,    -1,   485,    -1,   826,
       8,   825,    -1,   826,     8,   485,    -1,    -1,    -1,   830,
      -1,   843,    -1,   831,    -1,   844,    -1,   832,    -1,   833,
      -1,   308,   538,   834,    -1,   310,   538,    -1,   312,   538,
      15,   840,    21,    -1,   312,   538,    15,    21,    -1,   312,
     538,    -1,   313,   538,    15,   840,    21,    -1,   313,   538,
      15,    21,    -1,   313,   538,    -1,   361,   362,    -1,   835,
      -1,   836,    -1,   835,     8,   836,    -1,   361,   837,    -1,
     361,   839,    -1,   361,   838,    -1,   147,    15,   840,    21,
      -1,   148,    15,   840,    21,    -1,   180,    15,   840,    21,
      -1,   318,    15,   840,    21,    -1,   319,    15,   840,    21,
      -1,   314,    15,   841,    21,    -1,   315,    -1,   662,    -1,
     842,    -1,   841,     8,   842,    -1,   361,   316,    -1,   361,
     317,    -1,   309,    -1,   311,    -1,   846,    -1,   847,    -1,
     848,    -1,   849,    -1,   850,    -1,   333,    15,   851,    21,
      -1,   334,    15,   855,    21,    -1,   335,    15,   860,    21,
      -1,   337,   862,    -1,   338,    -1,   852,    -1,   851,     8,
     852,    -1,   853,    -1,   854,    -1,   361,   208,    15,   682,
      21,    -1,   361,   191,    15,   662,    21,    -1,   856,    -1,
     855,     8,   856,    -1,   857,    -1,   858,    -1,   859,    -1,
     361,   228,    15,   688,    21,    -1,   361,    41,    15,   688,
      21,    -1,   361,   209,    15,   702,    21,    -1,   861,    -1,
     860,     8,   861,    -1,   361,   336,    -1,   361,   340,    15,
     646,    21,    -1,   361,   339,    -1,   361,   339,    15,   646,
      21,    -1,   354,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   770,   770,   771,   775,   777,   791,   822,   831,   837,
     857,   866,   882,   894,   904,   911,   917,   922,   927,   951,
     978,   992,   994,   996,  1000,  1017,  1031,  1055,  1071,  1085,
    1103,  1105,  1112,  1116,  1117,  1124,  1125,  1133,  1134,  1136,
    1140,  1141,  1145,  1149,  1155,  1165,  1169,  1174,  1181,  1182,
    1183,  1184,  1185,  1186,  1187,  1188,  1189,  1190,  1191,  1192,
    1193,  1194,  1199,  1204,  1211,  1213,  1214,  1215,  1216,  1217,
    1218,  1219,  1220,  1221,  1222,  1223,  1224,  1227,  1231,  1239,
    1247,  1256,  1264,  1268,  1270,  1274,  1276,  1278,  1280,  1282,
    1284,  1286,  1288,  1290,  1292,  1294,  1296,  1298,  1300,  1302,
    1304,  1306,  1308,  1313,  1322,  1332,  1340,  1350,  1371,  1391,
    1392,  1394,  1398,  1400,  1404,  1408,  1410,  1414,  1420,  1424,
    1426,  1430,  1434,  1438,  1442,  1446,  1452,  1456,  1460,  1466,
    1471,  1478,  1489,  1502,  1513,  1526,  1536,  1549,  1554,  1561,
    1564,  1569,  1574,  1581,  1584,  1594,  1608,  1611,  1630,  1657,
    1659,  1671,  1679,  1680,  1681,  1682,  1683,  1684,  1685,  1690,
    1691,  1695,  1697,  1704,  1709,  1710,  1712,  1714,  1727,  1733,
    1739,  1748,  1757,  1770,  1771,  1774,  1778,  1793,  1808,  1826,
    1847,  1867,  1889,  1906,  1924,  1931,  1938,  1945,  1958,  1965,
    1972,  1983,  1987,  1989,  1994,  2012,  2023,  2035,  2047,  2061,
    2067,  2074,  2080,  2086,  2094,  2101,  2117,  2120,  2129,  2131,
    2135,  2139,  2159,  2163,  2165,  2169,  2170,  2173,  2175,  2177,
    2179,  2181,  2184,  2187,  2191,  2197,  2201,  2205,  2207,  2212,
    2213,  2217,  2221,  2223,  2227,  2229,  2231,  2236,  2240,  2242,
    2244,  2247,  2249,  2250,  2251,  2252,  2253,  2254,  2255,  2256,
    2259,  2260,  2266,  2269,  2270,  2272,  2276,  2277,  2280,  2281,
    2283,  2287,  2288,  2289,  2290,  2292,  2295,  2296,  2305,  2307,
    2314,  2321,  2328,  2337,  2339,  2341,  2345,  2347,  2351,  2360,
    2367,  2374,  2376,  2380,  2384,  2390,  2392,  2397,  2401,  2405,
    2412,  2419,  2429,  2431,  2435,  2447,  2450,  2459,  2472,  2478,
    2484,  2490,  2498,  2508,  2510,  2514,  2535,  2560,  2562,  2566,
    2598,  2599,  2603,  2603,  2608,  2612,  2620,  2629,  2638,  2648,
    2654,  2657,  2659,  2663,  2671,  2686,  2693,  2695,  2699,  2715,
    2715,  2719,  2721,  2733,  2735,  2739,  2745,  2757,  2769,  2786,
    2815,  2816,  2824,  2825,  2829,  2831,  2833,  2844,  2848,  2854,
    2856,  2860,  2862,  2864,  2868,  2870,  2874,  2876,  2878,  2880,
    2882,  2884,  2886,  2888,  2890,  2892,  2894,  2896,  2898,  2900,
    2902,  2904,  2906,  2908,  2910,  2912,  2914,  2916,  2918,  2922,
    2923,  2934,  3008,  3020,  3022,  3026,  3157,  3207,  3251,  3293,
    3351,  3353,  3355,  3394,  3437,  3448,  3449,  3453,  3458,  3459,
    3463,  3465,  3471,  3473,  3479,  3489,  3495,  3502,  3508,  3516,
    3524,  3540,  3550,  3563,  3570,  3572,  3595,  3597,  3599,  3601,
    3603,  3605,  3607,  3609,  3613,  3613,  3613,  3627,  3629,  3652,
    3654,  3656,  3672,  3674,  3676,  3690,  3693,  3695,  3703,  3705,
    3707,  3709,  3763,  3783,  3798,  3807,  3810,  3860,  3866,  3871,
    3889,  3891,  3893,  3895,  3897,  3900,  3906,  3908,  3910,  3913,
    3915,  3917,  3944,  3953,  3962,  3963,  3965,  3970,  3977,  3985,
    3987,  3991,  3994,  3996,  4000,  4006,  4008,  4010,  4012,  4016,
    4018,  4027,  4028,  4035,  4036,  4040,  4044,  4065,  4068,  4072,
    4074,  4081,  4086,  4087,  4098,  4115,  4138,  4163,  4164,  4171,
    4173,  4175,  4177,  4179,  4183,  4260,  4272,  4279,  4281,  4282,
    4284,  4293,  4300,  4307,  4315,  4320,  4325,  4328,  4331,  4334,
    4337,  4340,  4344,  4362,  4367,  4386,  4405,  4409,  4410,  4413,
    4417,  4422,  4429,  4431,  4433,  4437,  4438,  4449,  4464,  4468,
    4475,  4478,  4488,  4501,  4514,  4517,  4519,  4522,  4525,  4529,
    4538,  4541,  4545,  4547,  4553,  4557,  4559,  4561,  4568,  4572,
    4574,  4578,  4580,  4584,  4603,  4619,  4628,  4637,  4639,  4643,
    4669,  4684,  4699,  4716,  4724,  4733,  4741,  4746,  4751,  4773,
    4789,  4791,  4795,  4797,  4804,  4806,  4808,  4812,  4814,  4816,
    4818,  4820,  4822,  4826,  4829,  4832,  4838,  4844,  4853,  4857,
    4864,  4866,  4870,  4872,  4874,  4879,  4884,  4889,  4894,  4903,
    4908,  4914,  4915,  4930,  4931,  4932,  4933,  4934,  4935,  4936,
    4937,  4938,  4939,  4940,  4941,  4942,  4943,  4944,  4945,  4946,
    4947,  4948,  4951,  4952,  4953,  4954,  4955,  4956,  4957,  4958,
    4959,  4960,  4961,  4962,  4963,  4964,  4965,  4966,  4967,  4968,
    4969,  4970,  4971,  4972,  4973,  4974,  4975,  4976,  4977,  4978,
    4979,  4980,  4981,  4982,  4983,  4984,  4985,  4986,  4987,  4988,
    4989,  4990,  4991,  4992,  4993,  4994,  4998,  5000,  5011,  5032,
    5036,  5038,  5042,  5055,  5059,  5061,  5065,  5076,  5087,  5091,
    5093,  5097,  5099,  5101,  5116,  5128,  5148,  5168,  5190,  5196,
    5205,  5213,  5219,  5227,  5234,  5240,  5249,  5253,  5259,  5267,
    5281,  5295,  5300,  5316,  5331,  5359,  5361,  5365,  5367,  5371,
    5400,  5423,  5444,  5445,  5449,  5470,  5472,  5476,  5484,  5488,
    5493,  5495,  5497,  5499,  5505,  5507,  5511,  5521,  5525,  5527,
    5532,  5534,  5538,  5542,  5548,  5558,  5560,  5564,  5566,  5568,
    5575,  5593,  5594,  5598,  5600,  5604,  5611,  5621,  5650,  5665,
    5672,  5690,  5692,  5696,  5710,  5736,  5749,  5765,  5767,  5770,
    5772,  5778,  5782,  5810,  5812,  5816,  5824,  5830,  5833,  5891,
    5955,  5957,  5960,  5964,  5968,  5972,  5989,  6001,  6005,  6009,
    6019,  6024,  6029,  6036,  6045,  6045,  6056,  6067,  6069,  6073,
    6084,  6088,  6090,  6094,  6105,  6109,  6111,  6115,  6127,  6129,
    6136,  6138,  6142,  6158,  6166,  6177,  6179,  6183,  6186,  6189,
    6194,  6204,  6206,  6210,  6212,  6221,  6222,  6226,  6228,  6233,
    6234,  6235,  6236,  6237,  6238,  6239,  6240,  6241,  6242,  6245,
    6250,  6254,  6258,  6262,  6275,  6279,  6283,  6287,  6290,  6292,
    6294,  6298,  6300,  6304,  6309,  6313,  6317,  6319,  6323,  6331,
    6337,  6344,  6347,  6349,  6353,  6355,  6359,  6371,  6373,  6377,
    6381,  6383,  6387,  6389,  6391,  6393,  6395,  6397,  6399,  6403,
    6407,  6411,  6415,  6419,  6426,  6432,  6437,  6440,  6443,  6456,
    6458,  6462,  6464,  6469,  6475,  6481,  6487,  6493,  6499,  6505,
    6511,  6517,  6526,  6532,  6549,  6551,  6559,  6567,  6569,  6573,
    6577,  6579,  6583,  6585,  6593,  6597,  6609,  6612,  6630,  6632,
    6636,  6638,  6642,  6644,  6648,  6652,  6656,  6665,  6669,  6673,
    6678,  6682,  6694,  6696,  6700,  6705,  6709,  6711,  6715,  6717,
    6721,  6726,  6733,  6756,  6758,  6760,  6762,  6764,  6768,  6779,
    6783,  6798,  6805,  6812,  6813,  6817,  6821,  6829,  6833,  6837,
    6845,  6850,  6864,  6866,  6870,  6872,  6881,  6883,  6885,  6887,
    6923,  6927,  6931,  6935,  6939,  6951,  6953,  6957,  6960,  6962,
    6966,  6971,  6978,  6981,  6989,  6993,  6998,  7000,  7007,  7012,
    7016,  7020,  7024,  7028,  7032,  7035,  7037,  7041,  7043,  7045,
    7049,  7053,  7065,  7067,  7071,  7073,  7077,  7080,  7083,  7087,
    7093,  7105,  7107,  7111,  7113,  7117,  7125,  7137,  7138,  7140,
    7144,  7148,  7150,  7158,  7162,  7165,  7167,  7171,  7175,  7177,
    7180,  7182,  7183,  7184,  7185,  7186,  7187,  7188,  7189,  7190,
    7191,  7192,  7193,  7194,  7195,  7196,  7197,  7198,  7199,  7200,
    7201,  7202,  7203,  7204,  7205,  7206,  7207,  7210,  7216,  7222,
    7228,  7234,  7238,  7244,  7245,  7246,  7247,  7248,  7249,  7250,
    7251,  7252,  7255,  7260,  7265,  7271,  7277,  7283,  7288,  7294,
    7300,  7306,  7313,  7319,  7325,  7332,  7336,  7338,  7344,  7351,
    7357,  7363,  7369,  7375,  7381,  7387,  7393,  7399,  7405,  7411,
    7417,  7427,  7432,  7438,  7442,  7448,  7449,  7450,  7451,  7454,
    7462,  7468,  7474,  7479,  7485,  7492,  7498,  7502,  7508,  7509,
    7510,  7511,  7512,  7513,  7516,  7525,  7529,  7535,  7542,  7549,
    7556,  7565,  7571,  7577,  7581,  7587,  7588,  7591,  7597,  7603,
    7607,  7614,  7615,  7618,  7624,  7630,  7635,  7643,  7649,  7654,
    7661,  7665,  7671,  7672,  7673,  7674,  7675,  7676,  7677,  7678,
    7679,  7680,  7681,  7685,  7690,  7695,  7702,  7707,  7713,  7719,
    7724,  7729,  7734,  7738,  7743,  7748,  7752,  7757,  7761,  7767,
    7772,  7778,  7783,  7789,  7799,  7803,  7807,  7811,  7817,  7820,
    7824,  7825,  7826,  7827,  7828,  7829,  7832,  7836,  7840,  7842,
    7844,  7848,  7850,  7852,  7856,  7858,  7862,  7864,  7868,  7871,
    7874,  7879,  7881,  7883,  7885,  7887,  7891,  7895,  7900,  7904,
    7906,  7910,  7912,  7916,  7920,  7924,  7925,  7926,  7927,  7928,
    7931,  7935,  7939,  7943,  7947,  7951,  7953,  7957,  7958,  7961,
    7965,  7969,  7971,  7975,  7976,  7977,  7981,  7985,  7989,  7993,
    7995,  7999,  8001,  8003,  8005,  8008
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PERCENT", "AMPERSAND", "ASTER",
  "CLUSTER", "COLON", "COMMA", "DASTER", "DEFINED_OPERATOR", "DOT",
  "DQUOTE", "GLOBAL_A", "LEFTAB", "LEFTPAR", "MINUS", "PLUS", "POINT_TO",
  "QUOTE", "RIGHTAB", "RIGHTPAR", "AND", "DSLASH", "EQV", "EQ", "EQUAL",
  "FFALSE", "GE", "GT", "LE", "LT", "NE", "NEQV", "NOT", "OR", "TTRUE",
  "SLASH", "XOR", "REFERENCE", "AT", "ACROSS", "ALIGN_WITH", "ALIGN",
  "ALLOCATABLE", "ALLOCATE", "ARITHIF", "ASSIGNMENT", "ASSIGN",
  "ASSIGNGOTO", "ASYNCHRONOUS", "ASYNCID", "ASYNCWAIT", "BACKSPACE",
  "BAD_CCONST", "BAD_SYMBOL", "BARRIER", "BLOCKDATA", "BLOCK",
  "BOZ_CONSTANT", "BYTE", "CALL", "CASE", "CHARACTER", "CHAR_CONSTANT",
  "CHECK", "CLOSE", "COMMON", "COMPLEX", "COMPGOTO", "CONSISTENT_GROUP",
  "CONSISTENT_SPEC", "CONSISTENT_START", "CONSISTENT_WAIT", "CONSISTENT",
  "CONSTRUCT_ID", "CONTAINS", "CONTINUE", "CORNER", "CYCLE", "DATA",
  "DEALLOCATE", "HPF_TEMPLATE", "DEBUG", "DEFAULT_CASE", "DEFINE",
  "DERIVED", "DIMENSION", "DISTRIBUTE", "DOWHILE", "DOUBLEPRECISION",
  "DOUBLECOMPLEX", "DP_CONSTANT", "DVM_POINTER", "DYNAMIC", "ELEMENTAL",
  "ELSE", "ELSEIF", "ELSEWHERE", "ENDASYNCHRONOUS", "ENDDEBUG",
  "ENDINTERVAL", "ENDUNIT", "ENDDO", "ENDFILE", "ENDFORALL", "ENDIF",
  "ENDINTERFACE", "ENDMODULE", "ENDON", "ENDSELECT", "ENDTASK_REGION",
  "ENDTYPE", "ENDWHERE", "ENTRY", "EXIT", "EOLN", "EQUIVALENCE", "ERROR",
  "EXTERNAL", "F90", "FIND", "FORALL", "FORMAT", "FUNCTION", "GATE",
  "GEN_BLOCK", "HEAP", "HIGH", "IDENTIFIER", "IMPLICIT", "IMPLICITNONE",
  "INCLUDE_TO", "INCLUDE", "INDEPENDENT", "INDIRECT_ACCESS",
  "INDIRECT_GROUP", "INDIRECT", "INHERIT", "INQUIRE",
  "INTERFACEASSIGNMENT", "INTERFACEOPERATOR", "INTERFACE", "INTRINSIC",
  "INTEGER", "INTENT", "INTERVAL", "INOUT", "IN", "INT_CONSTANT", "LABEL",
  "LABEL_DECLARE", "LET", "LOCALIZE", "LOGICAL", "LOGICALIF", "LOOP",
  "LOW", "MAXLOC", "MAX", "MAP", "MINLOC", "MIN", "MODULE_PROCEDURE",
  "MODULE", "MULT_BLOCK", "NAMEEQ", "NAMELIST", "NEW_VALUE", "NEW",
  "NULLIFY", "OCTAL_CONSTANT", "ONLY", "ON", "ON_DIR", "ONTO", "OPEN",
  "OPERATOR", "OPTIONAL", "OTHERWISE", "OUT", "OWN", "PARALLEL",
  "PARAMETER", "PAUSE", "PLAINDO", "PLAINGOTO", "POINTER", "POINTERLET",
  "PREFETCH", "PRINT", "PRIVATE", "PRODUCT", "PROGRAM", "PUBLIC", "PURE",
  "RANGE", "READ", "REALIGN_WITH", "REALIGN", "REAL", "REAL_CONSTANT",
  "RECURSIVE", "REDISTRIBUTE_NEW", "REDISTRIBUTE", "REDUCTION_GROUP",
  "REDUCTION_START", "REDUCTION_WAIT", "REDUCTION", "REMOTE_ACCESS_SPEC",
  "REMOTE_ACCESS", "REMOTE_GROUP", "RESET", "RESULT", "RETURN", "REWIND",
  "SAVE", "SECTION", "SELECT", "SEQUENCE", "SHADOW_ADD", "SHADOW_COMPUTE",
  "SHADOW_GROUP", "SHADOW_RENEW", "SHADOW_START_SPEC", "SHADOW_START",
  "SHADOW_WAIT_SPEC", "SHADOW_WAIT", "SHADOW", "STAGE", "STATIC", "STAT",
  "STOP", "SUBROUTINE", "SUM", "SYNC", "TARGET", "TASK", "TASK_REGION",
  "THEN", "TO", "TRACEON", "TRACEOFF", "TRUNC", "TYPE", "TYPE_DECL",
  "UNDER", "UNKNOWN", "USE", "VIRTUAL", "VARIABLE", "WAIT", "WHERE",
  "WHERE_ASSIGN", "WHILE", "WITH", "WRITE", "COMMENT", "WGT_BLOCK",
  "HPF_PROCESSORS", "IOSTAT", "ERR", "END", "OMPDVM_ATOMIC",
  "OMPDVM_BARRIER", "OMPDVM_COPYIN", "OMPDVM_COPYPRIVATE",
  "OMPDVM_CRITICAL", "OMPDVM_ONETHREAD", "OMPDVM_DO", "OMPDVM_DYNAMIC",
  "OMPDVM_ENDCRITICAL", "OMPDVM_ENDDO", "OMPDVM_ENDMASTER",
  "OMPDVM_ENDORDERED", "OMPDVM_ENDPARALLEL", "OMPDVM_ENDPARALLELDO",
  "OMPDVM_ENDPARALLELSECTIONS", "OMPDVM_ENDPARALLELWORKSHARE",
  "OMPDVM_ENDSECTIONS", "OMPDVM_ENDSINGLE", "OMPDVM_ENDWORKSHARE",
  "OMPDVM_FIRSTPRIVATE", "OMPDVM_FLUSH", "OMPDVM_GUIDED",
  "OMPDVM_LASTPRIVATE", "OMPDVM_MASTER", "OMPDVM_NOWAIT", "OMPDVM_NONE",
  "OMPDVM_NUM_THREADS", "OMPDVM_ORDERED", "OMPDVM_PARALLEL",
  "OMPDVM_PARALLELDO", "OMPDVM_PARALLELSECTIONS",
  "OMPDVM_PARALLELWORKSHARE", "OMPDVM_RUNTIME", "OMPDVM_SECTION",
  "OMPDVM_SECTIONS", "OMPDVM_SCHEDULE", "OMPDVM_SHARED", "OMPDVM_SINGLE",
  "OMPDVM_THREADPRIVATE", "OMPDVM_WORKSHARE", "OMPDVM_NODES", "OMPDVM_IF",
  "IAND", "IEOR", "IOR", "ACC_REGION", "ACC_END_REGION",
  "ACC_CHECKSECTION", "ACC_END_CHECKSECTION", "ACC_GET_ACTUAL",
  "ACC_ACTUAL", "ACC_TARGETS", "ACC_ASYNC", "ACC_HOST", "ACC_CUDA",
  "ACC_LOCAL", "ACC_INLOCAL", "ACC_CUDA_BLOCK", "BY", "IO_MODE",
  "CP_CREATE", "CP_LOAD", "CP_SAVE", "CP_WAIT", "FILES", "VARLIST",
  "STATUS", "EXITINTERVAL", "TEMPLATE_CREATE", "TEMPLATE_DELETE",
  "SPF_ANALYSIS", "SPF_PARALLEL", "SPF_TRANSFORM", "SPF_NOINLINE",
  "SPF_PARALLEL_REG", "SPF_END_PARALLEL_REG", "SPF_PRIVATES_EXPANSION",
  "SPF_FISSION", "BINARY_OP", "UNARY_OP", "$accept", "program", "stat",
  "thislabel", "entry", "new_prog", "proc_attr", "procname", "funcname",
  "typedfunc", "opt_result_clause", "name", "progname", "blokname",
  "arglist", "args", "arg", "filename", "needkeyword", "keywordoff",
  "keyword_if_colon_follow", "spec", "interface", "defined_op", "operator",
  "intrinsic_op", "type_dcl", "end_type", "dcl", "options",
  "attr_spec_list", "attr_spec", "intent_spec", "access_spec", "intent",
  "optional", "static", "private", "private_attr", "sequence", "public",
  "public_attr", "type", "opt_key_hedr", "attrib", "att_type", "typespec",
  "typename", "lengspec", "proper_lengspec", "selector", "clause",
  "end_ioctl", "initial_value", "dimension", "allocatable", "pointer",
  "target", "common", "namelist", "namelist_group", "comblock", "var",
  "external", "intrinsic", "equivalence", "equivset", "equivlist",
  "equi_object", "data", "data1", "data_in", "in_data", "datapair",
  "datalvals", "datarvals", "datalval", "data_null", "d_name", "dataname",
  "datasubs", "datarange", "iconexprlist", "opticonexpr", "dataimplieddo",
  "dlist", "dataelt", "datarval", "datavalue", "BOZ_const", "int_const",
  "unsignedint", "real_const", "unsignedreal", "complex_const_data",
  "complex_part", "iconexpr", "iconterm", "iconfactor", "iconprimary",
  "savelist", "saveitem", "use_name_list", "use_key_word",
  "no_use_key_word", "use_name", "paramlist", "paramitem",
  "module_proc_stmt", "proc_name_list", "use_stat", "module_name",
  "only_list", "only_name", "rename_list", "rename_name", "dims",
  "dimlist", "$@1", "dim", "ubound", "labellist", "label", "implicit",
  "implist", "impitem", "imptype", "$@2", "type_implicit", "letgroups",
  "letgroup", "letter", "inside", "in_dcl", "opt_double_colon",
  "funarglist", "funarg", "funargs", "subscript_list", "expr", "uexpr",
  "addop", "ident", "lhs", "array_ele_substring_func_ref",
  "structure_component", "array_element", "asubstring", "opt_substring",
  "substring", "opt_expr", "simple_const", "numeric_bool_const",
  "integer_constant", "string_constant", "complex_const", "kind",
  "triplet", "vec", "$@3", "$@4", "allocate_object", "allocation_list",
  "allocate_object_list", "stat_spec", "pointer_name_list", "exec",
  "do_while", "opt_while", "plain_do", "case", "case_selector",
  "case_value_range", "case_value_range_list", "opt_construct_name",
  "opt_unit_name", "construct_name", "construct_name_colon", "logif",
  "forall", "forall_list", "forall_expr", "opt_forall_cond", "do_var",
  "dospec", "dotarget", "whereable", "iffable", "let", "goto", "opt_comma",
  "call", "callarglist", "callarg", "stop", "end_spec", "intonlyon",
  "intonlyoff", "io", "iofmove", "fmkwd", "iofctl", "ctlkwd", "inquire",
  "infmt", "ioctl", "ctllist", "ioclause", "nameeq", "read", "write",
  "print", "inlist", "inelt", "outlist", "out2", "other", "in_ioctl",
  "start_ioctl", "fexpr", "unpar_fexpr", "cmnt", "dvm_specification",
  "dvm_exec", "dvm_template", "template_obj", "dvm_dynamic",
  "dyn_array_name_list", "dyn_array_name", "dvm_inherit",
  "dummy_array_name_list", "dummy_array_name", "dvm_shadow",
  "shadow_attr_stuff", "sh_width_list", "sh_width", "sh_array_name",
  "dvm_processors", "dvm_indirect_group", "indirect_group_name",
  "dvm_remote_group", "remote_group_name", "dvm_reduction_group",
  "reduction_group_name", "dvm_consistent_group", "consistent_group_name",
  "opt_onto", "dvm_distribute", "dvm_redistribute", "dist_name_list",
  "distributee", "dist_name", "pointer_ar_elem", "processors_name",
  "opt_dist_format_clause", "dist_format_clause", "dist_format_list",
  "opt_key_word", "dist_format", "array_name", "derived_spec",
  "derived_elem_list", "derived_elem", "target_spec", "derived_target",
  "derived_subscript_list", "derived_subscript", "dummy_ident",
  "opt_plus_shadow", "plus_shadow", "shadow_id", "shadow_width",
  "dvm_align", "dvm_realign", "realignee_list", "alignee", "realignee",
  "align_directive_stuff", "align_base", "align_subscript_list",
  "align_subscript", "align_base_name", "dim_ident_list", "dim_ident",
  "dvm_combined_dir", "dvm_attribute_list", "dvm_attribute", "dvm_pointer",
  "dimension_list", "$@5", "pointer_var_list", "pointer_var", "dvm_heap",
  "heap_array_name_list", "heap_array_name", "dvm_consistent",
  "consistent_array_name_list", "consistent_array_name", "dvm_asyncid",
  "async_id_list", "async_id", "dvm_new_value", "dvm_parallel_on",
  "loop_var_list", "loop_var", "opt_on", "distribute_cycles",
  "par_subscript_list", "par_subscript", "opt_spec", "spec_list",
  "par_spec", "remote_access_spec", "consistent_spec", "consistent_group",
  "new_spec", "private_spec", "cuda_block_spec", "sizelist",
  "variable_list", "indirect_access_spec", "stage_spec", "across_spec",
  "in_out_across", "opt_in_out", "dependent_array_list", "dependent_array",
  "dependence_list", "dependence", "section_spec_list", "section_spec",
  "ar_section", "low_section", "high_section", "section", "reduction_spec",
  "opt_key_word_r", "no_opt_key_word_r", "reduction_group",
  "reduction_list", "reduction", "reduction_op", "loc_op", "shadow_spec",
  "shadow_group_name", "shadow_list", "shadow", "opt_corner",
  "array_ident", "array_ident_list", "dvm_shadow_start", "dvm_shadow_wait",
  "dvm_shadow_group", "dvm_reduction_start", "dvm_reduction_wait",
  "dvm_consistent_start", "dvm_consistent_wait", "dvm_remote_access",
  "group_name", "remote_data_list", "remote_data", "remote_index_list",
  "remote_index", "dvm_task", "task_array", "dvm_task_region", "task_name",
  "dvm_end_task_region", "task", "dvm_on", "opt_new_spec", "dvm_end_on",
  "dvm_map", "dvm_prefetch", "dvm_reset", "dvm_indirect_access",
  "indirect_list", "indirect_reference", "hpf_independent",
  "hpf_reduction_spec", "dvm_asynchronous", "dvm_endasynchronous",
  "dvm_asyncwait", "async_ident", "async", "dvm_f90", "dvm_debug_dir",
  "debparamlist", "debparam", "fragment_number", "dvm_enddebug_dir",
  "dvm_interval_dir", "interval_number", "dvm_exit_interval_dir",
  "dvm_endinterval_dir", "dvm_traceon_dir", "dvm_traceoff_dir",
  "dvm_barrier_dir", "dvm_check", "dvm_io_mode_dir", "mode_list",
  "mode_spec", "dvm_shadow_add", "template_ref", "shadow_axis_list",
  "shadow_axis", "opt_include_to", "dvm_localize", "localize_target",
  "target_subscript_list", "target_subscript", "aster_expr",
  "dvm_cp_create", "opt_mode", "dvm_cp_load", "dvm_cp_save", "dvm_cp_wait",
  "dvm_template_create", "template_list", "dvm_template_delete",
  "template_ident_list", "omp_specification_directive",
  "omp_execution_directive", "ompdvm_onethread",
  "omp_parallel_end_directive", "omp_parallel_begin_directive",
  "parallel_clause_list", "parallel_clause", "omp_variable_list_in_par",
  "ompprivate_clause", "ompfirstprivate_clause", "omplastprivate_clause",
  "ompcopyin_clause", "ompshared_clause", "ompdefault_clause", "def_expr",
  "ompif_clause", "ompnumthreads_clause", "ompreduction_clause",
  "ompreduction", "ompreduction_vars", "ompreduction_op",
  "omp_sections_begin_directive", "sections_clause_list",
  "sections_clause", "omp_sections_end_directive", "omp_section_directive",
  "omp_do_begin_directive", "omp_do_end_directive", "do_clause_list",
  "do_clause", "ompordered_clause", "ompschedule_clause", "ompschedule_op",
  "omp_single_begin_directive", "single_clause_list", "single_clause",
  "omp_single_end_directive", "end_single_clause_list",
  "end_single_clause", "ompcopyprivate_clause", "ompnowait_clause",
  "omp_workshare_begin_directive", "omp_workshare_end_directive",
  "omp_parallel_do_begin_directive", "paralleldo_clause_list",
  "paralleldo_clause", "omp_parallel_do_end_directive",
  "omp_parallel_sections_begin_directive",
  "omp_parallel_sections_end_directive",
  "omp_parallel_workshare_begin_directive",
  "omp_parallel_workshare_end_directive", "omp_threadprivate_directive",
  "omp_master_begin_directive", "omp_master_end_directive",
  "omp_ordered_begin_directive", "omp_ordered_end_directive",
  "omp_barrier_directive", "omp_atomic_directive", "omp_flush_directive",
  "omp_critical_begin_directive", "omp_critical_end_directive",
  "omp_common_var", "omp_variable_list", "op_slash_1", "op_slash_0",
  "acc_directive", "acc_region", "acc_checksection", "acc_get_actual",
  "acc_actual", "opt_clause", "acc_clause_list", "acc_clause",
  "data_clause", "targets_clause", "async_clause", "acc_var_list",
  "computer_list", "computer", "acc_end_region", "acc_end_checksection",
  "spf_directive", "spf_analysis", "spf_parallel", "spf_transform",
  "spf_parallel_reg", "spf_end_parallel_reg", "analysis_spec_list",
  "analysis_spec", "analysis_reduction_spec", "analysis_private_spec",
  "parallel_spec_list", "parallel_spec", "parallel_shadow_spec",
  "parallel_across_spec", "parallel_remote_access_spec",
  "transform_spec_list", "transform_spec", "region_name", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   339,   340,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   341,   342
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   343,   344,   344,   345,   345,   345,   345,   345,   345,
     345,   346,   347,   347,   347,   347,   347,   347,   347,   347,
     348,   349,   349,   349,   350,   351,   352,   352,   352,   352,
     353,   353,   354,   355,   355,   356,   356,   357,   357,   357,
     358,   358,   359,   359,   360,   361,   362,   363,   364,   364,
     364,   364,   364,   364,   364,   364,   364,   364,   364,   364,
     364,   364,   364,   364,   364,   364,   364,   364,   364,   364,
     364,   364,   364,   364,   364,   364,   364,   365,   365,   365,
     365,   365,   366,   367,   367,   368,   368,   368,   368,   368,
     368,   368,   368,   368,   368,   368,   368,   368,   368,   368,
     368,   368,   368,   369,   369,   370,   370,   371,   371,   372,
     372,   372,   373,   373,   374,   374,   374,   374,   374,   374,
     374,   374,   374,   374,   374,   374,   375,   375,   375,   376,
     376,   377,   377,   378,   378,   379,   379,   380,   380,   381,
     382,   383,   383,   384,   385,   385,   386,   387,   387,   388,
     388,   389,   390,   390,   390,   390,   390,   390,   390,   391,
     391,   392,   392,   392,   393,   393,   393,   393,   394,   394,
     394,   394,   395,   396,   396,   396,   397,   397,   398,   398,
     399,   399,   400,   400,   401,   401,   401,   401,   402,   402,
     402,   403,   404,   404,   405,   406,   406,   407,   407,   408,
     408,   409,   410,   410,   411,   411,   411,   412,   413,   413,
     414,   415,   416,   417,   417,   418,   418,   419,   419,   419,
     419,   419,   420,   421,   422,   423,   424,   425,   425,   426,
     426,   427,   428,   428,   429,   429,   429,   429,   430,   430,
     430,   431,   431,   431,   431,   431,   431,   431,   431,   431,
     431,   431,   432,   433,   433,   433,   434,   434,   435,   435,
     435,   436,   436,   436,   436,   437,   438,   438,   439,   439,
     439,   439,   439,   440,   440,   440,   441,   441,   442,   442,
     442,   443,   443,   444,   444,   445,   445,   446,   447,   448,
     448,   448,   449,   449,   450,   451,   452,   452,   453,   453,
     453,   453,   454,   455,   455,   456,   456,   457,   457,   458,
     459,   459,   461,   460,   460,   462,   462,   462,   462,   463,
     463,   464,   464,   465,   466,   466,   467,   467,   468,   470,
     469,   471,   471,   472,   472,   473,   473,   474,   475,   476,
     477,   477,   478,   478,   479,   479,   479,   480,   480,   481,
     481,   482,   482,   482,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   483,   483,   483,   483,   484,
     484,   485,   486,   486,   486,   487,   487,   487,   487,   488,
     489,   489,   489,   489,   490,   491,   491,   492,   493,   493,
     494,   494,   494,   494,   494,   495,   495,   495,   495,   496,
     497,   497,   497,   498,   499,   499,   500,   500,   500,   500,
     500,   500,   500,   500,   502,   503,   501,   504,   504,   505,
     505,   505,   506,   506,   506,   507,   508,   508,   509,   509,
     509,   509,   509,   509,   509,   509,   509,   509,   509,   509,
     509,   509,   509,   509,   509,   509,   509,   509,   509,   509,
     509,   509,   510,   510,   511,   511,   511,   512,   512,   513,
     513,   513,   513,   513,   514,   515,   515,   515,   515,   516,
     516,   517,   517,   518,   518,   519,   520,   521,   522,   523,
     523,   524,   525,   525,   526,   527,   527,   528,   528,   529,
     529,   529,   529,   529,   530,   530,   530,   530,   530,   530,
     530,   530,   530,   530,   530,   530,   530,   530,   530,   530,
     530,   530,   531,   532,   532,   532,   532,   533,   533,   534,
     535,   535,   536,   536,   536,   537,   537,   538,   539,   540,
     541,   541,   541,   541,   541,   541,   541,   541,   541,   541,
     541,   541,   541,   541,   542,   543,   543,   543,   544,   545,
     545,   546,   546,   547,   547,   548,   548,   549,   549,   550,
     550,   550,   550,   550,   550,   551,   552,   553,   554,   554,
     555,   555,   556,   556,   557,   557,   557,   558,   558,   558,
     558,   558,   558,   559,   559,   559,   559,   559,   560,   561,
     562,   562,   563,   563,   563,   563,   563,   563,   563,   563,
     563,   564,   564,   565,   565,   565,   565,   565,   565,   565,
     565,   565,   565,   565,   565,   565,   565,   565,   565,   565,
     565,   565,   566,   566,   566,   566,   566,   566,   566,   566,
     566,   566,   566,   566,   566,   566,   566,   566,   566,   566,
     566,   566,   566,   566,   566,   566,   566,   566,   566,   566,
     566,   566,   566,   566,   566,   566,   566,   566,   566,   566,
     566,   566,   566,   566,   566,   566,   567,   567,   568,   569,
     570,   570,   571,   572,   573,   573,   574,   575,   576,   577,
     577,   578,   578,   578,   579,   580,   580,   580,   581,   581,
     582,   583,   583,   584,   585,   585,   586,   587,   587,   588,
     589,   589,   590,   591,   591,   592,   592,   593,   593,   594,
     595,   596,   597,   597,   598,   599,   599,   600,   601,   601,
     601,   601,   601,   601,   601,   601,   602,   603,   604,   604,
     605,   605,   606,   606,   607,   608,   608,   609,   609,   609,
     610,   611,   611,   612,   612,   613,   614,   614,   615,   616,
     616,   617,   617,   618,   619,   620,   621,   622,   622,   623,
     623,   623,   624,   625,   625,   626,   626,   626,   627,   627,
     628,   628,   629,   629,   629,   629,   629,   629,   629,   629,
     629,   629,   629,   630,   632,   631,   631,   633,   633,   634,
     635,   636,   636,   637,   638,   639,   639,   640,   641,   641,
     642,   642,   643,   644,   645,   646,   646,   647,   648,   648,
     649,   650,   650,   651,   651,   652,   652,   653,   653,   654,
     654,   654,   654,   654,   654,   654,   654,   654,   654,   655,
     655,   656,   656,   657,   658,   658,   659,   660,   661,   661,
     661,   662,   662,   663,   663,   664,   665,   665,   666,   667,
     667,   667,   668,   668,   669,   669,   669,   670,   670,   671,
     672,   672,   673,   673,   673,   673,   673,   673,   673,   674,
     675,   676,   677,   678,   678,   678,   679,   680,   681,   682,
     682,   683,   683,   684,   684,   684,   684,   684,   684,   684,
     684,   684,   685,   685,   686,   686,   686,   686,   686,   687,
     688,   688,   689,   689,   689,   689,   690,   691,   692,   692,
     693,   693,   694,   694,   695,   696,   697,   698,   699,   700,
     700,   701,   702,   702,   703,   703,   704,   704,   705,   705,
     706,   706,   707,   708,   708,   708,   708,   708,   709,   710,
     711,   711,   712,   713,   713,   714,   715,   715,   716,   717,
     718,   718,   719,   719,   720,   720,   721,   721,   721,   721,
     722,   723,   724,   725,   726,   727,   727,   728,   729,   729,
     730,   730,   731,   732,   733,   734,   735,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   743,   744,   744,   744,
     745,   746,   747,   747,   748,   748,   749,   749,   750,   751,
     751,   752,   752,   753,   753,   754,   755,   756,   756,   756,
     757,   758,   758,   759,   760,   761,   761,   762,   763,   763,
     764,   765,   765,   765,   765,   765,   765,   765,   765,   765,
     765,   765,   765,   765,   765,   765,   765,   765,   765,   765,
     765,   765,   765,   765,   765,   765,   765,   766,   767,   768,
     768,   769,   769,   770,   770,   770,   770,   770,   770,   770,
     770,   770,   771,   772,   773,   774,   775,   776,   777,   778,
     778,   778,   779,   780,   781,   782,   783,   784,   784,   784,
     784,   784,   784,   784,   784,   784,   784,   784,   784,   784,
     784,   785,   785,   786,   786,   787,   787,   787,   787,   788,
     788,   789,   790,   790,   791,   791,   792,   792,   793,   793,
     793,   793,   793,   793,   794,   795,   795,   796,   796,   796,
     796,   797,   797,   798,   798,   799,   799,   800,   800,   801,
     801,   802,   802,   803,   804,   805,   806,   806,   807,   807,
     808,   808,   809,   809,   809,   809,   809,   809,   809,   809,
     809,   809,   809,   810,   811,   811,   812,   813,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   822,   823,
     823,   824,   824,   825,   826,   826,   826,   826,   827,   828,
     829,   829,   829,   829,   829,   829,   830,   831,   832,   832,
     832,   833,   833,   833,   834,   834,   835,   835,   836,   836,
     836,   837,   837,   837,   837,   837,   838,   839,   840,   841,
     841,   842,   842,   843,   844,   845,   845,   845,   845,   845,
     846,   847,   848,   849,   850,   851,   851,   852,   852,   853,
     854,   855,   855,   856,   856,   856,   857,   858,   859,   860,
     860,   861,   861,   861,   861,   862
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     3,     3,     3,     3,     2,     1,
       1,     1,     3,     3,     4,     5,     5,     3,     4,     3,
       0,     2,     2,     2,     1,     1,     4,     5,     4,     5,
       2,     5,     1,     0,     1,     0,     1,     0,     2,     3,
       1,     3,     1,     1,     1,     0,     0,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     4,     2,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     5,
       5,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     7,     2,     3,     7,     6,     0,
       2,     5,     1,     4,     1,     1,     1,     2,     1,     4,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     1,
       1,     7,     3,     4,     3,     4,     3,     2,     5,     0,
       2,     2,     5,     0,     4,     5,     0,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     5,     6,     6,     0,     1,     5,     9,     1,     1,
       2,     2,     0,     0,     2,     2,     5,     4,     4,     3,
       4,     3,     4,     3,     3,     4,     5,     3,     4,     5,
       3,     3,     1,     3,     2,     4,     3,     4,     3,     3,
       3,     3,     3,     3,     1,     4,     1,     1,     4,     3,
       0,     0,     4,     1,     3,     1,     3,     2,     3,     3,
       4,     2,     0,     1,     1,     3,     5,     1,     3,     0,
       1,     7,     1,     3,     2,     2,     3,     1,     1,     4,
       3,     2,     1,     1,     1,     1,     3,     1,     3,     1,
       1,     6,     1,     1,     2,     2,     1,     3,     1,     2,
       2,     1,     3,     1,     3,     5,     1,     1,     1,     2,
       2,     3,     3,     1,     3,     3,     1,     3,     1,     1,
       3,     1,     3,     1,     1,     3,     5,     0,     0,     1,
       4,     4,     1,     3,     3,     2,     1,     3,     3,     6,
       6,     7,     1,     1,     3,     3,     1,     1,     3,     3,
       0,     3,     0,     2,     3,     1,     1,     2,     3,     1,
       1,     1,     3,     1,     3,     1,     1,     3,     4,     0,
       2,     2,     1,     1,     3,     1,     3,     1,     0,     0,
       0,     2,     0,     1,     1,     1,     2,     2,     4,     1,
       3,     1,     3,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     1,
       1,     1,     1,     1,     1,     5,     6,     4,     5,     3,
       1,     1,     5,     4,     2,     0,     1,     5,     0,     1,
       1,     3,     1,     3,     2,     1,     1,     1,     1,     1,
       1,     3,     3,     5,     1,     1,     3,     2,     5,     4,
       4,     3,     2,     1,     0,     0,     6,     1,     1,     1,
       4,     5,     1,     4,     5,     0,     1,     3,     1,     1,
       1,     2,     3,     3,     2,     1,     2,     2,     2,     3,
       7,     3,     3,     1,     2,     2,     1,     2,     3,     1,
       1,     1,     5,     7,     0,     6,     4,    11,    13,     4,
       3,     3,     7,     8,     3,     1,     2,     2,     3,     1,
       3,     0,     1,     0,     1,     1,     2,     5,     6,     1,
       3,     3,     0,     2,     1,     5,     7,     0,     1,     3,
       3,     6,     5,     6,     4,     5,     5,     2,     1,     1,
      10,     1,     3,     4,     3,     3,     3,     3,     6,     6,
       5,     8,     2,     3,     3,     7,     7,     0,     1,     4,
       2,     4,     1,     2,     2,     1,     1,     0,     0,     0,
       2,     2,     2,     2,     2,     1,     2,     2,     3,     4,
       2,     3,     1,     3,     3,     1,     1,     1,     3,     1,
       1,     4,     5,     1,     1,     3,     3,     1,     4,     1,
       1,     1,     2,     2,     2,     1,     3,     3,     4,     4,
       1,     3,     1,     5,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     1,     3,     5,     5,     5,     0,     0,
       1,     3,     1,     1,     3,     3,     3,     3,     2,     3,
       3,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     2,     3,
       1,     3,     1,     3,     1,     3,     1,     4,     3,     1,
       3,     1,     3,     4,     1,     4,     4,     4,     3,     3,
       1,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     0,     5,     6,     8,     1,     3,     1,     1,     1,
       4,     1,     0,     2,     3,     2,     4,     0,     1,     5,
       4,     6,     4,     1,     4,     4,     1,     6,     1,     3,
       1,     3,     1,     4,     1,     1,     3,     1,     1,     3,
       1,     0,     1,     2,     3,     1,     2,     5,     4,     4,
       6,     1,     3,     1,     1,     6,     4,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     6,     4,
       1,     4,     1,     1,     1,     1,     4,     2,     7,     1,
       4,     1,     1,    11,     0,     2,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     8,
       1,     3,     2,     2,     7,     1,     3,     1,     3,     1,
       4,     1,     3,     1,     1,     0,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     8,
       6,     8,     6,     1,     6,     6,     6,     6,     1,     3,
       5,     1,     3,     8,     6,     6,     4,     5,     5,     2,
       2,     0,     1,     3,     1,     4,     7,     1,     3,     3,
       1,     3,     5,     3,     3,     1,     3,     1,     1,     3,
       3,     3,     3,    10,     8,    10,     0,     0,     1,     2,
       4,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     4,     4,     3,     9,     1,
       1,     3,     1,     5,     5,     9,     0,     1,     1,     3,
       3,     3,     3,     3,     6,     3,     3,     3,     3,     7,
       5,     1,     1,     3,     4,     1,     1,     3,     1,     1,
       3,     3,     2,     3,     4,     4,     5,     5,     1,     2,
       4,     4,     4,     0,     1,     2,     7,     6,     3,     3,
       7,     5,     1,     3,     1,     4,     2,     3,     3,     4,
       6,     3,     2,     3,     1,     1,     4,     5,     3,     6,
       2,     4,     2,     1,     3,     3,     0,     1,     3,     2,
       2,     2,     2,     9,     5,     1,     3,     2,     2,     2,
       9,     4,     1,     3,     1,     1,     2,     0,     7,     1,
       4,     1,     3,     1,     1,     1,    16,     0,     3,     3,
       3,     3,     6,     9,     5,     1,     3,     5,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     4,
       3,     4,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     2,     2,     2,     2,     2,     5,     1,
       1,     1,     4,     4,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     3,     4,     5,     1,     1,     1,     1,     4,
       3,     2,     4,     3,     4,     3,     4,     5,     1,     1,
       1,     1,     1,     1,     1,     7,     5,     1,     1,     1,
       1,     4,     3,     4,     5,     1,     1,     4,     3,     4,
       5,     1,     1,     2,     1,     2,     4,     3,     4,     3,
       4,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     4,     3,     2,     4,     3,     2,
       3,     2,     2,     2,     2,     2,     2,     3,     2,     5,
       2,     5,     2,     5,     1,     1,     3,     3,     0,     0,
       1,     1,     1,     1,     1,     1,     3,     2,     5,     4,
       2,     5,     4,     2,     2,     1,     1,     3,     2,     2,
       2,     4,     4,     4,     4,     4,     4,     1,     1,     1,
       3,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     4,     2,     1,     1,     3,     1,     1,     5,
       5,     1,     3,     1,     1,     1,     5,     5,     5,     1,
       3,     2,     5,     2,     5,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     1,    10,    11,     9,     0,     0,     3,   150,
     149,   789,   339,   537,   537,   537,   537,   537,   339,   537,
     555,   537,    20,   537,   537,   158,   537,   560,   339,   154,
     537,   339,   537,   537,   339,   485,   537,   537,   537,   338,
     537,   782,   537,   537,   340,   791,   537,   155,   156,   785,
      45,   537,   537,   537,   537,   537,   537,   537,   537,   557,
     537,   537,   483,   483,   537,   537,   537,   339,   537,     0,
     537,   339,   339,   537,   537,   338,    20,   339,   339,   325,
       0,   537,   537,   339,   339,   537,   339,   339,   339,   339,
     152,   339,   537,   211,   537,   157,   537,   537,     0,    20,
     339,   537,   537,   537,   559,   339,   537,   339,   535,   537,
     537,   339,   537,   537,   537,   339,    20,   339,    45,   537,
     537,   153,    45,   537,   339,   537,   537,   537,   339,   537,
     537,   556,   339,   537,   339,   537,   537,   537,   537,   537,
     537,   339,   339,   536,    20,   339,   339,   537,   537,   537,
       0,   339,     8,   339,   537,   537,   537,   783,   537,   537,
     537,   537,   537,   537,   537,   537,   537,   537,   537,   537,
     537,   537,   537,   537,   537,   537,   537,   537,   537,   537,
     537,   537,   537,   537,   339,   537,   784,   537,  1223,   537,
    1224,   537,   537,   537,   537,   537,   537,   537,   537,   537,
     537,     0,     0,     0,     0,  1234,   611,     0,    37,   611,
      73,    48,    49,    50,    65,    66,    76,    68,    69,    67,
     109,    58,     0,   146,   151,    52,    70,    71,    72,    51,
      59,    54,    55,    56,    60,   207,    75,    74,    57,   611,
     445,   440,   453,     0,     0,     0,   456,   439,   438,     0,
     508,   511,   537,   509,     0,   537,     0,   537,   545,     0,
       0,   552,    53,   459,   613,   616,   622,   618,   617,   623,
     624,   625,   626,   615,   632,   614,   633,   619,     0,   780,
     620,   627,   629,   628,   660,   634,   636,   637,   635,   638,
     639,   640,   641,   642,   621,   643,   644,   646,   647,   645,
     649,   650,   648,   674,   661,   662,   663,   664,   651,   652,
     653,   655,   654,   656,   657,   658,   659,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   631,   675,   630,  1032,
    1031,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,
    1042,  1043,  1044,  1045,  1046,  1047,  1030,  1048,  1049,  1050,
    1051,  1052,  1053,  1054,  1055,  1056,   460,  1190,  1192,  1194,
    1195,  1191,  1193,   461,  1225,  1226,  1227,  1228,  1229,     0,
       0,   340,     0,     0,     0,     0,     0,     0,     0,   992,
      35,     0,     0,   598,     0,     0,     0,     0,     0,     0,
     454,   507,   481,   210,     0,     0,     0,   481,     0,   312,
     339,   727,     0,   727,   538,     0,    23,   481,     0,   481,
     972,     0,   989,   483,   481,   481,   481,    32,   484,    81,
     444,   955,   481,   949,   105,   481,    37,   481,     0,   340,
       0,     0,    63,     0,     0,   329,    44,     7,   966,     0,
       0,     0,   599,     0,     0,    77,   340,     0,   986,   522,
       0,     0,     0,   296,   295,     0,     0,   813,     0,     0,
     340,     0,     0,   538,     0,   340,     0,     0,     0,   340,
      33,   340,    22,   599,     0,    21,     0,     0,     0,     0,
       0,     0,     0,   398,   340,    45,   140,     0,     0,     0,
       0,     0,     0,     0,     0,   787,   340,     0,   340,     0,
       0,   990,   991,     0,   339,   340,     0,     0,     0,   599,
       0,  1176,  1175,  1180,  1057,   727,  1182,   727,  1172,  1174,
    1058,  1163,  1166,  1169,   727,   727,   727,  1178,  1171,  1173,
     727,   727,   727,   727,  1111,   727,   727,  1188,  1145,     0,
      45,  1197,  1200,  1203,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    45,    45,  1255,  1233,   612,     4,    20,
      20,     0,     0,    45,     5,     0,     0,     0,     0,     0,
      45,    20,     0,     0,     0,   147,   164,     0,     0,     0,
       0,   528,     0,   528,     0,     0,     0,     0,   528,   222,
       6,   486,   537,   537,   446,   441,     0,   457,   448,   447,
     455,    82,   172,     0,     0,     0,   406,     0,   405,   410,
     408,   409,   407,   381,     0,     0,   351,   382,   354,   384,
     383,   355,   400,   402,   395,   353,   356,   598,   398,   542,
     543,     0,   380,   379,    32,     0,   602,   603,   540,     0,
     600,   599,     0,   544,   599,   564,   547,   546,   600,   550,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
       0,   776,   777,   775,     0,   773,   763,     0,     0,   435,
       0,   323,     0,   524,   974,   975,   971,    45,   310,   808,
     810,   973,    36,    13,   598,     0,   481,     0,   192,     0,
     310,     0,   184,     0,   709,   707,   843,   927,   928,   807,
     804,   805,   482,   516,   222,   435,   310,   676,   983,   978,
     470,   341,     0,     0,     0,     0,     0,   719,   722,   711,
       0,   497,   682,   679,   680,   451,     0,     0,   500,   984,
     442,   443,   458,   452,   471,   106,   499,    45,   517,     0,
     199,     0,   382,     0,     0,    37,    25,   803,   800,   801,
     324,   326,     0,     0,    45,   967,   968,     0,   700,   698,
     686,   683,   684,     0,     0,     0,    78,     0,    45,   987,
     985,     0,     0,   948,     0,    45,     0,    19,     0,     0,
       0,     0,   953,     0,     0,     0,   497,   523,     0,     0,
     931,   958,   599,     0,   599,   600,   139,    34,    12,   143,
     576,     0,   764,     0,     0,     0,   727,   706,   704,   888,
     925,   926,     0,   703,   701,   959,   399,   514,     0,     0,
       0,   909,     0,   921,   920,   923,   922,     0,   691,     0,
     689,   694,     0,     0,    37,    24,     0,   310,   940,   943,
       0,    45,     0,   302,   298,     0,     0,   577,   310,     0,
     527,     0,  1115,  1110,   527,  1147,  1177,     0,   527,   527,
     527,   527,   527,   527,  1170,   310,    46,  1196,  1205,  1206,
       0,     0,    45,     0,  1020,  1021,     0,   988,   349,     0,
       0,     0,     0,  1235,  1237,  1238,     0,     0,  1241,  1243,
    1244,  1245,     0,     0,  1249,     0,     0,    20,    43,    38,
      42,     0,    40,    17,    46,   310,   132,   134,   136,   110,
       0,     0,    20,   339,   148,   538,   598,   165,   146,   310,
     179,   181,   183,   187,   527,   190,   527,   196,   198,   200,
     209,     0,   213,     0,    45,     0,   449,   424,     0,   351,
     364,   363,   376,   362,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   598,     0,     0,   598,     0,     0,   398,
     404,   396,   512,     0,     0,   515,   570,   571,   575,     0,
     567,     0,   569,     0,   608,     0,     0,     0,     0,     0,
     554,   569,   558,     0,     0,   582,   548,   580,     0,     0,
     351,   353,   551,   586,   585,   553,   677,   310,   699,   702,
     705,   708,   310,   339,     0,   941,     0,    45,   758,   178,
       0,     0,     0,     0,     0,     0,   312,   812,     0,   529,
       0,   475,   479,     0,   469,   598,     0,   194,   185,     0,
     321,     0,   208,     0,   678,   598,     0,   786,   319,   316,
     313,   315,   320,   310,   727,   724,   733,   728,     0,     0,
       0,     0,     0,   725,   711,   727,     0,   790,     0,   498,
     539,     0,     0,     0,    18,   204,     0,     0,     0,   206,
     195,     0,   494,   492,   489,     0,    45,     0,   329,     0,
       0,   332,   330,     0,    45,   969,   381,   917,   964,     0,
       0,   962,     0,   561,     0,    87,    88,    86,    85,    91,
      90,   102,    95,    98,    97,   100,    99,    96,   101,    94,
      92,    89,    93,    83,     0,    84,   197,     0,     0,     0,
       0,     0,     0,   297,     0,   188,   436,     0,    45,   954,
     952,   133,   817,     0,   815,     0,     0,   292,   539,   180,
       0,   579,     0,   578,   287,   287,     0,   759,     0,   727,
     711,   935,     0,     0,   932,   284,   283,    62,   281,     0,
       0,     0,     0,     0,     0,     0,   688,   687,   135,    14,
     182,   942,    45,   945,   944,   146,     0,   103,    47,     0,
       0,   695,     0,   727,   527,     0,  1144,  1114,  1109,   727,
     527,  1146,  1189,   727,   527,   727,   527,   527,   527,   727,
     527,   727,   527,   696,     0,     0,     0,     0,  1217,     0,
       0,  1204,  1208,  1210,  1209,    45,  1199,   851,  1218,     0,
    1202,     0,     0,     0,   995,    45,    45,    45,     0,   390,
     391,  1025,     0,  1028,     0,     0,     0,    45,  1230,     0,
       0,     0,    45,  1231,  1251,  1253,     0,    45,  1232,    28,
      37,     0,     0,    39,     0,    30,   159,   116,   310,   339,
     118,   120,     0,   121,   114,   122,   130,   129,   123,   124,
     125,     0,   112,   115,    26,     0,   310,     0,     0,   144,
     177,     0,     0,   222,   222,     0,   224,   217,   221,     0,
       0,     0,   352,     0,   359,   361,   358,   357,   375,   377,
     371,   365,   504,   368,   366,   369,   367,   370,   372,   374,
     360,   373,   378,   598,   411,   389,     0,   343,     0,   414,
     415,   401,   412,   403,     0,   598,   513,     0,   532,   530,
       0,   598,   566,   573,   574,   572,   601,   610,   605,   607,
     609,   606,   604,   565,   549,     0,     0,     0,   351,     0,
       0,     0,     0,     0,   697,   779,     0,   789,   792,   782,
       0,   791,   785,     0,   783,   784,   781,   774,     0,   429,
       0,     0,   506,     0,     0,     0,     0,   811,   477,   476,
       0,   474,     0,   193,     0,   527,   806,   427,   428,   432,
       0,     0,     0,   314,   317,   176,     0,   598,     0,     0,
       0,     0,     0,   712,   723,   310,   462,   727,   681,     0,
     481,     0,     0,   201,     0,   394,   977,     0,     0,     0,
      16,   802,   327,   337,     0,   333,   335,   331,     0,     0,
       0,     0,     0,     0,     0,   961,   685,   562,    80,    79,
     128,   126,   127,   340,     0,   487,   423,     0,     0,     0,
       0,   191,     0,   520,     0,     0,   727,     0,     0,    64,
     527,   505,   601,   138,     0,   142,    45,     0,   711,     0,
       0,     0,     0,   930,     0,     0,     0,     0,     0,   910,
     912,     0,   692,   690,     0,    45,   947,    45,   946,   145,
     340,     0,   502,     0,  1179,     0,   727,  1181,     0,   727,
       0,     0,   727,     0,   727,     0,   727,     0,   727,     0,
       0,     0,    45,     0,     0,     0,  1207,     0,  1198,  1201,
     999,   997,   998,    45,   994,     0,     0,     0,   350,   598,
     598,     0,  1024,     0,  1027,     0,    45,  1236,     0,     0,
       0,  1242,     0,     0,  1250,    15,    29,    41,     0,   173,
     160,   117,     0,    45,     0,    45,    27,   159,   539,   539,
     169,   172,   168,     0,   186,   189,   214,     0,     0,     0,
     247,   245,   252,   249,   263,   256,   261,     0,     0,   215,
     238,   250,   242,   253,   243,   258,   244,     0,   237,     0,
     232,   229,   218,   219,     0,     0,   425,   351,     0,   387,
     598,   347,   344,   345,     0,   398,     0,   534,   533,     0,
       0,   581,   352,     0,     0,     0,   351,   588,   351,   592,
     351,   590,   310,     0,   598,   518,     0,     0,   976,     0,
     311,   478,   480,   172,   322,     0,   598,   519,     0,   980,
     598,   979,   318,   320,   726,     0,     0,     0,   736,     0,
       0,     0,     0,   710,   464,   481,   501,     0,   203,   202,
     381,   493,   490,   488,     0,   491,     0,   328,     0,     0,
       0,     0,     0,     0,   963,     0,  1009,     0,     0,   422,
     417,   950,   951,   721,   310,   957,   437,   816,   819,   825,
     294,   293,     0,   287,     0,     0,   289,   288,     0,   760,
     761,   713,     0,   939,   938,     0,   936,     0,   933,   282,
       0,  1015,  1004,     0,  1002,  1005,   755,     0,     0,   924,
     916,   693,     0,     0,     0,     0,     0,   300,     0,   299,
     307,     0,  1188,     0,  1188,  1188,  1124,     0,  1118,  1120,
    1121,  1119,   727,  1123,  1122,     0,  1188,   727,  1142,  1141,
       0,     0,  1185,  1184,     0,     0,  1188,     0,  1188,     0,
     727,  1063,  1067,  1068,  1069,  1065,  1066,  1070,  1071,  1064,
       0,  1152,  1156,  1157,  1158,  1154,  1155,  1159,  1160,  1153,
    1162,  1161,   727,     0,  1105,  1107,  1108,  1106,   727,     0,
    1135,  1136,   727,     0,     0,     0,     0,     0,     0,  1219,
       0,     0,   852,   996,     0,  1022,     0,   598,     0,  1026,
    1029,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   794,     0,   111,     0,   173,     0,   146,
       0,   171,   170,   267,   253,   266,     0,   255,   260,   254,
     259,     0,     0,     0,     0,     0,   222,   212,   223,   241,
       0,   222,   234,   235,     0,     0,     0,     0,   278,   223,
     279,     0,     0,   227,   268,   273,   276,   229,   220,     0,
     503,     0,   413,   385,   388,     0,   346,     0,   531,   568,
     569,     0,     0,   351,     0,     0,     0,   778,   772,   788,
       0,     0,     0,   525,     0,   340,   526,     0,   982,     0,
       0,     0,   740,     0,   738,   735,   730,   734,   732,     0,
      45,     0,   463,   450,   205,   334,   336,     0,     0,     0,
     965,   960,   131,     0,  1008,   421,     0,     0,   416,   956,
       0,    45,   814,   826,   827,   832,   836,   829,   837,   838,
     833,   835,   834,   830,   831,     0,     0,     0,     0,   285,
       0,     0,     0,     0,   934,   929,   472,     0,  1001,   727,
     911,     0,     0,   886,   104,   306,   301,   303,     0,     0,
       0,  1073,   727,  1074,  1075,    45,  1116,   727,  1143,  1139,
     727,  1188,     0,  1072,    45,  1076,     0,  1077,     0,  1061,
     727,  1150,   727,  1103,   727,  1133,   727,  1211,  1212,  1213,
    1221,  1222,    45,  1216,  1214,  1215,     0,     0,     0,   393,
    1240,   898,   899,   900,   897,   902,   896,   903,   895,   894,
     893,   901,   889,     0,     0,    45,  1239,  1247,  1248,  1246,
    1254,  1252,    31,   175,   174,     0,     0,   119,   113,   107,
       0,     0,   161,   598,   166,     0,   248,   246,   264,   257,
     262,   216,   222,   598,     0,   240,   236,   223,     0,   233,
       0,   270,   269,     0,   225,   229,     0,     0,     0,     0,
       0,   230,     0,   426,   386,   348,   397,     0,   583,   595,
     597,   596,     0,   430,     0,     0,   809,     0,   433,     0,
     981,   756,   729,     0,     0,    45,     0,     0,     0,   844,
     970,   845,  1014,     0,  1011,  1013,   420,   419,     0,     0,
     818,     0,   828,     0,   288,     0,     0,   765,   762,   719,
     714,   715,   717,   718,   937,  1003,  1007,     0,     0,   381,
       0,     0,     0,     0,     0,   309,   308,   521,     0,     0,
       0,  1117,  1140,     0,  1187,  1186,     0,     0,     0,  1062,
    1151,  1104,  1134,  1220,     0,     0,   392,     0,     0,     0,
       0,     0,   795,   162,   163,     0,     0,   239,   598,   241,
       0,   280,   228,     0,   272,   271,   274,   275,   277,   473,
       0,   770,   769,   771,     0,   767,   431,     0,   993,   434,
       0,   741,   739,     0,   731,     0,     0,     0,  1010,   418,
       0,     0,     0,     0,     0,   907,     0,     0,     0,     0,
       0,     0,   286,   291,   290,     0,     0,     0,  1000,   913,
     914,     0,   842,   887,   887,   305,   304,  1089,  1088,  1087,
    1094,  1095,  1096,  1093,  1090,  1092,  1091,  1100,  1097,  1098,
    1099,     0,  1084,  1128,  1127,  1129,  1130,     0,  1189,  1079,
    1081,  1080,     0,  1083,  1082,     0,  1023,     0,     0,   890,
     796,     0,   172,   265,     0,     0,   227,   226,     0,     0,
     766,   510,     0,     0,     0,   466,  1012,   824,   823,     0,
     821,   727,   856,     0,     0,     0,     0,     0,   905,   906,
       0,     0,     0,     0,   716,   918,  1006,    45,     0,     0,
       0,     0,     0,  1126,  1183,  1078,    45,   891,     0,     0,
       0,   251,   231,   495,   768,   757,   744,   737,   742,     0,
       0,   820,   861,   857,     0,     0,     0,     0,     0,     0,
       0,     0,   848,     0,   467,   720,     0,     0,   841,    45,
      45,   884,  1086,  1085,     0,     0,     0,   354,   799,   793,
     797,   167,     0,     0,   465,   822,     0,     0,     0,     0,
     854,   846,     0,   840,     0,   904,   855,     0,   847,     0,
     919,     0,     0,     0,  1125,     0,   892,     0,   496,     0,
     747,     0,   745,   748,   859,   860,     0,   862,   864,     0,
       0,     0,   849,   468,   915,   885,   883,     0,   798,   750,
     751,     0,   743,     0,   858,     0,   853,   839,     0,     0,
       0,     0,   749,   752,   746,   863,     0,     0,   867,   908,
     850,  1017,   753,     0,     0,     0,   865,    45,  1016,   754,
     869,   868,    45,     0,     0,     0,   870,   875,   877,   878,
    1018,  1019,     0,     0,     0,    45,   866,    45,    45,   598,
     881,   880,   879,   871,     0,   873,   874,     0,   876,     0,
      45,   882,   872
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     6,     7,   206,   380,   207,   834,   745,   208,
     903,   613,   798,   683,   563,   901,   902,   437,   904,  1221,
    1501,   209,   210,   614,  1124,  1125,   211,   212,   213,   573,
    1281,  1282,  1128,  1283,   214,   215,   216,   217,  1154,   218,
     219,  1155,   220,   576,   221,   222,   223,   224,  1559,  1560,
     918,  1571,   937,  1832,   225,   226,   227,   228,   229,   230,
     779,  1165,  1166,   231,   232,   233,   740,  1076,  1077,   234,
     235,   704,   449,   930,   931,  1587,   932,   933,  1870,  1597,
    1602,  1603,  1871,  1872,  1598,  1599,  1600,  1589,  1590,  1591,
    1592,  1844,  1594,  1595,  1596,  1846,  2081,  1874,  1875,  1876,
    1167,  1168,  1473,  1474,  1959,  1707,  1146,  1147,   236,   454,
     237,   844,  1976,  1977,  1739,  1740,  1027,   712,   713,  1050,
    1051,  1039,  1040,   238,   750,   751,   752,   753,  1092,  1434,
    1435,  1436,   393,   370,   400,  1326,  1611,  1327,   877,   999,
     616,   635,   617,   618,   619,   620,  1241,  1079,   970,  1884,
     817,   621,   622,   623,   624,   625,  1331,  1613,   626,  1301,
    1881,  1399,  1380,  1400,  1020,  1137,   239,   240,  1922,   241,
     242,   686,  1032,  1033,   703,   419,   243,   244,   245,   246,
    1083,  1084,  1428,  1891,  1892,  1070,   247,   248,   249,   250,
    1203,   251,   973,  1339,   252,   372,   721,  1417,   253,   254,
     255,   256,   257,   258,   646,   638,   979,   980,   981,   259,
     260,   261,   996,   997,  1002,  1003,  1004,  1328,   763,   639,
     795,   558,   262,   263,   264,   707,   265,   723,   724,   266,
     761,   762,   267,   495,   829,   830,   832,   268,   269,   759,
     270,   814,   271,   808,   272,   695,  1067,   273,   274,  2130,
    2131,  2132,  2133,  1694,  1064,   403,   715,   716,  1063,  1659,
    1722,  1913,  1914,  2327,  2328,  2391,  2392,  2410,  2422,  2423,
    1727,  1911,   275,   276,  1709,   667,   803,   804,  1899,  2194,
    2195,  1900,   664,   665,   277,   278,   279,   280,  2045,  2046,
    2359,  2360,   281,   748,   749,   282,   700,   701,   283,   679,
     680,   284,   285,  1143,  1144,  1699,  2120,  2289,  2290,  1942,
    1943,  1944,  1945,  1946,   697,  1947,  1948,  1949,  2343,  1228,
    1950,  1951,  1952,  2292,  2368,  2396,  2397,  2427,  2428,  2445,
    2446,  2447,  2448,  2449,  2460,  1953,  2142,  2309,   810,  1823,
    2032,  2033,  2034,  1954,   822,  1488,  1489,  1971,  1161,  2306,
     286,   287,   288,   289,   290,   291,   292,   293,   791,  1163,
    1164,  1715,  1716,   294,   838,   295,   774,   296,   775,   297,
    1140,   298,   299,   300,   301,   302,  1100,  1101,   303,   756,
     304,   305,   306,   675,   676,   307,   308,  1402,  1649,   709,
     309,   310,   770,   311,   312,   313,   314,   315,   316,   317,
    1233,  1234,   318,  1171,  1723,  1724,  2228,   319,  1687,  2113,
    2114,  1725,   320,  2438,   321,   322,   323,   324,  1242,   325,
    1244,   326,   327,   328,   329,   330,  1204,  1770,   856,  1748,
    1749,  1750,  1774,  1775,  1776,  2262,  1777,  1778,  1751,  2149,
    2353,  2251,   331,  1210,  1798,   332,   333,   334,   335,  1194,
    1752,  1753,  1754,  2257,   336,  1212,  1802,   337,  1200,  1757,
    1758,  1759,   338,   339,   340,  1206,  1792,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     354,   355,  1763,  1764,   857,  1510,   356,   357,   358,   359,
     360,   867,   868,   869,  1222,  1223,  1224,  1229,  1808,  1809,
     361,   362,   363,   364,   365,   366,   367,   368,   882,   883,
     884,   885,   887,   888,   889,   890,   891,   893,   894,   556
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1912
static const yytype_int16 yypact[] =
{
   -1912,   122, -1912, -1912, -1912, -1912,    60,  4592, -1912, -1912,
   -1912,   106, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,   886, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,    51, -1912, -1912,   744,   136, -1912, -1912, -1912,    51,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912,    91,    91, -1912, -1912, -1912, -1912, -1912,    91,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
     167, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,    91, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,   251, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
     301,   352, -1912, -1912, -1912, -1912, -1912,    51, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912,    51, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,   397,   466,   606,    91, -1912,   392,  1442,   715,   392,
   -1912, -1912, -1912,   749,   769,   775,   778, -1912, -1912, -1912,
     640,   794,    91, -1912, -1912,   801,   815,   832,   839,   620,
     179,   842,   891,   909, -1912,   134, -1912, -1912, -1912,   392,
   -1912, -1912, -1912,   716,   698,  1725,  1992, -1912, -1912,  2888,
   -1912,   916, -1912, -1912,  1938, -1912,   919, -1912, -1912,  1349,
     919,   943, -1912, -1912,   950, -1912, -1912, -1912,   964,   980,
     983,  1004,  1010, -1912, -1912, -1912, -1912,  1028,   999, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912,  1035, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,   131,
      91,   947,  1039,  1043,   644,    91,    91,   109,    91, -1912,
      91,    91,  1048, -1912,   177,  1063,    91,    91,    91,    91,
   -1912, -1912,    91, -1912,  1093,    91,   955,    91,  1105, -1912,
   -1912, -1912,    91, -1912,  1112,    91, -1912,    91,  1115,   150,
   -1912,   955, -1912,    91,    91,    91,    91, -1912, -1912, -1912,
   -1912, -1912,    91, -1912,    91,    91,   715,    91,  1121,   947,
      91,  1131, -1912,    91,    91, -1912, -1912, -1912,  1154,  1137,
      91,    91, -1912,  1151,  1159,    91,   947,  1169,  2888, -1912,
    1177,  1183,    91, -1912,  1181,    91,  1167, -1912,  1197,    91,
     947,  1199,  1205, -1912,   644,   947,    91,    91,  2210,    95,
      91,    99, -1912, -1912,   169, -1912,   212,    91,    91,    91,
    1215,    91,    91,  2888,   126, -1912, -1912,  1223,    91,    91,
      91,    91,    91,  2534,    91, -1912,   947,    91,   947,    91,
      91, -1912, -1912,    91, -1912,   947,    91,  1230,  1233, -1912,
      91, -1912, -1912,  1245, -1912, -1912,  1248, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912,  1264, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,    91,
   -1912, -1912,  1271,  1294,  1302,  2888,  2888,  2888,  2888,  2888,
    1311,  1313, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,  1147,   135, -1912, -1912,    91,    91,    91,    91,  1275,
   -1912, -1912,  1253,    91,    91, -1912,   651,    91,    91,    91,
      91,    91,   547,    91,  1167,    91,    91,  1121, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912,  1084, -1912, -1912, -1912,
   -1912, -1912, -1912,  2888,  2888,  2888, -1912,  2888, -1912, -1912,
   -1912, -1912, -1912, -1912,  2888,  2729, -1912,   186,  1328, -1912,
    1376, -1912,  1148,  1157,  1391, -1912, -1912,  1398,  2888, -1912,
   -1912,  1869, -1912, -1912,  1397,  1317,  1328, -1912, -1912,  1484,
     -16, -1912,  1869, -1912, -1912, -1912,  1423,   222,    82,  3032,
    3032,    91,    91,    91,    91,    91,    91,    91,  1426, -1912,
      91, -1912, -1912, -1912,   511, -1912, -1912,  1427,    91, -1912,
    2888, -1912,  1198,   522, -1912,  1428, -1912, -1912,  1429,  1440,
   -1912, -1912, -1912, -1912, -1912,  2598,    91,  1438, -1912,    91,
    1429,    91, -1912,   644, -1912, -1912, -1912, -1912, -1912, -1912,
    1450, -1912, -1912, -1912, -1912, -1912,  1429, -1912, -1912,  1446,
   -1912, -1912,   625,  1144,    91,   652,    86, -1912,  1449,  1290,
    2888,  1318, -1912,  1460, -1912, -1912,  2888,  2888, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,    91,
   -1912,    91,  1454,   168,    91,   715, -1912, -1912,  1462, -1912,
    1465, -1912,  1461,    78, -1912,  1467, -1912,    91, -1912, -1912,
   -1912,  1471, -1912,   919,  1456,  3218, -1912,    91, -1912,  5522,
   -1912,    91,  2888, -1912,  1469, -1912,    91, -1912,    91,    91,
      91,  1328,   405,    91,    91,    91,  1318, -1912,    91,   637,
   -1912, -1912, -1912,  1317,  1484, -1912, -1912, -1912, -1912, -1912,
   -1912,   131, -1912,  1427,  1478,  1449, -1912, -1912, -1912, -1912,
   -1912, -1912,    91, -1912, -1912, -1912,  5522, -1912,   177,  1424,
      91, -1912,  1473, -1912, -1912, -1912, -1912,  1475,  3255,   720,
   -1912, -1912,   251,    91,   715, -1912,    91,  1429, -1912,  1487,
    1476, -1912,    91, -1912,  1494,  2888,  2888, -1912,  1429,    91,
     232,    91,  1216,  1216,   303,  1216, -1912,  1489,   384,   436,
     465,   470,   491,   499, -1912,  1429,   850, -1912,  1500, -1912,
     107,   152, -1912,  3508,  5522,  3542,  3602,  1501,  5522,    91,
      91,   262,   726, -1912, -1912, -1912,    73,   732, -1912, -1912,
   -1912, -1912,   808,   768, -1912,    91,    91, -1912, -1912, -1912,
   -1912,   795, -1912, -1912,  1298,  1429, -1912, -1912, -1912, -1912,
    1754,    91, -1912, -1912, -1912, -1912, -1912, -1912, -1912,  1429,
   -1912, -1912, -1912, -1912,  1506, -1912,  1506, -1912, -1912, -1912,
   -1912,   588, -1912,   447, -1912,  1504, -1912, -1912,  3678,  1508,
    1511,  1511,  1694, -1912,  2888,  2888,  2888,  2888,  2888,  2888,
    2888,  2888,  2888,  2888,  2888,  2888,  2888,  2888,  2888,  2888,
    2888,  2888,  2888, -1912,  1458,  1394,  1503,   328,    84,  2888,
   -1912, -1912, -1912,   810,  1325, -1912, -1912, -1912, -1912,   825,
   -1912,  1291,  1108,  2888,  1516,  1317,  1317,  1317,  1317,  1317,
   -1912,  1381, -1912,   222,   222,  1328,  1520, -1912,  3032,  5522,
     127,   137, -1912,  1521,  1526, -1912, -1912,  1429, -1912, -1912,
   -1912, -1912,  1429, -1912,   571, -1912,   131, -1912, -1912, -1912,
      91,  3735,    91,  1522,  2888,  1468, -1912, -1912,    91, -1912,
    2888,  3769, -1912,   830, -1912, -1912,  1505, -1912, -1912,   831,
   -1912,    91, -1912,    91, -1912, -1912,  1144, -1912, -1912, -1912,
   -1912, -1912,  3805,  1429, -1912, -1912, -1912,  1523,  1525,  1528,
    1530,  1531,  1532, -1912,  1290, -1912,    91, -1912,  3839, -1912,
   -1912,    91,  3876,  3949, -1912,  1533,   833,  1543,  1391, -1912,
   -1912,    91, -1912,  1549, -1912,  1515, -1912,    91, -1912,  1430,
     611, -1912, -1912,   -11, -1912, -1912,  1553, -1912,  1546,  1556,
     837, -1912,    91,  3032,  1544, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912,  1548, -1912, -1912,   370,  1554,  1552,
    3983,  2641,   -77, -1912,  1527, -1912, -1912,   840, -1912, -1912,
   -1912, -1912, -1912,   847, -1912,  1541,   864, -1912, -1912, -1912,
    2888, -1912,  1589, -1912, -1912, -1912,   867, -1912,  1567, -1912,
    1290,  1561,  1570,   883, -1912, -1912, -1912,  1572, -1912,  1563,
    1568,  1562,    91,  2888,  2888,  2534, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,  1574,  1579, -1912,   506, -1912, -1912,  4021,
    4066, -1912,  1569, -1912,   528,  1578, -1912, -1912, -1912, -1912,
     556, -1912, -1912, -1912,   569, -1912,   579,   580,   603, -1912,
     605, -1912,   616, -1912,  1576,  1580,  1585,  1592, -1912,  1593,
    1594, -1912, -1912, -1912, -1912, -1912, -1912,  1328,  1581,  1590,
   -1912,  1597,   -70,   897, -1912, -1912, -1912, -1912,  2888,   627,
     779, -1912,   905, -1912,   906,  1604,  1605, -1912, -1912,  1607,
    1609,  1610, -1912, -1912, -1912,  1612,  1613, -1912, -1912, -1912,
     715,    91,   145, -1912,  1614, -1912,  1588, -1912,  1429, -1912,
   -1912, -1912,  1615, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,  1086, -1912, -1912, -1912,    91,  1429,   119,  1587, -1912,
   -1912,    91,    91, -1912,  1435,   447, -1912,  1616, -1912,  1573,
    2888,  3032, -1912,  2888,  1511,  1511,   589,   589,  1694,   666,
    5584,  1752,  5522,  1752,  1752,  1752,  1752,  1752,  5584,  2844,
    1511,  2844,  5553,  1503, -1912, -1912,  1611,  1626,  1050, -1912,
   -1912, -1912, -1912, -1912,  1630, -1912, -1912,   644,  5522, -1912,
    2888, -1912, -1912, -1912, -1912,  5522,    70,  5522,  1516,  1516,
     861,  1516,   613, -1912,  1520,  1631,   222,  4102,  1632,  1633,
    1635,  3032,  3032,  3032, -1912, -1912,    91,  1623, -1912, -1912,
    1629,  1449, -1912,   251, -1912, -1912, -1912, -1912,  1392, -1912,
     925,   644, -1912,   644,   952,  1639,   972, -1912,  5522,  2888,
    2598, -1912,   973, -1912,   644,  1506, -1912,   785,   789, -1912,
     974,  1507,   975, -1912,  2281, -1912,    86, -1912,  1659,    91,
      91,  2888,    91, -1912, -1912,  1429, -1912, -1912, -1912,  1432,
      91,  2888,    91, -1912,    91, -1912,  1328,  2888,  1655,  2641,
   -1912, -1912, -1912, -1912,   984, -1912,  1662, -1912,  1665,  1666,
    1668,  1479,  2888,    91,    91, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,   947,    91, -1912,  2686,  2944,  1667,    91,
      91, -1912,    91, -1912,    22,    91, -1912,  2888,    91, -1912,
    1506,  5522, -1912,  1682,    96,  1682, -1912,    91,  1290,  1685,
    2770,    91,    91, -1912,   177,  2888,   753,  2888,   994, -1912,
    1678,   995,  5522, -1912,    53, -1912, -1912, -1912, -1912, -1912,
     947,    49, -1912,    91, -1912,   813, -1912, -1912,  -112, -1912,
     100,   868, -1912,   817, -1912,   428, -1912,   277, -1912,    91,
      91,    91, -1912,    91,    91,   850, -1912,    91, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912,  1366,  1382,  1367,  5522, -1912,
    1503,    91, -1912,    91, -1912,    91, -1912, -1912,    91,    91,
      91, -1912,    91,    91, -1912, -1912, -1912, -1912,    91,   877,
   -1912, -1912,  1680, -1912,  1693, -1912, -1912,  1588, -1912, -1912,
   -1912, -1912,  5522,  2355, -1912, -1912, -1912,   480,   409,   409,
    1455,  1459, -1912, -1912,  1472,  1481,  1482,   615,    91, -1912,
   -1912, -1912, -1912,  1699, -1912, -1912, -1912,  1616, -1912,  1700,
   -1912,   763,  1692, -1912,  1698,  4143, -1912,  1709,  1712,  1391,
   -1912, -1912,  4199, -1912,  2888,  2888,  1325, -1912,  5522,  1869,
     222, -1912,   154,  3032,  3032,  3032,   184, -1912,   360, -1912,
     441, -1912,  1429,    91, -1912, -1912,  1706,  1001, -1912,  1708,
   -1912,  5522, -1912, -1912, -1912,  2888, -1912, -1912,  2888, -1912,
   -1912, -1912, -1912,  5522, -1912,  1507,  2888,  1713, -1912,  1714,
    1716,  4230,  1731, -1912,    66,    91, -1912,  1005, -1912, -1912,
    1717,  5522, -1912, -1912,  4199, -1912,  1430, -1912,  1430,    91,
      91,    91,  1016,  1020, -1912,    91,  1729,  1724,  2888,  4265,
    2829, -1912, -1912, -1912,  1429,  1328, -1912, -1912,  1575,  1738,
    5522, -1912,    91, -1912,  1734,  1736, -1912, -1912,  1497,  1747,
   -1912, -1912,  1749, -1912,  5522,  1024, -1912,  1030, -1912, -1912,
    4345, -1912, -1912,  1031, -1912, -1912,  5522,  1737,    91, -1912,
   -1912, -1912,  1744,  1748,  1559,  1701,    91,    91,  1758,  1771,
   -1912,   559, -1912,  1770, -1912, -1912, -1912,  1772, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912,   813, -1912, -1912, -1912, -1912,
    -112,    91, -1912, -1912,  1040,  1775, -1912,  1777, -1912,  1778,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
     868, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,   817, -1912, -1912, -1912, -1912, -1912,   428,
   -1912, -1912, -1912,   277,  1774,  1776,  1779,   784,  1054, -1912,
    1780,  1782,  1328, -1912,  1781, -1912,  1784,  1503,  1786, -1912,
   -1912,  1060,   786,  1064,  1071,  1089,  1118,  1120,  1155,  1787,
    2888,  2888, -1912, -1912,  1788, -1912,  1933,   877,  2428, -1912,
    1162, -1912,  5522, -1912, -1912, -1912,  1797, -1912, -1912, -1912,
   -1912,   328,   328,   328,   328,   328,  1435, -1912,  1795,  1806,
    1801,  1435,  1692, -1912,   447,   763,   140,   140, -1912, -1912,
   -1912,  1173,  1810,   970,   156, -1912,  1809,   763, -1912,  2888,
   -1912,  1800, -1912,  1391, -1912,  1050,  5522,  1802, -1912, -1912,
    1484,  1796,  1803,  1174,  1807,  1811,  1812, -1912, -1912, -1912,
    1815,    -2,   644, -1912,    91,   947,  5522,    -2,  5522,  1507,
    2888,  1813,  4594,  1182, -1912, -1912, -1912, -1912, -1912,  2888,
   -1912,  1816, -1912, -1912, -1912, -1912, -1912,  1185,  1192,  1207,
   -1912, -1912, -1912,   709, -1912,  5522,  2888,  2888,  4926, -1912,
      91, -1912, -1912,  1738, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912,  1818,    96,  1819,  3218, -1912,
      91,    91,    91,  2770, -1912, -1912, -1912,   753, -1912, -1912,
   -1912,  2481,    91, -1912, -1912,  1817,  1828, -1912,    91,    91,
    2888, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912,   100, -1912, -1912, -1912,  2888, -1912,  2888, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912,    91,    91,  1821,   809,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,  1823,  1831, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,  5522,  5522,  1210,  1820, -1912, -1912, -1912,
    1827,  4961, -1912, -1912, -1912,   480, -1912, -1912, -1912, -1912,
   -1912, -1912,  1435, -1912,    91, -1912, -1912,  1834,  1824, -1912,
    1053,   156,   156,   763, -1912,   763,   140,   140,   140,   140,
     140,  1303,  4992, -1912, -1912, -1912, -1912,  2888, -1912, -1912,
   -1912, -1912,  2187, -1912,    91,  1843,  1440,    91, -1912,    91,
   -1912,  5023, -1912,  2888,  2888, -1912,  5054,  1599,  2888, -1912,
   -1912, -1912, -1912,  1211, -1912, -1912,  5522,  5522,  2888,  1839,
   -1912,   914, -1912,  2888, -1912,  1835,  1836, -1912, -1912,  1840,
    1851, -1912, -1912, -1912, -1912, -1912,  1728,  1841,  1214,  1854,
    1856,  1229,   728,    91,    91, -1912, -1912,  5522,   558,  1844,
      19, -1912, -1912,  1829, -1912, -1912,   332,  5085,  5116, -1912,
   -1912, -1912, -1912, -1912,  1241,  1846,   946,    91,    91,   786,
    1861,  1862, -1912, -1912, -1912,  1587,  1850, -1912,  1503, -1912,
     763, -1912,  1303,  1855,   156,   156, -1912, -1912, -1912, -1912,
    5147, -1912,  4199, -1912,  1244, -1912, -1912,   644,  1581, -1912,
    1507,  5522, -1912,  1617, -1912,  1860,  5178,   709, -1912,  5522,
    2436,  1864,  1865,  1872,  1873,  1874,  1875,    91,    91,  1877,
    1878,  5209, -1912, -1912, -1912,  2888,    91,    91, -1912, -1912,
    1879,    91, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,  1891, -1912, -1912, -1912, -1912, -1912,  1256, -1912, -1912,
   -1912, -1912,  1881, -1912, -1912,  1892, -1912,   294,  1895, -1912,
   -1912,  1899, -1912, -1912,  1886,  1259,  1303, -1912,  2888,  2187,
   -1912, -1912,  2888,    91,  2888, -1912, -1912, -1912,  5522,  1267,
   -1912, -1912,  1864,    91,    91,    91,    91,    91, -1912, -1912,
    2888,  2888,  2888,  1273, -1912, -1912,  1904, -1912,  1277,  1907,
    1281,    91,  2888, -1912, -1912, -1912, -1912, -1912,  2888,    91,
    1896, -1912, -1912,  5240, -1912,  5522, -1912, -1912,  1901,  5271,
    2436, -1912,   312, -1912,  1911,  1282,  1283,  1912,  1289,  1906,
    1308,  5302,  5333,  1902,  5364, -1912,    91,  1847, -1912, -1912,
   -1912, -1912,  1581, -1912,  5395,  1601,  5426,   565, -1912,  1916,
   -1912, -1912,  2888,  2248, -1912, -1912,  1922,  1923,    91,    91,
   -1912, -1912,    91, -1912,  2534, -1912, -1912,  2888, -1912,  2888,
   -1912,  1910,  1316,  1330, -1912,  1919, -1912,    91,  5522,    91,
    5522,  1335, -1912, -1912, -1912, -1912,  1341, -1912,  1921,  1342,
    1359,  1361,  5457,  5522, -1912, -1912, -1912,  2888, -1912, -1912,
    1925,  2248, -1912,    91, -1912,  2888, -1912, -1912,  1917,  2888,
    1363,  2888,  1932, -1912, -1912, -1912,  5491,  1371, -1912, -1912,
    5522,  1942, -1912,  2888,  2888,  2888,  1936, -1912, -1912, -1912,
    5522, -1912, -1912,   -63,   102,  1380, -1912,  1949,  1951, -1912,
   -1912, -1912,  1944,  1944,  1944, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912,   504,  1953, -1912,  1838, -1912,  1387,
   -1912, -1912, -1912
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1912, -1912, -1912, -1912, -1912,   -15,  1743,  1072, -1912, -1912,
    -670,   -52, -1912, -1912,  -391, -1912,   705, -1912,   -50,  1065,
   -1912, -1912, -1912,  2004,    13, -1912, -1912, -1912, -1912, -1912,
   -1912,   139,   410,   790, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912,  -164,  -897, -1912, -1912, -1912,   882,   412,  1404,
   -1912,  -193, -1518,   146, -1912, -1912, -1912, -1912, -1912, -1912,
    1402,  -288,  -182, -1912, -1912, -1912,  1400, -1912,  -395, -1912,
   -1912, -1912, -1912,  1284, -1912, -1912,   696, -1264, -1533,  1058,
     395, -1520,  -187,   -81,  1062, -1912,   133,   143, -1775, -1912,
   -1506, -1266, -1504,  -231, -1912,   -55, -1535, -1610,  -960, -1912,
   -1912,   517,   848,   302,  -120,    50, -1912,   539, -1912, -1912,
   -1912, -1912, -1912,  -135, -1912,    31,  -598,   985, -1912,   967,
     610,   632,  -371, -1912, -1912,   928, -1912, -1912, -1912, -1912,
     341,   344,  1954,  3328,  -366, -1282,   147,  -440, -1020,   708,
    -534,  -606,  1656,   -64,  1583,  -861,  -986, -1912, -1912,  -622,
    -604,  -214, -1912,  -916, -1912,  -624,  -945, -1124, -1912, -1912,
   -1912,   124, -1912, -1912,  1329, -1912, -1912,  1799, -1912,  1805,
   -1912, -1912,   649, -1912,  -378,    18, -1912, -1912,  1822,  1830,
   -1912,   617, -1912,  -728,  -383,  1260, -1912,  1128, -1912, -1912,
      17, -1912,  1012,   434, -1912,  4395,  -407, -1086, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912,  -202, -1912,   435,  -854, -1912,
   -1912, -1912,   406, -1261,  -623,  1059,  -956,  -351,  -342,  -446,
     432,    71, -1912, -1912, -1912,  1405, -1912, -1912,   988, -1912,
   -1912,   958, -1912,  1231, -1911,   887, -1912, -1912, -1912,  1411,
   -1912,  1414, -1912,  1410, -1912,  1416,  -994, -1912, -1912, -1912,
    -151,  -242, -1912, -1912, -1912,  -372, -1912,  -240,   671,  -339,
     672, -1912,   -25, -1912, -1912, -1912,  -330, -1912, -1912, -1912,
   -1493, -1912, -1912, -1912, -1912, -1912, -1413,  -541,   123, -1912,
    -197, -1912,  1287,  1068, -1912, -1912,  1075, -1912, -1912, -1912,
   -1912,  -302, -1912, -1912,  1006, -1912, -1912,  1061, -1912,   190,
    1067, -1912, -1912,  -142,   634, -1912, -1912, -1912,  -230, -1912,
   -1912,   158, -1912,  -774,  -387,  -362, -1912, -1912, -1912, -1462,
   -1912, -1912, -1912,  -186, -1912, -1912,  -308, -1912,  -327, -1912,
    -346, -1912,  -347, -1794, -1039,  -760, -1912,  -121,  -470,  -920,
   -1671, -1912, -1912, -1912,  -477, -1500,   386, -1912,  -648, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,  -474, -1461,
     635, -1912,   155, -1912,  1463, -1912,  1621, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1437,   681, -1912,  1377,
   -1912, -1912, -1912, -1912,  1755, -1912, -1912, -1912,   217,  1723,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912,   607, -1912, -1912, -1912,   171, -1912, -1912, -1912, -1912,
     -71, -1858, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912,   576,   361,  -498, -1334,
   -1308, -1305, -1435, -1426, -1424, -1912, -1416, -1406, -1287, -1912,
   -1912, -1912, -1912, -1912,   343, -1912, -1912, -1912, -1912, -1912,
     388, -1403, -1402, -1912, -1912, -1912,   342, -1912, -1912,   387,
   -1912,   572, -1912, -1912, -1912, -1912,   353, -1912, -1912, -1912,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,
   -1912, -1912,   159, -1912,   157,  -106, -1912, -1912, -1912, -1912,
   -1912, -1912, -1912,   929, -1912, -1912, -1912,  -812, -1912,   141,
   -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912, -1912,   908,
   -1912, -1912, -1912,   904, -1912, -1912, -1912, -1912,   900, -1912
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1189
static const yytype_int16 yytable[] =
{
     406,   698,   971,   672,  1384,   668,  1683,  1458,   815,   811,
     418,   418,   823,   824,   825,   826,  1085,   426,  1240,   710,
    1717,  1289,   794,  1333,   975,  1001,  1001,  1005,  1593,   725,
    1588,   728,   687,   989,  1379,   737,   731,   732,   733,   864,
     637,  1608,  1360,   561,   734,   637,   453,   736,  1824,   738,
    1826,  1330,  1330,  1840,   643,  1859,   786,   647,   649,  1231,
    2138,   433,  1470,   741,  1710,  1183,  1873,  1074,   472,   939,
    1413,  1843,   475,  1845,  1920,  2115,   755,  1863,  1784,  1184,
     767,   420,  1878,  1821,   455,  -565,  2065,  1785,  1825,  1786,
    -563,  1056,  1037,   787,   783,  1621,   691,  1787,  1459,   788,
    -541,   470,   398,   796,   806,   799,   398,  1788,  1044,  1098,
    1790,  1791,  1530,  2253,  1249,  1000,  1000,   677,   818,  2450,
    1340,   369,     2,     3,  1732,  1905,  1018,   417,  1226,   497,
     833,   800,   836,   398,  1568,  1361,   661,  1761,   662,   842,
     898,    25,   588,  1704,  1057,  -593,    29,  -584,  1332,  -527,
     898,   401,   555,  1491,  1756,  1865,   899,  -593,  1438,  1240,
     718,  2078,  -594,   719,   978,   727,  1479,   847,    47,    48,
     575,   965,  1058,  1230,  -594,  1196,     8,  1771,   417,  1781,
    -339,  1794,  1398,  1800,   801,   982,  -565,   583,   989,   984,
     636,  1438,  -587,  2079,  1081,   636,   991,  1439,  -563,  -565,
     688,   963,   692,  1772,  -587,  1782,  1773,  1795,  1783,  1801,
    1796,  -137,  1059,   417,   689,  -141,  -527,  1927,  1928,  1929,
     417,  1737,    90,  1060,  1779,   417,  1789,   401,  1797,   417,
    2452,   436,    95,   611,   805,  -339,   417,   994,   417,  1181,
     588,  -541,   -61,  -584,  1460,  1531,   582,   584,  1532,  2254,
    1191,  1061,   589,  -593,   637,  2451,  2071,  2072,  1818,  2453,
     417,  1733,  1157,  -527,   417,  -339,   493,  1213,   611,   417,
    -594,  1440,     4,  1705,   417,   850,   974,   852,   121,   417,
     564,   417,  1250,  1099,   853,   854,   855,  2177,  1499,  1868,
     858,   859,   860,   861,   924,   862,   863,   965,   417,   990,
    -587,  1251,   992,  2255,  1440,  1675,   417,  1266,  1034,  1090,
     590,   588,  1847,  1849,  2256,  2317,   503,   663,   666,  2454,
    1921,  1290,   150,   673,   674,   678,   674,  -565,   682,   684,
    2070,  2068,   690,  1029,   694,   696,   696,   699,  1162,  -563,
     702,   417,  2066,   706,  1062,   702,  1065,  1152, -1113,  2115,
     717,   417,  -137,   722,  1086,   702,  -141,   702,  1784,  1621,
     504,   418,   702,   702,   702,  1334,   743,  1785,  -591,  1786,
     702,  1569,   735,   702,  1001,   702,   989,  1787,   989,     5,
    -591,   746,   747,   -61,  -584,   989,   908,  1788,   758,   760,
    1790,  1791,   588,   766,  -593,   781,   920,   921,   922,   923,
     773,  1667,   789,   777,   636,  1627,  1629,  1631,  -384,  1364,
    1498,  -594,   552,  1138,  1365,   790,  1430,   637,   797, -1138,
    1139,   637,  1682,  1496,   717,   807,   809,   809,   637,   813,
     790,   730,   964,  1159,  1573,   819,   821,   821,   821,   821,
     821,  -587,   831,  1179,   588,   835,  1771,   837,   773,  -589,
    1151,   840,  1153,  1245,   843,  1405,  1425,   417,   848,  1781,
    2366,  -589,  1295,  2401,  1358,  1794,  2184,  2185,  1742,  1800,
    1246,  2234,  1772,   588,  1614,  1773,  -591,   611,   588,  1001,
    1447,   553,  1838,  1839,  1711,  1782,  1019,   865,  1783, -1113,
     866,  1795,  2367,  1779,  1796,  1801,  1578,  1579,  2269,   588,
   -1060,  1584,   881,   886,   892,  2393,  1789,   588,  1287,  1038,
     900,  2141,  1797,   905,   906,   907,   690,  1450,  1451,  1016,
     910,   913,   914,  2259,  1490,   919,   690,   690,   690,   690,
     588,  2179,  1017,   927,   928,  2018,   588,  -527,  2182,  1348,
    1349,  1350,  1351,  1352,   895,   896,   989,  1648,  2128,  1843,
    1452,  1845, -1149,  2393,  2164,  1819,   911,  -589,  1585,  1744,
   -1138,  1103,   965,  2237,   588,  1288,  1160,   636,   965,  1000,
     688,   636,  1584,  -852,  2238,  2239,   417,   588,   636,   637,
    2240, -1165,  2241,   995,   689,  1980, -1168,   588,   588,  1091,
    1593,  2242,  1588,  2243,   944,  2244,  1293,  2064,   945,   706,
    1007,   758,   813,   807,   694,  1012,  1149, -1102,   837,  1014,
    1586,   588,  1323,   588,  1367, -1132,   690,  -591,   985,  1742,
    2260,   554,   986,  1856,   588,  1294,   960,  1025,   581,  1585,
    -382,  2261,  2452,  1046,   702,  2198,  1743,  1036,  1368,   690,
     965, -1060,  1539,  -527, -1112,  2276,  1047,   569,   570,   557,
     988,  1178,  1857,  1369,  1180,  1150,   915,  -527,  1370,  1371,
    1054,  2453,  1053,  2466,  2468,  1372,   916,  1627,  1629,  1631,
    1561,   944, -1137,  1055,    25,   945,  2472,  1001,  1606,    29,
    1240,  1586,   946,   947,   974, -1059,   640,  1075,  1567,  1080,
    1023,   648,  1082, -1149,  1401, -1148, -1164,  1276,  -589,  1085,
    1277,    47,    48,   960,  1093,  1096,  2268,  1804,  1805,  1806,
    1744,  1810,  1811,  1745,  1721,  1126,  2112,  2245,  1127, -1167,
    2246, -1101, -1165,   591,  1133,  1132,  1134, -1168,  1175,   636,
     562,  1141, -1131,  1145,  1247,    50,   690,  1001,  1001,  1001,
    1252,  1176,   989,   989,   989,   989,   989,  1248, -1102,   663,
    2021,   398,  2022,  1253,  2320,    90, -1132,   565,  1721,   399,
    1096,  2023,  1340,  2024,   571,    95,   690,  1000,  1656,  1607,
    2308,   637,   637,   637,   637,   637,  1257,   566,  1865,  1866,
    1867,   690,  -383,   567,   690, -1112,   568,    46,  -382,  1258,
    1187,  1186,  -383,   671,  1540,  1098,  1098,  2340,  2064,  1373,
     963,  1910,   574,  1262,   966,  2247,  1227,  1227,  2021,   577,
    2022,   121,  -387, -1137,  1406,   781,  1263,  1663,  1335,  2023,
      74,  2024,  1232,   578,   969,  1414, -1059,  1626,  1628,  1630,
    1374,  1336,  2336,  1341,  2338,   118, -1148, -1164,  1390,  1394,
     579,  1422,   122,  1259,   835,  1444,  1342,   580,  1462,  2352,
     585,  1391,  1395,    96,  1423,  1465,  2335,   417,  1445,  1284,
   -1167,  1463, -1101,  2248,  2249,  2250,   985,  1193,  1466,  1555,
     986,  1199,  1468, -1131,  1375,  1016,  1205,   632,   633,  1209,
    1211,  1296,  1261,   109,  1299,  1469,  2025,  2026,  1476,  2027,
    2028,  1482,   417,  -792,  -792,  1830,  2274,  1285,   988,   586,
    1490,  1765,  1490,  1831,  1483,  1533,  2056,  2057,  2058,  2059,
    2060,  2400,  1868,  1541,  1543,  2093,   592,   587,  1534,  1478,
    2029,   636,   636,   636,   636,   636,  1542,  1544,  2432,   995,
     995,   627,  2399,  1634,   642,  1330,  1330,  1330,  1330,  1330,
    2439,  1291,  2052,  1292,  2025,  2026,  1635,  2027,  2028,  -385,
     593,   650,  1765,  1505,   398,  2211,   781,   615,   651,  1508,
    1238,   969,  2030,  1511,   663,  1513,  1617,  1378,  2193,  1515,
    1382,  1517,   652,  1638,  1955,  2031,   678,  -230,  2029,   781,
    1046,  1335,  1646,  1650,  1616,  1732,  2076,  2077,   653,   699,
    1619,   654,  1676,  1640,  1643,  1647,  1651,  1214,  1215,  1001,
    1001,  1001,  1728,  1238,  1742,  1677,   658,   659,  1742,  1394,
    1636,  1887,   655,  1238,  1415,  1729,  1731,  1426,   656,   722,
    2030,  1743,  1903,  1644,  1238,  1743,  1924,  1668,  1444,  1669,
    1216,  1614,  1963,  2031,  1897,   747,   657,  1930,  1482,  1967,
    1240,  1931,  1666,   660,  1441,  1964,  1398,  2094,  1992,  2212,
     760,  1965,  1968,  2099,   669,  1648,  1655,  1456,   670,  1742,
     601,  1993,  2012,   685,   602,   603,   604,   605,  1527,  2076,
    2077,  1660,  2035,  1662,  2181,  2013,  1743,   606,   693,  1728,
    1490,  2020,  1766,  1438,   607,  2036,   608,  1685,  1464,  1893,
    1628,  1630,  2037,  1564,  1565,  1744,  1939,  1482,  1745,  1744,
    2010,  2011,  1745,  1746,   708,  2213,  1767,  1746,   705,  1574,
    2038,  1747,   711,   985,   609,  1747,  1768,   986,  2186,  2187,
    2188,  1769,  1733,  2214,   632,   633,  1728,   720,  1465,  1346,
     726,   987,  1494,  1766,  1736,  2215,   739,  2216,  2217,  2039,
    2218,  2040,   610,  2219,  1254,   988,   744,  1255,  1256,  1048,
    1744,  1049,   757,  1745,   601,  2193,   769,  1767,   602,   603,
     604,   605,   754,  1465,  1217,  1218,   764,  1768,  1219,  1220,
    2053,   606,  1769,  1890,   765,  1525,  2041,  1664,   607,   417,
     608,  2073,  -587,  2054,   768,  1535,  1536,  1537,  1817,   776,
    2104,   816,   771,  1527,  2074,  1882,  1440,   881,   772,   611,
    1527,   828,   886,  2105,   778,  2303,  2109,   892,   609,  1556,
     900,  1506,   780,  2110,   784,  1527,   978,  1509,  2170,  2207,
     785,  1512,  1175,  1514,  1512,  1512,  1698,  1516,  2111,  1518,
     812,  2171,  2208,  1566,  2220,  2230,   610,  1482,   820,   690,
    1894,  1895,  1896,  1296,  1981,   845,  1983,  1984,   846,  1527,
    2232,   612,  2279,   873,   874,   875,   876,   878,  1988,  1885,
     849,  2084,  2265,   851,  2312,  2280,  1755,  2073,  1995,  1760,
    1997,   897,  1780,   417,  1793,  2330,  1799,  2313,  1803, -1188,
    2322,  1238,   909,  1901,   989,  1482,   870,  1923,  2331,  2350,
    1444,  1527,   995,   611,  2345,  1907,  1343,  1482,  2348,  1909,
    1344,   601,  2351,  2370,  2371,   602,   603,   604,   605,   871,
    2373,   938,   940,   941,  1632,   942,  1728,   872,   606,  2076,
    2077,  1573,   943,   936,  2035,   607,   879,   608,   880,  2375,
    1337,   965,   793,   632,   633,   601,   816,  2405,  2035,   602,
     603,   604,   605,  2411,   606,   612,  2282,  1848,  1850,  2413,
    1444,  2406,   606,   608,   645,   609,  2412,  1658,  1658,   607,
    1658,   608,  2414,  2416,   631,   632,   633,  1482,   702,  1175,
    1075,  1238,  1075,   599,   600,  1670,   606,   912,  1021,  2435,
    2417,   609,  2418,   610,  2431,   608,   985,  2420,  2455,   609,
     986,   966,  2436,  1031,   967,  1610,  1695,   632,   633,  1354,
    1355,  2456,  1353,   968,   987,   637,   969,  1693,  2471,   610,
    1827,  1828,  1645,   609,  2461,  2462,  1145,   610,   988,   972,
     417,  1052,  1706,   983,  1197,  1198,  1708,  1201,  1068,  2382,
    2383,   993,   690,  1013,  1072,  1073,  1207,  1208,  1022,  1741,
     611,   610,   801,  1024,  1026,  1734,   634,  1735,  1028,  1738,
    1577,  1578,  1579,  1035,   417,  1227,  1227,  1227,  1041,  1227,
    1227,  1045,  1580,  1812,   401,  1066,   611,  1069,  1071,   963,
    1087,  1581,  1807,  1088,   611,  1094,  1089,   781,   634,  1102,
    1130,  1227,  1104,  1232,  1131,  1158,  1169,  1702,  1172,   985,
    1173,   978,   612,   986,  1582,  1182,  1822,  1185,   611,  1583,
     632,   633,  1188,  1196,  1202,    25,  1829,   987,  1225,  1238,
      29,  1264,  1986,  1127,   588,  1836,  1303,  1989,   612,  1300,
     945,   988,  1324,  1325,  -342,   986,   612,  1584,  1356,  1362,
    1999,  2095,    47,    48,  1363,  1385,  1858,  1383,  1407,  2097,
    1408,  1429,  1393,  1409,  2084,  1410,  1411,  1412,  1421,  1869,
     612,  1424,  2001,  1189,  1190,   636,   995,  1427,  2003,  1433,
    -931,  1442,  2005,  1443,  1461,  1448,   559,  1467,  1670,  1449,
    1454,  1670,  1670,  1670,  1477,  1453,  1480,  1481,  1485,  2305,
    1484,  1898,  1495,  1486,  1585,  2140,    90,  1497,  1487,  1527,
    1504,  1519,  1570,   915,   985,  1520,    95,   601,   986,  1507,
    1521,   602,   603,   604,   605,   632,   633,  1522,  1523,  1524,
    1472,  1528,   987,   702,   606,  1227,  1227,  1227,  1529,  1545,
    1546,   607,  1548,   608,  1549,  1550,   988,  1552,  1553,  1558,
    1563,  1601,  1609,  1932,  1610,  1604,  1586,  1615,   369,  1620,
    1623,  1624,   121,  1625,   399,  1098,  1639,  1633,  2339,  1490,
    1082,   609,  1304,  1305,  1306,  1307,  1308,  1309,  1310,  1311,
    1312,  1313,  1314,  1315,  1316,  1317,  1318,  1319,  1320,  1321,
    1322,  1665,  2233,   978,  1656,   560,  1673,   816,  1678,   610,
    1679,  1680,  1338,  1681,  1974,  1975,   150,  1439,  1692,  1345,
    1703,  1347,  1712,  1730,  1814,  1833,  1816,  1815,  2380,   944,
    1835,  1851,  2175,   945,  1861,  1852,  1357,  1877,  1864,  1991,
     946,   947,  2178,  1879,  1902,  1904,   417,   949,  1853,   951,
    2398,  1098,   953,   954,   955,   956,   957,  1854,  1855,  2136,
    1882,   960,   878,  1883,  1915,  1916,   611,  1917,  1388,  1919,
    2298,  2299,  2148,  -494,  1933,  1934,  1941,  2151,  1940,  1957,
    2152,  1958,  1960,   978,  1052,  1961,  1962,   944,  1969,  1972,
    2159,   945,  2160,  1973,  2161,  2398,  2162,  1733,   946,   947,
      13,    14,  1732,    15,    16,   949,  1978, -1189,    20,  1979,
   -1189, -1189, -1189, -1189, -1189,  1982,    23,  1985,   612,   960,
    1994,    27,  1996,  1998,    30,  2007,  2016,  2008,  1267,  2017,
    2009,  2014,    37,  2015,    38,  2055,    40,  2019,  2042,  2047,
    -381,  2062,  2067,  1869,  1869,  1869,  2063,  2075,  2080,  2334,
    2083,  2337,  2087,  2086,  2088,  1869,  2281,  2172,  2089,    59,
    2092,  2108,  2090,  2091,  2102,  2143,  2144,   781,  2167,  1457,
      70,  1268,  2166,   781,  2123,  2125,  2168,  1269,  2173,  -224,
    2180,  2197,   678,  2205,  2210,  2225,  2223,  2224,  1471,  2226,
    2227,  -843,  2229,  2231,    85,  2252,  2258,  2266,  2270,  2271,
    2107,  2273,  2283,  1270,   976,  2284,  2277,    93,   977,  2291,
    2293,   878,  1492,   828,   793,   632,   633,  2294,  2295,  2296,
    2297,  2121,  2300,  2301,  2307,   102,   606,  1271,  2311,  1272,
    2316,   104,  2315,  2318,  1706,   608,  2319,  2321,  1898,   108,
    2129,   110,  2346,   112,  2349,   114,  2363,  2361,  2369,  2372,
    2139,  2374,   119,  2378,  2387,  2381,  2145,  1738,  2385,  2394,
    2395,  2404,  1273,   609,  2407,  2150,  2415,  1274,  2429,   130,
     131,  1275,  2421,   629,  2156,  1276,  1538,   630,  1277,  2433,
    2437,  2442,  1227,   631,   632,   633,  2457,   143,  2458,  2459,
    2470,   610,  1807,   572,   598,   606,  2452,  1557,  1260,  1265,
    1278,  2126,  1437,  1834,   608,  2048,  1500,  1267,   155,  1837,
     917,   156,  2272,  2049,  1279,  2169,   926,   929,  1042,  1576,
    1280,  1297,  1862,  2275,  2183,  1298,  1572,  2069,   634,  2061,
    2176,  1719,   609,  1475,  2222,  1956,  2124,  1701,  1605,  2236,
    2146,  1386,  1858,  1403,  1652,  1637,  1432,  1925,   611,  2469,
    1268,  1869,  1926,  1869,  1869,  1869,  1869,  1869,  1869,   432,
     610,  2098,  2085,  1227,  1043,   978,  1612,    13,    14,  1642,
      15,    16,   782,   594,  1672,    20,  1148,  1392,  1618,   595,
    1888,  2332,  1270,    23,  1889,  2203,  1006,  1359,    27,  1418,
    1446,    30,  1493,  1177,  1008,  1010,   596,   634,  1009,    37,
     612,    38,  1011,    40,   597,  2304,  1271,  1654,  1272,  2202,
    1657,  2424,  2324,  2127,  1377,  2408,   742,   611,  1156,  1376,
     809,  2235,  1975,  1431,  2096,  1387,    59,  1641,  1031,  1697,
    2365,  2122,  1396,  2267,  1227,  2425,  2333,    70,  2441,  2463,
    2465,  1273,  1653,  2310,  1970,   742,  1274,  1718,  2134,  1661,
    1275,   839,   742,  1015,  1276,  1684,  2100,  1277,  1869,   878,
     802,    85,  1095,   681,   729,  1671,  2286,  1674,  2135,   612,
    1813,  2000,  2004,  1987,    93,  2006,  2002,  1990,  2153,  1278,
     878,  2155,  2314,  2163,  1526,  1547,  1551,  1554,     0,     0,
       0,     0,   102,  1279,  1689,   821,   821,     0,   104,  1280,
       0,     0,     0,     0,  2129,  1700,   108,     0,   110,     0,
     112,     0,   114,     0,     0,     0,     0,     0,  1714,   119,
       0,     0,  2191,  1720,  1456,  1726,     0,   601,     0,     0,
       0,   602,   603,   604,   605,     0,   130,   131,     0,     0,
       0,     0,     0,     0,   606,   792,     0,     0,     0,     0,
       0,   607,     0,   608,   143,   793,   632,   633,     0,     0,
    1227,  2326,     0,     0,     0,     0,     0,   606,     0,   925,
       0,  1096,     0,  1096,     0,   155,   608,  1227,   156,     0,
       0,   609,     0,  1721,  2357,     0,     0,  2347,   601,     0,
       0,     0,   602,   603,   604,   605,  2355,  2358,     0,     0,
       0,     0,     0,     0,   609,   606,     0,     0,     0,   610,
       0,  1842,   607,     0,   608,     0,  1048,     0,  2389,     0,
       0,   601,     0,     0,     0,   602,   603,   604,   605,  1822,
    1822,     0,   610,   742,     0,     0,     0,     0,   606,     0,
       0,     0,   609,     0,     0,   607,   417,   608,     0,     0,
       0,     0,  1886,   816,  1338,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  2358,   611,  2409,     0,   634,
     610,     0,     0,     0,     0,   609,     0,     0,     0,     0,
       0,     0,     0,  1906,     0,     0,  1908,     0,     0,   611,
    1841,     0,     0,     0,  1912,   601,     0,     0,     0,   602,
     603,   604,   605,   610,     0,     0,     0,   417,     0,     0,
       0,     0,   606,     0,     0,     0,     0,  2443,   612,   607,
       0,   608,  2444,     0,     0,  1078,  1935,   611,  1938,     0,
       0,     0,     0,     0,     0,  2444,     0,  2464,  2467,     0,
     417,   612,     0,  1097,     0,     0,     0,     0,     0,   609,
    2467,     0,     0,     0,     0,     0,     0,  1129,     0,     0,
     611,     0,     0,  2050,     0,  1135,  1136,     0,   601,     0,
    1142,  2287,   602,   603,   604,   605,   601,   610,     0,   612,
     602,   603,   604,   605,     0,   606,     0,     0,     0,     0,
       0,     0,   607,   606,   608,     0,     0,     0,  1097,     0,
     607,     0,   608,     0,     0,     0,  1170,     0,     0,     0,
       0,     0,   612,     0,   417,     0,     0,     0,   827,     0,
       0,   601,   609,     0,     0,   602,   603,   604,   605,     0,
     609,     0,     0,     0,   611,  1192,     0,  1195,   606,     0,
       0,     0,     0,     0,     0,   607,     0,   608,     0,     0,
     610,     0,     0,     0,     0,     0,   742,   742,   610,     0,
       0,     0,     0,     0,     0,  1239,  1243,     0,  2043,  2044,
       0,   827,     0,     0,   601,   609,  2051,     0,   602,   603,
     604,   605,     0,     0,     0,     0,   612,   417,     0,  2137,
       0,   606,     0,     0,     0,   417,     0,     0,   607,     0,
     608,     0,     0,   610,     0,     0,     0,   611,     0,     0,
       0,     0,     0,     0,     0,   611,     0,  2082,     0,     0,
       0,     0,     0,  1612,     0,     0,     0,     0,   609,     0,
       0,     0,     0,     0,     0,  1030,     0,     0,   601,     0,
     417,     0,   602,   603,   604,   605,     0,     0,  2101,   962,
       0,     0,     0,  1329,  1329,   606,   610,  2106,     0,   612,
     611,     0,   607,     0,   608,     0,     0,   612,     0,     0,
       0,     0,     0,     0,  2116,  2117,     0,     0,  1456,   742,
     742,   601,     0,     0,     0,   602,   603,   604,   605,     0,
       0,     0,   609,   417,     0,     0,     0,     0,   606,     0,
       0,  1714,     0,     0,     0,   607,  1239,   608,     0,   828,
       0,     0,   612,   611,     0,     0,     0,     0,  2147,     0,
     610,     0,     0,  1688,     0,     0,   601,     0,     0,  1397,
     602,   603,   604,   605,  2157,   609,  2158,     0,     0,     0,
       0,     0,     0,   606,     0,     0,     0,     0,     0,     0,
     607,     0,   608,     0,     0,     0,     0,   417,     0,     0,
       0,     0,     0,   610,   944,   612,     0,   742,   945,   601,
       0,     0,     0,     0,     0,   946,   947,   611,     0,     0,
     609,   948,   949,   950,   951,   952,     0,   953,   954,   955,
     956,   957,   958,     0,   959,     0,   960,   961,     0,  1123,
     417,     0,     0,   962,     0,     0,     0,  1713,   610,     0,
     601,     0,     0,     0,   602,   603,   604,   605,     0,     0,
     611,     0,     0,     0,     0,  2190,     0,   606,     0,   612,
    2192,     0,     0,     0,   607,     0,   608,     0,     0,     0,
       0,  2201,  1912,     0,     0,   417,  2206,     0,     0,     0,
     962,     0,     0,     0,     0,     0,  2209,     0,  1097,     0,
       0,  2221,   962,     0,   609,   611,  1937,     0,     0,   601,
       0,     0,   612,   602,   603,   604,   605,     0,     0,   944,
       0,     0,     0,   945,     0,     0,   606,     0,     0,     0,
     946,   947,   610,   607,     0,   608,   948,   949,     0,   951,
       0,     0,   953,   954,   955,   956,   957,   962,   962,   962,
     962,   960,   962,  1572,     0,     0,     0,   612,     0,     0,
       0,     0,     0,   609,     0,     0,     0,     0,   601,   417,
       0,     0,   602,   603,   604,   605,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   606,     0,     0,  2288,   611,
       0,   610,   607,     0,   608,     0,     0,     0,     0,     0,
       0,     0,     0,   878,     0,     0,     0,     0,     0,     0,
       0,     0,   962,     0,   962,   962,   962,   962,  1575,   944,
       0,  1690,   609,   945,   601,     0,     0,     0,   417,     0,
     946,   947,     0,     0,     0,  1691,   948,   949,   950,   951,
       0,   612,   953,   954,   955,   956,   957,   958,   611,   959,
     610,   960,   961,     0,     0,     0,  2323,  2192,     0,     0,
    2325,     0,  2329,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   962,     0,     0,     0,     0,  2341,  2342,
    2344,     0,   742,     0,     0,     0,     0,   417,     0,     0,
    2354,     0,     0,     0,     0,   962,  2356,     0,     0,     0,
     612,     0,     0,     0,     0,   962,     0,   611,  2288,     0,
       0,     0,   601,     0,     0,     0,   602,   998,   604,   605,
       0,     0,     0,     0,     0,     0,   962,     0,     0,   606,
       0,     0,     0,     0,     0,     0,   607,     0,   608,     0,
    2388,  2390,   962,     0,     0,     0,   962,   962,  1078,     0,
    1078,     0,   828,     0,     0,  2402,     0,  2403,     0,   612,
       0,     0,     0,     0,     0,     0,   609,     0,     0,  1097,
    1097,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1686,     0,     0,     0,     0,   878,   742,     0,  1696,  2390,
       0,  1142,     0,  2426,   610,     0,     0,  2430,     0,  1726,
       0,     0,     0,   802,   962,     0,     0,  1097,  1097,     0,
       0,  1726,  2440,  2426,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   742,
       0,   417,     0,     0,     0,     0,  1762,     0,     0,     0,
       0,     0,     0,     0,     0,   742,   742,   742,     0,   742,
     742,   611,     0,   742,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   962,   962,     0,     0,  1239,     0,  1820,
       0,   742,     0,     0,  1097,  1097,  1097,     0,  1142,  1142,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1105,     0,     0,     0,  1106,   601,     0,
       0,     0,     0,   612,  1107,  1108,     0,     0,     0,     0,
    1109,  1110,  1111,  1112,  1860,     0,  1113,  1114,  1115,  1116,
    1117,  1118,  1119,  1120,     0,  1121,  1122,     0,     0,     0,
     944,     0,  1174,     0,   945,   601,     0,     0,     0,     0,
       0,   946,   947,     0,     0,     0,   742,   948,   949,   950,
     951,     0,     0,   953,   954,   955,   956,   957,   958,     0,
     959,     0,   960,   961,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   962,   962,
     962,   962,   962,   962,   962,   962,   962,   962,   962,   962,
     962,   962,   962,   962,   962,   962,   962,     0,     0,     0,
       0,     0,     0,     0,     0,   742,   742,   742,     0,     0,
     371,     0,   962,     0,     0,     0,   377,     0,     0,   962,
       0,   962,     0,     0,     0,     0,   384,     0,     0,   386,
       0,   962,   389,     0,     0,     0,     0,     0,     0,   395,
       0,     0,     0,   402,     0,     0,     0,   405,     0,     0,
       0,     0,     0,     0,  1097,     0,     0,     0,     0,     0,
       0,     0,   962,     0,     0,   424,     0,     0,     0,   428,
     429,     0,     0,     0,     0,   434,   435,     0,     0,     0,
       0,   440,   441,     0,   443,   444,   445,   446,     0,   447,
       0,     0,     0,     0,     0,     0,     0,     0,   456,     0,
       0,     0,     0,   460,     0,   462,     0,     0,     0,   465,
       0,     0,     0,   469,     0,   471,     0,     0,     0,     0,
       0,     0,   477,     0,     0,     0,   481,     0,     0,     0,
     484,   962,   486,     0,     0,     0,     0,     0,     0,   494,
     496,     0,     0,   498,   499,   962,     0,     0,     0,   505,
       0,   506,     0,     0,     0,   510,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   962,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1329,  1329,  1329,
    1329,  1329,   537,   944,   539,     0,  1235,   945,   601,     0,
       0,     0,     0,     0,   946,   947,     0,     0,     0,     0,
     948,   949,   950,   951,     0,     0,   953,   954,   955,   956,
     957,   958,   962,   959,     0,   960,   961,   944,     0,     0,
    1236,   945,   601,     0,     0,     0,     0,  1239,   946,   947,
       0,     0,     0,  1397,   948,   949,   950,   951,     0,     0,
     953,   954,   955,   956,   957,   958,   962,   959,     0,   960,
     961,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  2119,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   944,     0,   962,
    1237,   945,   601,     0,     0,     0,   962,   802,   946,   947,
       0,     0,   962,     0,   948,   949,   950,   951,  1097,     0,
     953,   954,   955,   956,   957,   958,     0,   959,     0,   960,
     961,     0,     0,     0,     0,   962,     0,     0,  2154,     0,
       0,     0,     0,     0,     0,     0,     0,   962,     0,     0,
       0,     0,     0,     0,     0,   962,     0,     0,     0,     0,
       0,     0,   742,  2165,     0,   962,     0,     0,   962,     0,
       0,     0,     0,   944,     0,     0,     0,   945,   601,     0,
       0,     0,     0,   962,   946,   947,     0,     0,     0,  1302,
     948,   949,   950,   951,   962,     0,   953,   954,   955,   956,
     957,   958,     0,   959,     0,   960,   961,     0,   962,     0,
    1860,     0,     0,     0,   962,     0,     0,     0,   714,     0,
     962,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     944,     0,     0,     0,   945,   601,     0,     0,     0,     0,
    2196,   946,   947,   742,     0,  2199,  1381,   948,   949,   950,
     951,     0,     0,   953,   954,   955,   956,   957,   958,     0,
     959,     0,   960,   961,   944,     0,  1389,     0,   945,   601,
       0,     0,     0,     0,     0,   946,   947,     0,     0,     0,
       0,   948,   949,   950,   951,     0,     0,   953,   954,   955,
     956,   957,   958,     0,   959,     0,   960,   961,     0,     0,
     944,     0,  1404,     0,   945,   601,     0,     0,     0,     0,
       0,   946,   947,   742,   742,     0,     0,   948,   949,   950,
     951,     0,   841,   953,   954,   955,   956,   957,   958,     0,
     959,     0,   960,   961,   944,     0,   962,     0,   945,   601,
       0,     0,     0,     0,     0,   946,   947,     0,     0,     0,
    1416,   948,   949,   950,   951,     0,     0,   953,   954,   955,
     956,   957,   958,     0,   959,     0,   960,   961,     0,     0,
       0,   944,     0,  1097,     0,   945,   601,  1097,     0,     0,
     962,     0,   946,   947,     0,     0,     0,  1419,   948,   949,
     950,   951,     0,     0,   953,   954,   955,   956,   957,   958,
     962,   959,   962,   960,   961,     0,   962,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   962,
       0,     0,   962,     0,     0,     0,     0,     0,     0,  1097,
     742,  1097,  1097,  1097,   944,     0,     0,     0,   945,   601,
       0,     0,  1123,     0,     0,   946,   947,   742,     0,     0,
    1420,   948,   949,   950,   951,     0,     0,   953,   954,   955,
     956,   957,   958,     0,   959,     0,   960,   961,   944,     0,
       0,     0,   945,   601,     0,     0,     0,     0,     0,   946,
     947,     0,  1097,     0,  1455,   948,   949,   950,   951,     0,
       0,   953,   954,   955,   956,   957,   958,     0,   959,     0,
     960,   961,     0,     0,  1097,  1097,   944,     0,  1097,     0,
     945,   601,     0,     0,     0,     0,     0,   946,   947,     0,
       0,     0,  1502,   948,   949,   950,   951,   962,   962,   953,
     954,   955,   956,   957,   958,   962,   959,     0,   960,   961,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1097,
       0,   944,     0,     0,     0,   945,   601,     0,     0,     0,
       0,     0,   946,   947,     0,     0,   962,  1503,   948,   949,
     950,   951,     0,     0,   953,   954,   955,   956,   957,   958,
       0,   959,     0,   960,   961,   962,     0,   944,     0,     0,
     962,   945,   601,     0,     0,     0,     0,     0,   946,   947,
     962,   962,     0,  1622,   948,   949,   950,   951,     0,     0,
     953,   954,   955,   956,   957,   958,     0,   959,     0,   960,
     961,     0,     0,     0,     0,     0,     0,     0,   944,     0,
       0,   962,   945,   601,     0,     0,     0,     0,     0,   946,
     947,   962,   962,     0,  1880,   948,   949,   950,   951,     0,
       0,   953,   954,   955,   956,   957,   958,     0,   959,     0,
     960,   961,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   962,     0,   962,     0,     0,     0,
       0,     0,     0,     0,   944,   962,  1690,     0,   945,   601,
     962,     0,     0,   962,     0,   946,   947,     0,     0,     0,
       0,   948,   949,   950,   951,   962,     0,   953,   954,   955,
     956,   957,   958,     0,   959,   944,   960,   961,     0,   945,
     601,  1286,     0,     0,     0,     0,   946,   947,     0,     0,
       0,  1918,   948,   949,   950,   951,     0,     0,   953,   954,
     955,   956,   957,   958,     0,   959,     0,   960,   961,     0,
     944,     0,  1936,     0,   945,   601,     0,     0,     0,     0,
       0,   946,   947,     0,     0,     0,     0,   948,   949,   950,
     951,     0,   962,   953,   954,   955,   956,   957,   958,     0,
     959,     0,   960,   961,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   962,     0,   962,
       0,     0,     0,   962,     0,     0,     0,     0,     0,     0,
       0,  1366,     0,     0,     0,   962,   962,     0,   962,     0,
     944,     0,     0,     0,   945,   601,     0,     0,   962,     0,
     962,   946,   947,     0,     0,     0,  1966,   948,   949,   950,
     951,     0,     0,   953,   954,   955,   956,   957,   958,     0,
     959,     0,   960,   961,     0,     0,     0,     0,     0,     0,
       0,     0,   962,     0,   962,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   962,   962,     0,   373,
     374,   375,   376,     0,   378,     0,   379,     0,   381,   382,
       0,   383,     0,     0,     0,   385,     0,   387,   388,     0,
     962,   390,   391,   392,   962,   394,     0,   396,   397,     0,
       0,   404,     0,     0,   962,     0,   407,   408,   409,   410,
     411,   412,   413,   414,     0,   415,   416,     0,     0,   421,
     422,   423,     0,   425,     0,   427,     0,     0,   430,   431,
       0,     0,     0,     0,     0,     0,   438,   439,     0,     0,
     442,     0,     0,     0,     0,     0,     0,   448,     0,   450,
       0,   451,   452,     0,     0,     0,   457,   458,   459,     0,
       0,   461,     0,     0,   463,   464,     0,   466,   467,   468,
       0,     0,     0,     0,   473,   474,     0,     0,   476,     0,
     478,   479,   480,     0,   482,   483,     0,     0,   485,     0,
     487,   488,   489,   490,   491,   492,     0,     0,     0,     0,
       0,     0,   500,   501,   502,     0,     0,     0,     0,   507,
     508,   509,     0,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,     0,
     538,     0,   540,     0,   541,     0,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,     0,  1562,     9,   944,
       0,  2103,     0,   945,   601,    10,     0,     0,     0,     0,
     946,   947,     0,     0,     0,     0,   948,   949,   950,   951,
       0,     0,   953,   954,   955,   956,   957,   958,     0,   959,
       0,   960,   961,     0,     0,    11,    12,    13,    14,     0,
      15,    16,    17,    18,    19,    20,     0,   628,    21,    22,
     641,     0,   644,    23,    24,    25,     0,    26,    27,    28,
      29,    30,    31,     0,    32,    33,    34,    35,    36,    37,
       0,    38,    39,    40,    41,    42,    43,     0,     0,    44,
      45,    46,    47,    48,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    71,
       0,    72,    73,     0,    74,    75,    76,     0,     0,    77,
       0,     0,    78,    79,     0,    80,    81,    82,    83,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     0,
       0,     0,     0,     0,    93,    94,    95,    96,     0,     0,
       0,     0,    97,     0,     0,    98,    99,     0,     0,   100,
     101,     0,   102,     0,     0,     0,   103,     0,   104,     0,
     105,     0,     0,     0,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,     0,   116,   117,   118,     0,   119,
       0,   120,   121,     0,   122,     0,   123,   124,   125,   126,
       0,     0,   127,   128,   129,     0,   130,   131,   132,     0,
     133,   134,   135,     0,   136,     0,   137,   138,   139,   140,
     141,     0,   142,     0,   143,   144,     0,     0,   145,   146,
     147,     0,     0,   148,   149,     0,   150,   151,     0,   152,
     153,     0,     0,     0,   154,   155,     0,     0,   156,     0,
       0,   157,     0,     0,     0,   158,   159,     0,     0,   160,
     161,   162,     0,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,     0,   174,     0,     0,   175,     0,
       0,     0,   176,   177,   178,   179,   180,     0,   181,   182,
       0,     0,   183,   184,   185,   186,     0,     0,     0,     0,
     187,   188,   189,   190,   191,   192,     0,     0,     0,     0,
       0,     0,     0,     0,   193,   194,   195,   196,   197,     0,
       0,     0,   198,   199,   200,   201,   202,   203,     0,   204,
     205,   944,     0,  2118,     0,   945,   601,     0,     0,     0,
       0,     0,   946,   947,     0,     0,     0,     0,   948,   949,
     950,   951,     0,     0,   953,   954,   955,   956,   957,   958,
       0,   959,     0,   960,   961,     0,   944,     0,     0,     0,
     945,   601,     0,     0,     0,     0,     0,   946,   947,     0,
       0,     0,  2174,   948,   949,   950,   951,   934,   935,   953,
     954,   955,   956,   957,   958,     0,   959,   944,   960,   961,
       0,   945,   601,     0,     0,     0,     0,     0,   946,   947,
       0,     0,     0,  2189,   948,   949,   950,   951,     0,     0,
     953,   954,   955,   956,   957,   958,     0,   959,   944,   960,
     961,  2200,   945,   601,     0,     0,     0,     0,     0,   946,
     947,     0,     0,     0,     0,   948,   949,   950,   951,     0,
       0,   953,   954,   955,   956,   957,   958,     0,   959,   944,
     960,   961,     0,   945,   601,     0,     0,     0,     0,     0,
     946,   947,     0,     0,     0,  2204,   948,   949,   950,   951,
       0,     0,   953,   954,   955,   956,   957,   958,     0,   959,
     944,   960,   961,     0,   945,   601,     0,     0,     0,     0,
       0,   946,   947,     0,     0,     0,  2263,   948,   949,   950,
     951,     0,     0,   953,   954,   955,   956,   957,   958,     0,
     959,   944,   960,   961,     0,   945,   601,     0,     0,     0,
       0,     0,   946,   947,     0,     0,     0,  2264,   948,   949,
     950,   951,     0,     0,   953,   954,   955,   956,   957,   958,
       0,   959,   944,   960,   961,  2278,   945,   601,     0,     0,
       0,     0,     0,   946,   947,     0,     0,     0,     0,   948,
     949,   950,   951,     0,     0,   953,   954,   955,   956,   957,
     958,     0,   959,   944,   960,   961,     0,   945,   601,     0,
       0,     0,     0,     0,   946,   947,     0,     0,     0,  2285,
     948,   949,   950,   951,     0,     0,   953,   954,   955,   956,
     957,   958,     0,   959,   944,   960,   961,  2302,   945,   601,
       0,     0,     0,     0,     0,   946,   947,     0,     0,     0,
       0,   948,   949,   950,   951,     0,     0,   953,   954,   955,
     956,   957,   958,     0,   959,   944,   960,   961,  2362,   945,
     601,     0,     0,     0,     0,     0,   946,   947,     0,     0,
       0,     0,   948,   949,   950,   951,     0,     0,   953,   954,
     955,   956,   957,   958,     0,   959,   944,   960,   961,     0,
     945,   601,     0,     0,     0,     0,     0,   946,   947,     0,
       0,     0,  2364,   948,   949,   950,   951,     0,     0,   953,
     954,   955,   956,   957,   958,     0,   959,   944,   960,   961,
       0,   945,   601,     0,     0,     0,     0,     0,   946,   947,
       0,     0,     0,  2376,   948,   949,   950,   951,     0,     0,
     953,   954,   955,   956,   957,   958,     0,   959,   944,   960,
     961,  2377,   945,   601,     0,     0,     0,     0,     0,   946,
     947,     0,     0,     0,     0,   948,   949,   950,   951,     0,
       0,   953,   954,   955,   956,   957,   958,     0,   959,   944,
     960,   961,  2379,   945,   601,     0,     0,     0,     0,     0,
     946,   947,     0,     0,     0,     0,   948,   949,   950,   951,
       0,     0,   953,   954,   955,   956,   957,   958,     0,   959,
     944,   960,   961,     0,   945,   601,     0,     0,     0,     0,
       0,   946,   947,     0,     0,     0,  2384,   948,   949,   950,
     951,     0,     0,   953,   954,   955,   956,   957,   958,     0,
     959,   944,   960,   961,     0,   945,   601,     0,     0,     0,
       0,     0,   946,   947,     0,     0,     0,  2386,   948,   949,
     950,   951,     0,     0,   953,   954,   955,   956,   957,   958,
       0,   959,   944,   960,   961,  2419,   945,   601,     0,     0,
       0,     0,     0,   946,   947,     0,     0,     0,     0,   948,
     949,   950,   951,     0,     0,   953,   954,   955,   956,   957,
     958,     0,   959,     0,   960,   961,   944,     0,  2434,     0,
     945,   601,     0,     0,     0,     0,     0,   946,   947,     0,
       0,     0,     0,   948,   949,   950,   951,     0,     0,   953,
     954,   955,   956,   957,   958,     0,   959,   944,   960,   961,
       0,   945,   601,     0,     0,     0,     0,     0,   946,   947,
       0,     0,     0,     0,   948,   949,   950,   951,     0,     0,
     953,   954,   955,   956,   957,   958,     0,   959,   944,   960,
     961,     0,   945,     0,     0,     0,     0,     0,     0,   946,
     947,     0,     0,     0,     0,   948,   949,   950,   951,     0,
       0,   953,   954,   955,   956,   957,   958,     0,   959,   944,
     960,   961,     0,   945,     0,     0,     0,     0,     0,     0,
     946,   947,     0,     0,     0,     0,   948,   949,     0,   951,
       0,     0,   953,   954,   955,   956,   957,     0,     0,   959,
       0,   960,   961
};

static const yytype_int16 yycheck[] =
{
      50,   388,   624,   374,  1024,   371,  1443,  1131,   482,   479,
      62,    63,   489,   490,   491,   492,   744,    69,   879,   397,
    1481,   918,   468,   968,   628,   649,   650,   650,  1294,   407,
    1294,   409,   383,   639,  1020,   426,   414,   415,   416,   537,
     254,  1323,   998,   207,   422,   259,    98,   425,  1548,   427,
    1550,   967,   968,  1571,   256,  1588,   463,   259,   260,   871,
    1971,    76,  1148,   429,  1477,   839,  1601,   737,   118,   603,
    1064,  1577,   122,  1577,     8,  1933,   438,  1597,  1513,   839,
     446,    63,  1602,  1545,    99,    15,  1861,  1513,  1549,  1513,
       8,     5,   690,   464,   460,  1356,   384,  1513,   175,   465,
     116,   116,     7,   469,   476,   471,     7,  1513,   706,   757,
    1513,  1513,   182,    94,    41,   649,   650,     8,   484,   182,
     974,    15,     0,     1,    71,  1643,   667,   129,    21,   144,
     496,   473,   498,     7,    15,     8,     5,    37,     7,   505,
       5,    63,     8,    47,    58,     8,    68,    20,    64,    15,
       5,    15,   204,  1173,   266,    15,    21,    20,   169,  1020,
     402,     5,     8,   403,   166,    15,  1160,   509,    90,    91,
     222,     3,    86,    21,    20,   287,   116,  1511,   129,  1513,
     129,  1515,  1043,  1517,    15,   631,   116,     8,   794,   635,
     254,   169,     8,    37,    26,   259,   642,   208,   116,   129,
      23,    15,   384,  1511,    20,  1513,  1511,  1515,  1513,  1517,
    1515,   116,   126,   129,    37,   116,    37,  1679,  1680,  1681,
     129,   172,   144,   137,  1511,   129,  1513,    15,  1515,   129,
     128,    64,   154,   149,   476,   129,   129,    15,   129,   837,
       8,   257,   116,   116,   321,   315,   229,   230,   318,   230,
     848,   165,   235,   116,   468,   318,  1866,  1867,  1540,   157,
     129,   208,   803,   129,   129,   129,    15,   865,   149,   129,
     116,   282,   150,   177,   129,   515,   627,   517,   200,   129,
     209,   129,   209,   757,   524,   525,   526,  2062,  1185,   149,
     530,   531,   532,   533,   582,   535,   536,     3,   129,   641,
     116,   228,   644,   284,   282,  1429,   129,   905,   686,   231,
     239,     8,  1578,  1579,   295,    21,    15,   369,   370,   217,
     254,   919,   244,   375,   376,   377,   378,   257,   380,   381,
    1865,  1864,   384,   684,   386,   387,   388,   389,   812,   257,
     392,   129,  1862,   395,   258,   397,   718,   793,   116,  2207,
     402,   129,   257,   405,   745,   407,   257,   409,  1793,  1620,
       8,   413,   414,   415,   416,   969,   430,  1793,     8,  1793,
     422,  1287,   424,   425,   998,   427,   982,  1793,   984,   257,
      20,   433,   434,   257,   257,   991,   568,  1793,   440,   441,
    1793,  1793,     8,   445,   257,   459,   578,   579,   580,   581,
     452,  1421,   466,   455,   468,  1361,  1362,  1363,     3,  1007,
    1184,   257,    15,     8,  1012,   467,  1086,   631,   470,   116,
     782,   635,  1442,  1183,   476,   477,   478,   479,   642,   481,
     482,   413,   246,   805,  1288,   485,   488,   489,   490,   491,
     492,   257,   494,   834,     8,   497,  1780,   499,   500,     8,
     792,   503,   794,   191,   506,  1053,  1078,   129,   510,  1793,
     148,    20,    15,  2374,   998,  1799,  2076,  2077,   191,  1803,
     208,  2142,  1780,     8,  1328,  1780,   116,   149,     8,  1103,
    1103,    15,  1568,  1569,  1478,  1793,   668,   539,  1793,   257,
     540,  1799,   180,  1780,  1799,  1803,    16,    17,  2169,     8,
     116,    92,   552,   553,   554,  2363,  1793,     8,   915,   691,
     562,  1972,  1799,   565,   566,   567,   568,   147,   148,     8,
     570,   573,   574,   191,  1172,   577,   578,   579,   580,   581,
       8,  2064,    21,   585,   586,  1817,     8,    15,  2073,   985,
     986,   987,   988,   989,   559,   560,  1152,  1401,  1961,  2055,
     180,  2055,   116,  2411,  2016,  1541,   571,   116,   149,   282,
     257,   763,     3,     5,     8,   916,   806,   631,     3,  1103,
      23,   635,    92,     8,    16,    17,   129,     8,   642,   793,
      22,   116,    24,   647,    37,    26,   116,     8,     8,   753,
    1856,    33,  1856,    35,     5,    37,     8,  1861,     9,   651,
     652,   653,   654,   655,   656,   657,   788,   116,   660,   659,
     201,     8,   963,     8,    43,   116,   668,   257,     5,   191,
     288,    15,     9,     8,     8,    37,    37,   677,     8,   149,
       3,   299,   128,     8,   686,  2097,   208,   689,    67,   691,
       3,   257,    15,    23,   116,  2180,    21,     7,     8,   257,
      37,   833,    37,    82,   836,    18,     5,    37,    87,    88,
       8,   157,   714,  2457,  2458,    94,    15,  1623,  1624,  1625,
    1268,     5,   116,    21,    63,     9,  2470,  1301,  1301,    68,
    1541,   201,    16,    17,  1035,   116,   254,   739,  1286,   741,
     673,   259,   744,   257,  1045,   116,   116,   191,   257,  1427,
     194,    90,    91,    37,   754,   757,  2168,  1519,  1520,  1521,
     282,  1523,  1524,   285,     5,   767,     7,   159,   768,   116,
     162,   116,   257,     7,   776,   775,   778,   257,     8,   793,
      15,   783,   116,   785,     8,    95,   788,  1361,  1362,  1363,
       8,    21,  1348,  1349,  1350,  1351,  1352,    21,   257,   801,
      22,     7,    24,    21,  2272,   144,   257,     8,     5,    15,
     812,    33,  1616,    35,   124,   154,   818,  1301,    15,  1303,
    2231,   985,   986,   987,   988,   989,     8,     8,    15,    16,
      17,   833,     3,     8,   836,   257,     8,    89,     3,    21,
     842,   841,     3,   149,    15,  1443,  1444,  2297,  2062,   228,
      15,  1655,     8,     8,    15,   247,   870,   871,    22,     8,
      24,   200,     3,   257,  1054,   879,    21,  1415,     8,    33,
     122,    35,   872,     8,    15,  1065,   257,  1361,  1362,  1363,
     259,    21,  2294,     8,  2295,   195,   257,   257,     8,     8,
       8,     8,   202,   895,   896,     8,    21,     8,     8,  2311,
       8,    21,    21,   155,    21,     8,  2293,   129,    21,   911,
     257,    21,   257,   305,   306,   307,     5,   850,    21,  1260,
       9,   854,     8,   257,   303,     8,   859,    16,    17,   862,
     863,   933,   897,   185,   934,    21,   158,   159,    21,   161,
     162,     8,   129,     7,     8,    18,  2178,   912,    37,     8,
    1548,    84,  1550,    26,    21,     8,  1851,  1852,  1853,  1854,
    1855,  2372,   149,     8,     8,  1901,   218,     8,    21,  1159,
     192,   985,   986,   987,   988,   989,    21,    21,  2421,   993,
     994,    15,  2369,     8,    15,  1851,  1852,  1853,  1854,  1855,
    2433,   924,  1839,   926,   158,   159,    21,   161,   162,     3,
     252,     8,    84,  1193,     7,    41,  1020,   249,     8,  1199,
       8,    15,   234,  1203,  1016,  1205,  1337,  1017,  2092,  1209,
    1022,  1211,     8,    21,  1702,   247,  1028,     7,   192,  1043,
       8,     8,     8,     8,  1335,    71,    16,    17,     8,  1041,
    1341,     8,     8,    21,    21,    21,    21,   147,   148,  1623,
    1624,  1625,     8,     8,   191,    21,     7,     8,   191,     8,
    1381,  1615,     8,     8,  1066,    21,    21,  1081,     8,  1071,
     234,   208,    21,  1394,     8,   208,    21,  1422,     8,  1424,
     180,  1885,     8,   247,  1632,  1087,     8,    21,     8,     8,
    1901,    21,  1420,     8,  1094,    21,  1907,  1901,     8,   135,
    1102,    21,    21,  1907,    15,  1909,  1407,     7,    15,   191,
      10,    21,     8,    15,    14,    15,    16,    17,     8,    16,
      17,  1410,     8,  1412,    21,    21,   208,    27,    15,     8,
    1728,    21,   265,   169,    34,    21,    36,  1453,  1138,  1623,
    1624,  1625,    21,     7,     8,   282,  1694,     8,   285,   282,
     316,   317,   285,   290,   149,   191,   289,   290,    15,  1291,
      21,   298,     7,     5,    64,   298,   299,     9,  2078,  2079,
    2080,   304,   208,   209,    16,    17,     8,    15,     8,    21,
      15,    23,  1182,   265,  1500,   221,    15,   223,   224,    21,
     226,    21,    92,   229,   336,    37,    15,   339,   340,     5,
     282,     7,    15,   285,    10,  2279,   448,   289,    14,    15,
      16,    17,     8,     8,   314,   315,    15,   299,   318,   319,
       8,    27,   304,  1619,    15,  1225,    21,  1417,    34,   129,
      36,     8,     8,    21,    15,  1235,  1236,  1237,  1539,     8,
       8,   483,    15,     8,    21,    21,   282,  1247,    15,   149,
       8,   493,  1252,    21,    37,  2225,    21,  1257,    64,  1261,
    1262,  1194,    15,    21,    15,     8,   166,  1200,     8,     8,
      15,  1204,     8,  1206,  1207,  1208,  1466,  1210,    21,  1212,
      15,    21,    21,  1285,   320,    21,    92,     8,    15,  1291,
    1623,  1624,  1625,  1295,  1742,    15,  1744,  1745,    15,     8,
      21,   201,     8,   545,   546,   547,   548,   549,  1756,  1610,
      15,  1883,    21,    15,     8,    21,  1506,     8,  1766,  1509,
    1768,   124,  1512,   129,  1514,     8,  1516,    21,  1518,    15,
      21,     8,     7,  1634,  1890,     8,    15,  1665,    21,     8,
       8,     8,  1356,   149,    21,  1646,     5,     8,    21,  1650,
       9,    10,    21,    21,    21,    14,    15,    16,    17,    15,
      21,   603,   604,   605,  1366,   607,     8,    15,    27,    16,
      17,  2175,   614,   239,     8,    34,    15,    36,    15,    21,
       5,     3,    15,    16,    17,    10,   628,    21,     8,    14,
      15,    16,    17,     8,    27,   201,  2200,  1578,  1579,     8,
       8,    21,    27,    36,     5,    64,    21,  1409,  1410,    34,
    1412,    36,    21,    21,    15,    16,    17,     8,  1420,     8,
    1422,     8,  1424,   245,   246,  1427,    27,   124,   670,     8,
      21,    64,    21,    92,    21,    36,     5,  2407,     8,    64,
       9,    15,    21,   685,   246,     8,  1460,    16,    17,   993,
     994,    21,    21,   246,    23,  1619,    15,  1459,    21,    92,
    1552,  1553,  1395,    64,  2453,  2454,  1468,    92,    37,    21,
     129,   713,  1474,    26,   852,   853,  1476,   855,   720,  2349,
    2350,     8,  1484,     7,   726,   727,   860,   861,   240,  1503,
     149,    92,    15,    15,    15,  1495,   129,  1497,     8,  1501,
      15,    16,    17,    15,   129,  1519,  1520,  1521,     8,  1523,
    1524,    15,    27,  1527,    15,   175,   149,   149,     8,    15,
       8,    36,  1522,     8,   149,     8,    15,  1541,   129,     8,
     772,  1545,    26,  1533,    15,     7,    62,  1470,    15,     5,
      15,   166,   201,     9,    59,     8,  1546,    21,   149,    64,
      16,    17,     8,   287,    15,    63,  1558,    23,     8,     8,
      68,   213,  1752,  1563,     8,  1565,     8,  1757,   201,    15,
       9,    37,    64,   129,    21,     9,   201,    92,     8,     8,
    1770,  1902,    90,    91,     8,    67,  1588,    15,    15,  1905,
      15,    26,    37,    15,  2166,    15,    15,    15,    15,  1601,
     201,     8,  1792,   845,   846,  1619,  1620,     8,  1798,   129,
       7,    15,  1802,     7,    37,    21,   124,    26,  1620,    21,
      18,  1623,  1624,  1625,     7,    21,    15,     7,    15,  2227,
       8,  1633,     8,    15,   149,  1972,   144,     8,    26,     8,
      21,    15,     5,     5,     5,    15,   154,    10,     9,    21,
      15,    14,    15,    16,    17,    16,    17,    15,    15,    15,
      21,    21,    23,  1665,    27,  1679,  1680,  1681,    21,    15,
      15,    34,    15,    36,    15,    15,    37,    15,    15,    15,
      15,    15,    21,  1685,     8,    62,   201,     7,    15,     8,
       8,     8,   200,     8,    15,  2293,     7,   255,  2296,  2297,
    1702,    64,   944,   945,   946,   947,   948,   949,   950,   951,
     952,   953,   954,   955,   956,   957,   958,   959,   960,   961,
     962,   239,  2142,   166,    15,   233,    21,   969,    16,    92,
      15,    15,   974,    15,  1736,  1737,   244,   208,    21,   981,
       8,   983,     7,    15,   328,    15,   329,   315,  2346,     5,
       7,   246,  2053,     9,     5,   246,   998,    15,     8,  1761,
      16,    17,  2063,    15,     8,     7,   129,    23,   246,    25,
    2368,  2369,    28,    29,    30,    31,    32,   246,   246,  1969,
      21,    37,  1024,    21,    21,    21,   149,    21,  1030,     8,
    2217,  2218,  1982,    26,    15,    21,     8,  1987,   173,    15,
    1990,    15,   255,   166,  1046,     8,     7,     5,    21,    15,
    2000,     9,  2002,    15,  2004,  2413,  2006,   208,    16,    17,
      45,    46,    71,    48,    49,    23,    18,    25,    53,     8,
      28,    29,    30,    31,    32,    15,    61,    15,   201,    37,
      15,    66,    15,    15,    69,    21,    15,    21,    44,    15,
      21,    21,    77,    21,    79,     8,    81,    21,    21,    21,
      15,     5,  1864,  1865,  1866,  1867,    15,     7,     9,  2293,
      20,  2295,    26,    21,    21,  1877,  2197,     7,    21,   104,
      15,    15,    21,    21,    21,    18,     8,  1901,    15,  1131,
     115,    87,    21,  1907,    26,    26,    15,    93,    21,    15,
      26,     8,  1904,   254,    15,    15,    21,    21,  1150,     8,
     132,     7,    21,     7,   139,    21,    37,    21,     7,     7,
    1920,    21,   255,   119,     5,    15,    21,   152,     9,    15,
      15,  1173,  1174,  1175,    15,    16,    17,    15,    15,    15,
      15,  1941,    15,    15,    15,   170,    27,   143,     7,   145,
       8,   176,    21,     8,  1956,    36,     7,    21,  1960,   184,
    1962,   186,     8,   188,     7,   190,    15,    21,     7,     7,
    1972,    15,   197,    21,     8,    78,  1978,  1979,   327,     7,
       7,    21,   178,    64,    15,  1985,    15,   183,    21,   214,
     215,   187,    17,     5,  1994,   191,  1238,     9,   194,    17,
       8,    15,  2016,    15,    16,    17,     7,   232,     7,    15,
       7,    92,  2012,   220,   239,    27,   128,  1262,   896,   904,
     216,  1958,  1090,  1563,    36,  1836,  1186,    44,   253,  1567,
     576,   256,  2175,  1837,   230,  2035,   584,   587,   704,  1293,
     236,   933,  1597,  2180,  2075,   933,  1288,  1864,   129,  1856,
    2055,  1484,    64,  1155,  2124,  1703,  1956,  1468,  1300,  2144,
    1979,  1026,  2064,  1046,  1404,  1383,  1088,  1676,   149,  2459,
      87,  2073,  1678,  2075,  2076,  2077,  2078,  2079,  2080,    75,
      92,  1907,  1885,  2097,   705,   166,  1328,    45,    46,  1390,
      48,    49,   459,   244,  1427,    53,   786,  1035,  1340,   244,
    1616,  2291,   119,    61,  1619,  2105,   651,   998,    66,  1071,
    1102,    69,  1175,   832,   653,   655,   244,   129,   654,    77,
     201,    79,   656,    81,   244,  2226,   143,  1406,   145,  2104,
    1408,  2411,  2279,  1960,  1016,  2387,   430,   149,   801,  1014,
    2142,  2143,  2144,  1087,  1904,  1028,   104,  1389,  1390,  1465,
    2330,  1943,  1041,  2167,  2168,  2413,  2292,   115,  2435,  2455,
    2457,   178,  1404,  2234,  1728,   459,   183,  1482,  1963,  1411,
     187,   500,   466,   660,   191,  1444,  1909,   194,  2180,  1421,
     474,   139,   755,   378,   411,  1427,  2207,  1429,  1967,   201,
    1533,  1780,  1799,  1755,   152,  1803,  1793,  1760,  1991,   216,
    1442,  1992,  2258,  2012,  1225,  1247,  1252,  1257,    -1,    -1,
      -1,    -1,   170,   230,  1456,  2217,  2218,    -1,   176,   236,
      -1,    -1,    -1,    -1,  2226,  1467,   184,    -1,   186,    -1,
     188,    -1,   190,    -1,    -1,    -1,    -1,    -1,  1480,   197,
      -1,    -1,     5,  1485,     7,  1487,    -1,    10,    -1,    -1,
      -1,    14,    15,    16,    17,    -1,   214,   215,    -1,    -1,
      -1,    -1,    -1,    -1,    27,     5,    -1,    -1,    -1,    -1,
      -1,    34,    -1,    36,   232,    15,    16,    17,    -1,    -1,
    2294,  2283,    -1,    -1,    -1,    -1,    -1,    27,    -1,   583,
      -1,  2293,    -1,  2295,    -1,   253,    36,  2311,   256,    -1,
      -1,    64,    -1,     5,  2318,    -1,    -1,  2307,    10,    -1,
      -1,    -1,    14,    15,    16,    17,  2316,  2319,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    27,    -1,    -1,    -1,    92,
      -1,  1573,    34,    -1,    36,    -1,     5,    -1,    40,    -1,
      -1,    10,    -1,    -1,    -1,    14,    15,    16,    17,  2349,
    2350,    -1,    92,   647,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    64,    -1,    -1,    34,   129,    36,    -1,    -1,
      -1,    -1,  1614,  1615,  1616,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  2387,   149,  2389,    -1,   129,
      92,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1645,    -1,    -1,  1648,    -1,    -1,   149,
       5,    -1,    -1,    -1,  1656,    10,    -1,    -1,    -1,    14,
      15,    16,    17,    92,    -1,    -1,    -1,   129,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,  2437,   201,    34,
      -1,    36,  2442,    -1,    -1,   739,  1688,   149,  1690,    -1,
      -1,    -1,    -1,    -1,    -1,  2455,    -1,  2457,  2458,    -1,
     129,   201,    -1,   757,    -1,    -1,    -1,    -1,    -1,    64,
    2470,    -1,    -1,    -1,    -1,    -1,    -1,   771,    -1,    -1,
     149,    -1,    -1,     5,    -1,   779,   780,    -1,    10,    -1,
     784,     5,    14,    15,    16,    17,    10,    92,    -1,   201,
      14,    15,    16,    17,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    27,    36,    -1,    -1,    -1,   812,    -1,
      34,    -1,    36,    -1,    -1,    -1,   820,    -1,    -1,    -1,
      -1,    -1,   201,    -1,   129,    -1,    -1,    -1,     7,    -1,
      -1,    10,    64,    -1,    -1,    14,    15,    16,    17,    -1,
      64,    -1,    -1,    -1,   149,   849,    -1,   851,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    -1,
      92,    -1,    -1,    -1,    -1,    -1,   870,   871,    92,    -1,
      -1,    -1,    -1,    -1,    -1,   879,   880,    -1,  1830,  1831,
      -1,     7,    -1,    -1,    10,    64,  1838,    -1,    14,    15,
      16,    17,    -1,    -1,    -1,    -1,   201,   129,    -1,    78,
      -1,    27,    -1,    -1,    -1,   129,    -1,    -1,    34,    -1,
      36,    -1,    -1,    92,    -1,    -1,    -1,   149,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   149,    -1,  1879,    -1,    -1,
      -1,    -1,    -1,  1885,    -1,    -1,    -1,    -1,    64,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    10,    -1,
     129,    -1,    14,    15,    16,    17,    -1,    -1,  1910,   615,
      -1,    -1,    -1,   967,   968,    27,    92,  1919,    -1,   201,
     149,    -1,    34,    -1,    36,    -1,    -1,   201,    -1,    -1,
      -1,    -1,    -1,    -1,  1936,  1937,    -1,    -1,     7,   993,
     994,    10,    -1,    -1,    -1,    14,    15,    16,    17,    -1,
      -1,    -1,    64,   129,    -1,    -1,    -1,    -1,    27,    -1,
      -1,  1963,    -1,    -1,    -1,    34,  1020,    36,    -1,  1971,
      -1,    -1,   201,   149,    -1,    -1,    -1,    -1,  1980,    -1,
      92,    -1,    -1,     7,    -1,    -1,    10,    -1,    -1,  1043,
      14,    15,    16,    17,  1996,    64,  1998,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    -1,    36,    -1,    -1,    -1,    -1,   129,    -1,    -1,
      -1,    -1,    -1,    92,     5,   201,    -1,  1081,     9,    10,
      -1,    -1,    -1,    -1,    -1,    16,    17,   149,    -1,    -1,
      64,    22,    23,    24,    25,    26,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,    -1,    37,    38,    -1,   765,
     129,    -1,    -1,   769,    -1,    -1,    -1,     7,    92,    -1,
      10,    -1,    -1,    -1,    14,    15,    16,    17,    -1,    -1,
     149,    -1,    -1,    -1,    -1,  2087,    -1,    27,    -1,   201,
    2092,    -1,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,
      -1,  2103,  2104,    -1,    -1,   129,  2108,    -1,    -1,    -1,
     816,    -1,    -1,    -1,    -1,    -1,  2118,    -1,  1172,    -1,
      -1,  2123,   828,    -1,    64,   149,     7,    -1,    -1,    10,
      -1,    -1,   201,    14,    15,    16,    17,    -1,    -1,     5,
      -1,    -1,    -1,     9,    -1,    -1,    27,    -1,    -1,    -1,
      16,    17,    92,    34,    -1,    36,    22,    23,    -1,    25,
      -1,    -1,    28,    29,    30,    31,    32,   873,   874,   875,
     876,    37,   878,  2175,    -1,    -1,    -1,   201,    -1,    -1,
      -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,    10,   129,
      -1,    -1,    14,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,  2210,   149,
      -1,    92,    34,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  2225,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   938,    -1,   940,   941,   942,   943,  1292,     5,
      -1,     7,    64,     9,    10,    -1,    -1,    -1,   129,    -1,
      16,    17,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      -1,   201,    28,    29,    30,    31,    32,    33,   149,    35,
      92,    37,    38,    -1,    -1,    -1,  2278,  2279,    -1,    -1,
    2282,    -1,  2284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   999,    -1,    -1,    -1,    -1,  2300,  2301,
    2302,    -1,  1356,    -1,    -1,    -1,    -1,   129,    -1,    -1,
    2312,    -1,    -1,    -1,    -1,  1021,  2318,    -1,    -1,    -1,
     201,    -1,    -1,    -1,    -1,  1031,    -1,   149,  2330,    -1,
      -1,    -1,    10,    -1,    -1,    -1,    14,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,  1052,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,
    2362,  2363,  1068,    -1,    -1,    -1,  1072,  1073,  1422,    -1,
    1424,    -1,  2374,    -1,    -1,  2377,    -1,  2379,    -1,   201,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,  1443,
    1444,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1454,    -1,    -1,    -1,    -1,  2407,  1460,    -1,  1462,  2411,
      -1,  1465,    -1,  2415,    92,    -1,    -1,  2419,    -1,  2421,
      -1,    -1,    -1,  1477,  1130,    -1,    -1,  1481,  1482,    -1,
      -1,  2433,  2434,  2435,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1503,
      -1,   129,    -1,    -1,    -1,    -1,  1510,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1519,  1520,  1521,    -1,  1523,
    1524,   149,    -1,  1527,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1189,  1190,    -1,    -1,  1541,    -1,  1543,
      -1,  1545,    -1,    -1,  1548,  1549,  1550,    -1,  1552,  1553,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     5,    -1,    -1,    -1,     9,    10,    -1,
      -1,    -1,    -1,   201,    16,    17,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,  1588,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    38,    -1,    -1,    -1,
       5,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    16,    17,    -1,    -1,    -1,  1620,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1304,  1305,
    1306,  1307,  1308,  1309,  1310,  1311,  1312,  1313,  1314,  1315,
    1316,  1317,  1318,  1319,  1320,  1321,  1322,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1679,  1680,  1681,    -1,    -1,
      12,    -1,  1338,    -1,    -1,    -1,    18,    -1,    -1,  1345,
      -1,  1347,    -1,    -1,    -1,    -1,    28,    -1,    -1,    31,
      -1,  1357,    34,    -1,    -1,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    49,    -1,    -1,
      -1,    -1,    -1,    -1,  1728,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1388,    -1,    -1,    67,    -1,    -1,    -1,    71,
      72,    -1,    -1,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    83,    84,    -1,    86,    87,    88,    89,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   100,    -1,
      -1,    -1,    -1,   105,    -1,   107,    -1,    -1,    -1,   111,
      -1,    -1,    -1,   115,    -1,   117,    -1,    -1,    -1,    -1,
      -1,    -1,   124,    -1,    -1,    -1,   128,    -1,    -1,    -1,
     132,  1457,   134,    -1,    -1,    -1,    -1,    -1,    -1,   141,
     142,    -1,    -1,   145,   146,  1471,    -1,    -1,    -1,   151,
      -1,   153,    -1,    -1,    -1,   157,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1492,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1851,  1852,  1853,
    1854,  1855,   184,     5,   186,    -1,     8,     9,    10,    -1,
      -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    -1,    -1,    28,    29,    30,    31,
      32,    33,  1538,    35,    -1,    37,    38,     5,    -1,    -1,
       8,     9,    10,    -1,    -1,    -1,    -1,  1901,    16,    17,
      -1,    -1,    -1,  1907,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    31,    32,    33,  1572,    35,    -1,    37,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1940,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,    -1,  1605,
       8,     9,    10,    -1,    -1,    -1,  1612,  1961,    16,    17,
      -1,    -1,  1618,    -1,    22,    23,    24,    25,  1972,    -1,
      28,    29,    30,    31,    32,    33,    -1,    35,    -1,    37,
      38,    -1,    -1,    -1,    -1,  1641,    -1,    -1,  1992,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1653,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1661,    -1,    -1,    -1,    -1,
      -1,    -1,  2016,  2017,    -1,  1671,    -1,    -1,  1674,    -1,
      -1,    -1,    -1,     5,    -1,    -1,    -1,     9,    10,    -1,
      -1,    -1,    -1,  1689,    16,    17,    -1,    -1,    -1,    21,
      22,    23,    24,    25,  1700,    -1,    28,    29,    30,    31,
      32,    33,    -1,    35,    -1,    37,    38,    -1,  1714,    -1,
    2064,    -1,    -1,    -1,  1720,    -1,    -1,    -1,   400,    -1,
    1726,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       5,    -1,    -1,    -1,     9,    10,    -1,    -1,    -1,    -1,
    2094,    16,    17,  2097,    -1,  2099,    21,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,    38,     5,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    -1,
      -1,    22,    23,    24,    25,    -1,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,    -1,    37,    38,    -1,    -1,
       5,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    16,    17,  2167,  2168,    -1,    -1,    22,    23,    24,
      25,    -1,   504,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,    38,     5,    -1,  1842,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    -1,
      21,    22,    23,    24,    25,    -1,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,    -1,    37,    38,    -1,    -1,
      -1,     5,    -1,  2227,    -1,     9,    10,  2231,    -1,    -1,
    1886,    -1,    16,    17,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    32,    33,
    1906,    35,  1908,    37,    38,    -1,  1912,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1935,
      -1,    -1,  1938,    -1,    -1,    -1,    -1,    -1,    -1,  2293,
    2294,  2295,  2296,  2297,     5,    -1,    -1,    -1,     9,    10,
      -1,    -1,  1958,    -1,    -1,    16,    17,  2311,    -1,    -1,
      21,    22,    23,    24,    25,    -1,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,    -1,    37,    38,     5,    -1,
      -1,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,
      17,    -1,  2346,    -1,    21,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    31,    32,    33,    -1,    35,    -1,
      37,    38,    -1,    -1,  2368,  2369,     5,    -1,  2372,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,
      -1,    -1,    21,    22,    23,    24,    25,  2043,  2044,    28,
      29,    30,    31,    32,    33,  2051,    35,    -1,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  2413,
      -1,     5,    -1,    -1,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    16,    17,    -1,    -1,  2082,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    32,    33,
      -1,    35,    -1,    37,    38,  2101,    -1,     5,    -1,    -1,
    2106,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,
    2116,  2117,    -1,    21,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    31,    32,    33,    -1,    35,    -1,    37,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,    -1,
      -1,  2147,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,
      17,  2157,  2158,    -1,    21,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    31,    32,    33,    -1,    35,    -1,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  2190,    -1,  2192,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     5,  2201,     7,    -1,     9,    10,
    2206,    -1,    -1,  2209,    -1,    16,    17,    -1,    -1,    -1,
      -1,    22,    23,    24,    25,  2221,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,     5,    37,    38,    -1,     9,
      10,   913,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    -1,    -1,    28,    29,
      30,    31,    32,    33,    -1,    35,    -1,    37,    38,    -1,
       5,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    16,    17,    -1,    -1,    -1,    -1,    22,    23,    24,
      25,    -1,  2288,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  2323,    -1,  2325,
      -1,    -1,    -1,  2329,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1013,    -1,    -1,    -1,  2341,  2342,    -1,  2344,    -1,
       5,    -1,    -1,    -1,     9,    10,    -1,    -1,  2354,    -1,
    2356,    16,    17,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  2388,    -1,  2390,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  2402,  2403,    -1,    14,
      15,    16,    17,    -1,    19,    -1,    21,    -1,    23,    24,
      -1,    26,    -1,    -1,    -1,    30,    -1,    32,    33,    -1,
    2426,    36,    37,    38,  2430,    40,    -1,    42,    43,    -1,
      -1,    46,    -1,    -1,  2440,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    -1,    68,    -1,    70,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    81,    82,    -1,    -1,
      85,    -1,    -1,    -1,    -1,    -1,    -1,    92,    -1,    94,
      -1,    96,    97,    -1,    -1,    -1,   101,   102,   103,    -1,
      -1,   106,    -1,    -1,   109,   110,    -1,   112,   113,   114,
      -1,    -1,    -1,    -1,   119,   120,    -1,    -1,   123,    -1,
     125,   126,   127,    -1,   129,   130,    -1,    -1,   133,    -1,
     135,   136,   137,   138,   139,   140,    -1,    -1,    -1,    -1,
      -1,    -1,   147,   148,   149,    -1,    -1,    -1,    -1,   154,
     155,   156,    -1,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,    -1,
     185,    -1,   187,    -1,   189,    -1,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,    -1,  1269,     6,     5,
      -1,     7,    -1,     9,    10,    13,    -1,    -1,    -1,    -1,
      16,    17,    -1,    -1,    -1,    -1,    22,    23,    24,    25,
      -1,    -1,    28,    29,    30,    31,    32,    33,    -1,    35,
      -1,    37,    38,    -1,    -1,    43,    44,    45,    46,    -1,
      48,    49,    50,    51,    52,    53,    -1,   252,    56,    57,
     255,    -1,   257,    61,    62,    63,    -1,    65,    66,    67,
      68,    69,    70,    -1,    72,    73,    74,    75,    76,    77,
      -1,    79,    80,    81,    82,    83,    84,    -1,    -1,    87,
      88,    89,    90,    91,    -1,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -1,   117,
      -1,   119,   120,    -1,   122,   123,   124,    -1,    -1,   127,
      -1,    -1,   130,   131,    -1,   133,   134,   135,   136,    -1,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    -1,
      -1,    -1,    -1,    -1,   152,   153,   154,   155,    -1,    -1,
      -1,    -1,   160,    -1,    -1,   163,   164,    -1,    -1,   167,
     168,    -1,   170,    -1,    -1,    -1,   174,    -1,   176,    -1,
     178,    -1,    -1,    -1,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,    -1,   193,   194,   195,    -1,   197,
      -1,   199,   200,    -1,   202,    -1,   204,   205,   206,   207,
      -1,    -1,   210,   211,   212,    -1,   214,   215,   216,    -1,
     218,   219,   220,    -1,   222,    -1,   224,   225,   226,   227,
     228,    -1,   230,    -1,   232,   233,    -1,    -1,   236,   237,
     238,    -1,    -1,   241,   242,    -1,   244,   245,    -1,   247,
     248,    -1,    -1,    -1,   252,   253,    -1,    -1,   256,    -1,
      -1,   259,    -1,    -1,    -1,   263,   264,    -1,    -1,   267,
     268,   269,    -1,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,    -1,   283,    -1,    -1,   286,    -1,
      -1,    -1,   290,   291,   292,   293,   294,    -1,   296,   297,
      -1,    -1,   300,   301,   302,   303,    -1,    -1,    -1,    -1,
     308,   309,   310,   311,   312,   313,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   322,   323,   324,   325,   326,    -1,
      -1,    -1,   330,   331,   332,   333,   334,   335,    -1,   337,
     338,     5,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    16,    17,    -1,    -1,    -1,    -1,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    32,    33,
      -1,    35,    -1,    37,    38,    -1,     5,    -1,    -1,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,
      -1,    -1,    21,    22,    23,    24,    25,   592,   593,    28,
      29,    30,    31,    32,    33,    -1,    35,     5,    37,    38,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    31,    32,    33,    -1,    35,     5,    37,
      38,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,
      17,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    31,    32,    33,    -1,    35,     5,
      37,    38,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      16,    17,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      -1,    -1,    28,    29,    30,    31,    32,    33,    -1,    35,
       5,    37,    38,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    16,    17,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    31,    32,    33,    -1,
      35,     5,    37,    38,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    16,    17,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    32,    33,
      -1,    35,     5,    37,    38,     8,     9,    10,    -1,    -1,
      -1,    -1,    -1,    16,    17,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    -1,    -1,    28,    29,    30,    31,    32,
      33,    -1,    35,     5,    37,    38,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    -1,    21,
      22,    23,    24,    25,    -1,    -1,    28,    29,    30,    31,
      32,    33,    -1,    35,     5,    37,    38,     8,     9,    10,
      -1,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,    -1,
      -1,    22,    23,    24,    25,    -1,    -1,    28,    29,    30,
      31,    32,    33,    -1,    35,     5,    37,    38,     8,     9,
      10,    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,    -1,
      -1,    -1,    22,    23,    24,    25,    -1,    -1,    28,    29,
      30,    31,    32,    33,    -1,    35,     5,    37,    38,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,
      -1,    -1,    21,    22,    23,    24,    25,    -1,    -1,    28,
      29,    30,    31,    32,    33,    -1,    35,     5,    37,    38,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    31,    32,    33,    -1,    35,     5,    37,
      38,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,
      17,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    31,    32,    33,    -1,    35,     5,
      37,    38,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      16,    17,    -1,    -1,    -1,    -1,    22,    23,    24,    25,
      -1,    -1,    28,    29,    30,    31,    32,    33,    -1,    35,
       5,    37,    38,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    16,    17,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    31,    32,    33,    -1,
      35,     5,    37,    38,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    16,    17,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    31,    32,    33,
      -1,    35,     5,    37,    38,     8,     9,    10,    -1,    -1,
      -1,    -1,    -1,    16,    17,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    -1,    -1,    28,    29,    30,    31,    32,
      33,    -1,    35,    -1,    37,    38,     5,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    -1,    -1,    28,
      29,    30,    31,    32,    33,    -1,    35,     5,    37,    38,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,    17,
      -1,    -1,    -1,    -1,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    31,    32,    33,    -1,    35,     5,    37,
      38,    -1,     9,    -1,    -1,    -1,    -1,    -1,    -1,    16,
      17,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    31,    32,    33,    -1,    35,     5,
      37,    38,    -1,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    17,    -1,    -1,    -1,    -1,    22,    23,    -1,    25,
      -1,    -1,    28,    29,    30,    31,    32,    -1,    -1,    35,
      -1,    37,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   344,     0,     1,   150,   257,   345,   346,   116,     6,
      13,    43,    44,    45,    46,    48,    49,    50,    51,    52,
      53,    56,    57,    61,    62,    63,    65,    66,    67,    68,
      69,    70,    72,    73,    74,    75,    76,    77,    79,    80,
      81,    82,    83,    84,    87,    88,    89,    90,    91,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   117,   119,   120,   122,   123,   124,   127,   130,   131,
     133,   134,   135,   136,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   152,   153,   154,   155,   160,   163,   164,
     167,   168,   170,   174,   176,   178,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   193,   194,   195,   197,
     199,   200,   202,   204,   205,   206,   207,   210,   211,   212,
     214,   215,   216,   218,   219,   220,   222,   224,   225,   226,
     227,   228,   230,   232,   233,   236,   237,   238,   241,   242,
     244,   245,   247,   248,   252,   253,   256,   259,   263,   264,
     267,   268,   269,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   283,   286,   290,   291,   292,   293,
     294,   296,   297,   300,   301,   302,   303,   308,   309,   310,
     311,   312,   313,   322,   323,   324,   325,   326,   330,   331,
     332,   333,   334,   335,   337,   338,   347,   349,   352,   364,
     365,   369,   370,   371,   377,   378,   379,   380,   382,   383,
     385,   387,   388,   389,   390,   397,   398,   399,   400,   401,
     402,   406,   407,   408,   412,   413,   451,   453,   466,   509,
     510,   512,   513,   519,   520,   521,   522,   529,   530,   531,
     532,   534,   537,   541,   542,   543,   544,   545,   546,   552,
     553,   554,   565,   566,   567,   569,   572,   575,   580,   581,
     583,   585,   587,   590,   591,   615,   616,   627,   628,   629,
     630,   635,   638,   641,   644,   645,   693,   694,   695,   696,
     697,   698,   699,   700,   706,   708,   710,   712,   714,   715,
     716,   717,   718,   721,   723,   724,   725,   728,   729,   733,
     734,   736,   737,   738,   739,   740,   741,   742,   745,   750,
     755,   757,   758,   759,   760,   762,   764,   765,   766,   767,
     768,   785,   788,   789,   790,   791,   797,   800,   805,   806,
     807,   810,   811,   812,   813,   814,   815,   816,   817,   818,
     819,   820,   821,   822,   823,   824,   829,   830,   831,   832,
     833,   843,   844,   845,   846,   847,   848,   849,   850,    15,
     476,   476,   538,   538,   538,   538,   538,   476,   538,   538,
     348,   538,   538,   538,   476,   538,   476,   538,   538,   476,
     538,   538,   538,   475,   538,   476,   538,   538,     7,    15,
     477,    15,   476,   598,   538,   476,   361,   538,   538,   538,
     538,   538,   538,   538,   538,   538,   538,   129,   354,   518,
     518,   538,   538,   538,   476,   538,   354,   538,   476,   476,
     538,   538,   475,   348,   476,   476,    64,   360,   538,   538,
     476,   476,   538,   476,   476,   476,   476,   476,   538,   415,
     538,   538,   538,   354,   452,   348,   476,   538,   538,   538,
     476,   538,   476,   538,   538,   476,   538,   538,   538,   476,
     348,   476,   361,   538,   538,   361,   538,   476,   538,   538,
     538,   476,   538,   538,   476,   538,   476,   538,   538,   538,
     538,   538,   538,    15,   476,   576,   476,   348,   476,   476,
     538,   538,   538,    15,     8,   476,   476,   538,   538,   538,
     476,   538,   538,   538,   538,   538,   538,   538,   538,   538,
     538,   538,   538,   538,   538,   538,   538,   538,   538,   538,
     538,   538,   538,   538,   538,   538,   538,   476,   538,   476,
     538,   538,   538,   538,   538,   538,   538,   538,   538,   538,
     538,   538,    15,    15,    15,   354,   862,   257,   564,   124,
     233,   385,    15,   357,   564,     8,     8,     8,     8,     7,
       8,   124,   349,   372,     8,   354,   386,     8,     8,     8,
       8,     8,   533,     8,   533,     8,     8,     8,     8,   533,
     564,     7,   218,   252,   510,   512,   521,   522,   239,   530,
     530,    10,    14,    15,    16,    17,    27,    34,    36,    64,
      92,   149,   201,   354,   366,   482,   483,   485,   486,   487,
     488,   494,   495,   496,   497,   498,   501,    15,   538,     5,
       9,    15,    16,    17,   129,   484,   486,   494,   548,   562,
     563,   538,    15,   548,   538,     5,   547,   548,   563,   548,
       8,     8,     8,     8,     8,     8,     8,     8,     7,     8,
       8,     5,     7,   354,   625,   626,   354,   618,   477,    15,
      15,   149,   465,   354,   354,   726,   727,     8,   354,   642,
     643,   727,   354,   356,   354,    15,   514,   560,    23,    37,
     354,   404,   405,    15,   354,   588,   354,   657,   657,   354,
     639,   640,   354,   517,   414,    15,   354,   568,   149,   732,
     517,     7,   460,   461,   476,   599,   600,   354,   594,   600,
      15,   539,   354,   570,   571,   517,    15,    15,   517,   732,
     518,   517,   517,   517,   517,   354,   517,   357,   517,    15,
     409,   477,   485,   486,    15,   351,   354,   354,   636,   637,
     467,   468,   469,   470,     8,   658,   722,    15,   354,   582,
     354,   573,   574,   561,    15,    15,   354,   477,    15,   482,
     735,    15,    15,   354,   709,   711,     8,   354,    37,   403,
      15,   486,   487,   477,    15,    15,   539,   465,   477,   486,
     354,   701,     5,    15,   562,   563,   477,   354,   355,   477,
     561,    15,   485,   619,   620,   594,   598,   354,   586,   354,
     681,   681,    15,   354,   584,   701,   482,   493,   477,   361,
      15,   354,   687,   687,   687,   687,   687,     7,   482,   577,
     578,   354,   579,   477,   350,   354,   477,   354,   707,   709,
     354,   476,   477,   354,   454,    15,    15,   561,   354,    15,
     600,    15,   600,   600,   600,   600,   771,   827,   600,   600,
     600,   600,   600,   600,   771,   354,   361,   834,   835,   836,
      15,    15,    15,   482,   482,   482,   482,   481,   482,    15,
      15,   361,   851,   852,   853,   854,   361,   855,   856,   857,
     858,   859,   361,   860,   861,   348,   348,   124,     5,    21,
     354,   358,   359,   353,   361,   354,   354,   354,   405,     7,
     361,   348,   124,   354,   354,     5,    15,   392,   393,   354,
     405,   405,   405,   405,   404,   485,   403,   354,   354,   409,
     416,   417,   419,   420,   538,   538,   239,   395,   482,   483,
     482,   482,   482,   482,     5,     9,    16,    17,    22,    23,
      24,    25,    26,    28,    29,    30,    31,    32,    33,    35,
      37,    38,   366,    15,   246,     3,    15,   246,   246,    15,
     491,   492,    21,   535,   560,   493,     5,     9,   166,   549,
     550,   551,   562,    26,   562,     5,     9,    23,    37,   484,
     561,   562,   561,     8,    15,   486,   555,   556,    15,   482,
     483,   498,   557,   558,   559,   557,   568,   354,   582,   584,
     586,   588,   354,     7,   361,   707,     8,    21,   620,   405,
     507,   482,   240,   533,    15,   361,    15,   459,     8,   560,
       7,   482,   515,   516,   517,    15,   354,   459,   405,   464,
     465,     8,   416,   507,   459,    15,     8,    21,     5,     7,
     462,   463,   482,   354,     8,    21,     5,    58,    86,   126,
     137,   165,   258,   601,   597,   598,   175,   589,   482,   149,
     528,     8,   482,   482,   353,   354,   410,   411,   485,   490,
     354,    26,   354,   523,   524,   526,   357,     8,     8,    15,
     231,   385,   471,   361,     8,   722,   354,   485,   691,   701,
     719,   720,     8,   548,    26,     5,     9,    16,    17,    22,
      23,    24,    25,    28,    29,    30,    31,    32,    33,    34,
      35,    37,    38,   366,   367,   368,   354,   361,   375,   485,
     482,    15,   361,   354,   354,   485,   485,   508,     8,   658,
     713,   354,   485,   646,   647,   354,   449,   450,   528,   405,
      18,   561,   562,   561,   381,   384,   625,   620,     7,   598,
     600,   691,   701,   702,   703,   404,   405,   443,   444,    62,
     485,   746,    15,    15,     7,     8,    21,   576,   405,   357,
     405,   459,     8,   656,   678,    21,   361,   354,     8,   482,
     482,   459,   485,   533,   792,   485,   287,   804,   804,   533,
     801,   804,    15,   533,   769,   533,   808,   769,   769,   533,
     786,   533,   798,   459,   147,   148,   180,   314,   315,   318,
     319,   362,   837,   838,   839,     8,    21,   486,   662,   840,
      21,   840,   361,   743,   744,     8,     8,     8,     8,   485,
     488,   489,   761,   485,   763,   191,   208,     8,    21,    41,
     209,   228,     8,    21,   336,   339,   340,     8,    21,   354,
     350,   348,     8,    21,   213,   362,   459,    44,    87,    93,
     119,   143,   145,   178,   183,   187,   191,   194,   216,   230,
     236,   373,   374,   376,   354,   348,   476,   539,   560,   386,
     459,   533,   533,     8,    37,    15,   354,   422,   427,   361,
      15,   502,    21,     8,   482,   482,   482,   482,   482,   482,
     482,   482,   482,   482,   482,   482,   482,   482,   482,   482,
     482,   482,   482,   560,    64,   129,   478,   480,   560,   485,
     496,   499,    64,   499,   493,     8,    21,     5,   482,   536,
     551,     8,    21,     5,     9,   482,    21,   482,   562,   562,
     562,   562,   562,    21,   555,   555,     8,   482,   483,   558,
     559,     8,     8,     8,   459,   459,   476,    43,    67,    82,
      87,    88,    94,   228,   259,   303,   629,   626,   361,   489,
     505,    21,   354,    15,   481,    67,   460,   643,   482,     7,
       8,    21,   535,    37,     8,    21,   640,   485,   488,   504,
     506,   560,   730,   462,     7,   459,   600,    15,    15,    15,
      15,    15,    15,   589,   600,   354,    21,   540,   571,    21,
      21,    15,     8,    21,     8,   492,   486,     8,   525,    26,
     353,   637,   468,   129,   472,   473,   474,   390,   169,   208,
     282,   361,    15,     7,     8,    21,   574,   557,    21,    21,
     147,   148,   180,    21,    18,    21,     7,   482,   500,   175,
     321,    37,     8,    21,   361,     8,    21,    26,     8,    21,
     540,   482,    21,   445,   446,   445,    21,     7,   600,   589,
      15,     7,     8,    21,     8,    15,    15,    26,   688,   689,
     691,   481,   482,   578,   361,     8,   678,     8,   656,   386,
     376,   363,    21,    21,    21,   600,   533,    21,   600,   533,
     828,   600,   533,   600,   533,   600,   533,   600,   533,    15,
      15,    15,    15,    15,    15,   361,   836,     8,    21,    21,
     182,   315,   318,     8,    21,   361,   361,   361,   482,    15,
      15,     8,    21,     8,    21,    15,    15,   852,    15,    15,
      15,   856,    15,    15,   861,   357,   354,   359,    15,   391,
     392,   459,   476,    15,     7,     8,   354,   459,    15,   496,
       5,   394,   482,   551,   405,   485,   419,    15,    16,    17,
      27,    36,    59,    64,    92,   149,   201,   418,   420,   430,
     431,   432,   433,   434,   435,   436,   437,   422,   427,   428,
     429,    15,   423,   424,    62,   482,   557,   483,   478,    21,
       8,   479,   482,   500,   551,     7,   560,   465,   482,   560,
       8,   556,    21,     8,     8,     8,   483,   559,   483,   559,
     483,   559,   354,   255,     8,    21,   465,   464,    21,     7,
      21,   482,   515,    21,   465,   533,     8,    21,   551,   731,
       8,    21,   463,   482,   601,   560,    15,   603,   354,   602,
     602,   482,   602,   459,   600,   239,   517,   481,   411,   411,
     354,   482,   524,    21,   482,   500,     8,    21,    16,    15,
      15,    15,   481,   719,   720,   477,   485,   751,     7,   482,
       7,    21,    21,   354,   596,   486,   485,   647,   600,   648,
     482,   450,   533,     8,    47,   177,   354,   448,   361,   617,
     619,   589,     7,     7,   482,   704,   705,   702,   703,   444,
     482,     5,   603,   747,   748,   754,   482,   613,     8,    21,
      15,    21,    71,   208,   361,   361,   477,   172,   354,   457,
     458,   486,   191,   208,   282,   285,   290,   298,   772,   773,
     774,   781,   793,   794,   795,   600,   266,   802,   803,   804,
     600,    37,   485,   825,   826,    84,   265,   289,   299,   304,
     770,   772,   773,   774,   775,   776,   777,   779,   780,   781,
     600,   772,   773,   774,   775,   776,   777,   779,   780,   781,
     794,   795,   809,   600,   772,   773,   774,   781,   787,   600,
     772,   773,   799,   600,   840,   840,   840,   361,   841,   842,
     840,   840,   486,   744,   328,   315,   329,   560,   478,   489,
     485,   662,   361,   682,   688,   702,   688,   646,   646,   354,
      18,    26,   396,    15,   375,     7,   361,   391,   540,   540,
     395,     5,   482,   433,   434,   435,   438,   434,   436,   434,
     436,   246,   246,   246,   246,   246,     8,    37,   354,   421,
     485,     5,   423,   424,     8,    15,    16,    17,   149,   354,
     421,   425,   426,   439,   440,   441,   442,    15,   424,    15,
      21,   503,    21,    21,   492,   560,   482,   493,   536,   550,
     562,   526,   527,   483,   527,   527,   527,   459,   354,   621,
     624,   560,     8,    21,     7,   395,   482,   560,   482,   560,
     551,   614,   482,   604,   605,    21,    21,    21,    21,     8,
       8,   254,   511,   517,    21,   473,   474,   662,   662,   662,
      21,    21,   354,    15,    21,   482,     7,     7,   482,   459,
     173,     8,   652,   653,   654,   655,   656,   658,   659,   660,
     663,   664,   665,   678,   686,   526,   446,    15,    15,   447,
     255,     8,     7,     8,    21,    21,    21,     8,    21,    21,
     689,   690,    15,    15,   354,   354,   455,   456,    18,     8,
      26,   771,    15,   771,   771,    15,   600,   793,   771,   600,
     802,   354,     8,    21,    15,   771,    15,   771,    15,   600,
     770,   600,   809,   600,   787,   600,   799,    21,    21,    21,
     316,   317,     8,    21,    21,    21,    15,    15,   478,    21,
      21,    22,    24,    33,    35,   158,   159,   161,   162,   192,
     234,   247,   683,   684,   685,     8,    21,    21,    21,    21,
      21,    21,    21,   482,   482,   631,   632,    21,   374,   396,
       5,   482,   386,     8,    21,     8,   499,   499,   499,   499,
     499,   430,     5,    15,   420,   431,   424,   354,   421,   429,
     439,   440,   440,     8,    21,     7,    16,    17,     5,    37,
       9,   439,   482,    20,   492,   479,    21,    26,    21,    21,
      21,    21,    15,   489,   551,   465,   642,   477,   504,   551,
     731,   482,    21,     7,     8,    21,   482,   361,    15,    21,
      21,    21,     7,   752,   753,   754,   482,   482,     7,   485,
     649,   361,   654,    26,   448,    26,   367,   621,   619,   354,
     592,   593,   594,   595,   705,   748,   600,    78,   577,   354,
     657,   702,   679,    18,     8,   354,   458,   482,   600,   782,
     361,   600,   600,   827,   485,   825,   361,   482,   482,   600,
     600,   600,   600,   842,   662,   485,    21,    15,    15,   361,
       8,    21,     7,    21,    21,   560,   438,   431,   560,   421,
      26,    21,   439,   426,   440,   440,   441,   441,   441,    21,
     482,     5,   482,   500,   622,   623,   485,     8,   662,   485,
       8,   482,   605,   361,    21,   254,   482,     8,    21,   482,
      15,    41,   135,   191,   209,   221,   223,   224,   226,   229,
     320,   482,   447,    21,    21,    15,     8,   132,   749,    21,
      21,     7,    21,   681,   683,   354,   456,     5,    16,    17,
      22,    24,    33,    35,    37,   159,   162,   247,   305,   306,
     307,   784,    21,    94,   230,   284,   295,   796,    37,   191,
     288,   299,   778,    21,    21,    21,    21,   486,   662,   683,
       7,     7,   394,    21,   478,   425,   439,    21,     8,     8,
      21,   465,   551,   255,    15,    21,   753,     5,   482,   650,
     651,    15,   666,    15,    15,    15,    15,    15,   687,   687,
      15,    15,     8,   481,   593,   691,   692,    15,   702,   680,
     680,     7,     8,    21,   828,    21,     8,    21,     8,     7,
     395,    21,    21,   482,   623,   482,   354,   606,   607,   482,
       8,    21,   600,   666,   701,   719,   662,   701,   702,   691,
     688,   482,   482,   661,   482,    21,     8,   361,    21,     7,
       8,    21,   662,   783,   482,   361,   482,   486,   354,   633,
     634,    21,     8,    15,    21,   651,   148,   180,   667,     7,
      21,    21,     7,    21,    15,    21,    21,     8,    21,     8,
     691,    78,   682,   682,    21,   327,    21,     8,   482,    40,
     482,   608,   609,   754,     7,     7,   668,   669,   691,   719,
     702,   577,   482,   482,    21,    21,    21,    15,   634,   354,
     610,     8,    21,     8,    21,    15,    21,    21,    21,     8,
     481,    17,   611,   612,   609,   669,   482,   670,   671,    21,
     482,    21,   613,    17,     7,     8,    21,     8,   756,   613,
     482,   671,    15,   361,   361,   672,   673,   674,   675,   676,
     182,   318,   128,   157,   217,     8,    21,     7,     7,    15,
     677,   677,   677,   673,   361,   675,   676,   361,   676,   480,
       7,    21,   676
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 770 "gram1.y"
    { (yyval.bf_node) = BFNULL; ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 772 "gram1.y"
    { (yyval.bf_node) = set_stat_list((yyvsp[(1) - (3)].bf_node),(yyvsp[(2) - (3)].bf_node)); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 776 "gram1.y"
    { lastwasbranch = NO;  (yyval.bf_node) = BFNULL; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 778 "gram1.y"
    {
	       if ((yyvsp[(2) - (3)].bf_node) != BFNULL) 
               {	    
	          (yyvsp[(2) - (3)].bf_node)->label = (yyvsp[(1) - (3)].label);
	          (yyval.bf_node) = (yyvsp[(2) - (3)].bf_node);
	 	  if (is_openmp_stmt) {            /*OMP*/
			is_openmp_stmt = 0;
			if((yyvsp[(2) - (3)].bf_node)) {                        /*OMP*/
				if ((yyvsp[(2) - (3)].bf_node)->decl_specs != -BIT_OPENMP) (yyvsp[(2) - (3)].bf_node)->decl_specs = BIT_OPENMP; /*OMP*/
			}                               /*OMP*/
		  }                                       /*OMP*/
               }
	    ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 792 "gram1.y"
    { PTR_BFND p;

	     if(lastwasbranch && ! thislabel)
               /*if (warn_all)
		 warn("statement cannot be reached", 36);*/
	     lastwasbranch = thiswasbranch;
	     thiswasbranch = NO;
	     if((yyvsp[(2) - (3)].bf_node)) (yyvsp[(2) - (3)].bf_node)->label = (yyvsp[(1) - (3)].label);
	     if((yyvsp[(1) - (3)].label) && (yyvsp[(2) - (3)].bf_node)) (yyvsp[(1) - (3)].label)->statbody = (yyvsp[(2) - (3)].bf_node); /*8.11.06 podd*/
	     if((yyvsp[(1) - (3)].label)) {
		/*$1->statbody = $2;*/ /*8.11.06 podd*/
		if((yyvsp[(1) - (3)].label)->labtype == LABFORMAT)
		  err("label already that of a format",39);
		else
		  (yyvsp[(1) - (3)].label)->labtype = LABEXEC;
	     }
	     if (is_openmp_stmt) {            /*OMP*/
			is_openmp_stmt = 0;
			if((yyvsp[(2) - (3)].bf_node)) {                        /*OMP*/
				if ((yyvsp[(2) - (3)].bf_node)->decl_specs != -BIT_OPENMP) (yyvsp[(2) - (3)].bf_node)->decl_specs = BIT_OPENMP; /*OMP*/
			}                               /*OMP*/
	     }                                       /*OMP*/
             for (p = pred_bfnd; (yyvsp[(1) - (3)].label) && 
		  ((p->variant == FOR_NODE)||(p->variant == WHILE_NODE)) &&
                  (p->entry.for_node.doend) &&
		  (p->entry.for_node.doend->stateno == (yyvsp[(1) - (3)].label)->stateno);
		  p = p->control_parent)
                ++end_group;
	     (yyval.bf_node) = (yyvsp[(2) - (3)].bf_node);
     ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 823 "gram1.y"
    { /* PTR_LLND p; */
			doinclude( (yyvsp[(3) - (3)].charp) );
/*			p = make_llnd(fi, STRING_VAL, LLNULL, LLNULL, SMNULL);
			p->entry.string_val = $3;
			p->type = global_string;
			$$ = get_bfnd(fi, INCLUDE_STAT, SMNULL, p, LLNULL); */
			(yyval.bf_node) = BFNULL;
		;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 832 "gram1.y"
    {
	      err("Unclassifiable statement", 10);
	      flline();
	      (yyval.bf_node) = BFNULL;
	    ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 838 "gram1.y"
    { PTR_CMNT p;
              PTR_BFND bif; 
	    
              if (last_bfnd && last_bfnd->control_parent &&((last_bfnd->control_parent->variant == LOGIF_NODE)
	         ||(last_bfnd->control_parent->variant == FORALL_STAT)))
  	         bif = last_bfnd->control_parent;
              else
                 bif = last_bfnd;
              p=bif->entry.Template.cmnt_ptr;
              if(p)
                 p->string = StringConcatenation(p->string,commentbuf);
              else
              {
                 p = make_comment(fi,commentbuf, FULL);
                 bif->entry.Template.cmnt_ptr = p;
              }
 	      (yyval.bf_node) = BFNULL;         
            ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 858 "gram1.y"
    { 
	      flline();	 needkwd = NO;	inioctl = NO;
/*!!!*/
              opt_kwd_ = NO; intonly = NO; opt_kwd_hedr = NO; opt_kwd_r = NO; as_op_kwd_= NO; optcorner = NO;
	      yyerrok; yyclearin;  (yyval.bf_node) = BFNULL;
	    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 867 "gram1.y"
    {
	    if(yystno)
	      {
	      (yyval.label) = thislabel =	make_label_node(fi,yystno);
	      thislabel->scope = cur_scope();
	      if (thislabel->labdefined && (thislabel->scope == cur_scope()))
		 errstr("Label %s already defined",convic(thislabel->stateno),40);
	      else
		 thislabel->labdefined = YES;
	      }
	    else
	      (yyval.label) = thislabel = LBNULL;
	    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 883 "gram1.y"
    { PTR_BFND p;

	        if (pred_bfnd != global_bfnd)
		    err("Misplaced PROGRAM statement", 33);
		p = get_bfnd(fi,PROG_HEDR, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL);
		(yyvsp[(3) - (3)].symbol)->entry.prog_decl.prog_hedr=p;
 		set_blobs(p, global_bfnd, NEW_GROUP1);
	        add_scope_level(p, NO);
	        position = IN_PROC;
	    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 895 "gram1.y"
    {  PTR_BFND q = BFNULL;

	      (yyvsp[(3) - (3)].symbol)->variant = PROCEDURE_NAME;
	      (yyvsp[(3) - (3)].symbol)->decl = YES;   /* variable declaration has been seen. */
	      q = get_bfnd(fi,BLOCK_DATA, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL);
	      set_blobs(q, global_bfnd, NEW_GROUP1);
              add_scope_level(q, NO);
	    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 905 "gram1.y"
    { 
              install_param_list((yyvsp[(3) - (4)].symbol), (yyvsp[(4) - (4)].symbol), LLNULL, PROCEDURE_NAME); 
	      /* if there is only a control end the control parent is not set */
              
	     ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 912 "gram1.y"
    { install_param_list((yyvsp[(4) - (5)].symbol), (yyvsp[(5) - (5)].symbol), LLNULL, PROCEDURE_NAME); 
              if((yyvsp[(1) - (5)].ll_node)->variant == RECURSIVE_OP) 
                   (yyvsp[(4) - (5)].symbol)->attr = (yyvsp[(4) - (5)].symbol)->attr | RECURSIVE_BIT;
              pred_bfnd->entry.Template.ll_ptr3 = (yyvsp[(1) - (5)].ll_node);
            ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 918 "gram1.y"
    {
              install_param_list((yyvsp[(3) - (5)].symbol), (yyvsp[(4) - (5)].symbol), (yyvsp[(5) - (5)].ll_node), FUNCTION_NAME);  
  	      pred_bfnd->entry.Template.ll_ptr1 = (yyvsp[(5) - (5)].ll_node);
            ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 923 "gram1.y"
    {
              install_param_list((yyvsp[(1) - (3)].symbol), (yyvsp[(2) - (3)].symbol), (yyvsp[(3) - (3)].ll_node), FUNCTION_NAME); 
	      pred_bfnd->entry.Template.ll_ptr1 = (yyvsp[(3) - (3)].ll_node);
	    ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 928 "gram1.y"
    {PTR_BFND p, bif;
	     PTR_SYMB q = SMNULL;
             PTR_LLND l = LLNULL;

	     if(parstate==OUTSIDE || procclass==CLMAIN || procclass==CLBLOCK)
	        err("Misplaced ENTRY statement", 35);

	     bif = cur_scope();
	     if (bif->variant == FUNC_HEDR) {
	        q = make_function((yyvsp[(2) - (4)].hash_entry), bif->entry.Template.symbol->type, YES);
	        l = construct_entry_list(q, (yyvsp[(3) - (4)].symbol), FUNCTION_NAME); 
             }
             else if ((bif->variant == PROC_HEDR) || 
                      (bif->variant == PROS_HEDR) || /* added for FORTRAN M */
                      (bif->variant == PROG_HEDR)) {
	             q = make_procedure((yyvsp[(2) - (4)].hash_entry), YES);
  	             l = construct_entry_list(q, (yyvsp[(3) - (4)].symbol), PROCEDURE_NAME); 
             }
	     p = get_bfnd(fi,ENTRY_STAT, q, l, (yyvsp[(4) - (4)].ll_node), LLNULL);
	     set_blobs(p, pred_bfnd, SAME_GROUP);
             q->decl = YES;   /*4.02.03*/
             q->entry.proc_decl.proc_hedr = p; /*5.02.03*/
	    ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 952 "gram1.y"
    { PTR_SYMB s;
	      PTR_BFND p;
/*
	      s = make_global_entity($3, MODULE_NAME, global_default, NO);
	      s->decl = YES;  
	      p = get_bfnd(fi, MODULE_STMT, s, LLNULL, LLNULL, LLNULL);
	      s->entry.Template.func_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
*/
	      /*position = IN_MODULE;*/


               s = make_module((yyvsp[(3) - (3)].hash_entry));
	       s->decl = YES;   /* variable declaration has been seen. */
	        if (pred_bfnd != global_bfnd)
		    err("Misplaced MODULE statement", 33);
              p = get_bfnd(fi, MODULE_STMT, s, LLNULL, LLNULL, LLNULL);
	      s->entry.Template.func_hedr = p; /* !!!????*/
	      set_blobs(p, global_bfnd, NEW_GROUP1);
	      add_scope_level(p, NO);	
	      position =  IN_MODULE;    /*IN_PROC*/
              privateall = 0;
            ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 978 "gram1.y"
    { newprog(); 
	      if (position == IN_OUTSIDE)
	           position = IN_PROC;
              else if (position != IN_INTERNAL_PROC){ 
                if(!is_interface_stat(pred_bfnd))
	           position--;
              }
              else {
                if(!is_interface_stat(pred_bfnd))
                  err("Internal procedures can not contain procedures",304);
              }
	    ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 993 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, RECURSIVE_OP, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 995 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, PURE_OP, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 997 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, ELEMENTAL_OP, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 1001 "gram1.y"
    { PTR_BFND p;

	      (yyval.symbol) = make_procedure((yyvsp[(1) - (1)].hash_entry), LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
             /* if (pred_bfnd != global_bfnd)
		 {
	         err("Misplaced SUBROUTINE statement", 34);
		 }  
              */
	      p = get_bfnd(fi,PROC_HEDR, (yyval.symbol), LLNULL, LLNULL, LLNULL);
              (yyval.symbol)->entry.proc_decl.proc_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
            ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 1018 "gram1.y"
    { PTR_BFND p;

	      (yyval.symbol) = make_function((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
             /* if (pred_bfnd != global_bfnd)
	         err("Misplaced FUNCTION statement", 34); */
	      p = get_bfnd(fi,FUNC_HEDR, (yyval.symbol), LLNULL, LLNULL, LLNULL);
              (yyval.symbol)->entry.func_decl.func_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
            ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 1032 "gram1.y"
    { PTR_BFND p;
             PTR_LLND l;

	      (yyval.symbol) = make_function((yyvsp[(4) - (4)].hash_entry), (yyvsp[(1) - (4)].data_type), LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
              l = make_llnd(fi, TYPE_OP, LLNULL, LLNULL, SMNULL);
              l->type = (yyvsp[(1) - (4)].data_type);
	      p = get_bfnd(fi,FUNC_HEDR, (yyval.symbol), LLNULL, l, LLNULL);
              (yyval.symbol)->entry.func_decl.func_hedr = p;
            /*  if (pred_bfnd != global_bfnd)
	         err("Misplaced FUNCTION statement", 34);*/
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
/*
	      $$ = make_function($4, $1, LOCAL);
	      $$->decl = YES;
	      p = get_bfnd(fi,FUNC_HEDR, $$, LLNULL, LLNULL, LLNULL);
              if (pred_bfnd != global_bfnd)
	         errstr("cftn.gram: misplaced SUBROUTINE statement.");
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
*/
           ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 1056 "gram1.y"
    { PTR_BFND p;
             PTR_LLND l;
	      (yyval.symbol) = make_function((yyvsp[(5) - (5)].hash_entry), (yyvsp[(1) - (5)].data_type), LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
              if((yyvsp[(2) - (5)].ll_node)->variant == RECURSIVE_OP)
	         (yyval.symbol)->attr = (yyval.symbol)->attr | RECURSIVE_BIT;
              l = make_llnd(fi, TYPE_OP, LLNULL, LLNULL, SMNULL);
              l->type = (yyvsp[(1) - (5)].data_type);
             /* if (pred_bfnd != global_bfnd)
	         err("Misplaced FUNCTION statement", 34);*/
	      p = get_bfnd(fi,FUNC_HEDR, (yyval.symbol), LLNULL, l, (yyvsp[(2) - (5)].ll_node));
              (yyval.symbol)->entry.func_decl.func_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
            ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1072 "gram1.y"
    { PTR_BFND p;

	      (yyval.symbol) = make_function((yyvsp[(4) - (4)].hash_entry), TYNULL, LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
              if((yyvsp[(1) - (4)].ll_node)->variant == RECURSIVE_OP)
	        (yyval.symbol)->attr = (yyval.symbol)->attr | RECURSIVE_BIT;
              /*if (pred_bfnd != global_bfnd)
	         err("Misplaced FUNCTION statement",34);*/
	      p = get_bfnd(fi,FUNC_HEDR, (yyval.symbol), LLNULL, LLNULL, (yyvsp[(1) - (4)].ll_node));
              (yyval.symbol)->entry.func_decl.func_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
            ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1086 "gram1.y"
    { PTR_BFND p;
              PTR_LLND l;
	      (yyval.symbol) = make_function((yyvsp[(5) - (5)].hash_entry), (yyvsp[(2) - (5)].data_type), LOCAL);
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
              if((yyvsp[(1) - (5)].ll_node)->variant == RECURSIVE_OP)
	        (yyval.symbol)->attr = (yyval.symbol)->attr | RECURSIVE_BIT;
              l = make_llnd(fi, TYPE_OP, LLNULL, LLNULL, SMNULL);
              l->type = (yyvsp[(2) - (5)].data_type);
             /* if (pred_bfnd != global_bfnd)
	          err("Misplaced FUNCTION statement",34);*/
	      p = get_bfnd(fi,FUNC_HEDR, (yyval.symbol), LLNULL, l, (yyvsp[(1) - (5)].ll_node));
              (yyval.symbol)->entry.func_decl.func_hedr = p;
	      set_blobs(p, pred_bfnd, NEW_GROUP1);
              add_scope_level(p, NO);
            ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1104 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1106 "gram1.y"
    { PTR_SYMB s;
              s = make_scalar((yyvsp[(4) - (5)].hash_entry), TYNULL, LOCAL);
              (yyval.ll_node) = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
            ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 1113 "gram1.y"
    { (yyval.hash_entry) = look_up_sym(yytext); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 1116 "gram1.y"
    { (yyval.symbol) = make_program(look_up_sym("_MAIN")); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1118 "gram1.y"
    {
              (yyval.symbol) = make_program((yyvsp[(1) - (1)].hash_entry));
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
            ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1124 "gram1.y"
    { (yyval.symbol) = make_program(look_up_sym("_BLOCK")); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1126 "gram1.y"
    {
              (yyval.symbol) = make_program((yyvsp[(1) - (1)].hash_entry)); 
	      (yyval.symbol)->decl = YES;   /* variable declaration has been seen. */
	    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1133 "gram1.y"
    { (yyval.symbol) = SMNULL; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1135 "gram1.y"
    { (yyval.symbol) = SMNULL; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1137 "gram1.y"
    { (yyval.symbol) = (yyvsp[(2) - (3)].symbol); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 1142 "gram1.y"
    { (yyval.symbol) = set_id_list((yyvsp[(1) - (3)].symbol), (yyvsp[(3) - (3)].symbol)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1146 "gram1.y"
    {
	      (yyval.symbol) = make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, IO);
            ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1150 "gram1.y"
    { (yyval.symbol) = make_scalar(look_up_sym("*"), TYNULL, IO); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1156 "gram1.y"
    { char *s;

	      s = copyn(yyleng+1, yytext);
	      s[yyleng] = '\0';
	      (yyval.charp) = s;
	    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1165 "gram1.y"
    { needkwd = 1; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1169 "gram1.y"
    { needkwd = NO; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1174 "gram1.y"
    { colon_flag = YES; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1195 "gram1.y"
    {
	      saveall = YES;
	      (yyval.bf_node) = get_bfnd(fi,SAVE_DECL, SMNULL, LLNULL, LLNULL, LLNULL);
	    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1200 "gram1.y"
    {
	      (yyval.bf_node) = get_bfnd(fi,SAVE_DECL, SMNULL, (yyvsp[(4) - (4)].ll_node), LLNULL, LLNULL);
            ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1205 "gram1.y"
    { PTR_LLND p;

	      p = make_llnd(fi,STMT_STR, LLNULL, LLNULL, SMNULL);
	      p->entry.string_val = copys(stmtbuf);
	      (yyval.bf_node) = get_bfnd(fi,FORMAT_STAT, SMNULL, p, LLNULL, LLNULL);
             ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1212 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,PARAM_DECL, SMNULL, (yyvsp[(4) - (5)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 1228 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, INTERFACE_STMT, SMNULL, LLNULL, LLNULL, LLNULL); 
             /* add_scope_level($$, NO); */    
            ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 1232 "gram1.y"
    { PTR_SYMB s;

	      s = make_procedure((yyvsp[(3) - (3)].hash_entry), LOCAL);
	      s->variant = INTERFACE_NAME;
	      (yyval.bf_node) = get_bfnd(fi, INTERFACE_STMT, s, LLNULL, LLNULL, LLNULL);
              /*add_scope_level($$, NO);*/
	    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 1240 "gram1.y"
    { PTR_SYMB s;

	      s = make_function((yyvsp[(4) - (5)].hash_entry), global_default, LOCAL);
	      s->variant = INTERFACE_NAME;
	      (yyval.bf_node) = get_bfnd(fi, INTERFACE_OPERATOR, s, LLNULL, LLNULL, LLNULL);
              /*add_scope_level($$, NO);*/
	    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 1248 "gram1.y"
    { PTR_SYMB s;


	      s = make_procedure(look_up_sym("="), LOCAL);
	      s->variant = INTERFACE_NAME;
	      (yyval.bf_node) = get_bfnd(fi, INTERFACE_ASSIGNMENT, s, LLNULL, LLNULL, LLNULL);
              /*add_scope_level($$, NO);*/
	    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 1257 "gram1.y"
    { parstate = INDCL;
              (yyval.bf_node) = get_bfnd(fi, CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL); 
	      /*process_interface($$);*/ /*podd 01.02.03*/
              /*delete_beyond_scope_level(pred_bfnd);*/
	    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 1265 "gram1.y"
    { (yyval.hash_entry) = look_up_sym(yytext); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 1269 "gram1.y"
    { (yyval.hash_entry) = (yyvsp[(1) - (1)].hash_entry); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 1271 "gram1.y"
    { (yyval.hash_entry) = (yyvsp[(1) - (1)].hash_entry); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 1275 "gram1.y"
    { (yyval.hash_entry) = look_up_op(PLUS); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 1277 "gram1.y"
    { (yyval.hash_entry) = look_up_op(MINUS); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 1279 "gram1.y"
    { (yyval.hash_entry) = look_up_op(ASTER); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 1281 "gram1.y"
    { (yyval.hash_entry) = look_up_op(DASTER); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 1283 "gram1.y"
    { (yyval.hash_entry) = look_up_op(SLASH); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 1285 "gram1.y"
    { (yyval.hash_entry) = look_up_op(DSLASH); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 1287 "gram1.y"
    { (yyval.hash_entry) = look_up_op(AND); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 1289 "gram1.y"
    { (yyval.hash_entry) = look_up_op(OR); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 1291 "gram1.y"
    { (yyval.hash_entry) = look_up_op(XOR); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 1293 "gram1.y"
    { (yyval.hash_entry) = look_up_op(NOT); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 1295 "gram1.y"
    { (yyval.hash_entry) = look_up_op(EQ); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 1297 "gram1.y"
    { (yyval.hash_entry) = look_up_op(NE); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 1299 "gram1.y"
    { (yyval.hash_entry) = look_up_op(GT); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 1301 "gram1.y"
    { (yyval.hash_entry) = look_up_op(GE); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 1303 "gram1.y"
    { (yyval.hash_entry) = look_up_op(LT); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 1305 "gram1.y"
    { (yyval.hash_entry) = look_up_op(LE); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 1307 "gram1.y"
    { (yyval.hash_entry) = look_up_op(NEQV); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 1309 "gram1.y"
    { (yyval.hash_entry) = look_up_op(EQV); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1314 "gram1.y"
    {
             PTR_SYMB s;
         
             type_var = s = make_derived_type((yyvsp[(4) - (4)].hash_entry), TYNULL, LOCAL);	
             (yyval.bf_node) = get_bfnd(fi, STRUCT_DECL, s, LLNULL, LLNULL, LLNULL);
             add_scope_level((yyval.bf_node), NO);
	   ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1323 "gram1.y"
    { PTR_SYMB s;
         
             type_var = s = make_derived_type((yyvsp[(7) - (7)].hash_entry), TYNULL, LOCAL);	
	     s->attr = s->attr | type_opt;
             (yyval.bf_node) = get_bfnd(fi, STRUCT_DECL, s, (yyvsp[(5) - (7)].ll_node), LLNULL, LLNULL);
             add_scope_level((yyval.bf_node), NO);
	   ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1333 "gram1.y"
    {
	     (yyval.bf_node) = get_bfnd(fi, CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	     if (type_var != SMNULL)
               process_type(type_var, (yyval.bf_node));
             type_var = SMNULL;
	     delete_beyond_scope_level(pred_bfnd);
           ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1341 "gram1.y"
    {
             (yyval.bf_node) = get_bfnd(fi, CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL);
	     if (type_var != SMNULL)
               process_type(type_var, (yyval.bf_node));
             type_var = SMNULL;
	     delete_beyond_scope_level(pred_bfnd);	
           ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1351 "gram1.y"
    { 
	      PTR_LLND q, r, l;
	     /* PTR_SYMB s;*/
	      PTR_TYPE t;
	      int type_opts;

	      vartype = (yyvsp[(1) - (7)].data_type);
              if((yyvsp[(6) - (7)].ll_node) && vartype->variant != T_STRING)
                errstr("Non character entity  %s  has length specification",(yyvsp[(3) - (7)].hash_entry)->ident,41);
              t = make_type_node(vartype, (yyvsp[(6) - (7)].ll_node));
	      type_opts = type_options;
	      if ((yyvsp[(5) - (7)].ll_node)) type_opts = type_opts | DIMENSION_BIT;
	      if ((yyvsp[(5) - (7)].ll_node))
		 q = deal_with_options((yyvsp[(3) - (7)].hash_entry), t, type_opts, (yyvsp[(5) - (7)].ll_node), ndim, (yyvsp[(7) - (7)].ll_node), (yyvsp[(5) - (7)].ll_node));
	      else q = deal_with_options((yyvsp[(3) - (7)].hash_entry), t, type_opts, attr_dims, attr_ndim, (yyvsp[(7) - (7)].ll_node), (yyvsp[(5) - (7)].ll_node));
	      r = make_llnd(fi, EXPR_LIST, q, LLNULL, SMNULL);
	      l = make_llnd(fi, TYPE_OP, LLNULL, LLNULL, SMNULL);
	      l->type = vartype;
	      (yyval.bf_node) = get_bfnd(fi,VAR_DECL, SMNULL, r, l, (yyvsp[(2) - (7)].ll_node));
	    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1372 "gram1.y"
    { 
	      PTR_LLND q, r;
	    /*  PTR_SYMB s;*/
              PTR_TYPE t;
	      int type_opts;
              if((yyvsp[(5) - (6)].ll_node) && vartype->variant != T_STRING)
                errstr("Non character entity  %s  has length specification",(yyvsp[(3) - (6)].hash_entry)->ident,41);
              t = make_type_node(vartype, (yyvsp[(5) - (6)].ll_node));
	      type_opts = type_options;
	      if ((yyvsp[(4) - (6)].ll_node)) type_opts = type_opts | DIMENSION_BIT;
	      if ((yyvsp[(4) - (6)].ll_node))
		 q = deal_with_options((yyvsp[(3) - (6)].hash_entry), t, type_opts, (yyvsp[(4) - (6)].ll_node), ndim, (yyvsp[(6) - (6)].ll_node), (yyvsp[(4) - (6)].ll_node));
	      else q = deal_with_options((yyvsp[(3) - (6)].hash_entry), t, type_opts, attr_dims, attr_ndim, (yyvsp[(6) - (6)].ll_node), (yyvsp[(4) - (6)].ll_node));
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (6)].bf_node)->entry.Template.ll_ptr1);
       	    ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1391 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1393 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1395 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (5)].ll_node); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1399 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1401 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1405 "gram1.y"
    { type_options = type_options | PARAMETER_BIT; 
              (yyval.ll_node) = make_llnd(fi, PARAMETER_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1409 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1411 "gram1.y"
    { type_options = type_options | ALLOCATABLE_BIT;
              (yyval.ll_node) = make_llnd(fi, ALLOCATABLE_OP, LLNULL, LLNULL, SMNULL);
	    ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1415 "gram1.y"
    { type_options = type_options | DIMENSION_BIT;
	      attr_ndim = ndim;
	      attr_dims = (yyvsp[(2) - (2)].ll_node);
              (yyval.ll_node) = make_llnd(fi, DIMENSION_OP, (yyvsp[(2) - (2)].ll_node), LLNULL, SMNULL);
            ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1421 "gram1.y"
    { type_options = type_options | EXTERNAL_BIT;
              (yyval.ll_node) = make_llnd(fi, EXTERNAL_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1425 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (4)].ll_node); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1427 "gram1.y"
    { type_options = type_options | INTRINSIC_BIT;
              (yyval.ll_node) = make_llnd(fi, INTRINSIC_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1431 "gram1.y"
    { type_options = type_options | OPTIONAL_BIT;
              (yyval.ll_node) = make_llnd(fi, OPTIONAL_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1435 "gram1.y"
    { type_options = type_options | POINTER_BIT;
              (yyval.ll_node) = make_llnd(fi, POINTER_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1439 "gram1.y"
    { type_options = type_options | SAVE_BIT; 
              (yyval.ll_node) = make_llnd(fi, SAVE_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1443 "gram1.y"
    { type_options = type_options | SAVE_BIT; 
              (yyval.ll_node) = make_llnd(fi, STATIC_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1447 "gram1.y"
    { type_options = type_options | TARGET_BIT; 
              (yyval.ll_node) = make_llnd(fi, TARGET_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1453 "gram1.y"
    { type_options = type_options | IN_BIT;  type_opt = IN_BIT; 
              (yyval.ll_node) = make_llnd(fi, IN_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1457 "gram1.y"
    { type_options = type_options | OUT_BIT;  type_opt = OUT_BIT; 
              (yyval.ll_node) = make_llnd(fi, OUT_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1461 "gram1.y"
    { type_options = type_options | INOUT_BIT;  type_opt = INOUT_BIT;
              (yyval.ll_node) = make_llnd(fi, INOUT_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1467 "gram1.y"
    { type_options = type_options | PUBLIC_BIT; 
              type_opt = PUBLIC_BIT;
              (yyval.ll_node) = make_llnd(fi, PUBLIC_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1472 "gram1.y"
    { type_options =  type_options | PRIVATE_BIT;
               type_opt = PRIVATE_BIT;
              (yyval.ll_node) = make_llnd(fi, PRIVATE_OP, LLNULL, LLNULL, SMNULL);
            ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1479 "gram1.y"
    { 
	      PTR_LLND q, r;
	      PTR_SYMB s;

              s = make_scalar((yyvsp[(7) - (7)].hash_entry), TYNULL, LOCAL);
	      s->attr = s->attr | type_opt;	
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, INTENT_STMT, SMNULL, r, (yyvsp[(4) - (7)].ll_node), LLNULL);
	    ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1490 "gram1.y"
    { 
	      PTR_LLND q, r;
	      PTR_SYMB s;

              s = make_scalar((yyvsp[(3) - (3)].hash_entry), TYNULL, LOCAL);	
	      s->attr = s->attr | type_opt;
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
  	    ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1503 "gram1.y"
    { 
	      PTR_LLND q, r;
	      PTR_SYMB s;

              s = make_scalar((yyvsp[(4) - (4)].hash_entry), TYNULL, LOCAL);	
	      s->attr = s->attr | OPTIONAL_BIT;
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, OPTIONAL_STMT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1514 "gram1.y"
    { 
	      PTR_LLND q, r;
	      PTR_SYMB s;

              s = make_scalar((yyvsp[(3) - (3)].hash_entry), TYNULL, LOCAL);	
	      s->attr = s->attr | OPTIONAL_BIT;
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
  	    ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1527 "gram1.y"
    { 
	      PTR_LLND r;
	      PTR_SYMB s;

              s = (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol; 
              s->attr = s->attr | SAVE_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, STATIC_STMT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1537 "gram1.y"
    { 
	      PTR_LLND r;
	      PTR_SYMB s;

              s = (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol;
              s->attr = s->attr | SAVE_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
  	    ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1550 "gram1.y"
    {
	      privateall = 1;
	      (yyval.bf_node) = get_bfnd(fi, PRIVATE_STMT, SMNULL, LLNULL, LLNULL, LLNULL);
	    ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1555 "gram1.y"
    {
	      /*type_options = type_options | PRIVATE_BIT;*/
	      (yyval.bf_node) = get_bfnd(fi, PRIVATE_STMT, SMNULL, (yyvsp[(5) - (5)].ll_node), LLNULL, LLNULL);
            ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1561 "gram1.y"
    {type_opt = PRIVATE_BIT;;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1565 "gram1.y"
    { 
	      (yyval.bf_node) = get_bfnd(fi, SEQUENCE_STMT, SMNULL, LLNULL, LLNULL, LLNULL);
            ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1570 "gram1.y"
    {
	      /*saveall = YES;*/ /*14.03.03*/
	      (yyval.bf_node) = get_bfnd(fi, PUBLIC_STMT, SMNULL, LLNULL, LLNULL, LLNULL);
	    ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1575 "gram1.y"
    {
	      /*type_options = type_options | PUBLIC_BIT;*/
	      (yyval.bf_node) = get_bfnd(fi, PUBLIC_STMT, SMNULL, (yyvsp[(5) - (5)].ll_node), LLNULL, LLNULL);
            ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1581 "gram1.y"
    {type_opt = PUBLIC_BIT;;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1585 "gram1.y"
    {
	      type_options = 0;
              /* following block added by dbg */
	      ndim = 0;
	      attr_ndim = 0;
	      attr_dims = LLNULL;
	      /* end section added by dbg */
              (yyval.data_type) = make_type_node((yyvsp[(1) - (4)].data_type), (yyvsp[(3) - (4)].ll_node));
            ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1595 "gram1.y"
    { PTR_TYPE t;

	      type_options = 0;
	      ndim = 0;
	      attr_ndim = 0;
	      attr_dims = LLNULL;
              t = lookup_type((yyvsp[(3) - (5)].hash_entry));
	      vartype = t;
	      (yyval.data_type) = make_type_node(t, LLNULL);
            ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1608 "gram1.y"
    {opt_kwd_hedr = YES;;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 1613 "gram1.y"
    { PTR_TYPE p;
	      PTR_LLND q;
	      PTR_SYMB s;
              s = (yyvsp[(2) - (2)].hash_entry)->id_attr;
	      if (s)
		   s->attr = (yyvsp[(1) - (2)].token);
	      else {
		p = undeftype ? global_unknown : impltype[*(yyvsp[(2) - (2)].hash_entry)->ident - 'a'];
                s = install_entry((yyvsp[(2) - (2)].hash_entry), SOFT);
		s->attr = (yyvsp[(1) - (2)].token);
                set_type(s, p, LOCAL);
	      }
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(2) - (2)].hash_entry)->id_attr);
	      q = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,ATTR_DECL, SMNULL, q, LLNULL, LLNULL);
	    ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1632 "gram1.y"
    { PTR_TYPE p;
	      PTR_LLND q, r;
	      PTR_SYMB s;
	      int att;

	      att = (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1->entry.Template.ll_ptr1->
		    entry.Template.symbol->attr;
              s = (yyvsp[(3) - (3)].hash_entry)->id_attr;
	      if (s)
		   s->attr = att;
	      else {
		p = undeftype ? global_unknown : impltype[*(yyvsp[(3) - (3)].hash_entry)->ident - 'a'];
                s = install_entry((yyvsp[(3) - (3)].hash_entry), SOFT);
		s->attr = att;
                set_type(s, p, LOCAL);
	      }
	      q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].hash_entry)->id_attr);
	      for (r = (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1;
		   r->entry.list.next;
		   r = r->entry.list.next) ;
	      r->entry.list.next = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);

	    ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1658 "gram1.y"
    { (yyval.token) = ATT_GLOBAL; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1660 "gram1.y"
    { (yyval.token) = ATT_CLUSTER; ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1672 "gram1.y"
    {
/*		  varleng = ($1<0 || $1==TYLONG ? 0 : typesize[$1]); */
		  vartype = (yyvsp[(1) - (1)].data_type);
		;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1679 "gram1.y"
    { (yyval.data_type) = global_int; ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1680 "gram1.y"
    { (yyval.data_type) = global_float; ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1681 "gram1.y"
    { (yyval.data_type) = global_complex; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1682 "gram1.y"
    { (yyval.data_type) = global_double; ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1683 "gram1.y"
    { (yyval.data_type) = global_dcomplex; ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1684 "gram1.y"
    { (yyval.data_type) = global_bool; ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1685 "gram1.y"
    { (yyval.data_type) = global_string; ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1690 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1692 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1696 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, LEN_OP, (yyvsp[(3) - (5)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1698 "gram1.y"
    { PTR_LLND l;

                 l = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL); 
                 l->entry.string_val = (char *)"*";
                 (yyval.ll_node) = make_llnd(fi, LEN_OP, l,l, SMNULL);
                ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1705 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi, LEN_OP, (yyvsp[(5) - (6)].ll_node), (yyvsp[(5) - (6)].ll_node), SMNULL);;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1709 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1711 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1713 "gram1.y"
    { /*$$ = make_llnd(fi, PAREN_OP, $2, LLNULL, SMNULL);*/  (yyval.ll_node) = (yyvsp[(3) - (5)].ll_node);  ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1721 "gram1.y"
    { if((yyvsp[(7) - (9)].ll_node)->variant==LENGTH_OP && (yyvsp[(3) - (9)].ll_node)->variant==(yyvsp[(7) - (9)].ll_node)->variant)
                (yyvsp[(7) - (9)].ll_node)->variant=KIND_OP;
                (yyval.ll_node) = make_llnd(fi, CONS, (yyvsp[(3) - (9)].ll_node), (yyvsp[(7) - (9)].ll_node), SMNULL); 
            ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1728 "gram1.y"
    { if(vartype->variant == T_STRING)
                (yyval.ll_node) = make_llnd(fi,LENGTH_OP,(yyvsp[(1) - (1)].ll_node),LLNULL,SMNULL);
              else
                (yyval.ll_node) = make_llnd(fi,KIND_OP,(yyvsp[(1) - (1)].ll_node),LLNULL,SMNULL);
            ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1734 "gram1.y"
    { PTR_LLND l;
	      l = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
	      l->entry.string_val = (char *)"*";
              (yyval.ll_node) = make_llnd(fi,LENGTH_OP,l,LLNULL,SMNULL);
            ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1740 "gram1.y"
    { /* $$ = make_llnd(fi, SPEC_PAIR, $2, LLNULL, SMNULL); */
	     char *q;
             q = (yyvsp[(1) - (2)].ll_node)->entry.string_val;
  	     if (strcmp(q, "len") == 0)
               (yyval.ll_node) = make_llnd(fi,LENGTH_OP,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
             else
                (yyval.ll_node) = make_llnd(fi,KIND_OP,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);              
            ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1749 "gram1.y"
    { PTR_LLND l;
	      l = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
	      l->entry.string_val = (char *)"*";
              (yyval.ll_node) = make_llnd(fi,LENGTH_OP,l,LLNULL,SMNULL);
            ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1757 "gram1.y"
    {endioctl();;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1770 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1772 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (2)].ll_node); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1775 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, POINTST_OP, LLNULL, (yyvsp[(2) - (2)].ll_node), SMNULL); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1779 "gram1.y"
    { PTR_SYMB s;
	      PTR_LLND q, r;
	      if(! (yyvsp[(5) - (5)].ll_node)) {
		err("No dimensions in DIMENSION statement", 42);
	      }
              if(statement_kind == 1) /*DVM-directive*/
                err("No shape specification", 65);                
	      s = make_array((yyvsp[(4) - (5)].hash_entry), TYNULL, (yyvsp[(5) - (5)].ll_node), ndim, LOCAL);
	      s->attr = s->attr | DIMENSION_BIT;
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(5) - (5)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(5) - (5)].ll_node);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,DIM_STAT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1794 "gram1.y"
    {  PTR_SYMB s;
	      PTR_LLND q, r;
	      if(! (yyvsp[(4) - (4)].ll_node)) {
		err("No dimensions in DIMENSION statement", 42);
	      }
	      s = make_array((yyvsp[(3) - (4)].hash_entry), TYNULL, (yyvsp[(4) - (4)].ll_node), ndim, LOCAL);
	      s->attr = s->attr | DIMENSION_BIT;
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(4) - (4)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(4) - (4)].ll_node);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (4)].bf_node)->entry.Template.ll_ptr1);
	;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1810 "gram1.y"
    {/* PTR_SYMB s;*/
	      PTR_LLND r;

	         /*if(!$5) {
		   err("No dimensions in ALLOCATABLE statement",305);		
	           }
	          s = make_array($4, TYNULL, $5, ndim, LOCAL);
	          s->attr = s->attr | ALLOCATABLE_BIT;
	          q = make_llnd(fi,ARRAY_REF, $5, LLNULL, s);
	          s->type->entry.ar_decl.ranges = $5;
                  r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                */
              (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol->attr = (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol->attr | ALLOCATABLE_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, ALLOCATABLE_STMT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1828 "gram1.y"
    {  /*PTR_SYMB s;*/
	      PTR_LLND r;

	        /*  if(! $4) {
		      err("No dimensions in ALLOCATABLE statement",305);
		
	            }
	           s = make_array($3, TYNULL, $4, ndim, LOCAL);
	           s->attr = s->attr | ALLOCATABLE_BIT;
	           q = make_llnd(fi,ARRAY_REF, $4, LLNULL, s);
	           s->type->entry.ar_decl.ranges = $4;
	           r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                */
              (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol->attr = (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol->attr | ALLOCATABLE_BIT;
              r = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1848 "gram1.y"
    { PTR_SYMB s;
	      PTR_LLND  r;
           
	          /*  if(! $5) {
		      err("No dimensions in POINTER statement",306);	    
	              } 
	             s = make_array($4, TYNULL, $5, ndim, LOCAL);
	             s->attr = s->attr | POINTER_BIT;
	             q = make_llnd(fi,ARRAY_REF, $5, LLNULL, s);
	             s->type->entry.ar_decl.ranges = $5;
                   */

                  /*s = make_pointer( $4->entry.Template.symbol->parent, TYNULL, LOCAL);*/ /*17.02.03*/
                 /*$4->entry.Template.symbol->attr = $4->entry.Template.symbol->attr | POINTER_BIT;*/
              s = (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol; /*17.02.03*/
              s->attr = s->attr | POINTER_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, POINTER_STMT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1868 "gram1.y"
    {  PTR_SYMB s;
	      PTR_LLND r;

     	        /*  if(! $4) {
	        	err("No dimensions in POINTER statement",306);
	            }
	           s = make_array($3, TYNULL, $4, ndim, LOCAL);
	           s->attr = s->attr | POINTER_BIT;
	           q = make_llnd(fi,ARRAY_REF, $4, LLNULL, s);
	           s->type->entry.ar_decl.ranges = $4;
                */

                /*s = make_pointer( $3->entry.Template.symbol->parent, TYNULL, LOCAL);*/ /*17.02.03*/
                /*$3->entry.Template.symbol->attr = $3->entry.Template.symbol->attr | POINTER_BIT;*/
              s = (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol; /*17.02.03*/
              s->attr = s->attr | POINTER_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1890 "gram1.y"
    {/* PTR_SYMB s;*/
	      PTR_LLND r;


	     /* if(! $5) {
		err("No dimensions in TARGET statement",307);
	      }
	      s = make_array($4, TYNULL, $5, ndim, LOCAL);
	      s->attr = s->attr | TARGET_BIT;
	      q = make_llnd(fi,ARRAY_REF, $5, LLNULL, s);
	      s->type->entry.ar_decl.ranges = $5;
             */
              (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol->attr = (yyvsp[(4) - (4)].ll_node)->entry.Template.symbol->attr | TARGET_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi, TARGET_STMT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1907 "gram1.y"
    {  /*PTR_SYMB s;*/
	      PTR_LLND r;

	     /* if(! $4) {
		err("No dimensions in TARGET statement",307);
	      }
	      s = make_array($3, TYNULL, $4, ndim, LOCAL);
	      s->attr = s->attr | TARGET_BIT;
	      q = make_llnd(fi,ARRAY_REF, $4, LLNULL, s);
	      s->type->entry.ar_decl.ranges = $4;
              */
              (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol->attr = (yyvsp[(3) - (3)].ll_node)->entry.Template.symbol->attr | TARGET_BIT;
	      r = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1925 "gram1.y"
    { PTR_LLND p, q;

              p = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      q = make_llnd(fi,COMM_LIST, p, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,COMM_STAT, SMNULL, q, LLNULL, LLNULL);
	    ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1932 "gram1.y"
    { PTR_LLND p, q;

              p = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      q = make_llnd(fi,COMM_LIST, p, LLNULL, (yyvsp[(3) - (4)].symbol));
	      (yyval.bf_node) = get_bfnd(fi,COMM_STAT, SMNULL, q, LLNULL, LLNULL);
	    ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1939 "gram1.y"
    { PTR_LLND p, q;

              p = make_llnd(fi,EXPR_LIST, (yyvsp[(5) - (5)].ll_node), LLNULL, SMNULL);
	      q = make_llnd(fi,COMM_LIST, p, LLNULL, (yyvsp[(3) - (5)].symbol));
	      add_to_lowList(q, (yyvsp[(1) - (5)].bf_node)->entry.Template.ll_ptr1);
	    ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1946 "gram1.y"
    { PTR_LLND p, r;

              p = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      /*q = make_llnd(fi,COMM_LIST, p, LLNULL, SMNULL);*/
	      for (r = (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1;
		   r->entry.list.next;
		   r = r->entry.list.next);
	      add_to_lowLevelList(p, r->entry.Template.ll_ptr1);
	    ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1959 "gram1.y"
    { PTR_LLND q, r;

              q = make_llnd(fi,EXPR_LIST, (yyvsp[(4) - (4)].ll_node), LLNULL, SMNULL);
	      r = make_llnd(fi,NAMELIST_LIST, q, LLNULL, (yyvsp[(3) - (4)].symbol));
	      (yyval.bf_node) = get_bfnd(fi,NAMELIST_STAT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1966 "gram1.y"
    { PTR_LLND q, r;

              q = make_llnd(fi,EXPR_LIST, (yyvsp[(5) - (5)].ll_node), LLNULL, SMNULL);
	      r = make_llnd(fi,NAMELIST_LIST, q, LLNULL, (yyvsp[(3) - (5)].symbol));
	      add_to_lowList(r, (yyvsp[(1) - (5)].bf_node)->entry.Template.ll_ptr1);
	    ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1973 "gram1.y"
    { PTR_LLND q, r;

              q = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      for (r = (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1;
		   r->entry.list.next;
		   r = r->entry.list.next);
	      add_to_lowLevelList(q, r->entry.Template.ll_ptr1);
	    ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1984 "gram1.y"
    { (yyval.symbol) =  make_local_entity((yyvsp[(2) - (3)].hash_entry), NAMELIST_NAME,global_default,LOCAL); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1988 "gram1.y"
    { (yyval.symbol) = NULL; /*make_common(look_up_sym("*"));*/ ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1990 "gram1.y"
    { (yyval.symbol) = make_common((yyvsp[(2) - (3)].hash_entry)); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1995 "gram1.y"
    {  PTR_SYMB s;
	
	      if((yyvsp[(2) - (2)].ll_node)) {
		s = make_array((yyvsp[(1) - (2)].hash_entry), TYNULL, (yyvsp[(2) - (2)].ll_node), ndim, LOCAL);
                s->attr = s->attr | DIMENSION_BIT;
		s->type->entry.ar_decl.ranges = (yyvsp[(2) - (2)].ll_node);
		(yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(2) - (2)].ll_node), LLNULL, s);
	      }
	      else {
		s = make_scalar((yyvsp[(1) - (2)].hash_entry), TYNULL, LOCAL);	
		(yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
	      }

          ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 2013 "gram1.y"
    { PTR_LLND p, q;
              PTR_SYMB s;

	      s = make_external((yyvsp[(4) - (4)].hash_entry), TYNULL);
	      s->attr = s->attr | EXTERNAL_BIT;
              q = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      p = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,EXTERN_STAT, SMNULL, p, LLNULL, LLNULL);
	    ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 2024 "gram1.y"
    { PTR_LLND p, q;
              PTR_SYMB s;

	      s = make_external((yyvsp[(3) - (3)].hash_entry), TYNULL);
	      s->attr = s->attr | EXTERNAL_BIT;
              p = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      q = make_llnd(fi,EXPR_LIST, p, LLNULL, SMNULL);
	      add_to_lowLevelList(q, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	    ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 2036 "gram1.y"
    { PTR_LLND p, q;
              PTR_SYMB s;

	      s = make_intrinsic((yyvsp[(4) - (4)].hash_entry), TYNULL); /*make_function($3, TYNULL, NO);*/
	      s->attr = s->attr | INTRINSIC_BIT;
              q = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      p = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,INTRIN_STAT, SMNULL, p,
			     LLNULL, LLNULL);
	    ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 2048 "gram1.y"
    { PTR_LLND p, q;
              PTR_SYMB s;

	      s = make_intrinsic((yyvsp[(3) - (3)].hash_entry), TYNULL); /* make_function($3, TYNULL, NO);*/
	      s->attr = s->attr | INTRINSIC_BIT;
              p = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      q = make_llnd(fi,EXPR_LIST, p, LLNULL, SMNULL);
	      add_to_lowLevelList(q, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	    ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 2062 "gram1.y"
    {
	      (yyval.bf_node) = get_bfnd(fi,EQUI_STAT, SMNULL, (yyvsp[(3) - (3)].ll_node),
			     LLNULL, LLNULL);
	    ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 2068 "gram1.y"
    { 
	      add_to_lowLevelList((yyvsp[(3) - (3)].ll_node), (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	    ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 2075 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,EQUI_LIST, (yyvsp[(2) - (3)].ll_node), LLNULL, SMNULL);
           ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 2081 "gram1.y"
    { PTR_LLND p;
	      p = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      (yyval.ll_node) = make_llnd(fi,EXPR_LIST, (yyvsp[(1) - (3)].ll_node), p, SMNULL);
	    ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 2087 "gram1.y"
    { PTR_LLND p;

	      p = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(p, (yyvsp[(1) - (3)].ll_node));
	    ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 2095 "gram1.y"
    {  PTR_SYMB s;
           s=make_scalar((yyvsp[(1) - (1)].hash_entry),TYNULL,LOCAL);
           (yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
           s->attr = s->attr | EQUIVALENCE_BIT;
            /*$$=$1; $$->entry.Template.symbol->attr = $$->entry.Template.symbol->attr | EQUIVALENCE_BIT; */
        ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 2102 "gram1.y"
    {  PTR_SYMB s;
           s=make_array((yyvsp[(1) - (4)].hash_entry),TYNULL,LLNULL,0,LOCAL);
           (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (4)].ll_node), LLNULL, s);
           s->attr = s->attr | EQUIVALENCE_BIT;
            /*$$->entry.Template.symbol->attr = $$->entry.Template.symbol->attr | EQUIVALENCE_BIT; */
        ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 2121 "gram1.y"
    { PTR_LLND p;
              data_stat = NO;
	      p = make_llnd(fi,STMT_STR, LLNULL, LLNULL,
			    SMNULL);
              p->entry.string_val = copys(stmtbuf);
	      (yyval.bf_node) = get_bfnd(fi,DATA_DECL, SMNULL, p, LLNULL, LLNULL);
            ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 2135 "gram1.y"
    {data_stat = YES;;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 2139 "gram1.y"
    {
	      if (parstate == OUTSIDE)
	         { PTR_BFND p;

		   p = get_bfnd(fi,PROG_HEDR,
                                make_program(look_up_sym("_MAIN")),
                                LLNULL, LLNULL, LLNULL);
		   set_blobs(p, global_bfnd, NEW_GROUP1);
	           add_scope_level(p, NO);
		   position = IN_PROC; 
	  	   /*parstate = INDCL;*/
                 }
	      if(parstate < INDCL)
		{
		  /* enddcl();*/
		  parstate = INDCL;
		}
	    ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 2184 "gram1.y"
    {;;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 2188 "gram1.y"
    { (yyval.symbol)= make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 2192 "gram1.y"
    { (yyval.symbol)= make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL); 
              (yyval.symbol)->attr = (yyval.symbol)->attr | DATA_BIT; 
            ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 2198 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DATA_SUBS, (yyvsp[(2) - (3)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 2202 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DATA_RANGE, (yyvsp[(2) - (5)].ll_node), (yyvsp[(4) - (5)].ll_node), SMNULL); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 2206 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 2208 "gram1.y"
    { (yyval.ll_node) = add_to_lowLevelList((yyvsp[(3) - (3)].ll_node), (yyvsp[(1) - (3)].ll_node)); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 2212 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 2214 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 2218 "gram1.y"
    {(yyval.ll_node)= make_llnd(fi, DATA_IMPL_DO, (yyvsp[(2) - (7)].ll_node), (yyvsp[(6) - (7)].ll_node), (yyvsp[(4) - (7)].symbol)); ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 2222 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 2224 "gram1.y"
    { (yyval.ll_node) = add_to_lowLevelList((yyvsp[(3) - (3)].ll_node), (yyvsp[(1) - (3)].ll_node)); ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 2228 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DATA_ELT, (yyvsp[(2) - (2)].ll_node), LLNULL, (yyvsp[(1) - (2)].symbol)); ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 2230 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DATA_ELT, (yyvsp[(2) - (2)].ll_node), LLNULL, (yyvsp[(1) - (2)].symbol)); ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 2232 "gram1.y"
    {
              (yyvsp[(2) - (3)].ll_node)->entry.Template.ll_ptr2 = (yyvsp[(3) - (3)].ll_node);
              (yyval.ll_node) = make_llnd(fi, DATA_ELT, (yyvsp[(2) - (3)].ll_node), LLNULL, (yyvsp[(1) - (3)].symbol)); 
            ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 2237 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DATA_ELT, (yyvsp[(1) - (1)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 2261 "gram1.y"
    {if((yyvsp[(2) - (6)].ll_node)->entry.Template.symbol->variant != TYPE_NAME)
               errstr("Undefined type %s",(yyvsp[(2) - (6)].ll_node)->entry.Template.symbol->ident,319); 
           ;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 2306 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ICON_EXPR, (yyvsp[(1) - (1)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 2308 "gram1.y"
    {
              PTR_LLND p;

              p = intrinsic_op_node("+", UNARY_ADD_OP, (yyvsp[(2) - (2)].ll_node), LLNULL);
              (yyval.ll_node) = make_llnd(fi,ICON_EXPR, p, LLNULL, SMNULL);
            ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 2315 "gram1.y"
    {
              PTR_LLND p;
 
              p = intrinsic_op_node("-", MINUS_OP, (yyvsp[(2) - (2)].ll_node), LLNULL);
              (yyval.ll_node) = make_llnd(fi,ICON_EXPR, p, LLNULL, SMNULL);
            ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 2322 "gram1.y"
    {
              PTR_LLND p;
 
              p = intrinsic_op_node("+", ADD_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node));
              (yyval.ll_node) = make_llnd(fi,ICON_EXPR, p, LLNULL, SMNULL);
            ;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 2329 "gram1.y"
    {
              PTR_LLND p;
 
              p = intrinsic_op_node("-", SUBT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node));
              (yyval.ll_node) = make_llnd(fi,ICON_EXPR, p, LLNULL, SMNULL);
            ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 2338 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 2340 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("*", MULT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 2342 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("/", DIV_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 2346 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 2348 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("**", EXP_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 2352 "gram1.y"
    {
              PTR_LLND p;

              p = make_llnd(fi,INT_VAL, LLNULL, LLNULL, SMNULL);
              p->entry.ival = atoi(yytext);
              p->type = global_int;
              (yyval.ll_node) = make_llnd(fi,EXPR_LIST, p, LLNULL, SMNULL);
            ;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 2361 "gram1.y"
    {
              PTR_LLND p;
 
              p = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(1) - (1)].symbol));
              (yyval.ll_node) = make_llnd(fi,EXPR_LIST, p, LLNULL, SMNULL);
            ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 2368 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,EXPR_LIST, (yyvsp[(2) - (3)].ll_node), LLNULL, SMNULL);
            ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 2375 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,EXPR_LIST, (yyvsp[(1) - (1)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 2377 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 2381 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);
             (yyval.ll_node)->entry.Template.symbol->attr = (yyval.ll_node)->entry.Template.symbol->attr | SAVE_BIT;
           ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 2385 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,COMM_LIST, LLNULL, LLNULL, (yyvsp[(1) - (1)].symbol)); 
            (yyval.ll_node)->entry.Template.symbol->attr = (yyval.ll_node)->entry.Template.symbol->attr | SAVE_BIT;
          ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 2391 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(2) - (3)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 2393 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node), (yyvsp[(4) - (5)].ll_node), EXPR_LIST); ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 2397 "gram1.y"
    { as_op_kwd_ = YES; ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 2401 "gram1.y"
    { as_op_kwd_ = NO; ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 2406 "gram1.y"
    { 
             PTR_SYMB s; 
             s = make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);	
	     s->attr = s->attr | type_opt;
	     (yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
            ;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 2413 "gram1.y"
    { PTR_SYMB s;
	      s = make_function((yyvsp[(3) - (4)].hash_entry), global_default, LOCAL);
	      s->variant = INTERFACE_NAME;
              s->attr = s->attr | type_opt;
              (yyval.ll_node) = make_llnd(fi,OPERATOR_OP, LLNULL, LLNULL, s);
	    ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 2420 "gram1.y"
    { PTR_SYMB s;
	      s = make_procedure(look_up_sym("="), LOCAL);
	      s->variant = INTERFACE_NAME;
              s->attr = s->attr | type_opt;
              (yyval.ll_node) = make_llnd(fi,ASSIGNMENT_OP, LLNULL, LLNULL, s);
	    ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 2430 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 2432 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 2436 "gram1.y"
    { PTR_SYMB p;

                /* The check if name and expr have compatible types has
                   not been done yet. */ 
		p = make_constant((yyvsp[(1) - (3)].hash_entry), TYNULL);
 	        p->attr = p->attr | PARAMETER_BIT;
                p->entry.const_value = (yyvsp[(3) - (3)].ll_node);
		(yyval.ll_node) = make_llnd(fi,CONST_REF, LLNULL, LLNULL, p);
	    ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 2448 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, MODULE_PROC_STMT, SMNULL, (yyvsp[(2) - (2)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 2451 "gram1.y"
    { PTR_SYMB s;
 	      PTR_LLND q;

	      s = make_function((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);
	      s->variant = ROUTINE_NAME;
              q = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      (yyval.ll_node) = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	    ;}
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 2460 "gram1.y"
    { PTR_LLND p, q;
              PTR_SYMB s;

	      s = make_function((yyvsp[(3) - (3)].hash_entry), TYNULL, LOCAL);
	      s->variant = ROUTINE_NAME;
              p = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
	      q = make_llnd(fi,EXPR_LIST, p, LLNULL, SMNULL);
	      add_to_lowLevelList(q, (yyvsp[(1) - (3)].ll_node));
	    ;}
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 2473 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, USE_STMT, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL);
              /*add_scope_level($3->entry.Template.func_hedr, YES);*/ /*17.06.01*/
              copy_module_scope((yyvsp[(3) - (3)].symbol),LLNULL); /*17.03.03*/
              colon_flag = NO;
            ;}
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 2479 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, USE_STMT, (yyvsp[(3) - (6)].symbol), (yyvsp[(6) - (6)].ll_node), LLNULL, LLNULL); 
              /*add_scope_level(module_scope, YES); *//* 17.06.01*/
              copy_module_scope((yyvsp[(3) - (6)].symbol),(yyvsp[(6) - (6)].ll_node)); /*17.03.03 */
              colon_flag = NO;
            ;}
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 2485 "gram1.y"
    { PTR_LLND l;

	      l = make_llnd(fi, ONLY_NODE, LLNULL, LLNULL, SMNULL);
              (yyval.bf_node) = get_bfnd(fi, USE_STMT, (yyvsp[(3) - (6)].symbol), l, LLNULL, LLNULL);
            ;}
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 2491 "gram1.y"
    { PTR_LLND l;

	      l = make_llnd(fi, ONLY_NODE, (yyvsp[(7) - (7)].ll_node), LLNULL, SMNULL);
              (yyval.bf_node) = get_bfnd(fi, USE_STMT, (yyvsp[(3) - (7)].symbol), l, LLNULL, LLNULL);
            ;}
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 2499 "gram1.y"
    {
              if ((yyvsp[(1) - (1)].hash_entry)->id_attr == SMNULL)
	         warn1("Unknown module %s", (yyvsp[(1) - (1)].hash_entry)->ident,308);
              (yyval.symbol) = make_global_entity((yyvsp[(1) - (1)].hash_entry), MODULE_NAME, global_default, NO);
	      module_scope = (yyval.symbol)->entry.Template.func_hedr;
           
            ;}
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 2509 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 2511 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 2515 "gram1.y"
    {  PTR_HASH oldhash;
	       PTR_SYMB oldsym, newsym;
	       PTR_LLND l, m;

	       oldhash = just_look_up_sym_in_scope(module_scope, (yyvsp[(3) - (3)].hash_entry)->ident);
	       if (oldhash == HSNULL) {
                  errstr("Unknown identifier %s", (yyvsp[(3) - (3)].hash_entry)->ident,309);
	          (yyval.ll_node)= LLNULL;
	       }
	       else {
	         oldsym = oldhash->id_attr;
	         newsym = make_local_entity((yyvsp[(1) - (3)].hash_entry), oldsym->variant, oldsym->type, LOCAL);
	         /* copies data in entry.Template structure and attr */
	         copy_sym_data(oldsym, newsym);	         
	           /*newsym->entry.Template.base_name = oldsym;*//*19.03.03*/
	  	 l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, oldsym);
		 m = make_llnd(fi, VAR_REF, LLNULL, LLNULL, newsym);
		 (yyval.ll_node) = make_llnd(fi, RENAME_NODE, m, l, SMNULL);
 	      }
            ;}
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 2536 "gram1.y"
    {  PTR_HASH oldhash;
	       PTR_SYMB oldsym, newsym;
	       PTR_LLND m;

	       oldhash = just_look_up_sym_in_scope(module_scope, (yyvsp[(1) - (1)].hash_entry)->ident);
	       if (oldhash == HSNULL) {
                  errstr("Unknown identifier %s.", (yyvsp[(1) - (1)].hash_entry)->ident,309);
	          (yyval.ll_node)= LLNULL;
	       }
	       else {
	         oldsym = oldhash->id_attr;
	         newsym = make_local_entity((yyvsp[(1) - (1)].hash_entry), oldsym->variant, oldsym->type,
LOCAL);
	         /* copies data in entry.Template structure and attr */
	         copy_sym_data(oldsym, newsym);	         
	           /*newsym->entry.Template.base_name = oldsym;*//*19.03.03*/
	  	/* l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, oldsym);*/
		 m = make_llnd(fi, VAR_REF, LLNULL, LLNULL, newsym);
		 (yyval.ll_node) = make_llnd(fi, RENAME_NODE, m, LLNULL, oldsym);
 	      }
            ;}
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 2561 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 2563 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 2567 "gram1.y"
    {  PTR_HASH oldhash;
	       PTR_SYMB oldsym, newsym;
	       PTR_LLND l, m;

	       oldhash = just_look_up_sym_in_scope(module_scope, (yyvsp[(3) - (3)].hash_entry)->ident);
	       if (oldhash == HSNULL) {
                  errstr("Unknown identifier %s", (yyvsp[(3) - (3)].hash_entry)->ident,309);
	          (yyval.ll_node)= LLNULL;
	       }
	       else {
	         oldsym = oldhash->id_attr;
	         newsym = make_local_entity((yyvsp[(1) - (3)].hash_entry), oldsym->variant, oldsym->type,
LOCAL);
	         /* copies data in entry.Template structure and attr */
	         copy_sym_data(oldsym, newsym);	         
	           /*newsym->entry.Template.base_name = oldsym;*//*19.03.03*/
	  	 l = make_llnd(fi, VAR_REF, LLNULL, LLNULL, oldsym);
		 m = make_llnd(fi, VAR_REF, LLNULL, LLNULL, newsym);
		 (yyval.ll_node) = make_llnd(fi, RENAME_NODE, m, l, SMNULL);
 	      }
            ;}
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 2598 "gram1.y"
    { ndim = 0;	explicit_shape = 1; (yyval.ll_node) = LLNULL; ;}
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 2600 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node); ;}
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 2603 "gram1.y"
    { ndim = 0; explicit_shape = 1;;}
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 2604 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,EXPR_LIST, (yyvsp[(2) - (2)].ll_node), LLNULL, SMNULL);
	      (yyval.ll_node)->type = global_default;
	    ;}
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 2609 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 2613 "gram1.y"
    {
	      if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		(yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);
	      ++ndim;
	    ;}
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 2621 "gram1.y"
    {
	      if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		(yyval.ll_node) = make_llnd(fi, DDOT, LLNULL, LLNULL, SMNULL);
	      ++ndim;
              explicit_shape = 0;
	    ;}
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 2630 "gram1.y"
    {
	      if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		(yyval.ll_node) = make_llnd(fi,DDOT, (yyvsp[(1) - (2)].ll_node), LLNULL, SMNULL);
	      ++ndim;
              explicit_shape = 0;
	    ;}
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 2639 "gram1.y"
    {
	      if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		(yyval.ll_node) = make_llnd(fi,DDOT, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      ++ndim;
	    ;}
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 2649 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,STAR_RANGE, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->type = global_default;
              explicit_shape = 0;
	    ;}
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 2658 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 2660 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 323:

/* Line 1455 of yacc.c  */
#line 2664 "gram1.y"
    {PTR_LABEL p;
	     p = make_label_node(fi,convci(yyleng, yytext));
	     p->scope = cur_scope();
	     (yyval.ll_node) = make_llnd_label(fi,LABEL_REF, p);
	  ;}
    break;

  case 324:

/* Line 1455 of yacc.c  */
#line 2672 "gram1.y"
    { /*PTR_LLND l;*/

          /*   l = make_llnd(fi, EXPR_LIST, $3, LLNULL, SMNULL);*/
             (yyval.bf_node) = get_bfnd(fi,IMPL_DECL, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);
             redefine_func_arg_type();
           ;}
    break;

  case 325:

/* Line 1455 of yacc.c  */
#line 2687 "gram1.y"
    { /*undeftype = YES;
	    setimpl(TYNULL, (int)'a', (int)'z'); FB COMMENTED---> NOT QUITE RIGHT BUT AVOID PB WITH COMMON*/
	    (yyval.bf_node) = get_bfnd(fi,IMPL_DECL, SMNULL, LLNULL, LLNULL, LLNULL);
	  ;}
    break;

  case 326:

/* Line 1455 of yacc.c  */
#line 2694 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 2696 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 2700 "gram1.y"
    { 

            (yyval.ll_node) = make_llnd(fi, IMPL_TYPE, (yyvsp[(3) - (4)].ll_node), LLNULL, SMNULL);
            (yyval.ll_node)->type = vartype;
          ;}
    break;

  case 329:

/* Line 1455 of yacc.c  */
#line 2715 "gram1.y"
    { implkwd = YES; ;}
    break;

  case 330:

/* Line 1455 of yacc.c  */
#line 2716 "gram1.y"
    { vartype = (yyvsp[(2) - (2)].data_type); ;}
    break;

  case 331:

/* Line 1455 of yacc.c  */
#line 2720 "gram1.y"
    { (yyval.data_type) = (yyvsp[(2) - (2)].data_type); ;}
    break;

  case 332:

/* Line 1455 of yacc.c  */
#line 2722 "gram1.y"
    { (yyval.data_type) = (yyvsp[(1) - (1)].data_type);;}
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 2734 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 2736 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 2740 "gram1.y"
    {
	      setimpl(vartype, (int)(yyvsp[(1) - (1)].charv), (int)(yyvsp[(1) - (1)].charv));
	      (yyval.ll_node) = make_llnd(fi,CHAR_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.cval = (yyvsp[(1) - (1)].charv);
	    ;}
    break;

  case 336:

/* Line 1455 of yacc.c  */
#line 2746 "gram1.y"
    { PTR_LLND p,q;
	      
	      setimpl(vartype, (int)(yyvsp[(1) - (3)].charv), (int)(yyvsp[(3) - (3)].charv));
	      p = make_llnd(fi,CHAR_VAL, LLNULL, LLNULL, SMNULL);
	      p->entry.cval = (yyvsp[(1) - (3)].charv);
	      q = make_llnd(fi,CHAR_VAL, LLNULL, LLNULL, SMNULL);
	      q->entry.cval = (yyvsp[(3) - (3)].charv);
	      (yyval.ll_node)= make_llnd(fi,DDOT, p, q, SMNULL);
	    ;}
    break;

  case 337:

/* Line 1455 of yacc.c  */
#line 2758 "gram1.y"
    {
	      if(yyleng!=1 || yytext[0]<'a' || yytext[0]>'z')
		{
		  err("IMPLICIT item must be single letter", 37);
		  (yyval.charv) = '\0';
		}
	      else (yyval.charv) = yytext[0];
	    ;}
    break;

  case 338:

/* Line 1455 of yacc.c  */
#line 2769 "gram1.y"
    {
	      if (parstate == OUTSIDE)
	         { PTR_BFND p;

		   p = get_bfnd(fi,PROG_HEDR,
                                make_program(look_up_sym("_MAIN")),
                                LLNULL, LLNULL, LLNULL);
		   set_blobs(p, global_bfnd, NEW_GROUP1);
	           add_scope_level(p, NO);
		   position = IN_PROC; 
	  	   parstate = INSIDE;
                 }
	  
	    ;}
    break;

  case 339:

/* Line 1455 of yacc.c  */
#line 2786 "gram1.y"
    { switch(parstate)
		{
                case OUTSIDE:  
			{ PTR_BFND p;

			  p = get_bfnd(fi,PROG_HEDR,
                                       make_program(look_up_sym("_MAIN")),
                                       LLNULL, LLNULL, LLNULL);
			  set_blobs(p, global_bfnd, NEW_GROUP1);
			  add_scope_level(p, NO);
			  position = IN_PROC; 
	  		  parstate = INDCL; }
	                  break;
                case INSIDE:    parstate = INDCL;
                case INDCL:     break;

                case INDATA:
                         /*  err(
                     "Statement order error: declaration after DATA or function statement", 
                                 29);*/
                              break;

                default:
                           err("Declaration among executables", 30);
                }
        ;}
    break;

  case 342:

/* Line 1455 of yacc.c  */
#line 2824 "gram1.y"
    { (yyval.ll_node) = LLNULL; endioctl(); ;}
    break;

  case 343:

/* Line 1455 of yacc.c  */
#line 2826 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);  endioctl();;}
    break;

  case 344:

/* Line 1455 of yacc.c  */
#line 2830 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 345:

/* Line 1455 of yacc.c  */
#line 2832 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 346:

/* Line 1455 of yacc.c  */
#line 2834 "gram1.y"
    { PTR_LLND l;
	      l = make_llnd(fi, KEYWORD_ARG, (yyvsp[(1) - (2)].ll_node), (yyvsp[(2) - (2)].ll_node), SMNULL);
	      l->type = (yyvsp[(2) - (2)].ll_node)->type;
              (yyval.ll_node) = l; 
	    ;}
    break;

  case 347:

/* Line 1455 of yacc.c  */
#line 2845 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node), LLNULL, EXPR_LIST);
              endioctl(); 
            ;}
    break;

  case 348:

/* Line 1455 of yacc.c  */
#line 2849 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST);
              endioctl(); 
            ;}
    break;

  case 349:

/* Line 1455 of yacc.c  */
#line 2855 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 350:

/* Line 1455 of yacc.c  */
#line 2857 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 351:

/* Line 1455 of yacc.c  */
#line 2861 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 352:

/* Line 1455 of yacc.c  */
#line 2863 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node); ;}
    break;

  case 353:

/* Line 1455 of yacc.c  */
#line 2865 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 354:

/* Line 1455 of yacc.c  */
#line 2869 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 355:

/* Line 1455 of yacc.c  */
#line 2871 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 356:

/* Line 1455 of yacc.c  */
#line 2875 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 357:

/* Line 1455 of yacc.c  */
#line 2877 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("+", ADD_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 358:

/* Line 1455 of yacc.c  */
#line 2879 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("-", SUBT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 359:

/* Line 1455 of yacc.c  */
#line 2881 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("*", MULT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 360:

/* Line 1455 of yacc.c  */
#line 2883 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("/", DIV_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 361:

/* Line 1455 of yacc.c  */
#line 2885 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("**", EXP_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 362:

/* Line 1455 of yacc.c  */
#line 2887 "gram1.y"
    { (yyval.ll_node) = defined_op_node((yyvsp[(1) - (2)].hash_entry), (yyvsp[(2) - (2)].ll_node), LLNULL); ;}
    break;

  case 363:

/* Line 1455 of yacc.c  */
#line 2889 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("+", UNARY_ADD_OP, (yyvsp[(2) - (2)].ll_node), LLNULL); ;}
    break;

  case 364:

/* Line 1455 of yacc.c  */
#line 2891 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("-", MINUS_OP, (yyvsp[(2) - (2)].ll_node), LLNULL); ;}
    break;

  case 365:

/* Line 1455 of yacc.c  */
#line 2893 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".eq.", EQ_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 366:

/* Line 1455 of yacc.c  */
#line 2895 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".gt.", GT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 367:

/* Line 1455 of yacc.c  */
#line 2897 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".lt.", LT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 368:

/* Line 1455 of yacc.c  */
#line 2899 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".ge.", GTEQL_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 369:

/* Line 1455 of yacc.c  */
#line 2901 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".ge.", LTEQL_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 370:

/* Line 1455 of yacc.c  */
#line 2903 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".ne.", NOTEQL_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 371:

/* Line 1455 of yacc.c  */
#line 2905 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".eqv.", EQV_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 372:

/* Line 1455 of yacc.c  */
#line 2907 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".neqv.", NEQV_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 373:

/* Line 1455 of yacc.c  */
#line 2909 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".xor.", XOR_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 374:

/* Line 1455 of yacc.c  */
#line 2911 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".or.", OR_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 375:

/* Line 1455 of yacc.c  */
#line 2913 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".and.", AND_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 376:

/* Line 1455 of yacc.c  */
#line 2915 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node(".not.", NOT_OP, (yyvsp[(2) - (2)].ll_node), LLNULL); ;}
    break;

  case 377:

/* Line 1455 of yacc.c  */
#line 2917 "gram1.y"
    { (yyval.ll_node) = intrinsic_op_node("//", CONCAT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 378:

/* Line 1455 of yacc.c  */
#line 2919 "gram1.y"
    { (yyval.ll_node) = defined_op_node((yyvsp[(2) - (3)].hash_entry), (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node)); ;}
    break;

  case 379:

/* Line 1455 of yacc.c  */
#line 2922 "gram1.y"
    { (yyval.token) = ADD_OP; ;}
    break;

  case 380:

/* Line 1455 of yacc.c  */
#line 2923 "gram1.y"
    { (yyval.token) = SUBT_OP; ;}
    break;

  case 381:

/* Line 1455 of yacc.c  */
#line 2935 "gram1.y"
    { PTR_SYMB s;
	      PTR_TYPE t;
	     /* PTR_LLND l;*/

       	      if (!(s = (yyvsp[(1) - (1)].hash_entry)->id_attr))
              {
	         s = make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);
	     	 s->decl = SOFT;
	      } 
	
	      switch (s->variant)
              {
	      case CONST_NAME:
		   (yyval.ll_node) = make_llnd(fi,CONST_REF,LLNULL,LLNULL, s);
		   t = s->type;
	           if ((t != TYNULL) &&
                       ((t->variant == T_ARRAY) ||  (t->variant == T_STRING) ))
                                 (yyval.ll_node)->variant = ARRAY_REF;

                   (yyval.ll_node)->type = t;
	           break;
	      case DEFAULT:   /* if common region with same name has been
                                 declared. */
		   s = make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);
	     	   s->decl = SOFT;

	      case VARIABLE_NAME:
                   (yyval.ll_node) = make_llnd(fi,VAR_REF,LLNULL,LLNULL, s);
	           t = s->type;
	           if (t != TYNULL) {
                     if ((t->variant == T_ARRAY) ||  (t->variant == T_STRING) ||
                         ((t->variant == T_POINTER) && (t->entry.Template.base_type->variant == T_ARRAY) ) )
                         (yyval.ll_node)->variant = ARRAY_REF;

/*  	              if (t->variant == T_DERIVED_TYPE)
                         $$->variant = RECORD_REF; */
	           }
                   (yyval.ll_node)->type = t;
	           break;
	      case TYPE_NAME:
  	           (yyval.ll_node) = make_llnd(fi,TYPE_REF,LLNULL,LLNULL, s);
	           (yyval.ll_node)->type = s->type;
	           break;
	      case INTERFACE_NAME:
  	           (yyval.ll_node) = make_llnd(fi, INTERFACE_REF,LLNULL,LLNULL, s);
	           (yyval.ll_node)->type = s->type;
	           break;
              case FUNCTION_NAME:
                   if(isResultVar(s)) {
                     (yyval.ll_node) = make_llnd(fi,VAR_REF,LLNULL,LLNULL, s);
	             t = s->type;
	             if (t != TYNULL) {
                       if ((t->variant == T_ARRAY) ||  (t->variant == T_STRING) ||
                         ((t->variant == T_POINTER) && (t->entry.Template.base_type->variant == T_ARRAY) ) )
                         (yyval.ll_node)->variant = ARRAY_REF;
	             }
                     (yyval.ll_node)->type = t;
	             break;
                   }                                        
	      default:
  	           (yyval.ll_node) = make_llnd(fi,VAR_REF,LLNULL,LLNULL, s);
	           (yyval.ll_node)->type = s->type;
	           break;
	      }
             /* if ($$->variant == T_POINTER) {
	         l = $$;
	         $$ = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $$->type = l->type->entry.Template.base_type;
	      }
              */ /*11.02.03*/
           ;}
    break;

  case 382:

/* Line 1455 of yacc.c  */
#line 3009 "gram1.y"
    { PTR_SYMB  s;
	      (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); 
              s= (yyval.ll_node)->entry.Template.symbol;
              if ((((yyvsp[(1) - (1)].ll_node)->variant == VAR_REF) || ((yyvsp[(1) - (1)].ll_node)->variant == ARRAY_REF))  && (s->scope !=cur_scope()))  /*global_bfnd*/
              {
	          if(((s->variant == FUNCTION_NAME) && (!isResultVar(s))) || (s->variant == PROCEDURE_NAME) || (s->variant == ROUTINE_NAME))
                  { s = (yyval.ll_node)->entry.Template.symbol =  make_scalar(s->parent, TYNULL, LOCAL);
		    (yyval.ll_node)->type = s->type;  
		  }
              }
            ;}
    break;

  case 383:

/* Line 1455 of yacc.c  */
#line 3021 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 384:

/* Line 1455 of yacc.c  */
#line 3023 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 385:

/* Line 1455 of yacc.c  */
#line 3027 "gram1.y"
    { int num_triplets;
	      PTR_SYMB s;  /*, sym;*/
	      /* PTR_LLND l; */
	      PTR_TYPE tp;
	      /* l = $1; */
	      s = (yyvsp[(1) - (5)].ll_node)->entry.Template.symbol;
            
	      /* Handle variable to function conversion. */
	      if (((yyvsp[(1) - (5)].ll_node)->variant == VAR_REF) && 
	          (((s->variant == VARIABLE_NAME) && (s->type) &&
                    (s->type->variant != T_ARRAY)) ||
  	            (s->variant == ROUTINE_NAME))) {
	        s = (yyvsp[(1) - (5)].ll_node)->entry.Template.symbol =  make_function(s->parent, TYNULL, NO);
	        (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;
              }
	      if (((yyvsp[(1) - (5)].ll_node)->variant == VAR_REF) && (s->variant == FUNCTION_NAME)) { 
                if(isResultVar(s))
	          (yyvsp[(1) - (5)].ll_node)->variant = ARRAY_REF;
                else
                  (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;
              }
	      if (((yyvsp[(1) - (5)].ll_node)->variant == VAR_REF) && (s->variant == PROGRAM_NAME)) {
                 errstr("The name '%s' is invalid in this context",s->ident,285);
                 (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;
              }
              /* l = $1; */
	      num_triplets = is_array_section_ref((yyvsp[(4) - (5)].ll_node));
	      switch ((yyvsp[(1) - (5)].ll_node)->variant)
              {
	      case TYPE_REF:
                   (yyvsp[(1) - (5)].ll_node)->variant = STRUCTURE_CONSTRUCTOR;                  
                   (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
                   (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                   (yyval.ll_node)->type =  lookup_type(s->parent); 
	          /* $$ = make_llnd(fi, STRUCTURE_CONSTRUCTOR, $1, $4, SMNULL);
	           $$->type = $1->type;*//*18.02.03*/
	           break;
	      case INTERFACE_REF:
	       /*  sym = resolve_overloading(s, $4);
	           if (sym != SMNULL)
	  	   {
	              l = make_llnd(fi, FUNC_CALL, $4, LLNULL, sym);
	              l->type = sym->type;
	              $$ = $1; $$->variant = OVERLOADED_CALL;
	              $$->entry.Template.ll_ptr1 = l;
	              $$->type = sym->type;
	           }
	           else {
	             errstr("can't resolve call %s", s->ident,310);
	           }
	           break;
                 */ /*podd 01.02.03*/

                   (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;

	      case FUNC_CALL:
                   (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
                   (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                   if(s->type) 
                     (yyval.ll_node)->type = s->type;
                   else
                     (yyval.ll_node)->type = global_default;
	           late_bind_if_needed((yyval.ll_node));
	           break;
	      case DEREF_OP:
              case ARRAY_REF:
	           /* array element */
	           if (num_triplets == 0) {
                       if ((yyvsp[(4) - (5)].ll_node) == LLNULL) {
                           s = (yyvsp[(1) - (5)].ll_node)->entry.Template.symbol = make_function(s->parent, TYNULL, NO);
                           s->entry.func_decl.num_output = 1;
                           (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;
                           (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                       } else if ((yyvsp[(1) - (5)].ll_node)->type->variant == T_STRING) {
                           PTR_LLND temp = (yyvsp[(4) - (5)].ll_node);
                           int num_input = 0;

                           while (temp) {
                             ++num_input;
                             temp = temp->entry.Template.ll_ptr2;
                           }
                           (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
                           s = (yyvsp[(1) - (5)].ll_node)->entry.Template.symbol = make_function(s->parent, TYNULL, NO);
                           s->entry.func_decl.num_output = 1;
                           s->entry.func_decl.num_input = num_input;
                           (yyvsp[(1) - (5)].ll_node)->variant = FUNC_CALL;
                           (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                       } else {
       	                   (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
	                   (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                           (yyval.ll_node)->type = (yyvsp[(1) - (5)].ll_node)->type->entry.ar_decl.base_type;
                       }
                   }
                   /* substring */
	           else if ((num_triplets == 1) && 
                            ((yyvsp[(1) - (5)].ll_node)->type->variant == T_STRING)) {
    	           /*
                     $1->entry.Template.ll_ptr1 = $4;
	             $$ = $1; $$->type = global_string;
                   */
	                  (yyval.ll_node) = make_llnd(fi, 
			  ARRAY_OP, LLNULL, LLNULL, SMNULL);
    	                  (yyval.ll_node)->entry.Template.ll_ptr1 = (yyvsp[(1) - (5)].ll_node);
       	                  (yyval.ll_node)->entry.Template.ll_ptr2 = (yyvsp[(4) - (5)].ll_node)->entry.Template.ll_ptr1;
	                  (yyval.ll_node)->type = global_string;
                   }           
                   /* array section */
                   else {
    	             (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
	             (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node); tp = make_type(fi, T_ARRAY);     /**18.03.17*/
                     tp->entry.ar_decl.base_type = (yyvsp[(1) - (5)].ll_node)->type->entry.ar_decl.base_type; /**18.03.17 $1->type */
	             tp->entry.ar_decl.num_dimensions = num_triplets;
	             (yyval.ll_node)->type = tp;
                   }
	           break;
	      default:
                    if((yyvsp[(1) - (5)].ll_node)->entry.Template.symbol)
                      errstr("Can't subscript %s",(yyvsp[(1) - (5)].ll_node)->entry.Template.symbol->ident, 44);
                    else
	              err("Can't subscript",44);
             }
             /*if ($$->variant == T_POINTER) {
	        l = $$;
	        $$ = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	        $$->type = l->type->entry.Template.base_type;
	     }
              */  /*11.02.03*/

	     endioctl(); 
           ;}
    break;

  case 386:

/* Line 1455 of yacc.c  */
#line 3158 "gram1.y"
    { int num_triplets;
	      PTR_SYMB s;
	      PTR_LLND l;

	      s = (yyvsp[(1) - (6)].ll_node)->entry.Template.symbol;
/*              if ($1->type->variant == T_POINTER) {
	         l = $1;
	         $1 = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $1->type = l->type->entry.Template.base_type;
	      } */
	      if (((yyvsp[(1) - (6)].ll_node)->type->variant != T_ARRAY) ||
                  ((yyvsp[(1) - (6)].ll_node)->type->entry.ar_decl.base_type->variant != T_STRING)) {
	         errstr("Can't take substring of %s", s->ident, 45);
              }
              else {
  	        num_triplets = is_array_section_ref((yyvsp[(4) - (6)].ll_node));
	           /* array element */
                if (num_triplets == 0) {
                   (yyvsp[(1) - (6)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (6)].ll_node);
                  /* $1->entry.Template.ll_ptr2 = $6;*/
	          /* $$ = $1;*/
                   l=(yyvsp[(1) - (6)].ll_node);
                   /*$$->type = $1->type->entry.ar_decl.base_type;*/
                   l->type = global_string;  /**18.03.17* $1->type->entry.ar_decl.base_type;*/
                }
                /* array section */
                else {
    	           (yyvsp[(1) - (6)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (6)].ll_node);
    	           /*$1->entry.Template.ll_ptr2 = $6;
	           $$ = $1; $$->type = make_type(fi, T_ARRAY);
                   $$->type->entry.ar_decl.base_type = $1->type;
	           $$->type->entry.ar_decl.num_dimensions = num_triplets;
                  */
                   l = (yyvsp[(1) - (6)].ll_node); l->type = make_type(fi, T_ARRAY);
                   l->type->entry.ar_decl.base_type = global_string;   /**18.03.17* $1->type*/
	           l->type->entry.ar_decl.num_dimensions = num_triplets;
               }
                (yyval.ll_node) = make_llnd(fi, ARRAY_OP, l, (yyvsp[(6) - (6)].ll_node), SMNULL);
	        (yyval.ll_node)->type = l->type;
              
              /* if ($$->variant == T_POINTER) {
	          l = $$;
	          $$ = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	          $$->type = l->type->entry.Template.base_type;
	       }
               */  /*11.02.03*/
             }
             endioctl();
          ;}
    break;

  case 387:

/* Line 1455 of yacc.c  */
#line 3208 "gram1.y"
    {  int num_triplets;
	      PTR_LLND l,l1,l2;
              PTR_TYPE tp;

         /*   if ($1->variant == T_POINTER) {
	         l = $1;
	         $1 = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $1->type = l->type->entry.Template.base_type;
	      } */

              num_triplets = is_array_section_ref((yyvsp[(3) - (4)].ll_node));
              (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
              l2 = (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr2;  
              l1 = (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr1;                
              if(l2 && l2->type->variant == T_STRING)/*substring*/
                if(num_triplets == 1){
	           l = make_llnd(fi, ARRAY_OP, LLNULL, LLNULL, SMNULL);
    	           l->entry.Template.ll_ptr1 = l2;
       	           l->entry.Template.ll_ptr2 = (yyvsp[(3) - (4)].ll_node)->entry.Template.ll_ptr1;
	           l->type = global_string; 
                   (yyval.ll_node)->entry.Template.ll_ptr2 = l;                                          
                } else
                   err("Can't subscript",44);
              else if (l2 && l2->type->variant == T_ARRAY) {
                 if(num_triplets > 0) { /*array section*/
                   tp = make_type(fi,T_ARRAY);
                   tp->entry.ar_decl.base_type = (yyvsp[(1) - (4)].ll_node)->type->entry.ar_decl.base_type;
                   tp->entry.ar_decl.num_dimensions = num_triplets;
                   (yyval.ll_node)->type = tp;
                   l2->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
                   l2->type = (yyval.ll_node)->type;   
                  }                 
                 else {  /*array element*/
                   l2->type = l2->type->entry.ar_decl.base_type;
                   l2->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);   
                   if(l1->type->variant != T_ARRAY)  
                     (yyval.ll_node)->type = l2->type;
                 }
              } else 
                   {err("Can't subscript",44); /*fprintf(stderr,"%d  %d",$1->variant,l2);*/}
                   /*errstr("Can't subscript %s",l2->entry.Template.symbol->ident,441);*/
         ;}
    break;

  case 388:

/* Line 1455 of yacc.c  */
#line 3252 "gram1.y"
    { int num_triplets;
	      PTR_LLND l,q;

          /*     if ($1->variant == T_POINTER) {
	         l = $1;
	         $1 = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $1->type = l->type->entry.Template.base_type;
	      } */

              (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
	      if (((yyvsp[(1) - (5)].ll_node)->type->variant != T_ARRAY) &&
                  ((yyvsp[(1) - (5)].ll_node)->type->entry.ar_decl.base_type->variant != T_STRING)) {
	         err("Can't take substring",45);
              }
              else {
  	        num_triplets = is_array_section_ref((yyvsp[(3) - (5)].ll_node));
                l = (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr2;
                if(l) {
                /* array element */
	        if (num_triplets == 0) {
                   l->entry.Template.ll_ptr1 = (yyvsp[(3) - (5)].ll_node);       	           
                   l->type = global_string;
                }
                /* array section */
                else {	
    	             l->entry.Template.ll_ptr1 = (yyvsp[(3) - (5)].ll_node);
	             l->type = make_type(fi, T_ARRAY);
                     l->type->entry.ar_decl.base_type = global_string;
	             l->type->entry.ar_decl.num_dimensions = num_triplets;
                }
	        q = make_llnd(fi, ARRAY_OP, l, (yyvsp[(5) - (5)].ll_node), SMNULL);
	        q->type = l->type;
                (yyval.ll_node)->entry.Template.ll_ptr2 = q;
                if((yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1->type->variant != T_ARRAY)  
                     (yyval.ll_node)->type = q->type;
               }
             }
          ;}
    break;

  case 389:

/* Line 1455 of yacc.c  */
#line 3294 "gram1.y"
    { PTR_TYPE t;
	      PTR_SYMB  field;
	    /*  PTR_BFND at_scope;*/
              PTR_LLND l;


/*              if ($1->variant == T_POINTER) {
	         l = $1;
	         $1 = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $1->type = l->type->entry.Template.base_type;
	      } */

	      t = (yyvsp[(1) - (3)].ll_node)->type; 
	      
	      if (( ( ((yyvsp[(1) - (3)].ll_node)->variant == VAR_REF) 
	          ||  ((yyvsp[(1) - (3)].ll_node)->variant == CONST_REF) 
                  ||  ((yyvsp[(1) - (3)].ll_node)->variant == ARRAY_REF)
                  ||  ((yyvsp[(1) - (3)].ll_node)->variant == RECORD_REF)) && (t->variant == T_DERIVED_TYPE)) 
	          ||((((yyvsp[(1) - (3)].ll_node)->variant == ARRAY_REF) || ((yyvsp[(1) - (3)].ll_node)->variant == RECORD_REF)) && (t->variant == T_ARRAY) &&
                      (t = t->entry.ar_decl.base_type) && (t->variant == T_DERIVED_TYPE))) 
                {
                 t->name = lookup_type_symbol(t->name);
	         if ((field = component(t->name, yytext))) {                   
	            l =  make_llnd(fi, VAR_REF, LLNULL, LLNULL, field);
                    l->type = field->type;
                    if(field->type->variant == T_ARRAY || field->type->variant == T_STRING)
                      l->variant = ARRAY_REF; 
                    (yyval.ll_node) = make_llnd(fi, RECORD_REF, (yyvsp[(1) - (3)].ll_node), l, SMNULL);
                    if((yyvsp[(1) - (3)].ll_node)->type->variant != T_ARRAY)
                       (yyval.ll_node)->type = field->type;
                    else {
                       (yyval.ll_node)->type = make_type(fi,T_ARRAY);
                       if(field->type->variant != T_ARRAY) 
	                 (yyval.ll_node)->type->entry.ar_decl.base_type = field->type;
                       else
                         (yyval.ll_node)->type->entry.ar_decl.base_type = field->type->entry.ar_decl.base_type;
	               (yyval.ll_node)->type->entry.ar_decl.num_dimensions = t->entry.ar_decl.num_dimensions;
                       }
                 }
                  else  
                    errstr("Illegal component  %s", yytext,311);
              }                     
               else 
                    errstr("Can't take component  %s", yytext,311);
             ;}
    break;

  case 390:

/* Line 1455 of yacc.c  */
#line 3352 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 391:

/* Line 1455 of yacc.c  */
#line 3354 "gram1.y"
    {(yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 392:

/* Line 1455 of yacc.c  */
#line 3356 "gram1.y"
    {  int num_triplets;
               PTR_TYPE tp;
              /* PTR_LLND l;*/
	      if ((yyvsp[(1) - (5)].ll_node)->type->variant == T_ARRAY)
              {
  	         num_triplets = is_array_section_ref((yyvsp[(4) - (5)].ll_node));
	         /* array element */
	         if (num_triplets == 0) {
       	            (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
       	            (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
                    (yyval.ll_node)->type = (yyvsp[(1) - (5)].ll_node)->type->entry.ar_decl.base_type;
                 }
                 /* substring */
	       /*  else if ((num_triplets == 1) && 
                          ($1->type->variant == T_STRING)) {
    	                  $1->entry.Template.ll_ptr1 = $4;
	                  $$ = $1; $$->type = global_string;
                 }   */ /*podd*/        
                 /* array section */
                 else {
    	             (yyvsp[(1) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
	             (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node); tp = make_type(fi, T_ARRAY);
                     tp->entry.ar_decl.base_type = (yyvsp[(1) - (5)].ll_node)->type->entry.ar_decl.base_type;  /**18.03.17* $1->type */
	             tp->entry.ar_decl.num_dimensions = num_triplets;
                     (yyval.ll_node)->type = tp;
                 }
             } 
             else err("can't subscript",44);

            /* if ($$->variant == T_POINTER) {
	        l = $$;
	        $$ = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	        $$->type = l->type->entry.Template.base_type;
	     }
             */  /*11.02.03*/

            endioctl();
           ;}
    break;

  case 393:

/* Line 1455 of yacc.c  */
#line 3396 "gram1.y"
    {  int num_triplets;
	      PTR_LLND l,l1,l2;

         /*   if ($1->variant == T_POINTER) {
	         l = $1;
	         $1 = make_llnd(fi, DEREF_OP, l, LLNULL, SMNULL);
	         $1->type = l->type->entry.Template.base_type;
	      } */

              num_triplets = is_array_section_ref((yyvsp[(3) - (4)].ll_node));
              (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
              l2 = (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr2;  
              l1 = (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr1;                
              if(l2 && l2->type->variant == T_STRING)/*substring*/
                if(num_triplets == 1){
	           l = make_llnd(fi, ARRAY_OP, LLNULL, LLNULL, SMNULL);
    	           l->entry.Template.ll_ptr1 = l2;
       	           l->entry.Template.ll_ptr2 = (yyvsp[(3) - (4)].ll_node)->entry.Template.ll_ptr1;
	           l->type = global_string; 
                   (yyval.ll_node)->entry.Template.ll_ptr2 = l;                                          
                } else
                   err("Can't subscript",44);
              else if (l2 && l2->type->variant == T_ARRAY) {
                 if(num_triplets > 0) { /*array section*/
                   (yyval.ll_node)->type = make_type(fi,T_ARRAY);
                   (yyval.ll_node)->type->entry.ar_decl.base_type = l2->type->entry.ar_decl.base_type;
                   (yyval.ll_node)->type->entry.ar_decl.num_dimensions = num_triplets;
                   l2->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
                   l2->type = (yyval.ll_node)->type;   
                  }                 
                 else {  /*array element*/
                   l2->type = l2->type->entry.ar_decl.base_type;
                   l2->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);   
                   if(l1->type->variant != T_ARRAY)  
                     (yyval.ll_node)->type = l2->type;
                 }
              } else 
                   err("Can't subscript",44);
         ;}
    break;

  case 394:

/* Line 1455 of yacc.c  */
#line 3438 "gram1.y"
    { 
	      if ((yyvsp[(1) - (2)].ll_node)->type->variant == T_STRING) {
                 (yyvsp[(1) - (2)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(2) - (2)].ll_node);
                 (yyval.ll_node) = (yyvsp[(1) - (2)].ll_node); (yyval.ll_node)->type = global_string;
              }
              else errstr("can't subscript of %s", (yyvsp[(1) - (2)].ll_node)->entry.Template.symbol->ident,44);
            ;}
    break;

  case 395:

/* Line 1455 of yacc.c  */
#line 3448 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 396:

/* Line 1455 of yacc.c  */
#line 3450 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 397:

/* Line 1455 of yacc.c  */
#line 3454 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DDOT, (yyvsp[(2) - (5)].ll_node), (yyvsp[(4) - (5)].ll_node), SMNULL); ;}
    break;

  case 398:

/* Line 1455 of yacc.c  */
#line 3458 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 399:

/* Line 1455 of yacc.c  */
#line 3460 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 400:

/* Line 1455 of yacc.c  */
#line 3464 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 401:

/* Line 1455 of yacc.c  */
#line 3466 "gram1.y"
    { PTR_TYPE t;
               t = make_type_node((yyvsp[(1) - (3)].ll_node)->type, (yyvsp[(3) - (3)].ll_node));
               (yyval.ll_node) = (yyvsp[(1) - (3)].ll_node);
               (yyval.ll_node)->type = t;
             ;}
    break;

  case 402:

/* Line 1455 of yacc.c  */
#line 3472 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 403:

/* Line 1455 of yacc.c  */
#line 3474 "gram1.y"
    { PTR_TYPE t;
               t = make_type_node((yyvsp[(1) - (3)].ll_node)->type, (yyvsp[(3) - (3)].ll_node));
               (yyval.ll_node) = (yyvsp[(1) - (3)].ll_node);
               (yyval.ll_node)->type = t;
             ;}
    break;

  case 404:

/* Line 1455 of yacc.c  */
#line 3480 "gram1.y"
    {
              if ((yyvsp[(2) - (2)].ll_node) != LLNULL)
		 (yyval.ll_node) = make_llnd(fi, ARRAY_OP, (yyvsp[(1) - (2)].ll_node), (yyvsp[(2) - (2)].ll_node), SMNULL); 
	      else 
                 (yyval.ll_node) = (yyvsp[(1) - (2)].ll_node);
             ;}
    break;

  case 405:

/* Line 1455 of yacc.c  */
#line 3490 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,BOOL_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.bval = 1;
	      (yyval.ll_node)->type = global_bool;
	    ;}
    break;

  case 406:

/* Line 1455 of yacc.c  */
#line 3496 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,BOOL_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.bval = 0;
	      (yyval.ll_node)->type = global_bool;
	    ;}
    break;

  case 407:

/* Line 1455 of yacc.c  */
#line 3503 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,FLOAT_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.string_val = copys(yytext);
	      (yyval.ll_node)->type = global_float;
	    ;}
    break;

  case 408:

/* Line 1455 of yacc.c  */
#line 3509 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,DOUBLE_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.string_val = copys(yytext);
	      (yyval.ll_node)->type = global_double;
	    ;}
    break;

  case 409:

/* Line 1455 of yacc.c  */
#line 3517 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,INT_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.ival = atoi(yytext);
	      (yyval.ll_node)->type = global_int;
	    ;}
    break;

  case 410:

/* Line 1455 of yacc.c  */
#line 3525 "gram1.y"
    { PTR_TYPE t;
	      PTR_LLND p,q;
	      (yyval.ll_node) = make_llnd(fi,STRING_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.string_val = copys(yytext);
              if(yyquote=='\"') 
	        t = global_string_2;
              else
	        t = global_string;

	      p = make_llnd(fi,INT_VAL, LLNULL, LLNULL, SMNULL);
	      p->entry.ival = yyleng;
	      p->type = global_int;
              q = make_llnd(fi, LEN_OP, p, LLNULL, SMNULL); 
              (yyval.ll_node)->type = make_type_node(t, q);
	    ;}
    break;

  case 411:

/* Line 1455 of yacc.c  */
#line 3541 "gram1.y"
    { PTR_TYPE t;
	      (yyval.ll_node) = make_llnd(fi,STRING_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.string_val = copys(yytext);
              if(yyquote=='\"') 
	        t = global_string_2;
              else
	        t = global_string;
	      (yyval.ll_node)->type = make_type_node(t, (yyvsp[(1) - (3)].ll_node));
            ;}
    break;

  case 412:

/* Line 1455 of yacc.c  */
#line 3551 "gram1.y"
    { PTR_TYPE t;
	      (yyval.ll_node) = make_llnd(fi,STRING_VAL, LLNULL, LLNULL, SMNULL);
	      (yyval.ll_node)->entry.string_val = copys(yytext);
              if(yyquote=='\"') 
	        t = global_string_2;
              else
	        t = global_string;
	      (yyval.ll_node)->type = make_type_node(t, (yyvsp[(1) - (3)].ll_node));
            ;}
    break;

  case 413:

/* Line 1455 of yacc.c  */
#line 3564 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,COMPLEX_VAL, (yyvsp[(2) - (5)].ll_node), (yyvsp[(4) - (5)].ll_node), SMNULL);
	      (yyval.ll_node)->type = global_complex;
	    ;}
    break;

  case 414:

/* Line 1455 of yacc.c  */
#line 3571 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 415:

/* Line 1455 of yacc.c  */
#line 3573 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 416:

/* Line 1455 of yacc.c  */
#line 3596 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),SMNULL); ;}
    break;

  case 417:

/* Line 1455 of yacc.c  */
#line 3598 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (2)].ll_node),LLNULL,SMNULL); ;}
    break;

  case 418:

/* Line 1455 of yacc.c  */
#line 3600 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,make_llnd(fi,DDOT,(yyvsp[(1) - (5)].ll_node),(yyvsp[(3) - (5)].ll_node),SMNULL),(yyvsp[(5) - (5)].ll_node),SMNULL); ;}
    break;

  case 419:

/* Line 1455 of yacc.c  */
#line 3602 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,make_llnd(fi,DDOT,(yyvsp[(1) - (4)].ll_node),LLNULL,SMNULL),(yyvsp[(4) - (4)].ll_node),SMNULL); ;}
    break;

  case 420:

/* Line 1455 of yacc.c  */
#line 3604 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, make_llnd(fi,DDOT,LLNULL,(yyvsp[(2) - (4)].ll_node),SMNULL),(yyvsp[(4) - (4)].ll_node),SMNULL); ;}
    break;

  case 421:

/* Line 1455 of yacc.c  */
#line 3606 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,make_llnd(fi,DDOT,LLNULL,LLNULL,SMNULL),(yyvsp[(3) - (3)].ll_node),SMNULL); ;}
    break;

  case 422:

/* Line 1455 of yacc.c  */
#line 3608 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,LLNULL,(yyvsp[(2) - (2)].ll_node),SMNULL); ;}
    break;

  case 423:

/* Line 1455 of yacc.c  */
#line 3610 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,LLNULL,LLNULL,SMNULL); ;}
    break;

  case 424:

/* Line 1455 of yacc.c  */
#line 3613 "gram1.y"
    {in_vec=YES;;}
    break;

  case 425:

/* Line 1455 of yacc.c  */
#line 3613 "gram1.y"
    {in_vec=NO;;}
    break;

  case 426:

/* Line 1455 of yacc.c  */
#line 3614 "gram1.y"
    { PTR_TYPE array_type;
             (yyval.ll_node) = make_llnd (fi,CONSTRUCTOR_REF,(yyvsp[(4) - (6)].ll_node),LLNULL,SMNULL); 
             /*$$->type = $2->type;*/ /*28.02.03*/
             array_type = make_type(fi, T_ARRAY);
	     array_type->entry.ar_decl.num_dimensions = 1;
             if((yyvsp[(4) - (6)].ll_node)->type->variant == T_ARRAY)
	       array_type->entry.ar_decl.base_type = (yyvsp[(4) - (6)].ll_node)->type->entry.ar_decl.base_type;
             else
               array_type->entry.ar_decl.base_type = (yyvsp[(4) - (6)].ll_node)->type;
             (yyval.ll_node)->type = array_type;
           ;}
    break;

  case 427:

/* Line 1455 of yacc.c  */
#line 3628 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 428:

/* Line 1455 of yacc.c  */
#line 3630 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 429:

/* Line 1455 of yacc.c  */
#line 3653 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 430:

/* Line 1455 of yacc.c  */
#line 3655 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST); endioctl(); ;}
    break;

  case 431:

/* Line 1455 of yacc.c  */
#line 3657 "gram1.y"
    { stat_alloc = make_llnd(fi, SPEC_PAIR, (yyvsp[(4) - (5)].ll_node), (yyvsp[(5) - (5)].ll_node), SMNULL);
                  endioctl();
                ;}
    break;

  case 432:

/* Line 1455 of yacc.c  */
#line 3673 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 433:

/* Line 1455 of yacc.c  */
#line 3675 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST); endioctl(); ;}
    break;

  case 434:

/* Line 1455 of yacc.c  */
#line 3677 "gram1.y"
    { stat_alloc = make_llnd(fi, SPEC_PAIR, (yyvsp[(4) - (5)].ll_node), (yyvsp[(5) - (5)].ll_node), SMNULL);
             endioctl();
           ;}
    break;

  case 435:

/* Line 1455 of yacc.c  */
#line 3690 "gram1.y"
    {stat_alloc = LLNULL;;}
    break;

  case 436:

/* Line 1455 of yacc.c  */
#line 3694 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 437:

/* Line 1455 of yacc.c  */
#line 3696 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 438:

/* Line 1455 of yacc.c  */
#line 3704 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 439:

/* Line 1455 of yacc.c  */
#line 3706 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 440:

/* Line 1455 of yacc.c  */
#line 3708 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 441:

/* Line 1455 of yacc.c  */
#line 3710 "gram1.y"
    {
              (yyval.bf_node) = (yyvsp[(2) - (2)].bf_node);
              (yyval.bf_node)->entry.Template.ll_ptr3 = (yyvsp[(1) - (2)].ll_node);
            ;}
    break;

  case 442:

/* Line 1455 of yacc.c  */
#line 3764 "gram1.y"
    { PTR_BFND biff;

	      (yyval.bf_node) = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL); 
	      bind(); 
	      biff = cur_scope();
	      if ((biff->variant == FUNC_HEDR) || (biff->variant == PROC_HEDR)
		  || (biff->variant == PROS_HEDR) 
	          || (biff->variant == PROG_HEDR)
                  || (biff->variant == BLOCK_DATA)) {
                if(biff->control_parent == global_bfnd) position = IN_OUTSIDE;
		else if(!is_interface_stat(biff->control_parent)) position++;
              } else if  (biff->variant == MODULE_STMT)
                position = IN_OUTSIDE;
	      else err("Unexpected END statement read", 52);
             /* FB ADDED set the control parent so the empty function unparse right*/
              if ((yyval.bf_node))
                (yyval.bf_node)->control_parent = biff;
              delete_beyond_scope_level(pred_bfnd);
            ;}
    break;

  case 443:

/* Line 1455 of yacc.c  */
#line 3786 "gram1.y"
    {
              make_extend((yyvsp[(3) - (3)].symbol));
              (yyval.bf_node) = BFNULL; 
              /* delete_beyond_scope_level(pred_bfnd); */
             ;}
    break;

  case 444:

/* Line 1455 of yacc.c  */
#line 3799 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,CONTROL_END, SMNULL, LLNULL, LLNULL, LLNULL); 
	    bind(); 
	    delete_beyond_scope_level(pred_bfnd);
	    position = IN_OUTSIDE;
          ;}
    break;

  case 445:

/* Line 1455 of yacc.c  */
#line 3808 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 446:

/* Line 1455 of yacc.c  */
#line 3811 "gram1.y"
    {
              (yyval.bf_node) = (yyvsp[(2) - (2)].bf_node);
              (yyval.bf_node)->entry.Template.ll_ptr3 = (yyvsp[(1) - (2)].ll_node);
            ;}
    break;

  case 447:

/* Line 1455 of yacc.c  */
#line 3861 "gram1.y"
    { thiswasbranch = NO;
              (yyvsp[(1) - (2)].bf_node)->variant = LOGIF_NODE;
              (yyval.bf_node) = make_logif((yyvsp[(1) - (2)].bf_node), (yyvsp[(2) - (2)].bf_node));
	      set_blobs((yyvsp[(1) - (2)].bf_node), pred_bfnd, SAME_GROUP);
	    ;}
    break;

  case 448:

/* Line 1455 of yacc.c  */
#line 3867 "gram1.y"
    {
              (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node);
	      set_blobs((yyval.bf_node), pred_bfnd, NEW_GROUP1); 
            ;}
    break;

  case 449:

/* Line 1455 of yacc.c  */
#line 3872 "gram1.y"
    {
              (yyval.bf_node) = (yyvsp[(2) - (3)].bf_node);
              (yyval.bf_node)->entry.Template.ll_ptr3 = (yyvsp[(1) - (3)].ll_node);
	      set_blobs((yyval.bf_node), pred_bfnd, NEW_GROUP1); 
            ;}
    break;

  case 450:

/* Line 1455 of yacc.c  */
#line 3890 "gram1.y"
    { make_elseif((yyvsp[(4) - (7)].ll_node),(yyvsp[(7) - (7)].symbol)); lastwasbranch = NO; (yyval.bf_node) = BFNULL;;}
    break;

  case 451:

/* Line 1455 of yacc.c  */
#line 3892 "gram1.y"
    { make_else((yyvsp[(3) - (3)].symbol)); lastwasbranch = NO; (yyval.bf_node) = BFNULL; ;}
    break;

  case 452:

/* Line 1455 of yacc.c  */
#line 3894 "gram1.y"
    { make_endif((yyvsp[(3) - (3)].symbol)); (yyval.bf_node) = BFNULL; ;}
    break;

  case 453:

/* Line 1455 of yacc.c  */
#line 3896 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 454:

/* Line 1455 of yacc.c  */
#line 3898 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, CONTAINS_STMT, SMNULL, LLNULL, LLNULL, LLNULL); ;}
    break;

  case 455:

/* Line 1455 of yacc.c  */
#line 3901 "gram1.y"
    { thiswasbranch = NO;
              (yyvsp[(1) - (2)].bf_node)->variant = FORALL_STAT;
              (yyval.bf_node) = make_logif((yyvsp[(1) - (2)].bf_node), (yyvsp[(2) - (2)].bf_node));
	      set_blobs((yyvsp[(1) - (2)].bf_node), pred_bfnd, SAME_GROUP);
	    ;}
    break;

  case 456:

/* Line 1455 of yacc.c  */
#line 3907 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 457:

/* Line 1455 of yacc.c  */
#line 3909 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(2) - (2)].bf_node); (yyval.bf_node)->entry.Template.ll_ptr3 = (yyvsp[(1) - (2)].ll_node);;}
    break;

  case 458:

/* Line 1455 of yacc.c  */
#line 3911 "gram1.y"
    { make_endforall((yyvsp[(3) - (3)].symbol)); (yyval.bf_node) = BFNULL; ;}
    break;

  case 459:

/* Line 1455 of yacc.c  */
#line 3914 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 460:

/* Line 1455 of yacc.c  */
#line 3916 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 461:

/* Line 1455 of yacc.c  */
#line 3918 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 462:

/* Line 1455 of yacc.c  */
#line 3945 "gram1.y"
    { 	     
	     /*  if($5 && $5->labdefined)
		 execerr("no backward DO loops", (char *)NULL); */
	       (yyval.bf_node) = make_do(WHILE_NODE, LBNULL, SMNULL, (yyvsp[(4) - (5)].ll_node), LLNULL, LLNULL);
	       /*$$->entry.Template.ll_ptr3 = $1;*/	     
           ;}
    break;

  case 463:

/* Line 1455 of yacc.c  */
#line 3954 "gram1.y"
    {
               if( (yyvsp[(4) - (7)].label) && (yyvsp[(4) - (7)].label)->labdefined)
		  err("No backward DO loops", 46);
	        (yyval.bf_node) = make_do(WHILE_NODE, (yyvsp[(4) - (7)].label), SMNULL, (yyvsp[(7) - (7)].ll_node), LLNULL, LLNULL);            
	    ;}
    break;

  case 464:

/* Line 1455 of yacc.c  */
#line 3962 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 465:

/* Line 1455 of yacc.c  */
#line 3964 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(5) - (6)].ll_node);;}
    break;

  case 466:

/* Line 1455 of yacc.c  */
#line 3966 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (4)].ll_node);;}
    break;

  case 467:

/* Line 1455 of yacc.c  */
#line 3971 "gram1.y"
    {  
               if( (yyvsp[(4) - (11)].label) && (yyvsp[(4) - (11)].label)->labdefined)
		  err("No backward DO loops", 46);
	        (yyval.bf_node) = make_do(FOR_NODE, (yyvsp[(4) - (11)].label), (yyvsp[(7) - (11)].symbol), (yyvsp[(9) - (11)].ll_node), (yyvsp[(11) - (11)].ll_node), LLNULL);            
	    ;}
    break;

  case 468:

/* Line 1455 of yacc.c  */
#line 3978 "gram1.y"
    {
               if( (yyvsp[(4) - (13)].label) && (yyvsp[(4) - (13)].label)->labdefined)
		  err("No backward DO loops", 46);
	        (yyval.bf_node) = make_do(FOR_NODE, (yyvsp[(4) - (13)].label), (yyvsp[(7) - (13)].symbol), (yyvsp[(9) - (13)].ll_node), (yyvsp[(11) - (13)].ll_node), (yyvsp[(13) - (13)].ll_node));            
	    ;}
    break;

  case 469:

/* Line 1455 of yacc.c  */
#line 3986 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, CASE_NODE, (yyvsp[(4) - (4)].symbol), (yyvsp[(3) - (4)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 470:

/* Line 1455 of yacc.c  */
#line 3988 "gram1.y"
    { /*PTR_LLND p;*/
	     /* p = make_llnd(fi, DEFAULT, LLNULL, LLNULL, SMNULL); */
	      (yyval.bf_node) = get_bfnd(fi, DEFAULT_NODE, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL); ;}
    break;

  case 471:

/* Line 1455 of yacc.c  */
#line 3992 "gram1.y"
    { make_endselect((yyvsp[(3) - (3)].symbol)); (yyval.bf_node) = BFNULL; ;}
    break;

  case 472:

/* Line 1455 of yacc.c  */
#line 3995 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, SWITCH_NODE, SMNULL, (yyvsp[(6) - (7)].ll_node), LLNULL, LLNULL) ; ;}
    break;

  case 473:

/* Line 1455 of yacc.c  */
#line 3997 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, SWITCH_NODE, SMNULL, (yyvsp[(7) - (8)].ll_node), LLNULL, (yyvsp[(1) - (8)].ll_node)) ; ;}
    break;

  case 474:

/* Line 1455 of yacc.c  */
#line 4001 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node); ;}
    break;

  case 475:

/* Line 1455 of yacc.c  */
#line 4007 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 476:

/* Line 1455 of yacc.c  */
#line 4009 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DDOT, (yyvsp[(1) - (2)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 477:

/* Line 1455 of yacc.c  */
#line 4011 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DDOT, LLNULL, (yyvsp[(2) - (2)].ll_node), SMNULL); ;}
    break;

  case 478:

/* Line 1455 of yacc.c  */
#line 4013 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, DDOT, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL); ;}
    break;

  case 479:

/* Line 1455 of yacc.c  */
#line 4017 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, EXPR_LIST, (yyvsp[(1) - (1)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 480:

/* Line 1455 of yacc.c  */
#line 4019 "gram1.y"
    { PTR_LLND p;
	      
	      p = make_llnd(fi, EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(p, (yyvsp[(1) - (3)].ll_node));
	    ;}
    break;

  case 481:

/* Line 1455 of yacc.c  */
#line 4027 "gram1.y"
    { (yyval.symbol) = SMNULL; ;}
    break;

  case 482:

/* Line 1455 of yacc.c  */
#line 4029 "gram1.y"
    { (yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), CONSTRUCT_NAME, global_default,
                                     LOCAL); ;}
    break;

  case 483:

/* Line 1455 of yacc.c  */
#line 4035 "gram1.y"
    {(yyval.hash_entry) = HSNULL;;}
    break;

  case 484:

/* Line 1455 of yacc.c  */
#line 4037 "gram1.y"
    { (yyval.hash_entry) = (yyvsp[(1) - (1)].hash_entry);;}
    break;

  case 485:

/* Line 1455 of yacc.c  */
#line 4041 "gram1.y"
    {(yyval.hash_entry) = look_up_sym(yytext);;}
    break;

  case 486:

/* Line 1455 of yacc.c  */
#line 4045 "gram1.y"
    { PTR_SYMB s;
	             s = make_local_entity( (yyvsp[(1) - (2)].hash_entry), CONSTRUCT_NAME, global_default, LOCAL);             
                    (yyval.ll_node) = make_llnd(fi, VAR_REF, LLNULL, LLNULL, s);
                   ;}
    break;

  case 487:

/* Line 1455 of yacc.c  */
#line 4066 "gram1.y"
    { (yyval.bf_node) = make_if((yyvsp[(4) - (5)].ll_node)); ;}
    break;

  case 488:

/* Line 1455 of yacc.c  */
#line 4069 "gram1.y"
    { (yyval.bf_node) = make_forall((yyvsp[(4) - (6)].ll_node),(yyvsp[(5) - (6)].ll_node)); ;}
    break;

  case 489:

/* Line 1455 of yacc.c  */
#line 4073 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, EXPR_LIST, (yyvsp[(1) - (1)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 490:

/* Line 1455 of yacc.c  */
#line 4075 "gram1.y"
    { PTR_LLND p;	      
	      p = make_llnd(fi, EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(p, (yyvsp[(1) - (3)].ll_node));
	    ;}
    break;

  case 491:

/* Line 1455 of yacc.c  */
#line 4082 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi, FORALL_OP, (yyvsp[(3) - (3)].ll_node), LLNULL, (yyvsp[(1) - (3)].symbol)); ;}
    break;

  case 492:

/* Line 1455 of yacc.c  */
#line 4086 "gram1.y"
    { (yyval.ll_node)=LLNULL;;}
    break;

  case 493:

/* Line 1455 of yacc.c  */
#line 4088 "gram1.y"
    { (yyval.ll_node)=(yyvsp[(2) - (2)].ll_node);;}
    break;

  case 494:

/* Line 1455 of yacc.c  */
#line 4104 "gram1.y"
    { PTR_SYMB  s;
      	      if (!(s = (yyvsp[(1) - (1)].hash_entry)->id_attr))
              {
	         s = make_scalar((yyvsp[(1) - (1)].hash_entry), TYNULL, LOCAL);
	     	 s->decl = SOFT;
	      }
              (yyval.symbol) = s; 
	 ;}
    break;

  case 495:

/* Line 1455 of yacc.c  */
#line 4117 "gram1.y"
    { PTR_SYMB s;
              PTR_LLND l;
              int vrnt;

            /*  s = make_scalar($1, TYNULL, LOCAL);*/ /*16.02.03*/
              s = (yyvsp[(1) - (5)].symbol);
	      if (s->variant != CONST_NAME) {
                if(in_vec) 
                   vrnt=SEQ;
                else
                   vrnt=DDOT;     
                l = make_llnd(fi, SEQ, make_llnd(fi, vrnt, (yyvsp[(3) - (5)].ll_node), (yyvsp[(5) - (5)].ll_node), SMNULL),
                              LLNULL, SMNULL);
		(yyval.ll_node) = make_llnd(fi,IOACCESS, LLNULL, l, s);
		do_name_err = NO;
	      }
	      else {
		err("Symbolic constant not allowed as DO variable", 47);
		do_name_err = YES;
	      }
	    ;}
    break;

  case 496:

/* Line 1455 of yacc.c  */
#line 4140 "gram1.y"
    { PTR_SYMB s;
              PTR_LLND l;
              int vrnt;
              /*s = make_scalar($1, TYNULL, LOCAL);*/ /*16.02.03*/
              s = (yyvsp[(1) - (7)].symbol);
	      if( s->variant != CONST_NAME ) {
                if(in_vec) 
                   vrnt=SEQ;
                else
                   vrnt=DDOT;     
                l = make_llnd(fi, SEQ, make_llnd(fi, vrnt, (yyvsp[(3) - (7)].ll_node), (yyvsp[(5) - (7)].ll_node), SMNULL), (yyvsp[(7) - (7)].ll_node),
                              SMNULL);
		(yyval.ll_node) = make_llnd(fi,IOACCESS, LLNULL, l, s);
		do_name_err = NO;
	      }
	      else {
		err("Symbolic constant not allowed as DO variable", 47);
		do_name_err = YES;
	      }
	    ;}
    break;

  case 497:

/* Line 1455 of yacc.c  */
#line 4163 "gram1.y"
    { (yyval.label) = LBNULL; ;}
    break;

  case 498:

/* Line 1455 of yacc.c  */
#line 4165 "gram1.y"
    {
	       (yyval.label)  = make_label_node(fi,convci(yyleng, yytext));
	       (yyval.label)->scope = cur_scope();
	    ;}
    break;

  case 499:

/* Line 1455 of yacc.c  */
#line 4172 "gram1.y"
    { make_endwhere((yyvsp[(3) - (3)].symbol)); (yyval.bf_node) = BFNULL; ;}
    break;

  case 500:

/* Line 1455 of yacc.c  */
#line 4174 "gram1.y"
    { make_elsewhere((yyvsp[(3) - (3)].symbol)); lastwasbranch = NO; (yyval.bf_node) = BFNULL; ;}
    break;

  case 501:

/* Line 1455 of yacc.c  */
#line 4176 "gram1.y"
    { make_elsewhere_mask((yyvsp[(4) - (6)].ll_node),(yyvsp[(6) - (6)].symbol)); lastwasbranch = NO; (yyval.bf_node) = BFNULL; ;}
    break;

  case 502:

/* Line 1455 of yacc.c  */
#line 4178 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, WHERE_BLOCK_STMT, SMNULL, (yyvsp[(4) - (5)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 503:

/* Line 1455 of yacc.c  */
#line 4180 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, WHERE_BLOCK_STMT, SMNULL, (yyvsp[(5) - (6)].ll_node), LLNULL, (yyvsp[(1) - (6)].ll_node)); ;}
    break;

  case 504:

/* Line 1455 of yacc.c  */
#line 4185 "gram1.y"
    { PTR_LLND p, r;
             PTR_SYMB s1, s2 = SMNULL, s3, arg_list;
	     PTR_HASH hash_entry;

	   /*  if (just_look_up_sym("=") != HSNULL) {
	        p = intrinsic_op_node("=", EQUAL, $2, $4);
   	        $$ = get_bfnd(fi, OVERLOADED_ASSIGN_STAT, SMNULL, p, $2, $4);
             }	      
             else */ if ((yyvsp[(2) - (4)].ll_node)->variant == FUNC_CALL) {
                if(parstate==INEXEC){
                  	  err("Declaration among executables", 30);
                 /*   $$=BFNULL;*/
 	         (yyval.bf_node) = get_bfnd(fi,STMTFN_STAT, SMNULL, (yyvsp[(2) - (4)].ll_node), LLNULL, LLNULL);
                } 
                else {	         
  	         (yyvsp[(2) - (4)].ll_node)->variant = STMTFN_DECL;
		 /* $2->entry.Template.ll_ptr2 = $4; */
                 if( (yyvsp[(2) - (4)].ll_node)->entry.Template.ll_ptr1) {
		   r = (yyvsp[(2) - (4)].ll_node)->entry.Template.ll_ptr1->entry.Template.ll_ptr1;
                   if(r->variant != VAR_REF && r->variant != ARRAY_REF){
                     err("A dummy argument of a statement function must be a scalar identifier", 333);
                     s1 = SMNULL;
                   }
                   else                       
		     s1 = r ->entry.Template.symbol;
                 } else
                   s1 = SMNULL;
		 if (s1)
	            s1->scope = cur_scope();
 	         (yyval.bf_node) = get_bfnd(fi,STMTFN_STAT, SMNULL, (yyvsp[(2) - (4)].ll_node), LLNULL, LLNULL);
	         add_scope_level((yyval.bf_node), NO);
                 arg_list = SMNULL;
		 if (s1) 
                 {
	            /*arg_list = SMNULL;*/
                    p = (yyvsp[(2) - (4)].ll_node)->entry.Template.ll_ptr1;
                    while (p != LLNULL)
                    {
		    /*   if (p->entry.Template.ll_ptr1->variant != VAR_REF) {
			  errstr("cftn.gram:1: illegal statement function %s.", $2->entry.Template.symbol->ident);
			  break;
		       } 
                    */
                       r = p->entry.Template.ll_ptr1;
                       if(r->variant != VAR_REF && r->variant != ARRAY_REF){
                         err("A dummy argument of a statement function must be a scalar identifier", 333);
                         break;
                       }
	               hash_entry = look_up_sym(r->entry.Template.symbol->parent->ident);
	               s3 = make_scalar(hash_entry, s1->type, IO);
                       replace_symbol_in_expr(s3,(yyvsp[(4) - (4)].ll_node));
	               if (arg_list == SMNULL) 
                          s2 = arg_list = s3;
             	       else 
                       {
                          s2->id_list = s3;
                          s2 = s3;
                       }
                       p = p->entry.Template.ll_ptr2;
                    }
                 }
  		    (yyvsp[(2) - (4)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
		    install_param_list((yyvsp[(2) - (4)].ll_node)->entry.Template.symbol,
				       arg_list, LLNULL, FUNCTION_NAME);
	            delete_beyond_scope_level((yyval.bf_node));
		 
		/* else
		    errstr("cftn.gram: Illegal statement function declaration %s.", $2->entry.Template.symbol->ident); */
               }
	     }
	     else {
		(yyval.bf_node) = get_bfnd(fi,ASSIGN_STAT,SMNULL, (yyvsp[(2) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), LLNULL);
                 parstate = INEXEC;
             }
	  ;}
    break;

  case 505:

/* Line 1455 of yacc.c  */
#line 4261 "gram1.y"
    { /*PTR_SYMB s;*/
	
	      /*s = make_scalar($2, TYNULL, LOCAL);*/
  	      (yyval.bf_node) = get_bfnd(fi, POINTER_ASSIGN_STAT, SMNULL, (yyvsp[(3) - (5)].ll_node), (yyvsp[(5) - (5)].ll_node), LLNULL);
	    ;}
    break;

  case 506:

/* Line 1455 of yacc.c  */
#line 4273 "gram1.y"
    { PTR_SYMB p;

	      p = make_scalar((yyvsp[(5) - (5)].hash_entry), TYNULL, LOCAL);
	      p->variant = LABEL_VAR;
  	      (yyval.bf_node) = get_bfnd(fi,ASSLAB_STAT, p, (yyvsp[(3) - (5)].ll_node),LLNULL,LLNULL);
            ;}
    break;

  case 507:

/* Line 1455 of yacc.c  */
#line 4280 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,CONT_STAT,SMNULL,LLNULL,LLNULL,LLNULL); ;}
    break;

  case 509:

/* Line 1455 of yacc.c  */
#line 4283 "gram1.y"
    { inioctl = NO; ;}
    break;

  case 510:

/* Line 1455 of yacc.c  */
#line 4285 "gram1.y"
    { PTR_LLND	p;

	      p = make_llnd(fi,EXPR_LIST, (yyvsp[(10) - (10)].ll_node), LLNULL, SMNULL);
	      p = make_llnd(fi,EXPR_LIST, (yyvsp[(8) - (10)].ll_node), p, SMNULL);
	      (yyval.bf_node)= get_bfnd(fi,ARITHIF_NODE, SMNULL, (yyvsp[(4) - (10)].ll_node),
			    make_llnd(fi,EXPR_LIST, (yyvsp[(6) - (10)].ll_node), p, SMNULL), LLNULL);
	      thiswasbranch = YES;
            ;}
    break;

  case 511:

/* Line 1455 of yacc.c  */
#line 4294 "gram1.y"
    {
	      (yyval.bf_node) = subroutine_call((yyvsp[(1) - (1)].symbol), LLNULL, LLNULL, PLAIN);
/*	      match_parameters($1, LLNULL);
	      $$= get_bfnd(fi,PROC_STAT, $1, LLNULL, LLNULL, LLNULL);
*/	      endioctl(); 
            ;}
    break;

  case 512:

/* Line 1455 of yacc.c  */
#line 4301 "gram1.y"
    {
	      (yyval.bf_node) = subroutine_call((yyvsp[(1) - (3)].symbol), LLNULL, LLNULL, PLAIN);
/*	      match_parameters($1, LLNULL);
	      $$= get_bfnd(fi,PROC_STAT,$1,LLNULL,LLNULL,LLNULL);
*/	      endioctl(); 
	    ;}
    break;

  case 513:

/* Line 1455 of yacc.c  */
#line 4308 "gram1.y"
    {
	      (yyval.bf_node) = subroutine_call((yyvsp[(1) - (4)].symbol), (yyvsp[(3) - (4)].ll_node), LLNULL, PLAIN);
/*	      match_parameters($1, $3);
	      $$= get_bfnd(fi,PROC_STAT,$1,$3,LLNULL,LLNULL);
*/	      endioctl(); 
	    ;}
    break;

  case 514:

/* Line 1455 of yacc.c  */
#line 4316 "gram1.y"
    {
	      (yyval.bf_node) = get_bfnd(fi,RETURN_STAT,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);
	      thiswasbranch = YES;
	    ;}
    break;

  case 515:

/* Line 1455 of yacc.c  */
#line 4321 "gram1.y"
    {
	      (yyval.bf_node) = get_bfnd(fi,(yyvsp[(1) - (3)].token),SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);
	      thiswasbranch = ((yyvsp[(1) - (3)].token) == STOP_STAT);
	    ;}
    break;

  case 516:

/* Line 1455 of yacc.c  */
#line 4326 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, CYCLE_STMT, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL); ;}
    break;

  case 517:

/* Line 1455 of yacc.c  */
#line 4329 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, EXIT_STMT, (yyvsp[(3) - (3)].symbol), LLNULL, LLNULL, LLNULL); ;}
    break;

  case 518:

/* Line 1455 of yacc.c  */
#line 4332 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, ALLOCATE_STMT,  SMNULL, (yyvsp[(5) - (6)].ll_node), stat_alloc, LLNULL); ;}
    break;

  case 519:

/* Line 1455 of yacc.c  */
#line 4335 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, DEALLOCATE_STMT, SMNULL, (yyvsp[(5) - (6)].ll_node), stat_alloc , LLNULL); ;}
    break;

  case 520:

/* Line 1455 of yacc.c  */
#line 4338 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, NULLIFY_STMT, SMNULL, (yyvsp[(4) - (5)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 521:

/* Line 1455 of yacc.c  */
#line 4341 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi, WHERE_NODE, SMNULL, (yyvsp[(4) - (8)].ll_node), (yyvsp[(6) - (8)].ll_node), (yyvsp[(8) - (8)].ll_node)); ;}
    break;

  case 522:

/* Line 1455 of yacc.c  */
#line 4359 "gram1.y"
    {(yyval.ll_node) = LLNULL;;}
    break;

  case 523:

/* Line 1455 of yacc.c  */
#line 4363 "gram1.y"
    {
	      (yyval.bf_node)=get_bfnd(fi,GOTO_NODE,SMNULL,LLNULL,LLNULL,(PTR_LLND)(yyvsp[(3) - (3)].ll_node));
	      thiswasbranch = YES;
	    ;}
    break;

  case 524:

/* Line 1455 of yacc.c  */
#line 4368 "gram1.y"
    { PTR_SYMB p;

	      if((yyvsp[(3) - (3)].hash_entry)->id_attr)
		p = (yyvsp[(3) - (3)].hash_entry)->id_attr;
	      else {
	        p = make_scalar((yyvsp[(3) - (3)].hash_entry), TYNULL, LOCAL);
		p->variant = LABEL_VAR;
	      }

	      if(p->variant == LABEL_VAR) {
		  (yyval.bf_node) = get_bfnd(fi,ASSGOTO_NODE,p,LLNULL,LLNULL,LLNULL);
		  thiswasbranch = YES;
	      }
	      else {
		  err("Must go to assigned variable", 48);
		  (yyval.bf_node) = BFNULL;
	      }
	    ;}
    break;

  case 525:

/* Line 1455 of yacc.c  */
#line 4387 "gram1.y"
    { PTR_SYMB p;

	      if((yyvsp[(3) - (7)].hash_entry)->id_attr)
		p = (yyvsp[(3) - (7)].hash_entry)->id_attr;
	      else {
	        p = make_scalar((yyvsp[(3) - (7)].hash_entry), TYNULL, LOCAL);
		p->variant = LABEL_VAR;
	      }

	      if (p->variant == LABEL_VAR) {
		 (yyval.bf_node) = get_bfnd(fi,ASSGOTO_NODE,p,(yyvsp[(6) - (7)].ll_node),LLNULL,LLNULL);
		 thiswasbranch = YES;
	      }
	      else {
		err("Must go to assigned variable",48);
		(yyval.bf_node) = BFNULL;
	      }
	    ;}
    break;

  case 526:

/* Line 1455 of yacc.c  */
#line 4406 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,COMGOTO_NODE, SMNULL, (yyvsp[(4) - (7)].ll_node), (yyvsp[(7) - (7)].ll_node), LLNULL); ;}
    break;

  case 529:

/* Line 1455 of yacc.c  */
#line 4414 "gram1.y"
    { (yyval.symbol) = make_procedure((yyvsp[(3) - (4)].hash_entry), YES); ;}
    break;

  case 530:

/* Line 1455 of yacc.c  */
#line 4418 "gram1.y"
    { 
              (yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node), LLNULL, EXPR_LIST);
              endioctl();
            ;}
    break;

  case 531:

/* Line 1455 of yacc.c  */
#line 4423 "gram1.y"
    { 
               (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST);
               endioctl();
            ;}
    break;

  case 532:

/* Line 1455 of yacc.c  */
#line 4430 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 533:

/* Line 1455 of yacc.c  */
#line 4432 "gram1.y"
    { (yyval.ll_node)  = make_llnd(fi, KEYWORD_ARG, (yyvsp[(1) - (2)].ll_node), (yyvsp[(2) - (2)].ll_node), SMNULL); ;}
    break;

  case 534:

/* Line 1455 of yacc.c  */
#line 4434 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,LABEL_ARG,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL); ;}
    break;

  case 535:

/* Line 1455 of yacc.c  */
#line 4437 "gram1.y"
    { (yyval.token) = PAUSE_NODE; ;}
    break;

  case 536:

/* Line 1455 of yacc.c  */
#line 4438 "gram1.y"
    { (yyval.token) = STOP_STAT; ;}
    break;

  case 537:

/* Line 1455 of yacc.c  */
#line 4449 "gram1.y"
    { if(parstate == OUTSIDE)
		{ PTR_BFND p;

		  p = get_bfnd(fi,PROG_HEDR, make_program(look_up_sym("_MAIN")), LLNULL, LLNULL, LLNULL);
		  set_blobs(p, global_bfnd, NEW_GROUP1);
		  add_scope_level(p, NO);
		  position = IN_PROC; 
		}
		if(parstate < INDATA) enddcl();
		parstate = INEXEC;
		yystno = 0;
	      ;}
    break;

  case 538:

/* Line 1455 of yacc.c  */
#line 4464 "gram1.y"
    { intonly = YES; ;}
    break;

  case 539:

/* Line 1455 of yacc.c  */
#line 4468 "gram1.y"
    { intonly = NO; ;}
    break;

  case 540:

/* Line 1455 of yacc.c  */
#line 4476 "gram1.y"
    { (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (2)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 541:

/* Line 1455 of yacc.c  */
#line 4479 "gram1.y"
    { PTR_LLND p, q = LLNULL;

		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  q->entry.string_val = (char *)"unit";
		  q->type = global_string;
		  p = make_llnd(fi, SPEC_PAIR, q, (yyvsp[(2) - (2)].ll_node), SMNULL);
		  (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = p;
		  endioctl();
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 542:

/* Line 1455 of yacc.c  */
#line 4489 "gram1.y"
    { PTR_LLND p, q, r;

		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"*";
		  p->type = global_string;
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  q->entry.string_val = (char *)"unit";
		  q->type = global_string;
		  r = make_llnd(fi, SPEC_PAIR, p, q, SMNULL);
		  (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = r;
		  endioctl();
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 543:

/* Line 1455 of yacc.c  */
#line 4502 "gram1.y"
    { PTR_LLND p, q, r;

		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"**";
		  p->type = global_string;
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  q->entry.string_val = (char *)"unit";
		  q->type = global_string;
		  r = make_llnd(fi, SPEC_PAIR, p, q, SMNULL);
		  (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = r;
		  endioctl();
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 544:

/* Line 1455 of yacc.c  */
#line 4515 "gram1.y"
    { (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (2)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 545:

/* Line 1455 of yacc.c  */
#line 4518 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 546:

/* Line 1455 of yacc.c  */
#line 4520 "gram1.y"
    { (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (2)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 547:

/* Line 1455 of yacc.c  */
#line 4523 "gram1.y"
    { (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (2)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 548:

/* Line 1455 of yacc.c  */
#line 4526 "gram1.y"
    { (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (3)].ll_node);
		  (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (3)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (3)].bf_node); ;}
    break;

  case 549:

/* Line 1455 of yacc.c  */
#line 4530 "gram1.y"
    { (yyvsp[(1) - (4)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (4)].ll_node);
		  (yyvsp[(1) - (4)].bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (4)].bf_node); ;}
    break;

  case 550:

/* Line 1455 of yacc.c  */
#line 4539 "gram1.y"
    { (yyvsp[(1) - (2)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (2)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (2)].bf_node); ;}
    break;

  case 551:

/* Line 1455 of yacc.c  */
#line 4542 "gram1.y"
    { (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr2 = (yyvsp[(2) - (3)].ll_node);
		  (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (3)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (3)].bf_node); ;}
    break;

  case 552:

/* Line 1455 of yacc.c  */
#line 4546 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (1)].bf_node); ;}
    break;

  case 553:

/* Line 1455 of yacc.c  */
#line 4548 "gram1.y"
    { (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (3)].ll_node);
		  (yyval.bf_node) = (yyvsp[(1) - (3)].bf_node); ;}
    break;

  case 554:

/* Line 1455 of yacc.c  */
#line 4554 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (3)].bf_node); ;}
    break;

  case 555:

/* Line 1455 of yacc.c  */
#line 4558 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, BACKSPACE_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 556:

/* Line 1455 of yacc.c  */
#line 4560 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, REWIND_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 557:

/* Line 1455 of yacc.c  */
#line 4562 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, ENDFILE_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 558:

/* Line 1455 of yacc.c  */
#line 4569 "gram1.y"
    { (yyval.bf_node) = (yyvsp[(1) - (3)].bf_node); ;}
    break;

  case 559:

/* Line 1455 of yacc.c  */
#line 4573 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, OPEN_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 560:

/* Line 1455 of yacc.c  */
#line 4575 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, CLOSE_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 561:

/* Line 1455 of yacc.c  */
#line 4579 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi, INQUIRE_STAT, SMNULL, LLNULL, (yyvsp[(4) - (4)].ll_node), LLNULL);;}
    break;

  case 562:

/* Line 1455 of yacc.c  */
#line 4581 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi, INQUIRE_STAT, SMNULL, (yyvsp[(5) - (5)].ll_node), (yyvsp[(4) - (5)].ll_node), LLNULL);;}
    break;

  case 563:

/* Line 1455 of yacc.c  */
#line 4585 "gram1.y"
    { PTR_LLND p;
		  PTR_LLND q = LLNULL;

		  if ((yyvsp[(1) - (1)].ll_node)->variant == INT_VAL)
 	          {
		        PTR_LABEL r;

			r = make_label_node(fi, (long) (yyvsp[(1) - (1)].ll_node)->entry.ival);
			r->scope = cur_scope();
			p = make_llnd_label(fi, LABEL_REF, r);
		  }
		  else p = (yyvsp[(1) - (1)].ll_node); 
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  q->entry.string_val = (char *)"fmt";
		  q->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
		  endioctl();
		;}
    break;

  case 564:

/* Line 1455 of yacc.c  */
#line 4604 "gram1.y"
    { PTR_LLND p;
		  PTR_LLND q;

		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"*";
		  p->type = global_string;
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  q->entry.string_val = (char *)"fmt";
		  q->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
		  endioctl();
		;}
    break;

  case 565:

/* Line 1455 of yacc.c  */
#line 4620 "gram1.y"
    { PTR_LLND p;

		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"unit";
		  p->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, p, (yyvsp[(2) - (3)].ll_node), SMNULL);
		  endioctl();
		;}
    break;

  case 566:

/* Line 1455 of yacc.c  */
#line 4631 "gram1.y"
    { 
		  (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node);
		  endioctl();
		 ;}
    break;

  case 567:

/* Line 1455 of yacc.c  */
#line 4638 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); endioctl();;}
    break;

  case 568:

/* Line 1455 of yacc.c  */
#line 4640 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST); endioctl();;}
    break;

  case 569:

/* Line 1455 of yacc.c  */
#line 4644 "gram1.y"
    { PTR_LLND p;
		  PTR_LLND q;
 
		  nioctl++;
		  if ((nioctl == 2) && ((yyvsp[(1) - (1)].ll_node)->variant == INT_VAL))
 	          {
		        PTR_LABEL r;

			r = make_label_node(fi, (long) (yyvsp[(1) - (1)].ll_node)->entry.ival);
			r->scope = cur_scope();
			p = make_llnd_label(fi, LABEL_REF, r);
		  }
		  else p = (yyvsp[(1) - (1)].ll_node); 
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  if (nioctl == 1)
		        q->entry.string_val = (char *)"unit"; 
		  else {
                     if(((yyvsp[(1) - (1)].ll_node)->variant == VAR_REF) && (yyvsp[(1) - (1)].ll_node)->entry.Template.symbol->variant == NAMELIST_NAME)
                       q->entry.string_val = (char *)"nml";
                     else
                       q->entry.string_val = (char *)"fmt";
                  }
		  q->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
		;}
    break;

  case 570:

/* Line 1455 of yacc.c  */
#line 4670 "gram1.y"
    { PTR_LLND p;
		  PTR_LLND q;

		  nioctl++;
		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"*";
		  p->type = global_string;
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  if (nioctl == 1)
		        q->entry.string_val = (char *)"unit"; 
		  else  q->entry.string_val = (char *)"fmt";
		  q->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
		;}
    break;

  case 571:

/* Line 1455 of yacc.c  */
#line 4685 "gram1.y"
    { PTR_LLND p;
		  PTR_LLND q;

		  nioctl++;
		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"**";
		  p->type = global_string;
		  q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  if (nioctl == 1)
		        q->entry.string_val = (char *)"unit"; 
		  else  q->entry.string_val = (char *)"fmt";
		  q->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
		;}
    break;

  case 572:

/* Line 1455 of yacc.c  */
#line 4700 "gram1.y"
    { 
		  PTR_LLND p;
		  char *q;

		  q = (yyvsp[(1) - (2)].ll_node)->entry.string_val;
  		  if ((strcmp(q, "end") == 0) || (strcmp(q, "err") == 0) || (strcmp(q, "eor") == 0) || ((strcmp(q,"fmt") == 0) && ((yyvsp[(2) - (2)].ll_node)->variant == INT_VAL)))
 	          {
		        PTR_LABEL r;

			r = make_label_node(fi, (long) (yyvsp[(2) - (2)].ll_node)->entry.ival);
			r->scope = cur_scope();
			p = make_llnd_label(fi, LABEL_REF, r);
		  }
		  else p = (yyvsp[(2) - (2)].ll_node);

		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, (yyvsp[(1) - (2)].ll_node), p, SMNULL); ;}
    break;

  case 573:

/* Line 1455 of yacc.c  */
#line 4717 "gram1.y"
    { PTR_LLND p;
                  
		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"*";
		  p->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, (yyvsp[(1) - (2)].ll_node), p, SMNULL);
		;}
    break;

  case 574:

/* Line 1455 of yacc.c  */
#line 4725 "gram1.y"
    { PTR_LLND p;
		  p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  p->entry.string_val = (char *)"*";
		  p->type = global_string;
		  (yyval.ll_node) = make_llnd(fi, SPEC_PAIR, (yyvsp[(1) - (2)].ll_node), p, SMNULL);
		;}
    break;

  case 575:

/* Line 1455 of yacc.c  */
#line 4734 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
		  (yyval.ll_node)->entry.string_val = copys(yytext);
		  (yyval.ll_node)->type = global_string;
	        ;}
    break;

  case 576:

/* Line 1455 of yacc.c  */
#line 4742 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, READ_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 577:

/* Line 1455 of yacc.c  */
#line 4747 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi, WRITE_STAT, SMNULL, LLNULL, LLNULL, LLNULL);;}
    break;

  case 578:

/* Line 1455 of yacc.c  */
#line 4752 "gram1.y"
    {
	    PTR_LLND p, q, l;

	    if ((yyvsp[(3) - (4)].ll_node)->variant == INT_VAL)
		{
		        PTR_LABEL r;

			r = make_label_node(fi, (long) (yyvsp[(3) - (4)].ll_node)->entry.ival);
			r->scope = cur_scope();
			p = make_llnd_label(fi, LABEL_REF, r);
		}
	    else p = (yyvsp[(3) - (4)].ll_node);
	    
            q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
	    q->entry.string_val = (char *)"fmt";
            q->type = global_string;
            l = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);

            (yyval.bf_node) = get_bfnd(fi, PRINT_STAT, SMNULL, LLNULL, l, LLNULL);
	    endioctl();
	   ;}
    break;

  case 579:

/* Line 1455 of yacc.c  */
#line 4774 "gram1.y"
    { PTR_LLND p, q, r;
		
	     p = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
	     p->entry.string_val = (char *)"*";
	     p->type = global_string;
	     q = make_llnd(fi, KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
	     q->entry.string_val = (char *)"fmt";
             q->type = global_string;
             r = make_llnd(fi, SPEC_PAIR, q, p, SMNULL);
	     (yyval.bf_node) = get_bfnd(fi, PRINT_STAT, SMNULL, LLNULL, r, LLNULL);
	     endioctl();
           ;}
    break;

  case 580:

/* Line 1455 of yacc.c  */
#line 4790 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST);;}
    break;

  case 581:

/* Line 1455 of yacc.c  */
#line 4792 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST);;}
    break;

  case 582:

/* Line 1455 of yacc.c  */
#line 4796 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 583:

/* Line 1455 of yacc.c  */
#line 4798 "gram1.y"
    {
		  (yyvsp[(4) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(2) - (5)].ll_node);
		  (yyval.ll_node) = (yyvsp[(4) - (5)].ll_node);
		;}
    break;

  case 584:

/* Line 1455 of yacc.c  */
#line 4805 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST);  (yyval.ll_node)->type = (yyvsp[(1) - (1)].ll_node)->type;;}
    break;

  case 585:

/* Line 1455 of yacc.c  */
#line 4807 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 586:

/* Line 1455 of yacc.c  */
#line 4809 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 587:

/* Line 1455 of yacc.c  */
#line 4813 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 588:

/* Line 1455 of yacc.c  */
#line 4815 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 589:

/* Line 1455 of yacc.c  */
#line 4817 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 590:

/* Line 1455 of yacc.c  */
#line 4819 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 591:

/* Line 1455 of yacc.c  */
#line 4821 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 592:

/* Line 1455 of yacc.c  */
#line 4823 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); (yyval.ll_node)->type = (yyvsp[(1) - (3)].ll_node)->type;;}
    break;

  case 593:

/* Line 1455 of yacc.c  */
#line 4827 "gram1.y"
    { (yyval.ll_node) =  set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST);
	          (yyval.ll_node)->type = global_complex; ;}
    break;

  case 594:

/* Line 1455 of yacc.c  */
#line 4830 "gram1.y"
    { (yyval.ll_node) =  set_ll_list((yyvsp[(2) - (3)].ll_node), LLNULL, EXPR_LIST);
                  (yyval.ll_node)->type = (yyvsp[(2) - (3)].ll_node)->type; ;}
    break;

  case 595:

/* Line 1455 of yacc.c  */
#line 4833 "gram1.y"
    {
		  (yyvsp[(4) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(2) - (5)].ll_node);
		  (yyval.ll_node) =  set_ll_list((yyvsp[(4) - (5)].ll_node), LLNULL, EXPR_LIST);
                  (yyval.ll_node)->type = (yyvsp[(2) - (5)].ll_node)->type; 
		;}
    break;

  case 596:

/* Line 1455 of yacc.c  */
#line 4839 "gram1.y"
    {
		  (yyvsp[(4) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(2) - (5)].ll_node);
		  (yyval.ll_node) =  set_ll_list((yyvsp[(4) - (5)].ll_node), LLNULL, EXPR_LIST);
                  (yyval.ll_node)->type = (yyvsp[(2) - (5)].ll_node)->type; 
		;}
    break;

  case 597:

/* Line 1455 of yacc.c  */
#line 4845 "gram1.y"
    {
		  (yyvsp[(4) - (5)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(2) - (5)].ll_node);
		  (yyval.ll_node) =  set_ll_list((yyvsp[(4) - (5)].ll_node), LLNULL, EXPR_LIST);
                  (yyval.ll_node)->type = (yyvsp[(2) - (5)].ll_node)->type; 
		;}
    break;

  case 598:

/* Line 1455 of yacc.c  */
#line 4853 "gram1.y"
    { inioctl = YES; ;}
    break;

  case 599:

/* Line 1455 of yacc.c  */
#line 4857 "gram1.y"
    { startioctl();;}
    break;

  case 600:

/* Line 1455 of yacc.c  */
#line 4865 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 601:

/* Line 1455 of yacc.c  */
#line 4867 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node); ;}
    break;

  case 602:

/* Line 1455 of yacc.c  */
#line 4871 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 603:

/* Line 1455 of yacc.c  */
#line 4873 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 604:

/* Line 1455 of yacc.c  */
#line 4875 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,(yyvsp[(2) - (3)].token), (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      set_expr_type((yyval.ll_node));
	    ;}
    break;

  case 605:

/* Line 1455 of yacc.c  */
#line 4880 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,MULT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      set_expr_type((yyval.ll_node));
	    ;}
    break;

  case 606:

/* Line 1455 of yacc.c  */
#line 4885 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,DIV_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      set_expr_type((yyval.ll_node));
	    ;}
    break;

  case 607:

/* Line 1455 of yacc.c  */
#line 4890 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,EXP_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      set_expr_type((yyval.ll_node));
	    ;}
    break;

  case 608:

/* Line 1455 of yacc.c  */
#line 4895 "gram1.y"
    {
	      if((yyvsp[(1) - (2)].token) == SUBT_OP)
		{
		  (yyval.ll_node) = make_llnd(fi,SUBT_OP, (yyvsp[(2) - (2)].ll_node), LLNULL, SMNULL);
		  set_expr_type((yyval.ll_node));
		}
	      else	(yyval.ll_node) = (yyvsp[(2) - (2)].ll_node);
	    ;}
    break;

  case 609:

/* Line 1455 of yacc.c  */
#line 4904 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,CONCAT_OP, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);
	      set_expr_type((yyval.ll_node));
	    ;}
    break;

  case 610:

/* Line 1455 of yacc.c  */
#line 4909 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 611:

/* Line 1455 of yacc.c  */
#line 4914 "gram1.y"
    { comments = cur_comment = CMNULL; ;}
    break;

  case 612:

/* Line 1455 of yacc.c  */
#line 4916 "gram1.y"
    { PTR_CMNT p;
	    p = make_comment(fi,*commentbuf, HALF);
	    if (cur_comment)
               cur_comment->next = p;
            else {
	       if ((pred_bfnd->control_parent->variant == LOGIF_NODE) ||(pred_bfnd->control_parent->variant == FORALL_STAT))

	           pred_bfnd->control_parent->entry.Template.cmnt_ptr = p;

	       else last_bfnd->entry.Template.cmnt_ptr = p;
            }
	    comments = cur_comment = CMNULL;
          ;}
    break;

  case 676:

/* Line 1455 of yacc.c  */
#line 4999 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,HPF_TEMPLATE_STAT, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL); ;}
    break;

  case 677:

/* Line 1455 of yacc.c  */
#line 5001 "gram1.y"
    { PTR_SYMB s;
                if((yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr2)
                {
                  s = (yyvsp[(3) - (3)].ll_node)->entry.Template.ll_ptr1->entry.Template.symbol;
                  s->attr = s->attr | COMMON_BIT;
                }
	        add_to_lowLevelList((yyvsp[(3) - (3)].ll_node), (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	      ;}
    break;

  case 678:

/* Line 1455 of yacc.c  */
#line 5012 "gram1.y"
    {PTR_SYMB s;
	      PTR_LLND q;
	    /* 27.06.18
	      if(! explicit_shape)   
                err("Explicit shape specification is required", 50);
	    */  
	      s = make_array((yyvsp[(1) - (2)].hash_entry), TYNULL, (yyvsp[(2) - (2)].ll_node), ndim, LOCAL);
              if(s->attr & TEMPLATE_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & PROCESSORS_BIT) || (s->attr & TASK_BIT) || (s->attr & DVM_POINTER_BIT))
                errstr( "Inconsistent declaration of identifier  %s ", s->ident, 16);
              else
	        s->attr = s->attr | TEMPLATE_BIT;
              if((yyvsp[(2) - (2)].ll_node)) s->attr = s->attr | DIMENSION_BIT;  
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(2) - (2)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(2) - (2)].ll_node);
	      (yyval.ll_node) = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	     ;}
    break;

  case 679:

/* Line 1455 of yacc.c  */
#line 5033 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_DYNAMIC_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 680:

/* Line 1455 of yacc.c  */
#line 5037 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 681:

/* Line 1455 of yacc.c  */
#line 5039 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 682:

/* Line 1455 of yacc.c  */
#line 5043 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              if(s->attr &  DYNAMIC_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & PROCESSORS_BIT) || (s->attr & TASK_BIT) || (s->attr & HEAP_BIT)) 
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16); 
              else
                s->attr = s->attr | DYNAMIC_BIT;        
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	   ;}
    break;

  case 683:

/* Line 1455 of yacc.c  */
#line 5056 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_INHERIT_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 684:

/* Line 1455 of yacc.c  */
#line 5060 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 685:

/* Line 1455 of yacc.c  */
#line 5062 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 686:

/* Line 1455 of yacc.c  */
#line 5066 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              if((s->attr & PROCESSORS_BIT) ||(s->attr & TASK_BIT)  || (s->attr & TEMPLATE_BIT) || (s->attr & ALIGN_BIT) || (s->attr & DISTRIBUTE_BIT)) 
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16); 
              else
                s->attr = s->attr | INHERIT_BIT;        
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	   ;}
    break;

  case 687:

/* Line 1455 of yacc.c  */
#line 5077 "gram1.y"
    { PTR_LLND q;
             q = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
              /* (void)fprintf(stderr,"hpf.gram: shadow\n");*/ 
	     (yyval.bf_node) = get_bfnd(fi,DVM_SHADOW_DIR,SMNULL,q,(yyvsp[(4) - (4)].ll_node),LLNULL);
            ;}
    break;

  case 688:

/* Line 1455 of yacc.c  */
#line 5088 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node);;}
    break;

  case 689:

/* Line 1455 of yacc.c  */
#line 5092 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 690:

/* Line 1455 of yacc.c  */
#line 5094 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 691:

/* Line 1455 of yacc.c  */
#line 5098 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 692:

/* Line 1455 of yacc.c  */
#line 5100 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);;}
    break;

  case 693:

/* Line 1455 of yacc.c  */
#line 5102 "gram1.y"
    {
            if(parstate!=INEXEC) 
               err("Illegal shadow width specification", 56);  
            (yyval.ll_node) = make_llnd(fi,SHADOW_NAMES_OP, (yyvsp[(3) - (4)].ll_node), LLNULL, SMNULL);
          ;}
    break;

  case 694:

/* Line 1455 of yacc.c  */
#line 5117 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              if(s->attr & SHADOW_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & PROCESSORS_BIT) ||(s->attr & TASK_BIT)  || (s->attr & TEMPLATE_BIT) || (s->attr & HEAP_BIT)) 
                      errstr( "Inconsistent declaration of identifier %s", s->ident, 16); 
              else
        	      s->attr = s->attr | SHADOW_BIT;  
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	   ;}
    break;

  case 695:

/* Line 1455 of yacc.c  */
#line 5129 "gram1.y"
    { PTR_SYMB s;
	      PTR_LLND q, r;
	      if(! explicit_shape) {
              err("Explicit shape specification is required", 50);
		/* $$ = BFNULL;*/
	      }
	      s = make_array((yyvsp[(3) - (4)].hash_entry), TYNULL, (yyvsp[(4) - (4)].ll_node), ndim, LOCAL);
              if(s->attr &  PROCESSORS_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT) || (s->attr & TASK_BIT) || (s->attr & DVM_POINTER_BIT) || (s->attr & INHERIT_BIT))
                errstr("Inconsistent declaration of identifier %s", s->ident, 16);
              else
	        s->attr = s->attr | PROCESSORS_BIT;
              if((yyvsp[(4) - (4)].ll_node)) s->attr = s->attr | DIMENSION_BIT;
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(4) - (4)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(4) - (4)].ll_node);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,HPF_PROCESSORS_STAT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 696:

/* Line 1455 of yacc.c  */
#line 5149 "gram1.y"
    { PTR_SYMB s;
	      PTR_LLND q, r;
	      if(! explicit_shape) {
              err("Explicit shape specification is required", 50);
		/* $$ = BFNULL;*/
	      }
	      s = make_array((yyvsp[(3) - (4)].hash_entry), TYNULL, (yyvsp[(4) - (4)].ll_node), ndim, LOCAL);
              if(s->attr &  PROCESSORS_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT) || (s->attr & TASK_BIT) || (s->attr & DVM_POINTER_BIT) || (s->attr & INHERIT_BIT))
                errstr("Inconsistent declaration of identifier %s", s->ident, 16);
              else
	        s->attr = s->attr | PROCESSORS_BIT;
              if((yyvsp[(4) - (4)].ll_node)) s->attr = s->attr | DIMENSION_BIT;
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(4) - (4)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(4) - (4)].ll_node);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      (yyval.bf_node) = get_bfnd(fi,HPF_PROCESSORS_STAT, SMNULL, r, LLNULL, LLNULL);
	    ;}
    break;

  case 697:

/* Line 1455 of yacc.c  */
#line 5169 "gram1.y"
    {  PTR_SYMB s;
	      PTR_LLND q, r;
	      if(! explicit_shape) {
		err("Explicit shape specification is required", 50);
		/*$$ = BFNULL;*/
	      }
	      s = make_array((yyvsp[(3) - (4)].hash_entry), TYNULL, (yyvsp[(4) - (4)].ll_node), ndim, LOCAL);
              if(s->attr &  PROCESSORS_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT) || (s->attr & TASK_BIT) || (s->attr &  DVM_POINTER_BIT) || (s->attr & INHERIT_BIT) )
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16);
              else
	        s->attr = s->attr | PROCESSORS_BIT;
              if((yyvsp[(4) - (4)].ll_node)) s->attr = s->attr | DIMENSION_BIT;
	      q = make_llnd(fi,ARRAY_REF, (yyvsp[(4) - (4)].ll_node), LLNULL, s);
	      s->type->entry.ar_decl.ranges = (yyvsp[(4) - (4)].ll_node);
	      r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	      add_to_lowLevelList(r, (yyvsp[(1) - (4)].bf_node)->entry.Template.ll_ptr1);
	;}
    break;

  case 698:

/* Line 1455 of yacc.c  */
#line 5191 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	           (yyval.bf_node) = get_bfnd(fi,DVM_INDIRECT_GROUP_DIR, SMNULL, r, LLNULL, LLNULL);
                ;}
    break;

  case 699:

/* Line 1455 of yacc.c  */
#line 5197 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                   add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	           ;
                ;}
    break;

  case 700:

/* Line 1455 of yacc.c  */
#line 5206 "gram1.y"
    {(yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), REF_GROUP_NAME,global_default,LOCAL);
          if((yyval.symbol)->attr &  INDIRECT_BIT)
                errstr( "Multiple declaration of identifier  %s ", (yyval.symbol)->ident, 73);
           (yyval.symbol)->attr = (yyval.symbol)->attr | INDIRECT_BIT;
          ;}
    break;

  case 701:

/* Line 1455 of yacc.c  */
#line 5214 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	           (yyval.bf_node) = get_bfnd(fi,DVM_REMOTE_GROUP_DIR, SMNULL, r, LLNULL, LLNULL);
                ;}
    break;

  case 702:

/* Line 1455 of yacc.c  */
#line 5220 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                   add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
                ;}
    break;

  case 703:

/* Line 1455 of yacc.c  */
#line 5228 "gram1.y"
    {(yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), REF_GROUP_NAME,global_default,LOCAL);
           if((yyval.symbol)->attr &  INDIRECT_BIT)
                errstr( "Inconsistent declaration of identifier  %s ", (yyval.symbol)->ident, 16);
          ;}
    break;

  case 704:

/* Line 1455 of yacc.c  */
#line 5235 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	           (yyval.bf_node) = get_bfnd(fi,DVM_REDUCTION_GROUP_DIR, SMNULL, r, LLNULL, LLNULL);
                ;}
    break;

  case 705:

/* Line 1455 of yacc.c  */
#line 5241 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                   add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
	           ;
                ;}
    break;

  case 706:

/* Line 1455 of yacc.c  */
#line 5250 "gram1.y"
    {(yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), REDUCTION_GROUP_NAME,global_default,LOCAL);;}
    break;

  case 707:

/* Line 1455 of yacc.c  */
#line 5254 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	           (yyval.bf_node) = get_bfnd(fi,DVM_CONSISTENT_GROUP_DIR, SMNULL, r, LLNULL, LLNULL);
                ;}
    break;

  case 708:

/* Line 1455 of yacc.c  */
#line 5260 "gram1.y"
    {  PTR_LLND q,r;
                   q = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(3) - (3)].symbol));
                   r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
                   add_to_lowLevelList(r, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);	           
                ;}
    break;

  case 709:

/* Line 1455 of yacc.c  */
#line 5268 "gram1.y"
    {(yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), CONSISTENT_GROUP_NAME,global_default,LOCAL);;}
    break;

  case 710:

/* Line 1455 of yacc.c  */
#line 5282 "gram1.y"
    { PTR_SYMB s;
            if(parstate == INEXEC){
              if (!(s = (yyvsp[(2) - (3)].hash_entry)->id_attr))
              {
	         s = make_array((yyvsp[(2) - (3)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
	     	 s->decl = SOFT;
	      } 
            } else
              s = make_array((yyvsp[(2) - (3)].hash_entry), TYNULL, LLNULL, 0, LOCAL);

              (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (3)].ll_node), LLNULL, s);
            ;}
    break;

  case 711:

/* Line 1455 of yacc.c  */
#line 5295 "gram1.y"
    { (yyval.ll_node) = LLNULL; opt_kwd_ = NO;;}
    break;

  case 712:

/* Line 1455 of yacc.c  */
#line 5301 "gram1.y"
    { PTR_LLND q;
             if(!(yyvsp[(4) - (5)].ll_node))
               err("Distribution format list is omitted", 51);
            /* if($6)
               err("NEW_VALUE specification in DISTRIBUTE directive");*/
             q = set_ll_list((yyvsp[(3) - (5)].ll_node),LLNULL,EXPR_LIST);
	     (yyval.bf_node) = get_bfnd(fi,DVM_DISTRIBUTE_DIR,SMNULL,q,(yyvsp[(4) - (5)].ll_node),(yyvsp[(5) - (5)].ll_node));
            ;}
    break;

  case 713:

/* Line 1455 of yacc.c  */
#line 5317 "gram1.y"
    { PTR_LLND q;
                /*  if(!$4)
                  {err("Distribution format is omitted", 51); errcnt--;}
                 */
              q = set_ll_list((yyvsp[(3) - (6)].ll_node),LLNULL,EXPR_LIST);
                 /* r = LLNULL;
                   if($6){
                     r = set_ll_list($6,LLNULL,EXPR_LIST);
                     if($7) r = set_ll_list(r,$7,EXPR_LIST);
                   } else
                     if($7) r = set_ll_list(r,$7,EXPR_LIST);
                 */
	      (yyval.bf_node) = get_bfnd(fi,DVM_REDISTRIBUTE_DIR,SMNULL,q,(yyvsp[(4) - (6)].ll_node),(yyvsp[(6) - (6)].ll_node));;}
    break;

  case 714:

/* Line 1455 of yacc.c  */
#line 5332 "gram1.y"
    {
                 /* r = LLNULL;
                    if($5){
                      r = set_ll_list($5,LLNULL,EXPR_LIST);
                      if($6) r = set_ll_list(r,$6,EXPR_LIST);
                    } else
                      if($6) r = set_ll_list(r,$6,EXPR_LIST);
                  */
	      (yyval.bf_node) = get_bfnd(fi,DVM_REDISTRIBUTE_DIR,SMNULL,(yyvsp[(8) - (8)].ll_node) ,(yyvsp[(3) - (8)].ll_node),(yyvsp[(5) - (8)].ll_node) );
             ;}
    break;

  case 715:

/* Line 1455 of yacc.c  */
#line 5360 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 716:

/* Line 1455 of yacc.c  */
#line 5362 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 717:

/* Line 1455 of yacc.c  */
#line 5366 "gram1.y"
    {(yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 718:

/* Line 1455 of yacc.c  */
#line 5368 "gram1.y"
    {(yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 719:

/* Line 1455 of yacc.c  */
#line 5372 "gram1.y"
    {  PTR_SYMB s;
 
          if(parstate == INEXEC){
            if (!(s = (yyvsp[(1) - (1)].hash_entry)->id_attr))
              {
	         s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
	     	 s->decl = SOFT;
	      } 
            if(s->attr & PROCESSORS_BIT)
              errstr( "Illegal use of PROCESSORS name %s ", s->ident, 53);
            if(s->attr & TASK_BIT)
              errstr( "Illegal use of task array name %s ", s->ident, 71);

          } else {
            s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
            if(s->attr & DISTRIBUTE_BIT)
              errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
            else if( (s->attr & PROCESSORS_BIT) || (s->attr & TASK_BIT) || (s->attr & INHERIT_BIT))
              errstr("Inconsistent declaration of identifier  %s",s->ident, 16);
            else
              s->attr = s->attr | DISTRIBUTE_BIT;
          } 
         if(s->attr & ALIGN_BIT)
               errstr("A distributee may not have the ALIGN attribute:%s",s->ident, 54);
          (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);               	  
	;}
    break;

  case 720:

/* Line 1455 of yacc.c  */
#line 5401 "gram1.y"
    {  PTR_SYMB s;
          s = make_array((yyvsp[(1) - (4)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
        
          if(parstate != INEXEC) 
               errstr( "Illegal distributee:%s", s->ident, 312);
          else {
            if(s->attr & PROCESSORS_BIT)
               errstr( "Illegal use of PROCESSORS name %s ", s->ident, 53);  
            if(s->attr & TASK_BIT)
               errstr( "Illegal use of task array name %s ", s->ident, 71);        
            if(s->attr & ALIGN_BIT)
               errstr("A distributee may not have the ALIGN attribute:%s",s->ident, 54);
            if(!(s->attr & DVM_POINTER_BIT))
               errstr("Illegal distributee:%s", s->ident, 312);
          /*s->attr = s->attr | DISTRIBUTE_BIT;*/
	  (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (4)].ll_node), LLNULL, s); 
          }
        
	;}
    break;

  case 721:

/* Line 1455 of yacc.c  */
#line 5424 "gram1.y"
    {  PTR_SYMB s;
          if((s=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL)
            s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
          if((parstate == INEXEC) && !(s->attr & PROCESSORS_BIT))
               errstr( "'%s' is not processor array ", s->ident, 67);
	  (yyval.symbol) = s;
	;}
    break;

  case 722:

/* Line 1455 of yacc.c  */
#line 5444 "gram1.y"
    { (yyval.ll_node) = LLNULL;  ;}
    break;

  case 723:

/* Line 1455 of yacc.c  */
#line 5446 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (2)].ll_node);;}
    break;

  case 724:

/* Line 1455 of yacc.c  */
#line 5450 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node);;}
    break;

  case 725:

/* Line 1455 of yacc.c  */
#line 5471 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 726:

/* Line 1455 of yacc.c  */
#line 5473 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),EXPR_LIST); ;}
    break;

  case 727:

/* Line 1455 of yacc.c  */
#line 5476 "gram1.y"
    { opt_kwd_ = YES; ;}
    break;

  case 728:

/* Line 1455 of yacc.c  */
#line 5485 "gram1.y"
    {  
               (yyval.ll_node) = make_llnd(fi,BLOCK_OP, LLNULL, LLNULL, SMNULL);
        ;}
    break;

  case 729:

/* Line 1455 of yacc.c  */
#line 5489 "gram1.y"
    {  err("Distribution format BLOCK(n) is not permitted in FDVM", 55);
          (yyval.ll_node) = make_llnd(fi,BLOCK_OP, (yyvsp[(4) - (5)].ll_node), LLNULL, SMNULL);
          endioctl();
        ;}
    break;

  case 730:

/* Line 1455 of yacc.c  */
#line 5494 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,BLOCK_OP, LLNULL, LLNULL, (yyvsp[(3) - (4)].symbol)); ;}
    break;

  case 731:

/* Line 1455 of yacc.c  */
#line 5496 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,BLOCK_OP,  (yyvsp[(5) - (6)].ll_node),  LLNULL,  (yyvsp[(3) - (6)].symbol)); ;}
    break;

  case 732:

/* Line 1455 of yacc.c  */
#line 5498 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,BLOCK_OP,  LLNULL, (yyvsp[(3) - (4)].ll_node),  SMNULL); ;}
    break;

  case 733:

/* Line 1455 of yacc.c  */
#line 5500 "gram1.y"
    { 
          (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
          (yyval.ll_node)->entry.string_val = (char *) "*";
          (yyval.ll_node)->type = global_string;
        ;}
    break;

  case 734:

/* Line 1455 of yacc.c  */
#line 5506 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,INDIRECT_OP, LLNULL, LLNULL, (yyvsp[(3) - (4)].symbol)); ;}
    break;

  case 735:

/* Line 1455 of yacc.c  */
#line 5508 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,INDIRECT_OP, (yyvsp[(3) - (4)].ll_node), LLNULL, SMNULL); ;}
    break;

  case 736:

/* Line 1455 of yacc.c  */
#line 5512 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              if((s->attr & PROCESSORS_BIT) ||(s->attr & TASK_BIT)  || (s->attr & TEMPLATE_BIT)) 
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16); 
       
	      (yyval.symbol) = s;
	   ;}
    break;

  case 737:

/* Line 1455 of yacc.c  */
#line 5522 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DERIVED_OP, (yyvsp[(2) - (6)].ll_node), (yyvsp[(6) - (6)].ll_node), SMNULL); ;}
    break;

  case 738:

/* Line 1455 of yacc.c  */
#line 5526 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 739:

/* Line 1455 of yacc.c  */
#line 5528 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 740:

/* Line 1455 of yacc.c  */
#line 5533 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 741:

/* Line 1455 of yacc.c  */
#line 5535 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);;}
    break;

  case 742:

/* Line 1455 of yacc.c  */
#line 5539 "gram1.y"
    { 
              (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, (yyvsp[(1) - (1)].symbol));
	    ;}
    break;

  case 743:

/* Line 1455 of yacc.c  */
#line 5543 "gram1.y"
    { 
              (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (4)].ll_node), LLNULL, (yyvsp[(1) - (4)].symbol));
	    ;}
    break;

  case 744:

/* Line 1455 of yacc.c  */
#line 5549 "gram1.y"
    { 
              if (!((yyval.symbol) = (yyvsp[(1) - (1)].hash_entry)->id_attr))
              {
	         (yyval.symbol) = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL,0,LOCAL);
	         (yyval.symbol)->decl = SOFT;
	      } 
            ;}
    break;

  case 745:

/* Line 1455 of yacc.c  */
#line 5559 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 746:

/* Line 1455 of yacc.c  */
#line 5561 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 747:

/* Line 1455 of yacc.c  */
#line 5565 "gram1.y"
    {  (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 748:

/* Line 1455 of yacc.c  */
#line 5567 "gram1.y"
    {  (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 749:

/* Line 1455 of yacc.c  */
#line 5569 "gram1.y"
    {
                      (yyvsp[(2) - (3)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (3)].ll_node); 
                      (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node);   
                   ;}
    break;

  case 750:

/* Line 1455 of yacc.c  */
#line 5576 "gram1.y"
    { PTR_SYMB s;
            s = make_scalar((yyvsp[(1) - (1)].hash_entry),TYNULL,LOCAL);
	    (yyval.ll_node) = make_llnd(fi,DUMMY_REF, LLNULL, LLNULL, s);
            /*$$->type = global_int;*/
          ;}
    break;

  case 751:

/* Line 1455 of yacc.c  */
#line 5593 "gram1.y"
    {  (yyval.ll_node) = LLNULL; ;}
    break;

  case 752:

/* Line 1455 of yacc.c  */
#line 5595 "gram1.y"
    {  (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 753:

/* Line 1455 of yacc.c  */
#line 5599 "gram1.y"
    {  (yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 754:

/* Line 1455 of yacc.c  */
#line 5601 "gram1.y"
    {  (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 755:

/* Line 1455 of yacc.c  */
#line 5605 "gram1.y"
    {  if((yyvsp[(1) - (1)].ll_node)->type->variant != T_STRING)
                 errstr( "Illegal type of shadow_name", 627);
               (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); 
            ;}
    break;

  case 756:

/* Line 1455 of yacc.c  */
#line 5612 "gram1.y"
    { char *q;
          nioctl = 1;
          q = (yyvsp[(1) - (2)].ll_node)->entry.string_val;
          if((!strcmp(q,"shadow")) && ((yyvsp[(2) - (2)].ll_node)->variant == INT_VAL))                          (yyval.ll_node) = make_llnd(fi,SPEC_PAIR, (yyvsp[(1) - (2)].ll_node), (yyvsp[(2) - (2)].ll_node), SMNULL);
          else
          {  err("Illegal shadow width specification", 56);
             (yyval.ll_node) = LLNULL;
          }
        ;}
    break;

  case 757:

/* Line 1455 of yacc.c  */
#line 5622 "gram1.y"
    { char *ql, *qh;
          PTR_LLND p1, p2;
          nioctl = 2;
          ql = (yyvsp[(1) - (5)].ll_node)->entry.string_val;
          qh = (yyvsp[(4) - (5)].ll_node)->entry.string_val;
          if((!strcmp(ql,"low_shadow")) && ((yyvsp[(2) - (5)].ll_node)->variant == INT_VAL) && (!strcmp(qh,"high_shadow")) && ((yyvsp[(5) - (5)].ll_node)->variant == INT_VAL)) 
              {
                 p1 = make_llnd(fi,SPEC_PAIR, (yyvsp[(1) - (5)].ll_node), (yyvsp[(2) - (5)].ll_node), SMNULL);
                 p2 = make_llnd(fi,SPEC_PAIR, (yyvsp[(4) - (5)].ll_node), (yyvsp[(5) - (5)].ll_node), SMNULL);
                 (yyval.ll_node) = make_llnd(fi,CONS, p1, p2, SMNULL);
              } 
          else
          {  err("Illegal shadow width specification", 56);
             (yyval.ll_node) = LLNULL;
          }
        ;}
    break;

  case 758:

/* Line 1455 of yacc.c  */
#line 5651 "gram1.y"
    { PTR_LLND q;
              q = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
              (yyval.bf_node) = (yyvsp[(4) - (4)].bf_node);
              (yyval.bf_node)->entry.Template.ll_ptr1 = q;
            ;}
    break;

  case 759:

/* Line 1455 of yacc.c  */
#line 5666 "gram1.y"
    { PTR_LLND q;
              q = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
              (yyval.bf_node) = (yyvsp[(4) - (4)].bf_node);
              (yyval.bf_node)->variant = DVM_REALIGN_DIR; 
              (yyval.bf_node)->entry.Template.ll_ptr1 = q;
            ;}
    break;

  case 760:

/* Line 1455 of yacc.c  */
#line 5673 "gram1.y"
    {
              (yyval.bf_node) = (yyvsp[(3) - (6)].bf_node);
              (yyval.bf_node)->variant = DVM_REALIGN_DIR; 
              (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(6) - (6)].ll_node);
            ;}
    break;

  case 761:

/* Line 1455 of yacc.c  */
#line 5691 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 762:

/* Line 1455 of yacc.c  */
#line 5693 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 763:

/* Line 1455 of yacc.c  */
#line 5697 "gram1.y"
    {  PTR_SYMB s;
          s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
          if((s->attr & ALIGN_BIT)) 
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
          if((s->attr & PROCESSORS_BIT) || (s->attr & TASK_BIT) || (s->attr & INHERIT_BIT)) 
                errstr( "Inconsistent declaration of identifier  %s", s->ident, 16); 
          else  if(s->attr & DISTRIBUTE_BIT)
               errstr( "An alignee may not have the DISTRIBUTE attribute:'%s'", s->ident,57);             else
                s->attr = s->attr | ALIGN_BIT;     
	  (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	;}
    break;

  case 764:

/* Line 1455 of yacc.c  */
#line 5711 "gram1.y"
    {PTR_SYMB s;
        s = (yyvsp[(1) - (1)].ll_node)->entry.Template.symbol;
        if(s->attr & PROCESSORS_BIT)
               errstr( "Illegal use of PROCESSORS name %s ", s->ident, 53);
        else  if(s->attr & TASK_BIT)
              errstr( "Illegal use of task array name %s ", s->ident, 71);
        else if( !(s->attr & DIMENSION_BIT) && !(s->attr & DVM_POINTER_BIT))
            errstr("The alignee %s isn't an array", s->ident, 58);
        else {
            /*  if(!(s->attr & DYNAMIC_BIT))
                 errstr("'%s' hasn't the DYNAMIC attribute", s->ident, 59);
             */
              if(!(s->attr & ALIGN_BIT) && !(s->attr & INHERIT_BIT))
                 errstr("'%s' hasn't the ALIGN attribute", s->ident, 60);
              if(s->attr & DISTRIBUTE_BIT)
                 errstr("An alignee may not have the DISTRIBUTE attribute: %s", s->ident, 57);

/*               if(s->entry.var_decl.local == IO)
 *                 errstr("An alignee may not be the dummy argument");
*/
          }
	  (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	;}
    break;

  case 765:

/* Line 1455 of yacc.c  */
#line 5737 "gram1.y"
    { /* PTR_LLND r;
              if($7) {
                r = set_ll_list($6,LLNULL,EXPR_LIST);
                r = set_ll_list(r,$7,EXPR_LIST);
              }
              else
                r = $6;
              */
            (yyval.bf_node) = get_bfnd(fi,DVM_ALIGN_DIR,SMNULL,LLNULL,(yyvsp[(2) - (6)].ll_node),(yyvsp[(6) - (6)].ll_node));
           ;}
    break;

  case 766:

/* Line 1455 of yacc.c  */
#line 5750 "gram1.y"
    {
           (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (4)].ll_node), LLNULL, (yyvsp[(1) - (4)].symbol));        
          ;}
    break;

  case 767:

/* Line 1455 of yacc.c  */
#line 5766 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 768:

/* Line 1455 of yacc.c  */
#line 5768 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 769:

/* Line 1455 of yacc.c  */
#line 5771 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 770:

/* Line 1455 of yacc.c  */
#line 5773 "gram1.y"
    {
                  (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
                  (yyval.ll_node)->entry.string_val = (char *) "*";
                  (yyval.ll_node)->type = global_string;
                 ;}
    break;

  case 771:

/* Line 1455 of yacc.c  */
#line 5779 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 772:

/* Line 1455 of yacc.c  */
#line 5783 "gram1.y"
    { 
         /* if(parstate == INEXEC){ *for REALIGN directive*
              if (!($$ = $1->id_attr))
              {
	         $$ = make_array($1, TYNULL, LLNULL,0,LOCAL);
	     	 $$->decl = SOFT;
	      } 
          } else
             $$ = make_array($1, TYNULL, LLNULL, 0, LOCAL);
          */
          if (!((yyval.symbol) = (yyvsp[(1) - (1)].hash_entry)->id_attr))
          {
	       (yyval.symbol) = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL,0,LOCAL);
	       (yyval.symbol)->decl = SOFT;
	  } 
          (yyval.symbol)->attr = (yyval.symbol)->attr | ALIGN_BASE_BIT;
          if((yyval.symbol)->attr & PROCESSORS_BIT)
               errstr( "Illegal use of PROCESSORS name %s ", (yyval.symbol)->ident, 53);
          else  if((yyval.symbol)->attr & TASK_BIT)
               errstr( "Illegal use of task array name %s ", (yyval.symbol)->ident, 71);
          else
          if((parstate == INEXEC) /* for  REALIGN directive */
             &&   !((yyval.symbol)->attr & DIMENSION_BIT) && !((yyval.symbol)->attr & DVM_POINTER_BIT))
            errstr("The align-target %s isn't declared as array", (yyval.symbol)->ident, 61); 
         ;}
    break;

  case 773:

/* Line 1455 of yacc.c  */
#line 5811 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 774:

/* Line 1455 of yacc.c  */
#line 5813 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 775:

/* Line 1455 of yacc.c  */
#line 5817 "gram1.y"
    { PTR_SYMB s;
            s = make_scalar((yyvsp[(1) - (1)].hash_entry),TYNULL,LOCAL);
            if(s->type->variant != T_INT || s->attr & PARAMETER_BIT)             
              errstr("The align-dummy %s isn't a scalar integer variable", s->ident, 62); 
	   (yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
           (yyval.ll_node)->type = global_int;
         ;}
    break;

  case 776:

/* Line 1455 of yacc.c  */
#line 5825 "gram1.y"
    {  
          (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
          (yyval.ll_node)->entry.string_val = (char *) "*";
          (yyval.ll_node)->type = global_string;
        ;}
    break;

  case 777:

/* Line 1455 of yacc.c  */
#line 5831 "gram1.y"
    {   (yyval.ll_node) = make_llnd(fi,DDOT, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 778:

/* Line 1455 of yacc.c  */
#line 5834 "gram1.y"
    { PTR_SYMB s;
	             PTR_LLND q, r, p;
                     int numdim;
                     if(type_options & PROCESSORS_BIT) {    /* 27.06.18 || (type_options & TEMPLATE_BIT)){ */
                       if(! explicit_shape) {
                         err("Explicit shape specification is required", 50);
		         /*$$ = BFNULL;*/
	               }
                     } 

                    /*  else {
                       if($6)
                         err("Shape specification is not permitted", 263);
                     } */

                     if(type_options & DIMENSION_BIT)
                       { p = attr_dims; numdim = attr_ndim;}
                     else
                       { p = LLNULL; numdim = 0; }
                     if((yyvsp[(6) - (6)].ll_node))          /*dimension information after the object name*/
                     { p = (yyvsp[(6) - (6)].ll_node); numdim = ndim;} /*overrides the DIMENSION attribute */
	             s = make_array((yyvsp[(5) - (6)].hash_entry), TYNULL, p, numdim, LOCAL);

                     if((type_options & COMMON_BIT) && !(type_options & TEMPLATE_BIT))
                     {
                        err("Illegal combination of attributes", 63);
                        type_options = type_options & (~COMMON_BIT);
                     }
                     if((type_options & PROCESSORS_BIT) &&((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT) ||(type_options & TEMPLATE_BIT) || (type_options & DYNAMIC_BIT) ||(type_options & SHADOW_BIT) ))
                        err("Illegal combination of attributes", 63);
                     else  if((type_options & PROCESSORS_BIT) && ((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT)) )
                     {  errstr("Inconsistent declaration of  %s", s->ident, 16);
                        type_options = type_options & (~PROCESSORS_BIT);
                     }
                     else if ((s->attr & PROCESSORS_BIT) && ((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT) ||(type_options & TEMPLATE_BIT) || (type_options & DYNAMIC_BIT) ||(type_options & SHADOW_BIT))) 
                        errstr("Inconsistent declaration of  %s", s->ident, 16);
                     else if ((s->attr & INHERIT_BIT) && ((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT)))
                        errstr("Inconsistent declaration of  %s", s->ident, 16);
                     if(( s->attr & DISTRIBUTE_BIT) &&  (type_options & DISTRIBUTE_BIT))
                           errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & ALIGN_BIT) &&  (type_options & ALIGN_BIT))
                           errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & SHADOW_BIT) &&  (type_options & SHADOW_BIT))
                           errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & TEMPLATE_BIT) &&  (type_options & TEMPLATE_BIT))
                           errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & PROCESSORS_BIT) &&  (type_options & PROCESSORS_BIT))
                           errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
	             s->attr = s->attr | type_options;
                     if((yyvsp[(6) - (6)].ll_node)) s->attr = s->attr | DIMENSION_BIT;
                     if((s->attr & DISTRIBUTE_BIT) && (s->attr & ALIGN_BIT))
                       errstr("%s has the DISTRIBUTE and ALIGN attribute",s->ident, 64);
	             q = make_llnd(fi,ARRAY_REF, (yyvsp[(6) - (6)].ll_node), LLNULL, s);
	             if(p) s->type->entry.ar_decl.ranges = p;
	             r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	             (yyval.bf_node) = get_bfnd(fi,DVM_VAR_DECL, SMNULL, r, LLNULL,(yyvsp[(1) - (6)].ll_node));
	            ;}
    break;

  case 779:

/* Line 1455 of yacc.c  */
#line 5892 "gram1.y"
    { PTR_SYMB s;
	             PTR_LLND q, r, p;
                     int numdim;
                    if(type_options & PROCESSORS_BIT) { /*23.10.18  || (type_options & TEMPLATE_BIT)){ */
                       if(! explicit_shape) {
                         err("Explicit shape specification is required", 50);
		         /*$$ = BFNULL;*/
	               }
                     } 
                    /* else {
                       if($4)
                         err("Shape specification is not permitted", 263);
                     } */
                     if(type_options & DIMENSION_BIT)
                       { p = attr_dims; numdim = attr_ndim;}
                     else
                       { p = LLNULL; numdim = 0; }
                     if((yyvsp[(4) - (4)].ll_node))                   /*dimension information after the object name*/
                     { p = (yyvsp[(4) - (4)].ll_node); numdim = ndim;}/*overrides the DIMENSION attribute */
	             s = make_array((yyvsp[(3) - (4)].hash_entry), TYNULL, p, numdim, LOCAL);

                     if((type_options & COMMON_BIT) && !(type_options & TEMPLATE_BIT))
                     {
                        err("Illegal combination of attributes", 63);
                        type_options = type_options & (~COMMON_BIT);
                     }
                     if((type_options & PROCESSORS_BIT) &&((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT) ||(type_options & TEMPLATE_BIT) || (type_options & DYNAMIC_BIT) ||(type_options & SHADOW_BIT) ))
                       err("Illegal combination of attributes", 63);
                     else  if((type_options & PROCESSORS_BIT) && ((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT)) )
                     {  errstr("Inconsistent declaration of identifier %s", s->ident, 16);
                        type_options = type_options & (~PROCESSORS_BIT);
                     }
                     else if ((s->attr & PROCESSORS_BIT) && ((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT) ||(type_options & TEMPLATE_BIT) || (type_options & DYNAMIC_BIT) ||(type_options & SHADOW_BIT))) 
                          errstr("Inconsistent declaration of identifier  %s", s->ident,16);
                     else if ((s->attr & INHERIT_BIT) && ((type_options & ALIGN_BIT) ||(type_options & DISTRIBUTE_BIT)))
                          errstr("Inconsistent declaration of identifier %s", s->ident, 16);
                     if(( s->attr & DISTRIBUTE_BIT) &&  (type_options & DISTRIBUTE_BIT))
                          errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & ALIGN_BIT) &&  (type_options & ALIGN_BIT))
                          errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & SHADOW_BIT) &&  (type_options & SHADOW_BIT))
                          errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & TEMPLATE_BIT) &&  (type_options & TEMPLATE_BIT))
                          errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
                     if(( s->attr & PROCESSORS_BIT) &&  (type_options & PROCESSORS_BIT))
                          errstr( "Multiple declaration of identifier  %s ", s->ident, 73);   
	             s->attr = s->attr | type_options;
                     if((yyvsp[(4) - (4)].ll_node)) s->attr = s->attr | DIMENSION_BIT;
                     if((s->attr & DISTRIBUTE_BIT) && (s->attr & ALIGN_BIT))
                           errstr("%s has the DISTRIBUTE and ALIGN attribute",s->ident, 64);
	             q = make_llnd(fi,ARRAY_REF, (yyvsp[(4) - (4)].ll_node), LLNULL, s);
	             if(p) s->type->entry.ar_decl.ranges = p;
	             r = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);
	             add_to_lowLevelList(r, (yyvsp[(1) - (4)].bf_node)->entry.Template.ll_ptr1);
	            ;}
    break;

  case 780:

/* Line 1455 of yacc.c  */
#line 5956 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); type_options = type_opt; ;}
    break;

  case 781:

/* Line 1455 of yacc.c  */
#line 5958 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),EXPR_LIST); type_options = type_options | type_opt;;}
    break;

  case 782:

/* Line 1455 of yacc.c  */
#line 5961 "gram1.y"
    { type_opt = TEMPLATE_BIT;
               (yyval.ll_node) = make_llnd(fi,TEMPLATE_OP,LLNULL,LLNULL,SMNULL);
               ;}
    break;

  case 783:

/* Line 1455 of yacc.c  */
#line 5965 "gram1.y"
    { type_opt = PROCESSORS_BIT;
                (yyval.ll_node) = make_llnd(fi,PROCESSORS_OP,LLNULL,LLNULL,SMNULL);
               ;}
    break;

  case 784:

/* Line 1455 of yacc.c  */
#line 5969 "gram1.y"
    { type_opt = PROCESSORS_BIT;
                (yyval.ll_node) = make_llnd(fi,PROCESSORS_OP,LLNULL,LLNULL,SMNULL);
               ;}
    break;

  case 785:

/* Line 1455 of yacc.c  */
#line 5973 "gram1.y"
    { type_opt = DYNAMIC_BIT;
                (yyval.ll_node) = make_llnd(fi,DYNAMIC_OP,LLNULL,LLNULL,SMNULL);
               ;}
    break;

  case 786:

/* Line 1455 of yacc.c  */
#line 5990 "gram1.y"
    {
                if(! explicit_shape) {
                  err("Explicit shape specification is required", 50);
                }
                if(! (yyvsp[(3) - (4)].ll_node)) {
                  err("No shape specification", 65);
	        }
                type_opt = DIMENSION_BIT;
                attr_ndim = ndim; attr_dims = (yyvsp[(3) - (4)].ll_node);
                (yyval.ll_node) = make_llnd(fi,DIMENSION_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);
	       ;}
    break;

  case 787:

/* Line 1455 of yacc.c  */
#line 6002 "gram1.y"
    { type_opt = SHADOW_BIT;
                  (yyval.ll_node) = make_llnd(fi,SHADOW_OP,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
                 ;}
    break;

  case 788:

/* Line 1455 of yacc.c  */
#line 6006 "gram1.y"
    { type_opt = ALIGN_BIT;
                  (yyval.ll_node) = make_llnd(fi,ALIGN_OP,(yyvsp[(3) - (7)].ll_node),(yyvsp[(7) - (7)].ll_node),SMNULL);
                 ;}
    break;

  case 789:

/* Line 1455 of yacc.c  */
#line 6010 "gram1.y"
    { type_opt = ALIGN_BIT;
                  (yyval.ll_node) = make_llnd(fi,ALIGN_OP,LLNULL,SMNULL,SMNULL);
                ;}
    break;

  case 790:

/* Line 1455 of yacc.c  */
#line 6020 "gram1.y"
    { 
                 type_opt = DISTRIBUTE_BIT;
                 (yyval.ll_node) = make_llnd(fi,DISTRIBUTE_OP,(yyvsp[(2) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),SMNULL);
                ;}
    break;

  case 791:

/* Line 1455 of yacc.c  */
#line 6025 "gram1.y"
    { 
                 type_opt = DISTRIBUTE_BIT;
                 (yyval.ll_node) = make_llnd(fi,DISTRIBUTE_OP,LLNULL,LLNULL,SMNULL);
                ;}
    break;

  case 792:

/* Line 1455 of yacc.c  */
#line 6030 "gram1.y"
    {
                 type_opt = COMMON_BIT;
                 (yyval.ll_node) = make_llnd(fi,COMMON_OP, LLNULL, LLNULL, SMNULL);
                ;}
    break;

  case 793:

/* Line 1455 of yacc.c  */
#line 6037 "gram1.y"
    { 
	      PTR_LLND  l;
	      l = make_llnd(fi, TYPE_OP, LLNULL, LLNULL, SMNULL);
	      l->type = (yyvsp[(1) - (11)].data_type);
	      (yyval.bf_node) = get_bfnd(fi,DVM_POINTER_DIR, SMNULL, (yyvsp[(11) - (11)].ll_node),(yyvsp[(7) - (11)].ll_node), l);
	    ;}
    break;

  case 794:

/* Line 1455 of yacc.c  */
#line 6045 "gram1.y"
    {ndim = 0;;}
    break;

  case 795:

/* Line 1455 of yacc.c  */
#line 6046 "gram1.y"
    { PTR_LLND  q;
             if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		q = make_llnd(fi,DDOT,LLNULL,LLNULL,SMNULL);
	      ++ndim;
              (yyval.ll_node) = set_ll_list(q, LLNULL, EXPR_LIST);
	       /*$$ = make_llnd(fi,EXPR_LIST, q, LLNULL, SMNULL);*/
	       /*$$->type = global_default;*/
	    ;}
    break;

  case 796:

/* Line 1455 of yacc.c  */
#line 6057 "gram1.y"
    { PTR_LLND  q;
             if(ndim == maxdim)
		err("Too many dimensions", 43);
	      else if(ndim < maxdim)
		q = make_llnd(fi,DDOT,LLNULL,LLNULL,SMNULL);
	      ++ndim;
              (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), q, EXPR_LIST);
            ;}
    break;

  case 797:

/* Line 1455 of yacc.c  */
#line 6068 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 798:

/* Line 1455 of yacc.c  */
#line 6070 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 799:

/* Line 1455 of yacc.c  */
#line 6074 "gram1.y"
    {PTR_SYMB s;
           /* s = make_scalar($1,TYNULL,LOCAL);*/
            s = make_array((yyvsp[(1) - (1)].hash_entry),TYNULL,LLNULL,0,LOCAL);
            s->attr = s->attr | DVM_POINTER_BIT;
            if((s->attr & PROCESSORS_BIT) || (s->attr & TASK_BIT) || (s->attr & INHERIT_BIT))
               errstr( "Inconsistent declaration of identifier %s", s->ident, 16);     
            (yyval.ll_node) = make_llnd(fi,VAR_REF,LLNULL,LLNULL,s);
            ;}
    break;

  case 800:

/* Line 1455 of yacc.c  */
#line 6085 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_HEAP_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 801:

/* Line 1455 of yacc.c  */
#line 6089 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 802:

/* Line 1455 of yacc.c  */
#line 6091 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 803:

/* Line 1455 of yacc.c  */
#line 6095 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              s->attr = s->attr | HEAP_BIT;
              if((s->attr & PROCESSORS_BIT) ||(s->attr & TASK_BIT)  || (s->attr & TEMPLATE_BIT) || (s->attr & ALIGN_BIT) || (s->attr & DISTRIBUTE_BIT) || (s->attr & INHERIT_BIT) || (s->attr & DYNAMIC_BIT) || (s->attr & SHADOW_BIT) || (s->attr & DVM_POINTER_BIT)) 
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16); 
      
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	   ;}
    break;

  case 804:

/* Line 1455 of yacc.c  */
#line 6106 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CONSISTENT_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 805:

/* Line 1455 of yacc.c  */
#line 6110 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 806:

/* Line 1455 of yacc.c  */
#line 6112 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 807:

/* Line 1455 of yacc.c  */
#line 6116 "gram1.y"
    {  PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              s->attr = s->attr | CONSISTENT_BIT;
              if((s->attr & PROCESSORS_BIT) ||(s->attr & TASK_BIT)  || (s->attr & TEMPLATE_BIT) || (s->attr & ALIGN_BIT) || (s->attr & DISTRIBUTE_BIT) || (s->attr & INHERIT_BIT) || (s->attr & DYNAMIC_BIT) || (s->attr & SHADOW_BIT) || (s->attr & DVM_POINTER_BIT)) 
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16); 
      
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, LLNULL, LLNULL, s);
	   ;}
    break;

  case 808:

/* Line 1455 of yacc.c  */
#line 6128 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ASYNCID_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 809:

/* Line 1455 of yacc.c  */
#line 6130 "gram1.y"
    { PTR_LLND p;
              p = make_llnd(fi,COMM_LIST, LLNULL, LLNULL, SMNULL);              
              (yyval.bf_node) = get_bfnd(fi,DVM_ASYNCID_DIR, SMNULL, (yyvsp[(8) - (8)].ll_node), p, LLNULL);
            ;}
    break;

  case 810:

/* Line 1455 of yacc.c  */
#line 6137 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 811:

/* Line 1455 of yacc.c  */
#line 6139 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 812:

/* Line 1455 of yacc.c  */
#line 6143 "gram1.y"
    {  PTR_SYMB s;
              if((yyvsp[(2) - (2)].ll_node)){
                  s = make_array((yyvsp[(1) - (2)].hash_entry), global_default, (yyvsp[(2) - (2)].ll_node), ndim, LOCAL);
		  s->variant = ASYNC_ID;
                  s->attr = s->attr | DIMENSION_BIT;
                  s->type->entry.ar_decl.ranges = (yyvsp[(2) - (2)].ll_node);
                  (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(2) - (2)].ll_node), LLNULL, s);
              } else {
              s = make_local_entity((yyvsp[(1) - (2)].hash_entry), ASYNC_ID, global_default, LOCAL);
	      (yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
              }
	   ;}
    break;

  case 813:

/* Line 1455 of yacc.c  */
#line 6159 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_NEW_VALUE_DIR,SMNULL, LLNULL, LLNULL,LLNULL);;}
    break;

  case 814:

/* Line 1455 of yacc.c  */
#line 6169 "gram1.y"
    {  if((yyvsp[(6) - (7)].ll_node) &&  (yyvsp[(6) - (7)].ll_node)->entry.Template.symbol->attr & TASK_BIT)
                        (yyval.bf_node) = get_bfnd(fi,DVM_PARALLEL_TASK_DIR,SMNULL,(yyvsp[(6) - (7)].ll_node),(yyvsp[(7) - (7)].ll_node),(yyvsp[(4) - (7)].ll_node));
                    else
                        (yyval.bf_node) = get_bfnd(fi,DVM_PARALLEL_ON_DIR,SMNULL,(yyvsp[(6) - (7)].ll_node),(yyvsp[(7) - (7)].ll_node),(yyvsp[(4) - (7)].ll_node));
                 ;}
    break;

  case 815:

/* Line 1455 of yacc.c  */
#line 6178 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 816:

/* Line 1455 of yacc.c  */
#line 6180 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 818:

/* Line 1455 of yacc.c  */
#line 6187 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (3)].ll_node);;}
    break;

  case 819:

/* Line 1455 of yacc.c  */
#line 6190 "gram1.y"
    { (yyval.ll_node) = LLNULL; opt_kwd_ = NO;;}
    break;

  case 820:

/* Line 1455 of yacc.c  */
#line 6195 "gram1.y"
    {
          if((yyvsp[(1) - (4)].ll_node)->type->variant != T_ARRAY) 
           errstr("'%s' isn't array", (yyvsp[(1) - (4)].ll_node)->entry.Template.symbol->ident, 66);
                 (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
                 (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
                 (yyval.ll_node)->type = (yyvsp[(1) - (4)].ll_node)->type->entry.ar_decl.base_type;
         ;}
    break;

  case 821:

/* Line 1455 of yacc.c  */
#line 6205 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 822:

/* Line 1455 of yacc.c  */
#line 6207 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 823:

/* Line 1455 of yacc.c  */
#line 6211 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 824:

/* Line 1455 of yacc.c  */
#line 6213 "gram1.y"
    {
             (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
             (yyval.ll_node)->entry.string_val = (char *) "*";
             (yyval.ll_node)->type = global_string;
            ;}
    break;

  case 825:

/* Line 1455 of yacc.c  */
#line 6221 "gram1.y"
    {  (yyval.ll_node) = LLNULL;;}
    break;

  case 826:

/* Line 1455 of yacc.c  */
#line 6223 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 827:

/* Line 1455 of yacc.c  */
#line 6227 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 828:

/* Line 1455 of yacc.c  */
#line 6229 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (2)].ll_node),(yyvsp[(2) - (2)].ll_node),EXPR_LIST); ;}
    break;

  case 839:

/* Line 1455 of yacc.c  */
#line 6246 "gram1.y"
    { if((yyvsp[(5) - (8)].symbol)->attr & INDIRECT_BIT)
                            errstr("'%s' is not remote group name", (yyvsp[(5) - (8)].symbol)->ident, 68);
                          (yyval.ll_node) = make_llnd(fi,REMOTE_ACCESS_OP,(yyvsp[(7) - (8)].ll_node),LLNULL,(yyvsp[(5) - (8)].symbol));
                        ;}
    break;

  case 840:

/* Line 1455 of yacc.c  */
#line 6251 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,REMOTE_ACCESS_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 841:

/* Line 1455 of yacc.c  */
#line 6255 "gram1.y"
    {
                          (yyval.ll_node) = make_llnd(fi,CONSISTENT_OP,(yyvsp[(7) - (8)].ll_node),LLNULL,(yyvsp[(5) - (8)].symbol));
                        ;}
    break;

  case 842:

/* Line 1455 of yacc.c  */
#line 6259 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,CONSISTENT_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 843:

/* Line 1455 of yacc.c  */
#line 6263 "gram1.y"
    {  
            if(((yyval.symbol)=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL){
                errstr("'%s' is not declared as group", (yyvsp[(1) - (1)].hash_entry)->ident, 74);
                (yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry),CONSISTENT_GROUP_NAME,global_default,LOCAL);
            } else {
                if((yyval.symbol)->variant != CONSISTENT_GROUP_NAME)
                   errstr("'%s' is not declared as group", (yyvsp[(1) - (1)].hash_entry)->ident, 74);
            }
          ;}
    break;

  case 844:

/* Line 1455 of yacc.c  */
#line 6276 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,NEW_SPEC_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 845:

/* Line 1455 of yacc.c  */
#line 6280 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,NEW_SPEC_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 846:

/* Line 1455 of yacc.c  */
#line 6284 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_PRIVATE_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 847:

/* Line 1455 of yacc.c  */
#line 6288 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_CUDA_BLOCK_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 848:

/* Line 1455 of yacc.c  */
#line 6291 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 849:

/* Line 1455 of yacc.c  */
#line 6293 "gram1.y"
    {(yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 850:

/* Line 1455 of yacc.c  */
#line 6295 "gram1.y"
    {(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(3) - (5)].ll_node),EXPR_LIST); (yyval.ll_node) = set_ll_list((yyval.ll_node),(yyvsp[(5) - (5)].ll_node),EXPR_LIST);;}
    break;

  case 851:

/* Line 1455 of yacc.c  */
#line 6299 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 852:

/* Line 1455 of yacc.c  */
#line 6301 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 853:

/* Line 1455 of yacc.c  */
#line 6305 "gram1.y"
    { if(!((yyvsp[(5) - (8)].symbol)->attr & INDIRECT_BIT))
                         errstr("'%s' is not indirect group name", (yyvsp[(5) - (8)].symbol)->ident, 313);
                      (yyval.ll_node) = make_llnd(fi,INDIRECT_ACCESS_OP,(yyvsp[(7) - (8)].ll_node),LLNULL,(yyvsp[(5) - (8)].symbol));
                    ;}
    break;

  case 854:

/* Line 1455 of yacc.c  */
#line 6310 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,INDIRECT_ACCESS_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 855:

/* Line 1455 of yacc.c  */
#line 6314 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,STAGE_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 856:

/* Line 1455 of yacc.c  */
#line 6318 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,ACROSS_OP,(yyvsp[(4) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 857:

/* Line 1455 of yacc.c  */
#line 6320 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,ACROSS_OP,(yyvsp[(4) - (5)].ll_node),(yyvsp[(5) - (5)].ll_node),SMNULL);;}
    break;

  case 858:

/* Line 1455 of yacc.c  */
#line 6324 "gram1.y"
    {  if((yyvsp[(3) - (5)].ll_node))
                     (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(3) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),SMNULL);
                   else
                     (yyval.ll_node) = (yyvsp[(4) - (5)].ll_node);
                ;}
    break;

  case 859:

/* Line 1455 of yacc.c  */
#line 6332 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "in";
              (yyval.ll_node)->type = global_string;
            ;}
    break;

  case 860:

/* Line 1455 of yacc.c  */
#line 6338 "gram1.y"
    {
	      (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "out";
              (yyval.ll_node)->type = global_string;
            ;}
    break;

  case 861:

/* Line 1455 of yacc.c  */
#line 6344 "gram1.y"
    {  (yyval.ll_node) = LLNULL; opt_kwd_ = NO;;}
    break;

  case 862:

/* Line 1455 of yacc.c  */
#line 6348 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 863:

/* Line 1455 of yacc.c  */
#line 6350 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 864:

/* Line 1455 of yacc.c  */
#line 6354 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 865:

/* Line 1455 of yacc.c  */
#line 6356 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
                    (yyval.ll_node)-> entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);  
                  ;}
    break;

  case 866:

/* Line 1455 of yacc.c  */
#line 6360 "gram1.y"
    { /*  PTR_LLND p;
                       p = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
                       p->entry.string_val = (char *) "corner";
                       p->type = global_string;
                   */
                   (yyvsp[(1) - (7)].ll_node)-> entry.Template.ll_ptr1 = (yyvsp[(3) - (7)].ll_node);  
                   (yyval.ll_node) = make_llnd(fi,ARRAY_OP,(yyvsp[(1) - (7)].ll_node),(yyvsp[(6) - (7)].ll_node),SMNULL);
                 ;}
    break;

  case 867:

/* Line 1455 of yacc.c  */
#line 6372 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 868:

/* Line 1455 of yacc.c  */
#line 6374 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 869:

/* Line 1455 of yacc.c  */
#line 6378 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, (yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), SMNULL);;}
    break;

  case 870:

/* Line 1455 of yacc.c  */
#line 6382 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 871:

/* Line 1455 of yacc.c  */
#line 6384 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 872:

/* Line 1455 of yacc.c  */
#line 6388 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (5)].ll_node),make_llnd(fi,DDOT,(yyvsp[(3) - (5)].ll_node),(yyvsp[(5) - (5)].ll_node),SMNULL),SMNULL); ;}
    break;

  case 873:

/* Line 1455 of yacc.c  */
#line 6390 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (3)].ll_node),make_llnd(fi,DDOT,(yyvsp[(3) - (3)].ll_node),LLNULL,SMNULL),SMNULL); ;}
    break;

  case 874:

/* Line 1455 of yacc.c  */
#line 6392 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (3)].ll_node),make_llnd(fi,DDOT,LLNULL,(yyvsp[(3) - (3)].ll_node),SMNULL),SMNULL); ;}
    break;

  case 875:

/* Line 1455 of yacc.c  */
#line 6394 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(1) - (1)].ll_node),LLNULL,SMNULL); ;}
    break;

  case 876:

/* Line 1455 of yacc.c  */
#line 6396 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,LLNULL,make_llnd(fi,DDOT,(yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),SMNULL),SMNULL); ;}
    break;

  case 877:

/* Line 1455 of yacc.c  */
#line 6398 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,LLNULL,make_llnd(fi,DDOT,(yyvsp[(1) - (1)].ll_node),LLNULL,SMNULL),SMNULL); ;}
    break;

  case 878:

/* Line 1455 of yacc.c  */
#line 6400 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT,LLNULL,make_llnd(fi,DDOT,LLNULL,(yyvsp[(1) - (1)].ll_node),SMNULL),SMNULL); ;}
    break;

  case 879:

/* Line 1455 of yacc.c  */
#line 6404 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (3)].ll_node);;}
    break;

  case 880:

/* Line 1455 of yacc.c  */
#line 6408 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (3)].ll_node);;}
    break;

  case 881:

/* Line 1455 of yacc.c  */
#line 6412 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(3) - (3)].ll_node);;}
    break;

  case 882:

/* Line 1455 of yacc.c  */
#line 6416 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (3)].ll_node);;}
    break;

  case 883:

/* Line 1455 of yacc.c  */
#line 6420 "gram1.y"
    {PTR_LLND q;
                /* q = set_ll_list($9,$6,EXPR_LIST); */
                 q = set_ll_list((yyvsp[(6) - (10)].ll_node),LLNULL,EXPR_LIST); /*podd 11.10.01*/
                 q = add_to_lowLevelList((yyvsp[(9) - (10)].ll_node),q);        /*podd 11.10.01*/
                 (yyval.ll_node) = make_llnd(fi,REDUCTION_OP,q,LLNULL,SMNULL);
                ;}
    break;

  case 884:

/* Line 1455 of yacc.c  */
#line 6427 "gram1.y"
    {PTR_LLND q;
                 q = set_ll_list((yyvsp[(6) - (8)].ll_node),LLNULL,EXPR_LIST);
                 (yyval.ll_node) = make_llnd(fi,REDUCTION_OP,q,LLNULL,SMNULL);
                ;}
    break;

  case 885:

/* Line 1455 of yacc.c  */
#line 6433 "gram1.y"
    {  (yyval.ll_node) = make_llnd(fi,REDUCTION_OP,(yyvsp[(9) - (10)].ll_node),LLNULL,(yyvsp[(6) - (10)].symbol)); ;}
    break;

  case 886:

/* Line 1455 of yacc.c  */
#line 6437 "gram1.y"
    { opt_kwd_r = YES; ;}
    break;

  case 887:

/* Line 1455 of yacc.c  */
#line 6440 "gram1.y"
    { opt_kwd_r = NO; ;}
    break;

  case 888:

/* Line 1455 of yacc.c  */
#line 6444 "gram1.y"
    { 
                  if(((yyval.symbol)=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL) {
                      errstr("'%s' is not declared as reduction group", (yyvsp[(1) - (1)].hash_entry)->ident, 69);
                      (yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry),REDUCTION_GROUP_NAME,global_default,LOCAL);
                  } else {
                    if((yyval.symbol)->variant != REDUCTION_GROUP_NAME)
                      errstr("'%s' is not declared as reduction group", (yyvsp[(1) - (1)].hash_entry)->ident, 69);
                  }
                ;}
    break;

  case 889:

/* Line 1455 of yacc.c  */
#line 6457 "gram1.y"
    {(yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 890:

/* Line 1455 of yacc.c  */
#line 6459 "gram1.y"
    {(yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),EXPR_LIST);;}
    break;

  case 891:

/* Line 1455 of yacc.c  */
#line 6463 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,ARRAY_OP,(yyvsp[(1) - (4)].ll_node),(yyvsp[(3) - (4)].ll_node),SMNULL);;}
    break;

  case 892:

/* Line 1455 of yacc.c  */
#line 6465 "gram1.y"
    {(yyvsp[(3) - (6)].ll_node) = set_ll_list((yyvsp[(3) - (6)].ll_node),(yyvsp[(5) - (6)].ll_node),EXPR_LIST);
            (yyval.ll_node) = make_llnd(fi,ARRAY_OP,(yyvsp[(1) - (6)].ll_node),(yyvsp[(3) - (6)].ll_node),SMNULL);;}
    break;

  case 893:

/* Line 1455 of yacc.c  */
#line 6470 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "sum";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 894:

/* Line 1455 of yacc.c  */
#line 6476 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "product";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 895:

/* Line 1455 of yacc.c  */
#line 6482 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "min";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 896:

/* Line 1455 of yacc.c  */
#line 6488 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "max";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 897:

/* Line 1455 of yacc.c  */
#line 6494 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "or";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 898:

/* Line 1455 of yacc.c  */
#line 6500 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "and";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 899:

/* Line 1455 of yacc.c  */
#line 6506 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "eqv";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 900:

/* Line 1455 of yacc.c  */
#line 6512 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "neqv";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 901:

/* Line 1455 of yacc.c  */
#line 6518 "gram1.y"
    { err("Illegal reduction operation name", 70);
               errcnt--;
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "unknown";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 902:

/* Line 1455 of yacc.c  */
#line 6527 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "maxloc";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 903:

/* Line 1455 of yacc.c  */
#line 6533 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "minloc";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 904:

/* Line 1455 of yacc.c  */
#line 6550 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SHADOW_RENEW_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 905:

/* Line 1455 of yacc.c  */
#line 6558 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SHADOW_START_OP,LLNULL,LLNULL,(yyvsp[(4) - (4)].symbol));;}
    break;

  case 906:

/* Line 1455 of yacc.c  */
#line 6566 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SHADOW_WAIT_OP,LLNULL,LLNULL,(yyvsp[(4) - (4)].symbol));;}
    break;

  case 907:

/* Line 1455 of yacc.c  */
#line 6568 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SHADOW_COMP_OP,LLNULL,LLNULL,SMNULL);;}
    break;

  case 908:

/* Line 1455 of yacc.c  */
#line 6570 "gram1.y"
    {  (yyvsp[(5) - (9)].ll_node)-> entry.Template.ll_ptr1 = (yyvsp[(7) - (9)].ll_node); (yyval.ll_node) = make_llnd(fi,SHADOW_COMP_OP,(yyvsp[(5) - (9)].ll_node),LLNULL,SMNULL);;}
    break;

  case 909:

/* Line 1455 of yacc.c  */
#line 6574 "gram1.y"
    {(yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry), SHADOW_GROUP_NAME,global_default,LOCAL);;}
    break;

  case 910:

/* Line 1455 of yacc.c  */
#line 6578 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);;}
    break;

  case 911:

/* Line 1455 of yacc.c  */
#line 6580 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);;}
    break;

  case 912:

/* Line 1455 of yacc.c  */
#line 6584 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 913:

/* Line 1455 of yacc.c  */
#line 6586 "gram1.y"
    { PTR_LLND p;
          p = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
          p->entry.string_val = (char *) "corner";
          p->type = global_string;
          (yyval.ll_node) = make_llnd(fi,ARRAY_OP,(yyvsp[(1) - (5)].ll_node),p,SMNULL);
         ;}
    break;

  case 914:

/* Line 1455 of yacc.c  */
#line 6594 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (5)].ll_node);
          (yyval.ll_node)-> entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);  
        ;}
    break;

  case 915:

/* Line 1455 of yacc.c  */
#line 6598 "gram1.y"
    { PTR_LLND p;
          p = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
          p->entry.string_val = (char *) "corner";
          p->type = global_string;
          (yyvsp[(1) - (9)].ll_node)-> entry.Template.ll_ptr1 = (yyvsp[(4) - (9)].ll_node);  
          (yyval.ll_node) = make_llnd(fi,ARRAY_OP,(yyvsp[(1) - (9)].ll_node),p,SMNULL);
       ;}
    break;

  case 916:

/* Line 1455 of yacc.c  */
#line 6609 "gram1.y"
    { optcorner = YES; ;}
    break;

  case 917:

/* Line 1455 of yacc.c  */
#line 6613 "gram1.y"
    { PTR_SYMB s;
         s = (yyvsp[(1) - (1)].ll_node)->entry.Template.symbol;
         if(s->attr & PROCESSORS_BIT)
             errstr( "Illegal use of PROCESSORS name %s ", s->ident, 53);
         else if(s->attr & TASK_BIT)
             errstr( "Illegal use of task array name %s ", s->ident, 71);
         else
           if(s->type->variant != T_ARRAY) 
             errstr("'%s' isn't array", s->ident, 66);
           else 
              if((!(s->attr & DISTRIBUTE_BIT)) && (!(s->attr & ALIGN_BIT)))
               ; /*errstr("hpf.gram: %s is not distributed array", s->ident);*/
                
         (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);
        ;}
    break;

  case 918:

/* Line 1455 of yacc.c  */
#line 6631 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 919:

/* Line 1455 of yacc.c  */
#line 6633 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 920:

/* Line 1455 of yacc.c  */
#line 6637 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_SHADOW_START_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 921:

/* Line 1455 of yacc.c  */
#line 6639 "gram1.y"
    {errstr("Missing DVM directive prefix", 49);;}
    break;

  case 922:

/* Line 1455 of yacc.c  */
#line 6643 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_SHADOW_WAIT_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 923:

/* Line 1455 of yacc.c  */
#line 6645 "gram1.y"
    {errstr("Missing DVM directive prefix", 49);;}
    break;

  case 924:

/* Line 1455 of yacc.c  */
#line 6649 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_SHADOW_GROUP_DIR,(yyvsp[(3) - (6)].symbol),(yyvsp[(5) - (6)].ll_node),LLNULL,LLNULL);;}
    break;

  case 925:

/* Line 1455 of yacc.c  */
#line 6653 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_REDUCTION_START_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 926:

/* Line 1455 of yacc.c  */
#line 6657 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_REDUCTION_WAIT_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 927:

/* Line 1455 of yacc.c  */
#line 6666 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CONSISTENT_START_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 928:

/* Line 1455 of yacc.c  */
#line 6670 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CONSISTENT_WAIT_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 929:

/* Line 1455 of yacc.c  */
#line 6674 "gram1.y"
    { if(((yyvsp[(4) - (7)].symbol)->attr & INDIRECT_BIT))
                errstr("'%s' is not remote group name", (yyvsp[(4) - (7)].symbol)->ident, 68);
           (yyval.bf_node) = get_bfnd(fi,DVM_REMOTE_ACCESS_DIR,(yyvsp[(4) - (7)].symbol),(yyvsp[(6) - (7)].ll_node),LLNULL,LLNULL);
         ;}
    break;

  case 930:

/* Line 1455 of yacc.c  */
#line 6679 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_REMOTE_ACCESS_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL);;}
    break;

  case 931:

/* Line 1455 of yacc.c  */
#line 6683 "gram1.y"
    {  
            if(((yyval.symbol)=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL){
                errstr("'%s' is not declared as group", (yyvsp[(1) - (1)].hash_entry)->ident, 74);
                (yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry),REF_GROUP_NAME,global_default,LOCAL);
            } else {
              if((yyval.symbol)->variant != REF_GROUP_NAME)
                errstr("'%s' is not declared as group", (yyvsp[(1) - (1)].hash_entry)->ident, 74);
            }
          ;}
    break;

  case 932:

/* Line 1455 of yacc.c  */
#line 6695 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 933:

/* Line 1455 of yacc.c  */
#line 6697 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 934:

/* Line 1455 of yacc.c  */
#line 6701 "gram1.y"
    {
              (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
              (yyval.ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
            ;}
    break;

  case 935:

/* Line 1455 of yacc.c  */
#line 6706 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 936:

/* Line 1455 of yacc.c  */
#line 6710 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 937:

/* Line 1455 of yacc.c  */
#line 6712 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 938:

/* Line 1455 of yacc.c  */
#line 6716 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 939:

/* Line 1455 of yacc.c  */
#line 6718 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, LLNULL, LLNULL, SMNULL);;}
    break;

  case 940:

/* Line 1455 of yacc.c  */
#line 6722 "gram1.y"
    {  PTR_LLND q;
             q = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
             (yyval.bf_node) = get_bfnd(fi,DVM_TASK_DIR,SMNULL,q,LLNULL,LLNULL);
          ;}
    break;

  case 941:

/* Line 1455 of yacc.c  */
#line 6727 "gram1.y"
    {   PTR_LLND q;
              q = make_llnd(fi,EXPR_LIST, (yyvsp[(3) - (3)].ll_node), LLNULL, SMNULL);
	      add_to_lowLevelList(q, (yyvsp[(1) - (3)].bf_node)->entry.Template.ll_ptr1);
          ;}
    break;

  case 942:

/* Line 1455 of yacc.c  */
#line 6734 "gram1.y"
    { 
             PTR_SYMB s;
	      s = make_array((yyvsp[(1) - (2)].hash_entry), global_int, (yyvsp[(2) - (2)].ll_node), ndim, LOCAL);
              if((yyvsp[(2) - (2)].ll_node)){
                  s->attr = s->attr | DIMENSION_BIT;
                  s->type->entry.ar_decl.ranges = (yyvsp[(2) - (2)].ll_node);
              }
              else
                  err("No dimensions in TASK directive", 75);
              if(ndim > 1)
                  errstr("Illegal rank of '%s'", s->ident, 76);
              if(s->attr & TASK_BIT)
                errstr( "Multiple declaration of identifier  %s ", s->ident, 73);
              if((s->attr & ALIGN_BIT) ||(s->attr & DISTRIBUTE_BIT) ||(s->attr & TEMPLATE_BIT) || (s->attr & DYNAMIC_BIT) ||(s->attr & SHADOW_BIT) || (s->attr & PROCESSORS_BIT)  || (s->attr & DVM_POINTER_BIT) || (s->attr & INHERIT_BIT))
                errstr("Inconsistent declaration of identifier  %s", s->ident, 16);
              else
	        s->attr = s->attr | TASK_BIT;
    
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(2) - (2)].ll_node), LLNULL, s);	  
	    ;}
    break;

  case 943:

/* Line 1455 of yacc.c  */
#line 6757 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_TASK_REGION_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 944:

/* Line 1455 of yacc.c  */
#line 6759 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_TASK_REGION_DIR,(yyvsp[(3) - (4)].symbol),(yyvsp[(4) - (4)].ll_node),LLNULL,LLNULL);;}
    break;

  case 945:

/* Line 1455 of yacc.c  */
#line 6761 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_TASK_REGION_DIR,(yyvsp[(3) - (4)].symbol),LLNULL,(yyvsp[(4) - (4)].ll_node),LLNULL);;}
    break;

  case 946:

/* Line 1455 of yacc.c  */
#line 6763 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_TASK_REGION_DIR,(yyvsp[(3) - (5)].symbol),(yyvsp[(4) - (5)].ll_node),(yyvsp[(5) - (5)].ll_node),LLNULL);;}
    break;

  case 947:

/* Line 1455 of yacc.c  */
#line 6765 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_TASK_REGION_DIR,(yyvsp[(3) - (5)].symbol),(yyvsp[(5) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),LLNULL);;}
    break;

  case 948:

/* Line 1455 of yacc.c  */
#line 6769 "gram1.y"
    { PTR_SYMB s;
              if((s=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL)
                s = make_array((yyvsp[(1) - (1)].hash_entry), TYNULL, LLNULL, 0, LOCAL);
              
              if(!(s->attr & TASK_BIT))
                 errstr("'%s' is not task array", s->ident, 77);
              (yyval.symbol) = s;
              ;}
    break;

  case 949:

/* Line 1455 of yacc.c  */
#line 6780 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_END_TASK_REGION_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 950:

/* Line 1455 of yacc.c  */
#line 6784 "gram1.y"
    {  PTR_SYMB s;
              PTR_LLND q;
             /*
              s = make_array($1, TYNULL, LLNULL, 0, LOCAL);                           
	      if((parstate == INEXEC) && !(s->attr & TASK_BIT))
                 errstr("'%s' is not task array", s->ident, 77);  
              q =  set_ll_list($3,LLNULL,EXPR_LIST);
	      $$ = make_llnd(fi,ARRAY_REF, q, LLNULL, s);
              */

              s = (yyvsp[(1) - (4)].symbol);
              q =  set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, q, LLNULL, s);
	   ;}
    break;

  case 951:

/* Line 1455 of yacc.c  */
#line 6799 "gram1.y"
    {  PTR_LLND q; 
              q =  set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	      (yyval.ll_node) = make_llnd(fi,ARRAY_REF, q, LLNULL, (yyvsp[(1) - (4)].symbol));
	   ;}
    break;

  case 952:

/* Line 1455 of yacc.c  */
#line 6806 "gram1.y"
    {              
         (yyval.bf_node) = get_bfnd(fi,DVM_ON_DIR,SMNULL,(yyvsp[(3) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),LLNULL);
    ;}
    break;

  case 953:

/* Line 1455 of yacc.c  */
#line 6812 "gram1.y"
    {(yyval.ll_node) = LLNULL;;}
    break;

  case 954:

/* Line 1455 of yacc.c  */
#line 6814 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 955:

/* Line 1455 of yacc.c  */
#line 6818 "gram1.y"
    {(yyval.bf_node) = get_bfnd(fi,DVM_END_ON_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 956:

/* Line 1455 of yacc.c  */
#line 6822 "gram1.y"
    { PTR_LLND q;
        /* if(!($6->attr & PROCESSORS_BIT))
           errstr("'%s' is not processor array", $6->ident, 67);
         */
        q = make_llnd(fi,ARRAY_REF, (yyvsp[(7) - (7)].ll_node), LLNULL, (yyvsp[(6) - (7)].symbol));
        (yyval.bf_node) = get_bfnd(fi,DVM_MAP_DIR,SMNULL,(yyvsp[(3) - (7)].ll_node),q,LLNULL);
      ;}
    break;

  case 957:

/* Line 1455 of yacc.c  */
#line 6830 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_MAP_DIR,SMNULL,(yyvsp[(3) - (6)].ll_node),LLNULL,(yyvsp[(6) - (6)].ll_node)); ;}
    break;

  case 958:

/* Line 1455 of yacc.c  */
#line 6834 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_PREFETCH_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 959:

/* Line 1455 of yacc.c  */
#line 6838 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_RESET_DIR,(yyvsp[(3) - (3)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 960:

/* Line 1455 of yacc.c  */
#line 6846 "gram1.y"
    { if(!((yyvsp[(4) - (7)].symbol)->attr & INDIRECT_BIT))
                         errstr("'%s' is not indirect group name", (yyvsp[(4) - (7)].symbol)->ident, 313);
                      (yyval.bf_node) = get_bfnd(fi,DVM_INDIRECT_ACCESS_DIR,(yyvsp[(4) - (7)].symbol),(yyvsp[(6) - (7)].ll_node),LLNULL,LLNULL);
                    ;}
    break;

  case 961:

/* Line 1455 of yacc.c  */
#line 6851 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_INDIRECT_ACCESS_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL);;}
    break;

  case 962:

/* Line 1455 of yacc.c  */
#line 6865 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 963:

/* Line 1455 of yacc.c  */
#line 6867 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 964:

/* Line 1455 of yacc.c  */
#line 6871 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 965:

/* Line 1455 of yacc.c  */
#line 6873 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node); (yyval.ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);;}
    break;

  case 966:

/* Line 1455 of yacc.c  */
#line 6882 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,HPF_INDEPENDENT_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 967:

/* Line 1455 of yacc.c  */
#line 6884 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,HPF_INDEPENDENT_DIR,SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);;}
    break;

  case 968:

/* Line 1455 of yacc.c  */
#line 6886 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,HPF_INDEPENDENT_DIR,SMNULL, LLNULL, (yyvsp[(3) - (3)].ll_node), LLNULL);;}
    break;

  case 969:

/* Line 1455 of yacc.c  */
#line 6888 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,HPF_INDEPENDENT_DIR,SMNULL, (yyvsp[(3) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node),LLNULL);;}
    break;

  case 970:

/* Line 1455 of yacc.c  */
#line 6924 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,REDUCTION_OP,(yyvsp[(5) - (6)].ll_node),LLNULL,SMNULL);;}
    break;

  case 971:

/* Line 1455 of yacc.c  */
#line 6928 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ASYNCHRONOUS_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 972:

/* Line 1455 of yacc.c  */
#line 6932 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ENDASYNCHRONOUS_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 973:

/* Line 1455 of yacc.c  */
#line 6936 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ASYNCWAIT_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 974:

/* Line 1455 of yacc.c  */
#line 6940 "gram1.y"
    {  
            if(((yyval.symbol)=(yyvsp[(1) - (1)].hash_entry)->id_attr) == SMNULL) {
                errstr("'%s' is not declared as ASYNCID", (yyvsp[(1) - (1)].hash_entry)->ident, 115);
                (yyval.symbol) = make_local_entity((yyvsp[(1) - (1)].hash_entry),ASYNC_ID,global_default,LOCAL);
            } else {
              if((yyval.symbol)->variant != ASYNC_ID)
                errstr("'%s' is not declared as ASYNCID", (yyvsp[(1) - (1)].hash_entry)->ident, 115);
            }
     ;}
    break;

  case 975:

/* Line 1455 of yacc.c  */
#line 6952 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,VAR_REF, LLNULL, LLNULL, (yyvsp[(1) - (1)].symbol));;}
    break;

  case 976:

/* Line 1455 of yacc.c  */
#line 6954 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ARRAY_REF, (yyvsp[(3) - (4)].ll_node), LLNULL, (yyvsp[(1) - (4)].symbol));;}
    break;

  case 977:

/* Line 1455 of yacc.c  */
#line 6958 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_F90_DIR,SMNULL,(yyvsp[(3) - (5)].ll_node),(yyvsp[(5) - (5)].ll_node),LLNULL);;}
    break;

  case 978:

/* Line 1455 of yacc.c  */
#line 6961 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_DEBUG_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 979:

/* Line 1455 of yacc.c  */
#line 6963 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_DEBUG_DIR,SMNULL,(yyvsp[(3) - (6)].ll_node),(yyvsp[(5) - (6)].ll_node),LLNULL);;}
    break;

  case 980:

/* Line 1455 of yacc.c  */
#line 6967 "gram1.y"
    { 
              (yyval.ll_node) = set_ll_list((yyvsp[(2) - (2)].ll_node), LLNULL, EXPR_LIST);
              endioctl();
            ;}
    break;

  case 981:

/* Line 1455 of yacc.c  */
#line 6972 "gram1.y"
    { 
              (yyval.ll_node) = set_ll_list((yyvsp[(1) - (4)].ll_node), (yyvsp[(4) - (4)].ll_node), EXPR_LIST);
              endioctl();
            ;}
    break;

  case 982:

/* Line 1455 of yacc.c  */
#line 6979 "gram1.y"
    { (yyval.ll_node)  = make_llnd(fi, KEYWORD_ARG, (yyvsp[(1) - (2)].ll_node), (yyvsp[(2) - (2)].ll_node), SMNULL); ;}
    break;

  case 983:

/* Line 1455 of yacc.c  */
#line 6982 "gram1.y"
    {
	         (yyval.ll_node) = make_llnd(fi,INT_VAL, LLNULL, LLNULL, SMNULL);
	         (yyval.ll_node)->entry.ival = atoi(yytext);
	         (yyval.ll_node)->type = global_int;
	        ;}
    break;

  case 984:

/* Line 1455 of yacc.c  */
#line 6990 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ENDDEBUG_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 985:

/* Line 1455 of yacc.c  */
#line 6994 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_INTERVAL_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 986:

/* Line 1455 of yacc.c  */
#line 6998 "gram1.y"
    { (yyval.ll_node) = LLNULL;;}
    break;

  case 987:

/* Line 1455 of yacc.c  */
#line 7001 "gram1.y"
    { if((yyvsp[(1) - (1)].ll_node)->type->variant != T_INT)             
                    err("Illegal interval number", 78);
                  (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);
                 ;}
    break;

  case 988:

/* Line 1455 of yacc.c  */
#line 7009 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_EXIT_INTERVAL_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 989:

/* Line 1455 of yacc.c  */
#line 7013 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_ENDINTERVAL_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 990:

/* Line 1455 of yacc.c  */
#line 7017 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_TRACEON_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 991:

/* Line 1455 of yacc.c  */
#line 7021 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_TRACEOFF_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 992:

/* Line 1455 of yacc.c  */
#line 7025 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_BARRIER_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 993:

/* Line 1455 of yacc.c  */
#line 7029 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CHECK_DIR,SMNULL,(yyvsp[(9) - (9)].ll_node),(yyvsp[(5) - (9)].ll_node),LLNULL); ;}
    break;

  case 994:

/* Line 1455 of yacc.c  */
#line 7033 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_IO_MODE_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL);;}
    break;

  case 995:

/* Line 1455 of yacc.c  */
#line 7036 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 996:

/* Line 1455 of yacc.c  */
#line 7038 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 997:

/* Line 1455 of yacc.c  */
#line 7042 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_ASYNC_OP,LLNULL,LLNULL,SMNULL);;}
    break;

  case 998:

/* Line 1455 of yacc.c  */
#line 7044 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_LOCAL_OP, LLNULL,LLNULL,SMNULL);;}
    break;

  case 999:

/* Line 1455 of yacc.c  */
#line 7046 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,PARALLEL_OP, LLNULL,LLNULL,SMNULL);;}
    break;

  case 1000:

/* Line 1455 of yacc.c  */
#line 7050 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_SHADOW_ADD_DIR,SMNULL,(yyvsp[(4) - (9)].ll_node),(yyvsp[(6) - (9)].ll_node),(yyvsp[(9) - (9)].ll_node)); ;}
    break;

  case 1001:

/* Line 1455 of yacc.c  */
#line 7054 "gram1.y"
    {
                 if((yyvsp[(1) - (4)].ll_node)->type->variant != T_ARRAY) 
                    errstr("'%s' isn't array", (yyvsp[(1) - (4)].ll_node)->entry.Template.symbol->ident, 66);
                 if(!((yyvsp[(1) - (4)].ll_node)->entry.Template.symbol->attr & TEMPLATE_BIT))
                    errstr("'%s' isn't TEMPLATE", (yyvsp[(1) - (4)].ll_node)->entry.Template.symbol->ident, 628);
                 (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
                 (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
                 /*$$->type = $1->type->entry.ar_decl.base_type;*/
               ;}
    break;

  case 1002:

/* Line 1455 of yacc.c  */
#line 7066 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1003:

/* Line 1455 of yacc.c  */
#line 7068 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1004:

/* Line 1455 of yacc.c  */
#line 7072 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 1005:

/* Line 1455 of yacc.c  */
#line 7074 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 1006:

/* Line 1455 of yacc.c  */
#line 7078 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (2)].ll_node);;}
    break;

  case 1007:

/* Line 1455 of yacc.c  */
#line 7080 "gram1.y"
    { (yyval.ll_node) = LLNULL; opt_kwd_ = NO;;}
    break;

  case 1008:

/* Line 1455 of yacc.c  */
#line 7084 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_LOCALIZE_DIR,SMNULL,(yyvsp[(4) - (7)].ll_node),(yyvsp[(6) - (7)].ll_node),LLNULL); ;}
    break;

  case 1009:

/* Line 1455 of yacc.c  */
#line 7088 "gram1.y"
    {
                 if((yyvsp[(1) - (1)].ll_node)->type->variant != T_ARRAY) 
                    errstr("'%s' isn't array", (yyvsp[(1) - (1)].ll_node)->entry.Template.symbol->ident, 66); 
                 (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);
                ;}
    break;

  case 1010:

/* Line 1455 of yacc.c  */
#line 7094 "gram1.y"
    {
                 if((yyvsp[(1) - (4)].ll_node)->type->variant != T_ARRAY) 
                    errstr("'%s' isn't array", (yyvsp[(1) - (4)].ll_node)->entry.Template.symbol->ident, 66); 
                                 
                 (yyvsp[(1) - (4)].ll_node)->entry.Template.ll_ptr1 = (yyvsp[(3) - (4)].ll_node);
                 (yyval.ll_node) = (yyvsp[(1) - (4)].ll_node);
                 (yyval.ll_node)->type = (yyvsp[(1) - (4)].ll_node)->type->entry.ar_decl.base_type;
                ;}
    break;

  case 1011:

/* Line 1455 of yacc.c  */
#line 7106 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1012:

/* Line 1455 of yacc.c  */
#line 7108 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1013:

/* Line 1455 of yacc.c  */
#line 7112 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 1014:

/* Line 1455 of yacc.c  */
#line 7114 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,DDOT, LLNULL, LLNULL, SMNULL);;}
    break;

  case 1015:

/* Line 1455 of yacc.c  */
#line 7118 "gram1.y"
    { 
            (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL, LLNULL, LLNULL, SMNULL);
            (yyval.ll_node)->entry.string_val = (char *) "*";
            (yyval.ll_node)->type = global_string;
          ;}
    break;

  case 1016:

/* Line 1455 of yacc.c  */
#line 7126 "gram1.y"
    { 
                PTR_LLND q;
                if((yyvsp[(16) - (16)].ll_node))
                  q = make_llnd(fi,ARRAY_OP, (yyvsp[(14) - (16)].ll_node), (yyvsp[(16) - (16)].ll_node), SMNULL);
                else
                  q = (yyvsp[(14) - (16)].ll_node);                  
                (yyval.bf_node) = get_bfnd(fi,DVM_CP_CREATE_DIR,SMNULL,(yyvsp[(3) - (16)].ll_node),(yyvsp[(8) - (16)].ll_node),q); 
              ;}
    break;

  case 1017:

/* Line 1455 of yacc.c  */
#line 7137 "gram1.y"
    { (yyval.ll_node) = LLNULL; ;}
    break;

  case 1018:

/* Line 1455 of yacc.c  */
#line 7139 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi, PARALLEL_OP, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 1019:

/* Line 1455 of yacc.c  */
#line 7141 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_LOCAL_OP, LLNULL, LLNULL, SMNULL); ;}
    break;

  case 1020:

/* Line 1455 of yacc.c  */
#line 7145 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CP_LOAD_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL); ;}
    break;

  case 1021:

/* Line 1455 of yacc.c  */
#line 7149 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CP_SAVE_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL); ;}
    break;

  case 1022:

/* Line 1455 of yacc.c  */
#line 7151 "gram1.y"
    {
                PTR_LLND q;
                q = make_llnd(fi,ACC_ASYNC_OP,LLNULL,LLNULL,SMNULL);
                (yyval.bf_node) = get_bfnd(fi,DVM_CP_SAVE_DIR,SMNULL,(yyvsp[(3) - (6)].ll_node),q,LLNULL);
              ;}
    break;

  case 1023:

/* Line 1455 of yacc.c  */
#line 7159 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_CP_WAIT_DIR,SMNULL,(yyvsp[(3) - (9)].ll_node),(yyvsp[(8) - (9)].ll_node),LLNULL); ;}
    break;

  case 1024:

/* Line 1455 of yacc.c  */
#line 7163 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_TEMPLATE_CREATE_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL); ;}
    break;

  case 1025:

/* Line 1455 of yacc.c  */
#line 7166 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 1026:

/* Line 1455 of yacc.c  */
#line 7168 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 1027:

/* Line 1455 of yacc.c  */
#line 7172 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,DVM_TEMPLATE_DELETE_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL); ;}
    break;

  case 1028:

/* Line 1455 of yacc.c  */
#line 7176 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node), LLNULL, EXPR_LIST); ;}
    break;

  case 1029:

/* Line 1455 of yacc.c  */
#line 7178 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node), (yyvsp[(3) - (3)].ll_node), EXPR_LIST); ;}
    break;

  case 1057:

/* Line 1455 of yacc.c  */
#line 7211 "gram1.y"
    {
          (yyval.bf_node) = get_bfnd(fi,OMP_ONETHREAD_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	;}
    break;

  case 1058:

/* Line 1455 of yacc.c  */
#line 7217 "gram1.y"
    {
  	   (yyval.bf_node) = make_endparallel();
	;}
    break;

  case 1059:

/* Line 1455 of yacc.c  */
#line 7223 "gram1.y"
    {
  	   (yyval.bf_node) = make_parallel();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
	   opt_kwd_ = NO;
	;}
    break;

  case 1060:

/* Line 1455 of yacc.c  */
#line 7229 "gram1.y"
    {
  	   (yyval.bf_node) = make_parallel();
	   opt_kwd_ = NO;
	;}
    break;

  case 1061:

/* Line 1455 of yacc.c  */
#line 7235 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1062:

/* Line 1455 of yacc.c  */
#line 7239 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);	
	;}
    break;

  case 1072:

/* Line 1455 of yacc.c  */
#line 7256 "gram1.y"
    {
		(yyval.ll_node) = (yyvsp[(4) - (5)].ll_node);
        ;}
    break;

  case 1073:

/* Line 1455 of yacc.c  */
#line 7261 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_PRIVATE,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1074:

/* Line 1455 of yacc.c  */
#line 7266 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_FIRSTPRIVATE,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1075:

/* Line 1455 of yacc.c  */
#line 7272 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_LASTPRIVATE,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1076:

/* Line 1455 of yacc.c  */
#line 7278 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_COPYIN,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1077:

/* Line 1455 of yacc.c  */
#line 7284 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_SHARED,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1078:

/* Line 1455 of yacc.c  */
#line 7289 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_DEFAULT,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1079:

/* Line 1455 of yacc.c  */
#line 7295 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "private";
		(yyval.ll_node)->type = global_string;
	;}
    break;

  case 1080:

/* Line 1455 of yacc.c  */
#line 7301 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "shared";
		(yyval.ll_node)->type = global_string;
	;}
    break;

  case 1081:

/* Line 1455 of yacc.c  */
#line 7307 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "none";
		(yyval.ll_node)->type = global_string;
	;}
    break;

  case 1082:

/* Line 1455 of yacc.c  */
#line 7314 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_IF,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1083:

/* Line 1455 of yacc.c  */
#line 7320 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_NUM_THREADS,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1084:

/* Line 1455 of yacc.c  */
#line 7326 "gram1.y"
    {
		PTR_LLND q;
		q = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
		(yyval.ll_node) = make_llnd(fi,OMP_REDUCTION,q,LLNULL,SMNULL);
	;}
    break;

  case 1085:

/* Line 1455 of yacc.c  */
#line 7333 "gram1.y"
    {(yyval.ll_node) = make_llnd(fi,DDOT,(yyvsp[(2) - (4)].ll_node),(yyvsp[(4) - (4)].ll_node),SMNULL);;}
    break;

  case 1087:

/* Line 1455 of yacc.c  */
#line 7339 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "+";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1088:

/* Line 1455 of yacc.c  */
#line 7345 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "-";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1089:

/* Line 1455 of yacc.c  */
#line 7352 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "*";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1090:

/* Line 1455 of yacc.c  */
#line 7358 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "/";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1091:

/* Line 1455 of yacc.c  */
#line 7364 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "min";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1092:

/* Line 1455 of yacc.c  */
#line 7370 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "max";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1093:

/* Line 1455 of yacc.c  */
#line 7376 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) ".or.";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1094:

/* Line 1455 of yacc.c  */
#line 7382 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) ".and.";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1095:

/* Line 1455 of yacc.c  */
#line 7388 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) ".eqv.";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1096:

/* Line 1455 of yacc.c  */
#line 7394 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) ".neqv.";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1097:

/* Line 1455 of yacc.c  */
#line 7400 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "iand";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1098:

/* Line 1455 of yacc.c  */
#line 7406 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "ieor";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1099:

/* Line 1455 of yacc.c  */
#line 7412 "gram1.y"
    {
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "ior";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1100:

/* Line 1455 of yacc.c  */
#line 7418 "gram1.y"
    { err("Illegal reduction operation name", 70);
               errcnt--;
              (yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
              (yyval.ll_node)->entry.string_val = (char *) "unknown";
              (yyval.ll_node)->type = global_string;
             ;}
    break;

  case 1101:

/* Line 1455 of yacc.c  */
#line 7428 "gram1.y"
    {
  	   (yyval.bf_node) = make_sections((yyvsp[(4) - (4)].ll_node));
	   opt_kwd_ = NO;
	;}
    break;

  case 1102:

/* Line 1455 of yacc.c  */
#line 7433 "gram1.y"
    {
  	   (yyval.bf_node) = make_sections(LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1103:

/* Line 1455 of yacc.c  */
#line 7439 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1104:

/* Line 1455 of yacc.c  */
#line 7443 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);
	;}
    break;

  case 1109:

/* Line 1455 of yacc.c  */
#line 7455 "gram1.y"
    {
		PTR_LLND q;
   	        (yyval.bf_node) = make_endsections();
		q = set_ll_list((yyvsp[(4) - (4)].ll_node),LLNULL,EXPR_LIST);
                (yyval.bf_node)->entry.Template.ll_ptr1 = q;
                opt_kwd_ = NO;
	;}
    break;

  case 1110:

/* Line 1455 of yacc.c  */
#line 7463 "gram1.y"
    {
   	        (yyval.bf_node) = make_endsections();
	        opt_kwd_ = NO; 
	;}
    break;

  case 1111:

/* Line 1455 of yacc.c  */
#line 7469 "gram1.y"
    {
           (yyval.bf_node) = make_ompsection();
	;}
    break;

  case 1112:

/* Line 1455 of yacc.c  */
#line 7475 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_DO_DIR,SMNULL,(yyvsp[(4) - (4)].ll_node),LLNULL,LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1113:

/* Line 1455 of yacc.c  */
#line 7480 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_DO_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1114:

/* Line 1455 of yacc.c  */
#line 7486 "gram1.y"
    {
		PTR_LLND q;
		q = set_ll_list((yyvsp[(4) - (4)].ll_node),LLNULL,EXPR_LIST);
	        (yyval.bf_node) = get_bfnd(fi,OMP_END_DO_DIR,SMNULL,q,LLNULL,LLNULL);
      	        opt_kwd_ = NO;
	;}
    break;

  case 1115:

/* Line 1455 of yacc.c  */
#line 7493 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_END_DO_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1116:

/* Line 1455 of yacc.c  */
#line 7499 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1117:

/* Line 1455 of yacc.c  */
#line 7503 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);
	;}
    break;

  case 1124:

/* Line 1455 of yacc.c  */
#line 7517 "gram1.y"
    {
		/*$$ = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		$$->entry.string_val = (char *) "ORDERED";
		$$->type = global_string;*/
                (yyval.ll_node) = make_llnd(fi,OMP_ORDERED,LLNULL,LLNULL,SMNULL);
	;}
    break;

  case 1125:

/* Line 1455 of yacc.c  */
#line 7526 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_SCHEDULE,(yyvsp[(4) - (7)].ll_node),(yyvsp[(6) - (7)].ll_node),SMNULL);
	;}
    break;

  case 1126:

/* Line 1455 of yacc.c  */
#line 7530 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_SCHEDULE,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1127:

/* Line 1455 of yacc.c  */
#line 7536 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "STATIC";
		(yyval.ll_node)->type = global_string;
		
	;}
    break;

  case 1128:

/* Line 1455 of yacc.c  */
#line 7543 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "DYNAMIC";
		(yyval.ll_node)->type = global_string;
		
	;}
    break;

  case 1129:

/* Line 1455 of yacc.c  */
#line 7550 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "GUIDED";
		(yyval.ll_node)->type = global_string;
		
	;}
    break;

  case 1130:

/* Line 1455 of yacc.c  */
#line 7557 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,KEYWORD_VAL,LLNULL,LLNULL,SMNULL);
		(yyval.ll_node)->entry.string_val = (char *) "RUNTIME";
		(yyval.ll_node)->type = global_string;
		
	;}
    break;

  case 1131:

/* Line 1455 of yacc.c  */
#line 7566 "gram1.y"
    {
  	   (yyval.bf_node) = make_single();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
	   opt_kwd_ = NO;
	;}
    break;

  case 1132:

/* Line 1455 of yacc.c  */
#line 7572 "gram1.y"
    {
  	   (yyval.bf_node) = make_single();
	   opt_kwd_ = NO;
	;}
    break;

  case 1133:

/* Line 1455 of yacc.c  */
#line 7578 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1134:

/* Line 1455 of yacc.c  */
#line 7582 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);
	;}
    break;

  case 1137:

/* Line 1455 of yacc.c  */
#line 7592 "gram1.y"
    {
  	   (yyval.bf_node) = make_endsingle();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
	   opt_kwd_ = NO;
	;}
    break;

  case 1138:

/* Line 1455 of yacc.c  */
#line 7598 "gram1.y"
    {
  	   (yyval.bf_node) = make_endsingle();
	   opt_kwd_ = NO;
	;}
    break;

  case 1139:

/* Line 1455 of yacc.c  */
#line 7604 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1140:

/* Line 1455 of yacc.c  */
#line 7608 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);
	;}
    break;

  case 1143:

/* Line 1455 of yacc.c  */
#line 7619 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_COPYPRIVATE,(yyvsp[(2) - (2)].ll_node),LLNULL,SMNULL);
	;}
    break;

  case 1144:

/* Line 1455 of yacc.c  */
#line 7625 "gram1.y"
    {
		(yyval.ll_node) = make_llnd(fi,OMP_NOWAIT,LLNULL,LLNULL,SMNULL);
	;}
    break;

  case 1145:

/* Line 1455 of yacc.c  */
#line 7631 "gram1.y"
    {
           (yyval.bf_node) = make_workshare();
	;}
    break;

  case 1146:

/* Line 1455 of yacc.c  */
#line 7636 "gram1.y"
    {
		PTR_LLND q;
   	        (yyval.bf_node) = make_endworkshare();
		q = set_ll_list((yyvsp[(4) - (4)].ll_node),LLNULL,EXPR_LIST);
                (yyval.bf_node)->entry.Template.ll_ptr1 = q;
  	        opt_kwd_ = NO;
	;}
    break;

  case 1147:

/* Line 1455 of yacc.c  */
#line 7644 "gram1.y"
    {
   	        (yyval.bf_node) = make_endworkshare();
                opt_kwd_ = NO;
	;}
    break;

  case 1148:

/* Line 1455 of yacc.c  */
#line 7650 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_PARALLEL_DO_DIR,SMNULL,(yyvsp[(4) - (4)].ll_node),LLNULL,LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1149:

/* Line 1455 of yacc.c  */
#line 7655 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_PARALLEL_DO_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1150:

/* Line 1455 of yacc.c  */
#line 7662 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(3) - (4)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1151:

/* Line 1455 of yacc.c  */
#line 7666 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (5)].ll_node),(yyvsp[(4) - (5)].ll_node),EXPR_LIST);
	;}
    break;

  case 1163:

/* Line 1455 of yacc.c  */
#line 7686 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_END_PARALLEL_DO_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	;}
    break;

  case 1164:

/* Line 1455 of yacc.c  */
#line 7691 "gram1.y"
    {
           (yyval.bf_node) = make_parallelsections((yyvsp[(4) - (4)].ll_node));
	   opt_kwd_ = NO;
	;}
    break;

  case 1165:

/* Line 1455 of yacc.c  */
#line 7696 "gram1.y"
    {
           (yyval.bf_node) = make_parallelsections(LLNULL);
	   opt_kwd_ = NO;
	;}
    break;

  case 1166:

/* Line 1455 of yacc.c  */
#line 7703 "gram1.y"
    {
           (yyval.bf_node) = make_endparallelsections();
	;}
    break;

  case 1167:

/* Line 1455 of yacc.c  */
#line 7708 "gram1.y"
    {
           (yyval.bf_node) = make_parallelworkshare();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (4)].ll_node);
	   opt_kwd_ = NO;
	;}
    break;

  case 1168:

/* Line 1455 of yacc.c  */
#line 7714 "gram1.y"
    {
           (yyval.bf_node) = make_parallelworkshare();
	   opt_kwd_ = NO;
	;}
    break;

  case 1169:

/* Line 1455 of yacc.c  */
#line 7720 "gram1.y"
    {
           (yyval.bf_node) = make_endparallelworkshare();
	;}
    break;

  case 1170:

/* Line 1455 of yacc.c  */
#line 7725 "gram1.y"
    { 
	   (yyval.bf_node) = get_bfnd(fi,OMP_THREADPRIVATE_DIR, SMNULL, (yyvsp[(3) - (3)].ll_node), LLNULL, LLNULL);
	;}
    break;

  case 1171:

/* Line 1455 of yacc.c  */
#line 7730 "gram1.y"
    {
  	   (yyval.bf_node) = make_master();
	;}
    break;

  case 1172:

/* Line 1455 of yacc.c  */
#line 7735 "gram1.y"
    {
  	   (yyval.bf_node) = make_endmaster();
	;}
    break;

  case 1173:

/* Line 1455 of yacc.c  */
#line 7739 "gram1.y"
    {
  	   (yyval.bf_node) = make_ordered();
	;}
    break;

  case 1174:

/* Line 1455 of yacc.c  */
#line 7744 "gram1.y"
    {
  	   (yyval.bf_node) = make_endordered();
	;}
    break;

  case 1175:

/* Line 1455 of yacc.c  */
#line 7749 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_BARRIER_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	;}
    break;

  case 1176:

/* Line 1455 of yacc.c  */
#line 7753 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_ATOMIC_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	;}
    break;

  case 1177:

/* Line 1455 of yacc.c  */
#line 7758 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_FLUSH_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);
	;}
    break;

  case 1178:

/* Line 1455 of yacc.c  */
#line 7762 "gram1.y"
    {
           (yyval.bf_node) = get_bfnd(fi,OMP_FLUSH_DIR,SMNULL,LLNULL,LLNULL,LLNULL);
	;}
    break;

  case 1179:

/* Line 1455 of yacc.c  */
#line 7768 "gram1.y"
    {
  	   (yyval.bf_node) = make_critical();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
	;}
    break;

  case 1180:

/* Line 1455 of yacc.c  */
#line 7773 "gram1.y"
    {
  	   (yyval.bf_node) = make_critical();
	;}
    break;

  case 1181:

/* Line 1455 of yacc.c  */
#line 7779 "gram1.y"
    {
  	   (yyval.bf_node) = make_endcritical();
           (yyval.bf_node)->entry.Template.ll_ptr1 = (yyvsp[(4) - (5)].ll_node);
	;}
    break;

  case 1182:

/* Line 1455 of yacc.c  */
#line 7784 "gram1.y"
    {
  	   (yyval.bf_node) = make_endcritical();
	;}
    break;

  case 1183:

/* Line 1455 of yacc.c  */
#line 7790 "gram1.y"
    { 
		PTR_SYMB s;
		PTR_LLND l;
		s = make_common((yyvsp[(2) - (5)].hash_entry)); 
		l = make_llnd(fi,VAR_REF, LLNULL, LLNULL, s);
		(yyval.ll_node) = make_llnd(fi,OMP_THREADPRIVATE, l, LLNULL, SMNULL);
	;}
    break;

  case 1184:

/* Line 1455 of yacc.c  */
#line 7800 "gram1.y"
    {
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1185:

/* Line 1455 of yacc.c  */
#line 7804 "gram1.y"
    {	
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST);
	;}
    break;

  case 1186:

/* Line 1455 of yacc.c  */
#line 7808 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);
	;}
    break;

  case 1187:

/* Line 1455 of yacc.c  */
#line 7812 "gram1.y"
    { 
		(yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST);
	;}
    break;

  case 1188:

/* Line 1455 of yacc.c  */
#line 7817 "gram1.y"
    {
		operator_slash = 1;
	;}
    break;

  case 1189:

/* Line 1455 of yacc.c  */
#line 7820 "gram1.y"
    {
		operator_slash = 0;
	;}
    break;

  case 1196:

/* Line 1455 of yacc.c  */
#line 7833 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_REGION_DIR,SMNULL,(yyvsp[(3) - (3)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1197:

/* Line 1455 of yacc.c  */
#line 7837 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_CHECKSECTION_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1198:

/* Line 1455 of yacc.c  */
#line 7841 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_GET_ACTUAL_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1199:

/* Line 1455 of yacc.c  */
#line 7843 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_GET_ACTUAL_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1200:

/* Line 1455 of yacc.c  */
#line 7845 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_GET_ACTUAL_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1201:

/* Line 1455 of yacc.c  */
#line 7849 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_ACTUAL_DIR,SMNULL,(yyvsp[(4) - (5)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1202:

/* Line 1455 of yacc.c  */
#line 7851 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_ACTUAL_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1203:

/* Line 1455 of yacc.c  */
#line 7853 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_ACTUAL_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1204:

/* Line 1455 of yacc.c  */
#line 7857 "gram1.y"
    { (yyval.ll_node) = LLNULL;;}
    break;

  case 1205:

/* Line 1455 of yacc.c  */
#line 7859 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node); ;}
    break;

  case 1206:

/* Line 1455 of yacc.c  */
#line 7863 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1207:

/* Line 1455 of yacc.c  */
#line 7865 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1208:

/* Line 1455 of yacc.c  */
#line 7869 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (2)].ll_node);;}
    break;

  case 1209:

/* Line 1455 of yacc.c  */
#line 7872 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (2)].ll_node);;}
    break;

  case 1210:

/* Line 1455 of yacc.c  */
#line 7875 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(2) - (2)].ll_node);;}
    break;

  case 1211:

/* Line 1455 of yacc.c  */
#line 7880 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_INOUT_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1212:

/* Line 1455 of yacc.c  */
#line 7882 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_IN_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1213:

/* Line 1455 of yacc.c  */
#line 7884 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_OUT_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1214:

/* Line 1455 of yacc.c  */
#line 7886 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_LOCAL_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1215:

/* Line 1455 of yacc.c  */
#line 7888 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_INLOCAL_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1216:

/* Line 1455 of yacc.c  */
#line 7892 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_TARGETS_OP,(yyvsp[(3) - (4)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1217:

/* Line 1455 of yacc.c  */
#line 7896 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_ASYNC_OP,LLNULL,LLNULL,SMNULL);;}
    break;

  case 1218:

/* Line 1455 of yacc.c  */
#line 7901 "gram1.y"
    { (yyval.ll_node) = (yyvsp[(1) - (1)].ll_node);;}
    break;

  case 1219:

/* Line 1455 of yacc.c  */
#line 7905 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1220:

/* Line 1455 of yacc.c  */
#line 7907 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1221:

/* Line 1455 of yacc.c  */
#line 7911 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_HOST_OP, LLNULL,LLNULL,SMNULL);;}
    break;

  case 1222:

/* Line 1455 of yacc.c  */
#line 7913 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_CUDA_OP, LLNULL,LLNULL,SMNULL);;}
    break;

  case 1223:

/* Line 1455 of yacc.c  */
#line 7917 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_END_REGION_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1224:

/* Line 1455 of yacc.c  */
#line 7921 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,ACC_END_CHECKSECTION_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1230:

/* Line 1455 of yacc.c  */
#line 7932 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,SPF_ANALYSIS_DIR,SMNULL,(yyvsp[(3) - (4)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1231:

/* Line 1455 of yacc.c  */
#line 7936 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,SPF_PARALLEL_DIR,SMNULL,(yyvsp[(3) - (4)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1232:

/* Line 1455 of yacc.c  */
#line 7940 "gram1.y"
    {  (yyval.bf_node) = get_bfnd(fi,SPF_TRANSFORM_DIR,SMNULL,(yyvsp[(3) - (4)].ll_node),LLNULL,LLNULL);;}
    break;

  case 1233:

/* Line 1455 of yacc.c  */
#line 7944 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,SPF_PARALLEL_REG_DIR,(yyvsp[(2) - (2)].symbol),LLNULL,LLNULL,LLNULL);;}
    break;

  case 1234:

/* Line 1455 of yacc.c  */
#line 7948 "gram1.y"
    { (yyval.bf_node) = get_bfnd(fi,SPF_END_PARALLEL_REG_DIR,SMNULL,LLNULL,LLNULL,LLNULL);;}
    break;

  case 1235:

/* Line 1455 of yacc.c  */
#line 7952 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1236:

/* Line 1455 of yacc.c  */
#line 7954 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1239:

/* Line 1455 of yacc.c  */
#line 7962 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,REDUCTION_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL); ;}
    break;

  case 1240:

/* Line 1455 of yacc.c  */
#line 7966 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACC_PRIVATE_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1241:

/* Line 1455 of yacc.c  */
#line 7970 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1242:

/* Line 1455 of yacc.c  */
#line 7972 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1246:

/* Line 1455 of yacc.c  */
#line 7982 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SHADOW_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1247:

/* Line 1455 of yacc.c  */
#line 7986 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,ACROSS_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1248:

/* Line 1455 of yacc.c  */
#line 7990 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,REMOTE_ACCESS_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1249:

/* Line 1455 of yacc.c  */
#line 7994 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (1)].ll_node),LLNULL,EXPR_LIST); ;}
    break;

  case 1250:

/* Line 1455 of yacc.c  */
#line 7996 "gram1.y"
    { (yyval.ll_node) = set_ll_list((yyvsp[(1) - (3)].ll_node),(yyvsp[(3) - (3)].ll_node),EXPR_LIST); ;}
    break;

  case 1251:

/* Line 1455 of yacc.c  */
#line 8000 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SPF_NOINLINE_OP,LLNULL,LLNULL,SMNULL);;}
    break;

  case 1252:

/* Line 1455 of yacc.c  */
#line 8002 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SPF_FISSION_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1253:

/* Line 1455 of yacc.c  */
#line 8004 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SPF_PRIVATES_EXPANSION_OP,LLNULL,LLNULL,SMNULL);;}
    break;

  case 1254:

/* Line 1455 of yacc.c  */
#line 8006 "gram1.y"
    { (yyval.ll_node) = make_llnd(fi,SPF_PRIVATES_EXPANSION_OP,(yyvsp[(4) - (5)].ll_node),LLNULL,SMNULL);;}
    break;

  case 1255:

/* Line 1455 of yacc.c  */
#line 8009 "gram1.y"
    { (yyval.symbol) = make_parallel_region((yyvsp[(1) - (1)].hash_entry));;}
    break;



/* Line 1455 of yacc.c  */
#line 15503 "gram1.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



