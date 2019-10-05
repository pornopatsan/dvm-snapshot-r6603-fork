#include "user.h"

#define MAXTAGS 1000
#include "dvm_tag.h"


#ifdef IN_M_
#define EXTERN
#else
#define EXTERN extern
#endif

struct graph_node {
       int id;   //a number of node
       graph_node *next;
       graph_node *next_header_node; //???
       graph_node *Inext;
       SgFile *file;      
       SgStatement *st_header;
       SgSymbol *symb;              //??? st_header->symbol()
       struct edge *to_called;    //outcoming
       struct edge *from_calling; //incoming
       int split;     //flag
       int tmplt;     //flag
       int visited;   //flag for partition algorithm
       int clone;    //flag is clone node
       int count;    //counter of inline expansions
};

struct graph_node_list { 
       graph_node_list *next;
       graph_node *node;
};

struct edge {
       edge *next;
       graph_node *from;
       graph_node *to;
       int inlined; //1 - inlined, 0 - not inlined
};

struct edge_list {
       edge_list *next;
       edge *edg;
};


struct block_list {
       block_list *next;
       block_list *same_name;
       SgExpression *block;
};


struct distribute_list {
       distribute_list *next;
       SgStatement *stdis;
};

struct stmt_list {
       stmt_list *next;
       SgStatement *st;
};

struct label_list {
       label_list *next;
       SgLabel *lab;
       SgLabel *newlab;
};

struct dist_symb_list {
       dist_symb_list *next;
       SgSymbol *symb;
};


struct align {
       SgSymbol * symb;
       align * next;
       align * alignees;
       SgStatement * align_stmt;
};
struct mod_attr{
       SgSymbol *symb;
       SgSymbol *symb_list;
};
struct algn_attr {
       int type;
       align *ref;
};
struct rem_var {
       int index;
       int amv;
       int ncolon;
};
struct rem_acc {
       SgExpression *rml;
       SgStatement *rmout;
       int rmbuf_use[5];
       rem_acc *next;
};
struct group_name_list {
       group_name_list *next;
       SgSymbol *symb;
};
struct symb_list {
       symb_list *next;
       SgSymbol *symb;
};
struct base_list {
       base_list *next;
       SgSymbol *type_symbol;
       SgSymbol *base_symbol;
};
struct D_do_list {
       D_do_list *next;
       int No;
       int num_line;
       SgLabel *end_lab;
       SgSymbol *do_var;
};
struct interval_list {
       interval_list *prev;
       int No;
       SgStatement *begin_st;
};
struct D_fragment {
       D_fragment *next;
       int No;
};

struct fragment_list {
       int No;
       SgStatement *begin_st;
       int dlevel;
       int elevel;
       int dlevel_spec;
       int elevel_spec;
       fragment_list *next;
};
struct fragment_list_in {
       int N1;
       int N2;
       int level;
       fragment_list_in *next;
};
struct reduction_list {
       reduction_list *next;
       int red_op;
       SgExpression *red_var;
       int ind;
};
struct IND_ref_list {
       IND_ref_list *next;
       SgExpression *rmref;
       SgExpression *axis[7];
       SgExpression *coef[7];
       SgExpression *cons[7];
       int nc;
       int ind;
};

struct coeffs {
       SgSymbol *sc[10];
       int use;
};

struct heap_pointer_list {
       heap_pointer_list *next;
       SgSymbol *symb_heap;
       SgSymbol *symb_p;
};

struct filename_list {
       filename_list *next;
       char *name;
       SgSymbol *fns;
};

const int ROOT = 1;
const int NODE = 2;
const int GRAPH_NODE   = 1000;
const int PRE_BOUND = 1001;
const int CONSTANT_MAP = 1002;
const int ARRAY_MAP = 1003;
const int ARRAY_MAP_1 = 1004;
const int ARRAY_MAP_2 = 1005;
const int ADJUSTABLE_ = 1006;

const int MAX_INTRINSIC_NUM =300;

const int MAX_LOOP_LEVEL = 10; // 7 - maximal number of loops in parallel loop nest 
const int MAX_LOOP_NEST = 25;  // maximal number of nested loops
const int MAX_FILE_NUM = 100;  // maximal number of file reference in procedure
const int SIZE_IO_BUF = 262144; //4185600;   // IO buffer size in elements
const int ANTIDEP = 0;
const int FLOWDEP = 1;
#define FICT_INT  2000000000            /* -2147483648  0x7FFFFFFFL*/

