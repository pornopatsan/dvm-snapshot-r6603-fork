
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAXitem 10
#define MAXrank 10
#define MAXkw 256
#define SBLEN  200
#define MAXGROUP 128
#define MAXRCODE 512
#define RULELEN 8

#define TTT(s) {printf("\t%s\t",#s);}
#define PPP(x,y,z) printf("\n%s(%d): (%4d,%4d,%4d)",__FILE__,__LINE__,x,y,z);
#define PPPN(n)\
 printf("\t[%.4d]=%4d %4d %4d %4d\n",n,C(n),A(n),B(n),D(n));
#define ASSERT(c,fun) {if(!(c)) ASSfailed(#c,#fun);}
#define ASSERTC(c,N,fun) if(C(N)!=c) ASSfailed(#c,#fun);

#ifndef VERS
#define VERS "4.7.1 (20.12.2009)"
#endif

char Title[]=
 "\tCDVM to C convertor " VERS " (of " __DATE__ ")\n";
char Usage[]=
"\nCDVM-to-C converter command line parameters:"
"\nOptions:"
"\n\t-o <file> -- Output file (by default 'cdvm_out.c')"
"\n\t-s  -- Sequential"
"\n\t-e1 -- PPPA level 1 (P+(S over P))"
"\n\t-e2 -- PPPA level 2 (P+I)"
"\n\t-e3 -- PPPA level 3 (P+(S over P)+I)"
"\n\t-e4 -- PPPA level 4 (ALL)"
"\n\t-d1 -- trace modification of DVM-arrays"
"\n\t-d2 -- trace access to DVM-arrays"
"\n\t-d3 -- trace modification of all data"
"\n\t-d4 -- trace access to all data"
"\n\t-w  -- enable all Warnings"
"\n\t-w- -- disable all Warnings"
"\n\t-v  -- Verbose"
"\n\t-xN -- size of working tables (default -x16)"
    "\n";


#define E(n,c) Msg[n%1000]=c;
void MsgInit(char **Msg)
{

/* File level and operator errors */

E(2011,"5:The 'main' should get the command line parameters")
E(2012,"5:The 'main' should 'return <rc>;' for the DVM-Lib")
E(2021,"5:Implicitly created objects must precede 'main'")
E(2023,"5:'main' required for implicitly created objects")
E(2003,"5:DVM-operator outside function")
E(2005,"5:Misplaced declarative DVM-directive")
E(2006,"5:DVM-directive requires a non-empty operator")
E(2007,"5:Should be followed by the ';'")
E(2063,"5:TASK_REGION must be a sequence of ON-blocks or "
      "an ON-loop")

/* Declarations */

E(2013,"5:DVM-object should be defined as 'void * '")
E(2014,"5:DVM-arrays may be int, long, float, or double only")
E(2015,"5:Scalar can not be distributed")
E(2016,"5:DVM-pointer should be a C-pointer")
E(2034,"5:'*' is valid only with DISTRIBUTE and ALIGN")
E(2017,"5:Only 1D-arrays of ptrs to DVM-arrays supported")
E(2018,"5:Unsupported declarator for distributed data")
E(2033,"5:Can not initialize DVM-objects")
E(2064,"5:DVM declarator syntax error")
E(2084,"5:long, int, short or char variable (array) required")

/* Declaration and usage of DVM-objects */

E(2040,"5:Undefined")
E(2039,"5:This is not a DVM-object")
E(2020,"5:Rank error")
E(2041,"5:This is not a REDUCTION_GROUP")
E(2042,"5:This is not a SHADOW_GROUP")
E(2043,"5:This is not a REMOTE_GROUP")
E(2044,"5:This is not a TASK")
E(2055,"5:An array of void* required.")
E(2061,"5:MAP-target must be a processors section")
E(2062,"5:ON-target must be an element of task array")
E(2078,"5:The base is not distributed")
E(2079,"5:Wrong usage of a task array")

/* DISTRIBUTE, ALIGN, TEMPLATE */

E(2035,"5:'[*]' is only allowed in DVM( * DISTRIBUTE...)")
E(2036,"5:Array should be defined as DVM(DISTRIBUTE...)")
E(2069,"5:GENBLOCK requires non-distributed 1D int array")
E(2082,"5:WGTBLOCK requires non-distributed 1D double array")
E(2083,"5:Mixed GENBLOCK-WGTBLOCK distribution unsupported")
E(2056,"5:ONTO target must be a TASK or a PROCESSORS")
E(2057,"5:ONTO target must be an element of task array")
E(2050,"5:Align-expression syntax: [-][A*]var[+B|-B]")
E(2051,"5:Already used align|do variable")
E(2022,"5:The base of a static should be a (known) static")
E(2037,"5:The parameter should be a (non-static) TEMPLATE")
/**/E(2076,"5:Postponed distribution for a static array")
/**/E(2077,"4:Constant align-expression?"
   " Use braces '[(...)]' to suppress this warning")
E(2080,"5:Template unallowed here")

/* malloc & access to distributed data */

E(2029,"5:DVM-malloc requires (dim1*...*sizeof(...))")
E(2030,"3:Is it 'sizeof(<element type>)' ?")
E(2025,"5:Only DVM-arrays may be malloc'ed")
E(2026,"5:Static DVM-array can not be malloc'ed")
E(2027,"5:Only DVM-pointers may be assigned")
E(2058,"5:REDISTRIBUTE|REALIGN must follow this malloc")
E(2065,"5:Can not assign to REMOTE")
E(2066,"5:Can not get REMOTE address")
E(2068,"0:Possible non-local assignement.")
E(2024,"5:Only 1..6-D arrays may be distributed")
E(2031,"5:Too many distributed dimensions")
E(2032,"5:Not all distributed dimensions")

/* PARALLEL loop */

E(2004,"5:Not allowed in a PARALLEL loop")
E(2008,"5:Unallowed or duplicated sub-directive")
E(2009,"5:Too many headers in the PARALLEL loop")
E(2010,"5:Not enough headers in the PARALLEL loop")
E(2045,"5:Not a PARALLEL loop variable")
E(2046,"5:PARALLEL loop variables disordered")
E(2047,"5:Variable already used")
E(2048,"5:Loop variable required")
E(2049,"5:Only integer scalar loop variables allowed")
E(2081,"5:Task loop syntax: (PARALLEL [<v>] ON <task>[<v>])")

/* SHADOW */

E(2052,"5:Declared (or default) maximum width exceeded")
E(2053,"4:1D-array's shadow has no CORNERs")

/* REDUCTION */

E(2070,"1:Undefined RVAR")
E(2071,"1:Wrong type of RVAR")
E(2072,"1:Undefined RLOC")
E(2073,"1:Wrong type of RLOC")
E(2074,"1:Unallowed RVAR-expression")
E(2075,"1:Unallowed RLOC-expression")

/* Procedures */

E(2001,"5:Parameter should be defined as DVM(*...)")
E(2002,"5:Only DISTRIBUTE and ALIGN are valid for parameter.")
E(2019,"5:Return type may only be DVM( * DISTRIBUTE...)")

/* Data tracing */

E(2054,"0:This initialization will not be traced")
E(2059,"0:Can not trace ++, --, +=, -=, ...")
E(2060,"0:Can not trace multiple assignement")
E(2067,"0:Can not trace non-DVM type")

/* Array copy */

E(2090,"5,Wrong copy loop syntax")
E(2091,"5,Wrong COPY source or target syntax")

/* Miscallaneous */

E(2028,"5:Can fread-fwrite DVM-arrays as a whole only")
E(2099,"5:Do you mean multiple index?")
E(2000,"5:Not yet implemented... or error")

}
#undef  E
#define E(n) ERR(Msg[(n)%1000])

int ASSfailed(char *c, char *fun);
void dump(int N);
void dumpAll(int N);
void dump1(int N);
void nextLine(void );
void ERR(char *msg);
void scan(void );
char *LXW(int );
int Token(char *s);
int NDD(int c, int a, int b, int d);
int mk(int c, int a, int b);
int NDget(int N, int path);
void NDdel(int N);
int revert(int List);
int SSfind(int N, int f, int c);
int up(int n);
int skipA(int c, int N);
int skipB(int c, int N);
int toA(int c, int N);
int toB(int c, int N);
int to(int c);
int Len(int c, int N);
void conc(int N);
void concL(int L, int N);
int head();
int Lop(int c, int x, int y);
int Rop(int c, int x, int y);
void set(int c, int N, int V);
int get(int c, int N);
void del(int c, int N);
int Parse(int N, int rule, int abcd);
void UnPars(int N, int sg, int gen);
int ISWF(int IN);	/* Used by Parser. Returns 0 for OK */


//2009   To suppress warnings declare:
// whatis cline pref_name mk_templ cSHw Align
// mk_genblock mk_multblock AMaxis over Allowed
int whatis(int N);
int cline(void );
int pref_name(char *prefix, int n);
int mk_templ(int amv, int dims, int dir);
int cSHw(int what, int shadows, int rank);
int Align(int m, int exs, int vars);
int mk_genblock(int amv, int ps);
int mk_multblock(int amv);
int AMaxis(int N);
int over(int C);
int Allowed(int dop, int N);



int UX=16;	/* scaling factor for the size of tables */
int i, j, w;
char *wp;
char infile[64]="cdvm_in.c";	/* [default] input */
FILE*fin=0;
char outfile[64]="cdvm_out.c";	/* [default] output */
FILE*fout=0;
char msgfile[]="cdvm_msg";	/* file for messages */
FILE*fmsg=0;
char debfile[]="cdvm_deb";
FILE*fdeb=0;
int SYNTAX=0;	/* syntax-tree */
int SOURCE=0;	/* source */
int TREE=0;	/* input tree */
int curlex=0;	/* curremt lexem */
int inMain=0;	/* 1 when in 'main', -1 after it */
int mainRet=0;	/* 'return' operator in the 'main' */
int LoopNo=0;	/* Loops counter */
int OPTv=0;	/* -v  -- Verbose mode */
int OPTw=2;	/* -w  -- warnings */
int OPTe=0;	/* -e1..4 -- efficiency tracing */
int OPTd=0;	/* -d1..4 -- data tracing */
int OPTmax=4;
int OPTcmd=0;
int OPTnoe=0;	/* 1 if -e is suppressed by -d */
int OPTs=0;	/* -s -- No DVM, TRACE only */
int OPTm=0;	/* -m -- keep pp-ops in place */
int N;	/* A global for the current node */

/* Unpacked declaration of the current name                 */

int cID, cDecl, cDcltr, cType, cRTStype;
int cLBKr, cASTER, cLBKl;	/* ( * xxxx [r1]...) [l1]... */
int cDVM, cDir, cDIR, dRank, uRank;	/* DVM-directive */
char bf[4096];	/* input buffer */
char obf[1024];	/* debugging options as string */
char msg[128];	/* error message buffer */
char *Msg[100]={0};	/* pointers to error messages */
int lineno=0;	/* line number */
int ERRno=0;	/* number of errors */
int WNGno=0;	/* number of warnings */
extern char *Msg[100];
unsigned int HTsize;	/* size of tables */
int *HT0, *HT1;	/* hash table for strings and for nodes */

/*    Scanner globals                                       */

char *cc, *cp;	/* pointers to current token in bf */
int LXcode, LXval;	/* Code of the current token */
char *LX;	/* Buffer for tokens as strings (string memory) */
unsigned int LXSIZE;	/* Size of the string memory */
unsigned int LXfree=1;	/* Next free in the string memory */
int LXdlm[256]={0};	/* Index for delimiters in LX */

/* Codes of tokens by the first character                   */
/*                                                          */
/*      0 -- '0'..'9' -- number                             */
/*      1 -- 'a'..'f' -- hex-digit                          */
/*      2 -- letter -- identifier                           */
/*      3 - '"' -- string                                   */
/*      4 -- '\'' -- character literal                      */
/*      5 -- '.' -- possible real (e.g.  .5e-2)             */
/*      7 -- known delimiters (filled at initialization)    */
/*      8 -- white spaces                                   */
/*      9 -- new line                                       */

char LXcd[]="?????????89??8??????????????????"
   "8?36???4??????5?0000000000??????"
   "?11111122222222222222222222????2"
   "?11111122222222222222222222?????"
   "????????????????????????????????"
   "????????????????????????????????"
   "????????????????????????????????"
   "????????????????????????????????";

/* Symbolic names for tokens and codes of nodes             */

enum
   {
   LXEOF, TOKEN, DLM, KWD, LXX, LXANY, LXI, LXN,
   LXC, LXR, LXS, CPP, NL, LXSs, __DELIMITERS__, SEMIC,
   COMMA, LBA, LBS, LBK, RBA, RBK, RBS, POINT,
   PNT_AST, PPPOINT, QUEST, NOM, DBL_NOM, COLON, DBL_COLON, DIV,
   ADIV, COMM, CPPCOMM, MOD, AMOD, ADD, AADD, INC,
   SUB, ASUB, DEC, ARROW, ARR_AST, MUL, AMUL, ASSIGN,
   EQU, LT, LE, LSH, ALSH, GT, GE, RSH,
   ARSH, BNOT, NOT, NEQ, BAND, AAND, AND, BOR,
   AOR, OR, BXOR, AXOR, __KEYWORDS__, RETURN, IF, ELSE,
   WHILE, DO, FOR, BREAK, CONTINUE, SWITCH, CASE, DEFAULT,
   GOTO, SIZEOF, TYPEDEF, EXTERN, STATIC, REGISTER, AUTO, CONST,
   INT, SHORT, LONG, VOID_, CHAR, SIGNED, UNSIGNED, FLOAT,
   DOUBLE, STRUCT, UNION, ENUM, LX_DVM, PROCESSORS, DISTRIBUTE, BLOCK,
   GENBLOCK, WGTBLOCK, MULTBLOCK, ONTO, ALIGN, WITH, TEMPLATE, SHADOW,
   LX_SG, LX_RG, LX_RMG, LX_IG, TASK, REDISTRIBUTE, REALIGN, NEW,
   LX_CRTEMP, PARALLEL, ON, REDUCTION, SHRENEW, ACROSS, ACRIN, ACROUT,
   PIPE, LX_CRSG, CORNER, SHSTART, SHWAIT, SUM, PROD, MAX,
   MIN, LX_OR, LX_AND, MAXLOC, MINLOC, RSTART, RWAIT, REMOTE,
   INDIRECT, PREFETCH, RESET, MAP, LX_TASKREG, INTERVAL, DEBUG, BARRIER,
   LX_CP, COPY, CPSTART, CPWAIT, __LAST_KEYWORD__, INCA, DECA, LXcast,
   ADDR, CONT, LXskip, LXaster, XXdecl, XXtype, XXdecls, XXfields,
   XXfield, XXenums, XXdcltr, LXfun, LXfunKR, XXbody, XXoper, XXclist,
   XXslist, XXlist, XXexpr, XXparm, LX_NUMBER_OF_PROC, LX_FOR, LX_DO, LX_main,
   LX_malloc, LX_free, LX_printf, DVM_FOR, DVM_FOR_1, DVM_RB, DVM_DOPL, TASKLOOP,
   LXIrg, DVMbase, DVMvar, DVMind, DVMbind, DVMalign, LXIsg, DVMshw,
   DVMshad, DVMremote, LXIag, LXItask, LXIproc, DTscal, optD0, optD1,
   optD2, optD3, optD4, optE0, optE1, optE2, optE3, optE4,
   ONE, TWO, __LAST_CODE__, LASTLEX
   }
;

/*    The Linked Memory                                     */

int NDsize;	/* The size of the linked memory */
struct ND
   {
   int c;
   int a;
   int b;
   int d;
   }
*NDs;	/* The linked memory */
int NDfree;	/* The first free node */
int tmpList=0, tmpHead, tmpTail;	/* Temporary list */

/*    Functions to access nodes                             */

int A(int N)
   {
   ASSERT(N>=0, A());
   return NDs[N].a;
   }
int B(int N)
   {
   ASSERT(N>=0, B());
   return NDs[N].b;
   }
int C(int N)
   {
   ASSERT(N>=0, C());
   return NDs[N].c;
   }
int D(int N)
   {
   ASSERT(N>=0, D());
   return NDs[N].d;
   }
void wA(int N, int x)
   {
   ASSERT(N>0, wA);
   NDs[N].a=x;
   }
void wB(int N, int x)
   {
   ASSERT(N>0, wB);
   NDs[N].b=x;
   }
void wC(int N, int x)
   {
   ASSERT(N>0, wC);
   NDs[N].c=x;
   }
void wD(int N, int x)
   {
   ASSERT(N>0, wD);
   NDs[N].d=x;
   }
void wN(int N, int c, int a, int b)
   {
   wC(N, c);
   wA(N, a);
   wB(N, b);
   }

/*                                                          */

int Stack=0;	/* Parsing and semantic functions stack */
enum
   {
   tR0, tR1, tR2, tRW, tRx
   }
;	/* types of syntax rules */
int INRx;

/* Normal exit.                                             */

void FINISH(int rc)
   {
   if (rc) 	fprintf(fout, "\n\n#error \"Convertor finished with errors\"\n");
   fclose(fout);
   if (fdeb) 	
   if (fdeb) 	printf("%s", "+");
   dumpAll(TREE);
   dump(Stack);
   ;
   if (rc && fdeb==0) 	remove(outfile);	/* remove wrong output */

/*    close the other files                                 */

   if (fin) 	fclose(fin);
   if (fmsg) 	fclose(fmsg);
   if (fdeb) 	fclose(fdeb);

/*    report on the number of errors and warnings           */

   if (ERRno) 	fprintf(stderr, "\n****\t%d error(s) found. (See 'cdvm_msg')\n", ERRno);
   else
   if (WNGno && OPTw) 	fprintf(stderr, "\n****\t%d warning(s). (See 'cdvm_msg')\n", WNGno);
   else remove(msgfile);
   exit(rc);
   }

/* Try to open a file. Stop if open failed.                 */

void Open(FILE**pf, char *name, char *mode)
   {
   if ((*pf=fopen(name, mode))==0) 	
      {
      sprintf(bf, "Can not open \'%s\'\n", name);
         {
         fprintf(stderr, "\n%s\n", bf);
         exit(-1);
         }
      ;
      }
   }

/* Move pointer 'cc' over blank characters                  */

void SkipBlank(void )
   {
   for (; *cc==' ' || *cc=='\t'; cc++) 	;
   }

/* Read and adjust the next input line                      */

void getLine(void )
   {
   lineno++;	/* increment the line counter */
   if (!fgets(bf, sizeof (bf), fin)) 	strcpy(bf, "\032 End of file\n");	/* EOF for scanner */

/*    Skip initial spaces and confirm EOL ('\n')            */

   cc=bf;
   SkipBlank();
   w=strlen(cc);
   memmove(bf, cc, w);
   bf[w]='\n';
   bf[w+1]=0;
   }

/* Read the next significant line.                          */

void nextLine(void )
   {
loop:
   if (bf[0]!=0x1a) 	getLine();

/*    Process '#if 0'...'#endif' directive, ie. skip lines  */

   if (memcmp(bf, "#if ", 4)==0) 	
      {
      cc=bf+4;
      SkipBlank();
      if (memcmp(cc, "0\n", 2)==0) 	
         {
         int iflevel=1;	/* the level of enclosed '#if' */
         int ln0=lineno;	/* start line for error message */
         if (OPTv>1) 	printf("\nSkipping '#if 0': lines %d..", lineno);
         while (iflevel) 	
            {
            getLine();
            if (bf[0]==0x1a) 	
               {
               sprintf(bf, "'#endif' not found for '#if 0' in line %d\n", ln0);
                  {
                  fprintf(stderr, "\n%s\n", bf);
                  exit(-1);
                  }
               ;
               }
            if (memcmp(bf, "#if", 3)==0) 	iflevel++;	/* increase the level */
            if (memcmp(bf, "#endif", 6)==0) 	iflevel--;	/* decrease the level */
            }
         if (OPTv>1) 	printf("%d\n", lineno);	/* final line number */
         goto loop;
         }
      }
   cp=bf;	/* starting position for the scanner */
   }

/* Immediatly output a source line                          */

void putLine(void )
   {
   for (i=strlen(bf); i && bf[i-1]<=0x0d; i--) 	
      {
      bf[i-1]=0;
      }
   fprintf(fout, "%s\n", bf);
   }

/* Process an error                                         */

void ERR(char *msg)
   {
   int rc;	/* severity code of the message */
/*  9     -- fatal error -- stop execution                  */
/*  5..8  -- error                                          */
/*  2,3,4 -- default warning -- desabled by the -w- option  */
/*  0, 1  -- weak warning -- enabled by the -w option       */
   int lno=Stack?-D(Stack): lineno;	/* hte line number */
   ASSERT(msg!=0, ERR);
   rc=msg[0]-'0';	/* first char is a severity code */
   msg+=2;	/* bypass it */

/*    issue message and increase counters                   */

   if (rc>=OPTw) 	
      {
      fprintf(fmsg, "\n%s(%d)\t%s: %s \n", infile, lno, rc>=5?"ERROR": "WNG", msg);
      if (fdeb) 	
         {
         printf("\n%s(%d)\t%s: %s \n", infile, lno, rc>=5?"ERROR": "WNG", msg);
         }
      if (rc>=5) 	ERRno++;
      else WNGno++;
      }

/* termitane execution if the error is too serious          */

   if (rc>=9 || rc<0) 	FINISH(rc);
   }

/* Access to hash tables.                                   */
/*        item, len   -- object                             */
/*        ht          -- pointer to a table                 */
/*        ht_cmp      -- compare function                   */
/*        ht_new      -- write new function                 */
/* Find|write and return contents of a cell of the table    */

typedef int (*htfun)(char *, int );	/* function type */
int HTfind(char *item, int len, int *ht, htfun ht_cmp, htfun ht_new)
   {
   int h=1, i, rehash=0;
   char *p=item;

/*    evaluate [initial] hash value                         */

   for (i=0; i<(len); i++) 	
      {
      h=h*1997+(*p++);
      }

/*    while a cell is not empty, rehash                     */

   for (h=h%(HTsize); ht[h]; h=(h+*item+1997)%(HTsize)) 	
      {
      ASSERT(rehash++<1000, HTfind);
      if (ht_cmp(item, ht[h])) 	break ;	/* OK: item found */
      }
   if (ht[h]==0) 	ht[h]=ht_new(item, len);	/* item is new */
   return ht[h];
   }

/* Initialize the list of tokens (delimiters, keywords)     */

void LXinit(void )
   {
   LXdlm[0]=0;	/* disable delimiters in the scanner */
   Token("TOKEN");
   wD(TOKEN, 0);
   Token("DLM");
   wD(DLM, 0);
   Token("KWD");
   wD(KWD, 0);
   Token("LXX");
   wD(LXX, 0);
   Token("LXANY");
   wD(LXANY, 0);
   Token("IDENT");
   wD(LXI, 0);
   Token("NUMBER");
   wD(LXN, 0);
   Token("LXC");
   wD(LXC, 0);
   Token("REAL");
   wD(LXR, 0);
   Token("STRING");
   wD(LXS, 0);
   Token("CPP");
   wD(CPP, 0);
   Token("NewLine");
   wD(NL, 0);
   Token("STRINGs");
   wD(LXSs, 0);
   Token("__DELIMITERS__");
   wD(__DELIMITERS__, 0);
   Token(";");
   wD(SEMIC, 0);
   Token(",");
   wD(COMMA, 0);
   Token("(");
   wD(LBA, 0);
   Token("{");
   wD(LBS, 0);
   Token("[");
   wD(LBK, 0);
   Token(")");
   wD(RBA, 0);
   Token("]");
   wD(RBK, 0);
   Token("}");
   wD(RBS, 0);
   Token(".");
   wD(POINT, 0);
   Token(".*");
   wD(PNT_AST, 0);
   Token("...");
   wD(PPPOINT, 0);
   Token("?");
   wD(QUEST, 0);
   Token("#");
   wD(NOM, 0);
   Token("##");
   wD(DBL_NOM, 0);
   Token(":");
   wD(COLON, 0);
   Token("::");
   wD(DBL_COLON, 0);
   Token("/");
   wD(DIV, 0);
   Token("/=");
   wD(ADIV, 0);
   Token("/*");
   wD(COMM, 0);
   Token("//");
   wD(CPPCOMM, 0);
   Token("%");
   wD(MOD, 0);
   Token("%=");
   wD(AMOD, 0);
   Token("+");
   wD(ADD, 0);
   Token("+=");
   wD(AADD, 0);
   Token("++");
   wD(INC, 0);
   Token("-");
   wD(SUB, 0);
   Token("-=");
   wD(ASUB, 0);
   Token("--");
   wD(DEC, 0);
   Token("->");
   wD(ARROW, 0);
   Token("->*");
   wD(ARR_AST, 0);
   Token("*");
   wD(MUL, 0);
   Token("*=");
   wD(AMUL, 0);
   Token("=");
   wD(ASSIGN, 0);
   Token("==");
   wD(EQU, 0);
   Token("<");
   wD(LT, 0);
   Token("<=");
   wD(LE, 0);
   Token("<<");
   wD(LSH, 0);
   Token("<<=");
   wD(ALSH, 0);
   Token(">");
   wD(GT, 0);
   Token(">=");
   wD(GE, 0);
   Token(">>");
   wD(RSH, 0);
   Token(">>=");
   wD(ARSH, 0);
   Token("~");
   wD(BNOT, 0);
   Token("!");
   wD(NOT, 0);
   Token("!=");
   wD(NEQ, 0);
   Token("&");
   wD(BAND, 0);
   Token("&=");
   wD(AAND, 0);
   Token("&&");
   wD(AND, 0);
   Token("|");
   wD(BOR, 0);
   Token("|=");
   wD(AOR, 0);
   Token("||");
   wD(OR, 0);
   Token("^");
   wD(BXOR, 0);
   Token("^=");
   wD(AXOR, 0);
   Token("__KEYWORDS__");
   wD(__KEYWORDS__, 0);
   Token("return");
   wD(RETURN, 0);
   Token("if");
   wD(IF, 0);
   Token("else");
   wD(ELSE, 0);
   Token("while");
   wD(WHILE, 0);
   Token("do");
   wD(DO, 0);
   Token("for");
   wD(FOR, 0);
   Token("break");
   wD(BREAK, 0);
   Token("continue");
   wD(CONTINUE, 0);
   Token("switch");
   wD(SWITCH, 0);
   Token("case");
   wD(CASE, 0);
   Token("default");
   wD(DEFAULT, 0);
   Token("goto");
   wD(GOTO, 0);
   Token("sizeof");
   wD(SIZEOF, 0);
   Token("typedef");
   wD(TYPEDEF, 0);
   Token("extern");
   wD(EXTERN, 0);
   Token("static");
   wD(STATIC, 0);
   Token("register");
   wD(REGISTER, 0);
   Token("auto");
   wD(AUTO, 0);
   Token("const");
   wD(CONST, 0);
   Token("int");
   wD(INT, 0);
   Token("short");
   wD(SHORT, 0);
   Token("long");
   wD(LONG, 0);
   Token("void");
   wD(VOID_, 0);
   Token("char");
   wD(CHAR, 0);
   Token("signed");
   wD(SIGNED, 0);
   Token("unsigned");
   wD(UNSIGNED, 0);
   Token("float");
   wD(FLOAT, 0);
   Token("double");
   wD(DOUBLE, 0);
   Token("struct");
   wD(STRUCT, 0);
   Token("union");
   wD(UNION, 0);
   Token("enum");
   wD(ENUM, 0);
   Token("DVM");
   wD(LX_DVM, 0);
   Token("PROCESSORS");
   wD(PROCESSORS, 0);
   Token("DISTRIBUTE");
   wD(DISTRIBUTE, 0);
   Token("BLOCK");
   wD(BLOCK, 0);
   Token("GENBLOCK");
   wD(GENBLOCK, 0);
   Token("WGTBLOCK");
   wD(WGTBLOCK, 0);
   Token("MULT_BLOCK");
   wD(MULTBLOCK, 0);
   Token("ONTO");
   wD(ONTO, 0);
   Token("ALIGN");
   wD(ALIGN, 0);
   Token("WITH");
   wD(WITH, 0);
   Token("TEMPLATE");
   wD(TEMPLATE, 0);
   Token("SHADOW");
   wD(SHADOW, 0);
   Token("SHADOW_GROUP");
   wD(LX_SG, 0);
   Token("REDUCTION_GROUP");
   wD(LX_RG, 0);
   Token("REMOTE_GROUP");
   wD(LX_RMG, 0);
   Token("INDIRECT_GROUP");
   wD(LX_IG, 0);
   Token("TASK");
   wD(TASK, 0);
   Token("REDISTRIBUTE");
   wD(REDISTRIBUTE, 0);
   Token("REALIGN");
   wD(REALIGN, 0);
   Token("NEW");
   wD(NEW, 0);
   Token("CREATE_TEMPLATE");
   wD(LX_CRTEMP, 0);
   Token("PARALLEL");
   wD(PARALLEL, 0);
   Token("ON");
   wD(ON, 0);
   Token("REDUCTION");
   wD(REDUCTION, 0);
   Token("SHADOW_RENEW");
   wD(SHRENEW, 0);
   Token("ACROSS");
   wD(ACROSS, 0);
   Token("IN");
   wD(ACRIN, 0);
   Token("OUT");
   wD(ACROUT, 0);
   Token("PIPE");
   wD(PIPE, 0);
   Token("CREATE_SHADOW_GROUP");
   wD(LX_CRSG, 0);
   Token("CORNER");
   wD(CORNER, 0);
   Token("SHADOW_START");
   wD(SHSTART, 0);
   Token("SHADOW_WAIT");
   wD(SHWAIT, 0);
   Token("SUM");
   wD(SUM, 0);
   Token("PRODUCT");
   wD(PROD, 0);
   Token("MAX");
   wD(MAX, 0);
   Token("MIN");
   wD(MIN, 0);
   Token("OR");
   wD(LX_OR, 0);
   Token("AND");
   wD(LX_AND, 0);
   Token("MAXLOC");
   wD(MAXLOC, 0);
   Token("MINLOC");
   wD(MINLOC, 0);
   Token("REDUCTION_START");
   wD(RSTART, 0);
   Token("REDUCTION_WAIT");
   wD(RWAIT, 0);
   Token("REMOTE_ACCESS");
   wD(REMOTE, 0);
   Token("INDIRECT_ACCESS");
   wD(INDIRECT, 0);
   Token("PREFETCH");
   wD(PREFETCH, 0);
   Token("RESET");
   wD(RESET, 0);
   Token("MAP");
   wD(MAP, 0);
   Token("TASK_REGION");
   wD(LX_TASKREG, 0);
   Token("INTERVAL");
   wD(INTERVAL, 0);
   Token("DEBUG");
   wD(DEBUG, 0);
   Token("BARRIER");
   wD(BARRIER, 0);
   Token("COPY_FLAG");
   wD(LX_CP, 0);
   Token("COPY");
   wD(COPY, 0);
   Token("COPY_START");
   wD(CPSTART, 0);
   Token("COPY_WAIT");
   wD(CPWAIT, 0);
   Token("__LAST_KEYWORD__");
   wD(__LAST_KEYWORD__, 0);
   Token("INCA");
   wD(INCA, 0);
   Token("DECA");
   wD(DECA, 0);
   Token("LXcast");
   wD(LXcast, 0);
   Token("ADDR");
   wD(ADDR, 0);
   Token("CONT");
   wD(CONT, 0);
   Token("LXskip");
   wD(LXskip, 0);
   Token("LXaster");
   wD(LXaster, 0);
   Token("XXdecl");
   wD(XXdecl, 0);
   Token("XXtype");
   wD(XXtype, 0);
   Token("XXdecls");
   wD(XXdecls, 0);
   Token("XXfields");
   wD(XXfields, 0);
   Token("XXfield");
   wD(XXfield, 0);
   Token("XXenum");
   wD(XXenums, 0);
   Token("XXdcltr");
   wD(XXdcltr, 0);
   Token("LXfun");
   wD(LXfun, 0);
   Token("LXfunKR");
   wD(LXfunKR, 0);
   Token("XXbody");
   wD(XXbody, 0);
   Token("XXoper");
   wD(XXoper, 0);
   Token("XXclist");
   wD(XXclist, 0);
   Token("XXslist");
   wD(XXslist, 0);
   Token("XXlist");
   wD(XXlist, 0);
   Token("XXexpr");
   wD(XXexpr, 0);
   Token("XXparm");
   wD(XXparm, 0);
   Token("NUMBER_OF_PROCESSORS");
   wD(LX_NUMBER_OF_PROC, 0);
   Token("FOR");
   wD(LX_FOR, 0);
   Token("DO");
   wD(LX_DO, 0);
   Token("main");
   wD(LX_main, 0);
   Token("malloc");
   wD(LX_malloc, 0);
   Token("free");
   wD(LX_free, 0);
   Token("printf");
   wD(LX_printf, 0);
   Token("DVM_FOR");
   wD(DVM_FOR, 0);
   Token("DVM_FOR_1");
   wD(DVM_FOR_1, 0);
   Token("DVM_REDBLACK");
   wD(DVM_RB, 0);
   Token("DVM_DOPL");
   wD(DVM_DOPL, 0);
   Token("TaskLoop");
   wD(TASKLOOP, 0);
   Token("LXIrg");
   wD(LXIrg, 0);
   Token("DVMbase");
   wD(DVMbase, 0);
   Token("DVMvar");
   wD(DVMvar, 0);
   Token("DVMind");
   wD(DVMind, 0);
   Token("DVMbind");
   wD(DVMbind, 0);
   Token("DVMalign");
   wD(DVMalign, 0);
   Token("LXIsg");
   wD(LXIsg, 0);
   Token("DVMshw");
   wD(DVMshw, 0);
   Token("DVMshad");
   wD(DVMshad, 0);
   Token("DVMremote");
   wD(DVMremote, 0);
   Token("LXIag");
   wD(LXIag, 0);
   Token("LXItask");
   wD(LXItask, 0);
   Token("LXIproc");
   wD(LXIproc, 0);
   Token("DTscal");
   wD(DTscal, 0);
   Token("d0");
   wD(optD0, 0);
   Token("d1");
   wD(optD1, 0);
   Token("d2");
   wD(optD2, 0);
   Token("d3");
   wD(optD3, 0);
   Token("d4");
   wD(optD4, 0);
   Token("e0");
   wD(optE0, 0);
   Token("e1");
   wD(optE1, 0);
   Token("e2");
   wD(optE2, 0);
   Token("e3");
   wD(optE3, 0);
   Token("e4");
   wD(optE4, 0);
   Token("1");
   wD(ONE, 0);
   Token("2");
   wD(TWO, 0);
   Token("__LAST_CODE__");
   wD(__LAST_CODE__, 0);
   ASSERT(LXval==LASTLEX-1, LXinit);
   LXdlm[0]=-1;	/* enable delimiters in the scanner */
   }

