
/*  A Bison parser, made from annotate.y with Bison version GNU Bison version 1.22
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IFDEFA	258
#define	APPLYTO	259
#define	ALABELT	260
#define	SECTIONT	261
#define	SPECIALAF	262
#define	FROMT	263
#define	TOT	264
#define	TOTLABEL	265
#define	TOFUNCTION	266
#define	DefineANN	267
#define	IDENTIFIER	268
#define	TYPENAME	269
#define	SCSPEC	270
#define	TYPESPEC	271
#define	TYPEMOD	272
#define	CONSTANT	273
#define	STRING	274
#define	ELLIPSIS	275
#define	SIZEOF	276
#define	ENUM	277
#define	STRUCT	278
#define	UNION	279
#define	IF	280
#define	ELSE	281
#define	WHILE	282
#define	DO	283
#define	FOR	284
#define	SWITCH	285
#define	CASE	286
#define	DEFAULT_TOKEN	287
#define	BREAK	288
#define	CONTINUE	289
#define	RETURN	290
#define	GOTO	291
#define	ASM	292
#define	CLASS	293
#define	PUBLIC	294
#define	FRIEND	295
#define	ACCESSWORD	296
#define	OVERLOAD	297
#define	OPERATOR	298
#define	COBREAK	299
#define	COLOOP	300
#define	COEXEC	301
#define	LOADEDOPR	302
#define	MULTIPLEID	303
#define	MULTIPLETYPENAME	304
#define	ASSIGN	305
#define	OROR	306
#define	ANDAND	307
#define	EQCOMPARE	308
#define	ARITHCOMPARE	309
#define	LSHIFT	310
#define	RSHIFT	311
#define	UNARY	312
#define	PLUSPLUS	313
#define	MINUSMINUS	314
#define	HYPERUNARY	315
#define	DOUBLEMARK	316
#define	POINTSAT	317

#line 5 "annotate.y"

#include "macro.h"

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
 
#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

#ifdef _NEEDALLOCAH_
#  include <alloca.h>
#endif
 
#define ON 1
#define OFF 0
#define OTHER 2
#define ID_ONLY  1
#define RANGE_APPEAR 2
#define EXCEPTION_ON 4
#define EXPR_LR      8
#define VECTOR_CONST_APPEAR 16
#define ARRAY_OP_NEED 32
#define TRACEON 0

extern POINTER newNode();


#line 35 "annotate.y"
typedef union { 
         int       token ;
         char      charv ;
         char      *charp;
         PTR_BFND   bfnode ;
         PTR_LLND   ll_node ;
         PTR_SYMB   symbol  ;
         PTR_TYPE   data_type ;
         PTR_HASH   hash_entry ;
         PTR_LABEL  label ;        
         PTR_BLOB   blob_ptr ;
       } YYSTYPE;
#line 151 "annotate.y"
 char      *input_filename;	
   extern    int lastdecl_id;
   PTR_LLND ANNOTATE_NODE = NULL;
   PTR_BFND ANNOTATIONSCOPE = NULL;
   extern PTR_SYMB newSymbol();
   extern PTR_LLND newExpr();
   extern PTR_LLND makeInt();
   static int cur_counter =  0; 
   static int primary_flag=  0;
   PTR_TYPE global_int_annotation = NULL;
   extern PTR_LLND Follow_Llnd();
   static int recursive_yylex = OFF;
   static int exception_flag = 0;
   static PTR_HASH cur_id_entry;
   int line_pos_1 = 0;
   char *line_pos_fname = 0;
   static int old_line = 0;
   static int yylineno=0;
   static int yyerror();
   PTR_CMNT cur_comment = NULL;
   PTR_CMNT new_cur_comment = NULL ;
   PTR_HASH look_up();
   PTR_HASH look_up_type();
   char *STRINGTOPARSE = 0;
   int PTTOSTRINGTOPARSE = 0;
   int LENSTRINGTOPARSE = 0;
   extern PTR_LLND Make_Function_Call();
   static PTR_LLND check_array_id_format();
   static PTR_LLND look_up_section();
   extern PTR_SYMB getSymbolWithName(); /*getSymbolWithName(name, scope)*/
   PTR_SYMB Look_For_Symbol_Ann();
   char AnnExTensionNumber[255]; /* to symbole right for the annotation  */
   static int Recog_My_Token();
   static int look_up_specialfunction();
   static unMYGETC();
   static MYGETC();
   static int map_assgn_op();

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		211
#define	YYFLAG		-32768
#define	YYNTBASE	85

#define YYTRANSLATE(x) ((unsigned)(x) <= 317 ? yytranslate[x] : 114)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    83,     2,    84,     2,    70,    59,     2,    81,
    82,    68,    66,    50,    67,    77,    69,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    54,    79,    63,
    51,    62,    53,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    78,     2,    80,    58,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    57,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    52,    55,    56,    60,    61,    64,
    65,    71,    72,    73,    74,    75,    76
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,    10,    14,    15,    20,    21,    26,    27,    32,
    39,    41,    44,    49,    52,    55,    56,    58,    59,    64,
    69,    76,    77,    79,    83,    87,    88,    91,    93,    97,
    99,   101,   103,   105,   107,   108,   110,   112,   116,   119,
   123,   124,   126,   130,   132,   134,   136,   138,   140,   142,
   148,   152,   156,   157,   163,   167,   168,   170,   174,   176,
   178,   180,   183,   186,   190,   194,   198,   202,   206,   210,
   214,   218,   222,   226,   230,   234,   238,   242,   248,   252,
   256,   258,   261,   264,   268,   272,   276,   280,   284,   288,
   292,   296,   300,   304,   308,   312,   316,   320,   324,   328,
   334,   338,   342,   344,   346,   348,   352,   356,   358,   359,
   365,   370,   373,   376,   378,   382,   386,   389,   392,   394
};

static const short yyrhs[] = {    -1,
    78,    86,    87,    88,    90,    79,    91,    80,     0,    78,
    91,    80,     0,     0,     3,    81,   113,    82,     0,     0,
     5,    81,   113,    82,     0,     0,     4,    81,    89,    82,
     0,     4,    81,    89,    82,    25,    96,     0,     6,     0,
    11,    13,     0,     8,   113,     9,   113,     0,     9,   113,
     0,    10,   113,     0,     0,    92,     0,     0,     7,    81,
    97,    82,     0,    13,    81,    97,    82,     0,    12,    81,
   113,    50,    18,    82,     0,     0,    93,     0,    92,    50,
    93,     0,    16,    13,    94,     0,     0,    51,   108,     0,
    13,     0,     0,    50,    13,     0,    13,     0,    14,     0,
    67,     0,    83,     0,    98,     0,     0,    98,     0,   108,
     0,    98,    50,   108,     0,    78,    80,     0,    78,   100,
    80,     0,     0,   101,     0,   100,    50,   101,     0,   109,
     0,   103,     0,   104,     0,    99,     0,    18,     0,    13,
     0,   102,    54,   102,    54,   102,     0,   102,    54,   102,
     0,    18,    84,    18,     0,     0,   106,    54,   106,    54,
   106,     0,   106,    54,   106,     0,     0,   108,     0,   108,
    84,   108,     0,   108,     0,   105,     0,   110,     0,    95,
   110,     0,    21,   108,     0,   108,    66,   108,     0,   108,
    67,   108,     0,   108,    68,   108,     0,   108,    69,   108,
     0,   108,    70,   108,     0,   108,    61,   108,     0,   108,
    63,   108,     0,   108,    62,   108,     0,   108,    60,   108,
     0,   108,    59,   108,     0,   108,    57,   108,     0,   108,
    58,   108,     0,   108,    56,   108,     0,   108,    55,   108,
     0,   108,    53,   108,    54,   108,     0,   108,    51,   108,
     0,   108,    52,   108,     0,   112,     0,    95,   109,     0,
    21,   109,     0,   109,    66,   109,     0,   109,    67,   109,
     0,   109,    68,   109,     0,   109,    69,   109,     0,   109,
    70,   109,     0,   109,    64,   109,     0,   109,    65,   109,
     0,   109,    61,   109,     0,   109,    63,   109,     0,   109,
    62,   109,     0,   109,    60,   109,     0,   109,    59,   109,
     0,   109,    57,   109,     0,   109,    58,   109,     0,   109,
    56,   109,     0,   109,    55,   109,     0,   109,    53,    96,
    54,   109,     0,   109,    51,   109,     0,   109,    52,   109,
     0,    13,     0,    18,     0,   113,     0,    81,    96,    82,
     0,    81,     1,    82,     0,    99,     0,     0,   110,    81,
   111,    97,    82,     0,   110,    78,   107,    80,     0,   110,
    72,     0,   110,    73,     0,    18,     0,    81,   109,    82,
     0,    81,     1,    82,     0,   112,    72,     0,   112,    73,
     0,    19,     0,   113,    19,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   192,   193,   203,   214,   218,   227,   231,   241,   245,   253,
   262,   266,   271,   276,   281,   288,   293,   300,   305,   312,
   319,   330,   334,   339,   348,   367,   371,   380,   387,   393,
   396,   400,   404,   411,   417,   422,   429,   434,   444,   451,
   460,   464,   468,   479,   484,   488,   492,   499,   504,   511,
   519,   526,   534,   538,   544,   551,   555,   561,   566,   567,
   570,   580,   584,   588,   592,   596,   600,   604,   608,   613,
   617,   621,   626,   630,   634,   638,   642,   646,   651,   655,
   663,   671,   675,   679,   683,   687,   691,   695,   699,   703,
   707,   712,   716,   721,   727,   731,   735,   739,   743,   747,
   752,   756,   766,   773,   777,   781,   787,   791,   795,   810,
   851,   875,   880,   891,   897,   903,   907,   911,   918,   923
};