//enum{ Integer, Real, Double, Complex, Logical, DoubleComplex};
enum {UNIT_,FMT_,REC_,ERR_,IOSTAT_,END_,NML_,EOR_,SIZE_,ADVANCE_};
enum {U_,FILE_,STATUS_,ER_,IOST_,ACCESS_,FORM_,RECL_,BLANK_,EXIST_,
OPENED_,NUMBER_,NAMED_,NAME_,SEQUENTIAL_,DIRECT_,NEXTREC_,FORMATTED_,
UNFORMATTED_,POSITION_,ACTION_,READWRITE_,READ_,WRITE_,DELIM_,PAD_};

enum {ICHAR, CHAR,INT,IFIX,IDINT,FLOAT,REAL,SNGL,DBLE,CMPLX,DCMPLX,AINT,DINT,ANINT,DNINT,NINT,IDNINT,ABS,IABS,DABS,CABS,
      MOD,AMOD,DMOD, SIGN,ISIGN, DSIGN, DIM,IDIM,DDIM, MAX,MAX0, AMAX1,DMAX1, AMAX0,MAX1, MIN,MIN0,
      AMIN1,DMIN1,AMIN0,MIN1,LEN,INDEX,AIMAG,DIMAG,CONJG,DCONJG,SQRT,DSQRT,CSQRT,EXP,DEXP,CEXP,LOG,ALOG,DLOG,CLOG,
      LOG10,ALOG10,DLOG10,SIN,DSIN,CSIN,COS,DCOS,CCOS,TAN,DTAN,ASIN,DASIN,ACOS,DACOS,ATAN,DATAN,
      ATAN2,DATAN2,SINH,DSINH,COSH,DCOSH,TANH,DTANH, LGE,LGT,LLE,LLT};
//universal: ANINT,NINT,ABS,  MOD,SIGN,DIM,MAX,MIN,SQRT,EXP,LOG,LOG10,SIN,COS,TAN,ASIN,ACOS,ATAN,ATAN2,SINH,COSH,TANH
//enum {SIZE,LBOUND,UBOUND,LEN,CHAR,KIND,F_INT,F_REAL,F_CHAR,F_LOGICAL,F_CMPLX}; //intrinsic functions of Fortran 90

const int Integer = 0;
const int Real = 1;
const int Double = 2;
const int Complex = 3;
const int Logical = 4;
const int DComplex = 5;



#define ATTR_NODE(A) ((graph_node **)(A)->attributeValue(0,GRAPH_NODE))
#define GRAPHNODE(A) (*((graph_node **)(A)->attributeValue(0,GRAPH_NODE)))
#define PREBOUND(A) ((SgExpression **)(A)->attributeValue(0,PRE_BOUND))
#define ARRAYMAP(A) ((SgExpression *)(A)->attributeValue(0,ARRAY_MAP_1))
#define ARRAYMAP2(A) ((SgExpression *)(A)->attributeValue(0,ARRAY_MAP_2))
#define CONSTANTMAP(A) ((SgExpression *)(A)->attributeValue(0,CONSTANT_MAP))
#define ADJUSTABLE(A) ((SgExpression *)(A)->attributeValue(0,ADJUSTABLE_))