/*    'cmp' hash function for the tokens table              */

int LX_cmp(char *c, int h)
   {
   return !strcmp(c, LX-A(h));
   }

/*    'new' hash function for the tokens table              */

int LX_new(char *p, int len)
   {
   LXfree+=len;
   return NDD(TOKEN, -(p-LX), 0, 0);
   }

/* Find|write the curren token. Set global 'LXval'.         */

int LXfind(void )
   {
   int len=(int )(cp-cc);	/* the length */
   if ((LXfree+len+1)>=LXSIZE) 	
      {
      fprintf(stderr, "\n%s\n", "Token-table overflow");
      exit(-1);
      }
   ;

/*    write to the free space of the string memory          */

   memmove(LX+LXfree, cc, len);
   LX[LXfree+len]=0;

/*    find it, or 'LX_new' takes it in                      */

   LXval=HTfind(LX+LXfree, len+1, HT0, LX_cmp, LX_new);
   return LXval;
   }

/* Move 'cp' over chars of type 't' (to the end of token)   */

void LX_get(char t)
   {
   for (; (LXcd[(*cp)&255]|t)==t; cp++) 	;
   }

/* Skip a comment.                                          */

int comment(void )
   {
   for (; ; ) 	
   switch (*cp++)
      {
   case '*':
      if (*cp=='/') 	
         {
         cp++;
         return 1;
         }
      ;
      if (cp[-2]=='/') 	
         {
         ERR("2:Nested comment?");
         }
      break ;
   case '\n': nextLine();
      break ;
   case 0:
   case 0x1a: return 0;
      }
   }
/*----------------------------------------------------------*/
/* FUNCTION:  scan                                          */
/* Purpose:                                                 */
/*Main scanner function                                     */
/* Input:                                                   */
/*none                                                      */
/* Output:                                                  */
/*Globals 'LXcode', 'LXval', 'cc', 'cp' updated             */
/*/
                                                        */
void scan(void )
   {
   int i;

/*    loop to skip white tokens                             */

loop:
   cc=cp;	/* mark a start position */

/*    switching by the code of current character            */

   switch (LXcd[(*cp++)&255])
      {

/*    LXO, LXH, LXN (а м.б. LXR)                            */

   case '0':
      if (*cc=='0' && (cc[1]=='x' || cc[1]=='X')) 	
         {
         cc=++cp;
         LX_get('1');	/* hex */
         cc-=2;
         }
      else LX_get('0');	/* oct || dec */
      if ((*cc=='0' && cp-cc!=1) || (*cp!='.' && *cp!='e' && *cp!='E')) 	
         {
         if (*cp=='l' || *cp=='L') 	
            {
            cp++;
            if (*cp=='u' || *cp=='U') 	cp++;
            }
         else
         if (*cp=='u' || *cp=='U') 	
            {
            cp++;
            if (*cp=='l' || *cp=='L') 	cp++;
            }
         LXcode=LXN;
         LXfind();
         break ;	/* exit with LXN */
         }

/*    LXR                                                   */

real:
      if (*cp=='.') 	
         {
         cp++;
         LX_get('0');	/* are there digits? */
         if (cp==cc+1) 	
            {
            LXval=POINT;
            LXcode=DLM;	/* no: exit with a sole point */
            break ;
            }
         }
      if (*cp=='e' || *cp=='E') 	
         {
         cp++;
         if (*cp=='+' || *cp=='-') 	cp++;
         LX_get('0');	/* exp */
         }
      if (*cp=='l' || *cp=='L') 	
         {
         cp++;
         if (*cp=='f' || *cp=='F') 	cp++;
         }
      else
      if (*cp=='f' || *cp=='F') 	
         {
         cp++;
         if (*cp=='l' || *cp=='L') 	cp++;
         }
      LXcode=LXR;
      LXfind();
      break ;	/* exit with LXR */

/* IDENTIFIER or KEYWORD                                    */

   case '1':
   case '2': cp=cc;
      LX_get('3');	/* letters and digits */
      LXcode=LXI;
      LXfind();
      if (LXval>__KEYWORDS__ && LXval<__LAST_KEYWORD__) 	LXcode=KWD;	/* it is a keyword */
      break ;

/*    STRING "..."                                          */

   case '3':
      do
         {
         while (*cp++!='\"') 	
            {
            if (*cp=='\\') 	cp++;
            if (*cp=='\n') 	
               {
               ERR("9:UNTERMINATED STRING");
               nextLine();
               }
            }
         }
      while (cp[-2]=='\\') 	;	/* \" is not the end of string */
      LXcode=LXS;
      LXfind();
      break ;	/* exit with LXS */

/*    CHAR '...' -- is treated as a number                  */

   case '4':
      while (*cp!='\n' && (*cp!='\'')) 	
         {
         if (*cp=='\\') 	cp++;
         cp++;
         }
      if (*cp=='\n') 	ERR("9:CHAR missing '\'' ");
      cp++;
      LXcode=LXN;
      LXfind();
      break ;	/* exit with LXN !! */

/*    '.' -> may be the beginning or a float '.0'           */

   case '5':
      if (*cp>='0' && *cp<='9') 	
         {
         cp--;
         goto real;
         }
      goto dlmtr;

/*    '#' -> Preprocessor directive                         */

   case '6':
      if (cc!=bf) 	goto dflt;	/* not first character in the line */
      while (*cp==' ' || *cp=='\t') 	cp++;	/* skip blank chars */
      if (OPTm==0 || OPTm==1 && (memcmp(cp, "define", 6)==0 || memcmp(cp, "undef", 5))) 	
         {
         putLine();	/* output at once */
         goto newline;
         }
      for (; *cp && *cp!=0x0d && *cp!=0x0a; cp++) 	;
      *cp++='\n';
      *cp='\n';
      LXcode=CPP;
      LXfind();
      break ;	/* exit with CPP */

/*    (delimiter  1-4 char)                                 */

   case '7': dlmtr:
      if (LXdlm[0]==0) 	goto dflt;	/* disabled */
      for (i=LXdlm[(*cc)&255]; i; i--) 	
         {
         char *p=LX-A(i);	/* the latest delimiter */
         if (*(cp=cc)!=*p) 	break ;	/* not found */
         if (*++cp==*++p && *++cp==*++p && *++cp==*++p && *++cp==*++p) 	
            {
               {
               fprintf(stderr, "\n%s\n", "SCANNER FAULT [LXdlm]");
               exit(-1);
               }
            ;
            }
         else
         if (*p==0) 	break ;	/* OK */
         }
      LXval=i;	/* the code of delimiter */
      if (LXval==COMM) 	
         {
         int commorg=lineno;	/* start line for message */
         if (comment()==0) 	
            {
            lineno=commorg;
            ERR("9:Unterminated comment");
            }
         goto loop;	/* continue scanning */
         }
      if (LXval==CPPCOMM) 	
         {
         goto newline;	/* skip line and continue scanning */
         }
      LXcode=DLM;
      break ;	/* exit with DLM */

/*    new line                                              */

   case '9': newline: nextLine();	/* read next line */

/*    blanc char                                            */

   case '8': goto loop;	/* continue scanning */

/*    illegal character                                     */

   default : dflt:
      if (*cc==0x1a || *cc==0) 	
         {
         LXcode=0;
         LXval=0;
         break ;	/* exit with EOF */
         }

/*    If disabled, it is initialization.                    */

      else
      if (LXdlm[0]==0) 	
         {
         while (*cp && *cp!=' ') 	cp++;
         LXcode=__DELIMITERS__;
         LXfind();
         if (LXval>=NL) 	
            {
            w=(*cc)&255;
            if (LXcd[w]=='?') 	LXcd[w]='7';	/* mark a type of the character */
            LXdlm[w]=LXval;	/* finally remains the last*/
            }
         break ;
         }

/*    if enabled, issue an error message                    */

      else
         {
         char msg[64];
         sprintf(msg, "5:Invalid character \'%c\' 0x%.02X", *cc, (*cc)&255);
         ERR(msg);
         goto loop;	/* continue scanning */
         }
      }
   }

/* Find a character representation of a token by the code   */

char *LXW(int token)
   {
   if (C(token)==TOKEN && A(token)<0) 	return LX-A(token);
   else return "non-token";
   }

/* Create a TOKEN-node for a string                         */

int Token(char *s)
   {
   ASSERT(s!=0, Token);
   cp=s;
   scan();
   return LXval;
   }

/* Convert a string (with spaces!) to a LXI-node            */

int LxiS(char *s)
   {
   cc=s;
   cp=s+strlen(s);
   LXfind();
   return mk(LXI, LXval, 0);
   }

/* Create an LXI-node for a token                           */

int Lxi(int token)
   {
   ASSERTC(TOKEN, token, Lxi);
   return NDD(LXI, token, 0, 0);
   }

/* Create an LXN-node for a number                          */

int Lxn(int n)
   {
   int r;
   if (n<0) 	return NDD(SUB, 0, Lxn(-n), 0);
   sprintf(bf, "%d", n);
   r=mk(LXN, Token(bf), 0);
   return r;
   }

/* Initialize the linked memory. Create a free list         */

void NDinit()
   {
   struct ND*p;
   for (w=0; w<(NDsize); w++) 	
      {
      p=&NDs[w];
      p->a=p->b=p->c=0;
      p->d=w+1;
      }
   NDs[0].d=0;
   NDs[NDsize-1].d=0;
   NDfree=1;
   }

/* Free the node N                                          */

void NDdel(int N)
   {
   if (N) 	
      {
      wN(N, 0, 0, 0);
      wD(N, NDfree);
      NDfree=N;
      }
   }

/* 'cmp' hash function for nodes                            */

int ND_cmp(char *p, int n)
   {
   int *q=(int *)p;
   return q[0]==C(n) && q[1]==A(n) && q[2]==B(n);
   }

/* 'new' hash function for nodes                            */

int ND_new(char *p, int len)
   {
   int *q=(int *)p;
   return NDD(q[0], q[1], q[2], 0&len);
   }

/* Create a node <c,a,b,d>                                  */

int NDD(int c, int a, int b, int d)
   {
   int N=NDfree;	/* get the first free */
   struct ND*p=&NDs[N];	/* points to it */
   if (N==0) 	
      {
         {
         fprintf(stderr, "\n%s\n", "TREE OVERFLOW\n");
         exit(-1);
         }
      ;
      }
   if ((c|a|b|d)==0) 	return 0;	/* do not waste space */
   NDfree=p->d;	/* push free list */
   p->c=c;
   p->a=a;
   p->b=b;
   p->d=d;	/* fill the node */
   return N;
   }

/* Create or FIND a node <c,a,b,...>                        */

int mk(int c, int a, int b)
   {
   int p[3];
   p[0]=c;
   p[1]=a;
   p[2]=b;
   return HTfind((char *)&p, sizeof (p), HT1, ND_cmp, ND_new);
   }

/* Get a field by a 'path'. Eg. (N,123) equ. C(A(B(N)))     */

int NDget(int N, int n)
   {
   if (n==9999) 	return 0;	/*99-3*/
   for (; n; n/=10) 	
   switch (n%10)
      {
   case 1: N=C(N);
      break ;
   case 2: N=A(N);
      break ;
   case 3: N=B(N);
      break ;
   case 4: N=D(N);
      break ;
   default :
         {
         fprintf(stderr, "\n%s\n", "CDVM-C FAULT [NDget]");
         exit(-1);
         }
      ;
      }
   if (N<0) 	N=0;
   return N;
   }

/* Moving through lists (in particular, through the stack)  */

int Count;	/* steps counter */
int up(int n)
   {
   for (Count=0, w=Stack; w; Count++, w=B(w)) 	
   if (n--==0) 	break ;
   return A(w);
   }
int to(int c)
   {
   for (Count=0, w=Stack; w; Count++, w=B(w)) 	
   if (C(w)==c) 	break ;
   return A(w);
   }
int skipA(int c, int N)
   {
   for (Count=0, N=N; C(N)==c; Count++, N=A(N)) 	;
   return N;
   }
int toA0(int c, int N)
   {
   for (Count=0, N=N; N && C(N)!=c; Count++, N=A(N)) 	;
   return N;
   }
int toA(int c, int N)
   {
   for (Count=0, N=N; N && C(N)!=c; Count++, N=A(N)) 	;
   ASSERT(N!=0, toA);
   return N;
   }
int skipB(int c, int N)
   {
   for (Count=0, N=N; N; Count++, N=B(N)) 	
   if (C(N)!=c) 	break ;
   return N;
   }
int toB0(int c, int N)
   {
   for (Count=0, N=N; N; Count++, N=B(N)) 	
   if (C(N)==c) 	break ;
   return N;
   }
int toB(int c, int N)
   {
   for (Count=0, N=N; N; Count++, N=B(N)) 	
   if (C(N)==c) 	break ;
   ASSERT(N!=0, toB);
   return N;
   }
int SSfind(int N, int f, int c)
   {
   for (Count=0, N=N; N; Count++, N=B(N)) 	
      {
      w=NDget(A(N), f);
      if (C(w)==c) 	return w;
      }
   return 0;
   }

/*    Revert B-links in the B-list N                        */

int revert(int N)
   {
   int w=0, i;
   while (N) 	
      {
      i=B(N);
      wB(N, w);
      w=N;
      N=i;
      }
   return w;
   }

/* 'Push' on the 'tmpList'                                  */

void conc(int N)
   {
   tmpList=NDD(0, N, tmpList, 0);
   }

/* 'Pop' on the 'tmpList'                                   */

int head()
   {
   ASSERT(tmpList!=0, head());
   tmpHead=A(tmpList);
   tmpTail=B(tmpList);
   NDdel(tmpList);
   tmpList=tmpTail;
   return tmpHead;
   }

/* The number of operands. (The length of a list)           */

int LenR(int c, int N)
   {
   return C(N)!=c?1: LenR(c, A(N))+LenR(c, B(N));
   }
int Len(int c, int N)
   {
   return N==0?0: LenR(c, N);
   }

/* Apply Right|Left-associative operation                   */

int Rop(int c, int x, int y)
   {
   return C(x)==c?mk(c, A(x), Rop(c, B(x), y)): mk(c, x, y);
   }
int Lop(int c, int x, int y)
   {
   return C(y)==c?mk(c, Lop(c, x, A(y)), B(y)): mk(c, x, y);
   }

/* Set value V to a property c of a node N                  */

void set(int c, int N, int V)
   {
   if (-D(N)>1 && (w=toB0(c, -D(N)))!=0) 	wA(w, V);	/* already exists -> write */
   else
      {
      w=-D(N)>1?-D(N): 0;	/* existing properties list */
      wD(N, -NDD(c, V, w, 0));	/* Create and attach as first */
      }
   }

/* Get the value of a property c of a node N                */

int get(int c, int N)
   {
   return A(toB0(c, -D(N)));
   }

/* Delete a property c of a node N                          */

void del(int c, int N)
   {
   int Prev=0, n;
   if (D(N)>=0) 	return ;	/* properties list is absent */

/*    scan properties list, tracking the 'previous' node    */

   for (Count=0, n=-D(N); n; Count++, n=B(n)) 	
      {
      if (C(n)==c) 	break ;
      else Prev=n;
      }

/*    if the property found                                 */

   if (n) 	
      {
      if (Prev!=0) 	wB(Prev, B(n));	/* correct the previous */
      else wD(N, B(n));	/* the first node deleted */
      NDdel(n);	/* free the node */
      }
   }

/* Push the Stack                                           */

void SSpush(int N)
   {
   Stack=NDD(C(N), N, Stack, 0);
   }

/* Pop the Stack                                            */

void SSpop(void )
   {
   int N=Stack;
   Stack=B(Stack);
   NDdel(N);
   }

/* recursive part of the 'walk'                             */

int (*SEM)(int IN)=0;	/* active semantic function */
void walkR(int N)
   {
   int ab;
   if (N==0) 	return ;
   SSpush(N);	/* push the Stack */
   ab=SEM(1);	/* the first (descending) call of the SEM */

/* SEM-guided descent to the A and B branches               */

   if (ab&1) 	
      {
      if (A(N)>0) 	walkR(A(N));
      }
   if (ab&2) 	
      {
      if (B(N)>0) 	walkR(B(N));
      }
   if (ab) 	SEM(0);	/* the last (ascending) call of the SEM */
   SSpop();	/* pop the Stack */
   }

/* Walk through the subtree N with sem.function 'semproc'   */

int walk(int N, int (*semproc)(int IN))
   {
   int (*oldsem)(int IN)=SEM;	/* current sem. function */
   SEM=semproc;	/* change active sem. function */
   SSpush(LXANY);	/* push the Stack */
   walkR(N);	/* recursive walk through the subtree */
   N=D(Stack);
   SSpop();	/* pop the Stack */
   SEM=oldsem;	/* restore active sem.function */
   return N;
   }

/*----------------------------------------------------------*/
/*    Syntax tree creation                                  */
/*----------------------------------------------------------*/

int elist;	/* the elements list */
int rlist;	/* the rules list */
int rno;
int gno;

/* Create groups list                                       */

void STXgrlist(int g, int sg)
   {
   g=g;
   rlist=NDD(0, -1, rlist, 0);
   if (!sg) 	wC(rlist, rlist);
   }

/* The end of a group                                       */

void STXgroup(int s_g, int g, char *s)
   {
   int N=g;
   wD(N, Token(s));	/* group identidifer */
   if (s_g==0) 	
      {
      revert(rlist);	/* revert list of rules */
      rlist=0;
      }
   gno++;	/* next group number */
   rno=0;
   }

/* Add element to the elements list                         */

void STXelem(int n, int g, int opt)
   {
   elist=NDD(0, g, elist, -n-10000*opt);
   }

/* Add terminal element to the elements list                */

void STXterm(int lx)
   {
   elist=NDD(0, lx, elist, -9999);
   }

/* The end of a rule                                        */

void STXrule(int type, int c)
   {

/* Revert the elements list and add it to the rules list    */

   rlist=NDD(c, revert(elist), rlist, -type);
   elist=0;
   if (rno++==0) 	wA(gno, rlist);	/* reference to the first rule in a group */
   }

/* Syntax debugging                                         */

void STXtrc(void )
   {
   elist=NDD(0, 0, elist, 0);
   }
int SNTtrace(void )
   {
   printf("I_AM_HERE (%d) %d\n", curlex, D(curlex));
   return 0;
   }

/* Symbolic names for syntactic groups                      */

enum
   {
   Group0=LASTLEX-1, g00, g01, g02, cpp, gDecl, gDeclL, gXspec,
   gStcls, gType, g05, gXstruct, g06, gXenum, g08, g09,
   g10, gDcltr, gDcltr1, gXdcl, gXinit, g14, g15, gXnextdcl,
   gXargs, gXarg1, gParm, gParm3, gParm32, gDfun, gDfun2, gDfun32,
   gDbody, gIDs, gDargs, gDargs2, gDarg22, gXnextAdcl, Compound_stmt_body, g22, 
   gXdclop, gOper, Else_branch, Switch_body, gXcase1, gXcase3, gXcase32, gBOper,
   g290, g291, g292, g293, g294, g299, gXdo, Expression,
   Assignement_prt_expr, Conditional_prt_expr, g34, g35, g36, g37, g38, g39,
   g40, g41, Addition_prt_expr, g43, Unary_expr, Primary_expr, Bracketed_expr, g46,
   gNID, Identifier, gLXS, gXsizeof, g50, gXtname, gXadcl, gDVM,
   DVM_directive, Shadow_group, Reduction_group, Red_group_name, Task_name, Task, Distr_directive, Redistr_directive,
   Distr_stuff, Distr_format, Distr_target, Distr_target1, Distr_mode, Section_subscript, Section_subscript1, Align_directive,
   Realign_directive, Align_stuff, Align_source, Align_with, Align_target, Template, Sub_directive, Parallel_directive,
   Parallel_source, gDvar, Par_clauses, ParClause, Par_clause, Sh_renew_clause, Sh_start_clause, Sh_wait_clause,
   Across_clause, Pipe_clause, Remote_clause, Indirect_clause, Shadow_edge, Renewees, Renewee, SHAlim,
   Shadow_width, Reduction_var, Reduction_op, Access_group, Named_access, References, Reference, gDref,
   gDiref, Debug_modes, LastSTXGroup
   }
;

/* Build the syntax tree                                    */