static const char * const yytname[] = {   "$","error","$illegal.","IFDEFA","APPLYTO",
"ALABELT","SECTIONT","SPECIALAF","FROMT","TOT","TOTLABEL","TOFUNCTION","DefineANN",
"IDENTIFIER","TYPENAME","SCSPEC","TYPESPEC","TYPEMOD","CONSTANT","STRING","ELLIPSIS",
"SIZEOF","ENUM","STRUCT","UNION","IF","ELSE","WHILE","DO","FOR","SWITCH","CASE",
"DEFAULT_TOKEN","BREAK","CONTINUE","RETURN","GOTO","ASM","CLASS","PUBLIC","FRIEND",
"ACCESSWORD","OVERLOAD","OPERATOR","COBREAK","COLOOP","COEXEC","LOADEDOPR","MULTIPLEID",
"MULTIPLETYPENAME","','","'='","ASSIGN","'?'","':'","OROR","ANDAND","'|'","'^'",
"'&'","EQCOMPARE","ARITHCOMPARE","'>'","'<'","LSHIFT","RSHIFT","'+'","'-'","'*'",
"'/'","'%'","UNARY","PLUSPLUS","MINUSMINUS","HYPERUNARY","DOUBLEMARK","POINTSAT",
"'.'","'['","';'","']'","'('","')'","'!'","'#'","annotation","IfDefR","Alabel",
"ApplyTo","section","LocalDeclare","Expression_List","declare_local_list","onedeclare",
"domain","unop","expr","exprlist","nonnull_exprlist","vector_constant","vector_list",
"single_v_expr","element","triplet","compound_constant","array_expr_a","expr_no_commas_1",
"expr_vector","expr_no_commas","const_expr_no_commas","primary","@1","const_primary",
"string","@1"
};
#endif

static const short yyr1[] = {     0,
    85,    85,    85,    86,    86,    87,    87,    88,    88,    88,
    89,    89,    89,    89,    89,    90,    90,    91,    91,    91,
    91,    92,    92,    92,    93,    94,    94,    -1,    -1,    -1,
    -1,    95,    95,    96,    97,    97,    98,    98,    99,    99,
   100,   100,   100,   101,   101,   101,   101,   102,   102,   103,
   103,   104,   105,   105,   105,   106,   106,    -1,   107,   107,
   108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
   108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
   109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
   109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
   109,   109,   110,   110,   110,   110,   110,   110,   111,   110,
   110,   110,   110,   112,   112,   112,   112,   112,   113,   113
};

static const short yyr2[] = {     0,
     0,     8,     3,     0,     4,     0,     4,     0,     4,     6,
     1,     2,     4,     2,     2,     0,     1,     0,     4,     4,
     6,     0,     1,     3,     3,     0,     2,     1,     3,     1,
     1,     1,     1,     1,     0,     1,     1,     3,     2,     3,
     0,     1,     3,     1,     1,     1,     1,     1,     1,     5,
     3,     3,     0,     5,     3,     0,     1,     3,     1,     1,
     1,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     5,     3,     3,
     1,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
     3,     3,     1,     1,     1,     3,     3,     1,     0,     5,
     4,     2,     2,     1,     3,     3,     2,     2,     1,     2
};

static const short yydefact[] = {     1,
     4,     0,     0,     0,     0,     6,     0,     0,    35,     0,
    35,     0,     8,     3,   119,     0,   103,   104,     0,    32,
    41,     0,    33,     0,     0,    36,   108,    37,    61,   105,
     0,     0,     0,     0,    16,   120,     5,    63,    49,   114,
     0,    39,     0,     0,    47,     0,    42,     0,    45,    46,
    44,    81,     0,     0,    34,    62,    19,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   112,   113,    53,   109,     0,
    20,     0,     0,     0,     0,    17,    23,     0,   114,    83,
     0,     0,    82,     0,    40,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   117,   118,   107,   106,    38,
    79,    80,     0,    77,    76,    74,    75,    73,    72,    69,
    71,    70,    64,    65,    66,    67,    68,    60,     0,     0,
    57,    35,     0,     7,    11,     0,     0,     0,     0,     0,
    26,    18,     0,    52,   116,   115,    43,    48,    51,   101,
   102,     0,    99,    98,    96,    97,    95,    94,    91,    93,
    92,    89,    90,    84,    85,    86,    87,    88,     0,    56,
   111,     0,    21,     0,    14,    15,    12,     9,     0,    25,
     0,    24,     0,     0,    78,    55,    57,   110,     0,     0,
    27,     2,    50,   100,    56,    13,    10,    54,     0,     0,
     0
};

static const short yydefgoto[] = {   209,
     6,    13,    35,   150,    85,     7,    86,    87,   190,    24,
    54,    25,    26,    27,    46,    47,    48,    49,    50,   138,
   139,   140,    28,    51,    29,   142,    52,    30
};

static const short yypact[] = {   -55,
    61,   -51,   -50,   -43,   -16,    72,     4,    84,   155,    84,
   155,    24,   104,-32768,-32768,   -10,-32768,-32768,   155,-32768,
   164,   133,-32768,    -3,    35,    86,-32768,   295,    29,   118,
    13,    60,    84,    63,     8,-32768,-32768,-32768,-32768,   -17,
   168,-32768,   142,   168,-32768,   -14,-32768,    93,-32768,-32768,
   255,   -53,    66,    67,    86,    29,-32768,   155,   155,   155,
   155,   155,   155,   155,   155,   155,   155,   155,   155,   155,
   155,   155,   155,   155,   155,-32768,-32768,   151,-32768,   147,
-32768,    -6,   103,   153,    88,   125,-32768,   160,-32768,-32768,
    98,   201,-32768,   132,-32768,     9,   168,   168,   155,   168,
   168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
   168,   168,   168,   168,   168,-32768,-32768,-32768,-32768,   295,
   295,   333,   275,   399,   427,   453,   477,   499,   519,    89,
    89,    89,   -35,   -35,-32768,-32768,-32768,-32768,   129,   108,
   229,   155,   102,-32768,-32768,    84,    84,    84,   177,   119,
   152,     5,   186,-32768,-32768,-32768,-32768,-32768,   150,   255,
   314,   154,   384,   413,   440,   465,   488,   509,   128,   128,
   128,   206,   206,     1,     1,-32768,-32768,-32768,   155,   155,
-32768,   124,-32768,     2,   118,   118,-32768,   182,   155,-32768,
   137,-32768,     9,   168,   369,   165,   295,-32768,    84,   155,
   295,-32768,-32768,   351,   155,   118,-32768,-32768,   220,   221,
-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,    74,-32768,    71,-32768,   -15,
   -94,    -7,   -19,   -13,-32768,   134,   -89,-32768,-32768,-32768,
  -166,-32768,   -18,    18,   203,-32768,-32768,    -8
};


#define	YYLAST		589


static const short yytable[] = {    16,
    38,    31,    55,    32,   162,    44,   159,    45,    36,    17,
   199,     3,    36,   196,    18,    15,     4,     5,   116,   117,
    36,    39,     1,    84,    82,    44,   158,    44,    44,     8,
     9,    36,    73,    74,    75,    94,   -48,    10,   208,   120,
   121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   -22,    90,   141,
    92,    93,    80,     2,    11,    95,    88,     3,   113,   114,
   115,    37,     4,     5,    21,   144,    12,    22,    44,    55,
    45,    44,    44,    14,    44,    44,    44,    44,    44,    44,
    44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
    76,    77,    15,   203,    33,   207,    78,    34,   145,    79,
   146,   147,   148,   149,   160,   161,    57,   163,   164,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   176,   177,   178,    53,   182,    58,    36,   184,   185,   186,
   -18,    81,    91,    83,    39,    17,    96,   118,   119,    40,
    18,    15,    41,    19,    71,    72,    73,    74,    75,    89,
   195,   197,    41,    17,   143,   151,   152,    17,    18,    15,
   201,    19,    18,    15,   153,    19,    39,   154,    44,   155,
    55,    40,   180,   183,    41,    89,   197,   181,    41,   187,
   206,   109,   110,   111,   112,   113,   114,   115,    20,    20,
   188,    84,   189,   193,   -56,   198,   200,   194,    20,    21,
    21,   204,    43,    22,    23,    23,   202,    20,   205,   210,
   211,    20,    43,   192,    23,   191,    56,   157,    21,     0,
    20,    22,    21,    23,    20,    22,     0,    23,     0,     0,
     0,    21,     0,    42,    43,     0,    23,     0,    43,     0,
    23,    97,    98,    99,     0,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,   111,   112,   113,   114,   115,     0,     0,     0,    59,
    60,    61,   156,    62,    63,    64,    65,    66,    67,    68,
    69,    70,     0,     0,    71,    72,    73,    74,    75,     0,
     0,     0,     0,     0,     0,    97,    98,    99,   -59,   100,
   101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   114,   115,    59,    60,    61,   179,    62,
    63,    64,    65,    66,    67,    68,    69,    70,     0,     0,
    71,    72,    73,    74,    75,    59,    60,    61,     0,    62,
    63,    64,    65,    66,    67,    68,    69,    70,     0,     0,
    71,    72,    73,    74,    75,    98,    99,     0,   100,   101,
   102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
   112,   113,   114,   115,    60,    61,     0,    62,    63,    64,
    65,    66,    67,    68,    69,    70,     0,     0,    71,    72,
    73,    74,    75,    99,     0,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
   115,    61,     0,    62,    63,    64,    65,    66,    67,    68,
    69,    70,     0,     0,    71,    72,    73,    74,    75,   101,
   102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
   112,   113,   114,   115,    63,    64,    65,    66,    67,    68,
    69,    70,     0,     0,    71,    72,    73,    74,    75,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
   113,   114,   115,    64,    65,    66,    67,    68,    69,    70,
     0,     0,    71,    72,    73,    74,    75,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
    65,    66,    67,    68,    69,    70,     0,     0,    71,    72,
    73,    74,    75,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   114,   115,    66,    67,    68,    69,    70,
     0,     0,    71,    72,    73,    74,    75,   105,   106,   107,
   108,   109,   110,   111,   112,   113,   114,   115,    67,    68,
    69,    70,     0,     0,    71,    72,    73,    74,    75,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,    68,
    69,    70,     0,     0,    71,    72,    73,    74,    75
};