#define HEADER(A)  ((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_HEADER))
#define INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_HEADER)))
#define DVM000(N)   (new SgArrayRefExp(*dvmbuf, *new SgValueExp(N)))  
#define SH_GROUP(S)  (*((int *) (S) -> attributeValue(0, SHADOW_GROUP_IND)))
#define RED_GROUP(S)  (*((int *) (S) -> attributeValue(0, RED_GROUP_IND)))
#define SHADOW_(A) ((SgExpression **)(ORIGINAL_SYMBOL(A))->attributeValue(0,SHADOW_WIDTH))
#define POINTER_DIR(A) ((SgStatement **)(ORIGINAL_SYMBOL(A))->attributeValue(0,POINTER_))
#define DISTRIBUTE_DIRECTIVE(A) ((SgStatement **)(ORIGINAL_SYMBOL(A))->attributeValue(0,DISTRIBUTE_))
#define ARRAY_BASE_SYMBOL(A) ((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_BASE))
#define INDEX_SYMBOL(A) ((SgSymbol **)(A)->attributeValue(0,INDEX_DELTA))
#define INIT_LOOP_VAR(A) ((SgSymbol **)(A)->attributeValue(0,INIT_LOOP))
#define CONSISTENT_HEADER(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,CONSISTENT_ARRAY_HEADER)))
#define POINTER_INDEX(A) (*((int *)(A)->attributeValue(0,HEAP_INDEX)))
#define BUFFER_INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT)))
#define BUFFER_COUNT_PLUS_1(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT))) =  (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,BUFFER_COUNT)))+1;
#define PS_INDEX(A) (*((int *)(A)->attributeValue(0,TASK_INDEX)))
#define DEBUG_INDEX(A) (*((int*)(ORIGINAL_SYMBOL(A))->attributeValue(0,DEBUG_AR_INDEX)))
#define TASK_SYMBOL(A) (*((SgSymbol **)(ORIGINAL_SYMBOL(A))->attributeValue(0,TSK_SYMBOL)))
#define AR_COEFFICIENTS(A)  ((coeffs *) (ORIGINAL_SYMBOL(A))->attributeValue(0,ARRAY_COEF))
#define MAX_DVM   maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm  
#define FREE_DVM(A)  maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm;  ndvm-=A  
#define SET_DVM(A)   maxdvm = (maxdvm < ndvm) ? ndvm-1 : maxdvm;  ndvm=A  
#define FREE_HPF(A)  maxhpf = (maxhpf < nhpf) ? nhpf-1 : maxhpf;  nhpf-=A  
#define SET_HPF(A)   maxhpf = (maxhpf < nhpf) ? nhpf-1 : maxhpf;  nhpf=A  
#define HPF000(N)   (new SgArrayRefExp(*hpfbuf, *new SgValueExp(N)))  
#define IS_DUMMY(A)  ((A)->thesymb->entry.var_decl.local == IO) 
#define IS_TEMPLATE(A)  ((A)->attributes() & TEMPLATE_BIT) 
#define IN_COMMON(A)  ((A)->attributes() & COMMON_BIT) 
#define IN_DATA(A)  ((A)->attributes() & DATA_BIT) 
#define IN_EQUIVALENCE(A)  ((A)->attributes() & EQUIVALENCE_BIT) 
#define IS_ARRAY(A)  ((A)->attributes() & DIMENSION_BIT)
#define IS_ALLOCATABLE(A)  ((A)->attributes() & ALLOCATABLE_BIT) 
#define IS_ALLOCATABLE_POINTER(A)  (((A)->attributes() & ALLOCATABLE_BIT) || ((A)->attributes() & POINTER_BIT))
#define IS_POINTER_F90(A)  ((A)->attributes() & POINTER_BIT) 
#define CURRENT_SCOPE(A)  (((A)->scope() == cur_func) && ((A)->thesymb->entry.var_decl.local != BY_USE) )
#define IS_BY_USE(A)  ((A)->thesymb->entry.Template.base_name != 0) 
/*#define ORIGINAL_SYMBOL(A) (OriginalSymbol(A)) */
#define ORIGINAL_SYMBOL(A) (IS_BY_USE(A) ? (A)->moduleSymbol() : (A))
#define IS_SAVE(A) (((A)->attributes() & SAVE_BIT) || (saveall && !IS_TEMPLATE(A) && !IN_COMMON(A) && !IS_DUMMY(A)) )
#define IS_POINTER(A)  ((A)->attributes() & DVM_POINTER_BIT) 
#define IS_SH_GROUP_NAME(A) ((A)->variant() == SHADOW_GROUP_NAME)
#define IS_RED_GROUP_NAME(A) ((A)->variant() == REDUCTION_GROUP_NAME)
#define IS_GROUP_NAME(A) (((A)->variant() == SHADOW_GROUP_NAME) || ((A)->variant() == REDUCTION_GROUP_NAME) || ((A)->variant() == REF_GROUP_NAME)) 
#define IS_DVM_ARRAY(A) (((A)->attributes() & DISTRIBUTE_BIT) || ((A)->attributes() & ALIGN_BIT) || ((A)->attributes() & INHERIT_BIT))
#define IS_DISTR_ARRAY(A) (((A)->attributes() & DISTRIBUTE_BIT) || ((A)->attributes() & ALIGN_BIT) || ((A)->attributes() & INHERIT_BIT))
#define IN_MODULE (cur_func->variant() == MODULE_STMT)
#define IN_MAIN_PROGRAM (cur_func->variant() == PROG_HEDR)
#define DVM_PROC_IN_MODULE(A) ((mod_attr *)(A)->attributeValue(0,MODULE_STR))
#define LINE_NUMBER_BEFORE(ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore((many_files ? D_FileLine(ndvm,ST) : D_Lnumb(ndvm)) ,WHERE)
#define LINE_NUMBER_STL_BEFORE(STL,ST,WHERE)     doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),WHERE); ndvm--; InsertNewStatementBefore(STL= (many_files ? D_FileLine(ndvm,ST) :  D_Lnumb(ndvm)),WHERE)
#define LINE_NUMBER_AFTER(ST,WHERE)   InsertNewStatementAfter ((many_files ? D_FileLine(ndvm,ST) : D_Lnumb(ndvm)),WHERE,(WHERE)->controlParent());  doAssignStmtBefore(new SgValueExp((ST)->lineNumber()),cur_st); ndvm--
#define LINE_NUMBER_N_AFTER(N,WHERE,CP)  InsertNewStatementAfter((many_files ? D_FileLine(ndvm,CP): D_Lnumb(ndvm)),WHERE,CP);  doAssignStmtBefore(new SgValueExp(N),cur_st); ndvm--
#define LINE_NUMBER_NEXP_AFTER(NE,WHERE,CP)  InsertNewStatementAfter((many_files ? D_DummyFileLine(ndvm,"dvm_check"): D_Lnumb(ndvm)),WHERE,CP);  doAssignStmtBefore((NE),cur_st); ndvm--
#define ALIGN_RULE_INDEX(A) ((int*)(A)->attributeValue(0,ALIGN_RULE))
#define INTERVAL_LINE  (St_frag->begin_st->lineNumber())
#define INTERVAL_NUMBER  (St_frag->No)
#define GROUP_REF(S,I) (new SgArrayRefExp(*(S),*new SgValueExp(I)))
#define IS_DO_VARIABLE_USE(E) ((SgExpression **)(E)->attributeValue(0,DO_VARIABLE_USE))
#define HEADER_SIZE(A) (1+(maxbuf+1)*2*(Rank(A)+1))
#define HSIZE(R) (2*R + 2)
#define ARRAY_ELEMENT(A,I)  (new SgArrayRefExp(*A, *new SgValueExp(I)))
#define INTEGER_VALUE(E,C) ((E)->variant() == INT_VAL && (E)->valueInteger() == (C))
#define IS_INTRINSIC_TYPE(T) (!TYPE_RANGES((T)->thetype) && !TYPE_KIND_LEN((T)->thetype) && ((T)->variant() != T_DERIVED_TYPE))