void STXinit(void )
   {
   rlist=0;

/* Create the list of group headers                         */

   STXgrlist(g00, 0);
   STXgrlist(g01, 0);
   STXgrlist(g02, 1);
   STXgrlist(cpp, 0);
   STXgrlist(gDecl, 0);
   STXgrlist(gDeclL, 1);
   STXgrlist(gXspec, 0);
   STXgrlist(gStcls, 0);
   STXgrlist(gType, 0);
   STXgrlist(g05, 1);
   STXgrlist(gXstruct, 0);
   STXgrlist(g06, 0);
   STXgrlist(gXenum, 0);
   STXgrlist(g08, 0);
   STXgrlist(g09, 1);
   STXgrlist(g10, 1);
   STXgrlist(gDcltr, 0);
   STXgrlist(gDcltr1, 0);
   STXgrlist(gXdcl, 0);
   STXgrlist(gXinit, 0);
   STXgrlist(g14, 0);
   STXgrlist(g15, 1);
   STXgrlist(gXnextdcl, 0);
   STXgrlist(gXargs, 0);
   STXgrlist(gXarg1, 1);
   STXgrlist(gParm, 0);
   STXgrlist(gParm3, 0);
   STXgrlist(gParm32, 0);
   STXgrlist(gDfun, 0);
   STXgrlist(gDfun2, 0);
   STXgrlist(gDfun32, 0);
   STXgrlist(gDbody, 0);
   STXgrlist(gIDs, 0);
   STXgrlist(gDargs, 0);
   STXgrlist(gDargs2, 0);
   STXgrlist(gDarg22, 0);
   STXgrlist(gXnextAdcl, 0);
   STXgrlist(Compound_stmt_body, 0);
   STXgrlist(g22, 0);
   STXgrlist(gXdclop, 0);
   STXgrlist(gOper, 1);
   STXgrlist(Else_branch, 0);
   STXgrlist(Switch_body, 0);
   STXgrlist(gXcase1, 1);
   STXgrlist(gXcase3, 0);
   STXgrlist(gXcase32, 0);
   STXgrlist(gBOper, 0);
   STXgrlist(g290, 0);
   STXgrlist(g291, 0);
   STXgrlist(g292, 0);
   STXgrlist(g293, 0);
   STXgrlist(g294, 0);
   STXgrlist(g299, 0);
   STXgrlist(gXdo, 0);
   STXgrlist(Expression, 0);
   STXgrlist(Assignement_prt_expr, 1);
   STXgrlist(Conditional_prt_expr, 1);
   STXgrlist(g34, 1);
   STXgrlist(g35, 1);
   STXgrlist(g36, 1);
   STXgrlist(g37, 1);
   STXgrlist(g38, 1);
   STXgrlist(g39, 1);
   STXgrlist(g40, 1);
   STXgrlist(g41, 1);
   STXgrlist(Addition_prt_expr, 1);
   STXgrlist(g43, 1);
   STXgrlist(Unary_expr, 1);
   STXgrlist(Primary_expr, 1);
   STXgrlist(Bracketed_expr, 1);
   STXgrlist(g46, 1);
   STXgrlist(gNID, 1);
   STXgrlist(Identifier, 1);
   STXgrlist(gLXS, 0);
   STXgrlist(gXsizeof, 0);
   STXgrlist(g50, 0);
   STXgrlist(gXtname, 0);
   STXgrlist(gXadcl, 0);
   STXgrlist(gDVM, 0);
   STXgrlist(DVM_directive, 0);
   STXgrlist(Shadow_group, 0);
   STXgrlist(Reduction_group, 0);
   STXgrlist(Red_group_name, 0);
   STXgrlist(Task_name, 0);
   STXgrlist(Task, 0);
   STXgrlist(Distr_directive, 0);
   STXgrlist(Redistr_directive, 0);
   STXgrlist(Distr_stuff, 0);
   STXgrlist(Distr_format, 0);
   STXgrlist(Distr_target, 0);
   STXgrlist(Distr_target1, 0);
   STXgrlist(Distr_mode, 0);
   STXgrlist(Section_subscript, 0);
   STXgrlist(Section_subscript1, 1);
   STXgrlist(Align_directive, 0);
   STXgrlist(Realign_directive, 0);
   STXgrlist(Align_stuff, 0);
   STXgrlist(Align_source, 0);
   STXgrlist(Align_with, 0);
   STXgrlist(Align_target, 0);
   STXgrlist(Template, 0);
   STXgrlist(Sub_directive, 0);
   STXgrlist(Parallel_directive, 0);
   STXgrlist(Parallel_source, 0);
   STXgrlist(gDvar, 0);
   STXgrlist(Par_clauses, 0);
   STXgrlist(ParClause, 1);
   STXgrlist(Par_clause, 0);
   STXgrlist(Sh_renew_clause, 1);
   STXgrlist(Sh_start_clause, 1);
   STXgrlist(Sh_wait_clause, 1);
   STXgrlist(Across_clause, 1);
   STXgrlist(Pipe_clause, 1);
   STXgrlist(Remote_clause, 1);
   STXgrlist(Indirect_clause, 1);
   STXgrlist(Shadow_edge, 0);
   STXgrlist(Renewees, 0);
   STXgrlist(Renewee, 0);
   STXgrlist(SHAlim, 0);
   STXgrlist(Shadow_width, 0);
   STXgrlist(Reduction_var, 0);
   STXgrlist(Reduction_op, 0);
   STXgrlist(Access_group, 0);
   STXgrlist(Named_access, 0);
   STXgrlist(References, 0);
   STXgrlist(Reference, 0);
   STXgrlist(gDref, 0);
   STXgrlist(gDiref, 1);
   STXgrlist(Debug_modes, 0);
   STXgrlist(LastSTXGroup, 0);
   SYNTAX=revert(rlist);
   ASSERT(SYNTAX==LASTLEX, STXinit);

/* Create rules                                             */

   rlist=0;
   gno=Group0;
   elist=0;
   rno=0;
   STXgroup(0, g00, "g00");
   STXelem(2, g01, 0);
   STXrule(tR0, 0);

/* program ::= g02                                          */
/*        | program g02                                     */

   STXgroup(0, g01, "g01");
   STXelem(2, g02, 0);
   STXelem(3, gno, 1);
   STXrule(tR0, XXlist);
   STXgroup(1, g02, "g02");
   STXelem(0, cpp, 0);
   STXrule(tRx, 0);
   STXelem(2, cpp, 0);
   STXrule(tR0, CPP);
   STXelem(3, gDVM, 1);
   STXelem(2, gDecl, 0);
   STXrule(tR0, 0);
   STXgroup(0, cpp, "cpp");
   STXelem(0, CPP, 0);
   STXrule(tR0, CPP);
   STXgroup(0, gDecl, "gDecl");
   STXelem(2, gXspec, 1);
   STXelem(3, gDcltr, 0);
   STXrule(tRW, XXdecl);
   STXterm(LXI);
   STXterm(LXX);
   STXterm(LBA);
   STXrule(tRx, 0);
   STXterm(RETURN);
   STXelem(3, gDcltr, 0);
   STXrule(tR0, XXdecl);
   STXgroup(1, gDeclL, "gDeclL");
   STXelem(2, gXspec, 0);
   STXelem(3, gDcltr, 0);
   STXrule(tR0, XXdecl);
   STXgroup(0, gXspec, "gXspec");
   STXelem(2, gStcls, 1);
   STXelem(3, gType, 0);
   STXrule(tR0, 0);

/* storage class ::= static | auto | register | extern      */
/*        | typedef | const                                 */

   STXgroup(0, gStcls, "gStcls");
   STXterm(STATIC);
   STXelem(3, gno, 1);
   STXrule(tR1, STATIC);
   STXterm(AUTO);
   STXelem(3, gno, 1);
   STXrule(tR1, AUTO);
   STXterm(REGISTER);
   STXelem(3, gno, 1);
   STXrule(tR1, REGISTER);
   STXterm(EXTERN);
   STXelem(3, gno, 1);
   STXrule(tR1, EXTERN);
   STXterm(TYPEDEF);
   STXelem(3, gno, 1);
   STXrule(tR1, TYPEDEF);
   STXterm(CONST);
   STXelem(3, gno, 1);
   STXrule(tR1, CONST);

/* type ::=                                                 */

   STXgroup(0, gType, "gType");

/*        struct [LXI] <gXstruct> |                         */

   STXterm(STRUCT);
   STXelem(2, LXI, 1);
   STXelem(3, gXstruct, 1);
   STXrule(tR1, STRUCT);

/*        union [LXI] <gXstruct> |                          */

   STXterm(UNION);
   STXelem(2, LXI, 1);
   STXelem(3, gXstruct, 1);
   STXrule(tR1, UNION);

/*        enum  [LXI] <gXenum> |                            */

   STXterm(ENUM);
   STXelem(2, LXI, 1);
   STXelem(3, gXenum, 1);
   STXrule(tR1, ENUM);

/*        void                                              */

   STXterm(VOID_);
   ;
   STXrule(tR1, VOID_);

/*        <typedef>                                         */

   STXterm(LXI);
   STXterm(LXI);
   STXrule(tRx, 0);
   STXterm(LXI);
   STXterm(MUL);
   STXrule(tRx, 0);
   STXterm(LXI);
   STXterm(LBA);
   STXterm(MUL);
   STXrule(tRx, 0);
   STXelem(3, Identifier, 0);
   STXrule(tR0, TYPEDEF);

/*    | simple_type                                         */

   STXgroup(1, g05, "g05");
   STXterm(UNSIGNED);
   STXelem(3, gno, 1);
   STXrule(tR1, UNSIGNED);
   STXterm(SIGNED);
   STXelem(3, gno, 1);
   STXrule(tR1, SIGNED);
   STXterm(CHAR);
   STXelem(3, gno, 1);
   STXrule(tR1, CHAR);
   STXterm(SHORT);
   STXelem(3, gno, 1);
   STXrule(tR1, SHORT);
   STXterm(LONG);
   STXelem(3, gno, 1);
   STXrule(tR1, LONG);
   STXterm(INT);
   STXelem(3, gno, 1);
   STXrule(tR1, INT);
   STXterm(FLOAT);
   STXelem(3, gno, 1);
   STXrule(tR1, FLOAT);
   STXterm(DOUBLE);
   STXelem(3, gno, 1);
   STXrule(tR1, DOUBLE);

/*        { <fields> }                                      */

   STXgroup(0, gXstruct, "gXstruct");
   STXterm(LBS);
   STXelem(2, gno+1, 0);
   STXterm(RBS);
   STXrule(tR1, LBS);

/*                                                          */

   STXgroup(0, g06, "g06");
   STXelem(2, gDecl, 0);
   STXelem(3, gno, 1);
   STXrule(tR0, XXslist);
   STXgroup(0, gXenum, "gXenum");
   STXterm(LBS);
   STXelem(2, gno+1, 0);
   STXterm(RBS);
   STXrule(tR0, XXenums);
   STXgroup(0, g08, "g08");
   STXelem(2, gno+1, 0);
   STXterm(COMMA);
   STXelem(3, gno, 0);
   STXrule(tR2, COMMA);
   STXgroup(1, g09, "g09");
   STXelem(2, gno+1, 0);
   STXterm(ASSIGN);
   STXelem(3, Conditional_prt_expr, 0);
   STXrule(tR2, ASSIGN);
   STXgroup(1, g10, "g10");
   STXelem(0, LXI, 0);
   STXrule(tR0, LXI);
   STXgroup(0, gDcltr, "gDcltr");
   STXterm(LXI);
   STXterm(LBA);
   STXterm(LXI);
   STXterm(RBA);
   STXrule(tRx, 0);
   STXterm(LXI);
   STXterm(LBA);
   STXterm(LXI);
   STXterm(COMMA);
   STXrule(tRx, 0);
   STXelem(2, gDfun, 0);
   STXrule(tR0, LXfunKR);
   STXelem(2, gDcltr1, 1);
   STXelem(3, gXnextdcl, 0);
   STXrule(tR0, 0);
   STXgroup(0, gDcltr1, "gDcltr1");
   STXelem(2, gXdcl, 0);
   STXelem(3, gXinit, 1);
   STXrule(tR0, XXdcltr);
   STXgroup(0, gXdcl, "gXdcl");
   STXterm(LBA);
   STXelem(2, gno, 0);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXterm(MUL);
   STXelem(3, CONST, 1);
   STXelem(2, gno, 0);
   STXrule(tR1, LXaster);
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Assignement_prt_expr, 1);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, gno, 0);
   STXterm(LBA);
   STXelem(3, gXargs, 1);
   STXterm(RBA);
   STXrule(tR2, LXfun);
   STXelem(0, LXI, 0);
   STXrule(tR0, LXI);
   STXgroup(0, gXinit, "gXinit");
   STXterm(ASSIGN);
   STXelem(2, g15, 0);
   STXrule(tR1, ASSIGN);
   STXterm(COLON);
   STXelem(2, LXN, 0);
   STXrule(tR1, COLON);
   STXgroup(0, g14, "g14");
   STXelem(2, gno+1, 0);
   STXterm(COMMA);
   STXelem(3, gno, 0);
   STXrule(tR2, COMMA);
   STXgroup(1, g15, "g15");
   STXterm(LBS);
   STXelem(2, g14, 0);
   STXterm(RBS);
   STXrule(tR1, LBS);
   STXelem(0, Conditional_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, gXnextdcl, "gXnextdcl");
   STXterm(LBS);
   STXelem(2, Compound_stmt_body, 1);
   STXterm(RBS);
   STXelem(3, SEMIC, 1);
   STXrule(tR1, XXbody);
   STXterm(COMMA);
   STXelem(2, gDcltr, 0);
   STXrule(tR1, XXclist);
   STXterm(SEMIC);
   STXrule(tR0, SEMIC);
   STXgroup(0, gXargs, "gXargs");
   STXelem(2, gno+1, 0);
   STXterm(COMMA);
   STXelem(3, gno, 0);
   STXrule(tR2, COMMA);
   STXterm(PPPOINT);
   ;
   STXrule(tR1, PPPOINT);
   STXgroup(1, gXarg1, "gXarg1");
   STXelem(3, gDVM, 1);
   STXelem(2, gParm, 0);
   STXrule(tR0, XXparm);
   STXgroup(0, gParm, "gParm");
   STXelem(2, gXspec, 0);
   STXelem(3, gParm3, 0);
   STXrule(tR0, XXdecl);
   STXgroup(0, gParm3, "gParm3");
   STXelem(2, gParm32, 1);
   STXrule(tR0, 0);
   STXgroup(0, gParm32, "gParm32");
   STXelem(2, gXdcl, 0);
   STXrule(tR0, XXdcltr);
   STXgroup(0, gDfun, "gDfun");
   STXelem(2, gDfun2, 0);
   STXelem(3, gDbody, 0);
   STXrule(tR0, XXdcltr);
   STXgroup(0, gDfun2, "gDfun2");
   STXelem(2, gno, 0);
   STXterm(LBA);
   STXelem(3, gDfun32, 0);
   STXrule(tR2, LXfun);
   STXelem(0, LXI, 0);
   STXrule(tR0, LXI);
   STXgroup(0, gDfun32, "gDfun32");
   STXelem(2, gIDs, 0);
   STXterm(RBA);
   STXelem(3, gDargs, 1);
   STXrule(tR0, 0);
   STXgroup(0, gDbody, "gDbody");
   STXterm(LBS);
   STXelem(2, Compound_stmt_body, 1);
   STXterm(RBS);
   STXrule(tR1, XXbody);
   STXgroup(0, gIDs, "gIDs");
   STXelem(2, gno, 0);
   STXterm(COMMA);
   STXelem(3, gno, 0);
   STXrule(tR2, COMMA);
   STXelem(0, LXI, 0);
   STXrule(tR0, LXI);
   STXgroup(0, gDargs, "gDargs");
   STXelem(2, gno+1, 0);
   STXelem(3, gno, 1);
   STXrule(tR0, XXlist);
   STXgroup(0, gDargs2, "gDargs2");
   STXelem(3, gDVM, 1);
   STXelem(2, gDarg22, 0);
   STXrule(tR0, XXparm);
   STXgroup(0, gDarg22, "gDarg22");
   STXelem(2, gXspec, 0);
   STXelem(3, gDcltr, 0);
   STXrule(tR0, XXdecl);
   STXgroup(0, gXnextAdcl, "gXnextAdcl");
   STXterm(COMMA);
   STXelem(2, gDcltr, 0);
   STXrule(tR1, XXclist);
   STXterm(SEMIC);
   STXrule(tR0, SEMIC);
   STXgroup(0, Compound_stmt_body, "Compound_stmt_body");
   STXelem(2, g22, 0);
   STXelem(3, gno, 1);
   STXrule(tR0, XXlist);
   STXgroup(0, g22, "g22");
   STXelem(3, gDVM, 1);
   STXelem(2, gXdclop, 0);
   STXrule(tR0, 0);
   STXgroup(0, gXdclop, "gXdclop");
   STXelem(2, cpp, 0);
   STXrule(tR0, CPP);
   STXelem(2, gXspec, 0);
   STXelem(3, gDcltr, 0);
   STXrule(tR0, XXdecl);
   STXgroup(1, gOper, "gOper");
   STXelem(2, gXdo, 0);
   STXrule(tR0, XXoper);
   STXgroup(0, Else_branch, "Else_branch");
   STXterm(ELSE);
   STXelem(2, gOper, 0);
   STXrule(tR0, 0);
   STXgroup(0, Switch_body, "Switch_body");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXgroup(1, gXcase1, "gXcase1");
   STXterm(DEFAULT);
   STXterm(COLON);
   STXelem(3, gXcase3, 1);
   STXrule(tR1, DEFAULT);
   STXterm(CASE);
   STXelem(2, Assignement_prt_expr, 0);
   STXterm(COLON);
   STXelem(3, gXcase3, 1);
   STXrule(tR1, CASE);
   STXgroup(0, gXcase3, "gXcase3");
   STXelem(2, gno+1, 0);
   STXelem(3, gno, 1);
   STXrule(tR0, XXlist);
   STXgroup(0, gXcase32, "gXcase32");
   STXelem(3, gDVM, 1);
   STXelem(2, gOper, 0);
   STXrule(tR0, 0);
   STXgroup(0, gBOper, "gBOper");
   STXterm(LBS);
   STXrule(tRx, 0);
   STXelem(0, gOper, 0);
   STXrule(tR0, 0);
   STXelem(2, Compound_stmt_body, 0);
   STXrule(tR0, LBS);
   STXgroup(0, g290, "g290");
   STXelem(2, Bracketed_expr, 0);
   STXelem(3, gBOper, 0);
   STXrule(tR0, 0);
   STXgroup(0, g291, "g291");
   STXelem(2, gDvar, 0);
   STXterm(COMMA);
   STXelem(3, Conditional_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, g292, "g292");
   STXelem(2, gDvar, 0);
   STXterm(COMMA);
   STXelem(3, Expression, 0);
   STXrule(tR0, 0);
   STXgroup(0, g293, "g293");
   STXelem(2, Expression, 1);
   STXterm(SEMIC);
   STXelem(3, g294, 0);
   STXrule(tR0, 0);
   STXgroup(0, g294, "g294");
   STXelem(2, Expression, 1);
   STXterm(SEMIC);
   STXelem(3, Expression, 1);
   STXrule(tR0, 0);
   STXgroup(0, g299, "g299");
   STXelem(2, Conditional_prt_expr, 0);
   STXterm(COLON);
   STXelem(3, Conditional_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, gXdo, "gXdo");
   STXterm(IF);
   STXelem(2, g290, 0);
   STXelem(3, Else_branch, 1);
   STXrule(tR1, IF);
   STXterm(WHILE);
   STXelem(0, g290, 0);
   STXrule(tR1, WHILE);
   STXterm(DO);
   STXelem(3, gOper, 0);
   STXterm(WHILE);
   STXelem(2, Bracketed_expr, 0);
   STXterm(SEMIC);
   STXrule(tR1, DO);
   STXterm(FOR);
   STXterm(LBA);
   STXelem(2, g293, 0);
   STXterm(RBA);
   STXelem(3, g22, 0);
   STXrule(tR1, FOR);
   STXterm(SWITCH);
   STXelem(2, Bracketed_expr, 0);
   STXterm(LBS);
   STXelem(3, Switch_body, 0);
   STXterm(RBS);
   STXrule(tR1, SWITCH);
   STXterm(GOTO);
   STXelem(2, LXI, 0);
   STXterm(SEMIC);
   STXrule(tR1, GOTO);
   STXterm(SEMIC);
   ;
   STXrule(tR1, LXskip);
   STXterm(RETURN);
   STXelem(2, Assignement_prt_expr, 1);
   STXterm(SEMIC);
   STXrule(tR1, RETURN);
   STXterm(LX_FOR);
   STXterm(LBA);
   STXelem(2, g291, 0);
   STXterm(RBA);
   STXelem(3, g22, 0);
   STXrule(tR1, LX_FOR);
   STXterm(LX_DO);
   STXterm(LBA);
   STXelem(2, g292, 0);
   STXterm(RBA);
   STXelem(3, g22, 0);
   STXrule(tR1, LX_DO);
   STXterm(DVM_RB);
   STXelem(2, Bracketed_expr, 0);
   STXelem(3, gOper, 0);
   STXrule(tRW, DVM_RB);
   STXterm(DVM_FOR);
   STXelem(2, Bracketed_expr, 0);
   STXelem(3, gOper, 0);
   STXrule(tRW, DVM_FOR);
   STXterm(DVM_FOR_1);
   STXelem(2, Bracketed_expr, 0);
   STXelem(3, gOper, 0);
   STXrule(tRW, DVM_FOR_1);
   STXterm(DVM_DOPL);
   STXelem(2, Bracketed_expr, 0);
   STXelem(3, gOper, 0);
   STXrule(tRW, DVM_DOPL);
   STXterm(LBS);
   STXelem(2, Compound_stmt_body, 1);
   STXterm(RBS);
   STXrule(tR1, LBS);
   STXterm(CONTINUE);
   STXterm(SEMIC);
   STXrule(tR1, CONTINUE);
   STXterm(BREAK);
   STXterm(SEMIC);
   STXrule(tR1, BREAK);
   STXterm(LXI);
   STXterm(COLON);
   STXrule(tRx, 0);
   STXelem(2, gno+1, 0);
   STXterm(COLON);
   STXrule(tR0, COLON);
   STXelem(2, gno+1, 0);
   STXterm(SEMIC);
   STXrule(tR0, XXexpr);
   STXgroup(0, Expression, "Expression");
   STXelem(2, gno, 0);
   STXterm(COMMA);
   STXelem(3, gno+1, 0);
   STXrule(tR2, COMMA);
   STXgroup(1, Assignement_prt_expr, "Assignement_prt_expr");
   STXelem(2, gno+1, 0);
   STXterm(ASSIGN);
   STXelem(3, gno, 0);
   STXrule(tR2, ASSIGN);
   STXelem(2, gno+1, 0);
   STXterm(AADD);
   STXelem(3, gno, 0);
   STXrule(tR2, AADD);
   STXelem(2, gno+1, 0);
   STXterm(ASUB);
   STXelem(3, gno, 0);
   STXrule(tR2, ASUB);
   STXelem(2, gno+1, 0);
   STXterm(AMUL);
   STXelem(3, gno, 0);
   STXrule(tR2, AMUL);
   STXelem(2, gno+1, 0);
   STXterm(ADIV);
   STXelem(3, gno, 0);
   STXrule(tR2, ADIV);
   STXelem(2, gno+1, 0);
   STXterm(AMOD);
   STXelem(3, gno, 0);
   STXrule(tR2, AMOD);
   STXelem(2, gno+1, 0);
   STXterm(ARSH);
   STXelem(3, gno, 0);
   STXrule(tR2, ARSH);
   STXelem(2, gno+1, 0);
   STXterm(ALSH);
   STXelem(3, gno, 0);
   STXrule(tR2, ALSH);
   STXelem(2, gno+1, 0);
   STXterm(AAND);
   STXelem(3, gno, 0);
   STXrule(tR2, AAND);
   STXelem(2, gno+1, 0);
   STXterm(AXOR);
   STXelem(3, gno, 0);
   STXrule(tR2, AXOR);
   STXelem(2, gno+1, 0);
   STXterm(AOR);
   STXelem(3, gno, 0);
   STXrule(tR2, AOR);
   STXgroup(1, Conditional_prt_expr, "Conditional_prt_expr");
   STXelem(2, gno+1, 0);
   STXterm(QUEST);
   STXelem(3, g299, 0);
   STXrule(tR2, QUEST);
   STXgroup(1, g34, "g34");
   STXelem(2, gno, 0);
   STXterm(OR);
   STXelem(3, gno+1, 0);
   STXrule(tR2, OR);
   STXgroup(1, g35, "g35");
   STXelem(2, gno, 0);
   STXterm(AND);
   STXelem(3, gno+1, 0);
   STXrule(tR2, AND);
   STXgroup(1, g36, "g36");
   STXelem(2, gno, 0);
   STXterm(BOR);
   STXelem(3, gno+1, 0);
   STXrule(tR2, BOR);
   STXgroup(1, g37, "g37");
   STXelem(2, gno, 0);
   STXterm(BXOR);
   STXelem(3, gno+1, 0);
   STXrule(tR2, BXOR);
   STXgroup(1, g38, "g38");
   STXelem(2, gno, 0);
   STXterm(BAND);
   STXelem(3, gno+1, 0);
   STXrule(tR2, BAND);
   STXgroup(1, g39, "g39");
   STXelem(2, gno+1, 0);
   STXterm(EQU);
   STXelem(3, gno+1, 0);
   STXrule(tR2, EQU);
   STXelem(2, gno+1, 0);
   STXterm(NEQ);
   STXelem(3, gno+1, 0);
   STXrule(tR2, NEQ);
   STXgroup(1, g40, "g40");
   STXelem(2, gno+1, 0);
   STXterm(GT);
   STXelem(3, gno+1, 0);
   STXrule(tR2, GT);
   STXelem(2, gno+1, 0);
   STXterm(GE);
   STXelem(3, gno+1, 0);
   STXrule(tR2, GE);
   STXelem(2, gno+1, 0);
   STXterm(LT);
   STXelem(3, gno+1, 0);
   STXrule(tR2, LT);
   STXelem(2, gno+1, 0);
   STXterm(LE);
   STXelem(3, gno+1, 0);
   STXrule(tR2, LE);
   STXgroup(1, g41, "g41");
   STXelem(2, gno, 0);
   STXterm(RSH);
   STXelem(3, gno+1, 0);
   STXrule(tR2, RSH);
   STXelem(2, gno, 0);
   STXterm(LSH);
   STXelem(3, gno+1, 0);
   STXrule(tR2, LSH);
   STXgroup(1, Addition_prt_expr, "Addition_prt_expr");
   STXelem(2, gno, 1);
   STXterm(ADD);
   STXelem(3, gno+1, 0);
   STXrule(tR2, ADD);
   STXelem(2, gno, 1);
   STXterm(SUB);
   STXelem(3, gno+1, 0);
   STXrule(tR2, SUB);
   STXgroup(1, g43, "g43");
   STXelem(2, gno, 0);
   STXterm(MUL);
   STXelem(3, gno+1, 0);
   STXrule(tR2, MUL);
   STXelem(2, gno, 0);
   STXterm(DIV);
   STXelem(3, gno+1, 0);
   STXrule(tR2, DIV);
   STXelem(2, gno, 0);
   STXterm(MOD);
   STXelem(3, gno+1, 0);
   STXrule(tR2, MOD);
   STXgroup(1, Unary_expr, "Unary_expr");
   STXelem(2, gno, 0);
   STXterm(INC);
   ;
   STXrule(tR2, INCA);
   STXelem(2, gno, 0);
   STXterm(DEC);
   ;
   STXrule(tR2, DECA);
   STXterm(NOT);
   STXelem(3, gno, 0);
   STXrule(tR1, NOT);
   STXterm(BNOT);
   STXelem(3, gno, 0);
   STXrule(tR1, BNOT);
   STXterm(SUB);
   STXelem(3, gno, 0);
   STXrule(tR1, SUB);
   STXterm(ADD);
   STXelem(3, gno, 0);
   STXrule(tR1, ADD);
   STXterm(INC);
   STXelem(3, gno, 0);
   STXrule(tR1, INC);
   STXterm(DEC);
   STXelem(3, gno, 0);
   STXrule(tR1, DEC);
   STXterm(SIZEOF);
   STXelem(2, gXsizeof, 0);
   STXrule(tR1, SIZEOF);
   STXterm(BAND);
   STXelem(3, gno, 0);
   STXrule(tR1, ADDR);
   STXterm(MUL);
   STXelem(3, gno, 0);
   STXrule(tR1, CONT);
   STXgroup(1, Primary_expr, "Primary_expr");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Expression, 0);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, gno, 0);
   STXterm(POINT);
   STXelem(3, LXI, 0);
   STXrule(tR2, POINT);
   STXelem(2, gno, 0);
   STXterm(ARROW);
   STXelem(3, LXI, 0);
   STXrule(tR2, ARROW);
   STXelem(2, gno, 0);
   STXterm(LBA);
   STXelem(3, Expression, 1);
   STXterm(RBA);
   STXrule(tR2, LXfun);
   STXterm(LBA);
   STXterm(LXI);
   STXterm(RBA);
   STXterm(LBA);
   STXrule(tRx, 0);
   STXterm(LBA);
   STXterm(LXI);
   STXterm(RBA);
   STXterm(LXI);
   STXrule(tRx, 0);
   STXterm(LBA);
   STXterm(LXI);
   STXterm(MUL);
   STXterm(RBA);
   STXrule(tRx, 0);
   STXterm(LBA);
   STXelem(2, gXtname, 0);
   STXterm(RBA);
   STXrule(tRx, 0);
   STXterm(LBA);
   STXelem(2, gXtname, 0);
   STXterm(RBA);
   STXelem(3, Unary_expr, 0);
   STXrule(tR0, LXcast);
   STXgroup(1, Bracketed_expr, "Bracketed_expr");
   STXterm(LBA);
   STXelem(2, Expression, 1);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXgroup(1, g46, "g46");
   STXelem(0, LXR, 0);
   STXrule(tR0, LXR);
   STXelem(2, gLXS, 0);
   STXrule(tR0, LXSs);
   STXgroup(1, gNID, "gNID");
   STXelem(0, LXN, 0);
   STXrule(tR0, LXN);
   STXgroup(1, Identifier, "Identifier");
   STXelem(0, LXI, 0);
   STXrule(tR0, LXI);
   STXelem(2, LXI, 0);
   STXrule(tRW, DTscal);
   STXgroup(0, gLXS, "gLXS");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXelem(0, LXS, 0);
   STXrule(tR0, LXS);
   STXgroup(0, gXsizeof, "gXsizeof");
   STXterm(LBA);
   STXelem(2, g50, 0);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXelem(0, Conditional_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, g50, "g50");
   STXelem(0, gXtname, 0);
   STXrule(tRx, 0);
   STXelem(0, gXtname, 0);
   STXrule(tR0, XXtype);
   STXelem(0, Conditional_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, gXtname, "gXtname");
   STXelem(2, gXspec, 0);
   STXelem(3, gXadcl, 1);
   STXrule(tR0, XXtype);
   STXgroup(0, gXadcl, "gXadcl");
   STXterm(LBA);
   STXelem(2, gno, 0);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXterm(MUL);
   STXelem(3, CONST, 1);
   STXelem(2, gno, 0);
   STXrule(tR1, LXaster);
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Assignement_prt_expr, 1);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, gno, 0);
   STXterm(LBA);
   STXelem(3, gXargs, 1);
   STXterm(RBA);
   STXrule(tR2, LXfun);
   ;
   STXrule(tR0, 0);
   STXgroup(0, gDVM, "gDVM");
   STXterm(LX_DVM);
   STXterm(LBA);
   STXelem(3, MUL, 1);
   STXelem(2, gno+1, 0);
   STXterm(RBA);
   STXrule(tR1, LX_DVM);
   STXgroup(0, DVM_directive, "DVM_directive");
   STXterm(DISTRIBUTE);
   STXelem(2, Distr_directive, 1);
   STXelem(3, Sub_directive, 1);
   STXrule(tR1, DISTRIBUTE);
   STXterm(ALIGN);
   STXelem(2, Align_directive, 1);
   STXelem(3, Sub_directive, 1);
   STXrule(tR1, ALIGN);
   STXterm(LX_SG);
   ;
   STXrule(tR1, LX_SG);
   STXterm(LX_RG);
   ;
   STXrule(tR1, LX_RG);
   STXterm(LX_RMG);
   ;
   STXrule(tR1, LX_RMG);
   STXterm(LX_IG);
   ;
   STXrule(tR1, LX_IG);
   STXterm(TASK);
   ;
   STXrule(tR1, TASK);
   STXterm(PROCESSORS);
   ;
   STXrule(tR1, PROCESSORS);
   STXterm(LX_CP);
   ;
   STXrule(tR1, LX_CP);
   STXterm(INTERVAL);
   STXelem(2, Assignement_prt_expr, 1);
   STXrule(tR1, INTERVAL);
   STXterm(DEBUG);
   STXelem(2, LXN, 0);
   STXelem(3, Debug_modes, 1);
   STXrule(tR1, DEBUG);
   STXterm(BARRIER);
   ;
   STXrule(tR1, BARRIER);
   STXterm(PARALLEL);
   STXelem(2, Parallel_directive, 0);
   STXelem(3, Par_clauses, 1);
   STXrule(tR1, PARALLEL);
   STXelem(2, Parallel_directive, 0);
   STXelem(3, Par_clauses, 1);
   STXrule(tRW, TASKLOOP);
   STXterm(REMOTE);
   STXelem(2, Named_access, 1);
   STXelem(3, References, 0);
   STXrule(tR1, REMOTE);
   STXterm(INDIRECT);
   STXelem(2, Named_access, 1);
   STXelem(3, References, 0);
   STXrule(tR1, INDIRECT);
   STXterm(LX_TASKREG);
   STXelem(2, Task_name, 0);
   STXelem(3, Par_clauses, 1);
   STXrule(tR1, LX_TASKREG);
   STXterm(ON);
   STXelem(2, Task, 0);
   STXrule(tR1, ON);
   STXterm(REDISTRIBUTE);
   STXelem(2, Redistr_directive, 0);
   STXelem(3, NEW, 1);
   STXrule(tR1, REDISTRIBUTE);
   STXterm(REALIGN);
   STXelem(2, Realign_directive, 0);
   STXelem(3, NEW, 1);
   STXrule(tR1, REALIGN);
   STXterm(LX_CRTEMP);
   STXelem(2, Template, 0);
   STXrule(tR1, LX_CRTEMP);
   STXterm(LX_CRSG);
   STXelem(2, Shadow_group, 0);
   STXterm(COLON);
   STXelem(3, Renewees, 0);
   STXrule(tR1, LX_CRSG);
   STXterm(SHSTART);
   STXelem(2, Shadow_group, 0);
   STXrule(tR1, SHSTART);
   STXterm(SHWAIT);
   STXelem(2, Shadow_group, 0);
   STXrule(tR1, SHWAIT);
   STXterm(RSTART);
   STXelem(2, Reduction_group, 0);
   STXrule(tR1, RSTART);
   STXterm(RWAIT);
   STXelem(2, Reduction_group, 0);
   STXrule(tR1, RWAIT);
   STXterm(PREFETCH);
   STXelem(2, Access_group, 0);
   STXrule(tR1, PREFETCH);
   STXterm(RESET);
   STXelem(2, Access_group, 0);
   STXrule(tR1, RESET);
   STXterm(MAP);
   STXelem(2, Task, 0);
   STXelem(3, Distr_target, 0);
   STXrule(tR1, MAP);
   STXterm(COPY);
   ;
   STXrule(tR1, COPY);
   STXterm(CPSTART);
   STXelem(2, Unary_expr, 0);
   STXrule(tR1, CPSTART);
   STXterm(CPWAIT);
   STXelem(2, Unary_expr, 0);
   STXrule(tR1, CPWAIT);
   STXgroup(0, Shadow_group, "Shadow_group");
   STXelem(2, Identifier, 0);
   STXrule(tR0, LXIsg);
   STXgroup(0, Reduction_group, "Reduction_group");
   STXelem(2, Identifier, 0);
   STXrule(tR0, LXIrg);
   STXgroup(0, Red_group_name, "Red_group_name");
   STXterm(LXI);
   STXterm(COLON);
   STXrule(tRx, 0);
   STXelem(2, Identifier, 0);
   STXterm(COLON);
   STXrule(tR0, LXIrg);
   STXgroup(0, Task_name, "Task_name");
   STXelem(2, Identifier, 0);
   STXrule(tR0, LXItask);
   STXgroup(0, Task, "Task");
   STXelem(2, Task_name, 0);
   STXterm(LBK);
   STXelem(3, Expression, 0);
   STXterm(RBK);
   STXrule(tR0, LBK);
   STXgroup(0, Distr_directive, "Distr_directive");
   STXelem(2, Distr_stuff, 1);
   STXrule(tR0, 0);
   STXgroup(0, Redistr_directive, "Redistr_directive");
   STXelem(2, Distr_stuff, 0);
   STXelem(3, NEW, 1);
   STXrule(tR0, 0);
   STXgroup(0, Distr_stuff, "Distr_stuff");
   STXelem(2, Distr_format, 0);
   STXelem(3, Distr_target, 1);
   STXrule(tR0, 0);
   STXgroup(0, Distr_format, "Distr_format");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Distr_mode, 1);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, gDiref, 1);
   STXrule(tR0, DVMbase);
   STXgroup(0, Distr_target, "Distr_target");
   STXterm(ONTO);
   STXelem(2, gno+1, 0);
   STXrule(tR1, ONTO);
   STXgroup(0, Distr_target1, "Distr_target1");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Section_subscript, 1);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, Identifier, 0);
   STXrule(tR0, LXIproc);
   STXgroup(0, Distr_mode, "Distr_mode");
   STXterm(MUL);
   ;
   STXrule(tR1, MUL);
   STXterm(BLOCK);
   ;
   STXrule(tR1, BLOCK);
   STXterm(GENBLOCK);
   STXterm(LBA);
   STXelem(2, Identifier, 0);
   STXterm(RBA);
   STXrule(tR1, GENBLOCK);
   STXterm(WGTBLOCK);
   STXterm(LBA);
   STXelem(2, Identifier, 0);
   STXterm(COMMA);
   STXelem(3, Addition_prt_expr, 0);
   STXterm(RBA);
   STXrule(tR1, WGTBLOCK);
   STXgroup(0, Section_subscript, "Section_subscript");
   STXelem(2, gno+1, 0);
   STXterm(COLON);
   STXelem(3, gno+1, 0);
   STXrule(tR2, COLON);
   STXgroup(1, Section_subscript1, "Section_subscript1");
   STXelem(0, Addition_prt_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, Align_directive, "Align_directive");
   STXelem(2, Align_stuff, 1);
   STXrule(tR0, 0);
   STXgroup(0, Realign_directive, "Realign_directive");
   STXelem(2, Align_stuff, 0);
   STXelem(3, NEW, 1);
   STXrule(tR0, 0);
   STXgroup(0, Align_stuff, "Align_stuff");
   STXelem(2, Align_source, 1);
   STXelem(3, Align_with, 1);
   STXrule(tR0, DVMalign);
   STXgroup(0, Align_source, "Align_source");
   STXelem(2, gno, 1);
   STXterm(LBK);
   STXelem(3, Identifier, 1);
   STXterm(RBK);
   STXrule(tR2, DVMind);
   STXelem(2, gDiref, 1);
   STXrule(tR0, DVMbase);
   STXgroup(0, Align_with, "Align_with");
   STXterm(WITH);
   STXelem(0, gno+1, 0);
   STXrule(tR0, 0);
   STXgroup(0, Align_target, "Align_target");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Addition_prt_expr, 1);
   STXterm(RBK);
   STXrule(tR2, DVMbind);
   STXelem(2, gDiref, 0);
   STXrule(tR0, DVMbase);
   STXgroup(0, Template, "Template");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Addition_prt_expr, 0);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, gDiref, 1);
   STXrule(tR0, DVMbase);
   STXgroup(0, Sub_directive, "Sub_directive");
   STXterm(SEMIC);
   STXterm(TEMPLATE);
   STXelem(2, Template, 0);
   STXrule(tR2, TEMPLATE);
   STXterm(SEMIC);
   STXterm(SHADOW);
   STXelem(2, Shadow_edge, 0);
   STXrule(tR2, SHADOW);
   STXterm(SEMIC);
   STXrule(tR0, 0);
   STXgroup(0, Parallel_directive, "Parallel_directive");
   STXelem(2, Parallel_source, 0);
   STXterm(ON);
   STXelem(3, Align_target, 0);
   STXrule(tR0, DVMalign);
   STXgroup(0, Parallel_source, "Parallel_source");
   STXelem(2, gno, 1);
   STXterm(LBK);
   STXelem(3, Identifier, 1);
   STXterm(RBK);
   STXrule(tR2, DVMind);
   ;
   STXrule(tR0, DVMbase);
   STXgroup(0, gDvar, "gDvar");
   STXelem(2, Identifier, 0);
   STXrule(tR0, DVMvar);
   STXelem(0, LXI, 0);
   STXrule(tRW, LXI);
   STXgroup(0, Par_clauses, "Par_clauses");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXgroup(1, ParClause, "ParClause");
   STXterm(SEMIC);
   STXelem(0, gno+1, 0);
   STXrule(tR0, 0);
   STXgroup(0, Par_clause, "Par_clause");
   STXterm(REDUCTION);
   STXelem(2, Red_group_name, 1);
   STXelem(3, Reduction_op, 0);
   STXrule(tR1, REDUCTION);
   STXgroup(1, Sh_renew_clause, "Sh_renew_clause");
   STXterm(SHRENEW);
   STXelem(3, Renewees, 0);
   STXrule(tR1, SHRENEW);
   STXgroup(1, Sh_start_clause, "Sh_start_clause");
   STXterm(SHSTART);
   STXelem(2, Identifier, 0);
   STXrule(tR1, SHSTART);
   STXgroup(1, Sh_wait_clause, "Sh_wait_clause");
   STXterm(SHWAIT);
   STXelem(2, Identifier, 0);
   STXrule(tR1, SHWAIT);
   STXgroup(1, Across_clause, "Across_clause");
   STXterm(ACROSS);
   STXelem(2, ACRIN, 1);
   STXelem(3, Renewees, 0);
   STXrule(tR1, ACROSS);
   STXgroup(1, Pipe_clause, "Pipe_clause");
   STXterm(PIPE);
   STXelem(2, Identifier, 0);
   STXrule(tR1, PIPE);
   STXgroup(1, Remote_clause, "Remote_clause");
   STXterm(REMOTE);
   STXelem(2, Named_access, 1);
   STXelem(3, References, 0);
   STXrule(tR1, REMOTE);
   STXgroup(1, Indirect_clause, "Indirect_clause");
   STXterm(INDIRECT);
   STXelem(2, Named_access, 1);
   STXelem(3, References, 0);
   STXrule(tR1, INDIRECT);
   STXgroup(0, Shadow_edge, "Shadow_edge");
   STXelem(2, gno, 1);
   STXterm(LBK);
   STXelem(3, Shadow_width, 1);
   STXterm(RBK);
   STXrule(tR2, DVMshw);
   STXterm(LBK);
   STXelem(3, Shadow_width, 1);
   STXterm(RBK);
   STXrule(tR1, DVMshw);
   STXgroup(0, Renewees, "Renewees");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXelem(2, Renewee, 0);
   STXelem(3, CORNER, 1);
   STXrule(tR0, DVMshad);
   STXgroup(0, Renewee, "Renewee");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Shadow_width, 1);
   STXterm(RBK);
   STXrule(tR2, DVMshw);
   STXelem(2, gDiref, 0);
   STXelem(3, SHAlim, 1);
   STXrule(tR0, DVMbase);
   STXgroup(0, SHAlim, "SHAlim");
   STXterm(LBA);
   STXelem(2, Expression, 0);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXgroup(0, Shadow_width, "Shadow_width");
   STXelem(2, LXN, 0);
   STXterm(COLON);
   STXelem(3, LXN, 0);
   STXrule(tR2, COLON);
   STXelem(0, LXN, 0);
   STXrule(tR0, LXN);
   STXgroup(0, Reduction_var, "Reduction_var");
   STXelem(0, Primary_expr, 0);
   STXrule(tR0, 0);
   STXgroup(0, Reduction_op, "Reduction_op");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXterm(SUM);
   STXterm(LBA);
   STXelem(2, Primary_expr, 0);
   STXterm(RBA);
   STXrule(tR1, SUM);
   STXterm(PROD);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(RBA);
   STXrule(tR1, PROD);
   STXterm(MAX);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(RBA);
   STXrule(tR1, MAX);
   STXterm(MIN);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(RBA);
   STXrule(tR1, MIN);
   STXterm(LX_AND);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(RBA);
   STXrule(tR1, LX_AND);
   STXterm(LX_OR);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(RBA);
   STXrule(tR1, LX_OR);
   STXterm(MAXLOC);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(COMMA);
   STXelem(3, Primary_expr, 0);
   STXterm(RBA);
   STXrule(tR1, MAXLOC);
   STXterm(MINLOC);
   STXterm(LBA);
   STXelem(2, Reduction_var, 0);
   STXterm(COMMA);
   STXelem(3, Primary_expr, 0);
   STXterm(RBA);
   STXrule(tR1, MINLOC);
   STXgroup(0, Access_group, "Access_group");
   STXelem(2, Identifier, 0);
   STXrule(tR0, LXIag);
   STXgroup(0, Named_access, "Named_access");
   STXterm(LXI);
   STXterm(COLON);
   STXrule(tRx, 0);
   STXelem(0, Access_group, 0);
   STXterm(COLON);
   STXrule(tR0, 0);
   STXgroup(0, References, "References");
   STXelem(2, gno, 0);
   STXterm(LXX);
   STXelem(3, gno, 1);
   STXrule(tR2, LXX);
   STXelem(2, Reference, 0);
   STXrule(tR0, DVMremote);
   STXgroup(0, Reference, "Reference");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Addition_prt_expr, 1);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXelem(2, Identifier, 0);
   STXrule(tR0, DVMbase);
   STXgroup(0, gDref, "gDref");
   STXelem(2, gno, 0);
   STXterm(LBK);
   STXelem(3, Addition_prt_expr, 0);
   STXterm(RBK);
   STXrule(tR2, LBK);
   STXgroup(1, gDiref, "gDiref");
   STXterm(LBA);
   STXelem(2, gDref, 0);
   STXterm(RBA);
   STXrule(tR1, LBA);
   STXelem(0, Identifier, 0);
   STXrule(tR0, 0);
   STXgroup(0, Debug_modes, "Debug_modes");
   STXterm(SUB);
   STXterm(optD0);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optD0);
   STXelem(3, gno, 1);
   STXrule(tR0, optD0);
   STXterm(SUB);
   STXterm(optD1);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optD1);
   STXelem(3, gno, 1);
   STXrule(tR0, optD1);
   STXterm(SUB);
   STXterm(optD2);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optD2);
   STXelem(3, gno, 1);
   STXrule(tR0, optD2);
   STXterm(SUB);
   STXterm(optD3);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optD3);
   STXelem(3, gno, 1);
   STXrule(tR0, optD3);
   STXterm(SUB);
   STXterm(optD4);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optD4);
   STXelem(3, gno, 1);
   STXrule(tR0, optD4);
   STXterm(SUB);
   STXterm(optE0);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optE0);
   STXelem(3, gno, 1);
   STXrule(tR0, optE0);
   STXterm(SUB);
   STXterm(optE1);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optE1);
   STXelem(3, gno, 1);
   STXrule(tR0, optE1);
   STXterm(SUB);
   STXterm(optE2);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optE2);
   STXelem(3, gno, 1);
   STXrule(tR0, optE2);
   STXterm(SUB);
   STXterm(optE3);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optE3);
   STXelem(3, gno, 1);
   STXrule(tR0, optE3);
   STXterm(SUB);
   STXterm(optE4);
   STXrule(tRx, 0);
   STXterm(SUB);
   STXterm(optE4);
   STXelem(3, gno, 1);
   STXrule(tR0, optE4);
   STXgroup(0, LastSTXGroup, "LastSTXGroup");
   }