static const short yycheck[] = {     8,
    19,    10,    22,    11,    99,    21,    96,    21,    19,    13,
     9,     7,    19,   180,    18,    19,    12,    13,    72,    73,
    19,    13,    78,    16,    33,    41,    18,    43,    44,    81,
    81,    19,    68,    69,    70,    50,    54,    81,   205,    58,
    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    75,    50,    41,    78,
    43,    44,    50,     3,    81,    80,    84,     7,    68,    69,
    70,    82,    12,    13,    78,    82,     5,    81,    94,    99,
    94,    97,    98,    80,   100,   101,   102,   103,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
    72,    73,    19,   193,    81,   200,    78,     4,     6,    81,
     8,     9,    10,    11,    97,    98,    82,   100,   101,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
   113,   114,   115,     1,   142,    50,    19,   146,   147,   148,
    80,    82,     1,    81,    13,    13,    54,    82,    82,    18,
    18,    19,    21,    21,    66,    67,    68,    69,    70,    18,
   179,   180,    21,    13,    18,    13,    79,    13,    18,    19,
   189,    21,    18,    19,    50,    21,    13,    18,   194,    82,
   200,    18,    54,    82,    21,    18,   205,    80,    21,    13,
   199,    64,    65,    66,    67,    68,    69,    70,    67,    67,
    82,    16,    51,    54,    54,    82,    25,    54,    67,    78,
    78,   194,    81,    81,    83,    83,    80,    67,    54,     0,
     0,    67,    81,   153,    83,   152,    24,    94,    78,    -1,
    67,    81,    78,    83,    67,    81,    -1,    83,    -1,    -1,
    -1,    78,    -1,    80,    81,    -1,    83,    -1,    81,    -1,
    83,    51,    52,    53,    -1,    55,    56,    57,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    66,    67,    68,    69,    70,    -1,    -1,    -1,    51,
    52,    53,    82,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    -1,    -1,    66,    67,    68,    69,    70,    -1,
    -1,    -1,    -1,    -1,    -1,    51,    52,    53,    80,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    -1,    -1,
    66,    67,    68,    69,    70,    51,    52,    53,    -1,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    -1,    -1,
    66,    67,    68,    69,    70,    52,    53,    -1,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    52,    53,    -1,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    -1,    -1,    66,    67,
    68,    69,    70,    53,    -1,    55,    56,    57,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    53,    -1,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    -1,    -1,    66,    67,    68,    69,    70,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    56,    57,    58,    59,    60,    61,
    62,    63,    -1,    -1,    66,    67,    68,    69,    70,    57,
    58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
    68,    69,    70,    57,    58,    59,    60,    61,    62,    63,
    -1,    -1,    66,    67,    68,    69,    70,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    58,    59,    60,    61,    62,    63,    -1,    -1,    66,    67,
    68,    69,    70,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    59,    60,    61,    62,    63,
    -1,    -1,    66,    67,    68,    69,    70,    60,    61,    62,
    63,    64,    65,    66,    67,    68,    69,    70,    60,    61,
    62,    63,    -1,    -1,    66,    67,    68,    69,    70,    61,
    62,    63,    64,    65,    66,    67,    68,    69,    70,    61,
    62,    63,    -1,    -1,    66,    67,    68,    69,    70
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
    #ifdef __GNUC__
        #define alloca __builtin_alloca
    #else /* not GNU C.  */
        #if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
        #include <alloca.h>
    #else /* not sparc */
        #if defined (_WIN32 ) && !defined (__TURBOC__)
            #include <malloc.h>
        #else /* not MSDOS, or __TURBOC__ */
            #if defined(_AIX)
                #include <malloc.h>
                #pragma alloca
            #else /* not MSDOS, __TURBOC__, or _AIX */
                #ifdef __hpux
                    #ifdef __cplusplus
                    extern "C" {
                        void *alloca (unsigned int);
                    };
                #else /* not __cplusplus */
                    void *alloca ();
                #endif /* not __cplusplus */
        #endif /* __hpux */
        #endif /* not _AIX */
        #endif /* not MSDOS, or __TURBOC__ */
    #endif /* not sparc.  */
    #endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "/usr/local/lib/bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,yyss, 0);
#endif
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,yyvs, 0);
#endif
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,yyls, 0);
#endif
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 194 "annotate.y"
{ 
	  ANNOTATE_NODE = newExpr(EXPR_LIST,NULL,yyvsp[-6].ll_node,
	                   newExpr(EXPR_LIST,NULL,yyvsp[-5].ll_node,
			     newExpr(EXPR_LIST,NULL,yyvsp[-4].ll_node,
				newExpr(EXPR_LIST,NULL,yyvsp[-3].ll_node,
				   newExpr(EXPR_LIST,NULL,yyvsp[-1].ll_node,NULL)))));
	  if (TRACEON)
	    printf("Recognized ANNOTATION\n");
	;
    break;}
case 3:
#line 204 "annotate.y"
{ 
	  ANNOTATE_NODE = newExpr(EXPR_LIST,NULL,NULL,
	                   newExpr(EXPR_LIST,NULL,NULL,
			     newExpr(EXPR_LIST,NULL,NULL,
				newExpr(EXPR_LIST,NULL,NULL,
				   newExpr(EXPR_LIST,NULL,yyvsp[-1].ll_node,NULL)))));
	   if (TRACEON) printf("Recognized ANNOTATION\n");
	;
    break;}
case 4:
#line 215 "annotate.y"
{
	  yyval.ll_node = NULL; 
        ;
    break;}
case 5:
#line 219 "annotate.y"
{
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"IfDef", NULL);
	  yyval.ll_node = Make_Function_Call (ids,NULL,1,yyvsp[-1].ll_node);
	  if (TRACEON) printf("Recognized IFDEFA \n");
	;
    break;}
case 6:
#line 228 "annotate.y"
{	
	  yyval.ll_node = NULL; 
        ;
    break;}
case 7:
#line 232 "annotate.y"
{
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"Label", NULL);
	  yyval.ll_node = Make_Function_Call (ids,NULL,1,yyvsp[-1].ll_node);
	  if (TRACEON) printf("Recognized IFDEFA \n");
	  if (TRACEON) printf("Recognized ALABEL\n");
	;
    break;}
case 8:
#line 242 "annotate.y"
{
	  yyval.ll_node = NULL; 
        ;
    break;}
case 9:
#line 246 "annotate.y"
{
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"ApplyTo", NULL);
	  yyval.ll_node = Make_Function_Call (ids,NULL,2,yyvsp[-1].ll_node, NULL);
	   if (TRACEON) printf("Recognized APPLYTO \n");
	;
    break;}
case 10:
#line 254 "annotate.y"
{
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"ApplyTo", NULL);
	  yyval.ll_node = Make_Function_Call (ids,NULL,2,yyvsp[-3].ll_node,yyvsp[0].ll_node);
	   if (TRACEON) printf("Recognized APPLYTO \n");
	;
    break;}
case 11:
#line 263 "annotate.y"
{ /* SECTIONT return a string_val llnd */
            yyval.ll_node = yyvsp[0].ll_node;
          ;
    break;}
case 12:
#line 267 "annotate.y"
{
	    
            yyval.ll_node =  newExpr(VAR_REF,NULL,yyvsp[0].hash_entry);
          ;
    break;}
case 13:
#line 272 "annotate.y"
{  
             yyval.ll_node = newExpr(EXPR_LIST,NULL,yyvsp[-2].ll_node,
	                   newExpr(EXPR_LIST,NULL,yyvsp[0].ll_node,NULL));
          ;
    break;}
case 14:
#line 277 "annotate.y"
{  
             yyval.ll_node = newExpr(EXPR_LIST,NULL,NULL,
	                   newExpr(EXPR_LIST,NULL,yyvsp[0].ll_node,NULL));
          ;
    break;}
case 15:
#line 282 "annotate.y"
{  
             yyval.ll_node = yyvsp[0].ll_node;
          ;
    break;}
case 16:
#line 289 "annotate.y"
{
	  if (TRACEON) printf("Recognized LocalDeclare\n");
	  yyval.ll_node = NULL; 
        ;
    break;}
case 17:
#line 294 "annotate.y"
{
	  yyval.ll_node = yyvsp[0].ll_node;
	  if (TRACEON) printf("Recognized  declare_local_list\n");
	;
    break;}