//----------------------------------------------------------------------------------------

#define DECL(A)  ((A)->thesymb->decl) 
#define HEDR(A)  ((A)->thesymb->entry.Template.func_hedr)
#define PROGRAM_HEADER(A) ((A)->thesymb->entry.prog_decl.prog_hedr)

#define NON_CONFORMABLE   0
#define _IDENTICAL_       1
#define _CONSTANT_        2
#define _ARRAY_           3
#define SCALAR_ARRAYREF   4
#define VECTOR_ARRAYREF   5
#define _SUBARRAY_        6
         
EXTERN SgConstantSymb *Iconst[10];
EXTERN const char *tag[MAXTAGS];
EXTERN int ndvm;  // index for buffer array 'dvm000'	
EXTERN int maxdvm;  //  size of array 'dvm000' 
EXTERN int loc_distr;
EXTERN int send;  //set to 1 if I/O statement require 'send' operation
EXTERN char *fin_name; //input file name
EXTERN SgFile *current_file;    //current file
EXTERN SgStatement *where;//used in doAssignStmt: new statement is inserted before 'where' statement
EXTERN int nio;
EXTERN SgSymbol *bufIO[6];
EXTERN SgSymbol *loop_var[8]; // for generatig DO statements


EXTERN SgStatement *par_do;  // first DO statement of current parallel loop  
EXTERN int iplp; //dvm000 element number for storing ParLoopRef
EXTERN int irg;  //dvm000 element number for storing RedGroupRef
EXTERN int irgts;  //dvm000 element number for storing RedGroupRef(task_region)
EXTERN int idebrg;  //dvm000 element number for storing DebRedGroupRef
EXTERN SgExpression *redgref; // reduction group reference
EXTERN SgExpression *redgrefts; // reduction group reference for TASK_REGION
EXTERN SgExpression *debredgref; // debug reduction group reference
EXTERN SgExpression *red_list; // reduction operation list in FDVM program
EXTERN SgExpression *task_red_list; // reduction operation list (in TASK_REGION directive)
EXTERN int iconsg;  //dvm000 element number for storing ConsistGroupRef
EXTERN int iconsgts;  //dvm000 element number for storing ConsistGroupRef(task_region)
EXTERN int idebcg;  //dvm000 element number for storing DebRedGroupRef
EXTERN SgExpression *consgref; // consistent group reference
EXTERN SgExpression *consgrefts; // consistent group reference for TASK_REGION
EXTERN SgExpression *debconsgref; // debug reduction(consistent) group reference
EXTERN SgExpression *cons_list; // consistent array list in FDVM program
EXTERN SgExpression *task_cons_list; // consistent array list (in TASK_REGION directive)
EXTERN SgLabel *end_lab, *begin_lab; //labels for parallel loop nest
EXTERN D_do_list *cur_do;
EXTERN D_do_list *free_list;
EXTERN int Dloop_No;
EXTERN int pardo_No;
EXTERN int taskreg_No;
EXTERN int pardo_line;
EXTERN int D_end_do;
EXTERN int nfrag ; //counter of intervals for performance analizer 
EXTERN interval_list *St_frag ;
EXTERN interval_list *St_loop_first;
EXTERN interval_list *St_loop_last;
EXTERN int perf_analysis ; //set to 1 by -e1
EXTERN int close_loop_interval;
EXTERN stmt_list *goto_list;
EXTERN int len_int; //set by option -bind
EXTERN int len_long;//set by option -bind
EXTERN int bind;//set by option -bind
EXTERN int dvm_debug ;   //set to 1 by -d1  or -d2 or -d3 or -d4 flag
EXTERN int only_debug ;  //set to 1 by -s flag
EXTERN int level_debug ; //set to 1 by -d1, to 2 by -d2, ...
EXTERN fragment_list_in *debug_fragment; //set by option -d
EXTERN fragment_list_in *perf_fragment; //set by option -e
EXTERN int debug_regim; //set by option -d
EXTERN int check_regim; //set by option -dc
EXTERN int dbg_if_regim; //set by option -dbif
EXTERN  int IOBufSize; //set by option -bufio
EXTERN SgSymbol *dbg_var;
EXTERN int HPF_program;
EXTERN int rmbuf_size[6];
EXTERN int first_time;
EXTERN SgStatement *indep_st; //first INDEPENDENT directive of loop nest
EXTERN SgStatement *ins_st1, *ins_st2; // for INDEPENDENT loop
EXTERN SgSymbol *DoVar[MAX_LOOP_NEST], **IND_var, **IEX_var;
EXTERN int iarg; // for INDEPENDENT loop
//---------------------------------------------------------------------
EXTERN int errcnt;  // counter of errors in file 
EXTERN graph_node *first_node, *node_list, *first_header_node, *cur_node, *DAG_list, *top_node; 
EXTERN graph_node_list  *all_node_list, *header_node_list, *dead_node_list, *nobody_node_list;
EXTERN SgStatement *cur_func;  // current function
EXTERN SgSymbol *cur_symb, *top_symb_list, *sub_symb_list;
EXTERN int do_dummy, do_stmtfn; // flag for building call graph: by default do_dummy=0, do_stmtfn=0
EXTERN int gcount;
EXTERN SgStatement *cur_st;  // current statement  (for inserting)
EXTERN SgStatement *global_st;  // first statement of file (global_bfnd)
EXTERN stmt_list *entryst_list;
//EXTERN stmt_list  *DATA_list;
EXTERN int max_lab;  // maximal  label in file
EXTERN int num_lab;  // maximal(last)  new  label
EXTERN int vcounter;
EXTERN SgStatement *top_header, *top_last,* top_first_executable,*top_last_declaration, *top_global;
EXTERN label_list *format_labels, *top_labels, *proc_labels; 
EXTERN SgSymbol *do_var[10];
EXTERN symb_list *top_temp_vars;
EXTERN block_list *common_list, *common_list_l, *equiv_list, *equiv_list_l;
EXTERN block_list *top_common_list, *top_common_list_l, *top_equiv_list, *top_equiv_list_l;
EXTERN int modified;
EXTERN int intrinsic_type[MAX_INTRINSIC_NUM];
EXTERN const char *intrinsic_name[MAX_INTRINSIC_NUM];
EXTERN int deb_reg, with_cmnt;
//---------------------------------------------------------------------
/*  inl_exp.cpp   */
void initialize();
void InlinerDriver(SgFile *f);
void CallGraph(SgStatement *func);
void initVariantNames();
int isDummyArgument(SgSymbol *s);
int isStatementFunction(SgSymbol *s);
void FunctionCallSearch(SgExpression *e);
void FunctionCallSearch_Left(SgExpression *e);
void Arg_FunctionCallSearch(SgExpression *e);
stmt_list  *addToStmtList(stmt_list *pstmt, SgStatement *stat);
stmt_list  *delFromStmtList(stmt_list *pstmt);
graph_node_list  *addToNodeList(graph_node_list *pnode, graph_node *gnode);
graph_node_list  *delFromNodeList(graph_node_list *pnode, graph_node *gnode);
graph_node_list  *isInNodeList(graph_node_list *pnode, graph_node *gnode);
graph_node *CreateGraphNode(SgSymbol *s, SgStatement *header_st);
graph_node *NewGraphNode(SgSymbol *s, SgStatement *header_st);
void PrintGraphNode(graph_node *gnode);
void PrintGraphNodeWithAllEdges(graph_node *gnode);
void PrintWholeGraph();
void PrintWholeGraph_kind_2 ();
graph_node *NodeForSymbInGraph(SgSymbol *s, SgStatement *stheader);
void Call_Site(SgSymbol *s, int inlined);
edge *CreateOutcomingEdge(graph_node *gnode, int inlined);
edge *CreateIncomingEdge(graph_node *gnode, int inlined);
edge *NewEdge(graph_node *from, graph_node *to, int inlined);
void BuildingHeaderNodeList();
void RemovingDeadSubprograms();
int isHeaderNode(graph_node *gnode);
int isDeadNode(graph_node *gnode);
int isHeaderStmtSymbol(SgSymbol *s);
void DeleteIncomingEdgeFrom(graph_node *gnode, graph_node *from);
void ScanSymbolTable(SgFile *f);
void NoBodySubprograms();
void DeleteOutcomingEdgeTo(graph_node *gnode, graph_node *gto);
int isNoBodyNode(graph_node *gnode);
void ReseatEdges(graph_node *gnode, graph_node *newnode);
graph_node *SplittingNode(graph_node *gnode);
graph_node *CloneNode(graph_node *gnode);
void CopyOutcomingEdges(graph_node *gnode, graph_node *gnew);
void CopyIncomingEdges (graph_node *gnode, graph_node *gnew);
void RemovingUninlinedEdges();
void Partition();
void MoveEdgesPointTo(graph_node *gnode);
int unvisited_in(graph_node_list *interval);
int inInterval(graph_node *gnode,graph_node_list *interval);
int allPredecessorInInterval(graph_node *gnode,graph_node_list *interval);
void ReseatEdgesOutsideToNew(graph_node *gnode, graph_node *gnew,graph_node_list *interval);
void initIntrinsicNames();