/*----------------------------------------------------------*/
/*      Parser                                              */
/*----------------------------------------------------------*/


/* Compare the current token with the terminal element 'c'  */

int cmpLX(int c)
   {

/*    Compare token's code or value.                        */

   if (c!=(c<NL?C(curlex): A(curlex))) 	return 0;	/* unappropriate */

/*    If OK, move to the next token                         */

   while (C(++curlex)==TOKEN) 	
      {
      ;
      }
   return 1;	/* OK */
   }

/* Process 'look-forward' (Rx) rules.                       */

int SkipNext(int *r)
   {
   int rc=-1;	/* initialize as 'failed' */
   int lx;
   INRx++;	/* disable normal errors and tree processing */
   for (Count=0, *r=*r; *r; Count++, *r=B(*r)) 	
      {
      if (D(*r)!=-tRx) 	break ;	/* end of Rx fules */
      if (rc<0) 	
         {
         lx=curlex;	/* save source position */
         rc=Parse(0, *r, 0);	/* try to parse with errors disabled */
         curlex=lx;	/* restore source position */
         }
      }
   INRx--;	/* enable error and tree processing */
   return rc<0;	/* 1 == all rules failed => skip the next */
   }

/* Fill a filed 'n' or substitute a node (if n==0)          */

void Tset(int n, int x)
   {
   int N=A(Stack);	/* the current node */
   if (INRx) 	return ;	/* disabled */
   if (n==2) 	wA(N, x);	/* A-field */
   else
   if (n==3) 	wB(N, x);	/* B-field */
   else
   if (n==0) 	
      {
      if (x && C(x)==0) 	wC(x, C(N));	/* copy the code */
      wA(Stack, x);	/* relink the top of the stack */
      if ((C(N)|A(N)|B(N))==0 && D(N)==-1) 	NDdel(N);	/* free the node N if it is empty */
      }
   }

/* Prepare an error message                                 */

void ErrMsg(int curlex)
   {
   int prev=curlex-1, curr=curlex, tprev, tcurr;
   while (C(prev)==TOKEN) 	prev--;
   w=C(prev);
   tprev=(w>LXX && w<NL)?w: 0;
   prev=A(prev);
   w=C(curr);
   tcurr=(w>LXX && w<NL)?w: 0;
   curr=A(curr);
   sprintf(msg, "9:Unexpected  %s \'%s\' after %s \'%s\' ", tcurr?LXW(tcurr): "", LXW(curr), tprev?LXW(tprev): "", LXW(prev));
   }

/* Issue parser error                                       */

void ParsErr(int gr, int fr)
   {

/*    unappropriate token                                   */

   if (fdeb) 	
      {
      printf("\n------ ParsErr(%d, %d)------\n", gr, fr);
      }
   if (C(gr)==TOKEN) 	
      {
      ErrMsg(curlex);
      }

/*    can not finish selected prefix rule                   */

   else
   if (fr) 	
      {
      sprintf(msg, "9: <%s> syntax error.", LXW(D(gr)));
      }

/*    can not start any prefix rule                         */

   else
      {
      ErrMsg(curlex);
      }
   ERR(msg);
   }

/* The parser                                               */
/*    N  -- left operand for infix rules (already parsed)   */
/*    rule -- syntax rule                                   */
/*    abcd -- a field of the top rule                       */
int Parse(int N, int rule, int abcd)
   {
   int ce=A(rule);	/* elements of the rule */
   int fld;
   int fr;
   int moved=0;	/* "at the beginning of the rule" */
   int w;
   SSpush(NDD(C(rule), 0, 0, -1));	/* create new node */
   wD(Stack, D(curlex));	/* line number */

/*    For infix rule two first elements already matched     */

   if (D(rule)==-tR2) 	
      {
      fld=(-D(ce))%10000;	/* field of the left operand */
      if (fld!=9999) 	Tset(fld, N);	/* link subtree */
      else NDdel(N);
      ce=B(ce);
      ce=B(ce);	/* skip two elements */
      moved++;
      }

/*    For prefix rule first element already matched         */

   else
   if (D(rule)==-tR1) 	
      {
      ce=B(ce);	/* skip one elememt */
      moved++;
      }

/*    descending invocation of the semantic function        */

   if (INRx==0 && C(A(Stack))) 	
      {
      if (ISWF(1)) 	
         {
         N=0;
         goto failed;	/* unallowed by some semantic reason */
         }
      }

/*    Process elements of the current rule                  */

   for (Count=0, ce=ce; ce; Count++, ce=B(ce)) 	
      {
      int gr=A(ce);	/* code of terminal or group of rules */
      fld=(-D(ce))%10000;	/* field */

/*    If it is a terminal element...                        */

      if (C(gr)==TOKEN) 	
         {
         w=curlex;
         if (gr==LXX) 	continue ;	/* ie. empty delimiter */

/*    current token does not match                          */

         else
         if (!cmpLX(gr)) 	
            {
            if (gr<=NL && fld==0) 	
               {
               N=0;
               goto failed;
               }
            else goto error;
            }

/*    OK. Link to the tree if necessary.                    */

         else
         if (INRx==0) 	
            {
            moved++;
            if (fld!=9999) 	
               {
               int N=A(Stack);
               if (gr>NL) 	w=A(w);
               Tset(fld, w);
               if (fld==0) 	NDdel(N);
               }
            }
         continue ;	/* to the next element */
         }
      else
         {

/*    Find appropriate prefix rule                          */

         for (Count=0, fr=A(gr); fr; Count++, fr=B(fr)) 	
            {
            if (D(fr)==-tRx && SkipNext(&fr)) 	continue ;	/* skip depending on look-forward rules */

/*    Default or prefix rule with natching first element    */

            if (D(fr)==-tR0 || D(fr)==-tR1 && cmpLX(A(A(fr)))) 	
               {
               w=Parse(0, fr, fld);	/* Parse. 'w' is build subtree */
               Tset(fld, w);	/* link subtree */
               if (w>0) 	
                  {
                  moved++;
                  goto infix;	/* OK: Try infix rules */
                  }
               else
               if (D(fr)==-tR0) 	continue ;	/* Try next default rule */
               else goto error;	/* rule found abd failed */
               }
            }
         goto error;	/* rule not found */
infix:

/*    Find appropriate infix rule                           */

         for (Count=0, fr=A(gr); fr; Count++, fr=B(fr)) 	
            {
            if (D(fr)==-tR2 && ((w=A(B(A(fr))))==LXX || cmpLX(w))) 	
               {
               w=Parse(NDget(A(Stack), fld), fr, fld);	/* Parse */
               if (C(w)==LXX && B(w)==0) 	
                  {
                  NDdel(w);
                  break ;
                  }
               Tset(fld, w);	/* link to the tree */
               goto infix;	/* continue with infix rules */
               }
            }
         continue ;	/* the next element */
         }
error:
      if (D(ce)+fld) 	continue ;	/* optional element */
      if (INRx) 	goto failed;	/* messages disabled */
      if (moved) 	
         {
         ParsErr(gr, fr);	/* Parser error will stop! */
         }
      else
         {
         N=0;
         goto failed;	/* can try another rule */
         }
      break ;
      }

/*    The rule scanned                                      */

   N=A(Stack);

/*    ascenging invocation of the semantic function         */

   if (INRx==0 && C(N) && (abcd || C(N)==LXI)) 	ISWF(0);
   if (INRx && N) 	NDdel(N);
   N=A(Stack);	/* The result of parsing */
   SSpop();	/* pop the stack */
   return N;

/*    Abnormal exit. Do not invoke semantic.                */

failed:
   if (INRx) 	
      {
      N=-1;
      }
   NDdel(A(Stack));	/* free created dummy node */
   SSpop();	/* pop the stack */
   return N;
   }

/*----------------------------------------------------------*/
/* Unparser                                                 */
/*----------------------------------------------------------*/


/* Output a string                                          */

void G(char *s)
   {
   fprintf(fout, "%s", s);
   }

/* Output a new line character                              */

void Gnl()
   {
   fprintf(fout, "\n");
   }

/* The Unparser                                             */
/*    N   -- current node of the tree                       */
/*    sg  -- code of current element of syntax rule         */
/*    out -- output to file                                 */

void UnPars(int N, int sg, int out)
   {
   int ce, cr;

/* This is a terminal element of the rule                   */

   if (C(sg)==TOKEN) 	
      {
      w=sg>NL?sg: A(N);
      if (out) 	
         {

/* Output (with NL before and after)                        */

         if (w==LBS) 	Gnl();
         if (w==DVM_FOR || w==DVM_FOR_1 || w==DVM_RB || w==DVM_DOPL) 	Gnl();
         if (sg!=LXX) 	
            {
            G(LXW(w));
            }
         else
            {
            Gnl();
            }
         G(" ");
         if (w==SEMIC || w==RBS || w==LBS || w==CPP) 	Gnl();
         }
      }
   else
      {

/* Find rule by the code of the current node                */

      for (Count=0, cr=A(sg); cr; Count++, cr=B(cr)) 	
      if ((w=C(cr))!=0 && w==C(N) || w==0 && D(cr)==-tR0) 	
         {

/* Process all elements of the rule                         */

         for (Count=0, ce=A(cr); ce; Count++, ce=B(ce)) 	
            {
            int fld=(-D(ce))%10000;	/* field number */
            w=NDget(N, fld);	/* get the field */
            if (w==0 && (D(ce)+fld!=0)) 	;	/* Subtree is empty -- for optional element */
            else
            if (C(A(ce))==TOKEN || A(ce) && w) 	UnPars(w, A(ce), out);	/* terminal or non-empty subtree */
            else
            if (D(ce)+fld==0) 	
               {
               ;	/* empty subtree for non-optional element */
               fprintf(fout, "  /*????*/  ");
               if (fdeb) 	
                  {
                  fprintf(fdeb, "\n\nTree corrupted (ce,N):\n\n");
                  dump1(ce);
                  dump1(N);
                  dumpAll(0);
                  }
               ERR("9:[Parsing tree is corrupted]");	/* fatal error */
               }
            }
         if (C(cr)) 	break ;
         }
      }
   }

/*----------------------------------------------------------*/
/* Visibility scopes and declarations                       */
/*----------------------------------------------------------*/

int SCno=1;	/* The number of the current scope */
int SCext=0;	/* Stack of enclosing scopes -> (SCno,SCext,@) */
int SCloc=0;	/* Scope's definitions list -> (SCno,tok,@) */

/* Entering a new scope                                     */

void openScope(void )
   {
   SCext=NDD(SCno, SCloc, SCext, 0);	/* push scopes stack */
   SCno++;	/* increment scope number */
   SCloc=0;	/* initialize definitions list */
   }

/* Exiting a scope                                          */

void closeScope(void )
   {
   for (Count=0, i=SCloc; i; Count++, i=B(i)) 	wB(A(i), B(B(A(i))));	/* erase local declaration */
   SCloc=A(SCext);	/* restore current definitions list */
   SCno=C(SCext);	/* restore scope number */
   w=SCext;
   SCext=B(SCext);
   NDdel(w);
   }

/* Add a declaration to the current scope                   */

void addDecl(int N, int spec, int dvm)
   {
   int lxi, token;
   cID=spec;

/* Descent through the declarator to the identifier         */

   for (Count=0, w=N; w && C(cID)!=LXI; Count++, w=A(w)) 	
      {
      wD(w, cID);	/* set upward link */
      cID=w;
      }
   wB(cID, NDD(0, spec, dvm, 0));
   token=A(cID);

/* Previous declaration of the same identifier              */

   for (Count=0, w=B(token); w; Count++, w=B(w)) 	
      {
      if (C(w)!=SCno) 	break ;	/* in enclosing scope */
      else
      if (C(A(w))==C(cID)) 	goto err;	/* in the same scope */
      }

/* Add to the list of declarations of this identifier       */

   wB(token, NDD(SCno, cID, B(token), 0));

/* Add to the list of declarations of the current scope     */

   SCloc=NDD(SCno, token, SCloc, 0);
   return ;

/* Redeclaration warning.                                   */

err:
   ERR("0:Redeclaration (ignored)");
   return ;
   }

/* From using to definition                                 */

int Decl(int lx)
   {
   ASSERT(C(lx)==LXI, Decl);
   w=D(lx);
   if (w<=0) 	return 0;
   ASSERT(C(w)!=LXI, obsolet_Decl);	/*1001*/
   return lx;
   }

/* Get the type                                             */

int Type(int lx)
   {
   return B(A(B(Decl(lx))));
   }

/* Get DVM-directive                                        */

int DVMdir(int lx)
   {
   return A(B(B(Decl(lx))));
   }
int definedDVM(int N)
   {
   whatis(N);
   if (cDecl==0) 	E(2040);
   else
   if (cDir==0) 	E(2039);
   return cDir;
   }
int defined(int N)
   {
   whatis(N);
   if (cDecl==0) 	E(2040);
   return cDir;
   }

/* The rank of index expression                             */

int Rank(int N)
   {
   toA0(DVMbase, N);
   return Count;
   }

/* The rank of the staff of DVM-directive                   */

int DRank(int N)
   {
   if (C(N)==DISTRIBUTE) 	w=LBK;
   else
   if (C(N)==ALIGN || C(N)==PARALLEL) 	w=DVMind;
   else ASSERT(0, DRank);	/*.... или E("не DVM-array")*/
   skipA(w, toA0(w, N));
   return Count;
   }

/* Analize a declarator: (* id [.l.]) [.r.]                 */

int arrDcltr(int N)
   {
   N=A(N);
   N=skipA(LBK, N);	/* X [...]  -> X */
   cLBKr=Count;	/* rank */
   if (C(N)==LBA) 	N=A(N);	/* (X) -> X */
   if (C(N)==LXaster) 	
      {
      cASTER=1;	/* pointer or array of pointers */
      N=A(N);	/* *X -> X */
      }
   else cASTER=0;
   N=skipA(LBK, N);	/* X [...]  -> X */
   cLBKl=Count;	/* rank of array of pointers */
   return N;
   }

/* Unpack declaration                                       */

int whatis(int N)
   {

/*    Initialize resulting globals                          */

   cDecl=0;
   cDcltr=0;
   cLBKr=0;
   cASTER=0;
   cLBKl=0;
   cType=0;
   cRTStype=0;
   cDVM=0;
   cDir=0;
   cDIR=0;
   dRank=-1;

/*    uRank = the number of subscripts                      */

   w=C(N);
   if (w==LBK || w==DVMbind || w==DVMind) 	
      {
      cID=skipA(w, N);
      uRank=Count;
      }
   else
   if (w==LXfun) 	
      {
      cID=A(N);
      uRank=Len(COMMA, B(N));
      }
   else
      {
      cID=N;
      uRank=0;
      }
   if (N==0) 	return 0;

/*    Find an identifier                                    */

   switch (C(cID))
      {
   default : return 0;
   case LXI: break ;
   case DVMbase:
   case LXIproc:
   case LXItask: cID=A(cID);
      break ;
      }
   if (C(cID)==LBA) 	
      {
      cID=skipA(LBK, A(cID));
      }

/* Find its declaration                                     */

   cDecl=Decl(cID);
   ASSERT(cDecl>=0, whatis);

/* Analize the declatator and the type                      */

   if (cDecl) 	
      {
      cDcltr=cDecl;
      for (; C(cDcltr)!=XXdcltr; cDcltr=D(cDcltr)) 	;
      arrDcltr(cDcltr);
      cType=B(A(B(cDecl)));	/* the type */
      w=C(cType);
      if (w==INT || w==LONG || w==FLOAT || w==DOUBLE) 	cRTStype=w;	/* allowed RTS type */
      cDVM=B(B(cDecl));	/* is there a DVM-directive */
      }

/* Analize the directive                                    */

   if (cDVM) 	
      {
      cDir=A(cDVM);
      cDIR=C(cDir);	/* Directive type */
      w=0;
      if (cDIR) 	
         {
         if (cDIR==DISTRIBUTE) 	w=LBK;
         else
         if (cDIR==ALIGN || cDIR==PARALLEL) 	w=DVMind;
         if (w) 	skipA(w, toA0(w, cDir));
         dRank=Count;	/* the rank according to the directive */
         }
      }
   return 1;
   }

/*----------------------------------------------------------*/
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/* Semantic functions.                                      */
/*----------------------------------------------------------*/


/* Enclose into round brackets                              */

int Lba(int N)
   {
   return mk(LBA, N, 0);
   }

/* Add to the head of a comma-list                          */

int Comma1(int N, int x)
   {
   return N?NDD(COMMA, N, x, 0): x;
   }

/* Build a comma-list of the parameters                     */

int Comma(int no, ...)
   {
   va_list va;
   ASSERT(no!=0, Comma);
   va_start(va, no);
   for (w=0; no; no--) 	w=Comma1(w, va_arg(va, int ));
   va_end(va);
   return w;
   }

/* Convert the 'tmpList' into a bracketed comma-list        */

int BrComma()
   {
   for (w=0; tmpList; ) 	w=Comma1(w, head());
   return Lba(w);
   }

/* Add to the head of a XX-list                             */

int Xlist1(int N, int x)
   {
   if (N==0) 	return x;
   ASSERT(C(N)==XXoper || C(N)==XXdecl || C(N)==CPP, Xlist1);
   return NDD(XXlist, NDD(0, N, 0, 0), x, 0);
   }

/* Convert the 'tmpList' into an XX-list                    */

int Xlist(int N)
   {
   while (tmpList) 	N=Xlist1(head(), N);
   return N;
   }

/* Enclose into braces                                      */

int Lbs1(int xxlist)
   {
   ASSERTC(XXlist, xxlist, Lbs1);
   return NDD(XXoper, NDD(LBS, xxlist, 0, 0), 0, 0);
   }

/* Convert the 'tmpList' into a compound statement          */

int Lbs()
   {
   return Lbs1(Xlist(0));
   }

/* Create '{ a ; b; }'                                      */

int Lbs2(int a, int b)
   {
   return Lbs1(Xlist1(a, Xlist1(b, 0)));
   }

/* Create an empty operator                                 */

int NoOper()
   {
   return mk(XXoper, mk(LXskip, 0, 0), 0);
   }

/* Create a function call 'f( <N> )'                        */

int Fun(int f, int N)
   {
   return NDD(LXfun, f, N, 0);
   }

/* Create a function call statement '<s> ( <parm> );'       */

int Call(char *s, int parm)
   {
   return NDD(XXoper, NDD(XXexpr, Fun(LxiS(s), parm), 0, 0), 0, 0);
   }

/* Store created operator as the NEW-attribute              */

void NEWop(int nooper, int op)
   {
   if (nooper) 	op=NoOper();
   set(NEW, up(2), NDD(0, op, 0, 0));
   }

/* Parameters lists for DVM_ macroes                        */

int DVMx1(int x)
   {
   return Comma(2, cline(), x);
   }
int DVMx2(int x1, int x2)
   {
   return Comma(3, cline(), x1, x2);
   }
int DVMx3(int x1, int x2, int x3)
   {
   return Comma(4, cline(), x1, x2, x3);
   }
int DVMx4(int x1, int x2, int x3, int x4)
   {
   return Comma(5, cline(), x1, x2, x3, x4);
   }

/* '0'                                                      */

int null()
   {
   return Lxn(0);
   }

/* Line number as node -------------- LINENO error trap     */

int cline(void )
   {
   int ln=-D(Stack);
   if (ln==0) 	fprintf(stderr, "\nLINENO-error detected");
   ln=Lxn(ln);
   if (LX[-A(A(ln))]<'1' || LX[-A(A(ln))]>'9') 	
      {
      fprintf(stderr, "\nLINENO-error detected: bf=%s token=%s\n", bf, LX-A(A(ln)));
      PPPN(ln);
      PPPN(A(ln));
      }
   return ln;
   }

/* Cut the path from a full file name                       */

char *getName(char *s)
   {
   for (wp=s; *s; s++) 	
   if (*s=='\\' || *s=='/') 	
      {
      wp=s+1;
      }
   return wp;
   }

/* The header of enclosing parallel loop (or else 0)        */

int inPloop(int up)
   {
   int N=up?toB0(XXlist, Stack): Stack;
   for (Count=0, N=N; N; Count++, N=B(N)) 	
   if (C(A(B(A(N))))==PARALLEL) 	break ;
   return N;
   }

/* INQUIRY: In an operator (1) or in a declaration (0) ?    */

int inOper(void )
   {
   for (Count=0, w=Stack; w; Count++, w=B(w)) 	
      {
      if (C(w)==XXoper) 	return 1;
      if (C(w)==XXdcltr) 	return 0;
      }
   return 0;
   }

/* Convert subscripts '[i][j][k]' into comma-list 'i,j,k'   */

int Subscrs(int N)
   {
   switch (C(A(N)))
      {
   case LBK:
   case DVMind:
   case DVMbind: return NDD(COMMA, Subscrs(A(N)), B(N), 0);
   default : return B(N);
      }
   }

/* Convert a product 'i*j*k' into a comma-list 'i,j,k'      */

int mul2comma(int N)
   {
   return C(N)==MUL?NDD(COMMA, mul2comma(A(N)), mul2comma(B(N)), 0): N;
   }

/* Create a cast expression '(long)&<var>'                  */

int AddrAsLong(int var)
   {
   return mk(LXcast, mk(XXtype, mk(0, 0, mk(LONG, 0, 0)), 0),
   NDD(ADDR, 0, var, 0));
   }

/* Create a cast expression '(long) <var>'                  */

int AsLong(int var)
   {
   return mk(LXcast, mk(XXtype, mk(0, 0, mk(LONG, 0, 0)), 0),
   var);
   }

/* Convert a function call 'f(i,j...) into 'f[i][j]...'     */

int FunToArr(int N, int p)
   {
   if (C(p)==COMMA) 	return NDD(LBK, FunToArr(N, A(p)), B(p), 0);
   else return NDD(LBK, A(N), p, 0);
   }

/* Extract a variable from align expression 'A*v+B'         */

int Ind_A, Ind_B;
int IndVar(int N)
   {
   Ind_A=1;
   Ind_B=0;
   if (C(N)==ADD || C(N)==SUB && A(N)!=0) 	
      {
      Ind_B=B(N);
      if (C(N)==SUB) 	Ind_B=NDD(SUB, 0, Ind_B, 0);
      N=A(N);	/* X+B => X */
      }
   else Ind_B=Lxn(0);
   if (C(N)==SUB) 	
      {
      ASSERT(A(N)==0, IndVar);
      Ind_A=-1;
      N=B(N);	/* -X => X */
      }
   if (C(N)==MUL) 	
      {
      if (Ind_A==-1) 	Ind_A=NDD(SUB, 0, A(N), 0);
      else Ind_A=A(N);
      N=B(N);	/* A*X => X */
      }
   else
      {
      Ind_A=Lxn(Ind_A);
      }
   return N;
   }

/* Create the RTS name of some types and functions          */

int rts_name(int t)
   {
   char *s;
   switch (t)
      {
   case INT: s="rt_INT";
      break ;
   case LONG: s="rt_LONG";
      break ;
   case FLOAT: s="rt_FLOAT";
      break ;
   case DOUBLE: s="rt_DOUBLE";
      break ;
   case TEMPLATE: s="AMRef";
      break ;
   case LX_SG: s="ShadowGroupRef";
      break ;
   case LX_RG: s="RedGroupRef";
      break ;
   case LX_RMG: s="RegularAccessGroupRef";
      break ;
   case LX_IG: s="IndirectAccessGroupRef";
      break ;
   case PROCESSORS: s="PSRef";
      break ;
   case TASK: s="PSRef";
      break ;
   case LX_CP: s="/*CopyFlag*/long";
      break ;
   case SUM: s="rf_SUM";
      break ;
   case PROD: s="rf_PROD";
      break ;
   case MAX: s="rf_MAX";
      break ;
   case MIN: s="rf_MIN";
      break ;
   case LX_OR: s="rf_OR";
      break ;
   case LX_AND: s="rf_AND";
      break ;
   case MAXLOC: s="rf_MAX";
      break ;
   case MINLOC: s="rf_MIN";
      break ;
   case LX_free: s="DVM_FREE";
      break ;
   default : return Lxn(0);
      }
   return LxiS(s);
   }

/* Return RTS-allowed type (or else 0)                      */

int rts_type(int N)
   {
   w=C(B(A(N)));
   if (w!=INT && w!=LONG && w!=FLOAT && w!=DOUBLE) 	w=0;
   return w;
   }

/* INQUIRY: Is it a simple variable?                        */

int ISsimple(int v)
   {
   return C(v)==LXI && C(D(v))==XXdcltr;
   }

/* Unpack the comma-list 'N' to the array 'to'              */

int unpackto(int N, int to[])
   {
   if (C(N)==COMMA) 	
      {
      w=unpackto(A(N), to);
      to[w]=B(N);
      }
   else
      {
      w=0;
      to[w]=N;
      }
   return w+1;
   }

/* Code of loop or loc variable's type                      */