case 18:
#line 301 "annotate.y"
{
	  yyval.ll_node = NULL; 
	  if (TRACEON) printf("Recognized empty expr\n");
        ;
    break;}
case 19:
#line 306 "annotate.y"
{ /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[-3].hash_entry,global_int_annotation);
	  yyval.ll_node = Make_Function_Call (ids,NULL,1,yyvsp[-1].ll_node);
	  if (TRACEON) printf("Recognized Expression_List SPECIALAF  \n");
	;
    break;}
case 20:
#line 313 "annotate.y"
{ /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[-3].hash_entry,global_int_annotation);
	  yyval.ll_node = Make_Function_Call (ids,NULL,1,yyvsp[-1].ll_node);
	  if (TRACEON) printf("Recognized Expression_List SPECIALAF  \n");
	;
    break;}
case 21:
#line 320 "annotate.y"
{ /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (FUNCTION_NAME, "Define" ,global_int_annotation);
	  yyval.ll_node = Make_Function_Call (ids,NULL,2,yyvsp[-3].ll_node,yyvsp[-1].ll_node);
	  if (TRACEON) printf("Recognized Expression_List Define  \n");
	;
    break;}
case 22:
#line 331 "annotate.y"
{
	  yyval.ll_node = NULL; 
        ;
    break;}
case 23:
#line 335 "annotate.y"
{
           yyval.ll_node =  newExpr(EXPR_LIST,NODE_TYPE(yyvsp[0].ll_node),yyvsp[0].ll_node,NULL);
	   if (TRACEON) printf("Recognized onedeclare \n");
	 ;
    break;}
case 24:
#line 340 "annotate.y"
{
	   PTR_LLND ll_ptr ;
	   ll_ptr = Follow_Llnd(yyvsp[-2].ll_node,2);               
	   NODE_OPERAND1(ll_ptr) = newExpr(EXPR_LIST,NODE_TYPE(yyvsp[0].ll_node),yyvsp[0].ll_node,NULL);
	   if (TRACEON) printf("Recognized declare_local_list _inlist \n");
	   yyval.ll_node=yyvsp[-2].ll_node;
	 ;
    break;}
case 25:
#line 350 "annotate.y"
{
	    PTR_SYMB ids = NULL;
	    PTR_LLND expr;
            PTR_HASH p;
	    char temp1[256];
	    
	    /* need a symb there, will be global later */
            p = yyvsp[-1].hash_entry;
	    strcpy(temp1,AnnExTensionNumber);
	    strncat(temp1,p->ident,255);
	    ids = newSymbol (VARIABLE_NAME,temp1,global_int_annotation);
	    expr = newExpr(VAR_REF,global_int_annotation, ids);
	    if (yyvsp[0].ll_node)
	      yyval.ll_node = newExpr(ASSGN_OP,global_int_annotation,expr, yyvsp[0].ll_node);
            else
              yyval.ll_node = expr;
          ;
    break;}
case 26:
#line 368 "annotate.y"
{
	  yyval.ll_node = NULL; 
        ;
    break;}
case 27:
#line 372 "annotate.y"
{
	  yyval.ll_node = yyvsp[0].ll_node;
	;
    break;}
case 28:
#line 382 "annotate.y"
{ 
		  /* to modify, must be check before created */
		  yyval.symbol = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry, NULL); 
		  /* $$ = install_parameter($1,VARIABLE_NAME) ; */
                ;
    break;}
case 29:
#line 388 "annotate.y"
{ 
		  yyval.symbol = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry, NULL);
		;
    break;}
case 30:
#line 395 "annotate.y"
{ yyval.symbol = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry, NULL);;
    break;}
case 31:
#line 397 "annotate.y"
{ yyval.symbol = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry, NULL); ;
    break;}
case 32:
#line 401 "annotate.y"
{ 
                 yyval.token = MINUS_OP ;
	       ;
    break;}
case 33:
#line 405 "annotate.y"
{ 
                 yyval.token = NOT_OP ;
	       ;
    break;}
case 34:
#line 412 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;
                ;
    break;}
case 35:
#line 419 "annotate.y"
{ 
                  yyval.ll_node = LLNULL ;
                ;
    break;}
case 36:
#line 423 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ; 
                ;
    break;}
case 37:
#line 431 "annotate.y"
{ 
                  yyval.ll_node = newExpr(EXPR_LIST,NODE_TYPE(yyvsp[0].ll_node),yyvsp[0].ll_node,NULL);
                ;
    break;}
case 38:
#line 435 "annotate.y"
{ PTR_LLND ll_ptr ;
                  ll_ptr = Follow_Llnd(yyvsp[-2].ll_node,2);               
                  NODE_OPERAND1(ll_ptr) = newExpr(EXPR_LIST,NODE_TYPE(yyvsp[0].ll_node),yyvsp[0].ll_node,NULL);

                  yyval.ll_node=yyvsp[-2].ll_node;
                ;
    break;}
case 39:
#line 445 "annotate.y"
{     
                  yyval.ll_node = newExpr(VECTOR_CONST,NULL,NULL,NULL);
                  primary_flag = VECTOR_CONST_APPEAR ;
		  /* Temporarily setting */
                  NODE_TYPE(yyval.ll_node) = global_int_annotation ;
		;
    break;}
case 40:
#line 452 "annotate.y"
{   
                 yyval.ll_node = newExpr(VECTOR_CONST,NULL,yyvsp[-1].ll_node,NULL);
                  primary_flag = VECTOR_CONST_APPEAR ;
		  /* Temporarily setting */
                  NODE_TYPE(yyval.ll_node) = global_int_annotation ;
	       ;
    break;}
case 41:
#line 461 "annotate.y"
{
           yyval.ll_node = NULL;
        ;
    break;}
case 42:
#line 465 "annotate.y"
{ 
               yyval.ll_node = newExpr(EXPR_LIST,NULL,yyvsp[0].ll_node,NULL);
             ;
    break;}
case 43:
#line 469 "annotate.y"
{
               PTR_LLND ll_node1 ;
               ll_node1 = Follow_Llnd(yyvsp[-2].ll_node,2);
               NODE_OPERAND1(ll_node1)= newExpr(EXPR_LIST,NULL,yyvsp[0].ll_node,NULL);
	       yyval.ll_node=yyvsp[-2].ll_node;
	     ;
    break;}
case 44:
#line 481 "annotate.y"
{ 
               yyval.ll_node = yyvsp[0].ll_node;
             ;
    break;}
case 45:
#line 485 "annotate.y"
{ 
               yyval.ll_node = yyvsp[0].ll_node;
             ;
    break;}
case 46:
#line 489 "annotate.y"
{ 
               yyval.ll_node = yyvsp[0].ll_node;
             ;
    break;}
case 47:
#line 493 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;
                ;
    break;}
case 48:
#line 501 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;
	    ;
    break;}
case 49:
#line 505 "annotate.y"
{ 
		  yyval.ll_node = newExpr(VAR_REF, NULL,Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry, NULL));
		  exception_flag = ON ;
		;
    break;}
case 50:
#line 514 "annotate.y"
{ PTR_LLND p1,p2 ;
              p1 =  newExpr(DDOT,NULL,yyvsp[-4].ll_node,yyvsp[-2].ll_node);
              p2 = newExpr(DDOT,NULL,p1,yyvsp[0].ll_node);
              yyval.ll_node = p2 ;
	    ;
    break;}