/*  hlp.cpp */
SgLabel *  firstLabel(SgFile *f);
int isLabel(int num) ;
SgLabel * GetLabel();
SgLabel * GetNewLabel();
SgLabel * NewLabel();
//SgLabel * NewLabel(int lnum);
const char* header(int i);
char *UnparseExpr(SgExpression *e) ;
void printVariantName(int i);
void Error(const char *s, const char *t, int num, SgStatement *stmt);
void err(const char *s, int num, SgStatement *stmt);
void Err_g(const char *s, const char *t, int num);
void Warning(const char *s, const char *t, int num, SgStatement *stmt);
void warn(const char *s, int num, SgStatement *stmt);
void Warn_g(const char *s, const char *t, int num);
void errN(const char *s, int num, SgStatement *stmt);
void format_num (int num, char num3s[]);
SgExpression *ConnectList(SgExpression *el1, SgExpression *el2);
int is_integer_value(char *str);
void PrintSymbolTable(SgFile *f);
void printSymb(SgSymbol *s);
void printType(SgType *t);
void PrintTypeTable(SgFile *f);
int isSymbolNameInScope(char *name, SgStatement *scope);
int isSymbolName(char *name);
SgExpression *ReplaceIntegerParameter(SgExpression *e);
void SetScopeOfLabel(SgLabel *lab, SgStatement *scope);
SgLabel *isLabelWithScope(int num, SgStatement *stmt) ;
SgExpression *UpperBound(SgSymbol *ar, int i);
SgExpression *LowerBound(SgSymbol *ar, int i);
int Rank (SgSymbol *s);
symb_list  *AddToSymbList ( symb_list *ls, SgSymbol *s);
void MakeDeclarationForTempVarsInTop();
SgExpression *Calculate(SgExpression *er);
int ExpCompare(SgExpression *e1, SgExpression *e2);
SgExpression *Calculate_List(SgExpression *e);