int intVarTp(int var)
   {
   switch (C(Type(var)))
      {
   case LONG: return 0;
   case INT: return 1;
   case SHORT: return 2;
   case CHAR: return 3;
   default : E(2084);
      return -1;
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions: 'main'                               */
/*----------------------------------------------------------*/


/* Generate standard output file heading                    */

void genhead(void )
   {
   time_t tm;
   time(&tm);
   sprintf(bf, "%s -d%d -e%d\n", OPTs?"-s": "-p", OPTd, OPTe);	/* current options */
   fprintf(fout, "\n/******************************************************\n"
   " This file has been generated %s by %s \twith options\t%s"
   "******************************************************/\n"
   "static char _SOURCE_[]=\"%s\";\n%s"
   "#include \"cdvm_c.h\"\n%s"
   "/*****************************************************/\n\n", ctime(&tm), Title, bf, getName(infile), inMain?"#define MAIN_IS_HERE\n": "", OPTs!=0?"#undef NUMBER_OF_PROCESSORS\n": "");
   }

/* INQUIRY: the current function is 'main' ?                */

int isMain(int N)
   {
   N=A(A(N));
   return (C(N)==LXfun && toA(TOKEN, N)==LX_main);
   }

/* Get an argument name                                     */

int Arg1(int N)
   {
   if (C(N)==XXparm) 	N=A(N);
   if (C(N)==XXdecl) 	N=B(N);
   N=toA0(LXI, N);
   if (N==0) 	E(2011);
   return N;
   }

/* Create argument list (of the 'main' function)            */

int Arg(int N)
   {
   N=A(A(N));
   ASSERTC(LXfun, N, Arg);
   N=B(N);
   if (N==0 || C(N)!=COMMA || C(A(N))==COMMA) 	
      {
      E(2011);
      return DVMx2(Lxi(Token("argc")), Lxi(Token("argv")));
      }
   else return DVMx2(Arg1(A(N)), Arg1(B(N)));
   }

/*  List of renaming pairs <0, old, ..., new>               */

int RenList=0;

/* Add a pair to the 'RenList'                              */

void RenPair(char *a, char *b)
   {
   RenList=NDD(Token(a), Token(b), RenList, 0);
   }

/* Fill the 'RenList'                                       */

void Reninit(void )
   {
   RenPair("exit", "DVM_EXIT");
   RenPair("FILE", "DVMFILE");
   RenPair("clearerr", "dvm_clearerr");
   RenPair("fclose", "dvm_fclose");
   RenPair("feof", "dvm_feof");
   RenPair("ferror", "dvm_ferror");
   RenPair("fflush", "dvm_fflush");
   RenPair("fgetc", "dvm_fgetc");
   RenPair("fgetpos", "dvm_fgetpos");
   RenPair("fgets", "dvm_fgets");
   RenPair("fopen", "dvm_fopen");
   RenPair("fprintf", "dvm_void_fprintf");
   RenPair("fputc", "dvm_fputc");
   RenPair("fputs", "dvm_fputs");
   RenPair("fread", "dvm_fread");
   RenPair("freopen", "dvm_freopen");
   RenPair("fscanf", "dvm_fscanf");
   RenPair("fseek", "dvm_fseek");
   RenPair("fsetpos", "dvm_fsetpos");
   RenPair("ftell", "dvm_ftell");
   RenPair("fwrite", "dvm_fwrite");
   RenPair("getc", "dvm_getc");
   RenPair("getchar", "dvm_getchar");
   RenPair("gets", "dvm_gets");
   RenPair("printf", "dvm_void_printf");
   RenPair("putc", "dvm_putc");
   RenPair("putchar", "dvm_putchar");
   RenPair("puts", "dvm_puts");
   RenPair("rewind", "dvm_rewind");
   RenPair("scanf", "dvm_scanf");
   RenPair("setbuf", "dvm_setbuf");
   RenPair("setvbuf", "dvm_setvbuf");
   RenPair("tmpfile", "dvm_tmpfile");
   RenPair("ungetc", "dvm_ungetc");
   RenPair("vfprintf", "dvm_void_vfprintf");
   RenPair("void_vprintf", "dvm_void_vprintf");
   RenPair("vprintf", "dvm_vprintf");
   RenPair("fgetchar", "dvm_fgetchar");
   RenPair("fputchar", "dvm_fputchar");
   RenPair("vfscanf", "dvm_vfscanf");
   RenPair("vscanf", "dvm_vscanf");
   RenPair("STDIN", "DVMSTDIN");
   RenPair("STDOUT", "DVMSTDOUT");
   RenPair("STDERR", "DVMSTDERR");
   RenPair("STDAUX", "DVMSTDAUX");
   RenPair("STDPRN", "DVMSTDPRN");
   RenPair("close", "dvm_close");
   RenPair("fstat", "dvm_fstat");
   RenPair("lseek", "dvm_lseek");
   RenPair("open", "dvm_open");
   RenPair("read", "dvm_read");
   RenPair("write", "dvm_write");
   RenPair("STREAM0", "DVMSTREAM0");
   RenPair("STREAM1", "DVMSTREAM1");
   RenPair("STREAM2", "DVMSTREAM2");
   RenPair("STREAM3", "DVMSTREAM3");
   RenPair("STREAM4", "DVMSTREAM4");
   RenPair("remove", "dvm_remove");
   RenPair("rename", "dvm_rename");
   RenPair("tmpnam", "dvm_tmpnam");
   RenPair("access", "dvm_access");
   RenPair("unlink", "dvm_unlink");
   RenPair("stat", "dvm_stat");
   RenList=revert(RenList);
   }

/* Convert return operator in 'main' to DVM_RETURN macro    */

void wfRETURN()
   {
   if (inMain>0) 	
      {
      mainRet=up(1);	/* keep to satisfy 'wfXXbody' */
      w=Call("DVM_RETURN", DVMx1(A(N)));
      set(NEW, mainRet, Lbs2(w, NDD(XXoper, N, 0, 0)));
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions:  implicit actions                    */
/*----------------------------------------------------------*/


/* List of global implicit actions                          */

int IMglobal=0;

/* Store implicit creation (tracing) of 'id'                */

void addIMloc(int N, int id)
   {

/*        in global scope level (SCno==1)                   */

   if (SCno==1) 	
      {
      if (IMglobal<0) 	E(2021);	/* already generated */
      else IMglobal=NDD(0, A(id), IMglobal, 0);
      wD(IMglobal, N);
      }

/*        local                                             */

   else
      {
      ASSERT(SCloc!=0, addIMloc);
      wD(SCloc, N);	/* to the last declaration in the scope */
      }
   }

/* Insert local implicit actions                            */

int genIMloc()
   {
   int r=0, loc;

/*        search in the list of local declarations          */

   for (Count=0, loc=SCloc; loc; Count++, loc=B(loc)) 	
   if (D(loc)) 	
      {
      r=Xlist1(D(loc), r);
      wD(loc, 0);	/* clear */
      }
   return r?Lbs1(r): 0;
   }

/* Insert global implicit actions                           */

int genIMglob()
   {
   int r=0, im;
   for (Count=0, im=IMglobal; im; Count++, im=B(im)) 	r=Xlist1(D(im), r);
   IMglobal=-1;	/* indication of 'is already generated' */
   return r?Lbs1(r): 0;
   }

/* WF: the 'main' function should have 'return' operator    */

void wfXXbody()
   {
   if (isMain(up(1))) 	
      {
      if (mainRet==0) 	E(2012);
      inMain=-1;	/* indication of presence of 'main' */
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions: DVM(INTERVAL) and DVM(DEBUG)         */
/*----------------------------------------------------------*/


/* Create '{ DVM_BINTER(); ... DVM_EINTER(); }'             */

int interval(int N)
   {
   int no=Lxn(++LoopNo);
   ASSERT(OPTe>=2 && tmpList==0, interval);
   if ((w=A(A(B(N))))==0) 	w=LxiS("Fic_index");	/* default */
   conc(Call("DVM_BINTER", DVMx2(no, w)));
   conc(A(N));	/* interval body */
   conc(Call("DVM_EINTER", DVMx1(no)));
   return Lbs();
   }

/* Concatenate a debugging option to 'obf' string           */

void getDopt(char *opt)
   {
   char *dopt=obf+strlen(obf);
   *dopt++=' ';
   ++opt;
   *dopt++=*opt++;
   if (*opt==0 || *opt==':') 	
      {
      *dopt++='1';	/* -d equ. -d1 */
      }
   else *dopt++=*opt++;
   if (*opt!=':') 	strcat(dopt, ":0");	/* -dx equ. -dx:0 */
   else strcat(dopt, opt);
   }

/* The root of inverted ('LIFO') debugging options list.    */

int DoptROOT=0;

/* Convert a debigging option to its internal code          */

int DoptCode(int lxv)
   {
   int Dopt;
   switch (lxv)
      {
   case optD0: Dopt=0;
      break ;
   case optD1: Dopt=1;
      break ;
   case optD2: Dopt=2;
      break ;
   case optD3: Dopt=3;
      break ;
   case optD4: Dopt=4;
      break ;
   case optE0: Dopt=100;
      break ;
   case optE1: Dopt=101;
      break ;
   case optE2: Dopt=102;
      break ;
   case optE3: Dopt=103;
      break ;
   case optE4: Dopt=104;
      break ;
   default : Dopt=-1;
      break ;
      }
   return Dopt;
   }

/* Parse 'obf' string to 'DoptROOT' tree                    */

void scanDopt()
   {
   int Dopt=0;
   int low, high;
   strcat(obf, " ;;");	/* end of options string */
   cp=obf;
   scan();	/* start parsing */
nextopt:
   Dopt=DoptCode(LXval);	/* Debugging option 'dn' or 'en' */
   if (Dopt>=0) 	scan();
   else goto err;
   if (LXval==COLON) 	scan();	/* ':' start a list of ranges */
   else goto err;
nextrange:
   if (LXcode==LXN) 	
      {
      w=atoi(LXW(LXval));	/* low interval number */
      low=w;
      high=w;
      scan();
      }
   else goto err;
   if (LXval==SUB) 	
      {
      scan();	/*   '-' -- range */
      if (LXcode==LXN) 	
         {
         w=atoi(LXW(LXval));	/* high interval number */
         high=w;
         scan();
         }
      else goto err;
      }

/* Add range to the beginning of the list 'Dopt'            */

   DoptROOT=NDD(Dopt, low, DoptROOT, high);
   if (LXval==COMMA) 	
      {
      ;	/* ',' continue with the same option */
      scan();
      goto nextrange;
      }
   if (LXval==SEMIC) 	
      {
      return ;	/* end of string */
      }
   goto nextopt;	/* continue with the next option */
err:
   *cp=0;	/* cut string tail, issue message and stop */
   sprintf(bf, "DEBUG (-d|-e) options syntax error:  %s", obf);
      {
      fprintf(stderr, "\n%s\n", bf);
      exit(-1);
      }
   ;
   }

/* correct the debigging mode according to options          */

void setDeb(int N)
   {
   int n=atoi(LXW(A(A(N))));	/* interval number */

/* find the first range contaning the current interval      */

   for (Count=0, w=DoptROOT; w; Count++, w=B(w)) 	
   if (n>=A(w) && n<=D(w)) 	
      {
      if (OPTnoe) 	
         {
         if (C(w)<100) 	
            {
            OPTcmd=C(w);
            break ;
            }
         }
      else
      if (C(w)>=100) 	
         {
         OPTcmd=C(w)-100;
         break ;
         }
      }

/* get directive's mode as maximum                          */

   for (Count=0, n=B(N); n; Count++, n=B(n)) 	
      {
      w=DoptCode(C(n));	/* internal code of the limitation */
      if (OPTnoe) 	
         {
         if (w<100) 	
            {
            OPTmax=w;
            break ;
            }
         }
      else
      if (w>=100) 	
         {
         OPTmax=w-100;
         break ;
         }
      }
   }

/* BARRIER directive                                        */

void wfBARRIER()
   {
   NEWop(0, Call("DVM_BARRIER", cline()));
   }

/*----------------------------------------------------------*/
/* Semantic functions:    debugger (data tracing)           */
/*----------------------------------------------------------*/


/* INQUIRY: is it data modification ?                       */

int inAssign(int up)
   {
   int cl=A(curlex);	/* by the next token ! */
   if (C(up)==ASSIGN && A(up)==0 || cl==ASSIGN) 	return 1;	/* assignement */
   else
   if (C(up)==INC || C(up)==DEC || cl==INC || cl==DEC || cl==AADD || cl==ASUB || cl==AMUL || cl==ADIV || cl==AMOD || cl==AAND || cl==AOR || cl==AXOR) 	return -1;	/* modifying operator */
   else return 0;
   }

/* the name of a macro as a node (see 'dt_name' )           */

int stv=0, ldv=0;

/* macro name  depending on context and current options     */

int dt_name(int N)
   {
   if (stv==0) 	stv=LxiS("\n    DVM_STV");	/* initialize 'stv' */
   if (ldv==0) 	ldv=LxiS("\n    DVM_LDV");	/* initialize 'ldv' */
   w=inAssign(up(1));
   if (w==1) 	
      {
      if (OPTd==1 || OPTd==3) 	return 0;	/* -d1, -d3 -- do not trace assignement */
      else return ldv;	/* ! -- see stv2stva */
      }
   else
   if (w==-1) 	
      {
      E(2059);
      return 0;
      }
   else
      {
      if (OPTd==1 || OPTd==3) 	return to(ASSIGN)?ldv: 0;
      else return ldv;
      }
   }

/* For assignement substitute by the DVM_STVA macro.        */

void stv2stva(int N)
   {
   w=A(N);	/* left-hand side of assignement */
   if (C(w)==LXI) 	return ;
   w=get(NEW, w);
   if (w==0) 	return ;	/* no trace macro */
   if (!(C(A(w))==LXfun && (toA(LXI, w)==stv) || toA(LXI, w)==ldv)) 	return ;	/* it is not DVM_LDV or DVM_STV */

/* Create a macro for the assignement as a whole            */

   set(NEW, N, Fun(LxiS("\n    DVM_STVA"), NDD(COMMA, B(w), B(N), 0)));
   del(NEW, A(N));	/* not for the left-hand side */
   }

/* INQUIRY: is data tracing allowed here ?                  */

int DTallowed(int scal)
   {
   int rc=0;

/*    do not trace multiple assinement                      */

   if (C(up(1))==ASSIGN && C(up(2))==ASSIGN) 	
      {
      E(2060);
      rc=1;
      }

/*    do not trace in FOR or DO header                      */

   else
   if ((w=to(LX_FOR))!=0 && A(w)==0) 	rc=2;
   else
   if ((w=to(LX_DO))!=0 && A(w)==0) 	rc=3;

/*    do not trace subscripts                               */

   else
   if (scal && (w=to(LBK))!=0 && A(w)!=0) 	rc=4;

/*    do not trace parameters                               */

   else
   if ((w=to(LXfun))!=0 && C(Stack)!=LXfun && A(w)!=0) 	
      {
      rc=6;
      }

/*    it is not an element of distributed array             */

   else
   if (cDVM) 	
      {
      if (uRank!=dRank && dRank) 	rc=7;
      }

/*    untraceble type                                       */

   else
   if (cRTStype==0) 	
      {
      E(2067);
      rc=8;
      }
   return rc==0;
   }

/* Create a trace macro (if it is allowed and possible)     */

void cDTarr(int lxi, int N, int base)
   {
   int rt, var;
   if (DTallowed(0)==0) 	return ;	/* tracing is not allowed */
   rt=rts_name(cRTStype);	/* type as identifier */
   var=get(NEW, N);	/* node may be already modified */
   if (var==0) 	var=NDD(C(N), A(N), B(N), D(N));	/* else create a copy */
      {
      int macro=dt_name(N);	/* 'DVM_LDV' or 'DVM_STV' or 0 ! */
      if (macro) 	set(NEW, N, Fun(macro, Comma(5, cline(), Lxi(cRTStype), rt, var,
      base)));
      }
   }

/* The same (as 'cDTarr') for a scalar                      */

void cDTscal(int lxi, int N)
   {
   int rt, var;
   if (DTallowed(1)==0) 	return ;	/* tracing not allowed here */
   if (Decl(lxi)<=0) 	return ;	/* undefined */
   if (C(D(Decl(lxi)))!=XXdcltr) 	return ;
   w=up(1);
   if (C(w)==LBK && w==N) 	return ;
   if (DVMdir(lxi)) 	return ;	/* it is DVM object */
   rt=rts_name(cRTStype);	/* type as identifier */
   var=NDD(DTscal, N, 0, 0);	/* insert an intermidiate node */
   wA(Stack, var);	/* ! can not set(NEW) to LXI-node !*/
      {
      int macro=dt_name(N);	/* Macro name -- or 0 */
      if (macro) 	set(NEW, var, Fun(macro, Comma(5, cline(), Lxi(cRTStype), rt, N,
      Lxn(0))));
      }
   }

/* Array registration                                       */


/*----------------------------------------------------------*/
/* Semantic functions:  REMOTE_ACCESS                       */
/*----------------------------------------------------------*/

int RAloop;

/* Find an enclosing REMOTE_ACCESS directive or clause      */

int inRemote(void )
   {
   int dir=0;
   RAloop=0;
   dir=SSfind(Stack, 23, REMOTE);	/* REMOTE directive */
   if (dir) 	return dir;
   RAloop=inPloop(0);	/* enclosing PARALLEL loop */
   if (RAloop==0) 	return 0;
   RAloop=A(B(A(RAloop)));	/* a PARALLEL directive */
   ASSERTC(PARALLEL, RAloop, inRemote);
   w=B(RAloop);	/* subgdrectives */
   for (; w; w=B(w)) 	
      {
      if (C(w)==LXX) 	dir=A(w);
      else
         {
         dir=w;
         w=0;
         }
      if (C(dir)==REMOTE) 	return dir;	/* REMOTE clause */
      }
   RAloop=0;
   return 0;
   }

/* Search for a matching ACCESS pattern                     */

int RAcmpR(int N, int ra);
int equR(int x, int y);
int RAfind(int N, int ra)
   {
   ASSERTC(REMOTE, ra, RAfind);
   if (C(N)==LXfun) 	N=FunToArr(N, B(N));
   for (Count=0, ra=B(ra); ra; Count++, ra=B(ra)) 	
      {
      int r=A(toA(DVMremote, ra));	/* next pattern */
      if (RAcmpR(N, r)) 	return r;	/* OK */
      }
   return 0;
   }
int RAcmpR(int N, int ra)
   {
   if (C(ra)==LBK) 	
      {
      if (C(N)!=LBK) 	return 0;	/* not enough subscripts */
      w=RAcmpR(A(N), A(ra));	/* recursion */
      if (B(ra)==0) 	return w;	/* empty pattern always matches */
      else return w && equR(B(N), B(ra));	/* compare subscripts */
      }
   else return equR(N, ra);	/* compare bases (as expressions) */
   }
int equR(int x, int y)
   {
   if (C(y)==DVMbase) 	return equR(x, A(y));	/* DVMbase against LXI */
   if (C(x)!=C(y)) 	return 0;	/* different operations */
   if (C(x)<LXS) 	return A(x)==A(y);	/* compare terminals */
   return equR(A(x), A(y)) && equR(B(x), B(y));	/* recursion */
   }

/* Substitute by access to a buffer                         */

int RAsubstR(int N, int ra, int bufid)
   {
   int d, var, varno;
   int ret=bufid;	/* the identifier of buffer */
   if (C(N)==LXfun) 	N=FunToArr(N, B(N));
   if (C(ra)==LBK) 	
      {
      w=RAsubstR(A(N), A(ra), bufid);	/* recursion by subscripts */
      d=D(ra);
      if (d<0) 	
         {
         if (B(ra)==0) 	ret=NDD(LBK, w, B(N), 0);	/* full dimension */
         else ret=w;	/* ignore a constant dimention */
         }
      else
         {
         varno=C(d);
         var=A(d);
         if (varno!=0) 	
            {
            if (var!=0) 	ret=NDD(LBK, w, var, 0);	/* loop variable */
            else ret=NDD(LBK, w, B(N), 0);	/* for whole dimension */
            }
         else ret=w;	/* ignore constant dimension */
         }
      }
   return ret;
   }

/* Create buffer identifier 'arr_NNN'                       */

int mk_rabufid(int lx, int n)
   {
   char id[128];
   sprintf(id, "%s_%d", LXW(lx), n);
   return LxiS(id);
   }

/* Create buffer declaration 'static long arr_NNN[rank]'    */

int mk_radcl(int ra)
   {
   int base=toA(LXI, ra);	/* array identifier */
   int bufid=mk_rabufid(A(base), ra);	/* buffer identifier */
   int rank=Rank(ra);	/* pattern rank */
   int type=mk(0, mk(STATIC, 0, 0), mk(LONG, 0, 0));
   int dcltr=NDD(XXdcltr, NDD(LBK, bufid, Lxn((rank+1)*2), 0), 0, 0);
   return NDD(XXdecl, type, NDD(0, dcltr, NDD(SEMIC, 0, 0, 0),
   0), 0);
   }

/* Create REMOTE_ACCESS macro                               */

int mk_racrt(int ra, int rg)
   {
   int base=toA(LXI, ra);	/* array identifier */
   int rank=Rank(ra);	/* pattern rank */
   int bufid=mk_rabufid(A(base), ra);	/* buffer identifier */
   int ind[MAXrank];
   int As[MAXrank];
   int Bs[MAXrank];
   int r;
   int loop;

/*    Is it REMOTE_ACCESS clause of a PARALLEL loop?        */

   loop=A(B(A(N)));
   if (C(loop)!=PARALLEL) 	loop=0;

/*    No, it is sequential branch. Old implementation!!     */

   if (loop==0) 	
      {
      if (rank>4) 	ERR("9:REMOTE array rank > 4 unsupported");

/*    Prepare parameters for macro DVM_REMOTE_BUF           */

      ind[0]=ind[1]=ind[2]=ind[3]=mk(SUB, 0, Lxn(1));	/* initialize as '-1' */
      for (i=rank-1; C(ra)==LBK; ra=A(ra), i--) 	
         {
         ASSERT(i>=0, mk_racrt);
         if (D(ra)>0 && C(D(ra))==LXI) 	continue ;
         if (B(ra)) 	ind[i]=B(ra);	/* subscript from the pattern */
         }

/*    Create macro                                          */

      r=Call("\n DVM_REMOTE_BUF", Comma(7, bufid, Lxn(rank), base, ind[0], ind[1], ind[2],
      ind[3]));
      }

/*    In a PARALLEL loop. New implementation                */

   else
      {
      int is, as, bs, ls, hs, ss;
      for (i=rank-1; C(ra)==LBK; ra=A(ra), i--) 	
         {
         ASSERT(D(ra)!=0, mk_remote20);
         w=D(ra);
         ind[i]=Lxn(C(w));
         w=B(w);
         As[i]=A(w);
         Bs[i]=B(w);
         }

/*    Prepare parameters for macro DVM_REMOTE20G            */

      is=ind[0];
      as=As[0];
      bs=Bs[0];
      for (i=1; i<rank; i++) 	
         {
         is=mk(COMMA, is, ind[i]);
         as=mk(COMMA, as, As[i]);
         bs=mk(COMMA, bs, Bs[i]);
         }

/*    Create macro                                          */

      if (rg!=0) 	
         {
         r=Call("\n DVM_REMOTE20G", Comma(9, cline(), D(loop), A(rg), base, bufid, Lxn(rank),
         Lba(is), Lba(as), Lba(bs)));
         }
      else
         {
         r=Call("\n DVM_REMOTE20", Comma(8, cline(), D(loop), base, bufid, Lxn(rank), Lba(is),
         Lba(as), Lba(bs)));
         }
      }
   return r;
   }

/* Find pattern subscripts with loop variables              */

void wfREMOTE(void )
   {
   int ra, lbk;
   int loop=to(PARALLEL);	/* a PARALLEL directive */
   int loopid=D(loop);	/* loop number */
   int loopvars, lv;
   int expr, var, varno;
   if (loop==0) 	return ;	/* not in a loop */
   loopvars=A(A(loop));	/* loop variables */

/*    along patterns list                                   */

   for (Count=0, ra=B(N); ra; Count++, ra=B(ra)) 	
      {
      int r=A(toA(DVMremote, ra));	/* next pattern */

/*    along subscripts                                      */

      for (Count=0, lbk=r; C(lbk)==LBK; Count++, lbk=A(lbk)) 	
         {
         expr=B(lbk);

/*    Dimension as a whole -> (-1 0 0)                      */

         if (expr==0) 	
            {
            varno=-1;
            var=0;
            Ind_A=Lxn(0);
            Ind_B=Lxn(0);
            goto wrD;
            }
         var=IndVar(expr);	/* analyze the expression */
         if (var==0 || C(var)==LXN || C(var)==LBA) 	goto ce;	/* it is not a variable */
         if (C(var)!=LXI) 	
            {
            goto ce;
            }

/*    along loop variables list                             */

         varno=Len(DVMind, loopvars)-1;	/* Loop rank */
         for (Count=0, lv=loopvars; C(lv)==DVMind && A(var)!=A(B(lv)); Count++, lv=A(lv)) 	
            {
            varno--;
            }
         if (C(lv)!=DVMind) 	
            {
            goto ce;	/* not found */
            }

/*    For a loop variable ( A*vn+B ) -> (n,A,B)             */

         goto wrD;
ce:

/*    A constant expression -> (0,0,C)                      */

         var=0;
         varno=0;
         Ind_A=Lxn(0);
         Ind_B=expr;
wrD:
         wD(lbk, NDD(varno, var, NDD(0, Ind_A, Ind_B, 0), 0));
         }
      }
   }

/* Create REMOTE_ACCESS block                               */

int mk_remote(int dir, int oper)
   {
   int ra;
   int rg=A(dir);	/* remote_group identifier (or 0) */
   ASSERT(tmpList==0, mk_remote);
   for (Count=0, ra=B(dir); ra; Count++, ra=B(ra)) 	
      {
      w=C(ra)==LXX?A(ra): ra;
      conc(mk_radcl(A(w)));	/* declaration */
      }
   conc(Call("\n DVM_BLOCK_BEG", cline()));
   for (Count=0, ra=B(dir); ra; Count++, ra=B(ra)) 	
      {
      w=C(ra)==LXX?A(ra): ra;
      conc(mk_racrt(A(w), rg));	/* creation */
      }
   conc(oper);	/* the body of REMOTE_ACCESS */
   conc(Call("\n DVM_BLOCK_END", cline()));
   return Lbs();	/* make compound statement */
   }
void wfLXIag()
   {
   definedDVM(A(N));
   if (cDIR==LX_RMG || cDIR==LX_IG) 	return ;
   else
   if (cDIR) 	E(2043);
   }

/* Build declarators for 'remote_group prefetched' flags    */

int crRMGdcltrs(int N)
   {
   int id;	/* flag identifier */
   int init=mk(ASSIGN, Lxn(0), 0);	/* initialize as '=0;' */
   ASSERTC(XXdcltr, A(N), crRMGdcltrs);
   ASSERTC(LXaster, A(A(N)), crRMGdcltrs2);
   id=toA(LXI, A(A(N)));	/* remote_group name */
   id=pref_name("RMG_", id);	/* 'RMG_<group>' */
   if (C(B(N))==XXclist) 	w=NDD(XXclist, crRMGdcltrs(A(B(N))), 0, 0);	/* recursion */
   else w=B(N);
   return NDD(0, NDD(XXdcltr, id, init, 0), w, 0);	/* declarators list */
   }

/* PREFETCH directive -> DVM_PREFETCH macro                 */

void wfPREFETCH()
   {
   int id=toA(LXI, N);
   char *macro;
   whatis(id);
   if (cDIR==LX_RMG) 	macro="DVM_PREFETCH";
   else macro="DVM_PREFETCHI";
   NEWop(OPTs, Call(macro, DVMx1(id)));
   }

/* RESET directive -> DVM_RESET macro                       */

void wfRESET()
   {
   int id=toA(LXI, N);
   char *macro;
   whatis(id);
   if (cDIR==LX_RMG) 	macro="DVM_RESET";
   else macro="DVM_RESETI";
   NEWop(OPTs, Call(macro, DVMx1(id)));
   }

/*----------------------------------------------------------*/
/* Semantic functions: access to distributed data           */
/*----------------------------------------------------------*/


/* Create a type parameter of DAElm as identifier           */
/* DVM allowes 'int', 'long', 'float', and 'double'         */

int mk_datype(int N, int type)
   {
   if (C(type)==INT) 	w=NDD(COMMA, N, Lxi(INT), 0);
   else
   if (C(type)==LONG) 	w=NDD(COMMA, N, Lxi(LONG), 0);
   else
   if (C(type)==FLOAT) 	w=NDD(COMMA, N, Lxi(FLOAT), 0);
   else
   if (C(type)==DOUBLE) 	w=NDD(COMMA, N, Lxi(DOUBLE), 0);
   else ERR("0:Non-DVM type");
   return w;
   }

/* DAElm's parameters: (type, subscripts,...)               */

int mk_daind(int N, int type, int lbk)
   {
   int w;

/*    'fun' is a macro emulating multidimentional array     */
/*    Subscripts are already a COMMA-list                   */

   if (C(N)==LXfun) 	w=Lop(COMMA, mk_datype(A(N), type), B(N));

/*    Ordinary bracketed subscripts. Recursion.             */

   else
   if (lbk) 	w=NDD(COMMA, mk_daind(A(N), type, lbk-1), B(N), 0);

/*    The end of recursion. A type as the first parameter.  */

   else w=mk_datype(N, type);
   return w;
   }
int substDAlist=0;
int substDA(int lxi, int yes, int Rank)
   {
   if (inPloop(0)==0) 	return 0;
   for (Count=0, lxi=lxi; lxi && C(lxi)!=LXI; Count++, lxi=A(lxi)) 	;
   for (Count=0, w=substDAlist; w; Count++, w=A(w)) 	
      {
      if (B(w)==lxi) 	
         {
         if (yes==0) 	wC(w, 0);
         return C(w);
         }
      }
   substDAlist=NDD(LXX, substDAlist, lxi, 0);
   if (yes==0) 	
      {
      wC(substDAlist, 0);
      return 0;
      }
   wC(substDAlist, Rank);
   return 1;
   }

/* Verify and rebuild access to distributed data            */

void ISWFaccess(int N)
   {
   int ptr, elem;
   int base;
   int ra, ra20=0;
   int inloop;
   if (!inOper()) 	return ;	/* in operators only */
   if (whatis(N)==0) 	return ;	/* Unknown. Hence non-DVM. */

/*    If it is declared with a DVM-directive...             */
/*    as distributed array                                  */

   if (cDVM!=0) 	
   if (cDIR==DISTRIBUTE || cDIR==ALIGN) 	
      {
      ptr=B(cDVM);	/* Is it a DVM-pointer? */
      if (uRank>cLBKl) 	elem=1;	/* an element */
      else
      if (uRank==cLBKl) 	elem=0;	/* DA as a whole */
      else
      if (uRank<cLBKl) 	elem=-1;	/* array of DVM-pointers */

/*    Valid assignements are: <arr>=malloc() or <ptr>=...   */


/*    (Look forward two tokens: '=', 'malloc'.)             */

      if (A(curlex)==ASSIGN) 	
         {
         if (A(curlex+1)==LX_malloc) 	
            {
            if (elem || ptr) 	E(2025);	/* It is not an array */
            if (cASTER==0) 	E(2026);	/* Static already allocated */
            }
         else
            {
            if (elem==0 && ptr==0) 	E(2027);	/* Array should be directly malloc'ed */
            if (elem==0) 	
               {
               substDA(cID, 0, 0);
               }
            }
         }

/*    Distributed IO requires an array as a whole           */

      if (to(LXfun)) 	
         {
         w=up(1);
         if (C(w)==LXfun && (toA(TOKEN, w)==Token("fread") || toA(TOKEN, w)==Token("fwrite"))) 	
         if (elem!=0) 	E(2028);
         }

/*    Nothing to do with an array as a wh0le                */

      if (elem==0) 	return ;
      }
   if (!to(LX_DVM)) 	
      {

/*    Trace scalar                                          */

      if (uRank==0) 	
         {
         if (OPTd==3 || OPTd==4) 	cDTscal(cID, N);
         }

/*    Array                                                 */

      else
         {
         if (cDecl<=0) 	return ;	/* undefined */

/*    If this is a valid remote access                      */

         base=cID;
         ra=inRemote();
         ra20=A(ra);
         inloop=A(B(A(ra)));
         if (C(inloop)!=PARALLEL) 	inloop=0;
         if (ra!=0 && (ra=RAfind(N, ra))!=0) 	
            {
            if (inAssign(up(1))) 	E(2065);
            else
               {
               base=mk_rabufid(A(cID), ra);
               N=RAsubstR(N, ra, base);	/* access to a buffer */
               if (C(N)!=LBK) 	N=NDD(LBK, N, Lxn(0), 0);	/* 0-dim */
               skipA(LBK, N);
               uRank=Count;	/* new dimension */
               dRank=0;
               }
            }

/*    Access to an element with RTL macro DAElm or RBElm    */

         if (OPTs==0) 	
         if (uRank && cDir && (uRank>=dRank || dRank==0)) 	
            {
            char *name;
            if (uRank<1) 	
               {
               E(2000);
               return ;
               }
            if (uRank>6) 	
               {
               E(2031);
               return ;
               }
            if (ra==0 || RAloop==0) 	
               {
               w=substDA(N, 1, uRank);
               if (w==0)	/* Common access macro */
               name=(uRank==1?"\n   DAElm1": uRank==2?"\n   DAElm2": uRank==3?"\n   DAElm3": uRank==4?"\n   DAElm4": uRank==5?"\n   DAElm5": uRank==6?"\n   DAElm6": (char *)0);
               else name=(uRank==1?"\n   DVMda1": uRank==2?"\n   DVMda2": uRank==3?"\n   DVMda3": uRank==4?"\n   DVMda4": uRank==5?"\n   DVMda5": uRank==6?"\n   DVMda6": (char *)0);
               }
            else
               {
               substDA(N, 0, 0);
               name=(uRank==1?"\n   RBElm1": uRank==2?"\n   RBElm2": uRank==3?"\n   RBElm3": uRank==4?"\n   RBElm4": uRank==5?"\n   RBElm5": uRank==6?"\n   RBElm6": (char *)0);
               }
            set(NEW, A(Stack), NDD(LXfun, LxiS(name), mk_daind(N, B(A(B(cID))), uRank), 0));
            }

/*    Trace array element                                   */

         if (OPTd) 	cDTarr(cID, N, base);
         }
      }
   return ;
   }

/* Verify current subscript                                 */

void wfLBK()
   {
   if (C(B(N))==COMMA) 	E(2099);	/* What is "[i,j]" ? */

/*    Is it the rightmost subscript?                        */

   w=C(curlex)==DLM && (A(curlex)==LBK);	/* the next token !*/
   if (w==0) 	
      {
      if (C(up(3))==DISTRIBUTE) 	
         {
         skipA(LBK, N);
         if (Count>6) 	E(2024);
         }
      else ISWFaccess(N);	/* Process an access */
      }
   }

/* Verify an identifier                                     */

void wfLXI()
   {
   if (D(N)<0) 	
      {
      w=SSfind(B(A(N)), 0, LXI);
      if (w) 	
         {
         wD(N, 0);
         wA(Stack, w);
         N=w;
         }
      }

/*    Is it subscripted ?                                   */

   w=C(curlex)==DLM && (A(curlex)==LBK || A(curlex)==LBA);
   if (w==0) 	ISWFaccess(N);	/* Process an access */
   }

/* A function call                                          */

void wfLXfun()
   {

/*    "free(<DVM-array>);" convert to DVM_FREE macro        */

   if (toA(TOKEN, N)==LX_free && DVMdir(toA(LXI, B(N)))) 	
      {
      if (OPTs==0) 	set(NEW, N, Fun(rts_name(LX_free), DVMx1(B(N))));
      return ;
      }

/*    Macros may look like a function call                  */

   if (inOper() && DVMdir(A(N))) 	
      {
      w=D(Decl(A(N)));
      if (C(w)==LXfun) 	return ;	/* declared as a function */
      ISWFaccess(N);	/* Process as an access */
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions:  own statments                       */
/*----------------------------------------------------------*/


/* Convert subscripts to a COMMA-list                       */

int mk_ind_list(int N)
   {
   if (C(A(N))!=LBK) 	return B(N);	/* already a list */
   if (C(A(A(N)))==LBK) 	return NDD(COMMA, mk_ind_list(A(N)), B(N), 0);	/* recursion */
   else return NDD(COMMA, B(A(N)), B(N), 0);	/* the last subscript */
   }

/* Make an own block                                        */

void mk_local(int N)
   {
   int ass, islocal, w;
   ASSERTC(XXoper, N, mk_local);
   ass=A(A(N));
   whatis(A(ass));	/* analyse the base */
   if (cDVM==0 || uRank==0 || uRank<dRank) 	return ;	/* Nothing to do */
   w=up(1);
   E(2068);	/* Weak warning. */
   set(NEW, w, NDD(0, N, 0, 0));

/*    Make condition: " IS_LOCAL (...) " or "1".            */

   if (OPTs) 	islocal=Lxn(1);
   else islocal=Fun(LxiS("DVM_ISLOCAL"), DVMx3(cID, Lxn(uRank), Lba(mk_ind_list(A(ass)))));

/*    Make an if-operator " if(<condition>) <oper>;"        */

   w=NDD(XXoper, A(N), 0, 0);
   w=NDD(IF, NDD(0, Lba(islocal), w, 0), 0, 0);
   w=NDD(XXoper, w, 0, 0);
   w=Lbs1(Xlist1(w, 0));

/*    Make a block                                          */

   w=Lbs2(w, OPTs && OPTd==0?NoOper(): Call("\n    DVM_ENDLOCAL", cline()));
   set(NEW, N, w);	/* Store the operator */
   }

/*----------------------------------------------------------*/
/* Semantic functions:  malloc                              */
/*----------------------------------------------------------*/


/* Distributed array creation: <lhs>=malloc(<parm>);        */

int mk_alloc(int lhs, int parm, int stmt)
   {
   int amv, dims, sh, redis;
   int dir=0, exs=0, vars=0;
   if (OPTs!=0 && OPTd==0) 	return stmt;
   ASSERT(tmpList==0, mk_alloc);
   whatis(lhs);	/* analyse the lefthand side */
   dims=mul2comma(A(parm));	/* convert to a COMMA-list */
   skipA(COMMA, dims);
   uRank=Count+1;	/* the number of dimensions */
   if (OPTs==0) 	
      {

/*    Is it DISTRIBUTEd ?                                   */

      if (cDIR==DISTRIBUTE) 	
         {
         redis=3;	/* allow redistribution */
         sh=C(B(cDir))==SHADOW?B(cDir): 0;	/* get shadows */
         if (dRank!=0) 	
            {
            amv=LxiS("DVM_AMV");	/* implicit template */
            mk_templ(amv, dims, cDir);	/* create template */
            conc(Lbs());
            }
         }

/*    or ALIGNed ?                                          */

      else
      if (cDIR==ALIGN) 	
         {
         redis=2;	/* allow realign only */
         sh=C(B(cDir))==SHADOW?B(cDir): 0;	/* shadows */
         if (dRank!=0) 	
            {
            dir=A(A(cDir));
            amv=toA(LXI, B(dir));	/* the base of target */
            exs=Subscrs(B(dir));	/* alignement expressions */
            vars=Subscrs(A(dir));
            w=Decl(amv);
            w=B(B(w));	/*  DVM-directive */
            w=B(w);	/* pointer? */
            if (w) 	amv=NDD(CONT, 0, amv, 0);
            }
         }
      else ASSERT(0, mk_alloc);
      }
   if (OPTs!=0) 	
      {
      if (stmt!=0) 	conc(stmt);
      }
   else
      {

/*    Macro DVM_MALLOC hiding RTL's crtda_() function       */

      conc(Call("DVM_MALLOC", Comma(8, cline(), lhs, Lxn(uRank), B(parm), Lba(dims), cSHw(0,
      sh, uRank), cSHw(1, sh, uRank), Lxn(redis))));

/*    Prepare alignement (default or explicit)              */

      if (cDIR==DISTRIBUTE) 	
         {
         vars=uRank;
         }
      else uRank=LenR(COMMA, exs);

/*    Align immediatly or look for a RE directive           */

      if (dRank!=0) 	
         {
         conc(Call("DVM_ALIGN", Comma(7, cline(), lhs, amv, Lxn(uRank), Align(0, exs,
         vars), Align(1, exs, vars), Align(2, exs, vars))));
         }
      else
         {
         w=B(A(B(A(Stack))));
         if (w==0 || C(A(w))!=REDISTRIBUTE && C(A(w))!=REALIGN) 	E(2058);
         }
      }
   if (OPTd!=0) 	
      {
      int type=A(A(B(parm)));
      ASSERTC(XXtype, type, mk_alloc);
      type=B(A(type));
      conc(Call("DVM_REGARR", DVMx4(Lxn(LenR(COMMA, dims)), rts_name(cRTStype), lhs, Lba(dims))));
      }
   return Lbs();
   }

/* Postponed alignement (from a REDISTRIBUTE directive)     */

int mk_alloc2(int id, int base, int axis, int ps, int parm)
   {
   int amv;
   int dims=mul2comma(A(parm));
   int axno;
   ASSERT(tmpList==0, mk_alloc2);
   whatis(id);	/* analyse */
   ASSERT(cDIR==DISTRIBUTE, mk_alloc2);
   skipA(COMMA, dims);
   uRank=Count+1;

/*    Create implicit template                              */

   amv=LxiS("DVM_AMV");
   conc(Call("DVM_CREATE_TEMPLATE", DVMx4(Lxn(0), amv, Lxn(Len(COMMA, dims)), Lba(dims))));
   axno=Len(COMMA, axis);

/*    Get PS from the ONTO clause (or use dafault one)      */

   if (ps==0) 	ps=B(A(A(cDir)));
   if (ps==0) 	ps=null();
   else
      {
      ASSERTC(ONTO, ps, mk_alloc2);
      w=get(ONTO, ps);
      if (w && B(w)<axno) 	E(2031);
      w=A(w);
      if (w) 	
         {
         conc(w);
         ps=LxiS("DVM_PS");
         }
      else ps=A(ps);
      }

/*    Prepare the PS according to GENBLOCK format           */

   mk_genblock(Lxn(0), ps);
   mk_multblock(amv);

/*    Distribute the template (onto the PS)                 */

   conc(Call("DVM_DISTRIBUTE", DVMx4(amv, ps, Lxn(axno), Lba(axis))));

/*    Align the array with the template                     */

   conc(Call("DVM_ALIGN", Comma(7, cline(), base, amv, Lxn(uRank), Align(0, 0,
   uRank), Align(1, 0, uRank), Align(2, 0, uRank))));
   return Lbs();
   }

/*----------------------------------------------------------*/
/* Semantic functions:  GENBLOCK format                     */
/*----------------------------------------------------------*/


/* Recursive part of GENaxis                                */

int GENaxisR(int N)
   {
   int gb;
   if (C(N)!=LBK) 	return 0;	/* stop recursion */
   else w=GENaxisR(A(N));	/* recurtion */
   if (B(N)==0) 	return w;	/* skip non-distributed dimention */
   if (C(B(N))==GENBLOCK || C(B(N))==WGTBLOCK) 	gb=AddrAsLong(A(B(N)));	/* GENBLOCK array address */
   else gb=Lxn(0);	/* or 0 for BLOCK distributed */
   return w==0?gb: NDD(COMMA, w, gb, 0);
   }

/* Create a list of GENBLOCK arrays addresses               */

int GENaxis(int N)
   {
   N=toA0(LBK, N);
   for (Count=0, w=N; C(w)==LBK; Count++, w=A(w)) 	
   if (C(B(w))==GENBLOCK || C(B(w))==WGTBLOCK) 	return GENaxisR(N);
   return 0;	/* There are no GENBLOCKs */
   }

/* Recursive part of GENmbs                                 */

int GENmbsR(int N)
   {
   int gb;
   if (C(N)!=LBK) 	return 0;	/* stop recursion */
   else w=GENmbsR(A(N));	/* recurtion */
   if (B(N)!=0 && C(B(N))==MULTBLOCK) 	gb=B(B(N));
   else gb=Lxn(1);
   return w==0?gb: NDD(COMMA, w, gb, 0);
   }

/* Create a list of MULTBLOCK arrays addresses              */

int GENmbs(int N)
   {
   N=toA0(LBK, N);
   for (Count=0, w=N; C(w)==LBK; Count++, w=A(w)) 	;
   return GENmbsR(N);
   return 0;
   }

/* Recursive part of GENlng                                 */

int GENlngR(int N)
   {
   int gb;
   if (C(N)!=LBK) 	return 0;	/* stop recursion */
   else w=GENlngR(A(N));	/* recurtion */
   if (B(N)==0) 	return w;	/* skip non-distributed dimention */
   if (C(B(N))==GENBLOCK || C(B(N))==WGTBLOCK) 	gb=B(B(N));	/* WGTBLOCK array length */
   else gb=Lxn(0);	/* or else 0 */
   return w==0?gb: NDD(COMMA, w, gb, 0);
   }

/* Create a list of WGTBLOCK arrays lengths                 */

int GENlng(int N)
   {
   N=toA0(LBK, N);
   for (Count=0, w=N; C(w)==LBK; Count++, w=A(w)) 	
   if (C(B(w))==WGTBLOCK) 	return GENlngR(N);
   return 0;	/* There are no WGTBLOCKs */
   }

/* Create DVM_GENBLOCK macro if necessary                   */

int mk_genblock(int amv, int ps)
   {
   int gaxis, wgtlng, mbs;

/*    Get parameters from directive or from declaration     */

   if (C(N)==REDISTRIBUTE) 	gaxis=GENaxis(N);
   else gaxis=GENaxis(cDir);
   if (C(N)==REDISTRIBUTE) 	wgtlng=GENlng(N);
   else wgtlng=GENlng(cDir);
   if (gaxis==0) 	return 0;	/* nothing to do */

/*    Create the macro                                      */

   if (wgtlng==0) 	conc(Call("DVM_GENBLOCK", DVMx4(amv, ps, Lxn(Len(COMMA, gaxis)), Lba(gaxis))));
   else
   if (Len(COMMA, gaxis)!=Len(COMMA, wgtlng)) 	
      {
      E(2083);
      return 0;
      }
   else conc(Call("DVM_WGTBLOCK", Comma(6, cline(), amv, ps, Lxn(Len(COMMA, gaxis)), Lba(gaxis),
   Lba(wgtlng))));
   return 0;
   }

/* Create DVM_GENBLOCK macro if necessary                   */

int mk_multblock(int amv)
   {
   int mbs;
   if (C(N)==REDISTRIBUTE) 	mbs=GENmbs(N);
   else mbs=GENmbs(cDir);
   if (mbs) 	conc(Call("DVM_MULTBLOCK", Comma(4, cline(), amv, Lxn(Len(COMMA, mbs)), Lba(mbs))));
   return 0;
   }

/* Verify GENBLOCK distribution format                      */

void wfGENBLOCK()
   {
   whatis(A(N));	/* analyse the parameter */
   if (cDecl==0) 	E(2040);	/* undefined */
   else
   if (cDir || C(cType)!=INT || cLBKl+cASTER+cLBKr!=1) 	E(2069);	/* wrong type */
   }

/* Verify WGTBLOCK distribution format                      */

void wfWGTBLOCK()
   {
   whatis(A(N));	/* analyse the parameter */
   if (cDecl==0) 	E(2040);	/* undefined */
   else
   if (cDir || C(cType)!=DOUBLE || cLBKl+cASTER+cLBKr!=1) 	E(2082);	/* wrong type */
   }

/* Verify MULTBLOCK distribution format                     */

void wfMULTBLOCK()
   {
   whatis(A(N));	/* analyse the parameter */
   if (cDecl==0) 	E(2040);	/* undefined */
   else
   if (cDir || C(cType)!=INT || cLBKl+cASTER+cLBKr!=1) 	E(2069);	/* wrong type */
   }

/*----------------------------------------------------------*/
/* Semantic functions:  DISTRIBUTE directive                */
/*----------------------------------------------------------*/


/* Verify DISTRIBUTE directive: (* DISTRIBUTE ...[*]...)    */

void wfDISTRIBUTE()
   {
   if (B(up(1))==0) 	
      {
      for (Count=0, w=A(N); w; Count++, w=A(w)) 	
      if (C(B(w))==MUL) 	E(2035);
      }
   }

/* REDISTRIBUTE directive                                   */

void wfREDISTRIBUTE()
   {
   int ps=B(A(A(N)));	/* the ONTO-clause */
   int rnew=B(A(N))!=0;	/* 'NEW' */
   int stuff;
   int axis;
   int axno;
   stuff=A(A(A(N)));
   ASSERTC(LBK, stuff, wfREDISTRIBUTE);
   axis=AMaxis(stuff);	/* list of distribued dimensions */
   whatis(stuff);	/* analyse redistributed array */

/*    [*] distribution format is not allowed                */

   for (Count=0, w=A(N); w && C(w)!=DVMbase; Count++, w=A(w)) 	
   if (C(B(w))==MUL) 	E(2035);

/*    It is not DISTRIBUTEd array (template)                */

   if (cDIR!=DISTRIBUTE) 	E(2036);
   if (dRank && dRank!=uRank) 	E(2020);

/*    For sequential program erase the directive            */

   if (OPTs) 	
      {
      NEWop(1, 0);
      return ;
      }

/*    Postponed alignement; incomplete malloc               */

   if (dRank==0) 	
      {
      w=B(toB(XXlist, Stack));
      ASSERTC(XXlist, w, wfREDISTR2);
      w=A(A(A(w)));
      ASSERTC(XXoper, w, wfREDISTR3);
      w=A(A(w));	/* the previous operator */
      if (C(w)==ASSIGN && C(w=B(w))==LXfun && toA(TOKEN, w)==LX_malloc) 	
         {
         int base=skipA(LBK, stuff);
         base=A(base);
         if (OPTs==0) 	NEWop(0, mk_alloc2(cID, base, axis, ps, B(w)));
         return ;
         }
      }
   ASSERT(tmpList==0, wfREDISTRIBUTE);

/*    Normal redistribution                                 */

   axno=Len(COMMA, axis);	/* the number of distributed dims */
   if (ps==0) 	ps=null();	/* default PS */
   else
      {
      ASSERTC(ONTO, ps, wfREDISTR4);
      w=get(ONTO, ps);	/* ONTO implementation */
      if (w && B(w)<axno) 	E(2031);	/* not enough dims */
      w=A(w);	/* DVM_ONTO macro for PS section creation */
      if (w) 	
         {
         conc(w);
         ps=LxiS("DVM_PS");
         }
      else ps=A(ps);	/* PS as a whole */
      }

/*    Prepare the PS according to GENBLOCK format           */

   mk_genblock(cID, ps);
   mk_multblock(cID);

/*    DVM_REDISTRIBUTE macro                                */

   conc(Call("DVM_REDISTRIBUTE", Comma(6, cline(), cID, ps, Lxn(axno), Lba(axis), Lxn(rnew))));
   NEWop(0, Lbs());
   }

/*----------------------------------------------------------*/
/* Semantic functions:  ONTO & TASK                         */
/*----------------------------------------------------------*/


/* Create a COMMA-list of low or high bounds of ranges      */

int Ranges(int lh, int N, int dim)
   {
   int b=B(N);	/* current range: [] [low] [low:high] */
   if (lh==0) 	b=C(b)==COLON?A(b): b;	/* low */
   else
   if (lh==1) 	b=C(b)==COLON?B(b): b;	/* high */
   if (b==0) 	
      {
      if (lh==0) 	b=Lxn(0);	/* default low = 0 */
      else b=Fun(LxiS("DVM_PSSIZE"), Lxn(dim));	/* default high */
      }
   if (C(A(N))==LBK) 	b=NDD(COMMA, Ranges(lh, A(N), dim-1), b, 0);	/* recursion */
   return b;
   }

/* Make DVM_ONTO macro for PS section                       */

int crONTO(int N)
   {
   int ls=0, hs=0, k, id;
   id=skipA(LBK, N);	/* base PS */
   k=Count;	/* section rank */
   if (k) 	
      {
      ls=Ranges(0, N, k-1);	/* low bounds */
      hs=Ranges(1, N, k-1);	/* high bounds */
      }
   return Call("DVM_ONTO", DVMx4(A(id), Lxn(k), Lba(ls), Lba(hs)));
   }

/* Verify ONTO clause                                       */

void wfONTO()
   {
   whatis(A(N));	/* analyse PS */

/*    Undefined or non-DVM                                  */

   if (cDecl==0) 	
      {
      E(2040);
      return ;
      }
   else
   if (cDir==0) 	
      {
      E(2039);
      return ;
      }

/*    MAP target must be processors section                 */

   if (to(MAP)) 	
      {
      if (!(cDIR==PROCESSORS && uRank>0)) 	
         {
         E(2061);
         return ;
         }
      }

/*    Only an element of task array allowed as target       */

   if (cDIR==TASK) 	
      {
      if (uRank!=1) 	
         {
         E(2057);
         return ;
         }
      }

/*    Processors arrangement or section                     */

   else
   if (cDIR==PROCESSORS) 	
      {
      if (cLBKr!=0 || cASTER==0) 	
         {
         E(2064);
         return ;
         }
      if (uRank && cLBKl!=uRank) 	
         {
         E(2020);
         return ;
         }
      if (OPTs==0) 	set(ONTO, N, NDD(0, uRank?crONTO(A(N)): 0, cLBKl, 0));
      }
   }

/* MAP directive                                            */

void wfMAP()
   {
   w=get(ONTO, B(N));	/* ONTO clause implementation */
   if (w==0) 	return ;
   w=A(w);
   NEWop(0, Call("DVM_MAP", DVMx3(A(A(N)), B(A(N)), A(A(w)))));
   }

/* Convert ON-block:   DVM(ON <task>[<ind>]) <oper>;        */

int mkRUNAM(int N, int oper)
   {
   int task=toA(LXI, N);	/* TASK array */
   int ind=B(N);	/* task number */
   int cond;

/*    For parallel:                                         */
/*    if(DVM_RUN()) {DVM_NTASK(); <oper>; DVM_STOP(); }     */

   if (OPTs==0) 	
      {
      cond=Lba(Fun(LxiS("DVM_RUN"), DVMx2(task, ind)));
      if (OPTd) 	conc(Call("DVM_NTASK", DVMx1(ind)));
      conc(oper);
      conc(Call("DVM_STOP", cline()));
      return mk(XXoper, mk(IF, mk(0, cond, Lbs()), 0), 0);
      }

/*    Always for debugger: { DVM_NTASK; oper; }             */

   else
   if (OPTd) 	
      {
      conc(Call("DVM_NTASK", DVMx1(ind)));
      conc(oper);
      return Lbs();
      }

/*    Else do not modify                                    */

   else return oper;
   }

/* Verify and convert DVM(TASK_REGION <task> ...) {...}     */

int wfTASKREGION(int N)
   {
   int no=0, lxx;
   int dir=A(B(N));
   int task;
   int loopid=D(dir);
   int subdirs=B(A(B(N)));	/* task reduction */
   int reduction=0, reduction20=0;
   int red_group=0, red_vars=0;
   ASSERTC(LX_TASKREG, dir, wfTASKREGION);
   N=A(N);
   ASSERTC(XXlist, A(A(N)), wfTASKREGION);

/*    A list of ON-blocks or the sole ON-loop               */

   for (Count=0, lxx=A(A(N)); lxx; Count++, lxx=B(lxx)) 	
      {
      w=B(A(lxx));
      if (w==0) 	goto err;	/* no DVM-dir */
      w=A(w);
      if (no==-1) 	goto err;	/* s-th after ON-loop  */
      if (C(w)==TASKLOOP) 	
         {
         if (no!=0) 	goto err;	/* ON-loop after s-th else */
         no=-1;
         continue ;
         }
      else
      if (C(w)==ON) 	
         {
         no=1;
         continue ;
         }
      else goto err;	/* s-th other than ON-block or ON-loop */
      }

/*    Look for (and unpask) REDUCTION subdirective          */

   for (; subdirs; subdirs=B(subdirs)) 	
      {
      if (C(subdirs)==LXX) 	w=A(subdirs);
      else
         {
         w=subdirs;
         subdirs=0;
         }
      if (C(w)==REDUCTION) 	
         {
         red_group=A(A(w));
         red_vars=get(NEW, w);
         }
      }
   task=OPTs?Lxn(0): A(A(dir));

/*    Start creation                                        */

   conc(Call("DVM_TASKREGION", DVMx2(loopid, task)));

/*    Synchronious reduction                                */

   if (red_vars && red_group==0) 	
      {
      if (OPTs==0 || OPTd) 	conc(Call("\n DVM_REDUCTION", DVMx4(OPTs?Lxn(0): loopid, red_vars, Lxn(OPTs), Lxn(OPTd))));
      }

/*    Group reduction                                       */

   if (red_vars && red_group) 	
      {
      conc(Call("\n DVM_CREATE_RG", DVMx4(red_group, red_vars, Lxn(OPTs), Lxn(OPTd))));
      conc(Call("\n DVM_REDUCTION20", DVMx4(OPTs?Lxn(0): loopid, red_group, Lxn(OPTs), Lxn(OPTd))));
      }

/*    Body:  [DVM_BTASK;] oper; [DVM_ETASK;]                */

   if (OPTd) 	conc(Call("DVM_BTASK", DVMx1(loopid)));
   conc(N);
   if (OPTd) 	conc(Call("DVM_ETASK", DVMx1(loopid)));

/*    End of synchronous reduction                          */

   if (red_vars && red_group==0) 	
      {
      if (OPTs==0 || OPTd) 	conc(Call("\n DVM_END_REDUCTION", DVMx2(Lxn(OPTs), Lxn(OPTd))));
      }

/*    Return compound statement                             */

   return Lbs();
err:
   E(2063);
   return N;
   }

/* Task-loop: DVM(PARALLEL [var] ON task[var]) DO(var)...   */

int PTaskLoop(int N)
   {
   int oper=A(N);
   int dir=A(B(N));
   int loopid=D(dir);
   int vars=Subscrs(A(A(dir)));	/* loop variables */
   int exs=Subscrs(B(A(dir)));
   int task=B(A(dir));	/* target */
   int body=A(B(A(oper)));	/* loop body */

/*    1-D loop with one-to-one mapping                      */

   if (vars!=exs || C(vars)!=LXI) 	
      {
      E(2081);
      return 0;
      }

/*    A copy of loop header with modified body:             */
/*        DO(var...) if(DVM_RUN()) {... body;... }          */

   N=mkRUNAM(task, body);	/* build new body */
   N=NDD(0, N, 0, 0);
   N=NDD(XXoper, NDD(C(A(oper)), A(A(oper)), N, 0), 0, 0);
   return N;
   }

/*----------------------------------------------------------*/
/* Semantic functions:  TEMPLATE                            */
/*----------------------------------------------------------*/


/* Recursive part of AMaxis                                 */

int AMaxisR(int N, int no)
   {
   if (C(N)!=LBK) 	return 0;	/* stop recursion */
   else w=AMaxisR(A(N), no-1);	/* recursion */

/*    Make a list skipping non-distrubited dimensions       */

   return B(N)==0?w: w==0?Lxn(no): NDD(COMMA, w, Lxn(no), 0);
   }

/* Create a list of numbers of distributed dimensions       */

int AMaxis(int N)
   {
   skipA(LBK, N);
   return AMaxisR(N, Count);
   }

/* Build DVM_CREATE_TEMPLATE macro                          */

int mk_templ(int amv, int dims, int dir)
   {
   int axis, ps, axno;

/*    Nothing to do for sequential program                  */

   if (OPTs) 	
      {
      conc(NoOper());
      return 0;
      }
   ASSERTC(DISTRIBUTE, dir, mk_templ);
   axis=AMaxis(toA(LBK, dir));	/* dimensions list */
   conc(Call("DVM_CREATE_TEMPLATE", DVMx4(Lxn(0), amv, Lxn(Len(COMMA, dims)), Lba(dims))));
   axno=Len(COMMA, axis);
   ps=B(A(A(dir)));	/* the ONTO-clause (or 0) */
   if (ps==0) 	ps=null();	/* default PS */
   else
      {
      ASSERTC(ONTO, ps, mk_templ2);
      w=get(ONTO, ps);	/* ONTO implementation */
      if (w && B(w)<axno) 	E(2031);
      w=A(w);	/* DVM_ONTO macro for PS section creation */
      if (w) 	
         {
         conc(w);
         ps=LxiS("DVM_PS");
         }
      else ps=A(ps);	/* PS as a whole */
      }

/*    Prepare the PS according to GENBLOCK format           */

   mk_genblock(Lxn(0), ps);
   mk_multblock(amv);

/*    Distribute the template over the PS                   */

   conc(Call("DVM_DISTRIBUTE", DVMx4(amv, ps, Lxn(axno), Lba(axis))));
   return 1;
   }

/* Verify the rank of a "static" template                   */

void wfTEMPLATE()
   {
   if (C(A(N))==LBK) 	
      {
      w=DRank(to(DISTRIBUTE));
      skipA(LBK, A(N));
      if (w && w!=Count) 	E(2020);
      }
   }

/* Verify and convert CREATE_TEMPLATE directive             */

void wfLX_CRTEMP()
   {
   int stuff=A(N);
   whatis(stuff);	/* analyse declaration */
   if (cDIR!=DISTRIBUTE || C(B(cDir))!=TEMPLATE || C(A(B(cDir)))!=DVMbase) 	E(2037);
   skipA(LBK, stuff);	/* the number of subscripts */
   if (dRank && dRank!=Count) 	E(2020);

/*    Build macros and store new subtree                    */

   mk_templ(cID, Subscrs(stuff), cDir);
   w=Lbs();
   NEWop(OPTs, w);
   }

/*----------------------------------------------------------*/
/* Semantic functions:  ALIGN                               */
/*----------------------------------------------------------*/


/* Make a list of var-s, coeff-s or const-s                 */

int mk_vab(int m, int exs, int vars)
   {
   int exa[MAXrank], eno;	/* array of align expressions */
   int va[MAXrank], vno;	/* array of variables */
   int ret;
   int i;
   int e, v=0, a, b;
   int N=0;

/*    unpack subtrees to arrays                             */

   eno=unpackto(exs, exa);
   vno=unpackto(vars, va);

/*    loop over expressions                                 */

   for (i=0; i<(eno); i++) 	
      {
      e=exa[i];
      v=0;

/*        [] -> (-1,0,0)                                    */

      if (e==0) 	
         {
         N=Comma1(N, m==0?mk(SUB, 0, Lxn(1)): Lxn(0));
         continue ;
         }

/*        [v*a+b]                                           */

      if (C(e)==ADD) 	
         {
         b=B(e);
         e=A(e);
         }
      else
      if (C(e)==SUB && A(e)) 	
         {
         b=NDD(SUB, 0, B(e), 0);
         e=A(e);
         }
      else
         {
         b=Lxn(0);
         }
      if (C(e)==SUB) 	
         {
         e=B(e);
         ret=SUB;
         }
      else ret=ADD;
      if (C(e)==MUL) 	
         {
         a=A(e);
         e=B(e);
         }
      else a=Lxn(1);

/*        look for 'v' in the variables list                */

      v=0;
      if (C(e)==LXI) 	
         {
         for (j=0; j<(vno); j++) 	
         if (va[j] && A(e)==A(va[j])) 	
            {
            v=Lxn(j+1);	/* variable's number */
            break ;
            }
         }

/*        it is not an align variable                       */

      if (v==0) 	
         {
         v=a=Lxn(0);
         b=exa[i];
         }
      N=Comma1(N, m==0?v: m==1?(ret==SUB?NDD(SUB, 0, a, 0): a): b);
      }
   return Lba(N);
   }

/*  Build a list of align parameters                        */

int Align(int m, int exs, int vars)
   {
   if (vars>100 || vars==0) 	return mk_vab(m, exs, vars);
   else
      {
      int N=0;

/*    default: v=(1,2,3...),a=(1,1,...),b=(0,0,...)         */

      for (i=0; i<(vars); i++) 	N=Comma1(N, Lxn(m==0?i+1: m==1?1: 0));
      return Lba(N);
      }
   }
void wfALIGN()
   {
   }

/* Verify and convert REALIGN directive                     */

void wfREALIGN()
   {
   int stuff, exs, vars, rnew;
   stuff=A(A(N));
   exs=Subscrs(B(stuff));	/* align expressions */
   vars=Subscrs(A(stuff));	/* align variables */
   whatis(A(stuff));	/* analyse array declaration */
   if (cDVM==0) 	
      {
      E(2039);	/* non-DVM */
      return ;
      }
   rnew=(B(A(N))!=0);
   NEWop(OPTs, Call("DVM_REALIGN", Comma(8, cline(), A(skipA(DVMind, A(stuff))), A(skipA(DVMbind, B(stuff))),
   Lxn(Len(COMMA, exs)), Align(0, exs, vars), Align(1, exs, vars),
   Align(2, exs, vars), Lxn(rnew))));
   }

/* Verify the base of ALIGN directive                       */

void wfDVMalign()
   {
   if (B(N)==0) 	return ;	/* delayed */
   whatis(B(N));	/* analyse the declaration */
   if (cDir==0 || dRank<=0) 	return ;
   if (dRank!=uRank) 	E(2020);
   }

/* Verify ALIGN or PARALLEL variable                        */

void wfDVMind()
   {
   int token;
   if (B(N)==0) 	return ;	/* delayed */
   token=toA(TOKEN, B(N));

/*    must not coinside with any previous                   */

   if (token) 	
   for (Count=0, w=A(N); C(w)==DVMind; Count++, w=A(w)) 	
      {
      if (B(w) && token==toA(TOKEN, B(w))) 	E(2047);
      }
   if (to(PARALLEL)==0) 	return ;

/*    for PARALLEL some more tests                          */

   N=B(N);	/* must present */
   if (N==0) 	
      {
      E(2048);
      return ;
      }
   N=Decl(N);	/* must be defined */
   if (N<=0) 	
      {
      E(2040);
      return ;
      }
   w=intVarTp(N);	/* must be a simple integer variable */
   if (C(D(N))!=XXdcltr) 	E(2049);
   }

/* Verify ALIGN (or PARALLEL) expression                    */

void wfDVMbind()
   {
   int var=IndVar(B(N));	/* extract variable */
   if (var==0 || C(var)==LXN || C(var)==LBA) 	return ;
   if (C(var)!=LXI) 	
      {
      E(2050);
      return ;
      }
   w=A(up(1));

/*    Look for it in the variables list                     */

   for (Count=0, w=w; C(w)==DVMind && A(var)!=A(B(w)); Count++, w=A(w)) 	
      {
      ;
      }
   if (C(w)!=DVMind) 	
      {
      E(2077);
      return ;
      }

/*    May be used only once                                 */

   for (Count=0, w=A(N); C(w)==DVMbind; Count++, w=A(w)) 	
      {
      if (A(IndVar(B(w)))==A(var)) 	E(2051);
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions:  PARALLEL loop                       */
/*----------------------------------------------------------*/


/* Verify DO (FOR) variable                                 */

void wfDVMvar()
   {
   int v, depth=0, token=toA(TOKEN, N);

/*    Find enclosing loop                                   */

   for (w=B(Stack); ; w=B(B(B(w))), depth++) 	
      {
      i=C(B(w));
      if (i!=LX_FOR && i!=LX_DO) 	break ;
      }

/*    If directive exists and it is the PARALLEL directive  */

   v=A(B(A(w)));
   if (C(v)!=PARALLEL) 	return ;
   depth-=DRank(v);
   if (depth>0) 	return ;

/*    Find the variable in the loop variables list          */

   for (Count=0, v=toA(DVMind, v); C(v)==DVMind && token!=A(B(v)); Count++, v=A(v)) 	
      {
      depth++;
      }
   if (C(v)!=DVMind) 	E(2045);	/* not found */
   else
   if (depth!=0) 	E(2046);	/* misplaced */
   }

/* INQUIRY: is it a 'red-black' loop? ie. DO(v,a+b%2,c,2)   */

int ISredblack(int N)
   {
   if (C(N)==LX_DO) 	
      {
      w=B(A(N));
      if (A(B(w))==TWO) 	
         {
         w=A(A(w));
         if (C(w)==ADD) 	w=B(w);
         if (C(w)==MOD && A(B(w))==TWO) 	return 1;
         }
      }
   return 0;
   }

/* Build lists of variables, bounds or steps                */

int cRANGE(int vlhs, int rank, int N)
   {
   int R=0, p;

/*    get a list of variables from the directive            */

   if (vlhs==0) 	
      {
      ASSERTC(PARALLEL, N, cRANGE);
      N=toA(DVMind, N);
      for (; rank; rank--) 	
         {
         ASSERTC(DVMind, N, cRANGE2);
         conc(AddrAsLong(B(N)));
         N=A(N);
         }
      return BrComma();
      }

/*    get a list of variable's types                        */

   if (vlhs==-1) 	
      {
      ASSERTC(PARALLEL, N, cRANGE);
      N=toA(DVMind, N);
      for (; rank; rank--) 	
         {
         ASSERTC(DVMind, N, cRANGE2);
         conc(Lxn(intVarTp(B(N))));
         N=A(N);
         }
      return BrComma();
      }

/*    build other lists by the headers of loopp nest        */

   for (; rank; rank--) 	
      {
      if (C(N)==XXoper) 	N=A(N);
      p=B(A(N));

/*    FOR(v,N) -> (0, N-1, 1)                               */

      if (C(N)==LX_FOR) 	
         {
         switch (vlhs)
            {
         case 1: R=Comma1(R, Lxn(0));
            break ;
         case 2: R=Comma1(R, NDD(SUB, p, Lxn(1), 0));
            break ;
         case 3: R=Comma1(R, Lxn(1));
            break ;
            }
         }
      else
      if (C(N)==LX_DO) 	
         {

/*    DO(v,L,H,S) -> (L,H,S)                                */

         if (!ISredblack(N)) 	
            {
            switch (vlhs)
               {
            case 1: R=Comma1(R, A(A(p)));
               break ;
            case 2: R=Comma1(R, B(A(p)));
               break ;
            case 3: R=Comma1(R, B(p));
               break ;
               }
            }
         else

/*    'Red-black':  DO(v,[a+]b%2,c,2) -> (a|0 ,c,1)         */

            {
            switch (vlhs)
               {
            case 1: p=A(A(p));
               R=Comma1(R, C(p)==ADD?A(p): Lxn(0));
               break ;
            case 2: R=Comma1(R, B(A(p)));
               break ;
            case 3: R=Comma1(R, Lxn(1));
               break ;
               }
            }
         }
      else ASSERT(0, cRANGE);
      N=A(B(N));
      }
   return Lba(R);
   }

/* Build a PARALLEL loop                                    */

int Ploop(int N)
   {
   int oper=A(N);
   int dir=A(B(N));
   int subdirs=B(A(B(N)));	/* subdirectives */
   int loopid=D(dir);
   int vars=Subscrs(A(A(dir)));	/* loop variables */
   int rank=Len(COMMA, vars);	/* loop rank */
   int exs=Subscrs(B(A(dir)));	/* align expressions */
   int rb=Len(COMMA, exs);	/* target rank */
   int base=toA(LXI, B(A(dir)));	/* base */
   int reduction=0;	/* for REDUCTION subdirective */
   int red_group=0, red_vars=0;
   int remote=0;	/* for REMOTE_ACCESS subdirective */
   int across=0;	/* for ACROSS sucdirective */
   int pipe=0;	/* for PIPE-subdirective */
   int vs, ls, hs, ss;	/* variables, bounds, and steps */
   ASSERTC(PARALLEL, dir, Ploop);

/*    Subdirectives                                         */

   for (; subdirs; subdirs=B(subdirs)) 	
      {
      if (C(subdirs)==LXX) 	w=A(subdirs);
      else
         {
         w=subdirs;
         subdirs=0;
         }
      if (C(w)==REMOTE) 	remote=w;
      else
      if (C(w)==REDUCTION) 	
         {
         red_group=A(A(w));
         red_vars=get(NEW, w);
         }
      else
      if (C(w)==ACROSS) 	across=w;
      else
      if (C(w)==PIPE) 	pipe=w;
      else conc(get(NEW, w));
      }
   subdirs=tmpList?Lbs(): 0;
   vs=cRANGE(0, rank, dir);
   if (OPTd==0) 	
      {
      vs=0;
      for (i=0; i<(rank); i++) 	vs=Comma1(vs, Lxn(0));
      vs=Lba(vs);
      }
   ls=cRANGE(1, rank, oper);
   hs=cRANGE(2, rank, oper);
   ss=cRANGE(3, rank, oper);

/*    debugger shell: {DVM_PLOOP; loop; DVM_ENDLOOP}        */

   if (OPTd) 	conc(Call("DVM_PLOOP", Comma(6, cline(), loopid, Lxn(rank), ls, hs, ss)));
   if (OPTs) 	conc(oper);
   else conc(NDD(XXoper, NDD(DVM_DOPL, Lba(loopid), oper, 0), 0, 0));
   if (OPTd) 	conc(Call("DVM_ENDLOOP", DVMx1(loopid)));
   oper=Lbs();

/*    the REMOTE_ACCESS shell (define and load buffers      */

   if (remote && OPTs==0) 	oper=mk_remote(remote, oper);

/*    the ACROSS shell                                      */

   if (across && OPTs==0) 	
      {
      conc(get(NEW, across));
      conc(oper);
      oper=Lbs();
      }

/*    the PIPE shell                                        */

   if (pipe && (w=get(NEW, pipe))!=0 && OPTs==0) 	
      {
      conc(w);
      conc(oper);
      conc(Call("\n DVM_END_PIPE", cline()));
      oper=Lbs();
      }

/*    synhronous reduction shell                            */

   if (red_vars && red_group==0) 	
      {
      if (OPTs==0 || OPTd) 	conc(Call("\n DVM_REDUCTION", DVMx4(OPTs?Lxn(0): loopid, red_vars, Lxn(OPTs), Lxn(OPTd))));
      conc(oper);
      if (OPTs==0 || OPTd) 	conc(Call("\n DVM_END_REDUCTION", DVMx2(Lxn(OPTs), Lxn(OPTd))));
      oper=Lbs();
      }

/*    the main sequence                                     */


/*        create reduction group                            */

   if (red_vars && red_group) 	conc(Call("\n DVM_CREATE_RG", DVMx4(red_group, red_vars, Lxn(OPTs), Lxn(OPTd))));

/*        create loop header                                */

   conc(OPTs?NoOper(): Call("DVM_PARALLEL", DVMx2(loopid, Lxn(rank))));

/*        insert subdirectives (SHADOW)                     */

   if (subdirs) 	conc(subdirs);
      {
      int i=A(B(B(Decl(base))));
      if (C(i)==DISTRIBUTE && C(B(i))==TEMPLATE) 	base=NDD(ADDR, 0, base, 0);
      }

/*        map the parallel loop                             */

   conc(OPTs?NoOper(): Call("DVM_DO_ON", Comma(12, cline(), loopid, Lxn(rank), vs, ls, hs,
   ss, base, Lxn(rb), mk_vab(0, exs, vars), mk_vab(1, exs,
   vars), mk_vab(2, exs, vars))));

/*        fill reduction group                              */

   if (red_vars && red_group) 	conc(Call("\n DVM_REDUCTION20", DVMx4(OPTs?Lxn(0): loopid, red_group, Lxn(OPTs), Lxn(OPTd))));

/*        (modified) loop nest                              */

   conc(oper);

/*        close the loop                                    */

   conc(OPTs?NoOper(): Call("DVM_END_PARALLEL", DVMx1(loopid)));

/*        build compound statement                          */

   N=Lbs();

/*    PPPA shell: {DVM_BPLOOP; loop; DVM_ENLOOP;}            */

   if (OPTe!=0 && OPTe!=2) 	
      {
      conc(Call("DVM_BPLOOP", DVMx1(loopid)));
      conc(N);
      conc(Call("DVM_ENLOOP", DVMx1(loopid)));
      N=Lbs();
      }
   return N;
   }

/* Loop headers generation                                  */

void wfLX_FOR()
   {
   int lvl, ploop=0, dir, rank;
   w=Stack;
   if (C(N)==XXoper) 	w=B(B(w));

/*    Search for an enclosing PARALLEL directive            */

   for (lvl=0; C(w)==LX_DO || C(w)==LX_FOR; lvl++) 	
      {
      w=B(B(w));
      dir=A(B(A(w)));
      if (dir && C(dir)==PARALLEL) 	
         {
         ploop=A(w);
         break ;
         }
      w=B(w);
      }

/*    If a parallel directive was found                     */

   if (ploop!=0) 	
      {
      if (C(N)==XXoper) 	
         {
         rank=Rank(A(A(dir)));	/* loop rank (no. of variables) */
         if (rank==lvl+1) 	
            {
            if (OPTd) 	
               {
               int vars=cRANGE(0, rank, dir);	/* an iteration variables */
               int tps=cRANGE(-1, rank, dir);	/* variables' types */
               w=Call("DVM_ITER", DVMx3(Lxn(rank), vars, tps));
               }
            else w=NoOper();	/* or else an empty operator */
            w=Lbs2(w, NDD(XXoper, A(N), 0, 0));	/* build compund statement */
            set(NEW, N, w);	/* store a NEW node */
            }
         }
      else
      if (OPTs==0) 	
         {
         int loopid=D(dir);
         int parm=A(N);
         int src=Fun(NDD(LXI, C(N), 0, 0), Lop(COMMA, A(A(parm)), B(parm)));
         int op;

/*    New header: DVM_FOR or DVM_RB (Red-black)             */

         op=ISredblack(N)?DVM_RB: DVM_FOR;
         if (op==DVM_FOR) 	
            {
            if (OPTd==0) 	
            if (C(N)==LX_FOR || C(N)==LX_DO && A(B(B(A(N))))==ONE) 	op=DVM_FOR_1;
            parm=Comma(5, cline(), loopid, A(A(parm)), Lxn(lvl), src);
            }
         else
            {
            int e=A(A(B(parm)));
            e=C(e)==ADD?NDD(ADD, A(e), A(B(e)), 0): A(e);
            parm=Comma(6, cline(), loopid, A(A(parm)), Lxn(lvl), e, src);
            }
         w=NDD(op, Lba(parm), A(B(N)), 0);
         if (0) 	
            {
            for (Count=0, substDAlist=substDAlist; substDAlist; Count++, substDAlist=A(substDAlist)) 	
               {
               char daH[]="\n DVM_daH6";
               if (C(substDAlist)) 	
                  {
                  int arr=B(substDAlist);
                  int arrdim=6;
                  arrdim=C(substDAlist);
                  daH[9]='0'+arrdim;
                  conc(Call(daH, arr));
                  }
               }
            }
         conc(NDD(XXoper, w, 0, 0));
         w=A(Lbs());
         set(NEW, N, w);
         }
      }

/*    else it is not a parallel loop nest                   */

   else

/* Sequential loop tracing for the PPPA and the Debugger    */

   if (C(N)!=XXoper && (OPTe && OPTe!=2 || OPTd)) 	
      {
      int loopid=Lxn(++LoopNo);	/* increment loop number */
      int p1=DVMx1(loopid);
      int p2=DVMx1(loopid);

/*    PPPA: -e4, or 'sequential over a parallel'            */

      if (OPTe==4 || over(PARALLEL) || over(TASKLOOP)) 	
         {
         conc(Call("DVM_BSLOOP", p1));	/* trace loop start */
         conc(up(1));	/* loop body */
         conc(Call("DVM_ENLOOP", p2));	/* trace loop end */
         NEWop(0, Lbs());	/* build and store compound statement */
         }

/*    DEBUGGER: always trace loop and every iteration       */

      if (OPTd) 	
         {
         int var=A(A(A(N)));
         int tp=intVarTp(var);
         tp=Lba(Lxn(tp));
         w=Lba(AddrAsLong(var));
         w=Lbs2(Call("DVM_ITER", DVMx3(Lxn(1), w, tp)), (A(B(N))));
         set(NEW, B(N), NDD(0, w, 0, 0));	/* loop body with iteration trace */
         conc(Call("DVM_SLOOP", p1));	/* trace loop start */
         conc(NDD(XXoper, N, 0, 0));	/* NEW loop body */
         conc(Call("DVM_ENDLOOP", p2));	/* trace loop end */
         set(NEW, up(1), Lbs());	/* build and store compound statement */
         }
      }
   }

/*----------------------------------------------------------*/
/* Semantic functions: REDUCTION                            */
/*----------------------------------------------------------*/


/* Create descriprion of a reduction operation              */

int cRG0(int r)
   {
   int rf, re, rt, size, len=0;
   int loopid=Lxn(0);
   rf=rts_name(C(r));	/* RTS-name of reducrion operation */
   re=A(r);
   if (C(re)==LXfun) 	
      {
      len=B(re);
      re=A(re);
      re=NDD(LBK, re, Lxn(0), 0);
      }
   whatis(re);	/* analyse red.var. declaration */
   if (cDecl==0) 	
      {
      E(2040);
      return 0;
      }
   rt=rts_name(cRTStype);	/* RTS code of its type */

/*    size = 1 or sizeof(v):sizeof(*v)                      */

   if (cLBKl!=0 || cASTER!=0 || cLBKr!=0 && cLBKr!=1) 	E(2071);	/* simple or 1-D array */
   if (cLBKr==0 && uRank!=0) 	E(2074);
   if (len!=0) 	size=len;
   else
   if (cLBKr==0 || uRank!=0) 	size=Lxn(1);	/* simple or element */
   else size=NDD(DIV, Fun(Lxi(SIZEOF), cID), Fun(Lxi(SIZEOF), NDD(CONT, 0, cID, 0)),
   0);	/* array */

/*    Create DVM_RVAR or DVM_RLOC macro                     */

   if (B(r)==0) 	r=Fun(LxiS("\n    DVM_RVAR"), Comma(6, rf, re, rt, size, Lxn(OPTs), Lxn(OPTd)));
   else
      {
      int loctp=intVarTp(B(r));
      w=B(r);
      r=Fun(LxiS("\n    DVM_RLOC"), Comma(8, rf, re, rt, size, w, Lxn(loctp),
      Lxn(OPTs), Lxn(OPTd)));
      }
   return r;
   }

/* Recursion to transform a.[b.[c.d]] to ((a,b),c),d        */

int cRGR(int N, int lop)
   {
   w=cRG0(C(N)==LXX?A(N): N);	/* next element */
   if (lop) 	w=NDD(COMMA, lop, w, 0);	/* append to the 'lop' */
   return C(N)!=LXX?w: cRGR(B(N), w);	/* recursion */
   }

/* Biuld a bracketed list of reduction operations           */

int cRG(int Ad, int N)
   {
   int vars=Lba(cRGR(N, 0));
   return vars;
   }

/* REDUCTION subdirective to list of red. operations        */

void wfREDUCTION()
   {
   set(NEW, N, cRG(A(A(N)), B(N)));
   }

/* REDUCTION_START directive to macro                       */

void wfRSTART()
   {
   NEWop(OPTs && !OPTd, Call("DVM_REDUCTION_START", DVMx3(toA(LXI, N), Lxn(OPTs), Lxn(OPTd))));
   }

/* REDUCTION_WAIT directive to macro                        */

void wfRWAIT()
   {
   NEWop(OPTs && !OPTd, Call("DVM_REDUCTION_WAIT", DVMx3(toA(LXI, N), Lxn(OPTs), Lxn(OPTd))));
   }

/*----------------------------------------------------------*/
/* Semantic functions:  SHADOW and ACROSS                   */
/*----------------------------------------------------------*/

typedef struct
   {
   int lw;
   int hw;
   }
tWidth;

/* Returns provided shadow width or {-1,-1}                 */

tWidth Width(int N)
   {
   tWidth w;
   if (N==0) 	
      {
      w.lw=-1;
      w.hw=-1;
      }
   else
      {
      w.lw=atoi(LXW(A(A(N))));
      w.hw=A(B(N))?atoi(LXW(A(B(N)))): w.lw;
      }
   return w;
   }

/* Active width.                                            */

int cmpWidth(int here, int def)
   {
   if (def==-1) 	def=1;	/* default maximum =1 */
   if (here==-1) 	return def;	/* get default */
   else
   if (here<=def) 	return here;	/* get required */
   else return -1;	/* error: required > exist */
   }

/* Build a list of low or high widths                       */

int cSHw(int what, int shadows, int rank)
   {
   int i;
   int def=A(shadows);
   int savetmp=tmpList;
   tmpList=0;
   for (i=0; i<(rank); i++) 	
      {
      if (shadows==0) 	conc(Lxn(1));	/* default for all */
      else
         {
         tWidth d;
         d=Width(C(def)==DVMshw?B(def): 0);
         conc(Lxn(cmpWidth(-1, what==0?d.lw: d.hw)));
         def=C(def)==DVMshw?A(def): 0;
         }
      }
   w=BrComma();
   tmpList=savetmp;
   return w;
   }

/* Create DVM_SHADOWS | DVM_ACROSS macro                    */

int inAcross=0;	/* ACROSS flag to select a macro */
int cSG0(int N)
   {
   int i;
   int array;	/* 1 -- is an array, so use DVM_SHADOWSa */
   int herew=A(N);	/* widths in the directive */
   int var=toA(LXI, herew);	/* array */
   int shrank=Count;
   int corner=Lxn(B(N)!=0);	/* 'CORNER' */
   int dir=DVMdir(var);
   int defw=A(B(dir));	/* widths in the declaration */
   int rank;
   int here, def;
   int lwds, hwds;
   if (dir==0) 	return 0;
   rank=DRank(dir);
   if (rank==0) 	rank=shrank;

/*    evaluate actual widths                                */

   here=herew;
   def=defw;
   for (i=0; i<(rank); i++) 	
      {
      tWidth h, d;
      h=Width(C(here)==DVMshw?B(here): 0);	/* required */
      d=Width(C(def)==DVMshw?B(def): 0);	/* declared */
      conc(Lxn(cmpWidth(h.lw, d.lw)));
      here=C(here)==DVMshw?A(here): 0;
      def=C(def)==DVMshw?A(def): 0;
      }
   lwds=BrComma();	/* low widths */
   here=herew;
   def=defw;
   for (i=0; i<(rank); i++) 	
      {
      tWidth h, d;
      h=Width(C(here)==DVMshw?B(here): 0);
      d=Width(C(def)==DVMshw?B(def): 0);
      conc(Lxn(cmpWidth(h.hw, d.hw)));
      here=C(here)==DVMshw?A(here): 0;
      def=C(def)==DVMshw?A(def): 0;
      }
   hwds=BrComma();	/* hogh widths */

/*    single array or array of rointers?                    */

   array=0;
   w=D(Decl(var));
   if (C(w)==LBK) 	
      {
      for (; C(w)==LBK; w=D(w)) 	;
      if (C(w)!=XXdcltr) 	array=1;
      }

/*    build macro                                           */

   if (array) 	
      {
      int n=A(N);
      n=B(n)==0?Lxn(0): B(n);
      N=Fun(inAcross?LxiS("\n    DVM_ACROSS_SHa"): LxiS("\n    DVM_SHADOWSa"), Comma(6, var, Lxn(rank), lwds, hwds, corner, n));
      }
   else
      {
      N=Fun(inAcross?LxiS("\n    DVM_ACROSS_SH"): LxiS("\n    DVM_SHADOWS"), Comma(5, var, Lxn(rank), lwds, hwds, corner));
      }
   return N;
   }

/* Comma-list of macros for all renewees                    */

int cSG(int N, int lop)
   {
   w=cSG0(C(N)==LXX?A(N): N);	/* next renewee */
   if (lop) 	w=NDD(COMMA, lop, w, 0);	/* append to the 'lop' */
   return C(N)!=LXX?w: cSG(B(N), w);	/* recursion */
   }

/* Verify shadow widths list                                */

void wfSHADOW()
   {
   w=to(DISTRIBUTE);
   if (w==0) 	w=to(ALIGN);
   ASSERT(w, ISWF.SHAD);
   w=DRank(w);	/* rank of array */
   skipA(DVMshw, A(N));	/* the number of widths */
   if (w && w>Count) 	E(2020);
   }

/* Verify shadow width                                      */

void wfDVMshw()
   {
   tWidth def, here;
   int d, i, j;

/*    determine enclosing directive                         */

   if (to(DVMshad)) 	
      {
      d=DVMdir(toA(LXI, A(N)));	/* declaration */
      if (d==0) 	
         {
         return ;
         }
      }
   else
   if ((d=to(DISTRIBUTE))==0) 	d=to(ALIGN);
   ASSERT(C(d)==DISTRIBUTE || C(d)==ALIGN, ISWF.DVMshw);

/*                                                          */

   i=DRank(d);
   if (i==0 && C(B(d))==SHADOW) 	
      {
      skipA(DVMshw, A(B(d)));
      i=Count;
      }
   skipA(DVMshw, N);
   j=Count;
   if (i && i<j) 	
      {
      E(2020);
      return ;
      }
   here=Width(B(N));
   if (C(B(d))==SHADOW) 	
      {
      for (Count=0, w=A(B(d)); i--!=j; Count++, w=A(w)) 	
         {
         ;
         }
      def=Width(B(w));
      }
   else def=Width(0);
   if (to(DVMshad)) 	
      {
      if (cmpWidth(here.lw, def.lw)<0 || cmpWidth(here.hw, def.hw)<0) 	E(2052);
      }
   }

/* Verify renewee                                           */

void wfDVMshad()
   {
   w=defined(toA(LXI, A(N)));
   if (w==0) 	return ;
   w=DRank(w);	/* declared rank */
   if (w && C(A(N))==DVMshw) 	
      {
      if (w>Rank(A(N))) 	E(2020);
      }
   if (B(N) && w==1) 	E(2053);
   }

/* CREATE_SHADOW_GROUP directive to a macro                 */

void wfLX_CRSG()
   {
   inAcross=0;
   NEWop(OPTs, Call("DVM_CREATE_SHADOW_GROUP", DVMx2(toA(LXI, N), Lba(cSG(B(N), 0)))));
   }

/* SHADOW_RENEW subdirective to a macro                     */

void wfSHRENEW()
   {
   w=to(PARALLEL);
   inAcross=0;
      {
      int loopid=D(w);
      set(NEW, N, OPTs?NoOper(): Call("DVM_SHADOW_RENEW", Comma(2, loopid, Lba(cSG(B(N), 0)))));
      }
   }

/* SHADOW_START directive or subdirective to a macro        */

void wfSHSTART()
   {
   if ((w=to(PARALLEL))!=0) 	
      {
      int loopid=D(w);
      set(NEW, N, OPTs?NoOper(): Call("DVM_PAR_SHADOW_START", Comma(2, loopid, A(N))));
      }
   else NEWop(OPTs, Call("DVM_SHADOW_START", DVMx1(toA(LXI, N))));
   }

/* SHADOW_WAIT directive or subdirective to a macro         */

void wfSHWAIT()
   {
   if ((w=to(PARALLEL))!=0) 	
      {
      int loopid=D(w);
      set(NEW, N, OPTs?NoOper(): Call("DVM_PAR_SHADOW_WAIT", Comma(2, loopid, A(N))));
      }
   else NEWop(OPTs, Call("DVM_SHADOW_WAIT", DVMx1(toA(LXI, N))));
   }

/* ACROSS subdirective to a macro                           */

void wfACROSS()
   {
   int loopid;
   if (OPTs) 	return ;
   w=to(PARALLEL);
   loopid=D(w);
   inAcross=1;
   set(NEW, N, 	/*OPTs? NoOper():*/
   Call("DVM_ACROSS_IN", DVMx2(loopid, Lba(cSG(B(N), 0)))));
   inAcross=0;
   }

/* PIPE subdirective to a macro                             */

void wfPIPE()
   {
   int m=A(N);
   int rt;
   int loopid;
   int n=B(N);
   if (OPTs) 	return ;
   w=to(PARALLEL);
   loopid=D(w);
   whatis(m);
   if (cDecl==0) 	
      {
      E(2040);
      return ;
      }
   if (cDVM!=0) 	
      {
      E(2000);
      return ;
      }
   if (n==0) 	
      {
      n=A(cDcltr);
      if (C(n)!=LBK) 	
         {
         E(2000);
         return ;
         }
      n=B(n);
      }
   rt=rts_name(cRTStype);	/* RTS code of its type */
   set(NEW, N, Call("DVM_PIPE", DVMx4(loopid, m, rt, n)));
   return ;
   }

/*----------------------------------------------------------*/
/* Semantic functions:  array copy                          */
/*----------------------------------------------------------*/


/* unpack source|target of COPY                             */

void copystuff(int e, int *a, int *r, int *exs)
   {
   *r=0;
   *exs=0;
   if (C(e)==LBK) 	
      {
      *r=0;
      *exs=0;
      for (; C(e)==LBK; e=A(e), (*r)++) 	
         {
         w=B(e);
         *exs=*exs?Lop(COMMA, w, *exs): w;
         }
      *a=e;
      }
   else
   if (C(e)==LXfun) 	
      {
      *a=A(e);
      *exs=B(e);
      *r=Len(COMMA, *exs);
      }
   else E(2091);
   }

/* COPY | COPY_START loop to a macro                        */

int wfCOPY(int n)
   {
   int flag;	/* The copy flag if exists */
   int loop, rank;	/* copy loop */
   int v, f, l, s;	/* the next header parameters */
   int fs=0, ls=0, ss=0;	/* the loop parameters */
   int body, lhs, rhs;
   int fa, fr, fes;	/* From array, rank and subscripts */
   int ta, tr, tes;	/* To array, rank and subscripts */
   flag=A(A(B(n)));
   loop=A(A(n));
   for (rank=0; ; rank++) 	
      {
      if (C(loop)==LX_FOR) 	
         {
         w=A(loop);
         v=A(A(w));
         f=Lxn(0);
         l=NDD(SUB, B(w), Lxn(1), 0);
         s=Lxn(1);
         }
      else
      if (C(loop)==LX_DO) 	
         {
         w=A(loop);
         v=A(A(w));
         w=B(w);
         f=A(A(w));
         l=B(A(w));
         s=B(w);
         }
      else break ;
      w=NDD(ASSIGN, v, f, 0);
      fs=fs?NDD(COMMA, fs, w, 0): w;
      w=NDD(ASSIGN, v, l, 0);
      ls=ls?NDD(COMMA, ls, w, 0): w;
      w=NDD(AADD, v, s, 0);
      ss=ss?NDD(COMMA, ss, w, 0): w;
      body=A(B(loop));
      loop=A(body);
      }
   if (rank==0) 	E(2090);
   while (C(A(body))==LBS) 	
      {
      int xxlist=A(A(body));
      if (B(xxlist)!=0) 	E(2090);
      body=A(A(xxlist));
      }
   if (C(A(body))!=XXexpr || C(A(A(body)))!=ASSIGN) 	E(2090);
   body=A(A(body));
   copystuff(A(body), &ta, &tr, &tes);	/* lhs */
   copystuff(B(body), &fa, &fr, &fes);	/* rhs */
   if (flag) 	w=Call("DVM_COPY_STARTr", Comma(12, cline(), Lxn(rank), Lba(fs), Lba(ls), Lba(ss), fa,
   Lxn(fr), Lba(fes), ta, Lxn(tr), Lba(tes), flag));
   else w=Call("DVM_COPYr", Comma(11, cline(), Lxn(rank), Lba(fs), Lba(ls), Lba(ss), fa,
   Lxn(fr), Lba(fes), ta, Lxn(tr), Lba(tes)));
   return w;
   }

/* COPY_WAIT directive to macro                             */

void wfCPWAIT()
   {
   NEWop(OPTs, Call("DVM_COPY_WAIT", DVMx1(A(N))));
   }

/*----------------------------------------------------------*/
/* Semantic functions:  miscallaneous                       */
/*----------------------------------------------------------*/


/* INQUIRY: is it "void *" -- the type of DVM-objects       */

int isVoidPtr(int lbk)
   {
   if ((lbk==-1 && cLBKl>0 || lbk==-2 || lbk==cLBKl) && cASTER==1 && cLBKr==0 && cType==VOID_) 	return 1;
   if (lbk==0 || lbk==-2) 	E(2013);
   else E(2055);
   return 0;
   }

/* Verify the declarator of DVM array or object             */

void ISWFdcltr(int N, int spec, int dvm)
   {
   int dir=C(A(dvm));	/* directive */
   int rank;
   int id;
   id=arrDcltr(N);	/* identifier */
   cType=C(B(spec));	/* type */
   if (C(id)==LXfun) 	
      {
      if (dir!=DISTRIBUTE || B(dvm)==0) 	
         {
         E(2019);
         return ;
         }
      }
   else
   if (C(id)!=LXI) 	
      {
      E(2018);
      return ;
      }

/*    Depending on the code of directive                    */

   switch (dir)
      {
   case DISTRIBUTE:

/* A TEMPLATE must be 'void *'                              */
/* For a 'static' template make implicit CREATE_TAMPLATE    */

      if (C(B(A(dvm)))==TEMPLATE) 	
         {
         if (isVoidPtr(0)) 	
         if (C(A(B(A(dvm))))!=DVMbase && OPTs==0) 	
            {
            int dims=Subscrs(A(B(A(dvm))));
            mk_templ(A(A(N)), dims, A(dvm));
            w=Lbs();
            addIMloc(w, id);
            }
         return ;
         }

/*    ALIGN                                                 */

   case ALIGN: rank=DRank(A(dvm));	/* stuff rank */
      if (cType!=INT && cType!=LONG && cType!=FLOAT && cType!=DOUBLE) 	E(2014);	/* non-DVM type */
      else
      if (cLBKr+cASTER+cLBKl==0) 	E(2015);	/* scalar */
      else
      if (B(dvm) && cASTER==0) 	E(2016);	/* pointer */
      else
      if (B(dvm) && cLBKl>1) 	E(2017);	/* array of DVM-pointers */
      else
      if (cLBKr && rank && cLBKr+cASTER!=rank) 	E(2020);

/*    For 'static'                                          */

      if (cLBKl==0 && cASTER==0) 	
         {
         cDir=DVMdir(id);
         if (C(cDir)==ALIGN) 	
            {
            w=B(A(A(cDir)));
            if (w==0) 	
               {
               E(2076);
               return ;
               }
            w=Decl(toA(LXI, w));

/*    -- the base must be a known static                    */

            if (OPTs==0) 	
               {
               for (Count=0, i=SCloc; i; Count++, i=B(i)) 	
                  {
                  if (A(i)==A(w)) 	
                     {
                     i=D(i);
                     break ;
                     }
                  }
               if (i==0) 	
               for (Count=0, i=IMglobal; i; Count++, i=B(i)) 	
                  {
                  if (A(i)==A(w)) 	
                     {
                     break ;
                     }
                  }
               if (i==0) 	
                  {
                  E(2022);
                  return ;
                  }
               }
            }

/*    -- make implicit allocation                           */

         if (C(B(cDir))==TEMPLATE) 	
            {
            if (OPTs==0) 	
               {
               int dims=Subscrs(A(B(cDir)));
               mk_templ(id, dims, cDir);
               w=Lbs();
               addIMloc(w, id);
               }
            }
         else
            {
            int j;
            int stmt=NoOper();
            w=D(id);
            j=B(w);
            w=D(w);
            for (; C(w)==LBK; w=D(w)) 	
               {
               j=NDD(MUL, j, B(w), 0);
               }
            w=mk(0, 0, B(A(B(id))));
            w=NDD(SIZEOF, Lba(mk(XXtype, w, 0)), 0, 0);
            w=NDD(MUL, j, w, 0);
            w=mk_alloc(id, w, 0);
            addIMloc(w, id);
            }
         }
      break ;

/*    Groups must be declared as 'void *'                   */

   case LX_RG:
   case LX_SG:
   case LX_RMG:
   case LX_IG: isVoidPtr(0);
      return ;
   case LX_CP: isVoidPtr(-2);
      return ;

/*    PROCESSORS creation is always implicit                */

   case PROCESSORS:
      if (isVoidPtr(-1) && OPTs==0) 	
         {
         w=Lba(Subscrs(A(A(N))));
         w=Call("DVM_PROCESSORS", DVMx3(id, Lxn(cLBKl), w));
         addIMloc(w, id);
         }
      return ;

/*    TASK creation is always implicit                      */

   case TASK:
      if (isVoidPtr(1) && OPTs==0) 	
         {
         w=Call("DVM_TASK", Comma(2, id, B(A(A(N)))));
         addIMloc(w, id);
         }
      return ;
      }
   return ;
   }

/* Get the next declarator                                  */

int nextc(int i)
   {
   i=B(i);
   return C(i)==XXclist?A(i): i;
   }

/* Create prefixed name as string <prefix><id>              */

int pref_name(char *prefix, int n)
   {
   char id[128];
   sprintf(id, "%s%s", prefix, LXW(toA(TOKEN, n)));
   return LxiS(id);
   }

/* Create declarators list for 'AMV_<task>'                 */

int crAMVdcltrs(int N)
   {
   int id;
   id=toA(LXI, A(A(N)));	/* task idetifier */
   id=pref_name("AMV_", id);	/* AMV_<task> */
   if (C(B(N))==XXclist) 	w=NDD(XXclist, crAMVdcltrs(A(B(N))), 0, 0);	/* recursion */
   else w=B(N);	/* the end of list */
   return NDD(0, NDD(XXdcltr, id, 0, 0), w, 0);
   }

/* Create declarators list for 'DEB_<redgroup>'             */

int crDEBdcltrs(int N)
   {
   int init=mk(ASSIGN, Lxn(0), 0);	/* initialise as =0; */
   int id;
   id=toA(LXI, A(A(N)));	/* rg identifier */
   id=pref_name("DEB_", id);	/* DEB_<rg> */
   if (C(B(N))==XXclist) 	w=NDD(XXclist, crDEBdcltrs(A(B(N))), 0, 0);	/* recursion */
   else w=B(N);	/* the end of list */
   return NDD(0, NDD(XXdcltr, id, init, 0), w, 0);
   }

/* Modify declaration of DVM-object to '...Ref'             */

void crRef(int N, int type)
   {
   int init=mk(ASSIGN, Lxn(0), 0);	/* initialise as =0; */
   int newdcltr, w;
   int newtype=mk(TYPEDEF, 0, rts_name(type));	/* RTL-type */
   int decl=A(A(N));
   set(NEW, A(N), NDD(C(A(N)), decl, 0, 0));	/* erase the directive */
   if (OPTs && type!=LX_RG) 	return ;	/* sequential */
   set(NEW, A(decl), mk(0, 0, newtype));	/* new type */

/*    'AMViewRef AVM_<task>' for every task                 */

   if (type==TASK) 	
      {
      init=NDD(ASSIGN, NDD(LBS, Lxn(0), 0, 0), 0, 0);
      w=crAMVdcltrs(B(decl));
      w=NDD(XXdecl, NDD(0, 0, NDD(TYPEDEF, 0, LxiS("AMViewRef"), 0), 0),
      w, 0);
      w=NDD(XXlist, NDD(0, w, 0, 0), NDD(C(N), A(N), B(N),
      D(N)), 0);
      set(NEW, N, w);
      }

/*    'long RMG_<remote_group>' for every remote group      */

   if (type==LX_RMG) 	
      {
      w=crRMGdcltrs(B(decl));
      w=NDD(XXdecl, NDD(0, 0, NDD(LONG, 0, 0, 0), 0),
      w, 0);
      w=NDD(XXlist, NDD(0, w, 0, 0), NDD(C(N), A(N), B(N),
      D(N)), 0);
      set(NEW, N, w);
      }

/*    'ObjectRef DEB_<rg>' for every red group              */

   if (type==LX_RG) 	
      {
      int wt;
      w=crDEBdcltrs(B(decl));
      wt=NDD(TYPEDEF, 0, LxiS("ObjectRef"), 0);
      w=NDD(XXdecl, NDD(0, 0, wt, 0), w, 0);
      w=NDD(XXlist, NDD(0, w, 0, 0), NDD(C(N), A(N), B(N),
      D(N)), 0);
      set(NEW, N, w);
      }
   if (type==LX_CP) 	init=0;

/*    declarators list                                      */

   for (w=B(decl); w && C(w)!=SEMIC; w=nextc(w)) 	
      {
      newdcltr=A(A(A(w)));
      if (type==PROCESSORS) 	newdcltr=toA(LXI, newdcltr);	/* only id of processors */
      set(NEW, w, NDD(0, NDD(XXdcltr, newdcltr, init, 0), B(w),
      0));
      }
   }

/* Modify declaration of DVM-array to 'long ...[rank+1]'    */

void crHandler(int dd, int decl)
   {
   int newdcltr, w;
   int init;
   int isptr=B(B(dd));
   int dir=A(B(dd));
   int rank=DRank(dir);
   ASSERTC(XXdecl, decl, crHandler2);
   set(NEW, dd, NDD(C(dd), decl, 0, 0));	/* erase the directive */
   if (OPTs) 	return ;	/* nothing to do for sequential program */
   set(NEW, A(decl), mk(0, 0, NDD(LONG, 0, 0, 0)));	/* type */
   for (w=B(decl); w && C(w)!=SEMIC; w=nextc(w)) 	
      {
      newdcltr=A(w);	/* the next declarator */
      ASSERTC(XXdcltr, newdcltr, crHandler3);
      init=0;
      if (isptr==0) 	init=NDD(ASSIGN, NDD(LBS, Lxn(0), 0, 0), 0, 0);	/* initialise by 0's */
      newdcltr=skipA(LBK, A(newdcltr));	/* skip distributed dims */
      if (C(newdcltr)==LBA) 	newdcltr=A(newdcltr);

/*    If it is not a DVM-pointer, then build a handler      */

      if (isptr==0) 	
         {
         if (C(newdcltr)==LXaster) 	newdcltr=A(newdcltr);
         newdcltr=NDD(LBK, newdcltr, Lxn(rank?rank+1: 10), 0);
         }

/*    Store the new declarator                              */

      set(NEW, w, NDD(0, NDD(XXdcltr, newdcltr, init, 0), B(w),
      0));
      }
   }

/* Trace initialization and parameters                      */

void DTrace()
   {
   char *macro=0;
   if (OPTd==0) 	return ;	/* The Debugger is off */
   cType=rts_type(to(XXdecl));	/* check the type */
   if (cType==0) 	
      {
      E(2067);
      return ;
      }
   cID=toA(LXI, N);	/* get the identifier */
   if (Count==1) 	macro="\n    DVM_STV";	/* simple var */
   else
   if (B(N)) 	macro="\n    DVM_STVN";	/* array initialization */
   else return ;	/* do not trace array parameter */
   w=Call(macro, Comma(5, cline(), Lxi(cType), rts_name(cType), cID, Lxn(0)));
   addIMloc(w, cID);	/* store implicite action */
   }

/* Verify a declarator                                      */

void wfXXdcltr()
   {
   int spec, dvm, decl, init;
   w=toB(XXdecl, Stack);
   spec=A(A(w));	/* specifier */
   cDVM=B(A(B(w)));	/* directive */
   addDecl(N, spec, cDVM);	/* add the declarator */
   init=B(N);	/* initializer */
   if (init) 	
      {
      ASSERTC(ASSIGN, init, wfXXdcltr);
      if (cDVM) 	E(2033);	/* not for DVM arrays|objects */
      DTrace();	/* implicit tracing */
      }
   if (cDVM) 	ISWFdcltr(N, spec, cDVM);	/* verify the declarator */
   if (to(XXparm) && toA(TOKEN, to(LXfun))!=LX_main) 	DTrace();	/* implicit tracing of parameters */
   }

/* Verify and convert a declaration                         */

void wfXXdecl()
   {
   Allowed(XXdecl, N);	/* compare directive and declaration */
   if (to(XXparm) && B(up(1))!=0) 	
      {
      int w=up(1);
      cDVM=B(w);
      ASSERTC(LX_DVM, cDVM, wfXXdecl);
      cDir=A(cDVM);	/* DVM-directive of parameter */
      switch (C(cDir))
         {
      case DISTRIBUTE:
         if (C(B(cDir))==TEMPLATE) 	E(2002);
         else crHandler(w, N);
         break ;
      case ALIGN: crHandler(w, N);
         break ;
      default : E(2002);
         break ;
         }
      }
   }

/* DVM-pointer 'DVM(*...)' require DISTRIBUTE or ALIGN      */

void wfLX_DVM()
   {
   if (B(N)) 	
      {
      w=A(N);
      if (!(C(w)==DISTRIBUTE && C(B(w))!=TEMPLATE || C(w)==ALIGN)) 	E(2034);
      }
   }

/* Verify REDUCTION_GROUP name                              */

void wfLXIrg()
   {
   w=definedDVM(A(N));
   if (w && C(w)!=LX_RG) 	E(2041);
   }

/* Verify SHADOW_GROUP name                                 */

void wfLXIsg()
   {
   w=definedDVM(A(N));
   if (w && C(w)!=LX_SG) 	E(2042);
   }

/* Verify TASK name                                         */

void wfLXItask()
   {
   w=definedDVM(A(N));
   if (w && C(w)!=TASK) 	E(2044);
   if (OPTs==0) 	set(NEW, N, A(N));
   }

/* Verify PROCESSORS name                                   */

void wfLXIproc()
   {
   w=definedDVM(A(N));
   if (w && C(w)!=PROCESSORS && C(w)!=TASK) 	E(2056);
   if (OPTs==0) 	set(NEW, N, A(N));
   }

/*----------------------------------------------------------*/
/* Semantic functions:                                      */
/*----------------------------------------------------------*/


/* Is a directive compatible with operator|declaration?     */

int Allowed(int dop, int N)
   {
   int c;
   int semic;
   int chPloop=1;
   cDVM=B(up(1));
   if (cDVM) 	ASSERTC(LX_DVM, cDVM, Allowed);
   if (cDVM==0) 	return 0;	/* no directive */
   semic=(C(curlex)==DLM && A(curlex)==SEMIC);	/* the next token */
   cDir=A(cDVM);
   c=C(cDir);

/*    A formal parameter                                    */

   if (to(XXparm)) 	
      {
      if (c!=DISTRIBUTE && c!=ALIGN) 	E(2002);
      else
      if (B(cDVM)==0) 	E(2001);
      }
   else

/*    Other declarations                                    */

   switch (c)
      {
   case DISTRIBUTE:
   case ALIGN:
   case LX_RG:
   case LX_SG:
   case PROCESSORS:
   case LX_RMG:
   case LX_IG:
   case TASK:
   case LX_CP:
      if (dop==XXoper) 	E(2005);	/* require a declaration */
      break ;
   case DEBUG:
   case INTERVAL: chPloop=0;	/* allowed in a parallel loop */
   case PARALLEL:
   case REMOTE:
   case INDIRECT:
   case LX_TASKREG:
   case ON:
   case COPY:
   case CPSTART:
      if (!to(XXbody)) 	E(2003);	/* outside a function */
      else
      if (dop==XXoper && semic) 	E(2006);	/* require non-empty operator */
      break ;
   case REDISTRIBUTE:
   case REALIGN:
   case LX_CRTEMP:
   case LX_CRSG:
   case SHSTART:
   case SHWAIT:
   case RSTART:
   case RWAIT:
   case PREFETCH:
   case RESET:
   case MAP:
   case CPWAIT:
   case BARRIER:
      if (!to(XXbody)) 	E(2003);	/* outside a function */
      else
      if (dop==XXoper && !semic) 	E(2007);	/* require the empty operator */
      break ;
      }
   if (chPloop && inPloop(1)) 	E(2004);	/* unallowed in a parallel loop */
   return 0;	/*....*/
   }

/*----------------------------------------------------------*/
/* The main semantic function                               */
/*----------------------------------------------------------*/

int ISWF(int IN)
   {
   N=A(Stack);	/* the current node */

/*    The first invocation before parsing of a construct    */

   if (IN==1) 	
   switch (C(N))
      {
   case LBS: openScope();	/* entering new scope */
      break ;
   case XXdecl:
      if (C(up(1))==XXparm && (C(up(2))==LXfun || C(up(2))==XXlist && C(up(4))==LXfun)) 	openScope();	/* scope for formal parameters */
      break ;
   case XXbody: w=up(1);
      if (isMain(w)) 	
         {
         inMain=Arg(w);	/* command line parameters */
         mainRet=0;
         }
      break ;
   case RETURN:
      if (inPloop(1)) 	E(2004);	/* not in a parallel loop */
      break ;

/*    Verify the context of subdirectives                   */

   case REDUCTION:
   case SHRENEW:
   case SHSTART:
   case SHWAIT:
   case REMOTE:
   case INDIRECT:
   case ACROSS:
   case PIPE:
         {
         int w, c=C(N);
         if ((w=to(PARALLEL))!=0) 	
         for (Count=0, w=B(w); w; Count++, w=B(w)) 	
            {
            int i=(C(w)==LXX?C(A(w)): C(w));
            if (i==c || (c==SHRENEW || c==SHSTART || c==SHWAIT) && (i==SHRENEW || i==SHSTART || i==SHWAIT)) 	E(2008);
            }
         if (to(LX_TASKREG)) 	
         if (c!=REDUCTION) 	E(2008);
         }
      break ;

/*    Increment loops counter                               */

   case LX_TASKREG:
   case PARALLEL: wD(N, Lxn(++LoopNo));
      break ;

/*    Operator                                              */

   case XXoper: Allowed(XXoper, N);	/* compatibility with directive */
         {
         int loop, rank, dirrank;
         int deb=A(B(up(1)));	/* Is there DVM(DEBUG) directive ? */
         if (deb && C(deb)==DEBUG) 	
            {
            wD(deb, OPTmax*10+OPTcmd);	/* save */
            setDeb(deb);	/* set new debugging mode */
            w=(OPTcmd<OPTmax)?OPTcmd: OPTmax;
            if (OPTnoe) 	OPTd=w;
            else OPTe=w;
            }
         if (deb && (C(deb)==COPY || C(deb)==CPSTART)) 	
            {
            wD(deb, OPTd);
            OPTd=0;
            }

/*    Check the depth of parallel loop nest                 */

         w=up(2);
         if (C(w)==XXlist) 	
            {
            w=B(up(1));
            if (w) 	ASSERTC(LX_DVM, w, ISWF_1_XXoper);
            if (C(A(w))!=PARALLEL) 	break ;
            }
         loop=inPloop(0);
         if (loop==0) 	break ;	/* not in a parallel loop */
         w=A(B(A(loop)));
         dirrank=Rank(A(A(w)));
         rank=0;
         for (Count=0, w=Stack; w; Count++, w=B(w)) 	
            {
            if (w==loop) 	break ;
            if (C(w)==XXoper) 	rank++;
            if (C(w)==XXlist) 	
               {
               rank=-1;
               break ;
               }
            }
         if (rank<0) 	break ;
         if (C(curlex)==LXI && (A(curlex)==LX_FOR || A(curlex)==LX_DO)) 	
            {
            if (rank>dirrank) 	E(2009);
            }
         else
            {
            if (rank<=dirrank) 	E(2010);
            }
         }
      break ;
      }

/*    The last invocation after parsing of a construct      */

   else
   if (IN==0) 	
   switch (C(N))
      {
   case LBS: closeScope();	/* exiting a scope */
      break ;
   case RETURN: wfRETURN();	/* convert 'return' from the 'main' */
      break ;
   case ASSIGN:
      if (C(up(1))!=XXdcltr) 	stv2stva(N);
      break ;
   case XXdcltr: wfXXdcltr();
      break ;
   case XXdecl: wfXXdecl();
      w=A(B(N));
      if (C(A(w))==LXfun) 	closeScope();
   case XXbody: wfXXbody();
      break ;

/*    A list of declarations or operators                   */

   case XXlist: w=up(1);

/*    -- insert implicit operators                          */

      if (C(w)==XXbody || C(w)==XXlist && C(A(A(w)))==XXdecl) 	
         {
         if (inMain>1 && to(LBS)==0) 	
            {
            wA(A(inMain), mk(SUB, cline(), Lxn(1)));
            conc(Call("DVM_INIT", inMain));	/* DVM_INIT() */
            w=genIMglob();	/* global implicits */
            if (w) 	conc(w);
            w=genIMloc();	/* local implicits */
            if (w) 	conc(w);
            set(NEW, N, Xlist(NDD(C(N), A(N), B(N), D(N))));
            inMain=1;	/* "already generated" flag */
            }
         else
            {
            w=genIMloc();	/* locals only */
            if (w) 	
               {
               conc(w);
               set(NEW, N, Xlist(NDD(C(N), A(N), B(N), D(N))));
               }
            }
         }

/*    -- rebuild DVM'ed operators                           */

      if (C(A(A(N)))==XXoper) 	
         {
         int n=A(N);
         w=C(A(B(n)));
         switch (w)
            {
         case PARALLEL: set(NEW, n, NDD(0, Ploop(n), 0, 0));
            break ;
         case TASKLOOP: set(NEW, n, NDD(0, PTaskLoop(n), 0, 0));
            break ;
         case INTERVAL: w=OPTe>=2?interval(n): A(n);
            set(NEW, n, NDD(0, w, 0, 0));
            break ;
         case DEBUG: break ;
         case REMOTE: w=OPTs?A(n): mk_remote(A(B(n)), A(n));
            set(NEW, n, NDD(0, w, 0, 0));
            break ;
         case ON: w=mkRUNAM(A(A(B(n))), A(n));
            set(NEW, n, NDD(0, w, 0, 0));
            break ;
         case LX_TASKREG: w=wfTASKREGION(n);
            set(NEW, n, NDD(0, w, 0, 0));
            break ;
         case COPY:
         case CPSTART: w=OPTs?A(n): wfCOPY(n);
            set(NEW, n, NDD(0, w, 0, 0));
            break ;
            }
         }

/*    -- malloc                                             */

      if (C(w=A(A(N)))==XXoper) 	
         {
         int oper;
         int expr=A(w);
         int assign=A(expr);
         if (C(w=assign)==ASSIGN && C(w=B(w))==LXfun && toA(TOKEN, w)==LX_malloc) 	
            {
            int parm=B(w);
            int dims;
            whatis(A(assign));	/* analyse left-hand side */
            if (cDir==0) 	break ;	/* non-DVM */
            if (C(parm)!=MUL) 	E(2029);
            if (C(B(parm))!=SIZEOF) 	E(2030);
            skipA(MUL, parm);
            dims=Count;
            if (dRank && dRank<dims) 	E(2031);
            if (dRank && dRank>dims) 	E(2032);
               {
               set(NEW, A(A(N)), mk_alloc(A(assign), B(B(assign)), NDD(XXoper, expr, 0, 0)));
               }
            }
         break ;
         }

/*    DVM'ed declaration                                    */

      if (B(A(N))!=0) 	
         {
         int dir=A(B(A(N)));
         int type=C(dir);
         switch (type)
            {
         case DISTRIBUTE:
            if (C(B(dir))==TEMPLATE) 	crRef(N, TEMPLATE);
            else crHandler(A(N), A(A(N)));
            break ;
         case ALIGN: crHandler(A(N), A(A(N)));
            break ;
         case LX_SG:
         case LX_RG:
         case LX_RMG:
         case LX_IG:
         case PROCESSORS:
         case TASK:
         case LX_CP: crRef(N, type);
            break ;
         default : ASSERT(0, ISWF.LXX);
            break ;
            }
         }
      break ;

/* Operator: restore a debugging mode                       */

   case XXoper:
         {
         int deb=A(B(up(1)));
         if (deb && C(deb)==DEBUG) 	
            {
            w=D(deb);
            OPTmax=w/10;
            OPTcmd=w%10;
            w=(OPTcmd<OPTmax)?OPTcmd: OPTmax;
            if (OPTnoe) 	OPTd=w;
            else OPTe=w;
            }
         if (deb && (C(deb)==COPY || C(deb)==CPSTART)) 	
            {
            OPTd=D(deb);
            }
         }

/*    -- Is it an own assignement ?                         */

      if (!inPloop(0) && C(A(N))==XXexpr && C(A(A(N)))==ASSIGN) 	
         {
         mk_local(N);
         break ;
         }

/*    -- DO or FOR loop                                     */

   case LX_DO:
   case LX_FOR: wfLX_FOR();
      break ;

/* Other nodes                                              */

   case LBK: wfLBK();
      break ;
   case LXI: wfLXI();
      break ;
   case LXfun: wfLXfun();
      break ;
   case LX_DVM: wfLX_DVM();
      break ;
   case LXIrg: wfLXIrg();
      break ;
   case LXIsg: wfLXIsg();
      break ;
   case LXItask: wfLXItask();
      break ;
   case LXIproc: wfLXIproc();
      break ;
   case DISTRIBUTE: wfDISTRIBUTE();
      break ;
   case ONTO: wfONTO();
      break ;
   case GENBLOCK: wfGENBLOCK();
      break ;
   case WGTBLOCK: wfWGTBLOCK();
      break ;
   case TEMPLATE: wfTEMPLATE();
      break ;
   case SHADOW: wfSHADOW();
      break ;
   case REDISTRIBUTE: wfREDISTRIBUTE();
      break ;
   case ALIGN: wfALIGN();
      break ;
   case REALIGN: wfREALIGN();
      break ;
   case DVMalign: wfDVMalign();
      break ;
   case LX_CRTEMP: wfLX_CRTEMP();
      break ;
   case MAP: wfMAP();
      break ;
   case DVMvar: wfDVMvar();
      break ;
   case DVMind: wfDVMind();
      break ;
   case DVMbind: wfDVMbind();
      break ;
   case DVMshw: wfDVMshw();
      break ;
   case DVMshad: wfDVMshad();
      break ;
   case LX_CRSG: wfLX_CRSG();
      break ;
   case SHRENEW: wfSHRENEW();
      break ;
   case SHSTART: wfSHSTART();
      break ;
   case SHWAIT: wfSHWAIT();
      break ;
   case ACROSS: wfACROSS();
      break ;
   case PIPE: wfPIPE();
      break ;
   case REDUCTION: wfREDUCTION();
      break ;
   case RSTART: wfRSTART();
      break ;
   case RWAIT: wfRWAIT();
      break ;
   case CPWAIT: wfCPWAIT();
      break ;
   case BARRIER: wfBARRIER();
      break ;
   case LXIag: wfLXIag();
      break ;
   case REMOTE: wfREMOTE();
      break ;
   case PREFETCH: wfPREFETCH();
      break ;
   case RESET: wfRESET();
      break ;

/* The base of alignement or parallel loop                  */

   case DVMbase:
         {
         int base=A(N), dir;
         if (B(N)!=0) 	break ;
         ;

/*        null base only in a distribute or aling stuff     */

         if (base==0) 	
            {
            break ;
            }
         if (C(base)==LBA) 	base=A(base);
         w=whatis(base);	/* find and unpack the declaration */
         if (base && cDecl==0) 	
            {
            E(2040);
            break ;
            }

/*    TASK is allowed in a task parallel loop only          */

         if (cDIR==TASK) 	
            {
            w=up(2);
            if (C(w)!=PARALLEL) 	
               {
               E(2079);
               break ;
               }
            wC(w, TASKLOOP);
            break ;
            }

/*    A base must be distributed                            */

         if (cDIR!=DISTRIBUTE && cDIR!=ALIGN) 	
            {
            E(2078);
            break ;
            }

/*                                                          */


/*    A base in SHADOW must not be a TEMPLATE               */

         if (C(up(1))==DVMshad) 	
            {
            if (cDIR==DISTRIBUTE && C(B(cDir))==TEMPLATE) 	
               {
               E(2080);
               break ;
               }
            }
         }
      break ;
   default : ;
      }
   else ASSERT(0, ISWF);
   return 0;
   }

/*----------------------------------------------------------*/
/* Semantic function for nodes substitution                 */
/*----------------------------------------------------------*/

int pSUBST(int IN)
   {
   N=A(Stack);	/* next visited node */

/*    descending invocation                                 */

   if (IN==1) 	
   switch (C(N))
      {
   case LXI:
      if (OPTs==0) 	
         {
         for (Count=0, w=RenList; w; Count++, w=B(w)) 	
         if (C(w)==A(N)) 	wA(N, A(w));	/* rename */
         }

/*    other terminals                                       */

   case TOKEN:
   case DLM:
   case KWD:
   case LXN:
   case LXR:
   case LXC:
   case LXS:
   case LXSs:
   case CPP: return 0;	/* do not descent */
   case LXANY:
   case NL: ASSERT(0, walk(pSUBST));
   case LXX:
   default : return 3;	/* descent */
      }

/*    ascending invocation                                  */

   else
   if (IN==0) 	
      {
      if (-D(N)>1 && C(N)!=LXI) 	
         {
         w=get(NEW, N);	/* was new node created? */
         if (w) 	wN(N, C(w), A(w), B(w));	/* substitute */
         }
      }
   else ASSERT(0, pSUBST);
   return 0;
   }

/* Find a node with the given code below the current node   */

int Over=0;	/* Code to find */

/* 'Semantic function' looking for the code                 */

int pOver(int IN)
   {
   int N=A(Stack);	/* next visited node */
   if (Over<0) 	return 0;	/* already found -- do not descent */
   if (IN==1) 	
   switch (C(N))
      {

/*        terminals                                         */

   case LXI:
   case TOKEN:
   case DLM:
   case KWD:
   case LXN:
   case LXR:
   case LXC:
   case LXS:
   case LXSs:
   case CPP: return 0;	/* do not descent */
   case LXANY:
   case NL: ASSERT(0, walk(pSUBST));
   case LXX:
   default :
      if (C(N)==Over) 	
         {
         Over=-1;	/* OK: stop searching */
         break ;
         }
      return 3;	/* look for below */
      }
   return 0;
   }

/* Walk-through with the sem.function 'pOver'               */

int over(int C)
   {
   Over=C;	/* global for the code to be found */
   walk(N, pOver);	/* start walk-through */
   w=Over;
   Over=0;	/* clean the global */
   return w<0;	/* 1 if the code was found */
   }

/*----------------------------------------------------------*/
/*----------------------------------------------------------*/


/* Get command line options                                 */

int cmd_line(int args, char **arg)
   {
   int n;
   *bf=0;	/* clean message buffer */
   if (args==1) 	
      {
      strcpy(bf, Usage);
      return 1;	/* no parameters */
      }
   arg++;
   for (n=0; n<(args); n++) 	
      {
      if (arg[n]==0) 	break ;
      if (arg[n][0]!='-') 	strcpy(infile, arg[n]);	/* input file name */
      else
      switch (arg[n][1])
         {

/* -v                                                       */

      case 'v': OPTv=1;
         break ;
      case 'V': OPTv=2;
         break ;

/* -w, -w-                                                  */

      case 'w':
         if (arg[n][2]=='-') 	OPTw=5;
         else OPTw=0;
         break ;

/* -o outputfile                                            */

      case 'o':
         if (++n<args) 	strcpy(outfile, arg[n]);
         continue ;

/* -s                                                       */

      case 's': OPTs=1;
         OPTm=2;
         break ;

/* -p                                                       */

      case 'p': OPTs=0;
         OPTm=0;
         break ;

/* -d...                                                    */

      case 'd':
         if (OPTe) 	
            {
            OPTe=0;	/* '-e -d' equ. '-d' */
            }
         getDopt(arg[n]);	/* add to options string */
         if (arg[n][3]==0) 	
         switch (arg[n][2])
            {
         case '1': OPTd=1;
            break ;
         case '2': OPTd=2;
            break ;
         case '3': OPTd=3;
            break ;
         case '4': OPTd=4;
            break ;
         case '0': OPTd=0;
            break ;
         default : OPTd=1;
            break ;
            }
         OPTnoe=1;
         OPTcmd=OPTd;
         break ;

/* -e...                                                    */

      case 'e':
         if (OPTd) 	goto incompat;	/* '-d -e' is wrong */
         getDopt(arg[n]);	/* add to options string */
         if (arg[n][3]==0) 	
         switch (arg[n][2])
            {
         case '1': OPTe=1;
            break ;
         case '2': OPTe=2;
            break ;
         case '3': OPTe=3;
            break ;
         case '4': OPTe=4;
            break ;
         case '0': OPTe=0;
            break ;
         default : OPTe=1;
            break ;
            }
         OPTcmd=OPTe;
         break ;
      default : sprintf(bf, "\nUnknown option %s\n", arg[n]);
         return 1;

/* -? -h -H -- help screen                                  */

      case '?':
      case 'h':
      case 'H': strcpy(bf, Usage);
         return 1;

/* -xNN                                                     */

      case 'x':
         if ((UX=atoi(arg[n]+2))==0) 	
            {
            sprintf(bf, "\nWrong size option %s\n", arg[n]);
            return 1;
            }
         continue ;
incompat:
         sprintf(bf, "\nIncompatible options -d and -e.\n");
         return 1;
         }
      }
   return 0;
   }

/* Internal debugging (Dump of the linked memory)           */

void dump(int N)
   {
   if (fdeb==0) 	return ;
   fprintf(fdeb, "\n--------------------------\n");
   for (Count=0, N=N; N; Count++, N=B(N)) 	
      {
      fprintf(fdeb, "%4d ln%d\t->", N, D(N));
      dump1(A(N));
      }
   }
void dumpAll(int N)
   {
   if (fdeb==0) 	return ;
   fprintf(fdeb, "\n-------- Dump ND ---------\n");
   for (; N<NDsize; N++) 	dump1(N);
   }
char *dumpField(int N)
   {
   if (C(N)==TOKEN && A(N)<0) 	return LX-A(N);
   if (C(N)>LXX && C(N)<NL) 	
      {
      N=A(N);
      if (C(N)==TOKEN && A(N)<0) 	return LX-A(N);
      }
   return "";
   }
void dump1(int N)
   {
   int c, a, b, d;
   if (fdeb==0) 	return ;
   if (N<0) 	
      {
      fprintf(fdeb, "is not a node");
      return ;
      }
   c=C(N);
   a=A(N);
   b=B(N);
   d=D(N);
   if ((a|b|c)==0 && d==N+1) 	return ;
   fprintf(fdeb, "%4d:%6d%6d%6d%6d| ", N, c, a, b, d);
   if (c==1) 	
      {
      fprintf(fdeb, "====\t%s\t", a<0?LX-a: "");
      }
   else
      {
      if (c>0) 	fprintf(fdeb, "%s, ", dumpField(c));
      if (a>=0) 	fprintf(fdeb, "%s, ", dumpField(a));
      if (b>=0) 	fprintf(fdeb, "%s, ", dumpField(b));
      if (d>=0) 	fprintf(fdeb, "%s, ", dumpField(d));
      }
   fprintf(fdeb, "\n");
   }

/* An assertion failed; ussue message and dump, and stop    */

int ASSfailed(char *c, char *fun)
   {
   static int assert=0;
   int lno=-D(Stack);	/* the line of the current node */
   printf("\n%s() FATAL ERROR near line %d\t\t!(%s)\n", infile, lno, c);
   if (fdeb) 	fprintf(fdeb, "\n============================================="
   "\n---------- ASSERTION (%s) FAILED in %s\n", c, fun);
   if (assert==0) 	
      {
      assert=1;
      dump(Stack);
      dumpAll(SOURCE);
      }
   else return 0;	/* recursion ! */
   FINISH(-1);
   return 0;
   }

/*----------------------------------------------------------*/
/*  The 'main' function                                     */
/*----------------------------------------------------------*/

int main(int args, char **arg)
   {

/* Get command line parameters                              */

   if (cmd_line(args, arg)) 	
      {
      fprintf(stderr, "\n%s\n", bf);
      exit(-1);
      }
   ;
   if (sizeof (int )==2 && UX>1) 	
      {
      UX=1;
      fprintf(stderr, "16-bit version resets to -x1\n");
      }

/* Allocate memory for tables                               */

   HTsize=0x1ff1*UX+1;
   NDsize=8000*UX;
   LXSIZE=0x1ff0u*UX;
   HT0=(int *)calloc(HTsize, sizeof (*HT0));
   HT1=(int *)calloc(HTsize, sizeof (*HT1));
   LX=(char *)calloc(LXSIZE, sizeof (*LX));
   NDs=(struct ND*)calloc(NDsize, sizeof (*NDs));
   if (NDs==0) 	
      {
      fprintf(stderr, "\n%s\n", "Not enough memory");
      exit(-1);
      }
   ;

/* Open internal debugging file if necessary                */

   Open(&fmsg, msgfile, "w");
   fprintf(fmsg, "%s\n", Title);
   if ((fdeb=fopen(debfile, "r"))!=0) 	
      {
      fclose(fdeb);
      Open(&fdeb, debfile, "w");
      }

/* Initialize                                               */

   if (OPTv) 	printf("\n%s\t\t-x%d\n", Title, UX);
   NDinit();
   LXinit();
   STXinit();
   MsgInit(Msg);
   Reninit();

/* Parse debugging options string                           */

   if (obf[0]) 	
      {
      if (OPTv || fdeb) 	printf("\nDEBUG options: %s\n", obf);
      scanDopt();
      }
   else DoptROOT=0;

/* Open output and input files                              */

   Open(&fin, infile, "r");
   nextLine();
   Open(&fout, outfile, "w");

/* Read input file                                          */

   if (fdeb) 	printf("%s", "Reading...");
   SOURCE=NDfree;
   do
      {
      scan();
      NDD(LXcode, LXval, 0, -lineno);
      }
   while (LXcode) 	;
   ;

/* Parse source program                                     */

   if (fdeb) 	printf("%s", "Parsing...");
   for (curlex=SOURCE; C(curlex)==TOKEN; curlex++) 	;
   TREE=Parse(0, A(SYNTAX), 0);
   ;
   if (fdeb) 	
   if (fdeb) 	printf("%s", "+");
   dumpAll(SOURCE);
   ;

/* If the parser failed, stop execution                     */

   if (C(curlex)!=0) 	
      {
      if (fdeb) 	printf("%s", "\t Unparse");
      for (curlex=SOURCE; C(curlex)==TOKEN; curlex++) 	;
      UnPars(TREE, SYNTAX, 1);
      ;
      fprintf(fout, "\n\n#error \"The Parser can not proceed further\"\n");
      fclose(fout);
      ERR("9:Parser stopped before EOF (see cvdm_out.c\n");
      }
   if (TREE==0) 	
      {
      fprintf(stderr, "\n%s\n", "Empty tree... Nothing to do.");
      exit(-1);
      }
   ;
   if (IMglobal>0 && inMain==0) 	E(2023);
   if (!fdeb) 	
   if (ERRno) 	
      {
      FINISH(1000);
      }

/* Modify the tree and generate output                      */

   if (fdeb) 	printf("%s", "Converting...");
   walk(TREE, pSUBST);
   ;	/* Change the tree */
   genhead();
   if (fdeb) 	
   if (fdeb) 	printf("%s", "+");
   dumpAll(0);
   ;
   if (fdeb) 	printf("%s", "Writing...");
   UnPars(TREE, SYNTAX, 1);
   ;

/* OK                                                       */

   if (fdeb) 	printf("%s", "\tOK\n");
   FINISH(0);
   return 0;
   }