case 51:
#line 520 "annotate.y"
{ 
              yyval.ll_node= newExpr(DDOT,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
	    ;
    break;}
case 52:
#line 528 "annotate.y"
{ 
             yyval.ll_node=  newExpr(COPY_NODE,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
	   ;
    break;}
case 53:
#line 535 "annotate.y"
{
	      yyval.ll_node = NULL;
	    ;
    break;}
case 54:
#line 539 "annotate.y"
{ PTR_LLND p1,p2 ;
	      p1 =  newExpr(DDOT,NULL,yyvsp[-4].ll_node,yyvsp[-2].ll_node);
              p2 = newExpr(DDOT,NULL,p1,yyvsp[0].ll_node);
              yyval.ll_node = p2 ;
	    ;
    break;}
case 55:
#line 545 "annotate.y"
{
              yyval.ll_node= newExpr(DDOT,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
	    ;
    break;}
case 56:
#line 552 "annotate.y"
{
                yyval.ll_node = LLNULL ;
             ;
    break;}
case 57:
#line 556 "annotate.y"
{ 
                 yyval.ll_node = yyvsp[0].ll_node ;
           ;
    break;}
case 61:
#line 572 "annotate.y"
{ 
            /* Need Another way to check this one */
            /*  if (primary_flag & EXCEPTION_ON) Message("syntax error 6"); */
             if (exception_flag == ON)  { /* Message("undefined symbol",0); */
                                          exception_flag =OFF;
                                        }
             yyval.ll_node=yyvsp[0].ll_node ;
	   ;
    break;}
case 62:
#line 581 "annotate.y"
{	
		  yyval.ll_node=newExpr(yyvsp[-1].token,NULL,yyvsp[0].ll_node);
		;
    break;}
case 63:
#line 585 "annotate.y"
{ 
		  yyval.ll_node= newExpr(SIZE_OP,global_int_annotation,yyvsp[0].ll_node,LLNULL);
                ;
    break;}
case 64:
#line 589 "annotate.y"
{ 
		  yyval.ll_node=newExpr(ADD_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 65:
#line 593 "annotate.y"
{ 
		  yyval.ll_node=newExpr(SUBT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 66:
#line 597 "annotate.y"
{ 
		  yyval.ll_node=newExpr(MULT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 67:
#line 601 "annotate.y"
{ 
		  yyval.ll_node=newExpr(DIV_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 68:
#line 605 "annotate.y"
{ 
		  yyval.ll_node=newExpr(MOD_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 69:
#line 609 "annotate.y"
{ int op1 ;
                  op1 = (yyvsp[-1].token == ((int) LE_EXPR)) ? LE_OP : GE_OP ;
		  yyval.ll_node=newExpr(op1,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 70:
#line 614 "annotate.y"
{ 
		  yyval.ll_node=newExpr(LT_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 71:
#line 618 "annotate.y"
{ 
		  yyval.ll_node=newExpr(GT_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 72:
#line 622 "annotate.y"
{ int op1 ;
                  op1 = (yyvsp[-1].token == ((int) NE_EXPR)) ? NE_OP : EQ_OP  ;
     		  yyval.ll_node=newExpr(op1,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 73:
#line 627 "annotate.y"
{ 
		  yyval.ll_node=newExpr(BITAND_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 74:
#line 631 "annotate.y"
{ 
		  yyval.ll_node=newExpr(BITOR_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 75:
#line 635 "annotate.y"
{ 
		  yyval.ll_node=newExpr(XOR_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 76:
#line 639 "annotate.y"
{ 
		  yyval.ll_node=newExpr(AND_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 77:
#line 643 "annotate.y"
{ 
		  yyval.ll_node=newExpr(OR_OP,global_int_annotation,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 78:
#line 647 "annotate.y"
{ PTR_LLND ll_node1;
		  ll_node1=newExpr(EXPR_IF_BODY,yyvsp[-2].ll_node,yyvsp[0].ll_node);
		  yyval.ll_node=newExpr(EXPR_IF,NULL,yyvsp[-4].ll_node,ll_node1);
		;
    break;}
case 79:
#line 652 "annotate.y"
{ 
		  yyval.ll_node=newExpr(ASSGN_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 80:
#line 656 "annotate.y"
{ int op1 ;
                  op1 = map_assgn_op(yyvsp[-1].token);
		  yyval.ll_node=newExpr(op1,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 81:
#line 665 "annotate.y"
{ 
             if (exception_flag == ON)  { Message("undefined symbol",0);
                                          exception_flag =OFF;
                                        }
             yyval.ll_node=yyvsp[0].ll_node ;
	   ;
    break;}
case 82:
#line 672 "annotate.y"
{ 
		  yyval.ll_node=newExpr(yyvsp[-1].token,NULL,yyvsp[0].ll_node);
                ;
    break;}
case 83:
#line 676 "annotate.y"
{ 
		  yyval.ll_node=newExpr(SIZE_OP,NULL,yyvsp[0].ll_node);
                ;
    break;}
case 84:
#line 680 "annotate.y"
{ 
		  yyval.ll_node=newExpr(ADD_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 85:
#line 684 "annotate.y"
{ 
		  yyval.ll_node=newExpr(SUBT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 86:
#line 688 "annotate.y"
{ 
		  yyval.ll_node=newExpr(MULT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 87:
#line 692 "annotate.y"
{ 
		  yyval.ll_node=newExpr(DIV_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 88:
#line 696 "annotate.y"
{ 
		  yyval.ll_node=newExpr(MOD_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 89:
#line 700 "annotate.y"
{ 
		  yyval.ll_node=newExpr(LSHIFT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 90:
#line 704 "annotate.y"
{ 
		  yyval.ll_node=newExpr(RSHIFT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 91:
#line 708 "annotate.y"
{ int op1 ;
                  op1 = (yyvsp[-1].token == ((int) LE_EXPR)) ? LE_OP : GE_OP ;
		  yyval.ll_node=newExpr(op1,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 92:
#line 713 "annotate.y"
{ 
		  yyval.ll_node=newExpr(LT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 93:
#line 717 "annotate.y"
{ 
		  yyval.ll_node=newExpr(GT_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 94:
#line 722 "annotate.y"
{ int op1 ;

                  op1 = (yyvsp[-1].token == ((int) NE_EXPR)) ? NE_OP : EQ_OP  ;
		  yyval.ll_node=newExpr(op1,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 95:
#line 728 "annotate.y"
{ 
		  yyval.ll_node=newExpr(BITAND_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 96:
#line 732 "annotate.y"
{ 
		  yyval.ll_node=newExpr(BITOR_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 97:
#line 736 "annotate.y"
{ 
		  yyval.ll_node=newExpr(XOR_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 98:
#line 740 "annotate.y"
{ 
		  yyval.ll_node=newExpr(AND_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 99:
#line 744 "annotate.y"
{ 
		  yyval.ll_node=newExpr(OR_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 100:
#line 748 "annotate.y"
{ PTR_LLND ll_node1;
		  ll_node1=newExpr(EXPR_IF_BODY,yyvsp[-3].charv,yyvsp[-2].ll_node);
		  yyval.ll_node=newExpr(EXPR_IF,NULL,yyvsp[-4].ll_node,ll_node1);
		;
    break;}
case 101:
#line 753 "annotate.y"
{ 
		  yyval.ll_node=newExpr(ASSGN_OP,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 102:
#line 757 "annotate.y"
{ int op1 ;
                  op1 = map_assgn_op(yyvsp[-1].token);
		  yyval.ll_node=newExpr(op1,NULL,yyvsp[-2].ll_node,yyvsp[0].ll_node);
                ;
    break;}
case 103:
#line 768 "annotate.y"
{ PTR_SYMB symbptr;
		  symbptr = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, yyvsp[0].hash_entry,NULL);
		  yyval.ll_node = newExpr(VAR_REF,global_int_annotation,symbptr);
		  exception_flag = ON ;
		;
    break;}
case 104:
#line 774 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;
                ;
    break;}
case 105:
#line 778 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;    
                ;
    break;}
case 106:
#line 782 "annotate.y"
{ 
                  primary_flag = EXPR_LR ;
                  yyval.ll_node = yyvsp[-1].ll_node ;
                ;
    break;}
case 107:
#line 788 "annotate.y"
{
	     yyval.ll_node = NULL;
	   ;
    break;}
case 108:
#line 792 "annotate.y"
{
	     yyval.ll_node = yyvsp[0].ll_node;
	   ;
    break;}
case 109:
#line 796 "annotate.y"
{  PTR_SYMB symb;

                   if (exception_flag == ON)
		    {
                      /* strange behavior for default function */
		      symb = NODE_SYMB(yyvsp[-1].ll_node);
		      SYMB_CODE(symb) = FUNCTION_NAME;
                      exception_flag = OFF ;
                      yyval.ll_node =  Make_Function_Call (symb,NULL,0,NULL);
                     }
                   else
                      yyval.ll_node = yyvsp[-1].ll_node ;
		 ;
    break;}
case 110:
#line 811 "annotate.y"
{ PTR_LLND lnode_ptr ,llp ;
                  int      status;

                  llp = yyvsp[-2].ll_node ;
                  status = OFF ;
                  if ((llp->variant == FUNC_CALL) && (!llp->entry.Template.ll_ptr1))
                      { 
                         lnode_ptr = llp;
                         status = FUNC_CALL ;
		       }
                  if ((!status) &&((llp->variant == RECORD_REF)||
				   (llp->variant == POINTST_OP)))
		    {
                       lnode_ptr = llp->entry.Template.ll_ptr2;
		       if ((lnode_ptr)&&(lnode_ptr->variant== FUNCTION_REF))
			 {
                           lnode_ptr->variant = FUNC_CALL;
			 }
                       status = FUNC_CALL ;
		     }
                  if ((!status) &&(llp->variant== FUNCTION_REF))
		    {  llp->variant = FUNC_CALL ;
                       status = FUNC_CALL ;
		       lnode_ptr = llp;
		     }
		  if (!status) {
                       status = FUNCTION_OP;
		       lnode_ptr = llp;
		     }
                  switch (status) {
                  case FUNCTION_OP : yyval.ll_node =newExpr(FUNCTION_OP,yyvsp[-2].ll_node,yyvsp[-1].ll_node);
                                     yyval.ll_node->type = yyvsp[-2].ll_node->type ;
                                     break;
                  case FUNC_CALL :   lnode_ptr->entry.Template.ll_ptr1=yyvsp[-1].ll_node;
                                     yyval.ll_node = yyvsp[-2].ll_node ;
                                     break;
	          default :        Message("system error 10",0);
		  }
		;
    break;}
case 111:
#line 852 "annotate.y"
{ int status ;
             PTR_LLND ll_ptr,lp1;

             ll_ptr = check_array_id_format(yyvsp[-3].ll_node,&status);
             switch (status) {
             case NO : Message("syntax error ",0);
                       break ;
	     case ARRAY_OP_NEED:
                       lp1 = newExpr(EXPR_LIST,NULL,yyvsp[-1].ll_node,LLNULL);/*mod*/
                       yyval.ll_node = newExpr(ARRAY_OP,NULL,yyvsp[-3].ll_node,lp1);
                       break;
             case ID_ONLY :
                       ll_ptr->variant = ARRAY_REF ;
                       ll_ptr->entry.Template.ll_ptr1 = newExpr(EXPR_LIST,NULL,yyvsp[-1].ll_node,LLNULL);
                       yyval.ll_node = yyvsp[-3].ll_node ;
                       break;
             case RANGE_APPEAR :
	               ll_ptr->entry.Template.ll_ptr2 = newExpr(EXPR_LIST,NULL,yyvsp[-1].ll_node,LLNULL);
 	               yyval.ll_node = yyvsp[-3].ll_node ; 
                       break;
             }
/*             $$->type = adjust_deref_type($1->type,DEREF_OP);*/
           ;
    break;}
case 112:
#line 876 "annotate.y"
{ 
                  yyval.ll_node = newExpr(PLUSPLUS_OP,NULL,LLNULL,yyvsp[-1].ll_node);
		  yyval.ll_node->type = yyvsp[-1].ll_node->type ;
                ;
    break;}
case 113:
#line 881 "annotate.y"
{ 
                  yyval.ll_node = newExpr(MINUSMINUS_OP,NULL,LLNULL,yyvsp[-1].ll_node);
		  yyval.ll_node->type = yyvsp[-1].ll_node->type ;
		;
    break;}
case 114:
#line 894 "annotate.y"
{ 
                  yyval.ll_node = yyvsp[0].ll_node ;    
                ;
    break;}
case 115:
#line 898 "annotate.y"
{ 
                  primary_flag =EXPR_LR ;
                  yyval.ll_node = yyvsp[-1].ll_node ;
                ;
    break;}
case 116:
#line 904 "annotate.y"
{
	     yyval.ll_node = NULL;
	   ;
    break;}
case 117:
#line 908 "annotate.y"
{ 
                  yyval.ll_node = newExpr(PLUSPLUS_OP,NULL,LLNULL,yyvsp[-1].ll_node);
                ;
    break;}
case 118:
#line 912 "annotate.y"
{ 
                  yyval.ll_node = newExpr(MINUSMINUS_OP,NULL,LLNULL,yyvsp[-1].ll_node);
		;
    break;}
case 119:
#line 920 "annotate.y"
{ 
              yyval.ll_node = yyvsp[0].ll_node ;
            ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 465 "/usr/local/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 926 "annotate.y"

int lineno;			/* current line number in file being read */

/* comments structure */
#define MAX_COMMENT_SIZE  1024 
char    comment_buf[MAX_COMMENT_SIZE + 2];  /* OFFSET '2' to avoid boundary */
int     comment_cursor = 0;
int     global_comment_type;


/*************************************************************************
 *                                                                       *
 *                      lexical analyzer                                 * 
 *                                                                       *
 *************************************************************************/

static int maxtoken;		/* Current length of token buffer */
static char *token_buffer;	/* Pointer to token buffer */
static int previous_value ;     /* last token to be remembered */

/* frw[i] is index in rw of the first word whose length is i. */

#define MAXRESERVED 9

/*static char frw[10] =
  { 0, 0, 0, 2, 6, 14, 22, 34, 39, 44 };*/
static char frw[10] =
{ 0, 0, 0, 2, 5, 13, 21, 32, 37, 41 };

static char *rw[] =
  { "if", "do", 
    "int", "for", "asm",
    "case", "char", "auto", "goto", "else", "long", "void", "enum",
    "float", "short", "union", "break", "while", "const",  "IfDef","Label",
    "double", "static", "extern", "struct", "return", "sizeof", "switch", "signed","coexec","coloop","friend",
    "typedef", "default","private","cobreak", "ApplyTo",
    "unsigned", "continue", "register", "volatile","operator"};

static short rtoken[] =
  { IF, DO, 
    TYPESPEC, FOR, ASM,
    CASE, TYPESPEC, SCSPEC, GOTO, ELSE, TYPEMOD, TYPESPEC, ENUM,
    TYPESPEC, TYPEMOD, UNION, BREAK, WHILE, TYPEMOD,  IFDEFA, ALABELT,
    TYPESPEC, SCSPEC, SCSPEC, STRUCT, RETURN, SIZEOF, SWITCH, TYPEMOD,COEXEC,COLOOP,FRIEND,
    SCSPEC, DEFAULT_TOKEN,ACCESSWORD,COBREAK, APPLYTO,
    TYPEMOD, CONTINUE, SCSPEC, TYPEMOD,OPERATOR};

/* This table corresponds to rw and rtoken.
   Its element is an index in ridpointers  */

#define NORID RID_UNUSED

static enum rid rid[] =
  { NORID, NORID, 
    RID_INT, NORID, NORID,
    NORID, RID_CHAR, RID_AUTO, NORID, NORID, RID_LONG, RID_VOID, NORID,
    RID_FLOAT, RID_SHORT, NORID, NORID, NORID, RID_CONST, NORID, NORID,
    RID_DOUBLE, RID_STATIC, RID_EXTERN, NORID, NORID, NORID, NORID, RID_SIGNED,NORID,NORID,NORID,
    RID_TYPEDEF, NORID,RID_PRIVATE,NORID, NORID,
    RID_UNSIGNED, NORID, RID_REGISTER, RID_VOLATILE,NORID};

/* The elements of `ridpointers' are identifier nodes
   for the reserved type names and storage classes.  
tree ridpointers[(int) RID_MAX];
static tree line_identifier;    The identifier node named "line" */


void
init_lex ()
{
  //extern char *malloc();

  /* Start it at 0, because check_newline is called at the very beginning
     and will increment it to 1.  */
  lineno = 0;
  maxtoken = 40;
  lastdecl_id = 0;
  token_buffer = (char *) xmalloc((unsigned)(maxtoken+1));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,token_buffer, 0);
#endif
}

static void
reinit_parse_for_function ()
{
}

/* Put char into comment buffer. When the buffer is full, we make a comment */
/* structure and reset the comment_cursor. */
static int
put_char_buffer(c,sw)
char c ;
int sw;
{
/* no comment here */
return 0;
}

static int
skip_white_space(type)
  int type ;
{
  register int c;


  c = MYGETC();

  for (;;)
    {
      switch (c)
	{
	case '/':
	   return '/';	

	case '\n':
	case ' ':
	case '\t':
	case '\f':
	case '\r':
	case '\b':
	  c = MYGETC();
	  break;

	case '\\':
	  c = MYGETC();
	  if (c == '\n')
	    lineno++;
	  else
	    yyerror("stray '\\' in program");
	  c = MYGETC();
	  break;

	default:
	  return (c);
	}
    }
}

/* Take care of the comments in the tail of the source code */
static int
skip_white_space_2()
{
  register int c;

  c = MYGETC();
  for (;;)
    {
      switch (c)
	{
	case '/':
	  return '/';
	case '\n':
           return(c);

	case ' ':
	case '\t':
	case '\f':
	case '\r':
	case '\b':
	  c = MYGETC();
	  break;

	case '\\':
	  c = MYGETC();
	  if (c == '\n')
	    lineno++;
	  else
	    yyerror("stray '\\' in program");
	  c = MYGETC();
	  break;

	default:
	  return (c);
	}
    }
}



/* make the token buffer longer, preserving the data in it.
p should point to just beyond the last valid character in the old buffer
and the value points to the corresponding place in the new one.  */

static char *
extend_token_buffer(p)
char *p;
{
  register char *newbuf;
  register char *value;
  int newlength = maxtoken * 2 + 10;
  register char *p2, *p1;
  //extern char *malloc();

  newbuf = (char*)malloc((unsigned)(newlength+1));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,newbuf, 0);
#endif
  p2 = newbuf;
  p1 = newbuf + newlength + 1;
  while (p1 != p2) *p2++ = 0;

  value = newbuf;
  p2 = token_buffer;
  while (p2 != p)
   *value++ = *p2++;

  token_buffer = newbuf;

  maxtoken = newlength;

  return (value);
}




#define isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))
#define isdigit(char) (char >= '0' && char <= '9')
#define ENDFILE -1  /* token that represents end-of-file */
#define isanop(d) ((d == '+') || (d == '-') || (d == '&') || (d == '|') || (d == '<') || (d == '>') || (d == '*') || (d == '/') || (d == '%') || (d == '^') || (d == '!') || (d == '=') )


int
readescape ()
{
  register int c = MYGETC ();
  register int count, code;

  switch (c)
    {
    case 'x':
      code = 0;
      count = 0;
      while (1)
	{
	  c = MYGETC ();
	  if (!(c >= 'a' && c <= 'f')
	      && !(c >= 'A' && c <= 'F')
	      && !(c >= '0' && c <= '9'))
	    {
	      unMYGETC (c);
	      break;
	    }
	  if (c >= 'a' && c <= 'z')
	    c -= 'a' - 'A';
	  code *= 16;
	  if (c >= 'a' && c <= 'f')
	    code += c - 'a' + 10;
	  if (c >= 'A' && c <= 'F')
	    code += c - 'A' + 10;
	  if (c >= '0' && c <= '9')
	    code += c - '0';
	  count++;
	  if (count == 3)
	    break;
	}
      if (count == 0)
	yyerror ("\\x used with no following hex digits");
      return code;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':
      code = 0;
      count = 0;
      while ((c <= '7') && (c >= '0') && (count++ < 3))
	{
	  code = (code * 8) + (c - '0');
	  c = MYGETC ();
	}
      unMYGETC (c);
      return code;

    case '\\': case '\'': case '"':
      return c;

    case '\n':
      lineno++;
      return -1;

    case 'n':
      return c ;  /*     return TARGET_NEWLINE; */

    case 't':
      return c;  /*      return TARGET_TAB; */

    case 'r':
      return c;/*      return TARGET_CR; */

    case 'f':
      return c;/*       return TARGET_FF;*/

    case 'b':
      return c;/*       return TARGET_BS;*/

    case 'a':
      return c; /*      return TARGET_BELL;*/

    case 'v':
      return c; /*      return TARGET_VT;*/
    }
  return c;
}

 
int
yylex()
{
  register int c;
  register char *p;
  register int value;
  int low /*,high */ ;
  char *str1 ;
/*  double  ddval ; */
/*  int type; */
  int c3;



  if (recursive_yylex == OFF) new_cur_comment = (PTR_CMNT) NULL ;

  /* line_pos_1 = lineno +1 ; */
  c = skip_white_space(FULL);
  /*  yylloc.first_line = lineno;*/

  switch (c)
    {
    case EOF:
      value = ENDFILE; break;

    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
    case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
    case 'K':  case 'L':  case 'M':  case 'N':  case 'O':
    case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
    case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
    case 'Z':
    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
    case 'p':  case 'q':  case 'r':  case 's':  case 't':
    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
    case 'z':
    case '_':

      p = token_buffer;
      while (isalnum(c) || (c == '_') || (c == '~'))
	{
	  if (p >= token_buffer + maxtoken)
	    p = extend_token_buffer(p);
	  *p++ = c;
	  c = MYGETC();
	}

      *p = 0;
      unMYGETC(c);

      value = IDENTIFIER;


      if (p - token_buffer <= MAXRESERVED)
	{
	  register int lim = frw [p - token_buffer + 1];
	  register int i;

	  for (i = frw[p - token_buffer]; i < lim; i++)
	    if (rw[i][0] == token_buffer[0] && !strcmp(rw[i], token_buffer))
	      {
		if (rid[i])
		  yylval.token = (int) rid[i] ;
                  value = (int) rtoken[i];
		break;
	      }
	}

      { int temp;
	if ((temp = Recog_My_Token(token_buffer)) != -1)
	  {
	    yylval.token = temp;
	    value = temp;
	  }
      }

      if (value == IDENTIFIER)
	{ int t_status ;
	  PTR_LLND temp;
	/* temp move it out */

          yylval.hash_entry = look_up_type(token_buffer,&t_status);
          /* if ((t_status)&&(lastdecl_id ==0))   value = TYPENAME;    
	     Wait to fix that */
	  /* temporary fix */
           temp = look_up_section(token_buffer);
          if (temp)
	    {
	      yylval.ll_node = temp;
	      value = SECTIONT;
	    }

	  if (look_up_specialfunction(token_buffer))
	    {
	      value =  SPECIALAF;
	    }
	      
	  
        }

      break;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    case '.':
      {
	int base = 10;
	int count = 0;
	int largest_digit = 0;
	/* for multi-precision arithmetic,
	   we store only 8 live bits in each short,
	   giving us 64 bits of reliable precision */
	short shorts[8];
	int floatflag = 0;  /* Set 1 if we learn this is a floating constant */

	for (count = 0; count < 8; count++)
	  shorts[count] = 0;

	p = token_buffer;
	*p++ = c;

	if (c == '0')
	  {
	    *p++ = (c = MYGETC());
	    if ((c == 'x') || (c == 'X'))
	      {
		base = 16;
		*p++ = (c = MYGETC());
	      }
	    else
	      {
		base = 8;
	      }
	  }

	while (c == '.'
	       || (isalnum (c) && (c != 'l') && (c != 'L')
		   && (c != 'u') && (c != 'U')
		   && (!floatflag || ((c != 'f') && (c != 'F')))))
	  {
	    if (c == '.')
	      {
		if (base == 16)
		  yyerror ("floating constant may not be in radix 16");
		floatflag = 1;
		base = 10;
		*p++ = c = MYGETC ();
		/* Accept '.' as the start of a floating-point number
		   only when it is followed by a digit.
		   Otherwise, unread the following non-digit
		   and use the '.' as a structural token.  */
		if (p == token_buffer + 2 && !isdigit (c))
		  {
		    if (c == '.')
		      {
			c = MYGETC ();
			if (c == '.')
			  { 
                            value = ELLIPSIS ;
			    goto done ;
                          }
			yyerror ("syntax error");
		      }
		    unMYGETC (c);
		    value = '.';
                    goto done;
		  }
	      }
	    else
	      {
		if (isdigit(c))
		  {
		    c = c - '0';
		  }
		else if (base <= 10)
		  {
		    if ((c&~040) == 'E')
		      {
			if (base == 8)
			  yyerror ("floating constant may not be in radix 8");
			base = 10;
			floatflag = 1;
			break;   /* start of exponent */
		      }
		    yyerror ("nondigits in number and not hexadecimal");
		    c = 0;
		  }
		else if (c >= 'a')
		  {
		    c = c - 'a' + 10;
		  }
		else
		  {
		    c = c - 'A' + 10;
		  }
		if (c >= largest_digit)
		  largest_digit = c;
	    
		for (count = 0; count < 8; count++)
		  {
		    (shorts[count] *= base);
		    if (count)
		      {
			shorts[count] += (shorts[count-1] >> 8);
			shorts[count-1] &= (1<<8)-1;
		      }
		    else shorts[0] += c;
		  }
    
		*p++ = (c = MYGETC());
	      }
	  }

	if (largest_digit >= base)
	  yyerror ("numeric constant contains digits beyond the radix");

	/* Remove terminating char from the token buffer and delimit the string */
	*--p = 0;

	if (floatflag)
	  {
	   /*  enum rid type = DOUBLE_TYPE_CONST ; */

	    /* Read explicit exponent if any, and put it in tokenbuf.  */

	    if ((c == 'e') || (c == 'E'))
	      {
		*p++ = c;
		c = MYGETC();
		if ((c == '+') || (c == '-'))
		  {
		    *p++ = c;
		    c = MYGETC();
		  }
	        while (isdigit(c))
		  {
		    *p++ = c;
		    c = MYGETC();
		  }
	      }

	    *p = 0;

	    while (1)
	      {
/*		if (c == 'f' || c == 'F')
		  type = FLOAT_TYPE_CONST ;
		else if (c == 'l' || c == 'L')
		  type = LONG_DOUBLE_TYPE_CONST ;
		else */

                if((c != 'f') && (c != 'F') && (c != 'l') && (c !='L'))
		  {
		    if (isalnum (c))
		      {
			yyerror ("garbage at end of number");
			while (isalnum (c))
			  c = MYGETC ();
		      }
		    break;
		  }
		c = MYGETC ();
	      }

	    unMYGETC(c);

/*	    ddval = build_real_from_string (token_buffer, 0);  */
            str1= (char *) copys(token_buffer);
            yylval.ll_node = newExpr(FLOAT_VAL,NULL,LLNULL,LLNULL,str1);

	  }
	else
	  {
	    /* enum rid  type; */

	    /* int spec_unsigned = 0; */
	    /* int spec_long = 0;  */

	    while (1)
	      {
/*		if (c == 'u' || c == 'U')
		  {
		    spec_unsigned = 1;
		  }
		else if (c == 'l' || c == 'L')
		  {
		    spec_long = 1;
		  }
		else */

               if((c != 'u') && (c != 'U') && (c != 'l') && (c != 'L'))
		  {
		    if (isalnum (c))
		      {
			yyerror ("garbage at end of number");
			while (isalnum (c))
			  c = MYGETC ();
		      }
		    break;
		  }
		c = MYGETC ();
	      }

	    unMYGETC (c);

	    /* This is simplified by the fact that our constant
	       is always positive.  */

	    low= (shorts[3]<<24) + (shorts[2]<<16) + (shorts[1]<<8) + shorts[0] ;
	  /*  high = (shorts[7]<<24) + (shorts[6]<<16) + (shorts[5]<<8) + shorts[4] ; */
	    

	    /* type = LONG_UNSIGNED_TYPE_CONST ; */
	    yylval.ll_node = makeInt(low);	
	 }

	value = CONSTANT; break;
      }

    case '\'':
      c = MYGETC();
      {

      tryagain:

	if (c == '\\')
	  {
	    c = readescape ();
	    if (c < 0)
	      goto tryagain;
	  }
	else if (c == '\n')
	  {
	      Message ("ANSI C forbids newline in character constant",0);
	    lineno++;
	  }

	c3= c;

	c = MYGETC ();
	if (c != '\'')
	  yyerror("malformatted character constant");
        yylval.ll_node = newExpr(CHAR_VAL,LLNULL,LLNULL,low);
	yylval.ll_node->entry.cval = c3;  
	value = CONSTANT; break;
      }

    case '"':
      {
	c = MYGETC();
	p = token_buffer;

	while (c != '"')
	  {
	    if (c == '\\')
	      {
                /* New Added Three lines */
                if (p == token_buffer + maxtoken)
	          p = extend_token_buffer(p);
  	        *p++ = c;

		c = readescape ();
		if (c < 0)
		  goto skipnewline;
	      }
	    else if (c == '\n')
	      {
		  Message ("ANSI C forbids newline in string constant",0);
		lineno++;
	      }

	    if (p == token_buffer + maxtoken)
	      p = extend_token_buffer(p);
	    *p++ = c;

	  skipnewline:
	    c = MYGETC ();
	  }

	*p++ = 0;

	str1= (char *) copys(token_buffer);
        yylval.ll_node = (PTR_LLND) newNode(STRING_VAL);
	NODE_STRING_POINTER(yylval.ll_node) = str1;
	value = STRING; break;
      }
      
    case '+':
    case '-':
    case '&':
    case '|':
    case '<':
    case '>':
    case '*':
    case '/':
    case '%':
    case '^':
    case '!':
    case '=':
      {
	register int c1;
        if ( previous_value == OPERATOR )
           {
             p = token_buffer;
             while (isanop(c) )
              {
                  if (p >= token_buffer + maxtoken)
	          p = extend_token_buffer(p);
	          *p++ = c;
	          c = MYGETC();
               }
             *p = 0;
             unMYGETC(c);
             value =  LOADEDOPR ;
             yylval.hash_entry = look_up(token_buffer);
             break;
            }
      combine:

	switch (c)
	  {
	  case '+':
	    yylval.token = (int) PLUS_EXPR; break;
	  case '-':
	    yylval.token = (int) MINUS_EXPR; break;
	  case '&':
	    yylval.token = (int) BIT_AND_EXPR; break;
	  case '|':
	    yylval.token = (int) BIT_IOR_EXPR; break;
	  case '*':
	    yylval.token = (int) MULT_EXPR; break;
	  case '/':
	    yylval.token = (int) TRUNC_DIV_EXPR; break;
	  case '%':
	    yylval.token = (int) TRUNC_MOD_EXPR; break;
	  case '^':
	    yylval.token = (int) BIT_XOR_EXPR; break;
	  case LSHIFT:
	    yylval.token = (int) LSHIFT_EXPR; break;
	  case RSHIFT:
	    yylval.token = (int) RSHIFT_EXPR; break;
	  case '<':
	    yylval.token = (int) LT_EXPR; break;
	  case '>':
	    yylval.token = (int) GT_EXPR; break;
	  }	

	c1 = MYGETC();

	if (c1 == '=')
	  {
	    switch (c)
	      {
	      case '<':
		value = ARITHCOMPARE; yylval.token = (int) LE_EXPR; goto done;
	      case '>':
		value = ARITHCOMPARE; yylval.token = (int) GE_EXPR; goto done;
	      case '!':
		value = EQCOMPARE; yylval.token = (int) NE_EXPR; goto done;
	      case '=':
		value = EQCOMPARE; yylval.token = (int) EQ_EXPR; goto done;
	      }	
	    value = ASSIGN; goto done;
	  }
	else if (c == c1)
	  switch (c)
	    {
	    case '+':
	      value = PLUSPLUS; goto done;
	    case '-':
	      value = MINUSMINUS; goto done;
	    case '&':
	      value = ANDAND; goto done;
	    case '|':
	      value = OROR; goto done;
/* testing  */
/*            case ':':
              value = DOUBLEMARK; goto done;  */

	    case '<':
	      c = LSHIFT;
	      goto combine;
	    case '>':
	      c = RSHIFT;
	      goto combine;
	    }
	else if ((c == '-') && (c1 == '>'))
	  { value = POINTSAT; goto done; }
	unMYGETC (c1);


        value = c;
	goto done;
      }

    default:
      value = c;
    }

done:

  if (recursive_yylex == OFF) {
    previous_value = value ;
    line_pos_1 = lineno ;
    c = skip_white_space_2();
    if (c != '\n');
       unMYGETC(c);
    if (value != '}') 
      { c = skip_white_space(NEXT_FULL);
	if (c == '\n') lineno++ ;
	else           unMYGETC(c);
      }
    set_up_momentum(value,yylval.token);
    automata_driver(value);
    cur_counter++;
    old_line = yylineno  ;
    yylineno = line_pos_1;
  }

  if (TRACEON) printf("yylex returned %d\n", value);
  return (value);
}
 

static int yyerror(s)
        char   *s;
{
  /* Message(s,0); empty at the moment, generate false error report?
     to be modified later */
  return 1;  /* PHB needed a return val, 1 seems ok */
}


/*  primary :- primary [ expr_vector ]
 *  <1> check the LHS format
 *  <2> return : NO if incorrect format at LHS
 *               ID_ONLY if LHS only have id format (including multiple id)
 *               RANGE_APPEAR if LHS format owns both id and range_list
 */

static
PTR_LLND check_array_id_format(ll_ptr,state)
int *state;
PTR_LLND ll_ptr ;

{   PTR_LLND temp,temp1;

      temp = ll_ptr;
      switch (NODE_CODE(ll_ptr)) {
      case VAR_REF :
                     *state = ID_ONLY ;
                     return(ll_ptr);
      case ARRAY_REF :
                     temp1 = Follow_Llnd(NODE_OPERAND0(ll_ptr),2);
                     *state = RANGE_APPEAR;
                     return(temp1);
      case  ARRAY_OP:temp1 = Follow_Llnd(NODE_OPERAND1(ll_ptr),2);
                     *state =RANGE_APPEAR ;
                     return(temp1);
        default :    *state = ARRAY_OP_NEED ;
                     return(temp);
      }
  }

static
int
map_assgn_op(value)
int value;
{
  switch (value) {
  case ((int) PLUS_EXPR) :
      return(PLUS_ASSGN_OP);
  case ((int) MINUS_EXPR):
      return(MINUS_ASSGN_OP);
  case ((int) BIT_AND_EXPR):
      return(AND_ASSGN_OP);
  case ((int) BIT_IOR_EXPR):
      return(IOR_ASSGN_OP);
  case ((int) MULT_EXPR):
      return(MULT_ASSGN_OP);
  case ((int) TRUNC_DIV_EXPR):
      return(DIV_ASSGN_OP);
  case ((int) TRUNC_MOD_EXPR):
      return(MOD_ASSGN_OP);
  case ((int) BIT_XOR_EXPR):
      return(XOR_ASSGN_OP);
  case ((int) LSHIFT_EXPR):
      return(LSHIFT_ASSGN_OP);
  case ((int) RSHIFT_EXPR):
      return(RSHIFT_ASSGN_OP);
  }
return 0;
}

PTR_HASH 
look_up_type(st, ip)
     char *st;
     int *ip;
{
  char *pt;
  
  pt =  (char *) xmalloc(strlen(st) +1);
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,pt, 0);
#endif
  strcpy(pt,st);
 /* dummy, to be cleaned */
  return (PTR_HASH) pt;
}


PTR_HASH 
look_up(st)
     char *st;
{
  char *pt;
  
  pt =  (char *)  xmalloc(strlen(st) +1);
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,pt, 0);
#endif
  strcpy(pt,st);
 /* dummy, to be cleaned */
  return (PTR_HASH)  pt;
}

static
MYGETC()
{
  
  if (LENSTRINGTOPARSE <= PTTOSTRINGTOPARSE)
	return EOF;

  if (STRINGTOPARSE[ PTTOSTRINGTOPARSE] == '\0')
    {
      PTTOSTRINGTOPARSE++;
      return EOF;
    }
  
  PTTOSTRINGTOPARSE++;
  return STRINGTOPARSE[ PTTOSTRINGTOPARSE-1];
}

static
unMYGETC(c)
char c;
{
  if (LENSTRINGTOPARSE <= PTTOSTRINGTOPARSE)
    return EOF;

  if (PTTOSTRINGTOPARSE >0)
    PTTOSTRINGTOPARSE --;
  STRINGTOPARSE[ PTTOSTRINGTOPARSE] = c;
  return c;
}


/* CurrentScope should be the last in the list */
static char *sectionkeyword[] =
  { "NextStmt",
    "NextAnnotation",      
    "EveryWhere",
    "Follow",
/* keep it last*/  "CurrentScope"};


static PTR_LLND
look_up_section(str)
 char *str;
{ int i;
  PTR_LLND pt = NULL;

  for (i = 0; i < RID_MAX; i++)
    {
      if (strcmp(sectionkeyword[i], str) == 0)
	{
	  pt  = (PTR_LLND) newNode(STRING_VAL);
	  NODE_STRING_POINTER(pt) = (char *) xmalloc(strlen(str) +1);
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,NODE_STRING_POINTER(pt), 0);
#endif
	  strcpy(NODE_STRING_POINTER(pt),str);
	  return pt;
	}
      if (strcmp(sectionkeyword[i],"CurrentScope") == 0)
	return NULL;
    }
  
  return NULL;
}


/* Dummy should be the last in the list */
static char *specialfunction[] =
  { "ListOfAn",
    "Align",
    "Induction",
    "Used",
    "Modified",
    "Alias",
    "Permutation",
    "Assert",
/* keep it last*/  "Dummy"};

static int
look_up_specialfunction(str)
 char *str;
{ int i;

  for (i = 0; i < RID_MAX; i++)
    {
      if (strcmp(specialfunction[i], str) == 0)
	{
	  return TRUE;
	}
      if (strcmp(specialfunction[i],"Dummy") == 0)
	return NULL;
    }
  
  return NULL;
}


static int 
Recog_My_Token(str)
char *str;
{

  if (strcmp("FromAnn",str) == 0)
    return FROMT;

  if (strcmp("ToAnn",str) == 0)
    return TOT;

   if (strcmp("ToLabel",str) == 0)
    return TOTLABEL;

  if (strcmp("ToFunction",str) == 0)
    return TOFUNCTION;

  if (strcmp("Define",str) == 0)
    return DefineANN;

  return -1;
}


PTR_SYMB
Look_For_Symbol_Ann(code,name,type)
     int code;
     char *name;
     PTR_TYPE type;
{
  PTR_SYMB symb;
  char temp1[256];

  strcpy(temp1, AnnExTensionNumber);
  strncat(temp1,name,255);

  if ((symb = getSymbolWithName(temp1, ANNOTATIONSCOPE)))
    return symb;

  if ((symb = getSymbolWithName(name, ANNOTATIONSCOPE)))
    return symb;

  return  newSymbol (code,name,type);
}
 