/* inliner.cpp  */
void Inliner(graph_node *gtop);
void EntryPointList(SgFile *file);
void IntegerConstantSubstitution(SgStatement *header);
int isIntrinsicFunctionName(char *name);
char *ChangeIntrinsicFunctionName(char *name);
void RoutineCleaning(SgStatement *header);
void StatementCleaning(SgStatement *stmt);
SgSymbol *SearchFunction(SgExpression *e,SgStatement *stmt);
SgSymbol *PrecalculateFtoVar(SgExpression *e,SgStatement *stmt);
void PrecalculateActualParameters(SgSymbol *s,SgExpression *e,SgStatement *stmt);
void PrecalculateExpression(SgSymbol *sp,SgExpression *e,SgStatement *stmt);
void InsertNewStatementBefore (SgStatement *stat, SgStatement *current);
void InsertNewStatementAfter (SgStatement *stat, SgStatement *current, SgStatement *cp);
int ParameterType(SgExpression *e,SgStatement *stmt);
int TestSubscripts(SgExpression *e,SgStatement *stmt);
int TestRange(SgExpression *e,SgStatement *stmt);
SgSymbol *GetTempVarForF(SgSymbol *sf, SgType *t);
SgSymbol *GetTempVarForArg(int i, SgSymbol *sf, SgType *t);
SgSymbol *GetTempVarForSubscr(SgType *t);
SgSymbol *GetTempVarForBound(SgSymbol *sa);
SgStatement *InlineExpansion(graph_node *gtop, SgStatement *stmt, SgSymbol *sf, SgExpression *args);
int isInSymbolTable(SgSymbol *sym);
SgStatement * CreateTemplate(graph_node *gnode);
void SiteIndependentTransformation(graph_node *gnode); //(SgStatement *header);
void MoveToTopOfRoutine(SgStatement *entrystmt, SgStatement *first_executable);
void LogIf_to_IfThen(SgStatement *stmt);
void MoveToTopOfRoutine(SgStatement *entrystmt, SgStatement *first_executable);
SgStatement *ReplaceByGoToBottomOfRoutine(SgStatement *retstmt, SgLabel *lab_return);
void MoveFormatToTopOfRoutine(SgStatement *format_stmt, SgStatement *last_declaration);
int TestFormatLabel(SgLabel *lab);
int isInlinedCall(graph_node *gtop, graph_node *gnode);
void ReplaceReturnByContinue(SgStatement *return_st);
SgStatement *MoveFormatIntoTopLevel(SgStatement *format_stmt, int clone);
graph_node *getNodeForSymbol(graph_node *gtop,char *name);
int isInlinedCallSite(SgStatement *stmt);
graph_node *getAttrNodeForSymbol(SgSymbol *sf);
label_list  *addToLabelList(label_list *lablist, SgLabel *lab);
int isInLabelList(SgLabel *lab, label_list *lablist);
void ReplaceFormatLabelsInStmts(SgStatement *header);
int isLabelOfTop(SgLabel *lab);
void LabelList(SgStatement *header);
SgLabel *isInFormatMap(SgLabel *lab);
void SetScopeToLabels(SgStatement *header);
void AdjustableArrayBounds(SgStatement *header, SgStatement *after);
int isAdustableBound(SgExpression *bound);
int SearchVarRef(SgExpression *e);
void PrecalculateArrayBound(SgSymbol *ar,SgExpression *bound, SgStatement *after, SgStatement *header);
void ReplaceWholeArrayRefInIOStmts(SgStatement *header);
SgExpression *ImplicitLoop(SgSymbol *ar);
SgSymbol *GetImplicitDoVar(int j);
SgExpression * LowerLoopBound(SgSymbol *ar, int i);
SgExpression * UpperLoopBound(SgSymbol *ar, int i);
void RemapLocalVariables(SgStatement *header);
SgSymbol *CreateListOfLocalVariables(SgStatement *header);
void MakeDeclarationStmtInTop(SgSymbol *s);
SgSymbol *NextSymbol(SgSymbol *s);
SgSymbol *GetNewTopSymbol(SgSymbol *s);
int isInTopSymbList(SgSymbol *sym);
SgSymbol *GetImplicitDoVar(int j);
char *NewName(char *name);
SgSymbol *isTopName(char *name);
SgSymbol *isTopNameOfType(char *name, SgType *type);
void ReplaceIntegerParameterInTypeOfVars(SgStatement *header, SgStatement *last);
void ReplaceIntegerParameter_InType(SgType *t);
void MakeDeclarationStmtsForConstant(SgSymbol *s);
void RemapFunctionResultVar(SgExpression *topref, SgSymbol *sf);
SgStatement *TranslateSubprogramReferences(SgStatement *header);
//void TranslateExpression(SgExpression * e, int md[]);
SgExpression *TranslateExpression(SgExpression * e, int *md);
SgSymbol *SymbolMap(SgSymbol *s);
void InsertBlockAfter(SgStatement *after, SgStatement *first, SgStatement *last);
void ExtractSubprogramsOfCallGraph(graph_node *gtop);
int CompareConstants(SgSymbol *rs, SgSymbol *ts);
void RemapConstants(SgStatement *header,SgStatement *first_exec);
void RemapLocalObject(SgSymbol *s);
void CommonBlockList(SgStatement *stmt);
void TopCommonBlockList(SgStatement *stmt);
block_list *AddToBlockList(block_list *blist_last, SgExpression *eb);
void EquivBlockList(SgStatement *stmt);
void TranslateExpression_1(SgExpression *e);
void TranslateExpressionList(SgExpression *e) ;
SgStatement *DeclaringCommonBlock(SgExpression *bl);
void RemapCommonBlocks(SgStatement *header);
int isUnconflictingCommon(SgSymbol *s);
block_list *isConflictingCommon(SgSymbol *s);
SgType *BaseType(SgType *type);
block_list *isInCommonList(SgSymbol *s, block_list *blc );
int areOfSameType(SgSymbol *st, SgSymbol *sr);
int IntrinsicTypeSize(SgType *t);
int TypeSize(SgType *t);
int TypeLength(SgType *t);
void MakeRefsConformable(SgExpression *tref, SgExpression *ref);
void CalculateTopLevelRef(SgSymbol *tops,SgExpression *tref, SgExpression *ref);
void CreateTopCommonBlockList();
void RemapCommonObject(SgSymbol *s,SgSymbol *tops);
void RemapCommonList(SgExpression *el);
int CompareValues(PTR_LLND pe1,PTR_LLND pe2);
SgType * TypeOfResult(SgExpression *e); 
int is_IntrinsicFunction(SgSymbol *sf);
int IntrinsicInd(SgSymbol *sf);
SgType *TypeF(int indf,SgExpression *e);
SgType * SgTypeComplex(SgFile *f);
SgType * SgTypeDoubleComplex(SgFile *f);
void ConformActualAndFormalParameters(SgSymbol *scopy,SgExpression *args,SgStatement *parentSt);
SgSymbol *FirstDummy(SgSymbol *sf);
SgSymbol *NextDummy(SgSymbol *s);
int TestConformability(SgSymbol *darg, SgExpression *fact, SgStatement *parentSt);
int isScalar(SgSymbol *symb);
int SameType(SgSymbol *darg, SgExpression *fact);
int Same(SgType *ft,SgType *dt);
int isArray(SgSymbol *symb);
int TestShapes(SgArrayType *ftp, SgArrayType *dtp);
SgExpression *LowerBoundOfDim(SgExpression *e);
SgExpression *UpperBoundOfDim(SgExpression *e);
int IdenticalValues(SgExpression *e1, SgExpression *e2);
SgExpression *ArrayMap(SgSymbol *s);
//SgExpression *ArrayMap1(SgSymbol *s);
SgExpression *ArrayMap2(SgSymbol *s);
SgExpression *FirstIndexChange(SgExpression *e, SgExpression *index);
int SameShapes(SgArrayType *ftp, SgArrayType *dtp);
int is_NoExpansionFunction(SgSymbol *sf);
int isFormalProcedure(SgSymbol *symb);
int SameDims(SgExpression *fe,SgExpression *de); 
SgExpression *FirstIndexesChange(SgExpression *mape, SgExpression *re);
void ConformReferences(SgSymbol *darg, SgExpression *fact, SgStatement *parentSt);
void TranslateArrayTypeExpressions(SgSymbol *darg);
int isAdjustableArray(SgSymbol *param);
int TestBounds(SgExpression *fact, SgArrayType *ftp, SgArrayType *dtp);
void TransformForFortran77();
SgExpression *IndexChange(SgExpression *e, SgExpression *index, SgExpression *lbe);
int TestVector(SgExpression *fact, SgArrayType *ftp, SgArrayType *dtp);
SgType *TypeOfArgument(SgExpression *e);
void ReplaceContext(SgStatement *stmt);
int isDoEndStmt(SgStatement *stmt);
void ReplaceDoNestLabel(SgStatement *last_st, SgLabel *new_lab);
void EditExpressionList(SgExpression *e);
void Add_Comment(graph_node *g, SgStatement *stmt, int flag);
void PrintTopSymbList();
void PrintSymbList(SgSymbol *slist, SgStatement *header);

/*  driver.cpp */

//-----------------------------------------------------------------------

extern "C" char* funparse_bfnd(...);
extern "C" char* Tool_Unparse2_LLnode(...);
extern "C" void Init_Unparser(...);

//-----------------------------------------------------------------------
//extern SgLabel * LabelMapping(PTR_LABEL label);
